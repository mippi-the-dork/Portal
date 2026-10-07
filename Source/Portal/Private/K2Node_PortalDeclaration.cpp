// Copyright Mippithedork 2026, Inc. All Rights Reserved.

#include "K2Node_PortalDeclaration.h"

#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintNodeSpawner.h"
#include "EdGraph/EdGraph.h"
#include "EdGraph/EdGraphPin.h"
#include "EdGraphSchema_K2.h"
#include "Framework/Commands/UIAction.h"
#include "K2Node_PortalUsage.h"
#include "Kismet2/Kismet2NameValidators.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "PortalGraphUtils.h"
#include "ScopedTransaction.h"
#include "ToolMenu.h"
#include "ToolMenuSection.h"

#define LOCTEXT_NAMESPACE "K2Node_PortalDeclaration"

UK2Node_PortalDeclaration::UK2Node_PortalDeclaration(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
    , PortalName(TEXT("Portal"))
    , PortalColor(0.08f, 0.42f, 0.75f, 1.0f)
{
    bCanRenameNode = true;
}

void UK2Node_PortalDeclaration::AllocateDefaultPins()
{
    Super::AllocateDefaultPins();

    if (UEdGraphPin* OutputPin = GetOutputPin())
    {
        OutputPin->bHidden = true;
        OutputPin->bDefaultValueIsIgnored = true;
    }
}

void UK2Node_PortalDeclaration::PostPlacedNewNode()
{
    Super::PostPlacedNewNode();
    EnsurePortalIdentity(false);
    EnsureUniquePortalName();
}

void UK2Node_PortalDeclaration::PostLoad()
{
    Super::PostLoad();
    EnsurePortalIdentity(false);
}

void UK2Node_PortalDeclaration::PostPasteNode()
{
    Super::PostPasteNode();

    const bool bHasCollision = PortalGraphUtils::FindDeclarationByGuid(GetGraph(), PortalGuid, this) != nullptr;
    EnsurePortalIdentity(bHasCollision);
    EnsureUniquePortalName();
    RefreshLinkedUsages();
    RefreshPortalFamilyType();
}

void UK2Node_PortalDeclaration::PostReconstructNode()
{
    UK2Node::PostReconstructNode();

    if (UEdGraphPin* OutputPin = GetOutputPin())
    {
        OutputPin->bHidden = true;
        OutputPin->bDefaultValueIsIgnored = true;
    }

    EnsurePortalIdentity(false);
    RefreshLinkedUsages();
    RefreshPortalFamilyType();
}

void UK2Node_PortalDeclaration::NotifyPinConnectionListChanged(UEdGraphPin* Pin)
{
    UK2Node::NotifyPinConnectionListChanged(Pin);
    RefreshPortalFamilyType();
}

void UK2Node_PortalDeclaration::OnRenameNode(const FString& NewName)
{
    Modify();
    PortalName = FName(*NewName.TrimStartAndEnd());
    EnsureUniquePortalName();

    TArray<UK2Node_PortalUsage*> Usages;
    PortalGraphUtils::GetUsagesForDeclaration(this, Usages);
    for (UK2Node_PortalUsage* Usage : Usages)
    {
        if (Usage)
        {
            Usage->Modify();
            Usage->Declaration = this;
            Usage->DeclarationGuid = PortalGuid;
            Usage->DeclarationGraphGuid = GetGraph() ? GetGraph()->GraphGuid : FGuid();
            Usage->CacheDeclarationPresentation();
        }
    }

    if (UEdGraph* Graph = GetGraph())
    {
        Graph->NotifyGraphChanged();
        PortalGraphUtils::MarkOwningBlueprintModified(Graph);
    }
}

TSharedPtr<INameValidatorInterface> UK2Node_PortalDeclaration::MakeNameValidator() const
{
    // Match UK2Node_Knot: the graph rename UI requires a valid validator object
    // whenever bCanRenameNode is true. Portal enforces unique names in OnRenameNode.
    return MakeShareable(new FDummyNameValidator(EValidatorResult::Ok));
}

FText UK2Node_PortalDeclaration::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
    if (TitleType == ENodeTitleType::MenuTitle)
    {
        return LOCTEXT("PortalDeclarationMenuTitle", "Portal Declaration");
    }

    return FText::FromName(PortalName.IsNone() ? FName(TEXT("Portal")) : PortalName);
}

FText UK2Node_PortalDeclaration::GetTooltipText() const
{
    TArray<UK2Node_PortalUsage*> Usages;
    PortalGraphUtils::GetUsagesForDeclaration(this, Usages);

    return FText::Format(
        LOCTEXT("PortalDeclarationTooltip", "Portal Declaration '{0}'. {1} linked Usage(s). Right-click to create another Usage or select the Portal family."),
        FText::FromName(PortalName.IsNone() ? FName(TEXT("Portal")) : PortalName),
        FText::AsNumber(Usages.Num())
    );
}

FText UK2Node_PortalDeclaration::GetMenuCategory() const
{
    return LOCTEXT("PortalMenuCategory", "Portal");
}

