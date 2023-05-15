/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bhdr_fe.c $
 * $brcm_Revision: Hydra_Software_Devel/63 $
 * $brcm_Date: 10/2/12 12:44p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/hdr/src/core40nm/bhdr_fe.c $
 * 
 * Hydra_Software_Devel/63   10/2/12 12:44p rgreen
 * SW7145-2: Power up DVP_HR on FrontEnd Open; power down on FrontEnd
 * close
 * 
 * Hydra_Software_Devel/62   6/29/12 3:19p rgreen
 * SW7425-3377: Fix Coverity Error
 * 
 * Hydra_Software_Devel/61   5/31/12 7:20p rgreen
 * SW7425-3152: Reduce console debug messages
 * 
 * Hydra_Software_Devel/60   4/10/12 11:44a rgreen
 * SW7425-2851,SW7405-5557: Supress coverity dead code warning for code
 * used to warn for multiple front end implementation.  Downgraade WRN to
 * MSG
 * 
 * Hydra_Software_Devel/59   11/29/11 2:26p rgreen
 * SW7425-1858: Supress coverity deadcode warning
 * 
 * Hydra_Software_Devel/58   11/18/11 1:51p rgreen
 * SW7425-1381: Modify default equalizer settings; move
 * BHDR_FE_P_OpenChannel to private implementation file
 * 
 * Hydra_Software_Devel/57   11/1/11 2:11p rgreen
 * SW7435-20: add HDR support for 7435
 * 
 * Hydra_Software_Devel/56   10/10/11 3:46p rgreen
 * SW7429-21:  Add 7429 Support; remove unused reset phy controller for
 * all chips
 * 
 * Hydra_Software_Devel/55   9/29/11 5:25p rgreen
 * SW7425-1249: update multi channel pixel clock calculation
 * 
 * Hydra_Software_Devel/54   9/9/11 4:14p rgreen
 * SW7425-1249: Add support to calculate frequency on multiple channels
 * 
 * Hydra_Software_Devel/53   8/26/11 2:51p rgreen
 * SW7425-1200,SWDTV-7644:  Add dynamic power management support; Enable
 * DVP_HR timer support for 35230 only
 * 
 * Hydra_Software_Devel/52   8/19/11 3:28p rgreen
 * SWDTV-5441: Merge virtual TMR support back to main branch
 * 
 * Hydra_Software_Devel/SWDTV-5441/3   8/2/11 11:27a rgreen
 * SWDTV-5541: Update to use macro BHDR_P_FREQ_CHANGE_TIMER_MS instead of
 * hardcode for  StartTimer; reduce debug messages
 * 
 * Hydra_Software_Devel/SWDTV-5441/2   7/29/11 10:47a rgreen
 * SWDTV-5441: Remove depracated code for shared timer
 * 
 * Hydra_Software_Devel/SWDTV-5441/1   7/29/11 10:25a rgreen
 * SWDTV-5441: Use exclusive DVP_HR timer hardware instead of shared timer
 * hardware
 * 
 * Hydra_Software_Devel/51   8/10/11 5:29p rgreen
 * SW7425-692,SWDTV-7644: Fix incorrect macro name BHDR_PWR_SUPPORT to
 * BCHP_PWR_SUPPORT
 * 
 * Hydra_Software_Devel/50   8/10/11 4:04p rgreen
 * SW7425-692,SWDTV-7644: make power management functions private
 * 
 * Hydra_Software_Devel/48   7/18/11 4:33p rgreen
 * SW7425-692: Stub out BHDR_FE_Get/SetPowerState; used previously in the
 * HDMI_RX_3548 generation.  Probably can be depracated.
 * 
 * Hydra_Software_Devel/47   7/18/11 3:54p rgreen
 * SW7425-692: Add support for BCHP Power management; Remove references to
 * prior HDMI_RX_3548 generation power management
 * 
 * Hydra_Software_Devel/46   7/12/11 6:10p rgreen
 * SWDTV-6898: Default front end Equalizer to enabled
 * 
 * Hydra_Software_Devel/45   7/11/11 8:30p rgreen
 * SWDTV-6898: Explicitly assign BHDR Front End default settings vs
 * declaration
 * 
 * Hydra_Software_Devel/44   7/8/11 4:06p rgreen
 * SW7425-833: fix order of object destroy and free
 * 
 * Hydra_Software_Devel/43   7/7/11 4:56p rgreen
 * SW7425-833: Merge changes to use BDBG_OBJECT_ASSERT
 * 
 * Hydra_Software_Devel/SW7425-833/1   7/1/11 1:52p rgreen
 * SW7425-833: Add BDBG_OBJECT_ASSERT support
 * 
 * Hydra_Software_Devel/SWDTV-6898/1   6/29/11 2:00p rgreen
 * SWDTV-6898: fix incorrect access to hRegister from hFeChannel; use
 * hFrontend instead.
 * 
 * Hydra_Software_Devel/42   6/15/11 11:31a rbshah
 * SWDTV-6867: 35233: Correct merge error.
 * 
 * Hydra_Software_Devel/41   6/10/11 4:06p rgreen
 * SWDTV-6867, SWDTV-7419: Merge Changes
 * 
 * Hydra_Software_Devel/40   6/10/11 3:49p rgreen
 * SWDTV-6867, SWDTV-7419: Merge Changes
 * 
 * Hydra_Software_Devel/39   5/12/11 4:02p rgreen
 * SWDTV-6184,SWDTV-7092: Add private implentations for Front end
 * functions; reduce debug messages
 * 
 * Hydra_Software_Devel/hdr_35233/6   6/10/11 5:52p rbshah
 * SWDTV-6867: 35233: Incorporate review comments from Ron G.
 * 
 * Hydra_Software_Devel/hdr_35233/5   6/9/11 5:32p rbshah
 * SWDTV-7419: 35233: Ensure PLL Unlocked status is updated.
 * 
 * Hydra_Software_Devel/hdr_35233/4   6/1/11 3:43p rbshah
 * SWDTV-6867: 35233: Clean up after bring up
 * 
 * Hydra_Software_Devel/hdr_35233/3   5/9/11 4:53p rbshah
 * SWDTV-6867: 35233: Remove unnecessary chip specific code when equalizer
 * is enabled.
 * 
 * Hydra_Software_Devel/hdr_35233/2   5/2/11 8:21p rbshah
 * SWDTV-6867: 35233: Remove unnecessary logic in PLL lock-unlock handler.
 * 
 * Hydra_Software_Devel/hdr_35233/1   5/1/11 9:45a rbshah
 * SWDTV-6867: 35233: Initial bringup on 935233 CARD board.
 * 
 * SWDTV-6867: 35233: Initial bringup on 935233 CARD board.
 * 
  4/21/11 6:32p rgreen
 * SWDTV-6184: Create private implentations for functions to support 35233
 * 
 * Hydra_Software_Devel/36   4/20/11 10:09p rbshah
 * SWDTV-6184: 35233: Get HDR PI to build for the 35233 DTV chip.
 * 
 * Hydra_Software_Devel/35   4/20/11 4:58p rgreen
 * SWDTV-6184: Depracate BHDR_PulseHotPlug; reduce debug warning messages
 * 
 * Hydra_Software_Devel/34   4/18/11 6:37p rgreen
 * SW7425-186: Update setting for HOTPLUG_BYPASS for repeater applications
 * 
 * Hydra_Software_Devel/32   4/6/11 11:12a rbshah
 * SWDTV-6184: 35233: Get HDR PI to build for the 35233 DTV chip.
 * 
 * Hydra_Software_Devel/30   3/17/11 5:19p rgreen
 * SW3548-2013: Remove excess debug messages
 * 
 * Hydra_Software_Devel/29   12/2/10 2:42p rgreen
 * SW35230-2416: callback to mute handler when clock stops; Add missing
 * assignment for HDR handle to FeChannel
 * 
 * Hydra_Software_Devel/28   10/12/10 5:37p rgreen
 * SW35230-1598: Add Hdcp Reset function.  Reset HDCP when attaching FE to
 * RX.  Update audio Enable/Disable calls for AvMute
 * 
 * Hydra_Software_Devel/27   10/11/10 10:29a rgreen
 * SW35230-1614: Protect isr calls with CriticalSection; Fix interrupt IDs
 * for CLOCK_STOP and PLL_UNLOCK
 * 
 * Hydra_Software_Devel/26   9/29/10 5:20p rgreen
 * SW3548-3078: Update settings name
 * 
 * Hydra_Software_Devel/25   9/21/10 11:28a rgreen
 * SW35230-1347: Reset Frequency Range when PLL_LOCK is acquired.
 * Separate calls for resetting clock and data channels.  Update isr
 * context when calling the reset functions
 * 
 * Hydra_Software_Devel/24   9/16/10 3:54p rgreen
 * SW35230-1363,SW7425-21: Setup Frequency Range Detection for 35230 only
 * 
 * Hydra_Software_Devel/23   9/16/10 3:11p rgreen
 * SW35230-1363,SW7425-21: Move chip/phy specific code to private file
 * 
 * Hydra_Software_Devel/22   9/15/10 5:21p rgreen
 * SW35230-1363,SW7425-21: Move chip/phy specific code to private file
 * 
 * Hydra_Software_Devel/21   9/10/10 2:46p rgreen
 * SW35230-1193: Update  BHDR_P_ReadPixelClock_isr.; Pixel clock now read
 * in one location; report start/stop of clock Display FE debug messages
 * when BHDR_CONFIG_DEBUG_FRONT_END macro is set Remove unused code
 * 
 * Hydra_Software_Devel/20   9/1/10 5:07p rgreen
 * SW35230-1241: Enable clock stop as hotplug setting
 * 
 * Hydra_Software_Devel/19   8/26/10 3:37p rgreen
 * SW35230-790: Clear override range when clock when frequency is out of
 * range  Reset Pixel Clock count on freq change, clock stop, etc. force
 * pixel count detection after clock stops
 * 
 * Hydra_Software_Devel/18   8/10/10 5:28p rgreen
 * SW35230-880, SW35230-790, SW35230-806: Removed
 * BHDR_FE_P_CheckForStableFrequency_isr Added additional Channel Reset
 * after equalizer enable and PLL lock
 * 
 * Hydra_Software_Devel/17   8/10/10 10:59a rgreen
 * SW35230-880: Reduced delay to avoid interrupt issue in VEC
 * 
 * Hydra_Software_Devel/16   8/5/10 11:11a rgreen
 * SW35230-839, SW35230-807, SW35230-806, SW35230-810: enabled equalizer,
 * fixed format changing
 * 
 * Hydra_Software_Devel/15   7/29/10 1:59p rgreen
 * SW35230-807,SW35230-810: disable timer in isr at expired timer
 * interrupt; do not restart timer on successive pll lock interrupts;
 * reset fe channels on PLL Lock
 * 
 * Hydra_Software_Devel/14   7/27/10 8:06p rgreen
 * SW35230-568:  Improve front end auto frequency range detection  Disable
 * Freq adjustment until HDMI RX core is attached to front end Override
 * analog calibration
 * 
 * Hydra_Software_Devel/13   7/21/10 6:56p rgreen
 * SW35230-568,SW35230-686: Improve front end auto frequency range
 * detection Update naming convention for isr functions; update debug
 * messages Add BHDR_CONFIG_DEBUG_FRONT_END option Disable HDMI RX core
 * interrupt callbacks until core is attached to front end Remove HP
 * detect filter workaround for prior chip
 * 
 * Hydra_Software_Devel/12   7/13/10 1:05p rgreen
 * SW35230-568: Update FE processing to handle more format switches
 * 
 * Hydra_Software_Devel/11   7/11/10 10:43p rgreen
 * SW35230-568: Update FE processing to handle more format switches
 * 
 * Hydra_Software_Devel/10   7/2/10 6:33p rgreen
 * SW35230-451,SW35230-502: Update frontend range detection Disable
 * workaround to force hotplug state to connected
 * 
 * Hydra_Software_Devel/9   7/1/10 6:54p rgreen
 * SW35230-451: Update frontend range detection
 * 
 * Hydra_Software_Devel/8   6/24/10 5:37p rgreen
 * SW35230-424: Reset front end channels on frequency change Make sure PLL
 * reference range detection override is disabled when reset.
 * 
 * Hydra_Software_Devel/7   6/15/10 8:38p rgreen
 * SW35230-128:  Update pixel clock calculation Depracate ResetPll
 * function Add function to adjust PLL range
 * 
 * Hydra_Software_Devel/6   6/12/10 10:28p rgreen
 * SW35230-128: Update front end phy initialization; add workaround to
 * force HP connected state
 * 
 * Hydra_Software_Devel/5   6/11/10 2:29p rgreen
 * SW35230-284: Remove depracated HDMI Rx Front End interrupts/events
 * 
 * Hydra_Software_Devel/4   5/14/10 2:04p rgreen
 * SW35230-128: Fix DetachHdmiCore API declaration
 * 
 * Hydra_Software_Devel/3   1/11/10 11:04a rgreen
 * SW3548-2642: Rename BHDR_P_HotPlugDisconnect_isr
 * BHDR_P_HotPlugRemove_isr
 * 
 * Hydra_Software_Devel/2   12/1/09 2:59p rgreen
 * SW35230-15: Remove CEC; no direct support on 35230
 * 
 * Hydra_Software_Devel/1   11/25/09 3:13p rgreen
 * SW35230-15: Add 35230 HDR Support
 * 
 * 
 ***************************************************************************/

