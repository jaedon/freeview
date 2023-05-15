/***************************************************************************
*	  (c)2004-2010 Broadcom Corporation
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
*  1.	  This program, including its structure, sequence and organization, constitutes the valuable trade
*  secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
*  and to use this information only in connection with your use of Broadcom integrated circuit products.
*
*  2.	  TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
*  AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
*  WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
*  THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
*  OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
*  LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
*  OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
*  USE OR PERFORMANCE OF THE SOFTWARE.
*
*  3.	  TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
*  LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
*  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
*  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
*  THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
*  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
*  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
*  ANY LIMITED REMEDY.
 *
 * $brcm_Workfile: bhdm_priv.c $
 * $brcm_Revision: Hydra_Software_Devel/52 $
 * $brcm_Date: 9/10/12 1:42p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/hdm/src/core40nm/bhdm_priv.c $
 * 
 * Hydra_Software_Devel/59   8/6/13 6:00p rgreen
 * SW7429-710,SW7563-123: Implement channel swaps only for chips that
 * require it
 * 
 * Hydra_Software_Devel/58   8/2/13 2:21p rgreen
 * SW7429-710: Some 40nm chips require swapping of the HDMI_TX_PHY channel
 * settings
 * 
 * Hydra_Software_Devel/52   9/10/12 1:42p rgreen
 * SW7420-2349: Update BHDM_P_RxDeviceAttaced to isr function.  Update
 * calls to add Enter/Leave Critical Section
 * 
 * Hydra_Software_Devel/51   8/24/12 5:03p rgreen
 * SW7425-3803: Remove debug message
 * 
 * Hydra_Software_Devel/50   8/24/12 5:01p rgreen
 * SW7425-3803: Add Phy PowerDown for standby/close
 * 
 * Hydra_Software_Devel/49   6/14/12 10:44a rgreen
 * SW7425-3206, SW7425-3241:  Enable/Disable RSEN interrupts to match
 * state of the TMDS lines
 * 
 * Hydra_Software_Devel/48   5/7/12 7:53p rgreen
 * SW7422-467,SW7231-797,SW7358-263: Merge changes
 * 
 * Hydra_Software_Devel/SW7422-467/1   5/2/12 10:03p rgreen
 * SW7422-467,SW7231-797,SW7358-263: Remove enabling of TMDS lines at
 * BHDM_Open, enable when data is first output.   When disabling TMDS
 * lines, keep RSEN interrupt enabled and the TMDS Clock enabled for
 * detecting Receiver Power changes
 * 
 * Hydra_Software_Devel/47   4/24/12 2:41p rgreen
 * SW7425-2075: Merge Changes
 * 
 * Hydra_Software_Devel/SW7425-2075/1   4/12/12 11:58a jgarrett
 * SW7425-2075: Removing MAI Channel Reordering in HBR mode
 * 
 * Hydra_Software_Devel/45   4/3/12 11:21a rgreen
 * SW7425-2818,SW7552-163:  Always keep TMDS clock channel enabled to
 * ensure detecting ReceiverSense if attached Rx powers down just the
 * data channels when in standby
 * 
 * Hydra_Software_Devel/44   3/29/12 1:49p vle
 * SWBLURAY-28263: Merge to mainline. Fix HDMI Phy settings to ensure no
 * failure on HDMI Eye pattern for 480p.
 * 
 * Hydra_Software_Devel/43   3/9/12 6:52p vle
 * SW7435-44: Update HDMI_TX_PHY channel swap settings for all 40nm
 * platforms. Future platforms relies on the default values
 * 
 * Hydra_Software_Devel/42	 3/7/12 11:00a rgreen
 * SW7425-2515: Remove CEC code; see bcec pi
 * 
 * Hydra_Software_Devel/42	 3/7/12 10:52a rgreen
 * SW7425-2515: Remove CEC code; see bcec pi
 * 
 * Hydra_Software_Devel/41   2/9/12 3:59p rgreen
 * SW7231-345,SW7125-1146,SW7425-2361: Refactor HDMI Power Management;
 * separate TMDS power from clock
 * 
 * Hydra_Software_Devel/40   1/27/12 2:10p vle
 * SW7125-1146: merge to mainline
 * 
 * Hydra_Software_Devel/SW7125-1146/1   1/26/12 5:35p vle
 * SW7125-1146: Get RSEN setting at isr vs event time for applicable
 * platforms
 * 
 * Hydra_Software_Devel/39   1/23/12 4:46p rgreen
 * SW7125-1146, SW7231-345,SW7346-234: Remove enable/disable of RSEN
 * interrupt from EnableTmdsOutput; Get RSEN setting at isr vs event time
 * 
 * Hydra_Software_Devel/38   1/23/12 1:31p rgreen
 * SW7125-1146: Add BHDM_GetReceiverSense_isr support for 40nm
 * 
 * Hydra_Software_Devel/bdvd_v4.0/2   3/14/12 4:20p rbshah
 * SWBLURAY-28263:[ see Broadcom Issue Tracking JIRA for more info ]
 * 
 * Hydra_Software_Devel/bdvd_v4.0/1   1/17/12 1:02p rbshah
 * SWBLURAY-28113:[ see Broadcom Issue Tracking JIRA for more info ]
 * 
 * Hydra_Software_Devel/37   1/6/12 6:03p vle
 * SW7435-11: implement support for check/clearHotplugInterrupt for 7435
 * 
 * Hydra_Software_Devel/36   1/6/12 5:06p vle
 * SW7425-1890: Update HDMI pre_emphasis settings for 480p/576p 12bit deep
 * color with 4x pixel repitition
 * 
 * Hydra_Software_Devel/35   1/6/12 2:59p vle
 * SW7435-11: Add support for 7435
 * 
 * Hydra_Software_Devel/34   1/3/12 3:27p vle
 * SW7358-203: Merged to mainline.
 * 
 * Hydra_Software_Devel/33   12/22/11 3:35p vle
 * SW7425-1890: Update HDMI pre_emphasis settings based on output format.
 * 
 * Hydra_Software_Devel/bdvd_v4.0/2   12/19/11 1:52p rbshah
 * SWBLURAY-26245:[ see Broadcom Issue Tracking JIRA for more info ]
 * 
 * Hydra_Software_Devel/32   12/8/11 4:40p rgreen
 * SW7358-194: use BCHP_PWR_RESOURCE_HDMI_TX macro consistently
 * 
 * Hydra_Software_Devel/31   12/8/11 4:29p rgreen
 * SW7358-194: After checking RxSense, disable TMDS lines if previously
 * disabled
 * 
 * Hydra_Software_Devel/30   11/22/11 6:02p vle
 * SW7425-1140: Merge to mainline.  Remove all CEC functionality out of
 * HDM PI.
 * 
 * Hydra_Software_Devel/SW7425-1140/2   11/22/11 5:48p vle
 * SW7425-1140: Add BHDM_CONFIG_CEC_LEGACY_SUPPORT for backward compatible
 * for CEC legacy platforms.
 * 
 * Hydra_Software_Devel/SW7425-1140/1   11/16/11 12:17p vle
 * SW7425-1140: Remove all CEC functionalities out of HDM PI
 * 
 * Hydra_Software_Devel/29	 11/14/11 2:16p rgreen
 * SW7425-1710: Update BHDM_CONFIG macro usage.  Describe specific
 * functionality vs chip process
 * 
 * Hydra_Software_Devel/28	 10/28/11 1:20p rgreen
 * SW7425-1629: Update hardware  generation of GCP packet
 * 
 * Hydra_Software_Devel/27	 10/3/11 5:15p rgreen
 * SW7425-158: Update internal status of TMDS lines to avoid unnecessary
 * warning for mute/unmute when TMDS is powered down
 * 
 * Hydra_Software_Devel/26   8/26/11 5:09p vle
 * SW7231-322: Add support for 7231B0
 * 
 * Hydra_Software_Devel/25   8/23/11 5:16p vle
 * SW7231-345: Disable RxSense interrupt when TDMS lines are disabled.
 * 
 * Hydra_Software_Devel/24   8/17/11 6:38p vle
 * SW7231-322: Add support for 7231B0
 * 
 * Hydra_Software_Devel/23   8/9/11 11:38a vle
 * SWBLURAY-26743: Merge to mainline
 * 
 * Hydra_Software_Devel/SWBLURAY-26743/1   8/8/11 4:58p rbshah
 * SWBLURAY-26743:[ see Broadcom Issue Tracking JIRA for more info ]
 * 
 * Hydra_Software_Devel/22   7/27/11 5:57p vle
 * SW7346-348: Merge to main line. Do not toggle TMDS lines when checking
 * RxSense.
 * 
 * Hydra_Software_Devel/21   7/26/11 3:55p vle
 * SW7346-319: Make sure HDMI_TX_PHY PLL is not in reset
 * 
 * Hydra_Software_Devel/SW7346-348/1   7/22/11 4:09p vle
 * SW7346-348: back out changes in JIRA SW7408-290
 * 
 * Hydra_Software_Devel/20   7/15/11 5:12p vle
 * SW7422-462: Update N & CTS values for 32 and 44.1Khz, 148.5/1.001Mhz
 * pixel clock, 12bit deep color
 * 
 * Hydra_Software_Devel/19   7/1/11 11:21a vle
 * SW7422-459: fix indent
 * 
 * Hydra_Software_Devel/18   7/1/11 10:48a vle
 * SW7422-459: merge bdvd changes to mainline
 * 
 * Hydra_Software_Devel/bdvd_v4.0/3   6/30/11 4:06p rbshah
 * SWBLURAY-26245:[ see Broadcom Issue Tracking JIRA for more info ]
 * 
 * Hydra_Software_Devel/17   6/28/11 5:58p vle
 * SW7346-234: Merge to mainline
 * 
 * Hydra_Software_Devel/SW7346-234/2   6/28/11 5:56p vle
 * SW7346-234: Disable RSEN interrupt until the PHY is powered on. TAKE 2
 * 
 * Hydra_Software_Devel/SW7346-234/1   6/24/11 6:07p vle
 * SW7346-234: Disable RSEN interrupt until the PHY is powered on.
 * 
 * Hydra_Software_Devel/15   6/24/11 1:54p vle
 * SW7408-290: Make sure to disable TMDS lines appropriately when checking
 * for receiver sense.
 * 
 * Hydra_Software_Devel/14   6/13/11 6:08p vle
 * SW7231-147: Tweak Pre-emphasis settings for some 40nm A0 platforms to
 * fix HDMI eye diagram issue in 1080p 8bit color depth
 * 
 * Hydra_Software_Devel/13   5/6/11 1:12p vle
 * SW7231-128: Add power management support for 40nm platforms
 * 
 * Hydra_Software_Devel/12   4/26/11 3:32p vle
 * SW7125-798: Remove unused variable - fix compiler warning
 * 
 * Hydra_Software_Devel/11   4/25/11 7:31p vle
 * SW7125-798: fix no hdmi output issue on 7425/7231
 * 
 * Hydra_Software_Devel/10   4/22/11 11:37a vle
 * SW7125-798: Add API to allow customer to control pre-emphasis settings
 * 
 * Hydra_Software_Devel/9   3/21/11 7:08p vle
 * SW7550-664: Fix HDMI 4.1 output channel order
 * 
 * Hydra_Software_Devel/8   12/22/10 11:40a vle
 * SW7344-11: Configure HDMI_TX_PHY.CHANNEL_SWAP for 40nm platforms. Not
 * all chips can use the default values.
 * 
 * Hydra_Software_Devel/7   12/21/10 3:34p vle
 * SW7422-130: Merge to main branch
 * 
 * Hydra_Software_Devel/SW7422-130/1   12/20/10 7:46p vle
 * SW7422-130: When configure HDMI in master mode, do not RECENTER fifo
 * and USE_FULL has to be set to 0
 * 
 * Hydra_Software_Devel/6   12/17/10 2:36p vle
 * SW7344-11: Update support for 7344/7346 and all other 40nm platform due
 * to RDB update
 * 
 * Hydra_Software_Devel/5   11/24/10 11:13a vle
 * SW7422-97: Update HDMI PHY Settings for 40nm core.
 * 
 * Hydra_Software_Devel/4   11/11/10 2:08p vle
 * SW7422-23: TMDS lines need to be powered on for RxSense to work.
 * 
 * Hydra_Software_Devel/3   11/11/10 11:07a vle
 * SW7422-12: SW_INIT register doesn't require atomic access Power on PHY,
 * default is powered down.
 * 
 * Hydra_Software_Devel/3   11/11/10 11:05a vle
 * SW7422-12: SW_INIT register doesn't require Atomic access. Power on PHY
 * 
 * Hydra_Software_Devel/2   9/29/10 4:14p vle
 * SW7422-23: Fix build issues without CEC enable
 * 
 * Hydra_Software_Devel/1   9/28/10 7:25p vle
 * SW7422-23: Add support for 7422
 * 
 ***************************************************************************/

#include "bhdm.h"
#include "bhdm_priv.h"

BDBG_MODULE(BHDM_PRIV) ;

#define BHDM_CHECK_RC( rc, func )			  \
do												  \
{												  \
	if( (rc = BERR_TRACE(func)) != BERR_SUCCESS ) \
	{											  \
		goto done;								  \
	}											  \
} while(0)


/******************************************************************************
Summary:
HDMI Pixel Clock Rate Text - Useful for debug messages
NOTE: These entries match the number of entries in the BHDM_RM_VideoRates Table
*******************************************************************************/
#if BDBG_DEBUG_BUILD
const char *BHDM_PixelClockText[BHDM_PixelClock_eCount] =
{
	/* 8bit standard mode */
	"25.2", 					
	"25.2 / 1.001",

	"27",
	"27 * 1.001",

	"54",
	"54 * 1.001",
				
	"74.25",
	"74.25 / 1.001",

	"108",
	"108 * 1.001",

	"148.5",
	"148.5 / 1.001",

	
	/* 10bit deep color mode */
	"31.5", 					
	"31.5 / 1.001",

	"33.75",
	"33.75 * 1.001",

	"67.5",
	"67.5 * 1.001",
				
	"92.8125",
	"92.8125 / 1.001",

	"135",
	"135 * 1.001",

	"185.625",
	"185.625 / 1.001",
	

	/* 12bit deep color mode */
	"37.8", 					
	"37.8 / 1.001",
	
	"40.5",
	"40.5 * 1.001",
				
	"81",
	"81 * 1.001",

	"111.375",
	"111.375 / 1.001",

	"162",
	"162 * 1.001",
	
	"222.75",
	"222.75 / 1.001",


	/* PC/DVI/custom clock rates */	
	"40.000000",
	"65.000000",
	"64.935065",

	"60.375000",
	"74.375000",
	"64.000000",

	"56.304000",
	"67.4973027",
	"67.565"
	
} ;
#endif
	

/* HDMI Rate Manager now updated by VDC for smoother transitions */
/******************************************************************************
Summary:
HDMI Audio Clock Capture and Regeneration Definition (See HDMI Spec Section 7)
NOTE: These entries match the number of entries in the BHDM_RM_VideoRates Table
*******************************************************************************/
typedef struct _BHDM_P_AUDIO_CLK_VALUES_
{
	BHDM_InputPixelClock eInputPixelClock;
	BAVC_HDMI_BitsPerPixel eBitsPerPixel;
	uint32_t NValue ;
	uint32_t HW_NValue ;
	uint32_t CTS_0 ;
	uint32_t CTS_1 ;
	uint32_t CTS_PERIOD_0;
	uint32_t CTS_PERIOD_1;
	uint32_t CTS_0_REPEAT;
	uint32_t CTS_1_REPEAT;
} BHDM_P_AUDIO_CLK_VALUES ;


/******************************************************************************
Summary:
HDMI Audio Clock Capture and Regeneration Values for 32kHz
NOTE: These entries match the number of entries in the BHDM_RM_VideoRates Table
*******************************************************************************/
static const BHDM_P_AUDIO_CLK_VALUES  BHDM_32KHz_AudioClkValues[] =
{	/*		 InputPixelClock				BitsPerPixel						SW-N	HW-N	CTS_0	 CTS_1	   PERIOD	REPEAT */

	/******* 8bit standard mode ********/
	{ BHDM_PixelClock_e25_2,			BAVC_HDMI_BitsPerPixel_e24bit,	 4096,	 4096,	 25200,   25200,   32,	 32,  1,  1 },
	{ BHDM_PixelClock_e25_2_DIV_1_001,	BAVC_HDMI_BitsPerPixel_e24bit,	 4576,	 4096,	 28125,   28125,   35,	 36,  1,  3 },

	{ BHDM_PixelClock_e27,				BAVC_HDMI_BitsPerPixel_e24bit,	 4096,	 4096,	 27000,   27000,   32,	 32,  1,  1 },
	{ BHDM_PixelClock_e27_MUL_1_001,	BAVC_HDMI_BitsPerPixel_e24bit,	 4096,	 4096,	 27027,   27027,   32,	 32,  1,  1 },

	{ BHDM_PixelClock_e54,				BAVC_HDMI_BitsPerPixel_e24bit,	 4096,	 4096,	 54000,   54000,   32,	 32,  1,  1 },
	{ BHDM_PixelClock_e54_MUL_1_001,	BAVC_HDMI_BitsPerPixel_e24bit,	 4096,	 4096,	 54054,   54054,   32,	 32,  1,  1 },

	{ BHDM_PixelClock_e74_25,			BAVC_HDMI_BitsPerPixel_e24bit,	 4096,	 4096,	 74250,   74250,   32,	 32,  1,  1 },
	{ BHDM_PixelClock_e74_25_DIV_1_001, BAVC_HDMI_BitsPerPixel_e24bit,	11648,	11648,	210937,  210938,   91,	 91,  1,  1 },

	{ BHDM_PixelClock_e108, 			BAVC_HDMI_BitsPerPixel_e24bit,	 4096,	 4096,	108000,  108000,   32,	 32,  1,  1 },
	{ BHDM_PixelClock_e108_MUL_1_001,	BAVC_HDMI_BitsPerPixel_e24bit,	 4096,	 4096,	108108,  108108,   32,	 32,  1,  1 },

	{ BHDM_PixelClock_e148_5,			BAVC_HDMI_BitsPerPixel_e24bit,	 4096,	 4096,	148500,  148500,   32,	 32,  1,  1 },
	{ BHDM_PixelClock_e148_5_DIV_1_001, BAVC_HDMI_BitsPerPixel_e24bit,	11648,	11648,	421875,  421875,   91,	 91,  1,  1 },


	/******** 10bit deep color mode *********/
	{ BHDM_PixelClock_e25_2,			BAVC_HDMI_BitsPerPixel_e30bit,	 4096,	 4096,	 31500,   31500,   32,	 32,  1,  1 },
	{ BHDM_PixelClock_e25_2_DIV_1_001,	BAVC_HDMI_BitsPerPixel_e30bit,	 4096,	 4096,	 31468,   31469,   32,	 32,  1,  1 },

	{ BHDM_PixelClock_e27,				BAVC_HDMI_BitsPerPixel_e30bit,	 4096,	 4096,	 33750,   33750,   32,	 32,  1,  1 },
	{ BHDM_PixelClock_e27_MUL_1_001,	BAVC_HDMI_BitsPerPixel_e30bit,	 8192,	 8192,	 67567,   67568,   64,	 64,  1,  1 },

	{ BHDM_PixelClock_e54,				BAVC_HDMI_BitsPerPixel_e30bit,	 4096,	 4096,	 67500,   67500,   32,	 32,  1,  1 },
	{ BHDM_PixelClock_e54_MUL_1_001,	BAVC_HDMI_BitsPerPixel_e30bit,	 4096,	 4096,	 67567,   67568,   32,	 32,  1,  1 },

	{ BHDM_PixelClock_e74_25,			BAVC_HDMI_BitsPerPixel_e30bit,	 4096,	 4096,	 92812,   92813,   32,	 32,  1,  1 },
	{ BHDM_PixelClock_e74_25_DIV_1_001, BAVC_HDMI_BitsPerPixel_e30bit,	11648,	11648,	263671,  263672,   91,	 91,  1,  9 },

	{ BHDM_PixelClock_e108, 			BAVC_HDMI_BitsPerPixel_e30bit,	 4096,	 4096,	135000,  135000,   32,	 32,  1,  1 },
	{ BHDM_PixelClock_e108_MUL_1_001,	BAVC_HDMI_BitsPerPixel_e30bit,	 4096,	 4096,	135135,  135135,   32,	 32,  1,  1 },

	{ BHDM_PixelClock_e148_5,			BAVC_HDMI_BitsPerPixel_e30bit,	 4096,	 4096,	185625,  185625,   32,	 32,  1,  1 },
	{ BHDM_PixelClock_e148_5_DIV_1_001, BAVC_HDMI_BitsPerPixel_e30bit,	11648,	11648,	527343,  527344,   91,	 91,  1,  4 },


	/******** 12bit deep color mode *********/
	{ BHDM_PixelClock_e25_2,			BAVC_HDMI_BitsPerPixel_e36bit,	 4096,	 4096,	 37800,   37800,   32,	 32,  1,  1 },
	{ BHDM_PixelClock_e25_2_DIV_1_001,	BAVC_HDMI_BitsPerPixel_e36bit,	 9152,	 4096,	 84375,   84375,   71,	 72,  1,  1 },

	{ BHDM_PixelClock_e27,				BAVC_HDMI_BitsPerPixel_e36bit,	 4096,	 4096,	 40500,   40500,   32,	 32,  1,  1 },
	{ BHDM_PixelClock_e27_MUL_1_001,	BAVC_HDMI_BitsPerPixel_e36bit,	 8192,	 8192,	 81081,   81081,   64,	 64,  1,  1 },

	{ BHDM_PixelClock_e54,				BAVC_HDMI_BitsPerPixel_e36bit,	 4096,	 4096,	 81000,   81000,   32,	 32,  1,  1 },
	{ BHDM_PixelClock_e54_MUL_1_001,	BAVC_HDMI_BitsPerPixel_e36bit,	 4096,	 4096,	 81081,   81081,   32,	 32,  1,  1 },

	{ BHDM_PixelClock_e74_25,			BAVC_HDMI_BitsPerPixel_e36bit,	 4096,	 4096,	111375,  111375,   32,	 32,  1,  1 },
	{ BHDM_PixelClock_e74_25_DIV_1_001, BAVC_HDMI_BitsPerPixel_e36bit,	11648,	11648,	316406,  316407,   91,	 91,  7,  3 },

	{ BHDM_PixelClock_e108, 			BAVC_HDMI_BitsPerPixel_e36bit,	 4096,	 4096,	162000,  162000,   32,	 32,  1,  1 },
	{ BHDM_PixelClock_e108_MUL_1_001,	BAVC_HDMI_BitsPerPixel_e36bit,	 4096,	 4096,	162162,  162162,   32,	 32,  1,  1 },

	{ BHDM_PixelClock_e148_5,			BAVC_HDMI_BitsPerPixel_e36bit,	 4096,	 4096,	222750,  222750,   32,	 32,  1,  1 },
	{ BHDM_PixelClock_e148_5_DIV_1_001, BAVC_HDMI_BitsPerPixel_e36bit,	 4096,	 4096,	222527,  222528,   32,	 32,  1,  1 },


	/* PC/custom formats */
	{ BHDM_PixelClock_e40,             	BAVC_HDMI_BitsPerPixel_e24bit,	 4096,	 4096,	 40000,	  40000,   32,   32,  1,  1 },
	{ BHDM_PixelClock_e65,             	BAVC_HDMI_BitsPerPixel_e24bit,	 4096,	 4096,	 65000,	  65000,   32,   32,  1,  1 },
	{ BHDM_PixelClock_e65_DIV_1_001,	BAVC_HDMI_BitsPerPixel_e24bit,	 4096,	 4096,	 64935,	  64936,   32,   32,  9,  1 },
} ;


