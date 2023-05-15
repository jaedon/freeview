/***************************************************************************
*     Copyright (c) 2004-2010, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: brap_spdifrx_priv.c $
* $brcm_Revision: Hydra_Software_Devel/18 $
* $brcm_Date: 10/25/10 4:51p $
*
* Module Description:
*   Module name: SPDIF/HDMI In (Receiver)
*   This file contains the implementation of all APIs for the hardware SPDIF/HDMI
*   receiver abstraction.
*   
* Revision History:
*
* $brcm_Log: /magnum/portinginterface/rap/base3/brap_spdifrx_priv.c $
* 
* Hydra_Software_Devel/18   10/25/10 4:51p sgadara
* SW3556-1206: [3548,3556] Merge customer branch to main line
* 
* Hydra_Software_Devel/17   1/25/10 12:22p speter
* SW3548-2728:[3548] Changing the SPDIF Rx DTS algo type to DTS-HD since
* the PES parsing is available only in the framesync of DTS HD algorithm
* in the firmware.
* 
* Hydra_Software_Devel/16   11/24/09 2:32p speter
* SW3548-2629: [3548] Adding SPDIF Rx Passthru without DSP
* 
* Hydra_Software_Devel/15   11/24/09 11:58a speter
* SW3548-2629: [3548] Adding SPDIF Rx Passthru without DSP
* 
* Hydra_Software_Devel/14   10/9/09 4:54p sgadara
* SW3548-2532: [3548,3556] Adding DTS Broadcast decode support
* --> Correcting the enum type that is returned
* 
* Hydra_Software_Devel/13   7/7/09 11:34a speter
* PR 56011: [3548,3556] Implementing Code review comments (using RDB
* macro instead of constants)
* 
* Hydra_Software_Devel/12   1/20/09 10:06a speter
* PR 51298: [3548,3556] Changing the code to make sample rate calculation
* common
* 
* Hydra_Software_Devel/11   1/13/09 4:29p sgadara
* PR 50721: [3548,3556] Moving the fix to mainline.
* 
* Hydra_Software_Devel/10   12/22/08 2:55p speter
* PR 35668: [3548,3556] Adding the ALLOW_NZ_STUFFING bit to allow non
* zero stuffing for compressed frames. This is being enabled now for
* 3548 after the hardware fix.
* 
* Hydra_Software_Devel/RAP_354856_PH_4_1/1   1/8/09 9:45p sgadara
* PR 50721: [3548,3556] Fixing the code to prevent from entering into
* case of spdif source.
* 
* Hydra_Software_Devel/9   12/4/08 12:05p speter
* PR 49300: [3548,3556] Adding the workaround for HDMI hardware issue.
* Correcting the A0 case
* 
* Hydra_Software_Devel/8   12/4/08 11:50a speter
* PR 49300: [3548,3556] Adding the workaround for HDMI hardware issue
* 
* Hydra_Software_Devel/7   11/25/08 2:02p speter
* PR 49399: [3548,3556] Fixing coverity issue of un-initialized variable
* being used.
* 
* Hydra_Software_Devel/6   11/6/08 4:20p sgadara
* PR 48564: [3548,3556] Switching to PCM or PES during channel start for
* HDMI/SPDIF.
* 
* Hydra_Software_Devel/5   10/21/08 9:46p speter
* PR 38950: [3548,3556] Removing WaitForStreamStability calls in HDMI In
* detection.
* 
* Hydra_Software_Devel/4   10/18/08 3:30a speter
* PR 47360: [3548,3556]: Changing the design of Spdif Rx. SPDIF Rx is now
* a device level interrupt and handling of digital input format change
* is moved from RAP PI to higher layer.
* 
* Hydra_Software_Devel/3   10/17/08 2:33a speter
* PR 47360: [3548,3556]: Changing the design of Spdif Rx. SPDIF Rx is now
* a device level interrupt and handling of digital input format change
* is moved from RAP PI to higher layer.
* 
* Hydra_Software_Devel/2   10/15/08 9:55p speter
* PR 38950: [3548,3556] Changing AAC SBR to AAC SBR ADTS for SPDIF In and
* HDMI In.
* 
* Hydra_Software_Devel/1   10/10/08 11:16a speter
* PR 38950: [3548,3556,7405,7325,7335] Moving references from base2 to
* base3
* 
* Hydra_Software_Devel/19   8/11/08 1:22p speter
* PR 44780: [3548,3556] Fixing the HDMI bug in chip version A0. Due to
* this the audio was switching to compressed and PCM even though the
* soure was PCM. Disabling HDMI compressed related interrupts and code
* for A0
* 
* Hydra_Software_Devel/18   7/21/08 12:19p speter
* PR 44984: [3548,3556,7405] Merging the changes from Rap_NewFwArch
* branch to Hydra_Software_Devel
* 
* Hydra_Software_Devel/Rap_NewFwArch/1   7/21/08 12:06p speter
* PR 44780: [3548,3556] Adding HDMI In (PCM) support
* 
* Hydra_Software_Devel/17   10/12/07 2:41p speter
* PR 33800: [3563] Issue because of simultaneous writes to SPDIF Rx
* Registers. Fixed by making them critical sections.
* 
* Hydra_Software_Devel/16   10/11/07 10:27a speter
* PR 34642: [3563] Support AAC+ in SPDIF In
* 
* Hydra_Software_Devel/15   10/10/07 4:18p speter
* PR 34642: [3563] Support AAC+ in SPDIF In
* 
* Hydra_Software_Devel/14   9/28/07 3:58p speter
* PR 32734: [3563] Fixing the HDMI detection in interrupts and
* GetInputFormatInfo API
* 
* Hydra_Software_Devel/13   9/17/07 3:09p speter
* PR 32734: [3563] HDMI stream type detection isolated from SPDIF
* detection
* 
* Hydra_Software_Devel/12   8/1/07 1:51p speter
* PR 28716: [3563] Spdif In Interrupts of Interest changed.
* RATE_MISMATCH_INTR dropped and RATE_CHANGE_INTR added
* 
* Hydra_Software_Devel/11   6/30/07 1:53a speter
* PR 32679: [3563] Fixing the Input Switching problem. When the input was
* switched from I2S and SPDIF In the audio was lost as both bad and good
* interrupts were occuring simultaenously.
* 
* Hydra_Software_Devel/9   6/6/07 12:09p speter
* PR 29881: [3563] Fixed the Get Status Info in the case of Blu Ray DVD
* player. The issue was that with Blu Ray DVD player the status changes
* from compressed to PCM and keeps toggling. So we need to depend on
* bValidPC to identify.
* 
* Hydra_Software_Devel/8   5/25/07 4:44p speter
* PR 28716: HDMI / SPDIF Fix for Blu Ray DVD Player. Also HDMI compressed
* was having problems, fixed it by allowing more interrupts to be
* considered for the autoswitch logic
* 
* Hydra_Software_Devel/7   5/24/07 8:04p speter
* PR 28716: Added HDMI Compressed Algo1 to Compressed Algo2 switching.
* HDMI Passthru also supported.
* 
* Hydra_Software_Devel/6   5/24/07 3:47p speter
* PR 28716: [3563] Implemented HDMI Compressed In and switching between
* HDMI PCM and Compressed. Also fixed issues related to sampling rate
* handling of SPDIF and HDMI. Modified WaitForStreamStabiity function to
* handle HDMI switching (PCM <-> Compressed)
* 
* Hydra_Software_Devel/5   5/3/07 4:59p speter
* PR 28716: [3563] SpdifRx, Input Spdif from Comp1->Comp2 switching
* support added. Major code revamp done in SpdifRx Module.
* 
* Hydra_Software_Devel/4   4/30/07 2:51p speter
* PR 30282: [3563] SPDIF Input - Switching between PCM and Compressed:
* Fixed the bug that stop-start-stop from application does not work.
* Also corrected the spdifrxstate variables from uint32 to enums.
* 
* Hydra_Software_Devel/3   4/27/07 6:41p speter
* PR 28715: SPDIF and HDMI interrupts needs to be handled separately
* based on the input type and the state machine has to give the states
* also based on the input type apart from the interrupt status
* 
* Hydra_Software_Devel/2   4/27/07 2:43p speter
* PR 28715: [3563] Implementation of SPDIF Rx, Switching between PCM and
* Compressed input and vice versa (Not applicable to HDMI as it supports
* only PCM as of now)
* 
* Hydra_Software_Devel/1   3/27/07 12:12p speter
* PR 28718: Adding separate file for SpdifRx related functions
* 
* 
***************************************************************************/
#include "brap.h"
#include "brap_priv.h"

BDBG_MODULE(rap_spdifrx);		/* Register software module with debug interface */

/*
TODO: Need to add comments for all the functions
*/

