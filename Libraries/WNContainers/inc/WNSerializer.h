// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_CONTAINERS_SERIALIZER_H__
#define __WN_CONTAINERS_SERIALIZER_H__

#include "WNContainers/inc/WNDataBuffer.h"
#include "WNContainers/inc/WNSerializerBase.h"
#include "WNCore/inc/WNAssert.h"
#include "WNCore/inc/WNEndian.h"
#include "WNCore/inc/WNTypes.h"
#include "WNMemory/inc/WNBasic.h"
#include "WNMemory/inc/string_utility.h"

namespace wn {
namespace containers {
template <typename _Type>
class serializer final {};

template <>
class serializer<int32_t> : public serializer_base {
public:
  serializer(int32_t& _value) : m_value(_value) {}

  virtual size_t serialize(
      data_buffer& _dataBuffer, const uint32_t _flags) const override {
    (void)_flags;

    switch (_dataBuffer.type()) {
      case data_buffer_type::read_binary: {
        size_t returnedBytes = 0;
        size_t totalBytes = 0;
        do {
          const char* location =
              _dataBuffer.reserve(sizeof(int32_t) - totalBytes, returnedBytes);
          wn::memory::memcpy(reinterpret_cast<uint8_t*>(&m_value) + totalBytes,
              location, returnedBytes);
          totalBytes += returnedBytes;
        } while (totalBytes < sizeof(int32_t));

        m_value = core::from_big_endian(m_value);

        return (sizeof(int32_t));
      }
      case data_buffer_type::write_binary: {
        int32_t number = m_value;
        number = core::to_big_endian(number);
        size_t returnedBytes = 0;
        size_t totalBytes = 0;
        do {
          char* location =
              _dataBuffer.reserve(sizeof(int32_t) - totalBytes, returnedBytes);
          wn::memory::memcpy(location,
              reinterpret_cast<uint8_t*>(&number) + totalBytes, returnedBytes);
          totalBytes += returnedBytes;
        } while (totalBytes < sizeof(int32_t));
        return (sizeof(int32_t));
      }
      case data_buffer_type::read_text: {
        char tempBuffer[16];
        size_t returnedBytes = 0;
        size_t totalBytes = 0;
        size_t countFails = 0;

        do {
          countFails++;
          char* location = _dataBuffer.reserve(16 - totalBytes, returnedBytes);
          wn::memory::memcpy(
              reinterpret_cast<uint8_t*>(&tempBuffer) + totalBytes, location,
              returnedBytes);
          totalBytes += returnedBytes;
          if (countFails > 1) {
            break;
          }
        } while (totalBytes < 4);

        size_t outSize =
            memory::readint32(tempBuffer, m_value, totalBytes - 1);
        outSize += 1;  // consume the space that is supposed to be there

        return (outSize);
      }
      case data_buffer_type::write_text: {
        char tempBuffer[17];
        size_t outSize = memory::writeint32(tempBuffer, m_value, 16);
        tempBuffer[outSize++] = ' ';

        size_t returnedBytes = 0;
        size_t totalBytes = 0;
        do {
          char* location =
              _dataBuffer.reserve(outSize - totalBytes, returnedBytes);
          wn::memory::memcpy(location,
              reinterpret_cast<uint8_t*>(&tempBuffer) + totalBytes,
              returnedBytes);
          totalBytes += returnedBytes;
        } while (totalBytes < outSize);

        return (outSize);
      }
    }

    return (0);
  }

private:
  int32_t& m_value;
};

template <>
class serializer<int64_t> : public serializer_base {
public:
  serializer(int64_t& _value) : m_value(_value) {}

