#!/usr/bin/env python

import argparse
import os
import os.path
import re
import shutil
import subprocess
import sys
import time

def run_p(args):
    subprocess.check_call(args)

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
        parser.add_argument("--sdk",
                help="Location of the sdk", required=False)
        args = parser.parse_args(sys.argv[2:])
        adb = "adb"
        if args.sdk:
            args.sdk = os.path.expanduser(args.sdk)
            adb = os.path.join(args.sdk, "platform-tools", "adb")

        run_p([adb, "logcat", "-c"])
        run_p([adb, "shell", "am", "force-stop", args.package_name])

        if args.debug:
            run_p([adb, "shell", "touch", "/sdcard/wait-for-debugger.txt"])
            ndk_gdb = [sys.executable, "ndk-gdb.py"]
            if args.ndk_dir:
                args.ndk_dir = os.path.expanduser(args.ndk_dir)
                ndk_gdb = [sys.executable, os.path.join(args.ndk_dir,
                                                        "ndk-gdb.py")]
            ndk_gdb.extend(["--start", "--force", "--nowait"]);
            run_dir = os.getcwd()
            if args.apk_build_dir:
                args.apk_build_dir = os.path.expanduser(args.apk_build_dir)
                run_dir = args.apk_build_dir
            p = subprocess.Popen(ndk_gdb,
                    cwd=run_dir)
            p.wait()

        else:
            run_p([adb, "shell", "rm", "/sdcard/wait-for-debugger.txt"])
            run_p([ adb, "shell", "am", "start", args.package_name + "/." +
                args.activity_name])

            p = subprocess.Popen([
                adb, "logcat", "-s", args.package_name + ":V"],
                stdout=subprocess.PIPE,
                bufsize=1)

            returnre = re.compile("^RETURN ([0-9]*).$", re.MULTILINE)
            startre = re.compile("^--STARTED.$")
            finishre = re.compile("^--FINISHED.$")
            crashedre = re.compile("^--CRASHED.$")
            strip_stuff = re.compile("[A-Z]/" +
                    args.package_name + "\([ 0-9]*\): (.*)")
            has_started = False
            while(True):
                line = p.stdout.readline()
                if not line:
                    break
                m = strip_stuff.search(line)
                if not m:
                    print line
                    continue
                line = m.group(1)
                if not has_started:
                    m = startre.search(line)
                    if m:
                        has_started = True
                        continue
                match = returnre.search(line)
                crashed = crashedre.search(line)
                finish_match = finishre.search(line)
                if match:
                    sys.exit(int(match.group(1)))
                elif finish_match:
                    continue
                elif crashed:
                    run_p([adb, "pull", "/sdcard/crash"])
                    run_p([adb, "shell", "am", "force-stop", args.package_name])
                    run_p([adb, "shell", "rm", "/sdcard/crash"])
                    run_p([adb, "shell", "pull", "/sdcard/stdout.txt"])
                    with open(stdout.txt, "r") as f:
                        print f.read(),
                    run_p([adb, "shell", "rm", "/sdcard/stdout.txt"])
                    print "CRASHED"
                    time.sleep(1)
                    run_p([adb, "shell", "am", "force-stop", args.package_name])
                    name = "crash.dmp"
                    if os.path.exists(name):
                        i = 0
                        while os.path.exists("crash-" + str(i) + ".dmp"):
                            i += 1
                        name = "crash-"+str(i)+".dmp"
                    shutil.move(crash, name)
                    exit(-1)
                else:
                    print line
            pass

    def install(self):
        parser = argparse.ArgumentParser(
                prog="android_runner.py install",
                description="Run android program")
        parser.add_argument("apk", type=str, help="path/to/apk")
        parser.add_argument("--sdk", help="Location of the sdk", required=False)
        args = parser.parse_args(sys.argv[2:])
        adb = "adb"
        if args.sdk:
            adb = os.path.join(parse.sdk, "platform-tools", "adb")
        run_p([adb, "install", "-r", args.apk])

        pass

    def main(self):
         parser = argparse.ArgumentParser(
                    description="Run and install android .apks",
                    usage="""android_runner.py <command> [args]

    Commands:
        install:    Install the .apk file on the device.
        run:        Run the given android activity and record the output.
    """)
         parser.add_argument('command', help='Command to run',
                 choices=['install', 'run'])
         args = parser.parse_args(sys.argv[1:2])
         getattr(self, args.command)()

if __name__ == "__main__":
    Runner().main()
