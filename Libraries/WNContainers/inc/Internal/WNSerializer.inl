// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_CONTAINERS_SERIALIZER_INL__
#define __WN_CONTAINERS_SERIALIZER_INL__

#ifndef __WN_CONTAINERS_SERIALIZER_H__
    #error "WNSerializer.inl should never be included directly. Please include WNSerializer.h instead"
#endif

#include "WNCore/inc/WNTypes.h"
#include "WNCore/inc/WNEndian.h"
#include "WNCore/inc/WNAssert.h"
#include "WNStrings/inc/WNStrings.h"
#include "WNMemory/inc/WNManipulation.h"
#include "WNContainers/inc/WNDataBuffer.h"
#include "WNContainers/inc/WNSerializer.h"

namespace WNContainers {
    template <typename type>
    WN_FORCE_INLINE wn_size_t WNSerializer<type>::Serialize(WNDataBuffer& _dataBuffer, const wn_uint32 _serializeFlags) const {
        WN_UNUSED_ARGUMENT(_serializeFlags);

        static_assert(WN_STATIC_ASSERT_DEPENDENT_FAIL<type>::Value, "This type has no associated serializer");
    }

    WN_FORCE_INLINE WNSerializer<wn_uint8>::WNSerializer(wn_uint8& _item) :
        mNumber(_item) {
    }

    WN_FORCE_INLINE wn_size_t WNSerializer<wn_uint8>::Serialize(WNDataBuffer& _dataBuffer, const wn_uint32 _serializeFlags) const {
        WN_UNUSED_ARGUMENT(_serializeFlags);

        switch(_dataBuffer.GetType()) {
            case eWNReadBinary: {
                wn_size_t returnedBytes = 0;
                wn_size_t totalBytes = 0;
                do{
                    const wn_char* location = _dataBuffer.ReserveBytes(sizeof(wn_uint8) - totalBytes, returnedBytes);
                    WNMemory::WNMemCpy(reinterpret_cast<wn_uint8*>(&mNumber) + totalBytes, location, returnedBytes);
                    totalBytes += returnedBytes;
                } while(totalBytes < sizeof(wn_uint8));

                return(sizeof(wn_uint8));
            }
            case eWNWriteBinary: {
                wn_size_t returnedBytes = 0;
                wn_size_t totalBytes = 0;
                do {
                    wn_char* location = _dataBuffer.ReserveBytes(sizeof(wn_uint8) - totalBytes, returnedBytes);
                    WNMemory::WNMemCpy(location, reinterpret_cast<wn_uint8*>(&mNumber) + totalBytes, returnedBytes);
                    totalBytes += returnedBytes;
                } while(totalBytes < sizeof(wn_uint8));

                return(sizeof(wn_uint8));
            }
            case eWNReadText: {
                wn_char tempBuffer[4];
                wn_size_t returnedBytes = 0;
                wn_size_t totalBytes = 0;
                wn_size_t countFails = 0;

                do {
                    countFails++;
                    wn_char* location = _dataBuffer.ReserveBytes(4 - totalBytes, returnedBytes);
                    WNMemory::WNMemCpy(reinterpret_cast<wn_uint8*>(&tempBuffer) + totalBytes, location, returnedBytes);
                    totalBytes += returnedBytes;
                    if(countFails > 1) {
                        break;
                    }
                } while(totalBytes < 4);

                wn_uint32 number;
                wn_size_t outSize = WNStrings::WNReadUInt32(tempBuffer, number, totalBytes - 1);

                mNumber = number & 0xFF;
                outSize += 1; //consume the space that is supposed to be there

                return(outSize);
            }
            case eWNWriteText: {
                wn_char tempBuffer[4];
                wn_size_t outSize = WNStrings::WNWriteUInt32(tempBuffer, mNumber, 3);
                tempBuffer[outSize++] = ' ';

                wn_size_t returnedBytes = 0;
                wn_size_t totalBytes = 0;
                 do {
                    wn_char* location = _dataBuffer.ReserveBytes(outSize - totalBytes, returnedBytes);
                    WNMemory::WNMemCpy(location, reinterpret_cast<wn_uint8*>(&tempBuffer) + totalBytes, returnedBytes);
                    totalBytes += returnedBytes;
                } while(totalBytes < outSize);

                return(outSize);
            }
        }

        return(0);
    }

