// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_CONCURRENCY_CONFIG_H__
#define __WN_CONCURRENCY_CONFIG_H__

// POSIX specific /////////////////////////////////////////////////////////////

// Enables the use of pthreads spinlock implementation
//#define _WN_CONCURRENCY_CONFIG_USE_PTHREAD_SPIN_LOCK

#include "WNThreads/inc/Internal/WNConfig.h"

#endif // __WN_CONCURRENCY_CONFIG_H__