#!/usr/bin/env python

import os
import sys
import subprocess
# Only allows a single process to run at a time in windows.
# Allows as many as you want elsewhere.
# First parameter is lock name the rest are the program arguments

if os.name == 'nt':
  from ctypes import *
  _create_mutex = windll.kernel32.CreateMutexA
  _wait_for_single_object = windll.kernel32.WaitForSingleObject
  _release_mutex = windll.kernel32.ReleaseMutex
  mutex = _create_mutex(None, False, sys.argv[1])
  if not mutex:
    raise("Could not create mutex")
  _wait_for_single_object(mutex, -1)
  print "-----------------------"
  print sys.argv
  print "-----------------------"
  try:
    print sys.argv[2:]
    subprocess.check_call(sys.argv[2:])
    pass
  except Exception as e:
    _release_mutex(mutex)
    raise e
  _release_mutex(mutex)
else:
  subprocess.check_call(sys.argv[2:])