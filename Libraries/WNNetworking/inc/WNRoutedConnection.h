// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_NETWORKING_ROUTED_CONNECTION_H__
#define __WN_NETWORKING_ROUTED_CONNECTION_H__

#include "WNCore/inc/WNEndian.h"
#include "WNMemory/inc/WNAllocator.h"
#include "WNMemory/inc/WNIntrusivePtr.h"
#include "WNMemory/inc/WNUniquePtr.h"
#include "WNMultiTasking/inc/WNJobPool.h"
#include "WNNetworking/inc/WNNetworkingErrors.h"
#include "WNNetworking/inc/WNReceiveBuffer.h"
#include "WNNetworking/inc/WNReliableConnection.h"

namespace wn {

namespace multi_tasking {
class job_pool;
}  // namespace multi_tasking

namespace networking {
using route = uint32_t;

const uint64_t INVALID_LENGTH = 0xFFFFFFFFFFFFFFFF;
// Routed messages are
// a message-based prtocol.
// If it arrives, every one-shot message is guaranteed to be self-contained and
// intact. Any multi-part messages may be split into as many pieces as
// required, and the split does not necessarily have to match with what
// was sent in the first place.

// Furthermore for sending, any one-shot message must be less than
// wn::networking::MAX_NETWORK_BUFFER_SIZE, and any part of any multi-part
// message must also be less than wn::networking::MAX_NETWORK_BUFFER_SIZE

// Note: Routed connections are only available in asynchronous mode due to
//       how the routing is intended to work.

// Internally a RoutedMessage has the following format:
// | 32-bit route | 1-bit start | 1-bit end | 14-bit chunk_length
// | 16-bit packet_number | 64-bit message_info | <data>
// route is a unique identfier to/from a particular connection
// chunk_length defines how many bytes are in this chunk
// message_offset is the offset in bytes from the start of the message
// message_size is the total size of the message (0xFFFFFFFFFFFFFFFF) if
// unknown
// message_offset + chunk_size == message_size for the last message in any
//   stream

PACK(struct MessageHeader {
  uint16_t chunk_size;
  uint16_t packet_number;
  uint32_t target_route;
  uint64_t message_offset;
  uint64_t message_size;
});

static_assert(
    sizeof(MessageHeader) <= MAX_NETWORK_BUFFER_SIZE - MAX_NETWORK_MESSAGE_SIZE,
    "The header has to fit in the correct bytes");
struct SharedBuffer : public memory::intrusive_ptr_base {
  SharedBuffer(WNReceiveBuffer _buffer) : m_buffer(core::move(_buffer)) {}
  WNReceiveBuffer m_buffer;
};

struct RoutedMessage {
  RoutedMessage(RoutedMessage&& _other)
    : m_message_id(core::move(_other.m_message_id)),
      m_message_size(_other.m_message_size),
      m_message_offset(_other.m_message_offset),
      m_route(_other.m_route),
      data(_other.data),
      m_buffer(core::move(_other.m_buffer)),
      m_status(_other.m_status) {}

  // The ID of the message
  uint16_t m_message_id;
  // The total size of the message, 0 if unknown
  uint64_t m_message_size;
  // This message sections offset into the entire
  // message
  uint64_t m_message_offset;
  // This section of message. It may be the whole
  // message or just a part
  // This is the route for this message.
  route m_route;

  network_error m_status;

  containers::contiguous_range<char> data;

private:
  friend class WNRoutedConnection;
  RoutedMessage(MessageHeader* _header,
      memory::intrusive_ptr<SharedBuffer> _buffer,
      containers::contiguous_range<char> _data, network_error err)
    : m_buffer(_buffer), data(_data), m_status(err) {
    if (_header) {
      m_message_id = core::from_big_endian(_header->packet_number);
      m_route = core::from_big_endian(_header->target_route);
      m_message_size = core::from_big_endian(_header->message_size);
      m_message_offset = core::from_big_endian(_header->message_offset);
    }
  }
  memory::intrusive_ptr<SharedBuffer> m_buffer;
};

class WNRoutedConnection {
public:
  // Any messaged directed at route 0
  // will get routed to _default_route.
  // Any messages for which no route exists will also get routed to
  // _default_route.
  // Any callbacks, and some supporting jobs will be generated on the
  // given job
  // pool.
  WNRoutedConnection(memory::allocator* _allocator,
      multi_tasking::callback_ptr<RoutedMessage&&> _default_route,
      memory::unique_ptr<WNConnection> _underlying_connection,
      multi_tasking::job_pool* _pool)
    : m_sender(_allocator, _underlying_connection.get()),
      m_connection(_underlying_connection.get()),
      m_router(_allocator->construct<MessageRouter>(_allocator,
          core::move(_default_route), _pool,
          core::move(_underlying_connection))),
      m_allocator(_allocator),
      m_pool(_pool),
      m_next_route(1) {
    m_connection->get_recv_pipe()->recv_async(
        multi_tasking::make_callback<WNReceiveBuffer&&>(
            m_allocator, m_router, &MessageRouter::route_message),
        m_pool);
  }

