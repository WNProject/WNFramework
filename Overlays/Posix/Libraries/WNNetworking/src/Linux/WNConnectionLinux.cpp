// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNNetworking/inc/Internal/Linux/WNConnectionLinux.h"
#include "WNCore/inc/WNEndian.h"
#include "WNMath/inc/WNBasic.h"
#include "WNMultiTasking/inc/WNSpinLock.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <mutex>

using namespace WNNetworking;
using namespace WNContainers;

WNConnectionLinux::WNConnectionLinux(WNNetworkManager& _manager)
  : WNConnection(),
    mManager(_manager),
    mReadyToWrite(false),
    mSockFD(-1),
    mBufferChunkCount(0),
    mBufferWritten(0),
    mTotalSent(0),
    mCurrentReadBuffer(_manager),
    mInProcessedBytes(0),
    mReadHead(0),
    mOverflowAmount(0),
    mBufferBase(0) {
  mReadLocation =
      wn::memory::make_intrusive<WNBufferResource, WNNetworkManager&>(
          &allocator, _manager);
}

WNConnectionLinux::~WNConnectionLinux() {
  if (mSockFD >= 0) {
    close(mSockFD);

    mSockFD = -1;
  }
}

void WNConnectionLinux::Invalidate() {
  WNConnection::Invalidate();

  if (mSockFD >= 0) {
    close(mSockFD);

    mSockFD = -1;
  }
}

int32_t WNConnectionLinux::GetLinuxSocket() {
  return (mSockFD);
}

void WNConnectionLinux::SendBuffer(WNNetworkWriteBuffer& _buffer) {
  _buffer.FlushWrite();
  mSendBufferLock.lock();
  mSendBuffers.push_back(_buffer);
  mSendBufferLock.unlock();

  NotifyReadyToSend(false);
}

void WNConnectionLinux::NotifyReadyToSend(bool socketFree) {
  if (socketFree) {
    mWriteAtomic = 1;
  }

  size_t expected = 1;
  while (mWriteAtomic.compare_exchange_strong(expected, 1)) {
    if (!mWriteLock.try_lock()) {
      return;
    }

    mWriteAtomic = 0;

    if (!Send()) {
      mWriteAtomic = 1;

      mWriteLock.unlock();

      return;
    } else {
      mWriteLock.lock();

      break;
    }
  }
}

bool WNConnectionLinux::Send() {
  {
    std::lock_guard<wn::multi_tasking::spin_lock> guard(mSendBufferLock);

    if (mSendBuffers.empty()) {
      return (false);
    }
  }

  for (;;) {
    mSendBufferLock.lock();

    WNNetworkWriteBuffer& buff = mSendBuffers.front();

    mSendBufferLock.unlock();

    const WNNetworkWriteBuffer::WNBufferQueue& q = buff.GetChunks();

    WN_RELEASE_ASSERT(q.size() > 0);
    size_t buffLeft = q.size() - mBufferChunkCount;
    size_t writeLeft = q[mBufferChunkCount]->GetWritten() - mBufferWritten;
    WN_RELEASE_ASSERT(writeLeft > 0);
    do {
      int32_t s = write(mSockFD,
          q[mBufferChunkCount]->GetBaseLocation() + mBufferWritten, writeLeft);
      if (s > 0) {
        mBufferWritten += s;
        writeLeft -= s;
        mTotalSent += s;
      } else if (s == -1) {
        if (errno == EAGAIN) {
          return (true);
        }
        // char* s = strerror(errno);
        perror(NULL);
        mManager.DestroyConnection(this);
        return (true);
      } else {
        // char* s = strerror(errno);
        perror(NULL);
        // SOME REALLY BAD ERROR
        WN_RELEASE_ASSERT_DESC(false, "WTF?");
      }
      if (writeLeft == 0) {
        buffLeft -= 1;
        if (buffLeft == 0) {
          bool empty = false;
          mSendBufferLock.lock();
          mSendBuffers.pop_front();
          empty = mSendBuffers.empty();
          mSendBufferLock.unlock();
          mBufferWritten = 0;
          if (mSendBuffers.empty()) {
            return (false);
          }
          break;
        } else {
          mBufferChunkCount += 1;
          mBufferWritten = 0;
          writeLeft = q[mBufferChunkCount]->GetWritten();
        }
      }
    } while (true);
  }
  return (true);
}

