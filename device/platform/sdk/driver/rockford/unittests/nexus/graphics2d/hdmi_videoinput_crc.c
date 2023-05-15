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
 * $brcm_Workfile: hdmi_videoinput_crc.c $
 * $brcm_Revision: Hydra_Software_Devel/3 $
 * $brcm_Date: 9/24/12 2:42p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/graphics2d/hdmi_videoinput_crc.c $
 * 
 * Hydra_Software_Devel/3   9/24/12 2:42p ahulse
 * SW7425-3321: update crc's due to changes in SW7425-2172
 * 
 * Hydra_Software_Devel/2   8/23/12 8:52a cng
 * SW7425-3321: Close Display when starting up new format testing.
 * 
 * Hydra_Software_Devel/1   7/30/12 8:25a cng
 * SW7425-3321: Test for crc in hdmi, video and graphics plane with
 * different sets of color patterns
 * 
******************************************************************************/

/*
 * hdmi_videoinput_crc.c 
 *
 * You can choose to check for either vdc and/or hdmi crc in the command line. 
 * Display format can be specified, otherwise the test will loop through all
 * available formats. 
 *
 */

     
#if NEXUS_HAS_HDMI_OUTPUT
#define USAGE "\n*************************************************\n\
USAGE-> Refer hdmi_videoinput_crc.c for details\n\n\
Command: hdmi_videoinput_crc [-hdmi] [-video] [-graphics] [-format <fmt>]\n\n\
Options: fmt\n\
\tvideo/graphics = { 480p | 576p | 720p | 1080p | 720p50hz } \n\n\
\thdmi           = { 480p | 720p | 1080p | 1080i } \n\n\
Example: # nexus hdmi_videoinput_crc -video -format 1080p\n\
         # nexus hdmi_videoinput_crc -hdmi \n\
         # nexus hdmi_videoinput_crc -graphics\n\
*************************************************\n"
#else
#define USAGE "\n*************************************************\n\
USAGE-> Refer hdmi_videoinput_crc.c for details\n\n\
Command: hdmi_videoinput_crc [-video] [-graphics] [-format <fmt>]\n\n\
Options: fmt\n\
\tvideo/graphics = { 480p | 576p | 720p | 1080p | 720p50hz } \n\n\
Example: # nexus hdmi_videoinput_crc -video -format 1080p\n\
         # nexus hdmi_videoinput_crc -graphics\n\
*************************************************\n"
#endif
 
#include "bstd.h"
#include "bkni.h"
#include "nexus_types.h"
#include "nexus_platform.h"
#include "nexus_display.h"
#include "nexus_component_output.h"
#include "nexus_composite_output.h"
#include "nexus_core_utils.h"
#if NEXUS_HAS_HDMI_OUTPUT
#include "nexus_hdmi_output.h"
#endif
#include "nexus_graphics2d.h"
#include "nexus_video_output.h"
#include "nexus_video_window.h"
#include "nexus_video_image_input.h"
#include "nexus_video_input.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

BDBG_MODULE(hdmi_videoinput_crc);

#include "../common/vdcCrc.h"


static void complete(void *data, int unused) 
{
    BSTD_UNUSED(unused);
    BKNI_SetEvent((BKNI_EventHandle)data);
}

typedef struct goldenCrc
{
    NEXUS_DisplayCrcData *vdcCrc;
    NEXUS_HdmiOutputCrcData *hdmiCrc;
} goldenCrc;

typedef struct fmtStructure
{
    NEXUS_VideoFormat format;
    char* name;
    bool supported;
} fmtStructure;

typedef struct crcHdmiTestVectors
{
        colorPatterns idx;
        NEXUS_VideoFormat vidFormat;
        NEXUS_HdmiOutputCrcData data;
} crcHdmiTestVectors;

