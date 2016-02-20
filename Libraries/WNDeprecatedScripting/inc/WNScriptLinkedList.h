// Copyright (c) 2016, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef __WN_SCRIPT_LINKED_LIST_H__
#define __WN_SCRIPT_LINKED_LIST_H__
#include "WNCore/inc/WNTypes.h"
#include "WNDeprecatedScripting/inc/WNScriptingAllocations.h"
#include "WNMemory/inc/WNBasic.h"
namespace WNScripting {
template <typename T>
struct WNScriptLinkedList {
  WNScriptLinkedList() : first(nullptr), last(nullptr) {}
  ~WNScriptLinkedList() {
    WNScriptLinkedListNode* node = first;
    while (node != nullptr) {
      WNScriptLinkedListNode* next = node->next;
      wn::memory::destroy(node);
      node = next;
    }
    first = last = nullptr;
  }
  void PushBack(T* _elem) {
    WNScriptLinkedListNode* node =
        WN_SCRIPTNODE_NEW(WNScriptLinkedListNode(_elem));
    if (first == nullptr) {
      first = node;
      last = node;
    } else {
      last->next = node;
      last = node;
    }
  }

  struct WNScriptLinkedListNode {
    WNScriptLinkedListNode(T* _value) : value(_value), next(nullptr) {}
    T* value;
    WNScriptLinkedListNode* next;
  };
  WNScriptLinkedListNode* first;
  WNScriptLinkedListNode* last;
};
}

#endif  //__WN_SCRIPT_LINKED_LIST_H__