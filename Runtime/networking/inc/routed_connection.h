// Copyright (c) 2021, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_RUNTIME_NETWORKING_ROUTED_CONNECTION_H__
#define __WN_RUNTIME_NETWORKING_ROUTED_CONNECTION_H__

#include "WNContainers/inc/WNContiguousRange.h"
#include "WNMemory/inc/unique_ptr.h"
#include "WNMultiTasking/inc/job_pool.h"
#include "WNNetworking/inc/WNConnection.h"
#include "WNNetworking/inc/WNNetworkingErrors.h"
#include "core/inc/endian.h"

namespace wn {
namespace runtime {
namespace networking {
using route = uint32_t;

// chunk_info is a uint16_t
// The two most significant bits are used to
// declare what type of message this is.
// [0] -> one-shot message
// [1] -> first part of a multi-part message
// [2] -> internal part of a multi-part message
// [3] -> last part of a multi-part message
// The next bit defines whether or not a
// message should be transmitted reliably.
// Specifically, if this is set, and we are running
// over a non-reliable connection, this message
// will be retried.
// Not implemented yet.
PACK(struct message_header {
  route target_route;
  uint16_t packet_number;
  uint16_t chunk_info;
});

PACK(struct multipart_header {
  message_header header;
  uint64_t offset;
  uint32_t multipart_id;
});

using wn::networking::send_range;
using wn::networking::send_ranges;

static_assert(
    sizeof(message_header) <= wn::networking::MAX_NETWORK_BUFFER_SIZE -
                                  wn::networking::MAX_NETWORK_MESSAGE_SIZE,
    "The header has to fit in the maximum message");
struct shared_buffer : public memory::intrusive_ptr_base {
  shared_buffer(wn::networking::WNReceiveBuffer _buffer)
    : m_buffer(core::move(_buffer)) {}
  wn::networking::WNReceiveBuffer m_buffer;
};

enum class message_type {
  one_shot = 0,
  multipart_start = 1,
  multi_part_continue = 2,
  multi_part_end = 3
};

struct routed_message {
  routed_message(routed_message&& _other)
    : m_message_data(core::move(_other.m_message_data)),
      m_message_offset(_other.m_message_offset),
      m_multipart_id(_other.m_multipart_id),
      m_route(_other.m_route),
      m_data(_other.m_data),
      m_buffer(core::move(_other.m_buffer)),
      m_status(_other.m_status) {}

  inline uint16_t id() const {
    return m_message_data & 0x3FFF;
  }

  inline uint64_t offset() const {
    return m_message_offset;
  }

  inline message_type type() const {
    return static_cast<message_type>((m_message_data >> 14) & 0x3);
  }

  inline uint32_t multipart_id() const {
    return m_multipart_id;
  }

  inline route get_route() const {
    return m_route;
  }

  inline const containers::contiguous_range<char>& data() {
    return m_data;
  }

private:
  // The ID of the message. This will be recycled over
  // time.
  // If the message is specifically marked as reliable,
  // this ID will be held in a different pool.
  uint16_t m_message_data;

  // This message sections offset into the entire
  // message. If this is a single-shot message, this will be 0.
  uint64_t m_message_offset;

  // This is the id of the current multi-part message.
  uint32_t m_multipart_id;

  // This is the route for this message.
  route m_route;

  containers::contiguous_range<char> m_data;
  memory::intrusive_ptr<shared_buffer> m_buffer;

  wn::networking::network_error m_status;

  friend class routed_connection;
  routed_message(multipart_header* _header,
      memory::intrusive_ptr<shared_buffer> _buffer,
      containers::contiguous_range<char> _data,
      wn::networking::network_error err)
    : m_data(_data), m_buffer(_buffer), m_status(err) {
    m_message_data = core::from_big_endian(_header->header.chunk_info);
    m_route = core::from_big_endian(_header->header.target_route);
    m_message_offset = core::from_big_endian(_header->offset);
    m_multipart_id = core::from_big_endian(_header->multipart_id);
  }