/******************************************************************************
Summary:
HDMI Audio Clock Capture and Regeneration Values for 44.1kHz
NOTE: These entries match the number of entries in the BHDM_RM_VideoRates Table
*******************************************************************************/
static const BHDM_P_AUDIO_CLK_VALUES BHDM_44_1KHz_AudioClkValues[] =
{	/*		 InputPixelClock				BitsPerPixel						SW-N	HW-N	CTS_0	 CTS_1	   PERIOD	REPEAT */

	/******** 8bit standard mode *********/
	{ BHDM_PixelClock_e25_2,			BAVC_HDMI_BitsPerPixel_e24bit,	 6272,	 6272,	 28000,   28000,   49,	 49,  1,  1 },
	{ BHDM_PixelClock_e25_2_DIV_1_001,	BAVC_HDMI_BitsPerPixel_e24bit,	 7007,	 6272,	 31250,   31250,   54,	 55, 33, 95 },

	{ BHDM_PixelClock_e27,				BAVC_HDMI_BitsPerPixel_e24bit,	 6272,	 6272,	 30000,   30000,   49,	 49,  1,  1 },
	{ BHDM_PixelClock_e27_MUL_1_001,	BAVC_HDMI_BitsPerPixel_e24bit,	 6272,	 6272,	 30030,   30030,   49,	 49,  1,  1 },

	{ BHDM_PixelClock_e54,				BAVC_HDMI_BitsPerPixel_e24bit,	 6272,	 6272,	 60000,   60000,   49,	 49,  1,  1 },
	{ BHDM_PixelClock_e54_MUL_1_001,	BAVC_HDMI_BitsPerPixel_e24bit,	 6272,	 6272,	 60060,   60060,   49,	 49,  1,  1 },

	{ BHDM_PixelClock_e74_25,			BAVC_HDMI_BitsPerPixel_e24bit,	 6272,	 6272,	 82500,   82500,   49,	 49,  1,  1 },
	{ BHDM_PixelClock_e74_25_DIV_1_001, BAVC_HDMI_BitsPerPixel_e24bit,	17836,	 6272,	234375,  234375,  139,	140, 21, 11 },

	{ BHDM_PixelClock_e108, 			BAVC_HDMI_BitsPerPixel_e24bit,	 6272,	 6272,	120000,  120000,   49,	 49,  1,  1 },
	{ BHDM_PixelClock_e108_MUL_1_001,	BAVC_HDMI_BitsPerPixel_e24bit,	 6272,	 6272,	120120,  120120,   49,	 49,  1,  1 },

	{ BHDM_PixelClock_e148_5,			BAVC_HDMI_BitsPerPixel_e24bit,	 6272,	 6272,	165000,  165000,   49,	 49,  1,  1 },
	{ BHDM_PixelClock_e148_5_DIV_1_001, BAVC_HDMI_BitsPerPixel_e24bit,	 8918,	 6272,	234375,  234375,   69,	 70, 21, 43 },


	/******** 10bit deep color mode *********/
	{ BHDM_PixelClock_e25_2,			BAVC_HDMI_BitsPerPixel_e30bit,	 6272,	 6272,	 35000,   35000,   49,	 49,  1,  1 },
	{ BHDM_PixelClock_e25_2_DIV_1_001,	BAVC_HDMI_BitsPerPixel_e30bit,	 6272,	 6272,	 34965,   34965,   49,	 49,  1,  1 },

	{ BHDM_PixelClock_e27,				BAVC_HDMI_BitsPerPixel_e30bit,	 6272,	 6272,	 37500,   37500,   49,	 49,  1,  1 },
	{ BHDM_PixelClock_e27_MUL_1_001,	BAVC_HDMI_BitsPerPixel_e30bit,	 6272,	 6272,	 37537,   37538,   49,	 49,  1,  1 },

	{ BHDM_PixelClock_e54,				BAVC_HDMI_BitsPerPixel_e30bit,	 6272,	 6272,	 75000,   75000,   49,	 49,  1,  1 },
	{ BHDM_PixelClock_e54_MUL_1_001,	BAVC_HDMI_BitsPerPixel_e30bit,	 6272,	 6272,	 75075,   75075,   49,	 49,  1,  1 },

	{ BHDM_PixelClock_e74_25,			BAVC_HDMI_BitsPerPixel_e30bit,	 6272,	 6272,	103125,  103125,   49,	 49,  1,  1 },
	{ BHDM_PixelClock_e74_25_DIV_1_001, BAVC_HDMI_BitsPerPixel_e30bit,	 6272,	 6272,	103022,  103022,   49,	 49,  1,  1 },

	{ BHDM_PixelClock_e108, 			BAVC_HDMI_BitsPerPixel_e30bit,	 6272,	 6272,	150000,  150000,   49,	 49,  1,  1 },
	{ BHDM_PixelClock_e108_MUL_1_001,	BAVC_HDMI_BitsPerPixel_e30bit,	 6272,	 6272,	150150,  150150,   49,	 49,  1,  1 },

	{ BHDM_PixelClock_e148_5,			BAVC_HDMI_BitsPerPixel_e30bit,	 6272,	 6272,	206250,  206250,   49,	 49,  1,  1 },
	{ BHDM_PixelClock_e148_5_DIV_1_001, BAVC_HDMI_BitsPerPixel_e30bit,	 6272,	 6272,	206044,  206044,   49,	 49,  1,  1 },


	/******** 12bit deep color mode *********/
	{ BHDM_PixelClock_e25_2,			BAVC_HDMI_BitsPerPixel_e36bit,	 6272,	 6272,	 42000,   42000,   49,	 49,  1,  1 },
	{ BHDM_PixelClock_e25_2_DIV_1_001,	BAVC_HDMI_BitsPerPixel_e36bit,	 7007,	 6272,	 46875,   46875,   54,	 55, 33, 95 },

	{ BHDM_PixelClock_e27,				BAVC_HDMI_BitsPerPixel_e36bit,	 6272,	 6272,	 45000,   45000,   49,	 49,  1,  1 },
	{ BHDM_PixelClock_e27_MUL_1_001,	BAVC_HDMI_BitsPerPixel_e36bit,	 6272,	 6272,	 45045,   45045,   49,	 49,  1,  1 },

	{ BHDM_PixelClock_e54,				BAVC_HDMI_BitsPerPixel_e36bit,	 6272,	 6272,	 90000,   90000,   49,	 49,  1,  1 },
	{ BHDM_PixelClock_e54_MUL_1_001,	BAVC_HDMI_BitsPerPixel_e36bit,	 6272,	 6272,	 90090,   90090,   49,	 49,  1,  1 },

	{ BHDM_PixelClock_e74_25,			BAVC_HDMI_BitsPerPixel_e36bit,	 6272,	 6272,	123750,  123750,   49,	 49,  1,  1 },
	{ BHDM_PixelClock_e74_25_DIV_1_001, BAVC_HDMI_BitsPerPixel_e36bit,	 6272,	 6272,	123626,  123627,   49,	 49,  3,  2 },

	{ BHDM_PixelClock_e108, 			BAVC_HDMI_BitsPerPixel_e36bit,	 6272,	 6272,	180000,  180000,   49,	 49,  1,  1 },
	{ BHDM_PixelClock_e108_MUL_1_001,	BAVC_HDMI_BitsPerPixel_e36bit,	 6272,	 6272,	180180,  180180,   49,	 49,  1,  1 },

	{ BHDM_PixelClock_e148_5,			BAVC_HDMI_BitsPerPixel_e36bit,	 6272,	 6272,	247500,  247500,   49,	 49,  1,  1 },
	{ BHDM_PixelClock_e148_5_DIV_1_001, BAVC_HDMI_BitsPerPixel_e36bit,	 6272,	 6272,	247252,  247253,   49,	 49,  3,  7 },


	/* PC/custom formats */
	{ BHDM_PixelClock_e40,				BAVC_HDMI_BitsPerPixel_e24bit,	 6272,	 6272,	 44444,   44445,   49,	 49,  6,  4 },
	{ BHDM_PixelClock_e65,				BAVC_HDMI_BitsPerPixel_e24bit,	 6272,	 6272,	 72222,   72223,   49,	 49,  8,  2 },
	{ BHDM_PixelClock_e65_DIV_1_001,	BAVC_HDMI_BitsPerPixel_e24bit,	 6272,	 6272,	 72150,   72151,   49,	 49,  9,  1 },
} ;


/******************************************************************************
Summary:
HDMI Audio Clock Capture and Regeneration Values for 48kHz
NOTE: These entries match the number of entries in the BHDM_RM_VideoRates Table
*******************************************************************************/
static const BHDM_P_AUDIO_CLK_VALUES BHDM_48KHz_AudioClkValues[] =
{	/*		 InputPixelClock				BitsPerPixel						SW-N	HW-N	CTS_0	 CTS_1	   PERIOD	REPEAT */

	/******** 8bit standard mode *********/
	{ BHDM_PixelClock_e25_2,			BAVC_HDMI_BitsPerPixel_e24bit,	 6144,	 6144,	 25200,   25200,   48,	 48,  1,  1 },
	{ BHDM_PixelClock_e25_2_DIV_1_001,	BAVC_HDMI_BitsPerPixel_e24bit,	 6864,	 6144,	 28125,   28125,   53,	 54,  3,  5 },

	{ BHDM_PixelClock_e27,				BAVC_HDMI_BitsPerPixel_e24bit,	 6144,	 6144,	 27000,   27000,   48,	 48,  1,  1 },
	{ BHDM_PixelClock_e27_MUL_1_001,	BAVC_HDMI_BitsPerPixel_e24bit,	 6144,	 6144,	 27027,   27027,   48,	 48,  1,  1 },

	{ BHDM_PixelClock_e54,				BAVC_HDMI_BitsPerPixel_e24bit,	 6144,	 6144,	 54000,   54000,   48,	 48,  1,  1 },
	{ BHDM_PixelClock_e54_MUL_1_001,	BAVC_HDMI_BitsPerPixel_e24bit,	 6144,	 6144,	 54054,   54054,   48,	 48,  1,  1 },

	{ BHDM_PixelClock_e74_25,			BAVC_HDMI_BitsPerPixel_e24bit,	 6144,	 6144,	 74250,   74250,   48,	 48,  1,  1 },
	{ BHDM_PixelClock_e74_25_DIV_1_001, BAVC_HDMI_BitsPerPixel_e24bit,	11648,	11648,	140625,  140625,   91,	 91,  1,  1 },

	{ BHDM_PixelClock_e108, 			BAVC_HDMI_BitsPerPixel_e24bit,	 6144,	 6144,	108000,  108000,   48,	 48,  1,  1 },
	{ BHDM_PixelClock_e108_MUL_1_001,	BAVC_HDMI_BitsPerPixel_e24bit,	 6144,	 6144,	108108,  108108,   48,	 48,  1,  1 },

	{ BHDM_PixelClock_e148_5,			BAVC_HDMI_BitsPerPixel_e24bit,	 6144,	 6144,	148500,  148500,   48,	 48,  1,  1 },
	{ BHDM_PixelClock_e148_5_DIV_1_001, BAVC_HDMI_BitsPerPixel_e24bit,	 5824,	11648,	140625,  140625,   45,	 46,  1,  1 },


	/******** 10bit deep color mode *********/
	{ BHDM_PixelClock_e25_2,			BAVC_HDMI_BitsPerPixel_e30bit,	 6144,	 6144,	 31500,   31500,   48,	 48,  1,  1 },
	{ BHDM_PixelClock_e25_2_DIV_1_001,	BAVC_HDMI_BitsPerPixel_e30bit,	 9152,	 6144,	 46875,   46875,   71,	 72,  1,  1 },

	{ BHDM_PixelClock_e27,				BAVC_HDMI_BitsPerPixel_e30bit,	 6144,	 6144,	 33750,   33750,   48,	 48,  1,  1 },
	{ BHDM_PixelClock_e27_MUL_1_001,	BAVC_HDMI_BitsPerPixel_e30bit,	 8192,	 8192,	 45045,   45045,   64,	 64,  1,  1 },

	{ BHDM_PixelClock_e54,				BAVC_HDMI_BitsPerPixel_e30bit,	 6144,	 6144,	 67500,   67500,   48,	 48,  1,  1 },
	{ BHDM_PixelClock_e54_MUL_1_001,	BAVC_HDMI_BitsPerPixel_e30bit,	 6144,	 6144,	 67567,   67568,   48,	 48,  1,  1 },

	{ BHDM_PixelClock_e74_25,			BAVC_HDMI_BitsPerPixel_e30bit,	 6144,	 6144,	 92812,   92813,   48,	 48,  1,  1 },
	{ BHDM_PixelClock_e74_25_DIV_1_001, BAVC_HDMI_BitsPerPixel_e30bit,	 9472,	 9472,	142943,  142943,   74,	 74,  1,  1 },

	{ BHDM_PixelClock_e108, 			BAVC_HDMI_BitsPerPixel_e30bit,	 6144,	 6144,	135000,  135000,   48,	 48,  1,  1 },
	{ BHDM_PixelClock_e108_MUL_1_001,	BAVC_HDMI_BitsPerPixel_e30bit,	 6144,	 6144,	135135,  135135,   48,	 48,  1,  1 },

	{ BHDM_PixelClock_e148_5,			BAVC_HDMI_BitsPerPixel_e30bit,	 6144,	 6144,	185625,  185625,   48,	 48,  1,  1 },
	{ BHDM_PixelClock_e148_5_DIV_1_001, BAVC_HDMI_BitsPerPixel_e30bit,	11648,	11648,	351562,  351563,   91,	 91,  1,  1 },

	/******** 12bit deep color mode *********/
	{ BHDM_PixelClock_e25_2,			BAVC_HDMI_BitsPerPixel_e36bit,	 6144,	 6144,	 37800,   37800,   48,	 48,  1,  1 },
	{ BHDM_PixelClock_e25_2_DIV_1_001,	BAVC_HDMI_BitsPerPixel_e36bit,	 9152,	 6144,	 56250,   56250,   71,	 72,  1,  1 },

	{ BHDM_PixelClock_e27,				BAVC_HDMI_BitsPerPixel_e36bit,	 6144,	 6144,	 40500,   40500,   48,	 48,  1,  1 },
	{ BHDM_PixelClock_e27_MUL_1_001,	BAVC_HDMI_BitsPerPixel_e36bit,	 8192,	 8192,	 54054,   54054,   64,	 64,  1,  1 },

	{ BHDM_PixelClock_e54,				BAVC_HDMI_BitsPerPixel_e36bit,	 6144,	 6144,	 81000,   81000,   48,	 48,  1,  1 },
	{ BHDM_PixelClock_e54_MUL_1_001,	BAVC_HDMI_BitsPerPixel_e36bit,	 6144,	 6144,	 81081,   81081,   48,	 48,  1,  1 },

	{ BHDM_PixelClock_e74_25,			BAVC_HDMI_BitsPerPixel_e36bit,	 6144,	 6144,	111375,  111375,   48,	 48,  1,  1 },
	{ BHDM_PixelClock_e74_25_DIV_1_001, BAVC_HDMI_BitsPerPixel_e36bit,	11648,	11648,	210937,  210938,   91,	 91,  1,  1 },

	{ BHDM_PixelClock_e108, 			BAVC_HDMI_BitsPerPixel_e36bit,	 6144,	 6144,	162000,  162000,   48,	 48,  1,  1 },
	{ BHDM_PixelClock_e108_MUL_1_001,	BAVC_HDMI_BitsPerPixel_e36bit,	 6144,	 6144,	162162,  162162,   48,	 48,  1,  1 },

	{ BHDM_PixelClock_e148_5,			BAVC_HDMI_BitsPerPixel_e36bit,	 6144,	 6144,	222750,  222750,   48,	 48,  1,  1 },
	{ BHDM_PixelClock_e148_5_DIV_1_001, BAVC_HDMI_BitsPerPixel_e36bit,	11648,	11648,	421875,  421875,   91,	 91,  1,  1 },


	/* PC/custom formats */
	{ BHDM_PixelClock_e40,				BAVC_HDMI_BitsPerPixel_e24bit,	 6144,	 6144,	 40000,   40000,   48,	 48,  1,  1 },
	{ BHDM_PixelClock_e65,				BAVC_HDMI_BitsPerPixel_e24bit,	 6144,	 6144,	 65000,   65000,   48,	 48,  1,  1 },
	{ BHDM_PixelClock_e65_DIV_1_001,	BAVC_HDMI_BitsPerPixel_e24bit,	 6144,	 6144,	 64935,   64936,   48,	 48,  9,  1 },
} ;


