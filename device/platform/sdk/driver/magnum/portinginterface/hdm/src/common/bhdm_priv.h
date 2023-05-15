/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bhdm_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/94 $
 * $brcm_Date: 9/25/12 6:12p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: Q:/projects/7425/latest/magnum/portinginterface/hdm/src/common/bhdm_priv.h $
 * 
 * Hydra_Software_Devel/94   9/25/12 6:12p rgreen
 * SW7425-3907: When TMDS output has been disabled, do not set hotplug
 * event due to change in state of RxSense detection
 * 
 * Hydra_Software_Devel/93   9/20/12 3:26p vle
 * SW7445-23: Add HDM PI for 7445
 * 
 * Hydra_Software_Devel/92   9/10/12 1:42p rgreen
 * SW7420-2349: Update BHDM_P_RxDeviceAttaced to isr function.  Update
 * calls to add Enter/Leave Critical Section
 * 
 * Hydra_Software_Devel/91   8/24/12 5:01p rgreen
 * SW7425-3803: Add Phy PowerDown for standby/close
 * 
 * Hydra_Software_Devel/90   7/12/12 1:24p rgreen
 * SW7425-2989: Merge changes
 * 
 * Hydra_Software_Devel/SW7425-2989/2   7/11/12 6:17p rgreen
 * SW7425-2989: Prevent initial debug warning after format change
 * 
 * Hydra_Software_Devel/SW7425-2989/1   7/6/12 2:17p rgreen
 * SW7425-2989: Add debug monitor to report format changes to HDMI Tx core
 * 
 * Hydra_Software_Devel/89   6/20/12 2:35p rgreen
 * SW7358-263, SW7425-3206: Use polling to detect RxSense state instead of
 * interrupts
 * 
 * Hydra_Software_Devel/88   4/30/12 3:23p rgreen
 * SW7405-5557: Report HDMI Rx does not support a particular supported
 * BcmFormat only once per EDID initialization
 * 
 * Hydra_Software_Devel/87   4/24/12 2:31p rgreen
 * SW7425-2075: Merge Changes
 * 
 * Hydra_Software_Devel/SW7425-2075/1   4/12/12 11:58a jgarrett
 * SW7425-2075: Removing MAI Channel Reordering in HBR mode
 * 
 * Hydra_Software_Devel/86   4/10/12 5:06p rgreen
 * SW7425-2707: Force MonitorRange Settings for 60Hz if MonitorRange
 * descriptor is not found
 * 
 * Hydra_Software_Devel/85   3/22/12 4:05p rgreen
 * SW7425-2666: Update algorithm for checking for valid video into the
 * HDMI core
 * 
 * Hydra_Software_Devel/84   3/19/12 11:35a rgreen
 * SW7425-2650: Fix memory leak in HDM PI; Delcare/store supported Video
 * ID Codes in hdm handlle vs mallocing to build list each time
 * 
 * Hydra_Software_Devel/83   3/14/12 6:59p vle
 * SW7425-2515: Remove references to bhdm_cec files
 * 
 * Hydra_Software_Devel/82   3/1/12 2:50p rgreen
 * SW7425-2515: Remove unused CEC code which is now located in CEC pi;
 * Remove CEC from BHDM interrupt table
 * 
 * Hydra_Software_Devel/81   3/1/12 1:20p rgreen
 * SW7425-2515: Remove unused CEC code which is now located in CEC pi;
 * remove CEC EventHandle
 * 
 * Hydra_Software_Devel/80   2/23/12 10:50a rgreen
 * SW7125-1146,SW7408-317: Merge changes
 * 
 * Hydra_Software_Devel/SW7408-317/1   2/21/12 6:33p rgreen
 * SW7125-1146,SW7408-317: Treat RxSense and HP events separately.  Update
 * processing of both events
 * 
 * Hydra_Software_Devel/79   2/9/12 3:59p rgreen
 * SW7231-345,SW7125-1146,SW7425-2361: Refactor HDMI Power Management;
 * separate TMDS power from clock
 * 
 * Hydra_Software_Devel/78   1/27/12 2:10p vle
 * SW7125-1146: merge to mainline
 * 
 * Hydra_Software_Devel/SW7125-1146/2   1/26/12 5:35p vle
 * SW7125-1146: Get RSEN setting at isr vs event time for applicable
 * platforms
 * 
 * Hydra_Software_Devel/77   1/23/12 11:25a rgreen
 * SW7125-1146: Merge Changes
 * 
 * Hydra_Software_Devel/SW7125-1146/1   1/19/12 2:35p rgreen
 * SW7125-1146: Enable TMDS at open to fix interrupt issue; Get RSEN
 * setting at isr vs event time;
 * 
 * Hydra_Software_Devel/76   1/6/12 6:03p vle
 * SW7435-11: implement support for check/clearHotplugInterrupt for 7435
 * 
 * Hydra_Software_Devel/75   1/6/12 2:59p vle
 * SW7435-11: Add support for 7435
 * 
 * Hydra_Software_Devel/74   11/22/11 6:01p vle
 * SW7425-1140: Merge to mainline.  Remove all CEC functionality out of
 * HDM PI.
 * 
 * Hydra_Software_Devel/SW7425-1140/2   11/22/11 5:48p vle
 * SW7425-1140: Add BHDM_CONFIG_CEC_LEGACY_SUPPORT for backward compatible
 * for CEC legacy platforms.
 * 
 * Hydra_Software_Devel/SW7425-1140/1   11/16/11 12:16p vle
 * SW7425-1140: Remove all CEC functionalities out of HDM PI
 * 
 * Hydra_Software_Devel/73   11/14/11 2:15p rgreen
 * SW7425-1710: Update BHDM_CONFIG macro usage.  Describe specific
 * functionality vs chip process
 * 
 * Hydra_Software_Devel/72   10/11/11 4:50p vle
 * SW7429-5: Add support for 7429.
 * 
 * Hydra_Software_Devel/71   6/7/11 6:44p vle
 * SW7425-532: Add HDMI CEC support for 40nm chip. Use correct CEC
 * interrupt.
 * 
 * Hydra_Software_Devel/70   2/17/11 7:38p jtna
 * SW7420-1141: rework HDMI power management. break public API to allow
 * for StandbySettings
 * 
 * Hydra_Software_Devel/69   10/18/10 4:37p vle
 * SW7420-1177: Add DVO support for 7420
 * 
 * Hydra_Software_Devel/68   10/7/10 6:47p jtna
 * SW7420-972: merge hdmi power management
 * 
 * Hydra_Software_Devel/SW7420-972/1   10/6/10 7:01p jtna
 * SW7420-972: BCHP_PWR power management for HDM
 * 
 * Hydra_Software_Devel/67   9/29/10 4:14p vle
 * SW7422-23: Fix build issues without CEC enable
 * 
 * Hydra_Software_Devel/66   9/28/10 7:19p vle
 * SW7422-23: Refactor HDMI code to isolate platform dependent code
 * furthermore. Add support for 7422 and other 40nm platforms.
 * 
 * Hydra_Software_Devel/65   9/24/10 5:38p vle
 * SW7342-238: Take 2 The VEC will only operate double the rate (54Mhz) in
 * 480p format, not 480i. Make sure audio parameters for all other 27Mhz
 * pixel clock format are programmed correctly.
 * 
 * Hydra_Software_Devel/64   9/24/10 2:25p vle
 * SW7342-238: The VEC will only operate double the rate (54Mhz) in 480p
 * format, not 480i. Make sure audio parameters for all other 27Mhz pixel
 * clock format are programmed correctly.
 * 
 * Hydra_Software_Devel/63   8/27/10 7:48p vle
 * SW7400-2868: Fix potential EDID parser issue when parsing 3D supports
 * on the first 16 video descriptor
 * 
 * Hydra_Software_Devel/62   6/22/10 6:57p vle
 * SW7405-3994: Add support to parse all Shorthand and additional 3D
 * Timing/Structure support indication in HDMI 1.4a
 * 
 * Hydra_Software_Devel/61   6/4/10 6:09p vle
 * SW7405-3994: Merge to main branch
 * 
 * Hydra_Software_Devel/SW7405-3994/1   5/14/10 6:12p vle
 * SW7405-3994: Add support to check for supported 3D formats.
 * 
 * Hydra_Software_Devel/60   5/26/10 2:48p vle
 * SW7405-4333: Add support for 40, 65, and 65/1.001 Mhz pixel clock rate
 * 
 * Hydra_Software_Devel/59   4/23/10 10:43a vle
 * SW7420-676: merge to main branch
 * 
 * Hydra_Software_Devel/SW7420-676/1   4/21/10 2:27p vle
 * SW7420-676: Add API to return supported video info
 * 
 * Hydra_Software_Devel/58   4/16/10 6:50p vle
 * SW7420-543: BHDM_InputPixelClock enum should be private
 * 
 * Hydra_Software_Devel/57   4/2/10 6:40p vle
 * SW7601-172: Rename to clearly indicate SetGamutMetadataPacket is a
 * private API.
 * 
 * Hydra_Software_Devel/56   4/2/10 5:59p rgreen
 * SW7405-3994: Merge Update HDMI 1.4 parsing for 3D Structure fields in
 * the VSDB
 * 
 * Hydra_Software_Devel/SW7401-4363/1   3/5/10 4:11p rgreen
 * JIRA:SW7405-3994: Update HDMI 1.4 parsing for 3D Structure fields in
 * the VSDB
 * 
 * Hydra_Software_Devel/55   3/26/10 4:34p vle
 * SW7601-172: Merge xvYCC support with Gamut Metadata Packet transmission
 * from bdvd branch
 * 
 * Hydra_Software_Devel/54   3/1/10 11:32a rgreen
 * SW7420-579: Rename DetailedTiming to SupportedDetailTiming to eliminate
 * confusion when reading code
 * 
 * Hydra_Software_Devel/53   2/23/10 12:49a vle
 * SW7420-579: Refactor HDMI PI.
 * 
 * Hydra_Software_Devel/9   1/8/10 5:19p vle
 * SW7405-3740: Port changes to 7420, 7468, and other platforms.
 * Add isr callback to provide immediate notifcation of HP changes vs
 * waiting for event processing
 * 
 * Hydra_Software_Devel/8   1/6/10 4:38p vle
 * SW3548-2670: Centralize all CEC timing configurations
 * 
 * Hydra_Software_Devel/7   9/23/09 2:15p vle
 * SW7601-165: Merge changes in bdvd_v3.0 branch to main branch.
 * 
 * Hydra_Software_Devel/bdvd_v3.0/2   9/2/09 3:31p rbshah
 * PR16468[DVD]:[ see HiDef-DVD bug tracking system for more info ].
 * Merged with the latest portinginterface/hdm and syslib/hdcplib files.
 * 
 * Hydra_Software_Devel/bdvd_v2.0/bdvd_v2.1/2   7/14/09 1:30p rbshah
 * PR_15413[DVD]:[ see HiDef-DVD bug tracking system for more info ].
 * Merge work from the HDMI certification branch. Also addresses PR15437
 * and PR15220 (merged from v2.0). Plus coverity PR15782.
 * 
 * Hydra_Software_Devel/bdvd_v2.0/bdvd_v2.1/bdvd_hdmi_cert_v2.1/1   7/8/09 12:18p rbshah
 * Various fixes for Auto Hardware Ri,Pj checking. I2C changes are
 * temporary. Switch from software to hardware Ri checking by default.
 * 
 * Hydra_Software_Devel/bdvd_v2.0/bdvd_v2.1/1   4/3/09 6:12p rbshah
 * PR_13071[DVD]:[ see HiDef-DVD bug tracking system for more info ].  Add
 * CEC support at the BDVD/Display API. This is really a back port
 * from bdvd_v2.0 and dev_pr13071 branches.
 * 
 * Hydra_Software_Devel/6   8/26/09 3:41p vle
 * SW7405-2670:
 * Add implementation of interrupt based receiver sense
 * 
 * Hydra_Software_Devel/5   7/22/09 7:35p vle
 * PR56776: Prevent HDCP An Timeout
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
 * Hydra_Software_Devel/bdvd_v2.0/2   1/23/09 10:14a rbshah
 * PR_10346 [ see HiDef-DVD bug tracking system for more info ].  Checkin
 * code drop from Anthony Le for Auto Ri,Pj feature in the
 * 7601B0 (Digital Video PR50569).
 * 
 * This is disabled by default and will be turned on once it has
 * been tested and soaked.
 * 
 * Did verify the A0 build!
 *
 * Hydra_Software_Devel/bdvd_v2.0/1   1/21/09 11:56a rbshah
 * PR_10346 [ see HiDef-DVD bug tracking system for more info ].  Enhance
 * Display/HDMI API to allow application to mute/unmute just
 * audio (Digital Video PR50570).
 * 
 * This feature is only available on the 7601B0.
 * Hydra_Software_Devel/2   12/2/08 11:11a vle
 * PR49651: Fix CEC compiling issue for 7601/7420
 * 
 * Hydra_Software_Devel/1   10/9/08 4:40p vle
 * PR44535: Merge to main Hydra dev. branch
 * 
 * Hydra_Software_Devel/PR44535/1   8/6/08 7:46p vle
 * PR44535: Initial version
 * 
 ***************************************************************************/

