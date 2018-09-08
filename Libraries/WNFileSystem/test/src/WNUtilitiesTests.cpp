// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "executable_test/inc/WNTestHarness.h"
#include "WNFileSystem/src/WNUtilities.h"

using ::testing::ElementsAre;

TEST(utilities, sanitize_path) {
  wn::testing::allocator test_allocator;

  {
    wn::containers::string path1(&test_allocator);
    wn::containers::string path2(&test_allocator, ".");
    wn::containers::string path3(&test_allocator, "..");
    wn::containers::string path4(&test_allocator, "..\\");
    wn::containers::string path5(&test_allocator, "../");
    wn::containers::string path6(&test_allocator, "..\\..");
    wn::containers::string path7(&test_allocator, "../..");
    wn::containers::string path8(&test_allocator, "..\\..//");
    wn::containers::string path9(&test_allocator, "../..\\");
    wn::containers::string path10(
        &test_allocator, "\\//\\/\\/\\///\\\\/\\/\\/\\/\\\\\\///");
    wn::containers::string path11(
        &test_allocator, "..\\//.\\/\\../\\.///..\\.\\/\\/\\/\\/\\..\\..\\///");
    wn::containers::string path12(
        &test_allocator, "\\//\\/\\/\\///\\\\/\\/\\/\\/\\\\\\///.");
    wn::containers::string path13(
        &test_allocator, "\\//\\/\\/\\///\\\\/\\/\\/\\/\\\\\\///..");
    wn::containers::string path14(
        &test_allocator, "../temp//..\\temp/./temp\\.\\temp.txt");
    wn::containers::string path15(
        &test_allocator, "../temp//..\\temp/./temp\\.\\temp.txt.");
    wn::containers::string path16(
        &test_allocator, "../temp//..\\temp/./temp\\.\\temp.txt..");

    wn::file_system::internal::sanitize_path(path1);
    wn::file_system::internal::sanitize_path(path2);
    wn::file_system::internal::sanitize_path(path3);
    wn::file_system::internal::sanitize_path(path4);
    wn::file_system::internal::sanitize_path(path5);
    wn::file_system::internal::sanitize_path(path6);
    wn::file_system::internal::sanitize_path(path7);
    wn::file_system::internal::sanitize_path(path8);
    wn::file_system::internal::sanitize_path(path9);
    wn::file_system::internal::sanitize_path(path10);
    wn::file_system::internal::sanitize_path(path11);
    wn::file_system::internal::sanitize_path(path12);
    wn::file_system::internal::sanitize_path(path13);
    wn::file_system::internal::sanitize_path(path14);
    wn::file_system::internal::sanitize_path(path15);
    wn::file_system::internal::sanitize_path(path16);

    EXPECT_EQ(path1, "");
    EXPECT_EQ(path2, "");

#ifdef _WN_WINDOWS
    EXPECT_EQ(path3, "..\\");
    EXPECT_EQ(path4, "..\\");
    EXPECT_EQ(path5, "..\\");
    EXPECT_EQ(path6, "..\\..\\");
    EXPECT_EQ(path7, "..\\..\\");
    EXPECT_EQ(path8, "..\\..\\");
    EXPECT_EQ(path9, "..\\..\\");
    EXPECT_EQ(path10, "\\");
    EXPECT_EQ(path11, "..\\..\\..\\..\\..\\");
    EXPECT_EQ(path12, "\\");
    EXPECT_EQ(path13, "\\..\\");
    EXPECT_EQ(path14, "..\\temp\\temp\\temp.txt");
    EXPECT_EQ(path15, "..\\temp\\temp\\temp.txt.");
    EXPECT_EQ(path16, "..\\temp\\temp\\temp.txt..");
#elif defined _WN_POSIX
    EXPECT_EQ(path3, "../");
    EXPECT_EQ(path4, "../");
    EXPECT_EQ(path5, "../");
    EXPECT_EQ(path6, "../../");
    EXPECT_EQ(path7, "../../");
    EXPECT_EQ(path8, "../../");
    EXPECT_EQ(path9, "../../");
    EXPECT_EQ(path10, "/");
    EXPECT_EQ(path11, "../../../../../");
    EXPECT_EQ(path12, "/");
    EXPECT_EQ(path13, "/../");
    EXPECT_EQ(path14, "../temp/temp/temp.txt");
    EXPECT_EQ(path15, "../temp/temp/temp.txt.");
    EXPECT_EQ(path16, "../temp/temp/temp.txt..");
#endif
  }
}

