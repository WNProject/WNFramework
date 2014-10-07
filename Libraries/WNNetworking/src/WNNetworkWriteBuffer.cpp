////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "WNNetworking/inc/WNNetworkManager.h"
#include "WNNetworking/inc/WNNetworkWriteBuffer.h"
#include "WNContainers/inc/WNSerializer.h"
#include "WNCore/inc/WNEndian.h"
#include "WNConcurrency/inc/WNAtomic.h"
#include "WNMath/inc/WNBasic.h"

using namespace WNNetworking;
using namespace WNConcurrency;
using namespace WNContainers;
using namespace WNMath;

WNNetworkWriteBuffer::WNNetworkWriteBuffer(WNNetworkManager& _manager, WN_UINT32 _number) :
    mManager(_manager),
    mBufferPointer(0),
    mTotalWritten(0),
    mFlushed(WN_FALSE) {
    mChunks.push_back(WNAllocateResource<WNBufferResource, WNNetworkManager&>(mManager));
    _manager.InitializeBuffer(*this, _number);
}

WNNetworkWriteBuffer::WNNetworkWriteBuffer(const WNNetworkWriteBuffer& _other) :
    mManager(_other.mManager),
    mBufferPointer(_other.mBufferPointer),
    mTotalWritten(_other.mTotalWritten),
    mFlushed(_other.mFlushed) {
    (*this) = _other;
}

WNNetworkWriteBuffer& WNNetworkWriteBuffer::operator = (const WNNetworkWriteBuffer& _other) {
    if (&_other == this) {
        return(*this);
    }

    mFlushed = _other.mFlushed;
    mTotalWritten = _other.mTotalWritten;
    mBufferPointer = _other.mBufferPointer;

    #if defined _WN_MSVC && _WN_MSVC >= 160000000
        for (WNBufferQueue::const_iterator i = _other.GetChunks().cbegin(); i != _other.GetChunks().cend(); ++i) {
            mChunks.push_back(*i);
        }
    #else
        for (WNBufferQueue::const_iterator i = _other.GetChunks().begin(); i != _other.GetChunks().end(); ++i) {
            mChunks.push_back(*i);
        }
    #endif

    return(*this);
}

WN_BOOL WNNetworkWriteBuffer::Serialize(const WN_UINT32 _flags, const WNSerializerBase& _serializer) {
    WN_DEBUG_ASSERT(!mFlushed);

    const WN_SIZE_T size = _serializer.Serialize(*this, _flags);

    mBufferPointer += size;

    mChunks.back()->AddData(size);

    mTotalWritten += size;

    return(WN_TRUE);
}

WN_CHAR* WNNetworkWriteBuffer::ReserveBytes(const WN_SIZE_T _numBytes, WN_SIZE_T& _returnedBytes) {
    WN_DEBUG_ASSERT(!mFlushed);

    if (mBufferPointer == MAX_DATA_WRITE) {
        mChunks.back()->FillData();
        mChunks.push_back(WNAllocateResource<WNBufferResource, WNNetworkManager&>(mManager));

        mBufferPointer = 0;
    }

    _returnedBytes = WNMin(_numBytes, MAX_DATA_WRITE - mBufferPointer);

    return(mChunks.back()->GetPointer());
}

WNDataBufferType WNNetworkWriteBuffer::GetType() {
    return(eWNWriteBinary);
}

WN_VOID WNNetworkWriteBuffer::FlushWrite() {
    if (mFlushed) {
        return;
    }

    mFlushed = WN_TRUE;

    WN_UINT32 totalWritten = static_cast<WN_UINT32>(mTotalWritten);

    WNCore::WNToBigEndian(totalWritten);

    *(reinterpret_cast<WN_UINT32*>(mChunks.front()->GetBaseLocation())) = totalWritten;

    for (WNBufferQueue::iterator i = mChunks.begin(); i != mChunks.end(); ++i){
        (*i)->FlushWrite();
    }
}

const WNNetworkWriteBuffer::WNBufferQueue& WNNetworkWriteBuffer::GetChunks() const {
    return(mChunks);
}
