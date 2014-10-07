////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_CORE_INTERNAL_X86_PREFETCH_SSE_H__
#define __WN_CORE_INTERNAL_X86_PREFETCH_SSE_H__

#include <xmmintrin.h>

namespace WNCore {
    namespace __WNInternal {
        template <typename Type>
        class __WNPrefetch<Type, typename WNCore::WNEnableWhen<(!((sizeof(Type) == 4 || sizeof(Type) == 8) && WNCore::WNIsFloatingPoint<Type>::Value))>::Value> {
        public:
            static WN_FORCE_INLINE WN_VOID Execute(const Type* _address) {
                _mm_prefetch(reinterpret_cast<const char*>(_address), _MM_HINT_T0);
            }

        private:
            WN_FORCE_INLINE __WNPrefetch() {}
        };

        template <typename Type>
        class __WNPrefetch<Type, typename WNCore::WNEnableWhen<((sizeof(Type) == 4 || sizeof(Type) == 8) && WNCore::WNIsFloatingPoint<Type>::Value)>::Value> {
        public:
            static WN_FORCE_INLINE WN_VOID Execute(const Type* _address) {
                _mm_prefetch(reinterpret_cast<const char*>(_address), _MM_HINT_T1);
            }

        private:
            WN_FORCE_INLINE __WNPrefetch() {}
        };
    }
}

#endif // __WN_CORE_INTERNAL_X86_PREFETCH_SSE_H__