#if BHDM_CONFIG_88_2KHZ_AUDIO_SUPPORT
/******************************************************************************
Summary:
HDMI Audio Clock Capture and Regeneration Values for 88.2kHz
NOTE: These entries match the number of entries in the BHDM_RM_VideoRates Table
*******************************************************************************/
static BHDM_P_AUDIO_CLK_VALUES BHDM_88_2KHz_AudioClkValues[] =
{   /*                                                                   SW-N    HW-N    CTS_0    CTS_1     PERIOD   REPEAT */
	/******** 8bit standard mode *********/
    { BHDM_PixelClock_e25_2,           	BAVC_HDMI_BitsPerPixel_e24bit,	12544,	12544,	 28000,	  28000,   98,   98,  1,  1 },
    { BHDM_PixelClock_e25_2_DIV_1_001, 	BAVC_HDMI_BitsPerPixel_e24bit,	14014,	12544,	 31250,	  31250,  109,  110, 33, 31 },

    { BHDM_PixelClock_e27,             	BAVC_HDMI_BitsPerPixel_e24bit,	12544,	12544,	 30000,   30000,   98,   98,  1,  1 },
    { BHDM_PixelClock_e27_MUL_1_001,   	BAVC_HDMI_BitsPerPixel_e24bit,	12544,	12544,	 30030,   30030,   98,   98,  1,  1 },

    { BHDM_PixelClock_e54,             	BAVC_HDMI_BitsPerPixel_e24bit,	12544,	12544,	 60000,   60000,   98,   98,  1,  1 },
    { BHDM_PixelClock_e54_MUL_1_001,   	BAVC_HDMI_BitsPerPixel_e24bit,	12544,	12544,	 60060,   60060,   98,   98,  1,  1 },

    { BHDM_PixelClock_e74_25,          	BAVC_HDMI_BitsPerPixel_e24bit,	12544,	12544,	 82500,   82500,   98,   98,  1,  1 },
    { BHDM_PixelClock_e74_25_DIV_1_001,	BAVC_HDMI_BitsPerPixel_e24bit,	35672,	12544,	234375,  234375,  278,  279,  5, 11 },

    { BHDM_PixelClock_e108,            	BAVC_HDMI_BitsPerPixel_e24bit,	12544,	12544,	120000,  120000,   98,   98,  1,  1 },
    { BHDM_PixelClock_e108_MUL_1_001,  	BAVC_HDMI_BitsPerPixel_e24bit,	12544,	12544,	120120,  120120,   98,   98,  1,  1 },

    { BHDM_PixelClock_e148_5,          	BAVC_HDMI_BitsPerPixel_e24bit,	12544,	12544,	165000,  165000,   98,   98,  1,  1 },
    { BHDM_PixelClock_e148_5_DIV_1_001,	BAVC_HDMI_BitsPerPixel_e24bit,	17836,	12544,	234375,  234375,  139,  140, 21, 11 },

	/******** 10bit deep color mode *********/
    { BHDM_PixelClock_e25_2,           	BAVC_HDMI_BitsPerPixel_e30bit,	12544,	12544,	 35000,   35000,   98,   98,  1,  1 },
    { BHDM_PixelClock_e25_2_DIV_1_001, 	BAVC_HDMI_BitsPerPixel_e30bit,	12544,	12544,	 34965,   34965,   98,   98,  1,  1 },

    { BHDM_PixelClock_e27,             	BAVC_HDMI_BitsPerPixel_e30bit,	12544,	12544,	 37500,   37500,   98,   98,  1,  1 },
    { BHDM_PixelClock_e27_MUL_1_001,   	BAVC_HDMI_BitsPerPixel_e30bit,	12544,	12544,	 37537,   37538,   98,   98,  1,  1 },

    { BHDM_PixelClock_e54,             	BAVC_HDMI_BitsPerPixel_e30bit,	12544,	12544,	 75000,   75000,   98,   98,  1,  1 },
    { BHDM_PixelClock_e54_MUL_1_001,   	BAVC_HDMI_BitsPerPixel_e30bit,	12544,	12544,	 75075,   75075,   98,   98,  1,  1 },

    { BHDM_PixelClock_e74_25,          	BAVC_HDMI_BitsPerPixel_e30bit,	12544,	12544,	103125,  103125,   98,   98,  1,  1 },
    { BHDM_PixelClock_e74_25_DIV_1_001,	BAVC_HDMI_BitsPerPixel_e30bit,	12544,	12544,	103022,  103022,   98,   98,  1,  1 },

    { BHDM_PixelClock_e108,            	BAVC_HDMI_BitsPerPixel_e30bit,	12544,	12544,	150000,  150000,   98,   98,  1,  1 },
    { BHDM_PixelClock_e108_MUL_1_001,  	BAVC_HDMI_BitsPerPixel_e30bit,	12544,	12544,	150150,  150150,   98,   98,  1,  1 },

    { BHDM_PixelClock_e148_5,          	BAVC_HDMI_BitsPerPixel_e30bit,	12544,	12544,	206250,  206250,   98,   98,  1,  1 },
    { BHDM_PixelClock_e148_5_DIV_1_001,	BAVC_HDMI_BitsPerPixel_e30bit,	12544,	12544,	206044,  206044,   98,   98,  1,  1 },

	/******** 12bit deep color mode *********/
    { BHDM_PixelClock_e25_2,           	BAVC_HDMI_BitsPerPixel_e36bit,	12544,	12544,	 42000,   42000,   98,   98,  1,  1 },
    { BHDM_PixelClock_e25_2_DIV_1_001, 	BAVC_HDMI_BitsPerPixel_e36bit,	14014,	12544,	 46875,   46875,  109,  110, 33, 31 },

    { BHDM_PixelClock_e27,             	BAVC_HDMI_BitsPerPixel_e36bit,	12544,	12544,	 45000,   45000,   98,   98,  1,  1 },
    { BHDM_PixelClock_e27_MUL_1_001,   	BAVC_HDMI_BitsPerPixel_e36bit,	12544,	12544,	 45045,   45045,   98,   98,  1,  1 },

    { BHDM_PixelClock_e54,             	BAVC_HDMI_BitsPerPixel_e36bit,	12544,	12544,	 90000,   90000,   98,   98,  1,  1 },
    { BHDM_PixelClock_e54_MUL_1_001,   	BAVC_HDMI_BitsPerPixel_e36bit,	12544,	12544,	 90090,   90090,   98,   98,  1,  1 },

    { BHDM_PixelClock_e74_25,          	BAVC_HDMI_BitsPerPixel_e36bit,	12544,	12544,	123750,  123750,   98,   98,  1,  1 },
    { BHDM_PixelClock_e74_25_DIV_1_001,	BAVC_HDMI_BitsPerPixel_e36bit,	12544,	12544,	123626,  123627,   98,   98,  3,  2 },

    { BHDM_PixelClock_e108,            	BAVC_HDMI_BitsPerPixel_e36bit,	12544,	12544,	180000,  180000,   98,   98,  1,  1 },
    { BHDM_PixelClock_e108_MUL_1_001,  	BAVC_HDMI_BitsPerPixel_e36bit,	12544,	12544,	180180,  180180,   98,   98,  1,  1 },

    { BHDM_PixelClock_e148_5,          	BAVC_HDMI_BitsPerPixel_e36bit,	12544,	12544,	247500,  247500,   98,   98,  1,  1 },
    { BHDM_PixelClock_e148_5_DIV_1_001,	BAVC_HDMI_BitsPerPixel_e36bit,	35672,	12544,	703125,  703125,  278,  279,  5, 11 },
} ;
#endif


#if BHDM_CONFIG_96KHZ_AUDIO_SUPPORT
/******************************************************************************
Summary:
HDMI Audio Clock Capture and Regeneration Values for 96kHz
NOTE: These entries match the number of entries in the BHDM_RM_VideoRates Table
*******************************************************************************/
static const BHDM_P_AUDIO_CLK_VALUES BHDM_96KHz_AudioClkValues[] =
{	/*		 InputPixelClock				BitsPerPixel						SW-N	HW-N	CTS_0	 CTS_1	   PERIOD	REPEAT */

	/******** 8bit standard mode *********/
	{ BHDM_PixelClock_e25_2,			BAVC_HDMI_BitsPerPixel_e24bit,	12288,	12288,	 25200,   25200,   96,	 96,  1,  1 },
	{ BHDM_PixelClock_e25_2_DIV_1_001,	BAVC_HDMI_BitsPerPixel_e24bit,	13728,	12288,	 28125,   28125,  107,	108,  3,  1 },

	{ BHDM_PixelClock_e27,				BAVC_HDMI_BitsPerPixel_e24bit,	12288,	12288,	 27000,   27000,   96,	 96,  1,  1 },
	{ BHDM_PixelClock_e27_MUL_1_001,	BAVC_HDMI_BitsPerPixel_e24bit,	12288,	12288,	 27027,   27027,   96,	 96,  1,  1 },

	{ BHDM_PixelClock_e54,				BAVC_HDMI_BitsPerPixel_e24bit,	12288,	12288,	 54000,   54000,   96,	 96,  1,  1 },
	{ BHDM_PixelClock_e54_MUL_1_001,	BAVC_HDMI_BitsPerPixel_e24bit,	12288,	12288,	 54054,   54054,   96,	 96,  1,  1 },

	{ BHDM_PixelClock_e74_25,			BAVC_HDMI_BitsPerPixel_e24bit,	12288,	12288,	 74250,   74250,   96,	 96,  1,  1 },
	{ BHDM_PixelClock_e74_25_DIV_1_001, BAVC_HDMI_BitsPerPixel_e24bit,	23296,	23296,	140625,  140625,  182,	182,  1,  1 },

	{ BHDM_PixelClock_e108, 			BAVC_HDMI_BitsPerPixel_e24bit,	12288,	12288,	108000,  108000,   96,	 96,  1,  1 },
	{ BHDM_PixelClock_e108_MUL_1_001,	BAVC_HDMI_BitsPerPixel_e24bit,	12288,	12288,	108108,  108108,   96,	 96,  1,  1 },

	{ BHDM_PixelClock_e148_5,			BAVC_HDMI_BitsPerPixel_e24bit,	12288,	12288,	148500,  148500,   96,	 96,  1,  1 },
	{ BHDM_PixelClock_e148_5_DIV_1_001, BAVC_HDMI_BitsPerPixel_e24bit,	11648,	11648,	140625,  140625,   91,	 91,  1,  1 },


	/******** 10bit deep color mode *********/
	{ BHDM_PixelClock_e25_2,			BAVC_HDMI_BitsPerPixel_e30bit,	12288,	12288,	 31500,   31500,   96,	 96,  1,  1 },
	{ BHDM_PixelClock_e25_2_DIV_1_001,	BAVC_HDMI_BitsPerPixel_e30bit,	18304,	18304,	 46875,   46875,  143,	143,  1,  1 },

	{ BHDM_PixelClock_e27,				BAVC_HDMI_BitsPerPixel_e30bit,	12288,	12288,	 33750,   33750,   96,	 96,  1,  1 },
	{ BHDM_PixelClock_e27_MUL_1_001,	BAVC_HDMI_BitsPerPixel_e30bit,	16384,	16384,	 45045,   45045,  128,	128,  1,  1 },

	{ BHDM_PixelClock_e54,				BAVC_HDMI_BitsPerPixel_e30bit,	12288,	12288,	 67500,   67500,   96,	 96,  1,  1 },
	{ BHDM_PixelClock_e54_MUL_1_001,	BAVC_HDMI_BitsPerPixel_e30bit,	12288,	12288,	 67567,   67568,   96,	 96,  1,  1 },

	{ BHDM_PixelClock_e74_25,			BAVC_HDMI_BitsPerPixel_e30bit,	12288,	12288,	 92812,   92813,   96,	 96,  1,  1 },
	{ BHDM_PixelClock_e74_25_DIV_1_001, BAVC_HDMI_BitsPerPixel_e30bit,	18944,	18944,	142943,  142943,  148,	148,  1,  1 },

	{ BHDM_PixelClock_e108, 			BAVC_HDMI_BitsPerPixel_e30bit,	12288,	12288,	135000,  135000,   96,	 96,  1,  1 },
	{ BHDM_PixelClock_e108_MUL_1_001,	BAVC_HDMI_BitsPerPixel_e30bit,	12288,	12288,	135135,  135135,   96,	 96,  1,  1 },

	{ BHDM_PixelClock_e148_5,			BAVC_HDMI_BitsPerPixel_e30bit,	12288,	12288,	185625,  185625,   96,	 96,  1,  1 },
	{ BHDM_PixelClock_e148_5_DIV_1_001, BAVC_HDMI_BitsPerPixel_e30bit,	23296,	23296,	351562,  351563,  182,	182,  1,  1 },


	/******** 12bit deep color mode *********/
	{ BHDM_PixelClock_e25_2,			BAVC_HDMI_BitsPerPixel_e36bit,	12288,	12288,	 37800,   37800,   96,	 96,  1,  1 },
	{ BHDM_PixelClock_e25_2_DIV_1_001,	BAVC_HDMI_BitsPerPixel_e36bit,	18304,	18304,	 56250,   56250,  143,	143,  1,  1 },

	{ BHDM_PixelClock_e27,				BAVC_HDMI_BitsPerPixel_e36bit,	12288,	12288,	 40500,   40500,   96,	 96,  1,  1 },
	{ BHDM_PixelClock_e27_MUL_1_001,	BAVC_HDMI_BitsPerPixel_e36bit,	16384,	16384,	 54054,   54054,  128,	128,  1,  1 },

	{ BHDM_PixelClock_e54,				BAVC_HDMI_BitsPerPixel_e36bit,	12288,	12288,	 81000,   81000,   96,	 96,  1,  1 },
	{ BHDM_PixelClock_e54_MUL_1_001,	BAVC_HDMI_BitsPerPixel_e36bit,	12288,	12288,	 81081,   81081,   96,	 96,  1,  1 },

	{ BHDM_PixelClock_e74_25,			BAVC_HDMI_BitsPerPixel_e36bit,	12288,	12288,	111375,  111375,   96,	 96,  1,  1 },
	{ BHDM_PixelClock_e74_25_DIV_1_001, BAVC_HDMI_BitsPerPixel_e36bit,	23296,	23296,	210937,  210938,  182,	182,  1,  1 },

	{ BHDM_PixelClock_e108, 			BAVC_HDMI_BitsPerPixel_e36bit,	12288,	12288,	162000,  162000,   96,	 96,  1,  1 },
	{ BHDM_PixelClock_e108_MUL_1_001,	BAVC_HDMI_BitsPerPixel_e36bit,	12288,	12288,	162162,  162162,   96,	 96,  1,  1 },

	{ BHDM_PixelClock_e148_5,			BAVC_HDMI_BitsPerPixel_e36bit,	12288,	12288,	222750,  222750,   96,	 96,  1,  1 },
	{ BHDM_PixelClock_e148_5_DIV_1_001, BAVC_HDMI_BitsPerPixel_e36bit,	23296,	23296,	421875,  421875,  182,	182,  1,  1 },


	/* PC/custom formats */
	{ BHDM_PixelClock_e40,				BAVC_HDMI_BitsPerPixel_e24bit,	 12288,	 12288,	 40000,   40000,   96,	 96,  1,  1 },
	{ BHDM_PixelClock_e65,				BAVC_HDMI_BitsPerPixel_e24bit,	 12288,	 12288,	 65000,   65000,   96,	 96,  1,  1 },
	{ BHDM_PixelClock_e65_DIV_1_001,	BAVC_HDMI_BitsPerPixel_e24bit,	 12288,	 12288,	 64935,   64936,   96,	 96,  9,  1 },	    	
} ;
#endif


#if BHDM_CONFIG_176_4KHZ_AUDIO_SUPPORT
/******************************************************************************
Summary:
HDMI Audio Clock Capture and Regeneration Values for 176.4kHz
NOTE: These entries match the number of entries in the BHDM_RM_VideoRates Table
*******************************************************************************/
static const BHDM_P_AUDIO_CLK_VALUES BHDM_176_4KHz_AudioClkValues[] =
{	/*		 InputPixelClock				BitsPerPixel						SW-N	HW-N	CTS_0	 CTS_1	   PERIOD	REPEAT */

	/******** 8bit standard mode *********/
	{ BHDM_PixelClock_e25_2,			BAVC_HDMI_BitsPerPixel_e24bit,	25088,	25088,	 28000,   28000,  196,	196,  1,  1 },
	{ BHDM_PixelClock_e25_2_DIV_1_001,	BAVC_HDMI_BitsPerPixel_e24bit,	28028,	25088,	 31250,   31250,  218,	219,  1, 31 },

	{ BHDM_PixelClock_e27,				BAVC_HDMI_BitsPerPixel_e24bit,	25088,	25088,	 30000,   30000,  196,	196,  1,  1 },
	{ BHDM_PixelClock_e27_MUL_1_001,	BAVC_HDMI_BitsPerPixel_e24bit,	25088,	25088,	 30030,   30030,  196,	196,  1,  1 },

	{ BHDM_PixelClock_e54,				BAVC_HDMI_BitsPerPixel_e24bit,	25088,	25088,	 60000,   60000,  196,	196,  1,  1 },
	{ BHDM_PixelClock_e54_MUL_1_001,	BAVC_HDMI_BitsPerPixel_e24bit,	25088,	25088,	 60060,   60060,  196,	196,  1,  1 },

	{ BHDM_PixelClock_e74_25,			BAVC_HDMI_BitsPerPixel_e24bit,	25088,	25088,	 82500,   82500,  196,	196,  1,  1 },
	{ BHDM_PixelClock_e74_25_DIV_1_001, BAVC_HDMI_BitsPerPixel_e24bit,	71344,	25088,	234375,  234375,  557,	558,  5,  3 },

	{ BHDM_PixelClock_e108, 			BAVC_HDMI_BitsPerPixel_e24bit,	25088,	25088,	120000,  120000,  196,	196,  1,  1 },
	{ BHDM_PixelClock_e108_MUL_1_001,	BAVC_HDMI_BitsPerPixel_e24bit,	25088,	25088,	120120,  120120,  196,	196,  1,  1 },

	{ BHDM_PixelClock_e148_5,			BAVC_HDMI_BitsPerPixel_e24bit,	25088,	25088,	165000,  165000,  196,	196,  1,  1 },
	{ BHDM_PixelClock_e148_5_DIV_1_001, BAVC_HDMI_BitsPerPixel_e24bit,	35672,	25088,	234375,  234375,  278,	279,  5, 11 },


	/******** 10bit deep color mode *********/
	{ BHDM_PixelClock_e25_2,			BAVC_HDMI_BitsPerPixel_e30bit,	25088,	25088,	 35000,   35000,  196,	196,  1,  1 },
	{ BHDM_PixelClock_e25_2_DIV_1_001,	BAVC_HDMI_BitsPerPixel_e30bit,	25088,	25088,	 34965,   34965,  196,	196,  1,  1 },

	{ BHDM_PixelClock_e27,				BAVC_HDMI_BitsPerPixel_e30bit,	25088,	25088,	 37500,   37500,  196,	196,  1,  1 },
	{ BHDM_PixelClock_e27_MUL_1_001,	BAVC_HDMI_BitsPerPixel_e30bit,	25088,	25088,	 37537,   37538,  196,	196,  1,  1 },

	{ BHDM_PixelClock_e54,				BAVC_HDMI_BitsPerPixel_e30bit,	25088,	25088,	 75000,   75000,  196,	196,  1,  1 },
	{ BHDM_PixelClock_e54_MUL_1_001,	BAVC_HDMI_BitsPerPixel_e30bit,	25088,	25088,	 75075,   75075,  196,	196,  1,  1 },

	{ BHDM_PixelClock_e74_25,			BAVC_HDMI_BitsPerPixel_e30bit,	25088,	25088,	103125,  103125,  196,	196,  1,  1 },
	{ BHDM_PixelClock_e74_25_DIV_1_001, BAVC_HDMI_BitsPerPixel_e30bit,	25088,	25088,	103022,  103022,  196,	196,  1,  1 },

	{ BHDM_PixelClock_e108, 			BAVC_HDMI_BitsPerPixel_e30bit,	25088,	25088,	150000,  150000,  196,	196,  1,  1 },
	{ BHDM_PixelClock_e108_MUL_1_001,	BAVC_HDMI_BitsPerPixel_e30bit,	25088,	25088,	150150,  150150,  196,	196,  1,  1 },

	{ BHDM_PixelClock_e148_5,			BAVC_HDMI_BitsPerPixel_e30bit,	25088,	25088,	206250,  206250,  196,	196,  1,  1 },
	{ BHDM_PixelClock_e148_5_DIV_1_001, BAVC_HDMI_BitsPerPixel_e30bit,	25088,	25088,	206044,  206044,  196,	196,  1,  1 },


	/******** 12bit deep color mode *********/
	{ BHDM_PixelClock_e25_2,			BAVC_HDMI_BitsPerPixel_e36bit,	25088,	25088,	 42000,   42000,  196,	196,  1,  1 },
	{ BHDM_PixelClock_e25_2_DIV_1_001,	BAVC_HDMI_BitsPerPixel_e36bit,	28028,	25088,	 46875,   46875,  218,	219,  1, 31 },

	{ BHDM_PixelClock_e27,				BAVC_HDMI_BitsPerPixel_e36bit,	25088,	25088,	 45000,   45000,  196,	196,  1,  1 },
	{ BHDM_PixelClock_e27_MUL_1_001,	BAVC_HDMI_BitsPerPixel_e36bit,	25088,	25088,	 45045,   45045,  196,	196,  1,  1 },

	{ BHDM_PixelClock_e54,				BAVC_HDMI_BitsPerPixel_e36bit,	25088,	25088,	 90000,   90000,  196,	196,  1,  1 },
	{ BHDM_PixelClock_e54_MUL_1_001,	BAVC_HDMI_BitsPerPixel_e36bit,	25088,	25088,	 90090,   90090,  196,	196,  1,  1 },

	{ BHDM_PixelClock_e74_25,			BAVC_HDMI_BitsPerPixel_e36bit,	25088,	25088,	123750,  123750,  196,	196,  1,  1 },
	{ BHDM_PixelClock_e74_25_DIV_1_001, BAVC_HDMI_BitsPerPixel_e36bit,	25088,	25088,	123626,  123627,  196,	196,  3,  2 },

	{ BHDM_PixelClock_e108, 			BAVC_HDMI_BitsPerPixel_e36bit,	25088,	25088,	180000,  180000,  196,	196,  1,  1 },
	{ BHDM_PixelClock_e108_MUL_1_001,	BAVC_HDMI_BitsPerPixel_e36bit,	25088,	25088,	180180,  180180,  196,	196,  1,  1 },

	{ BHDM_PixelClock_e148_5,			BAVC_HDMI_BitsPerPixel_e36bit,	25088,	25088,	247500,  247500,  196,	196,  1,  1 },
	{ BHDM_PixelClock_e148_5_DIV_1_001, BAVC_HDMI_BitsPerPixel_e36bit,	71344,	25088,	703125,  703125,  557,	558,  5,  3 },


	/* PC/custom formats */
	{ BHDM_PixelClock_e40,				BAVC_HDMI_BitsPerPixel_e24bit,	 25088,  25088,  44444,   44445,  196,	196,  6,  4 },
	{ BHDM_PixelClock_e65,				BAVC_HDMI_BitsPerPixel_e24bit,	 25088,  25088,  72222,   72223,  196,	196,  8,  2 },
	{ BHDM_PixelClock_e65_DIV_1_001,	BAVC_HDMI_BitsPerPixel_e24bit,	 25088,  25088,  72150,   72151,  196,	196,  9,  1 },			
} ;
#endif


