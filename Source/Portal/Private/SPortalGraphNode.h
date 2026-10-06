// Copyright Mippithedork 2026, Inc. All Rights Reserved.

#pragma once

#include "KismetNodes/SGraphNodeK2Default.h"
#include "Widgets/DeclarativeSyntaxSupport.h"

class SGraphPin;
class UK2Node_PortalBase;

/**
 * Portal's native-looking K2 presentation.
 *
 * Portal keeps its Declaration-to-Usage bridge as a real graph connection,
 * but bridge pins are marked bHidden. Unreal's default K2 presentation still
 * creates widgets for those pins, which exposes the internal bridge visually.
 * This wrapper keeps default K2 presentation while omitting hidden pins from
 * the node's visible pin widget arrays.
 */
class SPortalGraphNode : public SGraphNodeK2Default
{
public:
    SLATE_BEGIN_ARGS(SPortalGraphNode) {}
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs, UK2Node_PortalBase* InNode);

protected:
    virtual void AddPin(const TSharedRef<SGraphPin>& PinToAdd) override;
};
