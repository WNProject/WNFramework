////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __WN_INSTRUCTION_LIST_H__
#define __WN_INSTRUCTION_LIST_H__

#include "WNScripting/inc/WNNode.h"
#include "WNScripting/inc/WNScriptLinkedList.h"
#include "WNScripting/inc/WNScriptingErrors.h"
#include "WNCore/inc/WNTypes.h"
#include "WNLogging/inc/WNLog.h"

namespace WNScripting {
    class WNInstruction;
    class WNCodeModule;
    struct WNFunctionDefinition;
    class WNInstructionList : public WNNode {
    public:
        WNInstructionList();
        WNInstructionList(WNInstruction* inst);
        virtual ~WNInstructionList();
        void AddInstruction(WNInstruction* inst);

        eWNTypeError GenerateCode(WNCodeModule& _module, const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog);

        WN_BOOL Returns();
    private:
        WNScriptLinkedList<WNInstruction> mInstructions;
        WN_BOOL mReturns;
    };
}
#endif//__WN_INSTRUCTION_LIST_H__
