# (c) 1997-2015 Netflix, Inc.  All content herein is protected by
# U.S. copyright and other applicable intellectual property laws and
# may not be copied without the express permission of Netflix, Inc.,
# which reserves all rights.  Reuse of any of this content for any
# purpose without the permission of Netflix, Inc. is strictly
# prohibited.


# Platform independent way generate a preprocessor file
# Invoke it with: cmake -P jspp.cmake -DFILES=<files> -DTARGET=<absolute path to target file>

cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

separate_arguments(FILES UNIX_COMMAND "${FILES}")

set(TOTAL "\n")

foreach(FILE ${FILES})
    set(TOTAL "${TOTAL}#include \"${FILE}\"\n")
endforeach()

file(WRITE ${TARGET} "#ifndef RTAGS\n${TOTAL}#endif\n")
