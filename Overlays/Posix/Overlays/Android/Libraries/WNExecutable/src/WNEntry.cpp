// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNCore/inc/WNTypes.h"
#include "WNEntryPoint/inc/WNEntryData.h"
#include "WNUtilities/inc/WNAndroidEventPump.h"
#include "WNUtilities/inc/WNAppData.h"
#include "WNUtilities/inc/WNCrashHandler.h"
#include "WNUtilities/inc/WNLoggingData.h"

#include <android/log.h>
#include <android_native_app_glue.h>
#include <sys/prctl.h>
#include <unistd.h>

extern int32_t wn_main(
    const wn::entry::system_data* data, int32_t _argc, char* _argv[]);

void wn_dummy() {}

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

  return (tempstr);
}

void* main_proxy_thread(void* _package_name) {
  wn::utilities::initialize_crash_handler();

  char* package_name = static_cast<char*>(_package_name);
  char* executable = nullptr;
  size_t path_length = strlen(full_path);
  for (size_t i = path_length; i != 0; --i) {
    if (full_path[i - 1] == '.') {
      executable = full_path[i - 1] + 1;
      break;
    }
  }
  if (executable == nullptr) {
    executable = package_name;
  }

  wn::entry::host_specific_data host_data{
      wn::utilities::gAndroidApp, _package_name};
  wn::entry::system_data system_data{&host_data, executable};

  int32_t retVal = wn_main(&system_data, 1, &package_name);

  __android_log_print(
      ANDROID_LOG_INFO, wn::utilities::gAndroidLogTag, "--FINISHED");
  __android_log_print(
      ANDROID_LOG_INFO, wn::utilities::gAndroidLogTag, "RETURN %d", retVal);

  wn::utilities::WNAndroidEventPump::GetInstance().KillMessagePump();

  return (nullptr);
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
      executable = full_path[i - 1] + 1;
      break;
    }
  }
  if (!executable) {
    executable = full_path;
  }

  wn::entry::host_specific_data host_data{
      nullptr, executable,
  };

  wn::entry::system_data system_data {
    &host_data, full_path
  }

  wn::utilities::gAndroidLogTag = _argv[0];
  return wn_main(&system_data, _argc, _argv);
}

void android_main(struct android_app* state) {
  char* packageName = GetPackageName(state);

  wn::utilities::gAndroidLogTag = packageName;
  wn::utilities::gAndroidApp = state;
  wn::utilities::gMainLooper = ALooper_forThread();

  app_dummy();

  __android_log_print(ANDROID_LOG_INFO, packageName, "--STARTED");

#if defined _WN_DEBUG
  if (access("/proc/sys/kernel/yama/ptrace_scope", F_OK) != -1) {
    prctl(PR_SET_PTRACER, PR_SET_PTRACER_ANY, 0, 0, 0);
  }

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
