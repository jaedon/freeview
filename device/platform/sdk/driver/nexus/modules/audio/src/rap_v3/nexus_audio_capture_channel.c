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
* $brcm_Workfile: nexus_audio_capture_channel.c $
* $brcm_Revision: 16 $
* $brcm_Date: 2/21/12 2:22p $
*
* API Description:
*   Capture channel resource manager.
*
* Revision History:
*
* $brcm_Log: /nexus/modules/audio/7405/src/nexus_audio_capture_channel.c $
* 
* 16   2/21/12 2:22p erickson
* SW7425-2130: fix debug warnings
* 
* 15   8/12/10 6:25p akam
* SW35230-211: Replaced BCHP_CHIP check with BRAP_VER.
* 
* 14   7/2/10 11:00a akam
* SW35230-211: Modified bHbrMode and audio description per 35230 RAP PI
*  changes.
* 
* 13   5/21/10 12:03p akam
* SW35230-48: Fix Nexus audio build for 35230. Removed most temporary
*  #ifdef for 35230.
* 
* 12   5/11/10 4:18p akam
* SW35230-211: Update Nexus audio module code per 35230 RAP PI changes.
* 
* 11   3/31/10 5:29p akam
* SW35230-48: Fix build for 35230.
* 
* 10   4/6/09 5:25p jgarrett
* PR 53393: Adding memory configuration options
* 
* 9   3/2/09 5:37p jgarrett
* PR 51468: Renaming globals to match naming conventions
* 
* 8   11/26/08 6:14p jgarrett
* PR 48983: Initializing number of output channel pairs
* 
* 7   11/13/08 5:01p jgarrett
* PR 48983: Disabling independent delay on capture channels
* 
* 6   11/12/08 3:49p jgarrett
* PR 48920: Adding a global independent delay flag that affects all
*  channels
* 
* 5   7/28/08 5:17p jgarrett
* PR 45178: Fixing inverted mute logic
* 
* 4   6/19/08 3:17p jgarrett
* PR 42243: Moving input volume to specific inputs and out of mixer
* 
* 3   4/21/08 12:53p jgarrett
* PR 41621: Adding resource allocation debug
* 
* 2   4/15/08 12:36p jgarrett
* PR 41621: Reworking association logic
* 
* 1   1/18/08 2:22p jgarrett
* PR 38808: Merging to main branch
* 
* Nexus_Devel/1   1/16/08 6:47p jgarrett
* PR 38753: Adding capture channel resource management
* 
***************************************************************************/

#include "nexus_audio_module.h"

BDBG_MODULE(nexus_audio_capture_channel);

#if NEXUS_NUM_AUDIO_CAPTURE_CHANNELS

static BRAP_ChannelHandle g_audioCaptureChannels[NEXUS_NUM_AUDIO_CAPTURE_CHANNELS];
static bool g_captureChannelAcquired[NEXUS_NUM_AUDIO_CAPTURE_CHANNELS];

/***************************************************************************
Summary:
	Init the capture channel manager.
***************************************************************************/
NEXUS_Error NEXUS_AudioCaptureChannel_P_Init(void)
{
    int i;
    BERR_Code errCode;
    BRAP_ChannelSettings channelSettings;

    BKNI_Memset(g_captureChannelAcquired, 0, sizeof(g_captureChannelAcquired));
    BKNI_Memset(g_captureChannelAcquired, 0, sizeof(g_captureChannelAcquired));

    BRAP_GetDefaultChannelSettings(g_NEXUS_audioModuleData.hRap, &channelSettings);
    channelSettings.eChType = BRAP_ChannelType_ePcmCapture;
#if BRAP_VER < 4
    channelSettings.eChSubType = BRAP_ChannelSubType_eNone;
#endif
    channelSettings.sChnRBufPool.uiMaxNumInChPairs[0]=1;
    #if BCHP_CHIP == 3548 || BCHP_CHIP == 3556
    channelSettings.sChnRBufPool.uiMaxNumOutChPairs[0]=1;
    #endif
    #if NEXUS_AUDIO_CAPTURE_CHANNEL_POST_PROCESSING_ENABLED
    channelSettings.sChnRBufPool.uiMaxNumOutChPairs[0]+=g_NEXUS_audioModuleData.moduleSettings.maximumProcessingBranches;
    #endif
    channelSettings.sChnRBufPool.uiMaxNumRBuf=0;	
    channelSettings.sChnRBufPool.sBufSettings.pLeftBufferStart=NULL;	
    channelSettings.sChnRBufPool.sBufSettings.pRightBufferStart=NULL;	
    channelSettings.sChnRBufPool.sBufSettings.uiSize=0;	
    channelSettings.sChnRBufPool.sBufSettings.uiWaterMark=0; 
    channelSettings.bEnaIndDly = g_NEXUS_audioModuleData.moduleSettings.independentDelay;

#if BRAP_VER >= 4
    channelSettings.sChnRBufPool.bHbrMode = g_NEXUS_audioModuleData.moduleSettings.hbrEnabled;
#endif

    for ( i = 0; i < NEXUS_NUM_AUDIO_CAPTURE_CHANNELS; i++ )
    {
        errCode = BRAP_OpenChannel(g_NEXUS_audioModuleData.hRap, &g_audioCaptureChannels[i], &channelSettings);
        if ( errCode )
        {
            errCode = BERR_TRACE(errCode);
            goto error;
        }
    }

    return BERR_SUCCESS;
error:
    NEXUS_AudioCaptureChannel_P_Uninit();
    return errCode;
}

