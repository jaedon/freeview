/******************************************************************************
 *    (c)2008-2009 Broadcom Corporation
 *
 * This program is the proprietary software of Broadcom Corporation and/or its licensors,
 * and may only be used, duplicated, modified or distributed pursuant to the terms and
 * conditions of a separate, written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 * no license (express or implied), right to use, or waiver of any kind with respect to the
 * Software, and Broadcom expressly reserves all rights in and to the Software and all
 * intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 * HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 * NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
 *
 * Except as expressly set forth in the Authorized License,
 *
 * 1.     This program, including its structure, sequence and organization, constitutes the valuable trade
 * secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of Broadcom integrated circuit products.
 *
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 * AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
 * WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
 * THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
 * OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
 * LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
 * OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
 * USE OR PERFORMANCE OF THE SOFTWARE.
 *
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
 * LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
 * EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
 * USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
 * ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
 * LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
 * ANY LIMITED REMEDY.
 *
 * $brcm_Workfile: untrusted_client.c $
 * $brcm_Revision: Hydra_Software_Devel/3 $
 * $brcm_Date: 11/18/11 1:23p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/multiprocess/untrusted_client.c $
 * 
 * Hydra_Software_Devel/3   11/18/11 1:23p erickson
 * SW7420-1148: shorten test, add test case
 * 
 * Hydra_Software_Devel/2   9/7/11 5:30p erickson
 * SW7420-1992: update naming
 * 
 * Hydra_Software_Devel/1   8/8/11 5:05p erickson
 * SW7420-1689: add test of untrusted client restrictions
 * 
 *****************************************************************************/
#include "nexus_platform.h"
#include "nexus_parser_band.h"
#include "nexus_ir_input.h"
#include "nexus_video_decoder.h"
#include "nexus_audio_decoder.h"
#include "nexus_simple_video_decoder_server.h"
#include "nexus_simple_audio_decoder_server.h"
#include "nexus_surface_compositor.h"
#include "nexus_playpump.h"
#include "nexus_message.h"
#include <assert.h>
#include <stdio.h>

int main(void)
{
    void *handle;
    NEXUS_Error rc;
    unsigned loops = 20;

    rc = NEXUS_Platform_Join();
    if (rc) return -1;

    /* call functions that should be denied to untrusted clients. 
    to test manually, run: simple_server -secure -timeout 0 */
    while (--loops) {
        /* various functions that aren't supported to untrusted clients */
        {
        NEXUS_ParserBandSettings settings;
        rc = NEXUS_ParserBand_SetSettings(0, &settings);
        BDBG_ASSERT(rc);
        rc = NEXUS_ParserBand_SetSettings(NEXUS_ParserBand_e0, &settings);
        BDBG_ASSERT(rc);
        }
        handle = NEXUS_IrInput_Open(0, NULL);
        BDBG_ASSERT(!handle);
        handle = NEXUS_VideoDecoder_Open(0, NULL);
        BDBG_ASSERT(!handle);
        handle = NEXUS_AudioDecoder_Open(0, NULL);
        BDBG_ASSERT(!handle);
        
        /* server-only functions */
        {
        NEXUS_SimpleAudioDecoderServerSettings settings;
        handle = NEXUS_SimpleAudioDecoder_Create(0, &settings);
        BDBG_ASSERT(!handle);
        }
        {
        NEXUS_SimpleVideoDecoderServerSettings settings;
        handle = NEXUS_SimpleVideoDecoder_Create(0, &settings);
        BDBG_ASSERT(!handle);
        }
        handle = NEXUS_SurfaceCompositor_Create(0);
        BDBG_ASSERT(!handle);
        
        {
        NEXUS_PlaypumpHandle playpump;
        NEXUS_PlaypumpSettings settings;
        playpump = NEXUS_Playpump_Open(0, NULL);
        NEXUS_Playpump_Close(playpump);
        /* closed handle */
        rc = NEXUS_Playpump_SetSettings(playpump, &settings);
        BDBG_ASSERT(rc);
        }
        
        /* function w/o handle */
        rc = NEXUS_Message_SetDssCapPattern(0, 0);
        BDBG_ASSERT(rc);
    
        /* bad handle */
        {
        NEXUS_MessageStatus status;        
        rc = NEXUS_Message_GetStatus((NEXUS_MessageHandle)0xdeadbeef, &status);
        BDBG_ASSERT(rc);
        }
    }

    printf("success\n");    
    NEXUS_Platform_Uninit();

    return 0;
}

