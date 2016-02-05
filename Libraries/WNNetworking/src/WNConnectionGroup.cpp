// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNNetworking/inc/WNConnectionGroup.h"
#include "WNNetworking/inc/WNNetworkWriteBuffer.h"
#include "WNMemory/inc/WNStringUtility.h"

#ifdef _WN_MSVC
    #pragma warning(push)
    #pragma warning(disable: 4275)
    #pragma warning(disable: 4996)
#endif

#include <algorithm>

#ifdef _WN_MSVC
    #pragma warning(pop)
#endif

using namespace WNNetworking;

WNConnectionGroup::WNConnectionGroup(const char* _groupName) {
  WN_DEBUG_ASSERT_DESC(wn::memory::strlen(_groupName) < 255, "Your group name must be < 255 characters long");

  mConnectionName = wn::memory::strndup(_groupName, 255);
}

WNConnectionGroup::~WNConnectionGroup() {
    wn::memory::heap_free(mConnectionName);
}

void WNConnectionGroup::AddConnection(WNConnection* _connection) {
    WN_DEBUG_ASSERT_DESC(std::find(mConnections.begin(), mConnections.end(), _connection) == mConnections.end(), "This connection is has already been added");

    mConnections.push_back(_connection);
}

void WNConnectionGroup::CleanConnection(WNConnection* _connection) {
    //If we find this is happening a LOT, should probably switch off to a std::list
    std::vector<WNConnection*>::iterator i = std::find(mConnections.begin(), mConnections.end(), _connection);

    if (i != mConnections.end()) {
        mConnections.erase(i);
    }
}

void WNConnectionGroup::SendBuffer(WNNetworkWriteBuffer& buffer) {
    WN_UNUSED_ARGUMENT(buffer);

    //DONOTHINGYET
}
