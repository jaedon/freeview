/******************************************************************************
 *    (c)2008-2009 Broadcom Corporation
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
 * $brcm_Workfile: fillrate.c $
 * $brcm_Revision: Hydra_Software_Devel/7 $
 * $brcm_Date: 12/13/11 2:26p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/graphics2d/fillrate.c $
 * 
 * Hydra_Software_Devel/7   12/13/11 2:26p erickson
 * SW7231-535: remove workaround
 * 
 * Hydra_Software_Devel/6   12/13/11 11:31a erickson
 * SW7231-535: actually check in workaround, fix warning
 *
 * Hydra_Software_Devel/5   12/13/11 11:26a erickson
 * SW7231-535: rework for BGRC_PACKET_VERIFY_SURFACE_RECTANGLE along with
 * temp workaround.
 *
 * Hydra_Software_Devel/4   1/11/11 3:41p spothana
 * SW7420-1372 : Check for NULL return in
 * NEXUS_Platform_GetFramebufferHeap and pass on default heap for surface
 * creation.
 *
 * Hydra_Software_Devel/3   1/7/11 3:56p spothana
 * SW7420-1372: Fill rate application is using incorrect heap index for
 * creating graphics frame buffer and offscreen surfaces.
 *
 * Hydra_Software_Devel/2   9/14/10 7:43p spothana
 * SW7420-1081 : Incorrect usage of heap for 7420
 *
 * Hydra_Software_Devel/1   8/16/10 1:03p erickson
 * SW7405-3671: packet blit tests
 *
 *****************************************************************************/

#include "nexus_platform.h"
#include "nexus_surface.h"
#include "nexus_graphics2d.h"
#include "nexus_display.h"
#include "nexus_composite_output.h"
#include "nexus_component_output.h"
#include "nexus_core_utils.h"
#include "bstd.h"
#include "bkni.h"
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

BDBG_MODULE(fillrate);

static void checkpointCallback(void *data, int unused)
{
    BSTD_UNUSED(unused);
    BKNI_SetEvent((BKNI_EventHandle)data);
}

static void
asyncCheckpoint(NEXUS_Graphics2DHandle gfx, BKNI_EventHandle event)
{
    NEXUS_Error rc;
    rc=NEXUS_Graphics2D_Checkpoint(gfx, NULL);
    if(rc==NEXUS_SUCCESS) {
        return;
    }
    if(rc==NEXUS_GRAPHICS2D_QUEUED) {
        rc = BKNI_WaitForEvent(event, BKNI_INFINITE);
        BDBG_ASSERT(!rc);
        return;
    }
    BDBG_ASSERT(!rc);
    return;
}

