// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "executable_entry/inc/executable_entry.h"
#include "WNCore/inc/types.h"
#include "WNUtilities/inc/WNAndroidEventPump.h"
#include "WNUtilities/inc/WNAppData.h"
#include "WNUtilities/inc/WNCrashHandler.h"
#include "WNUtilities/inc/WNLoggingData.h"
#include "executable_data/inc/executable_data.h"
#include "executable_data/inc/host_specific_data.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

#include <android/api-level.h>
#include <android/log.h>
#include <android_native_app_glue.h>
#include <sys/prctl.h>
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

extern int32_t wn_main(const wn::executable::executable_data* data);

namespace wn {
namespace executable {

void wn_dummy() {}

}  // namespace executable
}  // namespace wn

char* GetPackageName(struct android_app* state) {
  ANativeActivity* activity = state->activity;
  JNIEnv* env = 0;

  activity->vm->AttachCurrentThread(&env, 0);

  jclass cls = env->GetObjectClass(activity->clazz);
  jmethodID methodID =
      env->GetMethodID(cls, "getPackageName", "()Ljava/lang/String;");
  jobject result = env->CallObjectMethod(activity->clazz, methodID);

  char* tempstr;
  const char* str;
  jboolean isCopy;

  str = env->GetStringUTFChars((jstring)result, &isCopy);

  int newLen = strlen(str);

  tempstr = static_cast<char*>(malloc(sizeof(char) * newLen + 1));

  memcpy(tempstr, str, newLen);

  tempstr[newLen] = '\0';

  env->ReleaseStringUTFChars((jstring)result, str);
  activity->vm->DetachCurrentThread();

  return tempstr;
}

void* main_proxy_thread(void* _package_name) {
  wn::utilities::initialize_crash_handler();

  char* package_name = static_cast<char*>(_package_name);
  char* executable = nullptr;
  size_t path_length = strlen(package_name);

  for (size_t i = path_length; i != 0; --i) {
    if (package_name[i - 1] == '.') {
      executable = package_name + i;

      break;
    }
  }

  if (executable == nullptr) {
    executable = package_name;
  }

  wn::executable::host_specific_data host_data{wn::utilities::gAndroidApp,
      package_name, &wn::utilities::gWindowInitialized,
      &wn::utilities::WNAndroidEventPump::GetInstance()};
  wn::executable::executable_data executable_data{
      &host_data, executable, 1, &package_name};

  ::srand(static_cast<unsigned int>(::time(NULL)));

  const int32_t retVal = wn_main(&executable_data);

  __android_log_print(
      ANDROID_LOG_INFO, wn::utilities::gAndroidLogTag, "--FINISHED");
  __android_log_print(
      ANDROID_LOG_INFO, wn::utilities::gAndroidLogTag, "RETURN %d", retVal);

  wn::utilities::WNAndroidEventPump::GetInstance().KillMessagePump();

  return nullptr;
}

int main(int _argc, char* _argv[]) {
  const char* full_path;

  if (_argc > 0) {
    full_path = _argv[0];
  } else {
    full_path = "";
  }

  const char* executable = nullptr;
  size_t path_length = strlen(full_path);
  for (size_t i = path_length; i != 0; --i) {
    if (full_path[i - 1] == '/') {
      executable = full_path + i;
      break;
    }
  }
  if (!executable) {
    executable = full_path;
  }

  wn::executable::host_specific_data host_data{
      nullptr, executable, nullptr, nullptr};

  wn::executable::executable_data executable_data{
      &host_data, full_path, _argc, _argv};
  wn::utilities::gAndroidLogTag = _argv[0];

  ::srand(static_cast<unsigned int>(::time(NULL)));

  return wn_main(&executable_data);
}

void android_main(struct android_app* state) {
  char* packageName = GetPackageName(state);

  wn::utilities::gAndroidLogTag = packageName;
  wn::utilities::gAndroidApp = state;
  wn::utilities::gMainLooper = ALooper_forThread();

  app_dummy();

  __android_log_print(ANDROID_LOG_INFO, packageName, "--STARTED");

#if defined _WN_DEBUG
  FILE* debugFile = fopen("/sdcard/wait-for-debugger.txt", "r");

  if (debugFile) {
    __android_log_print(ANDROID_LOG_INFO, packageName, "--SLEEPING");

    sleep(10);  // sleep so that if we want to connect a debugger we can

    __android_log_print(ANDROID_LOG_INFO, packageName, "--WAKING UP");

    fclose(debugFile);
  } else {
    __android_log_print(ANDROID_LOG_INFO, packageName, "--NOT_DEBUGGING");
  }
#endif

  pthread_t mThread;

  pthread_create(&mThread, nullptr, main_proxy_thread, packageName);

  wn::utilities::WNAndroidEventPump::GetInstance().PumpMessages(state);

  pthread_join(mThread, nullptr);

  free(packageName);
  fclose(stdout);
}

#pragma GCC diagnostic pop
