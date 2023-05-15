/***************************************************************************
*     (c)2004-2012 Broadcom Corporation
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
* $brcm_Workfile: nexus_wmdrmpd.c $
* $brcm_Revision: 5 $
* $brcm_Date: 11/6/12 1:18p $
*
* API Description:
*   API name: wmdrmpd
*    Specific APIs related to Microsoft Windows Media DRM PD
*
* Revision History:
*
* $brcm_Log: /nexus/modules/wmdrmpd/src/nexus_wmdrmpd.c $
* 
* 5   11/6/12 1:18p hugost
* SW7435-473: Merged David's fix into main
* 
* 4   10/4/12 4:58p hugost
* SW7425-4027: merge into main
* 
* SW7425-4027/1   10/4/12 4:13p hugost
* SW7425-4027: Added new api.
* 
* 3   10/3/12 9:41a hugost
* SW7425-3986: Merges into main.
* 
* SW7425-3986/1   9/24/12 3:51p hugost
* Removed destructor attributes from NEXUS_WmDrmPd_Create. Added 2 new
*  funcitons NEXUS_WmDrmPdCore_GetDefaultAesCounterInfo() and
*  NEXUS_WmDrmPdCore_ProcessBlocksAesCounter().
* 
* 2   8/23/12 3:57p jgarrett
* SW7425-3757: Merge to main branch
* 
* SW7425-3757/1   8/15/12 4:55p hugost
* Added the following APIs: NEXUS_WmDrmPd_GetDefaultPsshBoxInfo(),
*  NEXUS_WmDrmPd_SetPsshBox(),
*  NEXUS_WmDrmPd_GetDefaultProtectionSchemeInfo(),
*  NEXUS_WmDrmPd_SetProtectionSchemeBox() and
*  NEXUS_WmDrmPd_CleanupLicenseStore().
* 
* 1   7/27/12 2:26p jgarrett
* SW7425-3281: Merge to main branch
* 
* SW7425-3281/4   7/17/12 4:44p jgarrett
* SW7425-3281: Incorporating code review feedback
* 
* SW7425-3281/3   7/13/12 3:04p piyushg
* SW7425-3281: Modifications based on nexus review.
* 
* SW7425-3281/2   7/8/12 4:54p piyushg
* SW7425-3281: Added support for playready licensce acquisition and Nexus
*  fileio
* 
* SW7425-3281/1   6/20/12 2:35p piyushg
* SW7425-3281: Initial checkin of Nexus directory structure and header
*  files.
* 
***************************************************************************/

#include "nexus_wmdrmpd_module.h"

/* Currently, all DRM routines pass through to the core module. */

void NEXUS_WmDrmPd_GetDefaultSettings(
    NEXUS_WmDrmPdSettings *pSettings    /* [out] default settings */
    )
{
    NEXUS_WmDrmPdCoreSettings coreSettings;

    BDBG_ASSERT(NULL != pSettings);

    BKNI_Memset(pSettings, 0, sizeof(NEXUS_WmDrmPdSettings));

    NEXUS_WmDrmPdCore_GetDefaultSettings(&coreSettings);
    pSettings->heap = coreSettings.heap;
    pSettings->playpump = coreSettings.playpump;
    pSettings->transportType = coreSettings.transportType;
    pSettings->policyCallback = coreSettings.policyCallback;
}

NEXUS_WmDrmPdHandle NEXUS_WmDrmPd_Create(
    const NEXUS_WmDrmPdSettings *pSettings  
    )
{
    NEXUS_WmDrmPdCoreSettings coreSettings;
    NEXUS_WmDrmPdSettings defaults;
    NEXUS_WmDrmPdCoreHandle hCore;

    if ( NULL == pSettings )
    {
        NEXUS_WmDrmPd_GetDefaultSettings(&defaults);
        pSettings = &defaults;
    }

    NEXUS_WmDrmPdCore_GetDefaultSettings(&coreSettings);
    coreSettings.heap = pSettings->heap;
    coreSettings.playpump = pSettings->playpump;
    coreSettings.transportType = pSettings->transportType;
    coreSettings.policyCallback = pSettings->policyCallback;
    coreSettings.ioHandle = nexus_wmdrmpd_p_create_io();
    hCore = NEXUS_WmDrmPdCore_Create(&coreSettings);
    if ( NULL == hCore )
    {
        (void)BERR_TRACE(NEXUS_UNKNOWN);
    }
    return hCore;
}

