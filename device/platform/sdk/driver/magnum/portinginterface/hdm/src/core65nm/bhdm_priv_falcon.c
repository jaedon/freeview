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
 * $brcm_Workfile: bhdm_priv_falcon.c $
 * $brcm_Revision: Hydra_Software_Devel/19 $
 * $brcm_Date: 9/10/12 1:42p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/hdm/src/core65nm/bhdm_priv_falcon.c $
 * 
 * Hydra_Software_Devel/19   9/10/12 1:42p rgreen
 * SW7420-2349: Update BHDM_P_RxDeviceAttaced to isr function.  Update
 * calls to add Enter/Leave Critical Section
 * 
 * Hydra_Software_Devel/18   8/24/12 5:13p vle
 * SW7425-3803:Add Phy PowerDown for standby/close
 * 
 * Hydra_Software_Devel/16   6/15/12 12:39p vle
 * SW7425-3206, SW7425-3241:  Enable/Disable RSEN interrupts to match
 * state of the TMDS lines
 * 
 * Hydra_Software_Devel/15   5/23/12 9:46a rgreen
 * SW7335-1383: Merge changes to main branch
 * 
 * Hydra_Software_Devel/SW7335-1383/3   5/22/12 8:01p rgreen
 * SW7335-1383,SW7425-2075: Move MAI Channel Ordering to isr time when
 * notified of audio changes
 * 
 * Hydra_Software_Devel/14   5/16/12 3:24p rgreen
 * SW7335-1383:  Merge changes
 * 
 * Hydra_Software_Devel/SW7335-1383/2   5/15/12 6:58p rgreen
 * SW7335-1383: Always leave TMDS CLK enabled for detection of RSEN
 * changes.  Replace BHDM_P_GetReceiverSense with
 * BHDM_P_GetReceiverSense_isr
 * 
 * Hydra_Software_Devel/SW7335-1383/1   3/23/12 2:08p rgreen
 * SW7335-1383: Merge in changes from main line
 * 
 * Hydra_Software_Devel/13   3/14/12 6:59p vle
 * SW7425-2515: Remove unused CEC code which is now located in CEC pi
 * 
 * Hydra_Software_Devel/12   2/17/12 10:09a rgreen
 * SW7125-1146, SW7231-345,SW7346-234: 65/130nm support; remove enabling
 * of RSEN interrupt in isr.  Already done in EnableDisplay
 * 
 * Hydra_Software_Devel/11   2/14/12 4:44p rgreen
 * SW7125-1146, SW7231-345,SW7346-234: Add private
 * BHDM_P_EnableTmdsOutput_isr for 65/130nm platforms
 * 
 * Hydra_Software_Devel/10   2/9/12 10:12a erickson
 * SW7125-1146: don't use BINT_Callback that hasn't been created
 * 
 * Hydra_Software_Devel/9   1/16/12 6:58p vle
 * SW7401-4424: Power down TMDS lines after checking receiver sense if
 * TMDS lines were previously disabled.
 * 
 * Hydra_Software_Devel/8   11/14/11 4:23p vle
 * SW7425-1710: Add private function for BHDM_RxDeviceAttached
 * 
 * Hydra_Software_Devel/7   6/28/11 6:20p vle
 * SW7346-234: Disable RSEN interrupt until the PHY is powered on.
 * 
 * Hydra_Software_Devel/5   9/29/10 4:14p vle
 * SW7422-23: Fix build issues without CEC enable
 * 
 * Hydra_Software_Devel/4   9/28/10 7:19p vle
 * SW7422-23: Refactor HDMI code to isolate platform dependent code
 * furthermore. Add support for 7422 and other 40nm platforms.
 * 
 * Hydra_Software_Devel/3   5/26/10 2:48p vle
 * SW7405-4333: Add support for 40, 65, and 65/1.001 Mhz pixel clock rate
 * 
 * Hydra_Software_Devel/2   4/16/10 6:48p vle
 * SW7420-543: fix pixel repetition  on 480p
 * 
 * Hydra_Software_Devel/1   2/23/10 12:42a vle
 * SW7420-579: Refactor HDMI PI code.
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
	uint32_t NValue ;
	uint32_t CTS ;
} BHDM_P_AUDIO_CLK_VALUES ;


/******************************************************************************
Summary:
HDMI Audio Clock Capture and Regeneration Values for 32kHz
NOTE: These entries match the number of entries in the BHDM_RM_VideoRates Table
*******************************************************************************/
static const BHDM_P_AUDIO_CLK_VALUES  BHDM_32KHz_AudioClkValues[] =
{
									  /*		NUM 	CTS  */
	/* BHDM_PixelClock_e25_2		   */	{  4096,  25200 },
	/* BHDM_PixelClock_e25_2_DIV_1_001 */	{  4576,  28125 },

	/* BHDM_PixelClock_e27			   */	{  4096,  27000 },	
	/* BHDM_PixelClock_e27_MUL_1_001   */	{  4096,  27027 },

	/* BHDM_PixelClock_e54		 */   	{  4096,	54000 },  
	/* BHDM_PixelClock_e54_MUL_1_001		 */   {  4096,	54054 },
									
	/* BHDM_PixelClock_e74_25		   */	{  4096,  74250 }, 
	/* BHDM_PixelClock_e74_25_DIV_1_001*/	{ 11648, 210937 },

	/* BHDM_PixelClock_e40	*/	{	4096,	40000	},
	/* BHDM_PixelClock_e65	*/	{	4096,	65000	},
	/* BHDM_PixelClock_e65_DIV_1_001	*/	{	4096,	64935	},
} ;


