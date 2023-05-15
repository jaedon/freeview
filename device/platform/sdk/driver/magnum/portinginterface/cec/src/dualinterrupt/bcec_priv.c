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
 * $brcm_Workfile: bcec_priv.c $
 * $brcm_Revision: Hydra_Software_Devel/11 $
 * $brcm_Date: 8/7/12 2:12p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/cec/7425/bcec_priv.c $
 * 
 * Hydra_Software_Devel/11   8/7/12 2:12p rgreen
 * SW7429-204:  Support both 65 and 40nm CEC wake up; call private
 * Clear/Mask event functions from bcec.c (fix JIRA checkin)
 * 
 * Hydra_Software_Devel/9   8/2/12 3:15p rgreen
 * SW7429-204: Fix incorrect wake up status.  Clear Wakeup Status and Mask
 * Wakeup Events for Both HDMI Tx and HDMI Rx cores.
 * 
 * Hydra_Software_Devel/8   7/30/12 3:56p rgreen
 * SW7429-204: For HDMI Rx/Tx combo chips mask wake up events on HDMI Rx
 * port.
 * 
 * Hydra_Software_Devel/7   7/26/12 5:17p vle
 * SW7429-204: Fix False CEC wake up in S2/S3 modes on 7429. Need to clear
 * HOTPLUG and CEC activities/status on HDMI_Rx side before going into
 * standby
 * 
 * Hydra_Software_Devel/6   3/21/12 10:35a vle
 * SW7425-2617: Fix bogus debug message that might lead to oops in kernel
 * mode.
 * 
 * Hydra_Software_Devel/5   3/12/12 6:04p vle
 * SW7425-2601: Fix build error in some cases.
 * 
 * Hydra_Software_Devel/4   3/1/12 1:20p vle
 * SW7425-2289: Additional update to make sure only AutoOn CEC is active
 * 
 * Hydra_Software_Devel/3   2/24/12 4:06p vle
 * SW7425-2289: Merge to mainline
 * 
 * Hydra_Software_Devel/SW7425-2289/1   2/22/12 6:11p vle
 * SW7425-2289: Add support for CEC AutoOn
 * 
 * Hydra_Software_Devel/2   2/17/12 3:44p vle
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

/***************************************
*** THIS IS for 40nm B0 (rev2)
***************************************/

#include "bstd.h"
#include "bdbg.h"
#include "bavc_hdmi.h" 

#include "bcec.h"
#include "bcec_priv.h"
#include "bcec_config.h"


BDBG_MODULE(BCEC_PRIV) ;


