#!/usr/bin/python
# Copyright (c) 2021, WNProject Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

'''
compile_obj.py takes a .obj file and converts it to our custom
binary format as described below.
'''

import sys
sys.dont_write_bytecode = True

import argparse
import os
import sys
import subprocess
import itertools

D3D12 = (["dxc", "-Zss", "-Fo"], ["-T", "vs_6_0"],
         ["-T", "ps_6_0"], ".dxbc", "-D", ".hlsl")
VULKAN = (["dxc", "-Zss", "-spirv", "-Fo"], ["-T", "vs_6_0"],
          ["-T", "ps_6_0"],  ".spv", "-D", ".hlsl")

OPTIONS = ["HAS_DIFFUSE", "HAS_SPEC", "HAS_MASK", "HAS_NORMAL"]


def main():
    parser = argparse.ArgumentParser(
        description="Converts a texture to either bc or astc format")
    parser.add_argument("--source",
                        required=True,
                        help="Source directory")
    parser.add_argument("--d3d12",
                        help="use BC instead of ASTC encoding",
                        action='store_true')
    parser.add_argument("--vulkan",
                        action='store_true',
                        help="Generate mips for this texture")

    command_line_args = parser.parse_args(sys.argv[1:])
    APIS = [VULKAN, D3D12]
    if command_line_args.d3d12:
        APIS = [D3D12]
    if command_line_args.vulkan:
        APIS = [VULKAN]

    def generate(api, options, input_name, output_name, stage):
        compile_args = []
        compile_args.extend(api[0])
        compile_args.append(output_name + api[3])
        compile_args.extend(api[stage])
        for o in options:
            compile_args.append(api[4] + o + "=1")
        compile_args.append(os.path.join(command_line_args.source, input_name))
        print compile_args
        subprocess.call(compile_args)

    for api in APIS:
        # First compile the vertex shader. It is easy
        generate(api, [], "vertex" + api[5], "vertex", 1)
        for i in range(0, 16):
            args = []
            if i & 1:
                args.append(OPTIONS[0])
            if i & 2:
                args.append(OPTIONS[1])
            if i & 4:
                args.append(OPTIONS[2])
            if i & 8:
                args.append(OPTIONS[3])
            generate(api, args, "pixel" + api[5], "pixel_" + str(i), 2)


if __name__ == "__main__":
    main()
    sys.exit(0)
