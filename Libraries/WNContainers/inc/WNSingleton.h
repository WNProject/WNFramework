// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_CONTAINERS_SINGLETON_H__
#define __WN_CONTAINERS_SINGLETON_H__

#include "WNCore/inc/WNTypes.h"

#include <mutex>

namespace WNContainers {
    template <typename type>
    class singleton {
    public:
        static wn_void initialize() {
            std::call_once(m_once, &create_instance);
        }

        static wn_void release() {
            if (m_instance != wn_nullptr) {
                wn::memory::destroy(m_instance);

                m_instance = wn_nullptr;

                m_once.Reset();
            }
        }

        static type* get() {
            initialize();

            return(m_instance);
        }

    protected:
        static type* m_instance;

    private:
        static wn_void create_instance() {
            if (m_instance == wn_nullptr) {
                m_instance = WN_NEW type;
            }
        }

        static std::once_flag m_once;
    };

    template <typename type>
    type* singleton<type>::m_instance = wn_nullptr;

    template <typename type>
    std::once_flag singleton<type>::m_once;
}

#endif // __WN_CONTAINERS_SINGLETON_H__