#if BHDM_CONFIG_192KHZ_AUDIO_SUPPORT
/******************************************************************************
Summary:
HDMI Audio Clock Capture and Regeneration Values for 192kHz
NOTE: These entries match the number of entries in the BHDM_RM_VideoRates Table
*******************************************************************************/
static const BHDM_P_AUDIO_CLK_VALUES BHDM_192KHz_AudioClkValues[] =
{	/*		 InputPixelClock				BitsPerPixel						SW-N	HW-N	CTS_0	 CTS_1	   PERIOD	REPEAT */

	/******** 8bit standard mode *********/
	{ BHDM_PixelClock_e25_2,			BAVC_HDMI_BitsPerPixel_e24bit,	24576,	24576,	 25200,   25200,  192,	192,  1,  1 },
	{ BHDM_PixelClock_e25_2_DIV_1_001,	BAVC_HDMI_BitsPerPixel_e24bit,	27456,	24576,	 28125,   28125,  214,	215,  1,  1 },

	{ BHDM_PixelClock_e27,				BAVC_HDMI_BitsPerPixel_e24bit,	24576,	24576,	 27000,   27000,  192,	192,  1,  1 },
	{ BHDM_PixelClock_e27_MUL_1_001,	BAVC_HDMI_BitsPerPixel_e24bit,	24576,	24576,	 27027,   27027,  192,	192,  1,  1 },

	{ BHDM_PixelClock_e54,				BAVC_HDMI_BitsPerPixel_e24bit,	24576,	24576,	 54000,   54000,  192,	192,  1,  1 },
	{ BHDM_PixelClock_e54_MUL_1_001,	BAVC_HDMI_BitsPerPixel_e24bit,	24576,	24576,	 54054,   54054,  192,	192,  1,  1 },

	{ BHDM_PixelClock_e74_25,			BAVC_HDMI_BitsPerPixel_e24bit,	24576,	24576,	 74250,   74250,  192,	192,  1,  1 },
	{ BHDM_PixelClock_e74_25_DIV_1_001, BAVC_HDMI_BitsPerPixel_e24bit,	46592,	46592,	140625,  140625,  364,	364,  1,  1 },

	{ BHDM_PixelClock_e108, 			BAVC_HDMI_BitsPerPixel_e24bit,	24576,	24576,	108000,  108000,  192,	192,  1,  1 },
	{ BHDM_PixelClock_e108_MUL_1_001,	BAVC_HDMI_BitsPerPixel_e24bit,	24576,	24576,	108108,  108108,  192,	192,  1,  1 },

	{ BHDM_PixelClock_e148_5,			BAVC_HDMI_BitsPerPixel_e24bit,	24576,	24576,	148500,  148500,  192,	192,  1,  1 },
	{ BHDM_PixelClock_e148_5_DIV_1_001, BAVC_HDMI_BitsPerPixel_e24bit,	23296,	23296,	140625,  140625,  182,	182,  1,  1 },


	/******** 10bit deep color mode *********/
	{ BHDM_PixelClock_e25_2,			BAVC_HDMI_BitsPerPixel_e30bit,	24576,	24576,	 31500,   31500,  192,	192,  1,  1 },
	{ BHDM_PixelClock_e25_2_DIV_1_001,	BAVC_HDMI_BitsPerPixel_e30bit,	36608,	36608,	 46875,   46875,  286,	286,  1,  1 },

	{ BHDM_PixelClock_e27,				BAVC_HDMI_BitsPerPixel_e30bit,	24576,	24576,	 33750,   33750,  192,	192,  1,  1 },
	{ BHDM_PixelClock_e27_MUL_1_001,	BAVC_HDMI_BitsPerPixel_e30bit,	32768,	32768,	 45045,   45045,  256,	256,  1,  1 },

	{ BHDM_PixelClock_e54,				BAVC_HDMI_BitsPerPixel_e30bit,	24576,	24576,	 67500,   67500,  192,	192,  1,  1 },
	{ BHDM_PixelClock_e54_MUL_1_001,	BAVC_HDMI_BitsPerPixel_e30bit,	24576,	24576,	 67567,   67568,  192,	192,  1,  1 },

	{ BHDM_PixelClock_e74_25,			BAVC_HDMI_BitsPerPixel_e30bit,	24576,	24576,	 92812,   92813,  192,	192,  1,  1 },
	{ BHDM_PixelClock_e74_25_DIV_1_001, BAVC_HDMI_BitsPerPixel_e30bit,	37888,	37888,	142943,  142943,  296,	296,  1,  1 },

	{ BHDM_PixelClock_e108, 			BAVC_HDMI_BitsPerPixel_e30bit,	24576,	24576,	135000,  135000,  192,	192,  1,  1 },
	{ BHDM_PixelClock_e108_MUL_1_001,	BAVC_HDMI_BitsPerPixel_e30bit,	24576,	24576,	135135,  135135,  192,	192,  1,  1 },

	{ BHDM_PixelClock_e148_5,			BAVC_HDMI_BitsPerPixel_e30bit,	24576,	24576,	185625,  185625,  192,	192,  1,  1 },
	{ BHDM_PixelClock_e148_5_DIV_1_001, BAVC_HDMI_BitsPerPixel_e30bit,	46592,	46592,	351562,  351563,  364,	364,  1,  1 },


	/******** 12bit deep color mode *********/
	{ BHDM_PixelClock_e25_2,			BAVC_HDMI_BitsPerPixel_e36bit,	24576,	24576,	 37800,   37800,  192,	192,  1,  1 },
	{ BHDM_PixelClock_e25_2_DIV_1_001,	BAVC_HDMI_BitsPerPixel_e36bit,	36608,	36608,	 56250,   56250,  286,	286,  1,  1 },

	{ BHDM_PixelClock_e27,				BAVC_HDMI_BitsPerPixel_e36bit,	24576,	24576,	 40500,   40500,  192,	192,  1,  1 },
	{ BHDM_PixelClock_e27_MUL_1_001,	BAVC_HDMI_BitsPerPixel_e36bit,	32768,	32768,	 54054,   54054,  256,	256,  1,  1 },

	{ BHDM_PixelClock_e54,				BAVC_HDMI_BitsPerPixel_e36bit,	24576,	24576,	 81000,   81000,  192,	192,  1,  1 },
	{ BHDM_PixelClock_e54_MUL_1_001,	BAVC_HDMI_BitsPerPixel_e36bit,	24576,	24576,	 81081,   81081,  192,	192,  1,  1 },

	{ BHDM_PixelClock_e74_25,			BAVC_HDMI_BitsPerPixel_e36bit,	24576,	24576,	111375,  111375,  192,	192,  1,  1 },
	{ BHDM_PixelClock_e74_25_DIV_1_001, BAVC_HDMI_BitsPerPixel_e36bit,	46592,	46592,	210937,  210938,  364,	364,  1,  1 },

	{ BHDM_PixelClock_e108, 			BAVC_HDMI_BitsPerPixel_e36bit,	24576,	24576,	162000,  162000,  192,	192,  1,  1 },
	{ BHDM_PixelClock_e108_MUL_1_001,	BAVC_HDMI_BitsPerPixel_e36bit,	24576,	24576,	162162,  162162,  192,	192,  1,  1 },

	{ BHDM_PixelClock_e148_5,			BAVC_HDMI_BitsPerPixel_e36bit,	24576,	24576,	222750,  222750,  192,	192,  1,  1 },
	{ BHDM_PixelClock_e148_5_DIV_1_001, BAVC_HDMI_BitsPerPixel_e36bit,	46592,	46592,	421875,  421875,  364,	364,  1,  1 },

	
	/* PC/custom formats */
	{ BHDM_PixelClock_e40,				BAVC_HDMI_BitsPerPixel_e24bit,	 24576,  24576,  40000,   40000,  192,	192,  1,  1 },
	{ BHDM_PixelClock_e65,				BAVC_HDMI_BitsPerPixel_e24bit,	 24576,  24576,  65000,   65000,  192,	192,  1,  1 },
	{ BHDM_PixelClock_e65_DIV_1_001,	BAVC_HDMI_BitsPerPixel_e24bit,	 24576,  24576,  64935,   64936,  192,	192,  9,  1 },				
} ;
#endif


BERR_Code BHDM_P_ConfigurePhy(BHDM_Handle hHDMI, BHDM_Settings *NewHdmiSettings)
{
	BERR_Code rc = BERR_SUCCESS ;
	BHDM_PreEmphasis_Configuration stPreEmphasisConfig;

	BDBG_ENTER(BHDM_P_ConfigurePhy) ;
	BDBG_ASSERT( hHDMI ) ;

	rc = BHDM_GetPreEmphasisConfiguration(hHDMI, &stPreEmphasisConfig);
	if (rc) BERR_TRACE(rc);

	/* Majority of the cases default to this config */
	stPreEmphasisConfig.uiHfEn = 0;
	stPreEmphasisConfig.uiPreEmphasis_Ch0 = 0xa;
	stPreEmphasisConfig.uiPreEmphasis_Ch1 = 0xa;
	stPreEmphasisConfig.uiPreEmphasis_Ch2 = 0xa;						
	stPreEmphasisConfig.uiPreEmphasis_CK = 0xa; 				

	/*** Additional setup of Tx PHY */
	switch (NewHdmiSettings->eInputVideoFmt)
	{	
	case BFMT_VideoFmt_e1080p:
	case BFMT_VideoFmt_e1080p_50Hz:
#ifdef BHDM_CONFIG_BLURAY_3D_SUPPORT
	case BFMT_VideoFmt_e1080p_24Hz_3D :
	case BFMT_VideoFmt_e720p_3D :
	case BFMT_VideoFmt_e720p_50Hz_3D :
#endif

		switch (NewHdmiSettings->stColorDepth.eBitsPerPixel)
		{
		case BAVC_HDMI_BitsPerPixel_e24bit:
			stPreEmphasisConfig.uiHfEn = 3;
#if defined(HUMAX_PLATFORM_BASE) && (BCHP_CHIP == 7429)
	/*adjustment for humax EMI test */
			stPreEmphasisConfig.uiPreEmphasis_Ch0 = 0x3a;
			stPreEmphasisConfig.uiPreEmphasis_Ch1 = 0x3a;
			stPreEmphasisConfig.uiPreEmphasis_Ch2 = 0x3a;		
			stPreEmphasisConfig.uiPreEmphasis_CK = 0x3a;
#else

#if (BCHP_CHIP == 7231)			
			/* 7231 is an exception compare to 40nm in this format */
			stPreEmphasisConfig.uiPreEmphasis_Ch0 = 0x3d;
			stPreEmphasisConfig.uiPreEmphasis_Ch1 = 0x3d;
			stPreEmphasisConfig.uiPreEmphasis_Ch2 = 0x3d;						
			stPreEmphasisConfig.uiPreEmphasis_CK = 0x3d;		
#else
			stPreEmphasisConfig.uiPreEmphasis_Ch0 = 0x3c;
			stPreEmphasisConfig.uiPreEmphasis_Ch1 = 0x3c;
			stPreEmphasisConfig.uiPreEmphasis_Ch2 = 0x3c;						
			stPreEmphasisConfig.uiPreEmphasis_CK = 0x3c; 		
#endif		
#endif /*defined(HUMAX_PLATFORM_BASE)*/
			break;

		case BAVC_HDMI_BitsPerPixel_e30bit:			
		case BAVC_HDMI_BitsPerPixel_e36bit:
			stPreEmphasisConfig.uiHfEn = 3;
			stPreEmphasisConfig.uiPreEmphasis_Ch0 = 0x7f;
			stPreEmphasisConfig.uiPreEmphasis_Ch1 = 0x7f;
			stPreEmphasisConfig.uiPreEmphasis_Ch2 = 0x7f;						
			stPreEmphasisConfig.uiPreEmphasis_CK = 0x7f; 			
			break;			
			
		default:
			BDBG_ERR(("Unknown color depth")) ;
			rc = BERR_INVALID_PARAMETER ;
			goto done ;
		}
		break;

	case BFMT_VideoFmt_e480p   :
	case BFMT_VideoFmt_e576p_50Hz :
		if ((NewHdmiSettings->ePixelRepetition == BAVC_HDMI_PixelRepetition_e4x) 
			&&
			((NewHdmiSettings->stColorDepth.eBitsPerPixel == BAVC_HDMI_BitsPerPixel_e30bit) 
			|| (NewHdmiSettings->stColorDepth.eBitsPerPixel == BAVC_HDMI_BitsPerPixel_e36bit)))
		{		
			stPreEmphasisConfig.uiHfEn = 3;
			stPreEmphasisConfig.uiPreEmphasis_Ch0 = 0x3c;
			stPreEmphasisConfig.uiPreEmphasis_Ch1 = 0x3c;
			stPreEmphasisConfig.uiPreEmphasis_Ch2 = 0x3c;						
			stPreEmphasisConfig.uiPreEmphasis_CK = 0x3c;			
		}
		break;

	default:		
		break;
	}

	/* Set pre-emp configuration */
	BHDM_SetPreEmphasisConfiguration(hHDMI, &stPreEmphasisConfig);

done:
	
	BDBG_LEAVE(BHDM_P_ConfigurePhy);
	return rc;
}



/******************************************************************************
BERR_Code BHDM_EnableTmdsOutput_isr
Summary: Enable (Display) TMDS Output
*******************************************************************************/
void BHDM_P_EnableTmdsOutput_isr(
   BHDM_Handle hHDMI,		/* [in] HDMI handle */
   bool bEnableTmdsOutput	/* [in] boolean to enable/disable */
)
{
	uint32_t Register ;
	uint32_t TmdsOutput ;
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_HDMI_REFACTORING)
	uint8_t DeviceAttached ;
#endif

	BDBG_ENTER(BHDM_P_EnableTmdsOutput_isr) ;
	BDBG_ASSERT( hHDMI ) ;

#if BHDM_CONFIG_DVO_SUPPORT
	/* TMDS is always off when DVO is enabled */
	bEnableTmdsOutput = false ;
#endif
	
	if (bEnableTmdsOutput)
		TmdsOutput = 0x0 ; /* TMDS ON */
	else 
		TmdsOutput = 0x1 ; /* TMDS OFF */

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_HDMI_REFACTORING)
	BHDM_P_RxDeviceAttached_isr(hHDMI,&DeviceAttached);
#endif

#if BHDM_CONFIG_DEBUG_TMDS
	BDBG_WRN(("Confgure TMDS %s", TmdsOutput ? "OFF" : "ON"));
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_HDMI_REFACTORING)
	BDBG_WRN(("Configure TMDS Clock %s", !DeviceAttached && TmdsOutput ? "OFF" : "ON"));
#endif
#endif

	Register = BREG_Read32(hHDMI->hRegister, BCHP_HDMI_TX_PHY_POWERDOWN_CTL) ;

	/* set TMDS lines to power on*/
	Register &= 
		~( BCHP_MASK(HDMI_TX_PHY_POWERDOWN_CTL, TX_CK_PWRDN) 
		 | BCHP_MASK(HDMI_TX_PHY_POWERDOWN_CTL, TX_2_PWRDN) 
		 | BCHP_MASK(HDMI_TX_PHY_POWERDOWN_CTL, TX_1_PWRDN) 
		 | BCHP_MASK(HDMI_TX_PHY_POWERDOWN_CTL, TX_0_PWRDN))  ;

	/* set TMDS lines to requested value on/off */
#if BHDM_CONFIG_SWAP_DEFAULT_PHY_CHANNELS
	Register |= 
		  /* Rx may need CLOCK signal to wake up; always leave clock powered */
		  /* !!!SWAPPED!!! TX_2 is the CLOCK; TX_CK os CHANNEL 1 */
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_HDMI_REFACTORING)
		  BCHP_FIELD_DATA(HDMI_TX_PHY_POWERDOWN_CTL, TX_2_PWRDN, !DeviceAttached && TmdsOutput)
#else
		  BCHP_FIELD_DATA(HDMI_TX_PHY_POWERDOWN_CTL, TX_2_PWRDN,0)
#endif

		  /* set remaining channels as requested */
		| BCHP_FIELD_DATA(HDMI_TX_PHY_POWERDOWN_CTL, TX_CK_PWRDN, TmdsOutput) 
		| BCHP_FIELD_DATA(HDMI_TX_PHY_POWERDOWN_CTL, TX_1_PWRDN, TmdsOutput) 
		| BCHP_FIELD_DATA(HDMI_TX_PHY_POWERDOWN_CTL, TX_0_PWRDN, TmdsOutput)  ;
#else
	Register |= 
		  /* Rx may need CLOCK signal to wake up; always leave clock powered */
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_HDMI_REFACTORING)
		  BCHP_FIELD_DATA(HDMI_TX_PHY_POWERDOWN_CTL, TX_CK_PWRDN, !DeviceAttached && TmdsOutput)
#else
		  BCHP_FIELD_DATA(HDMI_TX_PHY_POWERDOWN_CTL, TX_CK_PWRDN, 0)
#endif

		  /* set remaining channels as requested */
		| BCHP_FIELD_DATA(HDMI_TX_PHY_POWERDOWN_CTL, TX_2_PWRDN, TmdsOutput) 
		| BCHP_FIELD_DATA(HDMI_TX_PHY_POWERDOWN_CTL, TX_1_PWRDN, TmdsOutput) 
		| BCHP_FIELD_DATA(HDMI_TX_PHY_POWERDOWN_CTL, TX_0_PWRDN, TmdsOutput)  ;
#endif

	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_TX_PHY_POWERDOWN_CTL, Register) ;

	/* take TMDS lines out of reset */
	Register = BREG_Read32(hHDMI->hRegister, BCHP_HDMI_TX_PHY_RESET_CTL) ;
	Register &= 
		~( BCHP_MASK(HDMI_TX_PHY_RESET_CTL, TX_CK_RESET) 
		 | BCHP_MASK(HDMI_TX_PHY_RESET_CTL, TX_2_RESET) 
		 | BCHP_MASK(HDMI_TX_PHY_RESET_CTL, TX_1_RESET) 
		 | BCHP_MASK(HDMI_TX_PHY_RESET_CTL, TX_0_RESET))  ;

	Register |= 
		  BCHP_FIELD_DATA(HDMI_TX_PHY_RESET_CTL, PLL_RESETB, 1) 
		| BCHP_FIELD_DATA(HDMI_TX_PHY_RESET_CTL, PLLDIV_RSTB, 1) ;
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_TX_PHY_RESET_CTL, Register) ;

	/* remember  current TMDS setting... */
	hHDMI->tmdsEnabled = bEnableTmdsOutput ;

	BDBG_LEAVE(BHDM_P_EnableTmdsOutput_isr) ;
}


/******************************************************************************
BERR_Code BHDM_SetAudioMute
Summary: Implements HDMI Audio (only) mute enable/disable.
*******************************************************************************/
BERR_Code BHDM_SetAudioMute(
	BHDM_Handle hHDMI,			   /* [in] HDMI handle */
	bool bEnableAudioMute		   /* [in] boolean to enable/disable */
)
{
	BERR_Code rc = BERR_SUCCESS;
	BDBG_ENTER(BHDM_SetAudioMute) ;

#if BHDM_CONFIG_AUDIO_MAI_BUS_DISABLE_SUPPORT
{
	uint32_t Register ;
	BDBG_ASSERT( hHDMI );

	/* AudioMute valid for HDMI only */
	if	(hHDMI->DeviceSettings.eOutputFormat != BHDM_OutputFormat_eHDMIMode)
	{
		BDBG_WRN(("Audio Mute only application in HDMI mode."));
		return BERR_INVALID_PARAMETER;
	}
	Register = BREG_Read32(hHDMI->hRegister, BCHP_HDMI_MAI_CONFIG) ; 
	if (bEnableAudioMute) {
		Register |= BCHP_FIELD_DATA(HDMI_MAI_CONFIG, DISABLE_MAI_AUDIO, 1) ;
	}
	else 
	{
		Register &= ~BCHP_MASK(HDMI_MAI_CONFIG, DISABLE_MAI_AUDIO);
	}
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_MAI_CONFIG, Register) ; 

	hHDMI->AudioMuteState = bEnableAudioMute ;
	BDBG_MSG(("AudioMute %d", bEnableAudioMute)) ;
}
#else

	BSTD_UNUSED(hHDMI) ;
	BSTD_UNUSED(bEnableAudioMute) ;

#endif	

	BDBG_LEAVE(BHDM_SetAudioMute) ;
	return rc ;
}  /* END BHDM_SetAudioMute */


