// Copyright (c) 2021, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNContainers/inc/WNString.h"
#include "executable_test/inc/WNTestHarness.h"

static const char test_string1[] =
    "this is a very long string that will not get optimized away";
static const char test_string2[] =
    "another very long string that should not get optimized out";
static const size_t test_string1_length = sizeof(test_string1) - 1;

TEST(string, construction) {
  wn::testing::allocator allocator;

  {
    wn::containers::string s1(nullptr);
    wn::containers::string s2(&allocator, test_string1);
    wn::containers::string s3(s2);
    wn::containers::string s4(&allocator, s2);

    EXPECT_TRUE(s1.empty());
    EXPECT_FALSE(s2.empty());
    EXPECT_FALSE(s3.empty());
    EXPECT_EQ(s2.size(), s3.size());
    EXPECT_FALSE(s4.empty());
    EXPECT_EQ(s3.size(), s4.size());
  }
}

TEST(string, element_access) {
  wn::testing::allocator allocator;

  {
    const wn::containers::string string1(&allocator, test_string1);

    ASSERT_FALSE(string1.empty());
    EXPECT_EQ(string1.front(), 't');
    EXPECT_EQ(string1.back(), 'y');
    EXPECT_EQ(string1.at(2), 'i');
    EXPECT_EQ(string1[2], 'i');

    const wn::containers::string string2(&allocator, test_string1 + 1, 3);

    ASSERT_FALSE(string2.empty());
    EXPECT_EQ(string2.front(), 'h');
    EXPECT_EQ(string2.back(), 's');
    EXPECT_EQ(string2.at(1), 'i');
    EXPECT_EQ(string2[1], 'i');

    wn::containers::string string3(&allocator, test_string1);

    EXPECT_EQ(string3.front(), 't');
    EXPECT_EQ(string3.back(), 'y');
    EXPECT_EQ(string3.at(2), 'i');
    EXPECT_EQ(string3[2], 'i');

    wn::containers::string string4(&allocator, test_string1 + 1, 3);

    EXPECT_EQ(string4.front(), 'h');
    EXPECT_EQ(string4.back(), 's');
    EXPECT_EQ(string4.at(1), 'i');
    EXPECT_EQ(string4[1], 'i');
  }
}

TEST(string, iteration) {
  wn::testing::allocator allocator;

  {
    wn::containers::string s1(&allocator, test_string1);

    size_t count = 0;

    for (auto i = s1.begin(); i != s1.end(); ++i) {
      EXPECT_EQ(*i, *(test_string1 + count));

      count++;
    }

    EXPECT_EQ(count, test_string1_length);

    count = 0;

    for (wn::containers::string::const_iterator i = s1.begin(); i != s1.end();
         ++i) {
      EXPECT_EQ(*i, *(test_string1 + count));

      count++;
    }

    EXPECT_EQ(count, test_string1_length);

    count = 0;

    for (auto i = s1.cbegin(); i != s1.cend(); ++i) {
      EXPECT_EQ(*i, *(test_string1 + count));

      count++;
    }

    EXPECT_EQ(count, test_string1_length);

    count = 0;

    const wn::containers::string s2(s1);

    for (auto i = s2.begin(); i != s2.end(); ++i) {
      EXPECT_EQ(*i, *(test_string1 + count));

      count++;
    }

    EXPECT_EQ(count, test_string1_length);

    count = test_string1_length;

    for (auto i = s1.rbegin(); i != s1.rend(); ++i) {
      EXPECT_EQ(*i, *(test_string1 + count - 1));

      count--;
    }

    EXPECT_EQ(count, 0u);

    count = test_string1_length;

    for (wn::containers::string::const_reverse_iterator i = s1.rbegin();
         i != s1.rend(); ++i) {
      EXPECT_EQ(*i, *(test_string1 + count - 1));

      count--;
    }

    EXPECT_EQ(count, 0u);

    count = test_string1_length;

    for (auto i = s1.crbegin(); i != s1.crend(); ++i) {
      EXPECT_EQ(*i, *(test_string1 + count - 1));

      count--;
    }

    EXPECT_EQ(count, 0u);

    count = test_string1_length;

    for (auto i = s2.rbegin(); i != s2.rend(); ++i) {
      EXPECT_EQ(*i, *(test_string1 + count - 1));

      count--;
    }

    EXPECT_EQ(count, 0u);
  }
}

// capacity

TEST(string, empty) {
  wn::testing::allocator allocator;

  {
    const wn::containers::string s1(&allocator);

    EXPECT_TRUE(s1.empty());

    wn::containers::string s2(&allocator, test_string1);

    EXPECT_FALSE(s2.empty());

    s2.clear();

    EXPECT_TRUE(s2.empty());
  }
}

TEST(string, size) {
  wn::testing::allocator allocator;

  {
    const wn::containers::string s1(&allocator, test_string1);

    EXPECT_EQ(s1.size(), test_string1_length);
    EXPECT_EQ(s1.length(), test_string1_length);

    const wn::containers::string s2(&allocator, test_string1 + 1, 3);

    EXPECT_EQ(s2.size(), 3u);
    EXPECT_EQ(s2.length(), 3u);
  }
}

