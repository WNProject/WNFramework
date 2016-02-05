// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNTesting/inc/WNTestHarness.h"
#include "WNMemory/inc/WNStringUtility.h"

TEST(string_utility, strlen) {
  const char* test1 = "string";
  const char* test2 = "string_longer";
  const char* test3 = "string_longest";

  EXPECT_EQ(wn::memory::strlen(test1), 6);
  EXPECT_EQ(wn::memory::strlen(test2), 13);
  EXPECT_EQ(wn::memory::strlen(test3), 14);

#ifdef _WN_DEBUG
  WN_EXPECT_DEBUG_DEATH_IF_SUPPORTED(
      wn::memory::strlen(nullptr),
      "assertion failed!\n\nfile: .*\nline: .*\nmessage: string must not be "
      "nullptr");
#endif
}

TEST(string_utility, strnlen) {
  const char* test1 = "string";
  const char* test2 = "string_longer";
  const char* test3 = "string_longest";

  EXPECT_EQ(wn::memory::strnlen(test1, 12), 6);
  EXPECT_EQ(wn::memory::strnlen(test2, 13), 13);
  EXPECT_EQ(wn::memory::strnlen(test3, 6), 6);
  EXPECT_EQ(wn::memory::strnlen(test3, 0), 0);

#ifdef _WN_DEBUG
  WN_EXPECT_DEBUG_DEATH_IF_SUPPORTED(
      wn::memory::strnlen(nullptr, 5),
      "assertion failed!\n\nfile: .*\nline: .*\nmessage: string must not be "
      "nullptr");
#endif
}

TEST(string_utility, strcpy) {
  const char* test1 = "string";
  char test2[7] = {0};

  wn::memory::strcpy(test2, test1);

  EXPECT_EQ(test2[0], 's');
  EXPECT_EQ(test2[1], 't');
  EXPECT_EQ(test2[2], 'r');
  EXPECT_EQ(test2[3], 'i');
  EXPECT_EQ(test2[4], 'n');
  EXPECT_EQ(test2[5], 'g');

#ifdef _WN_DEBUG
  WN_EXPECT_DEBUG_DEATH_IF_SUPPORTED(
      wn::memory::strcpy(test2, nullptr),
      "assertion failed!\n\nfile: .*\nline: .*\nmessage: source string must "
      "not be nullptr");

  WN_EXPECT_DEBUG_DEATH_IF_SUPPORTED(
      wn::memory::strcpy(nullptr, test1),
      "assertion failed!\n\nfile: .*\nline: .*\nmessage: destination string "
      "must not be nullptr");

  WN_EXPECT_DEBUG_DEATH_IF_SUPPORTED(
      wn::memory::strcpy(test2, test2),
      "assertion failed!\n\nfile: .*\nline: .*\nmessage: destination and "
      "source must not be the same");
#endif
}

TEST(string_utility, strncpy) {
  const char* test1 = "string";
  const char* test2 = "temp";
  char test3[3] = {0};

  wn::memory::strncpy(test3, test1, 3);

  EXPECT_EQ(test3[0], 's');
  EXPECT_EQ(test3[1], 't');
  EXPECT_EQ(test3[2], 'r');

  wn::memory::strncpy(test3, test2, 0);

  EXPECT_EQ(test3[0], 's');
  EXPECT_EQ(test3[1], 't');
  EXPECT_EQ(test3[2], 'r');

#ifdef _WN_DEBUG
  WN_EXPECT_DEBUG_DEATH_IF_SUPPORTED(
      wn::memory::strncpy(test3, nullptr, 3),
      "assertion failed!\n\nfile: .*\nline: .*\nmessage: source string must "
      "not be nullptr");

  WN_EXPECT_DEBUG_DEATH_IF_SUPPORTED(
      wn::memory::strncpy(nullptr, test1, 3),
      "assertion failed!\n\nfile: .*\nline: .*\nmessage: destination string "
      "must not be nullptr");

  WN_EXPECT_DEBUG_DEATH_IF_SUPPORTED(
      wn::memory::strncpy(test3, test3, 3),
      "assertion failed!\n\nfile: .*\nline: .*\nmessage: destination and "
      "source must not be the same");
#endif
}

