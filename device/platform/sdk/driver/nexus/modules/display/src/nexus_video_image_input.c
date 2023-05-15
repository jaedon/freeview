/***************************************************************************
 *     (c)2007-2012 Broadcom Corporation
 *
 *  This program is the proprietary software of Broadcom Corporation and/or its licensors,
 *  and may only be used, duplicated, modified or distributed pursuant to the terms and
 *  conditions of a separate, written license agreement executed between you and Broadcom
 *  (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 *  no license (express or implied), right to use, or waiver of any kind with respect to the
 *  Software, and Broadcom expressly reserves all rights in and to the Software and all
 *  intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 *  HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 *  NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
 *
 *  Except as expressly set forth in the Authorized License,
 *
 *  1.     This program, including its structure, sequence and organization, constitutes the valuable trade
 *  secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 *  and to use this information only in connection with your use of Broadcom integrated circuit products.
 *
 *  2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 *  AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
 *  WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
 *  THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
 *  OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
 *  LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
 *  OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
 *  USE OR PERFORMANCE OF THE SOFTWARE.
 *
 *  3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
 *  LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
 *  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
 *  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
 *  THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
 *  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
 *  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
 *  ANY LIMITED REMEDY.
 *
 * $brcm_Workfile: nexus_video_image_input.c $
 * $brcm_Revision: 38 $
 * $brcm_Date: 10/11/12 12:29p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/display/src/nexus_video_image_input.c $
 * 
 * 38   10/11/12 12:29p vsilyaev
 * SW7435-329: Converted NEXUS_VideoInput to NEXUS_VideoInputHandle and
 *  added object verification
 * 
 * 37   10/10/12 10:32a erickson
 * SW7408-346: support graphics-only build
 * 
 * 36   9/24/12 5:57p ahulse
 * SW7425-2172: reduce shutdown time
 * 
 * 35   9/21/12 5:51p ahulse
 * SW7425-2172: reduce shutdown time
 * 
 * 34   9/17/12 10:37a ahulse
 * SW7425-3896,SW7425-2172: pass pts into BAVC_Gfx_Picture structure
 * 
 * 33   9/11/12 1:49p ahulse
 * SW7425-2172: Update image input to use MFD SetSurface support
 * 
 * 32   8/15/12 2:31p ahulse
 * SW7425-3728: fix coverity issue 20855
 * 
 * 31   6/29/12 3:38p ahulse
 * SW7425-3391: fix coverity issue 42866
 * 
 * 30   6/19/12 12:42p vsilyaev
 * SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
 * 
 * 29   3/6/12 4:43p ahulse
 * SW7425-1182: on submitting a surface, get link if doesn't already exist
 * 
 * 28   3/5/12 4:18p ahulse
 * SW7425-2548: fix coverity CID 36946
 * 
 * 27   3/5/12 3:25p ahulse
 * SW7425-2551: fix coverity CID 36940
 * 
 * 26   3/2/12 12:17p ahulse
 * SW7425-1182: use BVDC_Source_GetCapabilties to check supported pixel
 *  formats
 * 
 * 25   3/1/12 1:05p ahulse
 * SW7425-1182: extend supported pixel formats
 * 
 * 24   2/20/12 6:42p ahulse
 * SW7425-1511: Merge pushing/recycling API changes to main
 * 
 * SW7425-1511/1   2/15/12 6:04p ahulse
 * SW7425-1511: Extend image input to use new API for Pushing and
 *  recycling surfaces as well as allow vsync count, pts and jump to head
 *  of queue for submitted image.
 * 
 * 23   9/29/11 2:04p erickson
 * SW7425-1270: rename to progressiveScan1080
 * 
 * 22   9/9/11 4:45p erickson
 * SW7425-1270: add NEXUS_VideoImageInputSettings.is1080p60Capable
 * 
 * 21   8/22/11 6:07p akam
 * SWDTV-8006: Fixed coverity warning.
 * 
 * 20   7/8/11 1:17p erickson
 * SW7346-306: VideoImageInput must set eInterruptRefreshRate with display
 *  format
 * 
 * 19   6/16/11 2:57p erickson
 * SW7420-1942: NEXUS_VideoImageInput_Destroy can automatically call
 *  NEXUS_VideoInput_Shutdown
 * 
 * 18   4/25/11 9:33a erickson
 * SW3556-1233: fix warning
 * 
 * 17   3/15/11 11:40a mward
 * SW7400-3016:  Fix OVERRUN_STATIC.  Image not found is not busy.
 * 
 * 16   2/17/11 1:56p ahulse
 * SW7420-1380: Refactor ImageInput to use a queue and be non-blocking.
 *  Add xx_SetNextSurface to be non-blocking. xx_SetSurface maintains
 *  blocking and is deprecated
 * 
 * 15   11/19/10 10:46a erickson
 * SWGIGGSVIZIO-122: add check to NEXUS_VideoImageInput_SetSurface for
 *  minimum 64x64
 * 
 * 14   4/16/10 3:11p VISHK
 * SW7325-697: BMRC_MONITOR errors when NEXUS_Surface_Destroy() is called
 *  for video image input surface
 *
 * 13   3/26/10 4:33p VISHK
 * SW7325-697: BMRC_MONITOR errors when NEXUS_Surface_Destroy() is called
 *  for video image input surface
 *
 * 12   1/6/10 11:50a erickson
 * SW3548-2692: set link->info_valid = true after setting link->info.mfd.
 *  this allows nexus_vdb to calculate correct RTS for the image.
 *
 * 11   12/31/09 10:41a erickson
 * SW3548-2690: don't fire NEXUS_VideoImageInputSettings.imageCallback
 *  until the new image is actually being read by the MFD
 *
 * 10   9/9/09 1:19p erickson
 * SW7405-2977: allow build with no video decoder
 *
 * 9   11/7/08 12:30p erickson
 * PR48745: added NEXUS_VideoImageInputSettings.heap
 *
 * 8   10/2/08 9:58a erickson
 * PR47303: BSUR_Surface_Create api change
 *
 * 7   8/20/08 7:16p vsilyaev
 * PR 45830: Added support for 1080p feed
 *
 * 6   7/22/08 10:40a vsilyaev
 * PR 44727: Added functions to change settings of the video image
 *
 * 5   7/14/08 5:09p vsilyaev
 * PR 44727: Improved handling of imageCallback
 *
 * 4   7/14/08 2:33p vsilyaev
 * PR 44727: Added imageCallback
 *
 * 3   4/18/08 1:21p vsilyaev
 * PR 39986: Fixed handling of progressive outputs
 *
 * 2   3/27/08 4:22p vsilyaev
 * PR 40818: Improved internal API to allow source private heaps
 *
 * 1   3/21/08 12:09p vsilyaev
 * PR 39986: Added VideoImageInput
 *
  *
 **************************************************************************/
