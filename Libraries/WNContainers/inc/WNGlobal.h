////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_CONTAINERS_GLOBAL_H__
#define __WN_CONTAINERS_GLOBAL_H__

#include "WNCore/inc/WNTypes.h"

namespace WNContainers {
    template <typename Type, WN_UINT64 ID>
    class WNGlobal {
    public:
        static Type mValue;

    private:
        WNGlobal();
        WNGlobal(const WNGlobal&);
        WN_VOID operator = (const WNGlobal&);
    };
}

#endif // __WN_CONTAINERS_GLOBAL_H__