#include <stdio.h>
#include "CoreMinimal.h"
#include "Interface/IApplication.h"

DSY_NAMESPACE_BEGIN
extern IApplication* GApp;
DSY_NAMESPACE_END

USE_DSY_NAMESPACE

int main()
{
    int ret;

    printf("lalala");
    if ((ret = GApp->StartupModule()) != 0)
    {
        printf("App Start failed, Exit!!!");
        return ret;
    }

    while(!GApp->IsQuit())
    {
        GApp->Tick();
    }

    GApp->ShutdownModule();

    return 0;
}