#define NR_HDMI_FORMATS 4 /* change if change number of supported formats, eg now support 480p,720p,1080p,1080i so is 4 now */
/* Golden crc values when only using graphics plane */
crcHdmiTestVectors goldenHdmiGfxVectors[] = 
{
    /* BLACK_FIELD */
    { BLACK_FIELD, NEXUS_VideoFormat_e480p,   {0x7425} },
    { BLACK_FIELD, NEXUS_VideoFormat_e720p,   {0xe291} },
    { BLACK_FIELD, NEXUS_VideoFormat_e1080p,  {0xc444} },
    { BLACK_FIELD, NEXUS_VideoFormat_e1080i,  {0x3247} },
    
    /* COLOR_BAR */
    { COLOR_BAR, NEXUS_VideoFormat_e480p,     {0x6597} },
    { COLOR_BAR, NEXUS_VideoFormat_e720p,     {0x942d} },
    { COLOR_BAR, NEXUS_VideoFormat_e1080p,    {0x4b85} },
    { COLOR_BAR, NEXUS_VideoFormat_e1080i,    {0x832d} },
    
    /* RED_FIELD */
    { RED_FIELD, NEXUS_VideoFormat_e480p,     {0xfa0e} },
    { RED_FIELD, NEXUS_VideoFormat_e720p,     {0x8aa0} },
    { RED_FIELD, NEXUS_VideoFormat_e1080p,    {0x1c39} },
    { RED_FIELD, NEXUS_VideoFormat_e1080i,    {0xe50c} },
    
    /* GREEN_FIELD */
    { GREEN_FIELD, NEXUS_VideoFormat_e480p,   {0x52ee} },
    { GREEN_FIELD, NEXUS_VideoFormat_e720p,   {0xbb5b} },
    { GREEN_FIELD, NEXUS_VideoFormat_e1080p,  {0x76a3} },
    { GREEN_FIELD, NEXUS_VideoFormat_e1080i,  {0xd8ab} },
    
    /* BLUE_FIELD */
    { BLUE_FIELD, NEXUS_VideoFormat_e480p,    {0xb0a6} },
    { BLUE_FIELD, NEXUS_VideoFormat_e720p,    {0x945} },
    { BLUE_FIELD, NEXUS_VideoFormat_e1080p,   {0x4d8} },
    { BLUE_FIELD, NEXUS_VideoFormat_e1080i,   {0xc4a5} },
};

/* Golden crc values when using image input */
crcHdmiTestVectors goldenHdmiVideoVectors[] = 
{
    /* BLACK_FIELD */
    { BLACK_FIELD, NEXUS_VideoFormat_e480p,   {0x7425} },
    { BLACK_FIELD, NEXUS_VideoFormat_e720p,   {0xe291} },
    { BLACK_FIELD, NEXUS_VideoFormat_e1080p,  {0xc444} },
    { BLACK_FIELD, NEXUS_VideoFormat_e1080i,  {0x3247} },
    
    /* COLOR_BAR */
    { COLOR_BAR, NEXUS_VideoFormat_e480p,     {0x1ad4} }, 
    { COLOR_BAR, NEXUS_VideoFormat_e720p,     {0x942d} }, 
    { COLOR_BAR, NEXUS_VideoFormat_e1080p,    {0x4b85} },
    { COLOR_BAR, NEXUS_VideoFormat_e1080i,    {0x832d} },
    
    /* RED_FIELD */
    { RED_FIELD, NEXUS_VideoFormat_e480p,     {0x6bc0} }, 
    { RED_FIELD, NEXUS_VideoFormat_e720p,     {0xe3bb} },
    { RED_FIELD, NEXUS_VideoFormat_e1080p,    {0xf051} },
    { RED_FIELD, NEXUS_VideoFormat_e1080i,    {0xe50c} },
    
    /* GREEN_FIELD */
    { GREEN_FIELD, NEXUS_VideoFormat_e480p,   {0x85db} },
    { GREEN_FIELD, NEXUS_VideoFormat_e720p,   {0x2491} },
    { GREEN_FIELD, NEXUS_VideoFormat_e1080p,  {0x96ef} },
    { GREEN_FIELD, NEXUS_VideoFormat_e1080i,  {0xd8ab} },
    
    /* BLUE_FIELD */
    { BLUE_FIELD, NEXUS_VideoFormat_e480p,    {0xa0f2} },
    { BLUE_FIELD, NEXUS_VideoFormat_e720p,    {0x708d} },
    { BLUE_FIELD, NEXUS_VideoFormat_e1080p,   {0x920f} },
    { BLUE_FIELD, NEXUS_VideoFormat_e1080i,   {0xc4a5} },
};

