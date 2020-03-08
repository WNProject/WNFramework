// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNFileSystem/inc/WNFactory.h"
#include "WNMemory/inc/allocator.h"
#include "WNScripting/inc/WNFactory.h"
#include "executable_test/inc/WNTestHarness.h"

TEST(scripting_engine_factory, creation) {
  wn::scripting::factory factory;
  wn::testing::allocator allocator;
  wn::file_system::mapping_ptr mapping =
      wn::file_system::factory(&allocator, wn::testing::k_executable_data)
          .make_mapping(
              &allocator, wn::file_system::mapping_type::memory_backed);
  wn::memory::unique_ptr<wn::scripting::engine> engine = factory.get_engine(
      &allocator, wn::scripting::scripting_engine_type::jit_engine,
      mapping.get(), wn::logging::get_null_logger());
}
