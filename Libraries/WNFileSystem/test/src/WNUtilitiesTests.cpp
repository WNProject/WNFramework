// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "WNFileSystem/src/WNUtilities.h"
#include "WNTesting/inc/WNTestHarness.h"

TEST(utilities, sanitize_path) {
  wn::containers::string path1;
  wn::containers::string path2(".");
  wn::containers::string path3("..");
  wn::containers::string path4("..\\");
  wn::containers::string path5("../");
  wn::containers::string path6("..\\..");
  wn::containers::string path7("../..");
  wn::containers::string path8("..\\..//");
  wn::containers::string path9("../..\\");
  wn::containers::string path10("\\//\\/\\/\\///\\\\/\\/\\/\\/\\\\\\///");
  wn::containers::string path11(
      "..\\//.\\/\\../\\.///..\\.\\/\\/\\/\\/\\..\\..\\///");
  wn::containers::string path12("\\//\\/\\/\\///\\\\/\\/\\/\\/\\\\\\///.");
  wn::containers::string path13("\\//\\/\\/\\///\\\\/\\/\\/\\/\\\\\\///..");
  wn::containers::string path14("../temp//..\\temp/./temp\\.\\temp.txt");
  wn::containers::string path15("../temp//..\\temp/./temp\\.\\temp.txt.");
  wn::containers::string path16("../temp//..\\temp/./temp\\.\\temp.txt..");

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

TEST(utilities, validate_relative_path) {
  const wn::containers::string path1("");
  const wn::containers::string path2("temp.txt");

#ifdef _WN_WINDOWS
  const wn::containers::string path3("..\\");
  const wn::containers::string path4("temp\\temp.txt");
  const wn::containers::string path5("..\\temp\\temp.txt");
  const wn::containers::string path6("\\");
  const wn::containers::string path7("\\temp.txt");
  const wn::containers::string path8("\\..\\");
  const wn::containers::string path9("\\temp\\temp.txt");
  const wn::containers::string path10("\\..\\temp\\temp.txt");
#elif defined _WN_POSIX
  const wn::containers::string path3("../");
  const wn::containers::string path4("temp/temp.txt");
  const wn::containers::string path5("../temp/temp.txt");
  const wn::containers::string path6("/");
  const wn::containers::string path7("/temp.txt");
  const wn::containers::string path8("/../");
  const wn::containers::string path9("/temp/temp.txt");
  const wn::containers::string path10("/../temp/temp.txt");
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