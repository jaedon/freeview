/***************************************************************************
 *     Copyright (c) 2002-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: control_hdmi.cpp $
 * $brcm_Revision: 33 $
 * $brcm_Date: 3/26/12 3:18p $
 *
 * Module Description: Central control of Brutus
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/control_hdmi.cpp $
 * 
 * 33   3/26/12 3:18p vle
 * SW7425-2515: Merge to mainline
 * 
 * SW7425-2515/1   3/23/12 6:19p vle
 * SW7425-2515: Update Brutus to use new NEXUS_Cec module. Remove all CEC
 * related code out of hdmi_output module
 * 
 * 32   3/15/12 12:20p erickson
 * SW7425-1747: remove old code
 * 
 * 31   1/21/11 2:12p ahulse
 * SW7405-5096: Only output 3D if attached HDMI monitor supports it
 * 
 * 30   11/16/10 12:17p jtna
 * SW7405-4998: more 3D refactoring
 * 
 * 29   11/15/10 4:21p jtna
 * SW7405-4998: refactor 3DTV code to allow transition in PIG and better
 * content mode handling
 * 
 * 28   11/10/10 5:33p jtna
 * SW7401-4456: update
 * 
 * 27   11/9/10 11:20a jtna
 * SW7401-4456: fix auto mode
 * 
 * 26   11/8/10 5:40p vle
 * SW7405-4990: Add Brutus configuration to control the time out delay of
 * HDCP OSD error messages. The specific time out delay has a big impact
 * on Simplay test results.
 * 
 * 25   10/27/10 3:33p erickson
 * SW7401-4456: fix dtv
 * 
 * 24   10/13/10 7:14p jtna
 * SW7401-4456: 2D/3D switch based on EDID
 * 
 * 23   10/6/10 2:20p calvinho
 * SW7401-4460: Add functions hdmiVideoFormatHas3DSupport and
 * hdmiTVHas3DSupport
 * 
 * 22   9/22/10 5:19p gskerl
 * SW7468-102: Reverting previous change, it was intended to be made on a
 * branch
 * 
 * 21   9/1/10 2:17p gskerl
 * SW7468-102: Disabled CEC Standby message for IBC demo
 * 
 * 20   5/20/10 11:14a mward
 * SW7405-3997: Direct to Nexus calls conditioned with NEXUS_BUILD.  Set
 * HDMI VSI for 3DTV will not be supported without Nexus.
 * 
 * 19   4/26/10 2:46p erickson
 * SW7550-393: merge
 * 
 * SW7550-393/1   4/24/10 2:48p jianweiz
 * SW7550-393:hdmiSetVsi() API should consider the B_N_DVI_OUTPUTS in
 * order to avoid the compile error
 *
 * 18   4/16/10 4:06p erickson
 * SW7405-4089: make brutus resilient to config with zero decoders
 *
 * 17   4/9/10 9:25a erickson
 * SW7405-3997: downgrade WRN to MSG
 *
 * 16   4/9/10 8:17a erickson
 * SW7405-3997: fix DTV
 *
 * 15   4/5/10 11:30a erickson
 * SW7405-3997: set HDMI VSI for 3DTV
 *
 * 14   2/23/10 9:29p vle
 * SW7420-400: Prevent unnecessary HDCP authentication attempt.
 *
 * 13   1/20/10 11:41a vle
 * SW7405-1590: default value of brutus options are all 0's in NEXUS. Need
 * to check and enable Pj checking feature as needed
 *
 * 12   11/2/09 7:07p vle
 * SW7405-3338: Fix CEC compliance issue.
 *
 * 11   8/4/09 1:30p vle
 * PR49558: Add support to control av mute delay before and after format
 * change separately.
 * Fix DVI_USE_PREFERRED_FORMAT to work after hotplug.
 *
 * 10   5/20/09 11:01a vle
 * PR52751: should not call bdisplay_set directly, this will cause a
 * multiple call of bdisplay_set, which will cause HDCP ENC_EN/DIS signal
 * to be toggled.
 *
 * 9   4/16/09 12:28p mward
 * PR53139: Create brutus.cfg HDMI_PASSTHROUGH_ENABLED to initialize HDMI
 * to compressed audio.  Default "no".
 *
 * 8   2/23/09 6:13p vle
 * PR 48785: Add HDMI CEC screen to BRUTUS
 * Merge to main branch
 *
 * 7   11/26/08 6:09p vle
 * PR49583: Merge to main branch.
 *
 * PR49583/1   11/24/08 9:26p vle
 * PR49583: Fix HDCP certification issue.
 *
 * HDMI_TX_Plugfest11_200811/3   11/12/08 3:50p vle
 * PR48785: Non-supported CEC messages should response with a <Feature
 * Abort> message.
 *
 * HDMI_TX_Plugfest11_200811/2   11/10/08 7:12p vle
 * PR48842: Fix HDCP in BRUTUS. Make sure to report the status to Brutus
 * and restart HDCP authentication process on each format change.
 *
 * HDMI_TX_Plugfest11_200811/1   11/7/08 4:55p vle
 * PR48785: Add HDMI CEC screen to BRUTUS demo app
 *
 * 6   8/14/08 12:37p vle
 * PR 44168: enable CEC core to receive messages after initialize CEC
 * callback context
 *
 * 5   7/7/08 5:53p vle
 * PR 44482: Do not force NTSC when setting VESA formats.
 *
 * 4   6/13/08 11:06a erickson
 * PR43651: if hdmi output preferred output is VESA, force NTSC instead.
 *
 * 3   5/20/08 10:56p vle
 * PR 40497: Add HDMI CEC support to Brutus.
 *
 * 2   3/28/08 5:18p vle
 * PR41000: Add Simplay test mode and additional HDCP error state.
 *
 * 1   3/18/08 4:05p erickson
 * PR40307: added thumbnail feature
 *
 ***************************************************************************/
#include "control.h"
#include "mmessagebox.h"

BDBG_MODULE(brutus_control_hdmi);

void Control::hdmi_hotplug_callback(void *context)
{
    Control *control = (Control *)context;
    BDBG_WRN(("HDMI Hotplug Callback!"));
    control->_check_optimal_display = true;
}

void Control::hdmi_disconnect_callback(void *context)
{
    Control *control = (Control *)context;
    BDBG_WRN(("HDMI Disconnect Callback!"));
    control->_check_optimal_display = true;
}

void Control::hdmi_hdcp_callback(void *context)
{
    Control *control = (Control *)context;

    control->hdmiHdcpUpdateStatus() ;
}

void Control::hdmi_cec_message_callback(void *context)
{
    Control *control = (Control *)context;
    BDBG_WRN(("HDMI CEC Message Callback"));
    control->_check_hdmi_cec = true;
}

