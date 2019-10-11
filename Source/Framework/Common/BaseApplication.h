#pragma once

#include "Common/CoreMinimal.h"
#include "Interface/IApplication.h"

DSY_NAMESPACE_BEGIN

class BaseApplication : implements IApplication
{
public:
    virtual int StartupModule() override;
    virtual void ShutdownModule() override;
    virtual void Tick() override;
    virtual bool IsQuit() override;

protected:
    bool m_bQuit;
};

DSY_NAMESPACE_END