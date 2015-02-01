// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_NETWORKING_WINDOWS_OUT_CONNECTION_H__
#define __WN_NETWORKING_WINDOWS_OUT_CONNECTION_H__

#include "WNNetworking/inc/Internal/Windows/WNConnectionWindows.h"
#include "WNNetworking/inc/WNNetworkManager.h"
#include "WNCore/inc/WNTypes.h"

namespace WNNetworking {
    class WNOutConnectionWindows : public WNConnectionWindows {
    public:
        WNOutConnectionWindows(WNNetworkManager& _manager);
        virtual WN_FORCE_INLINE ~WNOutConnectionWindows () {}

        WNNetworkManagerReturnCode::type Initialize(WNConnectionType::type& _type, const wn_char* _target, wn_uint16 _port);

    private:
        WNConnectedCallback mCallback;
    };
};

#endif // __WN_NETWORKING_WINDOWS_OUT_CONNECTION_H__
