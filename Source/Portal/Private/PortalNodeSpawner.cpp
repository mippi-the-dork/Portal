// Copyright Mippithedork 2026, Inc. All Rights Reserved.

#include "PortalNodeSpawner.h"

#include "BlueprintActionFilter.h"
#include "EdGraph/EdGraph.h"
#include "EdGraph/EdGraphPin.h"
#include "EdGraphSchema_K2.h"
#include "K2Node_Knot.h"
#include "K2Node_PortalDeclaration.h"
#include "K2Node_PortalUsage.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "PortalNativeUtils.h"

#define LOCTEXT_NAMESPACE "PortalNodeSpawner"

namespace
{
    bool IsK2Graph(const UEdGraph* Graph)
    {
        return Graph && Graph->GetSchema() && Graph->GetSchema()->IsA<UEdGraphSchema_K2>();
    }

    FString GetPortalTypeLabel(const UK2Node_Knot* Input)
    {
        if (!Input || !Input->GetInputPin())
        {
            return TEXT("Wildcard");
        }

        const FEdGraphPinType& PinType = Input->GetInputPin()->PinType;
        if (PinType.PinCategory == UEdGraphSchema_K2::PC_Wildcard)
        {
            return TEXT("Wildcard");
        }

        if (UObject* SubCategoryObject = PinType.PinSubCategoryObject.Get())
        {
            return SubCategoryObject->GetName();
        }

        FString Label = PinType.PinCategory.ToString();
        if (!Label.IsEmpty())
        {
            Label[0] = FChar::ToUpper(Label[0]);
        }
        return Label.IsEmpty() ? TEXT("Data") : Label;
    }
}

UPortalNodeSpawner* UPortalNodeSpawner::CreateInputSpawner()
{
    UPortalNodeSpawner* Spawner = NewObject<UPortalNodeSpawner>(GetTransientPackage());
    // GetMenuActions() is running for UK2Node_PortalDeclaration, so UE requires
    // the registered spawner to advertise that same class. Invoke() immediately
    // migrates the temporary legacy node to Portal's native UK2Node_Knot backing.
    Spawner->NodeClass = UK2Node_PortalDeclaration::StaticClass();
    Spawner->SpawnKind = EPortalNodeSpawnKind::Input;
    Spawner->ConfigureCommonUi();
    Spawner->DefaultMenuSignature.MenuName = LOCTEXT("CreatePortalRerouteNode", "Create Portal Reroute Node");
    Spawner->DefaultMenuSignature.Tooltip = LOCTEXT("CreatePortalRerouteNodeTooltip", "Create a new named data reroute Portal Input backed by a native Blueprint reroute node.");
    Spawner->DefaultMenuSignature.Keywords = LOCTEXT("CreatePortalRerouteNodeKeywords", "portal reroute named reroute namedreroute input create add route data");
    return Spawner;
}

UPortalNodeSpawner* UPortalNodeSpawner::CreateOutputSpawner(UK2Node_Knot* PortalInput)
{
    if (!PortalInput || PortalNativeUtils::GetRole(PortalInput) != EPortalKnotRole::Declaration || !PortalInput->GetGraph())
    {
        return nullptr;
    }

    FPortalKnotData Data;
    if (!PortalNativeUtils::ReadPortalData(PortalInput, Data) || !Data.FamilyGuid.IsValid())
    {
        return nullptr;
    }

    UPortalNodeSpawner* Spawner = NewObject<UPortalNodeSpawner>(GetTransientPackage());
    // Output actions are registered from UK2Node_PortalUsage::GetMenuActions(),
    // so their advertised node class must match that generating class. Invoke()
    // converts the temporary Usage into a native reroute before returning.
    Spawner->NodeClass = UK2Node_PortalUsage::StaticClass();
    Spawner->SpawnKind = EPortalNodeSpawnKind::Output;
    Spawner->TargetGraph = PortalInput->GetGraph();
    Spawner->FamilyGuid = Data.FamilyGuid;
    Spawner->ConfigureCommonUi();

    const FText NameText = FText::FromName(Data.Name.IsNone() ? FName(TEXT("Portal")) : Data.Name);
    const FString TypeLabel = GetPortalTypeLabel(PortalInput);
    TArray<UK2Node_Knot*> ExistingOutputs;
    PortalNativeUtils::GetUsages(PortalInput, ExistingOutputs);

    Spawner->DefaultMenuSignature.Category = LOCTEXT("PortalOutputsCategory", "Portal|Outputs");
    Spawner->DefaultMenuSignature.MenuName = FText::Format(LOCTEXT("CreatePortalOutputFor", "Portal Output: {0}"), NameText);
    Spawner->DefaultMenuSignature.Tooltip = FText::Format(
        LOCTEXT("CreatePortalOutputForTooltip", "Create an Output for Portal Input '{0}'. Type: {1}. Existing Outputs: {2}."),
        NameText,
        FText::FromString(TypeLabel),
        FText::AsNumber(ExistingOutputs.Num()));
    Spawner->DefaultMenuSignature.Keywords = FText::Format(
        LOCTEXT("CreatePortalOutputForKeywords", "portal output named reroute namedreroute get getter route {0} {1}"),
        NameText,
        FText::FromString(TypeLabel));
    return Spawner;
}

