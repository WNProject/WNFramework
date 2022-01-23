// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_RUNTIME_WINDOW_ENUMS_H__
#define __WN_RUNTIME_WINDOW_ENUMS_H__

namespace wn {
namespace runtime {
namespace window {

enum class window_type { null, system };

enum class mouse_button : uint16_t {
  mouse_l,
  mouse_r,
  mouse_m,
  mouse_max,
};

enum class key_code : uint16_t {
  key_unimplemented,
  key_a,
  key_b,
  key_c,
  key_d,
  key_e,
  key_f,
  key_g,
  key_h,
  key_i,
  key_j,
  key_k,
  key_l,
  key_m,
  key_n,
  key_o,
  key_p,
  key_q,
  key_r,
  key_s,
  key_t,
  key_u,
  key_v,
  key_w,
  key_x,
  key_y,
  key_z,
  key_0,
  key_1,
  key_2,
  key_3,
  key_4,
  key_5,
  key_6,
  key_7,
  key_8,
  key_9,
  key_num_0,
  key_num_1,
  key_num_2,
  key_num_3,
  key_num_4,
  key_num_5,
  key_num_6,
  key_num_7,
  key_num_8,
  key_num_9,
  key_lshift,
  key_rshift,
  key_any_shift,
  key_lctrl,
  key_rctrl,
  key_any_ctrl,
  key_lalt,
  key_ralt,
  key_any_alt,
  key_esc,
  key_space,
  key_left,
  key_right,
  key_up,
  key_down,
  key_backspace,
  key_del,
  key_tab,
  key_return,
  key_max,
};

}  // namespace window
}  // namespace runtime
}  // namespace wn

#endif  // __WN_RUNTIME_WINDOW_ENUMS_H__
