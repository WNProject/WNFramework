////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_NETWORKING_WINDOWS_LISTEN_CONNECTION_H__
#define __WN_NETWORKING_WINDOWS_LISTEN_CONNECTION_H__

#include "WNNetworking/inc/Internal/Windows/WNConnectionWindows.h"
#include "WNNetworking/inc/WNNetworkManager.h"

namespace WNNetworking {
    class WNInConnectionWindows;

    class WNListenConnectionWindows : public WNConnectionWindows {
    public:
        WNListenConnectionWindows(WNNetworkManager& _manager, WNConnectionType::Type _type, WN_UINT16 _port, WNConnectedCallback _connected);
        virtual WN_FORCE_INLINE ~WNListenConnectionWindows() {}

        WNNetworkManagerReturnCode::Type Initialize();
        WNInConnectionWindows* AcceptConnection();

    private:
        WN_UINT16 mPort;
        WNConnectedCallback mConnectedCallback;
        WNConnectionType::Type mType;
        WN_BOOL mInitialized;
    };
};

#endif//__WN_NETWORKING_WINDOWS_LISTEN_CONNECTION_H__
