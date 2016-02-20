// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_FILE_SYSTEM_READ_FILE_BUFFER__
#define __WN_FILE_SYSTEM_READ_FILE_BUFFER__

#include "WNCore/inc/WNTypes.h"
#include "WNDeprecatedFileSystem/inc/WNFile.h"
#include "WNDeprecatedFileSystem/inc/WNFileBuffer.h"

namespace WNFileSystem {
class WNReadTextFileBuffer : public WNFileBuffer {
public:
  WNReadTextFileBuffer(
      const WNFileBufferType _type, const size_t _initialBufferSize = 1024);
  virtual ~WNReadTextFileBuffer();

  virtual bool serialize(const wn::containers::serializer_base& _serializer,
      const uint32_t _flags) override;
  virtual WNFile::WNFileError SetFile(const char* fileName);
  virtual char* reserve(
      const size_t _numBytes, size_t& _returnedBytes) override;
  virtual wn::containers::data_buffer_type type() const override;

private:
  WNFile mFile;
  char* mCurrentBuffer;
  char* mSpareBuffer;
  size_t mBufferUsage;
  size_t mBufferPosition;
  size_t mBufferSize;
  bool mEndFile;
};
}

#endif  // __WN_FILE_SYSTEM_READ_FILE_BUFFER__
