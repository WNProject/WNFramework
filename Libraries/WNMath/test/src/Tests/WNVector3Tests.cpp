#include "WNMath/test/inc/Common.h"
#include "WNTesting/inc/WNTestHarness.h"

TEST(vector3, construction) {
    wn::vector3<int32_t> vector1;
    wn::vector3<int32_t> vector2(1);
    wn::vector3<int32_t> vector3(vector2);
    wn::vector3<int32_t> vector4(std::move(vector3));
}