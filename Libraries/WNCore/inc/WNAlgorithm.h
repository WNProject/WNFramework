// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_CORE_ALGORITHM_H__
#define __WN_CORE_ALGORITHM_H__

#include "WNCore/inc/WNTypes.h"
#include "WNCore/inc/WNTypeTraits.h"

#include <algorithm>

namespace wn {
    template <typename input_it1, typename input_it2>
    WN_FORCE_INLINE wn_bool equal(input_it1 first1, input_it1 last1, input_it2 first2) {
        return(std::equal(first1, last1, first2));
    }

    template <typename input_it1, typename input_it2>
    WN_FORCE_INLINE wn_bool equal(input_it1 first1, input_it1 last1, input_it2 first2, input_it2 last2) {
        #ifdef __WN_HAS_CPP14_STL_EQUAL_AVAILABLE
            return(std::equal(first1, last1, first2, last2));
        #else
            for (; (first1 != last1 && first2 != last2); ++first1, ++first2) {
                if (*first1 != *first2) {
                    return(wn_false);
                }
            }

            return(first1 == last1 && first2 == last2);
        #endif
    }

    template <typename input_it1, typename input_it2, typename predicate>
    WN_FORCE_INLINE wn_bool equal_if(input_it1 first1, input_it1 last1, input_it2 first2, predicate p) {
        return(std::equal(first1, last1, first2, p));
    }

    template <typename input_it1, typename input_it2, typename predicate>
    WN_FORCE_INLINE wn_bool equal_if(input_it1 first1, input_it1 last1, input_it2 first2, input_it2 last2, predicate p) {
        #ifdef __WN_HAS_CPP14_STL_EQUAL_AVAILABLE
            return(std::equal(first1, last1, first2, last2, p));
        #else
            for (; (first1 != last1 && first2 != last2); ++first1, ++first2) {
                if (!p(*first1, *first2)) {
                    return(wn_false);
                }
            }

            return(first1 == last1 && first2 == last2);
        #endif
    }

    namespace internal {
        template <const uint32_t size, const uint32_t index = 0, typename = enable_if<wn_true>::type>
        struct unroll_loop {
            template <typename type, typename predicate>
            WN_FORCE_INLINE static wn_void execute(type& values, predicate p) {
                p(values[index]);
                unroll_loop<size, index + 1>::execute(values, p);
            }
        };

        template <const uint32_t size>
        struct unroll_loop<size, size> {
            template <typename type, typename predicate>
            WN_FORCE_INLINE static wn_void execute(type& values, predicate p) {
                WN_UNUSED_ARGUMENT(values);
                WN_UNUSED_ARGUMENT(p);
            }
        };

        template <const uint32_t size, const uint32_t index>
        struct unroll_loop<size, index, typename enable_if<(index >= 4 && size != index)>::type> {
            template <typename type, typename predicate>
            WN_FORCE_INLINE static wn_void execute(type& values, predicate p) {
                for (wn_size_t i = index; i < size; ++i) {
                    p(values[i]);
                }
            }
        };
    }

    template <const uint32_t size, typename type, typename predicate>
    WN_FORCE_INLINE wn_void for_each(type& values, predicate p) {
        internal::unroll_loop<size>::execute(values, p);
    }
}

#endif // __WN_CORE_ALGORITHM_H__