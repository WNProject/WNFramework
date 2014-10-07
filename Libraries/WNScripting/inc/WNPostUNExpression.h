////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __WN_POST_UN_EXPRESSION_H__
#define __WN_POST_UN_EXPRESSION_H__

#include "WNScripting/inc/WNPostExpression.h"
#include "WNScripting/inc/WNScriptingEnums.h"
namespace WNScripting {
    class WNPostUNExpression : public WNPostExpression {
    public:
        WNPostUNExpression(WNPostUNType _type);
        virtual ~WNPostUNExpression();
        virtual eWNTypeError GenerateCode(WNCodeModule& _module, const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog);
    private:
        WNPostUNType mType;

    };
}
#endif//__WN_POST_UN_EXPRESSION_H__
