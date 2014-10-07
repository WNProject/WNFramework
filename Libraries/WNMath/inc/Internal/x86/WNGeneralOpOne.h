////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_MATH_INTERNAL_X86_GENERAL_OP_ONE_H__
#define __WN_MATH_INTERNAL_X86_GENERAL_OP_ONE_H__

#ifdef __WN_SSE_AVAILABLE
    #include "WNMath/inc/Internal/x86/WNGeneralOpOne_SSE.h"
#endif

#ifdef __WN_SSE2_AVAILABLE
    #include "WNMath/inc/Internal/x86/WNGeneralOpOne_SSE2.h"
#endif

#ifdef __WN_AVX_AVAILABLE
    #include "WNMath/inc/Internal/x86/WNGeneralOpOne_AVX.h"
#endif

#endif // __WN_MATH_INTERNAL_X86_GENERAL_OP_ONE_H__