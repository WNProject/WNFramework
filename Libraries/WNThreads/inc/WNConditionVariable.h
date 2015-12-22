// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_CONCURRENCY_CONDITION_VARIABLE_H__
#define __WN_CONCURRENCY_CONDITION_VARIABLE_H__

#include "WNCore/inc/WNTypes.h"

#include <condition_variable>

namespace wn {
namespace threads {

typedef std::condition_variable_any condition_variable;

} // namespace threads
} // namespace wn

#endif // __WN_CONCURRENCY_CONDITION_VARIABLE_H__
