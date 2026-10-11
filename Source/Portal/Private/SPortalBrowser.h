// Copyright Mippithedork 2026, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class SSearchBox;
class STableViewBase;
class UK2Node_Knot;
class UEdGraph;

template <typename ItemType>
class SListView;

class ITableRow;

struct FPortalBrowserEntry
{
    TWeakObjectPtr<UK2Node_Knot> Input;
    FGuid FamilyGuid;
    FName Name;
    FString TypeLabel;
    int32 OutputCount = 0;
};

class SPortalBrowser final : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SPortalBrowser) {}
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);
    virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

private:
    using FEntryPtr = TSharedPtr<FPortalBrowserEntry>;

    void RefreshEntries(bool bForce = false);
    void ApplyFilter();
    FString BuildSignature(const UEdGraph* Graph) const;
    FString GetTypeLabel(const UK2Node_Knot* Input) const;

    TSharedRef<ITableRow> GenerateRow(FEntryPtr Entry, const TSharedRef<STableViewBase>& OwnerTable);
    void HandleSearchChanged(const FText& NewText);
    void HandleEntryDoubleClicked(FEntryPtr Entry);

    FReply HandleRefresh();
    FReply HandleNewInput();
    FReply HandleFocusInput(TWeakObjectPtr<UK2Node_Knot> WeakInput);
    FReply HandleCreateOutput(TWeakObjectPtr<UK2Node_Knot> WeakInput);
    FReply HandleNextOutput(TWeakObjectPtr<UK2Node_Knot> WeakInput);

    FText GetGraphLabel() const;
    FText GetSummaryText() const;

    TWeakObjectPtr<UEdGraph> ActiveGraph;
    FVector2f LastGraphPosition = FVector2f::ZeroVector;
    FString LastSignature;
    FText SearchText;
    float RefreshAccumulator = 0.0f;

    TArray<FEntryPtr> AllEntries;
    TArray<FEntryPtr> FilteredEntries;
    TSharedPtr<SListView<FEntryPtr>> ListView;
    TMap<FGuid, int32> OutputCycleIndex;
};
