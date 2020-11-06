#!/usr/bin/python
# Copyright (c) 2018, WNProject Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE.txt file.

'''
compile_obj.py takes a .obj file and converts it to our custom
binary format as described below.
'''

import sys
sys.dont_write_bytecode = True

import argparse
import array
import os
import shutil
import sys
import re
import tempfile
import math
import subprocess

from PIL import Image
from file_utilities import textures


def main():
    parser = argparse.ArgumentParser(
        description="Converts a texture to either bc or astc format")
    parser.add_argument("--source",
                        required=True,
                        help="source texture")
    parser.add_argument("--output-file",
                        required=True,
                        help="target texture")
    parser.add_argument("--fast-encode",
                        help="only use a fast encode for the images",
                        action='store_true')
    parser.add_argument("--bc",
                        help="use BC instead of ASTC encoding",
                        action='store_true')
    parser.add_argument("--generate-mips",
                        action='store_true',
                        help="Generate mips for this texture")
    parser.add_argument("--normal-map",
                        action='store_true',
                        help="Treat this image as a normal map")

    args = parser.parse_args(sys.argv[1:])

    build_texture = textures.convert_astc_texture
    if args.bc:
        build_texture = textures.convert_bc_texture

    build_texture(args.source, args.output_file, args.fast_encode,
                  args.normal_map, args.generate_mips)


if __name__ == "__main__":
    main()
    sys.exit(0)
