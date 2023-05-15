/***************************************************************************
*     Copyright (c) 2004-2009, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: brap_capport_priv.h $
* $brcm_Revision: Hydra_Software_Devel/3 $
* $brcm_Date: 6/12/09 3:44p $
*
* Module Description:
*   Module name: CAPPORT
*   This file lists all data structures, macros, enumerations and function 
*   prototypes for the Capture Port abstraction, which are internal ie NOT 
*   exposed to the application developer. These can be used only by the Audio
*   Manager and other FMM submodules. It covers the functionality of the 
*   Capture Hardware Block . This object is only used by a PCM Capture Channel
*   
* Revision History:
*
* $brcm_Log: /magnum/portinginterface/rap/base3/brap_capport_priv.h $
* 
* Hydra_Software_Devel/3   6/12/09 3:44p gautamk
* PR53302: [7405] Merging cleanup code to mainline
* 
* Hydra_Software_Devel/RAP_7405_CLEANUP/1   3/17/09 3:30p gautamk
* PR53302: [7405]Code cleanup
* 
* Hydra_Software_Devel/2   10/18/08 2:43a speter
* PR 47961: [3548,3556] Allowing Setinputconfig when the channel is
* running for RFAudio
* 
* Hydra_Software_Devel/1   7/21/08 2:16p speter
* PR 44984: [3548,3556,7405] Adding files from base2 which have new
* Architecture specific changes and cannot be merged back in base2
* 
* Hydra_Software_Devel/Rap_NewFwArch/5   6/11/08 7:32a speter
* PR38950: [3548,3556] Changing generic use of BTSC as RF Audio
* 
* Hydra_Software_Devel/Rap_NewFwArch/4   5/28/08 2:46a speter
* PR 39442: [3548,3556] Adding the Nicam and Pal A2 support
* 
* Hydra_Software_Devel/Rap_NewFwArch/3   5/5/08 11:26a speter
* PR 38950: [3548] Making 3548_FAMILY macro and using it for both 3548
* and 3556 cases.
* 
* Hydra_Software_Devel/Rap_NewFwArch/2   4/28/08 4:38p speter
* PR 38950: [3548] Fixing compilation for 3548/3556.
* 
* Hydra_Software_Devel/Rap_NewFwArch/1   4/22/08 2:17p gautamk
* PR41994: [7405] Using BCHP_VER_ Correctly...
* 
* Hydra_Software_Devel/12   9/30/07 1:54p sushmit
* PR35140: [7405] Develop 7325 Raptor PI for Bringup
* 
* Hydra_Software_Devel/11   6/13/07 7:27p nitinb
* PR 32116: Provide an API to scale the input signal to BTSC decoder
* 
* Hydra_Software_Devel/10   5/21/07 3:26p sushmit
* PR 27646: 7405 Compilation
* 
* Hydra_Software_Devel/9   5/14/07 5:25p kagrawal
* PR 30069: [7440] Added following support:
* - BRAP_P_GetBaseSR(), BRAP_P_GetPLLForSR(), BRAP_P_IsInternalCapPort()
* - Added AM changes for allocating Fs Timing Source
* - Allocating internal capPort in OpenCapPath()
* - Removed internal capPort copy and remove hack from
* BRAP_P_StartFmmModuleHandles
* - Populating sInPathProp->uiPathIdx for SM and XCD
* 
* Hydra_Software_Devel/8   5/11/07 8:58p sushmit
* PR 27646: Getting 7405 PI compiled
* 
* Hydra_Software_Devel/7   4/13/07 6:33p nitinb
* PR 25346: Added more BTSC code
* 
* Hydra_Software_Devel/6   4/4/07 5:02p kagrawal
* PR 28561: [7440, 3563] Modified capport module to support multiple
* instances of open and start by using open/start counts. Removed
* eState.
* 
* Hydra_Software_Devel/5   4/1/07 6:51a bhanus
* PR 28934 :[7440 B0] Added code for CAP Port Grouping.
* 
* Hydra_Software_Devel/4   3/30/07 1:46p nitinb
* PR 28191: Get BTSC A2 Status, need get BTSC_CTRL_EF_AMPL for noise
* detection
* 
* Hydra_Software_Devel/3   12/29/06 1:08a nitinb
* PR 26766: (3563-specific) Added BTSC private function prototypes
* 
* Hydra_Software_Devel/2   12/7/06 1:27p sushmit
* PR 25346: Merging 3563 to Hydra
* 
* Hydra_Software_Devel/RAP_3563_Merge/2   12/4/06 12:18p sushmit
* PR 24037: Adding 3563 specific preprocessor directive.
* 
* Hydra_Software_Devel/RAP_3563_Merge/1   12/4/06 11:11a sushmit
* PR 24037: Merging BTSC changes
* 
* Hydra_Software_Devel/RAP_3563_API_STUBS/1   12/3/06 10:58p nitinb
* PR 25346: Added BTSC code
* 
* Hydra_Software_Devel/1   8/28/06 4:34p speter
* PR 20654: Adding capture related files
***************************************************************************/
#ifndef _BRAP_CAPPORT_PRIV_H_
#define _BRAP_CAPPORT_PRIV_H_

