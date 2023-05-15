/***************************************************************************
 *     Copyright (c) 2005-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bads_7550.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 6/22/12 12:37p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ads/include/bads_7550.h $
 * 
 * Hydra_Software_Devel/1   6/22/12 12:37p farshidf
 * SW7425-2983: 7550 and 7552 ADS update
 * 
 * Hydra_Software_Devel/3   5/18/10 11:52a farshidf
 * SW7550-440: merge to main branch
 * 
 * Hydra_Software_Devel/SW7550-440/1   5/18/10 11:46a farshidf
 * SW7550-440: add the dpm callback
 * 
 * Hydra_Software_Devel/2   12/31/09 3:05p VISHK
 * SW7550-135: Support 7550 QAM/DVB-C support in Nexus
 * 
 * Hydra_Software_Devel/1   11/18/09 6:03p farshidf
 * SW7550-38: 7550 inital ADS files
 *
 ***************************************************************************/


#ifndef BADS_7517_H__
#define BADS_7517_H__

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
	BADS_7517_Open()

****************************************************************************/
BERR_Code BADS_7550_GetDefaultSettings(
	BADS_Settings *pDefSettings,		/* [out] Returns default setting */
	BCHP_Handle hChip					/* [in] Chip handle */
	);


/***************************************************************************
Summary:
    Data for the TNR->ADS interrupt callback
****************************************************************************/
typedef struct BADS_7550_TnrInterruptData
{
    bool bResetDpm;
    uint8_t iOutdivMs;
} BADS_7550_TnrInterruptData;

/***************************************************************************
Summary:
    BADS_7550_ProcessTnrInterrupt_isr
****************************************************************************/
BERR_Code BADS_7550_ProcessTnrInterrupt_isr(
	BADS_ChannelHandle hChannel,
	const BADS_7550_TnrInterruptData *pInterruptData
    );



#ifdef __cplusplus
}
#endif
 
#endif