  routed_message(message_header* _header,
      memory::intrusive_ptr<shared_buffer> _buffer,
      containers::contiguous_range<char> _data,
      wn::networking::network_error err)
    : m_data(_data), m_buffer(_buffer), m_status(err) {
    m_message_data = core::from_big_endian(_header->chunk_info);
    m_route = core::from_big_endian(_header->target_route);
    m_message_offset = 0;
    m_multipart_id = 0;
  }
};

class routed_connection : public multi_tasking::synchronized<> {
public:
  routed_connection(wn::memory::allocator* _allocator,
      wn::memory::unique_ptr<wn::networking::WNConnection> _connection,
      multi_tasking::callback<routed_message> _default_route)
    : m_allocator(_allocator),
      m_connection(core::move(_connection)),
      m_router(_allocator, core::move(_default_route)),
      m_sender(_allocator, m_connection.get()),
      m_receiver(_allocator, m_connection.get(), m_router.get()),
      m_multipart_id(0) {
    multi_tasking::job_pool::this_job_pool()->add_job(
        nullptr, &sender::send, m_sender.get());
    multi_tasking::job_pool::this_job_pool()->add_job(
        nullptr, &receiver::recv, m_receiver.get());
  }

private:
  struct message_router : public multi_tasking::synchronized<> {
    message_router(wn::memory::allocator* _allocator,
        multi_tasking::callback<routed_message> default_callback)
      : m_routes(_allocator) {
      m_routes[0] = default_callback;
    }

    void route_message(multi_tasking::async_function, routed_message message) {
      auto it = m_routes.find(message.get_route());
      if (it != m_routes.end()) {
        it->second->enqueue_job(
            multi_tasking::job_pool::this_job_pool(), core::move(message));
      } else {
        m_routes[0]->enqueue_job(
            multi_tasking::job_pool::this_job_pool(), core::move(message));
      }
    }

    void set_route(multi_tasking::async_function, route _route,
        multi_tasking::callback<routed_message> _callback) {
      m_routes[_route] = _callback;
    }

    containers::hash_map<route, multi_tasking::callback<routed_message>>
        m_routes;
  };

  struct send_message {
    bool stop = false;
    multi_tasking::job_signal* sent_signal = nullptr;
    containers::dynamic_array<send_range> send_data;
    // This multipart header is just the outer header.
    // If this is not a multi-part message only the message will be read/sent.
    multipart_header header;
  };

public:
  void set_route(multi_tasking::async_passthrough _p, route _route,
      multi_tasking::callback<routed_message> _callback) {
    _p->add_job(&message_router::set_route, m_router.get(), _route, _callback);
  }

  void send_multiple(
      multi_tasking::async_passthrough _p, route _route, send_ranges _ranges) {
    m_sender->add_message(_p->signal(), _ranges, _route);
  }

  void send_single(
      multi_tasking::async_passthrough p, route _route, send_range _range) {
    send_ranges ranges(&_range, 1);
    send_multiple(p, _route, ranges);
  }

  template <typename T>
  void send_object(multi_tasking::async_function, route _route, T _t) {
    if (_t.size() == 0) {
      return;
    }
    WN_DEBUG_ASSERT(&(*_t.begin()) + _t.size() == &(*_t.end()),
        "The sent data must be contiguious");
    send_range range(&(*_t.begin()), _t.size());
    send_ranges ranges(&range, 1);
    multi_tasking::job_signal signal(0);
    m_sender->add_message(&signal, ranges, _route);
    signal.wait_until(1);
  }

  struct message_token_ {
  private:
    message_token_() {}
    uint64_t message_offset = 0;
    route target_route = 0;
    uint32_t multipart_id = 0;
    friend class wn::runtime::networking::routed_connection;
  };
  using message_token = memory::unique_ptr<message_token_>;

