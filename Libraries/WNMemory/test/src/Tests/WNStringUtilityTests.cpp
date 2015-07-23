// Copyright (c) 2014, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNMemory/test/inc/Common.h"
#include "WNMemory/inc/WNStringUtility.h"

TEST(string_utility, strlen) {
  const wn_char* test1 = "string";
  const wn_char* test2 = "string_longer";
  const wn_char* test3 = "string_longest";

  EXPECT_EQ(wn::memory::strlen(test1), 6);
  EXPECT_EQ(wn::memory::strlen(test2), 13);
  EXPECT_EQ(wn::memory::strlen(test3), 14);

  #ifdef _WN_DEBUG
    EXPECT_DEATH({
        wn::memory::strlen(wn_nullptr);
      },
      "assertion failed!\n\nfile: .*\nline: .*\nmessage: string must not be "
      "nullptr"
    );
  #endif
}

TEST(string_utility, strcpy) {
  const wn_char* test1 = "string";
  wn_char test2[7] = {0};

  wn::memory::strcpy(test2, test1);

  EXPECT_EQ(test2[0], 's');
  EXPECT_EQ(test2[1], 't');
  EXPECT_EQ(test2[2], 'r');
  EXPECT_EQ(test2[3], 'i');
  EXPECT_EQ(test2[4], 'n');
  EXPECT_EQ(test2[5], 'g');

  #ifdef _WN_DEBUG
    EXPECT_DEATH({
        wn::memory::strcpy(test2, wn_nullptr);
      },
      "assertion failed!\n\nfile: .*\nline: .*\nmessage: source string must "
      "not be nullptr"
    );

    EXPECT_DEATH({
        wn::memory::strcpy(wn_nullptr, test1);
      },
      "assertion failed!\n\nfile: .*\nline: .*\nmessage: destination string "
      "must not be nullptr"
    );

    EXPECT_DEATH({
        wn::memory::strcpy(test2, test2);
      },
      "assertion failed!\n\nfile: .*\nline: .*\nmessage: destination and "
      "source must not be the same"
    );
  #endif
}

TEST(string_utility, strncpy) {
  const wn_char* test1 = "string";
  wn_char test2[3] = { 0 };

  wn::memory::strncpy(test2, test1, 3);

  EXPECT_EQ(test2[0], 's');
  EXPECT_EQ(test2[1], 't');
  EXPECT_EQ(test2[2], 'r');

  #ifdef _WN_DEBUG
    EXPECT_DEATH({
        wn::memory::strncpy(test2, wn_nullptr, 3);
      },
      "assertion failed!\n\nfile: .*\nline: .*\nmessage: source string must "
      "not be nullptr"
    );

    EXPECT_DEATH({
        wn::memory::strncpy(wn_nullptr, test1, 3);
      },
      "assertion failed!\n\nfile: .*\nline: .*\nmessage: destination string "
      "must not be nullptr"
    );

    EXPECT_DEATH({
        wn::memory::strncpy(test2, test2, 3);
      },
      "assertion failed!\n\nfile: .*\nline: .*\nmessage: destination and "
      "source must not be the same"
    );

    EXPECT_DEATH({
        wn::memory::strncpy(test2, test1, 0);
      },
      "assertion failed!\n\nfile: .*\nline: .*\nmessage: count must not be 0"
    );
  #endif
}

TEST(string_utility, strcat) {
  const wn_char* test1 = "ing";
  wn_char test2[7] = "str";

  wn::memory::strcat(test2, test1);

  EXPECT_EQ(test2[0], 's');
  EXPECT_EQ(test2[1], 't');
  EXPECT_EQ(test2[2], 'r');
  EXPECT_EQ(test2[3], 'i');
  EXPECT_EQ(test2[4], 'n');
  EXPECT_EQ(test2[5], 'g');

  #ifdef _WN_DEBUG
    EXPECT_DEATH({
        wn::memory::strcat(test2, wn_nullptr);
      },
      "assertion failed!\n\nfile: .*\nline: .*\nmessage: source string must "
      "not be nullptr"
    );

    EXPECT_DEATH({
        wn::memory::strcat(wn_nullptr, test1);
      },
      "assertion failed!\n\nfile: .*\nline: .*\nmessage: destination string "
      "must not be nullptr"
    );

    EXPECT_DEATH({
        wn::memory::strcat(test2, test2);
      },
      "assertion failed!\n\nfile: .*\nline: .*\nmessage: destination and "
      "source must not be the same"
    );
  #endif
}

TEST(string_utility, strncat) {
  const wn_char* test1 = "inging";
  wn_char test2[7] = "str";

  wn::memory::strncat(test2, test1, 3);

  EXPECT_EQ(test2[0], 's');
  EXPECT_EQ(test2[1], 't');
  EXPECT_EQ(test2[2], 'r');
  EXPECT_EQ(test2[3], 'i');
  EXPECT_EQ(test2[4], 'n');
  EXPECT_EQ(test2[5], 'g');
}

