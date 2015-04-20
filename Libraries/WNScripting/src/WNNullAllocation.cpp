// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNScripting/inc/WNNullAllocation.h"
#include "WNScripting/inc/WNNodeHelpers.h"


using namespace WNScripting;

WNNullAllocation::WNNullAllocation() {
}

WNNullAllocation::~WNNullAllocation() {

}

eWNTypeError WNNullAllocation::GenerateCode(WNCodeModule& _module, const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog) {
    eWNTypeError err = ok;

    if(ok != (err = GenerateNullAllocation(_module, _def, mValue, mScriptType))){
        _compilationLog.Log(WNLogging::eCritical, 0, "Error generating NULL allocation");
        LogLine(_compilationLog, WNLogging::eCritical);
        return(err);
    }
    mForceUse = true;
    mNewlyCreated = true;
    return(ok);
}

