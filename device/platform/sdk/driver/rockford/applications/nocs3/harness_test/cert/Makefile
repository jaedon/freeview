############################################################
#     (c)2003-2013 Broadcom Corporation
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
#  1.     This program, including its structure, sequence and organization, constitutes the valuable trade
#  secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
#  and to use this information only in connection with your use of Broadcom integrated circuit products.
#   
#  2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS" 
#  AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR 
#  WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO 
#  THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES 
#  OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, 
#  LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION 
#  OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF 
#  USE OR PERFORMANCE OF THE SOFTWARE.
#  
#  3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS 
#  LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR 
#  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR 
#  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF 
#  THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT 
#  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE 
#  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF 
#  ANY LIMITED REMEDY.
# 
# $brcm_Workfile: Makefile $
# $brcm_Revision: Hydra_Software_Devel/5 $
# $brcm_Date: 4/8/13 11:58p $
#
# Module Description:
#
# Revision History:
#
# Created: 08/28/2007 by Jon Garrett
#
# $brcm_Log: /rockford/applications/nocs3/harness_test/cert/Makefile $
# 
# Hydra_Software_Devel/5   4/8/13 11:58p jipeng
# SWSECURITY-251: nocs3 build with old and new Refsw views
# 
# Hydra_Software_Devel/4   12/13/12 1:31p zqian
# SW7346-555: Nocs Integration - fix wrong compile problem in BE
# 
# Hydra_Software_Devel/3   10/5/12 12:01p jipeng
# SWSECURITY-188: support NOCS3 CERT common functions
# 
# Hydra_Software_Devel/2   7/20/12 2:06p yili
# SW7346-555:NOCS3 integration
# 
# Hydra_Software_Devel/1   2/10/12 3:42p yili
# SW7346-555:NOCS3 certification
# 
# 20   6/5/08 10:27a ssood
# PR42739: Compiling playpump_ip app only for Linux platforms
# 
# 19   6/5/08 9:59a ssood
# PR42739: Added include path for Netaccel include files
# 
# 18   6/4/08 6:04p ssood
# PR42739: Adding Example App to demonstrate Live IP Playback using Nexus
#  Playpump Inteface
# 
# 17   5/21/08 12:03p rjlewis
# PR40352: Added VxWorks support.
# 
# 16   5/20/08 7:40p shyam
# PR 42103 : Add transport MPOD support
# 
# 15   5/20/08 7:37p shyam
# PR 42103 : Add transport MPOD support
# 
# 14   5/16/08 7:25p vishk
# PR 40034: Develop Nexus Smartcard Interface/Module.
# PR 40016: bsettop_smartcard: Develop SettopAPI-to-Nexus shim layer
# 
# 13   5/5/08 11:15a tokushig
# PR42421: removed psip example app - moved to
#  rockford/unittests/applibs/psip
# 
# 12   5/2/08 6:33p tokushig
# PR42421: inadvertant tab->space replacement fixed
# 
# 11   5/2/08 6:27p tokushig
# PR42421: add psip example
# 
# 10   4/4/08 4:02p erickson
# PR35551: separate qam and vsb tune examples
# 
# 9   4/3/08 10:53a erickson
# PR36068: always compile sync_channel
# 
# 8   3/24/08 7:02p mphillip
# PR39982: Update makefile for IR/UHF examples
# 
# 7   3/21/08 12:18p vsilyaev
# PR 39986: Added VideoImageInput
# 
# 6   3/13/08 7:41p jgarrett
# PR 39985: Adding tune_satellite
# 
# 5   3/11/08 4:38p jgarrett
# PR 39983: Adding audio_playback
# 
# 4   2/22/08 2:27p erickson
# PR36808: always copy examples to nexus/bin after building
# 
# 3   2/1/08 5:25p jgarrett
# PR 39017: Adding HdmiOutput
# 
# 2   1/23/08 9:47a erickson
# PR35198: add palettized graphics examples
# 
# 1   1/18/08 2:12p jgarrett
# PR 38808: Merging to main branch
# 
# Nexus_Devel/40   1/8/08 7:21p jgarrett
# PR 36159: Moving split_screen to DTV only
# 
# Nexus_Devel/39   1/8/08 10:57a erickson
# PR36159: refactor some 3563 Display API's
# 
# Nexus_Devel/38   12/20/07 2:30p jgarrett
# PR 38019: Adding tune_analog
# 
# Nexus_Devel/37   11/29/07 5:57p jgarrett
# PR 37778: Adding standard DTV platform defines
# 
# Nexus_Devel/36   11/29/07 2:24p erickson
# PR35457: doc update for 0.5 release
# 
# Nexus_Devel/35   11/26/07 11:46a jgarrett
# PR 37471: Adding i2s_input
# 
# Nexus_Devel/34   11/26/07 9:19a erickson
# PR37423: added gpio example
# 
# Nexus_Devel/33   11/26/07 1:44p gezhang
# PR36159: Bring up Nexus on 93563 platform
# 
# Nexus_Devel/31   11/21/07 11:13a erickson
# PR37423: added frontpanel example
# 
# Nexus_Devel/30   11/20/07 6:51p jgarrett
# PR 37130: Unifying 3563 frontend and platform
# 
# Nexus_Devel/29   11/12/07 6:49p jgarrett
# PR 36953: Adding 3563 defines
# 
# Nexus_Devel/28   11/9/07 6:06p vsilyaev
# PR 36788: Added timeshifting
# 
# Nexus_Devel/27   11/8/07 2:47p vsilyaev
# PR 36788: PVR record module
# 
# Nexus_Devel/26   11/8/07 9:09a erickson
# PR36802: added mosaic_decode
# 
# Nexus_Devel/25   10/23/07 6:07p vsilyaev
# PR 36199: Added S/G support to playpump
# 
# Nexus_Devel/24   10/19/07 9:39a erickson
# PR36275: added blit and fill
# 
# Nexus_Devel/23   10/18/07 10:27a erickson
# PR36152: always copy results into nexus/bin
# 
# Nexus_Devel/22   10/15/07 5:10p vsilyaev
# PR 35824: Added playback support
# 
# Nexus_Devel/21   10/10/07 3:04p erickson
# PR34926: created examples_lib to keep example apps simple
# 
# Nexus_Devel/20   10/10/07 12:04p erickson
# PR34926: added dual_decode
# 
# Nexus_Devel/19   10/10/07 11:34a jgarrett
# PR 35551: Adding tune
# 
# Nexus_Devel/18   10/9/07 2:04p erickson
# PR34925: added recpump
# 
# Nexus_Devel/17   10/5/07 1:53p erickson
# PR34926: add decode_video for incremental bringup
# 
# Nexus_Devel/16   10/3/07 2:50p erickson
# PR34925: replace relative paths
# 
# Nexus_Devel/15   10/3/07 1:05p erickson
# PR34926: added userdata example
# 
# Nexus_Devel/14   10/2/07 6:29p vsilyaev
# PR 34662: Added framebuffer example
# 
# Nexus_Devel/13   10/2/07 12:04p jgarrett
# PR 34416: Adding platform_app.inc
# 
# Nexus_Devel/12   9/28/07 11:20p erickson
# PR35395: work around syncthunk dependency issues for 0.1 release
# 
# Nexus_Devel/11   9/28/07 4:38p erickson
# PR34925: added nexus script for 0.1 release
# 
# Nexus_Devel/9   9/26/07 5:34p vsilyaev
# PR 34662: Set optimization to -Os
# 
# Nexus_Devel/8   9/26/07 3:13p erickson
# PR34926: added playpump
# 
# Nexus_Devel/7   9/25/07 3:14p vsilyaev
# PR 34662: Added Display example/test
# 
# Nexus_Devel/6   9/25/07 10:54a jgarrett
# PR 35002: Adding debug
# 
# Nexus_Devel/5   9/24/07 2:42p erickson
# PR34925: update
# 
# Nexus_Devel/4   9/21/07 6:26p jgarrett
# PR 35002: Adding boot example
# 
# Nexus_Devel/3   9/4/07 11:40a jgarrett
# PR 34416: Chaning to single OS include directory again
# 
# Nexus_Devel/2   8/31/07 6:28p jgarrett
# PR 34416: Moving nexus_top
# 
# Nexus_Devel/1   8/31/07 5:07p jgarrett
# PR 34416: Adding initial version
# 
# Nexus_Devel/2   8/31/07 4:15p jgarrett
# PR 34416: Adding baseline build system
# 
############################################################
# Nexus examples