  virtual size_t serialize(
      data_buffer& _dataBuffer, const uint32_t _flags) const override {
    (void)_flags;

    switch (_dataBuffer.type()) {
      case data_buffer_type::read_binary: {
        size_t returnedBytes = 0;
        size_t totalBytes = 0;
        do {
          const char* location =
              _dataBuffer.reserve(sizeof(int64_t) - totalBytes, returnedBytes);
          wn::memory::memcpy(reinterpret_cast<uint8_t*>(&m_value) + totalBytes,
              location, returnedBytes);
          totalBytes += returnedBytes;
        } while (totalBytes < sizeof(int64_t));

        m_value = core::from_big_endian(m_value);

        return (sizeof(int64_t));
      }
      case data_buffer_type::write_binary: {
        int64_t number = m_value;
        number = core::to_big_endian(number);
        size_t returnedBytes = 0;
        size_t totalBytes = 0;
        do {
          char* location =
              _dataBuffer.reserve(sizeof(int64_t) - totalBytes, returnedBytes);
          wn::memory::memcpy(location,
              reinterpret_cast<uint8_t*>(&number) + totalBytes, returnedBytes);
          totalBytes += returnedBytes;
        } while (totalBytes < sizeof(int64_t));
        return (sizeof(int64_t));
      }
      case data_buffer_type::read_text: {
        char tempBuffer[23];
        size_t returnedBytes = 0;
        size_t totalBytes = 0;
        size_t countFails = 0;

        do {
          countFails++;
          char* location = _dataBuffer.reserve(23 - totalBytes, returnedBytes);
          wn::memory::memcpy(
              reinterpret_cast<uint8_t*>(tempBuffer) + totalBytes, location,
              returnedBytes);
          totalBytes += returnedBytes;
          if (countFails > 1) {
            break;
          }
        } while (totalBytes < 23);

        size_t outSize =
            memory::WNReadInt64(tempBuffer, m_value, totalBytes - 1);
        outSize += 1;  // consume the space that is supposed to be there
        return (outSize);
      }
      case data_buffer_type::write_text: {
        char tempBuffer[24];
        size_t outSize = memory::writeint64(tempBuffer, m_value, 23);
        tempBuffer[outSize++] = ' ';

        size_t returnedBytes = 0;
        size_t totalBytes = 0;
        do {
          char* location =
              _dataBuffer.reserve(outSize - totalBytes, returnedBytes);
          wn::memory::memcpy(location,
              reinterpret_cast<uint8_t*>(&tempBuffer) + totalBytes,
              returnedBytes);
          totalBytes += returnedBytes;
        } while (totalBytes < outSize);

        return (outSize);
      }
    }

    return (0);
  }

private:
  int64_t& m_value;
};

template <>
class serializer<uint8_t> : public serializer_base {
public:
  serializer(uint8_t& _value) : m_value(_value) {}

  virtual size_t serialize(
      data_buffer& _dataBuffer, const uint32_t _flags) const override {
    (void)_flags;

    switch (_dataBuffer.type()) {
      case data_buffer_type::read_binary: {
        size_t returnedBytes = 0;
        size_t totalBytes = 0;
        do {
          const char* location =
              _dataBuffer.reserve(sizeof(uint8_t) - totalBytes, returnedBytes);

          wn::memory::memcpy(reinterpret_cast<uint8_t*>(&m_value) + totalBytes,
              location, returnedBytes);

          totalBytes += returnedBytes;
        } while (totalBytes < sizeof(uint8_t));

        return (sizeof(uint8_t));
      }
      case data_buffer_type::write_binary: {
        size_t returnedBytes = 0;
        size_t totalBytes = 0;
        do {
          char* location =
              _dataBuffer.reserve(sizeof(uint8_t) - totalBytes, returnedBytes);
          wn::memory::memcpy(location,
              reinterpret_cast<uint8_t*>(&m_value) + totalBytes, returnedBytes);
          totalBytes += returnedBytes;
        } while (totalBytes < sizeof(uint8_t));

        return (sizeof(uint8_t));
      }
      case data_buffer_type::read_text: {
        char tempBuffer[4];
        size_t returnedBytes = 0;
        size_t totalBytes = 0;
        size_t countFails = 0;

        do {
          countFails++;
          char* location = _dataBuffer.reserve(4 - totalBytes, returnedBytes);

          wn::memory::memcpy(
              reinterpret_cast<uint8_t*>(&tempBuffer) + totalBytes, location,
              returnedBytes);

          totalBytes += returnedBytes;
          if (countFails > 1) {
            break;
          }
        } while (totalBytes < 4);

        uint32_t number;
        size_t outSize =
            memory::readuint32(tempBuffer, number, totalBytes - 1);

        m_value = number & 0xFF;
        outSize += 1;  // consume the space that is supposed to be there

        return (outSize);
      }
      case data_buffer_type::write_text: {
        char tempBuffer[4];
        size_t outSize = memory::writeuint32(tempBuffer, m_value, 3);
        tempBuffer[outSize++] = ' ';

        size_t returnedBytes = 0;
        size_t totalBytes = 0;
        do {
          char* location =
              _dataBuffer.reserve(outSize - totalBytes, returnedBytes);
          wn::memory::memcpy(location,
              reinterpret_cast<uint8_t*>(&tempBuffer) + totalBytes,
              returnedBytes);
          totalBytes += returnedBytes;
        } while (totalBytes < outSize);

        return (outSize);
      }
    }

    return (0);
  }

private:
  uint8_t& m_value;
};

template <>
class serializer<uint32_t> : public serializer_base {
public:
  serializer(uint32_t& _value) : m_value(_value) {}

