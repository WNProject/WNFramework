// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_CONTAINERS_DYNAMIC_ARRAY_H__
#define __WN_CONTAINERS_DYNAMIC_ARRAY_H__

#include "WNCore/inc/WNTypes.h"
#include "WNMemory/inc/WNAllocator.h"
#include "WNMath/inc/WNBasic.h"

namespace WNContainers {
    template<typename type, typename T_Allocator = wn::default_expanding_allocator<50, 10>>
    class dynamic_array {
    public:
        typedef type* iterator;
        typedef type const* const_iterator;

        dynamic_array(wn::WNAllocator* _allocator = &dynamic_array::sAllocator): mSize(0), mMaxSize(0), mData(wn_nullptr) {
            mAllocator = _allocator;
        }

        dynamic_array(wn_size_t _size, wn::WNAllocator* _allocator = &dynamic_array::sAllocator): mSize(0), mMaxSize(0), mData(nullptr) {
            mAllocator = _allocator;
            reserve(_size);
        }

        dynamic_array(wn_size_t _size, const type& _elem, wn::WNAllocator* _allocator = &dynamic_array::sAllocator) : mSize(0), mData(nullptr), mMaxSize(0) {
            mAllocator = _allocator;
            insert(mData, _size, _elem);
        }

        ~dynamic_array() {
            if(mData) {
                for(wn_size_t i = 0; i < mSize; ++i) {
                    mData[i].~type();
                }
                mAllocator->Free(mData);
            }
        }

        template<typename T_Alloc>
        dynamic_array(const dynamic_array<type, T_Alloc>& _other) {
            mAllocator = &sAllocator;
            (*this) = _other;
        }

        dynamic_array(const dynamic_array& _other) {
            mAllocator = &sAllocator;
            (*this) = _other;
        }

        dynamic_array(dynamic_array&& _other) {
            mAllocator = &sAllocator;
            (*this) = std::move(_other);
        }

        template<typename T_Alloc>
        dynamic_array(dynamic_array<type, T_Alloc>&& _other) {
            mAllocator = &sAllocator;
            (*this) = std::move(_other);
        }

        template<typename T_Alloc>
        dynamic_array& operator=(const dynamic_array<type, T_Alloc>& _other) {
            if(&_other == this) {
                return(*this);
            }
            if(mData) {
                for(wn_size_t i = 0; i < mSize; ++i) {
                    mData[i].~type();
                }
            }
            mSize = 0;
            reserve(_other.mSize);
            for(wn_size_t i = 0; i < _other.mSize; ++i) {
                new(reinterpret_cast<void*>(&mData[i])) type(_other.mData[i]);
            }
            mSize = _other.mSize;
        }

        template<typename T_Alloc>
        dynamic_array& operator=(const dynamic_array<type, T_Alloc>&& _other) {
            if(&_other == this) {
                return(*this);
            }
            if(mData) {
                for(wn_size_t i = 0; i < mSize; ++i) {
                    mData[i].~type();
                }
                mAllocator->Free(mData);
            }
            mAllocator = _other.mAllocator;
            _other.mAllocator = &dynamic_array<type, T_Alloc>::sAllocator;
            mMaxSize = _other.mMaxSize;
            _other.mMaxSize = 0;
            mData = _other.mData;
            _other.mData = 0;
            mSize = _other.mSize;
            _other.mSize = 0;
        }

        wn_size_t size() {
            return(mSize);
        }
        wn_size_t capacity() {
            return(mMaxSize);
        }
        wn_bool empty() {
            return(mSize != 0);
        }

        wn_void swap_elements(iterator _first, iterator _second) {
            WN_DEBUG_ASSERT(_first >= mData && _first < mData + mSize);
            WN_DEBUG_ASSERT(_second >= mData && _second < mData + mSize);
            type t(std::move(*_first));
            *_first = std::move(*_second);
            *_second = std::move(t);
        }

        iterator emplace(const_iterator _position, type&& val) {
            iterator position = mData + (_position - mData);
            iterator newposition = shift(position, 1);
            position = newposition;
            new(reinterpret_cast<void*>(position)) type(std::move(val));
            return(newposition);
        }

        iterator insert(const_iterator _position, const type& val) {
            iterator position = mData + (_position - mData);
            iterator newposition = shift(position, 1);
            position = newposition;
            new(reinterpret_cast<void*>(position)) type(val);
            return(newposition);
        }

        template <typename T_Gen>
        iterator generate(const_iterator _position, wn_size_t _n, const T_Gen& _gen) {
            iterator position = mData + (_position - mData);
            iterator newposition = shift(position, _n);
            position = newposition;
            wn_size_t i = 0;
            while(_n--) {
                new(reinterpret_cast<void*>(position++)) type(_gen(i++));
            }
            return(newposition);
        }

        void resize(wn_size_t _numElements) {
            if(_numElements > mSize) {
                shift(mData, mSize - _numElements);
            } else {
                erase(mData + mSize, mData + mSize + _numElements);
            }
        }

        iterator insert(const_iterator _position, wn_size_t _n, const type& val) {
            iterator position = mData + (_position - mData);
            iterator newposition = shift(position, _n);
            position = newposition;
            while(_n--) {
                new(reinterpret_cast<void*>(position++)) type(val);
            }
            return(newposition);
        }

