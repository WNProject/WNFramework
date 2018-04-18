// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNApplicationData/inc/WNApplicationData.h"
#include "WNApplicationTest/inc/WNTestHarness.h"
#include "WNMemory/inc/unique_ptr.h"
#include "WNNetworking/inc/WNNetworkManager.h"
#include "networking/inc/manager.h"
#include "networking/inc/routed_connection.h"

static const uint16_t k_starting_port = 11100;

using wn::multi_tasking::callback;
using wn::multi_tasking::job_signal;
using wn::multi_tasking::sync_ptr;
using wn::runtime::networking::routed_accept_connection;
using wn::runtime::networking::routed_connection;
using wn::runtime::networking::routed_manager;
using wn::runtime::networking::routed_message;

using ::testing::ElementsAre;

TEST(networking_rt, basic_routed) {
  auto& data = wn::runtime::testing::k_application_data;
  auto job_pool = wn::multi_tasking::job_pool::this_job_pool();
  wn::memory::allocator* allocator = data->system_allocator;
  wn::multi_tasking::synchronized_destroy<routed_manager> mgr(allocator,
      wn::memory::make_unique<wn::networking::WNConcreteNetworkManager>(
          allocator, allocator, data->default_log));

  wn::containers::string received(allocator);
  job_signal listening(0);
  job_signal got_message(0);
  sync_ptr<routed_accept_connection> accept;
  wn::networking::network_error error;
  job_pool->add_job(&listening, &routed_manager::listen, mgr.get(),
      wn::runtime::networking::ip_protocol::ipv4, k_starting_port, &accept,
      &error);

  listening.wait_until(1);
  ASSERT_EQ(error, wn::networking::network_error::ok);

  sync_ptr<routed_connection> connect;
  job_pool->add_job(&listening,
      &routed_manager::connect<callback<routed_message>>, mgr.get(),
      wn::containers::string(allocator, "127.0.0.1"),
      wn::runtime::networking::ip_protocol::ipv4, k_starting_port, &connect,
      &error,
      wn::multi_tasking::make_unsynchronized_callback<routed_message>(allocator,
          wn::functional::function<void(routed_message message)>(
              allocator, [&](routed_message message) {
                received.append(message.data().data(), message.data().size());
                got_message.increment(1);
              })));

  listening.wait_until(2);

  ASSERT_EQ(error, wn::networking::network_error::ok);

  sync_ptr<routed_connection> accepted;
  job_pool->add_job(nullptr, &routed_accept_connection::accept, accept->get(),
      wn::multi_tasking::make_unsynchronized_callback<routed_message>(
          allocator, wn::functional::function<void(routed_message)>(
                         allocator, [](routed_message) {})),
      wn::multi_tasking::make_unsynchronized_callback<
          sync_ptr<routed_connection>, wn::networking::network_error>(
          allocator, wn::functional::function<void(sync_ptr<routed_connection>,
                         wn::networking::network_error)>(allocator,
                         [&](sync_ptr<routed_connection> c,
                             wn::networking::network_error err) {
                           if (err == wn::networking::network_error::ok) {
                             accepted = wn::core::move(c);
                             listening.increment(1);
                           }
                         })),
      wn::runtime::networking::k_infinite);

  listening.wait_until(3);

  ASSERT_EQ(error, wn::networking::network_error::ok);

  uint8_t send_data[] = {
      'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd', '\0'};

  job_pool->add_job(&listening, &routed_connection::send_single,
      accepted->get(), wn::runtime::networking::route(0u),
      wn::runtime::networking::send_range(send_data));

  listening.wait_until(4);
  ASSERT_EQ(error, wn::networking::network_error::ok);

  got_message.wait_until(1);
  ASSERT_EQ(wn::containers::string(allocator, "Hello World\0", 12), received);
}

TEST(networking_rt, non_default_route) {
  auto& data = wn::runtime::testing::k_application_data;
  auto job_pool = wn::multi_tasking::job_pool::this_job_pool();
  wn::memory::allocator* allocator = data->system_allocator;
  wn::multi_tasking::synchronized_destroy<routed_manager> mgr(allocator,
      wn::memory::make_unique<wn::networking::WNConcreteNetworkManager>(
          allocator, allocator, data->default_log));

  wn::containers::string received(allocator);
  job_signal listening(0);
  job_signal got_message(0);
  sync_ptr<routed_accept_connection> accept;
  wn::networking::network_error error;
  job_pool->add_job(&listening, &routed_manager::listen, mgr.get(),
      wn::runtime::networking::ip_protocol::ipv4, k_starting_port, &accept,
      &error);

  listening.wait_until(1);
  job_signal route_added(0);
  ASSERT_EQ(error, wn::networking::network_error::ok);

  sync_ptr<routed_connection> connect;
  job_pool->add_job(&listening,
      &routed_manager::connect<callback<routed_message>>, mgr.get(),
      wn::containers::string(allocator, "127.0.0.1"),
      wn::runtime::networking::ip_protocol::ipv4, k_starting_port, &connect,
      &error,
      wn::multi_tasking::make_unsynchronized_callback<routed_message>(
          allocator, wn::functional::function<void(routed_message message)>(
                         allocator, [&](routed_message) {})));

  listening.wait_until(2);

  ASSERT_EQ(error, wn::networking::network_error::ok);
  job_pool->add_job(&route_added, &routed_connection::set_route, connect->get(),
      wn::runtime::networking::route(1u),
      wn::multi_tasking::make_unsynchronized_callback<routed_message>(allocator,
          wn::functional::function<void(routed_message message)>(
              allocator, [&](routed_message message) {
                received.append(message.data().data(), message.data().size());
                got_message.increment(1);
              })));

  sync_ptr<routed_connection> accepted;
  job_pool->add_job(nullptr, &routed_accept_connection::accept, accept->get(),
      wn::multi_tasking::make_unsynchronized_callback<routed_message>(
          allocator, wn::functional::function<void(routed_message)>(
                         allocator, [](routed_message) {})),
      wn::multi_tasking::make_unsynchronized_callback<
          sync_ptr<routed_connection>, wn::networking::network_error>(
          allocator, wn::functional::function<void(sync_ptr<routed_connection>,
                         wn::networking::network_error)>(allocator,
                         [&](sync_ptr<routed_connection> c,
                             wn::networking::network_error err) {
                           if (err == wn::networking::network_error::ok) {
                             accepted = wn::core::move(c);
                             listening.increment(1);
                           }
                         })),
      wn::runtime::networking::k_infinite);

  listening.wait_until(3);

  ASSERT_EQ(error, wn::networking::network_error::ok);

  uint8_t send_data[] = {
      'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd', '\0'};

  job_pool->add_job(&listening, &routed_connection::send_single,
      accepted->get(), wn::runtime::networking::route(1u),
      wn::runtime::networking::send_range(send_data));

  listening.wait_until(4);
  ASSERT_EQ(error, wn::networking::network_error::ok);

  got_message.wait_until(1);
  ASSERT_EQ(wn::containers::string(allocator, "Hello World\0", 12), received);
}

