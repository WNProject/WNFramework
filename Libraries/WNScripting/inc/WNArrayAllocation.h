////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __WN_ARRAY_ALLOCATION_H__
#define __WN_ARRAY_ALLOCATION_H__
#include "WNScripting/inc/WNExpression.h"
#include "WNScripting/inc/WNScriptLinkedList.h"

namespace WNScripting {
    class WNTypeNode;
    class WNArrayAllocation : public WNExpression { 
    public:
        WNArrayAllocation();
        virtual ~WNArrayAllocation();
        WN_VOID SetType(WNTypeNode* _typeNode); 
        WN_VOID AddExpression(WNExpression* _expr); 
        WN_VOID AddLevel(); 
        WN_VOID SetCopyInitializer(WNExpression* _expression);
        eWNTypeError GenerateCode(WNCodeModule& _module, const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog); 

    private:
        WNScriptLinkedList<WNExpression> mArrayInitializers;
        WNExpression* mCopyInitializer;
        WNTypeNode* mType;
        WN_SIZE_T mLevels;
    };
}
#endif//__WN_ARRAY_ALLOCATION_H__
