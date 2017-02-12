// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNExecutableTest/inc/WNTestHarness.h"
#include "WNLogging/inc/WNBufferLogger.h"
#include "WNMemory/inc/WNAllocator.h"
#include "WNMultiTasking/inc/WNJobPool.h"
#include "WNMultiTasking/inc/WNSemaphore.h"
#include "WNNetworking/inc/WNNetworkManager.h"

void flush_buffer(void* v, const char* bytes, size_t length,
    const wn::logging::color_element*, size_t) {
  wn::containers::string* s = static_cast<wn::containers::string*>(v);
  s->append(bytes, length);
}

using buffer_logger = wn::logging::buffer_logger<flush_buffer>;
using log_buff = wn::containers::string;

TEST(raw_connection, send_async_data_from_server) {
  wn::testing::allocator allocator;
  log_buff buffer(&allocator);
  buffer_logger logger(&buffer);
  wn::logging::static_log<> slog(&logger);
  wn::logging::log* log = slog.log();
  {
    wn::multi_tasking::thread_job_pool pool(&allocator, 1);
    {
      wn::multi_tasking::job_signal listen_started(&pool, 0);
      wn::multi_tasking::semaphore wait_for_done;

      wn::networking::WNConcreteNetworkManager manager(&allocator, &pool, log);

      pool.add_unsynchronized_job(nullptr, [&]() {
        {
          auto listen_socket = manager.listen_remote_sync(
              wn::networking::ip_protocol::ipv4, 8080);
          listen_started.increment(1);
          auto accepted_socket = pool.call_blocking_function<
              wn::memory::unique_ptr<wn::networking::WNConnection>>(
              &wn::networking::WNAcceptConnection::accept_sync,
              listen_socket.get(), nullptr);
          ASSERT_NE(nullptr, accepted_socket);
          wn::containers::string my_str("hello_world", &allocator);
          wn::multi_tasking::job_signal signal(0);
          for (size_t i = 0; i < 10; ++i) {
            accepted_socket->get_send_pipe()->send_async(&signal, nullptr,
                {wn::networking::send_range(
                    reinterpret_cast<const uint8_t*>(my_str.data()),
                    my_str.length())});
          }
          signal.wait_until(10);
        }
        wait_for_done.notify();
      });

      pool.add_unsynchronized_job(nullptr, [&]() {
        {
          listen_started.wait_until(1);
          auto connect_socket = pool.call_blocking_function<
              wn::memory::unique_ptr<wn::networking::WNReliableConnection>>(
              [&]() {
                return manager.connect_remote_sync("127.0.0.1",
                    wn::networking::ip_protocol::ipv4, 8080, nullptr);
              });
          wn::containers::string received(&allocator);
          wn::containers::string expected(
              "hello_worldhello_worldhello_world"
              "hello_worldhello_worldhello_worldhello_worldhello_world"
              "hello_worldhello_world",
              &allocator);
          ASSERT_NE(nullptr, connect_socket);
          while (received != expected) {
            auto buff =
                pool.call_blocking_function<wn::networking::WNReceiveBuffer>(
                    [&]() {
                      return connect_socket->get_recv_pipe()->recv_sync();
                    });
            ASSERT_EQ(wn::networking::network_error::ok, buff.get_status());
            received += wn::containers::string(
                buff.data.data(), buff.data.size(), &allocator);
          }
        }
        wait_for_done.notify();
      });

      wait_for_done.wait();
      wait_for_done.wait();
    }
  }
}

