// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_CORE_TYPE_TRAITS_H__
#define __WN_CORE_TYPE_TRAITS_H__

#include "WNCore/inc/Internal/WNExtendedTypes.h"

#include <type_traits>

namespace wn {
    namespace internal {
        template <typename _type>
        struct is_floating_point : std::is_floating_point<_type> {};

        template <typename _type, typename conversion_type>
        struct is_floating_point<arithmetic_type<floating_point_traits<_type, conversion_type>>> : std::true_type {};

        template <typename _type>
        struct is_fixed_point : std::false_type {};

        template <typename _type, const wn_uint32 _precision>
        struct is_fixed_point<arithmetic_type<fixed_point_traits<_type, _precision>>> : std::true_type {};
    }

    template <typename _type, const _type _value>
    struct integral_constant : std::integral_constant<_type, _value> {};

    template <const wn_size_t _value>
    struct index_constant : integral_constant<wn_size_t, _value> {};

    template <const wn_bool _value>
    struct boolean_constant : integral_constant<wn_bool, _value> {};

    #ifdef __WN_HAS_CPP14_STL_INTEGER_SEQUENCE
        template <typename _type, const _type... _values>
        struct integral_sequence : std::integer_sequence<_type, _values...> {};

        template <const wn_size_t... _values>
        struct index_sequence : std::index_sequence<_values...> {};
    #else
        template <typename _type, const _type... _values>
        struct integral_sequence {
            typedef _type value_type;

            static WN_FORCE_INLINE wn_size_t size() {
                return(sizeof...(_values));
            }
        };

        template <const wn_size_t... _values>
        struct index_sequence : integral_sequence<wn_size_t, _values...> {};
    #endif

    template <const wn_bool... _values>
    struct boolean_sequence : integral_sequence<wn_bool, _values...> {};

    template <typename _type>
    struct decay : std::decay<_type> {};

    #ifdef __WN_HAS_CPP14_STL_DECAY_T
        template <typename _type>
        using decay_t = std::decay_t<_type>;
    #else
        template <typename _type>
        using decay_t = typename decay<_type>::type;
    #endif

    namespace internal {
        template <typename _type>
        struct is_signed : boolean_constant<(std::is_signed<_type>::value || is_floating_point<_type>::value)> {};

        template <typename _type, const wn_uint32 _precision>
        struct is_signed<arithmetic_type<fixed_point_traits<_type, _precision>>> : std::is_signed<_type>{};

        template <typename _type>
        struct is_unsigned : std::is_unsigned<_type> {};

        template <typename _type, const wn_uint32 _precision>
        struct is_unsigned<arithmetic_type<fixed_point_traits<_type, _precision>>> : std::is_unsigned<_type>{};

        template <typename _type>
        struct is_arithmetic : std::is_arithmetic<_type> {};

        template <typename traits_type>
        struct is_arithmetic<arithmetic_type<traits_type>> : std::true_type {};
    }

    template <typename _type>
    struct is_floating_point : internal::is_floating_point<typename decay<_type>::type> {};

    template <typename _type>
    struct is_fixed_point : internal::is_fixed_point<typename decay<_type>::type> {};

    template <typename _type>
    struct is_signed : internal::is_signed<typename decay<_type>::type> {};

    template <typename _type>
    struct is_unsigned : internal::is_unsigned<typename decay<_type>::type> {};

    template <typename _type>
    struct is_arithmetic : internal::is_arithmetic<typename decay<_type>::type> {};

    template <typename _type>
    struct is_real : boolean_constant<(is_fixed_point<_type>::value || is_floating_point<_type>::value)> {};

    template <typename _type1, typename _type2>
    struct is_same : std::is_same<_type1, _type2> {};

    template <typename _type1, typename _type2>
    struct is_same_decayed : is_same<typename decay<_type1>::type, typename decay<_type2>::type> {};

    template <const wn_bool... _values>
    struct boolean_and : is_same<boolean_sequence<_values...>, boolean_sequence<(_values || wn_true)...>> {};

    template <const wn_bool... _values>
    struct boolean_or : boolean_constant<!boolean_and<_values...>::value> {};

    template <typename _type, typename... _types>
    struct are_same : boolean_and<is_same<_type, _types>::value...> {};

    template <typename _type, typename... _types>
    struct are_same_decayed : are_same<typename decay<_type>::type, typename decay<_types>::type...> {};

    #ifdef __WN_HAS_CPP14_STL_IS_NULL_POINTER
        template <typename _type>
        struct is_null_pointer : std::is_null_pointer<_type> {};
    #else
        template <typename _type>
        struct is_null_pointer : is_same_decayed<wn_nullptr_t, _type> {};
    #endif

    template <const wn_bool _value, typename _type = wn_void>
    struct enable_if : std::enable_if<_value, _type> {};

    #ifdef __WN_HAS_CPP14_STL_ENABLE_IF_T
        template <const wn_bool _value, typename _type = wn_void>
        using enable_if_t = std::enable_if_t<_value, _type>;
    #else
        template <const wn_bool _value, typename _type = wn_void>
        using enable_if_t = typename enable_if<_value, _type>::type;
    #endif

    template <typename... _types>
    struct common_type : std::common_type<_types...> {};

    #ifdef __WN_HAS_CPP14_STL_COMMON_TYPE_T
        template <typename... _types>
        using common_type_t = std::common_type_t<_types...>;
    #else
        template <typename... _type>
        using common_type_t = typename common_type<_type...>::type;
    #endif

    template <typename _type>
    struct result_of : std::result_of<_type> {};

    #ifdef __WN_HAS_CPP14_STL_RESULT_OF_T
        template <typename _type>
        using result_of_t = std::result_of_t<_type>;
    #else
        template <typename _type>
        using result_of_t = typename result_of<_type>::type;
    #endif

    namespace internal {
        template <typename _Type, const wn_bool _Expression, typename _Result, typename... _Arguments>
        struct same_result : std::false_type {};

        template <typename _Type, typename _Result, typename... _Arguments>
        struct same_result<_Type, wn_true, _Result, _Arguments...> : boolean_constant<is_same<typename result_of<_Type(_Arguments...)>::type, _Result>::value> {};

        template <typename _Function, typename _Result, typename... _Arguments>
        class is_callable {
            typedef wn_char (&invalid)[1];
            typedef wn_char (&valid)[2];

            template <typename _Type>
            struct helper;

            template <typename _Type>
            static valid checker(helper<decltype(std::declval<_Type>()(std::declval<_Arguments>()...))>*);

            template <typename _Type>
            static invalid checker(...);

            template <typename _Type>
            struct callable : boolean_constant<sizeof(checker<_Type>(0)) == sizeof(valid)> {};

        public:
            enum {
                value = same_result<_Function, callable<_Function>::value, _Result, _Arguments...>::value
            };
        };
    }

    template <typename _Function, typename _Result, typename... _Arguments>
    struct is_callable : internal::is_callable<_Function, _Result, _Arguments...> {};
}

#endif // __WN_CORE_TYPE_TRAITS_H__