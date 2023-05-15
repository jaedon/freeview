#############################################################################
#
#       Copyright (c) 2007-2012, Broadcom Corporation.
#       All rights reserved.
#       Confidential Property of Broadcom Corporation.
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: Makefile $
# $brcm_Revision: Hydra_Software_Devel/119 $
# $brcm_Date: 10/17/12 4:15p $
#
# File Description:
#
# Revision History:
#
# $brcm_Log: /rockford/unittests/portinginterface/vdc/Makefile $
# 
# Hydra_Software_Devel/119   10/17/12 4:15p vanessah
# SW7445-60: add MRC monitor flag to speedup emulation
# 
# Hydra_Software_Devel/118   10/11/12 4:27p vanessah
# SW7425-2501: remove vdec input support
# 
# Hydra_Software_Devel/117   10/11/12 12:09p syang
# SW7346-572: add long-time-lock for vdc_test
# 
# Hydra_Software_Devel/116   10/9/12 2:01p syang
# SW7346-572: rm TV related (vdb, dynamic rts, ...); rm symbolic link to
# files in applications/bmetest/grc_test
# 
# Hydra_Software_Devel/115   10/9/12 10:18a vanessah
# SW7445-10: enable VCE/MUX in 7445
# 
# Hydra_Software_Devel/114   8/30/12 11:11a vanessah
# SW7425-3359: enable vice in compile
# 
# Hydra_Software_Devel/113   8/29/12 12:51p vanessah
# SW7425-3359: disable vice temporarily in compile
# 
# Hydra_Software_Devel/112   8/9/12 11:26a syang
# SW7425-2172: remove gfx_to_mfd.c and pan_vec_gen.c
# 
# Hydra_Software_Devel/111   8/2/12 2:14p tdo
# SW7563-9: Put back GRC into compilation for 7563
#
# Hydra_Software_Devel/110   7/31/12 3:02p tdo
# SW7563-9: Temporarily remove GRC for 7563 until support added.
#
# Hydra_Software_Devel/109   7/26/12 5:25p vanessah
# SW7360-3: remove power management for 7360 temporarily
#
# Hydra_Software_Devel/108   6/21/12 1:25p tdo
# SW7584-5: Merge to mainline
#
# Hydra_Software_Devel/SW7584-5/1   6/19/12 5:19p chengs
# SW7584-5: Add 7584 support.
#
# Hydra_Software_Devel/107   6/1/12 7:15p vanessah
# SW7435-201: add run time transcode rts selection
#
# Hydra_Software_Devel/106   5/16/12 3:53p vanessah
# SW7425-2643: enable VCE/MUX in VDC test to avoid compile error
#
# Hydra_Software_Devel/105   5/1/12 3:06p vanessah
# SW7425-2643: disable VCE/MUX in VDC test to avoid compile error
#
# Hydra_Software_Devel/104   3/13/12 3:46p vanessah
# SW7435-9: set the 7435 default for RTS 1u4t for EC
#
# Hydra_Software_Devel/103   3/6/12 9:57p vanessah
# SW7435-9: add Quad transcode support
#
# Hydra_Software_Devel/102   2/16/12 10:10a tdo
# SW7435-9: put back VCE and MUX compiling for 7435
#
# Hydra_Software_Devel/101   2/15/12 1:26p tdo
# SW7435-9: Temporarily remove VCE and MUX compiling for 7435 until the
# labeling issue resolved
#
# Hydra_Software_Devel/100   2/8/12 10:54a tdo
# SW7425-2330: Enable power management 2.0 in appframework
#
# Hydra_Software_Devel/99   2/2/12 10:22a tdo
# SW7435-9: Add VCE support back for 7435
#
# Hydra_Software_Devel/98   12/6/11 3:57p tdo
# SW7435-9: Temporarily exclude VCE compilation for 7435
#
# Hydra_Software_Devel/97   11/23/11 2:45p tdo
# SW7435-9: add support for 7435A0 in VDC
#
# Hydra_Software_Devel/96   11/8/11 3:52p yuxiaz
# SW7231-387: Added compile option to enable HD_DVI nxp support.
#
# Hydra_Software_Devel/95   9/6/11 10:39a vanessah
# SW7425-1246: make file change for 7425 b0 emulation
#
# Hydra_Software_Devel/94   8/3/11 12:40p tdo
# SW7425-1043: undo previous check in
#
# Hydra_Software_Devel/93   8/3/11 11:16a tdo
# SW7425-1043: vdc_test: gfxmfd does not work and give pink screen.
# Include DMA support for gfxmfd cmd to work with MFD on MEMC1
#
# Hydra_Software_Devel/92   4/25/11 5:20p pntruong
# SW7422-416: Adapted vdc_test build system to new software chiplead
# configspec that does not load the old dma.
#
# Hydra_Software_Devel/91   4/8/11 9:22a nilesh
# SW7425-297: Changed to use muxlib.inc and vce.inc for video encoder and
# muxer libraries
#
# Hydra_Software_Devel/90   4/4/11 12:13p nilesh
# SW7425-73: MP4 Mux support
#
# Hydra_Software_Devel/mp4_mux_devel/2   3/30/11 3:11p nilesh
# SW7425-73: Added MP4 support
#
# Hydra_Software_Devel/mp4_mux_devel/1   3/30/11 2:01a nilesh
# SW7425-73: Added muxer_ts.c
#
# Hydra_Software_Devel/89   3/16/11 5:03p pntruong
# SW7425-164: Default B_REFSW_DEBUG=y if unset in framework build system
# so not every single app needs to specify.
#
# Hydra_Software_Devel/88   3/15/11 3:57p darnstein
# SW7425-164: Adapt to changes in the "plat" script.
# - Change DEBUG to B_REFSW_DEBUG.
# - Change ARCH to B_REFSW_ARCH.
# - Change OS to B_REFSW_OS.
# - Change mips-linux-uclibc to mips-linux.
# - Change mipsel-linux-uclibc to mipsel-linux.
#
# Hydra_Software_Devel/87   3/8/11 4:36p tdo
# SW7420-1610: undo previous check in.  BCHP_PWR_SUPPORT=n is default.
#
# Hydra_Software_Devel/86   3/8/11 12:54p tdo
# SW7420-1610: temp fix for vdc_test bus error in EC
#
# Hydra_Software_Devel/85   2/16/11 4:31p yuxiaz
# SW7420-1178: Merge loopback support to mainline. Added compile option
# to enable loopback support.
#
# Hydra_Software_Devel/84   2/9/11 4:07p yuxiaz
# SW7400-2882: Added compile flag to enable multi-buffering log.
#
# Hydra_Software_Devel/83   1/12/11 4:55p yuxiaz
# SW7335-1133: Added compile flag for VDC heap debugging.
#
# Hydra_Software_Devel/82   1/7/11 4:57p hongtaoz
# SW7425-10: enable encoder for 7425 by default;
#
# Hydra_Software_Devel/81   12/16/10 11:23p pntruong
# SW7358-17: No rfm for 7358.
#
# Hydra_Software_Devel/80   12/10/10 2:25p pntruong
# SW7231-16: Default exclude hsm.
#
# Hydra_Software_Devel/79   12/3/10 10:38a hongtaoz
# SW7425-32: fixed compile error when VDC_TEST_ONLY=n;
#
# Hydra_Software_Devel/78   12/2/10 3:08p katrep
# SW7231-16:add more support for 7231
#
# Hydra_Software_Devel/77   11/29/10 10:54a pntruong
# SW7552-17: Fixed 7405 build errors.
#
# Hydra_Software_Devel/76   11/24/10 12:55p pntruong
# SW7552-17: Added initial 7552 support.
#
# Hydra_Software_Devel/75   11/16/10 5:02p pntruong
# SW7231-16: Initial support for 7231.
#
# Hydra_Software_Devel/74   11/11/10 6:43p pntruong
# SW7425-31: Refactored hdm/lvds/hdr/dvo and added support hdmi input on
# stb chips.  Removed dead codes.
#
# Hydra_Software_Devel/73   11/10/10 11:44a tdo
# SW7422-13: 7422 bringup
#
# Hydra_Software_Devel/72   11/3/10 2:39p nilesh
# SW7425-38: Added TS Muxlib syslib support
#
# Hydra_Software_Devel/71   11/1/10 2:03p yuxiaz
# SW7358-17: Added 7358 support to vdc_test.
#
# Hydra_Software_Devel/70   10/26/10 6:31p hongtaoz
# SW7425-32: added XPT support for 7425;
#
# Hydra_Software_Devel/69   10/21/10 11:19a nilesh
# SW7425-1: Added VCE Encoder Support (enable by compiling with
# SUPPORT_VCE=y)
#
# Hydra_Software_Devel/68   10/15/10 2:23p vanessah
# SW7358-15:  VDC_test compile w.r.t DMA
#
# Hydra_Software_Devel/67   9/27/10 4:07p hongtaoz
# SW7425-32: added VDC_TEST_ONLY compile option;
#
# Hydra_Software_Devel/66   9/23/10 3:36p tdo
# SW7422-13: add 7422 support to vdc_test
#
# Hydra_Software_Devel/65   12/15/09 5:48p yuxiaz
# SW7408-13: Get vdc_test compile for 7408. Clean up compile flags.
#
# Hydra_Software_Devel/64   12/9/09 11:08a rpan
# SW7468-22: Enable GRC for 7468.
#
# Hydra_Software_Devel/63   12/4/09 3:57p tdo
# SW7408-23: Add playback feature for digital source
#
# Hydra_Software_Devel/62   12/4/09 2:51p syang
# SW7550-70: add mem alloc msg
#
# Hydra_Software_Devel/61   12/1/09 4:46p rpan
# SW7468-22: Got vdc_test compiled for 7468.
#
# Hydra_Software_Devel/60   11/23/09 3:59p rpan
# SW7468-22: Initial work for 7468.
#
# Hydra_Software_Devel/59   11/17/09 2:06p syang
# SW7550-70: turn on HDM out
#
# Hydra_Software_Devel/58   10/23/09 10:49a pntruong
# SW7340-66: Add endianness support in appframework build system.
#
# Hydra_Software_Devel/57   9/1/09 8:04p albertl
# SW7125-13: Enabled HDMI output for 7125.
#
# Hydra_Software_Devel/56   8/31/09 5:56p albertl
# SW7125-13: Added initial 7125 support.
#
# Hydra_Software_Devel/55   8/10/09 3:51p jessem
# PR 54380: Changed all references to linuxclient to linuxemu.
#
# Hydra_Software_Devel/54   8/4/09 2:05p darnstein
# PR55225: no RFM hardware in 7342 chipset.
#
# Hydra_Software_Devel/53   8/4/09 1:01p darnstein
# PR55225: mechanical updates for 7342 chipset.
#
# Hydra_Software_Devel/52   7/31/09 6:49p syang
# PR 55812:  add 7550 support
#
# Hydra_Software_Devel/51   7/10/09 5:54p rpan
# PR55188: Convert scanf() to BTST_P_Scanf().
#
# Hydra_Software_Devel/50   6/22/09 4:20p darnstein
# PR55219: port to 7340 chipset.
#
# Hydra_Software_Devel/49   5/8/09 3:35p yuxiaz
# PR 54512: Added bgfx_blit.c
#
# Hydra_Software_Devel/48   4/29/09 11:12a jessem
# PR 54380: Changed IKOS_EMULATION to EMULATION. Added support for IKOS
# and PALLADIUM emulators.
#
# Hydra_Software_Devel/47   3/31/09 5:32p hongtaoz
# PR53038, PR53688: added compile option to enable double-buffer debug
# message;
#
# Hydra_Software_Devel/46   3/17/09 1:59p darnstein
# PR53265: Remove BVBI, BVBIlib software. It never worked.
#
# Hydra_Software_Devel/45   1/20/09 9:17a rpan
# PR50391: Bring up HDMI output.
#
# Hydra_Software_Devel/44   12/3/08 10:28p tdo
# PR46484: Add p3d support for 7420
#
# Hydra_Software_Devel/43   12/2/08 5:42p tdo
# PR46484: Bringup vdc_test
#
# Hydra_Software_Devel/42   10/22/08 11:04p nissen
# PR 46310: Added GRC and P3D tests.
#
# Hydra_Software_Devel/41   8/4/08 2:16p jessem
# PR 45385: Added bvdb_parser.c.
#
# Hydra_Software_Devel/40   7/22/08 2:14p yuxiaz
# PR44096: Add 656 out support on 3549.
#
# Hydra_Software_Devel/39   7/14/08 5:05p pntruong
# PR39152: Fixed build error.
#
# Hydra_Software_Devel/38   7/14/08 3:27p jessem
# PR 39152: Integrated use of VDB RTS.
#
# Hydra_Software_Devel/36   7/9/08 4:12p yuxiaz
# PR39322: Add 656 input support on 3548.
#
# Hydra_Software_Devel/35   7/1/08 5:16p rpan
# PR36501: Initial work for adding tuner support fror 93549.
#
# Hydra_Software_Devel/34   6/20/08 10:53a jessem
# PR 43759: Modified VDB rule.
#
# Hydra_Software_Devel/33   6/18/08 1:28p jessem
# PR 39237, PR 43759: Added BVDB (Video Usage Modes Database) tests.
#
# Hydra_Software_Devel/32   6/17/08 2:29p rpan
# PR43668: Adopt the new LVD PI for 3548/3556.
#
# Hydra_Software_Devel/31   6/11/08 11:49p tdo
# PR39331: Bringup dynamic contrast for 3548/3556
#
# Hydra_Software_Devel/30   6/11/08 11:24a rpan
# PR39807: Convert OUTPUT_RESOLUTION from string.
#
# Hydra_Software_Devel/29   6/11/08 11:04a rpan
# PR39807: Add panel macro OUTPUT_RESOLUTION.
#
# Hydra_Software_Devel/28   6/5/08 9:52p pntruong
# PR39321: Reenabled hdr compilations.
#
# Hydra_Software_Devel/27   6/4/08 2:15p rpan
# PR39807: Enable panel timing macro.
#
# Hydra_Software_Devel/26   5/5/08 12:04p rpan
# PR39807: Enabled DVPO support for 3548/3556.
#
# Hydra_Software_Devel/25   4/15/08 10:14a tdo
# PR39333: Remove xpt build for IKOS
#
# Hydra_Software_Devel/24   4/8/08 10:54a tdo
# PR41488:  Add 3556 build support
#
# Hydra_Software_Devel/23   4/1/08 3:10p hongtaoz
# PR40499: added RUL debug compile flag;
#
# Hydra_Software_Devel/22   3/31/08 3:43p jessem
# PR 39335: Added CM3D test file.
#
# Hydra_Software_Devel/21   3/10/08 8:35p darnstein
# PR25971: first try at DCS for 7400
#
# Hydra_Software_Devel/20   2/21/08 4:05p rpan
# PR39807: Added compile flag for IKOS.
#
# Hydra_Software_Devel/19   2/21/08 3:49p rpan
# PR39807: Work for bcm3548 bringup.
#
# Hydra_Software_Devel/18   2/7/08 5:56p jessem
# PR 39142: Removed -DIKOS compile flag and moved to framework.inc.
#
# Hydra_Software_Devel/17   2/7/08 5:04p jessem
# PR 39142: Added IKOS support.
#
# Hydra_Software_Devel/16   11/15/07 6:04p pntruong
# PR32896: Prevent false vcr detection with copy protection contents.
#
# Hydra_Software_Devel/15   11/9/07 11:58a yuxiaz
# PR34540: 7325 bringup.
#
# Hydra_Software_Devel/14   10/27/07 11:08p pntruong
# PR36137: Enable bvn debug if not specified.
#
# Hydra_Software_Devel/13   10/18/07 12:08a pntruong
# PR36137: [PC]:Set pc input video format without disable auto format
# detection.
#
# Hydra_Software_Devel/12   10/16/07 2:45p rpan
# PR34857: First version after merging the latest xvd_vdc_test and
# mvd_vdc_test as of Oct. 16, 2007, 12pm.
#
# Hydra_Software_Devel/1   9/17/07 4:38p rpan
# PR34857: Initial revision. Ported from xvd_vdc_test.
#
#
#############################################################################

