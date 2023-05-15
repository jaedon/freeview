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
 * $brcm_Workfile: bcec.c $
 * $brcm_Revision: Hydra_Software_Devel/19 $
 * $brcm_Date: 10/4/12 12:52p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/cec/src/common/bcec.c $
 * 
 * Hydra_Software_Devel/19   10/4/12 12:52p vle
 * SW7445-51: Add support for 7445
 * 
 * Hydra_Software_Devel/18   9/11/12 6:42p rgreen
 * SW7425-3776:Merge Changes
 * 
 * Hydra_Software_Devel/cec/2   8/23/12 5:23p cng
 * SW7425-3776: Corrected naming of cecVersion in settings
 * 
 * Hydra_Software_Devel/cec/1   8/21/12 10:23a cng
 * SW7425-3776: Add CEC version support in CecSettings. Corrected value of
 * device type.
 * 
 * Hydra_Software_Devel/17   8/9/12 11:04p rgreen
 * SW7429-204: No Clear/Mask wake-up event functions for 65nm
 * 
 * Hydra_Software_Devel/16   8/9/12 3:38p rgreen
 * SW7425-3712,SW7425-2851: Fix coverity issue
 * 
 * Hydra_Software_Devel/15   8/7/12 2:41p rgreen
 * SW7429-204: Create private functions for Clear/Mask wake-up events to
 * support both 65 and 40nm;  Update chip macros to support both
 * platforms (fix checkin comments)
 * 
 * Hydra_Software_Devel/13   7/19/12 11:06p vle
 * SW7346-923: Move power management TX_CEC references to BCEC PI to
 * prevent bus error
 * 
 * Hydra_Software_Devel/12   5/16/12 5:58p vle
 * SW7425-2852: Update CEC PI to use BDBG_OBJECT_ASSERT() on public APIs
 * 
 * Hydra_Software_Devel/11   4/9/12 2:34p vle
 * SW7425-2851: Fix compiler warning.
 * 
 * Hydra_Software_Devel/10   3/26/12 6:43p vle
 * SW7420-2283: Fix build warning for 65nm builds.
 * 
 * Hydra_Software_Devel/9   3/6/12 1:48p vle
 * SW7425-2289: Disable AutoOn feature by default
 * 
 * Hydra_Software_Devel/8   3/1/12 1:19p vle
 * SW7425-2289: Additional update to make sure only AutoOn CEC is active
 * 
 * Hydra_Software_Devel/7   2/28/12 10:56a vle
 * SW7425-2289, SW7420-2256: Add more debug messages, fix compiler
 * warnings for 65nm platforms
 * 
 * Hydra_Software_Devel/6   2/24/12 4:06p vle
 * SW7425-2289: Merge to mainline
 * 
 * Hydra_Software_Devel/SW7425-2289/1   2/22/12 6:10p vle
 * SW7425-2289: Add support for CEC AutoOn
 * 
 * Hydra_Software_Devel/5   2/21/12 4:32p vle
 * SW7425-2279: Update for 65nm platform support.
 * 
 * Hydra_Software_Devel/4   2/17/12 3:44p vle
 * SW7425-2279: Merge to mainline
 * 
 * Hydra_Software_Devel/SW7425-2279/7   2/16/12 4:55p vle
 * SW7425-2279: Additional updates after 2nd review. Still need to rework
 * Standby/Resume function to use CEC autoOn feature.
 * 
 * Hydra_Software_Devel/SW7425-2279/6   2/15/12 3:47p vle
 * SW7425-2279: Additionals updates after a quick review.
 * 
 * Hydra_Software_Devel/SW7425-2279/5   2/15/12 3:05p vle
 * SW7425-2279: fix typo
 * 
 * Hydra_Software_Devel/SW7425-2279/4   2/14/12 5:56p vle
 * SW7425-2279: More updates improving implementations.
 * 
 * Hydra_Software_Devel/SW7425-2279/3   2/13/12 3:32p vle
 * SW7420-682: Merge to SW7425-2279 branch
 * 
 * Hydra_Software_Devel/SW7425-2279/2   2/9/12 3:35p vle
 * SW7425-2279: Update BCEC PI implementations. Use separate events for
 * transmit and receive.
 * 
 * Hydra_Software_Devel/SW7425-2279/1   1/30/12 7:14p vle
 * SW7425-2279: Fix incorrect return of received CEC message
 * 
 * Hydra_Software_Devel/SW7420-682/1   2/3/12 6:41p vle
 * SW7420-682: First attempt at enable Auto-on CEC. Code hasn't gone
 * through testing.
 * 
 * Hydra_Software_Devel/3   12/21/11 1:40p vle
 * SW7405-4781: merge to mainline
 * 
 * Hydra_Software_Devel/SW7405-4781/1   12/21/11 12:12p vle
 * SW7405-4781: allow CEC device type to be configurable
 * 
 * Hydra_Software_Devel/2   12/9/11 11:00a vle
 * SW7425-1140: fix comments and add GetDefaultSettings API
 * 
 * Hydra_Software_Devel/1   11/22/11 6:23p vle
 * SW7425-1140: Add BCEC PI support
 * 
 * Hydra_Software_Devel/SW7425-1140/2   11/22/11 3:50p rgreen
 * SW7425-1140: Add macros for debug support
 * 
 * Hydra_Software_Devel/SW7425-1140/1   11/16/11 12:14p vle
 * SW7425-1140: Add CEC PI support for 7425
 * 
 ***************************************************************************/

#include "bstd.h"
#include "berr_ids.h"

#include "bcec.h"
#include "bcec_priv.h"
#include "bcec_config.h"
#include "bavc_hdmi.h"


#if BCHP_PWR_SUPPORT
#include "bchp_pwr.h"
#endif


BDBG_MODULE(BCEC) ;
BDBG_OBJECT_ID(BCEC_P_Handle);

 
#define BCEC_CHECK_RC( rc, func )				  \
do												  \
{												  \
	if( (rc = BERR_TRACE(func)) != BERR_SUCCESS ) \
	{											  \
		goto done;								  \
	}											  \
} while(0)


#define BCEC_REG_OFFSET 0

#if BCEC_CONFIG_DEBUG_OPCODE

