// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_CORE_ALGORITHM_H__
#define __WN_CORE_ALGORITHM_H__

#include "WNCore/inc/WNTypeTraits.h"

#include <algorithm>

namespace wn {
    namespace core {
        template <typename _InputIt1, typename _InputIt2>
        WN_FORCE_INLINE wn_bool equal(_InputIt1 _first1, _InputIt1 _last1, _InputIt2 _first2) {
           return(std::equal(_first1, _last1, _first2));
        }

        template <typename _InputIt1, typename _InputIt2>
        WN_FORCE_INLINE wn_bool equal(_InputIt1 _first1, _InputIt1 _last1, _InputIt2 _first2, _InputIt2 _last2) {
            #ifdef __WN_HAS_CPP14_STD_EQUAL_AVAILABLE
                return(std::equal(_first1, _last1, _first2, _last2));
            #else
                for (; (_first1 != _last1 && _first2 != _last2); ++_first1, ++_first2) {
                    if (*_first1 != *_first2) {
                        return(wn_false);
                    }
                }

                return(_first1 == _last1 && _first2 == _last2);
            #endif
        }

        template <typename _InputIt1, typename _InputIt2, typename _Predicate>
        WN_FORCE_INLINE wn_bool equal_if(_InputIt1 _first1, _InputIt1 _last1, _InputIt2 _first2, _Predicate _p) {
           return(std::equal(_first1, _last1, _first2, _p));
        }

        template <typename _InputIt1, typename _InputIt2, typename _Predicate>
        WN_FORCE_INLINE wn_bool equal_if(_InputIt1 _first1, _InputIt1 _last1, _InputIt2 _first2, _InputIt2 _last2, _Predicate _p) {
            #ifdef __WN_HAS_CPP14_STD_EQUAL_AVAILABLE
                return(std::equal(_first1, _last1, _first2, _last2, _p));
            #else
                for (; (_first1 != _last1 && _first2 != _last2); ++_first1, ++_first2) {
                    if (!_p(*_first1, *_first2)) {
                        return(wn_false);
                    }
                }

                return(_first1 == _last1 && _first2 == _last2);
            #endif
        }
    }
}

#endif // __WN_CORE_ALGORITHM_H__