// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef __WN_NODE_H__
#define __WN_NODE_H__
#include "WNCore/inc/WNTypes.h"
#include "WNLogging/inc/WNLog.h"

namespace WNScripting {
    struct WNScriptLocation {
        wn_int64  mStartIndex;
        wn_int64 mEndIndex;
        const wn_byte*  mLineStart;
        wn_int64 mLineNumber;
        wn_int64 mCharNumber;
    };

    class WNNode{
    public:
        wn_void SetStartPosition(const WNScriptLocation& _location) { mStartLocation = _location; }
        wn_void SetEndIndex(wn_int64 _idx) { mStartLocation.mEndIndex = _idx; }
        const WNScriptLocation& GetStartPosition() const { return(mStartLocation); }
        wn_void LogLine(WNLogging::WNLog& _log, WNLogging::WNLogLevel _level) const {
            wn_size_t lineLength = 1023;
            const wn_char* c = wn::memory::strnchr(reinterpret_cast<const wn_char*>(mStartLocation.mLineStart), '\n', lineLength);
            lineLength = (c - reinterpret_cast<const wn_char*>(mStartLocation.mLineStart));
            wn_char* vArray = static_cast<wn_char*>(WN_STACK_ALLOC(lineLength + 1));
            memcpy(vArray, mStartLocation.mLineStart, lineLength);
            vArray[lineLength] = '\0';
            _log.Log(_level, 0, "Line: ", mStartLocation.mLineNumber, "\n", vArray);
        }
    protected:
        WNScriptLocation mStartLocation;
    };
}
#endif//__WN_NODE_H__
