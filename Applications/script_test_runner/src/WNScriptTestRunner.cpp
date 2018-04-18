// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNContainers/inc/WNStringView.h"
#include "WNExecutable/inc/WNEntry.h"
#include "WNFileSystem/inc/WNFactory.h"
#include "WNLogging/inc/WNConsoleLogger.h"
#include "WNMemory/inc/allocation_tracker.h"
#include "WNMemory/inc/basic_allocator.h"
#include "WNRe2/inc/re2.h"
#include "WNScripting/inc/WNFactory.h"
#include "effcee/effcee.h"

using namespace wn;

inline containers::string_view to_view(const re2::StringPiece& sp) {
  return containers::string_view(sp.data(), sp.size());
}

class script_test_allocator : public memory::allocation_tracker {
private:
  using base = memory::allocation_tracker;

public:
  script_test_allocator() : base(&m_allocator) {}

private:
  memory::basic_allocator m_allocator;
};

template <uint32_t N>
bool call_function(logging::log* _log, wn::scripting::engine* e,
    const containers::string_view name,
    const containers::dynamic_array<int32_t>&, int32_t expected_return) {
  WN_RELEASE_ASSERT("Not implemented", N);
  return false;
}

template <>
bool call_function<0>(logging::log* _log, scripting::engine* e,
    const containers::string_view name,
    const containers::dynamic_array<int32_t>&, int32_t expected_return) {
  scripting::script_function<int32_t> func;
  if (!e->get_function(name, func)) {
    _log->log_error(
        "Could not find function ", name, " with ", 0, " parameters");
    return false;
  }
  int32_t ret = e->invoke(func);
  if (expected_return != ret) {
    _log->log_error(
        name, " returned: ", ret, " but we expected: ", expected_return);
    return false;
  }
  return true;
}

template <>
bool call_function<1>(logging::log* _log, scripting::engine* e,
    const containers::string_view name,
    const containers::dynamic_array<int32_t>& a, int32_t expected_return) {
  scripting::script_function<int32_t, int32_t> func;
  if (!e->get_function(name, func)) {
    _log->log_error(
        "Could not find function ", name, " with ", 1, " parameter");
    return false;
  }
  int32_t ret = e->invoke(func, a[0]);
  if (expected_return != ret) {
    _log->log_error(
        name, " returned: ", ret, " but we expected: ", expected_return);
    return false;
  }
  return true;
}

template <>
bool call_function<2>(logging::log* _log, scripting::engine* e,
    const containers::string_view name,
    const containers::dynamic_array<int32_t>& a, int32_t expected_return) {
  scripting::script_function<int32_t, int32_t, int32_t> func;
  if (!e->get_function(name, func)) {
    _log->log_error(
        "Could not find function ", name, " with ", 2, " parameters");
    return false;
  }
  int32_t ret = e->invoke(func, a[0], a[1]);
  if (expected_return != ret) {
    _log->log_error(
        name, " returned: ", ret, " but we expected: ", expected_return);
    return false;
  }
  return true;
}

template <>
bool call_function<3>(logging::log* _log, scripting::engine* e,
    const containers::string_view name,
    const containers::dynamic_array<int32_t>& a, int32_t expected_return) {
  scripting::script_function<int32_t, int32_t, int32_t, int32_t> func;
  if (!e->get_function(name, func)) {
    _log->log_error(
        "Could not find function ", name, " with ", 3, " parameters");
    return false;
  }
  int32_t ret = e->invoke(func, a[0], a[1], a[2]);
  if (expected_return != ret) {
    _log->log_error(
        name, " returned: ", ret, " but we expected: ", expected_return);
    return false;
  }
  return true;
}

enum class test_types : uint8_t { jit = 1 << 0, c = 1 << 1 };

bool call(logging::log* _log, scripting::engine* e,
    containers::string_view _name,
    const containers::dynamic_array<int32_t>& _values,
    int32_t expected_return) {
  switch (_values.size()) {
    case 0:
      return call_function<0>(_log, e, _name, _values, expected_return);
    case 1:
      return call_function<1>(_log, e, _name, _values, expected_return);
    case 2:
      return call_function<2>(_log, e, _name, _values, expected_return);
    case 3:
      return call_function<3>(_log, e, _name, _values, expected_return);
    default:
      WN_RELEASE_ASSERT(false, "Error: unimplemented number of values");
  }
  return false;
}

