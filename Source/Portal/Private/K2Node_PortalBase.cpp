// Copyright Mippithedork 2026, Inc. All Rights Reserved.

#include "K2Node_PortalBase.h"

#include "EdGraph/EdGraphPin.h"
#include "EdGraphSchema_K2.h"
#include "SPortalGraphNode.h"

UK2Node_PortalBase::UK2Node_PortalBase(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
}

TSharedPtr<SGraphNode> UK2Node_PortalBase::CreateVisualWidget()
{
    return SNew(SPortalGraphNode, this);
}

void UK2Node_PortalBase::AllocateDefaultPins()
{
    UEdGraphPin* InputPin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Wildcard, TEXT("InputPin"));
    InputPin->bDefaultValueIsIgnored = true;

    UEdGraphPin* OutputPin = CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Wildcard, TEXT("OutputPin"));
    OutputPin->bDefaultValueIsIgnored = true;
}

void UK2Node_PortalBase::ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
    Super::ExpandNode(CompilerContext, SourceGraph);

    const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();
    K2Schema->CombineTwoPinNetsAndRemoveOldPins(GetInputPin(), GetOutputPin());
}

bool UK2Node_PortalBase::IsNodeSafeToIgnore() const
{
    return true;
}

bool UK2Node_PortalBase::CanSplitPin(const UEdGraphPin* Pin) const
{
    return false;
}

bool UK2Node_PortalBase::ShouldOverridePinNames() const
{
    return true;
}

FText UK2Node_PortalBase::GetPinNameOverride(const UEdGraphPin& Pin) const
{
    return FText::GetEmpty();
}

UEdGraphPin* UK2Node_PortalBase::GetPassThroughPin(const UEdGraphPin* FromPin) const
{
    if (FromPin && Pins.Contains(FromPin) && Pins.Num() >= 2)
    {
        return FromPin == Pins[0] ? Pins[1] : Pins[0];
    }

    return nullptr;
}

UEdGraphPin* UK2Node_PortalBase::GetInputPin() const
{
    return Pins.IsValidIndex(0) ? Pins[0] : nullptr;
}

UEdGraphPin* UK2Node_PortalBase::GetOutputPin() const
{
    return Pins.IsValidIndex(1) ? Pins[1] : nullptr;
}