TEST(string_utility, strcmp) {
  const wn_char* test1 = "string";
  const wn_char* test2 = "string";
  const wn_char* test3 = "string1";
  const wn_char* test4 = "strin";

  EXPECT_EQ(wn::memory::strcmp(test1, test1), 0);
  EXPECT_EQ(wn::memory::strcmp(test1, test2), 0);
  EXPECT_LT(wn::memory::strcmp(test1, test3), 0);
  EXPECT_GT(wn::memory::strcmp(test1, test4), 0);

  #ifdef _WN_DEBUG
    EXPECT_DEATH({
        wn::memory::strcmp(test2, wn_nullptr);
      },
      "assertion failed!\n\nfile: .*\nline: .*\nmessage: string must not be "
      "nullptr"
    );

    EXPECT_DEATH({
        wn::memory::strcmp(wn_nullptr, test1);
      },
      "assertion failed!\n\nfile: .*\nline: .*\nmessage: string must not be "
      "nullptr"
    );
  #endif
}

TEST(string_utility, strncmp) {
  const wn_char* test1 = "string";
  const wn_char* test2 = "string";
  const wn_char* test3 = "string1";
  const wn_char* test4 = "strin";

  EXPECT_EQ(wn::memory::strncmp(test1, test1, 6), 0);
  EXPECT_EQ(wn::memory::strncmp(test1, test2, 6), 0);
  EXPECT_EQ(wn::memory::strncmp(test1, test3, 6), 0);
  EXPECT_EQ(wn::memory::strncmp(test1, test4, 5), 0);
  EXPECT_GT(wn::memory::strncmp(test1, test4, 6), 0);
  EXPECT_LT(wn::memory::strncmp(test1, test3, 7), 0);
}

TEST(string_utility, strdup) {
  const wn_char* test1 = "string";
  wn_char* test2 = wn::memory::strdup(test1);

  for (auto i = 0; i < 7; ++i) {
    EXPECT_EQ(test1[i], test2[i]);
  }

  wn::memory::heap_free(test2);

  #ifdef _WN_DEBUG
    EXPECT_DEATH({
        wn::memory::strdup(wn_nullptr);
      },
      "assertion failed!\n\nfile: .*\nline: .*\nmessage: string must not be "
      "nullptr"
    );
  #endif
}

TEST(string_utility, strndup) {
  const wn_char* test1 = "string";
  wn_char* test2 = wn::memory::strndup(test1, 3);

  for (auto i = 0; i < 3; ++i) {
    EXPECT_EQ(test1[i], test2[i]);
  }

  EXPECT_EQ(test2[3], '\0');
  EXPECT_EQ(wn::memory::strlen(test2), 3);

  wn::memory::heap_free(test2);

  #ifdef _WN_DEBUG
    EXPECT_DEATH({
        wn::memory::strndup(wn_nullptr, 5);
      },
      "assertion failed!\n\nfile: .*\nline: .*\nmessage: string must not be "
      "nullptr"
    );

    EXPECT_DEATH({
        wn::memory::strndup(test1, 0);
      },
      "assertion failed!\n\nfile: .*\nline: .*\nmessage: count must not be 0"
    );
  #endif
}

TEST(string_utility, strchr) {
  const wn_char* test = "string";

  EXPECT_EQ(wn::memory::strchr(test, 's'), test);
  EXPECT_EQ(wn::memory::strchr(test, 'r'), test + 2);
  EXPECT_EQ(wn::memory::strchr(test, 'n'), test + 4);
  EXPECT_EQ(wn::memory::strchr(test, 'g'), test + 5);

  #ifdef _WN_DEBUG
    EXPECT_DEATH({
        const wn_char* temp = wn_nullptr;

        wn::memory::strchr(temp, '\0');
      },
      "assertion failed!\n\nfile: .*\nline: .*\nmessage: string must not be "
      "nullptr"
    );

    EXPECT_DEATH({
        wn_char* temp = wn_nullptr;

        wn::memory::strchr(temp, '\0');
      },
      "assertion failed!\n\nfile: .*\nline: .*\nmessage: string must not be "
      "nullptr"
    );
  #endif
}

