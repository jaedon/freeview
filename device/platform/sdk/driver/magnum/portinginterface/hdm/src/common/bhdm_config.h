/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bhdm_config.h $
 * $brcm_Revision: Hydra_Software_Devel/110 $
 * $brcm_Date: 9/20/12 3:26p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/hdm/src/common/bhdm_config.h $
 * 
 * Hydra_Software_Devel/121   8/6/13 6:00p rgreen
 * SW7429-710,SW7563-123: Implement channel swaps only for chips that
 * require it
 * 
 * Hydra_Software_Devel/120   8/2/13 2:20p rgreen
 * SW7429-710: Some 40nm chips require swapping of the HDMI_TX_PHY channel
 * settings
 * 
 * Hydra_Software_Devel/110   9/20/12 3:26p vle
 * SW7445-23: Add HDM PI for 7445
 * 
 * Hydra_Software_Devel/109   9/11/12 4:15p rgreen
 * SW7435-306: Make the compilation of the new analog alternative BFMT
 * formats configurable.  Will allow compilation of latest HDM with older
 * reference code.  By default the formats are enabled
 * 
 * Hydra_Software_Devel/108   8/15/12 2:56p vle
 * SW7445-23: Add HDM PI support for 7445
 * 
 * Hydra_Software_Devel/107   8/9/12 10:05a rgreen
 * SW7425-2989: Specify 65nm platforms that support format detection.
 * 
 * Hydra_Software_Devel/106   8/8/12 11:04a cng
 * SW7425-2989: Enable Format Detection monitor for 65nm platforms
 * 
 * Hydra_Software_Devel/105   7/26/12 10:07a rgreen
 * SW7563-12: HDM Support for 7563
 * 
 * Hydra_Software_Devel/104   7/18/12 5:19p rgreen
 * SW7425-2989: Update BHDM_CONFIG_BTMR_SUPPORT usage
 * 
 * Hydra_Software_Devel/103   7/16/12 2:48p rgreen
 * SW7425-2989: Format Change Detection available in 40nm and newer chips.
 * Disable for earlier chips
 * 
 * Hydra_Software_Devel/102   7/12/12 4:45p rgreen
 * SW7425-2989: Merge changes
 * 
 * Hydra_Software_Devel/SW7425-2989/2   7/11/12 6:17p rgreen
 * SW7425-2989: Prevent initial debug warning after format change
 * 
 * Hydra_Software_Devel/SW7425-2989/1   7/6/12 2:16p rgreen
 * SW7425-2989: Add debug monitor to report format changes to HDMI Tx core
 * 
 * Hydra_Software_Devel/101   7/3/12 5:46p vle
 * SW7429-187: Add support for 7429 B0
 * 
 * Hydra_Software_Devel/100   6/20/12 2:35p rgreen
 * SW7358-263, SW7425-3206: Use polling to detect RxSense state instead of
 * interrupts
 * 
 * Hydra_Software_Devel/99   5/22/12 8:18p rgreen
 * SW7335-1383,SW7425-2075: Fix MAI Channel ordering for 65nm. Move MAI
 * Channel Ordering to isr time when notified of audio changes
 * 
 * Hydra_Software_Devel/98   5/15/12 4:34p vle
 * SW7584-12: add support for 7584
 * 
 * Hydra_Software_Devel/97   4/18/12 5:13p vle
 * SW7346-737: Fix build errors for 65nm platforms due to slight
 * differences in CRC register names
 * 
 * Hydra_Software_Devel/96   4/16/12 2:48p vle
 * SW7346-737: Merge to mainline
 * 
 * Hydra_Software_Devel/SW7346-737/1   4/12/12 4:22p vle
 * SW7346-737: Initial attempt adding CRC support in HDM
 * 
 * Hydra_Software_Devel/95   3/28/12 2:55p rgreen
 * SW7405-5557: Remove duplicate EDID debug option already contained in
 * bhdm_edid.c
 * 
 * Hydra_Software_Devel/94   3/9/12 6:52p vle
 * SW7435-44: Update HDMI_TX_PHY channel swap settings for all 40nm
 * platforms. Future platforms relies on the default values
 * 
 * Hydra_Software_Devel/93   3/1/12 11:29a vle
 * SW7435-44: Update HDMI_TX_PHY channel_swap settings for 7435
 * 
 * Hydra_Software_Devel/92   2/13/12 11:39a vle
 * SW7360-9: Add dual HPD support for 7360
 * 
 * Hydra_Software_Devel/91   1/20/12 4:29p vle
 * SW7360-9: Add support for 7360
 * 
 * Hydra_Software_Devel/90   1/19/12 2:34p rgreen
 * SW7125-1146: Enable TMDS at open to fix interrupt issue; Get RSEN
 * setting at isr vs event time; Add debug configurations for RSEN amd
 * TMDS settings
 * 
 * Hydra_Software_Devel/89   1/6/12 2:59p vle
 * SW7435-11: Add support for 7435
 * 
 * Hydra_Software_Devel/88   1/4/12 3:31p rgreen
 * SW7405-5004: Add Source Product Description (SPD) debug option
 * 
 * Hydra_Software_Devel/87   12/12/11 2:12p rgreen
 * SW7552-157: Fix 7552 build; remove references to unused
 * BHDM_CONFIG_40NM_REV2_SUPPORT
 * 
 * Hydra_Software_Devel/86   11/23/11 1:17p rgreen
 * SW7425-1140: Merge to mainline
 * 
 * Hydra_Software_Devel/SW7425-1140/1   11/22/11 5:48p vle
 * SW7425-1140: Add BHDM_CONFIG_CEC_LEGACY_SUPPORT for backward compatible
 * for CEC legacy platforms.
 * 
 * Hydra_Software_Devel/85   11/14/11 3:33p rgreen
 * SW7425-1710: Update BHDM_CONFIG macro usage.  Describe specific
 * functionality vs chip process
 * 
 * Hydra_Software_Devel/84   11/14/11 2:15p rgreen
 * SW7425-1710: Update BHDM_CONFIG macro usage.  Describe specific
 * functionality vs chip process
 * 
 * Hydra_Software_Devel/83   11/4/11 4:08p rgreen
 * SW7125-1128,SW7125-1138:  Use register based General Control Packet
 * only when attached receiver supports Deep Color
 * 
 * Hydra_Software_Devel/82   10/28/11 2:26p mward
 * SW7435-11: Add 7435.
 * 
 * Hydra_Software_Devel/81   10/12/11 3:30p vle
 * SW7429-5: Add support for 7429 - Take 2
 * 
 * Hydra_Software_Devel/80   10/11/11 4:50p vle
 * SW7429-5: Add support for 7429.
 * 
 * Hydra_Software_Devel/79   9/7/11 10:40a vle
 * SW7422-114: Only 40nm platforms should be using register based GCP for
 * AVMute
 * 
 * Hydra_Software_Devel/78   8/17/11 6:37p vle
 * SW7231-322: Add support for 7231B0
 * 
 * Hydra_Software_Devel/77   7/1/11 10:47a vle
 * SW7422-458: merge bdvd changes to mainline
 * 
 * Hydra_Software_Devel/bdvd_v4.0/1   6/28/11 2:13p rbshah
 * SWBLURAY-26245:[ see Broadcom Issue Tracking JIRA for more info ]
 * 
 * Hydra_Software_Devel/76   6/17/11 5:58p vle
 * SW7231-147: Tweak Pre-emphasis settings for some 40nm A0 platforms to
 * fix HDMI eye diagram issue in 1080p 8bit color depth - Take 2: For A0
 * only
 * 
 * Hydra_Software_Devel/75   6/17/11 1:42p vle
 * SW7405-5358, SW7405-5312: Separate 3D support from EDID parser to allow
 * easier back port
 * 
 * Hydra_Software_Devel/74   6/13/11 6:07p vle
 * SW7231-147: Tweak Pre-emphasis settings for some 40nm A0 platforms to
 * fix HDMI eye diagram issue in 1080p 8bit color depth
 * 
 * Hydra_Software_Devel/73   4/8/11 3:25p rgreen
 * SW7405-3994: Disable debug messages for HDMI 1.4a (VSI format messages)
 * 
 * Hydra_Software_Devel/72   2/10/11 7:43p vle
 * SW7550-672: Fix GCP packet handlings for 65nm platforms with HDMI 1.3
 * support.
 * 
 * Hydra_Software_Devel/71   12/27/10 3:18p pntruong
 * SW7231-10, SW7231-16: Bringup hdmi output.
 * 
 * Hydra_Software_Devel/71   12/27/10 3:17p pntruong
 * SW7231-10, SW7231-16: Bringup hdmi output.
 *
 * Hydra_Software_Devel/70   12/22/10 11:40a vle
 * SW7344-11: Configure HDMI_TX_PHY.CHANNEL_SWAP for 40nm platforms. Not
 * all chips can use the default values.
 *
 * Hydra_Software_Devel/69   12/21/10 4:47p vle
 * SWBLURAY-23688: Add support for 7640
 *
 * Hydra_Software_Devel/68   11/19/10 4:10p vle
 * SW7344-11: Add support for 7344 / 7346
 *
 * Hydra_Software_Devel/67   11/5/10 5:17p vle
 * SW7552-12: Add support for 7552
 *
 * Hydra_Software_Devel/66   11/5/10 3:32p vle
 * SW7231-10: Add support for 7231
 *
 * Hydra_Software_Devel/65   9/29/10 4:15p vle
 * SW7422-23: Add 7422 support
 *
 * Hydra_Software_Devel/64   9/28/10 7:19p vle
 * SW7422-23: Refactor HDMI code to isolate platform dependent code
 * furthermore. Add support for 7422 and other 40nm platforms.
 *
 * Hydra_Software_Devel/63   6/22/10 6:57p vle
 * SW7405-3994: Add support to parse all Shorthand and additional 3D
 * Timing/Structure support indication in HDMI 1.4a
 *
 * Hydra_Software_Devel/62   5/11/10 7:14p vle
 * SW7405-3994: Add VSDB debug config
 *
 * Hydra_Software_Devel/61   4/12/10 3:27p vle
 * SW7405-3997: Enable VSI debug message by default.
 *
 * Hydra_Software_Devel/60   2/23/10 12:49a vle
 * SW7420-579: Refactor HDMI PI.
 *
 * Hydra_Software_Devel/17   1/7/10 5:12p vle
 * SW7420-536: Add method to use RAM packets to generate GCP Packets for
 * various platforms.
 *
 * Hydra_Software_Devel/16   1/6/10 4:38p vle
 * SW3548-2670: Centralize all CEC timing configurations
 *
 * Hydra_Software_Devel/15   12/8/09 7:27p vle
 * SW7468-18: Add support for 7208
 *
 * Hydra_Software_Devel/14   11/20/09 11:51a vle
 * SW7408-14, SW7468-18: Add support for 7408 and 7468
 *
 * Hydra_Software_Devel/13   9/11/09 11:26a vle
 * SW7550-12: Add support for 7550
 *
 * Hydra_Software_Devel/12   9/1/09 2:00p vle
 * SW7630-17: Add support for Grain 7630
 *
 * Hydra_Software_Devel/11   8/26/09 9:17p vle
 * SW7125-9: Add support for 7125
 *
 * Hydra_Software_Devel/10   8/4/09 4:36p vle
 * PR55220, PR55228: Add support for 7340, 7342
 *
 * Hydra_Software_Devel/PR55545/1   7/24/09 11:39a mward
 * PR57053: Add 7125 for bring-up.
 *
 * Hydra_Software_Devel/9   7/22/09 7:35p vle
 * PR56776: Prevent HDCP An Timeout
 *
 * Hydra_Software_Devel/8   6/25/09 3:04p vle
 * PR56132, PR48151:
 * Add support for Get HDMI Vendor Specific Info Frame
 * Correct channel assignment for PCM 5.1
 *
 * Hydra_Software_Devel/7   6/23/09 3:42p vle
 * PR55933: Update for 7601/7635
 *
 * Hydra_Software_Devel/6   5/6/09 7:20p vle
 * PR52978: Add support for 7635
 *
 * Hydra_Software_Devel/bdvd_v3.0/1   5/1/09 9:29a rbshah
 * PR_14471[DVD]:[ see HiDef-DVD bug tracking system for more info ].
 * Bring HDMI to life. Audio is muted unconditionally all the time to
 * avoid noisy output (given the chip bug).
 *
 * Qucik verification of HDCP and all supported video resolutions.
 *
 * Reviewer: Narasimha.
 *
 * Hydra_Software_Devel/5   3/24/09 12:27a vle
 * PR52978: Add support for 7635
 *
 * Hydra_Software_Devel/4   3/9/09 3:21p vle
 * PR50570, PR50918, PR49277, PR49652, PR52873:
 * Add API to mute/unmute audio, update pixel repitition support, add
 * SetPixelDataOverride API for transmission of black video. Merge
 * changes/updates from bdvd_v2.0 to main branch.
 *
 * Hydra_Software_Devel/3   3/3/09 8:23p vle
 * PR50569: Add HW Ri/Pj checking feature. Merged from bdvd branch after
 * Rajul's testing effort.
 *
 * Hydra_Software_Devel/PR44535/bdvd_v2.0/3   1/23/09 10:14a rbshah
 * PR_10346 [ see HiDef-DVD bug tracking system for more info ].  Checkin
 * code drop from Anthony Le for Auto Ri,Pj feature in the
 * 7601B0 (Digital Video PR50569).
 *
 * This is disabled by default and will be turned on once it has
 * been tested and soaked.
 *
 * Did verify the A0 build!
 *
 * Hydra_Software_Devel/2   10/9/08 5:55p vle
 * PR45656: Add support for 7420
 *
 * Hydra_Software_Devel/1   10/9/08 4:38p vle
 * PR44535: Merge to main Hydra dev. branch
 *
 * Hydra_Software_Devel/PR44535/1   8/6/08 7:47p vle
 * PR44535: Initial version
 *
 ***************************************************************************/

