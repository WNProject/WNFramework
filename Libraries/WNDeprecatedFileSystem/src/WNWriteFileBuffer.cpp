// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNDeprecatedFileSystem/inc/WNWriteFileBuffer.h"
#include "WNMemory/inc/WNBasic.h"
#include "WNContainers/inc/WNSerializer.h"

WNFileSystem::WNWriteTextFileBuffer::WNWriteTextFileBuffer(const WNFileSystem::WNFileBufferType _type, const size_t _bufferSize) :
    WNFileBuffer(_type),
    mBufferUsage(0),
    mBufferSize(_bufferSize) {
    mCurrentBuffer = wn::memory::heap_allocate<char>(_bufferSize);

    WN_RELEASE_ASSERT(mCurrentBuffer != nullptr);
}

WNFileSystem::WNWriteTextFileBuffer::~WNWriteTextFileBuffer() {
    if (mFile.IsValid()) {
        DumpToFile();

        if (mCurrentBuffer != nullptr) {
            wn::memory::heap_free(mCurrentBuffer);
        }
    }
}

char* WNFileSystem::WNWriteTextFileBuffer::reserve(const size_t _numBytes, size_t& _returnedBytes) {
    if (_numBytes + mBufferUsage > mBufferSize) {
        if (mBufferUsage > 0) {
            if (!DumpToFile()) {
                wn::memory::heap_free(mCurrentBuffer);

                _returnedBytes = 0;

                return(nullptr);
            }
        }

        if (_numBytes > mBufferSize) {
            mCurrentBuffer = wn::memory::heap_reallocate(mCurrentBuffer, _numBytes);

            WN_RELEASE_ASSERT(mCurrentBuffer != nullptr);

            mBufferUsage = 0;
            mBufferSize = _numBytes;
        }
    }

    _returnedBytes = _numBytes;

    return(mCurrentBuffer + mBufferUsage);
}

bool WNFileSystem::WNWriteTextFileBuffer::DumpToFile() {
    if (!mFile.IsValid() || mBufferUsage == 0) {
        return(false);
    }

    const size_t written = mFile.WriteData(mCurrentBuffer, mBufferUsage);

    if (written != mBufferUsage) {
        return(false);
    }

    return(true);
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

WNFileSystem::WNFile::WNFileError WNFileSystem::WNWriteTextFileBuffer::SetFile(const char* _fileName) {
    WN_RELEASE_ASSERT_DESC(!mFile.IsValid(), "Error, trying to set a file on a text buffer that already exists");

    return(mFile.OpenFile(_fileName, WNFileSystem::WNFile::eWNFMClobber | WNFileSystem::WNFile::eWNFMExclusive |
                                     WNFileSystem::WNFile::eWNFMWrite | WNFileSystem::WNFile::eWNFMCreate | WNFileSystem::WNFile::eWNFMStream));
}

bool WNFileSystem::WNWriteTextFileBuffer::serialize(const wn::containers::serializer_base& _serializer, const uint32_t _flags) {
    mBufferUsage += _serializer.serialize(*this, _flags);

    return(true);
}
