// Copyright Mippithedork 2026, Inc. All Rights Reserved.

#include "PortalNodeFactory.h"

#include "K2Node_Knot.h"
#include "PortalNativeUtils.h"
#include "SPortalNativeKnotNode.h"

TSharedPtr<SGraphNode> FPortalNodeFactory::CreateNode(UEdGraphNode* Node) const
{
    UK2Node_Knot* Knot = Cast<UK2Node_Knot>(Node);
    if (!Knot || !PortalNativeUtils::IsPortalKnot(Knot))
    {
        return nullptr;
    }

    return SNew(SPortalNativeKnotNode, Knot);
}
