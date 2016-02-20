// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNNetworking/inc/WNNetworkManager.h"
#include "WNContainers/inc/WNSerializer.h"
#include "WNMemory/inc/WNBasic.h"
#include "WNNetworking/inc/WNConnectionGroup.h"
#include "WNNetworking/inc/WNNetworkWriteBuffer.h"

#ifdef _WN_MSVC
#pragma warning(push)
#pragma warning(                                                               \
    disable : 4275)  // non – DLL-interface used as base for DLL-interface
#pragma warning(disable : 4996)
#endif

#include <algorithm>

#ifdef _WN_MSVC
#pragma warning(pop)
#endif

void WNNetworking::WNNetworkManager::SetCallback(
    uint32_t _identifier, const WNMessageCallback& _callback) {
  if (mCallbackMap.find(_identifier) == mCallbackMap.end()) {
    mCallbackMap[_identifier] = _callback;
  }
}

void WNNetworking::WNNetworkManager::FireCallback(
    uint32_t _identifier, WNConnection* ptr, WNNetworkReadBuffer& _buffer) {
  WN_UNORDERED_MAP<uint32_t, WNMessageCallback>::iterator it =
      mCallbackMap.find(_identifier);

  // DO SOME LOGGING HERE
  (*it).second(ptr, _buffer);
}

WNNetworking::WNNetworkManagerReturnCode::type
WNNetworking::WNNetworkManager::CreateConnectionGroup(
    WNConnectionGroup*& _outHandle, const char* _groupName) {
  WN_UNUSED_ARGUMENT(_outHandle);

  mGroupList.push_back(wn::memory::construct<WNConnectionGroup>(_groupName));

  return (WNNetworkManagerReturnCode::ok);
}

void WNNetworking::WNNetworkManager::DestroyConnectionGroup(
    WNConnectionGroup* _group) {
  UnregisterConnection(_group);

  std::list<WNConnectionGroup*>::iterator i =
      std::find(mGroupList.begin(), mGroupList.end(), _group);

  WN_RELEASE_ASSERT_DESC(i != mGroupList.end(),
      "You attempted to delete a connection group more than once");

  mGroupList.erase(i);

  wn::memory::destroy(*i);
}

void WNNetworking::WNNetworkManager::UnregisterConnection(
    WNConnection* _connection) {
  for (auto i : mGroupList) {
    i->CleanConnection(_connection);
  }
}

void WNNetworking::WNNetworkManager::Cleanup() {
  for (auto& i : mGroupList) {
    wn::memory::destroy(i);
  }
}

void WNNetworking::WNNetworkManager::InitializeBuffer(
    WNNetworkWriteBuffer& _buffer, const uint32_t _number) {
  uint32_t number1 = 0;
  uint32_t number2 = _number;

  _buffer.serialize(wn::containers::serializer<uint32_t>(number1), 0);
  _buffer.serialize(wn::containers::serializer<uint32_t>(number2), 0);
}
