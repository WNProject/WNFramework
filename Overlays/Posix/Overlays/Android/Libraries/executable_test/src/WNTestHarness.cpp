// Copyright (c) 2020, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Some portions of this file based on googletest with the following license

// Copyright 2005, Google Inc.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "executable_test/inc/WNTestHarness.h"

#include "WNLogging/inc/WNConsoleLogger.h"
#include "WNLogging/inc/WNLog.h"

namespace wn {
namespace testing {

// Directly from googletest.
namespace {

// Formats a countable noun.  Depending on its quantity, either the
// singular form or the plural form is used. e.g.
//
// FormatCountableNoun(1, "formula", "formuli") returns "1 formula".
// FormatCountableNoun(5, "book", "books") returns "5 books".
static std::string FormatCountableNoun(
    int count, const char* singular_form, const char* plural_form) {
  return ::testing::internal::StreamableToString(count) + " " +
         (count == 1 ? singular_form : plural_form);
}

// Formats the count of tests.
static std::string FormatTestCount(int test_count) {
  return FormatCountableNoun(test_count, "test", "tests");
}

// Formats the count of test cases.
static std::string FormatTestCaseCount(int test_case_count) {
  return FormatCountableNoun(test_case_count, "test case", "test cases");
}

// Converts a TestPartResult::Type enum to human-friendly string
// representation.  Both kNonFatalFailure and kFatalFailure are translated
// to "Failure", as the user usually doesn't care about the difference
// between the two when viewing the test result.
static const char* TestPartResultTypeToString(
    ::testing::TestPartResult::Type type) {
  switch (type) {
    case ::testing::TestPartResult::kSuccess:
      return "Success";

    case ::testing::TestPartResult::kNonFatalFailure:
    case ::testing::TestPartResult::kFatalFailure:
#ifdef _MSC_VER
      return "error: ";
#else
      return "Failure\n";
#endif
    default:
      return "Unknown result type";
  }
}

// Prints a TestPartResult to an std::string.
static std::string PrintTestPartResultToString(
    const ::testing::TestPartResult& test_part_result) {
  return (::testing::Message()
          << ::testing::internal::FormatFileLocation(
                 test_part_result.file_name(), test_part_result.line_number())
          << " " << TestPartResultTypeToString(test_part_result.type())
          << test_part_result.message())
      .GetString();
}
}  // anonymous namespace

// Try to emulate the default printer but use android logging to do it.
class AndroidUnitTestPrinter : public ::testing::TestEventListener {
public:
  AndroidUnitTestPrinter()
    : m_test_static_log(&m_console_logger, logging::log_level::max, false),
      m_test_log(m_test_static_log.log()) {}

  virtual void OnTestProgramStart(const ::testing::UnitTest&) {}
  virtual void OnEnvironmentsSetUpEnd(const ::testing::UnitTest&) {}
  virtual void OnEnvironmentsTearDownEnd(const ::testing::UnitTest&) {}
  virtual void OnTestProgramEnd(const ::testing::UnitTest&) {}

  virtual void OnTestIterationStart(
      const ::testing::UnitTest& unit_test, int iteration);
  virtual void OnEnvironmentsSetUpStart(const ::testing::UnitTest& unit_test);
  virtual void OnTestCaseStart(const ::testing::TestCase& test_case);
  virtual void OnTestStart(const ::testing::TestInfo& test_info);
  virtual void OnTestPartResult(const ::testing::TestPartResult& result);
  virtual void OnTestEnd(const ::testing::TestInfo& test_info);
  virtual void OnTestCaseEnd(const ::testing::TestCase& test_case);
  virtual void OnEnvironmentsTearDownStart(
      const ::testing::UnitTest& unit_test);
  virtual void OnTestIterationEnd(
      const ::testing::UnitTest& unit_test, int iteration);

private:
  void PrintFullTestCommentIfPresent(const ::testing::TestInfo& info);
  void PrintFailedTests(const ::testing::UnitTest& unit_test);

