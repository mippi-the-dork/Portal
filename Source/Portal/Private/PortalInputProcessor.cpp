// Copyright Mippithedork 2026, Inc. All Rights Reserved.

#include "PortalInputProcessor.h"

#include "EdGraph/EdGraph.h"
#include "EdGraphSchema_K2.h"
#include "Framework/Application/SlateApplication.h"
#include "InputCoreTypes.h"
#include "K2Node_Knot.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Misc/MessageDialog.h"
#include "Layout/WidgetPath.h"
#include "PortalNativeUtils.h"
#include "ScopedTransaction.h"
#include "SGraphPanel.h"
#include "Widgets/SWindow.h"

namespace
{
    TWeakObjectPtr<UEdGraph> GLastRightClickGraph;
    FVector2f GLastRightClickPosition = FVector2f::ZeroVector;
    TWeakObjectPtr<UEdGraph> GLastInteractedGraph;
    FVector2f GLastInteractedPosition = FVector2f::ZeroVector;

    TSharedPtr<SGraphPanel> FindGraphPanelAtPosition(FSlateApplication& SlateApp, const FVector2D& ScreenPosition, int32 UserIndex)
    {
        const FWidgetPath WidgetPath = SlateApp.LocateWindowUnderMouse(
            ScreenPosition,
            SlateApp.GetInteractiveTopLevelWindows(),
            false,
            UserIndex);

        if (!WidgetPath.IsValid())
        {
            return nullptr;
        }

        for (int32 Index = WidgetPath.Widgets.Num() - 1; Index >= 0; --Index)
        {
            const TSharedRef<SWidget>& Widget = WidgetPath.Widgets[Index].Widget;
            if (Widget->GetTypeAsString() == TEXT("SGraphPanel"))
            {
                return StaticCastSharedRef<SGraphPanel>(Widget);
            }
        }

        return nullptr;
    }

    TSharedPtr<SGraphPanel> FindGraphPanelUnderMouse(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent)
    {
        return FindGraphPanelAtPosition(SlateApp, MouseEvent.GetScreenSpacePosition(), MouseEvent.GetUserIndex());
    }

    UK2Node_Knot* GetSelectedPortalDeclaration(const TSharedRef<SGraphPanel>& GraphPanel)
    {
        const TArray<UEdGraphNode*> SelectedNodes = GraphPanel->GetSelectedGraphNodes();
        if (SelectedNodes.Num() != 1)
        {
            return nullptr;
        }

        UK2Node_Knot* Knot = Cast<UK2Node_Knot>(SelectedNodes[0]);
        if (!Knot || !PortalNativeUtils::IsPortalKnot(Knot))
        {
            return nullptr;
        }

        if (PortalNativeUtils::GetRole(Knot) == EPortalKnotRole::Declaration)
        {
            return Knot;
        }

        return PortalNativeUtils::FindDeclarationForUsage(Knot, true);
    }
}

bool FPortalInputProcessor::GetLastGraphRightClick(const UEdGraph*& OutGraph, FVector2f& OutGraphPosition)
{
    OutGraph = GLastRightClickGraph.Get();
    OutGraphPosition = GLastRightClickPosition;
    return OutGraph != nullptr;
}

bool FPortalInputProcessor::GetLastInteractedGraph(const UEdGraph*& OutGraph, FVector2f& OutGraphPosition)
{
    OutGraph = GLastInteractedGraph.Get();
    OutGraphPosition = GLastInteractedPosition;
    return OutGraph != nullptr;
}

bool FPortalInputProcessor::HandleKeyDownEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent)
{
    if (InKeyEvent.GetKey() == EKeys::Delete)
    {
        TSharedPtr<SGraphPanel> GraphPanel = FindGraphPanelAtPosition(SlateApp, SlateApp.GetCursorPos(), 0);
        if (GraphPanel.IsValid() && GraphPanel->IsGraphEditable())
        {
            const TArray<UEdGraphNode*> SelectedNodes = GraphPanel->GetSelectedGraphNodes();
            if (SelectedNodes.Num() == 1)
            {
                UK2Node_Knot* Input = Cast<UK2Node_Knot>(SelectedNodes[0]);
                if (Input && PortalNativeUtils::GetRole(Input) == EPortalKnotRole::Declaration)
                {
                    TArray<UK2Node_Knot*> Outputs;
                    PortalNativeUtils::GetUsages(Input, Outputs);
                    if (!Outputs.IsEmpty())
                    {
                        const FText Message = FText::Format(
                            NSLOCTEXT("Portal", "DeleteInputWithOutputsPrompt",
                                "Portal Input '{0}' has {1} Output(s).\n\nYes: delete the entire Portal family.\nNo: delete only the Input and leave recoverable orphan Outputs.\nCancel: do nothing."),
                            FText::FromName(PortalNativeUtils::GetPortalName(Input)),
                            FText::AsNumber(Outputs.Num()));

                        const EAppReturnType::Type Result = FMessageDialog::Open(EAppMsgType::YesNoCancel, Message, NSLOCTEXT("Portal", "DeletePortalInputTitle", "Delete Portal Input"));
                        if (Result == EAppReturnType::Yes)
                        {
                            PortalNativeUtils::DeletePortalFamily(Input);
                            return true;
                        }
                        if (Result == EAppReturnType::Cancel)
                        {
                            return true;
                        }
                        if (Result == EAppReturnType::No)
                        {
                            UEdGraph* Graph = Input->GetGraph();
                            if (!Graph)
                            {
                                return true;
                            }

                            const FScopedTransaction Transaction(NSLOCTEXT("Portal", "DeletePortalInputOnly", "Delete Portal Input"));
                            Graph->Modify();
                            Input->Modify();
                            for (UEdGraphPin* Pin : Input->Pins)
                            {
                                if (Pin)
                                {
                                    Pin->BreakAllPinLinks();
                                }
                            }
                            Graph->RemoveNode(Input);
                            Graph->NotifyGraphChanged();
                            PortalNativeUtils::MarkBlueprintModified(Graph, true);
                            return true;
                        }
                    }
                }
            }
        }
    }

    if (InKeyEvent.GetKey() == EKeys::R)
    {
        bRKeyDown = true;
    }

    return false;
}

