#include "WNLogging/test/inc/Common.h"
#include "WNLogging/inc/WNLog.h"
#include "WNLogging/inc/WNFileLogger.h"
#include "WNLogging/inc/WNConsoleLogger.h"
#include <fcntl.h>

#ifdef _WN_ANDROID
    #define FILE_PREFIX "/sdcard/"
#else
    #define FILE_PREFIX
#endif

const char* sFileName = FILE_PREFIX "test" _WN_PLATFORM_NAME _WN_ARCH_NAME _WN_CONFIG_NAME ".txt";

wn_size_t NumWritten() {
    FILE * fp = fopen(sFileName, "r");
    if(!fp) {
       return(false);
    }
    fseek(fp, 0L, SEEK_END);
    int sz = ftell(fp);
    fclose(fp);
#ifdef _WN_WINDOWS
    if(::DeleteFile(sFileName) != TRUE)
    {
        return(false);
    }
#else
    unlink(sFileName);
#endif
    return(sz);
}

const wn_char* GetTempFile()
{
    return(sFileName);
}

using namespace WNLogging;
TEST(WNFileLogTest, Basiclogging) {
    {
        WNFileLogger<GetTempFile> mFileLogger;
        WNLog mDefaultLog = WNLog(&mFileLogger);
        mDefaultLog.Log(eError, 0, "asdf");
    }
    ASSERT_GE(NumWritten(), 1u);
}

TEST(WNFileLogTest, NoExessiveLogging) {
    {
        WNFileLogger<GetTempFile> mFileLogger;
        WNLog mDefaultLog(&mFileLogger);
        mDefaultLog.Log(eInfo, 0, "asdf");
    }
    ASSERT_EQ(NumWritten(), 0);
}

TEST(WNFileLogTest, FilledLog) {
    {
        WNFileLogger<GetTempFile> mFileLogger;
        WNLog mDefaultLog(&mFileLogger);
        for(int i = 0; i < 100; ++i){ //this should be at least asdf 100 times, but in reality
                                        //Error: asdf
            mDefaultLog.Log(eError, 0, "asdf");
        }
    }
    ASSERT_GE(NumWritten(), 400u);
}

TEST(WNFileLogTest, ChangeLogLevel) {
    {
        WNFileLogger<GetTempFile> mFileLogger;
        WNLog mDefaultLog(&mFileLogger);
        mDefaultLog.Log(eInfo, 0, "asdf");
        mDefaultLog.Flush();
    }
    ASSERT_EQ(NumWritten(), 0);
    {
        WNFileLogger<GetTempFile> mFileLogger;
        WNLog mDefaultLog(&mFileLogger);
        mDefaultLog.Log(eInfo, 0, "asdf");
        mDefaultLog.Flush();
        mDefaultLog.SetLogLevel(eInfo);
        mDefaultLog.Log(eInfo, 0, "asdf");
        mDefaultLog.Flush();
    }
    ASSERT_GE(NumWritten(), 1u);
}