TEST(utilities, validate_relative_path) {
  wn::testing::allocator test_allocator;

  {
    const wn::containers::string path1(&test_allocator);
    const wn::containers::string path2(&test_allocator, "temp.txt");

#ifdef _WN_WINDOWS
    const wn::containers::string path3(&test_allocator, "..\\");
    const wn::containers::string path4(&test_allocator, "temp\\temp.txt");
    const wn::containers::string path5(&test_allocator, "..\\temp\\temp.txt");
    const wn::containers::string path6(&test_allocator, "\\");
    const wn::containers::string path7(&test_allocator, "\\temp.txt");
    const wn::containers::string path8(&test_allocator, "\\..\\");
    const wn::containers::string path9(&test_allocator, "\\temp\\temp.txt");
    const wn::containers::string path10(
        &test_allocator, "\\..\\temp\\temp.txt");
#elif defined _WN_POSIX
    const wn::containers::string path3(&test_allocator, "../");
    const wn::containers::string path4(&test_allocator, "temp/temp.txt");
    const wn::containers::string path5(&test_allocator, "../temp/temp.txt");
    const wn::containers::string path6(&test_allocator, "/");
    const wn::containers::string path7(&test_allocator, "/temp.txt");
    const wn::containers::string path8(&test_allocator, "/../");
    const wn::containers::string path9(&test_allocator, "/temp/temp.txt");
    const wn::containers::string path10(&test_allocator, "/../temp/temp.txt");
#endif

    EXPECT_FALSE(wn::file_system::internal::validate_relative_path(path1));
    EXPECT_TRUE(wn::file_system::internal::validate_relative_path(path2));
    EXPECT_FALSE(wn::file_system::internal::validate_relative_path(path3));
    EXPECT_TRUE(wn::file_system::internal::validate_relative_path(path4));
    EXPECT_FALSE(wn::file_system::internal::validate_relative_path(path5));
    EXPECT_FALSE(wn::file_system::internal::validate_relative_path(path6));
    EXPECT_FALSE(wn::file_system::internal::validate_relative_path(path7));
    EXPECT_FALSE(wn::file_system::internal::validate_relative_path(path8));
    EXPECT_FALSE(wn::file_system::internal::validate_relative_path(path9));
    EXPECT_FALSE(wn::file_system::internal::validate_relative_path(path10));
  }
}

