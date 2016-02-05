// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_FILE_SYSTEM_PATH_H__
#define __WN_FILE_SYSTEM_PATH_H__

#include "WNCore/inc/WNBase.h"
#include "WNMemory/inc/WNStringUtility.h"

namespace WNFileSystem {
    WN_FORCE_INLINE size_t WNGetPathFromFilename(const char* _file) {
        const char* tempFile = _file;
        size_t braceChar = 0;

        for (; *tempFile != '\0'; ++tempFile) {
            if (*tempFile == '/' || *tempFile == '\\') {
                braceChar = tempFile - _file;
            }
        }

        return(braceChar > 0 ? braceChar + 1 : braceChar);
    }

    WN_FORCE_INLINE bool WNAppendPath(char* _path, const char* _append, size_t _maxSize) {
        const size_t appendLen = wn::memory::strlen(_append);
        size_t pathLen = wn::memory::strlen(_path);
        bool addPath = false;

        if (_path[pathLen - 1] != '/' && _path[pathLen - 1] != '\\') {
            pathLen += 1;
            addPath = true;
        }

        if (_maxSize < appendLen + pathLen) {
            return(false);
        }

        if (addPath) {
           _path[pathLen - 1] = '/';
        }

        wn::memory::memcpy(_path + pathLen, _append, appendLen);

        _path[pathLen + appendLen] = '\0';

        return(true);
    }
}

#endif // __WN_FILE_SYSTEM_PATH_H__
