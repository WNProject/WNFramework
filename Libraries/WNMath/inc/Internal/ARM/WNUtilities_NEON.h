////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_MATH_INTERNAL_ARM_UTILITIES_NEON_H__
#define __WN_MATH_INTERNAL_ARM_UTILITIES_NEON_H__

namespace WNMath {
    namespace __WNInternal {
        template <WN_UINT32 Dimension>
        struct __WNNumberRegisters_NEON_32_D {
            enum {
                Value = ((Dimension + 1) / 2)
            };
        };

        template <WN_UINT32 Dimension>
        struct __WNNumberRegisters_NEON_32_Q {
            enum {
                Value = (Dimension - ((Dimension / 4) * 4)) >= 3 ? ((Dimension + 3) / 4) : (Dimension / 4)
            };
        };
    }
};

#endif // __WN_MATH_INTERNAL_ARM_UTILITIES_NEON_H__