/* Macros for SPDIF_RCVR_CTRL.STATUS fields */
/* TODO: Need to remove these macros and use BCHP_GET_FIELD_DATA instead */
#define  CTRL_STATUS_BPC_VALID   	0x00008000
#define  CTRL_STATUS_GOOD_BIPHASE 	0x00000100
#define  CTRL_STATUS_VALIDITY     	0x00000060
#define  CTRL_STATUS_SAMPLE_RATE  	0x0000000f

/* 
There is a hardware bug in 3548 B0 that does not provide proper MAI FORMAT 
values. This workaround uses Channel Status Bits instead of MAI_FORMAT register
So the interrupt has to change to interrupt on Channel Status Bits instead of MAI FORMAT
and so is the logic that determines whether it is PCM or Compressed 
*/
#define HDMI_MAI_FORMAT_INCONSISTENCY_WORKAROUND 1

#define  INTERRUPTS_OF_INTEREST_FOR_SPDIF_INPUT		0x070E
#if (BRAP_3548_FAMILY == 1 && BCHP_VER==BCHP_VER_A0)
#define  INTERRUPTS_OF_INTEREST_FOR_HDMI_INPUT		0x0010
#elif (1 == HDMI_MAI_FORMAT_INCONSISTENCY_WORKAROUND)
#define  INTERRUPTS_OF_INTEREST_FOR_HDMI_INPUT		0x0590
#else
#define  INTERRUPTS_OF_INTEREST_FOR_HDMI_INPUT		0x0590
#endif

#define  BURST_PREAM_C_ALGO_ID						0x001f
#define  BURST_PREAM_C_PAYLOAD_MAY_CONTAIN_ERRORS	0x0080
#define  BURST_PREAM_C_DATA_TYPE_DEPENDENT_INFO		0x1f00
#define  BURST_PREAM_C_BIT_STREAM_NUMBER			0xD000






/* Prototypes of Internal Functions */
BERR_Code BRAP_SPDIFRX_P_WaitForStreamStability (
    BRAP_Handle hRap,                            /* [in] Rap handle */
    BRAP_P_SPDIFRX_InputFormat *psInputFormatInfo,  /* [out]Input stream info */
    uint32_t ui32Count,
    uint32_t ui32Delay,
    BRAP_SpdifRx_StreamType eExpectedStreamType
    );

static BRAP_P_SPDIFRX_InputState InferStreamType (
    BRAP_Handle hRap, 
    BRAP_CapInputPort eCapInputPort,                                    
    uint32_t ui32SpdifRxCtrlStatus, 
    uint32_t ui32SpdifRxCurrentEsrStatus
    );





BERR_Code 
BRAP_SPDIFRX_P_Open(
    BRAP_Handle         hRap,
    BRAP_CapInputPort	eSpdifRxInputPort
    )
{
    uint32_t ui32SpdifRxCtrlConfig = 0;
    uint32_t ui32FmmMiscReset = 0;
    uint32_t ui32SpdifRxEsrStatus = 0;    

	BDBG_ENTER(BRAP_SPDIFRX_P_Open);
	BDBG_ASSERT(hRap);
    BKNI_EnterCriticalSection();

    /* Mask all the interrupt for SPDIFRX */
    BRAP_Write32 (hRap->hRegister, BCHP_SPDIF_RCVR_ESR_MASK_SET, 0xFFFFFFFF);       

    /* First Read the current value in the register to retain defaults */
    ui32SpdifRxCtrlConfig = BRAP_Read32 (hRap->hRegister, BCHP_SPDIF_RCVR_CTRL_CONFIG);

    ui32SpdifRxCtrlConfig &= ~ ( (BCHP_MASK(SPDIF_RCVR_CTRL_CONFIG, INSERT_MODE))
                                |(BCHP_MASK(SPDIF_RCVR_CTRL_CONFIG, IGNORE_PERR_PCM))
                                |(BCHP_MASK(SPDIF_RCVR_CTRL_CONFIG, IGNORE_PERR_COMP))
                                |(BCHP_MASK(SPDIF_RCVR_CTRL_CONFIG, IGNORE_VALID_PCM))
                                |(BCHP_MASK(SPDIF_RCVR_CTRL_CONFIG, OUT_FORMAT_ENA))
                                |(BCHP_MASK(SPDIF_RCVR_CTRL_CONFIG, ALLOW_NZ_STUFFING))
                                |(BCHP_MASK(SPDIF_RCVR_CTRL_CONFIG, SOURCE_SEL)));

    /* 
        PR 35668: Some Blu Ray DVD Players send Non Zero values between compressed frames
        This was getting treated as PCM data and causing confusion in 3563 and a workaround was put.
        In 3548 this has been fixed in hardware. Enabling the bit here.
    */
    ui32SpdifRxCtrlConfig |= BCHP_FIELD_ENUM (SPDIF_RCVR_CTRL_CONFIG, ALLOW_NZ_STUFFING, Nonzero_OK);
    
    if (BRAP_CapInputPort_eSpdif == eSpdifRxInputPort)
    {
        /* Configure the SPDIF/HDMI Receiver using SPDIF params got using 
            SetInputConfig here */            
        ui32SpdifRxCtrlConfig |= BCHP_FIELD_ENUM (SPDIF_RCVR_CTRL_CONFIG, SOURCE_SEL, SPDIF);
        ui32SpdifRxCtrlConfig |= BCHP_FIELD_DATA (SPDIF_RCVR_CTRL_CONFIG, INSERT_MODE, 
            hRap->sInputSettings[BRAP_CapInputPort_eSpdif].sCapPortParams.uCapPortParams.sSpdifRxParams.eInsertMode);
        ui32SpdifRxCtrlConfig |= BCHP_FIELD_DATA (SPDIF_RCVR_CTRL_CONFIG, IGNORE_PERR_PCM, 
            hRap->sInputSettings[BRAP_CapInputPort_eSpdif].sCapPortParams.uCapPortParams.sSpdifRxParams.bIgnorePcmParityErr);
        ui32SpdifRxCtrlConfig |= BCHP_FIELD_DATA (SPDIF_RCVR_CTRL_CONFIG, IGNORE_PERR_COMP,                 
            hRap->sInputSettings[BRAP_CapInputPort_eSpdif].sCapPortParams.uCapPortParams.sSpdifRxParams.bIgnoreCompParityErr);
        ui32SpdifRxCtrlConfig |= BCHP_FIELD_DATA (SPDIF_RCVR_CTRL_CONFIG, IGNORE_VALID_PCM,                 
            hRap->sInputSettings[BRAP_CapInputPort_eSpdif].sCapPortParams.uCapPortParams.sSpdifRxParams.bIgnoreValidity);

	    /* UnMask the required interrupts for SPDIF Input alone. */
	    BRAP_Write32 (hRap->hRegister, BCHP_SPDIF_RCVR_ESR_MASK_CLEAR, INTERRUPTS_OF_INTEREST_FOR_SPDIF_INPUT);   

		/* Set the output format ena to PCM */
		ui32SpdifRxCtrlConfig |= BCHP_FIELD_ENUM (SPDIF_RCVR_CTRL_CONFIG, OUT_FORMAT_ENA, PCM); 	
		
    }
    else if (BRAP_CapInputPort_eHdmi == eSpdifRxInputPort)
    {
        /* Configure the SPDIF/HDMI Receiver using HDMI params got using 
            SetInputConfig here */  
        ui32SpdifRxCtrlConfig &= ~(BCHP_MASK(SPDIF_RCVR_CTRL_CONFIG, HDMI_SEL));
    
        ui32SpdifRxCtrlConfig |= BCHP_FIELD_ENUM (SPDIF_RCVR_CTRL_CONFIG, SOURCE_SEL, MAI);
        ui32SpdifRxCtrlConfig |= BCHP_FIELD_DATA (SPDIF_RCVR_CTRL_CONFIG, HDMI_SEL,             
            hRap->sInputSettings[BRAP_CapInputPort_eHdmi].sCapPortParams.uCapPortParams.sHdmiRxParams.eHdmiNo);
        ui32SpdifRxCtrlConfig |= BCHP_FIELD_DATA (SPDIF_RCVR_CTRL_CONFIG, INSERT_MODE,                 
            hRap->sInputSettings[BRAP_CapInputPort_eHdmi].sCapPortParams.uCapPortParams.sHdmiRxParams.eInsertMode);
        ui32SpdifRxCtrlConfig |= BCHP_FIELD_DATA (SPDIF_RCVR_CTRL_CONFIG, IGNORE_PERR_PCM,                 
            hRap->sInputSettings[BRAP_CapInputPort_eHdmi].sCapPortParams.uCapPortParams.sHdmiRxParams.bIgnorePcmParityErr);
        ui32SpdifRxCtrlConfig |= BCHP_FIELD_DATA (SPDIF_RCVR_CTRL_CONFIG, IGNORE_PERR_COMP,                 
            hRap->sInputSettings[BRAP_CapInputPort_eHdmi].sCapPortParams.uCapPortParams.sHdmiRxParams.bIgnoreCompParityErr);
        ui32SpdifRxCtrlConfig |= BCHP_FIELD_DATA (SPDIF_RCVR_CTRL_CONFIG, IGNORE_VALID_PCM, 
            hRap->sInputSettings[BRAP_CapInputPort_eHdmi].sCapPortParams.uCapPortParams.sHdmiRxParams.bIgnoreValidity);

	    /* UnMask the required interrupts for HDMI Input alone. */
	    BRAP_Write32 (hRap->hRegister, BCHP_SPDIF_RCVR_ESR_MASK_CLEAR, INTERRUPTS_OF_INTEREST_FOR_HDMI_INPUT);    

	    /* Set the output format ena to PCM*/
	    ui32SpdifRxCtrlConfig |= BCHP_FIELD_ENUM (SPDIF_RCVR_CTRL_CONFIG, OUT_FORMAT_ENA, PCM);		
		
    }
    


    /* Writing the Control Config register. The Receiver will then interrupt 
        which will be hanlded by the interrupt module and then notified to the 
        application through a call back from  where the BRAP_ProcessDigitalInputFormatChange 
        ia called. Only there the actual DSP start happens */
    BRAP_Write32 (hRap->hRegister, BCHP_SPDIF_RCVR_CTRL_CONFIG, ui32SpdifRxCtrlConfig);

    /* Setting the threshold for Good FMT interrupt. This needs to be set to 1.
    H/w people have confirmed that this will be fixed in C0 so we can select other values also */
    BRAP_Write32 (hRap->hRegister, BCHP_SPDIF_RCVR_CTRL_INTR_THRESHOLD, 0x01);

    ui32FmmMiscReset = BRAP_Read32 (hRap->hRegister, BCHP_AUD_FMM_MISC_RESET);
    ui32FmmMiscReset &= ~ (BCHP_MASK(AUD_FMM_MISC_RESET, RESET_SPDIFRX_LOGIC_B));                
    
    ui32FmmMiscReset |= (BCHP_FIELD_ENUM (AUD_FMM_MISC_RESET, RESET_SPDIFRX_LOGIC_B, Reset));
    BRAP_Write32 (hRap->hRegister, BCHP_AUD_FMM_MISC_RESET, ui32FmmMiscReset);

    ui32FmmMiscReset |= (BCHP_FIELD_ENUM (AUD_FMM_MISC_RESET, RESET_SPDIFRX_LOGIC_B, Inactive));
    BRAP_Write32 (hRap->hRegister, BCHP_AUD_FMM_MISC_RESET, ui32FmmMiscReset); 

    ui32SpdifRxEsrStatus = BRAP_Read32_isr (hRap->hRegister, BCHP_SPDIF_RCVR_ESR_STATUS);
	BDBG_MSG (("BCHP_SPDIF_RCVR_ESR_STATUS = 0x%x", ui32SpdifRxEsrStatus));
    BKNI_LeaveCriticalSection();
    
	BDBG_LEAVE(BRAP_SPDIFRX_P_Open);    

    return BERR_SUCCESS;
}


