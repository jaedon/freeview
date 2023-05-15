# (c) 1997-2015 Netflix, Inc.  All content herein is protected by
# U.S. copyright and other applicable intellectual property laws and
# may not be copied without the express permission of Netflix, Inc.,
# which reserves all rights.  Reuse of any of this content for any
# purpose without the permission of Netflix, Inc. is strictly
# prohibited.

cmake_minimum_required(VERSION 2.8)
include(CheckIncludeFile)


# ==============================================================================
project(nrd)

set(NRDP_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
get_filename_component(NRDP_ROOT_DIR "${NRDP_ROOT_DIR}" ABSOLUTE)
if (NOT NRDP_BINARY_ROOT_DIR)
    set(NRDP_BINARY_ROOT_DIR "${CMAKE_CURRENT_BINARY_DIR}")
    get_filename_component(NRDP_BINARY_ROOT_DIR "${NRDP_BINARY_ROOT_DIR}" ABSOLUTE)
endif ()
set(NRD_EXPORT_NAME "nrd")

# Crash reporting options, process them earlier before including nrdp.cmake
# --------------------------------------------------------------------------------------------------------------
set(NRDP_CRASH_REPORTING "internal" CACHE STRING "[-crash-reporting] Enable crash reporting: breakpad, internal, off")
string(TOLOWER "${NRDP_CRASH_REPORTING}" NRDP_CRASH_REPORTING)
if(NRDP_CRASH_REPORTING STREQUAL "breakpad")
    set(NRDP_CRASH_REPORTING_ENABLED ON)
    set(NRDP_CRASH_REPORTING_BREAKPAD TRUE)
elseif(NRDP_CRASH_REPORTING STREQUAL "internal")
    if(APPLE)
        set(NRDP_CRASH_REPORTING_ENABLED OFF)
        set(NRDP_CRASH_REPORTING_INTERNAL FALSE)
    else()
        set(NRDP_CRASH_REPORTING_ENABLED ON)
        set(NRDP_CRASH_REPORTING_INTERNAL TRUE)
    endif()
elseif(NRDP_CRASH_REPORTING STREQUAL "off")
    set(NRDP_CRASH_REPORTING_ENABLED OFF)
else()
    message(FATAL_ERROR "Unknown crash reporting setting '${NRDP_CRASH_REPORTING}'")
endif ()
# --------------------------------------------------------------------------------------------------------------
# 3rdparty
# -------------------------------------------------
set(NRDP_3RDPARTY_DIRECTORY "${NRDP_ROOT_DIR}/3rdparty/")

# Find required NRDP
# --------------------------------------------------------------------------------------------------------------
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include("nrdp")
include("Documentation")

# ==============================================================================

# Dependencies
# ------------------------------------------------------------------------------------------
find_program(DPKG_QUERY dpkg-query)
if ((EXISTS ${CMAKE_SOURCE_DIR}/dependencies) AND DPKG_QUERY)
    message("Checking toolchain dependencies:")
    execute_process(
        COMMAND ${CMAKE_COMMAND} -DFILENAME=${CMAKE_SOURCE_DIR}/dependencies -DCXX="${CMAKE_CXX_COMPILER}" -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/dependencies.cmake"
        RESULT_VARIABLE res_var
    )

    if (NOT "${res_var}" STREQUAL "0")
        message(FATAL_ERROR "Required dependencies not met")
    endif ()

    add_custom_command(OUTPUT "${CMAKE_BINARY_DIR}/dependencies.dep"
        DEPENDS "${CMAKE_SOURCE_DIR}/dependencies"
        COMMAND ${CMAKE_COMMAND} -DFILENAME="${CMAKE_SOURCE_DIR}/dependencies" -DCXX="${CMAKE_CXX_COMPILER}" -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/dependencies.cmake"
        COMMENT "Checking dependencies"
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )
    add_custom_target(check_dependencies ALL DEPENDS "${CMAKE_BINARY_DIR}/dependencies.dep")
    set_source_files_properties("${CMAKE_BINARY_DIR}/dependencies.dep" PROPERTIES GENERATED TRUE)
    message("")
endif ()

# ==============================================================================

# CTest infrastructure
# --------------------------------------------------------------------------------------------------------------
set(MEMORYCHECK_COMMAND "/usr/local/netflix/tests/runtest_valgrind.pl" CACHE FILEPATH "" FORCE)
#set(MEMORYCHECK_COMMAND_OPTIONS "--vgdb=yes --vgdb-error=0")
get_environment_variable(COVERAGE_COMMAND "Path to coverage generation tool for ctest" "")

include(CTest)
enable_testing()
# --------------------------------------------------------------------------------------------------------------

# Netflix options
set(GROUP_NRDP "NRDApp options")
add_component_option(NRDP_BENCHMARKS "Build benchmark applications" OFF nrd)
set(NRDP_TOOLS "none" CACHE STRING "[LIST] Sets the tools to build (use all to build all available tools)")
set(NRDP_TESTS "none" CACHE STRING "[LIST] Sets the tests to build (use all to build all available tests)")

add_component_option(NRDP_SAMPLES "Build NrdApp sample applications" OFF nrd)
if (NRDP_SAMPLES)
    set(NRDP_SAMPLES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/samples)
endif ()

## ---------------------------------------------------------------
include_directories(${NRDP_BINARY_ROOT_DIR}/include)
set(LIBNRDP_DIRECTORY "${NRDP_ROOT_DIR}/src/nrd")

# Common variables
set(PLATFORM_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/src/platform")
set(COMMON_DIRECTORY "${PLATFORM_DIRECTORY}/common")

add_subdirectory(src)
add_subdirectory(${NRDP_3RDPARTY_DIRECTORY}/mongoose)

if (NOT NRDP_TOOLS STREQUAL "none")
    add_optional_subdirectory(tools)
endif ()

if (NOT NRDP_TESTS STREQUAL "none")
    add_optional_subdirectory(tests)
endif ()

if (NRDP_BENCHMARKS)
    add_optional_subdirectory(benchmark)
endif ()

## ---------------------------------------------------------------
message(" ")
message("=============================================================")
message("NrdApp ${NRDP_VERSION_MAJOR}.${NRDP_VERSION_MINOR}.${NRDP_VERSION_PATCH}")
message("-------------------------------------------------------------")
print_component_options(nrd "  - ")

message("=============================================================")
message(" ")
add_component(NAME NrdApp VERSION "${NRDP_VERSION_MAJOR}.${NRDP_VERSION_MINOR}.${NRDP_VERSION_PATCH}" LOCATION ${CMAKE_CURRENT_SOURCE_DIR})

message(" ")
message("Test Configuration")
message("-------------------------------------------------------------")
message(" ")
message("   - MEMORYCHECK_COMMAND=${MEMORYCHECK_COMMAND}")
message("   - COVERAGE_COMMAND=${COVERAGE_COMMAND}")

if (BUILD_NRDSYSTEM MATCHES "linux")
    if (NOT CMAKE_COMPILER_IS_GNUCC)
        message("")
        message("----------------------------------------------------------------------------")
        message("WARNING: Your C compiler has not been detected as GNU compliant.")
        message("The standard set of C flags won't be used")
        message("----------------------------------------------------------------------------")
        message("")
    endif ()

    if (NOT CMAKE_COMPILER_IS_GNUCXX)
        message("")
        message("----------------------------------------------------------------------------")
        message("WARNING: Your C++ compiler has not been detected as GNU compliant.")
        message("The standard set of C++ flags won't be used")
        message("----------------------------------------------------------------------------")
        message("")
    endif ()
endif ()

