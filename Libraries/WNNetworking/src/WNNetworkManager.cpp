// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNNetworking/inc/WNNetworkManager.h"
#include "WNMemory/inc/WNAllocation.h"
#include "WNNetworking/inc/WNConnectionGroup.h"
#include "WNContainers/inc/WNSerializer.h"
#include "WNNetworking/inc/WNNetworkWriteBuffer.h"

#ifdef _WN_MSVC
    #pragma warning(push)
    #pragma warning(disable: 4275)  // non – DLL-interface used as base for DLL-interface
    #pragma warning(disable: 4996)
#endif

#include <algorithm>

#ifdef _WN_MSVC
    #pragma warning(pop)
#endif

wn_void WNNetworking::WNNetworkManager::SetCallback(wn_uint32 _identifier, const WNMessageCallback& _callback) {
    if(mCallbackMap.find(_identifier) == mCallbackMap.end()) {
        mCallbackMap[_identifier] = _callback;
    }
}

wn_void WNNetworking::WNNetworkManager::FireCallback(wn_uint32 _identifier, WNConnection* ptr, WNNetworkReadBuffer& _buffer) {
    WN_UNORDERED_MAP<wn_uint32, WNMessageCallback >::iterator it = mCallbackMap.find(_identifier);

    //DO SOME LOGGING HERE
    (*it).second.Execute(ptr, _buffer);
}

WNNetworking::WNNetworkManagerReturnCode::type WNNetworking::WNNetworkManager::CreateConnectionGroup(WNConnectionGroup*& _outHandle, const wn_char* _groupName) {
    WN_UNUSED_ARGUMENT(_outHandle);

    mGroupList.push_back(WN_NEW WNConnectionGroup(_groupName));

    return(WNNetworkManagerReturnCode::ok);
}

wn_void WNNetworking::WNNetworkManager::DestroyConnectionGroup(WNConnectionGroup* _group) {
    UnregisterConnection(_group);

    std::list<WNConnectionGroup*>::iterator i = std::find(mGroupList.begin(), mGroupList.end(), _group);

    WN_RELEASE_ASSERT_DESC(i != mGroupList.end(), "You attempted to delete a connection group more than once");

    mGroupList.erase(i);

    WN_DELETE(*i);
}

wn_void WNNetworking::WNNetworkManager::UnregisterConnection(WNConnection* _connection)
{
    for(std::list<WNConnectionGroup*>::iterator i = mGroupList.begin(); i != mGroupList.end(); ++i){
        (*i)->CleanConnection(_connection);
    }
}

wn_void WNNetworking::WNNetworkManager::Cleanup() {
    for(std::list<WNConnectionGroup*>::iterator i = mGroupList.begin(); i != mGroupList.end(); ++i){
        WN_DELETE(*i);
    }
}

wn_void WNNetworking::WNNetworkManager::InitializeBuffer(WNNetworkWriteBuffer& _buffer, wn_uint32 _number) {
    wn_uint32 num = 0;
    _buffer.Serialize(0, WNContainers::WNSerializer<wn_uint32>(num));
    _buffer.Serialize(0, WNContainers::WNSerializer<wn_uint32>(_number));
}
