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
 * $brcm_Workfile: nxsmaster.c $
 * $brcm_Revision: DirectFB_1_4_15_Port/2 $
 * $brcm_Date: 3/13/12 4:01p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /AppLibs/opensource/directfb/src/broadcom_files/private/DirectFB/1.4.15/platform/nxsmaster.c $
 *
 * DirectFB_1_4_15_Port/2   3/13/12 4:01p robertwm
 * SW7425-2616: DirectFB-1.4.15: Cannot run Nexus or OGL Nexus
 *  applications in DFB-XS mode.
 *
 * DirectFB_1_4_15_Port/1   2/9/12 2:34p christ
 * SW7425-1550: DirectFB: Require integration of Nexus Surface Compositor
 *  into DFB
 *
 * DirectFB_1_4_15_Port/DirectFB-XS_1_4_15_Port/3   1/27/12 7:37p christ
 * SW7231-625: DirectFB-XS: Allow Nexus client only builds.
 *
 * DirectFB_1_4_15_Port/DirectFB-XS_1_4_15_Port/2   1/16/12 1:26p robertwm
 * SW7425-2186: DirectFB-XS-1.4.15: Incorrect Nexus heap configuration in
 *  nxsmaster.c for multi-buffering mode.
 *
 * DirectFB_1_4_15_Port/DirectFB-XS_1_4_15_Port/1   12/9/11 5:10p robertwm
 * SW7425-1550: DirectFB: Require integration of Nexus Surface Compositor
 *  into DFB.
 *
 * DirectFB-XS_1_4_15_Port/1   12/9/11 2:39p robertwm
 * SW7425-1550: DirectFB: Require integration of Nexus Surface Compositor
 *  into DFB.
 *
 ******* Initially based off of Nexus refsw_server.c version 16 *****
 *
 *****************************************************************************/
#include "nexus_types.h"
#include "nexus_platform.h"
#include "nexus_platform_server.h"
#include "nexus_display.h"
#include "nexus_video_window.h"
#include "nexus_video_decoder.h"
#include "nexus_mosaic_video_decoder.h"
#include "nexus_mosaic_display.h"
#include "nexus_video_input.h"
#include "nexus_audio_decoder.h"
#include "nexus_audio_input.h"
#include "nexus_audio_mixer.h"
#include "nexus_hdmi_output.h"
#include "nexus_ac3_encode.h"
#include "nexus_simple_video_decoder_server.h"
#include "nexus_simple_audio_decoder_server.h"
#include "nexus_stc_channel.h"
#include "bipc_server.h"
#include "brc_client.h"
#include "nexus_surface_compositor.h"
#include "nexus_surface_cursor.h"
#include "nexus_input_router.h"
#include "blst_list.h"
#include "nexus_core_utils.h"
#if NEXUS_HAS_HDMI_INPUT
#include "nexus_hdmi_input.h"
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/poll.h>

#include "platform_init.h"

#if BCMNEXUS_NIR_SUPPORT
#include <linux/input.h> /* used for input_event support */
#include "nexus_ir_input.h"
#include "bcmnexus_ir_inputmode.h"
#include "bcmnexus_ir_convert.h"
#include "config.h"
#include "bcmnexus_sys_defines.h"
#if NEXUS_HAS_KEYPAD
#include "nexus_keypad.h"
#endif

#define NUM_IR_EVENTS                   10
#define DEFAULT_IR_REPEAT_MILLI_SECS    120
#define MAX_NIR_CLIENTS                 30
static bool bInputRouterExit;
#endif


BDBG_MODULE(nxsmaster);

#define SIMPLE_VIDEO_DECODER_ID   1
#define SIMPLE_AUDIO_DECODER_ID   1
#define SIMPLE_AUDIO_PLAYBACK_ID_BASE   1

#include "ipc_stubs_server.h"
#include "refsw_session.h"

#if NEXUS_HAS_SURFACE_COMPOSITOR
struct b_refsw_client;

struct brc_client {
    bool created;
    struct b_refsw_client *parent;
};

BDBG_OBJECT_ID(b_refsw_client);

struct display_state
{
    struct {
        NEXUS_DisplayHandle display;
        NEXUS_VideoWindowHandle window[NEXUS_NUM_VIDEO_WINDOWS];
        NEXUS_VideoWindowHandle mosaic_window[NEXUS_NUM_VIDEO_WINDOWS];
    } display[NEXUS_NUM_DISPLAYS];
};

struct simpledecoder_state
{
    struct display_state *display; /* shared */

    /* simple decoder containers */
    NEXUS_SimpleVideoDecoderHandle simpleVideoDecoder;
    NEXUS_SimpleAudioDecoderHandle simpleAudioDecoder;
    NEXUS_SimpleAudioPlaybackHandle simpleAudioPlayback[2];
    bool enabled; /* containers have resources */
    bool mosaic; /* true if resources are opened in mosaic */

    /* resources used to populate the containers */
    struct {
        NEXUS_VideoDecoderHandle videoDecoder; /* may be NULL */
    NEXUS_AudioDecoderHandle audioDecoder0, audioDecoder1;
    NEXUS_AudioMixerHandle audioMixer;
    NEXUS_Ac3EncodeHandle ac3Encoder;
#define NUM_AUDIO_PLAYBACKS 2
        NEXUS_AudioPlaybackHandle audioPlayback[NUM_AUDIO_PLAYBACKS];
    } resources;
};

typedef struct b_refsw_server {
    BLST_D_HEAD(b_refsw_client_list, b_refsw_client) clients;
    BKNI_MutexHandle lock;
    struct {
        bool exit;
        bipc_t server;
    } ipc;
     struct {
        bool move;
        bool hdsd_simul;
        bool cursor;
        unsigned framebuffers;
        const char *tunnel_client;
    } options;
    struct display_state display_state;
    struct simpledecoder_state decoder_state;

    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_SurfaceCompositorHandle surfaceCompositor;
    BKNI_EventHandle inactiveEvent;

    struct {
        NEXUS_SurfaceHandle surface;
        NEXUS_SurfaceCursorHandle cursor;
        int x,y; /* current coordinates */
    } cursor;

    struct {
        unsigned client;
        NEXUS_SurfaceCompositorClientId surfaceClientId;
        unsigned inputRouterId;
    } lastId;

} *b_refsw_server_t;

typedef struct b_refsw_client {
    BDBG_OBJECT(b_refsw_client)
    BLST_D_ENTRY(b_refsw_client) link;
    struct {
        int fd;
        bipc_server_client_t client;
        NEXUS_ClientHandle nexusClient;
    } ipc;
    struct {
        NEXUS_SurfaceClientHandle screen;
        NEXUS_InputClientHandle input;
    } resources;
    brc_client_client_name name;
    brc_client_client_info info;
    struct brc_client client;
    b_refsw_server_t server;
    struct {
        int x;
        int y;
        int width;
        int height;
    } movement;
} *b_refsw_client_t;

static int refserver_simpledecoder_set_decoder(struct simpledecoder_state *p_state, bool enabled);
static void b_set_windows(b_refsw_server_t server, bool enabled, NEXUS_SurfaceCompositorSettings *settings);
static void b_open_mosaic_windows(b_refsw_server_t server, bool enabled);

static void complete(void *context, int param)
{
    BSTD_UNUSED(param);
    BKNI_SetEvent((BKNI_EventHandle)context);
}

static b_refsw_client_t b_refsw_client_create(b_refsw_server_t server)
{
    b_refsw_client_t client;

    client = BKNI_Malloc(sizeof(*client));
    if(client==NULL) {(void)BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);goto err_alloc;}
    BDBG_OBJECT_INIT(client, b_refsw_client);
    BKNI_Memset(&client->name, 0, sizeof(client->name));
    BKNI_Memset(&client->info, 0, sizeof(client->info));
    client->resources.screen = NULL;
    client->resources.input = NULL;
    BLST_D_INSERT_HEAD(&server->clients, client, link);
    client->server = server;
    client->client.created = false;
    client->client.parent = NULL;
    client->movement.x = 0;
    client->movement.y = 0;
    client->movement.width = 0;
    client->movement.height = 0;

    return client;

err_alloc:
    return NULL;
}

static void b_refsw_client_shutdown(b_refsw_client_t client)
{
    if(client->resources.screen) {
        NEXUS_SurfaceCompositor_DestroyClient(client->resources.screen);
        client->resources.screen = NULL;
    }
    if(client->ipc.nexusClient) {
        NEXUS_Platform_UnregisterClient(client->ipc.nexusClient);
        client->ipc.nexusClient = NULL;
    }
    return;
}

static void b_refsw_client_destroy(b_refsw_client_t client)
{
    BDBG_OBJECT_ASSERT(client, b_refsw_client);
    b_refsw_client_shutdown(client);
    BLST_D_REMOVE(&client->server->clients, client, link);
    BDBG_OBJECT_DESTROY(client, b_refsw_client);
    BKNI_Free(client);
    return;
}