#include "bhdr.h"
#include "bhdr_priv.h"

#include "bhdr_fe.h"
#include "bhdr_fe_priv.h"

#if BCHP_PWR_SUPPORT
#include "bchp_pwr.h"
#endif

#include "bchp_hdmi_rx_fe_0.h"
#include "bchp_hdmi_rx_eq_0.h"
#include "bchp_dvp_hr_tmr.h"

BDBG_MODULE(BHDR_FE) ;

BDBG_OBJECT_ID(BHDR_FE_P_Handle);
BDBG_OBJECT_ID(BHDR_FE_P_ChannelHandle);

#define	BHDR_CHECK_RC( rc, func )	          \
do                                                \
{										          \
	if( (rc = BERR_TRACE(func)) != BERR_SUCCESS ) \
	{										      \
		goto done;							      \
	}										      \
} while(0)



typedef struct BHDR_FE_P_InterruptCbTable
{
	BINT_Id       IntrId;
	int               iParam2; 
	bool             enable ; /* debug purposes */
} BHDR_FE_P_InterruptCbTable ;


static const BHDR_FE_P_InterruptCbTable BHDR_FE_P_Intr[MAKE_INTR_FE_ENUM(LAST)] =
{
	/* 16 */   { BCHP_INT_ID_TMR_IRQ_0, BHDR_FE_INTR_eTMR_IRQ_0, true },
	/* 17 */   { BCHP_INT_ID_TMR_IRQ_1, BHDR_FE_INTR_eTMR_IRQ_1, true }
} ;	



static const BHDR_FE_P_InterruptCbTable BHDR_FE_P_ChannelIntr0[MAKE_INTR_ENUM(LAST)] =
{
#if BHDR_CONFIG_DUAL_HPD_SUPPORT
	/* 00 */   { BCHP_INT_ID_HP_CONNECTED_0, BHDR_FE_CHN_INTR_eHPD_CONNECTED, true},
	/* 01 */   { BCHP_INT_ID_HP_REMOVED_0, BHDR_FE_CHN_INTR_eHPD_REMOVED, true},
#else
	/* 00 */   { BCHP_INT_ID_RX_HOTPLUG_UPDATE_0, BHDR_FE_CHN_INTR_eRX_HOTPLUG_UPDATE, true},
#endif	
	/* 01 */   { BCHP_INT_ID_CLOCK_STOP_0, BHDR_FE_CHN_INTR_eCLOCK_STOP_0, true},
	/* 02*/   { BCHP_INT_ID_PLL_UNLOCK_0, BHDR_FE_CHN_INTR_ePLL_UNLOCK_0, true},
	/* 03 */   { BCHP_INT_ID_PLL_LOCK_0, BHDR_FE_CHN_INTR_ePLL_LOCK_0, true},
	/* 04 */   { BCHP_INT_ID_FREQ_CHANGE_0,  BHDR_FE_CHN_INTR_eFREQ_CHANGE_0, true},
} ;


static const BHDR_FE_P_InterruptCbTable *pInterrupts ;


#if BHDR_CONFIG_DVP_HR_TMR 
static void BHDR_FE_P_FrontEnd_isr(void *pParam1, int parm2) ;
#endif
static void BHDR_FE_P_Channel_isr(void *pParam1, int parm2) ;
 

/* The timers run at 27Mhz (27,000,000 times a second) and a microsecond is 1,000,000th of a second, so ... */
#define BHDR_P_ConversionFactor (27000000/1000000) /* <- yeah, I know, its 27 -- but the compiler will figure that out, right? */
unsigned long BHDR_P_MicroSecondsToTimer(unsigned long microSeconds) { return microSeconds * BHDR_P_ConversionFactor; }


/*******************************************************************************
BERR_Code BHDR_FE_GetDefaultSettings
Summary: Get the default settings for the HDMI device.
*******************************************************************************/
BERR_Code BHDR_FE_GetDefaultSettings(
	BCHP_Handle hChip,	
	BHDR_FE_Settings *pDefaultSettings  /* [in] pointer to memory to hold default settings */
) 
{
	BSTD_UNUSED(hChip) ;
	
	BDBG_ENTER(BHDR_FE_GetDefaultSettings) ;

	BKNI_Memset(pDefaultSettings, 0, sizeof(BHDR_FE_Settings)) ;

	pDefaultSettings->NoSettings = 0 ;

	BDBG_LEAVE(BHDR_FE_GetDefaultSettings) ;
	return BERR_SUCCESS ;
}


/******************************************************************************
BERR_Code BHDR_FE_Open
Summary: Open/Initialize the HDMI Rx Front End device .  The Front End can contain multiple channels
*******************************************************************************/
BERR_Code BHDR_FE_Open(
	BHDR_FE_Handle *phFrontEnd,       /* [in] HDMI Rx handle */
	BCHP_Handle hChip,                  /* Chip handle */
	BREG_Handle hRegister,              /* Register handle */
	BINT_Handle hInterrupt,             /* Interrupt handle */   
	const BHDR_FE_Settings  *pSettings /* [in] default HDMI settings */
) 
{
	BERR_Code      rc = BERR_SUCCESS ;
	BHDR_FE_Handle hFrontEnd = NULL ;
	uint32_t Register ;


	BDBG_ENTER(BHDR_FE_Open) ;

	/* verify parameters */
	BDBG_ASSERT( hChip );
	BDBG_ASSERT( hRegister );
	BDBG_ASSERT( hInterrupt );
	

#if BHDR_CONFIG_DEBUG_DISPLAY_FE_CONFIG
	/* display version information */
	BDBG_WRN(("*****************************************")) ;
	BDBG_WRN(("HDMI Receiver Frontend ")) ;
	BDBG_WRN(("$brcm_Workfile: bhdr_fe.c $")) ;
	BDBG_WRN(("$brcm_Revision: Hydra_Software_Devel/63 $")) ;
	BDBG_WRN(("*****************************************")) ;
#endif	

	/* create HDMI Front End (port) handle */	
	hFrontEnd = (BHDR_FE_Handle) BKNI_Malloc(sizeof(BHDR_FE_P_Handle)) ;
	if (!hFrontEnd)
	{
		BDBG_ERR(("Unable to allocate memory for HDMI Front End (Port) Handle")) ;
		rc = BERR_OUT_OF_SYSTEM_MEMORY ;
		goto done ;
	}

	/* zero out memory associated with the HDMI Front End Handle before using */
	BKNI_Memset(hFrontEnd, 0, sizeof(BHDR_FE_P_Handle)) ;
	BDBG_OBJECT_SET(hFrontEnd, BHDR_FE_P_Handle) ;
	
	hFrontEnd->DeviceSettings = *pSettings ;

	hFrontEnd->hRegister = hRegister ;
	hFrontEnd->hInterrupt = hInterrupt ;
	hFrontEnd->hChip = hChip ;


#if BCHP_PWR_SUPPORT
	/* Acquire DVP_HR resources for RDB access */
	BHDR_FE_P_PowerResourceAcquire_DVP_HR(hFrontEnd) ;
#endif

	/* ensure the DVP_HR block is powered on */	
	Register = BREG_Read32( hRegister, BCHP_DVP_HR_POWER_CONTROL );
	Register &= ~ BCHP_MASK(DVP_HR_POWER_CONTROL, RX_PHY_0_POWER_DOWN) ;
	BREG_Write32(hRegister, BCHP_DVP_HR_POWER_CONTROL, Register );


	/* Take HDMI Rx Cores out of reset  */
	BHDR_FE_P_Initialize(hFrontEnd) ;
	
	Register = BREG_Read32(hRegister, BCHP_HDMI_RX_FE_0_ANALOG_CFG_3) ;
	Register &= ~ BCHP_MASK(HDMI_RX_FE_0_ANALOG_CFG_3, 	USE_CDR_LOCK) ;
	BREG_Write32(hRegister, BCHP_HDMI_RX_FE_0_ANALOG_CFG_3, Register) ;


#if BHDR_CONFIG_DVP_HR_TMR 
	/* initialize timer0 for countdown */
	{
		BTMR_DeviceSettings stDeviceSettings;

		/* Set up the HDMI Rx FE Timer hardware */
		BTMR_GetDefaultDeviceSettings(&stDeviceSettings);
			stDeviceSettings.baseRegister = BCHP_DVP_HR_TMR_TIMER_IS; /* this is base of this timer block */
			stDeviceSettings.interruptNumber = BCHP_INT_ID_TMR_IRQ_0; /* this is the level two interrupt that captures the timer interrupt */
		rc = BTMR_Open(&hFrontEnd->hTimerDevice, hChip, hRegister, hInterrupt, &stDeviceSettings);

	}
#endif

	/* keep created pointer */
	*phFrontEnd = hFrontEnd ;

done:       

	if( (rc != BERR_SUCCESS) && (hFrontEnd))
	{
#if BCHP_PWR_SUPPORT 
	    /* Release DVP_HR resources in case of failure */	    
	    BHDR_FE_P_PowerResourceRelease_DVP_HR(hFrontEnd) ;
#endif

	    BKNI_Free(hFrontEnd) ;
	    *phFrontEnd = NULL  ;
	}
	    	
	BDBG_LEAVE(BHDR_FE_Open) ;
	return rc ;
}



