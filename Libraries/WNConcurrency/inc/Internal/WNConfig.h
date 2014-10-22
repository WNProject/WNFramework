// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_CONCURRENCY_INTERNAL_CONFIG_H__
#define __WN_CONCURRENCY_INTERNAL_CONFIG_H__

#ifdef _WN_LINUX 
    #if !defined _WN_CONCURRENCY_CONFIG_DO_NOT_USE_PTHREAD_SPIN_LOCK
        #define __WN_PTHREAD_SPIN_LOCKS
    #endif
#endif

#endif // __WN_CONCURRENCY_INTERNAL_CONFIG_H__
