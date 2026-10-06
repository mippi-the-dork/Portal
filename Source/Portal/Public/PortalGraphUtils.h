// Copyright Mippithedork 2026, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class UEdGraph;
class UEdGraphPin;
class UK2Node_PortalDeclaration;
class UK2Node_PortalUsage;

namespace PortalGraphUtils
{
    PORTAL_API UK2Node_PortalDeclaration* FindDeclarationByGuid(const UEdGraph* Graph, const FGuid& Guid, const UK2Node_PortalDeclaration* Ignore = nullptr);
    PORTAL_API void GetUsagesForDeclaration(const UK2Node_PortalDeclaration* Declaration, TArray<UK2Node_PortalUsage*>& OutUsages);
    PORTAL_API bool IsExternalPortalPin(const UK2Node_PortalDeclaration* Declaration, const UEdGraphPin* Pin);
    PORTAL_API bool IsExternalPortalPin(const UK2Node_PortalUsage* Usage, const UEdGraphPin* Pin);
    PORTAL_API void MarkOwningBlueprintModified(const UEdGraph* Graph);
}
