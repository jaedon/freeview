/******************************************************************************
 *    (c)2008-2012 Broadcom Corporation
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
 * $brcm_Workfile: packet_fill.c $
 * $brcm_Revision: 7 $
 * $brcm_Date: 1/25/12 5:22p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/examples/graphics/packet_fill.c $
 * 
 * 7   1/25/12 5:22p rjlewis
 * SW7425-1136: Added HDMI Support.
 * 
 * 6   1/13/12 7:58p erickson
 * SW7425-1136: add HDMI output, remove unused code
 * 
 * 5   5/25/11 4:12p erickson
 * SW7420-1200: align with blit and fill examples
 * 
 * 4   3/10/11 2:32p erickson
 * SW7420-1575: switch to NEXUS_Surface_InitPlane
 *
 * 3   11/29/10 2:57p erickson
 * SW7420-1078: add NEXUS_Platform_GetFramebufferHeap
 *
 * 2   8/25/10 11:30a erickson
 * SW7405-3671: fix indenting, remove bad comments
 *
 * 1   8/16/10 11:08a erickson
 * SW7405-3671: add packet examples
 *
 *****************************************************************************/

#include "nexus_platform.h"
#include "nexus_surface.h"
#if NEXUS_HAS_GRAPHICS2D
#include "nexus_graphics2d.h"
#include "bm2mc_packet.h"
#endif
#include "nexus_display.h"
#include "nexus_composite_output.h"
#include "nexus_component_output.h"
#if NEXUS_HAS_HDMI_OUTPUT
#include "nexus_hdmi_output.h"
#endif
#include "nexus_core_utils.h"
#include "bstd.h"
#include "bkni.h"
#include <stdlib.h>
#include <stdio.h>

BDBG_MODULE(packet_fill);

#if NEXUS_HAS_GRAPHICS2D
void complete(void *data, int unused)
{
    BSTD_UNUSED(unused);
    BKNI_SetEvent((BKNI_EventHandle)data);
}
#endif

