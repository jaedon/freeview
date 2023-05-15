/******************************************************************************
 *    (c)2011-2012 Broadcom Corporation
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
 * $brcm_Workfile: brc_client.h $
 * $brcm_Revision: 6 $
 * $brcm_Date: 10/16/12 5:21p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/examples/multiprocess/brc_client.h $
 * 
 * 6   10/16/12 5:21p erickson
 * SW7425-1792: add client hdmi input, protect with
 *  NEXUS_Platform_AcquireObject
 * 
 * 5   7/9/12 1:57p erickson
 * SW7231-844: allow server to pick default NEXUS_MemoryType_eFull heap
 * 
 * 4   4/20/12 10:08a erickson
 * SW7425-2882: create fully mapped heap and notify using app ipc
 * 
 * 3   10/27/11 2:37p erickson
 * SW7425-1364: rename to brc
 * 
 * 2   10/26/11 3:56p erickson
 * SW7425-1364: extend app ipc
 * 
 * 1   10/4/11 5:44p vsilyaev
 * SW7425-1364: Reference applicaion IPC and reference server
 * 
 *****************************************************************************/
#ifndef BRCM_CLIENT_H__
#define BRCM_CLIENT_H__

#include "bipc.h"
#include "nexus_platform_client.h"
#include "nexus_surface_compositor.h"
#include "nexus_video_decoder.h"
#if NEXUS_HAS_HDMI_INPUT
#include "nexus_hdmi_input.h"
#else
typedef void *NEXUS_HdmiInputHandle;
#endif

/* 
Broadcom Reference Client (brc) protocol 

The following API's are for reference only.
It is expected that customers will create their own API's for application IPC.
*/

#ifdef __cplusplus
extern "C" {
#endif

typedef struct brc_client *brc_client_t;

typedef struct brc_client_client_name  {
    char string[32];
} brc_client_client_name;

typedef struct brc_client_client_configuration {
    brc_client_client_name  name; /* name of the client application */
    struct {
        struct {
            bool required;
            NEXUS_Rect position;
        } screen; /* graphics */
        bool audioDecoder;
        bool audioPlayback;
        bool videoDecoder;
        bool inputRouter;
    } resources;
} brc_client_client_configuration;

typedef struct brc_client_client_info {
    NEXUS_Certificate certificate;
    NEXUS_SurfaceCompositorClientId surfaceClientId;
    unsigned audioDecoderId;
    unsigned videoDecoderId;
    unsigned inputRouterId;
} brc_client_client_info;

brc_client_t  brc_client_create(bipc_t ipc, const brc_client_client_configuration *config);

void  brc_client_destroy(brc_client_t client);

int   brc_client_get_clientinfo(brc_client_t client, brc_client_client_info *info);

/**
client requests server to change the format on the main display
server app can decide to change the secondary display to match 50/60 frame rate
server app can decide to add/remove video outputs as required
**/
int   brc_client_set_display_format(brc_client_t client, NEXUS_VideoFormat format);

/**
connect a client-owned VideoImageInput or regular VideoDecoder to a video window.
server will decide to connect to one or two displays.
**/
int   brc_client_add_hdmi_input(brc_client_t client, unsigned window_id, NEXUS_HdmiInputHandle input);
int   brc_client_add_video_decoder_input(brc_client_t client, unsigned window_id, NEXUS_VideoDecoderHandle input);
void  brc_client_remove_window_input(brc_client_t client, unsigned window_id);

/**
tell server to reconfigure simple decoders with mosaic mode or regular decoders
in mosaic mode, more decoders are available, but they lack PQ and clipping features
**/
int   brc_client_set_simple_decoder_mosaic_mode(brc_client_t client, bool enabled);

/**
position a client on the screen
**/
void  brc_client_get_composition(brc_client_t client, NEXUS_SurfaceComposition *pComposition);
int   brc_client_set_composition(brc_client_t client, const NEXUS_SurfaceComposition *pComposition);

/* ask the server to close the video decoder so that the client can open it.
returns 0 if server does not have the decoder open. */
int   brc_client_request_video_decoder(brc_client_t client, unsigned id);

/* tell the server that the client has closed its local decoder and
so the server can open it. */
void brc_client_return_video_decoder(brc_client_t client, unsigned id);

#ifdef __cplusplus
}
#endif

#endif /* BRCM_CLIENT_H__ */

