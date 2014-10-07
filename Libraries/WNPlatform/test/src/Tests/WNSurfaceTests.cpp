#include "WNPlatform/test/inc/Common.h"

#ifndef __WN_USE_PRECOMPILED_HEADER
    #include "WNPlatform/inc/WNSurfaceManager.h"
    #include "WNPlatform/inc/WNPlatformFactory.h"
    #include "WNConcurrency/inc/WNThread.h"
#endif


TEST(WNSurface, SurfaceCreationTest) {
   WNPlatform::WNSurfaceManager* manager = WNPlatform::WNPlatformFactory::CreateSurfaceManager();
   manager->Initialize();
   WNConcurrency::WNResourcePointer<WNPlatform::WNSurface> sPointer;
   WN_RELEASE_ASSERT(manager->CreateSurface(0, 0, 128, 128, sPointer) == WNPlatform::WNSurfaceManagerReturnCode::eWNOK);
   WNConcurrency::WNThreadSleep(5000); //sleep for 5 seconds
}
