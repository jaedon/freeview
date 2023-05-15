/***************************************************************************
*     (c)2004-2011 Broadcom Corporation
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
* $brcm_Workfile: nexus_audio_return_channel.c $
* $brcm_Revision: 1 $
* $brcm_Date: 8/18/11 5:51p $
*
* API Description:
*   API name: Audio Return Channel (ARC)
*    Specific APIs related to Audio Return Channel output.
*
* Revision History:
*
* $brcm_Log: /nexus/modules/audio/7422/src/nexus_audio_return_channel.c $
* 
* 1   8/18/11 5:51p jgarrett
* SWDTV-6306: Merge DTV APE changes to main branch
* 
* Nexus_APE_Integration/1   7/1/11 5:50p jgarrett
* SWDTV-6306: Integrated to latest 7422 baseline and nexus audio
*  directory structure
* 
* 2   5/13/11 1:57p gskerl
* SW7422-354: Added APE support for Audio Return Channel
* 
* 1   5/5/11 11:44a jgarrett
* SW7425-354: Adding 35230 stubs
* 
***************************************************************************/
#include "nexus_audio_module.h"

#if NEXUS_NUM_AUDIO_RETURN_CHANNEL

BDBG_MODULE(nexus_audio_return_channel);

BDBG_OBJECT_ID(NEXUS_AudioArc);

typedef struct NEXUS_AudioArc
{
    BDBG_OBJECT(NEXUS_AudioArc)
    bool opened;
    unsigned index;
    BAPE_AudioReturnChannelHandle bapeArcHandle;

    NEXUS_AudioReturnChannelSettings settings;
    NEXUS_AudioOutputObject connector;
} NEXUS_AudioArc;

static NEXUS_AudioArc g_arc[NEXUS_NUM_AUDIO_RETURN_CHANNEL];


/***************************************************************************
Summary:
    Get default settings for an Audio Return Channel Output
 ***************************************************************************/
void NEXUS_AudioReturnChannel_GetDefaultSettings(
    NEXUS_AudioReturnChannelSettings *pSettings   /* [out] default settings */
    )
{
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    pSettings->muted = false;
}


/***************************************************************************
Summary:
    Open an Audio Return Channel Output
 ***************************************************************************/
NEXUS_AudioReturnChannelHandle NEXUS_AudioReturnChannel_Open(
    unsigned index,
    const NEXUS_AudioReturnChannelSettings *pSettings     /* Pass NULL for default settings */
    )
{
    NEXUS_AudioReturnChannelHandle handle;
    BAPE_AudioReturnChannelHandle bapeArcHandle;
    NEXUS_Error errCode;

    /* Sanity check */
    if ( index >= NEXUS_NUM_AUDIO_RETURN_CHANNEL )
    {
        BDBG_ERR(("Arc Index %u out of range", index));
        errCode = BERR_TRACE(BERR_INVALID_PARAMETER);
        return NULL;
    }

    handle = &g_arc[index];
    if ( handle->opened )
    {
        BDBG_ERR(("Arc %u already open", index));
        errCode = BERR_TRACE(BERR_INVALID_PARAMETER);
        return NULL;
    }

    errCode = BAPE_AudioReturnChannel_Open( NEXUS_AUDIO_DEVICE_HANDLE, index, NULL, &bapeArcHandle );
    if ( errCode )
    {
        (void)BERR_TRACE(errCode);
        return NULL;
    }

    BKNI_Memset(handle, 0, sizeof(NEXUS_AudioArc));
    BDBG_OBJECT_SET(handle, NEXUS_AudioArc);
    handle->opened = true;
    handle->index = index;
    handle->bapeArcHandle = bapeArcHandle;

    /* Initialize connector */
    NEXUS_AUDIO_OUTPUT_INIT(&handle->connector, NEXUS_AudioOutputType_eArc, handle);

    errCode = NEXUS_AudioReturnChannel_SetSettings( handle, pSettings );

    return handle;
}


/***************************************************************************
Summary:
    Close an Audio Return Channel Output
 ***************************************************************************/
void NEXUS_AudioReturnChannel_Close(
    NEXUS_AudioReturnChannelHandle handle
    )
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_AudioArc);
    BDBG_ASSERT(handle->opened);

    NEXUS_AudioOutput_Shutdown(&handle->connector);
    BAPE_AudioReturnChannel_Close(handle->bapeArcHandle);
    /* This is equivalent to BDBG_OBJECT_UNSET and also clears other flags */
    BKNI_Memset(handle, 0, sizeof(NEXUS_AudioArc));
}


/***************************************************************************
Summary:
    Get the settings of an Audio Return Channel Output
 ***************************************************************************/
void NEXUS_AudioReturnChannel_GetSettings(
    NEXUS_AudioReturnChannelHandle handle,
    NEXUS_AudioReturnChannelSettings *pSettings    /* [out] Settings */
    )
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_AudioArc);
    BDBG_ASSERT(NULL != pSettings);

    *pSettings = handle->settings;
}