FLinearColor UK2Node_PortalDeclaration::GetNodeTitleColor() const
{
    return PortalColor;
}

bool UK2Node_PortalDeclaration::ShouldDrawNodeAsControlPointOnly(int32& OutInputPinIndex, int32& OutOutputPinIndex) const
{
    OutInputPinIndex = INDEX_NONE;
    OutOutputPinIndex = INDEX_NONE;
    return false;
}

void UK2Node_PortalDeclaration::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
    // Legacy 0.1-0.3 node class retained only so existing assets can load and
    // migrate to native UK2Node_Knot-backed Portals. New legacy nodes are not exposed.
}

void UK2Node_PortalDeclaration::GetNodeContextMenuActions(UToolMenu* Menu, UGraphNodeContextMenuContext* Context) const
{
    Super::GetNodeContextMenuActions(Menu, Context);

    if (!Menu || (Context && Context->bIsDebugging))
    {
        return;
    }

    TWeakObjectPtr<UK2Node_PortalDeclaration> WeakDeclaration(const_cast<UK2Node_PortalDeclaration*>(this));
    FToolMenuSection& Section = Menu->AddSection(TEXT("Portal"), LOCTEXT("PortalContextSection", "Portal"));
    Section.AddMenuEntry(
        TEXT("PortalCreateUsage"),
        LOCTEXT("PortalCreateUsageLabel", "Create Usage"),
        LOCTEXT("PortalCreateUsageTooltip", "Creates another Usage for this Portal."),
        FSlateIcon(),
        FUIAction(FExecuteAction::CreateLambda([WeakDeclaration]()
        {
            if (UK2Node_PortalDeclaration* Declaration = WeakDeclaration.Get())
            {
                Declaration->CreateUsageNode();
            }
        }))
    );

    Section.AddMenuEntry(
        TEXT("PortalSelectAllUsages"),
        LOCTEXT("PortalSelectAllUsagesLabel", "Select Portal Family"),
        LOCTEXT("PortalSelectAllUsagesTooltip", "Selects this Declaration and every Usage linked to it."),
        FSlateIcon(),
        FUIAction(FExecuteAction::CreateLambda([WeakDeclaration]()
        {
            if (UK2Node_PortalDeclaration* Declaration = WeakDeclaration.Get())
            {
                FKismetEditorUtilities::AddToSelection(Declaration->GetGraph(), Declaration);

                TArray<UK2Node_PortalUsage*> Usages;
                PortalGraphUtils::GetUsagesForDeclaration(Declaration, Usages);
                for (UK2Node_PortalUsage* Usage : Usages)
                {
                    if (Usage)
                    {
                        FKismetEditorUtilities::AddToSelection(Declaration->GetGraph(), Usage);
                    }
                }
            }
        }))
    );
}

bool UK2Node_PortalDeclaration::IsPortalIdentityValid() const
{
    return PortalGuid.IsValid();
}

void UK2Node_PortalDeclaration::EnsurePortalIdentity(bool bForceNewGuid)
{
    if (bForceNewGuid || !PortalGuid.IsValid())
    {
        do
        {
            PortalGuid = FGuid::NewGuid();
        }
        while (PortalGraphUtils::FindDeclarationByGuid(GetGraph(), PortalGuid, this) != nullptr);
    }
}

void UK2Node_PortalDeclaration::EnsureUniquePortalName()
{
    UEdGraph* Graph = GetGraph();
    FString BaseName = PortalName.IsNone() ? TEXT("Portal") : PortalName.ToString().TrimStartAndEnd();
    if (BaseName.IsEmpty())
    {
        BaseName = TEXT("Portal");
    }

    FString Candidate = BaseName;
    int32 Suffix = 2;

    auto IsTaken = [this, Graph](const FName CandidateName)
    {
        if (!Graph)
        {
            return false;
        }

        for (UEdGraphNode* Node : Graph->Nodes)
        {
            const UK2Node_PortalDeclaration* Other = Cast<UK2Node_PortalDeclaration>(Node);
            if (Other && Other != this && Other->PortalName == CandidateName)
            {
                return true;
            }
        }

        return false;
    };

    while (IsTaken(FName(*Candidate)))
    {
        Candidate = FString::Printf(TEXT("%s_%d"), *BaseName, Suffix++);
    }

    PortalName = FName(*Candidate);
}

void UK2Node_PortalDeclaration::RefreshLinkedUsages()
{
    TArray<UK2Node_PortalUsage*> Usages;
    PortalGraphUtils::GetUsagesForDeclaration(this, Usages);
    for (UK2Node_PortalUsage* Usage : Usages)
    {
        if (!Usage)
        {
            continue;
        }

        Usage->Declaration = this;
        Usage->DeclarationGuid = PortalGuid;
        Usage->DeclarationGraphGuid = GetGraph() ? GetGraph()->GraphGuid : FGuid();
        Usage->CacheDeclarationPresentation();
        Usage->EnsureHiddenLink();
    }
}