  virtual size_t serialize(
      data_buffer& _dataBuffer, const uint32_t _flags) const override {
    (void)_flags;

    switch (_dataBuffer.type()) {
      case data_buffer_type::read_binary: {
        size_t returnedBytes = 0;
        size_t totalBytes = 0;
        do {
          const char* location =
              _dataBuffer.reserve(sizeof(uint32_t) - totalBytes, returnedBytes);
          wn::memory::memcpy(reinterpret_cast<uint8_t*>(&m_value) + totalBytes,
              location, returnedBytes);
          totalBytes += returnedBytes;
        } while (totalBytes < sizeof(uint32_t));

        m_value = core::from_big_endian(m_value);

        return (sizeof(uint32_t));
      }
      case data_buffer_type::write_binary: {
        uint32_t number = m_value;
        number = core::to_big_endian(number);
        size_t returnedBytes = 0;
        size_t totalBytes = 0;
        do {
          char* location =
              _dataBuffer.reserve(sizeof(uint32_t) - totalBytes, returnedBytes);
          wn::memory::memcpy(location,
              reinterpret_cast<uint8_t*>(&number) + totalBytes, returnedBytes);
          totalBytes += returnedBytes;
        } while (totalBytes < sizeof(uint32_t));
        return (sizeof(uint32_t));
      }
      case data_buffer_type::read_text: {
        char tempBuffer[16];
        size_t returnedBytes = 0;
        size_t totalBytes = 0;
        size_t countFails = 0;

        do {
          countFails++;
          char* location = _dataBuffer.reserve(16 - totalBytes, returnedBytes);
          wn::memory::memcpy(
              reinterpret_cast<uint8_t*>(tempBuffer) + totalBytes, location,
              returnedBytes);
          totalBytes += returnedBytes;
          if (countFails > 1) {
            break;
          }
        } while (totalBytes < 16);

        size_t outSize =
            memory::readuint32(tempBuffer, m_value, totalBytes - 1);
        outSize += 1;  // consume the space that is supposed to be there
        return (outSize);
      }
      case data_buffer_type::write_text: {
        char tempBuffer[17];
        size_t outSize = memory::writeuint32(tempBuffer, m_value, 16);
        tempBuffer[outSize++] = ' ';

        size_t returnedBytes = 0;
        size_t totalBytes = 0;
        do {
          char* location =
              _dataBuffer.reserve(outSize - totalBytes, returnedBytes);
          wn::memory::memcpy(location,
              reinterpret_cast<uint8_t*>(&tempBuffer) + totalBytes,
              returnedBytes);
          totalBytes += returnedBytes;
        } while (totalBytes < outSize);

        return (outSize);
      }
    }

    return (0);
  }

private:
  uint32_t& m_value;
};

template <>
class serializer<uint64_t> : public serializer_base {
public:
  serializer(uint64_t& _value) : m_value(_value) {}

  virtual size_t serialize(
      data_buffer& _dataBuffer, const uint32_t _flags) const override {
    (void)_flags;

    switch (_dataBuffer.type()) {
      case data_buffer_type::read_binary: {
        size_t returnedBytes = 0;
        size_t totalBytes = 0;
        do {
          const char* location =
              _dataBuffer.reserve(sizeof(uint64_t) - totalBytes, returnedBytes);
          wn::memory::memcpy(reinterpret_cast<uint8_t*>(&m_value) + totalBytes,
              location, returnedBytes);
          totalBytes += returnedBytes;
        } while (totalBytes < sizeof(uint64_t));

        m_value = core::from_big_endian(m_value);

        return (sizeof(uint64_t));
      }
      case data_buffer_type::write_binary: {
        uint64_t number = m_value;
        number = core::to_big_endian(number);
        size_t returnedBytes = 0;
        size_t totalBytes = 0;
        do {
          char* location =
              _dataBuffer.reserve(sizeof(uint64_t) - totalBytes, returnedBytes);
          wn::memory::memcpy(location,
              reinterpret_cast<uint8_t*>(&number) + totalBytes, returnedBytes);
          totalBytes += returnedBytes;
        } while (totalBytes < sizeof(uint64_t));
        return (sizeof(uint64_t));
      }
      case data_buffer_type::read_text: {
        char tempBuffer[23];
        size_t returnedBytes = 0;
        size_t totalBytes = 0;
        size_t countFails = 0;

        do {
          countFails++;
          char* location = _dataBuffer.reserve(23 - totalBytes, returnedBytes);
          wn::memory::memcpy(
              reinterpret_cast<uint8_t*>(tempBuffer) + totalBytes, location,
              returnedBytes);
          totalBytes += returnedBytes;
          if (countFails > 1) {
            break;
          }
        } while (totalBytes < 23);

        size_t outSize =
            memory::WNReadUInt64(tempBuffer, m_value, totalBytes - 1);
        outSize += 1;  // consume the space that is supposed to be there
        return (outSize);
      }
      case data_buffer_type::write_text: {
        char tempBuffer[24];
        size_t outSize = memory::writeuint64(tempBuffer, m_value, 23);
        tempBuffer[outSize++] = ' ';

        size_t returnedBytes = 0;
        size_t totalBytes = 0;
        do {
          char* location =
              _dataBuffer.reserve(outSize - totalBytes, returnedBytes);
          wn::memory::memcpy(location,
              reinterpret_cast<uint8_t*>(&tempBuffer) + totalBytes,
              returnedBytes);
          totalBytes += returnedBytes;
        } while (totalBytes < outSize);

        return (outSize);
      }
    }

    return (0);
  }

private:
  uint64_t& m_value;
};

template <>
class serializer<float> : public serializer_base {
public:
  serializer(float& _value) : m_value(_value) {}

