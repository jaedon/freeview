/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bhdr_config.h $
 * $brcm_Revision: Hydra_Software_Devel/93 $
 * $brcm_Date: 9/24/12 5:53p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/hdr/src/common/bhdr_config.h $
 * 
 * Hydra_Software_Devel/93   9/24/12 5:53p rgreen
 * SW7445-24:  Add 28nm support
 * 
 * Hydra_Software_Devel/92   7/17/12 5:46p rgreen
 * SW7425-73: Merge Changes
 * 
 * Hydra_Software_Devel/SW7435-73/1   7/17/12 5:14p rgreen
 * SW7435-73:  Disable reset of clock/data channels when PLL state changes
 * 
 * Hydra_Software_Devel/91   7/3/12 5:48p vle
 * SW7429-187: Add support for 7429 B0
 * 
 * Hydra_Software_Devel/90   3/28/12 1:55p rgreen
 * SW7425-2515: Remove CEC support.  See CEC pi for CEC support
 * 
 * Hydra_Software_Devel/89   1/30/12 4:33p rgreen
 * SW7429-54,SW7435-34: Add workaround to update MAI bus configuration on
 * sample rate changes detected at vsync.  Update BHDR_GetAudioSampleFreq
 * to an iisr declaration
 * 
 * Hydra_Software_Devel/88   1/12/12 3:29p rgreen
 * SW7429-43: Add HBR audio support;
 * 
 * Hydra_Software_Devel/87   11/1/11 2:11p rgreen
 * SW7435-20: Add HDR Support for 7435
 * 
 * Hydra_Software_Devel/86   10/31/11 4:33p rgreen
 * SW7435-20:  Add 7435 HDR Support
 * 
 * Hydra_Software_Devel/85   10/10/11 1:21p rgreen
 * SW7429-21: Add 7429 Support
 * 
 * Hydra_Software_Devel/84   10/6/11 2:03p rgreen
 * SW7429-21: Add 7429 support
 * 
 * Hydra_Software_Devel/83   9/27/11 4:27p rgreen
 * SW7425-1233,SW7425-1249: Use udpated GCP handler; 7425 Bx does not
 * need to reset pixel clock and estimation circuits  when PLL locks,
 * unlocks
 * 
 * Hydra_Software_Devel/82   9/9/11 4:14p rgreen
 * SW7425-1249: Add support to calculate frequency on multiple channels
 * 
 * Hydra_Software_Devel/81   9/2/11 5:00p rgreen
 * SW7425-1233: Fix 742x B0 compilation
 * 
 * Hydra_Software_Devel/80   8/26/11 3:02p rgreen
 * SW7425-1200,SWDTV-7644:  Add dynamic power management support; Enable
 * DVP_HR timer support for 35230 only
 * 
 * Hydra_Software_Devel/79   8/19/11 3:29p rgreen
 * SWDTV-5441: Merge virtual TMR support back to main branch
 * 
 * Hydra_Software_Devel/SWDTV-5441/2   8/2/11 11:28a rgreen
 * SWDTV-5541:  Fix intermittent A/V loss.  Force audio/video recovery
 * 
 * Hydra_Software_Devel/SWDTV-5441/1   7/29/11 10:25a rgreen
 * SWDTV-5441: Use exclusive DVP_HR timer hardware instead of shared timer
 * hardware
 * 
 * Hydra_Software_Devel/78   8/17/11 3:42p rgreen
 * SWDTV-8288,SW7425-991:  Add hw passthrough support for 7425
 * 
 * Hydra_Software_Devel/77   8/12/11 12:56p rbshah
 * SWDTV-8236: 35233: Bypass setup of frequency range table for B0.
 * 
 * Hydra_Software_Devel/76   7/18/11 3:55p rgreen
 * SW7425-692: Add debug option to display power management
 * 
 * Hydra_Software_Devel/75   7/11/11 8:30p rgreen
 * SWDTV-6898: Add option for Hot Plug Detect signal disconnected from the
 * HDMI Rx
 * 
 * Hydra_Software_Devel/74   7/7/11 3:55p rgreen
 * SWDTV-5803: Enable manual PLL range setting for 35125
 * 
 * Hydra_Software_Devel/73   6/22/11 11:13a rgreen
 * SWDTV-6771: Add compile time option support for ISRC packets
 * 
 * Hydra_Software_Devel/72   6/10/11 3:49p rgreen
 * SWDTV-6867,SWDTV-7021: Merge Changes
 * 
 * Hydra_Software_Devel/hdr_35233/4   6/10/11 5:52p rbshah
 * SWDTV-6867: 35233: Incorporate review comments from Ron G.
 * 
 * Hydra_Software_Devel/hdr_35233/3   6/1/11 3:43p rbshah
 * SWDTV-6867: 35233: Clean up after bring up
 * 
 * Hydra_Software_Devel/hdr_35233/2   5/12/11 4:21p rbshah
 * SWDTV-7021: 35233: Setup TVMicro for proper hotplug handling.
 * 
 * Hydra_Software_Devel/hdr_35233/1   5/1/11 9:45a rbshah
 * SWDTV-6867: 35233: Initial bringup on 935233 CARD board.
 * 
 * Hydra_Software_Devel/71   5/13/11 3:07p rgreen
 * SWDTV-7092: Add additional options for Plugfest debugging
 * 
 * Hydra_Software_Devel/70   4/26/11 3:28p rgreen
 * SWDTV-6461: Add include file workaround for DVP_HR_OTP RDB block
 * 
 * Hydra_Software_Devel/69   4/26/11 12:01p rgreen
 * SWDTV-6184: Disable manual Frequency Range detection for 35233,
 * Organize configuration options
 * 
 * Hydra_Software_Devel/68   4/25/11 4:38p rgreen
 * SWDTV-6461: Add 35233 compilation support
 * 
 * Hydra_Software_Devel/67   4/21/11 3:54p rgreen
 * SW7422-186: Configure HDCP  Repeater Support  for all chips with both
 * HDMI Rx and Tx
 * 
 * Hydra_Software_Devel/66   4/12/11 2:37p rbshah
 * SWDTV-6184: 35233: Skip define BHDR_CONFIG_HDCP_KEY_OTP_ROM for now.
 * 
 * Hydra_Software_Devel/66   4/12/11 9:49a rbshah
 * SWDTV-6184: Don't define BHDR_CONFIG_HDCP_KEY_OTP_ROM for 35233 for
 * now.
 * 
 * Hydra_Software_Devel/65   3/30/11 5:02p rbshah
 * SWDTV-6184: 35233: Get HDR PI to build for the 35233 DTV chip.
 * 
 * Hydra_Software_Devel/64   3/15/11 11:03a rgreen
 * SW7425-170: Update GCP Mode setting for 742x chips; Add more
 * descriptive macro name BHDR_CONFIG_GCP_LEGACY_MODE; keep older name
 * BHDR_CONFIG_GCP_MODE1 for backwards compatibilty
 * 
 * Hydra_Software_Devel/63   3/15/11 1:24a hongtaoz
 * SW7425-170: fix A1 compile error;
 * 
 * Hydra_Software_Devel/62   3/7/11 5:47p rgreen
 * SWDTV-5816,SW7422-230: Merge changes from 35230 branch; add compilation
 * option for on-chip CEC support
 * 
 * Hydra_Software_Devel/61   3/3/11 11:48a rgreen
 * SWDTV-4664: Update HDCP Key Set verification and HDCP processing for
 * OTP ROM based chips
 * 
 * Hydra_Software_Devel/60   2/3/11 11:18a rgreen
 * SW35230-2882: Restore support for parsing Gamut Meta Data packet;
 * Support is disabled by default (see bhdr_config.h to enable)   Fix
 * naming convention for isr functions
 * 
 * Hydra_Software_Devel/59   1/31/11 7:35p rgreen
 * SW7422-129: Implement HDCP Rx Key Loading to Secure OTP RAM
 * 
 * Hydra_Software_Devel/58   1/18/11 2:02p rgreen
 * SW35230-568: Update GCP Mode setting for  3548/56
 * 
 * Hydra_Software_Devel/57   1/14/11 5:07p rgreen
 * SW35230-568: Update GCP Mode setting for  3548/56
 * 
 * Hydra_Software_Devel/56   1/13/11 1:42p rgreen
 * SW35230-568:Update GCP Mode setting for additional receiver chips
 * 
 * Hydra_Software_Devel/55   12/20/10 6:33p rgreen
 * SWBLURAY-23819: Add 7640 HDR Support
 * 
 * Hydra_Software_Devel/54   12/8/10 12:50p rgreen
 * SW35230-2374,SW7422-129: Add support for HDCP Key loading and
 * verfication
 * 
 * Hydra_Software_Devel/53   12/7/10 7:28p rgreen
 * SW35230-2416:  Enable HDCP Key Check and Symbol Loss reset for 35230
 * Only
 * 
 * Hydra_Software_Devel/52   12/7/10 4:59p rgreen
 * SW35230-2416: Add timer support to delay reset of front end;
 * 
 * Hydra_Software_Devel/51   12/3/10 9:07p rgreen
 * SW35230-2374: Add support for HDCP Key verfication
 * 
 * Hydra_Software_Devel/50   11/9/10 10:51a rgreen
 * SW35125-12: Add support for 35125
 * 
 * Hydra_Software_Devel/49   10/29/10 6:48p rgreen
 * SW7425-21,SW7422-69:Temporarily disable HDMI Rx CEC for 742x platforms
 * 
 * Hydra_Software_Devel/48   10/19/10 6:25p rgreen
 * SW7425-21,SW7422-69:Update HDMI Rx Config for 742x
 * 
 * Hydra_Software_Devel/47   10/18/10 5:03p rgreen
 * SW35230-1598:Reset HDCP on 35230 only
 * 
 * Hydra_Software_Devel/46   10/1/10 3:22p rgreen
 * SW35230-1522: Add C0 support
 * 
 * Hydra_Software_Devel/45   9/23/10 5:17p rgreen
 * SW7422-69: Add 7422 Support
 * 
 * Hydra_Software_Devel/44   9/21/10 10:43a rgreen
 * SW35230-1179,SW3548-2013:  Update reporting of General Control Packet:
 * Color Mode and AvMute settings
 * 
 * Hydra_Software_Devel/43   9/15/10 5:22p rgreen
 * SW35230-1363,SW7425-21: Move chip/phy specific code to private file
 * 
 * Hydra_Software_Devel/42   9/10/10 2:45p rgreen
 * SW35230-1193: Disable DC Phase Packing
 * 
 * Hydra_Software_Devel/41   8/30/10 3:03p rgreen
 * SW7425-21: Add support for 7422/7425
 * 
 * Hydra_Software_Devel/40   8/27/10 6:56p rgreen
 * SW35230-802: Respond to channel status update interrupts to correctly
 * configure MAI bus
 * 
 * Hydra_Software_Devel/39   7/29/10 3:11p rgreen
 * SW35230-807,SW35230-810, SW35230-736: Do not process HDR interrupts
 * until PLL Lock timer has expired
 * 
 * Hydra_Software_Devel/38   7/28/10 3:59p rgreen
 * SW35230-802: Use channel status bits to update MAI bus audio format
 * type
 * 
 * Hydra_Software_Devel/37   7/27/10 8:55p rgreen
 * SW35230-568:   Add option to override analog calibration; Add option to
 * select GCP Mode
 * 
 * Hydra_Software_Devel/36   7/22/10 4:54p rgreen
 * SW35230-422,SW3548-1540: Enable hw retrieval of the channel status bits
 * on the 35230 Copy the calculated Sampling Frequency to the Hdmi Rx
 * status
 * 
 * Hydra_Software_Devel/35   7/21/10 7:02p rgreen
 * SW35230-568: Improve front end auto frequency range detection
 * 
 * Hydra_Software_Devel/34   7/6/10 4:25p rgreen
 * SW35230-502: Enable default pixel packing
 * 
 * Hydra_Software_Devel/33   7/1/10 7:06p rgreen
 * SW35230-451: Update frontend range detection
 * 
 * Hydra_Software_Devel/32   6/7/10 5:27p rgreen
 * SW35230-128: Separate phy specific code to separate file
 * 
 * Hydra_Software_Devel/31   1/22/10 2:22p rgreen
 * SW3548-2013: Add  BHDR_CONFIG_DEBUG_DISPLAY_FE_CONFIG macro to show FE
 * configuration for debugging.
 * Use WRN instead of ERR for special HDR FE configurations
 * 
 * Hydra_Software_Devel/30   1/13/10 6:07p rgreen
 * SW3548-2013: Add catch all macro (BHDR_CONFIG_PLUGFEST_TEST)  for
 * Plugfest
 * 
 * Move BHDR_CONFIG_SYMBOL_LOSS_SM to configurable section
 * 
 * Hydra_Software_Devel/29   11/25/09 1:15p rgreen
 * SW35230-15: Add support for 35230
 * 
 * Hydra_Software_Devel/28   9/29/09 5:42p rgreen
 * SW3548-2013: Add option to enable/disable AUDIO_TYPE debug messages.
 * 
 * Hydra_Software_Devel/27   8/31/09 2:59p rgreen
 * SW3548-2013: Disable DEBUG_INPUT_SIGNAL macro
 * 
 * Hydra_Software_Devel/26   8/20/09 2:35p rgreen
 * PR54000,PR57890:
 * Add debug option for ACR packets
 * Add option for Symbol Loss SM debugging
 * 
 * Hydra_Software_Devel/25   8/13/09 6:01p rgreen
 * PR57565:Update settings for disabling equalization
 * 
 * Hydra_Software_Devel/24   8/12/09 5:32p rgreen
 * PR54387: Eliminate excessive messages for Audio SR debugging
 * 
 * Hydra_Software_Devel/23   5/1/09 4:17p rgreen
 * PR54000: Move debug functions to separate bhdr_debug.c file.
 * Add options to dump Infoframes, monitor format changes, monitor values
 * etc
 * 
 * Hydra_Software_Devel/22   4/8/09 10:03a rgreen
 * PR54000: Add debug information for AVI Infoframe
 * 
 * Hydra_Software_Devel/21   3/24/09 6:56p rgreen
 * PR53557: Disable Auto Clear AvMute.  Add option to mute audio
 * immediately from HDMI Rx core
 * 
 * Hydra_Software_Devel/20   3/19/09 10:44a rgreen
 * PR53396: Add BHDR_CONFIG_DEBUG_INPUT_SIGNAL macro to enable debug
 * messages.  Disable by default
 * 
 * Hydra_Software_Devel/19   3/6/09 10:32a rgreen
 * PR52879: Add compilation/debug support for on-chip EDID RAM
 * 
 * Hydra_Software_Devel/18   2/26/09 7:56p rgreen
 * PR51307: Add compile time option (BHDR_HDCP_REPEATER_SUPPORT) for HDCP
 * Repeater
 * 
 * Hydra_Software_Devel/17   2/25/09 10:35a rgreen
 * PR44505: Update AvMute auto clear functionality
 * 
 * Hydra_Software_Devel/16   2/9/09 3:54p rgreen
 * PR51752,PR51898:
 * Add debug support to disable AvMute Callback
 * 
 * Hydra_Software_Devel/15   1/26/09 5:27p rgreen
 * PR50940: Mute audio until N, CTS values are stable
 * 
 * Hydra_Software_Devel/14   1/19/09 9:10p rgreen
 * PR48877: change default behavior for hot plug configuration
 * 
 * Hydra_Software_Devel/13   12/12/08 5:00p rgreen
 * PR50335: Enable the internal pull up for proper HDMI Hot Plug operation
 * for the B1 reference board
 * 
 * Hydra_Software_Devel/PR50335/1   12/12/08 3:11p honglang
 * PR50335: Enable the internal pull up for proper HDMI Hot Plug operation
 * for the B1 reference board
 * 
 * Hydra_Software_Devel/12   12/9/08 11:24a rgreen
 * PR49943: Add support for i2c pullup configuration for B0 only
 * 
 * Hydra_Software_Devel/11   12/1/08 10:01p rgreen
 * PR48877: Set default for Hot Plug hw modification to "not installed";
 * User can configure as needed
 * 
 * Hydra_Software_Devel/10   11/14/08 3:59p rgreen
 * PR48775: PR48775: use N and CTS to calculate audio sample rate
 * 
 * Hydra_Software_Devel/9   11/12/08 3:58p rgreen
 * PR48877: Hot Plug pad configuration will not work unless the board has
 * been updated
 * 
 * Hydra_Software_Devel/8   11/11/08 5:52p rgreen
 * PR48741: Modify loop filter settings for B0 only; add as configuration
 * value
 * 
 * Hydra_Software_Devel/7   11/11/08 4:32p rgreen
 * PR48877:Hot Plug pad configuration available on B0 only
 * 
 * Hydra_Software_Devel/PR48775/1   11/10/08 4:50p honglang
 * PR48775: use N and CTS to calculate audio sample rate
 * 
 * Hydra_Software_Devel/6   10/31/08 2:06p rgreen
 * PR48444,PR46509: Update RDB setting for Channel Status config
 * Fix DCP Compliance test issue by switching back to DVI mode on loss of
 * HDMI stream
 * Add B0 debug interrupts
 * 
 * Hydra_Software_Devel/5   10/16/08 5:21p rgreen
 * PR47954,PR47970,PR48007:
 * Add option for EDID RAM
 * Add option adaptive equalization;
 * Add option for Initialize Packet RAM
 * 
 * Hydra_Software_Devel/4   10/9/08 4:40p rgreen
 * PR43214: Merge changes to support debug eye diagram
 * 
 * Hydra_Software_Devel/3   9/11/08 6:47p rgreen
 * PR45358,PR44257: Remove BHDR_CEC_SUPPORT defines/macros.
 * 
 * Hydra_Software_Devel/2   7/10/08 11:52a rgreen
 * PR44523: Add option to debug Vendor Specific Infoframe
 * Remove Force RGB if no AVI Infoframe workaround
 * 
 * Hydra_Software_Devel/PR44523/1   7/8/08 4:36p rgreen
 * PR44523: Add option to debug Vendor Specific Infoframe
 * Remove Force RGB if no AVI Infoframe workaround
 * 
 * Hydra_Software_Devel/1   5/20/08 2:41p rgreen
 * PR38947: Create separate config file for 3548/56
 * 
 ***************************************************************************/