    WN_FORCE_INLINE WNSerializer<wn_int32>::WNSerializer(wn_int32& _item) :
        mNumber(_item) {
    }

    WN_FORCE_INLINE wn_size_t WNSerializer<wn_int32>::Serialize(WNDataBuffer& _dataBuffer, const wn_uint32 _serializeFlags) const {
        WN_UNUSED_ARGUMENT(_serializeFlags);

        switch(_dataBuffer.GetType()) {
            case eWNReadBinary: {
                wn_size_t returnedBytes = 0;
                wn_size_t totalBytes = 0;
                do{
                    const wn_char* location = _dataBuffer.ReserveBytes(sizeof(wn_int32) - totalBytes, returnedBytes);
                    WNMemory::WNMemCpy(reinterpret_cast<wn_uint8*>(&mNumber) + totalBytes, location, returnedBytes);
                    totalBytes += returnedBytes;
                } while(totalBytes < sizeof(wn_int32));

                WNCore::WNFromBigEndian(mNumber);

                return(sizeof(wn_int32));
            }
            case eWNWriteBinary: {
                wn_int32 number = mNumber;
                WNCore::WNToBigEndian(number);
                wn_size_t returnedBytes = 0;
                wn_size_t totalBytes = 0;
                do {
                    wn_char* location = _dataBuffer.ReserveBytes(sizeof(wn_int32) - totalBytes, returnedBytes);
                    WNMemory::WNMemCpy(location, reinterpret_cast<wn_uint8*>(&number) + totalBytes, returnedBytes);
                    totalBytes += returnedBytes;
                } while(totalBytes < sizeof(wn_int32));
                return(sizeof(wn_int32));
            }
            case eWNReadText: {
                wn_char tempBuffer[16];
                wn_size_t returnedBytes = 0;
                wn_size_t totalBytes = 0;
                wn_size_t countFails = 0;

                do {
                    countFails++;
                    wn_char* location = _dataBuffer.ReserveBytes(16 - totalBytes, returnedBytes);
                    WNMemory::WNMemCpy(reinterpret_cast<wn_uint8*>(&tempBuffer) + totalBytes, location, returnedBytes);
                    totalBytes += returnedBytes;
                    if(countFails > 1) {
                        break;
                    }
                } while(totalBytes < 4);

                wn_size_t outSize = WNStrings::WNReadInt32(tempBuffer, mNumber, totalBytes - 1);
                outSize += 1; //consume the space that is supposed to be there

                return(outSize);
            }
            case eWNWriteText: {
                wn_char tempBuffer[17];
                wn_size_t outSize = WNStrings::WNWriteInt32(tempBuffer, mNumber, 16);
                tempBuffer[outSize++] = ' ';

                wn_size_t returnedBytes = 0;
                wn_size_t totalBytes = 0;
                do {
                    wn_char* location = _dataBuffer.ReserveBytes(outSize - totalBytes, returnedBytes);
                    WNMemory::WNMemCpy(location, reinterpret_cast<wn_uint8*>(&tempBuffer) + totalBytes, returnedBytes);
                    totalBytes += returnedBytes;
                } while(totalBytes < outSize);

                return(outSize);
            }
        }

        return(0);
    }

    WN_FORCE_INLINE WNSerializer<wn_uint32>::WNSerializer(wn_uint32& _item) :
        mNumber(_item) {
    }

