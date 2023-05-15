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
 * $brcm_Workfile: nexus_video_window_impl.h $
 * $brcm_Revision: 40 $
 * $brcm_Date: 7/3/12 5:11p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/display/7400/src/nexus_video_window_impl.h $
 * 
 * 40   7/3/12 5:11p bandrews
 * SW7425-717: merge to main
 * 
 * 39   6/19/12 12:42p vsilyaev
 * SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
 * 
 * SW7425-717/2   6/4/12 6:51p bandrews
 * SW7425-717: Added refresh rate for later; add prototype for
 *  UpdatePhaseDelay_isr
 * 
 * SW7425-717/1   6/4/12 5:59p bandrews
 * SW7425-717: update to fix some bugs and get correct status
 * 
 * 38   3/14/12 12:25p erickson
 * SW7401-4517: remove old code
 * 
 * 37   11/15/11 3:27p erickson
 * SW7425-1747: remove old code
 * 
 * 36   9/6/11 10:41a erickson
 * SW7346-472: clean up 3D right-view buffer returned by
 *  BVDC_Window_GetBuffer
 * 
 * 35   7/7/11 10:57a erickson
 * SW7405-5394: move BVDC_Window_SetMasterFrameRate logic into
 *  NEXUS_VideoWindow_P_ConfigMasterFrameRate, which is called from all
 *  places where it could change.
 * 
 * 34   3/31/11 3:58p gmohile
 * SW7408-260 : Open/close VDC handles during resume/standby
 * 
 * 33   2/15/11 5:33p gmohile
 * SW7408-210 : Merge initial version of module standby
 * 
 * SW7408-210/1   1/21/11 1:05p gmohile
 * Sw7408-210 : Add standby support
 * 
 * 32   12/28/10 3:57p erickson
 * SW7405-5064: added NEXUS_VideoWindowStatus.outputRect and
 *  NEXUS_VideoWindowSettings.outputRectChanged
 *
 * 31   12/16/10 5:26p petlee
 * SW35230-1815: Add new feature for PFA
 *
 * 30   3/26/10 2:19p erickson
 * SW7405-3965: refactor mosaic to support hd/sd simul mode
 *
 * 29   2/22/10 12:50p erickson
 * SW7405-3625: update backendMosaic impl
 *
 * 28   2/19/10 3:30p erickson
 * SW7405-3625: fix non-mosaic capable systems
 *
 * 27   2/19/10 1:23p erickson
 * SW7405-3625: add NEXUS_VideoWindowMosaicSettings for backendMosaic
 *  support
 *
 * 26   2/10/10 2:35p erickson
 * SW7405-3829: refactor internal functions to create/destroy VDC window.
 *  NEXUS_VideoWindowMadSettings.shrinkWidth requires window to be
 *  recreated.
 *
 * 25   1/11/10 2:02p erickson
 * SW7405-3718: add NEXUS_VideoWindowAfdSettings
 *
 * 24   1/6/10 4:18p erickson
 * SW7405-3533: move window init code into NEXUS_VideoWindow_P_InitState.
 *  this allows normal and mosaic windows to init to the same defaults.
 *
 * 23   12/9/09 3:10p erickson
 * SW7405-3394: refactor mosaic mode API
 *
 * 22   11/2/09 10:44a erickson
 * SW3548-2584: allow NULL color matrix for
 *  NEXUS_VideoWindow_SetColorMatrix
 *
 * 21   10/1/09 11:59a erickson
 * SW3548-2512: allow NEXUS_VideoWindow_SetColorMatrixNonLinearA/B/C to be
 *  called with NULL to unset user values.
 *
 * 20   9/17/09 3:02p erickson
 * SW3548-2348: account for window heaps separately from input heaps
 *
 * 19   8/10/09 8:17a gmohile
 * PR 56400 : Merge legacy vdc support to main line
 *
 * 97401_nexus/1   6/26/09 11:20a gmohile
 * PR 56400 : Add 7401 nexus support
 *
 * 18   4/20/09 4:37p jgarrett
 * PR 54346: Changing ForceZoom to ForceFull, adding better recovery from
 *  invalid app states.
 *
 * 17   4/14/09 4:50p jgarrett
 * PR 51743: Updating BVDB_GetVideoDisplayMode to new args
 *
 * 16   1/14/09 12:33p erickson
 * PR46300: fix non-DTV
 *
 * 15   1/13/09 6:15p erickson
 * PR46300: added NEXUS_VideoWindow_SetSpecialFeature1Settings
 *
 * 14   1/6/09 11:33a jgarrett
 * PR 48984: Merge dynamic RTS code to main branch
 *
 * PR48984/2   12/5/08 4:23p jgarrett
 * PR 48984: Adding RTS ID
 *
 * PR48984/1   11/20/08 6:47p jgarrett
 * PR 48984: Adding vdb restrictions
 *
 * 13   10/2/08 5:09p erickson
 * PR46300: added test feature 1 extension
 *
 * 12   5/23/08 10:49a erickson
 * PR42982: rework PQ Dirty logic to reapply settings on channel change
 *
 * 11   4/30/08 4:42p erickson
 * PR39453: add 3548/3556 impl
 *
 * 10   4/3/08 10:17a erickson
 * PR41122: move picture quality code to 7400
 *
 * 9   4/1/08 3:19p vsilyaev
 * PR 41027: Added use fixed mapping between nexus and VDC windowId's
 *
 * 8   4/1/08 1:34p erickson
 * PR40411: merge. added lb_event to drive pcinput position algo.
 *
 * 40411_take2/1   4/1/08 6:31p gorgon
 * PR40411: add letterBoxDetectionChange and GetLetterBoxStatus. Use event
 *  to triger lb.
 *
 * 7   3/20/08 3:00p erickson
 * PR40411: add letterBoxDetectionChange and GetLetterBoxStatus
 *
 * 6   3/11/08 2:10p erickson
 * PR40222: add NEXUS_VideoWindow_GetStatus to report video delay
 *
 * 5   2/29/08 5:39p erickson
 * PR40156: implement ColorMatrix function
 *
 * 4   1/31/08 9:55a erickson
 * PR36808: added SyncChannel interface
 *
 * 3   1/23/08 9:22p vobadm
 * PR35457: update docs
 *
 * 2   1/23/08 12:39p erickson
 * PR38919: rename NEXUS_Display_GetWindow to NEXUS_VideoWindow_Open
 *
 * 1   1/18/08 2:20p jgarrett
 * PR 38808: Merging to main branch
 *
 * Nexus_Devel/2   1/8/08 11:03a erickson
 * PR36159: refactor some 3563 Display API's
 *
 * Nexus_Devel/1   1/3/08 8:43p erickson
 * PR34662: refactor into smaller files
 *
 ***************************************************************************/
