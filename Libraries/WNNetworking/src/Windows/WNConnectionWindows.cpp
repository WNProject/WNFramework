// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNNetworking/inc/Internal/Windows/WNConnectionWindows.h"
#include "WNNetworking/inc/Internal/Windows/WNNetworkManagerWindows.h"
#include "WNMemory/inc/WNManipulation.h"
#include "WNMath/inc/WNBasic.h"
#include "WNCore/inc/WNEndian.h"
#include "WNConcurrency/inc/WNLockGuard.h"

using namespace WNNetworking;

WNConnectionWindows::WNConnectionWindows(WNNetworkManager& _manager) :
    WNConnection(),
    mSocket(0),
    mInProcessedBytes(0),
    mReadHead(0),
    mOverflowAmount(0),
    mBufferBase(0),
    mCurrentReadBuffer(_manager),
    mManager(_manager) {
    WNMemory::WNMemClrT(&mReceiveOverlap);
    WNMemory::WNMemClrT(&mSendOverlap);
    mReadLocation = WNConcurrency::WNAllocateResource<WNBufferResource, WNNetworkManager&>(_manager);
}

WNConnectionWindows::~WNConnectionWindows() {
    if (mSocket != 0) {
        closesocket(mSocket);

        mSocket = 0;
    }
}

WN_VOID WNConnectionWindows::Invalidate() {
    WNConnection::Invalidate();

    if (mSocket != 0) {
        closesocket(mSocket);

        mSocket = 0;
    }
}

SOCKET WNConnectionWindows::GetWindowsSocket() {
    return(mSocket);
} 

WN_BOOL WNConnectionWindows::Receive() {
    DWORD bytes;
    DWORD flags = 0;

    mReadLocation->PrepareRead();

    int received = WSARecv(mSocket, mReadLocation->GetReadWinBuf(), 1, &bytes, &flags, &mReceiveOverlap, NULL);
    int lastError = WSAGetLastError();

    if (SOCKET_ERROR == received && (WSA_IO_PENDING != lastError)) {
        return(WN_FALSE);
    }

    return(WN_TRUE);
}

WNConnectionWindows::eWNNetworkOperation WNConnectionWindows::GetOperation(LPOVERLAPPED _overlapped) {
    if (_overlapped == &mReceiveOverlap) {
        return(eWNReadOp);
    } else if (_overlapped == &mSendOverlap) {
        return(eWNWriteOp);
    } else {
        return(eWNNoOp);
    }
}

WN_BOOL WNConnectionWindows::ProcessRead(WNNetworkManagerWindows* _windowsManager, DWORD _bytesTransferred) {
    WN_SIZE_T processedBytes = 0;
    WN_RELEASE_ASSERT(mReadHead <= WNContainers::MAX_DATA_WRITE);
    while(processedBytes != _bytesTransferred) {
        WN_RELEASE_ASSERT(processedBytes < _bytesTransferred);
        WN_SIZE_T transferToOverflow = WNMath::WNMin<WN_SIZE_T>(8 - mOverflowAmount, _bytesTransferred);
        WNMemory::WNMemCpy(mOverflowLocation + mOverflowAmount, (mReadLocation->GetBaseLocation()) + mReadHead, transferToOverflow);
        mOverflowAmount += transferToOverflow;
        processedBytes += transferToOverflow;
        mInProcessedBytes += transferToOverflow;
        mBufferBase += transferToOverflow;
        mReadLocation->AddData(transferToOverflow);

        if(mOverflowAmount < 8) {
            mReadHead += transferToOverflow;
            WN_RELEASE_ASSERT(processedBytes == _bytesTransferred);
            if(mBufferBase == WNContainers::MAX_DATA_WRITE) {
                mReadLocation = WNConcurrency::WNAllocateResource<WNBufferResource, WNNetworkManager&>(mManager);
                mReadHead = 0;
                mBufferBase = 0;
            }
            break;
        }
        
        WN_UINT32 num = *reinterpret_cast<WN_UINT32*>(mOverflowLocation);
        WN_UINT32 callback = *reinterpret_cast<WN_UINT32*>(mOverflowLocation + 4);

        WNCore::WNFromBigEndian(callback);
        WNCore::WNFromBigEndian(num);

        WN_SIZE_T mMaxWrite = WNMath::WNMin<WN_SIZE_T>(_bytesTransferred, (num - mInProcessedBytes));

        mReadHead += mMaxWrite;
        processedBytes += mMaxWrite;

        if(mInProcessedBytes + mMaxWrite == num) {
            //We got all the data we needed
            mCurrentReadBuffer.AppendBuffer(mReadLocation, mReadHead, mBufferBase);
            _windowsManager->FireCallback(callback, this, mCurrentReadBuffer);
            mCurrentReadBuffer.Clear();
            mReadHead += transferToOverflow;
            mBufferBase = mReadHead;
            mInProcessedBytes = 0;
            mOverflowAmount = 0;
        } else {
            mInProcessedBytes += mMaxWrite;
            if(mReadHead == WNContainers::MAX_DATA_WRITE) {
                mCurrentReadBuffer.AppendBuffer(mReadLocation, mReadHead, mBufferBase);
            }
        }
        if(mReadHead == WNContainers::MAX_DATA_WRITE) {
            mReadLocation = WNConcurrency::WNAllocateResource<WNBufferResource, WNNetworkManager&>(mManager);
            mReadHead = 0;
            mBufferBase = 0;
        } else {
            mReadLocation->AddData(mMaxWrite);
        }
    }
    
    return(WN_TRUE);
}

WN_VOID WNConnectionWindows::AppendSendBuffer(WNNetworkWriteBuffer& _buff) {
    WNConcurrency::WNLockGuard<WNConcurrency::WNMutex> guard(mSendMutex);

    mWriteBuffers.push_back(_buff);

    if (mWriteBuffers.size() == 1) {
        Send();
    }
}

WN_VOID WNConnectionWindows::Send() {
    WNNetworkWriteBuffer& buff = mWriteBuffers.front();
    const std::vector<WNConcurrency::WNResourcePointer<WNBufferResource> >& sendBuffs = buff.GetChunks();
    mWSAWriteBuffers.clear();
    for(std::vector<WNConcurrency::WNResourcePointer<WNBufferResource> >::const_iterator i = sendBuffs.begin(); i != sendBuffs.end(); ++i) {
        mWSAWriteBuffers.push_back(*(*i)->GetWriteWinBuf());
    }
    DWORD bytes;
    int sent = WSASend(mSocket, &mWSAWriteBuffers[0], static_cast<DWORD>(sendBuffs.size()), &bytes, 0, &mSendOverlap, NULL);
    if(SOCKET_ERROR == sent && WSA_IO_PENDING != WSAGetLastError()) {
        mManager.DestroyConnection(this);
    }
}

WN_VOID WNConnectionWindows::ProcessWrite() {
    WNConcurrency::WNLockGuard<WNConcurrency::WNMutex> guard(mSendMutex);
    mWriteBuffers.pop_front();
    if(mWriteBuffers.size() > 0){
        Send();
    }
}

WN_VOID WNConnectionWindows::SendBuffer(WNNetworkWriteBuffer& _buffer) { 
    _buffer.FlushWrite();
    AppendSendBuffer(_buffer);
}