BERR_Code 
BRAP_SPDIFRX_P_Close(
    BRAP_Handle         hRap
    )
{
	BDBG_ENTER(BRAP_SPDIFRX_P_Close);
	BDBG_ASSERT(hRap);

    /* Mask all the interrupt for SPDIFRX */
    BRAP_Write32 (hRap->hRegister, BCHP_SPDIF_RCVR_ESR_MASK_SET, 0xFFFFFFFF);       

    
	BDBG_LEAVE(BRAP_SPDIFRX_P_Close);    

    return BERR_SUCCESS;
}

BERR_Code 
BRAP_SPDIFRX_P_Start(
    BRAP_Handle          hRap
    )
{
    uint32_t ui32SpdifRxCtrlConfig = 0;

	BDBG_ENTER(BRAP_SPDIFRX_P_Start);
	BDBG_ASSERT(hRap);

    /* First Read the current value in the register to retain defaults */
    ui32SpdifRxCtrlConfig = BRAP_Read32 (hRap->hRegister, BCHP_SPDIF_RCVR_CTRL_CONFIG);
    ui32SpdifRxCtrlConfig &= ~BCHP_MASK(SPDIF_RCVR_CTRL_CONFIG, OUTFIFO_ENA);
    
    /* Enable the SPDIF/HDMI Receiver */
    ui32SpdifRxCtrlConfig |= BCHP_FIELD_ENUM (SPDIF_RCVR_CTRL_CONFIG, OUTFIFO_ENA, Enable);    
    
    /* Write to the Register */
    BRAP_Write32 (hRap->hRegister, BCHP_SPDIF_RCVR_CTRL_CONFIG, ui32SpdifRxCtrlConfig);


	BDBG_LEAVE(BRAP_SPDIFRX_P_Start);  

    return BERR_SUCCESS;
}    


BERR_Code 
BRAP_SPDIFRX_P_Stop(
    BRAP_Handle          hRap
    )
{
    uint32_t ui32SpdifRxCtrlConfig = 0;

	BDBG_ENTER(BRAP_SPDIFRX_P_Stop);
	BDBG_ASSERT(hRap);    

    /* First Read the current value in the register to retain defaults */
    ui32SpdifRxCtrlConfig = BRAP_Read32 (hRap->hRegister, BCHP_SPDIF_RCVR_CTRL_CONFIG);
    ui32SpdifRxCtrlConfig &= ~BCHP_MASK(SPDIF_RCVR_CTRL_CONFIG, OUTFIFO_ENA);
    
    /* Enable the SPDIF/HDMI Receiver */
    ui32SpdifRxCtrlConfig |= BCHP_FIELD_ENUM (SPDIF_RCVR_CTRL_CONFIG, OUTFIFO_ENA, Disable);    
    
    /* Write to the Register */
    BRAP_Write32 (hRap->hRegister, BCHP_SPDIF_RCVR_CTRL_CONFIG, ui32SpdifRxCtrlConfig);

	BDBG_LEAVE(BRAP_SPDIFRX_P_Stop);

    return BERR_SUCCESS;
}    


BERR_Code 
BRAP_SPDIFRX_P_GetChannelStatus (
    BRAP_Handle          hRap,
    BRAP_SpdifRx_ChannelStatus  *pChannelStatus
    )
{
    uint32_t ui32RegVal = 0;
    
	BDBG_ENTER(BRAP_SPDIFRX_P_GetChannelStatus);
	BDBG_ASSERT(hRap);
	BDBG_ASSERT(pChannelStatus);

    /* Set HOLD to disable the update to channel status registers */
    ui32RegVal  = BRAP_Read32 (hRap->hRegister, BCHP_SPDIF_RCVR_CTRL_CHAN_STAT_CTRL);
    ui32RegVal &= ~(BCHP_MASK (SPDIF_RCVR_CTRL_CHAN_STAT_CTRL, HOLD));
    ui32RegVal |= BCHP_FIELD_ENUM (SPDIF_RCVR_CTRL_CHAN_STAT_CTRL, HOLD, No_update);
    BRAP_Write32 (hRap->hRegister, BCHP_SPDIF_RCVR_CTRL_CHAN_STAT_CTRL, ui32RegVal);

    /* Get the SPDIF Receiver Channel Status0 */
    pChannelStatus->aui32LtChnStatus[0]  = BRAP_Read32 (hRap->hRegister, BCHP_SPDIF_RCVR_CTRL_CHAN_STAT_L0);
    pChannelStatus->aui32RtChnStatus[0]  = BRAP_Read32 (hRap->hRegister, BCHP_SPDIF_RCVR_CTRL_CHAN_STAT_R0);
    /* Get the SPDIF Receiver Channel Status1 */
    pChannelStatus->aui32LtChnStatus[1] = BRAP_Read32 (hRap->hRegister, BCHP_SPDIF_RCVR_CTRL_CHAN_STAT1) & 0x00FF;
    pChannelStatus->aui32RtChnStatus[1] = BRAP_Read32 (hRap->hRegister, BCHP_SPDIF_RCVR_CTRL_CHAN_STAT1) & 0xFF00;

    /* Disable HOLD to enable the update to channel status registers */
    ui32RegVal  = BRAP_Read32 (hRap->hRegister, BCHP_SPDIF_RCVR_CTRL_CHAN_STAT_CTRL);
    ui32RegVal &= ~(BCHP_MASK (SPDIF_RCVR_CTRL_CHAN_STAT_CTRL, HOLD));
    ui32RegVal |= BCHP_FIELD_ENUM (SPDIF_RCVR_CTRL_CHAN_STAT_CTRL, HOLD, Update);
    BRAP_Write32 (hRap->hRegister, BCHP_SPDIF_RCVR_CTRL_CHAN_STAT_CTRL, ui32RegVal);   

 	BDBG_LEAVE(BRAP_SPDIFRX_P_GetChannelStatus);

    return BERR_SUCCESS;
}