void Control::initHdmiOutput()
{
    boutput_hdmi_settings s ;
    boutput_hdmi_get(_display_settings.dvi, &s) ;

    if (cfg()->getBool(Config::HDMI_HDCP_PJ_CHECK))
        s.hdcp_pj_checking_enable = true ;

    if (cfg()->getBool(Config::HDMI_HDCP_AUTO_PRE_EMPHASIS))
        s.preemphasis_support = true ;

#if BDBG_DEBUG_BUILD
    if (cfg()->getBool(Config::HDMI_HDCP_ENABLED))
        s.hdcp = true;
#endif
    /* HDMI Simplay test mode*/
    if (cfg()->getBool(Config::HDMI_HDCP_SIMPLAY_MODE_ENABLED))
        s.hdcp_disable_blue_screen = true;

    /* HDMI AVMute/AVUnmute Delay */
    s.hdmi_avmute_delay_pre_format_change = cfg()->getInt(Config::HDMI_AVMUTE_DELAY_PRE_FORMAT_CHANGE);
    s.hdmi_avmute_delay_post_format_change = cfg()->getInt(Config::HDMI_AVMUTE_DELAY_POST_FORMAT_CHANGE);

    /* HDCP Re-authentication delay settings. Default value is 2sec
        Note that this setting only apply if HDMI_HDCP_OSD_DISABLED is set. Otherwise, set this value
        to 0. */
    if (cfg()->getBool(Config::HDMI_HDCP_OSD_DISABLED))
        s.hdcp_failure_retry_delay = cfg()->getInt(Config::HDMI_HDCP_AUTH_RETRY_DELAY);
    else
        s.hdcp_failure_retry_delay = 0;
    /* set HDCP options regardless of HDMI Auto Mode */

    s.hdmi_audio_mode = _cfg->getBool(Config::HDMI_PASSTHROUGH_ENABLED) ?
        boutput_hdmi_audio_mode_compressed : boutput_hdmi_audio_mode_pcm;
    boutput_hdmi_set(_display_settings.dvi, &s);

    s.hdmi_hotplug_callback = hdmi_hotplug_callback;
    s.hdmi_disconnect_callback = hdmi_disconnect_callback;
    s.hdmi_hdcp_status_callback = hdmi_hdcp_callback;
    s.hdmi_cec_message_callback = hdmi_cec_message_callback;
    s.callback_context = (void *) this;


    _hdmiHdcpState = boutput_hdmi_hdcp_state_disabled;
    if (cfg()->getBool(Config::DVI_VALIDATE_EDID_FORMAT))
        s.validate_edid_format = true ;

    boutput_dvi_set(_display_settings.dvi, &s);
    if ( cfg()->getBool(Config::DVI_USE_PREFERRED_FORMAT))
    {
        // generate an initial "event" so that the preferred format is set initially
        setOptimalDisplay();
    }
}

bool Control::isValidHdmiFormat(bvideo_format format)
{
    if ( bconfig->features.video_format_is_supported[format] &&
         _hdmiCapabilities.video_format_is_supported[format] )
    {
        return true;
    }
    return false;
}

void Control::setOptimalDisplay()
{
    getOutput(&_display_settings);

    /* default false. set true if found. */
    _hdmiConnected = false;

    if (_display_settings.dvi) {
        bool validate_format_with_hdmi = cfg()->getBool(Config::DVI_VALIDATE_EDID_FORMAT);

        if (boutput_dvi_get_capabilities(_display_settings.dvi, &_hdmiCapabilities) == berr_not_available) {
            /* if no HDMI, then we can't validate with it. */
            validate_format_with_hdmi = false;
        }
        else {
            _hdmiConnected = true;
            /* _hdmiCapabilities has been populated */

            /* Reset audio format if new device is HDMI and does not support it */
            boutput_dvi_settings ds;
            boutput_dvi_get(_display_settings.dvi, &ds);
            if ( ds.hdmi_audio_mode != boutput_hdmi_audio_mode_pcm &&
                _hdmiCapabilities.hdmi &&
                 !_hdmiCapabilities.audio_mode_is_supported[ds.hdmi_audio_mode] )
            {
                ds.hdmi_audio_mode = _hdmiCapabilities.preferred_audio_mode;
                boutput_dvi_set(_display_settings.dvi, &ds);
            }
        }

        if (!validate_format_with_hdmi) {
            /* There's no HDMI or we don't care, but marked all "allowed" for simpler logic elsewhere. */
            for (int i=0;i<bvideo_format_count;i++)
                _hdmiCapabilities.video_format_is_supported[i] = true;
        }
    }

    /* modify display settings to optimal format for decoded video and HDMI capabilties/preferred format */
    if ( cfg()->getBool(Config::DVI_USE_PREFERRED_FORMAT))
        getOptimalDisplay(&_display_settings);

    setOutput(&_display_settings);

    /* check EDID for 3D support. this allows 2D/3D switch based on hotplug */
    if (brutus()->_control && !cfg()->getBool(Config::FORCE_3D_OUTPUT_FOR_INCOMPATIBLE_TV)) {
        if (brutus()->display()->s3dTv.cfg > Brutus3DTVMode_eOff || brutus()->display()->s3dTv.automode) {
            if (!hdmiTVHas3DSupport()) { /* if TV does not support 3D, then we may have to switch to 2D */
                if (brutus()->display()->s3dTv.output > Brutus3DTVMode_eOff) {
                    brutus()->set3DTVOutput(Brutus3DTVMode_eOff);
                }
            }
            else {
                if (brutus()->display()->s3dTv.output == Brutus3DTVMode_eOff) {
                    /* revert to original output mode */
                    brutus()->set3DTVOutput(brutus()->display()->s3dTv.cfg);
                }
            }
            fireEvent(evChangeChannel); /* this updates the ui */
        }
    }
}

void Control::getOptimalDisplay(bdisplay_settings *display_settings)
{
    bool pal = isPalMode(); /* once we start in PAL or NTSC, never switch. very few TV's would support the switch. */

    if (_hdmiConnected) {
        display_settings->format = _hdmiCapabilities.preferred_video_format;

        /* Make sure format is actually supported by the driver also (i.e. non-1080p or non-HD builds) */
        if ( !bconfig->features.video_format_is_supported[display_settings->format] )
        {
            static const bvideo_format pal_formats[] = {bvideo_format_1080i_50hz, bvideo_format_720p_50hz, bvideo_format_576p, bvideo_format_pal, bvideo_format_vesa};
            static const bvideo_format ntsc_formats[] = {bvideo_format_1080i, bvideo_format_720p, bvideo_format_480p, bvideo_format_ntsc, bvideo_format_vesa};
            const bvideo_format *format_array;
            int num_formats, i;

            if ( pal )
            {
                format_array = pal_formats;
                num_formats = sizeof(pal_formats)/sizeof(bvideo_format);
            }
            else
            {
                format_array = ntsc_formats;
                num_formats = sizeof(ntsc_formats)/sizeof(bvideo_format);
            }

            for ( i = 0; i < num_formats; i++ )
            {
                if ( isValidHdmiFormat(format_array[i]) )
                {
                    display_settings->format = format_array[i];
                    BDBG_WRN(("Monitor's preferred format (%d) is not supported.  Downgrading to %d.",
                              _hdmiCapabilities.preferred_video_format, format_array[i]));
                    break;
                }
            }
        }

        /* Set the vesa_settings structure for vesa mode */
        if (display_settings->format == bvideo_format_vesa) {
            boutput_hdmi_settings ds;
            boutput_dvi_get(display_settings->dvi, &ds);
            display_settings->vesa_settings =ds.vesa_settings;
            return;
        }
    }
    else {
        /* unchanged */
        display_settings->format = _display_settings.format;
    }


    bdecode_status decode_status;

    if (getDecode()) {
        bdecode_get_status(getDecode(), &decode_status);
    }
    else {
        BKNI_Memset(&decode_status, 0, sizeof(decode_status));
    }

    BDBG_MSG(("getOptimalDisplay: params -- height=%u, interlaced=%d, pal=%d", decode_status.source_height, decode_status.interlaced, pal));

    /* any zero or too-small format, make no change. */
    /* Also, if we don't have auto output format enabled, skip the format matching and just make sure the preferred format is valid */
    if (decode_status.source_height < 240 || !cfg()->getBool(Config::AUTO_OUTPUT_FORMAT)) {
        goto error_check;
    }

    if ( decode_status.source_height >= 1080 )
    {
        if ( !pal && isValidHdmiFormat(bvideo_format_1080i) ) {
            display_settings->format = bvideo_format_1080i;
            return;
        }
        if ( pal && isValidHdmiFormat(bvideo_format_1080i_50hz) ) {
            display_settings->format = bvideo_format_1080i_50hz;
            return;
        }
    }
    if ( decode_status.source_height >= 720 )
    {
        if ( !pal && isValidHdmiFormat(bvideo_format_720p) ) {
            display_settings->format =  bvideo_format_720p;
            return;
        }
        if ( pal && isValidHdmiFormat(bvideo_format_720p_50hz) ) {
            display_settings->format = bvideo_format_720p_50hz;
            return;
        }
    }

    if ( decode_status.source_height >= 576 )
    {
        if ( _hdmiConnected && isValidHdmiFormat(bvideo_format_pal) && decode_status.interlaced) {      /* explicitly check if hdmi connected and device supports PAL mode */
            display_settings->format = bvideo_format_pal;
            return;
        }
        if ( _hdmiConnected && isValidHdmiFormat(bvideo_format_576p) && !decode_status.interlaced) {    /* explicitly check if hdmi connected and device supports PAL mode */
            display_settings->format = bvideo_format_576p;
            return;
        }

    }

    /* SD heights are here... */
    if ( !pal && isValidHdmiFormat(bvideo_format_480p) && !decode_status.interlaced) {
        display_settings->format = bvideo_format_480p;
        return;
    }
    if ( pal && isValidHdmiFormat(bvideo_format_576p) && !decode_status.interlaced) {
        display_settings->format = bvideo_format_576p;
        return;
    }

    /* This requres a bit of knowledge about the enum ordering, but is correct for remaining items */
    if ( !pal )
    {
        if ( isValidHdmiFormat(bvideo_format_ntsc) )    {
            display_settings->format = bvideo_format_ntsc;
            return;
        }
        if ( isValidHdmiFormat(bvideo_format_ntsc_japan) ) {
            display_settings->format = bvideo_format_ntsc_japan;
            return;
        }
        if ( isValidHdmiFormat(bvideo_format_pal_m) ) {
            display_settings->format = bvideo_format_pal_m;
            return;
        }
    }
    else
    {
        if ( isValidHdmiFormat(bvideo_format_pal) ) {
            display_settings->format = bvideo_format_pal;
            return;
        }
        if ( isValidHdmiFormat(bvideo_format_pal_n) ) {
            display_settings->format = bvideo_format_pal_n;
            return;
        }
        if ( isValidHdmiFormat(bvideo_format_pal_nc) ) {
            display_settings->format = bvideo_format_pal_nc;
            return;
        }
        if ( isValidHdmiFormat(bvideo_format_secam) ) {
            display_settings->format = bvideo_format_secam;
            return;
        }
    }

error_check:

    /* At the end of all this... we still might not have a match */
    if ( !isValidHdmiFormat(display_settings->format) )
    {
        BDBG_WRN(("Unable to find a suitable format for this monitor.  Leaving current settings intact and disabling HDMI."));
        display_settings->format = _display_settings.format;
        display_settings->dvi = NULL;
    }

    return;
}

