/***************************************************************************
 *     Copyright (c) 2002-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: control_libtune.cpp $
 * $brcm_Revision: 9 $
 * $brcm_Date: 12/8/10 8:27p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/control_libtune.cpp $
 * 
 * 9   12/8/10 8:27p bandrews
 * SW35230-1320: merged libtune updates to main branch, needed to add
 * instance index to brutus
 * 
 * 8   6/2/10 9:24p bandrews
 * SW3548-2323: updated to use only window functions
 * 
 * 7   5/6/10 10:45a jtna
 * SW7420-527: added per-handle cache invalidation for NEXUS_VideoWindows
 * 
 * 6   5/5/10 5:05p jtna
 * SW7420-527: moved memset back to control.cpp and added more debug
 * statements
 * 
 * 5   4/20/10 11:57a jtna
 * SW7420-527: force a full retune only on a source change
 * 
 * 4   4/19/10 11:44a jtna
 * SW7420-527: force a full retune everytime
 * 
 * 3   4/8/10 2:20p jtna
 * SW7420-527: update to tune_app_defs.h
 * 
 * 2   3/30/10 7:49p jtna
 * SW7420-527: add support for uiControl
 * 
 * 1   3/30/10 12:43p jtna
 * SW7420-527: added control_libtune.cpp
 *
 ***************************************************************************/

#include "control.h"
#include "cfgsettings.h"

#include "bsettop.h"

#ifdef NEXUS_BUILD
#include "nexus_platform.h"
#include "nexus_core_utils.h"
#include "nexus_display.h"
#include "nexus_video_window.h"

#define BSETTOP_NO_API_THUNKS
extern "C" {
    #include "bsettop_types.h"
    #include "bsettop_impl.h"
}

#endif

BDBG_MODULE(brutus_control_libtune);

/* Brutus implements libtune support by accessing Nexus and shim internals directly */
#ifdef BRUTUS_LIBTUNE_SUPPORT

#include "nexus_video_window_tune.h"
#include "tune_app_defs.h"

static int Libtune_WindowWrapper(void * context, const uint32_t aFields[], const uint32_t aValues[], int num)
{
    return NEXUS_VideoWindow_SetTuneParams((NEXUS_VideoWindowHandle)context, aFields, aValues, num);
}

int Control::Libtune_Init(DecodeEngine const * const de, bdecode_window_t const * const window)
{
    const char* dbName = cfg()->get(Config::LIBTUNE_DBNAME, NULL);
    LIBTUNE_OPEN_SETTINGS settings;
    LIBTUNE_RESULT rc;

    if (dbName == NULL) {
        BDBG_ERR(("No database name specified (LIBTUNE_DBNAME=NULL). Aborting init."));
        return -1;
    }

    _check_libtune = false;

    LibTune_GetDefaultOpenSettings(&settings);
    settings.shareMode = LIBTUNE_MODE_SHARED;
    settings.tuningDatabaseFilename = (char*)dbName;

    /* save handles */
    DecodeEngine::Settings deSettings;
    de->getSettings(&deSettings);
    libtune.vdecoder = deSettings.decode->video_decode->nVideoDecoder;
    libtune.de = (DecodeEngine*)de;
    BDBG_ASSERT(libtune.vdecoder);

    /* assume single input. there's no easy way to register the source_change callback for the second input source, if there is one */
    libtune.input = NEXUS_VideoDecoder_GetConnector(libtune.vdecoder);
    libtune.window[0] = window[0]->nWindow;
    libtune.display[0] = window[0]->display->nDisplay;
    libtune.window[1] = window[2]->nWindow;
    libtune.display[1] = window[2]->display->nDisplay;

    /* open libtune */
    rc = LibTune_Open(&libtune.handle, &settings);
    BDBG_MSG(("Libtune_Init: %p", libtune.handle));
    if (rc != LIBTUNE_OK) {
        BDBG_ERR(("LibTune_Open() error: Error Code = %d\n", rc));
        libtune.handle = NULL;
        return (int)rc;
    }

#if 1
    /* temporary workaround that will call BVDC_Window_SetSharpness with enable=true */
    NEXUS_PictureCtrlCommonSettings pccSettings;
    NEXUS_PictureCtrl_GetCommonSettings(libtune.window[0], &pccSettings);
    NEXUS_PictureCtrl_SetCommonSettings(libtune.window[0], &pccSettings);
    NEXUS_PictureCtrl_GetCommonSettings(libtune.window[1], &pccSettings);
    NEXUS_PictureCtrl_SetCommonSettings(libtune.window[1], &pccSettings);
#endif

    /* set up interface handlers */
    LibTune_SetInterfaceTuningHandler(libtune.handle, 1, &Libtune_WindowWrapper);

    /* set up default key context */
    bdisplay_settings dsettings1, dsettings2;
    bdisplay_get(display()->display, &dsettings1);
    bdisplay_get(display()->display2, &dsettings2);

    libtune.sourceFormat = NEXUS_VideoFormat_eNtsc; /* video_source_changed is always called, so this is a don't care */
    libtune.displayFormat[0] = b_displayformat2nexus(dsettings1.format, NULL);
    libtune.displayFormat[1] = b_displayformat2nexus(dsettings2.format, NULL);
    libtune.userMode = Libtune_UserMode_eStandard;

    /* do not apply window settings on startup; the source needs to be active before applying the settings */

    return 0;
}

