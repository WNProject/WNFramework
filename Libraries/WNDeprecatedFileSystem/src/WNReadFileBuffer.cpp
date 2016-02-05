// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNDeprecatedFileSystem/inc/WNReadFileBuffer.h"
#include "WNMemory/inc/WNBasic.h"
#include "WNContainers/inc/WNSerializer.h"

WNFileSystem::WNReadTextFileBuffer::WNReadTextFileBuffer(const WNFileSystem::WNFileBufferType _type, const size_t _bufferSize) :
    WNFileBuffer(_type),
    mBufferUsage(0),
    mBufferSize(_bufferSize),
    mBufferPosition(0),
    mEndFile(false) {
    mCurrentBuffer = wn::memory::heap_allocate<char>(_bufferSize);

    WN_RELEASE_ASSERT(mCurrentBuffer != nullptr);
}

WNFileSystem::WNReadTextFileBuffer::~WNReadTextFileBuffer() {
    if (mFile.IsValid()) {
        if (mCurrentBuffer != nullptr) {
            wn::memory::heap_free(mCurrentBuffer);
        }
    }
}

char* WNFileSystem::WNReadTextFileBuffer::reserve(const size_t _numBytes, size_t& _returnedBytes) {
    if (_numBytes + mBufferPosition > mBufferSize) {
        if (_numBytes > mBufferSize) {
            WN_DEBUG_ASSERT(mBufferPosition <= mBufferUsage);

            mSpareBuffer = wn::memory::heap_reallocate(mSpareBuffer, _numBytes);

            wn::memory::memcpy(mSpareBuffer, mCurrentBuffer + mBufferPosition, mBufferUsage - mBufferPosition);

            mCurrentBuffer = mSpareBuffer;
            mSpareBuffer = wn::memory::heap_reallocate(mCurrentBuffer, _numBytes);
            mBufferSize = _numBytes;
            mBufferUsage -= mBufferPosition;
            mBufferPosition = 0;
        } else {
            WN_DEBUG_ASSERT(mBufferPosition <= mBufferUsage);

            wn::memory::memcpy(mSpareBuffer, mCurrentBuffer + mBufferPosition, mBufferUsage - mBufferPosition);

            char* currentBuffer = mCurrentBuffer;

            mCurrentBuffer = mSpareBuffer;
            mSpareBuffer = currentBuffer;
            mBufferUsage -= mBufferPosition;
            mBufferPosition = 0;
        }
    }

    size_t haveData = mBufferUsage - mBufferPosition;

    if (!mEndFile && haveData < _numBytes) {
        const size_t bytesRead = mFile.ReadData(mCurrentBuffer + mBufferUsage, _numBytes - haveData);

        if (bytesRead == 0) {
            mEndFile = true;
        }

        mBufferUsage += bytesRead;
        haveData += bytesRead;
    }

    _returnedBytes = haveData > _numBytes? _numBytes : haveData;

    return(mCurrentBuffer + mBufferPosition);
}

wn::containers::data_buffer_type WNFileSystem::WNReadTextFileBuffer::type() const {
    switch(mType) {
    case WNFileSystem::eWNText:
        return(wn::containers::data_buffer_type::read_text);
    case WNFileSystem::eWNBinary:
        return(wn::containers::data_buffer_type::read_binary);
    }

    return(wn::containers::data_buffer_type::read_text);
}

WNFileSystem::WNFile::WNFileError WNFileSystem::WNReadTextFileBuffer::SetFile(const char* _fileName) {
    mEndFile = false;

    WN_RELEASE_ASSERT_DESC(!mFile.IsValid(), "Error, trying to set a file on a text buffer that already exists");

    return(mFile.OpenFile(_fileName, WNFileSystem::WNFile::eWNFMExclusive | WNFileSystem::WNFile::eWNFMRead | WNFileSystem::WNFile::eWNFMStream));
}

bool WNFileSystem::WNReadTextFileBuffer::serialize(const wn::containers::serializer_base& _serializer, const uint32_t _flags) {
    mBufferPosition += _serializer.serialize(*this, _flags);

    return(true);
}
