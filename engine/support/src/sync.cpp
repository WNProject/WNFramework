// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNFunctional/inc/WNFunction.h"
#include "WNLogging/inc/WNLog.h"
#include "WNMultiTasking/inc/job_pool.h"
#include "WNScripting/inc/WNFactory.h"
#include "support/inc/string.h"

namespace wn {

namespace support {
namespace sync {

void yield() {
  auto* pool = multi_tasking::job_pool::this_job_pool();

  auto sync = pool->get_signal();

  pool->add_job("yield",
      functional::function<void()>(
          scripting::get_scripting_tls()->_support_allocator, []() {}),
      sync);
  sync.wait_until(1);
}

void register_scripting(memory::allocator*, scripting::engine* _engine) {
  _engine->register_function<decltype(&yield), &yield>("yield");
}

bool resolve_scripting(scripting::engine*) {
  return true;
}

}  // namespace sync
}  // namespace support
}  // namespace wn
