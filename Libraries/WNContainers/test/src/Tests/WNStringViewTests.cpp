// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNContainers/inc/WNStringView.h"
#include "WNTesting/inc/WNTestHarness.h"

TEST(string_view, construction) {
  const char* raw = "string1";
  wn::containers::string_view view1;
  const wn::containers::string_view view2(nullptr);
  const wn::containers::string_view view3(nullptr, nullptr);
  const wn::containers::string_view view4(raw, raw + 7);
  const wn::containers::string_view view5(raw, 7);
  const wn::containers::string_view view6(raw);
  wn::containers::string_view view7(view6);

  EXPECT_TRUE(view1.empty());
  EXPECT_EQ(view1.data(), nullptr);
  EXPECT_TRUE(view2.empty());
  EXPECT_EQ(view2.data(), nullptr);
  EXPECT_TRUE(view3.empty());
  EXPECT_EQ(view3.data(), nullptr);
  EXPECT_FALSE(view4.empty());
  EXPECT_EQ(view4.data(), raw);
  EXPECT_EQ(view4, "string1");
  EXPECT_FALSE(view5.empty());
  EXPECT_EQ(view5.data(), raw);
  EXPECT_EQ(view5, "string1");
  EXPECT_FALSE(view6.empty());
  EXPECT_EQ(view6.data(), raw);
  EXPECT_EQ(view6, "string1");
  EXPECT_FALSE(view7.empty());
  EXPECT_EQ(view7.data(), raw);
  EXPECT_EQ(view7, "string1");

  wn::containers::string_view view8(std::move(view7));

  EXPECT_TRUE(view7.empty());
  EXPECT_EQ(view7.data(), nullptr);
  EXPECT_FALSE(view8.empty());
  EXPECT_EQ(view8.data(), raw);
  EXPECT_EQ(view8, "string1");

  wn::containers::contiguous_range<const char> range(raw, 7);

  EXPECT_FALSE(range.empty());
  EXPECT_EQ(range.data(), raw);

  wn::containers::string_view view9(range);

  EXPECT_FALSE(view9.empty());
  EXPECT_EQ(view9.data(), raw);
  EXPECT_EQ(view9, "string1");

  wn::containers::string_view view10(std::move(range));

  EXPECT_TRUE(range.empty());
  EXPECT_EQ(range.data(), nullptr);
  EXPECT_FALSE(view10.empty());
  EXPECT_EQ(view10.data(), raw);
  EXPECT_EQ(view10, "string1");

  const wn::containers::string str("string2");
  const wn::containers::string_view view11(str);
  const wn::containers::string_view view12(str, 7);
  const wn::containers::string_view view13(str, 1, 5);

  EXPECT_FALSE(view11.empty());
  EXPECT_EQ(view11.data(), str.data());
  EXPECT_EQ(view11, "string2");
  EXPECT_FALSE(view12.empty());
  EXPECT_EQ(view12.data(), str.data());
  EXPECT_EQ(view12, "string2");
  EXPECT_FALSE(view13.empty());
  EXPECT_EQ(view13.data(), str.data() + 1);
  EXPECT_EQ(view13, "tring");

  const wn::containers::string_view view14(raw, 1, 5);

  EXPECT_FALSE(view14.empty());
  EXPECT_EQ(view14.data(), raw + 1);
  EXPECT_EQ(view14, "tring");
}

TEST(string_view, assignment) {
  const char* raw = "string";
  wn::containers::string_view view1(raw);

  view1 = nullptr;

  wn::containers::string_view view2;

  view2 = raw;

  wn::containers::string_view view3;

  view3 = view2;

  EXPECT_TRUE(view1.empty());
  EXPECT_EQ(view1.data(), nullptr);
  EXPECT_FALSE(view2.empty());
  EXPECT_EQ(view2.data(), raw);
  EXPECT_FALSE(view3.empty());
  EXPECT_EQ(view3.data(), raw);

  wn::containers::string_view view4;

  view4 = std::move(view3);

  EXPECT_TRUE(view3.empty());
  EXPECT_EQ(view3.data(), nullptr);
  EXPECT_FALSE(view4.empty());
  EXPECT_EQ(view4.data(), raw);
}

TEST(string_view, access) {
  const char* string = "string";
  wn::containers::string_view view1(string);

  ASSERT_FALSE(view1.empty());
  EXPECT_EQ(view1.front(), 's');
  EXPECT_EQ(view1.back(), 'g');
  EXPECT_EQ(view1.at(2), 'r');
  EXPECT_EQ(view1[2], 'r');
  EXPECT_EQ(view1.data(), string);

  wn::containers::string_view view2(string + 1, 3);

  ASSERT_FALSE(view2.empty());
  EXPECT_EQ(view2.front(), 't');
  EXPECT_EQ(view2.back(), 'i');
  EXPECT_EQ(view2.at(1), 'r');
  EXPECT_EQ(view2[1], 'r');
  EXPECT_EQ(view2.data(), string + 1);
}

TEST(string_view, capacity) {
  const char* string = "string";
  const wn::containers::string_view view1(string);

  EXPECT_EQ(view1.size(), 6);
  EXPECT_EQ(view1.length(), 6);
  EXPECT_EQ(view1.max_size(), 6);
  EXPECT_EQ(view1.max_size(), view1.size());

  const wn::containers::string_view view2(string + 1, 3);

  EXPECT_EQ(view2.size(), 3);
  EXPECT_EQ(view2.length(), 3);
  EXPECT_EQ(view2.max_size(), 3);
  EXPECT_EQ(view2.max_size(), view2.size());
}

