/******************************************************************************
 *    (c)2008 Broadcom Corporation
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
 * $brcm_Workfile: vdcCrc.h $
 * $brcm_Revision: Hydra_Software_Devel/5 $
 * $brcm_Date: 9/24/12 2:42p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/common/vdcCrc.h $
 * 
 * Hydra_Software_Devel/5   9/24/12 2:42p ahulse
 * SW7425-3321: update crc's due to changes in SW7425-2172
 * 
 * Hydra_Software_Devel/4   8/2/12 5:31p ahulse
 * SW7425-2678: update golden crc values as a result of change in SW7425-
 * 3321
 * 
 * Hydra_Software_Devel/3   7/9/12 10:03a ahulse
 * SW7425-2678: add missing header file
 * 
 * Hydra_Software_Devel/2   7/5/12 6:21p ahulse
 * SW7425-2678: extend to do crcCheck using image input
 * 
 * Hydra_Software_Devel/1   6/26/12 2:17p ahulse
 * SW7425-2678: collect crc check functions into header
 *
 *
 *
******************************************************************************/

/*
 * vdcCrc.h
 *
 * The purpose behind this set of utility functions is to provide simple functions to check
 * the crc value of current video ouput at a given resolution and format, with a "golden"
 * value previously established.
 *
 * (Note: this header can only be included once, and it includes code -- this allows it to
 * be dropped into existing unittests without modifying makefiles to link libraries or add
 * other source files.)
 */

#ifndef __CRC_CHECK_H__
#define __CRC_CHECK_H__

#include "nexus_display.h"

#include "nexus_core_utils.h"
#include <stdio.h>
#include <string.h>

#include "nexus_graphics2d.h"
#include "nexus_video_image_input.h"
/* If BDBG_MSG output is required, uncomment below, but also make sure the inclusion of this
   file (vdcCrc.h) is after the declaration of the BDBG_MODULE()
*/
#define BDBG_MSG_TRACE(x)  /* BDBG_MSG(x)  */

typedef enum colorPatterns {
    BLACK_FIELD,
    COLOR_BAR,
    RED_FIELD,
    GREEN_FIELD,
    BLUE_FIELD,
    PATTERN_eMAX
} colorPatterns;

/* SMPTE color bars */
#define NUM_COLORS 7
static uint32_t g_colors[NUM_COLORS] = {
    0xFFFFFFFF, /* white */
    0xFFFFFF00, /* yellow */
    0xFF00FFFF, /* cyan */
    0xFF00FF00, /* green */
    0xFFFF00FF, /* magenta */
    0xFFFF0000, /* red */
    0xFF0000FF  /* blue */
};

/* Structure to simplify passing handles around */
typedef struct crc_context {
    NEXUS_Graphics2DHandle      gfx;
    NEXUS_DisplayHandle         display;
    BKNI_EventHandle            checkpointEvent;
    NEXUS_SurfaceHandle         framebuffer;
    NEXUS_VideoImageInputHandle imageInput;
    NEXUS_SurfaceHandle         imgSurface;
} crc_context;

typedef struct {
        colorPatterns idx;
        NEXUS_VideoFormat vidFormat;
        NEXUS_DisplayCrcData data;
} crcTestVectors;

