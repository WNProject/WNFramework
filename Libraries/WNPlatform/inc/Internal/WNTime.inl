////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_PLATFORM_INTERNAL_TIME_INL__
#define __WN_PLATFORM_INTERNAL_TIME_INL__

#ifndef __WN_PLATFORM_TIME_H__
    #error "WNTime.inl should never be included directly. Please include WNTime.h instead"
#endif

namespace WNPlatform {
    WN_FORCE_INLINE WN_UINT32 WNGetTime() {
        return(static_cast<WN_UINT32>(WNGetBigTime()));
    }
}

#ifdef _WN_WINDOWS
    #include "WNPlatform/inc/Internal/Windows/WNTime.inl"
#elif defined _WN_LINUX || defined _WN_ANDROID
    #include "WNPlatform/inc/Internal/Linux/WNTime.inl"
#endif

#endif // __WN_PLATFORM_INTERNAL_TIME_INL__