// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_NETWORKING_CONNECTION_H__
#define __WN_NETWORKING_CONNECTION_H__

#include "WNCore/inc/WNTypes.h"

namespace WNNetworking {
    class WNNetworkWriteBuffer;

    class WNConnection {
    public:
        WNConnection();
        virtual ~WNConnection();

        const wn_char* GetName();
        wn_bool IsValid();
        virtual wn_void Invalidate();
        virtual wn_void SendBuffer(WNNetworkWriteBuffer& buffer) = 0;

    protected:
        wn_char* mConnectionName;
        wn_bool mIsValid;
    };
};

#endif // __WN_NETWORKING_CONNECTION_H__