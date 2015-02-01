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

WNNetworkReadBuffer::WNNetworkReadBuffer(WNNetworkManager& _manager) :
    mManager(_manager),
    mBufferPointer(0),
    mTotalRead(0),
    mInitialized(wn_false),
    mTotalSize(0),
    mLastChunk(wn_false),
    mDataOverflow(wn::make_intrusive<WNBufferResource, WNNetworkManager&>(_manager)) {
}

wn_bool WNNetworkReadBuffer::Serialize(wn_uint32 _flags, const WNSerializerBase& _serializer) {
    const wn_size_t read = _serializer.Serialize(*this, _flags);

    mBufferPointer += read;
    mTotalRead += read;

    WN_RELEASE_ASSERT(mTotalRead <= mTotalSize);
    
    if (mBufferPointer > MAX_DATA_WRITE) {
        mBufferPointer -= MAX_DATA_WRITE;
        mCurrentChunk = mChunks.erase(mCurrentChunk);
    }

    return(wn_true);
}

WNDataBufferType WNNetworkReadBuffer::GetType() {
    return(eWNReadBinary);
}

wn_char* WNNetworkReadBuffer::ReserveBytes(wn_size_t _numBytes, wn_size_t& _returnedBytes) {
    if (mBufferPointer == MAX_DATA_WRITE) {
        mBufferPointer = 0;
        mCurrentChunk++;

        _returnedBytes = wn::min(MAX_DATA_WRITE, _numBytes);

        return((*mCurrentChunk)->GetPointer());
    }

    _returnedBytes = wn::min(_numBytes, MAX_DATA_WRITE - mBufferPointer);
    return((*mCurrentChunk)->GetPointer());
}

wn_void WNNetworkReadBuffer::AppendBuffer(wn::intrusive_ptr<WNBufferResource>&  _buffer, wn_size_t _datacount, wn_size_t _dataOffset) {
    WN_RELEASE_ASSERT_DESC(_datacount < MAX_DATA_WRITE, "You cannot append a buffer larger than the set size");

    mInitialized = wn_true;
    wn_bool firstChunk = wn_false;

    if (mChunks.empty()) {
        mWriteOffset = _dataOffset;
        mBufferPointer = _dataOffset;
        firstChunk = wn_true;
    } else {
        WN_RELEASE_ASSERT_DESC(_dataOffset == 0, "Cannot append a buffer to a position other than the start of the stream if it has a space at the start");
    }
    
    mChunks.push_back(_buffer);

    if (firstChunk) {
        mCurrentChunk = mChunks.begin();
    }

    if ((_datacount + _dataOffset) != MAX_DATA_WRITE) {
        WN_RELEASE_ASSERT_DESC(!mLastChunk, "Error trying to append a non-full buffer to a write stream");

        mLastChunk = wn_true;
    }

    mTotalSize += _datacount;
}

wn_bool WNNetworkReadBuffer::Initialized() {
    return(mInitialized);
}

wn_void WNNetworkReadBuffer::Clear() {
    mInitialized = wn_false;
    mTotalSize = mWriteOffset = mBufferPointer = mTotalRead = mTotalSize = 0;
    mLastChunk = wn_false;

    mChunks.clear();
}

wn_char* WNNetworkReadBuffer::GetLastBuffer() {
    WN_DEBUG_ASSERT(mChunks.size() > 0);

    return(mChunks.back()->GetBaseLocation());
}

wn_void WNNetworkReadBuffer::PrepareRead() {
    mChunks.back()->PrepareRead();
}
