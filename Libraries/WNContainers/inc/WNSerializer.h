// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_CONTAINERS_SERIALIZER_H__
#define __WN_CONTAINERS_SERIALIZER_H__

#include "WNCore/inc/WNTypes.h"
#include "WNContainers/inc/WNSerializerBase.h"

namespace WNContainers {
    template <typename type>
    class WNSerializer : public WNSerializerBase {
    public:
        virtual wn_size_t Serialize(WNDataBuffer& _dataBuffer, const wn_uint32 _serializeFlags) const;
    };

    template <>
    class WNSerializer<wn_uint8> : public WNSerializerBase {
    public:
        WNSerializer(wn_uint8& _item);

        virtual wn_size_t Serialize(WNDataBuffer& _dataBuffer, const wn_uint32 _serializeFlags) const;

    private:
        WNSerializer& operator = (const WNSerializer&);

    private:
        wn_uint8& mNumber;
    };

    template <>
    class WNSerializer<wn_int32> : public WNSerializerBase {
    public:
        WNSerializer(wn_int32& _item);

        virtual wn_size_t Serialize(WNDataBuffer& _dataBuffer, const wn_uint32 _serializeFlags) const;

    private:
        WNSerializer& operator = (const WNSerializer&);

    private:
        wn_int32& mNumber;
    };

    template <>
    class WNSerializer<wn_uint32> : public WNSerializerBase {
    public:
        WNSerializer(wn_uint32& _item);

        virtual wn_size_t Serialize(WNDataBuffer& _dataBuffer, const wn_uint32 _serializeFlags) const;

    private:
        WNSerializer& operator = (const WNSerializer&);

    private:
        wn_uint32& mNumber;
    };

    template <>
    class WNSerializer<wn_int64> : public WNSerializerBase {
    public:
        WNSerializer(wn_int64& _item);

        virtual wn_size_t Serialize(WNDataBuffer& _dataBuffer, const wn_uint32 _serializeFlags) const ;

    private:
        WNSerializer& operator = (const WNSerializer&);

    private:
        wn_int64& mNumber;
    };

    template <>
    class WNSerializer<wn_uint64> : public WNSerializerBase {
    public:
        WNSerializer(wn_uint64& _item);

        virtual wn_size_t Serialize(WNDataBuffer& _dataBuffer, const wn_uint32 _serializeFlags) const;

    private:
        WNSerializer& operator = (const WNSerializer&);

    private:
        wn_uint64& mNumber;
    };

    template <>
    class WNSerializer<wn_char*> : public WNSerializerBase {
    public:
        WNSerializer(wn_char*& _item, wn_size_t _length);

        virtual wn_size_t Serialize(WNDataBuffer& _dataBuffer, const wn_uint32 _serializeFlags) const;

    private:
        WNSerializer& operator = (const WNSerializer&);

    private:
        wn_size_t mLength;
        wn_char* mString;
    };

    template <>
    class WNSerializer<wn_float32> : public WNSerializerBase {
    public:
        WNSerializer(wn_float32& _item);

        virtual wn_size_t Serialize(WNDataBuffer& _dataBuffer, const wn_uint32 _serializeFlags) const;

    private:
        WNSerializer& operator = (const WNSerializer&);

    private:
        wn_float32& mNumber;
    };
}

#include "WNContainers/inc/Internal/WNSerializer.inl"

#endif // __WN_CONTAINERS_SERIALIZER_H__
