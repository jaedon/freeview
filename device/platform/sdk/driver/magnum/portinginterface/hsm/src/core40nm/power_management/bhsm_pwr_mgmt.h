/***************************************************************************
 *     (c)2007-2011 Broadcom Corporation
 *
 *  This program is the proprietary software of Broadcom Corporation and/or its licensors,
 *  and may only be used, duplicated, modified or distributed pursuant to the terms and
 *  conditions of a separate, written license agreement executed between you and Broadcom
 *  (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 *  no license (express or implied), right to use, or waiver of any kind with respect to the
 *  Software, and Broadcom expressly reserves all rights in and to the Software and all
 *  intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 *  HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 *  NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
 *
 *  Except as expressly set forth in the Authorized License,
 *
 *  1.     This program, including its structure, sequence and organization, constitutes the valuable trade
 *  secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 *  and to use this information only in connection with your use of Broadcom integrated circuit products.
 *
 *  2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 *  AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
 *  WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
 *  THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
 *  OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
 *  LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
 *  OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
 *  USE OR PERFORMANCE OF THE SOFTWARE.
 *
 *  3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
 *  LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
 *  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
 *  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
 *  THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
 *  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
 *  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
 *  ANY LIMITED REMEDY.
 *
 * $brcm_Workfile: bhsm_pwr_mgmt.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 10/12/11 8:03p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/hsm/7425/b0/power_management/bhsm_pwr_mgmt.h $
 * 
 * Hydra_Software_Devel/1   10/12/11 8:03p atruong
 * SW7425-1251: HSM PI Support For BCM7425B0
 * 
 * Hydra_Software_Devel/1   1/28/11 10:00a atruong
 * SW7422-26: Add HSM PI Support - Additional HSM PI Modules
 *
 ******************************************************************************/


#ifndef BHSM_PWR_MGMT_H__
#define BHSM_PWR_MGMT_H__

#define POWER_MGMT

#include "bhsm.h"


#ifdef __cplusplus
extern "C" {
#endif

typedef enum BCMD_PwrMgmtOpType_e
{
    BCMD_PwrMgmtOpType_ePwrDwnPassive,
    BCMD_PwrMgmtOpType_ePwrDwnActiveBkGnd,          /* unsupported */
    BCMD_PwrMgmtOpType_ePwrDwnActiveBspShutdown,    /* unsupported */
    BCMD_PwrMgmtOpType_ePwrUp,  /* effectively unsupported */
    BCMD_PwrMgmtOpType_eMax
} BCMD_PwrMgmtOpType_e;


/* Module Specific Functions */

/**************************************************************************************************
Summary:

Description:
Structure that defines BHSM_PwrMgmtIO_t members

See Also:
BHSM_PwrMgmt()
**************************************************************************************************/
typedef struct BHSM_PwrMgmtIO {

	/* In: Power management operation */
	BCMD_PwrMgmtOpType_e			pwrMgmtOp;
	
	/* Out: 0 for success, otherwise failed. */
	uint32_t						unStatus;
    
} BHSM_PwrMgmtIO_t;

/**************************************************************************************************
Summary:

Description:

Calling Context: 

Performance and Timing:
This is a synchronous/blocking function that will not return until it completes.
 
Input:
in_handle              - BHSM_Handle, Host Secure module handle.
inoutp_pwrMgmtIO  - BHSM_PwrMgmtIO_t.

Output:
inoutp_pwrMgmtIO  - BHSM_PwrMgmtIO_t.

Returns:
BERR_SUCCESS - success
BHSM_STATUS_FAILED - failure
 
See Also:
N/A
**************************************************************************************************/
BERR_Code BHSM_PwrMgmt (
		BHSM_Handle				in_handle,
		BHSM_PwrMgmtIO_t		*inoutp_pwrMgmtIO
);











/* End of Module Specific Functions */


#ifdef __cplusplus
}
#endif

#endif /* BHSM_PWR_MGMT_H__ */







