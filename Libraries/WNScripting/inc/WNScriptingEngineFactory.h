// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef __WN_SCRIPTING_ENGINE_FACTORY_H__
#define __WN_SCRIPTING_ENGINE_FACTORY_H__

namespace WNScripting {
    class WNScriptingEngine;
    class WNScriptingEngineFactory {
    public:
        static WNScriptingEngine* CreateScriptingEngine();
    };
};

#endif//__WN_SCRIPTING_ENGINE_FACTORY_H__
