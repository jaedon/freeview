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
 * $brcm_Workfile: bcec_config.h $
 * $brcm_Revision: Hydra_Software_Devel/12 $
 * $brcm_Date: 10/4/12 12:52p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/cec/src/common/bcec_config.h $
 * 
 * Hydra_Software_Devel/12   10/4/12 12:52p vle
 * SW7445-51: Add support for 7445
 * 
 * Hydra_Software_Devel/11   7/26/12 5:17p vle
 * SW7429-204: Fix False CEC wake up in S2/S3 modes on 7429. Need to clear
 * HOTPLUG and CEC activities/status on HDMI_Rx side before going into
 * standby
 * 
 * Hydra_Software_Devel/10   5/15/12 4:34p vle
 * SW7584-8: Add support for 7584
 * 
 * Hydra_Software_Devel/9   3/7/12 10:50a vle
 * SW7425-2289: CEC DEBUG messages should be disabled by default.
 * 
 * Hydra_Software_Devel/8   3/5/12 5:47p vle
 * SW7425-2289: AUTO_ON feature is supported on all 40nm platforms.
 * 
 * Hydra_Software_Devel/7   3/1/12 1:19p vle
 * SW7425-2289: Additional update to make sure only AutoOn CEC is active
 * 
 * Hydra_Software_Devel/6   2/28/12 10:57a vle
 * SW7425-2289, SW7420-2256: Add more debug messages, fix compiler
 * warnings for 65nm platforms
 * 
 * Hydra_Software_Devel/5   2/21/12 4:33p vle
 * SW7425-2279: Update for 65nm platform support.
 * 
 * Hydra_Software_Devel/4   2/17/12 3:44p vle
 * SW7425-2279: Merge to mainline
 * 
 * Hydra_Software_Devel/SW7425-2279/1   2/9/12 3:35p vle
 * SW7425-2279: Update BCEC PI implementations. Use separate events for
 * transmit and receive.
 * 
 * Hydra_Software_Devel/3   1/16/12 6:50p xhuang
 * SW7552-193 : Add CEC support for 7552
 * 
 * Hydra_Software_Devel/2   12/21/11 1:39p vle
 * SW7405-4781: merge to mainline
 * 
 * Hydra_Software_Devel/SW7405-4781/1   12/21/11 12:12p vle
 * SW7405-4781: allow CEC device type to be configurable
 * 
 * Hydra_Software_Devel/1   11/22/11 6:23p vle
 * SW7425-1140: Add BCEC PI support
 * 
 * Hydra_Software_Devel/SW7425-1140/2   11/22/11 3:50p rgreen
 * SW7425-1140: Add macros for debug support
 * 
 * Hydra_Software_Devel/SW7425-1140/1   11/16/11 12:13p vle
 * SW7425-1140: Add CEC PI support for 7425
 * 
 ***************************************************************************/
 
#ifndef BCEC_CONFIG_H__
#define BCEC_CONFIG_H__

#include "bchp.h"
#include "bcec.h"


#ifdef __cplusplus
extern "C" {
#endif


/* The following configuration SHOULD NOT be enable in standard releases */
#define BCEC_CONFIG_DEBUG_CEC 0
#define BCEC_CONFIG_DEBUG_INTERRUPTS 0
#define BCEC_CONFIG_DEBUG_CEC_TIMING 0
#define BCEC_CONFIG_DEBUG_MESSAGE_TX 0
#define BCEC_CONFIG_DEBUG_MESSAGE_RX 0
#define BCEC_CONFIG_DEBUG_OPCODE 0

#define BCEC_CONFIG_P_MAX_MESSAGE_BUFFER 16

/***************************************
	DO NOT MODIFY THE BELOW CODE 
***************************************/
#if ((BCHP_CHIP == 7400) && (BCHP_VER >= BCHP_VER_B0)) || (BCHP_CHIP == 7405) || (BCHP_CHIP == 7325) \
		|| (BCHP_CHIP == 7335) || (BCHP_CHIP == 7336) || (BCHP_CHIP == 7340) || (BCHP_CHIP == 7342) \
		|| (BCHP_CHIP == 7420) || (BCHP_CHIP == 7125) || (BCHP_CHIP == 7550) || (BCHP_CHIP == 7208) \
		|| (BCHP_CHIP == 7408) || (BCHP_CHIP == 7468) || (BCHP_CHIP == 7601) || (BCHP_CHIP == 7630) \
		|| (BCHP_CHIP == 7635)
#define BCEC_CONFIG_65NM_SUPPORT 1
#endif


/*****************************
 * 40nm platforms:
 * + Rev. 1 corresponds to Ax
 * + Rev. 2 corresponds to
		* 40nm B0 and later (except 7552)
		* 7429 and later platforms
 ******************************/
#if    (BCHP_CHIP == 7422) || (BCHP_CHIP == 7425) || (BCHP_CHIP == 7231) \
	|| (BCHP_CHIP == 7358) || (BCHP_CHIP == 7344) || (BCHP_CHIP == 7346) \
	|| (BCHP_CHIP == 7640) || (BCHP_CHIP == 7552) || (BCHP_CHIP == 7429) \
	|| (BCHP_CHIP == 7435) || (BCHP_CHIP == 7360) || (BCHP_CHIP == 7584) 
#define BCEC_CONFIG_40NM_SUPPORT 1
#endif


/*****************************
 * 28nm platforms:
 ******************************/
#if (BCHP_CHIP == 7445)
#define BCEC_CONFIG_28NM_SUPPORT 1
#endif


/**********************************
	40nm Rev1 platforms have only 1 CEC 
	interrupt	for both send and receive
***********************************/	
#if ((BCHP_VER < BCHP_VER_B0) \
	&& ((BCHP_CHIP == 7422) || (BCHP_CHIP == 7425) || (BCHP_CHIP == 7231) \
	|| (BCHP_CHIP == 7358) || (BCHP_CHIP == 7344) || (BCHP_CHIP == 7346) \
	|| (BCHP_CHIP == 7640))) \
	|| (BCHP_CHIP == 7552) 
