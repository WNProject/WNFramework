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

        const WN_CHAR* GetName();
        WN_BOOL IsValid();
        virtual WN_VOID Invalidate();
        virtual WN_VOID SendBuffer(WNNetworkWriteBuffer& buffer) = 0;

    protected:
        WN_CHAR* mConnectionName;
        WN_BOOL mIsValid;
    };
};

#endif // __WN_NETWORKING_CONNECTION_H__