/***************************************************************************
BERR_Code BHDR_FE_Close
Summary: Close the HDMI Rx Front End Block
****************************************************************************/
BERR_Code BHDR_FE_Close(BHDR_FE_Handle hFrontEnd) 
{
	BERR_Code rc = BERR_SUCCESS ;
	uint32_t Register ;

	BDBG_ENTER(BHDR_FE_Close) ;
	BDBG_OBJECT_ASSERT(hFrontEnd, BHDR_FE_P_Handle) ;

#if BHDR_CONFIG_DVP_HR_TMR 
	BTMR_Close(hFrontEnd->hTimerDevice) ;
#endif

	/* power down the HDMI Rx Phy when closing */
	/* NOTE: close after timers have been disabled/destroyed */
	Register = BREG_Read32( hFrontEnd->hRegister, BCHP_DVP_HR_POWER_CONTROL );
	Register |= BCHP_MASK(DVP_HR_POWER_CONTROL, RX_PHY_0_POWER_DOWN) ;
	BREG_Write32(hFrontEnd->hRegister, BCHP_DVP_HR_POWER_CONTROL, Register );

#if BCHP_PWR_SUPPORT 
	BHDR_FE_P_PowerResourceRelease_DVP_HR(hFrontEnd) ;
#endif

	/* free HDMI Front End (port) handle */
	BDBG_OBJECT_DESTROY(hFrontEnd, BHDR_FE_P_Handle) ;
	BKNI_Free(hFrontEnd) ;

	BDBG_LEAVE(BHDR_FE_Close) ;
	return rc ;
}



/***************************************************************************
BERR_Code BHDR_FE_GetTotalChannels
Summary: Get the number of available channels for the Front End
****************************************************************************/
BERR_Code BHDR_FE_GetTotalChannels(
    BHDR_FE_Handle hFrontEnd, uint8_t *uiTotalChannels) 
{
	BERR_Code rc = BERR_SUCCESS ;

	BDBG_OBJECT_ASSERT(hFrontEnd, BHDR_FE_P_Handle) ;
	
	*uiTotalChannels = BHDR_FE_MAX_CHANNELS ;
	
	return rc ;
}



/*******************************************************************************
BERR_Code BHDR_FE_GetDefaultChannelSettings
Summary: Get the default settings for the HDMI device.
*******************************************************************************/
BERR_Code BHDR_FE_GetDefaultChannelSettings(
	BHDR_FE_Handle hFrontEnd, 
	BHDR_FE_ChannelSettings *pChSettings
) 
{
	BSTD_UNUSED(hFrontEnd) ;

	BDBG_ENTER(BHDR_FE_GetDefaultChannelSettings) ;

	BKNI_Memset(pChSettings, 0, sizeof(BHDR_FE_ChannelSettings)) ;

	pChSettings->uiChannel = 0 ;

	/* enable EQ on reference boards with no HDMI switch  (default) */
	/* disable EQ on boards that use a HDMI switch in front of the BCM chip */
	/* setting can be changed in BHDR_FE_OpenChannel Settings */	
	pChSettings->bEnableEqualizer = true ; 

	pChSettings->bExternalSwitch = false ;
	pChSettings->bHpdDisconnected = BHDR_CONFIG_HPD_DISCONNECTED ;


	BDBG_LEAVE(BHDR_FE_GetDefaultChannelSettings) ;
	return BERR_SUCCESS ;
}



/******************************************************************************
Summary:
*******************************************************************************/
BERR_Code BHDR_FE_OpenChannel(
	BHDR_FE_Handle hFrontEnd,       /* [in] HDMI Rx handle */
	BHDR_FE_ChannelHandle *phChannel, /* [out] Created channel handle */
	const BHDR_FE_ChannelSettings  *pChannelSettings /* [in] default HDMI settings */
) 
{
	BERR_Code rc = BERR_SUCCESS ;
	BHDR_FE_ChannelHandle hFeChannel ;

	BREG_Handle hRegister ;
	uint32_t Register ;
	uint32_t ulOffset ;
	uint8_t i ;
	uint8_t  uiChannel ;
	
#if  BHDR_CONFIG_LOOP_FILTER_SETTINGS
	uint8_t 
		uiEqInputClkSampleNeg,
		uiLoopFilterHighThreshold, 
		uiLoopFilterLowThreshold ;
#endif

	BDBG_ENTER(BHDR_FE_OpenChannel) ;
	BDBG_ASSERT(pChannelSettings) ;

	/* verify valid channel */
	uiChannel = pChannelSettings->uiChannel ;
	if (uiChannel >= BHDR_FE_P_eChannelMax)
	{
		rc = BHDR_INVALID_RESOURCE  ;
		goto done ;
	}

#if BHDR_CONFIG_DEBUG_DISPLAY_FE_CONFIG
	BDBG_WRN(("FE_%d Block Configuration", uiChannel)) ;
	BDBG_WRN(("    FE_%d  Equalizer: %s ", 
		uiChannel, pChannelSettings->bEnableEqualizer ? "Yes" : "No")) ;
	BDBG_WRN(("    FE_%d  External Switch: %s" , 
		uiChannel, pChannelSettings->bExternalSwitch ? "Yes" : "No")) ;
	BDBG_WRN(("    FE_%d  HPD Pin Disconnected: %s", 
		uiChannel, pChannelSettings->bHpdDisconnected  ? "Yes" : "No")) ;
	BDBG_WRN(("    FE_%d  Override Calibration: %s", 
		uiChannel, BHDR_CONFIG_OVERRIDE_ANALOG_CALIBRATION  ? "Yes" : "No")) ;
#endif
	
	/* create/allocate HDMI Front End (port) channel handle */
	hFeChannel = BKNI_Malloc(sizeof(BHDR_FE_P_ChannelHandle)) ;
	if (!hFeChannel)
	{
		BDBG_ERR(("Unable to allocate memory for HDMI FE Channel Handle")) ;
		rc = BERR_OUT_OF_SYSTEM_MEMORY ;
		goto done ;
	}

	/* zero out memory associated with the HDMI Front End Channel before using */
	BKNI_Memset(hFeChannel, 0, sizeof(BHDR_FE_P_ChannelHandle)) ;
	BDBG_OBJECT_SET(hFeChannel, BHDR_FE_P_ChannelHandle);

	/* assign settings */
	hFeChannel->eChannel = uiChannel ;	
	if (uiChannel)
	{
		BDBG_ERR(("*** Multiple FE Channels; Fix register offset! ***")) ;
		rc = BERR_INVALID_PARAMETER ;
		goto done ;
	}
	
	hFeChannel->ulOffset = ulOffset =  0 ;
	
	hRegister = hFeChannel->hRegister = hFrontEnd->hRegister ;
	hFeChannel->hChip = hFrontEnd->hChip ;

#if BCHP_PWR_SUPPORT 
	BHDR_FE_P_PowerResourceAcquire_HDMI_RX_FE(hFeChannel);
#endif

#if BHDR_CONFIG_DVP_HR_TMR 
	/* get the timerDevice for creating/using timers */
	hFeChannel->hTimerDevice = hFrontEnd->hTimerDevice ;
	{
	   	BTMR_Settings timerSettings  ;
	   	BTMR_GetDefaultTimerSettings(&timerSettings) ;
	   		timerSettings.type =  BTMR_Type_eCountDown ;
	   		timerSettings.cb_isr = (BTMR_CallbackFunc) BHDR_FE_P_FrontEnd_isr;
	   		timerSettings.pParm1 = hFrontEnd ;
	   		timerSettings.parm2 = hFeChannel->eChannel ;
	   		timerSettings.exclusive = true ;

	   	BHDR_CHECK_RC(rc, 
			BTMR_CreateTimer(hFeChannel->hTimerDevice,  &hFeChannel->hFeTimer, &timerSettings)) ;
	}
#endif

	BHDR_FE_P_OpenChannel(hFeChannel) ;

#if BHDR_CONFIG_FREQUENCY_RANGE_DETECTION
	BHDR_FE_P_SetupFrequencyRangeDetection(hFeChannel) ;
#endif

	/* enable/disable equalizer as requested */
	BHDR_FE_EnableEqualization(hFeChannel, pChannelSettings->bEnableEqualizer) ;

	BKNI_EnterCriticalSection() ;
		BHDR_FE_P_ResetFeClockChannel_isr(hFeChannel) ;
	BKNI_LeaveCriticalSection() ;


	/* Register/enable interrupt callbacks for channel */

	/* coverity[dead_error_condition] */
	switch (uiChannel)
	{
	case BHDR_FE_P_eChannel0 :
		pInterrupts = BHDR_FE_P_ChannelIntr0 ;
		break ;
	
	/* coverity[dead_error_condition] - channel validity test done above.. should never execute here */
	default :
		pInterrupts = BHDR_FE_P_ChannelIntr0 ;
		BDBG_ERR(("Unknown Front End Channel ID %d", uiChannel)) ;
		BDBG_ASSERT(false) ;
	}
	
	for (i = 0; i < MAKE_INTR_FE_CHN_ENUM(LAST) ; i++) 
	{
 		BHDR_CHECK_RC( rc, BINT_CreateCallback(	&(hFeChannel->hCallback[i]), 
			hFrontEnd->hInterrupt, pInterrupts[i].IntrId, 
			BHDR_FE_P_Channel_isr, (void *) hFeChannel, i ));
			
		/* clear interrupt callback */
		BHDR_CHECK_RC(rc, BINT_ClearCallback( hFeChannel->hCallback[i])) ;

		/* skip interrupt if not enabled in table...  */	
		if (!pInterrupts[i].enable)
			continue ;

		/* enable interrupts if HPD signal is connected  */
		/* i.e. direct connection or switch with HPD connected */
		if (!pChannelSettings->bHpdDisconnected)
		{
			BHDR_CHECK_RC( rc, BINT_EnableCallback( hFeChannel->hCallback[i] )) ;
		}
	}


	/* check if a device is attached and init the pixel count; ensures an update message */
	Register = BREG_Read32( hRegister, BCHP_HDMI_RX_FE_0_HOTPLUG_STATUS + ulOffset) ;
	hFeChannel->bTxDeviceAttached = BCHP_GET_FIELD_DATA(Register, 
			HDMI_RX_FE_0_HOTPLUG_STATUS, RX_HOTPLUG_IN) ;

	BDBG_MSG(("FE_%d RX HOT PLUG (HPD) : %s ", 
		hFeChannel->eChannel, 
		hFeChannel->bTxDeviceAttached ? "HIGH" : "LOW")) ;		

	hFeChannel->bPllLocked = false ;
	hFeChannel->PreviousbPllLocked = false ;
	hFeChannel->PreviousPixelClockCount = 0 ;
	

	/* default HDMI Channel Power Settings to on */
	hFeChannel->stPowerSettings.bHdmiRxPowered = true ;

	hFeChannel->uiHdrSel = BHDR_P_eHdrCoreIdNotAttached ;	
	
	BKNI_Memcpy(&hFeChannel->settings, pChannelSettings, 
		sizeof(BHDR_FE_ChannelSettings)) ;

	/* keep created Front End Channel and its settings */
	*phChannel = hFeChannel ;	

	hFrontEnd->channel[uiChannel] = hFeChannel ;

#if BCHP_PWR_SUPPORT
	BHDR_FE_P_PowerResourceRelease_HDMI_RX_FE(hFeChannel);
#endif

done:
	BDBG_LEAVE(BHDR_FE_OpenChannel) ;
	return rc ;

}