  ~WNRoutedConnection() {
    m_pool->add_job(nullptr, &MessageRouter::close_connection, m_router, 0u);
  }

  // Returns the next unused routing number.
  route get_unused_route() {
    return m_next_route.fetch_add(1);
  }

  // Stops routing messages directed at the given route, they will
  // subsequently be routed to _default_rotue until someone else
  // starts listening again.
  void remove_route(route _route) {
    m_pool->add_job(nullptr, &MessageRouter::remove_route, m_router, _route);
  }

  // When a message is receieved on the given route, a job will be created
  // with the given callback. This over-writes any previous notification
  // with the same _route.
  void recv_async(route _route, multi_tasking::job_signal* route_set,
      multi_tasking::callback_ptr<RoutedMessage&&> _callback) {
    m_pool->add_job(route_set, &MessageRouter::add_route, m_router, _route,
        core::move(_callback));
  }
  void send_async(route r, multi_tasking::job_signal* _signal,
      network_error* _error,
      const std::initializer_list<const send_range> _ranges) {
    return send_async(r, _signal, _error, _ranges.begin(), _ranges.size());
  }
  // Sends a single message asynchronously.
  template <typename It>
  void send_async(route r, multi_tasking::job_signal* _signal,
      network_error* _error, const It& _begin, size_t _size) {
    containers::dynamic_array<send_range> ranges(m_connection->m_allocator);
    ranges.reserve(_size + 1);

    auto dat = memory::make_unique<metadata_deleter::metadata>(m_allocator);
    dat->_signal_signal = _signal;
    multi_tasking::job_signal* to_signal = &dat->_wait_signal;
    auto metadata = &dat->_metadata;
    ranges.push_back(send_range(
        reinterpret_cast<uint8_t*>(&dat->_metadata), sizeof(dat->_metadata)));

    uint32_t total_message_length = 0;
    auto it = _begin;
    for (size_t i = 0; i < _size; ++i, ++it) {
      total_message_length += static_cast<uint32_t>(it->size());
      ranges.push_back(*it);
    }
    WN_DEBUG_ASSERT_DESC(total_message_length <= MAX_NETWORK_MESSAGE_SIZE,
        "The sent message must be less than MAX_NETWORK_MESSAGE_SIZE bytes");
    metadata->chunk_size = static_cast<uint16_t>(total_message_length);
    metadata->message_size = total_message_length;
    metadata->message_offset = 0;
    metadata->target_route = r;
    metadata->packet_number = 0;

    m_pool->add_job(
        nullptr, &metadata_deleter::delete_metadata, &deleter, core::move(dat));

    m_connection->m_job_pool->add_job(to_signal, &MessageSender::send_async,
        &m_sender, core::move(ranges), metadata, r, _error);
  }

  struct MessageToken_ {
  private:
    MessageToken_() {}
    uint64_t message_offset;
    uint64_t message_size;
    route target_route;
    friend class ::wn::networking::WNRoutedConnection;
  };

  using MessageToken = memory::unique_ptr<MessageToken_>;

  // Starts a new multi-part message, the message will be considered done
  // when a call to finish_multipart_message with the same MessageToken is
  // done.
  MessageToken start_multipart_message(route r,
      multi_tasking::job_signal* _signal, network_error* _error,
      uint64_t _total_size,
      const std::initializer_list<const send_range> _ranges) {
    return start_multipart_message(
        r, _signal, _error, _total_size, _ranges.begin(), _ranges.size());
  }

