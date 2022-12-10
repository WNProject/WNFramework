// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "executable_entry/inc/executable_entry.h"

#include "WNUtilities/inc/WNAndroidEventPump.h"
#include "WNUtilities/inc/WNAppData.h"
#include "WNUtilities/inc/WNCrashHandler.h"
#include "WNUtilities/inc/WNLoggingData.h"
#include "core/inc/types.h"
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
#include <vector>

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

std::string getExternalFilesDir(struct android_app* state) {
  ANativeActivity* activity = state->activity;
  JNIEnv* env;
  activity->vm->AttachCurrentThread(&env, NULL);

  // getExternalFilesDir() - java
  jclass cls_Env = env->FindClass("android/app/NativeActivity");
  jmethodID mid = env->GetMethodID(
      cls_Env, "getExternalFilesDir", "(Ljava/lang/String;)Ljava/io/File;");
  jobject obj_File = env->CallObjectMethod(activity->clazz, mid, NULL);
  jclass cls_File = env->FindClass("java/io/File");
  jmethodID mid_getPath =
      env->GetMethodID(cls_File, "getPath", "()Ljava/lang/String;");
  jstring obj_Path = (jstring)env->CallObjectMethod(obj_File, mid_getPath);

  char* tempstr;
  const char* str;
  jboolean isCopy;

  str = env->GetStringUTFChars((jstring)obj_Path, &isCopy);

  int newLen = strlen(str);

  tempstr = static_cast<char*>(malloc(sizeof(char) * newLen + 1));

  memcpy(tempstr, str, newLen);

  tempstr[newLen] = '\0';

  env->ReleaseStringUTFChars((jstring)obj_Path, str);
  activity->vm->DetachCurrentThread();

  std::string ret = tempstr;
  free(tempstr);
  return ret;
}

std::vector<char*> getArgs(struct android_app* state) {
  std::vector<char*> args;
  args.push_back(GetPackageName(state));

  ANativeActivity* activity = state->activity;
  JNIEnv* env = 0;

  activity->vm->AttachCurrentThread(&env, 0);

  jclass cls = env->GetObjectClass(activity->clazz);
  jmethodID methodID =
      env->GetMethodID(cls, "getArgs", "()[Ljava/lang/String;");
  jobjectArray result =
      (jobjectArray)env->CallObjectMethod(activity->clazz, methodID);
  if (result) {
    int stringCount = env->GetArrayLength(result);
    for (int i = 0; i < stringCount; ++i) {
      char* tempstr;
      jboolean isCopy;
      jstring js = (jstring)(env->GetObjectArrayElement(result, i));
      const char* str = env->GetStringUTFChars(js, &isCopy);
      int newLen = strlen(str);
      tempstr = static_cast<char*>(malloc(sizeof(char) * newLen + 1));

      memcpy(tempstr, str, newLen);

      tempstr[newLen] = '\0';
      env->ReleaseStringUTFChars(js, str);
      activity->vm->DetachCurrentThread();
      args.push_back(tempstr);
    }
  }

  return args;
}

void* main_proxy_thread(void* _user_data) {
  std::vector<char*>* args = static_cast<std::vector<char*>*>(_user_data);
  wn::utilities::initialize_crash_handler();

  char* package_name = static_cast<char*>((*args)[0]);
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
      &host_data, executable, static_cast<int32_t>(args->size()), args->data()};

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
  auto args = getArgs(state);

  wn::utilities::gAndroidLogTag = args[0];
  wn::utilities::gAndroidApp = state;
  wn::utilities::gMainLooper = ALooper_forThread();

  app_dummy();

  __android_log_print(ANDROID_LOG_INFO, args[0], "--STARTED");

#if defined _WN_DEBUG
  if (access("/proc/sys/kernel/yama/ptrace_scope", F_OK) != -1) {
    prctl(PR_SET_PTRACER, PR_SET_PTRACER_ANY, 0, 0, 0);
  }
  std::string fd = getExternalFilesDir(state);
  __android_log_print(ANDROID_LOG_INFO, args[0], "--Looking for %s",
      (fd + "/wait-for-debugger.txt").c_str());
  if (access((fd + "/wait-for-debugger.txt").c_str(), F_OK) != -1) {
    __android_log_print(ANDROID_LOG_INFO, args[0], "--SLEEPING");

    sleep(20);  // sleep so that if we want to connect a debugger we can

    __android_log_print(ANDROID_LOG_INFO, args[0], "--WAKING UP");
  } else {
    __android_log_print(ANDROID_LOG_INFO, args[0], "--NOT_DEBUGGING");
  }
#endif

  pthread_t mThread;

  pthread_create(&mThread, nullptr, main_proxy_thread, &args);

  wn::utilities::WNAndroidEventPump::GetInstance().PumpMessages(state);

  pthread_join(mThread, nullptr);

  for (size_t i = 1; i < args.size(); ++i) {
    free(args[i]);
  }
  fclose(stdout);
}

#pragma GCC diagnostic pop
