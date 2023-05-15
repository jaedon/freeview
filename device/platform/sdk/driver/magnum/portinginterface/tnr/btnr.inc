############################################################
#     Copyright (c) 2003-2012, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: btnr.inc $
# $brcm_Revision: Hydra_Software_Devel/50 $
# $brcm_Date: 10/30/12 4:39p $
#
# Module Description:
#
# Revision History:
#
# Created: 01/09/2007 by Jon Garrett
#
# $brcm_Log: /magnum/portinginterface/tnr/btnr.inc $
# 
# Hydra_Software_Devel/50   10/30/12 4:39p atanugul
# SW7584-54: Add TNR PI support for 7584
# 
# Hydra_Software_Devel/49   9/28/12 3:06p atanugul
# SW3128-228: Add support for BTNR_GetVersionInfo()
# 
# Hydra_Software_Devel/48   9/11/12 11:55a farshidf
# SW7425-3819: Remove symbolic links and flatten out THD directory
# structure
# 
# Hydra_Software_Devel/47   9/7/12 1:45p vishk
# SW7425-3765: Remove symbolic links and flatten out TNR directory
# structure for 3117/3114/3112/3109 frontends.
# 
# Hydra_Software_Devel/46   9/7/12 12:17p farshidf
# SW7425-3765: Remove symbolic links and flatten out TNR directory
# structure
# 
# Hydra_Software_Devel/45   9/7/12 11:06a mward
# SW7425-3765: restructure TNR directory
# 
# Hydra_Software_Devel/44   9/7/12 10:54a atanugul
# SW7425-3765: restructure TNR directory
# 
# Hydra_Software_Devel/43   9/7/12 10:39a farshidf
# SW7425-3765: Remove symbolic links and flatten out TNR directory
# structure
# 
# Hydra_Software_Devel/42   9/7/12 10:33a atanugul
# SW7425-3788: HAB directory restructure
# 
# Hydra_Software_Devel/41   9/7/12 10:02a atanugul
# SW7425-3788: HAB directory restructure
# 
# Hydra_Software_Devel/40   8/29/12 2:50p farshidf
# SW7552-336: re-structure TNR folder to point to AP tnr code
# 
# Hydra_Software_Devel/39   7/19/12 6:10p jasonh
# SW7435-263: Updating with new CHP directory structure.
# 
# Hydra_Software_Devel/38   7/9/12 5:06p atanugul
# SW3472-8: Add PI support for 3472
# 
# Hydra_Software_Devel/37   4/10/12 11:11a atanugul
# SW3462-4: Add support for 3462
# 
# Hydra_Software_Devel/36   2/23/12 12:48p farshidf
# SW3461-120: add the BTNR_J83A_SUPPORt flag for 7552
# 
# Hydra_Software_Devel/35   12/22/11 3:46p farshidf
# SW7552-170: exclude OOB for 7552 A0
# 
# Hydra_Software_Devel/34   12/16/11 12:01p atanugul
# SW3128-92: Add  FW auto detection support
# 
# Hydra_Software_Devel/33   12/9/11 3:07p farshidf
# SW7552-170: add the OOB files
# 
# Hydra_Software_Devel/32   8/30/11 4:31p atanugul
# SW3128-46: add support for BCM3124
# 
# Hydra_Software_Devel/31   8/30/11 4:18p atanugul
# SW3128-47: Add Support for BCM3123
# 
# Hydra_Software_Devel/30   8/30/11 1:02p atanugul
# SW3128-47: Add Support for BCM3123
# 
# Hydra_Software_Devel/29   7/27/11 1:55p farshidf
# SW3461-1: add the new callback file
# 
# Hydra_Software_Devel/28   6/12/11 12:17p farshidf
# SW7552-36: code cleanup
# 
# Hydra_Software_Devel/27   6/1/11 10:14a mward
# SW7125-956:  Provide the IFAGC_OD_MODE on 7125, but on the 97125 and
# 97019 Reference Platforms, that option should be on (y).  Default
# IFAGC_OD_MODE=y (only) for BCHP_CHIP==7125.  Compile with
# IFAGC_OD_MODE=n for platforms using push-pull AGC.
# 
# Hydra_Software_Devel/26   5/27/11 12:38p farshidf
# SW7552-36: add the tuner file
# 
# Hydra_Software_Devel/25   4/11/11 5:50p atanugul
# SW3128-2: Add 3128 AOB TNR Support
# 
# Hydra_Software_Devel/24   3/16/11 4:28p atanugul
# SW3461-3: Add support for BCM3461
# 
# Hydra_Software_Devel/23   3/4/11 3:19p vishk
# SW7422-171: Nexus support for the 3128 (8 ADS core + tuner on daughter
# board) plug on 7422 board
# 
# Hydra_Software_Devel/22   3/2/11 3:59p atanugul
# SW3128-2: Add Support for BCM3128
# 
# Hydra_Software_Devel/21   2/1/11 12:45p atanugul
# SW3128-2: Add Support for BCM3128
# 
# Hydra_Software_Devel/20   8/19/10 5:49p farshidf
# SW7550-440: AGC fix
# 
# Hydra_Software_Devel/TNR_7550_40_12/1   8/18/10 6:09p thayashi
# Added IFAGC_OD_MODE compile option
# 
# Hydra_Software_Devel/18   4/20/10 5:16p farshidf
# SW7550-38: remove btnr_7550_share.c since it is not used
# 
# Hydra_Software_Devel/17   12/18/09 10:54a farshidf
# SW7550-38: add the 3412 path
# 
# Hydra_Software_Devel/16   12/10/09 1:42p farshidf
# SW7550-38: LNA address define
# 
# Hydra_Software_Devel/15   12/10/09 1:37p farshidf
# SW7550-38: fix the inc file 7550
# 
# Hydra_Software_Devel/14   11/17/09 6:40p farshidf
# SW7550-38: add the 3410 path
# 
# Hydra_Software_Devel/13   11/13/09 1:43p farshidf
# SW7550-38: add the 7550 files
# 
# Hydra_Software_Devel/12   11/9/09 5:56p nitinb
# SW7550-38: Removed blank spaces that caused 7550 build break
# 
# Hydra_Software_Devel/11   11/6/09 7:00p farshidf
# SW7550-38: add the tuner files
# 
# Hydra_Software_Devel/10   6/17/09 11:18a vishk
# PR 46431: Add support for BCM3117, BCM3114, and BCM3112
# 
# Hydra_Software_Devel/10   6/17/09 11:18a vishk
# PR 46431: Add support for BCM3117, BCM3114, and BCM3112
# 
# Hydra_Software_Devel/9   3/11/09 4:15p vishk
# PR 46431: Add support for BCM3117, BCM3114, and BCM3112
# 
# Hydra_Software_Devel/8   3/11/09 2:38p vishk
# PR 46431: Add support for BCM3117, BCM3114, and BCM3112
# 
# Hydra_Software_Devel/7   3/4/09 8:50p vishk
# PR46431: Add support for BCM3117, BCM3114, and BCM3112
# 
# Hydra_Software_Devel/6   1/16/09 7:39p vishk
# PR46436: AOB PI: Add support for BCM3117 and BCM3114
# 
# Hydra_Software_Devel/5   5/1/08 11:16a vishk
# PR 39716: TNR PI support for DTT76800.
# 
# Hydra_Software_Devel/4   3/24/08 6:42p vishk
# PR 39710: To add support for Thompson DTT75409 TNR PI. for 93556
# reference board.
# 
# Hydra_Software_Devel/3   5/16/07 12:12p erickson
# PR29024: default 3420 off
# 
# Hydra_Software_Devel/2   5/12/07 4:04p dlwin
# PR 29024: Added 3420 PI.
# 
# Hydra_Software_Devel/1   1/11/07 2:39p jgarrett
# PR27004: Adding new .inc files
# 
############################################################