int Control::Libtune_CreateKey(int idx)
{
    unsigned int key = 0;

    switch (libtune.sourceFormat) {
        case NEXUS_VideoFormat_e1080i:
            key |= TUNE_KEY_FIELD_INSIZE_HD;
            key |= TUNE_KEY_FIELD_INFORMAT_I;
            break;
        case NEXUS_VideoFormat_e720p:
            key |= TUNE_KEY_FIELD_INSIZE_HD;
            key |= TUNE_KEY_FIELD_INFORMAT_P;
            break;
        case NEXUS_VideoFormat_e480p:
            key |= TUNE_KEY_FIELD_INSIZE_SD;
            key |= TUNE_KEY_FIELD_INFORMAT_P;
            break;
        case NEXUS_VideoFormat_eNtsc:
        default:
            key |= TUNE_KEY_FIELD_INSIZE_SD;
            key |= TUNE_KEY_FIELD_INFORMAT_I;
            break;
    }

    switch (libtune.displayFormat[idx]) {
        case NEXUS_VideoFormat_e1080i:
            key |= TUNE_KEY_FIELD_OUTSIZE_HD;
            key |= TUNE_KEY_FIELD_OUTFORMAT_I;
            break;
        case NEXUS_VideoFormat_e720p:
            key |= TUNE_KEY_FIELD_OUTSIZE_HD;
            key |= TUNE_KEY_FIELD_OUTFORMAT_P;
            break;
        case NEXUS_VideoFormat_e480p:
            key |= TUNE_KEY_FIELD_OUTSIZE_SD;
            key |= TUNE_KEY_FIELD_OUTFORMAT_P;
            break;
        case NEXUS_VideoFormat_eNtsc:
        default:
            key |= TUNE_KEY_FIELD_OUTSIZE_SD;
            key |= TUNE_KEY_FIELD_OUTFORMAT_I;
            break;
    }

    switch (libtune.userMode) {
        case Libtune_UserMode_eVivid:
            key |= TUNE_KEY_FIELD_USERMODE_VIVID;
            break;
        case Libtune_UserMode_eStandard:
        default:
            key |= TUNE_KEY_FIELD_USERMODE_STD;
            break;
    }

    switch (idx) {
        case 1:
            key |= TUNE_KEY_FIELD_WINDOW_W1;
            break;
        case 0:
        default:
            key |= TUNE_KEY_FIELD_WINDOW_W0;
            break;
    }

    return key;
}

