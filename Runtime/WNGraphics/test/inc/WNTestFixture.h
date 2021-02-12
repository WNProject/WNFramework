// Copyright (c) 2021, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _WN_GRAPHICS_TEST_TEST_FIXTURE_H__
#define _WN_GRAPHICS_TEST_TEST_FIXTURE_H__

#include "WNGraphics/inc/Internal/WNConfig.h"
#include "WNLogging/inc/WNBufferLogger.h"
#include "WNMemory/inc/allocator.h"
#include "executable_test/inc/WNTestHarness.h"

namespace wn {
namespace runtime {
namespace graphics {
namespace testing {

template <typename T>
class test_internal : public T {
public:
  test_internal()
    : T(),
      m_static_log(&m_logger),
      m_log(m_static_log.log()),
      m_logger(&m_buffer),
      m_buffer(&m_allocator) {}

  static void flush_buffer(void* v, const char* bytes, size_t length,
      const wn::logging::color_element*, size_t) {
    wn::containers::string* s = static_cast<wn::containers::string*>(v);
    s->append(bytes, length);
  }

  using buffer_logger = logging::buffer_logger<flush_buffer>;
  using log_buff = wn::containers::string;

  wn::testing::allocator m_allocator;
  logging::static_log<> m_static_log;
  logging::log* m_log;
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
}  // namespace runtime
}  // namespace wn

#endif  //  _WN_GRAPHICS_TEST_TEST_FIXTURE_H__