BERR_Code 
BRAP_SPDIFRX_P_GetRxStatus(
    BRAP_Handle          hRap,
    BRAP_SpdifRx_Status         *pStatus
    )
{
    uint32_t ui32ReceiverStatus = 0;
    uint32_t ui32BurstPreamble = 0;
    BRAP_P_SPDIFRX_InputFormat sInputFormatInfo;
    
	BDBG_ENTER(BRAP_SPDIFRX_P_GetRxStatus);
	BDBG_ASSERT(hRap);        
    BDBG_ASSERT(pStatus);     

    ui32ReceiverStatus = BRAP_Read32 (hRap->hRegister, BCHP_SPDIF_RCVR_CTRL_STATUS);
    
    /* Get the Input format information */
    BRAP_SPDIFRX_P_GetInputFormatInfo (hRap, &sInputFormatInfo);

    /* Populate the pStatus structure using ui32ReceiverStatus and sInputFormatInfo */

    /* Stream Type */
    pStatus->eStreamType = sInputFormatInfo.eStreamType;
    
    /* GoodBiPhase, ValidBPC, Signal Present, Valid Left and Right are directly filled */
    pStatus->bGoodBiphase = BCHP_GET_FIELD_DATA(ui32ReceiverStatus, SPDIF_RCVR_CTRL_STATUS, GOOD_BIPHASE);
    pStatus->bValidPC = BCHP_GET_FIELD_DATA(ui32ReceiverStatus, SPDIF_RCVR_CTRL_STATUS, BPC_VALID);
    pStatus->bSignalPresent = BCHP_GET_FIELD_DATA(ui32ReceiverStatus, SPDIF_RCVR_CTRL_STATUS, SIGNAL_PRESENT);
    pStatus->bValidLeft = BCHP_GET_FIELD_DATA(ui32ReceiverStatus, SPDIF_RCVR_CTRL_STATUS, VALIDITY_L);
    pStatus->bValidRight = BCHP_GET_FIELD_DATA(ui32ReceiverStatus, SPDIF_RCVR_CTRL_STATUS, VALIDITY_R);

	/* Even if bValid is false we still have information of Sampling rate and Algorithm type from BRAP_SPDIFRX_P_GetInputFormatInfo */
	pStatus->sPc.eAudioType = sInputFormatInfo.eType;
	pStatus->eSampleRate = sInputFormatInfo.eSampleRate;
	
    if (pStatus->bValidPC)
    {
        ui32BurstPreamble = BCHP_GET_FIELD_DATA(ui32ReceiverStatus, SPDIF_RCVR_CTRL_STATUS, BURST_PREAM_C);

        /* Now start filling the remaining fieds of sPc Structure */
        pStatus->sPc.bPayloadMayContainErrors = ui32BurstPreamble & BURST_PREAM_C_PAYLOAD_MAY_CONTAIN_ERRORS;
        pStatus->sPc.uiDataTypeDependentInfo = ui32BurstPreamble & BURST_PREAM_C_DATA_TYPE_DEPENDENT_INFO;
        pStatus->sPc.uiBitstreamNumber = ui32BurstPreamble & BURST_PREAM_C_BIT_STREAM_NUMBER;
    }    

    BDBG_LEAVE(BRAP_SPDIFRX_P_GetRxStatus);

    return BERR_SUCCESS;
}


