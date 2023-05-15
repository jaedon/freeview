# (c) 1997-2015 Netflix, Inc.  All content herein is protected by
# U.S. copyright and other applicable intellectual property laws and
# may not be copied without the express permission of Netflix, Inc.,
# which reserves all rights.  Reuse of any of this content for any
# purpose without the permission of Netflix, Inc. is strictly
# prohibited.

cmake_minimum_required(VERSION 2.8)

include(gibbon.cmake)
include(CheckCSourceCompiles)
include(CheckCXXSourceCompiles)
include(CMakeParseArguments)

# Common sources
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(bridge)
include_directories(text)
gibbon_add_sources(
    Animation.cpp
    Animation.h
    Color.h
    Debug.cpp
    Debug.h
    GibbonApplication.cpp
    GibbonApplication.h
    GibbonConfiguration.cpp
    GibbonConfiguration.h
    GibbonConsole.cpp
    GibbonConsole.h
    GibbonCookieManager.h
    GibbonCookieManager.cpp
    GibbonResourceManager.cpp
    GibbonResourceManager.h
    GraphicsEngine.h
    GraphicsEngine.cpp
    Image.cpp
    Image.h
    GibbonEvent.cpp
    GibbonEvent.h
    InputManager.cpp
    InputManager.h
    Point.h
    Size.h
    Rect.h
    RectPacker.cpp
    RectPacker.h
    Screen.cpp
    Screen.h
    ScriptEngine.cpp
    ScriptEngine.h
    Surface.cpp
    Surface.h
    DisplayList.cpp
    DisplayList.h
    SurfaceAtlas.cpp
    SurfaceAtlas.h
    SurfaceCache.cpp
    SurfaceCache.h
    SurfaceDecoder.cpp
    SurfaceDecoder.h
    Effects.cpp
    Effects.h
    Text.cpp
    Text.h
    Video.cpp
    Video.h
    Transform.cpp
    Transform.h
    Widget.cpp
    Widget.h
    main.cpp
    RemoteScreen.cpp
    RemoteScreen.h

    bridge/DebuggerBridge.cpp
    bridge/DebuggerBridge.h
    bridge/EffectBridge.cpp
    bridge/EffectBridge.h
    bridge/GibbonBaseBridge.cpp
    bridge/GibbonBridge.cpp
    bridge/GibbonBridge.h
    bridge/ImageBridge.cpp
    bridge/ImageBridge.h
    bridge/TextBridge.cpp
    bridge/TextBridge.h
    bridge/ScreenBridge.cpp
    bridge/ScreenBridge.h
    bridge/WidgetBridge.cpp
    bridge/WidgetBridge.h

    text/EdgeEffect.h
    text/Font.h
    text/FontManager.cpp
    text/FontManager.h
    text/IteratorsCache.cpp
    text/IteratorsCache.h
    text/Shadow.h
    text/Style.cpp
    text/Style.h
    text/TextClusterIterator.h
    text/TextCursor.h
    text/TextEngine.cpp
    text/TextEngine.h
    text/TextTruncation.h
    text/UnicodeBlocks.cpp
    text/UnicodeBlocks.h
    text/TextLocale.h
    text/TextLocale.cpp

    text/itemizer/TextItemizer.h
    text/itemizer/TextItemizer.cpp
    text/itemizer/TextItemizerCommon.h
    text/itemizer/TextAttributesItemizer.h
    text/itemizer/TextAttributesItemizer.cpp
    text/itemizer/TextDirectionItemizer.h
    text/itemizer/TextDirectionItemizer.cpp
    text/itemizer/TextScriptItemizer.h
    text/itemizer/TextScriptItemizer.cpp
    text/itemizer/TextFontItemizer.h
    text/itemizer/TextFontItemizer.cpp
    text/itemizer/TextLocaleItemizer.cpp
    text/itemizer/TextWordBreakItemizer.cpp
)

set(GROUP_GIBBON "Gibbon options")

add_component_option(GIBBON_ANIMATION_THREAD "Build with gibbon animation thread" ON nrd)
if(NOT GIBBON_ANIMATION_THREAD)
    add_definitions(-DGIBBON_NO_THREADANIMATION)
endif()

add_component_option(GIBBON_BSDIFF "bsdiff - From 3rdParty" ON nrd)
if(GIBBON_BSDIFF)
    set(BSDIFF_SRC  "${CMAKE_CURRENT_SOURCE_DIR}/../../../3rdparty/bsdiff")
    set(BSDIFF_BLD "bsdiff")
    include_directories(${BSDIFF_SRC})
    add_subdirectory("${BSDIFF_SRC}" "${BSDIFF_BLD}")
    add_definitions(-DGIBBON_HAVE_BSDIFF)
    gibbon_add_libraries(bsdiff)
endif()

add_component_option(GIBBON_BENCHMARKS "Build with gibbon benchmark" ON nrd)
if(GIBBON_BENCHMARKS)
    include_directories(benchmarks)
    gibbon_add_sources(benchmarks/Benchmarks.cpp)
    add_definitions(-DGIBBON_BENCHMARKS=1)
endif()

gibbon_add_js(
    resources/js/gibbon.js
    resources/js/scene.js
    resources/js/image.js
    resources/js/text.js
    resources/js/widget.js
    resources/js/effect.js
)

if(BUILD_QA)
  gibbon_add_sources(bridge/GibbonQABridge.cpp bridge/GibbonQABridge.h)
  gibbon_add_js(resources/js/gibbon-qa.js)
endif()

set(GIBBON_ERROR "error.js" CACHE STRING "Error page")
add_component_option(GIBBON_NICE_THREADS "Configure Gibbon threads with setting nice priorities" ON nrd)
add_component_option(GIBBON_COTIRE "Create Gibbon cotire targets" OFF nrd)
add_component_option(GIBBON_TCMALLOC "Use tcmalloc for Gibbon" ON nrd)
add_component_option(GIBBON_TILEMANAGER "Run gibbon with tilemanager" OFF nrdapp)
add_component_option(GIBBON_TEXT_CALC_PADDING "Calculate text padding based on the text run styles" OFF nrdapp)

set(GROUP_SYMBOLS "Symbol table options")
set(SYMBOLS_USERNAME "" CACHE STRING "User name for symbol table uploads")
set(SYMBOLS_PASSWORD "" CACHE STRING "Password for symbol table uploads")
set(SYMBOLS_FOLDER "" CACHE STRING "Folder name for symbol table uploads")
set(SYMBOLS_CURL "curl" CACHE STRING "Absolute path to the curl command line utility")
set(SYMBOLS_GPG_KEYRING "" CACHE STRING "Absolute path to a keyring to use to encrypt the symbol files before uploading")
set(SYMBOLS_GPG_RECIPIENT "" CACHE STRING "Recipient to use to encrypt the symbol files before uploading")

