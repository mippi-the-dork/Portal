// Copyright Mippithedork 2026, Inc. All Rights Reserved.

#pragma once

#include "Containers/Ticker.h"
#include "Modules/ModuleManager.h"

struct FGraphPanelNodeFactory;

class FPortalModule final : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

private:
    void RegisterMenus();
    bool TickRepair(float DeltaTime);

    TSharedPtr<FGraphPanelNodeFactory> NodeFactory;
    FTSTicker::FDelegateHandle RepairTickerHandle;
};