#ifndef BHDM_CONFIG_H__
#define BHDM_CONFIG_H__

/* HDMI Tx Configuration Options */

/* 
** All HDCP Rx are required to support Short or Fast I2C Reads, where a
** I2C read without an offset indicates to read data from the default I2C 
** offset of the device.  In the case of the HDCP Rx (0x74), the default 
** offset is 0x08 (Ri). 
**
** Some early DVI receivers did not  properly implement the Short Read 
** resulting in Authentication errors.  
** Disable the I2C Short Read here if the STB is having trouble authenticating
** with an older DVI Rx.  Failures might show the same HDCP R0'  value on each
** authentication attempt
*/

#define BHDM_CONFIG_HDCP_RI_SHORT_READ  1

/* enable PLL KICKSTART workaround; useful for IPSTB or excessive jitter streams */
#if (BCHP_CHIP == 7038) || (BCHP_CHIP == 7401)
#define BHDM_CONFIG_PLL_KICKSTART_WORKAROUND 1
#endif


/*
** Time to wait for HDCP An Value to be generated by core 
** the value should be generated within 1 frame 
** default value of 50ms here is about 
*/
#define BHDM_HDCP_CONFIG_AN_TIMEOUT_MS 50

#define BHDM_CONFIG_HDMI_3D_SUPPORT 1