set(SYMBOLS_STRIP "--strip-debug --strip-unneeded" CACHE STRING "Command line to pass to the strip utility after the debug symbols have been moved out to the debug file")
string(REPLACE " " ";" SYMBOLS_STRIP_FLAGS ${SYMBOLS_STRIP})

if(GIBBON_NICE_THREADS)
  add_definitions(-DGIBBON_NICE_THREADS)
endif()

if(NRDP_CRASH_REPORTING_BREAKPAD)
    gibbon_add_libraries(breakpad_client)
endif()


set(GIBBON_MEMANALYZER "no" CACHE STRING "Build with memanalyzer: 'no', 'static', 'shared'")

if(NOT BUILD_GIBBON_DIRECTORY)
    set(BUILD_GIBBON_DIRECTORY "${NRDP_ROOT_DIR}/gibbondir" CACHE PATH "[-gibbondir] Additional directory where Gibbon source files can be located")
endif()

# Gibbon options, can be overriden by a platform.cmake file
set(GIBBON_INPUT "auto" CACHE STRING "Gibbon input driver: 'auto' to let the platform select")
set(GIBBON_GRAPHICS "directfb" CACHE STRING "Gibbon graphics driver: directfb, gles2[-egl], null")
set(GIBBON_SCRIPT "jsc" CACHE STRING "Script: jsc")
set(GIBBON_PLATFORM "auto" CACHE STRING "Platform: posix")
set(GIBBON_LIBS "")
set(GIBBON_SPLASHSCREEN ON)
set(GIBBON_OEM_XML "${CMAKE_CURRENT_SOURCE_DIR}/resources/configuration/oem.xml")
set(GIBBON_IMAGE_PNG "auto" CACHE STRING "Use Gibbon PNG image loader: yes, no, auto")
set(GIBBON_IMAGE_JPEG "auto" CACHE STRING "Use Gibbon JPEG image loader: yes, no, auto")
set(GIBBON_IMAGE_MNG "auto" CACHE STRING "Use Gibbon MNG image loader: yes, no, auto")
set(GIBBON_IMAGE_WEBP "auto" CACHE STRING "Use Gibbon WEBP image loader: yes, no, auto")
set(GIBBON_MODE "executable" CACHE STRING "Gibbon link mode: executable,shared, static")
set(GIBBON_IMAGE_JPEG_METHOD "fast" CACHE STRING "JPEG DCT Method: fast, slow, float")
set(GIBBON_IMAGE_JPEG_UPSAMPLE "no" CACHE STRING "JPEG Fancy Upsample: yes, no")
set(GIBBON_LIBEDIT "auto" CACHE STRING "Use libedit if available: yes, no, auto")
set(GIBBON_SIGNAL_HANDLER "auto" CACHE STRING "Enable signal handlers if available: yes, no, auto")
set(GIBBON_SURFACEDECODER_FORK "auto" CACHE STRING "Use processes when surface decoding")

if (GIBBON_PLATFORM STREQUAL "auto")
        set(GIBBON_PLATFORM "posix")
endif()


# Import any predefined NetflixBridge JS files.
gibbon_add_js("${NETFLIXBRIDGE_JS_FILES}")

# -----------------------------------------------------------------------------------------------------------

message(" ")
message("=============================================================")
message(" Gibbon configuration:")
message("-------------------------------------------------------------")

create_data_directory(gibbon)

# Baked-in error page
# -----------------------------------------------------------------------------------------------------------
if(NOT EXISTS "${GIBBON_ERROR}")
    if(NOT IS_ABSOLUTE "${GIBBON_ERROR}")
        set(GIBBON_ERROR_FILE "${CMAKE_CURRENT_SOURCE_DIR}/resources/js/${GIBBON_ERROR}")
        if(NOT EXISTS "${GIBBON_ERROR_FILE}")
            message(FATAL_ERROR "Can't find the baked-in error page ${GIBBON_ERROR}")
        endif()
    endif()
else()
    set(GIBBON_ERROR_FILE "${GIBBON_ERROR}")
endif()

message(STATUS "Baked-in error page: ${GIBBON_ERROR_FILE}")
message(STATUS "Memanalyzer support: ${GIBBON_MEMANALYZER}")

# Platform
# -----------------------------------------------------------------------------------------------------------
message("  - Platform: ${GIBBON_PLATFORM}")
add_definitions(-DGIBBON_PLATFORM="${GIBBON_PLATFORM}")
gibbon_add_directory(NAME ${GIBBON_PLATFORM} PREFIX platform FILE platform.cmake VARIABLE GIBBON_PLATFORM_DIRECTORY)

if(GIBBON_TILEMANAGER)
    gibbon_add_libraries(nrdil)
    gibbon_add_sources(bridge/TileManagerBridge.cpp)
    gibbon_add_js(resources/js/tilemanager.js)
    copy_resource(gibbon ${CMAKE_CURRENT_SOURCE_DIR}/resources/js/tileviewer.js ${APP_DATA_DIR}/resources/js/)
    copy_resource(gibbon ${CMAKE_CURRENT_SOURCE_DIR}/resources/configuration/tilemanager.xml ${APP_DATA_DIR}/etc/conf/tilemanager.xml)
endif()
message(STATUS "Gibbon Tile Manager: ${GIBBON_TILEMANAGER}")

if(GIBBON_SPLASHSCREEN)
    gibbon_add_sources(SplashWidget.h SplashWidget.cpp)
endif()

