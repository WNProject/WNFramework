////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __WN_LOG_H__
#define __WN_LOG_H__
#include "WNCore/inc/WNAssert.h"
#include "WNCore/inc/WNTypes.h"
#include "WNStrings/inc/WNStrings.h"
#include <vector>


#define LTM(N) typename T##N
#define LVM(N) T##N v##N
namespace WNLogging {
    enum WNLogLevel { //starting at 1 so that we can turn off with 0
        eNone = 0,
        eCritical,
        eError,
        eWarning,
        eIssue,
        eInfo,
        eDebug,
        eLogMax
    };

    static const char* LogMessages[WNLogging::eLogMax] = { //-1 here because log levels start at 0
        "None:",
        "Critial:",
        "Error:",
        "Warning:",
        "Issue:",
        "Info:",
        "Debug:" 
	};

    enum WNLogFlags {
        eWNNoNewLine = 1<<0,
        eWNNoHeader = 1<<1
    };

    
    struct WNLogColorElement {
        WNLogLevel mLevel;
        WN_CHAR* mPosition;
    };

    class WNLogger {
	public:
        virtual WN_VOID FlushBuffer(const WN_CHAR* _buffer, WN_SIZE_T bufferSize, const std::vector<WNLogColorElement>& mColors) = 0;
	};

    #define LOGGER_ENCODING_HELPER(Type, formatter) \
    template<typename T> \
    struct _Enc##Type { static const WN_CHAR* GetVal(){return formatter;} }; \
    template<> struct _Enc##Type<WN_WCHAR> { static const WN_WCHAR* GetVal(){return L##formatter;} };
    
    LOGGER_ENCODING_HELPER(Ptr, "%p");
    template<typename T, typename BuffType>
    struct LogTypeHelper {
        WN_FORCE_INLINE static WN_BOOL DoLog(const T& _0, BuffType* _buffer, WN_SIZE_T& _bufferLeft) {
            int printed = WNStrings::WNTSnPrintf(_buffer, _bufferLeft, _EncPtr<BuffType>::GetVal(), _0);
            if(printed < 0 || static_cast<WN_SIZE_T>(printed) >= _bufferLeft) {
                return(WN_FALSE);
            }
            _bufferLeft -= printed;
            return(WN_TRUE);
        }
    };

    template<typename T, typename BuffType>
    WN_FORCE_INLINE WN_BOOL LogType(const T&_0, BuffType* _buffer, WN_SIZE_T& _bufferLeft) {
        return(LogTypeHelper<T, BuffType>::DoLog(_0, _buffer, _bufferLeft));
    }
	
    class WNLogParams {
    public:
        virtual WNLogLevel GetDefaultLogLevel() const = 0;
        virtual WN_SIZE_T GetBufferSize() const = 0;
        virtual WN_BOOL   FlushAfterMessage() const = 0;
    };

    template<WNLogLevel lvl = eError, WN_SIZE_T size = 1024, WN_BOOL flush=WN_FALSE>
    class WNDefaultLogParameters : public WNLogParams {
    public:
        virtual WNLogLevel GetDefaultLogLevel() const {
            return(lvl);  
        }
        virtual WN_SIZE_T GetBufferSize() const {
            return(size);
        }
        virtual WN_BOOL   FlushAfterMessage() const {
            return(flush);
        }
    };
   
    class WNLogElem {
        friend class WNLog;
        enum WNLogAmount {
            eWNFALSE,
            eWNTRUE
        };
    };
    
    class WNLog {
    public:
        WNLog(WNLogger* logger, const WNLogParams& params = WNDefaultLogParameters<>()) : mLogger(logger) {
            mBufferSize = params.GetBufferSize();
            mBufferLeft = mBufferSize;
            mCurrentLogLevel = params.GetDefaultLogLevel();
            mLogBuffer = static_cast<WN_CHAR*>(malloc(mBufferSize));
            mFlushAfterMessage = params.FlushAfterMessage();
        }

