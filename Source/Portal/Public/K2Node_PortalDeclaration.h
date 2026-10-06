// Copyright Mippithedork 2026, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "K2Node_PortalBase.h"
#include "K2Node_PortalDeclaration.generated.h"

class FBlueprintActionDatabaseRegistrar;
class INameValidatorInterface;
class UEdGraph;
class UEdGraphPin;
class UK2Node_PortalUsage;
class UToolMenu;
class UGraphNodeContextMenuContext;

UCLASS()
class PORTAL_API UK2Node_PortalDeclaration : public UK2Node_PortalBase
{
    GENERATED_BODY()

public:
    UK2Node_PortalDeclaration(const FObjectInitializer& ObjectInitializer);

    UPROPERTY(VisibleAnywhere, Category = "Portal")
    FGuid PortalGuid;

    UPROPERTY(EditAnywhere, Category = "Portal")
    FName PortalName;

    UPROPERTY(EditAnywhere, Category = "Portal")
    FLinearColor PortalColor;

    virtual void AllocateDefaultPins() override;
    virtual void PostPlacedNewNode() override;
    virtual void PostLoad() override;
    virtual void PostPasteNode() override;
    virtual void PostReconstructNode() override;
    virtual void NotifyPinConnectionListChanged(UEdGraphPin* Pin) override;
    virtual void OnRenameNode(const FString& NewName) override;
    virtual TSharedPtr<INameValidatorInterface> MakeNameValidator() const override;
    virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
    virtual FText GetTooltipText() const override;
    virtual FText GetMenuCategory() const override;
    virtual FLinearColor GetNodeTitleColor() const override;
    virtual bool ShouldDrawNodeAsControlPointOnly(int32& OutInputPinIndex, int32& OutOutputPinIndex) const override;
    virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
    virtual void GetNodeContextMenuActions(UToolMenu* Menu, UGraphNodeContextMenuContext* Context) const override;

    bool IsPortalIdentityValid() const;
    void EnsurePortalIdentity(bool bForceNewGuid = false);
    void EnsureUniquePortalName();
    void RefreshPortalFamilyType();
    void RefreshLinkedUsages();

    UK2Node_PortalUsage* CreateUsageNode();

private:
    bool bRefreshingFamily = false;
};
