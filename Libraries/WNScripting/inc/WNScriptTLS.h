// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef __WN_SCRIPTING_SCRIPT_TLS_H__
#define __WN_SCRIPTING_SCRIPT_TLS_H__

#include "WNContainers/inc/WNHashMap.h"
#include "WNMemory/inc/unique_ptr.h"

namespace wn {
namespace scripting {
class engine;
struct script_object_type;

struct scripting_tls_data {
  engine* _engine;
  containers::hash_map<uintptr_t, memory::unique_ptr<script_object_type>>*
      _object_types;
};

extern thread_local const scripting_tls_data* g_scripting_tls;

class tls_resetter {
public:
  tls_resetter() {
    _data = g_scripting_tls;
  }
  ~tls_resetter() {
    g_scripting_tls = _data;
  }

private:
  const scripting_tls_data* _data;
};

}  // namespace scripting
}  // namespace wn

#endif  // __WN_SCRIPTING_SCRIPT_TLS_H__
