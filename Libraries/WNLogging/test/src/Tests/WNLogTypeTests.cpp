// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNLogging/inc/WNLog.h"
#include "WNExecutableTest/inc/WNTestHarness.h"

TEST(WNLogValueTest, LogValueInt) {
  for (int i = 0; i < 10; ++i) {
    size_t buff_size = 25;
    char temp_buff[25];
    int t2 = rand();
    wn::logging::log_type(t2, temp_buff, buff_size);
    int t = 0;
    sscanf(temp_buff, "%d", &t);
    ASSERT_EQ(t, t2);
  }
}

TEST(WNLogValueTest, LogValueString) {
  for (int i = 0; i < 10; ++i) {
    size_t buff_size = 25;
    char temp_buff2[25];
    char temp_buff[25];
    char* t_buff2 = temp_buff2;
    int t2 = rand();
    sprintf(temp_buff2, "%d", t2);
    wn::logging::log_type(t_buff2, temp_buff, buff_size);
    ASSERT_EQ(strcmp(temp_buff, temp_buff2), 0);
  }
}
