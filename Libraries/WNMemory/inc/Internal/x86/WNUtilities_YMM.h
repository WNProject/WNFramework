////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_MEMORY_INTERNAL_X86_UTILITIES_YMM_H__
#define __WN_MEMORY_INTERNAL_X86_UTILITIES_YMM_H__

#ifndef __WN_YMM_ZEROUPPER
    #ifndef __WN_VEX_AVAILABLE
        #define __WN_YMM_ZEROUPPER() _mm256_zeroupper()
    #else
        #define __WN_YMM_ZEROUPPER()
    #endif
#endif

#endif // __WN_MEMORY_INTERNAL_X86_UTILITIES_YMM_H__