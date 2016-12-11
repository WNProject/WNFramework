// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNLogging/inc/WNBufferLogger.h"
#include "WNMemory/inc/WNAllocator.h"
#include "WNMultiTasking/inc/WNJobPool.h"
#include "WNMultiTasking/inc/WNSemaphore.h"
#include "WNNetworking/inc/WNNetworkManager.h"
#include "WNNetworking/inc/WNRoutedConnection.h"
#include "WNTesting/inc/WNTestHarness.h"

void flush_buffer(void* v, const char* bytes, size_t length,
    const std::vector<WNLogging::WNLogColorElement>&) {
  wn::containers::string* s = static_cast<wn::containers::string*>(v);
  s->append(bytes, length);
}

using buffer_logger = WNLogging::WNBufferLogger<flush_buffer>;
using log_buff = wn::containers::string;

TEST(routed_connection, default_route) {
  wn::testing::allocator allocator;
  log_buff buffer(&allocator);
  buffer_logger logger(&buffer);
  WNLogging::WNLog log(&logger);
  {
    wn::multi_tasking::thread_job_pool pool(&allocator, 3);
    {
      wn::multi_tasking::job_signal signal(&pool, 0);
      wn::multi_tasking::semaphore wait_for_done;

      wn::networking::WNConcreteNetworkManager manager(&allocator, &pool, &log);

      pool.add_unsynchronized_job(nullptr, [&]() {
        {
          auto listen_socket = manager.listen_remote_sync(
              wn::networking::ip_protocol::ipv4, 8080);

          const char* sent_message = "HelloWorld";
          struct connection_manager : wn::multi_tasking::synchronized<> {
            void on_accept(
                wn::memory::unique_ptr<wn::networking::WNRoutedConnection>
                    conn) {
              wn::multi_tasking::job_signal signal(0);
              for (size_t i = 0; i < 10; ++i) {
                conn->send_async(0, &signal, nullptr,
                    {wn::networking::send_range(
                        reinterpret_cast<const uint8_t*>(sent_message),
                        strlen(sent_message))});
              }
              signal.wait_until(10);
            }
            void default_route(wn::networking::RoutedMessage&&) {
              main_signal->increment(1);
            }
            const char* sent_message;
            wn::multi_tasking::job_signal* main_signal;
            connection_manager(
                const char* _s, wn::multi_tasking::job_signal* main_signal)
              : sent_message(_s), main_signal(main_signal) {}
          } mgr(sent_message, &signal);
          auto routed_accept = wn::networking::WNRoutedAcceptConnection(
              &allocator, wn::core::move(listen_socket), &pool,
              wn::multi_tasking::make_callback(
                  &allocator, &mgr, &connection_manager::on_accept),
              wn::multi_tasking::make_callback(
                  &allocator, &mgr, &connection_manager::default_route));

          wn::containers::string receieved(&allocator);
          struct received_message : wn::multi_tasking::synchronized<> {
            received_message(wn::multi_tasking::job_signal& signal,
                wn::memory::allocator* _alloc, const char* sent_message)
              : signal(signal), allocator(_alloc), sent_message(sent_message) {}
            void default_route(wn::networking::RoutedMessage&& message) {
              if (message.m_status != wn::networking::network_error::ok) {
                signal.increment(1);
                return;
              }
              auto received = wn::containers::string(
                  message.data.data(), message.data.size(), allocator);
              EXPECT_EQ(
                  wn::containers::string(sent_message, allocator), received);
              signal.increment(1);
            }
            wn::multi_tasking::job_signal& signal;
            wn::memory::allocator* allocator;
            const char* sent_message;
          } recv(signal, &allocator, sent_message);
          wn::networking::WNRoutedConnection routed_conn(&allocator,
              wn::multi_tasking::make_callback(
                  &allocator, &recv, &received_message::default_route),
              manager.connect_remote_sync("127.0.0.1",
                  wn::networking::ip_protocol::ipv4, 8080, nullptr),
              &pool);

          signal.wait_until(12);
        }
        wait_for_done.notify();
      });
      wait_for_done.wait();
    }
  }
}