#define NR_FORMATS 5 /* change this if change number of supported formats, eg now support 480p,720p,1080p,576p,720p50 so is 5 now */
crcTestVectors goldenGfxVectors[] = {
    /* BLACK_FIELD */
    { BLACK_FIELD, NEXUS_VideoFormat_e480p,   {{0x8984, 0x0fda, 0x0fda }} },
    { BLACK_FIELD, NEXUS_VideoFormat_e720p,   {{0x4efe, 0x8ba0, 0x8ba0 }} },
    { BLACK_FIELD, NEXUS_VideoFormat_e1080p,  {{0x15e, 0x381d, 0x381d }} },
    { BLACK_FIELD, NEXUS_VideoFormat_e576p,   {{0x6ada, 0xc3d2, 0xc3d2 }} },
    { BLACK_FIELD, NEXUS_VideoFormat_e720p50hz, {{0x4efe, 0x8ba0, 0x8ba0 }} },
    /* COLOR_BAR */
    { COLOR_BAR, NEXUS_VideoFormat_e480p,     {{0xbd4f, 0x8e64, 0x3a7d }} },
    { COLOR_BAR, NEXUS_VideoFormat_e720p,     {{0xdf01, 0x883e, 0xa290 }} },
    { COLOR_BAR, NEXUS_VideoFormat_e1080p,    {{0x888,  0x5c43, 0xac4b }} },
    { COLOR_BAR, NEXUS_VideoFormat_e576p,     {{0x7f2d, 0xdf17, 0x3c5c }} },
    { COLOR_BAR, NEXUS_VideoFormat_e720p50hz, {{0x5df4, 0xd024, 0x5989 }} },
    /* RED_FIELD */
    { RED_FIELD, NEXUS_VideoFormat_e480p,     {{0x80d2, 0x05d6, 0x0806 }} },
    { RED_FIELD, NEXUS_VideoFormat_e720p,     {{0x13b8, 0xf9eb, 0x7389 }} },
    { RED_FIELD, NEXUS_VideoFormat_e1080p,    {{0x2794, 0x214a, 0xb5ee }} },
    { RED_FIELD, NEXUS_VideoFormat_e576p,     {{0xa908, 0xf815, 0x7f4d }} },
    { RED_FIELD, NEXUS_VideoFormat_e720p50hz, {{0x13b8, 0xf9eb, 0x7389 }} },
    /* GREEN_FIELD */
    { GREEN_FIELD, NEXUS_VideoFormat_e480p,   {{0xed42, 0x6cc9, 0x8e52 }} },
    { GREEN_FIELD, NEXUS_VideoFormat_e720p,   {{0x1844, 0xa8e0, 0x8989 }} },
    { GREEN_FIELD, NEXUS_VideoFormat_e1080p,  {{0x55a0, 0xc225, 0xae7a }} },
    { GREEN_FIELD, NEXUS_VideoFormat_e576p,   {{0xa719, 0xf815, 0x7f4d }} },
    { GREEN_FIELD, NEXUS_VideoFormat_e720p50hz, {{0x1844, 0xa8e0, 0x8989 }} },
    /* BLUE_FIELD */
    { BLUE_FIELD, NEXUS_VideoFormat_e480p,    {{0x43e4, 0x0806, 0x4ab1 }} },
    { BLUE_FIELD, NEXUS_VideoFormat_e720p,    {{0x6f97, 0x7389, 0x94d5 }} },
    { BLUE_FIELD, NEXUS_VideoFormat_e1080p,   {{0x6d01, 0xb5ee, 0xa985 }} },
    { BLUE_FIELD, NEXUS_VideoFormat_e576p,    {{0x100f, 0x67d1, 0x7a78 }} },
    { BLUE_FIELD, NEXUS_VideoFormat_e720p50hz, {{0x6f97, 0x7389, 0x94d5 }} },
/* TODO 1080p50 */
};