#include "nexus_base.h"
#include "nexus_display_module.h"
#include "priv/nexus_surface_priv.h"
#include "nexus_power_management.h"

BDBG_MODULE(nexus_video_image_input);

#define BDBG_MSG_ISR(X) /* BDBG_MSG(X)  */

struct NEXUS_VideoImageInput {
    NEXUS_OBJECT(NEXUS_VideoImageInput);
    NEXUS_VideoInputObject input;
    NEXUS_TaskCallbackHandle  imageCallback;
    NEXUS_VideoImageInputSettings settings;
    bool connected;

    int curPic, quePic;
    int lastFree;
    int waitingPics;               /* keep tabs on pics waiting to be displayed */
    BFMT_VideoFmt displayFormat;

    struct tagfeeder {
        BAVC_SourceId sourceId;
        struct {
            BSUR_Surface_Handle frame;
        } surface;
        unsigned   age;            /* how long since pic was displayed */
        unsigned   primed;         /* set to Vsync display count when an image is loaded into queue, not cleared until image removed from queue */
        unsigned   loaded;         /* set to Vsync display count when pic is displayed, decremented until =1 while displayed */
        bool       displayed;      /* Set to true while pic is actively displayed */
        bool       freed;          /* Set to true when pic has transitioned from displayed to non-displayed and can be freed */
        NEXUS_SurfaceHandle usrSurface;
        NEXUS_SurfaceHandle freedSurface;
        BAVC_Gfx_Picture pic;
    } *feeder;
    int fifoSize;

    BVDC_Source_Handle source; 
    BKNI_EventHandle event;
    NEXUS_EventCallbackHandle eventHandler;
};

NEXUS_Error NEXUS_VideoImageInput_P_CleanUpSurfaces( NEXUS_VideoImageInputHandle imageInput );

static void
NEXUS_Display_P_ImageInputNext_isr(void  *disp, int unused, BAVC_Polarity  polarity, BAVC_SourceState  state, void **picture)
{
    NEXUS_VideoImageInputHandle imageInput = disp;

    BSTD_UNUSED(unused);
    BSTD_UNUSED(state);
    BSTD_UNUSED(polarity);
    BDBG_OBJECT_ASSERT(imageInput, NEXUS_VideoImageInput);
    BDBG_ASSERT(picture);

    *picture = NULL; 

    if ( imageInput->feeder[imageInput->curPic].primed  ) {
        BAVC_Gfx_Picture pic;
        BERR_Code rc;

        rc = BVDC_Source_GetSurface_isr(imageInput->source, &pic);
        if (!rc) {
            if (pic.hSurface == imageInput->feeder[imageInput->curPic].surface.frame ) {
                int i,freed=0;
                imageInput->feeder[imageInput->curPic].age++;
                imageInput->feeder[imageInput->curPic].displayed = true;
                if ( imageInput->feeder[imageInput->curPic].loaded ) {
                    imageInput->feeder[imageInput->curPic].loaded--;
                }


                for ( i=0; i < imageInput->fifoSize ; i++ ) {
                    if ( i != imageInput->curPic && imageInput->feeder[i].displayed ) {
                        imageInput->feeder[i].displayed = false;
                        imageInput->feeder[i].freed     = true;
                        freed++;
                    }
                }

                if ( imageInput->waitingPics || freed ) {            
                    /* Need to fire an event for new pic to be queued out of isr time */
                    /* and it should be ready in time for the next v sysnc */
                    BKNI_SetEvent(imageInput->event);
                }

            } 
        }
    }
    else if ( imageInput->feeder[imageInput->curPic].age && imageInput->feeder[imageInput->curPic].usrSurface==NULL ) {
        imageInput->feeder[imageInput->curPic].age++;
    }

    return;
}