/* this file shall be included only from nexus_display_module.h */
#ifndef NEXUS_VIDEO_WINDOW_IMPL_H__
#define NEXUS_VIDEO_WINDOW_IMPL_H__

void NEXUS_VideoWindow_P_InitState(NEXUS_VideoWindowHandle window, unsigned index, NEXUS_DisplayHandle display);
BERR_Code NEXUS_VideoWindow_P_SetVdcSettings(NEXUS_VideoWindowHandle window, const NEXUS_VideoWindowSettings *settings, bool force);

BERR_Code NEXUS_VideoWindow_P_CreateVdcWindow(NEXUS_VideoWindowHandle window, const NEXUS_VideoWindowSettings *cfg);
void NEXUS_VideoWindow_P_DestroyVdcWindow(NEXUS_VideoWindowHandle window);

NEXUS_Error NEXUS_VideoWindow_P_AddMosaicInput(NEXUS_VideoWindowHandle window, NEXUS_VideoInput input);
void NEXUS_VideoWindow_P_RemoveMosaicInput(NEXUS_VideoWindowHandle window, NEXUS_VideoInput input);
NEXUS_Error NEXUS_VideoWindow_P_ApplyMosaic(NEXUS_VideoWindowHandle window);
const BAVC_MFD_Picture *NEXUS_VideoWindow_P_CutBackendMosaic_isr(NEXUS_VideoWindowHandle window, const BAVC_MFD_Picture *pPicture);

