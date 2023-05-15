############################################################
#     Copyright (c) 2003-2012, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: icam.mak $
# $brcm_Revision: 1 $
# $brcm_Date: 1/12/12 7:01p $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /BSEAV/app/brutus2/build/icam.mak $
# 
# 1   1/12/12 7:01p tokushig
# SW7405-5581: Brutus2 initial check in
#  
############################################################

#
# This file to be included by Makefile.brutus
#

ICM_CLI_SUPPORT ?= n
ICM_BRCM_KEY_LADDER ?=n
ICAM_SCR3_TEST ?=y
ICM_NEXUS_BUILD ?=y

ROCKFORD=${MAGNUM}/../rockford

ifeq ($(ICM_NEXUS_BUILD),y)
 
	CFLAGS += -I$(MAGNUM)/portinginterface/xpt/$(BCHP_CHIP)/icm

	CFLAGS += -DBICM_ICAM2 -DBICM_LINUX_USER_MODE_DRV -DBICM_BRCM_KEY_LADDER

	VPATH += $(MAGNUM)/portinginterface/xpt/$(BCHP_CHIP)/icm/

	APP_OBJECTS += \
		bicm.o	\
		bicm_ecm.o	\
		bicm_emm.o	\
		bicm_sc.o	\
		bicm_ecm_priv.o	\
		bicm_emm_priv.o	\
		bicm_sc_priv.o	\
		bicm_priv.o	\
		bicm_plat_${BCHP_CHIP}.o \
		bicm_keyladder.o \
		bicm_portingApi_priv.o 
endif