# Requires the use of the 'plat' script which defines the following:
#   PLATFORM (97038, 97395, ...)
#   BCHP_REV (B0, B1, C0, ...)
#   B_REFSW_DEBUG
#   B_REFSW_ARCH
#   LINUX
#   TOOLCHAIN

# Target O/S (can be one of the following: linuxuser linuxemu vxworks)
ifeq ($(SYSTEM),vxworks)
    TARGET_OS = vxworks
else
ifeq ($(SYSTEM), linuxemu)
    TARGET_OS = linuxemu
    ifeq ($(BRDC_EMU_STUB_SERVER), y)
        CFLAGS += -DBRDC_EMU_STUB_SERVER=1
    endif
else
    TARGET_OS = linuxuser
endif
endif

# Directories relative to location of makefile
MAGNUM_DIR = ../../../../magnum
ROCKFORD_DIR = ../../../../rockford
FRAMEWORK_DIR = $(ROCKFORD_DIR)/appframework

# Name of application
ifeq (${SYSTEM},vxworks)
APP = vdc_test.out
else
APP = vdc_test
endif

ifeq ($(TARGET_OS),vxworks)
CFLAGS += -DVXWORKS
else
        ifeq ($(TARGET_OS),linuxemu)
        CFLAGS += -DEMULATION
        endif
