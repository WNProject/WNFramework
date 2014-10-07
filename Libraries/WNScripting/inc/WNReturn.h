////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __WN_RETURN_H__
#define __WN_RETURN_H__

#include "WNScripting/inc/WNInstruction.h"

namespace WNScripting {
    class WNExpression;
    class WNReturn : public WNInstruction {
    public:
        WNReturn(WNExpression* _expr, bool _changeOwnership = false);
        WNReturn();
        virtual ~WNReturn();

        virtual eWNTypeError GenerateCode(WNCodeModule& _module, const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog);

    private:
        WNExpression* mExpression;
        bool mChangeOwnership;
    };
}
#endif//__WN_RETURN_H__
