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
* $brcm_Workfile: nexus_audio_mux_output.c $
* $brcm_Revision: 1 $
* $brcm_Date: 7/30/12 4:36p $
*
* Revision History:
*
* $brcm_Log: /nexus/modules/audio/src/rap_v1/nexus_audio_mux_output.c $
* 
* 1   7/30/12 4:36p jgarrett
* SW7435-266: Switching to unified audio module structure
* 
* 11   6/20/12 1:29p vsilyaev
* SW7420-2085: Really fixing build
* 
* 10   6/20/12 1:11p jgarrett
* SW7420-2085: Fixing build when NEXUS_HAS_AUDIO_MUX_OUTPUT is not
*  defined
* 
* 9   6/19/12 12:41p vsilyaev
* SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
* 
* 8   6/15/12 2:31p jgarrett
* SW7425-3258: Correcting comments regarding size parameters to GetBuffer
* 
* 7   4/20/12 5:23p jgarrett
* SW7231-774: Resetting MuxOutput on a watchdog interrupt
* 
* 6   10/14/11 2:54p jgarrett
* SW7425-1389: Coverity CID 33805
* 
* 5   6/21/11 5:04p jgarrett
* SW7425-654: Adding Audio NRT APIs
* 
* 4   4/19/11 5:01p jgarrett
* SW7425-288: Merge to main branch
* 
* SW7425-288/1   4/19/11 2:36a nilesh
* SW7425-288: Propagate audio metadata buffer address
* 
* 3   3/21/11 7:05p jgarrett
* SW7422-356: Adding MuxOutput support in APE/Nexus
* 
* 2   2/22/11 5:31p jgarrett
* SW7422-146: Fixing build errors
* 
* 1   1/4/11 2:39p jgarrett
* SW7422-146: Adding 7425 APE support
* 
* 9   1/4/11 11:19a jgarrett
* SW7425-41: Adding debug and removing cache conversion
* 
* 8   12/30/10 4:06p jgarrett
* SW7425-41: Fixing stub prototype
* 
* 7   12/30/10 12:16p jgarrett
* SW7425-41: Linked audio mux output and stream mux
* 
* 6   12/30/10 11:12a jgarrett
* SW7425-41: Fixing compilation error in stub apis
* 
* 5   12/30/10 10:37a jgarrett
* SW7425-41: Making mux output only functional on 7425
* 
* 4   12/29/10 9:48p jgarrett
* SW7425-41: Removing stubs on 7425
* 
* 3   12/29/10 6:33p jgarrett
* SW7425-41: Stub RAP mux files
* 
* 2   12/29/10 6:29p jgarrett
* SW7425-41: Adding audio encoder and mux outputs
* 
* 1   12/17/10 4:45p jgarrett
* SW7425-41: Adding initial encoder/mux APIs
*
***************************************************************************/

#include "nexus_audio_module.h"

BDBG_MODULE(nexus_audio_mux_output);


typedef struct NEXUS_AudioMuxOutput
{
    NEXUS_OBJECT(NEXUS_AudioMuxOutput);
} NEXUS_AudioMuxOutput;


/* Stubs */

/***************************************************************************
Summary:
    Get default settings for an Audio Mux output
***************************************************************************/
void NEXUS_AudioMuxOutput_GetDefaultCreateSettings(
    NEXUS_AudioMuxOutputCreateSettings *pSettings   /* [out] default settings */
    )
{
    BSTD_UNUSED(pSettings);
}

/***************************************************************************
Summary:
    Open an Audio Mux Ouptut Handle
***************************************************************************/
NEXUS_AudioMuxOutputHandle NEXUS_AudioMuxOutput_Create(   /* attr{destructor=NEXUS_AudioMuxOutput_Destroy}  */
    const NEXUS_AudioMuxOutputCreateSettings *pSettings /* Pass NULL for default settings */
    )
{
    BSTD_UNUSED(pSettings);
    (void)BERR_TRACE(BERR_NOT_SUPPORTED);
    return NULL;
}


/***************************************************************************
Summary:
    Close an Audio Mux Ouptut Handle
***************************************************************************/
static void NEXUS_AudioMuxOutput_P_Finalizer(
    NEXUS_AudioMuxOutputHandle handle
    )
{
    BSTD_UNUSED(handle);
}

