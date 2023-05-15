/***************************************************************************
 *     Copyright (c) 2003-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bhdr_pwr.h $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 8/2/11 6:15p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: Q:/projects/7425/latest/magnum/portinginterface/hdr/7425/bhdr_pwr.h $
 * 
 * Hydra_Software_Devel/2   8/2/11 6:15p rgreen
 * SW7425-692:  Update power management support for standby/resume
 * 
 * Hydra_Software_Devel/1   6/8/11 4:53p rgreen
 * SW7425-692: Stubs for initial power management support
 * 
 ***************************************************************************/
#ifndef BHDR_PWR_H__
#define BHDR_PWR_H__

#include "bchp_pwr.h"	 
#include "bhdr_fe.h"       /* Chip Info */



/***************************************************************************
Summary:
	HDMI Rx  standby settings
	
****************************************************************************/
typedef struct BHDR_StandbySettings
{
	bool bEnableWakeup; /* If true, then allows wakeup from standby using HDMI Rx. 
	                       If false, the device is powered down during standby */
} BHDR_StandbySettings;

/******************************************************************************
Summary:
	Get default HDMI Rx standby settings
	
*******************************************************************************/
void BHDR_GetDefaultStandbySettings(
	BHDR_StandbySettings *pSettings
	);


/******************************************************************************
Summary:
	Enter standby mode
	
*******************************************************************************/
BERR_Code BHDR_Standby(
        BHDR_FE_Handle hFrontEnd, /* [in] HDR FE Handle */
	const BHDR_StandbySettings *pSettings /* optional */
	);

/******************************************************************************
Summary:
	Resume from standby mode
	
*******************************************************************************/
BERR_Code BHDR_Resume(
	BHDR_FE_Handle hFrontEnd /* [in] HDR FE Handle */
	);

#ifdef __cplusplus
}
#endif

#endif /* BHDR_PWR_H__ */