BERR_Code 
	BRAP_SPDIFRX_P_GetInputFormatInfo(
    BRAP_Handle          hRap,                 
    BRAP_P_SPDIFRX_InputFormat *psInputFormatInfo   
    )
{
    uint32_t    ui32ReceiverStatus = 0, ui32SpdifRxCtrlConfig = 0;
#if !((BRAP_3548_FAMILY == 1 && BCHP_VER==BCHP_VER_A0)) 
#if (1 == HDMI_MAI_FORMAT_INCONSISTENCY_WORKAROUND)
    uint32_t    ui32MAIFormat = 0, ui32ChanStatus = 0;
#else
    uint32_t    ui32MAIFormat = 0, ui32MAIAudioFormat = 0;
#endif
#endif

    uint32_t    ui32BurstPreamble = 0, ui32SourceSelected = 0;    
    bool        bValidPC = false;

    BDBG_ENTER(BRAP_SPDIFRX_P_GetInputFormatInfo);
	BDBG_ASSERT(hRap);        
    BDBG_ASSERT(psInputFormatInfo);   

	ui32SpdifRxCtrlConfig = BRAP_Read32 (hRap->hRegister, BCHP_SPDIF_RCVR_CTRL_CONFIG);
	ui32SourceSelected  = BCHP_GET_FIELD_DATA(ui32SpdifRxCtrlConfig, SPDIF_RCVR_CTRL_CONFIG, SOURCE_SEL);	

    switch (ui32SourceSelected)
    {
        case BCHP_SPDIF_RCVR_CTRL_CONFIG_SOURCE_SEL_MAI:
#if (BRAP_3548_FAMILY == 1 && BCHP_VER==BCHP_VER_A0)
		    psInputFormatInfo->eStreamType = BRAP_SpdifRx_StreamType_eLinear;
            psInputFormatInfo->eType = BRAP_DSPCHN_AudioType_ePcm;
            psInputFormatInfo->eSampleRate = BAVC_AudioSamplingRate_e48k;
            break;
#elif (1 == HDMI_MAI_FORMAT_INCONSISTENCY_WORKAROUND)
            ui32ChanStatus = BRAP_Read32 (hRap->hRegister, BCHP_SPDIF_RCVR_CTRL_CHAN_STAT_L0);
            ui32MAIFormat = BRAP_Read32 (hRap->hRegister, BCHP_SPDIF_RCVR_CTRL_MAI_FORMAT);

			switch (BCHP_GET_FIELD_DATA(ui32MAIFormat, SPDIF_RCVR_CTRL_MAI_FORMAT, SAMPLE_RATE))
			{
				case 7:
					psInputFormatInfo->eSampleRate = BAVC_AudioSamplingRate_e32k;
					break;

				case 8:
					psInputFormatInfo->eSampleRate = BAVC_AudioSamplingRate_e44_1k;
					break;

				case 9:
					psInputFormatInfo->eSampleRate = BAVC_AudioSamplingRate_e48k;
					break;

				case 12:
					psInputFormatInfo->eSampleRate = BAVC_AudioSamplingRate_e96k;
					break;

				default:
                    psInputFormatInfo->eSampleRate = BRAP_INVALID_VALUE;
					break;
						
			}
            
        	if ((ui32ChanStatus & 2) == 2)
    	    {
    	        /* Compressed */
    		    BDBG_MSG (("Hdmi In StreamType is Compressed"));
    		    psInputFormatInfo->eStreamType = BRAP_SpdifRx_StreamType_eCompressed;
                psInputFormatInfo->eType = BRAP_DSPCHN_AudioType_eInvalid;
    	    }
            else
            {
                /* PCM */
    		    BDBG_MSG (("Hdmi In StreamType is PCM"));
    		    psInputFormatInfo->eStreamType = BRAP_SpdifRx_StreamType_eLinear;
                psInputFormatInfo->eType = BRAP_DSPCHN_AudioType_ePcm;
            }
            break;
#else
    	    ui32MAIFormat = BRAP_Read32 (hRap->hRegister, BCHP_SPDIF_RCVR_CTRL_MAI_FORMAT);
    	    ui32MAIAudioFormat = BCHP_GET_FIELD_DATA(ui32MAIFormat, SPDIF_RCVR_CTRL_MAI_FORMAT, AUDIO_FORMAT);

			switch (BCHP_GET_FIELD_DATA(ui32MAIFormat, SPDIF_RCVR_CTRL_MAI_FORMAT, SAMPLE_RATE))
			{
				case 7:
					psInputFormatInfo->eSampleRate = BAVC_AudioSamplingRate_e32k;
					break;

				case 8:
					psInputFormatInfo->eSampleRate = BAVC_AudioSamplingRate_e44_1k;
					break;

				case 9:
					psInputFormatInfo->eSampleRate = BAVC_AudioSamplingRate_e48k;
					break;

				case 12:
					psInputFormatInfo->eSampleRate = BAVC_AudioSamplingRate_e96k;
					break;

				default:
                    psInputFormatInfo->eSampleRate = BRAP_INVALID_VALUE;
					break;
						
			}            

    		if ( BCHP_SPDIF_RCVR_CTRL_MAI_FORMAT_AUDIO_FORMAT_SPDIF_linearPCM == ui32MAIAudioFormat )
    		{
    		    BDBG_MSG (("Hdmi In StreamType is PCM"));
    		    psInputFormatInfo->eStreamType = BRAP_SpdifRx_StreamType_eLinear;
                psInputFormatInfo->eType = BRAP_DSPCHN_AudioType_ePcm;
    		}
    		else if ( BCHP_SPDIF_RCVR_CTRL_MAI_FORMAT_AUDIO_FORMAT_SPDIF_compressed == ui32MAIAudioFormat )
    		{
    		    BDBG_MSG (("Hdmi In StreamType is Compressed"));
    		    psInputFormatInfo->eStreamType = BRAP_SpdifRx_StreamType_eCompressed;
                psInputFormatInfo->eType = BRAP_DSPCHN_AudioType_eInvalid;
    		}
            break;
#endif                   
    
        case BCHP_SPDIF_RCVR_CTRL_CONFIG_SOURCE_SEL_SPDIF:
            ui32ReceiverStatus = BRAP_Read32 (hRap->hRegister, BCHP_SPDIF_RCVR_CTRL_STATUS);
            psInputFormatInfo->eStreamType = BCHP_GET_FIELD_DATA(ui32ReceiverStatus, SPDIF_RCVR_CTRL_STATUS, STREAM_TYPE);
            BDBG_MSG (("Spdif In StreamType is : %s", (psInputFormatInfo->eStreamType?"COMPRESSED":"PCM")));

            switch (BCHP_GET_FIELD_DATA(ui32ReceiverStatus, SPDIF_RCVR_CTRL_STATUS, SAMPLE_RATE))
	        {
	            case 0:
	                psInputFormatInfo->eSampleRate = BAVC_AudioSamplingRate_e16k;
	                break;

	            case 1:
	                psInputFormatInfo->eSampleRate = BAVC_AudioSamplingRate_e22_05k;
	                break;

	            case 2:
	                psInputFormatInfo->eSampleRate = BAVC_AudioSamplingRate_e24k;
	                break;

	            case 3:
	                psInputFormatInfo->eSampleRate = BAVC_AudioSamplingRate_e32k;
	                break;

	            case 4:
	                psInputFormatInfo->eSampleRate = BAVC_AudioSamplingRate_e44_1k;
	                break;

	            case 5:
	                psInputFormatInfo->eSampleRate = BAVC_AudioSamplingRate_e48k;
	                break;

	            case 6:
	                psInputFormatInfo->eSampleRate = BAVC_AudioSamplingRate_e64k;
	                break;

	            case 7:
	                psInputFormatInfo->eSampleRate = BAVC_AudioSamplingRate_e88_2k;
	                break;

	            case 8:
	                psInputFormatInfo->eSampleRate = BAVC_AudioSamplingRate_e96k;
	                break;

	            case 9:
	                psInputFormatInfo->eSampleRate = BAVC_AudioSamplingRate_e128k;
	                break;

	            case 10:
	                psInputFormatInfo->eSampleRate = BAVC_AudioSamplingRate_e176_4k;
	                break;

	            case 11:
	                psInputFormatInfo->eSampleRate = BAVC_AudioSamplingRate_e192k;
	                break;

	            default:
                    psInputFormatInfo->eSampleRate = BRAP_INVALID_VALUE;
	                break;    
            }

            if (BRAP_SpdifRx_StreamType_eLinear == psInputFormatInfo->eStreamType)
            {
                psInputFormatInfo->eType = BRAP_DSPCHN_AudioType_ePcm;
            }
            else if (BRAP_SpdifRx_StreamType_eCompressed == psInputFormatInfo->eStreamType)
            {
               /* Algorithm is made as Invalid. Will be updated if bValidPC is true */
                psInputFormatInfo->eType = BRAP_DSPCHN_AudioType_eInvalid;                
            }
            break;

        default:
            BDBG_ERR (("SpdifRx :: Input is not SPDIF or HDMI"));
            return BERR_TRACE (BERR_NOT_SUPPORTED);
    }
    ui32ReceiverStatus = BRAP_Read32 (hRap->hRegister, BCHP_SPDIF_RCVR_CTRL_STATUS);
	bValidPC = BCHP_GET_FIELD_DATA(ui32ReceiverStatus, SPDIF_RCVR_CTRL_STATUS, BPC_VALID);	
	BDBG_MSG (("SpdifRx bValidPC is %d", bValidPC));

    if (BRAP_SpdifRx_StreamType_eCompressed == psInputFormatInfo->eStreamType)
    {
        if (bValidPC)
    	{
    		ui32BurstPreamble = BCHP_GET_FIELD_DATA(ui32ReceiverStatus, SPDIF_RCVR_CTRL_STATUS, BURST_PREAM_C);
    		
            /* Audio type */
            switch (ui32BurstPreamble & BURST_PREAM_C_ALGO_ID)
            {
                case 1:
                    psInputFormatInfo->eType = BRAP_DSPCHN_AudioType_eAc3;
    				BDBG_MSG (("SpdifRx: AC3 Algorithm !!!"));
                    break;

                case 7:
                case 20:
                    psInputFormatInfo->eType = BRAP_DSPCHN_AudioType_eAac;
                    BDBG_MSG (("SpdifRx: AAC Algorithm !!!"));
                    break;
                case 52:
                    psInputFormatInfo->eType = BRAP_DSPCHN_AudioType_eAacSbrAdts;
                    BDBG_MSG (("SpdifRx: AAC-HE ADTS Algorithm !!!"));
                    break;
                    
                case 4:
                case 5:
                case 6:
                case 8:
                case 9:
                case 10:
                    psInputFormatInfo->eType = BRAP_DSPCHN_AudioType_eMpeg;
    				BDBG_MSG (("SpdifRx: MPEG Algorithm !!!"));
                    break;

                case 11:
                case 12:
                case 13:
                    psInputFormatInfo->eType = BRAP_DSPCHN_AudioType_eDtshd;
    				BDBG_MSG (("SpdifRx: DTS Algorithm !!!"));
                    break;

                default:
                    break;
            }  	
    	}
        else
        {
            psInputFormatInfo->eType = BRAP_DSPCHN_AudioType_eInvalid;
            BDBG_WRN(("BRAP_SPDIFRX_P_GetInputFormatInfo: Audio Algorithm is Invalid!"));
        }        
    }

    BDBG_LEAVE(BRAP_SPDIFRX_P_GetInputFormatInfo);    

    return BERR_SUCCESS;
}

/**********************************************************************
Summary:
   Process the stream type information and wait till the steam type is stable

Description:
    This function was used by the application earlier to identify if the stream is stable.
    But since the input detect logic below also confirms stream stability, it is not 
    used anymore.

Returns:
   BERR_SUCCESS 

See Also:
   None
**********************************************************************/
BERR_Code BRAP_SPDIFRX_P_WaitForStreamStability (
    BRAP_Handle hRap,                            /* [in] Rap handle */
    BRAP_P_SPDIFRX_InputFormat *psInputFormatInfo,  /* [out]Input stream info */
    uint32_t ui32Count,
    uint32_t ui32Delay,
    BRAP_SpdifRx_StreamType eExpectedStreamType
    )
{
    BRAP_SpdifRx_StreamType     eOldStreamType;
    uint32_t                    ui32LoopCount = 0;
        
	BDBG_ENTER(BRAP_SPDIFRX_P_WaitForStreamStability);

	BDBG_ASSERT(hRap);
	BDBG_ASSERT(psInputFormatInfo);

    /* Store the value that comes in as Old Stream Type */
    eOldStreamType = psInputFormatInfo->eStreamType;
    ui32LoopCount = ui32Count;
    
    while (ui32LoopCount)
    {
        BKNI_Sleep (ui32Delay);
        BRAP_SPDIFRX_P_GetInputFormatInfo (hRap, psInputFormatInfo);
        BDBG_MSG (("The count is %d", ui32LoopCount));
        
        if (eOldStreamType == psInputFormatInfo->eStreamType)
        {
            ui32LoopCount--;

			if ( (eExpectedStreamType == psInputFormatInfo->eStreamType) &&
				 (psInputFormatInfo->eType != BRAP_DSPCHN_AudioType_eInvalid)
			   )
			{
				/* You have obtained a valid stream of the requested type */
				break;
			}
        }
        else
        {
            ui32LoopCount = ui32Count;
        }
        eOldStreamType = psInputFormatInfo->eStreamType;
    }
        
    BDBG_LEAVE(BRAP_SPDIFRX_P_WaitForStreamStability);
    
    return BERR_SUCCESS;
}


