/***************************************************************************
 *     Copyright (c) 2003-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bhdr_fe_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/18 $
 * $brcm_Date: 12/29/09 3:47p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/hdr/3548/bhdr_fe_priv.h $
 * 
 * Hydra_Software_Devel/18   12/29/09 3:47p rgreen
 * SW3548-2670: change ChannelSettings member from 'DeviceSettings' to
 * 'settings'
 * 
 * Hydra_Software_Devel/17   6/5/09 3:32p rgreen
 * PR46645: Disable HDMI Frontend and Receiver interrupts when input is
 * not selected
 * 
 * Hydra_Software_Devel/PR46645/1   5/29/09 5:10p rgreen
 * PR46645,PR55586: Disable HDMI Frontend and Receiver interrupts when
 * input is not selected
 * 
 * Hydra_Software_Devel/16   3/2/09 3:41p rgreen
 * PR52672: Clear HDCP Authentication at hotplug, frequency change, and
 * PLL lock change
 * 
 * Hydra_Software_Devel/15   2/25/09 10:41a rgreen
 * PR52206: Eliminate excessive warning messages for HDMI Get Status
 * functions
 * 
 * Hydra_Software_Devel/14   2/9/09 3:56p rgreen
 * PR51752: BHDR_GetPixelClockEstimate moved from bhdr_fe.c to bhdr.c
 * 
 * Hydra_Software_Devel/13   11/14/08 4:00p rgreen
 * PR48775: PR46447: Merge changes to branch
 * Use N and CTS to calculate audio sample rate
 * 
 * Hydra_Software_Devel/PR48775/2   11/13/08 4:40p rgreen
 * PR46447: Merge changes to branch
 * 
 * Hydra_Software_Devel/12   11/10/08 5:30p rgreen
 * PR46447: Use same CEC structure for transmitter and receiver
 * 
 * Hydra_Software_Devel/PR48775/1   11/10/08 5:03p honglang
 * PR48775: use N and CTS to calculate audio sample rate
 * 
 * Hydra_Software_Devel/11   10/1/08 9:16p rgreen
 * PR46176: Add support for power management
 * 
 * Hydra_Software_Devel/10   9/11/08 5:40p rgreen
 * PR45482: PR45358,PR44257: Merge changes
 * 
 * Hydra_Software_Devel/PR45482/3   9/11/08 4:20p rgreen
 * PR45358,PR44257: Remove BHDR_CEC_SUPPORT defines/macros.  CEC enabled
 * by default
 * 
 * Hydra_Software_Devel/PR45482/2   8/27/08 4:19p rgreen
 * PR42257:Add CEC isr prototype; remove unused
 * BHDR_FE_P_HotPlugDisconnect_isr prototype
 * 
 * Hydra_Software_Devel/PR45482/1   8/7/08 2:03p honglang
 * PR44257,PR45482: support CEC on 3549/56 HDMI input
 * 
 * Hydra_Software_Devel/9   7/29/08 4:31p rgreen
 * PR44523: PR44697:Use PLL Lock status to determine if a vald Pixel Clock
 * can be calculated
 * 
 * Hydra_Software_Devel/PR44523/1   7/9/08 11:44p rgreen
 * PR44697:Use PLL Lock status to determine if a vald Pixel Clock can be
 * calculated
 * 
 * Hydra_Software_Devel/8   6/24/08 6:23p rgreen
 * PR44124:Fix possible float point exception error
 * 
 * Hydra_Software_Devel/7   6/23/08 10:30a rgreen
 * PR43984: Fix interrupt handling for multiple channels
 * 
 * Hydra_Software_Devel/6   6/19/08 6:51p rgreen
 * PR43752: Adjust FE PLL Setting based on pixel clock frequency
 * fix channel mapping for FE0 vs FE1
 * 
 * Hydra_Software_Devel/5   6/18/08 8:31p rgreen
 * PR38947: Fix HP interrupts for both ports
 * 
 * Hydra_Software_Devel/4   6/3/08 3:49p rgreen
 * PR38947: Save supporting device handles (chip, register, interrupt) in
 * Front End handle.
 * 
 * Hydra_Software_Devel/3   5/7/08 1:52p rgreen
 * PR38957,PR39463:
 * Replace BHDR_FE_GetEventHandle with Install/Uninstall Callbacks for
 * Front End Events (Hotplug and CEC)
 * Move HP Initialization for bhdr_fe file
 * Update BHDR_ConfigureAfterHotPlug to update core after HP Event
 * 
 * Hydra_Software_Devel/2   5/6/08 2:08p rgreen
 * PR38947,PR39463:
 * Implement AttachChannel function
 * 
 * Hydra_Software_Devel/1   4/4/08 4:34p rgreen
 * PR38947: Add Initial HDMI Rx 3548 Support
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

#include "bhdm_cec_priv.h" 


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
	 /* 15 */ MAKE_INTR_FE_ENUM(DUPLICATE_FE_SELECT),
	 /* 16 */ MAKE_INTR_FE_ENUM(MUX_0_IMPROPER_FE_SELECT_UPDATE), 
	 /* 17 */ MAKE_INTR_FE_ENUM(MUX_1_IMPROPER_FE_SELECT_UPDATE),
	 
	/*  */ MAKE_INTR_FE_ENUM(LAST)
	 
} BHDR_FE_P_InterruptMask ;



