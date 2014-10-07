////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
    template <typename Type>
    WN_FORCE_INLINE WN_SIZE_T WNSerializer<Type>::Serialize(WNDataBuffer& _dataBuffer, const WN_UINT32 _serializeFlags) const {
        WN_UNUSED_ARG(_serializeFlags);

        WN_STATIC_ASSERT_DESC(WN_STATIC_ASSERT_DEPENDENT_FAIL<Type>::Value, "This type has no associated serializer");
    }

    WN_FORCE_INLINE WNSerializer<WN_UINT8>::WNSerializer(WN_UINT8& _item) :
        mNumber(_item) {
    }

    WN_FORCE_INLINE WN_SIZE_T WNSerializer<WN_UINT8>::Serialize(WNDataBuffer& _dataBuffer, const WN_UINT32 _serializeFlags) const {
        WN_UNUSED_ARG(_serializeFlags);

        switch(_dataBuffer.GetType()) {
            case eWNReadBinary: {
                WN_SIZE_T returnedBytes = 0;
                WN_SIZE_T totalBytes = 0;
                do{
                    const WN_CHAR* location = _dataBuffer.ReserveBytes(sizeof(WN_UINT8) - totalBytes, returnedBytes);
                    WNMemory::WNMemCpy(reinterpret_cast<WN_UINT8*>(&mNumber) + totalBytes, location, returnedBytes);
                    totalBytes += returnedBytes;
                } while(totalBytes < sizeof(WN_UINT8));

                return(sizeof(WN_UINT8));
            }
            case eWNWriteBinary: {
                WN_SIZE_T returnedBytes = 0;
                WN_SIZE_T totalBytes = 0;
                do {
                    WN_CHAR* location = _dataBuffer.ReserveBytes(sizeof(WN_UINT8) - totalBytes, returnedBytes);
                    WNMemory::WNMemCpy(location, reinterpret_cast<WN_UINT8*>(&mNumber) + totalBytes, returnedBytes);
                    totalBytes += returnedBytes;
                } while(totalBytes < sizeof(WN_UINT8));

                return(sizeof(WN_UINT8));
            }
            case eWNReadText: {
                WN_CHAR tempBuffer[4];
                WN_SIZE_T returnedBytes = 0;
                WN_SIZE_T totalBytes = 0;
                WN_SIZE_T countFails = 0;

                do {
                    countFails++;
                    WN_CHAR* location = _dataBuffer.ReserveBytes(4 - totalBytes, returnedBytes);
                    WNMemory::WNMemCpy(reinterpret_cast<WN_UINT8*>(&tempBuffer) + totalBytes, location, returnedBytes);
                    totalBytes += returnedBytes;
                    if(countFails > 1) {
                        break;
                    }
                } while(totalBytes < 4);

                WN_UINT32 number;
                WN_SIZE_T outSize = WNStrings::WNReadUInt32(tempBuffer, number, totalBytes - 1);

                mNumber = number & 0xFF;
                outSize += 1; //consume the space that is supposed to be there

                return(outSize);
            }
            case eWNWriteText: {
                WN_CHAR tempBuffer[4];
                WN_SIZE_T outSize = WNStrings::WNWriteUInt32(tempBuffer, mNumber, 3);
                tempBuffer[outSize++] = ' ';

                WN_SIZE_T returnedBytes = 0;
                WN_SIZE_T totalBytes = 0;
                 do {
                    WN_CHAR* location = _dataBuffer.ReserveBytes(outSize - totalBytes, returnedBytes);
                    WNMemory::WNMemCpy(location, reinterpret_cast<WN_UINT8*>(&tempBuffer) + totalBytes, returnedBytes);
                    totalBytes += returnedBytes;
                } while(totalBytes < outSize);

                return(outSize);
            }
        }

        return(0);
    }

    WN_FORCE_INLINE WNSerializer<WN_INT32>::WNSerializer(WN_INT32& _item) :
        mNumber(_item) {
    }

    WN_FORCE_INLINE WN_SIZE_T WNSerializer<WN_INT32>::Serialize(WNDataBuffer& _dataBuffer, const WN_UINT32 _serializeFlags) const {
        WN_UNUSED_ARG(_serializeFlags);

        switch(_dataBuffer.GetType()) {
            case eWNReadBinary: {
                WN_SIZE_T returnedBytes = 0;
                WN_SIZE_T totalBytes = 0;
                do{
                    const WN_CHAR* location = _dataBuffer.ReserveBytes(sizeof(WN_INT32) - totalBytes, returnedBytes);
                    WNMemory::WNMemCpy(reinterpret_cast<WN_UINT8*>(&mNumber) + totalBytes, location, returnedBytes);
                    totalBytes += returnedBytes;
                } while(totalBytes < sizeof(WN_INT32));

                WNCore::WNFromBigEndian(mNumber);

                return(sizeof(WN_INT32));
            }
            case eWNWriteBinary: {
                WN_INT32 number = mNumber;
                WNCore::WNToBigEndian(number);
                WN_SIZE_T returnedBytes = 0;
                WN_SIZE_T totalBytes = 0;
                do {
                    WN_CHAR* location = _dataBuffer.ReserveBytes(sizeof(WN_INT32) - totalBytes, returnedBytes);
                    WNMemory::WNMemCpy(location, reinterpret_cast<WN_UINT8*>(&number) + totalBytes, returnedBytes);
                    totalBytes += returnedBytes;
                } while(totalBytes < sizeof(WN_INT32));
                return(sizeof(WN_INT32));
            }
            case eWNReadText: {
                WN_CHAR tempBuffer[16];
                WN_SIZE_T returnedBytes = 0;
                WN_SIZE_T totalBytes = 0;
                WN_SIZE_T countFails = 0;

                do {
                    countFails++;
                    WN_CHAR* location = _dataBuffer.ReserveBytes(16 - totalBytes, returnedBytes);
                    WNMemory::WNMemCpy(reinterpret_cast<WN_UINT8*>(&tempBuffer) + totalBytes, location, returnedBytes);
                    totalBytes += returnedBytes;
                    if(countFails > 1) {
                        break;
                    }
                } while(totalBytes < 4);

                WN_SIZE_T outSize = WNStrings::WNReadInt32(tempBuffer, mNumber, totalBytes - 1);
                outSize += 1; //consume the space that is supposed to be there

                return(outSize);
            }
            case eWNWriteText: {
                WN_CHAR tempBuffer[17];
                WN_SIZE_T outSize = WNStrings::WNWriteInt32(tempBuffer, mNumber, 16);
                tempBuffer[outSize++] = ' ';

                WN_SIZE_T returnedBytes = 0;
                WN_SIZE_T totalBytes = 0;
                do {
                    WN_CHAR* location = _dataBuffer.ReserveBytes(outSize - totalBytes, returnedBytes);
                    WNMemory::WNMemCpy(location, reinterpret_cast<WN_UINT8*>(&tempBuffer) + totalBytes, returnedBytes);
                    totalBytes += returnedBytes;
                } while(totalBytes < outSize);

                return(outSize);
            }
        }

        return(0);
    }

    WN_FORCE_INLINE WNSerializer<WN_UINT32>::WNSerializer(WN_UINT32& _item) :
        mNumber(_item) {
    }

    WN_FORCE_INLINE WN_SIZE_T WNSerializer<WN_UINT32>::Serialize(WNDataBuffer& _dataBuffer, const WN_UINT32 _serializeFlags) const {
        WN_UNUSED_ARG(_serializeFlags);

        switch(_dataBuffer.GetType()) {
            case eWNReadBinary: {
                WN_SIZE_T returnedBytes = 0;
                WN_SIZE_T totalBytes = 0;
                do{
                    const WN_CHAR* location = _dataBuffer.ReserveBytes(sizeof(WN_UINT32) - totalBytes, returnedBytes);
                    WNMemory::WNMemCpy(reinterpret_cast<WN_UINT8*>(&mNumber) + totalBytes, location, returnedBytes);
                    totalBytes += returnedBytes;
                } while(totalBytes < sizeof(WN_UINT32));

                WNCore::WNFromBigEndian(mNumber);

                return(sizeof(WN_UINT32));
            }
            case eWNWriteBinary: {
                WN_UINT32 number = mNumber;
                WNCore::WNToBigEndian(number);
                WN_SIZE_T returnedBytes = 0;
                WN_SIZE_T totalBytes = 0;
                do {
                    WN_CHAR* location = _dataBuffer.ReserveBytes(sizeof(WN_UINT32) - totalBytes, returnedBytes);
                    WNMemory::WNMemCpy(location, reinterpret_cast<WN_UINT8*>(&number) + totalBytes, returnedBytes);
                    totalBytes += returnedBytes;
                } while(totalBytes < sizeof(WN_UINT32));
                return(sizeof(WN_UINT32));
            }
            case eWNReadText: {
                WN_CHAR tempBuffer[16];
                WN_SIZE_T returnedBytes = 0;
                WN_SIZE_T totalBytes = 0;
                WN_SIZE_T countFails = 0;

                do {
                    countFails++;
                    WN_CHAR* location = _dataBuffer.ReserveBytes(16 - totalBytes, returnedBytes);
                    WNMemory::WNMemCpy(reinterpret_cast<WN_UINT8*>(tempBuffer) + totalBytes, location, returnedBytes);
                    totalBytes += returnedBytes;
                    if(countFails > 1) {
                        break;
                    }
                } while(totalBytes < 16);

                WN_SIZE_T outSize = WNStrings::WNReadUInt32(tempBuffer, mNumber, totalBytes - 1);
                outSize += 1; //consume the space that is supposed to be there
                return(outSize);
            }
            case eWNWriteText: {
                WN_CHAR tempBuffer[17];
                WN_SIZE_T outSize = WNStrings::WNWriteUInt32(tempBuffer, mNumber, 16);
                tempBuffer[outSize++] = ' ';

                WN_SIZE_T returnedBytes = 0;
                WN_SIZE_T totalBytes = 0;
                do {
                    WN_CHAR* location = _dataBuffer.ReserveBytes(outSize - totalBytes, returnedBytes);
                    WNMemory::WNMemCpy(location, reinterpret_cast<WN_UINT8*>(&tempBuffer) + totalBytes, returnedBytes);
                    totalBytes += returnedBytes;
                } while(totalBytes < outSize);

                return(outSize);
            }
        }

        return(0);
    }

    WN_FORCE_INLINE WNSerializer<WN_INT64>::WNSerializer(WN_INT64& _item) :
        mNumber(_item) {
    }

    WN_FORCE_INLINE WN_SIZE_T WNSerializer<WN_INT64>::Serialize(WNDataBuffer& _dataBuffer, const WN_UINT32 _serializeFlags) const {
        WN_UNUSED_ARG(_serializeFlags);

        switch(_dataBuffer.GetType()) {
            case eWNReadBinary: {
                WN_SIZE_T returnedBytes = 0;
                WN_SIZE_T totalBytes = 0;
                do{
                    const WN_CHAR* location = _dataBuffer.ReserveBytes(sizeof(WN_INT64) - totalBytes, returnedBytes);
                    WNMemory::WNMemCpy(reinterpret_cast<WN_UINT8*>(&mNumber) + totalBytes, location, returnedBytes);
                    totalBytes += returnedBytes;
                } while(totalBytes < sizeof(WN_INT64));

                WNCore::WNFromBigEndian(mNumber);

                return(sizeof(WN_INT64));
            }
            case eWNWriteBinary: {
                WN_INT64 number = mNumber;
                WNCore::WNToBigEndian(number);
                WN_SIZE_T returnedBytes = 0;
                WN_SIZE_T totalBytes = 0;
                do {
                    WN_CHAR* location = _dataBuffer.ReserveBytes(sizeof(WN_INT64) - totalBytes, returnedBytes);
                    WNMemory::WNMemCpy(location, reinterpret_cast<WN_UINT8*>(&number) + totalBytes, returnedBytes);
                    totalBytes += returnedBytes;
                } while(totalBytes < sizeof(WN_INT64));
                return(sizeof(WN_INT64));
            }
            case eWNReadText: {
                WN_CHAR tempBuffer[23];
                WN_SIZE_T returnedBytes = 0;
                WN_SIZE_T totalBytes = 0;
                WN_SIZE_T countFails = 0;

                do {
                    countFails++;
                    WN_CHAR* location = _dataBuffer.ReserveBytes(23 - totalBytes, returnedBytes);
                    WNMemory::WNMemCpy(reinterpret_cast<WN_UINT8*>(tempBuffer) + totalBytes, location, returnedBytes);
                    totalBytes += returnedBytes;
                    if(countFails > 1) {
                        break;
                    }
                } while(totalBytes < 23);

                WN_SIZE_T outSize = WNStrings::WNReadInt64(tempBuffer, mNumber, totalBytes - 1);
                outSize += 1; //consume the space that is supposed to be there
                return(outSize);
            }
            case eWNWriteText: {
                WN_CHAR tempBuffer[24];
                WN_SIZE_T outSize = WNStrings::WNWriteInt64(tempBuffer, mNumber, 23);
                tempBuffer[outSize++] = ' ';

                WN_SIZE_T returnedBytes = 0;
                WN_SIZE_T totalBytes = 0;
                do {
                    WN_CHAR* location = _dataBuffer.ReserveBytes(outSize - totalBytes, returnedBytes);
                    WNMemory::WNMemCpy(location, reinterpret_cast<WN_UINT8*>(&tempBuffer) + totalBytes, returnedBytes);
                    totalBytes += returnedBytes;
                } while(totalBytes < outSize);

                return(outSize);
            }
        }

        return(0);
    }

    WN_FORCE_INLINE WNSerializer<WN_UINT64>::WNSerializer(WN_UINT64& _item) :
        mNumber(_item) {
    }

    WN_FORCE_INLINE WN_SIZE_T WNSerializer<WN_UINT64>::Serialize(WNDataBuffer& _dataBuffer, const WN_UINT32 _serializeFlags) const {
        WN_UNUSED_ARG(_serializeFlags);

        switch(_dataBuffer.GetType()) {
            case eWNReadBinary: {
                WN_SIZE_T returnedBytes = 0;
                WN_SIZE_T totalBytes = 0;
                do{
                    const WN_CHAR* location = _dataBuffer.ReserveBytes(sizeof(WN_UINT64) - totalBytes, returnedBytes);
                    WNMemory::WNMemCpy(reinterpret_cast<WN_UINT8*>(&mNumber) + totalBytes, location, returnedBytes);
                    totalBytes += returnedBytes;
                } while(totalBytes < sizeof(WN_UINT64));

                WNCore::WNFromBigEndian(mNumber);

                return(sizeof(WN_UINT64));
            }
            case eWNWriteBinary: {
                WN_UINT64 number = mNumber;
                WNCore::WNToBigEndian(number);
                WN_SIZE_T returnedBytes = 0;
                WN_SIZE_T totalBytes = 0;
                do {
                    WN_CHAR* location = _dataBuffer.ReserveBytes(sizeof(WN_UINT64) - totalBytes, returnedBytes);
                    WNMemory::WNMemCpy(location, reinterpret_cast<WN_UINT8*>(&number) + totalBytes, returnedBytes);
                    totalBytes += returnedBytes;
                } while(totalBytes < sizeof(WN_UINT64));
                return(sizeof(WN_UINT64));
            }
            case eWNReadText: {
                WN_CHAR tempBuffer[23];
                WN_SIZE_T returnedBytes = 0;
                WN_SIZE_T totalBytes = 0;
                WN_SIZE_T countFails = 0;

                do {
                    countFails++;
                    WN_CHAR* location = _dataBuffer.ReserveBytes(23 - totalBytes, returnedBytes);
                    WNMemory::WNMemCpy(reinterpret_cast<WN_UINT8*>(tempBuffer) + totalBytes, location, returnedBytes);
                    totalBytes += returnedBytes;
                    if(countFails > 1) {
                        break;
                    }
                } while(totalBytes < 23);

                WN_SIZE_T outSize = WNStrings::WNReadUInt64(tempBuffer, mNumber, totalBytes - 1);
                outSize += 1; //consume the space that is supposed to be there
                return(outSize);
            }
            case eWNWriteText: {
                WN_CHAR tempBuffer[24];
                WN_SIZE_T outSize = WNStrings::WNWriteUInt64(tempBuffer, mNumber, 23);
                tempBuffer[outSize++] = ' ';

                WN_SIZE_T returnedBytes = 0;
                WN_SIZE_T totalBytes = 0;
                do {
                    WN_CHAR* location = _dataBuffer.ReserveBytes(outSize - totalBytes, returnedBytes);
                    WNMemory::WNMemCpy(location, reinterpret_cast<WN_UINT8*>(&tempBuffer) + totalBytes, returnedBytes);
                    totalBytes += returnedBytes;
                } while(totalBytes < outSize);

                return(outSize);
            }
        }

        return(0);
    }

    WN_FORCE_INLINE WNSerializer<WN_CHAR*>::WNSerializer(WN_CHAR*& _item, WN_SIZE_T _length) :
        mString(_item),
        mLength(_length) {
    }

    WN_FORCE_INLINE WN_SIZE_T WNSerializer<WN_CHAR*>::Serialize(WNDataBuffer& _dataBuffer, const WN_UINT32 _serializeFlags) const {
        WN_UNUSED_ARG(_serializeFlags);

        switch(_dataBuffer.GetType()) {
            case eWNReadBinary:
            case eWNReadText: {
                WN_SIZE_T returnedBytes = 0;
                WN_SIZE_T totalBytes = 0;
                do{
                    const WN_CHAR* location = _dataBuffer.ReserveBytes(mLength - totalBytes, returnedBytes);
                    WNMemory::WNMemCpy(reinterpret_cast<WN_UINT8*>(mString) + totalBytes, location, returnedBytes);
                    totalBytes += returnedBytes;
                } while(totalBytes < mLength);
                
                return(mLength);
            }
            case eWNWriteBinary:
            case eWNWriteText: {
                WN_SIZE_T returnedBytes = 0;
                WN_SIZE_T totalBytes = 0;
                do {
                    WN_CHAR* location = _dataBuffer.ReserveBytes(mLength - totalBytes, returnedBytes);
                    WNMemory::WNMemCpy(location, reinterpret_cast<WN_UINT8*>(mString) + totalBytes, returnedBytes);
                    totalBytes += returnedBytes;
                } while(totalBytes < mLength);
                return(mLength);
            }
        }

        return(0);
    }

    WN_FORCE_INLINE WNSerializer<WN_FLOAT32>::WNSerializer(WN_FLOAT32& _item) :
        mNumber(_item) {
    }

    WN_FORCE_INLINE WN_SIZE_T WNSerializer<WN_FLOAT32>::Serialize(WNDataBuffer& _dataBuffer, const WN_UINT32 _serializeFlags) const {
        WN_UNUSED_ARG(_serializeFlags);

        switch(_dataBuffer.GetType()) {
           case eWNReadBinary: {
                WN_SIZE_T returnedBytes = 0;
                WN_SIZE_T totalBytes = 0;
                do{
                    const WN_CHAR* location = _dataBuffer.ReserveBytes(sizeof(WN_FLOAT32) - totalBytes, returnedBytes);
                    WNMemory::WNMemCpy(reinterpret_cast<WN_UINT8*>(&mNumber) + totalBytes, location, returnedBytes);
                    totalBytes += returnedBytes;
                } while(totalBytes < sizeof(WN_FLOAT32));

                WNCore::WNFromBigEndian(mNumber);

                return(sizeof(WN_FLOAT32));
            }
            case eWNWriteBinary: {
                WN_FLOAT32 number = mNumber;
                WNCore::WNToBigEndian(number);
                WN_SIZE_T returnedBytes = 0;
                WN_SIZE_T totalBytes = 0;
                do {
                    WN_CHAR* location = _dataBuffer.ReserveBytes(sizeof(WN_FLOAT32) - totalBytes, returnedBytes);
                    WNMemory::WNMemCpy(location, reinterpret_cast<WN_UINT8*>(&number) + totalBytes, returnedBytes);
                    totalBytes += returnedBytes;
                } while(totalBytes < sizeof(WN_FLOAT32));
                return(sizeof(WN_FLOAT32));
            }
            case eWNReadText: {
                WN_CHAR tempBuffer[20];
                WN_SIZE_T returnedBytes = 0;
                WN_SIZE_T totalBytes = 0;
                WN_SIZE_T countFails = 0;

                do {
                    countFails++;
                    WN_CHAR* location = _dataBuffer.ReserveBytes(20 - totalBytes, returnedBytes);
                    WNMemory::WNMemCpy(reinterpret_cast<WN_UINT8*>(&tempBuffer) + totalBytes, location, returnedBytes);
                    totalBytes += returnedBytes;
                    if(countFails > 1) {
                        break;
                    }
                } while(totalBytes < 20);

                WN_SIZE_T outSize = WNStrings::WNReadFloat32(tempBuffer, mNumber, totalBytes - 1);
                outSize += 1; //consume the space that is supposed to be there
                return(outSize);
            }
            case eWNWriteText: {
                WN_CHAR tempBuffer[20];
                WN_SIZE_T outSize = WNStrings::WNWriteFloat32(tempBuffer, mNumber, 19);
                tempBuffer[outSize++] = ' ';

                WN_SIZE_T returnedBytes = 0;
                WN_SIZE_T totalBytes = 0;
                do {
                    WN_CHAR* location = _dataBuffer.ReserveBytes(outSize - totalBytes, returnedBytes);
                    WNMemory::WNMemCpy(location, reinterpret_cast<WN_UINT8*>(&tempBuffer) + totalBytes, returnedBytes);
                    totalBytes += returnedBytes;
                } while(totalBytes < outSize);

                return(outSize);
            }
        }

        return(0);
    }
}

#endif // __WN_CONTAINERS_SERIALIZER_INL__