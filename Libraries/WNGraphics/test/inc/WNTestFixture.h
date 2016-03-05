// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNGraphics/inc/Internal/WNConfig.h"
#include "WNLogging/inc/WNBufferLogger.h"
#include "WNMemory/inc/WNAllocator.h"
#include "WNTesting/inc/WNTestHarness.h"

namespace wn {
namespace graphics {
namespace testing {

template <typename T>
class test_internal : public T {
public:
  test_internal()
    : T(), m_log(&m_logger), m_logger(&m_buffer), m_buffer(&m_allocator) {}

  static void flush_buffer(void* v, const char* bytes, size_t length,
      const std::vector<WNLogging::WNLogColorElement>&) {
    wn::containers::string* s = static_cast<wn::containers::string*>(v);
    s->append(bytes, length);
  }

  using buffer_logger = WNLogging::WNBufferLogger<flush_buffer>;
  using log_buff = wn::containers::string;

  wn::testing::allocator m_allocator;
  WNLogging::WNLog m_log;
  buffer_logger m_logger;
  log_buff m_buffer;
};

using test = test_internal<::testing::Test>;
template <typename T>
using parameterized_test = test_internal<::testing::TestWithParam<T>>;

#if _WN_GRAPHICS_DEVICE_TYPES_AVAILABLE > 1
#define _WN_DEVICE_IS_MOCKABLE
#endif

}  // namespace testing
}  // namespace graphics
}  // namespace wn