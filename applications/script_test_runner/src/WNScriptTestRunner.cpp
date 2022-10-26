// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNContainers/inc/WNHashSet.h"
#include "WNContainers/inc/WNStringView.h"
#include "WNFileSystem/inc/WNFactory.h"
#include "WNLogging/inc/WNConsoleLogger.h"
#include "WNMemory/inc/allocation_tracker.h"
#include "WNMemory/inc/basic_allocator.h"
#include "WNScripting/inc/WNFactory.h"
#include "executable_data/inc/executable_data.h"
#include "executable_entry/inc/executable_entry.h"

#include <effcee/effcee.h>
#include <re2/re2.h>

using namespace wn;

int res_fn(void* user_data) {
  return (int)(uintptr_t)(user_data);
}

struct res_test : scripting::script_actor_type {
  using parent_type = void;
  void export_type(scripting::engine::script_type_importer<res_test>*) {}

  static wn::containers::string_view exported_name() {
    return "ResTest";
  }

  static wn::containers::string_view required_script() {
    return nullptr;
  }

  static void register_scripting(scripting::engine* _engine) {
    _engine->export_script_actor_type<res_test>();
  }
  static bool resolve_scripting(scripting::engine* _engine) {
    return _engine->resolve_script_actor_type<res_test>();
  }
};

class res_test_resource : public scripting::resource_manager {
public:
  res_test_resource(memory::allocator* _allocator)
    : scripting::resource_manager(
          containers::string(_allocator, "ResourceTest")),
      m_allocator(_allocator) {}
  bool convert_to_function(containers::string_view _resource_data,
      containers::string_view, logging::log*, containers::string* dat,
      core::optional<uintptr_t>* _user_data) override {
    (void)_user_data;
    dat->append("getNew").append(_resource_data);
    return true;
  }
  bool setup_resource(
      containers::string_view _res, containers::string* _out_string) override {
    *_out_string += _res;
    *_out_string += ".resdummy";
    return true;
  }
  containers::string_view get_file_extension() const override {
    return ".resdummy";
  }
  scripting::convert_type convert_file(logging::log*, file_system::mapping*,
      containers::string_view _file_name, containers::string*,
      containers::string* out_string) override {
    *out_string = R"(
Actor ResTest {
    @synchronized CString name = "";
    @synchronized CString class_name = "";
    CString get_name() {
        return name;
    }
    CString get_class_name() {
        return class_name;
    }
})";
    *out_string += "Actor ";
    *out_string += _file_name.substr(0, _file_name.size() - 9);
    *out_string += R"( : ResTest
{
  @synchronized Int x = 4;
}
)";
    *out_string += _file_name.substr(0, _file_name.size() - 9);
    *out_string += " getNew";
    *out_string += _file_name.substr(0, _file_name.size() - 9);
    *out_string += +"() {";
    *out_string += _file_name.substr(0, _file_name.size() - 9);
    *out_string += " x = ";
    *out_string += _file_name.substr(0, _file_name.size() - 9);
    *out_string += "(); ";
    *out_string += "return x; }\n";
    return scripting::convert_type::success;
  }

private:
  memory::allocator* m_allocator;
  containers::string s;
};

class test_resource : public scripting::resource_manager {
public:
  test_resource(memory::allocator* _allocator)
    : scripting::resource_manager(
          containers::string(_allocator, "TestResource")) {}
  bool convert_to_function(containers::string_view, containers::string_view,
      logging::log*, containers::string* dat,
      core::optional<uintptr_t>* _user_data) override {
    (void)_user_data;
    *dat = "dummy_file_foo";
    return true;
  }
  bool setup_resource(containers::string_view /*_res*/,
      containers::string* _out_string) override {
    *_out_string = "dummy_file.dummy";
    return true;
  }
  containers::string_view get_file_extension() const override {
    return ".dummy";
  }
  scripting::convert_type convert_file(logging::log* _log,
      file_system::mapping*, containers::string_view _file_name,
      containers::string* _out_data, containers::string* out_string) override {
    (void)_out_data;
    if (_file_name != "dummy_file.dummy") {
      _log->log_error("Invalid file name");
      return scripting::convert_type::failed;
    }
    *out_string = "Int dummy_file_foo() { return 4; }";
    return scripting::convert_type::success;
  }

private:
};

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

