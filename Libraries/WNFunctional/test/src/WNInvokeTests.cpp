// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNFunctional/inc/WNInvoke.h"
#include "executable_test/inc/WNTestHarness.h"

// test helpers

void no_return_no_parameters() {
  // do nothing
}

int return_no_parameters() {
  return 5;
}

int return_parameters(int _x) {
  return _x;
}

struct object {
  void no_return_no_parameters() {
    // do nothing
  }

  int return_no_parameters() {
    return 5;
  }

  int return_parameters(int _x) {
    return _x;
  }

  int set_member(int _x) {
    int t = m_y;

    m_y = _x;

    return t;
  }

  int get_member() {
    return m_y;
  }

  int m_y = 1;
};

// tests

TEST(invoke, static_functions) {
  wn::functional::invoke(no_return_no_parameters);

  EXPECT_EQ(wn::functional::invoke(return_no_parameters), 5);
  EXPECT_EQ(wn::functional::invoke(return_parameters, 6), 6);
}

TEST(invoke, member_functions) {
  object o;

  wn::functional::invoke(&object::no_return_no_parameters, o);
  wn::functional::invoke(&object::no_return_no_parameters, &o);

  EXPECT_EQ(wn::functional::invoke(&object::return_no_parameters, o), 5);
  EXPECT_EQ(wn::functional::invoke(&object::return_parameters, o, 6), 6);
  EXPECT_EQ(wn::functional::invoke(&object::set_member, o, 2), 1);
  EXPECT_EQ(wn::functional::invoke(&object::get_member, o), 2);
  EXPECT_EQ(wn::functional::invoke(&object::return_no_parameters, &o), 5);
  EXPECT_EQ(wn::functional::invoke(&object::return_parameters, &o, 6), 6);
  EXPECT_EQ(wn::functional::invoke(&object::set_member, &o, 1), 2);
  EXPECT_EQ(wn::functional::invoke(&object::get_member, &o), 1);
}

TEST(invoke, lambdas) {
  auto lambda1 = []() {};
  auto lambda2 = []() { return 5; };
  auto lambda3 = [](int _x) { return _x; };

  wn::functional::invoke(lambda1);

  EXPECT_EQ(wn::functional::invoke(lambda2), 5);
  EXPECT_EQ(wn::functional::invoke(lambda3, 6), 6);
}

TEST(invoke, functors) {
  struct functor {
    void operator()() {
      // do nothing
    }

    int operator()(int _x) {
      return _x;
    }
  };

  functor f;

  wn::functional::invoke(f);

  EXPECT_EQ(wn::functional::invoke(f, 5), 5);
}
