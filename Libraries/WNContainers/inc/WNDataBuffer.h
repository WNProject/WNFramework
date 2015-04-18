// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_CONTAINERS_DATA_BUFFER_H__
#define __WN_CONTAINERS_DATA_BUFFER_H__

#include "WNCore/inc/WNTypes.h"

namespace wn {
    namespace containers {
        class serializer_base;

        enum class data_buffer_type {
            read_binary,
            write_binary,
            read_text,
            write_text
        };

        static const wn_size_t MAX_DATA_WRITE = 1024;

        class data_buffer {
        public:
            virtual wn_bool serialize(const serializer_base& _serializer, const wn_uint32 _flags) = 0;
            virtual wn_char* reserve(const wn_size_t _count, wn_size_t& _reserved_count) = 0;
            virtual data_buffer_type type() const = 0;
        };
    }
}

#endif // __WN_CONTAINERS_DATA_BUFFER_H__