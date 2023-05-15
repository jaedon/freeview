/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: brutus_config.cpp $
* $brcm_Revision: 109 $
  * $brcm_Date: 8/20/12 4:18p $
  *
* Module Description: Set config file defaults and force
  * certain config settings after the user configuration has been read.
*
* Revision History:
*
*
* $brcm_Log: /BSEAV/app/brutus/src/brutus_config.cpp $
* 
* 109   8/20/12 4:18p chengs
* SW7584-42: Add Brutus support for 7584.
* 
* 108   5/18/12 7:40p bselva
* SW7360-19: Add support for 7360 platform
* 
* 107   12/21/11 3:52p mward
* SW7435-7:  Support Brutus build on (new, 7435-based) platform 97435.
* 
* 106   12/19/11 5:59p katrep
* SW7429-1:enabled pip
* 
* 105   12/1/11 5:31p mward
* SW7429-1:  SW7435-7:  Default HDSD_SINGLE on for 7429, 7435.
* 
* 104   2/15/11 12:06p xhuang
* SW7358-34: Add 7358/7552 support
* 
* 103   1/13/11 10:57a katrep
* SW7231-3:enable PIP by default for 7231 family
* 
* 102   1/4/11 4:45p jrubio
* SW7344-20: add hd pip
* 
* 101   12/28/10 3:15p hongtaoz
* SW7425-49: add 7425 support;
* 
* 100   12/15/10 10:38a jrubio
* SW7344-9: add 7344/7346 support
* 
* 99   11/8/10 5:40p vle
* SW7405-4990: Add Brutus configuration to control the time out delay of
* HDCP OSD error messages. The specific time out delay has a big impact
* on Simplay test results.
* 
* 98   10/21/10 4:54p nickh
* SW7422-80: Add 7422 support for hdsd_single, dual output, and triple
* display
* 
* 97   10/6/10 2:12p calvinho
* SW7401-4460: Default FORCE_3D_OUTPUT_FOR_INCOMPATIBLE_TV on
* 
* 96   3/8/10 5:33p jtna
* SW7420-490: do not override user-specified OUTPUT_FORMAT with hdmi's
* preferred format
* 
* 95   3/2/10 2:37p erickson
* SW7550-272: MERGE
* 
* SW7550-272/1   3/2/10 5:45p nitinb
* SW7550-272: included brutus_app.h to bring into effect macro
* B_DEFAULT_TO_ARGB8888_PIXEL
* 
* 94   2/10/10 2:01p rjain
* SW7550-198: Default HDSD_SINDLE to 1 for 7550
* 
* 93   1/22/10 2:57p erickson
* SW7550-210: refactor BCHP_CHIP lists
* 
* 92   12/26/09 2:31p gmohile
* SW7408-1 : Add 7408 support
* 
* 91   12/17/09 5:40p randyjew
* SW7468-6:Add 7468 Support for HDSD_SINGLE
* 
* 90   12/1/09 4:38p mward
* SW7125-129: Default DQT on.
* 
* SW7125-129/1   11/25/09 4:55p mward
* SW7125-129: Default DQT on.
* 
* 89   11/11/09 6:36p mward
* SW7400-2606: Sharpness should default to minimum.
* 
* 88   10/8/09 9:51a jrubio
* SW7342-45: add HDSD Simul mode for 734X
* 
* 87   10/1/09 5:26p mward
* SW7125-4: Default HDSD_SINGLE on for 7125.
* 
* 86   9/24/09 11:55a erickson
* SW7405-3069: add passive standby options for USB, SATA and ETHERNET.
* default to minimum power.
*
* 85   9/11/09 11:40a jjordan
* SW7405-3017: add option to force compressed audio on HDMI
* 
* 84   8/25/09 12:48p jtna
* SWDEPRECATED-3910: video primer fast channel change feature
* 
* 83   8/20/09 11:18a jrubio
* PR55232: turn off Dual Compositors on 7340/7342
* 
* 82   8/19/09 10:55a jrubio
* PR55232: add 7342/7340 support
* 
* 81   8/14/09 3:51p erickson
* PR55994: set THUMBNAIL_MODE default for PIXEL_FORMAT
*
* 80   8/4/09 1:26p vle
* PR49558: Add support to control av mute delay before and after format
* change separately.
*
* 79   7/9/09 11:51a jtna
* PR56522: Remove multi-decode fast channel change feature. To be
* replaced with video primer FCC.
*
* 78   4/16/09 12:31p mward
* PR53139: Create brutus.cfg HDMI_PASSTHROUGH_ENABLED to initialize HDMI
* to compressed audio.  Default "no".
*
* 77   4/1/09 3:07p nickh
* PR53588: Enable 7420 PIP support
*
* 76   2/4/09 10:23a jrubio
* PR51629: add 7336 support
*
* 75   10/31/08 2:56p jjordan
* PR47230: Default TTS auto-detect to off
*
* 74   10/31/08 12:29p jjordan
* PR47230: Add TTS Pacing
*
* PR47230/1   10/28/08 10:37a jjordan
* PR47230: Port TTS Pacing to Nexus
*
* 73   8/20/08 5:48p lwhite
* PR42739: replaced B_HAS_NETACCEL with B_HAS_IP
*
* 72   8/13/08 10:11a erickson
* PR42789: don't default BT_REMOTE_ADDRESS to one BD remote address
*
* 71   8/13/08 5:26a erickson
* PR42789: updates for BLUETOOTH_SUPPORT
*
* 70   7/18/08 4:21p erickson
* PR44919: clean up, use bconfig
*
* 69   5/27/08 4:43p jtna
* PR42200: Multi-decode fast channel change feature
*
* 68   5/8/08 9:46a jrubio
* PR42353: disable PIP for 7335^
*
* 67   4/2/08 2:20p rjlewis
* PR38655: If no disk support, remove record/playback from menu.
*
* 66   3/28/08 5:17p vle
* PR41000: Add Simplay test mode and additional HDCP error state.
*
* 65   3/20/08 11:55a erickson
* PR40307: cleanup old code
*
* 64   2/27/08 4:56p jrubio
* PR38665: set HDSD_SINGLE on for 7325, disable PIP
*
* 63   2/13/08 4:52p tokushig
* PR39534: added bluetooth remote capabilities to brutus
*
* 62   12/7/07 2:01p erickson
* PR37590: add new config options to control PVR action and BOF/EOF w/
* and w/o record
*
* 61   12/6/07 2:08p erickson
* PR37038: add CONTINUOUS_RECORD_DURATION, remove unused fixedSize and
* CIRCFILE.
*
* 60   12/4/07 5:20p katrep
* PR37217: Added support for 7335.
*
* 59   11/28/07 3:37p vle
* PR 36971: Add Brutus configuration delay after AVMute and before
* AVUnmute.
*
 ************************************************************************/
