########################################################################
# CMake build script for Google Mock.
#
# To run the tests for Google Mock itself on Linux, use 'make test' or
# ctest.  You can select which tests to run using 'ctest -R regex'.
# For more options, run 'ctest --help'.

# BUILD_SHARED_LIBS is a standard CMake variable, but we declare it here to
# make it prominent in the GUI.
option(BUILD_SHARED_LIBS "Build shared libraries (DLLs)." OFF)

option(gmock_build_tests "Build all of Google Mock's own tests." OFF)

# A directory to find Google Test sources.
if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/gtest/CMakeLists.txt")
  set(gtest_dir gtest)
else()
  set(gtest_dir ../gtest)
endif()

# Defines pre_project_set_up_hermetic_build() and set_up_hermetic_build().
include("${gtest_dir}/cmake/hermetic_build.cmake" OPTIONAL)

if (COMMAND pre_project_set_up_hermetic_build)
  # Google Test also calls hermetic setup functions from add_subdirectory,
  # although its changes will not affect things at the current scope.
  pre_project_set_up_hermetic_build()
endif()

########################################################################
#
# Project-wide settings

# Name of the project.
#
# CMake files in this project can refer to the root source directory
# as ${gmock_SOURCE_DIR} and to the root binary directory as
# ${gmock_BINARY_DIR}.
# Language "C" is required for find_package(Threads).
project(gmock CXX C)
cmake_minimum_required(VERSION 2.6.2)

if (COMMAND set_up_hermetic_build)
  set_up_hermetic_build()
endif()

# Instructs CMake to process Google Test's CMakeLists.txt and add its
# targets to the current scope.  We are placing Google Test's binary
# directory in a subdirectory of our own as VC compilation may break