int main(void)
{
#if NEXUS_HAS_GRAPHICS2D
    NEXUS_SurfaceHandle surface;
    NEXUS_SurfaceCreateSettings createSettings;
    NEXUS_DisplayHandle display;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_GraphicsSettings graphicsSettings;
    NEXUS_Graphics2DHandle gfx;
    NEXUS_Graphics2DFillSettings fillSettings;
    NEXUS_Graphics2DSettings gfxSettings;
    BKNI_EventHandle checkpointEvent, spaceAvailableEvent;
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_VideoFormatInfo info;
#if NEXUS_NUM_HDMI_OUTPUTS
    NEXUS_HdmiOutputStatus hdmiStatus;
    NEXUS_Error rc;
#endif
    int i;
    BM2MC_PACKET_Plane framebufferPlane;
    BM2MC_PACKET_Blend copyColor = {BM2MC_PACKET_BlendFactor_eSourceColor, BM2MC_PACKET_BlendFactor_eOne, false,
        BM2MC_PACKET_BlendFactor_eZero, BM2MC_PACKET_BlendFactor_eZero, false, BM2MC_PACKET_BlendFactor_eZero};
    BM2MC_PACKET_Blend copyAlpha = {BM2MC_PACKET_BlendFactor_eSourceAlpha, BM2MC_PACKET_BlendFactor_eOne, false,
        BM2MC_PACKET_BlendFactor_eZero, BM2MC_PACKET_BlendFactor_eZero, false, BM2MC_PACKET_BlendFactor_eZero};

    /* Bring up all modules for a platform in a default configuraiton for this platform */
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    NEXUS_Display_GetDefaultSettings(&displaySettings);
    displaySettings.displayType = NEXUS_DisplayType_eAuto;
    displaySettings.format = NEXUS_VideoFormat_eNtsc;
    display = NEXUS_Display_Open(0, &displaySettings);

#if NEXUS_NUM_COMPONENT_OUTPUTS
    NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
#endif
#if NEXUS_NUM_COMPOSITE_OUTPUTS
    NEXUS_Display_AddOutput(display, NEXUS_CompositeOutput_GetConnector(platformConfig.outputs.composite[0]));
#endif
#if NEXUS_NUM_HDMI_OUTPUTS
    NEXUS_Display_AddOutput(display, NEXUS_HdmiOutput_GetVideoConnector(platformConfig.outputs.hdmi[0]));
    rc = NEXUS_HdmiOutput_GetStatus(platformConfig.outputs.hdmi[0], &hdmiStatus);
    if ( !rc && hdmiStatus.connected )
    {
        /* If current display format is not supported by monitor, switch to monitor's preferred format. 
           If other connected outputs do not support the preferred format, a harmless error will occur. */
        NEXUS_Display_GetSettings(display, &displaySettings);
        if ( !hdmiStatus.videoFormatSupported[displaySettings.format] ) {
            displaySettings.format = hdmiStatus.preferredVideoFormat;
            NEXUS_Display_SetSettings(display, &displaySettings);
		}
    }
#endif

    NEXUS_VideoFormat_GetInfo(displaySettings.format, &info);

    NEXUS_Surface_GetDefaultCreateSettings(&createSettings);
    createSettings.pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;
    createSettings.width = info.width;
    createSettings.height = info.height;
    createSettings.heap = NEXUS_Platform_GetFramebufferHeap(0);
    surface = NEXUS_Surface_Create(&createSettings);
    NEXUS_Surface_InitPlane(surface, &framebufferPlane);

    BKNI_CreateEvent(&checkpointEvent);
    BKNI_CreateEvent(&spaceAvailableEvent);

    gfx = NEXUS_Graphics2D_Open(0, NULL);

    NEXUS_Graphics2D_GetSettings(gfx, &gfxSettings);
    gfxSettings.checkpointCallback.callback = complete;
    gfxSettings.checkpointCallback.context = checkpointEvent;
    gfxSettings.packetSpaceAvailable.callback = complete;
    gfxSettings.packetSpaceAvailable.context = spaceAvailableEvent;
    NEXUS_Graphics2D_SetSettings(gfx, &gfxSettings);

    /* fill with black */
    NEXUS_Graphics2D_GetDefaultFillSettings(&fillSettings);
    fillSettings.surface = surface;
    fillSettings.rect.width = createSettings.width;
    fillSettings.rect.height = createSettings.height;
    fillSettings.color = 0;
    NEXUS_Graphics2D_Fill(gfx, &fillSettings);

    NEXUS_Display_GetGraphicsSettings(display, &graphicsSettings);
    graphicsSettings.enabled = true;
    NEXUS_Display_SetGraphicsSettings(display, &graphicsSettings);
    NEXUS_Display_SetGraphicsFramebuffer(display, surface);

    /* fill random boxes */
    BDBG_WRN(("starting"));
    for(i=0;;) {
        NEXUS_Error rc;
        void *buffer, *next;
        size_t size;
        unsigned x;

        rc = NEXUS_Graphics2D_GetPacketBuffer(gfx, &buffer, &size, 1024);
        BDBG_ASSERT(!rc);
        if (!size) {
            /* internal queue is full. wait for space to become available. */
            BKNI_WaitForEvent(spaceAvailableEvent, BKNI_INFINITE);
            continue;
        }

        next = buffer;

        {
        BM2MC_PACKET_PacketOutputFeeder *pPacket = next;
        BM2MC_PACKET_INIT(pPacket, OutputFeeder, false);
        pPacket->plane = framebufferPlane;
        next = ++pPacket;
        }

        {
        BM2MC_PACKET_PacketBlend *pPacket = (BM2MC_PACKET_PacketBlend *)next;
        BM2MC_PACKET_INIT( pPacket, Blend, false );
        pPacket->color_blend = copyColor;
        pPacket->alpha_blend = copyAlpha;
        pPacket->color = 0;
        next = ++pPacket;
        }

        for (x=0;x<10;x++) {
            {
            BM2MC_PACKET_PacketSourceColor *pPacket = next;
            BM2MC_PACKET_INIT(pPacket, SourceColor, false );
            pPacket->color = (rand()%0xFFFFFF) | ((i/200)%0xFF<<24);
            next = ++pPacket;
            }

            {
            BM2MC_PACKET_PacketFillBlit *pPacket = next;
            BM2MC_PACKET_INIT(pPacket, FillBlit, true);
            pPacket->rect.x = rand() % (createSettings.width-20);
            pPacket->rect.y = rand() % (createSettings.height-20);
            pPacket->rect.width = 20;
            pPacket->rect.height = 20;
            next = ++pPacket;
            }

            i++;
        }

        rc = NEXUS_Graphics2D_PacketWriteComplete(gfx, (uint8_t*)next - (uint8_t*)buffer);
        BDBG_ASSERT(!rc);

        if (i && i%50000==0) {
            /* must do checkpoint before measurement */
            rc = NEXUS_Graphics2D_Checkpoint(gfx, NULL);
            if (rc == NEXUS_GRAPHICS2D_QUEUED) {
                BKNI_WaitForEvent(checkpointEvent, BKNI_INFINITE);
            }
            BDBG_WRN(("%d fills completed", i));
        }
        /* no flush is needed because we're not using the CPU */
    }
#else
	printf("This application is not supported on this platform!\n");
#endif

    return 0;
}