CFLAGS += -DLINUX
endif

ifeq ($(EMU),IKOS)
CFLAGS += -DIKOS_EMULATION
else
        ifeq ($(EMU),PALLADIUM)
CFLAGS += -DPALLADIUM_EMULATION
        endif
endif

# Application object file list, include path, and C flags
APP_SOURCES = \
        cmd_display.c \
        cmd_handle.c \
        cmd_misc.c \
        cmd_source.c \
        cmd_window.c \
        cmd_graphics.c \
        interactive.c \
        intercept.c \
        memview.c \
        bgfx_image.c \
        pxl_gen.c \
        static_buffer.c \
        vdc_test.c \
        vdc_scanf.c \
        test_util.c

SUPPORT_CM3D ?= y
ifeq (${SUPPORT_CM3D},y)
APP_SOURCES += \
        cmd_cm3d.c
endif

#
# Dynamics RTS & Panel Timing, 3563=FullHD+PIP, 3553=FullHD, 3552=WXGA
#
BVDC_DYNAMICS_RTS ?= 3553

#
# forced synclock double-buffer debug message
#
DEBUG_FORCE_SYNC_LOCK ?= n
ifeq (${DEBUG_FORCE_SYNC_LOCK}, y)
        CFLAGS += -DBVDC_DEBUG_FORCE_SYNC_LOCK
