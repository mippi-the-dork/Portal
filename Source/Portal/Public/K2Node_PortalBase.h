// Copyright Mippithedork 2026, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "K2Node.h"
#include "K2Node_PortalBase.generated.h"

class SGraphNode;

UCLASS(Abstract)
class PORTAL_API UK2Node_PortalBase : public UK2Node
{
    GENERATED_BODY()

public:
    UK2Node_PortalBase(const FObjectInitializer& ObjectInitializer);

    virtual void AllocateDefaultPins() override;
    virtual TSharedPtr<SGraphNode> CreateVisualWidget() override;
    virtual void ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override;
    virtual bool IsNodeSafeToIgnore() const override;
    virtual bool IsCompilerRelevant() const override { return false; }
    virtual bool CanSplitPin(const UEdGraphPin* Pin) const override;
    virtual bool ShouldOverridePinNames() const override;
    virtual FText GetPinNameOverride(const UEdGraphPin& Pin) const override;
    virtual UEdGraphPin* GetPassThroughPin(const UEdGraphPin* FromPin) const override;
    virtual bool IsNodePure() const override { return true; }
    virtual bool ShouldDrawCompact() const override { return true; }
    virtual FText GetCompactNodeTitle() const override { return GetNodeTitle(ENodeTitleType::FullTitle); }
    virtual bool ShowPaletteIconOnNode() const override { return false; }
    virtual int32 GetNodeRefreshPriority() const override { return EBaseNodeRefreshPriority::Low_UsesDependentWildcard; }

    UEdGraphPin* GetInputPin() const;
    UEdGraphPin* GetOutputPin() const;
};
