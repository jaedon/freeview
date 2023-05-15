# (c) 1997-2015 Netflix, Inc.  All content herein is protected by
# U.S. copyright and other applicable intellectual property laws and
# may not be copied without the express permission of Netflix, Inc.,
# which reserves all rights.  Reuse of any of this content for any
# purpose without the permission of Netflix, Inc. is strictly
# prohibited.

cmake_minimum_required( VERSION 2.8 )

# Headers
set( dfb-benchmark_HDRS
    Rect.h
    Sprite.h
    DFBSurface.h
    Benchmark.h
    Point.h
    Timer.h
    ISurface.h
    Color.h
)

# Source files
set( dfb-benchmark_SRCS
  Rect.cpp
  DFBSurface.cpp
  Benchmark.cpp
  Sprite.cpp
  Timer.cpp
  main.cpp
)

include_directories( ${CMAKE_CURRENT_SOURCE_DIR} ${TOOLCHAIN_DIRECTORY}/netflix/include/directfb )

## Final executable!
set( sysroot $ENV{TOOLCHAIN_SYSROOT} )
if( sysroot )
    set( sysroot "-Wl,-rpath-link,$ENV{TOOLCHAIN_SYSROOT}" )
    message( STATUS "Using sysroot directory: $ENV{TOOLCHAIN_SYSROOT}" )
else()
    set( sysroot "" )
endif()

add_custom_target(dfb-benchmark_data ALL
  COMMAND ${CMAKE_COMMAND} -E create_symlink ${NRDP_ROOT_DIR}/app/benchmark/directfb/Netflix_Logo.png ${CMAKE_CURRENT_BINARY_DIR}/Netflix_Logo.png
  COMMAND ${CMAKE_COMMAND} -E create_symlink ${NRDP_ROOT_DIR}/app/benchmark/directfb/Arial\ Bold.ttf ${CMAKE_CURRENT_BINARY_DIR}/Arial\ Bold.ttf
)

set(CMAKE_EXE_LINKER_FLAGS "${sysroot} -Wl,-rpath-link,${TOOLCHAIN_LIB}")
add_executable(dfb-benchmark ${dfb-benchmark_SRCS})
target_link_libraries(dfb-benchmark directfb)
set_target_properties(dfb-benchmark PROPERTIES OUTPUT_NAME "benchmark")
add_dependencies(dfb-benchmark dfb-benchmark_data)

# Installation
install( TARGETS dfb-benchmark DESTINATION bin )
