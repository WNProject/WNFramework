#include "WNMath/test/inc/Common.h"

TEST(vector2, construction) {
    wn::vector2<wn_int32> vector1;
    wn::vector2<wn_int32> vector2(1);
    wn::vector2<wn_int32> vector3(vector2);
    wn::vector2<wn_int32> vector4(std::move(vector3));
}