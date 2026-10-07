// Copyright Mippithedork 2026, Inc. All Rights Reserved.

#include "SPortalNativeKnotNode.h"

#include "EdGraph/EdGraphPin.h"
#include "K2Node_Knot.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "InputCoreTypes.h"
#include "ScopedTransaction.h"
#include "SGraphPin.h"
#include "Styling/AppStyle.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SSpacer.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/SInlineEditableTextBlock.h"
#include "Widgets/Text/STextBlock.h"

void SPortalNativeKnotNode::Construct(const FArguments& InArgs, UK2Node_Knot* InNode)
{
    SGraphNodeK2Default::Construct(SGraphNodeK2Default::FArguments(), InNode);
}

void SPortalNativeKnotNode::UpdateGraphNode()
{
    InputPins.Empty();
    OutputPins.Empty();
    RightNodeBox.Reset();
    LeftNodeBox.Reset();

    SetupErrorReporting();

    SAssignNew(InlineEditableText, SInlineEditableTextBlock)
        .Style(FAppStyle::Get(), "Graph.Node.NodeTitleInlineEditableText")
        .Text(this, &SPortalNativeKnotNode::GetPortalTitle)
        .OnTextCommitted(this, &SPortalNativeKnotNode::OnPortalNameCommitted)
        .IsReadOnly(this, &SPortalNativeKnotNode::IsNameReadOnly)
        .IsSelected(this, &SPortalNativeKnotNode::IsPortalSelectedExclusively)
        .ColorAndOpacity(this, &SPortalNativeKnotNode::GetPortalTextColor);

    this->GetOrAddSlot(ENodeZone::Center)
        .HAlign(HAlign_Center)
        .VAlign(VAlign_Center)
        [
            SNew(SBorder)
            .BorderImage(FAppStyle::GetBrush("Graph.Node.Body"))
            .BorderBackgroundColor(this, &SPortalNativeKnotNode::GetPortalTint)
            .Padding(FMargin(5.0f, 2.0f))
            .ToolTipText(this, &SPortalNativeKnotNode::GetPortalTooltip)
            [
                SNew(SHorizontalBox)

                + SHorizontalBox::Slot()
                .AutoWidth()
                .VAlign(VAlign_Center)
                [
                    SAssignNew(LeftNodeBox, SVerticalBox)
                ]

                + SHorizontalBox::Slot()
                .AutoWidth()
                .VAlign(VAlign_Center)
                .Padding(FMargin(4.0f, 0.0f, 3.0f, 0.0f))
                [
                    SNew(STextBlock)
                    .Text(this, &SPortalNativeKnotNode::GetPortalBadge)
                    .Font(FAppStyle::GetFontStyle("SmallFont"))
                    .ColorAndOpacity(this, &SPortalNativeKnotNode::GetPortalTextColor)
                ]

                + SHorizontalBox::Slot()
                .AutoWidth()
                .VAlign(VAlign_Center)
                [
                    InlineEditableText.ToSharedRef()
                ]

                + SHorizontalBox::Slot()
                .AutoWidth()
                .VAlign(VAlign_Center)
                .Padding(FMargin(4.0f, 0.0f, 0.0f, 0.0f))
                [
                    SNew(STextBlock)
                    .Text(this, &SPortalNativeKnotNode::GetPortalCountText)
                    .Font(FAppStyle::GetFontStyle("SmallFont"))
                    .ColorAndOpacity(FLinearColor(0.75f, 0.75f, 0.75f, 0.9f))
                ]

                + SHorizontalBox::Slot()
                .AutoWidth()
                .VAlign(VAlign_Center)
                [
                    SAssignNew(RightNodeBox, SVerticalBox)
                ]
            ]
        ];

    CreatePinWidgets();
}

void SPortalNativeKnotNode::AddPin(const TSharedRef<SGraphPin>& PinToAdd)
{
    const UEdGraphPin* Pin = PinToAdd->GetPinObj();
    if (!Pin)
    {
        return;
    }

    // The actual native knot pins remain normal and visible in the asset.
    // Portal only suppresses the internal bridge side in Slate. Removing the
    // plugin therefore restores the standard reroute pin and wire automatically.
    if ((IsDeclaration() && Pin->Direction == EGPD_Output) ||
        (IsUsage() && Pin->Direction == EGPD_Input))
    {
        return;
    }

    PinToAdd->SetOwner(SharedThis(this));
    PinToAdd->SetShowLabel(false);

    if (Pin->Direction == EGPD_Input)
    {
        if (LeftNodeBox.IsValid())
        {
            LeftNodeBox->AddSlot()
                .AutoHeight()
                .HAlign(HAlign_Left)
                .VAlign(VAlign_Center)
                [
                    PinToAdd
                ];
        }
        InputPins.Add(PinToAdd);
    }
    else
    {
        if (RightNodeBox.IsValid())
        {
            RightNodeBox->AddSlot()
                .AutoHeight()
                .HAlign(HAlign_Right)
                .VAlign(VAlign_Center)
                [
                    PinToAdd
                ];
        }
        OutputPins.Add(PinToAdd);
    }
}

