////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_NETWORKING_WINDOWS_IN_CONNECTION_H__
#define __WN_NETWORKING_WINDOWS_IN_CONNECTION_H__

#include "WNNetworking/inc/Internal/Windows/WNConnectionWindows.h"
#include "WNNetworking/inc/WNNetworkManager.h"
#include "WNCore/inc/WNTypes.h"

namespace WNNetworking {
    class WNInConnectionWindows : public WNConnectionWindows {
    public:
        WNInConnectionWindows(WNNetworkManager& _manager);
        virtual WN_FORCE_INLINE ~WNInConnectionWindows() {}

        WNNetworkManagerReturnCode::Type Initialize(SOCKET _listenSocket, WNConnectedCallback _callback);
        WN_BOOL FireCallback();

    private:
        WNConnectedCallback mCallback;
    };
};

#endif // __WN_NETWORKING_WINDOWS_IN_CONNECTION_H__