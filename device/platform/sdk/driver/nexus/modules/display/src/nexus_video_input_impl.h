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
 * $brcm_Workfile: nexus_video_input_impl.h $
 * $brcm_Revision: 71 $
 * $brcm_Date: 8/3/12 7:03p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/display/src/nexus_video_input_impl.h $
 * 
 * 71   8/3/12 7:03p bandrews
 * SW7420-2314: merge to main
 * 
 * SW7420-2314/2   7/23/12 7:41p bandrews
 * SW7420-2314: merge from main
 * 
 * SW7420-2314/1   7/23/12 7:16p bandrews
 * SW7420-2314: add event for window source changing
 * 
 * 70   7/13/12 3:39p erickson
 * SW7425-3107: refactor cc/tt/gs capture to prevent isr race-condition,
 *  prevent cpu-reordering failure, and to reduce code duplication
 * 
 * 69   7/9/12 3:00p erickson
 * SW7435-259: remove unused window param from NEXUS_VideoInput_P_Connect
 * 
 * 68   3/23/12 12:08p jtna
 * SW7346-725: use NEXUS_VideoInput_P_GetExisting() to avoid creating VDC
 *  source when not needed
 * 
 * 67   3/14/12 12:25p erickson
 * SW7401-4517: remove old code
 * 
 * 66   1/31/12 5:01p gmohile
 * SW7425-1473 : Fix raaga encoder support
 * 
 * 65   12/21/11 12:48p erickson
 * SW7425-1971: improve debug, remove unused code
 * 
 * 64   12/20/11 9:42a erickson
 * SW7346-617: move BVBI_TT_Line array off stack to avoid potential isr-
 *  time overflow
 * 
 * 63   12/16/11 5:28p erickson
 * SW7425-1878: merge XUDlib support for video encoder userdata
 * 
 * SW7425-1878/2   12/16/11 2:11p ajagmag
 * SW7425-1878: Update based on discussions with David E.
 * 
 * SW7425-1878/1   12/15/11 10:31a ajagmag
 * SW7425-1878: changes for user data encode use case
 * 
 * 62   11/15/11 3:27p erickson
 * SW7425-1747: remove old code
 * 
 * 61   9/15/10 11:44a petlee
 * SW35230-1156: Fix logic on how hdmi gets connected to hddvi
 * 
 * 60   3/26/10 2:19p erickson
 * SW7405-3965: refactor mosaic to support hd/sd simul mode
 * 
 * 59   3/23/10 3:25p petlee
 * SW35230-106: Merging branch SW35230-106 to mainline
 * 
 * SW35230-106a/6   3/22/10 11:44a petlee
 * SW35230-106: Minor API updates according to code review
 * 
 * SW35230-106a/5   3/10/10 6:26p petlee
 * SW35230-106: Move NEXUS_VideoInput_P_Create_VdcSource out of #if
 * 
 * SW35230-106a/4   3/9/10 2:00p petlee
 * SW35230-106: Update the re-work done for VideoInput.
 * 
 * SW35230-106a/3   2/26/10 3:12p petlee
 * SW35230-106: Remove unused private APIs
 * 
 * SW35230-106a/2   2/26/10 10:34a petlee
 * SW35230-106: Handle cases where sourceVdc is used when it has not been
 *  created yet. Public APIs should store state and set it later once
 *  sourceVdc gets created.
 * 
 * SW35230-106a/1   2/25/10 3:02p petlee
 * SW35230-106: Add new private APIs for managing the link object
 * 
 * 58   2/22/10 12:50p erickson
 * SW7405-3625: update backendMosaic impl
 *
 * 57   2/19/10 1:23p erickson
 * SW7405-3625: add NEXUS_VideoWindowMosaicSettings for backendMosaic
 *  support
 *
 * 56   2/10/10 11:57a erickson
 * SWDEPRECATED-2425: added comments about mapping of link state to public
 *  API functions
 *
 * 55   12/9/09 3:10p erickson
 * SW7405-3394: refactor mosaic mode API
 *
 * 54   9/17/09 3:01p erickson
 * SW3548-2348: remove unused prototype
 *
 * 53   8/10/09 10:09a erickson
 * PR56325: only feedback display information to sync-locked inputs
 *
 * 52   8/10/09 8:17a gmohile
 * PR 56400 : Merge legacy vdc support to main line
 *
 * 51   7/23/09 10:39a erickson
 * PR56969: add hddvi input
 *
 * 50   7/22/09 11:40a erickson
 * PR56997: separate AnalogVideoDecoder code
 *
 * 49   5/20/09 3:29p erickson
 * PR54880: keep track of whether VDC heaps were created and only destroy
 *  what was created
 *
 * 48   4/6/09 11:49a erickson
 * PR53137: keep VDEC up if any input is connected to AnalogVideoDecoder.
 *
 * 47   3/26/09 10:49a erickson
 * PR53613: allow NEXUS_VideoInput_GetColorMatrix to read from VDC is user
 *  has not set the NEXUS_ColorMatrix
 *
 * 46   3/16/09 11:07a erickson
 * PR53248: remove badParityBlock filter, add
 *  NEXUS_ClosedCaptionData.parityError.
 *
 * 45   3/4/09 1:03p jgarrett
 * PR 52618: Adding forcePcFormat option to HdmiInput
 *
 * 44   2/27/09 2:07p erickson
 * PR32888: increase NEXUS_VIDEO_INPUT_CRC_QUEUE_SIZE to s0
 *
 * 43   2/27/09 10:17a erickson
 * PR52115: provide filter on bad CC data
 *
 * 42   1/9/09 2:38a erickson
 * PR50502: defer source pending callback until the video has an active
 *  picture so that there's a valid format
 *
 * 41   1/6/09 11:33a jgarrett
 * PR 48984: Merge dynamic RTS code to main branch
 *
 * PR48984/3   12/10/08 11:29a jgarrett
 * PR 48984: Merge to latest baseline
 *
 * 40   12/8/08 5:04p erickson
 * PR50058: rework internal VideoInput vbi accounting, don't store refcnt
 *
 * PR48984/2   11/26/08 6:19p jgarrett
 * PR 48984: Merging to latest baseline
 *
 * 39   11/21/08 12:53p erickson
 * PR43413: added vps decode
 *
 * PR48984/1   11/20/08 6:47p jgarrett
 * PR 48984: Adding vdb restrictions
 *
 * 38   11/6/08 5:22p erickson
 * PR47030: rename NEXUS_VideoInput_P_DestroyLink
 *
 * 37   11/4/08 12:38p erickson
 * PR47030: add NEXUS_Display_P_DestroyHeap, refactor so that all VDC
 *  Sources are created by NEXUS_VideoInput_P_CreateLink
 *
 * 36   11/3/08 2:27p erickson
 * PR47030: add HdmiInput heap
 *
 * 35   10/30/08 8:56p erickson
 * PR47030: use NEXUS_HeapHandle for per-window and per-source heaps
 *
 * 34   10/17/08 3:23p erickson
 * PR47030: impl shared analog/digital heap
 *
 * 33   10/16/08 5:25p erickson
 * PR48006: call NEXUS_AnalogVideoDecoder_P_SetVbi with VDC source
 *  callback format
 *
 * 32   10/7/08 9:21p erickson
 * PR47070: added gemstar refcnt
 *
 * 31   10/2/08 12:55p erickson
 * PR40799: create/destroy BVDC_Source when AnalogVideoDecoder is
 *  connected/disconnected
 *
 * 30   9/26/08 11:58a erickson
 * PR46515: add VDEC and VBI power management
 *
 * 29   9/24/08 5:21p erickson
 * PR47070: add vbi closedCaption, teletext and gemStar DataReady
 *  callbacks
 *
 * 28   9/18/08 12:53p erickson
 * PR47070: added gemstar options to NEXUS_VideoInputVbiSettings
 *
 * 27   8/22/08 11:03p erickson
 * PR45711: move functions from private API into internal header
 *
 * 26   8/20/08 12:10p erickson
 * PR45616: prefer data from VDC Source callback over
 *  BVDC_Source_GetInputStatus
 *
 * 25   7/3/08 5:07p erickson
 * PR43699: Remove unnecessary VDC Get functions from Nexus to avoid race
 *  conditions
 *
 * 24   6/23/08 9:12a erickson
 * PR40922: merge
 *
 * PR40922/1   6/20/08 8:36p mlei
 * PR40922:Add SCART Input support
 *
 * 23   6/16/08 1:56p erickson
 * PR43504: added GemStar
 *
 * 22   6/11/08 12:46p vsilyaev
 * PR 43491: Added support for CCIR-656 input
 *
 * 21   5/20/08 4:34p erickson
 * PR41202: fixed VDC source/window heap support for WXGA support
 *
 * 20   5/9/08 12:44p erickson
 * PR39453: add sourcePending callback, set status.sourcePending based on
 *  VDC dirty bits.
 *
 * 19   4/30/08 3:08p erickson
 * PR39453: add 3548/3556 features
 *
 * 18   4/28/08 4:20p erickson
 * PR41533: update 3563 vbi
 *
 * 17   4/23/08 4:29p erickson
 * PR42071: added developer comments on NEXUS_VideoInput_P_Link state
 *
 * 16   4/11/08 11:06a vsilyaev
 * PR 40410: Fixed naming typo
 *
 * 15   4/7/08 4:10p erickson
 * PR32888: added NEXUS_CRC_CAPTURE
 *
 * 14   3/27/08 4:22p vsilyaev
 * PR 40818: Improved internal API to allow source private heaps
 *
 * 13   3/27/08 10:56a erickson
 * PR40742: fix 7401
 *
 * 12   3/26/08 11:44p vsilyaev
 * PR 40410: Added more video settings for HDMI Input interface
 *
 * 11   3/26/08 3:38p vsilyaev
 * PR 40862: Added code to propagate display framerate to the decoder
 *
 * 10   3/26/08 12:53p erickson
 * PR40742: fix warning
 *
 * 9   3/26/08 10:54a erickson
 * PR40742: added more HdmiInput which is passed in from VDC source
 *  changed callback
 *
 * 8   3/24/08 3:20p erickson
 * PR38442: reworked SetAdc logic into NEXUS_AnalogVideoDecoder_P_SetAdc
 *  function.
 *
 * 7   3/20/08 10:23a erickson
 * PR40716: set Timebase framerate when analog framerate changes
 *
 * 6   3/12/08 5:25p erickson
 * PR40412: get analog VideoInputStatus from the VDC source callback.
 *  BVDC_Source_GetInputStatus does not return reliable data.
 *
 * 5   3/12/08 4:32p erickson
 * PR40412: fixed use of BVDC_Source_InstallCallback
 *
 * 4   3/6/08 3:08p erickson
 * PR40103: convert to NEXUS_TaskCallback
 *
 * 3   2/1/08 3:20p erickson
 * PR38679: add vbi support
 *
 * 2   1/31/08 9:55a erickson
 * PR36808: added SyncChannel interface
 *
 * 1   1/18/08 2:20p jgarrett
 * PR 38808: Merging to main branch
 *
 * Nexus_Devel/5   1/3/08 9:20p erickson
 * PR38213: rename to AnalogVideoDecoder
 *
 * Nexus_Devel/4   1/3/08 8:43p erickson
 * PR34662: refactor into smaller files
 *
 * Nexus_Devel/3   12/18/07 1:35p erickson
 * PR38213: analog video input rework
 *
 * Nexus_Devel/2   11/26/07 12:50p vsilyaev
 * PR36159: Merged 3563 code
 *
 * Nexus_Devel/PR36159/1   11/26/07 10:10a gezhang
 * PR36159: Bring up Nexus on 93563 platform
 *
 * Nexus_Devel/1   11/9/07 11:16a erickson
 * PR36814: added more VDC HdDvi settings, moved to separate source file
 *
 * Nexus_Devel/5   11/2/07 4:42p vsilyaev
 * PR 36696: Used connector model for VideoInput's and VideoOutput's
 *
 * Nexus_Devel/4   10/4/07 1:09p vsilyaev
 * PR 34662: Removed VideoOutputDesc
 *
 * Nexus_Devel/3   9/25/07 3:12p vsilyaev
 * PR 34662: Fixed video output
 *
 * Nexus_Devel/2   9/25/07 1:02p vsilyaev
 * PR 34662: Fixed file names
 *
 * Nexus_Devel/1   9/25/07 12:49p vsilyaev
 * PR 34662: Internal API for display outputs
 *
 **************************************************************************/