void NEXUS_WmDrmPd_Destroy(
    NEXUS_WmDrmPdHandle handle
    )
{
    NEXUS_WmDrmPdCore_Destroy(handle);
    nexus_wmdrmpd_p_destroy_io();
}

NEXUS_Error NEXUS_WmDrmPd_ConfigureKeySlot(
    NEXUS_WmDrmPdHandle handle,
    NEXUS_KeySlotHandle keySlot
    )
{
    return NEXUS_WmDrmPdCore_ConfigureKeySlot(handle, keySlot);
}

void NEXUS_WmDrmPd_GetDefaultPsiObjectInfo(
    NEXUS_WmDrmPdPsiObjectInfo *pObject     /* [out] */
    )
{
    NEXUS_WmDrmPdCore_GetDefaultPsiObjectInfo(pObject);
}

NEXUS_Error NEXUS_WmDrmPd_SetPsiObject(
    NEXUS_WmDrmPdHandle handle,
    const NEXUS_WmDrmPdPsiObjectInfo *pInfo,
    const void *pData,
    size_t dataLength
    )
{
    return NEXUS_WmDrmPdCore_SetPsiObject(handle, 
                                          pInfo, 
                                          pData, 
                                          dataLength
                                          );
}

void NEXUS_WmDrmPd_GetDefaultPsshBoxInfo(
    NEXUS_WmDrmPdMp4PsshBoxInfo *pObject     /* [out] */
    )
{
    NEXUS_WmDrmPdCore_GetDefaultPsshBoxInfo(pObject);
}

NEXUS_Error NEXUS_WmDrmPd_SetPsshBox(
    NEXUS_WmDrmPdHandle handle,
    const NEXUS_WmDrmPdMp4PsshBoxInfo *pInfo,
    const void *pData,
    size_t dataLength
    )
{
    return NEXUS_WmDrmPdCore_SetPsshBox(handle,
                                        pInfo,
                                        pData,
                                        dataLength);
}


void NEXUS_WmDrmPd_GetDefaultProtectionSchemeInfo(
    NEXUS_WmDrmPdMp4ProtectionSchemeInfo *pObject     /* [out] */
    )
{
    NEXUS_WmDrmPdCore_GetDefaultProtectionSchemeInfo(pObject);
}

NEXUS_Error NEXUS_WmDrmPd_SetProtectionSchemeBox(
    NEXUS_WmDrmPdHandle handle,
    const NEXUS_WmDrmPdMp4ProtectionSchemeInfo *pInfo
    )
{
    return NEXUS_WmDrmPdCore_SetProtectionSchemeBox(handle,
                                                    pInfo);
}

NEXUS_Error NEXUS_WmDrmPd_SetCencrObject(
    NEXUS_WmDrmPdHandle handle,
    const void *pSecurityData,  
    size_t securityLength,      /* security data length in bytes */
    const void *pProtocolData,  
    size_t protocolLength,      /* protocol length in bytes */
    const void *pKeyIdData,     
    size_t keyIdLength,         /* key id length in bytes */
    const void *pLicenseUrlData,
    size_t licenseUrlLength     /* license url length in bytes */
    )
{
    return NEXUS_WmDrmPdCore_SetCencrObject(handle, 
                                            pSecurityData,
                                            securityLength,
                                            pProtocolData,
                                            protocolLength,
                                            pKeyIdData,
                                            keyIdLength,
                                            pLicenseUrlData,
                                            licenseUrlLength
                                            );
}

