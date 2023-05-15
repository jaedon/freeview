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
 * $brcm_Workfile: nexus_read_otp_id.c $
 * $brcm_Revision: 1 $
 * $brcm_Date: 1/28/11 7:31p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/extensions/security/usercmd/7422/src/nexus_read_otp_id.c $
 * 
 * 1   1/28/11 7:31p atruong
 * SW7422-235: Enable Nexus Security extension modules for 97422
 *
 ******************************************************************************/

#include "nexus_security_module.h"
#include "nexus_security_datatypes.h"
#include "nexus_security.h"
#include "priv/nexus_security_priv.h"
#include "priv/nexus_core.h"

#include "bhsm.h"
#include "bhsm_misc.h"

BDBG_MODULE(nexus_read_otp_id);

NEXUS_Error NEXUS_Security_ReadOtpId( 
		NEXUS_OtpIdType				otpId, 
		NEXUS_OtpIdOutput			*pOutput )
{
    BERR_Code                   rc;
    BHSM_Handle                 hHsm;
    BHSM_ReadOTPIdIO_t			readOTPIdIO;

    NEXUS_Security_GetHsm_priv(&hHsm);
    if (!hHsm)
        return NEXUS_INVALID_PARAMETER;
    /*
     * formulate the request structure
     */
	readOTPIdIO.OtpId = (BHSM_OtpIdRead_e)otpId;

    rc = BHSM_ReadOTPId(hHsm, &readOTPIdIO);
    if (rc != 0)
    {
        BDBG_ERR(("Error calling BHSM_ReadOTPId()\n"));
        return NEXUS_INVALID_PARAMETER;
    }
    /*
      collect output
      */
    if (readOTPIdIO.unStatus)
    {
        return NEXUS_INVALID_PARAMETER;
    }
    BKNI_Memcpy((void *)pOutput->otpId,
                (void *)readOTPIdIO.aucOTPId,
                readOTPIdIO.unIdSize);
    pOutput->size = readOTPIdIO.unIdSize;

    return NEXUS_SUCCESS;
}
