############################################################
#     Copyright (c) 2003-2012, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: bxpt.inc $
# $brcm_Revision: Hydra_Software_Devel/88 $
# $brcm_Date: 10/2/12 8:44a $
#
# Module Description:
#
# Revision History:
#
# Created: 01/09/2007 by Jon Garrett
#
# $brcm_Log: /magnum/portinginterface/xpt/bxpt.inc $
# 
# Hydra_Software_Devel/88   10/2/12 8:44a gmullen
# SW7445-17: Removed extra rule for bxpt_tsmux in core40nm block
# 
# Hydra_Software_Devel/87   10/1/12 11:19a gmullen
# SW7445-17: Merged to Hydra
# 
# Hydra_Software_Devel/SW7445-17/1   10/1/12 10:00a gmullen
# SW7445-17: Created core28nm file tree
# 
# Hydra_Software_Devel/86   9/10/12 6:01p gmullen
# SW7425-2914: Updated psg and tsmf for 65 nm parts, into new dir
# structure
# 
# Hydra_Software_Devel/85   7/6/12 4:29p gmullen
# SW7563-11: Added support
# 
# Hydra_Software_Devel/84   6/19/12 12:47p gmullen
# SW7584-19: TSMUX support missing
# 
# Hydra_Software_Devel/83   6/18/12 4:01p gmullen
# SW7425-2914: Removed incorrect fw checks for 7468
# 
# Hydra_Software_Devel/82   6/18/12 9:31a gmullen
# SW7425-2914: Merged to Hydra
# 
# Hydra_Software_Devel/SW7425-2914/3   6/11/12 3:08p gmullen
# SW7425-2914: bxpt_tsmux missing from 7231
# 
# Hydra_Software_Devel/SW7425-2914/2   6/11/12 2:12p gmullen
# SW7425-2914: bxpt_tsmux missing from 7346 builds
# 
# Hydra_Software_Devel/SW7425-2914/1   6/11/12 10:59a gmullen
# SW7425-2914: Moved and updated files to new home
# 
# Hydra_Software_Devel/81   5/2/12 10:23a gmullen
# SW7584-19: Merged to Hydra
# 
# Hydra_Software_Devel/SW7584-19/1   5/2/12 9:01a gmullen
# SW7584-19: Added support for 7584
# 
# Hydra_Software_Devel/80   2/20/12 11:32a gmullen
# SW7231-565: Fixed typo in inclusion of bxpt_tsmux
# 
# Hydra_Software_Devel/79   2/17/12 1:55p katrep
# SW7231-565:7231 b2 build
# 
# Hydra_Software_Devel/78   1/11/12 9:06a gmullen
# SW7360-1: Added 7360 to the chip list
# 
# Hydra_Software_Devel/77   1/4/12 11:34a gmullen
# SW7435-10: Merged addition of 7435 to build
# 
# Hydra_Software_Devel/SW7435-10/1   10/31/11 3:50p mward
# SW7435-10:  Initial 7435 support adapted from 7425 to allow
# appframework to compile.
# 
# Hydra_Software_Devel/76   1/2/12 3:12p gmullen
# SW7360-1: Added support
# 
# Hydra_Software_Devel/75   12/9/11 11:19a jtna
# SW7425-1863: compile bxpt_pwr_mgmt.c for 7231 and others
# 
# Hydra_Software_Devel/74   12/2/11 12:12p jtna
# SW7425-1863: refactor xpt standby power management
# 
# Hydra_Software_Devel/73   10/12/11 1:04p gmullen
# SW7429-3: 7429 rave uses uif_image directory
# 
# Hydra_Software_Devel/72   10/10/11 10:49a gmullen
# SW7429-3: Merged to Hydra
# 
# Hydra_Software_Devel/SW7429-3/1   10/10/11 10:43a gmullen
# SW7429-3: Added support
# 
# Hydra_Software_Devel/71   9/30/11 5:17p gmullen
# SW7425-1183: Added support for 7422, 7358, 7552, 7344, 7640, 7231, and
# 7346
# 
# Hydra_Software_Devel/70   9/30/11 4:04p gmullen
# SW7425-1183: Added bxpt_wakeup to builds for 7425
# 
# Hydra_Software_Devel/69   8/29/11 3:34p gmullen
# SW7346-419: Merged changes to Hydra
# 
# Hydra_Software_Devel/SW7346-419/1   8/29/11 3:20p gmullen
# SW7346-419: Added B0 support
# 
# Hydra_Software_Devel/68   8/22/11 5:45p gmullen
# SW7231-319: Merged to Hydra branch
# 
# Hydra_Software_Devel/SW7231-319/1   8/22/11 4:12p gmullen
# SW7231-319: Added B0 support
# 
# Hydra_Software_Devel/67   7/28/11 11:27a gmullen
# SW7405-5446: Added support
# 
# Hydra_Software_Devel/66   6/15/11 2:55p gmullen
# SW7425-719: Merged to Hydra branch
# 
# Hydra_Software_Devel/SW7425-719/1   6/15/11 2:35p gmullen
# SW7425-719: Added TSMF support
# 
# Hydra_Software_Devel/65   3/25/11 11:35a gmullen
# SWDTV-5976: Added 35233 support
# 
# Hydra_Software_Devel/64   2/2/11 4:49p gmullen
# SW7550-16: Added PSG support
# 
# Hydra_Software_Devel/63   1/7/11 3:19p gmullen
# SWBLURAY-23703: Added PI support
# 
# Hydra_Software_Devel/62   12/16/10 9:43a gmullen
# SW7552-3: Added 7552 to the Unified ITB list
# 
# Hydra_Software_Devel/61   12/1/10 11:20a gmullen
# SW7125-665: Added TSMF support
# 
# Hydra_Software_Devel/60   12/1/10 8:01a gmullen
# SW7425-15: Updated header and stub calls for TS mux
# 
# Hydra_Software_Devel/59   11/18/10 5:21p gmullen
# SW7344-2: Initial checkin
# 
# Hydra_Software_Devel/58   11/18/10 5:13p gmullen
# SW7346-3: Initial checkin
# 
# Hydra_Software_Devel/57   11/18/10 4:58p gmullen
# SW7231-6: Initial checkin
# 
# Hydra_Software_Devel/56   11/10/10 11:54a gmullen
# SW35125-19: Added support
# 
# Hydra_Software_Devel/55   11/1/10 10:47a gmullen
# SW7552-3: Added support for the 7552
# 
# Hydra_Software_Devel/54   10/29/10 9:25a gmullen
# SW7422-20: Src for legacy hw aren't needed
# 
# Hydra_Software_Devel/53   10/28/10 3:30p gmullen
# SW7358-12: Updated PI for the 7358
# 
# Hydra_Software_Devel/52   10/27/10 1:36p gmullen
# SW7422-20: Ported to 7422
# 
# Hydra_Software_Devel/51   10/25/10 5:39p gmullen
# SW7425-15: Make util does not like whitespaces.
# 
# Hydra_Software_Devel/50   10/25/10 5:32p gmullen
# SW7422-20: Changed _P_MAX defines to _NUM
# 
# Hydra_Software_Devel/49   10/25/10 9:13a gmullen
# SW7425-15: Added 7425 support
# 
# Hydra_Software_Devel/48   7/9/10 11:07a gmullen
# SW7550-448: Fixed compilation bug for non-TSMF builds
# 
# Hydra_Software_Devel/47   7/4/10 5:33p gmullen
# SW7630-81: Added support for QUICK
# 
# Hydra_Software_Devel/46   6/19/10 12:52p gmullen
# SW7550-448: Added TSMF support
# 
# Hydra_Software_Devel/45   4/30/10 5:23p gmullen
# SW7630-81: Created branch
# 
# Hydra_Software_Devel/44   2/8/10 3:03p gmullen
# SW7408-64: Added missing API.
# 
# Hydra_Software_Devel/43   1/25/10 11:54a gmullen
# SW7408-64: Added DirecTV support
# 
# Hydra_Software_Devel/42   12/9/09 2:46p gmullen
# SW7405-3331: Merged to mainline
# 
# Hydra_Software_Devel/41   11/23/09 5:13p gmullen
# SW7468-13: Updated PI and unit test
# 
# Hydra_Software_Devel/40   11/4/09 3:59p gmullen
# SW7400-2590: Merged to Hydra_Software_Devel
# 
# Hydra_Software_Devel/SW7400-2590/2   11/2/09 3:35p gmullen
# SW7400-2590: Changed location of ihex files for this issue
# 
# Hydra_Software_Devel/SW7400-2590/1   10/30/09 6:12p gmullen
# SW7400-2590: Added autoread ihex and PI extensions.
# 
# Hydra_Software_Devel/39   10/29/09 5:32p gmullen
# SW7408-9: Initial checkin
# 
# Hydra_Software_Devel/38   10/26/09 3:09p gmullen
# SW7468-13: Added XPT support
# 
# Hydra_Software_Devel/37   9/18/09 3:34p gmullen
# SW70015-1: Added xpt and RAVE unified format support
# 
# Hydra_Software_Devel/36   9/14/09 9:57a gmullen
# SW35230-2: Added support for 35230
# 
# Hydra_Software_Devel/35   9/9/09 8:11a piyushg
# SW7630-30: Add 7630 XPT PI support.
# 
# Hydra_Software_Devel/34   9/8/09 4:11p gmullen
# SW7630-43: Added support for 7630
# 
# Hydra_Software_Devel/33   8/11/09 10:40a piyushg
# PR55216: Added initial 7340 XPT support.
# 
# Hydra_Software_Devel/32   8/5/09 4:52p piyushg
# PR55545: Add 7125 XPT PI support
# Added file element "bxpt_rave_ihex.c".
# Added file element "bxpt_rave_ihex.h".
# 
# Hydra_Software_Devel/31   7/31/09 2:44p gmullen
# PR54331: Added 35130 to XPT support.
# 
# Hydra_Software_Devel/30   7/14/09 10:23a piyushg
# PR56771: Add XPT PI code for 7342.
# 
# Hydra_Software_Devel/29   6/17/09 6:46p gmullen
# PR56110: Added support.xpt/7550/uif_image/a0
# 
# Hydra_Software_Devel/28   6/11/09 9:14a gmullen
# PR55216: Initial checkin.
# 
# Hydra_Software_Devel/27   4/17/09 4:21p gmullen
# PR54331: Added 35130 to the build.
# 
# Hydra_Software_Devel/26   4/7/09 5:26p piyushg
# PR52986: Add support for 7635
# 
# Hydra_Software_Devel/25   1/27/09 1:09p gmullen
# PR51625: Added 7336 support
# 
# Hydra_Software_Devel/24   12/15/08 2:36p gmullen
# PR48908: Removed power management code from XPT PI.
# 
# Hydra_Software_Devel/23   11/26/08 4:14p gmullen
# PR47755: Added support for 7420.
# 
# Hydra_Software_Devel/22   10/30/08 1:15p gmullen
# PR46544: Fixed spelling error in define.
# 
# Hydra_Software_Devel/21   10/29/08 9:20a gmullen
# PR46544: Fixed compile error: code was built for unsupported chips.
# 
# Hydra_Software_Devel/20   10/28/08 6:18p gmullen
# PR46544: Default power management to ON. Added compile-time support to
# disable power management.
# 
# Hydra_Software_Devel/19   8/8/08 2:38p gmullen
# PR44537: PI for XPT on the 7601
# 
# Hydra_Software_Devel/18   3/26/08 11:24a gmullen
# PR38954: Added 3548 support to XPT PI.
# 
# Hydra_Software_Devel/17   12/14/07 9:52a gmullen
# PR37118: Forced unified ITB enabled on 7118 and 7401.
# 
# Hydra_Software_Devel/16   11/28/07 10:11a gmullen
# PR36900: Added 7335 support.
# 
# Hydra_Software_Devel/15   11/9/07 3:07p jgarrett
# PR 36774: Fixing use of CFLAGS
# 
# Hydra_Software_Devel/14   10/25/07 4:47p gmullen
# PR36446: Forced unified ITB support enabled on 7400
# 
# Hydra_Software_Devel/13   10/19/07 3:31p gmullen
# PR35018: Added 7325 support
# 
# Hydra_Software_Devel/12   7/25/07 4:10p gmullen
# PR32434: Changed 7118 unified ITB support to a command line option at
# build time.
# 
# Hydra_Software_Devel/11   7/24/07 4:14p gmullen
# PR32434: Added ihex files and adjusted bxpt.inc for unified ITB
# support.
# 
# Hydra_Software_Devel/10   7/12/07 11:24a gmullen
# PR32885: Fixed (potential) bad args in findstring call.
# 
# Hydra_Software_Devel/9   7/10/07 3:15p jgarrett
# PR 32885: Fixing UNIFIED_ITB_SUPPORT=y test
# 
# Hydra_Software_Devel/8   6/14/07 9:12a gmullen
# PR29614: Changed check for unified ITB support enabled in the build
# environment.
# 
# Hydra_Software_Devel/7   6/13/07 3:18p gmullen
# PR29614: Forced unified ITB support on for 7405. Added check for
# UNIFIED_ITB_SUPPORT for other chips.
# 
# Hydra_Software_Devel/6   5/31/07 3:40p gmohile
# PR 29614 : set UITB as default for 7403
# 
# Hydra_Software_Devel/5   4/13/07 7:01p gmullen
# PR29614: Added unified ITB support.
# 
# Hydra_Software_Devel/4   2/14/07 1:11p gmullen
# PR27642: Added support for 7405.
# 
# Hydra_Software_Devel/3   2/6/07 7:06p gmullen
# PR26217: Added B0 support.
# 
# Hydra_Software_Devel/2   1/16/07 4:58p jgarrett
# PR 27004: Adding OTP & swizzle support for 7038
# 
# Hydra_Software_Devel/1   1/11/07 2:39p jgarrett
# PR27004: Adding new .inc files
# 
############################################################