#
# Conditional Options for this Makefile
#
# BTNR_3250OB_SUPPORT - If defined, support will be added for the 3250 OOB tuner
# BTNR_3418_SUPPORT - If defined, support will be added for the 3418 tuner
# BTNR_3420_SUPPORT - If defined, support will be added for the 3420 IB tuner
# BTNR_DTT760X_SUPPORT - If defined, support will be added for the DTT760x tuner
# BTNR_DTT76800_SUPPORT - If defined, support will be added for the DTT76800 tuner
# BTNR_DTT75409_SUPPORT - If defined, support will be added for the DTT75409 tuner
# BTNR_3255IB_SUPPORT - If defined, support will be added for the 3255 IB tuner
# BTNR_3255OB_SUPPORT - If defined, support will be added for the 3255 OOB tuner
# BTNR_3117IB_SUPPORT - If defined, support will be added for the 3117 IB tuner
#

# Define this module
MAGNUM_MODULES += BTNR

# Module Includes
BTNR_INCLUDES += $(MAGNUM)/portinginterface/tnr/include/common
BTNR_INCLUDES += $(MAGNUM)/portinginterface/tnr/src/common
BTNR_INCLUDES += $(MAGNUM)/commonutils/fec/include

# Module Sources
BTNR_SOURCES += $(MAGNUM)/portinginterface/tnr/src/common/btnr.c

