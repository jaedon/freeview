/***************************************************************************
 *     Copyright (c) 2004-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: b_app_client.c $
 * $brcm_Revision: 4 $
 * $brcm_Date: 6/13/12 3:28p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/bwidgets/tests/b_app_client.c $
 * 
 * 4   6/13/12 3:28p tokushig
 * SW7425-3238: update graphics surface creation for 97425
 * 
 * 3   11/3/10 12:40p erickson
 * SW7420-1148: update speed
 * 
 * 2   10/27/10 11:17a erickson
 * SW7420-1148: update
 * 
 * 1   10/26/10 11:02a erickson
 * SW7420-1148: create client/server gfx app
 * 
 * 2   10/22/10 3:21p erickson
 * SWDEPRECATED-626: revive bwidgets on nexus
 * 
 ****************************************************************/
#include "b_app_client.h"
#include "b_app_server.h"
#include "nexus_platform.h"
#include "nexus_core_utils.h"
#include "bwin.h"
#include <assert.h>
#include <stdio.h>
#include <errno.h>
#include <pthread.h>
#include <string.h>

BDBG_MODULE(b_app_client);

static void setevent_callback(void *context, int param)
{
    BSTD_UNUSED(param);
    BKNI_SetEvent((BKNI_EventHandle)context);
}

void checkpoint(const struct b_offscreen_status *status)
{
    int rc;
    rc = NEXUS_Graphics2D_Checkpoint(status->blitter, NULL);
    if (rc == NEXUS_GRAPHICS2D_QUEUED) {
        BKNI_WaitForEvent(status->checkpoint_event, 0xffffffff);
    }
}

static void do_sync(bwin_framebuffer_t fb)
{
    NEXUS_Error rc;
    bwin_framebuffer_settings settings;
    struct b_offscreen_status *status;
    NEXUS_SurfaceHandle output;
    NEXUS_Graphics2DBlitSettings blitSettings;
    
    bwin_get_framebuffer_settings(fb, &settings);
    status = settings.data;
    output = status->back_surface[status->current_back_surface];
    
    /* flush */
    NEXUS_Surface_Flush(output);
    
    /* server: blit */
    NEXUS_Graphics2D_GetDefaultBlitSettings(&blitSettings);
    blitSettings.source.surface = status->surface;
    blitSettings.output.surface = output;
    rc = NEXUS_Graphics2D_Blit(status->blitter, &blitSettings);
    BDBG_ASSERT(!rc);
    
    if (status->server) {
        checkpoint(status);
        
        /* server: display */
        rc = NEXUS_Display_SetGraphicsFramebuffer(status->server->display, output);
        BDBG_ASSERT(!rc);
    }
    
    if (status->sync) {
        (status->sync)(status->context);
    }
    
    /* flip */
    status->current_back_surface = 1 - status->current_back_surface;
}