#ifndef BHDM_PRIV_H__
#define BHDM_PRIV_H__

#include "blst_queue.h"
#include "bhdm_config.h"
#include "bhdm_hdcp.h"
#include "bhdm_edid.h"

#include "bchp.h"       /* Chip Info */
#include "bchp_sun_top_ctrl.h" 
#include "breg_mem.h"   /* Chip register access. */
#include "bkni.h"       /* Kernel Interface */
#include "bint.h"       /* Interrupt */
#include "breg_i2c.h"   /* I2C */


#include "bchp_hdmi.h"
#include "bchp_hdmi_rm.h"  /* HDMI Rate Manager */
#include "bchp_hdmi_ram.h" /* HDMI Packet RAM */
#if BHDM_CONFIG_40NM_SUPPORT \
||	BHDM_CONFIG_28NM_SUPPORT
#include "bchp_aon_hdmi_tx.h"
#include "bchp_hdmi_tx_intr2.h"
#include "bchp_int_id_hdmi_tx_intr2.h"
#include "bchp_int_id_aon_pm_l2.h"
#include "bchp_int_id_aon_l2.h"
#else
#include "bchp_hdmi_intr2.h"
#include "bchp_int_id_hdmi_intr2.h"
#endif

#if BHDM_CONFIG_65NM_SUPPORT || BHDM_CONFIG_40NM_SUPPORT || BHDM_CONFIG_28NM_SUPPORT
#include "bchp_hdmi_tx_phy.h"
#endif

