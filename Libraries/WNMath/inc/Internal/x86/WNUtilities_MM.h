// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_MATH_INTERNAL_X86_UTILITIES_MM_H__
#define __WN_MATH_INTERNAL_X86_UTILITIES_MM_H__

namespace WNMath {
    namespace __WNInternal {
        template <WN_UINT32 Dimension>
        struct __WNNumberRegisters_MM_PI32 {
            enum {
                Value = (Dimension / 2)
            };
        };

        template <WN_UINT32 Dimension>
        struct __WNNumberRegisters_MM_PS {
            enum {
                Value = ((Dimension + 1) / 2)
            };
        };
    }
};

#endif // __WN_MATH_INTERNAL_X86_UTILITIES_MM_H__