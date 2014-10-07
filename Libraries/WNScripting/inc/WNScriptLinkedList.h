////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __WN_SCRIPT_LINKED_LIST_H__
#define __WN_SCRIPT_LINKED_LIST_H__
#include "WNCore/inc/WNTypes.h"
#include "WNScripting/inc/WNScriptingAllocations.h"
#include "WNMemory/inc/WNAllocation.h"
namespace WNScripting {
    template<typename T>
    struct WNScriptLinkedList {
        WNScriptLinkedList() : first(WN_NULL), last(WN_NULL) {}
        ~WNScriptLinkedList() {
            WNScriptLinkedListNode* node = first;
            while(node != WN_NULL) {
                WNScriptLinkedListNode* next = node->next;
                WN_DELETE(node);
                node = next;
            }
            first = last = WN_NULL;
        }
        void PushBack(T* _elem) {
            WNScriptLinkedListNode* node = WN_SCRIPTNODE_NEW(WNScriptLinkedListNode(_elem));
            if(first == WN_NULL) {
                first = node;
                last = node;
            } else {
                last->next = node;
                last = node;
            }
        }
    
        struct WNScriptLinkedListNode {
            WNScriptLinkedListNode(T* _value): value(_value), next(WN_NULL) {}
            T* value;
            WNScriptLinkedListNode* next;
        };
        WNScriptLinkedListNode* first;
        WNScriptLinkedListNode* last;
    };
}

#endif//__WN_SCRIPT_LINKED_LIST_H__