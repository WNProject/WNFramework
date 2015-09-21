// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_NETWORKING_LINUX_IN_CONNECTION_H__
#define __WN_NETWORKING_LINUX_IN_CONNECTION_H__

#include "WNNetworking/inc/Internal/Linux/WNConnectionLinux.h"

#include <netdb.h>

namespace WNNetworking {
    class WNInConnectionLinux: public WNConnectionLinux {
    public:
        WNInConnectionLinux(WNNetworkManager& _manager);

        wn_bool Initialize(wn_int32 _mySocket, sockaddr& _addr, socklen_t addrLen);
    };
};

#endif // __WN_NETWORKING_LINUX_IN_CONNECTION_H__