brc_client_t  brc_client_create(bipc_t ipc, const brc_client_client_configuration *config)
{
    b_refsw_client_t client = (void *)ipc;
    b_refsw_server_t server;
    NEXUS_ClientSettings clientSettings;
    NEXUS_Error rc;
    bool tunnelCapable;

    BDBG_OBJECT_ASSERT(client, b_refsw_client);
    if(client->client.created) {
        BDBG_WRN(("brc_client_create:%#lx (%d) client already created", (unsigned long)client, client->ipc.fd));
        return NULL;
    }
    server = client->server;
    client->name = config->name;
    client->name.string[sizeof(client->name.string)-1]='\0';
    BKNI_Memset(&client->info, 0, sizeof(client->info));
    client->info.certificate.length = BKNI_Snprintf((char *)client->info.certificate.data, sizeof(client->info.certificate.data), "%u,%#x%#x,%s", server->lastId.client, lrand48(), lrand48(), client->name.string);
    if(client->info.certificate.length>=sizeof(client->info.certificate.data)-1) {
        (void)BERR_TRACE(BERR_NOT_SUPPORTED);
        goto err_certificate;
    }
    tunnelCapable = server->options.tunnel_client && strstr(client->name.string, server->options.tunnel_client);
    server->lastId.client++;
    BDBG_LOG(("client %s registering as '%s'", client->name.string, (char *)client->info.certificate.data));
    if(config->resources.screen.required) {
        NEXUS_SurfaceCompositorClientSettings client_settings;
        client->resources.screen = NEXUS_SurfaceCompositor_CreateClient(server->surfaceCompositor, server->lastId.surfaceClientId);
        if(!client->resources.screen) {
            (void)BERR_TRACE(BERR_NOT_SUPPORTED);
            goto err_screen;
        }
        client->info.surfaceClientId = server->lastId.surfaceClientId;
        NEXUS_SurfaceCompositor_GetClientSettings(server->surfaceCompositor, client->resources.screen, &client_settings);
        client_settings.composition.position = config->resources.screen.position;
        client_settings.composition.zorder = client->info.surfaceClientId;
        switch(client->info.surfaceClientId%4) {
        case 0:
            client->movement.x = 1;
            client->movement.y = 1;
            client->movement.width = 0;
            client->movement.height = 0;
            client_settings.composition.position.width = 100;
            client_settings.composition.position.height= 100;
            break;
        case 1:
            client->movement.x = -1;
            client->movement.y = 2;
            client->movement.width = 3;
            client->movement.height = 3;
            client_settings.composition.position.width = 150;
            client_settings.composition.position.height= 150;
            break;
        case 2:
            client->movement.x = -3;
            client->movement.y = -2;
            client->movement.width = 0;
            client->movement.height = 0;
            client_settings.composition.position.width = 150;
            client_settings.composition.position.height= 150;
            break;
        default:
        case 3:
            break;
        }
        client_settings.composition.virtualDisplay.width = 720;
        client_settings.composition.virtualDisplay.height = 480;
        if(tunnelCapable) {
            client_settings.tunnelCapable = true;
            client->movement.x = 0;
            client->movement.y = 0;
            client->movement.width = 0;
            client->movement.height = 0;
            client_settings.composition.position.width = 720;
            client_settings.composition.position.height= 480;
            client_settings.composition.position.width = 720;
            client_settings.composition.position.height = 480;
            client_settings.composition.position.x = 0;
            client_settings.composition.position.y = 0;
            BDBG_LOG(("client %s(%p) is tunnelCapable", client->name.string, client->resources.screen));
        }
        rc = NEXUS_SurfaceCompositor_SetClientSettings(server->surfaceCompositor, client->resources.screen, &client_settings);
        if(rc!=NEXUS_SUCCESS) {
            (void)BERR_TRACE(BERR_NOT_SUPPORTED);
            goto err_screen_settings;
        }
        server->lastId.surfaceClientId++;
    }
    if(config->resources.audioDecoder) {
        client->info.audioDecoderId = SIMPLE_AUDIO_DECODER_ID;
    }
    if(config->resources.videoDecoder) {
        client->info.videoDecoderId = SIMPLE_VIDEO_DECODER_ID;
    }
    NEXUS_Platform_GetDefaultClientSettings(&clientSettings);
    clientSettings.authentication.certificate = client->info.certificate;
    clientSettings.configuration.heap[0] = NEXUS_Platform_GetFramebufferHeap(NEXUS_OFFSCREEN_SURFACE);
    clientSettings.configuration.heap[NEXUS_MAX_HEAPS-1] = server->platformConfig.heap[NEXUS_MAX_HEAPS-1];
    if(tunnelCapable) {
        clientSettings.configuration.heap[1] = NEXUS_Platform_GetFramebufferHeap(0);
    }
    clientSettings.configuration.mode = NEXUS_ClientMode_eUntrusted;
    client->ipc.nexusClient = NEXUS_Platform_RegisterClient(&clientSettings);
    if(!client->ipc.nexusClient) {(void)BERR_TRACE(BERR_NOT_SUPPORTED);goto err_client;}

    client->client.created = true;
    client->client.parent = client;

    return &client->client;

err_client:
err_screen_settings:
    NEXUS_SurfaceCompositor_DestroyClient(client->resources.screen);
err_screen:
err_certificate:
    return NULL;
}

int   brc_client_get_clientinfo(brc_client_t _client, brc_client_client_info *info)
{
    b_refsw_client_t client = _client->parent;

    BDBG_OBJECT_ASSERT(client, b_refsw_client);
    *info = client->info;
    return 0;
}

void  brc_client_get_composition(brc_client_t _client, NEXUS_SurfaceComposition *pComposition)
{
    b_refsw_client_t client = _client->parent;
    NEXUS_SurfaceCompositorClientSettings client_settings;

    BDBG_OBJECT_ASSERT(client, b_refsw_client);
    NEXUS_SurfaceCompositor_GetClientSettings(client->server->surfaceCompositor, client->resources.screen, &client_settings);
    *pComposition = client_settings.composition;
}

int   brc_client_set_composition(brc_client_t _client, const NEXUS_SurfaceComposition *pComposition)
{
    b_refsw_client_t client = _client->parent;
    NEXUS_SurfaceCompositorClientSettings client_settings;
    NEXUS_Error rc;

    BDBG_OBJECT_ASSERT(client, b_refsw_client);
    NEXUS_SurfaceCompositor_GetClientSettings(client->server->surfaceCompositor, client->resources.screen, &client_settings);
    client_settings.composition = *pComposition;
    rc = NEXUS_SurfaceCompositor_SetClientSettings(client->server->surfaceCompositor, client->resources.screen, &client_settings);
    /* stop movement */
    client->movement.x = 0;
    client->movement.y = 0;
    client->movement.width = 0;
    client->movement.height = 0;

    return (int)BERR_TRACE(rc);
}

static void b_set_windows(b_refsw_server_t server, bool enabled, NEXUS_SurfaceCompositorSettings *settings)
{
    unsigned j;
    for (j=0;j<NEXUS_NUM_DISPLAYS;j++) {
        if (enabled) {
            unsigned i;
            for (i=0;i<NEXUS_NUM_VIDEO_WINDOWS;i++) {
                NEXUS_VideoWindowHandle window;
                window = server->display_state.display[j].mosaic_window[i]?server->display_state.display[j].mosaic_window[i]:server->display_state.display[j].window[i];
                settings->display[j].window[i].window = window;
                if (window) {
                    NEXUS_VideoWindow_GetSettings(window, &settings->display[j].window[i].settings);
                }
            }
        }
        else {
            BKNI_Memset(settings->display[j].window, 0, sizeof(settings->display[j].window));
        }
    }
}

static void b_open_mosaic_windows(b_refsw_server_t server, bool enabled)
{
    unsigned i, j;
    for (j=0;j<NEXUS_NUM_DISPLAYS;j++) {
        for (i=0;i<NEXUS_NUM_VIDEO_WINDOWS;i++) {
            if (server->display_state.display[j].mosaic_window[i] && !enabled) {
                NEXUS_VideoWindow_Close(server->display_state.display[j].mosaic_window[i]);
                server->display_state.display[j].mosaic_window[i] = NULL;
            }
            else if (!server->display_state.display[j].mosaic_window[i] && enabled && server->display_state.display[j].window[i]) {
                server->display_state.display[j].mosaic_window[i] = NEXUS_VideoWindow_OpenMosaic(server->display_state.display[j].window[i], 0);
            }
        }
    }
}

int   brc_client_set_display_format(brc_client_t _client, NEXUS_VideoFormat format)
{
    NEXUS_DisplayHandle display;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_SurfaceCompositorSettings surface_compositor_settings;
    int rc;
    NEXUS_VideoFormatInfo formatInfo;
    b_refsw_client_t client = _client->parent;

    BKNI_ResetEvent(client->server->inactiveEvent);

    NEXUS_SurfaceCompositor_GetSettings(client->server->surfaceCompositor, &surface_compositor_settings);
    surface_compositor_settings.enabled = false;
    rc = NEXUS_SurfaceCompositor_SetSettings(client->server->surfaceCompositor, &surface_compositor_settings);
    if (rc) return BERR_TRACE(rc);

    rc = BKNI_WaitForEvent(client->server->inactiveEvent, 5000);
    if (rc) return BERR_TRACE(rc);
    while (1) {
        NEXUS_SurfaceCompositorStatus status;
        rc = NEXUS_SurfaceCompositor_GetStatus(client->server->surfaceCompositor, &status);
        BDBG_ASSERT(!rc);
        if (!status.numAcquiredTunneledSurfaces) break;
        BKNI_Sleep(100);
    }

    display = client->server->display_state.display[0].display;
    NEXUS_Display_GetSettings(display, &displaySettings);
    displaySettings.format = format;
    rc = NEXUS_Display_SetSettings(display, &displaySettings);
    if (rc) return BERR_TRACE(rc);

    NEXUS_VideoFormat_GetInfo(format, &formatInfo);

    NEXUS_SurfaceCompositor_GetSettings(client->server->surfaceCompositor, &surface_compositor_settings);
    surface_compositor_settings.enabled = true;
    NEXUS_Display_GetGraphicsSettings(display, &surface_compositor_settings.display[0].graphicsSettings);
    surface_compositor_settings.display[0].graphicsSettings.clip.width = formatInfo.width;
    surface_compositor_settings.display[0].graphicsSettings.clip.height = formatInfo.height;
    rc = NEXUS_SurfaceCompositor_SetSettings(client->server->surfaceCompositor, &surface_compositor_settings);
    if (rc) return BERR_TRACE(rc);

    return 0;
}

