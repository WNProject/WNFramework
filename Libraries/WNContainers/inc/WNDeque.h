// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

//#pragma once

#ifndef __WN_DEQUE_H__
#define __WN_DEQUE_H__
#include "WNCore/inc/WNTypes.h"
#include "WNContainers/inc/WNAllocator.h"
#include "WNContainers/inc/WNDynamicArray.h"
#include "WNMath/inc/WNBasic.h"
#include "WNMemory/inc/WNManipulation.h"

namespace WNContainers {
    template<typename T, WN_SIZE_T BlockSize = 10, typename T_Allocator=WNDefaultAllocator>
    class WNDeque {
    public:
        typedef WNDeque<T, BlockSize, T_Allocator> thisType;

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
            WN_SIGNED_T operator-(const iter& _other) {
                return(mElement - _other.mElement);
            }

            iter& operator+=(WN_SIZE_T _num) {
                mElement += _num;
                return(*this);
            }

            iter& operator-=(WN_SIZE_T _num) {
                mElement -= _num;
                return(*this);
            }

            iter operator+(WN_SIZE_T _num) {
                iter it(*this);
                return(it += _num);
            }
            iter operator-(WN_SIZE_T _num) {
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

            template<typename T1, WN_SIZE_T, typename T2>
            friend class WNDeque;
            template<typename T1>
            iter(const iter<T1>& _other): mDeque(_other.mDeque), mElement(_other.mElement) {
            }
            
            iter(thisType* _deq, WN_SIZE_T _element):
                mDeque(_deq), mElement(_element) {
            }
            thisType* mDeque;
            WN_SIZE_T mElement;
        };
        typedef iter<T> iterator;
        typedef iter<const T> const_iterator;

        WNDeque(WNAllocator* _allocator = &sAllocator): 
            mAllocator(_allocator),
            mUsedBlocks(0),
            mStartBlock(0),
            mStartLocation(0),
            mAllocatedBlocks(0),
            mNumElements(0){            
        }
        WNDeque(WN_SIZE_T _initialSize, const T& _initialValue, WNAllocator* _allocator = &sAllocator) :
            mAllocator(_allocator),
            mUsedBlocks(0),
            mStartBlock(0),
            mStartLocation(0),
            mAllocatedBlocks(0),
            mNumElements(0) {
            insert(cbegin(), _initialSize, _initialValue);
        }

        ~WNDeque() {
            clear();
            for(WN_SIZE_T i = 0; i < mAllocatedBlocks; ++i) {
                mAllocator->Free(mBlockList[(mStartBlock + i) % mBlockList.size()]);
            }
        }

        WN_SIZE_T size() {
            return(mNumElements);
        }

