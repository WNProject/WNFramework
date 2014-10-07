////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __WN_ID_EXPRESSION_H__
#define __WN_ID_EXPRESSION_H__
#include "WNScripting/inc/WNExpression.h"
namespace WNScripting {    
    class WNIDExpression : public WNExpression {
    public:
        WNIDExpression(const char* _name); 
        virtual ~WNIDExpression();
        virtual eWNTypeError GenerateCode(WNCodeModule& _module, const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog);
    private:
        char* mName;
    };
}
#endif//__WN_ID_EXPRESSION_H__
