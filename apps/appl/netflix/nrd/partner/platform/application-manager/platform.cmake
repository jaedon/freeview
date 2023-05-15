# (c) 1997-2015 Netflix, Inc.  All content herein is protected by
# U.S. copyright and other applicable intellectual property laws and
# may not be copied without the express permission of Netflix, Inc.,
# which reserves all rights.  Reuse of any of this content for any
# purpose without the permission of Netflix, Inc. is strictly
# prohibited.

get_filename_component(PLATFORM_DIRECTORY ${CMAKE_CURRENT_LIST_FILE} PATH)

set(GIBBON_TILEMANAGER ON)
add_definitions(-DNRDP_PLATFORM_APPMANAGER)
set(GIBBON_OEM_XML "${PLATFORM_DIRECTORY}/oem.xml")
gibbon_add_sources(main_am.cpp ApplicationManagerClient.cpp)

add_executable(amcontroller ${PLATFORM_DIRECTORY}/ApplicationManagerController.cpp ${PLATFORM_DIRECTORY}/ApplicationManager.cpp)
target_link_libraries(amcontroller nrdbase z)
add_executable(am ${PLATFORM_DIRECTORY}/ApplicationManager.cpp ${PLATFORM_DIRECTORY}/ApplicationManagerMain.cpp)
target_link_libraries(am nrdbase z)