BCEC_OpcodeTextTable opCodeTextTable[BCEC_MAX_OPCODES] = {
{ BCEC_OpCode_FeatureAbort, 			"FeatureAbort" },			  
{ BCEC_OpCode_ImageViewOn,				"ImageViewOn" },			  
{ BCEC_OpCode_TunerStepIncrement,		"TunerStepIncrement" }, 	  
{ BCEC_OpCode_TunerStepDecrement,		"TunerStepDecrement" }, 	  
{ BCEC_OpCode_TunerDeviceStatus,		"TunerDeviceStatus" },		  
{ BCEC_OpCode_DiveTunerDeviceStatus,	"DiveTunerDeviceStatus" },	  
{ BCEC_OpCode_RecordOn, 				"RecordOn" },				  
{ BCEC_OpCode_RecordStatus, 			"RecordStatus" },			  
{ BCEC_OpCode_RecordOff,				"RecordOff" },				  
{ BCEC_OpCode_TextViewOn,				"TextViewOn" }, 			  
{ BCEC_OpCode_RecordTVScreen,			"RecordTVScreen" }, 		  
{ BCEC_OpCode_GiveDeckStatus,			"GiveDeckStatus" }, 		  
{ BCEC_OpCode_DecStatus,				"DecStatus" },				  
{ BCEC_OpCode_SetMenuLanguage,			"SetMenuLanguage" },		  
{ BCEC_OpCode_ClearAnalogueTimer,		"ClearAnalogueTimer" }, 	  
{ BCEC_OpCode_SetAnalogueTimer, 		"SetAnalogueTimer" },		  
{ BCEC_OpCode_TimerStatus,				"TimerStatus" },			  
{ BCEC_OpCode_Standby,					"Standby" },				  
{ BCEC_OpCode_Play, 					"Play" },					  
{ BCEC_OpCode_DeckControl,				"DeckControl" },			  
{ BCEC_OpCode_TimerClearedStatus,		"TimerClearedStatus" }, 	  
{ BCEC_OpCode_UserControlPressed,		"UserControlPressed" }, 	  
{ BCEC_OpCode_UserControlReleased,		"UserControlReleased" },	  
{ BCEC_OpCode_GiveOSDName,				"GiveOSDName" },			  
{ BCEC_OpCode_SetOSDName,				"SetOSDName" }, 			  
{ BCEC_OpCode_SetOSDString, 			"SetOSDString" },			  
{ BCEC_OpCode_SetTimerProgramTitle, 	"SetTimerProgramTitle" },	  
{ BCEC_OpCode_SystemAudioModeRequest,	"SystemAudioModeRequest" },   
{ BCEC_OpCode_GiveAudioStatus,			"GiveAudioStatus" },		  
{ BCEC_OpCode_SetSystemAudioMode,		"SetSystemAudioMode" }, 	  
{ BCEC_OpCode_ReportAudioStatus,		"ReportAudioStatus" },		  
{ BCEC_OpCode_GiveSystemAudioModeStatus,"GiveSystemAudioModeStatus" },
{ BCEC_OpCode_SystemAudioModeStatus,	"SystemAudioModeStatus" },	  
{ BCEC_OpCode_RoutingChange,			"RoutingChange" },			  
{ BCEC_OpCode_RoutingInformation,		"RoutingInformation" }, 	  
{ BCEC_OpCode_ActiveSource, 			"ActiveSource" },			  
{ BCEC_OpCode_GivePhysicalAddress,		"GivePhysicalAddress" },	  
{ BCEC_OpCode_ReportPhysicalAddress,	"ReportPhysicalAddress" },	  
{ BCEC_OpCode_RequestActiveSource,		"RequestActiveSource" },	  
{ BCEC_OpCode_SetStreamPath,			"SetStreamPath" },			  
{ BCEC_OpCode_DeviceVendorID,			"DeviceVendorID" }, 		  
{ BCEC_OpCode_VendorCommand,			"VendorCommand" },			  
{ BCEC_OpCode_VendorRemoteButtonDown,	"VendorRemoteButtonDown" },   
{ BCEC_OpCode_VendorRemoteButtonUp, 	"VendorRemoteButtonUp" },	  
{ BCEC_OpCode_GiveDeviceVendorID,		"GiveDeviceVendorID" }, 	  
{ BCEC_OpCode_MenuRequest,				"MenuRequest" },			  
{ BCEC_OpCode_MenuStatus,				"MenuStatus" }, 			  
{ BCEC_OpCode_GiveDevicePowerStatus,	"GiveDevicePowerStatus" },	  
{ BCEC_OpCode_ReportPowerStatus,		"ReportPowerStatus" },		  
{ BCEC_OpCode_GetMenuLanguage,			"GetMenuLanguage" },		  
{ BCEC_OpCode_SelectAnalogueService,	"SelectAnalogueService" },	  
{ BCEC_OpCode_SelectDigitalService, 	"SelectDigitalService" },	  
{ BCEC_OpCode_SetDigitalTimer,			"SetDigitalTimer" },		  
{ BCEC_OpCode_ClearDigitalTimer,		"ClearDigitalTimer" },		  
{ BCEC_OpCode_SetAudioRate, 			"SetAudioRate" },			  
{ BCEC_OpCode_InActiveSource,			"InActiveSource" }, 		  
{ BCEC_OpCode_CECVersion,				"CECVersion" }, 			  
{ BCEC_OpCode_GetCECVersion,			"GetCECVersion" },			  
{ BCEC_OpCode_VendorCommandWithID,		"VendorCommandWithID" },	  
{ BCEC_OpCode_ClearExternalTimer,		"ClearExternalTimer" }, 	  
{ BCEC_OpCode_SetExternalTimer, 		"SetExternalTimer" },		  
{ BCEC_OpCode_Abort,					"Abort" } 
} ;

static char unsupportedOpCode[] = "Unsupported OpCode" ;
static const char *BCEC_OpcodeToString(uint8_t cecOpCode)
{
	uint8_t i ;

	i = 0 ;
	while  (i < BCEC_MAX_OPCODES)
	{
		if (opCodeTextTable[i].opCode == cecOpCode)
			return opCodeTextTable[i].opText ;
		i++ ;
	}
	return unsupportedOpCode ;
}
#endif 


/******************************************************************************
Summary:
Interrupt Callback Table to describe interrupt Names, ISRs, and Masks 
*******************************************************************************/
typedef struct BCEC_P_InterruptCbTable
{
	BINT_Id 		  IntrId;
	BINT_CallbackFunc pfCallback;
	int 			  iParam2; 
	bool			  enable ; /* debug purposes */
} BCEC_P_InterruptCbTable ;


#define BCEC_INT_CB_DEF(intr,  id, enable) \
	{intr, BCEC_P_HandleInterrupt_isr, id, enable},


static const BCEC_P_InterruptCbTable BCEC_Interrupts[] =
{
#if BCEC_CONFIG_DUAL_INTERRUPT
	BCEC_INT_CB_DEF(BCHP_INT_ID_CEC_RCVD_INTR_TX,
		BCEC_INTR_eRECEIVED, 1)
	BCEC_INT_CB_DEF(BCHP_INT_ID_CEC_SENT_INTR_TX,
		BCEC_INTR_eSENT, 1)

#elif BCEC_CONFIG_SINGLE_INTERRUPT
	BCEC_INT_CB_DEF(BCHP_INT_ID_CEC_INTR_TX,
		BCEC_INTR_eCEC, 1)
		
#else	/* legacy platforms 65nm and earlier */
	BCEC_INT_CB_DEF(BCHP_INT_ID_CEC_INTR,
		BCEC_INTR_eCEC, 1)
#endif		
} ;


