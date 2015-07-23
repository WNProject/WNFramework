// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_CONTAINERS_SERIALIZER_H__
#define __WN_CONTAINERS_SERIALIZER_H__

#include "WNCore/inc/WNTypes.h"
#include "WNCore/inc/WNEndian.h"
#include "WNCore/inc/WNAssert.h"
#include "WNContainers/inc/WNSerializerBase.h"
#include "WNContainers/inc/WNDataBuffer.h"
#include "WNMemory/inc/WNBasic.h"
#include "WNMemory/inc/WNStringUtility.h"

namespace wn {
    namespace containers {
        template <typename _Type>
        class serializer final {
        public:
            static_assert(core::dependent_false<_Type>::value, "there is no associated serializer for the given type");
        };

        template <>
        class serializer<wn_int32> : public serializer_base {
        public:
            serializer(wn_int32& _value) :
                m_value(_value) {
            }

            virtual wn_size_t serialize(data_buffer& _dataBuffer, const wn_uint32 _flags) const override {
                WN_UNUSED_ARGUMENT(_flags);

                switch (_dataBuffer.type()) {
                    case data_buffer_type::read_binary: {
                        wn_size_t returnedBytes = 0;
                        wn_size_t totalBytes = 0;
                        do {
                            const wn_char* location = _dataBuffer.reserve(sizeof(wn_int32) - totalBytes, returnedBytes);
                            wn::memory::memcpy(reinterpret_cast<wn_uint8*>(&m_value) + totalBytes, location, returnedBytes);
                            totalBytes += returnedBytes;
                        } while (totalBytes < sizeof(wn_int32));

                        m_value = core::from_big_endian(m_value);

                        return(sizeof(wn_int32));
                    }
                    case data_buffer_type::write_binary: {
                        wn_int32 number = m_value;
                        number = core::to_big_endian(number);
                        wn_size_t returnedBytes = 0;
                        wn_size_t totalBytes = 0;
                        do {
                            wn_char* location = _dataBuffer.reserve(sizeof(wn_int32) - totalBytes, returnedBytes);
                            wn::memory::memcpy(location, reinterpret_cast<wn_uint8*>(&number) + totalBytes, returnedBytes);
                            totalBytes += returnedBytes;
                        } while (totalBytes < sizeof(wn_int32));
                        return(sizeof(wn_int32));
                    }
                    case data_buffer_type::read_text: {
                        wn_char tempBuffer[16];
                        wn_size_t returnedBytes = 0;
                        wn_size_t totalBytes = 0;
                        wn_size_t countFails = 0;

                        do {
                            countFails++;
                            wn_char* location = _dataBuffer.reserve(16 - totalBytes, returnedBytes);
                            wn::memory::memcpy(reinterpret_cast<wn_uint8*>(&tempBuffer) + totalBytes, location, returnedBytes);
                            totalBytes += returnedBytes;
                            if (countFails > 1) {
                                break;
                            }
                        } while (totalBytes < 4);

                        wn_size_t outSize = memory::WNReadInt32(tempBuffer, m_value, totalBytes - 1);
                        outSize += 1; //consume the space that is supposed to be there

                        return(outSize);
                    }
                    case data_buffer_type::write_text: {
                        wn_char tempBuffer[17];
                        wn_size_t outSize = memory::writeint32(tempBuffer, m_value, 16);
                        tempBuffer[outSize++] = ' ';

                        wn_size_t returnedBytes = 0;
                        wn_size_t totalBytes = 0;
                        do {
                            wn_char* location = _dataBuffer.reserve(outSize - totalBytes, returnedBytes);
                            wn::memory::memcpy(location, reinterpret_cast<wn_uint8*>(&tempBuffer) + totalBytes, returnedBytes);
                            totalBytes += returnedBytes;
                        } while (totalBytes < outSize);

                        return(outSize);
                    }
                }

                return(0);
            }

        private:
            wn_int32& m_value;
        };

        template <>
        class serializer<wn_int64> : public serializer_base {
        public:
            serializer(wn_int64& _value) :
                m_value(_value) {
            }

