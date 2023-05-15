
# (c) 1997-2015 Netflix, Inc.  All content herein is protected by
# U.S. copyright and other applicable intellectual property laws and
# may not be copied without the express permission of Netflix, Inc.,
# which reserves all rights.  Reuse of any of this content for any
# purpose without the permission of Netflix, Inc. is strictly
# prohibited.

cmake_minimum_required(VERSION 2.8)

set(NRDDPI_DIRECTORY "${NRDP_BINARY_ROOT_DIR}/lib/dpi")

include_directories(${CMAKE_CURRENT_LIST_DIR})

add_custom_command(
    OUTPUT ${NRDDPI_DIRECTORY}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${NRDDPI_DIRECTORY}
)

set(GROUP_NRDP "NRD options")
# Options
# ------------------------------------------------------------------------------------------
set(GROUP_NRDP "libnrd options")
set(NRDP_ENDIAN "auto" CACHE STRING "[-endian] System endianness: auto, big, little, pdp")
set(NRDP_GPU_ENDIAN "auto" CACHE STRING "[-gpu-endian] GPU endiannes: auto (same as CPU), big, little, pdp")
set(NRDP_SCHEDULER_TYPE "other" CACHE STRING "[-scheduler] Thread scheduling policy: rr, fifo, other")
add_component_option(NRDP_MEMTEST "Build with debugging memory manager" OFF nrd)
add_component_option(NRDP_EXIT "Platform allows nrdp to exit" ON nrd)
add_component_option(NRDP_HAS_ESPLAYER "[-nrd-esplayer] Enable elementary stream player" ON nrd)
add_component_option(NRDP_MCSIM_DUMP "Enable input data dump for MediaControlSim" OFF nrd)
add_component_option(NRDP_HAS_IPV6 "[-nrd-ipv6] Enable IPv6 support" OFF nrd)
add_component_option(NRDP_HAS_TRACING "[-tracing] Force tracing" OFF nrd)
add_component_option(NRDP_HAS_OBJECTCOUNT "[-objectcount] Force object count" OFF nrd)
add_component_option(NRDP_HAS_PROFILING_LOG "[-nrd-profiling-log] Enable performance profiling logging" OFF nrd)
add_component_option(NRDP_HAS_PERFORMANCE_MARKERS "[-nrd-performance-markers] Enable performance profiling markers" OFF nrd)

add_component_option(NRDP_HAS_INSTRUMENTATION "[-nrd-instrumentation] Enable all instrumentation" ON nrd)
add_component_option(NRDP_HAS_ON_INSTRUMENTATION "[-nrd-on-instrumentation] Enable test instrumentation" ON nrd)
add_component_option(NRDP_HAS_SWITCHED_INSTRUMENTATION "[-nrd-switched-instrumentation] Enable debug instrumentation" ON nrd)
add_component_option(NRDP_HAS_TEST_INSTRUMENTATION "[-nrd-test-instrumentation] Enable test instrumentation" OFF nrd)
add_component_option(NRDP_HAS_DEBUG_INSTRUMENTATION "[-nrd-debug-instrumentation] Enable debug instrumentation" OFF nrd)
add_component_option(USE_MVC_SPLIT "[-use-mvc-split] use MVC split mode" OFF nrd)
add_component_option(NRDP_ASE "[-ase] Build with C++ ASE" ON nrd)
set(NRDP_CRASH_REPORTING "off" CACHE STRING "[-crash-reporting] Enable crash reporting: breakpad, internal, off")
# ------------------------------------------------------------------------------------------

# include("cmake/Documentation.cmake")

# ------------------------------------------------------------------------------------------
string(TOLOWER "${NRDP_CRASH_REPORTING}" NRDP_CRASH_REPORTING)
if (APPLE)
    set(NRDP_CRASH_REPORTING_ENABLED OFF)
elseif(NRDP_CRASH_REPORTING STREQUAL "breakpad")
    set(NRDP_CRASH_REPORTING_ENABLED ON)
    set(NRDP_CRASH_REPORTING_BREAKPAD TRUE)
    set(NRDP_CRASH_REPORTING_BREAKPAD TRUE PARENT_SCOPE)
elseif(NRDP_CRASH_REPORTING STREQUAL "internal")
    set(NRDP_CRASH_REPORTING_ENABLED ON)
    set(NRDP_CRASH_REPORTING_INTERNAL TRUE)
    set(NRDP_CRASH_REPORTING_INTERNAL TRUE PARENT_SCOPE)
