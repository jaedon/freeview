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
 * $brcm_Workfile: bhsm_pwr_mgmt.c $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 10/12/11 8:03p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/hsm/7425/b0/power_management/bhsm_pwr_mgmt.c $
 * 
 * Hydra_Software_Devel/1   10/12/11 8:03p atruong
 * SW7425-1251: HSM PI Support For BCM7425B0
 * 
 * Hydra_Software_Devel/1   1/28/11 10:00a atruong
 * SW7422-26: Add HSM PI Support - Additional HSM PI Modules
 *
 ******************************************************************************/


#include "bhsm_pwr_mgmt.h"



BDBG_MODULE(BHSM);


BERR_Code BHSM_PwrMgmt (
		BHSM_Handle				in_handle,
		BHSM_PwrMgmtIO_t		*inoutp_pwrMgmtIO
)
{
	BERR_Code errCode = BERR_SUCCESS;
	BHSM_ChannelHandle channelHandle = in_handle->channelHandles[BHSM_HwModule_eCmdInterface2];
    BHSM_P_CommandData_t inoutp_commandData;
    uint32_t ulParamLen = 0;

	BDBG_MSG(("Inside BHSM_PwrMgmt\n"));	
	BDBG_ENTER(BHSM_PwrMgmt);
	BDBG_ASSERT( in_handle );
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(in_handle->ulMagicNumber != BHSM_P_HANDLE_MAGIC_NUMBER ) );	

	BKNI_Memset(&inoutp_commandData, 0, sizeof(BHSM_P_CommandData_t));  /* safer to make VxWorks happy */

	inoutp_commandData.cmdId = BCMD_cmdType_ePowerMgmtOp;
	inoutp_commandData.unContMode = CONT_MODE_ALL_DATA;

	inoutp_commandData.unInputParamsBuf[ulParamLen/4] = inoutp_pwrMgmtIO->pwrMgmtOp;

	ulParamLen += 4;

	inoutp_commandData.unInputParamLen = ulParamLen;
	inoutp_commandData.bLockWaive = false;

	BHSM_P_CHECK_ERR_CODE_FUNC(errCode, 
			BHSM_P_CommonSubmitCommand (channelHandle, 
				&inoutp_commandData));				

	ulParamLen = 0;

	inoutp_pwrMgmtIO->unStatus = inoutp_commandData.unOutputParamsBuf[ulParamLen/4];

	ulParamLen += 4;

    
	/* Check status */
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(0 != inoutp_pwrMgmtIO->unStatus  ) );       

BHSM_P_DONE_LABEL:
	
	BDBG_LEAVE(BHSM_PwrMgmt);
	return( errCode );

}












