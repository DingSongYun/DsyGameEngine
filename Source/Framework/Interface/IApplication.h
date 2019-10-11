#pragma once

#include "Common/CoreMinimal.h"
#include "IRuntimeModule.h"

DSY_NAMESPACE_BEGIN

interface IApplication : implements IRuntimeModule
{
public:
    virtual int StartupModule() override = 0;
    virtual void ShutdownModule() override = 0;
    virtual void Tick() override = 0;

    virtual bool IsQuit() = 0;
};

DSY_NAMESPACE_END