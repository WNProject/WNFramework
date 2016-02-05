// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef __WN_ARG_LIST_H__
#define __WN_ARG_LIST_H__

#include "WNDeprecatedScripting/inc/WNNode.h"
#include "WNDeprecatedScripting/inc/WNScriptLinkedList.h"

namespace WNScripting {
    class WNExpression;
    struct WNFunctionExpression {
        WNFunctionExpression(WNExpression* _expr, bool _handOwnership) :
            expr(_expr), handOwnership(_handOwnership) {
        }
        WNExpression* expr;
        bool handOwnership;
    };

    class WNArgList : public WNNode {
    public:
        WNArgList();
        virtual ~WNArgList();
        void AddExpression(WNExpression* _expr, bool _handOwnership = false);
        WNScriptLinkedList<WNFunctionExpression>& GetExpressions();
    private:
        WNScriptLinkedList<WNFunctionExpression> mExpressionList;
    };
}
#endif//_WN_ARG_LIST_H__