/* available debug options; enables BDBG_MSGs etc.  */
#define BHDM_CONFIG_DEBUG_HDMI_PACKETS 0
#define BHDM_CONFIG_DEBUG_GCP_PACKET_USAGE 0
#define BHDM_CONFIG_DEBUG_AVI_INFOFRAME 0
#define BHDM_CONFIG_DEBUG_AUDIO_INFOFRAME 0
#define BHDM_CONFIG_DEBUG_VENDOR_SPECIFIC_INFOFRAME 0
#define BHDM_CONFIG_DEBUG_SPD_INFOFRAME 0
#define BHDM_CONFIG_DEBUG_FIFO 0
#define BHDM_CONFIG_DEBUG_HDCP_BCAPS 0
#define BHDM_CONFIG_DEBUG_EDID_VSDB 0
#define BHDM_CONFIG_DEBUG_EDID_3DSTRUCT 0
#define BHDM_CONFIG_DEBUG_EDID_3DMASK 0

#define BHDM_CONFIG_DEBUG_MAI_CHANNEL_MAP 0

#define BHDM_CONFIG_REPEATER_SIMULATION_TEST 0
#define BHDM_CONFIG_LINK_INTEGRITY_FAILURE_SIMULATION 0

#define BHDM_CONFIG_DEBUG_PJ_CHECKING 0

#define BHDM_CONFIG_DEBUG_TMDS 0
#define BHDM_CONFIG_DEBUG_RSEN 0


/* enable if BTMR is passed in BHDM_Open settings */
#define BHDM_CONFIG_BTMR_SUPPORT 1