bresult Control::hdmiHdcpUpdateStatus()
{
    bresult rc;
    boutput_hdmi_status status;

    rc = boutput_hdmi_get_status(_display_settings.dvi, &status);
    _hdmiHdcpState = status.hdcp_state;

    return rc;
}

void Control::hdmiHdcpStart()
{
    if (_display_settings.dvi)
    {
        boutput_dvi_settings s;
        boutput_dvi_get(_display_settings.dvi, &s);
        s.hdcp = true ;
        boutput_dvi_set(_display_settings.dvi, &s);
    }
}

void Control::handleHdmiHdcp()
{
    static bool dialog = false ;
    static char *HdcpMessage[] =
    {
        (char *)"HDCP Internal Err",
        (char *)"HDCP Disabled",
        (char *)"HDCP Authorizing",
        (char *)"HDCP R0 Failure",
        (char *)"HDCP Ri Failure",
        (char *)"HDCP Pj Failure",
        (char *)"HDCP Repeater Failure",
        (char *)"HDCP Enabled"
    };

    char buf[100]  ;
    int timeout;
    if (dialog)
        return ;

    switch (_hdmiHdcpState)
    {

        case boutput_hdmi_hdcp_state_disabled :
        case boutput_hdmi_hdcp_state_enabled :
        case boutput_hdmi_hdcp_state_init :
            return ;

        case boutput_hdmi_hdcp_state_internal_err :
        case boutput_hdmi_hdcp_state_auth_fail :
        case boutput_hdmi_hdcp_state_ri_fail :
        case boutput_hdmi_hdcp_state_pj_fail :
        case boutput_hdmi_hdcp_state_repeater_fail :
            break ;
    }

    sprintf(buf, "%s", (char *) HdcpMessage[_hdmiHdcpState]) ;

    _hdmiHdcpState = boutput_hdmi_hdcp_state_init  ;

    if (!cfg()->getBool(Config::HDMI_HDCP_OSD_DISABLED)) {
        dialog = true ;
        timeout = cfg()->getInt(Config::HDMI_HDCP_OSD_TIMEOUT_DELAY);
        if (MMessageBox::info(fb(), buf, timeout) == MMessageBox::bOK)
        {
            printf("Start HDCP again\n") ;
            _hdmiHdcpState = boutput_hdmi_hdcp_state_init ;
        }
        else
            printf("HDCP Disabled\n") ;
    }
    else    /* HDCP OSD is disabled. Re-authentication delay should be set appropriately */
    {
        boutput_hdmi_settings settings;
        boutput_hdmi_get(_display_settings.dvi, &settings);
        if (settings.hdcp_failure_retry_delay > 0)
        {
            /* any delay of more than 2s is probably too long */
            if (settings.hdcp_failure_retry_delay > 2000)
                settings.hdcp_failure_retry_delay = 2000;

            BKNI_Sleep(settings.hdcp_failure_retry_delay);
        }
    }

    hdmiHdcpStart() ;
    dialog = false ;

}


void Control::hdmiCecPowerUp(void *context)
{
    BDBG_ENTER(hdmiCecPowerUp);

    Control *control = (Control *)context;
    boutput_hdmi_cec_message_data *received_cec_message_data = control->hdmiCecMessageData();

    /* <ImageViewOn> or <TextViewOn> message receive as broadcast message or sent from unregistered device
    --> ignore*/
    if ((received_cec_message_data->destination_addr == boutput_hdmi_cec_broadcast_address)
    || (received_cec_message_data->initiator_addr == boutput_hdmi_cec_broadcast_address))
        return;

    /* received as directly addressed - bring STB out of standby */
    BDBG_MSG(("CEC <ImageViewOn> or <TextViewOn> received, power on device"));
    if (!control->brutus()->powerUp())
        BDBG_MSG(("Device is already powered on"));

    BDBG_LEAVE(hdmiCecPowerUp);
    return;
}


void Control::hdmiCecPowerDown(void *context)
{
    BDBG_ENTER(hdmiCecPowerDown);

    Control *control = (Control *)context;

    /* <Standby> message, put STB into standby */
    BDBG_MSG(("CEC <Standby> received, putting device into standby"));

    /* send <Inactive Source> */
    hdmiCecInactiveSourceMessage(control);

    if (!control->brutus()->powerDown())
        BDBG_MSG(("Device is already powered down"));

    BDBG_LEAVE(hdmiCecPowerDown);
    return;
}

void Control::hdmiCecImageViewOnMessage(void *context)
{
    bresult src = b_ok;
    boutput_hdmi_cec_message_data cec_message_data;
    bdisplay_settings display_settings;

    BDBG_ENTER(hdmiCecImageViewOnMessage);

    Control *control = (Control *)context;
    control->getOutput(&display_settings);

    BDBG_MSG(("Sending CEC <Image View On> message..."));

    /* Setup parameters for responding message */
    cec_message_data.initiator_addr = control->hdmiCecConfiguration()->logical_address;
    cec_message_data.destination_addr = control->hdmiCecMessageData()->initiator_addr;
    cec_message_data.opcode = 0x04;
    cec_message_data.message_length = 1;

    /* build CEC message buffer - first entry is always the opcode*/
    cec_message_data.message_buffer[0] = cec_message_data.opcode;

    /* send responding CEC message */
    src = boutput_hdmi_cec_send_message(display_settings.dvi, &cec_message_data);

    BDBG_LEAVE(hdmiCecImageViewOnMessage);
    return;
}


