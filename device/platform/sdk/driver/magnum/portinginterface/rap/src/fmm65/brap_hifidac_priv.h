/***************************************************************************
*     Copyright (c) 2004-2009, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: brap_hifidac_priv.h $
* $brcm_Revision: Hydra_Software_Devel/3 $
* $brcm_Date: 6/17/09 4:49p $
*
* Module Description:
*		This file contains all private macros, enums, structures and 
*		functions privately used inside the hifidac module of the 
*		Raptor PI.
*
* Revision History:
*
* $brcm_Log: /magnum/portinginterface/rap/base3/brap_hifidac_priv.h $
* 
* Hydra_Software_Devel/3   6/17/09 4:49p gautamk
* PR56010: [7405] Implmenting Code review comment
* 
* Hydra_Software_Devel/2   6/12/09 3:40p gautamk
* PR53302: [7405] Merging cleanup code to mainline
* 
* Hydra_Software_Devel/1   7/21/08 2:25p speter
* PR 44984: [3548,3556,7405] Adding files from base which have new
* Architecture specific changes and cannot be merged back in base
* 
* Hydra_Software_Devel/10   2/2/07 3:48p rjain
* PR 27477: Make BRAP_OP_P_DacSetVolume() and BRAP_OP_P_DacGetVolume()
* public
* 
* Hydra_Software_Devel/9   12/7/06 1:27p sushmit
* PR 25346: Merging 3563 to Hydra
* 
* Hydra_Software_Devel/RAP_3563_Merge/2   12/4/06 12:18p sushmit
* PR 24037: Adding 3563 specific preprocessor directive.
* 
* Hydra_Software_Devel/RAP_3563_Merge/1   12/4/06 10:45a sushmit
* PR 24037: Merging 7440 to 3563
* 
* Hydra_Software_Devel/8   6/19/06 2:23p rjain
* PR 22179: adding description for start-time / on-the-fly changeable
* paramters
* 
* Hydra_Software_Devel/7   6/19/06 12:16p rjain
* PR 22179: modifying  code to make certain Output port settings
* changeable at start-time and on the fly.
* 
* Hydra_Software_Devel/5   12/7/05 12:23p kagrawal
* PR 18312: Added declaration for BRAP_OP_P_DacSetTimebase_isr
* 
* Hydra_Software_Devel/4   12/6/05 6:17p kagrawal
* PR 18312: Added following ISR version routine:
* BRAP_OP_P_DacSetSampleRate_isr
* 
* Hydra_Software_Devel/3   11/10/05 11:52a kagrawal
* PR 17590: Merging PCM Playback and Mixing changes to
* Hydra_Software_Devel
* 
* Hydra_Software_Devel/Kapil_PCM_Playback/1   10/24/05 4:48p kagrawal
* PR 17590: Initial check in for PCM Playback and Mixing
* 
* Hydra_Software_Devel/2   10/28/05 2:32p bselva
* PR 16888: Added ISr safe mute function for dac
* 
* Hydra_Software_Devel/1   9/13/05 7:43p bmishra
* PR 16148: Merged 7401 related changes
* 
* Hydra_Software_Devel/BLR_RAP_7401_Bringup/2   8/31/05 6:40p bselva
* PR 16148: Adding DAC support
* 
* Hydra_Software_Devel/BLR_RAP_7401_Bringup/1   8/1/05 4:49p bmishra
* PR 16148: Initial version of the file
* 
* 
***************************************************************************/

#ifndef _BRAP_HIFIDAC_PRIV_H__ /*{{{*/
#define _BRAP_HIFIDAC_PRIV_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
	BAVC_AudioSamplingRate   eSamplingRate;
	uint32_t			uiNumerator;
	uint32_t			uiDenominator;
	uint32_t			uiSampleInc;
	uint32_t			uiPhaseInc;
}BRAP_OP_P_DacRateMangerSR;


BERR_Code
BRAP_OP_P_DacOpen (
    BRAP_OP_P_Handle      hOp,               /*[in] Output port handle */
    const BRAP_OP_P_DacSettings * pSettings  /*[in] Open time parameters */
);

/***************************************************************************
Summary:
    Programs the on-the-fly changeable parameters to the HW registers 
    for the Dac ie BCHP_HIFIDAC_CTRL0_CONFIG

Description:
    This PI takes a BRAP_OP_P_DacSettings structure.
    Based on this it programs the register BCHP_HIFIDAC_CTRL0_CONFIG
    It then saves the BRAP_OP_P_DacSettings structure in hOp.
    At present, the  on-the-fly changeable parameters for Dac are 
    eChannelOrder and eMuteType
    
Returns:
    BERR_SUCCESS on success
    
See Also:
    BRAP_OP_P_DacStart(), BRAP_OP_P_DacOpen(), BRAP_SetOutputConfig()
**************************************************************************/
BERR_Code BRAP_OP_P_DacHWConfig (
    BRAP_OP_P_Handle        hOp,              /*[in] Output port handle */
    const BRAP_OP_P_DacSettings * pSettings /*[in] Parameters */
);

