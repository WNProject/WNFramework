// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNNetworking/inc/WNBufferResource.h"
#include "WNNetworking/inc/WNNetworkManager.h"
#include "WNContainers/inc/WNDataBuffer.h"

using namespace WNNetworking;

WNBufferResource::WNBufferResource(const WNNetworkManager& manager) :
    wn::memory::intrusive_ptr_base(),
    mManager(manager),
    mCurLocation(0) {
    mBuffer = wn::memory::malloc<wn_char>(WNContainers::MAX_DATA_WRITE);
    mBaseLocation = 0;
    #ifdef _WN_WINDOWS
        mWinBuf.buf = mBuffer;
        mWinBuf.len = static_cast<ULONG>(mBaseLocation);
    #endif
}

WNBufferResource::~WNBufferResource() {
    wn::memory::free<wn_char>(mBuffer);
}

wn_char* WNBufferResource::GetPointer() const {
    return(mBuffer + mCurLocation);
}

wn_void WNBufferResource::Clear() {
    mCurLocation = 0;
}

wn_void WNBufferResource::FillData() {
    mCurLocation = WNContainers::MAX_DATA_WRITE;
}

wn_void WNBufferResource::AddData(wn_size_t _data) {
    mCurLocation += _data;

    WN_DEBUG_ASSERT(mCurLocation <= WNContainers::MAX_DATA_WRITE);
}

wn_char* WNBufferResource::GetBaseLocation() const {
    return(mBuffer);
}

wn_size_t WNBufferResource::GetSize() const {
    return(WNContainers::MAX_DATA_WRITE);
}

wn_size_t WNBufferResource::GetWritten() const {
    return(mCurLocation);
}

wn_void WNBufferResource::FlushWrite() {
    #ifdef _WN_WINDOWS
        mWinBuf.len = static_cast<ULONG>(mCurLocation);
    #endif
}

wn_void WNBufferResource::PrepareRead() {
    #ifdef _WN_WINDOWS
        mWinBuf.len = static_cast<ULONG>(WNContainers::MAX_DATA_WRITE - mCurLocation);
        mWinBuf.buf = GetPointer();
    #endif
}
