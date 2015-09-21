// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_NETWORKING_INTERNAL_LINUX_OUT_LISTEN_CONNECTION_LINUX_H__
#define __WN_NETWORKING_INTERNAL_LINUX_OUT_LISTEN_CONNECTION_LINUX_H__

#include "WNNetworking/inc/Internal/Linux/WNConnectionLinux.h"

namespace WNNetworking {
    class WNOutConnectionLinux: public WNConnectionLinux {
    public:
        WNOutConnectionLinux(WNNetworkManager& _manager);

        WNNetworkManagerReturnCode::type Initialize(WNConnectionType::type& _type, const wn_char* _target, wn_uint16 _port);
    };
};

#endif // __WN_NETWORKING_INTERNAL_LINUX_OUT_LISTEN_CONNECTION_LINUX_H__
