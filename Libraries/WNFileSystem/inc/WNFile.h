////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

        WNFileError OpenFile(const WN_CHAR* _name, WN_UINT32 _mode );
        WN_CHAR* GetDataBuffer();
        WN_BOOL CommitFileBuffer();
        WN_CHAR* ResizeDataBuffer(WN_SIZE_T _size );
        WN_SIZE_T ReadData(WN_CHAR* outBuffer, WN_SIZE_T _amount);
        WN_SIZE_T WriteData(WN_CHAR* inBuffer, WN_SIZE_T _amount);
        WN_SIZE_T GetFileSize();

        WN_BOOL IsValid();

        static WN_BOOL DoesFileExist(const WN_CHAR* _name );
        static WN_CHAR* GetFolderName(const WN_CHAR* _name);
        static WN_CHAR* GetFileName(const WN_CHAR* _name);
        static WN_VOID CollapseFolderStructure(WN_CHAR* _name);
        static WN_BOOL DeleteFile(const WN_CHAR* _name);
        
    private:
        WN_UINT32 mFileMode;
        WN_SIZE_T mMaxBufferSize;
        WN_SIZE_T mFileSize;
        WN_CHAR* mFileBuffer;
        WN_BOOL mIsFileOpen;
        WN_BOOL mStream;
        WN_BOOL mIsValid;

        #ifdef _WN_WINDOWS
            HANDLE mFileHandle;
        #elif defined _WN_LINUX || defined _WN_ANDROID
            FILE* mFileHandle;
        #endif
    };
}

#endif // __WN_FILE_SYSTEM_FILE_H__
