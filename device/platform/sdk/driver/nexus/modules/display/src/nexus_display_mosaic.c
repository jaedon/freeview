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
 * $brcm_Workfile: nexus_display_mosaic.c $
 * $brcm_Revision: 22 $
 * $brcm_Date: 8/24/12 5:15p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/display/src/nexus_display_mosaic.c $
 * 
 * 27   4/1/13 12:25p jtna
 * SW7346-725: don't shutdown videoInput if destination set
 * 
 * 26   3/21/13 1:25p erickson
 * SW7425-4645: add debug
 * 
 * 25   1/11/13 2:36p erickson
 * SW7425-4343: add critical section for mosaic child list
 * 
 * 22   8/24/12 5:15p erickson
 * SW7425-2236: set ulClearRectAlpha = 0
 * 
 * 21   8/9/12 3:13p erickson
 * SW7425-3725: silence warning
 * 
 * 20   6/26/12 9:53a mward
 * SW7420-2085:  Set the NEXUS_VideoWindow object in
 *  NEXUS_VideoWindow_OpenMosaic().
 * 
 * 19   6/5/12 1:31p erickson
 * SW7408-330: merge
 * 
 * SW7408-330/1   5/24/12 2:03p cnovak
 * SW7408-330: Set mosaic background color based on parent display
 * 
 * 18   3/23/12 12:07p jtna
 * SW7346-725: shutdown mosaic parent's video input before adding mosaic
 *  children
 * 
 * 17   2/1/12 1:49p erickson
 * SW7425-2236: set non-mosaic zorder based on the parent's zorder. this
 *  enables PIP mosaic.
 * 
 * 16   6/27/11 4:59p erickson
 * SW7425-466: force zorder to zero for one-mosaic configuration. this
 *  avoids potential conflicts with PIP (zorder=1).
 * 
 * 15   6/2/10 12:36p erickson
 * SW7400-2773: support all stripe widths
 *
 * 14   4/5/10 4:19p erickson
 * SW7405-3965: explicitly unset parentVideoInput->source in
 *  NEXUS_VideoWindow_P_RemoveMosaicInput
 *
 * 13   3/26/10 2:19p erickson
 * SW7405-3965: refactor mosaic to support hd/sd simul mode
 *
 * 12   3/16/10 1:26p erickson
 * SW7405-3625: insert mosaics in mosaicIndex order so that backendMosaic
 *  is applied correctly
 *
 * 11   3/2/10 11:29a erickson
 * SW7405-3625: add check to NEXUS_VideoWindow_P_RemoveMosaicInput
 *
 * 10   2/22/10 2:45p erickson
 * SW7405-3625: fix non-mosaic build
 *
 * 9   2/22/10 12:50p erickson
 * SW7405-3625: update backendMosaic impl
 *
 * 8   2/19/10 1:23p erickson
 * SW7405-3625: add NEXUS_VideoWindowMosaicSettings for backendMosaic
 *  support
 *
 * 7   2/12/10 3:23p mward
 * SW7125-143: bClearRectByMaskColor must be set when setting up mosaic
 *  mode.  Set mosaic background black.
 *
 * 6   1/6/10 4:18p erickson
 * SW7405-3533: if there's only one mosaic window, convert to a normal
 *  window. apply all settings, including MAD.
 *
 * 5   12/9/09 4:54p erickson
 * SW7405-3394: fix non-mosaic build
 *
 * 4   12/9/09 3:10p erickson
 * SW7405-3394: refactor mosaic mode API
 *
 * 3   7/10/09 3:58p erickson
 * PR56558: add NEXUS_DisplayModule_SetVideoDecoderModule to allow faster
 *  start up time
 *
 * 2   4/28/09 4:03p erickson
 * PR54676: allow VideoDecoder to incrementally create/destroy channels
 *
 * 1   1/18/08 2:20p jgarrett
 * PR 38808: Merging to main branch
 *
 * Nexus_Devel/2   11/8/07 10:56a erickson
 * PR36802: added #if for no mosaic
 *
 * Nexus_Devel/1   11/8/07 9:16a erickson
 * PR36802: added mosaic mode
 *
 ***************************************************************************/
#include "nexus_display_module.h"

BDBG_MODULE(nexus_display_mosaic);
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)
BDBG_FILE_MODULE(nexus_flow_video_window);
#endif