/********************
*	PRIVATE APIs 	*
*********************/	
static void BCEC_P_Initialize(BCEC_Handle hCEC) 
{
	BREG_Handle hRegister ;
	uint32_t Register ;
	uint32_t ulOffset ;

	BDBG_ENTER(BCEC_P_Initialize) ;

	hRegister = hCEC->stDependencies.hRegister ;
	ulOffset = BCEC_REG_OFFSET ;

	/*************************	
	** adjust default CEC timing **
	**************************/
	Register = BREG_Read32(hRegister, REGADDR_CEC_CNTRL_1 + ulOffset) ;
	Register &= ~ BCHP_MASK_CEC_CNTRL(CEC_CNTRL_1, DIV_CLK_CNT);
	Register |= BCHP_FIELD_DATA(REGNAME_CEC_CNTRL_1, DIV_CLK_CNT, 
				BCEC_CNTRL_1_DIV_CLK_CNT_VALUE);
	BREG_Write32(hRegister, REGADDR_CEC_CNTRL_1 + ulOffset, Register) ;

	Register = BREG_Read32(hRegister, REGADDR_CEC_CNTRL_2 + ulOffset) ;
	Register &= 
		~(BCHP_MASK_CEC_CNTRL(CEC_CNTRL_2, CNT_TO_400_US)
		| BCHP_MASK_CEC_CNTRL(CEC_CNTRL_2, CNT_TO_600_US)
		| BCHP_MASK_CEC_CNTRL(CEC_CNTRL_2, CNT_TO_800_US)
		| BCHP_MASK_CEC_CNTRL(CEC_CNTRL_2, CNT_TO_1300_US)
		| BCHP_MASK_CEC_CNTRL(CEC_CNTRL_2, CNT_TO_1500_US));
	
	Register |= 
		  BCHP_FIELD_DATA(REGNAME_CEC_CNTRL_2, CNT_TO_400_US, 
			BCEC_CNTRL_2_CNT_TO_400_US_VALUE)
		| BCHP_FIELD_DATA(REGNAME_CEC_CNTRL_2, CNT_TO_600_US, 
			BCEC_CNTRL_2_CNT_TO_600_US_VALUE)	
		| BCHP_FIELD_DATA(REGNAME_CEC_CNTRL_2, CNT_TO_800_US, 
			BCEC_CNTRL_2_CNT_TO_800_US_VALUE)
		| BCHP_FIELD_DATA(REGNAME_CEC_CNTRL_2, CNT_TO_1300_US, 
			BCEC_CNTRL_2_CNT_TO_1300_US_VALUE)
		| BCHP_FIELD_DATA(REGNAME_CEC_CNTRL_2, CNT_TO_1500_US, 
			BCEC_CNTRL_2_CNT_TO_1500_US_VALUE);
	BREG_Write32(hRegister, REGADDR_CEC_CNTRL_2 + ulOffset, Register) ;


	Register = BREG_Read32(hRegister, REGADDR_CEC_CNTRL_3 + ulOffset) ;
	Register &= 
		~(BCHP_MASK_CEC_CNTRL(CEC_CNTRL_3, CNT_TO_1700_US)
		| BCHP_MASK_CEC_CNTRL(CEC_CNTRL_3, CNT_TO_2050_US)
		| BCHP_MASK_CEC_CNTRL(CEC_CNTRL_3, CNT_TO_2400_US)
		| BCHP_MASK_CEC_CNTRL(CEC_CNTRL_3, CNT_TO_2750_US));
	
	Register |= 
		  BCHP_FIELD_DATA(REGNAME_CEC_CNTRL_3, CNT_TO_1700_US, 
			BCEC_CNTRL_3_CNT_TO_1700_US_VALUE)
		| BCHP_FIELD_DATA(REGNAME_CEC_CNTRL_3, CNT_TO_2050_US, 
			BCEC_CNTRL_3_CNT_TO_2050_US_VALUE)	
		| BCHP_FIELD_DATA(REGNAME_CEC_CNTRL_3, CNT_TO_2400_US, 
			BCEC_CNTRL_3_CNT_TO_2400_US_VALUE)
		| BCHP_FIELD_DATA(REGNAME_CEC_CNTRL_3, CNT_TO_2750_US, 
			BCEC_CNTRL_3_CNT_TO_2750_US_VALUE);
	BREG_Write32(hRegister, REGADDR_CEC_CNTRL_3 + ulOffset, Register) ;

	
	Register = BREG_Read32(hRegister, REGADDR_CEC_CNTRL_4 + ulOffset) ;
	Register &= 
		~(BCHP_MASK_CEC_CNTRL(CEC_CNTRL_4, CNT_TO_3500_US)
		| BCHP_MASK_CEC_CNTRL(CEC_CNTRL_4, CNT_TO_3600_US)
		| BCHP_MASK_CEC_CNTRL(CEC_CNTRL_4, CNT_TO_3900_US)		
		| BCHP_MASK_CEC_CNTRL(CEC_CNTRL_4, CNT_TO_4300_US));
	
	Register |= 
		  BCHP_FIELD_DATA(REGNAME_CEC_CNTRL_4, CNT_TO_3500_US, 
			BCEC_CNTRL_4_CNT_TO_3500_US_VALUE)
		| BCHP_FIELD_DATA(REGNAME_CEC_CNTRL_4, CNT_TO_3600_US, 
			BCEC_CNTRL_4_CNT_TO_3600_US_VALUE)	
		| BCHP_FIELD_DATA(REGNAME_CEC_CNTRL_4, CNT_TO_3900_US, 
			BCEC_CNTRL_4_CNT_TO_3900_US_VALUE)
		| BCHP_FIELD_DATA(REGNAME_CEC_CNTRL_4, CNT_TO_4300_US, 
			BCEC_CNTRL_4_CNT_TO_4300_US_VALUE);
	BREG_Write32(hRegister, REGADDR_CEC_CNTRL_4 + ulOffset, Register) ;


	Register = BREG_Read32(hRegister, REGADDR_CEC_CNTRL_5 + ulOffset) ;
	Register &= 
		~(BCHP_MASK_CEC_CNTRL(CEC_CNTRL_5, CNT_TO_4500_US)
		| BCHP_MASK_CEC_CNTRL(CEC_CNTRL_5, CNT_TO_4700_US));
 
	Register |= 
		  BCHP_FIELD_DATA(REGNAME_CEC_CNTRL_5, CNT_TO_4500_US, 
			BCEC_CNTRL_5_CNT_TO_4500_US_VALUE)
		| BCHP_FIELD_DATA(REGNAME_CEC_CNTRL_5, CNT_TO_4700_US, 
				BCEC_CNTRL_5_CNT_TO_4700_US_VALUE) ;
	BREG_Write32(hRegister, REGADDR_CEC_CNTRL_5 + ulOffset, Register) ;


#if BCEC_CONFIG_DEBUG_CEC_TIMING 
	BDBG_WRN(("CNT_TO_400_US_VALUE %d", BCEC_CNTRL_2_CNT_TO_400_US_VALUE )) ;
	BDBG_WRN(("CNT_TO_600_US_VALUE %d", BCEC_CNTRL_2_CNT_TO_600_US_VALUE )) ;
	BDBG_WRN(("CNT_TO_800_US_VALUE %d", BCEC_CNTRL_2_CNT_TO_800_US_VALUE )) ;
	BDBG_WRN(("CNT_TO_1300_US_VALUE %d", BCEC_CNTRL_2_CNT_TO_1300_US_VALUE)) ;
	BDBG_WRN(("CNT_TO_1500_US_VALUE %d", BCEC_CNTRL_2_CNT_TO_1500_US_VALUE)) ;


	BDBG_WRN(("CNT_TO_1700_US_VALUE %d", BCEC_CNTRL_3_CNT_TO_1700_US_VALUE));
	BDBG_WRN(("CNT_TO_2050_US_VALUE %d", BCEC_CNTRL_3_CNT_TO_2050_US_VALUE)) ;
	BDBG_WRN(("CNT_TO_2400_US_VALUE %d", BCEC_CNTRL_3_CNT_TO_2400_US_VALUE)) ;
	BDBG_WRN(("CNT_TO_2750_US_VALUE %d", BCEC_CNTRL_3_CNT_TO_2750_US_VALUE)) ;
																		  
	BDBG_WRN(("CNT_TO_3500_US_VALUE %d", BCEC_CNTRL_4_CNT_TO_3500_US_VALUE)) ;
	BDBG_WRN(("CNT_TO_3600_US_VALUE %d", BCEC_CNTRL_4_CNT_TO_3600_US_VALUE)) ;
	BDBG_WRN(("CNT_TO_3900_US_VALUE %d", BCEC_CNTRL_4_CNT_TO_3900_US_VALUE)) ;
	BDBG_WRN(("CNT_TO_4300_US_VALUE", BCEC_CNTRL_4_CNT_TO_4300_US_VALUE)) ;
																		  
																		  
	BDBG_WRN(("CNT_TO_4500_US_VALUE %d", BCEC_CNTRL_5_CNT_TO_4500_US_VALUE)) ;
	BDBG_WRN(("CNT_TO_4700_US_VALUE %d", BCEC_CNTRL_5_CNT_TO_4700_US_VALUE)) ;
#endif 

	/* set flag to indicate first CEC message */
	hCEC->firstCecMessage = true ;
	
	BDBG_LEAVE(BCEC_P_Initialize) ; 
}


