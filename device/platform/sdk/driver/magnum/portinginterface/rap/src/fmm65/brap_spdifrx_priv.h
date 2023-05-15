/***************************************************************************
*     Copyright (c) 2004-2009, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: brap_spdifrx_priv.h $
* $brcm_Revision: Hydra_Software_Devel/6 $
* $brcm_Date: 11/24/09 2:32p $
*
* Module Description:
*   Module name: SPDIF/HDMI In (Receiver)
*   This file contains the definitions and prototypes for the hardware SPDIF/HDMI
*   receiver abstraction.
*   
* Revision History:
*
* $brcm_Log: /magnum/portinginterface/rap/base3/brap_spdifrx_priv.h $
* 
* Hydra_Software_Devel/6   11/24/09 2:32p speter
* SW3548-2629: [3548] Adding SPDIF Rx Passthru without DSP
* 
* Hydra_Software_Devel/5   11/24/09 11:58a speter
* SW3548-2629: [3548] Adding SPDIF Rx Passthru without DSP
* 
* Hydra_Software_Devel/4   11/6/08 4:21p sgadara
* PR 48564: [3548,3556] Switching to PCM or PES during channel start for
* HDMI/SPDIF.
* 
* Hydra_Software_Devel/3   10/18/08 3:30a speter
* PR 47360: [3548,3556]: Changing the design of Spdif Rx. SPDIF Rx is now
* a device level interrupt and handling of digital input format change
* is moved from RAP PI to higher layer.
* 
* Hydra_Software_Devel/2   10/17/08 2:33a speter
* PR 47360: [3548,3556]: Changing the design of Spdif Rx. SPDIF Rx is now
* a device level interrupt and handling of digital input format change
* is moved from RAP PI to higher layer.
* 
* Hydra_Software_Devel/1   10/10/08 11:16a speter
* PR 38950: [3548,3556,7405,7325,7335] Moving references from base2 to
* base3
* 
* Hydra_Software_Devel/1   5/3/07 4:27p speter
* PR 28716: [3563] SpdifRx Comp1->Comp2 Changes. Major code revamp in
* Spdif Rx Handling
* 
***************************************************************************/

#ifndef _BRAP_SPDIFRX_PRIV_H__ /*{{{*/
#define _BRAP_SPDIFRX_PRIV_H__

/*{{{ Defines */

#define BRAP_P_SPDIFRX_INVALID_PREAMBLE_C      0x12340000	/* Maximum bits per sample */

/**********************************************************************
Summary:
    SPDIF Receiver Digital Input Format
Description:
   This structure returns information of Digital Input format whenever there
   is a change in the format. It is used for the notification to the application
   about the change in the format.
Notes: 
    This structure will be passed on to application callback function
    on Digital input format change
**********************************************************************/
typedef struct BRAP_P_SPDIFRX_InputFormat
{
    BRAP_SpdifRx_StreamType eStreamType;   /* Input Format. May be linear or compressed  */
    BRAP_DSPCHN_AudioType   eType;         /* Audio Type. This fiels is valid only 
                                                when the stream type is compressed */
    BAVC_AudioSamplingRate  eSampleRate;   /* Input Sampling Rate. This field in only 
                                                valid when the stream is PCM (linear) */
}BRAP_P_SPDIFRX_InputFormat;

/***************************************************************************
Summary:
    This enumeration is used by the SPDIF Rx for the State Machine Implementation
    in detecting the input change.
***************************************************************************/ 
typedef enum BRAP_P_SPDIFRX_InputState
{   
    BRAP_P_SPDIFRX_InputState_eStreamNone,
    BRAP_P_SPDIFRX_InputState_eStreamPCM,
    BRAP_P_SPDIFRX_InputState_eStreamHdmiPCM,    
    BRAP_P_SPDIFRX_InputState_eStreamPendingCompress,
    BRAP_P_SPDIFRX_InputState_eStreamGoodCompress,
    BRAP_P_SPDIFRX_InputState_eInvalid,
    BRAP_P_SPDIFRX_InputState_eMax
}BRAP_P_SPDIFRX_InputState;

/***************************************************************************
Summary:
    This structure is used as input by the SPDIF Rx State Machine Implementation
    in detecting the input change.
***************************************************************************/ 
typedef struct BRAP_P_SPDIFRX_DetectChange_InputParams
{
    /* The capture input port for which the input change is being monitored */
    unsigned int                eCapInputPort;     

    /* The Current Input State according to the detection logic */ 
    BRAP_P_SPDIFRX_InputState   eCurrentState;        

    /* The Current Compressed state. Valid only if the eCurrentState is GoodCompress */
    uint32_t                    ui32CurrentCompState; 
    
    /* The Current Esr Status Value stored in hRapch. To be used in detection logic */
    uint32_t                    ui32CurrentEsrStatus; 

}BRAP_P_SPDIFRX_DetectChange_InputParams;

typedef struct BRAP_P_SPDIFRX_DetectChange_OutputParams
{
    /* The Changed/New Input State according to the detection logic */ 
    BRAP_P_SPDIFRX_InputState       eNewState;

    /* The Changed/New Compressed state. Valid only if the eNewState is GoodCompress */
    uint32_t                        ui32NewCompState;    
    
    /* The information on the new changed input */
    BRAP_P_SPDIFRX_InputFormat      sInputFormatInfo;
}BRAP_P_SPDIFRX_DetectChange_OutputParams;


/* SPDIF Rx Private Functions */
BERR_Code 
BRAP_SPDIFRX_P_Open(
    BRAP_Handle         hRap,
    BRAP_CapInputPort	eSpdifRxInputPort
    );

BERR_Code 
BRAP_SPDIFRX_P_Close(
    BRAP_Handle         hRap
    );

BERR_Code 
BRAP_SPDIFRX_P_Start(
    BRAP_Handle          hRap
    );

BERR_Code 
BRAP_SPDIFRX_P_Stop(
    BRAP_Handle          hRap
    );

BERR_Code 
BRAP_SPDIFRX_P_GetRxStatus(
    BRAP_Handle          hRap,
    BRAP_SpdifRx_Status  *pStatus
    );

BERR_Code 
BRAP_SPDIFRX_P_GetChannelStatus (
    BRAP_Handle          hRap,
    BRAP_SpdifRx_ChannelStatus  *pChannelStatus
    );

BERR_Code 
BRAP_SPDIFRX_P_GetInputFormatInfo(
    BRAP_Handle          hRap,                 
    BRAP_P_SPDIFRX_InputFormat *psInputFormatInfo   
    );

bool 
BRAP_SPDIFRX_P_DetectInputChange (
    BRAP_Handle hRap, 
    BRAP_P_SPDIFRX_DetectChange_InputParams     *psInputParams,
    BRAP_P_SPDIFRX_DetectChange_OutputParams    *psOutputParams
    );


BRAP_P_SPDIFRX_InputState 
BRAP_SPDIFRX_P_SwitchToPES (
    BRAP_Handle hRap, 
    uint32_t ui32SpdifRxCtrlStatus 
    );

BERR_Code
BRAP_SPDIFRX_P_SwitchToPCM (
    BRAP_Handle hRap
    );

BERR_Code
BRAP_SPDIFRX_P_SwitchToNone (
    BRAP_Handle hRap
    );

BERR_Code
BRAP_SPDIFRX_P_SwitchToCompressed (
    BRAP_Handle hRap
    );


#endif /*}}} #ifndef _BRAP_SPDIFRX_PRIV_H__ */
	
/* End of File */
	