        ~WNLog() { Flush(); }
        WN_FORCE_INLINE WN_VOID SetLogLevel(WNLogLevel _level) { mCurrentLogLevel = _level; }
        WN_FORCE_INLINE WN_VOID Flush();
        WN_FORCE_INLINE WN_VOID FlushExternal(const WN_CHAR* _buffer, WN_SIZE_T _bufferSize, const std::vector<WNLogColorElement>& mColors);

        template<LTM(0)>
        WN_FORCE_INLINE WN_VOID Log(WNLogLevel, WN_SIZE_T, LVM(0));
        template<LTM(0), LTM(1)>
        WN_FORCE_INLINE WN_VOID Log(WNLogLevel, WN_SIZE_T, LVM(0), LVM(1));
        template<LTM(0), LTM(1), LTM(2)>
        WN_FORCE_INLINE WN_VOID Log(WNLogLevel, WN_SIZE_T, LVM(0), LVM(1), LVM(2));
        template<LTM(0), LTM(1), LTM(2), LTM(3)>
        WN_FORCE_INLINE WN_VOID Log(WNLogLevel, WN_SIZE_T, LVM(0), LVM(1), LVM(2), LVM(3));
        template<LTM(0), LTM(1), LTM(2), LTM(3), LTM(4)>
        WN_FORCE_INLINE WN_VOID Log(WNLogLevel, WN_SIZE_T, LVM(0), LVM(1), LVM(2), LVM(3), LVM(4));
        template<LTM(0), LTM(1), LTM(2), LTM(3), LTM(4), LTM(5)>
        WN_FORCE_INLINE WN_VOID Log(WNLogLevel, WN_SIZE_T, LVM(0), LVM(1), LVM(2), LVM(3), LVM(4), LVM(5));
        template<LTM(0), LTM(1), LTM(2), LTM(3), LTM(4), LTM(5), LTM(6)>
        WN_FORCE_INLINE WN_VOID Log(WNLogLevel, WN_SIZE_T, LVM(0), LVM(1), LVM(2), LVM(3), LVM(4), LVM(5), LVM(6));
        template<LTM(0), LTM(1), LTM(2), LTM(3), LTM(4), LTM(5), LTM(6), LTM(7)>
        WN_FORCE_INLINE WN_VOID Log(WNLogLevel, WN_SIZE_T, LVM(0), LVM(1), LVM(2), LVM(3), LVM(4), LVM(5), LVM(6), LVM(7));
        template<LTM(0), LTM(1), LTM(2), LTM(3), LTM(4), LTM(5), LTM(6), LTM(7), LTM(8)>
        WN_FORCE_INLINE WN_VOID Log(WNLogLevel, WN_SIZE_T, LVM(0), LVM(1), LVM(2), LVM(3), LVM(4), LVM(5), LVM(6), LVM(7), LVM(8));
        template<LTM(0), LTM(1), LTM(2), LTM(3), LTM(4), LTM(5), LTM(6), LTM(7), LTM(8), LTM(9)>
        WN_FORCE_INLINE WN_VOID Log(WNLogLevel, WN_SIZE_T, LVM(0), LVM(1), LVM(2), LVM(3), LVM(4), LVM(5), LVM(6), LVM(7), LVM(8), LVM(9));
    private:
        WNLog& operator=(const WNLog& _other); // dont want an assignment
        WN_FORCE_INLINE WN_VOID LogHeader(WNLogLevel);
        WN_FORCE_INLINE WN_VOID LogNewline();
        template<typename T0> WN_VOID LogParam(const T0& _val);

        WNLogger* mLogger;
        WN_CHAR* mLogBuffer;
        WN_SIZE_T mBufferLeft;
        WN_SIZE_T mBufferSize;
        std::vector<WNLogColorElement> mColorElements;
        WN_SIZE_T mCurrentLogLevel;
        WN_BOOL mFlushAfterMessage;
    };
}
#include "WNLogging/inc/WNLog.inl"
#include "WNLogging/inc/WNDefaultLogTypes.inl"
#endif//__WN_LOG_H__
