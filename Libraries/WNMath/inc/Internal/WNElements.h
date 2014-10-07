////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_MATH_INTERNAL_ELEMENTS_H__
#define __WN_MATH_INTERNAL_ELEMENTS_H__

#include "WNCore/inc/WNTypes.h"
#include "WNCore/inc/WNTypeTraits.h"
#include "WNCore/inc/WNAssert.h"

namespace WNMath {
    namespace __WNInternal {
        template <typename Type, WN_UINT32 Dimension, typename = WNCore::WNEnableWhen<WN_TRUE>::Value>
        struct __WNElements {
            WN_STATIC_ASSERT_DESC(Dimension > 0, "Must have a dimension of at least 1");

            Type mValues[Dimension];
        };
    }
}

#include "WNMath/inc/WNConfig.h"

#ifdef __WN_MATH_EXTENSIONS_ENABLED
    #ifdef _WN_X86
        #include "WNMath/inc/Internal/x86/WNElements.h"
    #elif defined _WN_ARM
        #include "WNMath/inc/Internal/ARM/WNElements.h"
    #endif
#endif

#endif // __WN_MATH_INTERNAL_ELEMENTS_H__