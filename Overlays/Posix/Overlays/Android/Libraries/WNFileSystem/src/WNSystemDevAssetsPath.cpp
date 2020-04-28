// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNContainers/inc/WNString.h"
#include "WNCore/inc/types.h"
#include "WNFileSystem/inc/jni_helper.h"
#include "WNFileSystem/src/WNSystemPaths.h"
#include "WNFileSystem/src/WNSystemTempPath.h"
#include "WNFileSystem/src/WNUtilities.h"
#include "WNMemory/inc/allocator.h"
#include "executable_data/inc/executable_data.h"
#include "executable_data/inc/host_specific_data.h"

#include <jni.h>

namespace wn {
namespace file_system {
namespace internal {

containers::string get_dev_assets_path(memory::allocator* _allocator,
    const executable::executable_data* _executable_data) {
  android_app* app = _executable_data->host_data->android_app;

  if (!app) {
    return get_current_working_path(_allocator, _executable_data);
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

    if (get_class_and_method(env, "android/app/NativeActivity",
            "getExternalFilesDir", "(Ljava/lang/String;)Ljava/io/File;",
            activity_class, get_cache_dir_method)) {
      jobject cache_dir = env->CallObjectMethod(
          activity->clazz, get_cache_dir_method, jstring(nullptr));

      if (cache_dir) {
        jclass file_class;
        jmethodID get_path_method;

        if (get_class_and_method(env, "java/io/File", "getPath",
                "()Ljava/lang/String;", file_class, get_path_method)) {
          jstring path_string = reinterpret_cast<jstring>(
              env->CallObjectMethod(cache_dir, get_path_method, nullptr));

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
