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
 * $brcm_Workfile: open.c $
 * $brcm_Revision: Hydra_Software_Devel/9 $
 * $brcm_Date: 9/6/12 6:07p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/video_decoder/open.c $
 * 
 * Hydra_Software_Devel/9   9/6/12 6:07p erickson
 * SW7435-349: add standard header
 * 
 **************************************************************************/
/* Nexus test app:
flavors of Open/Close
*/

#include <stdio.h>
#include <stdlib.h>
#include "bstd.h"
#include "bkni.h"
#include "nexus_platform.h"
#include "nexus_video_decoder.h"

int main(void)
{
    NEXUS_VideoDecoderHandle vdecode, vdecode2;
    NEXUS_VideoDecoderOpenSettings videoDecoderOpenSettings;
    unsigned i;
    unsigned loops = 1;
    NEXUS_Error rc;

    /* Bring up all modules for a platform in a default configuraiton for this platform */
    rc = NEXUS_Platform_Init(NULL);
    BDBG_ASSERT(!rc);

    for (i=0;i<loops;i++) {
        NEXUS_VideoDecoderStatus status;
        void *buffer;
        unsigned size;
        bool supported;

        /* bring up decoder with default settings */
        vdecode = NEXUS_VideoDecoder_Open(0, NULL);
        BDBG_ASSERT(vdecode);

        /* can't reopen it */
        printf("expected failure:\n");
        vdecode2 = NEXUS_VideoDecoder_Open(0, NULL);
        BDBG_ASSERT(!vdecode2);
        printf("expected failure done.\n");

        /* can close it */
        BKNI_Sleep(rand() % 200);
        NEXUS_VideoDecoder_Close(vdecode);

        /* bring up against with explicit default settings */
        NEXUS_VideoDecoder_GetDefaultOpenSettings(&videoDecoderOpenSettings);
        vdecode = NEXUS_VideoDecoder_Open(0, &videoDecoderOpenSettings);
        BDBG_ASSERT(vdecode);
        NEXUS_VideoDecoder_Close(vdecode);

#if NEXUS_NUM_VIDEO_DECODERS == 1
        printf("expected failure:\n");
        vdecode2 = NEXUS_VideoDecoder_Open(1, NULL);
        BDBG_ASSERT(!vdecode2);
        printf("expected failure done.\n");
#endif

        /* check calls are valid before start */
        vdecode = NEXUS_VideoDecoder_Open(0, NULL);
        BDBG_ASSERT(vdecode);

        NEXUS_VideoDecoder_Flush(vdecode);

        rc = NEXUS_VideoDecoder_GetStatus(vdecode, &status);
        BDBG_ASSERT(!rc);
        /* settings should be predictable because we never started decode */
        BDBG_ASSERT(status.source.width == 0);
        BDBG_ASSERT(status.source.height == 0);
        BDBG_ASSERT(status.display.width == 0);
        BDBG_ASSERT(status.display.height == 0);
        BDBG_ASSERT(status.started == false);
        BDBG_ASSERT(status.aspectRatio == NEXUS_AspectRatio_eUnknown);
        BDBG_ASSERT(status.fifoDepth == 0);
        BDBG_ASSERT(status.numDecoded == 0);
        BDBG_ASSERT(status.numDisplayed == 0);
        BDBG_ASSERT(status.numDecodeErrors == 0);
        BDBG_ASSERT(status.numDisplayErrors == 0);
        BDBG_ASSERT(status.numDecodeDrops == 0);
        BDBG_ASSERT(status.numDisplayDrops == 0);
        BDBG_ASSERT(status.numDisplayUnderflows == 0);
        BDBG_ASSERT(status.ptsErrorCount == 0);

        rc = NEXUS_VideoDecoder_GetUserDataBuffer(vdecode, &buffer, &size);
        BDBG_ASSERT(!rc);
        BDBG_ASSERT(size == 0);

        NEXUS_VideoDecoder_IsCodecSupported(vdecode, NEXUS_VideoCodec_eMpeg2, &supported);
        BDBG_ASSERT(supported == true);
        NEXUS_VideoDecoder_IsCodecSupported(vdecode, NEXUS_VideoCodec_eH263, &supported);
        BDBG_ASSERT(supported == true);
        NEXUS_VideoDecoder_IsCodecSupported(vdecode, (NEXUS_VideoCodec)(NEXUS_VideoCodec_eMax+10), &supported);
        BDBG_ASSERT(supported == false);

        NEXUS_VideoDecoder_Close(vdecode);
    }

    NEXUS_Platform_Uninit();
    printf("success\n");
    return 0;
}