bool 
BRAP_SPDIFRX_P_DetectInputChange (
    BRAP_Handle hRap, 
    BRAP_P_SPDIFRX_DetectChange_InputParams     *psInputParams,
    BRAP_P_SPDIFRX_DetectChange_OutputParams    *psOutputParams
    )
{
    uint32_t ui32SpdifRxCtrlStatus = 0;
    bool bChangeNeedsFirmwareDownload = false;
	uint32_t ui32SpdifRxCtrlConfig = 0;

	BDBG_ENTER(BRAP_SPDIFRX_P_DetectInputChange);

	BDBG_ASSERT(psInputParams);
	BDBG_ASSERT(psOutputParams);    

    BDBG_MSG (("Detect Input Change Logic ---- Input Params begin ----"));
    BDBG_MSG (("eCapInputPort : %d", psInputParams->eCapInputPort));
    BDBG_MSG (("eCurrentState : %x", psInputParams->eCurrentState));
    BDBG_MSG (("ui32CurrentCompState : %x", psInputParams->ui32CurrentCompState));
    BDBG_MSG (("ui32CurrentEsrStatus : %x", psInputParams->ui32CurrentEsrStatus));
    BDBG_MSG (("Detect Input Change Logic ---- Input Params end ----"));

	/* Initialize the output states as that of input */
	psOutputParams->eNewState = psInputParams->eCurrentState;
	psOutputParams->ui32NewCompState = psInputParams->ui32CurrentCompState;	
	BRAP_SPDIFRX_P_GetInputFormatInfo (hRap, &(psOutputParams->sInputFormatInfo));
    
    ui32SpdifRxCtrlStatus = BRAP_Read32 (hRap->hRegister, BCHP_SPDIF_RCVR_CTRL_STATUS);
	

    switch (psInputParams->eCapInputPort)
    {
        case BRAP_CapInputPort_eHdmi:
			if (psInputParams->ui32CurrentEsrStatus & INTERRUPTS_OF_INTEREST_FOR_HDMI_INPUT)
			{
				psOutputParams->eNewState = InferStreamType(
												hRap, 
												psInputParams->eCapInputPort,									 
												ui32SpdifRxCtrlStatus, 
												psInputParams->ui32CurrentEsrStatus
												);

				switch (psInputParams->eCurrentState)
				{
					case BRAP_P_SPDIFRX_InputState_eStreamNone:
						if (BRAP_P_SPDIFRX_InputState_eStreamHdmiPCM == psOutputParams->eNewState)
						{
							BDBG_MSG (("HDMI None -> PCM"));
			    			BRAP_SPDIFRX_P_SwitchToPCM(hRap);
			                BRAP_SPDIFRX_P_GetInputFormatInfo (hRap, &(psOutputParams->sInputFormatInfo));
							bChangeNeedsFirmwareDownload = true;
			    			BDBG_MSG (("(1h) Switched to HDMI PCM"));			
							BDBG_MSG (("The Input Change in HDMI In requires a firmware download"));														
						}
						else if (BRAP_P_SPDIFRX_InputState_eStreamGoodCompress == psOutputParams->eNewState)
						{
							BDBG_MSG (("HDMI None -> Compressed"));						
			       			BDBG_MSG (("Switching to PES - HDMI Compressed "));
						    ui32SpdifRxCtrlConfig = BRAP_Read32 (hRap->hRegister, BCHP_SPDIF_RCVR_CTRL_CONFIG);
						    ui32SpdifRxCtrlConfig &= ~ (BCHP_MASK(SPDIF_RCVR_CTRL_CONFIG, OUT_FORMAT_ENA));  
						    ui32SpdifRxCtrlConfig |= BCHP_FIELD_ENUM (SPDIF_RCVR_CTRL_CONFIG, OUT_FORMAT_ENA, PES);
						    BRAP_Write32 (hRap->hRegister, BCHP_SPDIF_RCVR_CTRL_CONFIG, ui32SpdifRxCtrlConfig);

			                /*BRAP_SPDIFRX_P_WaitForStreamStability (hRap,&(psOutputParams->sInputFormatInfo),10,100, BRAP_SpdifRx_StreamType_eCompressed);*/
							/* Get the latest Register value */
							ui32SpdifRxCtrlStatus = BRAP_Read32 (hRap->hRegister, BCHP_SPDIF_RCVR_CTRL_STATUS);
							
   						    psOutputParams->ui32NewCompState = (BCHP_GET_FIELD_DATA(ui32SpdifRxCtrlStatus, SPDIF_RCVR_CTRL_STATUS, BURST_PREAM_C));
							if (BRAP_SpdifRx_StreamType_eCompressed == psOutputParams->sInputFormatInfo.eStreamType)
							{
								bChangeNeedsFirmwareDownload = true;						
								BDBG_MSG (("The Input Change in HDMI In requires a firmware download"));							
							}							
						}
						break;

					case BRAP_P_SPDIFRX_InputState_eStreamHdmiPCM:
						if (BRAP_P_SPDIFRX_InputState_eStreamNone == psOutputParams->eNewState)
						{
							BDBG_MSG (("HDMI PCM -> None"));												
			    			BRAP_SPDIFRX_P_SwitchToNone(hRap);
			    			BDBG_MSG(( "(3h) Switched to HDMI NoInput"));						
						}
						else if (BRAP_P_SPDIFRX_InputState_eStreamGoodCompress == psOutputParams->eNewState)
						{
						
							BDBG_MSG (("HDMI PCM -> Compressed"));																		
			       			BDBG_MSG (("Switching to PES - HDMI Compressed"));
						    ui32SpdifRxCtrlConfig = BRAP_Read32 (hRap->hRegister, BCHP_SPDIF_RCVR_CTRL_CONFIG);
						    ui32SpdifRxCtrlConfig &= ~ (BCHP_MASK(SPDIF_RCVR_CTRL_CONFIG, OUT_FORMAT_ENA));  
						    ui32SpdifRxCtrlConfig |= BCHP_FIELD_ENUM (SPDIF_RCVR_CTRL_CONFIG, OUT_FORMAT_ENA, PES);
						    BRAP_Write32 (hRap->hRegister, BCHP_SPDIF_RCVR_CTRL_CONFIG, ui32SpdifRxCtrlConfig);
			                /*BRAP_SPDIFRX_P_WaitForStreamStability (hRap,&(psOutputParams->sInputFormatInfo),10,100, BRAP_SpdifRx_StreamType_eCompressed);*/
							/* Get the latest Register value */
							ui32SpdifRxCtrlStatus = BRAP_Read32 (hRap->hRegister, BCHP_SPDIF_RCVR_CTRL_STATUS);
   						    psOutputParams->ui32NewCompState = (BCHP_GET_FIELD_DATA(ui32SpdifRxCtrlStatus, SPDIF_RCVR_CTRL_STATUS, BURST_PREAM_C));							
							if (BRAP_SpdifRx_StreamType_eCompressed == psOutputParams->sInputFormatInfo.eStreamType)
							{
								bChangeNeedsFirmwareDownload = true;						
								BDBG_MSG (("The Input Change in HDMI In requires a firmware download"));							
							}
						}
						break;

					case BRAP_P_SPDIFRX_InputState_eStreamGoodCompress:
						if (BRAP_P_SPDIFRX_InputState_eStreamNone == psOutputParams->eNewState)
						{
							BDBG_MSG (("HDMI Compressed -> None"));																									
			    			BRAP_SPDIFRX_P_SwitchToNone(hRap);
			    			BDBG_MSG(( "(3h) Switched to HDMI NoInput"));						
						}
						else if (BRAP_P_SPDIFRX_InputState_eStreamHdmiPCM == psOutputParams->eNewState)
						{
							BDBG_MSG (("HDMI Compressed -> PCM"));																														
			    			BRAP_SPDIFRX_P_SwitchToPCM(hRap);
			                BRAP_SPDIFRX_P_GetInputFormatInfo (hRap, &(psOutputParams->sInputFormatInfo));
							bChangeNeedsFirmwareDownload = true;
			    			BDBG_MSG (("(1h) Switched to HDMI PCM"));		
							BDBG_MSG (("The Input Change in HDMI In requires a firmware download"));														
						}
						else if (BRAP_P_SPDIFRX_InputState_eStreamGoodCompress == psOutputParams->eNewState)
						{
			                /*BRAP_SPDIFRX_P_WaitForStreamStability (hRap,&(psOutputParams->sInputFormatInfo),100,50, BRAP_SpdifRx_StreamType_eCompressed);*/
							/* Get the latest Register value */
							ui32SpdifRxCtrlStatus = BRAP_Read32 (hRap->hRegister, BCHP_SPDIF_RCVR_CTRL_STATUS);
							
							psOutputParams->ui32NewCompState = (BCHP_GET_FIELD_DATA(ui32SpdifRxCtrlStatus, SPDIF_RCVR_CTRL_STATUS, BURST_PREAM_C));	
							BDBG_MSG (("This is the new Comp State : %x", psOutputParams->ui32NewCompState));
							if (BRAP_SpdifRx_StreamType_eCompressed == psOutputParams->sInputFormatInfo.eStreamType)
							{
								if (psInputParams->ui32CurrentCompState != psOutputParams->ui32NewCompState)
								{
									bChangeNeedsFirmwareDownload = true;						
									BDBG_MSG (("The Input Change in HDMI In requires a firmware download"));							
								}
							}						
						}
						break;

					case BRAP_P_SPDIFRX_InputState_eStreamPendingCompress:
	                case BRAP_P_SPDIFRX_InputState_eInvalid:						
	                case BRAP_P_SPDIFRX_InputState_eMax:
	                    BDBG_MSG (("Not supported. skipping"));
	                    break;
	                    
	                default:
	                    BDBG_ERR (("Not supported. Should not have come here"));						
				}
				
			}
            break;

        case BRAP_CapInputPort_eSpdif:          
			if (psInputParams->ui32CurrentEsrStatus & INTERRUPTS_OF_INTEREST_FOR_SPDIF_INPUT)
			{
				psOutputParams->eNewState = InferStreamType(
												hRap, 
												psInputParams->eCapInputPort,									 
												ui32SpdifRxCtrlStatus, 
												psInputParams->ui32CurrentEsrStatus
												);	
				
	            switch (psInputParams->eCurrentState)
	            {
	                case BRAP_P_SPDIFRX_InputState_eStreamNone:
	            		if (BRAP_P_SPDIFRX_InputState_eStreamPCM == psOutputParams->eNewState)
	            		{
	            			BRAP_SPDIFRX_P_SwitchToPCM(hRap);
	            			BDBG_MSG (("(1) Switched to PCM"));
	                    }
	            		else if (BRAP_P_SPDIFRX_InputState_eStreamPendingCompress == psOutputParams->eNewState)
	                    {
	            			BDBG_MSG (("(2) Pending switch to Compressed"));
	            			psOutputParams->eNewState = BRAP_SPDIFRX_P_SwitchToPES(hRap, ui32SpdifRxCtrlStatus);
	                    }                    
	                    break;

	                case BRAP_P_SPDIFRX_InputState_eStreamPCM:
	            		if (BRAP_P_SPDIFRX_InputState_eStreamNone == psOutputParams->eNewState)
	                    {
	            			BRAP_SPDIFRX_P_SwitchToNone(hRap);
	            			BDBG_MSG(( "(3) Switched to NoInput"));
	            		}
	            		else if (BRAP_P_SPDIFRX_InputState_eStreamPendingCompress == psOutputParams->eNewState)
	                    {
	            			BDBG_MSG (("(4) Pending switch to Compressed"));
	            			psOutputParams->eNewState = BRAP_SPDIFRX_P_SwitchToPES(hRap, ui32SpdifRxCtrlStatus);
	            		}                    
	                    break;

	                case BRAP_P_SPDIFRX_InputState_eStreamPendingCompress:
	            		if (BRAP_P_SPDIFRX_InputState_eStreamNone == psOutputParams->eNewState)
	                    {
	            			BRAP_SPDIFRX_P_SwitchToNone(hRap);
	            			BDBG_MSG(( "(5) Switched to NoInput"));
	                    }
	            		else if (BRAP_P_SPDIFRX_InputState_eStreamPCM == psOutputParams->eNewState)
	                    {
	            			BRAP_SPDIFRX_P_SwitchToPCM(hRap);
	            			BDBG_MSG(( "(6) Switched to PCM"));
	                    }
	            		else
	                    {
	                        psOutputParams->eNewState = psInputParams->eCurrentState;
	            			/* Recent switch to compressed. Check to see if we know type yet */
	            			if (CTRL_STATUS_BPC_VALID == (ui32SpdifRxCtrlStatus & CTRL_STATUS_BPC_VALID))
	                        {
	            				psOutputParams->eNewState = BRAP_P_SPDIFRX_InputState_eStreamGoodCompress;
	                            psOutputParams->ui32NewCompState = (BCHP_GET_FIELD_DATA(ui32SpdifRxCtrlStatus, SPDIF_RCVR_CTRL_STATUS, BURST_PREAM_C));
	            				BDBG_MSG(( "(7) Switched to compressed type %x", psOutputParams->ui32NewCompState));
	                        }
	                    }                    
	                    break;

	                case BRAP_P_SPDIFRX_InputState_eStreamGoodCompress:
	                    if (BRAP_P_SPDIFRX_InputState_eStreamNone == psOutputParams->eNewState)
	                    {
	            			BRAP_SPDIFRX_P_SwitchToNone (hRap);
	            			BDBG_MSG(("(5) Switched to NoInput"));
	                    }
	            		else if (BRAP_P_SPDIFRX_InputState_eStreamPCM == psOutputParams->eNewState)
	                    {
	            			BRAP_SPDIFRX_P_SwitchToPCM (hRap);
	            			BDBG_MSG(("(6) Switched to PCM"));
	                    }
	            		else
	           		    {
	           		        psOutputParams->eNewState = psInputParams->eCurrentState;
	            			if (CTRL_STATUS_BPC_VALID == (ui32SpdifRxCtrlStatus & CTRL_STATUS_BPC_VALID))
	           			    {
	            				psOutputParams->ui32NewCompState = (BCHP_GET_FIELD_DATA(ui32SpdifRxCtrlStatus, SPDIF_RCVR_CTRL_STATUS, BURST_PREAM_C));
	            				if (psOutputParams->ui32NewCompState != psInputParams->ui32CurrentCompState)
	                		    {
	            					BDBG_MSG (("(8) Switched to compressed type %x", psOutputParams->ui32NewCompState));
	                            }
	                        }            
	                    }                    
	                    break;


	                case BRAP_P_SPDIFRX_InputState_eInvalid:
	                case BRAP_P_SPDIFRX_InputState_eMax:
	                    BDBG_MSG (("Not supported. skipping"));
	                    break;
	                    
	                default:
	                    BDBG_ERR (("Not supported. Should not have come here"));
	            }

	            if ( BRAP_P_SPDIFRX_InputState_eStreamPCM == psOutputParams->eNewState || 
	                 BRAP_P_SPDIFRX_InputState_eStreamGoodCompress == psOutputParams->eNewState
	               )
	            {
	                BRAP_SPDIFRX_P_GetInputFormatInfo (hRap, &(psOutputParams->sInputFormatInfo));
	            }  

				if ( (psInputParams->eCurrentState != psOutputParams->eNewState)  ||
					 ( (BRAP_P_SPDIFRX_InputState_eStreamGoodCompress == psOutputParams->eNewState) &&
					   (psInputParams->ui32CurrentCompState != psOutputParams->ui32NewCompState)
					 )
				   )
					 
				{	
					if	( BRAP_P_SPDIFRX_InputState_eStreamPCM == psOutputParams->eNewState ||
						  BRAP_P_SPDIFRX_InputState_eStreamGoodCompress == psOutputParams->eNewState					)  
					{	
						BDBG_MSG (("The Input Change in SPDIF In requires a firmware download"));
						bChangeNeedsFirmwareDownload = true;
					}
				}	 
			}
            break;

        default:
            BDBG_ERR (("Input Mode Not Supported. Should not come here"));
    }


    BDBG_MSG (("Detect Input Change Logic ---- Output Params begin ----"));
    BDBG_MSG (("eNewState : %x", psOutputParams->eNewState));
    BDBG_MSG (("ui32NewCompState : %x", psOutputParams->ui32NewCompState));
    BDBG_MSG (("eStreamType : %x", psOutputParams->sInputFormatInfo.eStreamType));
    BDBG_MSG (("eType : %x", psOutputParams->sInputFormatInfo.eType));    
    BDBG_MSG (("eSampleRate : %x", psOutputParams->sInputFormatInfo.eSampleRate));    
    BDBG_MSG (("Detect Input Change Logic ---- Output Params end ----"));  
    
	BDBG_LEAVE (BRAP_SPDIFRX_P_DetectInputChange);   
    return bChangeNeedsFirmwareDownload;
}


