#ifndef __WN_UTILS_ANDROID_EVENT_PUMP_H__
#define __WN_UTILS_ANDROID_EVENT_PUMP_H__

#include "WNCore/inc/WNTypes.h"
#include "WNThreading/inc/WNSpinLock.h"
#include "WNThreading/inc/WNSemaphore.h"
#include <deque>
#include <android_native_app_glue.h>

namespace WNUtils {
    class WNAndroidEventPump {
    public:
        enum eMessageType {
            eDisplayCreated,
            eDisplayDestroyed,
            eMaxValue
        };
        typedef wn_void (*WNAndroidEventCallback)(eMessageType, android_app*, wn_uint32, wn_void*);
        wn_void PumpMessages(android_app* state); //returns when we tell it to return
        wn_void KillMessagePump();
        wn_void SubscribeToEvent(eMessageType _message, WNAndroidEventCallback _callback, wn_void* _userData);
        wn_void WaitForInit();
        static WNAndroidEventPump& GetInstance();
    private:
        enum eInternalMessage {
            eDisplayAdded,
            eExit
        };
        wn_bool PushMessage(eInternalMessage _message);
        wn_void FireCallback(eMessageType _eventi, wn_int32 _param, android_app* app);
        static wn_void HandleWindowCommand(android_app* app, int32_t cmd);
        WNAndroidEventPump();
        WNAndroidEventCallback mCallbacks[eMaxValue];
        wn_void* mCallbackData[eMaxValue];
        wn_bool mExiting;
        wn::threading::spin_lock mQueueLock;
        wn::threading::spin_lock mCallbackLock;
        std::deque<eInternalMessage> mMessageQueue;
        wn_bool mDisplayActive;
        ALooper* mMainLooper;
        wn::threading::semaphore mStartedSemaphore;
    };
};

#endif//__WN_UTILS_ANDROID_EVENT_PUMP_H__

