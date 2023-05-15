############################################################
#     Copyright (c) 2003-2006, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: settop.mak $
# $brcm_Revision: 3 $
# $brcm_Date: 9/25/06 2:43p $
#
# Module Description:
#
# Revision History:
#
# Created: 02/09/2001 by Marcus Kellerman
#
# $brcm_Log: /BSEAV/api/build/legacy/settop.mak $
# 
# 3   9/25/06 2:43p gmohile
# PR 24129: Add sds proxy path for 7313
# 
# 2   9/20/06 11:48a gmohile
# PR 24129: Added support for 7313
# 
# 1   2/7/05 6:53p dlwin
# Merge down for release 2005_REFSW_MERGETOMAIN:
# 
# Irvine_BSEAVSW_Devel/20   9/22/04 3:06p bandrews
# PR12687: Initial 7312 7327 support
# 
# Irvine_BSEAVSW_Devel/19   8/18/04 2:56p bandrews
# PR11657: Fixed sds include directory
# 
# Irvine_BSEAVSW_Devel/18   8/18/04 12:38p bandrews
# PR11657: 7329 support
# 
# Irvine_BSEAVSW_Devel/17   8/3/04 1:44p bandrews
# PR11657: Reverted to using mostly 7328 code after the video rdb
# problems were fixed (_0)
# 
# Irvine_BSEAVSW_Devel/16   7/28/04 5:51p bandrews
# PR11657: Added 7329
# 
# Irvine_BSEAVSW_Devel/15   6/1/04 4:24p bandrews
# PR10919: Added 7314
# 
# Irvine_BSEAVSW_Devel/14   5/12/04 10:23a erickson
# PR10967: added standard header
############################################################

# Porting interface and library directories
#

PORTINTF 		= $(SETTOP)/portinginterface
SYSLIB			= $(SETTOP)/syslib

ifeq ($(PLATFORM),97314)
	AUDIOCTRL_CHIP 	= $(PORTINTF)/audiocontrol/$(BCM_CHIP_FLAVOR)
else
	AUDIOCTRL_CHIP 	= $(PORTINTF)/audiocontrol/$(BCM_CHIP)
endif

AUDIOCTRL 		= $(PORTINTF)/audiocontrol
AUDIO 			= $(SETTOP)/audio
CHANMGRDIR		= $(SETTOP)/channelmgr
DEVICE 			= $(SETTOP)/deviceinterface

ifeq ($(findstring $(PLATFORM),97314),$(PLATFORM))
	DISPCTRL 		= $(PORTINTF)/displaycontrol/$(BCM_CHIP_FLAVOR)
else
	DISPCTRL 		= $(PORTINTF)/displaycontrol/$(BCM_CHIP)
endif

DMACTRL 		= $(PORTINTF)/dmactrl
DMA_CHIP 		= $(DMACTRL)/$(BCM_CHIP)
GFX2D			= $(SETTOP)/graphics2d
GFXCTRL 		= $(PORTINTF)/graphicscontrol
GFX_CHIP 		= $(GFXCTRL)/$(BCM_CHIP)

HWINTF 			= $(SETTOP)/hardwareinterface

ifeq ($(CHIP),venom2)
	HW_BOARD 		= $(HWINTF)/bcm93730_rb0
else
	HW_BOARD 		= $(HWINTF)/$(BCM_BOARD)
endif

ifeq ($(findstring $(PLATFORM),97314 97317),$(PLATFORM))
	HW_CHIP			= $(HWINTF)/$(BCM_CHIP_FLAVOR)/include
	HW_CHIP_BME		= $(HW_CHIP)/bme
else
	ifeq ($(PLATFORM),97112)
		HW_CHIP 		= $(HWINTF)/7112/include
		HW_CHIP_BME 	= $(HWINTF)/7115/include/bme
	else
		HW_CHIP 		= $(HWINTF)/$(BCM_CHIP)/include
		HW_CHIP_BME 	= $(HW_CHIP)/bme
	endif
endif

HW_BOARD_ENDIAN = $(HW_BOARD)/$(BCM_ENDIAN)
HW_CHIP_ENDIAN 	= $(HW_CHIP)/$(BCM_ENDIAN)

ICAM			= $(PORTINTF)/icam/$(BCM_CHIP)
KERNELINTF		= $(SETTOP)/kernelinterface
MEMLIB			= $(SETTOP)/memorylib
MISCLIB 		= $(SETTOP)/bcmmisclib