# Conditional Sources
ifneq ($(BTNR_3250OB_SUPPORT),)
BTNR_INCLUDES += $(MAGNUM)/portinginterface/tnr/include/3250ob/

BTNR_SOURCES += $(MAGNUM)/portinginterface/tnr/src/docsis/3250ob/btnr_3250ob.c
endif

ifneq ($(BTNR_3418_SUPPORT),)
BTNR_INCLUDES += $(MAGNUM)/portinginterface/tnr/src/standalone/3418/

BTNR_SOURCES += $(MAGNUM)/portinginterface/tnr/src/standalone/3418/btnr_3418.c
BTNR_SOURCES += $(MAGNUM)/portinginterface/tnr/src/standalone/3418/btnr_3418_regio.c
BTNR_SOURCES += $(MAGNUM)/portinginterface/tnr/src/standalone/3418/btnr_3418_smarttune.c
endif

ifneq ($(BTNR_3420_SUPPORT),)
BTNR_INCLUDES += $(MAGNUM)/portinginterface/tnr/src/standalone/3420/

BTNR_SOURCES += $(MAGNUM)/portinginterface/tnr/src/standalone/3420/btnr_3420.c
BTNR_SOURCES += $(MAGNUM)/portinginterface/tnr/src/standalone/3420/btnr_3420_priv.c
endif

ifneq ($(BTNR_DTT760X_SUPPORT),)
BTNR_INCLUDES += $(MAGNUM)/portinginterface/tnr/src/standalone/dtt760x/

BTNR_SOURCES += $(MAGNUM)/portinginterface/tnr/src/standalone/dtt760x/btnr_dtt760x.c
endif

ifneq ($(BTNR_DTT76800_SUPPORT),)
BTNR_INCLUDES += $(MAGNUM)/portinginterface/tnr/src/standalone/dtt76800/

BTNR_SOURCES += $(MAGNUM)/portinginterface/tnr/src/standalone/dtt76800/btnr_dtt76800.c
endif


ifneq ($(BTNR_DTT75409_SUPPORT),)
BTNR_INCLUDES += $(MAGNUM)/portinginterface/tnr/src/standalone/dtt75409/

BTNR_SOURCES += $(MAGNUM)/portinginterface/tnr/src/standalone/dtt75409/btnr_dtt75409.c
endif

ifneq ($(BTNR_3255IB_SUPPORT),)
BTNR_INCLUDES += $(MAGNUM)/portinginterface/tnr/include/3255ib/

