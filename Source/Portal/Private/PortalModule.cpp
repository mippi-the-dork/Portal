// Copyright Mippithedork 2026, Inc. All Rights Reserved.

#include "PortalModule.h"

#include "EdGraph/EdGraph.h"
#include "EdGraph/EdGraphNode.h"
#include "EdGraph/EdGraphPin.h"
#include "Engine/Blueprint.h"
#include "BlueprintActionDatabase.h"
#include "EdGraphSchema_K2.h"
#include "EdGraphUtilities.h"
#include "Framework/Application/SlateApplication.h"
#include "Framework/Commands/UIAction.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "GraphEditorModule.h"
#include "K2Node_Knot.h"
#include "K2Node_PortalDeclaration.h"
#include "K2Node_PortalUsage.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Layout/WidgetPath.h"
#include "PortalInputProcessor.h"
#include "PortalNativeUtils.h"
#include "PortalNodeFactory.h"
#include "SGraphPanel.h"
#include "ToolMenu.h"
#include "ToolMenuSection.h"
#include "ToolMenus.h"
#include "UObject/UObjectIterator.h"
#include "UObject/UObjectGlobals.h"

#define LOCTEXT_NAMESPACE "FPortalModule"

namespace
{
    UK2Node_Knot* GetContextKnot(FToolMenuSection& Section)
    {
        const UGraphNodeContextMenuContext* Context = Section.FindContext<UGraphNodeContextMenuContext>();
        if (!Context || Context->bIsDebugging || !Context->Node)
        {
            return nullptr;
        }

        return Cast<UK2Node_Knot>(const_cast<UEdGraphNode*>(Context->Node.Get()));
    }

