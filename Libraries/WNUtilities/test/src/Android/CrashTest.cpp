#include "WNUtilities/inc/Android/WNLoggingData.h"
#include "WNUtilities/inc/WNCrashHandler.h"

#include <android/log.h>
#include <android_native_app_glue.h>
#include <unistd.h>

WN_CHAR* GetPackageName(struct android_app* state) {
  ANativeActivity* activity = state->activity;
  JNIEnv* env = 0;
  activity->vm->AttachCurrentThread(&env, 0);

  jclass cls = env->GetObjectClass(activity->clazz);
  jmethodID methodID =
      env->GetMethodID(cls, "getPackageName", "()Ljava/lang/String;");
  jobject result = env->CallObjectMethod(activity->clazz, methodID);

  WN_CHAR* tempstr;
  const WN_CHAR* str;
  jboolean isCopy;
  str = env->GetStringUTFChars((jstring)result, &isCopy);
  int newLen = strlen(str);
  tempstr = static_cast<WN_CHAR*>(malloc(sizeof(char) * newLen + 1));
  memcpy(tempstr, str, newLen);
  tempstr[newLen] = '\0';
  env->ReleaseStringUTFChars((jstring)result, str);
  activity->vm->DetachCurrentThread();
  return (tempstr);
}

int __attribute__((noinline)) teste() {
  int* x = 0;
  *x = 5;
  return (*x);
}
int __attribute__((noinline)) testd() {
  volatile int a = teste();
  return (a);
}
int __attribute__((noinline)) testc() {
  volatile int a = testd();
  return (a);
}
int __attribute__((noinline)) testb() {
  volatile int a = testc();
  return (a);
}
int __attribute__((noinline)) testa() {
  volatile int a = testb();
  return (a);
}

void android_main(struct android_app* state) {
  freopen("/sdcard/stdout.txt", "a", stdout);
  app_dummy();
  WN_CHAR* packageName = GetPackageName(state);
  wn::utilities::gAndroidLogTag = packageName;
  FILE* debugFile = fopen("/sdcard/waitdebugger.txt", "r");
  if (debugFile) {
    __android_log_print(ANDROID_LOG_INFO, packageName, "--SLEEPING");
    sleep(10);  // sleep so that if we want to connect a debugger we can
    __android_log_print(ANDROID_LOG_INFO, packageName, "--WAKING UP");
    fclose(debugFile);
  }

  wn::utilities::initialize_crash_handler();
  __android_log_print(ANDROID_LOG_INFO, packageName, "--STARTED");

  testa();  // this should crash
  __android_log_print(ANDROID_LOG_INFO, packageName, "--FINISHED");
  __android_log_print(ANDROID_LOG_INFO, packageName, "RETURN -1");
  // we want to make sure we HAVENT reached here, it means our crash handler was
  // not doing what it was supposed to
}
