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
 * $brcm_Workfile: bhdm_priv_dvpht.c $
 * $brcm_Revision: Hydra_Software_Devel/37 $
 * $brcm_Date: 9/10/12 1:42p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/hdm/src/core65nm/bhdm_priv_dvpht.c $
 * 
 * Hydra_Software_Devel/37   9/10/12 1:42p rgreen
 * SW7420-2349: Update BHDM_P_RxDeviceAttaced to isr function.  Update
 * calls to add Enter/Leave Critical Section
 * 
 * Hydra_Software_Devel/36   8/24/12 5:13p vle
 * SW7425-3803:Add Phy PowerDown for standby/close
 * 
 * Hydra_Software_Devel/35   6/14/12 10:44a rgreen
 * SW7425-3206, SW7425-3241:  Enable/Disable RSEN interrupts to match
 * state of the TMDS lines
 * 
 * Hydra_Software_Devel/34   5/9/12 4:58p rgreen
 * SW7422-467,SW7231-797,SW7358-263: Apply TMDS changes for 65nm. When
 * disabling TMDS lines, keep RSEN interrupt enabled and the TMDS Clock
 * enabled for detecting Receiver Power changes
 * 
 * Hydra_Software_Devel/33   4/16/12 1:27p rgreen
 * SW7420-2285: Fix unused variable compilation warning in
 * BHDM_P_ConfigurePhy
 * 
 * Hydra_Software_Devel/32   4/9/12 3:12p vle
 * SW7420-2285: Merged to mainline
 * 
 * Hydra_Software_Devel/SW7420-2285/1   4/6/12 11:24a vle
 * SW7420-2285: Rework BHDM_P_ConfigurePhy to avoid possible bogus pre-emp
 * settings during format changes.
 * 
 * Hydra_Software_Devel/31   3/16/12 2:58p vle
 * SW7425-2515: Remove unused CEC code which is now located in CEC pi
 * 
 * Hydra_Software_Devel/30   2/16/12 11:38a rgreen
 * SW7125-1146,SW7408-317:  Protect Hot Plug interrupt register access
 * during task time.  Fix leave macro
 * 
 * Hydra_Software_Devel/29   2/9/12 4:52p rgreen
 * SW7125-1146,SW7425-2361: Refactor HDMI Power Management; separate TMDS
 * power from clock
 * 
 * Hydra_Software_Devel/28   1/27/12 2:10p vle
 * SW7125-1146: merge to mainline
 * 
 * Hydra_Software_Devel/SW7125-1146/2   1/26/12 6:09p vle
 * SW7125-1146: Remove GetReceiverSense private function.
 * 
 * Hydra_Software_Devel/27   1/23/12 11:25a rgreen
 * SW7125-1146: Merge Changes
 * 
 * Hydra_Software_Devel/SW7125-1146/1   1/19/12 2:34p rgreen
 * SW7125-1146, SW7231-345,SW7346-234: Remove enable/disable of RSEN
 * interrupt from EnableTmdsOutput; Get RSEN setting at isr vs event time
 * 
 * Hydra_Software_Devel/26   1/16/12 6:58p vle
 * SW7401-4424: Power down TMDS lines after checking receiver sense if
 * TMDS lines were previously disabled.
 * 
 * Hydra_Software_Devel/25   11/14/11 4:08p rgreen
 * SW7425-1710: Add private function for BHDM_RxDeviceAttached
 * 
 * Hydra_Software_Devel/24   11/4/11 4:08p rgreen
 * SW7125-1128,SW7125-1138:  Use register based General Control Packet
 * only when attached receiver supports Deep Color
 * 
 * Hydra_Software_Devel/23   10/28/11 6:12p vle
 * SW7125-1128: Remove redundant code
 * 
 * Hydra_Software_Devel/22   10/26/11 1:51p rgreen
 * SW7125-1128: Merge Changes
 * 
 * Hydra_Software_Devel/SW7125-1128/2   10/25/11 9:41a rgreen
 * SW7125-1128,SW7422-114:  Move goto statement from debug block to
 * regular code block
 * 
 * Hydra_Software_Devel/SW7125-1128/1   10/21/11 5:44p rgreen
 * SW7125-1128,SW7422-114: force use of hw GCP for all 40nm chips, do not
 * modify RAM GCP packet/transmission for non-deep color mode
 * 
 * Hydra_Software_Devel/21   9/16/11 1:21p vle
 * SW7550-767: USE_FULL field should not be use while HDMI in master mode.
 * 
 * Hydra_Software_Devel/20   9/15/11 5:08p vle
 * SW7231-345: Disable RxSense interrupt when TDMS lines are disabled.
 * 
 * Hydra_Software_Devel/19   7/27/11 6:34p vle
 * SW7408-290,SW7346-348: back out changes in SW7408-290. Make sure not to
 * toggle TMDS lines when checking for RxSense.
 * 
 * Hydra_Software_Devel/18   6/28/11 6:43p vle
 * SW7346-234: Disable RSEN interrupt until the PHY is powered on
 * 
 * Hydra_Software_Devel/17   6/23/11 3:13p vle
 * SW7408-290: Merged to mainline
 * 
 * Hydra_Software_Devel/SW7408-290/2   6/22/11 6:48p vle
 * SW7408-290: Fix bug in first attempt
 * 
 * Hydra_Software_Devel/SW7408-290/1   6/21/11 7:23p vle
 * SW7408-290: Make sure to disable TMDS lines appropriately when checking
 * for receiver sense.
 * 
 * Hydra_Software_Devel/16   4/22/11 11:54a vle
 * SW7125-798: Add NEXUS API to allow customer to control pre-emphasis
 * settings
 * 
 * Hydra_Software_Devel/15   3/21/11 7:04p vle
 * SW7550-664: Fix HDMI 5.1 output channel order
 * 
 * Hydra_Software_Devel/14   2/10/11 7:44p vle
 * SW7550-672: Fix GCP packet handlings for 65nm platforms with HDMI 1.3
 * support.
 * 
 * Hydra_Software_Devel/SW7125-798/1   2/3/11 11:27a vle
 * SW7125-798: Add NEXUS API to allow customer to control pre-emphasis
 * settings
 * 
 * Hydra_Software_Devel/13   12/9/10 6:02p vle
 * SW7420-1177: Add DVO support for 7420
 * 
 * Hydra_Software_Devel/12   10/7/10 6:47p jtna
 * SW7420-972: merge hdmi power management
 * 
 * Hydra_Software_Devel/SW7420-972/1   10/6/10 7:01p jtna
 * SW7420-972: BCHP_PWR power management for HDM
 * 
 * Hydra_Software_Devel/11   9/29/10 4:14p vle
 * SW7422-23: Fix build issues without CEC enable
 * 
 * Hydra_Software_Devel/10   9/28/10 7:19p vle
 * SW7422-23: Refactor HDMI code to isolate platform dependent code
 * furthermore. Add support for 7422 and other 40nm platforms.
 * 
 * Hydra_Software_Devel/9   9/24/10 5:38p vle
 * SW7342-238: Take 2 The VEC will only operate double the rate (54Mhz) in
 * 480p format, not 480i. Make sure audio parameters for all other 27Mhz
 * pixel clock format are programmed correctly.
 * 
 * Hydra_Software_Devel/8   9/24/10 2:25p vle
 * SW7342-238: The VEC will only operate double the rate (54Mhz) in 480p
 * format, not 480i. Make sure audio parameters for all other 27Mhz pixel
 * clock format are programmed correctly.
 * 
 * Hydra_Software_Devel/7   8/18/10 5:51p vle
 * SW7342-238: add comment
 * 
 * Hydra_Software_Devel/6   8/18/10 5:46p vle
 * SW7342-238: work around to fix distorted audio issue on 480p. For 480p
 * format, VEC will always provide rate of 54Mhz in the ratechange
 * callback to HDMI, regardless of whether pixel repetition is enabled.
 * 
 * Hydra_Software_Devel/5   5/26/10 2:48p vle
 * SW7405-4333: Add support for 40, 65, and 65/1.001 Mhz pixel clock rate
 * 
 * Hydra_Software_Devel/4   4/15/10 7:04p vle
 * SW7420-543: fix pixel repetition (2x, 4x) on 480p
 * 
 * Hydra_Software_Devel/3   3/23/10 5:05p vle
 * SW7340-120, SW7342-124: Improve HDMI Tx Eye Opening for 1080p 60Hz (8,
 * 10 & 12bit color depth)
 * 
 * Hydra_Software_Devel/2   3/22/10 4:01p vle
 * SW7420-639: Update 7420 DRV_AMP, DRV_REFCNTL and TERMR settings for
 * specific HDMI ouput pixel clock frequencies.
 * 
 * Hydra_Software_Devel/1   2/23/10 1:17a vle
 * SW7420-579: Refactor HDMI PI code
 * 
 ***************************************************************************/