/* this file shall be included only from nexus_display_module.h */
#ifndef NEXUS_VIDEO_INPUT_IMPL_H__
#define NEXUS_VIDEO_INPUT_IMPL_H__

/**
This section contains internal API's for the generic NEXUS_VideoInput connector.
**/

BDBG_OBJECT_ID_DECLARE(NEXUS_VideoInput_P_Link);

struct NEXUS_VideoInput_P_Link;

typedef struct NEXUS_VideoInput_P_Iface {
    BERR_Code (*connect)(struct NEXUS_VideoInput_P_Link *input);
    void (*disconnect)(struct NEXUS_VideoInput_P_Link *input);
} NEXUS_VideoInput_P_Iface;

/* The internal VBI settings combines the user's request via the VideoInput interface and
multiple Display interfaces. */
typedef struct NEXUS_VideoInputInternalVbiSettings {
    /* we must reference count these because multiple displays may enable/disable individual vbi types */
    unsigned teletextRefCnt;           /* Decode TT data and pass to callback */
    unsigned closedCaptionRefCnt;      /* Decode CC data and pass to callback */
    unsigned wssRefCnt;                /* Decode WSS data and pass to callback */
    unsigned cgmsRefCnt;               /* Decode CGMS data and pass to callback */
    unsigned gemStarRefCnt;            /* Decode GS data and pass to callback */
    unsigned vpsRefCnt;                /* Decode VPS data and pass to callback */
} NEXUS_VideoInputInternalVbiSettings;

