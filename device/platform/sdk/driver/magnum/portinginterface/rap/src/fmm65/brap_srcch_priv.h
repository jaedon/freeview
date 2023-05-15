/***************************************************************************
*     Copyright (c) 2004-2009, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: brap_srcch_priv.h $
* $brcm_Revision: Hydra_Software_Devel/3 $
* $brcm_Date: 6/12/09 3:37p $
*
* Module Description:
*   Module name: SRCCH
*   This file lists all data structures, macros, enumerations and function 
*   prototypes for the Source Channel (Fifo) abstraction, which are internal ie NOT
*   exposed to the application developer. These can be used only by the 
*   Audio Manager and other FMM submodules.
*   
* Revision History:
*
* $brcm_Log: /magnum/portinginterface/rap/base3/brap_srcch_priv.h $
* 
* Hydra_Software_Devel/3   6/12/09 3:37p gautamk
* PR53302: [7405] Merging cleanup code to mainline
* 
* Hydra_Software_Devel/2   9/9/08 6:50p sushmit
* PR 46211: [7405] Adding Data swapping support for PCM Playback
* 
* Hydra_Software_Devel/1   7/21/08 2:24p speter
* PR 44984: [3548,3556,7405] Adding files from base which have new
* Architecture specific changes and cannot be merged back in base
* 
* Hydra_Software_Devel/20   3/26/08 5:02p srajapur
* PR36670 : [7400,7118,7403,7401] Set audio channels mode to
* baudio_downmix_right, plug out and plug in HDMI cable will cause HDMI
* audio lost
* --->Added the code for setting the dualmono mode settings.
* --->Added a private routine to program the LR DATA control field
* 
* Hydra_Software_Devel/19   12/28/06 12:30p kagrawal
* PR 25607: [7440/3563] Added BRAP_SRCCH_P_GetCurrentParams()
* 
* Hydra_Software_Devel/18   10/6/06 5:22p kagrawal
* PR 24717: Merged 7440 Devel code to Hydra_Software_Devel
* 
* Hydra_Software_Devel/17   9/25/06 1:19p kagrawal
* PR 20654: Merged 7440 bring-up code
* 
* Hydra_Software_Devel/16   9/12/06 4:19p rjain
* PR 22760: Support for unsigned data in raptor
* 
* Hydra_Software_Devel/15   9/7/06 11:20a bhanus
* PR 22760 : Added support for 8Bit Unsigned Playback
* 
* Hydra_Software_Devel/14   3/1/06 9:40a kagrawal
* PR 18264: Implemented review comments for PCM playback
* 
* Hydra_Software_Devel/13   2/17/06 9:56a sushmit
* PR18604: Merging Encoder to latest.
* 
* Hydra_Software_Devel/RAP_AUD_ENC/2   2/6/06 4:18p sushmit
* PR18604: Merging latest Decoder PI to Encoder PI branch
* 
* Hydra_Software_Devel/12   12/26/05 5:05p kagrawal
* PR 18264: Added bCapDirect2SrcFIFO in sSrcChParams
* 
* Hydra_Software_Devel/11   12/3/05 4:54p kagrawal
* PR 18312: Removed unused ePcmOutputMode from source channel start
* interface
* 
* Hydra_Software_Devel/10   11/14/05 9:54a rjain
* PR 18129: Adding support for routing to multiple output ports
* 
* Hydra_Software_Devel/9   11/10/05 11:55a kagrawal
* PR 17590: Merging PCM Playback and Mixing changes to
* Hydra_Software_Devel
* 
* Hydra_Software_Devel/Kapil_PCM_Playback/3   11/9/05 10:28a kagrawal
* PR 17590: Added Rbuf ids in BRAP_SRCCH_P_Params structure
* 
* Hydra_Software_Devel/Kapil_PCM_Playback/2   11/1/05 9:45a kagrawal
* Implemented review comments on PCM playback and mixing
* 
* Hydra_Software_Devel/Kapil_PCM_Playback/1   10/24/05 5:04p kagrawal
* PR 17590: Initial check in for PCM Playback and Mixing
* 
* Hydra_Software_Devel/7   6/2/05 10:49a rjain
* PR 14143: Implemented FMM Code review comments.
* 
* Hydra_Software_Devel/6   3/10/05 3:25p rjain
* PR 13082:
* - Implemented the new Start-Stop scheme where Hosts prgms all
* registers. DSP uses Start_WRPoint and RBUF_EndAddress to start decode.
* - removed bMixingEnable from BRAP_MIXER_Params
* - removed sRBufParams from BRAP_DEC_AudioParams.
* - Added BRAP_RM_P_GetDpStreamId
* - Corrected programming of Stream Id in DP registers
* - Added BRAP_SRCCH_P_WaitForRampDown()
* - Other minor changes
* 
* Hydra_Software_Devel/5   2/2/05 7:12p nitinb
* PR 13082: Fixing compilation warnings
* 
* Hydra_Software_Devel/4   1/25/05 9:54a rjain
* PR13082: Modifying according to settings/bugs highlighted in emulation
* so far.
* 
* Hydra_Software_Devel/3   1/12/05 5:03p rjain
* PR13082: Updating with changes after compilation of FMM, AM and RM
* together.
* 
* Hydra_Software_Devel/2   1/11/05 11:57a rjain
* PR13082: Adding enum for DMA Slot Client ID
* 
* Hydra_Software_Devel/1   1/11/05 11:41a rjain
* PR 13082: Moved to new directory structure
* 
* 
***************************************************************************/


