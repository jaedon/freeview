/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bhdr_fe_priv.c $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 9/24/12 5:48p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/hdr/src/core28nm/bhdr_fe_priv.c $
 * 
 * Hydra_Software_Devel/2   9/24/12 5:48p rgreen
 * SW7445-24: Initial 28nm support for HDMI Rx Frontend
 * 
 ***************************************************************************/
#include "bhdr.h"
#include "bhdr_priv.h"
	 
#include "bhdr_fe.h"
#include "bhdr_fe_priv.h"

#include "bchp_hdmi_rx_eq_0.h"

#if BCHP_PWR_SUPPORT
#include "bchp_pwr.h"
#endif


BDBG_MODULE(BHDR_FE_PRIV) ;


void BHDR_FE_P_Initialize(BHDR_FE_Handle hFrontEnd) 
{

	BREG_Handle hRegister ;
	uint32_t Register ;
	uint32_t ulOffset ;

	BDBG_ENTER(BHDR_FE_P_Initialize) ;
	BDBG_OBJECT_ASSERT(hFrontEnd, BHDR_FE_P_Handle) ;

	hRegister = hFrontEnd->hRegister ;
	ulOffset = hFrontEnd->ulOffset ;

	Register = BREG_Read32(hRegister, BCHP_DVP_HR_TOP_SW_INIT) ;
	Register &= ~BCHP_MASK(DVP_HR_TOP_SW_INIT, RX_0) ;
	BREG_Write32(hRegister, BCHP_DVP_HR_TOP_SW_INIT, Register) ;
	
	Register = BREG_Read32(hRegister, BCHP_DVP_HR_TOP_SW_INIT) ;
	Register &= ~ BCHP_MASK(DVP_HR_TOP_SW_INIT, FE_0) ;
	BREG_Write32(hRegister, BCHP_DVP_HR_TOP_SW_INIT, Register) ;

	BDBG_LEAVE(BHDR_FE_P_Initialize) ;

}