void Control::Libtune_TuneWindow(int idx)
{
    LIBTUNE_RESULT rc = LIBTUNE_OK;
    LIBTUNE_INTERFACE_HANDLE_DESCRIPTOR handles[1];
    LIBTUNE_UI_CONTROL_DESCRIPTOR controls[1];

    if (!libtune.handle) {
        return;
    }

    handles[0].handle = libtune.window[idx];
    handles[0].instanceIndex = idx;
    handles[0].interfaceId = 1;

    controls[0].groupId = 0;
    controls[0].value = libtune.uiValue;
    rc = LibTune_Tune(libtune.handle, 1, handles, 1, controls, Libtune_CreateKey(idx));
    if (rc != LIBTUNE_OK) {
        BDBG_WRN(("Tune: Error Code = %d\n", rc));
    }
    return;
}

void Control::Libtune_TuneWindows()
{
    if (!libtune.handle) {
        return;
    }

    /* Brutus is implemented such that DecodeEngine.startLive() calls NEXUS_VideoWindow_AddInput(),
       which is a necessary call before NEXUS_VideoWindow_SetTuneParams() */
    if (!libtune.de->isActive()) {
        return;
    }

    BDBG_MSG(("Libtune_TuneWindows: %p", libtune.handle));

    /* Brutus can run in one of 4 modes. in each case, there are two windows */
    Libtune_TuneWindow(0);
    Libtune_TuneWindow(1);
}

void Control::Libtune_SetUserMode(int usermode)
{
    libtune.userMode = (LibTune_UserMode)usermode;
}

void Control::Libtune_SetUiControlValue(unsigned value)
{
    libtune.uiValue = value;
}

void Control::Libtune_SetDisplayFormat()
{
    /* set up default key context */
    bdisplay_settings dsettings1, dsettings2;
    bdisplay_get(display()->display, &dsettings1);
    bdisplay_get(display()->display2, &dsettings2);

    libtune.displayFormat[0] = b_displayformat2nexus(dsettings1.format, NULL);
    libtune.displayFormat[1] = b_displayformat2nexus(dsettings2.format, NULL);
}

void Control::Libtune_SourceChangeHandler()
{
    LIBTUNE_RESULT rc = LIBTUNE_OK;
    LIBTUNE_INTERFACE_HANDLE_DESCRIPTOR handles[1];
    NEXUS_VideoDecoderStatus status;

    NEXUS_VideoDecoder_GetStatus(libtune.vdecoder, &status);
    libtune.sourceFormat = status.format;

    /* for whatever reason, the sourceChanged callback fires twice, once with VideoFormat_eNtsc, source 0x0, display 0x0, 
       followed by the correct video format and sizes. ignore the first one */
    if (status.source.width == 0 && status.source.height == 0) {
        return;
    }

    BDBG_WRN(("Libtune_SourceChangeHandler: format %d, source %dx%d, display %dx%d", 
        status.format, status.source.width, status.source.height, status.display.width, status.display.height));

#if 0
    /* whenever bdecode_start() is called, it calls NEXUS_VideoWindow_RemoveInput() and 
       _AddInput(), which respectively destroy and create a VDC window. This, in turn, causes 
       BVDC_P_Window_Sharpness_Init() to get called, overriding some previous settings.
       as a workaround, we must force a full retune whenever bdecode_start() is called. 

       putting the workaround inside the sourceChanged callback is slightly overkill, 
       in the sense that, eg. a source size change will trigger this without having destroyed/created a 
       VDC window. however, this is the least intrusive way of doing things inside brutus */
    LibTune_ForceFullRetune(libtune.handle);
#else
    /* there is now per-handle cache invalidation support */
    handles[0].handle = libtune.window[0];
    handles[0].instanceIndex = 0;
    handles[0].interfaceId = 1;
    LibTune_ForceRetuneByHandle(libtune.handle, 1, handles);

    handles[0].handle = libtune.window[1];
    handles[0].instanceIndex = 1;
    handles[0].interfaceId = 1;
    LibTune_ForceRetuneByHandle(libtune.handle, 1, handles);
#endif
    
    Libtune_TuneWindows();
    return;
}

#endif /* BRUTUS_LIBTUNE_SUPPORT */

