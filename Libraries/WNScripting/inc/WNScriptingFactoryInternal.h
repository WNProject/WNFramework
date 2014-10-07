////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
