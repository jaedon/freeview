/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsettop_vbi.c $
 * $brcm_Revision: 9 $
 * $brcm_Date: 3/15/12 12:22p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/src/nexus/bsettop_vbi.c $
 * 
 * 9   3/15/12 12:22p erickson
 * SW7425-1747: remove old code
 * 
 * 8   8/14/08 11:25a vishk
 * PR 4537: bstream_vbi_settings structure call-back is not being called
 * when data is ready
 * 
 * 7   6/24/08 4:28p erickson
 * PR36068: fix warning
 *
 * 6   6/23/08 2:45p erickson
 * PR36068: no VBI encoding on DTV platform
 *
 * 5   5/19/08 3:04p erickson
 * PR42595: assign NEXUS_DisplayVbiSettings.vbiSource
 *
 * 4   4/3/08 5:42p jgarrett
 * PR 41312: Setting callback events
 *
 * 3   3/19/08 3:12p erickson
 * PR36068: remove WRN
 *
 * 2   3/11/08 11:10a erickson
 * PR36068: impl vbi
 *
 * 1   10/15/07 2:36p erickson
 * PR36068: initial
 *
 ***************************************************************************/
#include "bsettop_impl.h"
#include "nexus_video_input_vbi.h"
#include "nexus_display_vbi.h"

BDBG_MODULE(vbi);

static NEXUS_VideoInput bstream_p_get_video_input(bstream_t stream)
{
    if (stream->consumers.decode && stream->consumers.decode->video_decode) {
        if (stream->consumers.decode->video_decode->nVideoDecoder) {
            return NEXUS_VideoDecoder_GetConnector(stream->consumers.decode->video_decode->nVideoDecoder);
        }
    }
    return NULL;
}

void bstream_vbi_get_settings(bstream_t stream, bstream_vbi_settings *settings)
{
    *settings = stream->vbi_settings;
}

static void cc_data_ready_callback(void *context)
{
    bstream_t stream = context;
    if (stream->vbi_settings.cc_data_ready_callback) {
        b_unlock();
        (*stream->vbi_settings.cc_data_ready_callback)(stream->vbi_settings.callback_context);
        b_lock();
    }
    stream->cc_data_ready_timer = b_timer_schedule(15, cc_data_ready_callback, stream);
}