int b_init_offscreen(unsigned width, unsigned height, unsigned backwidth, unsigned backheight, struct b_offscreen_status *status)
{
    NEXUS_SurfaceMemory mem;
    NEXUS_SurfaceCreateSettings createSettings;
    NEXUS_Graphics2DSettings gfxSettings;
    NEXUS_Graphics2DFillSettings fillSettings;
    bwin_framebuffer_settings fb_settings;
    bwin_engine_settings win_engine_settings;
    NEXUS_Error rc;
    
    BKNI_Memset(status, 0, sizeof(*status));
    
    status->blitter = NEXUS_Graphics2D_Open(0, NULL);
    rc = BKNI_CreateEvent(&status->checkpoint_event);
    BDBG_ASSERT(!rc);
    NEXUS_Graphics2D_GetSettings(status->blitter, &gfxSettings);
    gfxSettings.checkpointCallback.callback = setevent_callback;
    gfxSettings.checkpointCallback.context = status->checkpoint_event;
    rc = NEXUS_Graphics2D_SetSettings(status->blitter, &gfxSettings);
    BDBG_ASSERT(!rc);
    
    NEXUS_Surface_GetDefaultCreateSettings(&createSettings);
    createSettings.pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;
    createSettings.width = width;
    createSettings.height = height;
    createSettings.heap = NEXUS_Platform_GetFramebufferHeap(0);
    status->surface = NEXUS_Surface_Create(&createSettings);
    NEXUS_Surface_GetMemory(status->surface, &mem);
    NEXUS_Graphics2D_GetDefaultFillSettings(&fillSettings);
    fillSettings.surface = status->surface;
    fillSettings.color = 0;
    rc = NEXUS_Graphics2D_Fill(status->blitter, &fillSettings);
    BDBG_ASSERT(!rc);
    
    checkpoint(status);
    
    /* a double-buffered back buffer used on the server for interaction with GFD, or used on 
    the client for interaction with the server */
    NEXUS_Surface_GetDefaultCreateSettings(&createSettings);
    createSettings.pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;
    createSettings.width = backwidth;
    createSettings.height = backheight;
    createSettings.heap = NEXUS_Platform_GetFramebufferHeap(NEXUS_OFFSCREEN_SURFACE);
    status->back_surface[0] = NEXUS_Surface_Create(&createSettings);
    status->back_surface[1] = NEXUS_Surface_Create(&createSettings);
    
    /* bring up bwin with a single framebuffer */
    bwin_engine_settings_init(&win_engine_settings);
    status->win_engine = bwin_open_engine(&win_engine_settings);
    BDBG_ASSERT(status->win_engine);

    status->default_font = bwin_open_font(status->win_engine, "fonts/arial_18_aa.bwin_font", -1, true);
    /* don't assume that the font was created. operate anyway. */
    if (!status->default_font) {
        BDBG_ERR(("Unable to create font"));
    }

    printf("bwin offscreen: framebuffer %p, %dx%d, pitch %d\n", mem.buffer, width, height, mem.pitch);
    bwin_framebuffer_settings_init(&fb_settings);
    fb_settings.width = width;
    fb_settings.height = height;
    fb_settings.pitch = mem.pitch;
    fb_settings.buffer = mem.buffer;
    /* do not set fb_settings.second_buffer */
    fb_settings.pixel_format = bwin_pixel_format_a8_r8_g8_b8;
    fb_settings.drawops.sync = do_sync;
    fb_settings.data = status;
    status->win_framebuffer = bwin_open_framebuffer(status->win_engine, &fb_settings);
    BDBG_ASSERT(status->win_framebuffer);

    bwin_get_framebuffer_settings(status->win_framebuffer, &fb_settings);
    status->win_framebuffer_window = fb_settings.window;
    
    return 0;
}

void b_uninit_offscreen(const struct b_offscreen_status *status)
{
    NEXUS_Graphics2D_Close(status->blitter);
    BKNI_DestroyEvent(status->checkpoint_event);
    bwin_close_framebuffer(status->win_framebuffer);
    bwin_close_engine(status->win_engine);
    if (status->surface) {
        NEXUS_Surface_Destroy(status->surface);
    }
    if (status->back_surface[0]) {
        NEXUS_Surface_Destroy(status->back_surface[0]);
    }
    if (status->back_surface[1]) {
        NEXUS_Surface_Destroy(status->back_surface[1]);
    }
}

