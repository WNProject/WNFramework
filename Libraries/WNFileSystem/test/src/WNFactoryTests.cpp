// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "executable_test/inc/WNTestHarness.h"
#include "WNFileSystem/inc/WNFactory.h"

#ifdef _WN_POSIX
#include <sys/stat.h>
#endif

using factory = ::testing::TestWithParam<wn::file_system::mapping_type>;

TEST_P(factory, make_mapping) {
  wn::testing::allocator allocator;

  {
    wn::file_system::factory f(&allocator, wn::testing::k_executable_data);
    const wn::file_system::mapping_ptr mp =
        f.make_mapping(&allocator, GetParam());

    ASSERT_NE(mp, nullptr);
  }
}

INSTANTIATE_TEST_CASE_P(all_mappings, factory,
    ::testing::Values(wn::file_system::mapping_type::scratch_directory,
        wn::file_system::mapping_type::memory_backed,
        wn::file_system::mapping_type::executable_directory,
        wn::file_system::mapping_type::current_working_directory));

TEST(factory, make_mapping_from_path) {
  wn::testing::allocator allocator;

  {
    wn::containers::string path(&allocator);

#ifdef _WN_WINDOWS  // get the temp path in windows
    wn::containers::array<CHAR, MAX_PATH + 1> buffer = {0};
    const DWORD result =
        ::GetTempPathA(static_cast<DWORD>(buffer.size()), buffer.data());

    path.assign(buffer.data(), result);
#elif defined _WN_ANDROID
    path = "/data/local/tmp/";
#elif defined _WN_POSIX  // get the temp path in linux
    static const char* vars[4] = {"TMPDIR", "TMP", "TEMP", "TEMPDIR"};

    for (const char* var : vars) {
      const char* temp_path = ::getenv(var);

      if (temp_path) {
        struct stat dstat;

        if (::stat(temp_path, &dstat) == 0) {
          if (S_ISDIR(dstat.st_mode)) {
            path = temp_path;

            if (path.size() > 0) {
              if (path.back() != '/') {
                path += '/';
              }

              break;
            }
          }
        }
      }
    }

    if (path.empty()) {
      struct stat dstat;

      if (::stat("/tmp", &dstat) == 0) {
        if (S_ISDIR(dstat.st_mode)) {
          path = "/tmp/";
        }
      }
    }
#else
#error "Must specify code for specific platform"
#endif

    ASSERT_FALSE(path.empty());

    wn::file_system::factory f(&allocator, wn::testing::k_executable_data);
    const wn::file_system::mapping_ptr mp = f.make_mapping(&allocator, path);

    ASSERT_NE(mp, nullptr);
  }
}