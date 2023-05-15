############################################################
#     Copyright (c) 2003-2008, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: svp.mak $
# $brcm_Revision: 14 $
# $brcm_Date: 12/31/08 10:27a $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /BSEAV/app/brutus/build/svp.mak $
# 
# 14   12/31/08 10:27a dbayani
# PR 50782: Move SVP functions in application and PI layer to xpt PI
# layer.
# 
# 13   7/9/08 9:51a dbayani
# PR 42686: clean up makefile.
# 
# 12   3/3/08 11:36a dbayani
# PR 38074: Udpate to build correctly for SVP.
# 
# 11   2/29/08 2:01p dbayani
# PR 38074: restructed SVP build.
# 
# 10   12/7/07 4:13p dbayani
# PR 37935: Moved svplib files to bsvplib.inc.
# 
# 9   8/15/07 4:39p dbayani
# PR 34096: Fix compile errors for SVP.
# 
# 8   6/20/07 1:28p dbayani
# PR 32380: Updated with latest SVP drivers.
# 
# 7   6/8/07 11:11a dbayani
# PR 31882: Updated SVP HDI drivers.
# 
# 6   6/1/07 9:01a dbayani
# PR 31742: Removed key ladder define from SVP build.
# 
# 5   5/25/07 12:02p dbayani
# PR 31601: Update make file for SVP components.
# 
# 4   5/25/07 10:01a dbayani
# PR 31596: Added SVP support for 7118.
# 
# 3   3/28/07 5:23p dbayani
# PR 29157: Cleaned up makefile a little.
# 
# 2   3/2/07 3:54p dbayani
# PR 28295: Move SVP out of XPT.
# 
# 1   1/3/07 7:55p dbayani
# PR 26537: Added support for SVP.
#  
############################################################

#
# This file to be included by Makefile.brutus
#

ICM_RPC ?=n
ICM_BRCM_KEY_LADDER ?=y
ICM_NEXUS_BUILD ?=y



#
# This is internal test code.
#
ROCKFORD=${MAGNUM}/../rockford

	CFLAGS += -DSVP_TEST \
	    -I$(ROCKFORD)/applications/diags/bcm9$(CHIP)/no-os/src/svp_hdi \
	    -I$(MAGNUM)/portinginterface/xpt/$(BCHP_CHIP)/svp 

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
		bicm_keyladder.o 
endif	    
	    
	   
	


	VPATH += $(ROCKFORD)/applications/diags/bcm9$(CHIP)/no-os/src/icm_cli \
		$(ROCKFORD)/applications/diags/bcm9$(CHIP)/no-os/src/icm_cli/verifiertest
		
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
		
		
ifeq ($(ICM_BRCM_KEY_LADDER),y)
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
		
		
	CFLAGS += -DBHSM_AEGIS_SUPPORT -DBICM_ICAM2 -DBICM_LINUX_USER_MODE_DRV -DBCHP_CHIP=$(CHIP)

					
	CFLAGS += -DBSP_SVP -DSVP_TEST -DBSP_GVP -DBSP_NDS -D__LINUX__ -DSOCKET_GLUE_SERVER -D__ghs -D__LittleEndian -DXRPC_MODULES_SERVER \
		-I$(ROCKFORD)/applications/diags/bcm$(PLATFORM)/no-os/src/icm_cli \
		-I$(ROCKFORD)/applications/diags/bcm$(PLATFORM)/no-os/src/icm_cli/verifiertest \
		-I$(ROCKFORD)/unittests/portinginterface/hsm/$(CHIP)/$(PLATFORM_PATH)/nsk_cmd_test/rpc/server/Inc \
		-I$(ROCKFORD)/unittests/portinginterface/hsm/$(CHIP)/$(PLATFORM_PATH)/nsk_cmd_test/rpc/XOSGlue/inc \
		-I$(ROCKFORD)/unittests/portinginterface/hsm/$(CHIP)/$(PLATFORM_PATH)/nsk_cmd_test/rpc/xremotemsg/inc \
		-I$(ROCKFORD)/unittests/portinginterface/hsm/$(CHIP)/$(PLATFORM_PATH)/nsk_cmd_test/rpc/linux \

	

	VPATH += $(ROCKFORD)/applications/diags/bcm9$(CHIP)/no-os/src/svp_hdi \
		$(ROCKFORD)/unittests/portinginterface/hsm/$(CHIP)/$(PLATFORM_PATH)/nsk_cmd_test/rpc/server/Src \
		$(ROCKFORD)/unittests/portinginterface/hsm/$(CHIP)/$(PLATFORM_PATH)/nsk_cmd_test/rpc/xremotemsg/src \
		$(ROCKFORD)/unittests/portinginterface/hsm/$(CHIP)/$(PLATFORM_PATH)/nsk_cmd_test/rpc/linux \
		$(MAGNUM)/portinginterface/hsm/$(CHIP)/$(PLATFORM_PATH)/nsk_cmd \
		$(MAGNUM)/portinginterface/xpt/$(BCHP_CHIP)/svp

				
	APP_OBJECTS += bsvp_hdi_util.o \
		bsvp_hdi_io.o \
		bsvp.o \
		bsvp_priv.o \
		XOBJECT_structs.o \
		XOBJECT_basic.o \
		DFG_debugTask.o \
		XDFG_Debug.o \
		XDFG_nskhal.o \
		XDFG_nskldi.o \
		XDFG_svpldi.o \
		XDFG_reset.o \
		XDFG_qc_tests.o \
		XDFG_cahdiver.o \
		XDFG_harm_tests.o \
		linux_os.o \
		p_msg.o \
		p_sema.o \
		nskldi.o \
		svpldi.o \
		reset.o \
		QC_Tests.o \
		harm_tests.o \
		nskhal.o
															
    ifeq ($(ICM_RPC), y)
    	CFLAGS += -DBSP_RPC -DQC_svpldi -DQC_reset -DQC_qc_tests -DQC_nskldi -DQC_nskhal -DQC_harm_tests -DQC_cahdiver
    	
    	APP_OBJECTS += IRDSockGlue.o \
			XRPC_modules_main.o \
			XRPC.o \
			XRPC_server_nskhal.o \
			XRPC_server_nskldi.o \
			XRPC_server_svpldi.o \
			XRPC_server_reset.o \
			XRPC_server_qc_tests.o \
			XRPC_server_cahdiver.o \
			XRPC_server_harm_tests.o \
			XRemoteMsg.o \
			XCom.o \
			XComSend.o \
			XComRcv.o
	endif

	FAKE_ICAM_OBJ = icamstub.o
	LIB_ICAM_VERIFIER_FLAG = -L$(ROCKFORD)/applications/diags/bcm9$(CHIP)/no-os/src/icm_cli/icamverifier  -lx
	BRUTUS_DIRS += $(ROCKFORD)/applications/diags/bcm97038/no-os/src/icm_cli/verifiertest		

	APP_OBJECTS += icamstub.o