  template <typename It>
  MessageToken start_multipart_message(route r,
      multi_tasking::job_signal* _signal, network_error* _error,
      uint64_t _total_size, const It& _begin, size_t _size) {
    containers::dynamic_array<send_range> ranges(m_connection->m_allocator);
    ranges.reserve(_size + 1);

    auto dat = memory::make_unique<metadata_deleter::metadata>(m_allocator);
    dat->_signal_signal = _signal;
    multi_tasking::job_signal* to_signal = &dat->_wait_signal;
    auto metadata = &dat->_metadata;
    ranges.push_back(send_range(
        reinterpret_cast<uint8_t*>(&dat->_metadata), sizeof(dat->_metadata)));

    uint32_t total_message_length = 0;
    auto it = _begin;
    for (size_t i = 0; i < _size; ++i, ++it) {
      total_message_length += static_cast<uint32_t>(it->size());
      ranges.push_back(*it);
    }
    WN_DEBUG_ASSERT_DESC(total_message_length <= MAX_NETWORK_MESSAGE_SIZE,
        "The sent message must be less than MAX_NETWORK_MESSAGE_SIZE bytes");
    metadata->chunk_size = static_cast<uint16_t>(total_message_length);
    metadata->message_size = _total_size;
    metadata->message_offset = 0;
    metadata->target_route = r;
    metadata->packet_number = 0;

    void* mt = m_allocator->allocate(sizeof(MessageToken_));

    MessageToken t = MessageToken(m_allocator, new (mt) MessageToken_());
    t->message_offset = metadata->chunk_size;
    t->target_route = r;
    t->message_size = _total_size;
    m_pool->add_job(
        nullptr, &metadata_deleter::delete_metadata, &deleter, core::move(dat));

    m_connection->m_job_pool->add_job(to_signal, &MessageSender::send_async,
        &m_sender, core::move(ranges), metadata, r, _error);
    return core::move(t);
  }

  MessageToken continue_multipart_message(MessageToken t,
      multi_tasking::job_signal* _signal, network_error* _error,
      const std::initializer_list<const send_range> _ranges) {
    return continue_multipart_message(
        core::move(t), _signal, _error, _ranges.begin(), _ranges.size());
  }

  // Continues a multi-part message.
  template <typename It>
  MessageToken continue_multipart_message(MessageToken t,
      multi_tasking::job_signal* _signal, network_error* _error,
      const It& _begin, size_t _size) {
    containers::dynamic_array<send_range> ranges(m_connection->m_allocator);
    ranges.reserve(_size + 1);

    auto dat = memory::make_unique<metadata_deleter::metadata>(m_allocator);
    dat->_signal_signal = _signal;
    multi_tasking::job_signal* to_signal = &dat->_wait_signal;
    auto metadata = &dat->_metadata;
    ranges.push_back(send_range(
        reinterpret_cast<uint8_t*>(&dat->_metadata), sizeof(dat->_metadata)));

    uint32_t total_message_length = 0;
    auto it = _begin;
    for (size_t i = 0; i < _size; ++i, ++it) {
      total_message_length += static_cast<uint32_t>(it->size());
      ranges.push_back(*it);
    }
    WN_DEBUG_ASSERT_DESC(total_message_length <= MAX_NETWORK_MESSAGE_SIZE,
        "The sent message must be less than MAX_NETWORK_MESSAGE_SIZE bytes");
    metadata->chunk_size = static_cast<uint16_t>(total_message_length);
    metadata->message_size = t->message_size;
    metadata->message_offset = t->message_offset;
    metadata->target_route = t->target_route;

    t->message_offset += metadata->chunk_size;
    m_pool->add_job(
        nullptr, &metadata_deleter::delete_metadata, &deleter, core::move(dat));

    m_connection->m_job_pool->add_job(to_signal, &MessageSender::send_async,
        &m_sender, core::move(ranges), metadata, t->target_route, _error);
    return core::move(t);
  }

  // Marks the end of a multi-part message
  void finish_multipart_message(MessageToken t,
      multi_tasking::job_signal* _signal, network_error* _error,
      const std::initializer_list<const send_range> _ranges) {
    finish_multipart_message(
        core::move(t), _signal, _error, _ranges.begin(), _ranges.size());
  }

