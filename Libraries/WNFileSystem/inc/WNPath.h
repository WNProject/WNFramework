// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_FILE_SYSTEM_PATH_H__
#define __WN_FILE_SYSTEM_PATH_H__

#include "WNCore/inc/WNBase.h"
#include "WNMemory/inc/WNMemory.h"
#include "WNStrings/inc/WNStrings.h"

namespace WNFileSystem {
    WN_FORCE_INLINE wn_size_t WNGetPathFromFilename(const wn_char* _file) {
        const wn_char* tempFile = _file;
        wn_size_t braceChar = 0;

        for (; *tempFile != '\0'; ++tempFile) {
            if (*tempFile == '/' || *tempFile == '\\') {
                braceChar = tempFile - _file;
            }
        }

        return(braceChar > 0 ? braceChar + 1 : braceChar);
    }

    WN_FORCE_INLINE wn_bool WNAppendPath(wn_char* _path, const wn_char* _append, wn_size_t _maxSize) {
        const wn_size_t appendLen = WNStrings::WNStrLen(_append);
        wn_size_t pathLen = WNStrings::WNStrLen(_path);
        wn_bool addPath = wn_false;

        if (_path[pathLen - 1] != '/' && _path[pathLen - 1] != '\\') {
            pathLen += 1;
            addPath = wn_true;
        }

        if (_maxSize < appendLen + pathLen) {
            return(wn_false);
        }

        if (addPath) {
           _path[pathLen - 1] = '/';
        }

        wn::memory::memcpy(_path + pathLen, _append, appendLen);

        _path[pathLen + appendLen] = '\0';

        return(wn_true);
    }
}

#endif // __WN_FILE_SYSTEM_PATH_H__