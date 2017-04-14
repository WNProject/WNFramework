// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNContainers/inc/WNStringView.h"
#include "WNExecutableTest/inc/WNTestHarness.h"

static const char test_string1[] =
    "this is a very long string that will not get optimized away";
static const char test_string2[] =
    "another very long string that should not get optimized out";
static const size_t test_string1_length = sizeof(test_string1) - 1;
static const size_t test_string2_length = sizeof(test_string2) - 1;

TEST(string_view, construction) {
  const char* raw = "string1";
  wn::containers::string_view v1;
  const wn::containers::string_view v2(nullptr);
  const wn::containers::string_view v3(nullptr, nullptr);
  const wn::containers::string_view v4(raw, raw + 7);
  const wn::containers::string_view v5(raw, 7);
  const wn::containers::string_view v6(raw);
  wn::containers::string_view v7(v6);

  EXPECT_TRUE(v1.empty());
  EXPECT_EQ(v1.data(), nullptr);
  EXPECT_TRUE(v2.empty());
  EXPECT_EQ(v2.data(), nullptr);
  EXPECT_TRUE(v3.empty());
  EXPECT_EQ(v3.data(), nullptr);
  EXPECT_FALSE(v4.empty());
  EXPECT_EQ(v4.data(), raw);
  EXPECT_EQ(v4, "string1");
  EXPECT_FALSE(v5.empty());
  EXPECT_EQ(v5.data(), raw);
  EXPECT_EQ(v5, "string1");
  EXPECT_FALSE(v6.empty());
  EXPECT_EQ(v6.data(), raw);
  EXPECT_EQ(v6, "string1");
  EXPECT_FALSE(v7.empty());
  EXPECT_EQ(v7.data(), raw);
  EXPECT_EQ(v7, "string1");

  wn::containers::string_view v8(std::move(v7));

  EXPECT_TRUE(v7.empty());
  EXPECT_EQ(v7.data(), nullptr);
  EXPECT_FALSE(v8.empty());
  EXPECT_EQ(v8.data(), raw);
  EXPECT_EQ(v8, "string1");

  wn::containers::contiguous_range<const char> range(raw, 7);

  EXPECT_FALSE(range.empty());
  EXPECT_EQ(range.data(), raw);

  wn::containers::string_view v9(range);

  EXPECT_FALSE(v9.empty());
  EXPECT_EQ(v9.data(), raw);
  EXPECT_EQ(v9, "string1");

  wn::containers::string_view v10(std::move(range));

  EXPECT_TRUE(range.empty());
  EXPECT_EQ(range.data(), nullptr);
  EXPECT_FALSE(v10.empty());
  EXPECT_EQ(v10.data(), raw);
  EXPECT_EQ(v10, "string1");

  wn::testing::allocator test_allocator;
  const wn::containers::string s(&test_allocator, "string2");
  const wn::containers::string_view v11(s);
  const wn::containers::string_view v12(s, 7);
  const wn::containers::string_view v13(s, 1, 5);

  EXPECT_FALSE(v11.empty());
  EXPECT_EQ(v11.data(), s.data());
  EXPECT_EQ(v11, "string2");
  EXPECT_FALSE(v12.empty());
  EXPECT_EQ(v12.data(), s.data());
  EXPECT_EQ(v12, "string2");
  EXPECT_FALSE(v13.empty());
  EXPECT_EQ(v13.data(), s.data() + 1);
  EXPECT_EQ(v13, "tring");

  const wn::containers::string_view v14(raw, 1, 5);

  EXPECT_FALSE(v14.empty());
  EXPECT_EQ(v14.data(), raw + 1);
  EXPECT_EQ(v14, "tring");
}

TEST(string_view, assignment) {
  const char* raw = "string";
  wn::containers::string_view v1(raw);

  v1 = nullptr;

  wn::containers::string_view v2;

  v2 = raw;

  wn::containers::string_view v3;

  v3 = v2;

  EXPECT_TRUE(v1.empty());
  EXPECT_EQ(v1.data(), nullptr);
  EXPECT_FALSE(v2.empty());
  EXPECT_EQ(v2.data(), raw);
  EXPECT_FALSE(v3.empty());
  EXPECT_EQ(v3.data(), raw);

  wn::containers::string_view v4;

  v4 = std::move(v3);

  EXPECT_TRUE(v3.empty());
  EXPECT_EQ(v3.data(), nullptr);
  EXPECT_FALSE(v4.empty());
  EXPECT_EQ(v4.data(), raw);
}