#ifndef BHDR_CONFIG_H__
#define BHDR_CONFIG_H__

#ifdef __cplusplus
extern "C" {
#endif


#include "bchp.h"


/* 
The Audio Clock Recovery Packet (ACRP) can be used to calculate the audio 
sample rate using N and CTS.  While this method can be used to determine 
the rate, the preferred method is to use the Channel Status bits extracted 
directly from the stream by the hardware.

Setting the define
	BHDR_CONFIG_USE_ACRP_EVERY_N_VSYNCS
	
will use the calculation. A nominal setting of 3 can be used.  The nominal setting
can also be used to debug the contents of the ACR Packet.   The N, CTS values 
will print at each sample rate change.

If the value is set to zero (default), the preferred method of extracting channel 
status bits directly from the stream will be used.
*/
#if ((BCHP_CHIP == 3548) || (BCHP_CHIP == 3556)) 
#define BHDR_CONFIG_USE_ACRP_EVERY_N_VSYNCS 3
#endif
#define BHDR_CONFIG_CONSECUTIVE_SR_CALCS 3


/* 
Enable BHDR_CONFIG_CLEAR_AVMUTE_AFTER_N_S option 
if a source device does not properly send Clear_AVMUTE after sending Set_AVMUTE  

If set > 0, AvMUTE will automatically clear after N seconds
If set = 0. the Auto Clear AvMute feature will be disabled 
NOTE:  Auto Clear AvMute may be a requirement for Simplay Certification
*/
#define BHDR_CONFIG_CLEAR_AVMUTE_AFTER_N_S 0


/* 
Enable BHDR_CONFIG_AVMUTE_AUDIO_IMMEDIATELY option 
to have the HDMI Core immediately disable sending audio packets to the audio core.

the alternative is to have the app handle audio and video muting simultaneously 
*/
#define BHDR_CONFIG_AVMUTE_AUDIO_IMMEDIATELY 1 

/* 
Enable BHDR_CONFIG_DEBUG_DISABLE_AVMUTE_CB option 
to disable AVMUTE Callback... can be used to check output display
of the screen when AvMute is not processed
*/
#define BHDR_CONFIG_DEBUG_DISABLE_AVMUTE_CB 0


/* 
Enable BHDR_CONFIG_SYMBOL_LOSS_SM option 
to enable state machine processing of the input signal.  
This option should be enabled when a N:1 switch is used in front  of the HDMI Rx
It should not be enabled for 35230
*/
#if ((BCHP_CHIP == 3548) || (BCHP_CHIP == 3556)) 
#define BHDR_CONFIG_SYMBOL_LOSS_SM 1
#endif



/* HOT PLUG DETECT SIGNAL */

/*
** Set  BHDR_CONFIG_HPD_DISCONNECTED  to 1 
** ONLY if board does not route the HPD signal from Pin 19 of the HDMI Connector directly
** to the HPD In of HDMI Rx
**
** Broadcom SV boards usually have this signal connected (default 0)
** Broadcom CARD boards may not have the signal connected.  If not connected set to 1
*/
#define BHDR_CONFIG_HPD_DISCONNECTED 0

#if ((BCHP_CHIP == 3548) || (BCHP_CHIP == 3556)) && (BCHP_VER >= BCHP_VER_B0)

/* HOT PLUG SETTINGS */
 
/* 
** Set  BHDR_CONFIG_HOTPLUG_PAD to 1 
** ONLY if board (see reference design) has the Hot Plug ECO applied as follows 
**      (1) Remove R1545 and R1547 (0 ohms).
**      (2) Change R1524 and R1529 with 0 ohms.
**
** It is expected that all new boards (BCM93549V10_01) with B1 chip have been 
** modified as above.  Older boards which do not have the mod should use 0
*/
#if (BCHP_VER >= BCHP_VER_B1) 
#define BHDR_CONFIG_HOTPLUG_PAD 1
#else
#define BHDR_CONFIG_HOTPLUG_PAD 0
#endif


/* DIGITAL LOOP FILTER SETTINGS */

/* NORMAL reference board; normal digital loop filter config settings; s*/
#define BHDR_CONFIG_LOOP_FILTER_PD_HIGH_THRESHOLD 0x4
#define BHDR_CONFIG_LOOP_FILTER_PD_LOW_THRESHOLD 0x7C

/* EXTERNAL SWITCH on board; digital loop filter config settings for use with external switch  */		
#define BHDR_CONFIG_LOOP_FILTER_PD_HIGH_THRESHOLD_WITH_SWITCH 0x8
#define BHDR_CONFIG_LOOP_FILTER_PD_LOW_THRESHOLD_WITH_SWITCH 0x0

#define BHDR_CONFIG_EQ_INPUT_CLK_SAMPLE_NEG 0x1

/* do not modify this setting */
#define BHDR_CONFIG_LOOP_FILTER_SETTINGS 1

#endif 


/* BHDR_CONFIG_DELAY_MODE_CHANGE_MS
    Delay processing of switch between HDMI and DVI mode 
    multiple mode interrupts may occur during transitions; 
    delay re-configuration until mode is stable for the specified time
    set to 0 to handle MODE switch immediately
*/
#define BHDR_CONFIG_DELAY_MODE_CHANGE_MS 100
#define BHDR_CONFIG_DEBUG_MODE_CHANGE 0



/* Enable DEBUG_TIMER_S  to fire BHDR_P_DebugMonitorHdmiRx_isr 
every N seconds.  function can be modified to read registers etc.
DEBUG ONLY!!
*/
#define BHDR_CONFIG_DEBUG_TIMER_S 0

/* available debug options; enables BDBG_MSGs etc.  */
#define BHDR_CONFIG_DEBUG_I2C 0



/* Enable BHDR_CONFIG_ISRC_PACKET_SUPPORT option 
to support processing of GAMUT Metadata packets

Gamut Packet Processing is not recommended 
*/
#define BHDR_CONFIG_GAMUT_PACKET_SUPPORT 0
#define BHDR_CONFIG_DEBUG_GAMUT_PACKET 0


/* 
Enable BHDR_CONFIG_ISRC_PACKET_SUPPORT option 
to support processing of ISRC packets. 

NOTE:  No commercial devices have been found that support ISRC packets
therefore this option has not been adequately tested
*/
#define BHDR_CONFIG_ISRC_PACKET_SUPPORT 0


/* HDMI Packet Debug Options  */
#define BHDR_CONFIG_DEBUG_INFO_PACKET_AVI       0
#define BHDR_CONFIG_DEBUG_INFO_PACKET_AUDIO   0
#define BHDR_CONFIG_DEBUG_INFO_PACKET_SPD       0
#define BHDR_CONFIG_DEBUG_INFO_PACKET_VENDOR_SPECIFIC     0
#define BHDR_CONFIG_DEBUG_ACR_PACKET 0 
#define BHDR_CONFIG_DEBUG_CHANNEL_STATUS 0

/* HDMI General Control Packet Debug Options  */
#define BHDR_CONFIG_DEBUG_GCP_DC 0
#define BHDR_CONFIG_DEBUG_GCP_AV_MUTE 0

/* HDMI Audio Format change from Normal to High bitrate audio */
#define BHDR_CONFIG_DEBUG_AUDIO_PACKET_CHANGE 0

/* HDMI Audio FORMAT from Channel Status Bits */
#define BHDR_CONFIG_DEBUG_AUDIO_FORMAT 0

/* HDCP Debug Options */
#define BHDR_CONFIG_DEBUG_HDCP_VALUES 0
#define BHDR_CONFIG_DEBUG_HDCP_KEY_LOADING 0

/* debug option to track RI updating; dependent upon working  */
#define BHDR_CONFIG_DEBUG_HDCP_RI_UPDATES 0

/* debug option to generate eye diagram; always built with kylin */
#if KYLIN
#define BHDR_CONFIG_DEBUG_EYE_DIAGRAM 1 
#endif

#define BHDR_CONFIG_DEBUG_DETECTED_FORMAT_SUMMARY 0
#define BHDR_CONFIG_DEBUG_DETECTED_FORMAT_DETAIL 0

/* debug optiion to verify EDID ram written correctly */
#define BHDR_CONFIG_DEBUG_EDID_RAM 0

#define BHDR_CONFIG_DEBUG_STREAM_ERRORS 0
#define BHDR_CONFIG_DEBUG_AUDIO_SR 0

/* debug option to display configuration of HP, Equalizer, etc. */
#define BHDR_CONFIG_DEBUG_DISPLAY_HDMI_RX_CONFIG 0
#define BHDR_CONFIG_DEBUG_DISPLAY_FE_CONFIG 0
#define BHDR_CONFIG_DEBUG_INPUT_SIGNAL 0
#define BHDR_CONFIG_DEBUG_FRONT_END 0

/* debug option to display HDMI Rx power management configuration */
#define BHDR_CONFIG_DEBUG_HDR_PWR 0

#define BHDR_CONFIG_DEBUG_OLD_TIMER_DEPRACATED 0


/******************************************************************/
/* Plugfest Configuration START                                      */
/******************************************************************/
#define BHDR_CONFIG_PLUGFEST_TEST 0

#if BHDR_CONFIG_PLUGFEST_TEST

#undef BHDR_CONFIG_DEBUG_HDCP_RI_UPDATES
#define BHDR_CONFIG_DEBUG_HDCP_RI_UPDATES 1

#undef BHDR_CONFIG_DEBUG_INFO_PACKET_AVI        
#undef BHDR_CONFIG_DEBUG_INFO_PACKET_AUDIO   
#undef BHDR_CONFIG_DEBUG_INFO_PACKET_SPD       
#undef BHDR_CONFIG_DEBUG_INFO_PACKET_VENDOR_SPECIFIC     
#define BHDR_CONFIG_DEBUG_INFO_PACKET_AVI        1
#define BHDR_CONFIG_DEBUG_INFO_PACKET_AUDIO   1
#define BHDR_CONFIG_DEBUG_INFO_PACKET_SPD       1
#define BHDR_CONFIG_DEBUG_INFO_PACKET_VENDOR_SPECIFIC     1

#undef BHDR_CONFIG_DEBUG_INPUT_SIGNAL 
#undef BHDR_CONFIG_DEBUG_FRONT_END 
#define BHDR_CONFIG_DEBUG_INPUT_SIGNAL 1
#define BHDR_CONFIG_DEBUG_FRONT_END 1

#undef BHDR_CONFIG_DEBUG_DETECTED_FORMAT_SUMMARY 
#define BHDR_CONFIG_DEBUG_DETECTED_FORMAT_SUMMARY 1


#undef BHDR_CONFIG_DEBUG_AUDIO_SR 
#define BHDR_CONFIG_DEBUG_AUDIO_SR 1

#endif




/******************************************************************/
/******************************************************************/
/******************************************************************/
/* chip revision specific configuration - DO NOT MODIFY ANY BELOW */
/******************************************************************/
/******************************************************************/
/******************************************************************/

#if ((BCHP_CHIP == 3548) || (BCHP_CHIP == 3556)) 
#define HDMI_RX_GEN 3548

#elif ((BCHP_CHIP == 35230) || (BCHP_CHIP == 35130) \
   || (BCHP_CHIP == 35125) || (BCHP_CHIP == 35233))