/***************************************************************************
Summary:
Get the NEXUS_AudioOutput connector to make upstream connection
    
Description:
NEXUS_AudioMuxOutput can be connected to NEXUS_AudioEncoder (for transcode mode) as follows:

    NEXUS_AudioOutput_AddInput(NEXUS_AudioMuxOutput_GetConnector(muxOutput), NEXUS_AudioEncoder_GetConnector(audioEncoder));
        
Or it can be connected to NEXUS_AudioDecoder (for passthrough mode) as follows:

    NEXUS_AudioOutput_AddInput(NEXUS_AudioMuxOutput_GetConnector(muxOutput), NEXUS_AudioDecoder_GetConnector(audioDecoder));
        
***************************************************************************/
NEXUS_AudioOutput NEXUS_AudioMuxOutput_GetConnector( /* attr{shutdown=NEXUS_AudioOutput_Shutdown} */
    NEXUS_AudioMuxOutputHandle handle
    )
{
    BSTD_UNUSED(handle);
    return NULL;
}


/***************************************************************************
Summary:
Get default start-time settings for a mux output
***************************************************************************/
void NEXUS_AudioMuxOutput_GetDefaultStartSettings(
    NEXUS_AudioMuxOutputStartSettings *pSettings  /* [out] default settings */
    )
{
    BSTD_UNUSED(pSettings);
}

/***************************************************************************
Summary:
Start capturing data.  

Decription:
An input must be connected prior to starting.

See Also:
NEXUS_AudioMuxOutput_Stop
NEXUS_AudioMuxOutput_GetConnector
***************************************************************************/
NEXUS_Error NEXUS_AudioMuxOutput_Start(
    NEXUS_AudioMuxOutputHandle handle,
    const NEXUS_AudioMuxOutputStartSettings *pSettings
    )
{
    BSTD_UNUSED(handle);
    BSTD_UNUSED(pSettings);
    return BERR_TRACE(BERR_NOT_SUPPORTED);
}

/***************************************************************************
Summary:
Stop capturing data.
***************************************************************************/
void NEXUS_AudioMuxOutput_Stop(
    NEXUS_AudioMuxOutputHandle handle
    )
{
    BSTD_UNUSED(handle);
}

/**
Summary:
**/
NEXUS_Error NEXUS_AudioMuxOutput_GetStatus(
    NEXUS_AudioMuxOutputHandle handle,
    NEXUS_AudioMuxOutputStatus *pStatus /* [out] */
    )
{
    BSTD_UNUSED(handle);
    BSTD_UNUSED(pStatus);
    return BERR_TRACE(BERR_NOT_SUPPORTED);
}

/**
Summary:
The mux manager (or other consumer) will call this API to get encoded frames from the NEXUS_AudioMuxOutput
**/
NEXUS_Error NEXUS_AudioMuxOutput_GetBuffer(
    NEXUS_AudioMuxOutputHandle handle,
    const NEXUS_AudioMuxOutputFrame **pBuffer, /* [out] pointer to NEXUS_AudioMuxOutputFrame structs */
    size_t *pSize, /* [out] size of pBuffer in bytes (not number of NEXUS_AudioMuxOutputFrame structs) */
    const NEXUS_AudioMuxOutputFrame **pBuffer2, /* [out] pointer to NEXUS_AudioMuxOutputFrame structs after wrap around */
    size_t *pSize2 /* [out] size of pBuffer2 in bytes (not number of NEXUS_AudioMuxOutputFrame structs) */
    )
{
    BSTD_UNUSED(handle);
    BSTD_UNUSED(pBuffer);
    BSTD_UNUSED(pSize);
    BSTD_UNUSED(pBuffer2);
    BSTD_UNUSED(pSize2);
    return BERR_TRACE(BERR_NOT_SUPPORTED);
}

/**
Summary:
Report how much data returned by last NEXUS_AudioMuxOutput_GetBuffer call was consumed
**/
NEXUS_Error NEXUS_AudioMuxOutput_ReadComplete(
    NEXUS_AudioMuxOutputHandle handle,
    unsigned framesCompleted /* must be <= pSize+pSize2 returned by last NEXUS_AudioMuxOutput_GetBuffer call. */
    )
{
    BSTD_UNUSED(handle);
    BSTD_UNUSED(framesCompleted);
    return BERR_TRACE(BERR_NOT_SUPPORTED);
}

NEXUS_Error NEXUS_AudioMuxOutput_GetDelayStatus(
    NEXUS_AudioMuxOutputHandle handle,
    NEXUS_AudioCodec codec,
    NEXUS_AudioMuxOutputDelayStatus *pStatus    /* [out] */
    )
{
    BSTD_UNUSED(handle);
    BSTD_UNUSED(codec);
    BSTD_UNUSED(pStatus);
    return BERR_TRACE(BERR_NOT_SUPPORTED);
}

void NEXUS_AudioMuxOutput_P_WatchdogReset(void)
{
    return;
}

NEXUS_OBJECT_CLASS_MAKE(NEXUS_AudioMuxOutput, NEXUS_AudioMuxOutput_Destroy);

