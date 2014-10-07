////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_PLATFORM_TIME_H__
#define __WN_PLATFORM_TIME_H__

#include "WNCore/inc/WNTypes.h"

typedef WN_UINT32 (*WN_TICK_COUNT_FUNCTION_T)();
typedef WN_UINT64 (*WN_BIG_TICK_COUNT_FUNCTION_T)();

namespace WNPlatform {
    WN_UINT32 WNGetTime();
    WN_UINT64 WNGetBigTime();

    WN_UINT32 WNGetTickCount();
    WN_UINT64 WNGetBigTickCount();
}

#include "WNPlatform/inc/Internal/WNTime.inl"

#endif // __WN_PLATFORM_TIME_H__