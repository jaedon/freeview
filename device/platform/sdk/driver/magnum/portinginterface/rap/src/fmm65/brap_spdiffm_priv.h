/***************************************************************************
*     Copyright (c) 2004-2009, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: brap_spdiffm_priv.h $
* $brcm_Revision: Hydra_Software_Devel/3 $
* $brcm_Date: 10/6/09 4:27p $
*
* Module Description:
*   Module name: SPDIFFM
*   This file lists all data structures, macros, enumerations and function 
*   prototypes for the SPDIF Formatter abstraction, which are internal ie NOT
*   exposed to the application developer. These can be used only by the 
*   Audio Manager and other FMM submodules.
* Revision History:
*
* $brcm_Log: /magnum/portinginterface/rap/base3/brap_spdiffm_priv.h $
* 
* Hydra_Software_Devel/3   10/6/09 4:27p gautamk
* SW7405-3063:[7405]Programming Repetition period 4 for DDP
* 
* Hydra_Software_Devel/2   6/12/09 3:37p gautamk
* PR53302: [7405] Merging cleanup code to mainline
* 
* Hydra_Software_Devel/1   7/21/08 2:24p speter
* PR 44984: [3548,3556,7405] Adding files from base which have new
* Architecture specific changes and cannot be merged back in base
* 
* Hydra_Software_Devel/Rap_NewFwArch/1   4/8/08 10:11a nitinb
* PR 39989: [7405] Added support for DDP pass through
* 
* Hydra_Software_Devel/24   12/11/07 12:28p bhanus
* PR 36468 : [7440] Reverted the code change  to disable the SPDIF clock
* and re-enbale on timer basis while transitioning from PCM - Comp or
* vice versa.
* 
* Hydra_Software_Devel/23   10/26/07 1:46p bhanus
* PR 36468 : [7440] Added code to disable the SPDIF clock and re-enbale
* on timer basis while transitioning from PCM - Comp or vice versa.
* 
* Hydra_Software_Devel/22   10/5/07 7:39p gdata
* PR35413 : [7440] merging the code to main branch
* 
* Hydra_Software_Devel/21   8/19/07 8:20p kagrawal
* PR 34154: [7440] Added CP_TOGGLE_RATE programming support
* 
* Hydra_Software_Devel/20   3/28/07 6:15p nitinb
* PR 28177: [3563, 7440] Fixed build issues for 7401, 7400 chips
* 
* Hydra_Software_Devel/19   3/28/07 5:15p nitinb
* PR 28177: Moved macro BRAP_SPDIFFM_CBIT_BUFFER_SIZE from brap_spdiffm.c
* to this file. Added fields bUseSpdifPackedChanStatusBits and
* sSpdifChanStatusBits to struct BRAP_SPDIFFM_P_Params. Added prototype
* of function BRAP_SPDIFFM_P_ProgramCBITBuffer_isr that is made non-
* static in brap_spdiffm.c
* 
* Hydra_Software_Devel/18   2/6/07 4:57p rjain
* PR 26509: [All chips] Changes to insert a dither signal or null/pause
* burst when muting SPDIF/HDMI
* 
* Hydra_Software_Devel/17   11/29/06 11:26a bhanus
* PR 25607 : Modified for SRC bringup for 7440
* - Modified interface BRAP_SPDIFFM_P_ProgramChanStatusBits_isr() and
* BRAP_SPDIFFM_P_ProgramChanStatusBits() for 7440
* 
* Hydra_Software_Devel/16   4/28/06 3:24p rjain
* PR 20318: Adding Open count and start count to hSpdifFm so that if
* multiple channels use the same hSpdfiFm, it is handled correctly.
* 
* Hydra_Software_Devel/15   3/30/06 2:27p kagrawal
* PR 20318: Implemented reveiw comments on Audio Manager
* 
* Hydra_Software_Devel/14   3/1/06 6:22p rjain
* PR 19946 :
* - Fixed some issues related to handling of Mai
* - Fixed some issues related to sampling rate changes for Mai, for
* cloned ports and for decode channels
* 
* Hydra_Software_Devel/13   12/13/05 1:56p kagrawal
* PR 18578: Added support for dither and burst mode programming for
* SPDIFFM
* 
* Hydra_Software_Devel/12   12/6/05 6:38p kagrawal
* PR 18312: Added following ISR version of routine:
* 1. BRAP_SPDIFFM_P_ProgramChanStatusBits_isr
* 
* Hydra_Software_Devel/11   11/30/05 4:20p kagrawal
* PR 18279: Rearchitected output sampling rate change for PCM channel
* 
* Hydra_Software_Devel/10   11/10/05 11:54a kagrawal
* PR 17590: Merging PCM Playback and Mixing changes to
* Hydra_Software_Devel
* 
* Hydra_Software_Devel/Kapil_PCM_Playback/1   11/7/05 7:31p kagrawal
* PR 17590: Enabled PCM playback on SPDIF output port
* 
* Hydra_Software_Devel/9   10/14/05 3:42p nitinb
* PR 17420: Added prototype of private function to get current parameters
* of SPDIF formatter (Required for Flush API)
* 
* Hydra_Software_Devel/7   6/2/05 10:49a rjain
* PR 14143: Implemented FMM Code review comments.
* 
* Hydra_Software_Devel/6   5/13/05 2:41p rjain
* PR 15160: Need ability to set various fields in the SPDIF channel
* status
* 
* Hydra_Software_Devel/5   5/4/05 11:09a rjain
* PR 14507:- Added proper States, open/start counts for modules.
* - Added appropriate checks to handle multiple calls to _Open(),
* _Start(), _Stop(), _Close() for modules.
* - Corrected SPDIFFM handle stored in hFmm / channel handle to be
* SPDIFFM Stream handle.
* - Some clean up, mroe comments/debug messages.
* - Added BRAP_RM_P_GetSpdifFmId() to RM
* 
* Hydra_Software_Devel/4   2/2/05 7:13p nitinb
* PR 13082: Fixing compilation warnings
* 
* Hydra_Software_Devel/3   1/25/05 9:54a rjain
* PR13082: Modifying according to settings/bugs highlighted in emulation
* so far.
* 
* Hydra_Software_Devel/2   1/12/05 5:03p rjain
* PR13082: Updating with changes after compilation of FMM, AM and RM
* together.
* 
* Hydra_Software_Devel/1   1/11/05 11:40a rjain
* PR 13082: Moved to new directory structure
* 
***************************************************************************/
#ifndef _BRAP_SPDIFFM_PRIV_H_
#define _BRAP_SPDIFFM_PRIV_H_

