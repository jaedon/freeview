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
 * $brcm_Workfile: thumbnail.h $
 * $brcm_Revision: 8 $
 * $brcm_Date: 10/15/10 2:59p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/thumbnail/thumbnail.h $
 * 
 * 8   10/15/10 2:59p erickson
 * SW35230-1411: use checkpoint instead of deprecated blockedSync
 * 
 * 7   8/14/09 3:13p erickson
 * PR55994: refactor to only use bthumbnail_extractor, not
 * bthumbnail_manager and _stream
 *
 * 6   4/22/09 2:11p erickson
 * PR45125: added bkni_multi.h
 *
 * 5   7/28/08 1:20p erickson
 * PR45125: fix thumbnail app's use of spacing. now it uses view_window,
 * not full_view.
 *
 * 4   7/24/08 11:50a erickson
 * PR42788: update for DTV
 *
 * 3   3/17/08 9:28a erickson
 * PR40307: use thumbnail library
 *
 * 2   3/8/08 5:15a erickson
 * PR40307: update
 *
 *****************************************************************************/
#ifndef THUMBNAIL_H__
#define THUMBNAIL_H__

#include "bstd.h"
#include "bkni.h"
#include "bkni_multi.h"

#include "nexus_still_decoder.h"
#include "nexus_surface.h"
#include "nexus_graphics2d.h"
#include "nexus_playpump.h"
#include "nexus_types.h"
#include "nexus_video_types.h"
#include "bthumbnail_extractor.h"
#include <stdio.h>

#define MAX_THUMBNAILS 200
#define DISPLAYED_THUMBNAILS 5

#if 0
#define THUMB_WIDTH 240
#define THUMB_HEIGHT 136
#define THUMB_GAP 15
#define THUMB_EDGE 50
#else
#define THUMB_WIDTH 220
#define THUMB_HEIGHT 120
#define THUMB_GAP 10
#define THUMB_EDGE 50
#endif

/* global data structure which makes for a simple demo app */
typedef struct thumbnail_data {
    NEXUS_VideoFormat display_format;
    const char *datafilename;
    const char *indexfilename;
    NEXUS_TransportType transportType;
    NEXUS_VideoCodec videoCodec;
    unsigned pid;

    BKNI_EventHandle still_done;

    FILE *stdio_indexfile, *stdio_datafile;
    bfile_io_read_t indexfile, datafile;
    bthumbnail_extractor_t thumbnail_extractor;

    struct {
        unsigned time;
        NEXUS_SurfaceHandle surface;
    } thumbnail[DISPLAYED_THUMBNAILS];

    unsigned spacing; /* in msec */
    unsigned base_time; /* time of leftmost still */
    unsigned current_time;

    NEXUS_Graphics2DHandle blitter;
    BKNI_EventHandle blitterEvent;
    NEXUS_PlaypumpHandle playpump;
    NEXUS_StillDecoderHandle stillDecoder;
} thumbnail_data;

extern thumbnail_data g_data;

/* main populates g_data, then invokes these functions to run the demo */
int  thumbnail_demo_init(void);
int  thumbnail_demo_run(void);
void thumbnail_demo_uninit(void);
void checkpoint(void);

/* thumbnail_stills code interacts with bthumbnail_extractor */
int thumbnail_decode_stills_init(void);
void thumbnail_decode_stills_uninit(void);
NEXUS_SurfaceHandle thumbnail_decode_still(unsigned time);

#endif
