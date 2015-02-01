// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_CONCURRENCY_CONDITION_VARIABLE_H__
#define __WN_CONCURRENCY_CONDITION_VARIABLE_H__

#include "WNCore/inc/WNBase.h"

#include <condition_variable>

namespace wn {
    typedef std::condition_variable_any condition_variable;
}

#endif // __WN_CONCURRENCY_CONDITION_VARIABLE_H__