###########################################################################
#	   (c)2007-2011 Broadcom Corporation
#
# This program is the proprietary software of Broadcom Corporation and/or its licensors,
# and may only be used, duplicated, modified or distributed pursuant to the terms and
# conditions of a separate, written license agreement executed between you and Broadcom
# (an "Authorized License").	Except as set forth in an Authorized License, Broadcom grants
# no license (express or implied), right to use, or waiver of any kind with respect to the
# Software, and Broadcom expressly reserves all rights in and to the Software and all
# intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
# HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
# NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
#
# Except as expressly set forth in the Authorized License,
#
# 1.	   This program, including its structure, sequence and organization, constitutes the valuable trade
# secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
# and to use this information only in connection with your use of Broadcom integrated circuit products.
#
#	2.	   TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
# AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
# WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
# THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
# OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
# LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
# OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
# USE OR PERFORMANCE OF THE SOFTWARE.
#
# 3.	   TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
# LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
# EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
# USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
# THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
# ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
# LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
# ANY LIMITED REMEDY.
#
# $brcm_Workfile: bcec.inc $
# $brcm_Revision: Hydra_Software_Devel/4 $
# $brcm_Date: 9/6/12 5:40p $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /magnum/portinginterface/cec/bcec.inc $
# 
# Hydra_Software_Devel/4   9/6/12 5:40p vle
# SW7425-3763: Remove symlinks from Magnum CEC modules
# 
# Hydra_Software_Devel/SW7425-3763/1   8/31/12 12:11p vle
# SW7425-3763: Remove symlinks from magnum CEC PI
# 
# Hydra_Software_Devel/3   8/2/12 3:16p rgreen
# SW7429-204: Always enable CEC compilation support
# 
# Hydra_Software_Devel/2   12/19/11 12:34p vle
# SW7425-1140: Fix build issue without CEC support. CEC functionalities
# in NEXUS_HDMI_OUTPUT modules is enabled with build option
# BHDM_CEC_SUPPORT=y
# 
# Hydra_Software_Devel/1   11/22/11 6:24p vle
# SW7425-1140: Add BCEC PI support
# 
# Hydra_Software_Devel/SW7425-1140/2   11/22/11 2:01p vle
# SW7425-1140: Make sure CEC is also enablee if the old BHDM_CEC_SUPPORT
# flag is used.
# 
# Hydra_Software_Devel/SW7425-1140/1   11/16/11 12:09p vle
# SW7425-1140: Add CEC PI
# 
###########################################################################

#
# Conditional Options for this Makefile
#
# BCEC_SUPPORT - If defined, this will include CEC support.
#

# Define this module
MAGNUM_MODULES += BCEC

# Module Includes
BCEC_INCLUDES += $(MAGNUM)/portinginterface/cec/include \
 		 $(MAGNUM)/portinginterface/cec/src/common 

# Module Sources
BCEC_SOURCES += \
	$(MAGNUM)/portinginterface/cec/src/common/bcec.c 


#################################
#### CEC core specific source ###
#################################
ifeq ($(filter $(BCHP_CHIP), 7125 7208 7325 7335 7336 7340 7342 7358 7400 7405 7408 7420 7468 7550), $(BCHP_CHIP))
BCEC_SOURCES +=	$(MAGNUM)/portinginterface/cec/src/singleinterrupt/bcec_priv.c

else
BCEC_SOURCES +=	$(MAGNUM)/portinginterface/cec/src/dualinterrupt/bcec_priv.c
endif


BCEC_DEFINES += BCEC_SUPPORT	


