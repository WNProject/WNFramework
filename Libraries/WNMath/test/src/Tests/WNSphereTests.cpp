#include "WNMath/test/inc/Common.h"
#include "WNTesting/inc/WNTestHarness.h"

TEST(sphere, construction) {
    wn::sphere<wn_int32> sphere1;

    const wn::vector3<wn_int32> temp;

    wn::sphere<wn_int32> sphere2(temp, 1);
}