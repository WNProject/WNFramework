// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNFileSystem/inc/WNReadFileBuffer.h"
#include "WNMemory/inc/WNManipulation.h"
#include "WNMemory/inc/WNAllocation.h"
#include "WNContainers/inc/WNSerializer.h"

WNFileSystem::WNReadTextFileBuffer::WNReadTextFileBuffer(const WNFileSystem::WNFileBufferType _type, const wn_size_t _bufferSize) :
    WNFileBuffer(_type),
    mBufferUsage(0),
    mBufferSize(_bufferSize),
    mBufferPosition(0),
    mEndFile(wn_false) {
    mCurrentBuffer = wn::malloc<wn_char>(_bufferSize);

    WN_RELEASE_ASSERT(mCurrentBuffer != wn_nullptr);
}

WNFileSystem::WNReadTextFileBuffer::~WNReadTextFileBuffer() {
    if (mFile.IsValid()) {
        if (mCurrentBuffer != wn_nullptr) {
            wn::free<wn_char>(mCurrentBuffer);
        }
    }
}

wn_char* WNFileSystem::WNReadTextFileBuffer::ReserveBytes(const wn_size_t _numBytes, wn_size_t& _returnedBytes) {
    if (_numBytes + mBufferPosition > mBufferSize) {
        if (_numBytes > mBufferSize) {
            WN_DEBUG_ASSERT(mBufferPosition <= mBufferUsage);

            mSpareBuffer = wn::realloc<wn_char>(mSpareBuffer, _numBytes);

            WNMemory::WNMemCpyT<wn_char>(mSpareBuffer, mCurrentBuffer + mBufferPosition, mBufferUsage - mBufferPosition);

            mCurrentBuffer = mSpareBuffer;
            mSpareBuffer = wn::realloc<wn_char>(mCurrentBuffer, _numBytes);
            mBufferSize = _numBytes;
            mBufferUsage -= mBufferPosition;
            mBufferPosition = 0;
        } else {
            WN_DEBUG_ASSERT(mBufferPosition <= mBufferUsage);

            WNMemory::WNMemCpyT<wn_char>(mSpareBuffer, mCurrentBuffer + mBufferPosition, mBufferUsage - mBufferPosition);

            wn_char* currentBuffer = mCurrentBuffer;

            mCurrentBuffer = mSpareBuffer;
            mSpareBuffer = currentBuffer;
            mBufferUsage -= mBufferPosition;
            mBufferPosition = 0;
        }
    }

    wn_size_t haveData = mBufferUsage - mBufferPosition;

    if (!mEndFile && haveData < _numBytes) {
        const wn_size_t bytesRead = mFile.ReadData(mCurrentBuffer + mBufferUsage, _numBytes - haveData);

        if (bytesRead == 0) {
            mEndFile = wn_true;
        }

        mBufferUsage += bytesRead;
        haveData += bytesRead;
    }

    _returnedBytes = haveData > _numBytes? _numBytes : haveData;

    return(mCurrentBuffer + mBufferPosition);
}

WNContainers::WNDataBufferType WNFileSystem::WNReadTextFileBuffer::GetType() {
    switch(mType) {
    case WNFileSystem::eWNText:
        return(WNContainers::eWNReadText);
    case WNFileSystem::eWNBinary:
        return(WNContainers::eWNReadBinary);
    }

    return(WNContainers::eWNReadText);
}

WNFileSystem::WNFile::WNFileError WNFileSystem::WNReadTextFileBuffer::SetFile(const wn_char* _fileName) {
    mEndFile = wn_false;

    WN_RELEASE_ASSERT_DESC(!mFile.IsValid(), "Error, trying to set a file on a text buffer that already exists");

    return(mFile.OpenFile(_fileName, WNFileSystem::WNFile::eWNFMExclusive | WNFileSystem::WNFile::eWNFMRead | WNFileSystem::WNFile::eWNFMStream));
}

wn_bool WNFileSystem::WNReadTextFileBuffer::Serialize(const wn_uint32 _flags, const WNContainers::WNSerializerBase& _serializer) {
    mBufferPosition += _serializer.Serialize(*this, _flags);

    return(wn_true);
}