/******************************************************************************
void BCEC_P_HandleInterrupt_isr
Summary: Handle interrupts for CEC core.
*******************************************************************************/
void BCEC_P_HandleInterrupt_isr(
	void *pParam1,						/* [in] Device handle */
	int parm2							/* [in] */
)
{
	uint32_t Register ;
	BCEC_Handle hCEC ;
	
#if BCEC_CONFIG_DEBUG_INTERRUPTS
	static const char *MsgStatus[] = {"NoACK", "ACK"} ;
	static const char *EomStatus[] = {"No", "Yes"} ;
#endif
	
	hCEC = (BCEC_Handle) pParam1 ;
	BDBG_OBJECT_ASSERT(hCEC, BCEC_P_Handle) ;

	/* 
	-- Interrupts to be handled
	00 CEC_INTR_SENT
	01 CEC_INTR_RECEIVED
	*/

	switch (parm2)
	{
	case BCEC_MAKE_INTR_ENUM(SENT) : 					   /* 00 */ 

		Register = BREG_Read32(hCEC->stDependencies.hRegister, REGADDR_CEC_CNTRL_1) ;
		hCEC->lastTransmitMessageStatus.uiStatus =
			BCHP_GET_FIELD_DATA(Register, REGNAME_CEC_CNTRL_1, TX_STATUS_GOOD) ;

		hCEC->lastTransmitMessageStatus.uiMessageLength =
			BCHP_GET_FIELD_DATA(Register, REGNAME_CEC_CNTRL_1, MESSAGE_LENGTH) ;
			
		hCEC->lastTransmitMessageStatus.uiEOM = 
			BCHP_GET_FIELD_DATA(Register, REGNAME_CEC_CNTRL_1, TX_EOM) ;
		
#if BCEC_CONFIG_DEBUG_INTERRUPTS
		BDBG_WRN(("Message SENT Interrupt (0x%x)!", parm2));
		BDBG_WRN(("Transmitted Length: %d; Xmit Status: %s ", 
						hCEC->lastTransmitMessageStatus.uiMessageLength, 
						MsgStatus[hCEC->lastTransmitMessageStatus.uiStatus])) ;
#endif					 
#if defined(HUMAX_PLATFORM_BASE)
		Register = BREG_Read32(hCEC->stDependencies.hRegister, REGADDR_CEC_CNTRL_1) ;
		Register &=  ~ BCHP_MASK_CEC_CNTRL(CEC_CNTRL_1, START_XMIT_BEGIN) ;
		BREG_Write32(hCEC->stDependencies.hRegister, REGADDR_CEC_CNTRL_1, Register) ;   /* 0 */
#endif
		/* Set CEC Sent Event */
		BKNI_SetEvent(hCEC->BCEC_EventCec_Transmitted);
		break;

		
	case BCEC_MAKE_INTR_ENUM(RECEIVED) :

		Register = BREG_Read32(hCEC->stDependencies.hRegister, REGADDR_CEC_CNTRL_1) ;
		hCEC->lastReceivedMessageStatus.uiStatus = 
			BCHP_GET_FIELD_DATA(Register, REGNAME_CEC_CNTRL_1, RX_STATUS_GOOD) ;
				
		hCEC->lastReceivedMessageStatus.uiEOM = 
			BCHP_GET_FIELD_DATA(Register, REGNAME_CEC_CNTRL_1, RX_EOM) ;
		
			/* number of rx CEC words that came in */
		hCEC->lastReceivedMessageStatus.uiMessageLength = 
			BCHP_GET_FIELD_DATA(Register, REGNAME_CEC_CNTRL_1, REC_WRD_CNT) ;
			
#if BCEC_CONFIG_DEBUG_INTERRUPTS
		BDBG_WRN(("Message RECEIVED interrupt (0x%x)!", parm2));
		BDBG_WRN(("Received %d Bytes, EOM status: %s", 
			hCEC->lastReceivedMessageStatus.uiMessageLength, 
			EomStatus[hCEC->lastReceivedMessageStatus.uiEOM])) ;
#endif				
	
		/* Set CEC Received Event */
		BKNI_SetEvent(hCEC->BCEC_EventCec_Received) ;
		break ;

	default :
		BDBG_ERR(("BCEC Unknown Interrupt ID=0x%x !", parm2 ));
		break;
	}

	return;
}

void BCEC_P_ClearWakeupStatus(BCEC_Handle hCEC, uint32_t StatusControlRegAddr) 
{
	uint32_t Register ;
	
	Register = BREG_Read32(hCEC->stDependencies.hRegister, StatusControlRegAddr) ;
	Register |= BCHP_MASK_DVP(CEC_ENERGYSTAR_CNTRL, HOTPLUG_CLR_INTERRUPT_DET)
			| BCHP_MASK_DVP(CEC_ENERGYSTAR_CNTRL, HOTPLUG_ACTIVITY_CLEAR)
			| BCHP_MASK_DVP(CEC_ENERGYSTAR_CNTRL, CEC_CLR_LOW_INTERRUPT_DET) 		
			| BCHP_MASK_DVP(CEC_ENERGYSTAR_CNTRL, CEC_CLR_INTERRUPT_DET) 		
			| BCHP_MASK_DVP(CEC_ENERGYSTAR_CNTRL, CEC_ACTIVITY_CLEAR);
	BREG_Write32(hCEC->stDependencies.hRegister, StatusControlRegAddr, Register) ;  /* Wr 1 */

	Register &= ~(BCHP_MASK_DVP(CEC_ENERGYSTAR_CNTRL, HOTPLUG_CLR_INTERRUPT_DET)
			| BCHP_MASK_DVP(CEC_ENERGYSTAR_CNTRL, HOTPLUG_ACTIVITY_CLEAR)
			| BCHP_MASK_DVP(CEC_ENERGYSTAR_CNTRL, CEC_CLR_LOW_INTERRUPT_DET)		
			| BCHP_MASK_DVP(CEC_ENERGYSTAR_CNTRL, CEC_CLR_INTERRUPT_DET)		
			| BCHP_MASK_DVP(CEC_ENERGYSTAR_CNTRL, CEC_ACTIVITY_CLEAR));
	BREG_Write32(hCEC->stDependencies.hRegister, StatusControlRegAddr, Register) ;  /* Wr 0 */ 		
}