/******************************************************************************
Summary:
HDMI Audio Clock Capture and Regeneration Values for 44.1kHz
NOTE: These entries match the number of entries in the BHDM_RM_VideoRates Table
*******************************************************************************/
static const BHDM_P_AUDIO_CLK_VALUES BHDM_44_1KHz_AudioClkValues[] =
{
									  /*		NUM 	CTS  */
	/* BHDM_PixelClock_e25_2		   */	{  6272,  28000 },
	/* BHDM_PixelClock_e25_2_DIV_1_001 */	{  7007,  31250 },

	/* BHDM_PixelClock_e27			   */	{  6272,  30000 },
	/* BHDM_PixelClock_e27_MUL_1_001   */	{  6272,  30030 },

	/* BHDM_PixelClock_e54	   */				{  6272,  60000 },	
	/* BHDM_PixelClock_e54_MUL_1_001		   */	{  6272,  60060 },
									
	/* BHDM_PixelClock_e74_25		   */	{  6272,  82500 },
	/* BHDM_PixelClock_e74_25_DIV_1_001*/	{ 17836, 234375 },
	
	/* BHDM_PixelClock_e40  */  {   6272,   44444   },
	/* BHDM_PixelClock_e65  */  {   6272,   72222   },
	/* BHDM_PixelClock_e65_DIV_1_001	  */  {   6272,   72150   },
} ;


/******************************************************************************
Summary:
HDMI Audio Clock Capture and Regeneration Values for 48kHz
NOTE: These entries match the number of entries in the BHDM_RM_VideoRates Table
*******************************************************************************/
static const BHDM_P_AUDIO_CLK_VALUES BHDM_48KHz_AudioClkValues[] =
{
									  /*		NUM 	CTS  */
	/* BHDM_PixelClock_e25_2		   */	{  6144,  25200 },
	/* BHDM_PixelClock_e25_2_DIV_1_001 */	{  6864,  28125 },

	/* BHDM_PixelClock_e27			   */	{  6144,  27000 },
	/* BHDM_PixelClock_e27_MUL_1_001   */	{  6144,  27027 },
									
	/* BHDM_PixelClock_e54	   */			{  6144,  54000 },	
	/* BHDM_PixelClock_e54_MUL_1_001		   */	{  6144,  54054 },
									  
	/* BHDM_PixelClock_e74_25		   */	{  6144,  74250 },
	/* BHDM_PixelClock_e74_25_DIV_1_001*/	{ 11648, 140625 },

	/* BHDM_PixelClock_e40  */  {   6144,   39960   },
	/* BHDM_PixelClock_e65  */  {   6144,   65000   },
	/* BHDM_PixelClock_e65_DIV_1_001	  */  {   6144,   64935   },
} ;


#if BHDM_CONFIG_96KHZ_AUDIO_SUPPORT
/******************************************************************************
Summary:
HDMI Audio Clock Capture and Regeneration Values for 96kHz
NOTE: These entries match the number of entries in the BHDM_RM_VideoRates Table
*******************************************************************************/
static const BHDM_P_AUDIO_CLK_VALUES BHDM_96KHz_AudioClkValues[] =
{
										   /*		N	   CTS	*/
	/* BHDM_PixelClock_e25_2		   */	   {  12288,  25200 },
	/* BHDM_PixelClock_e25_2_DIV_1_001 */	   {  13728,  28125 },

	/* BHDM_PixelClock_e27			   */	   { 12288,  27000 },
	/* BHDM_PixelClock_e27_MUL_1_001   */	   { 12288,  27027 },

	/* BHDM_PixelClock_e54		  */   {  12288,  54000 },	
	/* BHDM_PixelClock_e54_MUL_1_001    */  {  12288,  54054 },
										   
	/* BHDM_PixelClock_e74_25		   */	   { 12288,  74250 },
	/* BHDM_PixelClock_e74_25_DIV_1_001*/	   { 23296, 140625 },

	/* BHDM_PixelClock_e40  */  {   12288,   40000   },
	/* BHDM_PixelClock_e65  */  {   12288,   65000   },
	/* BHDM_PixelClock_e65_DIV_1_001    */  {   12288,   64935   },	
} ;
#endif


#if BHDM_CONFIG_176_4KHZ_AUDIO_SUPPORT
/******************************************************************************
Summary:
HDMI Audio Clock Capture and Regeneration Values for 176.4kHz
NOTE: These entries match the number of entries in the BHDM_RM_VideoRates Table
*******************************************************************************/
static const BHDM_P_AUDIO_CLK_VALUES BHDM_176_4KHz_AudioClkValues[] =
{
											 /* 	  N 	 CTS  */
	/* BHDM_PixelClock_e25_2		   */		{  25088,  28000 },
	/* BHDM_PixelClock_e25_2_DIV_1_001 */		{  28028,  31250 },

	/* BHDM_PixelClock_e27			   */		{ 25088,  30000 },
	/* BHDM_PixelClock_e27_MUL_1_001   */		{ 25088,  30030 },

	/* BHDM_PixelClock_e54 	*/			 {	25088,	60000 },  
	/* BHDM_PixelClock_e54_MUL_1_001		*/	 {	25088,	60060 },
									
	/* BHDM_PixelClock_e74_25		   */		{ 25088,  82500 },
	/* BHDM_PixelClock_e74_25_DIV_1_001*/		{ 71344, 234375 },

	/* BHDM_PixelClock_e40  */  {	 25088,	 44444	 },
	/* BHDM_PixelClock_e65  */  {	 25088,	 72222	 },
	/* BHDM_PixelClock_e65_DIV_1_001	 */  {	 25088,	 72150	 },	
} ;
#endif


