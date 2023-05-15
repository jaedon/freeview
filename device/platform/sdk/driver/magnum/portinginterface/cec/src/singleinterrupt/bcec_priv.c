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
 * $brcm_Revision: Hydra_Software_Devel/9 $
 * $brcm_Date: 8/14/12 10:18a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/cec/7405/bcec_priv.c $
 * 
 * Hydra_Software_Devel/9   8/14/12 10:18a rgreen
 * SW7358-232,SW7429-204: Fix RDB names for 7358 which has single CEC
 * interrupt
 * 
 * Hydra_Software_Devel/8   8/9/12 11:04p rgreen
 * SW7429-204: No Clear/Mask wake-up event functions for 65nm
 * 
 * Hydra_Software_Devel/7   8/7/12 2:39p rgreen
 * SW7429-204: Make Clear/Mask wake-up event functionality private
 * functions; call private Clear/Mask event functions from bcec.c (Fix
 * log comments)
 * 
 * Hydra_Software_Devel/5   3/21/12 10:35a vle
 * SW7425-2617: Fix bogus debug message that might lead to oops in kernel
 * mode.
 * 
 * Hydra_Software_Devel/4   3/9/12 2:38p vle
 * SW7358-232: Add enable auto-on for 7358 Ax
 * 
 * Hydra_Software_Devel/3   2/21/12 4:32p vle
 * SW7425-2279: Update for 65nm platform support.
 * 
 * Hydra_Software_Devel/2   12/19/11 1:54p vle
 * SW7405-5574: Remove comment
 * 
 * Hydra_Software_Devel/1   12/19/11 1:52p vle
 * SW7405-5574: Add support for 65nm platforms
 * 
 ***************************************************************************/

#include "bstd.h"
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
	int parm2							/* [in] not used */
)
{
	uint32_t Register ;
	BCEC_Handle hCEC ;
	
#if BCEC_CONFIG_DEBUG_INTERRUPTS
	static const char *MsgStatus[] = {"NoACK", "ACK"} ;
	static const char *EomStatus[] = {"No", "Yes"} ;
#endif
	
	hCEC = (BCEC_Handle) pParam1 ;
	BDBG_ASSERT(hCEC) ;

	/* 
	-- Interrupts to be handled
	00 CEC_INTR 
	*/

	switch (parm2)
	{
	case BCEC_MAKE_INTR_ENUM(CEC) :						   /* 00 */ 		
	{
		BAVC_HDMI_CEC_IntMessageType CECMsgType ;

		Register = BREG_Read32(hCEC->stDependencies.hRegister, REGADDR_CEC_CNTRL_5) ;
		CECMsgType = BCHP_GET_FIELD_DATA(Register, REGNAME_CEC_CNTRL_5, RX_CEC_INT) ;

		/* Received Interrupt */
		if (CECMsgType == BAVC_HDMI_CEC_IntMessageType_eReceive)
		{
			Register = BREG_Read32(hCEC->stDependencies.hRegister, REGADDR_CEC_CNTRL_1) ;
			hCEC->lastReceivedMessageStatus.uiStatus = 
				BCHP_GET_FIELD_DATA(Register, REGNAME_CEC_CNTRL_1, RX_STATUS_GOOD) ;
					
			hCEC->lastReceivedMessageStatus.uiEOM = 
				BCHP_GET_FIELD_DATA(Register, REGNAME_CEC_CNTRL_1, RX_EOM) ;

				/* number of rx CEC words that came in */
			hCEC->lastReceivedMessageStatus.uiMessageLength = 
				BCHP_GET_FIELD_DATA(Register, REGNAME_CEC_CNTRL_1, REC_WRD_CNT) ;
				
#if BCEC_CONFIG_DEBUG_INTERRUPTS
			BDBG_WRN(("CEC Interrupt - Message RECEIVED (0x%x)!", parm2));
			BDBG_WRN(("Received %d Bytes, EOM status: %s", 
				hCEC->lastReceivedMessageStatus.uiMessageLength, 
				EomStatus[hCEC->lastReceivedMessageStatus.uiEOM])) ;
#endif				

			/* Set CEC Received Event */
			BKNI_SetEvent(hCEC->BCEC_EventCec_Received) ;

		}
		else {
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

			/* Set CEC Sent Event */
			BKNI_SetEvent(hCEC->BCEC_EventCec_Transmitted);
		}
		
		break;
	}	
	default :
		BDBG_MSG(("BCEC Unknown Interrupt ID=0x%x !", parm2 ));
	}

	return;
}


