// Copyright Mippithedork 2026, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "K2Node_PortalBase.h"
#include "K2Node_PortalUsage.generated.h"

class FBlueprintActionDatabaseRegistrar;
class UEdGraphPin;
class UK2Node_PortalDeclaration;
class UToolMenu;
class UGraphNodeContextMenuContext;

UCLASS()
class PORTAL_API UK2Node_PortalUsage : public UK2Node_PortalBase
{
    GENERATED_BODY()

public:
    UK2Node_PortalUsage(const FObjectInitializer& ObjectInitializer);

    UPROPERTY()
    TObjectPtr<UK2Node_PortalDeclaration> Declaration;

    UPROPERTY(VisibleAnywhere, Category = "Portal")
    FGuid DeclarationGuid;

    /** Graph identity used to prevent a Usage pasted by itself into another graph from silently rebinding. */
    UPROPERTY()
    FGuid DeclarationGraphGuid;

    /** Last known presentation data remains available when the Declaration is deleted. */
    UPROPERTY()
    FName CachedPortalName;

    UPROPERTY()
    FLinearColor CachedPortalColor = FLinearColor(0.08f, 0.42f, 0.75f, 1.0f);

    virtual void AllocateDefaultPins() override;
    virtual void PostLoad() override;
    virtual void PostPasteNode() override;
    virtual void PostReconstructNode() override;
    virtual void NotifyPinConnectionListChanged(UEdGraphPin* Pin) override;
    virtual void ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override;
    virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
    virtual FText GetTooltipText() const override;
    virtual FLinearColor GetNodeTitleColor() const override;
    virtual bool ShouldDrawNodeAsControlPointOnly(int32& OutInputPinIndex, int32& OutOutputPinIndex) const override;
    virtual UObject* GetJumpTargetForDoubleClick() const override;
    virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
    virtual void GetNodeContextMenuActions(UToolMenu* Menu, UGraphNodeContextMenuContext* Context) const override;

    bool IsDeclarationValid() const;
    UK2Node_PortalDeclaration* ResolveDeclaration(bool bRepairHiddenLink = true);
    void SetDeclaration(UK2Node_PortalDeclaration* InDeclaration, bool bRepairHiddenLink = true);
    void EnsureHiddenLink();
    void CacheDeclarationPresentation();

private:
    UK2Node_PortalDeclaration* FindDeclarationFromHiddenLink() const;
};