  template <typename It>
  void finish_multipart_message(MessageToken t,
      multi_tasking::job_signal* _signal, network_error* _error,
      const It& _begin, size_t _size) {
    containers::dynamic_array<send_range> ranges(m_connection->m_allocator);
    ranges.reserve(_size + 1);

    auto dat = memory::make_unique<metadata_deleter::metadata>(m_allocator);
    dat->_signal_signal = _signal;
    multi_tasking::job_signal* to_signal = &dat->_wait_signal;
    auto metadata = &dat->_metadata;
    ranges.push_back(send_range(
        reinterpret_cast<uint8_t*>(&dat->_metadata), sizeof(dat->_metadata)));

    uint32_t total_message_length = 0;
    auto it = _begin;
    for (size_t i = 0; i < _size; ++i, ++it) {
      total_message_length += static_cast<uint32_t>(it->size());
      ranges.push_back(*it);
    }
    WN_DEBUG_ASSERT_DESC(total_message_length <= MAX_NETWORK_MESSAGE_SIZE,
        "The sent message must be less than MAX_NETWORK_MESSAGE_SIZE bytes");
    metadata->chunk_size = static_cast<uint16_t>(total_message_length);
    metadata->message_offset = t->message_offset;
    t->message_offset += metadata->chunk_size;

    metadata->message_size = t->message_offset;
    metadata->target_route = t->target_route;
    metadata->packet_number = 0;

    m_pool->add_job(
        nullptr, &metadata_deleter::delete_metadata, &deleter, core::move(dat));

    m_connection->m_job_pool->add_job(to_signal, &MessageSender::send_async,
        &m_sender, core::move(ranges), metadata, t->target_route, _error);
  }

protected:
  // TODO(awoloszyn): Make this a bit smarter. Use the object pool
  // to handle the metadata blocks rather than creating/destroying them.

  struct metadata_deleter {
    struct metadata {
      metadata() : _wait_signal(0) {}
      MessageHeader _metadata;
      multi_tasking::job_signal _wait_signal;
      multi_tasking::job_signal* _signal_signal;
    };
    void delete_metadata(memory::unique_ptr<metadata>& data) {
      auto dat = core::move(data);
      dat->_wait_signal.wait_until(1);
      if (dat->_signal_signal) {
        dat->_signal_signal->increment(1);
      }
    }
  } deleter;

  struct MessageSender : public multi_tasking::synchronized<> {
    struct send_async_params {
      containers::dynamic_array<send_range> _params;
      MessageHeader* header_block;
      route _route;
      network_error* _err;
    };

    void send_async(send_async_params& s) {
      auto number = m_packet_numbers.find(s._route);
      if (number == m_packet_numbers.end()) {
        number = m_packet_numbers.insert({s._route, static_cast<uint16_t>(0u)})
                     .first;
        WN_DEBUG_ASSERT_DESC(s._params[0].size() == sizeof(MessageHeader),
            "The first block of the message must be a message header");
      }
      MessageHeader* header = s.header_block;
      header->packet_number = number->second++;

      header->target_route = core::to_big_endian(header->target_route);
      header->chunk_size = core::to_big_endian(header->chunk_size);
      header->packet_number = core::to_big_endian(header->packet_number);
      header->message_size = core::to_big_endian(header->message_size);
      header->message_offset = core::to_big_endian(header->message_offset);

      if (s._err) {
        *s._err =
            m_connection->m_job_pool->call_blocking_function<network_error>(
                &WNConnection::do_send, m_connection,
                send_ranges(s._params.data(), s._params.size()));
      } else {
        m_connection->m_job_pool->call_blocking_function(&WNConnection::do_send,
            m_connection, send_ranges(s._params.data(), s._params.size()));
      }
    }
    MessageSender(memory::allocator* _allocator, WNConnection* _connection)
      : m_packet_numbers(_allocator), m_connection(_connection) {}

    containers::hash_map<route, uint16_t> m_packet_numbers;
    WNConnection* m_connection;
  } m_sender;

  struct MessageRouter : public multi_tasking::synchronized<> {
    struct route_addition {
      route _route;
      multi_tasking::callback_ptr<RoutedMessage&&> _callback;
    };
    void add_route(route_addition& _addition) {
      m_routes[_addition._route] = core::move(_addition._callback);
    }

