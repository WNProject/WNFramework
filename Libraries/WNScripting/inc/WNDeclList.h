////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __WN_DECL_LIST_H__
#define __WN_DECL_LIST_H__
#include "WNScripting/inc/WNNode.h"
#include "WNScripting/inc/WNScriptingErrors.h"
#include "WNScripting/inc/WNScriptLinkedList.h"
#include "WNScripting/inc/WNTypeElement.h"

#include <vector>
namespace WNScripting {
    class WNDeclaration;
    class WNTypeManager;
    class WNDeclList : public WNNode {
    public:
        WNDeclList(WNDeclaration* _decl); 
        virtual ~WNDeclList();
        void AddDeclaration(WNDeclaration* _decl); 

        eWNTypeError GetTypeList(WNTypeManager& _manager, std::vector<WNScriptType>& _vector, WNLogging::WNLog& _compilationLog); 

        eWNTypeError GetDeclarations(const WNScriptLinkedList<WNDeclaration>*& _outDeclaration) const; 

    private:
        WNScriptLinkedList<WNDeclaration> mDeclarations;
    };
}
#endif//__WN_DECL_LIST_H__