endif

#
# Panel Timing, 3563=FullHD, 3552=WXGA
#
OUTPUT_RESOLUTION ?= fhd

ifeq (${OUTPUT_RESOLUTION}, wxga)
    PANEL_TIMING = 3552
else
    PANEL_TIMING = 3563
endif


APP_INCLUDES = ./
APP_CFLAGS =
APP_LDFLAGS=


# Define other chips to use. If not defined, files will not be included.
#BCHP_QAM_CHIP = 3250
#BCHP_SDS_CHIP = 4500
#BCHP_VSB_CHIP = 3510

# Show build commands (y/n)?
VERBOSE=n
BVN_DEBUG ?= y
RUL_DEBUG ?= y
MRC_MONITOR ?= y
ifneq ($(findstring $(BCHP_CHIP), 7360),)
FRAMEWORK_DO_PM ?= y
endif

ifeq (${XVD_LOG},y)
        CFLAGS += -DENABLE_DM_LOGS -DDEBUG_DispMgr
endif

ifeq (${USER_ADJ_QP}, 1)
        CFLAGS += -DBTST_USE_ESTIMATED_ADJ_QP
endif

ifeq ($(BVDC_TIMESTAMP), y)
        CFLAGS += -DBVDC_DEBUG_TIMESTAMP
