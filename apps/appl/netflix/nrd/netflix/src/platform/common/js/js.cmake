# (c) 1997-2015 Netflix, Inc.  All content herein is protected by
# U.S. copyright and other applicable intellectual property laws and
# may not be copied without the express permission of Netflix, Inc.,
# which reserves all rights.  Reuse of any of this content for any
# purpose without the permission of Netflix, Inc. is strictly
# prohibited.

cmake_minimum_required(VERSION 2.8)
if("${CMAKE_VERSION}" VERSION_GREATER 2.8.12.2)
    cmake_policy(SET CMP0045 OLD)
    cmake_policy(SET CMP0046 OLD)
endif()
get_filename_component(COMMON_JS_DIR ${CMAKE_CURRENT_LIST_FILE} PATH)

set(GROUP_JS "JS options")

# Find optional MDXJS
# ------------------------------------------------------------------------------------------
if(NOT JS_MDXJS_DIRECTORY)
    set(JS_MDXJS_DIRECTORY "${NRDP_ROOT_DIR}/mdxjs" CACHE PATH "[-js-mdxjs] Directory where mdxjs source can be located")
endif()

set(MDXJS_EXPORT_SRCS TRUE)
find_nrd_module(MDXJS
                DIRECTORIES ${JS_MDXJS_DIRECTORY}
                SOURCE_FILES CMakeLists.txt;mdxapi.js
                BASE_DIRECTORY "${NRDP_BINARY_ROOT_DIR}"
                NO_VERSION)

# Export the variable to the parent scope so it's seen if this module is included by another target (highly likely)
set(MDXJS_FOUND "${MDXJS_FOUND}" PARENT_SCOPE)

if(MDXJS_FOUND)
    list(APPEND NETFLIXBRIDGE_JS_FILES ${MDXJS_EXPORTED_SRCS})
    add_component(NAME mdxjs LOCATION ${JS_MDXJS_DIRECTORY})
endif()

# ------------------------------------------------------------------------------------------

# Get the global list of files to be appended to NetflixBridge.js, which can be updated by any other module.

message(" ")
message("=============================================================")
message(" JavaScript bridge:")
message("-------------------------------------------------------------")

get_property(JS_EXPORTS GLOBAL PROPERTY JS_EXPORTS)
message(" + External JavaScript files: ${JS_EXPORTS}")

set(NETFLIXBRIDGE_JS_FILES
    ${JS_EXPORTS}
    ${NRDP_ROOT_DIR}/src/nrd/NBP/bridge/system.js
    ${NRDP_ROOT_DIR}/src/nrd/NBP/bridge/network.js
    ${NETFLIXBRIDGE_JS_FILES}
)

if(BUILD_SSO)
    set(NETFLIXBRIDGE_JS_FILES
        ${NETFLIXBRIDGE_JS_FILES}
        ${NRDP_ROOT_DIR}/src/dpi/common/sso/singlesignon.js
    )
endif()

if(BUILD_VOICE)
    set(NETFLIXBRIDGE_JS_FILES
        ${NETFLIXBRIDGE_JS_FILES}
        ${NRDP_ROOT_DIR}/src/dpi/common/voice/voice.js
    )
endif()

if(BUILD_SIGNUP)
    set(NETFLIXBRIDGE_JS_FILES
        ${NETFLIXBRIDGE_JS_FILES}
        ${NRDP_ROOT_DIR}/src/dpi/common/signup/signup.js
    )
endif()

message("=============================================================")
message(" ")

