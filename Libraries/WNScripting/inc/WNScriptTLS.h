// Copyright (c) 2020, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef __WN_SCRIPTING_SCRIPT_TLS_H__
#define __WN_SCRIPTING_SCRIPT_TLS_H__

#include "WNContainers/inc/WNHashMap.h"
#include "WNMemory/inc/unique_ptr.h"

namespace wn {
namespace scripting {

class engine;
struct script_object_type;

struct scripting_tls_data final {
  engine* _engine;
  containers::hash_map<uintptr_t, memory::unique_ptr<script_object_type>>*
      _object_types;
  logging::log* _log;
  memory::allocator* _support_allocator;
};

extern thread_local const scripting_tls_data* g_scripting_tls;

class tls_resetter final {
public:
  tls_resetter() {
    m_data = g_scripting_tls;
  }

  tls_resetter(const scripting_tls_data* _tls_data) {
    m_data = g_scripting_tls;
    g_scripting_tls = _tls_data;
  }

  ~tls_resetter() {
    g_scripting_tls = m_data;
  }

private:
  const scripting_tls_data* m_data;
};

}  // namespace scripting
}  // namespace wn

#endif  // __WN_SCRIPTING_SCRIPT_TLS_H__