void UPortalNodeSpawner::ConfigureCommonUi()
{
    DefaultMenuSignature.Category = LOCTEXT("PortalCategory", "Portal");
}

UEdGraphNode* UPortalNodeSpawner::Invoke(UEdGraph* ParentGraph, FBindingSet const& Bindings, FVector2D const Location) const
{
    if (!IsK2Graph(ParentGraph))
    {
        return nullptr;
    }

    // UE's Blueprint action registrar requires GetMenuActions() to register only
    // spawners whose NodeClass matches the class currently generating actions.
    // Portal therefore lets the normal spawner create a temporary legacy Portal
    // node, then immediately migrates that placeholder to the native UK2Node_Knot
    // representation before returning from the action. Nothing Portal-specific is
    // left serialized in the Blueprint.
    TSet<const UK2Node_Knot*> ExistingNativeKnots;
    for (UEdGraphNode* Node : ParentGraph->Nodes)
    {
        if (const UK2Node_Knot* Knot = Cast<UK2Node_Knot>(Node))
        {
            ExistingNativeKnots.Add(Knot);
        }
    }

    UEdGraphNode* Placeholder = Super::Invoke(ParentGraph, Bindings, Location);
    if (!Placeholder)
    {
        return nullptr;
    }

    // Transient/template graphs are never user assets. Keep the matching temporary
    // node class there and avoid metadata writes or migration.
    if (ParentGraph->GetOutermost() == GetTransientPackage() || ParentGraph->HasAnyFlags(RF_Transient))
    {
        return Placeholder;
    }

    FGuid ExpectedFamilyGuid;
    FVector2f PlaceholderPosition(
        static_cast<float>(Placeholder->NodePosX),
        static_cast<float>(Placeholder->NodePosY));

    if (SpawnKind == EPortalNodeSpawnKind::Input)
    {
        UK2Node_PortalDeclaration* LegacyInput = Cast<UK2Node_PortalDeclaration>(Placeholder);
        if (!LegacyInput)
        {
            ParentGraph->RemoveNode(Placeholder);
            ParentGraph->NotifyGraphChanged();
            return nullptr;
        }

        ExpectedFamilyGuid = LegacyInput->PortalGuid;
    }
    else
    {
        UK2Node_PortalUsage* LegacyOutput = Cast<UK2Node_PortalUsage>(Placeholder);
        UK2Node_Knot* NativeInput = PortalNativeUtils::FindDeclaration(ParentGraph, FamilyGuid);
        FPortalKnotData InputData;
        if (!LegacyOutput || !NativeInput || !PortalNativeUtils::ReadPortalData(NativeInput, InputData))
        {
            ParentGraph->RemoveNode(Placeholder);
            ParentGraph->NotifyGraphChanged();
            return nullptr;
        }

        // Seed the temporary Usage with the native family's identity. The migration
        // path copies this presentation/identity data, and RepairGraph restores the
        // invisible native bridge to the existing Input.
        LegacyOutput->Declaration = nullptr;
        LegacyOutput->DeclarationGuid = InputData.FamilyGuid;
        LegacyOutput->DeclarationGraphGuid = ParentGraph->GraphGuid;
        LegacyOutput->CachedPortalName = InputData.Name;
        LegacyOutput->CachedPortalColor = InputData.Color;
        ExpectedFamilyGuid = InputData.FamilyGuid;
    }

    if (!PortalNativeUtils::MigrateLegacyGraph(ParentGraph))
    {
        return nullptr;
    }

    PortalNativeUtils::RepairGraph(ParentGraph);

    UK2Node_Knot* Result = nullptr;
    float BestDistanceSquared = TNumericLimits<float>::Max();
    for (UEdGraphNode* Node : ParentGraph->Nodes)
    {
        UK2Node_Knot* Knot = Cast<UK2Node_Knot>(Node);
        if (!Knot || ExistingNativeKnots.Contains(Knot))
        {
            continue;
        }

        FPortalKnotData Data;
        if (!PortalNativeUtils::ReadPortalData(Knot, Data) || Data.FamilyGuid != ExpectedFamilyGuid)
        {
            continue;
        }

        const EPortalKnotRole ExpectedRole = SpawnKind == EPortalNodeSpawnKind::Input
            ? EPortalKnotRole::Declaration
            : EPortalKnotRole::Usage;
        if (Data.Role != ExpectedRole)
        {
            continue;
        }

        const FVector2f Delta(
            static_cast<float>(Knot->NodePosX) - PlaceholderPosition.X,
            static_cast<float>(Knot->NodePosY) - PlaceholderPosition.Y);
        const float DistanceSquared = Delta.SizeSquared();
        if (!Result || DistanceSquared < BestDistanceSquared)
        {
            Result = Knot;
            BestDistanceSquared = DistanceSquared;
        }
    }

    if (!Result)
    {
        return nullptr;
    }

    if (SpawnKind == EPortalNodeSpawnKind::Input)
    {
        FKismetEditorUtilities::BringKismetToFocusAttentionOnObject(Result, true);
    }

    return Result;
}