  wn::logging::console_logger<> m_console_logger;
  wn::logging::static_log<logging::log_level::max> m_test_static_log;
  wn::logging::log_impl<logging::log_level::max>* m_test_log;
};

static const char sTypeParamLabel[] = "TypeParam";
static const char sValueParamLabel[] = "GetParam()";

void AndroidUnitTestPrinter::PrintFullTestCommentIfPresent(
    const ::testing::TestInfo& test_info) {
  const char* const type_param = test_info.type_param();
  const char* const value_param = test_info.value_param();

  if (type_param != NULL || value_param != NULL) {
    m_test_log->log_info_flags(
        static_cast<size_t>(logging::log_flags::no_header) |
            static_cast<size_t>(logging::log_flags::no_newline),
        ", where ");
    if (type_param != NULL) {
      m_test_log->log_info_flags(
          static_cast<size_t>(logging::log_flags::no_header) |
              static_cast<size_t>(logging::log_flags::no_newline),
          sTypeParamLabel, " = ", type_param);
      if (value_param != NULL) {
        m_test_log->log_info_flags(
            static_cast<size_t>(logging::log_flags::no_header) |
                static_cast<size_t>(logging::log_flags::no_newline),
            " and ");
      }
    }
    if (value_param != NULL) {
      m_test_log->log_info_flags(
          static_cast<size_t>(logging::log_flags::no_header) |
              static_cast<size_t>(logging::log_flags::no_newline),
          sValueParamLabel, " = ", value_param);
    }
  }
}

static const char sUniversalFilter[] = "*";

void AndroidUnitTestPrinter::OnTestIterationStart(
    const ::testing::UnitTest& unit_test, int iteration) {
  if (::testing::GTEST_FLAG(repeat) != 1) {
    m_test_log->log_debug(
        "Repeating all tests (iteration ", iteration + 1, ") . . . ");
  }

  m_test_log->log_debug_flags(
      static_cast<size_t>(logging::log_flags::no_header), "");

  if (::testing::GTEST_FLAG(filter) != sUniversalFilter) {
    m_test_log->log_debug("Note: " GTEST_NAME_ " filter = ",
        ::testing::GTEST_FLAG(filter).c_str());
  }

  if (::testing::GTEST_FLAG(shuffle)) {
    m_test_log->log_debug("Note: Randomizing tests' orders with a seed of ",
        unit_test.random_seed(), " .");
  }

  m_test_log->log_info_flags(
      static_cast<size_t>(logging::log_flags::no_header) |
          static_cast<size_t>(logging::log_flags::no_newline),
      "[==========] ");
  m_test_log->log_info_flags(
      static_cast<size_t>(logging::log_flags::no_header) |
          static_cast<size_t>(logging::log_flags::no_newline),
      "Running ", FormatTestCount(unit_test.test_to_run_count()).c_str(),
      " from ",
      FormatTestCaseCount(unit_test.test_case_to_run_count()).c_str());
  m_test_log->flush();
}

void AndroidUnitTestPrinter::OnEnvironmentsSetUpStart(
    const ::testing::UnitTest&) {
  m_test_log->log_info_flags(
      static_cast<size_t>(logging::log_flags::no_newline),
      "[----------] Global test environment set-up.");
  m_test_log->flush();
}

void AndroidUnitTestPrinter::OnTestCaseStart(
    const ::testing::TestCase& test_case) {
  const std::string counts =
      FormatCountableNoun(test_case.test_to_run_count(), "test", "tests");
  m_test_log->log_info_flags(
      static_cast<size_t>(logging::log_flags::no_newline), "[----------] ");
  if (test_case.type_param() == NULL) {
    m_test_log->log_info_flags(
        static_cast<size_t>(logging::log_flags::no_header), counts.c_str(),
        " from ", test_case.name());
  } else {
    m_test_log->log_info_flags(
        static_cast<size_t>(logging::log_flags::no_header), counts.c_str(),
        " from ", test_case.name(), ", where ", sTypeParamLabel, " = ",
        test_case.type_param());
  }
  m_test_log->flush();
}

void AndroidUnitTestPrinter::OnTestStart(const ::testing::TestInfo& test_info) {
  m_test_log->log_info_flags(
      static_cast<size_t>(logging::log_flags::no_newline), "[ RUN      ] ");
  m_test_log->log_info_flags(static_cast<size_t>(logging::log_flags::no_header),
      test_info.test_case_name(), ".", test_info.name());
  m_test_log->flush();
}

void AndroidUnitTestPrinter::OnTestPartResult(
    const ::testing::TestPartResult& result) {
  if (result.type() == ::testing::TestPartResult::kSuccess)
    return;

  m_test_log->log_info(PrintTestPartResultToString(result).c_str());
  m_test_log->flush();
}

void AndroidUnitTestPrinter::OnTestEnd(const ::testing::TestInfo& test_info) {
  if (test_info.result()->Passed()) {
    m_test_log->log_info_flags(
        static_cast<size_t>(logging::log_flags::no_newline), "[       OK ] ");
  } else {
    m_test_log->log_error_flags(
        static_cast<size_t>(logging::log_flags::no_newline), "[  FAILED  ] ");
  }

  m_test_log->log_info_flags(
      static_cast<size_t>(logging::log_flags::no_header) |
          static_cast<size_t>(logging::log_flags::no_newline),
      test_info.test_case_name(), ".", test_info.name());

  if (test_info.result()->Failed())
    PrintFullTestCommentIfPresent(test_info);

  if (::testing::GTEST_FLAG(print_time)) {
    m_test_log->log_info_flags(
        static_cast<size_t>(logging::log_flags::no_header), " (",
        ::testing::internal::StreamableToString(
            test_info.result()->elapsed_time())
            .c_str(),
        " ms)");
  } else {
    m_test_log->log_info_flags(
        static_cast<size_t>(logging::log_flags::no_header), "");
  }
  m_test_log->flush();
}

void AndroidUnitTestPrinter::OnTestCaseEnd(
    const ::testing::TestCase& test_case) {
  if (!::testing::GTEST_FLAG(print_time))
    return;

  const std::string counts =
      FormatCountableNoun(test_case.test_to_run_count(), "test", "tests");

  m_test_log->log_info("[----------] ", counts.c_str(), " from ",
      test_case.name(), " (",
      ::testing::internal::StreamableToString(test_case.elapsed_time()).c_str(),
      "ms total)");
  m_test_log->flush();
}

void AndroidUnitTestPrinter::OnEnvironmentsTearDownStart(
    const ::testing::UnitTest&) {
  m_test_log->log_info("[----------] Global test environment tear-down\n");
  m_test_log->flush();
}

void AndroidUnitTestPrinter::PrintFailedTests(
    const ::testing::UnitTest& unit_test) {
  const int failed_test_count = unit_test.failed_test_count();
  if (failed_test_count == 0) {
    return;
  }

  for (int i = 0; i < unit_test.total_test_case_count(); ++i) {
    const ::testing::TestCase& test_case = *unit_test.GetTestCase(i);
    if (!test_case.should_run() || (test_case.failed_test_count() == 0)) {
      continue;
    }
    for (int j = 0; j < test_case.total_test_count(); ++j) {
      const ::testing::TestInfo& test_info = *test_case.GetTestInfo(j);
      if (!test_info.should_run() || test_info.result()->Passed()) {
        continue;
      }
      m_test_log->log_error_flags(
          static_cast<size_t>(logging::log_flags::no_newline), "[  FAILED  ] ",
          test_case.name(), ".", test_info.name());
      PrintFullTestCommentIfPresent(test_info);
      m_test_log->log_error_flags(
          static_cast<size_t>(logging::log_flags::no_header), "");
    }
  }
}

void AndroidUnitTestPrinter::OnTestIterationEnd(
    const ::testing::UnitTest& unit_test, int) {
  m_test_log->log_info_flags(
      static_cast<size_t>(logging::log_flags::no_newline), "[==========] ",
      FormatTestCount(unit_test.test_to_run_count()).c_str(), " from ",
      FormatTestCaseCount(unit_test.test_case_to_run_count()).c_str(), " ran.");

  if (::testing::GTEST_FLAG(print_time)) {
    m_test_log->log_info_flags(
        static_cast<size_t>(logging::log_flags::no_header) |
            static_cast<size_t>(logging::log_flags::no_newline),
        " (",
        ::testing::internal::StreamableToString(unit_test.elapsed_time())
            .c_str(),
        " ms total)");
  }
  m_test_log->log_info_flags(
      static_cast<size_t>(logging::log_flags::no_header), "");
  m_test_log->log_info("[  PASSED  ] ",
      FormatTestCount(unit_test.successful_test_count()).c_str(), ".");

  int num_failures = unit_test.failed_test_count();
  if (!unit_test.Passed()) {
    const int failed_test_count = unit_test.failed_test_count();
    m_test_log->log_error("[  FAILED  ] ",
        FormatTestCount(failed_test_count).c_str(), ", listed below");
    PrintFailedTests(unit_test);
    m_test_log->log_error("");
    m_test_log->log_error(
        num_failures, "FAILED ", num_failures == 1 ? "TEST" : "TESTS");
  }

  int num_disabled = unit_test.reportable_disabled_test_count();
  if (num_disabled && !::testing::GTEST_FLAG(also_run_disabled_tests)) {
    if (!num_failures) {
      m_test_log->log_info(0, "");
    }
    m_test_log->log_warning("YOU HAVE ", num_disabled, " DISABLED ",
        num_disabled == 1 ? "TEST." : "TESTS.");
  }
  m_test_log->flush();
}

void init_test_framework() {
  ::testing::TestEventListeners& listeners =
      ::testing::UnitTest::GetInstance()->listeners();
  listeners.Append(new AndroidUnitTestPrinter);
}
}  // namespace testing
}  // namespace wn
