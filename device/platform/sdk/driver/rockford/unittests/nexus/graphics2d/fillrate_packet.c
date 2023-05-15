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
 * $brcm_Workfile: fillrate_packet.c $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 10/21/10 2:23p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/graphics2d/fillrate_packet.c $
 * 
 * Hydra_Software_Devel/2   10/21/10 2:23p vsilyaev
 * SW7405-3671:  Added Nexus unittest to benchmark graphics 2D packet API
 * 
 *****************************************************************************/
/* Nexus unittest to benchmark graphics 2D packet API */

#include "nexus_platform.h"
#include "nexus_surface.h"
#include "nexus_graphics2d.h"
#include "bm2mc_packet.h"
#include "nexus_display.h"
#if NEXUS_DTV_PLATFORM
#include "nexus_platform_boardcfg.h"
#else
#include "nexus_composite_output.h"
#include "nexus_component_output.h"
#endif
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

static void packetCallback(void *data, int unused)
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
        BKNI_WaitForEvent(event, BKNI_INFINITE);
        return;
    }
    BDBG_ASSERT(rc!=NEXUS_GRAPHICS2D_BUSY);
    return;
}

static void
packetCheckpoint(NEXUS_Graphics2DHandle packet, BKNI_EventHandle event)
{
    NEXUS_Error rc;
    rc=NEXUS_Graphics2D_Checkpoint(packet, NULL);
    if(rc==NEXUS_SUCCESS) {
        return;
    }
    if(rc==NEXUS_GRAPHICS2D_QUEUED) {
        BKNI_WaitForEvent(event, BKNI_INFINITE);
        return;
    }
    BDBG_ASSERT(rc!=NEXUS_GRAPHICS2D_BUSY);
    return;
}



static void
reserve_space(NEXUS_Graphics2DHandle packetQueue, BKNI_EventHandle event, size_t max_packet, void **buf, size_t *bufSize)
{
    for(;;) {
        NEXUS_Error rc;
        rc = NEXUS_Graphics2D_GetPacketBuffer(packetQueue, buf, bufSize, max_packet);
        BDBG_ASSERT(!rc);
        if(*bufSize!=0) {
            return;
        }
        if(*buf==NULL) {
            BDBG_MSG(("waiting 1"));
            BKNI_WaitForEvent(event, 1000);
            continue;
        }
    }
}

static void run_benchmark_functions(NEXUS_Graphics2DHandle gfx, NEXUS_SurfaceHandle surface, const NEXUS_MemoryStatus *memStatus,  BKNI_EventHandle checkpointEvent, BKNI_EventHandle packetEvent, unsigned width, unsigned height, unsigned id)
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
    BSTD_UNUSED(memStatus);
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