#
# Conditional Options for this Makefile
#

# Define this module
MAGNUM_MODULES += BXPT
BXPT_DEFINES += UNIFIED_ITB_SUPPORT

ifeq ($(findstring $(BCHP_CHIP), 7445), $(BCHP_CHIP))
ifeq ($(TRANSCODING_32BIT_TIMESTAMP),y) # HUMAX_PLATFORM_BASE
 RAVE_IMAGE_DIR = cat4a_v1243
else
	RAVE_IMAGE_DIR = cat4a
endif	
	BXPT_INCLUDES += \
                $(MAGNUM)/portinginterface/xpt/src/common/ \
                $(MAGNUM)/portinginterface/xpt/src/core28nm/

	BXPT_SOURCES += \
		$(MAGNUM)/portinginterface/xpt/src/core28nm/bxpt.c \
		$(MAGNUM)/portinginterface/xpt/src/core28nm/bxpt_directv_playback.c \
		$(MAGNUM)/portinginterface/xpt/src/core28nm/bxpt_mpod.c \
		$(MAGNUM)/portinginterface/xpt/src/core28nm/bxpt_packetsub.c \
		$(MAGNUM)/portinginterface/xpt/src/core28nm/bxpt_pcr_offset_emv.c \
		$(MAGNUM)/portinginterface/xpt/src/core28nm/bxpt_playback.c \
		$(MAGNUM)/portinginterface/xpt/src/core28nm/bxpt_tsmux.c \
		$(MAGNUM)/portinginterface/xpt/src/common/bxpt_directv.c \
		$(MAGNUM)/portinginterface/xpt/src/common/bxpt_directv_pcr.c \
		$(MAGNUM)/portinginterface/xpt/src/common/bxpt_directv_pcroffset.c \
		$(MAGNUM)/portinginterface/xpt/src/common/bxpt_sam.c \
		$(MAGNUM)/portinginterface/xpt/src/common/bxpt_spid.c \
		$(MAGNUM)/portinginterface/xpt/src/common/bxpt_pcr.c \
		$(MAGNUM)/portinginterface/xpt/src/common/bxpt_interrupt.c \
		$(MAGNUM)/portinginterface/xpt/src/common/bxpt_directv_remux.c \
		$(MAGNUM)/portinginterface/xpt/src/common/bxpt_remux.c \
		$(MAGNUM)/portinginterface/xpt/src/common/bxpt_remux_priv.c \
		$(MAGNUM)/portinginterface/xpt/src/common/bxpt_mesg.c \
		$(MAGNUM)/portinginterface/xpt/src/common/bxpt_rsbuf_priv.c \
		$(MAGNUM)/portinginterface/xpt/src/common/bxpt_xcbuf_priv.c \
		$(MAGNUM)/portinginterface/xpt/src/common/bxpt_rave.c \
		$(MAGNUM)/portinginterface/xpt/src/rave/$(RAVE_IMAGE_DIR)/bxpt_rave_ihex.c \
		$(MAGNUM)/portinginterface/xpt/src/common/bxpt_wakeup.c \

		ifeq ($(BCHP_PWR_SUPPORT),y)
		BXPT_SOURCES += \
			$(MAGNUM)/portinginterface/xpt/src/core28nm/bxpt_pwr_mgmt.c
		endif