static void
NEXUS_ImageInput_P_EventHandler(void *cntx)
{
    NEXUS_VideoImageInputHandle imageInput = cntx;
    const NEXUS_DisplayModule_State *video= &g_NEXUS_DisplayModule_State;
    BERR_Code rc;

    if ( imageInput->feeder[imageInput->curPic].usrSurface ) {
        /* advance to next picture if available */
        if ( imageInput->feeder[imageInput->curPic].displayed ) {
            BDBG_MSG_ISR(("EvntHdlr, waitPics=%d curPic=%d].loaded=%d" , imageInput->waitingPics, imageInput->curPic, imageInput->feeder[imageInput->curPic].loaded ));
            if ( imageInput->feeder[imageInput->curPic].loaded <= 1  ) { /* count of one or less, means vsync count satisfied */
                BKNI_EnterCriticalSection();    

                if ( imageInput->waitingPics ) {
                    imageInput->waitingPics--;
                    /* If anything else is waiting advance to it */
                    imageInput->curPic++;
                    if ( imageInput->curPic >= imageInput->fifoSize ) imageInput->curPic = 0;
                    imageInput->feeder[imageInput->curPic].loaded = imageInput->feeder[imageInput->curPic].primed;
                
                    BKNI_LeaveCriticalSection(); 
        
                    rc = BVDC_Source_SetSurface( imageInput->source , &imageInput->feeder[imageInput->curPic].pic );
                    if (rc!=BERR_SUCCESS) { BERR_TRACE(rc);}

                    rc = BVDC_ApplyChanges(video->vdc);
                    if (rc!=BERR_SUCCESS) { BERR_TRACE(rc);}


                    NEXUS_TaskCallback_Fire(imageInput->imageCallback);
                }
                else
                    BKNI_LeaveCriticalSection(); 
            }
        }  
    }
    else {
        /* No surface is set kick start the process */
        BKNI_EnterCriticalSection();

        if ( imageInput->waitingPics ) {
            imageInput->waitingPics--;
            /* If anything else is waiting advance to it */
            imageInput->curPic++;
            if ( imageInput->curPic >= imageInput->fifoSize ) imageInput->curPic = 0;
            imageInput->feeder[imageInput->curPic].loaded = imageInput->feeder[imageInput->curPic].primed;

            BKNI_LeaveCriticalSection();

            rc = BVDC_Source_SetSurface( imageInput->source , &imageInput->feeder[imageInput->curPic].pic );
            if (rc!=BERR_SUCCESS) { BERR_TRACE(rc);}

            rc = BVDC_ApplyChanges(video->vdc);
            if (rc!=BERR_SUCCESS) { BERR_TRACE(rc);}
        }
        else 
            BKNI_LeaveCriticalSection(); 

    }
}

void NEXUS_ImageInput_GetDefaultGfxPic(BAVC_Gfx_Picture *pGfxPic) {

    BDBG_ASSERT(pGfxPic);
    BKNI_Memset(pGfxPic, 0, sizeof(BAVC_Gfx_Picture));
    pGfxPic->hSurface = NULL; 
    pGfxPic->eInOrientation = NEXUS_P_VideoOrientation_ToMagnum(NEXUS_VideoOrientation_e2D);
}

void
NEXUS_VideoImageInput_GetDefaultSurfaceSettings(NEXUS_VideoImageInputSurfaceSettings *pSettings)
{
    BDBG_ASSERT(pSettings);
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    pSettings->displayVsyncs = 1;
    return;
}

void
NEXUS_VideoImageInput_GetDefaultSettings(NEXUS_VideoImageInputSettings *pSettings)
{
    BDBG_ASSERT(pSettings);
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    NEXUS_CALLBACKDESC_INIT(&pSettings->imageCallback);
#if defined NEXUS_VIDEO_IMAGE_INPUT_1080P_FEED || BCHP_CHIP == 3548 || BCHP_CHIP == 3556
    /* macro and BCHP_CHIP support for backward compatibility */
    pSettings->progressiveScan1080 = true; 
#endif
    pSettings->fifoSize = 4;
    return;
}

