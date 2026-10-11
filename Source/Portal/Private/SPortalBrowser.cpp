// Copyright Mippithedork 2026, Inc. All Rights Reserved.

#include "SPortalBrowser.h"

#include "EdGraph/EdGraph.h"
#include "EdGraph/EdGraphNode.h"
#include "EdGraph/EdGraphPin.h"
#include "EdGraphSchema_K2.h"
#include "K2Node_Knot.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "PortalInputProcessor.h"
#include "PortalNativeUtils.h"
#include "Styling/AppStyle.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SSearchBox.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SSeparator.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Views/SListView.h"
#include "Widgets/Views/STableRow.h"

#define LOCTEXT_NAMESPACE "SPortalBrowser"

void SPortalBrowser::Construct(const FArguments& InArgs)
{
    ChildSlot
    [
        SNew(SBorder)
        .BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
        .Padding(8.0f)
        [
            SNew(SVerticalBox)

            + SVerticalBox::Slot()
            .AutoHeight()
            [
                SNew(SHorizontalBox)

                + SHorizontalBox::Slot()
                .FillWidth(1.0f)
                .VAlign(VAlign_Center)
                [
                    SNew(STextBlock)
                    .Text(this, &SPortalBrowser::GetGraphLabel)
                    .Font(FAppStyle::GetFontStyle("BoldFont"))
                ]

                + SHorizontalBox::Slot()
                .AutoWidth()
                .Padding(6.0f, 0.0f, 0.0f, 0.0f)
                [
                    SNew(SButton)
                    .Text(LOCTEXT("NewInputButton", "New Input"))
                    .ToolTipText(LOCTEXT("NewInputTooltip", "Create a Portal Input at the last position used in the active Blueprint graph."))
                    .OnClicked(this, &SPortalBrowser::HandleNewInput)
                ]

                + SHorizontalBox::Slot()
                .AutoWidth()
                .Padding(4.0f, 0.0f, 0.0f, 0.0f)
                [
                    SNew(SButton)
                    .Text(LOCTEXT("RefreshButton", "Refresh"))
                    .OnClicked(this, &SPortalBrowser::HandleRefresh)
                ]
            ]

            + SVerticalBox::Slot()
            .AutoHeight()
            .Padding(0.0f, 6.0f, 0.0f, 0.0f)
            [
                SNew(SSearchBox)
                .HintText(LOCTEXT("SearchHint", "Search Portal name or type"))
                .OnTextChanged(this, &SPortalBrowser::HandleSearchChanged)
            ]

            + SVerticalBox::Slot()
            .AutoHeight()
            .Padding(0.0f, 5.0f, 0.0f, 3.0f)
            [
                SNew(STextBlock)
                .Text(this, &SPortalBrowser::GetSummaryText)
                .ColorAndOpacity(FSlateColor::UseSubduedForeground())
            ]

            + SVerticalBox::Slot()
            .FillHeight(1.0f)
            [
                SAssignNew(ListView, SListView<FEntryPtr>)
                .ListItemsSource(&FilteredEntries)
                .OnGenerateRow(this, &SPortalBrowser::GenerateRow)
                .OnMouseButtonDoubleClick(this, &SPortalBrowser::HandleEntryDoubleClicked)
                .SelectionMode(ESelectionMode::Single)
            ]
        ]
    ];

    RefreshEntries(true);
}

void SPortalBrowser::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
    SCompoundWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);

    RefreshAccumulator += InDeltaTime;
    if (RefreshAccumulator >= 0.5f)
    {
        RefreshAccumulator = 0.0f;
        RefreshEntries(false);
    }
}

