////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_NETWORKING_INTERNAL_LINUX_OUT_LISTEN_CONNECTION_LINUX_H__
#define __WN_NETWORKING_INTERNAL_LINUX_OUT_LISTEN_CONNECTION_LINUX_H__

#include "WNNetworking/inc/Internal/Linux/WNConnectionLinux.h"

namespace WNNetworking {
    class WNOutConnectionLinux: public WNConnectionLinux {
    public:
        WNOutConnectionLinux(WNNetworkManager& _manager);

        WNNetworkManagerReturnCode::Type Initialize(WNConnectionType::Type& _type, const WN_CHAR* _target, WN_UINT16 _port);
    };
};

#endif // __WN_NETWORKING_INTERNAL_LINUX_OUT_LISTEN_CONNECTION_LINUX_H__
