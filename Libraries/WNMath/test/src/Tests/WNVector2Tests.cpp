#include "WNMath/test/inc/Common.h"
#include "WNTesting/inc/WNTestHarness.h"

TEST(vector2, construction) {
    wn::vector2<int32_t> vector1;
    wn::vector2<int32_t> vector2(1);
    wn::vector2<int32_t> vector3(vector2);
    wn::vector2<int32_t> vector4(std::move(vector3));
}