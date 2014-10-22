// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef __WN_ARG_LIST_H__
#define __WN_ARG_LIST_H__

#include "WNScripting/inc/WNNode.h"
#include "WNScripting/inc/WNScriptLinkedList.h"

namespace WNScripting {
    class WNExpression;
    struct WNFunctionExpression {
        WNFunctionExpression(WNExpression* _expr, WN_BOOL _handOwnership) :
            expr(_expr), handOwnership(_handOwnership) {
        }
        WNExpression* expr;
        WN_BOOL handOwnership;
    };

    class WNArgList : public WNNode {
    public:
        WNArgList(); 
        virtual ~WNArgList();
        void AddExpression(WNExpression* _expr, WN_BOOL _handOwnership = WN_FALSE); 
        WNScriptLinkedList<WNFunctionExpression>& GetExpressions(); 
    private:
        WNScriptLinkedList<WNFunctionExpression> mExpressionList;
    };
}
#endif//_WN_ARG_LIST_H__

