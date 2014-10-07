////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "WNNetworking/inc/WNNetworkFactory.h"
#include "WNMemory/inc/WNAllocation.h"

#ifdef _WN_WINDOWS
    #include "WNNetworking/inc/Internal/Windows/WNNetworkManagerWindows.h"
#elif defined _WN_LINUX || defined _WN_ANDROID
    #include "WNNetworking/inc/Internal/Linux/WNNetworkManagerLinux.h"
#endif

using namespace WNNetworking;

WNNetworkManager* WNNetworkFactory::CreateNetworkManager() {
    #ifdef _WN_WINDOWS
        return(WN_NEW WNNetworkManagerWindows());
    #elif defined _WN_LINUX || defined _WN_ANDROID
        return(WN_NEW WNNetworkManagerLinux());
    #endif
}