NEXUS_VideoWindowHandle NEXUS_VideoWindow_OpenMosaic( NEXUS_VideoWindowHandle parentWindow, unsigned mosaicIndex)
{
#if NEXUS_NUM_MOSAIC_DECODES
    NEXUS_VideoWindowHandle window;

    BDBG_OBJECT_ASSERT(parentWindow, NEXUS_VideoWindow);

    if (parentWindow->mosaic.parent) {
        BDBG_ERR(("Window %p is a mosaic child and so can't be a mosaic parent", parentWindow));
        return NULL;
    }
    if (mosaicIndex >= NEXUS_NUM_MOSAIC_DECODES) {
        BERR_TRACE(NEXUS_INVALID_PARAMETER);
        return NULL;
    }
    for (window = BLST_S_FIRST(&parentWindow->mosaic.children); window; window = BLST_S_NEXT(window, mosaic.link)) {
        if (window->mosaic.userIndex == mosaicIndex) {
            BDBG_ERR(("Window %p mosaic %d already opened", parentWindow, mosaicIndex));
            return NULL;
        }
    }

    window = BKNI_Malloc(sizeof(*window));
    if (!window) {
        BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);
        return NULL;
    }
    NEXUS_VideoWindow_P_InitState(window, mosaicIndex, parentWindow->display);
    NEXUS_OBJECT_SET(NEXUS_VideoWindow, window);
    window->open = true;
    window->mosaic.parent = parentWindow;
    window->mosaic.userIndex = mosaicIndex;

    /* Insert in mosaicIndex order
    This is convenient for NEXUS_VideoWindow_P_CutBackendMosaic_isr where there is no VideoDecoder order, only window order. */
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)
    BKNI_EnterCriticalSection();
#endif
    {
        NEXUS_VideoWindowHandle mosaicWindow, prev = NULL;
        for (mosaicWindow = BLST_S_FIRST(&parentWindow->mosaic.children); mosaicWindow; mosaicWindow = BLST_S_NEXT(mosaicWindow, mosaic.link)) {
            if (mosaicWindow->mosaic.userIndex > mosaicIndex) {
                if (prev) {
                    BLST_S_INSERT_AFTER(&parentWindow->mosaic.children, prev, window, mosaic.link);
                }
                else {
                    BLST_S_INSERT_HEAD(&parentWindow->mosaic.children, window, mosaic.link);
                }
                break;
            }
            prev = mosaicWindow;
        }
        if (!mosaicWindow) {
            if (prev) {
                BLST_S_INSERT_AFTER(&parentWindow->mosaic.children, prev, window, mosaic.link);
            }
            else {
                BLST_S_INSERT_HEAD(&parentWindow->mosaic.children, window, mosaic.link);
            }
        }
    }
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)
    BKNI_LeaveCriticalSection();
#endif

    /* default to a tile layout based on mosaicIndex */
    window->cfg.position.x = 50 + 130 * (mosaicIndex%4);
    window->cfg.position.y = 50 + 130 * (mosaicIndex/4);
    window->cfg.position.width = 100;
    window->cfg.position.height = 100;
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)
    BDBG_MODULE_MSG(nexus_flow_video_window, ("open mosaic %p, display %d, parent %d, mosaic %d", window, window->display->index, window->mosaic.parent->index, window->index));
#endif
    return window;
#else
    BSTD_UNUSED(parentWindow);
    BSTD_UNUSED(mosaicIndex);
    BERR_TRACE(NEXUS_NOT_SUPPORTED);
    return NULL;
#endif
}

/*
Summary:
Returns bits 'e'..'b' from word 'w',

Example:
    B_GET_BITS(0xDE,7,4)==0x0D
    B_GET_BITS(0xDE,3,0)=0x0E
*/
#define B_GET_BITS(w,e,b)  (((w)>>(b))&(((unsigned)(-1))>>((sizeof(unsigned))*8-(e+1-b))))

#define pVideo (&g_NEXUS_DisplayModule_State)