TEST(string_utility, strcat) {
  const char* test1 = "ing";
  char test2[7] = "str";

  wn::memory::strcat(test2, test1);

  EXPECT_EQ(test2[0], 's');
  EXPECT_EQ(test2[1], 't');
  EXPECT_EQ(test2[2], 'r');
  EXPECT_EQ(test2[3], 'i');
  EXPECT_EQ(test2[4], 'n');
  EXPECT_EQ(test2[5], 'g');

#ifdef _WN_DEBUG
  WN_EXPECT_DEBUG_DEATH_IF_SUPPORTED(
      wn::memory::strcat(test2, nullptr),
      "assertion failed!\n\nfile: .*\nline: .*\nmessage: source string must "
      "not be nullptr");

  WN_EXPECT_DEBUG_DEATH_IF_SUPPORTED(
      wn::memory::strcat(nullptr, test1),
      "assertion failed!\n\nfile: .*\nline: .*\nmessage: destination string "
      "must not be nullptr");

  WN_EXPECT_DEBUG_DEATH_IF_SUPPORTED(
      wn::memory::strcat(test2, test2),
      "assertion failed!\n\nfile: .*\nline: .*\nmessage: destination and "
      "source must not be the same");
#endif
}

TEST(string_utility, strncat) {
  const char* test1 = "inging";
  char test2[7] = "str";

  wn::memory::strncat(test2, test1, 3);

  EXPECT_EQ(test2[0], 's');
  EXPECT_EQ(test2[1], 't');
  EXPECT_EQ(test2[2], 'r');
  EXPECT_EQ(test2[3], 'i');
  EXPECT_EQ(test2[4], 'n');
  EXPECT_EQ(test2[5], 'g');

  wn::memory::strncat(test2, test1, 0);

  EXPECT_EQ(test2[0], 's');
  EXPECT_EQ(test2[1], 't');
  EXPECT_EQ(test2[2], 'r');
  EXPECT_EQ(test2[3], 'i');
  EXPECT_EQ(test2[4], 'n');
  EXPECT_EQ(test2[5], 'g');
  EXPECT_EQ(test2[6], '\0');
}

TEST(string_utility, strcmp) {
  const char* test1 = "string";
  const char* test2 = "string";
  const char* test3 = "string1";
  const char* test4 = "strin";

  EXPECT_EQ(wn::memory::strcmp(test1, test1), 0);
  EXPECT_EQ(wn::memory::strcmp(test1, test2), 0);
  EXPECT_LT(wn::memory::strcmp(test1, test3), 0);
  EXPECT_GT(wn::memory::strcmp(test1, test4), 0);

#ifdef _WN_DEBUG
  WN_EXPECT_DEBUG_DEATH_IF_SUPPORTED(
      wn::memory::strcmp(test2, nullptr),
      "assertion failed!\n\nfile: .*\nline: .*\nmessage: string must not be "
      "nullptr");

  WN_EXPECT_DEBUG_DEATH_IF_SUPPORTED(
      wn::memory::strcmp(nullptr, test1),
      "assertion failed!\n\nfile: .*\nline: .*\nmessage: string must not be "
      "nullptr");
#endif
}

TEST(string_utility, strncmp) {
  const char* test1 = "string";
  const char* test2 = "string";
  const char* test3 = "string1";
  const char* test4 = "strin";

  EXPECT_EQ(wn::memory::strncmp(test1, test1, 6), 0);
  EXPECT_EQ(wn::memory::strncmp(test1, test2, 6), 0);
  EXPECT_EQ(wn::memory::strncmp(test1, test3, 6), 0);
  EXPECT_EQ(wn::memory::strncmp(test1, test4, 5), 0);
  EXPECT_GT(wn::memory::strncmp(test1, test4, 6), 0);
  EXPECT_LT(wn::memory::strncmp(test1, test3, 7), 0);
  EXPECT_EQ(wn::memory::strncmp(test1, test3, 0), 0);
}

TEST(string_utility, strdup) {
  const char* test1 = "string";
  char* test2 = wn::memory::strdup(test1);

  for (auto i = 0; i < 7; ++i) {
    EXPECT_EQ(test1[i], test2[i]);
  }

  wn::memory::heap_free(test2);

#ifdef _WN_DEBUG
  WN_EXPECT_DEBUG_DEATH_IF_SUPPORTED(
      wn::memory::strdup(nullptr),
      "assertion failed!\n\nfile: .*\nline: .*\nmessage: string must not be "
      "nullptr");
#endif
}

