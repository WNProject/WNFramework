// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNWindow/inc/WNWindowsWindow.h"

#include "WNApplicationData/inc/WNApplicationData.h"
#include "WNMultiTasking/inc/job_pool.h"
#include "WNWindow/inc/WNInputContext.h"
#include "core/inc/base.h"
#include "executable_data/inc/executable_data.h"

#include <tchar.h>

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

  RECT rect = {LONG(m_x), LONG(m_y), LONG(m_width + m_x), LONG(m_height + m_y)};

  AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);
  const LONG left_diff = rect.left - m_x;
  const LONG top_diff = rect.top - m_y;

  rect.right -= left_diff;
  rect.bottom -= top_diff;
  rect.left = m_x;
  rect.top = m_y;

  HDC screen = GetDC(NULL);
  double h = GetDeviceCaps(screen, LOGPIXELSX);
  double v = GetDeviceCaps(screen, LOGPIXELSY);
  ReleaseDC(NULL, screen);
  m_dpi = (int)((h + v) / 2);

  m_job_pool->add_job(&m_signal, &windows_window::dispatch_loop, this, rect);
  return window_error::ok;
}

void windows_window::dispatch_loop(RECT rect) {
  m_job_pool->call_blocking_function([&]() {
    m_window.handle = CreateWindowEx(0, WN_WINDOW_CLASS_NAME, "",
        WS_OVERLAPPEDWINDOW, rect.left, rect.top, rect.right - rect.left,
        rect.bottom - rect.top, 0, 0, GetModuleHandle(NULL), NULL);

    m_signal.increment(1);
    if (!m_window.handle) {
      DWORD err = GetLastError();
      LPTSTR buff;
      FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER, nullptr, err, 0,
          (LPTSTR)&buff, 1024, nullptr);
      m_log->log_error("Failed to create Windows window: ", m_window.handle);
      m_log->flush();
      LocalFree(buff);
      if (m_creation_signal) {
        m_creation_signal->increment(1);
      }
      return;
    }
    m_window.instance = reinterpret_cast<HINSTANCE>(
        GetWindowLongPtr(m_window.handle, GWLP_HINSTANCE));

    m_log->log_info("Created windows window ", m_window.handle);
    SetWindowLongPtr(
        m_window.handle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
    ShowWindow(m_window.handle, SW_SHOW);
    if (m_creation_signal) {
      m_creation_signal->increment(1);
    }

    MSG msg;
    while (GetMessage(&msg, m_window.handle, 0, 0)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
      if (m_exit) {
        break;
      }
    }
  });
  m_signal.increment(1);
}

void windows_window::process_callback(UINT wm, WPARAM wp, LPARAM lp) {
  (void)wp;
  switch (wm) {
    case WM_SIZE:
      m_width = LOWORD(lp);
      m_height = HIWORD(lp);
      break;
  }
}

inline key_code wm_code_to_keycode(WPARAM wparam) {
  switch (wparam) {
    case VK_SHIFT:
      return key_code::key_any_shift;
    case VK_CONTROL:
      return key_code::key_any_ctrl;
    case VK_MENU:
      return key_code::key_any_alt;
    case VK_ESCAPE:
      return key_code::key_esc;
    case VK_SPACE:
      return key_code::key_space;
    case VK_LEFT:
      return key_code::key_left;
    case VK_RIGHT:
      return key_code::key_right;
    case VK_UP:
      return key_code::key_up;
    case VK_DOWN:
      return key_code::key_down;
    case VK_LSHIFT:
      return key_code::key_lshift;
    case VK_RSHIFT:
      return key_code::key_rshift;
    case VK_LCONTROL:
      return key_code::key_lctrl;
    case VK_RCONTROL:
      return key_code::key_rctrl;
    case VK_LMENU:
      return key_code::key_lalt;
    case VK_RMENU:
      return key_code::key_ralt;
    case VK_DELETE:
      return key_code::key_del;
    case VK_BACK:
      return key_code::key_backspace;
    case VK_TAB:
      return key_code::key_tab;
    case VK_RETURN:
      return key_code::key_return;
  }
  if (wparam >= 0x30 && wparam <= 0x39) {
    return static_cast<key_code>(
        static_cast<uint32_t>(key_code::key_0) + wparam - 0x30);
  } else if (wparam >= 0x60 && wparam <= 0x69) {
    return static_cast<key_code>(
        static_cast<uint32_t>(key_code::key_num_0) + wparam - 0x60);
  } else if (wparam >= 0x41 && wparam <= 0x5A) {
    return static_cast<key_code>(
        static_cast<uint32_t>(key_code::key_a) + wparam - 0x41);
  }

  return key_code::key_max;
}