            virtual wn_size_t serialize(data_buffer& _dataBuffer, const wn_uint32 _flags) const override {
                WN_UNUSED_ARGUMENT(_flags);

                switch (_dataBuffer.type()) {
                    case data_buffer_type::read_binary: {
                        wn_size_t returnedBytes = 0;
                        wn_size_t totalBytes = 0;
                        do {
                            const wn_char* location = _dataBuffer.reserve(sizeof(wn_int64) - totalBytes, returnedBytes);
                            wn::memory::memcpy(reinterpret_cast<wn_uint8*>(&m_value) + totalBytes, location, returnedBytes);
                            totalBytes += returnedBytes;
                        } while (totalBytes < sizeof(wn_int64));

                        m_value = core::from_big_endian(m_value);

                        return(sizeof(wn_int64));
                    }
                    case data_buffer_type::write_binary: {
                        wn_int64 number = m_value;
                        number = core::to_big_endian(number);
                        wn_size_t returnedBytes = 0;
                        wn_size_t totalBytes = 0;
                        do {
                            wn_char* location = _dataBuffer.reserve(sizeof(wn_int64) - totalBytes, returnedBytes);
                            wn::memory::memcpy(location, reinterpret_cast<wn_uint8*>(&number) + totalBytes, returnedBytes);
                            totalBytes += returnedBytes;
                        } while (totalBytes < sizeof(wn_int64));
                        return(sizeof(wn_int64));
                    }
                    case data_buffer_type::read_text: {
                        wn_char tempBuffer[23];
                        wn_size_t returnedBytes = 0;
                        wn_size_t totalBytes = 0;
                        wn_size_t countFails = 0;

                        do {
                            countFails++;
                            wn_char* location = _dataBuffer.reserve(23 - totalBytes, returnedBytes);
                            wn::memory::memcpy(reinterpret_cast<wn_uint8*>(tempBuffer)+totalBytes, location, returnedBytes);
                            totalBytes += returnedBytes;
                            if (countFails > 1) {
                                break;
                            }
                        } while (totalBytes < 23);

                        wn_size_t outSize = memory::WNReadInt64(tempBuffer, m_value, totalBytes - 1);
                        outSize += 1; //consume the space that is supposed to be there
                        return(outSize);
                    }
                    case data_buffer_type::write_text: {
                        wn_char tempBuffer[24];
                        wn_size_t outSize = memory::writeint64(tempBuffer, m_value, 23);
                        tempBuffer[outSize++] = ' ';

                        wn_size_t returnedBytes = 0;
                        wn_size_t totalBytes = 0;
                        do {
                            wn_char* location = _dataBuffer.reserve(outSize - totalBytes, returnedBytes);
                            wn::memory::memcpy(location, reinterpret_cast<wn_uint8*>(&tempBuffer) + totalBytes, returnedBytes);
                            totalBytes += returnedBytes;
                        } while (totalBytes < outSize);

                        return(outSize);
                    }
                }

                return(0);
            }

        private:
            wn_int64& m_value;
        };

        template <>
        class serializer<wn_uint8> : public serializer_base {
        public:
            serializer(wn_uint8& _value) :
                m_value(_value) {
            }