NEXUS_Error NEXUS_VideoWindow_P_ApplyMosaic(NEXUS_VideoWindowHandle window)
{
#if NEXUS_NUM_MOSAIC_DECODES
    BERR_Code rc;
    NEXUS_VideoWindowHandle mosaicWindow;
    NEXUS_VideoWindowHandle singleWindow = NULL;
    BVDC_Window_Handle windowVdc;
    unsigned cnt = 0;
    unsigned total = 0;
    bool mosaic_visible[NEXUS_NUM_MOSAIC_DECODES];
    BVDC_Rect mosaic_rects[NEXUS_NUM_MOSAIC_DECODES];
    uint8_t mosaic_zorder[NEXUS_NUM_MOSAIC_DECODES];
    unsigned mosaicTrackIndex = NEXUS_NUM_MOSAIC_DECODES;
    NEXUS_VideoInput_P_Link *link;
    BAVC_MFD_Picture *pPrevPicture = NULL;

    BDBG_OBJECT_ASSERT(window, NEXUS_VideoWindow);
    BDBG_ASSERT(!window->mosaic.parent);

    windowVdc = window->vdcState.window;
    if (!windowVdc) return 0;

    BKNI_Memset(mosaic_visible, 0, sizeof(mosaic_visible));

    for (mosaicWindow = BLST_S_FIRST(&window->mosaic.children); mosaicWindow; mosaicWindow = BLST_S_NEXT(mosaicWindow, mosaic.link)) {
        unsigned index;

        if (!mosaicWindow->input) continue;

        link = NEXUS_VideoInput_P_Get(mosaicWindow->input);
        BDBG_ASSERT(link);

        if (mosaicWindow->mosaic.mosaicSettings.backendMosaic.enabled) {
            index = mosaicWindow->mosaic.userIndex;

            /* build a linked list of BAVC_MFD_Picture structs for backend mosaic */
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)
            BKNI_EnterCriticalSection();
#endif
            if (pPrevPicture) {
                pPrevPicture->pNext = &mosaicWindow->mosaic.picture;
            }
            pPrevPicture = &mosaicWindow->mosaic.picture;
            pPrevPicture->pNext = NULL;
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)
            BKNI_LeaveCriticalSection();
#endif
        }
        else {
            index = link->mosaic.index;
        }

        mosaic_visible[index] = mosaicWindow->cfg.visible;
        mosaic_rects[index].lLeft = mosaicWindow->cfg.position.x;
        mosaic_rects[index].lTop = mosaicWindow->cfg.position.y;
        mosaic_rects[index].ulWidth = mosaicWindow->cfg.position.width;
        mosaic_rects[index].ulHeight = mosaicWindow->cfg.position.height;
        /* skip mosaic zorder=1 because of PIP */
        mosaic_zorder[index] = mosaicWindow->cfg.zorder>0?mosaicWindow->cfg.zorder+1:mosaicWindow->cfg.zorder;
        BDBG_MSG(("mosaic %d: v=%c z=%d position=%d,%d,%d,%d", index,
            mosaicWindow->cfg.visible?'y':'n',
            mosaicWindow->cfg.zorder,
            mosaicWindow->cfg.position.x,mosaicWindow->cfg.position.y,mosaicWindow->cfg.position.width,mosaicWindow->cfg.position.height));

        if (window->display->cfg.frameRateMaster) {
            if (window->display->cfg.frameRateMaster == mosaicWindow->input) {
                mosaicTrackIndex = index;
            }
        }
        else if (mosaicWindow->cfg.autoMaster) {
            mosaicTrackIndex = index;
        }

        if (index+1 > cnt) {
            cnt = index+1;
        }
        total++;
        singleWindow = mosaicWindow;
    }

    if (!total) {
        BDBG_MSG(("no mosaics"));
        rc = BVDC_Window_SetMosaicConfiguration(windowVdc, false, NULL);
        if (rc) {return BERR_TRACE(rc);}
    }
    else if (total == 1) {
        NEXUS_VideoInput_P_Link *link;
        NEXUS_VideoWindowSettings cfg = singleWindow->cfg;

        BDBG_MSG(("one mosaic: converting to regular window"));
        /* if only one mosaic, dynamically convert to a normal window */
        rc = BVDC_Window_SetMosaicConfiguration(windowVdc, false, NULL);
        if (rc) {return BERR_TRACE(rc);}

        link = NEXUS_VideoInput_P_Get(singleWindow->mosaic.parent->input);
        BDBG_ASSERT(link);
        BSTD_UNUSED(link);

        /* set non-mosaic zorder based on the parent's zorder. this enables PIP mosaic. */
        cfg.zorder = singleWindow->mosaic.parent->cfg.zorder;
        rc = NEXUS_VideoWindow_P_SetVdcSettings(singleWindow->mosaic.parent, &cfg, true);
        if (rc) {return BERR_TRACE(rc);}

        singleWindow->mosaic.parent->adjContext = singleWindow->adjContext;
        rc = NEXUS_VideoAdj_P_ApplySetSettings(singleWindow->mosaic.parent);
        if (rc) {return BERR_TRACE(rc);}
    }
    else {
        bool dummy;
        NEXUS_DisplayHandle display = window->display;
        BVDC_MosaicConfiguration mosaic_config;
        unsigned i;
        const NEXUS_Rect *pDisplayRect = &window->display->displayRect;

        /* VDC does not ignore invisible windows, so we must ensure ok values */
        for (i=0;i<cnt;i++) {
            if (!mosaic_visible[i]) {
                mosaic_rects[i].lLeft = mosaic_rects[i].lTop = 0;
                mosaic_rects[i].ulWidth = mosaic_rects[i].ulHeight = 100;
                mosaic_zorder[i] = 0;
            }
        }
        BDBG_MSG(("%d mosaics, array size %d", total, cnt));

        /* set parent properties that are required for mosaics */
        rc = BVDC_Window_SetScalerOutput(windowVdc, 0, 0, pDisplayRect->width, pDisplayRect->height);
        if (rc) {return BERR_TRACE(rc);}
        rc = BVDC_Window_SetDstRect(windowVdc, 0, 0, pDisplayRect->width, pDisplayRect->height);
        if (rc) {return BERR_TRACE(rc);}
        rc = BVDC_Window_SetVisibility(windowVdc, true);
        if (rc) {return BERR_TRACE(rc);}

        BVDC_Window_GetMosaicConfiguration(windowVdc, &dummy, &mosaic_config);
        mosaic_config.bVideoInMosaics = true;
        mosaic_config.bClearRectByMaskColor = true;
        mosaic_config.ulClearRectAlpha = 0;
        mosaic_config.ulMaskColorBlue = B_GET_BITS(display->cfg.background, 7, 0);
        mosaic_config.ulMaskColorGreen = B_GET_BITS(display->cfg.background, 15, 8);
        mosaic_config.ulMaskColorRed = B_GET_BITS(display->cfg.background, 23, 16);
        rc = BVDC_Window_SetMosaicConfiguration(windowVdc, true, &mosaic_config);
        if (rc) {return BERR_TRACE(rc);}

        rc = BVDC_Window_SetMosaicDstRects(windowVdc, cnt, mosaic_rects);
        if (rc) {return BERR_TRACE(rc);}
        rc = BVDC_Window_SetMosaicRectsVisibility(windowVdc, cnt, mosaic_visible);
        if (rc) {return BERR_TRACE(rc);}
        rc = BVDC_Window_SetMosaicRectsZOrder(windowVdc, cnt, mosaic_zorder);
        if (rc) {return BERR_TRACE(rc);}
        if (mosaicTrackIndex != NEXUS_NUM_MOSAIC_DECODES) {
            rc = BVDC_Window_SetMosaicTrack(windowVdc, mosaicTrackIndex);
            if (rc) {return BERR_TRACE(rc);}
        }
    }

    rc = BVDC_ApplyChanges(pVideo->vdc);
    if (rc) {return BERR_TRACE(rc);}

    return 0;
