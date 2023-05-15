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
 * $brcm_Workfile: blit.c $
 * $brcm_Revision: 5 $
 * $brcm_Date: 9/6/12 6:04p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/examples/blit.c $
 * 
 * 5   9/6/12 6:04p erickson
 * SW7435-349: add standard header
 * 
 **************************************************************************/
#include "bsettop.h"
#include <stdlib.h>
#include <stdio.h>

/* You can run in 2 modes.
1) explicit blit calls with no bgraphics_sync. this is the MANUAL_BLIT mode.
    this is the recommended approach for any non-trivial surface compositing.

2) implicit blit calls using bgraphics_sync.
    this is the simple mode. it provides backward compatibility with 7030's real-time graphics compositor.
    this should only be used for simple graphics apps.

see comments in bsettop_graphics.h for a discussion of bgraphics_sync.

MANUAL_BLIT is a #define just for this example app:
#define MANUAL_BLIT
*/

int main(void) {
    bresult rc = bsettop_init(BSETTOP_VERSION);
    bdisplay_t display = bdisplay_open(B_ID(0));
    bgraphics_t graphics = bgraphics_open(B_ID(0),display);
    bsurface_create_settings create_settings;
    bsurface_settings fb_settings;
    bsurface_t framebuffer, surface;
    bsettop_rect rect = {0,0,200,200};
    int i;

    if (rc) goto error;

    /* Get the framebuffer (or create on-screen surface) */
    bsurface_create_settings_init(&create_settings, graphics);
    framebuffer = bgraphics_create_framebuffer(graphics, &create_settings, false);
    bsurface_get(framebuffer, &fb_settings);

    /* Create off-screen surface */
    create_settings.width = rect.width;
    create_settings.height = rect.height;
    create_settings.pixel_format = bgraphics_pixel_format_a8_r8_g8_b8;
    surface = bsurface_create(graphics, &create_settings);

    for (i=0;;i++) {
        uint32_t color;
#ifdef MANUAL_BLIT
        /* no scaling but random position */
        bsettop_rect destrect = {
            rand() % (fb_settings.position.width - rect.width),
            rand() % (fb_settings.position.height - rect.height),
            rect.width,
            rect.height};
        /* do not set bsurface_settings.visible, .zorder, .position.x or .position.y.
        Those settings are only used by the bgraphics_sync algorithm and
        have no meaning if you are doing manual surface compositing. */
#else
        /* use visible surfaces and bgraphics_sync */
        bsurface_settings settings;
        bsurface_get(surface, &settings);
        settings.visible = true;
        settings.position.x = rand() % (fb_settings.position.width - rect.width);
        settings.position.y = rand() % (fb_settings.position.height - rect.height);
        bsurface_set(surface, &settings);
#endif

        color = rand() % 0xFFFFFFFF;
        printf("fill 0x%06x\n", color);
        bsurface_fill(surface, &rect, color);

#ifdef MANUAL_BLIT
        printf("copy %d,%d,%d,%d\n", destrect.x, destrect.y, destrect.width, destrect.height);
        bsurface_copy(framebuffer, &destrect, surface, &rect);
#else
        /* this copies from the off-screen visible surface to the frame buffer. this is simplistic surface compositing. */
        bgraphics_sync(graphics);
#endif
    }

    return 0;
error:
    return 1;
}