ifdef COMSPEC
# Any DOS environment
NEXUS_TOP := $(shell cd .. && cd)
else
NEXUS_TOP := $(shell cd ../../../../../nexus; pwd)
endif

PORTINGINTERFACE = $(NEXUS_TOP)/../magnum/portinginterface
NOCS3_ROOT = $(NEXUS_TOP)/../rockford/applications/nocs3

ifndef PLATFORM
$(error PLATFORM is not defined)
endif

# include cross-compiler definitions
include $(NEXUS_TOP)/platforms/$(PLATFORM)/build/platform_app.inc

EXTRA_OBJS := csdlib/csd.o

CERT_HARNESS_OBJ := \
		 ca_cert.o \
		 proclib.o \
		 cert_scr.o \
		 cert_dsc.o \
		 cert_reset.o \
		 comcmd.o \
		 tlvlib.o \
		 toollib.o \
		 $(NOCS3_ROOT)/harness_test/common/nocs3_harness_test_utilities.o \
		 $(NOCS3_ROOT)/harness_test/common/nocs3_harness_test_parser.o \
		 csd_harness_test_parser.o \
		 csd_harness_test_utilities.o \
		 csd_harness_test_ca_pvr.o

THEAPPS = \
	cert_harness_test_main

#	nocs_client
ifeq ($(B_REFSW_OS),vxworks)
# VxWorks needs a wrapper function to call main.
VXOBJS = vxworks_cmd.o
# We like to use .out for loadable objects.
APPSUFFIX = .out
APPS = $(addsuffix .out, $(THEAPPS))
else
APPS = $(THEAPPS) 
endif


