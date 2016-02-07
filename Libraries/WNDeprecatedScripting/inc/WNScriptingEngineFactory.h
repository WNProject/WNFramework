// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef __WN_SCRIPTING_ENGINE_FACTORY_H__
#define __WN_SCRIPTING_ENGINE_FACTORY_H__

namespace WNScripting {
class WNScriptingEngine;
}

namespace wn {
namespace scripting {
enum class factory_type {
  jit_engine,
  deprecated_engine,
};

class scripting_engine_factory {
public:
  static WNScripting::WNScriptingEngine* create_scripting_engine(
      factory_type _type);
};
}
};

#endif  //__WN_SCRIPTING_ENGINE_FACTORY_H__