static void run_benchmark_functions(NEXUS_Graphics2DHandle gfx, NEXUS_SurfaceHandle surface, BKNI_EventHandle checkpointEvent, BKNI_EventHandle packetEvent, unsigned width, unsigned height, unsigned id)
{
    NEXUS_Graphics2DFillSettings fillSettings;
    NEXUS_Graphics2DBlitSettings blitSettings;
    unsigned x,y;
    unsigned i;
    unsigned src_x, src_y;
    NEXUS_Pixel color;
    NEXUS_Error rc;
    NEXUS_SurfaceMemory mem;
    NEXUS_SurfaceCreateSettings createSettings;
    unsigned operations=10*1000*1000;
    BSTD_UNUSED(packetEvent);
    BSTD_UNUSED(id);

    NEXUS_Surface_GetCreateSettings(surface, &createSettings);
    NEXUS_Surface_GetMemory(surface, &mem);


    NEXUS_Graphics2D_GetDefaultFillSettings(&fillSettings);

    fillSettings.surface = surface;
    fillSettings.rect.width = width;
    fillSettings.rect.height = height;


    color = 0xFF000000;
    for(i=0,x=0,y=0;i<operations;i++) {
        unsigned step_x=width,step_y=height;
        fillSettings.rect.x = x;
        fillSettings.rect.y = y;
        fillSettings.color = color;
        x+=step_x;
        for(;;) {
            rc=NEXUS_Graphics2D_Fill(gfx, &fillSettings);
            if(rc==NEXUS_GRAPHICS2D_QUEUE_FULL) {
                asyncCheckpoint(gfx,checkpointEvent);
                BDBG_WRN(("repeating at %u", i));
            } else {
                BDBG_ASSERT(rc==NEXUS_SUCCESS);
                break;
            }
        }
        if(i%100000==0) {
            asyncCheckpoint(gfx,checkpointEvent);
            BDBG_WRN(("completed %u fills", i));
        }
        if(x+width>=createSettings.width) {
            unsigned c = i/512;
            color = 0xFF000000 | ((c*c*2287) & 0xFFFFFF);
            x= (x+width)-createSettings.width;
            y+=step_y;
            if(y+height>=createSettings.height) {
                y= (y+height)-createSettings.height;
            }
        }
    }

    NEXUS_Graphics2D_GetDefaultBlitSettings(&blitSettings);
    blitSettings.source.surface = surface;
    blitSettings.source.rect.width = width;
    blitSettings.source.rect.height = height;
    blitSettings.output.surface = surface;
    blitSettings.output.rect.width = width;
    blitSettings.output.rect.height = height;
    blitSettings.colorOp = NEXUS_BlitColorOp_eCopySource;
    blitSettings.alphaOp = NEXUS_BlitAlphaOp_eCopySource;

    for(i=0,src_x=0,x=0,src_y=1,y=0;i<operations;i++) {
        unsigned step_x=width,step_y=height;

        blitSettings.source.rect.x = src_x;
        blitSettings.source.rect.y = src_y;
        blitSettings.output.rect.x = x;
        blitSettings.output.rect.y = y;
        for(;;) {
            rc=NEXUS_Graphics2D_Blit(gfx, &blitSettings);
            if(rc==NEXUS_GRAPHICS2D_QUEUE_FULL) {
                asyncCheckpoint(gfx,checkpointEvent);
                BDBG_WRN(("repeating at %u", i));
            } else {
                BDBG_ASSERT(rc==NEXUS_SUCCESS);
                break;
            }
        }
        if(i%100000==0) {
            asyncCheckpoint(gfx,checkpointEvent);
            BDBG_WRN(("completed %u blits", i));
        }
        src_y+=step_y;
        y+=step_y;
        if(src_y+height>=createSettings.height) {
            y= (src_y+height)-createSettings.height;
            src_y = y+1;
            src_x+=step_x;
            if(src_x+width>=createSettings.width) {
                src_x= (src_x+width)-createSettings.width;
            }
            x = src_x;
        }
    }
    return;
}

#define NEXUS_GRCPACKET_SUPPORT 1
#if NEXUS_GRCPACKET_SUPPORT
#include "bm2mc_packet.h"

static void packetCallback(void *data, int unused)
{
    BSTD_UNUSED(unused);
    BKNI_SetEvent((BKNI_EventHandle)data);
}


static void
reserve_space(NEXUS_Graphics2DHandle gfx, BKNI_EventHandle event, size_t max_packet, void **buf, size_t *bufSize)
{
    for(;;) {
        NEXUS_Error rc;
        rc=NEXUS_Graphics2D_GetPacketBuffer(gfx, buf, bufSize, max_packet);
        BDBG_ASSERT(rc==NEXUS_SUCCESS);
        if(*buf==NULL) {
            BDBG_MSG(("waiting 1"));
            BKNI_WaitForEvent(event, 1000);
            continue;
        }
        BDBG_ASSERT(*bufSize>=max_packet);
        return;
    }
}

