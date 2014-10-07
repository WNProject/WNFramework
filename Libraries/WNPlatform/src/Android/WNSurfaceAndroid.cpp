////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "WNPlatform/inc/Internal/Android/WNSurfaceAndroid.h"
#include "WNPlatform/inc/Internal/Android/WNSurfaceManagerAndroid.h"
#include "WNUtils/inc/Android/WNAppData.h"

WNPlatform::WNSurfaceAndroid::WNSurfaceAndroid(WNPlatform::WNSurfaceManagerAndroid& _surfaceManager) :
    WNSurface(),
    mManager(_surfaceManager),
    mExiting(WN_FALSE),
    mInitialized(WN_FALSE) {
    mDisplay = EGL_NO_DISPLAY;
    mSurface = EGL_NO_SURFACE; 
    mConfig = 0;
}

WNPlatform::WNSurfaceAndroid::~WNSurfaceAndroid() {
    if(!mInitialized) {
        WNUtils::WNAndroidEventPump::GetInstance().SubscribeToEvent(WNUtils::WNAndroidEventPump::eDisplayCreated, NULL, NULL);
        WNUtils::WNAndroidEventPump::GetInstance().SubscribeToEvent(WNUtils::WNAndroidEventPump::eDisplayDestroyed, NULL, NULL);
    }
    mExiting = WN_TRUE;
    CleanupWindow(WNUtils::gAndroidApp);
    mManager.SurfaceDestroyed();
}

WN_BOOL WNPlatform::WNSurfaceAndroid::Initialize() {
    android_app* app = WNUtils::gAndroidApp;
    if(!app)
    {
        return(WN_FALSE);
    }

    WNUtils::WNAndroidEventPump::GetInstance().SubscribeToEvent(WNUtils::WNAndroidEventPump::eDisplayCreated, &WNSurfaceAndroid::HandleWindowCommand, this);
    WNUtils::WNAndroidEventPump::GetInstance().SubscribeToEvent(WNUtils::WNAndroidEventPump::eDisplayDestroyed, &WNSurfaceAndroid::HandleWindowCommand, this);
    mCreationSemaphore.Wait();
    mInitialized = (mSurface != EGL_NO_SURFACE);
    return(WN_TRUE);
}

WN_VOID WNPlatform::WNSurfaceAndroid::HandleWindowCommand(WNUtils::WNAndroidEventPump::eMessageType _msg, android_app* _app, WN_UINT32 _val, WN_VOID* appData) {
    WNPlatform::WNSurfaceAndroid* surface = reinterpret_cast<WNPlatform::WNSurfaceAndroid*>(appData);
    switch(_msg){
        case WNUtils::WNAndroidEventPump::eDisplayCreated:
            surface->InitializeWindow(_app);
            break;
        case WNUtils::WNAndroidEventPump::eDisplayDestroyed:
            surface->CleanupWindow(_app);
            break;
        default:
            break;
    }
}

WN_BOOL WNPlatform::WNSurfaceAndroid::InitializeWindow(android_app* state) {
    if(!mManager.InitializeDisplay(mDisplay, mConfig)) {
        mCreationSemaphore.Post();
        return(WN_FALSE);
    }
    
    EGLint format;
    if(EGL_FALSE == eglGetConfigAttrib(mDisplay, mConfig, EGL_NATIVE_VISUAL_ID, &format)) {
        mCreationSemaphore.Post();
        return(WN_FALSE);
    }
    if(0 > ANativeWindow_setBuffersGeometry(state->window, 0, 0, format)) {
        mCreationSemaphore.Post();
        return(WN_FALSE);
    }
    if(EGL_NO_SURFACE == (mSurface = eglCreateWindowSurface(mDisplay, mConfig, state->window, NULL))) {
        mCreationSemaphore.Post();
        return(WN_FALSE); 
    }
    
    EGLint w, h;
    eglQuerySurface(mDisplay, mSurface, EGL_WIDTH, &w);
    eglQuerySurface(mDisplay, mSurface, EGL_HEIGHT, &h);
    mWidth = w;
    mHeight = h;
    mCreationSemaphore.Post();
    return(WN_TRUE); 
}

WN_BOOL WNPlatform::WNSurfaceAndroid::CleanupWindow(android_app* state) {
    FireCallback(eWNSETDestroyed); //callback also not locked
    if(mDisplay != EGL_NO_DISPLAY) {
        eglMakeCurrent(mDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if(mSurface) {
            eglDestroySurface(mDisplay, mSurface);
        }
    }
    mSurface = EGL_NO_SURFACE;
    mDisplay = EGL_NO_DISPLAY;
    mConfig = WN_NULL;
    if(!mManager.DestroyDisplay()) {
       return(WN_FALSE); 
    }
    return(WN_TRUE);
}
WNPlatform::WNSurfaceNativeHandle WNPlatform::WNSurfaceAndroid::GetNativeHandle() const {
    return(mSurface);
}

WNPlatform::WNSurface::WNSurfaceError WNPlatform::WNSurfaceAndroid::Resize(WN_UINT32, WN_UINT32) {
    return(eWNOK);
}

WNPlatform::WNSurface::WNSurfaceError WNPlatform::WNSurfaceAndroid::Move(WN_UINT32, WN_UINT32) {
    return(eWNOK);
}

WN_BOOL WNPlatform::WNSurfaceAndroid::IsFullscreen() const {
    return(WN_TRUE);
}

WNPlatform::WNSurface::WNSurfaceError WNPlatform::WNSurfaceAndroid::SetFullscreen(WN_BOOL) {
    return(eWNOK);
}

WN_UINT32 WNPlatform::WNSurfaceAndroid::GetWidth() const {
    return(mWidth);
}

WN_UINT32 WNPlatform::WNSurfaceAndroid::GetHeight() const {
    return(mHeight);
}

WN_UINT32 WNPlatform::WNSurfaceAndroid::GetX() const {
    return(0);
}

WN_UINT32 WNPlatform::WNSurfaceAndroid::GetY() const {
    return(0);
}