NEXUS_VideoImageInputHandle
NEXUS_VideoImageInput_Open(unsigned index, const NEXUS_VideoImageInputSettings *pSettings)
{
    NEXUS_VideoImageInputHandle  imageInput;
    NEXUS_VideoImageInputSettings settings;
    NEXUS_Error rc;
    int i,checkedFifoSize;

    if(!pSettings) {
        NEXUS_VideoImageInput_GetDefaultSettings(&settings);
        pSettings = &settings;
    }

#if NEXUS_NUM_VIDEO_DECODERS
/* If there are no video decoders, there is no way to validate the index. */
    if (index >= NEXUS_NUM_VIDEO_DECODERS) {
        rc = BERR_TRACE(NEXUS_INVALID_PARAMETER);
        goto err_index;
    }
#endif
    imageInput = BKNI_Malloc(sizeof(*imageInput));
    if(!imageInput) {
        rc = BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);
        goto err_alloc;
    }
    NEXUS_OBJECT_INIT(NEXUS_VideoImageInput, imageInput);
    NEXUS_VIDEO_INPUT_INIT(&imageInput->input, NEXUS_VideoInputType_eImage, imageInput);

    checkedFifoSize = (int)pSettings->fifoSize;
    if( (int)pSettings->fifoSize < 2 ) {
        BDBG_MSG(("Overriding fifosize to min of 2 !"));
        checkedFifoSize = 2;    /* minimum queue size */
    }

    imageInput->imageCallback = NEXUS_TaskCallback_Create( imageInput, NULL);
    if ( NULL == imageInput->imageCallback )
    {
        rc = BERR_TRACE(BERR_OS_ERROR);
        goto err_callback;
    }


    rc = BKNI_CreateEvent(&imageInput->event);
    if(rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc);goto err_event;}
    imageInput->eventHandler = NEXUS_RegisterEvent(imageInput->event, NEXUS_ImageInput_P_EventHandler, imageInput );
    if(!imageInput->eventHandler) {rc=BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);goto err_event_handler;}

    imageInput->feeder = BKNI_Malloc(sizeof(struct tagfeeder) * checkedFifoSize );

    if(!imageInput->feeder) {
        rc = BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);
        goto err_feeder;
    }
    BKNI_Memset(imageInput->feeder, 0, sizeof(struct tagfeeder) * checkedFifoSize );

    imageInput->fifoSize = checkedFifoSize;

    for ( i=0; i < imageInput->fifoSize ; i++ ) {
        imageInput->feeder[i].sourceId = BAVC_SourceId_eMpeg0 + index;
        imageInput->feeder[i].usrSurface   = NULL;
        imageInput->feeder[i].freedSurface = NULL;
    }

    imageInput->settings = *pSettings;

    imageInput->curPic = imageInput->quePic = imageInput->lastFree = 0;
    imageInput->feeder[0].primed = 1;  /* for kickstart */
    NEXUS_OBJECT_REGISTER(NEXUS_VideoInput, &imageInput->input, Open);

    return imageInput;

err_feeder:
    NEXUS_UnregisterEvent(imageInput->eventHandler);
err_event_handler:
    BKNI_DestroyEvent(imageInput->event);
err_event:
    NEXUS_TaskCallback_Destroy(imageInput->imageCallback);
err_callback:
    BKNI_Free(imageInput);
err_alloc:
#if NEXUS_NUM_VIDEO_DECODERS
err_index:
#endif
    return NULL;
}

NEXUS_Error NEXUS_VideoImageInput_P_CleanUpSurfaces( NEXUS_VideoImageInputHandle imageInput )
{
    NEXUS_Error rc=NEXUS_SUCCESS;
    int i,j;
    j = imageInput->curPic; /* work backwards from current pic which can't be freed */
    for ( i=0; i < imageInput->fifoSize; i++ ) {
        j--;
        if ( j < 0 ) j = imageInput->fifoSize-1;
        if ( imageInput->feeder[j].freed && j != imageInput->curPic ) {
            const NEXUS_DisplayModule_State *video= &g_NEXUS_DisplayModule_State;

            if (imageInput->feeder[j].usrSurface) {
                NEXUS_Module_Lock(video->modules.surface);
                NEXUS_Surface_GetSurface_priv(imageInput->feeder[j].usrSurface);
                NEXUS_Module_Unlock(video->modules.surface);
            }
            BKNI_EnterCriticalSection();
            imageInput->feeder[j].freedSurface   = imageInput->feeder[j].usrSurface;
            imageInput->feeder[j].usrSurface     = NULL;
            imageInput->feeder[j].surface.frame  = NULL;
            imageInput->feeder[j].primed = 0 ;
            imageInput->feeder[j].freed  = false;
            BKNI_LeaveCriticalSection();
            imageInput->feeder[j].age = 0;  
        }
    }
    return rc;
}

static void
NEXUS_VideoImageInput_P_ShutdownFeeder(NEXUS_VideoImageInputHandle imageInput)
{
    BERR_Code rc;
    const NEXUS_DisplayModule_State *video= &g_NEXUS_DisplayModule_State;
    int i;
   
    /* Before letting go of the surface, verify it's still allocated. Users should not destroy the surface
    before either calling NEXUS_VideoImageInput_SetSurface(imageInput, NULL) or closing VideoImageInput. */
    if (imageInput->feeder[imageInput->curPic].usrSurface) {
        NEXUS_Module_Lock(video->modules.surface);
        NEXUS_Surface_GetSurface_priv(imageInput->feeder[imageInput->curPic].usrSurface);
        NEXUS_Module_Unlock(video->modules.surface);
    }

    /* first clean mfd_picture and only then destroy surfaces */
    BKNI_EnterCriticalSection();
    imageInput->waitingPics = 0;                   /* Disable loading of any pics in Isr */
    imageInput->feeder[imageInput->curPic].age = 0; 

    imageInput->feeder[imageInput->curPic].surface.frame  = NULL;
    imageInput->feeder[imageInput->curPic].usrSurface = NULL; 
    BKNI_LeaveCriticalSection();
    if (imageInput->connected) {
        unsigned watchdog = 0;
        /* We need to see 2 vsyncs occur to make sure this surface is not being read by the MFD */
        BDBG_ASSERT(imageInput->feeder[imageInput->curPic].age == 0);
        BKNI_EnterCriticalSection();
        imageInput->feeder[imageInput->curPic].age    = 1;
        imageInput->feeder[imageInput->curPic].loaded = 1;
        imageInput->feeder[imageInput->curPic].primed = 0;
        BKNI_LeaveCriticalSection();
        while (imageInput->feeder[imageInput->curPic].age <= 2 && watchdog++ < 100) {
            BKNI_Sleep(1);
        }
        imageInput->feeder[imageInput->curPic].age    = 0;
        imageInput->feeder[imageInput->curPic].loaded = 0;
        imageInput->feeder[imageInput->curPic].displayed = 0;
        /* Now the surface is unused */
    }

    /* Now VDC isn't holding any surface, so mark for cleanup any other surfaces left over */
    for ( i=0; i < imageInput->fifoSize; i++ ) {
        imageInput->feeder[i].primed = 0;
    }
    /* Do the clean up */
    rc = NEXUS_VideoImageInput_P_CleanUpSurfaces(imageInput);
    if ( rc ) rc = BERR_TRACE(rc);
    
    return;
}