#define HDMI_RX_GEN 35230

#elif (BCHP_CHIP == 7422) || (BCHP_CHIP == 7425) \
	|| (BCHP_CHIP == 7640) || (BCHP_CHIP == 7429)  || (BCHP_CHIP == 7435)
#define HDMI_RX_GEN 7422

#elif (BCHP_CHIP == 7445)
#define HDMI_RX_GEN 7445

#else
#error UNKNOWN_CHIP
#endif



/*********** Basic HDMI Rx Configuration ***********/ 
#if HDMI_RX_GEN == 3548

#define BHDR_FE_MAX_CHANNELS 2
#define BHDR_HAS_MULTIPLE_PORTS 2
#define BHDR_CONFIG_RESET_HDCP_ON_SYMBOL_LOCK	0
#define BHDR_CONFIG_MASTER_SLAVE_3D_SUPPORT 1

#elif HDMI_RX_GEN == 35230

#define BHDR_FE_MAX_CHANNELS 1
#define BHDR_HAS_MULTIPLE_PORTS 0
#define BHDR_CONFIG_RESET_HDCP_ON_SYMBOL_LOCK	1
#define BHDR_CONFIG_UPDATE_MAI_ON_VSYNC 1


#elif HDMI_RX_GEN == 7422

#define BHDR_FE_MAX_CHANNELS 1
#define BHDR_HAS_MULTIPLE_PORTS 0
#define BHDR_CONFIG_RESET_HDCP_ON_SYMBOL_LOCK	1
#define BHDR_CONFIG_UPDATE_MAI_ON_VSYNC 1