/******************************************************************************
Summary:
*******************************************************************************/
BERR_Code BHDR_FE_CloseChannel(
	BHDR_FE_ChannelHandle hFeChannel /* Created channel handle */
) 
{
	BERR_Code rc = BERR_SUCCESS ;
	uint8_t i ;

	BDBG_ENTER(BHDR_FE_CloseChannel) ;
	BDBG_OBJECT_ASSERT(hFeChannel, BHDR_FE_P_ChannelHandle);	

#if BHDR_CONFIG_DVP_HR_TMR 
	/* destroy used timer */
	if (hFeChannel->hFeTimer)
	{
		BTMR_DestroyTimer(hFeChannel->hFeTimer) ;
	}
#endif

	/* disable/destroy callbacks */
	for (i = 0; i < MAKE_INTR_FE_CHN_ENUM(LAST) ; i++) 
	{
 		BHDR_CHECK_RC( rc, BINT_DisableCallback(hFeChannel->hCallback[i])) ;
			
		BHDR_CHECK_RC(rc, BINT_DestroyCallback( hFeChannel->hCallback[i])) ;
	}	

	BHDR_FE_P_CloseChannel(hFeChannel) ;	

	BDBG_OBJECT_DESTROY(hFeChannel, BHDR_FE_P_ChannelHandle);
	BKNI_Free(hFeChannel) ;

done:
	BDBG_LEAVE(BHDR_FE_CloseChannel) ;
	return rc ;
}


/******************************************************************************
Summary:
*******************************************************************************/
BERR_Code BHDR_SetHotPlug(BHDR_Handle hHDR, BHDR_HotPlugSignal eHotPlugSignal)           
{
	BERR_Code rc = BERR_SUCCESS ;

	BDBG_OBJECT_ASSERT(hHDR, BHDR_P_Handle) ;


	BKNI_EnterCriticalSection() ;
		if (eHotPlugSignal == BHDR_HotPlugSignal_eHigh)
		{
			BDBG_MSG(("Force Hot Plug High (Attach Device)... ")) ;
			BHDR_P_HotPlugConnect_isr(hHDR) ;
		}
		else
		{
			BDBG_MSG(("Force Hot Plug Low (Detach Device)... ")) ;
			BHDR_P_HotPlugRemove_isr(hHDR) ;
		}
	BKNI_LeaveCriticalSection() ;
	
	BHDR_FE_P_SetHotPlug(hHDR->hFeChannel, eHotPlugSignal) ;

	return rc ;
}


/******************************************************************************
Summary:
*******************************************************************************/
BERR_Code BHDR_PulseHotPlug(BHDR_Handle hHDR)
{
	BSTD_UNUSED(hHDR) ;
	BDBG_WRN(("BHDR_PulseHotPlug DEPRACATED...")) ;
	BDBG_WRN(("Use higher NEXUS API NEXUS_HdmiInput_ToggleHotPlug to pulse")) ;	
	
	return BERR_NOT_SUPPORTED ;
}



/******************************************************************************
Summary:
*******************************************************************************/
void BHDR_FE_P_GetPllLockStatus_isr(BHDR_FE_ChannelHandle hFeChannel, bool *bLocked) 
{
	BREG_Handle hRegister  ;
	uint32_t ulOffset ;
	uint32_t Register ;
	
	uint8_t PllLock ;
	
	BDBG_OBJECT_ASSERT(hFeChannel, BHDR_FE_P_ChannelHandle);
	hRegister = hFeChannel->hRegister ;
	/* get offset for Front End */
	ulOffset = hFeChannel->ulOffset ;

	Register = BREG_Read32( hRegister, BCHP_HDMI_RX_FE_0_ANALOG_STATUS_2 + ulOffset) ;
	PllLock =	BCHP_GET_FIELD_DATA(Register, HDMI_RX_FE_0_ANALOG_STATUS_2, PLL_LOCK) ;
	hFeChannel->bPllLocked = PllLock ;

	*bLocked = false ;
	if (PllLock)
		*bLocked = true ;
}



/******************************************************************************
Summary:
*******************************************************************************/
BERR_Code BHDR_FE_P_GetPixelClockEstimate_isr(BHDR_FE_ChannelHandle hFeChannel,	
	uint32_t *EstimatedPixelClockRate 
)
{
	uint32_t rc = BERR_SUCCESS ;
	uint32_t PixelClockCount ;

	BHDR_FE_P_PixelClockStatus FePixelClockStatus[BHDR_FE_P_CLOCK_eChMax] ;


	bool bPixelClockChange ;
	bool bPllLocked ;
	uint8_t ClockStopped ;

	BDBG_OBJECT_ASSERT(hFeChannel, BHDR_FE_P_ChannelHandle) ;

	*EstimatedPixelClockRate = 0 ;

	/* first check if device is attached (or alternatively) if the PLL is locked... */
	BHDR_FE_P_GetPllLockStatus_isr(hFeChannel, &bPllLocked) ;

	/* Now read the pixel count register and note changes */
	BHDR_FE_P_GetPixelClockStatus_isr(hFeChannel, FePixelClockStatus) ;

	PixelClockCount = FePixelClockStatus[BHDR_FE_P_CLOCK_eChRef].PixelCount ;
	ClockStopped = FePixelClockStatus[BHDR_FE_P_CLOCK_eChRef].bClockStopped ;
 
	if ((hFeChannel->PreviousPixelClockCount > PixelClockCount + 5) 
	|| (hFeChannel->PreviousPixelClockCount + 5 < PixelClockCount ))
	{
		bPixelClockChange = true ;
		hFeChannel->PreviousPixelClockCount = PixelClockCount ;
	}
	else
		bPixelClockChange = false ;

	/* warn (only once); Tx device is attached, but probably stopped transmitting */
	if ((bPixelClockChange) && (!PixelClockCount))
	{
		BDBG_WRN(("FE_%d Pixel Clock Count is 0; Unable to calculate pixel clock...",
			hFeChannel->eChannel)) ;
		goto done ;
	}

	/* do not calculate Pixel Clock if count = 0 */
	if (!PixelClockCount)
	{
		goto done ;
	}

	/* Calculate the pixel clock rate */
	*EstimatedPixelClockRate = BHDR_FE_P_PIXEL_CLOCK_COUNT_TO_RATE(PixelClockCount) ;
	
#if BDBG_DEBUG_BUILD
	/* Inform of changes */
	if (bPixelClockChange)
	{
		BDBG_WRN(("FE_%d Estimated Pixel (%d) Clock Rate: %d.%d MHz", 
			hFeChannel->eChannel, PixelClockCount, 
			(*EstimatedPixelClockRate) / 1000, 
			(*EstimatedPixelClockRate) % 1000)) ;
		
	}
#endif

done:
	hFeChannel->PreviousbPllLocked = hFeChannel->bPllLocked ;
	hFeChannel->bPllLocked = bPllLocked ;
	return rc ;
}



#if BHDR_CONFIG_DVP_HR_TMR 
/******************************************************************************
void BHDR_FE_P_FrontEnd_isr
Summary: Handle interrupts from the HDMI Front End (not the channels)
*******************************************************************************/
static void BHDR_FE_P_FrontEnd_isr(
	void *pParam1,						/* [in] Device handle */
	int parm2							/* [in] Interrupt ID */
)
{
	BHDR_FE_Handle hFrontEnd  ;
	BREG_Handle hRegister ;

	hFrontEnd = (BHDR_FE_Handle) pParam1 ;
	BDBG_OBJECT_ASSERT(hFrontEnd, BHDR_FE_P_Handle) ;

	hRegister = hFrontEnd->hRegister ;

	switch (parm2)
	{
	case MAKE_INTR_FE_ENUM(TMR_IRQ_0) :

#if BHDR_CONFIG_MANUAL_FREQUENCY_DETECTION		
	{
		BHDR_FE_ChannelHandle hFeChannel ;
		uint32_t ulOffset ;
		
		uint8_t bPixelClockChange ;
		uint32_t PixelClockCount0 ;
		uint32_t newPixelClockCount = 0 ;

		/* HDMI_TODO get fe channel */
		hFeChannel = hFrontEnd->channel[0] ;	
		ulOffset = hFeChannel->ulOffset ;

		BDBG_WRN(("Manual Frequency Detection timer expired for Channel %d...",
			hFeChannel->eChannel)) ;

		/* workaround to fix intermittent loss of audio/video */
#if BHDR_CONFIG_FORCE_AV_RESTART
		hFeChannel->hHDR->bPacketErrors = true ;
#endif

		BHDR_FE_P_ReadPixelClock_isr(hFeChannel, 0, &PixelClockCount0, &bPixelClockChange) ; 
		if (PixelClockCount0)
			newPixelClockCount =  216000 * 2048 / PixelClockCount0 ;

		if (newPixelClockCount > 41000) 
		{
			BINT_EnableCallback_isr(hFeChannel->hCallback[MAKE_INTR_FE_CHN_ENUM(FREQ_CHANGE_0)]) ;
		}
		BINT_EnableCallback_isr(hFeChannel->hCallback[MAKE_INTR_FE_CHN_ENUM(CLOCK_STOP_0)] ) ;
		BINT_EnableCallback_isr(hFeChannel->hCallback[MAKE_INTR_FE_CHN_ENUM(PLL_UNLOCK_0)]) ;
	}
#endif

#if BHDR_CONFIG_MANUAL_PLL_RANGE_SETTING
	{
		BHDR_FE_ChannelHandle hFeChannel ;
		uint32_t ulOffset ;

		/* HDMI_TODO get fe channel */
		hFeChannel = hFrontEnd->channel[0] ;	
		ulOffset = hFeChannel->ulOffset ;

		BHDR_FE_P_AdjustFreqRange_isr(hFeChannel) ;
	}
#endif
		
		break ;

	case MAKE_INTR_FE_ENUM(TMR_IRQ_1) :
		/* REMOVE UNUSED TIMER */
		BDBG_ERR(("TIMER 1 Expired.....")) ;
		
		BDBG_ASSERT(false) ;
		
		break ;

	default	:
		BDBG_ERR(("Unknown Interrupt ID:%d", parm2)) ;
	} ;		

	/* L2 interrupts are reset automatically */
}
#endif


