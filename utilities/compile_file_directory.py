#!/usr/bin/python
# Copyright (c) 2017, WNProject Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

import argparse
import os
import shutil
import sys


CPP_FILE_PREFIX = """
// Copyright (c) 2020, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

# include "{prefix}.h"

namespace {prefix} {{

namespace {{
{file_contents}
}} // namespace

const wn::containers::contiguous_range<
    const wn::core::pair<wn::containers::string_view,
                         wn::containers::string_view>> get_files() {{
  static const wn::core::pair<wn::containers::string_view,
    wn::containers::string_view> file_data[] = {{
{contents}
  }};
  return wn::containers::contiguous_range<
    const wn::core::pair<wn::containers::string_view,
                         wn::containers::string_view>>(&file_data[0],
                            sizeof(file_data)/sizeof(file_data[0]));
}}
}} // namespace {prefix}"""

H_FILE_PREFIX = """
// Copyright (c) 2020, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

# pragma once
# include "core/inc/pair.h"
# include "WNContainers/inc/WNStringView.h"
# include "WNContainers/inc/WNContiguousRange.h"

namespace {prefix} {{

const wn::containers::contiguous_range<
    const wn::core::pair<wn::containers::string_view,
                         wn::containers::string_view>> get_files();
}}
"""


class HandleDirectory(argparse.Action):
    def __call__(self, parser, namespace, values, option_string=None):
        if self.ca.num_files_in_dir == 0 and self.ca.last_directory != "":
            self.ca.full_dirs.append(values)
        else:
            self.ca.last_directory = values
            self.ca.num_files_in_dir = 0


class HandleFile(argparse.Action):
    def __call__(self, parser, namespace, values, option_string=None):
        for v in values:
            self.ca.files_dirs[v] = self.ca.last_directory
            self.ca.num_files_in_dir += 1


class CustomAction:
    files_dirs = {}
    last_directory = ""
    num_files_in_dir = 0
    full_dirs = []

    def __init__(self):
        self.HandleDirectory = HandleDirectory
        self.HandleDirectory.ca = self
        self.HandleFile = HandleFile
        self.HandleFile.ca = self


def handle_file(filename):
    """handle_file takes a given filename and parses it
    it returns formatted the contents and the number
    of bytes that were in the file"""
    with open(filename, 'rb') as o:
        fc = "        "
        linect = 0
        lc = 0
        for x in o.read():
            if linect == 12:
                linect = 0
                fc += "\n        "
            linect += 1
            if (sys.version_info > (3, 0)):
                fc += "0x%02X," % x
            else:
                fc += "0x%02X," % ord(x)
            lc += 1
        return (lc, fc[:-1])


def main():
    ca = CustomAction()
    parser = argparse.ArgumentParser(
        description="Takes a directory and outputs a "
        "header and .cpp file that contain all of the "
        "contents of that directory")
    parser.add_argument("--directory",
                        action=ca.HandleDirectory,
                        help="directory root to compile")
    parser.add_argument("--prefix", type=str,
                        default="output",
                        help="prefix to use for the file name"
                        "and the contents")
    parser.add_argument("--output-directory",
                        default=".",
                        help="output directory for the files")
    parser.add_argument("--remove-prefix", 
                        help="Remove the number subdirectories from the directory",
                        type=int,
                        default=0,
                        required=False)
    parser.add_argument("--files",
                        nargs="+",
                        action=ca.HandleFile,
                        help="Specific list of files to gather")

    args = parser.parse_args(sys.argv[1:])
    
    with open(os.path.join(args.output_directory, args.prefix + ".h"), "w") as f:
        f.write(H_FILE_PREFIX.format(
            prefix=args.prefix
        ))

    with open(os.path.join(args.output_directory, args.prefix + ".cpp"), "w") as f:
        filenames = []
        file_contents = []
        contents_length = []
        for d in ca.full_dirs:
            for root, _, files in os.walk(d, topdown=False):
                for name in files:
                    if root == d:
                        fn = name
                    else:
                        fn = os.path.join(os.path.relpath(
                            root, d), name)
                    if args.remove_prefix > 0:
                        print(fn)
                        fn = os.path.join(os.path.normpath(fn).split(os.sep)[args.remove_prefix:])
                        print(fn)

                    filenames.append(fn)
                    lc, fc = handle_file(os.path.join(root, name))
                    file_contents.append(fc)
                    contents_length.append(lc)
        for fl, d in ca.files_dirs.items():
            lc, fc = handle_file(os.path.join(d, fl))
            file_contents.append(fc)
            contents_length.append(lc)
            if args.remove_prefix > 0:
                        fl = os.path.join(*os.path.normpath(fl).split(os.sep)[args.remove_prefix:])
            filenames.append(fl)
        file_data = ""
        contents_data = ""
        view_data = ""
        x = 0
        for a, b, c in zip(filenames, file_contents, contents_length):
            file_data += 'const unsigned char _{n}[] = "{name}";\n'.format(
                n=x, name=str.replace(a, "\\", "/"))
            contents_data += 'const unsigned char _{n}_data[] = {{\n{data}\n}};\n'.format(
                n=x, data=b)
            view_data += '        {{ wn::containers::string_view(reinterpret_cast<const char*>(_{n}), {nl}), wn::containers::string_view(reinterpret_cast<const char*>(_{n}_data), {ndl}) }},\n'.format(
                n=x, nl=str(len(a)), ndl=str(c))
            x += 1
        view_data = view_data[:-2]
        f.write(CPP_FILE_PREFIX.format(
            prefix=args.prefix,
            file_contents=(file_data + contents_data),
            contents=view_data
        ))


if __name__ == "__main__":
    main()
