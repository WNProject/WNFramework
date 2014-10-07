////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "WNScripting/inc/WNScriptingEngineFactory.h"
#include "WNScripting/inc/WNScriptingEngineImpl.h"
#include "WNMemory/inc/WNAllocation.h"

WNScripting::WNScriptingEngine* WNScripting::WNScriptingEngineFactory::CreateScriptingEngine() {
    return(WN_NEW WNScripting::WNScriptingEngineImpl());
}
