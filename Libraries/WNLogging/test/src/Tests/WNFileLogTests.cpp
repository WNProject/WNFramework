// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include <fcntl.h>
#include "WNExecutableTest/inc/WNTestHarness.h"
#include "WNLogging/inc/WNConsoleLogger.h"
#include "WNLogging/inc/WNFileLogger.h"
#include "WNLogging/inc/WNLog.h"

#ifdef _WN_ANDROID
#define FILE_PREFIX "/sdcard/"
#else
#define FILE_PREFIX
#endif

const char* sFileName =
    FILE_PREFIX "test" _WN_PLATFORM_NAME _WN_ARCH_NAME _WN_CONFIG_NAME ".txt";

size_t NumWritten() {
  FILE* fp = fopen(sFileName, "r");
  if (!fp) {
    return (false);
  }
  fseek(fp, 0L, SEEK_END);
  int sz = ftell(fp);
  fclose(fp);
#ifdef _WN_WINDOWS
  if (::DeleteFile(sFileName) != TRUE) {
    return (false);
  }
#else
  unlink(sFileName);
#endif
  return (sz);
}

const char* GetTempFile() {
  return (sFileName);
}

using namespace wn::logging;
TEST(WNFileLogTest, Basiclogging) {
  {
    file_logger<GetTempFile> file_logger;
    static_log<log_level::max> default_log =
        static_log<log_level::max>(&file_logger);
    default_log.log()->log_error("asdf");
  }
  ASSERT_GE(NumWritten(), 1u);
}

TEST(WNFileLogTest, NoExessiveLogging) {
  {
    file_logger<GetTempFile> file_logger;
    static_log<log_level::max> default_log(&file_logger);
    default_log.log()->log_error("asdf");
    ASSERT_EQ(NumWritten(), 0);
  }
}

TEST(WNFileLogTest, FilledLog) {
  {
    file_logger<GetTempFile> file_logger;
    static_log<log_level::max> default_log(&file_logger);
    for (int i = 0; i < 100;
         ++i) {  // this should be at least asdf 100 times, but in reality
      // Error: asdf
      default_log.log()->log_error("asdf");
    }
  }
  ASSERT_GE(NumWritten(), 400u);
}

TEST(WNFileLogTest, ChangeLogLevel) {
  {
    file_logger<GetTempFile> file_logger;
    static_log<log_level::max> default_log(&file_logger);
    default_log.log()->log_info("asdf");
    default_log.log()->flush();
  }
  ASSERT_EQ(NumWritten(), 0);
  {
    file_logger<GetTempFile> file_logger;
    static_log<log_level::max> default_log(&file_logger);
    default_log.log()->log_info("asdf");
    default_log.log()->flush();
    default_log.log()->set_log_level(log_level::info);
    default_log.log()->log_info("asdf");
    default_log.log()->flush();
  }
  ASSERT_GE(NumWritten(), 1u);
}
