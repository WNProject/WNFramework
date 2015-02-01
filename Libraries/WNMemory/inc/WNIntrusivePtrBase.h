// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_CONCURRENCY_INTRUSIVE_PTR_BASE_H__
#define __WN_CONCURRENCY_INTRUSIVE_PTR_BASE_H__

#include "WNCore/inc/WNTypes.h"

#include <atomic>

namespace wn {
    class intrusive_ptr_base {
    public:
        WN_FORCE_INLINE wn_void add_reference() {
            for (;;) {
                wn_size_t count = m_reference_count;
                const wn_size_t target_count = count + 1;

                if (m_reference_count.compare_exchange_strong(count, target_count)) {
                    break;
                }
            }
        }

        WN_FORCE_INLINE wn_bool remove_reference() {
            for (;;) {
                wn_size_t count = m_reference_count;
                const wn_size_t target_count = count - 1;

                if (m_reference_count.compare_exchange_strong(count, target_count)) {
                    return(target_count == 0);
                }
            }
        }

        WN_FORCE_INLINE wn_size_t reference_count() const {
            return(m_reference_count);
        }

    protected:
        WN_FORCE_INLINE intrusive_ptr_base() :
            m_reference_count(0) {
        }

    private:
        std::atomic<wn_size_t> m_reference_count;
    };
}

#endif // __WN_CONCURRENCY_INTRUSIVE_PTR_BASE_H__