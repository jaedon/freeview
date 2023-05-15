# (c) 1997-2015 Netflix, Inc.  All content herein is protected by
# U.S. copyright and other applicable intellectual property laws and
# may not be copied without the express permission of Netflix, Inc.,
# which reserves all rights.  Reuse of any of this content for any
# purpose without the permission of Netflix, Inc. is strictly
# prohibited.

if(NOT DEFINED GIBBON_SCRIPT_BUILTIN_JSC)
   add_component_option(GIBBON_SCRIPT_BUILTIN_JSC "JavaScriptCore - From 3rdParty" ON nrd)
endif()
add_component_option(GIBBON_SCRIPT_JSC_BACKDOOR "JavaScriptCore - With backdoor" ON nrd)
add_component_option(GIBBON_SCRIPT_JSC_BACKDOOR_DEBUGGER "JavaScriptCore - Enable DEBUGGER" ON nrd)

set(GIBBON_SCRIPT_JSC_BACKDOOR_OSALLOCATOR "auto" CACHE STRING "JavaScriptCore - Enable OSALLOCATOR callback: yes, no, auto")
set(GIBBON_SCRIPT_JSC_BACKDOOR_GC ON CACHE BOOLEAN "JavaScriptCore - Enable GC callback: yes, no, auto")

if(GIBBON_SCRIPT_BUILTIN_JSC)
    if(GIBBON_SCRIPT_JSC_BACKDOOR_GC STREQUAL "auto")
        if(BUILD_NRDSYSTEM STREQUAL "linux" AND "${CMAKE_BUILD_TYPE}" STREQUAL "debug")
            add_definitions(-DNRDP_JSC_ENABLE_GC_CALLBACKS)
        else()
            set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DNRDP_JSC_ENABLE_GC_CALLBACKS")
            set(CMAKE_CXX_FLAGS_TEST "${CMAKE_CXX_FLAGS_TEST} -DNRDP_JSC_ENABLE_GC_CALLBACKS")
        endif()
    elseif(GIBBON_SCRIPT_JSC_BACKDOOR_GC)
        add_definitions(-DNRDP_JSC_ENABLE_GC_CALLBACKS)
    endif()
    if(GIBBON_SCRIPT_JSC_BACKDOOR_OSALLOCATOR STREQUAL "auto")
        if(BUILD_NRDSYSTEM STREQUAL "linux" AND "${CMAKE_BUILD_TYPE}" STREQUAL "debug")
            add_definitions(-DNRDP_JSC_ENABLE_OSALLOCATOR_CALLBACKS)
        else()
            set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DNRDP_JSC_ENABLE_OSALLOCATOR_CALLBACKS")
            set(CMAKE_CXX_FLAGS_TEST "${CMAKE_CXX_FLAGS_TEST} -DNRDP_JSC_ENABLE_OSALLOCATOR_CALLBACKS")
        endif()
    elseif(GIBBON_SCRIPT_JSC_BACKDOOR_OSALLOCATOR)
        add_definitions(-DNRDP_JSC_ENABLE_OSALLOCATOR_CALLBACKS)
    endif()
    add_component_option_text(COMPONENT nrd DESCRIPTION "JavaScriptCore - Enable OSALLOCATOR" VALUE "${GIBBON_SCRIPT_JSC_BACKDOOR_OSALLOCATOR}")
    add_component_option_text(COMPONENT nrd DESCRIPTION "JavaScriptCore - Enable GC" VALUE "${GIBBON_SCRIPT_JSC_BACKDOOR_GC}")

    add_component_option(GIBBON_SCRIPT_JSC_DEBUG "JavaScriptCore - Debug mode" OFF nrd)
    add_component_option(GIBBON_SCRIPT_JSC_DEBUG_SYMBOLS "JavaScriptCore - Debug symbols" OFF nrd)
    add_component_option(GIBBON_SCRIPT_JSC_ASSERTIONS "JavaScriptCore - Assertions symbols" OFF nrd)
    add_component_option(GIBBON_SCRIPT_JSC_JIT "JavaScriptCore - JIT enabled" OFF nrd)
    if(NOT DEFINED GIBBON_SCRIPT_JSC_DYNAMIC)
        add_component_option(GIBBON_SCRIPT_JSC_DYNAMIC "JavaScriptCore - Build as a dynamic library" ON nrd)
    endif()
    add_component_option(GIBBON_SCRIPT_JSC_APP "JavaScriptCore - Build the JSC console app" OFF nrd)
    add_component_option(GIBBON_SCRIPT_JSC_VERBOSE "JavaScriptCore - Build in verbose mode" OFF nrd)
    add_component_option(GIBBON_SCRIPT_JSC_WARNINGS "JavaScriptCore - Enable warnings" OFF nrd)

    if(GIBBON_SCRIPT_JSC_DEBUG)
        message("    + DEBUG")
        set(GIBBON_SCRIPTENGINE_JSC_DEBUG TRUE)
        set(JAVASCRIPTCORE_BUILD_TYPE "debug")
    else()
        if(BUILD_NRDSYSTEM STREQUAL "linux")
            message("    + RELEASE")
            set(JAVASCRIPTCORE_BUILD_TYPE "release")
        else()
            message("    + PRODUCTION")
            set(JAVASCRIPTCORE_BUILD_TYPE "production")
       endif()
    endif()

    # JavaScriptCore library
    if(GIBBON_SCRIPT_JSC_DYNAMIC)
        message("    + DYNAMIC")
        gibbon_add_install_libraries(${CMAKE_CURRENT_BINARY_DIR}/libJavaScriptCore.so ${CMAKE_CURRENT_BINARY_DIR}/libWTF.so)
    else()
        message("    + STATIC")
        gibbon_add_install_libraries(${CMAKE_CURRENT_BINARY_DIR}/libJavaScriptCore.a ${CMAKE_CURRENT_BINARY_DIR}/libWTF.a)
    endif()

    if(GIBBON_SCRIPT_JSC_JIT)
        message("    + JIT")
        set(JAVASCRIPTCORE_JIT ON)
    endif()

    set(JAVASCRIPTCORE_SRC "${CMAKE_CURRENT_SOURCE_DIR}/../../../3rdparty/JavaScriptCore")
    set(JAVASCRIPTCORE_BLD "JavaScriptCore")

    # The following options are not available from configure (yet)
    set(JAVASCRIPTCORE_VERBOSE ${GIBBON_SCRIPT_JSC_VERBOSE})   # output the (annoying) list of configured options
    set(JAVASCRIPTCORE_BUILD_JSC ${GIBBON_SCRIPT_JSC_APP}) # build jsc

    if(GIBBON_SCRIPT_JSC_BACKDOOR)
      message("    + BACKDOOR")
      add_definitions(-DGIBBON_SCRIPT_JSC_BACKDOOR)
      if(GIBBON_SCRIPT_JSC_OSALLOCATOR_LOGS)
        message("      + VERBOSE OSALLOCATOR LOGS")
        add_definitions(-DNRDP_OSALLOCATOR_LOGS_ENABLED)
      endif()
      if(GIBBON_SCRIPT_JSC_BACKDOOR_DEBUGGER)
        add_definitions(-DGIBBON_SCRIPT_JSC_DEBUGGER)
      endif()
    endif()

    add_subdirectory("${JAVASCRIPTCORE_SRC}" "${JAVASCRIPTCORE_BLD}")

    set_target_properties(JavaScriptCore PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    set_target_properties(WTF PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

    add_definitions(-DBUILDING_GIBBON__ -DBUILDING_WITH_CMAKE=1 -DHAVE_CONFIG_H=1 -DXP_UNIX)

    include_directories(BEFORE "${CMAKE_CURRENT_BINARY_DIR}/${JAVASCRIPTCORE_BLD}/Source/" "${JAVASCRIPTCORE_SRC}/Source/"
                    "${JAVASCRIPTCORE_SRC}/Source/JavaScriptCore/heap" "${JAVASCRIPTCORE_SRC}/Source/JavaScriptCore/runtime"
                    "${JAVASCRIPTCORE_SRC}/Source/JavaScriptCore/ForwardingHeaders" "${JAVASCRIPTCORE_SRC}/Source/JavaScriptCore/wtf"
                    "${JAVASCRIPTCORE_SRC}/Source/JavaScriptCore/API" "${JAVASCRIPTCORE_SRC}/Source/JavaScriptCore/interpreter"
                    "${JAVASCRIPTCORE_SRC}/Source/JavaScriptCore/jit" "${JAVASCRIPTCORE_SRC}/Source/JavaScriptCore/assembler"
                    "${JAVASCRIPTCORE_SRC}/Source/JavaScriptCore" "${JAVASCRIPTCORE_SRC}/Source/JavaScriptCore/llint"
                    "${JAVASCRIPTCORE_SRC}/Source/JavaScriptCore/bytecode" "${JAVASCRIPTCORE_SRC}/Source/JavaScriptCore/parser"
                    "${JAVASCRIPTCORE_SRC}/Source/WTF")

    gibbon_add_libraries(JavaScriptCore icui18n icuuc)
    if(GIBBON_SCRIPT_JSC_DYNAMIC AND NOT APPLE)
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-rpath '-Wl,\$ORIGIN'")
    endif()
else()
    message("      + EXTERNAL")
    set(GIBBON_SCRIPT_JSC_BACKDOOR OFF)
    set(GIBBON_SCRIPT_JSC_BACKDOOR_OSALLOCATOR OFF)
    set(GIBBON_SCRIPT_JSC_BACKDOOR_DEBUGGER OFF)
endif()

# Bindings
set(GIBBON_SCRIPTENGINE_JSC TRUE)
include_directories(scriptengine/jsc)
if(NOT GIBBON_SCRIPT_JSC_BACKDOOR)
  gibbon_add_sources(NetflixJSCFrontdoor.cpp)
endif()

gibbon_add_sources(ScriptEngineJSC.cpp GibbonEventConnectionJSC.cpp JSC.cpp JSONParserJSC.cpp XMLParserJSC.cpp ArrayBufferJSC.cpp)
gibbon_add_js(${CMAKE_CURRENT_SOURCE_DIR}/resources/js/jsc.js)