/**
NEXUS_VideoInput_P_Link contains Display module information associated with a NEXUS_VideoInput (chiefly, BVDC_Source_Handle).

NEXUS_VideoInput.destination points to NEXUS_VideoInput_P_Link after the first NEXUS_VideoWindow_AddInput is called.
NEXUS_VideoInput.destination is cleared when NEXUS_VideoWindow_RemoveInput is called on the last window.
The reason NEXUS_VideoInput.destination is cleared even though NEXUS_VideoInput_P_Link is not destroyed is that's possible that
NEXUS_VideoInput could be connected to an interface for another module (not Display). This is not true at this time, but it could be.

When NEXUS_VideoInput is re-added to a window, any NEXUS_VideoInput_P_Link which was already created will be re-used. Its state will persist.
NEXUS_VideoInput_P_Link is only destroyed when NEXUS_VideoInput_Shutdown is called.

Also, when NEXUS_VideoInput.destination is NULL, any setting applied to NEXUS_VideoInput (e.g. NEXUS_VideoInput_SetSettings)
will be applied to the NEXUS_VideoInput_P_Link by means of a temporary lookup; the .destination will not be set.


Mapping of link state to public API functions:
1. NEXUS_VideoWindow_AddInput - creates link, VDC source & VDC window
2. NEXUS_VideoWindow_RemoveInput - destroys VDC window (not link or VDC source)
3. NEXUS_VideoInput_Get/Set - creates link (if not already created)
    TODO: it is not required that Get/Set will create the VDC source. for some chips, it's preferrable that it does not.
4. NEXUS_VideoInput_Shutdown - destroy link & VDC source (does implicit VideoWindow_RemoveInput if not already done)
**/