void SPortalBrowser::RefreshEntries(bool bForce)
{
    const UEdGraph* InteractedGraph = nullptr;
    FVector2f InteractedPosition = FVector2f::ZeroVector;
    if (FPortalInputProcessor::GetLastInteractedGraph(InteractedGraph, InteractedPosition))
    {
        ActiveGraph = const_cast<UEdGraph*>(InteractedGraph);
        LastGraphPosition = InteractedPosition;
    }

    UEdGraph* Graph = ActiveGraph.Get();
    const FString Signature = BuildSignature(Graph);
    if (!bForce && Signature == LastSignature)
    {
        return;
    }
    LastSignature = Signature;

    AllEntries.Reset();
    if (Graph)
    {
        for (UEdGraphNode* Node : Graph->Nodes)
        {
            UK2Node_Knot* Input = Cast<UK2Node_Knot>(Node);
            if (!Input || PortalNativeUtils::GetRole(Input) != EPortalKnotRole::Declaration)
            {
                continue;
            }

            const UEdGraphPin* InputPin = Input->GetInputPin();
            const UEdGraphPin* OutputPin = Input->GetOutputPin();
            if ((InputPin && InputPin->PinType.PinCategory == UEdGraphSchema_K2::PC_Exec) ||
                (OutputPin && OutputPin->PinType.PinCategory == UEdGraphSchema_K2::PC_Exec))
            {
                continue;
            }

            FPortalKnotData Data;
            if (!PortalNativeUtils::ReadPortalData(Input, Data))
            {
                continue;
            }

            TArray<UK2Node_Knot*> Outputs;
            PortalNativeUtils::GetUsages(Input, Outputs);

            FEntryPtr Entry = MakeShared<FPortalBrowserEntry>();
            Entry->Input = Input;
            Entry->FamilyGuid = Data.FamilyGuid;
            Entry->Name = Data.Name;
            Entry->TypeLabel = GetTypeLabel(Input);
            Entry->OutputCount = Outputs.Num();
            AllEntries.Add(Entry);
        }
    }

    AllEntries.Sort([](const FEntryPtr& A, const FEntryPtr& B)
    {
        if (!A.IsValid())
        {
            return false;
        }
        if (!B.IsValid())
        {
            return true;
        }
        return A->Name.ToString() < B->Name.ToString();
    });

    ApplyFilter();
}

void SPortalBrowser::ApplyFilter()
{
    FilteredEntries.Reset();
    const FString Filter = SearchText.ToString().TrimStartAndEnd();

    for (const FEntryPtr& Entry : AllEntries)
    {
        if (!Entry.IsValid())
        {
            continue;
        }

        if (Filter.IsEmpty() ||
            Entry->Name.ToString().Contains(Filter, ESearchCase::IgnoreCase) ||
            Entry->TypeLabel.Contains(Filter, ESearchCase::IgnoreCase))
        {
            FilteredEntries.Add(Entry);
        }
    }

    if (ListView.IsValid())
    {
        ListView->RequestListRefresh();
    }
}

FString SPortalBrowser::BuildSignature(const UEdGraph* Graph) const
{
    if (!Graph)
    {
        return TEXT("NoGraph");
    }

    FString Signature = FString::Printf(TEXT("%p|%s|%d"), Graph, *Graph->GetName(), Graph->Nodes.Num());
    for (UEdGraphNode* Node : Graph->Nodes)
    {
        const UK2Node_Knot* Input = Cast<UK2Node_Knot>(Node);
        if (!Input || PortalNativeUtils::GetRole(Input) != EPortalKnotRole::Declaration)
        {
            continue;
        }

        TArray<UK2Node_Knot*> Outputs;
        PortalNativeUtils::GetUsages(Input, Outputs);
        Signature += FString::Printf(
            TEXT("|%s:%s:%d:%s"),
            *Input->NodeGuid.ToString(EGuidFormats::Digits),
            *PortalNativeUtils::GetPortalName(Input).ToString(),
            Outputs.Num(),
            *GetTypeLabel(Input));
    }
    return Signature;
}

FString SPortalBrowser::GetTypeLabel(const UK2Node_Knot* Input) const
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

    if (UObject* TypeObject = PinType.PinSubCategoryObject.Get())
    {
        return TypeObject->GetName();
    }

    FString Label = PinType.PinCategory.ToString();
    if (!Label.IsEmpty())
    {
        Label[0] = FChar::ToUpper(Label[0]);
    }
    return Label.IsEmpty() ? TEXT("Data") : Label;
}