int   brc_client_add_window_input(brc_client_t _client, unsigned window_id, NEXUS_VideoInput input)
{
    b_refsw_client_t client = _client->parent;
    unsigned i;
    int rc;

    if (window_id >= NEXUS_NUM_VIDEO_WINDOWS) {
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }
    for (i=0;i<NEXUS_NUM_DISPLAYS;i++) {
        NEXUS_VideoWindowHandle window = client->server->display_state.display[i].window[window_id];
        if (window) {
            rc = NEXUS_VideoWindow_AddInput(window, input);
            if (rc) return BERR_TRACE(rc);
        }
    }
    return 0;
}

void  brc_client_remove_window_input(brc_client_t _client, unsigned window_id)
{
    b_refsw_client_t client = _client->parent;
    unsigned i;

    if (window_id >= NEXUS_NUM_VIDEO_WINDOWS) {
        BERR_TRACE(NEXUS_INVALID_PARAMETER);
        return;
    }
    for (i=0;i<NEXUS_NUM_DISPLAYS;i++) {
        NEXUS_VideoWindowHandle window = client->server->display_state.display[i].window[window_id];
        if (window) {
            NEXUS_VideoWindow_RemoveAllInputs(window);
        }
    }
}

int  brc_client_set_simple_decoder_mosaic_mode(brc_client_t _client, bool mosaic_enabled)
{
    b_refsw_client_t client = _client->parent;
    if (client->server->decoder_state.mosaic != mosaic_enabled) {
        bool decode_enabled = client->server->decoder_state.enabled;
        int rc;
        NEXUS_SurfaceCompositorSettings surface_compositor_settings;

        NEXUS_SurfaceCompositor_GetSettings(client->server->surfaceCompositor, &surface_compositor_settings);

        b_set_windows(client->server, false, &surface_compositor_settings);
        rc = NEXUS_SurfaceCompositor_SetSettings(client->server->surfaceCompositor, &surface_compositor_settings);
        if (rc) return BERR_TRACE(rc);

        /* must set false before changing .mosaic so that mosaic windows are destroyed */
        if (decode_enabled) {
            refserver_simpledecoder_set_decoder(&client->server->decoder_state, false);
        }
        client->server->decoder_state.mosaic = mosaic_enabled;

        b_open_mosaic_windows(client->server, client->server->decoder_state.mosaic);

        if (decode_enabled) {
            refserver_simpledecoder_set_decoder(&client->server->decoder_state, true);
        }

        b_set_windows(client->server, true, &surface_compositor_settings);
        rc = NEXUS_SurfaceCompositor_SetSettings(client->server->surfaceCompositor, &surface_compositor_settings);
        if (rc) return BERR_TRACE(rc);
    }
    return 0;
}

int   brc_client_request_video_decoder(brc_client_t _client, unsigned id)
{
    b_refsw_client_t client = _client->parent;
    if (id != 0) {
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }
    return refserver_simpledecoder_set_decoder(&client->server->decoder_state, false);
}

void brc_client_return_video_decoder(brc_client_t _client, unsigned id)
{
    b_refsw_client_t client = _client->parent;
    if (id != 0) {
        BERR_TRACE(NEXUS_INVALID_PARAMETER);
        return;
    }
    (void)refserver_simpledecoder_set_decoder(&client->server->decoder_state, true);
}

void  brc_client_destroy(brc_client_t _client)
{
    b_refsw_client_t client = _client->parent;

    BDBG_OBJECT_ASSERT(client, b_refsw_client);
    b_refsw_client_shutdown(client);
    client->client.created = false;
    client->client.parent = NULL;
    return;
}

#define B_MAX_CLIENTS   64

static void b_refsw_ipc_client_destroy(b_refsw_server_t server, b_refsw_client_t client)
{
    bipc_server_client_destroy(server->ipc.server, client->ipc.client);
    close(client->ipc.fd);
    b_refsw_client_destroy(client);
    return;
}

int brc_client_add_video_decoder_input(brc_client_t _client, unsigned window_id, NEXUS_VideoDecoderHandle handle)
{
    (void)_client;
    (void)window_id;
    (void)handle;
    return 0; // not supported for now
}

#if NEXUS_HAS_HDMI_INPUT
int brc_client_add_hdmi_input(brc_client_t _client, unsigned window_id, NEXUS_HdmiInputHandle handle)
#else
int brc_client_add_hdmi_input(brc_client_t _client, unsigned window_id, void* handle)
#endif
{
    (void)_client;
    (void)window_id;
    (void)handle;
    return 0; // not supported for now
}

static void *ipc_thread(void *context)
{
    b_refsw_server_t server = context;
    int listen_fd;
    int rc;
    struct sockaddr_un sock_addr;
    b_refsw_client_t client;
    unsigned prev_mask;

    BKNI_AcquireMutex(server->lock);
    listen_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if(listen_fd < 0) { perror("");rc=BERR_TRACE(errno); goto done; }

    BKNI_Memset(&sock_addr, 0, sizeof(sock_addr));
    sock_addr.sun_family = AF_UNIX;
    BDBG_ASSERT(sizeof(sock_addr.sun_path)>=sizeof(b_refsw_endpoint));
    unlink(b_refsw_endpoint);
    BKNI_Memcpy(sock_addr.sun_path, b_refsw_endpoint, sizeof(b_refsw_endpoint));

    fcntl(listen_fd, F_SETFL, O_NONBLOCK);

    prev_mask = umask(0);
    rc = bind(listen_fd, (struct sockaddr *)&sock_addr, sizeof(sock_addr.sun_family)+sizeof(b_refsw_endpoint));
    umask(prev_mask);
    if(rc!=0) { perror("");rc=BERR_TRACE(errno); goto done; }
    rc = listen(listen_fd, 10);
    if(rc!=0) { perror("");rc=BERR_TRACE(errno); goto done; }
    while(!server->ipc.exit) {
        struct pollfd fds[B_MAX_CLIENTS];
        b_refsw_client_t clients[B_MAX_CLIENTS];
        unsigned i,nfds,events;
        const unsigned timeout = 1000;

        for(client=BLST_D_FIRST(&server->clients),i=0;i<B_MAX_CLIENTS;i++,client=BLST_D_NEXT(client, link))  {
            fds[i].revents = 0;
            clients[i] = client;
            fds[i].events = POLLIN;
            if(client) {
                fds[i].fd = client->ipc.fd;
            } else {
                /* if reached B_MAX_CLIENTS, stop listening for new clients */
                fds[i].fd = listen_fd;
                i++;
                break;
            }
        }
        nfds = i;
        BKNI_ReleaseMutex(server->lock);
        BDBG_MSG(("poll %u", nfds));
        rc = poll(fds, nfds, timeout);
        BDBG_MSG(("poll %u->%d", nfds, rc));
        BKNI_AcquireMutex(server->lock);
        if(rc<0) { perror("");rc=BERR_TRACE(errno); goto done; }
        events = (unsigned)rc;
        for(i=0;i<nfds && events ;i++) {
            if(fds[i].revents & POLLIN) {
                events --;
                client = clients[i];
                if(client) {
                    rc = bipc_server_client_process(server->ipc.server, client->ipc.client);
                    if(rc!=0) {
                        BDBG_LOG(("closing client:%#lx with fd:%d ", (unsigned long)client, client->ipc.fd));
                        b_refsw_ipc_client_destroy(server, client);
                    }
                } else {
                    bipc_server_client_create_settings settings;

                    BDBG_ASSERT(listen_fd==fds[i].fd);
                    rc = accept(listen_fd, NULL, NULL);
                    if(rc<0) { perror("");(void)BERR_TRACE(errno); goto done;}
                    BDBG_LOG(("new client with fd:%d connected", rc));

                    client = b_refsw_client_create(server);
                    if(!client) {(void)BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);goto done;}
                    bipc_server_get_default_client_create_settings(&settings);
                    client->ipc.fd = rc;
                    settings.recv_fd = rc;
                    settings.send_fd = rc;
                    settings.ipc_context = client;
                    fcntl(rc, F_SETFL, O_NONBLOCK);
                    client->ipc.client = bipc_server_client_create(server->ipc.server, &settings);
                    if(!client->ipc.client) {(void)BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);goto done;}
                }
            }
        }
    }
done:
    while(NULL!=(client=BLST_D_FIRST(&server->clients))) {
        b_refsw_ipc_client_destroy(server, client);
    }
    close(listen_fd);

    BKNI_ReleaseMutex(server->lock);
    return NULL;
}