TEST(routed_connection, all_are_default) {
  wn::testing::allocator allocator;
  log_buff buffer(&allocator);
  buffer_logger logger(&buffer);
  WNLogging::WNLog log(&logger);
  {
    wn::multi_tasking::thread_job_pool pool(&allocator, 3);
    {
      wn::multi_tasking::job_signal signal(&pool, 0);
      wn::multi_tasking::semaphore wait_for_done;

      wn::networking::WNConcreteNetworkManager manager(&allocator, &pool, &log);

      pool.add_unsynchronized_job(nullptr, [&]() {
        {
          auto listen_socket = manager.listen_remote_sync(
              wn::networking::ip_protocol::ipv4, 8080);

          const char* sent_message = "HelloWorld";
          struct connection_manager : wn::multi_tasking::synchronized<> {
            void on_accept(
                wn::memory::unique_ptr<wn::networking::WNRoutedConnection>
                    conn) {
              wn::multi_tasking::job_signal signal(0);
              for (uint32_t i = 0; i < 10; ++i) {
                conn->send_async(i, &signal, nullptr,
                    {wn::networking::send_range(
                        reinterpret_cast<const uint8_t*>(sent_message),
                        strlen(sent_message))});
              }
              signal.wait_until(10);
            }
            void default_route(wn::networking::RoutedMessage&&) {
              main_signal->increment(1);
            }
            const char* sent_message;
            wn::multi_tasking::job_signal* main_signal;
            connection_manager(
                const char* _s, wn::multi_tasking::job_signal* main_signal)
              : sent_message(_s), main_signal(main_signal) {}
          } mgr(sent_message, &signal);
          auto routed_accept = wn::networking::WNRoutedAcceptConnection(
              &allocator, wn::core::move(listen_socket), &pool,
              wn::multi_tasking::make_callback(
                  &allocator, &mgr, &connection_manager::on_accept),
              wn::multi_tasking::make_callback(
                  &allocator, &mgr, &connection_manager::default_route));

          wn::containers::string receieved(&allocator);
          struct received_message : wn::multi_tasking::synchronized<> {
            received_message(wn::multi_tasking::job_signal& signal,
                wn::memory::allocator* _alloc, const char* sent_message)
              : signal(signal), allocator(_alloc), sent_message(sent_message) {}
            void default_route(wn::networking::RoutedMessage&& message) {
              if (message.m_status != wn::networking::network_error::ok) {
                signal.increment(1);
                return;
              }
              auto received = wn::containers::string(
                  message.data.data(), message.data.size(), allocator);
              EXPECT_EQ(
                  wn::containers::string(sent_message, allocator), received);
              signal.increment(1);
            }
            wn::multi_tasking::job_signal& signal;
            wn::memory::allocator* allocator;
            const char* sent_message;
          } recv(signal, &allocator, sent_message);
          wn::networking::WNRoutedConnection routed_conn(&allocator,
              wn::multi_tasking::make_callback(
                  &allocator, &recv, &received_message::default_route),
              manager.connect_remote_sync("127.0.0.1",
                  wn::networking::ip_protocol::ipv4, 8080, nullptr),
              &pool);

          signal.wait_until(12);
        }
        wait_for_done.notify();
      });
      wait_for_done.wait();
    }
  }
}

