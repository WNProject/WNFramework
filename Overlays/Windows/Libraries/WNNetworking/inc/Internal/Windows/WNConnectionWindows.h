// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_NETWORKING_WINDOWS_CONNECTION_H__
#define __WN_NETWORKING_WINDOWS_CONNECTION_H__

#include "WNMultiTasking/inc/WNMutex.h"
#include "WNNetworking/inc/WNBufferResource.h"
#include "WNNetworking/inc/WNConnection.h"
#include "WNNetworking/inc/WNNetworkReadBuffer.h"
#include "WNNetworking/inc/WNNetworkWriteBuffer.h"

#ifdef _WN_MSVC
#pragma warning(push)
#pragma warning(                                                               \
    disable : 4275)  // non – DLL-interface used as base for DLL-interface
#pragma warning(disable : 4996)
#endif

#include <deque>

#ifdef _WN_MSVC
#pragma warning(pop)
#endif

namespace WNNetworking {
class WNNetworkManagerWindows;

class WNConnectionWindows : public WNConnection {
public:
  WNConnectionWindows(WNNetworkManager& _manager);
  virtual ~WNConnectionWindows();

  virtual void Invalidate();
  virtual void SendBuffer(WNNetworkWriteBuffer& _buffer);

protected:
  enum eWNNetworkOperation { eWNWriteOp, eWNReadOp, eWNNoOp };

protected:
  friend class WNNetworkManagerWindows;

protected:
  SOCKET GetWindowsSocket();
  bool Receive();
  eWNNetworkOperation GetOperation(LPOVERLAPPED _overlapped);
  bool ProcessRead(
      WNNetworkManagerWindows* _windowsManager, DWORD _bytesTransferred);
  void AppendSendBuffer(WNNetworkWriteBuffer& _buff);
  void Send();
  void ProcessWrite();

protected:
  wn::memory::basic_allocator allocator;
  WNNetworkManager& mManager;
  WNNetworkReadBuffer mCurrentReadBuffer;
  wn::multi_tasking::mutex mSendMutex;
  std::deque<WNNetworkWriteBuffer> mWriteBuffers;
  std::vector<WSABUF> mWSAWriteBuffers;
  size_t mInProcessedBytes;
  size_t mReadHead;
  size_t mBufferBase;
  WSAOVERLAPPED mReceiveOverlap;
  WSAOVERLAPPED mSendOverlap;
  wn::memory::intrusive_ptr<WNBufferResource> mReadLocation;
  SOCKET mSocket;
  size_t mOverflowAmount;
  char mOverflowLocation[8];
};
};

#endif  // __WN_NETWORKING_WINDOWS_CONNECTION_H__
