////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "WNScripting/inc/WNDeclList.h"
#include "WNScripting/inc/WNDeclaration.h"
using namespace WNScripting; 

WNDeclList::WNDeclList(WNDeclaration* _decl) {
    mDeclarations.PushBack(_decl);
}

WNDeclList::~WNDeclList() {
    for(WNScriptLinkedList<WNDeclaration>::WNScriptLinkedListNode* i = mDeclarations.first; i != WN_NULL; i=i->next) {
        WN_DELETE(i->value);
    }
}

void WNDeclList::AddDeclaration(WNDeclaration* _decl) {
    mDeclarations.PushBack(_decl);
}

eWNTypeError WNDeclList::GetTypeList(WNTypeManager& _manager, std::vector<WNScriptType>& _vector, WNLogging::WNLog& _compilationLog) {
    eWNTypeError err = eWNOK;
    for(WNScriptLinkedList<WNDeclaration>::WNScriptLinkedListNode *i = mDeclarations.first; i != WN_NULL; i = i->next) {
        WNScriptType t;
        if(eWNOK != (err = i->value->GetType(_manager, t, _compilationLog))) {
            _compilationLog.Log(WNLogging::eError, 0, "Cannot find type for variable: ", i->value->GetName());
            return err;
        }
        _vector.push_back(t);
    }
    return(eWNOK);
}

eWNTypeError WNDeclList::GetDeclarations(const WNScriptLinkedList<WNDeclaration>*& _outDeclaration) const {
    _outDeclaration = &mDeclarations;
    return(eWNOK);
}

