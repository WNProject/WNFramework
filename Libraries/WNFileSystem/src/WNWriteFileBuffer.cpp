// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNFileSystem/inc/WNWriteFileBuffer.h"
#include "WNMemory/inc/WNAllocation.h"
#include "WNContainers/inc/WNSerializer.h"

WNFileSystem::WNWriteTextFileBuffer::WNWriteTextFileBuffer(const WNFileSystem::WNFileBufferType _type, const WN_SIZE_T _bufferSize) :
    WNFileBuffer(_type),
    mBufferUsage(0),
    mBufferSize(_bufferSize) {
    mCurrentBuffer = WNMemory::WNMallocT<WN_CHAR>(_bufferSize);

    WN_RELEASE_ASSERT(mCurrentBuffer != WN_NULL);
}

WNFileSystem::WNWriteTextFileBuffer::~WNWriteTextFileBuffer() {
    if (mFile.IsValid()) {
        DumpToFile();

        if (mCurrentBuffer != WN_NULL) {
            WNMemory::WNFreeT<WN_CHAR>(mCurrentBuffer);
        }
    }
}

WN_CHAR* WNFileSystem::WNWriteTextFileBuffer::ReserveBytes(const WN_SIZE_T _numBytes, WN_SIZE_T& _returnedBytes) {
    if (_numBytes + mBufferUsage > mBufferSize) {
        if (mBufferUsage > 0) {
            if (!DumpToFile()) {
                WNMemory::WNFreeT<WN_CHAR>(mCurrentBuffer);

                _returnedBytes = 0;

                return(WN_NULL);
            }
        }

        if (_numBytes > mBufferSize) {
            mCurrentBuffer = WNMemory::WNReallocT<WN_CHAR>(mCurrentBuffer, _numBytes);

            WN_RELEASE_ASSERT(mCurrentBuffer != WN_NULL);

            mBufferUsage = 0;
            mBufferSize = _numBytes;
        }
    }

    _returnedBytes = _numBytes;

    return(mCurrentBuffer + mBufferUsage);
}

WN_BOOL WNFileSystem::WNWriteTextFileBuffer::DumpToFile() {
    if (!mFile.IsValid() || mBufferUsage == 0) {
        return(WN_FALSE);
    }

    const WN_SIZE_T written = mFile.WriteData(mCurrentBuffer, mBufferUsage);

    if (written != mBufferUsage) {
        return(WN_FALSE);
    }

    return(WN_TRUE);
}

WNContainers::WNDataBufferType WNFileSystem::WNWriteTextFileBuffer::GetType() {
    switch(mType) {
    case WNFileSystem::eWNText:
        return(WNContainers::eWNWriteText);
    case WNFileSystem::eWNBinary:
        return(WNContainers::eWNWriteBinary);
    }

    mType = WNFileSystem::eWNText;

    return(WNContainers::eWNWriteText);
}

WNFileSystem::WNFile::WNFileError WNFileSystem::WNWriteTextFileBuffer::SetFile(const WN_CHAR* _fileName) {
    WN_RELEASE_ASSERT_DESC(!mFile.IsValid(), "Error, trying to set a file on a text buffer that already exists");

    return(mFile.OpenFile(_fileName, WNFileSystem::WNFile::eWNFMClobber | WNFileSystem::WNFile::eWNFMExclusive |
                                     WNFileSystem::WNFile::eWNFMWrite | WNFileSystem::WNFile::eWNFMCreate | WNFileSystem::WNFile::eWNFMStream));
}

WN_BOOL WNFileSystem::WNWriteTextFileBuffer::Serialize(const WN_UINT32 _flags, const WNContainers::WNSerializerBase& _serializer) {
    mBufferUsage += _serializer.Serialize(*this, _flags);

    return(WN_TRUE);
}