template <typename T>
bool call_function0(logging::log* _log, scripting::engine* e,
    const containers::string_view name,
    const containers::dynamic_array<int32_t>&, T expected_return) {
  scripting::script_function<T> func;
  if (!e->get_function(name, &func)) {
    _log->log_error(
        "Could not find function ", name, " with ", 0, " parameters");
    return false;
  }
  T ret = e->invoke(func);
  if (expected_return != ret) {
    _log->log_error(
        name, " returned: ", ret, " but we expected: ", expected_return);
    return false;
  }
  return true;
}

template <typename T>
bool call_function1(logging::log* _log, scripting::engine* e,
    const containers::string_view name,
    const containers::dynamic_array<int32_t>& a, T expected_return) {
  scripting::script_function<T, int32_t> func;
  if (!e->get_function(name, &func)) {
    _log->log_error(
        "Could not find function ", name, " with ", 1, " parameter");
    return false;
  }
  T ret = e->invoke(func, a[0]);
  if (expected_return != ret) {
    _log->log_error(
        name, " returned: ", ret, " but we expected: ", expected_return);
    return false;
  }
  return true;
}

template <typename T>
bool call_function2(logging::log* _log, scripting::engine* e,
    const containers::string_view name,
    const containers::dynamic_array<int32_t>& a, T expected_return) {
  scripting::script_function<T, int32_t, int32_t> func;
  if (!e->get_function(name, &func)) {
    _log->log_error(
        "Could not find function ", name, " with ", 2, " parameters");
    return false;
  }
  T ret = e->invoke(func, a[0], a[1]);
  if (expected_return != ret) {
    _log->log_error(
        name, " returned: ", ret, " but we expected: ", expected_return);
    return false;
  }
  return true;
}

template <typename T>
bool call_function3(logging::log* _log, scripting::engine* e,
    const containers::string_view name,
    const containers::dynamic_array<int32_t>& a, T expected_return) {
  scripting::script_function<T, int32_t, int32_t, int32_t> func;
  if (!e->get_function(name, &func)) {
    _log->log_error(
        "Could not find function ", name, " with ", 3, " parameters");
    return false;
  }
  T ret = e->invoke(func, a[0], a[1], a[2]);
  if (expected_return != ret) {
    _log->log_error(
        name, " returned: ", ret, " but we expected: ", expected_return);
    return false;
  }
  return true;
}

template <typename T>
bool call_function4(logging::log* _log, scripting::engine* e,
    const containers::string_view name,
    const containers::dynamic_array<int32_t>& a, T expected_return) {
  scripting::script_function<T, int32_t, int32_t, int32_t, int32_t> func;
  if (!e->get_function(name, &func)) {
    _log->log_error(
        "Could not find function ", name, " with ", 3, " parameters");
    return false;
  }
  T ret = e->invoke(func, a[0], a[1], a[2], a[3]);
  if (expected_return != ret) {
    _log->log_error(
        name, " returned: ", ret, " but we expected: ", expected_return);
    return false;
  }
  return true;
}

enum class test_types : uint8_t { jit = 1 << 0, c = 1 << 1 };

template <typename T>
bool call(logging::log* _log, scripting::engine* e,
    containers::string_view _name,
    const containers::dynamic_array<int32_t>& _values, T expected_return) {
  switch (_values.size()) {
    case 0:
      return call_function0<T>(_log, e, _name, _values, expected_return);
    case 1:
      return call_function1<T>(_log, e, _name, _values, expected_return);
    case 2:
      return call_function2<T>(_log, e, _name, _values, expected_return);
    case 3:
      return call_function3<T>(_log, e, _name, _values, expected_return);
    case 4:
      return call_function4<T>(_log, e, _name, _values, expected_return);
    default:
      WN_RELEASE_ASSERT(false, "Error: unimplemented number of values");
  }
  return false;
}

