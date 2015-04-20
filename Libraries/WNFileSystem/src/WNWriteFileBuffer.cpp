// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNFileSystem/inc/WNWriteFileBuffer.h"
#include "WNMemory/inc/WNAllocation.h"
#include "WNContainers/inc/WNSerializer.h"

WNFileSystem::WNWriteTextFileBuffer::WNWriteTextFileBuffer(const WNFileSystem::WNFileBufferType _type, const wn_size_t _bufferSize) :
    WNFileBuffer(_type),
    mBufferUsage(0),
    mBufferSize(_bufferSize) {
    mCurrentBuffer = wn::memory::heap_allocate<wn_char>(_bufferSize);

    WN_RELEASE_ASSERT(mCurrentBuffer != wn_nullptr);
}

WNFileSystem::WNWriteTextFileBuffer::~WNWriteTextFileBuffer() {
    if (mFile.IsValid()) {
        DumpToFile();

        if (mCurrentBuffer != wn_nullptr) {
            wn::memory::heap_free(mCurrentBuffer);
        }
    }
}

wn_char* WNFileSystem::WNWriteTextFileBuffer::reserve(const wn_size_t _numBytes, wn_size_t& _returnedBytes) {
    if (_numBytes + mBufferUsage > mBufferSize) {
        if (mBufferUsage > 0) {
            if (!DumpToFile()) {
                wn::memory::heap_free(mCurrentBuffer);

                _returnedBytes = 0;

                return(wn_nullptr);
            }
        }

        if (_numBytes > mBufferSize) {
            mCurrentBuffer = wn::memory::heap_reallocate(mCurrentBuffer, _numBytes);

            WN_RELEASE_ASSERT(mCurrentBuffer != wn_nullptr);

            mBufferUsage = 0;
            mBufferSize = _numBytes;
        }
    }

    _returnedBytes = _numBytes;

    return(mCurrentBuffer + mBufferUsage);
}

wn_bool WNFileSystem::WNWriteTextFileBuffer::DumpToFile() {
    if (!mFile.IsValid() || mBufferUsage == 0) {
        return(wn_false);
    }

    const wn_size_t written = mFile.WriteData(mCurrentBuffer, mBufferUsage);

    if (written != mBufferUsage) {
        return(wn_false);
    }

    return(wn_true);
}

wn::containers::data_buffer_type WNFileSystem::WNWriteTextFileBuffer::type() const {
    switch(mType) {
    case WNFileSystem::eWNText:
        return(wn::containers::data_buffer_type::write_text);
    case WNFileSystem::eWNBinary:
        return(wn::containers::data_buffer_type::write_binary);
    }

    return(wn::containers::data_buffer_type::write_text);
}

WNFileSystem::WNFile::WNFileError WNFileSystem::WNWriteTextFileBuffer::SetFile(const wn_char* _fileName) {
    WN_RELEASE_ASSERT_DESC(!mFile.IsValid(), "Error, trying to set a file on a text buffer that already exists");

    return(mFile.OpenFile(_fileName, WNFileSystem::WNFile::eWNFMClobber | WNFileSystem::WNFile::eWNFMExclusive |
                                     WNFileSystem::WNFile::eWNFMWrite | WNFileSystem::WNFile::eWNFMCreate | WNFileSystem::WNFile::eWNFMStream));
}

wn_bool WNFileSystem::WNWriteTextFileBuffer::serialize(const wn::containers::serializer_base& _serializer, const wn_uint32 _flags) {
    mBufferUsage += _serializer.serialize(*this, _flags);

    return(wn_true);
}
