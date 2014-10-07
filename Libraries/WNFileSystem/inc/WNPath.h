////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_FILE_SYSTEM_PATH_H__
#define __WN_FILE_SYSTEM_PATH_H__

#include "WNCore/inc/WNBase.h"
#include "WNMemory/inc/WNMemory.h"
#include "WNStrings/inc/WNStrings.h"

namespace WNFileSystem {
    WN_FORCE_INLINE WN_SIZE_T WNGetPathFromFilename(const WN_CHAR* _file) {
        const WN_CHAR* tempFile = _file;
        WN_SIZE_T braceChar = 0;

        for (; *tempFile != '\0'; ++tempFile) {
            if (*tempFile == '/' || *tempFile == '\\') {
                braceChar = tempFile - _file;
            }
        }

        return(braceChar > 0 ? braceChar + 1 : braceChar);
    }

    WN_FORCE_INLINE WN_BOOL WNAppendPath(WN_CHAR* _path, const WN_CHAR* _append, WN_SIZE_T _maxSize) {
        const WN_SIZE_T appendLen = WNStrings::WNStrLen(_append);
        WN_SIZE_T pathLen = WNStrings::WNStrLen(_path);
        WN_BOOL addPath = WN_FALSE;

        if (_path[pathLen - 1] != '/' && _path[pathLen - 1] != '\\') {
            pathLen += 1;
            addPath = WN_TRUE;
        }

        if (_maxSize < appendLen + pathLen) {
            return(WN_FALSE);
        }

        if (addPath) {
           _path[pathLen - 1] = '/';
        }

        WNMemory::WNMemCpy(_path + pathLen, _append, appendLen);

        _path[pathLen + appendLen] = '\0';

        return(WN_TRUE);
    }
}

#endif // __WN_FILE_SYSTEM_PATH_H__