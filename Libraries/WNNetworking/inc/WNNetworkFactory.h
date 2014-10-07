////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_NETWORKING_NETWORK_FACTORY_H__
#define __WN_NETWORKING_NETWORK_FACTORY_H__

#include "WNCore/inc/WNBase.h"

namespace WNNetworking {
    class WNNetworkManager;

    class WNNetworkFactory {
    public:
        static WNNetworkManager* CreateNetworkManager();

    private:
        WN_FORCE_INLINE WNNetworkFactory() {}
    };
}

#endif // __WN_NETWORKING_NETWORK_FACTORY_H__