TEST(string_utility, strndup) {
  const char* test1 = "string";
  char* test2 = wn::memory::strndup(test1, 3);

  for (auto i = 0; i < 3; ++i) {
    EXPECT_EQ(test1[i], test2[i]);
  }

  EXPECT_EQ(test2[3], '\0');
  EXPECT_EQ(wn::memory::strlen(test2), 3);

  wn::memory::heap_free(test2);

#ifdef _WN_DEBUG
  WN_EXPECT_DEBUG_DEATH_IF_SUPPORTED(
      wn::memory::strndup(nullptr, 5),
      "assertion failed!\n\nfile: .*\nline: .*\nmessage: string must not be "
      "nullptr");
#endif
}

TEST(string_utility, strchr) {
  const char* test = "string";

  EXPECT_EQ(wn::memory::strchr(test, 's'), test);
  EXPECT_EQ(wn::memory::strchr(test, 'r'), test + 2);
  EXPECT_EQ(wn::memory::strchr(test, 'n'), test + 4);
  EXPECT_EQ(wn::memory::strchr(test, 'g'), test + 5);

#ifdef _WN_DEBUG
  const char* temp = nullptr;
  WN_EXPECT_DEBUG_DEATH_IF_SUPPORTED(
      wn::memory::strchr(temp, '\0'),
      "assertion failed!\n\nfile: .*\nline: .*\nmessage: string must not be "
      "nullptr");
  char* temp2 = nullptr;

  WN_EXPECT_DEBUG_DEATH_IF_SUPPORTED(
      wn::memory::strchr(temp2, '\0'),
      "assertion failed!\n\nfile: .*\nline: .*\nmessage: string must not be "
      "nullptr");
#endif
}

TEST(string_utility, strnchr) {
  const char* test = "string";

  EXPECT_EQ(wn::memory::strnchr(test, 's', 6), test);
  EXPECT_EQ(wn::memory::strnchr(test, 'r', 6), test + 2);
  EXPECT_EQ(wn::memory::strnchr(test, 'n', 6), test + 4);
  EXPECT_EQ(wn::memory::strnchr(test, 'g', 6), test + 5);
  EXPECT_EQ(wn::memory::strnchr(test, 's', 1), test);
  EXPECT_EQ(wn::memory::strnchr(test, 'r', 4), test + 2);
  EXPECT_EQ(wn::memory::strnchr(test, 'n', 6), test + 4);
  EXPECT_EQ(wn::memory::strnchr(test, 'g', 7), test + 5);
  EXPECT_EQ(wn::memory::strnchr(test, 'r', 1), nullptr);
  EXPECT_EQ(wn::memory::strnchr(test, 'n', 1), nullptr);
  EXPECT_EQ(wn::memory::strnchr(test, 'g', 1), nullptr);

#ifdef _WN_DEBUG

  const char* temp = nullptr;
  WN_EXPECT_DEBUG_DEATH_IF_SUPPORTED(
      wn::memory::strnchr(temp, '\0', 6),
      "assertion failed!\n\nfile: .*\nline: .*\nmessage: string must not be "
      "nullptr");
  char* temp2 = nullptr;

  WN_EXPECT_DEBUG_DEATH_IF_SUPPORTED(
      wn::memory::strnchr(temp2, '\0', 6),
      "assertion failed!\n\nfile: .*\nline: .*\nmessage: string must not be "
      "nullptr");
#endif
}

TEST(string_utility, strnchr_past_end) {
  const char* test = "string";

  // strnchr should end at "size" or the end of the string.
  EXPECT_EQ(wn::memory::strnchr(test, 'f', 32), nullptr);
  EXPECT_EQ(wn::memory::strnchr(test, 'p', 0xFFFFFFFF), nullptr);
}

TEST(string_utility, strlwr) {
  char test[7] = "STrInG";

  EXPECT_EQ(wn::memory::strlwr(test), test);
  EXPECT_EQ(test[0], 's');
  EXPECT_EQ(test[1], 't');
  EXPECT_EQ(test[2], 'r');
  EXPECT_EQ(test[3], 'i');
  EXPECT_EQ(test[4], 'n');
  EXPECT_EQ(test[5], 'g');
  EXPECT_EQ(test[6], '\0');

#ifdef _WN_DEBUG
  WN_EXPECT_DEBUG_DEATH_IF_SUPPORTED(
      wn::memory::strlwr(nullptr),
      "assertion failed!\n\nfile: .*\nline: .*\nmessage: string must not be "
      "nullptr");
#endif
}

