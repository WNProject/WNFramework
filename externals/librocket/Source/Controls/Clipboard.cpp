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
// Copyright (c) 2021, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.
#include "../../Include/Rocket/Controls/Clipboard.h"
#include "../../Include/Rocket/Core/Context.h"
#include "../../Include/Rocket/Core/Types.h"
#include "../../Include/Rocket/Core/WString.h"
#if defined ROCKET_PLATFORM_WIN32
#include <windows.h>
#endif

namespace Rocket {
namespace Controls {
#if defined ROCKET_PLATFORM_WIN32
static HWND* kHwndKey;

struct procData {
  DWORD process_id;
  Core::Context* context;
};

static BOOL CALLBACK FindApplicationWindow(HWND hwnd, LPARAM user_data) {
  procData* dat = reinterpret_cast<procData*>(user_data);
  auto& application_hwnd = (*dat->context)(kHwndKey);
  DWORD hwnd_pid;
  GetWindowThreadProcessId(hwnd, &hwnd_pid);
  if (hwnd_pid == dat->process_id) {
    application_hwnd = hwnd;
    return FALSE;
  }

  return TRUE;
}

static HWND GetHWND(Core::Context* _context) {
  auto& application_hwnd = (*_context)(kHwndKey);
  if (application_hwnd != NULL)
    return application_hwnd;
  procData pd{GetCurrentProcessId(), _context};
  EnumWindows(FindApplicationWindow, reinterpret_cast<LPARAM>(&pd));
  return application_hwnd;
}
#endif

static Core::WString* kContentKey;

// Get the current contents of the clipboard.
Core::WString Clipboard::Get(Core::Context* _context) {
  auto& content = (*_context)(kContentKey);
#if defined ROCKET_PLATFORM_WIN32
  if (GetHWND(_context)) {
    Core::WString clipboard_content;

    if (!OpenClipboard(GetHWND(_context)))
      return clipboard_content;

    HANDLE clipboard_data = GetClipboardData(CF_UNICODETEXT);
    if (clipboard_data == NULL) {
      CloseClipboard();
      return clipboard_content;
    }

    const Rocket::Core::word* clipboard_text =
        (const Rocket::Core::word*)GlobalLock(clipboard_data);
    if (clipboard_text)
      clipboard_content.Assign(clipboard_text);
    GlobalUnlock(clipboard_data);

    CloseClipboard();
    return clipboard_content;
  } else
    return content;
#else
  return content;
#endif
}

// Set the contents of the clipboard.
void Clipboard::Set(Core::Context* _context, const Core::WString& _content) {
  auto& content = (*_context)(kContentKey);
#if defined ROCKET_PLATFORM_WIN32
  if (GetHWND(_context)) {
    if (!OpenClipboard(GetHWND(_context)))
      return;

    EmptyClipboard();

    HGLOBAL clipboard_data = GlobalAlloc(NULL, (_content.Length() + 1) * 2);

    WCHAR* pData = (WCHAR*)GlobalLock(clipboard_data);
    for (size_t i = 0; i < _content.Length(); ++i) {
      pData[i] = _content[i];
    }

    pData[_content.Length()] = 0;

    if (SetClipboardData(CF_UNICODETEXT, clipboard_data) == NULL) {
      GlobalUnlock(clipboard_data);
    }
    CloseClipboard();
  } else
    content = _content;
#else
  content = _content;
#endif
}

#if defined ROCKET_PLATFORM_WIN32
// Set the window handle of the application.
void Clipboard::SetHWND(Core::Context* _context, void* hwnd) {
  auto& application_hwnd = (*_context)(kHwndKey);
  application_hwnd = (HWND)hwnd;
}
#endif

void Clipboard::RegisterContextValues(Core::Context* _context) {
#if defined ROCKET_PLATFORM_WIN32
  _context->RegisterCachedType(kHwndKey, HWND(nullptr));
#endif
  _context->RegisterCachedType(kContentKey);
}
}  // namespace Controls
}  // namespace Rocket