#include "brap_priv.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BRAP_SPDIFFM_CBIT_BUFFER_SIZE 12

/* Defines for CP TOGGLE RATE */
#define BRAP_SPDIFFM_CP_TOGGLE_RATE_DISABLED 0
#define BRAP_SPDIFFM_MIN_CP_TOGGLE_RATE_HZ 4
#define BRAP_SPDIFFM_MAX_CP_TOGGLE_RATE_HZ 10




/***************************************************************************
Summary:
    This enumeration defines repeatition period of burst insertion for 
    SPDIF Formatter.

***************************************************************************/
typedef enum BRAP_SPDIFFM_P_BurstRepPeriod
{
    BRAP_SPDIFFM_P_BurstRepPeriod_eNone,    /* No burst is inserted */
    BRAP_SPDIFFM_P_BurstRepPeriod_ePer3,    /* Repetion period = 3 
                                               Used for AC3 and DTS */
    BRAP_SPDIFFM_P_BurstRepPeriod_ePer4,    /* Repetion period = 4 
                                               Used for AC3 Plus */                                               
    BRAP_SPDIFFM_P_BurstRepPeriod_ePer64,   /* Repetion period = 64 
                                               Used for MPEG2, low sampling 
                                               freq. E.g. */
    BRAP_SPDIFFM_P_BurstRepPeriod_ePer32,   /* Repetion period = 32 
                                               Used for all other MPEG1/2 
                                               formats. E.g. */
    BRAP_SPDIFFM_P_BurstRepPeriod_ePer1024, /* Repetion period = 1024 */
    BRAP_SPDIFFM_P_BurstRepPeriod_ePer4096  /* Repetion period = 4096 
                                               Recommended for use in 
                                               null burst */
}BRAP_SPDIFFM_P_BurstRepPeriod;
 