bool FPortalInputProcessor::HandleKeyUpEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent)
{
    if (InKeyEvent.GetKey() == EKeys::R)
    {
        bRKeyDown = false;
    }

    return false;
}

bool FPortalInputProcessor::HandleMouseButtonDownEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent)
{
    TSharedPtr<SGraphPanel> GraphPanel = FindGraphPanelUnderMouse(SlateApp, MouseEvent);

    if (GraphPanel.IsValid())
    {
        UEdGraph* InteractedGraph = GraphPanel->GetGraphObj();
        if (InteractedGraph && InteractedGraph->GetSchema() && InteractedGraph->GetSchema()->IsA<UEdGraphSchema_K2>())
        {
            const FVector2f PanelPosition = GraphPanel->GetCachedGeometry().AbsoluteToLocal(MouseEvent.GetScreenSpacePosition());
            GLastInteractedGraph = InteractedGraph;
            GLastInteractedPosition = GraphPanel->PanelCoordToGraphCoord(PanelPosition);
        }
    }

    // Cache the exact graph-space location before a right-click context menu opens. ToolMenus
    // is built after the cursor has moved into the popup, so reconstructing the click location
    // from the menu itself is unreliable.
    if (MouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
    {
        GLastRightClickGraph.Reset();
        GLastRightClickPosition = FVector2f::ZeroVector;

        if (GraphPanel.IsValid())
        {
            UEdGraph* ClickGraph = GraphPanel->GetGraphObj();
            if (ClickGraph && ClickGraph->GetSchema() && ClickGraph->GetSchema()->IsA<UEdGraphSchema_K2>())
            {
                const FVector2f PanelPosition = GraphPanel->GetCachedGeometry().AbsoluteToLocal(MouseEvent.GetScreenSpacePosition());
                GLastRightClickGraph = ClickGraph;
                GLastRightClickPosition = GraphPanel->PanelCoordToGraphCoord(PanelPosition);
            }
        }

        return false;
    }

    if (!bRKeyDown || MouseEvent.GetEffectingButton() != EKeys::LeftMouseButton ||
        !MouseEvent.IsShiftDown() || MouseEvent.IsControlDown() || MouseEvent.IsAltDown())
    {
        return false;
    }

    if (!GraphPanel.IsValid() || !GraphPanel->IsGraphEditable())
    {
        return false;
    }

    UEdGraph* Graph = GraphPanel->GetGraphObj();
    if (!Graph || !Graph->GetSchema() || !Graph->GetSchema()->IsA<UEdGraphSchema_K2>())
    {
        return false;
    }

    const FVector2f PanelPosition = GraphPanel->GetCachedGeometry().AbsoluteToLocal(MouseEvent.GetScreenSpacePosition());
    const FVector2f GraphPosition = GraphPanel->PanelCoordToGraphCoord(PanelPosition);

    if (UK2Node_Knot* Declaration = GetSelectedPortalDeclaration(GraphPanel.ToSharedRef()))
    {
        if (UK2Node_Knot* Output = PortalNativeUtils::CreateUsage(Declaration, &GraphPosition))
        {
            GraphPanel->SelectionManager.SelectSingleNode(Output);
            return true;
        }
        return false;
    }

    if (UK2Node_Knot* Input = PortalNativeUtils::CreatePortalInput(Graph, GraphPosition))
    {
        GraphPanel->SelectionManager.SelectSingleNode(Input);
        FKismetEditorUtilities::BringKismetToFocusAttentionOnObject(Input, true);
        return true;
    }

    return false;
}