void Control::hdmiCecTextViewOnMessage(void *context)
{
    bresult src = b_ok;
    boutput_hdmi_cec_message_data cec_message_data;
    bdisplay_settings display_settings;

    BDBG_ENTER(hdmiCecTextViewOnMessage);

    Control *control = (Control *)context;
    control->getOutput(&display_settings);

    BDBG_MSG(("Sending CEC <Text View On> message..."));

    /* Setup parameters for responding message */
    cec_message_data.initiator_addr = control->hdmiCecConfiguration()->logical_address;
    cec_message_data.destination_addr = control->hdmiCecMessageData()->initiator_addr;
    cec_message_data.opcode = 0x0D;
    cec_message_data.message_length = 1;

    /* build CEC message buffer - first entry is always the opcode*/
    cec_message_data.message_buffer[0] = cec_message_data.opcode;

    /* send responding CEC message */
    src = boutput_hdmi_cec_send_message(display_settings.dvi, &cec_message_data);

    BDBG_LEAVE(hdmiCecTextViewOnMessage);
    return;
}


void Control::hdmiCecActiveSourceMessage(void *context)
{
    bresult src = b_ok;
    boutput_hdmi_cec_message_data cec_message_data;
    bdisplay_settings display_settings;

    BDBG_ENTER(hdmiCecActiveSourceMessage);

    Control *control = (Control *)context;
    boutput_hdmi_cec_message_data *received_cec_message_data = control->hdmiCecMessageData();
    control->getOutput(&display_settings);

    /* <Request Active Source> message receive as non-broadcast message or
    sent from unregistered device --> ignore*/
    if ((received_cec_message_data->destination_addr != boutput_hdmi_cec_broadcast_address)
    || (received_cec_message_data->initiator_addr == boutput_hdmi_cec_broadcast_address))
        return;

    BDBG_MSG(("Sending CEC <Active Source> message..."));

    /* Setup parameters for responding message */
    cec_message_data.initiator_addr = control->hdmiCecConfiguration()->logical_address;
    cec_message_data.destination_addr = boutput_hdmi_cec_broadcast_address;
    cec_message_data.opcode = 0x82;
    cec_message_data.message_length = 3;

    /* build CEC message buffer - first entry is always the opcode*/
    cec_message_data.message_buffer[0] = cec_message_data.opcode;
    cec_message_data.message_buffer[1] = control->hdmiCecConfiguration()->physical_address[0];
    cec_message_data.message_buffer[2] = control->hdmiCecConfiguration()->physical_address[1];

    /* send responding CEC message */
    src = boutput_hdmi_cec_send_message(display_settings.dvi, &cec_message_data);

    BDBG_LEAVE(hdmiCecActiveSourceMessage);
    return;
}


void Control::hdmiCecInactiveSourceMessage(void *context)
{
    bresult src = b_ok;
    boutput_hdmi_cec_message_data cec_message_data;
    bdisplay_settings display_settings;

    BDBG_ENTER(hdmiCecInactiveSourceMessage);

    Control *control = (Control *)context;
    control->getOutput(&display_settings);

    BDBG_MSG(("Sending CEC <Inactive Source> message..."));

    /* Setup parameters for responding message */
    cec_message_data.initiator_addr = control->hdmiCecConfiguration()->logical_address;
    cec_message_data.destination_addr = control->hdmiCecMessageData()->initiator_addr;
    cec_message_data.opcode = 0x9D;
    cec_message_data.message_length = 3;

    /* build CEC message buffer - first entry is always the opcode*/
    cec_message_data.message_buffer[0] = cec_message_data.opcode;
    cec_message_data.message_buffer[1] = control->hdmiCecConfiguration()->physical_address[0];
    cec_message_data.message_buffer[2] = control->hdmiCecConfiguration()->physical_address[1];

    /* send responding CEC message */
    src = boutput_hdmi_cec_send_message(display_settings.dvi, &cec_message_data);

    BDBG_LEAVE(hdmiCecInactiveSourceMessage);
    return;
}

void Control::hdmiCecSetStreamPath(void *context)
{
    bresult src = b_ok;

    BDBG_ENTER(hdmiCecSetStreamPath);

    Control *control = (Control *)context;
    boutput_hdmi_cec_message_data *received_cec_message_data = control->hdmiCecMessageData();

    /* <Set Stream Path> message receive as non-broadcast message or
        sent from unregistered device --> ignore*/
    if ((received_cec_message_data->destination_addr != boutput_hdmi_cec_broadcast_address)
    || (received_cec_message_data->initiator_addr == boutput_hdmi_cec_broadcast_address))
        return;

    BDBG_MSG(("CEC <Set Stream Path> message received, response with <ActiveSource> if matching physical address"));

    /* If device physical address doesn't matches the address in the received message,
        ignore the message. Otherwise, broadcast an <Active Source> message */

    /* First byte in the message buffer is the opcode, ignore */
    if (control->hdmiCecMessageData()->message_buffer[1] != control->hdmiCecConfiguration()->physical_address[0]
        || control->hdmiCecMessageData()->message_buffer[2] != control->hdmiCecConfiguration()->physical_address[1])
    {
        BDBG_MSG(("CEC <SetStreamPath> received, TV request streaming path to another device. Ignore!!!"));
        goto done;
    }

    /* address matches, send <ActiveSource> */
    hdmiCecActiveSourceMessage(control);

done:
    BDBG_LEAVE(hdmiCecSetStreamPath);
    return;
}


void Control::hdmiCecOneTouchPlay(void *context)
{
    bresult src = b_ok;

    BDBG_ENTER(hdmiCecOneTouchPlay);

    Control *control = (Control *)context;

    /* Send <Image View On> Message first */
    hdmiCecImageViewOnMessage(control);

    /* Give the device some time to complete the task */
    BKNI_Sleep(100);

    /* Send <Active Source> Message next */
    hdmiCecActiveSourceMessage(control);


    BDBG_LEAVE(hdmiCecOneTouchPlay);
    return;
}


void Control::hdmiCecStandbyMessage(void *context)
{
    bresult src = b_ok;
    boutput_hdmi_cec_message_data cec_message_data;
    bdisplay_settings display_settings;

    BDBG_ENTER(hdmiCecStandbyMessage);

    Control *control = (Control *)context;
    control->getOutput(&display_settings);

    BDBG_MSG(("Sending CEC <Standby> message..."));

    /* Setup parameters for responding message */
    cec_message_data.initiator_addr = control->hdmiCecConfiguration()->logical_address;
    cec_message_data.destination_addr = boutput_hdmi_cec_broadcast_address;
    cec_message_data.opcode = 0x36;
    cec_message_data.message_length = 1;

    /* build CEC message buffer - first entry is always the opcode*/
    cec_message_data.message_buffer[0] = cec_message_data.opcode;

    /* send responding CEC message */
    src = boutput_hdmi_cec_send_message(display_settings.dvi, &cec_message_data);

    BDBG_LEAVE(hdmiCecStandbyMessage);
    return;
}


void Control::hdmiCecVersionMessage(void *context)
{
    bresult src = b_ok;
    boutput_hdmi_cec_message_data cec_message_data;
    bdisplay_settings display_settings;

    BDBG_ENTER(hdmiCecVersionMessage);

    Control *control = (Control *)context;
    boutput_hdmi_cec_message_data *received_cec_message_data = control->hdmiCecMessageData();
    control->getOutput(&display_settings);

    /* <GetCECVersion> message receive as broadcast message or sent from unregistered device --> ignore*/
    if ((received_cec_message_data->destination_addr == boutput_hdmi_cec_broadcast_address)
    || (received_cec_message_data->initiator_addr == boutput_hdmi_cec_broadcast_address))
        return;

    BDBG_MSG(("Sending CEC <CEC Version> message..."));

    /* Setup parameters for responding message */
    cec_message_data.initiator_addr = control->hdmiCecConfiguration()->logical_address;
    cec_message_data.destination_addr = control->hdmiCecMessageData()->initiator_addr;
    cec_message_data.opcode = 0x9E;
    cec_message_data.message_length = 2;

    /* build CEC message buffer - first entry is always the opcode*/
    cec_message_data.message_buffer[0] = cec_message_data.opcode;
    cec_message_data.message_buffer[1] = 0x02;  /* CEC version 1.2a since our device support HDMI 1.2a */

    /* send responding CEC message */
    src = boutput_hdmi_cec_send_message(display_settings.dvi, &cec_message_data);

    BDBG_LEAVE(hdmiCecVersionMessage);
    return;
}