typedef struct NEXUS_VideoInput_P_Link_Fifo {
    const char *name; /* for debug */
    unsigned bufferSize; /* number of elements */
    unsigned elementSize; /* size of each element in bytes */
    void *data; /* size is bufferSize*elementSize */
    unsigned wptr, rptr; /* in units of elementSize */
    bool inactive; /* set to true for time when buffer is reallocated */
    NEXUS_IsrCallbackHandle isrCallback;
} NEXUS_VideoInput_P_Link_Fifo;

typedef struct NEXUS_VideoInput_P_Link {
    BDBG_OBJECT(NEXUS_VideoInput_P_Link)
    BVDC_Source_Handle sourceVdc;
    bool copiedSourceVdc;
    BAVC_SourceId id;
    NEXUS_VideoInput input;
    NEXUS_HeapHandle heap;

    NEXUS_VideoInputSettings cfg;
    NEXUS_VideoInputStatus status;
    NEXUS_VideoInputResumeMode resumeMode;
    bool bColorMatrixSet;
    NEXUS_ColorMatrix colorMatrix;

    bool isDeferCfg;
    bool isDeferColorMatrix;
    bool isDeferForcePending;

#if NEXUS_HAS_VIDEO_ENCODER && !NEXUS_NUM_DSP_VIDEO_ENCODERS
    BXUDlib_Handle hXud; /* transcode user data component handle */
#endif
 
    NEXUS_VideoInputVbiSettings vbiSettings; /* from VideoInput_SetVbiSettings */

    /* VBI data capture */
    struct {
        NEXUS_VideoInput_P_Link_Fifo cc;
        NEXUS_VideoInput_P_Link_Fifo gs;
        NEXUS_VideoInput_P_Link_Fifo tt;
#define B_VBI_TT_LINES 20
        BVBI_TT_Line ttLines[B_VBI_TT_LINES];
        struct {
            uint16_t data;
            NEXUS_IsrCallbackHandle isrCallback;
        } wss;
        struct {
            uint32_t data;
            NEXUS_IsrCallbackHandle isrCallback;
        } cgms;
        struct {
            NEXUS_VpsData data;
            NEXUS_IsrCallbackHandle isrCallback;
        } vps;
    } vbi;

#if NEXUS_CRC_CAPTURE
    struct {
#define NEXUS_VIDEO_INPUT_CRC_QUEUE_SIZE 80
        NEXUS_VideoInputCrcData data[NEXUS_VIDEO_INPUT_CRC_QUEUE_SIZE];
        unsigned wptr, rptr;
    } crc;
#endif

    NEXUS_IsrCallbackHandle sourcePendingCallback;
    NEXUS_IsrCallbackHandle sourceChangedCallback;
    BKNI_EventHandle sourceChangedEvent, sourcePendingEvent, checkFormatChangedEvent;
    NEXUS_EventCallbackHandle sourceChangedEventHandler, sourcePendingEventHandler, checkFormatChangedEventHandler;
    bool deferSourcePending; /* if we receive a source pending callback before a picture is active, wait until we get an active picture */

    BVDC_Source_CallbackData vdcSourceCallbackData; /* keep a copy of the VDC source callback data. this is more reliable than BVDC_Source_GetInputStatus.
                                                       be sure to enter a critical section before reading multiple items from this structure. */
    BVDC_Heap_Handle vdcHeap;
    NEXUS_VideoInput_P_Iface iface;
    BLST_S_ENTRY(NEXUS_VideoInput_P_Link) link;
    unsigned ref_cnt; /* number of connections */
    struct { /* copy from NEXUS_VideoInput it's used to verify that association is still valid */
        NEXUS_VideoInputType type; /* type of video input */
        void *source; /* polymorphic pointer to the source, must be not NULL */
    } input_info;
    bool info_valid;
    union {
        BAVC_MFD_Picture mfd; /* only valid for video decoder inputs */
#if NEXUS_NUM_HDMI_INPUTS
        NEXUS_VideoHdmiInputSettings hdmi; /* only valid for hdmi inputs */
#endif
    } info;
    struct {
        unsigned index; /* index from VideoDecoder for the mosaic array. only applies to child link. */
        bool backendMosaic;
        NEXUS_VideoWindowHandle parentWindow[NEXUS_NUM_DISPLAYS]; /* only applies to parent link */
    } mosaic;
    struct
    {
        NEXUS_Callback connectionChangedCallback_isr; /* Notify SyncChannel when an input connection changes. */
        void * callbackContext;
    } sync;
} NEXUS_VideoInput_P_Link;

