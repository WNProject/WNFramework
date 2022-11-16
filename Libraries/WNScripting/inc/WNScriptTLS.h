// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef __WN_SCRIPTING_SCRIPT_TLS_H__
#define __WN_SCRIPTING_SCRIPT_TLS_H__

#include "WNContainers/inc/WNHashMap.h"
#include "WNFileSystem/inc/WNMapping.h"
#include "WNMemory/inc/unique_ptr.h"

namespace wn {
namespace scripting {

class engine;
struct script_object_type;
struct script_actor_type;

struct actor_header {
  void* user_data;
  void* user_data2;
  std::atomic<size_t> ref_count;
  void (*destructor)(void*);
  void (*update_values)(void*);
};

struct actor_function {
  void (*function)(void*);
  void* actor;
  actor_header* get_actor_header() {
    return reinterpret_cast<actor_header*>(actor) - 1;
  }
};

class scripting_runtime {
public:
  virtual ~scripting_runtime() {}
  // You are expected to allocate memory of the given size
  // (which will be LARGER than an actor_header).
  // You are then allowed to fill in both user_data and
  // user_data2 with anything you want.
  virtual actor_header* allocate_actor(size_t size) = 0;
  virtual void free_actor(actor_header* actor) = 0;
  // Assign an action to an actor. This does not have to be
  // accomplished immediately, but you are responsible for
  // making sure that no actor is completing 2 actions at
  // the same time.
  virtual void call_actor_function(int32_t delay, actor_function* function) = 0;

  virtual void update_actors() = 0;
};

struct scripting_tls_data final {
  engine* _engine;
  containers::hash_map<uintptr_t, memory::unique_ptr<script_object_type>>*
      _object_types;
  containers::hash_map<uintptr_t, memory::unique_ptr<script_actor_type>>*
      _actor_types;
  logging::log* _log;
  memory::allocator* _support_allocator;
  memory::allocator* _actor_allocator;
  scripting_runtime* _runtime;
};

const scripting_tls_data*& get_scripting_tls();

extern thread_local const scripting_tls_data* g_scripting_tls;

class tls_resetter final {
public:
  tls_resetter() {
    m_data = get_scripting_tls();
  }

  tls_resetter(const scripting_tls_data* _tls_data) {
    m_data = get_scripting_tls();
    get_scripting_tls() = _tls_data;
  }

  ~tls_resetter() {
    get_scripting_tls() = m_data;
  }

private:
  const scripting_tls_data* m_data;
};

}  // namespace scripting
}  // namespace wn

#endif  // __WN_SCRIPTING_SCRIPT_TLS_H__
