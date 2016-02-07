#include "WNMath/test/inc/Common.h"
#include "WNTesting/inc/WNTestHarness.h"

TEST(vector4, construction) {
  wn::vector4<int32_t> vector1;
  wn::vector4<int32_t> vector2(1);
  wn::vector4<int32_t> vector3(vector2);
  wn::vector4<int32_t> vector4(std::move(vector3));
}

TEST(vector4, normalize) {
  wn::vector4<float> vector(1.0f);

  vector.normalize();
}