typedef struct NEXUS_VideoInput_P_LinkData {
    BAVC_SourceId sourceId;
    NEXUS_HeapHandle heap;
    BVDC_Source_Handle sourceVdc;
} NEXUS_VideoInput_P_LinkData;

/* initializes mandatory fields of NEXUS_VideoInput_P_LinkData */
void NEXUS_VideoInput_P_LinkData_Init(NEXUS_VideoInput_P_LinkData *data, BAVC_SourceId sourceId);
void NEXUS_VideoInput_P_TriggerSourceChanged_isr(NEXUS_VideoInput_P_Link *link, bool sourcePending);
NEXUS_VideoInput_P_Link *NEXUS_VideoInput_P_CreateLink( NEXUS_VideoInput source, const NEXUS_VideoInput_P_LinkData *data, const NEXUS_VideoInput_P_Iface *iface );
void NEXUS_VideoInput_P_DestroyLink( NEXUS_VideoInput_P_Link *link );
NEXUS_VideoInput_P_Link *NEXUS_VideoInput_P_GetExisting( NEXUS_VideoInput input );
NEXUS_VideoInput_P_Link *NEXUS_VideoInput_P_Get( NEXUS_VideoInput input );
BERR_Code NEXUS_VideoInput_P_Connect( NEXUS_VideoInput input );
void NEXUS_VideoInput_P_Disconnect( NEXUS_VideoInput input );
/* this function called after display settings were changed */
void NEXUS_Display_P_VideoInputDisplayUpdate(NEXUS_VideoInput input, NEXUS_VideoWindowHandle window, const NEXUS_DisplaySettings *pSettings);
void NEXUS_VideoInput_P_VbiData_isr(NEXUS_VideoInput videoInput, const BVBI_Field_Handle *pVbiData, const NEXUS_ClosedCaptionData *pData, unsigned vbiCount);