macro(CREATE_JS_FILE sources target includes)
    # Concatenate all js files
    if (NRDP_PLATFORM_NOVA)
        set(JSCPP "mcpp.exe")
        if(NOT (CMAKE_C_COMPILER_ARG1 STREQUAL " " OR CMAKE_C_COMPILER_ARG1 STREQUAL ""))
            string(REPLACE " " ";" CMAKE_C_COMPILER_ARG1_TMP ${CMAKE_C_COMPILER_ARG1})
            list(APPEND JSCPP ${CMAKE_C_COMPILER_ARG1_TMP})
        endif()
        set(JSCPPFLAGS -N -I- -P -e UTF-8)
        if(CMAKE_BUILD_TYPE STREQUAL "debug")
            list(APPEND JSCPPFLAGS -DBUILD_DEBUG)
        else()
            list(APPEND JSCPPFLAGS -DNDEBUG)
        endif()

        add_custom_command(OUTPUT "${target}"
            DEPENDS ${NETFLIXBRIDGE_JS_FILES}
            COMMAND ${CMAKE_COMMAND} -DFILES="${includes} ${sources}" -DTARGET="${target}.c" -P "${COMMON_JS_DIR}/jspp.cmake"
            COMMAND ${JSCPP} ${JSCPPFLAGS} "${target}.c" -o "${target}"
            COMMENT "Building JavaScript [${target}]"
        )
    else()
        set(JSCPP ${CMAKE_C_COMPILER})
        if(NOT (CMAKE_C_COMPILER_ARG1 STREQUAL " " OR CMAKE_C_COMPILER_ARG1 STREQUAL ""))
            string(REPLACE " " ";" CMAKE_C_COMPILER_ARG1_TMP ${CMAKE_C_COMPILER_ARG1})
            list(APPEND JSCPP ${CMAKE_C_COMPILER_ARG1_TMP})
        endif()
        set(JSCPPFLAGS -Wp,-P -undef -Wundef -std=c99 -nostdinc -Wtrigraphs -fdollars-in-identifiers -E)
        if(CMAKE_BUILD_TYPE STREQUAL "debug")
            list(APPEND JSCPPFLAGS -Wp,-C -DBUILD_DEBUG)
        else()
            list(APPEND JSCPPFLAGS -DNDEBUG)
        endif()
        foreach(INCLUDE ${includes})
            list(APPEND JSCPPFLAGS -include ${INCLUDE})
        endforeach()
        add_custom_command(OUTPUT "${target}"
            DEPENDS ${NETFLIXBRIDGE_JS_FILES}
            COMMAND ${CMAKE_COMMAND} -DFILES="${sources}" -DTARGET="${target}.c" -P "${COMMON_JS_DIR}/jspp.cmake"
            COMMAND ${JSCPP} ${JSCPPFLAGS}  "${target}.c" -o "${target}"
            COMMENT "Building JavaScript [${target}]"
        )
    endif()

    # Minify the source code if the java runtime is present
    if(BUILD_SMALL)
        find_package(Java COMPONENTS Runtime)
        if(NOT Java_Runtime_FOUND)
            message(STATUS "WARNING: Java was not found. It will not be possible to minify the JavaScript source code")
        else()
        add_custom_command(OUTPUT "${target}"
            COMMAND ${CMAKE_COMMAND} -E rename "${target}" "${target}.max"
            COMMAND ${Java_JAVA_EXECUTABLE} -jar ${COMMON_JS_DIR}/compiler.jar --js_output_file ${target} --js ${target}.max --jscomp_off internetExplorerChecks --language_in ECMASCRIPT5 --compilation_level SIMPLE_OPTIMIZATIONS
            COMMENT "Minifying JavaScript [${target}]"
            APPEND
        )
        endif()
    endif()

endmacro()

macro(CREATE_DATA_JS_DIRECTORY target)
    get_target_property(target_directory ${target} RUNTIME_OUTPUT_DIRECTORY)
    if(NOT target_directory)
        set(target_directory "${CMAKE_CURRENT_BINARY_DIR}")
    endif()

    set(APP_DATA_DIR "${target_directory}/data")
    set(JS_RESOURCES_DIR "${COMMON_DIRECTORY}/js/resources")

    # Copy MDX resources, if necessary
#if(BUILD_MDX AND MDXLIB_SOURCE)
    #    copy_mdx_resources(${target} ${APP_DATA_DIR})
    #endif()

    file(MAKE_DIRECTORY "${NRDP_BINARY_ROOT_DIR}/src/generated/${target}/resources")

    if(BUILD_COMPILE_RESOURCES)
        set(netflix_bridge "${CMAKE_CURRENT_BINARY_DIR}/NetflixBridge.js")
    else()
        set(netflix_bridge "${APP_DATA_DIR}/resources/js/NetflixBridge.js")
    endif()

    create_js_file("${NETFLIXBRIDGE_JS_FILES}" "${netflix_bridge}" "${NRDP_BINARY_ROOT_DIR}/include/nrdbase/config.h")

    if(BUILD_COMPILE_RESOURCES)
        compile_resource(${target} "${netflix_bridge}" "${APP_DATA_DIR}/resources/js/NetflixBridge.js" COMPILABLE)
    else()
        add_resource(${target} "${netflix_bridge}")
    endif()

    set(NRDP_JS_BRIDGE "${netflix_bridge}")

endmacro()