    bool IsDataPortalInput(const UK2Node_Knot* Knot)
    {
        if (!Knot || PortalNativeUtils::GetRole(Knot) != EPortalKnotRole::Declaration)
        {
            return false;
        }

        const UEdGraphPin* InputPin = Knot->GetInputPin();
        const UEdGraphPin* OutputPin = Knot->GetOutputPin();
        return (!InputPin || InputPin->PinType.PinCategory != UEdGraphSchema_K2::PC_Exec) &&
               (!OutputPin || OutputPin->PinType.PinCategory != UEdGraphSchema_K2::PC_Exec);
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

    void GatherPortalInputs(const UEdGraph* Graph, TArray<UK2Node_Knot*>& OutInputs)
    {
        OutInputs.Reset();
        if (!Graph)
        {
            return;
        }

        for (UEdGraphNode* Node : Graph->Nodes)
        {
            UK2Node_Knot* Knot = Cast<UK2Node_Knot>(Node);
            if (IsDataPortalInput(Knot))
            {
                OutInputs.Add(Knot);
            }
        }

        OutInputs.Sort([](const UK2Node_Knot& A, const UK2Node_Knot& B)
        {
            return PortalNativeUtils::GetPortalName(&A).ToString() < PortalNativeUtils::GetPortalName(&B).ToString();
        });
    }

    void PopulatePortalOutputToolSubMenu(UToolMenu* SubMenu, TWeakObjectPtr<UEdGraph> WeakGraph, FVector2f SpawnPosition)
    {
        UEdGraph* Graph = WeakGraph.Get();
        if (!SubMenu || !Graph)
        {
            return;
        }

        FToolMenuSection& OutputSection = SubMenu->FindOrAddSection(TEXT("PortalOutputs"));
        TArray<UK2Node_Knot*> Inputs;
        GatherPortalInputs(Graph, Inputs);

        for (UK2Node_Knot* Input : Inputs)
        {
            if (!Input)
            {
                continue;
            }

            TArray<UK2Node_Knot*> Outputs;
            PortalNativeUtils::GetUsages(Input, Outputs);
            const FName PortalName = PortalNativeUtils::GetPortalName(Input);
            const FString TypeLabel = GetPortalTypeLabel(Input);
            const FText Label = FText::Format(
                LOCTEXT("PortalOutputMenuFormat", "{0}   [{1}]   {2}"),
                FText::FromName(PortalName),
                FText::FromString(TypeLabel),
                FText::AsNumber(Outputs.Num()));
            const FText Tooltip = FText::Format(
                LOCTEXT("PortalOutputMenuTooltip", "Create an Output for Portal Input '{0}' at the clicked location."),
                FText::FromName(PortalName));

            TWeakObjectPtr<UK2Node_Knot> WeakInput(Input);
            const FName EntryName(*FString::Printf(TEXT("PortalOutput_%s"), *Input->NodeGuid.ToString(EGuidFormats::Digits)));
            OutputSection.AddMenuEntry(
                EntryName,
                Label,
                Tooltip,
                FSlateIcon(),
                FUIAction(FExecuteAction::CreateLambda([WeakInput, SpawnPosition]()
                {
                    if (UK2Node_Knot* LiveInput = WeakInput.Get())
                    {
                        if (UK2Node_Knot* Output = PortalNativeUtils::CreateUsage(LiveInput, &SpawnPosition))
                        {
                            FKismetEditorUtilities::BringKismetToFocusAttentionOnObject(Output, false);
                        }
                    }
                })));
        }
    }

    void PopulatePortalInputPinSubMenu(FMenuBuilder& MenuBuilder, TWeakObjectPtr<UEdGraphNode> WeakOwner, FGuid PinId, const FVector2f* OptionalPinPosition)
    {
        UEdGraphNode* Owner = WeakOwner.Get();
        if (!Owner || !Owner->GetGraph())
        {
            return;
        }

        UEdGraphPin* TargetPin = nullptr;
        for (UEdGraphPin* Candidate : Owner->Pins)
        {
            if (Candidate && Candidate->PinId == PinId)
            {
                TargetPin = Candidate;
                break;
            }
        }
        if (!TargetPin)
        {
            return;
        }

        TArray<UK2Node_Knot*> Inputs;
        GatherPortalInputs(Owner->GetGraph(), Inputs);
        for (UK2Node_Knot* Input : Inputs)
        {
            if (!PortalNativeUtils::CanConnectPortalToInputPin(Input, TargetPin))
            {
                continue;
            }

            TArray<UK2Node_Knot*> Outputs;
            PortalNativeUtils::GetUsages(Input, Outputs);
            const FName PortalName = PortalNativeUtils::GetPortalName(Input);
            const FString TypeLabel = GetPortalTypeLabel(Input);
            const FText Label = FText::Format(
                LOCTEXT("ConnectPortalOutputMenuFormat", "{0}   [{1}]   {2}"),
                FText::FromName(PortalName),
                FText::FromString(TypeLabel),
                FText::AsNumber(Outputs.Num()));

            TWeakObjectPtr<UK2Node_Knot> WeakInput(Input);
            MenuBuilder.AddMenuEntry(
                Label,
                FText::Format(
                    LOCTEXT("ConnectPortalOutputMenuTooltip", "Create and connect an Output from Portal Input '{0}'."),
                    FText::FromName(PortalName)),
                FSlateIcon(),
                FUIAction(FExecuteAction::CreateLambda([WeakInput, WeakOwner, PinId, PinGraphPosition = OptionalPinPosition ? *OptionalPinPosition : FVector2f::ZeroVector, bHasPinGraphPosition = OptionalPinPosition != nullptr]()
                {
                    UK2Node_Knot* LiveInput = WeakInput.Get();
                    UEdGraphNode* LiveOwner = WeakOwner.Get();
                    if (!LiveInput || !LiveOwner)
                    {
                        return;
                    }

                    UEdGraphPin* LiveTargetPin = nullptr;
                    for (UEdGraphPin* Candidate : LiveOwner->Pins)
                    {
                        if (Candidate && Candidate->PinId == PinId)
                        {
                            LiveTargetPin = Candidate;
                            break;
                        }
                    }

                    if (UK2Node_Knot* Output = PortalNativeUtils::CreateUsageAndConnectToInputPin(LiveInput, LiveTargetPin, bHasPinGraphPosition ? &PinGraphPosition : nullptr))
                    {
                        FKismetEditorUtilities::BringKismetToFocusAttentionOnObject(Output, false);
                    }
                })));
        }

    }

    void PopulatePortalRetargetToolSubMenu(UToolMenu* SubMenu, TWeakObjectPtr<UK2Node_Knot> WeakOutput)
    {
        UK2Node_Knot* Output = WeakOutput.Get();
        if (!SubMenu || !Output || !Output->GetGraph())
        {
            return;
        }

        FToolMenuSection& RetargetSection = SubMenu->FindOrAddSection(TEXT("PortalRetargetOutputs"));
        TArray<UK2Node_Knot*> Inputs;
        GatherPortalInputs(Output->GetGraph(), Inputs);

        for (UK2Node_Knot* Input : Inputs)
        {
            if (!PortalNativeUtils::CanRetargetUsageToDeclaration(Output, Input))
            {
                continue;
            }

            TArray<UK2Node_Knot*> Outputs;
            PortalNativeUtils::GetUsages(Input, Outputs);
            const FName PortalName = PortalNativeUtils::GetPortalName(Input);
            const FString TypeLabel = GetPortalTypeLabel(Input);
            const FText Label = FText::Format(
                LOCTEXT("PortalRetargetMenuFormat", "{0}   [{1}]   {2}"),
                FText::FromName(PortalName),
                FText::FromString(TypeLabel),
                FText::AsNumber(Outputs.Num()));

            TWeakObjectPtr<UK2Node_Knot> WeakInput(Input);
            const FName EntryName(*FString::Printf(TEXT("PortalRetarget_%s"), *Input->NodeGuid.ToString(EGuidFormats::Digits)));
            RetargetSection.AddMenuEntry(
                EntryName,
                Label,
                FText::Format(
                    LOCTEXT("PortalRetargetMenuTooltip", "Connect this Portal Output to Input '{0}'. Existing downstream connections are preserved."),
                    FText::FromName(PortalName)),
                FSlateIcon(),
                FUIAction(FExecuteAction::CreateLambda([WeakOutput, WeakInput]()
                {
                    if (UK2Node_Knot* LiveOutput = WeakOutput.Get())
                    {
                        if (UK2Node_Knot* LiveInput = WeakInput.Get())
                        {
                            PortalNativeUtils::RetargetUsage(LiveOutput, LiveInput);
                        }
                    }
                })));
        }
    }

    bool HasCompatibleRetargetInput(const UK2Node_Knot* Output)
    {
        if (!Output || !Output->GetGraph())
        {
            return false;
        }

        TArray<UK2Node_Knot*> Inputs;
        GatherPortalInputs(Output->GetGraph(), Inputs);
        for (UK2Node_Knot* Input : Inputs)
        {
            if (PortalNativeUtils::CanRetargetUsageToDeclaration(Output, Input))
            {
                return true;
            }
        }
        return false;
    }

    bool GetCachedGraphClickPosition(const UEdGraph* Graph, FVector2f& OutPosition)
    {
        const UEdGraph* CachedGraph = nullptr;
        if (!FPortalInputProcessor::GetLastGraphRightClick(CachedGraph, OutPosition))
        {
            return false;
        }
        return CachedGraph == Graph;
    }

    TSharedRef<FExtender> ExtendPortalPinContextMenu(
        const TSharedRef<FUICommandList> CommandList,
        const UEdGraph* Graph,
        const UEdGraphNode* Node,
        const UEdGraphPin* Pin,
        bool bIsConst)
    {
        TSharedRef<FExtender> Extender = MakeShared<FExtender>();
        if (bIsConst || !Graph || !Graph->GetSchema() || !Graph->GetSchema()->IsA<UEdGraphSchema_K2>())
        {
            return Extender;
        }

        if (Pin && PortalNativeUtils::CanCreatePortalFromOutputPin(Pin))
        {
            TWeakObjectPtr<UEdGraphNode> WeakOwner(Pin->GetOwningNode());
            const FGuid PinId = Pin->PinId;
            FVector2f PinGraphPosition = FVector2f::ZeroVector;
            const bool bHasPinGraphPosition = GetCachedGraphClickPosition(Graph, PinGraphPosition);
            Extender->AddMenuExtension(
                TEXT("EdGraphSchemaPinActions"),
                EExtensionHook::Before,
                CommandList,
                FMenuExtensionDelegate::CreateLambda([WeakOwner, PinId, PinGraphPosition, bHasPinGraphPosition](FMenuBuilder& MenuBuilder)
                {
                    MenuBuilder.BeginSection(TEXT("PortalPinActions"), LOCTEXT("PortalPinActionsSection", "Portal"));
                    MenuBuilder.AddMenuEntry(
                        LOCTEXT("CreatePortalFromPinLabel", "Create Portal"),
                        LOCTEXT("CreatePortalFromPinTooltip", "Creates a Portal Input from this data output pin and connects it automatically."),
                        FSlateIcon(),
                        FUIAction(FExecuteAction::CreateLambda([WeakOwner, PinId, PinGraphPosition, bHasPinGraphPosition]()
                        {
                            UEdGraphNode* Owner = WeakOwner.Get();
                            if (!Owner)
                            {
                                return;
                            }

                            UEdGraphPin* CurrentPin = nullptr;
                            for (UEdGraphPin* Candidate : Owner->Pins)
                            {
                                if (Candidate && Candidate->PinId == PinId)
                                {
                                    CurrentPin = Candidate;
                                    break;
                                }
                            }

                            if (UK2Node_Knot* Input = PortalNativeUtils::CreatePortalFromOutputPin(CurrentPin, bHasPinGraphPosition ? &PinGraphPosition : nullptr))
                            {
                                FKismetEditorUtilities::BringKismetToFocusAttentionOnObject(Input, true);
                            }
                        })));
                    MenuBuilder.EndSection();
                }));
        }

        if (Pin && Pin->Direction == EGPD_Input && Pin->PinType.PinCategory != UEdGraphSchema_K2::PC_Exec && Pin->GetOwningNode())
        {
            TWeakObjectPtr<UEdGraphNode> WeakOwner(Pin->GetOwningNode());
            const FGuid PinId = Pin->PinId;
            FVector2f PinGraphPosition = FVector2f::ZeroVector;
            const bool bHasPinGraphPosition = GetCachedGraphClickPosition(Graph, PinGraphPosition);
            TArray<UK2Node_Knot*> Inputs;
            GatherPortalInputs(Graph, Inputs);

            bool bHasCompatibleInput = false;
            for (UK2Node_Knot* Input : Inputs)
            {
                if (PortalNativeUtils::CanConnectPortalToInputPin(Input, Pin))
                {
                    bHasCompatibleInput = true;
                    break;
                }
            }

            if (bHasCompatibleInput)
            {
                Extender->AddMenuExtension(
                    TEXT("EdGraphSchemaPinActions"),
                    EExtensionHook::Before,
                    CommandList,
                    FMenuExtensionDelegate::CreateLambda([WeakOwner, PinId, PinGraphPosition, bHasPinGraphPosition](FMenuBuilder& MenuBuilder)
                    {
                        MenuBuilder.BeginSection(TEXT("PortalInputPinActions"), LOCTEXT("PortalInputPinActionsSection", "Portal"));
                        MenuBuilder.AddSubMenu(
                            LOCTEXT("ConnectPortalOutputLabel", "Connect Portal Output"),
                            LOCTEXT("ConnectPortalOutputTooltip", "Create and connect an Output from an existing Portal Input."),
                            FNewMenuDelegate::CreateLambda([WeakOwner, PinId, PinGraphPosition, bHasPinGraphPosition](FMenuBuilder& SubMenuBuilder)
                            {
                                PopulatePortalInputPinSubMenu(SubMenuBuilder, WeakOwner, PinId, bHasPinGraphPosition ? &PinGraphPosition : nullptr);
                            }),
                            false,
                            FSlateIcon());
                        MenuBuilder.EndSection();
                    }));
            }
        }

        return Extender;
    }

    void AddPortalContextEntries(FToolMenuSection& Section)
    {
        UK2Node_Knot* Knot = GetContextKnot(Section);
        if (!Knot)
        {
            return;
        }

        const EPortalKnotRole Role = PortalNativeUtils::GetRole(Knot);
        TWeakObjectPtr<UK2Node_Knot> WeakKnot(Knot);

        if (Role == EPortalKnotRole::None)
        {
            if ((Knot->GetInputPin() && Knot->GetInputPin()->PinType.PinCategory == UEdGraphSchema_K2::PC_Exec) ||
                (Knot->GetOutputPin() && Knot->GetOutputPin()->PinType.PinCategory == UEdGraphSchema_K2::PC_Exec))
            {
                return;
            }

            Section.AddMenuEntry(
                TEXT("PortalConvertReroute"),
                LOCTEXT("PortalConvertRerouteLabel", "Convert to Portal Input"),
                LOCTEXT("PortalConvertRerouteTooltip", "Converts this native Blueprint reroute into a Portal Input. Existing outgoing branches become Portal Outputs."),
                FSlateIcon(),
                FUIAction(FExecuteAction::CreateLambda([WeakKnot]()
                {
                    if (UK2Node_Knot* Node = WeakKnot.Get())
                    {
                        PortalNativeUtils::ConvertRerouteToPortal(Node);
                    }
                }))
            );
            return;
        }

        if (Role == EPortalKnotRole::Declaration)
        {
            Section.AddMenuEntry(
                TEXT("PortalCreateUsage"),
                LOCTEXT("PortalCreateUsageLabel", "Create Output"),
                LOCTEXT("PortalCreateUsageTooltip", "Creates another Output for this Portal."),
                FSlateIcon(),
                FUIAction(FExecuteAction::CreateLambda([WeakKnot]()
                {
                    if (UK2Node_Knot* Node = WeakKnot.Get())
                    {
                        PortalNativeUtils::CreateUsage(Node);
                    }
                }))
            );

            Section.AddMenuEntry(
                TEXT("PortalDeleteFamily"),
                LOCTEXT("PortalDeleteFamilyLabel", "Delete Portal Family"),
                LOCTEXT("PortalDeleteFamilyTooltip", "Deletes this Portal Input and every Output in the family in one undoable operation."),
                FSlateIcon(),
                FUIAction(FExecuteAction::CreateLambda([WeakKnot]()
                {
                    if (UK2Node_Knot* Node = WeakKnot.Get())
                    {
                        PortalNativeUtils::DeletePortalFamily(Node);
                    }
                }))
            );
        }
        else
        {
            const bool bOrphan = PortalNativeUtils::IsOrphanUsage(Knot);

            if (bOrphan)
            {
                Section.AddMenuEntry(
                    TEXT("PortalCreateMissingInput"),
                    LOCTEXT("PortalCreateMissingInputLabel", "Create Missing Input"),
                    LOCTEXT("PortalCreateMissingInputTooltip", "Creates a new Portal Input for this orphaned Output. Other orphaned Outputs from the same family are repaired too."),
                    FSlateIcon(),
                    FUIAction(FExecuteAction::CreateLambda([WeakKnot]()
                    {
                        if (UK2Node_Knot* Node = WeakKnot.Get())
                        {
                            if (UK2Node_Knot* NewInput = PortalNativeUtils::CreateMissingInputForUsage(Node))
                            {
                                FKismetEditorUtilities::BringKismetToFocusAttentionOnObject(NewInput, true);
                            }
                        }
                    }))
                );
            }

            if (HasCompatibleRetargetInput(Knot))
            {
                Section.AddSubMenu(
                    TEXT("PortalRetargetOutput"),
                    bOrphan ? LOCTEXT("PortalReconnectOutputLabel", "Reconnect Output") : LOCTEXT("PortalChangeInputLabel", "Change Input"),
                    bOrphan ? LOCTEXT("PortalReconnectOutputTooltip", "Reconnect this orphaned Output to a compatible Portal Input in this graph.") : LOCTEXT("PortalChangeInputTooltip", "Move this Output to a different compatible Portal Input while preserving its downstream connections."),
                    FNewToolMenuDelegate::CreateLambda([WeakKnot](UToolMenu* SubMenu)
                    {
                        PopulatePortalRetargetToolSubMenu(SubMenu, WeakKnot);
                    }),
                    false,
                    FSlateIcon());
            }

            if (!bOrphan)
            {
                Section.AddMenuEntry(
                    TEXT("PortalJumpToDeclaration"),
                    LOCTEXT("PortalJumpToDeclarationLabel", "Jump to Input"),
                    LOCTEXT("PortalJumpToDeclarationTooltip", "Focuses this Output's Portal Input."),
                    FSlateIcon(),
                    FUIAction(
                        FExecuteAction::CreateLambda([WeakKnot]()
                        {
                            if (UK2Node_Knot* Node = WeakKnot.Get())
                            {
                                if (UK2Node_Knot* Declaration = PortalNativeUtils::FindDeclarationForUsage(Node, true))
                                {
                                    FKismetEditorUtilities::BringKismetToFocusAttentionOnObject(Declaration, false);
                                }
                            }
                        }),
                        FCanExecuteAction::CreateLambda([WeakKnot]()
                        {
                            UK2Node_Knot* Node = WeakKnot.Get();
                            return Node && PortalNativeUtils::FindDeclarationForUsage(Node, false) != nullptr;
                        })
                    )
                );

                Section.AddMenuEntry(
                    TEXT("PortalCreateAnotherUsage"),
                    LOCTEXT("PortalCreateAnotherUsageLabel", "Create Another Output"),
                    LOCTEXT("PortalCreateAnotherUsageTooltip", "Creates another Output for this Portal."),
                    FSlateIcon(),
                    FUIAction(
                        FExecuteAction::CreateLambda([WeakKnot]()
                        {
                            if (UK2Node_Knot* Node = WeakKnot.Get())
                            {
                                if (UK2Node_Knot* Declaration = PortalNativeUtils::FindDeclarationForUsage(Node, true))
                                {
                                    PortalNativeUtils::CreateUsage(Declaration);
                                }
                            }
                        }),
                        FCanExecuteAction::CreateLambda([WeakKnot]()
                        {
                            UK2Node_Knot* Node = WeakKnot.Get();
                            return Node && PortalNativeUtils::FindDeclarationForUsage(Node, false) != nullptr;
                        })
                    )
                );

            }

            if (bOrphan)
            {
                Section.AddMenuEntry(
                    TEXT("PortalConvertOrphanToReroute"),
                    LOCTEXT("PortalConvertOrphanToRerouteLabel", "Convert Output to Reroute"),
                    LOCTEXT("PortalConvertOrphanToRerouteTooltip", "Remove Portal metadata from this orphaned Output and leave it as a native Blueprint reroute."),
                    FSlateIcon(),
                    FUIAction(FExecuteAction::CreateLambda([WeakKnot]()
                    {
                        if (UK2Node_Knot* Node = WeakKnot.Get())
                        {
                            PortalNativeUtils::ConvertPortalFamilyToReroutes(Node);
                        }
                    }))
                );
            }
        }

        const bool bHasFamily = Role == EPortalKnotRole::Declaration ||
            (Role == EPortalKnotRole::Usage && !PortalNativeUtils::IsOrphanUsage(Knot));
        if (bHasFamily)
        {
            Section.AddMenuEntry(
                TEXT("PortalSelectFamily"),
                LOCTEXT("PortalSelectFamilyLabel", "Select Portal Family"),
                LOCTEXT("PortalSelectFamilyTooltip", "Selects the Input and every Output in this Portal family."),
                FSlateIcon(),
                FUIAction(FExecuteAction::CreateLambda([WeakKnot]()
                {
                    UK2Node_Knot* Node = WeakKnot.Get();
                    if (!Node)
                    {
                        return;
                    }

                    UK2Node_Knot* Declaration = PortalNativeUtils::GetRole(Node) == EPortalKnotRole::Declaration
                        ? Node
                        : PortalNativeUtils::FindDeclarationForUsage(Node, true);
                    if (!Declaration || !Declaration->GetGraph())
                    {
                        return;
                    }

                    FKismetEditorUtilities::AddToSelection(Declaration->GetGraph(), Declaration);
                    TArray<UK2Node_Knot*> Usages;
                    PortalNativeUtils::GetUsages(Declaration, Usages);
                    for (UK2Node_Knot* Usage : Usages)
                    {
                        if (Usage)
                        {
                            FKismetEditorUtilities::AddToSelection(Declaration->GetGraph(), Usage);
                        }
                    }
                }))
            );

            Section.AddMenuEntry(
                TEXT("PortalConvertFamilyToReroutes"),
                LOCTEXT("PortalConvertFamilyToReroutesLabel", "Convert Portal to Reroutes"),
                LOCTEXT("PortalConvertFamilyToReroutesTooltip", "Removes Portal metadata while preserving the native reroute nodes and physical Blueprint connections."),
                FSlateIcon(),
                FUIAction(FExecuteAction::CreateLambda([WeakKnot]()
                {
                    if (UK2Node_Knot* Node = WeakKnot.Get())
                    {
                        PortalNativeUtils::ConvertPortalFamilyToReroutes(Node);
                    }
                }))
            );
        }
    }

    FString BuildPortalActionSignature()
    {
        TArray<FString> Parts;
        for (TObjectIterator<UBlueprint> BlueprintIt; BlueprintIt; ++BlueprintIt)
        {
            UBlueprint* Blueprint = *BlueprintIt;
            if (!Blueprint || !Blueprint->IsAsset())
            {
                continue;
            }

            TArray<UK2Node_Knot*> Knots;
            FBlueprintEditorUtils::GetAllNodesOfClass(Blueprint, Knots);
            for (UK2Node_Knot* Knot : Knots)
            {
                if (!Knot || PortalNativeUtils::GetRole(Knot) != EPortalKnotRole::Declaration || !Knot->GetGraph())
                {
                    continue;
                }

                FPortalKnotData Data;
                if (!PortalNativeUtils::ReadPortalData(Knot, Data) || !Data.FamilyGuid.IsValid())
                {
                    continue;
                }

                const UEdGraphPin* Pin = Knot->GetInputPin();
                const FString TypeKey = Pin
                    ? FString::Printf(TEXT("%s:%s"), *Pin->PinType.PinCategory.ToString(), *GetNameSafe(Pin->PinType.PinSubCategoryObject.Get()))
                    : TEXT("None");

                TArray<UK2Node_Knot*> Outputs;
                PortalNativeUtils::GetUsages(Knot, Outputs);

                Parts.Add(FString::Printf(
                    TEXT("%u|%s|%s|%s|%s|%d"),
                    Knot->GetGraph()->GetUniqueID(),
                    *Knot->GetGraph()->GetPathName(),
                    *Data.FamilyGuid.ToString(EGuidFormats::Digits),
                    *Data.Name.ToString(),
                    *TypeKey,
                    Outputs.Num()));
            }
        }

        Parts.Sort();
        return FString::Join(Parts, TEXT(";"));
    }
}

void FPortalModule::StartupModule()
{
    LastActionSignature = TEXT("__PortalUninitialized__");

    NodeFactory = MakeShared<FPortalNodeFactory>();
    FEdGraphUtilities::RegisterVisualNodeFactory(NodeFactory);

    FGraphEditorModule& GraphEditorModule = FModuleManager::LoadModuleChecked<FGraphEditorModule>(TEXT("GraphEditor"));
    FGraphEditorModule::FGraphEditorMenuExtender_SelectedNode PinMenuExtender =
        FGraphEditorModule::FGraphEditorMenuExtender_SelectedNode::CreateStatic(&ExtendPortalPinContextMenu);
    GraphContextMenuExtenderHandle = PinMenuExtender.GetHandle();
    GraphEditorModule.GetAllGraphEditorContextMenuExtender().Add(PinMenuExtender);

    if (FSlateApplication::IsInitialized())
    {
        InputProcessor = MakeShared<FPortalInputProcessor>();
        FSlateApplication::Get().RegisterInputPreProcessor(InputProcessor, 0);
    }

    UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FPortalModule::RegisterMenus));

    RepairTickerHandle = FTSTicker::GetCoreTicker().AddTicker(
        FTickerDelegate::CreateRaw(this, &FPortalModule::TickRepair),
        0.75f);
}