#if BHDM_CONFIG_BTMR_SUPPORT
/* set BHDM_CONFIG_HOTPLUG_DELAY_MS (milliseconds) to delay hot plug processing */
#if defined(HUMAX_PLATFORM_BASE)
/*20130214 set delay to 500ms. To fix No AV issue durig fast HPD swtiching */
#define BHDM_CONFIG_HOTPLUG_DELAY_MS 0
#else
#define BHDM_CONFIG_HOTPLUG_DELAY_MS 0
#endif
/* periodic timer (seconds) to monitor/confirm stable format into HDMI core from BVN */
#define BHDM_CONFIG_DEBUG_MONITOR_FORMAT_CHANGE_S 5
#endif

/* 
Some platforms can translate alternative NTSC,480p formats (480+ lines) to
digital formats (exactly 480 lines).   If using older code, this config option
can be disabled to allow for successful compilation
*/

#define BHDM_CONFIG_ANALOG_TRANSLATION_SUPPORT 1


/******************************************************************/
/* chip revision specific configuration - DO NOT MODIFY ANY BELOW */
/******************************************************************/


/* all chips to use polling to determine Receiver Sense */
#define BHDM_CONFIG_RSEN_POLLING 1


/*************************************** 
	BHDM_CONFIG_DVO_SUPPORT
	- compile for HDMI Transmitter Core
*/
#define BHDM_CONFIG_TX 1

#if ((BCHP_CHIP == 7400) && (BCHP_VER >= BCHP_VER_B0)) \
||   (BCHP_CHIP == 7405) || (BCHP_CHIP == 7325) || (BCHP_CHIP == 7335) \
||   (BCHP_CHIP == 7336) || (BCHP_CHIP == 7340) || (BCHP_CHIP == 7342) \
||   (BCHP_CHIP == 7420) || (BCHP_CHIP == 7125) || (BCHP_CHIP == 7550) \
||   (BCHP_CHIP == 7208) || (BCHP_CHIP == 7408) || (BCHP_CHIP == 7468) \
||   (BCHP_CHIP == 7601) || (BCHP_CHIP == 7630) \
||   (BCHP_CHIP == 7635)
#define BHDM_CONFIG_65NM_SUPPORT 1
#endif



/* *NEW* */
#if ((BCHP_CHIP == 7422) || (BCHP_CHIP == 7425) || (BCHP_CHIP == 7231) \
||   (BCHP_CHIP == 7358) || (BCHP_CHIP == 7344) || (BCHP_CHIP == 7346) \
||   (BCHP_CHIP == 7552) || (BCHP_CHIP == 7640) \
||   (BCHP_CHIP == 7429) || (BCHP_CHIP == 7435) || (BCHP_CHIP == 7360) \
|| (BCHP_CHIP == 7584) || (BCHP_CHIP == 7563))

#define BHDM_CONFIG_40NM_SUPPORT 1
#define BHDM_CONFIG_SCHEDULER_REV_2 1
#define BHDM_CONFIG_HDCP_ADVANCED_HW_AUTO_RI_PJ_SUPPORT 1

#else

#define BHDM_CONFIG_CEC_LEGACY_SUPPORT 1
#endif

/* 28nm platforms */
#if (BCHP_CHIP == 7445)
#define BHDM_CONFIG_28NM_SUPPORT 1
#define BHDM_CONFIG_SCHEDULER_REV_2 1
#define BHDM_CONFIG_HDCP_ADVANCED_HW_AUTO_RI_PJ_SUPPORT 1
#endif


#if (((BCHP_CHIP == 7422) || (BCHP_CHIP == 7425) || (BCHP_CHIP == 7231) \
||    (BCHP_CHIP == 7358) || (BCHP_CHIP == 7344) || (BCHP_CHIP == 7346) \
||    (BCHP_CHIP == 7640)) \
&&    (BCHP_VER < BCHP_VER_B0)) \
|| (BCHP_CHIP == 7552)

#define BHDM_CONFIG_SINGLE_CEC_INTERRUPT  1
#endif


#if ((BCHP_VER >= BCHP_VER_B0) \
&&    ((BCHP_CHIP == 7422) || (BCHP_CHIP == 7425) || (BCHP_CHIP == 7231) \
||     (BCHP_CHIP == 7358) || (BCHP_CHIP == 7344) || (BCHP_CHIP == 7346) \
||     (BCHP_CHIP == 7640))) \
 \
|| (BCHP_CHIP == 7429) || (BCHP_CHIP == 7435) || (BCHP_CHIP == 7360) \
|| (BCHP_CHIP == 7584) || (BCHP_CHIP == 7563) \
\
/* 28nm platform */	\
|| (BCHP_CHIP == 7445)

#define BHDM_CONFIG_DUAL_CEC_TXRX_INTERRUPT 1
#define BHDM_CONFIG_PIXEL_OVERRIDE_UPDATE 1
#define BHDM_CONFIG_CLOCK_STOP_SUPPORT 1
#endif


#if (BCHP_CHIP == 7435) || (BCHP_CHIP == 7436) || (BCHP_CHIP == 7360) \
||  (BCHP_CHIP == 7445) ||  (BCHP_CHIP == 7584)	\
|| ((BCHP_CHIP == 7429) && (BCHP_VER >= BCHP_VER_B0)) \
|| (BCHP_CHIP == 7563)
#define BHDM_CONFIG_DUAL_HPD_SUPPORT 1
#endif


 /* 7231, 7358 and 7552 requires swapping of the HDMI_TX_PHY channel settings
 	The default values/settings do not work */
#if (BCHP_CHIP == 7231) || (BCHP_CHIP == 7358) || (BCHP_CHIP == 7552) || (BCHP_CHIP == 7543) \
  || (BCHP_CHIP == 7346) || (BCHP_CHIP == 7344) || (BCHP_CHIP == 7429)	
  
 /* the following macro is used to correctly read the clock channel which is internally swapped  */