TEST(string_utility, strnlwr) {
  char test1[7] = "STrInG";
  char test2[7] = "STrInG";

  EXPECT_EQ(wn::memory::strnlwr(test1, 3), test1);
  EXPECT_EQ(test1[0], 's');
  EXPECT_EQ(test1[1], 't');
  EXPECT_EQ(test1[2], 'r');
  EXPECT_EQ(test1[3], 'I');
  EXPECT_EQ(test1[4], 'n');
  EXPECT_EQ(test1[5], 'G');
  EXPECT_EQ(test1[6], '\0');

  EXPECT_EQ(wn::memory::strnlwr(test2, 0), test2);
  EXPECT_EQ(test2[0], 'S');
  EXPECT_EQ(test2[1], 'T');
  EXPECT_EQ(test2[2], 'r');
  EXPECT_EQ(test2[3], 'I');
  EXPECT_EQ(test2[4], 'n');
  EXPECT_EQ(test2[5], 'G');
  EXPECT_EQ(test2[6], '\0');

#ifdef _WN_DEBUG
  WN_EXPECT_DEBUG_DEATH_IF_SUPPORTED(
      wn::memory::strnlwr(nullptr, 5),
      "assertion failed!\n\nfile: .*\nline: .*\nmessage: string must not be "
      "nullptr");
#endif
}

TEST(string_utility, strupr) {
  char test[7] = "stRInG";

  EXPECT_EQ(wn::memory::strupr(test), test);
  EXPECT_EQ(test[0], 'S');
  EXPECT_EQ(test[1], 'T');
  EXPECT_EQ(test[2], 'R');
  EXPECT_EQ(test[3], 'I');
  EXPECT_EQ(test[4], 'N');
  EXPECT_EQ(test[5], 'G');
  EXPECT_EQ(test[6], '\0');

#ifdef _WN_DEBUG
  WN_EXPECT_DEBUG_DEATH_IF_SUPPORTED(
      wn::memory::strupr(nullptr),
      "assertion failed!\n\nfile: .*\nline: .*\nmessage: string must not be "
      "nullptr");
#endif
}

TEST(string_utility, strnupr) {
  char test1[7] = "sTrinG";
  char test2[7] = "sTrinG";

  EXPECT_EQ(wn::memory::strnupr(test1, 3), test1);
  EXPECT_EQ(test1[0], 'S');
  EXPECT_EQ(test1[1], 'T');
  EXPECT_EQ(test1[2], 'R');
  EXPECT_EQ(test1[3], 'i');
  EXPECT_EQ(test1[4], 'n');
  EXPECT_EQ(test1[5], 'G');
  EXPECT_EQ(test1[6], '\0');

  EXPECT_EQ(wn::memory::strnupr(test2, 0), test2);
  EXPECT_EQ(test2[0], 's');
  EXPECT_EQ(test2[1], 'T');
  EXPECT_EQ(test2[2], 'r');
  EXPECT_EQ(test2[3], 'i');
  EXPECT_EQ(test2[4], 'n');
  EXPECT_EQ(test2[5], 'G');
  EXPECT_EQ(test2[6], '\0');

#ifdef _WN_DEBUG
  WN_EXPECT_DEBUG_DEATH_IF_SUPPORTED(
      wn::memory::strnupr(nullptr, 5),
      "assertion failed!\n\nfile: .*\nline: .*\nmessage: string must not be "
      "nullptr");
#endif
}

TEST(string_utility, strhash) {
  char test1[7] = "sTrinG";
  char test2[7] = "string";

  const size_t result1 = wn::memory::strhash(test1);
  const size_t result2 = wn::memory::strhash(test2);

  EXPECT_NE(result1, result2);
}

TEST(string_utility, strnhash) {
  char test[] = "REPEAT LONG STRING THAT HAS A REPEAT";

  EXPECT_EQ(wn::memory::strnhash(test, 36), wn::memory::strnhash(test, 36));
  EXPECT_EQ(wn::memory::strnhash(test, 6), wn::memory::strnhash(test + 30, 6));
  EXPECT_NE(wn::memory::strnhash(test, 6), wn::memory::strnhash(test, 7));
}

TEST(string_utility, strnhash_overflow) {
  char test[] = "some string";
  char test2[] = "";  // Empty string

  EXPECT_EQ(wn::memory::strnhash(test, 11), wn::memory::strnhash(test, 12));
  EXPECT_EQ(wn::memory::strnhash(test, 400), wn::memory::strnhash(test, 500));
  EXPECT_EQ(wn::memory::strnhash(test2, 0), wn::memory::strnhash(test2, 12));
}