#if BHDM_CONFIG_HDMI_1_3_SUPPORT
#include "bchp_dvp_ht.h"
#endif


#ifdef __cplusplus
extern "C" {
#endif


#define	MAKE_INTR_ENUM(IntName)	BHDM_INTR_e##IntName
#define	MAKE_INTR_NAME(IntName)	"BHDM_" #IntName

							  
/******************************************************************************
Summary:
Enumeration of BHDM_Interrupts 
*******************************************************************************/
typedef enum
{
#if BHDM_CONFIG_DUAL_HPD_SUPPORT
	/* 00 */ MAKE_INTR_ENUM(HOTPLUG_REMOVED),
	/* 01 */ MAKE_INTR_ENUM(HOTPLUG_CONNECTED),
#else
	/* 00 */ MAKE_INTR_ENUM(HOTPLUG),
#endif	

	/* 01 */ MAKE_INTR_ENUM(DF_FULL_MINUS),
	/* 02 */ MAKE_INTR_ENUM(DF_ALMOST_FULL), 
	/* 03 */ MAKE_INTR_ENUM(DF_EMPTY_MINUS),
	/* 04 */ MAKE_INTR_ENUM(DF_ALMOST_EMPTY), 

	/* 05 */ MAKE_INTR_ENUM(PKT_WRITE_ERR),
		
	/* 07 */ MAKE_INTR_ENUM(HDCP_REPEATER_ERR),
	/* 08 */ MAKE_INTR_ENUM(HDCP_V_MISMATCH),
	/* 09 */ MAKE_INTR_ENUM(HDCP_V_MATCH),
	/* 10 */ MAKE_INTR_ENUM(HDCP_RI),
	/* 11 */ MAKE_INTR_ENUM(HDCP_AN),
	/* 12 */ MAKE_INTR_ENUM(PKT_OVERFLOW),
	/* 13 */ MAKE_INTR_ENUM(HDCP_PJ),
	/* 14 */ MAKE_INTR_ENUM(MAI_FORMAT_UPDATE),
	
#if BHDM_CONFIG_HDCP_AUTO_RI_PJ_CHECKING_SUPPORT
	/* 15 */ MAKE_INTR_ENUM(HDCP_PJ_MISMATCH),
	/* 16 */ MAKE_INTR_ENUM(HDCP_RI_A_MISMATCH),
	/* 17 */ MAKE_INTR_ENUM(HDCP_RI_B_MISMATCH),
#endif

#if BHDM_CONFIG_RECEIVER_SENSE_SUPPORT
#if !BHDM_CONFIG_RSEN_POLLING
	/* 18 */ MAKE_INTR_ENUM(RSEN),
#endif	
#endif	
	/* 19 */ MAKE_INTR_ENUM(LAST)
} BHDM_P_InterruptMask ;


/******************************************************************************
Summary:
Enumerated Type of pre-configured Video Rates from the VEC to the HDMI core

Description:
The HDMI Rate Manager must be configured to match the input clock to the HDMI
core.  This table enumerates those types

*******************************************************************************/
typedef enum
{
	/* 8bit standard mode */
	BHDM_PixelClock_e25_2			,
	BHDM_PixelClock_e25_2_DIV_1_001 ,
	
	BHDM_PixelClock_e27 			,
	BHDM_PixelClock_e27_MUL_1_001	,

#if BHDM_CONFIG_HDMI_1_3_SUPPORT
	/**************************
	* This entry is specific for 480p format. 
	* Currently, for orthogonal VEC platforms (7420, 7342, 7550, etc.), the VEC always run at 54Mhz for 480p format
	***********************/
	BHDM_PixelClock_e27_480p = BHDM_PixelClock_e27, 
	BHDM_PixelClock_e27_MUL_1_001_480p = BHDM_PixelClock_e27_MUL_1_001,
#endif

	BHDM_PixelClock_e54 			, /* 2 times pixel repetition 2x27 Mhz */
	BHDM_PixelClock_e54_MUL_1_001	,
	
	BHDM_PixelClock_e74_25			,
	BHDM_PixelClock_e74_25_DIV_1_001,

#if BHDM_CONFIG_HDMI_1_3_SUPPORT
	BHDM_PixelClock_e108			 , /* 4 times pixel repetition 4x27 Mhz */
	BHDM_PixelClock_e108_MUL_1_001	 ,

	BHDM_PixelClock_e148_5			,
	BHDM_PixelClock_e148_5_DIV_1_001,


	/* 10bit deep color mode */
	BHDM_PixelClock_e31_5,
	BHDM_PixelClock_e31_5_DIV_1_001,
	
	BHDM_PixelClock_e33_75,
	BHDM_PixelClock_e33_75_MUL_1_001,

	/**************************
	* This entry is specific for 480p format. 
	* Currently, for orthogonal VEC platforms (7420, 7342, 7550, etc.), the VEC always run at 54Mhz for 480p format
	***********************/
	BHDM_PixelClock_e33_75_480p = BHDM_PixelClock_e33_75, /* 27Mhz x 1.25 */
	BHDM_PixelClock_e33_75_MUL_1_001_480p = BHDM_PixelClock_e33_75_MUL_1_001,

	BHDM_PixelClock_e67_5			  , /* 2 times pixel repetition 2x33.75 Mhz */
	BHDM_PixelClock_e67_5_MUL_1_001   ,

	BHDM_PixelClock_e92_8125,
	BHDM_PixelClock_e92_8125_DIV_1_001,

	BHDM_PixelClock_e135			 , /* 4 times pixel repetition 4x33.75 Mhz */
	BHDM_PixelClock_e135_MUL_1_001	 ,

	BHDM_PixelClock_e185_625,
	BHDM_PixelClock_e185_625_DIV_1_001,


	/* 12bit deep color mode */
	BHDM_PixelClock_e37_8,
	BHDM_PixelClock_e37_8_DIV_1_001,

	BHDM_PixelClock_e40_5,
	BHDM_PixelClock_e40_5_MUL_1_001,

	/**************************
	* This entry is specific for 480p format. 
	* Currently, for orthogonal VEC platforms (7420, 7342, 7550, etc.), the VEC always run at 54Mhz for 480p format
	***********************/
	BHDM_PixelClock_e40_5_480p = BHDM_PixelClock_e40_5, 			/* 27Mhz x 1.5 */
	BHDM_PixelClock_e40_5_MUL_1_001_480p = BHDM_PixelClock_e40_5_MUL_1_001,

	BHDM_PixelClock_e81 			, /* 2 times pixel repetition 2x40.5 Mhz */
	BHDM_PixelClock_e81_MUL_1_001	,

	BHDM_PixelClock_e111_375,
	BHDM_PixelClock_e111_375_DIV_1_001,

	BHDM_PixelClock_e162			 , /* 4 times pixel repetition 4x40.5 Mhz */
	BHDM_PixelClock_e162_MUL_1_001	 ,

	BHDM_PixelClock_e222_75,
	BHDM_PixelClock_e222_75_DIV_1_001,
#endif

	/* DVI/PC/custom clock rates */
	
	BHDM_PixelClock_e40,
	BHDM_PixelClock_e65,
	BHDM_PixelClock_e65_DIV_1_001,

	BHDM_PixelClock_e60_375,
	BHDM_PixelClock_e74_375,
	BHDM_PixelClock_e64,

	BHDM_PixelClock_eCUSTOM_1366x768p_50,	/* Custom 1366x768 mode @ 60 */
	BHDM_PixelClock_eCUSTOM_1366x768p_5994, /* Custom 1366x768 mode @ 59.94 */
	BHDM_PixelClock_eCUSTOM_1366x768p_60,	/* Custom 1366x768 mode @ 60 */


	BHDM_PixelClock_eCount
} BHDM_InputPixelClock ;

#define BHDM_PixelClock_eUnused 		BHDM_PixelClock_eCount
#define BHDM_PixelClock_eDviClockRate	BHDM_PixelClock_e54


typedef struct BHDM_EDID_P_VideoDescriptor 
{
	BLST_Q_ENTRY(BHDM_EDID_P_VideoDescriptor ) link ;
	BFMT_VideoFmt eVideoFmt  ; /* BCM Video Format */
	uint8_t VideoIdCode ;      /* CEA-861B Video Id Code */
	uint8_t NativeFormat ;     /* Native Format for Monitor */
} BHDM_EDID_P_VideoDescriptor ;


/* declaration of the head type for Video Descriptor list */
typedef struct BHDM_EDID_VideoDescriptorHead BHDM_EDID_VideoDescriptorHead;
BLST_Q_HEAD(BHDM_EDID_VideoDescriptorHead, BHDM_EDID_P_VideoDescriptor );


typedef struct _BHDM_EDID_DATA_
{
	uint8_t Block[BHDM_EDID_BLOCKSIZE] ;
	uint8_t CachedBlock ;
	
	BHDM_EDID_BasicData	   BasicData ;
	bool BcmMonitorRangeParsed ;
	BHDM_EDID_MonitorRange MonitorRange ;
	uint8_t                MonitorName[BHDM_EDID_DESC_ASCII_STRING_LEN] ;
 	BHDM_EDID_DetailTiming SupportedDetailTimings[2] ; /* keep two most preferred timings */
 	uint8_t                SupportedDetailTimingsIn1stBlock ;
	uint8_t                RxHasHdmiSupport ;
	BHDM_EDID_RxVendorSpecificDB RxVSDB ;
	
	BHDM_EDID_VideoDescriptorHead   VideoDescriptorList ;
	uint8_t	NumBcmSupportedVideoDescriptors;
	uint8_t BcmSupportedVideoIdCodes[BHDM_EDID_MAX_CEA_VIDEO_ID_CODES] ;
	
	uint16_t First16VideoDescriptorsMask;
	
	uint8_t DescriptorHeader[BHDM_EDID_DESC_HEADER_LEN] ;
	
	/* keep track of Broadcom Audio/Video Formats supported by the EDID/monitor */
	bool BcmVideoFormatsChecked ;
	bool UnsupportedVideoFormatReported[BFMT_VideoFmt_eMaxCount] ;
	bool BcmSupportedVideoFormats[BFMT_VideoFmt_eMaxCount] ;

	BHDM_EDID_3D_Structure_ALL BcmSupported3DStructureAll;
	bool Bcm3DFormatsChecked ;
	BHDM_EDID_3D_Structure_ALL BcmSupported3DFormats[BFMT_VideoFmt_eMaxCount] ;
	
	bool BcmAudioFormatsChecked ;
	BHDM_EDID_AudioDescriptor BcmSupportedAudioFormats[BAVC_AudioFormat_eMaxCount] ;

	
	BHDM_EDID_ColorimetryDataBlock ColorimetryData;
	
} BHDM_EDID_DATA ;

typedef enum
{
	BHDM_EDID_STATE_eInvalid,
	BHDM_EDID_STATE_eInitialize,
	BHDM_EDID_STATE_eProcessing,
	BHDM_EDID_STATE_eOK
} BHDM_EDID_STATE;


#if BHDM_CONFIG_BTMR_SUPPORT
/* time units for BTMR which returns microseconds */
#define BHDM_P_MILLISECOND 1000 
#define BHDM_P_SECOND 1000000

typedef enum
{
	BHDM_P_TIMER_eHotPlug,
	BHDM_P_TIMER_eRxSense,
	BHDM_P_TIMER_eFormatDetection

} BHDM_P_TIMER ;
#endif

void BHDM_P_Hotplug_isr(BHDM_Handle hHDMI) ;

/*******************************************************************************
Private HDMI Handle Declaration 
*******************************************************************************/
typedef struct BHDM_P_Handle
{
	BCHP_Handle   hChip ;
	BREG_Handle   hRegister ;
	BINT_Handle   hInterrupt ;
	BREG_I2C_Handle hI2cRegHandle ;
	BINT_CallbackHandle hCallback[MAKE_INTR_ENUM(LAST)] ;

	BKNI_EventHandle BHDM_EventHDCP ;
	BKNI_EventHandle BHDM_EventHDCPRiValue ;
	BKNI_EventHandle BHDM_EventHDCPPjValue ;
	BKNI_EventHandle BHDM_EventHDCPRepeater;	
	BKNI_EventHandle BHDM_EventRxSense ;	
	BKNI_EventHandle BHDM_EventHotPlug ;
	BKNI_EventHandle BHDM_EventRAM ;   /* debugging events */
	BKNI_EventHandle BHDM_EventFIFO ;  /* debugging events */

	BHDM_Settings DeviceSettings ;

	/* moved from Device Settings */	
	BHDM_InputPixelClock    eInputPixelClock ;

	/* selected output port DVO12/DVO24/HDMI ; set once */
	BHDM_OutputPort eOutputPort ;

	uint8_t RxDeviceAttached ;
	bool tmdsEnabled;
	bool AvMuteState ;
	bool AudioMuteState ;
	bool hotplugInterruptFired;
	bool rxSensePowerDetected ;

	uint8_t PacketBytes[BHDM_NUM_PACKET_BYTES] ;

	bool OutputEnabled ; /* true if any TMDS line (clock or data is enabled */
	
	bool standby; /* true if in standby */
	bool enableWakeup; /* true if standby wakeup from CEC is enabled */

    
	/******************/
	/* HDCP variables */
	/******************/
	uint32_t HDCP_RiCount ;
	uint8_t HDCP_PjMismatchCount ;

	uint8_t HDCP_AutoRiMismatch_A;
	uint8_t HDCP_AutoRiMismatch_B;
	uint8_t HDCP_AutoPjMismatch;

	uint16_t 
		HDCP_Ri2SecsAgo, 
		HDCP_Ri4SecsAgo,
		HDCP_Ri6SecsAgo ;

	uint16_t 
		HDCP_TxRi, 
		HDCP_RxRi ;
		
	uint8_t
		HDCP_TxPj,
		HDCP_RxPj ;

	uint8_t HDCP_AuthenticatedLink ;

	BHDM_HDCP_Version
		HdcpVersion ;  /* HDCP Version to Use */

	uint8_t RxBCaps ;
	uint16_t RxStatus ;
	
	/* store copy of Attached KSV and Repeater KSV List */
	uint8_t HDCP_RxKsv[BHDM_HDCP_KSV_LENGTH] ;
	
	uint8_t HDCP_RepeaterDeviceCount ;
	uint8_t *HDCP_RepeaterKsvList ;

	BHDM_HDCP_OPTIONS HdcpOptions  ;
	bool bHdcpAnRequest ;
	bool bAutoRiPjCheckingEnabled  ;
	uint8_t AbortHdcpAuthRequest ;
	
	
	/******************/
	/* EDID variables */
	/******************/
	BHDM_EDID_STATE edidStatus;
	BHDM_EDID_DATA AttachedEDID ;

#if BHDM_CONFIG_PLL_KICKSTART_WORKAROUND
	uint32_t uiPllKickStartCount ;
#endif

	BHDM_CallbackFunc pfHotplugChangeCallback ;
	void *pvHotplugChangeParm1 ;
	int iHotplugChangeParm2 ;

#if BHDM_CONFIG_RECEIVER_SENSE_SUPPORT
	BHDM_CallbackFunc pfRxSenseChangeCallback ;
	void *pvRxSenseChangeParm1 ;
	int iRxSenseChangeParm2 ;
#endif

#if BHDM_CONFIG_BTMR_SUPPORT
	BTMR_Handle hTMR ;
	BTMR_TimerHandle TimerRxSense ;
	BTMR_TimerHandle TimerHotPlug ;
	BTMR_TimerHandle TimerFormatChange ;
	bool TimerFormatInitialChangeReported ;  /* eliminate first message  */
		
#endif

 } BHDM_P_Handle ;


/**********************************
 *	PRIVATE FUNCTIONS 
 **********************************/

/******************************************************************************
Summary:
Handle interrupts from the HDMI core.

Description:
Interrupts received from the HDMI core must be handled.  The following 
is a list of possible interrupts.

	o  HDCP_PJ_MISMATCH_INTR 
	o  HDCP_RI_A_MISMATCH_INTR 
	o  HDCP_RI_B_MISMATCH_INTR 

	o  HDCP_PJ_INTR 

	o  PKT_OVERFLOW_INTR 

	o  HDCP_AN_READY_INTR 
	o  HDCP_RI_INTR 
	o  HDCP_V_MATCH_INTR 
	o  HDCP_V_MISMATCH_INTR 
	o  HDCP_REPEATER_ERR_INTR 

	o  CEC_INTR 

	o  ILLEGAL_WRITE_TO_ACTIVE_RAM_PACKET_INTR 

	o  DRIFT_FIFO_ALMOST_EMPTY_INTR 
	o  DRIFT_FIFO_EMPTY_MINUS_INTR 
	o  DRIFT_FIFO_ALMOST_FULL_INTR 
	o  DRIFT_FIFO_FULL_MINUS_INTR 

	o  HOTPLUG_INTR 

Input:
	pParameter - pointer to interrupt specific information BHDM_Open.

Output:
	<None>
	
Returns:
	<None>

See Also:

*******************************************************************************/
void BHDM_P_HandleInterrupt_isr
(
	void *pParam1,						/* Device channel handle */
	int parm2							/* not used */
) ;
								
#if BHDM_CONFIG_DVO_SUPPORT
BERR_Code BHDM_DVO_P_EnableDvoPort(
	BHDM_Handle hHDMI,              /* [in] HDMI handle */
	BHDM_OutputFormat eOutputFormat /* [in] format to use on Output Port */
) ;
#endif

void BHDM_P_ConfigureInputAudioFmt(
	BHDM_Handle hHDMI,							/* [in] HDMI handle */
	BAVC_HDMI_AudioInfoFrame *stAudioInfoFrame	/* [in] audio Info Frame settings */
) ;

BERR_Code BHDM_P_WritePacket(
	BHDM_Handle hHDMI, 
	BHDM_Packet PhysicalHdmiRamPacketId,
	uint8_t PacketType, 
	uint8_t PacketVersion, 
	uint8_t PacketLength, 
	uint8_t *PacketBytes
) ;

void BHDM_P_VideoFmt2CEA861Code(
	BFMT_VideoFmt eVideoFmt, 
	BFMT_AspectRatio eAspectRatio, 
	BAVC_HDMI_PixelRepetition ePixelRepetition, 
	uint8_t *VideoID
) ;


#if BHDM_CONFIG_HDMI_1_3_SUPPORT
BERR_Code BHDM_P_SetGamutMetadataPacket(
	BHDM_Handle hHDMI		/* [in] HDMI Handle */
) ;

BERR_Code BHDM_P_ConfigurePhy(
	BHDM_Handle hHDMI, 				/* [in] HDMI handle */
	BHDM_Settings *NewHdmiSettings	/* [in] New HDMI settings */
);
#endif

void BHDM_P_ResetHdmiCore (BHDM_Handle hHDMI);

void BHDM_P_PowerOnPhy (BHDM_Handle hHDMI);
void BHDM_P_PowerOffPhy (BHDM_Handle hHDMI);

void BHDM_P_SetPreEmphasisMode (
	BHDM_Handle hHDMI, 
	uint8_t uValue, 
	uint8_t uDriverAmp
);

BERR_Code BHDM_P_GetPreEmphasisConfiguration (
	BHDM_Handle hHDMI,
	BHDM_PreEmphasis_Configuration *stPreEmphasisConfig
);


BERR_Code BHDM_P_SetPreEmphasisConfiguration(
	BHDM_Handle hHDMI,
	BHDM_PreEmphasis_Configuration *stPreEmphasisConfig
);

void BHDM_P_GetReceiverSense(
	BHDM_Handle hHDMI, 
	uint8_t *ReceiverSense
) ;

void BHDM_P_ClearHotPlugInterrupt(
   BHDM_Handle hHDMI		/* [in] HDMI handle */
);

void BHDM_P_CheckHotPlugInterrupt(
	BHDM_Handle hHDMI,		 /* [in] HDMI handle */
	uint8_t *bHotPlugInterrupt	/* [out] Interrupt asserted or not */
);

void BHDM_P_RxDeviceAttached_isr(
	BHDM_Handle hHDMI,		 /* [in] HDMI handle */
	uint8_t *bDeviceAttached	/* [out] Device Attached Status  */
) ;

#if BHDM_CONFIG_DEBUG_FIFO
BERR_Code BHDM_P_EnableFIFOInterrupts(
	BHDM_Handle hHDMI, bool on) ;
#endif

typedef struct _BHDM_P_FIFO_DATA_
{
	uint8_t uiRdAddr,   uiWrAddr ;
	uint8_t uiFullness ;
	bool bUnderFlow, bOverFlow ;
} BHDM_P_FIFO_DATA ;

void BHDM_P_CaptureFIFOData(BHDM_Handle hHDMI, BHDM_P_FIFO_DATA *FifoData) ;

void BHDM_P_EnableTmdsOutput_isr(
	BHDM_Handle hHDMI, bool bEnableTmdsOutput) ;


#ifdef __cplusplus
}
#endif
 
#endif /* BHDM_PRIV_H__ */
/* end bhdm_priv.h */