/***************************************************************************
Summary:
    Set the settings of an Audio Return Channel Output
 ***************************************************************************/
NEXUS_Error NEXUS_AudioReturnChannel_SetSettings(
    NEXUS_AudioReturnChannelHandle handle,
    const NEXUS_AudioReturnChannelSettings *pSettings    /* [in] Settings */
    )
{
    NEXUS_AudioReturnChannelSettings settings;
    BAPE_AudioReturnChannelSettings bapeArcSettings;
    NEXUS_Error errCode;

    /* Sanity Check */
    BDBG_OBJECT_ASSERT(handle, NEXUS_AudioArc);

    if ( NULL == pSettings )
    {
        NEXUS_AudioReturnChannel_GetDefaultSettings(&settings);
        pSettings = &settings;
    }
    BAPE_AudioReturnChannel_GetSettings(handle->bapeArcHandle, &bapeArcSettings);

    bapeArcSettings.muted = pSettings->muted;

    errCode = BAPE_AudioReturnChannel_SetSettings(handle->bapeArcHandle, &bapeArcSettings);
    if (errCode)
    {
        return BERR_TRACE(errCode);
    }

    handle->settings = *pSettings;

    /* success */
    return BERR_SUCCESS;
}


/***************************************************************************
Summary:
    Get the NEXUS_AudioOutput connector for an Audio Return Channel Output
 ***************************************************************************/
NEXUS_AudioOutput NEXUS_AudioReturnChannel_GetConnector(
    NEXUS_AudioReturnChannelHandle handle
    )
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_AudioArc);
    return &handle->connector;
}


/***************************************************************************
Summary:
    Set the master Spdif source for an Audio Return Channel output
 ***************************************************************************/
NEXUS_Error NEXUS_AudioReturnChannel_P_SetMaster(
    NEXUS_AudioOutput slaveHandle,      /* handle of ARC (slave) output device */
    NEXUS_AudioOutput sourceHandle      /* handle of SPDIF master */
    )
{
    NEXUS_Error errCode;
    BAPE_AudioReturnChannelHandle apeArcHandle;
    BAPE_AudioReturnChannelSettings apeArcSettings;
    NEXUS_AudioReturnChannelHandle nexusArcHandle;

    BDBG_ASSERT(slaveHandle->objectType == NEXUS_AudioOutputType_eArc);
    BDBG_ASSERT(NULL != slaveHandle->pObjectHandle);

    nexusArcHandle = slaveHandle->pObjectHandle;
    apeArcHandle = nexusArcHandle->bapeArcHandle;

    BAPE_AudioReturnChannel_GetSettings(apeArcHandle, &apeArcSettings);

    if ( NULL == sourceHandle )
    {
        apeArcSettings.master = NULL;
    }
    else
    {
        BDBG_ASSERT(sourceHandle->objectType == NEXUS_AudioOutputType_eSpdif);
        apeArcSettings.master = (BAPE_OutputPort)sourceHandle->port;
    }

    errCode = BAPE_AudioReturnChannel_SetSettings(apeArcHandle, &apeArcSettings);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }
    return BERR_SUCCESS;
}


/***************************************************************************
    Define stubs for when NEXUS_NUM_AUDIO_RETURN_CHANNEL == 0
 ***************************************************************************/
#else
void NEXUS_AudioReturnChannel_GetDefaultSettings(
    NEXUS_AudioReturnChannelSettings *pSettings   /* [out] default settings */
    )
{
    BSTD_UNUSED(pSettings);
}

NEXUS_AudioReturnChannelHandle NEXUS_AudioReturnChannel_Open(
    unsigned index,
    const NEXUS_AudioReturnChannelSettings *pSettings     /* Pass NULL for default settings */
    )
{
    BSTD_UNUSED(index);
    BSTD_UNUSED(pSettings);    
    return NULL;
}

void NEXUS_AudioReturnChannel_Close(
    NEXUS_AudioReturnChannelHandle handle
    )
{
    BSTD_UNUSED(handle);
}

void NEXUS_AudioReturnChannel_GetSettings(
    NEXUS_AudioReturnChannelHandle handle,
    NEXUS_AudioReturnChannelSettings *pSettings    /* [out] Settings */
    )
{
    BSTD_UNUSED(handle);
    BSTD_UNUSED(pSettings);
}

NEXUS_Error NEXUS_AudioReturnChannel_SetSettings(
    NEXUS_AudioReturnChannelHandle handle,
    const NEXUS_AudioReturnChannelSettings *pSettings    /* [in] Settings */
    )
{
    BSTD_UNUSED(handle);
    BSTD_UNUSED(pSettings);
    return BERR_TRACE(BERR_NOT_SUPPORTED);
}

NEXUS_AudioOutput NEXUS_AudioReturnChannel_GetConnector(
    NEXUS_AudioReturnChannelHandle handle
    )
{
    BSTD_UNUSED(handle);
    return NULL;
}

#endif /* NEXUS_NUM_AUDIO_RETURN_CHANNEL */






