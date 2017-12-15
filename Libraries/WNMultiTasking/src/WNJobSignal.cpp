// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNMultiTasking/inc/WNJobSignal.h"
#include "WNMultiTasking/inc/WNJobPool.h"

namespace wn {
namespace multi_tasking {

job_signal::job_signal(size_t _value) {
  value = _value;
  pool = job_pool::this_job_pool();
}

job_signal::job_signal() : pool(nullptr), value(0) {}

void job_signal::initialize_signal(job_pool* _pool, size_t _value) {
  WN_DEBUG_ASSERT_DESC(
      pool == nullptr, "You cannot initialize a job_signal twice");
  pool = _pool;
  value.exchange(_value);
}

void job_signal::increment(size_t _value) {
  pool->increment_signal(this, _value);
}

void job_signal::decrement(size_t _value) {
  pool->decrement_signal(this, _value);
}

void job_signal::set(size_t _value) {
  pool->update_signal(this, _value);
}

void job_signal::wait_until(size_t _value) {
  pool->block_job_until(this, _value);
}

size_t job_signal::current_value() {
  return value;
}

void synchronized_destroy_base::wait_for_destruction(synchronized<>* _sync) {
  _sync->schedule_for_destruction();
  _sync->on_destroy();
  wn::multi_tasking::job_signal s(0);
  synchronization_data* data = _sync->get_synchronization_data();
  wn::multi_tasking::job_pool::this_job_pool()->add_job_internal(
      memory::make_unique<synchronized_job<synchronized<>,
          decltype(&synchronized<>::sync_destruction)>>(
          wn::multi_tasking::job_pool::this_job_pool()->m_allocator,
          &data->signal, data->increment_job(), &s, _sync,
          &synchronized<>::sync_destruction));
  s.wait_until(1);
}

}  // namespace multi_tasking
}  // namespace wn