#include "bhdm.h"
#include "bhdm_priv.h"
#if BCHP_PWR_SUPPORT
#include "bchp_pwr.h"
#endif

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
	{ BHDM_PixelClock_e148_5_DIV_1_001, BAVC_HDMI_BitsPerPixel_e36bit,	11648,	11648,	632812,  632813,   91,	 91,  1,  1 },


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
	{ BHDM_PixelClock_e148_5_DIV_1_001, BAVC_HDMI_BitsPerPixel_e36bit,	17836,	 6272,	703125,  703125,  139,	140, 21, 11 },


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


#if BHDM_CONFIG_BLURAY_PLATFORMS		/* configure PHY for Bluray Platforms */
BERR_Code BHDM_P_ConfigurePhy(BHDM_Handle hHDMI, BHDM_Settings *NewHdmiSettings)
{
	BERR_Code rc = BERR_SUCCESS ;
	uint32_t Register ;

	uint8_t uDrvAmp;
	uint8_t uDrvRefCntl;
	uint8_t uTermR;

	BDBG_ENTER(BHDM_P_ConfigurePhy) ;
	BDBG_ASSERT( hHDMI ) ;


	/* Default */
	uDrvAmp = 3;
	uDrvRefCntl = 0;
	uTermR = 0;

	/*** Additional setup of Tx PHY */
	switch (NewHdmiSettings->eInputVideoFmt)
	{
	case BFMT_VideoFmt_eNTSC :	
	case BFMT_VideoFmt_eNTSC_J :
	case BFMT_VideoFmt_ePAL_B  :
	case BFMT_VideoFmt_ePAL_B1 :
	case BFMT_VideoFmt_ePAL_D  :
	case BFMT_VideoFmt_ePAL_D1 :
	case BFMT_VideoFmt_ePAL_G  :
	case BFMT_VideoFmt_ePAL_H  :
	case BFMT_VideoFmt_ePAL_I  :
	case BFMT_VideoFmt_ePAL_K  :
	case BFMT_VideoFmt_ePAL_M  :
	case BFMT_VideoFmt_ePAL_N  :
	case BFMT_VideoFmt_ePAL_NC :
	case BFMT_VideoFmt_eSECAM :
		if ((NewHdmiSettings->ePixelRepetition == BAVC_HDMI_PixelRepetition_e2x) &&
			(NewHdmiSettings->stColorDepth.eBitsPerPixel == BAVC_HDMI_BitsPerPixel_e36bit))
		{
			uDrvAmp = 1;
			uDrvRefCntl = 1;
			uTermR = 3;
		}
		break;
		
	case BFMT_VideoFmt_e480p   :
	case BFMT_VideoFmt_e576p_50Hz :
		if ((NewHdmiSettings->ePixelRepetition == BAVC_HDMI_PixelRepetition_e4x) &&
			(NewHdmiSettings->stColorDepth.eBitsPerPixel == BAVC_HDMI_BitsPerPixel_e36bit))
		{
			uDrvAmp = 1;
			uDrvRefCntl = 1;
			uTermR = 3;
		}
		break;
		
	case BFMT_VideoFmt_e1080p:
	case BFMT_VideoFmt_e1080p_50Hz:
		switch (NewHdmiSettings->stColorDepth.eBitsPerPixel)
		{
		case BAVC_HDMI_BitsPerPixel_e24bit:
			uDrvAmp = 8;
			uDrvRefCntl = 0;
			uTermR = 0;
			break;
			
		case BAVC_HDMI_BitsPerPixel_e30bit:
		case BAVC_HDMI_BitsPerPixel_e36bit:
			uDrvAmp = 1;
			uDrvRefCntl = 1;
			uTermR = 3;
			break;
			
		default:
			BDBG_ERR(("Unknown color depth")) ;
			rc = BERR_INVALID_PARAMETER ;
			goto done ;
		}
		break;
	default:
		break;
	}

	Register = BREG_Read32( hHDMI->hRegister, BCHP_HDMI_TX_PHY_HDMI_TX_PHY_CTL_0 ) ;
	Register &= ~( BCHP_MASK(HDMI_TX_PHY_HDMI_TX_PHY_CTL_0, DRV_AMP)
				| BCHP_MASK(HDMI_TX_PHY_HDMI_TX_PHY_CTL_0, DRV_REFCNTL)) ;

	Register |= BCHP_FIELD_DATA(HDMI_TX_PHY_HDMI_TX_PHY_CTL_0, DRV_AMP, uDrvAmp)
			| BCHP_FIELD_DATA(HDMI_TX_PHY_HDMI_TX_PHY_CTL_0, DRV_REFCNTL, uDrvRefCntl) ;
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_TX_PHY_HDMI_TX_PHY_CTL_0, Register) ;


	Register = BREG_Read32(hHDMI->hRegister, BCHP_HDMI_TX_PHY_HDMI_TX_PHY_CTL_2) ;
	Register &= ~( BCHP_MASK(HDMI_TX_PHY_HDMI_TX_PHY_CTL_2, TERMR) );
	Register |= BCHP_FIELD_DATA(HDMI_TX_PHY_HDMI_TX_PHY_CTL_2, TERMR, uTermR) ;
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_TX_PHY_HDMI_TX_PHY_CTL_2, Register) ;
 
done:
	
	BDBG_LEAVE(BHDM_P_ConfigurePhy);
	return rc;
}