elseif(NRDP_CRASH_REPORTING STREQUAL "off")
    set(NRDP_CRASH_REPORTING_ENABLED OFF)
else()
    message(FATAL_ERROR "Unknown crash reporting setting '${NRDP_CRASH_REPORTING}'")
endif()

set(NRDP_CRASH_REPORTING_ENABLED "${NRDP_CRASH_REPORTING_ENABLED}" PARENT_SCOPE)

# ------------------------------------------------------------------------------------------
string(TOLOWER "${NRDP_SCHEDULER_TYPE}" NRDP_SCHEDULER_TYPE)

if(NRDP_SCHEDULER_TYPE STREQUAL "rr")
    set(SCHEDULER_TYPE "SCHED_RR")
elseif(NRDP_SCHEDULER_TYPE STREQUAL "fifo")
    set(SCHEDULER_TYPE "SCHED_FIFO")
elseif(NRDP_SCHEDULER_TYPE STREQUAL "other")
    set(SCHEDULER_TYPE "SCHED_OTHER")
else()
    set(SCHEDULER_TYPE "SCHED_RR")
endif()

add_component_option_text(COMPONENT nrd DESCRIPTION "Thread scheduling policy" VALUE "${SCHEDULER_TYPE}")

# ------------------------------------------------------------------------------------------
add_component_option(NRDP_HAS_PARTNER_INTEGRATION "[-partner-integration] Build with nrdil support" ON nrd)

if (NRDP_HAS_PARTNER_INTEGRATION)
  add_subdirectory(integration)
endif ()

# ------------------------------------------------------------------------------------------
if(NOT NRDP_ENDIAN STREQUAL "auto")
    if(NRDP_ENDIAN STREQUAL "little")
        set(NRDP_LITTLE_ENDIAN TRUE)
    elseif(NRDP_ENDIAN STREQUAL "big")
        set(NRDP_BIG_ENDIAN TRUE)
    elseif(NRDP_ENDIAN STREQUAL "pdp")
        set(NRDP_PDP_ENDIAN TRUE)
    else()
        message(FATAL_ERROR "Unknown CPU endianness: ${NRDP_ENDIAN}")
    endif()
endif()

if(NOT NRDP_GPU_ENDIAN STREQUAL "auto")
    if(NRDP_GPU_ENDIAN STREQUAL "little")
        set(NRDP_GPU_LITTLE_ENDIAN TRUE)
    elseif(NRDP_GPU_ENDIAN STREQUAL "big")
        set(NRDP_GPU_BIG_ENDIAN TRUE)
    elseif(NRDP_GPU_ENDIAN STREQUAL "pdp")
        set(NRDP_GPU_PDP_ENDIAN TRUE)
    else()
        message(FATAL_ERROR "Unknown GPU endianness: ${NRDP_GPU_ENDIAN}")
    endif()
endif()
# ------------------------------------------------------------------------------------------

set(NRDP_CFLAGS ${${CFLAGS_VAR}})
set(NRDP_CXXFLAGS ${${CXXFLAGS_VAR}})
set(NRDP_LFLAGS "${${LFLAGS_VAR}} ${CMAKE_EXE_LINKER_FLAGS}")

if(BUILD_COMPILE_RESOURCES)
    set(NRDP_HAS_COMPILED_RESOURCES TRUE)
endif()

if(NOT NRDP_HAS_INSTRUMENTATION)
    set(NRDP_HAS_ON_INSTRUMENTATION FALSE)
    set(NRDP_HAS_SWITCHED_INSTRUMENTATION FALSE)
    set(NRDP_HAS_TEST_INSTRUMENTATION FALSE)
    set(NRDP_HAS_DEBUG_INSTRUMENTATION FALSE)
endif()

#ifdef NRDP_HAS_HDR_VIDEO
set(NRDP_HAS_HDR_VIDEO "yes" CACHE STRING "[HIDDEN] Enable HDR video profiles")
if(NRDP_HAS_HDR_VIDEO)
    add_definitions("-DNRDP_HAS_HDR_VIDEO")
endif()
#endif

#------------------------------------------------------------------------------------
set(GROUP_DPI "DPI options")

