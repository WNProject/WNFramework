////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_MATH_COMMON_H__
#define __WN_MATH_COMMON_H__

namespace WNMath {
    enum WNSnapDirection {
        eWNNearest,
        eWNDown,
        eWNUp,
        eWNTruncate
    };

    namespace WNFieldOfView {
        struct WNHorizontal {};
        struct WNVertical {};
    };
}

#endif // __WN_MATH_COMMON_H__