#else	/* Configure PHY for STB platforms  */
BERR_Code BHDM_P_ConfigurePhy(BHDM_Handle hHDMI, BHDM_Settings *NewHdmiSettings)
{
	BERR_Code rc = BERR_SUCCESS ;
	BHDM_PreEmphasis_Configuration stPreEmphasisConfig;

	BDBG_ENTER(BHDM_P_ConfigurePhy) ;
	BDBG_ASSERT( hHDMI ) ;


	/* Default settings for most formats*/	
	BHDM_CHECK_RC(rc, BHDM_P_GetPreEmphasisConfiguration(hHDMI, &stPreEmphasisConfig));
	stPreEmphasisConfig.uiDriverAmp = 3;
	stPreEmphasisConfig.uiRefCntl = 0;
	stPreEmphasisConfig.uiPreEmphasis_Ch0 = 0;
	stPreEmphasisConfig.uiPreEmphasis_Ch1 = 0;
	stPreEmphasisConfig.uiPreEmphasis_Ch2 = 0;						
	stPreEmphasisConfig.uiPreEmphasis_CK = 0;
	stPreEmphasisConfig.uiTermR = 0;


	/*** Tweak TX PHY settings based on formats */
	switch (NewHdmiSettings->eInputVideoFmt)
	{
	case BFMT_VideoFmt_e480p   :
	case BFMT_VideoFmt_e576p_50Hz :
		if ((NewHdmiSettings->ePixelRepetition == BAVC_HDMI_PixelRepetition_e4x) &&
			(NewHdmiSettings->stColorDepth.eBitsPerPixel == BAVC_HDMI_BitsPerPixel_e36bit))
		{
			stPreEmphasisConfig.uiDriverAmp = 1;
			stPreEmphasisConfig.uiRefCntl = 1;
			stPreEmphasisConfig.uiTermR = 3;
		}
		break;
		
	case BFMT_VideoFmt_e1080p:
	case BFMT_VideoFmt_e1080p_50Hz:
		stPreEmphasisConfig.uiDriverAmp = 1;
		stPreEmphasisConfig.uiRefCntl = 1;
		stPreEmphasisConfig.uiTermR = 3;
		
		switch (NewHdmiSettings->stColorDepth.eBitsPerPixel)
		{
		case BAVC_HDMI_BitsPerPixel_e24bit:				
			break;
			
		case BAVC_HDMI_BitsPerPixel_e30bit:
		case BAVC_HDMI_BitsPerPixel_e36bit:
			stPreEmphasisConfig.uiPreEmphasis_Ch0 = 4;
			stPreEmphasisConfig.uiPreEmphasis_Ch1 = 4;
			stPreEmphasisConfig.uiPreEmphasis_Ch2 = 4;						
			stPreEmphasisConfig.uiPreEmphasis_CK = 4; 			
			break;
		default:
			BDBG_ERR(("Unknown color depth")) ;
			rc = BERR_INVALID_PARAMETER ;
			goto done ;
		}
		break;
		
	default:
		/* default values define above */
		break;
	}

	/* Set configuration */
	BHDM_CHECK_RC(rc, BHDM_P_SetPreEmphasisConfiguration(hHDMI, &stPreEmphasisConfig));
 
done:
	
	BDBG_LEAVE(BHDM_P_ConfigurePhy);
	return rc;
}
#endif


/******************************************************************************
void BHDM_P_EnableTmdsOutput_isr
Summary: Enable (Display) TMDS Output
*******************************************************************************/
void BHDM_P_EnableTmdsOutput_isr(
   BHDM_Handle hHDMI,		/* [in] HDMI handle */
   bool bEnableTmdsOutput	/* [in] boolean to enable/disable */
)
{
	uint32_t Register ;
	uint32_t TmdsOutput ;

	BDBG_ENTER(BHDM_P_EnableTmdsOutput_isr) ;
	BDBG_ASSERT( hHDMI ) ;
	
	if (bEnableTmdsOutput)
		TmdsOutput = 0x0 ; /* TMDS ON */
	else 
		TmdsOutput = 0x1 ; /* TMDS OFF */

#if BHDM_CONFIG_DEBUG_TMDS
	BDBG_WRN(("Turn TMDS Lines %s", bEnableTmdsOutput ? "ON" : "OFF")) ;
#endif

	Register = BREG_Read32(hHDMI->hRegister, BCHP_HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL) ;
	Register &= 
		~( BCHP_MASK(HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL, reserved0) 
		 | BCHP_MASK(HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL, reserved1)) ;

	/* set TMDS lines to power on*/
	Register &= 
		~( BCHP_MASK(HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL, TX_CK_PWRDN) 
		 | BCHP_MASK(HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL, TX_2_PWRDN) 
		 | BCHP_MASK(HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL, TX_1_PWRDN) 
		 | BCHP_MASK(HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL, TX_0_PWRDN))  ;

	/* take TMDS lines out of reset */
	Register &= 
		~( BCHP_MASK(HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL, TX_CK_RESET) 
		 | BCHP_MASK(HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL, TX_2_RESET) 
		 | BCHP_MASK(HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL, TX_1_RESET) 
		 | BCHP_MASK(HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL, TX_0_RESET))  ;

	Register &= 
		~( BCHP_MASK(HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL, D_RESET) 
		 | BCHP_MASK(HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL, A_RESET)) ;

	/* set TMDS lines to requested value on/off */
	Register |= 
		  /* Rx may need CLOCK signal to wake up; always leave clock powered */
		  BCHP_FIELD_DATA(HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL, TX_CK_PWRDN, 0)  
		| BCHP_FIELD_DATA(HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL, TX_2_PWRDN, TmdsOutput) 
		| BCHP_FIELD_DATA(HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL, TX_1_PWRDN, TmdsOutput) 
		| BCHP_FIELD_DATA(HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL, TX_0_PWRDN, TmdsOutput)  ;

	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL, Register) ;


