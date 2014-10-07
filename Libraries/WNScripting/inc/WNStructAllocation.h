////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef __WN_STRUCT_ALLOCATION_H__
#define __WN_STRUCT_ALLOCATION_H__
#include "WNScripting/inc/WNExpression.h"

namespace WNScripting {    
    class WNTypeNode;
    class WNStructAllocation : public WNExpression {
    public:
        WNStructAllocation();
        virtual ~WNStructAllocation();
        WN_VOID SetType(WNTypeNode* _type);
        WN_VOID SetCopyInitializer(WNExpression* _expression); 
        eWNTypeError GenerateCode(WNCodeModule& _module, const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog);
    private:
        WNTypeNode* mType;
        WNExpression* mCopyInitializer;
    };
}
#endif//__WN_STRUCT_ALOCATION_H__