bresult bstream_vbi_set_settings(bstream_t stream, const bstream_vbi_settings *settings)
{
    NEXUS_DisplayVbiSettings displayVbiSettings;
    NEXUS_Error rc;

    /* ignore settings->cc.line & settings->tt.lines */

    if (settings->cc.display) {
        NEXUS_Display_GetVbiSettings(settings->cc.display->nDisplay, &displayVbiSettings);
        displayVbiSettings.closedCaptionRouting = true;
        displayVbiSettings.closedCaptionEnabled = true;
        displayVbiSettings.vbiSource = bstream_p_get_video_input(stream);
        rc = NEXUS_Display_SetVbiSettings(settings->cc.display->nDisplay, &displayVbiSettings);
        if (rc) return BSETTOP_ERROR(rc);
    }
    else if (stream->vbi_settings.cc.display) {
        NEXUS_Display_GetVbiSettings(stream->vbi_settings.cc.display->nDisplay, &displayVbiSettings);
        displayVbiSettings.closedCaptionRouting = false;
        displayVbiSettings.closedCaptionEnabled = false;
        rc = NEXUS_Display_SetVbiSettings(stream->vbi_settings.cc.display->nDisplay, &displayVbiSettings);
        if (rc) return BSETTOP_ERROR(rc);
    }

    if (settings->tt.display) {
        NEXUS_Display_GetVbiSettings(settings->tt.display->nDisplay, &displayVbiSettings);
        displayVbiSettings.teletextRouting = true;
        displayVbiSettings.teletextEnabled = true;
        displayVbiSettings.vbiSource = bstream_p_get_video_input(stream);
        rc = NEXUS_Display_SetVbiSettings(settings->tt.display->nDisplay, &displayVbiSettings);
        if (rc) return BSETTOP_ERROR(rc);
    }
    else if (stream->vbi_settings.tt.display) {
        NEXUS_Display_GetVbiSettings(stream->vbi_settings.tt.display->nDisplay, &displayVbiSettings);
        displayVbiSettings.teletextRouting = false;
        displayVbiSettings.teletextEnabled = false;
        rc = NEXUS_Display_SetVbiSettings(stream->vbi_settings.tt.display->nDisplay, &displayVbiSettings);
        if (rc) return BSETTOP_ERROR(rc);
    }

    /* don't use settings->cgms.display or settings->wss.display. this is
    automatically enabled in the set functions. */

    /* Nexus has no callback for cc data ready. If cc parsing is enabled, CC608 data will be available at 1 entry per vsync.
    The app can time it easily. */
    if (settings->cc_data_ready_callback) {
        if (!stream->cc_data_ready_timer) {
            stream->vbi_settings.cc_data_ready_callback = settings->cc_data_ready_callback;
            stream->vbi_settings.callback_context = settings->callback_context;
            stream->cc_data_ready_timer = b_timer_schedule(15, cc_data_ready_callback, stream);
        }
    }
    else if (stream->cc_data_ready_timer) {
        stream->vbi_settings.cc_data_ready_callback = NULL;
        stream->vbi_settings.callback_context = NULL;
        b_timer_cancel(stream->cc_data_ready_timer);
        stream->cc_data_ready_timer = NULL;
    }

    stream->vbi_settings = *settings;
    return 0;
}

bresult bstream_vbi_cc_read(bstream_t stream, bstream_vbi_cc *data, size_t length, size_t *length_read)
{
    NEXUS_VideoInput videoInput = bstream_p_get_video_input(stream);
    NEXUS_VideoInputVbiSettings inputsettings;
    
    *length_read = 0;
    if (!videoInput) return 0;

    NEXUS_VideoInput_GetVbiSettings(videoInput, &inputsettings);
    inputsettings.closedCaptionEnabled = true;
    NEXUS_VideoInput_SetVbiSettings(videoInput, &inputsettings);

    for (;*length_read<length;(*length_read)++) {
        NEXUS_Error rc;
        NEXUS_ClosedCaptionData entry;
        unsigned num;

        rc = NEXUS_VideoInput_ReadClosedCaption(videoInput, &entry, 1, &num);
        if (rc) return BSETTOP_ERROR(rc);
        if (!num) {
            break;
        }
        data[*length_read].data[0] = entry.data[0];
        data[*length_read].data[1] = entry.data[1];
        data[*length_read].field = 0; /* doesn't matter. hard code to top. */
        data[*length_read].cc_type = 0; /* always 608 */
    }
    return 0;
}

bresult bstream_vbi_cc708_read(bstream_t stream, bstream_cc_708 *data, size_t length, size_t *length_read)
{
    /* Nexus does not have a CC708 interface and none is planned.
    Instead, user should read & parse VideoDecoder userdata for CC708. */
    BSTD_UNUSED(stream);
    BSTD_UNUSED(data);
    BSTD_UNUSED(length);
    BSTD_UNUSED(length_read);
    return BSETTOP_ERROR(berr_not_supported);
}

bresult bdisplay_vbi_cc_write(bdisplay_t display, const bstream_vbi_cc *data, size_t length, size_t *length_written)
{
    *length_written = 0;
    for (;*length_written<length;(*length_written)++) {
        NEXUS_Error rc;
        NEXUS_ClosedCaptionData entry;
        unsigned num;

        entry.data[0] = data[*length_written].data[0];
        entry.data[1] = data[*length_written].data[1];
        rc = NEXUS_Display_WriteClosedCaption(display->nDisplay, &entry, 1, &num);
        if (rc) return BSETTOP_ERROR(rc);
        if (!num) {
            break;
        }
    }
    return 0;
}