void FPortalModule::ShutdownModule()
{
    if (RepairTickerHandle.IsValid())
    {
        FTSTicker::GetCoreTicker().RemoveTicker(RepairTickerHandle);
        RepairTickerHandle.Reset();
    }

    if (InputProcessor.IsValid() && FSlateApplication::IsInitialized())
    {
        FSlateApplication::Get().UnregisterInputPreProcessor(InputProcessor);
        InputProcessor.Reset();
    }

    if (GraphContextMenuExtenderHandle.IsValid() && FModuleManager::Get().IsModuleLoaded(TEXT("GraphEditor")))
    {
        FGraphEditorModule& GraphEditorModule = FModuleManager::GetModuleChecked<FGraphEditorModule>(TEXT("GraphEditor"));
        GraphEditorModule.GetAllGraphEditorContextMenuExtender().RemoveAll([this](const FGraphEditorModule::FGraphEditorMenuExtender_SelectedNode& Delegate)
        {
            return Delegate.GetHandle() == GraphContextMenuExtenderHandle;
        });
        GraphContextMenuExtenderHandle.Reset();
    }

    if (NodeFactory.IsValid())
    {
        FEdGraphUtilities::UnregisterVisualNodeFactory(NodeFactory);
        NodeFactory.Reset();
    }

    UToolMenus::UnRegisterStartupCallback(this);
    UToolMenus::UnregisterOwner(this);
}

