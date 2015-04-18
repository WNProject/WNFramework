// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_CONTAINERS_GLOBAL_H__
#define __WN_CONTAINERS_GLOBAL_H__

#include "WNCore/inc/WNTypes.h"

namespace WNContainers {
    template <typename type, wn_uint64 ID>
    class WNGlobal {
    public:
        static type mValue;

    private:
        WNGlobal();
        WNGlobal(const WNGlobal&);
        wn_void operator = (const WNGlobal&);
    };
}

#endif // __WN_CONTAINERS_GLOBAL_H__