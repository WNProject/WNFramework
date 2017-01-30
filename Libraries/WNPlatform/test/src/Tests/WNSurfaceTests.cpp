// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNTesting/inc/WNTestHarness.h"

#include "WNPlatform/inc/WNPlatformFactory.h"
#include "WNPlatform/inc/WNSurfaceManager.h"

TEST(surface, SurfaceCreationTest) {
  wn::WNSurfaceManager* manager = wn::WNPlatformFactory::CreateSurfaceManager();

  manager->Initialize();

  wn::surface_handle surface;

  EXPECT_EQ(wn::WNSurfaceManagerReturnCode::ok,
      manager->CreateSurface(0, 0, 128, 128, surface));
}
