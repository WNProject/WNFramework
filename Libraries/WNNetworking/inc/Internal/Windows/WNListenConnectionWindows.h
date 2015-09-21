// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_NETWORKING_WINDOWS_LISTEN_CONNECTION_H__
#define __WN_NETWORKING_WINDOWS_LISTEN_CONNECTION_H__

#include "WNNetworking/inc/Internal/Windows/WNConnectionWindows.h"
#include "WNNetworking/inc/WNNetworkManager.h"

namespace WNNetworking {
    class WNInConnectionWindows;

    class WNListenConnectionWindows : public WNConnectionWindows {
    public:
        WNListenConnectionWindows(WNNetworkManager& _manager, WNConnectionType::type _type, wn_uint16 _port, WNConnectedCallback _connected);
        virtual WN_FORCE_INLINE ~WNListenConnectionWindows() {}

        WNNetworkManagerReturnCode::type Initialize();
        WNInConnectionWindows* AcceptConnection();

    private:
        wn_uint16 mPort;
        WNConnectedCallback mConnectedCallback;
        WNConnectionType::type mType;
        wn_bool mInitialized;
    };
};

#endif//__WN_NETWORKING_WINDOWS_LISTEN_CONNECTION_H__
