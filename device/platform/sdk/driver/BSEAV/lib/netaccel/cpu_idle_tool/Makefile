############################################################
#     Copyright (c) 2003-2012, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: Makefile $
# $brcm_Revision: 12 $
# $brcm_Date: 3/9/12 6:44p $
############################################################

ifeq ($(KBUILD_EXTMOD),)

ifeq ($(LINUXDIR),)
LINUXDIR = $(LINUX)
endif

ARCH ?= mips

all:
	make ARCH=$(ARCH) -C $(LINUXDIR) KBUILD_EXTMOD=$(shell pwd)

clean:
	make  ARCH=$(ARCH)-C $(LINUXDIR) clean KBUILD_EXTMOD=$(shell pwd)

else

ifeq ($(findstring $(PLATFORM),97420 97410 97409 93380sms 93380vms 97425 97422 97346 97435), $(PLATFORM))
    EXTRA_CFLAGS += -DB_PERF_BMIPS_5000
else
    EXTRA_CFLAGS += -DB_PERF_BMIPS_4380
endif

EXTRA_FLAGS += -ggdb -fno-inline -O0 -Wall

#If you want to get the system wide MIPS performance stats (i/d, L2, RAC caches miss, hit, # of cycles & instructions), 
# enable this define.
#EXTRA_CFLAGS += -DBMIPS_PERFC_ENABLE

#Also, if you want to get the MIPS performance stats for a specific kernel module or specific portion of base kernel, 
# you will need to also enabled this define.
#In addition, you will also need to modify kernel to invoke snapshot calls (Look in the README file).
# EXTRA_CFLAGS += -DPERFC_INSTRUMENT_KERNEL

obj-m += bcm_perf_drv.o
bcm_perf_drv-objs += bmips_idle_stats.o bmips_perf_stats.o bmips_perf_drv.o
endif
