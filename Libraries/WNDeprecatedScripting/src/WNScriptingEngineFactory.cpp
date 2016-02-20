// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNDeprecatedScripting/inc/WNScriptingEngineFactory.h"
#include "WNDeprecatedScripting/inc/WNScriptingEngineImplDeprecated.h"
#include "WNMemory/inc/WNBasic.h"

WNScripting::WNScriptingEngine*
wn::scripting::scripting_engine_factory::create_scripting_engine(
    wn::scripting::factory_type _type) {
  switch (_type) {
    case wn::scripting::factory_type::deprecated_engine:
      return (wn::memory::construct<
          WNScripting::WNScriptingEngineImplDeprecated>());
    case wn::scripting::factory_type::jit_engine:
      return (nullptr);
  }
  return (nullptr);
}
