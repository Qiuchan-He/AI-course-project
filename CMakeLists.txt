# Usage:
#         cmake -S src -B builds/release
#         cmake --build builds/release
# The first call caches settings in the build directory and reads
# them from the cache on subsequent builds. If you want to change the
# settings of some options, do _not_ change the CMakeLists.txt files.
# Instead, create a new build directory, pass -DMY_OPTION=my_value to
# cmake. Alternatively, you can use a cmake GUI like ccmake to edit
# the cache.
#
# Two build targets are defined:
#
# * release (default)
#      -O3 optimisation, debugging symbols, assertions inactive
# * debug
#      -O3 optimisation, full debugging information, assertions active
#
# In all build targets, we overwrite the default configuration to
# include "-g", allow cross compilation and switch to pedantic error
# reporting.
#
# You can change the build target, by adding the parameter
#   -DCMAKE_BUILD_TYPE=type
# to the cmake call.

cmake_minimum_required(VERSION 3.22)

enable_testing()

project(ai23-project-software
    DESCRIPTION "Project software for the AI 23/24 course."
    LANGUAGES   CXX)

# Path containing custom CMake modules
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(common_cxx_flags)
include(macros)
include(options)

report_bitwidth()
set_up_build_types("Debug;Release")
set_up_options()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_BINARY_DIR}/lib)

if (WIN32)
    set(CMAKE_PDB_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib)
    set(CMAKE_PDB_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_BINARY_DIR}/lib)
    set(CMAKE_PDB_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_BINARY_DIR}/lib)
endif()

# Add planner components.

# Copy the translator into the output directory.
add_custom_target(translate ALL)
add_custom_command(TARGET translate POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/src/translate
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/translate
    COMMENT "Copying translator module into output directory")

set(FETCHCONTENT_UPDATES_DISCONNECTED TRUE)

if (DEFINED ENV{PATH_TORCH})
    if (NOT EXISTS $ENV{PATH_TORCH})
        message(FATAL_ERROR "Environment variable PATH_TORCH set, but points to a location that does not exist: $ENV{PATH_TORCH}")
    endif()

    message(STATUS "Environment variable PATH_TORCH set: $ENV{PATH_TORCH}. Using local installation.")
    set(Torch_ROOT "$ENV{PATH_TORCH}")
else()
    message(
        STATUS
        "Environment variable PATH_TORCH not set. Downloading Torch.")

    include(FetchContent)
    set(FETCHCONTENT_QUIET FALSE)

    string(TOLOWER ${CMAKE_SYSTEM_NAME} SYS_NAME)

    if (${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.24")
        cmake_policy(SET CMP0135 NEW)
    endif()

    if (${SYS_NAME} STREQUAL "darwin")
        if (${CMAKE_HOST_SYSTEM_PROCESSOR} STREQUAL "arm64")
            set(URL "https://fai.cs.uni-saarland.de/ai23/libtorch-darwin-arm64.zip")
            set(HASH 984eb0785f91e81f75bdad3b5638d993)
        else()
            set(URL "https://fai.cs.uni-saarland.de/ai23/libtorch-darwin.zip")
            set(HASH 880ef6223aadae9496081d1ce92eb560)
        endif()
    elseif (${SYS_NAME} STREQUAL "windows")
        set(URL "https://fai.cs.uni-saarland.de/ai23/libtorch-windows.zip")
        set(HASH c005bb13c3002b84cc3ed89209c687ba)
    elseif (${SYS_NAME} STREQUAL "linux")
        set(URL "https://fai.cs.uni-saarland.de/ai23/libtorch-linux.zip")
        set(HASH f0110bcbb0000abe1d203d080e3b0cda)
    else()
        message(FATAL_ERROR "Unsupported system.")
    endif()

    FetchContent_Declare(
        libtorch
        URL    ${URL}
        URL_HASH MD5=${HASH}
        DOWNLOAD_NO_PROGRESS False
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
        INSTALL_COMMAND ""
    )

    FetchContent_MakeAvailable(libtorch)

    set(Torch_ROOT "${libtorch_SOURCE_DIR}")
    message(STATUS "Torch directory: ${Torch_ROOT}")
endif()

find_package(Torch REQUIRED)

if (WIN32)
    if (DEFINED ENV{PATH_TORCH_DEBUG})
        if (NOT EXISTS $ENV{PATH_TORCH_DEBUG})
            message(FATAL_ERROR "Environment variable PATH_TORCH_DEBUG set, but points to a location that does not exist: $ENV{PATH_TORCH_DEBUG}")
        endif()

        message(STATUS "Environment variable PATH_TORCH_DEBUG set: $ENV{PATH_TORCH_DEBUG}. Using local installation.")
        set(Torchd_ROOT "$ENV{PATH_TORCH_DEBUG}")
    else()
        message(
            STATUS
            "Environment variable PATH_TORCH_DEBUG not set. Downloading Torch.")

        include(FetchContent)
        set(FETCHCONTENT_QUIET FALSE)

        if (${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.24")
            cmake_policy(SET CMP0135 NEW)
        endif()

        FetchContent_Declare(
            libtorchd
            URL "https://fai.cs.uni-saarland.de/ai23/libtorch-windows-debug.zip"
            URL_HASH MD5=a8630c2bc8fd3bd14383418f18477406
            DOWNLOAD_NO_PROGRESS False
            CONFIGURE_COMMAND ""
            BUILD_COMMAND ""
            INSTALL_COMMAND ""
        )

        FetchContent_MakeAvailable(libtorchd)

        set(Torchd_ROOT "${libtorchd_SOURCE_DIR}")
        message(STATUS "Torchd directory: ${Torchd_ROOT}")
    endif()

    find_package(Torchd REQUIRED)
else()
    add_library(torchd ALIAS torch)
endif()

# Add components as a subproject.
add_subdirectory(src)