TEST(raw_connection, async_accept) {
  wn::testing::allocator allocator;
  log_buff buffer(&allocator);
  buffer_logger logger(&buffer);
  wn::logging::static_log<> slog(&logger);
  wn::logging::log* log = slog.log();

  wn::multi_tasking::thread_job_pool pool(&allocator, 1);
  {
    wn::multi_tasking::job_signal listen_started(&pool, 0);
    wn::multi_tasking::semaphore wait_for_done;

    wn::networking::WNConcreteNetworkManager manager(&allocator, &pool, log);

    pool.add_unsynchronized_job(nullptr, [&]() {
      {
        auto listen_socket =
            manager.listen_remote_sync(wn::networking::ip_protocol::ipv4, 8080);
        listen_started.increment(1);

        wn::multi_tasking::job_signal signal(0);
        wn::containers::string my_str("hello_world", &allocator);

        struct cb_st {
          wn::containers::string& str;
          wn::multi_tasking::job_signal& signal;
          void accept(
              wn::memory::unique_ptr<wn::networking::WNConnection> connection) {
            ASSERT_NE(nullptr, connection.get());
            for (size_t i = 0; i < 10; ++i) {
              connection->get_send_pipe()->send_async(&signal, nullptr,
                  {wn::networking::send_range(
                      reinterpret_cast<const uint8_t*>(str.data()),
                      str.length())});
            }
            signal.wait_until(10);
            signal.increment(1);
          }
        } my_cb = {my_str, signal};

        listen_socket->accept_async(wn::multi_tasking::make_callback(
            &allocator, &my_cb, &cb_st::accept));
        signal.wait_until(11);
      }
      wait_for_done.notify();
    });

    pool.add_unsynchronized_job(nullptr, [&]() {
      {
        listen_started.wait_until(1);
        auto connect_socket = pool.call_blocking_function<
            wn::memory::unique_ptr<wn::networking::WNReliableConnection>>(
            [&]() {
              return manager.connect_remote_sync("127.0.0.1",
                  wn::networking::ip_protocol::ipv4, 8080, nullptr);
            });
        wn::containers::string received(&allocator);
        wn::containers::string expected(
            "hello_worldhello_worldhello_world"
            "hello_worldhello_worldhello_worldhello_worldhello_world"
            "hello_worldhello_world",
            &allocator);
        ASSERT_NE(nullptr, connect_socket);
        while (received != expected) {
          auto buff =
              pool.call_blocking_function<wn::networking::WNReceiveBuffer>(
                  [&]() {
                    return connect_socket->get_recv_pipe()->recv_sync();
                  });
          ASSERT_EQ(wn::networking::network_error::ok, buff.get_status());
          received += wn::containers::string(
              buff.data.data(), buff.data.size(), &allocator);
        }
      }
      wait_for_done.notify();
    });

    wait_for_done.wait();
    wait_for_done.wait();
  }
}