static void run_benchmark_packet(NEXUS_Graphics2DHandle gfx, NEXUS_SurfaceHandle surface, BKNI_EventHandle checkpointEvent, BKNI_EventHandle packetEvent, unsigned width, unsigned height, unsigned id)
{
    BM2MC_PACKET_PacketFillBlit *fill;
    BM2MC_PACKET_PacketCopyBlit *blit;
    BM2MC_PACKET_PacketSourceColor *sourceConstant;
    BM2MC_PACKET_PacketSourceFeeder *source;
    BM2MC_PACKET_PacketOutputFeeder *output;
    NEXUS_SurfaceMemory mem;
    void *buf;
    size_t bufSize;
    uint8_t *packet;
    uint8_t *last;
    unsigned x,y;
    unsigned src_x, src_y;
    unsigned i;
    unsigned max_packet;
    bool sync=false;
    NEXUS_Error rc;
    NEXUS_SurfaceCreateSettings createSettings;
    unsigned operations=10*1000*1000;
    NEXUS_Pixel extraColor = 0 /* 0xFF<<(8*id) */;

    NEXUS_Surface_GetMemory(surface, &mem);
    NEXUS_Surface_GetCreateSettings(surface, &createSettings);
    createSettings.pixelFormat = BM2MC_PACKET_PixelFormat_eA8_R8_G8_B8;
    rc=NEXUS_Graphics2D_GetPacketBuffer(gfx, &buf, &bufSize, 256);
    BDBG_ASSERT(rc==NEXUS_SUCCESS);
    packet = buf;
    last = packet + bufSize;

    sourceConstant = (void *)packet;
    BM2MC_PACKET_INIT(sourceConstant,SourceColor, false);
    sourceConstant->color = 0xFFFFFFFF;
    packet +=sizeof(*sourceConstant);

    {
    static const BM2MC_PACKET_Blend copyColor = {BM2MC_PACKET_BlendFactor_eSourceColor, BM2MC_PACKET_BlendFactor_eOne, false,
        BM2MC_PACKET_BlendFactor_eZero, BM2MC_PACKET_BlendFactor_eZero, false, BM2MC_PACKET_BlendFactor_eZero};
    static const BM2MC_PACKET_Blend copyAlpha = {BM2MC_PACKET_BlendFactor_eSourceAlpha, BM2MC_PACKET_BlendFactor_eOne, false,
        BM2MC_PACKET_BlendFactor_eZero, BM2MC_PACKET_BlendFactor_eZero, false, BM2MC_PACKET_BlendFactor_eZero};

    BM2MC_PACKET_PacketBlend *pPacket = (void *)packet;
    BM2MC_PACKET_INIT( pPacket, Blend, false );
    pPacket->color_blend = copyColor;
    pPacket->alpha_blend = copyAlpha;
    pPacket->color = 0;
    packet += sizeof(*pPacket);
    }

    output =  (void*)packet;
    BM2MC_PACKET_INIT(output,OutputFeeder, false);
    NEXUS_Surface_InitPlane(surface, &output->plane);
    packet +=sizeof(*output);

    for(i=1;i<2;i++) {
        fill = (void *)packet;
        BM2MC_PACKET_INIT(fill,FillBlit,true);
        fill->rect.x = 64;
        fill->rect.y = 64;
        fill->rect.width = 64*i;
        fill->rect.height = 64*i;
        packet +=sizeof(*fill);
        NEXUS_Graphics2D_PacketWriteComplete(gfx, packet-(uint8_t *)buf);
        asyncCheckpoint(gfx, checkpointEvent);
        max_packet = sizeof(*fill);
        reserve_space(gfx, packetEvent, max_packet, &buf, &bufSize);
        packet = buf;
    }

    max_packet = sizeof(*fill)+sizeof(*sourceConstant);
    for(i=0,x=0,y=0;i<operations;) {
        unsigned step_x=width,step_y=height;

        reserve_space(gfx, packetEvent, max_packet, &buf, &bufSize);
        packet = buf;
        last = packet + bufSize;

        for(;;) {
            uint8_t *next;
            next=packet+max_packet;
            if(next>last) {
                break;
            }
            i++;
            fill = (void *)packet;
            BM2MC_PACKET_INIT(fill,FillBlit,true);
            fill->rect.x = x;
            fill->rect.y = y;
            fill->rect.width = width;
            fill->rect.height = height;
            x+=step_x;
            packet += sizeof(*fill);
            if(x+width>=createSettings.width) {
                unsigned c = i/512;
                sourceConstant = (void *)packet;
                BM2MC_PACKET_INIT(sourceConstant,SourceColor, false);
                sourceConstant->color = 0xFF000000 | ((c*c*2287) & 0xFFFFFF) | extraColor;
                packet +=sizeof(*sourceConstant);

                x= (x+width)-createSettings.width;
                y+=step_y;
                if(y+height>=createSettings.height) {
                    y= (y+height)-createSettings.height;
                }
            }
            if(i%100000==0) {
                sync=true;
                BDBG_WRN(("%u:completed %u fills", id, i));
            }
        }
        BDBG_MSG(("%u:completing %u", id, packet-(uint8_t *)buf));
        NEXUS_Graphics2D_PacketWriteComplete(gfx, packet-(uint8_t *)buf);
        if(sync) {
            sync = false;
            asyncCheckpoint(gfx, checkpointEvent);
        }
    }
    reserve_space(gfx, packetEvent, sizeof(*source), &buf, &bufSize);
    source = (void *)buf;
    BM2MC_PACKET_INIT(source,SourceFeeder, false);
    NEXUS_Surface_InitPlane(surface, &source->plane);
    NEXUS_Graphics2D_PacketWriteComplete(gfx, sizeof(*source));
    max_packet = sizeof(*blit);
    for(i=0,src_x=0,x=0,src_y=1,y=0;i<operations;) {
        unsigned step_x=1,step_y=1;

        reserve_space(gfx, packetEvent, max_packet, &buf, &bufSize);
        packet = buf;
        last = packet + bufSize;

        for(;;) {
            uint8_t *next;
            next=packet+max_packet;
            if(next>last) {
                break;
            }
            i++;
            if(i%100000==0) {
                sync = true;
                BDBG_WRN(("%u:completed %u blits", id, i));
            }
            blit = (void *)packet;
            BM2MC_PACKET_INIT(blit,CopyBlit, true);
            blit->src_rect.x = src_x;
            blit->src_rect.y = src_y;
            blit->src_rect.width = width;
            blit->src_rect.height = height;
            blit->out_point.x = x;
            blit->out_point.y = y;
            src_y+=step_y;
            y+=step_y;
            if(src_y+height>=createSettings.height) {
                y= (src_y+height)-createSettings.height;
                src_y = y+1;
                src_x+=step_x;
                if(src_x+width>=createSettings.width) {
                    src_x= (src_x+width)-createSettings.width;
                }
                x = src_x;
            }
            packet += sizeof(*blit);
        }
        BDBG_MSG(("%u:completing %u", id, packet-(uint8_t *)buf));
        NEXUS_Graphics2D_PacketWriteComplete(gfx, packet-(uint8_t *)buf);
        if(sync) {
            sync = false;
            asyncCheckpoint(gfx, checkpointEvent);
        }
    }

    /* ensure blits are done before calling close */
    asyncCheckpoint(gfx, checkpointEvent);

    return;
}
#endif /* NEXUS_GRCPACKET_SUPPORT */

