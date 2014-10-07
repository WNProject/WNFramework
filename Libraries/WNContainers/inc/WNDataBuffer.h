////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_CONTAINERS_DATA_BUFFER_H__
#define __WN_CONTAINERS_DATA_BUFFER_H__

#include "WNCore/inc/WNTypes.h"

namespace WNContainers {
    class WNSerializerBase;

    enum WNDataBufferType {
        eWNReadBinary,
        eWNWriteBinary,
        eWNReadText,
        eWNWriteText
    };

    static const WN_SIZE_T MAX_DATA_WRITE = 1024;

    class WNDataBuffer {
    public:
        virtual WN_BOOL Serialize(const WN_UINT32 _flags, const WNSerializerBase& _serializer) = 0;
        virtual WN_CHAR* ReserveBytes(const WN_SIZE_T _numBytes, WN_SIZE_T& _returnedBytes) = 0;
        virtual WNDataBufferType GetType() = 0;
    };
}

#endif // __WN_CONTAINERS_DATA_BUFFER_H__
