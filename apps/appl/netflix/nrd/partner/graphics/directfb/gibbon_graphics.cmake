# (c) 1997-2015 Netflix, Inc.  All content herein is protected by
# U.S. copyright and other applicable intellectual property laws and
# may not be copied without the express permission of Netflix, Inc.,
# which reserves all rights.  Reuse of any of this content for any
# purpose without the permission of Netflix, Inc. is strictly
# prohibited.

set(GIBBON_GRAPHICSENGINE_DIRECTFB TRUE)
include_directories(${TOOLCHAIN_DIRECTORY}/netflix/include/directfb)
add_definitions(-DGIBBON_SURFACEDECODER_PLATFORM)
#add_definitions(-DGIBBON_NO_THREADANIMATION)
#add_definitions(-DGIBBON_SCREEN_FLIPTHREAD)

# insert for HUMAX Model
# add_definitions(-DGIBBON_FULL_SCREEN_VSYNC)
# add_definitions(-DGIBBON_FULL_SCREEN_RENDER)
add_definitions(-DGIBBON_DFB_VALIDATE_SURFACE)
add_definitions(-DGIBBON_SCREEN_FULLRENDER)
add_definitions(-DGIBBON_SCREEN_FLIPVSYNC)
add_definitions(-DGIBBON_DFBSCREEN_LAYER_DOUBLEBUFFER)

gibbon_add_sources(
    GraphicsEngineDFB.cpp
    ScreenDFB.cpp
    SurfaceDFB.cpp
    EffectsDFB.cpp
    FontDFB.cpp
)

if(DPI_IMPLEMENTATION STREQUAL "reference")
    add_definitions(-DGIBBON_VIDEO_PLATFORM)
    gibbon_add_sources(VideoDFB.cpp)
endif()


if(GIBBON_INPUT STREQUAL "auto")
    gibbon_set_input("directfb")
endif()

gibbon_add_libraries(directfb)
gibbon_copy_resource(graphics.xml ${APP_DATA_DIR}/etc/conf/graphics.xml "")
