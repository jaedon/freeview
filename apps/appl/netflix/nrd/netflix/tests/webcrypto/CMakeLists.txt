#
# (c) 1997-2015 Netflix, Inc.  All content herein is protected by
# U.S. copyright and other applicable intellectual property laws and
# may not be copied without the express permission of Netflix, Inc.,
# which reserves all rights. Reuse of any of this content for any
# purpose without the permission of Netflix, Inc. is strictly
# prohibited.

cmake_minimum_required( VERSION 2.8 )
project(webcrypto-tests)

add_custom_target(webcrypto-tests)

macro(make_test target sources)
  add_executable(${target} ${COMMON_SOURCES};${sources})
  target_link_libraries(${target} ${DEPLIBS})
  add_dependencies(webcrypto-tests ${target})
endmacro()


set(DEPLIBS
  nrddpi)

set(TEST_PROGRAMS
  TestWebCrypto
  TestLargeBuffers
  TestMaxKeys)


foreach(test_program ${TEST_PROGRAMS})
  make_test(${test_program} ${test_program}.cpp)
  add_test(NAME ${test_program} COMMAND ${test_program} "/home/skommidi/manuf_ss_bak.bin")
  set_tests_properties(${test_program} PROPERTIES FAIL_REGULAR_EXPRESSION "ERROR;FAIL;Test failed" LABELS "webcrypto_${test_program}")
endforeach(test_program)