TSharedRef<ITableRow> SPortalBrowser::GenerateRow(FEntryPtr Entry, const TSharedRef<STableViewBase>& OwnerTable)
{
    TWeakObjectPtr<UK2Node_Knot> WeakInput = Entry.IsValid() ? Entry->Input : TWeakObjectPtr<UK2Node_Knot>();

    return SNew(STableRow<FEntryPtr>, OwnerTable)
        .Padding(FMargin(2.0f, 2.0f))
        [
            SNew(SHorizontalBox)

            + SHorizontalBox::Slot()
            .FillWidth(1.0f)
            .VAlign(VAlign_Center)
            [
                SNew(STextBlock)
                .Text(Entry.IsValid() ? FText::FromName(Entry->Name) : FText::GetEmpty())
                .ToolTipText(LOCTEXT("RowNameTooltip", "Portal Input name. Double-click the row to focus the Input."))
            ]

            + SHorizontalBox::Slot()
            .AutoWidth()
            .VAlign(VAlign_Center)
            .Padding(8.0f, 0.0f)
            [
                SNew(SBox)
                .WidthOverride(110.0f)
                [
                    SNew(STextBlock)
                    .Text(Entry.IsValid() ? FText::FromString(Entry->TypeLabel) : FText::GetEmpty())
                    .ColorAndOpacity(FSlateColor::UseSubduedForeground())
                ]
            ]

            + SHorizontalBox::Slot()
            .AutoWidth()
            .VAlign(VAlign_Center)
            .Padding(4.0f, 0.0f, 8.0f, 0.0f)
            [
                SNew(SBox)
                .WidthOverride(26.0f)
                .HAlign(HAlign_Center)
                [
                    SNew(STextBlock)
                    .Text(Entry.IsValid() ? FText::AsNumber(Entry->OutputCount) : FText::GetEmpty())
                    .ToolTipText(LOCTEXT("OutputCountTooltip", "Current Portal Output count."))
                ]
            ]

            + SHorizontalBox::Slot()
            .AutoWidth()
            .Padding(2.0f, 0.0f)
            [
                SNew(SButton)
                .Text(LOCTEXT("FocusInputButton", "Input"))
                .ToolTipText(LOCTEXT("FocusInputTooltip", "Focus this Portal Input in the Blueprint graph."))
                .OnClicked(this, &SPortalBrowser::HandleFocusInput, WeakInput)
            ]

            + SHorizontalBox::Slot()
            .AutoWidth()
            .Padding(2.0f, 0.0f)
            [
                SNew(SButton)
                .Text(LOCTEXT("NextOutputButton", "Next Output"))
                .ToolTipText(LOCTEXT("NextOutputTooltip", "Cycle through this Portal's Outputs in graph position order."))
                .IsEnabled(Entry.IsValid() && Entry->OutputCount > 0)
                .OnClicked(this, &SPortalBrowser::HandleNextOutput, WeakInput)
            ]

            + SHorizontalBox::Slot()
            .AutoWidth()
            .Padding(2.0f, 0.0f)
            [
                SNew(SButton)
                .Text(LOCTEXT("CreateOutputButton", "+ Output"))
                .ToolTipText(LOCTEXT("CreateOutputTooltip", "Create another Output for this Portal."))
                .OnClicked(this, &SPortalBrowser::HandleCreateOutput, WeakInput)
            ]
        ];
}

void SPortalBrowser::HandleSearchChanged(const FText& NewText)
{
    SearchText = NewText;
    ApplyFilter();
}

void SPortalBrowser::HandleEntryDoubleClicked(FEntryPtr Entry)
{
    if (Entry.IsValid())
    {
        HandleFocusInput(Entry->Input);
    }
}

FReply SPortalBrowser::HandleRefresh()
{
    LastSignature.Reset();
    RefreshEntries(true);
    return FReply::Handled();
}

