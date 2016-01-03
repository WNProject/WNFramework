// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNMemory/inc/WNAllocator.h"
#include "WNScripting/inc/WNFactory.h"
#include "WNScripting/test/inc/Common.h"
#include "WNTesting/inc/WNTestHarness.h"

TEST(scripting_engine_factory, creation) {
  wn::scripting::factory factory;
  wn::memory::default_expanding_allocator<50> allocator;
  wn::scripting::type_validator validator(&allocator);
  wn::scripting::file_based_manager manager(&allocator);
  wn::memory::allocated_ptr<wn::scripting::engine> engine =
      factory.get_engine(wn::scripting::scripting_engine_type::jit_engine,
          &validator, &manager, WNLogging::get_null_logger(), &allocator);
}
