// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNNetworking/inc/Internal/Linux/WNConnectionLinux.h"
#include "WNMath/inc/WNBasic.h"
#include "WNConcurrency/inc/WNLockGuard.h"
#include "WNConcurrency/inc/WNSpinLock.h"
#include "WNConcurrency/inc/WNAtomic.h"
#include "WNCore/inc/WNEndian.h"

#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

using namespace WNNetworking;
using namespace WNContainers;
using namespace WNMath;
using namespace WNMemory;
using namespace WNConcurrency;

WNConnectionLinux::WNConnectionLinux(WNNetworkManager& _manager) :
    WNConnection(),
    mManager(_manager),
    mReadyToWrite(wn_false),
    mSockFD(-1),
    mBufferChunkCount(0),
    mBufferWritten(0),
    mTotalSent(0),
    mCurrentReadBuffer(_manager),
    mInProcessedBytes(0),
    mReadHead(0),
    mOverflowAmount(0),
    mBufferBase(0) {
    mReadLocation = make_intrusive<WNBufferResource, WNNetworkManager&>(_manager);
}

WNConnectionLinux::~WNConnectionLinux() {
    if (mSockFD >= 0) {
        close(mSockFD);

        mSockFD = -1;
    }
}

wn_void WNConnectionLinux::Invalidate() {
    WNConnection::Invalidate();

    if (mSockFD >= 0) {
        close(mSockFD);

        mSockFD = -1;
    }
}

wn_int32 WNConnectionLinux::GetLinuxSocket() {
    return(mSockFD);
}

wn_void WNConnectionLinux::SendBuffer(WNNetworkWriteBuffer& _buffer) {
    _buffer.FlushWrite();
    mSendBufferLock.Lock();
    mSendBuffers.push_back(_buffer);
    mSendBufferLock.Unlock();

    NotifyReadyToSend(wn_false);
}

wn_void WNConnectionLinux::NotifyReadyToSend(wn_bool socketFree) {
    if (socketFree) {
        WNAtomicSwap(&mWriteAtomic, 1);
    }

    while (WNAtomicCompareSwap(&mWriteAtomic, 1, 1) == 1) {
        if (!mWriteLock.TryLock()) {
            return;
        }

        WNAtomicSwap(&mWriteAtomic, 0);

        if (!Send()) {
            WNAtomicSwap(&mWriteAtomic, 1);

            mWriteLock.Unlock();

            return;
        } else {
            mWriteLock.Unlock();

            break;
        }
    }
}

wn_bool WNConnectionLinux::Send() {
    {
        WNLockGuard<WNSpinLock> guard(mSendBufferLock);

        if (mSendBuffers.empty()) {
            return(wn_false);
        }
    }

    for(;;) {
        wn_size_t count;

        mSendBufferLock.Lock();

        WNNetworkWriteBuffer& buff = mSendBuffers.front();

        mSendBufferLock.Unlock();

        const WNNetworkWriteBuffer::WNBufferQueue& q = buff.GetChunks();

        WN_RELEASE_ASSERT(q.size() > 0);
        wn_size_t buffLeft = q.size() - mBufferChunkCount;
        wn_size_t writeLeft =  q[mBufferChunkCount]->GetWritten() - mBufferWritten;
        WN_RELEASE_ASSERT(writeLeft > 0);
        do{
            wn_int32 s = write(mSockFD, q[mBufferChunkCount]->GetBaseLocation() + mBufferWritten, writeLeft);
            if(s > 0) {
                mBufferWritten += s;
                writeLeft -= s;
                mTotalSent += s;
            } else if(s == -1) {
                if(errno == EAGAIN) {
                    return(wn_true);
                }
                char* s = strerror(errno);
                perror(NULL);
                mManager.DestroyConnection(this);
                return(wn_true);
            } else {
                char* s = strerror(errno);
                perror(NULL);
                // SOME REALLY BAD ERROR
                WN_RELEASE_ASSERT_DESC(wn_false, "WTF?");
            }
            if(writeLeft == 0) {
                buffLeft -= 1;
                if(buffLeft == 0) {
                    wn_bool empty = wn_false;
                    mSendBufferLock.Lock();
                    mSendBuffers.pop_front();
                    empty = mSendBuffers.empty();
                    mSendBufferLock.Unlock();
                    mBufferWritten = 0;
                    if(mSendBuffers.empty()){
                        return(wn_false);
                    }
                    break;
                } else {
                   mBufferChunkCount += 1;
                   mBufferWritten = 0;
                   writeLeft = q[mBufferChunkCount]->GetWritten();
                }
            }
       } while(wn_true);
   }
   return(wn_true);
}