  message_token start_multipart_message(
      multi_tasking::job_signal* _signal, route _route, send_ranges _ranges) {
    message_token t = memory::make_unique_delegated<message_token_>(m_allocator,
        [](void* _memory) { return new (_memory) message_token_(); });
    t->target_route = _route;
    t->multipart_id = m_multipart_id++;
    for (auto& range : _ranges) {
      t->message_offset += range.size();
    }

    m_sender->add_message(_signal, _ranges, _route,
        message_type::multipart_start, 0, t->multipart_id);
    return t;
  }

  template <typename T>
  message_token start_multipart_message_object(
      multi_tasking::job_signal* _signal, route _route, T _tval) {
    message_token t = memory::make_unique_delegated<message_token_>(m_allocator,
        [](void* _memory) { return new (_memory) message_token_(); });
    t->target_route = _route;
    t->multipart_id = m_multipart_id++;
    t->message_offset = _tval.size();
    multi_tasking::job_pool::this_job_pool()->add_job(_signal,
        &routed_connection::send_internal<T>, this, core::move(_tval), _route,
        message_type::multipart_start, 0, t->multipart_id);
    return t;
  }

  message_token continue_multipart_message(multi_tasking::job_signal* _signal,
      message_token _old_token, send_ranges _ranges) {
    uint64_t old_offset = _old_token->message_offset;
    for (auto& range : _ranges) {
      _old_token->message_offset += range.size();
    }
    m_sender->add_message(_signal, _ranges, _old_token->target_route,
        message_type::multi_part_continue, old_offset,
        _old_token->multipart_id);
    return _old_token;
  }

  template <typename T>
  message_token continue_multipart_message_object(
      multi_tasking::job_signal* _signal, message_token _old_token, T _obj) {
    uint64_t old_offset = _old_token->message_offset;
    _old_token->message_offset += _obj.size();
    multi_tasking::job_pool::this_job_pool()->add_job(_signal,
        &routed_connection::send_internal<T>, this, core::move(_obj),
        _old_token->target_route, message_type::multi_part_continue, old_offset,
        _old_token->multipart_id);
    return _old_token;
  }

  void finish_multipart_message(multi_tasking::job_signal* _signal,
      message_token _old_token, send_ranges _ranges) {
    m_sender->add_message(_signal, _ranges, _old_token->target_route,
        message_type::multi_part_end, _old_token->message_offset,
        _old_token->multipart_id);
  }

  template <typename T>
  void finish_multipart_message_object(
      multi_tasking::job_signal* _signal, message_token _old_token, T _obj) {
    multi_tasking::job_pool::this_job_pool()->add_job(_signal,
        &routed_connection::send_internal<T>, this, core::move(_obj),
        _old_token->target_route, message_type::multi_part_end,
        _old_token->message_offset, _old_token->multipart_id);
  }

private:
  template <typename T>
  void send_internal(multi_tasking::async_function, T _t, route _route,
      message_type _type, uint64_t _offset, uint32_t _multipart_id) {
    if (_t.size() == 0) {
      return;
    }
    WN_DEBUG_ASSERT(&(*_t.begin()) + _t.size() == &(*_t.end()),
        "The sent data must be contiguious");

    send_range range(&(*_t.begin()), _t.size());
    send_ranges ranges(&range, 1);
    multi_tasking::job_signal signal(0);
    m_sender->add_message(
        &signal, ranges, _route, _type, _offset, _multipart_id);
    signal.wait_until(1);
  }

  struct sender : public multi_tasking::synchronized<> {
    sender(memory::allocator* _allocator,
        wn::networking::WNConnection* _underlying_conneciton)
      : m_allocator(_allocator),
        m_message_pool(_allocator),
        m_pending_messages(_allocator),
        m_underlying_connection(_underlying_conneciton),
        m_packet_number(0) {}

