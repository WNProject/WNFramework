////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_CONTAINERS_SERIALIZER_H__
#define __WN_CONTAINERS_SERIALIZER_H__

#include "WNCore/inc/WNTypes.h"
#include "WNContainers/inc/WNSerializerBase.h"

namespace WNContainers {
    template <typename Type>
    class WNSerializer : public WNSerializerBase {
    public:
        virtual WN_SIZE_T Serialize(WNDataBuffer& _dataBuffer, const WN_UINT32 _serializeFlags) const;
    };

    template <>
    class WNSerializer<WN_UINT8> : public WNSerializerBase {
    public:
        WNSerializer(WN_UINT8& _item);

        virtual WN_SIZE_T Serialize(WNDataBuffer& _dataBuffer, const WN_UINT32 _serializeFlags) const;

    private:
        WNSerializer& operator = (const WNSerializer&);

    private:
        WN_UINT8& mNumber;
    };

    template <>
    class WNSerializer<WN_INT32> : public WNSerializerBase {
    public:
        WNSerializer(WN_INT32& _item);

        virtual WN_SIZE_T Serialize(WNDataBuffer& _dataBuffer, const WN_UINT32 _serializeFlags) const;

    private:
        WNSerializer& operator = (const WNSerializer&);

    private:
        WN_INT32& mNumber;
    };

    template <>
    class WNSerializer<WN_UINT32> : public WNSerializerBase {
    public:
        WNSerializer(WN_UINT32& _item);

        virtual WN_SIZE_T Serialize(WNDataBuffer& _dataBuffer, const WN_UINT32 _serializeFlags) const;

    private:
        WNSerializer& operator = (const WNSerializer&);

    private:
        WN_UINT32& mNumber;
    };

    template <>
    class WNSerializer<WN_INT64> : public WNSerializerBase {
    public:
        WNSerializer(WN_INT64& _item);

        virtual WN_SIZE_T Serialize(WNDataBuffer& _dataBuffer, const WN_UINT32 _serializeFlags) const ;

    private:
        WNSerializer& operator = (const WNSerializer&);

    private:
        WN_INT64& mNumber;
    };

    template <>
    class WNSerializer<WN_UINT64> : public WNSerializerBase {
    public:
        WNSerializer(WN_UINT64& _item);

        virtual WN_SIZE_T Serialize(WNDataBuffer& _dataBuffer, const WN_UINT32 _serializeFlags) const;

    private:
        WNSerializer& operator = (const WNSerializer&);

    private:
        WN_UINT64& mNumber;
    };

    template <>
    class WNSerializer<WN_CHAR*> : public WNSerializerBase {
    public:
        WNSerializer(WN_CHAR*& _item, WN_SIZE_T _length);

        virtual WN_SIZE_T Serialize(WNDataBuffer& _dataBuffer, const WN_UINT32 _serializeFlags) const;

    private:
        WNSerializer& operator = (const WNSerializer&);

    private:
        WN_SIZE_T mLength;
        WN_CHAR* mString;
    };

    template <>
    class WNSerializer<WN_FLOAT32> : public WNSerializerBase {
    public:
        WNSerializer(WN_FLOAT32& _item);

        virtual WN_SIZE_T Serialize(WNDataBuffer& _dataBuffer, const WN_UINT32 _serializeFlags) const;

    private:
        WNSerializer& operator = (const WNSerializer&);

    private:
        WN_FLOAT32& mNumber;
    };
}

#include "WNContainers/inc/Internal/WNSerializer.inl"

#endif // __WN_CONTAINERS_SERIALIZER_H__
