/***************************************************************************
*     Copyright (c) 2004-2008, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: brap_hifidac_priv.h $
* $brcm_Revision: Hydra_Software_Devel/12 $
* $brcm_Date: 10/1/08 11:42a $
*
* Module Description:
*		This file contains all private macros, enums, structures and 
*		functions privately used inside the hifidac module of the 
*		Raptor PI.
*
* Revision History:
*
* $brcm_Log: /magnum/portinginterface/rap/base/brap_hifidac_priv.h $
* 
* Hydra_Software_Devel/12   10/1/08 11:42a bhanus
* PR 47477 : [7440]Removing programming of MAPPER_SOFTMUTE from Open time
* fro 7440
* - Moving it after DAC Start
* 
* Hydra_Software_Devel/11   8/11/08 2:26p bhanus
* PR43971: [7601] Merging DVD family for 7601 and onward chips
* 
* Hydra_Software_Devel/RAP_DVD_Family/1   7/17/08 9:08a gdata
* PR43971: [7440] Adding changes related to RDB
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

#if(BCHP_CHIP != 7601)

#if (BCHP_CHIP == 7440)
BERR_Code
BRAP_OP_P_DacOpenUnMute (
    BRAP_OP_P_Handle      hOp               /*[in] Output port handle */
);
#endif

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
#endif
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
#if(BCHP_CHIP != 7601)
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
#endif
#if ( BCHP_CHIP == 3563 )
/***************************************************************************
Summary:
	Sets the 5 band coeff coeffs for the given DAC output
****************************************************************************/
void BRAP_P_PogramDacEqualizerCoeff (
	BRAP_Handle     				hRap,			/* [in] Audio Device Handle */
	BRAP_OutputPort				eOutputPort,		/* [in] Output port to be set, currently being DAC or I2S */
	BRAP_P_Equalizer_GainCoeff	*pEqualizerCoeff	/* [in] Equalizer coeff */
);

/***************************************************************************
Summary:
	Sets the Tone control coeffs for the given DAC output

****************************************************************************/
BERR_Code BRAP_OP_P_DacSetToneCtrlCoeff(
	BRAP_Handle     				hRap,			/* [in] Audio Device Handle */
	BRAP_OutputPort				eOutputPort,		/* [in] Output port to be set, currently being DAC or I2S */
	BRAP_Equalizer_ToneControl		*psToneControl	/* [in] structure with parameters for Tone Control */
);
#endif

#ifdef __cplusplus
}
#endif

#endif /*}}} #ifndef _BRAP_HIFIDAC_PRIV_H__ */

