############################################################
#     Copyright (c) 2003-2010, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: bcm97456.inc $
# $brcm_Revision: 19 $
# $brcm_Date: 3/16/10 4:36p $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /BSEAV/api/build/magnum/bcm97456.inc $
# 
# 19   3/16/10 4:36p mward
# SW7400-2517:  Maintain REFSW_XVD_7400_SINGLEDECODE support.  Set
# B_MAX_MOSAIC_DECODES=0 at compile time if B_XVD_SINGLEDECODE=y.
# REFSW_PREREL_97400_SINGLE is deprecated, use REFSW_PREREL_97400.
# 
# 18   3/13/09 5:37p mward
# PR49827: PR52490: Change name of SRS VIQ to SRS TrueVolume
# PR49827, PR50613: Settings for Dolby Volume and SRS Volume IQ.
# 
# PR49827/2   2/25/09 12:17p mward
# PR52490: Change name of SRS VIQ to SRS TrueVolume
# 
# PR49827/1   1/13/09 5:11p mward
# PR49827, PR50613: Settings for Dolby Volume and SRS Volume IQ.
# 
# 17   11/4/08 4:14p mward
# PR44957: Pass configured RAP formats to Settop API, too.
# 
# 16   8/11/08 5:39p mward
# PR39988: For 7400, select DDP pass-thru support at run-time, switching
# use of RAP channels if necessary.  Merge to main.
# 
# PR39988/1   6/19/08 2:28p mward
# PR39988: Detect dual output / DDP pass for 7400 at runtime.
# 
# 15   11/28/07 12:52p mward
# PR37117: 7400 Platforms now use UNIFIED_ITB_SUPPORT unconditionally.
# 
# 14   8/29/07 6:54p mward
# PR31736:  Default on UNIFIED_ITB_SUPPORT for 97456.
# 
# 13   6/7/07 6:49p vle
# PR 31924: Define HDMI Tx Support for platform
# 
# 12   5/11/07 2:45p anilmm
# PR28294:  Enable UHF
# 
# 11   4/27/07 3:21p anilmm
# PR28294:  Enable smartcard for 97456B0
# 
# 10   4/4/07 12:31p mward
# PR29446: Use B_HAS_3255=n build option instead of B_NO_3255=y for more
# clarity and consistency.
# 
# 9   4/3/07 4:55p mward
# PR29446: Create B_NO_3255=y build option to test boards with 3255
# without using it
# 
# 8   3/9/07 4:55p anilmm
# PR28294: Enable HDMI and Playpump_IP for 97456B0
# 
# 7   3/1/07 2:58p haisongw
# PR28294:disabling UHF, SCDand HDMI for 97456b0 (temporary)
# 
# 7   3/1/07 2:56p haisongw
# PR28294: temporarily disable some stuff for 97456b0
# 
# 6   2/8/07 6:01p haisongw
# PR27767: enable SMARTCARD support for 97456 platform
#
# 5   10/10/06 4:40p haisongw
# PR24845: add B_HAS_OOB for non-oob support
#
# 4   9/5/06 3:32p jackli
# PR24025: add kernel mode settop api support for 97456
#
# 3   8/28/06 1:09p haisongw
# PR 22368: Add AOB, AUS and TNR3255ob support
#
# 2   7/10/06 4:01p haisongw
# PR 22368: Add AOB, AUS and TNR3255ob support
#
# 1   4/27/06 5:53p haisongw
# PR 20984: add 97456 platform support
#
############################################################

#CFLAGS += -DBMEM_SAFETY_CONFIG=BMEM_CONFIG_FASTEST
ifneq ($(subst N,n,${B_HAS_3255}),n)
include  ${ROCKFORD}/modules/3255/ads.inc
include  ${ROCKFORD}/modules/tnr.inc
include  ${ROCKFORD}/modules/3255/tnr3255ib.inc
include  ${ROCKFORD}/modules/rpc.inc

SRCS += bsettop_tuner_3255.c
CFLAGS += -DB_HAS_3255=1

OOB_SUPPORT ?=y
ifeq ($(OOB_SUPPORT),y)
CFLAGS += -DB_HAS_OOB=1
include  ${ROCKFORD}/modules/3255/aob.inc
include  ${ROCKFORD}/modules/3255/aus.inc
include  ${ROCKFORD}/modules/3255/tnr3255ob.inc
endif
endif


ifeq (${SYSTEM},linux)
# Needs a special version of a kernel, don't enable by default
# B_HAS_PLAYPUMP_IP ?= y
endif
ifeq (${B_HAS_PLAYPUMP_IP},y)
CFLAGS += -DB_HAS_PLAYPUMP_IP
CFLAGS += ${BNETIF_DMA_CFLAGS}
endif

B_HAS_SMARTCARD=y

include ${ROCKFORD}/modules/uhf.inc	

B_HAS_PCM=y
HDMI_TX_SUPPORT=y

ifeq ($(B_PASSTHRU_DDP_SUPPORT),)
CFLAGS += -DB_PASSTHRU_DDP_SUPPORT=1 -DRAP_I2S_COMPRESS_SUPPORT
endif

#By default disable AC3+ support 
ifneq ($(subst Y,y,${RAP_DDP_SUPPORT}),y)
CFLAGS += -DB_RAP_NO_AC3PLUS_SUPPORT
endif

#By default disable AC3 support 
ifneq ($(subst Y,y,${RAP_AC3_SUPPORT}),y)
CFLAGS += -DB_RAP_NO_AC3_SUPPORT
endif


#By default disable WMA support 
ifneq ($(subst Y,y,${RAP_WMA_SUPPORT}),y)
CFLAGS += -DB_RAP_NO_WMA_SUPPORT
endif

#Dolby Volume and SRS Volume IQ are mutually exclusive
ifeq ($(subst Y,y,${RAP_DOLBYVOLUME_SUPPORT}),y)
ifeq ($(subst Y,y,${RAP_SRSTRUVOL_SUPPORT}),y)
$(error Only one of RAP_DOLBYVOLUME_SUPPORT or RAP_SRSTRUVOL_SUPPORT should be "y")
else
CFLAGS += -DRAP_DOLBYVOLUME_SUPPORT
endif
endif

ifeq ($(subst Y,y,${RAP_SRSTRUVOL_SUPPORT}),y)
ifeq ($(subst Y,y,${RAP_DOLBYVOLUME_SUPPORT}),y)
$(error Only one of RAP_DOLBYVOLUME_SUPPORT or RAP_SRSTRUVOL_SUPPORT should be "y")
else
CFLAGS += -DRAP_SRSTRUVOL_SUPPORT
endif
endif

ifneq ($(subst Y,y,${B_XVD_SINGLEDECODE}),y)
CFLAGS += -DB_MAX_MOSAIC_DECODES=12
else
CFLAGS += -DB_MAX_MOSAIC_DECODES=0
endif