TEST(string, max_size) {
  wn::testing::allocator allocator;

  {
    const wn::containers::string s1(&allocator, test_string1);

    EXPECT_EQ(s1.max_size(),
        std::numeric_limits<wn::containers::string::size_type>::max() - 1);

    const wn::containers::string s2(&allocator, test_string1 + 1, 3);

    EXPECT_EQ(s2.max_size(),
        std::numeric_limits<wn::containers::string::size_type>::max() - 1);
  }
}

TEST(string, shrink_to_fit) {
  wn::testing::allocator allocator;

  {
    wn::containers::string s(&allocator);

    s.reserve(100);

    EXPECT_EQ(s.size(), 0u);
    EXPECT_EQ(s.capacity(), 100u);

    s += test_string1;

    EXPECT_EQ(s.size(), test_string1_length);
    EXPECT_EQ(s.capacity(), 100u);

    s.shrink_to_fit();

    EXPECT_EQ(s.size(), test_string1_length);
    EXPECT_EQ(s.size(), s.capacity());
  }
}

// operations

TEST(string, clear) {
  wn::testing::allocator allocator;

  {
    wn::containers::string s(&allocator, test_string1);

    EXPECT_EQ(s.size(), test_string1_length);
    EXPECT_GE(s.capacity(), test_string1_length);
    EXPECT_EQ(s, test_string1);

    s.clear();

    EXPECT_TRUE(s.empty());
    EXPECT_EQ(s.size(), 0u);
    EXPECT_GE(s.capacity(), 0u);
  }
}

TEST(string, starts_with) {
  wn::testing::allocator allocator;

  {
    wn::containers::string s1(&allocator, test_string1);
    const char* test_term1 = "this";

    EXPECT_TRUE(s1.starts_with(test_term1));

    wn::containers::string s2(&allocator, test_term1);

    EXPECT_TRUE(s1.starts_with(s2));
    EXPECT_TRUE(s1.starts_with('t'));
    EXPECT_TRUE(s2.starts_with(test_term1));

    const char* test_term2 = "is";

    EXPECT_FALSE(s1.starts_with(test_term2));

    wn::containers::string s3(&allocator, test_term2);

    EXPECT_FALSE(s1.starts_with(s3));
    EXPECT_FALSE(s1.starts_with('i'));
  }
}

TEST(string, ends_with) {
  wn::testing::allocator allocator;

  {
    wn::containers::string s1(&allocator, test_string1);
    const char* test_term1 = "away";

    EXPECT_TRUE(s1.ends_with(test_term1));

    wn::containers::string s2(&allocator, test_term1);

    EXPECT_TRUE(s1.ends_with(s2));
    EXPECT_TRUE(s1.ends_with('y'));
    EXPECT_TRUE(s2.ends_with(test_term1));

    const char* test_term2 = "optimized";

    EXPECT_FALSE(s1.ends_with(test_term2));

    wn::containers::string s3(&allocator, test_term2);

    EXPECT_FALSE(s1.ends_with(s3));
    EXPECT_FALSE(s1.ends_with('a'));
  }
}

TEST(string, erase) {
  wn::testing::allocator allocator;

  {
    wn::containers::string s(&allocator, test_string1);

    EXPECT_EQ(s.size(), 59u);
    EXPECT_GE(s.capacity(), 59u);

    s.erase(0, 3);

    EXPECT_EQ(s.size(), test_string1_length - 3);
    EXPECT_EQ(s.front(), 's');

    s.erase(42, 10);

    EXPECT_EQ(s.size(), test_string1_length - 13);
    EXPECT_EQ(s, "s is a very long string that will not get away");

    s = test_string1;

    EXPECT_EQ(s.size(), 59u);
    EXPECT_GE(s.capacity(), 59u);

    auto itr = s.cbegin() + 4;
    auto itr2 = s.erase(itr);

    EXPECT_EQ(*itr2, 'i');
    EXPECT_EQ(s.size(), test_string1_length - 1);
    EXPECT_EQ(s, "thisis a very long string that will not get optimized away");

    s = test_string1;

    EXPECT_EQ(s.size(), 59u);
    EXPECT_GE(s.capacity(), 59u);

    auto itr3 = s.cbegin() + 37;
    auto itr4 = s.cbegin() + 41;
    auto itr5 = s.erase(itr3, itr4);

    EXPECT_EQ(*itr5, 'g');
    EXPECT_EQ(s.size(), test_string1_length - 4);
    EXPECT_EQ(s, "this is a very long string that will get optimized away");

    s.erase(0, s.size());

    EXPECT_EQ(s.size(), 0u);
    EXPECT_GE(s.capacity(), 0u);
  }
}

TEST(string, push_pack) {
  wn::testing::allocator allocator;

  {
    wn::containers::string s(&allocator, test_string1);

    EXPECT_EQ(s.size(), 59u);
    EXPECT_GE(s.capacity(), 59u);
    EXPECT_EQ(s.back(), *(test_string1 + (test_string1_length - 1)));

    s.push_back('T');

    EXPECT_EQ(s.size(), 60u);
    EXPECT_GE(s.capacity(), 60u);
    EXPECT_EQ(s.back(), 'T');
  }
}

