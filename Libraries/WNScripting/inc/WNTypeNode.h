////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __WN_TYPE_NODE_H__
#define __WN_TYPE_NODE_H__
#include "WNScripting/inc/WNNode.h"
#include "WNScripting/inc/WNScriptingErrors.h"
#include "WNScripting/inc/WNScriptingEnums.h"
#include "WNScripting/inc/WNTypeElement.h"
#include "WNCore/inc/WNTypes.h"

namespace WNScripting {
    class WNTypeManager;
    class WNTypeNode : public WNNode {
    public:
        WNTypeNode(const char* _customType);
        virtual ~WNTypeNode();
        void AddArrayLevel();

        eWNTypeError GetType(WNTypeManager& _module, WNScriptType& _outType, WNLogging::WNLog& _compilationLog) const; 

    private:
        WNScriptTypeName mType;
        char* mCustomType;
        WN_SIZE_T mNumArrayLevels;
    };
}
#endif//__WN_TYPE_NODE_H__
