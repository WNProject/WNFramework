// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNMemory/inc/WNAllocator.h"
#include "WNFileSystem/inc/WNFactory.h"
#include "WNScripting/inc/WNFactory.h"
#include "WNScripting/test/inc/Common.h"
#include "WNTesting/inc/WNTestHarness.h"

TEST(scripting_engine_factory, creation) {
  wn::scripting::factory factory;
  wn::testing::allocator allocator;
  wn::scripting::type_validator validator(&allocator);
  wn::file_system::mapping_ptr mapping =
      wn::file_system::factory().make_mapping(
          wn::file_system::mapping_type::memory_backed, &allocator);
  wn::memory::unique_ptr<wn::scripting::engine> engine =
      factory.get_engine(wn::scripting::scripting_engine_type::jit_engine,
          &validator, mapping.get(), WNLogging::get_null_logger(), &allocator);
}
