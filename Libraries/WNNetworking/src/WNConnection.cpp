// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNNetworking/inc/WNConnection.h"
#include "WNMemory/inc/WNMemory.h"

WNNetworking::WNConnection::WNConnection() :
    mConnectionName(WN_NULL),
    mIsValid(WN_TRUE) {
}

const WN_CHAR* WNNetworking::WNConnection::GetName() {
    return(mConnectionName);
}

WNNetworking::WNConnection::~WNConnection() {
    if (mConnectionName != WN_NULL) {
        WN_DELETE(mConnectionName);

        mConnectionName = WN_NULL;
    }
}

WN_VOID WNNetworking::WNConnection::Invalidate() {
    mIsValid = WN_FALSE;
}

WN_BOOL WNNetworking::WNConnection::IsValid() {
    return(mIsValid);
}