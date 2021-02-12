// Copyright (c) 2021, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <jni.h>

namespace wn {
namespace file_system {
namespace internal {

bool inline get_class_and_method(JNIEnv* _env, const char* _class_name,
    const char* _method_name, const char* _method_format, jclass& _jclass,
    jmethodID& _jmethodid) {
  jclass c = _env->FindClass(_class_name);

  if (c) {
    jmethodID m = _env->GetMethodID(c, _method_name, _method_format);

    if (m) {
      _jclass = core::move(c);
      _jmethodid = core::move(m);

      return true;
    }
  }

  return false;
}

}  // namespace internal
}  // namespace file_system
}  // namespace wn
