#include "WNUtils/inc/Android/WNAndroidEventPump.h"

WNUtils::WNAndroidEventPump& WNUtils::WNAndroidEventPump::GetInstance() {
    static WNAndroidEventPump sPump;
    return(sPump);
}

WNUtils::WNAndroidEventPump::WNAndroidEventPump(): mExiting(wn_false), mDisplayActive(wn_false) {
    memset(mCallbacks, 0, sizeof(mCallbacks));
    memset(mCallbackData, 0, sizeof(mCallbackData));
    mMainLooper = NULL;
}

wn_void WNUtils::WNAndroidEventPump::FireCallback(eMessageType _event, wn_int32 _param, android_app* _state) {
    mCallbackLock.Lock();
    if(mCallbacks[_event] != 0) {
        mCallbacks[_event](_event, _state, _param, mCallbackData[_event]);
    }
    mCallbackLock.Unlock();
}

wn_void WNUtils::WNAndroidEventPump::SubscribeToEvent(eMessageType _message, WNAndroidEventCallback _callback, wn_void* _userData) {
    mCallbackLock.Lock();
    mCallbacks[_message] = _callback;
    mCallbackData[_message] = _userData;
    if(_message == eDisplayCreated) {
        PushMessage(eDisplayAdded);
    }
    mCallbackLock.Unlock();
}

wn_void WNUtils::WNAndroidEventPump::KillMessagePump() {
    PushMessage(eExit);
}

wn_void WNUtils::WNAndroidEventPump::WaitForInit() {
    mStartedSemaphore.Wait();
    mStartedSemaphore.Post();
}

wn_bool WNUtils::WNAndroidEventPump::PushMessage(eInternalMessage _message) {
    mQueueLock.Lock();
    if(mExiting) {
        return(wn_false);
    }

    if(_message == eExit) {
        mExiting = true;
    }
    mMessageQueue.push_back(_message);
    mQueueLock.Unlock();
    ALooper_wake(mMainLooper);
    return(wn_true);
}

wn_void WNUtils::WNAndroidEventPump::HandleWindowCommand(android_app* app, int32_t cmd) {
    WNAndroidEventPump* pump = reinterpret_cast<WNAndroidEventPump*>(app->userData);
    switch(cmd) {
        case APP_CMD_INIT_WINDOW:
            pump->mDisplayActive = wn_true;
            pump->FireCallback(eDisplayCreated, 0, app);
            break;
        case APP_CMD_TERM_WINDOW:
            pump->mDisplayActive = wn_true;
            pump->FireCallback(eDisplayDestroyed, 0, app);
            break;
    }
}

wn_void WNUtils::WNAndroidEventPump::PumpMessages(android_app* state) {
    if(!state) {
        return;
    }
    state->userData = this;
    state->onAppCmd = &HandleWindowCommand;

    mMainLooper = ALooper_forThread();
    mStartedSemaphore.Post();

    for(;;) {
        int ret;
        int event;
        android_poll_source* source;
        while((ret = ALooper_pollAll(1, NULL, &event, (void**)&source)) > 0) {
            if(source != NULL) {
                source->process(state, source);
            }
            if(state->destroyRequested != 0) {
                return;
            }
        }

        for(;;) {
            int msg = -1;
            mQueueLock.Lock();
            if(!mMessageQueue.empty()) {
                msg = mMessageQueue.front();
                mMessageQueue.pop_front();
            } else {
                mQueueLock.Unlock();
                break;
            }
            mQueueLock.Unlock();
            switch(msg) {
                case eDisplayAdded:
                    if(mDisplayActive) {
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
