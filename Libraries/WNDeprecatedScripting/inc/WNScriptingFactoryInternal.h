// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef __WN_SCRIPTING_FACTORY_INTERNAL_H__
#define __WN_SCRIPTING_FACTORY_INTERNAL_H__

namespace WNScripting {
    class WNTypeManager;
    class WNScopedVariableList;
    class WNScriptingFactoryInternal {
    public:
        static WNTypeManager* CreateTypeManager();
        static WNScopedVariableList* CreateScopedVariableList();
    };
};

#endif//__WN_SCRIPTING_ENGINE_FACTORY_H__
