#include "WNGraphics/inc/WNGraphicsDevice.h"
#include "WNGraphics/inc/WNGraphicsDeviceFactory.h"
#include "WNPlatform/inc/WNSurfaceManager.h"
#include "WNPlatform/inc/WNPlatformFactory.h"

using namespace WNGraphics;
using namespace WNPlatform;
using namespace WNConcurrency;

WN_INT32 WNMain(WN_INT32 _argc, WN_CHAR* _argv[]) {
    WN_UNUSED_ARG(_argc);
    WN_UNUSED_ARG(_argv);

    WNSurfaceManager* manager = WNPlatformFactory::CreateSurfaceManager();

    manager->Initialize();

    WNGraphicsDevice* device1 = WNGraphicsDeviceFactory::CreateGraphicsDevice(WN_NULL);
    WNGraphicsDevice* device2 = WNGraphicsDeviceFactory::CreateGraphicsDevice(WN_NULL);

    device2->Initialize(0, 0);
    device1->Initialize(0, 0);

    WNResourcePointer<WNSurface> sPointer;
    WN_RELEASE_ASSERT(manager->CreateSurface(0, 0, 128, 128, sPointer) == WNSurfaceManagerReturnCode::eWNOK);
    device1->BindSurface(sPointer, WN_FALSE);

    WNResourcePointer<WNSurface> sPointer1;
    WNSurfaceManagerReturnCode::Type ret;
    WN_RELEASE_ASSERT((ret = manager->CreateSurface(0, 0, 128, 128, sPointer1)) == WNSurfaceManagerReturnCode::eWNOK || ret == WNSurfaceManagerReturnCode::eWNResourceLimitReached);

    if(sPointer1) {
        device1->BindSurface(sPointer1, WN_FALSE);
    }
    WNResourcePointer<WNSurface> sPointer2;
    WN_RELEASE_ASSERT((ret = manager->CreateSurface(0, 0, 128, 128, sPointer2)) == WNSurfaceManagerReturnCode::eWNOK || ret == WNSurfaceManagerReturnCode::eWNResourceLimitReached);
    if(sPointer2) {
        device2->BindSurface(sPointer2, WN_FALSE);
        device2->SetActiveSurface(sPointer2);
    }
    WN_SIZE_T count = 0;
    WN_BOOL activeSurfaceFlag = WN_FALSE;

    for (;;) {
        if (activeSurfaceFlag || !sPointer1) {
            device1->SetActiveSurface(sPointer);
        } else {
            device1->SetActiveSurface(sPointer1);
        }

        activeSurfaceFlag = !activeSurfaceFlag;

        ++count;

        WN_FLOAT32 color[4];

        color[0] = rand() / static_cast<WN_FLOAT32>(RAND_MAX);
        color[1] = rand() / static_cast<WN_FLOAT32>(RAND_MAX);
        color[2] = rand() / static_cast<WN_FLOAT32>(RAND_MAX);
        color[3] = 1.0f;

        device1->StartDraw();
        device1->SetClearColor(color);
        device1->Clear();
        device1->EndDraw();
        if(sPointer2) {
            device2->StartDraw();
            device2->SetClearColor(color);
            device2->Clear();
            device2->EndDraw();
        }
        if (count % 600 == 0) {
            break;
        }
    }

    sPointer = WNResourcePointer<WNSurface>();
    sPointer1 = WNResourcePointer<WNSurface>();
    sPointer2 = WNResourcePointer<WNSurface>();

    device1->Release();
    device2->Release();
    manager->Release();


    WN_DELETE(manager);
    return(0);
}
