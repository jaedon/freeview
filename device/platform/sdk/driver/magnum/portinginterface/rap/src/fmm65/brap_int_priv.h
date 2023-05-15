/***************************************************************************
*     Copyright (c) 2004-2010, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: brap_int_priv.h $
* $brcm_Revision: Hydra_Software_Devel/13 $
* $brcm_Date: 9/6/10 5:17p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /magnum/portinginterface/rap/base3/brap_int_priv.h $
* 
* Hydra_Software_Devel/13   9/6/10 5:17p gautamk
* SW7550-459: [7550] Merging GFX support in mainline
* 
* Hydra_Software_Devel/RAP_GFX_SUPPORT_PH6P0/1   8/9/10 5:16p gautamk
* SW7550-459: [7550] Meging GFX support to Ph6p0 of 7550
* 
* Hydra_Software_Devel/RAP_GFX_SUPPORT/1   6/16/10 3:21p gautamk
* SW7550-459: [7550] Adding Support for GFx in RAP
* 
* Hydra_Software_Devel/12   10/27/09 12:42p gautamk
* SW7405-3110: [7405]Adding Destination sample rate change interrupt.
* This will need to be installed for HDMI. Whenver sample rate changes,
* this interrupt gets trigerred. So app need to install callback
* function on it to program HDMI rx sample rate. Checkin at mainline
* 
* Hydra_Software_Devel/11   10/2/09 2:35a srajapur
* SW7550-52: [7550] Merging 7550 branch code to main line
* 
* Hydra_Software_Devel/10   1/22/09 5:27p gautamk
* PR50455: [7405] Setting VOLUME STEP RAMP to 0x20 when DSP FW triggers
* EnableRamp interrupt.
* 
* Hydra_Software_Devel/9   10/29/08 3:26a speter
* PR 44771: [3548,3556] Adding interrupt for Ring Buffer Overflow and
* cleaning up base3 code to remove 3563 and 7440 specific code
* 
* Hydra_Software_Devel/8   9/19/08 8:53p sgadara
* PR 44770: [3548,3556] Implementing the Destination based Interrupts for
* 'capture to ringbuffer' feature.
* 
* Hydra_Software_Devel/7   6/11/08 7:30a speter
* PR38950: [3548,3556] Changing generic use of BTSC as RF Audio
* 
* Hydra_Software_Devel/6   5/5/08 11:41a speter
* PR 38950: [3548] Making 3548_FAMILY macro and using it for both 3548
* and 3556 cases.
* 
* Hydra_Software_Devel/5   4/28/08 4:29p speter
* PR 38950: [3548] Fixing compilation for 3548/3556.
* 
* Hydra_Software_Devel/4   2/1/08 9:13p gdata
* PR34648: [7405] Sending the decode interrupt at start time
* 
* Hydra_Software_Devel/3   1/28/08 2:21p gdata
* PR34648: [7405] Adding comments for different APIs
* 
* Hydra_Software_Devel/2   1/24/08 12:04a sushmit
* PR34648: [7405] Merging in updated code from Base2
* 
* Hydra_Software_Devel/1   1/22/08 4:47p sushmit
* PR34648: [7405] Merging in updated code from Base2
* 
* Hydra_Software_Devel/Rap_7405_NewFwIf/1   1/10/08 3:24p gdata
* PR 34648:[7440] Adding command for asynchronous interrupt
* 
* Hydra_Software_Devel/2   12/29/06 1:09a nitinb
* PR 26766: Added prototypes of BTSC interrupt private functions
* 
* Hydra_Software_Devel/1   12/15/06 10:40a speter
* PR 25607: Adding the base2 version of brap_int_priv.h to be used by
* 7440 onwards
* 
* 
***************************************************************************/

#ifndef BRAP_INT_PRIV_H__
#define BRAP_INT_PRIV_H__

#include "brap_priv.h"


#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
Summary: 
    This structure maintains information for the BRAP application callbacks 
    associated with a Raptor Interrupt.
    
Note:  
    The parameters passed to BRAP_CallbackFunc are
        void *    pParm1    : Application specific (passed unchanged)
        int       iParm2    : Application specific (passed unchanged)
        void *    pRAP_data : Raptor specific data exchanged between the 
                              BINT_Callback and the Application callback
******************************************************************************/
typedef struct BRAP_P_AppIntCbInfo
{
    BRAP_CallbackFunc pfAppCb;  /* Application specified Callback fnc */
    void *            pParm1;   /* Application specified parameter that needs 
                                   to be passed unchanged to the callback */
    int               iParm2;   /* Application specified parameter that needs 
                                   to be passed unchanged to the callback */
} BRAP_P_AppIntCbInfo;



/***************************************************************************
Summary:
	Clear all previous Raptor interrupts and Mask ALL raptor interrupts

Description:    
    This should be called in device open ie BRAP_Open(). It masks all raptor 
    interrupts making sure no interrupts come before the interrupt interface
    has been installed.

****************************************************************************/
BERR_Code BRAP_P_ClearInterrupts (
        BREG_Handle hRegister   /* [in] The register handle */
);


/***************************************************************************
Summary:
	Initialises interrupt handling in Raptor

Description:    
    This installs a BINT callback for the top level Audio L2 interrupt  
    for each Raptor channel (with Param1 as the raptor channel handle, 
    param2 is not used).

    This may differ across chip versions.

****************************************************************************/
BERR_Code BRAP_P_InterruptInstall (
	BRAP_ChannelHandle 		hRapCh		/* [in] Raptor Channel handle */
);


