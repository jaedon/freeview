#############################################################################
#    (c)2011 Broadcom Corporation
# 
# This program is the proprietary software of Broadcom Corporation and/or its licensors,
# and may only be used, duplicated, modified or distributed pursuant to the terms and
# conditions of a separate, written license agreement executed between you and Broadcom
# (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
# no license (express or implied), right to use, or waiver of any kind with respect to the
# Software, and Broadcom expressly reserves all rights in and to the Software and all
# intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
# HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
# NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.  
#  
# Except as expressly set forth in the Authorized License,
#  
# 1.     This program, including its structure, sequence and organization, constitutes the valuable trade
# secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
# and to use this information only in connection with your use of Broadcom integrated circuit products.
#  
# 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS" 
# AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR 
# WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO 
# THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES 
# OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, 
# LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION 
# OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF 
# USE OR PERFORMANCE OF THE SOFTWARE.
# 
# 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS 
# LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR 
# EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR 
# USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF 
# THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT 
# ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE 
# LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF 
# ANY LIMITED REMEDY.
#
# $brcm_Workfile: Makefile $
# $brcm_Revision: 16 $
# $brcm_Date: 11/30/12 2:18p $
# 
# Module Description:
# 
# Revision History:
# 
# $brcm_Log: /BSEAV/lib/security/common_drm/Makefile $
# 
# 16   11/30/12 2:18p gayatriv
# SW7425-4260:changes for commondrm for android
# 
# 15   11/21/12 3:43p cdisc
# SWSECURITY-200: memory heap management issue
# 
# 14   10/31/12 1:02p cdisc
# SWSECURITY-199: removing useless inclusions
# 
# 13   10/18/12 4:51p cdisc
# SWSECURITY-192: adding memory check
# 
# 12   10/4/12 11:30a cdisc
# SW7425-3979: updating for DTCP-IP streamline
# 
# SW7425-3979/1   10/3/12 1:26p cdisc
# SW7425-3979: update for rootfs option only for DRM bin file
# 
# 11   9/7/12 10:44a mphillip
# SW7425-3739: Merge symlink removal to main
# 
# SW7425-3739/1   9/5/12 5:16p mphillip
# SW7425-3739: Remove BSEAV/lib symlinks
# 
# 10   7/27/12 5:52p cdisc
# SW7429-211: refactoring to work with flat nexus directory structure
# 
# 9   6/1/12 10:25a cdisc
# SW7425-3138: adjusting Makefile after changes to bcrypt and openssl
# 
# 8   5/31/12 5:39p cdisc
# SW7425-3138: openssl migration to 1.0.0f and PMC adobe
# 
# 7   5/23/12 2:22p celysee
# SW7425-2894: Adding common drm adn dtcp-ip
# 
# 6   5/15/12 2:51p cdisc
# SW7425-3067: linking statically with opennsl
# 
# 5   4/24/12 4:21p cdisc
# SW7425-2928: merging with new common crypto code
# 
# 4   4/18/12 6:12p celysee
# SW7425-2894: Support for DTCP-IP with robustness rules
# 
# 3   2/23/12 3:59p cdisc
# SWSECURITY-110: changes to link with drmrootfs
# 
# 2   1/10/12 4:40p cdisc
# SW7125-1208: Makefile mods for new location
# 
# 3   12/22/11 11:43a cdisc
# SW7425-2046: common drm update
# 
# 2   8/23/11 9:53a cdisc
# SW7425-1114: adjusting Makefile for AppLibs environment
# 
# 1   8/22/11 9:35a cdisc
# SW7425-1114: initial checkin
# 
#############################################################################
NEXUS_PLATFORM ?= $(PLATFORM)
NEXUS_TOP := $(shell cd ../../../../nexus; pwd)
BSEAV_TOP := $(shell cd ../../../../BSEAV; pwd)

# Include nexus definitions
include $(NEXUS_TOP)/platforms/common/build/nexus_platforms.inc
include $(NEXUS_TOP)/platforms/common/build/platform_app.inc
include $(NEXUS_TOP)/build/nexus_defs.inc

ifeq ($(NEXUS_MODE),proxy)
B_REFSW_OS = linuxkernel
else
B_REFSW_OS = linuxuser
endif

# Sanity check that we received a valid platform
ifndef BCHP_CHIP
$(error Unsupported platform $(NEXUS_PLATFORM))
endif

include lib/cmndrm_dir.inc