void Control::hdmiCecGetVersionMessage(void *context)
{
    bresult src = b_ok;
    boutput_hdmi_cec_message_data cec_message_data;
    bdisplay_settings display_settings;

    BDBG_ENTER(hdmiCecGetVersionMessage);

    Control *control = (Control *)context;
    control->getOutput(&display_settings);

    BDBG_MSG(("Sending CEC <Get CEC Version> message...."));

    /* Setup parameters for responding message */
    cec_message_data.initiator_addr = control->hdmiCecConfiguration()->logical_address;
    cec_message_data.destination_addr = control->hdmiCecMessageData()->initiator_addr;
    cec_message_data.opcode = 0x9F;
    cec_message_data.message_length = 1;

    /* build CEC message buffer - first entry is always the opcode*/
    cec_message_data.message_buffer[0] = cec_message_data.opcode;

    /* send responding CEC message */
    src = boutput_hdmi_cec_send_message(display_settings.dvi, &cec_message_data);

    BDBG_LEAVE(hdmiCecGetVersionMessage);
    return;
}


void Control::hdmiCecGivePhysicalAddressMessage(void *context)
{
    bresult src = b_ok;
    boutput_hdmi_cec_message_data cec_message_data;
    bdisplay_settings display_settings;

    BDBG_ENTER(hdmiCecGivePhysicalAddressMessage);

    Control *control = (Control *)context;
    control->getOutput(&display_settings);

    /* <Give Physical Address>, opcode 0x83*/
    BDBG_MSG(("Sending CEC <Give Physical Address> message..."));

    /* Setup parameter for responding message */
    cec_message_data.initiator_addr = control->hdmiCecConfiguration()->logical_address;
    cec_message_data.destination_addr = control->hdmiCecMessageData()->initiator_addr;
    cec_message_data.opcode = 0x83;
    cec_message_data.message_length = 1;

    /* build CEC message buffer - first entry is always the opcode*/
    cec_message_data.message_buffer[0] = cec_message_data.opcode;

    /* send CEC response message */
    src = boutput_hdmi_cec_send_message(display_settings.dvi, &cec_message_data);

    BDBG_LEAVE(hdmiCecGivePhysicalAddressMessage);
    return;
}


void Control::hdmiCecReportPhysicalAddressMessage(void *context)
{
    bresult src = b_ok;
    boutput_hdmi_cec_message_data cec_message_data;
    bdisplay_settings display_settings;

    BDBG_ENTER(hdmiCecReportPhysicalAddressMessage);

    Control *control = (Control *)context;
    boutput_hdmi_cec_message_data *received_cec_message_data = control->hdmiCecMessageData();
    control->getOutput(&display_settings);

    /* <GivePhysicalAddress> message receive as broadcast message or sent from unregistered device --> ignore*/
    if ((received_cec_message_data->destination_addr == boutput_hdmi_cec_broadcast_address)
    || (received_cec_message_data->initiator_addr == boutput_hdmi_cec_broadcast_address))
        return;


    /* <Report Physical Address>, opcode 0x84*/
    BDBG_MSG(("Sending CEC <Report Physical Address> message"));

    /* Setup parameter for responding message */
    cec_message_data.initiator_addr = control->hdmiCecConfiguration()->logical_address;
    cec_message_data.destination_addr = boutput_hdmi_cec_broadcast_address; /* broadcast */
    cec_message_data.opcode = 0x84;
    cec_message_data.message_length = 4;

    /* build CEC message buffer - first entry is always the opcode*/
    cec_message_data.message_buffer[0] = cec_message_data.opcode;
    cec_message_data.message_buffer[1] = control->hdmiCecConfiguration()->physical_address[0];
    cec_message_data.message_buffer[2] = control->hdmiCecConfiguration()->physical_address[1];
    cec_message_data.message_buffer[3] = control->hdmiCecConfiguration()->device_type;

    /* send CEC response message */
    src = boutput_hdmi_cec_send_message(display_settings.dvi, &cec_message_data);

    BDBG_LEAVE(hdmiCecReportPhysicalAddressMessage);
    return;
}


void Control::hdmiCecGetMenuLanguageMessage(void *context)
{
    bresult src = b_ok;
    boutput_hdmi_cec_message_data cec_message_data;
    bdisplay_settings display_settings;

    BDBG_ENTER(hdmiCecGetMenuLanguageMessage);

    Control *control = (Control *)context;
    control->getOutput(&display_settings);

    BDBG_MSG(("Sending CEC <Get Menu Language> message..."));

    /* Setup parameters for responding message */
    cec_message_data.initiator_addr = control->hdmiCecConfiguration()->logical_address;
    cec_message_data.destination_addr = control->hdmiCecMessageData()->initiator_addr;
    cec_message_data.opcode = 0x91;
    cec_message_data.message_length = 1;

    /* build CEC message buffer - first entry is always the opcode*/
    cec_message_data.message_buffer[0] = cec_message_data.opcode;

    /* send responding CEC message */
    src = boutput_hdmi_cec_send_message(display_settings.dvi, &cec_message_data);

    BDBG_LEAVE(hdmiCecGetMenuLanguageMessage);
    return;
}


void Control::hdmiCecSetMenuLanguageMessage(void *context)
{
    bresult src = b_ok;
    boutput_hdmi_cec_message_data cec_message_data;
    bdisplay_settings display_settings;

    BDBG_ENTER(hdmiCecSetMenuLanguageMessage);

    Control *control = (Control *)context;
    boutput_hdmi_cec_message_data *received_cec_message_data = control->hdmiCecMessageData();
    control->getOutput(&display_settings);

    /* <GetMenuLanguage> message receive as broadcast message or
        sent from unregistered device --> ignore*/
    if ((received_cec_message_data->destination_addr == boutput_hdmi_cec_broadcast_address)
    || (received_cec_message_data->initiator_addr == boutput_hdmi_cec_broadcast_address))
        return;

    BDBG_MSG(("Sending CEC <Set Menu Language> message..."));

    /* Setup parameters for responding message */
    cec_message_data.initiator_addr = control->hdmiCecConfiguration()->logical_address;
    cec_message_data.destination_addr = control->hdmiCecMessageData()->initiator_addr;
    cec_message_data.opcode = 0x32;
    cec_message_data.message_length = 4;

    /* build CEC message buffer - first entry is always the opcode*/
    cec_message_data.message_buffer[0] = cec_message_data.opcode;
    cec_message_data.message_buffer[1] = 0x65;  /* e */
    cec_message_data.message_buffer[2] = 0x6E;  /* n */
    cec_message_data.message_buffer[3] = 0x67;  /* g */

    /* send responding CEC message */
    src = boutput_hdmi_cec_send_message(display_settings.dvi, &cec_message_data);

    BDBG_LEAVE(hdmiCecSetMenuLanguageMessage);
    return;
}


void Control::hdmiCecSetOSDNameMessage(void *context)
{
    bresult src = b_ok;
    boutput_hdmi_cec_message_data cec_message_data;
    bdisplay_settings display_settings;

    BDBG_ENTER(hdmiCecSetOSDNameMessage);
    Control *control = (Control *)context;
    boutput_hdmi_cec_message_data *received_cec_message_data = control->hdmiCecMessageData();
    control->getOutput(&display_settings);

    /* Received CEC message from unregistered device or sent from unregistered device --> ignore    */
    if ((received_cec_message_data->initiator_addr == boutput_hdmi_cec_broadcast_address)
    || (received_cec_message_data->destination_addr == boutput_hdmi_cec_broadcast_address))
        return;


    /* <Set OSD Name>, opcode 0x47 */
    BDBG_MSG(("Sending CEC <Set OSD Name> message"));

    /* Setup parameter for responding message */
    cec_message_data.initiator_addr = control->hdmiCecConfiguration()->logical_address;

    /* send to original initiator of <Give OSD Name> message */
    cec_message_data.destination_addr = control->hdmiCecMessageData()->initiator_addr;
    cec_message_data.opcode = 0x47;
    cec_message_data.message_length = 4;

    /* build CEC message buffer - first entry is always the opcode*/
    cec_message_data.message_buffer[0] = cec_message_data.opcode;

    if (control->hdmiCecConfiguration()->device_type == 0x03)       /* STB */
    {
        /* OSD Name = "STB" */
        cec_message_data.message_buffer[1] = 0x53;
        cec_message_data.message_buffer[2] = 0x54;
        cec_message_data.message_buffer[3] = 0x42;
    }
    else if (control->hdmiCecConfiguration()->device_type == 0x04) /* Playback device/DVD */
    {
        /* OSD Name = "DVD" */
        cec_message_data.message_buffer[1] = 0x44;
        cec_message_data.message_buffer[2] = 0x56;
        cec_message_data.message_buffer[3] = 0x44;
    }

    /* send CEC response message */
    src = boutput_hdmi_cec_send_message(display_settings.dvi, &cec_message_data);

    BDBG_LEAVE(hdmiCecSetOSDNameMessage);
    return;
}


