#ifndef __RE2_HELPERS_RE2_H__
#define __RE2_HELPERS_RE2_H__

#include "../../re2/re2/re2.h"
#ifdef __ANDROID__
#ifndef _LIBCPP_VERSION
#include <sstream>

namespace std {

template <typename T>
std::string to_string(const T& t) {
  std::stringstream ss;
  ss << t;
  return ss.str();
}
}
#endif  // _LIBCPP_VERSION
#endif  // __ANDROID__

#endif  // __RE2_HELPERS_RE2_H__