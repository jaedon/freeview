#!/bin/sh
 
#Tools prefix
TOOLCHAIN_PREFIX=mipsel-linux-uclibc
PRODUCT_DIR=`(cd ../../../../../ && pwd)`

USER_ID=`whoami`
 
#important variable that the build system expects to be present
export TOOLCHAIN_DIRECTORY="${CONFIG_TOOLCHAIN_PATH}"
 
#this is useful only for x86 builds, and it's used because we want to force a link against the version of libc included on the toolchain
export TOOLCHAIN_SYSROOT="${TOOLCHAIN_DIRECTORY}/${TOOLCHAIN_PREFIX}/sys-root"
TOOL_PREFIX="${TOOLCHAIN_DIRECTORY}/bin/${TOOLCHAIN_PREFIX}-"
 
#optional. we add the compiler to the path
export PATH="${TOOLCHAIN_DIRECTORY}/bin:${PATH}"
 
#Compiler flags
#Architecture specific CFLAGS (like -march, -mtune, -EL)
ARCH_FLAGS=-m32
 
#CPPFLAGS: C/C++ preprocessor flags, e.g. -I<include dir> if you have headers in a nonstandard directory <include dir>
export CPPFLAGS="-isystem ${PRODUCT_DIR}/packages/freetype/include \
-isystem ${PRODUCT_DIR}/packages/freetype/include/freetype2 \
-isystem ${PRODUCT_DIR}/packages/calypso/debug/include \
-isystem ${PRODUCT_DIR}/packages/c-ares/include \
-isystem ${PRODUCT_DIR}/packages/libwebp/include \
-isystem ${PRODUCT_DIR}/packages/graphite2/include \
-isystem ${PRODUCT_DIR}/packages/gperftools/include \
-isystem ${PRODUCT_DIR}/packages/harfbuzz/include \
-isystem ${PRODUCT_DIR}/packages/harfbuzz/include/harfbuzz \
-isystem ${PRODUCT_DIR}/packages/jpeg/include \
-isystem ${PRODUCT_DIR}/packages/libmng/include \
-isystem ${PRODUCT_DIR}/packages/libpng/include \
-isystem ${PRODUCT_DIR}/packages/icu4c/include \
-isystem ${PRODUCT_DIR}/packages/lcms/include \
-isystem ${PRODUCT_DIR}/packages/Tremor/include \
-isystem ${PRODUCT_DIR}/packages/zlib/include \
-isystem ${PRODUCT_DIR}/packages/openssl/include \
-isystem ${PRODUCT_DIR}/packages/libcurl/include \
-isystem ${PRODUCT_DIR}/packages/libexpat/include \
-isystem ${DIRECTFB_DIR}/usr/include \
-isystem ${DIRECTFB_DIR}/usr/include/directfb \
-isystem ${TOOLCHAIN_SYSROOT}/usr/include \
-isystem ${PRODUCT_DIR}/apps/appl/netflix/nxlink/include \
-isystem ${PRODUCT_DIR}/apps/octo/service/hama/include \
-isystem ${PRODUCT_DIR}/apps/octo/library/hlib/include \
-isystem ${PRODUCT_DIR}/device/include/hos"

#LDFLAGS: linker flags, e.g. -L<lib dir> if you have libraries in a nonnstandard directory <lib dir>
export LDFLAGS="-L${HUMAX_NFS_INSTALL_DIR}/usr/lib -L${HUMAX_NFS_INSTALL_DIR}/lib -Wl,-rpath,-rpath-link"
#LIBS: libraries to pass to the linker, e.g. -l<library>
#export LIBS="-ldl -lrt -lpthread -lpng -ljpeg -lfreetype -ldirectfb -lfusion -ldirect -lmng -lwebp -lssl -lcrypto -lnexus_client -lvorbisidec -lmi -lcalypso -lprdy -lz -lstdc++ -lavformat -lavcodec -lavfilter -lavutil -lswresample -lswscale -lxml2 -lViewRightWebClient -lViewRightWebWrapper -lnxlink -lappkit -lhlib -ldlib -lrlib -lsilib -loapi -lvkernel -ljansson -lhapi -ldapi -luapi -lnapi -lpapi -lsilib -lsamalib -lsapi -lexif -lid3 -lsqlite3 -llog4c -lcurl -lmemory-debug"
 
LIBS="-ldl -lrt -lpthread -lhapi -lhlib -lpng -ljpeg -lfreetype -ldirectfb -lfusion -ldirect -lmng -lcares -lexpat -lwebp -lssl -lcrypto -lnexus_client -lvorbisidec -lmi -lcalypso -lprdy -lz -lstdc++ -lavformat -lavcodec -lavfilter -lavutil -lswresample -lswscale -lxml2 -lViewRightWebClient -lViewRightWebWrapper -lnxlink -lappkit -lhlib -ldlib -lrlib -lsilib -loapi -lvkernel -ljansson -lhapi -ldapi -luapi -lnapi -lpapi -lsilib -lsamalib -lsapi -lexif -lid3 -lsqlite3 -llog4c -lcurl -lmemory-debug -licui18n"

if [ -f "${HUMAX_NFS_INSTALL_DIR}/usr/lib/libtapi.so" ]; then
LIBS="${LIBS} -ltapi"
fi

if [ -f "${HUMAX_NFS_INSTALL_DIR}/usr/lib/libsitextlib.so" ]; then
LIBS="${LIBS} -lsitextlib"
fi

export LIBS

#CFLAGS: C compiler flags
COMMON_CFLAGS="-fPIC -Wall -Wextra -fdiagnostics-show-option -Wno-long-long -Wno-variadic-macros -Wno-format -Wno-strict-aliasing -fstack-protector-all"
export CFLAGS="${COMMON_CFLAGS} -std=gnu99 -fgnu89-inline"
export CXXFLAGS="${COMMON_CFLAGS} -Wnon-virtual-dtor -Woverloaded-virtual"
 
export CMAKE_CXX_COMPILER="${TOOL_PREFIX}g++"
export CMAKE_C_COMPILER_FORCED="TRUE"
export CMAKE_AR="${TOOL_PREFIX}ar"
export CMAKE_C_COMPILER="${TOOL_PREFIX}gcc"
export CMAKE_C_COMPILER_FORCED="TRUE"

export CC="${TOOL_PREFIX}gcc"
export CXX="${TOOL_PREFIX}g++"
export AR="${TOOL_PREFIX}ar"
export LD="${TOOL_PREFIX}ld"
export STRIP="${TOOL_PREFIX}strip"
export RANLIB="${TOOL_PREFIX}ranlib"
export GDB="${TOOL_PREFIX}gdb"
export OBJCOPY="${TOOL_PREFIX}objcopy"
export COVERAGE_COMMAND="${TOOL_PREFIX}gcov"
 
#Flags to add when building in small mode
export SMALL_FLAGS="-s -O3"
export SMALL_CFLAGS=""
export SMALL_CXXFLAGS=""
#-fvisibility=hidden -fvisibility-inlines-hidden"

export CMAKE="/usr/bin/cmake"
export DRM_INCLUDE_DIRECTORY="${PRODUCT_DIR}/packages/playready/include"
#export CTEST="/usr/local/i686-netflix-linux-gnu-4.1/netflix/bin/ctest"
#export CMAKE="/usr/local/i686-netflix-linux-gnu-4.1/netflix/bin/cmake"
#export CTEST="/usr/local/i686-netflix-linux-gnu-4.1/netflix/bin/ctest"
