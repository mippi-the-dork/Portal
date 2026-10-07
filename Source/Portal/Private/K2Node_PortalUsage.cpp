// Copyright Mippithedork 2026, Inc. All Rights Reserved.

#include "K2Node_PortalUsage.h"

#include "BlueprintActionDatabaseRegistrar.h"
#include "EdGraph/EdGraph.h"
#include "EdGraph/EdGraphPin.h"
#include "EdGraphSchema_K2.h"
#include "Framework/Commands/UIAction.h"
#include "K2Node_PortalDeclaration.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "KismetCompiler.h"
#include "PortalGraphUtils.h"
#include "ToolMenu.h"
#include "ToolMenuSection.h"

#define LOCTEXT_NAMESPACE "K2Node_PortalUsage"

UK2Node_PortalUsage::UK2Node_PortalUsage(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    bCanRenameNode = false;
}

void UK2Node_PortalUsage::AllocateDefaultPins()
{
    Super::AllocateDefaultPins();

    if (UEdGraphPin* InputPin = GetInputPin())
    {
        InputPin->bHidden = true;
        InputPin->bDefaultValueIsIgnored = true;
    }
}

void UK2Node_PortalUsage::PostLoad()
{
    Super::PostLoad();
    ResolveDeclaration(true);
}

void UK2Node_PortalUsage::PostPasteNode()
{
    Super::PostPasteNode();

    // Resolve the copied physical bridge first. This is the most reliable way
    // to keep a Declaration + Usage selection together when both are pasted.
    ResolveDeclaration(true);

    if (Declaration)
    {
        Declaration->RefreshPortalFamilyType();
    }
}

void UK2Node_PortalUsage::PostReconstructNode()
{
    UK2Node::PostReconstructNode();

    if (UEdGraphPin* InputPin = GetInputPin())
    {
        InputPin->bHidden = true;
        InputPin->bDefaultValueIsIgnored = true;
    }

    ResolveDeclaration(true);
    if (Declaration)
    {
        Declaration->RefreshPortalFamilyType();
    }
}

void UK2Node_PortalUsage::NotifyPinConnectionListChanged(UEdGraphPin* Pin)
{
    UK2Node::NotifyPinConnectionListChanged(Pin);

    if (UK2Node_PortalDeclaration* Resolved = ResolveDeclaration(false))
    {
        Resolved->RefreshPortalFamilyType();
    }
}

void UK2Node_PortalUsage::ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
    if (!ResolveDeclaration(false))
    {
        const FString PortalLabel = CachedPortalName.IsNone() ? TEXT("Unknown") : CachedPortalName.ToString();
        CompilerContext.MessageLog.Error(*FString::Printf(TEXT("Portal Output @@ for '%s' has no valid Input."), *PortalLabel), this);
        BreakAllNodeLinks();
        return;
    }

    Super::ExpandNode(CompilerContext, SourceGraph);
}

FText UK2Node_PortalUsage::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
    if (IsDeclarationValid())
    {
        return FText::FromName(Declaration->PortalName);
    }

    if (!CachedPortalName.IsNone())
    {
        return FText::Format(LOCTEXT("MissingNamedPortalTitle", "Missing Portal: {0}"), FText::FromName(CachedPortalName));
    }

    return LOCTEXT("MissingPortalTitle", "Missing Portal");
}

FText UK2Node_PortalUsage::GetTooltipText() const
{
    if (IsDeclarationValid())
    {
        return FText::Format(
            LOCTEXT("PortalUsageTooltip", "Portal Output for '{0}'. Double-click to jump to its Input."),
            FText::FromName(Declaration->PortalName)
        );
    }

    if (!CachedPortalName.IsNone())
    {
        return FText::Format(
            LOCTEXT("MissingNamedPortalTooltip", "This Portal Output was linked to '{0}', but its Input is missing from this graph."),
            FText::FromName(CachedPortalName)
        );
    }

    return LOCTEXT("MissingPortalTooltip", "This Portal Output no longer has a valid Input in this graph.");
}

FLinearColor UK2Node_PortalUsage::GetNodeTitleColor() const
{
    return IsDeclarationValid() ? Declaration->PortalColor : FLinearColor(0.75f, 0.05f, 0.05f, 1.0f);
}

bool UK2Node_PortalUsage::ShouldDrawNodeAsControlPointOnly(int32& OutInputPinIndex, int32& OutOutputPinIndex) const
{
    OutInputPinIndex = INDEX_NONE;
    OutOutputPinIndex = INDEX_NONE;
    return false;
}

UObject* UK2Node_PortalUsage::GetJumpTargetForDoubleClick() const
{
    return IsDeclarationValid() ? Declaration.Get() : nullptr;
}

