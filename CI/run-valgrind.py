#!/usr/bin/env python

import subprocess
import sys
import re

VALGRIND_ARGUMENTS = [
  'valgrind',
  '--error-exitcode=1',
  '--leak-check=full',
  '--smc-check=all',
  '--track-origins=yes'
]

# Compute the command line.
command = VALGRIND_ARGUMENTS + sys.argv[1:]

# Run valgrind.
process = subprocess.Popen(command, stderr=subprocess.PIPE)
code = process.wait()
errors = process.stderr.readlines()

# If valgrind produced an error, we report that to the user.
if code != 0:
  sys.stderr.writelines(errors)
  sys.exit(code)

if "0 errors from 0 contexts" in str(errors):
  sys.exit(0)

sys.stderr.writelines(errors)
sys.exit(1)