/* Internal Helper Functions for Spdif Rx */

static BRAP_P_SPDIFRX_InputState InferStreamType (
    BRAP_Handle hRap, 
    BRAP_CapInputPort eCapInputPort,                                    
    uint32_t ui32SpdifRxCtrlStatus, 
    uint32_t ui32SpdifRxCurrentEsrStatus
    )
{
    uint32_t ui32ChanStatus = 0;
#if !((BRAP_3548_FAMILY == 1 && BCHP_VER==BCHP_VER_A0))  
#if (HDMI_MAI_FORMAT_INCONSISTENCY_WORKAROUND != 1)
	uint32_t ui32MAIAudioFormat = 0, ui32MAIFormat = 0;
#endif
#endif
    ui32ChanStatus = BRAP_Read32 (hRap->hRegister, BCHP_SPDIF_RCVR_CTRL_CHAN_STAT_L0);

    if (BRAP_CapInputPort_eHdmi == eCapInputPort)
    {
#if (BRAP_3548_FAMILY == 1 && BCHP_VER==BCHP_VER_A0)    
        return BRAP_P_SPDIFRX_InputState_eStreamHdmiPCM;    
#elif (1 == HDMI_MAI_FORMAT_INCONSISTENCY_WORKAROUND)
    	if ((ui32ChanStatus & 2) == 2)
    		return BRAP_P_SPDIFRX_InputState_eStreamGoodCompress;
        else
            return BRAP_P_SPDIFRX_InputState_eStreamHdmiPCM;
    
#else
	    ui32MAIFormat = BRAP_Read32 (hRap->hRegister, BCHP_SPDIF_RCVR_CTRL_MAI_FORMAT);
	    ui32MAIAudioFormat = BCHP_GET_FIELD_DATA(ui32MAIFormat, SPDIF_RCVR_CTRL_MAI_FORMAT, AUDIO_FORMAT);

		if ( 254 == ui32MAIAudioFormat )
		{
			BDBG_MSG (("LINEAR-PCM---MAI FORMAT"));
			return BRAP_P_SPDIFRX_InputState_eStreamHdmiPCM;
		}
		else if ( 255 == ui32MAIAudioFormat )
		{
			BDBG_MSG (("COMPRESSED---MAI FORMAT"));
			return BRAP_P_SPDIFRX_InputState_eStreamGoodCompress;
		}
		else 
		{
			BDBG_MSG (("NONE---MAI FORMAT"));
			return BRAP_P_SPDIFRX_InputState_eStreamNone;       
		}
#endif        
    }
    else if (BRAP_CapInputPort_eSpdif == eCapInputPort)
    {
    	if ((ui32SpdifRxCurrentEsrStatus & 4) == 4)
    	{
    		BDBG_MSG (("1(a) Input is Bad"));
    	    /*saw FMT_ERR_INTR*/

    		if ((ui32SpdifRxCtrlStatus & CTRL_STATUS_GOOD_BIPHASE) == CTRL_STATUS_GOOD_BIPHASE)
			{
				BDBG_MSG (("1(b) Input is Good"));
			}
    	}    
    	if ((ui32SpdifRxCtrlStatus & CTRL_STATUS_GOOD_BIPHASE) != CTRL_STATUS_GOOD_BIPHASE)
    		/*currently has no good input*/
    		return BRAP_P_SPDIFRX_InputState_eStreamNone;
    	else if ((ui32SpdifRxCtrlStatus & CTRL_STATUS_SAMPLE_RATE) > 11)
    		/*rate is not indicated (yet)*/
    		return BRAP_P_SPDIFRX_InputState_eStreamNone;
    	else if ((ui32SpdifRxCtrlStatus & CTRL_STATUS_VALIDITY) != 0)
    		/*at least one validity bit is 1 so treat as non-PCM*/
    		return BRAP_P_SPDIFRX_InputState_eStreamPendingCompress;
    	else if ((ui32ChanStatus & 2) == 2)
    		/*cstat bit 1 indicates compressed*/
    		return BRAP_P_SPDIFRX_InputState_eStreamPendingCompress;
    	else
    		return BRAP_P_SPDIFRX_InputState_eStreamPCM;    
    }
	else
	{
    	BDBG_ERR (("Input Mode Not Supported. Should not come here"));	
		return BRAP_P_SPDIFRX_InputState_eStreamNone;
	}
	
}

