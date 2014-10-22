// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MATH_INTERNAL_ARM_UTILITIES_NEON_H__
#define __WN_MATH_INTERNAL_ARM_UTILITIES_NEON_H__

namespace WNMath {
    namespace __WNInternal {
        template <WN_UINT32 Dimension>
        struct __WNNumberRegisters_NEON_32_D {
            enum {
                Value = ((Dimension + 1) / 2)
            };
        };

        template <WN_UINT32 Dimension>
        struct __WNNumberRegisters_NEON_32_Q {
            enum {
                Value = (Dimension - ((Dimension / 4) * 4)) >= 3 ? ((Dimension + 3) / 4) : (Dimension / 4)
            };
        };
    }
};

#endif // __WN_MATH_INTERNAL_ARM_UTILITIES_NEON_H__