#else
    BSTD_UNUSED(window);
    return BERR_TRACE(BERR_NOT_SUPPORTED);
#endif
}

void NEXUS_VideoWindow_GetMosaicSettings( NEXUS_VideoWindowHandle window, NEXUS_VideoWindowMosaicSettings *pSettings )
{
    BDBG_OBJECT_ASSERT(window, NEXUS_VideoWindow);
#if NEXUS_NUM_MOSAIC_DECODES
    *pSettings = window->mosaic.mosaicSettings;
#else
    BSTD_UNUSED(pSettings);
    BERR_TRACE(NEXUS_NOT_SUPPORTED);
#endif
}

NEXUS_Error NEXUS_VideoWindow_SetMosaicSettings( NEXUS_VideoWindowHandle window, const NEXUS_VideoWindowMosaicSettings *pSettings )
{
    BDBG_OBJECT_ASSERT(window, NEXUS_VideoWindow);
#if NEXUS_NUM_MOSAIC_DECODES
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)
    BKNI_EnterCriticalSection();
#endif
    window->mosaic.mosaicSettings = *pSettings;
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)
    BKNI_LeaveCriticalSection();
#endif
    return 0;
#else
    BSTD_UNUSED(pSettings);
    return BERR_TRACE(NEXUS_NOT_SUPPORTED);
