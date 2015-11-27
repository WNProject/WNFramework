#include "WNMath/test/inc/Common.h"
#include "WNTesting/inc/WNTestHarness.h"

TEST(vector4, construction) {
    wn::vector4<wn_int32> vector1;
    wn::vector4<wn_int32> vector2(1);
    wn::vector4<wn_int32> vector3(vector2);
    wn::vector4<wn_int32> vector4(std::move(vector3));
}

TEST(vector4, normalize) {
    wn::vector4<wn_float32> vector(1.0f);

    vector.normalize();
}