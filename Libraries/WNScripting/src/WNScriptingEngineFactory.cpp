// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNScripting/inc/WNScriptingEngineFactory.h"
#include "WNScripting/inc/WNScriptingEngineImpl.h"
#include "WNMemory/inc/WNAllocation.h"

WNScripting::WNScriptingEngine* WNScripting::WNScriptingEngineFactory::CreateScriptingEngine() {
    return(wn::memory::construct<WNScripting::WNScriptingEngineImpl>());
}