FReply SPortalBrowser::HandleNewInput()
{
    UEdGraph* Graph = ActiveGraph.Get();
    if (!Graph)
    {
        return FReply::Handled();
    }

    if (UK2Node_Knot* Input = PortalNativeUtils::CreatePortalInput(Graph, LastGraphPosition))
    {
        FKismetEditorUtilities::BringKismetToFocusAttentionOnObject(Input, true);
        LastSignature.Reset();
        RefreshEntries(true);
    }
    return FReply::Handled();
}

FReply SPortalBrowser::HandleFocusInput(TWeakObjectPtr<UK2Node_Knot> WeakInput)
{
    if (UK2Node_Knot* Input = WeakInput.Get())
    {
        FKismetEditorUtilities::BringKismetToFocusAttentionOnObject(Input, false);
    }
    return FReply::Handled();
}

FReply SPortalBrowser::HandleCreateOutput(TWeakObjectPtr<UK2Node_Knot> WeakInput)
{
    if (UK2Node_Knot* Input = WeakInput.Get())
    {
        const UEdGraph* LastGraph = nullptr;
        FVector2f LastPosition = FVector2f::ZeroVector;
        const bool bUseLastPosition = FPortalInputProcessor::GetLastInteractedGraph(LastGraph, LastPosition) && LastGraph == Input->GetGraph();
        if (UK2Node_Knot* Output = PortalNativeUtils::CreateUsage(Input, bUseLastPosition ? &LastPosition : nullptr))
        {
            FKismetEditorUtilities::BringKismetToFocusAttentionOnObject(Output, false);
            LastSignature.Reset();
            RefreshEntries(true);
        }
    }
    return FReply::Handled();
}

FReply SPortalBrowser::HandleNextOutput(TWeakObjectPtr<UK2Node_Knot> WeakInput)
{
    UK2Node_Knot* Input = WeakInput.Get();
    if (!Input)
    {
        return FReply::Handled();
    }

    TArray<UK2Node_Knot*> Outputs;
    PortalNativeUtils::GetUsages(Input, Outputs);
    Outputs.Sort([](const UK2Node_Knot& A, const UK2Node_Knot& B)
    {
        if (A.NodePosX != B.NodePosX)
        {
            return A.NodePosX < B.NodePosX;
        }
        return A.NodePosY < B.NodePosY;
    });

    if (Outputs.IsEmpty())
    {
        return FReply::Handled();
    }

    FPortalKnotData Data;
    PortalNativeUtils::ReadPortalData(Input, Data);
    int32& Index = OutputCycleIndex.FindOrAdd(Data.FamilyGuid);
    Index = FMath::Clamp(Index, 0, Outputs.Num() - 1);

    UK2Node_Knot* Output = Outputs[Index];
    Index = (Index + 1) % Outputs.Num();

    if (Output)
    {
        FKismetEditorUtilities::BringKismetToFocusAttentionOnObject(Output, false);
    }
    return FReply::Handled();
}

FText SPortalBrowser::GetGraphLabel() const
{
    if (const UEdGraph* Graph = ActiveGraph.Get())
    {
        return FText::Format(LOCTEXT("GraphLabel", "Portal Browser  |  {0}"), FText::FromString(Graph->GetName()));
    }
    return LOCTEXT("NoGraphLabel", "Portal Browser  |  No Blueprint graph selected");
}

FText SPortalBrowser::GetSummaryText() const
{
    if (!ActiveGraph.IsValid())
    {
        return LOCTEXT("NoGraphSummary", "Interact with a Blueprint graph, then open or refresh Portal Browser.");
    }

    if (SearchText.IsEmpty())
    {
        return FText::Format(LOCTEXT("Summary", "{0} Portal Input(s)"), FText::AsNumber(AllEntries.Num()));
    }

    return FText::Format(
        LOCTEXT("FilteredSummary", "Showing {0} of {1} Portal Input(s)"),
        FText::AsNumber(FilteredEntries.Num()),
        FText::AsNumber(AllEntries.Num()));
}

#undef LOCTEXT_NAMESPACE
