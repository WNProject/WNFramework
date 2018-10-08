// Copyright (c) 2018, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_ENGINE_SCRIPT_EXPORT_H__
#define __WN_ENGINE_SCRIPT_EXPORT_H__

#include "WNLogging/inc/WNLog.h"
#include "WNScripting/inc/WNEngine.h"

namespace wn {
namespace engine {

void register_scripting(scripting::engine* _engine);

}  // namespace engine
}  // namespace wn

#endif  // __WN_ENGINE_SCRIPT_EXPORT_H__