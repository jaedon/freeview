# (c) 1997-2015 Netflix, Inc.  All content herein is protected by
# U.S. copyright and other applicable intellectual property laws and
# may not be copied without the express permission of Netflix, Inc.,
# which reserves all rights.  Reuse of any of this content for any
# purpose without the permission of Netflix, Inc. is strictly
# prohibited.

set(GIBBON_GRAPHICSENGINE_GLES2 TRUE)
add_definitions(-DGIBBON_SCREEN_FULLRENDER -DGIBBON_COORDINATE_FLOAT -DGIBBON_SURFACE_COPY_PINGPONG -DGIBBON_GRAPHICSENGINE_DRAWTEXT_DL -DGIBBON_SCREEN_FLIPVSYNC)

get_filename_component(CURRENT_DIRECTORY ${CMAKE_CURRENT_LIST_FILE} PATH)
include_directories(${CURRENT_DIRECTORY})

gibbon_add_sources(
    GraphicsEngineGLES2.cpp
    ScreenGLES2.cpp
    SurfaceGLES2.cpp
    EffectsGLES2.cpp
    OpenGLContext.cpp
    OpenGLFBO.cpp
)

set(GIBBON_GRAPHICS_GL_WSYS "auto")
if(GIBBON_GRAPHICS STREQUAL "gles2-glx")
   set(GIBBON_GRAPHICS_GL_WSYS "glx")
elseif(GIBBON_GRAPHICS STREQUAL "gles2-egl")
   set(GIBBON_GRAPHICS_GL_WSYS "egl")
elseif(GIBBON_GRAPHICS STREQUAL "gles2-ios")
   set(GIBBON_GRAPHICS_GL_WSYS "ios")
elseif(GIBBON_GRAPHICS STREQUAL "gles2-osx")
   set(GIBBON_GRAPHICS_GL_WSYS "osx")
elseif(GIBBON_GRAPHICS_GL_WSYS STREQUAL "auto")
   if(IOS)
      set(GIBBON_GRAPHICS_GL_WSYS "ios")
   else()
      set(GIBBON_GRAPHICS_GL_WSYS "glx")
   endif()
endif()
message("    + Using GL: ${GIBBON_GRAPHICS_GL_WSYS}")

if(GIBBON_GRAPHICS_GL_WSYS STREQUAL "glx")
    gibbon_add_sources(OpenGLContextGLX.cpp)

    set(GIBBON_GRAPHICSENGINE_GLES2_GLX TRUE)

    find_package(OpenGL REQUIRED)
    find_package(X11 REQUIRED)

    if(APPLE)
        gibbon_add_libraries(GL ${OPENGL_LIBRARY})
    else()
        gibbon_add_libraries(${OPENGL_gl_LIBRARY})

        add_custom_command(OUTPUT "${CMAKE_BINARY_DIR}/include/GL" "${CMAKE_BINARY_DIR}/include/X11"
            COMMAND ${CMAKE_COMMAND} -E create_symlink "${OPENGL_INCLUDE_DIR}/GL" "${CMAKE_BINARY_DIR}/include/GL"
            COMMAND ${CMAKE_COMMAND} -E create_symlink "${X11_X11_INCLUDE_PATH}/X11" "${CMAKE_BINARY_DIR}/include/X11"
        )

        add_custom_target(gibbon_gl_links DEPENDS "${CMAKE_BINARY_DIR}/include/GL" "${CMAKE_BINARY_DIR}/include/X11")
        gibbon_add_dependencies(gibbon_gl_links)

        # Get the path names to the libraries so we can issue -rpath-link flags for them
        get_filename_component(GL_PATH ${OPENGL_gl_LIBRARY} PATH)
        list(APPEND RPATH_LINK_LIST ${GL_PATH})
        list(APPEND RPATH_LINK_LIST "/lib32" "/usr/lib32")

        foreach(LIBRARY IN LISTS X11_LIBRARIES)
            get_filename_component(X11_PATH ${LIBRARY} PATH)
            list(APPEND RPATH_LINK_LIST ${X11_PATH})
        endforeach()

        list(REMOVE_DUPLICATES RPATH_LINK_LIST)
        foreach(RPATH IN LISTS RPATH_LINK_LIST)
            SET(CMAKE_EXE_LINKER_FLAGS " ${CMAKE_EXE_LINKER_FLAGS} -Wl,-rpath-link,${RPATH}")
        endforeach()

        # Link against the system's libc, as X11 or OpenGL might require it
        if(EXISTS /lib/i386-linux-gnu/libc.so.6)
            gibbon_add_libraries(/lib/i386-linux-gnu/libc.so.6)
        endif()

    endif()

    gibbon_add_libraries(${X11_LIBRARIES})

    if(GIBBON_INPUT STREQUAL "auto")
        set(GIBBON_INPUT "x11")
    endif()
elseif(GIBBON_GRAPHICS_GL_WSYS STREQUAL "ios")
    gibbon_add_sources(OpenGLContextIOS.cpp)

    set(GIBBON_GRAPHICSENGINE_GLES2_IOS TRUE)
    gibbon_add_libraries("-framework OpenGLES" "-framework GLKit")
elseif(GIBBON_GRAPHICS_GL_WSYS STREQUAL "egl")
    gibbon_add_sources(OpenGLContextEGL.cpp)

    set(GIBBON_GRAPHICSENGINE_GLES2_EGL TRUE)
    gibbon_add_libraries(GLESv2 EGL)
elseif(GIBBON_GRAPHICS_GL_WSYS STREQUAL "osx")
    gibbon_add_sources(OpenGLContextOSX.mm)

    set(GIBBON_GRAPHICSENGINE_GLES2_OSX TRUE)
    gibbon_add_libraries("-framework OpenGL")

    if(GIBBON_INPUT STREQUAL "auto")
        set(GIBBON_INPUT "null")
    endif()
else()
    message(FATAL_ERROR "Unrecognized GL subsystem: ${GIBBON_GRAPHICS_GL_WSYS}")
endif()

gibbon_copy_resource(graphics.xml ${APP_DATA_DIR}/etc/conf/graphics.xml "")