#if BHDM_CONFIG_BLURAY_PLATFORMS
	Register = BREG_Read32(hHDMI->hRegister, BCHP_HDMI_TX_PHY_HDMI_TX_PHY_CTL_1) ;
	Register &= 
		~( BCHP_MASK(HDMI_TX_PHY_HDMI_TX_PHY_CTL_1, ICPX) 
		 | BCHP_MASK(HDMI_TX_PHY_HDMI_TX_PHY_CTL_1, RZ) 
		 | BCHP_MASK(HDMI_TX_PHY_HDMI_TX_PHY_CTL_1, CZ)
		 | BCHP_MASK(HDMI_TX_PHY_HDMI_TX_PHY_CTL_1, CP)
		 | BCHP_MASK(HDMI_TX_PHY_HDMI_TX_PHY_CTL_1, RN_2_1) 
		 | BCHP_MASK(HDMI_TX_PHY_HDMI_TX_PHY_CTL_1, RN_0)
		 | BCHP_MASK(HDMI_TX_PHY_HDMI_TX_PHY_CTL_1, CN) 
		 | BCHP_MASK(HDMI_TX_PHY_HDMI_TX_PHY_CTL_1, LF_ORDER))	;

	Register |= 
		  BCHP_FIELD_DATA(HDMI_TX_PHY_HDMI_TX_PHY_CTL_1, ICPX, 31) 
		| BCHP_FIELD_DATA(HDMI_TX_PHY_HDMI_TX_PHY_CTL_1, RZ, 1) 	
		| BCHP_FIELD_DATA(HDMI_TX_PHY_HDMI_TX_PHY_CTL_1, CZ, 1)
		| BCHP_FIELD_DATA(HDMI_TX_PHY_HDMI_TX_PHY_CTL_1, CP, 1)
		| BCHP_FIELD_DATA(HDMI_TX_PHY_HDMI_TX_PHY_CTL_1, RN_2_1, 0) 
		| BCHP_FIELD_DATA(HDMI_TX_PHY_HDMI_TX_PHY_CTL_1, RN_0, 0) 
		| BCHP_FIELD_DATA(HDMI_TX_PHY_HDMI_TX_PHY_CTL_1, CN, 0) 
		| BCHP_FIELD_DATA(HDMI_TX_PHY_HDMI_TX_PHY_CTL_1, LF_ORDER, 0) ;

	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_TX_PHY_HDMI_TX_PHY_CTL_1, Register) ;
	

	Register = BREG_Read32(hHDMI->hRegister, BCHP_HDMI_TX_PHY_HDMI_TX_PHY_CTL_2) ;
	Register &= 
		~( BCHP_MASK(HDMI_TX_PHY_HDMI_TX_PHY_CTL_2, PTAP_ADJ)
		 | BCHP_MASK(HDMI_TX_PHY_HDMI_TX_PHY_CTL_2, CTAP_ADJ)
		 | BCHP_MASK(HDMI_TX_PHY_HDMI_TX_PHY_CTL_2, BIASIN_EN)
		 | BCHP_MASK(HDMI_TX_PHY_HDMI_TX_PHY_CTL_2, NDIV_MOD)
		 | BCHP_MASK(HDMI_TX_PHY_HDMI_TX_PHY_CTL_2, KVCO_XS))  ;

	Register |= 
		  BCHP_FIELD_DATA(HDMI_TX_PHY_HDMI_TX_PHY_CTL_2, PTAP_ADJ, 1) 
		| BCHP_FIELD_DATA(HDMI_TX_PHY_HDMI_TX_PHY_CTL_2, CTAP_ADJ, 1)
		| BCHP_FIELD_DATA(HDMI_TX_PHY_HDMI_TX_PHY_CTL_2, BIASIN_EN, 1) 
		| BCHP_FIELD_DATA(HDMI_TX_PHY_HDMI_TX_PHY_CTL_2, NDIV_MOD, 3)
		| BCHP_FIELD_DATA(HDMI_TX_PHY_HDMI_TX_PHY_CTL_2, KVCO_XS, 7) ;

	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_TX_PHY_HDMI_TX_PHY_CTL_2, Register) ;

#endif

	BDBG_LEAVE(BHDM_P_EnableTmdsOutput_isr) ;
	return ;
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

		{BFMT_PXL_74_25MHz, 	BAVC_HDMI_BitsPerPixel_e24bit,	BAVC_HDMI_PixelRepetition_eNone,	BHDM_PixelClock_e74_25},		  /* 74.25	  */
		{BFMT_PXL_74_25MHz_DIV_1_001,	BAVC_HDMI_BitsPerPixel_e24bit,	BAVC_HDMI_PixelRepetition_eNone,	BHDM_PixelClock_e74_25_DIV_1_001},	/* 74.17582 */

		{BFMT_PXL_54MHz,	BAVC_HDMI_BitsPerPixel_e24bit,	BAVC_HDMI_PixelRepetition_e4x,	BHDM_PixelClock_e108},			   /* 27x4 (4x pixel repetition)	*/
		{BFMT_PXL_54MHz_MUL_1_001,	BAVC_HDMI_BitsPerPixel_e24bit,	BAVC_HDMI_PixelRepetition_e4x,	BHDM_PixelClock_e108_MUL_1_001},			   /* 27x4 (4x pixel repetition)	*/
		
		{BFMT_PXL_148_5MHz, 	BAVC_HDMI_BitsPerPixel_e24bit,	BAVC_HDMI_PixelRepetition_eNone,	BHDM_PixelClock_e148_5},			 /* 148.5 */
		{BFMT_PXL_148_5MHz_DIV_1_001,	BAVC_HDMI_BitsPerPixel_e24bit,	BAVC_HDMI_PixelRepetition_eNone,	BHDM_PixelClock_e148_5_DIV_1_001},			 /* 148.35165 */


		/* 10bit deep color mode */
		{BFMT_PXL_25_2MHz,	BAVC_HDMI_BitsPerPixel_e30bit,	BAVC_HDMI_PixelRepetition_eNone,	BHDM_PixelClock_e31_5}, 			/* 31.5 	*/
		{BFMT_PXL_25_2MHz_DIV_1_001,	BAVC_HDMI_BitsPerPixel_e30bit,	BAVC_HDMI_PixelRepetition_eNone,	BHDM_PixelClock_e31_5_DIV_1_001},	/* 31.4685 */

		{BFMT_PXL_27MHz,	BAVC_HDMI_BitsPerPixel_e30bit,	BAVC_HDMI_PixelRepetition_eNone,	BHDM_PixelClock_e33_75},			  /* 27 	*/
		{BFMT_PXL_27MHz_MUL_1_001,	BAVC_HDMI_BitsPerPixel_e30bit,	BAVC_HDMI_PixelRepetition_eNone,	BHDM_PixelClock_e33_75_MUL_1_001},	 /* 27.027 */

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
		
		{BFMT_PXL_74_25MHz, BAVC_HDMI_BitsPerPixel_e30bit,	BAVC_HDMI_PixelRepetition_eNone,	BHDM_PixelClock_e92_8125},			/* 92.8125	  */
		{BFMT_PXL_74_25MHz_DIV_1_001,	BAVC_HDMI_BitsPerPixel_e30bit,	BAVC_HDMI_PixelRepetition_eNone,	BHDM_PixelClock_e92_8125_DIV_1_001},  /* 92.7198 */

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
		
		{BFMT_PXL_74_25MHz, BAVC_HDMI_BitsPerPixel_e36bit,	BAVC_HDMI_PixelRepetition_eNone,	BHDM_PixelClock_e111_375},			/* 111.375	  */
		{BFMT_PXL_74_25MHz_DIV_1_001,	BAVC_HDMI_BitsPerPixel_e36bit,	BAVC_HDMI_PixelRepetition_eNone,	BHDM_PixelClock_e111_375_DIV_1_001},  /* 111.264 */

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
	bool masterMode;
		
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

	/* Additional settings for FIFO_CTL register */
	Register = BREG_Read32(hHDMI->hRegister, BCHP_HDMI_FIFO_CTL);
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

	BHDM_GetHdmiDataTransferMode(hHDMI, &masterMode);
	Register |= BCHP_FIELD_DATA(HDMI_FIFO_CTL, USE_FULL, masterMode?0:1)
			| BCHP_FIELD_DATA(HDMI_FIFO_CTL, USE_EMPTY, 1);
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_FIFO_CTL, Register);


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


	/****		Set Channel Map 	*/
	/* set CHANNEL_0_MAP = 0  - reset value */
	/* set CHANNEL_1_MAP = 1  - reset value */
	/* set CHANNEL_2_MAP = 2  - reset value */
	/* set CHANNEL_3_MAP = 3  - reset value */
	/* set CHANNEL_4_MAP = 4  - reset value */
	/* set CHANNEL_5_MAP = 5  - reset value */
	/* set CHANNEL_6_MAP = 6  - reset value */
	/* set CHANNEL_7_MAP = 7  - reset value */
	Register = 
		   BCHP_FIELD_DATA(HDMI_MAI_CHANNEL_MAP, CHANNEL_0_MAP, 0) 
		| BCHP_FIELD_DATA(HDMI_MAI_CHANNEL_MAP, CHANNEL_1_MAP, 1) 
		| BCHP_FIELD_DATA(HDMI_MAI_CHANNEL_MAP, CHANNEL_2_MAP, 4) 
		| BCHP_FIELD_DATA(HDMI_MAI_CHANNEL_MAP, CHANNEL_3_MAP, 5) 
		| BCHP_FIELD_DATA(HDMI_MAI_CHANNEL_MAP, CHANNEL_4_MAP, 3) 
		| BCHP_FIELD_DATA(HDMI_MAI_CHANNEL_MAP, CHANNEL_5_MAP, 2) 
		| BCHP_FIELD_DATA(HDMI_MAI_CHANNEL_MAP, CHANNEL_6_MAP, 6) 
		| BCHP_FIELD_DATA(HDMI_MAI_CHANNEL_MAP, CHANNEL_7_MAP, 7)
		| BCHP_FIELD_DATA(HDMI_MAI_CHANNEL_MAP, reserved0, 0) ;
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_MAI_CHANNEL_MAP, Register) ; 


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
	uint8_t uiColorDepth;
	

	BDBG_ENTER(BHDM_SetColorDepth);

	/* 
	-- if attached sink does not support Deep Color; continue sending only the 
	-- RAM based General Control Packet (GCP) i.e. REGISTER based GCP is not needed 
	*/

	if ((!hHDMI->AttachedEDID.RxVSDB.DeepColor_48bit)
	&& (!hHDMI->AttachedEDID.RxVSDB.DeepColor_36bit)
	&& (!hHDMI->AttachedEDID.RxVSDB.DeepColor_30bit))
	{
#if BHDM_CONFIG_DEBUG_GCP_PACKET_USAGE
		BDBG_WRN(("Attached sink does not support Deep Color")) ;
		BDBG_WRN(("REGISTER based GCP for Deep Color not configured")) ;
#endif		
		goto done ;
	}
	
