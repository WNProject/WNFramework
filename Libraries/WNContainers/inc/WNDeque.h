// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

//#pragma once

#ifndef __WN_CONTAINERS_DEQUE_H__
#define __WN_CONTAINERS_DEQUE_H__

#include "WNCore/inc/WNTypes.h"
#include "WNMemory/inc/WNAllocator.h"
#include "WNContainers/inc/WNDynamicArray.h"
#include "WNMemory/inc/WNManipulation.h"
#include "WNMath/inc/WNBasic.h"

namespace WNContainers {
    template <typename type, wn_size_t block_size = 10, typename allocator_type = wn::memory::default_allocator>
    class deque {
    public:
        typedef deque<type, block_size, allocator_type> self_type;

        template<typename T_It>
        class iter {
        public:
            iter(): mDeque(NULL), mElement(0) {
            }

            iter(const iter& _other): mDeque(_other.mDeque), mElement(_other.mElement) {
            }

            iter& operator=(const iter& _other) {
                mDeque = _other.mDeque;
                mElement = _other.mElement;
            }

            wn_signed_t operator-(const iter& _other) {
                return(mElement - _other.mElement);
            }

            iter& operator+=(wn_size_t _num) {
                mElement += _num;
                return(*this);
            }

            iter& operator-=(wn_size_t _num) {
                mElement -= _num;
                return(*this);
            }

            iter operator+(wn_size_t _num) {
                iter it(*this);
                return(it += _num);
            }
            iter operator-(wn_size_t _num) {
               iter it(*this);
               return(it -= _num);
            }

            T_It& operator*(){
                return(mDeque->operator[](mElement));
            }
            T_It* operator->() {
                return(&(mDeque->operator[](mElement)));
            }

            iter operator++(int) {
                iter it(*this);
                (*this) += 1;
                return(it);
            }
            iter operator--(int) {
                iter it(*this);
                (*this) -= 1;
                return(it);
            }
            iter& operator++() {
                return((*this) += 1);
            }
            iter& operator--() {
                return((*this) -= 1);
            }
        private:

            template<typename T1, wn_size_t, typename T2>
            friend class deque;
            template<typename T1>
            iter(const iter<T1>& _other): mDeque(_other.mDeque), mElement(_other.mElement) {
            }

            iter(self_type* _deq, wn_size_t _element):
                mDeque(_deq), mElement(_element) {
            }
            self_type* mDeque;
            wn_size_t mElement;
        };
        typedef iter<type> iterator;
        typedef iter<const type> const_iterator;

        deque(wn::memory::allocator* _allocator = &sAllocator) :
            mAllocator(_allocator),
            mUsedBlocks(0),
            mStartBlock(0),
            mStartLocation(0),
            mAllocatedBlocks(0),
            mNumElements(0){
        }
        deque(wn_size_t _initialSize, const type& _initialValue, wn::memory::allocator* _allocator = &sAllocator) :
            mAllocator(_allocator),
            mUsedBlocks(0),
            mStartBlock(0),
            mStartLocation(0),
            mAllocatedBlocks(0),
            mNumElements(0) {
            insert(cbegin(), _initialSize, _initialValue);
        }

        ~deque() {
            clear();
            for(wn_size_t i = 0; i < mAllocatedBlocks; ++i) {
                mAllocator->deallocate(mBlockList[(mStartBlock + i) % mBlockList.size()]);
            }
        }

        wn_size_t size() {
            return(mNumElements);
        }

        wn_size_t capacity() {
            return(mAllocatedBlocks * block_size);
        }

        iterator begin() {
            return(tBegin<iterator>());
        }
        const_iterator cbegin() {
            return(tBegin<const_iterator>());
        }
        iterator end() {
            return(tEnd<iterator>());
        }
        const_iterator cend() {
            return(tEnd<const_iterator>());
        }

        type& operator[](wn_size_t elem) {
            wn_size_t pos = mStartLocation + elem;
            wn_size_t block = pos / block_size;
            block = (block + mStartBlock) % mBlockList.size();
            pos %= block_size;
            return(mBlockList[block][pos]);
        }

        template<typename T_It>
        T_It erase(T_It _location, T_It _endIt) {
            wn_size_t numElements = (_endIt - _location);
            return(erase(_location, numElements));
        }

        template<typename T_It>
        T_It erase(T_It _location) {
            return(erase(_location, 1));
        }

