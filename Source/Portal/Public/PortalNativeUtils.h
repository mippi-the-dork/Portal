// Copyright Mippithedork 2026, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class UBlueprint;
class UEdGraph;
class UEdGraphNode;
class UEdGraphPin;
class UK2Node_Knot;

enum class EPortalKnotRole : uint8
{
    None,
    Declaration,
    Usage
};

struct FPortalKnotData
{
    EPortalKnotRole Role = EPortalKnotRole::None;
    FGuid FamilyGuid;
    FGuid SourceGraphGuid;
    FName Name;
    FLinearColor Color = FLinearColor(0.08f, 0.42f, 0.75f, 1.0f);
};

namespace PortalNativeUtils
{
    PORTAL_API bool ReadPortalData(const UK2Node_Knot* Knot, FPortalKnotData& OutData);
    PORTAL_API bool IsPortalKnot(const UK2Node_Knot* Knot);
    PORTAL_API EPortalKnotRole GetRole(const UK2Node_Knot* Knot);
    PORTAL_API FName GetPortalName(const UK2Node_Knot* Knot);
    PORTAL_API FLinearColor GetPortalColor(const UK2Node_Knot* Knot);

    PORTAL_API void WritePortalData(UK2Node_Knot* Knot, const FPortalKnotData& Data, bool bMarkDirty = true);
    PORTAL_API void ClearPortalData(UK2Node_Knot* Knot, bool bKeepReadableComment = true);
    PORTAL_API void SetPortalName(UK2Node_Knot* Declaration, const FName& NewName);

    PORTAL_API UK2Node_Knot* FindDeclaration(const UEdGraph* Graph, const FGuid& FamilyGuid);
    PORTAL_API UK2Node_Knot* FindDeclarationForUsage(const UK2Node_Knot* Usage, bool bAllowRepair = true);
    PORTAL_API void GetUsages(const UK2Node_Knot* Declaration, TArray<UK2Node_Knot*>& OutUsages);
    PORTAL_API bool IsOrphanUsage(const UK2Node_Knot* Usage);

    PORTAL_API UK2Node_Knot* CreateUsage(UK2Node_Knot* Declaration, const FVector2f* OptionalPosition = nullptr);
    PORTAL_API bool ConvertRerouteToPortal(UK2Node_Knot* Knot);
    PORTAL_API void ConvertPortalFamilyToReroutes(UK2Node_Knot* AnyFamilyNode);

    PORTAL_API bool RepairGraph(UEdGraph* Graph);
    PORTAL_API bool MigrateLegacyGraph(UEdGraph* Graph);
    PORTAL_API bool MigrateAndRepairBlueprint(UBlueprint* Blueprint);
    PORTAL_API void MigrateAndRepairLoadedBlueprints();

    PORTAL_API FString MakeFallbackComment(EPortalKnotRole Role, FName Name);
    PORTAL_API bool ParseFallbackComment(const FString& Comment, EPortalKnotRole& OutRole, FName& OutName);

    PORTAL_API void MarkBlueprintModified(const UEdGraph* Graph, bool bStructural);
}