#if BHDM_CONFIG_192KHZ_AUDIO_SUPPORT
/******************************************************************************
Summary:
HDMI Audio Clock Capture and Regeneration Values for 192kHz
NOTE: These entries match the number of entries in the BHDM_RM_VideoRates Table
*******************************************************************************/
static const BHDM_P_AUDIO_CLK_VALUES BHDM_192KHz_AudioClkValues[] =
{
											 /* 	  N 	 CTS  */
	/* BHDM_PixelClock_e25_2		   */		{  27456,  28125 },
	/* BHDM_PixelClock_e25_2_DIV_1_001 */		{  24576,  25200 },

	/* BHDM_PixelClock_e27			   */		{ 24576,  27000 },
	/* BHDM_PixelClock_e27_MUL_1_001   */		{ 24576,  27027 },
									
	/* BHDM_PixelClock_e54 	*/			 {	24576,	54000 },  
	/* BHDM_PixelClock_e54_MUL_1_001		*/	 {	24576,	54054 },

	/* BHDM_PixelClock_e74_25		   */		{ 24576,  74250 },
	/* BHDM_PixelClock_e74_25_DIV_1_001*/		{ 46592, 140625 },

	/* BHDM_PixelClock_e40  */  {	 24576,	 40000	 },
	/* BHDM_PixelClock_e65  */  {	 24576,	 65000	 },
	/* BHDM_PixelClock_e65_DIV_1_001	 */  {	 24576,	 64935	 },
} ;
#endif


/******************************************************************************
BERR_Code BHDM_P_EnableTmdsOutput_isr
Summary: Enable/drive (Display) TMDS Output lines (Clock, Tx0, Tx1, Tx2)
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

#if BHDM_CONFIG_DVO_SUPPORT
	/* TMDS is always off when DVO is enabled */
	bEnableTmdsOutput = false ;
#endif
	
	if (bEnableTmdsOutput)
		TmdsOutput = 0x0 ; /* TMDS ON */
	else 
		TmdsOutput = 0x1 ; /* TMDS OFF */

#if BHDM_CONFIG_DEBUG_TMDS
	BDBG_WRN(("Confgure TMDS %s", TmdsOutput ? "OFF" : "ON"));
#endif

#if BHDM_CONFIG_65NM_SUPPORT

	Register = BREG_Read32(hHDMI->hRegister, BCHP_HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL) ;
	Register &= 
		~( BCHP_MASK(HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL, reserved0) 
		 | BCHP_MASK(HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL, reserved1)) ;

	/* set TMDS lines to power on*/
	Register &= 
		~( BCHP_MASK(HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL, PLL_PWRDN)	
		 | BCHP_MASK(HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL, TX_CK_PWRDN) 
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
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL, Register) ;

	/* set TMDS lines to requested value on/off */
	Register |= 
		  BCHP_FIELD_DATA(HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL, PLL_PWRDN, TmdsOutput) 
		| BCHP_FIELD_DATA(HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL, TX_CK_PWRDN, 0) 
		| BCHP_FIELD_DATA(HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL, TX_2_PWRDN, TmdsOutput) 
		| BCHP_FIELD_DATA(HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL, TX_1_PWRDN, TmdsOutput) 
		| BCHP_FIELD_DATA(HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL, TX_0_PWRDN, TmdsOutput)  ;
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL, Register) ;


	Register = BREG_Read32(hHDMI->hRegister, BCHP_HDMI_TX_PHY_HDMI_TX_PHY_CTL_1) ;
	Register &= 
		~( BCHP_MASK(HDMI_TX_PHY_HDMI_TX_PHY_CTL_1, ICPX) 
		 | BCHP_MASK(HDMI_TX_PHY_HDMI_TX_PHY_CTL_1, RZ) 
		 | BCHP_MASK(HDMI_TX_PHY_HDMI_TX_PHY_CTL_1, RN) 
		 | BCHP_MASK(HDMI_TX_PHY_HDMI_TX_PHY_CTL_1, CN) 
		 | BCHP_MASK(HDMI_TX_PHY_HDMI_TX_PHY_CTL_1, LF_ORDER) 
		 | BCHP_MASK(HDMI_TX_PHY_HDMI_TX_PHY_CTL_1, CZ))  ;

	Register |= 
		  BCHP_FIELD_DATA(HDMI_TX_PHY_HDMI_TX_PHY_CTL_1, ICPX, 0x3) 
		| BCHP_FIELD_DATA(HDMI_TX_PHY_HDMI_TX_PHY_CTL_1, RZ, 1) 
		| BCHP_FIELD_DATA(HDMI_TX_PHY_HDMI_TX_PHY_CTL_1, RN, 0) 
		| BCHP_FIELD_DATA(HDMI_TX_PHY_HDMI_TX_PHY_CTL_1, CN, 1) 
		| BCHP_FIELD_DATA(HDMI_TX_PHY_HDMI_TX_PHY_CTL_1, LF_ORDER, 1) 
		| BCHP_FIELD_DATA(HDMI_TX_PHY_HDMI_TX_PHY_CTL_1, CZ, 1);
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_TX_PHY_HDMI_TX_PHY_CTL_1, Register) ;
	

	Register = BREG_Read32(hHDMI->hRegister, BCHP_HDMI_TX_PHY_HDMI_TX_PHY_CTL_2) ;
	Register &= 
		~( BCHP_MASK(HDMI_TX_PHY_HDMI_TX_PHY_CTL_2, PTAP_ADJ)
		 | BCHP_MASK(HDMI_TX_PHY_HDMI_TX_PHY_CTL_2, CTAP_ADJ)
		 | BCHP_MASK(HDMI_TX_PHY_HDMI_TX_PHY_CTL_2, LOWCUR_EN)
		 | BCHP_MASK(HDMI_TX_PHY_HDMI_TX_PHY_CTL_2, BIASIN_EN)
		 | BCHP_MASK(HDMI_TX_PHY_HDMI_TX_PHY_CTL_2, NDIV_MOD))	;

	Register |= 
		  BCHP_FIELD_DATA(HDMI_TX_PHY_HDMI_TX_PHY_CTL_2, PTAP_ADJ, 1) 
		| BCHP_FIELD_DATA(HDMI_TX_PHY_HDMI_TX_PHY_CTL_2, CTAP_ADJ, 1)
		| BCHP_FIELD_DATA(HDMI_TX_PHY_HDMI_TX_PHY_CTL_2, LOWCUR_EN, 1)
		| BCHP_FIELD_DATA(HDMI_TX_PHY_HDMI_TX_PHY_CTL_2, BIASIN_EN, 1) 
		| BCHP_FIELD_DATA(HDMI_TX_PHY_HDMI_TX_PHY_CTL_2, NDIV_MOD, 4) ;

	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_TX_PHY_HDMI_TX_PHY_CTL_2, Register) ;
	
