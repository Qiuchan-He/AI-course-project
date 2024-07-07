#!/usr/bin/env python3

import argparse
import os
import sys
import subprocess

if os.name == "nt":
    BINARY_EXTENSION = ".exe"
else:
    BINARY_EXTENSION = ""

parser = argparse.ArgumentParser(
    description="This script can be used to run the test from the command line.")

parser.add_argument(
    "-r",
    "--release",
    action="store_true",
    help="Runs the tests for the release build.")
parser.add_argument(
    "-f",
    "--filter",
    default="",
    help="Selects all tests that contain this string for listing/running.")
parser.add_argument(
    "-l",
    "--list",
    action="store_true",
    help="Lists the tests but does not run them.")
parser.add_argument(
    "-v",
    "--verbose",
    action="store_true",
    help="Enables verbose output, including output for succeeding tests.")

args = parser.parse_args()
    
build = 'release' if args.release else 'debug'
cwd = f"./builds/{build}"

if (not os.path.exists(f"{cwd}/bin/project_tests{BINARY_EXTENSION}")):
    print("ERROR: The test executable does not exist. Build the project first!")
    sys.exit(1)
    
command = ["ctest", "--output-on-failure", "-C", f"{build}"]
if (args.filter != ""):
    command.extend(["-R", args.filter])

if args.list:
    command.append("-N")
else:
    try:
        # Number of usable CPUs (Unix only)
        NUM_CPUS = len(os.sched_getaffinity(0))
    except AttributeError:
        # Number of available CPUs as a fall-back (may be None)
        NUM_CPUS = os.cpu_count()

    command.extend(["-j", f"{NUM_CPUS}"])
    
    if args.verbose:
        command.append("-V")

ENVIRONMENT = dict(os.environ)
ENVIRONMENT['GTEST_COLOR'] = 'yes'

# run ctest
try:
    print(f'Running command \'{" ".join(command)}\' in directory {cwd}')
    subprocess.run(command, env=ENVIRONMENT, cwd=cwd)
except subprocess.CalledProcessError as e:
    sys.exit(1)