endif

ifeq ($(BVDC_BUFFER), y)
        CFLAGS += -DBVDC_DEBUG_BUFFER
endif

ifeq ($(BVDC_MULTIBUF_LOG), y)
        CFLAGS += -DBVDC_BUF_LOG
endif

ifeq ($(BVN_DEBUG), y)
        CFLAGS += -DBVDC_SUPPORT_BVN_DEBUG=1
endif

ifeq ($(BMEM_TRACK), y)
        CFLAGS += -DBMEM_SAFETY_CONFIG=2
endif

ifeq ($(RUL_DEBUG), y)
        CFLAGS += -DBRDC_USE_CAPTURE_BUFFER=1
endif

ifeq ($(DYN_CONTRAST_DEBUG), 1)
        CFLAGS += -DPEP_DYN_CONTRAST_DBG
endif

ifeq ($(BTST_SUPPORT_656IN), y)
        CFLAGS += -DBTST_SUPPORT_656IN
endif

ifeq ($(BTST_SUPPORT_656OUT), y)
        CFLAGS += -DBTST_SUPPORT_656OUT
endif

ifeq ($(BTST_SUPPORT_DVO_HDDVI_LOOPBACK), y)
        CFLAGS += -DBTST_SUPPORT_DVO_HDDVI_LOOPBACK
