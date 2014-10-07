////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __WN_SCRIPTING_ALLOCATIONS_H__
#define __WN_SCRIPTING_ALLOCATIONS_H__

#include "WNCore/inc/WNBase.h"

#define WN_SCRIPTNODE_NEW(T) (new T)
#define WN_SCRIPTNODE_MALLOC(T) (malloc(T))

#ifdef _WN_WINDOWS
#define COPY_STRING(src, dest) { \
    if(src) { \
        WN_SIZE_T allocSize = strnlen_s(src, 256) + 1; \
        dest = static_cast<char*>(WN_SCRIPTNODE_MALLOC(allocSize)); \
        strncpy(dest, src, allocSize-1); \
        dest[allocSize-1] = '\0'; \
    } else { \
        dest = WN_NULL; \
    }\
}
#else
#define COPY_STRING(src, dest) { \
    if(src) { \
        WN_SIZE_T allocSize = strnlen(src, 256) + 1; \
        dest = static_cast<char*>(WN_SCRIPTNODE_MALLOC(allocSize)); \
        strncpy(dest, src, allocSize-1); \
        dest[allocSize-1] = '\0'; \
    } else { \
        dest = WN_NULL; \
    } \
}
#endif


#endif//__WN_SCRIPTING_ALLOCATIONS_H__
