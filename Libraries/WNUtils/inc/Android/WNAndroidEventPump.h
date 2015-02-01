#ifndef __WN_UTILS_ANDROID_EVENT_PUMP_H__
#define __WN_UTILS_ANDROID_EVENT_PUMP_H__

#include "WNCore/inc/WNTypes.h"
#include "WNConcurrency/inc/WNSpinLock.h"
#include "WNConcurrency/inc/WNSemaphore.h"
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
        typedef WN_VOID (*WNAndroidEventCallback)(eMessageType, android_app*, WN_UINT32, WN_VOID*);
        WN_VOID PumpMessages(android_app* state); //returns when we tell it to return
        WN_VOID KillMessagePump();
        WN_VOID SubscribeToEvent(eMessageType _message, WNAndroidEventCallback _callback, WN_VOID* _userData);
        WN_VOID WaitForInit();
        static WNAndroidEventPump& GetInstance();
    private:
        enum eInternalMessage {
            eDisplayAdded,
            eExit
        };
        WN_BOOL PushMessage(eInternalMessage _message);
        WN_VOID FireCallback(eMessageType _eventi, WN_INT32 _param, android_app* app);
        static WN_VOID HandleWindowCommand(android_app* app, int32_t cmd);
        WNAndroidEventPump();
        WNAndroidEventCallback mCallbacks[eMaxValue];
        WN_VOID* mCallbackData[eMaxValue];
        WN_BOOL mExiting;
        WNConcurrency::WNSpinLock mQueueLock;
        WNConcurrency::WNSpinLock mCallbackLock;
        std::deque<eInternalMessage> mMessageQueue;
        WN_BOOL mDisplayActive;
        ALooper* mMainLooper;
        WNConcurrency::WNSemaphore mStartedSemaphore;
    };
};

#endif//__WN_UTILS_ANDROID_EVENT_PUMP_H__