#ifndef _BRAP_SRCCH_PRIV_H_
#define _BRAP_SRCCH_PRIV_H_

#include "brap.h"
#include "brap_priv.h"

#ifdef __cplusplus
extern "C" {
#endif


/***************************************************************************
Summary:
    This enumeration defines which Source Channel to use as DMA Client

***************************************************************************/
typedef enum BRAP_SRCCH_P_DmaClientId
{
    BRAP_SRCCH_P_DmaClientId_e0 = 0, /* Select Src Ch 0 as DMA Client */
    BRAP_SRCCH_P_DmaClientId_e1,     /* Select Src Ch 1 as DMA Client */
    BRAP_SRCCH_P_DmaClientId_e2,     /* Select Src Ch 2 as DMA Client */
    BRAP_SRCCH_P_DmaClientId_e3,     /* Select Src Ch 3 as DMA Client */
    BRAP_SRCCH_P_DmaClientId_e4,     /* Select Src Ch 4 as DMA Client */
    BRAP_SRCCH_P_DmaClientId_e5,     /* Select Src Ch 5 as DMA Client */
    BRAP_SRCCH_P_DmaClientId_e6,     /* Select Src Ch 6 as DMA Client */
    BRAP_SRCCH_P_DmaClientId_e7      /* Select Src Ch 7 as DMA Client */

}BRAP_SRCCH_P_DmaClientId;

    
/***************************************************************************
Summary:
    This enumeration defines source channel state.

***************************************************************************/
typedef enum BRAP_SRCCH_P_State
{
    BRAP_SRCCH_P_State_eStopped = 0,    /* Stopped */
    BRAP_SRCCH_P_State_eRunning,        /* Running */
    BRAP_SRCCH_P_State_ePaused          /* Paused */

}BRAP_SRCCH_P_State;

/***************************************************************************
Summary:
    This enumeration defines LR data control as per RDB.

***************************************************************************/
typedef enum BRAP_SRCCH_P_LRDataControl
{
	BRAP_SRCCH_P_LRDataControl_LR_2_LR = 0, /* Left channel outputs as Left, 
											   Right channel outputs as right, 
											   no change */
	BRAP_SRCCH_P_LRDataControl_LR_2_RL = 1, /* Left channel outputs as Right, 
											   Right channel outputs as left */
	BRAP_SRCCH_P_LRDataControl_L_2_LR = 2,	/* Left channel outputs to both 
											   Left and right */	
	BRAP_SRCCH_P_LRDataControl_R_2_LR = 3 	/* Right channel outputs to both 
											   Left and right */	
}BRAP_SRCCH_P_LRDataControl;

/***************************************************************************
Summary:
    Parameters to be passed by Audio Manager on Opening a Source Channel.
    Note that this includes both public and private settings.

Description:
    No external settings at present.


See Also:

***************************************************************************/
typedef struct BRAP_SRCCH_P_Settings
{
	unsigned int tbd;
} BRAP_SRCCH_P_Settings;

/***************************************************************************
Summary:
    Parameters to be passed by Audio Manager on Starting a Source Channel.
    Note that this includes both public and private parameters.

Description:

Notes:
    If the stream sampling rate and the output sampling rate are different
    for a PCM playback or captured stream, SRC (Sampling rate conversion) 
    will be enabled. No SRC is done on decoded streams.

    No external parameters at present.

See Also:

***************************************************************************/
typedef struct BRAP_SRCCH_P_Params
{
    bool                bInputSrValid;  /* TRUE: If the input audio data 
                                           coming from DSP has a valid SR
                                           FALSE: Otherwise */

	BRAP_BufDataMode 	eBufDataMode;	/* Determines the mode of data in the 
										   buffer */ 
	
    bool                bCompress;      /* TRUE: The input is compressed data
                                           FALSE: The input is uncompressed */
    bool                bSharedRbuf ;   /* TRUE: This src channel shares 
                                                    the RBUF with another 
                                                    SrcChannel and behaves as 
                                                    slave */
    unsigned int     uiMasterSrcCh;  /* SrcCh Id of the Master SrcCh. valid 
                                                    only when bSharedRbuf is TRUE  */ 

    BAVC_AudioSamplingRate  eInputSR;   /* Sampling rate of the stream input
                                           to the Source Channel. 
                                           If bInputSrValid is FALSE,this value
                                           has been provided by the application.
                                           If bInputSrValid is TRUE, this value
                                           has been provided by DSP */

    BAVC_AudioSamplingRate  eOutputSR;  /* Sampling rate at the output of this 
                                           Source Channel. Also the sampling 
                                           rate at the Mixer Input */

	BRAP_InputBitsPerSample eInputBitsPerSample;
									   /* Bit resolution of the source channel */
	BRAP_SRCCH_P_LRDataControl eLRDataCntl;
									   /* LR Data flow control */
	bool 					bNotPauseWhenEmpty;
									   /* Flag to indicate if the same data to 
									   	  be comsumed again in the loop */
	bool 					bStartSelection;
									   /* If false, START PLAY from the data in 
									      dram at the address specified by read 
									      address immediately.
									      If true, START PLAY from the data in 
									      dram at the address specified by read 
									      address when write point exceeds value 
									      in its SOURCE_RINGBUF_x_START_WRPOINT */	
    bool                    bCapDirect2SrcFIFO;
                                       /* TRUE: if data is directly captured to
                                          the source FIFO for playback e.g. Bypass
                                          mode of a capture channel
                                          FALSE: for a normal playback where data
                                          comes in the ring buffer first and then 
                                          to the source FIFO. e.g. decode channel,
                                          playback channel, full duplex mode of a 
                                          capture channel, etc. */

    bool			        bProcessIdHigh;	
                                       /* Flag to indicate DMA Client Priority 
                                          Level */
	unsigned int 	        uiGroupId; /* Group ID to which this channel 
	                                      belongs */
#if BRAP_UNSIGNED_PCM_SUPPORTED
    bool                            bIsSigned;
                                    /* Indicates PCM Data Format. 
                                       TRUE: The PCM data is signed
                                       FALSE: The PCM data is unsigned */
#endif
    BRAP_DataEndian     eDataSwap;  /* To select whether FMM takes data as per
                                       system endian-ness or swapped */
}BRAP_SRCCH_P_Params; 

/***************************************************************************
Summary:
    Abstraction of a Source Channel 
    
Description:
    It contains all the information required to handle the Source Channel
    Particularly, it stores the indexes, handles for all required chip 
    related information, parent FMM handle, offset required to access 
    different source channels etc

Notes:    
    Mute can be done at 3 places:
    i) At source channel o/p: This will mute the source channel o/p itself, 
       so if this source channel goes to multiple mixer inputs all of them 
       will be muted. Handled in the Source Channel submodule.
   ii) At mixer input: This will mute a specific input of a mixer. Here 
       if the source channel goes to other mixer inputs also, then those 
       mixer inputs will not be affected by this mute. 
       Handled in the Mixer submodule.
  iii) At mixer output: This will mute the mixer ouput itself.
       Handled in the Mixer submodule.

See Also:
    
***************************************************************************/
typedef struct BRAP_SRCCH_P_Object
{
    BRAP_SRCCH_P_Settings   sSettings; /* Source Channel settings provided 
                                          during Open() */
    BRAP_SRCCH_P_Params     sParams;   /* Source Channel settings  
                                          provided during Start() */

    BCHP_Handle             hChip;     /* Handle to chip object */
    BREG_Handle             hRegister; /* Handle to register object */
    BMEM_Handle             hHeap;     /* Handle to memory object */
    BINT_Handle             hInt;      /* Handle to interrupt object */
    BRAP_FMM_P_Handle       hFmm;      /* Parent FMM handle */

    unsigned int            uiIndex;   /* Source Channel index */
    uint32_t                ui32Offset;  /* Offset of a register of current 
                                          source channel from the 
                                          corresponding register of the 
                                          first source channel */

    BRAP_SRCCH_P_State      eState;     /* State of the source channel */
    bool                    bMute;      /* Flag to indicate if this source 
                                           channel ouput has been muted */
	bool			bProcessIDHigh;	/*Flag to indicate DMA Client Priority Level */
	unsigned int 	uiGroupId;		/*The ID of the group of which this channel is a Part*/
}BRAP_SRCCH_P_Object;


/***************************************************************************
Summary:
    Opens a Source Channel.

Description:
    Initializes the Source Channel and returns a Source channel handle.    
    The handle can then be used for all other Source channel function calls.

Returns:
    BERR_SUCCESS on success
    Error code on failure

See Also:
    BRAP_SRCCH_P_Close, BRAP_SRCCH_P_GetDefaultSettings.
**************************************************************************/
BERR_Code BRAP_SRCCH_P_Open (
    BRAP_FMM_P_Handle       hFmm,            /* [in] Parent FMM handle */
    BRAP_SRCCH_P_Handle *   phSrcCh,         /* [out] Pointer to Source 
                                                Channel handle */
    unsigned int            uiSrcChIndex,    /* [in] Source channel index */
    const BRAP_SRCCH_P_Settings * pSettings  /* [in] Source channel settings */
);

/***************************************************************************
Summary:
    Releases all the resources associated with this Source Channel and frees 
    the handles.

Description:
    

Returns:
    BERR_SUCCESS on success
    Error code on failure

See Also:
    BRAP_SRCCH_P_Open
**************************************************************************/
BERR_Code BRAP_SRCCH_P_Close ( 
    BRAP_SRCCH_P_Handle hSrcCh      /* [in] Source Channel Handle */
);



/***************************************************************************
Summary:
    Starts a Source Channel.

Description:
    To start the Source FIFO taking data from ring buffer, this function must be
    called. Source channel will start based on the existing settings. 

Returns:
    BERR_SUCCESS on success
    Error code on failure

See Also:
    BRAP_SRCCH_P_Stop.
**************************************************************************/
BERR_Code BRAP_SRCCH_P_Start ( 
    BRAP_SRCCH_P_Handle     hSrcCh,        /* [in] Source Channel Handle */
    const BRAP_SRCCH_P_Params *   pParams  /* [in] Pointer to start
                                              parameters for this Source 
                                              Channel */ 
);



/***************************************************************************
Summary:
    Stops a Source Channel.

Description:
  

Returns:
    BERR_SUCCESS on success
    Error code on failure

See Also:
    BRAP_SRCCH_Start.
**************************************************************************/
BERR_Code BRAP_SRCCH_P_Stop ( 
    BRAP_SRCCH_P_Handle     hSrcCh        /* [in] Source Channel Handle */
);



/***************************************************************************
Summary:
    Returns default values for Source Channel Open time settings.

Description:
    For settings that the system cannot assign default values to, 
    an invalid value is returned. Note that the default settings are common
    for all Source Channels.

Returns:
    BERR_SUCCESS on success
    Error code on failure

See Also:
    BRAP_SRCCH_P_GetDefaultParams.
**************************************************************************/
BERR_Code 
BRAP_SRCCHF_P_GetDefaultSettings ( 
    BRAP_SRCCH_P_Settings   * pDefSettings  /*[out] Pointer to memory where 
                                              default settings should be 
                                              written */
);

/***************************************************************************
Summary:
    Returns default values for Source Channel Start time parameters.

Description:
    For parameters that the system cannot assign default values to, 
    an invalid value is returned. Note that the default parameters are common
    for all Source Channels.

Returns:
    BERR_SUCCESS on success
    Error code on failure

See Also:
    BRAP_SRCCH_P_GetDefaultSettings.
**************************************************************************/
BERR_Code 
BRAP_SRCCH_P_GetDefaultParams ( 
    BRAP_SRCCH_P_Params    *pDefParams   /*[out] Pointer to memory where 
                                           default settings should be 
                                           written */ 
);

/***************************************************************************
Summary:
    Mutes or un-Mutes the output of a Source Channel 

Description:
    For an uncompressed stream, this function Mutes with a Ramp down.
    Default ramp size is 80h => ramp down happens in 256fs.
    This function does nothing if Scale Ramping has been disabled.
    It returns error for a compressed stream.

Returns:
    BERR_SUCCESS on success
    Error code on failure 

See Also:
    BRAP_SRCCH_P_GetMute(), BRAP_SRCCH_P_WaitForRampDown()
    
**************************************************************************/
BERR_Code BRAP_SRCCH_P_SetMute ( 
    BRAP_SRCCH_P_Handle  hSrcCh,   /* [in] Source Channel Handle */
    BRAP_MIXER_P_Handle  hMixer,   /* [in] Mixer handle of connected mixer*/    
    bool                 bMute     /* [in] Mute enable/disable:
                                          TRUE: Mute source channel
                                          FALSE: Un-Mute */                     
);


/***************************************************************************
Summary:
    Gets whether or not the output of a Source Channel has been Muted.

Description:
    This function gets the mute state of the output of a Source Channel.\
    Valid only for an uncompressed stream.
    It returns error for a compressed stream.

Returns:
    BERR_SUCCESS on success
    Error code on failure 

See Also:
    BRAP_SRCCH_P_SetMute().
    
**************************************************************************/
BERR_Code BRAP_SRCCH_P_GetMute ( 
    BRAP_SRCCH_P_Handle  hSrcCh,   /* [in] Source Channel Handle */
    bool *               pMute     /* [out] Pointer to application memory 
                                                to write Mute Status */        
);


/***************************************************************************
Summary:
    Waits for Mute Ramp Down to complete.

Description:


Returns:
    BERR_SUCCESS on success
    Error code on failure

See Also:
    BRAP_SRCCH_P_SetMute(), BRAP_SRCCH_P_GetMute

**************************************************************************/
BERR_Code BRAP_SRCCH_P_WaitForRampDown ( 
    BRAP_SRCCH_P_Handle  hSrcCh,   /* [in] Source Channel Handle */
    BRAP_MIXER_P_Handle  hMixer   /* [in] Mixer handle of connected mixer*/    

);

/***************************************************************************
Summary:
    This private routine gets the current SrcCh params.
***************************************************************************/
BERR_Code 
BRAP_SRCCH_P_GetCurrentParams (
    BRAP_SRCCH_P_Handle     hSrcCh,         /* [in] SrcCh handle */
    BRAP_SRCCH_P_Params     *pCurParams     /* [out] Pointer to memory where 
                                             current params should be written */    
);

/***************************************************************************
Summary:
    This private routine programs the LR DATA control field
***************************************************************************/
BERR_Code BRAP_SRCCH_P_ProgramLRControl ( 
    BRAP_SRCCH_P_Handle     hSrcCh        /* [in] Source Channel Handle */
);


#ifdef __cplusplus
}
#endif


#endif /* _BRAP_SRCCH_PRIV_H_ */

/* End of File */