#define BHDR_CONFIG_REPEATER 1 

#if ((BCHP_CHIP == 7425) || (BCHP__CHIP == 7422) || (BCHP_CHIP == 7640)) \
&& (BCHP_VER < BCHP_VER_B0)

#define BHDR_FE_HP_LEGACY_SUPPORT 1

#endif


#if ((BCHP_CHIP == 7425) && (BCHP_VER >= BCHP_VER_B0)) \
	|| (BCHP_CHIP == 7429) || (BCHP_CHIP == 7435) 
#define BHDR_CONFIG_HW_PASSTHROUGH_SUPPORT 1
#define BHDR_CONFIG_FE_MULTI_CLOCK_SUPPORT 1
#endif


#if (BCHP_CHIP == 7435) \
	|| ((BCHP_CHIP == 7429) && (BCHP_VER >= BCHP_VER_B0))
#define BHDR_CONFIG_DUAL_HPD_SUPPORT 1
#endif


#define BHDR_CONFIG_HBR_SUPPORT 1

#elif HDMI_RX_GEN == 7445

#define BHDR_FE_MAX_CHANNELS 1
#define BHDR_HAS_MULTIPLE_PORTS 0
#define BHDR_CONFIG_RESET_HDCP_ON_SYMBOL_LOCK	1
#define BHDR_CONFIG_UPDATE_MAI_ON_VSYNC 1

