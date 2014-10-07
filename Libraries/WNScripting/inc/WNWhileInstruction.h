////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __WN_WHILE_INSTRUCTION_H__
#define __WN_WHILE_INSTRUCTION_H__

#include "WNScripting/inc/WNInstruction.h"

namespace WNScripting {
    class WNExpression;
    class WNInstructionList;
    class WNWhileInstruction : public WNInstruction {
    public:
        WNWhileInstruction(WNExpression* _cond, WNInstructionList* _body);
        virtual ~WNWhileInstruction();
        virtual eWNTypeError GenerateCode(WNCodeModule& _module, const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog);
    private:
        WNExpression* mCondition;
        WNInstructionList* mBody;
    };
}
#endif//__WN_WHILE_INSTRUCTION_H__