/* tell the VideoInput to reset its VBI state based on its own settings and every display's VBI settings */
NEXUS_Error NEXUS_VideoInput_P_SetVbiState( NEXUS_VideoInput videoInput );

/* Notify a potential format change from a down module to display */
void NEXUS_VideoInput_P_CheckFormatChange_isr(void *pParam);

/**
This section contains internal API's for specific video inputs.
These should be moved into separate _impl.h header files.
**/

#if NEXUS_NUM_HDMI_INPUTS
NEXUS_VideoInput_P_Link *NEXUS_VideoInput_P_OpenHdmi(NEXUS_VideoInput input);
void NEXUS_VideoInput_P_CloseHdmi(NEXUS_VideoInput_P_Link *link);
void NEXUS_VideoInput_P_SetHdmiInputStatus(NEXUS_VideoInput_P_Link *link);
void NEXUS_VideoInput_P_HdmiSourceCallback_isr(NEXUS_VideoInput_P_Link *link, const BVDC_Source_CallbackData *data);
#endif

#if NEXUS_NUM_656_INPUTS
NEXUS_VideoInput_P_Link *NEXUS_VideoInput_P_OpenCcir656(NEXUS_VideoInput input);
#endif

#if NEXUS_NUM_HDDVI_INPUTS
NEXUS_VideoInput_P_Link *NEXUS_VideoInput_P_OpenHdDviInput(NEXUS_VideoInput input);
#endif

#if NEXUS_NUM_VIDEO_DECODERS
NEXUS_VideoInput_P_Link *NEXUS_VideoInput_P_OpenDecoder(NEXUS_VideoInput input, NEXUS_VideoInput_P_Link *mosaicParent);
#endif

#endif