TEST(string_view, swap) {
  const char* string1 = "1string2";
  const char* string2 = "3string4";
  wn::containers::string_view view1(string1);
  wn::containers::string_view view2(string2);

  EXPECT_FALSE(view1.empty());
  EXPECT_FALSE(view2.empty());
  ASSERT_EQ(view1.data(), string1);
  ASSERT_EQ(view2.data(), string2);
  EXPECT_EQ(view1.front(), '1');
  EXPECT_EQ(view2.front(), '3');
  EXPECT_EQ(view1.back(), '2');
  EXPECT_EQ(view2.back(), '4');

  view1.swap(view2);

  EXPECT_FALSE(view1.empty());
  EXPECT_FALSE(view2.empty());
  ASSERT_EQ(view1.data(), string2);
  ASSERT_EQ(view2.data(), string1);
  EXPECT_EQ(view1.front(), '3');
  EXPECT_EQ(view2.front(), '1');
  EXPECT_EQ(view1.back(), '4');
  EXPECT_EQ(view2.back(), '2');
}

TEST(string_view, substr) {
  const char* string = "string";
  const wn::containers::string_view view1(string);
  const wn::containers::string_view view2(view1.substr());

  ASSERT_EQ(view2.length(), 6);
  EXPECT_EQ(view2[0], 's');
  EXPECT_EQ(view2[1], 't');
  EXPECT_EQ(view2[2], 'r');
  EXPECT_EQ(view2[3], 'i');
  EXPECT_EQ(view2[4], 'n');
  EXPECT_EQ(view2[5], 'g');

  const wn::containers::string_view view3(view1.substr(4));

  ASSERT_EQ(view3.length(), 2);
  EXPECT_EQ(view3[0], 'n');
  EXPECT_EQ(view3[1], 'g');

  const wn::containers::string_view view4(view1.substr(1, 3));

  ASSERT_EQ(view4.length(), 3);
  EXPECT_EQ(view4[0], 't');
  EXPECT_EQ(view4[1], 'r');
  EXPECT_EQ(view4[2], 'i');
}

TEST(string_view, find) {
  const char* string = "      string";
  const wn::containers::string_view view(string);

  EXPECT_FALSE(view.empty());

  size_t pos = view.find("ing");

  EXPECT_EQ(pos, 9);

  pos = view.find('t');

  EXPECT_EQ(pos, 7);
}

TEST(string_view, rfind) {
  const char* string = "      string";
  const wn::containers::string_view view(string);

  EXPECT_FALSE(view.empty());

  size_t pos = view.rfind("ri");

  EXPECT_EQ(pos, 8);

  pos = view.find('n');

  EXPECT_EQ(pos, 10);
}

TEST(string_view, find_first_of) {
  const char* string = "      string";
  const wn::containers::string_view view(string);

  EXPECT_FALSE(view.empty());

  size_t pos = view.find_first_of('s');

  EXPECT_EQ(pos, 6);

  pos = view.find_first_of("tr");

  EXPECT_EQ(pos, 7);

  pos = view.find_first_of("g");

  EXPECT_EQ(pos, 11);

  pos = view.find_first_of("xyz");

  EXPECT_EQ(pos, wn::containers::string_view::npos);
}

TEST(string_view, find_last_of) {
  const char* string1 = "      strings";
  const wn::containers::string_view view1(string1);

  EXPECT_FALSE(view1.empty());

  size_t pos = view1.find_last_of('s');

  EXPECT_EQ(pos, 12);

  pos = view1.find_last_of("tr");

  EXPECT_EQ(pos, 8);

  pos = view1.find_last_of("g");

  EXPECT_EQ(pos, 11);

  pos = view1.find_last_of("xyz");

  EXPECT_EQ(pos, wn::containers::string_view::npos);
}

TEST(string_view, find_first_not_of) {
  const char* string = "      string";
  const wn::containers::string_view view(string);

  EXPECT_FALSE(view.empty());

  size_t pos = view.find_first_not_of(' ');

  EXPECT_EQ(pos, 6);

  pos = view.find_first_not_of(" s");

  EXPECT_EQ(pos, 7);

  pos = view.find_first_not_of(" strin");

  EXPECT_EQ(pos, 11);

  pos = view.find_first_not_of(" string");

  EXPECT_EQ(pos, wn::containers::string_view::npos);
}

TEST(string_view, find_last_not_of) {
  const char* string = "      strings";
  const wn::containers::string_view view(string);

  EXPECT_FALSE(view.empty());

  size_t pos = view.find_last_not_of(' ');

  EXPECT_EQ(pos, 12);

  pos = view.find_last_not_of(" s");

  EXPECT_EQ(pos, 11);

  pos = view.find_last_not_of(" strin");

  EXPECT_EQ(pos, 11);

  pos = view.find_last_not_of(" string");

  EXPECT_EQ(pos, wn::containers::string_view::npos);
}

TEST(string_view, remove_prefix) {
  const char* string = "string";
  wn::containers::string_view view(string);

  ASSERT_FALSE(view.empty());
  EXPECT_EQ(view.front(), 's');
  EXPECT_EQ(view.back(), 'g');

  view.remove_prefix(2);

  EXPECT_EQ(view.front(), 'r');
  EXPECT_EQ(view.back(), 'g');
}

TEST(string_view, remove_suffix) {
  const char* string = "string";
  wn::containers::string_view view(string);

  ASSERT_FALSE(view.empty());
  EXPECT_EQ(view.front(), 's');
  EXPECT_EQ(view.back(), 'g');

  view.remove_suffix(2);

  EXPECT_EQ(view.front(), 's');
  EXPECT_EQ(view.back(), 'i');
}

TEST(string_view, to_string) {
  const char* string = "string that is long enough to not get optimized out";
  wn::containers::string_view view(string);
  wn::testing::allocator allocator;

  {
    wn::containers::string str = view.to_string();

    str = view.to_string(&allocator);
  }
}
