############################################################
#     Copyright (c) 2003-2012, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: smartcard.mak $
# $brcm_Revision: 1 $
# $brcm_Date: 1/12/12 7:02p $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /BSEAV/app/brutus2/build/smartcard.mak $
# 
# 1   1/12/12 7:02p tokushig
# SW7405-5581: Brutus2 initial check in
# 
# 16   3/14/08 3:53p qcheng
# PR40402: Add ICAM support to BCM7325 A0
# 
# 15   2/19/08 1:06p qxie
# PR39273: Smartcard: smartcard.mak made built broken
# 
# 14   2/15/08 7:23p qxie
# PR12345: add CWE test to 7038
# 
# 2   5/5/05 2:29p erickson
# PR15159: added quotes for DOS builds
# 
# 1   2/24/05 1:20p erickson
# PR14180: moved smartcard make outside regular makefile for readability
# 
############################################################

#
# This file to be included by Makefile.brutus
#
SCD_CLI_SUPPORT ?= n
ICM_CLI_SUPPORT ?= n
ICM_BRCM_KEY_LADDER ?=n
ifeq ($(CHIP),7038)
ICAM_SCR3_TEST ?=n
endif

ifeq ($(CHIP),7038)
	ifeq ($(SCD_CLI_SUPPORT),y)
# Smartcard applications
ROCKFORD=${MAGNUM}/../rockford
BSEAV=${MAGNUM}/../BSEAV
#ROCKFORD=$(shell  cd $(BSEAV)/../rockford && {PWD})
VPATH += $(ROCKFORD)/applications/diags/bcm97038/no-os/src/scd_cli
vpath %.h $(VPATH)
vpath %.c $(VPATH)
		APP_OBJECTS += \
			bscd_cli_aribtests.o \
			bscd_cli_conaxtests.o \
			bscd_cli_estests.o \
			bscd_cli_infra.o \
			bscd_cli_irdetotests.o \
			bscd_cli_isotests.o \
			bscd_cli_main.o \
			bscd_cli_mttests.o \
			bscd_cli_ndstests.o \
			bscd_cli_pf.o \
			bscd_cli_states.o \
			bscd_cli_util.o
		CFLAGS += -DBICM_LINUX_USER_MODE_DRV -DSCD_CLI_SUPPORT -I$(MAGNUM)/portinginterface/scd/$(CHIP) -I$(MAGNUM)/basemodules/int  -I$(MAGNUM)/basemodules/chp \
				-I$(MAGNUM)/basemodules/chp/$(CHIP)  -I$(MAGNUM)/basemodules/chp/$(CHIP)/rdb \
				-I$(MAGNUM)/basemodules/chp/$(CHIP)/rdb/$(PLATFORM_PATH) \
				-I$(ROCKFORD)/commondrivers/fpga/$(PLATFORM) \
				-I$(MAGNUM)/basemodules/reg \
				-I$(MAGNUM)/portinginterface/urt/$(CHIP) \
				-I$(BSEAV)/api/src -I$(BSEAV)/api/src/magnum \
				-I$(MAGNUM)/portinginterface/vdc/$(CHIP) \
				-I$(MAGNUM)/commonutils/sur/$(CHIP) \
				-I$(MAGNUM)/portinginterface/hdm/$(CHIP) \
				-I$(MAGNUM)/portinginterface/mvd/$(CHIP)  \
				-I$(MAGNUM)/portinginterface/rfm/$(CHIP)  \
				-I$(MAGNUM)/portinginterface/grc/$(CHIP)  \
				-I$(MAGNUM)/portinginterface/vbi/$(CHIP)  \
				-I$(MAGNUM)/syslib/vbilib \
				-I$(MAGNUM)/syslib/btslib \
				-I$(MAGNUM)/portinginterface/aud/$(CHIP)  \
				-I$(MAGNUM)/basemodules/mem \
				-I$(MAGNUM)/commonutils/rdc/$(CHIP) \
				-I$(MAGNUM)/commonutils/fmt \
				-I$(MAGNUM)/commonutils/pxl \
				-I$(MAGNUM)/commonutils/avc/$(CHIP) \
				-I$(MAGNUM)/syslib/hdmlib \
				-I$(MAGNUM)/syslib/pcrlib/$(CHIP) \
				-I$(MAGNUM)/portinginterface/i2c/$(CHIP) \
				-I$(ROCKFORD)/commondrivers/fpga/97038 \
				-I$(MAGNUM)/basemodules/mem/$(CHIP) \
				-I$(MAGNUM)/portinginterface/qus/3250 \
				-I$(MAGNUM)/portinginterface/qds/3250 \
				-I$(MAGNUM)/portinginterface/qob/3250 \
				-I$(MAGNUM)/portinginterface/tnr \
				-I$(MAGNUM)/portinginterface/xpt/$(CHIP) \
				-I$(ROCKFORD)/bsp/bcm97038/common \
				-I$(MAGNUM)/portinginterface/sds/4500 \
				-I$(MAGNUM)/portinginterface/vsb/3510 \
				-I$(MAGNUM)/portinginterface/lna/3405 \
				-I$(MAGNUM)/portinginterface/ifd/$(CHIP) \
				-I$(MAGNUM)/portinginterface/kpd/3250 \
				-I$(MAGNUM)/portinginterface/led/3250 \
				-I$(MAGNUM)/portinginterface/kir/$(CHIP) \
				-I$(ROCKFORD)/applications/diags/bcm97038/no-os/src/ -I../src/common \
				-I$(BSEAV)/lib/otfpvr
		BRUTUS_DIRS += ../src/scd_cli
	endif