endif

ifeq ($(BTST_SUPPORT_HDDVI_EXT_NXP), y)
        CFLAGS += -DBTST_SUPPORT_HDDVI_EXT_NXP
endif

BTST_MFD_SIZE ?= n
ifeq ($(BTST_MFD_SIZE), y)
        CFLAGS += -DBTST_MFD_SIZE=1
endif

BTST_FORCE_MUTE ?= n
ifeq ($(BTST_FORCE_MUTE), y)
        CFLAGS += -DBTST_FORCE_MUTE=1
endif

BTST_3563_DVO_BRINGUP ?= n
ifeq ($(BTST_3563_DVO_BRINGUP), y)
        CFLAGS += -DBTST_3563_DVO_BRINGUP=1
endif
ifeq (${MRC_MONITOR}, y)
	CFLAGS += -DBTST_P_USE_MRC_MONITOR=1
endif

# Include object file list, include path, and C flags for common utilities and base modules
# (sets MAGNUM_SOURCES, MAGNUM_INCLUDES, MAGNUM_CFLAGS, and MAGNUM_MODULE_DIR)
include $(FRAMEWORK_DIR)/build/magnum.inc

# Include object file list, include path, and C flags for other magnum modules required
# by this application (adds to MAGNUM_SOURCES, MAGNUM_INCLUDES, and MAGNUM_CFLAGS).
# Here is where you also define module specific makefile defines
BVDC_MACROVISION ?= y
BVDC_DCS ?= n
ifeq ($(BVDC_MACROVISION), y)
        CFLAGS += -DBVDC_MACROVISION=1
        CFLAGS += -DMACROVISION_SUPPORT=1
else
        CFLAGS += -DBVDC_MACROVISION=0
endif

ifeq ($(BVDC_DCS), y)
        ifeq ($(BVDC_MACROVISION),y)
                error Macrovision plus DCS not allowed
        endif
        CFLAGS += -DDCS_SUPPORT=1
endif

include $(MAGNUM_DIR)/portinginterface/vdc/bvdc.inc


# VDC test only
ifeq ($(VDC_TEST_ONLY),y)
        CFLAGS += -DBTST_VDC_ONLY=1

ifeq (${SUPPORT_VCE}, y)
    CFLAGS += -DBTST_P_SUPPORT_VCE=1
    include $(ROCKFORD_DIR)/unittests/portinginterface/vce/vce.inc
endif
else

APP_SOURCES += \
        mpg_decoder.c \
        xport.c \
        playback.c

#
# Default exclude, if chip does have it then put them in the list.
# most likely does not have it.
#
ifneq ($(findstring $(BCHP_CHIP), 7425 7435 7445),)
	SUPPORT_VCE ?= y
	SUPPORT_MUX ?= y
endif

