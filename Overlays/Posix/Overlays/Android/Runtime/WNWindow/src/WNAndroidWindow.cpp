// Copyright (c) 2020, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNWindow/inc/WNAndroidWindow.h"

#include "WNApplicationData/inc/WNApplicationData.h"
#include "WNMultiTasking/inc/job_pool.h"
#include "WNMultiTasking/inc/job_signal.h"
#include "executable_data/inc/executable_data.h"

namespace wn {
namespace runtime {
namespace window {

inline key_code android_keycode_to_keycode(int32_t keycode) {
  if (keycode >= AKEYCODE_0 && keycode <= AKEYCODE_9) {
    return static_cast<key_code>(
        static_cast<uint32_t>(key_code::key_0) + keycode - AKEYCODE_0);
  } else if (keycode >= AKEYCODE_NUMPAD_0 && keycode <= AKEYCODE_9) {
    return static_cast<key_code>(static_cast<uint32_t>(key_code::key_num_0) +
                                 keycode - AKEYCODE_NUMPAD_0);
  }

  switch (keycode) {
    case AKEYCODE_A:
      return key_code::key_a;
    case AKEYCODE_B:
      return key_code::key_b;
    case AKEYCODE_C:
      return key_code::key_c;
    case AKEYCODE_D:
      return key_code::key_d;
    case AKEYCODE_E:
      return key_code::key_e;
    case AKEYCODE_F:
      return key_code::key_f;
    case AKEYCODE_G:
      return key_code::key_g;
    case AKEYCODE_H:
      return key_code::key_h;
    case AKEYCODE_I:
      return key_code::key_i;
    case AKEYCODE_J:
      return key_code::key_j;
    case AKEYCODE_K:
      return key_code::key_k;
    case AKEYCODE_L:
      return key_code::key_l;
    case AKEYCODE_M:
      return key_code::key_m;
    case AKEYCODE_N:
      return key_code::key_n;
    case AKEYCODE_O:
      return key_code::key_o;
    case AKEYCODE_P:
      return key_code::key_p;
    case AKEYCODE_Q:
      return key_code::key_q;
    case AKEYCODE_R:
      return key_code::key_r;
    case AKEYCODE_S:
      return key_code::key_s;
    case AKEYCODE_T:
      return key_code::key_t;
    case AKEYCODE_U:
      return key_code::key_u;
    case AKEYCODE_V:
      return key_code::key_v;
    case AKEYCODE_W:
      return key_code::key_w;
    case AKEYCODE_X:
      return key_code::key_x;
    case AKEYCODE_Y:
      return key_code::key_y;
    case AKEYCODE_Z:
      return key_code::key_z;
    case AKEYCODE_SHIFT_LEFT:
      return key_code::key_lshift;
    case AKEYCODE_SHIFT_RIGHT:
      return key_code::key_rshift;
    case AKEYCODE_CTRL_LEFT:
      return key_code::key_lctrl;
    case AKEYCODE_CTRL_RIGHT:
      return key_code::key_rctrl;
    case AKEYCODE_ALT_LEFT:
      return key_code::key_lalt;
    case AKEYCODE_ALT_RIGHT:
      return key_code::key_ralt;
    case AKEYCODE_ESCAPE:
      return key_code::key_esc;
    case AKEYCODE_SPACE:
      return key_code::key_space;
    case AKEYCODE_DPAD_LEFT:
    case AKEYCODE_SOFT_LEFT:
      return key_code::key_left;
    case AKEYCODE_DPAD_RIGHT:
    case AKEYCODE_SOFT_RIGHT:
      return key_code::key_right;
    case AKEYCODE_DPAD_UP:
      return key_code::key_up;
    case AKEYCODE_DPAD_DOWN:
      return key_code::key_down;
    case AKEYCODE_DEL:
      return key_code::key_backspace;
    case AKEYCODE_FORWARD_DEL:
      return key_code::key_del;
  }
  return key_code::key_max;
}

window_error android_window::initialize() {
  return window_error::ok;
}

void android_window::wait_for_window_loop(void*) {
  m_job_pool->call_blocking_function([this]() {
    if (!m_app_data->executable_data->host_data->window_initialized ||
        !m_app_data->executable_data->host_data->android_app) {
      if (m_creation_signal) {
        m_creation_signal->increment(1);
      }
      return;
    }
    while (true) {
      if (m_destroy.load()) {
        if (m_creation_signal) {
          m_creation_signal->increment(1);
        }
        return;
      } else if (m_app_data->executable_data->host_data->window_initialized
                     ->load()) {
        m_data.window =
            m_app_data->executable_data->host_data->android_app->window;
        auto width = ANativeWindow_getWidth(m_data.window);
        auto height = ANativeWindow_getHeight(m_data.window);
        m_width = width;    // width;
        m_height = height;  // height;

        if (m_creation_signal) {
          m_creation_signal->increment(1);
        }
        return;
      }
      wn::multi_tasking::this_thread::sleep_for(std::chrono::milliseconds(15));
    }
  });
}

void android_window::handle_input_event(AInputEvent* _event) {
  int32_t event_type = AInputEvent_getType(_event);
  switch (event_type) {
    case AINPUT_EVENT_TYPE_MOTION:
      return handle_touch_event(_event);
    case AINPUT_EVENT_TYPE_KEY:
      return handle_key_event(_event);
    default:
      break;
  }
}

void android_window::handle_touch_event(AInputEvent* _event) {
  int32_t action = AMotionEvent_getAction(_event);
  uint8_t action_type = action & 0xFF;
  uint32_t pointer_index = (action >> 8) & 0xFFFFFF;
  uint32_t pointer_id = AMotionEvent_getPointerId(_event, pointer_index);

  if (action_type == AMOTION_EVENT_ACTION_DOWN ||
      action_type == AMOTION_EVENT_ACTION_POINTER_DOWN) {
    if (m_activated_pointer_id != -1) {
      return;
    }
    m_activated_pointer_id = pointer_id;
    m_mouse_states[0] = true;
    dispatch_input(input_event::mouse_down(mouse_button::mouse_l));
  }

  if (m_activated_pointer_id != pointer_id) {
    return;
  }
  m_cursor_x = AMotionEvent_getX(_event, pointer_id);
  m_cursor_y = AMotionEvent_getY(_event, pointer_id);
  dispatch_input(input_event::mouse_move(m_cursor_x, m_cursor_y));
  if (action_type == AMOTION_EVENT_ACTION_UP ||
      action_type == AMOTION_EVENT_ACTION_CANCEL ||
      action_type == AMOTION_EVENT_ACTION_POINTER_UP) {
    m_mouse_states[0] = false;
    m_activated_pointer_id = -1;
    dispatch_input(input_event::mouse_up(mouse_button::mouse_l));
  }
}

void android_window::handle_key_event(AInputEvent* _event) {
  int32_t action_type = AKeyEvent_getAction(_event);
  key_code code = android_keycode_to_keycode(AKeyEvent_getKeyCode(_event));

  if (action_type == AKEY_EVENT_ACTION_DOWN && code != key_code::key_max) {
    m_key_states[static_cast<uint32_t>(code)] = true;
    dispatch_input(input_event::key_event(event_type::key_down, code));
  } else if (action_type == AKEY_EVENT_ACTION_UP) {
    if (code != key_code::key_max) {
      m_key_states[static_cast<uint32_t>(code)] = false;
      dispatch_input(input_event::key_event(event_type::key_up, code));
    }
    // Try to get a text event from this as well
    {
      ANativeActivity* activity =
          m_app_data->executable_data->host_data->android_app->activity;
      JNIEnv* env = 0;

      activity->vm->AttachCurrentThread(&env, 0);
      jclass cls = env->GetObjectClass(activity->clazz);
      jmethodID method_id = env->GetMethodID(
          cls, "GetEventString", "(JJIIIIIIII)Ljava/lang/String;");

      jstring j_str = (jstring)env->CallObjectMethod(activity->clazz, method_id,
          AKeyEvent_getDownTime(_event), AKeyEvent_getEventTime(_event),
          AKeyEvent_getAction(_event), AKeyEvent_getKeyCode(_event),
          AKeyEvent_getRepeatCount(_event), AKeyEvent_getMetaState(_event),
          AInputEvent_getDeviceId(_event), AKeyEvent_getScanCode(_event),
          AKeyEvent_getFlags(_event), AInputEvent_getSource(_event));

      jsize len = env->GetStringLength(j_str);
      const jchar* chars = env->GetStringChars(j_str, nullptr);
      for (jsize i = 0; i < len; ++i) {
        switch (chars[i]) {
          case 0x00:  // null
          case 0x08:  // backspace
          case 0x0A:  // linefeed
          case 0x1B:  // escape
          case 0x09:  // tab
          case 0x0D:  // carriage return
            break;    // Ignore these for now
          default:
            dispatch_input(input_event::text_input(chars[i]));
        }
      }
      env->ReleaseStringChars(j_str, chars);
      activity->vm->DetachCurrentThread();
    }
  }
}

void android_window::show_keyboard() {
  ANativeActivity* activity =
      m_app_data->executable_data->host_data->android_app->activity;
  JNIEnv* env = 0;

  activity->vm->AttachCurrentThread(&env, 0);
  jclass cls = env->GetObjectClass(activity->clazz);
  jmethodID methodID = env->GetMethodID(cls, "ShowKeyboard", "()V");
  env->CallVoidMethod(activity->clazz, methodID);

  activity->vm->DetachCurrentThread();
}

void android_window::hide_keyboard() {
  ANativeActivity* activity =
      m_app_data->executable_data->host_data->android_app->activity;
  JNIEnv* env = 0;

  activity->vm->AttachCurrentThread(&env, 0);
  jclass cls = env->GetObjectClass(activity->clazz);
  jmethodID methodID = env->GetMethodID(cls, "HideKeyboard", "()V");
  env->CallVoidMethod(activity->clazz, methodID);

  activity->vm->DetachCurrentThread();
}

uint32_t android_window::get_dpi() const {
  ANativeActivity* activity =
      m_app_data->executable_data->host_data->android_app->activity;
  JNIEnv* env = 0;

  activity->vm->AttachCurrentThread(&env, 0);
  jclass cls = env->GetObjectClass(activity->clazz);
  jmethodID methodID = env->GetMethodID(cls, "GetDPI", "()I");
  jint i = env->CallIntMethod(activity->clazz, methodID);

  activity->vm->DetachCurrentThread();
  return i;
}

}  // namespace window
}  // namespace runtime
}  // namespace wn
