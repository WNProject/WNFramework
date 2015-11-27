#include "WNMath/test/inc/Common.h"
#include "WNTesting/inc/WNTestHarness.h"

TEST(vector3, construction) {
    wn::vector3<wn_int32> vector1;
    wn::vector3<wn_int32> vector2(1);
    wn::vector3<wn_int32> vector3(vector2);
    wn::vector3<wn_int32> vector4(std::move(vector3));
}