LRESULT CALLBACK windows_window::wnd_proc(
    HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  windows_window* window =
      reinterpret_cast<windows_window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
  if (window) {
    window->process_callback(uMsg, wParam, lParam);
  }

  switch (uMsg) {
    case WM_USER:
      if (wParam == static_cast<WPARAM>(lParam) && wParam == 0) {
        window->m_exit = true;
        // We can't use a PostQuitMessage. It won't close the
        // window. Instead use WM_CLOSE which will close the current
        // window.
        PostMessage(window->m_window.handle, WM_CLOSE, 0, 0);
        return -1;
      }
    case WM_DESTROY:
      window->m_exit = true;
      return DefWindowProc(hwnd, uMsg, wParam, lParam);
    case WM_KEYDOWN: {
      key_code k = wm_code_to_keycode(wParam);
      if (k != key_code::key_max) {
        window->m_key_states[static_cast<uint32_t>(k)] = true;
        window->dispatch_input(input_event::key_event(event_type::key_down, k));
      }
      return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    case WM_KEYUP: {
      key_code k = wm_code_to_keycode(wParam);
      if (k != key_code::key_max) {
        window->m_key_states[static_cast<uint32_t>(k)] = false;
        window->dispatch_input(input_event::key_event(event_type::key_up, k));
      }
      return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    case WM_SETFOCUS: {  // We just got focus
      return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    case WM_KILLFOCUS: {
      for (size_t i = 0; i < static_cast<size_t>(key_code::key_max); ++i) {
        window->m_key_states[i] = false;
      }
      for (size_t i = 0; i < static_cast<size_t>(mouse_button::mouse_max);
           ++i) {
        window->m_mouse_states[i] = false;
      }

      return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    case WM_LBUTTONDOWN:
      window->m_mouse_states[static_cast<size_t>(mouse_button::mouse_l)] = true;
      window->dispatch_input(input_event::mouse_down(mouse_button::mouse_l));
      return DefWindowProc(hwnd, uMsg, wParam, lParam);
    case WM_LBUTTONUP:
      window->m_mouse_states[static_cast<size_t>(mouse_button::mouse_l)] =
          false;
      window->dispatch_input(input_event::mouse_up(mouse_button::mouse_l));
      return DefWindowProc(hwnd, uMsg, wParam, lParam);
    case WM_MBUTTONDOWN:
      window->m_mouse_states[static_cast<size_t>(mouse_button::mouse_m)] = true;
      window->dispatch_input(input_event::mouse_down(mouse_button::mouse_m));
      return DefWindowProc(hwnd, uMsg, wParam, lParam);
    case WM_MBUTTONUP:
      window->m_mouse_states[static_cast<size_t>(mouse_button::mouse_m)] =
          false;
      window->dispatch_input(input_event::mouse_up(mouse_button::mouse_m));
      return DefWindowProc(hwnd, uMsg, wParam, lParam);
    case WM_RBUTTONDOWN:
      window->m_mouse_states[static_cast<size_t>(mouse_button::mouse_r)] = true;
      window->dispatch_input(input_event::mouse_down(mouse_button::mouse_r));
      return DefWindowProc(hwnd, uMsg, wParam, lParam);
    case WM_RBUTTONUP:
      window->m_mouse_states[static_cast<size_t>(mouse_button::mouse_r)] =
          false;
      window->dispatch_input(input_event::mouse_up(mouse_button::mouse_r));
      return DefWindowProc(hwnd, uMsg, wParam, lParam);
    case WM_MOUSEMOVE: {
      uint16_t x_pos = lParam & 0xFFFF;
      uint16_t y_pos = (lParam >> 16) & 0xFFFF;
      window->m_cursor_x = x_pos;
      window->m_cursor_y = y_pos;
      window->dispatch_input(input_event::mouse_move(x_pos, y_pos));
      return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    case WM_DPICHANGED: {
      window->m_dpi = ((wParam >> 16) & 0xFFFF + (wParam & 0xFFFF)) / 2;
      return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    case WM_CHAR: {
      switch (wParam) {
        case 0x08:  // backspace
        case 0x0A:  // linefeed
        case 0x1B:  // escape
        case 0x09:  // tab
        case 0x0D:  // carriage return
          break;    // Ignore these for now
        default:
          window->dispatch_input(input_event::text_input(wParam));
          break;
      }
      return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    case WM_MOUSEWHEEL: {
      int16_t mouse_move = static_cast<int16_t>((wParam >> 16) & 0xFFFF);
      window->dispatch_input(input_event::mouse_wheel(mouse_move));
      return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    default:
      return DefWindowProc(hwnd, uMsg, wParam, lParam);
  }
}

}  // namespace window
}  // namespace runtime
}  // namespace wn
