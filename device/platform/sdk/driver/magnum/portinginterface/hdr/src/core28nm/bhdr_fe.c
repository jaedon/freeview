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
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 9/24/12 5:48p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/hdr/src/core28nm/bhdr_fe.c $
 * 
 * Hydra_Software_Devel/2   9/24/12 5:48p rgreen
 * SW7445-24: Initial 28nm support for HDMI Rx Frontend
 * 
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
	/* 00 */   { BCHP_INT_ID_HP_CONNECTED_0, BHDR_FE_CHN_INTR_eHPD_CONNECTED, true},
	/* 01 */   { BCHP_INT_ID_HP_REMOVED_0, BHDR_FE_CHN_INTR_eHPD_REMOVED, true},
	/* 02 */   { BCHP_INT_ID_CLOCK_STOP_0, BHDR_FE_CHN_INTR_eCLOCK_STOP_0, true},
	/* 03 */   { BCHP_INT_ID_PLL_UNLOCK_0, BHDR_FE_CHN_INTR_ePLL_UNLOCK_0, true},
	/* 04 */   { BCHP_INT_ID_PLL_LOCK_0, BHDR_FE_CHN_INTR_ePLL_LOCK_0, true},
	/* 05 */   { BCHP_INT_ID_FREQ_CHANGE_0,  BHDR_FE_CHN_INTR_eFREQ_CHANGE_0, true},
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
	BDBG_WRN(("$brcm_Revision: Hydra_Software_Devel/2 $")) ;
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


	/* Take HDMI Rx Cores out of reset  */
	BHDR_FE_P_Initialize(hFrontEnd) ;
	
	Register = BREG_Read32(hRegister, BCHP_HDMI_RX_FE_0_ANALOG_CFG_3) ;
/*	Register &= ~ BCHP_MASK(HDMI_RX_FE_0_ANALOG_CFG_3, 	USE_CDR_LOCK) ; */
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

	BDBG_ENTER(BHDR_FE_Close) ;
	BDBG_OBJECT_ASSERT(hFrontEnd, BHDR_FE_P_Handle) ;

#if BHDR_CONFIG_DVP_HR_TMR 
	BTMR_Close(hFrontEnd->hTimerDevice) ;
#endif

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
		BDBG_WRN(("TIMER 0 (unused) Expired.....")) ;
		break ;

	case MAKE_INTR_FE_ENUM(TMR_IRQ_1) :
		/* REMOVE UNUSED TIMER */
		BDBG_WRN(("TIMER 1 (unused) Expired.....")) ;
		
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

		bCheckPixelCountChange = true ;

 		break ;

	case MAKE_INTR_FE_CHN_ENUM(FREQ_CHANGE_0) :
		hFeChannel->PreviousPixelClockCount = 0 ;
#if BHDR_CONFIG_DEBUG_FRONT_END	
		BDBG_WRN(("FE_%d Frequency Changed", hFeChannel->eChannel)) ;
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
		BDBG_WRN(("HDMI_TODO  Do we need to enabe/disable Equalization?")) ;
		return rc ;
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
		else if ((i != MAKE_INTR_FE_CHN_ENUM(HPD_CONNECTED))
		&&		 (i != MAKE_INTR_FE_CHN_ENUM(HPD_REMOVED)))
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

