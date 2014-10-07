////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_NETWORKING_INTERNAL_WINDOWS_BUFFER_RESOURCE_INL__
#define __WN_NETWORKING_INTERNAL_WINDOWS_BUFFER_RESOURCE_INL__

#include "WNCore/inc/WNBase.h"

namespace WNNetworking {
    WN_FORCE_INLINE const WSABUF* WNBufferResource::GetWriteWinBuf() const {
        return(&mWinBuf);
    }

    WN_FORCE_INLINE WSABUF* WNBufferResource::GetReadWinBuf() {
        return(&mWinBuf);
    }
}

#endif // __WN_NETWORKING_INTERNAL_WINDOWS_BUFFER_RESOURCE_INL__