endif

ifeq ($(findstring $(BCHP_CHIP), 7425 7422 7358 7552 7344 7640 7360 7435 7231 7346 7429 7584 7563), $(BCHP_CHIP))
	ifeq ($(findstring $(BCHP_CHIP), 7422 7358 7552 7344 7360 7563), $(BCHP_CHIP))
		RAVE_IMAGE_DIR = cat3a
	endif
	ifeq ($(findstring $(BCHP_CHIP), 7640), $(BCHP_CHIP))
		RAVE_IMAGE_DIR = cat3c
	endif
	ifeq ($(findstring $(BCHP_CHIP), 7425 7435 7231 7346 7429 7584), $(BCHP_CHIP))
ifeq ($(TRANSCODING_32BIT_TIMESTAMP),y) # HUMAX_PLATFORM_BASE
    RAVE_IMAGE_DIR = cat4a_v1243
else	
		RAVE_IMAGE_DIR = cat4a
endif
	endif

	BXPT_INCLUDES += \
                $(MAGNUM)/portinginterface/xpt/src/common/ \
                $(MAGNUM)/portinginterface/xpt/src/core40nm/

	BXPT_SOURCES += \
		$(MAGNUM)/portinginterface/xpt/src/core40nm/bxpt.c \
		$(MAGNUM)/portinginterface/xpt/src/core40nm/bxpt_directv_playback.c \
		$(MAGNUM)/portinginterface/xpt/src/core40nm/bxpt_mpod.c \
		$(MAGNUM)/portinginterface/xpt/src/core40nm/bxpt_packetsub.c \
		$(MAGNUM)/portinginterface/xpt/src/core40nm/bxpt_pcr_offset_emv.c \
		$(MAGNUM)/portinginterface/xpt/src/core40nm/bxpt_playback.c \
		$(MAGNUM)/portinginterface/xpt/src/common/bxpt_directv.c \
		$(MAGNUM)/portinginterface/xpt/src/common/bxpt_directv_pcr.c \
		$(MAGNUM)/portinginterface/xpt/src/common/bxpt_directv_pcroffset.c \
		$(MAGNUM)/portinginterface/xpt/src/common/bxpt_sam.c \
		$(MAGNUM)/portinginterface/xpt/src/common/bxpt_spid.c \
		$(MAGNUM)/portinginterface/xpt/src/common/bxpt_pcr.c \
		$(MAGNUM)/portinginterface/xpt/src/common/bxpt_interrupt.c \
		$(MAGNUM)/portinginterface/xpt/src/common/bxpt_directv_remux.c \
		$(MAGNUM)/portinginterface/xpt/src/common/bxpt_remux.c \
		$(MAGNUM)/portinginterface/xpt/src/common/bxpt_remux_priv.c \
		$(MAGNUM)/portinginterface/xpt/src/common/bxpt_mesg.c \
		$(MAGNUM)/portinginterface/xpt/src/common/bxpt_rsbuf_priv.c \
		$(MAGNUM)/portinginterface/xpt/src/common/bxpt_xcbuf_priv.c \
		$(MAGNUM)/portinginterface/xpt/src/common/bxpt_rave.c \
		$(MAGNUM)/portinginterface/xpt/src/rave/$(RAVE_IMAGE_DIR)/bxpt_rave_ihex.c \
		$(MAGNUM)/portinginterface/xpt/src/common/bxpt_wakeup.c \

		ifeq ($(BCHP_PWR_SUPPORT),y)
		BXPT_SOURCES += \
			$(MAGNUM)/portinginterface/xpt/src/core40nm/bxpt_pwr_mgmt.c
		endif

		ifeq ($(findstring $(BCHP_CHIP), 7231 7346 7425 7429 7435 7584), $(BCHP_CHIP))
		BXPT_SOURCES += \
			$(MAGNUM)/portinginterface/xpt/src/core40nm/bxpt_tsmux.c
		endif
		
		TSMFDIR = $(MAGNUM)/portinginterface/xpt/src/common/
		TSMFSRC = $(TSMFDIR)/bxpt_tsmf.c
		ifneq (,$(findstring $(TSMFSRC),$(wildcard $(TSMFSRC) )))
			BXPT_SOURCES += $(TSMFDIR)/bxpt_tsmf.c
		endif
