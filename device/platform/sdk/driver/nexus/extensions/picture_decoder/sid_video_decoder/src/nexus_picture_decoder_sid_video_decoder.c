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
* $brcm_Workfile: nexus_picture_decoder_sid_video_decoder.c $
* $brcm_Revision: 6 $
* $brcm_Date: 3/1/12 1:26p $
*
* API Description:
*
* Revision History:
*
* $brcm_Log: /nexus/extensions/picture_decoder/sid_video_decoder/7425/src/nexus_picture_decoder_sid_video_decoder.c $
* 
* 6   3/1/12 1:26p gmohile
* SW7425-2401 : set power state only during open/close
* 
* 5   3/1/12 12:41p gmohile
* SW7425-2401 : Add power management for picture decoder extension
* 
* 4   2/22/12 1:52p vsilyaev
* SW7425-2258: Improved handling of MJPEG decoders
* 
* 3   2/21/12 12:36p vsilyaev
* SW7425-2112: Merged MJPEG code
* 
* SW7425-2112/5   2/21/12 3:24p fbasso
* SW7425-2112: code clean up
*
* SW7425-2112/4   2/16/12 4:53p fbasso
* SW7425-2112: added support for channel change mode (hold last picture.
*
* SW7425-2112/3   2/15/12 11:27a fbasso
* SW7425-2112: Using BSID_GetRaveItbCbdConfigInfo to retrieve RAVE
*  configuration to use during video decoding.
*
* SW7425-2112/2   2/10/12 4:08p fbasso
* SW7425-2112: SID PI integration for motion jpeg video decode.
*
* SW7425-2112/1   2/10/12 10:51a fbasso
* SW7425-2112: SID PI integration for motion jpeg video decode.
*
*
***************************************************************************/

#include "nexus_picture_decoder_module.h"
#include "nexus_power_management.h"
#include "priv/nexus_core.h"

BDBG_MODULE(nexus_picture_decoder_sid_video_decoder);

BDBG_OBJECT_ID(NEXUS_SidVideoDecoder);

struct NEXUS_SidVideoDecoder {
    BDBG_OBJECT(NEXUS_SidVideoDecoder)
    bool opened;
    bool started;
    NEXUS_RaveStatus raveStatus;
    NEXUS_SidVideoDecoderStartSettings startSettings;
    BXDM_Decoder_Interface decoderInterface; /* keep a persisitent copy */
    BSID_ChannelHandle hSidCh;
};

static struct NEXUS_SidVideoDecoder g_decoders[NEXUS_NUM_SID_VIDEO_DECODERS];
static BKNI_EventHandle g_watchdogEvent;
static NEXUS_EventCallbackHandle g_watchdogCallback;

static void NEXUS_SidVideoDecoder_P_Watchdog(void *pParam);

#define LOCK_TRANSPORT()    NEXUS_Module_Lock(g_NEXUS_PictureDecoder_P_ModuleState.settings.transport)
#define UNLOCK_TRANSPORT()  NEXUS_Module_Unlock(g_NEXUS_PictureDecoder_P_ModuleState.settings.transport)

void NEXUS_SidVideoDecoder_GetDefaultStartSettings_priv( NEXUS_SidVideoDecoderStartSettings *pSettings )
{
    NEXUS_ASSERT_MODULE();
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    return;
}

void NEXUS_SidVideoDecoder_GetDefaultOpenSettings_priv( NEXUS_SidVideoDecoderOpenSettings *pSettings )
{
    NEXUS_ASSERT_MODULE();
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    pSettings->pictureHeap = g_pCoreHandles->heap[0];
    return;
}

NEXUS_Error NEXUS_SidVideoDecoder_SetSettings_priv(NEXUS_SidVideoDecoderHandle decoder, const NEXUS_SidVideoDecoderSettings *pSettings)
{
    NEXUS_Error rc = NEXUS_SUCCESS;

    BSTD_UNUSED(decoder);
    BSTD_UNUSED(pSettings);

    return rc;
}


NEXUS_SidVideoDecoderHandle NEXUS_SidVideoDecoder_Open_priv(unsigned index, const NEXUS_SidVideoDecoderOpenSettings *openSettings)
{
    NEXUS_SidVideoDecoderHandle  decoder;
    NEXUS_Error rc;

    NEXUS_ASSERT_MODULE();
    BSTD_UNUSED(openSettings);
    if(index>=NEXUS_NUM_SID_VIDEO_DECODERS) { rc=BERR_TRACE(NEXUS_INVALID_PARAMETER); goto err_args;}

    decoder = &g_decoders[index];
    if ( decoder->opened )
    {
        BDBG_ERR(("SID Video Decoder %u already opened", index));
        rc = BERR_TRACE(BERR_INVALID_PARAMETER);
        goto err_args;
    }

    BKNI_Memset(decoder, 0, sizeof(*decoder));
    BDBG_OBJECT_SET(decoder, NEXUS_SidVideoDecoder);
    decoder->opened = true;

    /* SID open channel */
    {
        BERR_Code retCode;
        BSID_Handle hSid = g_NEXUS_PictureDecoder_P_ModuleState.hwState.sid;
        BSID_OpenChannelSettings s_SidChOpenSettings;

        BSID_GetOpenChannelSettings(BSID_ChannelType_eMotion, &s_SidChOpenSettings);

        s_SidChOpenSettings.e_ChannelType = BSID_ChannelType_eMotion;
        s_SidChOpenSettings.u_ChannelSpecific.motion.hOutputBuffersHeap = openSettings->pictureHeap;
        s_SidChOpenSettings.u_ChannelSpecific.motion.ui32_OutputBuffersNumber = 3;
        s_SidChOpenSettings.u_ChannelSpecific.motion.ui32_OutputMaxWidth = 1920;
        s_SidChOpenSettings.u_ChannelSpecific.motion.ui32_OutputMaxHeight = 1080;

        BDBG_MSG(("BSID_OpenChannel"));

        NEXUS_PowerManagement_SetCoreState(NEXUS_PowerManagementCore_ePictureDecoder, true);
        retCode = BSID_OpenChannel(hSid, &g_decoders[index].hSidCh, 0xAA, &s_SidChOpenSettings);	
        if (retCode != BERR_SUCCESS)
        {
            BDBG_ERR(("BSID_OpenChannel failed err=0x%x", retCode));
            rc = BERR_TRACE(BERR_INVALID_PARAMETER);
            goto err_args;
        }
    }

    return decoder;

err_args:
    return NULL;
}

void NEXUS_SidVideoDecoder_Close_priv(NEXUS_SidVideoDecoderHandle decoder)
{
    NEXUS_ASSERT_MODULE();

    BDBG_OBJECT_ASSERT(decoder, NEXUS_SidVideoDecoder);
    /*BDBG_OBJECT_DESTROY(decoder, NEXUS_SidVideoDecoder);*/

    /* sid close channel */
    {
        BSID_CloseChannel(decoder->hSidCh);
        NEXUS_PowerManagement_SetCoreState(NEXUS_PowerManagementCore_ePictureDecoder, false);
    }

    return;
}

void NEXUS_SidVideoDecoder_GetDecoderInterface_priv(NEXUS_SidVideoDecoderHandle decoder, const BXDM_Decoder_Interface **decoderInterface, void **decoderContext)
{
    NEXUS_ASSERT_MODULE();
    BDBG_OBJECT_ASSERT(decoder, NEXUS_SidVideoDecoder);
    BSTD_UNUSED(decoderContext);

    /* SID get xdm interface */
    {
        BSID_GetXdmInterface(decoder->hSidCh, &decoder->decoderInterface, decoderContext);
    }

    *decoderInterface = &decoder->decoderInterface;
    return;
}

static const struct {
    NEXUS_VideoCodec nexusCodec;
    BAVC_VideoCompressionStd avcCodec;
} NEXUS_SidVideoDecoder_P_CodecTable[] = {
    {NEXUS_VideoCodec_eVp6,  BAVC_VideoCompressionStd_eVP6}
};

NEXUS_Error NEXUS_SidVideoDecoder_Start_priv(NEXUS_SidVideoDecoderHandle decoder, const NEXUS_SidVideoDecoderStartSettings *startSettings)
{
    BERR_Code rc;

    NEXUS_ASSERT_MODULE();
    BDBG_OBJECT_ASSERT(decoder, NEXUS_SidVideoDecoder);

    if ( decoder->started )
    {
        BDBG_ERR(("Already running."));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    if(startSettings->raveContext) {
        LOCK_TRANSPORT();
        rc = NEXUS_Rave_GetStatus_priv(startSettings->raveContext, &decoder->raveStatus);
        UNLOCK_TRANSPORT();
        if(rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc);goto  err_rave_status;}
    }

    /* SID start decode */
    {
        BERR_Code retCode = BERR_SUCCESS;
        BSID_StartDecodeSettings gs_StartDecodeSettings;

        gs_StartDecodeSettings.ps_RaveContextMap = &decoder->raveStatus.xptContextMap;	

        BDBG_MSG(("BSID_StartDecode"));	
        retCode = BSID_StartDecode(decoder->hSidCh, &gs_StartDecodeSettings);
        if (retCode != BERR_SUCCESS)
        {	    
            BDBG_ERR(("BSID_StartDecode failed err=0x%x", retCode));
            rc=BERR_TRACE(NEXUS_UNKNOWN);
            goto  err_start_decode;
        }
    }

    decoder->startSettings = *startSettings;
    decoder->started = true;

    return NEXUS_SUCCESS;

err_start_decode:
err_rave_status:
    return BERR_TRACE(rc);
}

void NEXUS_SidVideoDecoder_Stop_priv(NEXUS_SidVideoDecoderHandle decoder)
{
    NEXUS_ASSERT_MODULE();
    BDBG_OBJECT_ASSERT(decoder, NEXUS_SidVideoDecoder);

    /* SID stop decode */
    {
        BDBG_MSG(("BSID_StopDecode"));

        BSID_StopDecode(decoder->hSidCh);
    }    

    decoder->started = false;
    return;
}

NEXUS_Error NEXUS_SidVideoDecoder_Flush_priv(NEXUS_SidVideoDecoderHandle decoder)
{
    NEXUS_ASSERT_MODULE();
    BDBG_OBJECT_ASSERT(decoder, NEXUS_SidVideoDecoder);

    if ( !decoder->started )
    {
        BDBG_ERR(("Decoder is not running.  Cannot flush."));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    /* SID disable for flush decode */
    {
        BERR_Code retCode = BERR_SUCCESS;

        BDBG_MSG(("BSID_DisableForFlush"));

        retCode = BSID_DisableForFlush(decoder->hSidCh);
        if (retCode != BERR_SUCCESS)
        {
            BDBG_ERR(("BSID_StopDecode failed err=0x%x", retCode));
            return BERR_TRACE(NEXUS_UNKNOWN);
        }
    }


    if(decoder->startSettings.raveContext) {
        LOCK_TRANSPORT();
        NEXUS_Rave_Disable_priv(decoder->startSettings.raveContext);
        NEXUS_Rave_Flush_priv(decoder->startSettings.raveContext);
        UNLOCK_TRANSPORT();
    }

    /* SID flush channel */
    {
        BERR_Code retCode = BERR_SUCCESS;

        BDBG_MSG(("BSID_FlushChannel"));

        retCode = BSID_FlushChannel(decoder->hSidCh, NULL);
        if (retCode != BERR_SUCCESS)
        {
            BDBG_ERR(("BSID_FlushChannel failed err=0x%x", retCode));
            return BERR_TRACE(NEXUS_UNKNOWN);
        }
    }

    if(decoder->startSettings.raveContext) {
        LOCK_TRANSPORT();
        NEXUS_Rave_Enable_priv(decoder->startSettings.raveContext);
        UNLOCK_TRANSPORT();
    }

    return NEXUS_SUCCESS;
}

void NEXUS_SidVideoDecoder_GetRaveSettings_priv(NEXUS_RaveOpenSettings *raveSettings)
{
    NEXUS_ASSERT_MODULE();

    /* SID Get default Itb/Cdb configuration */
    {
        BSID_GetRaveItbCdbConfigInfo(&raveSettings->config);
    }

    return;
}

NEXUS_Error NEXUS_SidVideoDecoder_P_InitWatchdog(void)
{
    BERR_Code errCode;

    errCode = BKNI_CreateEvent(&g_watchdogEvent);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }
    g_watchdogCallback = NEXUS_RegisterEvent(g_watchdogEvent, NEXUS_SidVideoDecoder_P_Watchdog, NULL);
    if ( NULL == g_watchdogCallback )
    {
        BKNI_DestroyEvent(g_watchdogEvent);
        g_watchdogEvent = NULL;
        return BERR_TRACE(BERR_OS_ERROR);
    }

    /* Allow watchdog to be disabled for debugging */
    if ( !NEXUS_GetEnv("no_watchdog") )
    {
    }

    return BERR_SUCCESS;
}

void NEXUS_SidVideoDecoder_P_UninitWatchdog(void)
{
    NEXUS_UnregisterEvent(g_watchdogCallback);
    g_watchdogCallback = NULL;
    BKNI_DestroyEvent(g_watchdogEvent);
    g_watchdogEvent = NULL;
    return;
}

#if 0
static void NEXUS_SidVideoDecoder_P_Watchdog_isr(void *pParam1, int param2)
{
    BSTD_UNUSED(pParam1);
    BSTD_UNUSED(param2);
    BDBG_ERR(("SID Video Watchdog Interrupt Received"));
    BKNI_SetEvent_isr(g_watchdogEvent);
    return;
}
#endif

static void NEXUS_SidVideoDecoder_P_Watchdog(void *pParam)
{
    unsigned i;

    BSTD_UNUSED(pParam);

    LOCK_TRANSPORT();
    for ( i = 0; i < NEXUS_NUM_SID_VIDEO_DECODERS; i++ )
    {
        if ( g_decoders[i].opened && g_decoders[i].started )
        {
            NEXUS_Rave_Disable_priv(g_decoders[i].startSettings.raveContext);
            NEXUS_Rave_Flush_priv(g_decoders[i].startSettings.raveContext);
        }
    }
    UNLOCK_TRANSPORT();

    /* Process watchdog event */

    /* Restart RAVE contexts */
    LOCK_TRANSPORT();
    for ( i = 0; i < NEXUS_NUM_SID_VIDEO_DECODERS; i++ )
    {
        if ( g_decoders[i].opened && g_decoders[i].started )
        {
            NEXUS_Rave_Enable_priv(g_decoders[i].startSettings.raveContext);
        }
    }
    UNLOCK_TRANSPORT();
}

