/***************************************************************************
 *     Copyright (c) 2005-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bads_7125.h $
 * $brcm_Revision: Hydra_Software_Devel/3 $
 * $brcm_Date: 6/7/11 4:18p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ads/7125/bads_7125.h $
 * 
 * Hydra_Software_Devel/3   6/7/11 4:18p mward
 * SW7125-926:  Add interrupt from tuner to request LNA gain.
 * 
 * Hydra_Software_Devel/SW7125-926/1   4/28/11 5:32p mward
 * SW7125-926:  Add interrupt from tuner to request LNA gain.
 * 
 * Hydra_Software_Devel/2   7/16/10 2:11p mward
 * SW7125-513: Add back last 7550 history to show origin version.
 * 
 * Hydra_Software_Devel/1   7/12/10 2:58p mward
 * SW7125-513:  Add host control of 7125 internal DS and tuner.  Copy and
 * adapt 7550 implementation.
 * 
 * Hydra_Software_Devel/3   5/18/10 11:52a farshidf
 * SW7550-440: merge to main branch
 * 
 ***************************************************************************/


#ifndef BADS_7125_H__
#define BADS_7125_H__

#include "bchp.h"
#include "breg_mem.h"
#include "bint.h"
#include "bkni.h"
#include "berr_ids.h"
#include "bads.h"

#ifdef __cplusplus
extern "C" {
#endif


/***************************************************************************
Summary:
	This function returns the default settings for Qam In-Band Downstream module.

Description:
	This function is responsible for returns the default setting for 
	BADS module. The returning default setting should be when
	opening the device.

Returns:
	TODO:

See Also:
	BADS_7125_Open()

****************************************************************************/
BERR_Code BADS_7125_GetDefaultSettings(
	BADS_Settings *pDefSettings,		/* [out] Returns default setting */
	BCHP_Handle hChip					/* [in] Chip handle */
	);


/***************************************************************************
Summary:
    Data for the TNR->ADS interrupt callback
****************************************************************************/
typedef struct BADS_7125_TnrInterruptData
{
	enum 
	{
	  BADS_7125_SetDpm,
	  BADS_7125_ResetDpm,
	  BADS_7125_RequestLnaGain
	} action;
    uint8_t iOutdivMs;
} BADS_7125_TnrInterruptData;

/***************************************************************************
Summary:
    BADS_7125_ProcessTnrInterrupt_isr
****************************************************************************/
BERR_Code BADS_7125_ProcessTnrInterrupt_isr(
	BADS_ChannelHandle hChannel,
	const BADS_7125_TnrInterruptData *pInterruptData
    );



#ifdef __cplusplus
}
#endif
 
#endif