void Control::hdmiCecSetOSDStringMessage(void *context)
{
    bresult src = b_ok;
    boutput_hdmi_cec_message_data cec_message_data;
    bdisplay_settings display_settings;

    BDBG_ENTER(hdmiCecSetOSDStringMessage);

    Control *control = (Control *)context;
    control->getOutput(&display_settings);

    /* <Set OSD String>, opcode 0x64 */
    BDBG_MSG(("Sending CEC <Set OSD String> message..."));

    /* Setup parameter for responding message */
    cec_message_data.initiator_addr = control->hdmiCecConfiguration()->logical_address;
    cec_message_data.destination_addr = control->hdmiCecMessageData()->initiator_addr;
    cec_message_data.opcode = 0x64;
    cec_message_data.message_length = 10;

    /* build CEC message buffer - first entry is always the opcode*/
    cec_message_data.message_buffer[0] = cec_message_data.opcode;
    cec_message_data.message_buffer[1] = 0x0;   /* display for the default time of 5sec */
    cec_message_data.message_buffer[2] = 0x42;  /* B */
    cec_message_data.message_buffer[3] = 0x52;  /* R */
    cec_message_data.message_buffer[4] = 0x4F;  /* O */
    cec_message_data.message_buffer[5] = 0x41;  /* A */
    cec_message_data.message_buffer[6] = 0x44;  /* D */
    cec_message_data.message_buffer[7] = 0x43;  /* C */
    cec_message_data.message_buffer[8] = 0x4F;  /* O */
    cec_message_data.message_buffer[9] = 0x4D;  /* M */

    /* send CEC response message */
    src = boutput_hdmi_cec_send_message(display_settings.dvi, &cec_message_data);

    BDBG_LEAVE(hdmiCecSetOSDStringMessage);
    return;
}


void Control::hdmiCecGiveDevicePowerStatusMessage(void *context)
{
    bresult src = b_ok;
    boutput_hdmi_cec_message_data cec_message_data;
    bsettop_power_state state;
    bdisplay_settings display_settings;

    BDBG_ENTER(hdmiCecGiveDevicePowerStatusMessage);

    Control *control = (Control *)context;
    control->getOutput(&display_settings);

    BDBG_MSG(("Sending CEC <Give Device Power Status> message..."));

    /* Setup parameters for responding message */
    cec_message_data.initiator_addr = control->hdmiCecConfiguration()->logical_address;
    cec_message_data.destination_addr = control->hdmiCecMessageData()->initiator_addr;
    cec_message_data.opcode = 0x8F;
    cec_message_data.message_length = 1;

    /* build CEC message buffer - first entry is always the opcode*/
    cec_message_data.message_buffer[0] = cec_message_data.opcode;

    /* send responding CEC message */
    src = boutput_hdmi_cec_send_message(display_settings.dvi, &cec_message_data);

    BDBG_LEAVE(hdmiCecGiveDevicePowerStatusMessage);
    return;
}


void Control::hdmiCecReportPowerStatusMessage(void *context)
{
    bresult src = b_ok;
    boutput_hdmi_cec_message_data cec_message_data;
    bsettop_power_state state;
    bdisplay_settings display_settings;

    BDBG_ENTER(hdmiCecReportPowerStatusMessage);

    Control *control = (Control *)context;
    boutput_hdmi_cec_message_data *received_cec_message_data = control->hdmiCecMessageData();
    control->getOutput(&display_settings);
    bsettop_get_power_state(&state);

    /* <GiveDevicePowerStatus> message receive as broadcast message - ignore*/
    if ((received_cec_message_data->destination_addr == boutput_hdmi_cec_broadcast_address)
    || (received_cec_message_data->initiator_addr == boutput_hdmi_cec_broadcast_address))
        return;

    BDBG_MSG(("Sending CEC <Report Power Status> message..."));

    /* Setup parameters for responding message */
    cec_message_data.initiator_addr = control->hdmiCecConfiguration()->logical_address;
    cec_message_data.destination_addr = control->hdmiCecMessageData()->initiator_addr;
    cec_message_data.opcode = 0x90;
    cec_message_data.message_length = 2;

    /* build CEC message buffer - first entry is always the opcode*/
    cec_message_data.message_buffer[0] = cec_message_data.opcode;

    if (state.av_outputs == true) {
        cec_message_data.message_buffer[1] = 0x00;  /* ON */
    }
    else {
        cec_message_data.message_buffer[1] = 0x01;  /* Standby */
    }

    /* send responding CEC message */
    src = boutput_hdmi_cec_send_message(display_settings.dvi, &cec_message_data);

    BDBG_LEAVE(hdmiCecReportPowerStatusMessage);
    return;
}


void Control::hdmiCecReportAudioStatusMessage(void *context)
{
    bresult src = b_ok;
    boutput_hdmi_cec_message_data cec_message_data;
    bdisplay_settings display_settings;
    baudio_volume volume;
    int level;

    BDBG_ENTER(hdmiCecReportAudioStatusMessage);
    Control *control = (Control *)context;
    boutput_hdmi_cec_message_data *received_cec_message_data = control->hdmiCecMessageData();
    control->getOutput(&display_settings);

    /* Received <Give Audio Status> from unregistered device or incorrectly addressed - ignore */
    if ((received_cec_message_data->initiator_addr == boutput_hdmi_cec_broadcast_address)
    || (received_cec_message_data->destination_addr == boutput_hdmi_cec_broadcast_address))
        return;

    BDBG_MSG(("CEC <GiveAudioStatus> received, response with <ReportAudioStatus>"));
    /* Setup parameters for responding message */
    cec_message_data.initiator_addr = control->hdmiCecConfiguration()->logical_address;
    cec_message_data.destination_addr = control->hdmiCecMessageData()->initiator_addr;
    cec_message_data.opcode = 0x7A;
    cec_message_data.message_length = 2;

    /* build CEC message buffer - first entry is always the opcode*/
    cec_message_data.message_buffer[0] = cec_message_data.opcode;

    /* get audio volume and convert to brutus audio control level */
    bdisplay_get_dac_audio_volume(control->display()->display, &volume);
    level = (volume.left + 100) / 2;    /* scale from 0-100% */

    if (volume.muted) {
        cec_message_data.message_buffer[1] = 0x10;  /* Audio Mute ON */
    }
    else {
        cec_message_data.message_buffer[1] = level << 2;    /* Audio Mute Off, level 0-100% */
    }

    /* send responding CEC message */
    src = boutput_hdmi_cec_send_message(display_settings.dvi, &cec_message_data);

    BDBG_LEAVE(hdmiCecReportAudioStatusMessage);
    return;
}



