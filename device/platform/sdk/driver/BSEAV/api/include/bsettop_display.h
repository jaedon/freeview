/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsettop_display.h $
 * $brcm_Revision: 43 $
 * $brcm_Date: 3/26/12 3:12p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/include/bsettop_display.h $
 * 
 * 43   3/26/12 3:12p vsilyaev
 * SW7425-2515: Update Brutus to use new NEXUS_Cec module. Remove all CEC
 * related code out of hdmi_output module
 * 
 * SW7425-2515/1   3/23/12 6:19p vle
 * SW7425-2515: Update Brutus to use new NEXUS_Cec module. Remove all CEC
 * related code out of hdmi_output module
 * 
 * 42   8/4/09 12:41p vsilyaev
 * PR49558: Add support to control av mute delay before and after format
 * change separately.
 * 
 * PR49558/1   8/4/09 12:25p vle
 * PR49558: Add support to control av mute delay before and after format
 * change separately.
 * 
 * 41   5/11/09 3:35p vsilyaev
 * PR53373: Add HDMI 1.3a support, deep color, to Brutus/nexus
 * 
 * PR53373_DeepColor/1   4/16/09 3:56p vle
 * PR53373: Add HDMI 1.3a support, deep color, to Brutus/nexus
 * 
 * 40   2/20/09 1:39p jgarrett
 * PR 52355: Adding ANR
 *
 * 39   9/17/08 4:16p jgarrett
 * PR 47061: Adding audio transcode options
 *
 * 38   8/13/08 4:28p vsilyaev
 * PR 44168: Add new API allowing application to put enable CEC core to
 * receive messages.
 *
 * PR44168/1   8/13/08 3:47p vle
 * PR 44168: Add new API allowing application to put enable CEC core to
 * receive messages.
 *
 * 37   4/3/08 11:34a vsilyaev
 * PR32279:  Rework splitscreen demo mode
 *
 * PR32279/2   4/2/08 4:33p jtna
 * PR32279: Rework splitscreen demo mode
 *
 * PR32279/1   4/2/08 11:42a jtna
 * PR32279: Rework splitscreen demo mode
 *
 * 36   3/28/08 2:47p vsilyaev
 * PR41000: PR41000: Add hdcp_disable_blue_screen variable
 * PR41000: Add Simplay test mode and additional HDCP error state.
 *
 * PR41000/2   3/28/08 11:23a vle
 * PR41000: Add hdcp_disable_blue_screen variable
 *
 * PR41000/1   3/27/08 6:18p vle
 * PR41000: Add Simplay test mode and additional HDCP error state.
 *
 * 35   3/14/08 10:16a jgarrett
 * PR 37781: Merging to main branch
 *
 * PR37781/3   3/13/08 9:51p vle
 * PR 37781: Update naming convention on function prototype.
 * Pass parameter as pointer, not by reference
 *
 * PR37781/2   3/12/08 3:23p vle
 * PR 37781: Update naming conventions
 *
 * PR37781/1   3/11/08 6:44p vle
 * PR 37781: Add framework for CEC support
 *
 * 34   2/28/08 2:08p vsilyaev
 * PR 34858: 30616, 37945: Fixed typo
 *
 * 33   2/21/08 1:37p vsilyaev
 * PR 34858: 30616, 37945: Added boutput_spdif_channel_status
 *
 * 32   12/4/07 5:39p vsilyaev
 * PR 33367: Added audio delay settings
 *
 * 31   11/28/07 4:38p vsilyaev
 * PR 36971: Add Brutus configuration delay after AVMute and before
 * AVUnmute.
 *
 * PR36971/1   11/28/07 3:37p vle
 * PR 36971: Add Brutus configuration delay after AVMute and before
 * AVUnmute.
 *
 * 30   10/29/07 10:46a vsilyaev
 * PR 36308: Added copyright and catagory code
 *
 * 29   9/14/07 3:30p jgarrett
 * PR30689: PR30689: Add second CVBS output support in one display on
 * 7400.
 *
 * PR30689/1   9/13/07 6:41p mward
 * PR30689: Add second CVBS output support in one display on 7400.
 *
 * 28   8/22/07 5:29p jgarrett
 * PR 32044: Merging to main branch
 *
 * 7405_pip_support/1   8/21/07 2:38p katrep
 * PR34204: Added mosaic mode support for 7405.
 *
 * PR32044/1   6/26/07 5:54p jgarrett
 * PR 32044: Adding Mosaic Mode APIs
 *
 * 27   6/20/07 5:30p jgarrett
 * PR 26474: Adding boutput_hdmi_status and hdcp_status_callback
 *
 * 26   6/14/07 10:11a jgarrett
 * PR 32123: Adding boutput_hdmi_xxx
 *
 * PR32123/1   6/13/07 4:30p vle
 * PR 32123: Rename HDMI API's name
 *
 * 25   5/16/07 3:13p vsilyaev
 * PR 30807: Fixed typo
 *
 * 24   5/16/07 3:10p vsilyaev
 * PR30807:  Need an API to read HDMI device tree physical address
 *
 * PR30807/1   5/16/07 1:48p dlwin
 * PR 30807: Need an API to read HDMI device tree physical address
 *
 * 23   5/3/07 5:24p vsilyaev
 * PR 24374: Fixed description of bdecode_window_open and
 * bdecode_window_clone functions
 *
 * 22   4/26/07 5:48p vsilyaev
 * PR 29929: Added background color for video compositor
 *
 * PR26747_Plugfest8/1   4/16/07 8:28p vle
 * PR 26747: Added feature to report HDCP info/failures using on screen
 * graphics
 *
 * 21   4/11/07 4:28p vsilyaev
 * PR 29145: Added DCS type
 *
 * 20   2/23/07 3:41p jgarrett
 * PR 28036: Adding LAB/CAB adjustments
 *
 * 19   2/8/07 4:44p jgarrett
 * PR 27131: Fixing comment text
 *
 * 18   1/23/07 4:20p vsilyaev
 * PR25913: Added country code for the RF configuration
 *
 * 17   12/14/06 6:31p bandrews
 * PR24597: Deprecated refresh_rate; added frame_rate
 *
 * 16   11/30/06 7:20p jgarrett
 * PR 24698: Revising DNR control to allow separate configs for
 * MNR,BNR,DCR.
 *
 * 15   10/20/06 2:05p erickson
 * PR24374: clarify bdecode_window_settings.cliprect docs.
 *
 * 14   6/19/06 5:07p rgreen
 * PR20869, PR20866:
 * Add Settop API option for HDMI Pre-Emphasis  ; disabled by default
 * Add Settop API option for HDMI HDCP Pj Checking ; enabled by default
 * Add Settop API option to specify retry delay between HDCP
 * Authentications attempts
 * Use HDMI vs DVI in comments
 *
 * Use HDMI vs DVI in comments
 *
 * 13   5/23/06 4:50p erickson
 * PR21004: boutput_rf_set_audio_volume should only set mute. audio volume
 * is set via the DAC's which feed into RFM.
 *
 * 12   5/10/06 12:03p jgarrett
 * PR 17747: Allowing app to set HDMI audio mode
 *
 * 11   5/4/06 8:08p bandrews
 * PR21233: Added copyright and category code to spdif settings
 *
 * 10   5/2/06 2:45p jgarrett
 * PR 21291: Adding boutput_dvi_get_capabilities()
 *
 * 9   4/4/06 9:21p erickson
 * PR17108: rename to DNR
 *
 * 8   12/8/05 3:55p jrubio
 * PR18442: Added Digital Contour Removal structure
 *
 * 7   9/14/05 6:32p rgreen
 * PR16538,PR12828: Prevent failure of bdisplay_set when display format
 * not supported by attached DVI/HDMI monitor.
 *
 * Implement HDMI AvMute in SettopAPI
 *
 * 6   9/8/05 5:18p mphillip
 * PR16870: bdisplay_settings changes to support this
 *
 * 5   4/20/05 3:54p rgreen
 * PR 12953: Force EDID preferred format on HDMI HotPlug Event
 * Add callback to dvi settings to support
 *
 * 4   4/1/05 4:21p rgreen
 * PR12560: Add support for computer (VESA) formata
 *
 * 3   3/29/05 6:27p ikizyan
 * PR14646: Adding a sharpness control
 *
 * 2   3/23/05 1:30p erickson
 * PR12769: added nonlinear scale option
 *
 * 1   2/7/05 7:05p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 *
 * Irvine_BSEAVSW_Devel/46   12/17/04 9:23a erickson
 * PR12290: update clone docs
 *
 * Irvine_BSEAVSW_Devel/45   12/3/04 3:28p erickson
 * PR12388: added support for vesa video format settings
 *
 * Irvine_BSEAVSW_Devel/44   11/23/04 9:41a erickson
 * PR13336: added color space conversion parameters for decode_window
 *
 * Irvine_BSEAVSW_Devel/43   10/28/04 2:36p rgreen
 * PR 12953: Move edid_preferred_format from display settings to dvi
 * settings
 * Move code to retrieve preferred EDID format from bdisplay_get to
 * boutput_dvi_get
 *
 * Irvine_BSEAVSW_Devel/42   10/27/04 12:18p erickson
 * PR13096: added bdisplay_settings.mpaa_enabled option
 *
 * Irvine_BSEAVSW_Devel/41   10/8/04 5:30p rgreen
 * PR 12953: Initialize display format to preferred EDID format when
 * DVI/HDMI Rx connected
 *
 * Irvine_BSEAVSW_Devel/40   9/16/04 3:36p erickson
 * PR11081: added box_detect and deinterlacer to bdecode_window_settings
 * and comb_filter to banalog_params
 *
 * Irvine_BSEAVSW_Devel/39   9/13/04 1:58p erickson
 * PR11081: added interlacer option for bdisplay
 *
 * Irvine_BSEAVSW_Devel/38   6/2/04 2:43p erickson
 * PR11204: changed/removed deprecated settop api functions
 *
 * Irvine_BSEAVSW_Devel/36   5/17/04 11:03a erickson
 * PR11135: extended settop api with normalized get/set functions for
 * boutput
 *
 * Irvine_BSEAVSW_Devel/35   12/19/03 4:38p vsilyaev
 * PR 8850: Fixing build of legacy code after merge.
 *
 * Irvine_BSEAVSW_Devel/Irvine_BSEAVSW_Devel_7038/1   12/4/03 1:27p erickson
 * PR8850: initial Brutus support on 7038
 *
 * Irvine_BSEAVSW_Devel/31   11/11/03 4:55p erickson
 * PR8600: added note about disabling graphics when bdisplay_set is called
 *
 * Irvine_BSEAVSW_Devel/30   11/11/03 11:30a erickson
 * PR8596: add note to api regarding volume control for PCM data only
 *
 * Irvine_BSEAVSW_Devel/29   11/7/03 4:28p erickson
 * PR8583: removing, fixing or implementing TODO's
 *
 * Irvine_BSEAVSW_Devel/28   11/5/03 9:44a erickson
 * no more is_hd, is_pal. use real metrics like interlaced and
 * refresh_rate
 *
 * Irvine_BSEAVSW_Devel/27   11/4/03 2:42p erickson
 * documentation changes
 *
 * Irvine_BSEAVSW_Devel/26   11/4/03 9:01a erickson
 * unpacked some struct definitions in order to work with docjet
 *
 * Irvine_BSEAVSW_Devel/25   11/3/03 12:22p erickson
 * reversed logic on spdif pass_through. still a boolean.
 *
 * Irvine_BSEAVSW_Devel/24   10/31/03 11:58a erickson
 * settop api changes after internal review
 *
 * Irvine_BSEAVSW_Devel/23   10/30/03 9:47a erickson
 * Copied dbg interface so that we can compile out BRCM_DBG_MSG
 *
 * Irvine_BSEAVSW_Devel/22   10/29/03 5:04p erickson
 * PR8253: enable spdif volume contorl
 *
 * Irvine_BSEAVSW_Devel/21   10/28/03 11:26a erickson
 * settop api reworks after internal design view
 *
 * Irvine_BSEAVSW_Devel/20   10/23/03 2:58p erickson
 * added comments regarding aspect ratio
 *
 * Irvine_BSEAVSW_Devel/19   10/22/03 3:23p erickson
 * edited comment for bdecode_window_settings.cliprect
 *
 * Irvine_BSEAVSW_Devel/18   10/22/03 11:22a erickson
 * settop api pass-through type conversion
 *
 * Irvine_BSEAVSW_Devel/17   10/15/03 10:55a erickson
 * added bdisplay_get_format_settings to return more information about an
 * EDisplayFmt
 *
 * Irvine_BSEAVSW_Devel/16   10/1/03 3:01p erickson
 * added bdisplay_lookup_size and some TODO notes
 *
 * Irvine_BSEAVSW_Devel/15   9/30/03 1:13a erickson
 * updated docjet documentation
 *
 * Irvine_BSEAVSW_Devel/14   9/17/03 5:04p erickson
 * updated documentation for DocJet
 *
 * Irvine_BSEAVSW_Devel/13   8/12/03 9:42a erickson
 * changes after pre-review meeting
 *
 * Irvine_BSEAVSW_Devel/12   7/30/03 2:41p erickson
 * boutput_xxx objects no longer have public bobject_id's.
 *
 * Irvine_BSEAVSW_Devel/11   7/30/03 2:08p erickson
 * removed all return value comments because we're standardized and they
 * add nothing
 *
 * Irvine_BSEAVSW_Devel/10   7/30/03 12:14p erickson
 * static allocation of boutput's and default outputs for a display
 * removed decode_window_attach/dettach in favor of clone
 *
 * Irvine_BSEAVSW_Devel/9   7/30/03 10:20a erickson
 * some comments and TODO's
 *
 * Irvine_BSEAVSW_Devel/8   7/29/03 6:03p erickson
 * more rework
 *
 * Irvine_BSEAVSW_Devel/7   7/28/03 4:43p erickson
 * more comment work
 *
 * Irvine_BSEAVSW_Devel/6   7/25/03 8:13p vsilyaev
 * Added component output, fixed compilation errors.
 *
 * Irvine_BSEAVSW_Devel/5   7/25/03 4:22p erickson
 * in the middle of rework, does not compile
 *
 * Irvine_BSEAVSW_Devel/4   7/24/03 4:37p vsilyaev
 * Added support for multiple channels and implemented decode window API.
 *
 * Irvine_BSEAVSW_Devel/3   7/9/03 5:19p erickson
 * rework for new bband_t model
 *
 * Irvine_BSEAVSW_Devel/2   6/26/03 3:59p vsilyaev
 * Declare enum for the component output types.
 *
 * Irvine_BSEAVSW_Devel/1   6/13/03 3:08p vsilyaev
 * SetTop reference API.
 *
 *
 ***************************************************************************/
