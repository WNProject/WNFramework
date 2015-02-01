///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_FILE_SYSTEM_READ_FILE_BUFFER__
#define __WN_FILE_SYSTEM_READ_FILE_BUFFER__

#include "WNCore/inc/WNTypes.h"
#include "WNFileSystem/inc/WNFile.h"
#include "WNFileSystem/inc/WNFileBuffer.h"

namespace WNFileSystem {
    class WNReadTextFileBuffer : public WNFileBuffer {
    public:
        WNReadTextFileBuffer(const WNFileBufferType _type, const WN_SIZE_T _initialBufferSize = 1024);
        virtual ~WNReadTextFileBuffer();

        virtual WN_BOOL Serialize(const WN_UINT32 _flags, const WNContainers::WNSerializerBase& _serializer);
        virtual WNFile::WNFileError SetFile(const WN_CHAR* fileName);
        virtual WN_CHAR* ReserveBytes(const WN_SIZE_T _numBytes, WN_SIZE_T& _returnedBytes);
        virtual WNContainers::WNDataBufferType GetType();

    private:
        WNFile mFile;
        WN_CHAR* mCurrentBuffer;
        WN_CHAR* mSpareBuffer;
        WN_SIZE_T mBufferUsage;
        WN_SIZE_T mBufferPosition;
        WN_SIZE_T mBufferSize;
        WN_BOOL mEndFile;
    };
}

#endif // __WN_FILE_SYSTEM_READ_FILE_BUFFER__