#define BHDM_CONFIG_SWAP_DEFAULT_PHY_CHANNELS 1

#if (BCHP_CHIP == 7358) || (BCHP_CHIP == 7552) || (BCHP_CHIP == 7543) \
  || (BCHP_CHIP == 7346) || (BCHP_CHIP == 7344) || (BCHP_CHIP == 7429)
#define BHDM_CONFIG_HDMI_TX_PHY_CHANNEL_SWAP_TXCK_OUT_INV 0
#define BHDM_CONFIG_HDMI_TX_PHY_CHANNEL_SWAP_TX2_OUT_INV 0
#define BHDM_CONFIG_HDMI_TX_PHY_CHANNEL_SWAP_TX1_OUT_INV 0
#define BHDM_CONFIG_HDMI_TX_PHY_CHANNEL_SWAP_TX0_OUT_INV 0

#define BHDM_CONFIG_HDMI_TX_PHY_CHANNEL_SWAP_TXCK_OUT_SEL 2
#define BHDM_CONFIG_HDMI_TX_PHY_CHANNEL_SWAP_TX2_OUT_SEL 3
#define BHDM_CONFIG_HDMI_TX_PHY_CHANNEL_SWAP_TX1_OUT_SEL 0
#define BHDM_CONFIG_HDMI_TX_PHY_CHANNEL_SWAP_TX0_OUT_SEL 1 
#else
#define BHDM_CONFIG_HDMI_TX_PHY_CHANNEL_SWAP_TXCK_OUT_INV 0
#define BHDM_CONFIG_HDMI_TX_PHY_CHANNEL_SWAP_TX2_OUT_INV 0
#define BHDM_CONFIG_HDMI_TX_PHY_CHANNEL_SWAP_TX1_OUT_INV 0
#define BHDM_CONFIG_HDMI_TX_PHY_CHANNEL_SWAP_TX0_OUT_INV 0

#define BHDM_CONFIG_HDMI_TX_PHY_CHANNEL_SWAP_TXCK_OUT_SEL 3
#define BHDM_CONFIG_HDMI_TX_PHY_CHANNEL_SWAP_TX2_OUT_SEL 2
#define BHDM_CONFIG_HDMI_TX_PHY_CHANNEL_SWAP_TX1_OUT_SEL 1
#define BHDM_CONFIG_HDMI_TX_PHY_CHANNEL_SWAP_TX0_OUT_SEL 0 
#endif
#endif


#if BHDM_CONFIG_65NM_SUPPORT \
||  BHDM_CONFIG_40NM_SUPPORT \
|| 	BHDM_CONFIG_28NM_SUPPORT
#define BHDM_CONFIG_RECEIVER_SENSE_SUPPORT 1

#define BHDM_CONFIG_PRE_EMPHASIS_SUPPORT 1
#endif



#if (BCHP_CHIP == 7601) || (BCHP_CHIP == 7420) || (BCHP_CHIP == 7125) \
||  (BCHP_CHIP == 7340) ||  (BCHP_CHIP == 7342) \
||  (BCHP_CHIP == 7635) || (BCHP_CHIP == 7630) || (BCHP_CHIP == 7550) \
||  (BCHP_CHIP == 7408) || (BCHP_CHIP == 7468) || (BCHP_CHIP == 7208) \
|| BHDM_CONFIG_40NM_SUPPORT \
|| BHDM_CONFIG_28NM_SUPPORT
#define BHDM_CONFIG_HDMI_1_3_SUPPORT 1
#endif


/* check if a Tx Box is using DVO */
#if !defined(BHDM_CONFIG_DVO_SUPPORT)
#if BHDM_ENABLE_DVO 
#define BHDM_CONFIG_DVO_SUPPORT 1
#endif
#endif


/***************************************
	BHDM_CONFIG_DISABLE_MUX_VSYNC_ON_AVMUTE
	- disable mux_vsync when using AvMute in case HDCP is enabled
	- See PR11060
****************************************/
#if (BCHP_CHIP == 7038) \
	&& ((BCHP_VER <= BCHP_VER_B1))
#define BHDM_CONFIG_DISABLE_MUX_VSYNC_ON_AVMUTE 1
#endif


/***************************************
	BHDM_CONFIG_DISABLE_HDCP_AUTH_REPEATER_DEVCOUNT0
	- revoke authentication for repeaters with an attached device count of 0
	- See PR15217
***************************************/
#if (BCHP_CHIP == 7038) \
	&& (((BCHP_VER >= BCHP_VER_B0) && (BCHP_VER <= BCHP_VER_B2)) \
	||	  ((BCHP_VER <= BCHP_VER_C1)))

#define BHDM_CONFIG_DISABLE_HDCP_AUTH_REPEATER_DEVCOUNT0 1
#endif


/*************************************** 
	BHDM_CONFIG_DRIVER_AMP_DEFAULT 
****************************************/
#if (BCHP_CHIP == 7038)

#if BCHP_VER <= BCHP_VER_B2
#define BHDM_CONFIG_DRIVER_AMP_DEFAULT 6

#elif BCHP_VER >= BCHP_VER_C0
#define BHDM_CONFIG_DRIVER_AMP_DEFAULT 4

#else
#error "Unknown 7038 revision; need to specify PREDRIVER_AMP_DEFAULT value"
#endif

#elif (BCHP_CHIP == 7438) || (BCHP_CHIP == 7440) || (BCHP_CHIP == 7403)
#define BHDM_CONFIG_DRIVER_AMP_DEFAULT 5

#elif (BCHP_CHIP == 7401)
#define BHDM_CONFIG_DRIVER_AMP_DEFAULT 4