            virtual wn_size_t serialize(data_buffer& _dataBuffer, const wn_uint32 _flags) const override {
                WN_UNUSED_ARGUMENT(_flags);

                switch (_dataBuffer.type()) {
                case data_buffer_type::read_binary: {
                        wn_size_t returnedBytes = 0;
                        wn_size_t totalBytes = 0;
                        do {
                            const wn_char* location = _dataBuffer.reserve(sizeof(wn_uint8) - totalBytes, returnedBytes);

                            wn::memory::memcpy(reinterpret_cast<wn_uint8*>(&m_value) + totalBytes, location, returnedBytes);

                            totalBytes += returnedBytes;
                        } while (totalBytes < sizeof(wn_uint8));

                        return(sizeof(wn_uint8));
                    }
                    case data_buffer_type::write_binary: {
                        wn_size_t returnedBytes = 0;
                        wn_size_t totalBytes = 0;
                        do {
                            wn_char* location = _dataBuffer.reserve(sizeof(wn_uint8) - totalBytes, returnedBytes);
                            wn::memory::memcpy(location, reinterpret_cast<wn_uint8*>(&m_value) + totalBytes, returnedBytes);
                            totalBytes += returnedBytes;
                        } while (totalBytes < sizeof(wn_uint8));

                        return(sizeof(wn_uint8));
                    }
                    case data_buffer_type::read_text: {
                        wn_char tempBuffer[4];
                        wn_size_t returnedBytes = 0;
                        wn_size_t totalBytes = 0;
                        wn_size_t countFails = 0;

                        do {
                            countFails++;
                            wn_char* location = _dataBuffer.reserve(4 - totalBytes, returnedBytes);

                            wn::memory::memcpy(reinterpret_cast<wn_uint8*>(&tempBuffer) + totalBytes, location, returnedBytes);

                            totalBytes += returnedBytes;
                            if (countFails > 1) {
                                break;
                            }
                        } while (totalBytes < 4);

                        wn_uint32 number;
                        wn_size_t outSize = memory::WNReadUInt32(tempBuffer, number, totalBytes - 1);

                        m_value = number & 0xFF;
                        outSize += 1; //consume the space that is supposed to be there

                        return(outSize);
                    }
                    case data_buffer_type::write_text: {
                        wn_char tempBuffer[4];
                        wn_size_t outSize = memory::writeuint32(tempBuffer, m_value, 3);
                        tempBuffer[outSize++] = ' ';

                        wn_size_t returnedBytes = 0;
                        wn_size_t totalBytes = 0;
                        do {
                            wn_char* location = _dataBuffer.reserve(outSize - totalBytes, returnedBytes);
                            wn::memory::memcpy(location, reinterpret_cast<wn_uint8*>(&tempBuffer) + totalBytes, returnedBytes);
                            totalBytes += returnedBytes;
                        } while (totalBytes < outSize);

                        return(outSize);
                    }
                }

                return(0);
            }

        private:
            wn_uint8& m_value;
        };

        template <>
        class serializer<wn_uint32> : public serializer_base {
        public:
            serializer(wn_uint32& _value) :
                m_value(_value) {
            }

            virtual wn_size_t serialize(data_buffer& _dataBuffer, const wn_uint32 _flags) const override {
                WN_UNUSED_ARGUMENT(_flags);

                switch (_dataBuffer.type()) {
                    case data_buffer_type::read_binary: {
                        wn_size_t returnedBytes = 0;
                        wn_size_t totalBytes = 0;
                        do {
                            const wn_char* location = _dataBuffer.reserve(sizeof(wn_uint32) - totalBytes, returnedBytes);
                            wn::memory::memcpy(reinterpret_cast<wn_uint8*>(&m_value) + totalBytes, location, returnedBytes);
                            totalBytes += returnedBytes;
                        } while (totalBytes < sizeof(wn_uint32));

                        m_value = core::from_big_endian(m_value);

                        return(sizeof(wn_uint32));
                    }
                    case data_buffer_type::write_binary: {
                        wn_uint32 number = m_value;
                        number = core::to_big_endian(number);
                        wn_size_t returnedBytes = 0;
                        wn_size_t totalBytes = 0;
                        do {
                            wn_char* location = _dataBuffer.reserve(sizeof(wn_uint32) - totalBytes, returnedBytes);
                            wn::memory::memcpy(location, reinterpret_cast<wn_uint8*>(&number) + totalBytes, returnedBytes);
                            totalBytes += returnedBytes;
                        } while (totalBytes < sizeof(wn_uint32));
                        return(sizeof(wn_uint32));
                    }
                    case data_buffer_type::read_text: {
                        wn_char tempBuffer[16];
                        wn_size_t returnedBytes = 0;
                        wn_size_t totalBytes = 0;
                        wn_size_t countFails = 0;

                        do {
                            countFails++;
                            wn_char* location = _dataBuffer.reserve(16 - totalBytes, returnedBytes);
                            wn::memory::memcpy(reinterpret_cast<wn_uint8*>(tempBuffer)+totalBytes, location, returnedBytes);
                            totalBytes += returnedBytes;
                            if (countFails > 1) {
                                break;
                            }
                        } while (totalBytes < 16);

                        wn_size_t outSize = memory::WNReadUInt32(tempBuffer, m_value, totalBytes - 1);
                        outSize += 1; //consume the space that is supposed to be there
                        return(outSize);
                    }
                    case data_buffer_type::write_text: {
                        wn_char tempBuffer[17];
                        wn_size_t outSize = memory::writeuint32(tempBuffer, m_value, 16);
                        tempBuffer[outSize++] = ' ';

                        wn_size_t returnedBytes = 0;
                        wn_size_t totalBytes = 0;
                        do {
                            wn_char* location = _dataBuffer.reserve(outSize - totalBytes, returnedBytes);
                            wn::memory::memcpy(location, reinterpret_cast<wn_uint8*>(&tempBuffer) + totalBytes, returnedBytes);
                            totalBytes += returnedBytes;
                        } while (totalBytes < outSize);

                        return(outSize);
                    }
                }

                return(0);
            }