endif

#
# This is internal test code.
#
ifeq ($(CHIP),7038)
	ifeq ($(ICM_CLI_SUPPORT),y)
# Smartcard applications
#		ROCKFORD=$(shell  cd $(BSEAV)/../rockford && {PWD})
		ROCKFORD=${MAGNUM}/../rockford
		VPATH += $(ROCKFORD)/applications/diags/bcm97038/no-os/src/icm_cli \
				$(ROCKFORD)/applications/diags/bcm97038/no-os/src/icm_cli/verifiertest
				
		APP_OBJECTS += \
			bicm_cli_cahdiecm.o  \
			bicm_cli_cahdiemm.o \
			bicm_cli_cahdisc.o \
			bicm_cli_infra.o \
			bicm_cli_main.o \
			bicm_cli_util.o \
			arg.o \
			cadata.o \
			catest1.o \
			catest2.o \
			catest3.o

		ifeq ($(ICM_BRCM_KEY_LADDER), y)
				CFLAGS += -DBICM_BRCM_KEY_LADDER
				APP_OBJECTS += \
					cwencryp.o
		endif
		ifeq ($(ICAM_SCR3_TEST), y)
				CFLAGS += -DICAM_SCR3_TEST
				APP_OBJECTS += \
					catest_scr3.o
		else
				APP_OBJECTS += \
					catest.o
				
		endif


		CFLAGS += -DBICM_ICAM2 -DBICM_LINUX_USER_MODE_DRV -DBCHP_CHIP=7038 -I$(MAGNUM)/portinginterface/xpt/$(CHIP) \
				-I$(MAGNUM)/portinginterface/xpt/$(CHIP)/icm/ 	-I$(ROCKFORD)/bsp/bcm97038/common \
					-I$(MAGNUM)/basemodules/int  -I$(MAGNUM)/basemodules/int/$(CHIP) \
					-I$(MAGNUM)/basemodules/tmr/$(CHIP) \
					-I$(ROCKFORD)/commondrivers/fpga/$(PLATFORM) \
					-I$(BSEAV)/api/src -I$(BSEAV)/api/src/magnum \
					-I$(MAGNUM)/basemodules/chp -I$(MAGNUM)/basemodules/mem \
					-I$(MAGNUM)/commonutils/avc/$(CHIP) -I$(MAGNUM)/basemodules/chp/$(CHIP) \
					-I$(MAGNUM)/commonutils/sur/$(CHIP) \
					-I$(MAGNUM)/commonutils/fmt \
					-I$(MAGNUM)/commonutils/rdc/$(CHIP) \
					-I$(MAGNUM)/commonutils/fmt \
					-I$(MAGNUM)/commonutils/pxl \
					-I$(MAGNUM)/commonutils/img \
					-I$(MAGNUM)/syslib/pcrlib/$(CHIP) \
					-I$(MAGNUM)/basemodules/int \
					-I$(MAGNUM)/basemodules/int/7411 \
					-I$(MAGNUM)/basemodules/arc \
					-I$(MAGNUM)/basemodules/reg/7411 \
					-I$(MAGNUM)/basemodules/chp/7411 \
					-I$(MAGNUM)/commonutils/mrc/$(CHIP) \
					-I$(MAGNUM)/syslib/vbilib \
					-I$(MAGNUM)/syslib/btslib \
					-I$(MAGNUM)/basemodules/mem/$(CHIP)  \
					-I$(MAGNUM)/portinginterface/vdc/$(CHIP) \
					-I$(MAGNUM)/portinginterface/hdm/$(CHIP) \
					-I$(MAGNUM)/portinginterface/rfm/$(CHIP) \
					-I$(MAGNUM)/basemodules/chp/$(CHIP)/rdb/$(PLATFORM_PATH) \
					-I$(MAGNUM)/portinginterface/dma/$(CHIP)  \
					-I$(MAGNUM)/portinginterface/i2c/$(CHIP) \
					-I$(MAGNUM)/portinginterface/aud/$(CHIP)  \
					-I$(MAGNUM)/portinginterface/led/3250  \
					-I$(MAGNUM)/portinginterface/lna \
					-I$(MAGNUM)/portinginterface/mvd/$(CHIP)  \
					-I$(MAGNUM)/portinginterface/grc/$(CHIP)  \
					-I$(MAGNUM)/portinginterface/qus/3250 \
					-I$(MAGNUM)/portinginterface/qds/3250 \
					-I$(MAGNUM)/portinginterface/qob/3250 \
					-I$(MAGNUM)/portinginterface/tnr \
					-I$(MAGNUM)/portinginterface/vbi/$(CHIP)  \
					-I$(MAGNUM)/portinginterface/sds/4500 \
					-I$(MAGNUM)/portinginterface/vsb/3510 \
					-I$(MAGNUM)/portinginterface/xvd/7411 \
					-I$(MAGNUM)/portinginterface/rap/7411 \
					-I$(MAGNUM)/portinginterface/kpd/$(CHIP) \
					-I$(MAGNUM)/portinginterface/spi/$(CHIP)  \
					-I$(MAGNUM)/portinginterface/kir/$(CHIP)  \
					-I$(MAGNUM)/portinginterface/ifd/$(CHIP)  \
					-I$(MAGNUM)/syslib/hdmlib/ \
					-I$(MAGNUM)/basemodules/chp/$(CHIP)/rdb -I$(MAGNUM)/basemodules/reg \
					-I$(BSEAV)/lib/otfpvr \
					-I$(ROCKFORD)/applications/diags/bcm97038/no-os/src/icm_cli \
					-I$(ROCKFORD)/applications/diags/bcm97038/no-os/src/icm_cli/verifiertest 

		FAKE_ICAM_OBJ = icamstub.o
		LIB_ICAM_VERIFIER_FLAG = -L$(ROCKFORD)/applications/diags/bcm97038/no-os/src/icm_cli/icamverifier  -lx25
		BRUTUS_DIRS += $(ROCKFORD)/applications/diags/bcm97038/no-os/src/icm_cli/verifiertest		
	endif