#elif ((BCHP_CHIP == 7400) && (BCHP_VER >= BCHP_VER_B0)) || (BCHP_CHIP == 7405) \
	|| (BCHP_CHIP == 7325) || (BCHP_CHIP == 7335) || (BCHP_CHIP == 7336)
#define BHDM_CONFIG_DRIVER_AMP_DEFAULT 3

#elif (BCHP_CHIP == 7601) || (BCHP_CHIP == 7420) || (BCHP_CHIP == 7635) || (BCHP_CHIP == 7630) \
   || (BCHP_CHIP == 7340) || (BCHP_CHIP == 7342) || (BCHP_CHIP == 7125) || (BCHP_CHIP == 7550) \
   || (BCHP_CHIP == 7408) || (BCHP_CHIP == 7468) || (BCHP_CHIP == 7208) \
   || BHDM_CONFIG_40NM_SUPPORT \
   || BHDM_CONFIG_28NM_SUPPORT
#define BHDM_CONFIG_DRIVER_AMP_DEFAULT 5

#elif !defined(BHDM_CONFIG_TX)
	/* not used */
#else
#error "New/Unknown chip; no PREDRIVER_AMP_DEFAULT value"
#endif


/***************************************
	BHDM_CONFIG_DVO_7038BX_VERSION

	changes DVO pins in starting in 7038 C0
***************************************/

#if (BCHP_CHIP == 7038)

#if (BCHP_VER >=  BCHP_VER_B0) && (BCHP_VER <= BCHP_VER_B2)
#define BHDM_CONFIG_DVO_7038BX_VERSION 1
#else
#define BHDM_CONFIG_DVO_7038CX_VERSION 1
#endif

#elif (BCHP_CHIP == 7401)
#define BHDM_CONFIG_DVO_7401_VERSION 1
#endif


/***************************************
	BHDM_CONFIG_RDB_NAME_FIX
*/

#if (BCHP_CHIP == 7038) || (BCHP_CHIP == 7438) || (BCHP_CHIP == 3560)
#define BHDM_CONFIG_RDB_NAME_FIX 1
#endif


/***************************************
	BHDM_CONFIG_88_2KHZ_AUDIO_SUPPORT
***************************************/
#if ((BCHP_CHIP == 7038) && (BCHP_VER >= BCHP_VER_C0))	|| (BCHP_CHIP == 7438) \
	|| (BCHP_CHIP == 7601) || (BCHP_CHIP == 7420) || (BCHP_CHIP == 7635) || (BCHP_CHIP == 7630) \
	|| (BCHP_CHIP == 7340) || (BCHP_CHIP == 7342) || (BCHP_CHIP == 7125) || (BCHP_CHIP == 7550) \
	|| (BCHP_CHIP == 7408) || (BCHP_CHIP == 7468) || (BCHP_CHIP == 7208) \
	|| BHDM_CONFIG_40NM_SUPPORT \
	|| BHDM_CONFIG_28NM_SUPPORT
#define BHDM_CONFIG_88_2KHZ_AUDIO_SUPPORT 1
#endif


/***************************************
	BHDM_CONFIG_96KHZ_AUDIO_SUPPORT
***************************************/
#if (BCHP_CHIP == 7438) ||	(BCHP_CHIP == 7440) || (BCHP_CHIP==7400) || (BCHP_CHIP == 7405) \
	|| (BCHP_CHIP == 7601) || (BCHP_CHIP == 7420) || (BCHP_CHIP == 7635) || (BCHP_CHIP == 7630) \
	|| (BCHP_CHIP == 7340) || (BCHP_CHIP == 7342) || (BCHP_CHIP == 7125) || (BCHP_CHIP == 7550) \
	|| (BCHP_CHIP == 7408) || (BCHP_CHIP == 7468) || (BCHP_CHIP == 7208) \
	|| BHDM_CONFIG_40NM_SUPPORT \
	|| BHDM_CONFIG_28NM_SUPPORT	
#define BHDM_CONFIG_96KHZ_AUDIO_SUPPORT 1
#endif


/***************************************
	BHDM_CONFIG_176_4KHZ_AUDIO_SUPPORT
***************************************/
#if ((BCHP_CHIP == 7038) && (BCHP_VER >= BCHP_VER_C0))	\
	|| (BCHP_CHIP == 7438) ||  (BCHP_CHIP == 7405) || (BCHP_CHIP == 7400) \
	|| (BCHP_CHIP == 7335) || (BCHP_CHIP == 7336) || ((BCHP_CHIP == 7325) && (BCHP_VER >= BCHP_VER_B0)) \
	|| (BCHP_CHIP == 7601) || (BCHP_CHIP == 7420) || (BCHP_CHIP == 7635) || (BCHP_CHIP == 7630) \
	|| (BCHP_CHIP == 7340) || (BCHP_CHIP == 7342) || (BCHP_CHIP == 7125) || (BCHP_CHIP == 7550) \
	|| (BCHP_CHIP == 7408) || (BCHP_CHIP == 7468) || (BCHP_CHIP == 7208) \
	|| BHDM_CONFIG_40NM_SUPPORT \
	|| BHDM_CONFIG_28NM_SUPPORT	
#define BHDM_CONFIG_176_4KHZ_AUDIO_SUPPORT 1
#endif