static void
NEXUS_VideoImageInput_P_Finalizer(NEXUS_VideoImageInputHandle imageInput)
{
    NEXUS_OBJECT_ASSERT(NEXUS_VideoImageInput, imageInput);
    if(imageInput->input.destination!=NULL  || imageInput->input.ref_cnt!=0) {
        BDBG_WRN(("NEXUS_VideoImageInput_Destroy: %#lx videoInput (%#lx) is in use", (unsigned long)imageInput, (unsigned long)&imageInput->input));
        NEXUS_VideoInput_Shutdown(NEXUS_VideoImageInput_GetConnector(imageInput));
    }
    NEXUS_VideoImageInput_P_ShutdownFeeder(imageInput);
    NEXUS_TaskCallback_Destroy(imageInput->imageCallback);

    BKNI_Free(imageInput->feeder);

    NEXUS_UnregisterEvent(imageInput->eventHandler);
    BKNI_DestroyEvent(imageInput->event);

    NEXUS_OBJECT_DESTROY(NEXUS_VideoImageInput, imageInput);
    BKNI_Free(imageInput);
    return;
}

static void
NEXUS_VideoImageInput_P_Release(NEXUS_VideoImageInputHandle imageInput)
{
    NEXUS_OBJECT_ASSERT(NEXUS_VideoImageInput, imageInput);
    NEXUS_OBJECT_UNREGISTER(NEXUS_VideoInput, &imageInput->input, Close);
    return;
}

NEXUS_OBJECT_CLASS_MAKE_WITH_RELEASE(NEXUS_VideoImageInput, NEXUS_VideoImageInput_Close);

NEXUS_Error NEXUS_VideoImageInput_RecycleSurface( NEXUS_VideoImageInputHandle imageInput, NEXUS_SurfaceHandle *recycled, size_t num_entries, size_t *num_returned )
{
    NEXUS_Error rc;
    int i,j,nrFreeable;

    BDBG_OBJECT_ASSERT(imageInput, NEXUS_VideoImageInput);

    for ( i=0; i < (int)num_entries ; i++ ) {
        recycled[i] = NULL;
    }

    /* clean up anything changed since last time */
    rc = NEXUS_VideoImageInput_P_CleanUpSurfaces( imageInput );
    if ( rc ) rc = BERR_TRACE(rc);

    /* First go through list and see which ones are free, note their surface handles so we can pass back to caller */
    j = imageInput->lastFree;
    nrFreeable=0;
    for ( i=0; i < imageInput->fifoSize ; i++ ) {
        j++;
        if ( j >= imageInput->fifoSize ) j = 0;
        if ( imageInput->feeder[j].freedSurface ) {
            if ( nrFreeable < (int)num_entries ) {
                recycled[nrFreeable++] = imageInput->feeder[j].freedSurface;
                BDBG_MSG(("xx_Recyc recycled[nrFreeable=%d]=%p feeder[j=%d].freedSurface=%p" , nrFreeable-1, recycled[nrFreeable-1], j, imageInput->feeder[j].freedSurface ));
                BKNI_EnterCriticalSection();
                imageInput->feeder[j].freedSurface = NULL;
                imageInput->lastFree = j;
                BKNI_LeaveCriticalSection();
                if ( nrFreeable >= (int)num_entries ) break;
            }
        }
    }
    *num_returned = nrFreeable;
    if ( nrFreeable ) {
        /* Now release them from imageinput queue system */
        rc = NEXUS_VideoImageInput_P_CleanUpSurfaces( imageInput );
        if ( rc ) rc = BERR_TRACE(rc);

    }
    return rc;
}

NEXUS_Error NEXUS_VideoImageInput_CheckSurfaceCompletion( NEXUS_VideoImageInputHandle imageInput, NEXUS_SurfaceHandle image )
{
    NEXUS_Error rc;
    int i;

    BDBG_OBJECT_ASSERT(imageInput, NEXUS_VideoImageInput);

    rc = NEXUS_VideoImageInput_P_CleanUpSurfaces( imageInput );
    if ( rc ) rc = BERR_TRACE(rc);

    for ( i=0; i < imageInput->fifoSize ; i++ ) {
        /* find image to check for */
        if ( imageInput->feeder[i].usrSurface == image ) break;
    }

    if ( (i < imageInput->fifoSize) && imageInput->feeder[i].primed ) {
       return NEXUS_IMAGEINPUT_BUSY;    
    }
    else {
        return BERR_SUCCESS;
    }
}

