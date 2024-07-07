#!/usr/bin/env python3

import errno
import glob
import multiprocessing
import os
import subprocess
import sys

CONFIGS = {}
script_dir = os.path.dirname(__file__)
for config_file in sorted(
        glob.glob(os.path.join(script_dir, "*build_configs.py"))):
    with open(config_file) as f:
        config_file_content = f.read()
        exec(config_file_content, globals(), CONFIGS)

DEFAULT_CONFIG_NAME = CONFIGS.pop("DEFAULT")
DEBUG_CONFIG_NAME = CONFIGS.pop("DEBUG")
GTEST_COLOR = CONFIGS.pop("GTEST_COLOR")

# Setup up build environment
ENVIRONMENT = dict(os.environ)

ENVIRONMENT['GTEST_COLOR'] = GTEST_COLOR

# Additional configuration for MacOS. XCode's clang does not fully support
# C++ 20. We therefore assume that LLVM has been installed via Homebrew
# (brew install llvm) and use LLVM's clang and libc++, which support C++ 20.
if sys.platform == "darwin":
    try:
        subprocess.check_output("brew list llvm",
                                shell=True,
                                stderr=subprocess.STDOUT)
    except subprocess.CalledProcessError as e:
        print("Could not find LLVM installation. Make sure that LLVM is installed by running 'brew install llvm'.")
        sys.exit(1)

    # Get LLVM installation directory
    LLVM_BASE = subprocess.check_output(["brew", "--prefix", "llvm"],
                                              text=True).strip("\n")

    # Adjust the C and CXX compiler paths
    ENVIRONMENT['CC'] = os.path.join(LLVM_BASE, "bin", "clang")
    ENVIRONMENT['CXX'] = os.path.join(LLVM_BASE, "bin", "clang++")

CMAKE = "cmake"
CMAKE_GENERATOR = None
TOOLSET = None
ARCH = None
if os.name == "posix":
    CMAKE_GENERATOR = "Unix Makefiles"
elif os.name == "nt":
    CMAKE_GENERATOR = "Visual Studio 17 2022"
    TOOLSET = "host=x64"
    ARCH = "x64"

try:
    # Number of usable CPUs (Unix only)
    NUM_CPUS = len(os.sched_getaffinity(0))
except AttributeError:
    # Number of available CPUs as a fall-back (may be None)
    NUM_CPUS = os.cpu_count()


def print_usage():
    script_name = os.path.basename(__file__)
    configs = []
    for name, args in sorted(CONFIGS.items()):
        if name == DEFAULT_CONFIG_NAME:
            name += " (default)"
        if name == DEBUG_CONFIG_NAME:
            name += " (default with --debug)"
        configs.append(name + "\n    " + " ".join(args))
    configs_string = "\n  ".join(configs)
    cmake_name = os.path.basename(CMAKE)
    generator_name = CMAKE_GENERATOR.lower()
    default_config_name = DEFAULT_CONFIG_NAME
    debug_config_name = DEBUG_CONFIG_NAME
    print(
        f"""Usage: {script_name} [BUILD [BUILD ...]] [--all] [--debug] [MAKE_OPTIONS]

Build one or more predefined build configurations of Fast Downward. Each build
uses {cmake_name} to compile the code using {generator_name}. Build configurations
differ in the parameters they pass to {cmake_name}. By default, the build uses all
available cores if this number can be determined. Use the "-j" option for
{cmake_name} to override this default behaviour.

Build configurations
  {configs_string}

--all         Alias to build all build configurations.
--debug       Alias to build the default debug build configuration.
--help        Print this message and exit.

Make options
  All other parameters are forwarded to the build step.

Example usage:
  ./{script_name}                     # build {default_config_name} in #cores threads
  ./{script_name} -j4                 # build {default_config_name} in 4 threads
  ./{script_name} debug               # build debug
  ./{script_name} --debug             # build {debug_config_name}
  ./{script_name} release debug       # build release and debug configs
  ./{script_name} --all VERBOSE=true  # build all build configs with detailed logs
""")


def get_project_root_path():
    import __main__
    return os.path.dirname(__main__.__file__)


def get_builds_path():
    return os.path.join(get_project_root_path(), "builds")


def get_src_path():
    return os.path.join(get_project_root_path(), "src")


def get_build_path(config_name):
    return os.path.join(get_builds_path(), config_name)


def try_run(cmd, env):
    print(f'Executing command "{" ".join(cmd)}"')
    try:
        subprocess.check_call(cmd, env=env)
    except OSError as exc:
        if exc.errno == errno.ENOENT:
            print(
                f"Could not find '{cmd[0]}' on your PATH. For installation instructions, "
                "see https://www.fast-downward.org/ObtainingAndRunningFastDownward."
            )
            sys.exit(1)
        else:
            raise


def build(config_name, configure_parameters, build_parameters):
    print(f"Building configuration {config_name}.")
    build_path = get_build_path(config_name)
    generator_cmd = [CMAKE, "-S", get_project_root_path(), "-B", build_path]
    if CMAKE_GENERATOR:
        generator_cmd += ["-G", CMAKE_GENERATOR]
    if TOOLSET:
        generator_cmd += ["-T", TOOLSET]
    if ARCH:
        generator_cmd += ["-A", ARCH]
    generator_cmd += configure_parameters
    try_run(generator_cmd, env=ENVIRONMENT)

    build_cmd = [CMAKE, "--build", build_path]
    if NUM_CPUS:
        build_cmd += ["-j", f"{NUM_CPUS}"]
    if build_parameters:
        build_cmd += ["--"] + build_parameters
    try_run(build_cmd, env=ENVIRONMENT)

    print(f"Built configuration {config_name} successfully.")


def main():
    config_names = []
    build_parameters = []
    for arg in sys.argv[1:]:
        if arg == "--help" or arg == "-h":
            print_usage()
            sys.exit(0)
        elif arg == "--debug":
            config_names.append(DEBUG_CONFIG_NAME)
        elif arg == "--all":
            config_names.extend(sorted(CONFIGS.keys()))
        elif arg in CONFIGS:
            config_names.append(arg)
        else:
            build_parameters.append(arg)
    if not config_names:
        config_names.append(DEFAULT_CONFIG_NAME)
    for config_name in config_names:
        build(config_name, CONFIGS[config_name], build_parameters)


if __name__ == "__main__":
    main()