// Copyright (c) 2018, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MULTI_TASKING_WINDOWS_INTERNAL_THREAD_FUNCTIONS_H__
#define __WN_MULTI_TASKING_WINDOWS_INTERNAL_THREAD_FUNCTIONS_H__

#include "WNCore/inc/WNBase.h"
#include "WNMultiTasking/inc/call_once.h"

#include <chrono>

namespace wn {
namespace multi_tasking {
namespace this_thread {
namespace internal {

WN_INLINE void yield() {
  static once_flag once;
  static bool multi_threaded = false;
  static const auto mutli_threaded_test = [](bool* multi_threaded) {
    SYSTEM_INFO sysInfo = {0};

    ::GetSystemInfo(&sysInfo);

    *multi_threaded = sysInfo.dwNumberOfProcessors > 1;
  };

  call_once(once, mutli_threaded_test, &multi_threaded);

  multi_threaded ? ::YieldProcessor() : ::SwitchToThread();
}

template <typename Representation, typename Period>
WN_INLINE void sleep_for(
    const std::chrono::duration<Representation, Period>& _duration) {
  ::Sleep(static_cast<DWORD>(
      std::chrono::duration_cast<std::chrono::milliseconds>(_duration)
          .count()));
}

}  // namespace internal
}  // namespace this_thread
}  // namespace multi_tasking
}  // namespace wn

#endif  // __WN_MULTI_TASKING_WINDOWS_INTERNAL_THREAD_FUNCTIONS_H__