bool UPortalNodeSpawner::IsTemplateNodeFilteredOut(FBlueprintActionFilter const& Filter) const
{
    if (Super::IsTemplateNodeFilteredOut(Filter))
    {
        return true;
    }

    if (Filter.Context.Graphs.IsEmpty())
    {
        return true;
    }

    if (SpawnKind == EPortalNodeSpawnKind::Input)
    {
        bool bHasK2Graph = false;
        for (const UEdGraph* Graph : Filter.Context.Graphs)
        {
            bHasK2Graph |= IsK2Graph(Graph);
        }
        if (!bHasK2Graph)
        {
            return true;
        }

        // When the action menu was opened from a dragged pin, a Portal Input only makes
        // sense from a data output pin. Normal empty-graph search has no Context.Pins.
        for (const UEdGraphPin* Pin : Filter.Context.Pins)
        {
            if (!PortalNativeUtils::CanCreatePortalFromOutputPin(Pin))
            {
                return true;
            }
        }
        return false;
    }

    UEdGraph* Graph = TargetGraph.Get();
    if (!IsK2Graph(Graph))
    {
        return true;
    }

    bool bGraphMatchesContext = false;
    for (const UEdGraph* ContextGraph : Filter.Context.Graphs)
    {
        if (ContextGraph == Graph)
        {
            bGraphMatchesContext = true;
            break;
        }
    }
    if (!bGraphMatchesContext)
    {
        return true;
    }

    UK2Node_Knot* Input = PortalNativeUtils::FindDeclaration(Graph, FamilyGuid);
    if (!Input || PortalNativeUtils::GetRole(Input) != EPortalKnotRole::Declaration)
    {
        return true;
    }

    // An Output action can also participate in pin-drag search, but only when the
    // destination is an unconnected, compatible data input pin.
    for (const UEdGraphPin* Pin : Filter.Context.Pins)
    {
        if (!PortalNativeUtils::CanConnectPortalToInputPin(Input, Pin))
        {
            return true;
        }
    }

    return false;
}

#undef LOCTEXT_NAMESPACE
