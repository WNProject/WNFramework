///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_FILE_SYSTEM_WRITE_FILE_BUFFER_H__
#define __WN_FILE_SYSTEM_WRITE_FILE_BUFFER_H__

#include "WNCore/inc/WNTypes.h"
#include "WNFileSystem/inc/WNFile.h"
#include "WNFileSystem/inc/WNFileBuffer.h"

namespace WNFileSystem {
    class WNWriteTextFileBuffer : public WNFileBuffer {
    public:
        WNWriteTextFileBuffer(const WNFileBufferType _type, const wn_size_t _initialBufferSize = 1024*1024);
        virtual ~WNWriteTextFileBuffer();

        virtual wn_bool serialize(const wn::containers::serializer_base& _serializer, const wn_uint32 _flags) override;
        virtual WNFile::WNFileError SetFile(const wn_char* fileName);
        virtual wn_char* reserve(const wn_size_t _numBytes, wn_size_t& _returnedBytes) override;
        virtual wn::containers::data_buffer_type type() const override;

    private:
        wn_bool DumpToFile();

    private:
        WNFile mFile;
        wn_char* mCurrentBuffer;
        wn_size_t mBufferUsage;
        wn_size_t mBufferSize;
    };
}

#endif // __WN_FILE_SYSTEM_WRITE_FILE_BUFFER_H__
