////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_MATH_INTERNAL_X86_GENERAL_OP_NEGATE_H__
#define __WN_MATH_INTERNAL_X86_GENERAL_OP_NEGATE_H__

#ifdef __WN_SSE_AVAILABLE
    #include "WNMath/inc/Internal/x86/WNGeneralOpNegate_SSE.h"
#elif defined __WN_3DNOW_AVAILABLE
    #include "WNMath/inc/Internal/x86/WNGeneralOpNegate_3DNow.h"
#endif

#ifdef __WN_SSE2_AVAILABLE
    #include "WNMath/inc/Internal/x86/WNGeneralOpNegate_SSE2.h"
#endif

#endif // __WN_MATH_INTERNAL_X86_GENERAL_OP_NEGATE_H__