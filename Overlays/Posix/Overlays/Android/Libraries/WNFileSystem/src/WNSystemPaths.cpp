// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNFileSystem/src/WNSystemPaths.h"
#include "WNMemory/inc/WNAllocator.h"
#include "WNUtilities/inc/WNAppData.h"

#include <errno.h>
#include <jni.h>
#include <limits.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

namespace wn {
namespace file_system {
namespace internal {
namespace {

WN_INLINE bool get_class_and_method(JNIEnv* _env, const char* _class_name,
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

containers::string get_temp_path(memory::allocator* _allocator) {
  android_app* app = utilities::gAndroidApp;

  if (!app) {
    return nullptr;
  }

  const ANativeActivity* activity = app->activity;

  if (!activity) {
    return nullptr;
  }

  JavaVM* vm = activity->vm;

  if (!vm) {
    return nullptr;
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
    for (;;) {
      char name_template[9] = {0};
      uint32_t t = ::rand() % 100000000;
      int pos = 0;

      while (t) {
        name_template[++pos] = (t % 10 + '0');
        t -= t % 10;
        t /= 10;
      }

      containers::string path(temp_path + "scratch." + name_template + "/");
      static const mode_t mode = S_IRWXU | S_IRWXG;

      if (::mkdir(path.data(), mode) == 0) {
        return core::move(path);
      } else if (errno == EEXIST) {
        continue;
      }

      break;
    }
  }

  return nullptr;
}

containers::string get_executable_path(memory::allocator* _allocator) {
  return get_current_working_path(_allocator);
}

containers::string get_current_working_path(memory::allocator* _allocator) {
  containers::dynamic_array<char> buffer(_allocator);
  size_t size = static_cast<size_t>(PATH_MAX);

  for (;;) {
    buffer.resize(size);

    const char* result = ::getcwd(buffer.data(), buffer.size());

    if (result == NULL) {
      if (errno == ERANGE) {
        size *= 2;
      } else {
        return nullptr;
      }
    } else if (*result == '(' || *result != '/') {
      return nullptr;
    } else {
      return containers::string(_allocator, buffer.data());
    }
  }
}

}  // namespace internal
}  // namespace file_system
}  // namespace wn
