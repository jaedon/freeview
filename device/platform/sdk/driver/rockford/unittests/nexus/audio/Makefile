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
# $brcm_Revision: Hydra_Software_Devel/30 $
# $brcm_Date: 10/4/12 10:04a $
#
# Module Description:
#
# Revision History:
#
# Created: 08/28/2007 by Jon Garrett
#
# $brcm_Log: /rockford/unittests/nexus/audio/Makefile $
# 
# Hydra_Software_Devel/30   10/4/12 10:04a erickson
# SW7346-1058: add primer unittest
# 
# Hydra_Software_Devel/29   6/27/12 3:08p raghaven
# SW7425-3279: modified to include audio test app audio_decode_test
# 
# Hydra_Software_Devel/28   5/30/12 5:04p gskerl
# SW7429-29: Added audio_arc_output.c test program.
# 
# Hydra_Software_Devel/27   2/13/12 8:55a jgarrett
# SW7425-2268: Adding echo canceller test application
# 
# Hydra_Software_Devel/26   1/25/12 4:51p gskerl
# SW7429-18: Merged back to Hydra_Software_Devel.
# 
# Hydra_Software_Devel/SW7429-18_20120120/1   1/25/12 4:04p gskerl
# SW7429-18: Alphabetized list of APPS, added audio_arc_input app to
# list..
# 
# Hydra_Software_Devel/25   11/2/11 6:38p gskerl
# SW7425-1178: Added audio_hdmi_input_edid_ram to Makefile
# 
# Hydra_Software_Devel/25   11/2/11 6:36p gskerl
# SW7425-1178: Added audio_hdmi_input_edid_ram to Makefile
# 
# Hydra_Software_Devel/24   10/20/11 5:45p gprasad
# FWRAA-359: Add support for ring buffer capture for cert testing
# 
# Hydra_Software_Devel/23   9/27/11 11:21a vsilyaev
# SW7425-1352: Added unit test  to verify TSM for WMA streams
# 
# Hydra_Software_Devel/22   8/22/11 3:52p gskerl
# SW7425-1178: Cloning /nexus/examples/audio/audio_mixer.c to
# /rockford/unittests/nexus/audio/audio_mixer.c. Then added support for
# SPDIF, HDMI and I2S outputs. Added downscaling and modulation of the 1
# KHz tone. Changed to use a dspMixer instead of a standard mixer.
# 
# Hydra_Software_Devel/21   8/9/11 2:49p mahenm
# SW7420-1316 : Adding MS11 support flag
# 
# Hydra_Software_Devel/20   7/26/11 4:31p gprasad
# SW7420-1316: Adding support for BRCM 3d surround
# 
# Hydra_Software_Devel/19   7/26/11 1:47p mahenm
# SW7420-1316: Adding support for independent TVOL and TSHD
# 
# Hydra_Software_Devel/18   6/2/11 4:41p mahenm
# SW7420-1316 : [7420] DV258 support added
# 
# Hydra_Software_Devel/17   5/12/11 2:09p gprasad
# SW7422-429: [7422] Fixing the build error
# 
# Hydra_Software_Devel/16   4/19/11 1:34p gprasad
# SW7422-429: [7422] Nexus example application app_audio does not support
# ring buffer capture for 7422
# 
# Hydra_Software_Devel/15   3/22/11 3:16p gskerl
# SW7422-146: Added i2s_input_decoder unit test program
# 
# Hydra_Software_Devel/14   3/17/11 7:03p gskerl
# SW7422-146: Added i2s_input and audio_playback
# 
# Hydra_Software_Devel/13   12/7/10 9:12p kalidoss
# SW35230-2335: checking in changes for supporting SRS Studio Sound.
# 
# Hydra_Software_Devel/12   10/19/10 2:14p gprasad
# SW35230-1229: Adding supprot for enabling Audyssey ADV / ABX post
# processing
# 
# Hydra_Software_Devel/11   10/1/10 7:56a gprasad
# SW35230-1229: Enabling custom voice based on export
# 'NEXUS_AUDIO_EXTENSION_INC'
# 
# Hydra_Software_Devel/10   9/30/10 4:52p mward
# SW35230-1229: app_audio will only build for NEXUS_PLATFORM=935230.
# 
# Hydra_Software_Devel/9   9/21/10 10:30p gprasad
# SW35230-1229: Addiong support to enable rbuf capture using the compile
# time define RAP_RBUF_CAP_SUPPORT
# 
# Hydra_Software_Devel/8   9/10/10 5:30p gprasad
# SW35230-1229: Create nexus application for running certification tests
# 35230 : Updates
# 
# Hydra_Software_Devel/7   8/31/10 5:51p gprasad
# SW35230-1229: Create nexus application for running certification tests
# 35230
# 
# Hydra_Software_Devel/6   11/11/08 4:09p jtna
# PR42679: added unittest audio_test_auto
# 
# Hydra_Software_Devel/5   10/23/08 3:59p erickson
# PR42679: update
#
# Hydra_Software_Devel/4   5/14/08 2:59p erickson
# PR42685: added codec_test
#
# Hydra_Software_Devel/3   5/14/08 10:07a erickson
# PR42679: eliminate duplicate code in unittest Makefiles
#
############################################################