void UK2Node_PortalDeclaration::RefreshPortalFamilyType()
{
    if (bRefreshingFamily)
    {
        return;
    }

    TGuardValue<bool> Guard(bRefreshingFamily, true);

    UEdGraphPin* DeclarationInput = GetInputPin();
    UEdGraphPin* DeclarationHiddenOutput = GetOutputPin();
    if (!DeclarationInput || !DeclarationHiddenOutput)
    {
        return;
    }

    TArray<UK2Node_PortalUsage*> Usages;
    PortalGraphUtils::GetUsagesForDeclaration(this, Usages);

    const FEdGraphPinType* ChosenType = nullptr;

    for (UEdGraphPin* LinkedPin : DeclarationInput->LinkedTo)
    {
        if (LinkedPin && LinkedPin->PinType.PinCategory != UEdGraphSchema_K2::PC_Wildcard)
        {
            ChosenType = &LinkedPin->PinType;
            break;
        }
    }

    if (!ChosenType)
    {
        for (UK2Node_PortalUsage* Usage : Usages)
        {
            if (!Usage || !Usage->GetOutputPin())
            {
                continue;
            }

            for (UEdGraphPin* LinkedPin : Usage->GetOutputPin()->LinkedTo)
            {
                if (LinkedPin && LinkedPin->PinType.PinCategory != UEdGraphSchema_K2::PC_Wildcard)
                {
                    ChosenType = &LinkedPin->PinType;
                    break;
                }
            }

            if (ChosenType)
            {
                break;
            }
        }
    }

    FEdGraphPinType ResolvedType;
    if (ChosenType)
    {
        ResolvedType = *ChosenType;
    }
    else
    {
        ResolvedType.ResetToDefaults();
        ResolvedType.PinCategory = UEdGraphSchema_K2::PC_Wildcard;
    }

    auto ApplyType = [&ResolvedType](UEdGraphPin* Pin)
    {
        if (Pin && Pin->PinType != ResolvedType)
        {
            Pin->PinType = ResolvedType;
        }
    };

    ApplyType(DeclarationInput);
    ApplyType(DeclarationHiddenOutput);

    for (UK2Node_PortalUsage* Usage : Usages)
    {
        if (!Usage)
        {
            continue;
        }

        Usage->Declaration = this;
        Usage->DeclarationGuid = PortalGuid;
        Usage->DeclarationGraphGuid = GetGraph() ? GetGraph()->GraphGuid : FGuid();
        Usage->CacheDeclarationPresentation();
        Usage->EnsureHiddenLink();
        ApplyType(Usage->GetInputPin());
        ApplyType(Usage->GetOutputPin());
    }

    auto NotifyExternalNode = [](UEdGraphPin* LinkedPin)
    {
        if (LinkedPin)
        {
            if (UK2Node* Node = Cast<UK2Node>(LinkedPin->GetOwningNode()))
            {
                Node->PinConnectionListChanged(LinkedPin);
            }
        }
    };

    for (UEdGraphPin* LinkedPin : DeclarationInput->LinkedTo)
    {
        NotifyExternalNode(LinkedPin);
    }

    for (UK2Node_PortalUsage* Usage : Usages)
    {
        if (!Usage || !Usage->GetOutputPin())
        {
            continue;
        }

        for (UEdGraphPin* LinkedPin : Usage->GetOutputPin()->LinkedTo)
        {
            NotifyExternalNode(LinkedPin);
        }
    }
}

UK2Node_PortalUsage* UK2Node_PortalDeclaration::CreateUsageNode()
{
    UEdGraph* Graph = GetGraph();
    if (!Graph)
    {
        return nullptr;
    }

    const FScopedTransaction Transaction(LOCTEXT("CreatePortalUsageTransaction", "Create Portal Usage"));
    Graph->Modify();
    Modify();

    TArray<UK2Node_PortalUsage*> ExistingUsages;
    PortalGraphUtils::GetUsagesForDeclaration(this, ExistingUsages);
    const int32 ExistingUsageCount = ExistingUsages.Num();

    FGraphNodeCreator<UK2Node_PortalUsage> NodeCreator(*Graph);
    UK2Node_PortalUsage* Usage = NodeCreator.CreateUserInvokedNode(true, UK2Node_PortalUsage::StaticClass());

    Usage->Declaration = this;
    Usage->DeclarationGuid = PortalGuid;
    Usage->DeclarationGraphGuid = Graph->GraphGuid;
    Usage->CachedPortalName = PortalName;
    Usage->CachedPortalColor = PortalColor;

    Usage->NodePosX = NodePosX + 260;
    Usage->NodePosY = NodePosY + (ExistingUsageCount * 64);

    NodeCreator.Finalize();

    Usage->SetDeclaration(this, true);
    RefreshPortalFamilyType();
    Graph->NotifyGraphChanged();
    PortalGraphUtils::MarkOwningBlueprintModified(Graph);

    return Usage;
}

#undef LOCTEXT_NAMESPACE
