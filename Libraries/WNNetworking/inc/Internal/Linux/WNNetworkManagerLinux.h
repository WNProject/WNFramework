// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_NETWORKING_NETWORK_MANAGER_LINUX_H__
#define __WN_NETWORKING_NETWORK_MANAGER_LINUX_H__

#include "WNMultiTasking/inc/WNSpinLock.h"
#include "WNNetworking/inc/WNNetworkManager.h"

#include <vector>

namespace wn {
template <typename Return>
class thread;
}

namespace WNNetworking {
class WNConnectionLinux;
class WNListenConnectionLinux;

class WNNetworkManagerLinux : public WNNetworkManager {
public:
  WNNetworkManagerLinux();
  virtual ~WNNetworkManagerLinux();

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
  enum eWNLinuxInitializationState {
    eWNNotStarted,
    eWNEPollWriteCreated,
    eWNWriteThreadsCreated,
    eWNEPollReadCreated,
    eWNReadThreadsCreated,
    eWNEPollListenCreated,
    eWNListenThreadCreated,
    eWNInitializationComplete
  };

private:
  bool AddToReadEPoll(WNConnectionLinux* _conn);
  bool AddToWriteEPoll(WNConnectionLinux* _conn);
  void CleanAllConnections();
  void ReadThread();
  void WriteThread();
  void ListenThread();

private:
  wn::multi_tasking::spin_lock mListenMutex;
  wn::multi_tasking::spin_lock mIncommingMutex;
  wn::multi_tasking::spin_lock mOutgoingMutex;
  wn::multi_tasking::spin_lock mInvalidMutex;

  wn::memory::basic_allocator allocator;
  eWNLinuxInitializationState mInitializationState;
  int32_t mWriteEPollInstance;
  int32_t mReadEPollInstance;
  int32_t mListenEPollInstance;
  wn::multi_tasking::thread<void>* mListenThread;
  bool mShuttingDown;
  std::vector<wn::multi_tasking::thread<void>*> mReadThreads;
  std::vector<wn::multi_tasking::thread<void>*> mWriteThreads;
  std::list<WNListenConnectionLinux*> mListenConnections;
  std::list<WNConnection*> mInvalidConnections;
  std::list<WNConnectionLinux*> mIncommingConnections;
  std::list<WNConnectionLinux*> mOutgoingConnections;
};
};

#endif  // __WN_NETWORKING_NETWORK_MANAGER_WINDOWS_H__