/***************************************************************************
BERR_Code BCEC_P_ResetCore_isr
Summary: Reset CEC core
****************************************************************************/
static void BCEC_P_ResetCore_isr (BCEC_Handle hCEC, bool bReset)
{
	uint32_t Register;
	BREG_Handle hRegister ;
	uint32_t ulOffset;
	
	hRegister = hCEC->stDependencies.hRegister ;
	ulOffset = BCEC_REG_OFFSET ;


	Register = BREG_Read32(hRegister, REGADDR_CEC_CNTRL_5 + ulOffset) ;

#if BCEC_CONFIG_40NM_SUPPORT || BCEC_CONFIG_28NM_SUPPORT
	Register &= ~(
		  BCHP_MASK_DVP(CEC_CNTRL_5, CEC_TX_SW_INIT)
		| BCHP_MASK_DVP(CEC_CNTRL_5, CEC_RX_SW_INIT));
	
	Register |= 
		  BCHP_FIELD_DATA(REGNAME_CEC_CNTRL_5, CEC_TX_SW_INIT, bReset)
		| BCHP_FIELD_DATA(REGNAME_CEC_CNTRL_5, CEC_RX_SW_INIT, bReset);
	
#else
	Register &= ~(
		  BCHP_MASK_CEC_CNTRL(CEC_CNTRL_5, CEC_TX_SW_RESET)
		| BCHP_MASK_CEC_CNTRL(CEC_CNTRL_5, CEC_RX_SW_RESET));
	
	Register |= 
		  BCHP_FIELD_DATA(REGNAME_CEC_CNTRL_5, CEC_TX_SW_RESET, bReset)
		| BCHP_FIELD_DATA(REGNAME_CEC_CNTRL_5, CEC_RX_SW_RESET, bReset);
#endif	

	BREG_Write32(hRegister, REGADDR_CEC_CNTRL_5 + ulOffset, Register) ;


#ifdef BCEC_CONFIG_CEC_USE_PAD_SW_RESET
	Register = BREG_Read32(hRegister, REGADDR_CEC_CNTRL_5 + ulOffset) ;
	Register &= ~(BCHP_MASK_CEC_CNTRL(CEC_CNTRL_5, CEC_PAD_SW_RESET));

	Register |= BCHP_FIELD_DATA(REGNAME_CEC_CNTRL_5, CEC_PAD_SW_RESET, bReset);
	BREG_Write32(hRegister, REGADDR_CEC_CNTRL_5 + ulOffset, Register) ; 
#endif

	return;
}


/***************************************************************************
BERR_Code BCEC_P_EnableReceive_isr
Summary: Enable Receive - isr
****************************************************************************/
static void BCEC_P_EnableReceive_isr(
	BCEC_Handle hCEC	 /* [in] HDMI handle */
) 
{ 
	BREG_Handle hRegister ;
	uint32_t Register ;
	uint32_t ulOffset ;

	BDBG_ENTER(BCEC_P_EnableReceive_isr);
	
	hRegister = hCEC->stDependencies.hRegister ;
	ulOffset = BCEC_REG_OFFSET ;
	
	/* Enable Receive */
	Register = BREG_Read32(hRegister, REGADDR_CEC_CNTRL_1 + ulOffset) ;
	
	Register |= BCHP_MASK_CEC_CNTRL(CEC_CNTRL_1, CLEAR_RECEIVE_OFF) ;
	BREG_Write32(hRegister, REGADDR_CEC_CNTRL_1 + ulOffset, Register) ;  /* Wr 1 */
	
	Register &= ~ BCHP_MASK_CEC_CNTRL(CEC_CNTRL_1, CLEAR_RECEIVE_OFF) ;
	BREG_Write32(hRegister, REGADDR_CEC_CNTRL_1 + ulOffset, Register) ;  /* Wr 0 */

	BDBG_LEAVE(BCEC_P_EnableReceive_isr);
	return ;
}


/********************
*	PUBLIC APIs		*
*********************/	

/******************************************************************************
BERR_Code BCEC_Open
Summary: Open/Initialize the CEC device 
*******************************************************************************/
BERR_Code BCEC_Open(
   BCEC_Handle *phCEC,	  /* [out] CEC handle */
   const BCEC_Dependencies *pstDependencies
) 
{
	BERR_Code	rc = BERR_SUCCESS;

	BCHP_Handle hChip ;
	BINT_Handle hInterrupt ;
	BREG_Handle hRegister ;
	BCEC_Handle hCEC = NULL ;

	uint32_t Register ;
	uint8_t i ;

	BDBG_ENTER(BCEC_Open) ;

	
	/* verify parameters */
	BDBG_ASSERT(pstDependencies->hChip) ;
	BDBG_ASSERT(pstDependencies->hRegister) ;
	BDBG_ASSERT(pstDependencies->hInterrupt) ;

	hChip       = pstDependencies->hChip ;
	hInterrupt = pstDependencies->hInterrupt ;
	hRegister  = pstDependencies->hRegister ;

	/* create the CEC Handle */
	hCEC = BKNI_Malloc(sizeof(BCEC_P_Handle)) ;
	if (!hCEC)
	{
		BDBG_ERR(("Unable to allocate memory for CEC Handle")) ;
		rc = BERR_OUT_OF_SYSTEM_MEMORY ;
		BERR_TRACE(rc);
		goto done ;
	}

	/* zero out all memory associated with the HDMI Device Handle before using */
	BKNI_Memset(hCEC, 0, sizeof(BCEC_P_Handle)) ;
	BDBG_OBJECT_SET(hCEC, BCEC_P_Handle) ;

	/* assign the handles passed in as parameters */
	BKNI_Memcpy(&hCEC->stDependencies, pstDependencies, sizeof(BCEC_Dependencies)) ;

#ifdef BCHP_PWR_RESOURCE_HDMI_TX_CEC
	BCHP_PWR_AcquireResource(hChip, BCHP_PWR_RESOURCE_HDMI_TX_CEC);
#endif

	/* Initialize CEC core */
	BCEC_P_Initialize(hCEC);

	/* set default CEC address to 15 */
	Register = BREG_Read32(hRegister, REGADDR_CEC_CNTRL_1) ;
	Register &= ~ BCHP_MASK_CEC_CNTRL(CEC_CNTRL_1, CEC_ADDR);
	Register |= BCHP_FIELD_DATA(REGNAME_CEC_CNTRL_1, CEC_ADDR, 
							BAVC_HDMI_CEC_AllDevices_eUnRegistered);
	BREG_Write32(hRegister, REGADDR_CEC_CNTRL_1, Register) ;


	hCEC->stSettings.CecLogicalAddr = BCEC_CONFIG_UNINITIALIZED_LOGICAL_ADDR;

#if BCEC_CONFIG_40NM_SUPPORT	/* the default value works for 40nm rev2 */
	Register = BREG_Read32(hRegister, REGADDR_CEC_CNTRL_6) ;
		Register &= ~ ( 
			  BCHP_MASK_CEC_CNTRL(CEC_CNTRL_6, CEC_ADDR_1)
			| BCHP_MASK_CEC_CNTRL(CEC_CNTRL_6, CEC_ADDR_2));
		
		Register |= ( 
			  BCHP_FIELD_DATA(REGNAME_CEC_CNTRL_6, CEC_ADDR_1, 
			    BAVC_HDMI_CEC_AllDevices_eUnRegistered)
			| BCHP_FIELD_DATA(REGNAME_CEC_CNTRL_6, CEC_ADDR_2, 
			    BAVC_HDMI_CEC_AllDevices_eUnRegistered));
	BREG_Write32(hRegister, REGADDR_CEC_CNTRL_6, Register) ;

#elif BCEC_CONFIG_65NM_SUPPORT
	Register = BREG_Read32(hRegister, BCHP_HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL) ;
		Register &= ~BCHP_MASK(HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL, PWRDN_CEC) ;
	BREG_Write32(hRegister, BCHP_HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL, Register) ;
#endif	


	/* Create Events for use with Interrupts */
	BCEC_CHECK_RC(rc, BKNI_CreateEvent(&(hCEC->BCEC_EventCec_Transmitted))) ;
	BCEC_CHECK_RC(rc, BKNI_CreateEvent(&(hCEC->BCEC_EventCec_Received))) ;
	
	/* register/enable interrupt callbacks */
	for( i = 0; i < BCEC_MAKE_INTR_ENUM(LAST) ; i++ )
	{
		/* 
		** DEBUG
		** Create ALL interrupt callbacks
		** enable debug callbacks as needed;
		*/
	
		BCEC_CHECK_RC( rc, BINT_CreateCallback(
			&(hCEC->hCallback[i]), hInterrupt, 
			BCEC_Interrupts[i].IntrId,
			BCEC_P_HandleInterrupt_isr, (void *) hCEC, i ));
			
		/* clear interrupt callback */
		BCEC_CHECK_RC(rc, BINT_ClearCallback( hCEC->hCallback[i])) ;
		
		/* now enable it; if specified for startup */	
		if (!BCEC_Interrupts[i].enable)
			continue ;
			
		BCEC_CHECK_RC( rc, BINT_EnableCallback( hCEC->hCallback[i] ) );
	}

	
	/* keep created pointer */
	*phCEC = hCEC ;

done:
	if (rc != BERR_SUCCESS) 
	{
		if (hCEC != NULL)	
		{
			/* destroy cec events if needed */
			if (hCEC->BCEC_EventCec_Transmitted!= NULL)
				BKNI_DestroyEvent(hCEC->BCEC_EventCec_Transmitted);

			if (hCEC->BCEC_EventCec_Received!= NULL)
				BKNI_DestroyEvent(hCEC->BCEC_EventCec_Received);

			for( i = 0; i < BCEC_MAKE_INTR_ENUM(LAST); i++ )
			{
				if (hCEC->hCallback[i]) {
					/* all interrupts are now created; disable and destroy all on close */
					BINT_DisableCallback( hCEC->hCallback[i] );
					BINT_DestroyCallback( hCEC->hCallback[i] );
				}
			}
			
			BKNI_Free(hCEC);	
		}
		*phCEC=NULL;

		
#ifdef BCHP_PWR_RESOURCE_HDMI_TX_CEC
		/* on failure, power everything down */
		BCHP_PWR_ReleaseResource(hChip, BCHP_PWR_RESOURCE_HDMI_TX_CEC);
#endif
	}
	
	BDBG_LEAVE(BCEC_Open);
	return rc;
	
}