NEXUS_Error NEXUS_VideoWindow_P_ConfigMasterFrameRate(NEXUS_VideoWindowHandle window, const NEXUS_DisplaySettings *pDisplaySettings, const NEXUS_VideoWindowSettings *pWindowsettings);
void NEXUS_VideoWindow_P_UpdatePhaseDelay_isr(NEXUS_VideoWindowHandle window, unsigned refreshRate);

struct NEXUS_VideoWindow {
    NEXUS_OBJECT(NEXUS_VideoWindow);
    NEXUS_DisplayHandle display; /* pointer back to display */
    NEXUS_VideoWindowSettings cfg;
    NEXUS_VideoWindowSplitScreenSettings splitScreenSettings;
    NEXUS_VideoWindowStatus status;
    int phaseDelay; /* internal variable used to store phase delay as communicated by VDC, necessary for computing public phase delay later */
    bool open;
    bool layoutChanged;
    NEXUS_IsrCallbackHandle letterBoxDetectionCallback;
    NEXUS_IsrCallbackHandle outputRectChangedCallback;
    NEXUS_VideoWindowLetterBoxStatus letterBoxStatus;
    BKNI_EventHandle lb_event; /* for pcinput auto position lb return data */

    NEXUS_SurfaceHandle captureBuffer; /* current surface returned by NEXUS_VideoWindow_CaptureVideoBuffer */
    BSUR_Surface_Handle hRCaptureBuffer; /* 3D right-view surface not returned by NEXUS_VideoWindow_CaptureVideoBuffer */

    NEXUS_VideoWindowSyncSettings syncSettings;
    NEXUS_VideoWindowSyncStatus syncStatus;

    NEXUS_PixelFormat pixelFormat;

    NEXUS_ColorMatrix colorMatrix;
    bool colorMatrixSet;
    bool colorMatrixOverride;
#if NEXUS_HAS_CM3D
    NEXUS_ColorMatrix colorMatrixA;
    bool colorMatrixASet;
    bool colorMatrixAOverride;
    NEXUS_ColorMatrix colorMatrixB;
    bool colorMatrixBSet;
    bool colorMatrixBOverride;
    NEXUS_ColorMatrix colorMatrixC;
    bool colorMatrixCSet;
    bool colorMatrixCOverride;
#endif

    NEXUS_VideoWindowAfdSettings afdSettings;
    bool afdSet;

    unsigned index; /* 0 for main, 1 for pip. windowId is redundant and deprecated. */
    BVDC_WindowId windowId;
    BVDC_Heap_Handle vdcHeap;

#if NEXUS_NUM_MOSAIC_DECODES
    struct {
        /* fields for mosaic parent */
        BLST_S_HEAD(NEXUS_VideoInput_P_MosaicChildren, NEXUS_VideoWindow) children;

        /* fields for mosaic children */
        NEXUS_VideoWindowHandle parent;
        BLST_S_ENTRY(NEXUS_VideoWindow) link;
        unsigned userIndex; /* external window index, only used for duplicate index checking */
        NEXUS_VideoWindowMosaicSettings mosaicSettings;
        BAVC_MFD_Picture picture; /* used for backend */
    } mosaic;
#endif

    NEXUS_VideoAdjContext adjContext;
    NEXUS_PictureCtrlContext picContext;
    NEXUS_VideoInput input;
    NEXUS_VideoInput standby;    
    struct {
        BVDC_Window_Handle window; /* could be NULL */
        bool masterFrameRate;
    } vdcState;

    struct
    {
        bool mnrBnrPermitted;
        bool dcrPermitted;
        bool anrPermitted;
        NEXUS_PixelFormat anrMaxFormat;
        bool madPermitted;
        unsigned madFieldStore;
        NEXUS_PixelFormat madMaxFormat;
        bool forceCapture;
        NEXUS_PixelFormat maxFormat;
        bool forceBox;
        bool forceFull;
    } restrictions;

    unsigned rtsId;
};

NEXUS_OBJECT_CLASS_DECLARE(NEXUS_VideoWindow);
#endif /* NEXUS_VIDEO_WINDOW_IMPL_H__ */
