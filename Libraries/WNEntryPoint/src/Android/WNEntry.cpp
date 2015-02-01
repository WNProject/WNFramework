#include "WNCore/inc/WNTypes.h"
#include "WNUtils/inc/Android/WNLoggingData.h"
#include "WNUtils/inc/Android/WNAppData.h"
#include "WNUtils/inc/Android/WNAndroidEventPump.h"
#include "WNUtils/inc/WNCrashHandler.h"

#include <android_native_app_glue.h>
#include <unistd.h>
#include <android/log.h>

extern WN_INT32 WNMain(WN_INT32 _argc, WN_CHAR* _argv[]);

WN_CHAR* GetPackageName(struct android_app* state) {
    ANativeActivity* activity = state->activity;
    JNIEnv* env = 0;
	
    activity->vm->AttachCurrentThread(&env, 0);

    jclass cls = env->GetObjectClass(activity->clazz);
    jmethodID methodID = env->GetMethodID(cls, "getPackageName", "()Ljava/lang/String;");
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
	
    return(tempstr);
}

void* main_proxy_thread(void*) {
	WNUtils::InitializeCrashHandler();

	WN_INT32 retVal = WNMain(0, NULL);

	__android_log_print(ANDROID_LOG_INFO, WNUtils::gAndroidLogTag, "--FINISHED");
	__android_log_print(ANDROID_LOG_INFO, WNUtils::gAndroidLogTag, "RETURN %d", retVal);

	WNUtils::WNAndroidEventPump::GetInstance().KillMessagePump();

	return(NULL);
}

void android_main(struct android_app* state)
{
    freopen("/sdcard/stdout.txt", "a", stdout);

    WN_CHAR* packageName = GetPackageName(state);

    WNUtils::gAndroidLogTag = packageName;
    WNUtils::gAndroidApp = state;
    WNUtils::gMainLooper = ALooper_forThread();

    app_dummy();

    __android_log_print(ANDROID_LOG_INFO, packageName, "--STARTED");

	#if defined _WN_DEBUG 
		FILE* debugFile = fopen("/sdcard/waitdebugger.txt", "r");

		if (debugFile)
		{
			__android_log_print(ANDROID_LOG_INFO, packageName, "--SLEEPING");

			sleep(10); //sleep so that if we want to connect a debugger we can

			__android_log_print(ANDROID_LOG_INFO, packageName, "--WAKING UP");

			fclose(debugFile);
		}
	#endif

    pthread_t mThread;

    pthread_create(&mThread, NULL, main_proxy_thread, NULL);

    WNUtils::WNAndroidEventPump::GetInstance().PumpMessages(state); 

    pthread_join(mThread, NULL);

    free(packageName); 
    fclose(stdout);    
}
