// Copyright (c) 2020, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once
#include "Tracy.hpp"

#ifndef __WN_PROFILING_PROFILE_H__
#define __WN_PROFILING_PROFILE_H__

namespace wn {
namespace profiling {

struct FrameMarker {
  FrameMarker(const char* _tag);
  ~FrameMarker();
  const char* m_tag;
};

extern bool is_profiling_enabled();

#if WN_TRACY_CALLBACK_DEPTH > 0
#define PROFILE_REGION(n) ZoneNamedS(n, WN_TRACY_CALLBACK_DEPTH, true)
#define PROFILE_MESSAGE(c, m, s)                                               \
  TracyMessageCS(m, s, c, WN_TRACY_CALLBACK_DEPTH)
#else
#define PROFILE_REGION(n) ZoneNamed(n, true)
#define PROFILE_MESSAGE(c, m, s) TracyMessageC(m, s, c)
#endif

}  // namespace profiling
}  // namespace wn

#endif  // __WN_PROFILING_PROFILE_H__