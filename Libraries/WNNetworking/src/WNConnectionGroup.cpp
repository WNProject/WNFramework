// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNNetworking/inc/WNConnectionGroup.h"
#include "WNNetworking/inc/WNNetworkWriteBuffer.h"
#include "WNStrings/inc/WNStrings.h"

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

WNConnectionGroup::WNConnectionGroup(const wn_char* _groupName) {
    WN_DEBUG_ASSERT_DESC(WNStrings::WNStrLen(_groupName) < 255, "Your group name must be < 255 characters long");

    mConnectionName = WNStrings::WNStrNDup(_groupName, 255);
}

WNConnectionGroup::~WNConnectionGroup() {
    wn::memory::heap_free(mConnectionName);
}

wn_void WNConnectionGroup::AddConnection(WNConnection* _connection) {
    WN_DEBUG_ASSERT_DESC(std::find(mConnections.begin(), mConnections.end(), _connection) == mConnections.end(), "This connection is has already been added");

    mConnections.push_back(_connection);
}

wn_void WNConnectionGroup::CleanConnection(WNConnection* _connection) {
    //If we find this is happening a LOT, should probably switch off to a std::list
    std::vector<WNConnection*>::iterator i = std::find(mConnections.begin(), mConnections.end(), _connection);

    if (i != mConnections.end()) {
        mConnections.erase(i);
    }
}

wn_void WNConnectionGroup::SendBuffer(WNNetworkWriteBuffer& buffer) {
    WN_UNUSED_ARGUMENT(buffer);

    //DONOTHINGYET
}