set(DPI_IMPLEMENTATION "auto" CACHE STRING "[-dpi] Sets the NRD DPI library to build. 'system' is a special value that means use the passed LDFLAGS and CFLAGS but not build anything. The NRDP SDK ships with stub, reference-x86, and reference-sink DPI implementations.")
set(TEE_IMPLEMENTATION "reference" CACHE STRING "[-tee] Set the TEE implementation.  Default is 'reference'.")

if(NOT BUILD_DPI_DIRECTORY)
   set(BUILD_DPI_DIRECTORY "${NRDP_ROOT_DIR}/dpidir" CACHE PATH "[-dpidir] Additional directory where dpi source files can be located")
endif()

if(DPI_IMPLEMENTATION MATCHES "auto")
    if(BUILD_NRDSYSTEM MATCHES "linux")
        set(DPI_IMPLEMENTATION "reference")
    endif()
endif()

set(DPI_REFERENCE_DRM "auto" CACHE STRING "DRM implementation to use with the reference DPI. Only 'none' works with the provided toolchain")
if (DPI_REFERENCE_DRM STREQUAL "auto")
  set(DPI_REFERENCE_DRM "playready2.5-ss-tee")
endif()

#set(HAVE_DPI_CONFIGURATION_OPTIONS false)

# Check if we have a system dpi. We need to do it here because otherwise the imported target won't be visible by the other ones
if(DPI_IMPLEMENTATION MATCHES "system")
    get_environment_variable(DPI_LDFLAGS "LDFLAGS used when linking against the DPI" "")

    message(" ")
    message("=============================================================")
    message(" System DPI")
    message("-------------------------------------------------------------")
    message("  - DPI_LDFLAGS=${DPI_LDFLAGS}")

    add_library(nrddpi UNKNOWN IMPORTED)
    set_target_properties(nrddpi PROPERTIES IMPORTED_LINK_INTERFACE_LANGUAGES "CXX")

    parse_ldflags("${DPI_LDFLAGS}" LIBRARIES "SYSTEM_DPI_LIBRARIES" DIRECTORIES "SYSTEM_DPI_DIRECTORIES" FLAGS "SYSTEM_DPI_FLAGS")

    set(SYSTEM_DPI_LOCATION "")
    foreach(directory ${SYSTEM_DPI_DIRECTORIES})
        if(EXISTS "${directory}/libnrddpi.a")
            set(SYSTEM_DPI_LOCATION "${directory}/libnrddpi.a")
            break()
        elseif(EXISTS "${directory}/libnrddpi.so")
            set(SYSTEM_DPI_LOCATION "${directory}/libnrddpi.so")
            break()
        endif()
    endforeach()

    if(SYSTEM_DPI_DIRECTORIES)
        message("    + Library directories: ${SYSTEM_DPI_DIRECTORIES}")
        link_directories(${SYSTEM_DPI_DIRECTORIES})
    endif()

    if(SYSTEM_DPI_LIBRARIES)
        message("    + Additional libraries: ${SYSTEM_DPI_LIBRARIES}")
        set_target_properties(nrddpi PROPERTIES IMPORTED_LINK_INTERFACE_LIBRARIES "${SYSTEM_DPI_LIBRARIES}")
    endif()

    if(SYSTEM_DPI_FLAGS)
        message("    + Additional flags: ${SYSTEM_DPI_FLAGS}")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${SYSTEM_DPI_FLAGS}")
    endif()

    if(SYSTEM_DPI_LOCATION)
        message("    + Location: ${SYSTEM_DPI_LOCATION}")
        set_target_properties(nrddpi PROPERTIES IMPORTED_LOCATION ${SYSTEM_DPI_LOCATION})
    else()
        message(FATAL_ERROR "Can't find libnrddpi.a or .so on any of the library directories!")
    endif()

    message("=============================================================")
    message(" ")
else()
    add_subdirectory(dpi)
endif()
#------------------------------------------------------------------------------------

# This value can be set from each DPI and lets Nrd know if getDeviceLibOptions is going to be available or not
set(NRDP_DPI_CONFIGURATION_OPTIONS "${HAVE_DPI_CONFIGURATION_OPTIONS}")

add_subdirectory(base)
add_subdirectory(net)
add_subdirectory(ase)
add_subdirectory(demux)
add_subdirectory(nrd)
add_subdirectory(platform)