struct thread_context {
    pthread_t thread;
    unsigned id;
    NEXUS_SurfaceHandle surface;
    unsigned width;
    unsigned height;
};

static void *do_test(void *context_)
{
    BKNI_EventHandle checkpointEvent, packetEvent;
    struct thread_context *context = context_;
    NEXUS_Graphics2DOpenSettings openSettings;
    NEXUS_Graphics2DHandle gfx;
    NEXUS_Graphics2DSettings graphicsSettings;
    BKNI_CreateEvent(&packetEvent);
    BKNI_CreateEvent(&checkpointEvent);

    NEXUS_Graphics2D_GetDefaultOpenSettings(&openSettings);
    gfx = NEXUS_Graphics2D_Open(0, &openSettings);
    BDBG_ASSERT(gfx);
    NEXUS_Graphics2D_GetSettings(gfx, &graphicsSettings);
    graphicsSettings.checkpointCallback.callback = checkpointCallback;
    graphicsSettings.checkpointCallback.context = checkpointEvent;
    graphicsSettings.packetSpaceAvailable.callback = packetCallback;
    graphicsSettings.packetSpaceAvailable.context = packetEvent;
    NEXUS_Graphics2D_SetSettings(gfx, &graphicsSettings);

#if NEXUS_GRCPACKET_SUPPORT
    if(1) {
        run_benchmark_packet(gfx, context->surface, checkpointEvent, packetEvent, context->width, context->height, context->id);
    } else
#endif
    run_benchmark_functions(gfx, context->surface, checkpointEvent, packetEvent, context->width, context->height, context->id);

    NEXUS_Graphics2D_Close(gfx);
    BKNI_DestroyEvent(checkpointEvent);
    BKNI_DestroyEvent(packetEvent);
    return NULL;
}

#define N_CONTEXTS  4

