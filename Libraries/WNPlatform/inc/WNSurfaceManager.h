////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_PLATFORM_SURFACE_MANAGER_H__
#define __WN_PLATFORM_SURFACE_MANAGER_H__

#include "WNCore/inc/WNTypes.h"
#include "WNPlatform/inc/WNSurface.h"
#include "WNConcurrency/inc/WNResourcePointer.h"

namespace WNPlatform {
    namespace WNSurfaceManagerReturnCode {
        enum Type {
            #include "WNCore/inc/Internal/WNErrors.inc"
            eWNInitializationFailure,
            eWNResourceLimitReached
        };
    }

    class WNSurfaceManager {
    public:
        virtual WN_FORCE_INLINE ~WNSurfaceManager() {}

        virtual WNSurfaceManagerReturnCode::Type Initialize() = 0;
        virtual WNSurfaceManagerReturnCode::Type Release() = 0;
        virtual WNSurfaceManagerReturnCode::Type CreateSurface(WN_UINT32 _x, WN_UINT32 _y, WN_UINT32 _width, WN_UINT32 _height, WNConcurrency::WNResourcePointer<WNSurface>& _surface) = 0;
    };
}

#endif // __WN_PLATFORM_SURFACE_MANAGER_H__