endif

ifeq ($(findstring $(BCHP_CHIP), 7468), $(BCHP_CHIP))
	ifeq ($(findstring $(BCHP_CHIP), 7468), $(BCHP_CHIP))
		RAVE_IMAGE_DIR = cat2a
	endif

	BXPT_INCLUDES += $(MAGNUM)/portinginterface/xpt/src/core65nm/

	BXPT_SOURCES += \
		$(MAGNUM)/portinginterface/xpt/src/core65nm/bxpt.c \
		$(MAGNUM)/portinginterface/xpt/src/core65nm/bxpt_directv.c \
		$(MAGNUM)/portinginterface/xpt/src/core65nm/bxpt_directv_playback.c \
		$(MAGNUM)/portinginterface/xpt/src/core65nm/bxpt_directv_pcr.c \
		$(MAGNUM)/portinginterface/xpt/src/core65nm/bxpt_sam.c \
		$(MAGNUM)/portinginterface/xpt/src/core65nm/bxpt_pcr.c \
		$(MAGNUM)/portinginterface/xpt/src/core65nm/bxpt_spid.c \
		$(MAGNUM)/portinginterface/xpt/src/core65nm/bxpt_packetsub.c \
		$(MAGNUM)/portinginterface/xpt/src/core65nm/bxpt_playback.c \
		$(MAGNUM)/portinginterface/xpt/src/core65nm/bxpt_rave.c \
		$(MAGNUM)/portinginterface/xpt/src/rave/$(RAVE_IMAGE_DIR)/bxpt_rave_ihex.c \
		$(MAGNUM)/portinginterface/xpt/src/core65nm/bxpt_interrupt.c \
		$(MAGNUM)/portinginterface/xpt/src/core65nm/bxpt_mesg.c \
		$(MAGNUM)/portinginterface/xpt/src/core65nm/bxpt_directv_pcroffset.c \
		$(MAGNUM)/portinginterface/xpt/src/core65nm/bxpt_pcr_offset_emv.c 
