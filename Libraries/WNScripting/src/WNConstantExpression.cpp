// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNScripting/inc/WNConstantExpression.h"
#include "WNScripting/inc/WNCodeModule.h"
#include "WNScripting/inc/WNTypeManager.h"
#include "WNScripting/inc/WNScriptingAllocations.h"
#include "WNMemory/inc/WNAllocation.h"

using namespace WNScripting;

WNConstantExpression::WNConstantExpression(WNScriptTypeName _type, const char* _text) :
    mTypeName(_type),
    mText(wn_nullptr) {
    COPY_STRING(_text, mText);
}

WNConstantExpression::~WNConstantExpression() {
    wn::memory::destroy(mText);
}

eWNTypeError WNConstantExpression::GenerateCode(WNCodeModule& _module, const WNFunctionDefinition*, WNLogging::WNLog& _compilationLog) {
    WN_DEBUG_ASSERT(mTypeName < SC_MAX);
    mNewlyCreated = true;
    if(mTypeName == SC_CUSTOM) {
        WN_RELEASE_ASSERT_DESC(false, "TODO: IMPLEMENT");
    }

    if(ok != _module.GetTypeManager().GetTypeByName(WNScriptTypeNames[mTypeName], mScriptType))
    {
        _compilationLog.Log(WNLogging::eError, 0, "Trying to create constant of invalid type: ", WNScriptTypeNames[mTypeName]);
        LogLine(_compilationLog, WNLogging::eError);
        return(eWNBadType);
    }

    for(wn_size_t i = 0; i < WNScriptTypeLevels[mTypeName]; ++i) {
        _module.GetTypeManager().GetArrayOf(mScriptType, mScriptType);
    }

    const GenerateConstantOperation* op = _module.GetTypeManager().GetConstantOperation(mScriptType);
    if(!op) {
        _compilationLog.Log(WNLogging::eError, 0, "No constant generation possible for: ", WNScriptTypeNames[mTypeName]);
        LogLine(_compilationLog, WNLogging::eError);
        return(eWNBadType);
    }

    return(op->Execute(_module, mText, mForceUse, mValue));

}

