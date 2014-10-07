////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WN_CORE_INTERNAL_X86_PREFETCH_3D_NOW_H__
#define __WN_CORE_INTERNAL_X86_PREFETCH_3D_NOW_H__

#include <mm3dnow.h>

namespace WNCore {
    namespace __WNInternal {
        template <typename Type>
        class __WNPrefetch<Type, typename WNCore::WNEnableWhen<WN_TRUE>::Value> {
        public:
            static WN_FORCE_INLINE WN_VOID Execute(const Type* _address) {
                _m_prefetchw(reinterpret_cast<const void*>(_address));
            }

        private:
            WN_FORCE_INLINE __WNPrefetch() {}
        };
    }
}

#endif // __WN_CORE_INTERNAL_X86_PREFETCH_3D_NOW_H__