/***************************************************************************
BHDM_AudioVideoRateChangeCB_isr
Summary: Configure the Rate Manager to match the Rate Manager for the 
Video Display System.
****************************************************************************/
void BHDM_AudioVideoRateChangeCB_isr(
	BHDM_Handle hHDMI,
	int   CallbackType,
	void *pvAudioOrVideoData)
{
#if BDBG_DEBUG_BUILD
static char *AudioSampleRateText[]	= { "32kHz", "44.1kHz", "48kHz", "96kHz" ,"16kHz", "22_05kHz", 
	"24kHz", "64kHz", "88_2kHz", "128kHz", "176_4kHz", "192kHz", "8kHz", "12kHz", "11_025kHz"} ;
#endif

	uint32_t	Register ;
	bool masterMode;
	typedef struct BHDM_BAVC_Clock
	{
		uint32_t ulPixelClkRate ;
		BAVC_HDMI_BitsPerPixel eBitsPerPixel;
		BAVC_HDMI_PixelRepetition ePixelRepetition;
		BHDM_InputPixelClock eInputPixelClock ;
	} BHDM_BAVC_Clock ;
	
	static const  BHDM_BAVC_Clock BHDM_SupportedClocks[] =
	{
		/* 8bit standard mode */
		{BFMT_PXL_25_2MHz,	BAVC_HDMI_BitsPerPixel_e24bit,	BAVC_HDMI_PixelRepetition_eNone,	BHDM_PixelClock_e25_2}, 		  /* 25.2	  */
		{BFMT_PXL_25_2MHz_DIV_1_001,	BAVC_HDMI_BitsPerPixel_e24bit,	BAVC_HDMI_PixelRepetition_eNone,	BHDM_PixelClock_e25_2_DIV_1_001}, /* 25.1748  */

		{BFMT_PXL_27MHz,	BAVC_HDMI_BitsPerPixel_e24bit,	BAVC_HDMI_PixelRepetition_eNone,	BHDM_PixelClock_e27},			  /* 27 	*/
		{BFMT_PXL_27MHz_MUL_1_001,	BAVC_HDMI_BitsPerPixel_e24bit,	BAVC_HDMI_PixelRepetition_eNone,	BHDM_PixelClock_e27_MUL_1_001},   /* 27.027 */

		/**********************
		* Currently, for orthogonal VEC platforms (7420, 7342, 7550, etc.), the VEC always run at 54Mhz for 480p format, regardless of pixel repetition
		* If no pixel repetition, VEC runs at 54Mhz but downsample to 27Mhz before feeding into HDMI 
		* If 2x pixel repetition is enabled, VEC runs at 54Mhz but will not downsample. 
		* Thus, during the callback, the rate is always 54Mhz 
		***********************/
		{BFMT_PXL_54MHz,	BAVC_HDMI_BitsPerPixel_e24bit,	BAVC_HDMI_PixelRepetition_eNone,	BHDM_PixelClock_e27_480p},			  /* 27 	*/
		{BFMT_PXL_54MHz_MUL_1_001,	BAVC_HDMI_BitsPerPixel_e24bit,	BAVC_HDMI_PixelRepetition_eNone,	BHDM_PixelClock_e27_MUL_1_001_480p},   /* 27.027 */
		
		{BFMT_PXL_54MHz,	BAVC_HDMI_BitsPerPixel_e24bit,	BAVC_HDMI_PixelRepetition_e2x,	BHDM_PixelClock_e54},			  /* 27x2 (2x pixel repetition)    */
		{BFMT_PXL_54MHz_MUL_1_001,	BAVC_HDMI_BitsPerPixel_e24bit,	BAVC_HDMI_PixelRepetition_e2x,	BHDM_PixelClock_e54_MUL_1_001},   /* 27.027x2 (2x pixel repetition) */

		{BFMT_PXL_27MHz,	BAVC_HDMI_BitsPerPixel_e24bit,	BAVC_HDMI_PixelRepetition_e2x,	BHDM_PixelClock_e54},             /* 27x2 (2x pixel repetition)    */
		{BFMT_PXL_27MHz_MUL_1_001,	BAVC_HDMI_BitsPerPixel_e24bit,	BAVC_HDMI_PixelRepetition_e2x,	BHDM_PixelClock_e54_MUL_1_001},   /* 27.027x2 (2x pixel repetition) */
		
		{BFMT_PXL_74_25MHz, 	BAVC_HDMI_BitsPerPixel_e24bit,	BAVC_HDMI_PixelRepetition_eNone,	BHDM_PixelClock_e74_25},		  /* 74.25	  */
		{BFMT_PXL_74_25MHz_DIV_1_001,	BAVC_HDMI_BitsPerPixel_e24bit,	BAVC_HDMI_PixelRepetition_eNone,	BHDM_PixelClock_e74_25_DIV_1_001},	/* 74.17582 */

		{BFMT_PXL_27MHz,	BAVC_HDMI_BitsPerPixel_e24bit,	BAVC_HDMI_PixelRepetition_e4x,	BHDM_PixelClock_e108},             /* 27x4 (4x pixel repetition)    */
		{BFMT_PXL_27MHz_MUL_1_001,	BAVC_HDMI_BitsPerPixel_e24bit,	BAVC_HDMI_PixelRepetition_e4x,	BHDM_PixelClock_e108_MUL_1_001},   /* 27.027x4 (4x pixel repetition) */
		
		{BFMT_PXL_54MHz,	BAVC_HDMI_BitsPerPixel_e24bit,	BAVC_HDMI_PixelRepetition_e4x,	BHDM_PixelClock_e108},			   /* 27x4 (4x pixel repetition)	*/
		{BFMT_PXL_54MHz_MUL_1_001,	BAVC_HDMI_BitsPerPixel_e24bit,	BAVC_HDMI_PixelRepetition_e4x,	BHDM_PixelClock_e108_MUL_1_001},			   /* 27x4 (4x pixel repetition)	*/
		
		{BFMT_PXL_148_5MHz, 	BAVC_HDMI_BitsPerPixel_e24bit,	BAVC_HDMI_PixelRepetition_eNone,	BHDM_PixelClock_e148_5},			 /* 148.5 */
		{BFMT_PXL_148_5MHz_DIV_1_001,	BAVC_HDMI_BitsPerPixel_e24bit,	BAVC_HDMI_PixelRepetition_eNone,	BHDM_PixelClock_e148_5_DIV_1_001},			 /* 148.35165 */


		/* 10bit deep color mode */
		{BFMT_PXL_25_2MHz,	BAVC_HDMI_BitsPerPixel_e30bit,	BAVC_HDMI_PixelRepetition_eNone,	BHDM_PixelClock_e31_5}, 			/* 31.5 	*/
		{BFMT_PXL_25_2MHz_DIV_1_001,	BAVC_HDMI_BitsPerPixel_e30bit,	BAVC_HDMI_PixelRepetition_eNone,	BHDM_PixelClock_e31_5_DIV_1_001},	/* 31.4685 */

		{BFMT_PXL_27MHz,	BAVC_HDMI_BitsPerPixel_e30bit,	BAVC_HDMI_PixelRepetition_eNone,	BHDM_PixelClock_e33_75},             /* 33.75     */
		{BFMT_PXL_27MHz_MUL_1_001,	BAVC_HDMI_BitsPerPixel_e30bit,	BAVC_HDMI_PixelRepetition_eNone,	BHDM_PixelClock_e33_75_MUL_1_001},   /* 33.78375 */

		/**********************
		* Currently, for orthogonal VEC platforms (7420, 7342, 7550, etc.), the VEC always run at 54Mhz for 480p format, regardless of pixel repetition
		* If no pixel repetition, VEC runs at 54Mhz but downsample to 27Mhz before feeding into HDMI 
		* If 2x pixel repetition is enabled, VEC runs at 54Mhz but will not downsample. 
		* Thus, during the callback, the rate is always 54Mhz 
		***********************/
		{BFMT_PXL_54MHz,	BAVC_HDMI_BitsPerPixel_e30bit,	BAVC_HDMI_PixelRepetition_eNone,	BHDM_PixelClock_e33_75_480p},			 /* 33.75	  */
		{BFMT_PXL_54MHz_MUL_1_001,	BAVC_HDMI_BitsPerPixel_e30bit,	BAVC_HDMI_PixelRepetition_eNone,	BHDM_PixelClock_e33_75_MUL_1_001_480p},  /* 33.78375 */

		{BFMT_PXL_54MHz,	BAVC_HDMI_BitsPerPixel_e30bit,	BAVC_HDMI_PixelRepetition_e2x,	BHDM_PixelClock_e67_5}, 			/* 33.75x2 (2x pixel repetition)	*/
		{BFMT_PXL_54MHz_MUL_1_001,	BAVC_HDMI_BitsPerPixel_e30bit,	BAVC_HDMI_PixelRepetition_e2x,	BHDM_PixelClock_e67_5_MUL_1_001},	/* 33.78375x2 (2x pixel repetition) */
		
		{BFMT_PXL_27MHz,	BAVC_HDMI_BitsPerPixel_e30bit,	BAVC_HDMI_PixelRepetition_e2x,	BHDM_PixelClock_e67_5},             /* 33.75x2 (2x pixel repetition)    */
		{BFMT_PXL_27MHz_MUL_1_001,	BAVC_HDMI_BitsPerPixel_e30bit,	BAVC_HDMI_PixelRepetition_e2x,	BHDM_PixelClock_e67_5_MUL_1_001},   /* 33.78375x2 (2x pixel repetition) */
		
		{BFMT_PXL_74_25MHz, BAVC_HDMI_BitsPerPixel_e30bit,	BAVC_HDMI_PixelRepetition_eNone,	BHDM_PixelClock_e92_8125},			/* 92.8125	  */
		{BFMT_PXL_74_25MHz_DIV_1_001,	BAVC_HDMI_BitsPerPixel_e30bit,	BAVC_HDMI_PixelRepetition_eNone,	BHDM_PixelClock_e92_8125_DIV_1_001},  /* 92.7198 */

		{BFMT_PXL_27MHz,	BAVC_HDMI_BitsPerPixel_e30bit,	BAVC_HDMI_PixelRepetition_e4x,	BHDM_PixelClock_e135},             /* 33.75x4 (4x pixel repetition)     */
		{BFMT_PXL_27MHz_MUL_1_001,	BAVC_HDMI_BitsPerPixel_e30bit,	BAVC_HDMI_PixelRepetition_e4x,	BHDM_PixelClock_e135_MUL_1_001},   /* 33.78375x4 (4x pixel repetition) */
		
		{BFMT_PXL_54MHz,	BAVC_HDMI_BitsPerPixel_e30bit,	BAVC_HDMI_PixelRepetition_e4x,	BHDM_PixelClock_e135},			   /* 33.75x4 (4x pixel repetition) 	*/
		{BFMT_PXL_54MHz_MUL_1_001,	BAVC_HDMI_BitsPerPixel_e30bit,	BAVC_HDMI_PixelRepetition_e4x,	BHDM_PixelClock_e135_MUL_1_001},			   /* 33.75x4 (4x pixel repetition) 	*/		
		
		{BFMT_PXL_148_5MHz, BAVC_HDMI_BitsPerPixel_e30bit,	BAVC_HDMI_PixelRepetition_eNone,	BHDM_PixelClock_e185_625},			 /* 185.625 */
		{BFMT_PXL_148_5MHz_DIV_1_001,	BAVC_HDMI_BitsPerPixel_e30bit,	BAVC_HDMI_PixelRepetition_eNone,	BHDM_PixelClock_e185_625_DIV_1_001},			 /* 185.4396 */


		/* 12bit deep color mode */
		{BFMT_PXL_25_2MHz,	BAVC_HDMI_BitsPerPixel_e36bit,	BAVC_HDMI_PixelRepetition_eNone,	BHDM_PixelClock_e37_8}, 			/* 37.8 	*/
		{BFMT_PXL_25_2MHz_DIV_1_001,	BAVC_HDMI_BitsPerPixel_e36bit,	BAVC_HDMI_PixelRepetition_eNone,	BHDM_PixelClock_e37_8_DIV_1_001},	/* 37.76223 */

		{BFMT_PXL_27MHz,	BAVC_HDMI_BitsPerPixel_e36bit,	BAVC_HDMI_PixelRepetition_eNone,	BHDM_PixelClock_e40_5}, 			/* 40.5 	*/
		{BFMT_PXL_27MHz_MUL_1_001,	BAVC_HDMI_BitsPerPixel_e36bit,	BAVC_HDMI_PixelRepetition_eNone,	BHDM_PixelClock_e40_5_MUL_1_001},	/* 40.54 */

		/**********************
		* Currently, for orthogonal VEC platforms (7420, 7342, 7550, etc.), the VEC always run at 54Mhz for 480p format, regardless of pixel repetition
		* If no pixel repetition, VEC runs at 54Mhz but downsample to 27Mhz before feeding into HDMI 
		* If 2x pixel repetition is enabled, VEC runs at 54Mhz but will not downsample. 
		* Thus, during the callback, the rate is always 54Mhz 
		***********************/
		{BFMT_PXL_54MHz,	BAVC_HDMI_BitsPerPixel_e36bit,	BAVC_HDMI_PixelRepetition_eNone,	BHDM_PixelClock_e40_5_480p},			/* 40.5 	*/
		{BFMT_PXL_54MHz_MUL_1_001,	BAVC_HDMI_BitsPerPixel_e36bit,	BAVC_HDMI_PixelRepetition_eNone,	BHDM_PixelClock_e40_5_MUL_1_001_480p},	/* 40.54 */ 	

		{BFMT_PXL_54MHz,	BAVC_HDMI_BitsPerPixel_e36bit,	BAVC_HDMI_PixelRepetition_e2x,	BHDM_PixelClock_e81},			  /* 40.5x2 (2x pixel repetition)	  */
		{BFMT_PXL_54MHz_MUL_1_001,	BAVC_HDMI_BitsPerPixel_e36bit,	BAVC_HDMI_PixelRepetition_e2x,	BHDM_PixelClock_e81_MUL_1_001},   /* 40.54x2 (2x pixel repetition) */
		
		{BFMT_PXL_27MHz,	BAVC_HDMI_BitsPerPixel_e36bit,	BAVC_HDMI_PixelRepetition_e2x,	BHDM_PixelClock_e81},             /* 40.5x2 (2x pixel repetition)     */
		{BFMT_PXL_27MHz_MUL_1_001,	BAVC_HDMI_BitsPerPixel_e36bit,	BAVC_HDMI_PixelRepetition_e2x,	BHDM_PixelClock_e81_MUL_1_001},   /* 40.54x2 (2x pixel repetition) */
		
		{BFMT_PXL_74_25MHz, BAVC_HDMI_BitsPerPixel_e36bit,	BAVC_HDMI_PixelRepetition_eNone,	BHDM_PixelClock_e111_375},			/* 111.375	  */
		{BFMT_PXL_74_25MHz_DIV_1_001,	BAVC_HDMI_BitsPerPixel_e36bit,	BAVC_HDMI_PixelRepetition_eNone,	BHDM_PixelClock_e111_375_DIV_1_001},  /* 111.264 */

		{BFMT_PXL_27MHz,	BAVC_HDMI_BitsPerPixel_e36bit,	BAVC_HDMI_PixelRepetition_e4x,	BHDM_PixelClock_e162},             /* 40.5x4 (4x pixel repetition)     */
		{BFMT_PXL_27MHz_MUL_1_001,	BAVC_HDMI_BitsPerPixel_e36bit,	BAVC_HDMI_PixelRepetition_e4x,	BHDM_PixelClock_e162_MUL_1_001},   /* 40.54x4 (4x pixel repetition) */
		
		{BFMT_PXL_54MHz,	BAVC_HDMI_BitsPerPixel_e36bit,	BAVC_HDMI_PixelRepetition_e4x,	BHDM_PixelClock_e162},			   /* 40.5x4 (4x pixel repetition)	   */
		{BFMT_PXL_54MHz_MUL_1_001,	BAVC_HDMI_BitsPerPixel_e36bit,	BAVC_HDMI_PixelRepetition_e4x,	BHDM_PixelClock_e162_MUL_1_001},			   /* 40.5x4 (4x pixel repetition)	   */		
		
		{BFMT_PXL_148_5MHz, BAVC_HDMI_BitsPerPixel_e36bit,	BAVC_HDMI_PixelRepetition_eNone,	BHDM_PixelClock_e222_75},			 /* 222.75 */
		{BFMT_PXL_148_5MHz_DIV_1_001,	BAVC_HDMI_BitsPerPixel_e36bit,	BAVC_HDMI_PixelRepetition_eNone,	BHDM_PixelClock_e222_75_DIV_1_001}, 		 /* 222.5275 */
		

		/* DVI/PC/custom pixel rate */
		{BFMT_PXL_40MHz,	BAVC_HDMI_BitsPerPixel_e24bit,	BAVC_HDMI_PixelRepetition_eNone,	BHDM_PixelClock_e40},			  /* 40 MHz */
		{BFMT_PXL_65MHz,	BAVC_HDMI_BitsPerPixel_e24bit,	BAVC_HDMI_PixelRepetition_eNone,	BHDM_PixelClock_e65},			  /* 65 MHz */
		{BFMT_PXL_65MHz_DIV_1_001,	BAVC_HDMI_BitsPerPixel_e24bit,	BAVC_HDMI_PixelRepetition_eNone,	BHDM_PixelClock_e65_DIV_1_001},			  /* 64.935 MHz */
		
		{BFMT_PXL_60_375MHz,	BAVC_HDMI_BitsPerPixel_e24bit,	BAVC_HDMI_PixelRepetition_eNone,	BHDM_PixelClock_e60_375},		  /* 60.375 */
		{BFMT_PXL_74_375MHz,	BAVC_HDMI_BitsPerPixel_e24bit,	BAVC_HDMI_PixelRepetition_eNone,	BHDM_PixelClock_e74_375},		  /* 74.375 */
		{BFMT_PXL_64MHz,	BAVC_HDMI_BitsPerPixel_e24bit,	BAVC_HDMI_PixelRepetition_eNone,	BHDM_PixelClock_e64},			  /* 64.000 */

		
		{BFMT_PXL_56_304MHz,	BAVC_HDMI_BitsPerPixel_e24bit,	BAVC_HDMI_PixelRepetition_eNone,	BHDM_PixelClock_eCUSTOM_1366x768p_50},	 /* 1366x768 @ 50	 */
		{BFMT_PXL_67_565MHz_DIV_1_001,	BAVC_HDMI_BitsPerPixel_e24bit,	BAVC_HDMI_PixelRepetition_eNone,	BHDM_PixelClock_eCUSTOM_1366x768p_5994}, /* 1366x768 @ 59.94 */
		{BFMT_PXL_67_565MHz,	BAVC_HDMI_BitsPerPixel_e24bit,	BAVC_HDMI_PixelRepetition_eNone,	BHDM_PixelClock_eCUSTOM_1366x768p_60},	 /* 1366x768 @ 60	 */
	} ;

	
	BHDM_InputPixelClock eInputPixelClock ;   
	uint8_t i ;
	
	BAVC_VdcDisplay_Info *pVdcRateInfo ; /* VIDEO callback */
	uint32_t	ulPixelClkRate; 		 /* see defines in bfmt.h (Mhz) */
	uint32_t	ulVertRefreshRate;		 /* see defines in bfmt.h (Mhz) */
	BAVC_HDMI_BitsPerPixel eBitsPerPixel;
	BAVC_HDMI_PixelRepetition ePixelRepetition;
										/* OR */
						
	BAVC_Audio_Info *pAudioData ;		/* AUDIO callback */
	BAVC_AudioSamplingRate eInputAudioSamplingRate ;
	BHDM_P_AUDIO_CLK_VALUES *pAudioParameters ;

		
	BDBG_ENTER(BHDM_AudioVideoRateChangeCB_isr) ;

	if ((CallbackType != BHDM_Callback_Type_eVideoChange)
	&&	(CallbackType != BHDM_Callback_Type_eAudioChange)
	&&	(CallbackType != BHDM_Callback_Type_eManualAudioChange))
	{
		BDBG_ERR((
			"Error in Callback Type %d; Use BHDM_Callback_Type_eXXX as int argument",
			CallbackType)) ;
		goto done ;
	}

	if (CallbackType == BHDM_Callback_Type_eVideoChange)
	{
		pVdcRateInfo = (BAVC_VdcDisplay_Info *) pvAudioOrVideoData ;
		ulPixelClkRate	  = pVdcRateInfo->ulPixelClkRate ;
		ulVertRefreshRate = pVdcRateInfo->ulVertRefreshRate ;
		eBitsPerPixel = hHDMI->DeviceSettings.stColorDepth.eBitsPerPixel;
		ePixelRepetition = hHDMI->DeviceSettings.ePixelRepetition;
#if 0
		BDBG_MSG(("Refresh Rate:	 0x%x PixelClock: %d", 
			ulVertRefreshRate, ulPixelClkRate)) ;
#endif	
		BDBG_MSG(("\nVideo Rate Change Callback")) ;
		
		eInputPixelClock = BHDM_PixelClock_eUnused ;

		for (i = 0 ; i < sizeof(BHDM_SupportedClocks) / sizeof(BHDM_BAVC_Clock) ; i++) 
			if ((ulPixelClkRate == BHDM_SupportedClocks[i].ulPixelClkRate) &&
				(eBitsPerPixel == BHDM_SupportedClocks[i].eBitsPerPixel) &&
				(ePixelRepetition == BHDM_SupportedClocks[i].ePixelRepetition))
			{
				eInputPixelClock = BHDM_SupportedClocks[i].eInputPixelClock ;
				break ;
			}

		if (eInputPixelClock == BHDM_PixelClock_eUnused)
		{
			BDBG_ERR(("Unknown HDMI Pixel Clock Rate: %d", ulPixelClkRate)) ;
			goto done ;
		}
		
		/* update our device settings to reflect the new Pixel Clock Rate in use */ 	
		hHDMI->eInputPixelClock = eInputPixelClock ;
		eInputAudioSamplingRate = hHDMI->DeviceSettings.eAudioSamplingRate ;

		BDBG_MSG(("\t************************************")) ;
		BDBG_MSG(("\t**** Video Rate Manager Updated ****")) ;
		BDBG_MSG(("\t************************************")) ;
	}	
	else  /* Audio Callback Only */
	{
#if BDBG_DEBUG_BUILD
		if	(CallbackType == BHDM_Callback_Type_eManualAudioChange) {
			BDBG_MSG(("\t********************************************")) ;
			BDBG_MSG(("\t**** Manual Sample Rate Change Callback ****")) ;
			BDBG_MSG(("\t*******************************************")) ;
		}
		else {
			BDBG_MSG(("\t*******************************************")) ;
			BDBG_MSG(("\t**** Audio Sample Rate Change Callback ****")) ;
			BDBG_MSG(("\t******************************************")) ;
		}			
#endif			

		/* get the new Audio Sampling Rate to use... */
		pAudioData = (BAVC_Audio_Info *) pvAudioOrVideoData ;
		eInputAudioSamplingRate = pAudioData->eAudioSamplingRate ;
		hHDMI->DeviceSettings.eAudioSamplingRate = pAudioData->eAudioSamplingRate ;
		
		/* use current Pixel Clock Rate for adjusting the Audio parameters */
		eInputPixelClock = hHDMI->eInputPixelClock ;
	}

		
	/* 
	** removed prior code to skip Audio Configuration when in DVI mode 
	** or using the DVO port; configure HDMI Audio Registers regardless 
	** of whether or not we are in HDMI mode 
	*/


	/* configure Audio Registers regardless of format (DVI vs HDMI) */
	
	/* 
	** re-configure the HDMI Clock Regeneration Packet 
	** even if it is DVI Only, CRP is used only HDMI mode
	**
	** CODE from removed function
	**	BHDM_ConfigureAudioClockRegenerationPackets(hHDMI, 
	**		hHDMI->DeviceSettings.eAudioSamplingRate)) ;
	*/
		
	/* First, determine what N(umerator) and CTS values */
	/* are to be used based on the Audio Rate */
	switch( eInputAudioSamplingRate ) 
	{

	case BAVC_AudioSamplingRate_e32k :
		pAudioParameters = (BHDM_P_AUDIO_CLK_VALUES *) BHDM_32KHz_AudioClkValues ;
		break ;

	case BAVC_AudioSamplingRate_e44_1k :
		pAudioParameters = (BHDM_P_AUDIO_CLK_VALUES *) BHDM_44_1KHz_AudioClkValues ;
		break ;

	case BAVC_AudioSamplingRate_e48k :
		pAudioParameters = (BHDM_P_AUDIO_CLK_VALUES *) BHDM_48KHz_AudioClkValues ;
		break ;

#if BHDM_CONFIG_88_2KHZ_AUDIO_SUPPORT
	case BAVC_AudioSamplingRate_e88_2k :
		pAudioParameters = (BHDM_P_AUDIO_CLK_VALUES *) BHDM_88_2KHz_AudioClkValues ;
		break ;
#endif

#if BHDM_CONFIG_96KHZ_AUDIO_SUPPORT
	case BAVC_AudioSamplingRate_e96k	:
		pAudioParameters = (BHDM_P_AUDIO_CLK_VALUES *) BHDM_96KHz_AudioClkValues ;
		break ;
#endif		

#if BHDM_CONFIG_176_4KHZ_AUDIO_SUPPORT
	case BAVC_AudioSamplingRate_e176_4k    :
		pAudioParameters = (BHDM_P_AUDIO_CLK_VALUES *) BHDM_176_4KHz_AudioClkValues ;
		break ;
#endif		

#if BHDM_CONFIG_192KHZ_AUDIO_SUPPORT
	case BAVC_AudioSamplingRate_e192k:
		pAudioParameters = (BHDM_P_AUDIO_CLK_VALUES *) BHDM_192KHz_AudioClkValues;
		break ;
#endif		

	default :
		BDBG_ERR(("Unsupported eAudioSamplingRate eNumeration: %d", 
			eInputAudioSamplingRate)) ;
		goto done ;
	}
	
#if BDBG_DEBUG_BUILD	
	BDBG_MSG(("HDMI Audio Configuration:")) ;
	BDBG_MSG(("  Pixel Clock Rate:	%s", BHDM_PixelClockText[eInputPixelClock])) ;

	if (eInputAudioSamplingRate < BAVC_AudioSamplingRate_eUnknown)
		BDBG_MSG(("  Audio Sample Rate: %s", AudioSampleRateText[eInputAudioSamplingRate])) ;
	else
		BDBG_MSG(("  Audio Sample Rate: Unknown")) ;
#endif

	/*
	  * If HW (External) CTS, then we don't have to program any of
	  * the CTS registers. If SW CTS, we get all the values from
	  * our audio parameters table.
	  */
	Register =	BCHP_FIELD_DATA(HDMI_CRP_CFG, reserved1, 0) 
				| BCHP_FIELD_DATA(HDMI_CRP_CFG, USE_MAI_BUS_SYNC_FOR_CTS_GENERATION, 1) 
				| BCHP_FIELD_DATA(HDMI_CRP_CFG, CRP_DISABLE, 0) 
				| BCHP_FIELD_DATA(HDMI_CRP_CFG, reserved0, 0);

	/* HW (External) CTS */
	if (hHDMI->DeviceSettings.CalculateCts)
	{
		Register |= BCHP_FIELD_DATA(HDMI_CRP_CFG, EXTERNAL_CTS_EN, 1) 
				| BCHP_FIELD_DATA(HDMI_CRP_CFG, N_VALUE, pAudioParameters[eInputPixelClock].HW_NValue);
		BREG_Write32(hHDMI->hRegister, BCHP_HDMI_CRP_CFG, Register) ; 

		BREG_Write32(hHDMI->hRegister, BCHP_HDMI_CTS_0, 0) ; 
		BREG_Write32(hHDMI->hRegister, BCHP_HDMI_CTS_1, 0) ; 
		BREG_Write32(hHDMI->hRegister, BCHP_HDMI_CTS_PERIOD_0, 0) ; 
		BREG_Write32(hHDMI->hRegister, BCHP_HDMI_CTS_PERIOD_1, 0) ; 
	}
	else	/* SW CTS */
	{
		Register |= BCHP_FIELD_DATA(HDMI_CRP_CFG, EXTERNAL_CTS_EN, 0) 
				| BCHP_FIELD_DATA(HDMI_CRP_CFG, N_VALUE, pAudioParameters[eInputPixelClock].NValue);
		BREG_Write32(hHDMI->hRegister, BCHP_HDMI_CRP_CFG, Register) ; 

		Register = BCHP_FIELD_DATA(HDMI_CTS_0, reserved0, 0)
				| BCHP_FIELD_DATA(HDMI_CTS_0, CTS_0, pAudioParameters[eInputPixelClock].CTS_0) ;
		BREG_Write32(hHDMI->hRegister, BCHP_HDMI_CTS_0, Register) ; 

		Register = BCHP_FIELD_DATA(HDMI_CTS_1, reserved0, 0)
				| BCHP_FIELD_DATA(HDMI_CTS_1, CTS_1, pAudioParameters[eInputPixelClock].CTS_1) ;
		BREG_Write32(hHDMI->hRegister, BCHP_HDMI_CTS_1, Register) ; 

		Register = BCHP_FIELD_DATA(HDMI_CTS_PERIOD_0, CTS_0_REPEAT, pAudioParameters[eInputPixelClock].CTS_0_REPEAT) 
				| BCHP_FIELD_DATA(HDMI_CTS_PERIOD_0, reserved0, 0) 
				| BCHP_FIELD_DATA(HDMI_CTS_PERIOD_0, CTS_PERIOD_0, pAudioParameters[eInputPixelClock].CTS_PERIOD_0) ;
		BREG_Write32(hHDMI->hRegister, BCHP_HDMI_CTS_PERIOD_0, Register) ; 

		Register = BCHP_FIELD_DATA(HDMI_CTS_PERIOD_1, CTS_1_REPEAT, pAudioParameters[eInputPixelClock].CTS_1_REPEAT) 
				| BCHP_FIELD_DATA(HDMI_CTS_PERIOD_1, reserved0, 0) 
				| BCHP_FIELD_DATA(HDMI_CTS_PERIOD_1, CTS_PERIOD_1, pAudioParameters[eInputPixelClock].CTS_PERIOD_1) ;
		BREG_Write32(hHDMI->hRegister, BCHP_HDMI_CTS_PERIOD_1, Register) ; 
	}

	/* Additional settings for FIFO_CTL register only if HDMI is not configured in Master Mode */
	Register = BREG_Read32( hHDMI->hRegister, BCHP_HDMI_FIFO_CTL) ;
	masterMode = (bool) BCHP_GET_FIELD_DATA(Register, HDMI_FIFO_CTL, MASTER_OR_SLAVE_N);		

	/* In master mode, No RECENTER and USE_FULL needs to be set to 0 */
	if (masterMode)
	{
		Register &= ~ BCHP_MASK(HDMI_FIFO_CTL, USE_FULL); 
		Register |= BCHP_FIELD_DATA(HDMI_FIFO_CTL, USE_FULL, 0);
		BREG_Write32(hHDMI->hRegister, BCHP_HDMI_FIFO_CTL, Register);
	}
	else
	{
		Register &=
		~( BCHP_MASK(HDMI_FIFO_CTL, RECENTER)
		 | BCHP_MASK(HDMI_FIFO_CTL, CAPTURE_POINTERS)); 

		/* Set to 1 */
		Register |=
		  BCHP_FIELD_DATA(HDMI_FIFO_CTL, RECENTER, 1)
		| BCHP_FIELD_DATA(HDMI_FIFO_CTL, CAPTURE_POINTERS, 1);
		BREG_Write32(hHDMI->hRegister, BCHP_HDMI_FIFO_CTL, Register);

		/* Now set to 0 */
		Register |=
		  BCHP_FIELD_DATA(HDMI_FIFO_CTL, RECENTER, 0)
		| BCHP_FIELD_DATA(HDMI_FIFO_CTL, CAPTURE_POINTERS, 0);
		BREG_Write32(hHDMI->hRegister, BCHP_HDMI_FIFO_CTL, Register);	

		/* Set additional fields */
		Register &= ~( BCHP_MASK(HDMI_FIFO_CTL, USE_FULL)
					| BCHP_MASK(HDMI_FIFO_CTL, USE_EMPTY)); 

		Register |= BCHP_FIELD_DATA(HDMI_FIFO_CTL, USE_FULL, 1)
				| BCHP_FIELD_DATA(HDMI_FIFO_CTL, USE_EMPTY, 1);
		BREG_Write32(hHDMI->hRegister, BCHP_HDMI_FIFO_CTL, Register);
	}

#if BDBG_DEBUG_BUILD
	if (hHDMI->DeviceSettings.CalculateCts)
	{
		BDBG_MSG(("%4d		*	   *",
			pAudioParameters[eInputPixelClock].HW_NValue)) ;
	}
	else
	{
		BDBG_MSG(("   N   CTS 0 - CTS 1")) ;
		BDBG_MSG(("%4d	%6d-%6d",
			pAudioParameters[eInputPixelClock].NValue, 
			pAudioParameters[eInputPixelClock].CTS_0,
			pAudioParameters[eInputPixelClock].CTS_1)) ;
		BDBG_MSG(("CTS 0 Repeat Value: %d Period: %d", 
			pAudioParameters[eInputPixelClock].CTS_0_REPEAT, pAudioParameters[eInputPixelClock].CTS_PERIOD_0)) ;
		BDBG_MSG(("CTS 1 Repeat Value: %d Period: %d", 
			pAudioParameters[eInputPixelClock].CTS_1_REPEAT, pAudioParameters[eInputPixelClock].CTS_PERIOD_1)) ;
	}	
#endif	

done:
	BDBG_LEAVE(BHDM_AudioVideoRateChangeCB_isr) ;
	return ;
}


