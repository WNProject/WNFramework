// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNLogging/inc/WNBufferLogger.h"
#include "WNMemory/inc/WNAllocator.h"
#include "WNMultiTasking/inc/WNJobPool.h"
#include "WNMultiTasking/inc/WNSemaphore.h"
#include "WNNetworking/inc/WNNetworkManager.h"
#include "WNTesting/inc/WNTestHarness.h"

void flush_buffer(void* v, const char* bytes, size_t length,
    const wn::logging::color_element*, size_t) {
  wn::containers::string* s = static_cast<wn::containers::string*>(v);
  s->append(bytes, length);
}

using buffer_logger = wn::logging::buffer_logger<flush_buffer>;
using log_buff = wn::containers::string;

using connection_tests = ::testing::TestWithParam<wn::networking::ip_protocol>;

const char* localhost_addresses[2] = {"127.0.0.1", "::1"};

TEST_P(connection_tests, connection) {
  wn::testing::allocator allocator;
  log_buff buffer(&allocator);
  buffer_logger logger(&buffer);
  wn::logging::static_log<> slog(&logger);
  wn::logging::log* log = slog.log();

  wn::multi_tasking::thread_job_pool pool(&allocator, 2);
  {
    wn::multi_tasking::semaphore final_semaphore;
    wn::multi_tasking::semaphore listen_started;
    wn::multi_tasking::semaphore wait_for_done;

    wn::networking::WNConcreteNetworkManager manager(&allocator, &pool, log);

    pool.add_unsynchronized_job(nullptr, [&]() {
      {
        auto listen_socket = manager.listen_remote_sync(GetParam(), 8080);
        listen_started.notify();
        auto accepted_socket = listen_socket->accept_sync();
        ASSERT_NE(nullptr, accepted_socket);
        final_semaphore.wait();
      }
      wait_for_done.notify();
    });

    pool.add_unsynchronized_job(nullptr, [&]() {
      {
        listen_started.wait();
        auto connect_socket = manager.connect_remote_sync(
            localhost_addresses[static_cast<uint32_t>(GetParam())], GetParam(),
            8080, nullptr);
        ASSERT_NE(nullptr, connect_socket);
        final_semaphore.notify();
      }
      wait_for_done.notify();
    });

    wait_for_done.wait();
    wait_for_done.wait();
  }
}

TEST_P(connection_tests, connect_to_any) {
  wn::testing::allocator allocator;
  log_buff buffer(&allocator);
  buffer_logger logger(&buffer);
  wn::logging::static_log<> slog(&logger);
  wn::logging::log* log = slog.log();

  wn::multi_tasking::thread_job_pool pool(&allocator, 2);
  {
    wn::multi_tasking::semaphore final_semaphore;
    wn::multi_tasking::semaphore listen_started;
    wn::multi_tasking::semaphore wait_for_done;

    wn::networking::WNConcreteNetworkManager manager(&allocator, &pool, log);

    pool.add_unsynchronized_job(nullptr, [&]() {
      {
        auto listen_socket = manager.listen_remote_sync(GetParam(), 8080);
        listen_started.notify();
        auto accepted_socket = listen_socket->accept_sync();
        ASSERT_NE(nullptr, accepted_socket);
        final_semaphore.wait();
      }
      wait_for_done.notify();
    });

    pool.add_unsynchronized_job(nullptr, [&]() {
      {
        listen_started.wait();
        auto connect_socket = manager.connect_remote_sync(
            localhost_addresses[static_cast<uint32_t>(GetParam())],
            wn::networking::ip_protocol::ip_any, 8080, nullptr);
        ASSERT_NE(nullptr, connect_socket);
        final_semaphore.notify();
      }
      wait_for_done.notify();
    });

    wait_for_done.wait();
    wait_for_done.wait();
  }
}

TEST(raw_connection, send_data_from_server) {
  wn::testing::allocator allocator;
  log_buff buffer(&allocator);
  buffer_logger logger(&buffer);
  wn::logging::static_log<> slog(&logger);
  wn::logging::log* log = slog.log();

  wn::multi_tasking::thread_job_pool pool(&allocator, 2);
  {
    wn::multi_tasking::semaphore final_semaphore;
    wn::multi_tasking::semaphore listen_started;
    wn::multi_tasking::semaphore wait_for_done;

    wn::networking::WNConcreteNetworkManager manager(&allocator, &pool, log);

    pool.add_unsynchronized_job(nullptr, [&]() {
      {
        auto listen_socket =
            manager.listen_remote_sync(wn::networking::ip_protocol::ipv4, 8080);
        listen_started.notify();
        auto accepted_socket = listen_socket->accept_sync();
        ASSERT_NE(nullptr, accepted_socket);
        wn::containers::string my_str("hello_world", &allocator);
        accepted_socket->get_send_pipe()->send_sync({wn::networking::send_range(
            reinterpret_cast<const uint8_t*>(my_str.data()), my_str.length())});
      }
      wait_for_done.notify();
    });

    pool.add_unsynchronized_job(nullptr, [&]() {
      {
        listen_started.wait();
        auto connect_socket = manager.connect_remote_sync(
            "127.0.0.1", wn::networking::ip_protocol::ipv4, 8080, nullptr);
        ASSERT_NE(nullptr, connect_socket);
        auto buff = connect_socket->get_recv_pipe()->recv_sync();
        ASSERT_EQ(wn::networking::network_error::ok, buff.get_status());
        ASSERT_EQ("hello_world",
            wn::containers::string(buff.data.data(), buff.data.size()));
      }
      wait_for_done.notify();
    });

    wait_for_done.wait();
    wait_for_done.wait();
  }
}