        private:
            wn_uint32& m_value;
        };

        template <>
        class serializer<wn_uint64> : public serializer_base {
        public:
            serializer(wn_uint64& _value) :
                m_value(_value) {
            }

            virtual wn_size_t serialize(data_buffer& _dataBuffer, const wn_uint32 _flags) const override {
                WN_UNUSED_ARGUMENT(_flags);

                switch (_dataBuffer.type()) {
                    case data_buffer_type::read_binary: {
                        wn_size_t returnedBytes = 0;
                        wn_size_t totalBytes = 0;
                        do {
                            const wn_char* location = _dataBuffer.reserve(sizeof(wn_uint64) - totalBytes, returnedBytes);
                            wn::memory::memcpy(reinterpret_cast<wn_uint8*>(&m_value) + totalBytes, location, returnedBytes);
                            totalBytes += returnedBytes;
                        } while (totalBytes < sizeof(wn_uint64));

                        m_value = core::from_big_endian(m_value);

                        return(sizeof(wn_uint64));
                    }
                    case data_buffer_type::write_binary: {
                        wn_uint64 number = m_value;
                        number = core::to_big_endian(number);
                        wn_size_t returnedBytes = 0;
                        wn_size_t totalBytes = 0;
                        do {
                            wn_char* location = _dataBuffer.reserve(sizeof(wn_uint64) - totalBytes, returnedBytes);
                            wn::memory::memcpy(location, reinterpret_cast<wn_uint8*>(&number) + totalBytes, returnedBytes);
                            totalBytes += returnedBytes;
                        } while (totalBytes < sizeof(wn_uint64));
                        return(sizeof(wn_uint64));
                    }
                    case data_buffer_type::read_text: {
                        wn_char tempBuffer[23];
                        wn_size_t returnedBytes = 0;
                        wn_size_t totalBytes = 0;
                        wn_size_t countFails = 0;

                        do {
                            countFails++;
                            wn_char* location = _dataBuffer.reserve(23 - totalBytes, returnedBytes);
                            wn::memory::memcpy(reinterpret_cast<wn_uint8*>(tempBuffer)+totalBytes, location, returnedBytes);
                            totalBytes += returnedBytes;
                            if (countFails > 1) {
                                break;
                            }
                        } while (totalBytes < 23);

                        wn_size_t outSize = memory::WNReadUInt64(tempBuffer, m_value, totalBytes - 1);
                        outSize += 1; //consume the space that is supposed to be there
                        return(outSize);
                    }
                    case data_buffer_type::write_text: {
                        wn_char tempBuffer[24];
                        wn_size_t outSize = memory::writeuint64(tempBuffer, m_value, 23);
                        tempBuffer[outSize++] = ' ';

                        wn_size_t returnedBytes = 0;
                        wn_size_t totalBytes = 0;
                        do {
                            wn_char* location = _dataBuffer.reserve(outSize - totalBytes, returnedBytes);
                            wn::memory::memcpy(location, reinterpret_cast<wn_uint8*>(&tempBuffer) + totalBytes, returnedBytes);
                            totalBytes += returnedBytes;
                        } while (totalBytes < outSize);

                        return(outSize);
                    }
                }

                return(0);
            }


        private:
            wn_uint64& m_value;
        };

        template <>
        class serializer<wn_float32> : public serializer_base {
        public:
            serializer(wn_float32& _value) :
                m_value(_value) {
            }

