#!/usr/bin/env python

import argparse
from threading import Timer
import os
import os.path
import platform
import re
import shutil
import signal
import subprocess
import sys
import time
from string import Template


def parse_new_output(package_name, line):
    splits = line.split()
    if (len(splits) >= 6):
        if (not splits[2].isdigit()):
            return None
        if (splits[5] != package_name + ":"):
            return None
    else:
        return None
    # We have to remove the first 5 things from the input,
    # but keep the spaces in the output. (So we cannot use join)
    newline = line
    for x in range(0, 6):
        newline = newline[len(splits[x]):]
        newline = newline.lstrip()
    return (splits[2], newline.rstrip())


def kill_proc(proc, timeout):
    timeout["value"] = True
    proc.kill()


def run_p(args, working_dir=None):
    subprocess.check_call(args, cwd=working_dir)


def shell_p(args, working_dir=None):
    subprocess.check_call(args, cwd=working_dir, shell=True)


def run_p_silent(args):
    with open(os.devnull) as FNULL:
        subprocess.check_call(args, stdout=FNULL,
                              stderr=FNULL)


def run_p_background(args):
    file = open(os.devnull)
    print args
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
        ret_code, timeout = run_p_timeout(timeout_count, args)
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
    def run_adb_silent(self, args):
        a = [self.adb]
        a.extend(args)
        return run_p_silent(a)

    def run_shell_silent(self, args):
        a = [self.adb, 'shell']
        a.extend(args)
        return run_p_silent(a)

    def run_as_shell_silent(self, args):
        a = [self.adb, 'shell', 'run-as', self.args.package_name]
        a.extend(args)
        return run_p_silent(a)

    def push_file_to(self, source, dest):
        self.run_adb_silent(["push", source, '/data/local/tmp/tmpfile'])
        self.run_shell_silent(
            ["chmod", "777", '/data/local/tmp/tmpfile'])
        self.run_as_shell_silent(
            ['cp', '/data/local/tmp/tmpfile', dest])

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
                            help="Android ndk directory (for debugging)",
                            required=False)
        parser.add_argument("--build_dir", type=str,
                            help="Directory that the debug apk was built out of",
                            required=False)
        parser.add_argument('--target_arch', type=str, default='armeabi-v7a',
                            help="target architecture to debug on. Must be supplied if debugging",
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

        self.args = args
        self.adb = adb

        if args.debug:
            try:
                self.run_shell_silent(
                    ["touch", "/sdcard/wait-for-debugger.txt"])
            except:
                pass

            _, data_dir = run_p_output([adb, 'shell', 'run-as',
                                        args.package_name, '/system/bin/sh', '-c', 'pwd'], strip=True)

            info("Package install directory is %s" % data_dir)

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
                abis.extend(primary_abi.split(','))

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

            arch_to_dir = {
                'armeabi-v7a': 'armeabi',
                'armeabi': 'armeabi',
                'arm64-v8a': 'arm64-v8a',
                'x86': 'x86'
            }

            try:
                self.run_as_shell_silent(['killall', '-9', 'lldb-server'])
            except:
                pass

            self.run_as_shell_silent(['mkdir', '-p', 'lldb/bin'])
            self.push_file_to(
                os.path.join(args.sdk, 'lldb', '3.1', 'android',
                             arch_to_dir[args.target_arch], 'lldb-server'), "lldb/bin/lldb-server")
            self.push_file_to(
                os.path.join(args.sdk, 'lldb', '3.1', 'android', 'start_lldb_server.sh'), "lldb/start_lldb_server.sh")

            p = subprocess.Popen([
                adb, "logcat", "-v", "brief", "-s", args.package_name + ":V",
                "*:F"],
                stdout=subprocess.PIPE,
                bufsize=1)

            self.run_shell_silent(["am", "start", "-S", "%s/%s" % (args.package_name,
                                                                   args.activity_name)])
            info("Starting %s/.%s" % (args.package_name, args.activity_name))

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
            server = run_p_background([adb, 'shell', 'run-as', args.package_name,
                                       'lldb/start_lldb_server.sh', data_dir + '/lldb',
                                       'unix-abstract', data_dir, 'lldb',
                                       '"lldb process:gdb-remote packets"'])

            info("Started lldb_server")

            arch_to_local_dir = {
                'armeabi-v7a': 'armeabi-v7a',
                'armeabi': 'armeabi-v7a',
                'arm64-v8a': 'arm64-v8a',
                'x86': 'x86'
            }

            with open('lldb_args.txt', 'w') as f:
                f.writelines([
                    'platform select remote-android\n',
                    'platform connect unix-abstract-connect://{}/lldb\n'.format(
                        data_dir),
                    'settings set target.exec-search-paths {}\n'.format(os.path.join(
                        args.build_dir, arch_to_local_dir[args.target_arch], 'lib')),
                    'process attach --pid {}'.format(pid)
                ])

            args = []
            if platform.system() == 'Windows':
                args = ['start', '/WAIT']
            elif platform.system() == 'Linux':
                args = ['xterm', '-e']
            elif platform.system() == 'Darwin':
                args = ['xterm', '-e']
            args.extend(['lldb', '-s', 'lldb_args.txt'])
            if platform.system() == 'Windows':
                args.extend(['-X'])
            shell_p(args)
            server.kill()
            self.run_as_shell_silent(['killall', '-9', 'lldb-server'])
        else:
            try:
                run_p([adb, "shell", "rm", "/sdcard/wait-for-debugger.txt"])
            except:
                pass
            info("Starting %s/.%s" % (args.package_name, args.activity_name))
            run_p([adb, "shell", "am", "start", args.package_name + "/" +
                   args.activity_name])

            p = subprocess.Popen([
                adb, "logcat", "-s", args.package_name + ":V",
                "*:F"],
                stdout=subprocess.PIPE,
                bufsize=1)

            returnre = re.compile("^RETURN ([0-9]*)\s*$", re.MULTILINE)
            startre = re.compile("^--STARTED\s*$")
            finishre = re.compile("^--FINISHED\s*$")
            crashedre = re.compile("^--CRASHED--\s*$")
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
                m2 = parse_new_output(args.package_name, line)
                aborted = False
                if not m and not m2:
                    if abortRE:
                        m = abortRE.search(original_line)
                        if not m:
                            print line
                            continue
                        aborted = True
                    else:
                        continue
                if (m2):
                    line = m2[1]
                    new_pid = m2[0]
                else:
                    line = m.group(2)
                    new_pid = m.group(1)

                if has_started and pid != new_pid:
                    # We have gotten a message that LOOKS like ours,
                    # but is for a different pid (crash-test?)
                    continue
                if not has_started:
                    abortRE = re.compile(
                        "^F.*\((" + new_pid + ")\):(.*ABORTING.*)")
                    m = startre.search(line)
                    if m:
                        has_started = True
                        pid = new_pid
                        continue

                match = returnre.search(line)
                crashed = aborted or crashedre.search(line)
                finish_match = finishre.search(line)
                if match:
                    p.kill()
                    sys.exit(int(match.group(1)))
                elif finish_match:
                    continue
                elif crashed:
                    run_p([adb, "pull", "/sdcard/crash"])
                    run_p([adb, "shell", "am", "force-stop", args.package_name])
                    run_p([adb, "shell", "rm", "/sdcard/crash"])
                    print "CRASHED"
                    time.sleep(1)
                    run_p([adb, "shell", "am", "force-stop", args.package_name])
                    name = "crash.dmp"
                    if os.path.exists(name):
                        i = 0
                        while os.path.exists("crash-" + str(i) + ".dmp"):
                            i += 1
                        name = "crash-" + str(i) + ".dmp"
                    shutil.move("crash", name)
                    p.kill()
                    sys.exit(-1)
                else:
                    print line
            p.kill()
            sys.exit(0)

    def uninstall(self):
        parser = argparse.ArgumentParser(
            prog="android_runner.py uninstall",
            description="Run android program")
        parser.add_argument("package_name", type=str,
                            help="Package name to start")
        parser.add_argument(
            "--sdk", help="Location of the sdk", required=False)
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
        parser.add_argument(
            "--sdk", help="Location of the sdk", required=False)
        parser.add_argument(
            "--package_name", help="Name of the package", required=False)
        args = parser.parse_args(sys.argv[2:])
        adb = "adb"
        if args.sdk:
            adb = os.path.join(args.sdk, "platform-tools", "adb")
        try:
            run_p_silent([adb, "shell", "cmd", "package",
                          "uninstall", "-k", args.package_name])
        except:
            pass
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