void FPortalModule::RegisterMenus()
{
    FToolMenuOwnerScoped OwnerScoped(this);

    // GraphEditor builds class-specific node menus using this naming convention.
    // Extending the class menu keeps Portal entries limited to native reroute nodes.
    UToolMenu* KnotMenu = UToolMenus::Get()->ExtendMenu(TEXT("GraphEditor.GraphNodeContextMenu.K2Node_Knot"));
    if (KnotMenu)
    {
        FToolMenuSection& PortalSection = KnotMenu->FindOrAddSection(TEXT("Portal"), LOCTEXT("PortalMenuSection", "Portal"));
        PortalSection.AddDynamicEntry(
            TEXT("PortalDynamicActions"),
            FNewToolMenuSectionDelegate::CreateStatic(&AddPortalContextEntries));
    }
}

bool FPortalModule::TickRepair(float DeltaTime)
{
    PortalNativeUtils::MigrateAndRepairLoadedBlueprints();

    const FString ActionSignature = BuildPortalActionSignature();
    if (ActionSignature != LastActionSignature)
    {
        LastActionSignature = ActionSignature;
        FBlueprintActionDatabase::Get().RefreshClassActions(UK2Node_PortalDeclaration::StaticClass());
        FBlueprintActionDatabase::Get().RefreshClassActions(UK2Node_PortalUsage::StaticClass());
    }

    return true;
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FPortalModule, Portal)