#include "brap.h"
#include "brap_priv.h"

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************
Summary:
    Parameters to be passed by Audio Manager on Opening a Capture Port.
    Note that this includes both public and private settings.

Description:
    No settings at present.
See Also:
    BRAP_CAPPORT_P_Open
***************************************************************************/
typedef struct BRAP_CAPPORT_P_Settings
{
    int tbd;
}BRAP_CAPPORT_P_Settings;

/***************************************************************************
Summary:
    Parameters to be passed by Audio Manager on Starting a Capture Port.
    Note that this includes both public and private parameters.

See Also:
    BRAP_CAPPORT_P_Start
***************************************************************************/

typedef struct BRAP_CAPPORT_P_Params
{
    unsigned int            uiInputFciId;
    BRAP_InputBitsPerSample	eInputBitsPerSample;
    unsigned int            uiGrpId;
    unsigned int            uiFsTmgSrc;     /* FS Timing Source */
    BRAP_OP_Pll 			ePll;           /* PLL to be associated */
    BAVC_AudioSamplingRate  eSamplingRate;  /* Sampling rate of the data flowing 
                                               through internal CapPort */
}BRAP_CAPPORT_P_Params;

/***************************************************************************
Summary:
    Abstraction of a Capture Port 
    
Description:
    It contains all the information required to handle the Capture Port
    Particularly, it stores the type, handles for all required chip 
    related information, parent FMM handle, state etc

See Also:
    
***************************************************************************/
typedef struct BRAP_CAPPORT_P_Object
{
    BRAP_CAPPORT_P_Settings sSettings;  /* Capture Port settings provided 
                                           during Open() */
    BRAP_CAPPORT_P_Params   sParams;    /* Capture Port settings  
                                           provided during Start() */
    BCHP_Handle             hChip;      /* Handle to chip object */
    BREG_Handle             hRegister;  /* Handle to register object */
    BMEM_Handle             hHeap;      /* Handle to memory object */
    BINT_Handle             hInt;       /* Handle to interrupt object */
    BRAP_FMM_P_Handle       hFmm;       /* Parent FMM handle */
    BRAP_CapInputPort       eCapPort;   /* Capture Port Type */
    unsigned int            uiOpenCnt;  /* No. of open audio channels currently
                                           routed to this capture port */
    unsigned int            uiStartCnt; /* No. of active audio channels currently
                                           routed to this capture port */
}BRAP_CAPPORT_P_Object;

/***************************************************************************
Summary:
    Opens a Capture port
    
Description:
    Initializes the Capture port and returns a Capture Port handle.The handle 
    can then be used for all other function calls.

Returns:
    BERR_SUCCESS else error

See Also:
    BRAP_CAPPORT_P_Close
**************************************************************************/
BERR_Code BRAP_CAPPORT_P_Open (
    BRAP_FMM_P_Handle           hFmm,           /* [in] Parent FMM handle */
    BRAP_CAPPORT_P_Handle       *phCapPort,     /* [out] Pointer to Capture Port handle */
    BRAP_CapInputPort           eCapPort,       /* [in] Capture Port index */
    const BRAP_CAPPORT_P_Settings *pSettings      /* [in] Capture Port settings */
);

/***************************************************************************
Summary:
    Releases all the resources associated with this Capture port and frees 
    the handles.
    
Description:

Returns:
    BERR_SUCCESS else error

See Also:
BRAP_CAPPORT_P_Open
**************************************************************************/
BERR_Code BRAP_CAPPORT_P_Close ( 
    BRAP_CAPPORT_P_Handle   hCapPort      /* [in] Capture Port Handle */
);

/***************************************************************************
Summary:
    Enables a Capture Port.

Description:
    This function should be called for enabling the capture port to get the 
    date from its input.
    
Returns:
    BERR_SUCCESS on success
    Error code on failure

See Also:
    BRAP_CAPPORT_P_Stop, BRAP_CAPPORT_P_HWConfig
**************************************************************************/
BERR_Code BRAP_CAPPORT_P_Start ( 
	BRAP_CAPPORT_P_Handle       hCapPort,   /* [in] Capture Port Handle */
    const BRAP_CAPPORT_P_Params *pParams    /* [in] Pointer to start parameters 
                                               for this Capture Port */ 
);