/***************************************************************************
Summary:
    Parameters to be passed by Audio Manager on Opening a SPDIF Formatter.
    Note that this includes both public and private settings.
    No settings at present

Description:


See Also:

***************************************************************************/
typedef struct BRAP_SPDIFFM_P_Settings
{
    BRAP_SPDIFFM_Settings sExtSettings;     /* Settings provided by the 
                                               application*/
}BRAP_SPDIFFM_P_Settings;



/***************************************************************************
Summary:
    Parameters to be passed by Audio Manager on Starting the SPDIF Formatter.
    Note that this includes both public and private parameters.

Description:
    bSeparateLRChanNum = 0 => Dont override the Channel Status buffer setttings.
                              Left and Right channel numbers are equal.
    bSeparateLRChanNum = 1 => Override the Channel Status buffer setttings.
                              For Left channel CBIT[20:21] is replaced by 10
                              For Right channel CBIT[20:21] is replaced by 01
                              CBIT[22:23] are unchanged.

See Also:

***************************************************************************/
typedef struct BRAP_SPDIFFM_P_Params
{
    BRAP_SPDIFFM_Params    sExtParams;   /* Parameters provided by application*/
    bool                   bCompressed;  /* TRUE: input data is compressed 
                                             FALSE: input data is not compressed*/
    bool                   bSeparateLRChanNum;   /* TRUE: CBIT[20:21] is overridden
                                                    FALSE: CBIT[20:21] is used */
	bool				bUseSpdifPackedChanStatusBits;
							/* Applies only when output port is SPDIF.
							    This field decides whether to use SPDIF channel 
							    status bits packed in two 32-bit words
							    OR seperated into individual parameters.
							    TRUE = Use field sSpdifPackedChanStatusBits
							    FALSE = Use field sSpdifChanStatusParams 
							    When TRUE, all the channel status bits can be
							    set by application. If channel is a decode channel,
							    then firmware is disabled from programming
							    any of the channel status bits. 
							    This field can be changed only when decode is
							    in "Stop" state. */

	BRAP_OP_SpdifPackedChannelStatusBits		sSpdifPackedChanStatusBits;
							/* Applies only when output port is SPDIF.
							    This field is valid only when
							    bUseSpdifPackedChanStatusBits = TRUE.
							    This field contains 39 bits of SPDIF channel
							    status packed into two 32-bit words.
							    Word 0 contains channel status bits [31:0]
							    and Word 1 contains channel status bits [39:32] */
    unsigned int            uiCpToggleRate;
                                /* Allows toggling of the copy protection bit 
                                   (bit 2, Cp) according to section A.2 of IEC 
                                   60958-3. When enabled, valid value should be
                                   between 4 to 10 (in Hz). 0 indicates it is 
                                   disabled. */
	BRAP_OP_SpdifChanStatusParams sChanStatusParams;
										 /* SPDIF channel status params used to
										    program CBIT buffer for a PCM channel*/
    BRAP_SPDIFFM_P_BurstRepPeriod eBurstRepPeriod;
                                         /* Burst repetition period used only when
                                            bCompressed is TRUE */
    BAVC_AudioSamplingRate        eSamplingRate;
                                         /* Output sampling rate */
    bool                bUseHwCBit;         /* TRUE: Use HW C-Bit programer
                                                            FALSE: Use SPDIF FM C-Bit Programer */
}BRAP_SPDIFFM_P_Params;


/***************************************************************************
Summary:
    Abstraction of a SPDIF Formatter Stream
    
Description:
    It contains all the information required to handle the SPDIF Formatter Stream
    Particularly, it stores the indexes, handles for all required chip 
    related information, parent FMM handle etc

See Also:
    
***************************************************************************/
typedef struct BRAP_SPDIFFM_P_Object
{
    BRAP_SPDIFFM_P_Settings sSettings;  /* Setting provided during _Open(). */
    BRAP_SPDIFFM_P_Params   sParams;    /* Settings provided during _Start().*/

    BCHP_Handle             hChip;      /* Handle to chip object */
    BREG_Handle             hRegister;  /* Handle to register object */
    BMEM_Handle             hHeap;      /* Handle to memory object */
    BINT_Handle             hInt;       /* Handle to interrupt object */


    BRAP_FMM_P_Handle       hFmm;           /* Parent FMM handle */
    unsigned int            uiIndex;        /* SPFIDFM index */
    unsigned int            uiStreamIndex;  /* Stream 0 or Stream 1 */
    uint32_t                ui32Offset;       /* Offset of a register for Input 1
                                               from corresponding register for
                                               Input 0 */
    unsigned int           uiOpenCnt;   /* No. of open audio channels currently
                                           routed to this SPDIFFM stream */
    unsigned int           uiStartCnt;  /* No. of active audio channels currently
                                           routed to this SPDIFFM stream */                                               
    
}BRAP_SPDIFFM_P_Object;