#endif
}

#if NEXUS_NUM_MOSAIC_DECODES
const BAVC_MFD_Picture *NEXUS_VideoWindow_P_CutBackendMosaic_isr(NEXUS_VideoWindowHandle window, const BAVC_MFD_Picture *pPicture)
{
    unsigned ulStripeWidth;
    NEXUS_VideoWindowHandle mosaicWindow;
    const BAVC_MFD_Picture *pFirstPicture = NULL;
    unsigned id = 0;

    BDBG_OBJECT_ASSERT(window, NEXUS_VideoWindow);

    switch (pPicture->eStripeWidth) {
    case BAVC_StripeWidth_e64Byte: ulStripeWidth = 64; break;
    case BAVC_StripeWidth_e128Byte: ulStripeWidth = 128; break;
    case BAVC_StripeWidth_e256Byte: ulStripeWidth = 256; break;
    default:
        BDBG_ERR(("unsupported stripe width %d", pPicture->eStripeWidth));
        return NULL;
    }

    for (mosaicWindow = BLST_S_FIRST(&window->mosaic.children); mosaicWindow; mosaicWindow = BLST_S_NEXT(mosaicWindow, mosaic.link)) {
        BAVC_MFD_Picture *pThis, *pNext;
        unsigned ulStripeNum;
        NEXUS_VideoWindowMosaicSettings *pMosaicSettings = &mosaicWindow->mosaic.mosaicSettings;
        unsigned x, y;

        if (!pMosaicSettings->backendMosaic.enabled) continue;

        pThis = &mosaicWindow->mosaic.picture;
        if (!pFirstPicture) {
            pFirstPicture = pThis;
        }

        pNext = pThis->pNext;
        *pThis = *pPicture; /* overwrite everything */
        pThis->pNext = pNext; /* restore the pNext pointer */
        pThis->ulChannelId = id++;

        /* avoid div-by-zero math */
        if (!pMosaicSettings->backendMosaic.clipBase.width || !pMosaicSettings->backendMosaic.clipBase.height) {
            continue;
        }

        x = pMosaicSettings->backendMosaic.clipRect.x * pThis->ulSourceHorizontalSize / pMosaicSettings->backendMosaic.clipBase.width;
        y = pMosaicSettings->backendMosaic.clipRect.y * pThis->ulSourceVerticalSize / pMosaicSettings->backendMosaic.clipBase.height;

        pThis->ulSourceHorizontalSize = pThis->ulSourceHorizontalSize * pMosaicSettings->backendMosaic.clipRect.width / pMosaicSettings->backendMosaic.clipBase.width;
        pThis->ulDisplayHorizontalSize = pThis->ulDisplayHorizontalSize * pMosaicSettings->backendMosaic.clipRect.width / pMosaicSettings->backendMosaic.clipBase.width;
        pThis->ulSourceVerticalSize = pThis->ulSourceVerticalSize * pMosaicSettings->backendMosaic.clipRect.height / pMosaicSettings->backendMosaic.clipBase.height;
        pThis->ulDisplayVerticalSize = pThis->ulDisplayVerticalSize * pMosaicSettings->backendMosaic.clipRect.height / pMosaicSettings->backendMosaic.clipBase.height;

        ulStripeNum = x / ulStripeWidth;

        pThis->pLuminanceFrameBufferAddress = (void*)
            ((uint8_t*)pThis->pLuminanceFrameBufferAddress +
             ulStripeNum * pThis->ulLuminanceNMBY * 16 * ulStripeWidth +
             y * ulStripeWidth +
             (x % ulStripeWidth));

        pThis->pChrominanceFrameBufferAddress = (void*)
            ((uint8_t*)pThis->pChrominanceFrameBufferAddress +

            /* stripe offset */
             ulStripeNum * pThis->ulChrominanceNMBY * 16 * ulStripeWidth +

            /* vertical offset within a stripe (take care of 422 vs 420 chroma type) */
             ((y * ulStripeWidth /2) << (pThis->eYCbCrType - BAVC_YCbCrType_e4_2_0)) +

            /* horizontal offset within a stripe */
             (x % ulStripeWidth));
    }

    return pFirstPicture ? pFirstPicture : pPicture;
}

