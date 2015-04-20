// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNPlatform/inc/Internal/XWindows/WNSurfaceManagerXWindows.h"
#include "WNPlatform/inc/Internal/XWindows/WNSurfaceXWindows.h"

using namespace wn;

WNSurfaceManagerReturnCode::type WNSurfaceManagerXWindows::Initialize() {
    XInitThreads();

    mDisplay = XOpenDisplay(NULL);

    if (!mDisplay) {
        return(WNSurfaceManagerReturnCode::eWNInitializationFailure);
    }

    mScreen = DefaultScreen(mDisplay);
    mRootWindow = RootWindow(mDisplay, mScreen);

    static int visual_attribs[] =
    {
        GLX_X_RENDERABLE, True,
        GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
        GLX_RENDER_TYPE, GLX_RGBA_BIT,
        GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
        GLX_RED_SIZE, 8,
        GLX_GREEN_SIZE, 8,
        GLX_BLUE_SIZE, 8,
        GLX_ALPHA_SIZE, 8,
        GLX_DEPTH_SIZE, 24,
        GLX_STENCIL_SIZE, 8,
        GLX_DOUBLEBUFFER, True,
//TODO: Try to create multisampled first, and if that fails use non MSAA.
//        GLX_SAMPLE_BUFFERS, 1,
//        GLX_SAMPLES, 1,
        None
    };

    int fbcount = 0;

    GLXFBConfig* fbc = glXChooseFBConfig(mDisplay, DefaultScreen(mDisplay), visual_attribs, &fbcount);

    if (!fbc) {
        return(WNSurfaceManagerReturnCode::eWNInitializationFailure);
    }

    mFBConfig = fbc[0];

    XFree(fbc);

    mVisualInfo = glXGetVisualFromFBConfig(mDisplay, mFBConfig);

    return(WNSurfaceManagerReturnCode::ok);
}

WNSurfaceManagerReturnCode::type WNSurfaceManagerXWindows::CreateSurface(wn_uint32 _x, wn_uint32 _y, wn_uint32 _width, wn_uint32 _height, memory::intrusive_ptr<surface>& _surface) {
  memory::intrusive_ptr<WNSurfaceXWindows> ptr = memory::make_intrusive<WNSurfaceXWindows, WNSurfaceManagerXWindows&>(*this);

    if (!ptr->Initialize(_x, _y, _width, _height, mDisplay, mVisualInfo)) {
        return(WNSurfaceManagerReturnCode::eWNInitializationFailure);
    }

    _surface = ptr;

    return(WNSurfaceManagerReturnCode::ok);
}

WNSurfaceManagerReturnCode::type WNSurfaceManagerXWindows::Release() {
    return(WNSurfaceManagerReturnCode::ok);
}

WNSurfaceManagerXWindows::WNSurfaceManagerXWindows() :
    mDisplay(NULL),
    mRootWindow(0) {
}
