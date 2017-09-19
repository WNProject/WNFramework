// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNFileSystem/src/WNSystemPaths.h"
#include "WNMemory/inc/WNAllocator.h"
#include "WNUtilities/inc/WNAppData.h"

#include <jni.h>
#include <sys/stat.h>
#include <time.h>

namespace wn {
namespace file_system {
namespace internal {
namespace {

WN_INLINE bool get_class_and_method(JNIEnv* _env,
    const containers::string& _class_name,
    const containers::string& _method_name,
    const containers::string& _method_format, jclass& _jclass,
    jmethodID& _jmethodid) {
  jclass c = _env->FindClass(_class_name.c_str());

  if (c) {
    jmethodID m =
        _env->GetMethodID(c, _method_name.c_str(), _method_format.c_str());

    if (m) {
      _jclass = core::move(c);
      _jmethodid = core::move(m);

      return true;
    }
  }

  return false;
}

containers::string get_temp_path(memory::allocator* _allocator) {
  android_app* app = utilities::gAndroidApp;

  if (!app) {
    return false;
  }

  const ANativeActivity* activity = app->activity;

  if (!activity) {
    return false;
  }

  JavaVM* vm = activity->vm;

  if (!vm) {
    return false;
  }

  containers::string path(_allocator);
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
              path = path_chars;

              env->ReleaseStringUTFChars(path_string, path_chars);
            }
          }
        }
      }
    }

    vm->DetachCurrentThread();
  }

  return core::move(path);
}

}  // anonymous namespace

containers::string get_scratch_path(memory::allocator* _allocator) {
  containers::string temp_path(get_temp_path(_allocator));

  if (!temp_path.empty()) {
    char name_template[7] = {0};

    for (;;) {
      memory::strncpy(name_template, "XXXXXX", 6);

      if (::mktemp(name_template)) {
        containers::string path(temp_path + "scratch." + name_template + "/");
        static const mode_t mode = S_IRWXU | S_IRWXG;

        if (::mkdir(path.data(), mode) == 0) {
          return path;
        } else if (errno == EEXIST) {
          continue;
        }
      }

      break;
    }
  }

  return nullptr;
}

}  // namespace internal
}  // namespace file_system
}  // namespace wn
