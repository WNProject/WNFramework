// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_NETWORKING_NETWORK_WRITE_BUFFER_H__
#define __WN_NETWORKING_NETWORK_WRITE_BUFFER_H__

#include "WNContainers/inc/WNDataBuffer.h"
#include "WNCore/inc/WNBase.h"
#include "WNCore/inc/WNTypes.h"
#include "WNMemory/inc/WNIntrusivePtr.h"
#include "WNNetworking/inc/WNBufferResource.h"

#ifdef _WN_MSVC
#pragma warning(push)
#pragma warning(                                                               \
    disable : 4275)  // non – DLL-interface used as base for DLL-interface
#endif

#include <vector>

#ifdef _WN_MSVC
#pragma warning(pop)
#endif

namespace WNContainers {
class WNSerializerBase;
}

namespace WNNetworking {
class WNNetworkManager;
class WNBufferResource;

class WNNetworkWriteBuffer : public wn::containers::data_buffer {
public:
  typedef std::vector<wn::memory::intrusive_ptr<WNNetworking::WNBufferResource>>
      WNBufferQueue;

public:
  WNNetworkWriteBuffer(WNNetworkManager& _manager, uint32_t _number);
  WNNetworkWriteBuffer(const WNNetworkWriteBuffer& _other);
  virtual WN_FORCE_INLINE ~WNNetworkWriteBuffer() {}

  WNNetworkWriteBuffer& operator=(const WNNetworkWriteBuffer& _other);

  virtual bool serialize(const wn::containers::serializer_base& _serializer,
      const uint32_t _flags) override;
  virtual char* reserve(
      const size_t _numBytes, size_t& _returnedBytes) override;
  virtual wn::containers::data_buffer_type type() const override;

  const WNBufferQueue& GetChunks() const;
  void FlushWrite();

private:
  wn::memory::basic_allocator allocator;
  WNNetworkManager& mManager;
  WNBufferQueue mChunks;

  bool mFlushed;
  size_t mTotalWritten;
  size_t mBufferPointer;
};
}

#endif  // __WN_NETWORKING_NETWORK_WRITE_BUFFER__