    WN_FORCE_INLINE wn_size_t WNSerializer<wn_uint32>::Serialize(WNDataBuffer& _dataBuffer, const wn_uint32 _serializeFlags) const {
        WN_UNUSED_ARGUMENT(_serializeFlags);

        switch(_dataBuffer.GetType()) {
            case eWNReadBinary: {
                wn_size_t returnedBytes = 0;
                wn_size_t totalBytes = 0;
                do{
                    const wn_char* location = _dataBuffer.ReserveBytes(sizeof(wn_uint32) - totalBytes, returnedBytes);
                    WNMemory::WNMemCpy(reinterpret_cast<wn_uint8*>(&mNumber) + totalBytes, location, returnedBytes);
                    totalBytes += returnedBytes;
                } while(totalBytes < sizeof(wn_uint32));

                WNCore::WNFromBigEndian(mNumber);

                return(sizeof(wn_uint32));
            }
            case eWNWriteBinary: {
                wn_uint32 number = mNumber;
                WNCore::WNToBigEndian(number);
                wn_size_t returnedBytes = 0;
                wn_size_t totalBytes = 0;
                do {
                    wn_char* location = _dataBuffer.ReserveBytes(sizeof(wn_uint32) - totalBytes, returnedBytes);
                    WNMemory::WNMemCpy(location, reinterpret_cast<wn_uint8*>(&number) + totalBytes, returnedBytes);
                    totalBytes += returnedBytes;
                } while(totalBytes < sizeof(wn_uint32));
                return(sizeof(wn_uint32));
            }
            case eWNReadText: {
                wn_char tempBuffer[16];
                wn_size_t returnedBytes = 0;
                wn_size_t totalBytes = 0;
                wn_size_t countFails = 0;

                do {
                    countFails++;
                    wn_char* location = _dataBuffer.ReserveBytes(16 - totalBytes, returnedBytes);
                    WNMemory::WNMemCpy(reinterpret_cast<wn_uint8*>(tempBuffer) + totalBytes, location, returnedBytes);
                    totalBytes += returnedBytes;
                    if(countFails > 1) {
                        break;
                    }
                } while(totalBytes < 16);

                wn_size_t outSize = WNStrings::WNReadUInt32(tempBuffer, mNumber, totalBytes - 1);
                outSize += 1; //consume the space that is supposed to be there
                return(outSize);
            }
            case eWNWriteText: {
                wn_char tempBuffer[17];
                wn_size_t outSize = WNStrings::WNWriteUInt32(tempBuffer, mNumber, 16);
                tempBuffer[outSize++] = ' ';

                wn_size_t returnedBytes = 0;
                wn_size_t totalBytes = 0;
                do {
                    wn_char* location = _dataBuffer.ReserveBytes(outSize - totalBytes, returnedBytes);
                    WNMemory::WNMemCpy(location, reinterpret_cast<wn_uint8*>(&tempBuffer) + totalBytes, returnedBytes);
                    totalBytes += returnedBytes;
                } while(totalBytes < outSize);

                return(outSize);
            }
        }

        return(0);
    }

    WN_FORCE_INLINE WNSerializer<wn_int64>::WNSerializer(wn_int64& _item) :
        mNumber(_item) {
    }

    WN_FORCE_INLINE wn_size_t WNSerializer<wn_int64>::Serialize(WNDataBuffer& _dataBuffer, const wn_uint32 _serializeFlags) const {
        WN_UNUSED_ARGUMENT(_serializeFlags);

        switch(_dataBuffer.GetType()) {
            case eWNReadBinary: {
                wn_size_t returnedBytes = 0;
                wn_size_t totalBytes = 0;
                do{
                    const wn_char* location = _dataBuffer.ReserveBytes(sizeof(wn_int64) - totalBytes, returnedBytes);
                    WNMemory::WNMemCpy(reinterpret_cast<wn_uint8*>(&mNumber) + totalBytes, location, returnedBytes);
                    totalBytes += returnedBytes;
                } while(totalBytes < sizeof(wn_int64));

                WNCore::WNFromBigEndian(mNumber);

                return(sizeof(wn_int64));
            }
            case eWNWriteBinary: {
                wn_int64 number = mNumber;
                WNCore::WNToBigEndian(number);
                wn_size_t returnedBytes = 0;
                wn_size_t totalBytes = 0;
                do {
                    wn_char* location = _dataBuffer.ReserveBytes(sizeof(wn_int64) - totalBytes, returnedBytes);
                    WNMemory::WNMemCpy(location, reinterpret_cast<wn_uint8*>(&number) + totalBytes, returnedBytes);
                    totalBytes += returnedBytes;
                } while(totalBytes < sizeof(wn_int64));
                return(sizeof(wn_int64));
            }
            case eWNReadText: {
                wn_char tempBuffer[23];
                wn_size_t returnedBytes = 0;
                wn_size_t totalBytes = 0;
                wn_size_t countFails = 0;

                do {
                    countFails++;
                    wn_char* location = _dataBuffer.ReserveBytes(23 - totalBytes, returnedBytes);
                    WNMemory::WNMemCpy(reinterpret_cast<wn_uint8*>(tempBuffer) + totalBytes, location, returnedBytes);
                    totalBytes += returnedBytes;
                    if(countFails > 1) {
                        break;
                    }
                } while(totalBytes < 23);

                wn_size_t outSize = WNStrings::WNReadInt64(tempBuffer, mNumber, totalBytes - 1);
                outSize += 1; //consume the space that is supposed to be there
                return(outSize);
            }
            case eWNWriteText: {
                wn_char tempBuffer[24];
                wn_size_t outSize = WNStrings::WNWriteInt64(tempBuffer, mNumber, 23);
                tempBuffer[outSize++] = ' ';

                wn_size_t returnedBytes = 0;
                wn_size_t totalBytes = 0;
                do {
                    wn_char* location = _dataBuffer.ReserveBytes(outSize - totalBytes, returnedBytes);
                    WNMemory::WNMemCpy(location, reinterpret_cast<wn_uint8*>(&tempBuffer) + totalBytes, returnedBytes);
                    totalBytes += returnedBytes;
                } while(totalBytes < outSize);

                return(outSize);
            }
        }

        return(0);
    }