    void remove_route(route _route) {
      m_routes.erase(_route);
    }

    void close_connection(uint32_t) {
      if (m_underlying_connection_closed) {
        m_pool->destroy_synchronized(
            memory::unique_ptr<MessageRouter>(m_allocator, this));
      } else {
        m_underlying_connection->close();
        m_underlying_connection_closed = true;
      }
    }

    void route_message(WNReceiveBuffer&& _in_buffer) {
      if (_in_buffer.get_status() != network_error::ok) {
        m_default_route->enqueue_job(m_pool,
            RoutedMessage(nullptr, nullptr, {}, _in_buffer.get_status()));
        if (m_underlying_connection_closed) {
          m_pool->destroy_synchronized(
              memory::unique_ptr<MessageRouter>(m_allocator, this));
        }
        m_underlying_connection_closed = true;
        return;
      }
      WNReceiveBuffer* in_buffer = &_in_buffer;
      memory::intrusive_ptr<SharedBuffer> current_message;
      do {
        if (m_has_temp_buffer) {
          if (!m_header) {
            const uint16_t length = sizeof(MessageHeader);
            const uint16_t have = m_read_amount;
            const uint16_t left = length - have;

            const uint16_t copy =
                left > in_buffer->data.size()
                    ? static_cast<uint16_t>(in_buffer->data.size())
                    : left;
            containers::copy_to(
                &m_temp_buffer.data, in_buffer->data.data(), have, copy);
            m_header =
                reinterpret_cast<MessageHeader*>(m_temp_buffer.data.data());
            m_temp_buffer.data.remove_prefix(length);
            in_buffer->data.remove_prefix(copy);
          }
          if (!m_header) {
            return;
          }
          const uint16_t length = core::from_big_endian(m_header->chunk_size);
          // TODO(awoloszyn): If we start getting bad data
          // close the connection completely.
          if (length > MAX_NETWORK_MESSAGE_SIZE) {
            return;
          }
          const uint16_t have = m_read_amount;
          const uint16_t left = length - have;
          const uint16_t copy =
              left > in_buffer->data.size()
                  ? static_cast<uint16_t>(in_buffer->data.size())
                  : left;
          containers::copy_to(
              &m_temp_buffer.data, in_buffer->data.data(), have, copy);
          in_buffer->data.remove_prefix(copy);
          m_read_amount += copy;
          if (m_read_amount != length) {
            // Still not enough data, leave now.
            return;
          }
          m_temp_buffer.data.remove_suffix(m_temp_buffer.data.size() - length);
          route r = core::from_big_endian(m_header->target_route);
          auto* route_callback = &m_default_route;
          auto it = m_routes.find(r);
          if (it != m_routes.end()) {
            route_callback = &it->second;
          }
          containers::contiguous_range<char> dat = m_temp_buffer.data;
          (*route_callback)
              ->enqueue_job(
                  m_pool, RoutedMessage(m_header,
                              memory::make_intrusive<SharedBuffer>(m_allocator,
                                            core::move(m_temp_buffer)),
                              dat, network_error::ok));
          m_has_temp_buffer = false;
          continue;
        }
        if (in_buffer->data.size() < sizeof(MessageHeader)) {
          m_has_temp_buffer = true;
          m_temp_buffer = m_underlying_connection->m_manager->acquire_buffer();
          containers::copy_to(&m_temp_buffer.data, in_buffer->data.data(), 0,
              in_buffer->data.size());
          m_read_amount = static_cast<uint16_t>(in_buffer->data.size());
          m_has_temp_buffer = true;
          return;
        }
        m_header = reinterpret_cast<MessageHeader*>(in_buffer->data.data());
        uint16_t size = core::from_big_endian(m_header->chunk_size);
        // If there is not enough room in this message, then copy to temp
        if (in_buffer->data.size() < size + sizeof(MessageHeader)) {
          m_temp_buffer = m_underlying_connection->m_manager->acquire_buffer();
          containers::copy_to(&m_temp_buffer.data, in_buffer->data.data(), 0,
              in_buffer->data.size());
          m_header =
              reinterpret_cast<MessageHeader*>(m_temp_buffer.data.data());
          m_has_temp_buffer = true;
          m_read_amount = static_cast<uint16_t>(in_buffer->data.size());
        }

        auto* route_callback = &m_default_route;
        route r = core::from_big_endian(m_header->target_route);
        auto it = m_routes.find(r);
        if (it != m_routes.end()) {
          route_callback = &it->second;
        }
        in_buffer->data.remove_prefix(sizeof(MessageHeader));
        containers::contiguous_range<char> dat(in_buffer->data.data(), size);
        in_buffer->data.remove_prefix(size);
        if (!current_message) {
          current_message = memory::make_intrusive<SharedBuffer>(
              m_allocator, core::move(_in_buffer));
          in_buffer = &current_message->m_buffer;
        }

        (*route_callback)
            ->enqueue_job(m_pool, RoutedMessage(m_header, current_message, dat,
                                      network_error::ok));
      } while (in_buffer->data.size() > 0);
    }

