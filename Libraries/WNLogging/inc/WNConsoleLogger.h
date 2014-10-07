////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __WN_CONSOLE_LOGGER_H__
#define __WN_CONSOLE_LOGGER_H__
#include "WNCore/inc/WNBase.h"
#include "WNLogging/inc/WNLog.h"
#include <vector>
namespace WNLogging {
    enum WNConsoleLocation {
        eStdOut,
        eStdErr
    };
    template<WNConsoleLocation T_Level = eStdOut>
    class WNConsoleLogger: public WNLogger {
    public:
        WNConsoleLogger();
        ~WNConsoleLogger();
        virtual WN_INLINE WN_VOID FlushBuffer(const WN_CHAR* _buffer, WN_SIZE_T bufferSize, const std::vector<WNLogColorElement>&);
    private:
#if defined(_WN_WINDOWS)
        HANDLE mConsoleHandle;
        BOOL   mConsoleCreated;
        BOOL   mInitialized;
#elif defined(_WN_ANDROID)
        WN_INT32 mLogPriority; 
#endif
    };
}

#ifdef _WN_WINDOWS
#include "WNLogging/inc/Internal/WNConsoleLoggerWindows.inl"
#elif defined _WN_ANDROID
#include "WNLogging/inc/Internal/WNConsoleLoggerAndroid.inl"
#else
#include "WNLogging/inc/Internal/WNConsoleLoggerLinux.inl"
#endif

#endif