ifeq ($(ICM_CLI_SUPPORT),y)
	VPATH += $(ROCKFORD)/applications/diags/bcm$(PLATFORM)/no-os/src/icm_cli \
			$(ROCKFORD)/applications/diags/bcm$(PLATFORM)/no-os/src/icm_cli/verifiertest
			
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

	ifeq ($(ICAM_SCR3_TEST),y)
		CFLAGS += -DICAM_SCR3_TEST
		APP_OBJECTS += \
			catest_scr3.o 
	else
		APP_OBJECTS += \
			catest.o 
	endif

	ifeq ($(ICM_BRCM_KEY_LADDER), y)
			CFLAGS += -DBICM_BRCM_KEY_LADDER
			APP_OBJECTS += \
				bicm_cwencryp.o \
				cwencryp.o
	endif

	ifeq ($(ICM_NEXUS_BUILD),y)
		BCHP_CHIP = $(CHIP)
		MAGNUM_MODULES :=
		OS =linuxuser
		
		include $(MAGNUM)/basemodules/dbg/bdbg.inc
		include $(MAGNUM)/basemodules/int/bint.inc  
		include $(MAGNUM)/basemodules/mem/bmem.inc  
		include $(MAGNUM)/basemodules/std/bstd.inc	
		include $(MAGNUM)/basemodules/tmr/btmr.inc
		include $(MAGNUM)/basemodules/chp/bchp.inc	     
		include $(MAGNUM)/basemodules/err/berr.inc  
		include $(MAGNUM)/basemodules/kni/bkni.inc  
		include $(MAGNUM)/basemodules/reg/breg.inc  
		include $(MAGNUM)/basemodules/test/btst.inc

		include $(MAGNUM)/commonutils/avc/bavc.inc
		include $(MAGNUM)/commonutils/img/bimg.inc
		#include $(MAGNUM)/commonutils/mth/bmth.inc
		#include $(MAGNUM)/commonutils/rdc/brdc.inc
		include $(MAGNUM)/commonutils/sur/bsur.inc
		include $(MAGNUM)/commonutils/fmt/bfmt.inc
		include $(MAGNUM)/commonutils/lst/blst.inc
		#include $(MAGNUM)/commonutils/mrc/bmrc.inc
		include $(MAGNUM)/commonutils/pxl/bpxl.inc
		#include $(MAGNUM)/commonutils/rsp/brsp.inc
		        
		#include $(MAGNUM)/portinginterface/dgp/bdgp.inc
		#include $(MAGNUM)/portinginterface/grc/bgrc.inc  
		#include $(MAGNUM)/portinginterface/i2c/bi2c.inc	
		#include $(MAGNUM)/portinginterface/kir/bkir.inc  
		#include $(MAGNUM)/portinginterface/lna/blna.inc
		#include $(MAGNUM)/portinginterface/rap/brap.inc	
		#include $(MAGNUM)/portinginterface/sds/bsds.inc  
		#include $(MAGNUM)/portinginterface/tnr/btnr.inc  
		#include $(MAGNUM)/portinginterface/vbi/bvbi.inc  
		include $(MAGNUM)/portinginterface/xpt/bxpt.inc
		include $(MAGNUM)/portinginterface/dma/bdma.inc  
		#include $(MAGNUM)/portinginterface/hdm/bhdm.inc  
		#include $(MAGNUM)/portinginterface/icp/bicp.inc	
		#include $(MAGNUM)/portinginterface/kpd/bkpd.inc  
		#include $(MAGNUM)/portinginterface/rfm/brfm.inc	
		#include $(MAGNUM)/portinginterface/spi/bspi.inc  
		#include $(MAGNUM)/portinginterface/uhf/buhf.inc  
		#include $(MAGNUM)/portinginterface/vdc/bvdc.inc  
		#include $(MAGNUM)/portinginterface/xvd/bxvd.inc
		#include $(MAGNUM)/portinginterface/ast/bast.inc  
		include $(MAGNUM)/portinginterface/gio/bgio.inc  
		include $(MAGNUM)/portinginterface/hsm/bhsm.inc  
		#include $(MAGNUM)/portinginterface/irb/birb.inc	
		#include $(MAGNUM)/portinginterface/led/bled.inc  
		#include $(MAGNUM)/portinginterface/pwm/bpwm.inc	 
		#include $(MAGNUM)/portinginterface/scd/bscd.inc	
		#include $(MAGNUM)/portinginterface/thd/bthd.inc  
		#include $(MAGNUM)/portinginterface/urt/burt.inc  
		#include $(MAGNUM)/portinginterface/vsb/bvsb.inc

		CFLAGS +=$(addprefix -I,$(sort $(foreach module, $(MAGNUM_MODULES), $($(module)_INCLUDES))))
		CFLAGS += -I${MAGNUM}/../BSEAV/linux/driver/usermode 

		CFLAGS +=  -DNEXUS_BUILD
	else
		include $(BSEAV)/api/build/magnum/Makefile.inc
		include $(ROCKFORD)/inc/compose.mak
	endif	

	CFLAGS += -DBHSM_AEGIS_SUPPORT -DBSP_NDS -DBICM_ICAM2 -DBICM_LINUX_USER_MODE_DRV -DBCHP_CHIP=$(CHIP) \
				-I$(ROCKFORD)/applications/diags/bcm$(PLATFORM)/no-os/src/icm_cli \
				-I$(ROCKFORD)/applications/diags/bcm$(PLATFORM)/no-os/src/icm_cli/verifiertest 

	ifeq ($(APP_NAME),brutus_fakeicam)
		APP_OBJECTS += icamstub.o
	endif
	ifeq ($(APP_NAME),brutus_realicam)
#	LDFLAGS += -L$(ROCKFORD)/applications/diags/bcm$(PLATFORM)/no-os/src/icm_cli/icamverifier  -lx25
#	LIBS += $(ROCKFORD)/applications/diags/bcm$(PLATFORM)/no-os/src/icm_cli/icamverifier/libx.a
#	LDFLAGS += -L$(ROCKFORD)/applications/diags/bcm$(PLATFORM)/no-os/src/icm_cli/icamverifier  -lverifier
#	LDFLAGS += -L$(ROCKFORD)/applications/diags/bcm$(PLATFORM)/no-os/src/icm_cli/icamverifier  -lverifier_debug
	LDFLAGS += -L$(ROCKFORD)/applications/diags/bcm$(PLATFORM)/no-os/src/icm_cli/icamverifier  -lverifier_linux_2_6_18_0_7325
	endif
	
endif