ifdef COMSPEC
# Any DOS environment
NEXUS_TOP := $(shell cd ../../../../nexus && cd)
else
NEXUS_TOP := $(shell cd ../../../../nexus; pwd)
endif

APPS = \
    app_system_watchdog \
    audio_arc_input \
    audio_arc_output \
    audio_decode_test \
    audio_drip_decode \
    audio_hdmi_input_edid_ram \
    audio_mixer \
    audio_playback \
    audio_test_auto \
    codec_test \
    i2s_input \
    i2s_input_decoder \
    audio_wma_tsm  \
    echo_canceller \
	playback_pip

APPS_DEP = $(APPS:%=%.d)

# audio_test requires user input

# For app_audio to play asf/avi files
BSEAV = $(NEXUS_TOP)/../BSEAV
include $(BSEAV)/lib/bfile/bfile.inc
ifeq ($(MEDIA_AVI_SUPPORT),y)
include $(BSEAV)/lib/avi/bavi.inc
endif
ifeq ($(MEDIA_ASF_SUPPORT),y)
include $(BSEAV)/lib/asf/basf.inc
endif

include $(BSEAV)/lib/utils/batom.inc
include $(BSEAV)/lib/media/bmedia.inc
CFLAGS += $(addprefix -I,$(BMEDIA_INCLUDES) $(BFILE_MEDIA_INCLUDES)) -I$(BSEAV)/lib/mpeg2_ts_parse -I$(BSEAV)/lib/tspsi
CFLAGS += $(addprefix -D,$(BMEDIA_PROBE_DEFINES))

# For Enabling ring buffer capture # 
MAGNUM = $(NEXUS_TOP)/../magnum
CFLAGS += -I$(MAGNUM)/basemodules/chp/$(BCHP_CHIP)/rdb/$(BCHP_VER_LOWER) -I$(MAGNUM)/basemodules/mem
ifeq ($(RAP_RBUF_CAP_SUPPORT),y)
CFLAGS += -DBDSP_FW_RBUF_CAPTURE
endif


# For Enabling Auto Vol Leveling (BRCM AVL) # 
ifeq ($(RAP_AVL_SUPPORT),y)
CFLAGS += -DNEXUS_AVL_ENABLE
endif

# For Enabling Auto Vol Leveling (Phillips AVL) # 
ifeq ($(RAP_AVLP_SUPPORT),y)
CFLAGS += -DNEXUS_AVLP_ENABLE
endif

# For Enabling 3D Surround (BRCM 3D Surround) # 
ifeq ($(RAP_BRCM_3DSURROUND_SUPPORT),y)
CFLAGS += -DNEXUS_3DSURROUND_ENABLE
endif

# For Enabling Audyssey Volume # 
ifeq ($(RAP_ADV_SUPPORT),y)
CFLAGS += -DNEXUS_ADV_ENABLE
endif

# For Enabling Audyssey ABX # 
ifeq ($(RAP_ABX_SUPPORT),y)
CFLAGS += -DNEXUS_ABX_ENABLE
endif

# For SRS code studio # 
ifeq ($(RAP_SRSSTUDIOSOUND_SUPPORT),y)
CFLAGS += -DNEXUS_SRSSTUDIOSOUND_ENABLE
endif
# For SRS TVOL # 
ifeq ($(RAP_SRSTRUVOL_SUPPORT),y)
CFLAGS += -DNEXUS_SRSTRUVOL_ENABLE
endif
# For SRS TSHD # 
ifeq ($(RAP_SRSHD_SUPPORT),y)
CFLAGS += -DNEXUS_SRSHD_ENABLE
endif

# For DV258 support
ifeq ($(RAP_DV258_SUPPORT),y)
CFLAGS += -DNEXUS_DV258_ENABLE
CFLAGS += -DRAP_DV258_SUPPORT
CFLAGS += -DUSE_DV258
endif

# For MS11 support # 
ifeq ($(RAP_MS11_SUPPORT),y)
CFLAGS += -DNEXUS_MS11_SUPPORT
endif

ifneq ($(NEXUS_AUDIO_EXTENSION_INC),)
CFLAGS += -DCUSTOM_VOICE_ENABLE
endif

include ../common/Makefile.inc

#-include $(APPS_DEP)
app_audio : rbuf_capture.h
