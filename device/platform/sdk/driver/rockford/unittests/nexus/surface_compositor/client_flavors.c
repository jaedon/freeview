/******************************************************************************
 *    (c)2008-2011 Broadcom Corporation
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
 * $brcm_Workfile: client_flavors.c $
 * $brcm_Revision: Hydra_Software_Devel/21 $
 * $brcm_Date: 9/24/12 6:29p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/surface_compositor/client_flavors.c $
 * 
 * Hydra_Software_Devel/21   9/24/12 6:29p mward
 * SW7435-370:  Clean up resources in error returns from
 * tunneled_client().
 * 
 * Hydra_Software_Devel/20   8/8/12 4:57p vsilyaev
 * SW7435-296: Added unit-test for set/publish cliet
 * 
 * Hydra_Software_Devel/19   4/13/12 10:09a erickson
 * SW7425-2874: stop callbacks before destroying clients
 * 
 * Hydra_Software_Devel/18   12/22/11 2:48p vsilyaev
 * SW7420-2148: Added case for blending
 * 
 * Hydra_Software_Devel/17   12/22/11 2:23p vsilyaev
 * SW7420-2148: Added another test
 * 
 * Hydra_Software_Devel/16   11/22/11 1:41p vsilyaev
 * SW7420-2148: Use faster path for Set/Update clients, better handle out-
 * of-phase displayed callback
 * 
 * Hydra_Software_Devel/15   11/18/11 2:57p erickson
 * SW7420-1148: typo
 * 
 * Hydra_Software_Devel/14   11/7/11 10:58a vsilyaev
 * SW7420-2119: Refactored tunnel mode support
 * 
 * Hydra_Software_Devel/13   11/3/11 9:15p vsilyaev
 * SW7420-2119: Improved handling of 'real' tunnel clients
 * 
 * Hydra_Software_Devel/12   11/1/11 5:26p vsilyaev
 * SW7420-2119: Returned back support for single frame buffer rendering
 * 
 * Hydra_Software_Devel/11   11/1/11 1:02p vsilyaev
 * SW7420-2114: Added support for native 3D graphics
 * 
 * Hydra_Software_Devel/10   10/28/11 1:33p vsilyaev
 * SW7420-2110: Use better blend equation for cursor rendering
 * 
 * Hydra_Software_Devel/9   10/27/11 4:24p vsilyaev
 * SW7420-2110: Added support  for incremental (without redrawing of
 * entire framebuffer) cursor drawing
 * 
 * Hydra_Software_Devel/8   10/26/11 3:26p vsilyaev
 * SW7420-1992: Added update and movement delay
 * 
 * Hydra_Software_Devel/7   10/24/11 5:43p vsilyaev
 * SW7420-1992: Updated cursor support
 * 
 * Hydra_Software_Devel/6   10/20/11 2:13p vsilyaev
 * SW7420-1992: Added 3D compositing
 * 
 * Hydra_Software_Devel/5   10/18/11 12:32p vsilyaev
 * SW7420-1992: Moved virtual display into the composition settings
 * 
 * Hydra_Software_Devel/4   10/12/11 12:50p vsilyaev
 * SW7420-1992: On assert print client handle
 * 
 * Hydra_Software_Devel/3   10/10/11 10:32a erickson
 * SW7420-1992: add tunneled_client, run for finite time for automation,
 * add options
 * 
 * Hydra_Software_Devel/2   9/13/11 6:21p vsilyaev
 * SW7420-1148: Added push client
 * 
 * Hydra_Software_Devel/1   9/7/11 7:15p vsilyaev
 * SW7420-1148: Test cases for the surface compositor
 * 
 *
 *****************************************************************************/

#include "nexus_platform.h"
#include "nexus_surface.h"
#include "nexus_graphics2d.h"
#include "nexus_display.h"
#include "nexus_base_mmap.h"
#include "nexus_component_output.h"
#include "nexus_core_utils.h"
#include "bstd.h"
#include "bkni.h"
#include "nexus_surface_compositor.h"
#include "nexus_surface_cursor.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <errno.h>

BDBG_MODULE(client_flavors);

#define UNIT_ASSERT(client,cond) do { if(!(cond)) {BDBG_ERR(("client %p", client));BDBG_ASSERT(cond);} }while(0)

static void complete(void *data, int unused)
{
    BSTD_UNUSED(unused);
    BKNI_SetEvent((BKNI_EventHandle)data);
}

static void checkpoint(NEXUS_Graphics2DHandle gfx, BKNI_EventHandle event)
{
    int rc;
    rc = NEXUS_Graphics2D_Checkpoint(gfx, NULL);
    if (rc == NEXUS_GRAPHICS2D_QUEUED) {
        BKNI_WaitForEvent(event, BKNI_INFINITE);
    }
}

/*
application options
*/
#define TUNNEL_MODE 0
#define SINGLEBUFFER_MODE 0
#if TUNNEL_MODE
#define TUNNEL_SPLIT 0
#endif

#if SINGLEBUFFER_MODE && TUNNEL_MODE
/* single buffered tunneled mode can only have 1 client */
#define NUM_CLIENTS 1
#else
#define NUM_CLIENTS 7
#endif

static const NEXUS_Rect screen = {0,0,720, 480};
#if TUNNEL_SPLIT
static const NEXUS_Rect sidebar = {480,0,240, 480};
#else
#define sidebar  screen
#endif

struct client_state  {
    pthread_t thread;
    unsigned id;
    bool done;
}; 

static int tunneled_client(struct client_state *state);
static int set_client(struct client_state *state);
static int push_client(struct client_state *state);
static int update_client(struct client_state *state);
static int push_lean_client(struct client_state *state);
static int set_publish_client(struct client_state *state);

static struct {
    bool enable;
    NEXUS_SurfaceHandle surface;
    NEXUS_SurfaceCursorHandle cursor;
} cursor;

typedef struct {
    unsigned min;
    unsigned max;
} range;

typedef struct {
    unsigned count;
    unsigned limit;
} delay_state;

static struct {
    range client_movement;
    range cursor_movement;
    range client_update;
} options;

struct client_info {
    unsigned width, height;
    int inc_x, inc_y;
    int inc_width, inc_height;
    bool tunnelCapable;
    bool displayCache;
    bool enable;
    const NEXUS_Rect *bounds;
    int (*client)(struct client_state *);
    delay_state delay;
} g_client_transform[NUM_CLIENTS] = {
    {
#if TUNNEL_SPLIT
        480, 480, 
#else
        720, 480,
#endif
        0, 0, 0, 0, TUNNEL_MODE,  false, true, &screen, tunneled_client, {0,0}}, /* stationary full screen */
#if NUM_CLIENTS > 1
    {100, 100, 1, 1, 0, 0, false, false, true, &sidebar, set_client, {0,0} },
    {150, 150,-1, 2, 3, 3, false, false, true, &sidebar, push_client, {0,0} },
    {200, 200, -1, -1, 0, 0, false, false, true, &sidebar, update_client, {0,0} },
    {250, 200, +1, -1, 0, 0, false, false, true, &sidebar, push_lean_client, {0,0} },
    {350, 200, -1, -2, 0, 0, false, false, true, &sidebar, set_publish_client, {0,0} },
    {250, 250,-3,-2, 0, 0, false, true,  true, &sidebar, NULL, {0,0} }  /* display cache test */
#endif
};

/* global data for callbacks */
static NEXUS_SurfaceCompositorHandle surface_compositor;
static NEXUS_SurfaceClientHandle blit_client[NUM_CLIENTS];

static bool update_delay(const range *range, delay_state *state)
{
    state->count++;
    if(state->count < state->limit) {
        return false;
    }
    state->count = 0;
    state->limit = range->min;
    if(range->min != range->max) {
        state->limit += rand() % (1+(range->max - range->min));
    }
    return true;
}

static void client_delay(const range *range)
{
    delay_state state = {0,0};
    update_delay(range, &state);
    if(range->max == 0) {
        return;
    }
    for(;;) {
        BKNI_Sleep(30);
        if(update_delay(range, &state)) {
            break;
        }
    }
    return;
}

static void parse_range(const char *str, range *range)
{
    if(sscanf(str, "%u,%u", &range->min, &range->max)!=2) {
        range->max=range->min;
    }
    return;
}

