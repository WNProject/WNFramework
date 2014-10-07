////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "WNScripting/inc/WNScriptingFactoryInternal.h"
#include "WNScripting/inc/WNTypeManagerImpl.h"
#include "WNMemory/inc/WNAllocation.h"
#include "WNScripting/inc/WNScopedVaraiableListImpl.h"

WNScripting::WNTypeManager* WNScripting::WNScriptingFactoryInternal::CreateTypeManager() {
    return(WN_NEW WNScripting::WNTypeManagerImpl());
}

WNScripting::WNScopedVariableList* WNScripting::WNScriptingFactoryInternal::CreateScopedVariableList() {
    return(WN_NEW WNScripting::WNScopedVariableListImpl());
}
