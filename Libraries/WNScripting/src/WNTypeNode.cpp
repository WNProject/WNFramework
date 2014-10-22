// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNScripting/inc/WNTypeNode.h"
#include "WNScripting/inc/WNScriptingAllocations.h"
#include "WNScripting/inc/WNCodeModule.h"
#include "WNScripting/inc/WNTypeManager.h"
using namespace WNScripting;

WNTypeNode::WNTypeNode(const char* _customType) :
    mType(SC_CUSTOM),
    mNumArrayLevels(0){
        COPY_STRING(_customType, mCustomType);
}

WNTypeNode::~WNTypeNode() {
    WN_DELETE(mCustomType);
}
    
void WNTypeNode::AddArrayLevel() {
    mNumArrayLevels += 1;
}

eWNTypeError WNTypeNode::GetType(WNTypeManager& _manager, WNScriptType& _outType, WNLogging::WNLog& _compilationLog) const {
    eWNTypeError err = eWNOK;
    if(eWNOK != (err = _manager.GetTypeByName(mCustomType, _outType))) {
        _compilationLog.Log(WNLogging::eError, 0, "Could not find type ", mCustomType);
        LogLine(_compilationLog, WNLogging::eError);
        return(err);
    }
    for(WN_SIZE_T i = 0; i < mNumArrayLevels; ++i) {
        if(eWNOK != (err = _manager.GetArrayOf(_outType, _outType))) {
            _compilationLog.Log(WNLogging::eError, 0, "Could not find type ", mCustomType);
            LogLine(_compilationLog, WNLogging::eError);
            return(err);
        }   
    }
    
    

    return(eWNOK);
}
