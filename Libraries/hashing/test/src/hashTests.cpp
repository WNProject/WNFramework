// Copyright (c) 2021, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNLogging/inc/WNConsoleLogger.h"
#include "WNLogging/inc/WNLog.h"
#include "executable_test/inc/WNTestHarness.h"
#include "hashing/inc/hash.h"

using wn::hashing::hash;

TEST(hashing, _32_bit) {
  hash<32> hash(0);

  EXPECT_EQ(0x02CC5D05u, hash.get_data());
  const char* c = "Hello World!";
  hash.append(
      wn::containers::contiguous_range<const char>(c, wn::memory::strlen(c)));

  EXPECT_EQ(0x0BD69788u, hash.get_data());
  hash.append(
      wn::containers::contiguous_range<const char>(c, wn::memory::strlen(c)));

  EXPECT_EQ(0x9D05DC75u, hash.get_data());
}

TEST(hashing, _64_bit) {
  hash<64> hash(0);

  EXPECT_EQ(0xEF46DB3751D8E999, hash.get_data());
  const char* c = "Hello World!";

  hash.append(
      wn::containers::contiguous_range<const char>(c, wn::memory::strlen(c)));
  EXPECT_EQ(0xA52B286A3E7F4D91u, hash.get_data());

  hash.append(
      wn::containers::contiguous_range<const char>(c, wn::memory::strlen(c)));
  EXPECT_EQ(0x9A0BEADF4A70923Cu, hash.get_data());
}

TEST(hashing, _96_bit) {
  hash<96> hash(0, 0);
  auto hd = hash.get_data();

  EXPECT_EQ(0x51D8E999u, hd.parts[0]);
  EXPECT_EQ(0xEF46DB37u, hd.parts[1]);
  EXPECT_EQ(0x02CC5D05u, hd.parts[2]);

  const char* c = "Hello World!";

  hash.append(
      wn::containers::contiguous_range<const char>(c, wn::memory::strlen(c)));

  hd = hash.get_data();
  EXPECT_EQ(0x3E7F4D91u, hd.parts[0]);
  EXPECT_EQ(0xA52B286Au, hd.parts[1]);
  EXPECT_EQ(0x0BD69788u, hd.parts[2]);

  hash.append(
      wn::containers::contiguous_range<const char>(c, wn::memory::strlen(c)));

  hd = hash.get_data();
  EXPECT_EQ(0x4A70923Cu, hd.parts[0]);
  EXPECT_EQ(0x9A0BEADFu, hd.parts[1]);
  EXPECT_EQ(0x9D05DC75u, hd.parts[2]);
}