/***************************************************************************
Summary:
    Opens a SPDIF Formatter instance

Description:

Returns:
    BERR_SUCCESS on success
    Error code on failure

See Also:
    BRAP_SPDIFFM_P_Close, BRAP_SPDIFFM_P_GetDefaultSettings.
**************************************************************************/
BERR_Code 
BRAP_SPDIFFM_P_Open (
    BRAP_FMM_P_Handle         hFmm,             /* [in] Parent FMM handle */
    BRAP_SPDIFFM_P_Handle *   phSpdifFm,        /* [out] SPDIFFM stream handle */ 
    unsigned int              uiStreamIndex,    /* [in] SPDIFFM Stream index */
    const BRAP_SPDIFFM_P_Settings * pSettings   /* [in] The SPDIFFM stream settings*/                                         
);

/***************************************************************************
Summary:
    Releases all the resources associated with this SPDIF Formatter and frees 
    the handles.

Description:

Returns:
    BERR_SUCCESS on success
    Error code on failure

See Also:
    BRAP_SRCCH_P_Open.
**************************************************************************/
BERR_Code 
BRAP_SPDIFFM_P_Close ( 
    BRAP_SPDIFFM_P_Handle  hSpdifFm /* [in] SPDIF Formatter handle */
);


/***************************************************************************
Summary:
    Starts SPDIF Formatter

Description:

Returns:
    BERR_SUCCESS on success
    Error code on failure

See Also:
    BRAP_SPDIFFM_P_Stop 
    
**************************************************************************/
BERR_Code 
BRAP_SPDIFFM_P_Start ( 
	BRAP_ChannelHandle 		hRapCh,		  /* [in] Rap channel handle */	
    BRAP_SPDIFFM_P_Handle   hSpdifFm,     /* [in] SPDIF Formatter handle */
    const BRAP_SPDIFFM_P_Params *pParams /* [in] Pointer to start
                                                  parameters */ 
);


/***************************************************************************
Summary:
    Stops SPDIF Formatter

Description:
   
Returns:
    BERR_SUCCESS on success
    Error code on failure

See Also:
    BRAP_SPDIFFM_P_Start 
    
**************************************************************************/
BERR_Code 
BRAP_SPDIFFM_P_Stop (
    BRAP_SPDIFFM_P_Handle  hSpdifFm /* [in] SPDIF Formatter handle */
);



/***************************************************************************
Summary:
    Returns default values for SPDIF Formatter Start time parameters.

Description:
    For parameters that the system cannot assign default values to, 
    an invalid value is returned 

Returns:
    BERR_SUCCESS on success
    Error code on failure

See Also:
**************************************************************************/
BERR_Code 
BRAP_SPDIFFM_P_GetDefaultParams ( 
    BRAP_SPDIFFM_P_Params    *pDefParams   /* Pointer to memory where default
                                              parameters should be written */    
);

/***************************************************************************
Summary:
    Returns current values for SPDIF Formatter Start time parameters.

Description:
	Returns current parameter values stored in handle.

Returns:
    BERR_SUCCESS on success
    Error code on failure

See Also:
**************************************************************************/
BERR_Code 
BRAP_SPDIFFM_P_GetCurrentParams (
	BRAP_SPDIFFM_P_Handle  hSpdifFm, /* [in] SPDIF Formatter handle */
    BRAP_SPDIFFM_P_Params    *pCurParams   /* Pointer to memory where current 
                                              parameters should be written */    
);

/***************************************************************************
Summary:
    Returns default values for SPDIF Formatter Open time parameters.

Description:
    For parameters that the system cannot assign default values to, 
    an invalid value is returned 

Returns:
    BERR_SUCCESS on success
    Error code on failure

See Also:
**************************************************************************/
BERR_Code 
BRAP_SPDIFFM_P_GetDefaultSettings ( 
    BRAP_SPDIFFM_P_Settings *pDefSettings   /* Pointer to memory where default
                                              settings should be written */    
);