#if BHDM_CONFIG_DEBUG_GCP_PACKET_USAGE
		BDBG_WRN(("GCP Deep Deep Color Support: GCP_CONFIG Register"));
#endif


	/* Check bits per pixel settings */
	switch (stColorDepthSettings->eBitsPerPixel)
	{
	case BAVC_HDMI_BitsPerPixel_e24bit:

		/* 8 bit normal color mode */ 	
		uiColorDepth = BAVC_HDMI_GCP_ColorDepth_e24bpp;
		break;
	
	case BAVC_HDMI_BitsPerPixel_e30bit:
		if (!hHDMI->AttachedEDID.RxVSDB.DeepColor_30bit)
		{
			BDBG_ERR(("Attached sink does not support 10-bit deep color mode"));
			rc = BERR_NOT_SUPPORTED;
			goto done;
		}

		/* 10 bit deep color mode */ 	
		uiColorDepth = BAVC_HDMI_GCP_ColorDepth_e30bpp;
		break;

		
	case BAVC_HDMI_BitsPerPixel_e36bit:
		if (!hHDMI->AttachedEDID.RxVSDB.DeepColor_36bit)
		{
			BDBG_ERR(("Attached receiver does not support 12-bit deep color"));
			rc = BERR_NOT_SUPPORTED;
			goto done;
		}
	
		/* 12 bit deep color mode */
		uiColorDepth = BAVC_HDMI_GCP_ColorDepth_e36bpp;
		break;

	
	case BAVC_HDMI_BitsPerPixel_e48bit:
		/* 16 bit deep color mode */
		uiColorDepth = BAVC_HDMI_GCP_ColorDepth_e48bpp;
		BDBG_ERR(("16 bit deep color is not supported"));
		rc = BERR_NOT_SUPPORTED;
		goto done;
		break;
		
	default:
		BDBG_ERR(("Invalid color depth specified: %d", 
			stColorDepthSettings->eBitsPerPixel));
		rc = BERR_INVALID_PARAMETER;
		goto done;
		break;
	}


	/* Configure for hardware based GCP */
	Register = BREG_Read32(hHDMI->hRegister, BCHP_HDMI_DEEP_COLOR_CONFIG_1); 
	Register &= ~BCHP_MASK(HDMI_DEEP_COLOR_CONFIG_1, COLOR_DEPTH) ;
	Register |= BCHP_FIELD_DATA(HDMI_DEEP_COLOR_CONFIG_1, COLOR_DEPTH, uiColorDepth) ;
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_DEEP_COLOR_CONFIG_1, Register); 

	Register = BREG_Read32(hHDMI->hRegister, BCHP_HDMI_GCP_WORD_1); 
	Register &= ~BCHP_MASK(HDMI_GCP_WORD_1, GCP_SUBPACKET_BYTE_1) ;
	Register |= BCHP_FIELD_DATA(HDMI_GCP_WORD_1, GCP_SUBPACKET_BYTE_1, uiColorDepth) ;
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_GCP_WORD_1, Register); 

	/* 
	-- Enable transmission of hardware based GCP  
	-- NOTE:  packing phase will be updated automatically 
	*/	
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

	/* Red */
	Register = BREG_Read32(hHDMI->hRegister, BCHP_DVP_HT_HDMI_TX_0_TEST_DATA_GEN_CFG_10) ;
	Register &= 
		~( BCHP_MASK(DVP_HT_HDMI_TX_0_TEST_DATA_GEN_CFG_10, DVO_0_FLAT_FIELD_1_RED) 
		 | BCHP_MASK(DVP_HT_HDMI_TX_0_TEST_DATA_GEN_CFG_10, DVO_0_FLAT_FIELD_2_RED) );
	BREG_Write32(hHDMI->hRegister, BCHP_DVP_HT_HDMI_TX_0_TEST_DATA_GEN_CFG_10, Register);

	Register |= BCHP_FIELD_DATA(DVP_HT_HDMI_TX_0_TEST_DATA_GEN_CFG_10, DVO_0_FLAT_FIELD_1_RED, (uiRed12bits << 4))
			| BCHP_FIELD_DATA(DVP_HT_HDMI_TX_0_TEST_DATA_GEN_CFG_10, DVO_0_FLAT_FIELD_2_RED, (uiRed12bits << 4)) ;
	BREG_Write32(hHDMI->hRegister, BCHP_DVP_HT_HDMI_TX_0_TEST_DATA_GEN_CFG_10, Register);

	/* Green */
	Register = BREG_Read32(hHDMI->hRegister, BCHP_DVP_HT_HDMI_TX_0_TEST_DATA_GEN_CFG_11) ;
	Register &= 
		~( BCHP_MASK(DVP_HT_HDMI_TX_0_TEST_DATA_GEN_CFG_11, DVO_0_FLAT_FIELD_1_GREEN) 
		| BCHP_MASK(DVP_HT_HDMI_TX_0_TEST_DATA_GEN_CFG_11, DVO_0_FLAT_FIELD_2_GREEN) );
	BREG_Write32(hHDMI->hRegister, BCHP_DVP_HT_HDMI_TX_0_TEST_DATA_GEN_CFG_11, Register);

	Register |= BCHP_FIELD_DATA(DVP_HT_HDMI_TX_0_TEST_DATA_GEN_CFG_11, DVO_0_FLAT_FIELD_1_GREEN, (uiGreen12bits << 4))
			| BCHP_FIELD_DATA(DVP_HT_HDMI_TX_0_TEST_DATA_GEN_CFG_11, DVO_0_FLAT_FIELD_2_GREEN, (uiGreen12bits << 4)) ;
	BREG_Write32(hHDMI->hRegister, BCHP_DVP_HT_HDMI_TX_0_TEST_DATA_GEN_CFG_11, Register);

	/* Blue */
	Register = BREG_Read32(hHDMI->hRegister, BCHP_DVP_HT_HDMI_TX_0_TEST_DATA_GEN_CFG_12) ;
	Register &= 
		~( BCHP_MASK(DVP_HT_HDMI_TX_0_TEST_DATA_GEN_CFG_12, DVO_0_FLAT_FIELD_1_BLUE) 
		| BCHP_MASK(DVP_HT_HDMI_TX_0_TEST_DATA_GEN_CFG_12, DVO_0_FLAT_FIELD_2_BLUE) );
	BREG_Write32(hHDMI->hRegister, BCHP_DVP_HT_HDMI_TX_0_TEST_DATA_GEN_CFG_12, Register);

	Register |= BCHP_FIELD_DATA(DVP_HT_HDMI_TX_0_TEST_DATA_GEN_CFG_12, DVO_0_FLAT_FIELD_1_BLUE, (uiBlue12bits << 4))
			| BCHP_FIELD_DATA(DVP_HT_HDMI_TX_0_TEST_DATA_GEN_CFG_12, DVO_0_FLAT_FIELD_2_BLUE, (uiBlue12bits << 4)) ;
	BREG_Write32(hHDMI->hRegister, BCHP_DVP_HT_HDMI_TX_0_TEST_DATA_GEN_CFG_12, Register);


	/* Setup mode */
	Register = BREG_Read32(hHDMI->hRegister, BCHP_DVP_HT_HDMI_TX_0_TEST_DATA_GEN_CFG_13) ;
	Register &= ~BCHP_MASK(DVP_HT_HDMI_TX_0_TEST_DATA_GEN_CFG_13, DVO_0_GEN_TEST_MODE) ;
	BREG_Write32(hHDMI->hRegister, BCHP_DVP_HT_HDMI_TX_0_TEST_DATA_GEN_CFG_13, Register) ;

	Register |= BCHP_FIELD_DATA(DVP_HT_HDMI_TX_0_TEST_DATA_GEN_CFG_13, DVO_0_GEN_TEST_MODE, 4);
	BREG_Write32(hHDMI->hRegister, BCHP_DVP_HT_HDMI_TX_0_TEST_DATA_GEN_CFG_13, Register);


	/* Enable */
	Register = BREG_Read32(hHDMI->hRegister, BCHP_DVP_HT_HDMI_TX_0_TEST_DATA_GEN_CFG_0) ;
	Register &= ~BCHP_MASK(DVP_HT_HDMI_TX_0_TEST_DATA_GEN_CFG_0, DVO_0_TEST_MODE) ;
	BREG_Write32(hHDMI->hRegister, BCHP_DVP_HT_HDMI_TX_0_TEST_DATA_GEN_CFG_0, Register) ;

	Register |= BCHP_FIELD_DATA(DVP_HT_HDMI_TX_0_TEST_DATA_GEN_CFG_0, DVO_0_TEST_MODE, 3);
	BREG_Write32(hHDMI->hRegister, BCHP_DVP_HT_HDMI_TX_0_TEST_DATA_GEN_CFG_0, Register);


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

	/* Disable */
	Register = BREG_Read32(hHDMI->hRegister, BCHP_DVP_HT_HDMI_TX_0_TEST_DATA_GEN_CFG_0) ;
	Register &= ~BCHP_MASK(DVP_HT_HDMI_TX_0_TEST_DATA_GEN_CFG_0, DVO_0_TEST_MODE) ;
	BREG_Write32(hHDMI->hRegister, BCHP_DVP_HT_HDMI_TX_0_TEST_DATA_GEN_CFG_0, Register) ;

	Register |= BCHP_FIELD_DATA(DVP_HT_HDMI_TX_0_TEST_DATA_GEN_CFG_0, DVO_0_TEST_MODE, 0);
	BREG_Write32(hHDMI->hRegister, BCHP_DVP_HT_HDMI_TX_0_TEST_DATA_GEN_CFG_0, Register);


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

	BDBG_ENTER(BHDM_WaitForStableVideo);
	BHDM_ClearHotPlugInterrupt(hHDMI);

	while (waitThusFar < maxWait) 
	{
		uint8_t notStable = false;

		Register = BREG_Read32(hHDMI->hRegister, BCHP_HDMI_FORMAT_DET_UPDATE_STATUS) ;

		if (Register & (BCHP_MASK(HDMI_FORMAT_DET_UPDATE_STATUS, UPDATED_HAP)
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
		else
		{
			Register = BREG_Read32(hHDMI->hRegister, BCHP_HDMI_READ_POINTERS) ;
			if (Register & (BCHP_MASK(HDMI_READ_POINTERS, DRIFT_UNDERFLOW)
						  | BCHP_MASK(HDMI_READ_POINTERS, DRIFT_OVERFLOW)))
			{
				notStable = true;
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


/****************************
**		PRIVATE FUNCTIONS 
*****************************/
void BHDM_P_ResetHdmiCore (BHDM_Handle hHDMI)
{

#if BHDM_CONFIG_RDB_NAME_FIX
	BREG_AtomicUpdate32(hHDMI->hRegister, BCHP_SUN_TOP_CTRL_SW_RESET, 0,
			  BCHP_FIELD_DATA(SUN_TOP_CTRL_SW_RESET, hdmi_core_sw_reset, 1));
	BREG_AtomicUpdate32(hHDMI->hRegister, BCHP_SUN_TOP_CTRL_SW_RESET,
			  BCHP_FIELD_DATA(SUN_TOP_CTRL_SW_RESET, hdmi_core_sw_reset, 1),0);
#else
	BREG_AtomicUpdate32(hHDMI->hRegister, BCHP_SUN_TOP_CTRL_SW_RESET, 0,
			  BCHP_FIELD_DATA(SUN_TOP_CTRL_SW_RESET, hdmi_sw_reset, 1));
	BREG_AtomicUpdate32(hHDMI->hRegister, BCHP_SUN_TOP_CTRL_SW_RESET,
			  BCHP_FIELD_DATA(SUN_TOP_CTRL_SW_RESET, hdmi_sw_reset, 1),0);
#endif

	return;
}

void BHDM_P_PowerOnPhy (BHDM_Handle hHDMI)
{
	uint32_t Register;
	
	/* Assert the fields to prevent DRIFT FIFO UNDERFLOW when trying to 
		authenticate with DVI receivers */
	Register = BREG_Read32(hHDMI->hRegister, BCHP_HDMI_FIFO_CTL);
	Register |= BCHP_FIELD_DATA(HDMI_FIFO_CTL, USE_EMPTY, 1)
				| BCHP_FIELD_DATA(HDMI_FIFO_CTL, USE_FULL, 1);
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_FIFO_CTL, Register) ;
	
	/* PR 28685: Enable Random bit block power pown. */
	Register = BREG_Read32(hHDMI->hRegister, BCHP_HDMI_TX_PHY_HDMI_TX_PHY_CTL_0);
	Register &= ~ BCHP_MASK(HDMI_TX_PHY_HDMI_TX_PHY_CTL_0, RND_PWRDN);
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_TX_PHY_HDMI_TX_PHY_CTL_0, Register) ;
	
	/* power PLL etc. */
	Register = BREG_Read32(hHDMI->hRegister, BCHP_HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL);
	Register &= 
		~( BCHP_MASK(HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL, PLL_PWRDN) 
		| BCHP_MASK(HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL, REF_COMP_PWRDN) 
		| BCHP_MASK(HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL, PWRDN_BG)
		| BCHP_MASK(HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL, PWRDN_REFAMP)) ;
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL, Register) ;

	return;
}


void BHDM_P_PowerOffPhy (BHDM_Handle hHDMI)
{
	uint32_t Register;
	
	Register = BREG_Read32(hHDMI->hRegister, BCHP_HDMI_TX_PHY_HDMI_TX_PHY_CTL_0);
	Register &= ~ BCHP_MASK(HDMI_TX_PHY_HDMI_TX_PHY_CTL_0, RND_PWRDN);
	Register |= BCHP_FIELD_DATA(HDMI_TX_PHY_HDMI_TX_PHY_CTL_0, RND_PWRDN, 1);
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_TX_PHY_HDMI_TX_PHY_CTL_0, Register) ;
	
	/* power PLL etc. */
	Register = BREG_Read32(hHDMI->hRegister, BCHP_HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL);
	Register &= 
		~( BCHP_MASK(HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL, PLL_PWRDN) 
		| BCHP_MASK(HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL, REF_COMP_PWRDN) 
		| BCHP_MASK(HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL, PWRDN_BG)
		| BCHP_MASK(HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL, PWRDN_REFAMP)) ;

	Register |=
		    BCHP_FIELD_DATA(HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL, PLL_PWRDN, 1)
		  | BCHP_FIELD_DATA(HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL, REF_COMP_PWRDN, 1)
		  | BCHP_FIELD_DATA(HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL, PWRDN_BG, 1)
		  | BCHP_FIELD_DATA(HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL, PWRDN_REFAMP, 1);
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL, Register) ;

	return;
}


