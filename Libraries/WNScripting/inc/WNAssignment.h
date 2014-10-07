////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __WN_ASSIGNMENT_H__
#define __WN_ASSIGNMENT_H__
#include "WNScripting/inc/WNScriptingEnums.h"
#include "WNScripting/inc/WNInstruction.h"

namespace WNScripting {
    class WNLValue;
    class WNExpression;
    class WNCodeModule;
    class WNAssignment : public WNInstruction {
    public:
        WNAssignment(WNLValue* _lValue);
        virtual ~WNAssignment();
        void AddValue(WNAssignType _type, WNExpression* value);

        virtual eWNTypeError GenerateCode(WNCodeModule& _module, const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog); 
    private:
        WNAssignType mAssignType;
        WNLValue* mLValue;
        WNExpression* mAssignExpression;
    };
}
#endif//__WN_ASSIGNMENT_H__