/******************************************************************************
BERR_Code BCEC_Close
Summary: Close the CEC handle
*******************************************************************************/
void BCEC_Close(
   BCEC_Handle hCEC  /* [in] CEC handle */
)
{
	uint32_t i ;

	BDBG_ENTER(BCEC_Close) ;
	BDBG_OBJECT_ASSERT(hCEC, BCEC_P_Handle) ;

	/* Disable and Destroy the HDMI Callbacks */
	for( i = 0; i < BCEC_MAKE_INTR_ENUM(LAST); i++ )
	{
		if (hCEC->hCallback[i]) {	
			/* all interrupts are now created; disable and destroy all on close */
			BINT_DisableCallback( hCEC->hCallback[i] );
			BINT_DestroyCallback( hCEC->hCallback[i] );
		}
	}

	/* destroy Cec events if needed */
	if (hCEC->BCEC_EventCec_Transmitted!= NULL)
		BKNI_DestroyEvent(hCEC->BCEC_EventCec_Transmitted);
	
	if (hCEC->BCEC_EventCec_Received!= NULL)
		BKNI_DestroyEvent(hCEC->BCEC_EventCec_Received);

#if BCEC_CONFIG_AUTO_ON_SUPPORT
	/* if autoOn feature is enabled, do not hold CEC core in reset */
	if (hCEC->stSettings.enableAutoOn)
		goto done;
#endif	

	BKNI_EnterCriticalSection();
		/* Hold CEC core in reset; i.e. CEC will not respond */
		BCEC_P_ResetCore_isr (hCEC, 1);
	BKNI_LeaveCriticalSection();


#if BCEC_CONFIG_AUTO_ON_SUPPORT
done:	
#endif

#ifdef BCHP_PWR_RESOURCE_HDMI_TX_CEC
	/* release the CEC	*/
	BCHP_PWR_ReleaseResource(hCEC->stDependencies.hChip, BCHP_PWR_RESOURCE_HDMI_TX_CEC);
#endif


	/* free memory associated with the CEC handle */
	BKNI_Memset(hCEC, 0, sizeof(BCEC_P_Handle)) ;
	BKNI_Free( (void *) hCEC) ;
	

	BDBG_LEAVE(BCEC_Close) ;
	return ;
}


/***************************************************************************
BERR_Code BCEC_GetEventHandle
Summary: Get the event handle for checking CEC events.
****************************************************************************/
BERR_Code BCEC_GetEventHandle(
   BCEC_Handle hCEC,			/* [in] HDMI handle */
   BCEC_EventType eEventType,
   BKNI_EventHandle *pBCECEvent /* [out] event handle */
)
{
	BERR_Code	   rc = BERR_SUCCESS;

	BDBG_ENTER(BCEC_GetEventHandle) ;
	BDBG_OBJECT_ASSERT(hCEC, BCEC_P_Handle) ;

	switch (eEventType)
	{
	case BCEC_EventCec_eTransmitted:
		*pBCECEvent = hCEC->BCEC_EventCec_Transmitted ;
		break ;

	case BCEC_EventCec_eReceived:
		*pBCECEvent = hCEC->BCEC_EventCec_Received ;
		break ;

	default :
		BDBG_ERR(("Invalid Event Type: %d", eEventType)) ;
		rc = BERR_INVALID_PARAMETER ;
		goto done ;
	}


done:
	BDBG_LEAVE(BCEC_GetEventHandle) ;
	return rc ;
}


void BCEC_GetDefaultSettings(
	BCEC_Settings *pstDefaultCecSettings
)
{
	BKNI_Memset(pstDefaultCecSettings,0,sizeof(BCEC_Settings));

	pstDefaultCecSettings->enable = true;
	pstDefaultCecSettings->CecLogicalAddr = BCEC_CONFIG_UNINITIALIZED_LOGICAL_ADDR;
	pstDefaultCecSettings->CecPhysicalAddr[0] = 0xFF;
	pstDefaultCecSettings->CecPhysicalAddr[1] = 0xFF;
	pstDefaultCecSettings->eDeviceType = BCEC_CONFIG_DEVICE_TYPE;
	pstDefaultCecSettings->enableAutoOn = false;
	pstDefaultCecSettings->cecVersion = BCEC_VERSION;
	return ;
}


BERR_Code BCEC_GetSettings(
	BCEC_Handle hCEC,		  /* [in] CEC handle */
	BCEC_Settings *pstCecSettings
)
{
	BERR_Code rc = BERR_SUCCESS ;
	BREG_Handle hRegister ;
	uint32_t ulOffset ;

	BDBG_ENTER(BCEC_GetSettings);
	BDBG_OBJECT_ASSERT(hCEC, BCEC_P_Handle) ;

	BKNI_Memset(pstCecSettings, 0, sizeof(BCEC_Settings));

	hRegister = hCEC->stDependencies.hRegister ;
	ulOffset = BCEC_REG_OFFSET ;

	if (hCEC->stSettings.CecLogicalAddr == BCEC_CONFIG_UNINITIALIZED_LOGICAL_ADDR) {	
		BDBG_WRN(("CEC Logical Address Uninitialized"));	
	}

	pstCecSettings->enable = hCEC->stSettings.enable;
	pstCecSettings->CecLogicalAddr = hCEC->stSettings.CecLogicalAddr;	
	BKNI_Memcpy(pstCecSettings->CecPhysicalAddr, &hCEC->stSettings.CecPhysicalAddr[0], 2); 
	pstCecSettings->eDeviceType = hCEC->stSettings.eDeviceType;
	pstCecSettings->enableAutoOn = hCEC->stSettings.enableAutoOn;
	pstCecSettings->cecVersion = hCEC->stSettings.cecVersion;
	
	
	BDBG_MSG(("BCM%d Device's Logical Addr: %d", BCHP_CHIP, pstCecSettings->CecLogicalAddr));
	BDBG_MSG(("BCM%d Device's Physical Addr: %02X %02X", BCHP_CHIP,
		pstCecSettings->CecPhysicalAddr[0], pstCecSettings->CecPhysicalAddr[1])) ;


	BDBG_LEAVE(BCEC_GetSettings);
	return rc ;
}


