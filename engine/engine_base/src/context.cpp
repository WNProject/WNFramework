// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "engine_base/inc/context.h"

#include "WNScripting/inc/WNEngine.h"

using namespace wn;

// First let us export the context to scripting.
namespace wn {
namespace scripting {
template <>
struct exported_script_type<engine_base::context> {
  static containers::string_view exported_name() {
    return "Context";
  }

  template <typename T>
  static void export_type(T*) {}
};
}  // namespace scripting
}  // namespace wn

namespace wn {
namespace engine_base {
void register_context(scripting::engine* _engine) {
  _engine->register_cpp_type<engine_base::context>();
}

struct engine_header {
  multi_tasking::signal_ptr m_signal = nullptr;
  std::atomic<size_t> m_next_value = 0;
};

scripting::actor_header* context::allocate_actor(size_t size) {
  auto header =
      reinterpret_cast<scripting::actor_header*>(m_allocator->allocate(size));
  engine_header* eg = reinterpret_cast<engine_header*>(header);
  eg->m_signal = m_application_data->default_job_pool->get_signal();
  eg->m_next_value = 0;
  return header;
}

void context::free_actor(scripting::actor_header* actor) {
  m_actors_to_delete.push_back(actor);
}

void context::call_actor_function(scripting::actor_function* function) {
  auto eh = reinterpret_cast<engine_header*>(function->get_actor_header());
  size_t nv = eh->m_next_value++;
  m_application_data->default_job_pool->add_job("act!", functional::function<void()>(
                  m_allocator, [function, nv]() {
    auto eh =
            reinterpret_cast<engine_header*>(function->get_actor_header());
    eh->m_signal.wait_until(nv);
        function->function(function->actor);
  }));
}


}  // namespace engine_base
}  // namespace wn
