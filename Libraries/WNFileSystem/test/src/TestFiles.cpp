
// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

# include "TestFiles.h"

namespace TestFiles {

namespace {
const char _0[] = "b/b.txt";
const char _1[] = "a.txt";
const char _0_data[] = {
        0x48,0x65,0x6C,0x6C,0x6F,0x20,0x57,0x6F,0x72,0x6C,0x64,0x20,
        0x32
};
const char _1_data[] = {
        0x48,0x65,0x6C,0x6C,0x6F,0x20,0x57,0x6F,0x72,0x6C,0x64
};

} // namespace

const wn::containers::contiguous_range<
    const wn::core::pair<wn::containers::string_view,
                         wn::containers::string_view>> get_files() {
  static const wn::core::pair<wn::containers::string_view,
    wn::containers::string_view> file_data[] = {
        { wn::containers::string_view(_0, 7), wn::containers::string_view(_0_data, 13) },
        { wn::containers::string_view(_1, 5), wn::containers::string_view(_1_data, 11) }
  };
  return wn::containers::contiguous_range<
    const wn::core::pair<wn::containers::string_view,
                         wn::containers::string_view>>(&file_data[0],
                            sizeof(file_data)/sizeof(file_data[0]));
}
} // namespace TestFiles