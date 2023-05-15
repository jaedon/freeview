/***************************************************************************
*     Copyright (c) 2003-2012, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: bhdm_debug.c $
* $brcm_Revision: Hydra_Software_Devel/3 $
* $brcm_Date: 7/16/12 2:48p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /magnum/portinginterface/hdm/src/common/bhdm_debug.c $
* 
* Hydra_Software_Devel/3   7/16/12 2:48p rgreen
* SW7425-2989: Format Change Detection available in 40nm and newer chips.
* Disable for earlier chips
* 
* Hydra_Software_Devel/2   7/12/12 4:14p rgreen
* SW7425-2989: Merge changes
* 
* Hydra_Software_Devel/SW7425-2989/2   7/11/12 6:17p rgreen
* SW7425-2989: Prevent initial debug warning after format change
* 
* Hydra_Software_Devel/SW7425-2989/1   7/6/12 2:22p rgreen
* SW7425-2989: Add debug monitor to report format changes to HDMI Tx core
* 
***************************************************************************/

#include "bhdm.h"
#include "bhdm_priv.h"


BDBG_MODULE(BHDM_DEBUG) ;


void BHDM_DEBUG_P_Monitor_FormatChangesStart(BHDM_Handle hHDMI)
{
#if BHDM_CONFIG_FORMAT_CHANGE_DETECTION_SUPPORT
	uint32_t Register ;
	BREG_Handle hRegister = hHDMI->hRegister ;
	
	Register = 0xFFFFFFFF;
	BREG_Write32(hRegister, BCHP_HDMI_FORMAT_DET_UPDATE_CLEAR, Register) ;	
	Register = 0;
	BREG_Write32(hRegister, BCHP_HDMI_FORMAT_DET_UPDATE_CLEAR, Register) ;
	
	BTMR_StartTimer(hHDMI->TimerFormatChange, 
		BHDM_P_SECOND * BHDM_CONFIG_DEBUG_MONITOR_FORMAT_CHANGE_S) ;

	hHDMI->TimerFormatInitialChangeReported = false ;
#else
	BDBG_WRN(("HDMI Debug Format Change Monitor not supported by this chip")) ;
	BSTD_UNUSED(hHDMI) ;
#endif
}

/******************************************************************************
void BHDM_DEBUG_P_FormatChangeCheck_isr
Summary: Check/detect format changes
*******************************************************************************/
void BHDM_DEBUG_P_Monitor_FormatChangesCheck_isr(BHDM_Handle hHDMI) 
{
#if BHDM_CONFIG_FORMAT_CHANGE_DETECTION_SUPPORT
	uint32_t Register ;
	BREG_Handle hRegister ;

	hRegister = hHDMI->hRegister ;
	
	Register = BREG_Read32(hRegister, BCHP_HDMI_FORMAT_DET_UPDATE_STATUS) ;
	if (Register)
	{
		if (!hHDMI->TimerFormatInitialChangeReported)
		{
			/* skip reporting first format change warning; initial change expected after a format change */
			BDBG_MSG(("Skip reporting format change that is expected after a format change")) ;
			hHDMI->TimerFormatInitialChangeReported = true ;
		}
		else
		{
			BDBG_ERR(("!!! Format Change into HDMI Tx Core detected")) ;
		}
		
		Register = 0xFFFFFFFF ;
		BREG_Write32(hRegister, BCHP_HDMI_FORMAT_DET_UPDATE_CLEAR, Register) ;	
		Register = 0 ;
		BREG_Write32(hRegister, BCHP_HDMI_FORMAT_DET_UPDATE_CLEAR, Register) ;	
	}
	else
	{
		BDBG_MSG(("Stable Format into HDMI Tx Core...")) ;
	}
	BTMR_StartTimer_isr(hHDMI->TimerFormatChange, 
		BHDM_P_SECOND * BHDM_CONFIG_DEBUG_MONITOR_FORMAT_CHANGE_S) ;
#else
	BSTD_UNUSED(hHDMI) ;
#endif
}


