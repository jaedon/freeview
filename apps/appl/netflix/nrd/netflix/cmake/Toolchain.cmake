# (c) 1997-2015 Netflix, Inc.  All content herein is protected by
# U.S. copyright and other applicable intellectual property laws and
# may not be copied without the express permission of Netflix, Inc.,
# which reserves all rights.  Reuse of any of this content for any
# purpose without the permission of Netflix, Inc. is strictly
# prohibited.

cmake_minimum_required(VERSION 2.8)
include("CMakeParseArguments")
include("CheckIncludeFile")
include("CheckCSourceCompiles")

set(GROUP_BUILD "Build options")


# Helper macros
# -------------------------------------------------

# Pretty print boolean variable
macro(PRETTY_PRINT message variable)
    if (${variable})
        message("${message}Yes")
    else()
        message("${message}No")
    endif()
endmacro()


# Extract the library paths from a LDFLAGS variable, so we can later add them as CMake library paths and have then count in CMake's RPATH calculations
macro(PARSE_LDFLAGS ldflags)
    cmake_parse_arguments(PLD "" "LIBRARIES;DIRECTORIES;FLAGS" "" ${ARGN})
    string(STRIP "${ldflags}" ldflags)

    # Directories (-L)
    if(PLD_DIRECTORIES)
        string(REGEX MATCHALL "[-][L]([a-zA-Z0-9/._-]*)" PLD_temp "${ldflags}")
        set(${PLD_DIRECTORIES} "")
        foreach(directory ${PLD_temp})
            string(REGEX REPLACE "^-L" "" directory "${directory}")
            list(APPEND ${PLD_DIRECTORIES} ${directory})
        endforeach()
    endif()

    # Libraries (-l)
    if(PLD_LIBRARIES)
        string(REGEX MATCHALL "[-][l]([a-zA-Z0-9/._-]*)" PLD_temp "${ldflags}")
        set(${PLD_LIBRARIES} "")
        foreach(library ${PLD_temp})
            string(REGEX REPLACE "^-l" "" library "${library}")
            list(APPEND ${PLD_LIBRARIES} ${library})
        endforeach()
    endif()

    # Other flags
    if(PLD_FLAGS)
        string(REGEX REPLACE "[-][L]([a-zA-Z0-9/._-]*)" "." PLD_temp "${ldflags}")
        string(REGEX REPLACE "[-][l]([a-zA-Z0-9/._-]*)" "." PLD_temp "${PLD_temp}")
        string(REGEX REPLACE "[.][ ]" "" ${PLD_FLAGS} ${PLD_temp})
        if("${${PLD_FLAGS}}" STREQUAL ".")
            set(${PLD_FLAGS} "" )
        endif()
    endif()
endmacro()

macro(PRINT_FLAG header title value)
    string(STRIP "${value}" tmp_value)
    message("${header}${title}=${tmp_value}")
endmacro()

# -------------------------------------------------


# Netflix global options
# -------------------------------------------------
option(BUILD_QA "Build QA specific routines in nrd-core/NBP bridge" ON)
option(BUILD_SYMBOLS "Build with debug symbols" ON)
option(BUILD_DEBUG "[-debug-code] Enable debug code (-DBUILD_DEBUG)" ON)
option(BUILD_PRODUCTION "[-production-code] Enable production code (-DBUILD_PRODUCTION)" OFF)
option(BUILD_COMPILE_RESOURCES "Whether to compile resources with the application or not" OFF)

if(BUILD_COMPILE_RESOURCES)
    add_definitions(-DNRDP_COMPILED_RESOURCES)
endif()

        if(BUILD_DEBUG)
            add_definitions(-DBUILD_DEBUG)
        endif()

        if(BUILD_PRODUCTION)
            add_definitions(-DBUILD_PRODUCTION)
        endif()

# -------------------------------------------------

# Use a global property to make sure we do this only once
get_property(toolchain_processed GLOBAL PROPERTY toolchain_processed SET)