endif

ifeq ($(findstring $(BCHP_CHIP), 7405 7336 7325 7335 7420 7340 7342 7125), $(BCHP_CHIP))
	ifeq ($(findstring $(BCHP_CHIP), 7125 7420 7340 7342), $(BCHP_CHIP))
		RAVE_IMAGE_DIR = cat2a
	endif
	ifeq ($(findstring $(BCHP_CHIP), 7405 7336), $(BCHP_CHIP))
		RAVE_IMAGE_DIR = cat1b
	endif
	ifeq ($(findstring $(BCHP_CHIP), 7325 7335), $(BCHP_CHIP))
		RAVE_IMAGE_DIR = cat1a
	endif

	BXPT_INCLUDES += $(MAGNUM)/portinginterface/xpt/src/core65nm/

	BXPT_SOURCES += \
		$(MAGNUM)/portinginterface/xpt/src/core65nm/bxpt.c \
		$(MAGNUM)/portinginterface/xpt/src/core65nm/bxpt_directv.c \
		$(MAGNUM)/portinginterface/xpt/src/core65nm/bxpt_directv_playback.c \
		$(MAGNUM)/portinginterface/xpt/src/core65nm/bxpt_directv_pcr.c \
		$(MAGNUM)/portinginterface/xpt/src/core65nm/bxpt_directv_pcroffset.c \
		$(MAGNUM)/portinginterface/xpt/src/core65nm/bxpt_sam.c \
		$(MAGNUM)/portinginterface/xpt/src/core65nm/bxpt_spid.c \
		$(MAGNUM)/portinginterface/xpt/src/core65nm/bxpt_packetsub.c \
		$(MAGNUM)/portinginterface/xpt/src/core65nm/bxpt_playback.c \
		$(MAGNUM)/portinginterface/xpt/src/core65nm/bxpt_pcr.c \
		$(MAGNUM)/portinginterface/xpt/src/core65nm/bxpt_interrupt.c \
		$(MAGNUM)/portinginterface/xpt/src/core65nm/bxpt_directv_remux.c \
		$(MAGNUM)/portinginterface/xpt/src/core65nm/bxpt_remux.c \
		$(MAGNUM)/portinginterface/xpt/src/core65nm/bxpt_remux_private.c \
		$(MAGNUM)/portinginterface/xpt/src/core65nm/bxpt_mpod.c \
		$(MAGNUM)/portinginterface/xpt/src/core65nm/bxpt_mesg.c \
		$(MAGNUM)/portinginterface/xpt/src/core65nm/bxpt_rsbuf.c \
		$(MAGNUM)/portinginterface/xpt/src/core65nm/bxpt_xcbuf.c \
		$(MAGNUM)/portinginterface/xpt/src/core65nm/bxpt_rave.c \
		$(MAGNUM)/portinginterface/xpt/src/rave/$(RAVE_IMAGE_DIR)/bxpt_rave_ihex.c \
		$(MAGNUM)/portinginterface/xpt/src/core65nm/bxpt_pcr_offset_emv.c 

		TSMFDIR = $(MAGNUM)/portinginterface/xpt/src/core65nm/
		TSMFSRC = $(TSMFDIR)/bxpt_tsmf.c
		ifneq (,$(findstring $(TSMFSRC),$(wildcard $(TSMFSRC) )))
			BXPT_SOURCES += $(TSMFDIR)/bxpt_tsmf.c
		endif

		PSGDIR = $(MAGNUM)/portinginterface/xpt/src/core65nm/
		PSGSRC = $(PSGDIR)/bxpt_psg.c
		ifneq (,$(findstring $(PSGSRC),$(wildcard $(PSGSRC) )))
			BXPT_SOURCES += $(PSGDIR)/bxpt_psg.c
		endif