#define BHDR_CONFIG_REPEATER 1 

#define BHDR_CONFIG_DUAL_HPD_SUPPORT 1

#define BHDR_CONFIG_FE_MULTI_CLOCK_SUPPORT 1

#else
#error Unknown/Unsupported chip
#endif

#if (HDMI_RX_GEN == 3548) && (BCHP_VER >= BCHP_VER_B0)
#define BHDR_CONFIG_ENABLE_ADAPTIVE_EQUALIZATION 1
#define BHDR_CONFIG_FAST_PACKET_INIT 1
#define BHDR_CONFIG_DISABLE_EDID_RAM 1
#define BHDR_CONFIG_I2C_PAD_CONTROL 1
#endif


#if (HDMI_RX_GEN == 3548)
#define BHDR_CONFIG_HW_CHANNEL_STATUS 0
#define BHDR_CONFIG_FORCE_MAI_BUS_FORMAT 1
#else
#define BHDR_CONFIG_HW_CHANNEL_STATUS 1
#endif



#if (HDMI_RX_GEN == 35230)
#define BHDR_CONFIG_DC_PACKING_PHASE_ENABLE 0
#define BHDR_CONFIG_REENABLE_INTRS 1

#if BCHP_CHIP == 35230
#define BHDR_CONFIG_DVP_HR_TMR 1
#endif
#endif


