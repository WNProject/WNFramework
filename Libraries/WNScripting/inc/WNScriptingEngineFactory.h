////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
