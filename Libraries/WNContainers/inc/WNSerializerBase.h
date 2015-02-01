// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_CONTAINERS_SERIALIZER_BASE_H__
#define __WN_CONTAINERS_SERIALIZER_BASE_H__

#include "WNCore/inc/WNTypes.h"

namespace WNContainers {
    class WNDataBuffer;

    class WNSerializerBase {
    public:
        virtual WN_SIZE_T Serialize(WNDataBuffer& _buffer, const WN_UINT32 _serializeFlags) const = 0;
    };
}

#endif // __WN_CONTAINERS_SERIALIZER_BASE_H__
