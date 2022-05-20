// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_SUPPORT_SYNC_H__
#define __WN_SUPPORT_SYNC_H__

#include "WNContainers/inc/WNString.h"
#include "WNScripting/inc/WNEngine.h"

namespace wn {
namespace support {
namespace sync {
void register_scripting(
    memory::allocator* _allocator, scripting::engine* _engine);
bool resolve_scripting(scripting::engine* _engine);

}  // namespace sync
}  // namespace support
}  // namespace wn

#endif  //  __WN_SUPPORT_SYNC_H__