/***************************************************************************
Summary:
    Disables the Capture port.

Description:
    This function disables the capture port.
Returns:
    BERR_Success else Error
See Also:
    BRAP_CAPPORT_P_Start
**************************************************************************/
BERR_Code BRAP_CAPPORT_P_Stop ( 
    BRAP_CAPPORT_P_Handle   hCapPort    /* [in] Capture Port Handle */
);

/***************************************************************************
Summary:
    Returns the Default Params for Capture Port.

Description:
    For parameters that the system cannot assign default values to, 
    an invalid value is returned. Note that the default parameters are common
    for all Capture Ports.
    
Returns:
    BERR_SUCCESS else error
    
See Also:

**************************************************************************/
BERR_Code 
BRAP_CAPPORT_P_GetDefaultParams ( 
    BRAP_CAPPORT_P_Params    *pDefParams    /* Pointer to memory where default
                                               settings should be written */    
);

#if ((BRAP_3548_FAMILY == 1) )

typedef struct BRAP_P_RfAudioRateMangerSR
{
	BAVC_AudioSamplingRate   eSamplingRate;
	uint32_t			ui32Numerator;
	uint32_t			ui32Denominator;
	uint32_t			ui32SampleInc;
	uint32_t			ui32PhaseInc;
}BRAP_P_RfAudioRateMangerSR;

void BRAP_RFAUDIO_P_SapXMute_isr (
    BRAP_CAPPORT_P_Handle       phCapPort     /* [out] Pointer to Capture Port handle */
);

BERR_Code BRAP_RFAUDIO_P_SetOutputSampleRate ( 
    BRAP_CAPPORT_P_Handle       phCapPort     /* [out] Pointer to Capture Port handle */
);

BERR_Code BRAP_RFAUDIO_P_InitDecoder (
	BRAP_Handle	hRap		/* [in] RAP Handle */
);

BERR_Code BRAP_RFAUDIO_P_SetSettings (
    BRAP_CAPPORT_P_Handle       phCapPort     /* [in] Pointer to Capture Port handle */
);

BERR_Code BRAP_RFAUDIO_P_Start (
    BRAP_CAPPORT_P_Handle       phCapPort     /* [out] Pointer to Capture Port handle */
);

BERR_Code BRAP_RFAUDIO_P_Stop (
    BRAP_CAPPORT_P_Handle       phCapPort     /* [out] Pointer to Capture Port handle */
);   

BERR_Code BRAP_RFAUDIO_P_GetStatus (
    BRAP_CAPPORT_P_Handle       phCapPort,     /* [in] Pointer to Capture Port handle */
	BRAP_RfAudioStatus  *sStatus			   /* [out] Status of the channel */
);

BERR_Code BRAP_RFAUDIO_P_GetEnvelopeFreqAmpl (
    BRAP_CAPPORT_P_Handle       phCapPort,     /* [in] Pointer to Capture Port handle */
	uint32_t  *ui32EnvelopeFreqAmpl			/* [out] Status of the channel */
);

BERR_Code BRAP_RFAUDIO_P_SetMute (
    BRAP_CAPPORT_P_Handle       phCapPort,     /* [in] Pointer to Capture Port handle */
    bool						bMute		/* [out ] Mute Enable
    											true = mute
    											false = un-mute */
);   

BERR_Code BRAP_RFAUDIO_P_GetMute (
    BRAP_CAPPORT_P_Handle       phCapPort,     /* [in] Pointer to Capture Port handle */
    bool *bMute							/* [out] Mute status */
);

BERR_Code BRAP_RFAUDIO_P_SetBtscOutputMode (
    BRAP_CAPPORT_P_Handle       phCapPort,     /* [in] Pointer to Capture Port handle */
    BRAP_RfAudioBtscOutputMode		eOutputMode	 /* [in] BTSC Output Mode */
);

BERR_Code BRAP_RFAUDIO_P_GetBtscOutputMode (
    BRAP_CAPPORT_P_Handle       phCapPort,     /* [in] Pointer to Capture Port handle */
    BRAP_RfAudioBtscOutputMode		*eOutputMode	 /* [out] BTSC Output Mode */
);

BERR_Code BRAP_RFAUDIO_P_SetEiajOutputMode (
    BRAP_CAPPORT_P_Handle       phCapPort,     /* [in] Pointer to Capture Port handle */
    BRAP_RfAudioEiajOutputMode		eOutputMode	 /* [in] Eiaj Output Mode */
);

