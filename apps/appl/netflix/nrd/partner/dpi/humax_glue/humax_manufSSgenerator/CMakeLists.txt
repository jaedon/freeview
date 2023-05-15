# (c) 1997-2015 Netflix, Inc.  All content herein is protected by
# U.S. copyright and other applicable intellectual property laws and
# may not be copied without the express permission of Netflix, Inc.,
# which reserves all rights.  Reuse of any of this content for any
# purpose without the permission of Netflix, Inc. is strictly
# prohibited.

cmake_minimum_required( VERSION 2.8 )

set(CMAKE_REQUIRED_LIBRARIES_SAVED ${CMAKE_REQUIRED_LIBRARIES})

message(" ")
message("=============================================================")
message("Manufacturing Secure Store Generator")
message("=============================================================")
message(" ")


set(SOURCES
    ManufSS.cpp
    main.cpp
)
#define add
add_definitions( -DHUMAX_PLATFORM_BASE )

file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
add_executable(humax_manufss ${SOURCES} ${HEADERS})
target_link_libraries(humax_manufss ssl crypto)
set_target_properties(humax_manufss PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/src/platform/gibbon)
add_custom_target(humax_manufss_script_binary
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/netflix_manuss.sh ${CMAKE_BINARY_DIR}/src/platform/gibbon
)
add_dependencies(humax_manufss manufss_script_binary)