TEST(raw_connection, send_data_from_client) {
  wn::testing::allocator allocator;
  log_buff buffer(&allocator);
  buffer_logger logger(&buffer);
  wn::logging::static_log<> slog(&logger);
  wn::logging::log* log = slog.log();

  wn::multi_tasking::thread_job_pool pool(&allocator, 2);
  {
    wn::multi_tasking::semaphore final_semaphore;
    wn::multi_tasking::semaphore listen_started;
    wn::multi_tasking::semaphore wait_for_done;

    wn::networking::WNConcreteNetworkManager manager(&allocator, &pool, log);

    pool.add_unsynchronized_job(nullptr, [&]() {
      {
        auto listen_socket =
            manager.listen_remote_sync(wn::networking::ip_protocol::ipv4, 8080);
        listen_started.notify();
        auto accepted_socket = listen_socket->accept_sync();
        ASSERT_NE(nullptr, accepted_socket);
        auto buff = accepted_socket->get_recv_pipe()->recv_sync();
        ASSERT_EQ(wn::networking::network_error::ok, buff.get_status());
        ASSERT_EQ("hello_world",
            wn::containers::string(buff.data.data(), buff.data.size()));
      }
      wait_for_done.notify();
    });

    pool.add_unsynchronized_job(nullptr, [&]() {
      {
        listen_started.wait();
        auto connect_socket = manager.connect_remote_sync(
            "127.0.0.1", wn::networking::ip_protocol::ipv4, 8080, nullptr);
        ASSERT_NE(nullptr, connect_socket);
        wn::containers::string my_str("hello_world", &allocator);
        connect_socket->get_send_pipe()->send_sync({wn::networking::send_range(
            reinterpret_cast<const uint8_t*>(my_str.data()), my_str.length())});
      }
      wait_for_done.notify();
    });

    wait_for_done.wait();
    wait_for_done.wait();
  }
}

TEST(raw_connection, multi_send) {
  wn::testing::allocator allocator;
  log_buff buffer(&allocator);
  buffer_logger logger(&buffer);
  wn::logging::static_log<> slog(&logger);
  wn::logging::log* log = slog.log();

  wn::multi_tasking::thread_job_pool pool(&allocator, 2);
  {
    wn::multi_tasking::semaphore final_semaphore;
    wn::multi_tasking::semaphore listen_started;
    wn::multi_tasking::semaphore wait_for_done;

    wn::networking::WNConcreteNetworkManager manager(&allocator, &pool, log);

    pool.add_unsynchronized_job(nullptr, [&]() {
      {
        auto listen_socket =
            manager.listen_remote_sync(wn::networking::ip_protocol::ipv4, 8080);
        listen_started.notify();
        auto accepted_socket = listen_socket->accept_sync();
        ASSERT_NE(nullptr, accepted_socket);
        const char* hello = "hello";
        const char* world = "_world";

        accepted_socket->get_send_pipe()->send_sync(
            {{wn::networking::send_range(
                 reinterpret_cast<const uint8_t*>(hello), strlen(hello))},
                {wn::networking::send_range(
                    reinterpret_cast<const uint8_t*>(world), strlen(world))}});
      }
      wait_for_done.notify();
    });

    pool.add_unsynchronized_job(nullptr, [&]() {
      {
        listen_started.wait();
        auto connect_socket = manager.connect_remote_sync(
            "127.0.0.1", wn::networking::ip_protocol::ipv4, 8080, nullptr);
        ASSERT_NE(nullptr, connect_socket);
        auto buff = connect_socket->get_recv_pipe()->recv_sync();
        ASSERT_EQ(wn::networking::network_error::ok, buff.get_status());
        EXPECT_EQ(wn::containers::string("hello_world"),
            wn::containers::string(buff.data.data(), buff.data.size()));
      }
      wait_for_done.notify();
    });

    wait_for_done.wait();
    wait_for_done.wait();
  }
}

INSTANTIATE_TEST_CASE_P(all_ip_types, connection_tests,
    ::testing::Values(
        wn::networking::ip_protocol::ipv4, wn::networking::ip_protocol::ipv6));
