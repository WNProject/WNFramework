#ifndef __WN_UTILITIES_APP_DATA_H__
#define __WN_UTILITIES_APP_DATA_H__

#include "WNCore/inc/WNTypes.h"

#include <android_native_app_glue.h>

namespace wn {
namespace utilities {

extern android_app* gAndroidApp;
extern ALooper* gMainLooper;

}  // namespace utilities
}  // namespace wn

#endif  //__WN_UTILITIES_APP_DATA_H__