/*********** GCP Packet Handling ***********/ 


#if (BCHP_CHIP == 35230) \
||  (BCHP_CHIP == 35125) \
|| ((BCHP_CHIP == 7422) && (BCHP_VER < BCHP_VER_B0)) \
|| ((BCHP_CHIP == 7425) && (BCHP_VER < BCHP_VER_B0)) \
|| ((BCHP_CHIP == 7640) && (BCHP_VER < BCHP_VER_B0)) \
||  (BCHP_CHIP == 35233)
/* new clear macro name */
#define BHDR_CONFIG_GCP_LEGACY_MODE 1

/* keep prior (unclear) name for backward compatibility */
#define BHDR_CONFIG_GCP_MODE1 1

#elif (HDMI_RX_GEN == 3548) || HDMI_RX_GEN == 7445 \
|| ((BCHP_CHIP == 7425) && (BCHP_VER >= BCHP_VER_B0)) \
|| ((BCHP_CHIP == 7429) || (BCHP_CHIP == 7435) ) 

/* not used */

#else
#error Update GCP Mode Configuration Setting
#endif



/*********** HDCP Configuration ***********/ 

/* used only when HDMI Rx is used as a repeater */
#define BHDR_CONFIG_HDCP_REPEATER_MAX_KSV_FIFO 15

