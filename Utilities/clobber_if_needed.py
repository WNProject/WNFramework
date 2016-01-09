#!/usr/bin/python

import argparse
import os
import shutil
import sys
import time

def main():
    parser = argparse.ArgumentParser(
      description="Test for clobber file, and delete build"
        "directory if different")
    parser.add_argument("--build_dir", type=str,
            required=True,
            help="Build Directory")
    parser.add_argument("--source_dir", type=str,
            required=True,
            help="Source Directory")
    args = parser.parse_args(sys.argv[1:])

    build_file = os.path.join(args.build_dir, "clobber.txt")
    source_file = os.path.join(args.source_dir, "clobber.txt")

    try:
      print build_file
      old_time = int(os.path.getmtime(build_file))
    except:
      old_time = 0

      try:
      new_time = int(os.path.getmtime(source_file))
    except:
      new_time = 0

      if (not old_time == new_time):
      try:
        shutil.rmtree(args.build_dir)
        os.mkdir(args.build_dir)
        shutil.copy2(source_file, build_file)
      except Exception as e:
        print e
        pass

if __name__ == "__main__":
  main()