#define BCEC_CONFIG_SINGLE_INTERRUPT 1
#endif


/**********************************
	40nm Rev2 platforms have 2 separate 
	CEC interrupts: send and receive
***********************************/    
#if ((BCHP_VER >= BCHP_VER_B0) \
	&& ((BCHP_CHIP == 7422) || (BCHP_CHIP == 7425) || (BCHP_CHIP == 7231) \
	|| (BCHP_CHIP == 7358) || (BCHP_CHIP == 7344) || (BCHP_CHIP == 7346) \
	|| (BCHP_CHIP == 7640) || (BCHP_CHIP == 7552))) \
	|| (BCHP_CHIP == 7429) || (BCHP_CHIP == 7435) || (BCHP_CHIP == 7360) \
	|| (BCHP_CHIP == 7584) \
	|| BCEC_CONFIG_28NM_SUPPORT
#define BCEC_CONFIG_DUAL_INTERRUPT 1
#endif


/***********************************
	Platforms support AUTO_ON Features
***********************************/	
#if BCEC_CONFIG_40NM_SUPPORT || BCEC_CONFIG_28NM_SUPPORT
#define BCEC_CONFIG_AUTO_ON_SUPPORT 1
#endif


/****************************************
	Platforms with both HDMI_TX and HDMI_Rx
****************************************/	
#if		(BCHP_CHIP == 7640) || (BCHP_CHIP == 7422) \
	 || (BCHP_CHIP == 7425) || (BCHP_CHIP == 7429) \
	 || (BCHP_CHIP == 7435) || (BCHP_CHIP == 7445)
#define BCEC_CONFIG_HAS_HDMI_RX 1	
#endif


/***************************************
	Older platforms use CEC PAD_SW_RESET register
	7601/7420 and newer platforms no longer use this.
*****************************************/
#if (BCHP_CHIP==7038) || (BCHP_CHIP==7438) || (BCHP_CHIP==7440) || \
	(BCHP_CHIP==7400) || (BCHP_CHIP==7401) || (BCHP_CHIP==7405) || \
	(BCHP_CHIP==7335) || (BCHP_CHIP==7336) || (BCHP_CHIP==7325)
#define BCEC_CONFIG_CEC_USE_PAD_SW_RESET 1
#endif


/***************************************
	Uninitialized CEC logical address
**************************************/
#define BCEC_CONFIG_UNINITIALIZED_LOGICAL_ADDR 0xFF


/* HDMI Tx DVD/Bluray */
#if (BCHP_CHIP==7438) || (BCHP_CHIP==7440) || (BCHP_CHIP==7601) || \
	(BCHP_CHIP==7635) || (BCHP_CHIP==7630) || (BCHP_CHIP==7640)
#define BCEC_CONFIG_DEVICE_TYPE 0x04
#else
#define BCEC_CONFIG_DEVICE_TYPE 0x03
#endif


#ifdef __cplusplus
}
#endif 

#endif /* BCEC_CONFIG_H__ */
/* End of File */

