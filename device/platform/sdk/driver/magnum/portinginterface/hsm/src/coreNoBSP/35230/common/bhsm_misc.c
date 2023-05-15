/***************************************************************************
 *     (c)2011 Broadcom Corporation
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
 * $brcm_Workfile: bhsm_misc.c $
 * $brcm_Revision: Hydra_Software_Devel/3 $
 * $brcm_Date: 7/27/11 12:44p $
 *
 * Module Description:
 *
 * Revision History:
 ***************************************************************************/

#include "bstd.h"
#include "bkni.h"


#include "bhsm_misc.h"


BDBG_MODULE(BHSM);


BERR_Code	BHSM_ReadOTPId(
		BHSM_Handle			in_handle,
		BHSM_ReadOTPIdIO_t	*inoutp_readOTPIdIO
)
{
	BERR_Code errCode = BERR_SUCCESS;
	uint32_t unOtpRead = 0;


	BDBG_MSG(("%s - Entered function", __FUNCTION__));
	BDBG_ENTER(BHSM_ReadOTPId);
	BDBG_ASSERT( in_handle );
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(in_handle->ulMagicNumber != BHSM_P_HANDLE_MAGIC_NUMBER ) );	

	unOtpRead = BREG_Read32( in_handle->regHandle, XPT_OTP_ID_WORD_LO );

	inoutp_readOTPIdIO->aucOTPId[4] = (unOtpRead & 0xFF000000)>>24;
	inoutp_readOTPIdIO->aucOTPId[5] = (unOtpRead & 0x00FF0000)>>16;
	inoutp_readOTPIdIO->aucOTPId[6] = (unOtpRead & 0x0000FF00)>>8;
	inoutp_readOTPIdIO->aucOTPId[7] = (unOtpRead & 0x000000FF);

	BDBG_MSG(("%s - LOW unOtpRead(0x%08x) - 0x%02x 0x%02x 0x%02x 0x%02x",
			__FUNCTION__, unOtpRead, inoutp_readOTPIdIO->aucOTPId[4], inoutp_readOTPIdIO->aucOTPId[5],
			inoutp_readOTPIdIO->aucOTPId[6], inoutp_readOTPIdIO->aucOTPId[7]));

	unOtpRead = 0;
	unOtpRead = BREG_Read32( in_handle->regHandle, XPT_OTP_ID_WORD_HI );

	inoutp_readOTPIdIO->aucOTPId[0] = (unOtpRead & 0xFF000000)>>24;
	inoutp_readOTPIdIO->aucOTPId[1] = (unOtpRead & 0x00FF0000)>>16;
	inoutp_readOTPIdIO->aucOTPId[2] = (unOtpRead & 0x0000FF00)>>8;
	inoutp_readOTPIdIO->aucOTPId[3] = (unOtpRead & 0x000000FF);

	BDBG_MSG(("%s - HIGH unOtpRead(0x%08x) - 0x%02x 0x%02x 0x%02x 0x%02x",
			__FUNCTION__, unOtpRead, inoutp_readOTPIdIO->aucOTPId[0], inoutp_readOTPIdIO->aucOTPId[1],
			inoutp_readOTPIdIO->aucOTPId[2], inoutp_readOTPIdIO->aucOTPId[3]));

	inoutp_readOTPIdIO->unIdSize = 8;
	inoutp_readOTPIdIO->unStatus = 0;
	BDBG_MSG(("%s - unIdSize = '%u'     unStatus = '%u'", __FUNCTION__, inoutp_readOTPIdIO->unIdSize, inoutp_readOTPIdIO->unStatus));
	
BHSM_P_DONE_LABEL:
	
	BDBG_LEAVE(BHSM_ReadOTPId);
	BDBG_MSG(("%s - Exiting function", __FUNCTION__));
	return( errCode );
}