TEST(routed_connection, multiple_routes) {
  wn::testing::allocator allocator;
  log_buff buffer(&allocator);
  buffer_logger logger(&buffer);
  WNLogging::WNLog log(&logger);
  {
    wn::multi_tasking::thread_job_pool pool(&allocator, 3);
    {
      wn::multi_tasking::job_signal signal(&pool, 0);
      wn::multi_tasking::semaphore wait_for_done;

      wn::networking::WNConcreteNetworkManager manager(&allocator, &pool, &log);

      pool.add_unsynchronized_job(nullptr, [&]() {
        {
          auto listen_socket = manager.listen_remote_sync(
              wn::networking::ip_protocol::ipv4, 8080);

          const char* sent_message = "HelloWorld";
          const char* sent_message2 = "foo";
          const char* sent_message32 = "bar";
          struct connection_manager : wn::multi_tasking::synchronized<> {
            void on_accept(
                wn::memory::unique_ptr<wn::networking::WNRoutedConnection>
                    conn) {
              main_signal->wait_until(2);
              wn::multi_tasking::job_signal signal(0);
              for (uint32_t i = 0; i < 10; ++i) {
                conn->send_async(0, &signal, nullptr,
                    {wn::networking::send_range(
                        reinterpret_cast<const uint8_t*>(sent_message),
                        strlen(sent_message))});
                conn->send_async(2, &signal, nullptr,
                    {wn::networking::send_range(
                        reinterpret_cast<const uint8_t*>(sent_message2),
                        strlen(sent_message2))});
                conn->send_async(32, &signal, nullptr,
                    {wn::networking::send_range(
                        reinterpret_cast<const uint8_t*>(sent_message32),
                        strlen(sent_message32))});
              }
              signal.wait_until(30);
            }
            void default_route(wn::networking::RoutedMessage&&) {
              main_signal->increment(1);
            }
            const char* sent_message;
            const char* sent_message2;
            const char* sent_message32;
            wn::multi_tasking::job_signal* main_signal;
            connection_manager(const char* _s, const char* _s2,
                const char* _s32, wn::multi_tasking::job_signal* main_signal)
              : sent_message(_s),
                sent_message2(_s2),
                sent_message32(_s32),
                main_signal(main_signal) {}
          } mgr(sent_message, sent_message2, sent_message32, &signal);
          auto routed_accept = wn::networking::WNRoutedAcceptConnection(
              &allocator, wn::core::move(listen_socket), &pool,
              wn::multi_tasking::make_callback(
                  &allocator, &mgr, &connection_manager::on_accept),
              wn::multi_tasking::make_callback(
                  &allocator, &mgr, &connection_manager::default_route));

          wn::containers::string receieved(&allocator);
          struct received_message : wn::multi_tasking::synchronized<> {
            received_message(wn::multi_tasking::job_signal& signal,
                wn::memory::allocator* _alloc, const char* sent_message,
                const char* sent_message2, const char* sent_message32)
              : signal(signal),
                allocator(_alloc),
                sent_message(sent_message),
                sent_message2(sent_message2),
                sent_message32(sent_message32) {}
            void default_route(wn::networking::RoutedMessage&& message) {
              if (message.m_status != wn::networking::network_error::ok) {
                signal.increment(1);
                return;
              }
              auto received = wn::containers::string(
                  message.data.data(), message.data.size(), allocator);
              EXPECT_EQ(
                  wn::containers::string(sent_message, allocator), received);
              signal.increment(1);
            }
            void route32(wn::networking::RoutedMessage&& message) {
              auto received = wn::containers::string(
                  message.data.data(), message.data.size(), allocator);
              EXPECT_EQ(
                  wn::containers::string(sent_message32, allocator), received);
              signal.increment(1);
            }
            void route2(wn::networking::RoutedMessage&& message) {
              auto received = wn::containers::string(
                  message.data.data(), message.data.size(), allocator);
              EXPECT_EQ(
                  wn::containers::string(sent_message2, allocator), received);
              signal.increment(1);
            }
            wn::multi_tasking::job_signal& signal;
            wn::memory::allocator* allocator;
            const char* sent_message;
            const char* sent_message2;
            const char* sent_message32;
          } recv(
              signal, &allocator, sent_message, sent_message2, sent_message32);
          wn::networking::WNRoutedConnection routed_conn(&allocator,
              wn::multi_tasking::make_callback(
                  &allocator, &recv, &received_message::default_route),
              manager.connect_remote_sync("127.0.0.1",
                  wn::networking::ip_protocol::ipv4, 8080, nullptr),
              &pool);
          routed_conn.recv_async(
              2, &signal, wn::multi_tasking::make_callback(
                              &allocator, &recv, &received_message::route2));
          routed_conn.recv_async(
              32, &signal, wn::multi_tasking::make_callback(
                               &allocator, &recv, &received_message::route32));
          signal.wait_until(34);
        }
        wait_for_done.notify();
      });
      wait_for_done.wait();
    }
  }
}