/***************************************************************************
Summary:
	CLoses interrupt handling in Raptor

Description:    
    This may differ across chip versions.

****************************************************************************/
BERR_Code BRAP_P_InterruptUnInstall (
	BRAP_ChannelHandle 		hRapCh		/* [in] Raptor Channel handle */
);

#if (BRAP_7550_FAMILY !=1)
/***************************************************************************
Summary:
	Initialises interrupt handling in Raptor

Description:    
    This installs a BINT callback for the top level Audio L2 interrupt  
    for each Raptor channel (with Param1 as the raptor destination handle, 
    param2 is not used).

    This may differ across chip versions.

****************************************************************************/
BERR_Code BRAP_P_DestinationLevelInterruptInstall(
	BRAP_DestinationHandle        hDstHandle    /* [in] Raptor Destination handle */
);


/***************************************************************************
Summary:
	CLoses interrupt handling in Raptor

Description:    
    This may differ across chip versions.

****************************************************************************/
BERR_Code BRAP_P_DestinationLevelInterruptUnInstall (
	BRAP_DestinationHandle        hDstHandle    /* [in] Raptor Destination handle */
);

void BRAP_P_DSP2Host_isr (
        void * pParm1, /* [in] Raptor channel handle */
        int    iParm2  /* [in] Not used */        
);
#endif

/***************************************************************************
Description:
	This API will mask the bit for the corrosponding task id. So that FW can 
	not raise the interrupt for this task any more.
Returns:
	BERR_SUCCESS - If successful
***************************************************************************/
BERR_Code   BRAP_P_MaskTask(
        BRAP_FWIF_P_FwTaskHandle hTask
);

/***************************************************************************
Description:
	This API will unmask the bit for the corrosponding task id. So that FW can 
	raise the interrupt for this task.
Returns:
	BERR_SUCCESS - If successful
***************************************************************************/
BERR_Code   BRAP_P_UnmaskTask(
        BRAP_FWIF_P_FwTaskHandle hTask
);

void BRAP_P_EnableRfAudioInterrupts (
    BRAP_CAPPORT_P_Handle       phCapPort     /* [out] Pointer to Capture Port handle */
);

void BRAP_P_DisableRfAudioInterrupts (
    BRAP_CAPPORT_P_Handle       phCapPort     /* [out] Pointer to Capture Port handle */
);

BERR_Code BRAP_P_DeviceLevelInterruptInstall (
	BRAP_Handle		hRap
);

BERR_Code BRAP_P_DeviceLevelInterruptUnInstall (
	BRAP_Handle		hRap
);

void BRAP_P_FmmRbufFreeWaterMark_isr (
        void * pParm1,  /* [in] Raptor channel handle */
        int    iParm2   /* [in] RBUF index */
);

void BRAP_P_RampEnableTimer_isr(
        void * pParm1, /* [in] Raptor channel handle */
        int    iParm2  /* [in] RBUF index */
);


void BRAP_P_FmmDestinationRbufFullMark_isr (
	void *pParm1,       /* [in] Raptor destination handle */
	int dummy           
);

void BRAP_P_FmmDestinationRbufFlowControl_isr (
	void *pParm1,       /* [in] Raptor destination handle */
	int dummy           
);


/***************************************************************************
Description:
	This API will send the command to FW to disbale all the asynchronous events 
	for any task. This api will be called in the start of the task so that we 
	can enable event as per required later.
Returns:
	BERR_SUCCESS - If successful
***************************************************************************/
BERR_Code BRAP_P_DisableAllEvent(
    BRAP_FWIF_P_FwTaskHandle hTask
);

/***************************************************************************
Description:
	This API installs the decoder interrupts.
Returns:
	BERR_Code 
See Also:
	BRAP_P_InterruptInstall
***************************************************************************/
BERR_Code BRAP_P_DecoderInterrupt(
    BRAP_FWIF_P_FwTaskHandle    hTask		/* [in] Task handle */
);

/***************************************************************************
Description:
	This API Installs the top level interrupt handlers for DSP ping command.
Returns:
	BERR_Code 
See Also:
	BRAP_P_AckUnInstall
***************************************************************************/
BERR_Code BRAP_P_AckInstall(
    BRAP_DSP_Handle         hDsp
);    

/***************************************************************************
Description:
	This API uninstalls the top level interrupt handlers for DSP ping command.
Returns:
	BERR_Code 
See Also:
	BRAP_P_AckInstall
***************************************************************************/
BERR_Code BRAP_P_AckUnInstall(
    BRAP_DSP_Handle         hDsp
);

/***************************************************************************
Description:
    This Isr function is called when sample rate associated with any Destination is programmed.
    
Returns:
	void 
***************************************************************************/

void BRAP_P_DestinationSampleRateChange_isr (
        void * pParm1, /* [in] Raptor destination handle */
        unsigned int    iParm2  
);

/***************************************************************************
Description:
	This API will be a helper function and will search the TASK in which FW
	is doing decoding.
Returns:
    void
***************************************************************************/
 void BRAP_P_GetTask(
        BRAP_ChannelHandle          hRapCh,
        BRAP_FWIF_P_FwTaskHandle    *hTask
);


#ifdef __cplusplus
}
#endif

#endif /* BRAP_INT_PRIV_H__ */
/* End of File */    