BRAP_P_SPDIFRX_InputState 
BRAP_SPDIFRX_P_SwitchToPES (
    BRAP_Handle hRap, 
    uint32_t ui32SpdifRxCtrlStatus 
    )
{
    uint32_t ui32SpdifRxCtrlConfig = 0;
    /* Switch to PES */
    ui32SpdifRxCtrlConfig = BRAP_Read32 (hRap->hRegister, BCHP_SPDIF_RCVR_CTRL_CONFIG);
    ui32SpdifRxCtrlConfig &= ~ (BCHP_MASK(SPDIF_RCVR_CTRL_CONFIG, OUT_FORMAT_ENA));  
    ui32SpdifRxCtrlConfig |= BCHP_FIELD_ENUM (SPDIF_RCVR_CTRL_CONFIG, OUT_FORMAT_ENA, PES);
    BRAP_Write32 (hRap->hRegister, BCHP_SPDIF_RCVR_CTRL_CONFIG, ui32SpdifRxCtrlConfig);
    
	if ( (ui32SpdifRxCtrlStatus & CTRL_STATUS_BPC_VALID) == CTRL_STATUS_BPC_VALID)
    {
		BDBG_MSG (("Immediate switch to known compressed type"));
		BDBG_MSG (("(9) Switched to compressed type %x", (BCHP_GET_FIELD_DATA(ui32SpdifRxCtrlStatus, SPDIF_RCVR_CTRL_STATUS, BURST_PREAM_C))));
		return BRAP_P_SPDIFRX_InputState_eStreamGoodCompress;
    }
	else
    {
		BDBG_MSG (("Delayed switch to known compressed type"));
		return BRAP_P_SPDIFRX_InputState_eStreamPendingCompress;
    }
}

BERR_Code
BRAP_SPDIFRX_P_SwitchToPCM (
    BRAP_Handle hRap
    )
{
    uint32_t ui32SpdifRxCtrlConfig = 0;
    
    /* Switch to PCM */
    ui32SpdifRxCtrlConfig = BRAP_Read32 (hRap->hRegister, BCHP_SPDIF_RCVR_CTRL_CONFIG);
    ui32SpdifRxCtrlConfig &= ~ (BCHP_MASK(SPDIF_RCVR_CTRL_CONFIG, OUT_FORMAT_ENA));  
    ui32SpdifRxCtrlConfig |= BCHP_FIELD_ENUM (SPDIF_RCVR_CTRL_CONFIG, OUT_FORMAT_ENA, PCM);
    BRAP_Write32 (hRap->hRegister, BCHP_SPDIF_RCVR_CTRL_CONFIG, ui32SpdifRxCtrlConfig);  
	BDBG_MSG (("Came to SWITCH TO PCM ....."));

    return BERR_SUCCESS;
}

BERR_Code
BRAP_SPDIFRX_P_SwitchToNone (
    BRAP_Handle hRap
    )
{
    uint32_t ui32SpdifRxCtrlConfig = 0;
    
	/* here is where to mute output */ /* LOOKS LIKE ERROR BELOW. WE NEED TO MUTE OUTPUT */
    /* Switch to PCM */
    ui32SpdifRxCtrlConfig = BRAP_Read32 (hRap->hRegister, BCHP_SPDIF_RCVR_CTRL_CONFIG);
    ui32SpdifRxCtrlConfig &= ~ (BCHP_MASK(SPDIF_RCVR_CTRL_CONFIG, OUT_FORMAT_ENA));  
    ui32SpdifRxCtrlConfig |= BCHP_FIELD_ENUM (SPDIF_RCVR_CTRL_CONFIG, OUT_FORMAT_ENA, PCM);
    BRAP_Write32 (hRap->hRegister, BCHP_SPDIF_RCVR_CTRL_CONFIG, ui32SpdifRxCtrlConfig);    
	BDBG_MSG (("Came to SWITCH TO PCM ....."));	

    return BERR_SUCCESS;
}

BERR_Code
BRAP_SPDIFRX_P_SwitchToCompressed (
    BRAP_Handle hRap
    )
{
    uint32_t ui32SpdifRxCtrlConfig = 0;
    
    /* Switch to Compressed */
    ui32SpdifRxCtrlConfig = BRAP_Read32 (hRap->hRegister, BCHP_SPDIF_RCVR_CTRL_CONFIG);
    ui32SpdifRxCtrlConfig &= ~ (BCHP_MASK(SPDIF_RCVR_CTRL_CONFIG, OUT_FORMAT_ENA));  
    ui32SpdifRxCtrlConfig |= BCHP_FIELD_ENUM (SPDIF_RCVR_CTRL_CONFIG, OUT_FORMAT_ENA, COMP);
    BRAP_Write32 (hRap->hRegister, BCHP_SPDIF_RCVR_CTRL_CONFIG, ui32SpdifRxCtrlConfig);  
	BDBG_MSG (("Came to SWITCH TO COMP ....."));

    return BERR_SUCCESS;
}


	
/* End of File */
	
