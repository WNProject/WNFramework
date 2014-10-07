////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __WN_INSTRUCTION_H__
#define __WN_INSTRUCTION_H__
#include "WNCore/inc/WNTypes.h"
#include "WNScripting/inc/WNNode.h"
#include "WNScripting/inc/WNScriptingErrors.h"
#include "WNLogging/inc/WNLog.h"

namespace WNScripting {
    struct WNFunctionDefinition;
    class WNCodeModule;
    class WNInstruction : public WNNode {
    public:
        WNInstruction();
        virtual ~WNInstruction();
        virtual eWNTypeError GenerateCode(WNCodeModule&, const WNFunctionDefinition*, WNLogging::WNLog& _compilationLog) = 0;
        WN_BOOL Returns();
    protected:
        WN_BOOL mReturns;
    };
}
#endif//__WN_INSTRUCTION_H__
