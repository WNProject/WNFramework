// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_CORE_TYPE_TRAITS_H__
#define __WN_CORE_TYPE_TRAITS_H__

#include "WNCore/inc/Internal/WNExtendedTypes.h"

#include <type_traits>

namespace wn {
    namespace core {
        template <const wn_bool _Value, typename _Type = wn_void>
        struct enable_if : std::enable_if<_Value, _Type> {};

        #ifdef __WN_HAS_CPP14_STD_ENABLE_IF_T
            template <const wn_bool _Value, typename _Type = wn_void>
            using enable_if_t = std::enable_if_t<_Value, _Type>;
        #else
            template <const wn_bool _Value, typename _Type = wn_void>
            using enable_if_t = typename enable_if<_Value, _Type>::type;
        #endif

        template <typename _Type>
        struct decay : std::decay<_Type> {};

        #ifdef __WN_HAS_CPP14_STD_DECAY_T
            template <typename _Type>
            using decay_t = std::decay_t<_Type>;
        #else
            template <typename _Type>
            using decay_t = typename decay<_Type>::type;
        #endif

        template<typename _Type>
        struct exists : std::true_type {};

        template <typename _Type, const _Type _Value>
        struct integral_constant : std::integral_constant<_Type, _Value> {};

        template <const wn_size_t _Value>
        struct index_constant : integral_constant<wn_size_t, _Value> {};

        #ifdef __WN_HAS_CPP14_STD_INTEGER_SEQUENCE
            template <typename _Type, const _Type... _Values>
            struct integral_sequence : std::integer_sequence<_Type, _Values...> {};

            template <const wn_size_t... _Values>
            struct index_sequence : std::index_sequence<_Values...> {};
        #else
            template <typename _Type, const _Type... _Values>
            struct integral_sequence {
                typedef _Type value_type;

                static WN_FORCE_INLINE wn_size_t size() {
                    return(sizeof...(_Values));
                }
            };

            template <const wn_size_t... _Values>
            struct index_sequence : integral_sequence<wn_size_t, _Values...> {};
        #endif

        template <const wn_bool _Value>
        struct boolean_constant : integral_constant<wn_bool, _Value> {};

        template <const wn_bool... _Values>
        struct boolean_sequence : integral_sequence<wn_bool, _Values...> {};

        template <typename _Type1, typename _Type2>
        struct is_same : std::is_same<_Type1, _Type2> {};

        template <typename _Type1, typename _Type2>
        struct is_same_decayed : is_same<decay_t<_Type1>, decay_t<_Type2>> {};

        template <const wn_bool... _Values>
        struct boolean_and : is_same<boolean_sequence<_Values...>, boolean_sequence<(_Values || wn_true)...>> {};

        template <const wn_bool... _Values>
        struct boolean_or : boolean_constant<!boolean_and<!_Values...>::value> {};

        template <typename _Type, typename... _Types>
        struct are_same : boolean_and<is_same<_Type, _Types>::value...> {};

        template <typename _Type, typename... _Types>
        struct are_same_decayed : are_same<decay_t<_Type>, decay_t<_Types>...> {};

        #ifdef __WN_HAS_CPP14_STD_IS_NULL_POINTER
            template <typename _Type>
            struct is_null_pointer : std::is_null_pointer<_Type> {};
        #else
            template <typename _Type>
            struct is_null_pointer : is_same_decayed<wn_nullptr_t, _Type> {};
        #endif

            template <typename... _Types>
            struct common_type : std::common_type<_Types...> {};

        #ifdef __WN_HAS_CPP14_STD_COMMON_TYPE_T
            template <typename... _Types>
            using common_type_t = std::common_type_t<_Types...>;
        #else
            template <typename... _Type>
            using common_type_t = typename common_type<_Type...>::type;
        #endif

        template <typename _Type>
        struct result_of : std::result_of<_Type> {};

        #ifdef __WN_HAS_CPP14_STD_RESULT_OF_T
            template <typename _Type>
            using result_of_t = std::result_of_t<_Type>;
        #else
            template <typename _Type>
            using result_of_t = typename result_of<_Type>::type;
        #endif

        namespace internal {
            template <typename _Type>
            struct is_extended_type : std::false_type {};

            template <typename _TraitsType>
            struct is_extended_type<arithmetic_type<_TraitsType>> : std::true_type {};

            template <typename _Type, typename = enable_if_t<wn_true>>
            struct is_floating_point : std::is_floating_point<_Type> {};

            template <typename _Type>
            struct is_floating_point<_Type, typename enable_if<exists<typename _Type::traits_type::conversion_type>::value>::type> : std::true_type {};

            template <typename _Type, typename = enable_if_t<wn_true>>
            struct is_fixed_point : std::false_type {};

            template <typename _Type>
            struct is_fixed_point<_Type, typename enable_if<exists<typename _Type::traits_type::scale_type>::value>::type> : std::true_type {};

            template <typename _Type, typename = enable_if_t<wn_true>>
            struct is_signed : std::is_signed<_Type> {};

            template <typename _Type>
            struct is_signed<_Type, typename enable_if<is_floating_point<_Type>::value>::type> : std::true_type {};

            template <typename _Type>
            struct is_signed<_Type, typename enable_if<boolean_and<is_extended_type<_Type>::value, !is_floating_point<_Type>::value>::value>::type> : std::is_signed<typename _Type::value_type> {};

            template <typename _Type, typename = enable_if_t<wn_true>>
            struct is_unsigned : boolean_or<std::is_unsigned<_Type>::value, is_floating_point<_Type>::value> {};

            template <typename _Type>
            struct is_unsigned<_Type, typename enable_if<is_extended_type<_Type>::value>::type> : std::is_unsigned<typename _Type::value_type> {};

            template <typename _Type>
            struct is_arithmetic : std::is_arithmetic<_Type> {};

            template <typename traits_type>
            struct is_arithmetic<arithmetic_type<traits_type>> : std::true_type {};

            template <typename _Type, const wn_bool _Expression, typename _Return, typename... _Arguments>
            struct same_return : std::false_type {};

            template <typename _Type, typename _Return, typename... _Arguments>
            struct same_return<_Type, wn_true, _Return, _Arguments...> : is_same<result_of_t<_Type(_Arguments...)>, _Return> {};

            template <typename _Function, typename _Return, typename... _Arguments>
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
                enum : wn_bool {
                    value = same_return<_Function, callable<_Function>::value, _Return, _Arguments...>::value
                };
            };
        }

        template <typename _Type>
        struct is_extended_type : internal::is_extended_type<decay_t<_Type>> {};

        template <typename _Type>
        struct is_arithmetic : internal::is_arithmetic<decay_t<_Type>> {};

        template <typename _Type>
        struct is_signed : internal::is_signed<decay_t<_Type>> {};

        template <typename _Type>
        struct is_unsigned : internal::is_unsigned<decay_t<_Type>> {};

        template <typename _Type>
        struct is_floating_point : internal::is_floating_point<decay_t<_Type>> {};

        template <typename _Type>
        struct is_fixed_point : internal::is_fixed_point<decay_t<_Type>> {};

        template <typename _Type>
        struct is_real : boolean_or<is_fixed_point<_Type>::value, is_floating_point<_Type>::value> {};

        template <typename _Function, typename _Type>
        struct is_callable : std::false_type {};

        template <typename _Function, typename _Return, typename... _Arguments>
        struct is_callable<_Function, _Return(_Arguments...)> : internal::is_callable<_Function, _Return, _Arguments...> {};
    }
}

#endif // __WN_CORE_TYPE_TRAITS_H__