/******************************************************************************
void BHDR_FE_P_ClearHdcpAuthentication_isr
Summary: Clear internal HDCP engine
*******************************************************************************/
static void BHDR_FE_P_ClearHdcpAuthentication_isr(BHDR_FE_ChannelHandle hFeChannel) 
{
	BREG_Handle hRegister ; 
	uint32_t ulOffset, Register  ;
	
	BDBG_OBJECT_ASSERT(hFeChannel, BHDR_FE_P_ChannelHandle);	
	hRegister= hFeChannel->hRegister ;

	ulOffset = hFeChannel->ulHdrOffset ;

	/* clear HDCP authenticated status in HDMI Rx Core */
	Register = BREG_Read32(hRegister, BCHP_HDMI_RX_0_HDCP_DEBUG + ulOffset) ;

	Register &= ~ BCHP_MASK(HDMI_RX_0_HDCP_DEBUG, CLEAR_RX_AUTHENTICATED_P) ;
	BREG_Write32(hRegister, BCHP_HDMI_RX_0_HDCP_DEBUG + ulOffset , Register) ;
	
	Register |= BCHP_MASK(HDMI_RX_0_HDCP_DEBUG, CLEAR_RX_AUTHENTICATED_P) ; 
	BREG_Write32(hRegister, BCHP_HDMI_RX_0_HDCP_DEBUG  + ulOffset , Register) ;
}


/******************************************************************************
void BHDR_FE_P_ResetFeDataChannels_isr
Summary: Reset the analog front end channels
*******************************************************************************/
void BHDR_FE_P_ResetFeDataChannels_isr(BHDR_FE_ChannelHandle hFeChannel) 
{		
	uint32_t Register ;
	uint32_t ulOffset ;
	BREG_Handle  hRegister ;

	hRegister = hFeChannel->hRegister ;
	ulOffset = hFeChannel->ulOffset ;

#if BHDR_CONFIG_DEBUG_FRONT_END	
	BDBG_WRN(("FE_%d Reset Front End Channels...", hFeChannel->eChannel)) ;
#endif
	Register = BREG_Read32(hRegister, BCHP_HDMI_RX_FE_0_RESET_CONTROL + ulOffset) ;
	Register |= BCHP_MASK(HDMI_RX_FE_0_RESET_CONTROL, ANALOG_CHANNEL_2_RESET) ;
	Register |= BCHP_MASK(HDMI_RX_FE_0_RESET_CONTROL, ANALOG_CHANNEL_1_RESET) ;
	Register |= BCHP_MASK(HDMI_RX_FE_0_RESET_CONTROL, ANALOG_CHANNEL_0_RESET) ;
	BREG_Write32(hRegister, BCHP_HDMI_RX_FE_0_RESET_CONTROL + ulOffset, Register) ;

	Register &= ~ BCHP_MASK(HDMI_RX_FE_0_RESET_CONTROL, ANALOG_CHANNEL_2_RESET) ;
	Register &= ~ BCHP_MASK(HDMI_RX_FE_0_RESET_CONTROL, ANALOG_CHANNEL_1_RESET) ;
	Register &= ~ BCHP_MASK(HDMI_RX_FE_0_RESET_CONTROL, ANALOG_CHANNEL_0_RESET) ;
	BREG_Write32(hRegister, BCHP_HDMI_RX_FE_0_RESET_CONTROL + ulOffset, Register) ;
}

 
/******************************************************************************
 void BHDR_FE_P_ResetFeClockChannel_isr
 Summary: Reset the analog front end channels
 *******************************************************************************/
 void BHDR_FE_P_ResetFeClockChannel_isr(BHDR_FE_ChannelHandle hFeChannel) 
 {		 
	 uint32_t Register ;
	 uint32_t ulOffset ;
	 BREG_Handle  hRegister ;
 
	 hRegister = hFeChannel->hRegister ;
	 ulOffset = hFeChannel->ulOffset ;
 
#if BHDR_CONFIG_DEBUG_FRONT_END	
	 BDBG_WRN(("FE_%d Reset Front End Clock Channel...", hFeChannel->eChannel)) ;
#endif

	 Register = BREG_Read32(hRegister, BCHP_HDMI_RX_FE_0_RESET_CONTROL + ulOffset) ;
	 Register |= BCHP_MASK(HDMI_RX_FE_0_RESET_CONTROL, ANALOG_CHANNEL_CLOCK_RESET) ;
	 BREG_Write32(hRegister, BCHP_HDMI_RX_FE_0_RESET_CONTROL + ulOffset, Register) ;
	 
	 Register &= ~ BCHP_MASK(HDMI_RX_FE_0_RESET_CONTROL, ANALOG_CHANNEL_CLOCK_RESET) ;
	 BREG_Write32(hRegister, BCHP_HDMI_RX_FE_0_RESET_CONTROL + ulOffset, Register) ;
 }
 

static void BHDR_FE_P_FireHotPlugCb_isr(BHDR_FE_ChannelHandle hFeChannel)
{
#if BHDR_CONFIG_DEBUG_FRONT_END
	BDBG_WRN(("FE_%d RX HOT PLUG update (HPD) : %s ", 
		hFeChannel->eChannel, 
		hFeChannel->bTxDeviceAttached ? "HIGH" : "LOW")) ;
#endif
	
	/* inform higher level of Connect/Disconnect interrupt */
	if (hFeChannel->pfHotPlugCallback_isr)
	{
		hFeChannel->pfHotPlugCallback_isr(hFeChannel->pvHotPlugParm1, 
			hFeChannel->iHotPlugParm2, &hFeChannel->bTxDeviceAttached) ;
	}
	else
	{
		BDBG_WRN(("FE_%d No HotPlug callback installed...",
			hFeChannel->eChannel)) ;
	}

	/* clear HDCP authenticated status in HDMI Rx Core */
	BHDR_FE_P_ClearHdcpAuthentication_isr(hFeChannel) ;

}

  
/******************************************************************************
void BHDR_FE_P_Channel_isr
Summary: Handle interrupts from the HDMI core.
*******************************************************************************/
void BHDR_FE_P_Channel_isr(
	void *pParam1,						/* [in] Device handle */
	int parm2							/* [in] Interrupt ID */
)
{
	BHDR_FE_ChannelHandle hFeChannel  ;
	BREG_Handle hRegister ;
#if ! BHDR_CONFIG_DUAL_HPD_SUPPORT 
	uint32_t Register ;
#endif
	uint32_t ulOffset   ;
	bool bCheckPixelCountChange ;
	bool bPllLocked;

	BDBG_ENTER(BHDR_FE_P_Channel_isr) ;
	hFeChannel = (BHDR_FE_ChannelHandle) pParam1 ;
	BDBG_OBJECT_ASSERT(hFeChannel, BHDR_FE_P_ChannelHandle) ;

	if (hFeChannel->uiHdrSel == BHDR_P_eHdrCoreIdNotAttached)  
	{
#if BHDR_CONFIG_DEBUG_FRONT_END	
		BDBG_WRN(("No HDMI Rx core (not ready) attached to frontend")) ;
#endif
		return ;
	}

	hRegister = hFeChannel->hRegister ;

	/* get offset for Front End */
	ulOffset = hFeChannel->ulOffset ;
	bCheckPixelCountChange = false ;

	switch (parm2)
	{
#if BHDR_CONFIG_DUAL_HPD_SUPPORT 
	case MAKE_INTR_FE_CHN_ENUM(HPD_CONNECTED) : 

		hFeChannel->bTxDeviceAttached = true ;
		BHDR_FE_P_FireHotPlugCb_isr(hFeChannel) ;
		
		bCheckPixelCountChange = true ;
		break ;
		
	case MAKE_INTR_FE_CHN_ENUM(HPD_REMOVED) : 
		
		hFeChannel->bTxDeviceAttached = false  ;
		BHDR_FE_P_FireHotPlugCb_isr(hFeChannel) ;

		bCheckPixelCountChange = true ;
		break ;
		
#else		
	case MAKE_INTR_FE_CHN_ENUM(RX_HOTPLUG_UPDATE) : 
		Register = BREG_Read32( hRegister, BCHP_HDMI_RX_FE_0_HOTPLUG_STATUS + ulOffset) ;
		hFeChannel->bTxDeviceAttached = BCHP_GET_FIELD_DATA(Register, 
			HDMI_RX_FE_0_HOTPLUG_STATUS, RX_HOTPLUG_IN) ;

		BHDR_FE_P_FireHotPlugCb_isr(hFeChannel) ;

		bCheckPixelCountChange = true ;
		break ;
#endif

			
	case MAKE_INTR_FE_CHN_ENUM(PLL_UNLOCK_0) :
#if BHDR_CONFIG_DEBUG_FRONT_END	
		BDBG_WRN(("FE_%d   PLL -_-UNLOCKED-_-", hFeChannel->eChannel)) ;
#endif
		BHDR_FE_P_GetPllLockStatus_isr(hFeChannel, &bPllLocked) ;

#if BHDR_CONFIG_RESET_PIXEL_CLOCK_ESTIMATION
#if BHDR_CONFIG_DEBUG_FRONT_END	
		BDBG_WRN(("FE_%d   Reset CLK count ", hFeChannel->eChannel)) ;
#endif
		BHDR_FE_P_ResetPixelClockEstimation_isr(hFeChannel) ;
#endif
		bCheckPixelCountChange = false ;
		break ;
		
	case MAKE_INTR_FE_CHN_ENUM(PLL_LOCK_0) : 

#if BHDR_CONFIG_DEBUG_FRONT_END	
		BDBG_WRN(("FE_%d   PLL ----LOCKED----", hFeChannel->eChannel)) ;
#endif
		BHDR_FE_P_GetPllLockStatus_isr(hFeChannel, &bPllLocked) ;

#if BHDR_CONFIG_RESET_CLOCK_AND_DATA_CHANNELS
		/* enable the count down timer if not already enabled */
	 	BHDR_FE_P_ResetFeDataChannels_isr(hFeChannel) ;
		BHDR_FE_P_ResetFeClockChannel_isr(hFeChannel) ;
#endif

#if BHDR_CONFIG_MANUAL_PLL_RANGE_SETTING
		BHDR_FE_P_AdjustFreqRange_isr(hFeChannel) ;
#endif
		bCheckPixelCountChange = true ;

 		break ;

	case MAKE_INTR_FE_CHN_ENUM(FREQ_CHANGE_0) :
		hFeChannel->PreviousPixelClockCount = 0 ;
#if BHDR_CONFIG_DEBUG_FRONT_END	
		BDBG_WRN(("FE_%d Frequency Changed", hFeChannel->eChannel)) ;
#endif

#if BHDR_CONFIG_MANUAL_FREQUENCY_DETECTION
		BINT_DisableCallback_isr(hFeChannel->hCallback[MAKE_INTR_FE_CHN_ENUM(FREQ_CHANGE_0)]) ;
		BINT_DisableCallback_isr(hFeChannel->hCallback[MAKE_INTR_FE_CHN_ENUM(CLOCK_STOP_0)]) ;
		BINT_DisableCallback_isr(hFeChannel->hCallback[MAKE_INTR_FE_CHN_ENUM(PLL_UNLOCK_0)]) ;

#define BHDR_P_FREQ_CHANGE_TIMER_MS 300
		BTMR_ReadTimer_isr(hFeChannel->hFeTimer, &Register) ;
		if (Register)
		{
#if BHDR_CONFIG_DEBUG_FRONT_END	
			BDBG_WRN(("CH%d TIMER 0 is already running %dms left .....!!!!!!!!!!!", 
				hFeChannel->eChannel, Register / BHDR_P_MILLISECOND)) ;
#endif
			break ;		
		}

#if BHDR_CONFIG_DEBUG_FRONT_END	
		BDBG_WRN(("%d TIMER 0 Started... (Frequency Change) for %d microseconds\n", 
			__LINE__, BHDR_P_FREQ_CHANGE_TIMER_MS)) ;
#endif


		BTMR_StartTimer_isr(hFeChannel->hFeTimer, 
			BHDR_P_FREQ_CHANGE_TIMER_MS * BHDR_P_MILLISECOND) ;
#endif		
		break ;


	case MAKE_INTR_FE_CHN_ENUM(CLOCK_STOP_0) :
#if BHDR_CONFIG_DEBUG_FRONT_END	
		BDBG_WRN(("FE_%d Clock STOPPED", hFeChannel->eChannel)) ;
#endif
		/* force pixel count detection */
		bCheckPixelCountChange = true ;
		break ;


	default	:
		bCheckPixelCountChange = false ;		
		BDBG_ERR(("Unknown Interrupt ID:%d", parm2)) ;
	} ;		

#if BHDR_CONFIG_MANUAL_FREQUENCY_DETECTION
	/* re-enable interrupts when pixel clock changes */
	if (bCheckPixelCountChange)
	{
		uint8_t bPixelClockChange ;
		uint32_t PixelClockCount ;
		
		BHDR_FE_P_ReadPixelClock_isr(hFeChannel, 0, &PixelClockCount, &bPixelClockChange)  ;
		if (bPixelClockChange)
			BINT_EnableCallback_isr(hFeChannel->hCallback[MAKE_INTR_FE_CHN_ENUM(FREQ_CHANGE_0)]) ;
	}	
#endif	
	/* L2 interrupts are reset automatically */
	BDBG_LEAVE(BHDR_FE_P_Channel_isr) ;
}