/**
Need to find parent mosaic NEXUS_VideoInput which matches the child mosaic mosaicInput.
We must match by going back to the VideoDecoder mosaic parent.
Be aware that a single parent mosaic NEXUS_VideoInput will serve multiple mosaic windows on multiple displays.
**/
static NEXUS_VideoInput NEXUS_VideoWindow_P_AssignMosaicInput(NEXUS_VideoInput mosaicInput)
{
    NEXUS_VideoInput input = NULL;
    unsigned freeInput = NEXUS_NUM_MOSAIC_DECODE_SETS;
    NEXUS_VideoDecoderDisplayConnection connection;
    unsigned i;

    if (!pVideo->modules.videoDecoder) {
        BERR_TRACE(NEXUS_NOT_SUPPORTED);
        return NULL;
    }

    NEXUS_Module_Lock(pVideo->modules.videoDecoder);
    NEXUS_VideoDecoder_GetDisplayConnection_priv(mosaicInput->source, &connection);
    NEXUS_Module_Unlock(pVideo->modules.videoDecoder);

    for (i=0;i<NEXUS_NUM_MOSAIC_DECODE_SETS;i++) {
        if (pVideo->mosaicInput[i].input.source) {
            if (connection.parentIndex == pVideo->mosaicInput[i].parentIndex) {
                input = &pVideo->mosaicInput[i].input;
                break;
            }
        }
        else if (freeInput == NEXUS_NUM_MOSAIC_DECODE_SETS) {
            freeInput = i;
        }
    }

    if (!input && freeInput < NEXUS_NUM_MOSAIC_DECODE_SETS) {
        input = &pVideo->mosaicInput[freeInput].input;
        input->source = mosaicInput->source; /* this is the link */
        pVideo->mosaicInput[freeInput].parentIndex = connection.parentIndex; /* cache this value */
    }

    return input;
}

NEXUS_Error NEXUS_VideoWindow_P_AddMosaicInput(NEXUS_VideoWindowHandle window, NEXUS_VideoInput input)
{
    NEXUS_Error rc;
    NEXUS_VideoInput parentVideoInput;
    NEXUS_VideoInput_P_Link *parentLink;

    BDBG_OBJECT_ASSERT(window, NEXUS_VideoWindow);

    /* Mosaic windows share a common parent VideoWindow and a common parent NEXUS_VideoInput_P_Link.
       There is a single NEXUS_VideoInput_P_Link for each VideoDecoder set of mosaics. One NEXUS_VideoInput_P_Link could serve multiple displays (HD/SD simul).
       The parent link is not connected back to VideoDecoder; it only holds the BVDC_Source for the Display module.
       Each mosaic window has its own, separate NEXUS_VideoInput_P_Link as well, used to connect back to each mosaic decoder and to store its mosaicIndex. */

    if (input->type != NEXUS_VideoInputType_eDecoder) {
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }

    parentVideoInput = NEXUS_VideoWindow_P_AssignMosaicInput(input);
    if (!parentVideoInput) return BERR_TRACE(NEXUS_INVALID_PARAMETER);

    if (!window->mosaic.parent->input) {
        NEXUS_VideoInput_P_Link *link;
        BLST_S_DICT_FIND(&pVideo->inputs, link, input, input, link);
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)
        if (link && !input->destination) {
            BDBG_WRN(("Shutting down input %p. You must call NEXUS_VideoInput_Shutdown() before calling NEXUS_VideoWindow_AddInput() with a mosaic window", input));
            NEXUS_VideoInput_Shutdown(input);
        }
#else
        if (link) {
            BDBG_WRN(("Shutting down input %p. You must call NEXUS_VideoInput_Shutdown() before calling NEXUS_VideoWindow_AddInput() with a mosaic window", input));
            NEXUS_VideoInput_Shutdown(input);
        }
#endif
        rc = NEXUS_VideoWindow_AddInput(window->mosaic.parent, parentVideoInput);
        if (rc) {
            parentVideoInput->source = NULL; /* reverse the effect of NEXUS_VideoWindow_P_AssignMosaicInput */
            return BERR_TRACE(rc);
        }

        parentLink = parentVideoInput->destination;
        BDBG_ASSERT(parentLink);

        parentLink->mosaic.parentWindow[window->display->index] = window->mosaic.parent;

        /* The decoder is not mosaic. so only the parentLink can connect. instead, the display module
        will cut up the single stream into the desired number of mosaics in NEXUS_VideoInput_P_DecoderDataReady_isr.
        This must be set after NEXUS_VideoWindow_AddInput. */
        parentLink->mosaic.backendMosaic = window->mosaic.mosaicSettings.backendMosaic.enabled;
    }
    else {
        if (window->mosaic.parent->input != parentVideoInput) {
            BDBG_ERR(("mosaic mismatch parent window %p already has input %p, mosaic window maps to input %p",
                window->mosaic.parent, window->mosaic.parent->input, window, parentVideoInput));
            return BERR_TRACE(NEXUS_UNKNOWN);
        }
        parentLink = parentVideoInput->destination;
    }

    /* now create this input's link, but use the parentLink for the shared VDC source. */
    if (input->destination == NULL) {
        NEXUS_VideoInput_P_Link *link;
        BLST_S_DICT_FIND(&pVideo->inputs, link, input, input, link);
        if (!link) {
            link = NEXUS_VideoInput_P_OpenDecoder(input, parentLink);
            if (!link) {
                return BERR_TRACE(NEXUS_UNKNOWN);
            }
        }
    }

    return 0;
}

