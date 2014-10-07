////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_PLATFORM_PLATFORM_FACTORY_H__
#define __WN_PLATFORM_PLATFORM_FACTORY_H__

#include "WNCore/inc/WNBase.h"

namespace WNPlatform {
    class WNSurfaceManager;

    class WNPlatformFactory {
    public:
        static WNSurfaceManager* CreateSurfaceManager();

    private:
        WN_FORCE_INLINE WNPlatformFactory() {}
    };
}

#endif // __WN_PLATFORM_PLATFORM_FACTORY_H__
