// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

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

        static const size_t MAX_DATA_WRITE = 1024;

        class data_buffer {
        public:
            virtual bool serialize(const serializer_base& _serializer, const uint32_t _flags) = 0;
            virtual char* reserve(const size_t _count, size_t& _reserved_count) = 0;
            virtual data_buffer_type type() const = 0;
        };
    }
}

#endif // __WN_CONTAINERS_DATA_BUFFER_H__