static int32_t increment_number(int32_t i) {
  return i + 1;
}

class test_runtime : public wn::scripting::scripting_runtime {
public:
  test_runtime(wn::memory::allocator* _allocator)
    : m_allocator(_allocator), m_actors(_allocator) {}

  wn::scripting::actor_header* allocate_actor(size_t size) override {
    auto t = reinterpret_cast<wn::scripting::actor_header*>(
        m_allocator->allocate(size));
    m_actors.insert(t);
    return t;
  }

  void free_actor(wn::scripting::actor_header* actor) override {
    if (actor->destructor) {
      actor->destructor(actor + 1);
    }
    m_actors.erase(m_actors.find(actor));
    return m_allocator->deallocate(actor);
  }

  void call_actor_function(wn::scripting::actor_function* function) override {
    function->function(function);
  }

  void update_actors() {
    for (auto& it : m_actors) {
      it->update_values(it + 1);
    }
  }

  wn::memory::allocator* m_allocator;
  wn::containers::hash_set<wn::scripting::actor_header*> m_actors;
};

// This is just for testing, but we just want a simple way to update our actors.
static memory::unique_ptr<test_runtime> _rt;

static void update_actors() {
  _rt->update_actors();
}

int32_t wn_main(const ::wn::executable::executable_data* _executable_data) {
  executable::wn_dummy();
  uint8_t tests_to_run = 0xff;
  script_test_allocator allocator;
  _rt = wn::memory::make_unique<test_runtime>(&allocator, &allocator);

  containers::string_view test_file;
  containers::string_view data_dir;
  bool parse_data_dir = false;
  bool parse_test_file = false;
  bool has_data_dir = false;
  bool verbose = false;
  bool only_jit = false;
  bool only_c = false;

  for (size_t i = 0; i < static_cast<size_t>(_executable_data->argc); ++i) {
    if (containers::string_view("--test_file") ==
        containers::string_view(_executable_data->argv[i])) {
      parse_test_file = true;
      parse_data_dir = false;
      continue;
    }
    if (containers::string_view("--data_dir") ==
        containers::string_view(_executable_data->argv[i])) {
      parse_data_dir = true;
      parse_test_file = false;
      continue;
    }
    if (containers::string_view("--verbose") ==
        containers::string_view(_executable_data->argv[i])) {
      verbose = true;
    }
    if (containers::string_view("--only_jit") ==
        containers::string_view(_executable_data->argv[i])) {
      only_jit = true;
    }
    if (containers::string_view("--only_c") ==
        containers::string_view(_executable_data->argv[i])) {
      only_c = true;
    }

    if (parse_test_file) {
      test_file = _executable_data->argv[i];
      parse_test_file = false;
    } else if (parse_data_dir) {
      data_dir = _executable_data->argv[i];
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

  wn::file_system::factory fs_factory(&allocator, _executable_data, log.log());

  file_system::mapping_ptr files = fs_factory.make_mapping(&allocator,
      has_data_dir ? data_dir : fs_factory.get_current_working_path());
  wn::file_system::mapping_ptr output_mapping =
      wn::file_system::factory(&allocator, _executable_data, log.log())
          .make_mapping(
              &allocator, wn::file_system::mapping_type::memory_backed);
  output_mapping->create_directory("/");

  scripting::factory script_factory;

  memory::unique_ptr<scripting::engine> jit = script_factory.get_engine(
      &allocator, scripting::scripting_engine_type::jit_engine, files.get(),
      log.log(), &allocator, &allocator, _rt.get());

  memory::unique_ptr<scripting::translator> translator =
      script_factory.get_translator(&allocator,
          scripting::translator_type::c_translator, files.get(),
          output_mapping.get(), log.log());

  // Register increment_number
  jit->register_function<decltype(&increment_number), &increment_number>(
      "increment_number");
  jit->register_function<decltype(&update_actors), &update_actors>(
      "update_actors");
  res_test::register_scripting(jit.get());

  translator->register_cpp_function("increment_number", &increment_number);
  translator->register_cpp_function("update_actors", &update_actors);

  // Register a named constant;
  const int32_t constant_a = 42;
  const int32_t constant_b = -10;
  jit->register_named_constant<int32_t>("named_constant_a", constant_a);
  jit->register_named_constant<int32_t>("named_constant_b", constant_b);
  translator->register_named_constant<int32_t>("named_constant_a", constant_a);
  translator->register_named_constant<int32_t>("named_constant_b", constant_b);

  jit->register_resource<int>(
      memory::make_unique<test_resource>(&allocator, &allocator));
  translator->register_resource<int>(
      memory::make_unique<test_resource>(&allocator, &allocator));
  jit->register_resource<scripting::script_actor_pointer<res_test>>(
      memory::make_unique<res_test_resource>(&allocator, &allocator));
  translator->register_resource<scripting::script_actor_pointer<res_test>>(
      memory::make_unique<res_test_resource>(&allocator, &allocator));

  auto test_file_string = test_file.to_string(&allocator);
  file_system::result res;
  file_system::file_ptr iptr = files->open_file(test_file_string, res);
  if (!iptr) {
    return -1;
  }

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
    res_test::resolve_scripting(jit.get());
  }
  bool success = true;
  do {
    if (tests_to_run & static_cast<uint8_t>(test_types::c)) {
      log.log()->log_info("Testing C translator parsing");
      scripting::parse_error c_error =
          translator->translate_file(test_file_string.c_str());

      if (c_error != scripting::parse_error::ok) {
        log.log()->log_critical(
            "Error parsing ", test_file_string, " for C translation");
        success = false;
        break;
      }
      log.log()->log_info("Finished testing C translator parsing");

      containers::string output_file_string = test_file_string + ".c";

      file_system::file_ptr ptr =
          output_mapping->open_file(output_file_string, res);

      if (res != file_system::result::ok) {
        log.log()->log_critical(
            "Error, could not find output file ", test_file_string);
        success = false;
        break;
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
        success = false;
        break;
      }
      log.log()->log_info("Filecheck results for C translation successful");
    }
  } while (false);

  do {
    if (tests_to_run & static_cast<uint8_t>(test_types::jit)) {
      re2::RE2::Options options;
      re2::RE2 function_call_re(
          "[[:space:]]*//"
          "[[:space:]]*RUN:[[:space:]]*([a-z][a-zA-Z0-9_]*)((?:[[:space:]]+-?["
          "0-"
          "9]+"
          ")*)[[:space:]]*->[[:space:]]*(-?[0-9]+)[[:space:]]*",
          options);
      auto lines =
          containers::string_view(input_file).split(&allocator, '\n', false);
      re2::StringPiece matches[10];  // For now 10 matches is probably enough
      uint32_t num_matches = 10;
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
          bool this_success = call<int32_t>(log.log(), jit.get(),
              to_view(matches[1]), parameters, return_val);
          if (!success) {
            if (return_val <= std::numeric_limits<uint8_t>::max() &&
                return_val >= 0) {
              log.log()->log_info(
                  "Trying again to find function with Char return ",
                  to_view(matches[1]));
              this_success =
                  call<uint8_t>(log.log(), jit.get(), to_view(matches[1]),
                      parameters, static_cast<uint8_t>(return_val));
              if (this_success) {
                log.log()->log_info(
                    "Char version succeeded ", to_view(matches[1]));
              }
            }
          }
          log.log()->log_info("JIT function ", to_view(matches[1]), ": ",
              this_success ? "SUCCESS" : "FAILURE");
          success &= this_success;
        }
      }
    }
  } while (false);
  log.log()->flush();
  _rt.reset();
  return success ? 0 : -1;
}
