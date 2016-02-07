// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_MATH_INTERNAL_GENERAL_TRAITS_H__
#define __WN_MATH_INTERNAL_GENERAL_TRAITS_H__

#include "WNMath/inc/WNConfig.h"

#ifdef __WN_MATH_EXTENSIONS_ENABLED
#ifdef _WN_X86
#include "WNMath/inc/Internal/Extensions/x86/WNGeneralTraits.h"
#endif
#else
#include "WNMath/inc/Internal/Generic/WNGeneralTraits.h"

namespace wn {
namespace internal {
namespace math {
typedef general_traits_generic general_traits;
}
}
}
#endif

#endif  // __WN_MATH_INTERNAL_GENERAL_TRAITS_H__