endif

ifeq ($(findstring $(BCHP_CHIP), 7400), $(BCHP_CHIP))
	RAVE_IMAGE_DIR = cat7400e

	BXPT_INCLUDES += $(MAGNUM)/portinginterface/xpt/src/core65nm/

	BXPT_SOURCES += \
		$(MAGNUM)/portinginterface/xpt/src/core65nm/bxpt.c \
		$(MAGNUM)/portinginterface/xpt/src/core65nm/bxpt_directv.c \
		$(MAGNUM)/portinginterface/xpt/src/core65nm/bxpt_directv_playback.c \
		$(MAGNUM)/portinginterface/xpt/src/core65nm/bxpt_directv_pcr.c \
		$(MAGNUM)/portinginterface/xpt/src/core65nm/bxpt_sam.c \
		$(MAGNUM)/portinginterface/xpt/src/core65nm/bxpt_spid.c \
		$(MAGNUM)/portinginterface/xpt/src/core65nm/bxpt_packetsub.c \
		$(MAGNUM)/portinginterface/xpt/src/core65nm/bxpt_playback.c \
		$(MAGNUM)/portinginterface/xpt/src/core65nm/bxpt_pcr.c \
		$(MAGNUM)/portinginterface/xpt/src/core65nm/bxpt_interrupt.c \
		$(MAGNUM)/portinginterface/xpt/src/core65nm/bxpt_directv_remux.c \
		$(MAGNUM)/portinginterface/xpt/src/core65nm/bxpt_remux.c \
		$(MAGNUM)/portinginterface/xpt/src/core65nm/bxpt_remux_private.c \
		$(MAGNUM)/portinginterface/xpt/src/core65nm/bxpt_mpod.c \
		$(MAGNUM)/portinginterface/xpt/src/core65nm/bxpt_mesg.c \
		$(MAGNUM)/portinginterface/xpt/src/core65nm/bxpt_rsbuf.c \
		$(MAGNUM)/portinginterface/xpt/src/core65nm/bxpt_xcbuf.c \
		$(MAGNUM)/portinginterface/xpt/src/core65nm/bxpt_rave.c \
		$(MAGNUM)/portinginterface/xpt/src/core65nm/bxpt_directv_pcroffset.c \
		$(MAGNUM)/portinginterface/xpt/src/rave/$(RAVE_IMAGE_DIR)/bxpt_rave_ihex.c \
		$(MAGNUM)/portinginterface/xpt/src/core65nm/bxpt_pcr_offset_emv.c 
