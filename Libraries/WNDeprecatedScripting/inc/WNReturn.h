// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef __WN_RETURN_H__
#define __WN_RETURN_H__

#include "WNDeprecatedScripting/inc/WNInstruction.h"

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
