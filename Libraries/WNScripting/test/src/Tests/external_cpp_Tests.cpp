// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNFileSystem/inc/WNFactory.h"
#include "WNLogging/inc/WNBufferLogger.h"
#include "WNMemory/inc/allocator.h"
#include "WNScripting/inc/WNFactory.h"
#include "WNScripting/test/inc/external_test_assets.h"
#include "effcee/effcee.h"
#include "executable_test/inc/WNTestHarness.h"

using wn::scripting::script_pointer;
using wn::scripting::shared_script_pointer;
using wn::scripting::slice;
using wn::scripting::wn_array;
using wn::scripting::wn_array_ptr;

void flush_buffer(void* v, const char* bytes, size_t length,
    const wn::logging::color_element*, size_t) {
  wn::containers::string* s = static_cast<wn::containers::string*>(v);
  s->append(bytes, length);
}

using buffer_logger = wn::logging::buffer_logger<flush_buffer>;
using log_buff = wn::containers::string;

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
  wn::containers::string str;
  external_struct2() : external_struct({}, 0, 0) {}
  void increment_x() override {
    x += 2;
  }

  int32_t get_y(wn_array_ptr<int32_t> _arr, int32_t y) {
    return (*_arr)[y];
  }

  void printf(const char* c) {
    str.append(c);
  }
};

struct external_slice_struct {
  int32_t vals[32];
  external_slice_struct() {
    for (int32_t i = 0; i < 32; ++i) {
      vals[i] = i;
    }
  }

  int32_t do_x(wn::scripting::slice<int32_t> x) {
    return x[1];
  }

  slice<int32_t> get_vals() {
    return slice<int32_t>(&vals[0], {32});
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
    _exporter->register_nonvirtual_function<
        decltype(&external_struct::increment_i), &external_struct::increment_i>(
        "increment_i");
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
  static void export_type(
      wn::scripting::exporter<external_struct2>* _exporter) {
    _exporter->register_nonvirtual_function<decltype(&external_struct2::get_y),
        &external_struct2::get_y>("get_y");
    _exporter->register_nonvirtual_function<decltype(&external_struct2::printf),
        &external_struct2::printf>("printf");
  }
};

template <>
struct exported_script_type<external_slice_struct> {
  static wn::containers::string_view exported_name() {
    return "ExternalSliceStruct";
  }
  static void export_type(
      wn::scripting::exporter<external_slice_struct>* _exporter) {
    _exporter->register_nonvirtual_function<
        decltype(&external_slice_struct::do_x), &external_slice_struct::do_x>(
        "do_x");
    _exporter->register_nonvirtual_function<
        decltype(&external_slice_struct::get_vals),
        &external_slice_struct::get_vals>("get_vals");
  }
};

}  // namespace scripting
}  // namespace wn

struct a_type : wn::scripting::script_object_type {
  using parent_type = void;

  void export_type(
      wn::scripting::engine::script_type_importer<a_type>* _importer) {
    _importer->register_function("do_a", &do_a);
  }
  static wn::containers::string_view exported_name() {
    return "ExportedA";
  }

  wn::scripting::scripting_object_function<a_type, int32_t, int32_t> do_a;
};

struct b_type : a_type {
  using parent_type = a_type;
  void export_type(
      wn::scripting::engine::script_type_importer<b_type>* _importer) {
    _importer->register_function("do_b", &do_b);
    _importer->register_function("do_c", &do_c);
  }
  static wn::containers::string_view exported_name() {
    return "ExportedB";
  }

  wn::scripting::scripting_object_function<b_type, int32_t, int32_t> do_b;
  wn::scripting::scripting_virtual_object_function<b_type, int32_t, int32_t>
      do_c;
};

