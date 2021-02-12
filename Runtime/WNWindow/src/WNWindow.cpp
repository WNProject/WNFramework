// Copyright (c) 2021, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNWindow/inc/WNWindow.h"

#include "WNMultiTasking/inc/spin_lock.h"
#include "WNWindow/inc/WNInputContext.h"

namespace wn {
namespace runtime {
namespace window {
window::~window() {}

input_context* window::get_input_context() {
  auto ctx = memory::make_unique<input_context>(m_allocator, m_allocator);
  multi_tasking::spin_lock_guard guard(m_input_context_lock);
  auto ret_ctx = ctx.get();
  m_input_contexts[ctx.get()] = std::move(ctx);
  return ret_ctx;
}

void window::release_input_context(input_context* _context) {
  multi_tasking::spin_lock_guard guard(m_input_context_lock);
  m_input_contexts.erase(_context);
}

void window::dispatch_input(const input_event& event) {
  multi_tasking::spin_lock_guard guard(m_input_context_lock);
  for (auto& input : m_input_contexts) {
    input.first->push_event(event);
  }
}

}  // namespace window
}  // namespace runtime
}  // namespace wn
