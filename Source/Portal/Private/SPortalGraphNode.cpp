// Copyright Mippithedork 2026, Inc. All Rights Reserved.

#include "SPortalGraphNode.h"

#include "EdGraph/EdGraphPin.h"
#include "K2Node_PortalBase.h"
#include "SGraphPin.h"

void SPortalGraphNode::Construct(const FArguments& InArgs, UK2Node_PortalBase* InNode)
{
    SGraphNodeK2Default::Construct(SGraphNodeK2Default::FArguments(), InNode);
}

void SPortalGraphNode::AddPin(const TSharedRef<SGraphPin>& PinToAdd)
{
    const UEdGraphPin* PinObject = PinToAdd->GetPinObj();
    if (PinObject && PinObject->bHidden)
    {
        // Do not add Portal's internal bridge pin to the visible Slate pin
        // arrays. The UEdGraphPin and its connection remain intact for the
        // graph model and compiler.
        return;
    }

    SGraphNodeK2Default::AddPin(PinToAdd);
}