NEXUS_Error
NEXUS_VideoImageInput_PushSurface(NEXUS_VideoImageInputHandle imageInput, NEXUS_SurfaceHandle image, const NEXUS_VideoImageInputSurfaceSettings *pSettings ) {

    int i,j,availableSlots=0;
    const NEXUS_DisplayModule_State *video= &g_NEXUS_DisplayModule_State;
    BSUR_Surface_Handle frame;
    BERR_Code rc;
    unsigned width;
    unsigned height;
    NEXUS_SurfaceCreateSettings surfaceCfg;
    NEXUS_VideoImageInputSurfaceSettings defaultSettings;
    NEXUS_VideoInput_P_Link *link;
    BVDC_Source_Capabilities vdcCapabilities;
    BPXL_Format magnumPixelFormat;

    BAVC_Gfx_Picture pic; 

    BDBG_OBJECT_ASSERT(imageInput, NEXUS_VideoImageInput);

    if(image==NULL) {
        /* Passing in NULL, flushes the pipeline, first set current feeder to NULL */
        NEXUS_VideoImageInput_P_ShutdownFeeder(imageInput);
        goto done; /* shortcut */
    }

    if(!pSettings) {
        NEXUS_VideoImageInput_GetDefaultSurfaceSettings(&defaultSettings);
        pSettings = &defaultSettings;
    }

    /* clean up any surfaces newly freed */
    rc = NEXUS_VideoImageInput_P_CleanUpSurfaces( imageInput );
    if ( rc ) rc = BERR_TRACE(rc);

    /* add new surface to queue */
    NEXUS_Surface_GetCreateSettings(image, &surfaceCfg);
    width = surfaceCfg.width;
    height = surfaceCfg.height;
    if (width < 64 || height < 64) {
        BDBG_ERR(("minimum surface size for VideoImageInput is 64x64. surface %p is %dx%d", image, width, height));
        rc = BERR_TRACE(NEXUS_INVALID_PARAMETER);
        goto err_surface;
    }
    link = NEXUS_VideoInput_P_Get( NEXUS_VideoImageInput_GetConnector(imageInput) );
    BDBG_ASSERT(link);
    BDBG_OBJECT_ASSERT(link, NEXUS_VideoInput_P_Link);
    BDBG_ASSERT(link->sourceVdc);

    rc = NEXUS_P_PixelFormat_ToMagnum( surfaceCfg.pixelFormat, &magnumPixelFormat);
    if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc);goto err_surface;}

    BVDC_Source_GetCapabilities(link->sourceVdc, &vdcCapabilities) ;
    if ( !vdcCapabilities.pfIsPxlfmtSupported(magnumPixelFormat) ) {
        BDBG_WRN(("pixelFormat %d of image=%p is NOT supported", surfaceCfg.pixelFormat, image ));
        rc = BERR_TRACE(NEXUS_INVALID_PARAMETER); /* unsupported  pixelFormat */
        goto err_surface;
    }

    if ( pSettings->infront ) {
        /* de-queue all queued surfaces except current active, add new surface in now cleared list */
        j = imageInput->curPic + 1;
        if ( j >= imageInput->fifoSize) j = 0;

        BKNI_EnterCriticalSection();
        for ( i=0; i < imageInput->fifoSize ; i++ ) {
            if ( imageInput->waitingPics ) {
                imageInput->feeder[j].freed  = true;
                imageInput->waitingPics--;
                j++;
                if ( j >= imageInput->fifoSize) j = 0;
            }            
            else {
                break;
            } 
        }
        BKNI_LeaveCriticalSection();

        /* clean up any surfaces newly freed */
        rc = NEXUS_VideoImageInput_P_CleanUpSurfaces( imageInput );
        if ( rc ) rc = BERR_TRACE(rc);

        BKNI_EnterCriticalSection();
        imageInput->quePic = imageInput->curPic+1;
        if ( imageInput->quePic >= imageInput->fifoSize ) imageInput->quePic = 0;
        BKNI_LeaveCriticalSection();
        availableSlots = imageInput->fifoSize - 1;
        goto doQueue;
    }
    else { 
        /* check if can add to queue and do so if able */
        for ( i=0; i < imageInput->fifoSize ; i++ ) {
            if ( !imageInput->feeder[i].usrSurface && !imageInput->feeder[i].primed ) {
                availableSlots++;
                continue ;
            }
            else if ( image == imageInput->feeder[i].usrSurface ) {
                /* passed in surface is in list, abort and return busy */
                return NEXUS_IMAGEINPUT_BUSY; 
            }
        }   
        if ( availableSlots == 0 ) {
            return NEXUS_IMAGEINPUT_QUEUE_FULL;            
        }
        else {
            BDBG_MSG(("NEXUS_VideoImageInput_PushSurface image=%p curPic=%d available slots=%d" , image, imageInput->curPic, availableSlots ));
        }
        imageInput->quePic++;
        if ( imageInput->quePic >= imageInput->fifoSize ) imageInput->quePic = 0;
    }
