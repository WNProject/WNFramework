import os
import subprocess
import sys

os.environ['ANDROID_TOOLCHAIN_FILE']=sys.argv[1]
subprocess.check_call(sys.argv[2:],env=os.environ)