/* Collects and Checks Hdmi CRC Data */
int displayHdmiCrc( NEXUS_HdmiOutputHandle hdmiOutput, const uint16_t refCrc ) 
{
/* Poll vdc 6 times to get crc - if increase this, CRC_QUEUE_SIZE may need adjusting */
#define POLL_TIMES 6
#define CRC_QUEUE_SIZE 600

    NEXUS_HdmiOutputSettings hdmiSettings;
    NEXUS_HdmiOutputCrcData data[16];
    NEXUS_Error rc;
    unsigned num, i;
    int j;
    
    NEXUS_HdmiOutput_GetSettings( hdmiOutput, &hdmiSettings);
    hdmiSettings.crcQueueSize = CRC_QUEUE_SIZE;      /* Crc enabled when set non-zero */  
    NEXUS_HdmiOutput_SetSettings( hdmiOutput, &hdmiSettings);

    for( i = 0 ; i < POLL_TIMES ; i++ ) 
    {
        num = 0;
        while ( !num )
        {
            rc = NEXUS_HdmiOutput_GetCrcData( hdmiOutput, data, 16, &num );
            BDBG_ASSERT(!rc);
            if (!num) {
                BKNI_Sleep(10);
                continue;
            }
#if 0
            for ( j=0; j<(int)num; j++ ) 
            {
                BDBG_MSG(("\t Hdmi Crc: %#x\n", data[j].crc ));
            }
#else
            j = num;
#endif
        }
        BKNI_Sleep(16);
    
    }
    
    if ( j ) j--;
    
    NEXUS_HdmiOutput_GetSettings( hdmiOutput, &hdmiSettings);
    hdmiSettings.crcQueueSize = 0;        
    NEXUS_HdmiOutput_SetSettings( hdmiOutput, &hdmiSettings);
    
    BDBG_MSG(( "\n\t Hdmi Crc: %#x\t Golden Crc: %#x\n", data[j].crc, refCrc ));

    if ( data[j].crc == refCrc ) 
    {
        return 0;
    }
    else 
    {
        printf( "\n>>>FAIL with Hdmi Crc: %#x\t Golden Crc: %#x\n", data[j].crc, refCrc );
        return 1;
    }
    
}

/* Collects Reference CRC values */
int getGoldenCrc( const crc_context *context, const uint32_t pattern, goldenCrc *crcRef )
{
    NEXUS_DisplayCrcData vdcData;
    NEXUS_HdmiOutputCrcData hdmiData;
    NEXUS_DisplaySettings displaySettings;
    int formatIdx;
    
    NEXUS_Display_GetSettings( context->display, &displaySettings );
    
    switch ( displaySettings.format ) 
    {
        case NEXUS_VideoFormat_e480p :    formatIdx = 0; break;
        case NEXUS_VideoFormat_e720p :    formatIdx = 1; break;
        case NEXUS_VideoFormat_e1080p:    formatIdx = 2; break;
        case NEXUS_VideoFormat_e576p:     formatIdx = 3; break;
        case NEXUS_VideoFormat_e1080i:    formatIdx = 3; break;
        case NEXUS_VideoFormat_e720p50hz: formatIdx = 4; break;
        default  : 
            formatIdx = 0; 
            printf("\nNo golden crc for this format, skip test for this format.\n\n"); 
            return 1;
    }
    
    BKNI_Memset( &vdcData, 0, sizeof(NEXUS_DisplayCrcData) );
    crcRef->vdcCrc = &vdcData;
    
    BKNI_Memset( &hdmiData, 0, sizeof(NEXUS_HdmiOutputCrcData) );
    crcRef->hdmiCrc = &hdmiData;
    
    if ( context->imageInput ) 
    {
        crcRef->vdcCrc = &(goldenVideoVectors[(pattern*NR_FORMATS)+formatIdx].data);
        crcRef->hdmiCrc = &(goldenHdmiVideoVectors[(pattern*NR_HDMI_FORMATS)+formatIdx].data);
    }
    else 
    {
        crcRef->vdcCrc = &(goldenGfxVectors[(pattern*NR_FORMATS)+formatIdx].data);
        crcRef->hdmiCrc = &(goldenHdmiGfxVectors[(pattern*NR_HDMI_FORMATS)+formatIdx].data);
    }
    BDBG_MSG(( "pattern=%d formatIdx=%d" , pattern, formatIdx ));
    
    return 0;
}

