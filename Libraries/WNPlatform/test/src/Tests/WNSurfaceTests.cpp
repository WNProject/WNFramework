#include "WNPlatform/test/inc/Common.h"

TEST(surface, SurfaceCreationTest) {
   wn::WNSurfaceManager* manager = wn::WNPlatformFactory::CreateSurfaceManager();

   manager->Initialize();

   wn::surface_handle surface;

   ASSERT_EQ(manager->CreateSurface(0, 0, 128, 128, surface), wn::WNSurfaceManagerReturnCode::ok);
}
