/*
 * This source file is part of libRocket, the HTML/CSS Interface Middleware
 *
 * For the latest information, see http://www.librocket.com
 *
 * Copyright (c) 2008-2010 CodePoint Ltd, Shift Technology Ltd
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */
// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.
#include "StreamFile.h"
#include "../../Include/Rocket/Core.h"
#include "../../Include/Rocket/Core/FileInterface.h"
#include "precompiled.h"

namespace Rocket {
namespace Core {

StreamFile::StreamFile(Context* _context) : m_context(_context) {
  file_handle = 0;
  length = 0;
}

StreamFile::~StreamFile() {
  if (file_handle)
    Close();
}

/// Attempts to open the stream pointing at a given URL.
bool StreamFile::Open(const String& path) {
  String url_safe_path = path.Replace(":", "|");
  SetStreamDetails(URL(url_safe_path), Stream::MODE_READ);

  if (file_handle)
    Close();

  // Fix the path if a leading colon has been replaced with a pipe.
  String fixed_path = path.Replace("|", ":");
  file_handle = GetFileInterface(m_context)->Open(fixed_path);
  if (!file_handle) {
    Log::Message(m_context, Log::LT_WARNING, "Unable to open file %s.",
        fixed_path.CString());
    return false;
  }

  GetLength();

  return true;
}

// Closes the stream.
void StreamFile::Close() {
  if (file_handle) {
    GetFileInterface(m_context)->Close(file_handle);
    file_handle = 0;
  }

  length = 0;
}

/// Returns the size of this stream (in bytes).
size_t StreamFile::Length() const {
  return length;
}

// Returns the position of the stream pointer (in bytes).
size_t StreamFile::Tell() const {
  return GetFileInterface(m_context)->Tell(file_handle);
}

// Sets the stream position (in bytes).
bool StreamFile::Seek(long offset, int origin) const {
  return GetFileInterface(m_context)->Seek(file_handle, offset, origin);
}

// Read from the stream.
size_t StreamFile::Read(void* buffer, size_t bytes) const {
  return GetFileInterface(m_context)->Read(buffer, bytes, file_handle);
}

// Write to the stream at the current position.
size_t StreamFile::Write(const void* ROCKET_UNUSED_PARAMETER(buffer),
    size_t ROCKET_UNUSED_PARAMETER(bytes)) {
  ROCKET_ERROR;
  return 0;
}

// Truncate the stream to the specified length.
size_t StreamFile::Truncate(size_t ROCKET_UNUSED_PARAMETER(bytes)) {
  ROCKET_ERROR;
  return 0;
}

// Returns true if the stream is ready for reading, false otherwise.
bool StreamFile::IsReadReady() {
  return Tell() < Length();
}

// Returns true if the stream is ready for writing, false otherwise.
bool StreamFile::IsWriteReady() {
  return false;
}
// Determines the length of the stream.
void StreamFile::GetLength() {
  length = GetFileInterface(m_context)->Length(file_handle);
}
}  // namespace Core
}  // namespace Rocket