TEST(routed_connection, multipart_message) {
  wn::testing::allocator allocator;
  log_buff buffer(&allocator);
  buffer_logger logger(&buffer);
  WNLogging::WNLog log(&logger);
  {
    wn::multi_tasking::thread_job_pool pool(&allocator, 3);
    {
      wn::multi_tasking::job_signal signal(&pool, 0);
      wn::multi_tasking::semaphore wait_for_done;

      wn::networking::WNConcreteNetworkManager manager(&allocator, &pool, &log);

      pool.add_unsynchronized_job(nullptr, [&]() {
        {
          auto listen_socket = manager.listen_remote_sync(
              wn::networking::ip_protocol::ipv4, 8080);

          const char* sent_message = "HelloWorld";
          struct connection_manager : wn::multi_tasking::synchronized<> {
            void on_accept(
                wn::memory::unique_ptr<wn::networking::WNRoutedConnection>
                    conn) {
              wn::multi_tasking::job_signal signal(0);

              auto token = conn->start_multipart_message(0, &signal, nullptr,
                  0xFFFFFFFFFFFFFFFF,
                  {wn::networking::send_range(
                      reinterpret_cast<const uint8_t*>(sent_message),
                      strlen(sent_message))});

              token = conn->continue_multipart_message(wn::core::move(token),
                  &signal, nullptr,
                  {wn::networking::send_range(
                      reinterpret_cast<const uint8_t*>(sent_message),
                      strlen(sent_message))});

              conn->finish_multipart_message(wn::core::move(token), &signal,
                  nullptr, {wn::networking::send_range(
                               reinterpret_cast<const uint8_t*>(sent_message),
                               strlen(sent_message))});

              signal.wait_until(3);
            }
            void default_route(wn::networking::RoutedMessage&&) {
              main_signal->increment(1);
            }
            const char* sent_message;
            wn::multi_tasking::job_signal* main_signal;
            connection_manager(
                const char* _s, wn::multi_tasking::job_signal* main_signal)
              : sent_message(_s), main_signal(main_signal) {}
          } mgr(sent_message, &signal);
          auto routed_accept = wn::networking::WNRoutedAcceptConnection(
              &allocator, wn::core::move(listen_socket), &pool,
              wn::multi_tasking::make_callback(
                  &allocator, &mgr, &connection_manager::on_accept),
              wn::multi_tasking::make_callback(
                  &allocator, &mgr, &connection_manager::default_route));

          wn::containers::string receieved(&allocator);
          struct received_message : wn::multi_tasking::synchronized<> {
            received_message(wn::multi_tasking::job_signal& signal,
                wn::memory::allocator* _alloc, const char* sent_message)
              : signal(signal), allocator(_alloc), sent_message(sent_message) {}
            void default_route(wn::networking::RoutedMessage&& message) {
              if (message.m_status != wn::networking::network_error::ok) {
                signal.increment(1);
                return;
              }
              auto received = wn::containers::string(
                  message.data.data(), message.data.size(), allocator);
              EXPECT_EQ(
                  message_idx * strlen(sent_message), message.m_message_offset);
              EXPECT_EQ(
                  wn::containers::string(sent_message, allocator), received);
              if (message_idx == 2) {
                EXPECT_EQ(message.m_message_offset + message.data.size(),
                    message.m_message_size);
              }
              signal.increment(1);
              ++message_idx;
            }
            wn::multi_tasking::job_signal& signal;
            wn::memory::allocator* allocator;
            const char* sent_message;
            size_t message_idx = 0;
          } recv(signal, &allocator, sent_message);
          wn::networking::WNRoutedConnection routed_conn(&allocator,
              wn::multi_tasking::make_callback(
                  &allocator, &recv, &received_message::default_route),
              manager.connect_remote_sync("127.0.0.1",
                  wn::networking::ip_protocol::ipv4, 8080, nullptr),
              &pool);

          signal.wait_until(5);
        }
        wait_for_done.notify();
      });
      wait_for_done.wait();
    }
  }
}