#else

	Register = BREG_Read32(hHDMI->hRegister, BCHP_HDMI_ANALOG_CTL) ;
	Register &= 
		~( BCHP_MASK(HDMI_ANALOG_CTL, TXCK_PWRDN) 
		 | BCHP_MASK(HDMI_ANALOG_CTL, TX2_PWRDN) 
		 | BCHP_MASK(HDMI_ANALOG_CTL, TX1_PWRDN) 
		 | BCHP_MASK(HDMI_ANALOG_CTL, TX0_PWRDN))  ;

	Register |= 
		  BCHP_FIELD_DATA(HDMI_ANALOG_CTL, TXCK_PWRDN, TmdsOutput) 
		| BCHP_FIELD_DATA(HDMI_ANALOG_CTL, TX2_PWRDN, TmdsOutput) 
		| BCHP_FIELD_DATA(HDMI_ANALOG_CTL, TX1_PWRDN, TmdsOutput) 
		| BCHP_FIELD_DATA(HDMI_ANALOG_CTL, TX0_PWRDN, TmdsOutput)  ;

	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_ANALOG_CTL, Register) ;

#endif

	BDBG_LEAVE(BHDM_P_EnableTmdsOutput_isr) ;
	return ;
}


/***************************************************************************
BHDM_AudioVideoRateChangeCB_isr
Summary: Configure the Rate Manager to match the Rate Manager for the 
Video Display System.
****************************************************************************/
#define CTS_REPEAT 10