        template<typename T_It>
        T_It erase(T_It _location, wn_size_t _num) {
            T_It erase_start = _location;
            for(wn_size_t i = 0; i < _num; ++i) {
                (*erase_start).~type();
            }

            if(closerToFront(_location)) {
                T_It copy_to = _location + _num - 1;
                T_It copy_from = _location - 1;
                wn_size_t copy_count = _location - tBegin<T_It>();
                for(wn_size_t i = 0; i < copy_count; ++i) {
                    new(&(*copy_to))type(*copy_from);
                    (*copy_from).~type();
                    --copy_to;
                    --copy_from;
                }
                erase_from_beginning(_num);
                return(copy_to);
            } else {
                T_It copy_to = _location;
                T_It copy_from = _location + _num;
                wn_size_t location = (_location - tBegin<T_It>());
                wn_size_t copy_count = mNumElements - location;
                for(wn_size_t i = 0; i < copy_count; ++i) {
                    new(&(*copy_to))type(*copy_from);
                    (*copy_from).~type();
                    ++copy_to;
                    ++copy_from;
                }
                erase_from_end(_num);
                return(copy_from);
            }
        }

        void push_back(const type& _value) {
            insert(begin() + mNumElements);
        }

        void push_front(const type& _value) {
            insert(cbegin(), 1, _value);
        }

        void pop_front() {
            erase(begin(), 1);
        }

        void pop_back() {
            erase(end() - 1, 1);
        }

        void clear() {
            erase(begin(), end());
        }

        template <typename T_Gen>
        iterator generate(const_iterator _position, wn_size_t _n, const T_Gen& _gen) {
            iterator position = generate_gap(_position, _n);
            iterator newposition = position;
            wn_size_t i = 0;
            while(_n--) {
                new(&(*(position++))) type(_gen(i++));
            }
            return(newposition);
        }

        template<typename T_It>
        iterator insert(T_It _location, const type& _value) {
            return(insert(_location, 1, _value));
        }

        template<typename T_It>
        iterator insert(T_It _location, wn_size_t _num, const type& _value) {
            iterator iter = generate_gap(_location, _num);
            iterator newIter = iter;
            for(wn_size_t i = 0; i < _num; ++i) {
                new(&(*(newIter++)))type(_value);
            }
            return(iter);
        }
    private:
        template<typename T_It>
        iterator generate_gap(T_It _location, wn_size_t _num) {
            if(closerToFront(_location)) {
                wn_size_t location = (_location - tBegin<T_It>());
                allocate_elements_at_begin(_num);
                iterator copy_to = begin();
                iterator copy_from = begin() + _num;
                for(wn_size_t i = 0;  i < location; ++i) {
                    new(&(*copy_to))type(*copy_from);
                    (*copy_from).~type();
                    ++copy_to;
                    ++copy_from;
                }
                return(copy_to);
            } else {
                wn_size_t location = (_location - tBegin<T_It>());
                wn_size_t elements_to_end = mNumElements - location;
                allocate_elements_at_end(_num);
                iterator copy_to = end() - 1;
                iterator copy_from = begin() + location + elements_to_end - 1;

                for(wn_size_t i = 0; i < elements_to_end; ++i) {
                    new (&(*copy_to))type(*copy_from);
                    (*copy_from).~type();
                    --copy_to;
                    --copy_from;
                }
                return(tBegin<T_It>() + location);
            }
        }

        template<typename T_It>
        T_It tBegin() {
            return(T_It(this, 0));
        }

        template<typename T_It>
        T_It tEnd() {
            return(T_It(this, mNumElements));
        }

        template<typename T_It>
        bool closerToFront(T_It _iter) {
            return(_iter - tBegin<T_It>() < tEnd<T_It>() - _iter);
        }

        wn_void erase_from_beginning(wn_size_t _elements) {
            wn_size_t elements_in_first_block = mStartLocation;
            if(elements_in_first_block < _elements) {
                while(_elements > elements_in_first_block && elements_in_first_block > 0) {
                    _elements -= elements_in_first_block;
                    if(mBlockList.size() != mAllocatedBlocks) {
                        mBlockList[(mStartBlock + mAllocatedBlocks + 1) % mBlockList.size()] = mBlockList[mStartBlock];
                        mBlockList[mStartBlock] = NULL;
                    }
                    mStartLocation = 0;
                    mStartBlock += 1;
                    mUsedBlocks -= 1;
                    mNumElements -= elements_in_first_block;
                    elements_in_first_block = wn::min(block_size, mNumElements);
                }
                mNumElements -= _elements;
                mStartLocation += _elements;
            } else {
                mNumElements -= _elements;
                mStartLocation += _elements;
            }
        }

        wn_void erase_from_end(wn_size_t _elements) {
            wn_size_t elements_in_last_block = (mStartLocation + mNumElements) % block_size;

            if(elements_in_last_block < _elements) {
                while(_elements > elements_in_last_block && elements_in_last_block > 0) {
                    _elements -= elements_in_last_block;
                    mUsedBlocks -= 1;
                    mNumElements -= elements_in_last_block;
                    elements_in_last_block = wn::min(block_size, mNumElements);
                }
                mNumElements -= _elements;
            } else {
                mNumElements -= _elements;
            }
        }

