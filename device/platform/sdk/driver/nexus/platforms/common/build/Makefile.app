############################################################
#	  (c)2003-2012 Broadcom Corporation
#
#  This program is the proprietary software of Broadcom Corporation and/or its licensors,
#  and may only be used, duplicated, modified or distributed pursuant to the terms and
#  conditions of a separate, written license agreement executed between you and Broadcom
#  (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
#  no license (express or implied), right to use, or waiver of any kind with respect to the
#  Software, and Broadcom expressly reserves all rights in and to the Software and all
#  intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
#  HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
#  NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
#
#  Except as expressly set forth in the Authorized License,
#
#  1.	  This program, including its structure, sequence and organization, constitutes the valuable trade
#  secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
#  and to use this information only in connection with your use of Broadcom integrated circuit products.
#
#  2.	  TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
#  AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
#  WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
#  THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
#  OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
#  LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
#  OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
#  USE OR PERFORMANCE OF THE SOFTWARE.
#
#  3.	  TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
#  LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
#  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
#  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
#  THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
#  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
#  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
#  ANY LIMITED REMEDY.
#
# $brcm_Workfile: Makefile.app $
# $brcm_Revision: 22 $
# $brcm_Date: 9/20/12 11:34a $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /nexus/platforms/common/build/Makefile.app $
# 
# 22   9/20/12 11:34a erickson
# SW7435-235: adapt for symlink removal
# 
# 21   9/20/12 10:16a erickson
# SW7435-235: adapt for symlink removal
# 
# 20   9/14/12 11:04a erickson
# SW7435-235: adapt for symlink removal
# 
# 19   8/23/12 2:06p erickson
# SW7435-235: hide errors
# 
# 18   8/20/12 3:37p erickson
# SW7425-3260: also define NEXUS_SERVER_LDFLAGS for completeness
# 
# 17   8/20/12 12:25p erickson
# SW7425-3260: add NEXUS_CLIENT_LDFLAGS for make nexus_headers
# 
# 16   4/5/12 10:30a erickson
# SW7425-1969: copy berr.h
# 
# 15   4/3/12 12:02p erickson
# SW7425-1969: copy all blst headers
# 
# 14   3/29/12 1:02p erickson
# SW7425-2607: remove module-only check. moved to platform makefile so
#  it's always checked.
# 
# 13   3/13/12 4:54p erickson
# SW7425-2607: make module-only nexus.h to ensure public API has no
#  platform dependency
# 
# 12   3/9/12 1:38p erickson
# SW7425-1969: copy minimum set of magnum basemodules, create stubs for
#  internal/chip-specific headers
# 
# 11   3/8/12 4:45p erickson
# SW7425-1969: add NEXUS_HEADERS=y option and nexus.cpp.h output
# 
# 10   6/1/11 4:36p erickson
# SW7405-3833: support relative NEXUS_TOP paths
# 
# 9   4/8/11 11:17p alexpan
# SW7420-1563: Non-Nexus headers will be copied by the application
# 
# 8   4/1/11 12:04p erickson
# SW7405-3833: fix B_REFSW_CROSS_COMPILE
# 
# 7   3/2/11 9:22p alexpan
# SW7420-1563: Fix Android compile error with missing header file
#  bchp_m2mc.h
# 
# 7   3/2/11 9:19p alexpan
# SW7420-1563: Fix Android compile error with missing header file
#  bchp_m2mc.h
# 
# 6   2/18/11 4:54p zhangjq
# SW7420-1419:Clean up Android Media Player code
# 
# 5   12/22/10 3:57p erickson
# SW7420-1321: fix parallel build
#
# 4   12/7/10 10:02a erickson
# SW7405-3833: use separate cp per nexus module because of shared header
#  files via symlinks
#
# 3   12/6/10 4:16p erickson
# SW7405-3833: add call to generate_nexus_config.pl
#
# 2   4/1/10 2:16p erickson
# SW7405-3833: fix NEXUS_LDFLAGS
#
# 1   2/12/10 10:24a erickson
# SW7405-3833: add nexus_headers option to copy header files and static
#  platform_app.inc to nexus/bin/include
#
############################################################

# Makefile.app is called by nexus/platforms/$(NEXUS_PLATFORM)/build/Makefile to generate the static nexus/bin/include/platform_app.inc
# The static platform_app.inc can be delivered with a binary libnexus.so
# This Makefile assumes that NEXUS_TOP and NEXUS_PLATFORM are defined.
#
# Example usage:
#
# cd nexus/examples
# make clean
#
# # build nexus binary
# make api
#
# # fill nexus/bin/include w/ headers and platform_app.inc
# make nexus_headers
#
# # build using nexus/bin/include
# make NEXUS_PREBUILT_BINARY=y
#

ifndef NEXUS_TOP
$(error NEXUS_TOP is not defined)
endif

