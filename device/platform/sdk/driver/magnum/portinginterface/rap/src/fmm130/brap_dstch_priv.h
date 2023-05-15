/***************************************************************************
*     Copyright (c) 2004-2006, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: brap_dstch_priv.h $
* $brcm_Revision: Hydra_Software_Devel/5 $
* $brcm_Date: 2/17/06 10:00a $
*
* Module Description:
*   Module name: DSTCH
*   This file lists all data structures, macros, enumerations and function 
*   prototypes for the Destination Channel (Fifo) abstraction, which are 
*   internal ie NOT exposed to the application developer. These can be used 
*   only by the Audio Manager and other FMM submodules. It covers the 
*	functionality of the Capture Hardware Block also. This object is only 
*	used by a PCM Capture Channel
*   
* Revision History:
*
* $brcm_Log: /magnum/portinginterface/rap/base/brap_dstch_priv.h $
* 
* Hydra_Software_Devel/5   2/17/06 10:00a sushmit
* PR18604: Merging Encoder to latest.
* 
* Hydra_Software_Devel/RAP_AUD_ENC/2   2/6/06 4:23p sushmit
* PR18604: Merging latest Decoder PI to Encoder PI branch
* 
* Hydra_Software_Devel/4   1/6/06 5:45p kagrawal
* PR 18264: Support for Full Duplex Capture Mode. Enabled DMA for
* Destination FIFO
* 
* Hydra_Software_Devel/3   12/9/05 4:40p rjain
* PR 18264: Capture PI Development
* 
* Hydra_Software_Devel/2   11/25/05 7:36p bmishra
* PR 18264: Adding initial code for the Destination Channel
* 
* Hydra_Software_Devel/1   11/25/05 12:42p bmishra
* PR 18264: Capture PI Development
*
***************************************************************************/


#ifndef _BRAP_DSTCH_PRIV_H_
#define _BRAP_DSTCH_PRIV_H_

#include "brap.h"
#include "brap_priv.h"

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************
Summary:
    This enumeration defines which Dest Channel to use as DMA Client
  
***************************************************************************/
typedef enum BRAP_DSTCH_P_DmaClientId
{
    BRAP_DSTCH_P_DmaClientId_e0 = 0x10, /* Select Dst Ch 0 as DMA Client */
    BRAP_DSTCH_P_DmaClientId_e1 = 0x11  /* Select Dst Ch 1 as DMA Client */
}BRAP_DSTCH_P_DmaClientId;

/***************************************************************************
Summary:
    This enumeration defines source channel state.

***************************************************************************/
typedef enum BRAP_DSTCH_P_State
{
    BRAP_DSTCH_P_State_eStopped = 0,    /* Stopped */
    BRAP_DSTCH_P_State_eRunning,        /* Running */
    BRAP_DSTCH_P_State_ePaused          /* Paused */

}BRAP_DSTCH_P_State;

/***************************************************************************
Summary:
    Parameters to be passed by Audio Manager on Opening a Destination Channel.
    Note that this includes both public and private settings.

Description:
    No external settings at present.


See Also:

***************************************************************************/
typedef struct BRAP_DSTCH_P_Settings
{
	unsigned int tbd;
	
} BRAP_DSTCH_P_Settings;

/***************************************************************************
Summary:
    Parameters to be passed by Audio Manager on Starting a Destination Channel.
    Note that this includes both public and private parameters.

Description:

Notes:
    If the stream sampling rate and the output sampling rate are different
    for a PCM playback or captured stream, SRC (Sampling rate conversion) 
    will be enabled. No SRC is done on decoded streams.

    No external parameters at present.

See Also:

***************************************************************************/
typedef struct BRAP_DSTCH_P_Params
{
	BRAP_DSTCH_Params	sExtParams;		/* External Parameters received from 
										   the Application */
	BRAP_CaptureMode	eCapMode;		/* Capture mode: Whether this channel 
										   is used for capture only or for 
										   full duplex (captured data goes to 
										   ring buffers and played back at the 
										   output ) or by-pass mode (capture 
										   data goes payed back at the output 
										   port without coming to ring 
										   buffers) */
    BRAP_BufDataMode 	eBufDataMode;	/* Determines the mode of data in the 
										   buffer */ 
	BRAP_CapInputType	eCapInputType;	/* Type of PCM Capture Input Source */
	bool 				bNotPauseWhenFull;
									   /* Flag to indicate if capture should 
										  continue if corresponding ring 
										  buffers are full. TRUE: Continue 
										  Capture when the Ring buffer(s) 
										  is(are) full, FALSE: Pause Capture 
										  when when the Ring buffer(s) 
										  is(are) full */
	unsigned int 		uiLtOrSnglRBufId;
									   /* Left or single rbuf id that should be 
									   	  associated to this destination channel */
	unsigned int 		uiRtRBufId;
									   /* Right rbuf id that should be associated 
									   	  to this source channel */
	unsigned int        uiSrcChId;     /* Id of the source channel to be connected. 
										  This is ignored if eCapMode ==  
										  BRAP_CAP_CaptureMode_eCaptureOnly  */
	BRAP_InputBitsPerSample	eInputBitsPerSample;/* Input bits per sample of PCM data*/   
    
}BRAP_DSTCH_P_Params; 