BERR_Code BRAP_RFAUDIO_P_GetEiajOutputMode (
	BRAP_CAPPORT_P_Handle       phCapPort,     /* [in] Pointer to Capture Port handle */
    BRAP_RfAudioEiajOutputMode		*eOutputMode	 /* [out] Eiaj Output Mode */
);

BERR_Code BRAP_RFAUDIO_P_SetKoreaA2OutputMode (
	BRAP_CAPPORT_P_Handle       phCapPort,     /* [in] Pointer to Capture Port handle */
	BRAP_RfAudioKoreaA2OutputMode  eOutputMode	  /* [in] Korea A2 Output mode */
);

BERR_Code BRAP_RFAUDIO_P_GetKoreaA2OutputMode (
	BRAP_CAPPORT_P_Handle       phCapPort,     /* [in] Pointer to Capture Port handle */
	BRAP_RfAudioKoreaA2OutputMode  *eOutputMode	/* [out] Korea A2 Output mode */
);

BERR_Code BRAP_RFAUDIO_P_SetNicamOutputMode (
	BRAP_CAPPORT_P_Handle       phCapPort,     /* [in] Pointer to Capture Port handle */
	BRAP_RfAudioNicamOutputMode  eOutputMode	  /* [in] Nicam Output mode */
);

BERR_Code BRAP_RFAUDIO_P_GetNicamOutputMode (
	BRAP_CAPPORT_P_Handle       phCapPort,     /* [in] Pointer to Capture Port handle */
	BRAP_RfAudioNicamOutputMode  *eOutputMode	/* [out] Nicam Output mode */
);

BERR_Code BRAP_RFAUDIO_P_SetPalA2OutputMode (
	BRAP_CAPPORT_P_Handle       phCapPort,     /* [in] Pointer to Capture Port handle */
	BRAP_RfAudioPalA2OutputMode  eOutputMode	  /* [in] PAL-A2 Output mode */
);

BERR_Code BRAP_RFAUDIO_P_GetPalA2OutputMode (
	BRAP_CAPPORT_P_Handle       phCapPort,     /* [in] Pointer to Capture Port handle */
	BRAP_RfAudioPalA2OutputMode  *eOutputMode	/* [out] PAL-A2 Output mode */
);

BERR_Code BRAP_RFAUDIO_P_SetStereoConfig (
    BRAP_CAPPORT_P_Handle       phCapPort,     /* [in] Pointer to Capture Port handle */
    const BRAP_RfAudioStereoConfig	*psStereoCfg	/* [in]  Stereo Config */
);

BERR_Code BRAP_RFAUDIO_P_GetStereoConfig (
    BRAP_CAPPORT_P_Handle       phCapPort,     /* [in] Pointer to Capture Port handle */
    BRAP_RfAudioStereoConfig		*psStereoCfg	/* [out] Stereo Config */
);

BERR_Code BRAP_RFAUDIO_P_SetSapConfig (
    BRAP_CAPPORT_P_Handle       phCapPort,     /* [in] Pointer to Capture Port handle */
    const BRAP_RfAudioSapConfig	*psSapConfig		/* [in] SAP config */
);

BERR_Code BRAP_RFAUDIO_P_GetSapConfig (
    BRAP_CAPPORT_P_Handle       phCapPort,     /* [in] Pointer to Capture Port handle */
    BRAP_RfAudioSapConfig			*psSapConfig		/* [out] SAP config */
);

BERR_Code BRAP_RFAUDIO_P_SetFirmwareReg ( 
	BRAP_CAPPORT_P_Handle		phCapPort,     /* [in] Pointer to Capture Port handle */
	uint32_t					uRegNum,	   /* [in] Reg number 1, 2, 3, 4 */
	bool						bValue		   /* true or false */
);

BERR_Code BRAP_RFAUDIO_P_ScaleOutput ( 
	BRAP_CAPPORT_P_Handle		phCapPort,     /* [in] Pointer to Capture Port handle */
	uint32_t ui32Reg
);

BERR_Code BRAP_RFAUDIO_P_ScaleInput ( 
	BRAP_CAPPORT_P_Handle		phCapPort,     /* [in] Pointer to Capture Port handle */
	uint32_t ui32InputScaleValue				/* [in] Input scale value */
);
#endif /* ( BCHP_CHIP==3548 ) */

#ifdef __cplusplus
}
#endif


#endif /* _BRAP_CAPPORT_PRIV_H_ */

/* End of File */

