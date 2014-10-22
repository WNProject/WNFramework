// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_DYNAMIC_ARRAY_H__
#define __WN_DYNAMIC_ARRAY_H__
#include "WNCore/inc/WNTypes.h"
#include "WNContainers/inc/WNAllocator.h"
#include "WNMath/inc/WNBasic.h"

namespace WNContainers {
    template<typename T, typename T_Allocator=WNDefaultExpandingAllocator<50, 10>>
    class WNDynamicArray {
    public:
        typedef T* iterator;
        typedef T const * const_iterator;

        WNDynamicArray(WNAllocator* _allocator = &WNDynamicArray::sAllocator): mSize(0), mMaxSize(0), mData(WN_NULL) {
            mAllocator = _allocator;
        }

        WNDynamicArray(WN_SIZE_T _size, WNAllocator* _allocator = &WNDynamicArray::sAllocator): mSize(0), mMaxSize(0), mData(nullptr) {
            mAllocator = _allocator;
            reserve(_size);
        }

        WNDynamicArray(WN_SIZE_T _size, const T& _elem, WNAllocator* _allocator = &WNDynamicArray::sAllocator): mSize(0), mData(nullptr), mMaxSize(0) {
            mAllocator = _allocator;
            insert(mData, _size, _elem);
        }

        ~WNDynamicArray() {
            if(mData) {
                for(WN_SIZE_T i = 0; i < mSize; ++i) {
                    mData[i].~T();
                }
                mAllocator->Free(mData);
            }
        }
        
        template<typename T_Alloc>
        WNDynamicArray(const WNDynamicArray<T, T_Alloc>& _other) {
            mAllocator = &sAllocator;
            (*this) = _other;
        }

        WNDynamicArray(const WNDynamicArray& _other) {
            mAllocator = &sAllocator;
            (*this) = _other;
        }

        WNDynamicArray(WNDynamicArray&& _other) {
            mAllocator = &sAllocator;
            (*this) = std::move(_other);
        }

        template<typename T_Alloc>
        WNDynamicArray(WNDynamicArray<T, T_Alloc>&& _other) {
            mAllocator = &sAllocator;
            (*this) = std::move(_other);
        }

        template<typename T_Alloc>
        WNDynamicArray& operator=(const WNDynamicArray<T, T_Alloc>& _other) {
            if(&_other == this) {
                return(*this);
            }
            if(mData) {
                for(WN_SIZE_T i = 0; i < mSize; ++i) {
                    mData[i].~T();
                }
            }
            mSize = 0;
            reserve(_other.mSize);
            for(WN_SIZE_T i = 0; i < _other.mSize; ++i) {
                new(reinterpret_cast<void*>(&mData[i])) T(_other.mData[i]);
            }
            mSize = _other.mSize;
        }

        template<typename T_Alloc>
        WNDynamicArray& operator=(const WNDynamicArray<T, T_Alloc>&& _other) {
            if(&_other == this) {
                return(*this);
            }
            if(mData) {
                for(WN_SIZE_T i = 0; i < mSize; ++i) {
                    mData[i].~T();
                }
                mAllocator->Free(mData);
            }
            mAllocator = _other.mAllocator;
            _other.mAllocator = &WNDynamicArray<T, T_Alloc>::sAllocator;
            mMaxSize = _other.mMaxSize;
            _other.mMaxSize = 0;
            mData = _other.mData;
            _other.mData = 0;
            mSize = _other.mSize;
            _other.mSize = 0;
        }

        WN_SIZE_T size() {
            return(mSize);
        }
        WN_SIZE_T capacity() {
            return(mMaxSize);
        }
        WN_BOOL empty() {
            return(mSize != 0);
        }

        WN_VOID swap_elements(iterator _first, iterator _second) {
            WN_DEBUG_ASSERT(_first >= mData && _first < mData + mSize);
            WN_DEBUG_ASSERT(_second >= mData && _second < mData + mSize);
            T t(std::move(*_first));
            *_first = std::move(*_second);
            *_second = std::move(t);
        }

        iterator emplace(const_iterator _position, T&& val) {
            iterator position = mData + (_position - mData);
            iterator newposition = shift(position, 1);
            position = newposition;
            new(reinterpret_cast<void*>(position)) T(std::move(val));
            return(newposition);
        }

        iterator insert(const_iterator _position, const T& val) {
            iterator position = mData + (_position - mData);
            iterator newposition = shift(position, 1);
            position = newposition;
            new(reinterpret_cast<void*>(position)) T(val);
            return(newposition);
        }

        template <typename T_Gen> 
        iterator generate(const_iterator _position, WN_SIZE_T _n, const T_Gen& _gen) {
            iterator position = mData + (_position - mData);
            iterator newposition = shift(position, _n);
            position = newposition;
            WN_SIZE_T i = 0;
            while(_n--) {
                new(reinterpret_cast<void*>(position++)) T(_gen(i++));
            }
            return(newposition);
        }
        