void NEXUS_VideoWindow_P_RemoveMosaicInput(NEXUS_VideoWindowHandle window, NEXUS_VideoInput input)
{
    NEXUS_VideoInput parentVideoInput;
    NEXUS_VideoInput_P_Link *parentLink;

    BDBG_OBJECT_ASSERT(window, NEXUS_VideoWindow);
    BDBG_ASSERT(window->mosaic.parent);
    BDBG_OBJECT_ASSERT(window->mosaic.parent, NEXUS_VideoWindow);

    /* check if this source is used for the mosaic parent. if so, switch the mosaic parent to
    another mosaic's source. if there are no more mosaics, destroy the parent link. */

    parentVideoInput = window->mosaic.parent->input;
    if (!parentVideoInput) return;
    BDBG_OBJECT_ASSERT(parentVideoInput, NEXUS_VideoInput);

    parentLink = parentVideoInput->destination;
    if (!parentLink) return;
    BDBG_OBJECT_ASSERT(parentLink, NEXUS_VideoInput_P_Link);

    /* are we removing the mosaic video input which was used to connect the parent video input? if so, we must adjust the parent */
    if (parentVideoInput->source == input->source) {
        unsigned i;
        NEXUS_VideoWindowHandle mosaicChild = NULL;

        /* find another mosaic which is currently using this parentLink */
        for (i=0;i<NEXUS_NUM_DISPLAYS;i++) {
            NEXUS_VideoWindowHandle parentWindow = parentLink->mosaic.parentWindow[i];
            if (!parentWindow) continue;

            BDBG_OBJECT_ASSERT(parentWindow, NEXUS_VideoWindow);
            for (mosaicChild = BLST_S_FIRST(&parentWindow->mosaic.children); mosaicChild; mosaicChild = BLST_S_NEXT(mosaicChild, mosaic.link)) {
                if ((mosaicChild != window) && mosaicChild->input && mosaicChild->input->source) {
                    i = NEXUS_NUM_DISPLAYS; /* break out of outer loop */
                    break;
                }
            }
        }

        if (mosaicChild) {
            BDBG_MSG(("moving internal mosaic parent input(%p) from %p to %p", parentVideoInput, window, mosaicChild));
            parentVideoInput->source = mosaicChild->input->source;
            parentLink->input_info.source = mosaicChild->input->source;
        }
        else {
            /* parentVideoInput was not assigned to another mosaic child. it is unused. */
            BDBG_MSG(("shutting down internal mosaic parent input(%p)", parentVideoInput));
            parentLink->mosaic.backendMosaic = false; /* this will allow parent to disconnect if backendMosaic was true */
            NEXUS_VideoInput_Shutdown(parentVideoInput);
            parentVideoInput->source = NULL;
        }
    }
}
#endif