# Convert include paths into single variable
NEXUS_APP_INCLUDE_PATHS := $(foreach module, $(NEXUS_MODULES), $(NEXUS_$(module)_PUBLIC_INCLUDES))
NEXUS_APP_DEFINES := $(foreach module, $(NEXUS_MODULES), $(NEXUS_$(module)_DEFINES))
NEXUS_APP_DEFINES += $(foreach module, $(NEXUS_MODULES),NEXUS_HAS_$(module))

# Convert magnum includes into the same variable
NEXUS_APP_INCLUDE_PATHS += $(foreach module, $(MAGNUM_MODULES), $($(module)_INCLUDES))
NEXUS_APP_DEFINES += $(foreach module, $(MAGNUM_MODULES), $($(module)_DEFINES))


# Custom include paths
CFLAGS += -I./include -I./include/priv


CFLAGS += -DPIC -fpic
CFLAGS += $(addprefix -I, ${NEXUS_APP_INCLUDE_PATHS})

# This is the minimum needed to compile and link with Nexus
CFLAGS += $(NEXUS_CFLAGS) $(addprefix -I,$(NEXUS_APP_INCLUDE_PATHS)) $(addprefix -D,$(NEXUS_APP_DEFINES))

# add Bcrypt flags
CFLAGS += -I$(BSEAV_TOP)/lib/security/bcrypt/include

ifeq ($(ANDROID_BUILD),y)
CFLAGS += -I$(BSEAV_TOP)/../AppLibs/opensource/android/src/mips-ics/external/openssl/include
else
CFLAGS += -I$(BSEAV_TOP)/lib/openssl/$(NEXUS_PLATFORM)/include 
endif


# common crypto include path
CFLAGS += -I${BSEAV_TOP}/lib/security/common_crypto/include

CFLAGS += -I$(BSEAV_TOP)/lib/drmrootfs


#allow c++ style comments
CFLAGS += -std=c99
ifeq ($(ANDROID_BUILD),y)
CFLAGS += -EL -DANDROID
endif

############################################################################
#                              
#vvvv#####################vvvvvvvvvvvvvvvvvvvvvvv#####################vvvv##

# DRM Makefile exclude the 'include' subfolder which is built with the makefile
D_ALL_SUBDIRS		:= $(filter-out %include, $(shell find . -type d))
D_ALL_SUBDIRS	:= $(filter-out ./examples/%, ${D_ALL_SUBDIRS})
D_ALL_MODULE_DIR	:= $(filter-out %include, $(shell find . -maxdepth 1 -type d))
#$(error XXX D_ALL_SUBDIRS = ${D_ALL_SUBDIRS} XXX)

vpath %.cpp    		${D_ALL_SUBDIRS}
vpath %.c    		${D_ALL_SUBDIRS}
vpath %.h      		${D_ALL_SUBDIRS}
LIB_BASENAME        := cmndrm


############
# Set F_SRC_EXCLUDES to the basenames of C/C++ files you would like to 
# exclude from compilation.  By default, all *.c and *.cpp files will
# be compiled.  
#####
F_SRC_EXCLUDES      = drm_data.c


############
# Set F_PUBLIC_INCS to the basenames of include files that are to be 
# used by other libraries or applications.  These files will be 
# installed into .../${BCHP_VER}/include/
#####
#F_PUBLIC_INCS    	+= $(sort $(notdir $(shell find ${D_ALL_MODULE_DIR} -name '*.h')))
F_PUBLIC_INCS    	+= 


#$(error ${F_PUBLIC_INCS})

############
# If your library depends on another shared lib, you should add that 
# lib here (and any additional lib directories to search).
#####

ifeq ($(ANDROID_BUILD),y)
LDFLAGS				+= -L$(BSEAV_TOP)/lib/drmrootfs/lib/android -ldrmrootfs
LDFLAGS				+= -L$(BSEAV_TOP)/lib/security/bcrypt/lib/android -lbcrypt
else
LDFLAGS				+= -L$(BSEAV_TOP)/lib/drmrootfs/lib/${B_REFSW_OS} -ldrmrootfs
LDFLAGS				+= -L$(BSEAV_TOP)/lib/security/bcrypt/lib -lbcrypt
endif

# TODO: uncheck when shared OpenSSL library is available
ifeq ($(ANDROID_BUILD),y)
LDFLAGS                 += -L$(BSEAV_TOP)/../AppLibs/opensource/android/src/mips-ics/out/target/product/bcm7425/system/lib -lcrypto
LDFLAGS					+= -L$(NEXUS_TOP)/bin -lnexus
LDFLAGS					+= -EL
else
LDFLAGS				+= -L$(BSEAV_TOP)/lib/openssl/$(NEXUS_PLATFORM)/lib -lcrypto
endif


