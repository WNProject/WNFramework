// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_SUPPORT_SUBPROCESS_H__
#define __WN_SUPPORT_SUBPROCESS_H__

#include "WNContainers/inc/WNString.h"
#include "WNMultiTasking/inc/job_pool.h"
#include "WNScripting/inc/WNEngine.h"
#include "platform_utils/inc/subprocess.h"

namespace wn {
namespace engine_base {
class context;
}
namespace support {

class subprocess {
public:
  static void register_scripting(
      memory::allocator* _allocator, scripting::engine* _engine);
  static bool resolve_scripting(scripting::engine* _engine);

  subprocess(memory::allocator* _allocator, engine_base::context* _context,
      const char* _program, const scripting::slice<const char*> _args);
  ~subprocess();
  int32_t join();
  const char* get_stdout();
  const char* get_stderr();

private:
  multi_tasking::signal_ptr m_signal;
  runtime::platform_utils::subprocess_return m_return;
};

}  // namespace support
}  // namespace wn

#endif  // __WN_SUPPORT_SUBPROCESS_H__