        wn_void allocate_elements_at_end(wn_size_t _elements) {
            wn_size_t unused_blocks = total_unused_blocks();
            wn_size_t extra_last_elements = total_extra_elements_in_last_block();
            wn_size_t totalElements = (extra_last_elements + unused_blocks * block_size);
            if(totalElements > _elements) {
                wn_size_t extraElements = _elements;
                extraElements -= wn::min(extra_last_elements, extraElements);
                wn_size_t extra_used_blocks = ((extraElements  + block_size - 1)/ block_size);
                mUsedBlocks += extra_used_blocks;
            } else {
                //we need more blocks;
                wn_size_t neededExtraBlocks = (_elements - totalElements + block_size - 1) / block_size;
                if(mAllocatedBlocks + neededExtraBlocks > mBlockList.size()) {
                    add_block_space(mAllocatedBlocks + neededExtraBlocks - mBlockList.size());
                }
                for(wn_size_t i = 0; i < neededExtraBlocks; ++i) {
                    wn::memory::allocation_pair p = mAllocator->allocate(sizeof(type*), block_size);
                    mBlockList[(mStartBlock + mAllocatedBlocks) % mBlockList.size()] = static_cast<type*>(p.m_location);
                    mAllocatedBlocks += 1;
                }
                mUsedBlocks = mAllocatedBlocks;
            }
            mNumElements += _elements;
        }

        wn_void allocate_elements_at_begin(wn_size_t _elements) {
            wn_size_t leftovers_in_block = mStartLocation;
            if(leftovers_in_block < _elements) {
                wn_size_t accountedElements = leftovers_in_block;
                wn_size_t additional_blocks = (_elements - accountedElements);
                additional_blocks = (additional_blocks + block_size) / block_size;
                wn_size_t haveBlocks = total_unused_blocks();
                if(haveBlocks < additional_blocks) {
                    //We need at least some more blocks
                    wn_size_t neededBlocks = additional_blocks - haveBlocks;
                    if(mBlockList.size() - mAllocatedBlocks < neededBlocks) {
                        add_block_space(mAllocatedBlocks + neededBlocks - mBlockList.size());
                    }
                    for(wn_size_t i = 0; i < neededBlocks; ++i) {
                        wn::memory::allocation_pair p = mAllocator->allocate(sizeof(type*), block_size);
                        mBlockList[(mStartBlock + mAllocatedBlocks) % mBlockList.size()] = static_cast<type*>(p.m_location);
                        mAllocatedBlocks += 1;
                    }
                }

                for(wn_size_t i = 0; i < additional_blocks; ++i) {
                    if(mStartBlock == 0) {
                        mStartBlock = mBlockList.size() - 1;
                    } else {
                        mStartBlock -= 1;
                    }
                    mUsedBlocks += 1;
                    if(mBlockList.size() != mAllocatedBlocks) {
                        mBlockList[mStartBlock] = mBlockList[(mStartBlock + mAllocatedBlocks) % mBlockList.size()];
                        mBlockList[(mStartBlock + mAllocatedBlocks) % mBlockList.size()] = NULL;
                    }
                    accountedElements += block_size;
                }
                mStartLocation = accountedElements - _elements;
            } else {
                mStartLocation -= wn::min(leftovers_in_block, _elements);
            }
            mNumElements += _elements;
        }


        wn_void add_block_space(wn_size_t _numBlocks){
            wn_size_t oldLength = mBlockList.size();
            mBlockList.insert(mBlockList.end(), _numBlocks, NULL);
            wn_size_t totalBlocks = mBlockList.capacity();
            mBlockList.insert(mBlockList.end(), mBlockList.capacity() - mBlockList.size(), NULL);
            if(0 != mStartBlock) {
                wn_size_t mAddedSize = mBlockList.size() - oldLength;
                wn_size_t mCopySize = (oldLength - mStartBlock);
                wn::memory::memmove(&mBlockList[mStartBlock + mAddedSize], &mBlockList[mStartBlock], mCopySize);
                #ifdef _WN_DEBUG
                    wn::memory::memzero(&mBlockList[mStartBlock], mAddedSize);
                #endif
                mStartBlock += mAddedSize;
            }

        }

        wn_size_t total_unused_blocks() {
            return(mAllocatedBlocks - mUsedBlocks);
        }
        wn_size_t total_extra_elements_in_last_block() {
            wn_size_t num = block_size - (mStartLocation + mNumElements) % block_size;
            return((num == block_size)? 0 : num);
        }

        dynamic_array<type*> mBlockList;
        wn_size_t mUsedBlocks;
        wn_size_t mAllocatedBlocks;
        wn_size_t mStartLocation;
        wn_size_t mNumElements;
        wn_size_t mStartBlock;
        wn::memory::allocator* mAllocator;
        static allocator_type sAllocator;
    };

    template<typename type, wn_size_t block_size, typename T_Alloc> T_Alloc deque<type, block_size, T_Alloc>::sAllocator;
};

#endif