BERR_Code BCEC_SetSettings(
	BCEC_Handle hCEC,		  /* [in] CEC handle */
	const BCEC_Settings *pstCecSettings
)
{
	BERR_Code rc = BERR_SUCCESS ;
	BREG_Handle hRegister ;
	uint32_t Register ;
	uint32_t ulOffset ;

	BDBG_ENTER(BCEC_SetSettings);
	BDBG_OBJECT_ASSERT(hCEC, BCEC_P_Handle) ;

	hRegister = hCEC->stDependencies.hRegister ;
	ulOffset = BCEC_REG_OFFSET ;
	
	BKNI_EnterCriticalSection();
		if (pstCecSettings->enable)
		{
			/* a change in logical address require a reset of the CEC core */
			if ((pstCecSettings->CecLogicalAddr != hCEC->stSettings.CecLogicalAddr)
			|| (!hCEC->stSettings.enable))
			{
				Register = BREG_Read32(hRegister, REGADDR_CEC_CNTRL_1 + ulOffset) ;
					Register &= ~ BCHP_MASK_CEC_CNTRL(CEC_CNTRL_1, CEC_ADDR) ;
					Register |= BCHP_FIELD_DATA(REGNAME_CEC_CNTRL_1, CEC_ADDR, pstCecSettings->CecLogicalAddr) ;
				BREG_Write32(hRegister, REGADDR_CEC_CNTRL_1 + ulOffset, Register) ;

				/* if current state is enabled */
				if (hCEC->stSettings.enable)
				{
					/* toggle reset CEC core */
					BCEC_P_ResetCore_isr (hCEC, 1);
					BCEC_P_ResetCore_isr (hCEC, 0);
				}
			}	
		}

		/* only enable/disable CEC core when there's a change  */
		if (hCEC->stSettings.enable != pstCecSettings->enable) {
			BCEC_P_ResetCore_isr (hCEC, !pstCecSettings->enable);
		}
	BKNI_LeaveCriticalSection();	


	/* Save settings */
	hCEC->stSettings.enable = pstCecSettings->enable;
	BKNI_Memcpy(hCEC->stSettings.CecPhysicalAddr, pstCecSettings->CecPhysicalAddr, 2) ;
	hCEC->stSettings.eDeviceType = pstCecSettings->eDeviceType;
	hCEC->stSettings.CecLogicalAddr = pstCecSettings->CecLogicalAddr;		
	hCEC->stSettings.enableAutoOn = pstCecSettings->enableAutoOn;
	hCEC->stSettings.cecVersion = pstCecSettings->cecVersion;
	
	/* Reset FirstCecMessage */
	hCEC->firstCecMessage = true; 

	BDBG_LEAVE(BCEC_SetSettings);	
	return rc ;
}


BERR_Code BCEC_GetTransmitMessageStatus(
	BCEC_Handle hCEC,	   /* [in] CEC handle */
	BCEC_MessageStatus *pstCecMessageStatus
)	
{
	BERR_Code rc = BERR_SUCCESS;
	
	BDBG_ENTER(BCEC_GetTransmitMessageStatus);
	BDBG_OBJECT_ASSERT(hCEC, BCEC_P_Handle) ;
	BDBG_ASSERT(pstCecMessageStatus);

	BKNI_EnterCriticalSection();
		pstCecMessageStatus->uiStatus = hCEC->lastTransmitMessageStatus.uiStatus;
		pstCecMessageStatus->uiMessageLength = hCEC->lastTransmitMessageStatus.uiMessageLength;
		pstCecMessageStatus->uiEOM = hCEC->lastTransmitMessageStatus.uiEOM;
	BKNI_LeaveCriticalSection();

	BDBG_LEAVE(BCEC_GetTransmitMessageStatus);
	return rc;
}


BERR_Code BCEC_GetReceivedMessageStatus(
	BCEC_Handle hCEC,	   /* [in] CEC handle */
	BCEC_MessageStatus *pstCecMessageStatus
)	
{
	BERR_Code rc = BERR_SUCCESS;

	BDBG_ENTER(BCEC_GetReceivedMessageStatus);
	BDBG_OBJECT_ASSERT(hCEC, BCEC_P_Handle) ;
	BDBG_ASSERT(pstCecMessageStatus);
	
	BKNI_EnterCriticalSection();
		pstCecMessageStatus->uiStatus = hCEC->lastReceivedMessageStatus.uiStatus;
		pstCecMessageStatus->uiMessageLength = hCEC->lastReceivedMessageStatus.uiMessageLength;
		pstCecMessageStatus->uiEOM = hCEC->lastReceivedMessageStatus.uiEOM;
	BKNI_LeaveCriticalSection();
	
	BDBG_LEAVE(BCEC_GetReceivedMessageStatus);
	return rc;
}


BERR_Code BCEC_PingLogicalAddr(
   BCEC_Handle hCEC,	  /* [in] HDMI handle */
   uint8_t uiLogicalAddr	/* [in] device logical address */
) 
{
	BERR_Code rc = BERR_SUCCESS ;
	BREG_Handle hRegister ;
	uint32_t Register ;
	uint32_t ulOffset ;
	uint8_t i ;

	BDBG_ENTER(BCEC_PingLogicalAddr);
	BDBG_OBJECT_ASSERT(hCEC, BCEC_P_Handle) ;

	hRegister = hCEC->stDependencies.hRegister ;
	ulOffset = BCEC_REG_OFFSET ;
	
	BKNI_EnterCriticalSection();
		if (hCEC->firstCecMessage)
		{
			hCEC->firstCecMessage = false ;
		}
		else
		{
			/* verify no message is currently being transmitted */
			Register = BREG_Read32(hRegister, REGADDR_CEC_CNTRL_1 + ulOffset) ;
			i = BCHP_GET_FIELD_DATA(Register, REGNAME_CEC_CNTRL_1, TX_EOM) ;
			if (i == 0)
			{
				BDBG_WRN(("Transmit CEC is Busy (TX_EOM: %d)!!... retry Ping later", i)) ;
				rc = BCEC_TRANSMIT_BUSY ;
				goto done ;
			}
		}
		
		Register = 0 ;
		/* load the first nibble with the source/destination addr */
		Register = (uiLogicalAddr << 4) | uiLogicalAddr ;
		BDBG_MSG(("CecMsg[00]: Initiator %02X, Destination %02X", (Register >> 4),(Register & 0x0F))) ;

		/* write the first nibble in case there is no PayLoad i.e zero length message */
		BREG_Write32(hRegister, REGADDR_CEC_TX_DATA_1 + ulOffset, Register) ;
		
		/* set up the message length (=0) before xmit */
		Register = BREG_Read32(hRegister, REGADDR_CEC_CNTRL_1 + ulOffset) ;
		Register &= ~ BCHP_MASK_CEC_CNTRL(CEC_CNTRL_1, MESSAGE_LENGTH) ;
		Register |= BCHP_FIELD_DATA(REGNAME_CEC_CNTRL_1, MESSAGE_LENGTH, 0) ;
		BREG_Write32(hRegister, REGADDR_CEC_CNTRL_1 + ulOffset, Register) ;
		
		/* toggle the start xmit bit */ 
		Register &=  ~ BCHP_MASK_CEC_CNTRL(CEC_CNTRL_1, START_XMIT_BEGIN) ;
		BREG_Write32(hRegister, REGADDR_CEC_CNTRL_1 + ulOffset, Register) ;   /* 0 */
		
		Register |= BCHP_FIELD_DATA(REGNAME_CEC_CNTRL_1, START_XMIT_BEGIN, 1) ;
		BREG_Write32(hRegister, REGADDR_CEC_CNTRL_1 + ulOffset, Register) ;   /* 1 */
done:
	BKNI_LeaveCriticalSection();
	
	BDBG_LEAVE(BCEC_PingLogicalAddr);
	return rc ; 
}