TEST(path_tests, split_sanitized_path) {
  wn::testing::allocator test_allocator;

  {
    wn::containers::string path1(&test_allocator);
    wn::containers::string path2(&test_allocator, ".");
    wn::containers::string path3(&test_allocator, "..");
    wn::containers::string path4(&test_allocator, "..\\");
    wn::containers::string path5(&test_allocator, "../");
    wn::containers::string path6(&test_allocator, "..\\..");
    wn::containers::string path7(&test_allocator, "../..");
    wn::containers::string path8(&test_allocator, "..\\..//");
    wn::containers::string path9(&test_allocator, "../..\\");
    wn::containers::string path10(
        &test_allocator, "\\//\\/\\/\\///\\\\/\\/\\/\\/\\\\\\///");
    wn::containers::string path11(
        &test_allocator, "..\\//.\\/\\../\\.///..\\.\\/\\/\\/\\/\\..\\..\\///");
    wn::containers::string path12(
        &test_allocator, "\\//\\/\\/\\///\\\\/\\/\\/\\/\\\\\\///.");
    wn::containers::string path13(
        &test_allocator, "\\//\\/\\/\\///\\\\/\\/\\/\\/\\\\\\///..");
    wn::containers::string path14(
        &test_allocator, "../temp//..\\temp/./temp\\.\\temp.txt");
    wn::containers::string path15(
        &test_allocator, "../temp//..\\temp/./temp\\.\\temp.txt.");
    wn::containers::string path16(
        &test_allocator, "../temp//..\\temp/./temp\\.\\temp.txt..");

    wn::file_system::internal::sanitize_path(path1);
    wn::file_system::internal::sanitize_path(path2);
    wn::file_system::internal::sanitize_path(path3);
    wn::file_system::internal::sanitize_path(path4);
    wn::file_system::internal::sanitize_path(path5);
    wn::file_system::internal::sanitize_path(path6);
    wn::file_system::internal::sanitize_path(path7);
    wn::file_system::internal::sanitize_path(path8);
    wn::file_system::internal::sanitize_path(path9);
    wn::file_system::internal::sanitize_path(path10);
    wn::file_system::internal::sanitize_path(path11);
    wn::file_system::internal::sanitize_path(path12);
    wn::file_system::internal::sanitize_path(path13);
    wn::file_system::internal::sanitize_path(path14);
    wn::file_system::internal::sanitize_path(path15);
    wn::file_system::internal::sanitize_path(path16);

    wn::containers::dynamic_array<wn::containers::string_view> split_path_1(
        &test_allocator);
    wn::containers::dynamic_array<wn::containers::string_view> split_path_2(
        &test_allocator);
    wn::containers::dynamic_array<wn::containers::string_view> split_path_3(
        &test_allocator);
    wn::containers::dynamic_array<wn::containers::string_view> split_path_4(
        &test_allocator);
    wn::containers::dynamic_array<wn::containers::string_view> split_path_5(
        &test_allocator);
    wn::containers::dynamic_array<wn::containers::string_view> split_path_6(
        &test_allocator);
    wn::containers::dynamic_array<wn::containers::string_view> split_path_7(
        &test_allocator);
    wn::containers::dynamic_array<wn::containers::string_view> split_path_8(
        &test_allocator);
    wn::containers::dynamic_array<wn::containers::string_view> split_path_9(
        &test_allocator);
    wn::containers::dynamic_array<wn::containers::string_view> split_path_10(
        &test_allocator);
    wn::containers::dynamic_array<wn::containers::string_view> split_path_11(
        &test_allocator);
    wn::containers::dynamic_array<wn::containers::string_view> split_path_12(
        &test_allocator);
    wn::containers::dynamic_array<wn::containers::string_view> split_path_13(
        &test_allocator);
    wn::containers::dynamic_array<wn::containers::string_view> split_path_14(
        &test_allocator);
    wn::containers::dynamic_array<wn::containers::string_view> split_path_15(
        &test_allocator);
    wn::containers::dynamic_array<wn::containers::string_view> split_path_16(
        &test_allocator);

    EXPECT_TRUE(
        wn::file_system::internal::split_sanitized_path(path1, split_path_1));
    EXPECT_TRUE(
        wn::file_system::internal::split_sanitized_path(path2, split_path_2));
    EXPECT_TRUE(
        wn::file_system::internal::split_sanitized_path(path3, split_path_3));
    EXPECT_TRUE(
        wn::file_system::internal::split_sanitized_path(path4, split_path_4));
    EXPECT_TRUE(
        wn::file_system::internal::split_sanitized_path(path5, split_path_5));
    EXPECT_TRUE(
        wn::file_system::internal::split_sanitized_path(path6, split_path_6));
    EXPECT_TRUE(
        wn::file_system::internal::split_sanitized_path(path7, split_path_7));
    EXPECT_TRUE(
        wn::file_system::internal::split_sanitized_path(path8, split_path_8));
    EXPECT_TRUE(
        wn::file_system::internal::split_sanitized_path(path9, split_path_9));
    EXPECT_TRUE(
        wn::file_system::internal::split_sanitized_path(path10, split_path_10));
    EXPECT_TRUE(
        wn::file_system::internal::split_sanitized_path(path11, split_path_11));
    EXPECT_TRUE(
        wn::file_system::internal::split_sanitized_path(path12, split_path_12));
    EXPECT_TRUE(
        wn::file_system::internal::split_sanitized_path(path13, split_path_13));
    EXPECT_TRUE(
        wn::file_system::internal::split_sanitized_path(path14, split_path_14));
    EXPECT_TRUE(
        wn::file_system::internal::split_sanitized_path(path15, split_path_15));
    EXPECT_TRUE(
        wn::file_system::internal::split_sanitized_path(path16, split_path_16));

    EXPECT_THAT(split_path_1, ElementsAre());
    EXPECT_THAT(split_path_2, ElementsAre());
    EXPECT_THAT(split_path_3, ElementsAre(".."));
    EXPECT_THAT(split_path_4, ElementsAre(".."));
    EXPECT_THAT(split_path_5, ElementsAre(".."));
    EXPECT_THAT(split_path_6, ElementsAre("..", ".."));
    EXPECT_THAT(split_path_7, ElementsAre("..", ".."));
    EXPECT_THAT(split_path_8, ElementsAre("..", ".."));
    EXPECT_THAT(split_path_9, ElementsAre("..", ".."));
    EXPECT_THAT(split_path_10, ElementsAre());
    EXPECT_THAT(split_path_11, ElementsAre("..", "..", "..", "..", ".."));
    EXPECT_THAT(split_path_12, ElementsAre());
    EXPECT_THAT(split_path_13, ElementsAre(".."));
    EXPECT_THAT(split_path_14, ElementsAre("..", "temp", "temp", "temp.txt"));
    EXPECT_THAT(split_path_15, ElementsAre("..", "temp", "temp", "temp.txt."));
    EXPECT_THAT(split_path_16, ElementsAre("..", "temp", "temp", "temp.txt.."));
  }
}

