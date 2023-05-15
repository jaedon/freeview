/***************************************************************************
 *     (c)2007-2012 Broadcom Corporation
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
 * $brcm_Workfile: nexus_hmac_sha_cmd.c $
 * $brcm_Revision: 4 $
 * $brcm_Date: 10/8/12 3:45p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/extensions/security/usercmd/src/nexus_hmac_sha_cmd.c $
 * 
 * 4   10/8/12 3:45p atruong
 * SW7435-399: HMAC/SHA input buffer to be allocated with
 *  NEXUS_Memory_Allocate()
 * 
 * 3   2/15/12 12:58p atruong
 * SWSECURITY-109: HMAC-SHA command support for 40-nm platforms
 * 
 * 2   6/29/11 7:25p atruong
 * SW7425-812:HSM PI API modification for new HMAC-SHA support
 * 
 * 1   1/28/11 7:31p atruong
 * SW7422-235: Enable Nexus Security extension modules for 97422
 *
 ******************************************************************************/


#include "nexus_hmac_sha_cmd.h"
#include "nexus_security_module.h"
#include "nexus_security_datatypes.h"
#include "nexus_security.h"
#include "priv/nexus_security_priv.h"
#include "priv/nexus_core.h"

#include "bhsm.h"
#include "bsp_s_commands.h"
#include "bsp_s_misc.h"
#include "bhsm_misc.h"
#include "bsp_s_hw.h"

#include "bhsm_user_cmds.h"

BDBG_MODULE(nexus_hmac_sha_cmd);



void NEXUS_HMACSHA_GetDefaultOpSettings(
    NEXUS_HMACSHA_OpSettings *pOpSettings /* [out] */
    )

{
    BKNI_Memset(pOpSettings, 0, sizeof(*pOpSettings));
	pOpSettings->dataSrc   = NEXUS_HMACSHA_DataSource_eDRAM;
	pOpSettings->keySource = NEXUS_HMACSHA_KeySource_eKeyVal;
	pOpSettings->context   = NEXUS_HMACSHA_BSPContext_eContext0;

}


NEXUS_Error NEXUS_HMACSHA_PerformOp(
    const NEXUS_HMACSHA_OpSettings *pOpSettings,    /* structure holding input parameters */
    NEXUS_HMACSHA_DigestOutput     *pOutput         /* [out] structure holding digest buffer and size */
    )
{
    BHSM_Handle             hHsm;
    BHSM_UserHmacShaIO_t    userHmacShaIO;
    NEXUS_Error             rc = NEXUS_SUCCESS;

    NEXUS_Security_GetHsm_priv (&hHsm);
    if ( !hHsm )
        return NEXUS_NOT_INITIALIZED;

	BKNI_Memset(&userHmacShaIO, 0, sizeof(BHSM_UserHmacShaIO_t));

	/*
	 * Assemble the command block
	 */
	userHmacShaIO.oprMode   = pOpSettings->opMode;
	userHmacShaIO.shaType   = pOpSettings->shaType;
	userHmacShaIO.keySource = pOpSettings->keySource;

	/* Take in only the max key length supported */
	if (pOpSettings->keyLength <= NEXUS_HMACSHA_KEY_LEN)
		userHmacShaIO.unKeyLength = pOpSettings->keyLength;
	else
		userHmacShaIO.unKeyLength = NEXUS_HMACSHA_KEY_LEN;

	if (pOpSettings->keySource == NEXUS_HMACSHA_KeySource_eKeyLadder)
	{
		userHmacShaIO.VirtualKeyLadderID = pOpSettings->VKL;
		userHmacShaIO.keyLayer           = pOpSettings->keyLayer;
	}
	else
	{
		BKNI_Memcpy(&(userHmacShaIO.keyData), &(pOpSettings->keyData), userHmacShaIO.unKeyLength);
	}
	userHmacShaIO.keyIncMode    = (BHSM_HMACSHA_KeyInclusion_e)pOpSettings->keyIncMode;
	userHmacShaIO.byteSwap      = pOpSettings->byteSwap;
	userHmacShaIO.contextSwitch = pOpSettings->context;
	userHmacShaIO.dataSource    = pOpSettings->dataSrc;
	if (pOpSettings->dataSrc == NEXUS_HMACSHA_DataSource_eCmdBuf)
	{
		/* This mode is currently not supported */
		rc = NEXUS_NOT_SUPPORTED;
		goto error;
	}
	else /* Data is supplied in a buffer pointed to by dataAddress */
	{
		/* Make sure address is from a NEXUs_Memory_Allocate(), for this to work in both kernel and user mode Nexus */
		if (!NEXUS_P_CpuAccessibleAddress(pOpSettings->dataAddress)) 
		{
			/* this pointer must have driver-side mapping */
			return BERR_TRACE(NEXUS_INVALID_PARAMETER);
		}
		
		userHmacShaIO.pucInputData   = pOpSettings->dataAddress;
		userHmacShaIO.unInputDataLen = pOpSettings->dataSize;
	}
	userHmacShaIO.contMode = pOpSettings->dataMode;

	/* Submit the command now */
	rc = BHSM_UserHmacSha(hHsm, &userHmacShaIO);
    if (rc != 0)
    {
        BDBG_ERR(("Error calling BHSM_UserHmacSha()\n"));
		rc = NEXUS_INVALID_PARAMETER;
		goto error;
    }

	/* If there are one or more data buffer left to submit, just fall through */
	if (pOpSettings->dataMode == NEXUS_HMACSHA_DataMode_eMore)
	{
		goto error;
	}
	else /* else collect the digest value */
	{
		if (userHmacShaIO.unStatus)
		{
			BDBG_ERR(("Error calling BHSM_UserHmacSha()\n"));
			rc = NEXUS_INVALID_PARAMETER;
			goto error;
		}
		BKNI_Memcpy((void *)pOutput->digestData,
					(void *)userHmacShaIO.aucOutputDigest,
					userHmacShaIO.digestSize);
		pOutput->digestSize = userHmacShaIO.digestSize;

	}

error:

    return rc;


}






