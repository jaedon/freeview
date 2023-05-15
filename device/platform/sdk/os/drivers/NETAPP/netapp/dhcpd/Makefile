############################################################################
#    (c)2006-2012 Broadcom Corporation
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
# $brcm_Revision: 5 $
# $brcm_Date: 2/3/12 4:18p $
# 
# Module Description:
# 
# Revision History:
#
# $brcm_Log: $
#
############################################################################
#
# DESCRIPTION:
#   Makefile for building a dhcp server static library
#
############################################################################
NETAPP_TOP		:= ../..
LIB_NAME 		:= $(shell basename ${CURDIR})
include ${NETAPP_TOP}/build/common.inc

EXCLUDED_SRCS	:= test.c
F_PUBLIC_INCS   := include/dhcp.h
INC_DIRS 		+= common include
SRC_DIRS 		+= common linux
F_SRCS 			:= $(filter-out \
           			$(addprefix %/, ${EXCLUDED_SRCS}), \
               	 		$(foreach d,${SRC_DIRS},$(wildcard ${d}/*.c ${d}/*.cpp)))

F_OBJS_COMPILED := $(patsubst %.c,%.o, $(patsubst %.cpp,%.o, ${F_SRCS}))
F_OBJS          += ${F_OBJS_PRECOMPILED} ${F_OBJS_COMPILED}
F_PUBLIC_LIBS	+= lib${LIB_NAME}.a
F_INSTALLED_LIBS+= $(addprefix ${NETAPP_OUTPUT_LIB_DIR}/, $(notdir ${F_PUBLIC_LIBS}))
CFLAGS			+= $(addprefix -I, ${INC_DIRS}) -fPIC -D_REENTRANT -I${NETAPP_OUTPUT_INC_DIR}
F_INSTALLED_INCS+= $(addprefix ${NETAPP_OUTPUT_INC_DIR}/, $(notdir ${F_PUBLIC_INCS}))

install: ${F_INSTALLED_LIBS} ${F_INSTALLED_INCS}

${F_INSTALLED_INCS}: ${NETAPP_OUTPUT_INC_DIR}/%.h : %.h
	${Q_}install  -m 0664 $< $@

${NETAPP_OUTPUT_LIB_DIR}/%.a : %.a
	${Q_}install -m 0755 $< $@ 

lib${LIB_NAME}.a: ${F_OBJS}
	@printf "[${LD}] -------------> %s \n" $@
	${Q_}${LD} -r -o $@ $^

distclean: clean
	
clean:
	@echo "======== Cleaning ${LIB_NAME}"
	${Q_}rm -f ${F_OBJS} ${F_INSTALLED_LIBS} ${F_PUBLIC_LIBS} ${F_INSTALLED_INCS}

############################################################################
#                             DEP FILES, PHONY
#vvvv#####################vvvvvvvvvvvvvvvvvvvvvvv#####################vvvv##
-include *.d
.PHONY: start install clean distclean
#^^^^#####################^^^^^^^^^^^^^^^^^^^^^^^#####################^^^^##
vpath %.h ${INC_DIRS}
	