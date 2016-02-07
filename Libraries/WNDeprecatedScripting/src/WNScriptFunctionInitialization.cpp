// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include <stdlib.h>
#include "WNDeprecatedScripting/inc/WNBuiltinTypeInitialization.h"
#include "WNDeprecatedScripting/inc/WNScriptingEngine.h"

namespace WNScripting {
void* TestMalloc(size_t size) {
  void* x = malloc(size);
  return (x);
}

void TestFree(void* ptr) {
  return (free(ptr));
}
void Assert() {
  assert(false);
}
void DebugPrint(int x) {
  printf("%d\n", x);
}

eWNTypeError WNBuiltIninitializer::InitializeFunctions(
    WNScriptingEngine& _engine) {
  eWNTypeError err = ok;

  if (ok != (err = _engine.register_c_function("Assert", &Assert))) {
    return (err);
  }

  if (ok != (err = _engine.register_c_function("DebugPrint", &DebugPrint))) {
    return (err);
  }

  return (ok);
}
}
