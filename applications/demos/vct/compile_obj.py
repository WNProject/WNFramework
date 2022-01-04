#!/usr/bin/python
# Copyright (c) 2022, WNProject Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

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
from file_utilities import obj_file


def main():
    parser = argparse.ArgumentParser(
        description="Takes a .obj file, (and possible associated mtl files"
        " and converts them to a binary format. Also compresses all the textures")
    parser.add_argument("--objfile",
                        required=True,
                        help="obj file to compile")
    parser.add_argument("--output-file",
                        required=True,
                        help="output file")
    parser.add_argument("--dont-encode",
                        help="don't re-encode any images",
                        action='store_true')
    parser.add_argument("--fast-encode",
                        help="only use a fast encode for the images",
                        action='store_true')
    parser.add_argument("--bc",
                        help="use BC instead of ASTC encoding",
                        action='store_true')

    args = parser.parse_args(sys.argv[1:])
    texture_encoding = obj_file.ASTC_ENCODING
    if args.bc:
        texture_encoding = obj_file.BC_ENCODING

    obj_file.compile_obj_file(args.objfile, args.output_file,
                              texture_encoding, args.dont_encode, args.fast_encode)


if __name__ == "__main__":
    main()
    sys.exit(0)