    WN_FORCE_INLINE WNSerializer<wn_uint64>::WNSerializer(wn_uint64& _item) :
        mNumber(_item) {
    }

    WN_FORCE_INLINE wn_size_t WNSerializer<wn_uint64>::Serialize(WNDataBuffer& _dataBuffer, const wn_uint32 _serializeFlags) const {
        WN_UNUSED_ARGUMENT(_serializeFlags);

        switch(_dataBuffer.GetType()) {
            case eWNReadBinary: {
                wn_size_t returnedBytes = 0;
                wn_size_t totalBytes = 0;
                do{
                    const wn_char* location = _dataBuffer.ReserveBytes(sizeof(wn_uint64) - totalBytes, returnedBytes);
                    WNMemory::WNMemCpy(reinterpret_cast<wn_uint8*>(&mNumber) + totalBytes, location, returnedBytes);
                    totalBytes += returnedBytes;
                } while(totalBytes < sizeof(wn_uint64));

                WNCore::WNFromBigEndian(mNumber);

                return(sizeof(wn_uint64));
            }
            case eWNWriteBinary: {
                wn_uint64 number = mNumber;
                WNCore::WNToBigEndian(number);
                wn_size_t returnedBytes = 0;
                wn_size_t totalBytes = 0;
                do {
                    wn_char* location = _dataBuffer.ReserveBytes(sizeof(wn_uint64) - totalBytes, returnedBytes);
                    WNMemory::WNMemCpy(location, reinterpret_cast<wn_uint8*>(&number) + totalBytes, returnedBytes);
                    totalBytes += returnedBytes;
                } while(totalBytes < sizeof(wn_uint64));
                return(sizeof(wn_uint64));
            }
            case eWNReadText: {
                wn_char tempBuffer[23];
                wn_size_t returnedBytes = 0;
                wn_size_t totalBytes = 0;
                wn_size_t countFails = 0;

                do {
                    countFails++;
                    wn_char* location = _dataBuffer.ReserveBytes(23 - totalBytes, returnedBytes);
                    WNMemory::WNMemCpy(reinterpret_cast<wn_uint8*>(tempBuffer) + totalBytes, location, returnedBytes);
                    totalBytes += returnedBytes;
                    if(countFails > 1) {
                        break;
                    }
                } while(totalBytes < 23);

                wn_size_t outSize = WNStrings::WNReadUInt64(tempBuffer, mNumber, totalBytes - 1);
                outSize += 1; //consume the space that is supposed to be there
                return(outSize);
            }
            case eWNWriteText: {
                wn_char tempBuffer[24];
                wn_size_t outSize = WNStrings::WNWriteUInt64(tempBuffer, mNumber, 23);
                tempBuffer[outSize++] = ' ';

                wn_size_t returnedBytes = 0;
                wn_size_t totalBytes = 0;
                do {
                    wn_char* location = _dataBuffer.ReserveBytes(outSize - totalBytes, returnedBytes);
                    WNMemory::WNMemCpy(location, reinterpret_cast<wn_uint8*>(&tempBuffer) + totalBytes, returnedBytes);
                    totalBytes += returnedBytes;
                } while(totalBytes < outSize);

                return(outSize);
            }
        }

        return(0);
    }

    WN_FORCE_INLINE WNSerializer<wn_char*>::WNSerializer(wn_char*& _item, wn_size_t _length) :
        mString(_item),
        mLength(_length) {
    }

