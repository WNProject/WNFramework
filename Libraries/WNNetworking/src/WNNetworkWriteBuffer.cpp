// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNNetworking/inc/WNNetworkManager.h"
#include "WNNetworking/inc/WNNetworkWriteBuffer.h"
#include "WNContainers/inc/WNSerializer.h"
#include "WNCore/inc/WNEndian.h"
#include "WNMath/inc/WNBasic.h"

using namespace WNNetworking;
using namespace WNContainers;

WNNetworkWriteBuffer::WNNetworkWriteBuffer(WNNetworkManager& _manager, wn_uint32 _number) :
    mManager(_manager),
    mBufferPointer(0),
    mTotalWritten(0),
    mFlushed(wn_false) {
    mChunks.push_back(wn::memory::make_intrusive<WNBufferResource, WNNetworkManager&>(mManager));
    _manager.InitializeBuffer(*this, _number);
}

WNNetworkWriteBuffer::WNNetworkWriteBuffer(const WNNetworkWriteBuffer& _other) :
    mManager(_other.mManager),
    mBufferPointer(_other.mBufferPointer),
    mTotalWritten(_other.mTotalWritten),
    mFlushed(_other.mFlushed) {
    (*this) = _other;
}

WNNetworkWriteBuffer& WNNetworkWriteBuffer::operator = (const WNNetworkWriteBuffer& _other) {
    if (&_other == this) {
        return(*this);
    }

    mFlushed = _other.mFlushed;
    mTotalWritten = _other.mTotalWritten;
    mBufferPointer = _other.mBufferPointer;

    #if defined _WN_MSVC && _WN_MSVC >= 160000000
        for (WNBufferQueue::const_iterator i = _other.GetChunks().cbegin(); i != _other.GetChunks().cend(); ++i) {
            mChunks.push_back(*i);
        }
    #else
        for (WNBufferQueue::const_iterator i = _other.GetChunks().begin(); i != _other.GetChunks().end(); ++i) {
            mChunks.push_back(*i);
        }
    #endif

    return(*this);
}

wn_bool WNNetworkWriteBuffer::serialize(const wn::containers::serializer_base& _serializer, const wn_uint32 _flags) {
    WN_DEBUG_ASSERT(!mFlushed);

    const wn_size_t size = _serializer.serialize(*this, _flags);

    mBufferPointer += size;

    mChunks.back()->AddData(size);

    mTotalWritten += size;

    return(wn_true);
}

wn_char* WNNetworkWriteBuffer::reserve(const wn_size_t _numBytes, wn_size_t& _returnedBytes) {
    WN_DEBUG_ASSERT(!mFlushed);

    if (mBufferPointer == wn::containers::MAX_DATA_WRITE) {
        mChunks.back()->FillData();
        mChunks.push_back(wn::memory::make_intrusive<WNBufferResource, WNNetworkManager&>(mManager));

        mBufferPointer = 0;
    }

    _returnedBytes = wn::min(_numBytes, wn::containers::MAX_DATA_WRITE - mBufferPointer);

    return(mChunks.back()->GetPointer());
}

wn::containers::data_buffer_type WNNetworkWriteBuffer::type() const {
    return(wn::containers::data_buffer_type::write_binary);
}

wn_void WNNetworkWriteBuffer::FlushWrite() {
    if (mFlushed) {
        return;
    }

    mFlushed = wn_true;

    wn_uint32 totalWritten = static_cast<wn_uint32>(mTotalWritten);

    totalWritten = wn::core::to_big_endian(totalWritten);

    *(reinterpret_cast<wn_uint32*>(mChunks.front()->GetBaseLocation())) = totalWritten;

    for (WNBufferQueue::iterator i = mChunks.begin(); i != mChunks.end(); ++i){
        (*i)->FlushWrite();
    }
}

const WNNetworkWriteBuffer::WNBufferQueue& WNNetworkWriteBuffer::GetChunks() const {
    return(mChunks);
}