void BHDR_FE_P_OpenChannel(
	BHDR_FE_ChannelHandle hFeChannel) /* [out] Created channel handle */
{
	BREG_Handle hRegister ;
	uint32_t Register ;
	uint32_t ulOffset ;
	
	BDBG_ENTER(BHDR_FE_P_OpenChannel) ;
	BDBG_OBJECT_ASSERT(hFeChannel, BHDR_FE_P_ChannelHandle);

	hRegister = hFeChannel->hRegister ;
	ulOffset = hFeChannel->ulOffset ;

	/* allow the Hot Plug to power down the PHY */
	Register = BREG_Read32(hRegister, BCHP_HDMI_RX_FE_0_HDMI_RX_PHY_RESET_AND_POWER_CFG_1 + ulOffset) ;
	Register &= ~ BCHP_MASK(HDMI_RX_FE_0_HDMI_RX_PHY_RESET_AND_POWER_CFG_1,
		ALLOW_HOTPLUG_TO_POWER_DOWN_PHY) ;
	Register |= BCHP_FIELD_DATA(HDMI_RX_FE_0_HDMI_RX_PHY_RESET_AND_POWER_CFG_1, 
		ALLOW_HOTPLUG_TO_POWER_DOWN_PHY, 1) ;
	BREG_Write32(hRegister, BCHP_HDMI_RX_FE_0_HDMI_RX_PHY_RESET_AND_POWER_CFG_1 + ulOffset, Register) ;

#if BHDR_CONFIG_FORMAT_DETECT_INPUT
		/* Config format detect - can't use the default */
	Register = BREG_Read32(hRegister, BCHP_HDMI_RX_0_MISC_CONTROL + ulOffset) ;
	Register &= ~ BCHP_MASK(HDMI_RX_0_MISC_CONTROL, FORMAT_DETECT_INPUT_SELECT) ;
	BREG_Write32(hRegister, BCHP_HDMI_RX_0_MISC_CONTROL + ulOffset, Register) ;
#endif

	
	Register = BREG_Read32( hRegister, BCHP_HDMI_RX_FE_0_HOTPLUG_CONTROL  + ulOffset) ;
	Register |= BCHP_MASK(HDMI_RX_FE_0_HOTPLUG_CONTROL, ENABLE_CLOCK_STOPPED_AS_HOTPLUG) ; /* 1 */
#if BHDR_CONFIG_HDCP_REPEATER
	/* HDMI_TODO Hot Plug bypass must be disabled to force HPD signal */
	Register &= ~ BCHP_MASK(HDMI_RX_FE_0_HOTPLUG_CONTROL, HOTPLUG_BYPASS) ; 
#endif
	BREG_Write32(hRegister, BCHP_HDMI_RX_FE_0_HOTPLUG_CONTROL + ulOffset, Register) ;


	Register = BREG_Read32(hRegister, BCHP_HDMI_RX_EQ_0_RX_FREQ_MON_CONTROL1_CH0 + ulOffset) ;
	Register &= ~ (
		  BCHP_MASK(HDMI_RX_EQ_0_RX_FREQ_MON_CONTROL1_CH0, MON_CORRECT_EN) 
		| BCHP_MASK(HDMI_RX_EQ_0_RX_FREQ_MON_CONTROL1_CH0, MON_MARGIN_VAL))  ;
	Register |= 
		  BCHP_FIELD_DATA(HDMI_RX_EQ_0_RX_FREQ_MON_CONTROL1_CH0, MON_CORRECT_EN, 1)
		| BCHP_FIELD_DATA(HDMI_RX_EQ_0_RX_FREQ_MON_CONTROL1_CH0, MON_MARGIN_VAL, 0x51) ;
	BREG_Write32(hRegister, 
		BCHP_HDMI_RX_EQ_0_RX_FREQ_MON_CONTROL1_CH0 + ulOffset, Register) ;


	Register = BREG_Read32(hRegister, BCHP_HDMI_RX_EQ_0_RX_FREQ_MON_CONTROL1_CH1 + ulOffset) ;
	Register &= ~ (
		  BCHP_MASK(HDMI_RX_EQ_0_RX_FREQ_MON_CONTROL1_CH1, MON_CORRECT_EN) 
		| BCHP_MASK(HDMI_RX_EQ_0_RX_FREQ_MON_CONTROL1_CH1, MON_MARGIN_VAL))  ;
	Register |= 
		  BCHP_FIELD_DATA(HDMI_RX_EQ_0_RX_FREQ_MON_CONTROL1_CH1, MON_CORRECT_EN, 1)
		| BCHP_FIELD_DATA(HDMI_RX_EQ_0_RX_FREQ_MON_CONTROL1_CH1, MON_MARGIN_VAL, 0x51) ;
	BREG_Write32(hRegister, 
		BCHP_HDMI_RX_EQ_0_RX_FREQ_MON_CONTROL1_CH1 + ulOffset, Register) ;


	Register = BREG_Read32(hRegister, BCHP_HDMI_RX_EQ_0_RX_FREQ_MON_CONTROL1_CH2 + ulOffset) ;
	Register &= ~ (
		  BCHP_MASK(HDMI_RX_EQ_0_RX_FREQ_MON_CONTROL1_CH2, MON_CORRECT_EN) 
		| BCHP_MASK(HDMI_RX_EQ_0_RX_FREQ_MON_CONTROL1_CH2, MON_MARGIN_VAL))  ;
	Register |= 
		  BCHP_FIELD_DATA(HDMI_RX_EQ_0_RX_FREQ_MON_CONTROL1_CH2, MON_CORRECT_EN, 1)
		| BCHP_FIELD_DATA(HDMI_RX_EQ_0_RX_FREQ_MON_CONTROL1_CH2, MON_MARGIN_VAL, 0x51) ;
	BREG_Write32(hRegister, 
		BCHP_HDMI_RX_EQ_0_RX_FREQ_MON_CONTROL1_CH2 + ulOffset, Register) ;
	
	BDBG_LEAVE(BHDR_FE_P_OpenChannel) ;
}