void Control::hdmiCecDeviceVendorIDMessage(void *context)
{
    bresult src = b_ok;
    boutput_hdmi_cec_message_data cec_message_data;
    bdisplay_settings display_settings;

    BDBG_ENTER(hdmiCecDeviceVendorIDMessage);

    Control *control = (Control *)context;
    boutput_hdmi_cec_message_data *received_cec_message_data = control->hdmiCecMessageData();
    control->getOutput(&display_settings);

    /* <Give Device Vendor ID> message receive as broadcast message  --> ignore*/
    if (received_cec_message_data->destination_addr == boutput_hdmi_cec_broadcast_address)
        return;

    BDBG_MSG(("CEC <GiveDeviceVendorID> received, response with <DeviceVendorID>"));

    /* Setup parameters for responding message */
    cec_message_data.initiator_addr = control->hdmiCecConfiguration()->logical_address;
    cec_message_data.destination_addr = boutput_hdmi_cec_broadcast_address;
    cec_message_data.opcode = 0x87;
    cec_message_data.message_length = 4;

    /* build CEC message buffer - first entry is always the opcode */
    cec_message_data.message_buffer[0] = cec_message_data.opcode;
    cec_message_data.message_buffer[1] = 0x42;  /* B */
    cec_message_data.message_buffer[2] = 0x43;  /* C */
    cec_message_data.message_buffer[3] = 0x4D;  /* M*/

    /* send responding CEC message */
    src = boutput_hdmi_cec_send_message(display_settings.dvi, &cec_message_data);

    BDBG_LEAVE(hdmiCecDeviceVendorIDMessage);
    return;
}


void Control::hdmiCecFeatureAbortMessage(void *context)
{
    bresult src = b_ok;
    boutput_hdmi_cec_message_data cec_message_data;
    bdisplay_settings display_settings;

    BDBG_ENTER(hdmiCecFeatureAbortMessage);

    Control *control = (Control *)context;
    control->getOutput(&display_settings);

    BDBG_WRN(("The requested feature (Opcode %02X) is not supported by this device",
                control->hdmiCecMessageData()->opcode));

    /* Send a <Feature Abort> message */
    cec_message_data.initiator_addr = control->hdmiCecConfiguration()->logical_address;
    cec_message_data.destination_addr = control->hdmiCecMessageData()->initiator_addr;
    cec_message_data.opcode = 0x00;
    cec_message_data.message_length = 3;

    /* build CEC message buffer */
    cec_message_data.message_buffer[0] = cec_message_data.opcode;
    cec_message_data.message_buffer[1] = control->hdmiCecMessageData()->opcode; /* Feature Opcode aborted */
    cec_message_data.message_buffer[2] = 4; /* Abort reason: refused */

    /* send responding CEC message */
    src = boutput_hdmi_cec_send_message(display_settings.dvi, &cec_message_data);

    BDBG_LEAVE(hdmiCecFeatureAbortMessage);
    return;
}


/* CEC message and handler table */
static const Control::HdmiCecHandlerType cecMessageHandlerTable[] =
{
    {0x00, NULL},                                   /* <FeatureAbort> received  */
    {0x04, Control::hdmiCecPowerUp},                /* <ImageViewOn> received */
    {0x05, Control::hdmiCecFeatureAbortMessage},    /* <TunerStepIncrement> received    */
    {0x06, Control::hdmiCecFeatureAbortMessage},    /* <TunerStepDecrement> received    */
    {0x07, Control::hdmiCecFeatureAbortMessage},    /* <TunerDeviceStatus> received */
    {0x08, Control::hdmiCecFeatureAbortMessage},    /* <DiveTunerDeviceStatus> received */
    {0x09, Control::hdmiCecFeatureAbortMessage},    /* <RecordOn> received  */
    {0x0A, Control::hdmiCecFeatureAbortMessage},    /* <RecordStatus> received  */
    {0x0B, Control::hdmiCecFeatureAbortMessage},    /* <RecordOff> received */
    {0x0D, Control::hdmiCecPowerUp},                /* <TextViewOn> received */
    {0x0F, Control::hdmiCecFeatureAbortMessage},    /* <RecordTVScreen> received    */
    {0x1A, Control::hdmiCecFeatureAbortMessage},    /* <GiveDeckStatus> received    */
    {0x1B, Control::hdmiCecFeatureAbortMessage},    /* <DecStatus> received */
    {0x32, NULL},                                   /* <SetMenuLanguage> received   */
    {0x33, Control::hdmiCecFeatureAbortMessage},    /* <ClearAnalogueTimer> received    */
    {0x34, Control::hdmiCecFeatureAbortMessage},    /* <SetAnalogueTimer> received  */
    {0x35, Control::hdmiCecFeatureAbortMessage},    /* <TimerStatus> received   */
    {0x36, Control::hdmiCecPowerDown},              /* <Standby> received*/
    {0x41, Control::hdmiCecFeatureAbortMessage},    /* <Play> received  */
    {0x42, Control::hdmiCecFeatureAbortMessage},    /* <DeckControl> received   */
    {0x43, Control::hdmiCecFeatureAbortMessage},    /* <TimerClearedStatus> received    */
    {0x44, Control::hdmiCecFeatureAbortMessage},    /* <UserControlPressed> received */
    {0x45, Control::hdmiCecFeatureAbortMessage},    /* <UserControlReleased> received */
    {0x46, Control::hdmiCecSetOSDNameMessage},      /* <GiveOSDName> received */
    {0x47, NULL},                                   /* <SetOSDName> received */
    {0x64, NULL},                                   /* <SetOSDString> received */
    {0x67, Control::hdmiCecFeatureAbortMessage},    /* <SetTimerProgramTitle> received  */
    {0x70, Control::hdmiCecFeatureAbortMessage},    /* <SystemAudioModeRequest> received    */
    {0x71, Control::hdmiCecReportAudioStatusMessage},       /* <GiveAudioStatus> received */
    {0x72, Control::hdmiCecFeatureAbortMessage},    /* <SetSystemAudioMode> received    */
    {0x7A, NULL},                                   /* <ReportAudioStatus> received */
    {0x7D, Control::hdmiCecFeatureAbortMessage},    /* <GiveSystemAudioModeStatus> received */
    {0x7E, Control::hdmiCecFeatureAbortMessage},    /* <SystemAudioModeStatus> received */
    {0x80, NULL},                                   /* <RoutingChange> received */
    {0x81, NULL},                                   /* <RoutingInformation> received */
    {0x82, NULL},                                       /* <ActiveSource> received  */
    {0x83, Control::hdmiCecReportPhysicalAddressMessage},   /* <GivePhysicalAddress> received */
    {0x84, NULL},                                           /* <ReportPhysicalAddress> received */
    {0x85, Control::hdmiCecActiveSourceMessage},            /* <RequestActiveSource> received */
    {0x86, Control::hdmiCecSetStreamPath},          /* <SetStreamPath> received */
    {0x87, NULL},                                   /* <DeviceVendorID> received    */
    {0x89, Control::hdmiCecFeatureAbortMessage},    /* <VendorCommand> received */
    {0x8A, Control::hdmiCecFeatureAbortMessage},    /* <VendorRemoteButtonDown> received    */
    {0x8A, Control::hdmiCecFeatureAbortMessage},    /* <VendorRemoteButtonUp> received */
    {0x8C, Control::hdmiCecDeviceVendorIDMessage},          /* <GiveDeviceVendorID> received */
    {0x8D, Control::hdmiCecFeatureAbortMessage},    /* <MenuRequest> received   */
    {0x8E, Control::hdmiCecFeatureAbortMessage},            /* <MenuStatus> received    */
    {0x8F, Control::hdmiCecReportPowerStatusMessage},       /* <GiveDevicePowerStatus> received */
    {0x90, NULL},                                           /* <ReportPowerStatus> received */
    {0x91, Control::hdmiCecSetMenuLanguageMessage},     /* <GetMenuLanguage> received */
    {0x92, Control::hdmiCecFeatureAbortMessage},                /* <SelectAnalogueService> received */
    {0x93, Control::hdmiCecFeatureAbortMessage},    /* <SelectDigitalService> received  */
    {0x97, Control::hdmiCecFeatureAbortMessage},    /* <SetDigitalTimer> received   */
    {0x99, Control::hdmiCecFeatureAbortMessage},    /* <ClearDigitalTimer> received */
    {0x9A, Control::hdmiCecFeatureAbortMessage},    /* <SetAudioRate> received  */
    {0x9D, NULL},                                   /* <InActiveSource> received    */
    {0x9E, NULL},                                       /* <CECVersion> received    */
    {0x9F, Control::hdmiCecVersionMessage},             /* <GetCECVersion> received */
    {0xA0, Control::hdmiCecFeatureAbortMessage},    /* <VendorCommandWithID> received   */
    {0xA1, Control::hdmiCecFeatureAbortMessage},    /* <ClearExternalTimer> received    */
    {0xA2, Control::hdmiCecFeatureAbortMessage},    /* <SetExternalTimer> received  */
    {0xFF, NULL},   /* <Abort> received. This message is reserved for testing purpose. Ignore   */
};