            virtual wn_size_t serialize(data_buffer& _dataBuffer, const wn_uint32 _flags) const override {
                WN_UNUSED_ARGUMENT(_flags);

                switch (_dataBuffer.type()) {
                case data_buffer_type::read_binary: {
                    wn_size_t returnedBytes = 0;
                    wn_size_t totalBytes = 0;
                    do {
                        const wn_char* location = _dataBuffer.reserve(sizeof(wn_float32) - totalBytes, returnedBytes);
                        wn::memory::memcpy(reinterpret_cast<wn_uint8*>(&m_value) + totalBytes, location, returnedBytes);
                        totalBytes += returnedBytes;
                    } while (totalBytes < sizeof(wn_float32));

                    m_value = core::from_big_endian(m_value);

                    return(sizeof(wn_float32));
                }
                case data_buffer_type::write_binary: {
                    wn_float32 number = m_value;
                    number = core::to_big_endian(number);
                    wn_size_t returnedBytes = 0;
                    wn_size_t totalBytes = 0;
                    do {
                        wn_char* location = _dataBuffer.reserve(sizeof(wn_float32) - totalBytes, returnedBytes);
                        wn::memory::memcpy(location, reinterpret_cast<wn_uint8*>(&number) + totalBytes, returnedBytes);
                        totalBytes += returnedBytes;
                    } while (totalBytes < sizeof(wn_float32));
                    return(sizeof(wn_float32));
                }
                case data_buffer_type::read_text: {
                    wn_char tempBuffer[20];
                    wn_size_t returnedBytes = 0;
                    wn_size_t totalBytes = 0;
                    wn_size_t countFails = 0;

                    do {
                        countFails++;
                        wn_char* location = _dataBuffer.reserve(20 - totalBytes, returnedBytes);
                        wn::memory::memcpy(reinterpret_cast<wn_uint8*>(&tempBuffer) + totalBytes, location, returnedBytes);
                        totalBytes += returnedBytes;
                        if (countFails > 1) {
                            break;
                        }
                    } while (totalBytes < 20);

                    wn_size_t outSize = memory::WNReadFloat32(tempBuffer, m_value, totalBytes - 1);
                    outSize += 1; //consume the space that is supposed to be there
                    return(outSize);
                }
                case data_buffer_type::write_text: {
                    wn_char tempBuffer[20];
                    wn_size_t outSize = memory::WNWriteFloat32(tempBuffer, m_value, 19);
                    tempBuffer[outSize++] = ' ';

                    wn_size_t returnedBytes = 0;
                    wn_size_t totalBytes = 0;
                    do {
                        wn_char* location = _dataBuffer.reserve(outSize - totalBytes, returnedBytes);
                        wn::memory::memcpy(location, reinterpret_cast<wn_uint8*>(&tempBuffer) + totalBytes, returnedBytes);
                        totalBytes += returnedBytes;
                    } while (totalBytes < outSize);

                    return(outSize);
                }
                }

                return(0);
            }

        private:
            wn_float32& m_value;
        };

        template <>
        class serializer<wn_char*> : public serializer_base {
        public:
            serializer(wn_char* _value, wn_size_t _length) :
                m_value(_value),
                m_length(_length) {
            }

            virtual wn_size_t serialize(data_buffer& _data_buffer, const wn_uint32 _flags) const override {
                WN_UNUSED_ARGUMENT(_flags);

                switch (_data_buffer.type()) {
                    case data_buffer_type::read_binary:
                    case data_buffer_type::read_text:
                        return(read(_data_buffer));
                    case data_buffer_type::write_binary:
                    case data_buffer_type::write_text:
                        return(write(_data_buffer));
                }

                return(0);
            }

        private:
            wn_size_t read(data_buffer& _data_buffer) const {
                wn_size_t returned_bytes = 0;
                wn_size_t total_bytes = 0;

                do {
                    const wn_char* location = _data_buffer.reserve(m_length - total_bytes, returned_bytes);

                    wn::memory::memory_copy(m_value + total_bytes, location, returned_bytes);

                    total_bytes += returned_bytes;
                } while (total_bytes < m_length);

                return(m_length);
            }

            wn_size_t write(data_buffer& _data_buffer) const {
                wn_size_t returned_bytes = 0;
                wn_size_t total_bytes = 0;

                do {
                    wn_char* location = _data_buffer.reserve(m_length - total_bytes, returned_bytes);

                    wn::memory::memory_copy(location, m_value + total_bytes, returned_bytes);

                    total_bytes += returned_bytes;
                } while (total_bytes < m_length);

                return(m_length);
            }

            wn_char* m_value;
            wn_size_t m_length;
        };
    }
}

#endif // __WN_CONTAINERS_SERIALIZER_H__