    void on_destroy() override {
      {
        multi_tasking::lock_guard<multi_tasking::spin_lock> guard(
            m_message_lock);
        send_message msg;
        msg.stop = true;
        m_pending_messages.push_back(msg);
      }
      m_message_resource.notify();
    }
    void add_message(multi_tasking::job_signal* _signal, send_ranges _ranges,
        route _route, message_type _type = message_type::one_shot,
        uint64_t _multipart_offset = 0, uint32_t _multipart_id = 0) {
      uint32_t length = 0;
      for (auto& range : _ranges) {
        length += static_cast<uint16_t>(range.size());
      }
      WN_DEBUG_ASSERT(length < wn::networking::MAX_NETWORK_MESSAGE_SIZE,
          "The message length is too long");
      // This is a temporary list, we just take a message out of the pool
      // and put it in this list for now. We then move this to another
      // list. We only actually use the allocator if there were no messages
      // in the pool.
      containers::list<send_message> temp_list(m_allocator);
      {
        multi_tasking::lock_guard<multi_tasking::spin_lock> guard(
            m_message_lock);
        if (!m_message_pool.empty()) {
          m_message_pool.transfer_to(
              m_message_pool.begin(), temp_list.begin(), temp_list);
        }
      }
      if (temp_list.empty()) {
        temp_list.emplace(temp_list.begin());
        temp_list.begin()->send_data =
            containers::dynamic_array<send_range>(m_allocator);
      }
      auto it = temp_list.begin();
      it->header.multipart_id = core::to_big_endian(_multipart_id);
      it->header.offset = core::to_big_endian(_multipart_offset);
      it->header.header.target_route = core::to_big_endian(_route);
      it->header.header.chunk_info = core::to_big_endian(
          static_cast<uint16_t>(static_cast<uint16_t>(length) |
                                (static_cast<uint16_t>(_type) << 14)));
      it->header.header.packet_number =
          core::to_big_endian(m_packet_number.fetch_add(1));

      it->send_data.clear();
      it->send_data.reserve(_ranges.size());
      for (auto& range : _ranges) {
        it->send_data.push_back(range);
      }

      it->stop = false;
      it->sent_signal = _signal;
      {
        multi_tasking::lock_guard<multi_tasking::spin_lock> guard(
            m_message_lock);
        temp_list.transfer_to(
            temp_list.begin(), m_pending_messages.begin(), m_pending_messages);
      }
      m_message_resource.notify();
    }

    void send(multi_tasking::async_blocking_function) {
      multi_tasking::job_pool::this_job_pool()->call_blocking_function(
          [this]() {
            containers::list<send_message> currently_sending_messages(
                m_allocator);
            containers::dynamic_array<wn::networking::send_range> ranges(
                m_allocator);
            containers::dynamic_array<multipart_header> headers(m_allocator);
            bool exit = false;
            while (!exit) {
              uint32_t num_messages = 1;

              m_message_resource.wait();
              // Get all of the messages that are ready to send at once.
              // Way better than sending a whole bunch one at a time.
              while (m_message_resource.try_wait()) {
                ++num_messages;
              }
              {
                multi_tasking::lock_guard<multi_tasking::spin_lock> guard(
                    m_message_lock);
                m_pending_messages.transfer_to(m_pending_messages.begin(),
                    m_pending_messages.begin() + num_messages,
                    currently_sending_messages.begin(),
                    currently_sending_messages);
                exit = (currently_sending_messages.end() - 1)->stop;
                if (exit) {
                  currently_sending_messages.transfer_to(
                      currently_sending_messages.end() - 1,
                      m_message_pool.begin(), m_message_pool);
                }
              }

              if (currently_sending_messages.empty()) {
                continue;
              }

              ranges.clear();
              for (auto it = currently_sending_messages.begin();
                   it != currently_sending_messages.end(); ++it) {
                if (static_cast<message_type>(
                        (core::from_big_endian(it->header.header.chunk_info) >>
                            14) &
                        0x3) != message_type::one_shot) {
                  ranges.push_back(
                      send_range(reinterpret_cast<const uint8_t*>(&it->header),
                          sizeof(multipart_header)));
                } else {
                  ranges.push_back(
                      send_range(reinterpret_cast<const uint8_t*>(&it->header),
                          sizeof(message_header)));
                }
                for (size_t j = 0; j < it->send_data.size(); ++j) {
                  ranges.push_back(it->send_data[j]);
                }
              }

              m_underlying_connection->get_send_pipe()->send_sync(
                  send_ranges(ranges.data(), ranges.size()));

              for (auto it = currently_sending_messages.begin();
                   it != currently_sending_messages.end(); ++it) {
                if (it->sent_signal) {
                  it->sent_signal->increment(1);
                }
              }

              {
                multi_tasking::lock_guard<multi_tasking::spin_lock> guard(
                    m_message_lock);
                currently_sending_messages.transfer_to(
                    currently_sending_messages.begin(),
                    currently_sending_messages.end(), m_message_pool.begin(),
                    m_message_pool);
              }
            }
          });
    }