        WN_SIZE_T capacity() {
            return(mAllocatedBlocks * BlockSize);
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

        T& operator[](WN_SIZE_T elem) {
            WN_SIZE_T pos = mStartLocation + elem;
            WN_SIZE_T block = pos / BlockSize;
            block = (block + mStartBlock) % mBlockList.size();
            pos %= BlockSize;
            return(mBlockList[block][pos]);
        }
        
        template<typename T_It>
        T_It erase(T_It _location, T_It _endIt) {
            WN_SIZE_T numElements = (_endIt - _location);
            return(erase(_location, numElements));
        }

        template<typename T_It>
        T_It erase(T_It _location) {
            return(erase(_location, 1));
        }

        template<typename T_It>
        T_It erase(T_It _location, WN_SIZE_T _num) {
            T_It erase_start = _location;
            for(WN_SIZE_T i = 0; i < _num; ++i) {
                (*erase_start).~T();
            }

            if(closerToFront(_location)) {
                T_It copy_to = _location + _num - 1;
                T_It copy_from = _location - 1;
                WN_SIZE_T copy_count = _location - tBegin<T_It>();
                for(WN_SIZE_T i = 0; i < copy_count; ++i) {
                    new(&(*copy_to))T(*copy_from);
                    (*copy_from).~T();
                    --copy_to;
                    --copy_from;
                }
                erase_from_beginning(_num);
                return(copy_to);
            } else {
                T_It copy_to = _location;
                T_It copy_from = _location + _num;
                WN_SIZE_T location = (_location - tBegin<T_It>());
                WN_SIZE_T copy_count = mNumElements - location;
                for(WN_SIZE_T i = 0; i < copy_count; ++i) {
                    new(&(*copy_to))T(*copy_from);
                    (*copy_from).~T();
                    ++copy_to;
                    ++copy_from;
                }
                erase_from_end(_num);
                return(copy_from);
            }
        }
        
        void push_back(const T& _value) {
            insert(begin() + mNumElements);
        }
        
        void push_front(const T& _value) {
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
        iterator generate(const_iterator _position, WN_SIZE_T _n, const T_Gen& _gen) {
            iterator position = generate_gap(_position, _n);
            iterator newposition = position;
            WN_SIZE_T i = 0;
            while(_n--) {
                new(&(*(position++))) T(_gen(i++));
            }
            return(newposition);
        }

        template<typename T_It>
        iterator insert(T_It _location, const T& _value) {
            return(insert(_location, 1, _value));
        }

        template<typename T_It>
        iterator insert(T_It _location, WN_SIZE_T _num, const T& _value) {
            iterator iter = generate_gap(_location, _num);
            iterator newIter = iter;
            for(WN_SIZE_T i = 0; i < _num; ++i) {
                new(&(*(newIter++)))T(_value);
            }
            return(iter);
        }
    private:
        template<typename T_It>
        iterator generate_gap(T_It _location, WN_SIZE_T _num) {
            if(closerToFront(_location)) {
                WN_SIZE_T location = (_location - tBegin<T_It>());
                allocate_elements_at_begin(_num);
                iterator copy_to = begin();
                iterator copy_from = begin() + _num;
                for(WN_SIZE_T i = 0;  i < location; ++i) {
                    new(&(*copy_to))T(*copy_from);
                    (*copy_from).~T();
                    ++copy_to;
                    ++copy_from;
                }
                return(copy_to);
            } else {
                WN_SIZE_T location = (_location - tBegin<T_It>());
                WN_SIZE_T elements_to_end = mNumElements - location;
                allocate_elements_at_end(_num);
                iterator copy_to = end() - 1;
                iterator copy_from = begin() + location + elements_to_end - 1;
                    
                for(WN_SIZE_T i = 0; i < elements_to_end; ++i) {
                    new (&(*copy_to))T(*copy_from);
                    (*copy_from).~T();
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
        
        WN_VOID erase_from_beginning(WN_SIZE_T _elements) {
            WN_SIZE_T elements_in_first_block = mStartLocation;
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
                    elements_in_first_block = WNMath::WNMin(BlockSize, mNumElements);
                }
                mNumElements -= _elements;
                mStartLocation += _elements;
            } else {
                mNumElements -= _elements;
                mStartLocation += _elements;
            }
        }

        WN_VOID erase_from_end(WN_SIZE_T _elements) {
            WN_SIZE_T elements_in_last_block = (mStartLocation + mNumElements) % BlockSize;

            if(elements_in_last_block < _elements) {
                while(_elements > elements_in_last_block && elements_in_last_block > 0) {
                    _elements -= elements_in_last_block;
                    mUsedBlocks -= 1;
                    mNumElements -= elements_in_last_block;
                    elements_in_last_block = WNMath::WNMin(BlockSize, mNumElements);
                }
                mNumElements -= _elements;
            } else {
                mNumElements -= _elements;
            }
        }

        WN_VOID allocate_elements_at_end(WN_SIZE_T _elements) {
            WN_SIZE_T unused_blocks = total_unused_blocks();
            WN_SIZE_T extra_last_elements = total_extra_elements_in_last_block();
            WN_SIZE_T totalElements = (extra_last_elements + unused_blocks * BlockSize);
            if(totalElements > _elements) {
                WN_SIZE_T extraElements = _elements;
                extraElements -= WNMath::WNMin(extra_last_elements, extraElements);
                WN_SIZE_T extra_used_blocks = ((extraElements  + BlockSize - 1)/ BlockSize);
                mUsedBlocks += extra_used_blocks;
            } else {
                //we need more blocks;
                WN_SIZE_T neededExtraBlocks = (_elements - totalElements + BlockSize - 1) / BlockSize;
                if(mAllocatedBlocks + neededExtraBlocks > mBlockList.size()) {
                    add_block_space(mAllocatedBlocks + neededExtraBlocks - mBlockList.size());
                }
                for(WN_SIZE_T i = 0; i < neededExtraBlocks; ++i) {
                    WNAllocationPair p = mAllocator->Allocate(sizeof(T*), BlockSize);
                    mBlockList[(mStartBlock + mAllocatedBlocks) % mBlockList.size()] = static_cast<T*>(p.m_pLocation);
                    mAllocatedBlocks += 1;
                }
                mUsedBlocks = mAllocatedBlocks;
            }
            mNumElements += _elements;
        }

        WN_VOID allocate_elements_at_begin(WN_SIZE_T _elements) {
            WN_SIZE_T leftovers_in_block = mStartLocation;
            if(leftovers_in_block < _elements) {
                WN_SIZE_T accountedElements = leftovers_in_block;
                WN_SIZE_T additional_blocks = (_elements - accountedElements);
                additional_blocks = (additional_blocks + BlockSize) / BlockSize;
                WN_SIZE_T haveBlocks = total_unused_blocks();
                if(haveBlocks < additional_blocks) {
                    //We need at least some more blocks
                    WN_SIZE_T neededBlocks = additional_blocks - haveBlocks;
                    if(mBlockList.size() - mAllocatedBlocks < neededBlocks) {
                        add_block_space(mAllocatedBlocks + neededBlocks - mBlockList.size());
                    }
                    for(WN_SIZE_T i = 0; i < neededBlocks; ++i) {
                        WNAllocationPair p = mAllocator->Allocate(sizeof(T*), BlockSize);
                        mBlockList[(mStartBlock + mAllocatedBlocks) % mBlockList.size()] = static_cast<T*>(p.m_pLocation);
                        mAllocatedBlocks += 1;
                    }
                }
                
                for(WN_SIZE_T i = 0; i < additional_blocks; ++i) {
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
                    accountedElements += BlockSize;
                }
                mStartLocation = accountedElements - _elements;
            } else {
                mStartLocation -= WNMath::WNMin(leftovers_in_block, _elements);
            }
            mNumElements += _elements;
        }


        WN_VOID add_block_space(WN_SIZE_T _numBlocks){ 
            WN_SIZE_T oldLength = mBlockList.size();
            mBlockList.insert(mBlockList.end(), _numBlocks, NULL);
            WN_SIZE_T totalBlocks = mBlockList.capacity();
            mBlockList.insert(mBlockList.end(), mBlockList.capacity() - mBlockList.size(), NULL);
            if(0 != mStartBlock) {
                WN_SIZE_T mAddedSize = mBlockList.size() - oldLength;
                WN_SIZE_T mCopySize = (oldLength - mStartBlock);
                WNMemory::WNMemMove(&mBlockList[mStartBlock + mAddedSize], &mBlockList[mStartBlock], mCopySize * sizeof(T*));
#ifdef _WN_DEBUG
                WNMemory::WNMemClr(&mBlockList[mStartBlock], sizeof(T*) * mAddedSize);
#endif
                mStartBlock += mAddedSize;
            }
            
        }

        WN_SIZE_T total_unused_blocks() {
            return(mAllocatedBlocks - mUsedBlocks);
        }
        WN_SIZE_T total_extra_elements_in_last_block() {
            WN_SIZE_T num = BlockSize - (mStartLocation + mNumElements) % BlockSize;
            return((num == BlockSize)? 0 : num);
        }

        WNDynamicArray<T*> mBlockList;
        WN_SIZE_T mUsedBlocks;
        WN_SIZE_T mAllocatedBlocks;
        WN_SIZE_T mStartLocation;
        WN_SIZE_T mNumElements;
        WN_SIZE_T mStartBlock;
        WNAllocator* mAllocator;
        static T_Allocator sAllocator;
    };

    template<typename T, WN_SIZE_T BlockSize, typename T_Alloc> T_Alloc WNDeque<T, BlockSize, T_Alloc>::sAllocator;
};

#endif
