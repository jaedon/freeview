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
* $brcm_Workfile: nexus_wmdrmpd.h $
* $brcm_Revision: 4 $
* $brcm_Date: 10/4/12 1:13p $
*
* API Description:
*   API name: wmdrmpd
*    Specific APIs related to Microsoft Windows Media DRM PD
*
* Revision History:
*
* $brcm_Log: /nexus/modules/wmdrmpd/include/nexus_wmdrmpd.h $
* 
* 4   10/4/12 1:13p erickson
* SW7425-4027: merge
* 
* SW7425-4027/1   10/3/12 2:24p hugost
* New API NEXUS_WmDrmPd_GetCustomLicenseChallenge()
* 
* 3   10/2/12 12:35p erickson
* SW7425-3986: add NEXUS_WmDrmPd_ProcessBlocksAesCounter, deprecate dma
*  handle
* 
* 2   8/23/12 3:57p jgarrett
* SW7425-3757: Merge to main branch
* 
* SW7425-3757/1   8/15/12 4:56p hugost
* Added the following APIs: NEXUS_WmDrmPd_GetDefaultPsshBoxInfo(),
*  NEXUS_WmDrmPd_SetPsshBox(),
*  NEXUS_WmDrmPd_GetDefaultProtectionSchemeInfo(),
*  NEXUS_WmDrmPd_SetProtectionSchemeBox() and
*  NEXUS_WmDrmPd_CleanupLicenseStore()..
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

#include "nexus_wmdrmpd_types.h"
#include "nexus_wmdrmpd_core.h"
#include "nexus_playpump.h"
#include "nexus_dma.h"
#include "nexus_security.h"

#ifndef NEXUS_WMDRMPD_H_
#define NEXUS_WMDRMPD_H_

