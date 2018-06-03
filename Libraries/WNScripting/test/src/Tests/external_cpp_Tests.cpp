// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNExecutableTest/inc/WNTestHarness.h"
#include "WNFileSystem/inc/WNFactory.h"
#include "WNMemory/inc/allocator.h"
#include "WNScripting/inc/WNFactory.h"
#include "WNScripting/test/inc/external_test_assets.h"
#include "effcee/effcee.h"

struct external_inner_struct {
  int32_t y;
};

struct external_struct {
  external_struct(external_inner_struct _s2, int32_t _i, int32_t _x)
    : s2(_s2), i(_i), x(_x) {}
  external_inner_struct s2;
  int32_t i;
  int32_t x;
  void increment_i() {
    ++i;
  }
  virtual void increment_x() {
    x++;
  }
};

struct external_struct2 : public external_struct {
  external_struct2() : external_struct({}, 0, 0) {}
  void increment_x() override {
    x += 2;
  }
};

namespace wn {
namespace scripting {
template <>
struct exported_script_type<external_inner_struct> {
  static wn::containers::string_view exported_name() {
    return "ExternalInnerStruct";
  }
  template <typename T>
  static void export_type(T* _exporter) {
    _exporter->register_member("y", &external_inner_struct::y);
  }
};

template <>
struct exported_script_type<external_struct> {
  static wn::containers::string_view exported_name() {
    return "ExternalStruct";
  }
  static void export_type(wn::scripting::exporter<external_struct>* _exporter) {
    _exporter->register_member("i", &external_struct::i);
    _exporter->register_member("s2", &external_struct::s2);
    _exporter->register_nonvirtual_function(
        "increment_i", &external_struct::increment_i);
    _exporter->register_virtual<void>()
        .register_virtual<&external_struct::increment_x>("increment_x");
  }
};

template <>
struct exported_script_type<external_struct2> {
  using parent_type = external_struct;
  static wn::containers::string_view exported_name() {
    return "ExternalStructChild";
  }
  template <typename T>
  static void export_type(T*) {}
};

}  // namespace scripting
}  // namespace wn

TEST(scripting_engine_factory, external) {
  wn::scripting::factory factory;
  wn::testing::allocator allocator;
  wn::file_system::mapping_ptr mapping =
      wn::file_system::factory(&allocator, wn::testing::k_system_data)
          .make_mapping(
              &allocator, wn::file_system::mapping_type::memory_backed);
  mapping->initialize_files(external_test_assets::get_files());

  wn::memory::unique_ptr<wn::scripting::engine> jit = factory.get_engine(
      &allocator, wn::scripting::scripting_engine_type::jit_engine,
      mapping.get(), wn::logging::get_null_logger());

  wn::memory::unique_ptr<wn::scripting::translator> translator =
      factory.get_translator(&allocator,
          wn::scripting::translator_type::c_translator, mapping.get(),
          mapping.get(), wn::logging::get_null_logger());

  // Register our new type.
  jit->register_cpp_type<external_inner_struct>();
  jit->register_cpp_type<external_struct>();
  jit->register_child_cpp_type<external_struct2>();
  translator->register_cpp_type<external_inner_struct>();
  translator->register_cpp_type<external_struct>();
  translator->register_child_cpp_type<external_struct2>();

  wn::file_system::result res;
  wn::file_system::file_ptr iptr =
      mapping->open_file("src/Tests/ExternalTest/external_test.wns", res);
  wn::containers::string input_file =
      wn::containers::string_view(iptr->typed_data<char>(), iptr->size())
          .to_string(&allocator);
  iptr = nullptr;

  ASSERT_EQ(wn::file_system::result::ok, res);

  wn::scripting::parse_error jit_error =
      jit->parse_file("src/Tests/ExternalTest/external_test.wns");
  ASSERT_EQ(wn::scripting::parse_error::ok, jit_error);

  wn::scripting::parse_error c_error =
      translator->translate_file("src/Tests/ExternalTest/external_test.wns");
  ASSERT_EQ(wn::scripting::parse_error::ok, c_error);

  wn::containers::string output_file_string = wn::containers::string(
      &allocator, "src/Tests/ExternalTest/external_test.wns.c");

  wn::file_system::file_ptr ptr = mapping->open_file(output_file_string, res);
  ASSERT_NE(nullptr, ptr);

  wn::containers::string_view output_file =
      wn::containers::string_view(ptr->typed_data<char>(), ptr->size());

  effcee::Options o = effcee::Options().SetInputName(
      "src/Tests/ExternalTest/external_test.wns");

  effcee::Result effceeResult =
      effcee::Match(effcee::StringPiece(output_file.data(), output_file.size()),
          effcee::StringPiece(input_file.data(), input_file.size()), o);

  EXPECT_EQ(effcee::Result::Status::Ok, effceeResult.status())
      << effceeResult.message();

  wn::scripting::script_function<int32_t, external_struct*> extern_struct_func;
  wn::scripting::script_function<int32_t, external_struct*> extern_struct_func2;
  wn::scripting::script_function<int32_t, external_struct*> extern_struct_func3;
  wn::scripting::script_function<int32_t, external_struct*> extern_struct_func4;
  wn::scripting::script_function<int32_t, external_struct2*>
      extern_struct_func5;

  ASSERT_TRUE(jit->get_function("test1", &extern_struct_func));
  ASSERT_TRUE(jit->get_function("test2", &extern_struct_func2));
  ASSERT_TRUE(jit->get_function("test3", &extern_struct_func3));
  ASSERT_TRUE(jit->get_function("test4", &extern_struct_func4));
  ASSERT_TRUE(jit->get_function("test5", &extern_struct_func5));

  // test1
  {
    external_struct s{{}, 32, 4};
    external_struct s2{{}, 42, 9};
    EXPECT_EQ(32, jit->invoke(extern_struct_func, &s));
    EXPECT_EQ(42, jit->invoke(extern_struct_func, &s2));
  }

  // test2
  {
    external_struct s{{}, 32, 12};
    EXPECT_EQ(64, jit->invoke(extern_struct_func2, &s));
    EXPECT_EQ(64, s.i);
  }

  // test3
  {
    external_struct s{{1024}, 32, 1024};
    EXPECT_EQ(1024, jit->invoke(extern_struct_func3, &s));
    EXPECT_EQ(1024, s.s2.y);
  }

  // test4
  {
    external_struct s{{42}, 1, 3};

    ASSERT_EQ(2, jit->invoke(extern_struct_func4, &s));
    EXPECT_EQ(2, s.i);
    EXPECT_EQ(4, s.x);
    external_struct2 s2;

    EXPECT_EQ(1,
        jit->invoke(extern_struct_func4, static_cast<external_struct*>(&s2)));
    EXPECT_EQ(1, s2.i);
    EXPECT_EQ(2, s2.x);
  }

  // test5
  {
    external_struct2 s2;

    EXPECT_EQ(1, jit->invoke(extern_struct_func5, &s2));
    EXPECT_EQ(1, s2.i);
    EXPECT_EQ(2, s2.x);
  }
}