TEST(string, pop_pack) {
  wn::testing::allocator allocator;

  {
    wn::containers::string s(&allocator, test_string1);

    EXPECT_EQ(s.size(), 59u);
    EXPECT_GE(s.capacity(), 59u);
    EXPECT_EQ(s.back(), *(test_string1 + (test_string1_length - 1)));

    s.pop_back();

    EXPECT_EQ(s.size(), 58u);
    EXPECT_GE(s.capacity(), 58u);
    EXPECT_EQ(s.back(), *(test_string1 + (test_string1_length - 2)));
  }
}

TEST(string, append) {
  wn::testing::allocator allocator;

  {
    wn::containers::string s(&allocator, test_string1);

    EXPECT_EQ(s.size(), 59u);
    EXPECT_GE(s.capacity(), 59u);
    EXPECT_EQ(s.back(), *(test_string1 + (test_string1_length - 1)));

    s.append(" ever");

    EXPECT_EQ(s.size(), 64u);
    EXPECT_GE(s.capacity(), 64u);
    EXPECT_EQ(s.back(), 'r');
  }
}

TEST(string, resize) {
  wn::testing::allocator allocator;

  {
    wn::containers::string s(&allocator);

    EXPECT_EQ(s.size(), 0u);
    EXPECT_TRUE(s.empty());

    s.resize(10);

    EXPECT_EQ(s.size(), 10u);
    EXPECT_FALSE(s.empty());
    EXPECT_GE(s.capacity(), 10u);

    s.resize(5);

    EXPECT_EQ(s.size(), 5u);
    EXPECT_FALSE(s.empty());
    EXPECT_GE(s.capacity(), 5u);

    s.clear();

    EXPECT_EQ(s.size(), 0u);
    EXPECT_TRUE(s.empty());
    EXPECT_GE(s.capacity(), 0u);

    s.resize(20, 'a');

    EXPECT_EQ(s.size(), 20u);
    EXPECT_FALSE(s.empty());
    EXPECT_GE(s.capacity(), 20u);

    for (char c : s) {
      EXPECT_EQ(c, 'a');
    }

    s = test_string1;

    EXPECT_EQ(s, test_string1);

    s.resize(4);

    EXPECT_EQ(s, "this");
  }
}

TEST(string, swap) {
  wn::testing::allocator allocator;

  {
    wn::containers::string s1(&allocator, test_string1);
    wn::containers::string s2(&allocator, test_string2);

    EXPECT_FALSE(s1.empty());
    EXPECT_FALSE(s2.empty());
    EXPECT_EQ(s1, test_string1);
    EXPECT_EQ(s2, test_string2);

    s1.swap(s2);

    EXPECT_FALSE(s1.empty());
    EXPECT_FALSE(s2.empty());
    EXPECT_EQ(s1, test_string2);
    EXPECT_EQ(s2, test_string1);
  }
}

// searching

TEST(string, rfind) {
  wn::testing::allocator allocator;

  {
    const wn::containers::string s(&allocator, test_string1);

    EXPECT_FALSE(s.empty());

    size_t pos = s.rfind("a");

    EXPECT_EQ(pos, 57u);

    pos = s.rfind('o');

    EXPECT_EQ(pos, 45u);

    pos = s.rfind("x");

    EXPECT_EQ(pos, static_cast<size_t>(wn::containers::string::npos));
  }
}

TEST(string, find_first_of) {
  wn::testing::allocator allocator;

  {
    const wn::containers::string s(&allocator, test_string1);

    EXPECT_FALSE(s.empty());

    size_t pos = s.find_first_of('s');

    EXPECT_EQ(pos, 3u);

    pos = s.find_first_of('s', pos + 1);

    EXPECT_EQ(pos, 6u);

    pos = s.find_first_of("tr");

    EXPECT_EQ(pos, 0u);

    pos = s.find_first_of("tr", pos + 1);

    EXPECT_EQ(pos, 12u);

    pos = s.find_first_of("g");

    EXPECT_EQ(pos, 18u);

    pos = s.find_first_of("x");

    EXPECT_EQ(pos, static_cast<size_t>(wn::containers::string::npos));
  }
}

TEST(string, find_last_of) {
  wn::testing::allocator allocator;

  {
    const char* string1 = "      strings all over";
    const wn::containers::string s(&allocator, string1);

    EXPECT_FALSE(s.empty());

    size_t pos = s.find_last_of('s');

    EXPECT_EQ(pos, 12u);

    pos = s.find_last_of('s', pos - 1);

    EXPECT_EQ(pos, 6u);

    pos = s.find_last_of("tr");

    EXPECT_EQ(pos, 21u);

    pos = s.find_last_of("g");

    EXPECT_EQ(pos, 11u);

    pos = s.find_last_of("xyz");

    EXPECT_EQ(pos, static_cast<size_t>(wn::containers::string::npos));
  }
}
