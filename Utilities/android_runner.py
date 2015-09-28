#!/usr/bin/env python

import argparse
from threading import Timer
import os
import os.path
import re
import shutil
import signal
import subprocess
import sys
import time
from string import Template

def kill_proc(proc, timeout):
  timeout["value"] = True
  proc.kill()

def run_p(args, working_dir=None):
  subprocess.check_call(args, cwd=working_dir)

def run_p_silent(args):
  with open(os.devnull) as FNULL:
    subprocess.check_call(args, stdout=FNULL,
        stderr=FNULL)

def run_p_background(args):
  file = open(os.devnull)
  return subprocess.Popen(args, stdout=file, stderr=file)

def run_p_output(args, strip=False):
  try:
    output = subprocess.check_output(args)
    if strip:
      output = output.strip(' \r\n\t')
    return 0, output
  except:
    return -1, ""

def run_p_timeout(timeout, args):
  proc = subprocess.Popen(args)
  timer_val = {"value": False}
  timer = Timer(timeout, kill_proc, [proc, timer_val])
  timer.start()
  proc.communicate()
  timer.cancel()
  retcode = proc.returncode
  tval = timer_val.get("value")
  return retcode, tval

def run_p_timeout_retry(timeout_count, retry_count, args):
  while retry_count:
    retry_count = retry_count - 1
    ret_code,timeout = run_p_timeout(timeout_count, args)
    if (not timeout):
        return ret_code
    print "RETRYING"
    time.sleep(10)
  return -1

def info(message, *additional_args):
  print message,
  for arg in additional_args:
    print " ", arg,
  print ""

def error(message, *additional_args):
  print message,
  for arg in additional_args:
    print " ", arg,
  print ""
  exit(-1)

