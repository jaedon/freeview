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
 * $brcm_Workfile: refsw_server.c $
 * $brcm_Revision: 44 $
 * $brcm_Date: 10/16/12 5:21p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/examples/multiprocess/refsw_server.c $
 * 
 * 44   10/16/12 5:21p erickson
 * SW7425-1792: add client hdmi input, protect with
 *  NEXUS_Platform_AcquireObject
 * 
 * 43   9/13/12 3:10p erickson
 * SW7435-354: call NEXUS_Platform_UnregisterClient before
 *  NEXUS_SurfaceCompositor_DestroyClient to avoid full-screen flash of
 *  video
 * 
 * 42   8/24/12 4:02p erickson
 * SW7429-252: don't WaitForEvent in app ipc. this creates a deadlock race
 *  condition with graphics2d. instead, advance state in inactive
 *  callback.
 * 
 * 41   8/10/12 10:25a erickson
 * SW7425-2734: rework examples to use NEXUS_ClientResources for decoder
 *  ids
 * 
 * 40   8/2/12 10:09a erickson
 * SW7429-227: add -frontend to --help output
 * 
 * 39   8/1/12 4:00p erickson
 * SW7425-2734: allow server to limit resources for untrusted clients
 * 
 * 38   7/9/12 1:57p erickson
 * SW7231-844: allow server to pick default NEXUS_MemoryType_eFull heap
 * 
 * 37   7/9/12 8:46a jgarrett
 * SW7231-872: Merge to main branch
 * 
 * SW7231-872/4   7/2/12 2:17p marcusk
 * SW7231-872: Added debug output and fixed HDMI initialization
 * 
 * SW7231-872/3   7/2/12 11:37a marcusk
 * SW7231-872: update with a few fixes and improvements.  Supports multi-
 *  channel PCM output over HDMI
 * 
 * 36   6/27/12 3:15p jgarrett
 * SW7231-872: Merge to main branch
 * 
 * SW7231-872/2   6/26/12 4:48p marcusk
 * SW7231-872: Fixed some minor bugs and added better defaults for HDMI
 * 
 * SW7231-872/1   6/25/12 4:26p marcusk
 * SW7231-872: Default behavior is to passthrough AC3 and DTS streams to
 *  SPDIF output.
 * 
 * 35   6/13/12 3:07p erickson
 * SW7425-2981: refactor into b_simpledecoder wrapper library
 * 
 * 34   6/6/12 11:41a erickson
 * SW7425-2981: rework simple audio decoder input[] configuration
 * 
 * 33   5/15/12 5:05p erickson
 * SW7231-786: fix to previous checkin
 * 
 * 32   5/11/12 4:07p erickson
 * SW7231-786: add -mixed_hdmi and -mixed_spdif options
 * 
 * 31   5/2/12 5:08p erickson
 * SW7425-2981: fix to previous checkin
 * 
 * 30   5/2/12 4:16p erickson
 * SW7425-2981: add general hdmi/spdif configurability based on app or
 *  hdmi edid
 *
 * 29   4/20/12 10:08a erickson
 * SW7425-2882: create fully mapped heap and notify using app ipc
 *
 * 28   1/26/12 5:38p erickson
 * SW7425-2116: add colorMatrix fade for client 0
 *
 * 27   1/4/12 11:49a erickson
 * SW7420-1690: use chmod instead of umask to avoid race conditions in
 *  multithreaded system
 *
 * 26   12/19/11 10:09a erickson
 * SW7425-1795: use eFull heap for graphics2d packet fifo or playpump fifo
 *
 * 25   11/29/11 11:39a erickson
 * SW7420-1148: add -mode option
 *
 * 24   11/18/11 1:24p erickson
 * SW7420-1148: add -timeout for automation
 *
 * 23   11/18/11 10:19a erickson
 * SW7422-445: use
 *  NEXUS_Platform_GetFramebufferHeap(NEXUS_OFFSCREEN_SURFACE) for client
 *  heap so that server code will work across a variety of platforms
 *
 * 22   11/17/11 12:51p erickson
 * SW7425-1364: support 1080i output
 *
 * 21   11/11/11 4:43p erickson
 * SW7420-1992: refactor video client code to not require compositor
 *  shutdown
 *
 * 20   11/11/11 3:53p erickson
 * SW7420-1135: fix shutdown
 *
 * 19   11/11/11 11:28a erickson
 * SW7420-1135: add regular decoder and mosaic support
 *
 * 18   11/10/11 2:18p erickson
 * SW7420-1135: add regular decoder support to cmd_client and refsw_server
 *
 * 17   11/7/11 2:34p erickson
 * SW7420-1992: don't make surfacecmp inactive until tunneled surfaces are
 *  released
 *
 * 16   11/7/11 11:02a vsilyaev
 * SW7420-2119: Refactored tunnel mode support
 *
 * 15   11/1/11 3:25p erickson
 * SW7420-1690: set permissions to allow non-root users
 *
 * 14   11/1/11 1:46p vsilyaev
 * SW7420-2114: Added support for native 3d
 *
 * 13   10/28/11 1:33p vsilyaev
 * SW7420-2110: Use better blend equation for cursor rendering
 *
 * 12   10/27/11 2:37p erickson
 * SW7425-1364: rename to brc
 *
 * 11   10/26/11 4:01p erickson
 * SW7425-1364: extend app ipc
 *
 * 10   10/24/11 5:43p vsilyaev
 * SW7420-1992: Updated cursor support
 *
 * 9   10/20/11 2:13p vsilyaev
 * SW7420-1992: Added 3D compositing
 *
 * 8   10/20/11 1:56p erickson
 * SW7420-1992: embed NEXUS_VideoWindowSettings in
 *  NEXUS_SurfaceCompositorDisplaySettings
 *
 * 7   10/18/11 12:32p vsilyaev
 * SW7420-1992: Moved virtual display into the composition settings
 *
 * 6   10/13/11 4:54p vsilyaev
 * SW7420-1992: Fixed shutdown sequemce for video window
 *
 * 5   10/13/11 2:15p vsilyaev
 * SW7425-1364: Added support for simple decoder
 *
 * 4   10/12/11 5:19p vsilyaev
 * SW7425-1364: Added client initiated movement support
 *
 * 3   10/5/11 2:05p vsilyaev
 * SW7420-1992: Improved shutdown sequence
 *
 * 2   10/5/11 11:24a vsilyaev
 * SW7425-1364: Added reference server
 *
 * 1   10/4/11 5:44p vsilyaev
 * SW7425-1364: Reference applicaion IPC and reference server
 *
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
#include "b_simpledecoder.h"
#include "nexus_stc_channel.h"
#include "bipc_server.h"
#include "brc_client.h"
#include "nexus_surface_compositor.h"
#include "nexus_surface_cursor.h"
#include "nexus_input_router.h"
#include "blst_list.h"
#include "nexus_core_utils.h"
#include "../../utils/namevalue.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/poll.h>