static void move_cursor(void)
{
    static delay_state  cursor_delay = {0,0};
    if(cursor.enable && update_delay(&options.cursor_movement, &cursor_delay)) {
        NEXUS_SurfaceCursorSettings config;
        NEXUS_SurfaceCursor_GetSettings (cursor.cursor, &config);
        config.composition.position.x +=2;
        config.composition.position.y +=1;
        if(config.composition.position.x >= config.composition.virtualDisplay.width) {
            config.composition.position.x -= config.composition.virtualDisplay.width+11;
        }
        if(config.composition.position.y >= config.composition.virtualDisplay.height) {
            config.composition.position.y -= config.composition.virtualDisplay.height+17;
        }
        NEXUS_SurfaceCursor_SetSettings (cursor.cursor, &config);
    }
    return;
}

static void *cursor_thread_entry(void *unused)
{
    BSTD_UNUSED(unused);
    for(;;) {
        BKNI_Sleep(20);
        if(!cursor.enable) {
            break;
        }
        move_cursor();
    }
    return NULL;
}

void framebuffer_callback(void *context, int param)
{
    unsigned i;
    NEXUS_SurfaceCompositorClientSettings client_settings;    
    NEXUS_Error rc;
    
    BSTD_UNUSED(context);
    BSTD_UNUSED(param);
    
    for (i=0;i<NUM_CLIENTS;i++) {
        struct client_info *client = &g_client_transform[i];
        if (!blit_client[i]) continue;
        
        if (!client->inc_x &&
            !client->inc_y &&
            !client->inc_width &&
            !client->inc_height) continue;

        if(!update_delay(&options.client_movement, &client->delay)) {
            continue;
        }
            
        NEXUS_SurfaceCompositor_GetClientSettings(surface_compositor, blit_client[i], &client_settings);
        client_settings.composition.position.x += client->inc_x;
        client_settings.composition.position.y += client->inc_y;
        
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
#if TUNNEL_SPLIT
        if(client_settings.composition.position.x < client->bounds->x) {
            client_settings.composition.position.x = client->bounds->x;
        }
        if(client_settings.composition.position.y < client->bounds->y) {
            client_settings.composition.position.y = client->bounds->y;
        }
#endif
        
        /* grow/shrink to test clipping combinations */
        if (client->inc_width) {
            client_settings.composition.position.width += client->inc_width;
            if ((client->inc_width > 0 && client_settings.composition.position.width >= 300) ||
                (client->inc_width < 0 && client_settings.composition.position.width < 50))
            {
                client->inc_width *= -1;
            }
        }
        if (client->inc_height) {
            client_settings.composition.position.height += client->inc_height;
            if ((client->inc_height > 0 && client_settings.composition.position.height >= 300) ||
                (client->inc_height < 0 && client_settings.composition.position.height < 50))
            {
                client->inc_height *= -1;
            }
        }
        
        rc = NEXUS_SurfaceCompositor_SetClientSettings(surface_compositor, blit_client[i], &client_settings);
        BDBG_ASSERT(!rc);
    }
}

typedef struct {
    const char *name;
    int value;
} namevalue_t;

const namevalue_t g_videoFormatStrs[] = {
    {"ntsc",      NEXUS_VideoFormat_eNtsc},
    {"480i",      NEXUS_VideoFormat_eNtsc},
    {"pal",       NEXUS_VideoFormat_ePal},
    {"576i",      NEXUS_VideoFormat_ePal},
    {"1080i",     NEXUS_VideoFormat_e1080i},
    {"720p",      NEXUS_VideoFormat_e720p},
    {"480p",      NEXUS_VideoFormat_e480p},
    {"576p",      NEXUS_VideoFormat_e576p},
    {"1080p",     NEXUS_VideoFormat_e1080p},
    {"720p3D",    NEXUS_VideoFormat_e3D_720p},
    {"1080p3D",   NEXUS_VideoFormat_e3D_1080p_24Hz},
    {NULL, 0}
};

static const namevalue_t g_videoOrientation[] = {
    {"2D", NEXUS_VideoOrientation_e2D},
    {"LeftRight", NEXUS_VideoOrientation_e3D_LeftRight},   
    {"3D", NEXUS_VideoOrientation_e3D_LeftRight},
    {"OverUnder", NEXUS_VideoOrientation_e3D_OverUnder},
    {NULL, 0}
};

static unsigned lookup(const namevalue_t *table, const char *name)
{
    unsigned i;
    unsigned value;
    for (i=0;table[i].name;i++) {
        if (!strcasecmp(table[i].name, name)) {
            return table[i].value;
        }
    }
    errno = 0; /* there is only way to know that strtol failed is to look at errno, so clear it first */
    value = strtol(name, NULL, 0);
    if(errno != 0) {
        errno = 0;
        value = table[0].value;
    }
    printf("Unknown cmdline param '%s', using %u as value\n", name, value);
    return value;
}


static void print_list(const namevalue_t *table)
{
    unsigned i;
    const char *sep=" {";
    for (i=0;table[i].name;i++) {
        /* skip aliases */
        if (i > 0 && table[i].value == table[i-1].value) continue;
        printf("%s%s",sep,table[i].name);
        sep = ",";
    }
    printf("}");
}

static void print_list_option(const char *name, const namevalue_t *list)
{
    printf("  -%s ", name);
    print_list(list);
    printf("\n");
}




static void print_usage(const char *app)
{
    printf(
    "usage: nexus %s OPTIONS\n"
    "options:\n"
    "  --help|-h    print this help screen\n"
    "  -rand        randomly start/stop clients\n"
    "  -seed x      use seed for random number generator\n"
    "  -3d          enable 3D composition\n"
    "  -cursor      draw cursor\n"
    "  -movement min[,max] - control how often clients moved\n"
    "  -update min[,max] - control how often clients updated\n"
    "  -cursor_movement min[,max] - control how often cursor moved\n"
    "  -timeout X   run for X seconds, then exit (default 120)\n"
    "  -disable n   disable client 'n'\n",
    app);
    printf(
    "  -buffers n  number of framebuffers for main display\n"
    "  -secondary_buffers n  number of framebuffers for secondary display\n"
    "  -tunnel n   enable tunnel mode for the client 'n'\n"
    );
    print_list_option("main  format of main display", g_videoFormatStrs);
    print_list_option("main_3d  orientation of main display", g_videoOrientation);
    print_list_option("secondary  format of secondary display", g_videoFormatStrs);
}

static void *thread_entry(void *arg)
{
    struct client_state *state = arg;
    g_client_transform[state->id].client(state);
    return NULL;
}

static void start_client(struct client_state *state)
{
    const struct client_info *client = &g_client_transform[state->id];
    state->thread = (pthread_t)0;
    if (client->enable && client->client) {
        int rc;
        BDBG_WRN(("start client %d", state->id));
        rc = pthread_create(&state->thread, NULL, thread_entry, state);
        BDBG_ASSERT(rc==0);
    }
}
    
static void stop_client(struct client_state *state)
{
    if (state->thread) {
        state->done = true;
        BDBG_WRN(("stop client %d", state->id));
        pthread_join(state->thread, NULL);
        state->thread = 0;
    }
}    

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