void BHDM_P_SetPreEmphasisMode (BHDM_Handle hHDMI, uint8_t uValue, uint8_t uDriverAmp)
{
	uint32_t Register;

	Register = BREG_Read32( hHDMI->hRegister, BCHP_HDMI_TX_PHY_HDMI_TX_PHY_CTL_0) ;
	Register &= ~(BCHP_MASK(HDMI_TX_PHY_HDMI_TX_PHY_CTL_0, PREEMP_CK)
			  | BCHP_MASK(HDMI_TX_PHY_HDMI_TX_PHY_CTL_0, PREEMP_2)
			  | BCHP_MASK(HDMI_TX_PHY_HDMI_TX_PHY_CTL_0, PREEMP_1)
			  | BCHP_MASK(HDMI_TX_PHY_HDMI_TX_PHY_CTL_0, PREEMP_0));
	
	Register |= BCHP_FIELD_DATA(HDMI_TX_PHY_HDMI_TX_PHY_CTL_0, PREEMP_CK, uValue)
			| BCHP_FIELD_DATA(HDMI_TX_PHY_HDMI_TX_PHY_CTL_0, PREEMP_2, uValue)
			| BCHP_FIELD_DATA(HDMI_TX_PHY_HDMI_TX_PHY_CTL_0, PREEMP_1, uValue)
			| BCHP_FIELD_DATA(HDMI_TX_PHY_HDMI_TX_PHY_CTL_0, PREEMP_0, uValue) ;
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_TX_PHY_HDMI_TX_PHY_CTL_0, Register) ;

	Register = BREG_Read32( hHDMI->hRegister, BCHP_HDMI_TX_PHY_HDMI_TX_PHY_CTL_0) ;
	Register &= ~ BCHP_MASK(HDMI_TX_PHY_HDMI_TX_PHY_CTL_0, DRV_AMP);
	Register |=  BCHP_FIELD_DATA(HDMI_TX_PHY_HDMI_TX_PHY_CTL_0, DRV_AMP, uDriverAmp);
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_TX_PHY_HDMI_TX_PHY_CTL_0, Register) ;

	return;
}


