// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNApplicationData/inc/WNApplicationData.h"
#include "WNApplicationTest/inc/WNTestHarness.h"
#include "WNMemory/inc/WNUniquePtr.h"
#include "WNNetworking/inc/WNNetworkManager.h"
#include "networking/inc/connection.h"
#include "networking/inc/manager.h"

static const uint16_t k_starting_port = 10100;

using wn::multi_tasking::job_signal;
using wn::runtime::networking::accept_connection;
using wn::runtime::networking::connection;
using wn::runtime::networking::manager;

using wn::multi_tasking::sync_ptr;

using ::testing::ElementsAre;

TEST(networking_rt, basic) {
  auto& data = wn::runtime::testing::k_application_data;
  auto job_pool = wn::multi_tasking::job_pool::this_job_pool();
  wn::memory::allocator* allocator = data->system_allocator;
  wn::multi_tasking::synchronized_destroy<manager> mgr(allocator,
      wn::memory::make_unique<wn::networking::WNConcreteNetworkManager>(
          allocator, allocator, data->default_log));

  job_signal listening(0);
  sync_ptr<accept_connection> accept;
  wn::networking::network_error error;
  job_pool->add_job(&listening, &manager::listen, mgr.get(),
      wn::runtime::networking::ip_protocol::ipv4, k_starting_port, &accept,
      &error);

  listening.wait_until(1);
  ASSERT_EQ(error, wn::networking::network_error::ok);

  sync_ptr<connection> connect;
  job_pool->add_job(&listening, &manager::connect, mgr.get(),
      wn::containers::string(allocator, "127.0.0.1"),
      wn::runtime::networking::ip_protocol::ipv4, k_starting_port, &connect,
      &error);

  listening.wait_until(2);

  ASSERT_EQ(error, wn::networking::network_error::ok);

  sync_ptr<connection> accepted;
  job_pool->add_job(
      &listening, &accept_connection::accept, accept->get(), &accepted, &error);

  listening.wait_until(3);

  ASSERT_EQ(error, wn::networking::network_error::ok);

  uint8_t send_data[] = {
      'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd', '\0'};

  job_pool->add_job(&listening, &connection::send, connect->get(),
      wn::containers::contiguous_range<uint8_t>(send_data));

  listening.wait_until(4);
  ASSERT_EQ(error, wn::networking::network_error::ok);

  wn::networking::WNReceiveBuffer recv(
      wn::networking::network_error::uninitialized);

  job_pool->add_job(&listening, &connection::recv, accepted->get(), &recv);
  listening.wait_until(5);

  ASSERT_EQ(recv.get_status(), wn::networking::network_error::ok);
  ASSERT_EQ(recv.data.size(), sizeof(send_data));
  for (size_t i = 0; i < recv.data.size(); ++i) {
    EXPECT_EQ(static_cast<uint8_t>(recv.data[i]), send_data[i]);
  }
}

TEST(networking_rt, accept_cleans_up) {
  auto& data = wn::runtime::testing::k_application_data;
  auto job_pool = wn::multi_tasking::job_pool::this_job_pool();
  wn::memory::allocator* allocator = data->system_allocator;
  wn::multi_tasking::synchronized_destroy<manager> mgr(allocator,
      wn::memory::make_unique<wn::networking::WNConcreteNetworkManager>(
          allocator, allocator, data->default_log));
  size_t i = 0;

  wn::functional::function<void(
      sync_ptr<connection>, wn::networking::network_error)>
      cb = [&i](sync_ptr<connection> connection,
               wn::networking::network_error error) {
        if (error == wn::networking::network_error::aborted) {
          i += 1;
        } else {
          i += 2;
        }
      };
  {
    job_signal listening(0);
    sync_ptr<accept_connection> accept;
    wn::networking::network_error error;
    job_pool->add_job(&listening, &manager::listen, mgr.get(),
        wn::runtime::networking::ip_protocol::ipv4, k_starting_port, &accept,
        &error);

    listening.wait_until(1);
    ASSERT_EQ(error, wn::networking::network_error::ok);

    job_pool->add_job(nullptr, &accept_connection::accept_with_callback,
        accept->get(),
        wn::multi_tasking::make_unsynchronized_callback<sync_ptr<connection>,
            wn::networking::network_error>(allocator, cb),
        wn::runtime::networking::k_infinite);
  }
  EXPECT_EQ(i, 1u);
}

TEST(networking_rt, recv_cleans_up) {
  auto& data = wn::runtime::testing::k_application_data;
  auto job_pool = wn::multi_tasking::job_pool::this_job_pool();
  wn::memory::allocator* allocator = data->system_allocator;
  wn::multi_tasking::synchronized_destroy<manager> mgr(allocator,
      wn::memory::make_unique<wn::networking::WNConcreteNetworkManager>(
          allocator, allocator, data->default_log));
  size_t i = 0;
  {
    job_signal listening(0);
    sync_ptr<accept_connection> accept;
    wn::networking::network_error error;
    job_pool->add_job(&listening, &manager::listen, mgr.get(),
        wn::runtime::networking::ip_protocol::ipv4, k_starting_port, &accept,
        &error);

    listening.wait_until(1);
    ASSERT_EQ(error, wn::networking::network_error::ok);
    {
      sync_ptr<connection> connect;
      job_pool->add_job(&listening, &manager::connect, mgr.get(),
          wn::containers::string(allocator, "127.0.0.1"),
          wn::runtime::networking::ip_protocol::ipv4, k_starting_port, &connect,
          &error);

      listening.wait_until(2);

      job_pool->add_job(&listening, &connection::recv_with_callback,
          connect->get(),
          wn::multi_tasking::make_unsynchronized_callback(allocator,
              wn::functional::function<void(wn::networking::WNReceiveBuffer)>(
                  [&i](wn::networking::WNReceiveBuffer b) {
                    if (b.get_status() ==
                        wn::networking::network_error::aborted) {
                      i += 1;
                    } else {
                      i += 2;
                    }
                  })),
          wn::runtime::networking::k_infinite);
    }
  }
  EXPECT_EQ(i, 1u);
}
