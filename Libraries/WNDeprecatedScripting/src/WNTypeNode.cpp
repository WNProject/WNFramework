// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNDeprecatedScripting/inc/WNTypeNode.h"
#include "WNDeprecatedScripting/inc/WNScriptingAllocations.h"
#include "WNDeprecatedScripting/inc/WNCodeModule.h"
#include "WNDeprecatedScripting/inc/WNTypeManager.h"
using namespace WNScripting;

WNTypeNode::WNTypeNode(const char* _customType) :
    mType(SC_CUSTOM),
    mNumArrayLevels(0){
        COPY_STRING(_customType, mCustomType);
}

WNTypeNode::~WNTypeNode() {
    wn::memory::destroy(mCustomType);
}

void WNTypeNode::AddArrayLevel() {
    mNumArrayLevels += 1;
}

eWNTypeError WNTypeNode::GetType(WNTypeManager& _manager, WNScriptType& _outType, WNLogging::WNLog& _compilationLog) const {
    eWNTypeError err = ok;
    if(ok != (err = _manager.get_type_by_name(mCustomType, _outType))) {
        _compilationLog.Log(WNLogging::eError, 0, "Could not find type ", mCustomType);
        LogLine(_compilationLog, WNLogging::eError);
        return(err);
    }
    for(wn_size_t i = 0; i < mNumArrayLevels; ++i) {
        if(ok != (err = _manager.get_array_of(_outType, _outType))) {
            _compilationLog.Log(WNLogging::eError, 0, "Could not find type ", mCustomType);
            LogLine(_compilationLog, WNLogging::eError);
            return(err);
        }
    }



    return(ok);
}
