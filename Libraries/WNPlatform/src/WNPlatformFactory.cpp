////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "WNCore/inc/WNBase.h"
#include "WNPlatform/inc/WNPlatformFactory.h"
#include "WNMemory/inc/WNAllocation.h"

#ifdef _WN_WINDOWS
    #include "WNPlatform/inc/Internal/Windows/WNSurfaceManagerWindows.h"
#elif defined _WN_LINUX
    #include "WNPlatform/inc/Internal/XWindows/WNSurfaceManagerXWindows.h"
#elif defined _WN_ANDROID
    #include "WNPlatform/inc/Internal/Android/WNSurfaceManagerAndroid.h"
#endif

WNPlatform::WNSurfaceManager* WNPlatform::WNPlatformFactory::CreateSurfaceManager() {
    #ifdef _WN_WINDOWS
        return(WN_NEW WNPlatform::WNSurfaceManagerWindows());
    #elif defined _WN_LINUX
        return(WN_NEW WNPlatform::WNSurfaceManagerXWindows());
    #elif defined _WN_ANDROID
        return(WN_NEW WNPlatform::WNSurfaceManagerAndroid());
    #endif
}