/* Display different patterns */ 
int drawImage( crc_context *context, const uint32_t pattern ) 
{
    NEXUS_Graphics2DFillSettings fillSettings;
    NEXUS_DisplaySettings   displaySettings;
    NEXUS_VideoFormatInfo   info;
    NEXUS_Error rc;
    int i;
    uint32_t fillColor = pattern;
    NEXUS_SurfaceHandle  img, tmpImg;
    NEXUS_SurfaceCreateSettings  surfaceCfg;
    NEXUS_GraphicsSettings gfxSettings;

    NEXUS_Display_GetSettings( context->display, &displaySettings );

    NEXUS_VideoFormat_GetInfo( displaySettings.format, &info);

    /* Make sure destination is clear by blitting black to it */
    NEXUS_Graphics2D_GetDefaultFillSettings(&fillSettings);

    if ( context->imageInput ) 
    {
        NEXUS_Surface_GetDefaultCreateSettings(&surfaceCfg);
        surfaceCfg.width  = info.width;
        surfaceCfg.height = info.height;
        surfaceCfg.pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;
        tmpImg = NEXUS_Surface_Create(&surfaceCfg);
        BDBG_ASSERT(tmpImg);
        img = tmpImg;
    }
    else 
    {
        NEXUS_Surface_Destroy(context->framebuffer);
        NEXUS_Surface_GetDefaultCreateSettings(&surfaceCfg);
        surfaceCfg.width  = info.width;
        surfaceCfg.height = info.height;
        surfaceCfg.pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;
        surfaceCfg.heap = NEXUS_Platform_GetFramebufferHeap(0);
        context->framebuffer = NEXUS_Surface_Create(&surfaceCfg);
        BDBG_ASSERT(context->framebuffer);
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
    if (rc == NEXUS_GRAPHICS2D_QUEUED) 
    {
        rc = BKNI_WaitForEvent(context->checkpointEvent, BKNI_INFINITE);
    }
    BDBG_ASSERT(!rc);

    if ( context->imageInput ) 
    {
        /* Make sure image input layer is clear by blitting black to it */
        setImageInput( context , img , fillSettings.rect );
    }
    if ( pattern < PATTERN_eMAX ) 
    {
        switch ( pattern ) 
        {
            case BLACK_FIELD: 
                fillColor = 0x0; /*0xFF000000*/
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
    }

    NEXUS_Graphics2D_GetDefaultFillSettings(&fillSettings);
    if ( context->imageInput ) 
    {
        fillSettings.surface = tmpImg;
        NEXUS_VideoImageInput_SetSurface(context->imageInput, NULL );  /* release image */
    }
    else 
    {
        fillSettings.surface = img;
    }
    
    if ( pattern == 1 ) 
    {
        /* draw for the HD display */
        fillSettings.rect.y = 0;
        fillSettings.rect.height = info.height;
        fillSettings.rect.width = info.width/NUM_COLORS;
        for (i=0;i<NUM_COLORS;i++) {
            fillSettings.rect.x = fillSettings.rect.width * i;
            fillSettings.color = g_colors[i];
            BDBG_MSG(( "Fill i=%d x=%d w=%d with color=0x%8x" , i, fillSettings.rect.x, fillSettings.rect.width, g_colors[i] ));
            NEXUS_Graphics2D_Fill(context->gfx, &fillSettings);
        }
        fillSettings.rect.width = info.width;
    }
    else 
    {
        /* Flat field display */
        fillSettings.rect.width = info.width;
        fillSettings.rect.y = 0;
        fillSettings.rect.height = info.height;
        fillSettings.color = fillColor;
        NEXUS_Graphics2D_Fill(context->gfx, &fillSettings);
    }

    /* needed to execute queued blits */
    rc = NEXUS_Graphics2D_Checkpoint(context->gfx, NULL);
    if (rc == NEXUS_GRAPHICS2D_QUEUED) 
    {
        rc = BKNI_WaitForEvent(context->checkpointEvent, BKNI_INFINITE);
    }
    BDBG_ASSERT(!rc);
    
    if ( context->imageInput ) 
    {
        BDBG_MSG(("SET IMAGE TO PATTERN"));
        setImageInput( context , tmpImg, fillSettings.rect );
        NEXUS_Surface_Destroy(tmpImg);
    }
    else 
    {
        NEXUS_Display_GetGraphicsSettings(context->display, &gfxSettings);
        gfxSettings.enabled = true;
        NEXUS_Display_SetGraphicsSettings(context->display, &gfxSettings);
        NEXUS_Display_SetGraphicsFramebuffer(context->display, img);
    }
    
    return rc;
}

static NEXUS_HeapHandle b_get_image_input_heap(unsigned imageInputIndex) 
{
    NEXUS_PlatformSettings platformSettings;
    unsigned videoDecoderIndex, heapIndex;
    NEXUS_PlatformConfiguration platformConfig;

    /* The imageInput index is the same as the MPEG feeder (MFD) index. we need to find a heap that is RTS-accessible by this MFD.
    This can be learned from the VideoDecoderModule because the MFD normally reads from AVD picture buffers. */

    /* first, get platform settings, which gives us VideoDecoderModuleSettings */
    NEXUS_Platform_GetSettings(&platformSettings);

    /* next, find the VideoDecoder index that uses this MFD */
    for (videoDecoderIndex=0;videoDecoderIndex<NEXUS_NUM_VIDEO_DECODERS;videoDecoderIndex++) 
    {
        if (platformSettings.videoDecoderModuleSettings.mfdMapping[videoDecoderIndex] == imageInputIndex) 
        {
            /* we've found a video decoder that uses this MFD */
            break;
        }
    }
    if (videoDecoderIndex == NEXUS_NUM_VIDEO_DECODERS) 
    {
        /* this MFD is unused by VideoDecoder, so we can't know the heap */
        return NULL;
    }

    /* now, get the heap index for this video decoder. the MFD must be able to read from this heap
    if it's able to read AVD decoded pictures placed into this heap. */
    heapIndex = platformSettings.videoDecoderModuleSettings.avdHeapIndex[videoDecoderIndex];

    /* then return the heap */
    NEXUS_Platform_GetConfiguration(&platformConfig);
    return platformConfig.heap[heapIndex];
}

int main( int argc, const char *argv[] )
{
    NEXUS_Error rc;
    NEXUS_DisplayHandle display;
    NEXUS_VideoWindowHandle window;
    NEXUS_SurfaceHandle frameBuffer;
    NEXUS_SurfaceCreateSettings surfaceCfg;
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_VideoImageInputHandle imageInput;
    NEXUS_VideoInput videoInput;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_Graphics2DHandle gfx;
    NEXUS_Graphics2DSettings gfxSettings;
    BKNI_EventHandle checkpointEvent;
    NEXUS_VideoFormatInfo info;
    NEXUS_SurfaceHandle img;
    NEXUS_VideoWindowSettings windowCfg;
#if NEXUS_NUM_HDMI_OUTPUTS
    NEXUS_HdmiOutputStatus hdmiStatus;
    NEXUS_HdmiOutputHandle hdmiOutput; 
#endif

    int i, fmtIdx, error;
    bool hdmi = false, video = false, graphics = false, fmtLoop = true, resultFail = false; 
    crc_context crc_config;
    goldenCrc goldenCrcData;
    NEXUS_VideoFormat inputFmt = 0;

    static fmtStructure allFormats[] = 
    {
        { NEXUS_VideoFormat_e1080p, "1080p", false },
        { NEXUS_VideoFormat_e1080i, "1080i", false },
        { NEXUS_VideoFormat_e720p, "720p", false },
        { NEXUS_VideoFormat_e720p50hz, "720p50hz", false },
        { NEXUS_VideoFormat_e480p, "480p", false },
        { NEXUS_VideoFormat_e576p, "576p", false },
        { NEXUS_VideoFormat_ePal, "PAL", false },/**/
        
        /* END of ARRAY */
        { 0, "", 0 }
    };    
#define NUM_PATTERNS 5                                    
    colorPatterns pattern[NUM_PATTERNS] = 
    { 
        BLACK_FIELD,
        COLOR_BAR,
        RED_FIELD,
        GREEN_FIELD,
        BLUE_FIELD
    };

    /* Parse & Read Command Line */
    if ( argc > 1 ) 
    {
        for ( i = 1; i < argc; i++ ) 
        {
            /*Set Flags */
            video = video || !strcmp(argv[i], "-video");
            graphics = graphics || !strcmp(argv[i], "-graphics");
#if NEXUS_NUM_HDMI_OUTPUTS
            hdmi = hdmi || !strcmp(argv[i], "-hdmi");
#endif
            
            if ( !strcmp(argv[i], "-format") ) 
            {
                fmtLoop = false ;
                if ( ++i >= argc ) /* Check for format */
                {
                    printf( USAGE);
                    return EXIT_FAILURE;
                }
                /* Set Format */
                for( fmtIdx = 0 ; allFormats[fmtIdx].format ; fmtIdx++) 
                {
                    if( !strcmp(allFormats[fmtIdx].name, argv[i]) ) 
                    {
                        inputFmt = allFormats[fmtIdx].format;
                        break;
                    }
                }
                /* Exit if format from user doesn't match with formats on list */
                if (!inputFmt) 
                {
                    printf("\nFORMAT %s NOT LISTED\n", argv[i] );
                    printf( USAGE );
                    return EXIT_FAILURE;
                }
            }
        }
        /* Must test at least video or hdmi or graphics */
        if ( !( video || hdmi || graphics ) ) 
        {
            printf( USAGE );
            return EXIT_FAILURE;
        }
    }
    else /* No command line is shown */
    {
        printf( USAGE );
        return EXIT_FAILURE;
    }
    
    /* Initialize Platform */
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    rc = NEXUS_Platform_Init(&platformSettings);
    BDBG_ASSERT(!rc);
    NEXUS_Platform_GetConfiguration(&platformConfig);
    
    /* Bring up display only to see if hdmi format is supported*/
    NEXUS_Display_GetDefaultSettings(&displaySettings);
    display = NEXUS_Display_Open(0, &displaySettings);
    
#if NEXUS_NUM_COMPONENT_OUTPUTS
    NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
#endif
#if 0
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
    /* Close display now and start up again when test begins */
    NEXUS_Display_Close(display);
    
    /* Setting the first or only format to test.  */
    if( hdmi ) 
    {
        /* Hdmi test only the supported formats in a loop */
        if( fmtLoop ) 
        {
            printf("\nThe following formats are supported and will be used: \n");
            for( i = 0 ; allFormats[i].format ; i++) 
            {
                if( hdmiStatus.videoFormatSupported[allFormats[i].format] ) 
                {
                    allFormats[i].supported = true;
                    printf("> %s  \n", allFormats[i].name );
                }
            }
            /* Set index fmtIdx and inputFmt to be the first supported format in the list */
            for( fmtIdx = 0 ; allFormats[fmtIdx].format ; fmtIdx++) 
            {
                if( allFormats[fmtIdx].supported ) 
                { 
                    inputFmt = allFormats[fmtIdx].format;
                    break; 
                }
            }
            /*No valid formats from the list*/
            if ( !inputFmt )
            {
                printf( "\n>>> NO FORMATS ARE SUPPORTED\n\n" );
                error = 1;
                goto shutdown;
            }
        }
        else if( !hdmiStatus.videoFormatSupported[allFormats[fmtIdx].format] )
        {
            printf("\n>>> FORMAT %s NOT SUPPORTED\n\n", allFormats[fmtIdx].name );
            error = 1;
            goto shutdown;
        }
    }
    else if ( video || graphics ) 
    { 
        if ( fmtLoop )
        {
            fmtIdx = 0;
            inputFmt = allFormats[0].format;
            
            /* Video / Graphics test only 1080p, 720p, 480p, 576p, 720p50hz formats */
            for( i = 0 ; allFormats[i].format ; i++ ) 
            {
                allFormats[i].supported = ( !strcmp( allFormats[i].name, "1080p") ||
                    !strcmp( allFormats[i].name, "720p") ||
                    !strcmp( allFormats[i].name, "480p") ||
                    !strcmp( allFormats[i].name, "576p") ||
                    !strcmp( allFormats[i].name, "720p50hz") ) ;
            } 
        }        
        else
        {
            /* Video / Graphics test only 1080p, 720p, 480p, 576p, 720p50hz formats */
            /* Exit if format from user input is not one of the following */
            if ( strcmp( allFormats[fmtIdx].name, "1080p") &&
                strcmp( allFormats[fmtIdx].name, "720p") &&
                strcmp( allFormats[fmtIdx].name, "480p") &&
                strcmp( allFormats[fmtIdx].name, "576p") &&
                strcmp( allFormats[fmtIdx].name, "720p50hz") ) 
            { 
                printf( "\n>>> Format Input Invalid for Video / Graphics\n" );
                printf( USAGE );
                goto shutdown;
            }
        }
    }

    /* Begin Test */
    /* Outter loop to change formats */
    while ( allFormats[fmtIdx].format ) 
    {
        NEXUS_Platform_GetConfiguration(&platformConfig);

        /* Bring up display */
        NEXUS_Display_GetDefaultSettings(&displaySettings);
        displaySettings.format = inputFmt;
        display = NEXUS_Display_Open(0, &displaySettings);

#if NEXUS_NUM_COMPONENT_OUTPUTS
        NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
#endif
#if 0
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

        window = NEXUS_VideoWindow_Open(display, 0);
        BDBG_ASSERT(window);

        imageInput = NEXUS_VideoImageInput_Open(0, NULL);
        BDBG_ASSERT(imageInput);

        videoInput = NEXUS_VideoImageInput_GetConnector(imageInput);
        BDBG_ASSERT(videoInput);
        
        rc = NEXUS_VideoWindow_AddInput(window, videoInput);
        BDBG_ASSERT(!rc);
        
        NEXUS_Platform_GetConfiguration(&platformConfig);
        hdmiOutput = platformConfig.outputs.hdmi[0];

        BKNI_CreateEvent(&checkpointEvent);
        gfx = NEXUS_Graphics2D_Open(0, NULL);
        NEXUS_Graphics2D_GetSettings(gfx, &gfxSettings);
        gfxSettings.checkpointCallback.callback = complete;
        gfxSettings.checkpointCallback.context = checkpointEvent;
        NEXUS_Graphics2D_SetSettings(gfx, &gfxSettings);
        
        NEXUS_VideoFormat_GetInfo(displaySettings.format, &info);  
        NEXUS_Surface_GetDefaultCreateSettings(&surfaceCfg);
        surfaceCfg.pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;
        surfaceCfg.width = info.width;
        surfaceCfg.height = info.height;
        surfaceCfg.heap = NEXUS_Platform_GetFramebufferHeap(0);
        frameBuffer = NEXUS_Surface_Create(&surfaceCfg);
        if( !frameBuffer ) 
        {
            fprintf(stderr, "Can't create framebuffer\n");
            return -1;
        }

        /* Now create img surface */
        NEXUS_VideoWindow_GetSettings(window, &windowCfg);
        BDBG_MSG(("windowCfg: width=%d hght=%d x=%d, y=%d", windowCfg.position.width, windowCfg.position.height,
                                                                windowCfg.position.x,     windowCfg.position.y ));

        NEXUS_Surface_GetDefaultCreateSettings(&surfaceCfg);
#if 0    
        if( windowCfg.position.width == 1920 ) 
        {
            surfaceCfg.width = 960;
        }
        else 
        {
            surfaceCfg.width = 720;
        }
#else
        surfaceCfg.width = windowCfg.position.width;
#endif
        surfaceCfg.height = windowCfg.position.height;
        BDBG_MSG(("IMG SURFACE: width=%d hght=%d", surfaceCfg.width, surfaceCfg.height));
        surfaceCfg.heap = b_get_image_input_heap(0);
        surfaceCfg.pixelFormat = NEXUS_PixelFormat_eCr8_Y18_Cb8_Y08;
        img = NEXUS_Surface_Create(&surfaceCfg);
        BDBG_ASSERT(img);

        /* Configure Crc Context */
        crc_config.gfx = gfx;
        crc_config.display = display;
        crc_config.checkpointEvent = checkpointEvent;
        crc_config.framebuffer = frameBuffer;
        crc_config.imgSurface = img;
        crc_config.imageInput = (graphics? NULL : imageInput);    
    
        error = 0;
        /* Set Display to correct format */
        printf("\nTESTING FORMAT: %s...........................\n\n", allFormats[fmtIdx].name);
        
        /* Inner for loop to change color patterns */
        for( i = 0; i < NUM_PATTERNS; i++ ) 
        {
#if 0 /* For Testing */
            error = crcCheck( &crc_config , pattern[i] );
#else 
            /* Read Golden CRCs from video and HDMI Output */
            error = getGoldenCrc( &crc_config, pattern[i], &goldenCrcData );
            if ( error ) { break; }
            
            /* Draw Out Patterns */
            error = drawImage( &crc_config, pattern[i] );
            BDBG_ASSERT(!error);
            
            printf("\n>>> %s: Showing Pattern %d\n", allFormats[fmtIdx].name, i );
            
            /* Video Check */
            if( video || graphics ) 
            {
                BDBG_MSG(("\n\n\tGoldenCrcData %s: \tluma: %#x\tcb: %#x\tcr: %#x  \n\n", 
                    video ? "Video" : "Graphics", 
                    goldenCrcData.vdcCrc->cmp.luma, 
                    goldenCrcData.vdcCrc->cmp.cb, 
                    goldenCrcData.vdcCrc->cmp.cr  ));    
                                                             
                error = displayVdcCrc( &crc_config, *goldenCrcData.vdcCrc , NULL ); 
            }
            
            /* Hdmi Check */
            if ( hdmi ) 
            {
                error += displayHdmiCrc( hdmiOutput, goldenCrcData.hdmiCrc->crc ); 
            }
#endif
            printf("\n>>>............ TEST %s\n", (error)?"FAIL":"PASS" );
            resultFail = resultFail || error;
            /*getchar();*/
            NEXUS_VideoImageInput_SetSurface(imageInput, NULL);
        }

        /* Shut down */
        NEXUS_VideoWindow_RemoveAllInputs(window);
        NEXUS_VideoInput_Shutdown(videoInput);
        NEXUS_VideoImageInput_Close(imageInput);

        BKNI_DestroyEvent(checkpointEvent);

        if ( gfx ) NEXUS_Graphics2D_Close( gfx );
        NEXUS_VideoWindow_Close(window);
        NEXUS_Display_Close(display);
        NEXUS_Surface_Destroy(frameBuffer);
        NEXUS_Surface_Destroy(img);    
 
        /* Loop if no format is specified */
        if ( !( fmtLoop ) ) { break; }
        else 
        { 
            /* Set to next supported format */
            while ( !allFormats[++fmtIdx].supported && allFormats[fmtIdx].format ) {}
            inputFmt = allFormats[fmtIdx].format;
        }
    }

shutdown:     
    resultFail = resultFail || error;
    NEXUS_Platform_Uninit();

    if ( resultFail ) 
    {
        printf( "\n>>> TEST: hdmi_videoinput_crc FAIL\n" );
        return EXIT_FAILURE;
    }
    printf( "\n>>> ALL CRC TESTS PASS\n" );
    return EXIT_SUCCESS;
}

