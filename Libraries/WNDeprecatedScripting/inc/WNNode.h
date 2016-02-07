// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef __WN_NODE_H__
#define __WN_NODE_H__
#include "WNCore/inc/WNTypes.h"
#include "WNLogging/inc/WNLog.h"

namespace WNScripting {
struct WNScriptLocation {
  int64_t mStartIndex;
  int64_t mEndIndex;
  const uint8_t* mLineStart;
  int64_t mLineNumber;
  int64_t mCharNumber;
};

class WNNode {
public:
  void SetStartPosition(const WNScriptLocation& _location) {
    mStartLocation = _location;
  }
  void SetEndIndex(int64_t _idx) {
    mStartLocation.mEndIndex = _idx;
  }
  const WNScriptLocation& GetStartPosition() const {
    return (mStartLocation);
  }
  void LogLine(WNLogging::WNLog& _log, WNLogging::WNLogLevel _level) const {
    size_t lineLength = 1023;
    const char* c = wn::memory::strnchr(
        reinterpret_cast<const char*>(mStartLocation.mLineStart), '\n',
        lineLength);
    lineLength = (c - reinterpret_cast<const char*>(mStartLocation.mLineStart));
    char* vArray = static_cast<char*>(WN_STACK_ALLOC(lineLength + 1));
    memcpy(vArray, mStartLocation.mLineStart, lineLength);
    vArray[lineLength] = '\0';
    _log.Log(_level, 0, "Line: ", mStartLocation.mLineNumber, "\n", vArray);
  }

protected:
  WNScriptLocation mStartLocation;
};
}
#endif  //__WN_NODE_H__