NEXUS_Error NEXUS_WmDrmPd_SetXcencrObject(
    NEXUS_WmDrmPdHandle handle,
    const void *pData,  
    size_t dataLength   /* extended encr size in bytes */
    )
{
    return NEXUS_WmDrmPdCore_SetXcencrObject(handle, 
                                             pData,
                                             dataLength
                                             );
}

void NEXUS_WmDrmPd_GetDefaultDigsignObjectInfo(
    NEXUS_WmDrmPdDigsignObjectInfo *pObject     /* [out] */
    )
{
    NEXUS_WmDrmPdCore_GetDefaultDigsignObjectInfo(pObject);
}

NEXUS_Error NEXUS_WmDrmPd_SetDigsignObject(
    NEXUS_WmDrmPdHandle handle,
    const NEXUS_WmDrmPdDigsignObjectInfo *pInfo,
    const void *pData, 
    size_t dataLength   /* extended encr size in bytes */
    )
{
    return NEXUS_WmDrmPdCore_SetDigsignObject(handle, 
                                              pInfo,
                                              pData,
                                              dataLength
                                              );
}

NEXUS_Error NEXUS_WmDrmPd_LoadLicense(
    NEXUS_WmDrmPdHandle handle
    )
{
    return NEXUS_WmDrmPdCore_LoadLicense(handle);
}

NEXUS_Error NEXUS_WmDrmPd_GetLicenseChallenge(
    NEXUS_WmDrmPdHandle handle,
    NEXUS_WmDrmPdLicenseChallenge *pChallenge   /* [out] */
    )
{
    return NEXUS_WmDrmPdCore_GetLicenseChallenge(handle, pChallenge);
}

NEXUS_Error NEXUS_WmDrmPd_GetCustomLicenseChallenge(
    NEXUS_WmDrmPdHandle handle,
    const void *pCustomData, /* attr{nelem=customDataLength;reserved=128} */
    size_t customDataLength,
    NEXUS_WmDrmPdLicenseChallenge *pChallenge   /* [out] */
    )
{
    return NEXUS_WmDrmPdCore_GetCustomLicenseChallenge(handle, pCustomData, customDataLength, pChallenge);
}

NEXUS_Error NEXUS_WmDrmPd_LicenseChallengeComplete(
    NEXUS_WmDrmPdHandle handle,
    uint32_t responseLength,        /* Response length in bytes */
    unsigned responseOffset         /* Offset to the start of the response within the data buffer */
    )
{
    return NEXUS_WmDrmPdCore_LicenseChallengeComplete(handle, responseLength, responseOffset);
}

NEXUS_Error NEXUS_WmDrmPd_GetPolicyStatus(
    NEXUS_WmDrmPdHandle handle,
    NEXUS_WmDrmPdPolicyStatus *pStatus  /* [out] */
    )
{
    return NEXUS_WmDrmPdCore_GetPolicyStatus(handle, pStatus);
}


void NEXUS_WmDrmPd_GetDefaultAesCounterInfo(
    NEXUS_WmdrmPdAesCounterInfo *pInfo
    )
{
    NEXUS_WmDrmPdCore_GetDefaultAesCounterInfo(pInfo);
}


NEXUS_Error NEXUS_WmDrmPd_ProcessBlocksAesCounter(
    NEXUS_WmDrmPdHandle handle,
    const NEXUS_WmdrmPdAesCounterInfo *pInfo,
    const NEXUS_DmaJobBlockSettings *pDmaBlocks,
    unsigned nBlocks
    )
{
    return NEXUS_WmDrmPdCore_ProcessBlocksAesCounter(handle, pInfo, pDmaBlocks, nBlocks);
}


void NEXUS_WmDrmPd_CleanupLicenseStore(
    NEXUS_WmDrmPdHandle handle
    )
{
    NEXUS_WmDrmPdCore_CleanupLicenseStore(handle);
    return;
}

