// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef __WN_SCRIPT_FILE_H__
#define __WN_SCRIPT_FILE_H__

#include "WNScripting/inc/WNNode.h"
#include "WNScripting/inc/WNScriptingErrors.h"
#include "WNScripting/inc/WNScriptLinkedList.h"
#include "WNScripting/inc/WNTypeElement.h"
#include "WNCore/inc/WNTypes.h"
#include "WNLogging/inc/WNLog.h"

#include <vector>
#include <list>
namespace WNScripting {
    class WNFunction;
    class WNStruct;
    class WNCodeModule;
    struct WNFunctionDefinition;
    class WNScriptFile : public WNNode {
    public:
        WNScriptFile();
        virtual ~WNScriptFile();
        void AddFunction(WNFunction* _node);

        void AddStruct(WNStruct* _node);

        void AddInclude(wn_char* _node);

        const WNScriptLinkedList<wn_char>::WNScriptLinkedListNode* GetFirstInclude() const;

        eWNTypeError PreProcess(WNCodeModule& _module, std::vector<WNScriptType>& _containedTypes, std::vector<WNFunctionDefinition*>& _containedFunctions, WNLogging::WNLog& _compilationLog);

        eWNTypeError FinalizePreProcess(WNCodeModule& _module, std::vector<WNFunctionDefinition*>& _containedFunctions, std::list<WNStruct*>& _toBeLinkedStructs, WNLogging::WNLog& _compilationLog);

        eWNTypeError DumpHeaders(WNCodeModule& _module, WNLogging::WNLog& _compilationLog);

        virtual eWNTypeError GenerateCode(WNCodeModule& _module, WNLogging::WNLog& _compilationLog);

    private:
        WNScriptLinkedList<WNFunction> mFunctions;
        WNScriptLinkedList<WNStruct>   mStructs;
        WNScriptLinkedList<wn_char>    mIncludes;
    };
}
#endif //__WN_SCRIPT_FILE_H__
