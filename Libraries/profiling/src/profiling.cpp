#include "profiling/inc/profiling.h"

namespace wn {
namespace profiling {

FrameMarker::FrameMarker(const char* _tag) : m_tag(_tag) {
  FrameMarkStart(m_tag);
}

FrameMarker::~FrameMarker() {
  FrameMarkEnd(m_tag);
}

bool is_profiling_enabled() {
#ifdef TRACY_ENABLE
  return true;
#else
  return false;
#endif
}

}  // namespace profiling
}  // namespace wn
