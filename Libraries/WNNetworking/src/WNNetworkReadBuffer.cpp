// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNNetworking/inc/WNNetworkReadBuffer.h"
#include "WNNetworking/inc/WNNetworkManager.h"
#include "WNNetworking/inc/WNNetworkReadBuffer.h"
#include "WNNetworking/inc/WNBufferResource.h"
#include "WNContainers/inc/WNSerializer.h"
#include "WNContainers/inc/WNDataBuffer.h"
#include "WNMemory/inc/WNAllocation.h"
#include "WNMemory/inc/WNManipulation.h"
#include "WNMath/inc/WNBasic.h"

using namespace WNNetworking;
using namespace WNConcurrency;
using namespace WNContainers;
using namespace WNMemory;
using namespace WNMath;

WNNetworkReadBuffer::WNNetworkReadBuffer(WNNetworkManager& _manager) :
    mManager(_manager),
    mBufferPointer(0),
    mTotalRead(0),
    mInitialized(WN_FALSE),
    mTotalSize(0),
    mLastChunk(WN_FALSE),
    mDataOverflow(WNAllocateResource<WNBufferResource, WNNetworkManager&>(_manager)) {
}

WN_BOOL WNNetworkReadBuffer::Serialize(WN_UINT32 _flags, const WNSerializerBase& _serializer) {
    const WN_SIZE_T read = _serializer.Serialize(*this, _flags);

    mBufferPointer += read;
    mTotalRead += read;

    WN_RELEASE_ASSERT(mTotalRead <= mTotalSize);
    
    if (mBufferPointer > MAX_DATA_WRITE) {
        mBufferPointer -= MAX_DATA_WRITE;
        mCurrentChunk = mChunks.erase(mCurrentChunk);
    }

    return(WN_TRUE);
}

WNDataBufferType WNNetworkReadBuffer::GetType() {
    return(eWNReadBinary);
}

WN_CHAR* WNNetworkReadBuffer::ReserveBytes(WN_SIZE_T _numBytes, WN_SIZE_T& _returnedBytes) {
    if (mBufferPointer == MAX_DATA_WRITE) {
        mBufferPointer = 0;
        mCurrentChunk++;

        _returnedBytes = WNMin(MAX_DATA_WRITE, _numBytes);

        return((*mCurrentChunk)->GetPointer());
    }

    _returnedBytes = WNMin(_numBytes, MAX_DATA_WRITE - mBufferPointer);
    return((*mCurrentChunk)->GetPointer());
}

WN_VOID WNNetworkReadBuffer::AppendBuffer(WNResourcePointer<WNBufferResource>&  _buffer, WN_SIZE_T _datacount, WN_SIZE_T _dataOffset) {
    WN_RELEASE_ASSERT_DESC(_datacount < MAX_DATA_WRITE, "You cannot append a buffer larger than the set size");

    mInitialized = WN_TRUE;
    WN_BOOL firstChunk = WN_FALSE;

    if (mChunks.empty()) {
        mWriteOffset = _dataOffset;
        mBufferPointer = _dataOffset;
        firstChunk = WN_TRUE;
    } else {
        WN_RELEASE_ASSERT_DESC(_dataOffset == 0, "Cannot append a buffer to a position other than the start of the stream if it has a space at the start");
    }
    
    mChunks.push_back(_buffer);

    if (firstChunk) {
        mCurrentChunk = mChunks.begin();
    }

    if ((_datacount + _dataOffset) != MAX_DATA_WRITE) {
        WN_RELEASE_ASSERT_DESC(!mLastChunk, "Error trying to append a non-full buffer to a write stream");

        mLastChunk = WN_TRUE;
    }

    mTotalSize += _datacount;
}

WN_BOOL WNNetworkReadBuffer::Initialized() {
    return(mInitialized);
}

WN_VOID WNNetworkReadBuffer::Clear() {
    mInitialized = WN_FALSE;
    mTotalSize = mWriteOffset = mBufferPointer = mTotalRead = mTotalSize = 0;
    mLastChunk = WN_FALSE;

    mChunks.clear();
}

WN_CHAR* WNNetworkReadBuffer::GetLastBuffer() {
    WN_DEBUG_ASSERT(mChunks.size() > 0);

    return(mChunks.back()->GetBaseLocation());
}

WN_VOID WNNetworkReadBuffer::PrepareRead() {
    mChunks.back()->PrepareRead();
}
