// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_FILE_SYSTEM_WRITE_FILE_BUFFER_H__
#define __WN_FILE_SYSTEM_WRITE_FILE_BUFFER_H__

#include "WNCore/inc/WNTypes.h"
#include "WNDeprecatedFileSystem/inc/WNFile.h"
#include "WNDeprecatedFileSystem/inc/WNFileBuffer.h"

namespace WNFileSystem {
class WNWriteTextFileBuffer : public WNFileBuffer {
public:
  WNWriteTextFileBuffer(const WNFileBufferType _type,
      const size_t _initialBufferSize = 1024 * 1024);
  virtual ~WNWriteTextFileBuffer();

  virtual bool serialize(const wn::containers::serializer_base& _serializer,
      const uint32_t _flags) override;
  virtual WNFile::WNFileError SetFile(const char* fileName);
  virtual char* reserve(
      const size_t _numBytes, size_t& _returnedBytes) override;
  virtual wn::containers::data_buffer_type type() const override;

private:
  bool DumpToFile();

private:
  WNFile mFile;
  char* mCurrentBuffer;
  size_t mBufferUsage;
  size_t mBufferSize;
};
}

#endif  // __WN_FILE_SYSTEM_WRITE_FILE_BUFFER_H__
