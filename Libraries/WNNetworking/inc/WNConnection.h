// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_NETWORKING_CONNECTION_H__
#define __WN_NETWORKING_CONNECTION_H__

#include "WNCore/inc/WNTypes.h"

namespace WNNetworking {
class WNNetworkWriteBuffer;

class WNConnection {
public:
  WNConnection();
  virtual ~WNConnection();

  const char* GetName();
  bool IsValid();
  virtual void Invalidate();
  virtual void SendBuffer(WNNetworkWriteBuffer& buffer) = 0;

protected:
  char* mConnectionName;
  bool mIsValid;
};
};

#endif  // __WN_NETWORKING_CONNECTION_H__