/******************************************************************************
Summary: Attach the HDMI Rx Device to a Front End Channel 
*******************************************************************************/
BERR_Code BHDR_FE_AttachHdmiRxCore(
	BHDR_FE_ChannelHandle hFeChannel,
	BHDR_Handle hHDR)
{
	
	BERR_Code rc = BERR_SUCCESS ;
	BHDR_FE_P_Channel eNewFeChannel ;

	BHDR_P_HdrCoreId eCoreId ;
	

	BDBG_ENTER(BHDR_FE_AttachHdmiRxCore) ;
	
	BDBG_OBJECT_ASSERT(hHDR, BHDR_P_Handle) ;
	BDBG_OBJECT_ASSERT(hFeChannel, BHDR_FE_P_ChannelHandle) ;
#if BHDR_CONFIG_DEBUG_DISPLAY_FE_CONFIG
	BDBG_WRN(("Attach HDMI_RX_FE_%d =====> HDMI_RX_%d ", 
		hFeChannel->eChannel, hHDR->eCoreId)) ; 
#endif

	/* verify specified channel does not exceed max channels */
	eNewFeChannel = hFeChannel->eChannel ;
	if (eNewFeChannel >= BHDR_FE_P_eChannelMax)
	{
		rc = BERR_INVALID_PARAMETER ;
		goto done ;
	}
	
	eCoreId = hHDR->eCoreId ;
	if (eCoreId >= BHDR_P_eHdrCoreIdMax)
	{
		BDBG_ERR(("Unknown HDMI Rx Core ID %d", eCoreId)) ;	
		goto done ;
	}


	/* assign new FrontEnd to HDMI Rx core */
	hHDR->hFeChannel = (BHDR_FE_ChannelHandle) hFeChannel ;

	/* remember FE Channel's assigned core */
	hFeChannel->uiHdrSel = eCoreId ;
	
	/* also remember the HDR register offset for updating HDR registers */
	hFeChannel->ulHdrOffset = hHDR->ulOffset ;

	hFeChannel->hHDR = hHDR ;

	/* Hot Plug now pulsed at higher level API */
	
	BKNI_EnterCriticalSection() ;
		BHDR_P_HotPlugConnect_isr(hHDR);
		
		if (!hFeChannel->settings.bHpdDisconnected)
		{
			BHDR_FE_P_EnableInterrupts_isr(hFeChannel, true) ;
			BHDR_P_EnableInterrupts_isr(hHDR, true) ;
		}

		BHDR_P_ResetHdcp_isr(hHDR) ;
				
		BHDR_FE_P_ResetFeDataChannels_isr(hFeChannel) ;

	BKNI_LeaveCriticalSection() ;	

done:	
	BDBG_LEAVE(BHDR_FE_AttachHdmiRxCore) ;
	return rc ;
} 


/******************************************************************************
Summary: Detach the HDMI Rx Device from the Front End Channel 
*******************************************************************************/
BERR_Code BHDR_FE_DetachHdmiRxCore(
	BHDR_FE_ChannelHandle hFeChannel, BHDR_Handle hHDR) 
{
	BERR_Code rc = BERR_SUCCESS ;
	
	BDBG_ENTER(BHDR_FE_DetachHdmiRxCore) ;
	BDBG_OBJECT_ASSERT(hFeChannel, BHDR_FE_P_ChannelHandle) ;
	BDBG_OBJECT_ASSERT(hHDR, BHDR_P_Handle) ;
	
	/* forget  FE Channel's assigned core */
	hFeChannel->uiHdrSel = BHDR_P_eHdrCoreIdNotAttached ;
	
	BDBG_LEAVE(BHDR_FE_DetachHdmiRxCore) ;
	return rc ;
} 



/******************************************************************************
Summary: Get the Front End Channel attached to the HDMI Rx Core  
*******************************************************************************/
BERR_Code BHDR_FE_GetAttachedHdmiRxCore(
	BHDR_FE_ChannelHandle hFeChannel,
	uint8_t *uiHdrSel)  
{
	BERR_Code rc = BERR_SUCCESS ;
	
	BDBG_ENTER(BHDR_FE_GetAttachedHdmiRxCore) ;
	BDBG_OBJECT_ASSERT(hFeChannel, BHDR_FE_P_ChannelHandle) ;

	*uiHdrSel = hFeChannel->uiHdrSel ;
	
	BDBG_LEAVE(BHDR_FE_GetAttachedHdmiRxCore) ;
	return rc ;
} 



