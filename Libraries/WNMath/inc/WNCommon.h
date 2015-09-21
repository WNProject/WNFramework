// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MATH_COMMON_H__
#define __WN_MATH_COMMON_H__

namespace wn {
    enum class snap_direction {
        nearest,
        down,
        up,
        truncate
    };

    typedef snap_direction snap_direction_type;

    namespace WNFieldOfView {
        struct WNHorizontal {};
        struct WNVertical {};
    };
}

#endif // __WN_MATH_COMMON_H__