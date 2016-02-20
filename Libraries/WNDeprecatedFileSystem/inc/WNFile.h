// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_FILE_SYSTEM_FILE_H__
#define __WN_FILE_SYSTEM_FILE_H__

#include "WNCore/inc/WNBase.h"
#include "WNCore/inc/WNTypes.h"

#ifdef _WN_WINDOWS

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

  WNFileError OpenFile(const char* _name, uint32_t _mode);
  char* GetDataBuffer();
  bool CommitFileBuffer();
  char* ResizeDataBuffer(size_t _size);
  size_t ReadData(char* outBuffer, size_t _amount);
  size_t WriteData(const char* inBuffer, size_t _amount);
  size_t GetFileSize() const;

  bool IsValid();

  static bool DoesFileExist(const char* _name);
  static char* GetFolderName(const char* _name);
  static char* GetFileName(const char* _name);
  static void CollapseFolderStructure(char* _name);
  static bool DeleteFile(const char* _name);

private:
  uint32_t mFileMode;
  size_t mMaxBufferSize;
  size_t mFileSize;
  char* mFileBuffer;
  bool mIsFileOpen;
  bool mStream;
  bool mIsValid;

#ifdef _WN_WINDOWS
  HANDLE mFileHandle;
#elif defined _WN_LINUX || defined _WN_ANDROID
  FILE* mFileHandle;
#endif
};
}

#endif  // __WN_FILE_SYSTEM_FILE_H__