/******************************************************************************
Summary: Enable/Disable Equalization 
*******************************************************************************/
BERR_Code BHDR_FE_EnableEqualization(BHDR_FE_ChannelHandle hFeChannel, bool bEnable) 
{
	BERR_Code rc = BERR_SUCCESS ;
	BREG_Handle hRegister ;
	uint32_t ulOffset  ;
	uint32_t Register ;
	
	BDBG_OBJECT_ASSERT(hFeChannel, BHDR_FE_P_ChannelHandle) ;
	
	hRegister = hFeChannel->hRegister ;
	ulOffset = hFeChannel->ulOffset ;
	
	if (!bEnable)
	{
#if BHDR_CONFIG_LOOP_FILTER_CONTROL
		Register = BREG_Read32(hRegister, BCHP_HDMI_RX_FE_0_LOOP_FILT_CTRL + ulOffset) ;
		Register &= ~ BCHP_MASK(HDMI_RX_FE_0_LOOP_FILT_CTRL, aeq_enable) ;
		Register |=  BCHP_MASK(HDMI_RX_FE_0_LOOP_FILT_CTRL, aeq_freeze) ;
		BREG_Write32(hRegister, BCHP_HDMI_RX_FE_0_LOOP_FILT_CTRL + ulOffset, Register) ;
#endif
		BDBG_WRN(("HDMI_TODO  Do we need to enabe/disable Equalization?")) ;
		return rc ;
	}
	else
	{
#if BHDR_CONFIG_LOOP_FILTER_CONTROL
		Register = BREG_Read32(hRegister, BCHP_HDMI_RX_FE_0_LOOP_FILT_CTRL + ulOffset) ;
		Register |=  BCHP_MASK(HDMI_RX_FE_0_LOOP_FILT_CTRL, aeq_enable) ;
		Register &= ~ BCHP_MASK(HDMI_RX_FE_0_LOOP_FILT_CTRL, aeq_freeze) ;
		BREG_Write32(hRegister, BCHP_HDMI_RX_FE_0_LOOP_FILT_CTRL + ulOffset, Register) ;
#endif
	}


	Register = BREG_Read32(hRegister, BCHP_HDMI_RX_EQ_0_AEQ_CONTROL1_CH0+ ulOffset) ;
	Register |= BCHP_FIELD_DATA(HDMI_RX_EQ_0_AEQ_CONTROL1_CH0, 	INVERT_OS, 1) ;
	BREG_Write32(hRegister, BCHP_HDMI_RX_EQ_0_AEQ_CONTROL1_CH0 + ulOffset, Register) ;	
	
	Register = BREG_Read32(hRegister, BCHP_HDMI_RX_EQ_0_AEQ_CONTROL1_CH1+ ulOffset) ;
	Register |= BCHP_FIELD_DATA(HDMI_RX_EQ_0_AEQ_CONTROL1_CH1, 	INVERT_OS, 1) ;
	BREG_Write32(hRegister, BCHP_HDMI_RX_EQ_0_AEQ_CONTROL1_CH1 + ulOffset, Register) ;	
	
	Register = BREG_Read32(hRegister, BCHP_HDMI_RX_EQ_0_AEQ_CONTROL1_CH2+ ulOffset) ;
	Register |= BCHP_FIELD_DATA(HDMI_RX_EQ_0_AEQ_CONTROL1_CH2, 	INVERT_OS, 1) ;
	BREG_Write32(hRegister, BCHP_HDMI_RX_EQ_0_AEQ_CONTROL1_CH2 + ulOffset, Register) ;	
	
	Register = BREG_Read32(hRegister, BCHP_HDMI_RX_EQ_0_AEQ_CONTROL1_CH3+ ulOffset) ;
	Register |= BCHP_FIELD_DATA(HDMI_RX_EQ_0_AEQ_CONTROL1_CH3, 	INVERT_OS, 1) ;
	BREG_Write32(hRegister, BCHP_HDMI_RX_EQ_0_AEQ_CONTROL1_CH3 + ulOffset, Register) ;	
	
	Register = 0x989c ;
	BREG_Write32(hRegister, BCHP_HDMI_RX_EQ_0_AEQ_CONTROL2_CH0 + ulOffset, Register) ;	
	BREG_Write32(hRegister, BCHP_HDMI_RX_EQ_0_AEQ_CONTROL2_CH1 + ulOffset, Register) ;	
	BREG_Write32(hRegister, BCHP_HDMI_RX_EQ_0_AEQ_CONTROL2_CH2 + ulOffset, Register) ;	
	BREG_Write32(hRegister, BCHP_HDMI_RX_EQ_0_AEQ_CONTROL2_CH3 + ulOffset, Register) ;	
	
	Register = BREG_Read32(hRegister, BCHP_HDMI_RX_EQ_0_RX_HSPMD_CONTROL1_CH0+ ulOffset) ;
	Register |= BCHP_FIELD_DATA(HDMI_RX_EQ_0_RX_HSPMD_CONTROL1_CH0, 	RX_PHACT_8B4BN, 1) ;
	BREG_Write32(hRegister, BCHP_HDMI_RX_EQ_0_RX_HSPMD_CONTROL1_CH0 + ulOffset, Register) ;	
	
	Register = BREG_Read32(hRegister, BCHP_HDMI_RX_EQ_0_RX_HSPMD_CONTROL1_CH1+ ulOffset) ;
	Register |= BCHP_FIELD_DATA(HDMI_RX_EQ_0_RX_HSPMD_CONTROL1_CH1, 	RX_PHACT_8B4BN, 1) ;
	BREG_Write32(hRegister, BCHP_HDMI_RX_EQ_0_RX_HSPMD_CONTROL1_CH1 + ulOffset, Register) ;	
	
	Register = BREG_Read32(hRegister, BCHP_HDMI_RX_EQ_0_RX_HSPMD_CONTROL1_CH2+ ulOffset) ;
	Register |= BCHP_FIELD_DATA(HDMI_RX_EQ_0_RX_HSPMD_CONTROL1_CH2, 	RX_PHACT_8B4BN, 1) ;
	BREG_Write32(hRegister, BCHP_HDMI_RX_EQ_0_RX_HSPMD_CONTROL1_CH2 + ulOffset, Register) ;	
	
	Register = BREG_Read32(hRegister, BCHP_HDMI_RX_EQ_0_RX_HSPMD_CONTROL1_CH3+ ulOffset) ;
	Register |= BCHP_FIELD_DATA(HDMI_RX_EQ_0_RX_HSPMD_CONTROL1_CH3, 	RX_PHACT_8B4BN, 1) ;
	BREG_Write32(hRegister, BCHP_HDMI_RX_EQ_0_RX_HSPMD_CONTROL1_CH3 + ulOffset, Register) ;	
	
	Register = BREG_Read32(hRegister, BCHP_HDMI_RX_EQ_0_AEQ_CONTROL1_CH0+ ulOffset) ;
	Register |= BCHP_FIELD_DATA(HDMI_RX_EQ_0_AEQ_CONTROL1_CH0, 	ENABLE, 1) ;
	BREG_Write32(hRegister, BCHP_HDMI_RX_EQ_0_AEQ_CONTROL1_CH0 + ulOffset, Register) ;	
	
	Register = BREG_Read32(hRegister, BCHP_HDMI_RX_EQ_0_AEQ_CONTROL1_CH1+ ulOffset) ;
	Register |= BCHP_FIELD_DATA(HDMI_RX_EQ_0_AEQ_CONTROL1_CH1, 	ENABLE, 1) ;
	BREG_Write32(hRegister, BCHP_HDMI_RX_EQ_0_AEQ_CONTROL1_CH1 + ulOffset, Register) ;	
	
	Register = BREG_Read32(hRegister, BCHP_HDMI_RX_EQ_0_AEQ_CONTROL1_CH2+ ulOffset) ;
	Register |= BCHP_FIELD_DATA(HDMI_RX_EQ_0_AEQ_CONTROL1_CH2, 	ENABLE, 1) ;
	BREG_Write32(hRegister, BCHP_HDMI_RX_EQ_0_AEQ_CONTROL1_CH2 + ulOffset, Register) ;	
	
	Register = BREG_Read32(hRegister, BCHP_HDMI_RX_EQ_0_AEQ_CONTROL1_CH3+ ulOffset) ;
	Register |= BCHP_FIELD_DATA(HDMI_RX_EQ_0_AEQ_CONTROL1_CH3, 	ENABLE, 1) ;
	BREG_Write32(hRegister, BCHP_HDMI_RX_EQ_0_AEQ_CONTROL1_CH3 + ulOffset, Register) ;

	BKNI_EnterCriticalSection() ;
		BHDR_FE_P_ResetFeClockChannel_isr(hFeChannel) ;
	BKNI_LeaveCriticalSection() ;
	
	return rc ;

}


/**************************************************************************
Summary: Register a callback function to be called when a Hot Plug event occurs
**************************************************************************/
BERR_Code BHDR_FE_InstallHotPlugCallback( 
	BHDR_FE_ChannelHandle hFeChannel,			/* [in] HDMI Front End Handle */
	const BHDR_FE_CallbackFunc pfCallback_isr, /* [in] cb for notification */
	void *pvParm1,  /* [in] the first argument (void *) passed to the callback function */ 
	int iParm2      /* [in] the second argument(int) passed to the callback function */ 
) 
{
	BERR_Code rc = BERR_SUCCESS ;
	BDBG_ENTER(BHDR_FE_InstallHotPlugCallback);
	BDBG_OBJECT_ASSERT(hFeChannel, BHDR_FE_P_ChannelHandle) ;
	
	if (hFeChannel->pfHotPlugCallback_isr)
	{
		BDBG_ERR(("Callback handler already installed for Hot Plug...overriding")) ;
	}

	BKNI_EnterCriticalSection() ;
		hFeChannel->pfHotPlugCallback_isr = pfCallback_isr ;
		hFeChannel->pvHotPlugParm1 = pvParm1 ;
		hFeChannel->iHotPlugParm2 = iParm2 ;
	BKNI_LeaveCriticalSection() ;

	BDBG_LEAVE(BHDR_FE_InstallHotPlugCallback) ;
	return rc ;
}



/**************************************************************************
Summary:Remove a previously registered callback function for Hot Plug events
**************************************************************************/
BERR_Code BHDR_FE_UnInstallHotPlugCallback( 
	BHDR_FE_ChannelHandle hFeChannel,                       /* [in] HDMI Rx Frontend Channel Handle */
	const BHDR_FE_CallbackFunc pfCallback_isr /* [in] cb for Hot Plug notification  */
) 
{
	BERR_Code rc = BERR_SUCCESS ;
	BDBG_ENTER(BHDR_FE_UnInstallHotPlugCallback);
	BDBG_OBJECT_ASSERT(hFeChannel, BHDR_FE_P_ChannelHandle) ;
	
	BSTD_UNUSED(pfCallback_isr) ;

	if (hFeChannel->pfHotPlugCallback_isr == (BHDR_CallbackFunc) NULL)
	{
		BDBG_WRN(("No callback handler to uninstall for HotPlug callback")) ;
		goto done ;
	}

	BKNI_EnterCriticalSection() ;
		hFeChannel->pfHotPlugCallback_isr = (BHDR_CallbackFunc) NULL ;
		hFeChannel->pvHotPlugParm1 = (void *) NULL  ;
		hFeChannel->iHotPlugParm2 = 0  ;
	BKNI_LeaveCriticalSection() ;

done :	
	BDBG_LEAVE(BHDR_FE_UnInstallHotPlugCallback) ;
	return rc ;
}


/**************************************************************************
Summary: Power Down Core Sequence
**************************************************************************/
void BHDR_FE_P_PowerDownCore(
	BHDR_FE_ChannelHandle hFeChannel) 
{
	BREG_Handle hRegister ;
		
	uint32_t ulOffset  ;
	
	BDBG_OBJECT_ASSERT(hFeChannel, BHDR_FE_P_ChannelHandle) ;
	hRegister = hFeChannel->hRegister ;
	/* get offset for Front End */
	ulOffset = hFeChannel->ulOffset ;
}


