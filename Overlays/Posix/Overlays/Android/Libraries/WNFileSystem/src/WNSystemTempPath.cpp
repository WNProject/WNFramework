// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNFileSystem/src/WNSystemTempPath.h"
#include "WNContainers/inc/WNString.h"
#include "WNCore/inc/WNTypes.h"
#include "WNExecutable/inc/WNEntry.h"
#include "WNExecutable/inc/WNEntryData.h"
#include "WNFileSystem/src/WNSystemPaths.h"
#include "WNFileSystem/src/WNUtilities.h"
#include "WNMemory/inc/WNAllocator.h"

#include <jni.h>

namespace wn {
namespace file_system {
namespace internal {
namespace {

bool get_class_and_method(JNIEnv* _env, const char* _class_name,
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

}  // namespace

containers::string get_temp_path(
    memory::allocator* _allocator, const entry::system_data* _system_data) {
  android_app* app = _system_data->host_data->android_app;

  if (!app) {
    return containers::string(_allocator, "/data/local/tmp/");
  }

  containers::string path(_allocator);
  const ANativeActivity* activity = app->activity;

  if (!activity) {
    return nullptr;
  }

  JavaVM* vm = activity->vm;

  if (!vm) {
    return nullptr;
  }

  JNIEnv* env;

  if (vm->AttachCurrentThread(&env, nullptr) == JNI_OK) {
    jclass activity_class;
    jmethodID get_cache_dir_method;

    if (get_class_and_method(env, "android/app/NativeActivity", "getCacheDir",
            "()Ljava/io/File;", activity_class, get_cache_dir_method)) {
      jobject cache_dir =
          env->CallObjectMethod(activity->clazz, get_cache_dir_method);

      if (cache_dir) {
        jclass file_class;
        jmethodID get_path_method;

        if (get_class_and_method(env, "java/io/File", "getPath",
                "()Ljava/lang/String;", file_class, get_path_method)) {
          jstring path_string = reinterpret_cast<jstring>(
              env->CallObjectMethod(cache_dir, get_path_method));

          if (path_string) {
            const char* path_chars =
                env->GetStringUTFChars(path_string, nullptr);

            if (path_chars) {
              path += path_chars;

              env->ReleaseStringUTFChars(path_string, path_chars);

              if (!path.empty() && path.back() != '/') {
                path += '/';
              }

              internal::sanitize_path(path);
            }
          }
        }
      }
    }

    vm->DetachCurrentThread();
  }

  return core::move(path);
}

}  // namespace internal
}  // namespace file_system
}  // namespace wn