/******************************************************************************
Summary:
Enumeration of BHDR_FE_ChnInterrupts 
*******************************************************************************/
typedef enum
{
	 /* 00 */ MAKE_INTR_FE_CHN_ENUM(RX_HOTPLUG_UPDATE),
	 /* 01 */ MAKE_INTR_FE_CHN_ENUM(CEC_LOW),
	 /* 02 */ MAKE_INTR_FE_CHN_ENUM(CEC),
	 /* 03 */ MAKE_INTR_FE_CHN_ENUM(PLL_LOCK),   
	 /* 04 */ MAKE_INTR_FE_CHN_ENUM(FREQ_CHANGE),   


	/*  */ MAKE_INTR_FE_CHN_ENUM(LAST)
	 
} BHDR_FE_P_ChnInterruptMask ;




/* Get the offset of two groups of register. */
/* HDMI TODO Fix for >2 FrontEnds */
#define BHDR_FE_P_GET_REG_OFFSET(ChannelId) \
	((BHDR_FE_P_eChannel0 == (ChannelId)) ? 0 : (BCHP_HDMI_RX_FE_1_REG_START - BCHP_HDMI_RX_FE_0_REG_START))



/*******************************************************************************
Private HDMI Rx Frontend Channel Handle Declaration 
*******************************************************************************/
typedef struct BHDR_FE_P_ChannelHandle
{
	BINT_Handle   hInterrupt ;
	BREG_Handle hRegister ;
	
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
	
	BHDR_FE_CallbackFunc pfCecCallback_isr ;
	void                       *pvCecParm1;
	int                         iCecParm2;
	
	uint32_t PreviousPixelClockCount	;
	bool PreviousbPllLocked ;
	uint32_t EstimatedPixelClockRate ;

	/******************/
	/* CEC variables  */
	/******************/
	BCEC_CONFIG cecConfiguration ;

	BHDR_FE_ChannelPowerSettings stPowerSettings ;

} BHDR_FE_P_ChannelHandle ;



/*******************************************************************************
Private FrontEnd Handle Declaration (encompasses all Front End channels)
*******************************************************************************/
typedef struct BHDR_FE_P_Handle
{
	BINT_Handle   hInterrupt ;
	BREG_Handle hRegister ;
	BCHP_Handle hChip ;
	
	uint32_t                       ulOffset ;

	BHDR_FE_Settings DeviceSettings ;
	
	BINT_CallbackHandle hCallback[MAKE_INTR_FE_ENUM(LAST)] ;

	BKNI_EventHandle BHDR_FE_Event_DUPLICATE_FE_SELECT ; 
	BKNI_EventHandle BHDR_FE_Event_MUX_0_IMPROPER_FE_SELECT_UPDATE ;
	BKNI_EventHandle BHDR_FE_Event_MUX_1_IMPROPER_FE_SELECT_UPDATE ;

} BHDR_FE_P_Handle ;



/******************************************************************************
Summary:
Handle interrupts from the HDMIRx core.

Description:
Interrupts received from the HDMIRx core must be handled.  The following 
is a list of possible interrupts.


Input:
	pParameter - pointer to interrupt specific information BHDR_Open.

Output:
	<None>
	
Returns:
	<None>

See Also:

*******************************************************************************/
void BHDR_FE_P_FrontEndIntr_isr
(
	void *pParam1,						/* Device channel handle */
	int parm2							/* not used */
) ;
								

void BHDR_FE_P_ChannelIntr_isr
(
	void *pParam1,						/* Device channel handle */
	int parm2							/* not used */
) ;
								

void BHDR_FE_P_GetPllLockStatus_isr(BHDR_FE_ChannelHandle hFeChannel, 
	bool *bLocked) ;

BERR_Code BHDR_FE_P_GetPixelClockEstimate_isr(BHDR_FE_ChannelHandle hFeChannel, 
	uint32_t *EstimatedPixelClockRate) ;

 void BHDR_FE_P_CEC_isr(BHDR_FE_ChannelHandle hFeChannel)  ;
void BHDR_FE_P_EnableInterrupts_isr(BHDR_FE_ChannelHandle hFeChannel, bool enable) ;

	
#ifdef __cplusplus
}
#endif
 
#endif
/* end bhdr_fe_ priv.h */