/***************************************************************************
Summary:
	Uninit the capture channel manager.
***************************************************************************/
NEXUS_Error NEXUS_AudioCaptureChannel_P_Uninit(void)
{
    int i;

    for ( i = 0; i < NEXUS_NUM_AUDIO_CAPTURE_CHANNELS; i++ )
    {
        if ( NULL != g_audioCaptureChannels[i] )
        {
            BRAP_CloseChannel(g_audioCaptureChannels[i]);
            g_audioCaptureChannels[i] = NULL;
        }
    }

    return BERR_SUCCESS;
}

/***************************************************************************
Summary:
	Acquire a channel from the resource manager.
***************************************************************************/
BRAP_ChannelHandle NEXUS_AudioCaptureChannel_P_Acquire_tagged(const char *file, int line)
{
    int i;

    #if !BDBG_NO_MSG
    BDBG_MSG(("Acquiring capture channel from %s:%d", file, line));
    #else
    BSTD_UNUSED(file);
    BSTD_UNUSED(line);
    #endif

    for ( i = 0; i < NEXUS_NUM_AUDIO_CAPTURE_CHANNELS; i++ )
    {
        if ( false == g_captureChannelAcquired[i] )
        {
            g_captureChannelAcquired[i] = true;
            return g_audioCaptureChannels[i];
        }
    }

    BDBG_ERR(("Out of capture channels"));
    return NULL;
}

/***************************************************************************
Summary:
	Release an acquired channel back to the resource manager.
***************************************************************************/
void NEXUS_AudioCaptureChannel_P_Release_tagged(BRAP_ChannelHandle handle, const char *file, int line)
{
    int i;

    #if !BDBG_NO_MSG
    BDBG_MSG(("Releasing capture channel from %s:%d", file, line));
    #else
    BSTD_UNUSED(file);
    BSTD_UNUSED(line);
    #endif

    BDBG_ASSERT(NULL != handle);

    for ( i = 0; i < NEXUS_NUM_AUDIO_CAPTURE_CHANNELS; i++ )
    {
        if ( g_audioCaptureChannels[i] == handle )
        {
            BDBG_ASSERT(g_captureChannelAcquired[i] == true);
            g_captureChannelAcquired[i] = false;
            return;
        }
    }

    BDBG_ERR(("Invalid channel handle specified"));
    BDBG_ASSERT(0);
}

/***************************************************************************
Summary:
	Set volume of a capture channel
***************************************************************************/
NEXUS_Error NEXUS_AudioCaptureChannel_P_SetVolume(BRAP_ChannelHandle handle, int32_t left, int32_t right, bool mute)
{
    BRAP_ChannelConfigParams *pConfig;
    NEXUS_Error errCode;

    if ( NULL == handle )
    {
        /* Fail silently */
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }
    pConfig = BKNI_Malloc(sizeof(*pConfig)); /* must malloc this large structure to avoid possible stack overflow */
    if ( NULL == pConfig )
    {
        return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
    }

    errCode = BRAP_GetCurrentChannelConfig(handle,
                                           BRAP_DSPCHN_AudioType_eInvalid,
                                           #if BCHP_CHIP == 3563
                                           BRAP_MAX_PP_BRANCH_SUPPORTED,
                                           BRAP_MAX_PP_PER_BRANCH_SUPPORTED,
                                           #endif
                                           pConfig);
    if ( BERR_SUCCESS == errCode )
    {
        if ( mute )
        {
            pConfig->sMixingCoef.ui32Coef[BRAP_OutputChannel_eLeft][BRAP_OutputChannel_eLeft] = 0;
            pConfig->sMixingCoef.ui32Coef[BRAP_OutputChannel_eRight][BRAP_OutputChannel_eRight] = 0;
        }
        else
        {
            pConfig->sMixingCoef.ui32Coef[BRAP_OutputChannel_eLeft][BRAP_OutputChannel_eLeft] = left;
            pConfig->sMixingCoef.ui32Coef[BRAP_OutputChannel_eRight][BRAP_OutputChannel_eRight] = right;
        }
    
        errCode = BRAP_SetChannelConfig(handle, pConfig);
        if ( errCode )
        {
            errCode = BERR_TRACE(errCode);
        }    
    }
    else
    {
        errCode = BERR_TRACE(errCode);
    }

    BKNI_Free(pConfig);
    return errCode;
}
#endif