#include "brutus_config.h"
#include "brutus_app.h"
#include <bsettop.h>

void setDefaults(Config *cfg)
{
    cfg->set(Config::VBI_ENABLED, true);
    cfg->set(Config::PROMPT_ENABLED, true);
    cfg->set(Config::RFMOD_CHANNEL, 3);
    cfg->set(Config::VIDEO_PRIMER_FASTCHANNELCHANGE_ENABLED, false);    
#if BCHP_CHIP == 7400 || BCHP_CHIP == 7405 || BCHP_CHIP == 7038 || BCHP_CHIP == 7420 || BCHP_CHIP == 7422 || BCHP_CHIP == 7425 || \
    BCHP_CHIP == 7346 || BCHP_CHIP == 7344 || BCHP_CHIP == 7231 || BCHP_CHIP == 7429 || BCHP_CHIP == 7435 || BCHP_CHIP == 7584
    cfg->set(Config::PIP_ENABLED, 1);
#endif
#ifdef PVR_SUPPORT
#if defined(__vxworks) && !defined(SATA_DISK_SUPPORT) && !defined(USB_DISK_SUPPORT)
    /* We need PVR_SUPPORT to get IP-STB, but without SATA or USB disk we shouldn't enable these */
    cfg->set(Config::PLAYBACK_ENABLED, false);
    cfg->set(Config::RECORD_ENABLED, false);
#else
    cfg->set(Config::PLAYBACK_ENABLED, true);
    cfg->set(Config::RECORD_ENABLED, true);
#endif
    cfg->set(Config::RECORD_TIME_WINDOW, 60);
    cfg->set(Config::CONTINUOUS_RECORD_DURATION, 60 * 5); /* 5 minutes */
    cfg->set(Config::LOOP_PLAYBACK, true);
    cfg->set(Config::PVR_TIMELINE_ENABLED, 1);
#endif
    // MP3_ENABLED and PICTURES_ENABLED must be explicitly turned on
    cfg->set(Config::REMOTE_ENABLED, true);
#if BCHP_CHIP == 7401 || BCHP_CHIP == 7403 || BCHP_CHIP == 7400 || BCHP_CHIP == 7405 || BCHP_CHIP == 7335 || BCHP_CHIP == 7325 || \
    BCHP_CHIP == 7336 || BCHP_CHIP == 7420 || BCHP_CHIP == 7125 || BCHP_CHIP == 7342 || BCHP_CHIP == 7340 || BCHP_CHIP == 7468 || \
    BCHP_CHIP == 7408 || BCHP_CHIP == 7550 || BCHP_CHIP == 7422 || BCHP_CHIP == 7425 || BCHP_CHIP == 7231 || BCHP_CHIP == 7346 || \
    BCHP_CHIP == 7344 || BCHP_CHIP == 7358 || BCHP_CHIP == 7552 || BCHP_CHIP == 7429 || BCHP_CHIP == 7435 || BCHP_CHIP == 7360 || \
    BCHP_CHIP == 7584
    /* default to dual compositor */
    cfg->set(Config::HDSD_SINGLE, true);
#endif
    cfg->set(Config::LIVEDECODE_ENABLED, true);

    cfg->set(Config::NO_SIGNAL_BACKGROUND_COLOR,(int)0xff000000);

    // this also requires the PICTURES_SUPPORT compile-time flag.
    // if this is set, default this to enabled
    cfg->set(Config::PICTURES_ENABLED, true);
    cfg->set(Config::DVI_VALIDATE_EDID_FORMAT, true);

    cfg->set(Config::HDMI_HDCP_PJ_CHECK, true)  ;
    cfg->set(Config::HDMI_HDCP_AUTH_RETRY_DELAY, 2000) ;
    cfg->set(Config::HDMI_HDCP_AUTO_PRE_EMPHASIS, false) ;
    cfg->set(Config::HDMI_HDCP_OSD_DISABLED, false);
    cfg->set(Config::HDMI_HDCP_OSD_TIMEOUT_DELAY, 5000);
    cfg->set(Config::HDMI_HDCP_SIMPLAY_MODE_ENABLED, false);
    cfg->set(Config::HDMI_AVMUTE_DELAY_PRE_FORMAT_CHANGE, 100);
    cfg->set(Config::HDMI_AVMUTE_DELAY_POST_FORMAT_CHANGE, 100);
#if BDBG_DEBUG_BUILD
    cfg->set(Config::HDMI_HDCP_ENABLED, false);
#endif
    cfg->set(Config::HDMI_FORCE_COMPRESSED_MENU_ITEM, false);
    cfg->set(Config::FORCE_3D_OUTPUT_FOR_INCOMPATIBLE_TV, true);


#if BLUETOOTH_SUPPORT
    cfg->set(Config::BT_ENABLED, true);
#endif

    /* set up default paths and filenames */
    cfg->set(Config::AUDIO_PATH, "audio");
    cfg->set(Config::PICTURES_PATH, "pictures");
    cfg->set("VIDEODIR0", "videos");
    cfg->set(Config::SCRIPTS_PATH, "scripts");
    cfg->set(Config::IMAGES_PATH, "images");
    cfg->set(Config::SKINS_PATH, "skins");
    cfg->set(Config::FONTS_PATH, "fonts");

    cfg->set(Config::HOME_LOGO, "bcmlogo.png");
    cfg->set(Config::CHANNEL_MAP, "channels.txt");
    cfg->set(Config::PROGRAM_GUIDE, "programguide.txt");
    cfg->set(Config::NORMAL_FONT, "fonts/arial_18_aa.bwin_font;-1");
    cfg->set(Config::MAINMENU_TITLE_FONT, "fonts/arial_italic_35_aa.bwin_font;-1");
    cfg->set(Config::SCREEN_TITLE_FONT, "fonts/arial_italic_35_aa.bwin_font;-1");
    cfg->set(Config::TVSCREEN_MESSAGE_FONT, "fonts/arial_black_24_aa.bwin_font;-1");
    cfg->set(Config::TVSCREEN_MESSAGE_FONT_DROPSHADOW, false);
    cfg->set(Config::TINY_FONT, "fonts/arial_11.bwin_font;-1");

#ifdef ENCODER_SUPPORT
    cfg->set(Config::ENCODER_ENABLED, true);
#endif
    cfg->set(Config::APP_DEBUG_LEVEL, "wrn");
    cfg->set(Config::SPDIF_PASSTHROUGH_ENABLED, true);
    cfg->set(Config::HDMI_PASSTHROUGH_ENABLED, false);

#if B_DEFAULT_TO_ARGB8888_PIXEL
    cfg->set(Config::PIXEL_FORMAT, "argb8888");
#else
    cfg->set(Config::PIXEL_FORMAT, "argb1555");
#endif
    cfg->set(Config::CHROMAKEY_COLOR,(int)0x00000000);
    cfg->set(Config::BACKGROUND_COLOR,(int)0xff373755);
    cfg->set(Config::PVRBAR_OUTLINE_COLOR,(int)0xff373755);
    cfg->set(Config::PVRBAR_PLAYBACK_COLOR,(int)0xff117711);
    cfg->set(Config::PVRBAR_RECORDING_COLOR,(int)0xff773333);
    cfg->set(Config::PVRBAR_CURSOR_COLOR,(int)0xff111111);
    cfg->set(Config::PVRBAR_CURSOR_WIDTH,4);
    cfg->set(Config::TVBEVEL_WIDTH,10);
#if defined B_HAS_DRM_ND || defined B_HAS_IP
     cfg->set(Config::SKIN_FILE, "skin_drmnd.xml");
#else
    cfg->set(Config::SKIN_FILE, "skin.xml");
#endif
    cfg->set(Config::SCREEN_X,38);
    cfg->set(Config::SCREEN_Y,25);

    cfg->set(Config::AUDIODECODE_ENABLED,true);
    cfg->set(Config::VIDEODECODE_ENABLED,true);
    cfg->set(Config::SCALED_VIDEO_ENABLED,true);
    cfg->set(Config::DEFERRED_CHANNELCHANGED_ENABLED,true);
    cfg->set(Config::TRICKMODES_ENABLED,true);
    cfg->set(Config::DQT_ENABLED,true);
    cfg->set(Config::DQT_PICS_PER_GOP,10);
    cfg->set(Config::IP_USE_LIVE_PLAYBACK, true);
#ifdef LIVEMEDIA_SUPPORT
    cfg->set(Config::SAP_TIMEOUT_INTERVAL, 3600);   //default timeout is 1 hour
#endif

    cfg->set(Config::TTS_AUTO_DETECT, false);
    cfg->set(Config::TTS_PACING_MAX_ERROR, 2636);
    cfg->set(Config::TTS_MIN_BUF_DEPTH, 125000);
    cfg->set(Config::TTS_INIT_BUF_DEPTH, 625000);
    cfg->set(Config::TTS_MAX_BUF_DEPTH, 1250000);
    cfg->set(Config::TTS_MAX_CLOCK_MISMATCH, 60);

    cfg->set(Config::VIDEO_CONTRAST, 0);
    cfg->set(Config::VIDEO_SATURATION, 0);
    cfg->set(Config::VIDEO_HUE, 0);
    cfg->set(Config::VIDEO_BRIGHTNESS, 0);
    cfg->set(Config::VIDEO_SHARPNESS, -32768);
    cfg->set(Config::VIDEO_BNR, 0);
    cfg->set(Config::VIDEO_MNR, 0);
    cfg->set(Config::VIDEO_DCR, 0);
    cfg->set(Config::VIDEO_DYNAMIC_CONTRAST, 0);
    cfg->set(Config::VIDEO_FLESH_TONE, 0);
    cfg->set(Config::VIDEO_GREEN_STRETCH, 0);
    cfg->set(Config::VIDEO_BLUE_STRETCH, 0);

    cfg->set(Config::POWERSTANDBY_USB, true);
    cfg->set(Config::POWERSTANDBY_SATA, true);
    cfg->set(Config::POWERSTANDBY_ETHERNET, true);
}