/******************************************************************************
Summary:
Configure the MAI Audio Input Bus
*******************************************************************************/
void BHDM_P_ConfigureInputAudioFmt(
   BHDM_Handle hHDMI,		   /* [in] HDMI handle */
   BAVC_HDMI_AudioInfoFrame *stAudioInfoFrame
)
{
	uint32_t Register ;
	uint8_t ChannelMask = 0x03 ;  /* default to 2 channels */
	
#if BHDM_CONFIG_AUDIO_MAI_BUS_DISABLE_SUPPORT
	uint8_t DisableMai = hHDMI->AudioMuteState == true ? 1 : 0;
#endif

	switch (stAudioInfoFrame->SpeakerAllocation)
	{
	 case BHDM_ChannelAllocation_e_xx__xx__xx__xx__xx___xx_FR__FL :
		ChannelMask = 0x03 ;		break ;
		
	 case BHDM_ChannelAllocation_e_xx__xx__xx__xx__xx__LFE_FR__FL  :
		ChannelMask = 0x07 ;		break ;
	 
	 case BHDM_ChannelAllocation_e_xx__xx__xx__xx__FC___xx_FR__FL  :
		ChannelMask = 0x0B ;		break ;
		
	 case BHDM_ChannelAllocation_e_xx__xx__xx__xx__FC__LFE_FR__FL  : 
		ChannelMask = 0x0F ;		break ;
		
	 case BHDM_ChannelAllocation_e_xx__xx__xx__RC__xx___xx_FR__FL :
		ChannelMask = 0x13 ;		break ;
	 
	 case BHDM_ChannelAllocation_e_xx__xx__xx__RC__xx__LFE_FR__FL :
		ChannelMask = 0x17 ;		break ;
	 
	 case BHDM_ChannelAllocation_e_xx__xx__xx__RC__FC___xx_FR__FL :
		ChannelMask = 0x1B ;		break ;
		
	 case BHDM_ChannelAllocation_e_xx__xx__xx__RC__FC__LFE_FR__FL :
		ChannelMask = 0x1F ;		break ;
	 
	 case BHDM_ChannelAllocation_e_xx__xx__RR__RL__xx___xx_FR__FL :
		ChannelMask = 0x33 ;		break ;
		
	 case BHDM_ChannelAllocation_e_xx__xx__RR__RL__xx__LFE_FR__FL :
		ChannelMask = 0x37 ;		break ;
	 
	 case BHDM_ChannelAllocation_e_xx__xx__RR__RL__FC___xx_FR__FL :
		ChannelMask = 0x3B ;		break ;
	 
	 case BHDM_ChannelAllocation_e_xx__xx__RR__RL__FC__LFE_FR__FL :
		ChannelMask = 0x3F ;		break ;
		
	 case BHDM_ChannelAllocation_e_xx__RC__RR__RL__xx___xx_FR__FL :
		ChannelMask = 0x73 ;		break ;
	 
	 case BHDM_ChannelAllocation_e_xx__RC__RR__RL__xx__LFE_FR__FL :
		ChannelMask = 0x77 ;		break ;
		
	 case BHDM_ChannelAllocation_e_xx__RC__RR__RL__FC___xx_FR__FL :
		ChannelMask = 0x7B ;		break ;
		
	 case BHDM_ChannelAllocation_e_xx__RC__RR__RL__FC__LFE_FR__FL :
		ChannelMask = 0x7F ;		break ;

	case BHDM_ChannelAllocation_e_RRC_RLC__RR__RL__xx__xx_FR__FL :
		ChannelMask = 0xF3 ;	   break ;

	case BHDM_ChannelAllocation_e_RRC_RLC__RR__RL__xx_LFE_FR__FL :
		ChannelMask = 0xF7 ;	   break ;

	case BHDM_ChannelAllocation_e_RRC_RLC__RR__RL__FC__xx_FR__FL:
		ChannelMask = 0xFB ;	   break ;

	 case BHDM_ChannelAllocation_e_RRC_RLC__RR__RL__FC_LFE_FR__FL :
		ChannelMask = 0xFF ;		break ;

	default : 
		BDBG_WRN(("UnSupported Speaker/Channel Mapping; %#X", 
			stAudioInfoFrame->SpeakerAllocation)) ;

	}
	/*CP*  10 Configure the MAI Bus */
	/****		Set Channel Mask	*/
	/* clear MAI_BIT_REVERSE bit  - reset value */
	/* set MAI_CHANNEL_MASK = 3   - reset value */

	Register = BCHP_FIELD_DATA(HDMI_MAI_CONFIG, MAI_BIT_REVERSE, 0) 
#if BHDM_CONFIG_AUDIO_MAI_BUS_DISABLE_SUPPORT
		| BCHP_FIELD_DATA(HDMI_MAI_CONFIG, DISABLE_MAI_AUDIO, DisableMai)
#endif
		| BCHP_FIELD_DATA(HDMI_MAI_CONFIG, MAI_CHANNEL_MASK, 0xFF) ;
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_MAI_CONFIG, Register) ; 

	/*CP*  11 Configure Audio */

	/* clear ZERO_DATA_ON_SAMPLE_FLAT		- reset value */
	/* clear AUDIO_SAMPLE_FLAT = 4'd0		- reset value */
	/* clear ZERO_DATA_ON_INACTIVE_CHANNELS - reset value */
	/* clear SAMPLE_PRESENT = 4'd0			- reset value */
	/* clear FORCE_SAMPLE_PRESENT			- reset value */
	/* clear FORCE_B_FRAME					- reset value */
	/* clear B_FRAME = 4'd0 				- reset value */
	/* clear B_FRAME_IDENTIFIER = 4'd1					  */
	/* clear AUDIO_LAYOUT					- reset value */
	/* clear FORCE_AUDIO_LAYOUT 			- reset value */
	/* clear AUDIO_CEA_MASK = 8'd0			- reset value */
	Register = 
		  BCHP_FIELD_DATA(HDMI_AUDIO_PACKET_CONFIG, ZERO_DATA_ON_SAMPLE_FLAT, 1) 
		| BCHP_FIELD_DATA(HDMI_AUDIO_PACKET_CONFIG, AUDIO_SAMPLE_FLAT, 0) 
		| BCHP_FIELD_DATA(HDMI_AUDIO_PACKET_CONFIG, ZERO_DATA_ON_INACTIVE_CHANNELS, 1) 
		| BCHP_FIELD_DATA(HDMI_AUDIO_PACKET_CONFIG, SAMPLE_PRESENT, 0) 
		| BCHP_FIELD_DATA(HDMI_AUDIO_PACKET_CONFIG, FORCE_SAMPLE_PRESENT, 0) 
		| BCHP_FIELD_DATA(HDMI_AUDIO_PACKET_CONFIG, FORCE_B_FRAME, 0) 
		| BCHP_FIELD_DATA(HDMI_AUDIO_PACKET_CONFIG, B_FRAME, 0) 
		| BCHP_FIELD_DATA(HDMI_AUDIO_PACKET_CONFIG, B_FRAME_IDENTIFIER, 1) 
		| BCHP_FIELD_DATA(HDMI_AUDIO_PACKET_CONFIG, AUDIO_LAYOUT, 0) 
		| BCHP_FIELD_DATA(HDMI_AUDIO_PACKET_CONFIG, FORCE_AUDIO_LAYOUT, 0) 
		| BCHP_FIELD_DATA(HDMI_AUDIO_PACKET_CONFIG, AUDIO_CEA_MASK, ChannelMask) ;
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_AUDIO_PACKET_CONFIG, Register) ; 


#if BHDM_CONFIG_DEBUG_AUDIO_INFOFRAME
	BDBG_MSG(("Channel Mask: %#x", ChannelMask)) ;
#endif
}


/******************************************************************************
Summary:
Set the color mode setting and update the General Control Packet to reflect the current color mode settings
*******************************************************************************/
BERR_Code BHDM_SetColorDepth(
   BHDM_Handle hHDMI,		   /* [in] HDMI handle */
   BHDM_ColorDepth_Settings *stColorDepthSettings
)
{
	BERR_Code rc = BERR_SUCCESS;
	uint32_t Register;
	uint8_t GcpSubPacketByte1 ;
	BAVC_HDMI_GCP_ColorDepth eColorDepth ;
	
	BDBG_ENTER(BHDM_SetColorDepth);

	/* Check bits per pixel settings */
	switch (stColorDepthSettings->eBitsPerPixel)
	{
	case BAVC_HDMI_BitsPerPixel_e24bit:
		/* regular 24bpp mode; no deep color */
		
		eColorDepth = BAVC_HDMI_GCP_ColorDepth_e24bpp ;
		GcpSubPacketByte1 = 0 ;
		break;

	
	case BAVC_HDMI_BitsPerPixel_e30bit:
		if (!hHDMI->AttachedEDID.RxVSDB.DeepColor_30bit)
		{
			BDBG_WRN(("Attached sink does not support 10-bit deep color mode"));
			rc = BERR_NOT_SUPPORTED;
			goto done;
		}

		eColorDepth = BAVC_HDMI_GCP_ColorDepth_e30bpp ;
		GcpSubPacketByte1 = BAVC_HDMI_GCP_ColorDepth_e30bpp ;
		break;

		
	case BAVC_HDMI_BitsPerPixel_e36bit:
		if (!hHDMI->AttachedEDID.RxVSDB.DeepColor_36bit)
		{
			BDBG_WRN(("Attached receiver does not support 12-bit deep color"));
			rc = BERR_NOT_SUPPORTED;
			goto done;
		}
	
		eColorDepth = BAVC_HDMI_GCP_ColorDepth_e36bpp ;
		GcpSubPacketByte1 = BAVC_HDMI_GCP_ColorDepth_e36bpp ;
		break;

	
	case BAVC_HDMI_BitsPerPixel_e48bit:
		BDBG_WRN(("16 bit deep color is not supported"));
		rc = BERR_NOT_SUPPORTED;
		goto done ;
		
	default:  /* use 24bpp if unknown color depth */
		BDBG_ERR(("Invalid Color Depth %d specified; default to 24bpp", 
			stColorDepthSettings->eBitsPerPixel));
	
		eColorDepth = BAVC_HDMI_GCP_ColorDepth_e24bpp ;
		GcpSubPacketByte1 = 0 ;
	
		break;
	}

	/* Configure color depth and GCP packets related register so that the hardware
		will update the packing phase accordingly */
	Register = BREG_Read32(hHDMI->hRegister, BCHP_HDMI_DEEP_COLOR_CONFIG_1); 
	Register &= ~BCHP_MASK(HDMI_DEEP_COLOR_CONFIG_1, COLOR_DEPTH) ;
	Register &= ~BCHP_MASK(HDMI_DEEP_COLOR_CONFIG_1, DEFAULT_PHASE) ;
	Register |= BCHP_FIELD_DATA(HDMI_DEEP_COLOR_CONFIG_1, COLOR_DEPTH, eColorDepth) ;
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_DEEP_COLOR_CONFIG_1, Register); 

	Register = BREG_Read32(hHDMI->hRegister, BCHP_HDMI_GCP_WORD_1); 	
	Register &= ~BCHP_MASK(HDMI_GCP_WORD_1, GCP_SUBPACKET_BYTE_1) ;
	Register |= BCHP_FIELD_DATA(HDMI_GCP_WORD_1, GCP_SUBPACKET_BYTE_1, GcpSubPacketByte1) ;
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_GCP_WORD_1, Register); 

	Register = BREG_Read32(hHDMI->hRegister, BCHP_HDMI_GCP_CONFIG); 
	Register &= ~BCHP_MASK(HDMI_GCP_CONFIG, GCP_ENABLE) ;
	Register |= BCHP_FIELD_DATA(HDMI_GCP_CONFIG, GCP_ENABLE, 1) ;
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_GCP_CONFIG, Register); 

	
done:
	BKNI_Memcpy(&hHDMI->DeviceSettings.stColorDepth, stColorDepthSettings, sizeof(BHDM_ColorDepth_Settings));
	BDBG_LEAVE(BHDM_SetColorDepth);
	return rc;

}


/******************************************************************************
Summary:
Get the current color depth setting
*******************************************************************************/
BERR_Code BHDM_GetColorDepth(
   BHDM_Handle hHDMI,		   /* [in] HDMI handle */
   BHDM_ColorDepth_Settings *stColorDepthSettings /* [out] color depth setting returns */
)
{	
	BDBG_ENTER(BHDM_GetColorDepth);
	BDBG_ASSERT(hHDMI);
	
	if (stColorDepthSettings)
		BKNI_Memcpy(stColorDepthSettings, &hHDMI->DeviceSettings.stColorDepth, sizeof(BHDM_ColorDepth_Settings));

	BDBG_LEAVE(BHDM_GetColorDepth);
	return BERR_SUCCESS;
}


