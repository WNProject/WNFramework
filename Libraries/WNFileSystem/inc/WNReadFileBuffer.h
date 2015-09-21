// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_FILE_SYSTEM_READ_FILE_BUFFER__
#define __WN_FILE_SYSTEM_READ_FILE_BUFFER__

#include "WNCore/inc/WNTypes.h"
#include "WNFileSystem/inc/WNFile.h"
#include "WNFileSystem/inc/WNFileBuffer.h"

namespace WNFileSystem {
    class WNReadTextFileBuffer : public WNFileBuffer {
    public:
        WNReadTextFileBuffer(const WNFileBufferType _type, const wn_size_t _initialBufferSize = 1024);
        virtual ~WNReadTextFileBuffer();

        virtual wn_bool serialize(const wn::containers::serializer_base& _serializer, const wn_uint32 _flags) override;
        virtual WNFile::WNFileError SetFile(const wn_char* fileName);
        virtual wn_char* reserve(const wn_size_t _numBytes, wn_size_t& _returnedBytes) override;
        virtual wn::containers::data_buffer_type type() const override;

    private:
        WNFile mFile;
        wn_char* mCurrentBuffer;
        wn_char* mSpareBuffer;
        wn_size_t mBufferUsage;
        wn_size_t mBufferPosition;
        wn_size_t mBufferSize;
        wn_bool mEndFile;
    };
}

#endif // __WN_FILE_SYSTEM_READ_FILE_BUFFER__