int32_t wn_main(const ::wn::entry::system_data* _system_data) {
  entry::wn_dummy();
  uint8_t tests_to_run = 0xff;
  script_test_allocator allocator;

  containers::string_view test_file;
  containers::string_view data_dir;
  bool parse_data_dir = false;
  bool parse_test_file = false;
  bool has_data_dir = false;
  bool verbose = false;
  bool only_jit = false;
  bool only_c = false;
  for (size_t i = 0; i < static_cast<size_t>(_system_data->argc); ++i) {
    if (containers::string_view("--test_file") ==
        containers::string_view(_system_data->argv[i])) {
      parse_test_file = true;
      parse_data_dir = false;
      continue;
    }
    if (containers::string_view("--data_dir") ==
        containers::string_view(_system_data->argv[i])) {
      parse_data_dir = true;
      parse_test_file = false;
      continue;
    }
    if (containers::string_view("--verbose") ==
        containers::string_view(_system_data->argv[i])) {
      verbose = true;
    }
    if (containers::string_view("--only_jit") ==
        containers::string_view(_system_data->argv[i])) {
      only_jit = true;
    }
    if (containers::string_view("--only_c") ==
        containers::string_view(_system_data->argv[i])) {
      only_c = true;
    }

    if (parse_test_file) {
      test_file = _system_data->argv[i];
      parse_test_file = false;
    } else if (parse_data_dir) {
      data_dir = _system_data->argv[i];
      has_data_dir = true;
      parse_data_dir = false;
    }
  }

  // These are static so that their 1MB of space does not end up on the stack.
  static logging::console_logger<> logger;
  static logging::static_log<logging::log_level::max, 1024 * 1024> log(
      &logger, verbose ? logging::log_level::max : logging::log_level::error);

  if (only_jit + only_c > 1) {
    log.log()->log_critical(
        "You must specify at most one of only_jit or only_c");
    return (-1);
  }

  if (only_jit) {
    tests_to_run = static_cast<uint8_t>(test_types::jit);
  } else if (only_c) {
    tests_to_run = static_cast<uint8_t>(test_types::c);
  }

  wn::file_system::factory fs_factory(&allocator, _system_data);

  file_system::mapping_ptr files = fs_factory.make_mapping(&allocator,
      has_data_dir ? data_dir : fs_factory.get_current_working_path());
  wn::file_system::mapping_ptr output_mapping =
      wn::file_system::factory(&allocator, _system_data)
          .make_mapping(
              &allocator, wn::file_system::mapping_type::memory_backed);
  output_mapping->create_directory("/");

  scripting::factory script_factory;

  memory::unique_ptr<scripting::engine> jit =
      script_factory.get_engine(&allocator,
          scripting::scripting_engine_type::jit_engine, files.get(), log.log());
  memory::unique_ptr<scripting::translator> translator =
      script_factory.get_translator(&allocator,
          scripting::translator_type::c_translator, files.get(),
          output_mapping.get(), log.log());

  auto test_file_string = test_file.to_string(&allocator);
  file_system::result res;
  file_system::file_ptr iptr = files->open_file(test_file_string, res);
  containers::string input_file =
      containers::string_view(iptr->typed_data<char>(), iptr->size())
          .to_string(&allocator);
  iptr = nullptr;

  if (res != file_system::result::ok) {
    log.log()->log_critical("Error opening input file ", test_file_string);
    return -1;
  }

  if (tests_to_run & static_cast<uint8_t>(test_types::jit)) {
    log.log()->log_info("Testing JIT parsing");
    scripting::parse_error jit_error =
        jit->parse_file(test_file_string.c_str());
    if (jit_error != scripting::parse_error::ok) {
      log.log()->log_critical("Error parsing ", test_file_string, " for jit");
      return -1;
    }
    log.log()->log_info("Finished testing JIT");
  }

  if (tests_to_run & static_cast<uint8_t>(test_types::c)) {
    log.log()->log_info("Testing C translator parsing");
    scripting::parse_error c_error =
        translator->translate_file(test_file_string.c_str());

    if (c_error != scripting::parse_error::ok) {
      log.log()->log_critical(
          "Error parsing ", test_file_string, " for C translation");
      return -1;
    }
    log.log()->log_info("Finished testing C translator parsing");

    containers::string output_file_string = test_file_string + ".c";

    file_system::file_ptr ptr =
        output_mapping->open_file(output_file_string, res);

    if (res != file_system::result::ok) {
      log.log()->log_critical(
          "Error, could not find output file ", test_file_string);
      return -1;
    }

    containers::string_view output_file =
        containers::string_view(ptr->typed_data<char>(), ptr->size());

    effcee::Options o =
        effcee::Options().SetInputName(test_file_string.c_str());

    log.log()->log_info("Testing filecheck results for C translation");
    effcee::Result effceeResult = effcee::Match(
        effcee::StringPiece(output_file.data(), output_file.size()),
        effcee::StringPiece(input_file.data(), input_file.size()), o);

    if (effceeResult.status() != effcee::Result::Status::Ok) {
      log.log()->log_critical(
          "Error, filecheck failed: ", effceeResult.message().c_str());
      log.log()->log_critical("File contents:\n\n", output_file);
      return -1;
    }
    log.log()->log_info("Filecheck results for C translation successful");
  }
  if (tests_to_run & static_cast<uint8_t>(test_types::jit)) {
    re2::RE2::Options options;
    re2::RE2 function_call_re(
        "[[:space:]]*//"
        "[[:space:]]*RUN:[[:space:]]*([a-z][a-zA-Z0-9_]*)((?:[[:space:]]+-?[0-"
        "9]+"
        ")*)[[:space:]]*->[[:space:]]*(-?[0-9]+)[[:space:]]*",
        options);
    auto lines =
        containers::string_view(input_file).split(&allocator, '\n', false);
    re2::StringPiece matches[10];  // For now 10 matches is probably enough
    uint32_t num_matches = 10;
    bool success = true;
    for (auto l : lines) {
      if (function_call_re.Match(re2::StringPiece(l.data(), l.size()), 0,
              l.size(), re2::RE2::ANCHOR_START, matches, num_matches)) {
        // 0 is the whole match
        // 1 is the function name
        // n-1 is the return value
        containers::dynamic_array<int32_t> parameters(&allocator);
        containers::dynamic_array<containers::string_view> params =
            to_view(matches[2]).split(&allocator, "\t\n ");

        uint32_t return_val =
            atol(to_view(matches[3]).to_string(&allocator).c_str());
        for (auto& v : params) {
          parameters.push_back(atol(v.to_string(&allocator).c_str()));
        }

        log.log()->log_info("Calling JIT function ", to_view(matches[1]));
        success &= call(
            log.log(), jit.get(), to_view(matches[1]), parameters, return_val);
        log.log()->log_info("JIT function ", to_view(matches[1]), ": ",
            success ? "SUCCESS" : "FAILURE");
      }
    }
  }

  return 0;
}
