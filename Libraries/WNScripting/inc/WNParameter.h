////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __WN_PARAMETER_H__
#define __WN_PARAMETER_H__

#include "WNCore/inc/WNTypes.h"
#include "WNScripting/inc/WNTypeElement.h"
namespace WNScripting {
    struct WNParameter {
        WNParameter(WNScriptType _type) :
            mType(_type) {
        }
        WNParameter() :
            mType(WN_NULL) {
        }
        WNScriptType mType;

        union {
            WN_SIZE_T mSizetVal;
            WN_UINT32 mUint32Val;
            WN_INT32 mInt32Val;
            WN_INT64 mInt64val;
            WN_FLOAT32 mFloatVal;
            WN_FLOAT64 mDoubleVal;
        };
    };
}
#endif//__WN_PARAMETER_H__