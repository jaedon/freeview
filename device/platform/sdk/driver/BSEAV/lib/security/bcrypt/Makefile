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
# 
#############################################################################
NEXUS_PLATFORM ?= $(PLATFORM)
NEXUS_TOP := $(shell cd ../../../../nexus; pwd)
BSEAV_TOP := $(shell cd ../../../../BSEAV; pwd)

# Include nexus definitions


include $(NEXUS_TOP)/platforms/common/build/nexus_platforms.inc
include $(NEXUS_TOP)/platforms/common/build/platform_app.inc
include $(NEXUS_TOP)/build/nexus_defs.inc

# Sanity check that we received a valid platform
ifndef BCHP_CHIP
$(error Unsupported platform $(NEXUS_PLATFORM))
endif


# Convert include paths into single variable
NEXUS_APP_INCLUDE_PATHS := $(foreach module, $(NEXUS_MODULES), $(NEXUS_$(module)_PUBLIC_INCLUDES))
NEXUS_APP_DEFINES := $(foreach module, $(NEXUS_MODULES), $(NEXUS_$(module)_DEFINES))
NEXUS_APP_DEFINES += $(foreach module, $(NEXUS_MODULES),NEXUS_HAS_$(module))

# Convert magnum includes into the same variable
NEXUS_APP_INCLUDE_PATHS += $(foreach module, $(MAGNUM_MODULES), $($(module)_INCLUDES))
NEXUS_APP_DEFINES += $(foreach module, $(MAGNUM_MODULES), $($(module)_DEFINES))


# Custom include paths
CFLAGS += -I./include -I./include/priv
CFLAGS += -I$(NEXUS_TOP)/modules/dma/$(BCHP_CHIP)/include
CFLAGS += -I$(NEXUS_TOP)/modules/dma/$(BCHP_CHIP)/include/priv
CFLAGS += -I$(NEXUS_TOP)/modules/security/$(BCHP_CHIP)/include
CFLAGS += -I$(NEXUS_TOP)/modules/security/$(BCHP_CHIP)/include/priv
CFLAGS += -I$(NEXUS_TOP)/modules/core/$(BCHP_CHIP)/include
CFLAGS += -I$(NEXUS_TOP)/platforms/$(NEXUS_PLATFORM)/include
CFLAGS += -I$(MAGNUM)/portinginterface/hsm/$(BCHP_CHIP)/${BCHP_VER}/common
CFLAGS += -DPIC -fpic
CFLAGS += $(addprefix -I, ${NEXUS_APP_INCLUDE_PATHS})

# This is the minimum needed to compile and link with Nexus
CFLAGS += $(NEXUS_CFLAGS) $(addprefix -I,$(NEXUS_APP_INCLUDE_PATHS)) $(addprefix -D,$(NEXUS_APP_DEFINES))

ifeq ($(ANDROID_BUILD),y)
CFLAGS += -I$(BSEAV_TOP)/../AppLibs/opensource/android/src/mips-ics/external/openssl/include
CFLAGS += -EL
else
CFLAGS += -I$(BSEAV_TOP)/lib/openssl/$(PLATFORM)/include 
endif

CFLAGS += -I$(BSEAV_TOP)/lib/security/bcrypt/include

CFLAGS += -I$(BSEAV_TOP)/lib/drmrootfs


#allow c++ style comments
CFLAGS += -std=c99

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
LIB_BASENAME        := bcrypt


############
# Set F_SRC_EXCLUDES to the basenames of C/C++ files you would like to 
# exclude from compilation.  By default, all *.c and *.cpp files will
# be compiled.  
#####
#F_SRC_EXCLUDES      = 


############
# Set F_PUBLIC_INCS to the basenames of include files that are to be 
# used by other libraries or applications.  These files will be 
# installed into .../${BCHP_VER}/include/
#####
F_PUBLIC_INCS    	+= $(sort $(notdir $(shell find ${D_ALL_MODULE_DIR} -name '*.h')))


#$(error ${F_PUBLIC_INCS})

############
# If your library depends on another shared lib, you should add that 
# lib here (and any additional lib directories to search).
#####
#LDFLAGS				+= -L$(BSEAV_TOP)/lib/drmrootfs -ldrmrootfs