void b_get_surface_data(const struct b_offscreen_status *status, b_message *msg)
{
    NEXUS_SurfaceMemory mem[2];
    NEXUS_SurfaceCreateSettings settings;
    NEXUS_MemoryStatus memStatus;
    struct b_message_report_offscreen_data *data;
    
    b_message_init(msg);
    msg->type = b_message_type_report_offscreen;
    msg->datasize = sizeof(*data);
    data = (struct b_message_report_offscreen_data *)msg->data; /* map it */
    
    NEXUS_Surface_GetMemory(status->back_surface[0], &mem[0]);
    NEXUS_Surface_GetMemory(status->back_surface[1], &mem[1]);
    NEXUS_Surface_GetCreateSettings(status->back_surface[0], &settings);
    
    /* assume all surfaces on the default heap */
    if (!settings.heap) {
        NEXUS_PlatformConfiguration platformConfig;
        NEXUS_Platform_GetConfiguration(&platformConfig);
        settings.heap = platformConfig.heap[0];
    }
    (void)NEXUS_Heap_GetStatus(settings.heap, &memStatus);
    
    data->offset[0] = memStatus.offset + (uint8_t*)mem[0].buffer - (uint8_t*)memStatus.addr;
    data->offset[1] = memStatus.offset + (uint8_t*)mem[1].buffer - (uint8_t*)memStatus.addr;
    data->width = settings.width;
    data->height = settings.height;
    data->pitch = mem[0].pitch;
    data->pixelFormat = settings.pixelFormat;
}

NEXUS_Error b_create_surfaces(const struct b_message_report_offscreen_data *data, NEXUS_SurfaceHandle *pSurface0, NEXUS_SurfaceHandle *pSurface1)
{
    NEXUS_SurfaceCreateSettings settings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_MemoryStatus memStatus;
    
    NEXUS_Platform_GetConfiguration(&platformConfig);
    (void)NEXUS_Heap_GetStatus(platformConfig.heap[0], &memStatus);
    
    NEXUS_Surface_GetDefaultCreateSettings(&settings);
    settings.width = data->width;
    settings.height = data->height;
    settings.pitch = data->pitch;
    settings.pixelFormat = data->pixelFormat;
    settings.pMemory = data->offset[0] - memStatus.offset + (uint8_t*)memStatus.addr;
    *pSurface0 = NEXUS_Surface_Create(&settings);
    
    settings.pMemory = data->offset[1] - memStatus.offset + (uint8_t*)memStatus.addr;
    *pSurface1 = NEXUS_Surface_Create(&settings);
    
    return 0;
}

/**************************************************/

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/poll.h>
#include <sys/un.h>
#include <unistd.h>

int b_connect_to_server()
{
    struct sockaddr_un sock_addr;
    int rc;
    int fd;
    
    fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd == -1) {
        return BERR_TRACE(errno);
    }
 
    BKNI_Memset(&sock_addr, 0, sizeof(sock_addr));
    sock_addr.sun_family = AF_UNIX;
    strcpy(sock_addr.sun_path, B_UNIX_SOCKET_NAME);
    rc = connect(fd, (struct sockaddr *)&sock_addr, strlen(sock_addr.sun_path)+sizeof(sock_addr.sun_family));
    if (rc == -1) {
        rc = BERR_TRACE(errno);
        goto error;
    }
    
    printf("client connected to server\n");
    return fd;
 
error:
    close(fd);
    return -1;
}

void b_disconnect_from_server(int fd)
{
    shutdown(fd, SHUT_RDWR);
    close(fd);
}

#define MSG_HEADER_SIZE(msg) ((uint8_t*)(msg)->data - (uint8_t*)(msg))

void b_message_init(b_message *msg)
{
    /* don't init data. */
    BKNI_Memset(msg, 0, MSG_HEADER_SIZE(msg));
}

int b_recv_message(int fd, b_message *msg)
{
    ssize_t n;
    
    n = read(fd, msg, B_MAX_MSG);
    if (n < 0) return -1;
    
    if (n < MSG_HEADER_SIZE(msg)) {
        return -1;
    }
    msg->datasize = n - MSG_HEADER_SIZE(msg);
    if (msg->datasize > B_MAX_MSG) {
        msg->datasize = B_MAX_MSG;
    }
    
    return 0;
}

int b_send_message(int fd, const b_message *msg)
{
    int rc;
    rc = write(fd, msg, MSG_HEADER_SIZE(msg) + msg->datasize);
    if (rc == -1) {
        if (errno == EAGAIN || errno == EINTR) {
            /* TODO: loop until sent */
        }
    }
    
    return 0;
}