        void resize(WN_SIZE_T _numElements) {
            if(_numElements > mSize) {
                shift(mData, mSize - _numElements);
            } else {
                erase(mData + mSize, mData + mSize + _numElements);
            }
        }

        iterator insert(const_iterator _position, WN_SIZE_T _n, const T& val) {
            iterator position = mData + (_position - mData);
            iterator newposition = shift(position, _n);
            position = newposition;
            while(_n--) {
                new(reinterpret_cast<void*>(position++)) T(val);
            }
            return(newposition);
        }

        iterator insert(const_iterator _position, iterator start, iterator end) {
            iterator position = mData + (_position - mData);
            iterator newposition = shift(position, end - start);
            position = newposition;
            while(start != end) {
                new(reinterpret_cast<void*>(position++)) T(*start);
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
            WN_SIZE_T i = _endPos - _position;
            return(unshift(_position, i));
        }
        iterator erase(const_iterator _position, WN_SIZE_T _numElements) {
            WN_DEBUG_ASSERT(_position >= mData && _position <= mData + mSize);
            return(unshift(_position, _numElements));
        }
        template<typename T_Alloc>
        WN_VOID swap(WNDynamicArray<T, T_Alloc>& _other) {
            if(&_other == this) {
                return;
            }
            std::swap(mAllocator, _other.mAllocator);
            std::swap(mMaxSize, _other.mMaxSize);
            std::swap(mData, _other.mData);
            std::swap(mSize, _other.mSize);
        }

        WN_VOID reserve(WN_SIZE_T _count) {
            if(_count < mMaxSize) {
                return;
            }
            if(!mData) {
                WNAllocationPair pair = mAllocator->Allocate(sizeof(T), _count);
                mMaxSize = pair.count;
                mData = reinterpret_cast<T*>(pair.m_pLocation);
            } else {
                //TODO optimize this based on type traits
                WNAllocationPair pair = mAllocator->AllocateForResize(sizeof(T), _count, mMaxSize);
                mMaxSize = pair.count;
                T* newData = reinterpret_cast<T*>(pair.m_pLocation);
                for(WN_SIZE_T i = 0; i < mSize; ++i) {
                    new(reinterpret_cast<void*>(newData[i])) T(std::move(mData[i]));
                    mData[i].~T(); 
                }
                mAllocator->Free(mData);
                mData = newData;
            }
        }

        void push_back(const T& val) {
            insert(mData + mSize, val);
        }
        void push_back(T&& val) {
            emplace(mData + mSize, std::move(val));
        } 

        void pop_back() {
            erase(mData + mSize - 1);
        }

        T& operator[](WN_SIZE_T i) {
            return(mData[i]);
        }
        const T& operator[](WN_SIZE_T i) const {
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
        iterator shift(const_iterator _startPt, WN_SIZE_T _count) {
            WN_DEBUG_ASSERT(_startPt <= mData + mSize && (_startPt >= mData));
            WN_SIZE_T originalPosition = _startPt - mData;
            if(mMaxSize < (mSize + _count)) {
                iterator startPt =  mData + (_startPt - mData);;
                WNAllocationPair alloc = mAllocator->AllocateForResize(sizeof(T), mSize + _count, mMaxSize);
                mMaxSize = alloc.count;
                T* newData = reinterpret_cast<T*>(alloc.m_pLocation);
                T* allocated = newData;
                for(T* i = mData; i < startPt; ++i) {
                    new(reinterpret_cast<void*>(newData++)) T(std::move(*i));
                    i->~T();
                }
                newData+=_count;
                for(T* i = startPt; i < mData + mSize; ++i) {
                    new(reinterpret_cast<void*>(newData++)) T(std::move(*i));
                    i->~T();
                }
                mAllocator->Free(mData);
                mData = allocated;
            } else {
                for(T* i = mData + mSize + _count - 1; i > (_startPt + _count - 1); --i) {
                    new(reinterpret_cast<void*>(i))T(std::move(*(i - _count - 1)));
                    (i - _count - 1)->~T();
                }
            }
            mSize = mSize + _count;
            return(mData + originalPosition);
        }

        iterator unshift(const_iterator _startPt, WN_SIZE_T _count) {
            WN_DEBUG_ASSERT(_startPt <= mData + mSize && (_startPt >= mData));
            WN_DEBUG_ASSERT(_startPt + _count <= mData + mSize);
            iterator startPt = mData + (_startPt - mData);
            for(T* i = startPt; i < startPt + _count; ++i){
                i->~T();
            }
            for(T* i = startPt + _count; i < mData + mSize; ++i) {
                new(reinterpret_cast<void*>(i - _count)) T(std::move(*i));
                i->~T();
            }
            mSize = mSize - _count;
            return(startPt);
        }

        template<typename T1, typename T2>
        friend class WNDynamicArray;

        static T_Allocator sAllocator;
        WNAllocator* mAllocator;
        T* mData;
        WN_SIZE_T mMaxSize;
        WN_SIZE_T mSize;
    };

    template<typename T, typename T_Alloc> T_Alloc WNDynamicArray<T, T_Alloc>::sAllocator;
};

#endif