endif

ifeq ($(findstring $(BCHP_CHIP), 7408), $(BCHP_CHIP))
	RAVE_IMAGE_DIR = cat7408b

	BXPT_INCLUDES += $(MAGNUM)/portinginterface/xpt/src/core65nm/

	BXPT_SOURCES += \
		$(MAGNUM)/portinginterface/xpt/src/core65nm/bxpt.c \
		$(MAGNUM)/portinginterface/xpt/src/core65nm/bxpt_directv.c \
		$(MAGNUM)/portinginterface/xpt/src/core65nm/bxpt_directv_playback.c \
		$(MAGNUM)/portinginterface/xpt/src/core65nm/bxpt_directv_pcr.c \
		$(MAGNUM)/portinginterface/xpt/src/core65nm/bxpt_directv_pcroffset.c \
		$(MAGNUM)/portinginterface/xpt/src/core65nm/bxpt_spid.c \
		$(MAGNUM)/portinginterface/xpt/src/core65nm/bxpt_playback.c \
		$(MAGNUM)/portinginterface/xpt/src/core65nm/bxpt_pcr.c \
		$(MAGNUM)/portinginterface/xpt/src/core65nm/bxpt_rave.c \
		$(MAGNUM)/portinginterface/xpt/src/rave/$(RAVE_IMAGE_DIR)/bxpt_rave_ihex.c \
		$(MAGNUM)/portinginterface/xpt/src/core65nm/bxpt_pcr_offset_emv.c 
