# (c) 1997-2015 Netflix, Inc.  All content herein is protected by
# U.S. copyright and other applicable intellectual property laws and
# may not be copied without the express permission of Netflix, Inc.,
# which reserves all rights.  Reuse of any of this content for any
# purpose without the permission of Netflix, Inc. is strictly
# prohibited.


# Platform independent way to replace strings from
# Invoke it with: cmake -DFILE=<file> -DSEARCH=<regular_expression> -DREPLACE=<replace_expression> -DVERBOSE -P replace.cmake -DBINARY=1

cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

if(VERBOSE)
    message("Reading ${FILE} and replacing its contents...")
endif()
if (BINARY)
  file(READ ${FILE} CONTENTS HEX)
else ()
  file(READ ${FILE} CONTENTS)
endif ()
string(REGEX REPLACE "${SEARCH}" "${REPLACE}" CONTENTS "${CONTENTS}")

if(VERBOSE)
    message("Writing ${FILE}...")
endif()
file(WRITE ${FILE} "${CONTENTS}")