void UK2Node_PortalUsage::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
    // Usage nodes are intentionally not exposed as standalone Blueprint actions.
    // Create them from a Portal Declaration so they always start bound.
}

void UK2Node_PortalUsage::GetNodeContextMenuActions(UToolMenu* Menu, UGraphNodeContextMenuContext* Context) const
{
    Super::GetNodeContextMenuActions(Menu, Context);

    if (!Menu || (Context && Context->bIsDebugging))
    {
        return;
    }

    TWeakObjectPtr<UK2Node_PortalUsage> WeakUsage(const_cast<UK2Node_PortalUsage*>(this));
    FToolMenuSection& Section = Menu->AddSection(TEXT("Portal"), LOCTEXT("PortalContextSection", "Portal"));
    Section.AddMenuEntry(
        TEXT("PortalJumpToDeclaration"),
        LOCTEXT("PortalJumpToDeclarationLabel", "Jump to Input"),
        LOCTEXT("PortalJumpToDeclarationTooltip", "Focuses this Output's Input."),
        FSlateIcon(),
        FUIAction(
            FExecuteAction::CreateLambda([WeakUsage]()
            {
                if (UK2Node_PortalUsage* Usage = WeakUsage.Get())
                {
                    if (UK2Node_PortalDeclaration* Resolved = Usage->ResolveDeclaration(true))
                    {
                        FKismetEditorUtilities::BringKismetToFocusAttentionOnObject(Resolved, false);
                    }
                }
            }),
            FCanExecuteAction::CreateLambda([WeakUsage]()
            {
                const UK2Node_PortalUsage* Usage = WeakUsage.Get();
                return Usage && Usage->IsDeclarationValid();
            })
        )
    );

    Section.AddMenuEntry(
        TEXT("PortalCreateAnotherUsage"),
        LOCTEXT("PortalCreateAnotherUsageLabel", "Create Another Output"),
        LOCTEXT("PortalCreateAnotherUsageTooltip", "Creates another Output for the same Portal."),
        FSlateIcon(),
        FUIAction(
            FExecuteAction::CreateLambda([WeakUsage]()
            {
                if (UK2Node_PortalUsage* Usage = WeakUsage.Get())
                {
                    if (UK2Node_PortalDeclaration* Resolved = Usage->ResolveDeclaration(true))
                    {
                        Resolved->CreateUsageNode();
                    }
                }
            }),
            FCanExecuteAction::CreateLambda([WeakUsage]()
            {
                const UK2Node_PortalUsage* Usage = WeakUsage.Get();
                return Usage && Usage->IsDeclarationValid();
            })
        )
    );

    Section.AddMenuEntry(
        TEXT("PortalSelectFamily"),
        LOCTEXT("PortalSelectFamilyLabel", "Select Portal Family"),
        LOCTEXT("PortalSelectFamilyTooltip", "Selects the Input and all Outputs for this Portal."),
        FSlateIcon(),
        FUIAction(
            FExecuteAction::CreateLambda([WeakUsage]()
            {
                if (UK2Node_PortalUsage* Usage = WeakUsage.Get())
                {
                    if (UK2Node_PortalDeclaration* Resolved = Usage->ResolveDeclaration(true))
                    {
                        FKismetEditorUtilities::AddToSelection(Resolved->GetGraph(), Resolved);

                        TArray<UK2Node_PortalUsage*> Usages;
                        PortalGraphUtils::GetUsagesForDeclaration(Resolved, Usages);
                        for (UK2Node_PortalUsage* FamilyUsage : Usages)
                        {
                            if (FamilyUsage)
                            {
                                FKismetEditorUtilities::AddToSelection(Resolved->GetGraph(), FamilyUsage);
                            }
                        }
                    }
                }
            }),
            FCanExecuteAction::CreateLambda([WeakUsage]()
            {
                const UK2Node_PortalUsage* Usage = WeakUsage.Get();
                return Usage && Usage->IsDeclarationValid();
            })
        )
    );
}

bool UK2Node_PortalUsage::IsDeclarationValid() const
{
    const UEdGraph* Graph = GetGraph();
    if (!Graph || !IsValid(Declaration.Get()))
    {
        return false;
    }

    if (Declaration->GetGraph() != Graph || Declaration->PortalGuid != DeclarationGuid)
    {
        return false;
    }

    // DeclarationGraphGuid was added in 0.2.0. Invalid means a legacy Portal
    // asset and is accepted once so it can be upgraded in place.
    return !DeclarationGraphGuid.IsValid() || DeclarationGraphGuid == Graph->GraphGuid;
}

