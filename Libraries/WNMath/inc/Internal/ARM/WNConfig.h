////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_MATH_INTERNAL_ARM_CONFIG_H__
#define __WN_MATH_INTERNAL_ARM_CONFIG_H__

#ifdef __WN_NEON_AVAILABLE
    #include <arm_neon.h>

    #include "WNMath/inc/Internal/ARM/WNUtilities_NEON.h"
#else
    #undef __WN_MATH_EXTENSIONS_ENABLED
#endif

#endif // __WN_MATH_INTERNAL_ARM_CONFIG_H__