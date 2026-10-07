// Copyright Mippithedork 2026, Inc. All Rights Reserved.

#include "PortalInputProcessor.h"

#include "EdGraph/EdGraph.h"
#include "EdGraphSchema_K2.h"
#include "Framework/Application/SlateApplication.h"
#include "InputCoreTypes.h"
#include "K2Node_Knot.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Layout/WidgetPath.h"
#include "PortalNativeUtils.h"
#include "SGraphPanel.h"
#include "Widgets/SWindow.h"

namespace
{
    TSharedPtr<SGraphPanel> FindGraphPanelUnderMouse(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent)
    {
        const FWidgetPath WidgetPath = SlateApp.LocateWindowUnderMouse(
            MouseEvent.GetScreenSpacePosition(),
            SlateApp.GetInteractiveTopLevelWindows(),
            false,
            MouseEvent.GetUserIndex());

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

bool FPortalInputProcessor::HandleKeyDownEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent)
{
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
    if (!bRKeyDown || MouseEvent.GetEffectingButton() != EKeys::LeftMouseButton ||
        !MouseEvent.IsShiftDown() || MouseEvent.IsControlDown() || MouseEvent.IsAltDown())
    {
        return false;
    }

    TSharedPtr<SGraphPanel> GraphPanel = FindGraphPanelUnderMouse(SlateApp, MouseEvent);
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