UK2Node_PortalDeclaration* UK2Node_PortalUsage::FindDeclarationFromHiddenLink() const
{
    const UEdGraph* Graph = GetGraph();
    const UEdGraphPin* InputPin = GetInputPin();
    if (!Graph || !InputPin)
    {
        return nullptr;
    }

    for (UEdGraphPin* LinkedPin : InputPin->LinkedTo)
    {
        if (!LinkedPin)
        {
            continue;
        }

        UK2Node_PortalDeclaration* LinkedDeclaration = Cast<UK2Node_PortalDeclaration>(LinkedPin->GetOwningNode());
        if (LinkedDeclaration && LinkedDeclaration->GetGraph() == Graph && LinkedPin == LinkedDeclaration->GetOutputPin())
        {
            return LinkedDeclaration;
        }
    }

    return nullptr;
}

UK2Node_PortalDeclaration* UK2Node_PortalUsage::ResolveDeclaration(bool bRepairHiddenLink)
{
    UEdGraph* Graph = GetGraph();
    if (!Graph)
    {
        Declaration = nullptr;
        return nullptr;
    }

    // First trust an internal physical bridge that survived copy/paste. This
    // ensures a copied Portal family binds to its copied Declaration, even if
    // the Declaration later receives a replacement GUID due to a collision.
    if (UK2Node_PortalDeclaration* LinkedDeclaration = FindDeclarationFromHiddenLink())
    {
        Declaration = LinkedDeclaration;
        DeclarationGuid = LinkedDeclaration->PortalGuid;
        DeclarationGraphGuid = Graph->GraphGuid;
        CacheDeclarationPresentation();

        if (bRepairHiddenLink)
        {
            EnsureHiddenLink();
        }
        return Declaration;
    }

    if (IsDeclarationValid())
    {
        DeclarationGraphGuid = Graph->GraphGuid;
        CacheDeclarationPresentation();
        if (bRepairHiddenLink)
        {
            EnsureHiddenLink();
        }
        return Declaration;
    }

    Declaration = nullptr;

    // A Usage copied by itself may repair by GUID only when it is still in the
    // graph that originally owned the Declaration. This prevents cross-graph
    // paste from silently attaching to an unrelated Portal with the same GUID.
    const bool bLegacyUsage = !DeclarationGraphGuid.IsValid();
    const bool bSameOriginalGraph = DeclarationGraphGuid == Graph->GraphGuid;
    if (bLegacyUsage || bSameOriginalGraph)
    {
        Declaration = PortalGraphUtils::FindDeclarationByGuid(Graph, DeclarationGuid);
    }

    if (Declaration)
    {
        DeclarationGuid = Declaration->PortalGuid;
        DeclarationGraphGuid = Graph->GraphGuid;
        CacheDeclarationPresentation();

        if (bRepairHiddenLink)
        {
            EnsureHiddenLink();
        }
    }
    else if (UEdGraphPin* InputPin = GetInputPin())
    {
        InputPin->BreakAllPinLinks();
    }

    return Declaration;
}

void UK2Node_PortalUsage::SetDeclaration(UK2Node_PortalDeclaration* InDeclaration, bool bRepairHiddenLink)
{
    Modify();
    Declaration = InDeclaration;
    DeclarationGuid = InDeclaration ? InDeclaration->PortalGuid : FGuid();
    DeclarationGraphGuid = (InDeclaration && InDeclaration->GetGraph()) ? InDeclaration->GetGraph()->GraphGuid : FGuid();
    CacheDeclarationPresentation();

    if (bRepairHiddenLink)
    {
        EnsureHiddenLink();
    }
}

void UK2Node_PortalUsage::CacheDeclarationPresentation()
{
    if (IsValid(Declaration.Get()))
    {
        CachedPortalName = Declaration->PortalName;
        CachedPortalColor = Declaration->PortalColor;
    }
}

void UK2Node_PortalUsage::EnsureHiddenLink()
{
    UEdGraphPin* InputPin = GetInputPin();
    if (!InputPin)
    {
        return;
    }

    InputPin->bHidden = true;
    InputPin->bDefaultValueIsIgnored = true;

    if (!IsValid(Declaration.Get()) || Declaration->GetGraph() != GetGraph())
    {
        InputPin->BreakAllPinLinks();
        return;
    }

    UEdGraphPin* DeclarationOutput = Declaration->GetOutputPin();
    if (!DeclarationOutput)
    {
        return;
    }

    DeclarationOutput->bHidden = true;
    DeclarationOutput->bDefaultValueIsIgnored = true;

    const bool bAlreadyLinked = InputPin->LinkedTo.Contains(DeclarationOutput) && DeclarationOutput->LinkedTo.Contains(InputPin);
    if (bAlreadyLinked)
    {
        return;
    }

    InputPin->BreakAllPinLinks();

    const UEdGraphSchema* Schema = GetSchema();
    if (Schema)
    {
        Schema->TryCreateConnection(DeclarationOutput, InputPin);
    }
}

#undef LOCTEXT_NAMESPACE