void BHDR_FE_P_CloseChannel(BHDR_FE_ChannelHandle hFeChannel) 
{
	BREG_Handle hRegister ;
	uint32_t ulOffset ;

	BDBG_ENTER(BHDR_FE_P_CloseChannel) ;
	BDBG_OBJECT_ASSERT(hFeChannel, BHDR_FE_P_ChannelHandle) ;
	
	hRegister = hFeChannel->hRegister;
	ulOffset = hFeChannel->ulOffset;

	BDBG_LEAVE(BHDR_FE_P_CloseChannel) ;
}


void BHDR_FE_P_ResetPixelClockEstimation_isr(BHDR_FE_ChannelHandle hFeChannel) 
{
	BREG_Handle hRegister ;
	uint32_t Register ;
	uint32_t ulOffset ;

	BDBG_ENTER(BHDR_FE_P_ResetPixelClockEstimation_isr) ;
	BDBG_OBJECT_ASSERT(hFeChannel, BHDR_FE_P_ChannelHandle) ;

	hRegister = hFeChannel->hRegister;
	ulOffset = hFeChannel->ulOffset;

	Register = BREG_Read32(hRegister, BCHP_DVP_HR_HDMI_FE_0_SW_INIT ) ;
	Register &= ~ (BCHP_MASK(DVP_HR_HDMI_FE_0_SW_INIT, FREQ_EST)) ;
		
	Register |= BCHP_FIELD_DATA(DVP_HR_HDMI_FE_0_SW_INIT, FREQ_EST, 1) ;	
	BREG_Write32(hRegister, BCHP_DVP_HR_HDMI_FE_0_SW_INIT, Register) ;

	Register &= ~ (BCHP_MASK(DVP_HR_HDMI_FE_0_SW_INIT, FREQ_EST)) ;
	BREG_Write32(hRegister, BCHP_DVP_HR_HDMI_FE_0_SW_INIT, Register) ;

	BDBG_LEAVE(BHDR_FE_P_ResetPixelClockEstimation_isr) ;
}


/******************************************************************************
void BHDR_FE_P_GetPixelClockStatus_isr
Summary: Reads appropriate FE registers to get Pixel Clock data
*******************************************************************************/
void BHDR_FE_P_GetPixelClockStatus_isr(BHDR_FE_ChannelHandle hFeChannel, 
	BHDR_FE_P_PixelClockStatus *ClockStatus) 

