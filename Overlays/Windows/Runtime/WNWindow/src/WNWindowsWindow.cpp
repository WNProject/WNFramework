// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNWindow/inc/WNWindowsWindow.h"
#include "WNApplication/inc/WNApplicationData.h"
#include "WNEntryPoint/inc/WNEntryData.h"
#include "WNMultiTasking/inc/WNJobPool.h"

#include <tchar.h>
#include <windows.h>

#define WN_WINDOW_CLASS_NAME _T("WNWindowClass")

namespace wn {
namespace runtime {
namespace window {

window_error windows_window::initialize() {
  WNDCLASSEX window_class;

  window_class.cbSize = sizeof(WNDCLASSEX);
  window_class.style = CS_HREDRAW | CS_VREDRAW;
  window_class.lpfnWndProc = &wnd_proc;
  window_class.cbClsExtra = 0;
  window_class.cbWndExtra = 0;
  window_class.hInstance = GetModuleHandle(NULL);
  window_class.hIcon = NULL;
  window_class.hCursor = NULL;
  window_class.hbrBackground = NULL;
  window_class.lpszMenuName = NULL;
  window_class.lpszClassName = WN_WINDOW_CLASS_NAME;
  window_class.hIconSm = NULL;

  if (!RegisterClassEx(&window_class)) {
    m_log->log_warning(
        "Could not register Windows window class,"
        " may be already registered");
    wn::memory::memory_zero(&window_class);

    if (GetClassInfoEx(
            GetModuleHandle(NULL), WN_WINDOW_CLASS_NAME, &window_class) == 0) {
      m_log->log_error("Window creation failure");
      return window_error::error;
    }
  }
  m_log->log_info("Found/Registered Windows window class");

  RECT rect = {LONG(m_x), LONG(m_y), LONG(m_x + m_width), LONG(m_y + m_height)};

  AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

  rect.left += m_x;
  rect.right += m_x;
  rect.top += m_y;
  rect.bottom += m_y;

  m_job_pool->add_job(&m_signal, &windows_window::dispatch_loop, this, rect);
  return window_error::ok;
}

void windows_window::dispatch_loop(RECT rect) {
  m_job_pool->call_blocking_function([&]() {
    m_window = CreateWindowEx(0, WN_WINDOW_CLASS_NAME, "", WS_OVERLAPPEDWINDOW,
        rect.left, rect.right, rect.right - rect.left, rect.bottom - rect.top,
        0, 0, GetModuleHandle(NULL), NULL);

    m_signal.increment(1);
    if (!m_window) {
      DWORD err = GetLastError();
      LPTSTR buff;
      FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER, nullptr, err, 0,
          (LPTSTR)&buff, 1024, nullptr);
      m_log->log_error("Failed to create Windows window: ", m_window);
      m_log->flush();
      LocalFree(buff);
      if (m_creation_signal) {
        m_creation_signal->increment(1);
      }
      return;
    }

    m_log->log_info("Created windows window ", m_window);
    SetWindowLongPtr(m_window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
    ShowWindow(m_window, SW_SHOW);
    if (m_creation_signal) {
      m_creation_signal->increment(1);
    }

    MSG msg;
    while (GetMessage(&msg, m_window, 0, 0)) {
      m_log->log_info("Receieved Windows Message: ", msg.hwnd, ", ",
          msg.message, ", ", msg.wParam, ", ", msg.lParam);
      TranslateMessage(&msg);
      DispatchMessage(&msg);

      if (m_exit) {
        break;
      }
    }
  });
  m_signal.increment(1);
}

void windows_window::process_callback(UINT, LPARAM, WPARAM) {
  // Handle events here: Resize etc.
}

LRESULT CALLBACK windows_window::wnd_proc(
    HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  windows_window* window =
      reinterpret_cast<windows_window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
  window->process_callback(uMsg, lParam, wParam);

  switch (uMsg) {
    case WM_USER:
      if (wParam == static_cast<WPARAM>(lParam) && wParam == 0) {
        window->m_exit = true;
        return (-1);
      }
    case WM_DESTROY:
      window->m_exit = true;
      return (DefWindowProc(hwnd, uMsg, wParam, lParam));
    default:
      return (DefWindowProc(hwnd, uMsg, wParam, lParam));
  }
}

}  // namespace window
}  // namespace runtime
}  // namespace wn
