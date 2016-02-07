// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_NETWORKING_LINUX_CONNECTION_H__
#define __WN_NETWORKING_LINUX_CONNECTION_H__

#include "WNMultiTasking/inc/WNSpinLock.h"
#include "WNNetworking/inc/WNConnection.h"
#include "WNNetworking/inc/WNNetworkManager.h"
#include "WNNetworking/inc/WNNetworkReadBuffer.h"
#include "WNNetworking/inc/WNNetworkWriteBuffer.h"

namespace WNNetworking {
class WNNetworkManagerLinux;
class WNNetworkWriteBuffer;

class WNConnectionLinux : public WNConnection {
public:
  WNConnectionLinux(WNNetworkManager& _manager);
  virtual ~WNConnectionLinux();

  virtual void Invalidate();
  virtual void SendBuffer(WNNetworkWriteBuffer& _buffer);
  int32_t GetLinuxSocket();
  void NotifyReadyToSend(bool socketFree);
  void NotifyReadReady();

protected:
  void ReadReady();
  bool Send();
  WNNetworkManager& mManager;
  bool mReadyToWrite;
  int32_t mSockFD;
  size_t mBufferChunkCount;
  size_t mBufferWritten;
  size_t mTotalSent;
  std::atomic<size_t> mReadAtomic;
  std::atomic<size_t> mWriteAtomic;
  wn::multi_tasking::spin_lock mReadLock;
  wn::multi_tasking::spin_lock mWriteLock;
  wn::multi_tasking::spin_lock mSendBufferLock;
  std::deque<WNNetworkWriteBuffer> mSendBuffers;

  wn::memory::basic_allocator allocator;
  WNNetworkReadBuffer mCurrentReadBuffer;
  size_t mInProcessedBytes;
  size_t mReadHead;
  size_t mBufferBase;
  wn::memory::intrusive_ptr<WNBufferResource> mReadLocation;
  size_t mOverflowAmount;
  char mOverflowLocation[8];
};
};

#endif  // __WN_NETWORKING_LINUX_CONNECTION_H__
