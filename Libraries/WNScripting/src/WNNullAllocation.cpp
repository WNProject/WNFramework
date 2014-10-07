////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "WNScripting/inc/WNNullAllocation.h"
#include "WNScripting/inc/WNNodeHelpers.h"


using namespace WNScripting; 

WNNullAllocation::WNNullAllocation() {
}

WNNullAllocation::~WNNullAllocation() {
    
}

eWNTypeError WNNullAllocation::GenerateCode(WNCodeModule& _module, const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog) {
    eWNTypeError err = eWNOK;
    
    if(eWNOK != (err = GenerateNullAllocation(_module, _def, mValue, mScriptType))){
        _compilationLog.Log(WNLogging::eCritical, 0, "Error generating NULL allocation");
        LogLine(_compilationLog, WNLogging::eCritical);
        return(err);
    }
    mForceUse = true;
    mNewlyCreated = true;
    return(eWNOK);
}