BTNR_SOURCES += \
    $(MAGNUM)/portinginterface/tnr/src/docsis/3255ib/btnr_3255ib.c
endif

ifneq ($(BTNR_3255OB_SUPPORT),)
BTNR_INCLUDES += $(MAGNUM)/portinginterface/tnr/include/3255ob/

BTNR_SOURCES += \
    $(MAGNUM)/portinginterface/tnr/src/docsis/3255ob/btnr_3255ob.c
endif

ifneq ($(BTNR_3117IB_SUPPORT),)
BTNR_31xx_CHIP := 3117ib
BTNR_INCLUDES += $(MAGNUM)/portinginterface/tnr/include/3117ib/
BTNR_SOURCES += $(MAGNUM)/portinginterface/tnr/src/8051/3117ib/btnr_3117ib.c
endif

ifneq ($(BTNR_3112IB_SUPPORT),)
BTNR_31xx_CHIP := 3112ib
BTNR_INCLUDES += $(MAGNUM)/portinginterface/tnr/include/3112ib/
BTNR_SOURCES += $(MAGNUM)/portinginterface/tnr/src/8051/3112ib/btnr_3112ib.c
endif

ifneq ($(BTNR_3109IB_SUPPORT),)
BTNR_31xx_CHIP := 3109ib
BTNR_INCLUDES += $(MAGNUM)/portinginterface/tnr/include/3109ib/
BTNR_SOURCES += $(MAGNUM)/portinginterface/tnr/src/8051/3109ib/btnr_3109ib.c
endif

ifneq ($(BTNR_3114IB_SUPPORT),)
BTNR_31xx_CHIP := 3114ib
BTNR_INCLUDES += $(MAGNUM)/portinginterface/tnr/include/3114ib/
BTNR_SOURCES += $(MAGNUM)/portinginterface/tnr/src/8051/3114ib/btnr_3114ib.c
endif

ifneq ($(BTNR_3128IB_SUPPORT),)
BTNR_312X_CHIP := 3128ib
BTNR_DEFINES += BTNR_3128_SUPPORT
BTNR_INCLUDES += $(MAGNUM)/portinginterface/tnr/include/3128ib/
BTNR_SOURCES += $(MAGNUM)/portinginterface/tnr/src/leapwideband/3128ib/btnr_3128ib.c
endif

ifneq ($(BTNR_7584IB_SUPPORT),)
BTNR_7584_CHIP := 7584ib
BTNR_DEFINES += BTNR_7584_SUPPORT
BTNR_INCLUDES += $(MAGNUM)/portinginterface/tnr/include/7584ib/
BTNR_SOURCES += $(MAGNUM)/portinginterface/tnr/src/leapwideband/7584ib/btnr_7584ib.c
BTNR_SOURCES += $(MAGNUM)/portinginterface/tnr/src/leapwideband/7584ib/btnr_7584ib_priv.c
endif

ifneq ($(BTNR_3461_SUPPORT),)
BTNR_INCLUDES += $(MAGNUM)/portinginterface/tnr/include/3461ib/
BTNR_SOURCES += $(MAGNUM)/portinginterface/tnr/src/leapnarrowband/3461ib/btnr_3461ib.c
BTNR_SOURCES += $(MAGNUM)/portinginterface/tnr/src/leapnarrowband/3461ib/btnr_3461ib_priv.c
endif

ifneq ($(BTNR_3462_SUPPORT),)
BTNR_INCLUDES += $(MAGNUM)/portinginterface/tnr/include/3462ib/
BTNR_INCLUDES += $(MAGNUM)/portinginterface/tnr/src/leapnarrowband
BTNR_SOURCES += $(MAGNUM)/portinginterface/tnr/src/leapnarrowband/3462ib/btnr_3462ib.c
BTNR_SOURCES += $(MAGNUM)/portinginterface/tnr/src/leapnarrowband/btnr_34xxib_priv.c
endif