/***************************************
	BHDM_CONFIG_192KHZ_AUDIO_SUPPORT
***************************************/
#if ((BCHP_CHIP == 7038) && (BCHP_VER >= BCHP_VER_C0))	|| (BCHP_CHIP == 7438) \
	|| (BCHP_CHIP == 7440) || (BCHP_CHIP == 7443) || (BCHP_CHIP==7400) || (BCHP_CHIP == 7405) \
	|| (BCHP_CHIP == 7335) || (BCHP_CHIP == 7336) || (BCHP_CHIP == 7325) || (BCHP_CHIP == 7401) \
	|| (BCHP_CHIP == 7601) || (BCHP_CHIP == 7420) || (BCHP_CHIP == 7635) || (BCHP_CHIP == 7630) \
	|| (BCHP_CHIP == 7340) || (BCHP_CHIP == 7342) || (BCHP_CHIP == 7125) || (BCHP_CHIP == 7550) \
	|| (BCHP_CHIP == 7408) || (BCHP_CHIP == 7468) || (BCHP_CHIP == 7208) \
	|| BHDM_CONFIG_40NM_SUPPORT \
	|| BHDM_CONFIG_28NM_SUPPORT	
#define BHDM_CONFIG_192KHZ_AUDIO_SUPPORT 1
#endif


/***************************************
	BHDM_CONFIG_1080P_5060HZ_SUPPORT

	7340/7342/7408/7208 uses dvp_ht_lite core, does not support 1080p/60 & 1080p/50
***************************************/
#if (BCHP_CHIP == 7601) || (BCHP_CHIP == 7420) || (BCHP_CHIP == 7635) || (BCHP_CHIP == 7630) \
	|| (BCHP_CHIP == 7125) || (BCHP_CHIP == 7550) || (BCHP_CHIP == 7468) \
	|| BHDM_CONFIG_40NM_SUPPORT \
	|| BHDM_CONFIG_28NM_SUPPORT	
#define BHDM_CONFIG_1080P_5060HZ_SUPPORT 1
#endif


/***************************************
	BHDM_CONFIG_1366_FORMAT_CHECK
**************************************/
#if !((BCHP_CHIP == 7401) || (BCHP_CHIP == 7038))
#define BHDM_CONFIG_1366_FORMAT_CHECK 1
#endif


#if BHDM_CEC_SUPPORT
/***************************************
	BHDM_CONFIG_CEC_DEVICE_TYPE
**************************************/
#if (BCHP_CHIP == 7438) || (BCHP_CHIP == 7440) || (BCHP_CHIP == 7601) \
        || (BCHP_CHIP==7635) || (BCHP_CHIP == 7630) || (BCHP_CHIP == 7640)
#define BHDM_CONFIG_CEC_DEVICE_TYPE 0x04
#else
#define BHDM_CONFIG_CEC_DEVICE_TYPE 0x03
#endif


/***************************************
	Uninitialized CEC logical address
**************************************/
#define BHDM_CONFIG_CEC_UNINITIALIZED_LOGICAL_ADDR 0xFF


/***************************************
	Older platforms use CEC PAD_SW_RESET register
	7601/7420 and newer platforms no longer use this.
*/
#if (BCHP_CHIP == 7038)	|| (BCHP_CHIP == 7438) || (BCHP_CHIP == 7440) || (BCHP_CHIP==7400) \
	|| (BCHP_CHIP == 7401) || (BCHP_CHIP == 7405) || (BCHP_CHIP == 7335) || (BCHP_CHIP == 7336) \
	|| (BCHP_CHIP == 7325)
#define BHDM_CONFIG_CEC_USE_PAD_SW_RESET 1
#endif


#endif


/***************************************
	Broadcom Supported audio formats; 
	use to determine support for Audio Formats specified in EDID	
**************************************/
#if (BCHP_CHIP == 7401) || (BCHP_CHIP == 7400) || (BCHP_CHIP == 7402) || (BCHP_CHIP == 7403) \
	|| (BCHP_CHIP == 7405) || (BCHP_CHIP == 7325) || (BCHP_CHIP == 7335) || (BCHP_CHIP == 7336) \
	|| (BCHP_CHIP == 7601) || (BCHP_CHIP == 7420) || (BCHP_CHIP == 7635) || (BCHP_CHIP == 7630) \
	|| (BCHP_CHIP == 7340) || (BCHP_CHIP == 7342) || (BCHP_CHIP == 7125) || (BCHP_CHIP == 7550) \
	|| (BCHP_CHIP == 7408) || (BCHP_CHIP == 7468) || (BCHP_CHIP == 7208) \
	|| BHDM_CONFIG_40NM_SUPPORT \
	|| BHDM_CONFIG_28NM_SUPPORT	
#define BHDM_CONFIG_AUDIO_SUPPORT_DDP 1
#endif


#if (BCHP_CHIP == 7401) ||	(BCHP_CHIP == 7400) || (BCHP_CHIP == 7402) || (BCHP_CHIP == 7403) \
	|| (BCHP_CHIP == 7405) || (BCHP_CHIP == 7325) || (BCHP_CHIP == 7335) || (BCHP_CHIP == 7336)
#define BHDM_CONFIG_AUDIO_SUPPORT_WMAPRO 1
#endif


#if  (BCHP_CHIP == 7440) || (BCHP_CHIP == 7601) || (BCHP_CHIP == 7420) || (BCHP_CHIP == 7635) \
	|| (BCHP_CHIP == 7630) || (BCHP_CHIP == 7340) || (BCHP_CHIP == 7342) || (BCHP_CHIP == 7125) \
	|| (BCHP_CHIP == 7550) || (BCHP_CHIP == 7408) || (BCHP_CHIP == 7468) || (BCHP_CHIP == 7208) \
	|| BHDM_CONFIG_40NM_SUPPORT \
	|| BHDM_CONFIG_28NM_SUPPORT	
#define BHDM_CONFIG_AUDIO_SUPPORT_DTSHD 1
#define BHDM_CONFIG_AUDIO_SUPPORT_MATMLP 1
#endif


