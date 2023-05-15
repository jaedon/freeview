# (c) 1997-2015 Netflix, Inc.  All content herein is protected by
# U.S. copyright and other applicable intellectual property laws and
# may not be copied without the express permission of Netflix, Inc.,
# which reserves all rights.  Reuse of any of this content for any
# purpose without the permission of Netflix, Inc. is strictly
# prohibited.

string(TOUPPER "${GIBBON_MEMANALYZER}" GIBBON_MEMANALYZER)

include_directories(. red_black_tree)

add_library(memanalyzer ${GIBBON_MEMANALYZER} memanalyzer/ma_rss_info_generator memanalyzer/red_black_tree/red_black_tree.c)
set_target_properties(memanalyzer PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(gibbon memanalyzer)

if("${GIBBON_MEMANALYZER}" STREQUAL "STATIC")
    gibbon_add_install_libraries(${CMAKE_CURRENT_BINARY_DIR}/libmemanalyzer.a)
else()
    gibbon_add_install_libraries(${CMAKE_CURRENT_BINARY_DIR}/libmemanalyzer.so)
endif()