# TODO: uncheck when shared OpenSSL library is available
ifeq ($(ANDROID_BUILD),y)
LDFLAGS             += -L$(BSEAV_TOP)/../AppLibs/opensource/android/src/mips-ics/out/target/product/bcm7425/system/lib -lcrypto
LDFLAGS             += -EL
LDFLAGS		    += -L$(NEXUS_TOP)/bin -lnexus
else
LDFLAGS		    += -L$(BSEAV_TOP)/lib/openssl/${PLATFORM}/lib -lcrypto
endif






############
# Directory where to install the headers
#####
D_FOR_INC_INSTALL	:= include



############
# You probably shouldn't have to modify the text below this point.
#####
F_PUBLIC_LIBS    	+= lib${LIB_BASENAME}.a lib${LIB_BASENAME}.so
ifeq ($(ANDROID_BUILD),y)
D_FOR_LIB_INSTALL	:= lib/android
else
D_FOR_LIB_INSTALL	:= lib
endif

F_SRCS	       		:= $(filter-out ${F_SRC_EXCLUDES}, $(notdir $(wildcard $(addsuffix /*.c, ${D_ALL_SUBDIRS}))))
F_SRCS_GPG          := 
F_PREBUILT_OBJS     := $(patsubst %.gpg,%.o,${F_SRCS_GPG})
F_OBJS 		   		:= $(patsubst %.c,%.o, ${F_SRCS})  ${F_PREBUILT_OBJS}
F_INSTALLED_LIBS	:= $(addprefix ${D_FOR_LIB_INSTALL}/, ${F_PUBLIC_LIBS})
#^^^^#####################^^^^^^^^^^^^^^^^^^^^^^^#####################^^^^##



############################################################################
#                              MAIN TARGETS
#vvvv#####################vvvvvvvvvvvvvvvvvvvvvvv#####################vvvv##
ifeq ($(ANDROID_BUILD),y)
all: ${F_PUBLIC_LIBS}
else
all: prep_openssl ${F_PUBLIC_LIBS}
endif

prep_openssl:
	if [ ! -d "${BSEAV_TOP}/lib/openssl/${PLATFORM}" ]; then \
		echo "${BSEAV_TOP}/lib/openssl/${PLATFORM} not found, calling make in ${BSEAV_TOP}/lib/openssl/"; \
		make -C ${BSEAV_TOP}/lib/openssl/; \
	fi

clean: 
	find -name '*.d' -o -name '*.o'  | xargs rm -f
	ls *.a  | xargs rm -f
	ls *.so | xargs rm -f


distclean: clean
#	find ./lib -name '*.a' -o -name '*.so' | xargs rm -f
#	rm -f ${F_INSTALLED_LIBS} 
#	rm -f ${D_FOR_INC_INSTALL}/*.h ${F_INSTALLED_LIBS} 

#^^^^#####################^^^^^^^^^^^^^^^^^^^^^^^#####################^^^^##



############################################################################
#                             BUILD RULES
#vvvv#####################vvvvvvvvvvvvvvvvvvvvvvv#####################vvvv##
lib${LIB_BASENAME}.so: ${F_OBJS} 
# TODO: linking with static OpenSSL library for now, need to generate shared libraries
	${CC} -shared -o $@  ${LDFLAGS}  $^ -Wl,-dy
	${STRIP_COMMAND}

lib${LIB_BASENAME}.a: ${F_OBJS} 
	${AR} rc  $@  $^ 
	${STRIP_COMMAND}

idirs:
	@ [ -d ${D_FOR_LIB_INSTALL} ] || mkdir -p ${D_FOR_LIB_INSTALL}
	@ [ -d ${D_FOR_INC_INSTALL} ] || mkdir -p ${D_FOR_INC_INSTALL}

install: all idirs ${F_INSTALLED_LIBS} 

${D_FOR_LIB_INSTALL}/%.a : %.a
	install -m 0755 $< $@ 

${D_FOR_LIB_INSTALL}/%.so : %.so
	install -m 0755 $< $@ 

${F_PREBUILT_OBJS} : %.o : %.obj
	${BDVD_PPRINT.cp}
	@cp $^ $@
	
copy:	
	@echo [Copying proper bcrypt lib version...]
	cp -f $(BSEAV_TOP)/lib/security/bcrypt/lib/libbcrypt.so $(NEXUS_TOP)/bin
	