  virtual size_t serialize(
      data_buffer& _dataBuffer, const uint32_t _flags) const override {
    (void)_flags;

    switch (_dataBuffer.type()) {
      case data_buffer_type::read_binary: {
        size_t returnedBytes = 0;
        size_t totalBytes = 0;
        do {
          const char* location =
              _dataBuffer.reserve(sizeof(float) - totalBytes, returnedBytes);
          wn::memory::memcpy(reinterpret_cast<uint8_t*>(&m_value) + totalBytes,
              location, returnedBytes);
          totalBytes += returnedBytes;
        } while (totalBytes < sizeof(float));

        m_value = core::from_big_endian(m_value);

        return (sizeof(float));
      }
      case data_buffer_type::write_binary: {
        float number = m_value;
        number = core::to_big_endian(number);
        size_t returnedBytes = 0;
        size_t totalBytes = 0;
        do {
          char* location =
              _dataBuffer.reserve(sizeof(float) - totalBytes, returnedBytes);
          wn::memory::memcpy(location,
              reinterpret_cast<uint8_t*>(&number) + totalBytes, returnedBytes);
          totalBytes += returnedBytes;
        } while (totalBytes < sizeof(float));
        return (sizeof(float));
      }
      case data_buffer_type::read_text: {
        char tempBuffer[20];
        size_t returnedBytes = 0;
        size_t totalBytes = 0;
        size_t countFails = 0;

        do {
          countFails++;
          char* location = _dataBuffer.reserve(20 - totalBytes, returnedBytes);
          wn::memory::memcpy(
              reinterpret_cast<uint8_t*>(&tempBuffer) + totalBytes, location,
              returnedBytes);
          totalBytes += returnedBytes;
          if (countFails > 1) {
            break;
          }
        } while (totalBytes < 20);

        size_t outSize =
            memory::WNReadFloat32(tempBuffer, m_value, totalBytes - 1);
        outSize += 1;  // consume the space that is supposed to be there
        return (outSize);
      }
      case data_buffer_type::write_text: {
        char tempBuffer[20];
        size_t outSize = memory::WNWriteFloat32(tempBuffer, m_value, 19);
        tempBuffer[outSize++] = ' ';

        size_t returnedBytes = 0;
        size_t totalBytes = 0;
        do {
          char* location =
              _dataBuffer.reserve(outSize - totalBytes, returnedBytes);
          wn::memory::memcpy(location,
              reinterpret_cast<uint8_t*>(&tempBuffer) + totalBytes,
              returnedBytes);
          totalBytes += returnedBytes;
        } while (totalBytes < outSize);

        return (outSize);
      }
    }

    return (0);
  }

private:
  float& m_value;
};

template <>
class serializer<char*> : public serializer_base {
public:
  serializer(char* _value, size_t _length)
    : m_value(_value), m_length(_length) {}

  virtual size_t serialize(
      data_buffer& _data_buffer, const uint32_t _flags) const override {
    (void)_flags;

    switch (_data_buffer.type()) {
      case data_buffer_type::read_binary:
      case data_buffer_type::read_text:
        return (read(_data_buffer));
      case data_buffer_type::write_binary:
      case data_buffer_type::write_text:
        return (write(_data_buffer));
    }

    return (0);
  }

private:
  size_t read(data_buffer& _data_buffer) const {
    size_t returned_bytes = 0;
    size_t total_bytes = 0;

    do {
      const char* location =
          _data_buffer.reserve(m_length - total_bytes, returned_bytes);

      wn::memory::memory_copy(m_value + total_bytes, location, returned_bytes);

      total_bytes += returned_bytes;
    } while (total_bytes < m_length);

    return (m_length);
  }

  size_t write(data_buffer& _data_buffer) const {
    size_t returned_bytes = 0;
    size_t total_bytes = 0;

    do {
      char* location =
          _data_buffer.reserve(m_length - total_bytes, returned_bytes);

      wn::memory::memory_copy(location, m_value + total_bytes, returned_bytes);

      total_bytes += returned_bytes;
    } while (total_bytes < m_length);

    return (m_length);
  }

  char* m_value;
  size_t m_length;
};
}  // namespace containers
}  // namespace wn

#endif  // __WN_CONTAINERS_SERIALIZER_H__