void BCEC_P_MaskWakeupEvents(BCEC_Handle hCEC, uint32_t ConfigRegisterAddr) 
{
	uint32_t Register ;
	
	/* mask  "CEC address match" to prevent it from waking up the chip. 
	If this event is enabled, any CEC messages with the address specified in the 
	AON_HDMI_TX.CEC_CNTRL.CEC_ADDR will cause the chip to wake up. 
	This is not the desired behavior */

	Register = BREG_Read32(hCEC->stDependencies.hRegister, ConfigRegisterAddr) ;
		Register |= 
			  BCHP_MASK_DVP(CEC_ENERGYSTAR_CFG2, CEC_MASK_IRQ)           /* CEC Addr Match */
			| BCHP_MASK_DVP(CEC_ENERGYSTAR_CFG2, CEC_MASK_LOW_IRQ) ;/* Invalid CEC Command */

		/* mask the hot plug signal. 
		    TODO should chip wake up if device is connected/removed on Tx port ??? */
		Register |= 
			  BCHP_MASK_DVP(CEC_ENERGYSTAR_CFG2, HOTPLUG_MASK_DETACH)  /* HP Remove */
			|BCHP_MASK_DVP(CEC_ENERGYSTAR_CFG2, HOTPLUG_MASK_IRQ) ;      /* HP Connect */
	BREG_Write32(hCEC->stDependencies.hRegister, ConfigRegisterAddr, Register) ;  /* Wr 1 */
}


