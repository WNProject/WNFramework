#ifndef __WN_APP_DATA_H__
#define __WN_APP_DATA_H__
#include "WNCore/inc/WNTypes.h"
#include <android_native_app_glue.h>

namespace WNUtils {
   extern android_app* gAndroidApp;
   extern ALooper* gMainLooper;
}

#endif//__WN_APP_DATA_H__