static void print_usage(const char *name)
{
    printf(
    "usage: nexus %s [--help|-h]\n"
    "options:\n"
    "  --help|-h      \tprint this help screen\n"
    "  -move off      \tdisable movement\n"
    "  -sd off        \tdisable Standart Definition display\n"
    "  -3d on/lr/ur   \tenable stereoscopic (3D) video\n"
    "  -native_3d on  \tenable native stereoscopic (3D) video\n"
    "  -cursor on     \tenable cursor\n"
#if BCMNEXUS_NSC_SUPPORT == BCMNEXUS_NSC_TUNNELLED_MODE
    "  -tunnel client \tname of client that allowed to allocate tunnelled surfaces\n"
#endif
    "  -framebuffers n\tnumber of framebuffers allocated for the primary display\n",
    name
    );
}

static const bipc_server_descriptor * const ipc_interfaces [] = {
    &bipc_brc_client_server_descriptor
};

static void framebuffer_callback(void *context, int param)
{
    b_refsw_server_t server = context;
    b_refsw_client_t client;

    BSTD_UNUSED(param);
    BKNI_AcquireMutex(server->lock);

    for(client=BLST_D_FIRST(&server->clients);client;client=BLST_D_NEXT(client, link))  {

        NEXUS_SurfaceCompositorClientSettings client_settings;
        NEXUS_Error rc;

        if(!server->options.move) {
            break;
        }
        if(!client->resources.screen) {
            continue;
        }

        if (!client->movement.x &&
            !client->movement.y &&
            !client->movement.width &&
            !client->movement.height) continue;

        NEXUS_SurfaceCompositor_GetClientSettings(server->surfaceCompositor, client->resources.screen, &client_settings);
        client_settings.composition.position.x += client->movement.x;
        client_settings.composition.position.y += client->movement.y;

        /* slide clients so that offscreen coordinates are tested */
        if (client_settings.composition.position.x >= (int)client_settings.composition.virtualDisplay.width) {
            client_settings.composition.position.x = -1 * (int)client_settings.composition.position.width;
        }
        else if (client_settings.composition.position.x < -1 * (int)client_settings.composition.position.width) {
            client_settings.composition.position.x = client_settings.composition.virtualDisplay.width;
        }
        if (client_settings.composition.position.y >= (int)client_settings.composition.virtualDisplay.height) {
            client_settings.composition.position.y = -1 * (int)client_settings.composition.position.height;
        }
        else if (client_settings.composition.position.y < -1 * (int)client_settings.composition.position.height) {
            client_settings.composition.position.y = client_settings.composition.virtualDisplay.height;
        }

        /* grow/shrink to test clipping combinations */
        if (client->movement.width) {
            client_settings.composition.position.width += client->movement.width;
            if ((client->movement.width > 0 && client_settings.composition.position.width >= 300) ||
                (client->movement.width < 0 && client_settings.composition.position.width < 50))
            {
                client->movement.width *= -1;
            }
        }
        if (client->movement.height) {
            client_settings.composition.position.height += client->movement.height;
            if ((client->movement.height > 0 && client_settings.composition.position.height >= 300) ||
                (client->movement.height < 0 && client_settings.composition.position.height < 50))
            {
                client->movement.height *= -1;
            }
        }

        rc = NEXUS_SurfaceCompositor_SetClientSettings(server->surfaceCompositor, client->resources.screen, &client_settings);
        BDBG_ASSERT(!rc);
    }
    if(server->options.cursor) {
        NEXUS_SurfaceCursorSettings config;
        NEXUS_SurfaceCursor_GetSettings (server->cursor.cursor, &config);
        config.composition.position.x +=2;
        config.composition.position.y +=1;
        if(config.composition.position.x >= config.composition.virtualDisplay.width) {
            config.composition.position.x -= config.composition.virtualDisplay.width+11;
        }
        if(config.composition.position.y >= config.composition.virtualDisplay.height) {
            config.composition.position.y -= config.composition.virtualDisplay.height+17;
        }
        NEXUS_SurfaceCursor_SetSettings (server->cursor.cursor, &config);
    }

    BKNI_ReleaseMutex(server->lock);
    return;
}

int refserver_simpledecoder_init(struct simpledecoder_state *p_state)
{
    NEXUS_SimpleVideoDecoderServerSettings videoSettings;
    NEXUS_SimpleAudioDecoderServerSettings audioSettings;
    NEXUS_SimpleAudioPlaybackServerSettings audioPlaybackSettings;
    unsigned i;

    /* create simple decoder resources, but give them no resources here. */
    NEXUS_SimpleVideoDecoder_GetDefaultServerSettings(&videoSettings);
    p_state->simpleVideoDecoder = NEXUS_SimpleVideoDecoder_Create(SIMPLE_VIDEO_DECODER_ID, &videoSettings);

    NEXUS_SimpleAudioDecoder_GetDefaultServerSettings(&audioSettings);
    p_state->simpleAudioDecoder = NEXUS_SimpleAudioDecoder_Create(SIMPLE_AUDIO_DECODER_ID, &audioSettings);

    NEXUS_SimpleAudioPlayback_GetDefaultServerSettings(&audioPlaybackSettings);
    audioPlaybackSettings.decoder = p_state->simpleAudioDecoder;
    for (i=0;i<NUM_AUDIO_PLAYBACKS;i++) {
        p_state->simpleAudioPlayback[i] = NEXUS_SimpleAudioPlayback_Create(SIMPLE_AUDIO_PLAYBACK_ID_BASE+i, &audioPlaybackSettings);
    }

    /* this populates the simple decoder containers */
    refserver_simpledecoder_set_decoder(p_state, true);

    return 0;
}

void refserver_simpledecoder_uninit(struct simpledecoder_state *p_state)
{
    unsigned i;

    NEXUS_SimpleVideoDecoder_Destroy(p_state->simpleVideoDecoder);
    for (i=0;i<NUM_AUDIO_PLAYBACKS;i++) {
        NEXUS_SimpleAudioPlayback_Destroy(p_state->simpleAudioPlayback[i]);
    }
    NEXUS_SimpleAudioDecoder_Destroy(p_state->simpleAudioDecoder);

    refserver_simpledecoder_set_decoder(p_state, false);
}

