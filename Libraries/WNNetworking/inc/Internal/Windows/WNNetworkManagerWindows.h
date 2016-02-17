// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_NETWORKING_NETWORK_MANAGER_WINDOWS_H__
#define __WN_NETWORKING_NETWORK_MANAGER_WINDOWS_H__

#include "WNMultiTasking/inc/WNSpinLock.h"
#include "WNNetworking/inc/WNNetworkManager.h"

#include <vector>

namespace wn {
template <typename Return>
class thread;
}

namespace WNNetworking {
class WNListenConnectionWindows;
class WNInConnectionWindows;
class WNConnectionWindows;
class WNOutConnectionWindows;

class WNNetworkManagerWindows : public WNNetworkManager {
public:
  WNNetworkManagerWindows();
  virtual ~WNNetworkManagerWindows();

  virtual WNNetworkManagerReturnCode::type Initialize(
      uint32_t _numWorkerThreads);
  virtual WNNetworkManagerReturnCode::type ConnectTo(WNConnection*& _outHandle,
      WNConnectionType::type _type, const char* _target, uint16_t _port);
  virtual WNNetworkManagerReturnCode::type CreateListener(
      WNConnection*& _outHandle, WNConnectionType::type _type, uint16_t _port,
      const WNConnectedCallback& _callback);
  virtual void Cleanup();
  virtual void DestroyConnection(WNConnection* _connection);

private:
  void IOCPThread();
  void ListenThread();

  enum eWNWindowsInitializationState {
    eWNNotStarted,
    eWNWSAStartup,
    eWNAcceptEventCreated,
    eWNShutdownEventCreated,
    eWNIOCPCreated,
    eWNThreadsCreated,
    eWNInitializationCompleted
  };

  wn::memory::basic_allocator allocator;
  uint32_t mMaxThreads;
  HANDLE mIOCP;
  HANDLE mAcceptEvent;
  HANDLE mShutdownEvent;
  eWNWindowsInitializationState mInitializationState;
  wn::multi_tasking::thread* mListenThread;
  wn::multi_tasking::spin_lock mListenerMutex;
  wn::multi_tasking::spin_lock mRecievedMutex;
  wn::multi_tasking::spin_lock mInvalidMutex;
  wn::multi_tasking::spin_lock mOutgoingMutex;

  std::vector<wn::multi_tasking::thread*> mThreads;
  std::list<WNListenConnectionWindows*> mIncommingConnections;
  std::list<WNOutConnectionWindows*> mOutgoingConnections;
  std::list<WNInConnectionWindows*> mReceivedConnections;
  std::list<WNConnection*> mInvalidConnections;
};
};

#endif  // __WN_NETWORKING_NETWORK_MANAGER_WINDOWS_H__