TEST(string_view, element_access) {
  const wn::containers::string_view v1(test_string1);

  ASSERT_FALSE(v1.empty());
  EXPECT_EQ(v1.front(), 't');
  EXPECT_EQ(v1.back(), 'y');
  EXPECT_EQ(v1.at(2), 'i');
  EXPECT_EQ(v1[2], 'i');
  EXPECT_EQ(v1.data(), test_string1);

  const wn::containers::string_view v2(test_string1 + 1, 3);

  ASSERT_FALSE(v2.empty());
  EXPECT_EQ(v2.front(), 'h');
  EXPECT_EQ(v2.back(), 's');
  EXPECT_EQ(v2.at(1), 'i');
  EXPECT_EQ(v2[1], 'i');
  EXPECT_EQ(v2.data(), test_string1 + 1);
}

TEST(string_view, conversion) {
  wn::containers::string_view v(test_string1);
  wn::testing::allocator test_allocator;

  {
    wn::containers::string s(v.to_string(&test_allocator));

    EXPECT_EQ(s, test_string1);
  }
}

// capacity

TEST(string_view, empty) {
  const wn::containers::string_view v1;

  EXPECT_TRUE(v1.empty());

  wn::containers::string_view v2(test_string1);

  EXPECT_FALSE(v2.empty());

  v2.clear();

  EXPECT_TRUE(v2.empty());
}

TEST(string_view, size) {
  const wn::containers::string_view v1(test_string1);

  EXPECT_EQ(v1.size(), test_string1_length);
  EXPECT_EQ(v1.length(), test_string1_length);

  const wn::containers::string_view v2(test_string1 + 1, 3);

  EXPECT_EQ(v2.size(), 3);
  EXPECT_EQ(v2.length(), 3);
}

TEST(string_view, max_size) {
  const wn::containers::string_view v1(test_string1);

  EXPECT_EQ(v1.max_size(),
      std::numeric_limits<wn::containers::string_view::size_type>::max());

  const wn::containers::string_view v2(test_string1 + 1, 3);

  EXPECT_EQ(v2.max_size(),
      std::numeric_limits<wn::containers::string_view::size_type>::max());
}

// operations

TEST(string_view, clear) {
  wn::containers::string_view s(test_string1);

  EXPECT_EQ(s.size(), test_string1_length);
  EXPECT_GE(s.capacity(), test_string1_length);
  EXPECT_EQ(s.back(), 'y');

  s.clear();

  EXPECT_TRUE(s.empty());
  EXPECT_EQ(s.size(), 0);
  EXPECT_GE(s.capacity(), 0);
}

TEST(string_view, remove_prefix) {
  const char* string = "string";
  wn::containers::string_view v(string);

  ASSERT_FALSE(v.empty());
  EXPECT_EQ(v.front(), 's');
  EXPECT_EQ(v.back(), 'g');

  v.remove_prefix(2);

  EXPECT_EQ(v.front(), 'r');
  EXPECT_EQ(v.back(), 'g');
}

TEST(string_view, remove_suffix) {
  const char* string = "string";
  wn::containers::string_view v(string);

  ASSERT_FALSE(v.empty());
  EXPECT_EQ(v.front(), 's');
  EXPECT_EQ(v.back(), 'g');

  v.remove_suffix(2);

  EXPECT_EQ(v.front(), 's');
  EXPECT_EQ(v.back(), 'i');
}

TEST(string_view, substr) {
  const char* string = "string";
  const wn::containers::string_view v1(string);
  const wn::containers::string_view v2(v1.substr());

  ASSERT_EQ(v2.length(), 6);
  EXPECT_EQ(v2, "string");

  const wn::containers::string_view v3(v1.substr(4));

  ASSERT_EQ(v3.length(), 2);
  EXPECT_EQ(v3, "ng");

  const wn::containers::string_view v4(v1.substr(1, 3));

  ASSERT_EQ(v4.length(), 3);
  EXPECT_EQ(v4, "tri");
}

