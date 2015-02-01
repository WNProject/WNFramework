// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_CONCURRENCY_INTRUSIVE_PTR_H__
#define __WN_CONCURRENCY_INTRUSIVE_PTR_H__

#include "WNMemory/inc/WNAllocation.h"

#include <type_traits>

namespace wn {
   template <typename type>
    class intrusive_ptr final {
    public:
        typedef type element_type;

        WN_FORCE_INLINE intrusive_ptr() :
            m_pointer(wn_nullptr) {
        }

        WN_FORCE_INLINE explicit intrusive_ptr(const wn_nullptr_t) :
            intrusive_ptr() {
        }

        WN_FORCE_INLINE explicit intrusive_ptr(type* _pointer) :
            intrusive_ptr() {
            reset(_pointer);
        }

        WN_FORCE_INLINE explicit intrusive_ptr(type* _pointer, const wn_bool _add_reference) :
            intrusive_ptr() {
            reset(_pointer, _add_reference);
        }

        template <typename other_type>
        WN_FORCE_INLINE explicit intrusive_ptr(other_type* _pointer) :
            intrusive_ptr() {
            reset(_pointer);
        }

        template <typename other_type>
        WN_FORCE_INLINE explicit intrusive_ptr(other_type* _pointer, const wn_bool _add_reference) :
            intrusive_ptr() {
            reset(_pointer, _add_reference);
        }

        WN_FORCE_INLINE intrusive_ptr(intrusive_ptr&& _intrusive_ptr) :
            intrusive_ptr() {
            reset(_intrusive_ptr.get());

            _intrusive_ptr.reset();
        }

        WN_FORCE_INLINE intrusive_ptr(const intrusive_ptr& _intrusive_ptr) :
            intrusive_ptr() {
            reset(_intrusive_ptr.get());
        }

        template <typename other_type>
        WN_FORCE_INLINE intrusive_ptr(intrusive_ptr<other_type>&& _intrusive_ptr) :
            intrusive_ptr() {
            reset(_intrusive_ptr.get());

            _intrusive_ptr.reset();
        }

        template <typename other_type>
        WN_FORCE_INLINE intrusive_ptr(const intrusive_ptr<other_type>& _intrusive_ptr) :
            intrusive_ptr() {
            reset(_intrusive_ptr.get());
        }

        WN_FORCE_INLINE ~intrusive_ptr() {
            remove_reference();
        }

        WN_FORCE_INLINE type* operator -> () const {
            return(get());
        }

        WN_FORCE_INLINE type& operator * () const {
            return(*get());
        }

        WN_FORCE_INLINE operator wn_bool() const {
            return(get() != wn_nullptr);
        }

        WN_FORCE_INLINE intrusive_ptr& operator = (const wn_nullptr_t) {
            reset();

            return(*this);
        }

        WN_FORCE_INLINE intrusive_ptr& operator = (intrusive_ptr&& _intrusive_ptr) {
            reset(_intrusive_ptr.get());

            _intrusive_ptr.reset();

            return(*this);
        }

        WN_FORCE_INLINE intrusive_ptr& operator = (const intrusive_ptr& _intrusive_ptr) {
            reset(_intrusive_ptr.get());

            return(*this);
        }

        template <typename other_type>
        WN_FORCE_INLINE intrusive_ptr& operator = (intrusive_ptr<other_type>&& _intrusive_ptr) {
            reset(_intrusive_ptr.get());

            _intrusive_ptr.reset();

            return(*this);
        }

        template <typename other_type>
        WN_FORCE_INLINE intrusive_ptr& operator = (const intrusive_ptr<other_type>& _intrusive_ptr) {
            reset(_intrusive_ptr.get());

            return(*this);
        }

        WN_FORCE_INLINE type* get() const {
            return(m_pointer);
        }

        WN_FORCE_INLINE wn_size_t use_count() const {
            const type* pointer = get();

            if (pointer != wn_nullptr) {
                return(pointer->reference_count());
            }

            return(0);
        }

        WN_FORCE_INLINE wn_bool unique() const {
            return(use_count() == 1);
        }

        WN_FORCE_INLINE wn_void reset() {
            remove_reference();
        }

        WN_FORCE_INLINE wn_void reset(wn_nullptr_t) {
            reset();
        }

        WN_FORCE_INLINE wn_void reset(type* _pointer) {
            if (_pointer != wn_nullptr) {
                _pointer->add_reference();
            }

            remove_reference();

            m_pointer = _pointer;
        }

        WN_FORCE_INLINE wn_void reset(type* _pointer, const wn_bool _add_reference) {
            if (_pointer != wn_nullptr && _add_reference) {
                _pointer->add_reference();
            }

            remove_reference();

            m_pointer = _pointer;
        }

        template <typename other_type>
        WN_FORCE_INLINE wn_void reset(other_type* _pointer) {
            if (_pointer != wn_nullptr) {
                _pointer->add_reference();
            }

            remove_reference();

            m_pointer = _pointer;
        }

        template <typename other_type>
        WN_FORCE_INLINE wn_void reset(other_type* _pointer, const wn_bool _add_reference) {
            if (_pointer != wn_nullptr && _add_reference) {
                _pointer->add_reference();
            }

            remove_reference();

            m_pointer = _pointer;
        }

        WN_FORCE_INLINE wn_void swap(intrusive_ptr& _intrusive_ptr) {
            const intrusive_ptr pointer(_intrusive_ptr);

            _intrusive_ptr.reset(get());

            reset(pointer.get());
        }

        WN_FORCE_INLINE type* release() {
            type* pointer = get();

            m_pointer = wn_nullptr;

            return(pointer);
        }