TEST(string_utility, strnchr) {
  const wn_char* test = "string";

  EXPECT_EQ(wn::memory::strnchr(test, 's', 6), test);
  EXPECT_EQ(wn::memory::strnchr(test, 'r', 6), test + 2);
  EXPECT_EQ(wn::memory::strnchr(test, 'n', 6), test + 4);
  EXPECT_EQ(wn::memory::strnchr(test, 'g', 6), test + 5);
  EXPECT_EQ(wn::memory::strnchr(test, 's', 1), test);
  EXPECT_EQ(wn::memory::strnchr(test, 'r', 4), test + 2);
  EXPECT_EQ(wn::memory::strnchr(test, 'n', 6), test + 4);
  EXPECT_EQ(wn::memory::strnchr(test, 'g', 7), test + 5);
  EXPECT_EQ(wn::memory::strnchr(test, 'r', 1), wn_nullptr);
  EXPECT_EQ(wn::memory::strnchr(test, 'n', 1), wn_nullptr);
  EXPECT_EQ(wn::memory::strnchr(test, 'g', 1), wn_nullptr);

  #ifdef _WN_DEBUG
    EXPECT_DEATH({
        const wn_char* temp = wn_nullptr;

        wn::memory::strnchr(temp, '\0', 6);
      },
      "assertion failed!\n\nfile: .*\nline: .*\nmessage: string must not be "
      "nullptr"
    );

    EXPECT_DEATH({
        wn_char* temp = wn_nullptr;

        wn::memory::strnchr(temp, '\0', 6);
      },
      "assertion failed!\n\nfile: .*\nline: .*\nmessage: string must not be "
      "nullptr"
    );

    EXPECT_DEATH({
        wn::memory::strnchr(test, '\0', 0);
      },
      "assertion failed!\n\nfile: .*\nline: .*\nmessage: count must not be 0"
    );

    EXPECT_DEATH({
        wn_char temp[7] = "string";

        wn::memory::strnchr(temp, '\0', 0);
      },
      "assertion failed!\n\nfile: .*\nline: .*\nmessage: count must not be 0"
    );
  #endif
}

TEST(string_utility, strlwr) {
  wn_char test[7] = "STrInG";

  EXPECT_EQ(wn::memory::strlwr(test), test);
  EXPECT_EQ(test[0], 's');
  EXPECT_EQ(test[1], 't');
  EXPECT_EQ(test[2], 'r');
  EXPECT_EQ(test[3], 'i');
  EXPECT_EQ(test[4], 'n');
  EXPECT_EQ(test[5], 'g');
  EXPECT_EQ(test[6], '\0');

  #ifdef _WN_DEBUG
    EXPECT_DEATH({
        wn::memory::strlwr(wn_nullptr);
      },
      "assertion failed!\n\nfile: .*\nline: .*\nmessage: string must not be "
      "nullptr"
    );
  #endif
}

TEST(string_utility, strnlwr) {
  wn_char test[7] = "STrInG";

  EXPECT_EQ(wn::memory::strnlwr(test, 3), test);
  EXPECT_EQ(test[0], 's');
  EXPECT_EQ(test[1], 't');
  EXPECT_EQ(test[2], 'r');
  EXPECT_EQ(test[3], 'I');
  EXPECT_EQ(test[4], 'n');
  EXPECT_EQ(test[5], 'G');
  EXPECT_EQ(test[6], '\0');

  #ifdef _WN_DEBUG
    EXPECT_DEATH({
        wn::memory::strnlwr(wn_nullptr, 5);
      },
      "assertion failed!\n\nfile: .*\nline: .*\nmessage: string must not be "
      "nullptr"
    );

    EXPECT_DEATH({
        wn::memory::strnlwr(test, 0);
      },
      "assertion failed!\n\nfile: .*\nline: .*\nmessage: count must not be 0"
    );
  #endif
}

TEST(string_utility, strupr) {
  wn_char test[7] = "stRInG";

  EXPECT_EQ(wn::memory::strupr(test), test);
  EXPECT_EQ(test[0], 'S');
  EXPECT_EQ(test[1], 'T');
  EXPECT_EQ(test[2], 'R');
  EXPECT_EQ(test[3], 'I');
  EXPECT_EQ(test[4], 'N');
  EXPECT_EQ(test[5], 'G');
  EXPECT_EQ(test[6], '\0');

  #ifdef _WN_DEBUG
    EXPECT_DEATH({
        wn::memory::strupr(wn_nullptr);
      },
      "assertion failed!\n\nfile: .*\nline: .*\nmessage: string must not be "
      "nullptr"
    );
  #endif
}

TEST(string_utility, strnupr) {
  wn_char test[7] = "sTrinG";

  EXPECT_EQ(wn::memory::strnupr(test, 3), test);
  EXPECT_EQ(test[0], 'S');
  EXPECT_EQ(test[1], 'T');
  EXPECT_EQ(test[2], 'R');
  EXPECT_EQ(test[3], 'i');
  EXPECT_EQ(test[4], 'n');
  EXPECT_EQ(test[5], 'G');
  EXPECT_EQ(test[6], '\0');

  #ifdef _WN_DEBUG
    EXPECT_DEATH({
        wn::memory::strnupr(wn_nullptr, 5);
      },
      "assertion failed!\n\nfile: .*\nline: .*\nmessage: string must not be "
      "nullptr"
    );

    EXPECT_DEATH({
        wn::memory::strnupr(test, 0);
      },
      "assertion failed!\n\nfile: .*\nline: .*\nmessage: count must not be 0"
    );
  #endif
}