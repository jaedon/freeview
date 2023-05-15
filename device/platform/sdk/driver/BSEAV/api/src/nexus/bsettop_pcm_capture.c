/***************************************************************************
 *     (c)2012 Broadcom Corporation
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
 * $brcm_Workfile: bsettop_pcm_capture.c $
 * $brcm_Revision: 2 $
 * $brcm_Date: 9/6/12 6:07p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/src/nexus/bsettop_pcm_capture.c $
 * 
 * 2   9/6/12 6:07p erickson
 * SW7435-349: add standard header
 * 
 **************************************************************************/
#include "bsettop_impl.h"
#include "bsettop.h"

BDBG_MODULE(pcm_capture);

/*
Summary:
Required to initialize the bpcm_capture_settings structure.
*/
void bpcm_capture_settings_init(
    bpcm_capture_settings *settings,	/* [out] */
    bpcm_capture_t pcmcapture		/* required for possible resource-dependent defaults */
)
{
    BSTD_UNUSED(settings);
    BSTD_UNUSED(pcmcapture);
}

/*
Summary:
Open a PCM capture channel.
*/
bpcm_capture_t bpcm_capture_open(
    bobject_t id				/* id corresponds to the pcm capture channel. */
)
{
    BSTD_UNUSED(id);
    (void)BSETTOP_ERROR(berr_not_supported);
    return NULL;
}

/*
Summary:
Close a PCM capture channel.
Description:
Capture must already be stopped.
*/
void bpcm_capture_close(
    bpcm_capture_t pcmcapture
)
{
    BSTD_UNUSED(pcmcapture);
}

/*
Summary:
Start capturing PCM audio.
*/
bresult bpcm_capture_start(
    bpcm_capture_t pcmcapture,
    bdisplay_t display,					/* which output to capture from. */
    const bpcm_capture_settings *settings
)
{
    BSTD_UNUSED(pcmcapture);
    BSTD_UNUSED(display);
    BSTD_UNUSED(settings);
    return BSETTOP_ERROR(berr_not_supported);
}

/**
Summary:
Get data available in the pcm capture buffer.

Description:
This is a non-destructive call. You can call it as many times as you want.
After reading data from the buffer, you should call bpcm_capture_read_complete
to report how much of the buffer was consumed.
**/
bresult bpcm_capture_get_buffer(
    bpcm_capture_t pcmcapture,
    void **data,
    size_t *length
)
{
    BSTD_UNUSED(pcmcapture);
    BSTD_UNUSED(data);
    BSTD_UNUSED(length);
    return BSETTOP_ERROR(berr_not_supported);
}

/**
Summary:
**/
bresult bpcm_capture_read_complete(
    bpcm_capture_t pcmcapture,
    size_t amount_read
)
{
    BSTD_UNUSED(pcmcapture);
    BSTD_UNUSED(amount_read);
    return BSETTOP_ERROR(berr_not_supported);
}

/*
Summary:
Stop PCM capture
*/
bresult bpcm_capture_stop(
    bpcm_capture_t pcmcapture
)
{
    BSTD_UNUSED(pcmcapture);
    return BSETTOP_ERROR(berr_not_supported);
}

