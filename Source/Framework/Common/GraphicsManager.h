#pragma once

#include "Common/CoreMinimal.h"
#include "Interface/IRuntimeModule.h"

DSY_NAMESPACE_BEGIN

class GraphicsManager : implements IRuntimeModule
{
public:
    virtual ~GraphicsManager() {}
};

DSY_NAMESPACE_END