int conformConfig(Config *cfg)
{
    if (!cfg->get(Config::PLAYBACK_AT_EOF)) {
        cfg->set(Config::PLAYBACK_AT_EOF, "loop");
    }
    if (!cfg->get(Config::PLAYBACK_AT_BOF)) {
        cfg->set(Config::PLAYBACK_AT_BOF, "loop");
    }
    if (!cfg->get(Config::PLAYBACK_AT_EOF_WITH_RECORD)) {
        cfg->set(Config::PLAYBACK_AT_EOF_WITH_RECORD, "play");
    }
    if (!cfg->get(Config::PLAYBACK_AT_BOF_WITH_RECORD)) {
        cfg->set(Config::PLAYBACK_AT_BOF_WITH_RECORD, "play");
    }

#ifdef DUAL_CMP_SUPPORT
    if (cfg->getBool(Config::DUAL_OUTPUT)) {
        /* This means we're going to drive two UI's from the same application */
        cfg->set(Config::PIP_ENABLED, false);

        #if B_TRIPLE_VEC_SYSTEM || B_TRIPLE_DISPLAY_DUAL_VEC_SYSTEM
        /* Force HDSD_SINGLE for 7400 */
        cfg->set(Config::HDSD_SINGLE, true);
        #endif
    }
    else if (cfg->getBool(Config::SECOND_DISPLAY)) {
#if B_TRIPLE_VEC_SYSTEM || B_TRIPLE_DISPLAY_DUAL_VEC_SYSTEM
        if (!cfg->get(Config::ECHO_REMOTE)) {
            cfg->set(Config::ECHO_REMOTE, 1);
        }
#endif
        // -2 defaults to SA_REMOTE, but there are times to override this
        if (!cfg->get(Config::SA_REMOTE))
            cfg->set(Config::SA_REMOTE, 1);

        cfg->set(Config::PIP_ENABLED, 0);
    }

    if (cfg->getBool(Config::SIDEBYSIDE_ENABLED)) {
        cfg->set(Config::PVR_TIMELINE_ENABLED, 0);
    }
#endif

    if (bconfig->resources.decode.total < 2) {
        cfg->set(Config::PIP_ENABLED, 0);
        cfg->set(Config::SECOND_DISPLAY, 0);
    }

    if (bconfig->resources.display.total < 2) {
        cfg->set(Config::HDSD_SINGLE, 0);
    }

    /* if an OUTPUT_FORMAT has been specified, do not override it with hdmi's preferred format */
    if (!cfg->get(Config::OUTPUT_FORMAT)) {
        cfg->set(Config::DVI_USE_PREFERRED_FORMAT, true);
    }

    /* set cont_count_ignore in the environment if requested */
    if (cfg->getBool(Config::CONTINUITY_COUNTER_DISABLED)) {
#ifdef LINUX
        setenv("cont_count_ignore","1", 1);
#else
        /* FIXME: need VxWorks support */
#endif
    }

    if ( !bconfig->features.sharpness )
        cfg->set(Config::VIDEO_SHARPNESS, 0);
    if ( !bconfig->features.dnr_bnr )
        cfg->set(Config::VIDEO_BNR, 0);
    if ( !bconfig->features.dnr_mnr )
        cfg->set(Config::VIDEO_MNR, 0);
    if ( !bconfig->features.dnr_dcr )
        cfg->set(Config::VIDEO_DCR, 0);
    if ( !bconfig->features.lab )
        cfg->set(Config::VIDEO_DYNAMIC_CONTRAST, 0);
    if ( !bconfig->features.cab )
    {
        cfg->set(Config::VIDEO_FLESH_TONE, 0);
        cfg->set(Config::VIDEO_GREEN_STRETCH, 0);
        cfg->set(Config::VIDEO_BLUE_STRETCH, 0);
    }

    /* This has to be done here instead of in setDefaults because
    we have to wait for bconfig to be initialized by bsettop_init.
    */
    if (!cfg->get(Config::HOME_TITLE)) {
        char buf[256];
        BKNI_Snprintf(buf, 256, "BCM%s", bconfig->board);
        cfg->set(Config::HOME_TITLE, buf);
    }
    if (!cfg->get(Config::NEW_VIDEO_DESCRIPTION)) {
        char buf[256];
        BKNI_Snprintf(buf, 256, "Recorded on the %s.", cfg->get(Config::HOME_TITLE));
        cfg->set(Config::NEW_VIDEO_DESCRIPTION, buf);
    }

    if (cfg->getBool(Config::THUMBNAIL_MODE)) {
        cfg->set(Config::PIXEL_FORMAT, "ARGB8888");
    }

    /**
    * Read in the OUTPUT_SETTINGS_FILE, if it is specified.
    * This file is created by outputsscreen.cpp. The reason it's
    * a separate file is that I don't want the app to rewrite the
    * main config file.
    **/
    const char *file;
    file = cfg->get(Config::OUTPUT_SETTINGS_FILE);
    if (file)
        cfg->read(file);

    /**
    * Read in the CHANNEL_MAP_SELECT_FILE, if it is specified.
    * This file is created by editchannelsscreen.cpp. The reason it's
    * a separate file is that I don't want the app to rewrite the
    * main config file.
    **/
    file = cfg->get(Config::CHANNEL_MAP_SELECT_FILE);
    if (file)
        cfg->read(file);

    return 0;
}

