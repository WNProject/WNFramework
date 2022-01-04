// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_HASHING_HASH_H__
#define __WN_HASHING_HASH_H__

#include "WNContainers/inc/WNContiguousRange.h"
#include "WNLogging/inc/WNLog.h"

#include <xxhash/xxhash.h>

namespace wn {
namespace hashing {

template <size_t N>
class hash {};

template <>
class hash<32> final {
public:
  hash(uint32_t _seed = 0) : m_state(XXH32_createState()) {
    reset(_seed);
  }

  ~hash() {
    XXH32_freeState(m_state);
  }

  void reset(uint32_t _seed = 0) {
    XXH32_reset(m_state, _seed);
  }

  template <typename T>
  void append(const containers::contiguous_range<T>& data) {
    static_assert(core::is_scalar<T>::value, "Can only hash scalars");

    XXH32_update(m_state, data.data(), sizeof(T) * data.size());
  }

  uint32_t get_data() const {
    return XXH32_digest(m_state);
  }

private:
  XXH32_state_s* m_state;
};

template <>
class hash<64> final {
public:
  hash(uint64_t _seed = 0) : m_state(XXH64_createState()) {
    reset(_seed);
  }

  ~hash() {
    XXH64_freeState(m_state);
  }

  void reset(uint64_t _seed = 0) {
    XXH64_reset(m_state, _seed);
  }

  template <typename T>
  void append(const containers::contiguous_range<T>& data) {
    static_assert(core::is_scalar<T>::value, "Can only hash scalars");

    XXH64_update(m_state, data.data(), sizeof(T) * data.size());
  }

  uint64_t get_data() const {
    return XXH64_digest(m_state);
  }

private:
  XXH64_state_s* m_state;
};

template <size_t N>
struct hash_data final {
  static_assert(
      (N % sizeof(uint32_t)) == 0, "Must be multiple of sizeof(uint32_t)");

  uint32_t parts[N / sizeof(uint32_t)];
};

template <>
class hash<96> final {
public:
  using value_type = hash_data<96>;

  hash(uint64_t _seed_high = 0, uint32_t _seed_low = 0)
    : m_state64(XXH64_createState()), m_state32(XXH32_createState()) {
    reset(_seed_high, _seed_low);
  }

  ~hash() {
    XXH64_freeState(m_state64);
    XXH32_freeState(m_state32);
  }

  void reset(uint64_t _seed_high = 0, uint32_t _seed_low = 0) {
    XXH64_reset(m_state64, _seed_high);
    XXH32_reset(m_state32, _seed_low);
  }

  template <typename T>
  void append(const containers::contiguous_range<T>& data) {
    static_assert(core::is_scalar<T>::value, "Can only hash scalars");

    XXH64_update(m_state64, data.data(), sizeof(T) * data.size());
    XXH32_update(m_state32, data.data(), sizeof(T) * data.size());
  }

  value_type get_data() const {
    const uint64_t hash_data0 = XXH64_digest(m_state64);
    const uint32_t part0 = (hash_data0 >> 32) & 0xFFFFFFFF;
    const uint32_t part1 = hash_data0 & 0xFFFFFFFF;

    return value_type{{part1, part0, XXH32_digest(m_state32)}};
  }

private:
  XXH64_state_s* m_state64;
  XXH32_state_s* m_state32;
};

}  // namespace hashing

namespace logging {

static const char hex_values[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8',
    '9', 'A', 'B', 'C', 'D', 'E', 'F'};

template <size_t N, typename BuffType>
struct log_type_helper<hashing::hash<N>, BuffType> {
  inline static bool do_log(
      const hashing::hash<N>& _0, BuffType* _buffer, size_t& _buffer_left) {
    size_t last_buffer_left = _buffer_left;

    if (!log_type_helper<char[3], BuffType>::do_log(
            "0x", _buffer, last_buffer_left)) {
      return false;
    }

    _buffer += 2;

    auto hash_val = _0.get_data();
    static const size_t output_size = sizeof(hash_val);
    static const size_t output_bytes = output_size * 2;

    if (last_buffer_left <= output_bytes) {
      return false;
    }

    char ob[output_size];

    memory::memcpy(ob, &hash_val, output_size);

    for (size_t i = 0; i < output_size; ++i) {
      _buffer[0] = hex_values[(ob[output_size - i - 1] >> 4) & 0xF];
      _buffer[1] = hex_values[ob[output_size - i - 1] & 0xF];
      _buffer += 2;
      last_buffer_left -= 2;
    }

    _buffer_left = last_buffer_left;

    return true;
  }
};

}  // namespace logging
}  // namespace wn

#endif  // __WN_HASHING_HASH_H__