ifneq ($(BTNR_3472_SUPPORT),)
BTNR_INCLUDES += $(MAGNUM)/portinginterface/tnr/include/3472ib/
BTNR_INCLUDES += $(MAGNUM)/portinginterface/tnr/src/leapnarrowband
BTNR_SOURCES += $(MAGNUM)/portinginterface/tnr/src/leapnarrowband/3472ib/btnr_3472ib.c
BTNR_SOURCES += $(MAGNUM)/portinginterface/tnr/src/leapnarrowband/btnr_34xxib_priv.c
endif

ifneq ($(BTNR_31xx_CHIP),)
BTNR_INCLUDES += $(MAGNUM)/portinginterface/tnr/src/8051/
BTNR_SOURCES += $(MAGNUM)/portinginterface/tnr/src/8051/btnr_31xxib_priv.c
endif

ifneq ($(BTNR_312X_CHIP),)
BTNR_SOURCES += $(MAGNUM)/portinginterface/tnr/src/leapwideband/$(BTNR_312X_CHIP)/btnr_312xib_priv.c
endif

ifneq ($(BTNR_3117OB_SUPPORT),)
BTNR_31xx_CHIP_OB := 3117ob
BTNR_INCLUDES += $(MAGNUM)/portinginterface/tnr/include/3117ob/
BTNR_SOURCES += $(MAGNUM)/portinginterface/tnr/src/8051/3117ob/btnr_3117ob.c
endif

ifneq ($(BTNR_3114OB_SUPPORT),)
BTNR_31xx_CHIP_OB := 3114ob
BTNR_INCLUDES += $(MAGNUM)/portinginterface/tnr/include/3114ob/
BTNR_SOURCES += $(MAGNUM)/portinginterface/tnr/src/8051/3114ob/btnr_3114ob.c
endif

ifneq ($(BTNR_3128OB_SUPPORT),)
BTNR_312X_CHIP_OB := 3128ob
BTNR_DEFINES += BTNR_3128_SUPPORT
BTNR_INCLUDES += $(MAGNUM)/portinginterface/tnr/include/3128ob/
BTNR_SOURCES += $(MAGNUM)/portinginterface/tnr/src/leapwideband/3128ob/btnr_3128ob.c
endif

ifneq ($(BTNR_7584OB_SUPPORT),)
BTNR_7584_CHIP_OB := 7584ob
BTNR_DEFINES += BTNR_7584_SUPPORT
BTNR_INCLUDES += $(MAGNUM)/portinginterface/tnr/include/7584ob/
BTNR_SOURCES += $(MAGNUM)/portinginterface/tnr/src/leapwideband/7584ob/btnr_7584ob.c
BTNR_SOURCES += $(MAGNUM)/portinginterface/tnr/src/leapwideband/7584ob/btnr_7584ob_priv.c
endif

ifneq ($(BTNR_31xx_CHIP_OB),)
BTNR_INCLUDES += $(MAGNUM)/portinginterface/tnr/src/8051/
BTNR_SOURCES += $(MAGNUM)/portinginterface/tnr/src/8051/btnr_31xxob_priv.c
endif

ifneq ($(BTNR_312X_CHIP_OB),)
BTNR_SOURCES += $(MAGNUM)/portinginterface/tnr/src/leapwideband/$(BTNR_312X_CHIP_OB)/btnr_312xob_priv.c
endif

ifneq ($(BTNR_7550_SUPPORT),)
BTNR_INCLUDES += $(MAGNUM)/portinginterface/tnr/include/7550ib
BTNR_INCLUDES += $(MAGNUM)/basemodules/chp/include/3412/rdb/a0
BTNR_INCLUDES += $(MAGNUM)/portinginterface/tnr/src/soc/7550ib
BTNR_SOURCES += $(MAGNUM)/portinginterface/tnr/src/soc/7550ib/btnr_7550ib.c \
				$(MAGNUM)/portinginterface/tnr/src/soc/7550ib/btnr_75xxib_priv.c \
				$(MAGNUM)/portinginterface/tnr/src/soc/7550ib/btnr_7550ib_lna.c \
				$(MAGNUM)/portinginterface/tnr/src/soc/7550ib/btnr_7550ib_status.c \
				$(MAGNUM)/portinginterface/tnr/src/soc/7550ib/btnr_7550ib_tune.c \
				$(MAGNUM)/portinginterface/tnr/src/soc/7550ib/btnr_7550ib_utils.c