    multi_tasking::semaphore m_message_resource;
    multi_tasking::spin_lock m_message_lock;
    memory::allocator* m_allocator;
    containers::list<send_message> m_message_pool;
    containers::list<send_message> m_pending_messages;
    wn::networking::WNConnection* m_underlying_connection;
    std::atomic<uint16_t> m_packet_number;
  };

  struct receiver : public multi_tasking::synchronized<> {
    receiver(memory::allocator* _allocator,
        wn::networking::WNConnection* _underlying_connection,
        message_router* _router)
      : m_allocator(_allocator),
        m_underlying_connection(_underlying_connection),
        m_router(_router) {}
    void recv(multi_tasking::async_blocking_function) {
      multi_tasking::job_pool::this_job_pool()->call_blocking_function(
          [this]() {
            wn::networking::WNReceiveBuffer temp_buffer =
                wn::networking::WNReceiveBuffer(
                    wn::networking::network_error::closed);
            uint16_t num_temp_bytes = 0;
            bool has_temp_buffer = false;
            bool exit = false;
            while (!exit) {
              wn::networking::WNReceiveBuffer buff =
                  m_underlying_connection->get_recv_pipe()->recv_sync();
              if (buff.get_status() != wn::networking::network_error::ok) {
                wn::networking::network_error err = buff.get_status();
                message_header header;
                header.chunk_info = 0;
                header.packet_number = 0;
                header.target_route = 0;
                memory::intrusive_ptr<shared_buffer> sb =
                    memory::make_intrusive<shared_buffer>(
                        m_allocator, core::move(buff));

                multi_tasking::job_pool::this_job_pool()->add_job(nullptr,
                    &message_router::route_message, m_router,
                    routed_message(&header, core::move(sb),
                        containers::contiguous_range<char>(), err));
                return;
              }

              uint16_t source_buffer_offset = 0;
              auto fill_to = [&temp_buffer, &buff, &num_temp_bytes,
                                 &source_buffer_offset](
                                 uint16_t fill_max) -> bool {
                if (num_temp_bytes > fill_max) {
                  return true;
                }
                const uint16_t additional_bytes = fill_max - num_temp_bytes;
                const uint16_t bytes_left_in_buffer = static_cast<uint16_t>(
                    buff.data.size() - source_buffer_offset);
                const uint16_t copy_count =
                    additional_bytes > bytes_left_in_buffer
                        ? bytes_left_in_buffer
                        : additional_bytes;
                containers::copy_to(&temp_buffer.data,
                    buff.data.data() + source_buffer_offset, copy_count);
                num_temp_bytes += copy_count;
                source_buffer_offset += copy_count;
                return copy_count == additional_bytes;
              };
              if (has_temp_buffer) {
                if (!fill_to(sizeof(message_header))) {
                  continue;
                }
                multipart_header h;
                // We have to memcpy the message_header, it is not guaranteed
                // that the bytes in the header are aligned, this can
                // cause crashes on some architectures if we try and use them.
                memory::memcpy(
                    &h.header, temp_buffer.data.data(), sizeof(message_header));
                const uint16_t info =
                    core::from_big_endian(h.header.chunk_info);
                const bool is_multi_part =
                    static_cast<message_type>((info >> 14) & 0x3) !=
                    message_type::one_shot;

                if (is_multi_part) {
                  if (!fill_to(sizeof(multipart_header))) {
                    continue;
                  }
                }
                const uint16_t header_size = is_multi_part
                                                 ? sizeof(multipart_header)
                                                 : sizeof(message_header);
                const uint16_t message_size = header_size + (info & 0x1FFF);
                if (!fill_to(message_size)) {
                  continue;
                }
                memory::intrusive_ptr<shared_buffer> sb =
                    memory::make_intrusive<shared_buffer>(
                        m_allocator, core::move(temp_buffer));
                if (is_multi_part) {
                  memory::memcpy(
                      &h, temp_buffer.data.data(), sizeof(multipart_header));

                  multi_tasking::job_pool::this_job_pool()->add_job(nullptr,
                      &message_router::route_message, m_router,
                      routed_message(&h, core::move(sb),
                          containers::contiguous_range<char>(
                              temp_buffer.data.data() + header_size,
                              message_size - header_size),
                          wn::networking::network_error::ok));
                } else {
                  multi_tasking::job_pool::this_job_pool()->add_job(nullptr,
                      &message_router::route_message, m_router,
                      routed_message(&h.header, core::move(sb),
                          containers::contiguous_range<char>(
                              temp_buffer.data.data() + header_size,
                              message_size - header_size),
                          wn::networking::network_error::ok));
                }
                has_temp_buffer = false;
              }

              wn::networking::WNReceiveBuffer* receive_buffer = &buff;
              memory::intrusive_ptr<shared_buffer> sb;

              auto has_enough_data = [receive_buffer, &source_buffer_offset](
                                         uint16_t bytes) -> bool {
                return receive_buffer->data.size() - source_buffer_offset >=
                       bytes;
              };
              while (source_buffer_offset != receive_buffer->data.size()) {
                if (!has_enough_data(sizeof(message_header))) {
                  break;
                }
                multipart_header h;
                // We have to memcpy the message_header, it is not guaranteed
                // that the bytes in the header are aligned, this can
                // cause crashes on some architectures if we try and use them.
                memory::memcpy(&h.header,
                    receive_buffer->data.data() + source_buffer_offset,
                    sizeof(message_header));

                const uint16_t info =
                    core::from_big_endian(h.header.chunk_info);
                const bool is_multi_part =
                    static_cast<message_type>((info >> 14) & 0x3) !=
                    message_type::one_shot;
                uint16_t header_size = is_multi_part ? sizeof(multipart_header)
                                                     : sizeof(message_header);
                uint16_t data_size = info & 0x1FFF;
                if (!has_enough_data(header_size + data_size)) {
                  break;
                }
                if (!sb) {
                  sb = memory::make_intrusive<shared_buffer>(
                      m_allocator, core::move(buff));
                  receive_buffer = &sb->m_buffer;
                }
                if (is_multi_part) {
                  memory::memcpy(&h,
                      receive_buffer->data.data() + source_buffer_offset,
                      sizeof(multipart_header));

                  multi_tasking::job_pool::this_job_pool()->add_job(nullptr,
                      &message_router::route_message, m_router,
                      routed_message(&h, sb,
                          containers::contiguous_range<char>(
                              receive_buffer->data.data() +
                                  source_buffer_offset + header_size,
                              data_size),
                          wn::networking::network_error::ok));
                } else {
                  multi_tasking::job_pool::this_job_pool()->add_job(nullptr,
                      &message_router::route_message, m_router,
                      routed_message(&h.header, sb,
                          containers::contiguous_range<char>(
                              receive_buffer->data.data() +
                                  source_buffer_offset + header_size,
                              data_size),
                          wn::networking::network_error::ok));
                }
                source_buffer_offset += header_size + data_size;
              }
              if (source_buffer_offset != receive_buffer->data.size()) {
                temp_buffer = m_underlying_connection->get_buffer_manager()
                                  ->acquire_buffer();
                has_temp_buffer = true;
                num_temp_bytes = static_cast<uint16_t>(
                    receive_buffer->data.size() - source_buffer_offset);
                memory::memcpy(temp_buffer.data.data(),
                    receive_buffer->data.data() + source_buffer_offset,
                    num_temp_bytes);
              }
            }
          });
    }
    memory::allocator* m_allocator;
    wn::networking::WNConnection* m_underlying_connection;
    message_router* m_router;
  };

