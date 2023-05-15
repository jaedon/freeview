#/***************************************************************************
# *     (c)2007-2011 Broadcom Corporation
# *
# *  This program is the proprietary software of Broadcom Corporation and/or its licensors,
# *  and may only be used, duplicated, modified or distributed pursuant to the terms and
# *  conditions of a separate, written license agreement executed between you and Broadcom
# *  (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
# *  no license (express or implied), right to use, or waiver of any kind with respect to the
# *  Software, and Broadcom expressly reserves all rights in and to the Software and all
# *  intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
# *  HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
# *  NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
# *
# *  Except as expressly set forth in the Authorized License,
# *
# *  1.     This program, including its structure, sequence and organization, constitutes the valuable trade
# *  secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
# *  and to use this information only in connection with your use of Broadcom integrated circuit products.
# *
# *  2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
# *  AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
# *  WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
# *  THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
# *  OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
# *  LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
# *  OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
# *  USE OR PERFORMANCE OF THE SOFTWARE.
# *
# *  3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
# *  LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
# *  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
# *  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
# *  THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
# *  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
# *  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
# *  ANY LIMITED REMEDY.
# *
# * $brcm_Workfile:  $
# * $brcm_Revision:  $
# * $brcm_Date:  $
# *
# * Module Description:
# *
# * Revision History:
# *
# * $brcm_Log:  $
# *
# ******************************************************************************/

# add the extra for specific customer mode only



ifeq ($(BHSM_CUST),NAGRA)

#$(warning  ..........bhsm_nagra.inc is entered with BHSM_CUST= $(BHSM_CUST) )

# specific features
#BHSM_AV_CMD_FEATURE=1
#BHSM_REG_VERIFY_FEATURE=1
#HSM_SCRAMBLE_SDRAM_FEATURE=1
#BHSM_CHALLENGE_RESPONSE_FEATURE=1

#  common units in all kinds of customer modes, covered in bhsm.inc already
BHSM_OTPMSP_FEATURE=1
BHSM_KEY_LADDER_FEATURE=1
BHSM_OTPMSP=ON
BHSM_KEYLADDER=ON

# Specific Units
#BHSM_IP_LICENSING=ON

# to enable testApp specific units to build
BHSM_CUST_NAGRA=1

CFLAGS += -DBSP_NAGRA
CFLAGS += -DBHSM_AEGIS_SUPPORT

BHSM_DEFINES += BSP_NAGRA


# specific shared header files, no units, in bhsm.inc already
#BHSM_INCLUDES += portinginterface/hsm/${BCHP_CHIP}/${BCHP_VER_LOWER}/cust_nagra \
#		  portinginterface/hsm/${BCHP_CHIP}/${BCHP_VER_LOWER}/cust_nagra/aegis


endif

#####################################################

#####################################################

ifdef BHSM_REG_VERIFY_FEATURE
BHSM_INCLUDES += 	$(MAGNUM)/portinginterface/hsm/src/${CORETECH}/region_verify \
			$(MAGNUM)/portinginterface/hsm/src/${CORETECH}/region_verify/aegis
	
BHSM_SOURCES += 	$(MAGNUM)/portinginterface/hsm/src/${CORETECH}/region_verify/bhsm_verify_reg.c
endif	

#No longer supported in 40-nm
#ifdef BHSM_CHALLENGE_RESPONSE_FEATURE
#BHSM_INCLUDES += $(MAGNUM)/portinginterface/hsm/src/${CORETECH}/challenge_response
#BHSM_INCLUDES += $(MAGNUM)/portinginterface/hsm/src/${CORETECH}/challenge_response/aegis
#
#BHSM_SOURCES += $(MAGNUM)/portinginterface/hsm/src/${CORETECH}/challenge_response/bhsm_challenge_response.c
#         $(warning  ..........BHSM_CHALLENGE_RESPONSE_FEATURE is built in  )
#endif

#No longer supported in 40-nm
#ifdef BHSM_SCRAMBLE_SDRAM_FEATURE
#BHSM_INCLUDES += 	$(MAGNUM)/portinginterface/hsm/src/${CORETECH}/sdram_scrambler \
#			$(MAGNUM)/portinginterface/hsm/src/${CORETECH}/sdram_scrambler/aegis
#	
#BHSM_SOURCES += 	$(MAGNUM)/portinginterface/hsm/src/${CORETECH}/sdram_scrambler/bhsm_scramble_sdram.c
#endif


ifdef BHSM_AV_CMD_FEATURE
BHSM_INCLUDES += 	$(MAGNUM)/portinginterface/hsm/src/${CORETECH}/av_cmd \
			$(MAGNUM)/portinginterface/hsm/src/${CORETECH}/av_cmd/aegis
	
BHSM_SOURCES += 	$(MAGNUM)/portinginterface/hsm/src/${CORETECH}/av_cmd/bhsm_avcmd.c
endif


