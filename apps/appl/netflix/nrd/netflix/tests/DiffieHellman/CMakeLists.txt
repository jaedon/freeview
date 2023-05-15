#
# (c) 1997-2015 Netflix, Inc.  All content herein is protected by
# U.S. copyright and other applicable intellectual property laws and
# may not be copied without the express permission of Netflix, Inc.,
# which reserves all rights. Reuse of any of this content for any
# purpose without the permission of Netflix, Inc. is strictly
# prohibited.

cmake_minimum_required( VERSION 2.8 )
project(dh-tests)

add_definitions(
#  -DALL
)

#enable_testing()
add_custom_target(dh-tests)

macro(add_teeapi_test target sources)
  add_executable(${target} ${COMMON_SOURCES};${sources})
  target_link_libraries(${target} ${DEPLIBS})
  add_dependencies(dh-tests ${target})
endmacro()


set(DEPLIBS
  nrddpi)

set(TEEAPITESTS_PROGRAMS
  TestDiffieHellman)

foreach(test_program ${TEEAPITESTS_PROGRAMS})
  add_teeapi_test(${test_program} ${test_program}.cpp)
endforeach(test_program)