static int refserver_simpledecoder_set_decoder(struct simpledecoder_state *p_state, bool enabled)
    {
    NEXUS_SimpleVideoDecoderServerSettings videoSettings;
    NEXUS_SimpleAudioDecoderServerSettings audioSettings;
    NEXUS_SimpleAudioPlaybackServerSettings audioPlaybackSettings[NUM_AUDIO_PLAYBACKS];
    int rc;
    unsigned i;


    /* short circuit no change */
    if (enabled == p_state->enabled) {
        return 0;
    }

    if (enabled) {
        NEXUS_PlatformConfiguration platformConfig;

        NEXUS_Platform_GetConfiguration(&platformConfig);

        NEXUS_SimpleVideoDecoder_GetServerSettings(p_state->simpleVideoDecoder, &videoSettings);
        NEXUS_SimpleAudioDecoder_GetServerSettings(p_state->simpleAudioDecoder, &audioSettings);

        for (i=0;i<NUM_AUDIO_PLAYBACKS;i++) {
            NEXUS_SimpleAudioPlayback_GetServerSettings(p_state->simpleAudioPlayback[i], &audioPlaybackSettings[i]);
        }

        /* open resources and assign to structs. only videoDecoder and audioDecoder0 will be considered required. */
        BDBG_ASSERT(!p_state->resources.videoDecoder);
        BDBG_ASSERT(!p_state->resources.audioDecoder0);

        if (p_state->mosaic) {
            /* TODO: if mosaic, enable more simple decoder containers */
            NEXUS_VideoDecoderOpenMosaicSettings openSettings;
            NEXUS_VideoDecoder_GetDefaultOpenMosaicSettings(&openSettings);
            /* SD */
            openSettings.maxWidth = 720;
            openSettings.maxHeight = 480;
            p_state->resources.videoDecoder = NEXUS_VideoDecoder_OpenMosaic(0, 0, &openSettings);
            if (!p_state->resources.videoDecoder) {
                return BERR_TRACE(NEXUS_UNKNOWN);
            }
        }
        else {
            p_state->resources.videoDecoder = NEXUS_VideoDecoder_Open(0, NULL);
            if (!p_state->resources.videoDecoder) {
                return BERR_TRACE(NEXUS_UNKNOWN);
            }
        }
        videoSettings.window[0] = p_state->display->display[0].window[0];
        videoSettings.window[1] = p_state->display->display[1].window[0];
        videoSettings.videoDecoder = p_state->resources.videoDecoder;

        p_state->resources.audioDecoder0 = NEXUS_AudioDecoder_Open(0, NULL);
        if (!p_state->resources.audioDecoder0) {
            NEXUS_VideoDecoder_Close(p_state->resources.videoDecoder);
            p_state->resources.videoDecoder = NULL;
            return BERR_TRACE(NEXUS_UNKNOWN);
        }
        p_state->resources.audioDecoder1 = NEXUS_AudioDecoder_Open(1, NULL);
        p_state->resources.audioMixer = NEXUS_AudioMixer_Open(NULL);
        p_state->resources.ac3Encoder = NEXUS_Ac3Encode_Open(NULL);
        if (p_state->resources.audioMixer) {
            /* without a mixer, we can't add pcm playback */
            for (i=0;i<NUM_AUDIO_PLAYBACKS;i++) {
                p_state->resources.audioPlayback[i] = NEXUS_AudioPlayback_Open(NEXUS_ANY_ID, NULL);
            }
        }

#define ENABLE_AC3_TRANSCODING (0)  /* Change this to 1 to enable AC3 Transcoding.  Disconnected nodes are not supported on all platforms. */
#if ENABLE_AC3_TRANSCODING
        if (p_state->resources.ac3Encoder) {
            NEXUS_Ac3Encode_AddInput(p_state->resources.ac3Encoder, NEXUS_AudioDecoder_GetConnector(p_state->resources.audioDecoder0, NEXUS_AudioDecoderConnectorType_eMultichannel));
        }
#endif
        audioSettings.primary = p_state->resources.audioDecoder0;
        audioSettings.secondary = p_state->resources.audioDecoder1;

        if (p_state->resources.audioMixer) {
            /* any mixed output must be connected outside of the simple decoder and are not configurable per codec.
            they are used for primary decoder PCM output as well as PCM playback. */
            rc = NEXUS_AudioMixer_AddInput(p_state->resources.audioMixer, NEXUS_AudioDecoder_GetConnector(p_state->resources.audioDecoder0, NEXUS_AudioDecoderConnectorType_eStereo));
            BDBG_ASSERT(!rc);
            for (i=0;i<NUM_AUDIO_PLAYBACKS;i++) {
                if (p_state->resources.audioPlayback[i]) {
                    rc = NEXUS_AudioMixer_AddInput(p_state->resources.audioMixer, NEXUS_AudioPlayback_GetConnector(p_state->resources.audioPlayback[i]));
                    BDBG_ASSERT(!rc);
                }
            }
            rc = NEXUS_AudioOutput_AddInput(NEXUS_AudioDac_GetConnector(platformConfig.outputs.audioDacs[0]), NEXUS_AudioMixer_GetConnector(p_state->resources.audioMixer));
            BDBG_ASSERT(!rc);
#define HDMI_HAS_MIXED_PCM 1
#if HDMI_HAS_MIXED_PCM
            rc = NEXUS_AudioOutput_AddInput(NEXUS_HdmiOutput_GetAudioConnector(platformConfig.outputs.hdmi[0]), NEXUS_AudioMixer_GetConnector(p_state->resources.audioMixer));
            BDBG_ASSERT(!rc);
#endif
        }

#if !HDMI_HAS_MIXED_PCM
        audioSettings.hdmi.outputs[0] = platformConfig.outputs.hdmi[0];
        /* leave settings.hdmi.input[] = NULL, which will output unmixed PCM */
#endif

        /* for this example, SPDIF will be an unmixed output, which is configurable per codec */
        audioSettings.spdif.outputs[0] = platformConfig.outputs.spdif[0];
        /* if input[] is unspecified, it will be PCM from primary decoder */
        if (p_state->resources.audioDecoder1) {
            audioSettings.spdif.input[NEXUS_AudioCodec_eAc3] = NEXUS_AudioDecoder_GetConnector(p_state->resources.audioDecoder1, NEXUS_AudioDecoderConnectorType_eCompressed);
        }
        audioSettings.spdif.input[NEXUS_AudioCodec_eAc3Plus] = NEXUS_AudioDecoder_GetConnector(p_state->resources.audioDecoder0, NEXUS_AudioDecoderConnectorType_eCompressed);
#if ENABLE_AC3_TRANSCODING
        audioSettings.spdif.input[NEXUS_AudioCodec_eAacAdts] =
        audioSettings.spdif.input[NEXUS_AudioCodec_eAacLoas] =
        audioSettings.spdif.input[NEXUS_AudioCodec_eAacPlusAdts] =
        audioSettings.spdif.input[NEXUS_AudioCodec_eAacPlusLoas] = NEXUS_Ac3Encode_GetConnector(p_state->resources.ac3Encoder);
#endif
        for (i=0;i<NUM_AUDIO_PLAYBACKS;i++) {
            audioPlaybackSettings[i].decoder = p_state->simpleAudioDecoder;
            audioPlaybackSettings[i].playback = p_state->resources.audioPlayback[i];
        }
    }
    else {
        /* unassign from structs, but don't close here */
        videoSettings.videoDecoder = NULL;
        videoSettings.window[0] = NULL;
        videoSettings.window[1] = NULL;
        audioSettings.primary = NULL;
        audioSettings.secondary = NULL;
        BKNI_Memset(&audioSettings.spdif, 0, sizeof(audioSettings.spdif));
        BKNI_Memset(&audioSettings.hdmi, 0, sizeof(audioSettings.hdmi));
        for (i=0;i<NUM_AUDIO_PLAYBACKS;i++) {
            audioPlaybackSettings[i].decoder = NULL;
            audioPlaybackSettings[i].playback = NULL;
        }
    }

#if 0
    /*Causes kernel oops. NEXUS_SimpleVideo/AudioDecoder_GetServerSettings()can't be called as decoders are already destroyed during shutdown,
      SetServerSettings() calls below are also disabled. As per refSW phase 6.0(BCM97425)Decoders need to be destroyed before closing to avoid Nexus error and oops.
      */
    rc = NEXUS_SimpleVideoDecoder_SetServerSettings(p_state->simpleVideoDecoder, &videoSettings);
    if (rc) return BERR_TRACE(rc);
    rc = NEXUS_SimpleAudioDecoder_SetServerSettings(p_state->simpleAudioDecoder, &audioSettings);
    if (rc) return BERR_TRACE(rc);
    for (i=0;i<NUM_AUDIO_PLAYBACKS;i++) {
        rc = NEXUS_SimpleAudioPlayback_SetServerSettings(p_state->simpleAudioPlayback[i], &audioPlaybackSettings[i]);
        if (rc) return BERR_TRACE(rc);
    }
#endif

    if (!enabled) {
        /* now that we've removed from the container, we can close resources */
        NEXUS_VideoDecoder_Close(p_state->resources.videoDecoder);
        if (p_state->resources.ac3Encoder) {
            NEXUS_Ac3Encode_Close(p_state->resources.ac3Encoder);
        }
        if (p_state->resources.audioMixer) {
            NEXUS_AudioMixer_RemoveAllInputs(p_state->resources.audioMixer);
            NEXUS_AudioInput_Shutdown(NEXUS_AudioDecoder_GetConnector(p_state->resources.audioDecoder0, NEXUS_AudioDecoderConnectorType_eMultichannel));

            NEXUS_AudioInput_Shutdown(NEXUS_AudioMixer_GetConnector(p_state->resources.audioMixer));
            NEXUS_AudioMixer_Close(p_state->resources.audioMixer);
        }
        for (i=0;i<NUM_AUDIO_PLAYBACKS;i++) {
            if (p_state->resources.audioPlayback[i]) {
                NEXUS_AudioPlayback_Close(p_state->resources.audioPlayback[i]);
            }
        }
        NEXUS_AudioDecoder_Close(p_state->resources.audioDecoder0);
        if (p_state->resources.audioDecoder1) {
                    NEXUS_AudioDecoder_Close(p_state->resources.audioDecoder1);
        }
        BKNI_Memset(&p_state->resources, 0, sizeof(p_state->resources));
    }
    p_state->enabled = enabled;

    return 0;
}

static struct b_refsw_server server;

static void make_cursor(NEXUS_SurfaceHandle surface, const NEXUS_SurfaceCreateSettings *settings)
{
    NEXUS_SurfaceMemory memory;
    unsigned i;

    NEXUS_Surface_GetMemory(surface, &memory);
    BKNI_Memset(memory.buffer, 0, memory.pitch*settings->height);
    /* just a cross */
    for(i=0;i<settings->height;i++) {
        NEXUS_Pixel *pixel = (NEXUS_Pixel*)memory.buffer + i*memory.pitch/sizeof(NEXUS_Pixel) + settings->width/2;
        *pixel = 0xFFFFFFFF;
    }
    for(i=0;i<settings->width;i++) {
        NEXUS_Pixel *pixel = (NEXUS_Pixel *)memory.buffer + (settings->height/2)*memory.pitch/sizeof(NEXUS_Pixel) + i;
        *pixel = 0xFFFFFFFF;
    }
    NEXUS_Surface_Flush(surface);
    return;
}
#if BCMNEXUS_NIR_SUPPORT
/*-------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

/*
** Broadcom specific routines.
*/
#if NEXUS_HAS_INPUT_ROUTER
#define MAX_HID 2
static struct {
    int fd;
    const char *node;
    int major;
    int minor;
} g_hid[MAX_HID] = {
    /* a real app would make this dynamic */
    {-1, "/dev/input/event0", 13, 64},
    {-1, "/dev/input/event1", 13, 65}
/*    {-1, "/dev/input/event2", 13, 66}*/
};

static pthread_t g_hid_thread;

static void *usb_hid_thread(void *context)
{
    while (1) {
        unsigned i;
        int rc;
        struct pollfd fds[MAX_HID];
        for (i=0;i<MAX_HID;i++) {
            if (g_hid[i].fd == -1) goto done; /* exiting */
            fds[i].fd = g_hid[i].fd;
            fds[i].events = POLLIN;
        }
        rc = poll(fds, i, 1000);
        if (rc == -1) break;

        if (!bInputRouterExit)
            BKNI_SetEvent((BKNI_EventHandle)context);
    }
done:
    return NULL;
}