TEST(string_view, swap) {
  const char* string1 = "1string2";
  const char* string2 = "3string4";
  wn::containers::string_view v1(string1);
  wn::containers::string_view v2(string2);

  EXPECT_FALSE(v1.empty());
  EXPECT_FALSE(v2.empty());
  ASSERT_EQ(v1.data(), string1);
  ASSERT_EQ(v2.data(), string2);
  EXPECT_EQ(v1.front(), '1');
  EXPECT_EQ(v2.front(), '3');
  EXPECT_EQ(v1.back(), '2');
  EXPECT_EQ(v2.back(), '4');

  v1.swap(v2);

  EXPECT_FALSE(v1.empty());
  EXPECT_FALSE(v2.empty());
  ASSERT_EQ(v1.data(), string2);
  ASSERT_EQ(v2.data(), string1);
  EXPECT_EQ(v1.front(), '3');
  EXPECT_EQ(v2.front(), '1');
  EXPECT_EQ(v1.back(), '4');
  EXPECT_EQ(v2.back(), '2');
}

// searching

TEST(string_view, find) {
  const char* string = "      string";
  const wn::containers::string_view v(string);

  EXPECT_FALSE(v.empty());

  size_t pos = v.find("ing");

  EXPECT_EQ(pos, 9);

  pos = v.find('t');

  EXPECT_EQ(pos, 7);
}

TEST(string_view, rfind) {
  const wn::containers::string_view v(test_string1);

  EXPECT_FALSE(v.empty());

  size_t pos = v.rfind("a");

  EXPECT_EQ(pos, 57);

  pos = v.rfind('o');

  EXPECT_EQ(pos, 45);

  pos = v.rfind("x");

  EXPECT_EQ(pos, wn::containers::string_view::npos);
}

TEST(string_view, find_first_of) {
  const wn::containers::string_view v(test_string1);

  EXPECT_FALSE(v.empty());

  size_t pos = v.find_first_of('s');

  EXPECT_EQ(pos, 3);

  pos = v.find_first_of("tr");

  EXPECT_EQ(pos, 0);

  pos = v.find_first_of("g");

  EXPECT_EQ(pos, 18);

  pos = v.find_first_of("x");

  EXPECT_EQ(pos, wn::containers::string_view::npos);
}

TEST(string_view, find_last_of) {
  const char* string1 = "      strings";
  const wn::containers::string_view v1(string1);

  EXPECT_FALSE(v1.empty());

  size_t pos = v1.find_last_of('s');

  EXPECT_EQ(pos, 12);

  pos = v1.find_last_of("tr");

  EXPECT_EQ(pos, 8);

  pos = v1.find_last_of("g");

  EXPECT_EQ(pos, 11);

  pos = v1.find_last_of("xyz");

  EXPECT_EQ(pos, wn::containers::string_view::npos);
}

TEST(string_view, find_first_not_of) {
  const char* string = "      string";
  const wn::containers::string_view v(string);

  EXPECT_FALSE(v.empty());

  size_t pos = v.find_first_not_of(' ');

  EXPECT_EQ(pos, 6);

  pos = v.find_first_not_of(" s");

  EXPECT_EQ(pos, 7);

  pos = v.find_first_not_of(" strin");

  EXPECT_EQ(pos, 11);

  pos = v.find_first_not_of(" string");

  EXPECT_EQ(pos, wn::containers::string_view::npos);
}

TEST(string_view, find_last_not_of) {
  const char* string = "      strings";
  const wn::containers::string_view v(string);

  EXPECT_FALSE(v.empty());

  size_t pos = v.find_last_not_of(' ');

  EXPECT_EQ(pos, 12);

  pos = v.find_last_not_of(" s");

  EXPECT_EQ(pos, 11);

  pos = v.find_last_not_of(" strin");

  EXPECT_EQ(pos, 11);

  pos = v.find_last_not_of(" string");

  EXPECT_EQ(pos, wn::containers::string_view::npos);
}
