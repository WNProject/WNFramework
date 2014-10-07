////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __WN_SCRIPTING_ERRORS_H__
#define __WN_SCRIPTING_ERRORS_H__

enum eWNTypeError {
#include "WNCore/inc/Internal/WNErrors.inc"
    eWNAlreadyExists,
    eWNInvalidName,
    eWNInvalidConstant,
    eWNInvalidCast,
    eWNCannotCreateType,
    eWNInvalidOperation,
    eWNAmbiguous,
    eWNPartiallyAmbiguous,
    eWNUnknownResolution,
    eWNNoVirtual
};

#endif//__WN_SCRIPTING_ERRORS_H__