void WNConnectionLinux::NotifyReadReady() {
  mReadAtomic = 1;
  size_t expected = 1;

  while (mReadAtomic.compare_exchange_strong(expected, 1) == 1) {
    if (!mReadLock.try_lock()) {
      return;
    }
    mReadAtomic = 0;
    ReadReady();
    mReadLock.unlock();
  }
}

void WNConnectionLinux::ReadReady() {
  size_t processedBytes = 0;

  while (1) {
    uint32_t transferred = read(mSockFD, mReadLocation->GetPointer(),
        mReadLocation->GetSize() - mReadLocation->GetWritten());
    if (transferred == -1) {
      if (errno != EAGAIN) {
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

    size_t processedBytes = 0;
    WN_RELEASE_ASSERT(mReadHead <= wn::containers::MAX_DATA_WRITE);
    while (processedBytes != transferred) {
      WN_RELEASE_ASSERT(processedBytes < transferred);
      size_t transferToOverflow =
          wn::min<size_t>(8 - mOverflowAmount, transferred);
      wn::memory::memcpy(mOverflowLocation + mOverflowAmount,
          (mReadLocation->GetBaseLocation()) + mReadHead, transferToOverflow);
      mOverflowAmount += transferToOverflow;
      processedBytes += transferToOverflow;
      mInProcessedBytes += transferToOverflow;
      mBufferBase += transferToOverflow;
      mReadLocation->AddData(transferToOverflow);

      if (mOverflowAmount < 8) {
        mReadHead += transferToOverflow;
        WN_RELEASE_ASSERT(processedBytes == transferred);
        if (mBufferBase == wn::containers::MAX_DATA_WRITE) {
          mReadLocation =
              wn::memory::make_intrusive<WNBufferResource, WNNetworkManager&>(
                  &allocator, mManager);
          mReadHead = 0;
          mBufferBase = 0;
        }
        break;
      }

      uint32_t num = *reinterpret_cast<uint32_t*>(mOverflowLocation);
      uint32_t callback = *reinterpret_cast<uint32_t*>(mOverflowLocation + 4);

      callback = wn::core::from_big_endian(callback);
      num = wn::core::from_big_endian(num);

      size_t mMaxWrite =
          wn::min<size_t>(transferred, (num - mInProcessedBytes));

      mReadHead += mMaxWrite;
      processedBytes += mMaxWrite;

      if (mInProcessedBytes + mMaxWrite == num) {
        // We got all the data we needed
        mCurrentReadBuffer.AppendBuffer(mReadLocation, mReadHead, mBufferBase);
        mManager.FireCallback(callback, this, mCurrentReadBuffer);
        mCurrentReadBuffer.Clear();
        mReadHead += transferToOverflow;
        mBufferBase = mReadHead;
        mInProcessedBytes = 0;
        mOverflowAmount = 0;
      } else {
        mInProcessedBytes += mMaxWrite;
        if (mReadHead == wn::containers::MAX_DATA_WRITE) {
          mCurrentReadBuffer.AppendBuffer(
              mReadLocation, mReadHead, mBufferBase);
        }
      }
      if (mReadHead == wn::containers::MAX_DATA_WRITE) {
        mReadLocation =
            wn::memory::make_intrusive<WNBufferResource, WNNetworkManager&>(
                &allocator, mManager);
        mReadHead = 0;
        mBufferBase = 0;
      } else {
        mReadLocation->AddData(mMaxWrite);
      }
    }
  }
}