    WN_FORCE_INLINE wn_size_t WNSerializer<wn_char*>::Serialize(WNDataBuffer& _dataBuffer, const wn_uint32 _serializeFlags) const {
        WN_UNUSED_ARGUMENT(_serializeFlags);

        switch(_dataBuffer.GetType()) {
            case eWNReadBinary:
            case eWNReadText: {
                wn_size_t returnedBytes = 0;
                wn_size_t totalBytes = 0;
                do{
                    const wn_char* location = _dataBuffer.ReserveBytes(mLength - totalBytes, returnedBytes);
                    WNMemory::WNMemCpy(reinterpret_cast<wn_uint8*>(mString) + totalBytes, location, returnedBytes);
                    totalBytes += returnedBytes;
                } while(totalBytes < mLength);
                
                return(mLength);
            }
            case eWNWriteBinary:
            case eWNWriteText: {
                wn_size_t returnedBytes = 0;
                wn_size_t totalBytes = 0;
                do {
                    wn_char* location = _dataBuffer.ReserveBytes(mLength - totalBytes, returnedBytes);
                    WNMemory::WNMemCpy(location, reinterpret_cast<wn_uint8*>(mString) + totalBytes, returnedBytes);
                    totalBytes += returnedBytes;
                } while(totalBytes < mLength);
                return(mLength);
            }
        }

        return(0);
    }

    WN_FORCE_INLINE WNSerializer<wn_float32>::WNSerializer(wn_float32& _item) :
        mNumber(_item) {
    }

    WN_FORCE_INLINE wn_size_t WNSerializer<wn_float32>::Serialize(WNDataBuffer& _dataBuffer, const wn_uint32 _serializeFlags) const {
        WN_UNUSED_ARGUMENT(_serializeFlags);

        switch(_dataBuffer.GetType()) {
           case eWNReadBinary: {
                wn_size_t returnedBytes = 0;
                wn_size_t totalBytes = 0;
                do{
                    const wn_char* location = _dataBuffer.ReserveBytes(sizeof(wn_float32) - totalBytes, returnedBytes);
                    WNMemory::WNMemCpy(reinterpret_cast<wn_uint8*>(&mNumber) + totalBytes, location, returnedBytes);
                    totalBytes += returnedBytes;
                } while(totalBytes < sizeof(wn_float32));

                WNCore::WNFromBigEndian(mNumber);

                return(sizeof(wn_float32));
            }
            case eWNWriteBinary: {
                wn_float32 number = mNumber;
                WNCore::WNToBigEndian(number);
                wn_size_t returnedBytes = 0;
                wn_size_t totalBytes = 0;
                do {
                    wn_char* location = _dataBuffer.ReserveBytes(sizeof(wn_float32) - totalBytes, returnedBytes);
                    WNMemory::WNMemCpy(location, reinterpret_cast<wn_uint8*>(&number) + totalBytes, returnedBytes);
                    totalBytes += returnedBytes;
                } while(totalBytes < sizeof(wn_float32));
                return(sizeof(wn_float32));
            }
            case eWNReadText: {
                wn_char tempBuffer[20];
                wn_size_t returnedBytes = 0;
                wn_size_t totalBytes = 0;
                wn_size_t countFails = 0;

                do {
                    countFails++;
                    wn_char* location = _dataBuffer.ReserveBytes(20 - totalBytes, returnedBytes);
                    WNMemory::WNMemCpy(reinterpret_cast<wn_uint8*>(&tempBuffer) + totalBytes, location, returnedBytes);
                    totalBytes += returnedBytes;
                    if(countFails > 1) {
                        break;
                    }
                } while(totalBytes < 20);

                wn_size_t outSize = WNStrings::WNReadFloat32(tempBuffer, mNumber, totalBytes - 1);
                outSize += 1; //consume the space that is supposed to be there
                return(outSize);
            }
            case eWNWriteText: {
                wn_char tempBuffer[20];
                wn_size_t outSize = WNStrings::WNWriteFloat32(tempBuffer, mNumber, 19);
                tempBuffer[outSize++] = ' ';

                wn_size_t returnedBytes = 0;
                wn_size_t totalBytes = 0;
                do {
                    wn_char* location = _dataBuffer.ReserveBytes(outSize - totalBytes, returnedBytes);
                    WNMemory::WNMemCpy(location, reinterpret_cast<wn_uint8*>(&tempBuffer) + totalBytes, returnedBytes);
                    totalBytes += returnedBytes;
                } while(totalBytes < outSize);

                return(outSize);
            }
        }

        return(0);
    }
}

#endif // __WN_CONTAINERS_SERIALIZER_INL__