#include "WNUtilities/inc/WNAndroidEventPump.h"

#include <cstring>

#include "WNUtilities/inc/WNAppData.h"

namespace wn {
namespace utilities {

WNAndroidEventPump& WNAndroidEventPump::GetInstance() {
  static WNAndroidEventPump sPump;
  return (sPump);
}

WNAndroidEventPump::WNAndroidEventPump()
  : mExiting(false), mDisplayActive(false) {
  memset(mCallbacks, 0, sizeof(mCallbacks));
  memset(mCallbackData, 0, sizeof(mCallbackData));
  mMainLooper = NULL;
}

void WNAndroidEventPump::FireCallback(
    eMessageType _event, int32_t _param, android_app* _state) {
  mCallbackLock.lock();
  if (mCallbacks[_event] != 0) {
    mCallbacks[_event](_event, _state, _param, mCallbackData[_event]);
  }
  mCallbackLock.unlock();
}

void WNAndroidEventPump::FireInputCallback(AInputEvent* _param) {
  mCallbackLock.lock();
  for (auto& cb : m_input_callbacks) {
    cb.callback(_param, cb.user_data);
  }
  mCallbackLock.unlock();
}

void WNAndroidEventPump::SubscribeToEvent(
    eMessageType _message, WNAndroidEventCallback _callback, void* _userData) {
  mCallbackLock.lock();
  mCallbacks[_message] = _callback;
  mCallbackData[_message] = _userData;
  if (_callback && _message == eDisplayCreated) {
    PushMessage(eDisplayAdded);
  }
  mCallbackLock.unlock();
}

WNAndroidEventPump::input_callback_tok
WNAndroidEventPump::SubscribeToInputEvents(
    WNAndroidInputEventCallback _callback, void* _userData) {
  mCallbackLock.lock();
  m_input_callbacks.push_back({_userData, _callback});
  auto ret = m_input_callbacks.end() - 1;
  mCallbackLock.unlock();
  return ret;
}

void WNAndroidEventPump::UnsubscribeFromInputEvents(
    WNAndroidEventPump::input_callback_tok _token) {
  mCallbackLock.lock();
  m_input_callbacks.erase(_token);
  mCallbackLock.unlock();
}

void WNAndroidEventPump::KillMessagePump() {
  PushMessage(eExit);
}

void WNAndroidEventPump::WaitForInit() {
  mStartedSemaphore.wait();
  mStartedSemaphore.notify();
}

bool WNAndroidEventPump::PushMessage(eInternalMessage _message) {
  mQueueLock.lock();
  if (mExiting) {
    return (false);
  }

  if (_message == eExit) {
    mExiting = true;
  }
  mMessageQueue.push_back(_message);
  mQueueLock.unlock();
  ALooper_wake(mMainLooper);
  return true;
}

void WNAndroidEventPump::HandleWindowCommand(android_app* app, int32_t cmd) {
  WNAndroidEventPump* pump =
      reinterpret_cast<WNAndroidEventPump*>(app->userData);
  switch (cmd) {
    case APP_CMD_INIT_WINDOW:
      pump->mDisplayActive = true;
      wn::utilities::gWindowInitialized = true;
      pump->FireCallback(eDisplayCreated, 0, app);
      break;
    case APP_CMD_TERM_WINDOW:
      pump->mDisplayActive = true;
      pump->FireCallback(eDisplayDestroyed, 0, app);
      break;
  }
}

int32_t WNAndroidEventPump::HandleInputEvent(
    android_app* app, AInputEvent* _event) {
  WNAndroidEventPump* pump =
      reinterpret_cast<WNAndroidEventPump*>(app->userData);
  pump->FireInputCallback(_event);
  return 0;
}

void WNAndroidEventPump::PumpMessages(android_app* state) {
  if (!state) {
    return;
  }
  state->userData = this;
  state->onAppCmd = &HandleWindowCommand;
  state->onInputEvent = &HandleInputEvent;

  mMainLooper = ALooper_forThread();
  mStartedSemaphore.notify();

  for (;;) {
    int ret;
    int event;
    android_poll_source* source;
    while ((ret = ALooper_pollAll(1, NULL, &event, (void**)&source)) > 0) {
      if (source != NULL) {
        source->process(state, source);
      }
      if (state->destroyRequested != 0) {
        return;
      }
    }

    for (;;) {
      int msg = -1;
      mQueueLock.lock();
      if (!mMessageQueue.empty()) {
        msg = mMessageQueue.front();
        mMessageQueue.pop_front();
      } else {
        mQueueLock.unlock();
        break;
      }
      mQueueLock.unlock();
      switch (msg) {
        case eDisplayAdded:
          if (mDisplayActive) {
            FireCallback(eDisplayCreated, 0, state);
          }
          break;
        case eExit: {
          ANativeActivity_finish(state->activity);
          break;
        }
      }
    }
  }
}

}  // namespace utilities
}  // namespace wn