bresult bstream_vbi_tt_read(bstream_t stream, bstream_vbi_tt *data, size_t length, size_t *length_read)
{
    NEXUS_VideoInput videoInput = bstream_p_get_video_input(stream);

    *length_read = 0;
    if (!videoInput) return 0;

    for (;*length_read<length;(*length_read)++) {
        NEXUS_Error rc;
        NEXUS_TeletextLine entry;
        unsigned num;

        rc = NEXUS_VideoInput_ReadTeletext(videoInput, &entry, 1, &num);
        if (rc) return BSETTOP_ERROR(rc);
        if (!num) {
            break;
        }
        data[*length_read].framingCode = entry.framingCode;
        BKNI_Memcpy(data[*length_read].data, entry.data, NEXUS_TELETEXT_LINESIZE);
    }
    return 0;
}

bresult bdisplay_vbi_tt_write(bdisplay_t display, const bstream_vbi_tt *data, size_t length, uint8_t polarity)
{
    unsigned length_written;
    BSTD_UNUSED(polarity);
    /* See PR 40468. This function should be redesigned to include flow control. See TODO below. */

    length_written = 0;
    for (;length_written<length;(length_written)++) {
        NEXUS_Error rc;
        NEXUS_TeletextLine entry;
        unsigned num;

        entry.lineNumber = length_written; /* generate a line number from the write() call */
        entry.framingCode = data[length_written].framingCode;
        BKNI_Memcpy(entry.data, data[length_written].data, NEXUS_TELETEXT_LINESIZE);

        rc = NEXUS_Display_WriteTeletext(display->nDisplay, &entry, 1, &num);
        if (rc) return BSETTOP_ERROR(rc);

        if (!num) {
            BKNI_Sleep(10); /* TODO: here's the problem. we should not be sleeping. we should exit & report length_written. */
        }
    }
    return 0;
}

bresult bdisplay_vbi_set_cgms(bdisplay_t display, uint32_t cgms_value)
{
    NEXUS_DisplayVbiSettings vbiSettings;
    NEXUS_Error rc;

    display->cgms_enabled = (cgms_value != BDISPLAY_VBI_INVALID_CGMS);

    NEXUS_Display_GetVbiSettings(display->nDisplay, &vbiSettings);
    vbiSettings.cgmsEnabled = display->cgms_enabled;
    rc = NEXUS_Display_SetVbiSettings(display->nDisplay, &vbiSettings);
    if (rc) return BSETTOP_ERROR(rc);

    if (display->cgms_enabled) {
        rc = NEXUS_Display_SetCgms(display->nDisplay, cgms_value);
        if (rc) return BSETTOP_ERROR(rc);
    }

    return 0;
}

bresult bdisplay_vbi_set_wss(bdisplay_t display, uint16_t wss_value)
{
    NEXUS_DisplayVbiSettings vbiSettings;
    NEXUS_Error rc;

    display->wss_enabled = (wss_value != BDISPLAY_VBI_INVALID_WSS);

    NEXUS_Display_GetVbiSettings(display->nDisplay, &vbiSettings);
    vbiSettings.wssEnabled = display->wss_enabled;
    rc = NEXUS_Display_SetVbiSettings(display->nDisplay, &vbiSettings);
    if (rc) return BSETTOP_ERROR(rc);

    if (display->wss_enabled) {
        rc = NEXUS_Display_SetWss(display->nDisplay, wss_value);
        if (rc) return BSETTOP_ERROR(rc);
    }

    return 0;
}

bresult bdisplay_vbi_get_cgms(bdisplay_t display, bool *enabled)
{
    *enabled = display->cgms_enabled;
    return 0;
}

bresult bdisplay_vbi_get_wss(bdisplay_t display, bool *enabled)
{
    *enabled = display->wss_enabled;
    return 0;
}