#if (HDMI_RX_GEN == 3548) || (HDMI_RX_GEN == 35230)

#define BHDR_CONFIG_HDCP_KEY_OTP_ROM 1

#elif (HDMI_RX_GEN == 7422) || (HDMI_RX_GEN == 7445)

#define BHDR_CONFIG_HDCP_KEY_OTP_RAM 1

#else
/* need to add other supported chips */
#error Specify HDCP Type for new chip
#endif



/**********************************/
#if (BCHP_CHIP == 35230) 

#define BHDR_CONFIG_RESET_FE_ON_SYMBOL_LOSS 1

#if (BCHP_VER <= BCHP_VER_B0)
#define BHDR_CONFIG_OVERRIDE_ANALOG_CALIBRATION 1
#define BHDR_CONFIG_LOOKUP_MANUAL_PDIV_SETTING 1
#else
#define BHDR_CONFIG_OVERRIDE_ANALOG_CALIBRATION 0
#endif

#if (BCHP_VER <= BCHP_VER_C0)
#define BHDR_CONFIG_MANUAL_PLL_RANGE_SETTING 1
#endif

#else

/* all other chips */
#define BHDR_CONFIG_OVERRIDE_ANALOG_CALIBRATION 0
#endif


/**********************************/
#if (BCHP_CHIP == 35125) 
#define BHDR_CONFIG_MANUAL_PLL_RANGE_SETTING 1
#endif



