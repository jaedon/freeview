/***************************************************************************
*     Copyright (c) 2004-2009, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: brap_fmm_priv.h $
* $brcm_Revision: Hydra_Software_Devel/3 $
* $brcm_Date: 12/23/09 3:47p $
*
* Module Description:
*   Module name: FMM
*   This file lists all data structures, macros, enumerations and function 
*   prototypes for the top level FMM abstraction, which are internal ie NOT
*   exposed to the application developer. These can be used only by the 
*   Audio Manager and other FMM submodules.
*
* Revision History:
*
* $brcm_Log: /magnum/portinginterface/rap/base3/brap_fmm_priv.h $
* 
* Hydra_Software_Devel/3   12/23/09 3:47p sgadara
* SW3556-904: [3548,3556] Adding feature of "Fixed Ringbuffer Capture at
* 48khz sample rate"
* --> Adding function to return SRC Coefficient Index
* 
* Hydra_Software_Devel/2   6/12/09 3:42p gautamk
* PR53302: [7405] Merging cleanup code to mainline
* 
* Hydra_Software_Devel/RAP_7405_CLEANUP/1   3/17/09 3:32p gautamk
* PR53302: [7405]Code cleanup
* 
* Hydra_Software_Devel/1   10/10/08 11:07a speter
* PR 38950: [3548,3556,7405,7325,7335] Moving references from base to
* base3
* 
* Hydra_Software_Devel/21   9/16/08 12:10p bhanus
* PR 46903 : [7440]Fixing compilation break for 7401/7405 etc.
* 
* Hydra_Software_Devel/20   9/15/08 6:17p bhanus
* PR 46903 : [7440] Adding support for programming external MCLK
* 
* Hydra_Software_Devel/19   7/21/08 12:45p speter
* PR 44984: [3548,3556,7405] Merging the changes from Rap_NewFwArch
* branch to Hydra_Software_Devel
* 
* Hydra_Software_Devel/Rap_NewFwArch/1   4/22/08 2:21p gautamk
* PR41994: [7405] Using BCHP_VER_ Correctly...
* 
* Hydra_Software_Devel/18   5/11/07 9:02p sushmit
* PR 27646: Getting 7405 PI compiled
* 
* Hydra_Software_Devel/17   2/22/07 12:34p bhanus
* PR 28040 : Modified code for 7440 B0 porting
* 
* Hydra_Software_Devel/16   10/6/06 5:15p kagrawal
* PR 24717: Merged 7440 Devel code to Hydra_Software_Devel
* 
* Hydra_Software_Devel/15   9/25/06 1:18p kagrawal
* PR 20654: Merged 7440 bring-up code
* 
* Hydra_Software_Devel/14   8/31/06 12:28p rjain
* PR 22868: Moving the hSecureMem handle into BRAP_Settings()
* 
* Hydra_Software_Devel/13   8/23/06 4:30p rjain
* PR 22868: added code to pass a 2nd memory handle (for secure region).
* this 2nd handle is used for RBUF memory allocation. Note: application
* has to take care to pass correct secure memory handle
* 
* Hydra_Software_Devel/11   12/9/05 4:40p rjain
* PR 18264: Capture PI Development
* 
* Hydra_Software_Devel/10   11/22/05 1:55p bselva
* PR 17490: Fix for 44.1KHz I2S output
* 
* Hydra_Software_Devel/9   11/2/05 3:25p bselva
* PR 17590: Added watchdog support for 7401
* 
* Hydra_Software_Devel/8   9/19/05 4:35p rjain
* PR 16899: Making changes to the FMM modules for watchdog recovery.
* 
* Hydra_Software_Devel/7   6/10/05 3:27p rjain
* PR 15837: Moving RBUF memory allocation from FMM_P_Open() to
* RBUF_P_Open() => only memory for opened RBUFs will be allocated, not
* for all 12.
* 
* Hydra_Software_Devel/6   6/7/05 11:44a rjain
* PR 14143: Implemented FMM Code review comments.
* 
* Hydra_Software_Devel/5   6/2/05 10:45a rjain
* PR 14143: Implemented FMM Code review comments.
* 
* Hydra_Software_Devel/4   5/4/05 11:08a rjain
* PR 14507:- Added proper States, open/start counts for modules.
* - Added appropriate checks to handle multiple calls to _Open(),
* _Start(), _Stop(), _Close() for modules.
* - Corrected SPDIFFM handle stored in hFmm / channel handle to be
* SPDIFFM Stream handle.
* - Some clean up, mroe comments/debug messages.
* - Added BRAP_RM_P_GetSpdifFmId() to RM
* 
* Hydra_Software_Devel/3   2/2/05 7:11p nitinb
* PR 13082: Fixing compilation warnings
* 
* Hydra_Software_Devel/2   1/12/05 5:01p rjain
* PR13082: Updating with changes after compilation of FMM, AM and RM
* together.
* 
* Hydra_Software_Devel/1   1/11/05 11:37a rjain
* PR 13082: Moved to new directory structure
* 
***************************************************************************/