/***************************************************************************
Summary:
    Abstraction of a Destination Channel 
    
Description:
    It contains all the information required to handle the Destination Channel
    Particularly, it stores the indexes, handles for all required chip 
    related information, parent FMM handle, offset required to access 
    different source channels etc

See Also:
    
***************************************************************************/
typedef struct BRAP_DSTCH_P_Object
{
    BRAP_DSTCH_P_Settings   sSettings; /* Destination Channel settings provided 
                                          during Open() */
    BRAP_DSTCH_P_Params     sParams;   /* Destination Channel settings  
                                          provided during Start() */

    BCHP_Handle             hChip;     /* Handle to chip object */
    BREG_Handle             hRegister; /* Handle to register object */
    BMEM_Handle             hHeap;     /* Handle to memory object */
    BINT_Handle             hInt;      /* Handle to interrupt object */
    BRAP_FMM_P_Handle       hFmm;      /* Parent FMM handle */

    unsigned int            uiIndex;   /* Destination Channel index */
    uint32_t                ui32Offset;  /* Offset of a register of current 
                                          destination channel from the 
                                          corresponding register of the 
                                          first destination channel */

    BRAP_DSTCH_P_State      eState;     /* State of the destination channel */

}BRAP_DSTCH_P_Object;


/***************************************************************************
Summary:
    Opens a Destination Channel.

Description:
    Initializes the Destination Channel and returns a Destination channel handle.    
    The handle can then be used for all other Destination channel function calls.

Returns:
    BERR_SUCCESS on success
    Error code on failure

See Also:
    BRAP_DSTCH_P_Close, BRAP_DSTCH_P_GetDefaultSettings.
**************************************************************************/
BERR_Code BRAP_DSTCH_P_Open (
    BRAP_FMM_P_Handle       hFmm,            /* [in] Parent FMM handle */
    BRAP_DSTCH_P_Handle *   phDstCh,         /* [out] Pointer to Destination 
                                                Channel handle */
    unsigned int            uiDstChIndex,    /* [in] Destination channel index */
    const BRAP_DSTCH_P_Settings * pSettings  /* [in] Destination channel settings */
);

/***************************************************************************
Summary:
    Releases all the resources associated with this Destination Channel 
    and frees the handles.

Description:
    

Returns:
    BERR_SUCCESS on success
    Error code on failure

See Also:
    BRAP_DSTCH_P_Open
**************************************************************************/
BERR_Code BRAP_DSTCH_P_Close ( 
    BRAP_DSTCH_P_Handle hDstCh      /* [in] Destination Channel Handle */
);



/***************************************************************************
Summary:
    Starts a Destination Channel.

Description:
    To start the Destination FIFO taking data from ring buffer, this function 
    must be called. Destination channel will start based on the existing settings. 

Returns:
    BERR_SUCCESS on success
    Error code on failure

See Also:
    BRAP_DSTCH_P_Stop.
**************************************************************************/
BERR_Code BRAP_DSTCH_P_Start (
    BRAP_DSTCH_P_Handle			hDstCh,   /* [in] Destination Channel Handle */
    const BRAP_DSTCH_P_Params	*pParams  /* [in] Pointer to start parameters 
											 for this Destination Channel */ 
);



/***************************************************************************
Summary:
    Stops a Destination Channel.

Description:
  

Returns:
    BERR_SUCCESS on success
    Error code on failure

See Also:
    BRAP_DSTCH_Start.
**************************************************************************/
BERR_Code BRAP_DSTCH_P_Stop ( 
    BRAP_DSTCH_P_Handle     hDstCh        /* [in] Destination Channel Handle */
);




/***************************************************************************
Summary:
    Returns default values for Destination Channel Start time parameters.

Description:
    For parameters that the system cannot assign default values to, 
    an invalid value is returned. Note that the default parameters are common
    for all Destination Channels.

Returns:
    BERR_SUCCESS on success
    Error code on failure

See Also:
    BRAP_DSTCH_P_GetDefaultSettings.
**************************************************************************/
BERR_Code 
BRAP_DSTCH_P_GetDefaultParams ( 
    BRAP_DSTCH_P_Params    *pDefParams   /*[out] Pointer to memory where 
                                           default settings should be 
                                           written */ 
);

#ifdef __cplusplus
}
#endif


#endif /* _BRAP_DSTCH_PRIV_H_ */

/* End of File */
