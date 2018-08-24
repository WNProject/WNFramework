// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#ifndef __WN_FUNCTIONAL_DEFER_H__
#define __WN_FUNCTIONAL_DEFER_H__

#include "WNFunctional/inc/WNFunction.h"

namespace wn {
namespace functional {

class defer {
public:
  defer(functional::function<void()> _function) : m_function(_function) {}

  ~defer() {
    if (m_function) {
      m_function();
    }
  }

  void cancel() {
    m_function = nullptr;
  }

private:
  functional::function<void()> m_function;
};

}  // namespace functional
}  // namespace wn

#endif  // __WN_FUNCTIONAL_DEFER_H__