/* Frequency range detect config. */
#if (BHDR_CONFIG_MANUAL_PLL_RANGE_SETTING \
||  ((BCHP_CHIP == 35233) && (BCHP_VER == BCHP_VER_A0)))
#define BHDR_CONFIG_FREQUENCY_RANGE_DETECTION 1
#else
#define BHDR_CONFIG_FREQUENCY_RANGE_DETECTION 0
#endif


#if (HDMI_RX_GEN == 35230)

#if (BCHP_CHIP == 35230) || (BCHP_CHIP == 35125)
#define BHDR_CONFIG_MANUAL_FREQUENCY_DETECTION 1

#elif (BCHP_CHIP == 35233)
#define BHDR_CONFIG_MANUAL_FREQUENCY_DETECTION 0

#else

#error Missing Chip ID for frequency Detection
#endif


#endif


/* New "Format Detect Input" select and "Loop 
 * Filter Control" config in the 35233 FE. */
#if BCHP_CHIP == 35233
#define BHDR_CONFIG_FORMAT_DETECT_INPUT 1
#define BHDR_CONFIG_LOOP_FILTER_CONTROL 1
#else
#define BHDR_CONFIG_FORMAT_DETECT_INPUT 0
#define BHDR_CONFIG_LOOP_FILTER_CONTROL 0
#endif

/* We don't need to reset the pixel clock estimation and clock
 * and data channels whenever PLL locks or unlocks in 35233. */

#if ((HDMI_RX_GEN == 3548) || ((BCHP_CHIP == 7425) && (BCHP_VER < BCHP_VER_B0)))
#define BHDR_CONFIG_RESET_PIXEL_CLOCK_ESTIMATION  1
#define BHDR_CONFIG_RESET_CLOCK_AND_DATA_CHANNELS 1
#else
#define BHDR_CONFIG_RESET_PIXEL_CLOCK_ESTIMATION  0
#define BHDR_CONFIG_RESET_CLOCK_AND_DATA_CHANNELS 0
#endif

#if BCHP_CHIP == 35233
#define BHDR_CONFIG_DEBUG_DETECTED_FORMAT_SUMMARY_WITH_VSYNC_RATE 0
#endif

#ifdef __cplusplus
}
#endif


 
#endif
/* end bhdr_config.h */