set(FORK_TEST "
    #include <unistd.h>

    int main()
    {
        pid_t p = fork();
        return 0;
    }
    ")
check_c_source_compiles("${FORK_TEST}" HAVE_FORK)
if (GIBBON_SURFACEDECODER_FORK STREQUAL "auto")
   set(GIBBON_SURFACEDECODER_FORK ${HAVE_FORK})
endif()

set(MALLINFO_TEST "
    #include <malloc.h>

    int main()
    {
        struct mallinfo info;
        info = mallinfo();
        (void)info;
        return 0;
    }
    ")
check_c_source_compiles("${MALLINFO_TEST}" HAVE_MALLINFO)

set(MALLOC_TRIM_TEST "
    #include <malloc.h>

#ifdef __UCLIBC__
#error malloc_trim() is not thread-safe in uClibc!
#endif


    int main()
    {
        malloc_trim(0);
        return 0;
    }
    ")
check_c_source_compiles("${MALLOC_TRIM_TEST}" HAVE_MALLOC_TRIM)

set(MALLOC_USABLE_SIZE_TEST "
    #include <malloc.h>

    int main()
    {
        return malloc_usable_size(0);
    }
    ")
check_c_source_compiles("${MALLOC_USABLE_SIZE_TEST}" HAVE_MALLOC_USABLE_SIZE)

if(GIBBON_TCMALLOC)
    set(TCMALLOC_LIBRARY tcmalloc)
    set(TCMALLOC_TEST "
        #include <gperftools/malloc_extension.h>

        int main()
        {
            MallocExtension* ext = MallocExtension::instance();
            size_t allocated;
            ext->GetNumericProperty(\"generic.heap_size\", &allocated);
        }
        ")
    set(CMAKE_REQUIRED_LIBRARIES ${TCMALLOC_LIBRARY})
    check_cxx_source_compiles("${TCMALLOC_TEST}" HAVE_GIBBON_TCMALLOC)

    if (NOT HAVE_GIBBON_TCMALLOC)
        set(TCMALLOC_LIBRARY tcmalloc_minimal)
        set(CMAKE_REQUIRED_LIBRARIES "${TCMALLOC_LIBRARY};pthread")
        unset(HAVE_GIBBON_TCMALLOC CACHE)
        check_cxx_source_compiles("${TCMALLOC_TEST}" HAVE_GIBBON_TCMALLOC)
    endif()
else()
    set(HAVE_GIBBON_TCMALLOC) # unset
endif()

# Third party libraries

# -----------------------------------------------------------------------------------------------------------
# libjpeg
if(NOT HAVE_LIBJPEG AND (GIBBON_IMAGE_JPEG STREQUAL "auto" OR GIBBON_IMAGE_JPEG))
    set(CMAKE_REQUIRED_LIBRARIES "jpeg")
    set(LIBJPEG_TEST "
        #include <stdlib.h>
        #include <stddef.h>
        #include <stdio.h>
        #include <jpeglib.h>

        int main()
        {
            struct jpeg_decompress_struct cinfo;
            jpeg_start_decompress(&cinfo);
            return 0;
        }
     ")
    check_c_source_compiles("${LIBJPEG_TEST}" HAVE_LIBJPEG)

    if (NOT HAVE_LIBJPEG AND GIBBON_IMAGE_JPEG STREQUAL "auto")
        message(FATAL_ERROR "ERROR: libjpeg was not detected")
    elseif(NOT HAVE_LIBJPEG)
        message(STATUS "WARNING: libjpeg was not detected")
    endif()
elseif(NOT HAVE_LIBJPEG)
    set(HAVE_LIBJPEG FALSE)
endif()

if(GIBBON_IMAGE_JPEG_UPSAMPLE STREQUAL "yes")
    set(GIBBON_JPEG_UPSAMPLE TRUE)
endif()

if(GIBBON_IMAGE_JPEG_METHOD STREQUAL "slow")
    set(GIBBON_JPEG_SLOW TRUE)
elseif(GIBBON_IMAGE_JPEG_METHOD STREQUAL "float")
    set(GIBBON_JPEG_FLOAT TRUE)
endif()

# -----------------------------------------------------------------------------------------------------------
# libpng
if(NOT HAVE_LIBPNG AND (GIBBON_IMAGE_PNG STREQUAL "auto" OR GIBBON_IMAGE_PNG))
    set(CMAKE_REQUIRED_LIBRARIES "png;z;m")
    set(LIBPNG_TEST "
        #include <stdlib.h>
        #include <png.h>

        int main()
        {
            png_structp png_ptr;
            png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
            png_destroy_read_struct(&png_ptr, 0, 0);
            return 0;
        }
    ")
    check_c_source_compiles("${LIBPNG_TEST}" HAVE_LIBPNG)

    if (NOT HAVE_LIBPNG AND GIBBON_IMAGE_PNG STREQUAL "auto")
        message(FATAL_ERROR "ERROR: libpng was not detected")
    elseif(NOT HAVE_LIBPNG)
        message(STATUS "WARNING: libpng was not detected")
    endif()
elseif(NOT HAVE_LIBPNG)
    set(HAVE_LIBPNG FALSE)
endif()


# -----------------------------------------------------------------------------------------------------------
# libmng
if(NOT HAVE_LIBMNG AND (GIBBON_IMAGE_MNG STREQUAL "auto" OR GIBBON_IMAGE_MNG))
    set(CMAKE_REQUIRED_LIBRARIES "mng;z;jpeg;m")
    set(LIBMNG_TEST "
        #define MNG_USE_SO
        #include <libmng.h>

        int main()
        {
            mng_handle mng = mng_initialize(0, 0, 0, 0);
            mng_cleanup(&mng);
            return 0;
        }
    ")
    check_c_source_compiles("${LIBMNG_TEST}" HAVE_LIBMNG)

    if (NOT HAVE_LIBMNG AND GIBBON_IMAGE_MNG STREQUAL "auto")
        message(FATAL_ERROR "ERROR: libmng was not detected")
    elseif(NOT HAVE_LIBMNG)
        message(STATUS "WARNING: libmng was not detected")
    endif()
elseif(NOT HAVE_LIBMNG)
    set(HAVE_LIBMNG FALSE)
endif()

# -----------------------------------------------------------------------------------------------------------
# libedit

if(GIBBON_LIBEDIT STREQUAL "auto" OR GIBBON_LIBEDIT)
  set(CMAKE_REQUIRED_LIBRARIES "edit;tinfo")
  set(LIBEDIT_TEST "
            #include <histedit.h>
            int main()
            {
                EditLine *el = el_init(0, 0, 0, 0);
                (void)el;
                return 0;
            }
        ")
  check_c_source_compiles("${LIBEDIT_TEST}" HAVE_LIBEDIT)

  if(NOT HAVE_LIBEDIT AND GIBBON_LIBEDIT STREQUAL "yes")
    message(FATAL_ERROR "ERROR: libedit was not detected")
  elseif(NOT HAVE_LIBEDIT)
    message(STATUS "WARNING: libedit was not detected")
  else()
    gibbon_add_libraries(edit tinfo)
  endif()
else()
  set(HAVE_LIBEDIT FALSE)
endif()


if(HAVE_LIBEDIT)
    list(APPEND GIBBON_LIBS edit tinfo)
endif()

# -----------------------------------------------------------------------------------------------------------
# libwebp
if(NOT HAVE_LIBWEBP AND (GIBBON_IMAGE_WEBP STREQUAL "auto" OR GIBBON_IMAGE_WEBP))
    set(CMAKE_REQUIRED_LIBRARIES "webp;m")
    set(LIBWEBP_TEST "
        #include <webp/decode.h>
        int main()
        {
            int width, height;
            WebPGetInfo((const uint8_t*)0, 0, &width, &height);
            return 0;
        }
    ")
    check_c_source_compiles("${LIBWEBP_TEST}" HAVE_LIBWEBP)

    if (NOT HAVE_LIBWEBP AND GIBBON_IMAGE_WEBP STREQUAL "auto")
        message(FATAL_ERROR "ERROR: libwebp was not detected")
    elseif(NOT HAVE_LIBWEBP)
        message(STATUS "WARNING: libwebp was not detected")
    endif()

    # check if we have the premultiplied colorspace
    set(LIBWEBP_PREMUL_TEST "
    #include <webp/decode.h>
    int main()
    {
        WebPDecoderConfig config;
        config.output.colorspace = MODE_Argb;
        (void)config;
        return 0;
    }
    ")
    check_c_source_compiles("${LIBWEBP_PREMUL_TEST}" HAVE_LIBWEBP_PREMUL)
elseif(NOT HAVE_LIBWEBP)
    set(HAVE_LIBWEBP FALSE)
endif()


# Image loaders
# -----------------------------------------------------------------------------------------------------------
message("  - Image Loaders:")

if(HAVE_LIBMNG)
    message("    + MNG")
    gibbon_add_sources(SurfaceDecoderMNG.cpp)
    gibbon_add_libraries(mng)
endif()

if(HAVE_LIBWEBP)
    message("    + WebP")
    gibbon_add_sources(SurfaceDecoderWebP.cpp)
    gibbon_add_libraries(webp)
endif()

if(HAVE_LIBPNG)
    message("    + PNG")
    gibbon_add_sources(SurfaceDecoderPNG.cpp)
    gibbon_add_libraries(png)
endif()

if(HAVE_LIBJPEG)
    message("    + JPEG")
    gibbon_add_sources(SurfaceDecoderJPEG.cpp)
    gibbon_add_libraries(jpeg)
endif()


# Signal handlers
# -----------------------------------------------------------------------------------------------------------
if (NOT (GIBBON_SIGNAL_HANDLERS STREQUAL "auto"))
    set(SIGNAL_TEST "
        #include <signal.h>

        void nf_signalHandler(int signal) { (void)signal; }

        int main()
        {
            signal(SIGSEGV, nf_signalHandler);
            signal(SIGBUS, nf_signalHandler);
            signal(SIGABRT, nf_signalHandler);
            signal(SIGINT, nf_signalHandler);
            return 0;
        }
    ")
    check_c_source_compiles("${SIGNAL_TEST}" HAVE_SIGNAL_HANDLERS)
    message("  - Signal Handlers: ${HAVE_SIGNAL_HANDLERS}")
endif()

# SplashScreen
# -----------------------------------------------------------------------------------------------------------
message("  - Splash screen: ${GIBBON_SPLASHSCREEN}")

# ScriptEngine
# -----------------------------------------------------------------------------------------------------------
message("  - Script engine: ${GIBBON_SCRIPT}")
gibbon_add_directory(NAME "${GIBBON_SCRIPT}" PREFIX scriptengine)

# Freetype
# -----------------------------------------------------------------------------------------------------------
gibbon_add_libraries(freetype)
gibbon_add_libraries(harfbuzz graphite2)
include_directories(text/freetype "${TOOLCHAIN_DIRECTORY}/netflix/include/harfbuzz")
gibbon_add_sources(
    text/freetype/GibbonFreetype.cpp
    text/freetype/GibbonHarfbuzz.cpp
    text/freetype/FontFreetype.cpp
)

# Graphics backend
# -----------------------------------------------------------------------------------------------------------
message("  - Graphics backend: ${GIBBON_GRAPHICS}")
gibbon_add_directory(NAME "${GIBBON_GRAPHICS}" PREFIX graphics)

# Input backend
# -----------------------------------------------------------------------------------------------------------
if (GIBBON_INPUT STREQUAL "auto")
   set(GIBBON_INPUT "null")
endif()
message("  - Input backend: ${GIBBON_INPUT}")
gibbon_add_directory(NAME "${GIBBON_INPUT}" PREFIX input)

message("")
get_directory_property(GIBBON_INCLUDES INCLUDE_DIRECTORIES)
#message("  - Gibbon includes: ${GIBBON_INCLUDES}")
message("  - Gibbon link mode: ${GIBBON_MODE}")

if(NOT GIBBON_BUILD_ID)
    string(RANDOM LENGTH 32 ALPHABET "1234567890ABCDEF" RANDOM_STRING)
    set(GIBBON_BUILD_ID ${RANDOM_STRING} CACHE STRING "Gibbon Build ID")
endif()
message("  - Gibbon build id: ${GIBBON_BUILD_ID}")

configure_file_if_different("config.h.in" "${CMAKE_CURRENT_BINARY_DIR}/include/gibbon/config.h")
include_directories("${CMAKE_CURRENT_BINARY_DIR}/include")

message("=============================================================")
message(" ")

gibbon_get_sources(SOURCES)
gibbon_get_includes(GIBBON_INCLUDES)
gibbon_get_libraries(GIBBON_LIBS)
gibbon_get_js(NETFLIXBRIDGE_JS_FILES)

include(${COMMON_DIRECTORY}/js/js.cmake)
create_data_js_directory(gibbon)

# Give the platform a chance to override the build command
if(COMMAND gibbon_build)
    gibbon_build()
else()
    if(GIBBON_MODE STREQUAL "executable")
        add_definitions(-DGIBBON_MAIN)
        add_executable(gibbon ${SOURCES})

        if(NRDP_HAS_SOCKETIZER)
            target_link_libraries(gibbon nrdsocketizer)
        endif()

        if(DEFINED GIBBON_MEMANALYZER AND NOT GIBBON_MEMANALYZER STREQUAL "no")
            message("INCLUDING MEMANALYZER!!!")
            include(memanalyzer/CMakeLists.txt)
        endif()

        if(GIBBON_TCMALLOC)
            if(NOT HAVE_GIBBON_TCMALLOC)
                message("tcmalloc/gperftools not detected")
            else()
                target_link_libraries(gibbon ${TCMALLOC_LIBRARY})
            endif()
        endif()

        target_link_libraries(gibbon ${GIBBON_LIBS} nrd nrddpi pthread)

    elseif(GIBBON_MODE STREQUAL "shared")
        add_library(gibbon SHARED ${SOURCES})
        target_link_libraries(gibbon ${GIBBON_LIBS} nrd nrddpi pthread)

    elseif(GIBBON_MODE STREQUAL "static")
        add_library(gibbon STATIC ${SOURCES})
    else()
        message(FATAL_ERROR "Unknown gibbon link mode '${GIBBON_MODE}'")
    endif()
    if(NOT NRDP_PLATFORM_NOVA)
        set_target_properties(gibbon PROPERTIES OUTPUT_NAME "netflix")
    endif()
    set_property(TARGET gibbon APPEND PROPERTY INCLUDE_DIRECTORIES ${GIBBON_INCLUDES})

    if(GIBBON_MODE STREQUAL "executable")
        gibbon_get_install_libraries(GIBBON_INSTALL_LIBRARIES)

        create_install_target(
            TARGET gibbon
            DESCRIPTION "Application"
            COMPONENT "NRDP"
            RESOURCES ${APP_DATA_DIR}
            LIBRARIES ${GIBBON_INSTALL_LIBRARIES}
        )
    endif()

endif()

gibbon_get_dependencies(GIBBON_DEPENDENCIES)
if(NOT "${GIBBON_DEPENDENCIES}" STREQUAL "")
    add_dependencies(gibbon ${GIBBON_DEPENDENCIES})
endif()

copy_resource(gibbon ${CMAKE_CURRENT_SOURCE_DIR}/resources/configuration/gibbon.xml ${APP_DATA_DIR}/etc/conf/gibbon.xml)
copy_resource(gibbon ${CMAKE_CURRENT_SOURCE_DIR}/resources/configuration/rs-server.xml ${APP_DATA_DIR}/etc/conf/rs-server.xml NON_COMPILABLE)
copy_resource(gibbon ${CMAKE_CURRENT_SOURCE_DIR}/resources/configuration/rs-client.xml ${APP_DATA_DIR}/etc/conf/rs-client.xml NON_COMPILABLE)
copy_resource(gibbon ${CMAKE_CURRENT_SOURCE_DIR}/resources/configuration/test.xml ${APP_DATA_DIR}/etc/conf/test.xml NON_COMPILABLE)
copy_resource(gibbon ${CMAKE_CURRENT_SOURCE_DIR}/resources/configuration/stress.xml ${APP_DATA_DIR}/etc/conf/stress.xml NON_COMPILABLE)
copy_resource(gibbon ${CMAKE_CURRENT_SOURCE_DIR}/resources/configuration/1080.xml ${APP_DATA_DIR}/etc/conf/1080.xml DEBUG)
copy_resource(gibbon ${GIBBON_OEM_XML} ${APP_DATA_DIR}/etc/conf/oem.xml)
copy_resource(gibbon ${CMAKE_CURRENT_SOURCE_DIR}/resources/js/splash.js ${APP_DATA_DIR}/resources/js/)
copy_resource(gibbon ${CMAKE_CURRENT_SOURCE_DIR}/resources/js/error.js ${APP_DATA_DIR}/resources/js/ NON_COMPILABLE)
copy_resource(gibbon ${GIBBON_ERROR_FILE} ${APP_DATA_DIR}/resources/js/error.js NON_COMPILABLE)
copy_resource(gibbon ${CMAKE_CURRENT_SOURCE_DIR}/resources/js/error-text/* ${APP_DATA_DIR}/resources/js/error-text/ NON_COMPILABLE)
copy_resource(gibbon ${CMAKE_CURRENT_SOURCE_DIR}/resources/js/tests/* ${APP_DATA_DIR}/resources/js/tests/ DEBUG)
copy_resource(gibbon ${CMAKE_CURRENT_SOURCE_DIR}/resources/js/inject/* ${APP_DATA_DIR}/resources/js/inject/ DEBUG)

# Handle the Partner Bridge
gibbon_get_partnerbridge(GIBBON_PARTNERBRIDGE)
if(DEFINED GIBBON_PARTNERBRIDGE) # Platform supplied set of Partner Bridge file(s)
    message("Using a platform supplied Partner Bridge: ${GIBBON_PARTNERBRIDGE}")
    foreach(bridge ${GIBBON_PARTNERBRIDGE})
        if(EXISTS "${bridge}")
            copy_resource(gibbon "${bridge}" ${APP_DATA_DIR}/resources/js/)
        else()
            message(FATAL_ERROR "Platform Partner Bridge file not found: ${bridge}")
        endif()
    endforeach()
else()
    if(EXISTS "${GIBBON_PLATFORM_DIRECTORY}/PartnerBridge.js") # Override
        message("Using custom PartnerBridge.js")
        copy_resource(gibbon ${GIBBON_PLATFORM_DIRECTORY}/PartnerBridge.js ${APP_DATA_DIR}/resources/js/)
    else() # Default
        message("Using the default PartnerBridge.js")
        copy_resource(gibbon ${CMAKE_CURRENT_SOURCE_DIR}/resources/default/PartnerBridge.js ${APP_DATA_DIR}/resources/js/)
    endif()
endif()

# Platform.xml
gibbon_get_platformxml(GIBBON_PLATFORMXML)
if(DEFINED GIBBON_PLATFORMXML)
    message("Using a platform supplied Platform.xml: ${GIBBON_PLATFORMXML}")
    foreach(platformx ${GIBBON_PLATFORMXML})
        if(EXISTS "${platformx}")
            copy_resource(gibbon "${platformx}" ${APP_DATA_DIR}/etc/conf/)
        else()
            message(FATAL_ERROR "Custom platform.xml file not found: ${platformx}")
        endif()
    endforeach()
else()
    if(EXISTS "${GIBBON_PLATFORM_DIRECTORY}/platform.xml") # Override
        message("Using custom platform.xml")
        copy_resource(gibbon ${GIBBON_PLATFORM_DIRECTORY}/platform.xml ${APP_DATA_DIR}/etc/conf/)
    else() # Default
        if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/resources/configuration/platform.xml")
            message("Using the default platform.xml file")
            copy_resource(gibbon ${CMAKE_CURRENT_SOURCE_DIR}/resources/configuration/platform.xml ${APP_DATA_DIR}/etc/conf/platform.xml)
        else()
            message("Not using a platform.xml file")
        endif()
    endif()
endif()

# Copy appboot keys
gibbon_get_keys(GIBBON_KEYS)
if(DEFINED GIBBON_KEYS) # The platform supplies its own set of keys
    message("Using platform supplied keys: ${GIBBON_KEYS}")
    foreach(key ${GIBBON_KEYS})
        if(EXISTS "${key}")
            copy_resource(gibbon "${key}" ${APP_DATA_DIR}/etc/keys/)
        else()
            message(FATAL_ERROR "Platform Key file not found: ${key}")
        endif()
    endforeach()
else()
    if(EXISTS "${GIBBON_PLATFORM_DIRECTORY}/appboot.key") # Override
        message("Using custom appboot.key")
        copy_resource(gibbon ${GIBBON_PLATFORM_DIRECTORY}/appboot.key ${APP_DATA_DIR}/etc/keys/)
    else() # Default
        message("Using the default keys")
        copy_resource(gibbon ${NRDP_ROOT_DIR}/resources/etc/keys/appboot.key.sdk ${APP_DATA_DIR}/etc/keys/appboot.key.sdk)
        copy_resource(gibbon ${NRDP_ROOT_DIR}/resources/etc/keys/appboot.key ${APP_DATA_DIR}/etc/keys/appboot.key)
    endif()
endif()

copy_resource(gibbon ${CMAKE_CURRENT_SOURCE_DIR}/resources/img/* ${APP_DATA_DIR}/resources/img/ NON_COMPILABLE)
copy_resource(gibbon ${CMAKE_CURRENT_SOURCE_DIR}/resources/wav/* ${APP_DATA_DIR}/resources/wav/ NON_COMPILABLE)
copy_resource(gibbon ${CMAKE_CURRENT_SOURCE_DIR}/resources/html/* ${APP_DATA_DIR}/resources/html/ NON_COMPILABLE)
copy_resource(gibbon ${CMAKE_CURRENT_SOURCE_DIR}/resources/js/stats.js ${APP_DATA_DIR}/resources/js/ NON_COMPILABLE)

copy_resource(gibbon ${CMAKE_CURRENT_SOURCE_DIR}/resources/gibbon/fonts/fonts.xml ${APP_DATA_DIR}/fonts/)


#Fonts
#-----------------------------------------------------------------------------------------------------
# Arial
copy_resource(gibbon ${CMAKE_CURRENT_SOURCE_DIR}/resources/gibbon/fonts/Arial_for_Netflix-R.ttf "${APP_DATA_DIR}/fonts/" NON_COMPILABLE)
copy_resource(gibbon ${CMAKE_CURRENT_SOURCE_DIR}/resources/gibbon/fonts/Arial_for_Netflix-B.ttf "${APP_DATA_DIR}/fonts/" NON_COMPILABLE)
copy_resource(gibbon ${CMAKE_CURRENT_SOURCE_DIR}/resources/gibbon/fonts/Arial_for_Netflix-I.ttf "${APP_DATA_DIR}/fonts/" NON_COMPILABLE)

# CC Fonts
copy_resource(gibbon ${CMAKE_CURRENT_SOURCE_DIR}/resources/gibbon/fonts/ScreenSansMonoForNetflix.ttf "${APP_DATA_DIR}/fonts/" NON_COMPILABLE)
copy_resource(gibbon ${CMAKE_CURRENT_SOURCE_DIR}/resources/gibbon/fonts/ScreenSerifForNetflix.ttf "${APP_DATA_DIR}/fonts/" NON_COMPILABLE)
copy_resource(gibbon ${CMAKE_CURRENT_SOURCE_DIR}/resources/gibbon/fonts/ScreenSerifMonoForNetflix.ttf "${APP_DATA_DIR}/fonts/" NON_COMPILABLE)
copy_resource(gibbon ${CMAKE_CURRENT_SOURCE_DIR}/resources/gibbon/fonts/PlateGothicForNetflix.ttf "${APP_DATA_DIR}/fonts/" NON_COMPILABLE)
copy_resource(gibbon ${CMAKE_CURRENT_SOURCE_DIR}/resources/gibbon/fonts/AshleyScriptForNetflix.ttf "${APP_DATA_DIR}/fonts/" NON_COMPILABLE)
copy_resource(gibbon ${CMAKE_CURRENT_SOURCE_DIR}/resources/gibbon/fonts/FloridianScriptForNetflix.ttf "${APP_DATA_DIR}/fonts/" NON_COMPILABLE)

# Japanese
copy_resource(gibbon ${CMAKE_CURRENT_SOURCE_DIR}/resources/gibbon/fonts/HeiseiMaruGoth213_CForNetflix.ttf "${APP_DATA_DIR}/fonts/" NON_COMPILABLE)

# Korean
copy_resource(gibbon ${CMAKE_CURRENT_SOURCE_DIR}/resources/gibbon/fonts/YDGothic120ProForNetflix.ttf "${APP_DATA_DIR}/fonts/" NON_COMPILABLE)

# Traditional Chinese
copy_resource(gibbon ${CMAKE_CURRENT_SOURCE_DIR}/resources/gibbon/fonts/MHeiM-B5HK_CForNetflix.ttf "${APP_DATA_DIR}/fonts/" NON_COMPILABLE)

# Simplified Chinese
copy_resource(gibbon ${CMAKE_CURRENT_SOURCE_DIR}/resources/gibbon/fonts/MHeiM18030_CForNetflix.ttf "${APP_DATA_DIR}/fonts/" NON_COMPILABLE)


# Armenian
copy_resource(gibbon ${CMAKE_CURRENT_SOURCE_DIR}/resources/gibbon/fonts/HandsetSansArmenian-Regular.ttf "${APP_DATA_DIR}/fonts/" NON_COMPILABLE)
copy_resource(gibbon ${CMAKE_CURRENT_SOURCE_DIR}/resources/gibbon/fonts/HandsetSansArmenian-Bold.ttf "${APP_DATA_DIR}/fonts/" NON_COMPILABLE)

# Georgian
copy_resource(gibbon ${CMAKE_CURRENT_SOURCE_DIR}/resources/gibbon/fonts/Georgian_for_Netflix-R.ttf "${APP_DATA_DIR}/fonts/" NON_COMPILABLE)
copy_resource(gibbon ${CMAKE_CURRENT_SOURCE_DIR}/resources/gibbon/fonts/Georgian_for_Netflix-B.ttf "${APP_DATA_DIR}/fonts/" NON_COMPILABLE)

# Thai
copy_resource(gibbon ${CMAKE_CURRENT_SOURCE_DIR}/resources/gibbon/fonts/HandsetSansThai-Regular.ttf "${APP_DATA_DIR}/fonts/" NON_COMPILABLE)
copy_resource(gibbon ${CMAKE_CURRENT_SOURCE_DIR}/resources/gibbon/fonts/HandsetSansThai-Bold.ttf "${APP_DATA_DIR}/fonts/" NON_COMPILABLE)


# Devanagari
copy_resource(gibbon ${CMAKE_CURRENT_SOURCE_DIR}/resources/gibbon/fonts/HandsetSansDevanagari-Regular.ttf "${APP_DATA_DIR}/fonts/" NON_COMPILABLE)
copy_resource(gibbon ${CMAKE_CURRENT_SOURCE_DIR}/resources/gibbon/fonts/HandsetSansDevanagari-Bold.ttf "${APP_DATA_DIR}/fonts/" NON_COMPILABLE)

# Bengali
copy_resource(gibbon ${CMAKE_CURRENT_SOURCE_DIR}/resources/gibbon/fonts/HandsetSansBengali-Regular.ttf "${APP_DATA_DIR}/fonts/" NON_COMPILABLE)
copy_resource(gibbon ${CMAKE_CURRENT_SOURCE_DIR}/resources/gibbon/fonts/HandsetSansBengali-Bold.ttf "${APP_DATA_DIR}/fonts/" NON_COMPILABLE)

# Tamil
copy_resource(gibbon ${CMAKE_CURRENT_SOURCE_DIR}/resources/gibbon/fonts/HandsetSansTamil-Regular.ttf "${APP_DATA_DIR}/fonts/" NON_COMPILABLE)
copy_resource(gibbon ${CMAKE_CURRENT_SOURCE_DIR}/resources/gibbon/fonts/HandsetSansTamil-Bold.ttf "${APP_DATA_DIR}/fonts/" NON_COMPILABLE)

# Telugu
copy_resource(gibbon ${CMAKE_CURRENT_SOURCE_DIR}/resources/gibbon/fonts/HandsetSansTelugu-Regular.ttf "${APP_DATA_DIR}/fonts/" NON_COMPILABLE)
copy_resource(gibbon ${CMAKE_CURRENT_SOURCE_DIR}/resources/gibbon/fonts/HandsetSansTelugu-Bold.ttf "${APP_DATA_DIR}/fonts/" NON_COMPILABLE)

# Gujarati
copy_resource(gibbon ${CMAKE_CURRENT_SOURCE_DIR}/resources/gibbon/fonts/HandsetSansGujarati-Regular.ttf "${APP_DATA_DIR}/fonts/" NON_COMPILABLE)
copy_resource(gibbon ${CMAKE_CURRENT_SOURCE_DIR}/resources/gibbon/fonts/HandsetSansGujarati-Bold.ttf "${APP_DATA_DIR}/fonts/" NON_COMPILABLE)

# Kannada
copy_resource(gibbon ${CMAKE_CURRENT_SOURCE_DIR}/resources/gibbon/fonts/HandsetSansKannada-Regular.ttf "${APP_DATA_DIR}/fonts/" NON_COMPILABLE)
copy_resource(gibbon ${CMAKE_CURRENT_SOURCE_DIR}/resources/gibbon/fonts/HandsetSansKannada-Bold.ttf "${APP_DATA_DIR}/fonts/" NON_COMPILABLE)

# Malayalam
copy_resource(gibbon ${CMAKE_CURRENT_SOURCE_DIR}/resources/gibbon/fonts/HandsetSansMalayalam-Regular.ttf "${APP_DATA_DIR}/fonts/" NON_COMPILABLE)
copy_resource(gibbon ${CMAKE_CURRENT_SOURCE_DIR}/resources/gibbon/fonts/HandsetSansMalayalam-Bold.ttf "${APP_DATA_DIR}/fonts/" NON_COMPILABLE)

# Gurmukhi
copy_resource(gibbon ${CMAKE_CURRENT_SOURCE_DIR}/resources/gibbon/fonts/HandsetSansGurmukhi-Regular.ttf "${APP_DATA_DIR}/fonts/" NON_COMPILABLE)
copy_resource(gibbon ${CMAKE_CURRENT_SOURCE_DIR}/resources/gibbon/fonts/HandsetSansGurmukhi-Bold.ttf "${APP_DATA_DIR}/fonts/" NON_COMPILABLE)

# Oriya
copy_resource(gibbon ${CMAKE_CURRENT_SOURCE_DIR}/resources/gibbon/fonts/HandsetSansOriya-Regular.ttf "${APP_DATA_DIR}/fonts/" NON_COMPILABLE)
copy_resource(gibbon ${CMAKE_CURRENT_SOURCE_DIR}/resources/gibbon/fonts/HandsetSansOriya-Bold.ttf "${APP_DATA_DIR}/fonts/" NON_COMPILABLE)

#-----------------------------------------------------------------------------------------------------

# always copy boot.js out of resources; people who are testing it should use -0.
set(GIBBON_BOOT_FILE "${CMAKE_CURRENT_SOURCE_DIR}/resources/js/boot.js")
message(STATUS "Baked-in appboot client: ${GIBBON_BOOT_FILE}")
copy_resource(gibbon ${GIBBON_BOOT_FILE} ${APP_DATA_DIR}/resources/js/boot.js)

# ICU resources
if(NRDP_BIG_ENDIAN)
    set(ICU_ENDIAN "b")
else()
    set(ICU_ENDIAN "l")
endif()
copy_resource(gibbon "${CMAKE_CURRENT_SOURCE_DIR}/resources/gibbon/icu/icudt54${ICU_ENDIAN}/*" "${APP_DATA_DIR}/icu/icudt54${ICU_ENDIAN}/" NON_COMPILABLE)
copy_resource(gibbon "${CMAKE_CURRENT_SOURCE_DIR}/resources/gibbon/icu/icudt54${ICU_ENDIAN}/brkitr/*" "${APP_DATA_DIR}/icu/icudt54${ICU_ENDIAN}/brkitr/" NON_COMPILABLE)
copy_resource(gibbon "${CMAKE_CURRENT_SOURCE_DIR}/resources/gibbon/icu/icu_rules.txt" "${APP_DATA_DIR}/icu/" NON_COMPILABLE)


if(BUILD_NRDSYSTEM STREQUAL "linux")
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/netflix.supp DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/.valgrindrc DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endif()

create_resources_library(gibbon)

if(BUILD_NRDSYSTEM STREQUAL "linux")
    if(CMAKE_BUILD_TYPE STREQUAL "release")
        message(" ")
        message("=============================================================")
        message("-- Generating binary HASH for Gibbon")
        message("=============================================================")
        # run checksum script after the executable is built
        add_custom_command(
            TARGET gibbon
            POST_BUILD
            COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/../../../scripts/configure/hash.sh ${CMAKE_CURRENT_BINARY_DIR}/hash.txt ${APP_DATA_DIR}/etc/conf/common.xml ${CMAKE_CURRENT_BINARY_DIR}/netflix )
    endif()
endif()

if(GIBBON_NICE_THREADS)
  add_definitions(-DGIBBON_NICE_THREADS)
endif()

# Cotire (COmpile TIme REduction) support - Add a gibbon_unity target that creates a single compilation unit
if(GIBBON_COTIRE)
    cotire(gibbon)
    target_link_libraries(gibbon_unity ${GIBBON_LIBS} nrd nrddpi pthread)
endif()

if(NRDP_CRASH_REPORTING_ENABLED)
    message("Adding symbol stripping commands...")

    if(NOT SYMBOLS_USERNAME OR NOT SYMBOLS_PASSWORD OR NOT SYMBOLS_FOLDER)
        set(SYMBOLS_UPLOAD FALSE)
        message("Symbol uploading disabled")
    else()
        set(SYMBOLS_UPLOAD TRUE)
        message("Symbol uploading enabled")
    endif()

    get_environment_variable(DUMP_SYMS "Path to breakpad's DUMP_SYMS executable" "${TOOLCHAIN_DIRECTORY}/netflix/bin/dump_syms")
    get_environment_variable(STRIP "Path to the strip executable" "strip")
    get_environment_variable(OBJCOPY "Path to the objcopy executable" "objcopy")

    if(GIBBON_MODE STREQUAL "executable")
        set(APPLICATION_EXECUTABLE "netflix")
    elseif(GIBBON_MODE STREQUAL "shared")
        set(APPLICATION_EXECUTABLE "libnetflix.so")
    elseif(GIBBON_MODE STREQUAL "static")
        set(APPLICATION_EXECUTABLE "libnetflix.a")
    endif()

    #-------------------------------------------------------------------------------------------------------------------------
    macro(CREATE_DEBUG_SYMBOLS archive description primary)
        if(${primary})
            add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/netflix.id
                WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                #DEPENDS gibbon
                COMMENT "Exporting debug symbols..."
            )

            if(NRDP_CRASH_REPORTING_INTERNAL)
                add_custom_target(symbols ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/netflix.id gibbon)
            else()
                add_custom_target(symbols DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/netflix.id gibbon)
            endif()
            if(SYMBOLS_UPLOAD)
                add_custom_target(symbols_upload DEPENDS symbols)
            endif()
    endif()

        if(NRDP_CRASH_REPORTING_BREAKPAD)
            add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/netflix.id ${CMAKE_CURRENT_BINARY_DIR}/${archive}.sym
                COMMAND ${CMAKE_COMMAND} -E echo "Processing ${description}..."
                COMMAND ${DUMP_SYMS} ${archive} > ${archive}.sym
                COMMAND head -n1 ${archive}.sym | cut -f4 -d ' ' > ${archive}.id
                COMMAND ${CMAKE_COMMAND} -E echo "Copying symbols..."
                COMMAND mkdir -p ./symbols/${archive}/`cat ${archive}.id`
                COMMAND cp ${archive}.sym ./symbols/${archive}/`cat ${archive}.id`/
                COMMAND ${STRIP} ${SYMBOLS_STRIP_FLAGS} ${archive}
                WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                APPEND
            )
        elseif(NRDP_CRASH_REPORTING_INTERNAL)
            add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/netflix.id ${CMAKE_CURRENT_BINARY_DIR}/${archive}.sym
                COMMAND ${CMAKE_COMMAND} -E echo "Processing ${description}..."
                COMMAND ${OBJCOPY} --only-keep-debug ${archive} ${archive}.sym
                COMMAND ${STRIP} ${SYMBOLS_STRIP_FLAGS} ${archive}
				#HUMAX : mipsel-linux-uclibc-objcopy doesn't support below option.
				#COMMAND ${OBJCOPY} --add-gnu-debuglink=${archive}.sym ${archive}
                COMMAND echo "${GIBBON_BUILD_ID}" > ${archive}.id
                WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                APPEND
            )
        endif()

        if(SYMBOLS_UPLOAD)
            # Compression
            add_custom_command(TARGET symbols_upload
                COMMENT "Compressing ${archive}.sym (${description})..."
                WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                COMMAND gzip < ${CMAKE_CURRENT_BINARY_DIR}/${archive}.sym > ${CMAKE_CURRENT_BINARY_DIR}/${archive}.sym.gz
            )

            # Encryption
            if(NOT "${SYMBOLS_GPG_KEYRING}" STREQUAL "" AND EXISTS "${SYMBOLS_GPG_KEYRING}" AND NOT "${SYMBOLS_GPG_RECIPIENT}" STREQUAL "")
                add_custom_command(TARGET symbols_upload
                    COMMENT "Encrypting ${archive}.sym.gz (${description})..."
                    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                    COMMAND gpg --yes --no-default-keyring --keyring ${SYMBOLS_GPG_KEYRING} --trust-model always -e -r ${SYMBOLS_GPG_RECIPIENT} -z 0 -o ${archive}.sym.gz.pgp ${archive}.sym.gz
                )
                set(${archive}_UPLOAD "${archive}.sym.gz.pgp")
            else()
                set(${archive}_UPLOAD "${archive}.sym.gz")
            endif()

            add_custom_command(TARGET symbols_upload
                COMMENT "Uploading ${${archive}_UPLOAD} (${description})..."
                WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                COMMAND ${CMAKE_COMMAND} -DFILE=${CMAKE_CURRENT_BINARY_DIR}/${${archive}_UPLOAD} -DFOLDER=${SYMBOLS_FOLDER} -DUSER=${SYMBOLS_USERNAME} -DPASSWORD=${SYMBOLS_PASSWORD} -DCURL=${SYMBOLS_CURL} -P "${CMAKE_MODULE_PATH}/upload.cmake"
            )

            set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_MAKE_CLEAN_FILES ${CMAKE_CURRENT_BINARY_DIR}/${archive}.id ${CMAKE_CURRENT_BINARY_DIR}/${archive}.gz.id ${CMAKE_CURRENT_BINARY_DIR}/${archive}.sym ${CMAKE_CURRENT_BINARY_DIR}/${archive}.sym.gz ${CMAKE_CURRENT_BINARY_DIR}/${archive}.sym.gz.pgp)
        endif()

        if(${primary})
            add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/netflix.id
                COMMAND ${CMAKE_COMMAND} -E echo "Creating netflix.id file"
                COMMAND ${CMAKE_COMMAND} -E copy_if_different ${archive}.id netflix.id
                APPEND
            )
            set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_MAKE_CLEAN_FILES ${CMAKE_CURRENT_BINARY_DIR}/netflix.id)
        endif()
    endmacro()
    #-------------------------------------------------------------------------------------------------------------------------

    set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_MAKE_CLEAN_FILES symbols)

    create_debug_symbols(${APPLICATION_EXECUTABLE} "netflix application" TRUE)

    if(GIBBON_SCRIPT_JSC_DYNAMIC)
        create_debug_symbols("libJavaScriptCore.so" "libJavaScriptCore" FALSE)
    endif()

endif()

