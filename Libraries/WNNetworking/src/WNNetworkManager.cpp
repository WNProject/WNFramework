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

WN_VOID WNNetworking::WNNetworkManager::SetCallback(WN_UINT32 _identifier, const WNMessageCallback& _callback) {
    if(mCallbackMap.find(_identifier) == mCallbackMap.end()) {
        mCallbackMap[_identifier] = _callback;
    }
}

WN_VOID WNNetworking::WNNetworkManager::FireCallback(WN_UINT32 _identifier, WNConnection* ptr, WNNetworkReadBuffer& _buffer) {
    WN_UNORDERED_MAP<WN_UINT32, WNMessageCallback >::iterator it = mCallbackMap.find(_identifier);

    //DO SOME LOGGING HERE
    (*it).second.Execute(ptr, _buffer);
}

WNNetworking::WNNetworkManagerReturnCode::Type WNNetworking::WNNetworkManager::CreateConnectionGroup(WNConnectionGroup*& _outHandle, const WN_CHAR* _groupName) {
    WN_UNUSED_ARG(_outHandle);

    mGroupList.push_back(WN_NEW WNConnectionGroup(_groupName));

    return(WNNetworkManagerReturnCode::eWNOK);
}

WN_VOID WNNetworking::WNNetworkManager::DestroyConnectionGroup(WNConnectionGroup* _group) {
    UnregisterConnection(_group);

    std::list<WNConnectionGroup*>::iterator i = std::find(mGroupList.begin(), mGroupList.end(), _group);

    WN_RELEASE_ASSERT_DESC(i != mGroupList.end(), "You attempted to delete a connection group more than once");

    mGroupList.erase(i);

    WN_DELETE(*i);
}

WN_VOID WNNetworking::WNNetworkManager::UnregisterConnection(WNConnection* _connection)
{
    for(std::list<WNConnectionGroup*>::iterator i = mGroupList.begin(); i != mGroupList.end(); ++i){
        (*i)->CleanConnection(_connection);
    }
}

WN_VOID WNNetworking::WNNetworkManager::Cleanup() {
    for(std::list<WNConnectionGroup*>::iterator i = mGroupList.begin(); i != mGroupList.end(); ++i){
        WN_DELETE(*i);
    }
}

WN_VOID WNNetworking::WNNetworkManager::InitializeBuffer(WNNetworkWriteBuffer& _buffer, WN_UINT32 _number) {
    WN_UINT32 num = 0;
    _buffer.Serialize(0, WNContainers::WNSerializer<WN_UINT32>(num));
    _buffer.Serialize(0, WNContainers::WNSerializer<WN_UINT32>(_number));
}
