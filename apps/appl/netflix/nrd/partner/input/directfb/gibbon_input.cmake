# U.S. copyright and other applicable intellectual property laws and
# may not be copied without the express permission of Netflix, Inc.,
# which reserves all rights.  Reuse of any of this content for any
# purpose without the permission of Netflix, Inc. is strictly
# prohibited.

MESSAGE( STATUS "GLOBALCONFIG_PATH:         " ${GLOBALCONFIG_PATH} )

# GLOBALCONFIG_PATH defined in /appl/netflix/nrd/netflix/configure and /apps/appl_make/netflix/nrd.build/setupenv.sh
if(GLOBALCONFIG_PATH)
add_definitions(-DGIBBON_USE_GLOBALCONFIG)
endif()

set(GIBBON_INPUT_DIRECTFB TRUE)
# gibbon_add_sources(InputManagerDFB.cpp)
gibbon_add_sources(InputManagerDFB_HAPI.cpp)
gibbon_add_libraries(directfb)

gibbon_copy_resource(input.xml ${APP_DATA_DIR}/etc/conf/input.xml "")