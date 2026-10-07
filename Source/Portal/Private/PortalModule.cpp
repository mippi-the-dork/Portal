// Copyright Mippithedork 2026, Inc. All Rights Reserved.

#include "PortalModule.h"

#include "EdGraph/EdGraphNode.h"
#include "EdGraphUtilities.h"
#include "Framework/Commands/UIAction.h"
#include "K2Node_Knot.h"
#include "Kismet2/KismetEditorUtilities.h"
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
            Section.AddMenuEntry(
                TEXT("PortalConvertReroute"),
                LOCTEXT("PortalConvertRerouteLabel", "Convert to Portal Declaration"),
                LOCTEXT("PortalConvertRerouteTooltip", "Converts this native Blueprint reroute into a Portal Declaration. Existing outgoing branches become Portal Usages."),
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
                LOCTEXT("PortalCreateUsageLabel", "Create Usage"),
                LOCTEXT("PortalCreateUsageTooltip", "Creates another Usage for this Portal."),
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
                LOCTEXT("PortalJumpToDeclarationLabel", "Jump to Declaration"),
                LOCTEXT("PortalJumpToDeclarationTooltip", "Focuses this Usage's Portal Declaration."),
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
                LOCTEXT("PortalCreateAnotherUsageLabel", "Create Another Usage"),
                LOCTEXT("PortalCreateAnotherUsageTooltip", "Creates another Usage for this Portal."),
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
            LOCTEXT("PortalSelectFamilyTooltip", "Selects the Declaration and every Usage in this Portal family."),
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
