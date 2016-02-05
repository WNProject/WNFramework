// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

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

        WNNetworkManagerReturnCode::type Initialize(SOCKET _listenSocket, WNConnectedCallback _callback);
        bool FireCallback();

    private:
        WNConnectedCallback mCallback;
    };
};

#endif // __WN_NETWORKING_WINDOWS_IN_CONNECTION_H__