static int usb_hid_init(BKNI_EventHandle event)
{
    unsigned i;
    int rc;

    mkdir("/dev/input", 0700);
    for (i=0;i<MAX_HID;i++) {
        mknod(g_hid[i].node, 0700 | S_IFCHR, makedev(g_hid[i].major,g_hid[i].minor));
        g_hid[i].fd = open(g_hid[i].node, O_NONBLOCK|O_RDONLY);
        if (g_hid[i].fd == -1) {
            BDBG_ERR(("unable to open %s: %d", g_hid[i].node, errno));
            return BERR_TRACE(errno);
        }
    }
    rc = pthread_create(&g_hid_thread, NULL, usb_hid_thread, event);
    if (rc) return -1;

    return 0;
}

static void usb_hid_uninit(void)
{
    unsigned i;
    for (i=0;i<MAX_HID;i++) {
        if (g_hid[i].fd != -1) {
            close(g_hid[i].fd);
            g_hid[i].fd = -1;
        }
    }
    pthread_cancel(g_hid_thread);
}

static int usb_hid_read(struct input_event *input)
{
    unsigned i;
    for (i=0;i<MAX_HID;i++) {
        ssize_t n;
        n = read(g_hid[i].fd, input, sizeof(*input));
        if (!n) continue;
        if (n == -1) {
            if (errno == EWOULDBLOCK) {
                continue;
            }
            else {
                return BERR_TRACE(errno);
            }
        }

        if (n != sizeof(*input)) {
            return BERR_TRACE(-1);
        }
        /* got one event */
        return 0;
    }
    /* nothing */
    return -1;
}
#endif

DFB_PlatformIrInputMode BDFB_Input_Router_lookup_ir_protocol(
    const char *name )
{
    DFB_PlatformIrInputMode inputMode;

    BDBG_MSG(("%s: looking for IR protocol \"%s\"", __FUNCTION__, name));

    inputMode = DFB_PlatformLookupIrInputmode(name);

    if (inputMode >= DFB_PlatformIrInputMode_eMax )
    {
        /* should stop recursive lookup if things are bad */
        if ( strcasecmp(name, DIRECTFB_IR_PROTOCOL ) )
        {
            BDBG_MSG(("nxsmaster: Cannot find IR protocol \"%s\" - defaulting to \"%s\"!", name, DIRECTFB_IR_PROTOCOL));
            inputMode = BDFB_Input_Router_lookup_ir_protocol(DIRECTFB_IR_PROTOCOL);
        }
        else
            BDBG_MSG(("nxsmaster: Cannot find IR protocol"));
    }
    else
        BDBG_MSG(("nxsmaster: %s: found IR protocol \"%s\" [%d]", __FUNCTION__, name, inputMode));

    return inputMode;
}

void nir_callback(void *context, int param)
{
    BSTD_UNUSED(param);
    if (!bInputRouterExit)
        BKNI_SetEvent((BKNI_EventHandle)context);
}

static void * nexus_input_router_masterThread(void* arg)
{
    /*Nexus Input Router code */
    NEXUS_InputRouterHandle router;
    NEXUS_InputClientHandle inputRoutercClient[B_MAX_CLIENTS];
    NEXUS_Error rc;
    unsigned i;
    BKNI_EventHandle nirevent;
    NEXUS_IrInputHandle irInput;
    NEXUS_IrInputSettings irSettings;
#if NEXUS_HAS_KEYPAD
    NEXUS_KeypadHandle keypad;
    NEXUS_KeypadSettings keypadSettings;
#endif
    NEXUS_InputRouterCode mouseCode;

    BSTD_UNUSED(arg);

    BKNI_CreateEvent(&nirevent);

    usb_hid_init(nirevent);

    NEXUS_IrInput_GetDefaultSettings(&irSettings);
    irSettings.mode = DFB_Platform_P_DFBtoNexusIrModeTable[BDFB_Input_Router_lookup_ir_protocol(DIRECTFB_IR_PROTOCOL)];
    BDBG_MSG(("IR Mode: %d ", irSettings.mode));
    irSettings.dataReady.callback = nir_callback;
    irSettings.dataReady.context = nirevent;
    irSettings.repeatFilterTime = DEFAULT_IR_REPEAT_MILLI_SECS;
    irSettings.eventQueueSize = (irSettings.eventQueueSize < NUM_IR_EVENTS) ? NUM_IR_EVENTS : irSettings.eventQueueSize;
    irInput = NEXUS_IrInput_Open(0, &irSettings);
    BDBG_MSG(("nxsmaster: NEXUS_IrInput_Open() Done ! "));
    sleep(1);

#if NEXUS_HAS_KEYPAD
    NEXUS_Keypad_GetDefaultSettings(&keypadSettings);
    keypadSettings.dataReady.callback = nir_callback;
    keypadSettings.dataReady.context = nirevent;
    keypad = NEXUS_Keypad_Open(0, &keypadSettings);
#endif

    router = NEXUS_InputRouter_Create(0);
    BDBG_MSG(("nxsmaster: NEXUS_InputRouter_Create() Done ! "));

    if(!router) { perror("");rc=BERR_TRACE(errno); goto nir_done; }

    for (i=0;i<MAX_NIR_CLIENTS;i++) {
        /* this server grants all codes to all clients. the client can limit what it wants. */
        inputRoutercClient[i] = NEXUS_InputRouter_CreateClient(router, i);

        BDBG_ASSERT(inputRoutercClient[i]);
    }

    /* set up a persistant code for the mouse. this allows handling of incremental changes (REL_X,REL_Y). */
    NEXUS_InputRouter_GetDefaultCode(&mouseCode);
    mouseCode.deviceType = NEXUS_InputRouterDevice_ePointer;
    mouseCode.filterMask = 1<<mouseCode.deviceType;
    mouseCode.data.pointer.x = 0;
    mouseCode.data.pointer.y = 0;
    mouseCode.data.pointer.button = 0;

    BKNI_ResetEvent(nirevent);
    while(bInputRouterExit == false)
    {
        bool wait = true;
        NEXUS_InputRouterCode code;

        if (irInput) {
            NEXUS_IrInputEvent irEvent;
            unsigned n;
            bool overflow;
            rc = NEXUS_IrInput_GetEvents(irInput, &irEvent, 1, &n, &overflow);
            if(overflow) {printf("nxsmaster: IR event overflow\n");}
            if (n == 1) {
                BDBG_MSG(("irinput code=%#x", irEvent.code));
                NEXUS_InputRouter_GetDefaultCode(&code);
                code.deviceType = NEXUS_InputRouterDevice_eIrInput;
                code.filterMask = 1<<code.deviceType;
                code.data.irInput.index = 0;
                code.data.irInput.code = irEvent.code;
                code.data.irInput.repeat = irEvent.repeat;
                NEXUS_InputRouter_SendCode(router, &code);
                wait = false;
            }
        }

#if  NEXUS_HAS_KEYPAD
        if (keypad) {
            NEXUS_KeypadEvent keypadEvent;
            unsigned n;
            bool overflow;
            rc = NEXUS_Keypad_GetEvents(keypad, &keypadEvent, 1, &n, &overflow);
            BDBG_ASSERT(!rc);
            if (n == 1) {
                BDBG_MSG(("keypad code=%#x", keypadEvent.code));
                NEXUS_InputRouter_GetDefaultCode(&code);
                code.deviceType = NEXUS_InputRouterDevice_eKeypad;
                code.filterMask = 1<<code.deviceType;
                code.data.keypad.index = 0;
                code.data.keypad.code = keypadEvent.code;
                code.data.keypad.repeat = keypadEvent.repeat;
                NEXUS_InputRouter_SendCode(router, &code);
                wait = false;
            }
        }
#endif

        {
            struct input_event input;
            NEXUS_InputRouter_GetDefaultCode(&code);
            while (1) { /* loop until empty */
                rc = usb_hid_read(&input);
                if (rc) break;
                BDBG_MSG(("input_event: time=%#x type=%#x code=%#x value=%#x", input.time, input.type, input.code, input.value));
                switch (input.type) {
                case EV_REL:
                    switch (input.code) {
                        case REL_X: mouseCode.data.pointer.x += input.value; break;
                        case REL_Y: mouseCode.data.pointer.y += input.value; break;
                    }
                    NEXUS_InputRouter_SendCode(router, &mouseCode);
                    break;
                case EV_ABS:
                    switch (input.code) {
                        case ABS_X: mouseCode.data.pointer.x = input.value; break;
                        case ABS_Y: mouseCode.data.pointer.y = input.value; break;
                    }
                    NEXUS_InputRouter_SendCode(router, &mouseCode);
                    break;
                case EV_KEY:
                    switch (input.code) {
                        case BTN_LEFT:
                            mouseCode.data.pointer.button = input.value << 0;
                            NEXUS_InputRouter_SendCode(router, &mouseCode);
                            break;
                        case BTN_RIGHT:
                            mouseCode.data.pointer.button = input.value << 1;
                            NEXUS_InputRouter_SendCode(router, &mouseCode);
                            break;
                        case BTN_MIDDLE:
                            mouseCode.data.pointer.button = input.value << 2;
                            NEXUS_InputRouter_SendCode(router, &mouseCode);
                            break;
                        default:
                            code.deviceType = NEXUS_InputRouterDevice_eKeyboard;
                            code.filterMask = 1<<code.deviceType;
                            code.data.keyboard.code = input.code;
                            code.data.keyboard.down = input.value;
                            NEXUS_InputRouter_SendCode(router, &code);
                            break;
                    }
                    break;
                }
                wait = false;
            }
        }
        if (wait) {
        //rc= BKNI_WaitForEvent(nirevent, BKNI_INFINITE);
        rc= BKNI_WaitForEvent(nirevent, 500);
        if (BERR_TIMEOUT == rc) {continue;}
        }
    }


nir_done:

    for (i=0;i<MAX_NIR_CLIENTS;i++) {
        NEXUS_InputRouter_DestroyClient(inputRoutercClient[i]);
    }
    NEXUS_InputRouter_Destroy(router);

    NEXUS_IrInput_Close(irInput);
    BKNI_DestroyEvent(nirevent);
    usb_hid_uninit();

    return 0 ;
}