#ifndef BSETTOP_DISPLAY_H__
#define BSETTOP_DISPLAY_H__

#ifdef __cplusplus
extern "C"
{
#endif

/*=*******************************************
A display is a coordinated set of video/audio outputs that are usually sent
to one TV set.
Each display may have multiple outputs (e.g. svideo, composite), but each
output will carry the same content.

Decode windows are boxes of video content on a display. You control decode
windows and audio volume through the display API.

Some chips support more than one display (7038, 7320 and Venom2).
For these systems, you will open two bdisplay_t handles and have one bdecode_window_t handle
created for each display.
**********************************************/

#define boutput_hdmi_cec_max_transmit_length 15 /* + 1 for CEC header length */
#define boutput_hdmi_cec_broadcast_address 0xF  /* CEC Broadcast address */

/*
Summary:
    Open a display.
Description:
    The new display object will already have default outputs configured.
    You can change the configuration by calling bdisplay_get, making changes,
    then calling bdisplay_set.
*/
bdisplay_t bdisplay_open(
    bobject_t display_id /* handle used to identify a particular display */
    );

/*
Summary:
   Close a display.
Description:
   All decode windows opened or cloned for this display should have already been closed,
   otherwise you get an inconsistent state.
*/
void bdisplay_close(
    bdisplay_t display /* handle returned by bdisplay_open */
    );

/*
Summary:
    svideo output handle returned by boutput_svideo_open()
*/
typedef struct boutput_svideo *boutput_svideo_t;

/*
Summary:
    composite output handle returned by boutput_composite_open()
*/
typedef struct boutput_composite *boutput_composite_t;

/*
Summary:
    RF modulator output handle returned by boutput_rf_open()
*/
typedef struct boutput_rf *boutput_rf_t;

/**
Summary:
Settings for RF output
**/
typedef struct boutput_rf_settings {
    unsigned channel;                   /* Channel 3 or 4 */
    char country[2];                    /* 2 letter iso3166 country code */
} boutput_rf_settings;

/**
Summary:
Set new RF settings.
Description:
These settings will not be applied until bdisplay_set is called with the
RF handle assigned.
**/
bresult boutput_rf_set(boutput_rf_t rf,
    const boutput_rf_settings *settings);

/**
Summary:
Get the current RF settings.
**/
void boutput_rf_get(boutput_rf_t rf,
    boutput_rf_settings *settings /* [out] */
    );

/*
Summary:
    HDMI output handle returned by boutput_hdmi_open()
*/
typedef struct boutput_hdmi *boutput_hdmi_t;


/**
Summary:
Settings for SPDIF output
**/
typedef struct boutput_spdif_channel_status {
    bool        professional_mode;  /* [0:0] The professional mode flag.
                                    TRUE: Professional mode. Other user
                                    options will not be considered.
                                    FALSE: Consumer mode. - Read Only*/

    bool        pcm;                /* [1:1] For output Linear  bLinear_PCM=true,
                                    If output is Compressed bLinear_PCM=false - Read Only */

    bool        copyright;          /* [2:2] Software CopyRight assert.
                                    TRUE: CopyRight is asserted
                                    FALSE: CopyRight is not asserted    */

    uint8_t     pre_emphasis;       /*[3:5] pre-emphasis */

    uint8_t     channel_status_mode;/* [6:7] Gives channel status - Read Only*/

    uint8_t     category_code;      /* [8:15] Category code in spdif channel status, including L-bit */

    uint8_t     source_number;      /*[16:19] Source  number*/

    bool        channel_num;        /*[20:23]  TRUE:  Left channel num = 0000
                                    Right Channel Num = 0000
                                    FALSE: Left channel num = 1000
                                    Right Channel Num = 0100 */

    uint8_t     sampling_frequency; /*[24:27]Sampling frequency - Read Only*/

    uint8_t     clock_accuracy;     /* [28:29] Clock Accuracy */

    bool        max_word_length;    /* [32:32] For setting of 32bit of Channel Status
                                    Bit 32 0 Maximum audio sample word length is 20 bits
                                    1 Maximum audio sample word length is 24 bits */

    uint8_t     sample_word_length; /* [33:35] Sample word length */

    uint8_t     original_sampling_frequency;/* [36:39] Original sampling frequency*/

    uint8_t     cgms_a;             /* [40:41] */
} boutput_spdif_channel_status;

/*
Summary:
Settings returned by bdisplay_get_format_settings
*/
typedef struct bvideo_format_settings {
    unsigned width;   /* height of video format in pixels */
    unsigned height;  /* width of video format in lines */
    unsigned refresh_rate;  /* 20061213 DEPRECATED -- refresh rate of video format in Hz */
    bvideo_frame_rate frame_rate; /* 20061213 bandrews - replaces refresh_rate */
    bool interlaced;
} bvideo_format_settings;

/*
Summary:
    HDMI HDCP Status
*/
typedef enum boutput_hdmi_hdcp_state
{
    boutput_hdmi_hdcp_state_internal_err,
    boutput_hdmi_hdcp_state_disabled,
    boutput_hdmi_hdcp_state_init,
    boutput_hdmi_hdcp_state_auth_fail,

    boutput_hdmi_hdcp_state_ri_fail,
    boutput_hdmi_hdcp_state_pj_fail,
    boutput_hdmi_hdcp_state_repeater_fail,

    boutput_hdmi_hdcp_state_enabled
} boutput_hdmi_hdcp_state;

/*
Summary:
    HDMI CEC message type
*/
typedef enum boutput_hdmi_cec_message_type
{
    boutput_hdmi_cec_message_type_transmit = 0,
    boutput_hdmi_cec_message_type_receive = 1
} boutput_hdmi_cec_message_type ;

/*
Summary:
    All information from a received CEC message or to
    transmit a CEC message
*/
typedef struct boutput_hdmi_cec_message_data
{
    boutput_hdmi_cec_message_type message_type;
    uint8_t initiator_addr;
    uint8_t destination_addr;
    uint8_t opcode;
    uint8_t message_buffer[boutput_hdmi_cec_max_transmit_length];
    uint8_t message_length;
} boutput_hdmi_cec_message_data;

/*
Summary:
    All CEC related information from the device.
*/
typedef struct boutput_hdmi_cec_configuration
{
    uint8_t logical_address;
    uint8_t physical_address[2];
    uint8_t device_type;
} boutput_hdmi_cec_configuration;

/*
Summary:
    HDMI deep color mode enum
*/
typedef enum boutput_hdmi_color_depth
{
	boutput_hdmi_color_depth_8bit,
	boutput_hdmi_color_depth_10bit,
	boutput_hdmi_color_depth_12bit,
	boutput_hdmi_color_depth_16bit
} boutput_hdmi_color_depth;

/**
Summary:
Settings for HDMI output
**/
typedef struct boutput_hdmi_settings {
    bvideo_format edid_preferred_format ; /* format preferred by attached DVI/HDMI device */
    bvideo_format_settings vesa_settings ; /* if format == bvideo_format_vesa,
                                        then these settings are used. */

    bool hdcp ;
    uint16_t hdcp_failure_retry_delay ;
    bool hdcp_pj_checking_enable ;
    bool preemphasis_support ;
    bool hdcp_disable_blue_screen;  /* If true, upon HDCP failure, a OSD message will be displayed on HDMI
                                            instead of blue screen */
    bool validate_edid_format;      /* If true, bdisplay_set will only accept formats
                                       that are valid in the attached device's EDID data. */
    unsigned hdmi_avmute_delay_pre_format_change;    /* Delay (in ms) after sending AVMute and before the format change */									   
    unsigned hdmi_avmute_delay_post_format_change;    /* Delay (in ms) before sending AVUnMute and after the format change */	
    bsettop_callback hdmi_hotplug_callback;     /* Called when a device connects */
    bsettop_callback hdmi_disconnect_callback;  /* Called when a device disconnects */
    bsettop_callback hdmi_hdcp_status_callback; /* Called when HDCP status changes */
    bsettop_callback hdmi_cec_message_callback; /* Called when a CEC message is received */
    void *callback_context;             /* all callbacks share this context */

    boutput_hdmi_audio_mode hdmi_audio_mode;  /* Audio mode for HDMI */
    baudio_format compressed_audio_format; /* compressed output format.  If hdmi_audio_mode = compressed and this is
                                              set to a valid format (e.g. baudio_format_dts), output will be
                                              transcoded to that format.  By default this is baudio_format_unknown,
                                              which will leave the compressed data in the original format. */
    boutput_spdif_channel_status    hdmi_spdif_channel_status; /* HDMI's Spdif audio output's Channel status */
    unsigned audio_delay; /* delay, in ms, applied to the HDMI audio output */
    boutput_hdmi_color_depth hdmi_color_depth;  /* color depth settings 8, 10, 12, or 16bits */
} boutput_hdmi_settings;

/**
Summary:
Set new HDMI settings.
Description:
These settings will not be applied until bdisplay_set is called with the
HDMI handle assigned.
**/
bresult boutput_hdmi_set(boutput_hdmi_t dvi,
    const boutput_hdmi_settings *settings);

/**
Summary:
Get the current HDMI settings.
**/
void boutput_hdmi_get(boutput_hdmi_t dvi,
    boutput_hdmi_settings *settings /* [out] */
    );

/**
Summary:
  DVI/HDMI device capabilities
**/
typedef struct boutput_hdmi_capabilities
{
    bvideo_format preferred_video_format;
    bool video_format_is_supported[bvideo_format_count];

    boutput_hdmi_audio_mode preferred_audio_mode;
    bool audio_mode_is_supported[boutput_hdmi_audio_mode_count];

    bool hdmi;  /* True if HDMI, false if DVI */
    boutput_hdmi_rx_edid_info rx_edid_info;     /* contians HDMI Rx EDID info. */
} boutput_hdmi_capabilities;

/**
Summary:
Get the current HDMI device capabilities.
**/
bresult boutput_hdmi_get_capabilities(boutput_hdmi_t dvi,
                                     boutput_hdmi_capabilities *caps /* [out] */
                                     );

/**
Summary:
    The current HDMI status
**/
typedef struct boutput_hdmi_status
{
    boutput_hdmi_hdcp_state hdcp_state;
} boutput_hdmi_status;

/**
Summary:
Get the current HDCP status
**/
bresult boutput_hdmi_get_status(boutput_hdmi_t dvi,
                                boutput_hdmi_status *status /* [out] */
                                     );

/**
Summary:
Get the received CEC message
**/
bresult boutput_hdmi_cec_get_message(boutput_hdmi_t dvi,
                                boutput_hdmi_cec_message_data *cec_message_data /* [out] */
                                );

/**
Summary:
Send CEC message
**/
bresult boutput_hdmi_cec_send_message(boutput_hdmi_t dvi,
                                const boutput_hdmi_cec_message_data *cec_message_data
                                );

/**
Summary:
Get the CEC configuration from the device
**/
bresult boutput_hdmi_cec_get_configuration(boutput_hdmi_t dvi,
                                boutput_hdmi_cec_configuration *cec_configuration   /* [out] */
                                );

/**
Summary:
Check the current status of the device
**/
bool boutput_hdmi_cec_is_device_ready(boutput_hdmi_t dvi);


/*
Summary:
    Component output handle returned by boutput_component_open()
*/
typedef struct boutput_component *boutput_component_t;

/*
Summary:
    Color format of the component output.
*/
typedef enum boutput_component_type {
    boutput_component_type_rgb,     /* RGB output */
    boutput_component_type_yprpb    /* YPrPb output */
} boutput_component_type;

/**
Summary:
Settings for Component output
**/
typedef struct boutput_component_settings {
    boutput_component_type type;    /* color space of the component video */
} boutput_component_settings;

/**
Summary:
Set new Component settings.
Description:
These settings will not be applied until bdisplay_set is called with the
Component handle assigned.
**/
bresult boutput_component_set(boutput_component_t component,
    const boutput_component_settings *settings);

/**
Summary:
Get the current Component settings.
**/
void boutput_component_get(boutput_component_t component,
    boutput_component_settings *settings /* [out] */
    );

/*
Summary:
    SPDIF audio handle returned by boutput_spdif_open()
*/
typedef struct boutput_spdif *boutput_spdif_t;

/**
Summary:
Settings for SPDIF output
**/
typedef struct boutput_spdif_settings {
    bool        pcm;                /* If true, always send PCM audio to SPDIF output. If the format
                                    is not supported by the chip, you will get no audio output.
                                    If false, the audio will be passed-through if supported
                                    by SPDIF, otherwise it is decoded by the chip.

                                    Specifics:
                                    MPEG audio (layer 1/2) will always be decoded and PCM
                                    will be sent to SPDIF.
                                    We cannot decode any audio format other than MPEG audio.
                                    On pre-7038 chips, we can pass-through AC3.
                                    On 7110/7315 B0, we can pass-through DTS.
                                    On 7038, we can also pass-through MP3 and AAC.*/

    boutput_spdif_channel_status    spdif_channel_status; /* Spdif Channel status */
    unsigned    audio_delay;                              /* delay, in ms, applied to the SPDIF output */
    baudio_format compressed_audio_format; /* compressed output format.  If pcm = false and this format is
                                              set to a valid format (e.g. baudio_format_dts), output will be
                                              transcoded to that format.  By default this is baudio_format_unknown,
                                              which will leave the compressed data in the original format. */
} boutput_spdif_settings;

/**
Summary:
Set new SPDIF settings.
Description:
These settings will not be applied until bdisplay_set is called with the
SPDIF handle assigned.
**/
bresult boutput_spdif_set(boutput_spdif_t spdif,
    const boutput_spdif_settings *settings);

/**
Summary:
Get the current SPDIF settings.
**/
void boutput_spdif_get(boutput_spdif_t spdif,
    boutput_spdif_settings *settings /* [out] */
    );

/*
Summary:
    Open a SPDIF audio output.
Description:
    It must be assigned to a display.
*/
boutput_spdif_t
boutput_spdif_open(bobject_t spdif_id);

/*
Summary:
    Open RF modulator output.
Description:
    It must be assigned to a display.
*/
boutput_rf_t
boutput_rf_open(bobject_t rfmod_id);

/*
Summary:
    Open composite output.
*/
boutput_composite_t
boutput_composite_open(bobject_t composite_id);

/*
Summary:
    Open svideo output.
*/
boutput_svideo_t
boutput_svideo_open(bobject_t svideo_id);

/*
Summary:
    Open HDMI output.
Description:
    It must be assigned to a display.
*/
boutput_hdmi_t
boutput_hdmi_open(bobject_t dvi_id);

/*
Summary:
    Open component output.
Description:
    It must be assigned to a display.
*/
boutput_component_t
boutput_component_open(bobject_t component_id);

/*
Summary:
    Physical aspect ratio of the SD display.
Description:
    This only applies to SD displays. HD displays are assumes to be 16x9 and this
    value is ignored.
*/
typedef enum bdisplay_aspect_ratio {
  bdisplay_aspect_ratio_4x3,
  bdisplay_aspect_ratio_16x9
} bdisplay_aspect_ratio;

/*
Summary:
    How to convert content to a display based on its aspect ratio.
Description:
    If a decode window is full size, then the zoom, box or panscan modes are applied.
    Otherwise the content is sized to the decode window.
    The behavior of this enum depends on the bdisplay_aspect_ratio.
*/
typedef enum bdisplay_content_mode {
   bdisplay_content_mode_zoom,      /* cut off content to preserve aspect ratio */
   bdisplay_content_mode_box,       /* either windowbox or letterbox to preserve aspect ratio */
   bdisplay_content_mode_panscan,   /* Used for 16x9 source with pan scan vectors on 4x3 display only,
                                        otherwise same as zoom. */
   bdisplay_content_mode_full,      /* distort aspect ratio but see all the content and no sidebars. */
   bdisplay_content_mode_full_nonlinear /* non-linear upscaling to full screen where
                                    the edge of the content will have more distorted
                                    aspect ratio. */
} bdisplay_content_mode;

/*
  Summary:
  Digital Noise Removal Levels
*/
typedef enum bwindow_dnr_level {
   /* Disable DNR */
   bwindow_dnr_level_off = 0,
   /* These can be referred to by name */
   bwindow_dnr_level_low,
   bwindow_dnr_level_medium_low,
   bwindow_dnr_level_medium,
   bwindow_dnr_level_medium_high,
   bwindow_dnr_level_high,
   /* And also by number */
   bwindow_dnr_level_1=bwindow_dnr_level_low,
   bwindow_dnr_level_2,
   bwindow_dnr_level_3,
   bwindow_dnr_level_4,
   bwindow_dnr_level_5,
   /* Highest value available */
   bwindow_dnr_level_max=bwindow_dnr_level_5
} bwindow_dnr_level;

/*
Summary:
    Split Screen Mode for DNR (Digital Noise Reduction) and PEP (Picture Enhancement Processing) Demo
*/
typedef enum bwindow_splitscreen_mode {
    bwindow_splitscreen_mode_wholescreen = 0,   /* apply processing to entire screen */
    bwindow_splitscreen_mode_left,          /* apply processing to left-half of screen */
    bwindow_splitscreen_mode_right          /* apply processing to right-half of screen */
} bwindow_splitscreen_mode;

/*
Summary:
    Macrovision types.
*/
typedef enum bmacrovision_type {
    bmacrovision_type_none = 0, /* No macrovision on outputs. */
    bmacrovision_type_agc_only, /* AGC only. */
    bmacrovision_type_agc_2lines,   /* AGC + 2 Line color stripe. */
    bmacrovision_type_agc_4lines,   /* AGC + 4 Line color stripe. */
    bmacrovision_type_custom,   /* User-provded tables. */
    bmacrovision_type_agc_only_rgb, /* PAL MV cert test AGC only with MV on RGB. */
    bmacrovision_type_agc_2lines_rgb,   /* NTSC MV cert test AGC + 2 Line color stripe, with MV on RGB. */
    bmacrovision_type_agc_4lines_rgb,   /* NTSC MV cert test AGC + 4 Line color stripe. */
    bmacrovision_type_test01,   /* MV certification test 01. */
    bmacrovision_type_test02    /* MV certification test 02. */
} bmacrovision_type;

/*
Summary:
    Macrovision table structure.
*/
typedef struct bmacrovision_tables {
    /* See bvdc_macrovision.h for the source of the numbers */
    uint8_t cpc_table[2];
    uint8_t cps_table[33];
} bmacrovision_tables;

/*
 Summary:
    DCS (Dwight Cavendish Systems ) types.
*/
typedef enum bdcs_type {
   bdcs_type_dcs_off = 0,
   bdcs_type_dcs_on
} bdcs_type;


/*
Summary:
Display settings for configuring outputs, video format and aspect ratio.
*/
typedef struct bdisplay_settings {
    bvideo_format format;               /* Video format of the display. */
    bdisplay_aspect_ratio aspect_ratio; /* Aspect ratio of the display. This only applies to SD display formats. */
    bdisplay_content_mode content_mode; /* How to adapt content to the aspect ratio
                                        of the display. */
    bool mpaa_enabled;              /* Enabled MPAA if available */

    boutput_composite_t composite;  /* attached composite output */
    boutput_composite_t composite2; /* attached 2nd composite output */
    boutput_svideo_t svideo;        /* attached svideo output */
    boutput_component_t component;  /* attached component output */
    boutput_rf_t rf;                /* attached RF modulator output */
    boutput_hdmi_t dvi; /* attached DVI or HDMI output */
    boutput_spdif_t spdif;          /* attached SPDIF audio output */

    bvideo_format_settings vesa_settings; /* if format == bvideo_format_vesa,
                                        then these settings are used. */
    bmacrovision_type macrovision_type; /* Macrovision output type */
    bmacrovision_tables *macrovision_tables;    /* Custom macrovision tables. This is only used for bmacrovision_type_custom, NULL on bdisplay_get, if unchanged, or if unused. */
    bdcs_type dcs_type; /* controls Dwight Cavendish Systems  processing */
    uint32_t background_color; /* background color in YCrCb colorspace */
    unsigned i2s_delay; /* delay, in ms, applied to the I2S audio output port */
    unsigned dac_delay; /* delay, in ms, applied to the DAC audio output port */
} bdisplay_settings;

/*
Summary:
Look up video format settings for a display.

Description:
This is different from bvideo_get_format_settings because it also supports
vesa format settings if settings->format == bvideo_format_vesa.
*/
void bdisplay_get_video_format_settings(
    const bdisplay_settings *settings,
    bvideo_format_settings *format_settings /* [out] */
    );

/*
Summary:
Look up settings for a bvideo_format

Description:
This function does not support bvideo_format_vesa. Use bdisplay_get_video_format_settings
for that.
*/
void bvideo_get_format_settings(
    bvideo_format format,           /* Format that is being queried */
    bvideo_format_settings *settings    /* [out] Information about that format */
    );

/*
Summary:
   Configure the outputs, video format and aspect ratio of the display.

Description:
   format and aspect_ratio are simple variables. The other members (svideo, composite, etc.)
   are handles to boutput_xxx objects.

   boutput_xxx_open functions return handles to objects that may already exist and may
   already be assigned to display's by default. The exact boutput default configuration
   is documented for each platform.

    If a graphics engine is linked to this display, bdisplay_set will automatically
    disable the graphics engine. You must call bgraphics_set,
    set bgraphics_settings.enabled=true,
    make any other necessary changes (like scaling) because of the new
    display format, then call bgraphics_set.
    This must be done because the graphics state may fail with the display format
    changes and these issues must be handled by the application.

Example:
    // Example 1 doesn't assume any default outputs
    bdisplay_t display = bdisplay_open(B_ID(0)); // first display
    bdisplay_settings settings;
    boutput_component_t component = boutput_component_open(B_ID(0)); // first component output
    boutput_hdmi_t dvi = boutput_hdmi_open(B_ID(0)); // first hdmi output

    bdisplay_get(diplay, &settings);
    settings.format = bvideo_format_ntsc;
    settings.aspect_ratio = bdisplay_aspect_ratio_4x3;
    settings.content_mode = bdisplay_content_mode_full;
    settings.component = component;
    settings.dvi = dvi;
    bdisplay_set(diplay, &settings);

    // Example 2 is based on the knowledge that the 97320 implementation comes
    // with svideo pre-configured for display 0.
    bdisplay_t display0 = bdisplay_open(B_ID(0)); // first display
    bdisplay_t display1 = bdisplay_open(B_ID(1)); // second display
    bdisplay_settings settings0, settings1;

    bdisplay_get(diplay0, &settings0);
    bdisplay_get(diplay1, &settings1);
    if (settings0.rf) {
        boutput_rf_settings rf_settings;
        boutput_rf_get(settings0.rf, &rf_settings);
        rf_settings.channel = 4;
        boutput_rf_set(settings0.rf, &rf_settings);
    }
    settings1.svideo = settings0.svideo;
    settings0.svideo = NULL;
    bdisplay_set(diplay0, &settings0); // must set 0 first to clear svideo
    bdisplay_set(diplay1, &settings1);
*/
bresult
bdisplay_set(
    bdisplay_t display, /* handle returned by bdisplay_open */
    const bdisplay_settings *settings   /* desired display settings */
    );

/*
Summary:
    Get the current outputs, video format and aspect ratio for a display.
*/
void
bdisplay_get(
    bdisplay_t display, /* handle returned by bdisplay_open */
    bdisplay_settings *settings /* [out] current settings of display */
    );

/*
Summary:
 Open a new decode window.
Description:
 The window_id is a global id, not relative to the display.
 However, the same window ID has a different meaning when used on different displays.

 On a dual decode/dual display system:
  Window 0 can only be used on display 0.
  Window 1 can be used on display 0 or 1. Window 1 on display 0 is PIP. Window 1 on display 1 is a "dual TV" system.
  See bdecode_window_clone for window ID's 2 and 3 on display 1.

 On a single decode/dual display system:
  Window 0 can only be used on display 0.
  See bdecode_window_clone for window ID 1 on display 1.
*/
bdecode_window_t bdecode_window_open(
    bobject_t window_id, /* window's object id */
    bdisplay_t display /* display on which the window appears */
    );

/*
Summary:
 Open a new mosaic decode window.
Description:
 Mosaic windows derive from a parent video window.  The parent can be created either
 via bdecode_window_open or bdecode_window_clone.

 Mosaic windows can be independently sized and placed within the parent window, but do
 not have zorder relative to each other on the display, therefore they can not overlap.
 The zorder of the parent window will be used to determine compositing in the final display.

 If mosaic windows are to be used on a dual-display system that uses bdecode_window_clone,
 a corresponding mosaic window should be opened on both the master window and the cloned
 window.  Each can be sized independently for aspect ratio correction.

 The mosaic object ID passed in to this routine must match the object ID passed into
 bdecode_open_mosaic.
*/
bdecode_window_t bdecode_window_open_mosaic(
    bdecode_window_t parent_window, /* Parent window to derive from */
    bobject_t mosaic_id /* The object ID for the mosaic decoder.  This must match the value passed to bdecode_open_mosaic. */
    );

/*
Summary:
    Hide and close a decode window.
Description:
    Any decode which you started to this decode window should have already been
    stopped.
*/
void
bdecode_window_close(
    bdecode_window_t window
    );

/*
Summary:
Create a new window by cloning from another window which allows the same
video content to go to multiple windows on different displays.

Description:
This supports the HD/SD simul mode on dual display platforms like 7038 and 740x.
In some HD/SD simul configurations, each window can have different dimensions and
aspect ratios, and so the separate bdecode_window_t handles allow the API to
control this.

You can only call bdecode_start with the original window, not with the clone.

For single decode systems (e.g. 7401, 7118), you can clone window0 on dispay0
 to window1 on display1. This is an HD/SD simul system without PIP.

For dual decode systems (e.g. 7038, 7400), you can clone window0 on display0
 to window2 on display1, and window1 on display0 to window3 on display 1.
 This is an HD/SD simul system with PIP.
*/
bdecode_window_t bdecode_window_clone(
            bdecode_window_t window, /* window to be cloned */
            bobject_t clone_window_id, /* id for new cloned window */
            bdisplay_t display /* display on which the cloned window will appear */
        );

/*
Summary:
   Set the audio levels for the DAC's associated with this display.
*/
bresult bdisplay_set_dac_audio_volume(
    bdisplay_t display,     /* handle returned by bdisplay_open */
    const baudio_volume *volume /* desired volume */
    );

/*
Summary:
   Get the audio levels for the DAC's associated with this display.
*/
bresult bdisplay_get_dac_audio_volume(
    bdisplay_t display,     /* handle returned by bdisplay_open */
    baudio_volume *volume   /* [out] current volume of DAC */
    );

/*
Summary:
    Set the audio mute for the rf modulator.
Description:
    This function only sets mute. The rf modulator's input comes from the audio DAC's,
    so any change to bdisplay_get_dac_audio_volume will affect its level.
*/
bresult boutput_rf_set_audio_volume(
    boutput_rf_t rf,
    const baudio_volume *volume /* desired volume */
    );

/*
Summary:
   Set the spdif volume.
Description:
    Does not set the spdif->audio_format. Use bdisplay_set for this.
*/
bresult boutput_spdif_set_audio_volume(
    boutput_spdif_t spdif,
    const baudio_volume *volume /* desired volume */
    );

/*
Summary:
   Get the spdif volume.
Description:
    The spdif volume control only works for PCM audio. Compressed data
    cannot be modified and must be handled by the external SPDIF decoder.
*/
bresult boutput_spdif_get_audio_volume(
    boutput_spdif_t spdif,
    baudio_volume *volume /* [out] current volume of spdif device */
    );


/*
Summary:
   Get the audio levels for the rf modulator.
*/
bresult boutput_rf_get_audio_volume(
    boutput_rf_t rf,
    baudio_volume *volume   /* [out] current volume of the rf modulater */
    );

/* End of Display API  */


/* Decode window API */

/*
Summary:
   Decode window settings returned by bdecode_window_get.
*/
typedef struct bdecode_window_settings {
    bsettop_rect position;      /* Position and size on the display. Use display-relative coordinates. */
    bsettop_rect cliprect;      /* Source position and size. Use window-relative coordinates
                                     and not source-relative coordinates. This has the advantage of allowing
                                     the Settop API to reapply them for dynamic picture change, analog scale down, etc.
                                     If you change the size of the window, you must recalculate the cliprect.
                                     See api/examples/window_clipping.c for example.
                                     If width or height is 0, no clipping is applied. */
    bool visible;                /* Is the window visible? */
    unsigned zorder;             /* 0 is on bottom. For two decode window systems, 1 is on top. */
    bool cloned;                 /* read-only. true if this window was created with
                                     bdecode_window_clone. */
    bool deinterlacer;           /* If true, the motion adaptive deinterlacer (MAD)
                                     is allowed to be used if available and appropriate. */
    bool box_detect;             /* If true, the box detect hardware is allowed to be
                                     used if available and appropriate. */

    int16_t contrast;            /* Color space conversion for the decode window (not the
                                     display). Ranges between -32768 and 32767.
                                     Default is 0. */
    int16_t saturation;          /* Color space conversion for the decode window (not the
                                     display). Ranges between -32768 and 32767.
                                     Default is 0. */
    int16_t hue;                 /* Color space conversion for the decode window (not the
                                     display). Ranges between -32768 and 32767.
                                     Default is 0. */
    int16_t brightness;          /* Color space conversion for the decode window (not the
                                     display). Ranges between -32768 and 32767.
                                     Default is 0. */
    int16_t sharpness;           /* Sharpness enhancement for the decode window (not the
                                     display). Ranges between -32768 and 32767.
                                     Default is 0. */

    bwindow_dnr_level dcr_level; /* Digital Contour Reduction Level */
    bwindow_dnr_level mnr_level; /* Mosquito Noise Reduction Level */
    bwindow_dnr_level bnr_level; /* Mosquito Noise Reduction Level */
    bwindow_dnr_level anr_level; /* Analog Noise Reduction Level */

    unsigned green_stretch;      /* Green Stretch Value -- Ranges between 0 (default) and 4 */
    unsigned blue_stretch;       /* Blue Stretch Value -- Ranges between 0 (default) and 4 */
    unsigned auto_flesh_tone;    /* Auto Flesh Tone Value -- Ranges between 0 (default) and 4 */

    unsigned dynamic_contrast;   /* Dynamic contrast value.  Ranges from 0 (default) and 256  */

    bool display_rate_master;    /* Set this property to true if the window should control the
                                    display output timebase.  This is a toggle property.  There
                                    must be only one display rate master, settings this to true
                                    will clear the property from all other windows on the display.
                                    Setting this to false will have no effect. */
    bwindow_splitscreen_mode splitscreen_mode;  /* Split Screen Demo Mode */
} bdecode_window_settings;

/*
Summary:
    Get the current settings of a decode window.
*/
bresult bdecode_window_get(
    bdecode_window_t window,
    bdecode_window_settings *settings   /* [out] */
    );

/*
Summary:
    Apply new settings to the decode window.
Description:
    Only those individual settings which have changed will be asserted.
*/
bresult bdecode_window_set(
    bdecode_window_t window,
    const bdecode_window_settings *settings
    );

/* End of decode window API  */

#ifdef __cplusplus
}
#endif


#endif /* BSETTOP_H__ */

