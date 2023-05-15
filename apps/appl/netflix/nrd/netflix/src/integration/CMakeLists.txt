cmake_minimum_required(VERSION 2.8)
include_directories(${CMAKE_CURRENT_LIST_DIR})

include(CheckCXXSymbolExists)

check_cxx_symbol_exists(inotify_init "sys/inotify.h" HAVE_INOTIFY)
if (HAVE_INOTIFY AND NOT NRDIL_DISABLE_INOTIFY)
  add_definitions(-DNRDIL_HAS_INOTIFY=1)
endif ()

include(CheckCSourceCompiles)

check_c_source_compiles("
#include <time.h>
int main()
{
    struct tm t;
    timegm(&t);
    (void)t;
    return 0;
}" NRDIL_HAS_TIMEGM)

if (NRDIL_HAS_TIMEGM)
  add_definitions(-DNRDIL_HAS_TIMEGM=1)
endif ()

check_c_source_compiles("
#include <time.h>
int main()
{
    struct tm t;
    t.tm_gmtoff = 0;
    (void)t;
    return 0;
 }" NRDIL_HAS_TM_GMTOFF)

if (NRDIL_HAS_TM_GMTOFF)
  add_definitions(-DNRDIL_HAS_TM_GMTOFF=1)
endif ()

check_c_source_compiles("
#include <time.h>
int main()
{
    struct tm t;
    t.tm_zone = 0;
    (void)t;
    return 0;
 }" NRDIL_HAS_TM_ZONE)

if (NRDIL_HAS_TM_ZONE)
  add_definitions(-DNRDIL_HAS_TM_ZONE=1)
endif ()

include(CheckCXXSourceCompiles)
check_cxx_source_compiles("
#include <sys/types.h>
#include <sys/stat.h>
int main(int, char**)
{
    struct stat st;
    return st.st_mtim.tv_sec;
}" NRDIL_HAS_STATMTIM)

if (NRDIL_HAS_STATMTIM)
  add_definitions(-DNRDIL_HAS_STATMTIM)
endif ()

add_library(nrdil TileManager.cpp TileManager.h Splash.cpp Splash.h Tile.h Buffer.h Buffer.cpp)
set_target_properties(nrdil PROPERTIES COMPILE_DEFINITIONS NRDIL)
target_link_libraries(nrdil ssl crypto curl)

create_install_target(
    TARGET nrdil
    DESCRIPTION "Netflix Integration Library"
    COMPONENT "NRDP"
    PUBLIC_HEADERS Buffer.h Tile.h TileManager.h Splash.h
)