BDBG_MODULE(refsw_server);

#include "ipc_stubs_server.h"
#include "refsw_session.h"
#include "../../utils/namevalue.c" /* TEMP */

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
        NEXUS_VideoWindowHandle mosaic_window[NEXUS_SURFACE_COMPOSITOR_VIDEO_WINDOWS];
    } display[NEXUS_NUM_DISPLAYS];
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
        bool ignore_edid;
    } options;
    struct display_state display_state;
    
    struct {
        b_simpledecoder_resources resources;
        b_simpledecoder_config config;
        b_simpledecoder_state state;
        bool mosaic_enabled;
    } simpledecoder;

    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_SurfaceCompositorHandle surfaceCompositor;
    BKNI_EventHandle inactiveEvent;
    struct {
        bool pending;
        NEXUS_VideoFormat format;
    } displayFormatChange;

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

    NEXUS_ClientMode defaultClientMode;
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
        bool fade;
        uint8_t alpha;
    } movement;
    
    NEXUS_VideoDecoderHandle videoDecoder; /* from brc_client_add_video_decoder_input */
    NEXUS_HdmiInputHandle hdmiInput; /* from brc_client_add_hdmi_input */
    bool request_decode;
} *b_refsw_client_t;

static int refserver_simpledecoder_set_decoder(b_refsw_server_t server, bool enabled);
static void b_set_windows(b_refsw_server_t server, bool enabled, NEXUS_SurfaceCompositorSettings *settings);
static void b_open_mosaic_windows(b_refsw_server_t server, bool enabled);

static b_refsw_client_t b_refsw_client_create(b_refsw_server_t server)
{
    b_refsw_client_t client;

    client = BKNI_Malloc(sizeof(*client));
    if(client==NULL) {(void)BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);goto err_alloc;}
    BKNI_Memset(client, 0, sizeof(*client));
    BDBG_OBJECT_SET(client, b_refsw_client);
    BLST_D_INSERT_HEAD(&server->clients, client, link);
    client->server = server;
    return client;

err_alloc:
    return NULL;
}

struct {
    bool set;
    NEXUS_InterfaceName videoDecoderInterface;
    NEXUS_InterfaceName hdmiInputInterface;
} g_interface;

static void b_init_interfaces(void)
{
    if (!g_interface.set) {
        NEXUS_Platform_GetDefaultInterfaceName(&g_interface.videoDecoderInterface);
        strcpy(g_interface.videoDecoderInterface.name, "NEXUS_VideoDecoder");
        NEXUS_Platform_GetDefaultInterfaceName(&g_interface.hdmiInputInterface);
        strcpy(g_interface.hdmiInputInterface.name, "NEXUS_HdmiInput");
        g_interface.set = true;
    }
}

static void b_refsw_release_inputs(b_refsw_client_t client)
{
    if (client->videoDecoder) {
        NEXUS_Platform_ReleaseObject(&g_interface.videoDecoderInterface, client->videoDecoder);
        client->videoDecoder = NULL;
    }
    if (client->hdmiInput) {
        NEXUS_Platform_ReleaseObject(&g_interface.hdmiInputInterface, client->hdmiInput);
        client->hdmiInput = NULL;
    }
}

