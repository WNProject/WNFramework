// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "engine_base/inc/context.h"

#include <algorithm>
#include "WNMultiTasking/inc/semaphore.h"
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

namespace {
struct heap_time_comparator {
  constexpr bool operator()(
      const std::pair<
          std::chrono::time_point<std::chrono::high_resolution_clock>,
          scripting::actor_function*>& lhs,
      const std::pair<
          std::chrono::time_point<std::chrono::high_resolution_clock>,
          scripting::actor_function*>& rhs) {
    return lhs.first > rhs.first;
  }
};
}  // namespace

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
  if (static_cast<uint32_t>(delay) & (1 << 31)) {
    m_action_lock.lock();
    m_time_delayed_actions.push_back(
        std::make_pair(std::chrono::high_resolution_clock::now() +
                           std::chrono::milliseconds(
                               static_cast<uint32_t>(delay) & ~(1 << 31)),
            function));
    std::push_heap(m_time_delayed_actions.begin(), m_time_delayed_actions.end(),
        heap_time_comparator{});
    m_action_lock.unlock();
    // Delaying based on time.
    return;
  }
  WN_DEBUG_ASSERT(delay == 0 || delay == 1, "Unhandled delay value");
  if (delay == 1) {
    m_action_lock.lock();
    m_next_actions.push_back(function);
    m_action_lock.unlock();
    return;
  }
  m_action_lock.lock();
  m_pending_action_count++;
  m_action_lock.unlock();
  auto eh = reinterpret_cast<engine_header*>(function->get_actor_header());
  size_t nv = eh->m_next_value++;
  m_application_data->default_job_pool->add_job(
      "act!", functional::function<void()>(m_allocator, [this, function, nv]() {
        auto eh =
            reinterpret_cast<engine_header*>(function->get_actor_header());
        eh->m_signal.wait_until(nv);
        m_update_lock.lock();
        m_actors_to_update.insert(
            reinterpret_cast<scripting::actor_header*>(eh));
        m_update_lock.unlock();
        m_engine->act(function);

        m_update_lock.lock();
        m_actors_to_update.insert(
            reinterpret_cast<scripting::actor_header*>(eh));
        m_update_lock.unlock();
        eh->m_signal.increment_by(1);
        m_action_sem.notify();

        if (1 == m_pending_action_count--) {
          m_cv.notify_all();
        }
      }));
}

void context::wait(multi_tasking::signal_ptr& signal, uint64_t value) {
  if (!signal.might_wait(value)) {
    return;
  }

  auto actor = scripting::get_scripting_tls()->_actor;
  if (!actor) {
    signal.wait_until(value);
    return;
  }
  auto eh = reinterpret_cast<engine_header*>(actor->get_actor_header());
  eh->m_signal.increment_by(1);
  m_waiting_action_count++;

  m_update_lock.lock();
  m_actors_to_update.insert(reinterpret_cast<scripting::actor_header*>(eh));
  m_update_lock.unlock();

  if (1 == m_pending_action_count--) {
    m_cv.notify_all();
  }

  signal.wait_until(value);
  m_waiting_action_count--;
  m_action_lock.lock();
  m_pending_action_count++;
  m_action_lock.unlock();
  eh->m_signal.wait_until(eh->m_next_value++);
}

std::chrono::time_point<std::chrono::high_resolution_clock>
context::update_actors() {
  m_action_lock.lock();
  m_cv.wait(m_action_lock, [this]() { return m_pending_action_count == 0; });

  // There are currently no active jobs (whoo).
  m_update_lock.lock();
  for (auto& it : m_actors_to_update) {
    // TODO: We can optimize this a LOT, by only
    //       updating the actors that have actually
    //       taken actions (otherwise they remain unchanged)
    m_engine->update(it);
  }

  m_actors_to_update.clear();
  m_update_lock.unlock();
  for (auto a : m_next_actions) {
    if (m_actors_to_delete.find(a->get_actor_header()) !=
        m_actors_to_delete.end()) {
      m_log->log_warning("Deleting actor ", a->actor,
          " which has scheduled actions, dropping actions");
    } else {
      m_pending_action_count++;
      auto eh = reinterpret_cast<engine_header*>(a->get_actor_header());
      size_t nv = eh->m_next_value++;
      m_application_data->default_job_pool->add_job(
          "act!", functional::function<void()>(m_allocator, [this, a, nv]() {
            auto eh = reinterpret_cast<engine_header*>(a->get_actor_header());
            eh->m_signal.wait_until(nv);
            m_engine->act(a);

            m_update_lock.lock();
            m_actors_to_update.insert(
                reinterpret_cast<scripting::actor_header*>(eh));
            m_update_lock.unlock();
            eh->m_signal.increment_by(1);
            if (1 == m_pending_action_count--) {
              m_cv.notify_all();
            }
          }));
    }
  }
  auto now = std::chrono::high_resolution_clock::now();
  while (!m_time_delayed_actions.empty()) {
    auto i = m_time_delayed_actions.front();
    if (i.first > now) {
      break;
    }
    std::pop_heap(m_time_delayed_actions.begin(), m_time_delayed_actions.end(),
        heap_time_comparator{});
    m_time_delayed_actions.pop_back();
    if (m_actors_to_delete.find(i.second->get_actor_header()) !=
        m_actors_to_delete.end()) {
      m_log->log_warning("Deleting actor ", i.second->actor,
          " which has scheduled actions, dropping actions");
      continue;
    }
    m_pending_action_count++;
    auto eh = reinterpret_cast<engine_header*>(i.second->get_actor_header());
    size_t nv = eh->m_next_value++;
    m_application_data->default_job_pool->add_job("act!",
        functional::function<void()>(m_allocator, [this, a = i.second, nv]() {
          auto eh = reinterpret_cast<engine_header*>(a->get_actor_header());
          eh->m_signal.wait_until(nv);
          m_engine->act(a);

          m_update_lock.lock();
          m_actors_to_update.insert(
              reinterpret_cast<scripting::actor_header*>(eh));
          m_update_lock.unlock();
          eh->m_signal.increment_by(1);
          if (1 == m_pending_action_count--) {
            m_cv.notify_all();
          }
        }));
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

  std::chrono::time_point<std::chrono::high_resolution_clock> tp;
  if (!m_waiting_action_count && !m_pending_action_count &&
      !m_time_delayed_actions.empty()) {
    tp = m_time_delayed_actions[0].first;
  }
  m_action_lock.unlock();
  return tp;
}

}  // namespace engine_base
}  // namespace wn