wn_void WNConnectionLinux::NotifyReadReady() {
    WNAtomicSwap(&mReadAtomic, 1);

    while(WNAtomicCompareSwap(&mReadAtomic, 1, 1) == 1) {
        if(!mReadLock.TryLock()) {
            return;
        }
        WNAtomicSwap(&mReadAtomic, 0);
        ReadReady();
        mReadLock.Unlock();
    }
}

wn_void WNConnectionLinux::ReadReady() {
   wn_size_t processedBytes = 0;

   while(1){
        wn_uint32 transferred = read(mSockFD, mReadLocation->GetPointer(), mReadLocation->GetSize() - mReadLocation->GetWritten());
        if(transferred == -1){
            if(errno != EAGAIN) {
                char* s = strerror(errno);
                perror(NULL);
                mManager.DestroyConnection(this);
                return;
            }
            break;
        } else if (transferred == 0) {
            mManager.DestroyConnection(this);
            return;
        }

        wn_size_t processedBytes = 0;
        WN_RELEASE_ASSERT(mReadHead <= MAX_DATA_WRITE);
        while(processedBytes != transferred) {
            WN_RELEASE_ASSERT(processedBytes < transferred);
            wn_size_t transferToOverflow = wn::min<wn_size_t>(8 - mOverflowAmount, transferred);
            memcpy(mOverflowLocation + mOverflowAmount, (mReadLocation->GetBaseLocation()) + mReadHead, transferToOverflow);
            mOverflowAmount += transferToOverflow;
            processedBytes += transferToOverflow;
            mInProcessedBytes += transferToOverflow;
            mBufferBase += transferToOverflow;
            mReadLocation->AddData(transferToOverflow);

            if(mOverflowAmount < 8) {
                mReadHead += transferToOverflow;
                WN_RELEASE_ASSERT(processedBytes == transferred);
                if(mBufferBase == MAX_DATA_WRITE) {
                    mReadLocation = wn::memory::make_intrusive<WNBufferResource, WNNetworkManager&>(mManager);
                    mReadHead = 0;
                    mBufferBase = 0;
                }
                break;
            }

            wn_uint32 num = *reinterpret_cast<wn_uint32*>(mOverflowLocation);
            wn_uint32 callback = *reinterpret_cast<wn_uint32*>(mOverflowLocation + 4);

            WNCore::WNFromBigEndian(callback);
            WNCore::WNFromBigEndian(num);

            wn_size_t mMaxWrite = wn::min<wn_size_t>(transferred, (num - mInProcessedBytes));

            mReadHead += mMaxWrite;
            processedBytes += mMaxWrite;

            if(mInProcessedBytes + mMaxWrite == num) {
                //We got all the data we needed
                mCurrentReadBuffer.AppendBuffer(mReadLocation, mReadHead, mBufferBase);
                mManager.FireCallback(callback, this, mCurrentReadBuffer);
                mCurrentReadBuffer.Clear();
                mReadHead += transferToOverflow;
                mBufferBase = mReadHead;
                mInProcessedBytes = 0;
                mOverflowAmount = 0;
            } else {
                mInProcessedBytes += mMaxWrite;
                if(mReadHead == MAX_DATA_WRITE) {
                    mCurrentReadBuffer.AppendBuffer(mReadLocation, mReadHead, mBufferBase);
                }
            }
            if(mReadHead == MAX_DATA_WRITE) {
                mReadLocation = make_intrusive<WNBufferResource, WNNetworkManager&>(mManager);
                mReadHead = 0;
                mBufferBase = 0;
            } else {
                mReadLocation->AddData(mMaxWrite);
            }
        }
    }
}
