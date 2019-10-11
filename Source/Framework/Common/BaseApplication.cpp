#include "BaseApplication.h"

DSY_NAMESPACE_BEGIN

int BaseApplication::StartupModule()
{
    m_bQuit = false;

    return 0;
}

void BaseApplication::ShutdownModule()
{

}

void BaseApplication::Tick()
{

}

bool BaseApplication::IsQuit()
{
    return m_bQuit;
}

DSY_NAMESPACE_END