endif

ifeq ($(findstring $(BCHP_CHIP), 7550), $(BCHP_CHIP))
	RAVE_IMAGE_DIR = cat2a

	BXPT_INCLUDES += $(MAGNUM)/portinginterface/xpt/src/core65nm/

	BXPT_SOURCES += \
		$(MAGNUM)/portinginterface/xpt/src/core65nm/bxpt.c \
		$(MAGNUM)/portinginterface/xpt/src/core65nm/bxpt_spid.c \
		$(MAGNUM)/portinginterface/xpt/src/core65nm/bxpt_packetsub.c \
		$(MAGNUM)/portinginterface/xpt/src/core65nm/bxpt_playback.c \
		$(MAGNUM)/portinginterface/xpt/src/core65nm/bxpt_pcr.c \
		$(MAGNUM)/portinginterface/xpt/src/core65nm/bxpt_remux.c \
		$(MAGNUM)/portinginterface/xpt/src/core65nm/bxpt_remux_private.c \
		$(MAGNUM)/portinginterface/xpt/src/core65nm/bxpt_mpod.c \
		$(MAGNUM)/portinginterface/xpt/src/core65nm/bxpt_rave.c \
		$(MAGNUM)/portinginterface/xpt/src/rave/$(RAVE_IMAGE_DIR)/bxpt_rave_ihex.c \
		$(MAGNUM)/portinginterface/xpt/src/core65nm/bxpt_pcr_offset_emv.c 
endif

# Module Includes
BXPT_INCLUDES += $(MAGNUM)/portinginterface/xpt/include
BXPT_INCLUDES += $(MAGNUM)/portinginterface/xpt/src/rave/$(RAVE_IMAGE_DIR)









