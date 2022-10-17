// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_RUNTIME_WINDOW_MACOS_HELPERS_H__
#define __WN_RUNTIME_WINDOW_MACOS_HELPERS_H__

enum macos_event_type {
  macos_event_empty,
  macos_event_key_down,
  macos_event_key_up,
  macos_event_mouse_down,
  macos_event_mouse_up,
  macos_event_mouse_move,
  macos_event_mouse_wheel,
  macos_event_flags_changed
};

// PLease keep this in order with
// the normal keycode. Its simpler.
enum macos_key_code {
  macos_key_unimplemented,
  macos_key_a,
  macos_key_b,
  macos_key_c,
  macos_key_d,
  macos_key_e,
  macos_key_f,
  macos_key_g,
  macos_key_h,
  macos_key_i,
  macos_key_j,
  macos_key_k,
  macos_key_l,
  macos_key_m,
  macos_key_n,
  macos_key_o,
  macos_key_p,
  macos_key_q,
  macos_key_r,
  macos_key_s,
  macos_key_t,
  macos_key_u,
  macos_key_v,
  macos_key_w,
  macos_key_x,
  macos_key_y,
  macos_key_z,
  macos_key_0,
  macos_key_1,
  macos_key_2,
  macos_key_3,
  macos_key_4,
  macos_key_5,
  macos_key_6,
  macos_key_7,
  macos_key_8,
  macos_key_9,
  macos_key_num_0,
  macos_key_num_1,
  macos_key_num_2,
  macos_key_num_3,
  macos_key_num_4,
  macos_key_num_5,
  macos_key_num_6,
  macos_key_num_7,
  macos_key_num_8,
  macos_key_num_9,
  macos_key_lshift,
  macos_key_rshift,
  macos_key_any_shift,
  macos_key_lctrl,
  macos_key_rctrl,
  macos_key_any_ctrl,
  macos_key_lalt,
  macos_key_ralt,
  macos_key_any_alt,
  macos_key_esc,
  macos_key_space,
  macos_key_left,
  macos_key_right,
  macos_key_up,
  macos_key_down,
  macos_key_backspace,
  macos_key_del,
  macos_key_tab,
  macos_key_return,
  macos_key_max,
};

enum macos_mouse_button {
  macos_mouse_button_l,
  macos_mouse_button_r,
  macos_mouse_button_m,
  macos_mouse_button_max,
};

struct macos_keyboard_event {
  enum macos_key_code code;
  char characters[256];
};

struct macos_mouse_event {
  uint16_t x;
  uint16_t y;
  enum macos_mouse_button button;
};

struct macos_wheel_event {
  float amount;
};

struct macos_flags_event {
  bool control;
  bool option;
  bool command;
  bool shift;
};

struct macos_event {
  enum macos_event_type type;
  int window_number;
  union {
    struct macos_keyboard_event kb;
    struct macos_mouse_event mb;
    struct macos_wheel_event we;
    struct macos_flags_event fl;
  };
};

#endif  // __WN_RUNTIME_WINDOW_MACOS_HELPERS_H__
