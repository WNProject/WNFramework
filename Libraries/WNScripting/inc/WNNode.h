////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __WN_NODE_H__
#define __WN_NODE_H__
#include "WNCore/inc/WNTypes.h"
#include "WNLogging/inc/WNLog.h"

namespace WNScripting {
    struct WNScriptLocation {
        WN_INT64  mStartIndex;
        WN_INT64 mEndIndex;
        const WN_BYTE*  mLineStart;
        WN_INT64 mLineNumber;
        WN_INT64 mCharNumber;
    };

    class WNNode{ 
    public:
        WN_VOID SetStartPosition(const WNScriptLocation& _location) { mStartLocation = _location; }
        WN_VOID SetEndIndex(WN_INT64 _idx) { mStartLocation.mEndIndex = _idx; }
        const WNScriptLocation& GetStartPosition() const { return(mStartLocation); }
        WN_VOID LogLine(WNLogging::WNLog& _log, WNLogging::WNLogLevel _level) const {
            WN_SIZE_T lineLength = 1023;
            const WN_CHAR* c = WNStrings::WNStrNChr(reinterpret_cast<const WN_CHAR*>(mStartLocation.mLineStart), 1023, '\n');
            lineLength = (c - reinterpret_cast<const WN_CHAR*>(mStartLocation.mLineStart));
            WN_CHAR* vArray = static_cast<WN_CHAR*>(WN_STACK_ALLOC(lineLength + 1));
            memcpy(vArray, mStartLocation.mLineStart, lineLength);
            vArray[lineLength] = '\0';
            _log.Log(_level, 0, "Line: ", mStartLocation.mLineNumber, "\n", vArray);
        }
    protected:
        WNScriptLocation mStartLocation;
    };
}
#endif//__WN_NODE_H__
