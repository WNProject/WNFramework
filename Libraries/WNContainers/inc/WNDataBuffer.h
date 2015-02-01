// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_CONTAINERS_DATA_BUFFER_H__
#define __WN_CONTAINERS_DATA_BUFFER_H__

#include "WNCore/inc/WNTypes.h"

namespace WNContainers {
    class WNSerializerBase;

    enum WNDataBufferType {
        eWNReadBinary,
        eWNWriteBinary,
        eWNReadText,
        eWNWriteText
    };

    static const wn_size_t MAX_DATA_WRITE = 1024;

    class WNDataBuffer {
    public:
        virtual wn_bool Serialize(const wn_uint32 _flags, const WNSerializerBase& _serializer) = 0;
        virtual wn_char* ReserveBytes(const wn_size_t _numBytes, wn_size_t& _returnedBytes) = 0;
        virtual WNDataBufferType GetType() = 0;
    };
}

#endif // __WN_CONTAINERS_DATA_BUFFER_H__