#ifndef _BRAP_FMM_PRIV_H_
#define _BRAP_FMM_PRIV_H_


#ifdef __cplusplus
extern "C" {
#endif

#define	BRAP_FMM_P_MAX_EXT_MCLK		2



    
/* Declarations for Cross referenced structures */

/***************************************************************************
Summary:
    Handle to the RBUF Object
    
Description:
    This is the handle that is exposed to the AudioManager. It is actually a 
    pointer to the RBUF Object abstraction

See Also:
    
***************************************************************************/    
typedef struct BRAP_RBUF_P_Object * BRAP_RBUF_P_Handle;

/***************************************************************************
Summary:
    Handle to the Dstination Channel Object
    
Description:
    This is the handle that is exposed to the AudioManager. It is actually a 
    pointer to the Destination Channel Object abstraction
    
See Also:
    
***************************************************************************/
typedef struct BRAP_DSTCH_P_Object * BRAP_DSTCH_P_Handle;

/***************************************************************************
Summary:
    Handle to the Source Channel Object
    
Description:
    This is the handle that is exposed to the AudioManager. It is actually a 
    pointer to the Source Channel Object abstraction
    
See Also:
    
***************************************************************************/
typedef struct BRAP_SRCCH_P_Object * BRAP_SRCCH_P_Handle;

/***************************************************************************
Summary:
    Handle to the Mixer Object
    
Description:
    This is the handle that is exposed to the AudioManager. It is actually a 
    pointer to the Mixer Object abstraction
    
See Also:
    
***************************************************************************/
typedef struct BRAP_MIXER_P_Object * BRAP_MIXER_P_Handle;


/***************************************************************************
Summary:
    Handle to the SPDIF Formatter Object
    
Description:
    This is the handle that is exposed to the AudioManager. It is actually a 
    pointer to the SPDIF Formatter Object abstraction

See Also:

***************************************************************************/
typedef struct BRAP_SPDIFFM_P_Object * BRAP_SPDIFFM_P_Handle;



/***************************************************************************
Summary:
    Handle to an Output port 
    
Description:
    This is the handle that is exposed to the AudioManager. It is actually a 
    pointer to an Output port  abstraction

See Also:

***************************************************************************/
typedef struct BRAP_OP_P_Object * BRAP_OP_P_Handle;



/***************************************************************************
Summary:
    Handle to the FMM Object
    
Description:
    This is the handle that is exposed to the AudioManager. It is actually a 
    pointer to the FMM Object abstraction
    
See Also:
    
***************************************************************************/
typedef struct BRAP_FMM_P_Object   * BRAP_FMM_P_Handle;


/***************************************************************************
Summary:
    Handle to the SRC Object
    
Description:
    This is the handle that is exposed to the AudioManager. It is actually a 
    pointer to the SRC Object abstraction
    
See Also:
    
***************************************************************************/
typedef struct BRAP_SRC_P_Object   * BRAP_SRC_P_Handle;  


/***************************************************************************
Summary:
    Handle to the Capture Port Object
    
Description:
    This is the handle that is exposed to the AudioManager. It is actually a 
    pointer to the Capture Port Object abstraction
    
See Also:
    
***************************************************************************/
typedef struct BRAP_CAPPORT_P_Object    * BRAP_CAPPORT_P_Handle;  


/***************************************************************************
Summary:
    Parameters to be passed by Audio Manager on Opening the FMM module.

Description:
    This lists all the parameters required to open the top level FMM, 
    including both internal and external parameters.
    Note that the internal parameters are not exposed to the Application.
    Currently no Settings are required for top level FMM.

See Also:
    

***************************************************************************/
typedef struct BRAP_FMM_P_Settings
{
	unsigned int tbd;    
}BRAP_FMM_P_Settings;


/***************************************************************************
Summary:
    This structure stores info about the SRAM Memory allocated to SRC
Description:

See Also:

***************************************************************************/
typedef struct BRAP_SRC_P_SramMemLocationInfo
{
    bool            bAllocated;     /* If this SRAM location is already 
                                       allocated */
	unsigned int    uiBaseAddr;     /* Base Address of Memory Location */
    unsigned int    uiSize;         /* Size of Memory Location */
}BRAP_SRC_P_SramMemLocationInfo;

/***************************************************************************
Summary:
    Abstraction of the top level FMM module.
    
Description:
    It contains all the information required to handle the FMM device.
    Particularly, it stores the FMM index (in case there are multiple FMMs),
    handles for all required chip related information, handles of all 
    associated submodules etc. 

See Also:
    All submodule handles.
***************************************************************************/
typedef struct BRAP_FMM_P_Object
{
    BRAP_FMM_P_Settings sSettings;  /* Settings passed by Application for FMM
                                       device open */
    BCHP_Handle          hChip;     /* Handle to chip object */
    BREG_Handle          hRegister; /* Handle to register object */
    BMEM_Handle          hHeap;     /* Handle to memory object */
    BINT_Handle          hInt;      /* Handle to interrupt object */

    unsigned int         uiIndex;   /* FMM index */
    uint32_t             ui32Offset;  /* Offset of a register of current FMM 
                                       from the corresponding register
                                       of the first FMM  */    

    BRAP_Handle            hRap;      /* AUD handle */
    BRAP_RBUF_P_Handle     hRBuf[BRAP_RM_P_MAX_RBUFS]; /* Ring buffer handles */
    BRAP_SRCCH_P_Handle    hSrcCh[BRAP_RM_P_MAX_SRC_CHANNELS]; /* Source Channel 
                                                               handles */
	BRAP_DSTCH_P_Handle    hDstCh[BRAP_RM_P_MAX_DST_CHANNELS]; /* Destination Channel 
                                                               handles */

    BRAP_SPDIFFM_P_Handle  hSpdifFm[BRAP_RM_P_MAX_SPDIFFM_STREAMS];   /* SPDIFFM Stream handles */
    BRAP_OP_P_Handle       hOp[BRAP_RM_P_MAX_OUTPUTS];        /* O/P handles */

    BRAP_SRC_P_Handle      hSrc[BRAP_RM_P_MAX_SRC_BLCK]
                               [BRAP_RM_P_MAX_SRC_PER_SRC_BLCK];/* SRC Handles */
    BRAP_SRC_P_SramMemLocationInfo *pSrcMemLocationInfo[BRAP_RM_P_MAX_SRC_BLCK];
    bool                            bSrcCoeffUsed[BRAP_RM_P_MAX_SRC_BLCK][360];  
    BRAP_MIXER_P_Handle    hMixer[BRAP_RM_P_MAX_DP_BLCK]
                                 [BRAP_RM_P_MAX_MIXER_PER_DP_BLCK];  /* Mixer handles */
    BRAP_CAPPORT_P_Handle   hCapPort[BRAP_CapInputPort_eMax];  /*CAP Port Handle*/

    uint32_t                ui32MixerPbFciId[BRAP_RM_P_MAX_DP_BLCK]
                                            [BRAP_RM_P_MAX_MIXER_PB_FCI_ID];
    uint32_t                ui32MixerPbFciIdCount[BRAP_RM_P_MAX_DP_BLCK]
                                            [BRAP_RM_P_MAX_MIXER_PB_FCI_ID];
    BRAP_OP_ExtMClkSettings sExtMClkSettings[BRAP_FMM_P_MAX_EXT_MCLK];
}BRAP_FMM_P_Object;


/***************************************************************************
Summary:
    Opens the top level FMM module.

Description:
    This function is called only by the AudioManager on device Open.
    It allocates required memory and clears/resets required hardware
    registers of all submodules.

Returns:
    BERR_SUCCESS on success
    Error code on failure

See Also:
    BRAP_FMM_P_Close
    
****************************************************************************/
BERR_Code BRAP_FMM_P_Open (
    BRAP_Handle					hRap,        /* [in] Audio Device Handle */
    BRAP_FMM_P_Handle *         phFmm,        /* [out] Pointer to FMM Handle*/
    unsigned int                uiFmmIndex,  /* [in] FMM Index */
    const BRAP_FMM_P_Settings * pSettings   /* [in] FMM Settings */
);

/***************************************************************************
Summary:
    Closes the top level FMM module.

Description:
    This function is called only by the AudioManager on device Close.
    It allocates frees memory and clears/resets required hardware
    registers of all submodules.

Returns:
    BERR_SUCCESS on success
    Error code on failure

See Also:
    BRAP_FMM_P_Open
    
****************************************************************************/
BERR_Code BRAP_FMM_P_Close (
    BRAP_FMM_P_Handle hFmm        /* [in] FMM Device Handle */
);



/***************************************************************************
Summary: Gets the watchdog recovery flag 

Description:
	This function returns current status of watchdog recovery flag. This
	is required to know whether a function is getting called in watchdog
	context or from application.

Returns:
	Watchdog flag

See Also:
BRAP_P_GetWatchdogRecoveryFlag, BRAP_DSP_P_GetWatchdogRecoveryFlag
**************************************************************************/
bool BRAP_FMM_P_GetWatchdogRecoveryFlag (
        BRAP_FMM_P_Handle hFmm       /* [in] FMM Device Handle */
);

/***************************************************************************
Summary: Resets the FMM Block

Description:

Returns:
	None

See Also:
**************************************************************************/
void BRAP_FMM_P_ResetHardware (
	BRAP_FMM_P_Handle hFmm /* [in] FMM Device Handle */
);
#ifdef __cplusplus
}
#endif


#endif /* _BRAP_FMM_PRIV_H_ */


/* End of File */
