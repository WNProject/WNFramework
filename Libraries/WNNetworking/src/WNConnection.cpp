// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNNetworking/inc/WNConnection.h"
#include "WNMemory/inc/WNMemory.h"

WNNetworking::WNConnection::WNConnection() :
    mConnectionName(wn_nullptr),
    mIsValid(wn_true) {
}

const wn_char* WNNetworking::WNConnection::GetName() {
    return(mConnectionName);
}

WNNetworking::WNConnection::~WNConnection() {
    if (mConnectionName != wn_nullptr) {
        wn::memory::destroy(mConnectionName);

        mConnectionName = wn_nullptr;
    }
}

wn_void WNNetworking::WNConnection::Invalidate() {
    mIsValid = wn_false;
}

wn_bool WNNetworking::WNConnection::IsValid() {
    return(mIsValid);
}