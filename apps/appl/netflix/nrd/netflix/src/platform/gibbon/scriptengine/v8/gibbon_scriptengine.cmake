# (c) 1997-2015 Netflix, Inc.  All content herein is protected by
# U.S. copyright and other applicable intellectual property laws and
# may not be copied without the express permission of Netflix, Inc.,
# which reserves all rights.  Reuse of any of this content for any
# purpose without the permission of Netflix, Inc. is strictly
# prohibited.

get_environment_variable(TOOLCHAIN_DIRECTORY "Toolchain Directory" "")
set(V8_DIR "${TOOLCHAIN_DIRECTORY}/netflix/lib")
gibbon_add_libraries(-Wl,--start-group ${V8_DIR}/libv8_libbase.a ${V8_DIR}/libv8_base.ia32.a ${V8_DIR}/libv8_snapshot.a ${V8_DIR}/libicuuc.a ${V8_DIR}/libicudata.a ${V8_DIR}/libicui18n.a -Wl,--end-group)

# Bindings
set(GIBBON_SCRIPTENGINE_V8 TRUE)
include_directories(scriptengine/v8)
gibbon_add_sources(ScriptEngineV8.cpp JSONParserV8.cpp)
gibbon_add_js(${CMAKE_CURRENT_SOURCE_DIR}/resources/js/v8.js)