doQueue:

    NEXUS_Module_Lock(video->modules.surface);

    frame = NEXUS_Surface_GetSurface_priv(image);
    BDBG_ASSERT(frame);

    NEXUS_ImageInput_GetDefaultGfxPic(&pic);
    pic.hSurface = frame;
    pic.ulOrigPTS = pSettings->pts;

    NEXUS_Module_Unlock(video->modules.surface);
    BKNI_EnterCriticalSection();
    imageInput->feeder[imageInput->quePic].usrSurface     = image;
    imageInput->feeder[imageInput->quePic].surface.frame  = frame;
    imageInput->feeder[imageInput->quePic].freed  = false;
    imageInput->feeder[imageInput->quePic].primed = pSettings->displayVsyncs;
    imageInput->feeder[imageInput->quePic].age = 1;

    imageInput->feeder[imageInput->quePic].pic = pic;

    imageInput->waitingPics++;
    BKNI_LeaveCriticalSection();
    BDBG_MSG(("xx_PushSurf image=%p timestamp=0x%8x curPic=%d quePic=%d waitPics=%d" ,image , pSettings->pts, imageInput->curPic, imageInput->quePic, imageInput->waitingPics ));
    BKNI_SetEvent(imageInput->event); /* synchronise adding pictures with isr */

done:
    return BERR_SUCCESS;
err_surface:
    return rc;
}

NEXUS_Error
NEXUS_VideoImageInput_SetNextSurface(NEXUS_VideoImageInputHandle imageInput, NEXUS_SurfaceHandle image )
{
    const NEXUS_DisplayModule_State *video= &g_NEXUS_DisplayModule_State;
    BSUR_Surface_Handle frame;
    BERR_Code rc;
    unsigned width;
    unsigned height;
    NEXUS_SurfaceCreateSettings surfaceCfg;
    NEXUS_VideoInput_P_Link *link;
    BVDC_Source_Capabilities vdcCapabilities;
    BPXL_Format magnumPixelFormat;
    BAVC_Gfx_Picture pic;

    BDBG_OBJECT_ASSERT(imageInput, NEXUS_VideoImageInput);
    if( image && imageInput->feeder[imageInput->curPic].usrSurface==image) {
        return NEXUS_IMAGEINPUT_BUSY;
    }

    if(image==NULL) {
        /* Passing in NULL, flushes the pipeline, first set current feeder to NULL */
        NEXUS_VideoImageInput_P_ShutdownFeeder(imageInput);
        goto done; /* shortcut */
    }

    rc = NEXUS_VideoImageInput_P_CleanUpSurfaces( imageInput );
    if ( rc ) rc = BERR_TRACE(rc);

    if ( imageInput->waitingPics >= imageInput->fifoSize - 2 ) {
        return NEXUS_IMAGEINPUT_QUEUE_FULL;
    }

    NEXUS_Surface_GetCreateSettings(image, &surfaceCfg);
    width = surfaceCfg.width;
    height = surfaceCfg.height;
    if (width < 64 || height < 64) {
        BDBG_ERR(("minimum surface size for VideoImageInput is 64x64. surface %p is %dx%d", image, width, height));
        rc = BERR_TRACE(NEXUS_INVALID_PARAMETER);
        goto err_surface;
    }
    link = NEXUS_VideoInput_P_Get( NEXUS_VideoImageInput_GetConnector(imageInput) );
    BDBG_ASSERT(link);
    BDBG_OBJECT_ASSERT(link, NEXUS_VideoInput_P_Link);
    BDBG_ASSERT(link->sourceVdc);

    rc = NEXUS_P_PixelFormat_ToMagnum( surfaceCfg.pixelFormat, &magnumPixelFormat);
    if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc);goto err_surface;}

    BVDC_Source_GetCapabilities(link->sourceVdc, &vdcCapabilities) ;
    if ( !vdcCapabilities.pfIsPxlfmtSupported(magnumPixelFormat) ) {
        BDBG_WRN(("pixelFormat %d of image=%p is NOT supported", surfaceCfg.pixelFormat, image ));
        rc = BERR_TRACE(NEXUS_INVALID_PARAMETER); /* unsupported  pixelFormat */
        goto err_surface;
    }
    imageInput->quePic++;
    if ( imageInput->quePic >= imageInput->fifoSize ) imageInput->quePic = 0;

    NEXUS_Module_Lock(video->modules.surface);

    frame = NEXUS_Surface_GetSurface_priv(image);
    BDBG_ASSERT(frame);

    NEXUS_Module_Unlock(video->modules.surface);

    imageInput->feeder[imageInput->quePic].usrSurface     = image;
    imageInput->feeder[imageInput->quePic].surface.frame  = frame;

    imageInput->source = link->sourceVdc;

    NEXUS_ImageInput_GetDefaultGfxPic(&pic);
    pic.hSurface = frame;

    BKNI_EnterCriticalSection();

    imageInput->feeder[imageInput->quePic].freed  = false;
    imageInput->feeder[imageInput->quePic].primed = 1;  /* legacy api, no Vsync count */
    imageInput->feeder[imageInput->quePic].age = 1;
    imageInput->feeder[imageInput->quePic].pic = pic;
    imageInput->waitingPics++;

    BKNI_LeaveCriticalSection();

    BKNI_SetEvent(imageInput->event); /* Let handler start new picture */

done:
    return BERR_SUCCESS;

err_surface:
    return rc;
}