BERR_Code
BRAP_OP_P_DacStart (
    BRAP_OP_P_Handle        hOp,             /*[in] Output port handle */
    const BRAP_OP_P_DacParams * pParams    /*[in] Start time parameters */
);

BERR_Code
BRAP_OP_P_DacStop (
    BRAP_OP_P_Handle        hOp              /*[in] Output port handle */
);


/***************************************************************************
Summary:
	Mutes/Unmutes the DAC output

Description:
	This API is required to mute/unmute the output of the specified DAC.
	PI maintains this state upon channel change.

Returns:
	BERR_SUCCESS 

See Also:
	BRAP_OP_P_DacGetMute
	
****************************************************************************/
BERR_Code
BRAP_OP_P_DacSetMute (
    BRAP_Handle     hRap,			/* [in] Audio Device Handle */
    BRAP_OutputPort     eOpType,	/* [in] Output Type */
    bool bMute						/* [in] True = Mute; false = unmute*/
);

BERR_Code
BRAP_OP_P_DacSetMute_isr (
    BRAP_Handle     hRap,			/* [in] Audio Device Handle */
    BRAP_OutputPort     eOpType,	/* [in] Output Type */
    bool bMute						/* [in] True = Mute; false = unmute*/
);

/***************************************************************************
Summary:
	Retrieves the mute status

Description:
	This API is required to get mute/unmute status of the
	output of the specified DAC.

Returns:
	BERR_SUCCESS 

See Also:
	BRAP_OP_P_DacSetMute
	
****************************************************************************/
BERR_Code
BRAP_OP_P_DacGetMute (
    BRAP_Handle     hRap,			/* [in] Audio Device Handle */
    BRAP_OutputPort     eOpType,	/* [in] Output Type */
    bool *pbMute					/* [out]True = Mute; false = unmute*/
);

/***************************************************************************
Summary:
	Sets the sample rate for the given DAC output

Description:
	This API is required to set the sampling rate of DAC.
	This is mainly used for playback of PCM buffers from
	memory. In decode mode the F/W sets it according to
	the frequency.

Returns:
	BERR_SUCCESS 

See Also:
	
****************************************************************************/
BERR_Code
BRAP_OP_P_DacSetSampleRate(
    BRAP_Handle     hRap,					/* [in] Audio Device Handle */
    BRAP_OutputPort     eOpType,			/* [in] Output Type */
    BAVC_AudioSamplingRate eSamplingRate	/* [in]The sampling rate to be programmed */
);

/***************************************************************************
Summary:
	Sets the sample rate for the given DAC output

Description:
    ISR version of BRAP_OP_P_DacSetSampleRate
    
Returns:
	BERR_SUCCESS 

See Also:
		
****************************************************************************/
BERR_Code
BRAP_OP_P_DacSetSampleRate_isr(
    BRAP_Handle     hRap,					/* [in] Audio Device Handle */
    BRAP_OutputPort     eOpType,			/* [in] Output Type */
    BAVC_AudioSamplingRate eSamplingRate	/* [in]The sampling rate to be programmed */
);

/***************************************************************************
Summary:
	Sets the timebase for the given DAC output

Description:
	This API is required to set the timebase of DAC.

Returns:
	BERR_SUCCESS 

See Also:
	
****************************************************************************/
BERR_Code BRAP_OP_P_DacSetTimebase(
    BRAP_OP_P_Handle      hOp,               /*[in] Output port handle */
	BAVC_Timebase		  eTimebase			 /*[in] time base to program */
);

/***************************************************************************
Summary:
	Sets the timebase for the given DAC output

Description:
    ISR version of BRAP_OP_P_DacSetTimebase.

Returns:
	BERR_SUCCESS 

See Also:
	
****************************************************************************/
BERR_Code BRAP_OP_P_DacSetTimebase_isr(
    BRAP_OP_P_Handle      hOp,               /*[in] Output port handle */
	BAVC_Timebase		  eTimebase			 /*[in] time base to program */
);

 uint32_t BRAP_OP_P_GetDacOffset(BRAP_OutputPort eOpType, uint32_t *pOffset);
#ifdef __cplusplus
}
#endif

#endif /*}}} #ifndef _BRAP_HIFIDAC_PRIV_H__ */