endif

ifeq ($(REF_BOARD_LNA), y)
BTNR_DEFINES += REF_7550_BOARD
endif

ifneq ($(BTNR_7125_SUPPORT),)
BTNR_INCLUDES += $(MAGNUM)/portinginterface/tnr/include/7125
BTNR_INCLUDES += $(MAGNUM)/basemodules/chp/include/3412/rdb/a0
BTNR_SOURCES += $(MAGNUM)/portinginterface/tnr/src/soc/7125ib/btnr_7125ib.c \
				$(MAGNUM)/portinginterface/tnr/src/soc/7125ib/btnr_7125ib_priv.c \
				$(MAGNUM)/portinginterface/tnr/src/soc/7125ib/btnr_7125_lna.c \
				$(MAGNUM)/portinginterface/tnr/src/soc/7125ib/btnr_7125_status.c \
				$(MAGNUM)/portinginterface/tnr/src/soc/7125ib/btnr_7125_tune.c \
				$(MAGNUM)/portinginterface/tnr/src/soc/7125ib/btnr_7125_utils.c
ifneq ($(IFAGC_OD_MODE), n)
BTNR_DEFINES += IFAGC_OD_MODE
endif
else
ifeq ($(IFAGC_OD_MODE), y)
BTNR_DEFINES += IFAGC_OD_MODE
endif
endif


ifneq ($(BTNR_7552_SUPPORT),)
BTNR_INCLUDES += $(MAGNUM)/portinginterface/tnr/include/3x7xib
BTNR_INCLUDES += $(MAGNUM)/portinginterface/tnr/src/soc
BTNR_INCLUDES += $(MAGNUM)/portinginterface/tnr/src/soc/3x7xib
BTNR_SOURCES += $(MAGNUM)/portinginterface/tnr/src/soc/3x7xib/btnr_3x7x.c \
				$(MAGNUM)/portinginterface/tnr/src/soc/btnr_3x7xib_priv.c \
				$(MAGNUM)/portinginterface/tnr/src/soc/3x7xib/btnr_init.c \
				$(MAGNUM)/portinginterface/tnr/src/soc/3x7xib/btnr_tune.c \
				$(MAGNUM)/portinginterface/tnr/src/soc/3x7xib/btnr_tune_ofdm.c \
				$(MAGNUM)/portinginterface/tnr/src/soc/3x7xib/btnr_tune_ads.c \
				$(MAGNUM)/portinginterface/tnr/src/soc/3x7xib/btnr_callback.c
ifeq ($(BTNR_7552_J83A_SUPPORT), y)
BTNR_DEFINES += BTNR_J83A_SUPPORT
endif
ifeq ($(BCHP_VER),B0)
BTNR_INCLUDES += $(MAGNUM)/portinginterface/tnr/include/3x7xob
BTNR_INCLUDES += $(MAGNUM)/portinginterface/tnr/src/soc/3x7xob
BTNR_SOURCES += $(MAGNUM)/portinginterface/tnr/src/soc/3x7xob/btnr_ob_3x7x.c \
				$(MAGNUM)/portinginterface/tnr/src/soc/3x7xob/btnr_3x7xob_priv.c \
				$(MAGNUM)/portinginterface/tnr/src/soc/3x7xob/btnr_ob_init.c \
				$(MAGNUM)/portinginterface/tnr/src/soc/3x7xob/btnr_ob_tune.c 
endif
endif




