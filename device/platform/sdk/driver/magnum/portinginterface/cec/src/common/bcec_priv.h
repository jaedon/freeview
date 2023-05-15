/***************************************************************************
*	   (c)2007-2011 Broadcom Corporation
*
* This program is the proprietary software of Broadcom Corporation and/or its licensors,
* and may only be used, duplicated, modified or distributed pursuant to the terms and
* conditions of a separate, written license agreement executed between you and Broadcom
* (an "Authorized License").	Except as set forth in an Authorized License, Broadcom grants
* no license (express or implied), right to use, or waiver of any kind with respect to the
* Software, and Broadcom expressly reserves all rights in and to the Software and all
* intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
* HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
* NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
*
* Except as expressly set forth in the Authorized License,
*
* 1.	   This program, including its structure, sequence and organization, constitutes the valuable trade
* secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
* and to use this information only in connection with your use of Broadcom integrated circuit products.
*
*	2.	   TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
* AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
* WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
* THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
* OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
* LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
* OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
* USE OR PERFORMANCE OF THE SOFTWARE.
*
* 3.	   TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
* LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
* EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
* USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
* THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
* ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
* LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
* ANY LIMITED REMEDY.
*
* $brcm_Workfile: bcec_priv.h $
* $brcm_Revision: Hydra_Software_Devel/10 $
* $brcm_Date: 10/4/12 12:52p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /magnum/portinginterface/cec/src/common/bcec_priv.h $
* 
* Hydra_Software_Devel/10   10/4/12 12:52p vle
* SW7445-51: Add support for 7445
* 
* Hydra_Software_Devel/9   8/9/12 11:04p rgreen
* SW7429-204: No Clear/Mask wake-up event functions for 65nm
* 
* Hydra_Software_Devel/8   8/7/12 2:44p rgreen
* SW7429-204: Create private Clear/Mask wake-up event functions to
* support both 65 and 40nm; Update chip macros to support both platforms
* (Fix checkin comments)
* 
* Hydra_Software_Devel/6   8/2/12 3:16p rgreen
* SW7429-204: Move RDB register definitions to private header file
* 
* Hydra_Software_Devel/5   2/28/12 10:57a vle
* SW7425-2289, SW7420-2256: Add more debug messages, fix compiler
* warnings for 65nm platforms
* 
* Hydra_Software_Devel/4   2/24/12 4:06p vle
* SW7425-2289: Merge to mainline
* 
* Hydra_Software_Devel/SW7425-2289/1   2/22/12 6:11p vle
* SW7425-2289: Add support for CEC AutoOn
* 
* Hydra_Software_Devel/3   2/17/12 3:44p vle
* SW7425-2279: Merge to mainline
* 
* Hydra_Software_Devel/SW7425-2279/4   2/16/12 4:55p vle
* SW7425-2279: Additional updates after 2nd review. Still need to rework
* Standby/Resume function to use CEC autoOn feature.
* 
* Hydra_Software_Devel/SW7425-2279/3   2/13/12 3:33p vle
* SW7420-682: Merge to SW7425-2279 branch
* 
* Hydra_Software_Devel/SW7425-2279/2   2/9/12 3:35p vle
* SW7425-2279: Update BCEC PI implementations. Use separate events for
* transmit and receive.
* 
* Hydra_Software_Devel/SW7425-2279/1   1/30/12 7:14p vle
* SW7425-2279: Fix incorrect return of received CEC message
* 
* Hydra_Software_Devel/SW7420-682/1   2/3/12 6:42p vle
* SW7420-682: First attempt at enable Auto-on CEC. Code hasn't gone
* through testing.
* 
* Hydra_Software_Devel/2   1/17/12 6:46p vle
* SW7125-1214: Fix crash issue duing initialization in proxy mode.
* 
* Hydra_Software_Devel/1   11/22/11 6:23p vle
* SW7425-1140: Add BCEC PI support
* 
* Hydra_Software_Devel/SW7425-1140/1   11/16/11 12:14p vle
* SW7425-1140: Add CEC PI support for 7425
* 
***************************************************************************/
#ifndef BCEC_PRIV_H__
#define BCEC_PRIV_H__

#include "bcec.h"