{
	BREG_Handle hRegister ;
	uint32_t Register ;
	uint32_t ulFeOffset ;
	uint32_t RegAddr ;
	uint8_t Channel ;

	BDBG_ENTER(BHDR_FE_P_GetPixelClockStatus_isr) ;
	BDBG_OBJECT_ASSERT(hFeChannel, BHDR_FE_P_ChannelHandle) ;	

	hRegister = hFeChannel->hRegister ;
	ulFeOffset = hFeChannel->ulOffset ;

	
	RegAddr = BCHP_HDMI_RX_FE_0_PIX_CLK_CNT + ulFeOffset ;
	for (Channel = 0 ; Channel < BHDR_FE_P_CLOCK_eChMax ; Channel++)
	{
		Register = BREG_Read32(hRegister, RegAddr) ;
		ClockStatus[Channel].PixelCount = 
			BCHP_GET_FIELD_DATA(Register, HDMI_RX_FE_0_PIX_CLK_CNT, PIX_CLK_CNT) ;

		RegAddr = RegAddr + 4 ;

		Register = BREG_Read32(hRegister, BCHP_DVP_HR_FREQ_MEASURE_CONTROL) ;
		Register &= ~ BCHP_MASK(DVP_HR_FREQ_MEASURE_CONTROL, SOURCE) ;
		Register |= BCHP_FIELD_DATA(DVP_HR_FREQ_MEASURE_CONTROL, SOURCE, Channel) ;
		BREG_Write32(hRegister, BCHP_DVP_HR_FREQ_MEASURE_CONTROL, Register) ;

		Register = BREG_Read32(hRegister, BCHP_DVP_HR_FREQ_MEASURE) ;

		ClockStatus[Channel].Frequency = 
			BCHP_GET_FIELD_DATA(Register, DVP_HR_FREQ_MEASURE, VALUE) ;
	}


	Register = BREG_Read32(hRegister, BCHP_HDMI_RX_FE_0_CLK_CNT_STATUS + ulFeOffset) ;
	ClockStatus[BHDR_FE_P_CLOCK_eChRef].bClockStopped = 
		BCHP_GET_FIELD_DATA(Register, HDMI_RX_FE_0_CLK_CNT_STATUS, CLOCK_STOPPED_REF) ;

	ClockStatus[BHDR_FE_P_CLOCK_eCh0].bClockStopped = 
		BCHP_GET_FIELD_DATA(Register, HDMI_RX_FE_0_CLK_CNT_STATUS, CLOCK_STOPPED_CH0) ;
	ClockStatus[BHDR_FE_P_CLOCK_eCh1].bClockStopped = 
		BCHP_GET_FIELD_DATA(Register, HDMI_RX_FE_0_CLK_CNT_STATUS, CLOCK_STOPPED_CH1) ;
	ClockStatus[BHDR_FE_P_CLOCK_eCh2].bClockStopped = 
		BCHP_GET_FIELD_DATA(Register, HDMI_RX_FE_0_CLK_CNT_STATUS, CLOCK_STOPPED_CH2) ;



#if BHDR_CONFIG_DEBUG_FRONT_END
	BDBG_WRN(("Frontend Clock Status:")) ;
	for (Channel = 0 ; Channel < BHDR_FE_P_CLOCK_eChMax ; Channel++)
	{
		BDBG_WRN(("Channel_%d  Pixel Count= %d  Frequency= %d Clock Status: %s",
			Channel, 	
			ClockStatus[Channel].PixelCount, ClockStatus[Channel].Frequency,
			ClockStatus[Channel].bClockStopped ? "Stopped" : "Running")) ;
	}
#endif

	BDBG_LEAVE(BHDR_FE_P_GetPixelClockStatus_isr) ;
}


/******************************************************************************
Summary:
*******************************************************************************/
void BHDR_FE_P_SetHotPlug(BHDR_FE_ChannelHandle hFeChannel, BHDR_HotPlugSignal eHotPlugSignal)           
{
	BREG_Handle hRegister ;
	uint32_t Register ;
	uint32_t ulFeOffset  ;

	BDBG_ENTER(BHDR_FE_P_SetHotPlug) ;
	BDBG_OBJECT_ASSERT(hFeChannel, BHDR_FE_P_ChannelHandle) ;	

	hRegister= hFeChannel->hRegister ;
	ulFeOffset = hFeChannel->ulOffset ;
	
	Register = BREG_Read32(hRegister, BCHP_HDMI_RX_FE_0_HOTPLUG_CONTROL + ulFeOffset) ;

	Register &= ~ (
		  BCHP_MASK(HDMI_RX_FE_0_HOTPLUG_CONTROL, HOTPLUG_BYPASS) 
		| BCHP_MASK(HDMI_RX_FE_0_HOTPLUG_CONTROL, OVERRIDE_HOTPLUG_OUT) 
		| BCHP_MASK(HDMI_RX_FE_0_HOTPLUG_CONTROL, OVERRIDE_HOTPLUG_OUT_VALUE))  ;

	Register |= 
		  BCHP_FIELD_DATA(HDMI_RX_FE_0_HOTPLUG_CONTROL, HOTPLUG_BYPASS, eHotPlugSignal) 
		| BCHP_FIELD_DATA(HDMI_RX_FE_0_HOTPLUG_CONTROL, OVERRIDE_HOTPLUG_OUT, !eHotPlugSignal) 
		| BCHP_FIELD_DATA(HDMI_RX_FE_0_HOTPLUG_CONTROL, OVERRIDE_HOTPLUG_OUT_VALUE, eHotPlugSignal) ;

	
	BREG_Write32(hRegister, BCHP_HDMI_RX_FE_0_HOTPLUG_CONTROL + ulFeOffset, Register) ;

	BDBG_LEAVE(BHDR_FE_P_SetHotPlug) ;
}