void BHDM_AudioVideoRateChangeCB_isr(
	BHDM_Handle hHDMI,
	int   CallbackType,
	void *pvAudioOrVideoData)
{

#if BHDM_CONFIG_TX
#if BDBG_DEBUG_BUILD
static char *AudioSampleRateText[]	= { "32kHz", "44.1kHz", "48kHz", "96kHz" ,"16kHz", "22_05kHz", 
	"24kHz", "64kHz", "88_2kHz", "128kHz", "176_4kHz", "192kHz", "8kHz", "12kHz", "11_025kHz"} ;
#endif

	uint32_t	Register ;
#endif
	
	typedef struct BHDM_BAVC_Clock
	{
		uint32_t ulPixelClkRate ;
		BAVC_HDMI_PixelRepetition ePixelRepetition; 	
		BHDM_InputPixelClock eInputPixelClock ;
	} BHDM_BAVC_Clock ;
	
	static const  BHDM_BAVC_Clock BHDM_SupportedClocks[BHDM_PixelClock_eCount] =
	{
		{BFMT_PXL_25_2MHz,	BAVC_HDMI_PixelRepetition_eNone,	BHDM_PixelClock_e25_2}, 		  /* 25.2	  */
		{BFMT_PXL_25_2MHz_DIV_1_001,	BAVC_HDMI_PixelRepetition_eNone,	BHDM_PixelClock_e25_2_DIV_1_001}, /* 25.1748  */

		{BFMT_PXL_27MHz,	BAVC_HDMI_PixelRepetition_eNone,	BHDM_PixelClock_e27},			  /* 27 	*/
		{BFMT_PXL_27MHz_MUL_1_001,	BAVC_HDMI_PixelRepetition_eNone,	BHDM_PixelClock_e27_MUL_1_001},   /* 27.027 */

		{BFMT_PXL_54MHz,	BAVC_HDMI_PixelRepetition_e2x,	BHDM_PixelClock_e54},			  /* 27x2 (2x pixel repetition)    */
		{BFMT_PXL_54MHz_MUL_1_001,	BAVC_HDMI_PixelRepetition_e2x,	BHDM_PixelClock_e54_MUL_1_001},   /* 27.027x2 (2x pixel repetition) */

		{BFMT_PXL_74_25MHz, 	BAVC_HDMI_PixelRepetition_eNone,	BHDM_PixelClock_e74_25},		  /* 74.25	  */
		{BFMT_PXL_74_25MHz_DIV_1_001,	BAVC_HDMI_PixelRepetition_eNone,	BHDM_PixelClock_e74_25_DIV_1_001},	/* 74.17582 */

		/* DVI/PC/custom pixel rate */
		{BFMT_PXL_40MHz,	BAVC_HDMI_PixelRepetition_eNone,	BHDM_PixelClock_e40},			  /* 40 MHz */
		{BFMT_PXL_65MHz,	BAVC_HDMI_PixelRepetition_eNone,	BHDM_PixelClock_e65},			  /* 65 MHz */
		{BFMT_PXL_65MHz_DIV_1_001,	BAVC_HDMI_PixelRepetition_eNone,	BHDM_PixelClock_e65_DIV_1_001},			  /* 64.935 MHz */

		{BFMT_PXL_60_375MHz,	BAVC_HDMI_PixelRepetition_eNone,	BHDM_PixelClock_e60_375},		  /* 60.375 */
		{BFMT_PXL_74_375MHz,	BAVC_HDMI_PixelRepetition_eNone,	BHDM_PixelClock_e74_375},		  /* 74.375 */
		{BFMT_PXL_64MHz,	BAVC_HDMI_PixelRepetition_eNone,	BHDM_PixelClock_e64},			  /* 64.000 */

		{BFMT_PXL_56_304MHz,	BAVC_HDMI_PixelRepetition_eNone,	BHDM_PixelClock_eCUSTOM_1366x768p_50},	 /* 1366x768 @ 50	 */
		{BFMT_PXL_67_565MHz_DIV_1_001,	BAVC_HDMI_PixelRepetition_eNone,	BHDM_PixelClock_eCUSTOM_1366x768p_5994}, /* 1366x768 @ 59.94 */
		{BFMT_PXL_67_565MHz,	BAVC_HDMI_PixelRepetition_eNone,	BHDM_PixelClock_eCUSTOM_1366x768p_60},	 /* 1366x768 @ 60	 */
	} ;
	
	BHDM_InputPixelClock eInputPixelClock ;   
	uint8_t i ;
	
	BAVC_VdcDisplay_Info *pVdcRateInfo ; /* VIDEO callback */
	uint32_t	ulPixelClkRate; 		 /* see defines in bfmt.h (Mhz) */
	uint32_t	ulVertRefreshRate;		 /* see defines in bfmt.h (Mhz) */
	BAVC_HDMI_PixelRepetition ePixelRepetition;
	
										/* OR */
						
	BAVC_Audio_Info *pAudioData ;		/* AUDIO callback */
	BAVC_AudioSamplingRate eInputAudioSamplingRate ;
#if BHDM_CONFIG_TX
	BHDM_P_AUDIO_CLK_VALUES *pAudioParameters ;
	uint32_t uiCTS_1 ;
	uint8_t CalculateCts = 0 ;
#endif	

		
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
		ePixelRepetition = hHDMI->DeviceSettings.ePixelRepetition;
	
#if 0
		BDBG_MSG(("Refresh Rate:	 0x%x PixelClock: %d PixelRepetition: %d", 
			ulVertRefreshRate, ulPixelClkRate, ePixelRepetition)) ;
#endif	
		BDBG_MSG(("\nVideo Rate Change Callback")) ;
		
		eInputPixelClock = BHDM_PixelClock_eUnused ;
		for (i = 0 ; i < sizeof(BHDM_SupportedClocks) / sizeof(BHDM_BAVC_Clock) ; i++) 
			if ((ulPixelClkRate == BHDM_SupportedClocks[i].ulPixelClkRate) &&
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

#if BHDM_CONFIG_TX
		eInputAudioSamplingRate = hHDMI->DeviceSettings.eAudioSamplingRate ;
#endif
		
		BDBG_MSG(("\tVideo Rate Manager Updated")) ;
	}	
	else  /* Audio Callback Only */
	{
#if BDBG_DEBUG_BUILD
		if	(CallbackType == BHDM_Callback_Type_eManualAudioChange)
			BDBG_MSG(("\nManual Sample Rate Change Callback")) ;
		else
			BDBG_MSG(("\nAudio Sample Rate Change Callback")) ;
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

 #if BHDM_CONFIG_TX

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

#if 0
	if ((pAudioParameters[eInputPixelClock].NValue == 1)
	||	(pAudioParameters[eInputPixelClock].CTS == 1))
	{
		/* HDMI_TODO *CP* */		
		BDBG_ERR(("N and CTS values not properly specified for PixelClock enumeration %d", 
			eInputPixelClock)) ;
		rc = BERR_INVALID_PARAMETER ;
		goto done ;
	}
#endif

	/* clear  CRP_DISABLE	   -  reset value */
	/* clear  EXTERNAL_CTS_EN  -  reset value */
	/* set	  N_VALUE = 20'dN  -  N is selected based on tables 7-1, 7-2, 
								  and 7-3 in the HDMI spec */
								  
	if (hHDMI->DeviceSettings.CalculateCts)
		CalculateCts = 1 ;
		
	Register =
		  BCHP_FIELD_DATA(HDMI_CRP_CFG, reserved1, 0) 
		| BCHP_FIELD_DATA(HDMI_CRP_CFG, 
			USE_MAI_BUS_SYNC_FOR_CTS_GENERATION, 0) 
		| BCHP_FIELD_DATA(HDMI_CRP_CFG, CRP_DISABLE, 0) 
		| BCHP_FIELD_DATA(HDMI_CRP_CFG, EXTERNAL_CTS_EN, CalculateCts) 
		| BCHP_FIELD_DATA(HDMI_CRP_CFG, reserved0, 0) 
		| BCHP_FIELD_DATA(HDMI_CRP_CFG, N_VALUE,
			pAudioParameters[eInputPixelClock].NValue) ;
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_CRP_CFG, Register) ; 
	
	/*set	CTS_0 = 20'dCTS_0  -  CTS_0 is selected based on tables 7-1, 7-2, 
								  and 7-3 in the HDMI spec.  If 2 values are 
								  listed in the table, pick the first. */
	Register = 
		  BCHP_FIELD_DATA(HDMI_CTS_0, reserved0, 0)
		| BCHP_FIELD_DATA(HDMI_CTS_0, CTS_0, pAudioParameters[eInputPixelClock].CTS) ;
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_CTS_0, Register) ; 
	
	/*set	CTS_1 = 20'dCTS_1  -  CTS_1 is selected based on tables 7-1, 7-2, 
								  and 7-3 in the HDMI spec.  If 2 values are 
								  listed in the table, use the second for 
								  alternating CTS values. */
	if ((eInputPixelClock == BHDM_PixelClock_e74_25_DIV_1_001)
	&&	(eInputAudioSamplingRate == BAVC_AudioSamplingRate_e32k))
		/* Table 7-1 */
		uiCTS_1 = 210938 ;
	else
		uiCTS_1 = pAudioParameters[eInputPixelClock].CTS ;
		
	Register = 
		  BCHP_FIELD_DATA(HDMI_CTS_1, reserved0, 0) 
		| BCHP_FIELD_DATA(HDMI_CTS_1, CTS_1, uiCTS_1) ;
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_CTS_1, Register) ; 

	/* set	 CTS_0_REPEAT = 8'd1  */
	/* set	 CTS_0_PERIOD = 20'dK */
	Register =
		  BCHP_FIELD_DATA(HDMI_CTS_PERIOD_0, CTS_0_REPEAT, CTS_REPEAT) 
		| BCHP_FIELD_DATA(HDMI_CTS_PERIOD_0, reserved0, 0) 
		| BCHP_FIELD_DATA(HDMI_CTS_PERIOD_0, CTS_PERIOD_0, pAudioParameters[eInputPixelClock].CTS) ;
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_CTS_PERIOD_0, Register) ; 
	
	/* set	 CTS_1_REPEAT = 8'd1  */
	/* set	 CTS_1_PERIOD = 20'dK */
	Register =
		  BCHP_FIELD_DATA(HDMI_CTS_PERIOD_1, CTS_1_REPEAT, CTS_REPEAT) 
		| BCHP_FIELD_DATA(HDMI_CTS_PERIOD_1, reserved0, 0) 
		| BCHP_FIELD_DATA(HDMI_CTS_PERIOD_1, CTS_PERIOD_1, uiCTS_1) ;
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_CTS_PERIOD_1, Register) ; 