int main(int argc, char **argv)
{
    NEXUS_SurfaceHandle desktop;
    NEXUS_SurfaceCreateSettings createSettings;
    NEXUS_DisplayHandle display[2];
    NEXUS_DisplaySettings displaySettings;
    NEXUS_Graphics2DHandle gfx;
    NEXUS_Graphics2DSettings gfxSettings;
    NEXUS_Graphics2DFillSettings fillSettings;
    BKNI_EventHandle event;
    NEXUS_PlatformConfiguration platformConfig;
    int rc;
    NEXUS_PlatformSettings platformSettings;
    NEXUS_SurfaceCompositorSettings surface_compositor_settings;
    NEXUS_SurfaceClientHandle desktop_client;
    NEXUS_SurfaceCompositorClientSettings client_settings;
    unsigned i;
    int curarg = 1;
    struct client_state client_state[NUM_CLIENTS];
    bool randomize = false;
    unsigned timeout = 2*60; /* seconds */
    NEXUS_VideoOrientation  orientation = NEXUS_VideoOrientation_e2D;
    unsigned seed = time(NULL);
    pthread_t cursor_thread = 0;
    struct {
        NEXUS_VideoFormat videoFormat;
        bool overrideVideoOrientation;
        NEXUS_VideoOrientation videoOrientation;
        unsigned framebuffers;
    } display_opts[2]= {
        {NEXUS_VideoFormat_e720p, false, NEXUS_VideoOrientation_e2D, 0},
        {NEXUS_VideoFormat_eNtsc, false, NEXUS_VideoOrientation_e2D, 0}
    };

    cursor.enable = false;
    
    while (curarg < argc) {
        if (!strcmp(argv[curarg], "--help") || !strcmp(argv[curarg], "-h")) {
            print_usage(argv[0]);
            return 0;
        }
        else if (!strcmp(argv[curarg], "-rand")) {
            /* randomly start/stop clients */
            randomize = true;
        }
        else if (!strcmp(argv[curarg], "-3d")) {
            orientation = NEXUS_VideoOrientation_e3D_LeftRight;
        }
        else if (!strcmp(argv[curarg], "-cursor")) {
            cursor.enable = true;
        }
        else if (!strcmp(argv[curarg], "-timeout") && curarg+1<argc) {
            /* 0 means getchar() */
            timeout = atoi(argv[++curarg]);
        }
        else if (!strcmp(argv[curarg], "-seed") && curarg+1<argc) {
            seed = atoi(argv[++curarg]);
        }
        else if (!strcmp(argv[curarg], "-movement") && curarg+1<argc) {
            parse_range(argv[++curarg], &options.client_movement);
        }
        else if (!strcmp(argv[curarg], "-update") && curarg+1<argc) {
            parse_range(argv[++curarg], &options.client_update);
        }
        else if (!strcmp(argv[curarg], "-cursor_movement") && curarg+1<argc) {
            cursor.enable = true;
            parse_range(argv[++curarg], &options.cursor_movement);
        } else if (!strcmp(argv[curarg], "-main") && curarg+1<argc) {
            display_opts[0].videoFormat = lookup(g_videoFormatStrs, argv[++curarg]);
        } else if (!strcmp(argv[curarg], "-secondary") && curarg+1<argc) {
            display_opts[1].videoFormat = lookup(g_videoFormatStrs, argv[++curarg]);
        } else if (!strcmp(argv[curarg], "-main_3d") && curarg+1<argc) {
            display_opts[0].videoOrientation = lookup(g_videoOrientation, argv[++curarg]);
            display_opts[0].overrideVideoOrientation = true;
        } else if(!strcmp(argv[curarg], "-disable") && curarg+1<argc) {
            int client = atoi(argv[++curarg]);
            if(client>=0 && client<NUM_CLIENTS) {
                g_client_transform[client].enable = false;
            }
        } else if(!strcmp(argv[curarg], "-buffers") && curarg+1<argc) {
            display_opts[0].framebuffers = atoi(argv[++curarg]);
        } else if(!strcmp(argv[curarg], "-secondary_buffers") && curarg+1<argc) {
            display_opts[1].framebuffers = atoi(argv[++curarg]);
        } else if(!strcmp(argv[curarg], "-tunnel") && curarg+1<argc) {
            int client = atoi(argv[++curarg]);
            if(client>=0 && client<NUM_CLIENTS) {
                g_client_transform[client].tunnelCapable = true;
            }
        } else {
            fprintf(stderr,"invalid argument %s\n", argv[curarg]);
            print_usage(argv[0]);
            return -1;
        }
        curarg++;
    }
    srand(seed);

    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    rc = NEXUS_Platform_Init(&platformSettings);
    if (rc) return -1;
    NEXUS_Platform_GetConfiguration(&platformConfig);

    BKNI_CreateEvent(&event);
    BKNI_Memset(blit_client, 0, sizeof(blit_client));

    display[1] = NULL;
    NEXUS_Display_GetDefaultSettings(&displaySettings);
#if 0
    displaySettings.background = 0xFF; /* makes easier to see resulting alpha */
#endif
    displaySettings.format = display_opts[0].videoFormat;
    displaySettings.display3DSettings.overrideOrientation = display_opts[0].overrideVideoOrientation;
    displaySettings.display3DSettings.orientation = display_opts[0].videoOrientation;
    display[0] = NEXUS_Display_Open(0, &displaySettings);
#if !TUNNEL_MODE 
    displaySettings.format = display_opts[1].videoFormat;
    displaySettings.display3DSettings.overrideOrientation = display_opts[1].overrideVideoOrientation;
    displaySettings.display3DSettings.orientation = display_opts[1].videoOrientation;
    display[1] = NEXUS_Display_Open(1, &displaySettings);
#endif
#if NEXUS_NUM_COMPONENT_OUTPUTS
    NEXUS_Display_AddOutput(display[0], NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
#endif
#if NEXUS_NUM_HDMI_OUTPUTS
    NEXUS_Display_AddOutput(display[0], NEXUS_HdmiOutput_GetVideoConnector(platformConfig.outputs.hdmi[0]));
#endif
#if NEXUS_NUM_COMPOSITE_OUTPUTS
    if(display[1]) {
        NEXUS_Display_AddOutput(display[1], NEXUS_CompositeOutput_GetConnector(platformConfig.outputs.composite[0]));
    }
#endif

    gfx = NEXUS_Graphics2D_Open(NEXUS_ANY_ID, NULL);
    NEXUS_Graphics2D_GetSettings(gfx, &gfxSettings);
    gfxSettings.checkpointCallback.callback = complete;
    gfxSettings.checkpointCallback.context = event;
    NEXUS_Graphics2D_SetSettings(gfx, &gfxSettings);

    /* create desktop surface for background */
    NEXUS_Surface_GetDefaultCreateSettings(&createSettings);
    createSettings.pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;
    createSettings.width = 720;
    createSettings.height = 480;
    desktop = NEXUS_Surface_Create(&createSettings);
    NEXUS_Graphics2D_GetDefaultFillSettings(&fillSettings);
    fillSettings.color = 0xFF000000; /* black background */
    fillSettings.surface = desktop;
    rc = NEXUS_Graphics2D_Fill(gfx, &fillSettings);
    BDBG_ASSERT(!rc);
    checkpoint(gfx, event);

    /* create surface compositor server */    
    surface_compositor = NEXUS_SurfaceCompositor_Create(0);
    NEXUS_SurfaceCompositor_GetSettings(surface_compositor, &surface_compositor_settings);
    NEXUS_Display_GetGraphicsSettings(display[0], &surface_compositor_settings.display[0].graphicsSettings);
    surface_compositor_settings.display[0].graphicsSettings.enabled = true;
    surface_compositor_settings.display[0].display = display[0];
#if SINGLEBUFFER_MODE
    surface_compositor_settings.display[0].framebuffer.number = 1;
#elif TUNNEL_MODE
    surface_compositor_settings.display[0].framebuffer.number = 7;
#else
    surface_compositor_settings.display[0].framebuffer.number = 2;
#endif
    if(display_opts[0].framebuffers !=0) {
        surface_compositor_settings.display[0].framebuffer.number = display_opts[0].framebuffers;
    }
    surface_compositor_settings.display[0].framebuffer.width = 1280;
    surface_compositor_settings.display[0].framebuffer.height = 720;
    surface_compositor_settings.display[0].framebuffer.backgroundColor = 0xFF000000; /* black background */
    surface_compositor_settings.display[0].framebuffer.heap = NEXUS_Platform_GetFramebufferHeap(0);
    if(orientation!=NEXUS_VideoOrientation_e2D) {
        surface_compositor_settings.display[0].display3DSettings.overrideOrientation = true;
        surface_compositor_settings.display[0].display3DSettings.orientation = orientation;
    }
    if (display[1]) {
        NEXUS_Display_GetGraphicsSettings(display[1], &surface_compositor_settings.display[1].graphicsSettings);
        surface_compositor_settings.display[1].graphicsSettings.enabled = true;
        surface_compositor_settings.display[1].display = display[1];
        surface_compositor_settings.display[1].framebuffer.width = 720;
        surface_compositor_settings.display[1].framebuffer.height = 480;
        surface_compositor_settings.display[1].framebuffer.backgroundColor = 0xFF000000; /* black background */
        surface_compositor_settings.display[1].framebuffer.heap = NEXUS_Platform_GetFramebufferHeap(1);
        if(display_opts[1].framebuffers !=0) {
            surface_compositor_settings.display[1].framebuffer.number = display_opts[1].framebuffers;
        }
    }
    surface_compositor_settings.frameBufferCallback.callback = framebuffer_callback;
    surface_compositor_settings.frameBufferCallback.context = surface_compositor;
    rc = NEXUS_SurfaceCompositor_SetSettings(surface_compositor, &surface_compositor_settings);
    BDBG_ASSERT(!rc);

    if(cursor.enable) {
        NEXUS_SurfaceCreateSettings settings;
        NEXUS_SurfaceCursorCreateSettings cursorSettings;
        NEXUS_SurfaceCursorSettings config;

        NEXUS_Surface_GetDefaultCreateSettings(&settings);

        settings.width = 16;
        settings.height = 16;
        settings.pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;

        cursor.surface = NEXUS_Surface_Create(&settings);
        BDBG_ASSERT(cursor.surface);
        make_cursor(cursor.surface, &settings);

        NEXUS_SurfaceCursor_GetDefaultCreateSettings(&cursorSettings);
        cursorSettings.surface = cursor.surface;
        cursor.cursor = NEXUS_SurfaceCursor_Create(surface_compositor, &cursorSettings);
        BDBG_ASSERT(cursor.cursor);
        NEXUS_SurfaceCursor_GetSettings (cursor.cursor, &config);
        config.composition.visible = true;
        config.composition.virtualDisplay.width  = 400;
        config.composition.virtualDisplay.height = 200;
        config.composition.position.x = 0;
        config.composition.position.y = 0;
        config.composition.position.width = 10;
        config.composition.position.height = 10;
        NEXUS_SurfaceCursor_SetSettings (cursor.cursor, &config);
    }

#if USE_BACKGROUND_SURFACE
    /* create desktop_client for the server */
    desktop_client = NEXUS_SurfaceCompositor_CreateClient(surface_compositor, 100);
    rc = NEXUS_SurfaceClient_SetSurface(desktop_client, desktop);
    BDBG_ASSERT(!rc);
    /* default settings make it fullscreen, zorder=0 */
#else
    BSTD_UNUSED(desktop_client);
#endif

    /* create surface for client app. it will be acquired by the client using id 1. */    
    for (i=0;i<NUM_CLIENTS;i++) {
        struct client_info *client = &g_client_transform[i];
      
        client_state[i].thread = 0;

        client_state[i].id = i;

        if(!client->enable) {
            continue;
        }

        blit_client[i] = NEXUS_SurfaceCompositor_CreateClient(surface_compositor, i);
        NEXUS_SurfaceCompositor_GetClientSettings(surface_compositor, blit_client[i], &client_settings);
        client_settings.composition.position.x = client->bounds->x;
        client_settings.composition.position.y = client->bounds->y;
        client_settings.composition.position.width = client->width;
        client_settings.composition.position.height = client->height;
        client_settings.composition.zorder = i+1; /* above desktop_client */
        client_settings.composition.displayCache = g_client_transform[i].displayCache;
        client_settings.tunnelCapable = client->tunnelCapable;
        client_settings.composition.virtualDisplay.width = 720;
        client_settings.composition.virtualDisplay.height = 480;
        if(client->client == push_lean_client ) {
            static const NEXUS_BlendEquation colorUseSourceAlpha = {
                    NEXUS_BlendFactor_eSourceColor, NEXUS_BlendFactor_eSourceAlpha, false, NEXUS_BlendFactor_eDestinationColor, NEXUS_BlendFactor_eInverseSourceAlpha, false, NEXUS_BlendFactor_eZero
            };

            static const NEXUS_BlendEquation alphaAddSourceAndDestination = {
                    NEXUS_BlendFactor_eDestinationAlpha, NEXUS_BlendFactor_eOne, false, NEXUS_BlendFactor_eZero, NEXUS_BlendFactor_eZero, false, NEXUS_BlendFactor_eSourceAlpha
            };
            client_settings.composition.colorBlend = colorUseSourceAlpha;
            client_settings.composition.alphaBlend = alphaAddSourceAndDestination;

        }
        rc = NEXUS_SurfaceCompositor_SetClientSettings(surface_compositor, blit_client[i], &client_settings);
        BDBG_ASSERT(!rc);
    }

    if(cursor.enable) {
        int urc;
        urc = pthread_create(&cursor_thread, NULL, cursor_thread_entry, NULL);
        BDBG_ASSERT(urc==0);
    }

    printf("client_flavors server is running...\n");
    
    if (randomize) {
        unsigned loops = 1000;
        while (loops--) {
            unsigned client_id = rand()%NUM_CLIENTS;
            struct client_state *state = &client_state[client_id];
            switch (rand()%2) {
            case 0:
                if (!state->thread) {
                    start_client(state);
                }
                break;
            case 1:    
                if (state->thread) {
                    stop_client(state);
                }
                break;
            }
            BKNI_Sleep(rand()%2000);
        }
        for (i=0;i<NUM_CLIENTS;i++) {
            struct client_state *state = &client_state[i];
            stop_client(state);
        }
    }
    else {
        for (i=0;i<NUM_CLIENTS;i++) {
            struct client_state *state = &client_state[i];
            start_client(state);
        }
        
        if (timeout) {
            printf("run for %d seconds...\n", timeout);
            BKNI_Sleep(timeout * 1000);
        }
        else {
            printf("press ENTER to exit\n");
            getchar();
        }
        
        for (i=0;i<NUM_CLIENTS;i++) {
            struct client_state *state = &client_state[i];
            stop_client(state);
        }
    }
    
    NEXUS_StopCallbacks(surface_compositor);
    
    for (i=0;i<NUM_CLIENTS;i++) {
        if(blit_client[i]) {
            NEXUS_SurfaceCompositor_DestroyClient(blit_client[i]);
        }
    }
#if USE_BACKGROUND_SURFACE
    NEXUS_SurfaceCompositor_DestroyClient(desktop_client);
#endif
    if(cursor.enable) {
        cursor.enable = false;
        pthread_join(cursor_thread, NULL);
        NEXUS_SurfaceCursor_Destroy(cursor.cursor);
        NEXUS_Surface_Destroy(cursor.surface);
    }

    NEXUS_SurfaceCompositor_Destroy(surface_compositor);
    NEXUS_Surface_Destroy(desktop);

    NEXUS_Graphics2D_Close(gfx);
    BKNI_DestroyEvent(event);
    NEXUS_Display_Close(display[0]);
    if (display[1]) {
        NEXUS_Display_Close(display[1]);
    }
    NEXUS_Platform_Uninit();

    return 0;
}

static int tunneled_client(struct client_state *state)
{
    NEXUS_SurfaceHandle surfaces[2];
    bool  busy[2];
    unsigned nsurfaces=2;
    unsigned current;
    NEXUS_SurfaceCreateSettings createSettings;
    NEXUS_Graphics2DHandle gfx;
    NEXUS_Graphics2DSettings gfxSettings;
    NEXUS_Graphics2DFillSettings fillSettings;
    BKNI_EventHandle checkpointEvent, packetSpaceAvailableEvent, displayedEvent;
    unsigned i;
    NEXUS_Error rc;
    NEXUS_SurfaceClientHandle tunnel_client;
    NEXUS_SurfaceClientSettings client_settings;
	int retval;
    
    BKNI_CreateEvent(&checkpointEvent);
    BKNI_CreateEvent(&packetSpaceAvailableEvent);
    BKNI_CreateEvent(&displayedEvent);

    gfx = NEXUS_Graphics2D_Open(NEXUS_ANY_ID, NULL);
    NEXUS_Graphics2D_GetSettings(gfx, &gfxSettings);
    gfxSettings.checkpointCallback.callback = complete;
    gfxSettings.checkpointCallback.context = checkpointEvent;
    gfxSettings.packetSpaceAvailable.callback = complete;
    gfxSettings.packetSpaceAvailable.context = packetSpaceAvailableEvent;
    rc = NEXUS_Graphics2D_SetSettings(gfx, &gfxSettings);
    BDBG_ASSERT(!rc);

    tunnel_client = NEXUS_SurfaceClient_Acquire(state->id);
    if (!tunnel_client) {
        BDBG_ERR(("NEXUS_SurfaceClient_Acquire %d failed, client_id %d", state->id));
        retval = -1;
		goto err_acquire;
    }
    
    NEXUS_SurfaceClient_GetSettings(tunnel_client, &client_settings);
    client_settings.displayed.callback = complete;
    client_settings.displayed.context = displayedEvent;
    rc = NEXUS_SurfaceClient_SetSettings(tunnel_client, &client_settings);
    BDBG_ASSERT(!rc);

    /* fill with black */
    for(i=0;i<nsurfaces;i++) {
        busy[i] = false;
        rc = NEXUS_SurfaceClient_AcquireTunneledSurface(tunnel_client, &surfaces[i]);
        if(rc!=NEXUS_SUCCESS) {
            break;
        }
    }
    if(i==0) {
        BDBG_ERR(("not suported number of surfaces %u", i));
        retval = -1;
		goto error;
    }
    nsurfaces = i;
    current = 0;
    NEXUS_Surface_GetCreateSettings(surfaces[current], &createSettings);
    NEXUS_Graphics2D_GetDefaultFillSettings(&fillSettings);
    fillSettings.surface = surfaces[current];
    fillSettings.rect.width = createSettings.width;
    fillSettings.rect.height = createSettings.height;
    fillSettings.color = 0;
    NEXUS_Graphics2D_Fill(gfx, &fillSettings);
    checkpoint(gfx, checkpointEvent);
    rc = NEXUS_SurfaceClient_PushTunneledSurface(tunnel_client, surfaces[current], NULL, false);
    UNIT_ASSERT(tunnel_client,!rc);
    busy[current] = true;
    rc = BKNI_WaitForEvent(displayedEvent, 5000);
    UNIT_ASSERT(tunnel_client, !rc);

    NEXUS_Graphics2D_GetDefaultFillSettings(&fillSettings); /* don't get defaults more often than necessary */
    for(i=0;!state->done;) {
        current = (current+1)%nsurfaces;
        if(busy[current] && nsurfaces>1 ) {
            unsigned j;
            for(j=0;j<nsurfaces;j++) {
                if(!busy[j]) {
                    current = j;
                    break;
                }
            }
            if(j>=nsurfaces) {
                for(j=0;j<1000;j++) {
                    NEXUS_SurfaceHandle surface;
                    size_t n;
                    rc = NEXUS_SurfaceClient_RecycleTunneledSurface(tunnel_client, &surface, 1, &n);
                    UNIT_ASSERT(tunnel_client, rc==0);
                    if(n==0) {
                        UNIT_ASSERT(tunnel_client, j<100);
                        BKNI_Sleep(10);
                        continue;
                    }
                    for(j=0;j<nsurfaces;j++) {
                        if(surfaces[j]==surface) {
                            busy[j] = false;
                            break;
                        }
                    }
                    UNIT_ASSERT(tunnel_client, j!=nsurfaces);
                    break;
                }
                continue;
            }
        }
        NEXUS_Surface_GetCreateSettings(surfaces[current], &createSettings); /* we could rely on the cache if we know display size has not changed */
        
        fillSettings.surface = surfaces[current];
        fillSettings.rect.x = 0;
        fillSettings.rect.y = 0;
        fillSettings.rect.width = createSettings.width;
        fillSettings.rect.height = createSettings.height;
        fillSettings.color = 0;
        rc = NEXUS_Graphics2D_Fill(gfx, &fillSettings);
        BDBG_ASSERT(!rc);
        
        {
            static unsigned temp_x = 0;
            if (++temp_x + 20 >= createSettings.width) {
                temp_x = 0;
            }
            fillSettings.surface = surfaces[current];
            fillSettings.rect.x = temp_x;
            fillSettings.rect.y = 0;
            fillSettings.rect.width = 20;
            fillSettings.rect.height = createSettings.height;
            fillSettings.color = 0xFFFF0000;
            rc = NEXUS_Graphics2D_Fill(gfx, &fillSettings);
            BDBG_ASSERT(!rc);
        }

        /* must do checkpoint. server uses different blitter fifo. */
        checkpoint(gfx, checkpointEvent);

        busy[current] = true;
        rc = NEXUS_SurfaceClient_PushTunneledSurface(tunnel_client, surfaces[current], NULL, false);
        BDBG_ASSERT(!rc);
        
        rc = BKNI_WaitForEvent(displayedEvent, 5000);
        BDBG_ASSERT(!rc);
        client_delay(&options.client_update);
    }
    for(i=0;i<nsurfaces;i++) {
        NEXUS_SurfaceClient_ReleaseTunneledSurface(tunnel_client, surfaces[i]);
    }

	retval = 0;

error:
    NEXUS_StopCallbacks(tunnel_client);
    NEXUS_SurfaceClient_Release(tunnel_client);
err_acquire:
    NEXUS_Graphics2D_Close(gfx);
    BKNI_DestroyEvent(displayedEvent);
    BKNI_DestroyEvent(checkpointEvent);
    BKNI_DestroyEvent(packetSpaceAvailableEvent);

    return retval;
}

static int set_client(struct client_state *state)
{
    NEXUS_SurfaceHandle surface;
    NEXUS_SurfaceMemory mem;
    NEXUS_SurfaceCreateSettings createSettings;
    NEXUS_Graphics2DHandle gfx;
    NEXUS_Graphics2DSettings gfxSettings;
    NEXUS_Graphics2DFillSettings fillSettings;
    BKNI_EventHandle checkpointEvent, packetSpaceAvailableEvent, displayedEvent;
    int i;
    NEXUS_Error rc;
    NEXUS_SurfaceClientHandle blit_client;
    NEXUS_SurfaceClientSettings client_settings;
    NEXUS_Graphics2DBlitSettings blitSettings;

    NEXUS_Surface_GetDefaultCreateSettings(&createSettings);
    createSettings.pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;
    createSettings.width = 720;
    createSettings.height = 480;
    /* createSettings.heap is NULL. proxy will populate. */
    surface = NEXUS_Surface_Create(&createSettings);
    NEXUS_Surface_GetMemory(surface, &mem);

    BKNI_CreateEvent(&checkpointEvent);
    BKNI_CreateEvent(&packetSpaceAvailableEvent);
    BKNI_CreateEvent(&displayedEvent);

    gfx = NEXUS_Graphics2D_Open(NEXUS_ANY_ID, NULL);
    NEXUS_Graphics2D_GetSettings(gfx, &gfxSettings);
    gfxSettings.checkpointCallback.callback = complete;
    gfxSettings.checkpointCallback.context = checkpointEvent;
    gfxSettings.packetSpaceAvailable.callback = complete;
    gfxSettings.packetSpaceAvailable.context = packetSpaceAvailableEvent;
    rc = NEXUS_Graphics2D_SetSettings(gfx, &gfxSettings);
    BDBG_ASSERT(!rc);

    /* draw gradient on left side of black framebuffer using synchronous blits */
#define SIDEBAR_WIDTH 100
    NEXUS_Graphics2D_GetDefaultFillSettings(&fillSettings);
    fillSettings.surface = surface;
    fillSettings.rect.width = createSettings.width;
    fillSettings.rect.height = createSettings.height;
    fillSettings.color = 0;
    rc = NEXUS_Graphics2D_Fill(gfx, &fillSettings);
    BDBG_ASSERT(!rc);
    for (i=0;i<(int)createSettings.height;i+=2) {
        fillSettings.rect.y = i;
        fillSettings.rect.width = SIDEBAR_WIDTH;
        fillSettings.rect.height = 2;
        fillSettings.color =
            (0xFF << 24) |
            (((i/2) % 0xFF) << 16) |
            (((i) % 0xFF) << 8) |
            (((i*2) % 0xFF));
        do {
        rc = NEXUS_Graphics2D_Fill(gfx, &fillSettings);
        if (rc) {
                /* if blit fails, must checkpoint and resubmit */
                BKNI_WaitForEvent(packetSpaceAvailableEvent, BKNI_INFINITE);
            }
        } while (rc);
    }
    checkpoint(gfx, checkpointEvent);

    blit_client = NEXUS_SurfaceClient_Acquire(state->id);
    if (!blit_client) {
        BDBG_ERR(("NEXUS_SurfaceClient_Acquire %d failed, client_id %d", state->id));
        return -1;
    }
    
    NEXUS_SurfaceClient_GetSettings(blit_client, &client_settings);
    client_settings.displayed.callback = complete;
    client_settings.displayed.context = displayedEvent;
    rc = NEXUS_SurfaceClient_SetSettings(blit_client, &client_settings);
    BDBG_ASSERT(!rc);

    rc = NEXUS_SurfaceClient_SetSurface(blit_client, surface);
    UNIT_ASSERT(blit_client, !rc);
    rc = BKNI_WaitForEvent(displayedEvent, 5000);
    UNIT_ASSERT(blit_client, !rc);

    NEXUS_Graphics2D_GetDefaultBlitSettings(&blitSettings);

    /* blit from left-hand gradient into the rest of the framebuffer */
    for(i=0;!state->done;) {
        unsigned j;
        for (j=0;j<50;j++) {
            blitSettings.source.surface = surface;
            blitSettings.source.rect.x = 0;
            blitSettings.source.rect.y = (i/300) % (createSettings.height - 140);
            blitSettings.source.rect.width = SIDEBAR_WIDTH;
            blitSettings.source.rect.height = 140;

            blitSettings.output.surface = surface;
            blitSettings.output.rect.x = (rand() % (createSettings.width-120)) + SIDEBAR_WIDTH;
            blitSettings.output.rect.y = (rand() % (createSettings.height-20));
            blitSettings.output.rect.width = 20;
            blitSettings.output.rect.height = 20;
            do {
                rc = NEXUS_Graphics2D_Blit(gfx, &blitSettings);
                if (rc) {
                    /* if blit fails, must checkpoint and resubmit */
                    BKNI_WaitForEvent(packetSpaceAvailableEvent, BKNI_INFINITE);
                }
            } while (rc);
            i++;
        }

        /* must do checkpoint. server uses different blitter fifo. */
        checkpoint(gfx, checkpointEvent);

        /* tell server to blit */
        rc = NEXUS_SurfaceClient_UpdateSurface(blit_client, NULL);
        BDBG_ASSERT(!rc);
        rc = BKNI_WaitForEvent(displayedEvent, 5000);
        BDBG_ASSERT(!rc);
        client_delay(&options.client_update);

    }

    NEXUS_StopCallbacks(blit_client);
    NEXUS_SurfaceClient_Release(blit_client);
    BKNI_DestroyEvent(displayedEvent);
    BKNI_DestroyEvent(checkpointEvent);
    BKNI_DestroyEvent(packetSpaceAvailableEvent);
    NEXUS_Surface_Destroy(surface);
    NEXUS_Graphics2D_Close(gfx);

    return 0;
}

static int update_client(struct client_state *state)
{
    NEXUS_SurfaceHandle surface;
    NEXUS_SurfaceMemory mem;
    NEXUS_SurfaceCreateSettings createSettings;
    NEXUS_Graphics2DHandle gfx;
    NEXUS_Graphics2DSettings gfxSettings;
    NEXUS_Graphics2DFillSettings fillSettings;
    BKNI_EventHandle checkpointEvent, packetSpaceAvailableEvent, displayedEvent, recycledEvent;
    unsigned i;
    NEXUS_Error rc;
    NEXUS_SurfaceClientHandle blit_client;
    NEXUS_SurfaceClientSettings client_settings;
    NEXUS_Graphics2DBlitSettings blitSettings;

    NEXUS_Surface_GetDefaultCreateSettings(&createSettings);
    createSettings.pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;
    createSettings.width = 720;
    createSettings.height = 480;
    /* createSettings.heap is NULL. proxy will populate. */
    surface = NEXUS_Surface_Create(&createSettings);
    NEXUS_Surface_GetMemory(surface, &mem);

    BKNI_CreateEvent(&checkpointEvent);
    BKNI_CreateEvent(&packetSpaceAvailableEvent);
    BKNI_CreateEvent(&displayedEvent);
    BKNI_CreateEvent(&recycledEvent);

    gfx = NEXUS_Graphics2D_Open(NEXUS_ANY_ID, NULL);
    NEXUS_Graphics2D_GetSettings(gfx, &gfxSettings);
    gfxSettings.checkpointCallback.callback = complete;
    gfxSettings.checkpointCallback.context = checkpointEvent;
    gfxSettings.packetSpaceAvailable.callback = complete;
    gfxSettings.packetSpaceAvailable.context = packetSpaceAvailableEvent;
    rc = NEXUS_Graphics2D_SetSettings(gfx, &gfxSettings);
    BDBG_ASSERT(!rc);

    /* clear target */
    NEXUS_Graphics2D_GetDefaultFillSettings(&fillSettings);
    fillSettings.surface = surface;
    fillSettings.rect.width = createSettings.width;
    fillSettings.rect.height = createSettings.height;
    fillSettings.color = 0;
    rc = NEXUS_Graphics2D_Fill(gfx, &fillSettings);
    BDBG_ASSERT(!rc);
    checkpoint(gfx, checkpointEvent);

    blit_client = NEXUS_SurfaceClient_Acquire(state->id);
    if (!blit_client) {
        BDBG_ERR(("NEXUS_SurfaceClient_Acquire %d failed, client_id %d", state->id));
        return -1;
    }
    
    NEXUS_SurfaceClient_GetSettings(blit_client, &client_settings);
    client_settings.displayed.callback = complete;
    client_settings.displayed.context = displayedEvent;
    client_settings.recycled.callback = complete;
    client_settings.recycled.context = recycledEvent;
    rc = NEXUS_SurfaceClient_SetSettings(blit_client, &client_settings);
    BDBG_ASSERT(!rc);

    rc = NEXUS_SurfaceClient_SetSurface(blit_client, surface);
    UNIT_ASSERT(blit_client, !rc);
    rc = BKNI_WaitForEvent(recycledEvent, 5000);
    UNIT_ASSERT(blit_client, !rc);
    rc = BKNI_WaitForEvent(displayedEvent, 5000);
    UNIT_ASSERT(blit_client, !rc);

    NEXUS_Graphics2D_GetDefaultBlitSettings(&blitSettings);

    for(i=0;!state->done;) {
        /* fill random rectangle */
        fillSettings.rect.width = (rand()%(createSettings.width/4))+1;
        fillSettings.rect.height = (rand()%(createSettings.height/4))+1;
        fillSettings.rect.x = rand() % (createSettings.width-fillSettings.rect.width);
        fillSettings.rect.y = rand() % (createSettings.height-fillSettings.rect.height);
        fillSettings.color = rand()%0xFFFFFF | 0xFF000000;
        rc = NEXUS_Graphics2D_Fill(gfx, &fillSettings);
        BDBG_ASSERT(!rc);

        /* must do checkpoint. server uses different blitter fifo. */
        checkpoint(gfx, checkpointEvent);

        /* tell server to blit */
        rc = NEXUS_SurfaceClient_UpdateSurface(blit_client, &fillSettings.rect);
        BDBG_ASSERT(!rc);
        rc = BKNI_WaitForEvent(recycledEvent, 5000);
        BDBG_ASSERT(!rc);
        client_delay(&options.client_update);
        i++;
        if(i%10000==0) {
            BDBG_LOG(("update_client:%u completed %u updates", state->id, i));
        }
    }

    NEXUS_StopCallbacks(blit_client);
    NEXUS_SurfaceClient_Release(blit_client);
    BKNI_DestroyEvent(displayedEvent);
    BKNI_DestroyEvent(checkpointEvent);
    BKNI_DestroyEvent(packetSpaceAvailableEvent);
    BKNI_DestroyEvent(recycledEvent);
    NEXUS_Surface_Destroy(surface);
    NEXUS_Graphics2D_Close(gfx);

    return 0;
}


#define NUM_SURFACES 50
struct {
    NEXUS_SurfaceHandle handle;
    bool submitted;
} g_surface[NUM_SURFACES];
unsigned submit_ptr, recycle_ptr;

static void recycle_next(NEXUS_SurfaceClientHandle blit_client);

static int push_client(struct client_state *state)
{
    NEXUS_SurfaceCreateSettings createSettings;
    NEXUS_Graphics2DHandle gfx;
    NEXUS_Graphics2DSettings gfxSettings;
    NEXUS_Graphics2DFillSettings fillSettings;
    BKNI_EventHandle checkpointEvent, packetSpaceAvailableEvent, recycledEvent;
    int i;
    NEXUS_Error rc;
    NEXUS_SurfaceClientHandle blit_client;
    NEXUS_SurfaceClientSettings client_settings;
    
    BKNI_CreateEvent(&checkpointEvent);
    BKNI_CreateEvent(&packetSpaceAvailableEvent);
    BKNI_CreateEvent(&recycledEvent);

    gfx = NEXUS_Graphics2D_Open(NEXUS_ANY_ID, NULL);
    NEXUS_Graphics2D_GetSettings(gfx, &gfxSettings);
    gfxSettings.checkpointCallback.callback = complete;
    gfxSettings.checkpointCallback.context = checkpointEvent;
    gfxSettings.packetSpaceAvailable.callback = complete;
    gfxSettings.packetSpaceAvailable.context = packetSpaceAvailableEvent;
    rc = NEXUS_Graphics2D_SetSettings(gfx, &gfxSettings);
    BDBG_ASSERT(!rc);

    NEXUS_Surface_GetDefaultCreateSettings(&createSettings);
    NEXUS_Graphics2D_GetDefaultFillSettings(&fillSettings);
    
    createSettings.pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;
    createSettings.width = 200;
    createSettings.height = 200;
    for (i=0;i<NUM_SURFACES;i++) {
        g_surface[i].handle = NEXUS_Surface_Create(&createSettings);
        
        /* green background */
        fillSettings.surface = g_surface[i].handle;
        fillSettings.rect.width = 0;
        fillSettings.rect.height = 0;
        fillSettings.color = 0xFF208020;
        rc = NEXUS_Graphics2D_Fill(gfx, &fillSettings);
        BDBG_ASSERT(!rc);
        
        /* gray white box */
        fillSettings.surface = g_surface[i].handle;
        fillSettings.rect.x = i*(createSettings.width - 20)/NUM_SURFACES;
        fillSettings.rect.y = 0;
        fillSettings.rect.width = 20;
        fillSettings.rect.height = createSettings.height;
        fillSettings.color = 0xFF333355;
        rc = NEXUS_Graphics2D_Fill(gfx, &fillSettings);
        BDBG_ASSERT(!rc);
    }
    checkpoint(gfx, checkpointEvent);

    blit_client = NEXUS_SurfaceClient_Acquire(state->id);
    if (!blit_client) {
        BDBG_ERR(("NEXUS_SurfaceClient_Acquire %d failed, client_id %d", state->id));
        return -1;
    }
    
    NEXUS_SurfaceClient_GetSettings(blit_client, &client_settings);
    client_settings.recycled.callback = complete;
    client_settings.recycled.context = recycledEvent;
    rc = NEXUS_SurfaceClient_SetSettings(blit_client, &client_settings);
    BDBG_ASSERT(!rc);

    /* push already-rendered surfaces in as fast as possible. allow display vsync to flow control. */
    for(i=0;!state->done;) {
        
        if (g_surface[submit_ptr].submitted) {
            recycle_next(blit_client);
            rc = BKNI_WaitForEvent(recycledEvent, 5000);
            BDBG_ASSERT(!rc);
            client_delay(&options.client_update);
            continue;
        }
        
        rc = NEXUS_SurfaceClient_PushSurface(blit_client, g_surface[submit_ptr].handle, NULL, false);
        BDBG_ASSERT(!rc);
        g_surface[submit_ptr].submitted = true;
        recycle_next(blit_client);
        if (++submit_ptr == NUM_SURFACES) {
            submit_ptr = 0;
        }
    }
    NEXUS_SurfaceClient_Clear(blit_client);
    for (i=0;i<NUM_SURFACES;i++) {
        NEXUS_Surface_Destroy(g_surface[i].handle);
    }
    NEXUS_StopCallbacks(blit_client);
    NEXUS_SurfaceClient_Release(blit_client);
    NEXUS_Graphics2D_Close(gfx);
    BKNI_DestroyEvent(recycledEvent);
    BKNI_DestroyEvent(checkpointEvent);
    BKNI_DestroyEvent(packetSpaceAvailableEvent);

    return 0;
}

static void recycle_next(NEXUS_SurfaceClientHandle blit_client)
{
    unsigned num;
    do {
        NEXUS_SurfaceHandle surface;
        NEXUS_Error rc;
        rc = NEXUS_SurfaceClient_RecycleSurface(blit_client, &surface, 1, &num);
        BDBG_ASSERT(!rc);
        if (num) {
            /* they should be recycled in the order submitted */
            BDBG_ASSERT(g_surface[recycle_ptr].handle == surface);
            g_surface[recycle_ptr].submitted = false;
            if (++recycle_ptr == NUM_SURFACES) {
                recycle_ptr = 0;
            }
        }
    } while (num);
}


static int push_lean_client(struct client_state *state)
{
    NEXUS_SurfaceCreateSettings createSettings;
    NEXUS_Graphics2DHandle gfx;
    NEXUS_Graphics2DSettings gfxSettings;
    NEXUS_Graphics2DFillSettings fillSettings;
    BKNI_EventHandle checkpointEvent, packetSpaceAvailableEvent, recycledEvent;
    unsigned i;
    NEXUS_Error rc;
    NEXUS_SurfaceClientHandle blit_client;
    NEXUS_SurfaceClientSettings client_settings;
    struct {
        bool busy;
        bool box_valid;
        NEXUS_Rect box;
        NEXUS_SurfaceHandle surface;
    } surfaces[2];
    const unsigned nsurfaces = sizeof(surfaces)/sizeof(surfaces[0]);
    NEXUS_Pixel background = 0x80808080; /* semitransparent gray */
    NEXUS_Rect box;
    int step_x, step_y;

    BKNI_CreateEvent(&checkpointEvent);
    BKNI_CreateEvent(&packetSpaceAvailableEvent);
    BKNI_CreateEvent(&recycledEvent);

    gfx = NEXUS_Graphics2D_Open(NEXUS_ANY_ID, NULL);
    NEXUS_Graphics2D_GetSettings(gfx, &gfxSettings);
    gfxSettings.checkpointCallback.callback = complete;
    gfxSettings.checkpointCallback.context = checkpointEvent;
    gfxSettings.packetSpaceAvailable.callback = complete;
    gfxSettings.packetSpaceAvailable.context = packetSpaceAvailableEvent;
    rc = NEXUS_Graphics2D_SetSettings(gfx, &gfxSettings);
    BDBG_ASSERT(!rc);

    NEXUS_Surface_GetDefaultCreateSettings(&createSettings);
    NEXUS_Graphics2D_GetDefaultFillSettings(&fillSettings);
    
    createSettings.pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;
    createSettings.width = 400;
    createSettings.height = 300;
    box.x = box.y = 0;
    box.width = box.height = 100;
    step_x = step_y = 1;
    for (i=0;i<nsurfaces;i++) {
        surfaces[i].busy = false;
        surfaces[i].box_valid = false;
        surfaces[i].surface = NEXUS_Surface_Create(&createSettings);
        BDBG_ASSERT(surfaces[i].surface);
        
        /* gray background */
        fillSettings.surface = surfaces[i].surface;
        fillSettings.rect.x = fillSettings.rect.y = 0;
        fillSettings.rect.width = createSettings.width;
        fillSettings.rect.height = createSettings.height;
        fillSettings.color = background;
        rc = NEXUS_Graphics2D_Fill(gfx, &fillSettings);
        BDBG_ASSERT(!rc);
    }
    checkpoint(gfx, checkpointEvent);

    blit_client = NEXUS_SurfaceClient_Acquire(state->id);
    if (!blit_client) {
        BDBG_ERR(("NEXUS_SurfaceClient_Acquire %d failed, client_id %d", state->id));
        return -1;
    }
    
    NEXUS_SurfaceClient_GetSettings(blit_client, &client_settings);
    client_settings.recycled.callback = complete;
    client_settings.recycled.context = recycledEvent;
    rc = NEXUS_SurfaceClient_SetSettings(blit_client, &client_settings);
    BDBG_ASSERT(!rc);

    for(i=0;!state->done;) {
        unsigned j;
        for (j=0;j<nsurfaces;j++) {
            if(!surfaces[j].busy) { break; }
        }
        if(j>=nsurfaces) {
            unsigned num;
            NEXUS_SurfaceHandle surface;
            rc = NEXUS_SurfaceClient_RecycleSurface(blit_client, &surface, 1, &num);
            BDBG_ASSERT(!rc);
            if(num){
                for (j=0;j<nsurfaces;j++) {
                    if(surfaces[j].surface == surface) {
                        surfaces[j].busy = false;
                        break;
                    }
                }
                BDBG_ASSERT(j<nsurfaces);
            } else {
                rc = BKNI_WaitForEvent(recycledEvent, 2000);
                BDBG_ASSERT(!rc);
            }
            continue;
        }
        fillSettings.surface = surfaces[j].surface;
        if(surfaces[j].box_valid) {
             fillSettings.rect = surfaces[j].box;
             fillSettings.color = background;
             rc = NEXUS_Graphics2D_Fill(gfx, &fillSettings);
             BDBG_ASSERT(!rc);
        }
        box.x += step_x;
        if(box.x+box.width > createSettings.width || box.x < 0) {
            step_x = -step_x;
            box.x += step_x;
        }
        box.y += step_y;
        if(box.y+box.height > createSettings.height || box.y < 0) {
            step_y = -step_y;
            box.y += step_y;
        }
        fillSettings.rect = box;
        fillSettings.color = 0xFF000000 | ((i * 251)&0xFFFFFF);
        surfaces[j].busy = true;
        surfaces[j].box_valid = true;
        surfaces[j].box = box;
        rc = NEXUS_Graphics2D_Fill(gfx, &fillSettings);
        BDBG_ASSERT(!rc);
        checkpoint(gfx, checkpointEvent);
        rc = NEXUS_SurfaceClient_PushSurface(blit_client, surfaces[j].surface, NULL, true);
        BDBG_ASSERT(!rc);
        i++;
        if(i%10000==0) {
            BDBG_LOG(("push_lean_client:%u completed %u updates", state->id, i));
        }
    }
    NEXUS_SurfaceClient_Clear(blit_client);
    for (i=0;i<nsurfaces;i++) {
        NEXUS_Surface_Destroy(surfaces[i].surface);
    }
    NEXUS_StopCallbacks(blit_client);
    NEXUS_SurfaceClient_Release(blit_client);
    NEXUS_Graphics2D_Close(gfx);
    BKNI_DestroyEvent(recycledEvent);
    BKNI_DestroyEvent(checkpointEvent);
    BKNI_DestroyEvent(packetSpaceAvailableEvent);

    return 0;
}

static int set_publish_client(struct client_state *state)
{
    NEXUS_SurfaceHandle surface;
    NEXUS_SurfaceMemory mem;
    NEXUS_SurfaceCreateSettings createSettings;
    NEXUS_Graphics2DHandle gfx;
    NEXUS_Graphics2DSettings gfxSettings;
    NEXUS_Graphics2DFillSettings fillSettings;
    BKNI_EventHandle checkpointEvent, packetSpaceAvailableEvent, displayedEvent, recycledEvent;
    unsigned i;
    NEXUS_Error rc;
    NEXUS_SurfaceClientHandle blit_client;
    NEXUS_SurfaceClientSettings client_settings;
    NEXUS_Graphics2DBlitSettings blitSettings;
    bool publish=true;

    NEXUS_Surface_GetDefaultCreateSettings(&createSettings);
    createSettings.pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;
    createSettings.width = 720;
    createSettings.height = 480;
    /* createSettings.heap is NULL. proxy will populate. */
    surface = NEXUS_Surface_Create(&createSettings);
    NEXUS_Surface_GetMemory(surface, &mem);

    BKNI_CreateEvent(&checkpointEvent);
    BKNI_CreateEvent(&packetSpaceAvailableEvent);
    BKNI_CreateEvent(&displayedEvent);
    BKNI_CreateEvent(&recycledEvent);

    gfx = NEXUS_Graphics2D_Open(NEXUS_ANY_ID, NULL);
    NEXUS_Graphics2D_GetSettings(gfx, &gfxSettings);
    gfxSettings.checkpointCallback.callback = complete;
    gfxSettings.checkpointCallback.context = checkpointEvent;
    gfxSettings.packetSpaceAvailable.callback = complete;
    gfxSettings.packetSpaceAvailable.context = packetSpaceAvailableEvent;
    rc = NEXUS_Graphics2D_SetSettings(gfx, &gfxSettings);
    BDBG_ASSERT(!rc);

    /* clear target */
    NEXUS_Graphics2D_GetDefaultFillSettings(&fillSettings);
    fillSettings.surface = surface;
    fillSettings.rect.width = createSettings.width;
    fillSettings.rect.height = createSettings.height;
    fillSettings.color = 0;
    rc = NEXUS_Graphics2D_Fill(gfx, &fillSettings);
    BDBG_ASSERT(!rc);
    checkpoint(gfx, checkpointEvent);

    blit_client = NEXUS_SurfaceClient_Acquire(state->id);
    if (!blit_client) {
        BDBG_ERR(("NEXUS_SurfaceClient_Acquire %d failed, client_id %d", state->id));
        return -1;
    }
    
    NEXUS_SurfaceClient_GetSettings(blit_client, &client_settings);
    client_settings.displayed.callback = complete;
    client_settings.displayed.context = displayedEvent;
    client_settings.recycled.callback = complete;
    client_settings.recycled.context = recycledEvent;
    rc = NEXUS_SurfaceClient_SetSettings(blit_client, &client_settings);
    BDBG_ASSERT(!rc);

    rc = NEXUS_SurfaceClient_SetSurface(blit_client, surface);
    UNIT_ASSERT(blit_client, !rc);
    rc = NEXUS_SurfaceClient_PublishSurface(blit_client);
    UNIT_ASSERT(blit_client, !rc);
    rc = BKNI_WaitForEvent(displayedEvent, 5000);
    UNIT_ASSERT(blit_client, !rc);

    NEXUS_Graphics2D_GetDefaultBlitSettings(&blitSettings);

    for(i=0;!state->done;i++) {
        unsigned x = rand() % createSettings.width;
        unsigned y = rand() % createSettings.height;
        NEXUS_Pixel *buf = (NEXUS_Pixel *)((uint8_t *)mem.buffer+y*mem.pitch);
        buf[x] = rand();
        if (i%5000==0) {
            /* flush cached memory */
            NEXUS_Surface_Flush(surface);
        }
        if (i%3000==0) {
            BKNI_Sleep(1);
        }
        if (i%70000==0) {
            publish = !publish;
            if(publish) {
                rc = NEXUS_SurfaceClient_PublishSurface(blit_client);
                UNIT_ASSERT(blit_client, !rc);
            } else {
                rc = BKNI_WaitForEvent(recycledEvent, 5000);
                UNIT_ASSERT(blit_client, !rc);
                rc = NEXUS_SurfaceClient_SetSurface(blit_client, surface);
                UNIT_ASSERT(blit_client, !rc);
           }
        }
        if (i%370000==0) {
            publish = true;
            rc = BKNI_WaitForEvent(recycledEvent, 5000);
            UNIT_ASSERT(blit_client, !rc);
            rc = NEXUS_SurfaceClient_SetSurface(blit_client, surface);
            UNIT_ASSERT(blit_client, !rc);
            rc = NEXUS_SurfaceClient_PublishSurface(blit_client);
            UNIT_ASSERT(blit_client, !rc);
        }
    }

    NEXUS_StopCallbacks(blit_client);
    NEXUS_SurfaceClient_Release(blit_client);
    BKNI_DestroyEvent(displayedEvent);
    BKNI_DestroyEvent(checkpointEvent);
    BKNI_DestroyEvent(packetSpaceAvailableEvent);
    BKNI_DestroyEvent(recycledEvent);
    NEXUS_Surface_Destroy(surface);
    NEXUS_Graphics2D_Close(gfx);

    return 0;
}