TEST(raw_connection, async_accept_no_callback) {
  wn::testing::allocator allocator;
  log_buff buffer(&allocator);
  buffer_logger logger(&buffer);
  wn::logging::static_log<> slog(&logger);
  wn::logging::log* log = slog.log();

  wn::multi_tasking::thread_job_pool pool(&allocator, 1);
  {
    wn::multi_tasking::job_signal listen_started(&pool, 0);
    wn::multi_tasking::semaphore wait_for_done;

    wn::networking::WNConcreteNetworkManager manager(&allocator, &pool, log);

    pool.add_unsynchronized_job(nullptr, [&]() {
      {
        auto listen_socket =
            manager.listen_remote_sync(wn::networking::ip_protocol::ipv4, 8080);
        listen_started.increment(1);

        wn::multi_tasking::job_signal signal(0);
        wn::containers::string my_str("hello_world", &allocator);

        wn::multi_tasking::job_signal accepted(0);
        wn::memory::unique_ptr<wn::networking::WNConnection> connection;
        wn::networking::network_error err;
        listen_socket->accept_async(&accepted, &connection, &err);

        accepted.wait_until(1);
        ASSERT_NE(nullptr, connection.get());
        ASSERT_EQ(wn::networking::network_error::ok, err);

        for (size_t i = 0; i < 10; ++i) {
          connection->get_send_pipe()->send_async(&signal, nullptr,
              {wn::networking::send_range(
                  reinterpret_cast<const uint8_t*>(my_str.data()),
                  my_str.length())});
        }
        signal.wait_until(10);
      }
      wait_for_done.notify();
    });

    pool.add_unsynchronized_job(nullptr, [&]() {
      {
        listen_started.wait_until(1);
        auto connect_socket = pool.call_blocking_function<
            wn::memory::unique_ptr<wn::networking::WNReliableConnection>>(
            [&]() {
              return manager.connect_remote_sync("127.0.0.1",
                  wn::networking::ip_protocol::ipv4, 8080, nullptr);
            });
        wn::containers::string received(&allocator);
        wn::containers::string expected(
            "hello_worldhello_worldhello_world"
            "hello_worldhello_worldhello_worldhello_worldhello_world"
            "hello_worldhello_world",
            &allocator);
        ASSERT_NE(nullptr, connect_socket);
        while (received != expected) {
          auto buff =
              pool.call_blocking_function<wn::networking::WNReceiveBuffer>(
                  [&]() {
                    return connect_socket->get_recv_pipe()->recv_sync();
                  });
          ASSERT_EQ(wn::networking::network_error::ok, buff.get_status());
          received += wn::containers::string(
              buff.data.data(), buff.data.size(), &allocator);
        }
      }
      wait_for_done.notify();
    });

    wait_for_done.wait();
    wait_for_done.wait();
  }
}

TEST(raw_connection, async_listen_connect) {
  wn::testing::allocator allocator;
  log_buff buffer(&allocator);
  buffer_logger logger(&buffer);
  wn::logging::static_log<> slog(&logger);
  wn::logging::log* log = slog.log();

  wn::multi_tasking::thread_job_pool pool(&allocator, 1);
  {
    wn::multi_tasking::semaphore wait_for_done;

    wn::networking::WNConcreteNetworkManager manager(&allocator, &pool, log);

    pool.add_unsynchronized_job(nullptr, [&]() {
      {
        wn::memory::unique_ptr<wn::networking::WNReliableAcceptConnection>
            listen_socket;
        wn::memory::unique_ptr<wn::networking::WNReliableConnection>
            connect_socket;

        wn::networking::network_error err;
        wn::networking::network_error err2;
        wn::multi_tasking::job_signal signal(0);

        manager.listen_remote_async(wn::networking::ip_protocol::ipv4, 8080,
            &signal, &listen_socket, &err);

        signal.wait_until(1);
        ASSERT_EQ(wn::networking::network_error::ok, err);
        ASSERT_NE(nullptr, listen_socket.get());

        // Once we have created the listen socket, we can now create the send
        // socket
        manager.connect_remote_async("127.0.0.1",
            wn::networking::ip_protocol::ipv4, 8080, &signal, &connect_socket,
            &err);
        wn::containers::string received(&allocator);
        wn::containers::string expected(
            "hello_worldhello_worldhello_world"
            "hello_worldhello_worldhello_worldhello_worldhello_world"
            "hello_worldhello_world",
            &allocator);

        wn::containers::string my_str("hello_world", &allocator);

        wn::memory::unique_ptr<wn::networking::WNConnection> connection;
        listen_socket->accept_async(&signal, &connection, &err2);

        // Wait for both the connect and the accept to complete
        signal.wait_until(3);

        ASSERT_NE(nullptr, connection.get());
        ASSERT_EQ(wn::networking::network_error::ok, err2);
        ASSERT_NE(nullptr, connect_socket.get());
        ASSERT_EQ(wn::networking::network_error::ok, err);

        for (size_t i = 0; i < 10; ++i) {
          connection->get_send_pipe()->send_async(&signal, nullptr,
              {wn::networking::send_range(
                  reinterpret_cast<const uint8_t*>(my_str.data()),
                  my_str.length())});
        }

        while (received != expected) {
          auto buff =
              pool.call_blocking_function<wn::networking::WNReceiveBuffer>(
                  [&]() {
                    return connect_socket->get_recv_pipe()->recv_sync();
                  });
          ASSERT_EQ(wn::networking::network_error::ok, buff.get_status());
          received += wn::containers::string(
              buff.data.data(), buff.data.size(), &allocator);
        }

        signal.wait_until(13);
      }
      wait_for_done.notify();
    });

    wait_for_done.wait();
  }
}