static void run_benchmark_packet(NEXUS_Graphics2DHandle packetQueue, NEXUS_SurfaceHandle surface, const NEXUS_MemoryStatus *memStatus, BKNI_EventHandle checkpointEvent, BKNI_EventHandle packetEvent, unsigned width, unsigned height, unsigned id)
{
    BM2MC_PACKET_PacketFillBlit *fill;
    BM2MC_PACKET_PacketCopyBlit *blit;
    BM2MC_PACKET_PacketSourceColor *sourceConstant;
    BM2MC_PACKET_PacketSourceFeeder *sourcePlane;
    BM2MC_PACKET_PacketOutputFeeder *output;
    BM2MC_PACKET_PacketBlend *blend;
    static const BM2MC_PACKET_Blend copyColor = {BM2MC_PACKET_BlendFactor_eSourceColor, BM2MC_PACKET_BlendFactor_eOne, false,
        BM2MC_PACKET_BlendFactor_eZero, BM2MC_PACKET_BlendFactor_eZero, false, BM2MC_PACKET_BlendFactor_eZero};
    static const BM2MC_PACKET_Blend copyAlpha = {BM2MC_PACKET_BlendFactor_eSourceAlpha, BM2MC_PACKET_BlendFactor_eOne, false,
        BM2MC_PACKET_BlendFactor_eZero, BM2MC_PACKET_BlendFactor_eZero, false, BM2MC_PACKET_BlendFactor_eZero};
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
    BDBG_MSG(("surface %#x:%#x", memStatus->offset + (uint8_t *)mem.buffer - (uint8_t *)memStatus->addr, mem.buffer));

    rc=NEXUS_Graphics2D_GetPacketBuffer(packetQueue, &buf, &bufSize, 1024);
    BDBG_ASSERT(rc==NEXUS_SUCCESS);
    packet = buf;
    last = packet + bufSize;

    output =  (void*)packet;
    BM2MC_PACKET_INIT(output,OutputFeeder, false);
    output->plane.address = memStatus->offset + (uint8_t *)mem.buffer - (uint8_t *)memStatus->addr;
    output->plane.pitch = mem.pitch;
    output->plane.format = createSettings.pixelFormat;
    packet +=sizeof(*output);

    blend =  (void*)packet;
    BM2MC_PACKET_INIT(blend , Blend, false);
    blend->color_blend = copyColor;
    blend->alpha_blend = copyAlpha;
    blend->color = 0;
    packet +=sizeof(*blend);

    sourceConstant = (void *)packet;
    BM2MC_PACKET_INIT(sourceConstant,SourceColor, false);
    sourceConstant->color = 0xFFFFFFFF; 
    packet += sizeof(*sourceConstant);


    for(i=1;i<2;i++) {
        fill = (void *)packet;
        BM2MC_PACKET_INIT(fill,FillBlit,true);
        fill->rect.x = 64;
        fill->rect.y = 64;
        fill->rect.width = 64*i;
        fill->rect.height = 64*i;
        packet +=sizeof(*fill);
        NEXUS_Graphics2D_PacketWriteComplete(packetQueue, packet-(uint8_t *)buf); 
        packetCheckpoint(packetQueue, checkpointEvent);
        max_packet = sizeof(*fill);
        reserve_space(packetQueue, packetEvent, max_packet, &buf, &bufSize);
        packet = buf;
    }
    max_packet = sizeof(*fill)+sizeof(*sourceConstant);
    for(i=0,x=0,y=0;i<operations;) {
        unsigned step_x=width,step_y=height;

        reserve_space(packetQueue, packetEvent, max_packet, &buf, &bufSize);
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
        NEXUS_Graphics2D_PacketWriteComplete(packetQueue, packet-(uint8_t *)buf); 
        if(sync) {
            sync = false;
            packetCheckpoint(packetQueue, checkpointEvent);
        }
    }
    reserve_space(packetQueue, packetEvent, sizeof(*sourcePlane), &buf, &bufSize);
    sourcePlane= (void *)buf;
    BM2MC_PACKET_INIT(sourcePlane,SourceFeeder, false);
    sourcePlane->plane.address = memStatus->offset + (uint8_t *)mem.buffer - (uint8_t *)memStatus->addr;
    sourcePlane->plane.pitch = mem.pitch;
    sourcePlane->plane.format = createSettings.pixelFormat;
    sourcePlane->color = 0;
    NEXUS_Graphics2D_PacketWriteComplete(packetQueue, sizeof(*sourcePlane));
    max_packet = sizeof(*blit);
    for(i=0,src_x=0,x=0,src_y=1,y=0;i<operations;) {
        unsigned step_x=1,step_y=1;

        reserve_space(packetQueue, packetEvent, max_packet, &buf, &bufSize);
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
            BM2MC_PACKET_INIT(blit,CopyBlit,true);
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
        NEXUS_Graphics2D_PacketWriteComplete(packetQueue, packet-(uint8_t *)buf); 
        if(sync) {
            sync = false;
            packetCheckpoint(packetQueue, checkpointEvent);
        }
    }
    return;
}

struct thread_context {
    pthread_t thread;
    unsigned id;
    NEXUS_SurfaceHandle surface;
    const NEXUS_MemoryStatus *memStatus;
    unsigned width;
    unsigned height;
    bool packet;
};

static void *do_test(void *context_)
{
    NEXUS_Graphics2DHandle packetQueue;
    NEXUS_Graphics2DSettings packetSettings;
    BKNI_EventHandle checkpointEvent, packetEvent;
    struct thread_context *context = context_;
    BKNI_CreateEvent(&packetEvent);
    BKNI_CreateEvent(&checkpointEvent);

    packetQueue = NEXUS_Graphics2D_Open(0, NULL);
    NEXUS_Graphics2D_GetSettings(packetQueue, &packetSettings);
    packetSettings.packetSpaceAvailable.callback = packetCallback;
    packetSettings.packetSpaceAvailable.context = packetEvent;
    packetSettings.checkpointCallback.callback = checkpointCallback;
    packetSettings.checkpointCallback.context = checkpointEvent;
    NEXUS_Graphics2D_SetSettings(packetQueue, &packetSettings);

    if(context->packet) {
        run_benchmark_packet(packetQueue, context->surface, context->memStatus, checkpointEvent, packetEvent, context->width, context->height, context->id);
    } else {
        run_benchmark_functions(packetQueue, context->surface, context->memStatus, checkpointEvent, packetEvent, context->width, context->height, context->id);
    }
    NEXUS_Graphics2D_Close(packetQueue);
    BKNI_DestroyEvent(checkpointEvent);
    BKNI_DestroyEvent(packetEvent);
    return NULL;
}

#define N_CONTEXTS  4