class Runner:
  def run(self):
    parser = argparse.ArgumentParser(
        prog="android_runner.py run",
        description="Run android program")
    parser.add_argument("package_name", type=str,
        help="Package name to start")
    parser.add_argument("activity_name", type=str,
        help="Activity to start")
    parser.add_argument("--debug", action="store_true",
        help="Run the debugger first")
    parser.add_argument("--ndk_dir", type=str,
        help="Android ndk directory for debugging",
        required=False)
    parser.add_argument("--apk_build_dir", type=str,
        help="Directory that the debug apk was built out of",
        required=False)
    parser.add_argument("--gdb", type=str, default='gdb',
        help="gdb executable to run. Must be supplied if debugging",
        required=False)
    parser.add_argument('--target_arch', type=str, default='armeabi-v7a',
        help="target architecture to debug on. Must be supplied if debugging",
        required=False)
    parser.add_argument("--sdk",
        help="Location of the sdk", required=False)
    parser.add_argument("--gdb_command",
        help=("Arguments use for gdb. You can reference the original" +
              " gdb executable wiht $gdb, and the original arguments with" +
              " $args. \n Defaults to \"$gdb $args\""),
        required=False, default="$gdb $args")
    args = parser.parse_args(sys.argv[2:])
    adb = "adb"
    if args.sdk:
      args.sdk = os.path.expanduser(args.sdk)
      adb = os.path.join(args.sdk, "platform-tools", "adb")

    run_p([adb, "logcat", "-c"])
    run_p([adb, "shell", "am", "force-stop", args.package_name])

    if args.debug:
      run_p_silent([adb, "shell", "rm", "/sdcard/stdout.txt"])
      run_p_silent([adb, "shell", "touch", "/sdcard/wait-for-debugger.txt"])

      # Assume that the build was done with gdbserver getting packaged.
      debug_port = '5039'
      abis = []

      ret_val, primary_abi = run_p_output([adb, "shell",
        "getprop", "ro.product.cpu.abilist32"], strip=True)

      if 0 == ret_val and primary_abi:
        abis.extend(primary_abi.split(','))

      ret_val, primary_abi = run_p_output([adb, "shell",
        "getprop", "ro.product.cpu.abilist64"], strip=True)
      if 0 == ret_val and primary_abi:
        abis.extend(primary_abi.split(','), strip=True)

      ret_val, primary_abi = run_p_output([adb, "shell",
        "getprop", "ro.product.cpu.abi"], strip=True)
      if 0 == ret_val and primary_abi:
        abis.extend([primary_abi])

      ret_val, primary_abi = run_p_output([adb, "shell",
        "getprop", "ro.product.cpu.abi2"], strip=True)
      if 0 == ret_val and primary_abi:
        abis.extend([primary_abi])

      info("Found adb abis: ", ' '.join(abis))

      abis = [x.strip(' \n\r\t') for x in abis]

      if not args.target_arch in abis:
        error("Device does not support ", args.target_arch)

      info("Running program with abi: ", args.target_arch)

      is_64 = '64' in args.target_arch

      # Check if gdbserver is in /data/data/package_name/lib
      gdb_server_location = '/data/data/%s/lib/gdbserver' % args.package_name

      try:
        _,output = run_p_output([adb, 'shell', 'ls', gdb_server_location], strip=True)
        if output != gdb_server_location:
          exit("Gdbserver could not be found at %s", gdb_server_location)
      except:
        exit("Gdbserver could not be found at %s", gdb_server_location)

      obj_path = os.path.join(args.apk_build_dir.strip('"'),
                               'obj', 'local',
                               args.target_arch)

      gdb_setup_path = os.path.join(args.apk_build_dir.strip('"'),
                                    'libs', args.target_arch, 'gdb.setup')
      
      libdir_name = ''
      app_process_name = ''
      if (is_64 == 64):
          libdir_name = 'lib64'
          app_process_name = 'app_process64'
      else:
          libdir_name = 'lib'
          try:
            _,output = run_p_output(
                  [adb, 'shell',
                   'test -e /system/bin/app_process32 && echo Found'],
                  strip=True)
            if output == 'Found':
              app_process_name = 'app_process32'
            else:
              app_process_name = 'app_process'
          except:
            app_process_name = 'app_process'

      run_p_silent([adb, 'pull', '/system/bin/%s' % (app_process_name),
             os.path.join(obj_path, app_process_name)])
      info("Downloading ", '/system/bin/%s' % (app_process_name))
      run_p_silent([adb, 'pull', '/system/%s/libc.so' % libdir_name,
             os.path.join(obj_path, "libc.so")])
      info("Downloading ", '/system/%s/libc.so' % (libdir_name))
      run_p_silent([adb, "shell", "am", "start", "%s/.%s" % (args.package_name,
             args.activity_name)])
      info("Starting %s/.%s" % (args.package_name, args.activity_name))

      p = subprocess.Popen([
          adb, "logcat", "-s", args.package_name + ":V",
          "*:F"],
          stdout=subprocess.PIPE,
          bufsize=1)

      startre = re.compile("^--STARTED..?$")
      strip_stuff = re.compile("[A-Z]/" +
              args.package_name + "\(([ 0-9]*)\): (.*)")

      pid = 0
      while(True):
        line = p.stdout.readline()
        original_line = line
        if not line:
          break
        m = strip_stuff.search(line)
        aborted = False
        if not m:
          continue
        line = m.group(2)
        new_pid = m.group(1)
        m = startre.search(line)
        if m:
          pid = new_pid
          break

      info("Process started with pid", pid)
      _,data_dir = run_p_output([adb, 'shell', 'run-as',
               args.package_name, '/system/bin/sh', '-c', 'pwd'], strip=True)

      info("Package install directory is %s" % data_dir)
      server = run_p_background([adb, 'shell', 'run-as', args.package_name,
               gdb_server_location, '+debug-socket',
               '--attach', pid])

      info("Started gdbserver")

      run_p([adb, 'forward', 'tcp:%s' % debug_port,
             'localfilesystem:%s/debug-socket' % data_dir])

      info("Forwarded socket %s for debugging" % debug_port)
      default_args = ['-x', gdb_setup_path,
                  '-e', os.path.join(obj_path, app_process_name),
                  '-ex', '"target remote:%s"' % debug_port]

      info("------------------------\n\n")
      info("To force the program to pause run the following command")
      info("%s shell run-as %s kill -5 %s" % (adb, args.package_name, pid))
      info("\n\n------------------------")

      gdb_command = Template(args.gdb_command)
      gdb_command = gdb_command.substitute(gdb=args.gdb, args=' '.join(default_args))
      gdb_command = re.findall(r'(?:[^\s,"]|"(?:\\.|[^"])*")+', gdb_command)
      info("Running gdb with args ", ' '.join(gdb_command))
      run_p(gdb_command, working_dir=obj_path)
      server.kill()
    else:
        run_p([adb, "shell", "rm", "/sdcard/stdout.txt"])
        run_p([adb, "shell", "rm", "/sdcard/wait-for-debugger.txt"])
        run_p([ adb, "shell", "am", "start", args.package_name + "/." +
            args.activity_name])

        p = subprocess.Popen([
            adb, "logcat", "-s", args.package_name + ":V",
            "*:F"],
            stdout=subprocess.PIPE,
            bufsize=1)

        returnre = re.compile("^RETURN ([0-9]*)..?$", re.MULTILINE)
        startre = re.compile("^--STARTED..?$")
        finishre = re.compile("^--FINISHED..?$")
        crashedre = re.compile("^--CRASHED--..?$")
        abortRE = ""
        strip_stuff = re.compile("[A-Z]/" +
                args.package_name + "\(([ 0-9]*)\): (.*)")
        has_started = False
        pid = 0
        while(True):
          line = p.stdout.readline()
          original_line = line
          if not line:
            break
          m = strip_stuff.search(line)
          aborted = False
          if not m:
            if abortRE:
              m = abortRE.search(original_line)
              if not m:
                print line
                continue
              aborted = True
            else:
                continue
          line = m.group(2)
          new_pid = m.group(1)
          if has_started and pid != m.group(1):
            # We have gotten a message that LOOKS like ours,
            # but is for a different pid (crash-test?)
            continue
          if not has_started:
            abortRE = re.compile(
	             "^F.*\((" + m.group(1) + ")\):(.*ABORTING.*)")
            m = startre.search(line)
            if m:
              has_started = True
              pid = new_pid
              continue

          match = returnre.search(line)
          crashed = aborted or crashedre.search(line)
          finish_match = finishre.search(line)
          if match:
            run_p([adb, "pull", "/sdcard/stdout.txt"])
            with open("stdout.txt", "r") as f:
              print f.read(),
            run_p([adb, "shell", "rm", "/sdcard/stdout.txt"])
            os.remove("stdout.txt")
            p.kill()
            sys.exit(int(match.group(1)))
          elif finish_match:
            continue
          elif crashed:
            run_p([adb, "pull", "/sdcard/crash"])
            run_p([adb, "shell", "am", "force-stop", args.package_name])
            run_p([adb, "shell", "rm", "/sdcard/crash"])
            run_p([adb, "pull", "/sdcard/stdout.txt"])
            with open("stdout.txt", "r") as f:
              print f.read(),
            run_p([adb, "shell", "rm", "/sdcard/stdout.txt"])
            os.remove("stdout.txt")
            print "CRASHED"
            time.sleep(1)
            run_p([adb, "shell", "am", "force-stop", args.package_name])
            name = "crash.dmp"
            if os.path.exists(name):
              i = 0
              while os.path.exists("crash-" + str(i) + ".dmp"):
                i += 1
              name = "crash-"+str(i)+".dmp"
            shutil.move("crash", name)
            p.kill()
            sys.exit(-1)
          else:
            print "===="
            print line
            print "===="
        p.kill()
        sys.exit(0)

  def uninstall(self):
    parser = argparse.ArgumentParser(
            prog="android_runner.py uninstall",
            description="Run android program")
    parser.add_argument("package_name", type=str,
            help="Package name to start")
    parser.add_argument("--sdk", help="Location of the sdk", required=False)
    args = parser.parse_args(sys.argv[2:])
    if args.sdk:
        adb = os.path.join(args.sdk, "platform-tools", "adb")
    sys.exit(run_p_silent([adb, "shell", "pm", "uninstall",
      args.package_name]))

  def install(self):
    parser = argparse.ArgumentParser(
            prog="android_runner.py install",
            description="Run android program")
    parser.add_argument("apk", type=str, help="path/to/apk")
    parser.add_argument("--sdk", help="Location of the sdk", required=False)
    args = parser.parse_args(sys.argv[2:])
    adb = "adb"
    if args.sdk:
        adb = os.path.join(args.sdk, "platform-tools", "adb")
    sys.exit(run_p([adb, "install", "-r", args.apk]))

  def main(self):
    parser = argparse.ArgumentParser(
               description="Run and install android .apks",
               usage="""android_runner.py <command> [args]
   
    This program is not meant to be run by itself
    it is meant to be run by scritps derived from
    quick_run.py.in
    
  Commands:
      install:    Install the .apk file on the device.
      run:        Run the given android activity and record the output.
      uninstall:  Remove the installed program.
  """)
    parser.add_argument('command', help='Command to run',
            choices=['install', 'run', 'uninstall'])
    args = parser.parse_args(sys.argv[1:2])
    getattr(self, args.command)()

if __name__ == "__main__":
  Runner().main()