struct receiver : public wn::multi_tasking::synchronized<> {
  receiver(wn::memory::allocator* _alloc)
    : data(_alloc), allocator(_alloc), read_done(0) {}
  void on_recv(wn::networking::WNReceiveBuffer&& buff) {
    if (buff.get_status() == wn::networking::network_error::ok) {
      data +=
          wn::containers::string(buff.data.data(), buff.data.size(), allocator);
      return;
    }
    { wn::networking::WNReceiveBuffer to_destroy = wn::core::move(buff); }
    read_done.increment(1);
  }
  wn::containers::string data;
  wn::memory::allocator* allocator;
  wn::multi_tasking::job_signal read_done;
};

TEST(raw_connection, async_recv) {
  wn::testing::allocator allocator;
  log_buff buffer(&allocator);
  buffer_logger logger(&buffer);
  wn::logging::static_log<> slog(&logger);
  wn::logging::log* log = slog.log();

  wn::multi_tasking::thread_job_pool pool(&allocator, 1);
  {
    wn::multi_tasking::semaphore wait_for_done;

    wn::networking::WNConcreteNetworkManager manager(&allocator, &pool, log);

    pool.add_unsynchronized_job(nullptr, [&]() {
      {
        wn::memory::unique_ptr<wn::networking::WNReliableAcceptConnection>
            listen_socket;
        wn::memory::unique_ptr<wn::networking::WNReliableConnection>
            connect_socket;

        wn::networking::network_error err;
        wn::networking::network_error err2;
        wn::multi_tasking::job_signal signal(0);

        manager.listen_remote_async(wn::networking::ip_protocol::ipv4, 8080,
            &signal, &listen_socket, &err);

        signal.wait_until(1);
        ASSERT_EQ(wn::networking::network_error::ok, err);
        ASSERT_NE(nullptr, listen_socket.get());

        // Once we have created the listen socket, we can now create the send
        // socket
        manager.connect_remote_async("127.0.0.1",
            wn::networking::ip_protocol::ipv4, 8080, &signal, &connect_socket,
            &err);
        wn::containers::string expected(
            "hello_worldhello_worldhello_world"
            "hello_worldhello_worldhello_worldhello_worldhello_world"
            "hello_worldhello_world",
            &allocator);

        wn::containers::string my_str("hello_world", &allocator);

        receiver r = {&allocator};
        {
          wn::memory::unique_ptr<wn::networking::WNConnection> connection;
          listen_socket->accept_async(&signal, &connection, &err2);

          // Wait for both the connect and the accept to complete
          signal.wait_until(3);

          ASSERT_NE(nullptr, connection.get());
          ASSERT_EQ(wn::networking::network_error::ok, err2);
          ASSERT_NE(nullptr, connect_socket.get());
          ASSERT_EQ(wn::networking::network_error::ok, err);

          for (size_t i = 0; i < 10; ++i) {
            connection->get_send_pipe()->send_async(&signal, nullptr,
                {wn::networking::send_range(
                    reinterpret_cast<const uint8_t*>(my_str.data()),
                    my_str.length())});
          }

          connect_socket->get_recv_pipe()->recv_async(
              wn::multi_tasking::make_callback(
                  &allocator, &r, &receiver::on_recv));
          signal.wait_until(13);
        }
        r.read_done.wait_until(1);
        EXPECT_EQ(expected, r.data);
        wait_for_done.notify();
      }
    });
    wait_for_done.wait();
  }
}