/***************************************************************************
Summary:
    Prepares and programs channel status bits. 
Description:

Returns:
    BERR_SUCCESS on success
    Error code on failure
See Also:
	BRAP_SPDIFFM_P_PrepareCBITData, BRAP_SPDIFFM_P_ProgramCBITData
**************************************************************************/

BERR_Code BRAP_SPDIFFM_P_ProgramChanStatusBits(
    BRAP_SPDIFFM_P_Handle   hSpdifFm,       /* [in] SPDIF Formatter handle */
    BAVC_AudioSamplingRate  eSamplingRate   /* [in] SPDIF Output SR */
    ) ; 
/***************************************************************************
Summary:
    Prepares and programs channel status bits. 
Description:
    ISR version of BRAP_SPDIFFM_P_ProgramChanStatusBits()
Returns:
    BERR_SUCCESS on success
    Error code on failure
See Also:
	BRAP_SPDIFFM_P_PrepareCBITData, BRAP_SPDIFFM_P_ProgramCBITData
**************************************************************************/

BERR_Code BRAP_SPDIFFM_P_ProgramChanStatusBits_isr(
    BRAP_SPDIFFM_P_Handle   hSpdifFm,       /* [in] SPDIF Formatter handle */
    BAVC_AudioSamplingRate  eSamplingRate   /* [in] SPDIF Output SR */
    ) ; 
/***************************************************************************
Summary:
    Changes Burst repetition period for MPEG-2 as per IEC-61937-1. 
Description:
    If sampling rate of an MPEG-2 stream is < 32kHz then burst repetition 
    rate is set to PER_64 else it is set to PER_32. Applied only if the 
    data out is compressed from SPDIFFM. 
Returns:
    BERR_SUCCESS on success
See Also:

**************************************************************************/
BERR_Code BRAP_SPDIFFM_P_ChangeBurstRepPeriodForMPEG2_isr(
    BRAP_SPDIFFM_P_Handle   hSpdifFm    /* [in] SPDIF Formatter handle */
);



/***************************************************************************
Summary:
    If null/pause burst is enabled, for compressed data, this PI sets the 
    OVERWRITE_DATA flag which determines 
    whether or not the data from the mixer will be overwritten by a Burst.
    Must be called only after the output port has been configured and opened.
    
Description:

REP_PERIOD   MUTE
    0         0     pause/null/zero never inserted
    0         1     zero overwrites samples from mixer
  non-0       0     pause/null on underflow
  non-0       1     pause/null overwrites sample from mixer


Returns:
    BERR_SUCCESS on success
    Error code on failure
    
See Also:
	

**************************************************************************/
BERR_Code
BRAP_SPDIFFM_P_InsertBurstOnMute (
    BRAP_SPDIFFM_P_Handle   hSpdifFm,   /* [in] SPDIF Formatter handle */
    bool                    bOverwrite  /* TRUE: overwrite data from mixer
                                           FALSE: Don't overwrite data from 
                                           mixer */
) ;

BERR_Code 
BRAP_SPDIFFM_P_ProgramCBITBuffer_isr( 
    BRAP_SPDIFFM_P_Handle   hSpdifFm,   /* [in] SPDIF Formatter handle */
    uint32_t			*pui32CBITBuffer /* [in] SPDIF bit buffer */
);


/***************************************************************************
Summary:
    Programs dither and burst for SPDIFFM.
Description:
    This internal routine does the register programming for dither and burst
    for SPDIFFM.
Returns:
    BERR_SUCCESS on success
    Error code on failure
See Also:
	
**************************************************************************/
BERR_Code
BRAP_SPDIFFM_P_ProgramDitherAndBurst(
    BRAP_SPDIFFM_P_Handle   hSpdifFm,   /* [in] SPDIF Formatter handle */
    BRAP_SPDIFFM_P_Settings *pSpdiffmSettings,/* [in] SPDIFFM settings */
    BRAP_SPDIFFM_P_BurstRepPeriod eBurstRepPeriod 
                                              /* [in] Burst repetition period */
);


#ifdef __cplusplus
}
#endif


#endif /* _BRAP_SPDIFFM_PRIV_H_ */

/* End of File */
