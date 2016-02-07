// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNNetworking/inc/WNConnection.h"
#include "WNMemory/inc/WNBasic.h"

WNNetworking::WNConnection::WNConnection()
  : mConnectionName(nullptr), mIsValid(true) {}

const char* WNNetworking::WNConnection::GetName() {
  return (mConnectionName);
}

WNNetworking::WNConnection::~WNConnection() {
  if (mConnectionName != nullptr) {
    wn::memory::destroy(mConnectionName);

    mConnectionName = nullptr;
  }
}

void WNNetworking::WNConnection::Invalidate() {
  mIsValid = false;
}

bool WNNetworking::WNConnection::IsValid() {
  return (mIsValid);
}