endif

#
# This is internal test code.
#
ifeq ($(CHIP),7401)
	ifeq ($(ICM_CLI_SUPPORT),y)
# Smartcard applications
#		ROCKFORD=$(shell  cd $(BSEAV)/../rockford && {PWD})
		ROCKFORD=${MAGNUM}/../rockford
		VPATH += $(ROCKFORD)/applications/diags/bcm97401/no-os/src/icm_cli \
				$(ROCKFORD)/applications/diags/bcm97401/no-os/src/icm_cli/verifiertest
				
		APP_OBJECTS += \
			bicm_cli_cahdiecm.o  \
			bicm_cli_cahdiemm.o \
			bicm_cli_cahdisc.o \
			bicm_cli_infra.o \
			bicm_cli_main.o \
			bicm_cli_util.o \
			arg.o \
			cadata.o \
			catest.o \
			catest1.o \
			catest2.o \
			catest3.o

		ifeq ($(ICM_BRCM_KEY_LADDER), y)
				CFLAGS += -DBICM_BRCM_KEY_LADDER
				APP_OBJECTS += \
					cwencryp.o
		endif


		CFLAGS += -DBHSM_AEGIS_SUPPORT -DBICM_ICAM2 -DBICM_LINUX_USER_MODE_DRV -DBCHP_CHIP=7401 -I$(MAGNUM)/portinginterface/xpt/$(CHIP) \
				-I$(MAGNUM)/portinginterface/xpt/$(CHIP)/icm/ 	-I$(ROCKFORD)/bsp/bcm97401/common \
					-I$(MAGNUM)/basemodules/int  -I$(MAGNUM)/basemodules/int/$(CHIP) \
					-I$(BSEAV)/api/src -I$(BSEAV)/api/src/magnum \
					-I$(MAGNUM)/basemodules/chp -I$(MAGNUM)/basemodules/mem \
					-I$(MAGNUM)/commonutils/avc/$(CHIP) -I$(MAGNUM)/basemodules/chp/$(CHIP) \
					-I$(MAGNUM)/commonutils/sur/$(CHIP) \
					-I$(MAGNUM)/commonutils/fmt \
					-I$(MAGNUM)/commonutils/rdc/$(CHIP) \
					-I$(MAGNUM)/commonutils/fmt \
					-I$(MAGNUM)/commonutils/pxl \
					-I$(MAGNUM)/commonutils/img \
					-I$(MAGNUM)/syslib/pcrlib/$(CHIP) \
					-I$(MAGNUM)/basemodules/int \
					-I$(MAGNUM)/basemodules/int/7411 \
					-I$(MAGNUM)/basemodules/arc \
					-I$(MAGNUM)/basemodules/reg/7411 \
					-I$(MAGNUM)/basemodules/chp/7411 \
					-I$(MAGNUM)/syslib/vbilib \
					-I$(MAGNUM)/syslib/btslib \
					-I$(MAGNUM)/basemodules/mem/$(CHIP)  \
					-I$(MAGNUM)/portinginterface/vdc/$(CHIP) \
					-I$(MAGNUM)/portinginterface/hdm/$(CHIP) \
					-I$(MAGNUM)/portinginterface/rfm/$(CHIP) \
					-I$(MAGNUM)/basemodules/chp/$(CHIP)/rdb/$(PLATFORM_PATH) \
					-I$(MAGNUM)/portinginterface/i2c/$(CHIP) \
					-I$(MAGNUM)/portinginterface/aud/$(CHIP)  \
					-I$(MAGNUM)/portinginterface/mvd/$(CHIP)  \
					-I$(MAGNUM)/portinginterface/grc/$(CHIP)  \
					-I$(MAGNUM)/portinginterface/vbi/$(CHIP)  \
					-I$(MAGNUM)/portinginterface/sds/4500 \
					-I$(MAGNUM)/portinginterface/vsb/3510 \
					-I$(MAGNUM)/portinginterface/xvd/7411 \
					-I$(MAGNUM)/portinginterface/rap/7411 \
					-I$(MAGNUM)/portinginterface/kpd/$(CHIP) \
					-I$(MAGNUM)/portinginterface/spi/$(CHIP)  \
					-I$(MAGNUM)/portinginterface/kir/$(CHIP)  \
					-I$(MAGNUM)/portinginterface/ifd/$(CHIP)  \
					-I$(MAGNUM)/portinginterface/led/$(CHIP) \
					-I$(MAGNUM)/syslib/hdmlib/ \
					-I$(MAGNUM)/basemodules/chp/$(CHIP)/rdb -I$(MAGNUM)/basemodules/reg \
					-I$(BSEAV)/lib/otfpvr \
					-I$(ROCKFORD)/commondrivers/fpga/$(PLATFORM) \
					-I$(ROCKFORD)/applications/diags/bcm97401/no-os/src/icm_cli \
					-I$(ROCKFORD)/applications/diags/bcm97401/no-os/src/icm_cli/verifiertest \
					-I$(MAGNUM)/portinginterface/xpt/$(CHIP)/hsm/$(BCHP_VER)/common \
					-I$(MAGNUM)/portinginterface/xpt/$(CHIP)/hsm/$(BCHP_VER)/common/aegis \
					-I$(MAGNUM)/portinginterface/xpt/$(CHIP)/hsm/$(BCHP_VER)/keyladder \
					-I$(MAGNUM)/portinginterface/xpt/$(CHIP)/hsm/$(BCHP_VER)/keyladder/aegis \
					-I$(MAGNUM)/portinginterface/xpt/$(CHIP)/hsm/$(BCHP_VER)/cust_n \
					-I$(MAGNUM)/portinginterface/xpt/$(CHIP)/hsm/$(BCHP_VER)/cust_n/aegis \
					-I$(MAGNUM)/portinginterface/xpt/$(CHIP)/hsm/$(BCHP_VER)/debug_rdwr \
					-I$(MAGNUM)/portinginterface/xpt/$(CHIP)/hsm/$(BCHP_VER)/otpmsp


		FAKE_ICAM_OBJ = icamstub.o
		LIB_ICAM_VERIFIER_FLAG = -L$(ROCKFORD)/applications/diags/bcm97401/no-os/src/icm_cli/icamverifier  -lx
		BRUTUS_DIRS += $(ROCKFORD)/applications/diags/bcm97038/no-os/src/icm_cli/verifiertest		
	endif
endif
