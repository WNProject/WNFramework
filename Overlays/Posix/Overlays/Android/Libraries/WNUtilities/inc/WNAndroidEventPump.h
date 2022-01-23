// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef __WN_UTILS_ANDROID_EVENT_PUMP_H__
#define __WN_UTILS_ANDROID_EVENT_PUMP_H__

#include "WNMultiTasking/inc/semaphore.h"
#include "WNMultiTasking/inc/spin_lock.h"
#include "core/inc/types.h"

#include <android/input.h>
#include <android/native_window.h>
#include <android_native_app_glue.h>
#include <deque>

namespace wn {
namespace utilities {

class WNAndroidEventPump {
public:
  enum eMessageType { eDisplayCreated, eDisplayDestroyed, eMaxValue };
  typedef void (*WNAndroidEventCallback)(
      eMessageType, android_app*, uint32_t, void*);
  typedef void (*WNAndroidInputEventCallback)(AInputEvent* event, void*);

private:
  struct input_callbacks {
    void* user_data;
    WNAndroidInputEventCallback callback;
  };

public:
  void PumpMessages(android_app* state);  // returns when we tell it to return
  void KillMessagePump();
  void SubscribeToEvent(
      eMessageType _message, WNAndroidEventCallback _callback, void* _userData);

  void WaitForInit();
  static WNAndroidEventPump& GetInstance();

  using input_callback_tok = std::deque<input_callbacks>::iterator;
  input_callback_tok SubscribeToInputEvents(
      const WNAndroidInputEventCallback _callback, void* _userData);
  void UnsubscribeFromInputEvents(input_callback_tok token);

private:
  enum eInternalMessage { eDisplayAdded, eExit };
  bool PushMessage(eInternalMessage _message);
  void FireCallback(eMessageType _eventi, int32_t _param, android_app* app);
  void FireInputCallback(AInputEvent* event);
  static void HandleWindowCommand(android_app* app, int32_t cmd);
  static int32_t HandleInputEvent(android_app* app, AInputEvent* _event);
  WNAndroidEventPump();
  WNAndroidEventCallback mCallbacks[eMaxValue];
  void* mCallbackData[eMaxValue];
  bool mExiting;
  wn::multi_tasking::spin_lock mQueueLock;
  wn::multi_tasking::spin_lock mCallbackLock;
  std::deque<eInternalMessage> mMessageQueue;

  std::deque<input_callbacks> m_input_callbacks;
  bool mDisplayActive;
  ALooper* mMainLooper;
  wn::multi_tasking::semaphore mStartedSemaphore;
};

}  // namespace utilities
}  // namespace wn

#endif  //__WN_UTILS_ANDROID_EVENT_PUMP_H__
