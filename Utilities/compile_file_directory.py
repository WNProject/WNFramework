#!/usr/bin/python
# Copyright (c) 2017, WNProject Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE.txt file.

import argparse
import os
import shutil
import sys


CPP_FILE_PREFIX = """
// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

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
// Copyright (c) 2017, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

# pragma once
# include "WNCore/inc/WNPair.h"
# include "WNContainers/inc/WNStringView.h"
# include "WNContainers/inc/WNContiguousRange.h"

namespace {prefix} {{

const wn::containers::contiguous_range<
    const wn::core::pair<wn::containers::string_view,
                         wn::containers::string_view>> get_files();
}}
"""


def main():
    parser = argparse.ArgumentParser(
        description="Takes a directory and outputs a "
        "header and .cpp file that contain all of the "
        "contents of that directory")
    parser.add_argument("--directory", type=str,
                        required=True,
                        help="directory root to compile")
    parser.add_argument("--prefix", type=str,
                        default="output",
                        help="prefix to use for the file name"
                        "and the contents")
    parser.add_argument("--output-directory",
                        default=".",
                        help="output directory for the files")
    args = parser.parse_args(sys.argv[1:])

    with open(os.path.join(args.output_directory, args.prefix + ".h"), "w") as f:
        f.write(H_FILE_PREFIX.format(
            prefix=args.prefix
        ))

    with open(os.path.join(args.output_directory, args.prefix + ".cpp"), "w") as f:
        filenames = []
        file_contents = []
        contents_length = []
        for root, _, files in os.walk(args.directory, topdown=False):
            for name in files:
                if root == args.directory:
                    filenames.append(name)
                else:
                    filenames.append(os.path.join(os.path.relpath(
                        root, args.directory), name))
                with open(os.path.join(root, name), 'rb') as o:
                    fc = "        "
                    linect = 0
                    lc = 0
                    for x in o.read():
                        if linect == 12:
                            linect = 0
                            fc += "\n        "
                        linect += 1
                        fc += "0x%02X," % ord(x)
                        lc += 1
                    contents_length.append(lc)
                    file_contents.append(fc[:-1])
        file_data = ""
        contents_data = ""
        view_data = ""
        x = 0
        for a, b, c in zip(filenames, file_contents, contents_length):
            file_data += 'const char _{n}[] = "{name}";\n'.format(
                n=x, name=str.replace(a, "\\", "/"))
            contents_data += 'const char _{n}_data[] = {{\n{data}\n}};\n'.format(
                n=x, data=b)
            view_data += '        {{ wn::containers::string_view(_{n}, {nl}), wn::containers::string_view(_{n}_data, {ndl}) }},\n'.format(
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
