////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_CORE_INTERNAL_ARM_BASE_MSVC_H__
#define __WN_CORE_INTERNAL_ARM_BASE_MSVC_H__

#ifdef _WN_CORE_CONFIG_MSVC_ARM_FORCE_VFP4
    #define __WN_VFP4_AVAILABLE
#elif defined _WN_CORE_CONFIG_MSVC_ARM_FORCE_VFP3
    #define __WN_VFP3_AVAILABLE
#endif

#define __WN_NEON_AVAILABLE

#ifdef _M_ARM_FP
    #if _M_ARM_FP >= 30 && _M_ARM_FP <= 39
        #ifndef __WN_VFP3_AVAILABLE
            #define __WN_VFP3_AVAILABLE
        #endif
    #elif _M_ARM_FP >= 40 &&_M_ARM_FP <= 49
        #ifndef __WN_VFP4_AVAILABLE
            #define __WN_VFP4_AVAILABLE
        #endif
    #endif
#endif

#endif // __WN_CORE_INTERNAL_ARM_BASE_MSVC_H__
