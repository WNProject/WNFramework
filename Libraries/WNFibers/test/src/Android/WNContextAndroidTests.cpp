// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNTesting/inc/WNTestHarness.h"
#include "WNFibers/src/Android/WNContext.h"

int foo(volatile int* x, volatile int* y, ucontext_t* _ucontext) {
  EXPECT_LE(*x, 100);
  EXPECT_EQ(*x, *y + 1);

  return wn_setcontext(_ucontext);
}

TEST(context_android, basic) {
  ucontext_t ucontext;

  ::memset(&ucontext, 0, sizeof(ucontext_t));

  volatile int x = 0;
  volatile int y = 0;
  int result = wn_getcontext(&ucontext);

  y = x;
  x += 1;

  if (x <= 100) {
    result = foo(&x, &y, &ucontext);
  }

  EXPECT_EQ(x, 101);
}