if(NOT toolchain_processed)
    set_property(GLOBAL PROPERTY toolchain_processed TRUE)

    # Detect build type
    # -------------------------------------------------
    if(BUILD_NUMBER)
        set(HAS_BUILD_NUMBER ON)
    endif()
    if(BUILD_BRANCH)
        set(HAS_BUILD_BRANCH ON)
    endif()
    if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "[HIDDEN]Build mode: Release, Debug or Test" FORCE)
    endif()
    string(TOLOWER "${CMAKE_BUILD_TYPE}" CMAKE_BUILD_TYPE)
    string(TOUPPER "CMAKE_LINKER_FLAGS_${CMAKE_BUILD_TYPE}" LFLAGS_VAR)
    string(TOUPPER "CMAKE_C_FLAGS_${CMAKE_BUILD_TYPE}" CFLAGS_VAR)
    string(TOUPPER "CMAKE_CXX_FLAGS_${CMAKE_BUILD_TYPE}" CXXFLAGS_VAR)

    # Detect the system
    # -------------------------------------------------
    set(BUILD_NRDSYSTEM "auto" CACHE STRING "[-system] System type")
    if(BUILD_NRDSYSTEM MATCHES auto)
        set(BUILD_NRDSYSTEM "linux")
    endif()

    # Detect the system
    # -------------------------------------------------
        if(CMAKE_CONFIGURATION_TYPES)
            set(CMAKE_CONFIGURATION_TYPES Debug Release Test)
            set(CMAKE_CONFIGURATION_TYPES "${CMAKE_CONFIGURATION_TYPES}" CACHE STRING "[HIDDEN]Build configurations" FORCE)
        endif()

    # Include toolchain file
    # ---------------------------------------------------------------
    if(EXISTS "${NRDP_ROOT_DIR}/cmake/${BUILD_NRDSYSTEM}.cmake")
        include("${NRDP_ROOT_DIR}/cmake/${BUILD_NRDSYSTEM}.cmake")
    endif()

    set(NRDP_EMBEDDED_RESOURCES_USE_SIZE FALSE)

    if(BUILD_NRDSYSTEM STREQUAL "linux")
        if(NOT CMAKE_COMPILER_IS_GNUCC)
            message("")
            message("----------------------------------------------------------------------------")
            message("WARNING: Your C compiler has not been detected as GNU compliant.")
            message("The standard set of C flags won't be used")
            message("----------------------------------------------------------------------------")
            message("")
        endif()

        if(NOT CMAKE_COMPILER_IS_GNUCXX)
            message("")
            message("----------------------------------------------------------------------------")
            message("WARNING: Your C++ compiler has not been detected as GNU compliant.")
            message("The standard set of C++ flags won't be used")
            message("----------------------------------------------------------------------------")
            message("")
        endif()

        set(CMAKE_C_FLAGS_TEST "${CMAKE_C_FLAGS_TEST} ${BUILD_QA_FLAGS}")
        set(CMAKE_CXX_FLAGS_TEST "${CMAKE_CXX_FLAGS_TEST} ${BUILD_QA_FLAGS}")

        # Clear the "damage" made by the automatically included UnixPaths.cmake
        list(REMOVE_ITEM CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "/lib")
        list(REMOVE_ITEM CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "/usr/lib")
        list(REMOVE_ITEM CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "/usr/lib32")
        list(REMOVE_ITEM CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "/usr/lib64")
        list(REMOVE_ITEM CMAKE_C_IMPLICIT_INCLUDE_DIRECTORIES "/usr/include")
        list(REMOVE_ITEM CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES "/usr/include")

        # add some common system paths
        set(CMAKE_LIBRARY_PATH /lib/i386-linux-gnu /usr/lib/i386-linux-gnu /lib32 /usr/lib32)

        add_definitions(-DLINUX) #-D_FILE_OFFSET_BITS=64
        set(BUILD_INSTALL_TARGET OFF)

        # CMake removes the -L and -l switches from LDFLAGS before passing the variable to the linker. We need to get the library paths ourselves and use CMake commands to add them.
        parse_ldflags("${CMAKE_EXE_LINKER_FLAGS}" DIRECTORIES "LDFLAGS_DIRECTORIES")
        link_directories(${LDFLAGS_DIRECTORIES})

        # Command line to use to convert a file into an object to be embedded in the executable
        get_environment_variable(LD "LD executable" "")
        if (NOT "${LD}" STREQUAL "")
            set(CMAKE_RESOURCE_COMPILER "${LD}" CACHE FILEPATH "Location of the ld (for resource compilation)" FORCE)
        endif()
        set(CMAKE_RESOURCE_COMPILER_FLAGS -r -b binary -o <DESTINATION_O> <SOURCE>)

        get_environment_variable(AR "AR executable" "")
        if (NOT "${AR}" STREQUAL "")
            set(CMAKE_AR "${AR}" CACHE FILEPATH "Location of the ar utility" FORCE)
        endif()

        get_environment_variable(RANLIB "Ranlib executable" "")
        if (NOT "${RANLIB}" STREQUAL "")
            set(CMAKE_RANLIB "${RANLIB}" CACHE FILEPATH "Location of the ranlib utility" FORCE)
        endif()
    endif()

    if(IOS)
        add_definitions(-DNRDP_PLATFORM_IOS)
        link_directories(${TOOLCHAIN_DIRECTORY}/netflix/lib/)
        include_directories(${TOOLCHAIN_DIRECTORY}/netflix/include/)
        set(CMAKE_RESOURCE_COMPILER "${CMAKE_LINKER}")
        set(CMAKE_RESOURCE_COMPILER_FLAGS -r -b binary -o <DESTINATION_O> <SOURCE>)
    endif()

    # Options
    # ------------------------------------------------------------------------------------------
    set(GROUP_BUILD "Build options")

    # CMake internal variables
    option(BUILD_INSTALL_TARGET "[-install] Build installation targets" ON)
    option(BUILD_SHARED_LIBS "[-shared] Build shared libraries" OFF)
    set(CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}" CACHE STRING "[-prefix] Installation directory" FORCE)
    option(BUILD_SMALL "Create smaller binaries by optimizing and stripping their symbols" OFF)
    option(BUILD_GPROF "Compile using GPROF" OFF)
    option(BUILD_GPERFTOOLS "Compile using gperftools" OFF)
    option(BUILD_COVERAGE "Compile with code coverage information" OFF)
    option(BUILD_PROFILE "Compile for profling" OFF)
    option(BUILD_INFORMATION "Prints information about the build system when configuring" OFF)

    if(BUILD_GPERFTOOLS OR BUILD_GPROF)
        set(BUILD_PROFILE ON)
    endif()

    # Global options
    # -------------------------------------------------
    if(BUILD_QA)
        set(BUILD_QA_FLAGS "-DBUILD_QA")
    else()
        set(BUILD_QA_FLAGS "")
    endif()

    get_environment_variable(DEBUG_FLAGS "Debug flags" "")

    if(NOT DEBUG_FLAGS STREQUAL "")
        list(REMOVE_ITEM CMAKE_C_FLAGS_DEBUG "-g")
        list(REMOVE_ITEM CMAKE_CXX_FLAGS_DEBUG "-g")
        list(APPEND CMAKE_C_FLAGS_DEBUG ${DEBUG_FLAGS})
        list(APPEND CMAKE_CXX_FLAGS_DEBUG ${DEBUG_FLAGS})
    endif()

    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} ${BUILD_QA_FLAGS}")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${BUILD_QA_FLAGS}")
        #set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")
        #set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")


    if(BUILD_PROFILE)
        set(${CFLAGS_VAR} "${${CFLAGS_VAR}} -p")
        set(${CXXFLAGS_VAR} "${${CXXFLAGS_VAR}} -p")
    endif()

    if(BUILD_GPROF)
        set(${LFLAGS_VAR} "${${LFLAGS_VAR}} -pg")
        set(${CFLAGS_VAR} "${${CFLAGS_VAR}} -pg")
        set(${CXXFLAGS_VAR} "${${CXXFLAGS_VAR}} -pg")
    endif()

    if(BUILD_COVERAGE)
        set(${CFLAGS_VAR} "${${CFLAGS_VAR}} -g -O0 -fprofile-arcs -ftest-coverage")
        set(${CXXFLAGS_VAR} "${${CXXFLAGS_VAR}} -g -O0 -fprofile-arcs -ftest-coverage")
    endif()

    if(BUILD_SMALL)
        set(${CFLAGS_VAR} "${${CFLAGS_VAR}} ${SMALL_FLAGS} ${SMALL_CFLAGS}")
        set(${CXXFLAGS_VAR} "${${CXXFLAGS_VAR}} ${SMALL_FLAGS} ${SMALL_CXXFLAGS}")
    else()
        if(NOT SMALL_FLAGS)
            set(SMALL_FLAGS "") # Prevent unused variable warning
        endif()
    endif()

    # If crash reporting is enabled, force the addition of the debug flags
    if(NRDP_CRASH_REPORTING_ENABLED)
        if(DEBUG_FLAGS STREQUAL "" OR NOT DEFINED DEBUG_FLAGS)
            set(DEBUG_FLAGS "-g")
        endif()
        # Remove any optimization level the debug flags might introduce
        string(REGEX REPLACE "-[oO][0-9]" "" DEBUG_FLAGS "${DEBUG_FLAGS}")

        set(${CFLAGS_VAR} "${${CFLAGS_VAR}} ${DEBUG_FLAGS}")
        set(${CXXFLAGS_VAR} "${${CXXFLAGS_VAR}} ${DEBUG_FLAGS}")

        # Make sure gcc is not being invoked with -s to strip the final executable
        string(REPLACE "-s" "" ${CFLAGS_VAR} "${${CFLAGS_VAR}}")
        string(REPLACE "-s" "" ${CXXFLAGS_VAR} "${${CXXFLAGS_VAR}}")
    endif()

    # Platform checks
    # ------------------------------------------------------------------------------------------
    check_include_file("mcheck.h" HAVE_MCHECK_H)
    check_include_file("malloc.h" HAVE_MALLOC_H)
    check_include_file("sys/prctl.h" HAVE_PRCTL_H)
    if(HAVE_MALLOC_H)
        add_definitions(-DHAVE_MALLOC_H=1)
    endif()

    if(HAVE_MCHECK_H)
        add_definitions(-DHAVE_MCHECK_H=1)
    endif()

    if(HAVE_PRCTL_H)
        add_definitions(-DHAVE_PRCTL_H=1)
    endif()

    # Print some status information
    message(" ")
    message("=============================================================")
    message("Build system configuration")
    message("-------------------------------------------------------------")
    message("  Toolchain information:")
    message(" ")
    message("   - STRIP=${CMAKE_STRIP}")
    message("   - SMALL_FLAGS=${SMALL_FLAGS}")
    message("     + SMALL_CFLAGS=${SMALL_CFLAGS}")
    message("     + SMALL_CXXFLAGS=${SMALL_CXXFLAGS}")
    message("   - DEBUG_FLAGS=${DEBUG_FLAGS}")
    message("   - CFLAGS=${${CFLAGS_VAR}} (${CMAKE_C_FLAGS})")
    message("   - CXXFLAGS=${${CXXFLAGS_VAR}} (${CMAKE_CXX_FLAGS})")
    message("   - LFLAGS=${${LFLAGS_VAR}}")
    message("   - LDFLAGS=${CMAKE_EXE_LINKER_FLAGS}")
    foreach(DIRECTORY ${LDFLAGS_DIRECTORIES})
        message("     + ${DIRECTORY}")
    endforeach()
    message("-------------------------------------------------------------")
    message("  Platform checks:")
    message(" ")
    pretty_print("   - mcheck.h: " HAVE_MCHECK_H)
    pretty_print("   - malloc.h: " HAVE_MALLOC_H)
    pretty_print("   - sys/prctl.h: " HAVE_PRCTL_H)
    message(" ")
    message("-------------------------------------------------------------")
    message("  Build configuration:")
    message(" ")
    pretty_print("   - Debug symbols: " BUILD_SYMBOLS)
    pretty_print("   - Strip binary: " BUILD_SMALL)
    pretty_print("   - DEBUG code: " BUILD_DEBUG)
    pretty_print("   - PRODUCTION code: " BUILD_PRODUCTION)
    pretty_print("   - QA code: " BUILD_QA)
    message(" ")
    message("  Build options:")
    message(" ")
    pretty_print("   - Compiled resources: " BUILD_COMPILE_RESOURCES)
    pretty_print("   - gprof: " BUILD_GPROF)
    pretty_print("   - gperf: " BUILD_GPERFTOOLS)
    pretty_print("   - Code coverage (gcov): " BUILD_COVERAGE)
    pretty_print("   - Profiling: " PROFILE)
    message("   - Build type: ${CMAKE_BUILD_TYPE}")
    message("   - Installation prefix: ${CMAKE_INSTALL_PREFIX}")
    message(" ")

    if(BUILD_INFORMATION)
        message("-------------------------------------------------------------")
        message("Build system information:")
        message("-------------------------------------------------------------")
        include(CMakePrintSystemInformation)
    endif()

    message("=============================================================")
    message(" ")
endif()