int main(int argc, const char *argv[])
{
    NEXUS_SurfaceHandle framebuffer;
    NEXUS_SurfaceCreateSettings createSettings;
    NEXUS_DisplayHandle display;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_GraphicsSettings graphicsSettings;
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_VideoFormatInfo info;
    NEXUS_SurfaceMemory mem;
    unsigned width=1,height=1;
    struct thread_context context[4];
    unsigned i;

    /* Bring up all modules for a platform in a default configuraiton for this platform */
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    NEXUS_Display_GetDefaultSettings(&displaySettings);
    displaySettings.displayType = NEXUS_DisplayType_eAuto;
    displaySettings.format = NEXUS_VideoFormat_e720p;
    display = NEXUS_Display_Open(0, &displaySettings);
#if NEXUS_NUM_COMPONENT_OUTPUTS
    NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
#endif
#if NEXUS_NUM_HDMI_OUTPUTS
    NEXUS_Display_AddOutput(display, NEXUS_HdmiOutput_GetVideoConnector(platformConfig.outputs.hdmi[0]));
#endif

    NEXUS_VideoFormat_GetInfo(displaySettings.format, &info);

    NEXUS_Surface_GetDefaultCreateSettings(&createSettings);
    createSettings.pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;
    createSettings.width = info.width;
    createSettings.height = info.height;
    /*
     *  if NULL is returned from NEXUS_Platform_GetFramebufferHeap, then
     *   create surface will use the default heap.
     */
    createSettings.heap = NEXUS_Platform_GetFramebufferHeap(0);
    framebuffer = NEXUS_Surface_Create(&createSettings);

    if(argc>1) {
        width = strtol(argv[1], NULL, 0);
    }
    if(argc>2) {
        height = strtol(argv[2], NULL, 0);
    }

    NEXUS_Display_GetGraphicsSettings(display, &graphicsSettings);
    graphicsSettings.enabled = true;
    NEXUS_Display_SetGraphicsSettings(display, &graphicsSettings);
    NEXUS_Display_SetGraphicsFramebuffer(display, framebuffer);

    NEXUS_Surface_GetMemory(framebuffer, &mem);
    BKNI_Memset(mem.buffer, 0xC0, mem.pitch * createSettings.height);
    NEXUS_Surface_Flush(framebuffer);

    for(i=0;i<N_CONTEXTS;i++) {
        unsigned scale=0+1;
        NEXUS_Surface_GetDefaultCreateSettings(&createSettings);
        createSettings.pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;
        createSettings.width = info.width/2;
        createSettings.height = info.height/2;
        createSettings.pitch = mem.pitch;
        /*
         * This can be allocated from the graphics heap on any of
         * of the memory controllers, but for now using the memory
         * controller heap that's assigned for frame buffer.
         * if NULL is returned from NEXUS_Platform_GetFramebufferHeap, then
         *   create surface will use the default heap.
         */
        createSettings.heap = NEXUS_Platform_GetFramebufferHeap(0);
        switch(i) {
        case 0:
            createSettings.pMemory = mem.buffer;
            break;
        case 1:
            createSettings.pMemory = (uint8_t *)mem.buffer + 4*(info.width/2);
            break;
        case 2:
            createSettings.pMemory = (uint8_t *)mem.buffer + createSettings.height*mem.pitch;
            break;
        case 3:
            createSettings.pMemory = (uint8_t *)mem.buffer + createSettings.height*mem.pitch + 4*(info.width/2);
            break;
        default:
            BDBG_ASSERT(0);
        }
        context[i].surface = NEXUS_Surface_Create(&createSettings);
        context[i].id = i+1;
        context[i].width = scale*width;
        context[i].height = scale*height;
    }
    for(i=0;i<N_CONTEXTS;i++) {
        pthread_create(&context[i].thread, NULL, do_test, &context[i]);
    }
    for(i=0;i<N_CONTEXTS;i++) {
        pthread_join(context[i].thread, NULL);
        NEXUS_Surface_Destroy(context[i].surface);
    }

#if 0
    fprintf(stderr, "Done. Press Enter to exit\n");
    getchar();
#else
    /* automation */
    fprintf(stderr, "Done.\n");
#endif

    NEXUS_Display_GetGraphicsSettings(display, &graphicsSettings);
    graphicsSettings.enabled = false;
    NEXUS_Display_SetGraphicsSettings(display, &graphicsSettings);
    NEXUS_Display_Close(display);
    NEXUS_Surface_Destroy(framebuffer);
    NEXUS_Platform_Uninit();

    return 0;
}

