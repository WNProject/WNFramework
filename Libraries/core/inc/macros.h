// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#ifndef __WN_CORE_MACRO_UTILITIES_H__
#define __WN_CORE_MACRO_UTILITIES_H__

// Based on partial solutions at
// http://stackoverflow.com/questions/9183993/msvc-variadic-macro-expansion
// http://stackoverflow.com/questions/1872220/is-it-possible-to-iterate-over-arguments-in-variadic-macros

#define WN_FOREACH_1(CALL, PARAM, X) CALL(PARAM, X)
#define WN_FOREACH_2(CALL, PARAM, X, _1)                                       \
  CALL(PARAM, X) WN_FOREACH_1(CALL, PARAM, _1)
#define WN_FOREACH_3(CALL, PARAM, X, _2, _1)                                   \
  CALL(PARAM, X) WN_FOREACH_2(CALL, PARAM, _2, _1)
#define WN_FOREACH_4(CALL, PARAM, X, _3, _2, _1)                               \
  CALL(PARAM, X) WN_FOREACH_3(CALL, PARAM, _3, _2, _1)
#define WN_FOREACH_5(CALL, PARAM, X, _4, _3, _2, _1)                           \
  CALL(PARAM, X) WN_FOREACH_4(CALL, PARAM, _4, _3, _2, _1)
#define WN_FOREACH_6(CALL, PARAM, X, _5, _4, _3, _2, _1)                       \
  CALL(PARAM, X) WN_FOREACH_5(CALL, PARAM, _5, _4, _3, _2, _1)
#define WN_FOREACH_7(CALL, PARAM, X, _6, _5, _4, _3, _2, _1)                   \
  CALL(PARAM, X) WN_FOREACH_6(CALL, PARAM, _6 _5, _4, _3, _2, _1)
#define WN_FOREACH_8(CALL, PARAM, X, _7, _6, _5, _4, _3, _2, _1)               \
  CALL(PARAM, X) WN_FOREACH_7(CALL, PARAM, _7, _6, _5, _4, _3, _2, _1)
#define WN_FOREACH_9(CALL, PARAM, X, _8, _7, _6, _5, _4, _3, _2, _1)           \
  CALL(PARAM, X) WN_FOREACH_8(CALL, PARAM, _8, _7, _6, _5, _4, _3, _2, _1)
#define WN_FOREACH_10(CALL, PARAM, X, _9, _8, _7, _6, _5, _4, _3, _2, _1)      \
  CALL(PARAM, X) WN_FOREACH_8(CALL, PARAM, _9, _8, _7, _6, _5, _4, _3, _2, _1)

#define REMOVE_COMMA(x, y) x y

#define RETURN_ARG_COUNT(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, count, ...)  \
  count
#define EXPAND_ARGS(args) RETURN_ARG_COUNT args
#define COUNT_ARGS_MAX10(...)                                                  \
  EXPAND_ARGS((__VA_ARGS__, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0))

#define RUN_FOREACH2(count) WN_FOREACH_##count
#define RUN_FOREACH1(count) RUN_FOREACH2(count)
#define RUN_FOREACH(count) RUN_FOREACH1(count)

#define FOR_EACH(name, param, ...)                                             \
  REMOVE_COMMA(                                                                \
      RUN_FOREACH(COUNT_ARGS_MAX10(__VA_ARGS__)), (name, param, __VA_ARGS__))

#define ATTACH(first, second) first##second

#endif  // __WN_CORE_MACRO_UTILITIES_H__
