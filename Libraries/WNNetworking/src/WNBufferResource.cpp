// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNNetworking/inc/WNBufferResource.h"
#include "WNContainers/inc/WNDataBuffer.h"
#include "WNNetworking/inc/WNNetworkManager.h"

using namespace WNNetworking;

WNBufferResource::WNBufferResource(const WNNetworkManager& manager)
  : wn::memory::intrusive_ptr_base(), mManager(manager), mCurLocation(0) {
  mBuffer = wn::memory::heap_allocate<char>(wn::containers::MAX_DATA_WRITE);
  mBaseLocation = 0;
#ifdef _WN_WINDOWS
  mWinBuf.buf = mBuffer;
  mWinBuf.len = static_cast<ULONG>(mBaseLocation);
#endif
}

WNBufferResource::~WNBufferResource() {
  wn::memory::heap_free(mBuffer);
}

char* WNBufferResource::GetPointer() const {
  return (mBuffer + mCurLocation);
}

void WNBufferResource::Clear() {
  mCurLocation = 0;
}

void WNBufferResource::FillData() {
  mCurLocation = wn::containers::MAX_DATA_WRITE;
}

void WNBufferResource::AddData(size_t _data) {
  mCurLocation += _data;

  WN_DEBUG_ASSERT(mCurLocation <= wn::containers::MAX_DATA_WRITE);
}

char* WNBufferResource::GetBaseLocation() const {
  return (mBuffer);
}

size_t WNBufferResource::GetSize() const {
  return (wn::containers::MAX_DATA_WRITE);
}

size_t WNBufferResource::GetWritten() const {
  return (mCurLocation);
}

void WNBufferResource::FlushWrite() {
#ifdef _WN_WINDOWS
  mWinBuf.len = static_cast<ULONG>(mCurLocation);
#endif
}

void WNBufferResource::PrepareRead() {
#ifdef _WN_WINDOWS
  mWinBuf.len =
      static_cast<ULONG>(wn::containers::MAX_DATA_WRITE - mCurLocation);
  mWinBuf.buf = GetPointer();
#endif
}