/* Golden crc values when using image input */
crcTestVectors goldenVideoVectors[] = {
    /* BLACK_FIELD */
    { BLACK_FIELD, NEXUS_VideoFormat_e480p,   {{0x8984, 0x0fda, 0x0fda }} },
    { BLACK_FIELD, NEXUS_VideoFormat_e720p,   {{0x4efe, 0x8ba0, 0x8ba0 }} },
    { BLACK_FIELD, NEXUS_VideoFormat_e1080p,  {{0x15e, 0x381d, 0x381d }} },
    { BLACK_FIELD, NEXUS_VideoFormat_e576p,   {{0x6ada, 0xc3d2, 0xc3d2 }} },
    { BLACK_FIELD, NEXUS_VideoFormat_e720p50hz, {{0x4efe, 0x8ba0, 0x8ba0 }} },
    /* COLOR_BAR */
    { COLOR_BAR, NEXUS_VideoFormat_e480p,     {{0x2cb0, 0x3c11, 0x80ce }} },
    { COLOR_BAR, NEXUS_VideoFormat_e720p,     {{0x5df4, 0xd024, 0x5989 }} },
    { COLOR_BAR, NEXUS_VideoFormat_e1080p,    {{0x45e1, 0x578e, 0x04dd }} },
    { COLOR_BAR, NEXUS_VideoFormat_e576p,     {{0x1a64, 0x2574, 0x9ef7 }} },
    { COLOR_BAR, NEXUS_VideoFormat_e720p50hz, {{0x5df4, 0xd024, 0x5989 }} },
    /* RED_FIELD */
    { RED_FIELD, NEXUS_VideoFormat_e480p,     {{0xc860, 0x05d6, 0x2e77 }} },
    { RED_FIELD, NEXUS_VideoFormat_e720p,     {{0xe386, 0x50c9, 0x0fa6 }} },
    { RED_FIELD, NEXUS_VideoFormat_e1080p,    {{0xe28d, 0x4fd6, 0xff7b }} },
    { RED_FIELD, NEXUS_VideoFormat_e576p,     {{0x9b10, 0xf452, 0xb7bb }} },
    { RED_FIELD, NEXUS_VideoFormat_e720p50hz, {{0xe386, 0x50c9, 0x0fa6 }} },
    /* GREEN_FIELD */
    { GREEN_FIELD, NEXUS_VideoFormat_e480p,   {{0x865e, 0x6cc9, 0x8e52 }} },
    { GREEN_FIELD, NEXUS_VideoFormat_e720p,   {{0xc55e, 0x4d95, 0xa135 }} },
    { GREEN_FIELD, NEXUS_VideoFormat_e1080p,  {{0x3943, 0x4829, 0x2676 }} },
    { GREEN_FIELD, NEXUS_VideoFormat_e576p,   {{0xa980, 0xf815, 0x7f4d }} },
    { GREEN_FIELD, NEXUS_VideoFormat_e720p50hz, {{0xc55e, 0x4d95, 0xa135 }} },
    /* BLUE_FIELD */
    { BLUE_FIELD, NEXUS_VideoFormat_e480p,    {{0x28f8, 0x2e77, 0x4ab1 }} },
    { BLUE_FIELD, NEXUS_VideoFormat_e720p,    {{0x6642, 0x0fa6, 0xf03e }} },
    { BLUE_FIELD, NEXUS_VideoFormat_e1080p,   {{0x8952, 0xff7b, 0xc519 }} },
    { BLUE_FIELD, NEXUS_VideoFormat_e576p,    {{0x1e1e, 0xb7bb, 0x7a78 }} },
    { BLUE_FIELD, NEXUS_VideoFormat_e720p50hz, {{0x6642, 0x0fa6, 0xf03e }} },
/* TODO 1080p50 */
};
    

int displayVdcCrc( crc_context *context, NEXUS_DisplayCrcData ref_data, const char *label ) {

    int i,j,rc;
    unsigned num;
    NEXUS_DisplaySettings        displaySettings;
    NEXUS_DisplayCrcData         data[16];

/* Poll vdc 6 times to get crc - if increase this, CRC_QUEUE_SIZE may need adjusting */
#define POLL_TIMES 6
#define CRC_QUEUE_SIZE 600

    NEXUS_Display_GetSettings(context->display, &displaySettings);
    displaySettings.crcQueueSize = CRC_QUEUE_SIZE; /* This enables the CRC capture */
    NEXUS_Display_SetSettings( context->display, &displaySettings );

    for( i=0 ; i < POLL_TIMES ; i++ ) {
        num = 0;
        while ( !num )
        {
            rc = NEXUS_Display_GetCrcData(context->display, data, 16, &num);
            BDBG_ASSERT(!rc);
            if (!num) {
                BKNI_Sleep(10);
                continue;
            }
#if 0 /* if want to see ALL crc vaues retrieved from xx_GetCrcData, set to #if 1 */
            for (j=0;j<(int)num;j++) {
                BDBG_MSG_TRACE(("CMP CRC num=%d j=%d %#x %#x %#x\n", num, j, data[j].cmp.luma, data[j].cmp.cb, data[j].cmp.cr));
            }
#else
            j = num;
#endif
        }
        BKNI_Sleep(16); /* approx 1 vsync for ntsc etc */
    }

    NEXUS_Display_GetSettings(context->display, &displaySettings);
    displaySettings.crcQueueSize = 0;
    NEXUS_Display_SetSettings( context->display, &displaySettings );

    if ( j ) j--;

    /* Non zero's in ref data means no check */
    if ( ref_data.cmp.luma != 0 && ref_data.cmp.cb != 0  && ref_data.cmp.cr != 0 ) {
        if ( data[j].cmp.luma == ref_data.cmp.luma && data[j].cmp.cb == ref_data.cmp.cb && data[j].cmp.cr == ref_data.cmp.cr ) { 
            BDBG_MSG_TRACE(( "\n\t %s CRC matched reference CRC of luma=%#x cb=%#x cr=%#x j=%d\n\n" , label!=NULL ? label:"" , 
                data[j].cmp.luma, data[j].cmp.cb, data[j].cmp.cr , j ));
            rc = 0;
        }
        else {
         BDBG_MSG_TRACE(( "\n\t %s CRC FAILED (failing CRC luma=%#x cb=%#x cr=%#x) j=%d\n" , label != NULL ? label:"" , 
                data[j].cmp.luma, data[j].cmp.cb, data[j].cmp.cr , j ));
         BDBG_MSG_TRACE(("\t\t                            ( ref    CRC luma=%#x cb=%#x cr=%#x) j=%d\n\n" ,
                 ref_data.cmp.luma, ref_data.cmp.cb, ref_data.cmp.cr , j ));
            rc = 1;
        }
    }
    else {
        fprintf(stderr, "\n\tUnknown Color/Pattern %s, final CRC j=%d y=%#x u=%#x v=%#x\n" , label!=NULL ? label:"" ,
                j, data[j].cmp.luma, data[j].cmp.cb, data[j].cmp.cr );
    }
    return rc;
}