ifeq (${SUPPORT_VCE}, y)
    CFLAGS += -DBTST_P_SUPPORT_VCE=1
    include $(ROCKFORD_DIR)/unittests/portinginterface/vce/vce.inc

    ifeq (${SUPPORT_MUX}, y)
        CFLAGS += -DBTST_P_SUPPORT_MUX=1
        include $(ROCKFORD_DIR)/unittests/syslib/muxlib/muxlib.inc
    endif
endif

ifneq ($(findstring $(BCHP_CHIP), 7435 7422 7425 3548 3556),)
        CFLAGS += -DBTST_P_SUPPORT_HDR=1
        include $(MAGNUM_DIR)/portinginterface/hdr/bhdr.inc
endif

ifneq ($(findstring $(BCHP_CHIP), 7405 7325 7335 7336 7552 7584),)
        CFLAGS += -DBTST_P_SUPPORT_RFM_OUTPUT=1
        include $(MAGNUM_DIR)/portinginterface/rfm/brfm.inc
endif

ifneq ($(findstring $(BCHP_CHIP), 7400 7405),)
        CFLAGS += -DBTST_P_SUPPORT_HSM=1
        include $(MAGNUM_DIR)/portinginterface/hsm/bhsm.inc
endif

ifneq ($(findstring $(BCHP_CHIP), 7400 3548 3556 7420 7336 7340 7342 7125 7408),)
        APP_LDFLAGS += -lm
        CFLAGS      += -DBTST_P_SUPPORT_P3D=1
        include $(MAGNUM_DIR)/portinginterface/p3d/bp3d.inc
endif

ifneq ($(findstring $(BCHP_CHIP), 7405 7400),)
        CFLAGS += -DBTST_P_SUPPORT_DMA=1
        include $(MAGNUM_DIR)/portinginterface/dma/bdma.inc
endif

ifneq ($(findstring $(BCHP_CHIP), 3548 3556),)
        CFLAGS += -DBTST_P_SUPPORT_LVD=1
        include $(MAGNUM_DIR)/portinginterface/pwm/bpwm.inc
        include $(MAGNUM_DIR)/portinginterface/lvd/blvd.inc
        ifeq (${BCHP_CHIP}, 3548)
                BTNR_DTT76800_SUPPORT = y
                include $(MAGNUM_DIR)/portinginterface/tnr/btnr.inc
                include $(MAGNUM_DIR)/portinginterface/vsb/bvsb.inc
                ifneq ($(TARGET_OS),linuxemu)
                        APP_SOURCES += frontend.c cmd_frontend.c
                endif
        endif
endif

#
# Always include!!!
#
include $(MAGNUM_DIR)/portinginterface/xpt/bxpt.inc
include $(MAGNUM_DIR)/portinginterface/xvd/bxvd.inc

#
# Currently Supported chips:
#   7400 7405 7325 7335 7336 7340 7342 7550 3548 3556 7420 7125 7468 7408
#   7422 7425 7358 7231 7344 7346
#

#
# Default include, if chip does not then put them in the list.  If chip
# has it don't put it here but rather get the module owner to support it.
# If absolutely need to bypass do so AND file the JIRA.
# "I mean it" -- Philip Truong.
#

# Don't have HDM?
ifeq ($(findstring $(BCHP_CHIP), 3548 3556),)
        CFLAGS += -DBTST_P_SUPPORT_HDM=1
        include $(MAGNUM_DIR)/portinginterface/hdm/bhdm.inc
endif

# Don't have GRC?
ifeq ($(findstring $(BCHP_CHIP), 7550),)
        CFLAGS += -DBTST_P_SUPPORT_GRC=1
        include $(MAGNUM_DIR)/portinginterface/grc/bgrc.inc
endif

# !VDC_TEST_ONLY
endif


# At this point, user can add additional files, include paths, and C flags to
# MAGNUM_OBJ, MAGNUM_INC, and MAGNUM_CFLAGS (optional).

# Include other common modules (adds to COMMON_SOURCES, COMMON_INCLUDES, and
# COMMON_CFLAGS)
#include $(FRAMEWORK_DIR)/build/modules/common/fpga.inc

# Specifies how magnum files and application files are built.
include $(FRAMEWORK_DIR)/build/$(TARGET_OS)/build.inc

#
# End of file
#

