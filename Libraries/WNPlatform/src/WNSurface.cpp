////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "WNCore/inc/WNBase.h"

#ifdef _WN_DEBUG
    #include "WNPlatform/inc/WNSurface.h"

    #undef WN_INLINE
    #define WN_INLINE

    #undef WN_FORCE_INLINE
    #define WN_FORCE_INLINE

    #include "WNPlatform/inc/Internal/WNSurface.inl"
#endif