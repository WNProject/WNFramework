// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_FILE_SYSTEM_FILE_BUFFER_H__
#define __WN_FILE_SYSTEM_FILE_BUFFER_H__

#include "WNContainers/inc/WNDataBuffer.h"

namespace WNFileSystem {
    enum WNFileBufferType {
        eWNBinary,
        eWNText
    };

    class WNFileBuffer : public wn::containers::data_buffer {
    public:
        WNFileBuffer(WNFileBufferType _type);

    protected:
        WNFileBufferType mType;
    };
 }

#endif // __WN_FILE_SYSTEM_FILE_BUFFER_H__