// Copyright Mippithedork 2026, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

class FPortalModule final : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};
