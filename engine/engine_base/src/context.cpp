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
  m_action_lock.lock();
  m_actors.insert(header);
  m_action_lock.unlock();
  return header;
}

void context::free_actor(scripting::actor_header* actor) {
  m_action_lock.lock();
  m_actors_to_delete.insert(actor);
  m_action_lock.unlock();
}

void context::call_actor_function(
    int32_t delay, scripting::actor_function* function) {
  WN_DEBUG_ASSERT(delay == 0 || delay == 1, "Unhandled delay value");
  if (delay == 1) {
    m_action_lock.lock();
    m_next_actions.push_back(function);
    m_action_lock.unlock();
    return;
  }
  ++m_pending_actions;
  auto eh = reinterpret_cast<engine_header*>(function->get_actor_header());
  size_t nv = eh->m_next_value++;
  m_application_data->default_job_pool->add_job(
      "act!", functional::function<void()>(m_allocator, [this, function, nv]() {
        auto eh =
            reinterpret_cast<engine_header*>(function->get_actor_header());
        eh->m_signal.wait_until(nv);
        m_engine->act(function);
        eh->m_signal.increment_by(1);
        if (--m_pending_actions == 0) {
          m_update_signal.increment_by(1);
        }
      }));
}

void context::update_actors() {
  if (m_pending_actions) {
    m_update_signal.wait_until(++m_update_count);
  }
  // There are currently no active jobs (whoo).
  m_action_lock.lock();
  for (auto& it : m_actors) {
    // TODO: We can optimize this a LOT, by only
    //       updating the actors that have actually
    //       taken actions (otherwise they remain unchanged)
    m_engine->update(it);
  }

  for (auto a : m_next_actions) {
    if (m_actors_to_delete.find(a->get_actor_header()) !=
        m_actors_to_delete.end()) {
      m_log->log_warning("Deleting actor ", a->actor,
          " which has scheduled actions, dropping actions");
    } else {
      ++m_pending_actions;
      auto eh = reinterpret_cast<engine_header*>(a->get_actor_header());
      size_t nv = eh->m_next_value++;
      m_application_data->default_job_pool->add_job(
          "act!", functional::function<void()>(m_allocator, [this, a, nv]() {
            auto eh = reinterpret_cast<engine_header*>(a->get_actor_header());
            eh->m_signal.wait_until(nv);
            m_engine->act(a);
            eh->m_signal.increment_by(1);
            if (--m_pending_actions == 0) {
              m_update_signal.increment_by(1);
            }
          }));
    }
  }
  m_next_actions.clear();

  for (auto d : m_actors_to_delete) {
    if (d->destructor) {
      d->destructor(d + 1);
    }
    m_actors.erase(m_actors.find(d));
    m_allocator->deallocate(d);
  }
  m_actors_to_delete.clear();

  m_action_lock.unlock();
}

}  // namespace engine_base
}  // namespace wn
