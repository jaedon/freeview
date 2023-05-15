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
 * $brcm_Workfile: hdsdsingle_pip.c $
 * $brcm_Revision: 2 $
 * $brcm_Date: 9/6/12 6:07p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/examples/hdsdsingle_pip.c $
 * 
 * 2   9/6/12 6:07p erickson
 * SW7435-349: add standard header
 * 
 **************************************************************************/
#include "bsettop.h"
#include <stdio.h>


/* This app demonstrates HDSD single interface with  PIP decode using streamer input */
int main(void) {
    bresult rc = bsettop_init(BSETTOP_VERSION);
    bdecode_t decode0 = bdecode_open(B_ID(0));
    bdecode_t decode1 = bdecode_open(B_ID(1));
    bdisplay_t display0 = bdisplay_open(B_ID(0));
    bdisplay_t display1 = bdisplay_open(B_ID(1));
    bdecode_window_t window0 = bdecode_window_open(B_ID(0), display0);
    bdecode_window_t window1 = bdecode_window_open(B_ID(1), display0);
    bdecode_window_t window2 = bdecode_window_clone(window0, B_ID(2), display1);
    bdecode_window_t window3 = bdecode_window_clone(window1, B_ID(3), display1);
    /* two graphics are totally independent. user makes sure content is the same */
    bgraphics_t graphics0 = bgraphics_open(B_ID(0), display0);
    bgraphics_t graphics1 = bgraphics_open(B_ID(1), display1);
    bstream_t stream0,stream1;
    bstream_mpeg mpeg;
    bband_t band;
    bsurface_t fb0, fb1;
    bdecode_window_settings dw_settings;
    bdisplay_settings display_settings;
    bsurface_create_settings surface_create_settings;
    if (rc) goto error;

    /* Switch display to HD and SD */
    bdisplay_get(display0, &display_settings);
    display_settings.format = bvideo_format_1080i;
    display_settings.svideo = NULL;
    display_settings.composite = NULL;
    display_settings.rf = NULL;
    display_settings.component = boutput_component_open(B_ID(0));
    bdisplay_set(display0, &display_settings);
    bdisplay_get(display1, &display_settings);
    display_settings.format = bvideo_format_ntsc;
    display_settings.svideo = boutput_svideo_open(B_ID(0));
    display_settings.composite = boutput_composite_open(B_ID(0));
    display_settings.rf = boutput_rf_open(B_ID(0));
    bdisplay_set(display1, &display_settings);


    /* reposition pip's */
    /* bdecode_window_get(window0, &dw_settings);
    dw_settings.position.x = 1920/2;
    dw_settings.position.y = 0;
    dw_settings.position.width = 1920/2;
    dw_settings.position.height = 1080/2;
    bdecode_window_set(window0, &dw_settings);
    bdecode_window_get(window1, &dw_settings);
    dw_settings.position.x = 720/2;
    dw_settings.position.y = 0;
    dw_settings.position.width = 720/2;
    dw_settings.position.height = 480/2;
    bdecode_window_set(window1, &dw_settings); */


    bstream_mpeg_init(&mpeg);
    mpeg.video[0].pid = 0x31;
    mpeg.audio[0].pid = 0x34;
    mpeg.audio[0].format = baudio_format_ac3;
    mpeg.pcr_pid = 0x31;

    band = bstreamer_attach(B_ID(0), bstream_mpeg_type_ts);
    if (!band) goto error;

    stream0 = bstream_open(band, &mpeg);
    if (!stream0) goto error;

    if (bdecode_start(decode0, stream0, window0))
        goto error;

    printf("press enter to start pip decode");
    getchar();

    mpeg.video[0].pid = 0x11;
    mpeg.audio[0].pid = 0x14;
    mpeg.audio[0].format = baudio_format_ac3;
    mpeg.pcr_pid = 0x11;

    stream1= bstream_open(band,&mpeg);
    if(!stream1) goto error;

    if (bdecode_start(decode1, stream1, window1))
        goto error;

    getchar();
    bdecode_stop(decode1);
    bdecode_stop(decode0);
    bdecode_window_close(window2);
    bdecode_window_close(window3);
    bdecode_window_close(window1);
    bdecode_window_close(window0);
    bgraphics_close(graphics0);
    bgraphics_close(graphics1);
    bdisplay_close(display0);
    bdisplay_close(display1);
    bdecode_close(decode1);
    bdecode_close(decode0);
    bsettop_uninit();

    return 0;

    error:
    return 1;

}