#if BDBG_DEBUG_BUILD
	BDBG_MSG(("   N   CTS 0 - CTS 1")) ;
	if (hHDMI->DeviceSettings.CalculateCts)
	{
		BDBG_MSG(("%4d		*	   *",
			pAudioParameters[eInputPixelClock].NValue)) ;
	}
	else
	{
		BDBG_MSG(("%4d	%6d-%6d",
			pAudioParameters[eInputPixelClock].NValue, 
			pAudioParameters[eInputPixelClock].CTS, uiCTS_1)) ;
		BDBG_MSG(("CTS 0 Repeat Value: %d Period: %d", 
			CTS_REPEAT, pAudioParameters[eInputPixelClock].CTS)) ;
		BDBG_MSG(("CTS 1 Repeat Value: %d Period: %d", CTS_REPEAT, uiCTS_1)) ;
	}
#endif	
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
		ChannelMask = 0x73 ;		break ;
		
	 case BHDM_ChannelAllocation_e_xx__RC__RR__RL__FC___xx_FR__FL :
		ChannelMask = 0x7B ;		break ;
		
	 case BHDM_ChannelAllocation_e_xx__RC__RR__RL__FC__LFE_FR__FL :
		ChannelMask = 0x7F ;		break ;

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

	Register = 
		  BCHP_FIELD_DATA(HDMI_MAI_CONFIG, MAI_BIT_REVERSE, 0) 
		| BCHP_FIELD_DATA(HDMI_MAI_CONFIG, MAI_CHANNEL_MASK, ChannelMask) ;
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
	BDBG_WRN(("Channel Mask: %#x", ChannelMask)) ;
#endif
}


#if BHDM_CONFIG_PLL_KICKSTART_WORKAROUND
BERR_Code BHDM_Debug_GetPllLockStatus(
	BHDM_Handle hHDMI, BHDM_DEBUG_PLL_LOCK_STATUS *stPllStatus)
{
	uint32_t Register ;

	BDBG_ASSERT(hHDMI) ;
	
	Register = BREG_Read32(hHDMI->hRegister, BCHP_HDMI_RM_VCXO_CTRL) ;
	stPllStatus->Lock = BCHP_GET_FIELD_DATA(Register, HDMI_RM_VCXO_CTRL, LOCK) ;
	stPllStatus->VcxoRegister =  Register ;
	stPllStatus->uiPllKickStartCount = hHDMI->uiPllKickStartCount ;

	if (!stPllStatus->Lock)
	{
		BDBG_ERR(("_-_-_-_-_-_- HDMI PLL UnLocked _-_-_-_-_-_-")) ;
		BDBG_ERR(("VCXO Register %x", stPllStatus->VcxoRegister)) ;
	}	
	
	return BERR_SUCCESS ;
}