#ifdef __cplusplus
extern "C" {
#endif


#if BCEC_CONFIG_40NM_SUPPORT \
 || BCEC_CONFIG_28NM_SUPPORT
#include "bchp_aon_hdmi_tx.h"
#include "bchp_hdmi_tx_intr2.h"
#include "bchp_int_id_hdmi_tx_intr2.h"
#include "bchp_int_id_aon_pm_l2.h"
#include "bchp_int_id_aon_l2.h"

#elif BCEC_CONFIG_65NM_SUPPORT
#include "bchp_hdmi_intr2.h"
#include "bchp_int_id_hdmi_intr2.h"
#include "bchp_hdmi_tx_phy.h"

#else
#include "bchp_hdmi_intr2.h"
#include "bchp_int_id_hdmi_intr2.h"
#include "bchp_hdmi_tx_phy.h"
#endif

#if BCEC_CONFIG_HAS_HDMI_RX
#include "bchp_aon_hdmi_rx.h"
#endif


#if BCEC_CONFIG_40NM_SUPPORT || BCEC_CONFIG_28NM_SUPPORT
#define BCHP_MASK_DVP(Register,Field) BCHP_AON_HDMI_TX_##Register##_##Field##_MASK

#define BCHP_MASK_CEC_CNTRL(Register,Field) BCHP_AON_HDMI_TX_##Register##_##Field##_MASK

#define REGADDR_CEC_CNTRL_1 BCHP_AON_HDMI_TX_CEC_CNTRL_1 
#define REGNAME_CEC_CNTRL_1 AON_HDMI_TX_CEC_CNTRL_1 

#define REGADDR_CEC_CNTRL_2 BCHP_AON_HDMI_TX_CEC_CNTRL_2
#define REGNAME_CEC_CNTRL_2 AON_HDMI_TX_CEC_CNTRL_2

#define REGADDR_CEC_CNTRL_3 BCHP_AON_HDMI_TX_CEC_CNTRL_3
#define REGNAME_CEC_CNTRL_3 AON_HDMI_TX_CEC_CNTRL_3

#define REGADDR_CEC_CNTRL_4 BCHP_AON_HDMI_TX_CEC_CNTRL_4
#define REGNAME_CEC_CNTRL_4 AON_HDMI_TX_CEC_CNTRL_4

#define REGADDR_CEC_CNTRL_5 BCHP_AON_HDMI_TX_CEC_CNTRL_5
#define REGNAME_CEC_CNTRL_5 AON_HDMI_TX_CEC_CNTRL_5

#define REGADDR_CEC_CNTRL_6 BCHP_AON_HDMI_TX_CEC_CNTRL_6
#define REGNAME_CEC_CNTRL_6 AON_HDMI_TX_CEC_CNTRL_6

#define REGADDR_CEC_TX_DATA_1 BCHP_AON_HDMI_TX_CEC_TX_DATA_1 
#define REGNAME_CEC_TX_DATA_1 AON_HDMI_TX_CEC_TX_DATA_1 

#define REGADDR_CEC_RX_DATA_1 BCHP_AON_HDMI_TX_CEC_RX_DATA_1 
#define REGNAME_CEC_RX_DATA_1 AON_HDMI_TX_CEC_RX_DATA_1 

#define REGADDR_CEC_TX_AUTO_CEC_CFG BCHP_AON_HDMI_TX_AUTO_CEC_CFG
#define REGNAME_CEC_TX_AUTO_CEC_CFG AON_HDMI_TX_AUTO_CEC_CFG

#define REGADDR_CEC_TX_AUTO_CEC_CNTRL BCHP_AON_HDMI_TX_AUTO_CEC_CNTRL
#define REGNAME_CEC_TX_AUTO_CEC_CNTRL AON_HDMI_TX_AUTO_CEC_CNTRL

#define REGADDR_CEC_TX_AUTO_CEC_FEATURE_ABORT_CFG BCHP_AON_HDMI_TX_AUTO_CEC_FEATURE_ABORT_CFG
#define REGNAME_CEC_TX_AUTO_CEC_FEATURE_ABORT_CFG AON_HDMI_TX_AUTO_CEC_FEATURE_ABORT_CFG

#define REGADDR_CEC_TX_AUTO_CEC_POWER_STATUS_CFG BCHP_AON_HDMI_TX_AUTO_CEC_POWER_STATUS_CFG
#define REGNAME_CEC_TX_AUTO_CEC_POWER_STATUS_CFG AON_HDMI_TX_AUTO_CEC_POWER_STATUS_CFG

#define REGADDR_CEC_TX_AUTO_CEC_CHECK_STANDBY_CFG BCHP_AON_HDMI_TX_AUTO_CEC_CHECK_STANDBY_CFG
#define REGNAME_CEC_TX_AUTO_CEC_CHECK_STANDBY_CFG AON_HDMI_TX_AUTO_CEC_CHECK_STANDBY_CFG

#define REGADDR_CEC_TX_AUTO_CEC_CHECK_WAKE1_CFG_0 BCHP_AON_HDMI_TX_AUTO_CEC_CHECK_WAKE1_CFG_0
#define REGNAME_CEC_TX_AUTO_CEC_CHECK_WAKE1_CFG_0 AON_HDMI_TX_AUTO_CEC_CHECK_WAKE1_CFG_0

#define REGADDR_CEC_TX_AUTO_CEC_CHECK_WAKE1_CFG_1 BCHP_AON_HDMI_TX_AUTO_CEC_CHECK_WAKE1_CFG_1
#define REGNAME_CEC_TX_AUTO_CEC_CHECK_WAKE1_CFG_1 AON_HDMI_TX_AUTO_CEC_CHECK_WAKE1_CFG_1

#define REGADDR_CEC_TX_AUTO_CEC_CHECK_WAKE1_CFG_2 BCHP_AON_HDMI_TX_AUTO_CEC_CHECK_WAKE1_CFG_2
#define REGNAME_CEC_TX_AUTO_CEC_CHECK_WAKE1_CFG_2 AON_HDMI_TX_AUTO_CEC_CHECK_WAKE1_CFG_2

#define REGADDR_CEC_TX_AUTO_CEC_CHECK_WAKE1_CFG_3 BCHP_AON_HDMI_TX_AUTO_CEC_CHECK_WAKE1_CFG_3
#define REGNAME_CEC_TX_AUTO_CEC_CHECK_WAKE1_CFG_3 AON_HDMI_TX_AUTO_CEC_CHECK_WAKE1_CFG_3

#define REGADDR_CEC_TX_AUTO_CEC_CHECK_WAKE2_CFG_0 BCHP_AON_HDMI_TX_AUTO_CEC_CHECK_WAKE2_CFG_0
#define REGNAME_CEC_TX_AUTO_CEC_CHECK_WAKE2_CFG_0 AON_HDMI_TX_AUTO_CEC_CHECK_WAKE2_CFG_0

#define REGADDR_CEC_TX_AUTO_CEC_CHECK_WAKE2_CFG_1 BCHP_AON_HDMI_TX_AUTO_CEC_CHECK_WAKE2_CFG_1
#define REGNAME_CEC_TX_AUTO_CEC_CHECK_WAKE2_CFG_1 AON_HDMI_TX_AUTO_CEC_CHECK_WAKE2_CFG_1

#define REGADDR_CEC_TX_AUTO_CEC_CHECK_WAKE3_CFG_0 BCHP_AON_HDMI_TX_AUTO_CEC_CHECK_WAKE3_CFG_0
#define REGNAME_CEC_TX_AUTO_CEC_CHECK_WAKE3_CFG_0 AON_HDMI_TX_AUTO_CEC_CHECK_WAKE3_CFG_0

#define REGADDR_CEC_TX_AUTO_CEC_CHECK_WAKE3_CFG_1 BCHP_AON_HDMI_TX_AUTO_CEC_CHECK_WAKE3_CFG_1
#define REGNAME_CEC_TX_AUTO_CEC_CHECK_WAKE3_CFG_1 AON_HDMI_TX_AUTO_CEC_CHECK_WAKE3_CFG_1

#define REGADDR_CEC_TX_AUTO_CEC_CHECK_WAKE4_CFG_0 BCHP_AON_HDMI_TX_AUTO_CEC_CHECK_WAKE4_CFG_0
#define REGNAME_CEC_TX_AUTO_CEC_CHECK_WAKE4_CFG_0 AON_HDMI_TX_AUTO_CEC_CHECK_WAKE4_CFG_0

#define REGADDR_CEC_TX_AUTO_CEC_CHECK_WAKE4_CFG_1 BCHP_AON_HDMI_TX_AUTO_CEC_CHECK_WAKE4_CFG_1
#define REGNAME_CEC_TX_AUTO_CEC_CHECK_WAKE4_CFG_1 AON_HDMI_TX_AUTO_CEC_CHECK_WAKE4_CFG_1

#define REGADDR_CEC_TX_CEC_ENERGYSTAR_CFG1 BCHP_AON_HDMI_TX_CEC_ENERGYSTAR_CFG1
#define REGNAME_CEC_TX_CEC_ENERGYSTAR_CFG1 AON_HDMI_TX_CEC_ENERGYSTAR_CFG1

#define REGADDR_CEC_TX_CEC_ENERGYSTAR_CFG2 BCHP_AON_HDMI_TX_CEC_ENERGYSTAR_CFG2
#define REGNAME_CEC_TX_CEC_ENERGYSTAR_CFG2 AON_HDMI_TX_CEC_ENERGYSTAR_CFG2

#define REGADDR_CEC_TX_CEC_ENERGYSTAR_CNTRL BCHP_AON_HDMI_TX_CEC_ENERGYSTAR_CNTRL
#define REGNAME_CEC_TX_CEC_ENERGYSTAR_CNTRL AON_HDMI_TX_CEC_ENERGYSTAR_CNTRL

#define REGADDR_CEC_TX_CEC_ENERGYSTAR_STATUS BCHP_AON_HDMI_TX_CEC_ENERGYSTAR_STATUS
#define REGNAME_CEC_TX_CEC_ENERGYSTAR_STATUS AON_HDMI_TX_CEC_ENERGYSTAR_STATUS


#elif BCEC_CONFIG_65NM_SUPPORT

#define BCHP_MASK_CEC_CNTRL(Register,Field) BCHP_HDMI_##Register##_##Field##_MASK

#define REGADDR_CEC_CNTRL_1 BCHP_HDMI_CEC_CNTRL_1 
#define REGNAME_CEC_CNTRL_1 HDMI_CEC_CNTRL_1 

#define REGADDR_CEC_CNTRL_2 BCHP_HDMI_CEC_CNTRL_2
#define REGNAME_CEC_CNTRL_2 HDMI_CEC_CNTRL_2

#define REGADDR_CEC_CNTRL_3 BCHP_HDMI_CEC_CNTRL_3
#define REGNAME_CEC_CNTRL_3 HDMI_CEC_CNTRL_3

#define REGADDR_CEC_CNTRL_4 BCHP_HDMI_CEC_CNTRL_4
#define REGNAME_CEC_CNTRL_4 HDMI_CEC_CNTRL_4

#define REGADDR_CEC_CNTRL_5 BCHP_HDMI_CEC_CNTRL_5
#define REGNAME_CEC_CNTRL_5 HDMI_CEC_CNTRL_5


#define REGADDR_CEC_TX_DATA_1 BCHP_HDMI_CEC_TX_DATA_1 
#define REGNAME_CEC_TX_DATA_1 HDMI_CEC_TX_DATA_1 

#define REGADDR_CEC_RX_DATA_1 BCHP_HDMI_CEC_RX_DATA_1 
#define REGNAME_CEC_RX_DATA_1 HDMI_CEC_RX_DATA_1 

#else
#define BCHP_MASK_DVP(Register,Field) BCHP_HDMI_##Register##_##Field##_MASK

#define REGADDR_CEC_CNTRL_1 BCHP_HDMI_CEC_CNTRL_1 
#define REGNAME_CEC_CNTRL_1 HDMI_CEC_CNTRL_1 

#define REGADDR_CEC_CNTRL_2 BCHP_HDMI_CEC_CNTRL_2
#define REGNAME_CEC_CNTRL_2 HDMI_CEC_CNTRL_2

#define REGADDR_CEC_CNTRL_3 BCHP_HDMI_CEC_CNTRL_3
#define REGNAME_CEC_CNTRL_3 HDMI_CEC_CNTRL_3

#define REGADDR_CEC_CNTRL_4 BCHP_HDMI_CEC_CNTRL_4
#define REGNAME_CEC_CNTRL_4 HDMI_CEC_CNTRL_4

#define REGADDR_CEC_CNTRL_5 BCHP_HDMI_CEC_CNTRL_5
#define REGNAME_CEC_CNTRL_5 HDMI_CEC_CNTRL_5

#define REGADDR_CEC_TX_DATA_1 BCHP_HDMI_CEC_TX_DATA_1 
#define REGNAME_CEC_TX_DATA_1 HDMI_CEC_TX_DATA_1 

#define REGADDR_CEC_RX_DATA_1 BCHP_HDMI_CEC_RX_DATA_1 
#define REGNAME_CEC_RX_DATA_1 HDMI_CEC_RX_DATA_1 
#endif


#if BCEC_CONFIG_HAS_HDMI_RX
#define REGADDR_CEC_RX_CEC_ENERGYSTAR_CFG1 BCHP_AON_HDMI_RX_CEC_ENERGYSTAR_CFG1
#define REGNAME_CEC_RX_CEC_ENERGYSTAR_CFG1 AON_HDMI_RX_CEC_ENERGYSTAR_CFG1

#define REGADDR_CEC_RX_CEC_ENERGYSTAR_CFG2 BCHP_AON_HDMI_RX_CEC_ENERGYSTAR_CFG2
#define REGNAME_CEC_RX_CEC_ENERGYSTAR_CFG2 AON_HDMI_RX_CEC_ENERGYSTAR_CFG2

#define REGADDR_CEC_RX_CEC_ENERGYSTAR_CNTRL BCHP_AON_HDMI_RX_CEC_ENERGYSTAR_CNTRL
#define REGNAME_CEC_RX_CEC_ENERGYSTAR_CNTRL AON_HDMI_RX_CEC_ENERGYSTAR_CNTRL

#define REGADDR_CEC_RX_CEC_ENERGYSTAR_STATUS BCHP_AON_HDMI_RX_CEC_ENERGYSTAR_STATUS
#define REGNAME_CEC_RX_CEC_ENERGYSTAR_STATUS AON_HDMI_RX_CEC_ENERGYSTAR_STATUS
#endif


/* CEC default timings */
#if (BCHP_CHIP == 7038) || (BCHP_CHIP == 7401) || (BCHP_CHIP == 7403) || \
	(BCHP_CHIP == 7440) || (BCHP_CHIP == 7400) || (BCHP_CHIP == 7405) || \
	(BCHP_CHIP == 7325) || (BCHP_CHIP == 7335)
/* Optimal settings for HDMI CEC control registers for 108Mhz system clock */
#define BCEC_CNTRL_1_DIV_CLK_CNT_VALUE			0xa8b
#define BCEC_CNTRL_2_CNT_TO_400_US_VALUE		0x00e
#define BCEC_CNTRL_2_CNT_TO_600_US_VALUE		0x018
#define BCEC_CNTRL_2_CNT_TO_800_US_VALUE		0x022
#define BCEC_CNTRL_2_CNT_TO_1300_US_VALUE		0x032
#define BCEC_CNTRL_2_CNT_TO_1500_US_VALUE		0x03c
#define BCEC_CNTRL_3_CNT_TO_1700_US_VALUE		0x046
#define BCEC_CNTRL_3_CNT_TO_2050_US_VALUE		0x050
#define BCEC_CNTRL_3_CNT_TO_2400_US_VALUE		0x062
#define BCEC_CNTRL_3_CNT_TO_2750_US_VALUE		0x070
#define BCEC_CNTRL_4_CNT_TO_3500_US_VALUE		0x08A
#define BCEC_CNTRL_4_CNT_TO_3600_US_VALUE		0x094
#define BCEC_CNTRL_4_CNT_TO_3900_US_VALUE		0x09e
#define BCEC_CNTRL_4_CNT_TO_4300_US_VALUE		0x0aa
#define BCEC_CNTRL_5_CNT_TO_4500_US_VALUE		0x0b4
#define BCEC_CNTRL_5_CNT_TO_4700_US_VALUE		0x0be

#else
/* Optimal settings for HDMI CEC control registers for 27Mhz system clock */
#define BCEC_CNTRL_1_DIV_CLK_CNT_VALUE			0x545
#define BCEC_CNTRL_2_CNT_TO_400_US_VALUE		0x007
#define BCEC_CNTRL_2_CNT_TO_600_US_VALUE		0x00c
#define BCEC_CNTRL_2_CNT_TO_800_US_VALUE		0x011
#define BCEC_CNTRL_2_CNT_TO_1300_US_VALUE		0x019
#define BCEC_CNTRL_2_CNT_TO_1500_US_VALUE		0x01e
#define BCEC_CNTRL_3_CNT_TO_1700_US_VALUE		0x023
#define BCEC_CNTRL_3_CNT_TO_2050_US_VALUE		0x028
#define BCEC_CNTRL_3_CNT_TO_2400_US_VALUE		0x031
#define BCEC_CNTRL_3_CNT_TO_2750_US_VALUE		0x038
#define BCEC_CNTRL_4_CNT_TO_3500_US_VALUE		0x045
#define BCEC_CNTRL_4_CNT_TO_3600_US_VALUE		0x04a
#define BCEC_CNTRL_4_CNT_TO_3900_US_VALUE		0x04f
#define BCEC_CNTRL_4_CNT_TO_4300_US_VALUE		0x055
#define BCEC_CNTRL_5_CNT_TO_4500_US_VALUE		0x05a
#define BCEC_CNTRL_5_CNT_TO_4700_US_VALUE		0x05f
#endif



#define BCEC_MAKE_INTR_ENUM(IntName) BCEC_INTR_e##IntName
#define BCEC_MAKE_INTR_NAME(IntName) "BCEC_" #IntName


#define BCEC_MAX_OPCODES 62
typedef struct 
{
	uint8_t opCode ;
	const char *opText ;
} BCEC_OpcodeTextTable ;

							  
/******************************************************************************
Summary:
Enumeration of BCEC_Interrupts 
*******************************************************************************/
typedef enum
{
#if BCEC_CONFIG_DUAL_INTERRUPT
	/* 00 */ BCEC_MAKE_INTR_ENUM(RECEIVED),
	/* 01 */ BCEC_MAKE_INTR_ENUM(SENT),	
#else
	/* 00 */ BCEC_MAKE_INTR_ENUM(CEC),
#endif	
	/* 01 */ BCEC_MAKE_INTR_ENUM(LAST)
} BCEC_P_InterruptMask ;


typedef struct BCEC_P_ReceivedMessageInfo
{
	BAVC_HDMI_CEC_MessageData data;	
	BCEC_MessageStatus status;
} BCEC_P_ReceivedMessageInfo ;


/*******************************************************************************
Private CEC Handle Declaration 
*******************************************************************************/
BDBG_OBJECT_ID_DECLARE(BCEC_P_Handle);

typedef struct BCEC_P_Handle
{
	BDBG_OBJECT(BCEC_P_Handle)
	BCEC_Dependencies stDependencies;
	BCEC_Settings stSettings;
	BINT_CallbackHandle hCallback[BCEC_MAKE_INTR_ENUM(LAST)] ;

	BKNI_EventHandle BCEC_EventCec_Transmitted ;
	BKNI_EventHandle BCEC_EventCec_Received;
	BCEC_MessageStatus lastTransmitMessageStatus ;
	BCEC_MessageStatus lastReceivedMessageStatus ;
	bool firstCecMessage ;
	
	bool standby; /* true if in standby */
	bool enableAutoOn;
} BCEC_P_Handle ;


void BCEC_P_HandleInterrupt_isr(
	void *pParam1,						/* [in] Device handle */
	int parm2							/* [in] not used */
);


void BCEC_P_MaskWakeupEvents(
	BCEC_Handle hCEC, 
	uint32_t ConfigRegisterAddr) ;


void BCEC_P_ClearWakeupStatus(
	BCEC_Handle hCEC, 
	uint32_t StatusControlRegAddr) ;


#if BCEC_CONFIG_AUTO_ON_SUPPORT
void BCEC_P_EnableAutoOn(
	BCEC_Handle hCEC, 
	bool enable
);
#endif


#ifdef __cplusplus
 }
#endif
  
#endif /* BCEC_PRIV_H__ */
 
 
