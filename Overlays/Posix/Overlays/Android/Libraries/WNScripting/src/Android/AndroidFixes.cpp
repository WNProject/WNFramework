// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

// Android does not have log2 support pre version 18.
// LLVM does not detect this properly.
// When llvm DOES detect this, if uses __builtin_log2l.
// __builtin_log2l does not work in gcc only clang.

#if ANDROID_NATIVE_API_LEVEL && ANDROID_NATIVE_API_LEVEL < 18
#include <math.h>
extern "C" {
double log2(double x) {
  return log(x) / log(2.0);
}

#ifndef __clang__
long double log2l(long double x) {
  return log((double)x) / log(2.0);
}
#endif
}
#endif