/******************************************************************************
Summary:
	Set pixel data override
*******************************************************************************/
BERR_Code BHDM_SetPixelDataOverride(
	BHDM_Handle hHDMI,		   /* [in] HDMI handle */
	uint8_t red, 
	uint8_t green, 
	uint8_t blue
)
{
	BERR_Code rc = BERR_SUCCESS;
	uint32_t Register;

	uint16_t uiRed12bits   = red;
	uint16_t uiGreen12bits = green;
	uint16_t uiBlue12bits  = blue;

	BDBG_ENTER(BHDM_SetPixelDataOverride) ;


#if BHDM_CONFIG_PIXEL_OVERRIDE_UPDATE
	/* Red */
	Register = BREG_Read32(hHDMI->hRegister, BCHP_DVP_HT_TVG_BAR_CFG_1A) ;
	Register &= ~ BCHP_MASK(DVP_HT_TVG_BAR_CFG_1A, CH2);
	BREG_Write32(hHDMI->hRegister, BCHP_DVP_HT_TVG_BAR_CFG_1A, Register);

	Register |= BCHP_FIELD_DATA(DVP_HT_TVG_BAR_CFG_1A, CH2, (uiRed12bits << 4));
	BREG_Write32(hHDMI->hRegister, BCHP_DVP_HT_TVG_BAR_CFG_1A, Register);


	Register = BREG_Read32(hHDMI->hRegister, BCHP_DVP_HT_TVG_BAR_CFG_2A) ;
	Register &= ~ BCHP_MASK(DVP_HT_TVG_BAR_CFG_2A, CH2);
	BREG_Write32(hHDMI->hRegister, BCHP_DVP_HT_TVG_BAR_CFG_2A, Register);
	
	Register |= BCHP_FIELD_DATA(DVP_HT_TVG_BAR_CFG_2A, CH2, (uiRed12bits << 4));
	BREG_Write32(hHDMI->hRegister, BCHP_DVP_HT_TVG_BAR_CFG_2A, Register);


	/* Green */
	Register = BREG_Read32(hHDMI->hRegister, BCHP_DVP_HT_TVG_BAR_CFG_1A) ;
	Register &= ~ BCHP_MASK(DVP_HT_TVG_BAR_CFG_1A, CH1);
	BREG_Write32(hHDMI->hRegister, BCHP_DVP_HT_TVG_BAR_CFG_1A, Register);

	Register |= BCHP_FIELD_DATA(DVP_HT_TVG_BAR_CFG_1A, CH1, (uiGreen12bits << 4));
	BREG_Write32(hHDMI->hRegister, BCHP_DVP_HT_TVG_BAR_CFG_1A, Register);


	Register = BREG_Read32(hHDMI->hRegister, BCHP_DVP_HT_TVG_BAR_CFG_2A) ;
	Register &= ~ BCHP_MASK(DVP_HT_TVG_BAR_CFG_2A, CH1);
	BREG_Write32(hHDMI->hRegister, BCHP_DVP_HT_TVG_BAR_CFG_2A, Register);

	Register |= BCHP_FIELD_DATA(DVP_HT_TVG_BAR_CFG_2A, CH1, (uiGreen12bits << 4));
	BREG_Write32(hHDMI->hRegister, BCHP_DVP_HT_TVG_BAR_CFG_2A, Register);


	/* Blue */	
	Register = BREG_Read32(hHDMI->hRegister, BCHP_DVP_HT_TVG_BAR_CFG_1B) ;
	Register &= ~ BCHP_MASK(DVP_HT_TVG_BAR_CFG_1B, CH0);
	BREG_Write32(hHDMI->hRegister, BCHP_DVP_HT_TVG_BAR_CFG_1B, Register);
	
	Register |= BCHP_FIELD_DATA(DVP_HT_TVG_BAR_CFG_1B, CH0, (uiBlue12bits << 4));
	BREG_Write32(hHDMI->hRegister, BCHP_DVP_HT_TVG_BAR_CFG_1B, Register);
	
	
	Register = BREG_Read32(hHDMI->hRegister, BCHP_DVP_HT_TVG_BAR_CFG_2B) ;
	Register &= ~ BCHP_MASK(DVP_HT_TVG_BAR_CFG_2B, CH0);
	BREG_Write32(hHDMI->hRegister, BCHP_DVP_HT_TVG_BAR_CFG_2B, Register);
	
	Register |= BCHP_FIELD_DATA(DVP_HT_TVG_BAR_CFG_2B, CH0, (uiBlue12bits << 4));
	BREG_Write32(hHDMI->hRegister, BCHP_DVP_HT_TVG_BAR_CFG_2B, Register);


	/* Setup mode & Enable */
	Register = BREG_Read32(hHDMI->hRegister, BCHP_DVP_HT_TVG_CFG_0) ;
	Register &= ~( BCHP_MASK(DVP_HT_TVG_CFG_0, PATTERN_SELECT) 
				| BCHP_MASK(DVP_HT_TVG_CFG_0, TEST_MODE));
	BREG_Write32(hHDMI->hRegister, BCHP_DVP_HT_TVG_CFG_0, Register) ;

	Register |= BCHP_FIELD_DATA(DVP_HT_TVG_CFG_0, PATTERN_SELECT, 4)
			| BCHP_FIELD_DATA(DVP_HT_TVG_CFG_0, TEST_MODE, 3);
	BREG_Write32(hHDMI->hRegister, BCHP_DVP_HT_TVG_CFG_0, Register);

#else
	/* Red */
	Register = BREG_Read32(hHDMI->hRegister, BCHP_DVP_HT_HDMI_TX_0_TDG_CFG_10) ;
	Register &= 
		~( BCHP_MASK(DVP_HT_HDMI_TX_0_TDG_CFG_10, DVO_0_FLAT_FIELD_1_RED) 
		 | BCHP_MASK(DVP_HT_HDMI_TX_0_TDG_CFG_10, DVO_0_FLAT_FIELD_2_RED) );
	BREG_Write32(hHDMI->hRegister, BCHP_DVP_HT_HDMI_TX_0_TDG_CFG_10, Register);

	Register |= BCHP_FIELD_DATA(DVP_HT_HDMI_TX_0_TDG_CFG_10, DVO_0_FLAT_FIELD_1_RED, (uiRed12bits << 4))
			| BCHP_FIELD_DATA(DVP_HT_HDMI_TX_0_TDG_CFG_10, DVO_0_FLAT_FIELD_2_RED, (uiRed12bits << 4)) ;
	BREG_Write32(hHDMI->hRegister, BCHP_DVP_HT_HDMI_TX_0_TDG_CFG_10, Register);

	/* Green */
	Register = BREG_Read32(hHDMI->hRegister, BCHP_DVP_HT_HDMI_TX_0_TDG_CFG_11) ;
	Register &= 
		~( BCHP_MASK(DVP_HT_HDMI_TX_0_TDG_CFG_11, DVO_0_FLAT_FIELD_1_GREEN) 
		| BCHP_MASK(DVP_HT_HDMI_TX_0_TDG_CFG_11, DVO_0_FLAT_FIELD_2_GREEN) );
	BREG_Write32(hHDMI->hRegister, BCHP_DVP_HT_HDMI_TX_0_TDG_CFG_11, Register);

	Register |= BCHP_FIELD_DATA(DVP_HT_HDMI_TX_0_TDG_CFG_11, DVO_0_FLAT_FIELD_1_GREEN, (uiGreen12bits << 4))
			| BCHP_FIELD_DATA(DVP_HT_HDMI_TX_0_TDG_CFG_11, DVO_0_FLAT_FIELD_2_GREEN, (uiGreen12bits << 4)) ;
	BREG_Write32(hHDMI->hRegister, BCHP_DVP_HT_HDMI_TX_0_TDG_CFG_11, Register);

	/* Blue */
	Register = BREG_Read32(hHDMI->hRegister, BCHP_DVP_HT_HDMI_TX_0_TDG_CFG_12) ;
	Register &= 
		~( BCHP_MASK(DVP_HT_HDMI_TX_0_TDG_CFG_12, DVO_0_FLAT_FIELD_1_BLUE) 
		| BCHP_MASK(DVP_HT_HDMI_TX_0_TDG_CFG_12, DVO_0_FLAT_FIELD_2_BLUE) );
	BREG_Write32(hHDMI->hRegister, BCHP_DVP_HT_HDMI_TX_0_TDG_CFG_12, Register);

	Register |= BCHP_FIELD_DATA(DVP_HT_HDMI_TX_0_TDG_CFG_12, DVO_0_FLAT_FIELD_1_BLUE, (uiBlue12bits << 4))
			| BCHP_FIELD_DATA(DVP_HT_HDMI_TX_0_TDG_CFG_12, DVO_0_FLAT_FIELD_2_BLUE, (uiBlue12bits << 4)) ;
	BREG_Write32(hHDMI->hRegister, BCHP_DVP_HT_HDMI_TX_0_TDG_CFG_12, Register);


	/* Setup mode */
	Register = BREG_Read32(hHDMI->hRegister, BCHP_DVP_HT_HDMI_TX_0_TDG_CFG_13) ;
	Register &= ~BCHP_MASK(DVP_HT_HDMI_TX_0_TDG_CFG_13, DVO_0_GEN_TEST_MODE) ;
	BREG_Write32(hHDMI->hRegister, BCHP_DVP_HT_HDMI_TX_0_TDG_CFG_13, Register) ;

	Register |= BCHP_FIELD_DATA(DVP_HT_HDMI_TX_0_TDG_CFG_13, DVO_0_GEN_TEST_MODE, 4);
	BREG_Write32(hHDMI->hRegister, BCHP_DVP_HT_HDMI_TX_0_TDG_CFG_13, Register);


	/* Enable */
	Register = BREG_Read32(hHDMI->hRegister, BCHP_DVP_HT_HDMI_TX_0_TDG_CFG_0) ;
	Register &= ~BCHP_MASK(DVP_HT_HDMI_TX_0_TDG_CFG_0, DVO_0_TEST_MODE) ;
	BREG_Write32(hHDMI->hRegister, BCHP_DVP_HT_HDMI_TX_0_TDG_CFG_0, Register) ;

	Register |= BCHP_FIELD_DATA(DVP_HT_HDMI_TX_0_TDG_CFG_0, DVO_0_TEST_MODE, 3);
	BREG_Write32(hHDMI->hRegister, BCHP_DVP_HT_HDMI_TX_0_TDG_CFG_0, Register);
#endif

	BDBG_LEAVE(BHDM_SetPixelDataOverride) ;
	return rc;
}


/******************************************************************************
Summary:
	Clear pixel data override
*******************************************************************************/
BERR_Code BHDM_ClearPixelDataOverride(
	BHDM_Handle hHDMI		   /* [in] HDMI handle */
)
{
	BERR_Code rc = BERR_SUCCESS;
	uint32_t Register;

	BDBG_ENTER(BHDM_ClearPixelDataOverride) ;


#if BHDM_CONFIG_PIXEL_OVERRIDE_UPDATE

	/* Disable */
	Register = BREG_Read32(hHDMI->hRegister, BCHP_DVP_HT_TVG_CFG_0) ;
	Register &= ~ BCHP_MASK(DVP_HT_TVG_CFG_0, TEST_MODE);
	BREG_Write32(hHDMI->hRegister, BCHP_DVP_HT_TVG_CFG_0, Register) ;

	Register |= BCHP_FIELD_DATA(DVP_HT_TVG_CFG_0, TEST_MODE, 0);
	BREG_Write32(hHDMI->hRegister, BCHP_DVP_HT_TVG_CFG_0, Register);

#else
	/* Disable */
	Register = BREG_Read32(hHDMI->hRegister, BCHP_DVP_HT_HDMI_TX_0_TDG_CFG_0) ;
	Register &= ~BCHP_MASK(DVP_HT_HDMI_TX_0_TDG_CFG_0, DVO_0_TEST_MODE) ;
	BREG_Write32(hHDMI->hRegister, BCHP_DVP_HT_HDMI_TX_0_TDG_CFG_0, Register) ;

	Register |= BCHP_FIELD_DATA(DVP_HT_HDMI_TX_0_TDG_CFG_0, DVO_0_TEST_MODE, 0);
	BREG_Write32(hHDMI->hRegister, BCHP_DVP_HT_HDMI_TX_0_TDG_CFG_0, Register);
#endif

	BDBG_LEAVE(BHDM_ClearPixelDataOverride) ;
	return rc;
}


/******************************************************************************
Summary:
	Wait for stable video in HDMI core a specific amount of time	
*******************************************************************************/
BERR_Code BHDM_WaitForStableVideo(
	BHDM_Handle hHDMI,			/* [in] HDMI handle */
	uint32_t stablePeriod,		/* [in] Period of time video should be stable */
	uint32_t maxWait			/* [in] Max amount of time to wait */
)
{
	BERR_Code rc = BERR_TIMEOUT;
	uint32_t Register;
	
	uint32_t waitThusFar = 0;
	uint32_t stableTime  = 0;
	uint32_t waitIncr	 = 10;
	uint8_t bHPInterrupt = false;
	uint32_t driftFifoErrors = 0;
	uint32_t prevLineCount1 = 0;
	uint32_t prevLineCount2 = 0;
	uint32_t currLineCount;
	bool masterMode;

	BDBG_ENTER(BHDM_WaitForStableVideo);
	BHDM_ClearHotPlugInterrupt(hHDMI);

	BHDM_GetHdmiDataTransferMode(hHDMI, &masterMode);

	while (waitThusFar < maxWait) 
	{
		uint8_t notStable = false;

		/*
		* First, ensure video is really flowing in from the VEC
		*/
		Register = BREG_Read32(hHDMI->hRegister, BCHP_HDMI_FORMAT_DET_7) ;
		currLineCount = BCHP_GET_FIELD_DATA(Register, HDMI_FORMAT_DET_7, UUT_CURRENT_LINE_COUNT) ;
		if (currLineCount == prevLineCount1 && currLineCount == prevLineCount2)
		{
			notStable = true;
		}
		else
		{
			prevLineCount2 = prevLineCount1;
			prevLineCount1 = currLineCount;

			Register = BREG_Read32(hHDMI->hRegister, BCHP_HDMI_FORMAT_DET_UPDATE_STATUS) ;

			if (Register & (BCHP_MASK(HDMI_FORMAT_DET_UPDATE_STATUS, UPDATED_HAP)
							| BCHP_MASK(HDMI_FORMAT_DET_UPDATE_STATUS, UPDATED_VBLANK2)
							| BCHP_MASK(HDMI_FORMAT_DET_UPDATE_STATUS, UPDATED_VBLANK1)
							| BCHP_MASK(HDMI_FORMAT_DET_UPDATE_STATUS, UPDATED_HSYNC_HIGH)
							| BCHP_MASK(HDMI_FORMAT_DET_UPDATE_STATUS, UPDATED_HSYNC_LOW)
							| BCHP_MASK(HDMI_FORMAT_DET_UPDATE_STATUS, UPDATED_HFP)
							| BCHP_MASK(HDMI_FORMAT_DET_UPDATE_STATUS, UPDATED_HSP)
							| BCHP_MASK(HDMI_FORMAT_DET_UPDATE_STATUS, UPDATED_HBP)
							| BCHP_MASK(HDMI_FORMAT_DET_UPDATE_STATUS, UPDATED_VAL1)
							| BCHP_MASK(HDMI_FORMAT_DET_UPDATE_STATUS, UPDATED_VFP_1)
							| BCHP_MASK(HDMI_FORMAT_DET_UPDATE_STATUS, UPDATED_VBP_1)
							| BCHP_MASK(HDMI_FORMAT_DET_UPDATE_STATUS, UPDATED_VSPO_1)
							| BCHP_MASK(HDMI_FORMAT_DET_UPDATE_STATUS, UPDATED_VAL2)
							| BCHP_MASK(HDMI_FORMAT_DET_UPDATE_STATUS, UPDATED_VFP_2)
							| BCHP_MASK(HDMI_FORMAT_DET_UPDATE_STATUS, UPDATED_VBP_2)
							| BCHP_MASK(HDMI_FORMAT_DET_UPDATE_STATUS, UPDATED_VSPO_2)))
			{
				Register = BCHP_MASK(HDMI_FORMAT_DET_UPDATE_CLEAR, CLEAR_UPDATED_HAP)
							| BCHP_MASK(HDMI_FORMAT_DET_UPDATE_CLEAR, CLEAR_UPDATED_VBLANK2)
							| BCHP_MASK(HDMI_FORMAT_DET_UPDATE_CLEAR, CLEAR_UPDATED_VBLANK1)
							| BCHP_MASK(HDMI_FORMAT_DET_UPDATE_CLEAR, CLEAR_UPDATED_HSYNC_HIGH)
							| BCHP_MASK(HDMI_FORMAT_DET_UPDATE_CLEAR, CLEAR_UPDATED_HSYNC_LOW)
							| BCHP_MASK(HDMI_FORMAT_DET_UPDATE_CLEAR, CLEAR_UPDATED_HFP)
							| BCHP_MASK(HDMI_FORMAT_DET_UPDATE_CLEAR, CLEAR_UPDATED_HSP)
							| BCHP_MASK(HDMI_FORMAT_DET_UPDATE_CLEAR, CLEAR_UPDATED_HBP)
							| BCHP_MASK(HDMI_FORMAT_DET_UPDATE_CLEAR, CLEAR_UPDATED_VAL1)
							| BCHP_MASK(HDMI_FORMAT_DET_UPDATE_CLEAR, CLEAR_UPDATED_VFP_1)
							| BCHP_MASK(HDMI_FORMAT_DET_UPDATE_CLEAR, CLEAR_UPDATED_VBP_1)
							| BCHP_MASK(HDMI_FORMAT_DET_UPDATE_CLEAR, CLEAR_UPDATED_VSPO_1)
							| BCHP_MASK(HDMI_FORMAT_DET_UPDATE_CLEAR, CLEAR_UPDATED_VAL2)
							| BCHP_MASK(HDMI_FORMAT_DET_UPDATE_CLEAR, CLEAR_UPDATED_VFP_2)
							| BCHP_MASK(HDMI_FORMAT_DET_UPDATE_CLEAR, CLEAR_UPDATED_VBP_2)
							| BCHP_MASK(HDMI_FORMAT_DET_UPDATE_CLEAR, CLEAR_UPDATED_VSPO_2) ;

				BREG_Write32(hHDMI->hRegister, BCHP_HDMI_FORMAT_DET_UPDATE_CLEAR, Register) ;
				BREG_Write32(hHDMI->hRegister, BCHP_HDMI_FORMAT_DET_UPDATE_CLEAR, 0) ;
				notStable = true;
			}

			else if (masterMode == false)
			{
				/* 
				* Capture (pointers) status before we read it.
				*/
				Register = BREG_Read32( hHDMI->hRegister, BCHP_HDMI_FIFO_CTL) ;

				Register &= ~BCHP_MASK(HDMI_FIFO_CTL, CAPTURE_POINTERS) ;
				BREG_Write32(hHDMI->hRegister, BCHP_HDMI_FIFO_CTL, Register) ;

				Register |= BCHP_FIELD_DATA(HDMI_FIFO_CTL, CAPTURE_POINTERS, 1) ;
				BREG_Write32(hHDMI->hRegister, BCHP_HDMI_FIFO_CTL, Register) ;

				Register = BREG_Read32(hHDMI->hRegister, BCHP_HDMI_READ_POINTERS) ;
				if (Register & (BCHP_MASK(HDMI_READ_POINTERS, DRIFT_UNDERFLOW)
								| BCHP_MASK(HDMI_READ_POINTERS, DRIFT_OVERFLOW)))
				{
					notStable = true;

					/*
					* Re-center the Drift FIFO if we get excessive overflow or underflow
					* errors. There is a bug with the 76xx where the auto re-center
					* logic (use_full, use_empty) does not work as expected in terms of 
					* clearing these errors.
					*/
					if (++driftFifoErrors % 10 == 0)
					{
						Register = BREG_Read32( hHDMI->hRegister, BCHP_HDMI_FIFO_CTL) ;

						Register &= ~BCHP_MASK(HDMI_FIFO_CTL, RECENTER) ;
						BREG_Write32(hHDMI->hRegister, BCHP_HDMI_FIFO_CTL, Register) ;

						Register |= BCHP_FIELD_DATA(HDMI_FIFO_CTL, RECENTER, 1) ;
						BREG_Write32(hHDMI->hRegister, BCHP_HDMI_FIFO_CTL, Register) ;
					}	
				}
			}
		}

		BKNI_Sleep(waitIncr);
		waitThusFar += waitIncr;

		if (notStable == false)
		{
			stableTime += waitIncr;
			if (stableTime >= stablePeriod)
			{
				rc = BERR_SUCCESS;
				goto done;
			}
		}
		else
		{
			stableTime = 0;
		}
		
		BHDM_CheckHotPlugInterrupt(hHDMI, &bHPInterrupt);
		if (bHPInterrupt == true)
			goto done;
	}

done:
	BDBG_LEAVE(BHDM_WaitForStableVideo);
	return rc;
	
}


/**********************************
**  		PRIVATE FUNCTIONS 
***********************************/
void BHDM_P_ResetHdmiCore (BHDM_Handle hHDMI)
{	

	/* Reset the HDMI core */
	
	BREG_Write32(hHDMI->hRegister, BCHP_SUN_TOP_CTRL_SW_INIT_0_SET,
		BCHP_FIELD_DATA(SUN_TOP_CTRL_SW_INIT_0_SET, dvp_ht_sw_init, 1));

	BREG_Write32(hHDMI->hRegister, BCHP_SUN_TOP_CTRL_SW_INIT_0_CLEAR,
		BCHP_FIELD_DATA(SUN_TOP_CTRL_SW_INIT_0_CLEAR, dvp_ht_sw_init, 1));
	
	return;
}