        iterator insert(const_iterator _position, iterator start, iterator end) {
            iterator position = mData + (_position - mData);
            iterator newposition = shift(position, end - start);
            position = newposition;
            while(start != end) {
                new(reinterpret_cast<void*>(position++)) type(*start);
                ++start;
            }
            return(newposition);
        }

        iterator erase(const_iterator _position) {
            WN_DEBUG_ASSERT(_position >= mData && _position < mData + mSize);
            return(unshift(_position, 1));
        }
        iterator erase(const_iterator _position, const_iterator _endPos) {
            WN_DEBUG_ASSERT(_position >= mData && _position <= mData + mSize);
            WN_DEBUG_ASSERT(_endPos >= mData && _endPos <= mData + mSize);
            wn_size_t i = _endPos - _position;
            return(unshift(_position, i));
        }
        iterator erase(const_iterator _position, wn_size_t _numElements) {
            WN_DEBUG_ASSERT(_position >= mData && _position <= mData + mSize);
            return(unshift(_position, _numElements));
        }
        template<typename T_Alloc>
        wn_void swap(dynamic_array<type, T_Alloc>& _other) {
            if(&_other == this) {
                return;
            }
            std::swap(mAllocator, _other.mAllocator);
            std::swap(mMaxSize, _other.mMaxSize);
            std::swap(mData, _other.mData);
            std::swap(mSize, _other.mSize);
        }

        wn_void reserve(wn_size_t _count) {
            if(_count < mMaxSize) {
                return;
            }
            if(!mData) {
                wn::WNAllocationPair pair = mAllocator->Allocate(sizeof(type), _count);
                mMaxSize = pair.count;
                mData = reinterpret_cast<type*>(pair.m_pLocation);
            } else {
                //TODO optimize this based on type traits
                wn::WNAllocationPair pair = mAllocator->AllocateForResize(sizeof(type), _count, mMaxSize);
                mMaxSize = pair.count;
                type* newData = reinterpret_cast<type*>(pair.m_pLocation);
                for(wn_size_t i = 0; i < mSize; ++i) {
                    new(reinterpret_cast<void*>(newData[i])) type(std::move(mData[i]));
                    mData[i].~type();
                }
                mAllocator->Free(mData);
                mData = newData;
            }
        }

        void push_back(const type& val) {
            insert(mData + mSize, val);
        }
        void push_back(type&& val) {
            emplace(mData + mSize, std::move(val));
        }

        void pop_back() {
            erase(mData + mSize - 1);
        }

        type& operator[](wn_size_t i) {
            return(mData[i]);
        }
        const type& operator[](wn_size_t i) const {
            return(mData[i]);
        }

        iterator begin() const {
            return(mData);
        }
        iterator end() const {
            return(mData + mSize);
        }
        const_iterator cbegin() const {
            return(mData);
        }
        const_iterator cend() const {
            return(mData + mSize);
        }

    private:
        iterator shift(const_iterator _startPt, wn_size_t _count) {
            WN_DEBUG_ASSERT(_startPt <= mData + mSize && (_startPt >= mData));
            wn_size_t originalPosition = _startPt - mData;
            if(mMaxSize < (mSize + _count)) {
                iterator startPt =  mData + (_startPt - mData);;
                wn::WNAllocationPair alloc = mAllocator->AllocateForResize(sizeof(type), mSize + _count, mMaxSize);
                mMaxSize = alloc.count;
                type* newData = reinterpret_cast<type*>(alloc.m_pLocation);
                type* allocated = newData;
                for(type* i = mData; i < startPt; ++i) {
                    new(reinterpret_cast<void*>(newData++)) type(std::move(*i));
                    i->~type();
                }
                newData+=_count;
                for(type* i = startPt; i < mData + mSize; ++i) {
                    new(reinterpret_cast<void*>(newData++)) type(std::move(*i));
                    i->~type();
                }
                mAllocator->Free(mData);
                mData = allocated;
            } else {
                for(type* i = mData + mSize + _count - 1; i > (_startPt + _count - 1); --i) {
                    new(reinterpret_cast<void*>(i))type(std::move(*(i - _count - 1)));
                    (i - _count - 1)->~type();
                }
            }
            mSize = mSize + _count;
            return(mData + originalPosition);
        }

        iterator unshift(const_iterator _startPt, wn_size_t _count) {
            WN_DEBUG_ASSERT(_startPt <= mData + mSize && (_startPt >= mData));
            WN_DEBUG_ASSERT(_startPt + _count <= mData + mSize);
            iterator startPt = mData + (_startPt - mData);
            for(type* i = startPt; i < startPt + _count; ++i){
                i->~type();
            }
            for(type* i = startPt + _count; i < mData + mSize; ++i) {
                new(reinterpret_cast<void*>(i - _count)) type(std::move(*i));
                i->~type();
            }
            mSize = mSize - _count;
            return(startPt);
        }

        template<typename T1, typename T2>
        friend class dynamic_array;

        static T_Allocator sAllocator;
        wn::WNAllocator* mAllocator;
        type* mData;
        wn_size_t mMaxSize;
        wn_size_t mSize;
    };

    template<typename type, typename T_Alloc> T_Alloc dynamic_array<type, T_Alloc>::sAllocator;
};

#endif