#if BCEC_CONFIG_AUTO_ON_SUPPORT 
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
			  BCHP_MASK_DVP(CEC_ENERGYSTAR_CFG2, CEC_MASK_IRQ)      /* CEC Addr Match */
			| BCHP_MASK_DVP(CEC_ENERGYSTAR_CFG2, CEC_MASK_LOW_IRQ) ;/* Invalid CEC Command */

		/* mask the hot plug signal. 
		    TODO should chip wake up if device is connected/removed on Tx port ??? */
		Register |= 
			BCHP_MASK_DVP(CEC_ENERGYSTAR_CFG2, HOTPLUG_MASK_IRQ) ;  /* HP Event */
	BREG_Write32(hCEC->stDependencies.hRegister, ConfigRegisterAddr, Register) ;  /* Wr 1 */
}





/* Enable CEC AutoOn feature */
void BCEC_P_EnableAutoOn(BCEC_Handle hCEC, bool enable)
{
	uint32_t Register;
	

	if (enable)
	{
		BDBG_MSG(("Enable Auto-On CEC function"));
		
		/* Clear HOTPLUG and "CEC Address Match" wake up events */		
		Register = BREG_Read32(hCEC->stDependencies.hRegister, REGADDR_CEC_TX_CEC_ENERGYSTAR_CNTRL) ;
		Register |= BCHP_MASK_DVP(CEC_ENERGYSTAR_CNTRL, HOTPLUG_CLR_INTERRUPT_DET)
				| BCHP_MASK_DVP(CEC_ENERGYSTAR_CNTRL, HOTPLUG_ACTIVITY_CLEAR)
				| BCHP_MASK_DVP(CEC_ENERGYSTAR_CNTRL, CEC_CLR_LOW_INTERRUPT_DET)		
				| BCHP_MASK_DVP(CEC_ENERGYSTAR_CNTRL, CEC_CLR_INTERRUPT_DET)		
				| BCHP_MASK_DVP(CEC_ENERGYSTAR_CNTRL, CEC_ACTIVITY_CLEAR);
		BREG_Write32(hCEC->stDependencies.hRegister, REGADDR_CEC_TX_CEC_ENERGYSTAR_CNTRL, Register) ;  /* Wr 1 */

		Register &= ~(BCHP_MASK_DVP(CEC_ENERGYSTAR_CNTRL, HOTPLUG_CLR_INTERRUPT_DET)
				| BCHP_MASK_DVP(CEC_ENERGYSTAR_CNTRL, HOTPLUG_ACTIVITY_CLEAR)
				| BCHP_MASK_DVP(CEC_ENERGYSTAR_CNTRL, CEC_CLR_LOW_INTERRUPT_DET)		
				| BCHP_MASK_DVP(CEC_ENERGYSTAR_CNTRL, CEC_CLR_INTERRUPT_DET)		
				| BCHP_MASK_DVP(CEC_ENERGYSTAR_CNTRL, CEC_ACTIVITY_CLEAR));
		BREG_Write32(hCEC->stDependencies.hRegister, REGADDR_CEC_TX_CEC_ENERGYSTAR_CNTRL, Register) ;  /* Wr 0 */		

		/* MASK "CEC address match" to prevent it from waking up the chip. 
		If this event is enabled, any CEC messages with the address matched the CEC_ADDR field in 
		AON_HDMI_TX.CEC_CNTRL.CEC_ADDR will cause the chip to wake up. That is not desired behavior */
		Register = BREG_Read32(hCEC->stDependencies.hRegister, REGADDR_CEC_TX_CEC_ENERGYSTAR_CFG2) ;
		Register |= BCHP_MASK_DVP(CEC_ENERGYSTAR_CFG2, CEC_MASK_IRQ)
				| BCHP_MASK_DVP(CEC_ENERGYSTAR_CFG2, CEC_MASK_LOW_IRQ);
		BREG_Write32(hCEC->stDependencies.hRegister, REGADDR_CEC_TX_CEC_ENERGYSTAR_CFG2, Register) ;  /* Wr 1 */

		
		
	}
	else
	{
		BDBG_MSG(("Disable Auto-On CEC function"));
		
	}
		
	return;
}

#endif


