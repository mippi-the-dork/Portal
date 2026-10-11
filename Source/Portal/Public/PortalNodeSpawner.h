// Copyright Mippithedork 2026, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BlueprintNodeSpawner.h"
#include "PortalNodeSpawner.generated.h"

class UK2Node_Knot;
class UEdGraph;
class FBlueprintActionFilter;

enum class EPortalNodeSpawnKind : uint8
{
    Input,
    Output
};

UCLASS(Transient)
class PORTAL_API UPortalNodeSpawner final : public UBlueprintNodeSpawner
{
    GENERATED_BODY()

public:
    static UPortalNodeSpawner* CreateInputSpawner();
    static UPortalNodeSpawner* CreateOutputSpawner(UK2Node_Knot* PortalInput);

    virtual UEdGraphNode* Invoke(UEdGraph* ParentGraph, FBindingSet const& Bindings, FVector2D const Location) const override;
    virtual bool IsTemplateNodeFilteredOut(FBlueprintActionFilter const& Filter) const override;

private:
    void ConfigureCommonUi();

    EPortalNodeSpawnKind SpawnKind = EPortalNodeSpawnKind::Input;
    TWeakObjectPtr<UEdGraph> TargetGraph;
    FGuid FamilyGuid;
};
