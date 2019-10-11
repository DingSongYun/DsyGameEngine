#pragma once

#include "Common/CoreMinimal.h"

DSY_NAMESPACE_BEGIN

interface IRuntimeModule
{
public:
    virtual ~IRuntimeModule() {};
    virtual int StartupModule() = 0;
    virtual void ShutdownModule() = 0;
    virtual void Tick() = 0;
};

DSY_NAMESPACE_END