TEST(path_tests, get_pieces_from_sanitized_path) {
  wn::testing::allocator test_allocator;

  {
    wn::containers::string path1(&test_allocator);
    wn::containers::string path2(&test_allocator, "../");
    wn::containers::string path3(&test_allocator, "a/b.txt");
    wn::containers::string path4(&test_allocator, "a/b/c.txt");
    wn::containers::string path5(&test_allocator, "a.txt");

    wn::file_system::internal::sanitize_path(path1);
    wn::file_system::internal::sanitize_path(path2);
    wn::file_system::internal::sanitize_path(path3);
    wn::file_system::internal::sanitize_path(path4);
    wn::file_system::internal::sanitize_path(path5);

    wn::containers::string_view directory_piece_1;
    wn::containers::string_view directory_piece_2;
    wn::containers::string_view directory_piece_3;
    wn::containers::string_view directory_piece_4;
    wn::containers::string_view directory_piece_5;

    wn::containers::string_view file_piece_1;
    wn::containers::string_view file_piece_2;
    wn::containers::string_view file_piece_3;
    wn::containers::string_view file_piece_4;
    wn::containers::string_view file_piece_5;

    wn::file_system::internal::get_directory_from_sanitized_path(
        path1, directory_piece_1);
    wn::file_system::internal::get_directory_from_sanitized_path(
        path2, directory_piece_2);
    wn::file_system::internal::get_directory_from_sanitized_path(
        path3, directory_piece_3);
    wn::file_system::internal::get_directory_from_sanitized_path(
        path4, directory_piece_4);
    wn::file_system::internal::get_directory_from_sanitized_path(
        path5, directory_piece_5);

    wn::file_system::internal::get_filename_from_sanitized_path(
        path1, file_piece_1);
    wn::file_system::internal::get_filename_from_sanitized_path(
        path2, file_piece_2);
    wn::file_system::internal::get_filename_from_sanitized_path(
        path3, file_piece_3);
    wn::file_system::internal::get_filename_from_sanitized_path(
        path4, file_piece_4);
    wn::file_system::internal::get_filename_from_sanitized_path(
        path5, file_piece_5);

    EXPECT_EQ("", directory_piece_1);
    EXPECT_EQ("", file_piece_1);
    EXPECT_EQ("..", directory_piece_2);
    EXPECT_EQ("", file_piece_2);
    EXPECT_EQ("a", directory_piece_3);
    EXPECT_EQ("b.txt", file_piece_3);

#ifdef _WN_WINDOWS
    EXPECT_EQ("a\\b", directory_piece_4);
#else
    EXPECT_EQ("a/b", directory_piece_4);
#endif

    EXPECT_EQ("c.txt", file_piece_4);
    EXPECT_EQ("", directory_piece_5);
    EXPECT_EQ("a.txt", file_piece_5);
  }
}