void BHDR_FE_P_PowerResourceAcquire_DVP_HR(BHDR_FE_Handle hFrontEnd) 
{
	BDBG_ENTER(BHDR_FE_P_PowerResourceAcquire_DVP_HR) ;
	BDBG_OBJECT_ASSERT(hFrontEnd, BHDR_FE_P_Handle) ;

#if BHDR_CONFIG_DEBUG_HDR_PWR
	BDBG_WRN(("Acquire HDMI_RX_0_216M Resource at line %d", __LINE__)) ;
#endif
#if BCHP_PWR_RESOURCE_HDMI_RX0_216M
	BCHP_PWR_AcquireResource(hFrontEnd->hChip, BCHP_PWR_RESOURCE_HDMI_RX0_216M);
#endif

	BDBG_LEAVE(BHDR_FE_P_PowerResourceAcquire_DVP_HR) ;
}


void BHDR_FE_P_PowerResourceRelease_DVP_HR(BHDR_FE_Handle hFrontEnd) 
{
	BDBG_ENTER(BHDR_FE_P_PowerResourceRelease_DVP_HR) ;
	BDBG_OBJECT_ASSERT(hFrontEnd, BHDR_FE_P_Handle) ;

#if BHDR_CONFIG_DEBUG_HDR_PWR
	BDBG_WRN(("Release HDMI_RX_0_216M Resource at line %d", __LINE__)) ;
#endif
#if BCHP_PWR_RESOURCE_HDMI_RX0_216M
	BCHP_PWR_ReleaseResource(hFrontEnd->hChip, BCHP_PWR_RESOURCE_HDMI_RX0_216M);
#endif
 	
	BDBG_LEAVE(BHDR_FE_P_PowerResourceRelease_DVP_HR) ;
}


void BHDR_FE_P_PowerResourceAcquire_HDMI_RX_FE(BHDR_FE_ChannelHandle hFeChannel)
{
	BDBG_ENTER(BHDR_FE_P_PowerResourceAcquire_HDMI_RX_FE) ;
	BDBG_OBJECT_ASSERT(hFeChannel, BHDR_FE_P_ChannelHandle);

#if BHDR_CONFIG_DEBUG_HDR_PWR
	BDBG_WRN(("Acquire HDMI_RX0_FE Resource at line %d", __LINE__)) ;
#endif
#if BCHP_PWR_RESOURCE_HDMI_RX0_FE
	BCHP_PWR_AcquireResource(hFeChannel->hChip, BCHP_PWR_RESOURCE_HDMI_RX0_FE);
#endif

	BDBG_LEAVE(BHDR_FE_P_PowerResourceAcquire_HDMI_RX_FE) ;
}


void BHDR_FE_P_PowerResourceRelease_HDMI_RX_FE(BHDR_FE_ChannelHandle hFeChannel)
{
	BDBG_ENTER(BHDR_FE_P_PowerResourceAcquire_HDMI_RX_FE) ;
	BDBG_OBJECT_ASSERT(hFeChannel, BHDR_FE_P_ChannelHandle);

#if BHDR_CONFIG_DEBUG_HDR_PWR
	BDBG_WRN(("Release HDMI_RX0_FE Resource at line %d", __LINE__)) ;
#endif
#if BCHP_PWR_RESOURCE_HDMI_RX0_FE
	BCHP_PWR_ReleaseResource(hFeChannel->hChip, BCHP_PWR_RESOURCE_HDMI_RX0_FE);
#endif

	BDBG_LEAVE(BHDR_FE_P_PowerResourceAcquire_HDMI_RX_FE) ;
}


