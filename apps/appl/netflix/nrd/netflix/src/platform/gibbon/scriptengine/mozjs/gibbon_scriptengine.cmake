# (c) 1997-2015 Netflix, Inc.  All content herein is protected by
# U.S. copyright and other applicable intellectual property laws and
# may not be copied without the express permission of Netflix, Inc.,
# which reserves all rights.  Reuse of any of this content for any
# purpose without the permission of Netflix, Inc. is strictly
# prohibited.

# SpiderMonkey library
gibbon_add_libraries(js_static)
add_definitions(-DMOZILLA_CLIENT -DSTATIC_JS_API)
include_directories("${TOOLCHAIN_DIRECTORY}/netflix/include/js")

# Bindings
set(GIBBON_SCRIPTENGINE_MOZJS TRUE)
include_directories(scriptengine/mozjs)
gibbon_add_sources(ScriptEngineMOZJS.cpp)
gibbon_add_js(${CMAKE_CURRENT_SOURCE_DIR}/resources/js/mozjs.js)