void BHDM_P_PowerOffPhy (BHDM_Handle hHDMI)
{	
	uint32_t Register;
	BREG_Handle hRegister = hHDMI->hRegister ;

	Register = BREG_Read32(hRegister, BCHP_HDMI_TX_PHY_POWERDOWN_CTL);
	Register |=
		  BCHP_MASK(HDMI_TX_PHY_POWERDOWN_CTL, RNDGEN_PWRDN)
		| BCHP_MASK(HDMI_TX_PHY_POWERDOWN_CTL, PLL_PWRDN) 
		| BCHP_MASK(HDMI_TX_PHY_POWERDOWN_CTL, BIAS_PWRDN)
		| BCHP_MASK(HDMI_TX_PHY_POWERDOWN_CTL, PHY_PWRDN) ;
	BREG_Write32(hRegister, BCHP_HDMI_TX_PHY_POWERDOWN_CTL, Register) ;
}


void BHDM_P_PowerOnPhy (BHDM_Handle hHDMI)
{	
	uint32_t Register;
	bool masterMode;

	BHDM_GetHdmiDataTransferMode(hHDMI, &masterMode);

#if BHDM_CONFIG_CLOCK_STOP_SUPPORT
	Register = BREG_Read32(hHDMI->hRegister, BCHP_DVP_HT_CLOCK_STOP);
	Register &= ~ BCHP_MASK(DVP_HT_CLOCK_STOP, PIXEL);
	BREG_Write32(hHDMI->hRegister, BCHP_DVP_HT_CLOCK_STOP, Register) ;
#endif

#if BHDM_CONFIG_SWAP_DEFAULT_PHY_CHANNELS
	BDBG_LOG(("HDMI Tx Phy CK and CH2 signals will be swapped")) ;
#endif

	/* Assert the fields to prevent DRIFT FIFO UNDERFLOW when trying to 
		authenticate with DVI receivers */
	Register = BREG_Read32(hHDMI->hRegister, BCHP_HDMI_FIFO_CTL);
	Register &= ~( BCHP_MASK(HDMI_FIFO_CTL, USE_EMPTY)
				| BCHP_MASK(HDMI_FIFO_CTL, USE_FULL));

	Register |= BCHP_FIELD_DATA(HDMI_FIFO_CTL, USE_EMPTY, 1)
				| BCHP_FIELD_DATA(HDMI_FIFO_CTL, USE_FULL, masterMode?0:1);
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_FIFO_CTL, Register) ;

	/* PR 28685: Enable Random bit block power down. */
	/* Power PLL, etc. */
	Register = BREG_Read32(hHDMI->hRegister, BCHP_HDMI_TX_PHY_POWERDOWN_CTL);
	Register &= ~( BCHP_MASK(HDMI_TX_PHY_POWERDOWN_CTL, RNDGEN_PWRDN)
				| BCHP_MASK(HDMI_TX_PHY_POWERDOWN_CTL, PLL_PWRDN) 
				| BCHP_MASK(HDMI_TX_PHY_POWERDOWN_CTL, BIAS_PWRDN)
				| BCHP_MASK(HDMI_TX_PHY_POWERDOWN_CTL, PHY_PWRDN)) ;
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_TX_PHY_POWERDOWN_CTL, Register) ;
	

	/* Bring PLL PHY out of reset */
	Register = BREG_Read32(hHDMI->hRegister, BCHP_HDMI_TX_PHY_RESET_CTL);
	Register &= ~( BCHP_MASK(HDMI_TX_PHY_RESET_CTL, PLL_RESETB)
				| BCHP_MASK(HDMI_TX_PHY_RESET_CTL, PLLDIV_RSTB)) ;
	
	Register |= BCHP_FIELD_DATA(HDMI_TX_PHY_RESET_CTL, PLL_RESETB, 1)
				| BCHP_FIELD_DATA(HDMI_TX_PHY_RESET_CTL, PLLDIV_RSTB, 1);
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_TX_PHY_RESET_CTL, Register) ;

		
#if BHDM_CONFIG_SWAP_DEFAULT_PHY_CHANNELS
	/* Program HDMI_TX_PHY.CHANNEL_SWAP with defaults; required swaps located in bhdm_config.h */

	Register = BREG_Read32(hHDMI->hRegister, BCHP_HDMI_TX_PHY_CHANNEL_SWAP);
		Register &= ~( 
			  BCHP_MASK(HDMI_TX_PHY_CHANNEL_SWAP, TXCK_OUT_INV)
				| BCHP_MASK(HDMI_TX_PHY_CHANNEL_SWAP, TXCK_OUT_SEL)
				| BCHP_MASK(HDMI_TX_PHY_CHANNEL_SWAP, TX2_OUT_INV)	
				| BCHP_MASK(HDMI_TX_PHY_CHANNEL_SWAP, TX2_OUT_SEL)
				| BCHP_MASK(HDMI_TX_PHY_CHANNEL_SWAP, TX1_OUT_INV)
				| BCHP_MASK(HDMI_TX_PHY_CHANNEL_SWAP, TX1_OUT_SEL)
				| BCHP_MASK(HDMI_TX_PHY_CHANNEL_SWAP, TX0_OUT_INV)
				| BCHP_MASK(HDMI_TX_PHY_CHANNEL_SWAP, TX0_OUT_SEL));
	
		Register |= 
			  BCHP_FIELD_DATA(HDMI_TX_PHY_CHANNEL_SWAP, TXCK_OUT_INV, 
						BHDM_CONFIG_HDMI_TX_PHY_CHANNEL_SWAP_TXCK_OUT_INV)
				| BCHP_FIELD_DATA(HDMI_TX_PHY_CHANNEL_SWAP, TXCK_OUT_SEL, 
						BHDM_CONFIG_HDMI_TX_PHY_CHANNEL_SWAP_TXCK_OUT_SEL)
				| BCHP_FIELD_DATA(HDMI_TX_PHY_CHANNEL_SWAP, TX2_OUT_INV, 
						BHDM_CONFIG_HDMI_TX_PHY_CHANNEL_SWAP_TX2_OUT_INV)
				| BCHP_FIELD_DATA(HDMI_TX_PHY_CHANNEL_SWAP, TX2_OUT_SEL, 
						BHDM_CONFIG_HDMI_TX_PHY_CHANNEL_SWAP_TX2_OUT_SEL)
				| BCHP_FIELD_DATA(HDMI_TX_PHY_CHANNEL_SWAP, TX1_OUT_INV, 
						BHDM_CONFIG_HDMI_TX_PHY_CHANNEL_SWAP_TX1_OUT_INV)
				| BCHP_FIELD_DATA(HDMI_TX_PHY_CHANNEL_SWAP, TX1_OUT_SEL, 
						BHDM_CONFIG_HDMI_TX_PHY_CHANNEL_SWAP_TX1_OUT_SEL)
				| BCHP_FIELD_DATA(HDMI_TX_PHY_CHANNEL_SWAP, TX0_OUT_INV, 
						BHDM_CONFIG_HDMI_TX_PHY_CHANNEL_SWAP_TX0_OUT_INV)
				| BCHP_FIELD_DATA(HDMI_TX_PHY_CHANNEL_SWAP, TX0_OUT_SEL, 
						BHDM_CONFIG_HDMI_TX_PHY_CHANNEL_SWAP_TX0_OUT_SEL);
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_TX_PHY_CHANNEL_SWAP, Register) ;
#endif 

	return;
}


void BHDM_P_SetPreEmphasisMode (BHDM_Handle hHDMI, uint8_t uValue, uint8_t uDriverAmp)
{
	uint32_t Register;
	
	Register = BREG_Read32( hHDMI->hRegister, BCHP_HDMI_TX_PHY_CTL_0) ;
	Register &= ~(BCHP_MASK(HDMI_TX_PHY_CTL_0, PREEMP_2)
			  | BCHP_MASK(HDMI_TX_PHY_CTL_0, PREEMP_1)
			  | BCHP_MASK(HDMI_TX_PHY_CTL_0, PREEMP_0));

	Register |= BCHP_FIELD_DATA(HDMI_TX_PHY_CTL_0, PREEMP_2, uValue)
			| BCHP_FIELD_DATA(HDMI_TX_PHY_CTL_0, PREEMP_1, uValue)
			| BCHP_FIELD_DATA(HDMI_TX_PHY_CTL_0, PREEMP_0, uValue) ;
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_TX_PHY_CTL_0, Register) ;


	Register = BREG_Read32( hHDMI->hRegister, BCHP_HDMI_TX_PHY_CTL_1) ;
	Register &= ~(BCHP_MASK(HDMI_TX_PHY_CTL_1, PREEMP_CK));
	Register |= BCHP_FIELD_DATA(HDMI_TX_PHY_CTL_1, PREEMP_CK, uValue);
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_TX_PHY_CTL_1, Register) ;

	BSTD_UNUSED(uDriverAmp);
	return;
}


BERR_Code BHDM_P_GetPreEmphasisConfiguration (
	BHDM_Handle hHDMI,
	BHDM_PreEmphasis_Configuration *stPreEmphasisConfig
)
{
	BERR_Code rc = BERR_SUCCESS;
	uint32_t Register;

	Register = BREG_Read32( hHDMI->hRegister, BCHP_HDMI_TX_PHY_CTL_0) ;
	stPreEmphasisConfig->uiHfEn = BCHP_GET_FIELD_DATA(Register, HDMI_TX_PHY_CTL_0, HF_EN);
	stPreEmphasisConfig->uiPreEmphasis_Ch2 = BCHP_GET_FIELD_DATA(Register,HDMI_TX_PHY_CTL_0,PREEMP_2);
	stPreEmphasisConfig->uiPreEmphasis_Ch1 = BCHP_GET_FIELD_DATA(Register,HDMI_TX_PHY_CTL_0,PREEMP_1);
	stPreEmphasisConfig->uiPreEmphasis_Ch0 = BCHP_GET_FIELD_DATA(Register,HDMI_TX_PHY_CTL_0,PREEMP_0);

	Register = BREG_Read32( hHDMI->hRegister, BCHP_HDMI_TX_PHY_CTL_1) ;
	stPreEmphasisConfig->uiPreEmphasis_CK = BCHP_GET_FIELD_DATA(Register,HDMI_TX_PHY_CTL_1,PREEMP_CK);
	stPreEmphasisConfig->uiCurrentRatioSel = BCHP_GET_FIELD_DATA(Register, HDMI_TX_PHY_CTL_1, CURRENT_RATIO_SEL);
	
	Register = BREG_Read32( hHDMI->hRegister, BCHP_HDMI_TX_PHY_CTL_2) ;
	stPreEmphasisConfig->uiKP = BCHP_GET_FIELD_DATA(Register, HDMI_TX_PHY_CTL_2, KP);
	stPreEmphasisConfig->uiKI = BCHP_GET_FIELD_DATA(Register, HDMI_TX_PHY_CTL_2, KI);
	stPreEmphasisConfig->uiKA = BCHP_GET_FIELD_DATA(Register, HDMI_TX_PHY_CTL_2, KA);

	return rc;
}


BERR_Code BHDM_P_SetPreEmphasisConfiguration(
	BHDM_Handle hHDMI,
	BHDM_PreEmphasis_Configuration *stPreEmphasisConfig)
{
	BERR_Code rc = BERR_SUCCESS;
	uint32_t Register;
	
	/* Set Preemphasis configurations */
	Register = BREG_Read32( hHDMI->hRegister, BCHP_HDMI_TX_PHY_CTL_0) ;
	Register &= ~(
		 BCHP_MASK(HDMI_TX_PHY_CTL_0, HF_EN)
		| BCHP_MASK(HDMI_TX_PHY_CTL_0, PREEMP_2)
		| BCHP_MASK(HDMI_TX_PHY_CTL_0, PREEMP_1)
		| BCHP_MASK(HDMI_TX_PHY_CTL_0, PREEMP_0)) ;
		
	Register |=
		 BCHP_FIELD_DATA(HDMI_TX_PHY_CTL_0, HF_EN, stPreEmphasisConfig->uiHfEn)
		| BCHP_FIELD_DATA(HDMI_TX_PHY_CTL_0, PREEMP_2, stPreEmphasisConfig->uiPreEmphasis_Ch2)
		| BCHP_FIELD_DATA(HDMI_TX_PHY_CTL_0, PREEMP_1, stPreEmphasisConfig->uiPreEmphasis_Ch1)
		| BCHP_FIELD_DATA(HDMI_TX_PHY_CTL_0, PREEMP_0, stPreEmphasisConfig->uiPreEmphasis_Ch0) ;
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_TX_PHY_CTL_0, Register) ;

	Register = BREG_Read32( hHDMI->hRegister, BCHP_HDMI_TX_PHY_CTL_1) ;
	Register &= ~ (
		 BCHP_MASK(HDMI_TX_PHY_CTL_1, PREEMP_CK)
		| BCHP_MASK(HDMI_TX_PHY_CTL_1, CURRENT_RATIO_SEL));
	Register |= 
		 BCHP_FIELD_DATA(HDMI_TX_PHY_CTL_1, PREEMP_CK, stPreEmphasisConfig->uiPreEmphasis_CK)
		| BCHP_FIELD_DATA(HDMI_TX_PHY_CTL_1, CURRENT_RATIO_SEL, stPreEmphasisConfig->uiCurrentRatioSel);
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_TX_PHY_CTL_1, Register) ;

	Register = BREG_Read32( hHDMI->hRegister, BCHP_HDMI_TX_PHY_CTL_2) ;
	Register &= ~ (
		 BCHP_MASK(HDMI_TX_PHY_CTL_2, KP)
		| BCHP_MASK(HDMI_TX_PHY_CTL_2, KI)
		| BCHP_MASK(HDMI_TX_PHY_CTL_2, KA));
	Register |= 
		 BCHP_FIELD_DATA(HDMI_TX_PHY_CTL_2, KP, stPreEmphasisConfig->uiKP)
		| BCHP_FIELD_DATA(HDMI_TX_PHY_CTL_2, KI, stPreEmphasisConfig->uiKI)
		| BCHP_FIELD_DATA(HDMI_TX_PHY_CTL_2, KA, stPreEmphasisConfig->uiKA);
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_TX_PHY_CTL_2, Register) ;


	return rc;
}


void BHDM_P_ClearHotPlugInterrupt(
   BHDM_Handle hHDMI		/* [in] HDMI handle */
)
{
	uint32_t Register ;

#if BHDM_CONFIG_DUAL_HPD_SUPPORT
	/* reset boolean status */
	hHDMI->hotplugInterruptFired = false;
	BSTD_UNUSED(Register);
	
#else
	Register = BREG_Read32(hHDMI->hRegister, BCHP_AON_HDMI_TX_HDMI_HOTPLUG_CONTROL) ;
	Register &= ~BCHP_MASK(AON_HDMI_TX_HDMI_HOTPLUG_CONTROL, CLEAR_HOTPLUG_INT_STATUS) ;
	Register |= BCHP_FIELD_DATA(AON_HDMI_TX_HDMI_HOTPLUG_CONTROL, CLEAR_HOTPLUG_INT_STATUS, 1) ;
	BREG_Write32(hHDMI->hRegister, BCHP_AON_HDMI_TX_HDMI_HOTPLUG_CONTROL, Register) ;

	Register &= ~BCHP_MASK(AON_HDMI_TX_HDMI_HOTPLUG_CONTROL, CLEAR_HOTPLUG_INT_STATUS) ;
	Register |= BCHP_FIELD_DATA(AON_HDMI_TX_HDMI_HOTPLUG_CONTROL, CLEAR_HOTPLUG_INT_STATUS, 0) ;
	BREG_Write32(hHDMI->hRegister, BCHP_AON_HDMI_TX_HDMI_HOTPLUG_CONTROL, Register) ;
#endif

	return;
}


void BHDM_P_CheckHotPlugInterrupt(
	BHDM_Handle hHDMI,		 /* [in] HDMI handle */
	uint8_t *bHotPlugInterrupt	/* [out] Interrupt asserted or not */
)
{
	uint32_t Register ;

#if BHDM_CONFIG_DUAL_HPD_SUPPORT
	*bHotPlugInterrupt = hHDMI->hotplugInterruptFired;
	BSTD_UNUSED(Register);

#else
	Register = BREG_Read32(hHDMI->hRegister, BCHP_AON_HDMI_TX_HDMI_HOTPLUG_STATUS) ;

	if (Register & BCHP_MASK(AON_HDMI_TX_HDMI_HOTPLUG_STATUS, HOTPLUG_INT_STATUS))
		*bHotPlugInterrupt = true;
	else
		*bHotPlugInterrupt = false;
#endif

	return;
}


void BHDM_P_RxDeviceAttached_isr(
	BHDM_Handle hHDMI,		 /* [in] HDMI handle */
	uint8_t *bDeviceAttached	/* [out] Device Attached Status  */
)
{
	uint32_t Register ;

	BDBG_ENTER(BHDM_P_RxDeviceAttached) ;

	Register = BREG_Read32(hHDMI->hRegister, BCHP_HDMI_HOTPLUG_STATUS) ;
	*bDeviceAttached = 
		BCHP_GET_FIELD_DATA(Register, HDMI_HOTPLUG_STATUS, HOTPLUG_STATUS) ;

	BDBG_LEAVE(BHDM_P_RxDeviceAttached) ;
	return ;
}


/******************************************************************************
Summary:
	Enable/Disable FIFO interrupts for debuging
*******************************************************************************/
#if BHDM_CONFIG_DEBUG_FIFO
BERR_Code BHDM_P_EnableFIFOInterrupts(
	BHDM_Handle hHDMI, 
	bool on)
{
	BERR_Code	   rc = BERR_SUCCESS ;
	uint32_t Register ; 
	
	if (on)
	{
		/* clear any pending interrupts first */
		Register = 
			  BCHP_FIELD_DATA(HDMI_TX_INTR2_CPU_CLEAR, DRIFT_FIFO_FULL_MINUS_INTR, 1) 
			| BCHP_FIELD_DATA(HDMI_TX_INTR2_CPU_CLEAR, DRIFT_FIFO_ALMOST_FULL_INTR, 1) 
			| BCHP_FIELD_DATA(HDMI_TX_INTR2_CPU_CLEAR, DRIFT_FIFO_EMPTY_MINUS_INTR, 1) 
			| BCHP_FIELD_DATA(HDMI_TX_INTR2_CPU_CLEAR, DRIFT_FIFO_ALMOST_EMPTY_INTR, 1) ;
		BREG_Write32(hHDMI->hRegister, BCHP_HDMI_TX_INTR2_CPU_CLEAR, Register) ;
			

		/* enable the interrupts */ 		  
		BHDM_CHECK_RC( rc, BINT_EnableCallback( hHDMI->hCallback[MAKE_INTR_ENUM(DF_FULL_MINUS)] )) ;
		BHDM_CHECK_RC( rc, BINT_EnableCallback( hHDMI->hCallback[MAKE_INTR_ENUM(DF_ALMOST_FULL)])) ; 
		BHDM_CHECK_RC( rc, BINT_EnableCallback( hHDMI->hCallback[MAKE_INTR_ENUM(DF_EMPTY_MINUS)])) ;  
		BHDM_CHECK_RC( rc, BINT_EnableCallback( hHDMI->hCallback[MAKE_INTR_ENUM(DF_ALMOST_EMPTY)])) ; 
		
		/* disable any interrupt masks; so we can see the interrupts if they occur */
		Register = 
			  BCHP_FIELD_DATA(HDMI_TX_INTR2_CPU_MASK_CLEAR, DRIFT_FIFO_FULL_MINUS_INTR, 1) 
			| BCHP_FIELD_DATA(HDMI_TX_INTR2_CPU_MASK_CLEAR, DRIFT_FIFO_ALMOST_FULL_INTR, 1) 
			| BCHP_FIELD_DATA(HDMI_TX_INTR2_CPU_MASK_CLEAR, DRIFT_FIFO_EMPTY_MINUS_INTR, 1) 
			| BCHP_FIELD_DATA(HDMI_TX_INTR2_CPU_MASK_CLEAR, DRIFT_FIFO_ALMOST_EMPTY_INTR, 1) ;
		BREG_Write32(hHDMI->hRegister, BCHP_HDMI_TX_INTR2_CPU_MASK_CLEAR, Register) ;
	}
	else
	{
		/* disable the interrupt callbacks */		
		BHDM_CHECK_RC( rc, BINT_DisableCallback( hHDMI->hCallback[MAKE_INTR_ENUM(DF_FULL_MINUS)])) ;
		BHDM_CHECK_RC( rc, BINT_DisableCallback( hHDMI->hCallback[MAKE_INTR_ENUM(DF_ALMOST_FULL)])) ; 
		BHDM_CHECK_RC( rc, BINT_DisableCallback( hHDMI->hCallback[MAKE_INTR_ENUM(DF_EMPTY_MINUS)])) ;  
		BHDM_CHECK_RC( rc, BINT_DisableCallback( hHDMI->hCallback[MAKE_INTR_ENUM(DF_ALMOST_EMPTY)])) ;
		
		/* mask the interrupts;  */
		Register = 
			  BCHP_FIELD_DATA(HDMI_TX_INTR2_CPU_MASK_SET, DRIFT_FIFO_FULL_MINUS_INTR, 1) 
			| BCHP_FIELD_DATA(HDMI_TX_INTR2_CPU_MASK_SET, DRIFT_FIFO_ALMOST_FULL_INTR, 1) 
			| BCHP_FIELD_DATA(HDMI_TX_INTR2_CPU_MASK_SET, DRIFT_FIFO_EMPTY_MINUS_INTR, 1) 
			| BCHP_FIELD_DATA(HDMI_TX_INTR2_CPU_MASK_SET, DRIFT_FIFO_ALMOST_EMPTY_INTR, 1) ;
		BREG_Write32(hHDMI->hRegister, BCHP_HDMI_TX_INTR2_CPU_MASK_SET, Register) ;
	} 

done:
	return rc ;
}
#endif


