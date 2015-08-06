// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNDeprecatedScripting/inc/WNInstruction.h"

using namespace WNScripting;

WNInstruction::WNInstruction() : mReturns(wn_false) { }
WNInstruction::~WNInstruction() {}

wn_bool WNInstruction::Returns() { return(mReturns); }