BERR_Code BCEC_XmitMessage(
	BCEC_Handle hCEC,	  /* [in] CEC handle */
	const BAVC_HDMI_CEC_MessageData *pMessageData 	/* [in] ptr to storage for CEC msg */
)
{
	BERR_Code rc = BERR_SUCCESS ;
	BREG_Handle hRegister ;
	uint32_t Register ;
	uint32_t ulOffset ;
	uint32_t TxDataRegisterOffset ;
	uint8_t i, j ;
	uint8_t XmitMessageLength ;
	
#if BCEC_CONFIG_DEBUG_MESSAGE_TX
	/* allocate 3 bytes  for each OpCode / Parameter followed by a space i.e. "%02X "
		Also allow for message header byte */
	char XmitMessage[3 *(BCEC_CONFIG_P_MAX_MESSAGE_BUFFER + 1)];
	uint8_t debugMsgOffset = 0;
#endif	

	BDBG_ENTER(BCEC_XmitMessage);
	BDBG_OBJECT_ASSERT(hCEC, BCEC_P_Handle) ;

	hRegister = hCEC->stDependencies.hRegister ;
	ulOffset = BCEC_REG_OFFSET ;
	if (pMessageData->messageLength > BAVC_HDMI_CEC_MAX_XMIT_LENGTH)  
	{
		/* configure for Continuous Mode */
		BDBG_ERR(("CEC Continuous Mode not implemented yet")) ;
		rc = BCEC_NOT_IMPLEMENTED ;
		BERR_TRACE(rc);
		goto done ;
	}

	if (hCEC->stSettings.CecLogicalAddr == BCEC_CONFIG_UNINITIALIZED_LOGICAL_ADDR)
	{
		BDBG_WRN(("CEC Logical Address has not been initialized; Unable to Send message")) ;
		rc = BERR_NOT_INITIALIZED ;
		BERR_TRACE(rc);
		goto done ;
	}


	if (hCEC->firstCecMessage)
	{
		hCEC->firstCecMessage = false ;
	}
	else
	{
		/*	verify no message is currently being transmitted.
			TX_EOM field is not modified in any other software context (only update by the hardware)
			Thus, this is essentially an atomic read 
		*/
		Register = BREG_Read32(hRegister, REGADDR_CEC_CNTRL_1 + ulOffset) ;
		i = BCHP_GET_FIELD_DATA(Register, REGNAME_CEC_CNTRL_1, TX_EOM) ;
		if (i == 0)
		{
			BDBG_WRN(("Transmit CEC is Busy (TX_EOM: %d)!!... retry transmit later", i)) ;
			rc = BCEC_TRANSMIT_BUSY ;
			BERR_TRACE(rc);
			goto done ;
		}
	}

	
	/* load the CEC Msg Bytes */
	Register = 0 ;
	TxDataRegisterOffset = 0 ;
	
	/* load the first nibble with the source/destination addr */
	j = 1 ;
	Register = (hCEC->stSettings.CecLogicalAddr << 4) | pMessageData->destinationAddr;

#if BCEC_CONFIG_DEBUG_MESSAGE_TX	
	/* Only fordebug purposes. BKNI_Snprintf should never be used in production 
		code due to timing effects */
	debugMsgOffset += BKNI_Snprintf(XmitMessage+debugMsgOffset, 
		sizeof (XmitMessage) - debugMsgOffset, "%02X ", Register) ;
#endif

	/* write the first nibble in case there is no PayLoad i.e zero length message */
	BREG_Write32(hRegister, REGADDR_CEC_TX_DATA_1 + ulOffset + TxDataRegisterOffset, Register) ;
	
	XmitMessageLength = pMessageData->messageLength /*+ 1*/ ;
	for ( i = 0 ; XmitMessageLength && (i <= XmitMessageLength) ; i = i + 4) 
	{
		for ( ; j < 4 ; j++)
		{
			Register |= pMessageData->messageBuffer[j+i-1] << (8 * j) ;
			
			if (j + i == XmitMessageLength)
				break ;
		}
			
		BREG_Write32(hRegister, REGADDR_CEC_TX_DATA_1  + ulOffset + TxDataRegisterOffset, Register) ;
			
		BDBG_MSG(("CEC TxReg %#08x: %#08x", 
			REGADDR_CEC_TX_DATA_1 + ulOffset + TxDataRegisterOffset, Register)) ;
			
		j = 0 ;
		Register = 0 ;
		TxDataRegisterOffset += 4 ;
	}


	/* set up  the length */
	BKNI_EnterCriticalSection();
	Register = BREG_Read32(hRegister, REGADDR_CEC_CNTRL_1 + ulOffset) ;
	Register &= ~ BCHP_MASK_CEC_CNTRL(CEC_CNTRL_1, MESSAGE_LENGTH) ;
	Register |= BCHP_FIELD_DATA(REGNAME_CEC_CNTRL_1, MESSAGE_LENGTH, pMessageData->messageLength) ;
	BREG_Write32(hRegister, REGADDR_CEC_CNTRL_1 + ulOffset, Register) ;
	
	/* toggle the start xmit bit */ 
	Register &=  ~ BCHP_MASK_CEC_CNTRL(CEC_CNTRL_1, START_XMIT_BEGIN) ;
	BREG_Write32(hRegister, REGADDR_CEC_CNTRL_1 + ulOffset, Register) ;   /* 0 */
	
	Register |= BCHP_FIELD_DATA(REGNAME_CEC_CNTRL_1, START_XMIT_BEGIN, 1) ;
	BREG_Write32(hRegister, REGADDR_CEC_CNTRL_1 + ulOffset, Register) ;   /* 1 */
	BKNI_LeaveCriticalSection();
	

#if BCEC_CONFIG_DEBUG_MESSAGE_TX
	/* CEC print/process message, only for debug purposes. 
		BKNI_Snprintf should never be used in production code due to timing effects */
	for (i = 0; i < XmitMessageLength && debugMsgOffset<sizeof(XmitMessage); i++) 
	{
		debugMsgOffset += BKNI_Snprintf(XmitMessage+debugMsgOffset, 
			sizeof (XmitMessage) - debugMsgOffset, "%02X ", pMessageData->messageBuffer[i]) ;
	}

	BDBG_WRN(("CEC Message Length %d transmitted: %s", XmitMessageLength, XmitMessage)) ;
#endif

#if BCEC_CONFIG_DEBUG_OPCODE
	BDBG_WRN(("Transmitted CEC Mesage <%s> - Opcode [0x%x]", 
		BCEC_OpcodeToString(pMessageData->messageBuffer[0]),
		pMessageData->messageBuffer[0]));
#endif

done:
	
	BDBG_LEAVE(BCEC_XmitMessage);
	return rc ; 
}