/***************************************
	Force sending ENC_DIS signal from PI
	if this is not handled by the hardware
***************************************/
#if ((BCHP_CHIP==7400) && (BCHP_VER>=BCHP_VER_E0))
	/* Do not force ENC_DIS/ENC_EN signal */
#else
#define BHDM_CONFIG_HDCP_FORCE_ENC_SIGNAL 1
#endif


/***************************************
	BHDM_CONFIG_AUTO_RI_PJ_CHECK
***************************************/
#if ((BCHP_CHIP == 7601) && (BCHP_VER >= BCHP_VER_B0)) \
	|| (BCHP_CHIP == 7420) || (BCHP_CHIP==7125) \
	|| (BCHP_CHIP == 7340) || (BCHP_CHIP == 7342) \
	|| (BCHP_CHIP == 7635) || (BCHP_CHIP == 7630) \
	|| (BCHP_CHIP == 7550) || (BCHP_CHIP == 7408) \
	|| (BCHP_CHIP == 7468) || (BCHP_CHIP == 7208) \
	|| BHDM_CONFIG_40NM_SUPPORT \
	|| BHDM_CONFIG_28NM_SUPPORT	
#define BHDM_CONFIG_HDCP_AUTO_RI_PJ_CHECKING_SUPPORT 1
#endif


/***************************************
	BHDM_CONFIG_AUDIO_MAI_BUS_DISABLE_SUPPORT
**************************************/
#if ((BCHP_CHIP == 7601) && (BCHP_VER >= BCHP_VER_B0)) \
	|| (BCHP_CHIP == 7420) || (BCHP_CHIP==7125) \
	|| (BCHP_CHIP == 7340) || (BCHP_CHIP == 7342) \
	|| (BCHP_CHIP == 7635) || (BCHP_CHIP == 7630) \
	|| (BCHP_CHIP == 7550) || (BCHP_CHIP == 7468) \
	|| (BCHP_CHIP == 7408) || (BCHP_CHIP == 7208) \
	|| BHDM_CONFIG_40NM_SUPPORT \
	|| BHDM_CONFIG_28NM_SUPPORT	
#define BHDM_CONFIG_AUDIO_MAI_BUS_DISABLE_SUPPORT 1
#endif


/***************************************
	BHDM_CONFIG_REGISTER_GCP_FOR_AV_MUTE
	use Register based General Control Packet (GCP)
	vs RAM Packet based General Control Packet 
***************************************/
#if BHDM_CONFIG_40NM_SUPPORT \
	|| BHDM_CONFIG_28NM_SUPPORT
#define BHDM_CONFIG_REGISTER_GCP_FOR_AV_MUTE 1
#endif


/***************************************
	BHDM_CONFIG_BLURAY_PLATFORMS
***************************************/
#if (BCHP_CHIP == 7601) || (BCHP_CHIP == 7635) || (BCHP_CHIP == 7630) || (BCHP_CHIP == 7640)
#define BHDM_CONFIG_BLURAY_PLATFORMS
#endif


/*****************************************
	PRE-EMPHASIS Settings to work around bad PLL in some 40nm 
	A0 platforms. This hardware	issue is fixed in December 2010
******************************************/
#if (((BCHP_CHIP == 7422) || (BCHP_CHIP == 7425) || (BCHP_CHIP == 7231) \
	|| (BCHP_CHIP == 7344) || (BCHP_CHIP == 7346) || (BCHP_CHIP == 7640)) \
	&& (BCHP_VER < BCHP_VER_A1))
#define BHDM_CONFIG_40NM_PREEMP_WORKAROUND	1
#endif


/***************************************
	BHDM_CONFIG_AUDIO_MAI_BUS_CHANNEL_MAP_1TO1
***************************************/
#ifdef BHDM_CONFIG_BLURAY_PLATFORMS
#define BHDM_CONFIG_AUDIO_MAI_BUS_CHANNEL_MAP_1TO1 1
#endif


/***************************************
	CRC circuit only available in DVP_HT but not the FALCON 
****************************************/
#if BHDM_CONFIG_HDMI_1_3_SUPPORT
#define BHDM_CONFIG_CRC_SUPPORT 1

#if BHDM_CONFIG_40NM_SUPPORT \
	|| BHDM_CONFIG_28NM_SUPPORT
#define REGADDR_CRC_CHECK_STATUS_0 BCHP_DVP_HT_HDMI_TX_0_IN_CRC_CHECK_STATUS_0 
#define REGNAME_CRC_CHECK_STATUS_0 DVP_HT_HDMI_TX_0_IN_CRC_CHECK_STATUS_0 

#elif BHDM_CONFIG_65NM_SUPPORT
#define REGADDR_CRC_CHECK_STATUS_0 BCHP_DVP_HT_HDMI_TX_0_IN_DVP_CRC_CHECK_STATUS_0
#define REGNAME_CRC_CHECK_STATUS_0 DVP_HT_HDMI_TX_0_IN_DVP_CRC_CHECK_STATUS_0
#endif

#endif	/*end BHDM_CONFIG_HDMI_1_3_SUPPORT */


#if BHDM_CONFIG_BTMR_SUPPORT

/* all 40nm and some 65nm chips  support format detection */
#if BHDM_CONFIG_40NM_SUPPORT \
|| 	BHDM_CONFIG_28NM_SUPPORT \
|| (BCHP_CHIP == 7125) || (BCHP_CHIP == 7340) || (BCHP_CHIP == 7342) \
|| (BCHP_CHIP == 7208) || (BCHP_CHIP == 7408) || (BCHP_CHIP == 7468) \
|| (BCHP_CHIP == 7231) || (BCHP_CHIP == 7420) 
#define BHDM_CONFIG_FORMAT_CHANGE_DETECTION_SUPPORT 1
#endif

#endif

#endif /* BHDM_CONFIG_H__ */