BERR_Code BHDM_P_GetPreEmphasisConfiguration (
	BHDM_Handle hHDMI,
	BHDM_PreEmphasis_Configuration *stPreEmphasisConfig
)
{
	BERR_Code rc = BERR_SUCCESS;
	uint32_t Register;

	Register = BREG_Read32( hHDMI->hRegister, BCHP_HDMI_TX_PHY_HDMI_TX_PHY_CTL_0) ;
	stPreEmphasisConfig->uiDriverAmp = BCHP_GET_FIELD_DATA(Register,HDMI_TX_PHY_HDMI_TX_PHY_CTL_0,DRV_AMP);
	stPreEmphasisConfig->uiRefCntl = BCHP_GET_FIELD_DATA(Register, HDMI_TX_PHY_HDMI_TX_PHY_CTL_0, DRV_REFCNTL);
	stPreEmphasisConfig->uiPreEmphasis_CK = BCHP_GET_FIELD_DATA(Register,HDMI_TX_PHY_HDMI_TX_PHY_CTL_0,PREEMP_CK);
	stPreEmphasisConfig->uiPreEmphasis_Ch2 = BCHP_GET_FIELD_DATA(Register,HDMI_TX_PHY_HDMI_TX_PHY_CTL_0,PREEMP_2);
	stPreEmphasisConfig->uiPreEmphasis_Ch1 = BCHP_GET_FIELD_DATA(Register,HDMI_TX_PHY_HDMI_TX_PHY_CTL_0,PREEMP_1);
	stPreEmphasisConfig->uiPreEmphasis_Ch0 = BCHP_GET_FIELD_DATA(Register,HDMI_TX_PHY_HDMI_TX_PHY_CTL_0,PREEMP_0);

	Register = BREG_Read32( hHDMI->hRegister, BCHP_HDMI_TX_PHY_HDMI_TX_PHY_CTL_2) ;
	stPreEmphasisConfig->uiTermR = BCHP_GET_FIELD_DATA(Register, HDMI_TX_PHY_HDMI_TX_PHY_CTL_2, TERMR);
	
	return rc;
}


