// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_ENGINE_BASE_CONTEXT_H__
#define __WN_ENGINE_BASE_CONTEXT_H__

#include "WNApplicationData/inc/WNApplicationData.h"
#include "WNContainers/inc/WNHashSet.h"
#include "WNMultiTasking/inc/job_pool.h"
#include "WNScripting/inc/WNEngine.h"
namespace wn {
namespace file_system {
class mapping;
}
namespace engine_base {

class context : public scripting::scripting_runtime {
  // TODO(awoloszyn): Make this less public
public:
  context(memory::allocator* _allocator)
    : m_actors(_allocator), m_actors_to_delete(_allocator) {}

  const runtime::application::application_data* m_application_data;
  scripting::engine* m_engine;
  memory::allocator* m_allocator;
  memory::allocator* m_ui_allocator;
  logging::log* m_log;
  file_system::mapping* m_file_mapping;

  scripting::actor_header* allocate_actor(size_t size) override;
  void free_actor(scripting::actor_header* actor) override;
  void call_actor_function(scripting::actor_function* function) override;

private:
  containers::hash_set<scripting::actor_header*> m_actors;
  containers::deque<scripting::actor_header*> m_actors_to_delete;
};

void register_context(scripting::engine* _engine);

}  // namespace engine_base
}  // namespace wn

#endif  // __WN_ENGINE_BASE_CONTEXT_H__