FReply SPortalNativeKnotNode::OnMouseButtonDoubleClick(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent)
{
    if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton && IsUsage())
    {
        if (UK2Node_Knot* Usage = Cast<UK2Node_Knot>(GraphNode))
        {
            if (UK2Node_Knot* Declaration = PortalNativeUtils::FindDeclarationForUsage(Usage, true))
            {
                FKismetEditorUtilities::BringKismetToFocusAttentionOnObject(Declaration, false);
                return FReply::Handled();
            }
        }
    }

    return SGraphNodeK2Default::OnMouseButtonDoubleClick(InMyGeometry, InMouseEvent);
}

FText SPortalNativeKnotNode::GetPortalTitle() const
{
    const UK2Node_Knot* Knot = Cast<UK2Node_Knot>(GraphNode);
    FName Name = PortalNativeUtils::GetPortalName(Knot);
    if (Name.IsNone())
    {
        Name = FName(TEXT("Portal"));
    }

    if (IsOrphan())
    {
        return FText::Format(NSLOCTEXT("Portal", "MissingPortalTitle", "Missing: {0}"), FText::FromName(Name));
    }
    return FText::FromName(Name);
}

FText SPortalNativeKnotNode::GetPortalBadge() const
{
    if (IsOrphan())
    {
        return FText::FromString(TEXT("!"));
    }
    return IsDeclaration() ? FText::FromString(TEXT("D")) : FText::FromString(TEXT("U"));
}

FText SPortalNativeKnotNode::GetPortalCountText() const
{
    const UK2Node_Knot* Knot = Cast<UK2Node_Knot>(GraphNode);
    if (!Knot || !IsDeclaration())
    {
        return FText::GetEmpty();
    }

    TArray<UK2Node_Knot*> Usages;
    PortalNativeUtils::GetUsages(Knot, Usages);
    return Usages.IsEmpty() ? FText::GetEmpty() : FText::Format(NSLOCTEXT("Portal", "UsageCount", "x{0}"), FText::AsNumber(Usages.Num()));
}

FText SPortalNativeKnotNode::GetPortalTooltip() const
{
    const UK2Node_Knot* Knot = Cast<UK2Node_Knot>(GraphNode);
    if (!Knot)
    {
        return FText::GetEmpty();
    }

    if (IsOrphan())
    {
        return NSLOCTEXT("Portal", "OrphanTooltip", "This Portal Usage has no valid Declaration. Convert it to a regular reroute or reconnect it to a Declaration.");
    }

    if (IsDeclaration())
    {
        TArray<UK2Node_Knot*> Usages;
        PortalNativeUtils::GetUsages(Knot, Usages);
        return FText::Format(NSLOCTEXT("Portal", "DeclarationTooltip", "Portal Declaration. {0} linked Usage(s). The saved node is a native Blueprint reroute."), FText::AsNumber(Usages.Num()));
    }

    return NSLOCTEXT("Portal", "UsageTooltip", "Portal Usage. Double-click to jump to its Declaration. The saved node is a native Blueprint reroute.");
}

FSlateColor SPortalNativeKnotNode::GetPortalTint() const
{
    if (IsOrphan())
    {
        return FLinearColor(0.65f, 0.04f, 0.04f, 1.0f);
    }
    return PortalNativeUtils::GetPortalColor(Cast<UK2Node_Knot>(GraphNode));
}

FSlateColor SPortalNativeKnotNode::GetPortalTextColor() const
{
    return FLinearColor::White;
}

bool SPortalNativeKnotNode::IsDeclaration() const
{
    return PortalNativeUtils::GetRole(Cast<UK2Node_Knot>(GraphNode)) == EPortalKnotRole::Declaration;
}

bool SPortalNativeKnotNode::IsUsage() const
{
    return PortalNativeUtils::GetRole(Cast<UK2Node_Knot>(GraphNode)) == EPortalKnotRole::Usage;
}

bool SPortalNativeKnotNode::IsOrphan() const
{
    const UK2Node_Knot* Knot = Cast<UK2Node_Knot>(GraphNode);
    return Knot && PortalNativeUtils::IsOrphanUsage(Knot);
}

bool SPortalNativeKnotNode::IsNameReadOnly() const
{
    return !IsDeclaration();
}

bool SPortalNativeKnotNode::IsPortalSelectedExclusively() const
{
    return IsSelectedExclusively();
}

void SPortalNativeKnotNode::OnPortalNameCommitted(const FText& NewText, ETextCommit::Type CommitType)
{
    if (!IsDeclaration())
    {
        return;
    }

    UK2Node_Knot* Knot = Cast<UK2Node_Knot>(GraphNode);
    if (!Knot)
    {
        return;
    }

    FString NameString = NewText.ToString().TrimStartAndEnd();
    if (NameString.IsEmpty())
    {
        NameString = TEXT("Portal");
    }

    const FScopedTransaction Transaction(NSLOCTEXT("Portal", "RenamePortal", "Rename Portal"));
    if (UEdGraph* Graph = Knot->GetGraph())
    {
        Graph->Modify();
    }
    PortalNativeUtils::SetPortalName(Knot, FName(*NameString));
}