void setImageInput( crc_context *context , NEXUS_SurfaceHandle  img , NEXUS_Rect rect) {

    NEXUS_Graphics2DBlitSettings blitSettings;
    NEXUS_Error rc;

        NEXUS_Graphics2D_GetDefaultBlitSettings(&blitSettings);
        blitSettings.source.surface     = img;
        blitSettings.source.rect.x      = 0;
        blitSettings.source.rect.y      = 0;
        blitSettings.source.rect.width  = rect.width;
        blitSettings.source.rect.height = rect.height;
        blitSettings.output.surface     = context->imgSurface;
        NEXUS_Graphics2D_Blit( context->gfx, &blitSettings);  /* don't wait for blit to complete */
        rc = NEXUS_Graphics2D_Checkpoint(context->gfx, NULL); /* require to execute queue */
        if (rc == NEXUS_GRAPHICS2D_QUEUED) {
            BKNI_WaitForEvent(context->checkpointEvent, BKNI_INFINITE);
        }
        NEXUS_VideoImageInput_SetSurface(context->imageInput, context->imgSurface );

        BKNI_Sleep(3*16); /* allow time for image to propogate to vdc */
}

int crcCheck( crc_context *context, uint32_t pattern ) {

    NEXUS_Graphics2DFillSettings fillSettings;
    NEXUS_DisplaySettings   displaySettings;
    NEXUS_VideoFormatInfo   info;
    NEXUS_DisplayCrcData ref_data;
    static NEXUS_DisplayCrcData *pRef_Data;
    NEXUS_Error rc;
    int i,formatIdx;
    bool crcSupported=false;
    uint32_t fillColor = pattern;
    NEXUS_SurfaceHandle  img,tmpImg;
    NEXUS_SurfaceCreateSettings  surfaceCfg;

    NEXUS_Display_GetSettings( context->display, &displaySettings );

    switch ( displaySettings.format ) {
        case NEXUS_VideoFormat_e480p :    formatIdx = 0; crcSupported = true; break;
        case NEXUS_VideoFormat_e720p :    formatIdx = 1; crcSupported = true; break;
        case NEXUS_VideoFormat_e1080p:    formatIdx = 2; crcSupported = true; break;
        case NEXUS_VideoFormat_e576p:     formatIdx = 3; crcSupported = true; break;
        case NEXUS_VideoFormat_e720p50hz: formatIdx = 4; crcSupported = true; break;
        default  : formatIdx = 0; break;
    }

    NEXUS_VideoFormat_GetInfo( displaySettings.format, &info);

    /* Make sure destination is clear by blitting black to it */
    NEXUS_Graphics2D_GetDefaultFillSettings(&fillSettings);

    if ( context->imageInput ) {
        NEXUS_Surface_GetDefaultCreateSettings(&surfaceCfg);
        surfaceCfg.width  = info.width;
        surfaceCfg.height = info.height;
        surfaceCfg.pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;
        tmpImg = NEXUS_Surface_Create(&surfaceCfg);
        BDBG_ASSERT(tmpImg);
        img = tmpImg;
    }
    else {
        img = context->framebuffer;
    }

    /* Make sure graphics layer is clear by blitting black to it */
    NEXUS_Graphics2D_GetDefaultFillSettings(&fillSettings);
    fillSettings.surface = img;
    fillSettings.rect.x = 0;
    fillSettings.rect.y = 0;
    fillSettings.rect.height = info.height;
    fillSettings.rect.width  = info.width;
    fillSettings.color = 0xff000000;
    NEXUS_Graphics2D_Fill(context->gfx, &fillSettings);

    /* needed to execute queued blits */
    rc = NEXUS_Graphics2D_Checkpoint(context->gfx, NULL);
    if (rc == NEXUS_GRAPHICS2D_QUEUED) {
        rc = BKNI_WaitForEvent(context->checkpointEvent, BKNI_INFINITE);
    }
    BDBG_ASSERT(!rc);

    if ( context->imageInput ) {
        /* Make sure image input layer is clear by blitting black to it */
        setImageInput( context , img , fillSettings.rect );
    }

    BKNI_Memset( &ref_data, 0, sizeof(NEXUS_DisplayCrcData) );
    pRef_Data = &ref_data;

    if ( crcSupported && pattern < PATTERN_eMAX ) {
        switch ( displaySettings.format ) {
            case NEXUS_VideoFormat_e480p :    formatIdx = 0; break;
            case NEXUS_VideoFormat_e720p :    formatIdx = 1; break;
            case NEXUS_VideoFormat_e1080p:    formatIdx = 2; break;
            case NEXUS_VideoFormat_e576p:     formatIdx = 3; break;
            case NEXUS_VideoFormat_e720p50hz: formatIdx = 4; break;
            default  : formatIdx = 0; break;
        }

        switch (pattern ) {
            case BLACK_FIELD: 
                fillColor = 0x0;
                break;
            case COLOR_BAR: 
                break;
            case RED_FIELD: 
                fillColor = 0xFFFF0000; 
                break;
            case GREEN_FIELD: 
                fillColor = 0xFF00FF00; 
                break;
            case BLUE_FIELD: 
                fillColor = 0xFF0000FF; 
                break;
        }
        if ( context->imageInput ) {
            pRef_Data = &(goldenVideoVectors[(pattern*NR_FORMATS)+formatIdx].data);
        }
        else {
            pRef_Data = &(goldenGfxVectors[(pattern*NR_FORMATS)+formatIdx].data);
        }
        BDBG_MSG_TRACE(( "pattern=%d formatIdx=%d pRefData=%p" , pattern, formatIdx, pRef_Data ));
        BDBG_MSG_TRACE(( "   luma=%#x cb=%#x cr=%#x " , 
                pRef_Data->cmp.luma, pRef_Data->cmp.cb, pRef_Data->cmp.cr ));
    }

    NEXUS_Graphics2D_GetDefaultFillSettings(&fillSettings);
    if ( context->imageInput ) {
        fillSettings.surface = tmpImg;
        NEXUS_VideoImageInput_SetSurface(context->imageInput, NULL );  /* release image */
    }
    else {
        fillSettings.surface = img;
    }
    
    if ( pattern == 1 ) {
        /* draw for the HD display */
        fillSettings.rect.y = 0;
        fillSettings.rect.height = info.height;
        fillSettings.rect.width = info.width/NUM_COLORS;
        BDBG_MSG_TRACE(( "2D_Fill with color=0x%8x" , pattern ));
        for (i=0;i<NUM_COLORS;i++) {
            fillSettings.rect.x = fillSettings.rect.width * i;
            fillSettings.color = g_colors[i];
            BDBG_MSG_TRACE(( "Fill i=%d x=%d w=%d with color=0x%8x" , i, fillSettings.rect.x, fillSettings.rect.width, g_colors[i] ));
            NEXUS_Graphics2D_Fill(context->gfx, &fillSettings);
        }
        fillSettings.rect.width = info.width;
    }
    else {
        /* Flat field display */
        fillSettings.rect.width = info.width;
        fillSettings.rect.y = 0;
        fillSettings.rect.height = info.height;
        fillSettings.color = fillColor;
        BDBG_MSG_TRACE(( "2D_Fill with color=0x%8x" , pattern ));
        NEXUS_Graphics2D_Fill(context->gfx, &fillSettings);

    }

    /* needed to execute queued blits */
    rc = NEXUS_Graphics2D_Checkpoint(context->gfx, NULL);
    if (rc == NEXUS_GRAPHICS2D_QUEUED) {
        rc = BKNI_WaitForEvent(context->checkpointEvent, BKNI_INFINITE);
    }
    BDBG_ASSERT(!rc);

    if ( context->imageInput ) {
        setImageInput( context , tmpImg, fillSettings.rect );
        NEXUS_Surface_Destroy(tmpImg);
    }
    rc = displayVdcCrc( context, *pRef_Data , NULL );

    return rc;
}
#endif
