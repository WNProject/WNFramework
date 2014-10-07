////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_NETWORKING_LINUX_IN_CONNECTION_H__
#define __WN_NETWORKING_LINUX_IN_CONNECTION_H__

#include "WNNetworking/inc/Internal/Linux/WNConnectionLinux.h"

#include <netdb.h>

namespace WNNetworking {
    class WNInConnectionLinux: public WNConnectionLinux {
    public:
        WNInConnectionLinux(WNNetworkManager& _manager);

        WN_BOOL Initialize(WN_INT32 _mySocket, sockaddr& _addr, socklen_t addrLen);
    };
};

#endif // __WN_NETWORKING_LINUX_IN_CONNECTION_H__