ifeq ($(PLATFORM),97314)
	MPEGAUDIO 		= $(PORTINTF)/mpegaudio/7317
else
	MPEGAUDIO 		= $(PORTINTF)/mpegaudio/$(BCM_CHIP)
endif

MPEGENCODER		= $(PORTINTF)/encoder

ifeq ($(CHIP),venom2)
	MPEGVIDEO 		= $(PORTINTF)/mpegvideo/venom2
else
	MPEGVIDEO 		= $(PORTINTF)/mpegvideo/minititan
endif

MPEGVIDEO_FW 	= $(MPEGVIDEO)/microcode
MPEGVIDEO_INC 	= $(MPEGVIDEO)/include
MPEGVIDEO_SRC 	= $(MPEGVIDEO)/source

PI_BTSC			= $(PORTINTF)/btsc/$(BCM_CHIP)

ifeq ($(PLATFORM),97314)
	PI_DEXLIB		= $(PORTINTF)/common/dexlib/$(BCM_CHIP_FLAVOR)
else
	PI_DEXLIB		= $(PORTINTF)/common/dexlib/$(BCM_CHIP)
endif

PI_GFX2D		= $(PORTINTF)/graphics2d
PI_GPIO 		= $(PORTINTF)/gpio
PI_INCLUDE 		= $(PORTINTF)/include
PLD             = $(PORTINTF)/pld
QAMPF           = $(PORTINTF)/qam

ifeq ($(findstring $(CHIP),7312 7313 7314 7315 7317 7319 7320 7327 7328 7329),$(CHIP))
	QAMMAIN         = $(QAMPF)/7115
else
	ifeq ($(BCM_BOARD_NO),93740)
		QAMMAIN         = $(QAMPF)
	else
		QAMMAIN         = $(QAMPF)/$(BCM_CHIP)
	endif
endif

QAM_3125		= ${QAMPF}/3125
QAM_3250		= ${QAMPF}/3250

ifeq ($(CHIP),venom2)
	QAM_3250_BOARD	= $(QAM_3250)/93730a0
else
	QAM_3250_BOARD	=
endif

ifeq ($(findstring $(CHIP),7312 7313 7327 7328 7329),$(CHIP))
	SDS_PF			= ${PORTINTF}/sds/$(BCM_CHIP)
ifeq ($(findstring $(CHIP),7313),$(CHIP))
	SDS_PROXY		= ${PORTINTF}/sds/$(BCM_CHIP)/proxy
endif
else
	SDS_PF			= ${PORTINTF}/sds
endif

SL_AUDIOLIB		= $(SYSLIB)/audiolib/$(BCM_CHIP)
SL_BTSC			= $(SYSLIB)/btsclib
SL_CCDECODE		= $(SYSLIB)/ccdlib
SL_CCENCODE		= $(SYSLIB)/ccelib
SL_CFG_DISP		= $(SYSLIB)/configdisplaylib/$(BCM_CHIP)

ifeq ($(CHIP),venom2)
	SL_CHANLIB      = $(SYSLIB)/chanlib
else
	ifeq ($(CHIP),7328)
		# it was never created for 7328
		SL_CHANLIB      = $(SYSLIB)/chanlib/bcm97320
	else
		ifeq ($(PLATFORM),97112)
			# it was never created for 7112
			SL_CHANLIB      = $(SYSLIB)/chanlib/bcm97115
		else
			SL_CHANLIB      = $(SYSLIB)/chanlib/$(BCM_BOARD)
		endif
	endif
endif

SL_GRAPHICS		= $(SYSLIB)/graphicslib/$(BCM_CHIP)
SL_MPEGAUDIO	= $(SYSLIB)/mpegaudiolib/$(BCM_CHIP)
SL_MPEGVIDEO	= $(SYSLIB)/mpegvideolib/$(BCM_CHIP)
SL_PCM			= $(SYSLIB)/pcmlib/$(BCM_CHIP)
SL_PVRLIB		= $(SYSLIB)/pvrlib/$(BCM_CHIP)
SL_SERIALPCR	= $(SYSLIB)/serialpcrlib/$(BCM_CHIP)
SL_TRANSPORT	= $(SYSLIB)/transportlib/$(BCM_CHIP)
SL_VIDEO		= $(SYSLIB)/videolib/$(BCM_CHIP)