/* Enable CEC AutoOn feature */
void BCEC_P_EnableAutoOn(BCEC_Handle hCEC, bool enable)
{
	BREG_Handle hRegister ;
	uint32_t Register;
	
	hRegister = hCEC->stDependencies.hRegister ;
	

	/* whether enabling or disabling AutoON; clear all WakeUp status values
	     and mask any unwanted Wake Up events */
	
	BCEC_P_ClearWakeupStatus(hCEC, REGADDR_CEC_TX_CEC_ENERGYSTAR_CNTRL) ;

	BCEC_P_MaskWakeupEvents(hCEC, REGADDR_CEC_TX_CEC_ENERGYSTAR_CFG2) ;


#if BCEC_CONFIG_HAS_HDMI_RX
	/* If HDMI_RX is also on this chip, 
	    be sure to it's WakeUp status and mask unwanted events */

	BCEC_P_ClearWakeupStatus(hCEC, REGADDR_CEC_RX_CEC_ENERGYSTAR_CNTRL) ;

	BCEC_P_MaskWakeupEvents(hCEC, REGADDR_CEC_RX_CEC_ENERGYSTAR_CFG2) ;

#endif

	if (enable)
	{
		BDBG_MSG(("Enable Auto-On CEC function"));
		
		/* Toggle to clear last AutoCEC Event */
		Register = BREG_Read32(hRegister, REGADDR_CEC_TX_AUTO_CEC_CNTRL) ;
		Register |= BCHP_MASK_DVP(AUTO_CEC_CNTRL, CLEAR_AUTO_CEC_XACTION_MATCH)
				| BCHP_MASK_DVP(AUTO_CEC_CNTRL, CLEAR_SET_AUTOCEC_RUN)
				| BCHP_MASK_DVP(AUTO_CEC_CNTRL, CLEAR_WAKE_REASON);			
		BREG_Write32(hRegister, REGADDR_CEC_TX_AUTO_CEC_CNTRL, Register) ;  /* Wr 1 */

		Register &= ~(BCHP_MASK_DVP(AUTO_CEC_CNTRL, CLEAR_AUTO_CEC_XACTION_MATCH)
				| BCHP_MASK_DVP(AUTO_CEC_CNTRL, CLEAR_SET_AUTOCEC_RUN)
				| BCHP_MASK_DVP(AUTO_CEC_CNTRL, CLEAR_WAKE_REASON));
		BREG_Write32(hRegister, REGADDR_CEC_TX_AUTO_CEC_CNTRL, Register) ;  /* Wr 0 */			


		/* Enable Auto CEC */
		Register = BREG_Read32(hRegister, REGADDR_CEC_TX_AUTO_CEC_CFG) ;
		Register |= BCHP_MASK_DVP(AUTO_CEC_CFG, SELECT_CEC_TX)
				| BCHP_MASK_DVP(AUTO_CEC_CFG, AUTO_CEC_EN)
				| BCHP_MASK_DVP(AUTO_CEC_CFG, AUTO_CLEAR_CEC_INTR_DET);
		BREG_Write32(hRegister, REGADDR_CEC_TX_AUTO_CEC_CFG, Register) ;  /* Wr 1 */ 		

		/* Need to toggle AUTO_CLEAR_CEC_INTR_DET */
		Register = BREG_Read32(hRegister, REGADDR_CEC_TX_AUTO_CEC_CFG) ;
		Register &= ~(BCHP_MASK_DVP(AUTO_CEC_CFG, AUTO_CLEAR_CEC_INTR_DET));
		BREG_Write32(hRegister, REGADDR_CEC_TX_AUTO_CEC_CFG, Register) ;  /* Wr 0 */ 		


		/*********************************************************
		  * Configure/enable specific CEC messages to wake up the device	  *
		  * The chip will only wake up if one of these CEC messages are	  *
		  * received												  *
		  *********************************************************/
		
		/* Configure Check Power Status - Opcode 0x8F. 
			The hardware will automatically response to request of current 
			power status. The chip will not be	waken up when this CEC message is received 
		*********/
		Register = BREG_Read32(hRegister, REGADDR_CEC_TX_AUTO_CEC_POWER_STATUS_CFG) ;
		Register &= ~(BCHP_MASK_DVP(AUTO_CEC_POWER_STATUS_CFG, POWER_STAT_EN)
				| BCHP_MASK_DVP(AUTO_CEC_POWER_STATUS_CFG, POWER_STAT_REARM)
				| BCHP_MASK_DVP(AUTO_CEC_POWER_STATUS_CFG, POWER_STAT_SENSITIVITY));
		Register |= BCHP_FIELD_DATA(REGNAME_CEC_TX_AUTO_CEC_POWER_STATUS_CFG, POWER_STAT_EN, 1)
				| BCHP_FIELD_DATA(REGNAME_CEC_TX_AUTO_CEC_POWER_STATUS_CFG, POWER_STAT_REARM, 1)
				| BCHP_FIELD_DATA(REGNAME_CEC_TX_AUTO_CEC_POWER_STATUS_CFG, POWER_STAT_SENSITIVITY, 7);
		BREG_Write32(hRegister, REGADDR_CEC_TX_AUTO_CEC_POWER_STATUS_CFG, Register) ;

		
		/********************************
		   enable <ImageViewOn> - OpCode 0x04 
		   and     <TextViewOn>  -  OpCode 0x0D
		********************************/
		Register = BREG_Read32(hRegister, REGADDR_CEC_TX_AUTO_CEC_CHECK_WAKE1_CFG_0) ;
		Register &= ~(BCHP_MASK_DVP(AUTO_CEC_CHECK_WAKE1_CFG_0, TEXT_VIEW_EN)
				| BCHP_MASK_DVP(AUTO_CEC_CHECK_WAKE1_CFG_0, TEXT_VIEW_SENSITIVITY)
				| BCHP_MASK_DVP(AUTO_CEC_CHECK_WAKE1_CFG_0, IMAGE_VIEW_EN)
				| BCHP_MASK_DVP(AUTO_CEC_CHECK_WAKE1_CFG_0, IMAGE_VIEW_SENSITIVITY));
		Register |= BCHP_FIELD_DATA(REGNAME_CEC_TX_AUTO_CEC_CHECK_WAKE1_CFG_0, TEXT_VIEW_EN, 1)
				| BCHP_FIELD_DATA(REGNAME_CEC_TX_AUTO_CEC_CHECK_WAKE1_CFG_0, TEXT_VIEW_SENSITIVITY, 7)
				| BCHP_FIELD_DATA(REGNAME_CEC_TX_AUTO_CEC_CHECK_WAKE1_CFG_0, IMAGE_VIEW_EN, 1)
				| BCHP_FIELD_DATA(REGNAME_CEC_TX_AUTO_CEC_CHECK_WAKE1_CFG_0, IMAGE_VIEW_SENSITIVITY, 7);
		BREG_Write32(hRegister, REGADDR_CEC_TX_AUTO_CEC_CHECK_WAKE1_CFG_0, Register) ;


		/********************************
		 enable <ActiveSouce> - OpCode 0x82 
		********************************/
		Register = BREG_Read32(hRegister, REGADDR_CEC_TX_AUTO_CEC_CHECK_WAKE1_CFG_1) ;
		Register &= ~(BCHP_MASK_DVP(AUTO_CEC_CHECK_WAKE1_CFG_1, ACTIVE_SOURCE_EN)
				| BCHP_MASK_DVP(AUTO_CEC_CHECK_WAKE1_CFG_1, ACTIVE_SOURCE_SENSITIVITY));
		Register |= BCHP_FIELD_DATA(REGNAME_CEC_TX_AUTO_CEC_CHECK_WAKE1_CFG_1, ACTIVE_SOURCE_EN, 1)
				| BCHP_FIELD_DATA(REGNAME_CEC_TX_AUTO_CEC_CHECK_WAKE1_CFG_1, ACTIVE_SOURCE_SENSITIVITY, 7);
		BREG_Write32(hRegister, REGADDR_CEC_TX_AUTO_CEC_CHECK_WAKE1_CFG_1, Register) ;


		/********************************
		 enable <UserPowerOn>
		    and <UserPowerToggle> - OpCode 0x44 
		********************************/
		Register = BREG_Read32(hRegister, REGADDR_CEC_TX_AUTO_CEC_CHECK_WAKE1_CFG_3) ;
		Register &= ~(BCHP_MASK_DVP(AUTO_CEC_CHECK_WAKE1_CFG_3, USER_POWER_ON_EN)
				| BCHP_MASK_DVP(AUTO_CEC_CHECK_WAKE1_CFG_3, USER_POWER_ON_SENSITIVITY)
				| BCHP_MASK_DVP(AUTO_CEC_CHECK_WAKE1_CFG_3, USER_POWER_TOGGLE_EN)
				| BCHP_MASK_DVP(AUTO_CEC_CHECK_WAKE1_CFG_3, USER_POWER_TOGGLE_SENSITIVITY));
		Register |= BCHP_FIELD_DATA(REGNAME_CEC_TX_AUTO_CEC_CHECK_WAKE1_CFG_3, USER_POWER_ON_EN, 1)
				| BCHP_FIELD_DATA(REGNAME_CEC_TX_AUTO_CEC_CHECK_WAKE1_CFG_3, USER_POWER_ON_SENSITIVITY, 7)
				| BCHP_FIELD_DATA(REGNAME_CEC_TX_AUTO_CEC_CHECK_WAKE1_CFG_3, USER_POWER_TOGGLE_EN, 1)
				| BCHP_FIELD_DATA(REGNAME_CEC_TX_AUTO_CEC_CHECK_WAKE1_CFG_3, USER_POWER_TOGGLE_SENSITIVITY, 7);
		BREG_Write32(hRegister, REGADDR_CEC_TX_AUTO_CEC_CHECK_WAKE1_CFG_3, Register) ;

		/* Rearm WAKE1 configurations to enable rechecking on the above CEC messages */
		Register = BREG_Read32(hRegister, REGADDR_CEC_TX_AUTO_CEC_CHECK_WAKE1_CFG_0) ;
		Register |= BCHP_MASK_DVP(AUTO_CEC_CHECK_WAKE1_CFG_0, WAKE1_REARM);
		BREG_Write32(hRegister, REGADDR_CEC_TX_AUTO_CEC_CHECK_WAKE1_CFG_0, Register) ;  /* Wr 1 */ 		


		/********************************
		   enable <PlayForward> - OpCode 0x41 
		********************************/
		Register = BREG_Read32(hRegister, REGADDR_CEC_TX_AUTO_CEC_CHECK_WAKE2_CFG_0) ;
		Register &= ~(BCHP_MASK_DVP(AUTO_CEC_CHECK_WAKE2_CFG_0, PLAY_FORWARD_EN)
				| BCHP_MASK_DVP(AUTO_CEC_CHECK_WAKE2_CFG_0, PLAY_FORWARD_SENSITIVITY));
		Register |= BCHP_FIELD_DATA(REGNAME_CEC_TX_AUTO_CEC_CHECK_WAKE2_CFG_0, PLAY_FORWARD_EN, 1)
				| BCHP_FIELD_DATA(REGNAME_CEC_TX_AUTO_CEC_CHECK_WAKE2_CFG_0, PLAY_FORWARD_SENSITIVITY, 7);
		BREG_Write32(hRegister, REGADDR_CEC_TX_AUTO_CEC_CHECK_WAKE2_CFG_0, Register) ;


		/********************************
		  enable <DeckControlEject> - OpCode 0x42 	
		********************************/
		Register = BREG_Read32(hRegister, REGADDR_CEC_TX_AUTO_CEC_CHECK_WAKE2_CFG_1) ;
		Register &= ~(BCHP_MASK_DVP(AUTO_CEC_CHECK_WAKE2_CFG_1, EJECT_EN)
				| BCHP_MASK_DVP(AUTO_CEC_CHECK_WAKE2_CFG_1, EJECT_SENSITIVITY));
		Register |= BCHP_FIELD_DATA(REGNAME_CEC_TX_AUTO_CEC_CHECK_WAKE2_CFG_1, EJECT_EN, 1)
				| BCHP_FIELD_DATA(REGNAME_CEC_TX_AUTO_CEC_CHECK_WAKE2_CFG_1, EJECT_SENSITIVITY, 7);
		BREG_Write32(hRegister, REGADDR_CEC_TX_AUTO_CEC_CHECK_WAKE2_CFG_1, Register) ;

		
		/* Rearm WAKE2 configurations to enable rechecking */
		Register = BREG_Read32(hRegister, REGADDR_CEC_TX_AUTO_CEC_CHECK_WAKE2_CFG_0) ;
		Register |= BCHP_MASK_DVP(AUTO_CEC_CHECK_WAKE2_CFG_0, WAKE2_REARM);
		BREG_Write32(hRegister, REGADDR_CEC_TX_AUTO_CEC_CHECK_WAKE2_CFG_0, Register) ;  /* Wr 1 */ 		
		
	}
	else
	{
		BDBG_MSG(("Disable Auto-On CEC function"));
		
		Register = BREG_Read32(hRegister, REGADDR_CEC_TX_AUTO_CEC_CFG) ;
		Register &= ~(BCHP_MASK_DVP(AUTO_CEC_CFG, SELECT_CEC_TX)
				| BCHP_MASK_DVP(AUTO_CEC_CFG, AUTO_CEC_EN));									/* Wr 0 */
		Register |= BCHP_MASK_DVP(AUTO_CEC_CFG, AUTO_CLEAR_CEC_INTR_DET);						/* Wr 1 */
		BREG_Write32(hRegister, REGADDR_CEC_TX_AUTO_CEC_CFG, Register) ; 		

		/* Need to toggle AUTO_CLEAR_CEC_INTR_DET */
		Register = BREG_Read32(hRegister, REGADDR_CEC_TX_AUTO_CEC_CFG) ;
		Register &= ~(BCHP_MASK_DVP(AUTO_CEC_CFG, AUTO_CLEAR_CEC_INTR_DET)); 		 		 	 /* Wr 0 */
		BREG_Write32(hRegister, REGADDR_CEC_TX_AUTO_CEC_CFG, Register) ;	   
	}
		
	return;
}


