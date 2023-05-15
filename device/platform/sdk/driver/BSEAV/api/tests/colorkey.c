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
 * $brcm_Workfile: colorkey.c $
 * $brcm_Revision: 3 $
 * $brcm_Date: 9/6/12 6:04p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/tests/colorkey.c $
 * 
 * 3   9/6/12 6:04p erickson
 * SW7435-349: add standard header
 * 
 **************************************************************************/
#include "bsettop.h"
#include <stdio.h>

#ifndef USE_STREAMER
#define USE_STREAMER 1
#endif

#define PIXEL888(r,g,b) ((uint32_t)(((r&0xFF)<<(16)) | ((g&0xFF)<<8) | (b&0xFF)))
#define PIXEL(r,g,b) PIXEL888(r,g,b)
//#define PIXEL(r,g,b) ((uint32_t)(((r&0xFF)<<(16)) | 0xFF000000 | ((g&0xFF)<<8) | (b&0xFF)))

int main(void) {
    bresult rc = bsettop_init(BSETTOP_VERSION);
    btuner_t tuner = btuner_open(B_ID(0));
    bdecode_t decode = bdecode_open(B_ID(0));
    bdisplay_t display = bdisplay_open(B_ID(0));
    bdecode_window_t window = bdecode_window_open(B_ID(0), display);
    btuner_qam_params qam;
    bstream_t stream;
    bstream_mpeg mpeg;
    bband_t band;
    bgraphics_t graphics = bgraphics_open(B_ID(0),display);
    bsurface_create_settings cfg;
    bsurface_settings scfg;
    bsurface_t framebuffer;
    bgraphics_settings gcfg;
    bsettop_rect rect;


    if (rc) goto error;

    btuner_qam_params_init(&qam, tuner);
    qam.symbol_rate = 5056900;
    qam.mode = 64;

    bstream_mpeg_init(&mpeg);
    mpeg.video[0].pid = 0x31;
    mpeg.audio[0].pid = 0x34;
    mpeg.audio[0].format = baudio_format_ac3;
    mpeg.pcr_pid = 0x31;

#if USE_STREAMER
    printf("Decoding from a streamer...\n");
    band = bstreamer_attach( B_ID(0), bstream_mpeg_type_ts );
#else
    printf("Decoding from a QAM demod...\n");
    band = btuner_tune_qam(tuner, 765000000, &qam);
#endif

    if (!band) goto error;

    stream = bstream_open(band, &mpeg);
    if (!stream) goto error;

    if (bdecode_start(decode, stream, window))
        goto error;

    bsurface_create_settings_init(&cfg, graphics);

    framebuffer = bsurface_get_display(graphics, &cfg);

    rect.x=rect.y=0;
    rect.width=cfg.width/2;
    rect.height=cfg.height/2;
    bsurface_fill(framebuffer, &rect, PIXEL(0xFF, 0, 0));
    rect.x=rect.width;
    bsurface_fill(framebuffer, &rect, PIXEL(0, 0, 0xFF));
    rect.x=0;
    rect.y=cfg.height/2;
    bsurface_fill(framebuffer, &rect, PIXEL(0xFF, 0xFF, 0xFF));
    rect.x=rect.width;
    rect.height=cfg.height/4;
    bsurface_fill(framebuffer, &rect, PIXEL(0, 0xFF, 0));

    /* katrep: do not leave out the lower right uninitialized corner fill dont leave it empty */
    rect.y=(cfg.height/2 + cfg.height/4);
    rect.height=cfg.height/4;
    bsurface_fill(framebuffer, &rect, PIXEL(0, 0, 0));
      

    bgraphics_sync(graphics);

    printf("no color keying\n");
    bsurface_get(framebuffer, &scfg);
    printf("alpha %#x\n", (unsigned)scfg.alpha);
    getchar(); /* press enter to stop decode */

    scfg.alpha = 0x80;
    printf("alpha %#x\n", (unsigned)scfg.alpha);
    bsurface_set(framebuffer, &scfg);
    bgraphics_sync(graphics);
    getchar();
    
    scfg.alpha = 0xFF;
    printf("alpha %#x\n", (unsigned)scfg.alpha);
    bsurface_set(framebuffer, &scfg);
    bgraphics_sync(graphics);
    getchar();

    scfg.alpha = 0x60;
    printf("alpha %#x\n", (unsigned)scfg.alpha);
    bsurface_set(framebuffer, &scfg);
    bgraphics_sync(graphics);
    getchar();

    bgraphics_get(graphics, &gcfg);
    gcfg.chromakey_enabled = true;
    gcfg.upper_chromakey = PIXEL888(0, 0, 0xFF);
    gcfg.lower_chromakey = PIXEL888(0, 0, 0xFE);
    bgraphics_set(graphics, &gcfg);
    printf("color key applied press  enter to proceed\n");
    getchar();
    framebuffer = bsurface_get_display(graphics, &cfg);

    rect.x=rect.y=0;
    rect.width=cfg.width/2;
    rect.height=cfg.height/2;
    bsurface_fill(framebuffer, &rect, PIXEL(0xFF, 0, 0));
    rect.x=rect.width;
    bsurface_fill(framebuffer, &rect, PIXEL(0, 0, 0xFF));
    rect.x=0;
    rect.y=cfg.height/2;
    bsurface_fill(framebuffer, &rect, PIXEL(0xFF, 0xFF, 0xFF));
    rect.x=rect.width;
    rect.height=cfg.height/4;
    bsurface_fill(framebuffer, &rect, PIXEL(0, 0xFF, 0));

    bgraphics_sync(graphics);


    bsurface_get(framebuffer, &scfg);
    printf("alpha %#x\n", (unsigned)scfg.alpha);
    bgraphics_sync(graphics);
    getchar(); 

    scfg.alpha = 0x10;
    printf("alpha %#x\n", (unsigned)scfg.alpha);
    bsurface_set(framebuffer, &scfg);
    bgraphics_sync(graphics);
    getchar();

    scfg.alpha = 0x80;
    printf("alpha %#x\n", (unsigned)scfg.alpha);
    bsurface_set(framebuffer, &scfg);
    bgraphics_sync(graphics);
    getchar();

    scfg.alpha = 0xF0;
    printf("alpha %#x\n", (unsigned)scfg.alpha);
    bsurface_set(framebuffer, &scfg);
    bgraphics_sync(graphics);
    getchar(); 


    bsettop_uninit();
    return 0;
error:
    return 1;
}
