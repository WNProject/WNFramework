////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __WN_NULL_ALLOCATION_H__
#define __WN_NULL_ALLOCATION_H__
#include "WNScripting/inc/WNExpression.h"

namespace WNScripting {    
    class WNNullAllocation : public WNExpression {
    public:
        WNNullAllocation();
        virtual ~WNNullAllocation();
        eWNTypeError GenerateCode(WNCodeModule& _module, const WNFunctionDefinition* _def, WNLogging::WNLog& _compilationLog);
    private:
    };
}
#endif//__WN_NULL_ALLOCATION_H__
