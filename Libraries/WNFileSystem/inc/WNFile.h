// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_FILE_SYSTEM_FILE_H__
#define __WN_FILE_SYSTEM_FILE_H__

#include "WNCore/inc/WNBase.h"
#include "WNCore/inc/WNTypes.h"

#ifdef _WN_WINDOWS
    #include <windows.h>
#elif defined _WN_LINUX || defined _WN_ANDROID
    #include <stdio.h>
#endif

namespace WNFileSystem {
    class WNFile {
    public:
        WNFile();
        ~WNFile();

        enum WNFileMode {
            eWNFMNone = 0,
            eWNFMRead = 1,
            eWNFMWrite = (1 << 1),
            eWNFMExclusive = (1 << 2),
            eWNFMCreate = (1 << 3),
            eWNFMClobber = (1 << 4),
            eWNFMStream = (1 << 5),
            eWNFMMax
        };

        enum WNFileError {
            #include "WNCore/inc/Internal/WNErrors.inc"
            eWNFEBadMode,
            eWNFETooLarge,
            eWNFEBadLock,
            eWNFEMax
        };

        WNFileError OpenFile(const wn_char* _name, wn_uint32 _mode );
        wn_char* GetDataBuffer();
        wn_bool CommitFileBuffer();
        wn_char* ResizeDataBuffer(wn_size_t _size );
        wn_size_t ReadData(wn_char* outBuffer, wn_size_t _amount);
        wn_size_t WriteData(wn_char* inBuffer, wn_size_t _amount);
        wn_size_t GetFileSize();

        wn_bool IsValid();

        static wn_bool DoesFileExist(const wn_char* _name );
        static wn_char* GetFolderName(const wn_char* _name);
        static wn_char* GetFileName(const wn_char* _name);
        static wn_void CollapseFolderStructure(wn_char* _name);
        static wn_bool DeleteFile(const wn_char* _name);
        
    private:
        wn_uint32 mFileMode;
        wn_size_t mMaxBufferSize;
        wn_size_t mFileSize;
        wn_char* mFileBuffer;
        wn_bool mIsFileOpen;
        wn_bool mStream;
        wn_bool mIsValid;

        #ifdef _WN_WINDOWS
            HANDLE mFileHandle;
        #elif defined _WN_LINUX || defined _WN_ANDROID
            FILE* mFileHandle;
        #endif
    };
}

#endif // __WN_FILE_SYSTEM_FILE_H__
