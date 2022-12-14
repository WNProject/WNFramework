// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_ENGINE_BASE_CONTEXT_H__
#define __WN_ENGINE_BASE_CONTEXT_H__

#include "WNApplicationData/inc/WNApplicationData.h"
#include "WNContainers/inc/WNHashSet.h"
#include "WNMultiTasking/inc/job_pool.h"
#include "WNMultiTasking/inc/spin_lock.h"
#include "WNScripting/inc/WNEngine.h"

#include <chrono>

namespace wn {
namespace file_system {
class mapping;
}
namespace engine_base {

class context : public scripting::scripting_runtime {
  // TODO(awoloszyn): Make this less public
public:
  context(memory::allocator* _allocator,
      const runtime::application::application_data* _application_data)
    : m_application_data(_application_data),
      m_actors(_allocator),
      m_actors_to_delete(_allocator),
      m_actors_to_update(_allocator),
      m_next_actions(_allocator),
      m_time_delayed_actions(_allocator) {}

  const runtime::application::application_data* m_application_data;
  scripting::engine* m_engine;
  memory::allocator* m_allocator;
  memory::allocator* m_ui_allocator;
  logging::log* m_log;
  file_system::mapping* m_file_mapping;

  scripting::actor_header* allocate_actor(size_t size) override;
  void free_actor(scripting::actor_header* actor) override;
  void call_actor_function(
      int32_t delay, scripting::actor_function* function) override;
  logging::log* get_log() override {
    return m_log;
  }
  std::chrono::time_point<std::chrono::high_resolution_clock> update_actors()
      override;

  void wait(multi_tasking::signal_ptr& signal, uint64_t value) override;

private:
  containers::hash_set<scripting::actor_header*> m_actors;
  containers::hash_set<scripting::actor_header*> m_actors_to_delete;
  containers::hash_set<scripting::actor_header*> m_actors_to_update;
  std::atomic<size_t> m_pending_action_count = 0;
  std::atomic<size_t> m_waiting_action_count = 0;
  multi_tasking::spin_lock m_total_lock;
  multi_tasking::condition_variable m_cv;
  multi_tasking::spin_lock m_action_lock;
  multi_tasking::spin_lock m_update_lock;

  multi_tasking::semaphore m_action_sem;
  containers::deque<scripting::actor_function*> m_next_actions;
  containers::dynamic_array<
      std::pair<std::chrono::time_point<std::chrono::high_resolution_clock>,
          scripting::actor_function*>>
      m_time_delayed_actions;
};

void register_context(scripting::engine* _engine);

}  // namespace engine_base
}  // namespace wn

#endif  // __WN_ENGINE_BASE_CONTEXT_H__
