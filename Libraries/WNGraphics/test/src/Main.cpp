#include "WNGraphics/inc/WNGraphicsDevice.h"
#include "WNGraphics/inc/WNGraphicsDeviceFactory.h"
#include "WNPlatform/inc/WNSurfaceManager.h"
#include "WNPlatform/inc/WNPlatformFactory.h"

using namespace WNGraphics;
using namespace wn;

wn_int32 wn_main(wn_int32 _argc, wn_char* _argv[]) {
    WN_UNUSED_ARGUMENT(_argc);
    WN_UNUSED_ARGUMENT(_argv);

    WNSurfaceManager* manager = WNPlatformFactory::CreateSurfaceManager();

    manager->Initialize();

    WNGraphicsDevice* device1 = WNGraphicsDeviceFactory::CreateGraphicsDevice(wn_nullptr);
    WNGraphicsDevice* device2 = WNGraphicsDeviceFactory::CreateGraphicsDevice(wn_nullptr);

    device2->Initialize(0, 0);
    device1->Initialize(0, 0);

    wn::memory::intrusive_ptr<surface> sPointer;
    WN_RELEASE_ASSERT(manager->CreateSurface(0, 0, 128, 128, sPointer) == WNSurfaceManagerReturnCode::ok);
    device1->BindSurface(sPointer, wn_false);

    wn::memory::intrusive_ptr<surface> sPointer1;
    WNSurfaceManagerReturnCode::type ret;
    WN_RELEASE_ASSERT((ret = manager->CreateSurface(0, 0, 128, 128, sPointer1)) == WNSurfaceManagerReturnCode::ok || ret == WNSurfaceManagerReturnCode::eWNResourceLimitReached);

    if(sPointer1) {
        device1->BindSurface(sPointer1, wn_false);
    }
    wn::memory::intrusive_ptr<surface> sPointer2;
    WN_RELEASE_ASSERT((ret = manager->CreateSurface(0, 0, 128, 128, sPointer2)) == WNSurfaceManagerReturnCode::ok || ret == WNSurfaceManagerReturnCode::eWNResourceLimitReached);
    if(sPointer2) {
        device2->BindSurface(sPointer2, wn_false);
        device2->SetActiveSurface(sPointer2);
    }
    wn_size_t count = 0;
    wn_bool activeSurfaceFlag = wn_false;

    for (;;) {
        if (activeSurfaceFlag || !sPointer1) {
            device1->SetActiveSurface(sPointer);
        } else {
            device1->SetActiveSurface(sPointer1);
        }

        activeSurfaceFlag = !activeSurfaceFlag;

        ++count;

        wn_float32 color[4];

        color[0] = rand() / static_cast<wn_float32>(RAND_MAX);
        color[1] = rand() / static_cast<wn_float32>(RAND_MAX);
        color[2] = rand() / static_cast<wn_float32>(RAND_MAX);
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

    sPointer = wn::memory::intrusive_ptr<surface>();
    sPointer1 = wn::memory::intrusive_ptr<surface>();
    sPointer2 = wn::memory::intrusive_ptr<surface>();

    device1->Release();
    device2->Release();
    manager->Release();

    wn::memory::destroy(manager);

    return(0);
}