BERR_Code BHDM_Debug_KickStartPll(BHDM_Handle hHDMI)
{
	uint32_t Register ;
	BDBG_ASSERT(hHDMI) ;

	/* kick start PLL */
	/*	
	FILRSTB -----|__________________|---
	DIVRST	________|-----------|_______
	RESETB	------------|___|-----------
	*/

	hHDMI->uiPllKickStartCount++ ;
	BDBG_WRN(("Kickstarting HDMI PLL... Kickstart Count: %d", 
		hHDMI->uiPllKickStartCount)) ;
	
	Register = BREG_Read32(hHDMI->hRegister, BCHP_HDMI_RM_VCXO_CTRL) ;
	Register &= ~BCHP_MASK(HDMI_RM_VCXO_CTRL, reserved0) ;
	
	/* FILTER RESET */
	Register &= ~BCHP_MASK(HDMI_RM_VCXO_CTRL, FILRSTB) ;  /* 0 */
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_RM_VCXO_CTRL, Register) ;
	
		/* DIV_RST */
		Register &= ~BCHP_MASK(HDMI_RM_VCXO_CTRL, DIV_RST) ;		 /* 0 */
		BREG_Write32(hHDMI->hRegister, BCHP_HDMI_RM_VCXO_CTRL, Register) ;
		
		Register |= BCHP_MASK(HDMI_RM_VCXO_CTRL, DIV_RST) ; 		 /* 1 */
		BREG_Write32(hHDMI->hRegister, BCHP_HDMI_RM_VCXO_CTRL, Register) ;
		BKNI_Delay(1) ;  /* active high reset */

			/* INT_RESTB */
			Register |= BCHP_MASK(HDMI_RM_VCXO_CTRL, INT_RESETB) ;		 /* 1 */
			BREG_Write32(hHDMI->hRegister, BCHP_HDMI_RM_VCXO_CTRL, Register) ;

			Register &= ~BCHP_MASK(HDMI_RM_VCXO_CTRL, INT_RESETB) ; 	 /* 0 */
			BREG_Write32(hHDMI->hRegister, BCHP_HDMI_RM_VCXO_CTRL, Register) ;
			BKNI_Delay(1) ;  /* active low reset */

			Register |= BCHP_MASK(HDMI_RM_VCXO_CTRL, INT_RESETB) ;		 /* 1 */
			BREG_Write32(hHDMI->hRegister, BCHP_HDMI_RM_VCXO_CTRL, Register) ;
			BKNI_Delay(1) ;  /* active low reset */

		Register &= ~BCHP_MASK(HDMI_RM_VCXO_CTRL, DIV_RST) ;		 /* 0 */
		BREG_Write32(hHDMI->hRegister, BCHP_HDMI_RM_VCXO_CTRL, Register) ;
		BKNI_Delay(1) ;  

	Register |= BCHP_MASK(HDMI_RM_VCXO_CTRL, FILRSTB) ;   /* 1 */
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_RM_VCXO_CTRL, Register) ;

	/* allow some PLL settle time */		
	BKNI_Delay(10) ;

	Register = BREG_Read32(hHDMI->hRegister, BCHP_HDMI_RM_CONTROL) ;
	BDBG_WRN(("HDMI_RM_CONTROL: %x", Register)) ;
	
	Register = BREG_Read32(hHDMI->hRegister, BCHP_HDMI_RM_RATE_RATIO) ;
	BDBG_WRN(("HDMI_RM_RATE_RATIO: %x", Register)) ;
	
	Register = BREG_Read32(hHDMI->hRegister, BCHP_HDMI_RM_SAMPLE_INC) ; 
	BDBG_WRN(("HDMI_RM_SAMPLE_INC	: %x", Register)) ;
	
	Register = BREG_Read32(hHDMI->hRegister, BCHP_HDMI_RM_PHASE_INC) ;
	BDBG_WRN((" HDMI_RM_PHASE_INC	: %x", Register)) ;
	
	Register = BREG_Read32(hHDMI->hRegister, BCHP_HDMI_RM_INTEGRATOR) ; 
	BDBG_WRN(("HDMI_RM_INTEGRATOR	: %x", Register)) ;
	
	Register = BREG_Read32(hHDMI->hRegister, BCHP_HDMI_RM_VCXO_CTRL) ;
	BDBG_WRN(("HDMI_RM_VCXO_CTRL	: %x", Register)) ;
	
	Register = BREG_Read32(hHDMI->hRegister, BCHP_HDMI_RM_FORMAT	) ;
	BDBG_WRN(("HDMI_RM_FORMAT	: %x", Register)) ;
	
	Register = BREG_Read32(hHDMI->hRegister, BCHP_HDMI_ANALOG_CTL	) ;
	BDBG_WRN(("HDMI_ANALOG_CTL	: %x", Register)) ;
	
	return BERR_SUCCESS ;
}
#endif


