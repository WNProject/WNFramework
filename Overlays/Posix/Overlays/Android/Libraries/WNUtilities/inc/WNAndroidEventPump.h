// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef __WN_UTILS_ANDROID_EVENT_PUMP_H__
#define __WN_UTILS_ANDROID_EVENT_PUMP_H__

#include "WNCore/inc/WNTypes.h"
#include "WNMultiTasking/inc/WNSemaphore.h"
#include "WNMultiTasking/inc/spin_lock.h"

#include <android_native_app_glue.h>
#include <deque>

namespace wn {
namespace utilities {

class WNAndroidEventPump {
public:
  enum eMessageType { eDisplayCreated, eDisplayDestroyed, eMaxValue };
  typedef void (*WNAndroidEventCallback)(
      eMessageType, android_app*, uint32_t, void*);
  void PumpMessages(android_app* state);  // returns when we tell it to return
  void KillMessagePump();
  void SubscribeToEvent(
      eMessageType _message, WNAndroidEventCallback _callback, void* _userData);
  void WaitForInit();
  static WNAndroidEventPump& GetInstance();

private:
  enum eInternalMessage { eDisplayAdded, eExit };
  bool PushMessage(eInternalMessage _message);
  void FireCallback(eMessageType _eventi, int32_t _param, android_app* app);
  static void HandleWindowCommand(android_app* app, int32_t cmd);
  WNAndroidEventPump();
  WNAndroidEventCallback mCallbacks[eMaxValue];
  void* mCallbackData[eMaxValue];
  bool mExiting;
  wn::multi_tasking::spin_lock mQueueLock;
  wn::multi_tasking::spin_lock mCallbackLock;
  std::deque<eInternalMessage> mMessageQueue;
  bool mDisplayActive;
  ALooper* mMainLooper;
  wn::multi_tasking::semaphore mStartedSemaphore;
};

}  // namespace utilities
}  // namespace wn

#endif  //__WN_UTILS_ANDROID_EVENT_PUMP_H__
