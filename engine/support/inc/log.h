// Copyright (c) 2020, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_SUPPORT_LOG_H_
#define __WN_SUPPORT_LOG_H_

#include "WNLogging/inc/WNLog.h"
#include "WNScripting/inc/WNEngine.h"

namespace wn {
namespace support {
namespace log {
void register_scripting(
    memory::allocator* _allocator, scripting::engine* _engine);
bool resolve_scripting(scripting::engine* _engine);

}  // namespace log
}  // namespace support
}  // namespace wn

#endif  // __WN_SUPPORT_LOG_H_
