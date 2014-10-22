// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNScripting/inc/WNInstruction.h"

using namespace WNScripting;

WNInstruction::WNInstruction() : mReturns(WN_FALSE) { }
WNInstruction::~WNInstruction() {}

WN_BOOL WNInstruction::Returns() { return(mReturns); }