BERR_Code BCEC_GetReceivedMessage(
	BCEC_Handle hCEC,			/* [in] HDMI handle */
	BAVC_HDMI_CEC_MessageData *pRecvMessageData 	/* [out] ptr to storage for received CEC msg */
) 
{
	BERR_Code rc = BERR_SUCCESS ;
	uint8_t cecMessageBuffer[BCEC_CONFIG_P_MAX_MESSAGE_BUFFER];
	uint8_t i, j ;
	uint8_t RxCecWordCount ;
	uint32_t RegisterOffset ;
	uint32_t Register ;
	
#if BCEC_CONFIG_DEBUG_MESSAGE_RX
	/* allocate 3 bytes  for each OpCode / Parameter followed by a space i.e. "%02X "
		Also allow for message header byte */
	char receivedMsg[3 * (BCEC_CONFIG_P_MAX_MESSAGE_BUFFER + 1)];
#endif
	
	BDBG_ENTER(BCEC_GetReceivedMessage);
	BDBG_OBJECT_ASSERT(hCEC, BCEC_P_Handle) ;

	Register = BREG_Read32(hCEC->stDependencies.hRegister, REGADDR_CEC_CNTRL_1) ;
	RxCecWordCount = BCHP_GET_FIELD_DATA(Register, REGNAME_CEC_CNTRL_1, REC_WRD_CNT) ;
	pRecvMessageData->messageLength = RxCecWordCount ;

	RxCecWordCount++ ;	
	/* get the received words and place into the buffer */
	RegisterOffset = 0 ;
	for (i = 0 ; i < RxCecWordCount ; i = i + 4)
	{
		Register = BREG_Read32(hCEC->stDependencies.hRegister, REGADDR_CEC_RX_DATA_1 + RegisterOffset) ;
		for (j = 0 ; j + i < RxCecWordCount; j++)
			cecMessageBuffer[i+j] = Register >> (8 * j) & 0xFF ;
			
		RegisterOffset = RegisterOffset + 4 ;
	}

	/* save the received message */
	pRecvMessageData->initiatorAddr = (cecMessageBuffer[0] >> 4) & 0x0F;
	pRecvMessageData->destinationAddr = cecMessageBuffer[0] & 0x0F;
	BKNI_Memcpy(pRecvMessageData->messageBuffer, cecMessageBuffer+1, pRecvMessageData->messageLength);

#if BCEC_CONFIG_DEBUG_MESSAGE_RX
	/* For debugging purposes only. BKNI_Snprintf should never be used 
		in production code due to timing effects */
	for (i = 0, j = 0; i < pRecvMessageData->messageLength && j<(sizeof(receivedMsg)-1); i++) {
		j += BKNI_Snprintf(receivedMsg+j, sizeof(receivedMsg)-j, "%02X ", 
			pRecvMessageData->messageBuffer[i]) ;
	}
	
	BDBG_MSG(("CEC Message Length %d Received: %s", 
		pRecvMessageData->messageLength, receivedMsg)) ;
#endif	


#if BCEC_CONFIG_DEBUG_OPCODE
	BDBG_WRN(("Received CEC Mesage <%s> - Opcode [0x%x]", 
		BCEC_OpcodeToString(pRecvMessageData->messageBuffer[0]),
		pRecvMessageData->messageBuffer[0]));
#endif


	BDBG_LEAVE(BCEC_GetReceivedMessage);	
	return rc ;
}


BERR_Code BCEC_EnableReceive(
	BCEC_Handle hCEC	 /* [in] HDMI handle */
) 
{
	BERR_Code rc = BERR_SUCCESS ;
	BDBG_ENTER(BCEC_EnableReceive);
	BDBG_OBJECT_ASSERT(hCEC, BCEC_P_Handle) ;	
	
	BDBG_MSG(("Enable CEC Receive Mode")) ;
	BKNI_EnterCriticalSection();
		BCEC_P_EnableReceive_isr(hCEC);
	BKNI_LeaveCriticalSection();

	BDBG_LEAVE(BCEC_EnableReceive);
	return rc ;
}


BERR_Code BCEC_ReportPhysicalAddress(
	BCEC_Handle hCEC	  /* [in] HDMI handle */
) 
{
	BERR_Code rc = BERR_SUCCESS ;
	BAVC_HDMI_CEC_MessageData stMessageData;

	BDBG_ENTER(BCEC_ReportPhysicalAddress);
	BDBG_OBJECT_ASSERT(hCEC, BCEC_P_Handle) ;
	BKNI_Memset(&stMessageData, 0, sizeof(BAVC_HDMI_CEC_MessageData));

	if (hCEC->stSettings.CecLogicalAddr == BCEC_CONFIG_UNINITIALIZED_LOGICAL_ADDR)
	{
		BDBG_WRN(("CEC Logical Address has not been initialized; Unable to Send message")) ;
		rc = BERR_NOT_INITIALIZED ;
		BERR_TRACE(rc);
		goto done ;
	}

	/********************************** 
		CEC Message Buffer consists of: 
			hexOpCode
			device physical address
			device type
	***********************************/
	
	/* CEC message opcode = 0x84 */
	stMessageData.messageBuffer[0] = BCEC_OpCode_ReportPhysicalAddress;

	/* [Device Physical Address] */ 
	stMessageData.messageBuffer[1] = hCEC->stSettings.CecPhysicalAddr[0];
	stMessageData.messageBuffer[2] = hCEC->stSettings.CecPhysicalAddr[1];

	/* Device Type */
	stMessageData.messageBuffer[3] = hCEC->stSettings.eDeviceType;

	/* Broadcast CEC message */
	stMessageData.initiatorAddr = hCEC->stSettings.CecLogicalAddr;
	stMessageData.destinationAddr = BCEC_BROADCAST_ADDR;
	stMessageData.messageLength = 4;
	rc = BCEC_XmitMessage(hCEC, &stMessageData);

done:
	BDBG_LEAVE(BCEC_ReportPhysicalAddress);
	return rc ;
}


/******************************************************************************
Summary: Enter standby mode
*******************************************************************************/
BERR_Code BCEC_Standby(
	BCEC_Handle hCEC, /* [in] CEC Handle */
	const BCEC_StandbySettings *pSettings
	)
{
	BERR_Code rc = BERR_SUCCESS;
	
#if BCEC_CONFIG_AUTO_ON_SUPPORT	
	uint8_t i;

	BDBG_OBJECT_ASSERT(hCEC, BCEC_P_Handle) ;

	if (hCEC->standby) {
		BDBG_ERR(("Already in standby"));
		rc = BERR_UNKNOWN;
		BERR_TRACE(rc);
		goto done;
	}

	if (!hCEC->stSettings.enableAutoOn)
	{
		BDBG_ERR(("AutoOn feature is disabled. Enable through CEC settings"));
#ifdef BCHP_PWR_RESOURCE_HDMI_TX_CEC
		BCHP_PWR_ReleaseResource(hCEC->stDependencies.hChip, BCHP_PWR_RESOURCE_HDMI_TX_CEC);
#endif		
		rc = BERR_NOT_SUPPORTED;
		BERR_TRACE(rc);
		goto done;
	}

	/** Disable ALL interrupt before going to standby */
	for( i = 0; i < BCEC_MAKE_INTR_ENUM(LAST) ; i++ ) {
		BCEC_CHECK_RC( rc, BINT_DisableCallback( hCEC->hCallback[i] ) );
	}


	/* enable autoOn CEC */	
	BCEC_P_EnableAutoOn(hCEC, true);
	hCEC->standby = true;
	
done:	
	
#else
	/* No AutoON Support */
	BSTD_UNUSED(hCEC) ;
#endif

	BSTD_UNUSED(pSettings);	
	return rc; 
}


/******************************************************************************
Summary: Resume standby mode
*******************************************************************************/
BERR_Code BCEC_Resume(
	BCEC_Handle hCEC /* [in] CEC Handle */
	)
{
	BERR_Code rc = BERR_SUCCESS;

#if BCEC_CONFIG_AUTO_ON_SUPPORT 
	uint8_t i;

	BDBG_OBJECT_ASSERT(hCEC, BCEC_P_Handle) ;

	if (!hCEC->standby) 
	{
		BDBG_ERR(("Not in standby"));
		rc = BERR_UNKNOWN;		
		BERR_TRACE(rc);
		goto done;
	}

	if (!hCEC->stSettings.enableAutoOn)
	{
#ifdef BCHP_PWR_RESOURCE_HDMI_TX_CEC
		BCHP_PWR_AcquireResource(hCEC->stDependencies.hChip, BCHP_PWR_RESOURCE_HDMI_TX_CEC);
#endif
	}

	hCEC->standby = false;
	BCEC_P_EnableAutoOn(hCEC, false);

	/** Enable ALL interrupts after resume from standby */
	for( i = 0; i < BCEC_MAKE_INTR_ENUM(LAST) ; i++ ) {
		BCEC_CHECK_RC( rc, BINT_EnableCallback( hCEC->hCallback[i] ) );
	}
	
done:		
	
#else
	BSTD_UNUSED(hCEC) ;
#endif
	
	return rc ;
}



/* End of file. */