.PHONY: api clean

all:  api $(EXTRA_OBJS) $(CERT_HARNESS_OBJ) $(APPS) 

# For linux builds, link to the correct libraries
ifneq ($(findstring linux,$(B_REFSW_OS)),)
LDFLAGS := -lnexus -L$(NEXUS_TOP)/bin -lpthread
endif


# This builds the settop api
api:
	echo "$(MAKE) -C $(NEXUS_TOP)/build"
	$(MAKE) -C $(NEXUS_TOP)/build CUST=NAGRA

# This cleans local apps only, keep libnexus.so and bcmdriver.ko 
clean:
	-$(RM) $(APPS) *.d *.o *.out $(EXTRA_OBJS)
	-$(RM) $(CERT_HARNESS_OBJ)

# This cleans nexus and local apps
distclean: clean
	$(MAKE) -C $(NEXUS_TOP)/build clean

ifeq ($(B_REFSW_ARCH),mips-linux)
CFLAGS += -DCFG_BIG_ENDIAN=1
else
CFLAGS += -DCFG_BIG_ENDIAN=0
endif	    

# This is the minimum needed to compile and link with Nexus
CFLAGS += $(NEXUS_CFLAGS) $(addprefix -I,$(NEXUS_APP_INCLUDE_PATHS)) $(addprefix -D,$(NEXUS_APP_DEFINES)) -I$(NEXUS_TOP)/../BSEAV/lib/netaccel/include \
      -I$(NEXUS_TOP)/modules/security/include \
      -I$(NEXUS_TOP)/modules/security/include/priv  \
	  -I$(NEXUS_TOP)/modules/security/src \
	  -I$(NEXUS_TOP)/modules/core/include/ \
	  -I$(NEXUS_TOP)/modules/core/include/priv \
      -I$(NEXUS_TOP)/modules/security/$(BCHP_CHIP)/include \
      -I$(NEXUS_TOP)/modules/security/$(BCHP_CHIP)/include/priv  \
	  -I$(NEXUS_TOP)/modules/security/$(BCHP_CHIP)/src \
	  -I$(NEXUS_TOP)/modules/core/$(BCHP_CHIP)/include/ \
	  -I$(NEXUS_TOP)/modules/core/$(BCHP_CHIP)/include/priv \
	  -I$(NEXUS_TOP)/base/include \
	  -I$(NEXUS_TOP)/base/include/linuxuser \
	  -I$(PORTINGINTERFACE)/../basemodules/mem \
	  -I$(PORTINGINTERFACE)/../basemodules/int \
    -I$(PORTINGINTERFACE)/hsm/src/core40nm/common \
	  -I$(PORTINGINTERFACE)/hsm/include/core40nm/common/Zeus20/aegis \
	  -I$(PORTINGINTERFACE)/hsm/include/core40nm/common/Zeus25/aegis \
	  -I$(PORTINGINTERFACE)/hsm/include/core40nm/common/Zeus30/aegis \
	  -I$(PORTINGINTERFACE)/hsm/include/core40nm/common/Zeus20/cust_nagra/aegis \
	  -I$(PORTINGINTERFACE)/hsm/include/core40nm/common/Zeus25/cust_nagra/aegis \
	  -I$(PORTINGINTERFACE)/hsm/include/core40nm/common/Zeus30/cust_nagra/aegis \
	  -I$(PORTINGINTERFACE)/hsm/src/core40nm/common \
	  -I$(PORTINGINTERFACE)/hsm/src/core40nm/keyladder \
	  -I$(PORTINGINTERFACE)/hsm/src/core40nm/otpmsp \
	  -I$(PORTINGINTERFACE)/dma/ \
	  -I$(PORTINGINTERFACE)/hsm/$(BCHP_CHIP)/$(BCHP_VER_LOWER)/common \
	  -I$(PORTINGINTERFACE)/hsm/$(BCHP_CHIP)/$(BCHP_VER_LOWER)/common/aegis \
	  -I$(PORTINGINTERFACE)/hsm/$(BCHP_CHIP)/$(BCHP_VER_LOWER)/cust_nagra/aegis \
	  -I$(PORTINGINTERFACE)/hsm/$(BCHP_CHIP)/$(BCHP_VER_LOWER)/keyladder \
	  -I$(PORTINGINTERFACE)/hsm/$(BCHP_CHIP)/$(BCHP_VER_LOWER)/otpmsp \
	  -I$(PORTINGINTERFACE)/dma/$(BCHP_CHIP) \
	  -I$(NOCS3_ROOT)/harness_test/common/		\
	  -Icsdlib/

# Always build with debug
CFLAGS += -g

# Build examples_lib.c

# Implicit rule for building local apps
%$(APPSUFFIX): %.c $(EXTRA_OBJS) $(CERT_HARNESS_OBJ) $(VXOBJS)
	echo [Compile... $<]
	$(CC) -o $@ $(filter %.c %.s %.o, $^)  $(CFLAGS) $(LDFLAGS)
	mv $@ cert 
	cp cert $(NEXUS_TOP)/bin 

install: all