/*-------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
#endif /*BCMNEXUS_NIR_SUPPORT*/

static bool quit = false;
static pthread_mutex_t signalMutex;
static pthread_cond_t signalCond;

void signalHandler(int sig)
{
    BSTD_UNUSED(sig);
    pthread_mutex_lock(&signalMutex);
    quit = true;
    pthread_cond_signal(&signalCond);
    pthread_mutex_unlock(&signalMutex);
}

int main(int argc, char **argv)
{
    unsigned i;
    NEXUS_PlatformStartServerSettings serverSettings;
    NEXUS_Error rc;
    int curarg = 1;
    bipc_server_create_settings ipc_settings;
    NEXUS_SurfaceCompositorSettings surface_compositor_settings;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_PlatformSettings platformSettings;
    pthread_t ipc_thread_id;
#if BCMNEXUS_NIR_SUPPORT
    pthread_t nir_thread_id;
#endif
    NEXUS_VideoOrientation  orientation = NEXUS_VideoOrientation_e2D;
    bool native_3d = false;
    NEXUS_SurfaceClientHandle clientDFB[5] ;
    NEXUS_SurfaceCompositorClientSettings client_settings;

    quit = false;
    pthread_cond_init(&signalCond,NULL);
    pthread_mutex_init(&signalMutex,NULL);

    memset(&server, 0, sizeof(server));
    server.options.move = true;
    server.options.hdsd_simul = false;
    server.options.cursor = false;
    server.options.framebuffers = 8;    /* Default to 8 to allow 'tunnelled' mode to work */
    server.options.tunnel_client = NULL;
    server.decoder_state.display = &server.display_state;

    while (curarg < argc) {
        if (!strcmp(argv[curarg], "--help") || !strcmp(argv[curarg], "-h")) {
            print_usage(argv[0]);
            return 0;
        }
        else if (!strcmp(argv[curarg], "-move") && curarg+1<argc) {
            curarg++;
            if (!strcmp(argv[curarg], "off")) {
                server.options.move = false;
            }
        }
        else if (!strcmp(argv[curarg], "-sd") && curarg+1<argc) {
            curarg++;
            if (!strcmp(argv[curarg], "off")) {
                server.options.hdsd_simul = false;
            }
        }
        else if (!strcmp(argv[curarg], "-cursor") && curarg+1<argc) {
            curarg++;
            if (strcmp(argv[curarg], "on")==0) {
                server.options.cursor = true;
            }
        } else if(!strcmp(argv[curarg], "-framebuffers") && curarg+1<argc) {
            server.options.framebuffers = atoi(argv[++curarg]);
        } else if(!strcmp(argv[curarg], "-tunnel") && curarg+1<argc) {
#if BCMNEXUS_NSC_SUPPORT == BCMNEXUS_NSC_TUNNELLED_MODE
            server.options.tunnel_client = argv[++curarg];
#else
            fprintf(stderr,"Tunnelled mode not supported in this build\n");
            return -1;
#endif
        }
        else if (!strcmp(argv[curarg], "-3d") && curarg+1<argc) {
            curarg++;
            if (strcmp(argv[curarg], "on")==0 || strcmp(argv[curarg], "lr")==0) {
                orientation = NEXUS_VideoOrientation_e3D_LeftRight;
            } else if(strcmp(argv[curarg], "ou")==0) {
                orientation = NEXUS_VideoOrientation_e3D_OverUnder;
            }
        }
        else if (!strcmp(argv[curarg], "-native_3d") && curarg+1<argc) {
            curarg++;
            if (strcmp(argv[curarg], "on")==0) {
                native_3d = true;
                if(orientation == NEXUS_VideoOrientation_e2D) {
                    orientation = NEXUS_VideoOrientation_e3D_LeftRight;
                }
            }
        } else {
            fprintf(stderr,"invalud argument %s\n", argv[curarg]);
            print_usage(argv[0]);
            return -1;
        }
        curarg++;
    }

    NEXUS_Platform_GetDefaultSettings(&platformSettings);

    platformSettings.openFrontend = false;

    /* Provide the DirectFB clients with a 4MB dedicated heap.
     *
     * This heap needs to have the same properties as the main display heap,
     * but for security reasons is separated. The DirectFB applications are
     * treated as untrusted slave apps, so we don't let them have access to the main
     * display heap where a malicious application could cause damage to the running
     * system or corrupt the display.
     *
     * The index NEXUS_MAX_HEAPS-1 is chosen to avoid conflicts with default heaps on all chips .
     *
     * The heap is used for packet buffer graphics buffers (256K per client) & SID driver (1.5MB).
     */

    platformSettings.heap[NEXUS_MAX_HEAPS-1].size = 4 * 1024 * 1024;
    rc = NEXUS_Platform_Init(&platformSettings);
    if (rc) return -1;


    NEXUS_Platform_GetConfiguration(&server.platformConfig);

    /* open HD/SD smilu display */
    NEXUS_Display_GetDefaultSettings(&displaySettings);
    displaySettings.format = NEXUS_VideoFormat_e720p;
    if(native_3d) {
        displaySettings.display3DSettings.overrideOrientation = true;
        displaySettings.display3DSettings.orientation = orientation;
    }
    server.display_state.display[0].display = NEXUS_Display_Open(0, &displaySettings);
    server.display_state.display[0].window[0] = NEXUS_VideoWindow_Open(server.display_state.display[0].display, 0);
    NEXUS_Display_GetDefaultSettings(&displaySettings);

#if NEXUS_NUM_DISPLAYS > 1
    if (server.options.hdsd_simul) {
        displaySettings.format = NEXUS_VideoFormat_eNtsc;
        server.display_state.display[1].display = NEXUS_Display_Open(1, &displaySettings);
    }
    if (server.display_state.display[1].display) {
        server.display_state.display[1].window[0] = NEXUS_VideoWindow_Open(server.display_state.display[1].display, 0);
    }
#endif

#if NEXUS_NUM_COMPONENT_OUTPUTS
    NEXUS_Display_AddOutput(server.display_state.display[0].display, NEXUS_ComponentOutput_GetConnector(server.platformConfig.outputs.component[0]));
#endif
#if NEXUS_NUM_HDMI_OUTPUTS
    NEXUS_Display_AddOutput(server.display_state.display[0].display, NEXUS_HdmiOutput_GetVideoConnector(server.platformConfig.outputs.hdmi[0]));
#endif
#if NEXUS_NUM_COMPOSITE_OUTPUTS
    if(server.display_state.display[1].display) {
        NEXUS_Display_AddOutput(server.display_state.display[1].display, NEXUS_CompositeOutput_GetConnector(server.platformConfig.outputs.composite[0]));
    }
#endif

    BKNI_CreateMutex(&server.lock);
    refserver_simpledecoder_init(&server.decoder_state);

    {
        int i;
        NEXUS_MemoryStatus      status;

        for (i = 0 ; i < NEXUS_MAX_HEAPS; ++i)
        {
            if (server.platformConfig.heap[i] == NULL)
                continue;

            if (NEXUS_Heap_GetStatus(server.platformConfig.heap[i],&status)  == NEXUS_SUCCESS)
            {
                printf("Heap[%d]: handle=%p, memcIndex=%d, range=<%p-%p>, size=%d, offset=0x%08x, type=%d\n",
                       i, (void *)server.platformConfig.heap[i], status.memcIndex, status.addr, (void *)((uint32_t)status.addr+status.size-1),
                       status.size, status.offset, status.memoryType);
            }

        }
    }

    NEXUS_Platform_GetDefaultStartServerSettings(&serverSettings);

#if BCMNEXUS_NSC_SUPPORT == BCMNEXUS_NSC_TUNNELLED_MODE
    serverSettings.allowUnauthenticatedClients = true; /* client is written this way */
    serverSettings.unauthenticatedConfiguration.mode = NEXUS_ClientMode_eProtected;

    serverSettings.unauthenticatedConfiguration.heap[0] =  server.platformConfig.heap[0];
    serverSettings.unauthenticatedConfiguration.heap[1] =  server.platformConfig.heap[1];
    serverSettings.unauthenticatedConfiguration.heap[2] =  server.platformConfig.heap[2];
    serverSettings.unauthenticatedConfiguration.heap[3] =  server.platformConfig.heap[3];
    serverSettings.unauthenticatedConfiguration.heap[4] =  server.platformConfig.heap[4];
    serverSettings.unauthenticatedConfiguration.heap[5] =  server.platformConfig.heap[5];
    serverSettings.unauthenticatedConfiguration.heap[6] =  server.platformConfig.heap[6];
    serverSettings.unauthenticatedConfiguration.heap[7] =  server.platformConfig.heap[7];
#else
    /* Multi buffered client mode */
    serverSettings.allowUnauthenticatedClients = true; /* client is written this way */
    serverSettings.unauthenticatedConfiguration.mode = NEXUS_ClientMode_eProtected;
     // not working yet serverSettings.unauthenticatedConfiguration.mode = NEXUS_ClientMode_eUntrusted;
    serverSettings.unauthenticatedConfiguration.heap[0] =  NEXUS_Platform_GetFramebufferHeap(NEXUS_OFFSCREEN_SURFACE);
    serverSettings.unauthenticatedConfiguration.heap[NEXUS_MAX_HEAPS-1] = server.platformConfig.heap[NEXUS_MAX_HEAPS-1];
    printf("Unauthenticated heap handle=%p\n", (void *)serverSettings.unauthenticatedConfiguration.heap[0]);
#endif

    rc = NEXUS_Platform_StartServer(&serverSettings);
    BDBG_ASSERT(!rc);

    BKNI_CreateEvent(&server.inactiveEvent);

    server.surfaceCompositor = NEXUS_SurfaceCompositor_Create(0);
    NEXUS_SurfaceCompositor_GetSettings(server.surfaceCompositor, &surface_compositor_settings);

    for (i = 0; i < sizeof(clientDFB) / sizeof(clientDFB[0]); i++)
    {
        clientDFB[i] = NEXUS_SurfaceCompositor_CreateClient(server.surfaceCompositor, i);

        NEXUS_SurfaceCompositor_GetClientSettings(server.surfaceCompositor, clientDFB[i], &client_settings);

        client_settings.tunnelCapable = false;
        client_settings.composition.zorder = i;
        client_settings.composition.visible = true;
        client_settings.composition.position.x = 0;
        client_settings.composition.position.y = 0;
        client_settings.composition.position.width = 1280;
        client_settings.composition.position.height = 720;
        client_settings.composition.virtualDisplay.width = 1280;
        client_settings.composition.virtualDisplay.height = 720;

        switch (i)
        {
            case 1: /* dfb_clientid=1 */
                client_settings.composition.position.x = 0;
                client_settings.composition.position.y = 0;
                client_settings.composition.position.width = 1280/2;
                client_settings.composition.position.height = 720/2;
                break;
            case 2: /* dfb_clientid=2 */
                client_settings.composition.position.x = 1280/2;
                client_settings.composition.position.y = 0;
                client_settings.composition.position.width = 1280/2;
                client_settings.composition.position.height = 720/2;
                break;
            case 3: /* dfb_clientid=3 */
                client_settings.composition.position.x = 0;
                client_settings.composition.position.y = 720/2;
                client_settings.composition.position.width = 1280/2;
                client_settings.composition.position.height = 720/2;
                break;
            case 4: /* dfb_clientid=4 */
                client_settings.composition.position.x = 1280/2;
                client_settings.composition.position.y = 720/2;
                client_settings.composition.position.width = 1280/2;
                client_settings.composition.position.height = 720/2;
                break;
            case 0: /* dfb_clientid=0 */
            default:
                client_settings.tunnelCapable = true;
                break;
        }

        NEXUS_SurfaceCompositor_SetClientSettings(server.surfaceCompositor, clientDFB[i], &client_settings);
    }

    NEXUS_Display_GetGraphicsSettings(server.display_state.display[0].display, &surface_compositor_settings.display[0].graphicsSettings);
    surface_compositor_settings.display[0].graphicsSettings.enabled = true;
    surface_compositor_settings.display[0].display = server.display_state.display[0].display;
    surface_compositor_settings.display[0].framebuffer.width = 1280;
    surface_compositor_settings.display[0].framebuffer.height = 720;
    surface_compositor_settings.display[0].framebuffer.backgroundColor = 0xFF000000; /* black background */
    surface_compositor_settings.display[0].framebuffer.heap = NEXUS_Platform_GetFramebufferHeap(0);
    surface_compositor_settings.inactiveCallback.callback = complete;
    surface_compositor_settings.inactiveCallback.context = server.inactiveEvent;
    if(server.options.framebuffers) {
        surface_compositor_settings.display[0].framebuffer.number = server.options.framebuffers;
    }

    if(!native_3d && orientation!=NEXUS_VideoOrientation_e2D) {
        surface_compositor_settings.display[0].display3DSettings.overrideOrientation = true;
        surface_compositor_settings.display[0].display3DSettings.orientation = orientation;
    }

    if (server.display_state.display[1].display) {
        NEXUS_Display_GetGraphicsSettings(server.display_state.display[1].display, &surface_compositor_settings.display[1].graphicsSettings);
        surface_compositor_settings.display[1].graphicsSettings.enabled = true;
        surface_compositor_settings.display[1].display = server.display_state.display[1].display;
        surface_compositor_settings.display[1].framebuffer.width = 720;
        surface_compositor_settings.display[1].framebuffer.height = 480;
        surface_compositor_settings.display[1].framebuffer.backgroundColor = 0xFF000000; /* black background */
        surface_compositor_settings.display[1].framebuffer.heap = NEXUS_Platform_GetFramebufferHeap(1);
    }

    b_set_windows(&server, true, &surface_compositor_settings);

    surface_compositor_settings.frameBufferCallback.callback = framebuffer_callback;
    surface_compositor_settings.frameBufferCallback.context = &server;
    rc = NEXUS_SurfaceCompositor_SetSettings(server.surfaceCompositor, &surface_compositor_settings);
    BDBG_ASSERT(!rc);
    if(server.options.cursor) {
        NEXUS_SurfaceCreateSettings settings;
        NEXUS_SurfaceCursorCreateSettings cursorSettings;
        NEXUS_SurfaceCursorSettings config;

        NEXUS_Surface_GetDefaultCreateSettings(&settings);

        settings.width = 8;
        settings.height = 8;
        server.cursor.surface = NEXUS_Surface_Create(&settings);
        BDBG_ASSERT(server.cursor.surface);
        make_cursor(server.cursor.surface, &settings);
        NEXUS_SurfaceCursor_GetDefaultCreateSettings(&cursorSettings);

        cursorSettings.surface = server.cursor.surface;
        server.cursor.cursor = NEXUS_SurfaceCursor_Create(server.surfaceCompositor, &cursorSettings);
        BDBG_ASSERT(server.cursor.cursor);
        NEXUS_SurfaceCursor_GetSettings (server.cursor.cursor, &config);
        config.composition.visible = true;
        config.composition.virtualDisplay.width  = 400;
        config.composition.virtualDisplay.height = 200;
        config.composition.position.x = 100;
        config.composition.position.y = 100;
        config.composition.position.width = 8;
        config.composition.position.height = 8;
        NEXUS_SurfaceCursor_SetSettings (server.cursor.cursor, &config);
    }

    bipc_server_get_default_create_settings(&ipc_settings);
    ipc_settings.interfaces = ipc_interfaces;
    ipc_settings.interface_count = sizeof(ipc_interfaces)/sizeof(*ipc_interfaces);

    BLST_D_INIT(&server.clients);
    server.ipc.exit = false;
    server.ipc.server = bipc_server_create(&ipc_settings);
    BDBG_ASSERT(server.ipc.server);

    signal(SIGPIPE, SIG_IGN);
    printf("Press CTRL-C to shutdown server\n");
    if(pthread_create(&ipc_thread_id, NULL, ipc_thread, &server)) {
        BDBG_ASSERT(0);
    }

#if BCMNEXUS_NIR_SUPPORT
    if(pthread_create(&nir_thread_id, NULL, nexus_input_router_masterThread, NULL)) {
        BDBG_ASSERT(0);
    }
#endif

    (void) signal(SIGINT, signalHandler);

    pthread_mutex_lock(&signalMutex);
    while (!quit)
    {
        pthread_cond_wait(&signalCond, &signalMutex);
    }
    pthread_mutex_unlock(&signalMutex);

    for (i = 0; i < sizeof(clientDFB) / sizeof(clientDFB[0]); i++)
        NEXUS_SurfaceCompositor_DestroyClient(clientDFB[i]);

#if BCMNEXUS_NIR_SUPPORT
    bInputRouterExit = true;

    pthread_join(nir_thread_id, NULL);
#endif

    server.ipc.exit = true;
    pthread_join(ipc_thread_id, NULL);
    bipc_server_destroy(server.ipc.server);
    NEXUS_Platform_StopServer();
    BKNI_DestroyMutex(server.lock);

    if(server.options.cursor) {
        NEXUS_SurfaceCursor_Destroy(server.cursor.cursor);
        NEXUS_Surface_Destroy(server.cursor.surface);
    }

    /* stop the server before closing resources that may be in use by clients.
    if it's an untrusted client, handle verification may fail the call. but a trusted client bypasses the
    check and could kill the server. */
    NEXUS_SurfaceCompositor_Destroy(server.surfaceCompositor);
    BKNI_DestroyEvent(server.inactiveEvent);
    refserver_simpledecoder_uninit(&server.decoder_state);
    NEXUS_VideoWindow_Close(server.display_state.display[0].window[0]);
    NEXUS_Display_Close(server.display_state.display[0].display);
    if (server.display_state.display[1].display) {
        NEXUS_VideoWindow_Close(server.display_state.display[1].window[0]);
        NEXUS_Display_Close(server.display_state.display[1].display);
    }


    NEXUS_Platform_Uninit();
    return 0;
}
#else
int main(void)
{
    printf("ERROR: NEXUS_SurfaceCompositor  not supported\n");
    return -1;
}
#endif


