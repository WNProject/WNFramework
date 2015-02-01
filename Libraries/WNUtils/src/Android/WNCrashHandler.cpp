#include "WNUtils/inc/WNCrashHandler.h"
#include "WNUtils/inc/Android/WNLoggingData.h"
#include <pthread.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <android/log.h>
#define COUNT_OF(x) (sizeof(x) / sizeof(x[0]))

typedef struct {
    uintptr_t absolute_pc;
    uintptr_t stack_top;
    size_t stack_size;
} backtrace_frame_t;

typedef struct {
    uintptr_t relative_pc;
    uintptr_t relative_symbol_addr;
    char* map_name;
    char* symbol_name;
    char* demangled_name; 
} backtrace_symbol_t;


struct map_info_t;

typedef map_info_t* (*acquire_my_map_info_list_func)();
typedef void (*release_my_map_info_list_func)(map_info_t*);
typedef ssize_t (*unwind_backtrace_signal_arch_func)(siginfo_t*, void*, const map_info_t*, backtrace_frame_t*, size_t, size_t);
typedef void (*get_backtrace_symbols_func)(const backtrace_frame_t*, size_t, backtrace_symbol_t*);
typedef void (*free_backtrace_symbols_func)(backtrace_symbol_t*, size_t);

const int signalsToCatch[] = {
    SIGSEGV,
    SIGILL
};


struct StackUnwinder {
    StackUnwinder() {
        mInitialized = false;
        pthread_mutex_init(&mCrashMutex, NULL);
        for(int i = 0; i < COUNT_OF(signalsToCatch); ++i) {
            struct sigaction sigAct;
            sigAct.sa_sigaction = error_func;
            sigAct.sa_flags = SA_RESTART | SA_SIGINFO; //we want SA_RESTART otherwise all semaphores will fail when we hit this
            sigaction(signalsToCatch[i], &sigAct, &mOldSignals[i]);
        }
        pthread_key_create(&mProcessingKey, NULL);
    }
    static void error_func(int signo, siginfo_t* info, void* context);
    wn_bool InitializeHandler() {
        void* const lib = dlopen("libcorkscrew.so", RTLD_LAZY|RTLD_LOCAL); 
        if(!lib) { //libcorkscrew does not exist on this machine
            return(wn_false);
        }
        acquire_my_map_info_list = reinterpret_cast<acquire_my_map_info_list_func>(dlsym(lib, "acquire_my_map_info_list"));
        release_my_map_info_list = reinterpret_cast<release_my_map_info_list_func>(dlsym(lib, "release_my_map_info_list"));
        unwind_backtrace_signal_arch = reinterpret_cast<unwind_backtrace_signal_arch_func>(dlsym(lib, "unwind_backtrace_signal_arch"));
        get_backtrace_symbols = reinterpret_cast<get_backtrace_symbols_func>(dlsym(lib, "get_backtrace_symbols"));
        free_backtrace_symbols = reinterpret_cast<free_backtrace_symbols_func>(dlsym(lib, "free_backtrace_symbols"));
        return(acquire_my_map_info_list && release_my_map_info_list && unwind_backtrace_signal_arch && get_backtrace_symbols && free_backtrace_symbols);
    }
    acquire_my_map_info_list_func acquire_my_map_info_list;
    release_my_map_info_list_func release_my_map_info_list;
    unwind_backtrace_signal_arch_func unwind_backtrace_signal_arch;
    get_backtrace_symbols_func get_backtrace_symbols;
    free_backtrace_symbols_func free_backtrace_symbols;
    pthread_mutex_t mCrashMutex;
    struct sigaction mOldSignals[COUNT_OF(signalsToCatch)];
    pthread_key_t mProcessingKey;
    bool mInitialized;

};

static StackUnwinder* g_CrashHandler = NULL;
#define MAX_FRAMES 64
void StackUnwinder::error_func(int sig, siginfo_t* info, void* context) {
    if(reinterpret_cast<size_t>(pthread_getspecific(g_CrashHandler->mProcessingKey)) == 1) { //If we crashed in our crash handler, give up, its not worth it
        for(int i = 0; i < COUNT_OF(signalsToCatch); ++i) { 
            if(signalsToCatch[i] == sig) {
                g_CrashHandler->mOldSignals[i].sa_sigaction(sig, info, context);
            }
        }
    }
    pthread_mutex_lock(&g_CrashHandler->mCrashMutex);
    pthread_setspecific(g_CrashHandler->mProcessingKey, reinterpret_cast<void*>(1));
    if(g_CrashHandler->InitializeHandler()) {
        int file = open("/sdcard/crash", O_CREAT | O_APPEND | O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH );
        map_info_t* mapInfo = g_CrashHandler->acquire_my_map_info_list();
        backtrace_frame_t frames[MAX_FRAMES];
        backtrace_symbol_t symbols[MAX_FRAMES];
        ssize_t numFrames = g_CrashHandler->unwind_backtrace_signal_arch(info, context, mapInfo, frames, 1, MAX_FRAMES);
        g_CrashHandler->get_backtrace_symbols(frames, numFrames, symbols);
        for(int i = 0; i < numFrames; ++i) {
            for(int j = sizeof(void*)*2-1; j >= 0; --j) {
                unsigned char x = 0xF & ( reinterpret_cast<size_t>(symbols[i].relative_pc) >> (j* 4));
                const char* val = "0";
#define CASE(x) case 0x##x: val = #x; break;
                switch(x) {
                    CASE(0)
                    CASE(1)
                    CASE(2)
                    CASE(3)
                    CASE(4)
                    CASE(5)
                    CASE(6)
                    CASE(7)
                    CASE(8)
                    CASE(9)
                    CASE(A)
                    CASE(B)
                    CASE(C)
                    CASE(D)
                    CASE(E)
                    CASE(F)
                }
                ssize_t throwaway = write(file, val, 1);//don't really care if it fails, keep going
            }
            ssize_t throwaway = write(file, "\n", 1);
        }
        close(file);
        g_CrashHandler->free_backtrace_symbols(symbols, numFrames);
        g_CrashHandler->release_my_map_info_list(mapInfo);
    }

    pthread_setspecific(g_CrashHandler->mProcessingKey, reinterpret_cast<void*>(0));
    if(WNUtils::gAndroidLogTag != NULL) {
        __android_log_print(ANDROID_LOG_FATAL, WNUtils::gAndroidLogTag, "--CRASHED--");
    } else {
        __android_log_print(ANDROID_LOG_FATAL, "APP", "--CRASHED--");
    }
    pthread_mutex_unlock(&g_CrashHandler->mCrashMutex);
    for(int i = 0; i < COUNT_OF(signalsToCatch); ++i) {
        if(signalsToCatch[i] == sig) {
            g_CrashHandler->mOldSignals[i].sa_sigaction(sig, info, context);
        }
    }
}

wn_void WNUtils::InitializeCrashHandler() {
    g_CrashHandler = new(StackUnwinder);
}