static void b_refsw_client_shutdown(b_refsw_client_t client)
{
    /* unregister client first so that decoder is stopped before destroying surfacecmp client to avoid flash of video */
    if(client->ipc.nexusClient) {
        NEXUS_Platform_UnregisterClient(client->ipc.nexusClient);
        client->ipc.nexusClient = NULL;
    }
    if(client->resources.screen) {
        NEXUS_SurfaceCompositor_DestroyClient(client->resources.screen);
        client->resources.screen = NULL;
    }
    b_refsw_release_inputs(client);
    
    if (client->request_decode) {
        client->request_decode = false;
        (void)refserver_simpledecoder_set_decoder(client->server, true);
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
            client->movement.fade = true;
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
        client->info.audioDecoderId = server->simpledecoder.state.id.simpleAudioDecoder;
    }
    if(config->resources.videoDecoder) {
        client->info.videoDecoderId = server->simpledecoder.state.id.simpleVideoDecoder[0];
    }
    NEXUS_Platform_GetDefaultClientSettings(&clientSettings);
    clientSettings.authentication.certificate = client->info.certificate;
    clientSettings.configuration.heap[0] = NEXUS_Platform_GetFramebufferHeap(NEXUS_OFFSCREEN_SURFACE); /* default heap for client. VC4 usable, eApplication mapping. */
    if (tunnelCapable) {
        clientSettings.configuration.heap[1] = NEXUS_Platform_GetFramebufferHeap(0);
    }
    clientSettings.configuration.heap[NEXUS_MAX_HEAPS-1] = server->platformConfig.heap[NEXUS_MAX_HEAPS-1]; /* for packet blit and playpump */
    clientSettings.configuration.mode = server->defaultClientMode;
    clientSettings.configuration.resources.surfaceClient.id[0] = client->info.surfaceClientId;
    clientSettings.configuration.resources.surfaceClient.total = 1;
    clientSettings.configuration.resources.simpleVideoDecoder.id[0] = client->info.audioDecoderId;
    clientSettings.configuration.resources.simpleVideoDecoder.total = 1;
    clientSettings.configuration.resources.simpleAudioDecoder.id[0] = client->info.videoDecoderId;
    clientSettings.configuration.resources.simpleAudioDecoder.total = 1;
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

static NEXUS_VideoWindowHandle b_get_window(b_refsw_server_t server, unsigned display_index, unsigned window_index)
{
    if (server->display_state.display[display_index].mosaic_window[window_index]) {
        return server->display_state.display[display_index].mosaic_window[window_index];
    }
    else if (window_index<NEXUS_NUM_VIDEO_WINDOWS) {
        return server->display_state.display[display_index].window[window_index];
    }
    else {
        return NULL;
    }
}

static void b_set_windows(b_refsw_server_t server, bool enabled, NEXUS_SurfaceCompositorSettings *settings)
{
    unsigned i, j;
    for (j=0;j<NEXUS_NUM_DISPLAYS;j++) {
        if (enabled) {
            for (i=0;i<NEXUS_SURFACE_COMPOSITOR_VIDEO_WINDOWS;i++) {
                NEXUS_VideoWindowHandle window = b_get_window(server, j, i);
                settings->display[j].window[i].window = window;
                BDBG_MSG(("b_set_windows %d %d: %p", j, i, window));
                if (window) {
                    NEXUS_VideoWindow_GetSettings(window, &settings->display[j].window[i].settings);
                }
            }
        }
        else {
            /* bring main window to full screen */
            NEXUS_DisplaySettings displaySettings;
            NEXUS_VideoWindowSettings windowSettings;
            NEXUS_VideoFormatInfo info;
            
            BKNI_Memset(settings->display[j].window, 0, sizeof(settings->display[j].window));
        
            NEXUS_Display_GetSettings(server->display_state.display[i].display, &displaySettings);
            NEXUS_VideoFormat_GetInfo(displaySettings.format, &info);
        
            NEXUS_VideoWindow_GetSettings(server->display_state.display[i].window[0], &windowSettings);
            windowSettings.position.x = 0;
            windowSettings.position.y = 0;
            windowSettings.position.width = info.width;
            windowSettings.position.height = info.height;
            windowSettings.visible = true;
            windowSettings.clipBase = windowSettings.position;
            windowSettings.clipRect = windowSettings.position;
            (void)NEXUS_VideoWindow_SetSettings(server->display_state.display[i].window[0], &windowSettings);
        }
    }
}

static void b_open_mosaic_windows(b_refsw_server_t server, bool enabled)
{
    unsigned i, j;
    for (j=0;j<NEXUS_NUM_DISPLAYS;j++) {
        for (i=0;i<NEXUS_SURFACE_COMPOSITOR_VIDEO_WINDOWS;i++) {
            if (server->display_state.display[j].mosaic_window[i] && !enabled) {
                NEXUS_VideoWindow_Close(server->display_state.display[j].mosaic_window[i]);
                server->display_state.display[j].mosaic_window[i] = NULL;
                BDBG_MSG(("close mosaic window %d %d", j, i));
            }
            else if (!server->display_state.display[j].mosaic_window[i] && enabled && server->display_state.display[j].window[0]) {
                /* only do mosaic from .window[0] */
                server->display_state.display[j].mosaic_window[i] = NEXUS_VideoWindow_OpenMosaic(server->display_state.display[j].window[0], i);
                BDBG_MSG(("open mosaic window %d %d: %p", j, i, server->display_state.display[j].mosaic_window[i]));
            }
        }
    }
}

int   brc_client_set_display_format(brc_client_t _client, NEXUS_VideoFormat format)
{
    NEXUS_SurfaceCompositorSettings surface_compositor_settings;
    int rc;
    b_refsw_client_t client = _client->parent;

    BDBG_WRN(("brc_client_set_display_format"));
    BKNI_ResetEvent(client->server->inactiveEvent);
    client->server->displayFormatChange.pending = true;
    client->server->displayFormatChange.format = format;

    NEXUS_SurfaceCompositor_GetSettings(client->server->surfaceCompositor, &surface_compositor_settings);
    surface_compositor_settings.enabled = false;
    rc = NEXUS_SurfaceCompositor_SetSettings(client->server->surfaceCompositor, &surface_compositor_settings);
    if (rc) return BERR_TRACE(rc);
    
    return 0;
}

static int b_refsw_display_format_change(b_refsw_server_t server)
{
    NEXUS_DisplayHandle display;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_SurfaceCompositorSettings surface_compositor_settings;
    int rc;
    NEXUS_VideoFormatInfo formatInfo;
    
    display = server->display_state.display[0].display;
    NEXUS_Display_GetSettings(display, &displaySettings);
    displaySettings.format = server->displayFormatChange.format;
    rc = NEXUS_Display_SetSettings(display, &displaySettings);
    if (rc) return BERR_TRACE(rc);

    NEXUS_VideoFormat_GetInfo(server->displayFormatChange.format, &formatInfo);

    NEXUS_SurfaceCompositor_GetSettings(server->surfaceCompositor, &surface_compositor_settings);
    surface_compositor_settings.enabled = true;
    NEXUS_Display_GetGraphicsSettings(display, &surface_compositor_settings.display[0].graphicsSettings);
    surface_compositor_settings.display[0].graphicsSettings.clip.width = formatInfo.width;
    surface_compositor_settings.display[0].graphicsSettings.clip.height = formatInfo.height;
    rc = NEXUS_SurfaceCompositor_SetSettings(server->surfaceCompositor, &surface_compositor_settings);
    if (rc) return BERR_TRACE(rc);
    BDBG_WRN(("enabled surface_compositor again"));

    return 0;
}

int   brc_client_add_video_decoder_input(brc_client_t _client, unsigned window_id, NEXUS_VideoDecoderHandle handle)
{
    b_refsw_client_t client = _client->parent;
    unsigned i;
    int rc;

    b_init_interfaces();
        
    if (window_id >= NEXUS_SURFACE_COMPOSITOR_VIDEO_WINDOWS) {
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }
    rc = NEXUS_Platform_AcquireObject(client->ipc.nexusClient, &g_interface.videoDecoderInterface, handle);
    if (rc) return BERR_TRACE(rc);
    client->videoDecoder = handle;
    
    for (i=0;i<NEXUS_NUM_DISPLAYS;i++) {
        NEXUS_VideoWindowHandle window = b_get_window(client->server, i, window_id);
        if (window) {
            rc = NEXUS_VideoWindow_AddInput(window, NEXUS_VideoDecoder_GetConnector(handle));
            if (rc) {rc = BERR_TRACE(rc); goto err_add_input;}
        }
    }
    return 0;
    
err_add_input:
    NEXUS_Platform_ReleaseObject(&g_interface.videoDecoderInterface, handle);
    return rc;
}

int   brc_client_add_hdmi_input(brc_client_t _client, unsigned window_id, NEXUS_HdmiInputHandle handle)
{
#if NEXUS_HAS_HDMI_INPUT
    b_refsw_client_t client = _client->parent;
    unsigned i;
    int rc;
    
    b_init_interfaces();

    if (window_id >= NEXUS_SURFACE_COMPOSITOR_VIDEO_WINDOWS) {
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }
    rc = NEXUS_Platform_AcquireObject(client->ipc.nexusClient, &g_interface.hdmiInputInterface, handle);
    if (rc) return BERR_TRACE(rc);
    client->hdmiInput = handle;
    
    for (i=0;i<NEXUS_NUM_DISPLAYS;i++) {
        NEXUS_VideoWindowHandle window = b_get_window(client->server, i, window_id);
        if (window) {
            rc = NEXUS_VideoWindow_AddInput(window, NEXUS_HdmiInput_GetVideoConnector(handle));
            if (rc) {rc = BERR_TRACE(rc); goto err_add_input;}
        }
    }
    return 0;
    
err_add_input:
    NEXUS_Platform_ReleaseObject(&g_interface.hdmiInputInterface, handle);
    return rc;
#else
    BSTD_UNUSED(_client);
    BSTD_UNUSED(window_id);
    BSTD_UNUSED(handle);
    return BERR_TRACE(NEXUS_NOT_SUPPORTED);
#endif
}

void  brc_client_remove_window_input(brc_client_t _client, unsigned window_id)
{
    b_refsw_client_t client = _client->parent;
    unsigned i;

    if (window_id >= NEXUS_SURFACE_COMPOSITOR_VIDEO_WINDOWS) {
        BERR_TRACE(NEXUS_INVALID_PARAMETER);
        return;
    }
    for (i=0;i<NEXUS_NUM_DISPLAYS;i++) {
        NEXUS_VideoWindowHandle window = b_get_window(client->server, i, window_id);
        if (window) {
            NEXUS_VideoWindow_RemoveAllInputs(window);
        }
    }
    
    b_refsw_release_inputs(client);
}

int refserver_simpledecoder_set_decoder(b_refsw_server_t server, bool enabled)
{
    if (!enabled) {
        b_simpledecoder_set_resources(&server->simpledecoder.state, false, &server->simpledecoder.resources);
        b_simpledecoder_resources_close(&server->simpledecoder.resources);
    }
    else {
        unsigned i, j;
        int rc;
        b_simpledecoder_resources_config resources_config;
        
        b_simpledecoder_get_default_resources_config(&resources_config);
        resources_config.numVideoDecoders = server->simpledecoder.mosaic_enabled?4:1;
        resources_config.heap = server->platformConfig.heap[NEXUS_MAX_HEAPS-1];
        rc = b_simpledecoder_resources_open(&server->simpledecoder.resources, &resources_config);
        if (rc) return BERR_TRACE(rc);
        
        for (j=0;j<NEXUS_NUM_DISPLAYS;j++) {
            for (i=0;i<B_MAX_VIDEO_WINDOWS && i<NEXUS_SURFACE_COMPOSITOR_VIDEO_WINDOWS;i++) {        
                server->simpledecoder.resources.display[j].window[i].window = server->display_state.display[j].window[i];
            }
        }
        rc = b_simpledecoder_set_resources(&server->simpledecoder.state, true, &server->simpledecoder.resources);
        if (rc) return BERR_TRACE(rc);
        
        rc = b_simpledecoder_set_config(&server->simpledecoder.state, &server->simpledecoder.config, &server->simpledecoder.resources);
        if (rc) return BERR_TRACE(rc);
    }
    return 0;
}

int  brc_client_set_simple_decoder_mosaic_mode(brc_client_t _client, bool mosaic_enabled)
{
    b_refsw_client_t client = _client->parent;
    if (client->server->simpledecoder.mosaic_enabled != mosaic_enabled) {
        bool decode_enabled = client->server->simpledecoder.state.enabled;
        int rc;
        NEXUS_SurfaceCompositorSettings surface_compositor_settings;

        NEXUS_SurfaceCompositor_GetSettings(client->server->surfaceCompositor, &surface_compositor_settings);
        /* you don't have to disable surfacecmp to change video windows. there are no actions in-flight. */

        b_set_windows(client->server, false, &surface_compositor_settings);
        rc = NEXUS_SurfaceCompositor_SetSettings(client->server->surfaceCompositor, &surface_compositor_settings);
        if (rc) return BERR_TRACE(rc);

        /* must set false before changing .mosaic so that mosaic windows are destroyed */
        if (decode_enabled) {
            refserver_simpledecoder_set_decoder(client->server, false);
        }
        
        client->server->simpledecoder.mosaic_enabled = mosaic_enabled;

        b_open_mosaic_windows(client->server, client->server->simpledecoder.mosaic_enabled);

        if (decode_enabled) {
            refserver_simpledecoder_set_decoder(client->server, true);
        }
        
        b_set_windows(client->server, true, &surface_compositor_settings);
        rc = NEXUS_SurfaceCompositor_SetSettings(client->server->surfaceCompositor, &surface_compositor_settings);
        if (rc) return BERR_TRACE(rc);
    }
    return 0;
}

int brc_client_request_video_decoder(brc_client_t _client, unsigned id)
{
    b_refsw_client_t client = _client->parent;
    if (id != 0) {
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }
    client->request_decode = true;
    return refserver_simpledecoder_set_decoder(client->server, false);
}

void brc_client_return_video_decoder(brc_client_t _client, unsigned id)
{
    b_refsw_client_t client = _client->parent;
    if (id != 0) {
        BERR_TRACE(NEXUS_INVALID_PARAMETER);
        return;
    }
    client->request_decode = false;
    (void)refserver_simpledecoder_set_decoder(client->server, true);
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

static void *ipc_thread(void *context)
{
    b_refsw_server_t server = context;
    int listen_fd;
    int rc;
    struct sockaddr_un sock_addr;
    b_refsw_client_t client;

    BKNI_AcquireMutex(server->lock);
    listen_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if(listen_fd < 0) { perror("");rc=BERR_TRACE(errno); goto done; }

    BKNI_Memset(&sock_addr, 0, sizeof(sock_addr));
    sock_addr.sun_family = AF_UNIX;
    BDBG_ASSERT(sizeof(sock_addr.sun_path)>=sizeof(b_refsw_endpoint));
    unlink(b_refsw_endpoint);
    BKNI_Memcpy(sock_addr.sun_path, b_refsw_endpoint, sizeof(b_refsw_endpoint));

    fcntl(listen_fd, F_SETFL, O_NONBLOCK);

    rc = bind(listen_fd, (struct sockaddr *)&sock_addr, sizeof(sock_addr.sun_family)+sizeof(b_refsw_endpoint));
    if(rc!=0) { perror("");rc=BERR_TRACE(errno); goto done; }
    /* allow non-root access */
    chmod(b_refsw_endpoint, 0666);
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
    "usage: nexus %s [--help|-h]\n",
    name);
    printf(
    "options:\n"
    "  --help|-h      \tprint this help screen\n"
    "  -move off      \tdisable movement\n"
    "  -sd off        \tdisable Standard Definition display (default is enabled)\n"
    "  -3d on/lr/ou   \tenable stereoscopic (3D) video\n"
    );
    printf(
    "  -native_3d on  \tenable native stereoscopic (3D) video\n"
    "  -cursor on     \tenable cursor\n"
    "  -tunnel client \tname of client that is allowed to allocate tunneled surfaces\n"
    "  -framebuffers n\tnumber of framebuffers allocated for the primary display\n"
    "  -timeout X     \texit after X seconds. default is to prompt for user.\n"
    );
    printf(
    "  -ignore_edid   \tdon't consider HDMI EDID for audio/video output\n"
    "  -frontend      \tenable frontend\n"
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

        if (client->movement.fade) {
            client_settings.composition.colorMatrixEnabled = true;
            NEXUS_Graphics2D_SetAlphaColorMatrix(client->movement.alpha++, &client_settings.composition.colorMatrix);
        }
        else {
            client_settings.composition.colorMatrixEnabled = false;
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

static struct b_refsw_server g_server;

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

#if NEXUS_HAS_HDMI_OUTPUT
static void hotplug_callback(void *pParam, int iParam)
{
    NEXUS_HdmiOutputStatus status;
    NEXUS_HdmiOutputHandle hdmi = pParam;
    int rc;
    b_refsw_server_t server = &g_server;

    BSTD_UNUSED(iParam);
    rc = NEXUS_HdmiOutput_GetStatus(hdmi, &status);
    if (rc || !status.connected) {
        return;
    }
        
    if (!server->options.ignore_edid) {
        b_simpledecoder_hdmi_edid_config(&server->simpledecoder.config, &status, &server->simpledecoder.resources);
        b_simpledecoder_set_config(&server->simpledecoder.state, &server->simpledecoder.config, &server->simpledecoder.resources);
    }
}
#endif

static void b_refsw_surface_compositor_inactive(void *context, int param)
{
    b_refsw_server_t server = context;
    BSTD_UNUSED(param);
    if (server->displayFormatChange.pending) {
        server->displayFormatChange.pending = false;
        b_refsw_display_format_change(server);
    }
}

int main(int argc, char **argv)
{
    NEXUS_PlatformStartServerSettings serverSettings;
    NEXUS_Error rc;
    int curarg = 1;
    bipc_server_create_settings ipc_settings;
    NEXUS_SurfaceCompositorSettings surface_compositor_settings;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_PlatformSettings platformSettings;
    pthread_t ipc_thread_id;
    NEXUS_VideoOrientation  orientation = NEXUS_VideoOrientation_e2D;
    bool native_3d = false;
    unsigned timeout = 0;
    bool openFrontend = false;
    b_refsw_server_t server = &g_server;

    memset(server, 0, sizeof(&server));
    server->options.move = true;
    server->options.hdsd_simul = true;
    server->options.cursor = false;
    server->options.framebuffers = 0;
    server->options.tunnel_client = NULL;
    server->defaultClientMode = NEXUS_ClientMode_eProtected;

    while (curarg < argc) {
        if (!strcmp(argv[curarg], "--help") || !strcmp(argv[curarg], "-h")) {
            print_usage(argv[0]);
            return 0;
        }
        else if (!strcmp(argv[curarg], "-ignore_edid")) {
            server->options.ignore_edid = true;
        }
        else if (!strcmp(argv[curarg], "-frontend")) {
            openFrontend = true;
        }
        else if (!strcmp(argv[curarg], "-timeout") && argc>curarg+1) {
            timeout = strtoul(argv[++curarg], NULL, 0);
        }
        else if (!strcmp(argv[curarg], "-move") && curarg+1<argc) {
            curarg++;
            if (!strcmp(argv[curarg], "off")) {
                server->options.move = false;
            }
        }
        else if (!strcmp(argv[curarg], "-sd") && curarg+1<argc) {
            curarg++;
            if (!strcmp(argv[curarg], "off")) {
                server->options.hdsd_simul = false;
            }
        }
        else if (!strcmp(argv[curarg], "-cursor") && curarg+1<argc) {
            curarg++;
            if (strcmp(argv[curarg], "on")==0) {
                server->options.cursor = true;
            }
        } else if(!strcmp(argv[curarg], "-framebuffers") && curarg+1<argc) {
            server->options.framebuffers = atoi(argv[++curarg]);
        } else if(!strcmp(argv[curarg], "-tunnel") && curarg+1<argc) {
            server->options.tunnel_client = argv[++curarg];
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
        }
        else if (!strcmp(argv[curarg], "-mode") && curarg+1 < argc) {
            ++curarg;
            if (!strcmp("untrusted", argv[curarg])) {
                server->defaultClientMode = NEXUS_ClientMode_eUntrusted;
            }
            else if (!strcmp("protected", argv[curarg])) {
                server->defaultClientMode = NEXUS_ClientMode_eProtected;
            }
            else if (!strcmp("unprotected", argv[curarg])) {
                server->defaultClientMode = NEXUS_ClientMode_eUnprotected;
            }
        }
        else {
            fprintf(stderr,"invalud argument %s\n", argv[curarg]);
            print_usage(argv[0]);
            return -1;
        }
        curarg++;
    }

    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    /* use default heaps. for realworld apps, the server app may want to create a special heap
    to isolate untrusted clients. */

    /* create 8MB eFull heap for untrusted clients to do packet blit and playpump. realworld apps may want
    a pool of heaps so untrusted clients don't share. */
    platformSettings.heap[NEXUS_MAX_HEAPS-1].size = 8 * 1024 * 1024;
    platformSettings.heap[NEXUS_MAX_HEAPS-1].memcIndex = 0;
    platformSettings.heap[NEXUS_MAX_HEAPS-1].guardBanding = false; /* corruptions shouldn't cause server failures */
    platformSettings.heap[NEXUS_MAX_HEAPS-1].memoryType = NEXUS_MemoryType_eFull; /* requires for packet blit and playpump */

    platformSettings.openFrontend = openFrontend;
    rc = NEXUS_Platform_Init(&platformSettings);
    if (rc) return -1;


    NEXUS_Platform_GetConfiguration(&server->platformConfig);

    /* open HD/SD smilu display */
    NEXUS_Display_GetDefaultSettings(&displaySettings);
    displaySettings.format = NEXUS_VideoFormat_e720p;
    if(native_3d) {
        displaySettings.display3DSettings.overrideOrientation = true;
        displaySettings.display3DSettings.orientation = orientation;
    }
    server->display_state.display[0].display = NEXUS_Display_Open(0, &displaySettings);
    server->display_state.display[0].window[0] = NEXUS_VideoWindow_Open(server->display_state.display[0].display, 0);
    NEXUS_Display_GetDefaultSettings(&displaySettings);

#if NEXUS_NUM_DISPLAYS > 1
    if (server->options.hdsd_simul) {
        displaySettings.format = NEXUS_VideoFormat_eNtsc;
        server->display_state.display[1].display = NEXUS_Display_Open(1, &displaySettings);
    }
    if (server->display_state.display[1].display) {
        server->display_state.display[1].window[0] = NEXUS_VideoWindow_Open(server->display_state.display[1].display, 0);
    }
#endif

#if NEXUS_NUM_COMPONENT_OUTPUTS
    NEXUS_Display_AddOutput(server->display_state.display[0].display, NEXUS_ComponentOutput_GetConnector(server->platformConfig.outputs.component[0]));
#endif
#if NEXUS_NUM_COMPOSITE_OUTPUTS
    if(server->display_state.display[1].display) {
        NEXUS_Display_AddOutput(server->display_state.display[1].display, NEXUS_CompositeOutput_GetConnector(server->platformConfig.outputs.composite[0]));
    }
#endif

    BKNI_CreateMutex(&server->lock);

    b_simpledecoder_get_default_config(&server->simpledecoder.config);

    rc = b_simpledecoder_init(&server->simpledecoder.state, 0);
    BDBG_ASSERT(!rc);

    rc = refserver_simpledecoder_set_decoder(server, true);
    BDBG_ASSERT(!rc);

#if NEXUS_NUM_HDMI_OUTPUTS
    /* Need to configure HDMI after simple decoder since HDMI callback impl needs
     * initialized simple decoder.
     */
    NEXUS_Display_AddOutput(server->display_state.display[0].display, NEXUS_HdmiOutput_GetVideoConnector(server->platformConfig.outputs.hdmi[0]));
    {
        NEXUS_HdmiOutputSettings hdmiSettings;
        NEXUS_HdmiOutput_GetSettings(server->platformConfig.outputs.hdmi[0], &hdmiSettings);
        hdmiSettings.hotplugCallback.callback = hotplug_callback;
        hdmiSettings.hotplugCallback.context = server->platformConfig.outputs.hdmi[0];
        rc = NEXUS_HdmiOutput_SetSettings(server->platformConfig.outputs.hdmi[0], &hdmiSettings);
        BDBG_ASSERT(!rc);
        /* Configure HDMI audio outputs */
        hotplug_callback(server->platformConfig.outputs.hdmi[0], 0);
    }
#endif
    
    NEXUS_Platform_GetDefaultStartServerSettings(&serverSettings);
    serverSettings.allowUnauthenticatedClients = false; /* client is written this way */
    rc = NEXUS_Platform_StartServer(&serverSettings);
    BDBG_ASSERT(!rc);

    BKNI_CreateEvent(&server->inactiveEvent);
    
    server->surfaceCompositor = NEXUS_SurfaceCompositor_Create(0);
    NEXUS_SurfaceCompositor_GetSettings(server->surfaceCompositor, &surface_compositor_settings);
    NEXUS_Display_GetGraphicsSettings(server->display_state.display[0].display, &surface_compositor_settings.display[0].graphicsSettings);
    surface_compositor_settings.display[0].graphicsSettings.enabled = true;
    surface_compositor_settings.display[0].display = server->display_state.display[0].display;
    surface_compositor_settings.display[0].framebuffer.width = 1920;
    surface_compositor_settings.display[0].framebuffer.height = 1080;
    surface_compositor_settings.display[0].framebuffer.backgroundColor = 0x00000000; /* transparent background */
    surface_compositor_settings.display[0].framebuffer.heap = NEXUS_Platform_GetFramebufferHeap(0);
    surface_compositor_settings.inactiveCallback.callback = b_refsw_surface_compositor_inactive;
    surface_compositor_settings.inactiveCallback.context = server;

    if(server->options.framebuffers) {
        surface_compositor_settings.display[0].framebuffer.number = server->options.framebuffers;
    }

    if(!native_3d && orientation!=NEXUS_VideoOrientation_e2D) {
        surface_compositor_settings.display[0].display3DSettings.overrideOrientation = true;
        surface_compositor_settings.display[0].display3DSettings.orientation = orientation;
    }

    if (server->display_state.display[1].display) {
        NEXUS_Display_GetGraphicsSettings(server->display_state.display[1].display, &surface_compositor_settings.display[1].graphicsSettings);
        surface_compositor_settings.display[1].graphicsSettings.enabled = true;
        surface_compositor_settings.display[1].display = server->display_state.display[1].display;
        surface_compositor_settings.display[1].framebuffer.width = 720;
        surface_compositor_settings.display[1].framebuffer.height = 480;
        surface_compositor_settings.display[1].framebuffer.backgroundColor = 0x00000000; /* transparent background */
        surface_compositor_settings.display[1].framebuffer.heap = NEXUS_Platform_GetFramebufferHeap(1);
    }

    b_set_windows(server, true, &surface_compositor_settings);

    surface_compositor_settings.frameBufferCallback.callback = framebuffer_callback;
    surface_compositor_settings.frameBufferCallback.context = &g_server;
    rc = NEXUS_SurfaceCompositor_SetSettings(server->surfaceCompositor, &surface_compositor_settings);
    BDBG_ASSERT(!rc);
    if(server->options.cursor) {
        NEXUS_SurfaceCreateSettings settings;
        NEXUS_SurfaceCursorCreateSettings cursorSettings;
        NEXUS_SurfaceCursorSettings config;

        NEXUS_Surface_GetDefaultCreateSettings(&settings);

        settings.width = 8;
        settings.height = 8;
        server->cursor.surface = NEXUS_Surface_Create(&settings);
        BDBG_ASSERT(server->cursor.surface);
        make_cursor(server->cursor.surface, &settings);
        NEXUS_SurfaceCursor_GetDefaultCreateSettings(&cursorSettings);

        cursorSettings.surface = server->cursor.surface;
        server->cursor.cursor = NEXUS_SurfaceCursor_Create(server->surfaceCompositor, &cursorSettings);
        BDBG_ASSERT(server->cursor.cursor);
        NEXUS_SurfaceCursor_GetSettings (server->cursor.cursor, &config);
        config.composition.visible = true;
        config.composition.virtualDisplay.width  = 400;
        config.composition.virtualDisplay.height = 200;
        config.composition.position.x = 100;
        config.composition.position.y = 100;
        config.composition.position.width = 8;
        config.composition.position.height = 8;
        NEXUS_SurfaceCursor_SetSettings (server->cursor.cursor, &config);
    }

    bipc_server_get_default_create_settings(&ipc_settings);
    ipc_settings.interfaces = ipc_interfaces;
    ipc_settings.interface_count = sizeof(ipc_interfaces)/sizeof(*ipc_interfaces);

    BLST_D_INIT(&server->clients);
    server->ipc.exit = false;
    server->ipc.server = bipc_server_create(&ipc_settings);
    BDBG_ASSERT(server->ipc.server);

    signal(SIGPIPE, SIG_IGN);
    if(pthread_create(&ipc_thread_id, NULL, ipc_thread, server)) {
        BDBG_ASSERT(0);
    }

    if (!timeout) {
        printf("Press ENTER to shutdown server\n");
        getchar();
    }
    else {
        /* auto close */
        BKNI_Sleep(timeout*1000);
    }

    server->ipc.exit = true;
    pthread_join(ipc_thread_id, NULL);
    bipc_server_destroy(server->ipc.server);
    NEXUS_Platform_StopServer();
    BKNI_DestroyMutex(server->lock);

    if(server->options.cursor) {
        NEXUS_SurfaceCursor_Destroy(server->cursor.cursor);
        NEXUS_Surface_Destroy(server->cursor.surface);
    }

    /* stop the server before closing resources that may be in use by clients.
    if it's an untrusted client, handle verification may fail the call. but a trusted client bypasses the
    check and could kill the server-> */
    NEXUS_SurfaceCompositor_Destroy(server->surfaceCompositor);
    server->surfaceCompositor = NULL;
    BKNI_DestroyEvent(server->inactiveEvent);
    
    (void)refserver_simpledecoder_set_decoder(server, false);
    BDBG_ASSERT(!rc);
    b_simpledecoder_uninit(&server->simpledecoder.state);

    NEXUS_StopCallbacks(server->platformConfig.outputs.hdmi[0]);    
    b_open_mosaic_windows(server, false); /* destroy mosaics */
    NEXUS_VideoWindow_Close(server->display_state.display[0].window[0]);
    NEXUS_Display_Close(server->display_state.display[0].display);
    if (server->display_state.display[1].display) {
        NEXUS_VideoWindow_Close(server->display_state.display[1].window[0]);
        NEXUS_Display_Close(server->display_state.display[1].display);
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