    MessageRouter(memory::allocator* _allocator,
        multi_tasking::callback_ptr<RoutedMessage&&> _default_route,
        multi_tasking::job_pool* _pool,
        memory::unique_ptr<WNConnection> _underlying_connection)
      : m_allocator(_allocator),
        m_default_route(core::move(_default_route)),
        m_routes(_allocator),
        m_pool(_pool),
        m_underlying_connection(core::move(_underlying_connection)),
        m_temp_buffer(network_error::closed),
        m_has_temp_buffer(false) {}

  private:
    memory::allocator* m_allocator;
    multi_tasking::callback_ptr<RoutedMessage&&> m_default_route;
    containers::hash_map<route, multi_tasking::callback_ptr<RoutedMessage&&>>
        m_routes;
    multi_tasking::job_pool* m_pool;
    memory::unique_ptr<WNConnection> m_underlying_connection;
    WNReceiveBuffer m_temp_buffer;
    uint16_t m_read_amount = 0;
    MessageHeader* m_header = nullptr;
    bool m_underlying_connection_closed = false;
    bool m_has_temp_buffer;
  };

  memory::allocator* m_allocator;
  WNConnection* m_connection;  // Leave this above MessageRouter
  MessageRouter* m_router;
  multi_tasking::job_pool* m_pool;
  std::atomic<uint32_t> m_next_route;
};

// When a new connection is established, this will kick off a job with
// the
// given callback. The callback will be given ownership of a
// WNRoutedConnection that is connected to the endpoint.
class WNRoutedAcceptConnection {
public:
  WNRoutedAcceptConnection(memory::allocator* _allocator,
      memory::unique_ptr<WNAcceptConnection> _underlying_connection,
      multi_tasking::job_pool* _pool,
      multi_tasking::callback_ptr<memory::unique_ptr<WNRoutedConnection>>
          _new_connection_job,
      multi_tasking::callback_ptr<RoutedMessage&&> _default_route)
    : m_allocator(_allocator),
      m_pool(_pool),
      m_underlying_connection(core::move(_underlying_connection)),
      m_default_route(core::move(_default_route)),
      m_on_new_connection(core::move(_new_connection_job)) {
    if (!m_pool) {
      m_pool = m_underlying_connection->m_pool;
    }
    m_underlying_connection->accept_async(multi_tasking::make_callback(
        m_allocator, this, &WNRoutedAcceptConnection::on_accept));
  }

protected:
  void on_accept(memory::unique_ptr<WNConnection> _ptr) {
    memory::unique_ptr<WNRoutedConnection> conn =
        memory::make_unique<WNRoutedConnection>(m_allocator, m_allocator,
            m_default_route->clone(m_allocator), core::move(_ptr), m_pool);
    m_on_new_connection->enqueue_job(m_pool, core::move(conn));
  }

  multi_tasking::job_pool* m_pool;
  memory::allocator* m_allocator;
  WNAcceptConnection* m_connection;
  memory::unique_ptr<WNAcceptConnection> m_underlying_connection;
  multi_tasking::callback_ptr<RoutedMessage&&> m_default_route;
  multi_tasking::callback_ptr<memory::unique_ptr<WNRoutedConnection>>
      m_on_new_connection;
};

}  // namespace networking
}  // namespace wn

#endif  // __WN_NETWORKING_ROUTED_CONNECTION_H__