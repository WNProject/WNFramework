// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNLogging/inc/WNBufferLogger.h"
#include "WNMemory/inc/WNAllocator.h"
#include "WNMultiTasking/inc/WNJobPool.h"
#include "WNMultiTasking/inc/WNSemaphore.h"
#include "WNNetworking/inc/WNNetworkManager.h"
#include "WNTesting/inc/WNTestHarness.h"

void flush_buffer(void* v, const char* bytes, size_t length,
    const std::vector<WNLogging::WNLogColorElement>&) {
  wn::containers::string* s = static_cast<wn::containers::string*>(v);
  s->append(bytes, length);
}

using buffer_logger = WNLogging::WNBufferLogger<flush_buffer>;
using log_buff = wn::containers::string;

using connection_tests = ::testing::TestWithParam<wn::networking::ip_protocol>;

const char* localhost_addresses[2] = {"127.0.0.1", "::1"};

TEST_P(connection_tests, connection) {
  wn::testing::allocator allocator;
  log_buff buffer(&allocator);
  buffer_logger logger(&buffer);
  WNLogging::WNLog log(&logger);

  wn::multi_tasking::thread_job_pool pool(&allocator, 2);
  {
    wn::multi_tasking::semaphore final_semaphore;
    wn::multi_tasking::semaphore listen_started;
    wn::multi_tasking::semaphore wait_for_done;

    wn::networking::WNConcreteNetworkManager manager(&allocator, &pool, &log);

    pool.add_job(wn::multi_tasking::make_job(&allocator, [&]() {
      {
        auto listen_socket = manager.listen_remote_sync(GetParam(), 8080);
        listen_started.notify();
        auto accepted_socket = listen_socket->accept_sync();
        final_semaphore.wait();
      }
      wait_for_done.notify();
    }));

    pool.add_job(wn::multi_tasking::make_job(&allocator, [&]() {
      {
        listen_started.wait();
        auto connect_socket = manager.connect_remote_sync(
            localhost_addresses[static_cast<uint32_t>(GetParam())], GetParam(),
            8080, nullptr);
        final_semaphore.notify();
      }
      wait_for_done.notify();
    }));

    wait_for_done.wait();
    wait_for_done.wait();
  }
}

TEST_P(connection_tests, connect_to_any) {
  wn::testing::allocator allocator;
  log_buff buffer(&allocator);
  buffer_logger logger(&buffer);
  WNLogging::WNLog log(&logger);

  wn::multi_tasking::thread_job_pool pool(&allocator, 2);
  {
    wn::multi_tasking::semaphore final_semaphore;
    wn::multi_tasking::semaphore listen_started;
    wn::multi_tasking::semaphore wait_for_done;

    wn::networking::WNConcreteNetworkManager manager(&allocator, &pool, &log);

    pool.add_job(wn::multi_tasking::make_job(&allocator, [&]() {
      {
        auto listen_socket = manager.listen_remote_sync(GetParam(), 8080);
        listen_started.notify();
        auto accepted_socket = listen_socket->accept_sync();
        final_semaphore.wait();
      }
      wait_for_done.notify();
    }));

    pool.add_job(wn::multi_tasking::make_job(&allocator, [&]() {
      {
        listen_started.wait();
        auto connect_socket = manager.connect_remote_sync(
            localhost_addresses[static_cast<uint32_t>(GetParam())],
            wn::networking::ip_protocol::ip_any, 8080, nullptr);
        final_semaphore.notify();
      }
      wait_for_done.notify();
    }));

    wait_for_done.wait();
    wait_for_done.wait();
  }
}

INSTANTIATE_TEST_CASE_P(all_ip_types, connection_tests,
    ::testing::Values(wn::networking::ip_protocol::ipv4,
                            wn::networking::ip_protocol::ipv6));
