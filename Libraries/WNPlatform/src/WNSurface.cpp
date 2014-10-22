// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNCore/inc/WNBase.h"

#ifdef _WN_DEBUG
    #include "WNPlatform/inc/WNSurface.h"

    #undef WN_INLINE
    #define WN_INLINE

    #undef WN_FORCE_INLINE
    #define WN_FORCE_INLINE

    #include "WNPlatform/inc/Internal/WNSurface.inl"
#endif