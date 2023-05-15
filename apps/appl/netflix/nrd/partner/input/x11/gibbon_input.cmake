# U.S. copyright and other applicable intellectual property laws and
# may not be copied without the express permission of Netflix, Inc.,
# which reserves all rights.  Reuse of any of this content for any
# purpose without the permission of Netflix, Inc. is strictly
# prohibited.

set(GIBBON_INPUT_X11 TRUE)
gibbon_add_sources(InputManagerX11.cpp)

find_package(X11 REQUIRED)

#add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/include/X11
#    COMMAND ${CMAKE_COMMAND} -E create_symlink "${X11_X11_INCLUDE_PATH}/X11" "${CMAKE_BINARY_DIR}/include/X11"
#)

add_custom_target(gibbon_x11_links DEPENDS "${CMAKE_BINARY_DIR}/include/X11")
gibbon_add_dependencies(gibbon_x11_links)

if(EXISTS /lib/i386-linux-gnu/libc.so.6)
    gibbon_add_libraries(/lib/i386-linux-gnu/libc.so.6)
endif()

gibbon_add_libraries(${X11_LIBRARIES})