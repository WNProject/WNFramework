// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNExecutableTest/inc/WNTestHarness.h"
#include "WNMultiTasking/src/Android/WNContext.h"

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

void new_func(void* f) {
  int* y = reinterpret_cast<int*>(f);
  *y += 1;
  volatile int foo = *y + 1;
  volatile int bar = *y * 20;
  volatile int baz = *y * 33;
  (void)foo;
  (void)bar;
  (void)baz;
}

TEST(context_android, make_context) {
  ucontext_t ucontext;
  ::memset(&ucontext, 0, sizeof(ucontext_t));

  ucontext_t my_context;
  ::memset(&my_context, 0, sizeof(ucontext_t));

  char stack[1024];
  memset(stack, 0, sizeof(stack));

  volatile int x = 0;
  int y = 0;

  // Set up new context to move to.
  wn_getcontext(&ucontext);
  ucontext.uc_stack.ss_sp = stack;
  ucontext.uc_stack.ss_size = 1024;

  // When this is done we want to my_context.
  ucontext.uc_link = &my_context;
  wn_makecontext(&ucontext, new_func, reinterpret_cast<void*>(&y));

  // We we are done, return to here.
  wn_getcontext(&my_context);
  x += 1;
  EXPECT_EQ(y, x - 1);
  if (x <= 100) {
    wn_setcontext(&ucontext);
  }
  EXPECT_EQ(101, x);
}

struct contexts {
  ucontext_t* mine;
  ucontext_t* next;
  int data;
};

void new_func1(void* f) {
  contexts* ctx = static_cast<contexts*>(f);
  ctx->data += 102;
  wn_swapcontext(ctx->mine, ctx->next);
  ctx->data += 40;
}

void new_func2(void* f) {
  contexts* ctx = static_cast<contexts*>(f);
  ctx->data += 12;
  wn_swapcontext(ctx->mine, ctx->next);
  ctx->data -= 100;
}

TEST(context_android, swap_context) {
  ucontext_t main_context;
  ucontext_t new_func1_context;
  ucontext_t new_func2_context;

  char stack1[1024];
  char stack2[1024];

  ::memset(&main_context, 0, sizeof(ucontext_t));
  ::memset(&new_func1_context, 0, sizeof(ucontext_t));
  ::memset(&new_func2_context, 0, sizeof(ucontext_t));

  new_func1_context.uc_stack.ss_sp = stack1;
  new_func1_context.uc_stack.ss_size = 1024;

  new_func2_context.uc_stack.ss_sp = stack2;
  new_func2_context.uc_stack.ss_size = 1024;

  new_func1_context.uc_link = &main_context;
  new_func2_context.uc_link = &new_func1_context;

  contexts f1_ctx = {&new_func1_context, &new_func2_context, 32};
  contexts f2_ctx = {&new_func2_context, &new_func1_context, 41};

  wn_makecontext(&new_func1_context, new_func1, &f1_ctx);
  wn_makecontext(&new_func2_context, new_func2, &f2_ctx);

  wn_swapcontext(&main_context, &new_func2_context);

  EXPECT_EQ(32 + 102 + 40, f1_ctx.data);
  EXPECT_EQ(41 + 12 - 100, f2_ctx.data);
}