TEST(networking_rt, multi_message) {
  auto& data = wn::runtime::testing::k_application_data;
  auto job_pool = wn::multi_tasking::job_pool::this_job_pool();
  wn::memory::allocator* allocator = data->system_allocator;
  wn::multi_tasking::synchronized_destroy<routed_manager> mgr(allocator,
      wn::memory::make_unique<wn::networking::WNConcreteNetworkManager>(
          allocator, allocator, data->default_log));

  wn::containers::string received(allocator);
  job_signal listening(0);
  job_signal got_message(0);
  sync_ptr<routed_accept_connection> accept;
  wn::networking::network_error error;
  job_pool->add_job(&listening, &routed_manager::listen, mgr.get(),
      wn::runtime::networking::ip_protocol::ipv4, k_starting_port, &accept,
      &error);

  listening.wait_until(1);
  ASSERT_EQ(error, wn::networking::network_error::ok);

  struct message_got : public wn::multi_tasking::synchronized<> {
    message_got(wn::containers::string& s, job_signal* _signal)
      : s(s), signal(_signal) {}
    void got_message(
        wn::multi_tasking::async_function, routed_message message) {
      if (num_got > 2) {
        return;
      }
      EXPECT_EQ(message.offset(), s.length());
      s.append(message.data().data(), message.data().size());
      signal->increment(1);
      ++num_got;
      if (num_got == 1) {
        ASSERT_EQ(message.type(),
            wn::runtime::networking::message_type::multipart_start);
      } else if (num_got == 2) {
        ASSERT_EQ(message.type(),
            wn::runtime::networking::message_type::multi_part_continue);
      } else if (num_got == 3) {
        ASSERT_EQ(message.type(),
            wn::runtime::networking::message_type::multi_part_end);
      }
    }
    uint32_t num_got = 0;
    wn::containers::string& s;
    job_signal* signal;
  };

  wn::multi_tasking::synchronized_destroy<message_got> msg(
      received, &got_message);

  sync_ptr<routed_connection> connect;
  job_pool->add_job(&listening,
      &routed_manager::connect<callback<routed_message>>, mgr.get(),
      wn::containers::string(allocator, "127.0.0.1"),
      wn::runtime::networking::ip_protocol::ipv4, k_starting_port, &connect,
      &error,
      wn::multi_tasking::make_callback(
          allocator, msg.get(), &message_got::got_message));

  listening.wait_until(2);

  ASSERT_EQ(error, wn::networking::network_error::ok);

  sync_ptr<routed_connection> accepted;
  job_pool->add_job(nullptr, &routed_accept_connection::accept, accept->get(),
      wn::multi_tasking::make_unsynchronized_callback<routed_message>(
          allocator, wn::functional::function<void(routed_message)>(
                         allocator, [](routed_message) {})),
      wn::multi_tasking::make_unsynchronized_callback<
          sync_ptr<routed_connection>, wn::networking::network_error>(
          allocator, wn::functional::function<void(sync_ptr<routed_connection>,
                         wn::networking::network_error)>(allocator,
                         [&](sync_ptr<routed_connection> c,
                             wn::networking::network_error err) {
                           if (err == wn::networking::network_error::ok) {
                             accepted = wn::core::move(c);
                             listening.increment(1);
                           }
                         })),
      wn::runtime::networking::k_infinite);

  listening.wait_until(3);

  ASSERT_EQ(error, wn::networking::network_error::ok);

  uint8_t send_data[] = {
      'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd', '\0'};

  auto token = accepted->get()->start_multipart_message_object(&listening,
      wn::runtime::networking::route(0u),
      wn::runtime::networking::send_range(send_data));
  token = accepted->get()->continue_multipart_message_object(&listening,
      wn::core::move(token), wn::runtime::networking::send_range(send_data));
  accepted->get()->finish_multipart_message_object(&listening,
      wn::core::move(token), wn::runtime::networking::send_range(send_data));

  listening.wait_until(6);
  ASSERT_EQ(error, wn::networking::network_error::ok);

  got_message.wait_until(3);
  ASSERT_EQ(wn::containers::string(
                allocator, "Hello World\0Hello World\0Hello World\0", 36),
      received);
}