# include the dynamic platform_app.inc
include $(NEXUS_TOP)/platforms/$(NEXUS_PLATFORM)/build/platform_app.inc

NEXUS_CFLAGS := $(NEXUS_CFLAGS) $(addprefix -D,$(NEXUS_APP_DEFINES)) -I\$${NEXUS_BIN_DIR}/include
NEXUS_LDFLAGS := $(NEXUS_LDFLAGS) -L\$${NEXUS_BIN_DIR} -lpthread
ifeq ($(NEXUS_MODE),proxy)
NEXUS_CLIENT_LDFLAGS := $(NEXUS_LDFLAGS) -lnexus
else
NEXUS_CLIENT_LDFLAGS := $(NEXUS_LDFLAGS) -lnexus_client
endif
NEXUS_LDFLAGS := $(NEXUS_LDFLAGS) -lnexus 

.PHONY: copy_headers generate_inc generate_config_h generate_nexus_header

all: generate_inc generate_config_h generate_nexus_header

# override BCHP_INCLUDES so we don't copy all rdb header files. apps only need bchp.h.
BCHP_INCLUDES = $(MAGNUM)/basemodules/chp

# copy to bin/include
# because of common symlinks, we must do a separate cp per nexus module
# single cp is sufficient for magnum modules
copy_headers:
	${Q_}$(MKDIR) ${NEXUS_BIN_DIR}/include
	${Q_}$(foreach module, $(NEXUS_MODULES), $(CP) $(addsuffix /*.h, $(NEXUS_$(module)_PUBLIC_INCLUDES)) ${NEXUS_BIN_DIR}/include 2>/dev/null;)
# these bfile & utils header files are needed for file module public API
# TODO: nexus modules needs method for specifying their external header files. NEXUS_<MODULE>_MAGNUM_MODULES is a mix of internal & external
	${Q_}(cd $(NEXUS_TOP)/../BSEAV/lib/bfile; $(CP) bfile_types.h bfile_io.h ${NEXUS_BIN_DIR}/include)
	${Q_}(cd $(NEXUS_TOP)/../BSEAV/lib/utils; $(CP) bioatom.h balloc.h bpool.h ${NEXUS_BIN_DIR}/include)
# bstd is required for nexus api
	${Q_}$(CP) -f $(NEXUS_TOP)/../magnum/basemodules/std/types/$(B_REFSW_OS)/bstd_defs.h ${NEXUS_BIN_DIR}/include
	${Q_}$(CP) -f $(NEXUS_TOP)/../magnum/basemodules/std/bstd.h ${NEXUS_BIN_DIR}/include
# bdbg and bkni is convenient for apps
	${Q_}$(CP) -f $(NEXUS_TOP)/../magnum/basemodules/dbg/bdbg.h ${NEXUS_BIN_DIR}/include
	${Q_}$(CP) -f $(NEXUS_TOP)/../magnum/basemodules/dbg/bdbg_app.h ${NEXUS_BIN_DIR}/include
	${Q_}$(CP) -f $(NEXUS_TOP)/../magnum/basemodules/dbg/bdbg_priv.h ${NEXUS_BIN_DIR}/include
	${Q_}$(CP) -f $(NEXUS_TOP)/../magnum/basemodules/kni/generic/bkni.h ${NEXUS_BIN_DIR}/include
	${Q_}$(CP) -f $(NEXUS_TOP)/../magnum/basemodules/kni/generic/bkni_multi.h ${NEXUS_BIN_DIR}/include
	${Q_}$(CP) -f $(NEXUS_TOP)/../magnum/basemodules/err/berr.h ${NEXUS_BIN_DIR}/include
# overwrite nexus redirects
	${Q_}$(CP) -f $(NEXUS_TOP)/../magnum/portinginterface/grc/include/bm2mc_packet.h ${NEXUS_BIN_DIR}/include
	${Q_}cd ${NEXUS_BIN_DIR}/include; perl -n -e 'if (/\#include \"(.+\/nexus_platform_features.h)\"/) {system("cp -f ${NEXUS_TOP}/platforms/$$1 .");}' nexus_platform_features.h
	${Q_}cd ${NEXUS_BIN_DIR}/include; perl -n -e 'if (/\#include \"(.+\/nexus_platform_version.h)\"/) {system("cp -f ${NEXUS_TOP}/platforms/$$1 .");}' nexus_platform_version.h
# blst is required for bdbg
	${Q_}$(CP) -f $(NEXUS_TOP)/../magnum/commonutils/lst/blst_*.h ${NEXUS_BIN_DIR}/include
# do not allow the required headers to bleed into internal, chip-specific headers
	${Q_}echo "/* stub file */" >${NEXUS_BIN_DIR}/include/bstd_cfg.h
	${Q_}echo "/* stub file */" >${NEXUS_BIN_DIR}/include/bchp.h

FILE = ${NEXUS_BIN_DIR}/include/platform_app.inc
CONFIG_H_FILE = ${NEXUS_BIN_DIR}/include/nexus_config.h

# NEXUS_TOP may be relative. convert to absolute path.
generate_inc: copy_headers
	${Q_}${RM} ${FILE}
	${Q_}echo "#######################################" >>${FILE}
	${Q_}echo "# Nexus platform_app.inc" >>${FILE}
	${Q_}echo "# This file is generated with 'make nexus_headers' and placed into nexus/bin/include" >>${FILE}
	${Q_}echo "#######################################" >>${FILE}
	${Q_}echo "" >>${FILE}
	${Q_}echo "# Top of the nexus tree. Modify this to make the binary portable." >>${FILE}
	${Q_}echo "NEXUS_TOP ?= $(shell cd ${NEXUS_TOP}; pwd)" >>${FILE}
	${Q_}echo "NEXUS_BIN_DIR ?= ${NEXUS_BIN_DIR}" >>${FILE}
	${Q_}echo "" >>${FILE}
	${Q_}echo "# Build environment" >>${FILE}
	${Q_}echo "B_REFSW_OS  = ${B_REFSW_OS}" >>${FILE}
	${Q_}echo "NEXUS_PLATFORM  = ${NEXUS_PLATFORM}" >>${FILE}
	${Q_}echo "NEXUS_MODE  = ${NEXUS_MODE}" >>${FILE}
	${Q_}echo "BCHP_CHIP  = ${BCHP_CHIP}" >>${FILE}
	${Q_}echo "BCHP_VER  = ${BCHP_VER}" >>${FILE}
	${Q_}echo "" >>${FILE}
	${Q_}echo "NEXUS_CFLAGS = ${NEXUS_CFLAGS}" >>${FILE}
	${Q_}echo "" >>${FILE}
	${Q_}echo "NEXUS_LDFLAGS = ${NEXUS_LDFLAGS}" >>${FILE}
	${Q_}echo "NEXUS_SERVER_LDFLAGS = ${NEXUS_LDFLAGS}" >>${FILE}
	${Q_}echo "NEXUS_CLIENT_LDFLAGS = ${NEXUS_CLIENT_LDFLAGS}" >>${FILE}
	${Q_}echo "" >>${FILE}
	${Q_}echo "B_REFSW_CROSS_COMPILE ?= ${B_REFSW_TOOLCHAIN_ARCH}-" >>${FILE}
	${Q_}echo "" >>${FILE}
	${Q_}echo "# Standard toolchain" >>${FILE}
	${Q_}echo "AS      = \$${B_REFSW_CROSS_COMPILE}as" >>${FILE}
	${Q_}echo "LD      = \$${B_REFSW_CROSS_COMPILE}ld" >>${FILE}
	${Q_}echo "CC      = \$${B_REFSW_CROSS_COMPILE}gcc" >>${FILE}
	${Q_}echo "CXX     = \$${B_REFSW_CROSS_COMPILE}c++" >>${FILE}
	${Q_}echo "AR      = \$${B_REFSW_CROSS_COMPILE}ar" >>${FILE}
	${Q_}echo "NM      = \$${B_REFSW_CROSS_COMPILE}nm" >>${FILE}
	${Q_}echo "STRIP   = \$${B_REFSW_CROSS_COMPILE}strip" >>${FILE}
	${Q_}echo "OBJCOPY = \$${B_REFSW_CROSS_COMPILE}objcopy" >>${FILE}
	${Q_}echo "OBJDUMP = \$${B_REFSW_CROSS_COMPILE}objdump" >>${FILE}
	${Q_}echo "RANLIB  = \$${B_REFSW_CROSS_COMPILE}ranlib" >>${FILE}

generate_config_h: generate_inc
	${Q_}$(PERL) $(NEXUS_TOP)/build/generate_nexus_config.pl ${FILE} >${CONFIG_H_FILE}

CPP=$(B_REFSW_CROSS_COMPILE)cpp
NEXUS_HEADER=$(NEXUS_BIN_DIR)/include/nexus.h
NEXUS_CPP_HEADER=$(NEXUS_BIN_DIR)/include/nexus.cpp.h
generate_nexus_header: copy_headers
	${Q_}cd $(NEXUS_BIN_DIR)/include;\
	    echo "#ifndef NEXUS_H__" >$(NEXUS_HEADER); \
	    echo "#define NEXUS_H__" >>$(NEXUS_HEADER); \
	    ls *.h|grep -v nexus_config.h|awk '{print "#include \"" $$1 "\""}' >>$(NEXUS_HEADER); \
	    echo "#endif" >>$(NEXUS_HEADER)
	${Q_}$(CPP) -P $(NEXUS_HEADER) $(NEXUS_CFLAGS) >$(NEXUS_CPP_HEADER)