TEST(scripting_engine_factory, external) {
  wn::scripting::factory factory;
  wn::testing::allocator allocator;

  log_buff buffer(&allocator);
  buffer_logger logger(&buffer);
  wn::logging::static_log<> slog(&logger);
  wn::logging::log* log = slog.log();

  wn::file_system::mapping_ptr mapping =
      wn::file_system::factory(&allocator, wn::testing::k_executable_data)
          .make_mapping(
              &allocator, wn::file_system::mapping_type::memory_backed);
  mapping->initialize_files(external_test_assets::get_files());

  wn::memory::unique_ptr<wn::scripting::engine> jit =
      factory.get_engine(&allocator,
          wn::scripting::scripting_engine_type::jit_engine, mapping.get(), log);

  wn::memory::unique_ptr<wn::scripting::translator> translator =
      factory.get_translator(&allocator,
          wn::scripting::translator_type::c_translator, mapping.get(),
          mapping.get(), log);

  // Register our new type.
  jit->register_cpp_type<external_inner_struct>();
  jit->register_cpp_type<external_struct>();
  jit->register_cpp_type<external_slice_struct>();
  jit->register_child_cpp_type<external_struct2>();
  jit->export_script_type<a_type>();
  jit->export_script_type<b_type>();

  translator->register_cpp_type<external_inner_struct>();
  translator->register_cpp_type<external_struct>();
  translator->register_cpp_type<external_slice_struct>();
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
  ASSERT_EQ(wn::scripting::parse_error::ok, jit_error)
      << (log->flush(), buffer.c_str());

  wn::scripting::parse_error c_error =
      translator->translate_file("src/Tests/ExternalTest/external_test.wns");
  ASSERT_EQ(wn::scripting::parse_error::ok, c_error)
      << (log->flush(), buffer.c_str());

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
  wn::scripting::script_function<int32_t, wn_array_ptr<int32_t>, int32_t>
      extern_func_6;
  wn::scripting::script_function<int32_t, external_struct2*, int32_t>
      extern_func_7;
  wn::scripting::script_function<int32_t, external_struct2*> extern_func_8;
  wn::scripting::script_function<int32_t, external_slice_struct*> extern_func_9;
  wn::scripting::script_function<int32_t, slice<int32_t>, int32_t>
      extern_func_10;
  wn::scripting::script_function<int32_t, external_slice_struct*, int32_t>
      extern_func_11;
  wn::scripting::script_function<slice<int32_t>, wn_array_ptr<int32_t>, int32_t>
      extern_func_12;
  wn::scripting::script_function<shared_script_pointer<a_type>> extern_func_13;
  wn::scripting::script_function<shared_script_pointer<b_type>> extern_func_14;
  wn::scripting::script_function<shared_script_pointer<b_type>> extern_func_15;

  ASSERT_TRUE(jit->get_function("test1", &extern_struct_func));
  ASSERT_TRUE(jit->get_function("test2", &extern_struct_func2));
  ASSERT_TRUE(jit->get_function("test3", &extern_struct_func3));
  ASSERT_TRUE(jit->get_function("test4", &extern_struct_func4));
  ASSERT_TRUE(jit->get_function("test5", &extern_struct_func5));
  ASSERT_TRUE(jit->get_function("test6", &extern_func_6));
  ASSERT_TRUE(jit->get_function("test7", &extern_func_7));
  ASSERT_TRUE(jit->get_function("test8", &extern_func_8));
  ASSERT_TRUE(jit->get_function("test9", &extern_func_9));
  ASSERT_TRUE(jit->get_function("test10", &extern_func_10));
  ASSERT_TRUE(jit->get_function("test11", &extern_func_11));
  ASSERT_TRUE(jit->get_function("test12", &extern_func_12));
  ASSERT_TRUE(jit->get_function("test13", &extern_func_13));
  ASSERT_TRUE(jit->get_function("test14", &extern_func_14));
  ASSERT_TRUE(jit->get_function("test15", &extern_func_15));

  ASSERT_TRUE(jit->resolve_script_type<a_type>());
  ASSERT_TRUE(jit->resolve_script_type<b_type>());

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

  // test6
  {
    wn_array<int32_t, 42> array;
    for (int32_t i = 0; i < 42; ++i) {
      array[i] = i;
    }
    EXPECT_EQ(15, jit->invoke(extern_func_6, &array, 15));
    EXPECT_EQ(17, jit->invoke(extern_func_6, &array, 17));
    EXPECT_EQ(15, jit->invoke(extern_func_6, &array, 15));
    EXPECT_NE(12, jit->invoke(extern_func_6, &array, 102));
  }

  // test7
  {
    external_struct2 s2;
    EXPECT_EQ(15, jit->invoke(extern_func_7, &s2, 15));
    EXPECT_EQ(22, jit->invoke(extern_func_7, &s2, 22));
  }

  // test8
  {
    external_struct2 s2;
    s2.str = wn::containers::string(&allocator);
    jit->invoke(extern_func_8, &s2);
    EXPECT_EQ("Hello World!", s2.str);
  }

  // test9
  {
    external_slice_struct s;
    EXPECT_EQ(5, jit->invoke(extern_func_9, &s));
  }

  // test 10
  {
    int32_t foo[12];
    for (uint32_t i = 0; i < 12; ++i) {
      foo[i] = i;
    }
    slice<int32_t> s(&foo[0], {12});
    EXPECT_EQ(0, jit->invoke(extern_func_10, s, 0));
    EXPECT_EQ(7, jit->invoke(extern_func_10, s, 7));
    EXPECT_EQ(11, jit->invoke(extern_func_10, s, 11));
  }

  // test11
  {
    external_slice_struct s;
    EXPECT_EQ(0, jit->invoke(extern_func_11, &s, 0));
    EXPECT_EQ(12, jit->invoke(extern_func_11, &s, 12));
    EXPECT_EQ(15, jit->invoke(extern_func_11, &s, 15));
  }

  // test12
  {
    wn_array<int32_t, 42> array;
    for (int32_t i = 0; i < 42; ++i) {
      array[i] = i;
    }
    slice<int> s = jit->invoke(extern_func_12, &array, 5);
    EXPECT_EQ(5, s[0]);
    EXPECT_EQ(7, s[2]);
    EXPECT_EQ(37u, s.size());
  }

  // test13
  {
    shared_script_pointer<a_type> arr_t = jit->invoke(extern_func_13);
    EXPECT_NE(nullptr, arr_t.unsafe_ptr());
    EXPECT_EQ(7, arr_t.invoke(&a_type::do_a, 4));
    EXPECT_EQ(19, arr_t.invoke(&a_type::do_a, 12));
  }

  // test14
  {
    shared_script_pointer<b_type> arr_t = jit->invoke(extern_func_14);
    EXPECT_NE(nullptr, arr_t.unsafe_ptr());
    EXPECT_EQ(7, arr_t.get().parent().invoke(&a_type::do_a, 4));
    EXPECT_EQ(15, arr_t.invoke(&b_type::do_b, 4));
  }

  // test14-2
  {
    shared_script_pointer<b_type> arr_t = jit->invoke(extern_func_14);
    EXPECT_NE(nullptr, arr_t.unsafe_ptr());
    EXPECT_EQ(11, arr_t.invoke(&b_type::do_b, 4));
  }

  // test15
  {
    shared_script_pointer<b_type> arr_t = jit->invoke(extern_func_15);
    EXPECT_NE(nullptr, arr_t.unsafe_ptr());
    EXPECT_EQ(12, arr_t.invoke(&b_type::do_c, 4));
  }
}