#SMARTCARD		= $(PORTINTF)/smartcardlib
SMARTCARD_NDS   = $(SMARTCARD)/standards/nds
SMARTCARD       = $(PORTINTF)/smartcardlib/rc0
SOFTI2C 		= $(SETTOP)/softi2c

ifeq ($(CHIP),venom2)
	SURFACEDRAW 	= $(SETTOP)/surfacedraw
else
	ifeq ($(PLATFORM),97314)
		SURFACEDRAW 	= $(SETTOP)/surfacedraw/$(BCM_CHIP_FLAVOR)
	else
		SURFACEDRAW 	= $(SETTOP)/surfacedraw/$(BCM_CHIP)
	endif
endif

ifeq ($(findstring $(PLATFORM),97314 97317),$(PLATFORM))
	TM				= $(PORTINTF)/tm/$(BCM_CHIP_FLAVOR)
else
	TM				= $(PORTINTF)/tm/$(BCM_CHIP)
endif

ifeq ($(findstring $(PLATFORM),97314),$(PLATFORM))
	TRANSPORT 		= $(PORTINTF)/transport/$(BCM_CHIP_FLAVOR)
else
	TRANSPORT 		= $(PORTINTF)/transport/$(BCM_CHIP)
endif

TUNERPF         = $(PORTINTF)/tuner/$(BCM_BOARD)
UPGPF           = $(PORTINTF)/upg

ifeq ($(CHIP),venom2)
	UPGMAIN         = $(UPGPF)/3250_legacy
else
	ifeq ($(CHIP),7328)
		UPGMAIN         = $(UPGPF)/7320
	else
		ifeq ($(PLATFORM),97314)
			UPGMAIN         = $(UPGPF)/$(BCM_CHIP_FLAVOR)
		else
			UPGMAIN         = $(UPGPF)/$(BCM_CHIP)
		endif
	endif
endif

ifeq ($(findstring $(PLATFORM),97314),$(PLATFORM))
	VID_CHIP 		= $(VIDCTRL)/$(BCM_CHIP_FLAVOR)
else
	VID_CHIP 		= $(VIDCTRL)/$(BCM_CHIP)
endif

VIDCTRL 		= $(PORTINTF)/videocontrol
VIDMGR 			= $(SETTOP)/videodisplaymgr

ifeq ($(findstring $(PLATFORM),97314),$(PLATFORM))
	VIDMGR_CHIP 	= $(VIDMGR)/$(BCM_CHIP_FLAVOR)
else
	VIDMGR_CHIP 	= $(VIDMGR)/$(BCM_CHIP)
endif

PI_RFMOD  		= $(PORTINTF)/rfmodulator/${BCM_CHIP}
SL_RFMOD       	= $(SYSLIB)/rfmodulatorlib/${BCM_CHIP}

# vbi
PI_CCE 			= $(PORTINTF)/ccecontrol

ifeq ($(findstring $(PLATFORM),97314),$(PLATFORM))
	PI_CCE_CHIP		= $(PORTINTF)/ccecontrol/$(BCM_CHIP_FLAVOR)
else
	PI_CCE_CHIP		= $(PORTINTF)/ccecontrol/$(BCM_CHIP)
endif

ifeq ($(PLATFORM),97314)
	PI_CCD_CHIP		= $(PORTINTF)/ccdcontrol/$(BCM_CHIP_FLAVOR)
else
	PI_CCD_CHIP		= $(PORTINTF)/ccdcontrol/$(BCM_CHIP)
endif

PI_CCD 			= $(PORTINTF)/ccdcontrol

ifeq ($(findstring $(PLATFORM),97314),$(PLATFORM))
	PI_TT_CHIP		= $(PORTINTF)/ttcontrol/$(BCM_CHIP_FLAVOR)
	else
PI_TT_CHIP		= $(PORTINTF)/ttcontrol/$(BCM_CHIP)
endif

PI_TT			= $(PORTINTF)/ttcontrol
SL_TT			= $(SYSLIB)/ttlib
SL_VBI			= $(SYSLIB)/vbilib/$(BCM_CHIP)

VENDRDRV		= $(SETTOP)/vendordrivers
PWRAMP			= $(VENDRDRV)/poweramp
SUPERIO			= ${VENDRDRV}/superio
