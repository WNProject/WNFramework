// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_NETWORKING_LINUX_LISTEN_CONNECTION_H__
#define __WN_NETWORKING_LINUX_LISTEN_CONNECTION_H__

#include "WNNetworking/inc/Internal/Linux/WNConnectionLinux.h"

namespace WNNetworking {
    class WNInConnectionLinux;

    class WNListenConnectionLinux: public WNConnectionLinux {
    public:
        WNListenConnectionLinux(WNNetworkManager& _manager, WNConnectionType::type _type, uint16_t _port, WNConnectedCallback _connected);
        virtual WN_FORCE_INLINE ~WNListenConnectionLinux() {}

        WNNetworkManagerReturnCode::type Initialize();
        WNInConnectionLinux* ReceiveConnection();

    private:
        uint16_t mPort;
        WNConnectedCallback mConnectedCallback;
        WNConnectionType::type mType;
    };
};

#endif // __WN_NETWORKING_LINUX_LISTEN_CONNECTION_H__
