// Copyright Mippithedork 2026, Inc. All Rights Reserved.

#pragma once

#include "KismetNodes/SGraphNodeK2Default.h"
#include "PortalNativeUtils.h"
#include "Widgets/DeclarativeSyntaxSupport.h"

class SGraphPin;
class UK2Node_Knot;

class SPortalNativeKnotNode final : public SGraphNodeK2Default
{
public:
    SLATE_BEGIN_ARGS(SPortalNativeKnotNode) {}
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs, UK2Node_Knot* InNode);

    virtual void UpdateGraphNode() override;
    virtual void AddPin(const TSharedRef<SGraphPin>& PinToAdd) override;
    virtual FReply OnMouseButtonDoubleClick(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent) override;

private:
    FText GetPortalTitle() const;
    FText GetPortalIcon() const;
    FText GetPortalCountText() const;
    FText GetPortalTooltip() const;
    FSlateColor GetPortalTint() const;
    FSlateColor GetPortalTextColor() const;
    bool IsDeclaration() const;
    bool IsUsage() const;
    bool IsOrphan() const;
    bool IsNameReadOnly() const;
    bool IsPortalSelectedExclusively() const;
    void OnPortalNameCommitted(const FText& NewText, ETextCommit::Type CommitType);
};