BERR_Code BHDM_P_SetPreEmphasisConfiguration(
	BHDM_Handle hHDMI,
	BHDM_PreEmphasis_Configuration *stPreEmphasisConfig)
{
	BERR_Code rc = BERR_SUCCESS;
	uint32_t Register;
	
	/* Set Preemphasis configurations */
	Register = BREG_Read32( hHDMI->hRegister, BCHP_HDMI_TX_PHY_HDMI_TX_PHY_CTL_0) ;
	Register &= ~(
		  BCHP_MASK(HDMI_TX_PHY_HDMI_TX_PHY_CTL_0, DRV_AMP)
		| BCHP_MASK(HDMI_TX_PHY_HDMI_TX_PHY_CTL_0, DRV_REFCNTL)
		| BCHP_MASK(HDMI_TX_PHY_HDMI_TX_PHY_CTL_0, PREEMP_CK)
		| BCHP_MASK(HDMI_TX_PHY_HDMI_TX_PHY_CTL_0, PREEMP_2)
		| BCHP_MASK(HDMI_TX_PHY_HDMI_TX_PHY_CTL_0, PREEMP_1)
		| BCHP_MASK(HDMI_TX_PHY_HDMI_TX_PHY_CTL_0, PREEMP_0)) ;
		
	Register |=
		  BCHP_FIELD_DATA(HDMI_TX_PHY_HDMI_TX_PHY_CTL_0, DRV_AMP, stPreEmphasisConfig->uiDriverAmp)
		| BCHP_FIELD_DATA(HDMI_TX_PHY_HDMI_TX_PHY_CTL_0, DRV_REFCNTL, stPreEmphasisConfig->uiRefCntl)		  
		| BCHP_FIELD_DATA(HDMI_TX_PHY_HDMI_TX_PHY_CTL_0, PREEMP_CK, stPreEmphasisConfig->uiPreEmphasis_CK)
		| BCHP_FIELD_DATA(HDMI_TX_PHY_HDMI_TX_PHY_CTL_0, PREEMP_2, stPreEmphasisConfig->uiPreEmphasis_Ch2)
		| BCHP_FIELD_DATA(HDMI_TX_PHY_HDMI_TX_PHY_CTL_0, PREEMP_1, stPreEmphasisConfig->uiPreEmphasis_Ch1)
		| BCHP_FIELD_DATA(HDMI_TX_PHY_HDMI_TX_PHY_CTL_0, PREEMP_0, stPreEmphasisConfig->uiPreEmphasis_Ch0) ;
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_TX_PHY_HDMI_TX_PHY_CTL_0, Register) ;

	Register = BREG_Read32( hHDMI->hRegister, BCHP_HDMI_TX_PHY_HDMI_TX_PHY_CTL_2) ;
	Register &= ~ BCHP_MASK(HDMI_TX_PHY_HDMI_TX_PHY_CTL_2, TERMR);
	Register |= BCHP_FIELD_DATA(HDMI_TX_PHY_HDMI_TX_PHY_CTL_2, TERMR, stPreEmphasisConfig->uiTermR);
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_TX_PHY_HDMI_TX_PHY_CTL_2, Register) ;

	return rc;
}


void BHDM_P_ClearHotPlugInterrupt(
   BHDM_Handle hHDMI		/* [in] HDMI handle */
)
{
	uint32_t Register ;

	/* MI must have 1 written; followed by a 0 */
	Register = BREG_Read32(hHDMI->hRegister, BCHP_HDMI_HOTPLUG_INT) ;
	Register &= ~BCHP_MASK(HDMI_HOTPLUG_INT, MI) ;
	Register |= BCHP_FIELD_DATA(HDMI_HOTPLUG_INT, MI, 1) ;
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_HOTPLUG_INT, Register) ;

	Register &= ~BCHP_MASK(HDMI_HOTPLUG_INT, MI) ;
	Register |= BCHP_FIELD_DATA(HDMI_HOTPLUG_INT, MI, 0) ;
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_HOTPLUG_INT, Register) ;

	return;
}


void BHDM_P_CheckHotPlugInterrupt(
	BHDM_Handle hHDMI,		 /* [in] HDMI handle */
	uint8_t *bHotPlugInterrupt	/* [out] Interrupt asserted or not */
)
{
	uint32_t Register ;

	Register = BREG_Read32(hHDMI->hRegister, BCHP_HDMI_HOTPLUG_INT) ;

	if (Register & BCHP_MASK(HDMI_HOTPLUG_INT, MI))
		*bHotPlugInterrupt = true;
	else
		*bHotPlugInterrupt = false;

	return;
}


void BHDM_P_RxDeviceAttached_isr(
	BHDM_Handle hHDMI,		 /* [in] HDMI handle */
	uint8_t *bDeviceAttached	/* [out] Device Attached Status  */
)
{
	uint32_t Register ;

	BDBG_ENTER(BHDM_P_RxDeviceAttached) ;

	Register = BREG_Read32(hHDMI->hRegister, BCHP_HDMI_HOTPLUG) ;
	*bDeviceAttached = 
		BCHP_GET_FIELD_DATA(Register, HDMI_HOTPLUG, HOTPLUG_STATE) ;

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
			  BCHP_FIELD_DATA(HDMI_INTR2_CPU_CLEAR, DRIFT_FIFO_FULL_MINUS_INTR, 1) 
			| BCHP_FIELD_DATA(HDMI_INTR2_CPU_CLEAR, DRIFT_FIFO_ALMOST_FULL_INTR, 1) 
			| BCHP_FIELD_DATA(HDMI_INTR2_CPU_CLEAR, DRIFT_FIFO_EMPTY_MINUS_INTR, 1) 
			| BCHP_FIELD_DATA(HDMI_INTR2_CPU_CLEAR, DRIFT_FIFO_ALMOST_EMPTY_INTR, 1) ;
		BREG_Write32(hHDMI->hRegister, BCHP_HDMI_INTR2_CPU_CLEAR, Register) ;
			

		/* enable the interrupts */ 		  
		BHDM_CHECK_RC( rc, BINT_EnableCallback( hHDMI->hCallback[MAKE_INTR_ENUM(DF_FULL_MINUS)] )) ;
		BHDM_CHECK_RC( rc, BINT_EnableCallback( hHDMI->hCallback[MAKE_INTR_ENUM(DF_ALMOST_FULL)])) ; 
		BHDM_CHECK_RC( rc, BINT_EnableCallback( hHDMI->hCallback[MAKE_INTR_ENUM(DF_EMPTY_MINUS)])) ;  
		BHDM_CHECK_RC( rc, BINT_EnableCallback( hHDMI->hCallback[MAKE_INTR_ENUM(DF_ALMOST_EMPTY)])) ; 
		
		/* disable any interrupt masks; so we can see the interrupts if they occur */
		Register = 
			  BCHP_FIELD_DATA(HDMI_INTR2_CPU_MASK_CLEAR, DRIFT_FIFO_FULL_MINUS_INTR, 1) 
			| BCHP_FIELD_DATA(HDMI_INTR2_CPU_MASK_CLEAR, DRIFT_FIFO_ALMOST_FULL_INTR, 1) 
			| BCHP_FIELD_DATA(HDMI_INTR2_CPU_MASK_CLEAR, DRIFT_FIFO_EMPTY_MINUS_INTR, 1) 
			| BCHP_FIELD_DATA(HDMI_INTR2_CPU_MASK_CLEAR, DRIFT_FIFO_ALMOST_EMPTY_INTR, 1) ;
		BREG_Write32(hHDMI->hRegister, BCHP_HDMI_INTR2_CPU_MASK_CLEAR, Register) ;
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
			  BCHP_FIELD_DATA(HDMI_INTR2_CPU_MASK_SET, DRIFT_FIFO_FULL_MINUS_INTR, 1) 
			| BCHP_FIELD_DATA(HDMI_INTR2_CPU_MASK_SET, DRIFT_FIFO_ALMOST_FULL_INTR, 1) 
			| BCHP_FIELD_DATA(HDMI_INTR2_CPU_MASK_SET, DRIFT_FIFO_EMPTY_MINUS_INTR, 1) 
			| BCHP_FIELD_DATA(HDMI_INTR2_CPU_MASK_SET, DRIFT_FIFO_ALMOST_EMPTY_INTR, 1) ;
		BREG_Write32(hHDMI->hRegister, BCHP_HDMI_INTR2_CPU_MASK_SET, Register) ;
	} 

done:
	return rc ;
}
#endif



