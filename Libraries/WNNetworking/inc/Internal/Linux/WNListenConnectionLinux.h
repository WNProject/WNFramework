////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_NETWORKING_LINUX_LISTEN_CONNECTION_H__
#define __WN_NETWORKING_LINUX_LISTEN_CONNECTION_H__

#include "WNNetworking/inc/Internal/Linux/WNConnectionLinux.h"

namespace WNNetworking {
    class WNInConnectionLinux;

    class WNListenConnectionLinux: public WNConnectionLinux {
    public:
        WNListenConnectionLinux(WNNetworkManager& _manager, WNConnectionType::Type _type, WN_UINT16 _port, WNConnectedCallback _connected);
        virtual WN_FORCE_INLINE ~WNListenConnectionLinux() {}

        WNNetworkManagerReturnCode::Type Initialize();
        WNInConnectionLinux* ReceiveConnection();

    private:
        WN_UINT16 mPort;
        WNConnectedCallback mConnectedCallback;
        WNConnectionType::Type mType;
    };
};

#endif // __WN_NETWORKING_LINUX_LISTEN_CONNECTION_H__
