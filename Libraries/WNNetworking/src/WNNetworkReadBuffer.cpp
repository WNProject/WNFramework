// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNNetworking/inc/WNNetworkReadBuffer.h"
#include "WNContainers/inc/WNDataBuffer.h"
#include "WNContainers/inc/WNSerializer.h"
#include "WNMath/inc/WNBasic.h"
#include "WNMemory/inc/WNBasic.h"
#include "WNMemory/inc/WNBasic.h"
#include "WNNetworking/inc/WNBufferResource.h"
#include "WNNetworking/inc/WNNetworkManager.h"
#include "WNNetworking/inc/WNNetworkReadBuffer.h"

using namespace WNNetworking;
using namespace WNContainers;

WNNetworkReadBuffer::WNNetworkReadBuffer(WNNetworkManager& _manager)
  : mManager(_manager),
    mBufferPointer(0),
    mTotalRead(0),
    mInitialized(false),
    mTotalSize(0),
    mLastChunk(false),
    mDataOverflow(
        wn::memory::make_intrusive<WNBufferResource, WNNetworkManager&>(
            &allocator, _manager)) {}

bool WNNetworkReadBuffer::serialize(
    const wn::containers::serializer_base& _serializer, const uint32_t _flags) {
  const size_t read = _serializer.serialize(*this, _flags);

  mBufferPointer += read;
  mTotalRead += read;

  WN_RELEASE_ASSERT(mTotalRead <= mTotalSize);

  if (mBufferPointer > wn::containers::MAX_DATA_WRITE) {
    mBufferPointer -= wn::containers::MAX_DATA_WRITE;
    mCurrentChunk = mChunks.erase(mCurrentChunk);
  }

  return (true);
}

wn::containers::data_buffer_type WNNetworkReadBuffer::type() const {
  return (wn::containers::data_buffer_type::read_binary);
}

char* WNNetworkReadBuffer::reserve(
    const size_t _numBytes, size_t& _returnedBytes) {
  if (mBufferPointer == wn::containers::MAX_DATA_WRITE) {
    mBufferPointer = 0;
    mCurrentChunk++;

    _returnedBytes = wn::min(wn::containers::MAX_DATA_WRITE, _numBytes);

    return ((*mCurrentChunk)->GetPointer());
  }

  _returnedBytes =
      wn::min(_numBytes, wn::containers::MAX_DATA_WRITE - mBufferPointer);
  return ((*mCurrentChunk)->GetPointer());
}

void WNNetworkReadBuffer::AppendBuffer(
    wn::memory::intrusive_ptr<WNBufferResource>& _buffer, size_t _datacount,
    size_t _dataOffset) {
  WN_RELEASE_ASSERT_DESC(_datacount < wn::containers::MAX_DATA_WRITE,
      "You cannot append a buffer larger than the set size");

  mInitialized = true;
  bool firstChunk = false;

  if (mChunks.empty()) {
    mWriteOffset = _dataOffset;
    mBufferPointer = _dataOffset;
    firstChunk = true;
  } else {
    WN_RELEASE_ASSERT_DESC(_dataOffset == 0,
        "Cannot append a buffer to a position other than the start of the "
        "stream if it has a space at the start");
  }

  mChunks.push_back(_buffer);

  if (firstChunk) {
    mCurrentChunk = mChunks.begin();
  }

  if ((_datacount + _dataOffset) != wn::containers::MAX_DATA_WRITE) {
    WN_RELEASE_ASSERT_DESC(!mLastChunk,
        "Error trying to append a non-full buffer to a write stream");

    mLastChunk = true;
  }

  mTotalSize += _datacount;
}

bool WNNetworkReadBuffer::Initialized() {
  return (mInitialized);
}

void WNNetworkReadBuffer::Clear() {
  mInitialized = false;
  mTotalSize = mWriteOffset = mBufferPointer = mTotalRead = mTotalSize = 0;
  mLastChunk = false;

  mChunks.clear();
}

char* WNNetworkReadBuffer::GetLastBuffer() {
  WN_DEBUG_ASSERT(mChunks.size() > 0);

  return (mChunks.back()->GetBaseLocation());
}

void WNNetworkReadBuffer::PrepareRead() {
  mChunks.back()->PrepareRead();
}