#ifdef __cplusplus
extern "C" {
#endif


/***************************************************************************
Summary: 
WMDRMPD Handle
***************************************************************************/
typedef struct NEXUS_WmDrmPdCore *NEXUS_WmDrmPdHandle;

/***************************************************************************
Summary:
WMDRMPD settings.

Description:
Settings provided to create a WMDRMPD instance
***************************************************************************/
typedef struct NEXUS_WmDrmPdSettings
{
    NEXUS_HeapHandle heap;                  /* Optional heap to allocate internal data structures.  Must be CPU accessible. */
    NEXUS_PlaypumpHandle playpump;          /* Playpump handle required for ASF */
    NEXUS_DmaHandle dma;                    /* This parameter is unused and deprecated. */
    NEXUS_TransportType transportType;      /* Transport container used.  ASF and MP4 are supported. */
    NEXUS_CallbackDesc policyCallback;      /* Policy update callback */
} NEXUS_WmDrmPdSettings;

/***************************************************************************
Summary: 
Get WMDRMPD default settings. 
***************************************************************************/
void NEXUS_WmDrmPd_GetDefaultSettings(
    NEXUS_WmDrmPdSettings *pSettings    /* [out] default settings */
    );

/***************************************************************************
Summary: 
Create a WMDRMPD handle. 
***************************************************************************/
NEXUS_WmDrmPdHandle NEXUS_WmDrmPd_Create(
    const NEXUS_WmDrmPdSettings *pSettings  
    );

/***************************************************************************
Summary: 
Destroy a WMDRMPD handle. 
***************************************************************************/
void NEXUS_WmDrmPd_Destroy(
    NEXUS_WmDrmPdHandle handle
    );

/***************************************************************************
Summary: 
Setup a generic keyslot for DRM decryption. 
 
Description: 
This Keyslot should be provided to NEXUS_PlaypumpSettings.securityContext 
to link this DRM context with NEXUS_Playpump. 
***************************************************************************/
NEXUS_Error NEXUS_WmDrmPd_ConfigureKeySlot(
    NEXUS_WmDrmPdHandle handle,
    NEXUS_KeySlotHandle keySlot
    );

/***************************************************************************
Summary: 
Get Default ASF PSI Object Info
***************************************************************************/
void NEXUS_WmDrmPd_GetDefaultPsiObjectInfo(
    NEXUS_WmDrmPdPsiObjectInfo *pObject     /* [out] */
    );

/***************************************************************************
Summary: 
Set ASF PSI Object
***************************************************************************/
NEXUS_Error NEXUS_WmDrmPd_SetPsiObject(
    NEXUS_WmDrmPdHandle handle,
    const NEXUS_WmDrmPdPsiObjectInfo *pObjectInfo,
    const void *pData,  /* attr{nelem=dataLength;reserved=128} */
    size_t dataLength
    );

/***************************************************************************
Summary: 
Set ASF Cencr Object
***************************************************************************/
NEXUS_Error NEXUS_WmDrmPd_SetCencrObject(
    NEXUS_WmDrmPdHandle handle,
    const void *pSecurityData,  /* attr{nelem=securityLength;reserved=128} */
    size_t securityLength,      /* security data length in bytes */
    const void *pProtocolData,  /* attr{nelem=protocolLength;reserved=128} */
    size_t protocolLength,      /* protocol length in bytes */
    const void *pKeyIdData,     /* attr{nelem=keyIdLength;reserved=128} */
    size_t keyIdLength,         /* key id length in bytes */
    const void *pLicenseUrlData,/* attr{nelem=licenseUrlLength;reserved=128} */
    size_t licenseUrlLength     /* license url length in bytes */
    );

/***************************************************************************
Summary: 
Set ASF Xcencr Object
***************************************************************************/
NEXUS_Error NEXUS_WmDrmPd_SetXcencrObject(
    NEXUS_WmDrmPdHandle handle,
    const void *pData,  /* attr{nelem=dataLength;reserved=128} */
    size_t dataLength   /* extended encr size in bytes */
    );

/***************************************************************************
Summary: 
Get Default ASF Digsign Object Info
***************************************************************************/
void NEXUS_WmDrmPd_GetDefaultDigsignObjectInfo(
    NEXUS_WmDrmPdDigsignObjectInfo *pObject     /* [out] */
    );

/***************************************************************************
Summary: 
Set ASF Digsign Object
***************************************************************************/
NEXUS_Error NEXUS_WmDrmPd_SetDigsignObject(
    NEXUS_WmDrmPdHandle handle,
    const NEXUS_WmDrmPdDigsignObjectInfo *pInfo,
    const void *pData,  /* attr{nelem=dataLength;reserved=128} */
    size_t dataLength   /* extended encr size in bytes */
    );

/***************************************************************************
Summary: 
Get Default PSSH Box
***************************************************************************/
void NEXUS_WmDrmPd_GetDefaultPsshBoxInfo(
    NEXUS_WmDrmPdMp4PsshBoxInfo *pObject     /* [out] */
    );

/***************************************************************************
Summary: 
Set MP4 PSSH Box
***************************************************************************/
NEXUS_Error NEXUS_WmDrmPd_SetPsshBox(
    NEXUS_WmDrmPdHandle handle,
    const NEXUS_WmDrmPdMp4PsshBoxInfo *pInfo,
    const void *pData,   /* attr{nelem=dataLength;reserved=128} */
    size_t dataLength
    );

/***************************************************************************
Summary: 
Get Default Protection Scheme Information Box
***************************************************************************/
void NEXUS_WmDrmPd_GetDefaultProtectionSchemeInfo(
    NEXUS_WmDrmPdMp4ProtectionSchemeInfo *pObject     /* [out] */
    );

/***************************************************************************
Summary: 
Set Protection Scheme Information Box for a given track.
***************************************************************************/
NEXUS_Error NEXUS_WmDrmPd_SetProtectionSchemeBox(
    NEXUS_WmDrmPdHandle handle,
    const NEXUS_WmDrmPdMp4ProtectionSchemeInfo *pInfo
    );

/***************************************************************************
Summary: 
Attempt to Load a DRM License 
 
Description: 
After the file encryption objects have been set with one or more of the 
NEXUS_WmdrmPd_SetXxxxObject() routines, this function will check if a 
license is available in the license store for the content.  If a license 
is available, the routine will return NEXUS_SUCCESS.  If not, the 
routine will return NEXUS_NOT_AVAILABLE and the application will be 
responsible for calling NEXUS_WmDrmPd_GetLicenseChallenge and 
NEXUS_WmDrmPd_LicenseChallengeComplete(). 
 
See Also: 
NEXUS_WmDrmPd_SetPsiObject 
NEXUS_WmDrmPd_SetCencrObject 
NEXUS_WmDrmPd_SetXcencrObject 
NEXUS_WmDrmPd_SetDigsignObject 
NEXUS_WmDrmPd_GetLicenseChallenge 
***************************************************************************/
NEXUS_Error NEXUS_WmDrmPd_LoadLicense(
    NEXUS_WmDrmPdHandle handle
    );

/***************************************************************************
Summary: 
Request License Challenge Information 
 
Description: 
If an error is returned from NEXUS_WmDrmPd_LoadLicense, this routine should 
be called to get the license challenge information required to acquire 
a new license.  When the server responds with the challenge data, the 
application should call NEXUS_WmDrmPd_LicenseChallengeComplete().  If 
an error occurs (e.g. timeout), the application should call 
NEXUS_WmDrmPd_LicenseChallengeComplete(handle, 0, 0) to report the 
failure. 
 
See Also: 
NEXUS_WmDrmPd_LoadLicense 
NEXUS_WmDrmPd_LicenseChallengeComplete 
***************************************************************************/
NEXUS_Error NEXUS_WmDrmPd_GetLicenseChallenge(
    NEXUS_WmDrmPdHandle handle,
    NEXUS_WmDrmPdLicenseChallenge *pChallenge   /* [out] */
    );

/***************************************************************************
Summary: 
Request License Challenge Information 
 
Description: 
If an error is returned from NEXUS_WmDrmPd_LoadLicense, this routine should 
be called to get the license challenge information required to acquire 
a new license.

The application can specify custom data to be added to the challenge. If no
custom data is necessary, use NEXUS_WmDrmPd_GetLicenseChallenge() instead.

When the server responds with the challenge data, the 
application should call NEXUS_WmDrmPd_LicenseChallengeComplete().  If 
an error occurs (e.g. timeout), the application should call 
NEXUS_WmDrmPd_LicenseChallengeComplete(handle, 0, 0) to report the 
failure. 
 
See Also: 
NEXUS_WmDrmPd_LoadLicense 
NEXUS_WmDrmPd_LicenseChallengeComplete 
NEXUS_WmDrmPd_GetLicenseChallenge
***************************************************************************/
NEXUS_Error NEXUS_WmDrmPd_GetCustomLicenseChallenge(
    NEXUS_WmDrmPdHandle handle,
    const void *pCustomData, /* attr{nelem=customDataLength;reserved=128} */
    size_t customDataLength,
    NEXUS_WmDrmPdLicenseChallenge *pChallenge   /* [out] */
    );

/***************************************************************************
Summary: 
Send License Challenge Response 
 
Description: 
This call should indicate the response data length and offset to the 
response payload within the NEXUS_WmDrmPdLicenseChallenge.pResponseBuffer 
buffer.  If an error occurred (e.g. timeout), pass an offset and length 
of 0 into this routine. 
 
See Also: 
NEXUS_WmDrmPd_GetLicenseChallenge 
***************************************************************************/
NEXUS_Error NEXUS_WmDrmPd_LicenseChallengeComplete(
    NEXUS_WmDrmPdHandle handle,
    uint32_t responseLength,        /* Response length in bytes */
    unsigned responseOffset         /* Offset to the start of the response within the data buffer */
    );

/***************************************************************************
Summary: 
Get License Policy Status
***************************************************************************/
NEXUS_Error NEXUS_WmDrmPd_GetPolicyStatus(
    NEXUS_WmDrmPdHandle handle,
    NEXUS_WmDrmPdPolicyStatus *pStatus  /* [out] */
    );

/***************************************************************************
Summary: 
Deletes all expired licenses from the license store and performs maintenance
on the data store file.
***************************************************************************/
void NEXUS_WmDrmPd_CleanupLicenseStore(
    NEXUS_WmDrmPdHandle handle
    );

/***************************************************************************
Summary: 
Get Default ASE counter mode Information.
***************************************************************************/
void NEXUS_WmDrmPd_GetDefaultAesCounterInfo(
    NEXUS_WmdrmPdAesCounterInfo *pInfo
    );

/***************************************************************************
Summary: 
Does an AES-CTR operation on the DMA blocks.
 
Description: 
This function does an AES CTR operation using the Playready content key. 
This function is useful for application models where the data must be decrypted
before is feed to the play pump. Like in the case of Smooth Streaming for example
when the SDK feeds decrypt ES packets to the play pump.

***************************************************************************/
NEXUS_Error NEXUS_WmDrmPd_ProcessBlocksAesCounter(
    NEXUS_WmDrmPdHandle handle,
    const NEXUS_WmdrmPdAesCounterInfo *pInfo,
    const NEXUS_DmaJobBlockSettings *pDmaBlocks,  /* attr{nelem=nBlocks;reserved=4} pointer to array of DMA blocks */
    unsigned nBlocks                              /* Must be < NEXUS_DmaJobSettings.numBlocks */
    );

#ifdef __cplusplus
}
#endif

#endif /* #ifndef NEXUS_WMDRMPD_H_ */