############
# Directory where to install the headers
#####
D_FOR_INC_INSTALL	:= include



############
# You probably shouldn't have to modify the text below this point.
#####
F_PUBLIC_LIBS    	+= lib${LIB_BASENAME}.a lib${LIB_BASENAME}.so
D_FOR_LIB_INSTALL	:= lib
D_FOR_LIB_INSTALL_CHIP_VERSION	:= $(addprefix ${D_FOR_LIB_INSTALL}/, ${CMNDRM_DIR})
F_SRCS	       		:= $(filter-out ${F_SRC_EXCLUDES}, $(notdir $(wildcard $(addsuffix /*.c, ${D_ALL_SUBDIRS}))))
F_SRCS_GPG          := 
F_PREBUILT_OBJS     := $(patsubst %.gpg,%.o,${F_SRCS_GPG})
F_OBJS 		   		:= $(patsubst %.c,%.o, ${F_SRCS})  ${F_PREBUILT_OBJS}
F_INSTALLED_LIBS	:= $(addprefix ${D_FOR_LIB_INSTALL}/, ${F_PUBLIC_LIBS})
F_INSTALLED_LIBS	+= $(addprefix ${D_FOR_LIB_INSTALL_CHIP_VERSION}/, ${F_PUBLIC_LIBS})
#^^^^#####################^^^^^^^^^^^^^^^^^^^^^^^#####################^^^^##



############################################################################
#                              MAIN TARGETS
#vvvv#####################vvvvvvvvvvvvvvvvvvvvvvv#####################vvvv##
ifeq ($(ANDROID_BUILD),y)
#for android build we do not attept to build openssl but just use the prebuilt library
all: ${F_PUBLIC_LIBS}
else
all: prep_openssl ${F_PUBLIC_LIBS}
endif

prep_openssl:
	if [ ! -d "${BSEAV_TOP}/lib/openssl/$(NEXUS_PLATFORM)" ]; then \
		echo "${BSEAV_TOP}/lib/openssl/$(NEXUS_PLATFORM) not found, calling make in ${BSEAV_TOP}/lib/openssl/"; \
		make -C ${BSEAV_TOP}/lib/openssl/; \
	fi

clean: 
	find -name '*.d' -o -name '*.o'  | xargs rm -f
	ls *.a  | xargs rm -f
	ls *.so | xargs rm -f


#^^^^#####################^^^^^^^^^^^^^^^^^^^^^^^#####################^^^^##


############################################################################
#                             BUILD RULES
#vvvv#####################vvvvvvvvvvvvvvvvvvvvvvv#####################vvvv##
lib${LIB_BASENAME}.so: ${F_OBJS} 
	@ echo [Linking shared library .... $@ ]
	@ ${CC} -shared -o $@  ${LDFLAGS}  $^ -Wl,-dy
	${STRIP_COMMAND}

lib${LIB_BASENAME}.a: ${F_OBJS} 
	@ echo [Linking static library .... $@ ]
	@ ${AR} rc  $@  $^ 
	${STRIP_COMMAND}

idirs:
	@ [ -d ${D_FOR_LIB_INSTALL} ] || mkdir -p ${D_FOR_LIB_INSTALL}
	@ [ -d ${D_FOR_LIB_INSTALL_CHIP_VERSION} ] || mkdir -p ${D_FOR_LIB_INSTALL_CHIP_VERSION}
	@ [ -d ${D_FOR_INC_INSTALL} ] || mkdir -p ${D_FOR_INC_INSTALL}

install: all idirs ${F_INSTALLED_LIBS} 

${D_FOR_LIB_INSTALL}/%.a : %.a
	install -m 0755 $< $@ 

${D_FOR_LIB_INSTALL}/%.so : %.so
	install -m 0755 $< $@ 

${D_FOR_LIB_INSTALL_CHIP_VERSION}/%.a : %.a
	install -m 0755 $< $@ 

${D_FOR_LIB_INSTALL_CHIP_VERSION}/%.so : %.so
	install -m 0755 $< $@ 

%.o: %.c
	@ echo [Compiling .... $< ]
	@ ${CC} ${CFLAGS} -c -o $@ $< 
	
copy:	
	@echo [Copying proper Common DRM lib version...]
	cp -f $(BSEAV_TOP)/lib/security/common_drm/lib/${CMNDRM_DIR}/libcmndrm.so $(NEXUS_TOP)/bin
	