NEXUS_Error
NEXUS_VideoImageInput_SetSurface(NEXUS_VideoImageInputHandle imageInput, NEXUS_SurfaceHandle image )
{
    BERR_Code rc;
    int i,cnt=6;

    do  {
        /* if image_input queue is full, wait for a slot opening up */
        rc = NEXUS_VideoImageInput_SetNextSurface(imageInput, image );
        if(rc==NEXUS_IMAGEINPUT_QUEUE_FULL) {
            BKNI_Sleep(8);  /* wait half a vsync if queue full */
        }
        else if ( rc==NEXUS_INVALID_PARAMETER ) goto err_param;
    }
    while( rc );

    if ( image != NULL ) {
        i = imageInput->curPic; /* "snapshot" current picture value so we're not following a moving target */
        do {
            /* spin half vsyncs until current picture removed from display - ie a new pic has been displayed */
            BKNI_Sleep(8);
        } while ( imageInput->feeder[i].displayed && cnt-- );
    }
err_param:
    return rc;
}

NEXUS_VideoInputHandle
NEXUS_VideoImageInput_GetConnector(NEXUS_VideoImageInputHandle imageInput)
{
    BDBG_OBJECT_ASSERT(imageInput, NEXUS_VideoImageInput);
    return &imageInput->input;
}


static BERR_Code
NEXUS_VideoImageInput_P_Connect(NEXUS_VideoInput_P_Link *link)
{
    BERR_Code rc;
    NEXUS_VideoImageInputHandle imageInput;
    

    BDBG_ASSERT(link->input && link->input->source);
    imageInput = link->input->source;
    BDBG_OBJECT_ASSERT(imageInput, NEXUS_VideoImageInput);

    NEXUS_PowerManagement_SetCoreState(NEXUS_PowerManagementCore_eImageInput, true);

    imageInput->source = link->sourceVdc;
    rc = BVDC_Source_InstallPictureCallback( imageInput->source , NEXUS_Display_P_ImageInputNext_isr, imageInput , 0);
    if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc);goto err_source_cfg;}

    imageInput->connected = true;

    return rc;

err_source_cfg:
    return rc;

}

static void
NEXUS_VideoImageInput_P_Disconnect(NEXUS_VideoInput_P_Link *link)
{
    NEXUS_VideoImageInputHandle imageInput;
    BERR_Code rc;

    BDBG_ASSERT(link->input && link->input->source);
    imageInput = link->input->source;
    BDBG_OBJECT_ASSERT(imageInput, NEXUS_VideoImageInput);

    rc = BVDC_Source_InstallPictureCallback(link->sourceVdc, NULL, NULL, 0);
    if (rc) rc = BERR_TRACE(rc);

    NEXUS_PowerManagement_SetCoreState(NEXUS_PowerManagementCore_eImageInput, false);

    imageInput->connected = false;
}


NEXUS_VideoInput_P_Link *
NEXUS_VideoImageInput_P_OpenInput(NEXUS_VideoInput input)
{
    NEXUS_VideoInput_P_Iface iface;
    NEXUS_VideoInput_P_LinkData data;
    NEXUS_VideoImageInputHandle imageInput = input->source;

    BDBG_ASSERT(input->source);
    BDBG_ASSERT(input->type == NEXUS_VideoInputType_eImage);
    BDBG_OBJECT_ASSERT(imageInput, NEXUS_VideoImageInput);

    iface.connect = NEXUS_VideoImageInput_P_Connect;
    iface.disconnect = NEXUS_VideoImageInput_P_Disconnect;
    NEXUS_VideoInput_P_LinkData_Init(&data, imageInput->feeder[imageInput->curPic].sourceId);
    data.heap = imageInput->settings.heap;
    return NEXUS_VideoInput_P_CreateLink(input, &data, &iface);
}

void
NEXUS_VideoImageInput_GetSettings(NEXUS_VideoImageInputHandle imageInput, NEXUS_VideoImageInputSettings *pSettings)
{
    BDBG_OBJECT_ASSERT(imageInput, NEXUS_VideoImageInput);
    BDBG_ASSERT(pSettings);
    *pSettings = imageInput->settings;
    return;
}

NEXUS_Error
NEXUS_VideoImageInput_SetSettings(NEXUS_VideoImageInputHandle imageInput, const NEXUS_VideoImageInputSettings *pSettings)
{
    BDBG_OBJECT_ASSERT(imageInput, NEXUS_VideoImageInput);
    BDBG_ASSERT(pSettings);
    if(pSettings->qScale>127) { return BERR_TRACE(BERR_INVALID_PARAMETER); }

    NEXUS_Module_TaskCallback_Set(imageInput->imageCallback, &pSettings->imageCallback);
    imageInput->settings = *pSettings;
    return NEXUS_SUCCESS;
}

void NEXUS_VideoImageInput_P_UpdateDisplayInformation(NEXUS_VideoImageInputHandle imageInput, const NEXUS_DisplaySettings *pSettings)
{
    BFMT_VideoFmt fmt;
    NEXUS_Error rc;
    
    BDBG_OBJECT_ASSERT(imageInput, NEXUS_VideoImageInput);
    rc = NEXUS_P_VideoFormat_ToMagnum(pSettings->format, &fmt);
    if (!rc) {
        BKNI_EnterCriticalSection();
        imageInput->displayFormat = fmt;
        BKNI_LeaveCriticalSection();
    }
}

