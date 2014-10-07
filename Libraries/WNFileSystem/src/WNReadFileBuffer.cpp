////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "WNFileSystem/inc/WNReadFileBuffer.h"
#include "WNMemory/inc/WNManipulation.h"
#include "WNMemory/inc/WNAllocation.h"
#include "WNContainers/inc/WNSerializer.h"

WNFileSystem::WNReadTextFileBuffer::WNReadTextFileBuffer(const WNFileSystem::WNFileBufferType _type, const WN_SIZE_T _bufferSize) :
    WNFileBuffer(_type),
    mBufferUsage(0),
    mBufferSize(_bufferSize),
    mBufferPosition(0),
    mEndFile(WN_FALSE) {
    mCurrentBuffer = WNMemory::WNMallocT<WN_CHAR>(_bufferSize);

    WN_RELEASE_ASSERT(mCurrentBuffer != WN_NULL);
}

WNFileSystem::WNReadTextFileBuffer::~WNReadTextFileBuffer() {
    if (mFile.IsValid()) {
        if (mCurrentBuffer != WN_NULL) {
            WNMemory::WNFreeT<WN_CHAR>(mCurrentBuffer);
        }
    }
}

WN_CHAR* WNFileSystem::WNReadTextFileBuffer::ReserveBytes(const WN_SIZE_T _numBytes, WN_SIZE_T& _returnedBytes) {
    if (_numBytes + mBufferPosition > mBufferSize) {
        if (_numBytes > mBufferSize) {
            WN_DEBUG_ASSERT(mBufferPosition <= mBufferUsage);

            mSpareBuffer = WNMemory::WNReallocT<WN_CHAR>(mSpareBuffer, _numBytes);

            WNMemory::WNMemCpyT<WN_CHAR>(mSpareBuffer, mCurrentBuffer + mBufferPosition, mBufferUsage - mBufferPosition);

            mCurrentBuffer = mSpareBuffer;
            mSpareBuffer = WNMemory::WNReallocT<WN_CHAR>(mCurrentBuffer, _numBytes);
            mBufferSize = _numBytes;
            mBufferUsage -= mBufferPosition;
            mBufferPosition = 0;
        } else {
            WN_DEBUG_ASSERT(mBufferPosition <= mBufferUsage);

            WNMemory::WNMemCpyT<WN_CHAR>(mSpareBuffer, mCurrentBuffer + mBufferPosition, mBufferUsage - mBufferPosition);

            WN_CHAR* currentBuffer = mCurrentBuffer;

            mCurrentBuffer = mSpareBuffer;
            mSpareBuffer = currentBuffer;
            mBufferUsage -= mBufferPosition;
            mBufferPosition = 0;
        }
    }

    WN_SIZE_T haveData = mBufferUsage - mBufferPosition;

    if (!mEndFile && haveData < _numBytes) {
        const WN_SIZE_T bytesRead = mFile.ReadData(mCurrentBuffer + mBufferUsage, _numBytes - haveData);

        if (bytesRead == 0) {
            mEndFile = WN_TRUE;
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

WNFileSystem::WNFile::WNFileError WNFileSystem::WNReadTextFileBuffer::SetFile(const WN_CHAR* _fileName) {
    mEndFile = WN_FALSE;

    WN_RELEASE_ASSERT_DESC(!mFile.IsValid(), "Error, trying to set a file on a text buffer that already exists");

    return(mFile.OpenFile(_fileName, WNFileSystem::WNFile::eWNFMExclusive | WNFileSystem::WNFile::eWNFMRead | WNFileSystem::WNFile::eWNFMStream));
}

WN_BOOL WNFileSystem::WNReadTextFileBuffer::Serialize(const WN_UINT32 _flags, const WNContainers::WNSerializerBase& _serializer) {
    mBufferPosition += _serializer.Serialize(*this, _flags);

    return(WN_TRUE);
}
