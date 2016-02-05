#include "WNMath/test/inc/Common.h"
#include "WNTesting/inc/WNTestHarness.h"

TEST(sphere, construction) {
    wn::sphere<int32_t> sphere1;

    const wn::vector3<int32_t> temp;

    wn::sphere<int32_t> sphere2(temp, 1);
}