/**************************************************************************
Summary: Enable/Disable Frontend Interrupts
**************************************************************************/
void BHDR_FE_P_EnableInterrupts_isr(BHDR_FE_ChannelHandle hFeChannel, bool enable)
{
	BERR_Code rc  ;
	uint32_t ulOffset  ;
	uint8_t i ;

	BDBG_ENTER(BHDR_FE_P_EnableInterrupts_isr) ;

	/* do not enable/disable interrupts unless HPD signal is disconnected  */
	if (!hFeChannel->settings.bHpdDisconnected)
		return ;

	/* get offset for Front End */
	ulOffset = hFeChannel->ulOffset ;
	if (hFeChannel->eChannel == BHDR_FE_P_eChannel0)
	{
		pInterrupts = BHDR_FE_P_ChannelIntr0 ;
	}

	for (i = 0; i < MAKE_INTR_FE_CHN_ENUM(LAST) ; i++) 
	{
		/* clear interrupt callback */
		rc =  BINT_ClearCallback_isr( hFeChannel->hCallback[i]) ;

		/* skip interrupt if not enabled in table...  */	
		if (!pInterrupts[i].enable)
			continue ;

 		if (enable)
			BINT_EnableCallback_isr( hFeChannel->hCallback[i] ) ;

	   /* never disable hot plug interrupt */
#if BHDR_CONFIG_DUAL_HPD_SUPPORT
		else if ((i != MAKE_INTR_FE_CHN_ENUM(HPD_CONNECTED))
		&&		 (i != MAKE_INTR_FE_CHN_ENUM(HPD_REMOVED)))
#else
		else if (i != MAKE_INTR_FE_CHN_ENUM(RX_HOTPLUG_UPDATE))
#endif
		{
			BINT_DisableCallback_isr( hFeChannel->hCallback[i] ) ;
			BDBG_WRN(("Disable Interrupt %d; ", i)) ;
		}
		else
		{
			BDBG_WRN(("Keep Interrupt %d Enabled", i)) ; 
		}
	}
	BDBG_LEAVE(BHDR_FE_P_EnableInterrupts_isr) ;
}


/**************************************************************************
Summary:Set Power State  
**************************************************************************/
BERR_Code BHDR_FE_SetPowerState(
	BHDR_FE_ChannelHandle hFeChannel, BHDR_FE_ChannelPowerSettings * stPowerSettings) 
{

	BDBG_ENTER(BHDR_FE_SetPowerState);
	BDBG_OBJECT_ASSERT(hFeChannel, BHDR_FE_P_ChannelHandle) ;

#if BCHP_PWR_SUPPORT
	if (stPowerSettings->bHdmiRxPowered )
	{
	    BHDR_FE_P_PowerResourceAcquire_HDMI_RX_FE(hFeChannel) ;
	} 
	else 
	{
	    BHDR_FE_P_PowerResourceRelease_HDMI_RX_FE(hFeChannel);
	}
#else
	BDBG_WRN(("BCHP_PWR_SUPPORT not enabled/available")) ;
#endif

	BKNI_Memcpy(&hFeChannel->stPowerSettings, stPowerSettings, 
		    sizeof(BHDR_FE_ChannelPowerSettings)) ;

	BDBG_LEAVE(BHDR_FE_SetPowerState) ;
		
	return BERR_SUCCESS ;
}


/**************************************************************************
Summary:Get Power State  
**************************************************************************/
BERR_Code BHDR_FE_GetPowerState(
	BHDR_FE_ChannelHandle hFeChannel, BHDR_FE_ChannelPowerSettings * stPowerSettings) 
{
	BERR_Code rc = BERR_SUCCESS ;

	BDBG_ENTER(BHDR_FE_GetPowerState);
	BDBG_OBJECT_ASSERT(hFeChannel, BHDR_FE_P_ChannelHandle) ;	

	BKNI_Memcpy(stPowerSettings, &hFeChannel->stPowerSettings,
		sizeof(BHDR_FE_ChannelPowerSettings)) ;

	BDBG_LEAVE(BHDR_FE_GetPowerState) ;
	return rc ;
}


#if BHDR_CONFIG_DEBUG_EYE_DIAGRAM 
BERR_Code BHDR_FE_DEBUG_GetEyeDiagramData(BHDR_Handle hHDR, uint8_t *padc_data) 
{
	uint32_t rc = BERR_SUCCESS ;
#if HDMI_RX_GEN == 3548
	BREG_Handle hRegister ;
	uint32_t Register ;
	uint32_t ulOffset ;
	uint32_t status;
	uint8_t adc_clk_dly;
	uint16_t i, j;
	uint16_t colcount = 2048;
	uint8_t adc_sample_num = 6;
	uint32_t adc_raw_data[4][2048];

	hRegister = hHDR->hRegister ;
	
	BDBG_OBJECT_ASSERT (hHDR, BHDR_P_Handle) ;
	BDBG_OBJECT_ASSERT(hHDR->hFeChannel, BHDR_FE_P_ChannelHandle) ;

	/* get offset for Front End */
	ulOffset = hHDR->hFeChannel->ulOffset ;
	
	/* Freeze the equalizer by zeroing out the phase error */
	Register = BREG_Read32(hRegister, BCHP_HDMI_RX_FE_0_DIGITAL_LOOP_FILTER_CTL_1 + ulOffset) ;
	Register |= BCHP_FIELD_DATA(HDMI_RX_FE_0_DIGITAL_LOOP_FILTER_CTL_1, LOOP_FILTER_PHASE_ERROR_ZERO, 1) ;
	BREG_Write32(hRegister, BCHP_HDMI_RX_FE_0_DIGITAL_LOOP_FILTER_CTL_1 + ulOffset, Register) ;

	/*  set the ADC delay to 0 */
	Register = BREG_Read32(hRegister, BCHP_HDMI_RX_FE_0_PLL_CTRL_1 + ulOffset) ;
	adc_clk_dly = (uint8_t) BCHP_GET_FIELD_DATA(Register, 
			HDMI_RX_FE_0_PLL_CTRL_1, ADCCLK_DLY) ;
	Register &= ~BCHP_MASK(HDMI_RX_FE_0_PLL_CTRL_1, ADCCLK_DLY) ;
	BREG_Write32(hRegister, BCHP_HDMI_RX_FE_0_PLL_CTRL_1 + ulOffset, Register) ;

	/* Capture the data */
	for(i = 0; i < colcount; i++)	
	{ 
		/* Start ADC capture and wait til done. */
		status = 0;

		Register = BREG_Read32(hRegister, BCHP_HDMI_RX_FE_0_ANALOG_CFG_3 + ulOffset) ;
		Register |= BCHP_FIELD_DATA(HDMI_RX_FE_0_ANALOG_CFG_3, ADC_CAPTURE_BEGIN, 1) ;
		BREG_Write32(hRegister, BCHP_HDMI_RX_FE_0_ANALOG_CFG_3 + ulOffset, Register) ;

		Register &= ~BCHP_MASK(HDMI_RX_FE_0_ANALOG_CFG_3, ADC_CAPTURE_BEGIN) ;
		BREG_Write32(hRegister, BCHP_HDMI_RX_FE_0_ANALOG_CFG_3 + ulOffset, Register) ;

		while(status == 0)
		{
			Register = BREG_Read32(hRegister, BCHP_HDMI_RX_FE_0_ANALOG_ADC_READBACK_0 + ulOffset) ;
			status = Register & BCHP_MASK(HDMI_RX_FE_0_ANALOG_ADC_READBACK_0, ADC_CAPTURE_DONE);
			BKNI_Delay(100) ;		
		}

		/* Read back captured data. */
		adc_raw_data[0][i] = 0x7FFFFFFF & BREG_Read32(hRegister, BCHP_HDMI_RX_FE_0_ANALOG_ADC_READBACK_0 + ulOffset);
		adc_raw_data[1][i] = 0x7FFFFFFF & BREG_Read32(hRegister, BCHP_HDMI_RX_FE_0_ANALOG_ADC_READBACK_1 + ulOffset);
		adc_raw_data[2][i] = 0x7FFFFFFF & BREG_Read32(hRegister, BCHP_HDMI_RX_FE_0_ANALOG_ADC_READBACK_2 + ulOffset);
		adc_raw_data[3][i] = 0x7FFFFFFF & BREG_Read32(hRegister, BCHP_HDMI_RX_FE_0_ANALOG_ADC_READBACK_3 + ulOffset);

		/* Rotate to the next point */
		Register = BREG_Read32(hRegister, BCHP_HDMI_RX_FE_0_PLL_CTRL_1 + ulOffset) ;
		Register &= ~BCHP_MASK(HDMI_RX_FE_0_PLL_CTRL_1, ADCCLK_DLY) ;
		Register |= BCHP_FIELD_DATA(HDMI_RX_FE_0_PLL_CTRL_1, ADCCLK_DLY, i & 0x01F) ;
		BREG_Write32(hRegister, BCHP_HDMI_RX_FE_0_PLL_CTRL_1 + ulOffset, Register) ;
		
	}

	/* Thaw the control loop */
	Register = BREG_Read32(hRegister, BCHP_HDMI_RX_FE_0_LOOP_FILT_CTRL + ulOffset) ;
	Register |= 
		BCHP_FIELD_DATA(HDMI_RX_FE_0_LOOP_FILT_CTRL, bwsel_prop, 1) |
		BCHP_FIELD_DATA(HDMI_RX_FE_0_LOOP_FILT_CTRL, bwsel_integ, 1);
	BREG_Write32(hRegister, BCHP_HDMI_RX_FE_0_LOOP_FILT_CTRL + ulOffset, Register) ;

	Register = BREG_Read32(hRegister, BCHP_HDMI_RX_FE_0_PLL_CTRL_1 + ulOffset) ;
	Register &= ~BCHP_MASK(HDMI_RX_FE_0_PLL_CTRL_1, ADCCLK_DLY) ;
	Register |= BCHP_FIELD_DATA(HDMI_RX_FE_0_PLL_CTRL_1, ADCCLK_DLY, adc_clk_dly) ;
	BREG_Write32(hRegister, BCHP_HDMI_RX_FE_0_PLL_CTRL_1 + ulOffset, Register) ;

	/* Unfreeze the equalizer */
	Register = BREG_Read32(hRegister, BCHP_HDMI_RX_FE_0_DIGITAL_LOOP_FILTER_CTL_1 + ulOffset) ;
	Register &= ~BCHP_MASK(HDMI_RX_FE_0_DIGITAL_LOOP_FILTER_CTL_1, LOOP_FILTER_PHASE_ERROR_ZERO) ;
	BREG_Write32(hRegister, BCHP_HDMI_RX_FE_0_DIGITAL_LOOP_FILTER_CTL_1 + ulOffset, Register) ;

	/*Postprocess the captured data to break out the individual samples.*/
	for(i = 0; i < colcount; i++)
	{
		for(j = 0; j < 24; j++)
		{
			*(padc_data + j*colcount + i) = (uint8_t)((adc_raw_data[j/adc_sample_num][i] >> ((j % adc_sample_num) * 5)) & 0x01F);
		}		
	}
#else
	BSTD_UNUSED(hHDR) ;
	BSTD_UNUSED(padc_data) ;
#endif	
return rc ;
}
#endif

/* end of file */