    private:
        template <typename other_type>
        friend class intrusive_ptr;

        WN_FORCE_INLINE wn_void add_reference() const {
            if (m_pointer != wn_nullptr) {
                m_pointer->add_reference();
            }
        }

        WN_FORCE_INLINE wn_void remove_reference() {
            if (m_pointer != wn_nullptr) {
                if (m_pointer->remove_reference()) {
                    WN_DELETE(m_pointer);
                }
            }

            m_pointer = wn_nullptr;
        }

        type* m_pointer;
    };

    template <typename type1, typename type2>
    WN_FORCE_INLINE wn_bool operator == (const intrusive_ptr<type1>& _intrusive_ptr1,
                                         const intrusive_ptr<type2>& _intrusive_ptr2) {
        return(_intrusive_ptr1.get() == _intrusive_ptr2.get());
    }

    template <typename type1, typename type2>
    WN_FORCE_INLINE wn_bool operator != (const intrusive_ptr<type1>& _intrusive_ptr1,
                                         const intrusive_ptr<type2>& _intrusive_ptr2) {
        return(_intrusive_ptr1.get() != _intrusive_ptr2.get());
    }

    template <typename type1, typename type2>
    WN_FORCE_INLINE wn_bool operator < (const intrusive_ptr<type1>& _intrusive_ptr1,
                                        const intrusive_ptr<type2>& _intrusive_ptr2) {
        return(_intrusive_ptr1.get() < _intrusive_ptr2.get());
    }

    template <typename type1, typename type2>
    WN_FORCE_INLINE wn_bool operator > (const intrusive_ptr<type1>& _intrusive_ptr1,
                                        const intrusive_ptr<type2>& _intrusive_ptr2) {
        return(_intrusive_ptr1.get() > _intrusive_ptr2.get());
    }

    template <typename type1, typename type2>
    WN_FORCE_INLINE wn_bool operator <= (const intrusive_ptr<type1>& _intrusive_ptr1,
                                         const intrusive_ptr<type2>& _intrusive_ptr2) {
        return(_intrusive_ptr1.get() <= _intrusive_ptr2.get());
    }

    template <typename type1, typename type2>
    WN_FORCE_INLINE wn_bool operator >= (const intrusive_ptr<type1>& _intrusive_ptr1,
                                         const intrusive_ptr<type2>& _intrusive_ptr2) {
        return(_intrusive_ptr1.get() >= _intrusive_ptr2.get());
    }

    template <typename type>
    WN_FORCE_INLINE wn_bool operator == (const intrusive_ptr<type>& _intrusive_ptr, const wn_nullptr_t) {
        return(_intrusive_ptr.get() == wn_nullptr);
    }

    template <typename type>
    WN_FORCE_INLINE wn_bool operator == (const wn_nullptr_t, const intrusive_ptr<type>& _intrusive_ptr) {
        return(wn_nullptr == _intrusive_ptr.get());
    }

    template <typename type>
    WN_FORCE_INLINE wn_bool operator != (const intrusive_ptr<type>& _intrusive_ptr, const wn_nullptr_t) {
        return(_intrusive_ptr.get() != wn_nullptr);
    }

    template <typename type>
    WN_FORCE_INLINE wn_bool operator != (const wn_nullptr_t, const intrusive_ptr<type>& _intrusive_ptr) {
        return(wn_nullptr != _intrusive_ptr.get());
    }

    template <typename type>
    WN_FORCE_INLINE wn_bool operator < (const intrusive_ptr<type>& _intrusive_ptr, const wn_nullptr_t) {
        return(_intrusive_ptr.get() < wn_nullptr);
    }

    template <typename type>
    WN_FORCE_INLINE wn_bool operator < (const wn_nullptr_t, const intrusive_ptr<type>& _intrusive_ptr) {
        return(wn_nullptr < _intrusive_ptr.get());
    }

    template <typename type>
    WN_FORCE_INLINE wn_bool operator >(const intrusive_ptr<type>& _intrusive_ptr, const wn_nullptr_t) {
        return(_intrusive_ptr.get() > wn_nullptr);
    }

    template <typename type>
    WN_FORCE_INLINE wn_bool operator > (const wn_nullptr_t, const intrusive_ptr<type>& _intrusive_ptr) {
        return(wn_nullptr > _intrusive_ptr.get());
    }

    template <typename type>
    WN_FORCE_INLINE wn_bool operator <= (const intrusive_ptr<type>& _intrusive_ptr, const wn_nullptr_t) {
        return(_intrusive_ptr.get() <= wn_nullptr);
    }

    template <typename type>
    WN_FORCE_INLINE wn_bool operator <= (const wn_nullptr_t, const intrusive_ptr<type>& _intrusive_ptr) {
        return(wn_nullptr <= _intrusive_ptr.get());
    }

    template <typename type>
    WN_FORCE_INLINE wn_bool operator >= (const intrusive_ptr<type>& _intrusive_ptr, const wn_nullptr_t) {
        return(_intrusive_ptr.get() >= wn_nullptr);
    }

    template <typename type>
    WN_FORCE_INLINE wn_bool operator >= (const wn_nullptr_t, const intrusive_ptr<type>& _intrusive_ptr) {
        return(wn_nullptr >= _intrusive_ptr.get());
    }

    template <typename type, typename... arguments>
    WN_FORCE_INLINE intrusive_ptr<type> make_intrusive(arguments&&... _arguments) {
        return(intrusive_ptr<type>(WN_NEW type(std::forward<arguments>(_arguments)...)));
    }
}

#endif // __WN_CONCURRENCY_INTRUSIVE_PTR_H__