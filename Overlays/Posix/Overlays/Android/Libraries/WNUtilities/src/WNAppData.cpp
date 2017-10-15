#include "WNUtilities/inc/WNAppData.h"

#include <atomic>

namespace wn {
namespace utilities {

android_app* gAndroidApp = nullptr;
ALooper* gMainLooper = nullptr;
std::atomic<bool> gWindowInitialized;

}  // namespace utilities
}  // namespace wn
