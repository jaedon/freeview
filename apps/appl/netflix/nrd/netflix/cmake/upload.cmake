# (c) 1997-2015 Netflix, Inc.  All content herein is protected by
# U.S. copyright and other applicable intellectual property laws and
# may not be copied without the express permission of Netflix, Inc.,
# which reserves all rights.  Reuse of any of this content for any
# purpose without the permission of Netflix, Inc. is strictly
# prohibited.

# Uploads symbols to Netflix's box crash reporting account
# Invoke it with: cmake -DUSER=<username> -DPASSWORD=<password> -DFILE=<absolute path to file to upload> -DFOLDER=<upload folder> -DCURL=<path to the curl utility> -P upload.cmake

cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

get_filename_component(symbols "${FILE}" NAME)
set(symbols_we ${symbols})
string(REPLACE ".sym.pgp.gz" "" symbols_we "${symbols_we}")
string(REPLACE ".sym.gz" "" symbols_we "${symbols_we}")
string(REPLACE ".pgp" "" symbols_we "${symbols_we}")
string(REPLACE ".sym" "" symbols_we "${symbols_we}")
string(REPLACE ".gz" "" symbols_we "${symbols_we}")
get_filename_component(directory "${FILE}" PATH)

if(NOT EXISTS "${FILE}")
    message(FATAL_ERROR "File ${FILE} not found")
endif()

set(id "${directory}/${symbols_we}.id")

if(NOT EXISTS "${id}")
    message(FATAL_ERROR "File ${id} not found")
endif()

file(STRINGS ${id} build_id LIMIT_COUNT 1)
if("${build_id}" STREQUAL "")
    message(FATAL_ERROR "The build id is empty")
endif()

if(NOT FOLDER)
    message(FATAL_ERROR "Upload folder not specified")
endif()

if(NOT USER)
    message(FATAL_ERROR "Upload user name not specified")
endif()

if(NOT PASSWORD)
    message(FATAL_ERROR "Upload password not specified")
endif()

if(NOT CURL)
    # Curl was not specified. Assume, then, it's on the PATH
    set(CURL "curl")
endif()

message("Uploading ${FILE} for build id ${build_id} to folder ${FOLDER} using ${CURL}...")

execute_process(
    COMMAND ${CURL} -k --anyauth --user ${USER}:${PASSWORD} -T ${FILE} https://dav.box.com/dav/${FOLDER}/${symbols}.${build_id}
    TIMEOUT 120
    RESULT_VARIABLE result
    OUTPUT_VARIABLE output
    ERROR_VARIABLE err
)

if(result)
    message(FATAL_ERROR "Error uploading: ${result}\n${err}")
endif()

string(REGEX MATCH "<d:error" error "${output}")
if(error)
    string(REGEX MATCH "<s:exception>(.*)</s:exception>" exception "${output}")
    string(REPLACE "<s:exception>" "" exception "${exception}")
    string(REPLACE "</s:exception>" "" exception "${exception}")

    string(REGEX MATCH "<s:message>(.*)</s:message>" message "${output}")
    string(REPLACE "<s:message>" "" message "${message}")
    string(REPLACE "</s:message>" "" message "${message}")

    message(FATAL_ERROR "Error uploading: ${message} (${exception})")
else()
    message("File upload complete")
endif()
