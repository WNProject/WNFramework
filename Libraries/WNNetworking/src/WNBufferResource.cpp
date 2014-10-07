////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "WNNetworking/inc/WNBufferResource.h"
#include "WNNetworking/inc/WNNetworkManager.h"
#include "WNContainers/inc/WNDataBuffer.h"

using namespace WNNetworking;
using namespace WNMemory;

WNBufferResource::WNBufferResource(const WNNetworkManager& manager) :
    WNResourceBase(),
    mManager(manager),
    mCurLocation(0) {
    mBuffer = WNMallocT<WN_CHAR>(WNContainers::MAX_DATA_WRITE);
    mBaseLocation = 0;
    #ifdef _WN_WINDOWS
        mWinBuf.buf = mBuffer;
        mWinBuf.len = static_cast<ULONG>(mBaseLocation);
    #endif
}

WNBufferResource::~WNBufferResource() {
    WNFreeT<WN_CHAR>(mBuffer);
}

WN_CHAR* WNBufferResource::GetPointer() const {
    return(mBuffer + mCurLocation);
}

WN_VOID WNBufferResource::Clear() {
    mCurLocation = 0;
}

WN_VOID WNBufferResource::FillData() {
    mCurLocation = WNContainers::MAX_DATA_WRITE;
}

WN_VOID WNBufferResource::AddData(WN_SIZE_T _data) {
    mCurLocation += _data;

    WN_DEBUG_ASSERT(mCurLocation <= WNContainers::MAX_DATA_WRITE);
}

WN_CHAR* WNBufferResource::GetBaseLocation() const {
    return(mBuffer);
}

WN_SIZE_T WNBufferResource::GetSize() const {
    return(WNContainers::MAX_DATA_WRITE);
}

WN_SIZE_T WNBufferResource::GetWritten() const {
    return(mCurLocation);
}

WN_VOID WNBufferResource::FlushWrite() {
    #ifdef _WN_WINDOWS
        mWinBuf.len = static_cast<ULONG>(mCurLocation);
    #endif
}

WN_VOID WNBufferResource::PrepareRead() {
    #ifdef _WN_WINDOWS
        mWinBuf.len = static_cast<ULONG>(WNContainers::MAX_DATA_WRITE - mCurLocation);
        mWinBuf.buf = GetPointer();
    #endif
}
