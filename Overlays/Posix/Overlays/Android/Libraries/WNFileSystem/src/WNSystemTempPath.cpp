// Copyright (c) 2021, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNFileSystem/src/WNSystemTempPath.h"

#include "WNContainers/inc/WNString.h"
#include "WNFileSystem/inc/jni_helper.h"
#include "WNFileSystem/src/WNSystemPaths.h"
#include "WNFileSystem/src/WNUtilities.h"
#include "WNMemory/inc/allocator.h"
#include "WNMultiTasking/inc/job_pool.h"
#include "core/inc/types.h"
#include "executable_data/inc/executable_data.h"
#include "executable_data/inc/host_specific_data.h"

#include <jni.h>

namespace wn {
namespace file_system {
namespace internal {

containers::string get_temp_path(memory::allocator* _allocator,
    const executable::executable_data* _executable_data) {
  android_app* app = _executable_data->host_data->android_app;
  auto job_pool = multi_tasking::job_pool::this_job_pool();

  if (!app || !job_pool) {
    return containers::string(_allocator, "/data/local/tmp/");
  }

  containers::string path(_allocator);
  job_pool->call_blocking_function("GET_DEV_ASSETS",
      functional::function<void()>(_allocator, [&path, app]() {
        const ANativeActivity* activity = app->activity;

        if (!activity) {
          return;
        }

        JavaVM* vm = activity->vm;

        if (!vm) {
          return;
        }

        JNIEnv* env;

        if (vm->AttachCurrentThread(&env, nullptr) == JNI_OK) {
          jclass activity_class;
          jmethodID get_cache_dir_method;

          if (get_class_and_method(env, "android/app/NativeActivity",
                  "getCacheDir", "()Ljava/io/File;", activity_class,
                  get_cache_dir_method)) {
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
      }));

  return core::move(path);
}

}  // namespace internal
}  // namespace file_system
}  // namespace wn
