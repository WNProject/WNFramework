////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __WN_IF_INSTRUCTION_H__
#define __WN_IF_INSTRUCTION_H__

#include "WNScripting/inc/WNScriptLinkedList.h"
#include "WNScripting/inc/WNInstruction.h"
namespace WNScripting {
    class WNExpression;
    class WNInstructionList;
    class WNElseIf : public WNInstruction {
    public:
        WNElseIf(WNExpression* _cond, WNInstructionList* _body);
        virtual ~WNElseIf();

        virtual eWNTypeError GenerateCode(WNCodeModule& _module, const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog);

        WNExpression* GetCondition();

        WNInstructionList* GetBody();
    private:
        WNExpression* mCondition;
        WNInstructionList* mBody;
    };

    class WNIFInstruction : public WNInstruction {
    public:
        WNIFInstruction(WNExpression* _cond, WNInstructionList* _body);
        virtual ~WNIFInstruction();
        void AddElseIf(WNElseIf* _elseif);
        void AddElse(WNInstructionList* _else);

        virtual eWNTypeError GenerateCode(WNCodeModule& _module, const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog);
    private:
        WNExpression* mCondition;
        WNInstructionList* mElse;
        WNInstructionList* mBody;
        WNScriptLinkedList<WNElseIf> mElseIfNodes;
    };
}
#endif//__WN_IF_INSTRUCTION_H__