int main(int argc, const char *argv[])
{
    NEXUS_SurfaceHandle surface;
    NEXUS_SurfaceCreateSettings createSettings;
    NEXUS_DisplayHandle display;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_GraphicsSettings graphicsSettings;
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_Graphics2DOpenSettings openSettings;
    NEXUS_VideoFormatInfo info;
    NEXUS_Error rc;
    NEXUS_MemoryStatus memStatus;
    NEXUS_HeapHandle heap;
    NEXUS_SurfaceMemory mem;
    /* NEXUS_Graphics2DSettings graphics2DSettings; */
    unsigned width=1,height=1;
    struct thread_context context[4];
    unsigned i;

    /* Bring up all modules for a platform in a default configuraiton for this platform */
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);
    NEXUS_Platform_GetConfiguration(&platformConfig);

#if NEXUS_DTV_PLATFORM
    NEXUS_Display_GetDefaultSettings(&displaySettings);
    display = NEXUS_Display_Open(0, &displaySettings);
    NEXUS_Display_AddOutput(display, NEXUS_PanelOutput_GetConnector(platformConfig.outputs.panel[0]));
    NEXUS_BoardCfg_ConfigurePanel(true, true, true);
#else
    NEXUS_Display_GetDefaultSettings(&displaySettings);
    displaySettings.displayType = NEXUS_DisplayType_eAuto;
    displaySettings.format = NEXUS_VideoFormat_e720p;
    display = NEXUS_Display_Open(0, &displaySettings);
#if NEXUS_NUM_COMPONENT_OUTPUTS
    NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
#endif
#if NEXUS_NUM_SCART_INPUTS
    NEXUS_Display_AddOutput(display, NEXUS_ScartInput_GetVideoOutputConnector(platformConfig.inputs.scart[0]));
#if NEXUS_NUM_SCART_INPUTS>1
    NEXUS_Display_AddOutput(display, NEXUS_ScartInput_GetVideoOutputConnector(platformConfig.inputs.scart[1]));
#endif
#endif
#endif

    NEXUS_VideoFormat_GetInfo(displaySettings.format, &info);

    NEXUS_Surface_GetDefaultCreateSettings(&createSettings);
    createSettings.pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;
    createSettings.width = info.width;
    createSettings.height = info.height;
    surface = NEXUS_Surface_Create(&createSettings);


    NEXUS_Graphics2D_GetDefaultOpenSettings(&openSettings);
    if(argc>1) {
        width = strtol(argv[1], NULL, 0);
    }
    if(argc>2) {
        height = strtol(argv[2], NULL, 0);
    }
#if NEXUS_DTV_PLATFORM
     BDBG_ASSERT(platformConfig.memc[0].heap[0]);
     heap = platformConfig.memc[0].heap[0];
#else
     BDBG_ASSERT(platformConfig.heap[0]);
     heap = platformConfig.heap[0];
#endif

    rc = NEXUS_Heap_GetStatus(heap, &memStatus);
    BDBG_ASSERT(rc==NEXUS_SUCCESS);
    BDBG_ERR(("heap %#x:%#x", memStatus.offset, memStatus.addr));

    NEXUS_Display_GetGraphicsSettings(display, &graphicsSettings);
    graphicsSettings.enabled = true;
    NEXUS_Display_SetGraphicsSettings(display, &graphicsSettings);
    NEXUS_Display_SetGraphicsFramebuffer(display, surface);

    NEXUS_Surface_GetMemory(surface, &mem);
    BKNI_Memset(mem.buffer, 0xC0, mem.pitch * createSettings.height); 
    NEXUS_Surface_Flush(surface); 

    for(i=0;i<N_CONTEXTS;i++) {
        unsigned scale=0+1;
        NEXUS_Surface_GetDefaultCreateSettings(&createSettings);
        createSettings.pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;
        createSettings.width = info.width/2;
        createSettings.height = info.height/2;
        createSettings.pitch = mem.pitch;
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
        context[i].memStatus = &memStatus;
        context[i].width = scale*width;
        context[i].height = scale*height;
        context[i].packet = true;
    }
    for(i=0;i<N_CONTEXTS;i++) {
        pthread_create(&context[i].thread, NULL, do_test, &context[i]);
    }
    for(i=0;i<N_CONTEXTS;i++) {
        pthread_join(context[i].thread, NULL);
        NEXUS_Surface_Destroy(context[i].surface);
    }

    fprintf(stderr, "Done. Press Enter to exit\n");
    getchar();

    NEXUS_Display_GetGraphicsSettings(display, &graphicsSettings);
    graphicsSettings.enabled = false;
    NEXUS_Display_SetGraphicsSettings(display, &graphicsSettings);
    NEXUS_Display_Close(display);
    NEXUS_Surface_Destroy(surface);
    NEXUS_Platform_Uninit();

    return 0;
}