/****************************
**  	PRIVATE FUNCTIONS 
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

#if BHDM_CONFIG_65NM_SUPPORT

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


#else

	Register = BREG_Read32(hHDMI->hRegister, BCHP_HDMI_DRIVER_PRE_EMPHASIS_CTL) ;
	hHDMI->DeviceSettings.uiDriverAmpDefault = BHDM_CONFIG_DRIVER_AMP_DEFAULT ;
	Register &= 
		~(BCHP_MASK(HDMI_DRIVER_PRE_EMPHASIS_CTL, RND_PWRDN)  
		 | BCHP_MASK(HDMI_DRIVER_PRE_EMPHASIS_CTL, DRIVER_AMP)) ;
	Register |= 	
		  BCHP_FIELD_DATA(HDMI_DRIVER_PRE_EMPHASIS_CTL, RND_PWRDN, 0) 
		| BCHP_FIELD_DATA(HDMI_DRIVER_PRE_EMPHASIS_CTL, DRIVER_AMP, 
			hHDMI->DeviceSettings.uiDriverAmpDefault) ;
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_DRIVER_PRE_EMPHASIS_CTL, Register) ;

	Register = BREG_Read32(hHDMI->hRegister, BCHP_HDMI_ANALOG_CTL) ; 
	Register |= BCHP_FIELD_DATA(HDMI_ANALOG_CTL, BIAS_CNT_17_0, 0x300) ;
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_ANALOG_CTL, Register) ;

#endif

	return;
}


void BHDM_P_PowerOffPhy (BHDM_Handle hHDMI)
{
	uint32_t Register;

#if BHDM_CONFIG_65NM_SUPPORT
	
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

#else

	Register = BREG_Read32(hHDMI->hRegister, BCHP_HDMI_DRIVER_PRE_EMPHASIS_CTL) ;
	hHDMI->DeviceSettings.uiDriverAmpDefault = BHDM_CONFIG_DRIVER_AMP_DEFAULT ;
	Register &= ~ BCHP_MASK(HDMI_DRIVER_PRE_EMPHASIS_CTL, RND_PWRDN);
	Register |= BCHP_FIELD_DATA(HDMI_DRIVER_PRE_EMPHASIS_CTL, RND_PWRDN, 1);
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_DRIVER_PRE_EMPHASIS_CTL, Register) ;

#endif

	return;
}


void BHDM_P_SetPreEmphasisMode (BHDM_Handle hHDMI, uint8_t uValue, uint8_t uDriverAmp)
{
	uint32_t Register;

#if BHDM_CONFIG_65NM_SUPPORT

	Register = BREG_Read32( hHDMI->hRegister, BCHP_HDMI_TX_PHY_HDMI_TX_PHY_CTL_0) ;
	Register &= ~(BCHP_MASK(HDMI_TX_PHY_HDMI_TX_PHY_CTL_0, DRV_AMP)
			  | BCHP_MASK(HDMI_TX_PHY_HDMI_TX_PHY_CTL_0, PREEMP_CK)
			  | BCHP_MASK(HDMI_TX_PHY_HDMI_TX_PHY_CTL_0, PREEMP_2)
			  | BCHP_MASK(HDMI_TX_PHY_HDMI_TX_PHY_CTL_0, PREEMP_1)
			  | BCHP_MASK(HDMI_TX_PHY_HDMI_TX_PHY_CTL_0, PREEMP_0));
	
	Register |= BCHP_FIELD_DATA(HDMI_TX_PHY_HDMI_TX_PHY_CTL_0, DRV_AMP, uDriverAmp)
			| BCHP_FIELD_DATA(HDMI_TX_PHY_HDMI_TX_PHY_CTL_0, PREEMP_CK, uValue)
			| BCHP_FIELD_DATA(HDMI_TX_PHY_HDMI_TX_PHY_CTL_0, PREEMP_2, uValue)
			| BCHP_FIELD_DATA(HDMI_TX_PHY_HDMI_TX_PHY_CTL_0, PREEMP_1, uValue)
			| BCHP_FIELD_DATA(HDMI_TX_PHY_HDMI_TX_PHY_CTL_0, PREEMP_0, uValue) ;
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_TX_PHY_HDMI_TX_PHY_CTL_0, Register) ;

#else

	Register = BREG_Read32( hHDMI->hRegister, BCHP_HDMI_DRIVER_PRE_EMPHASIS_CTL) ;
	Register &= ~(BCHP_MASK(HDMI_DRIVER_PRE_EMPHASIS_CTL, DRIVER_AMP)
		| BCHP_MASK(HDMI_DRIVER_PRE_EMPHASIS_CTL, PRE_EMPHASIS_CK)
		| BCHP_MASK(HDMI_DRIVER_PRE_EMPHASIS_CTL, PRE_EMPHASIS_C2)
		| BCHP_MASK(HDMI_DRIVER_PRE_EMPHASIS_CTL, PRE_EMPHASIS_C1)
		| BCHP_MASK(HDMI_DRIVER_PRE_EMPHASIS_CTL, PRE_EMPHASIS_C0)) ;
		
	Register |= BCHP_FIELD_DATA(HDMI_DRIVER_PRE_EMPHASIS_CTL, DRIVER_AMP, uDriverAmp)
		| BCHP_FIELD_DATA(HDMI_DRIVER_PRE_EMPHASIS_CTL, PRE_EMPHASIS_CK, uValue)
		| BCHP_FIELD_DATA(HDMI_DRIVER_PRE_EMPHASIS_CTL, PRE_EMPHASIS_C2, uValue)
		| BCHP_FIELD_DATA(HDMI_DRIVER_PRE_EMPHASIS_CTL, PRE_EMPHASIS_C1, uValue)
		| BCHP_FIELD_DATA(HDMI_DRIVER_PRE_EMPHASIS_CTL, PRE_EMPHASIS_C0, uValue) ;
		
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_DRIVER_PRE_EMPHASIS_CTL, Register) ;

#endif

	return;
}


#if BHDM_CONFIG_65NM_SUPPORT
BERR_Code BHDM_P_GetPreEmphasisConfiguration (
	BHDM_Handle hHDMI,
	BHDM_PreEmphasis_Configuration *stPreEmphasisConfig
)
{
	BERR_Code rc = BERR_SUCCESS;
	uint32_t Register;

	Register = BREG_Read32( hHDMI->hRegister, BCHP_HDMI_TX_PHY_HDMI_TX_PHY_CTL_0) ;
	stPreEmphasisConfig->uiDriverAmp = BCHP_GET_FIELD_DATA(Register,HDMI_TX_PHY_HDMI_TX_PHY_CTL_0,DRV_AMP);
	stPreEmphasisConfig->uiPreEmphasis_CK = BCHP_GET_FIELD_DATA(Register,HDMI_TX_PHY_HDMI_TX_PHY_CTL_0,PREEMP_CK);
	stPreEmphasisConfig->uiPreEmphasis_Ch2 = BCHP_GET_FIELD_DATA(Register,HDMI_TX_PHY_HDMI_TX_PHY_CTL_0,PREEMP_2);
	stPreEmphasisConfig->uiPreEmphasis_Ch1 = BCHP_GET_FIELD_DATA(Register,HDMI_TX_PHY_HDMI_TX_PHY_CTL_0,PREEMP_1);
	stPreEmphasisConfig->uiPreEmphasis_Ch0 = BCHP_GET_FIELD_DATA(Register,HDMI_TX_PHY_HDMI_TX_PHY_CTL_0,PREEMP_0);

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
		| BCHP_MASK(HDMI_TX_PHY_HDMI_TX_PHY_CTL_0, PREEMP_CK)
		| BCHP_MASK(HDMI_TX_PHY_HDMI_TX_PHY_CTL_0, PREEMP_2)
		| BCHP_MASK(HDMI_TX_PHY_HDMI_TX_PHY_CTL_0, PREEMP_1)
		| BCHP_MASK(HDMI_TX_PHY_HDMI_TX_PHY_CTL_0, PREEMP_0)) ;
		
	Register |=
		  BCHP_FIELD_DATA(HDMI_TX_PHY_HDMI_TX_PHY_CTL_0, DRV_AMP, stPreEmphasisConfig->uiDriverAmp)
		| BCHP_FIELD_DATA(HDMI_TX_PHY_HDMI_TX_PHY_CTL_0, PREEMP_CK, stPreEmphasisConfig->uiPreEmphasis_CK)
		| BCHP_FIELD_DATA(HDMI_TX_PHY_HDMI_TX_PHY_CTL_0, PREEMP_2, stPreEmphasisConfig->uiPreEmphasis_Ch2)
		| BCHP_FIELD_DATA(HDMI_TX_PHY_HDMI_TX_PHY_CTL_0, PREEMP_1, stPreEmphasisConfig->uiPreEmphasis_Ch1)
		| BCHP_FIELD_DATA(HDMI_TX_PHY_HDMI_TX_PHY_CTL_0, PREEMP_0, stPreEmphasisConfig->uiPreEmphasis_Ch0) ;
		
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_TX_PHY_HDMI_TX_PHY_CTL_0, Register) ;

	return rc;
}
#endif



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

