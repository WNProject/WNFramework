////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "WNScripting/inc/WNInstruction.h"

using namespace WNScripting;

WNInstruction::WNInstruction() : mReturns(WN_FALSE) { }
WNInstruction::~WNInstruction() {}

WN_BOOL WNInstruction::Returns() { return(mReturns); }
