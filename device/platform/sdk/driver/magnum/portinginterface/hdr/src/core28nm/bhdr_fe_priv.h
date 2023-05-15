/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bhdr_fe_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 9/24/12 5:48p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/hdr/src/core28nm/bhdr_fe_priv.h $
 * 
 * Hydra_Software_Devel/2   9/24/12 5:48p rgreen
 * SW7445-24: Initial 28nm support for HDMI Rx Frontend
 * 
 * 
 ***************************************************************************/


#ifndef BHDR_FE_PRIV_H__
#define BHDR_FE_PRIV_H__


#ifdef __cplusplus
extern "C" {
#endif

#include "bchp_common.h"
#include "bchp_sun_top_ctrl.h"

#include "bchp_hdmi_rx_fe_0.h"
#include "bchp_dvp_hr_intr2.h"
#include "bchp_int_id_dvp_hr_intr2.h"



#define	MAKE_INTR_FE_ENUM(IntName)	BHDR_FE_INTR_e##IntName
#define	MAKE_INTR_FE_NAME(IntName)	"BHDR_FE_" #IntName

#define	MAKE_INTR_FE_CHN_ENUM(IntName)	BHDR_FE_CHN_INTR_e##IntName
#define	MAKE_INTR_FE_CHN_NAME(IntName)	"BHDR_FE_CHN_" #IntName




typedef enum BHDR_FE_P_Channel
{
	BHDR_FE_P_eChannel0 = 0,
	BHDR_FE_P_eChannel1,
	BHDR_FE_P_eChannelMax
} BHDR_FE_P_Channel ;



/******************************************************************************
Summary:
Enumeration of BHDR_FE_Interrupts 
*******************************************************************************/
typedef enum
{
	 /* 16 */ MAKE_INTR_FE_ENUM(TMR_IRQ_0), 
	 /* 17 */ MAKE_INTR_FE_ENUM(TMR_IRQ_1),
	 
	/*  */ MAKE_INTR_FE_ENUM(LAST)
	 
} BHDR_FE_P_InterruptMask ;



/******************************************************************************
Summary:
Enumeration of BHDR_FE_ChnInterrupts 
*******************************************************************************/
typedef enum
{
	 /* 00 */ MAKE_INTR_FE_CHN_ENUM(HPD_CONNECTED),
	 /* 01 */ MAKE_INTR_FE_CHN_ENUM(HPD_REMOVED),
	 /* 02 */ MAKE_INTR_FE_CHN_ENUM(CLOCK_STOP_0),   
	 /* 03 */ MAKE_INTR_FE_CHN_ENUM(PLL_UNLOCK_0),   
	 /* 04 */ MAKE_INTR_FE_CHN_ENUM(PLL_LOCK_0),   
	 /* 05 */ MAKE_INTR_FE_CHN_ENUM(FREQ_CHANGE_0),   


	/*  */ MAKE_INTR_FE_CHN_ENUM(LAST)
	 
} BHDR_FE_P_ChnInterruptMask ;



/* Get the offset of two groups of register. */
/* HDMI TODO Fix for >2 FrontEnds */
#define BHDR_FE_P_GET_REG_OFFSET(ChannelId) \
	((BHDR_FE_P_eChannel0 == (ChannelId)) ? 0 : (BCHP_HDMI_RX_FE_1_REG_START - BCHP_HDMI_RX_FE_0_REG_START))


/* Pixel Clock Count <--> Rate (KHz) */
#define BHDR_FE_P_PIXEL_CLOCK_COUNT_TO_RATE(PixelClockCount)   \
        ((108000 * 8192) / PixelClockCount)


#define BHDR_FE_P_PIXEL_CLOCK_RATE_TO_COUNT BHDR_FE_P_PIXEL_CLOCK_COUNT_TO_RATE

/*******************************************************************************
Private HDMI Rx Frontend Channel Handle Declaration 
*******************************************************************************/
BDBG_OBJECT_ID_DECLARE(BHDR_FE_P_ChannelHandle);
typedef struct BHDR_FE_P_ChannelHandle
{
	BDBG_OBJECT(BHDR_FE_P_ChannelHandle)
	BINT_Handle   hInterrupt ;
	BREG_Handle hRegister ;
	BCHP_Handle hChip ;
	
	BHDR_FE_P_Channel    eChannel  ;
	uint32_t                       ulOffset ;

	BHDR_FE_ChannelSettings settings ;
	
	bool bTxDeviceAttached ;
	bool bPreviousTxDeviceAttached ;
	bool bPllLocked ;

	BHDR_Handle hHDR ;
	uint8_t uiHdrSel ;
	uint32_t ulHdrOffset ;

	BINT_CallbackHandle hCallback[MAKE_INTR_FE_CHN_ENUM(LAST)] ;
	
	BHDR_FE_CallbackFunc pfHotPlugCallback_isr ;
	void                       *pvHotPlugParm1;
	int                         iHotPlugParm2;
	
	
	uint32_t PreviousPixelClockCount	;
	bool PreviousbPllLocked ;
	uint32_t EstimatedPixelClockRate ;
	uint32_t OldEstPixelClockCount ;
	bool bClockStopped ;


	BHDR_FE_ChannelPowerSettings stPowerSettings ;

} BHDR_FE_P_ChannelHandle ;


/*******************************************************************************
Private FrontEnd Handle Declaration (encompasses all Front End channels)
*******************************************************************************/
BDBG_OBJECT_ID_DECLARE(BHDR_FE_P_Handle);
typedef struct BHDR_FE_P_Handle
{
	BDBG_OBJECT(BHDR_FE_P_Handle)

	BINT_Handle   hInterrupt ;
	BREG_Handle hRegister ;
	BCHP_Handle hChip ;
	
	uint32_t                       ulOffset ;

	BHDR_FE_Settings DeviceSettings ;
	BHDR_FE_ChannelHandle channel[BHDR_FE_MAX_CHANNELS] ;
	
	BINT_CallbackHandle hCallback[MAKE_INTR_FE_ENUM(LAST)] ;

#if BCHP_PWR_SUPPORT	
	bool standby;      /* true if in standby */
	bool enableWakeup; /* true if standby wakeup from CEC is enabled */
#endif

} BHDR_FE_P_Handle ;


/* channels are Clock, Ch0, Ch1, and C2 */


typedef enum
{
BHDR_FE_P_CLOCK_eChRef = 0,

BHDR_FE_P_CLOCK_eCh0,
BHDR_FE_P_CLOCK_eCh1,
BHDR_FE_P_CLOCK_eCh2,

BHDR_FE_P_CLOCK_eChMax
} BHDR_FE_P_Clock_Channel ;

#define BHDR_FE_P_CHANNEL_COUNT BHDR_FE_P_CLOCK_eChMax ;


/******************************************************************************
Summary:
structure containing FE Clock Information
*******************************************************************************/
typedef struct
{
	uint32_t PixelCount ;
	uint32_t Frequency ;
	bool bClockStopped ;
} BHDR_FE_P_PixelClockStatus ;


void BHDR_FE_P_GetPllLockStatus_isr(BHDR_FE_ChannelHandle hFeChannel, 
	bool *bLocked) ;

BERR_Code BHDR_FE_P_GetPixelClockEstimate_isr(BHDR_FE_ChannelHandle hFeChannel, 
	uint32_t *EstimatedPixelClockRate) ;

void BHDR_FE_P_EnableInterrupts_isr(BHDR_FE_ChannelHandle hFeChannel, bool enable) ;

void BHDR_FE_P_Initialize(BHDR_FE_Handle hFrontEnd) ;
void BHDR_FE_P_OpenChannel(BHDR_FE_ChannelHandle hFeChannel) ;
void BHDR_FE_P_CloseChannel(BHDR_FE_ChannelHandle hFeChannel) ;
void BHDR_FE_P_ResetPixelClockEstimation_isr(BHDR_FE_ChannelHandle hFeChannel) ;
void BHDR_FE_P_UnReset(BHDR_FE_ChannelHandle hFeChannel) ;
void BHDR_FE_P_ResetFeDataChannels_isr(BHDR_FE_ChannelHandle hFeChannel) ;
void BHDR_FE_P_ResetFeClockChannel_isr(BHDR_FE_ChannelHandle hFeChannel) ;
void BHDR_FE_P_GetPixelClockData_isr(BHDR_FE_ChannelHandle hFeChannel, uint32_t *PixelClockCount, uint8_t *ClockStopped) ;

void BHDR_FE_P_GetPixelClockStatus_isr(
	BHDR_FE_ChannelHandle hFeChannel, BHDR_FE_P_PixelClockStatus *PixelClock) ;

void BHDR_FE_P_SetHotPlug(BHDR_FE_ChannelHandle hFeChannel, BHDR_HotPlugSignal)  ;

#if BCHP_PWR_SUPPORT
void BHDR_FE_P_PowerResourceRelease_DVP_HR(BHDR_FE_Handle hFrontEnd) ;
void BHDR_FE_P_PowerResourceAcquire_DVP_HR(BHDR_FE_Handle hFrontEnd) ;
void BHDR_FE_P_PowerResourceRelease_HDMI_RX_FE(BHDR_FE_ChannelHandle hFeChannel) ;
void BHDR_FE_P_PowerResourceAcquire_HDMI_RX_FE(BHDR_FE_ChannelHandle hFeChannel) ;

#endif
	
#ifdef __cplusplus
}
#endif
 
#endif
/* end bhdr_fe_ priv.h */

