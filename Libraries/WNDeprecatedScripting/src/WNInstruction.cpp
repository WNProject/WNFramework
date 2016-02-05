// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNDeprecatedScripting/inc/WNInstruction.h"

using namespace WNScripting;

WNInstruction::WNInstruction() : mReturns(false) { }
WNInstruction::~WNInstruction() {}

bool WNInstruction::Returns() { return(mReturns); }