bresult Control::hdmiCecHandler()
{
    bresult src = b_ok;
    boutput_hdmi_cec_message_data cec_message_data;
    uint8_t index;


    /* Retrieve Cec message data */
    src = boutput_hdmi_cec_get_message(_display_settings.dvi, &_hdmiCecMessageData);
    if (src != b_ok)
        goto done;

    if (_hdmiCecMessageData.message_type == boutput_hdmi_cec_message_type_receive)
    {
        /* Retrieve cec related information */
        src = boutput_hdmi_cec_get_configuration(_display_settings.dvi, &_hdmiCecConfiguration);
        if (src != b_ok)
        {
            BDBG_ERR(("Error retrieving cec related information from device"));
            goto done;
        }

        for (index=0; index<sizeof(cecMessageHandlerTable)/sizeof(HdmiCecHandlerType); index++)
        {
            /* found matching CEC message, call appropriate handler function */
            if (_hdmiCecMessageData.opcode == cecMessageHandlerTable[index].opcode)
            {
                if (cecMessageHandlerTable[index].cecMessageHandlerCallback!= NULL)
                {
                    cecMessageHandlerTable[index].cecMessageHandlerCallback((void *) this);
                    break;
                }
            }
        }
    }
    else if (_hdmiCecMessageData.message_type == boutput_hdmi_cec_message_type_transmit)
    {
        /* Check to make sure the process of polling for logical address complete */
        if (!boutput_hdmi_cec_is_device_ready(_display_settings.dvi))
        {
            BDBG_WRN(("CEC device logical address has not been allocated"));
            src = berr_invalid_state;
            goto done;
        }

        /* Logical address is allocated. Build and send CEC messages as needed */
    }
    else
    {
        BDBG_ERR(("Invalid HDMI CEC message type"));
        src = berr_invalid_parameter;
        goto done;
    }


done:

    return src;

}

#ifdef NEXUS_BUILD
#define BSETTOP_NO_API_THUNKS
extern "C" {
    #include "bsettop_impl.h"
}

bresult Control::hdmiSetVsi()
{
#if !B_N_DVI_OUTPUTS
    return b_ok;
#else
    NEXUS_HdmiOutputVendorSpecificInfoFrame vsi;

    NEXUS_HdmiOutputHandle hdmiOutput;
    bresult rc;

    if(_cfg->getBool(Config::FORCE_3D_OUTPUT_FOR_INCOMPATIBLE_TV)==false && !hdmiTVHas3DSupport()) {
        return b_ok;
    }

    if (!_display_settings.dvi) {
        return b_ok;
    }

    hdmiOutput = _display_settings.dvi->handle;
    if (!hdmiOutput) {
        return b_ok;
    }

    NEXUS_HdmiOutput_GetVendorSpecificInfoFrame(hdmiOutput, &vsi);
    switch (display()->s3dTv.output) {
        case Brutus3DTVMode_eOff:
            vsi.hdmiVideoFormat = NEXUS_HdmiVendorSpecificInfoFrame_HDMIVideoFormat_eNone;
            break;
        case Brutus3DTVMode_eSBS:
            vsi.hdmiVideoFormat = NEXUS_HdmiVendorSpecificInfoFrame_HDMIVideoFormat_e3DFormat;
            vsi.hdmi3DStructure = NEXUS_HdmiVendorSpecificInfoFrame_3DStructure_eSidexSideHalf;
            break;
        case Brutus3DTVMode_eOU:
            vsi.hdmiVideoFormat = NEXUS_HdmiVendorSpecificInfoFrame_HDMIVideoFormat_e3DFormat;
            vsi.hdmi3DStructure = NEXUS_HdmiVendorSpecificInfoFrame_3DStructure_eTopAndBottom;
            break;
    }
    BDBG_MSG(("NEXUS_HdmiOutput_SetVendorSpecificInfoFrame %d %d", vsi.hdmiVideoFormat, vsi.hdmi3DStructure));
    rc = (bresult)NEXUS_HdmiOutput_SetVendorSpecificInfoFrame(hdmiOutput, &vsi);
    if (rc) return BSETTOP_ERROR(rc);

    return b_ok;
#endif
}
#else
bresult Control::hdmiSetVsi()
{
    return b_ok;
}
#endif

bool Control::hdmiVideoFormatHas3DSupport(NEXUS_VideoFormat vFmt){
#if NEXUS_NUM_HDMI_OUTPUTS
    NEXUS_HdmiOutputHandle hdmiOutput;
    NEXUS_HdmiOutputStatus status;
    NEXUS_Error rc;

    if(vFmt >= NEXUS_VideoFormat_eMax) {
        BDBG_ERR(("Invalid parameter"));
        return false;
    }

    hdmiOutput = _display_settings.dvi->handle;
    if(!hdmiOutput || !hdmiConnected()) {
        BDBG_MSG(("HDMI output not available"));
        return false;
    }
    
    rc = NEXUS_HdmiOutput_GetStatus(hdmiOutput, &status);
    if(rc) {
        return false;
    }

    if(status.hdmi3DFormatsSupported[vFmt]){
        return true;
    }
#else
    BSTD_UNUSED(vFmt);
#endif
    return false;
}

bool Control::hdmiTVHas3DSupport(){
#if NEXUS_NUM_HDMI_OUTPUTS
    NEXUS_HdmiOutputHandle hdmiOutput;
    NEXUS_HdmiOutputStatus status;
    NEXUS_Error rc;
    int i;

    static bvideo_format_settings vesa_settings;
    NEXUS_VideoFormat nexusDisplayFormat;

    hdmiOutput = _display_settings.dvi->handle;
    if(!hdmiOutput || !hdmiConnected()) {
        BDBG_MSG(("HDMI output not available"));
        return false;
    }

    rc = NEXUS_HdmiOutput_GetStatus(hdmiOutput, &status);
    if(rc) {
        return false;
    }

    if ( _display_settings.format == bvideo_format_vesa) {
         boutput_hdmi_settings ds;
         boutput_dvi_get( _display_settings.dvi, &ds);
         memcpy( &vesa_settings , &(ds.vesa_settings) , sizeof( bvideo_format_settings ) );
    }

    /* NOTE: for 3D to work in a fully automatic mode, make sure AUTO_OUTPUT_FORMAT is selected in the cfg file */
    /*       once selected this code will select 3D output for a given output resolution if the attached device supports it */
    nexusDisplayFormat = b_displayformat2nexus(_display_settings.format, &vesa_settings );
    if ( nexusDisplayFormat < NEXUS_VideoFormat_eMax && status.hdmi3DFormatsSupported[nexusDisplayFormat] ) {
        switch( display()->s3dTv.source ) {
            case Brutus3DTVMode_eSBS :
                if( status.hdmi3DFormatsSupported[nexusDisplayFormat] & NEXUS_HdmiOutput_3DStructure_SideBySideHalfHorizontal ) return true;
             break;
             case Brutus3DTVMode_eOU :
                 if( status.hdmi3DFormatsSupported[nexusDisplayFormat] & NEXUS_HdmiOutput_3DStructure_TopAndBottom ) return true;
             break;
			 /* TODO: Add support for Frame Packing & full res, when platforms support it */
             case Brutus3DTVMode_eOff :
             default:
             break;
        }
    }
    
#endif
    return false;
}