  void on_destroy() {
    wn::multi_tasking::job_signal signal(0);
    wn::multi_tasking::job_pool::this_job_pool()->call_blocking_function_in_job(
        &signal, &wn::networking::WNConnection::close, m_connection.get());
    signal.wait_until(1);
  }
  wn::memory::allocator* m_allocator;
  wn::memory::unique_ptr<wn::networking::WNConnection> m_connection;

  multi_tasking::synchronized_destroy<message_router> m_router;
  multi_tasking::synchronized_destroy<sender> m_sender;
  multi_tasking::synchronized_destroy<receiver> m_receiver;
  std::atomic<uint32_t> m_multipart_id;
};  // namespace networking

class routed_accept_connection : public wn::multi_tasking::synchronized<> {
public:
  routed_accept_connection(memory::allocator* _allocator,
      memory::unique_ptr<wn::networking::WNAcceptConnection>
          _underlying_connection)
    : m_allocator(_allocator),
      m_underlying_connection(core::move(_underlying_connection)) {}

  // This will return aborted if this connection has been closed
  //  while the callback is still being run.
  void accept(multi_tasking::async_blocking_function,
      multi_tasking::callback<routed_message> _default_route,
      multi_tasking::callback<multi_tasking::sync_ptr<routed_connection>,
          wn::networking::network_error>
          _on_new_connection,
      size_t _num_connections = k_infinite) {
    multi_tasking::job_pool::this_job_pool()->call_blocking_function(
        [this, _default_route, _on_new_connection, _num_connections]() {
          for (size_t i = 0; i < _num_connections; ++i) {
            if (_num_connections == k_infinite) {
              i = 0;
            }
            wn::networking::network_error err =
                wn::networking::network_error::ok;
            memory::unique_ptr<wn::networking::WNConnection> base_connection =
                m_underlying_connection->accept_sync(&err);
            if (err != wn::networking::network_error::ok) {
              if (scheduled_for_destruction()) {
                // The decision here is, if we are being closed, due to
                // this object being destroyed, then we should
                // simply return aborted.
                err = wn::networking::network_error::aborted;
              }
              _on_new_connection->enqueue_job(
                  multi_tasking::job_pool::this_job_pool(), nullptr, err);
              break;
            } else {
              _on_new_connection->enqueue_job(
                  multi_tasking::job_pool::this_job_pool(),
                  multi_tasking::make_sync<routed_connection>(m_allocator,
                      m_allocator, core::move(base_connection), _default_route),
                  err);
            }
          }
        });
  }

private:
  void on_destroy() {
    wn::multi_tasking::job_signal signal(0);
    wn::multi_tasking::job_pool::this_job_pool()->call_blocking_function_in_job(
        &signal, &wn::networking::WNAcceptConnection::close,
        m_underlying_connection.get());
    signal.wait_until(1);
  }

  memory::allocator* m_allocator;
  memory::unique_ptr<wn::networking::WNAcceptConnection>
      m_underlying_connection;
};

struct routed_network {
  using connection_type = routed_connection;
  using accept_type = routed_accept_connection;
};

}  // namespace networking
}  // namespace runtime
}  // namespace wn
#endif  // __WN_RUNTIME_NETWORKING_ROUTED_CONNECTION_H__
