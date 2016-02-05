#include "WNUtils/inc/WNCrashHandler.h"

#include <fcntl.h>
#include <pthread.h>
#include <execinfo.h>
#include <unistd.h>
#include <errno.h>
#include <cstdio>

#define COUNT_OF(x) (sizeof(x) / sizeof(x[0]))

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
        int file = open("crash", O_CREAT | O_APPEND | O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH );
        void* backtraceValues[MAX_FRAMES];
        int numTraces = backtrace(backtraceValues, MAX_FRAMES);
        for(int i= 0; i < numTraces; ++i) {
            for(int j = sizeof(void*)*2-1; j >= 0; --j) {
                unsigned char x = 0xF & ( reinterpret_cast<size_t>(backtraceValues[i]) >> (j* 4));
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

    puts("--CRASHED");
    pthread_setspecific(g_CrashHandler->mProcessingKey, reinterpret_cast<void*>(0));
    pthread_mutex_unlock(&g_CrashHandler->mCrashMutex);
    for(int i = 0; i < COUNT_OF(signalsToCatch); ++i) {
        if(signalsToCatch[i] == sig) {
            g_CrashHandler->mOldSignals[i].sa_sigaction(sig, info, context);
        }
    }
}

void WNUtils::InitializeCrashHandler() {
    g_CrashHandler = new(StackUnwinder);
}
