// Copyright Mippithedork 2026, Inc. All Rights Reserved.

#include "PortalModule.h"

#include "EdGraph/EdGraphNode.h"
#include "EdGraph/EdGraphPin.h"
#include "EdGraphSchema_K2.h"
#include "EdGraphUtilities.h"
#include "Framework/Application/SlateApplication.h"
#include "Framework/Commands/UIAction.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "GraphEditorModule.h"
#include "K2Node_Knot.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "PortalInputProcessor.h"
#include "PortalNativeUtils.h"
#include "PortalNodeFactory.h"
#include "ToolMenu.h"
#include "ToolMenuSection.h"
#include "ToolMenus.h"

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

    TSharedRef<FExtender> ExtendPortalPinContextMenu(
        const TSharedRef<FUICommandList> CommandList,
        const UEdGraph* Graph,
        const UEdGraphNode* Node,
        const UEdGraphPin* Pin,
        bool bIsConst)
    {
        TSharedRef<FExtender> Extender = MakeShared<FExtender>();
        if (bIsConst || !Graph || !Graph->GetSchema() || !Graph->GetSchema()->IsA<UEdGraphSchema_K2>() ||
            !PortalNativeUtils::CanCreatePortalFromOutputPin(Pin))
        {
            return Extender;
        }

        TWeakObjectPtr<UEdGraphNode> WeakOwner(Pin->GetOwningNode());
        const FGuid PinId = Pin->PinId;
        Extender->AddMenuExtension(
            TEXT("EdGraphSchemaPinActions"),
            EExtensionHook::Before,
            CommandList,
            FMenuExtensionDelegate::CreateLambda([WeakOwner, PinId](FMenuBuilder& MenuBuilder)
            {
                MenuBuilder.BeginSection(TEXT("PortalPinActions"), LOCTEXT("PortalPinActionsSection", "Portal"));
                MenuBuilder.AddMenuEntry(
                    LOCTEXT("CreatePortalFromPinLabel", "Create Portal"),
                    LOCTEXT("CreatePortalFromPinTooltip", "Creates a Portal Input from this data output pin and connects it automatically."),
                    FSlateIcon(),
                    FUIAction(FExecuteAction::CreateLambda([WeakOwner, PinId]()
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

                        if (UK2Node_Knot* Input = PortalNativeUtils::CreatePortalFromOutputPin(CurrentPin))
                        {
                            FKismetEditorUtilities::BringKismetToFocusAttentionOnObject(Input, true);
                        }
                    })));
                MenuBuilder.EndSection();
            }));

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
        }
        else
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

void FPortalModule::StartupModule()
{
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
    return true;
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FPortalModule, Portal)
