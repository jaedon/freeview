/***************************************************************************
*     Copyright (c) 2004-2009, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: brap_spdiffm.c $
* $brcm_Revision: Hydra_Software_Devel/73 $
* $brcm_Date: 11/21/09 7:00a $
*
* Module Description:
*   Module name: SPDIFFM
*   This file contains the implementation of all PIs for the SPDIF
*   Formatter abstraction. 
*
* Revision History:
*
* $brcm_Log: /magnum/portinginterface/rap/base/brap_spdiffm.c $
* 
* Hydra_Software_Devel/73   11/21/09 7:00a srajapur
* SW7400-2534 : [7400] DDP test streams fail to play after initial
* playback. HDMI audio has severe static.  --> Changed the programming
* of REP_PERIOD as per the standard for DDP.
* 
* Hydra_Software_Devel/72   2/27/09 6:53p srajapur
* PR50653 : [7401]  MP3 playback stops when triggering watchdog-
* interrupt.
* -->Removing the SCALE_RAMP_STEP_SIZE  for PCM playback channel
* 
* Hydra_Software_Devel/71   12/23/08 7:05p srajapur
* PR50653 : [7400]  MP3 playback stops when triggering watchdog-
* interrupt.
* -->Programmed the SCALE_RAMP_STEP_SIZE  for PCM playback channel
* 
* Hydra_Software_Devel/70   12/9/08 11:59a srajapur
* PR43718 : [7400] No audio on SPDIF passthrough for ac3 streams when
* rap_ac3_support=n
* --> Enabled the Fw stream enable in MS when passthroug mode is set
* 
* Hydra_Software_Devel/69   11/24/08 10:14p srajapur
* PR45478 : [7400] Brief HDMI signal loss on Samsung
* PR46653 : [7400] Noise heard in changing between PCM to compressed
* through Dynex TV HDMI
* 
* Hydra_Software_Devel/68   8/13/08 2:34p bhanus
* PR43971: [7440,7601] Merging Base2 Code Clean Up and DVD family for
* 7601 and onward chips
* 
* Hydra_Software_Devel/RAP_DVD_Family/1   6/23/08 8:09p gdata
* PR43971: [7443] Creating DVD family for 7443 and onward chips
* 
* Hydra_Software_Devel/67   6/9/08 3:34p srajapur
* PR 39172 : [7400] HDMI has static noice right after channel channel on
* Sharp LC-32GD4UM
* 
* Hydra_Software_Devel/66   6/6/08 11:50a srajapur
* PR 39172 : [7400] HDMI has static noice right after channel channel on
* Sharp LC-32GD4UM
* 
* Hydra_Software_Devel/65   5/27/08 7:02p srajapur
* PR42245 : [7400] Added 7.1 Channel Pass-Thru Support for DDP on HDMI
* output - Removed the usuage of HW C-bits
* 
* Hydra_Software_Devel/64   5/19/08 5:25p srajapur
* PR42245 : [7400] Added 7.1 Channel Pass-Thru Support for DDP on HDMI
* output
* 
* Hydra_Software_Devel/63   4/30/08 1:02p gdata
* PR41468: [7440]Merging RAP_7440_Production branch to main branch.
* 
* Hydra_Software_Devel/62   4/3/08 1:33p srajapur
* PR40452 : [7403,7401,7118,7400]  Noise on S/Pdif output when doing
* program switching.
* ---->Setting the INSERT_ON_UFLOW to 0, by setting this when an input
* sample is unavailable we will wait until input is ready.
* 
* Hydra_Software_Devel/61   2/26/08 3:54p srajapur
* PR34858: [7400,7118,7403,7401] Added API support to set the 41 bits of
* channel status in SPDIF
* 
* Hydra_Software_Devel/60   2/14/08 6:55p srajapur
* PR34858: [7400,7118,7403,7401] Added API support to set the 41 bits of
* channel status in SPDIF
* 
* Hydra_Software_Devel/59   1/14/08 11:54a sushmit
* PR 36003: [7405] Fixing compilation warnings.
* 
* Hydra_Software_Devel/Rap_7440_Production/1   4/30/08 12:11p gdata
* PR42341:  [7440] Correcing the CSTAT programming for I2S transmission
* to internal HDMI
* 
* Hydra_Software_Devel/58   1/10/08 1:51p bhanus
* PR 38642 : [7440] Fixing compilation break.
* 
* Hydra_Software_Devel/57   1/10/08 12:24p bhanus
* PR 38642 : [7440] Added code for not overwriting the CBITs in case
* bUseSpdifPackedChanStatusBits is set to true.
* 
* Hydra_Software_Devel/56   12/11/07 12:29p bhanus
* PR 36468 : [7440] Reverted the code change  to disable the SPDIF clock
* and re-enbale on timer basis while transitioning from PCM - Comp or
* vice versa.
* 
* Hydra_Software_Devel/55   11/26/07 3:13p bselva
* PR 37395:[7401,7118,7401,7400] Fix for the PR 37395 added to the latest
* code
* 
* Hydra_Software_Devel/54   11/16/07 11:28a srajapur
* PR 37070: [7401,7403,7118,7400] Removed extra parameter added from the
* spdif frame.
* 
* Hydra_Software_Devel/53   10/26/07 7:20p bhanus
* PR 36477 : [7440] Fixing the Bus error introduced due to wrong offset
* in the last checkin
* 
* Hydra_Software_Devel/52   10/26/07 1:46p bhanus
* PR 36468 : [7440] Added code to disable the SPDIF clock and re-enbale
* on timer basis while transitioning from PCM - Comp or vice versa.
* 
* Hydra_Software_Devel/51   10/11/07 4:10p sushmit
* PR 36003: [7405] Fixing Memory Leaks & Extra Initializers
* 
* Hydra_Software_Devel/50   10/10/07 7:32p srajapur
* PR 34858 : [7401,7403,7118,7400] Added API support to set the bit-32 of
* channel status in SPDIF.
* 
* Hydra_Software_Devel/49   10/5/07 7:39p gdata
* PR35413 : [7440] Merging the DTS-CD branch changes on main branch
* 
* Hydra_Software_Devel/48   9/25/07 11:55a srajapur
* PR 35022 : [7401,7403,7118,7400] Modified the code with the fix and
* checked in the file into Clearcase.Rap LATEST.
* 
* Hydra_Software_Devel/47   9/21/07 9:56a speter
* PR 34571:[3563,7440,7405] Fixing compilation errors.
* 
* Hydra_Software_Devel/46   9/20/07 1:35p bselva
* PR 34571:[7401,7118,7400] PI needs to initialize the CBITs in SPDIF
* formatter start for the values that it knows before starting the
* decoder.
* 
* Hydra_Software_Devel/45   8/28/07 12:33p kagrawal
* PR34399: [7440][HiDefDvd 5311] Removed a redundant error check that was
* restricting the number of SPDIFFM open - as a result HDMI was not
* getting added to the channel association for multi-channel
* 
* Hydra_Software_Devel/44   8/19/07 8:22p kagrawal
* PR 34154: [7440] Added CP_TOGGLE_RATE programming support
* 
* Hydra_Software_Devel/43   6/12/07 2:40p bhanus
* PR 30069 : [7440 B0] Fixed a issue in programming the SpdifFm CBITs for
* stream1(HiDef-DVD PR 4254).
* 
* Hydra_Software_Devel/42   4/24/07 5:29p nitinb
* PR 28177: [3563, 7440] Second argument of
* BRAP_SPDIFFM_P_ProgramCBITBuffer_isr() expects C-bit values in lower
* 16 bits of array of uint32_t elements
* 
* Hydra_Software_Devel/41   3/30/07 4:09p nitinb
* PR 28177: Corrected programming field OFFSET of register
* AUD_FMM_MS_CTRL_FW_CBIT_CTRL_0.
* 
* Hydra_Software_Devel/40   3/28/07 6:14p nitinb
* PR 28177: [3563, 7440] Fixed build issues for 7401, 7400 chips
* 
* Hydra_Software_Devel/39   3/28/07 5:15p nitinb
* PR 28177: [3563, 7440] Made function
* BRAP_SPDIFFM_P_ProgramCBITBuffer_isr non static. Updated function
* BRAP_SPDIFFM_P_HWConfig to conditionally set channel status parameters
* provided in packed format.
* 
* Hydra_Software_Devel/38   3/28/07 3:20p rjain
* PR 29074: clear the CP_TOGGLE_RATE field while programming the CBIT
* buffer for PCM channels
* 
* Hydra_Software_Devel/37   3/8/07 10:51a rjain
* PR 28524:[7401, 7118, 7403] code clean up: replacing (BCHP_CHIP ==
* 7401) || (BCHP_CHIP == 7118) || (BCHP_CHIP == 7403) with
* (BRAP_7401_FAMILY == 1) where appropriate
* 
* Hydra_Software_Devel/36   2/27/07 10:51a bselva
* PR 26219: Added the support for inserting burst on muting
* 
* Hydra_Software_Devel/35   2/16/07 3:48p bselva
* PR 26219: Added the support for 7400 B0 compilation
* 
* Hydra_Software_Devel/34   2/6/07 5:57p nitinb
* PR 26509: Excluding this change for 3563 until RDB is available
* 
* Hydra_Software_Devel/33   2/6/07 4:57p rjain
* PR 26509: [All chips] Changes to insert a dither signal or null/pause
* burst when muting SPDIF/HDMI
* 
* Hydra_Software_Devel/32   11/29/06 11:26a bhanus
* PR 25607 : Modified for SRC bringup for 7440
* - Modified interface BRAP_SPDIFFM_P_ProgramChanStatusBits_isr() and
* BRAP_SPDIFFM_P_ProgramChanStatusBits() for 7440
* 
* Hydra_Software_Devel/31   11/3/06 2:39p gautamk
* PR24342: resolving compiler warning for DEBUG=n build
* 
* Hydra_Software_Devel/30   9/25/06 1:21p kagrawal
* PR 20654: Merged 7440 bring-up code
* 
* Hydra_Software_Devel/29   8/11/06 4:02p bselva
* PR 23192: Fixed the compilation warning
* 
* Hydra_Software_Devel/28   6/6/06 12:20p rjain
* PR 21950: fixing compilation warnings
* 
* Hydra_Software_Devel/27   5/29/06 4:58p rjain
* PR 21780: RAP_SPDIFFM_P_BurstRepPeriod_eNone wasnt handled in
* BRAP_SPDIFFM_P_ProgramDitherAndBurst(). Added it.
* 
* Hydra_Software_Devel/26   5/18/06 6:17p rjain
* PR 21607: replacing BDBG_ASSERT(0) with return BERR_TRACE()
* 
* Hydra_Software_Devel/25   4/28/06 3:24p rjain
* PR 20318: Adding Open count and start count to hSpdifFm so that if
* multiple channels use the same hSpdfiFm, it is handled correctly.
* 
* Hydra_Software_Devel/24   4/24/06 11:50a kagrawal
* PR 21159: Replaced uiGetSamplingRate[] by BRAP_P_ConvertSR() to support
* all the BAVC_AudioSamplingRate enums
* 
* Hydra_Software_Devel/23   3/30/06 2:27p kagrawal
* PR 20318: Implemented reveiw comments on Audio Manager
* 
* Hydra_Software_Devel/22   3/1/06 6:22p rjain
* PR 19946 :
* - Fixed some issues related to handling of Mai
* - Fixed some issues related to sampling rate changes for Mai, for
* cloned ports and for decode channels
* 
* Hydra_Software_Devel/21   2/1/06 3:14p rjain
* PR 19275: passing dither etc correctly in addOutputPort. Also fixed
* some misc things.
* 
* Hydra_Software_Devel/20   12/13/05 5:39p kagrawal
* PR 18578: Corrected a if check for 7401
* 
* Hydra_Software_Devel/19   12/13/05 1:54p kagrawal
* PR 18578: Added support for dither and burst mode programming for
* SPDIFFM
* 
* Hydra_Software_Devel/18   12/6/05 6:38p kagrawal
* PR 18312: Added following ISR version of routine:
* 1. BRAP_SPDIFFM_P_ProgramChanStatusBits_isr
* 
* Hydra_Software_Devel/17   12/1/05 12:16p rjain
* PR 18296: Now we dont disable the MS stream on a channel stop. We have
* it enabled always. So taht it can send out a dither signal while the
* channel is stopped.
* 
* Hydra_Software_Devel/16   11/30/05 4:19p kagrawal
* PR 18279: Rearchitected output sampling rate change for PCM channel
* 
* Hydra_Software_Devel/15   11/15/05 8:02p bselva
* PR 17590: Added support for MAI linked to Flex
* 
* Hydra_Software_Devel/14   11/10/05 11:54a kagrawal
* PR 17590: Merging PCM Playback and Mixing changes to
* Hydra_Software_Devel
* 
* Hydra_Software_Devel/Kapil_PCM_Playback/2   11/9/05 12:47p kagrawal
* PR 17590: Fixed a compilation error
* 
* Hydra_Software_Devel/Kapil_PCM_Playback/1   11/7/05 7:28p kagrawal
* PR 17590: Enable PCM playback on SPDIF output port
* 
* Hydra_Software_Devel/13   10/14/05 3:42p nitinb
* PR 17420: Added private function to get current parameters of SPDIF
* formatter (Required for Flush API)
* 
* Hydra_Software_Devel/12   9/19/05 4:36p rjain
* PR 16899: Making changes to the FMM modules for watchdog recovery.
* 
* Hydra_Software_Devel/10   6/2/05 10:48a rjain
* PR 14143: Implemented FMM Code review comments.
* 
* Hydra_Software_Devel/9   5/13/05 2:41p rjain
* PR 15160: Need ability to set various fields in the SPDIF channel
* status
* 
* Hydra_Software_Devel/8   5/4/05 11:09a rjain
* PR 14507:- Added proper States, open/start counts for modules.
* - Added appropriate checks to handle multiple calls to _Open(),
* _Start(), _Stop(), _Close() for modules.
* - Corrected SPDIFFM handle stored in hFmm / channel handle to be
* SPDIFFM Stream handle.
* - Some clean up, mroe comments/debug messages.
* - Added BRAP_RM_P_GetSpdifFmId() to RM
* 
* Hydra_Software_Devel/7   4/11/05 2:43p rjain
* PR 13082: modifiying debug msgs and module names
* 
* Hydra_Software_Devel/6   4/6/05 6:16p rjain
* PR 13082: Adding modifications for SPDIF
* 
* Hydra_Software_Devel/5   2/16/05 1:46p rjain
* PR 14143: Implementing some comments from Phase 1 Code review done till
* date
* 
* Hydra_Software_Devel/4   2/2/05 5:34p rjain
* PR13082:Converted BREG_Read32 and BRAP_Read32 and BREG_Write32 to
* BRAP_Write32
* 
* Hydra_Software_Devel/3   1/25/05 9:54a rjain
* PR13082: Modifying according to settings/bugs highlighted in emulation
* so far.
* 
* Hydra_Software_Devel/2   1/12/05 5:59p rjain
* PR13082: Converted BRAP_Read32 and BRAP_Write32 to BREG_XXX
* 
* Hydra_Software_Devel/1   1/11/05 11:40a rjain
* PR 13082: Moved to new directory structure
* 
***************************************************************************/

#include "brap.h"
#include "brap_priv.h"

BDBG_MODULE (rap_spdiffm);

#define BRAP_SPDIFFM_PLAYBACK_MS0_CBIT_BUFFER 0
#define BRAP_SPDIFFM_PLAYBACK_MS1_CBIT_BUFFER 12


#if BRAP_SPDIFFM_ASYNC_TX_RE_ENABLE
#define SPDIF_TX_OFF_MICROSECS	100 /* Turn off SPDIF TX for .1msecs after PCM<->Compress switch*/
static void BRAP_P_Spdif_Tx_Enable_isr (
        void * pParm1, /* [in]  BRAP_SPDIFFM_P_Handle*/
        int    iParm2  /* [in]  */
		);

static BERR_Code BRAP_SPDIFFM_P_SetSPDIFtx(
    BRAP_SPDIFFM_P_Handle   hSpdifFm,    /* [in] SPDIF Formatter handle */
	bool					enableTX	 /* [in] bool to turn on or off */
	);

static BERR_Code BRAP_SPDIFFM_P_SetSPDIFtx_isr(
    BRAP_SPDIFFM_P_Handle   hSpdifFm,    /* [in] SPDIF Formatter handle */
	bool					bEnableTx	 /* [in] bool to turn on or off */
	);

#endif
static BERR_Code 
BRAP_SPDIFFM_P_HWConfig ( 
	BRAP_ChannelHandle 		hRapCh,		/* [in] Rap channel handle */	
    BRAP_SPDIFFM_P_Handle   hSpdifFm    /* [in] SPDIF Formatter handle */
);

#if (BRAP_7440_ARCH == 1)
static BERR_Code 
BRAP_SPDIFFM_P_PrepareCBITData_isr(
	uint32_t *pui32CBitData, 			/* [out] CBit buffer */
	BRAP_OP_SpdifChanStatusParams sSpdifChanStatusParams,
										/* SPDIF Channel status params */
	BAVC_AudioSamplingRate eOutputSamplingRate,
										/* Output sampling rate */
    bool                   bCompressed  /* TRUE: input data is compressed */
);
#else
static BERR_Code 
BRAP_SPDIFFM_P_PrepareCBITData_isr(
	uint32_t *pui32CBitData, 			/* [out] CBit buffer */
	BRAP_OP_SpdifChanStatusParams sSpdifChanStatusParams,
										/* SPDIF Channel status params */
	BAVC_AudioSamplingRate eOutputSamplingRate,
										/* Output sampling rate */
	bool                   bCompressed  /* TRUE: input data is compressed */
);
#endif

/* Default Settings and Parameters */
static const BRAP_SPDIFFM_P_Params defSpdifFmParams =
{
    {
        true    /* sExtParams.bSpdifFormat */
    },
    true,       /* bCompressed */
    false,      /* bSeparateLRChanNum */
#if (BRAP_7440_ARCH == 1)
    false,		/* bUseSpdifPackedChanStatusBits */
	{	/* sSpdifPackedChanStatusBits */
		{
			0x0,
			0x0
        	}
	},
    BRAP_SPDIFFM_CP_TOGGLE_RATE_DISABLED, /* uiCpToggleRate */
#endif	
#if (BRAP_7440_ARCH == 1)||(BCHP_CHIP == 7400 )
    false,  /*bHbrEnable*/
#endif    
#if (BRAP_7440_ARCH == 0)
	{			
		/* sChanStatusParams */
	    false,  /* bProfessionalMode = Consumer Mode */
		false, /* bLinear_PCM = true for linear output */
		false,  /* bSwCopyRight = Not asserted */
		0, /*  "0" = 2 audio channels without pre-emphasis */
		0, /* mode 0 */
		0,      /* ui16CategoryCode = 0 */
		0, /* source number */
		0,      /* bSeparateLRChanNum = 0 =>Left=right channel num=0000*/
		0,       /* ui16ClockAccuracy = 0 (Level II) */
		0, /* "0" - Maximum audio sample word length is 20 bits*/
		0, /* "1" - 20 bits */
		0, /* 48k -Original sampling frequency */
		0 /* No copying is permitted */
	},
#else /* if( BRAP_7440_ARCH == 1)*/
	{			
		/* sChanStatusParams */
	    false,  /* bProfessionalMode = Consumer Mode */

    	false,  /* bSwCopyRight = Not asserted */


	    0,      /* ui16CategoryCode = 0 */

    	0,      /* bSeparateLRChanNum = 0 =>Left=right channel num=0000*/
    	0        /* ui16ClockAccuracy = 0 (Level II) */
    },
#endif

	BRAP_SPDIFFM_P_BurstRepPeriod_eNone, /* eBurstRepPeriod */
	BAVC_AudioSamplingRate_e48k
};

static const BRAP_SPDIFFM_P_Settings defSpdifFmSettings =
{
    {
        true,                           /* sExtSettings.bEnableDither */
        BRAP_SPDIFFM_BurstType_ePause   /* sExtSettings.eBurstType */
    }
};

BERR_Code 
BRAP_SPDIFFM_P_GetDefaultParams ( 
    BRAP_SPDIFFM_P_Params    *pDefParams   /* Pointer to memory where default
                                              settings should be written */    
)
{
    BERR_Code ret = BERR_SUCCESS;
    BDBG_ENTER (BRAP_SPDIFFM_P_GetDefaultParams);

    BDBG_ASSERT (pDefParams);            
    
    *pDefParams = defSpdifFmParams;
    BDBG_LEAVE (BRAP_SPDIFFM_P_GetDefaultParams);
    return ret;
}

BERR_Code 
BRAP_SPDIFFM_P_GetDefaultSettings ( 
    BRAP_SPDIFFM_P_Settings *pDefSettings   /* Pointer to memory where default
                                              settings should be written */    
)
{
    BERR_Code ret = BERR_SUCCESS;
    BDBG_ENTER (BRAP_SPDIFFM_P_GetDefaultSettings);

    BDBG_ASSERT (pDefSettings);            
    
    *pDefSettings = defSpdifFmSettings;
    BDBG_LEAVE (BRAP_SPDIFFM_P_GetDefaultSettings);
    return ret;
}

BERR_Code 
BRAP_SPDIFFM_P_GetCurrentParams (
	BRAP_SPDIFFM_P_Handle  hSpdifFm, /* [in] SPDIF Formatter handle */
    BRAP_SPDIFFM_P_Params    *pCurParams   /* Pointer to memory where current 
                                              parameters should be written */    
)
{
    BERR_Code ret = BERR_SUCCESS;
    BDBG_ENTER ( BRAP_SPDIFFM_P_GetCurrentParams );
	BDBG_ASSERT( hSpdifFm );
	BDBG_ASSERT( pCurParams );

	*pCurParams = hSpdifFm->sParams;

    BDBG_LEAVE ( BRAP_SPDIFFM_P_GetCurrentParams );
    return ret;
}

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
    BRAP_SPDIFFM_P_Handle   hSpdifFm   /* [in] SPDIF Formatter handle */
    )
{
    uint32_t ui32RegValue = 0;
    BERR_Code ret = BERR_SUCCESS;
    unsigned int uiSR = 0;

    BDBG_ENTER(BRAP_SPDIFFM_P_ChangeBurstRepPeriodForMPEG2_isr);
    BDBG_ASSERT(hSpdifFm);
    
    /* Change Burst repetition period only for a compressed stream */
  if( (hSpdifFm->sParams.bCompressed) && 
	(hSpdifFm->sSettings.sExtSettings.eBurstType != BRAP_SPDIFFM_BurstType_eNone) )
    {
        ui32RegValue = BRAP_Read32_isr (hSpdifFm->hRegister, 
                    BCHP_AUD_FMM_MS_CTRL_FW_BURST_0 + hSpdifFm->ui32Offset);
        
        ui32RegValue &= ~( BCHP_MASK (AUD_FMM_MS_CTRL_FW_BURST_0, REP_PERIOD));    

        /* Convert eSR to uiSR */
        ret = BRAP_P_ConvertSR(hSpdifFm->sParams.eSamplingRate, &uiSR);
        if(ret != BERR_SUCCESS)
        {
            ret = BERR_TRACE(ret);
            return ret;
        }
        
        if(uiSR < 32000)
        {
            BDBG_MSG(("ChangeBurstRepPeriodForMPEG2_isr: burst rep_period = 64"));
            ui32RegValue |= (BCHP_FIELD_ENUM (AUD_FMM_MS_CTRL_FW_BURST_0, 
                                                          REP_PERIOD, PER_64));
        }
        else
        {
            BDBG_MSG(("ChangeBurstRepPeriodForMPEG2_isr: burst rep_period = 32"));
            ui32RegValue |= (BCHP_FIELD_ENUM (AUD_FMM_MS_CTRL_FW_BURST_0,   
                                                          REP_PERIOD, PER_32));
        }

        BRAP_Write32_isr (hSpdifFm->hRegister, 
                      hSpdifFm->ui32Offset + BCHP_AUD_FMM_MS_CTRL_FW_BURST_0,
                      ui32RegValue); 
    }

    BDBG_LEAVE(BRAP_SPDIFFM_P_ChangeBurstRepPeriodForMPEG2_isr);
    return BERR_SUCCESS;
}

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
)
{
    BERR_Code ret = BERR_SUCCESS;
    uint32_t ui32Offset = 0;
    uint32_t ui32RegValue = 0;
    
    BDBG_ENTER(BRAP_SPDIFFM_P_ProgramDitherAndBurst);

    /* Validate input params */
    BDBG_ASSERT(pSpdiffmSettings);
    BDBG_ASSERT(hSpdifFm);

    BDBG_MSG(("BRAP_SPDIFFM_P_ProgramDitherAndBurst: hSpdifFm->sSettings:"));
    BDBG_MSG(("\t bEnableDither=%d", hSpdifFm->sSettings.sExtSettings.bEnableDither));    
    BDBG_MSG(("\t eBurstType=%d", hSpdifFm->sSettings.sExtSettings.eBurstType));        

    ui32Offset = hSpdifFm->ui32Offset;

    /* Program the DITHER_ENA */
    ui32RegValue = BRAP_Read32 (hSpdifFm->hRegister, BCHP_AUD_FMM_MS_CTRL_FW_STREAM_CTRL_0 + ui32Offset);
    ui32RegValue &= ~( BCHP_MASK (AUD_FMM_MS_CTRL_FW_STREAM_CTRL_0, DITHER_ENA) );    
    if(hSpdifFm->sParams.bCompressed == false)
    {
        BDBG_MSG(("BRAP_SPDIFFM_P_ProgramDitherAndBurst: bComp == false bEnaDither = %d", 
                pSpdiffmSettings->sExtSettings.bEnableDither));
        /* For uncompressed, do as per the user settings */    
        if(pSpdiffmSettings->sExtSettings.bEnableDither)
            ui32RegValue |= (BCHP_FIELD_ENUM (AUD_FMM_MS_CTRL_FW_STREAM_CTRL_0, 
                                      DITHER_ENA, Enable));
        else
            ui32RegValue |= (BCHP_FIELD_ENUM (AUD_FMM_MS_CTRL_FW_STREAM_CTRL_0, 
                                      DITHER_ENA, Disable));
    }
    else
    {
        BDBG_MSG(("BRAP_SPDIFFM_P_ProgramDitherAndBurst: bComp = true disabling dither"));
        /* For compressed, always disable dither */
        ui32RegValue |= (BCHP_FIELD_ENUM (AUD_FMM_MS_CTRL_FW_STREAM_CTRL_0, 
                                      DITHER_ENA, Disable));
    }
    
    BRAP_Write32 (hSpdifFm->hRegister, 
                  ui32Offset + BCHP_AUD_FMM_MS_CTRL_FW_STREAM_CTRL_0,
                  ui32RegValue); 
        
    /* Program the BURST details */
    ui32RegValue = 0;
    ui32RegValue &= ~( BCHP_MASK (AUD_FMM_MS_CTRL_FW_BURST_0, TYPE) |
                       BCHP_MASK (AUD_FMM_MS_CTRL_FW_BURST_0, REP_PERIOD) |
                       BCHP_MASK (AUD_FMM_MS_CTRL_FW_BURST_0, STOP));    

    if(hSpdifFm->sParams.bCompressed == false)
    {
        BDBG_MSG(("BRAP_SPDIFFM_P_ProgramDitherAndBurst: bComp = false, disabling burst "));
        /* For uncompressed, always disable the burst */
        ui32RegValue |= (BCHP_FIELD_ENUM (AUD_FMM_MS_CTRL_FW_BURST_0, 
                                          REP_PERIOD, None));
        ui32RegValue |= (BCHP_FIELD_ENUM (AUD_FMM_MS_CTRL_FW_BURST_0, 
                                          STOP, Disable));
    }
    else
    {
        /* For compressed, do as per the user settings */
        switch(pSpdiffmSettings->sExtSettings.eBurstType)
        {
            case BRAP_SPDIFFM_BurstType_eNone:
                BDBG_MSG(("BRAP_SPDIFFM_P_ProgramDitherAndBurst: burst None"));
                /* Insert nither pause nor null burst */
                ui32RegValue |= (BCHP_FIELD_ENUM (AUD_FMM_MS_CTRL_FW_BURST_0, 
                                          REP_PERIOD, None));
                ui32RegValue |= (BCHP_FIELD_ENUM (AUD_FMM_MS_CTRL_FW_BURST_0, 
                                          STOP, Disable));
                break;
                
            case BRAP_SPDIFFM_BurstType_eNull:
                BDBG_MSG(("BRAP_SPDIFFM_P_ProgramDitherAndBurst: burst Null"));
                /* Type = Null burst */
                ui32RegValue |= (BCHP_FIELD_ENUM (AUD_FMM_MS_CTRL_FW_BURST_0, 
                                          TYPE, Null));
                ui32RegValue |= (BCHP_FIELD_ENUM (AUD_FMM_MS_CTRL_FW_BURST_0, 
                                          STOP, Enable));
                break;
                
            case BRAP_SPDIFFM_BurstType_ePause:
                BDBG_MSG(("BRAP_SPDIFFM_P_ProgramDitherAndBurst: burst Pause"));
                /* Type = Pause burst */
                ui32RegValue |= (BCHP_FIELD_ENUM (AUD_FMM_MS_CTRL_FW_BURST_0, 
                                          TYPE, Pause));
                ui32RegValue |= (BCHP_FIELD_ENUM (AUD_FMM_MS_CTRL_FW_BURST_0, 
                                          STOP, Enable));
                break;

            default:
                BDBG_ERR(("BRAP_SPDIFFM_P_ProgramDitherAndBurst: Illegal eBurstType(%d)",
                    pSpdiffmSettings->sExtSettings.eBurstType));
                return BERR_TRACE(BERR_INVALID_PARAMETER);
        }/* switch eBurstType */

        /* Only for Null or Pause burst type, program the repetition period */
        if(pSpdiffmSettings->sExtSettings.eBurstType == BRAP_SPDIFFM_BurstType_eNull ||
           pSpdiffmSettings->sExtSettings.eBurstType == BRAP_SPDIFFM_BurstType_ePause )
        {
            switch (eBurstRepPeriod)
            {
                case BRAP_SPDIFFM_P_BurstRepPeriod_eNone:
                    BDBG_MSG(("BRAP_SPDIFFM_P_ProgramDitherAndBurst: burst rep period : none"));
                    ui32RegValue |= (BCHP_FIELD_ENUM (AUD_FMM_MS_CTRL_FW_BURST_0, 
                                          REP_PERIOD, None));
                    break;            
                case BRAP_SPDIFFM_P_BurstRepPeriod_ePer3:
                    BDBG_MSG(("BRAP_SPDIFFM_P_ProgramDitherAndBurst: burst rep period 3"));
                    ui32RegValue |= (BCHP_FIELD_ENUM (AUD_FMM_MS_CTRL_FW_BURST_0, 
                                          REP_PERIOD, PER_3));
                    break;
                    
                case BRAP_SPDIFFM_P_BurstRepPeriod_ePer4:
                    BDBG_MSG(("BRAP_SPDIFFM_P_ProgramDitherAndBurst: burst rep period 4"));
                    ui32RegValue |= (BCHP_FIELD_DATA (AUD_FMM_MS_CTRL_FW_BURST_0, 
                                          REP_PERIOD, 2));
                    break;
                                                
                case BRAP_SPDIFFM_P_BurstRepPeriod_ePer32:
                    BDBG_MSG(("BRAP_SPDIFFM_P_ProgramDitherAndBurst: burst rep period 32"));
                    ui32RegValue |= (BCHP_FIELD_ENUM (AUD_FMM_MS_CTRL_FW_BURST_0, 
                                          REP_PERIOD, PER_32));
                    break;

                case BRAP_SPDIFFM_P_BurstRepPeriod_ePer64:
                    BDBG_MSG(("BRAP_SPDIFFM_P_ProgramDitherAndBurst: burst rep period 64"));
                    ui32RegValue |= (BCHP_FIELD_ENUM (AUD_FMM_MS_CTRL_FW_BURST_0, 
                                          REP_PERIOD, PER_64));
                    break;

                case BRAP_SPDIFFM_P_BurstRepPeriod_ePer1024:
                    BDBG_MSG(("BRAP_SPDIFFM_P_ProgramDitherAndBurst: burst rep period 1024"));
                    ui32RegValue |= (BCHP_FIELD_ENUM (AUD_FMM_MS_CTRL_FW_BURST_0, 
                                          REP_PERIOD, PER_1024));
                    break;

                case BRAP_SPDIFFM_P_BurstRepPeriod_ePer4096:
                    BDBG_MSG(("BRAP_SPDIFFM_P_ProgramDitherAndBurst: burst rep period 4096"));
                    ui32RegValue |= (BCHP_FIELD_ENUM (AUD_FMM_MS_CTRL_FW_BURST_0, 
                                          REP_PERIOD, PER_4096));
                    break;

                default:
                    BDBG_ERR(("BRAP_SPDIFFM_P_ProgramDitherAndBurst: Invalid eBurstRepPeriod(%d)",
                        eBurstRepPeriod));
                return BERR_TRACE(BERR_INVALID_PARAMETER);
            }/* switch eBurstRepPeriod */
        }
    }
        
    BRAP_Write32 (hSpdifFm->hRegister, 
                  ui32Offset + BCHP_AUD_FMM_MS_CTRL_FW_BURST_0,
                  ui32RegValue); 

    BDBG_LEAVE(BRAP_SPDIFFM_P_ProgramDitherAndBurst);
    return ret;
}


/***************************************************************************
Summary:
    This PI sets/clears the OVERWRITE_DATA flag which determines 
    whether or not the data from the mixer will be overwritten.
    When this bit is set, by a Dither/Null 
    Burst/P
    Must be called only after the output port has been configured and opened.
    
Description:

For compressed data:
REP_PERIOD   OVERWRITE_DATA
    0         0     Pause/null/zero never inserted. Samples sent out as is.
    0         1     Overwrite sample from mixer with zeros
  non-0       0     Insert pause/null on underflow.
  non-0       1     Overwrite samples from mixer with null/pause burst.

For PCM data:
DITHER_ENA  OVERWRITE_DATA
    0         0     Nothing inserted.
    0         1     Overwrite sample from mixer with zeros
    1         0     Insert dither on underflow.
    1         1     Overwrite samples from mixer with dither signal

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
)               
{
    BERR_Code ret = BERR_SUCCESS;
    uint32_t ui32RegVal = 0;
    
    BDBG_ENTER(BRAP_SPDIFFM_P_InsertBurstOnMute);

    BDBG_ASSERT(hSpdifFm);
    
    BDBG_MSG(("BRAP_SPDIFFM_P_InsertBurstOnMute: eBurstType=%d", hSpdifFm->sSettings.sExtSettings.eBurstType));  

    ui32RegVal = BRAP_Read32 (hSpdifFm->hRegister, BCHP_AUD_FMM_MS_CTRL_FW_STREAM_CTRL_0 + hSpdifFm->ui32Offset);
    
    ui32RegVal &= ~(BCHP_MASK (AUD_FMM_MS_CTRL_FW_STREAM_CTRL_0, OVERWRITE_DATA));

    if (bOverwrite == true)
    {
        ui32RegVal |= (BCHP_FIELD_DATA (AUD_FMM_MS_CTRL_FW_STREAM_CTRL_0, OVERWRITE_DATA, 1));    
    }
    BRAP_Write32 (hSpdifFm->hRegister, 
                  BCHP_AUD_FMM_MS_CTRL_FW_STREAM_CTRL_0 + hSpdifFm->ui32Offset, 
                  ui32RegVal);     

    BDBG_LEAVE (BRAP_SPDIFFM_P_InsertBurstOnMute);
    return ret;       
}

BERR_Code 
BRAP_SPDIFFM_P_Open (
    BRAP_FMM_P_Handle         hFmm,             /* [in] Parent FMM handle */
    BRAP_SPDIFFM_P_Handle *   phSpdifFm,        /* [out] SPDIFFM stream handle */ 
    unsigned int              uiStreamIndex,    /* [in] SPDIFFM Stream index */
    const BRAP_SPDIFFM_P_Settings * pSettings   /* [in] The SPDIFFM stream settings*/                                         
)
{
    BERR_Code ret = BERR_SUCCESS;
    BRAP_SPDIFFM_P_Handle hSpdifFm;
    uint32_t ui32RegValue=0;

    BDBG_ENTER (BRAP_SPDIFFM_P_Open);

    /* 1. Check all input parameters to the function. Return error if
      - FMM handle is NULL
      - Given index exceeds maximum no. of SPDIF Formatters
      - Pointer to Settings structure is NULL
      - Pointer to SPDIF Formatter handle is NULL     */
    BDBG_ASSERT (hFmm);
    BDBG_ASSERT (phSpdifFm);
    if (uiStreamIndex > (BRAP_RM_P_MAX_SPDIFFM_STREAMS - 1))
    {
        return BERR_TRACE (BERR_NOT_SUPPORTED);
    }    
    
    if (BRAP_FMM_P_GetWatchdogRecoveryFlag (hFmm) == false)
    {   /* If not in WatchDog recovery */  

        BDBG_ASSERT (pSettings);
        if (hFmm->hSpdifFm[uiStreamIndex] != NULL)
        {

            /* This SPDIFFM stream was previoulsy opened by another audio channel.
            So this time, just increment the open count. Dont change the 
            settings etc. */
            /* OpenCount cannot exceed the no. of Mixer inputs!! */
            hSpdifFm = hFmm->hSpdifFm[uiStreamIndex];

#if (BRAP_DVD_FAMILY != 1)
            /* In 7440, an SPDIFFM can be opened more than the number of mixer 
               inputs */
            if (hSpdifFm->uiOpenCnt >= BRAP_RM_P_MAX_MIXER_INPUTS)
            {
                BDBG_ERR (("BRAP_SPDIFFM_P_Open: SpdifFm stream %d already has %d open"
                        "audio channels. Cannot add another!!!",
                        uiStreamIndex, hSpdifFm->uiOpenCnt));
                return BERR_TRACE (BERR_NOT_SUPPORTED);
            }
#endif        
            *phSpdifFm = hSpdifFm;
            hSpdifFm->uiOpenCnt ++;
        
            
            BDBG_MSG (("BRAP_SPDIFFM_P_Open: SpdifFm stream %d was already open." 
                    "New open count = %d", 
                        uiStreamIndex, hSpdifFm->uiOpenCnt));
            BDBG_LEAVE (BRAP_SPDIFFM_P_Open);
            return ret;
        }
      
        /* 2. Allocate memory for the SPDIF Formatter handle. Fill in parameters 
        in the SPDIF Formatter handle. */
    
        /* Allocate Mixer handle */
        hSpdifFm = (BRAP_SPDIFFM_P_Handle) BKNI_Malloc (sizeof(BRAP_SPDIFFM_P_Object));
        if (hSpdifFm == NULL)
        {
            return BERR_TRACE (BERR_OUT_OF_SYSTEM_MEMORY);
        }
        
        /* Clear the handle memory */
        BKNI_Memset(hSpdifFm, 0, sizeof(BRAP_SPDIFFM_P_Object));

        /* Initialise known elements in Source Channel handle */
        hSpdifFm->hChip = hFmm->hChip;
        hSpdifFm->hRegister = hFmm->hRegister;
        hSpdifFm->hHeap = hFmm->hHeap;
        hSpdifFm->hInt = hFmm->hInt;
        hSpdifFm->hFmm = hFmm;
        hSpdifFm->uiStreamIndex = uiStreamIndex;
        hSpdifFm->uiOpenCnt = 1;
        hSpdifFm->uiStartCnt = 0;
#if BRAP_SPDIFFM_ASYNC_TX_RE_ENABLE
		hSpdifFm->hTimer = NULL;
#endif
        
        BRAP_RM_P_GetSpdifFmId (uiStreamIndex, &(hSpdifFm->uiIndex));

        /* SPDIF Formatter register offset */
        hSpdifFm->ui32Offset = (BCHP_AUD_FMM_MS_CTRL_FW_STREAM_CTRL_1 - 
                          BCHP_AUD_FMM_MS_CTRL_FW_STREAM_CTRL_0)
                          * hSpdifFm->uiStreamIndex;
      
        /* Fill up the Settings Structure */
        hSpdifFm->sSettings = *pSettings;

#if BRAP_SPDIFFM_ASYNC_TX_RE_ENABLE
        if(NULL != hFmm->hRap->hTmr)
		{
			BTMR_Settings sTimerSettings;

			BTMR_GetDefaultTimerSettings(&sTimerSettings);
			sTimerSettings.type = BTMR_Type_eCountDown;
			sTimerSettings.cb_isr = (BTMR_CallbackFunc) BRAP_P_Spdif_Tx_Enable_isr;
			sTimerSettings.pParm1 = hSpdifFm;
			sTimerSettings.parm2 = 0;
			sTimerSettings.exclusive = false;

			ret = BTMR_CreateTimer (hFmm->hRap->hTmr, &hSpdifFm->hTimer, &sTimerSettings);
            BDBG_MSG (("BRAP_SPDIFFM_P_Open: BTMR_CreateTimer 0x%x", (unsigned int) hSpdifFm->hTimer));
			if(ret != BERR_SUCCESS)
			{
				BDBG_ERR(("BRAP_SPDIFFM_P_Open(): BTMR_CreateTimer returned err(%d)",ret));
				return BERR_TRACE (BERR_NOT_SUPPORTED);
			}
		}
        else
        {
            BDBG_ERR(("BRAP_SPDIFFM_P_Open: Could not create timer Timer Device Handle is NUll"));
            return BERR_TRACE (BERR_NOT_SUPPORTED);
        }
#endif

    }   /* End: If not in WatchDog recovery */
    else
    {
        hSpdifFm = *phSpdifFm; 
    }

    /* dither */
    ui32RegValue = BRAP_Read32 (hSpdifFm->hRegister, BCHP_AUD_FMM_MS_CTRL_FW_STREAM_CTRL_0 + hSpdifFm->ui32Offset);
    ui32RegValue &= ~( (BCHP_MASK (AUD_FMM_MS_CTRL_FW_STREAM_CTRL_0, DITHER_ENA))
                       |(BCHP_MASK (AUD_FMM_MS_CTRL_FW_STREAM_CTRL_0, INSERT_WHEN_DISA)));

    if(hSpdifFm->sSettings.sExtSettings.bEnableDither)
        ui32RegValue |= (BCHP_FIELD_ENUM (AUD_FMM_MS_CTRL_FW_STREAM_CTRL_0, 
                                      DITHER_ENA, Enable));
    else
        ui32RegValue |= (BCHP_FIELD_ENUM (AUD_FMM_MS_CTRL_FW_STREAM_CTRL_0, 
                                      DITHER_ENA, Disable));

    /* Insert dither signal even when STREAM_ENA is off */
    ui32RegValue |= (BCHP_FIELD_ENUM (AUD_FMM_MS_CTRL_FW_STREAM_CTRL_0, 
                                      INSERT_WHEN_DISA, Insert));

#if (BCHP_CHIP == 7400 && BCHP_VER ==BCHP_VER_A0 ) || (BRAP_7401_FAMILY == 1)
    ui32RegValue &= ~(BCHP_MASK (AUD_FMM_MS_CTRL_FW_STREAM_CTRL_0, OVERWRITE_DATA));    
    ui32RegValue &= ~(BCHP_MASK (AUD_FMM_MS_CTRL_FW_STREAM_CTRL_0, CSTAT_ONLY)); 
#endif	
    
    BRAP_Write32 (hSpdifFm->hRegister,
                  hSpdifFm->ui32Offset + BCHP_AUD_FMM_MS_CTRL_FW_STREAM_CTRL_0,
                  ui32RegValue); 

    /* Disable BURST - insert neither pause nor null burst */
    ui32RegValue = BRAP_Read32 (hSpdifFm->hRegister, BCHP_AUD_FMM_MS_CTRL_FW_BURST_0 + hSpdifFm->ui32Offset);
    ui32RegValue &= ~(BCHP_MASK (AUD_FMM_MS_CTRL_FW_BURST_0, REP_PERIOD));     
    ui32RegValue |= (BCHP_FIELD_ENUM (AUD_FMM_MS_CTRL_FW_BURST_0, REP_PERIOD, None));
    BRAP_Write32 (hSpdifFm->hRegister, 
                  hSpdifFm->ui32Offset + BCHP_AUD_FMM_MS_CTRL_FW_BURST_0,
                  ui32RegValue);     

    /* PR 18296: if dither is enabled, we want the SPDIFFM to be alive always... and not disable it at stert/stop.
        So, we just enable it at opne time itself */

    /* Enable the SPDIF Formatter output */
    ui32RegValue = BRAP_Read32 (hSpdifFm->hRegister, BCHP_AUD_FMM_MS_CTRL_STRM_ENA);
	

    /* Program STREAMX_ENA field to 'enable' depending upon which 
       SPDIF Formatter Input is used */
    switch (hSpdifFm->uiStreamIndex)
    {
        case 0:
            ui32RegValue &= ~(BCHP_MASK (AUD_FMM_MS_CTRL_STRM_ENA, STREAM0_ENA));
            ui32RegValue |= BCHP_FIELD_ENUM (AUD_FMM_MS_CTRL_STRM_ENA, 
                                             STREAM0_ENA, Enable);
            break;
        case 1:
            ui32RegValue &= ~(BCHP_MASK (AUD_FMM_MS_CTRL_STRM_ENA, STREAM1_ENA));
            ui32RegValue |= BCHP_FIELD_ENUM (AUD_FMM_MS_CTRL_STRM_ENA, 
                                             STREAM1_ENA, Enable);
            break;
        default:
            BDBG_ERR (("BRAP_SPDIFFM_P_Open(): Incorrect SPDIFFM stream index %d", 
                        hSpdifFm->uiStreamIndex));
            return BERR_TRACE (BERR_INVALID_PARAMETER);
    }

    BRAP_Write32 (hSpdifFm->hRegister, BCHP_AUD_FMM_MS_CTRL_STRM_ENA,
                  ui32RegValue);

    BDBG_MSG(("hSpdifFm->sSettings:"));
    BDBG_MSG(("\t bEnableDither=%d", hSpdifFm->sSettings.sExtSettings.bEnableDither));    
    BDBG_MSG(("\t eBurstType=%d", hSpdifFm->sSettings.sExtSettings.eBurstType));        
    
    /* Configure SPDIF Formatter Hardware if reqd*/
        
    if (BRAP_FMM_P_GetWatchdogRecoveryFlag (hFmm) == false)
    {   /* If not in WatchDog recovery */  

        /* Store the SPDIF Formatter handle inside the FMM handle */
        hFmm->hSpdifFm[uiStreamIndex] = hSpdifFm;
 
        /* Return the filled handle */
        *phSpdifFm = hSpdifFm;
    }   /* End: If not in WatchDog recovery */
    
    BDBG_LEAVE(BRAP_SPDIFFM_P_Open);
    return ret;

}




BERR_Code 
BRAP_SPDIFFM_P_Close ( 
    BRAP_SPDIFFM_P_Handle  hSpdifFm        /* [in] SPDIF Formatter Stream handle */
)
{
    BERR_Code ret = BERR_SUCCESS;
    uint32_t ui32RegValue = 0;
    
    BDBG_ENTER (BRAP_SPDIFFM_P_Close);
    BDBG_ASSERT (hSpdifFm);
#if 0 
    if (hSpdifFm->eState != BRAP_SPDIFFM_P_State_eStopped)
    {
        BDBG_ERR (("BRAP_SPDIFFM_P_Close: SPDIFFM %d stream %d is still active"
                    "Cant close it!! First stop this stream!!",
                    hSpdifFm->uiIndex, hSpdifFm->uiStreamIndex));
        return BERR_TRACE (BERR_INVALID_PARAMETER);
    }
#endif
    if (hSpdifFm->uiOpenCnt == 0)
    { 
        /* This should never happen. If it does, it means the system
           has gone into an invalid state!!!!*/
            BDBG_ERR (("BRAP_SPDIFFM_P_Close: SPDIFFM Stream %d has NO open"
                        "audio channels!!!", hSpdifFm->uiStreamIndex));
            return BERR_TRACE (BERR_NOT_SUPPORTED);
    }
    hSpdifFm->uiOpenCnt --;

    if (hSpdifFm->uiOpenCnt == 0)
    {   
        /* If this was the last open audio channel,
           free the handle etc */

#if BRAP_SPDIFFM_ASYNC_TX_RE_ENABLE
		if (hSpdifFm->hTimer != NULL)
		{
			BTMR_DestroyTimer(hSpdifFm->hTimer);
			hSpdifFm->hTimer = NULL;
		}
#endif
        /* PR 18296: if dither is enabled, we want the SPDIFFM to be alive always... and not disable it at start/stop.
        So, we just enable it at opne time itself and disable at close */
        
        ui32RegValue = BRAP_Read32 (hSpdifFm->hRegister, 
                BCHP_AUD_FMM_MS_CTRL_STRM_ENA);

        /* Program STREAMX_ENA field to 'disable' depending upon which 
           SPDIF Formatter is used */
/* PR45478 : Brief HDMI signal loss on Samsung */
/* PR46653 : Noise heard in changing between PCM to compressed through Dynex TV HDMI */           
#if 0           
        switch (hSpdifFm->uiStreamIndex)
        {
            case 0:
                ui32RegValue &= ~(BCHP_MASK (AUD_FMM_MS_CTRL_STRM_ENA, STREAM0_ENA));
                ui32RegValue |= BCHP_FIELD_ENUM (AUD_FMM_MS_CTRL_STRM_ENA, 
                                                 STREAM0_ENA, Disable);
                break;
            case 1:
                ui32RegValue &= ~(BCHP_MASK (AUD_FMM_MS_CTRL_STRM_ENA, STREAM1_ENA));
                ui32RegValue |= BCHP_FIELD_ENUM (AUD_FMM_MS_CTRL_STRM_ENA, 
                                                 STREAM1_ENA, Disable);
                break;
            default:
                BDBG_ERR (("BRAP_SPDIFFM_P_Stop(): Invalid input index 0x%08x ", 
                            hSpdifFm->uiStreamIndex));    
                return BERR_TRACE (BERR_NOT_SUPPORTED);
        }
#endif
        BRAP_Write32 (hSpdifFm->hRegister, 
                      BCHP_AUD_FMM_MS_CTRL_STRM_ENA,
                      ui32RegValue);


        /* Remove referrence to this SPDIF Formatter from the parent FMM */ 
        hSpdifFm->hFmm->hSpdifFm[hSpdifFm->uiStreamIndex] = NULL;

	BDBG_MSG (("BRAP_SPDIFFM_P_Close: SPDIFFM Stream %d closed. hSpdifFm->uiOpenCnt=%d",
		hSpdifFm->uiStreamIndex, hSpdifFm->uiOpenCnt));

        /* Free the SPDIF Formatter Handle memory*/
        BKNI_Free (hSpdifFm); 
    }      
    BDBG_LEAVE (BRAP_SPDIFFM_P_Close);
    return ret;
}


BERR_Code 
BRAP_SPDIFFM_P_Start ( 
	BRAP_ChannelHandle 		hRapCh,		  /* [in] Rap channel handle */	
    BRAP_SPDIFFM_P_Handle   hSpdifFm,     /* [in] SPDIF Formatter handle */
    const BRAP_SPDIFFM_P_Params * pParams /* [in] Pointer to start
                                                  parameters */ 
)
{
    BERR_Code ret = BERR_SUCCESS;

    BDBG_ENTER (BRAP_SPDIFFM_P_Start);
    BDBG_ASSERT (hSpdifFm);
    
    BDBG_MSG (("BRAP_SPDIFFM_P_Start(): hSpdifFm=0x%x SPDIFFM Stream index=%d ", hSpdifFm, hSpdifFm->uiStreamIndex)); 

    if (BRAP_FMM_P_GetWatchdogRecoveryFlag (hSpdifFm->hFmm) == false)
    {   /* If not in WatchDog recovery */  
        BDBG_ASSERT (pParams);

        if (hSpdifFm->uiStartCnt > 0)
        {
            /* This hSpdifFm was previoulsy started by another audio channel.
            So this time, just increment the start count. Dont change the 
            settings etc. */
            /* start Count cannot exceed the open count!! */
            if (hSpdifFm->uiStartCnt >= hSpdifFm->uiOpenCnt)
            {
                BDBG_ERR (("BRAP_SPDIFFM_P_Start: Cannot start SPDIFFM Stream %d. "
                        "Start Count (%d) >= Open Count (%d)",
                        hSpdifFm->uiStreamIndex, hSpdifFm->uiStartCnt, hSpdifFm->uiOpenCnt));
                return BERR_TRACE (BERR_NOT_SUPPORTED);
            }
        
            hSpdifFm->uiStartCnt ++;

            BDBG_MSG (("BRAP_SPDIFFM_P_Start: SPDIFFM Stream %d was already started." 
                    "New start count = %d", 
                    hSpdifFm->uiStreamIndex, hSpdifFm->uiStartCnt));
            BDBG_LEAVE (BRAP_OP_P_Start);
            return ret;
        }

        /* Store the start parameters */
        hSpdifFm->sParams = *pParams;   
    }

#if 0    
    if (hSpdifFm->eState != BRAP_SPDIFFM_P_State_eStopped)
    {

        BDBG_WRN (("BRAP_SPDIFFM_P_Start: SPDIFFM %d stream %d is already active!!"
                    "Ignoring this call to _Start()!!",
                    hSpdifFm->uiIndex, hSpdifFm->uiStreamIndex));
        BDBG_LEAVE (BRAP_SPDIFFM_P_Start);
        return BERR_SUCCESS; 
    }    
#endif
    /* Configure SPDIF Formatter Hardware */
    BRAP_SPDIFFM_P_HWConfig (hRapCh, hSpdifFm);
    if(ret != BERR_SUCCESS)
    {
    	BDBG_ERR(("BRAP_SPDIFFM_P_Start(): BRAP_SPDIFFM_P_HWConfig() failed(%d)",ret));
    	return BERR_TRACE(ret);
    }

    if ((BRAP_FMM_P_GetWatchdogRecoveryFlag (hSpdifFm->hFmm) == false)
          && (ret == BERR_SUCCESS))
    {   /* If not in WatchDog recovery */  
        hSpdifFm->uiStartCnt ++;
    }
    
    BDBG_LEAVE (BRAP_SPDIFFM_P_Start);
    return ret;
}


BERR_Code 
BRAP_SPDIFFM_P_Stop (
    BRAP_SPDIFFM_P_Handle  hSpdifFm       /* [in] SPDIF Formatter handle */
)
{
    BERR_Code ret = BERR_SUCCESS;
    uint32_t  ui32RegValue;

    BDBG_ENTER (BRAP_SPDIFFM_P_Stop);
    BDBG_ASSERT (hSpdifFm);
  
    if (hSpdifFm->uiStartCnt == 0)
    {
        BDBG_WRN (("BRAP_SPDIFFM_P_Stop: SPDIFFM %d stream %d is not active!!"
                    "Ignoring this call to _Stop()!!",
                    hSpdifFm->uiIndex, hSpdifFm->uiStreamIndex));
        BDBG_LEAVE (BRAP_SPDIFFM_P_Stop);
        return BERR_SUCCESS;  
    }   
    hSpdifFm->uiStartCnt --;   

    BDBG_MSG (("BRAP_SPDIFFM_P_Stop: SPDIFFM Stream  %d new start count =%d",
                hSpdifFm->uiStreamIndex, hSpdifFm->uiStartCnt));
    if (hSpdifFm->uiStartCnt != 0)
    {
            BDBG_LEAVE (BRAP_SPDIFFM_P_Stop);
            return ret;
    }    

    ui32RegValue = BRAP_Read32 (hSpdifFm->hRegister, 
            BCHP_AUD_FMM_MS_CTRL_FW_STREAM_CTRL_0 + hSpdifFm->ui32Offset);
    ui32RegValue &= ~( BCHP_MASK (AUD_FMM_MS_CTRL_FW_STREAM_CTRL_0, STREAM_ENA) );    
    ui32RegValue |= (BCHP_FIELD_ENUM (AUD_FMM_MS_CTRL_FW_STREAM_CTRL_0, 
                                      STREAM_ENA, Disable));
    BRAP_Write32 (hSpdifFm->hRegister, 
                  hSpdifFm->ui32Offset + BCHP_AUD_FMM_MS_CTRL_FW_STREAM_CTRL_0,
                  ui32RegValue); 
    
    BDBG_LEAVE (BRAP_SPDIFFM_P_Stop);
    return ret;
}


#if BRAP_SPDIFFM_ASYNC_TX_RE_ENABLE
/***************************************************************************
Description:
	This timer callback turns SPDIF TX back on after the alotted time
Returns:
	void 
See Also:
	BRAP_P_Spdif_Tx_Enable_isr
***************************************************************************/
static void BRAP_P_Spdif_Tx_Enable_isr (
        void * pParm1, /* [in]  BRAP_SPDIFFM_P_Handle*/
        int    iParm2  /* [in]  */
)
{
	BRAP_SPDIFFM_P_Handle   hSpdifFm;

    BDBG_ENTER (BRAP_P_Spdif_Tx_Enable_isr);    
	BDBG_ASSERT (pParm1);
	BSTD_UNUSED ( iParm2 );

	hSpdifFm = (BRAP_SPDIFFM_P_Handle) pParm1;

	BDBG_MSG(("BRAP_P_Spdif_Tx_Enable_isr() hSpdifFm 0x%x",(unsigned int)hSpdifFm->hTimer));

	BRAP_SPDIFFM_P_SetSPDIFtx_isr(hSpdifFm, true);

	BDBG_LEAVE (BRAP_P_Spdif_Tx_Enable_isr);
	return;
}


/***************************************************************************
Description:
	Turns the Spdif Tx 
Returns:
	void 
See Also:
	BRAP_P_Spdif_Tx_Enable_isr,BRAP_SPDIFFM_P_SetSPDIFtx
***************************************************************************/
static BERR_Code BRAP_SPDIFFM_P_SetSPDIFtx_isr(
    BRAP_SPDIFFM_P_Handle   hSpdifFm,    /* [in] SPDIF Formatter handle */
	bool					bEnableTx	 /* [in] bool to turn on or off */
	)
{
	BERR_Code ret = BERR_SUCCESS;
	uint32_t  ui32RegVal=0;
	uint32_t  ui32RegOffset=0;
	bool	  bTxEnabled=false;

    BDBG_ENTER (BRAP_SPDIFFM_P_SetSPDIFtx_isr);

    if(hSpdifFm->uiStreamIndex == 0)
        ui32RegOffset=0;
    else if (hSpdifFm->uiStreamIndex == 1)
        ui32RegOffset = BCHP_AUD_FMM_OP_CTRL_SPDIF_CFG_1 - BCHP_AUD_FMM_OP_CTRL_SPDIF_CFG_0;

    ui32RegVal = BRAP_Read32 (hSpdifFm->hRegister, BCHP_AUD_FMM_OP_CTRL_SPDIF_CFG_0 + ui32RegOffset);
	bTxEnabled =  (ui32RegVal & (BCHP_MASK (AUD_FMM_OP_CTRL_SPDIF_CFG_0, CLOCK_ENABLE))) != 0;
	ui32RegVal &= ~(BCHP_MASK (AUD_FMM_OP_CTRL_SPDIF_CFG_0, CLOCK_ENABLE));

	BDBG_MSG(("BRAP_SPDIFFM_P_SetSPDIFtx(): bEnableTx %d bTxEnabled %d  hSpdifFm->hTimer 0x%x", bEnableTx, bTxEnabled, (unsigned int)hSpdifFm->hTimer));

	if (true == bEnableTx)
	{
		/* should only be call by isr */
		if (false == bTxEnabled)
		{
			/* turn TX on only if it is off */
			ui32RegVal |= (BCHP_FIELD_ENUM(AUD_FMM_OP_CTRL_SPDIF_CFG_0,  
                                    CLOCK_ENABLE, Enable));

			BRAP_Write32 (hSpdifFm->hRegister, 
					   BCHP_AUD_FMM_OP_CTRL_SPDIF_CFG_0 + ui32RegOffset, 
					   ui32RegVal);
		}
	}

	else
	{
		/* turn TX off */
		ui32RegVal |= (BCHP_FIELD_ENUM(AUD_FMM_OP_CTRL_SPDIF_CFG_0,  
									CLOCK_ENABLE, Disable));

		BRAP_Write32 (hSpdifFm->hRegister, 
				   BCHP_AUD_FMM_OP_CTRL_SPDIF_CFG_0 + ui32RegOffset, 
				   ui32RegVal);

		/* set timer */
		if (hSpdifFm->hTimer != NULL)
		{
			ret = BTMR_StartTimer(hSpdifFm->hTimer, SPDIF_TX_OFF_MICROSECS);
			BDBG_MSG(("BRAP_SPDIFFM_P_SetSPDIFtx(): BTMR_StartTimer (0x%x, %d) returned err(%d)",(unsigned int)hSpdifFm->hTimer, SPDIF_TX_OFF_MICROSECS, ret));
			if (ret != BERR_SUCCESS)
			{
				BDBG_ERR(("BRAP_SPDIFFM_P_SetSPDIFtx(): BTMR_StartTimer returned err(%d)",ret));
			}
		}
		else
		{
			BDBG_MSG(("BRAP_SPDIFFM_P_SetSPDIFtx():hSpdifFm->hTimer == NULL"));
			ret = BERR_NOT_SUPPORTED;
		}

	}
    BDBG_LEAVE (BRAP_SPDIFFM_P_SetSPDIFtx_isr);

	return ret;
}

/***************************************************************************
Description:
	Turns the Spdif Tx 
Returns:
	void 
See Also:
	BRAP_P_Spdif_Tx_Enable_isr, BRAP_SPDIFFM_P_SetSPDIFtx_isr
***************************************************************************/
static BERR_Code BRAP_SPDIFFM_P_SetSPDIFtx(
    BRAP_SPDIFFM_P_Handle   hSpdifFm,    /* [in] SPDIF Formatter handle */
	bool					bEnableTx	 /* [in] bool to turn on or off */
	)
{
	BERR_Code ret = BERR_SUCCESS;

    BDBG_ENTER (BRAP_SPDIFFM_P_SetSPDIFtx);

    ret = BRAP_SPDIFFM_P_SetSPDIFtx_isr(hSpdifFm, bEnableTx);

    BDBG_LEAVE (BRAP_SPDIFFM_P_SetSPDIFtx);
    return ret;
}
#endif

/***************************************************************************
Summary:
    Configures the HW registers for the SPDIF Formatter
    Must be called only after the output port has been configured and opened.    
Description:

Returns:
    BERR_SUCCESS on success
    Error code on failure

See Also:
    None.
**************************************************************************/
static BERR_Code 
BRAP_SPDIFFM_P_HWConfig ( 
	BRAP_ChannelHandle 		hRapCh,		/* [in] Rap channel handle */	
    BRAP_SPDIFFM_P_Handle   hSpdifFm    /* [in] SPDIF Formatter handle */
)
{
   
	BERR_Code ret = BERR_SUCCESS;
	BRAP_SPDIFFM_P_Params sParams;
	uint32_t  ui32RegVal;
#if BRAP_SPDIFFM_ASYNC_TX_RE_ENABLE
	bool		bLastCompressed = false;
#endif

#if (BRAP_7440_ARCH == 1)
	uint32_t  ui32SpdifChStatusBits[BRAP_SPDIFFM_CBIT_BUFFER_SIZE];
	unsigned int uiCnt = 0;
#endif	

#if ( BRAP_7440_ARCH == 0 )
	BSTD_UNUSED(hRapCh);
#endif
	
    BDBG_ENTER (BRAP_SPDIFFM_P_HWConfig);
    BDBG_ASSERT (hSpdifFm);
    BDBG_MSG (("BRAP_SPDIFFM_P_HWConfig(): hSpdifFm=0x%x SPDIFFM Stream index=0x%08x ", hSpdifFm, hSpdifFm->uiStreamIndex));   
  
    sParams = hSpdifFm->sParams;    

    BDBG_MSG(("BRAP_SPDIFFM_P_HWConfig: hSpdifFm->sSettings:"));
    BDBG_MSG(("\t bEnableDither=%d", hSpdifFm->sSettings.sExtSettings.bEnableDither));    
    BDBG_MSG(("\t eBurstType=%d", hSpdifFm->sSettings.sExtSettings.eBurstType));        

    ret = BRAP_SPDIFFM_P_ProgramDitherAndBurst(hSpdifFm, 
                                               &hSpdifFm->sSettings,
                                               sParams.eBurstRepPeriod);
	if(ret != BERR_SUCCESS)
	{
		BDBG_ERR(("BRAP_SPDIFFM_P_HWConfig(): ProgramDitherAndBurst returned err(%d)",ret));
		return BERR_TRACE(ret);
	}
    
    ui32RegVal = BRAP_Read32 (hSpdifFm->hRegister, BCHP_AUD_FMM_MS_CTRL_FW_STREAM_CTRL_0 + hSpdifFm->ui32Offset);
    
#if BRAP_SPDIFFM_ASYNC_TX_RE_ENABLE
	bLastCompressed = (ui32RegVal & (BCHP_MASK (AUD_FMM_MS_CTRL_FW_STREAM_CTRL_0, COMP_OR_LINEAR))) != 0;
#endif
    ui32RegVal &= ~( (BCHP_MASK (AUD_FMM_MS_CTRL_FW_STREAM_CTRL_0, CHAN_OVERRIDE)) 
                     | (BCHP_MASK (AUD_FMM_MS_CTRL_FW_STREAM_CTRL_0, VALIDITY)) 
                     | (BCHP_MASK (AUD_FMM_MS_CTRL_FW_STREAM_CTRL_0, COMP_OR_LINEAR))
                     | (BCHP_MASK (AUD_FMM_MS_CTRL_FW_STREAM_CTRL_0, SPDIF_OR_PCM))
                     | (BCHP_MASK (AUD_FMM_MS_CTRL_FW_STREAM_CTRL_0, FLUSH_ON_UFLOW))
                     | (BCHP_MASK (AUD_FMM_MS_CTRL_FW_STREAM_CTRL_0, INSERT_ON_UFLOW))
#if ( BCHP_CHIP != 3563 )
                     | (BCHP_MASK (AUD_FMM_MS_CTRL_FW_STREAM_CTRL_0, INSERT_WHEN_DISA))      
#endif                     
#if (BRAP_DVD_FAMILY == 1)
                     | (BCHP_MASK (AUD_FMM_MS_CTRL_FW_STREAM_CTRL_0, CSTAT_ONLY))      
#endif
                     | (BCHP_MASK (AUD_FMM_MS_CTRL_FW_STREAM_CTRL_0, STREAM_ENA)) );

    if (sParams.bSeparateLRChanNum == true)
    {
        ui32RegVal |= (BCHP_FIELD_DATA (AUD_FMM_MS_CTRL_FW_STREAM_CTRL_0, CHAN_OVERRIDE, 1));
    }
    if (sParams.sExtParams.bSpdifFormat == true)
    {
        ui32RegVal |= (BCHP_FIELD_DATA (AUD_FMM_MS_CTRL_FW_STREAM_CTRL_0, SPDIF_OR_PCM, 1));
        if (sParams.bCompressed == true)
        {
            ui32RegVal |= (BCHP_FIELD_DATA (AUD_FMM_MS_CTRL_FW_STREAM_CTRL_0, 
                                            VALIDITY, 1));
        } 
        else  
        {
            ui32RegVal |= (BCHP_FIELD_DATA (AUD_FMM_MS_CTRL_FW_STREAM_CTRL_0, 
                                            VALIDITY, 0));
        }             
    }
	else
	{
	    ui32RegVal |= (BCHP_FIELD_DATA (AUD_FMM_MS_CTRL_FW_STREAM_CTRL_0, SPDIF_OR_PCM, 0));
	}

    if (sParams.bCompressed == true)
    {
        ui32RegVal |= (BCHP_FIELD_DATA (AUD_FMM_MS_CTRL_FW_STREAM_CTRL_0, COMP_OR_LINEAR, 1));
    }    
	else  
	{
	    ui32RegVal |= (BCHP_FIELD_DATA (AUD_FMM_MS_CTRL_FW_STREAM_CTRL_0, COMP_OR_LINEAR, 0));
    }    
#if (BRAP_DVD_FAMILY == 1)
    if(hSpdifFm->sParams.bHbrEnable == true)
    {
        ui32RegVal |= (BCHP_FIELD_DATA (AUD_FMM_MS_CTRL_FW_STREAM_CTRL_0, CSTAT_ONLY, 1));
    }
    else
    {
        ui32RegVal |= (BCHP_FIELD_DATA (AUD_FMM_MS_CTRL_FW_STREAM_CTRL_0, CSTAT_ONLY, 0));
    }
#endif

#if BRAP_SPDIFFM_ASYNC_TX_RE_ENABLE
	if (bLastCompressed != sParams.bCompressed)
	{
		BDBG_MSG(("BRAP_SPDIFFM_P_HWConfig last_compressed(%d) != sParams.bCompressed(%d)",bLastCompressed, sParams.bCompressed));

		ret = BRAP_SPDIFFM_P_SetSPDIFtx(hSpdifFm, false);
		if (ret != BERR_SUCCESS)
		{
			BDBG_ERR(("BRAP_SPDIFFM_P_SetSPDIFtx( false): failed (%d)",ret));
			/* if failed, could not start timer */
			BRAP_SPDIFFM_P_SetSPDIFtx(hSpdifFm, true);
		}
	}
#endif

	/* In 7440 based arch for channel type other than decode, CBIT buffer need to be programmed */
	/* But for 7401 and 7400 chip we update the CBITs irrespective of Channel type */
#if BRAP_7440_ARCH
	if(hRapCh->eChannelType != BRAP_ChannelType_eDecode)
#endif     
	{
#if (BRAP_7440_ARCH == 1)
		ret = BRAP_SPDIFFM_P_ProgramChanStatusBits(hSpdifFm,hSpdifFm->sParams.eSamplingRate);
#else
		ret = BRAP_SPDIFFM_P_ProgramChanStatusBits(hSpdifFm);
#endif
		if(ret != BERR_SUCCESS)
		{
			BDBG_ERR(("BRAP_SPDIFFM_P_HWConfig(): ProgramChanStatusBits returned err(%d)",ret));
			return BERR_TRACE(ret);
		}
	}

#if (BRAP_7440_ARCH==1)
	if(( hRapCh->eChannelType == BRAP_ChannelType_eDecode ) 
		&&( true==hSpdifFm->sParams.bUseSpdifPackedChanStatusBits ))
	{
		for(uiCnt = 0; uiCnt < BRAP_SPDIFFM_CBIT_BUFFER_SIZE; uiCnt++)
		{
			ui32SpdifChStatusBits[uiCnt] = 0;
		}
		ui32SpdifChStatusBits[0] = hSpdifFm->sParams.sSpdifPackedChanStatusBits.ui32ChStatusBits[0];
		ui32SpdifChStatusBits[1] = hSpdifFm->sParams.sSpdifPackedChanStatusBits.ui32ChStatusBits[1];
		/* Second argument to function BRAP_SPDIFFM_P_ProgramCBITBuffer_isr() should have
		 * C-Bit values initialized in lower 16-bits of the array elements. */
		ui32SpdifChStatusBits[0] = hSpdifFm->sParams.sSpdifPackedChanStatusBits.ui32ChStatusBits[0] & 0xFFFF;
		ui32SpdifChStatusBits[1] = ( hSpdifFm->sParams.sSpdifPackedChanStatusBits.ui32ChStatusBits[0] & 0xFFFF0000 ) >> 16;
		ui32SpdifChStatusBits[2] = hSpdifFm->sParams.sSpdifPackedChanStatusBits.ui32ChStatusBits[1] & 0xFFFF;
		
    		BKNI_EnterCriticalSection();
		ret = BRAP_SPDIFFM_P_ProgramCBITBuffer_isr(hSpdifFm, &(ui32SpdifChStatusBits[0] )); 
    		BKNI_LeaveCriticalSection();

		if(ret != BERR_SUCCESS)
		{
			BDBG_ERR(("BRAP_SPDIFFM_P_HWConfig(): ProgramCBITBuffer_isr returned err(%d)",ret));
			return BERR_TRACE(ret);
		}
	}
#endif

    ui32RegVal |= (BCHP_FIELD_DATA (AUD_FMM_MS_CTRL_FW_STREAM_CTRL_0, INSERT_ON_UFLOW, 1));
    ui32RegVal |= (BCHP_FIELD_DATA (AUD_FMM_MS_CTRL_FW_STREAM_CTRL_0, FLUSH_ON_UFLOW, 1));
    ui32RegVal |= (BCHP_FIELD_DATA (AUD_FMM_MS_CTRL_FW_STREAM_CTRL_0, INSERT_WHEN_DISA, 1));

#if (BRAP_7401_FAMILY == 1) || ( BCHP_CHIP == 7400 )
    /* Decode on MAI */
    if(!(BRAP_P_ChannelType_eDecode == hRapCh->eChannelType) || (sParams.bCompressed == true))
        ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_MS_CTRL_FW_STREAM_CTRL_0, STREAM_ENA, Enable));
 
	BRAP_Write32 (hSpdifFm->hRegister,
                  BCHP_AUD_FMM_MS_CTRL_FW_RAMP_AMOUNT_0 + hSpdifFm->ui32Offset, 
                  0x0);  	
#else
    ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_MS_CTRL_FW_STREAM_CTRL_0, STREAM_ENA, Enable));
#endif

   
    BRAP_Write32 (hSpdifFm->hRegister, 
                  BCHP_AUD_FMM_MS_CTRL_FW_STREAM_CTRL_0 + hSpdifFm->ui32Offset, 
                  ui32RegVal); 
   
   /* Dont Bypass  the MS */
    BRAP_Write32 (hSpdifFm->hRegister, 
                  BCHP_AUD_FMM_MS_CTRL_USEQ_BYPASS,
                  0x0);

    /* Note: Currently the DSP FW programs the CBIT buffers. If not done by 
       the DSP FW, PI should do it here. */

    BDBG_LEAVE (BRAP_SPDIFFM_P_HWConfig);
    return ret;
}

#if (BRAP_7440_ARCH == 1)
/***************************************************************************
Summary:
    Prepares the CBIT buffer with the SPDIF channel status params
Description:
    NOTE: this PI should be called only by PB and CAP channels. 
    The DSP will program the CBIT buffers for a decode channel
Returns:
    BERR_SUCCESS on success
    Error code on failure

See Also:
	BRAP_SPDIFFM_P_ProgramCBITBuffer_isr
**************************************************************************/
static BERR_Code 
BRAP_SPDIFFM_P_PrepareCBITData_isr(
	uint32_t *pui32CBitData, 			/* [out] CBit buffer */
	BRAP_OP_SpdifChanStatusParams sSpdifChanStatusParams,
										/* SPDIF Channel status params */
	BAVC_AudioSamplingRate eOutputSamplingRate,
										/* Output sampling rate */
    bool                   bCompressed  /* TRUE: input data is compressed  */
)
{
	unsigned int uiSpdiffmSR;
	int count;
	bool bSwCopyRightAsserted;
	bool bProfMode, bSeparateLR;
	uint32_t ui32ChanDataLow, ui32ChanDataHigh;
	uint32_t ui32CategoryCode, ui32ClockAccuracy;
	uint32_t *pui32ChanDataPtr = pui32CBitData;

    BDBG_ENTER (BRAP_SPDIFFM_P_PrepareCBITData_isr);
    BSTD_UNUSED (bCompressed);
    
	bProfMode = sSpdifChanStatusParams.bProfessionalMode;
	bSeparateLR = sSpdifChanStatusParams.bSeparateLRChanNum;
	ui32ClockAccuracy = sSpdifChanStatusParams.ui16ClockAccuracy;
	ui32CategoryCode = (uint32_t)sSpdifChanStatusParams.ui16CategoryCode;
	bSwCopyRightAsserted = sSpdifChanStatusParams.bSwCopyRight;
	bSwCopyRightAsserted ^= 1;
	
	/*  These frequency mapping has been defined based on relation between 
	 	BAVC_AudioSamplingRate and SPDIF sampling frequencies
        Refer IEC 60958-3.
		Note that these are to be stored in bit reversed order	*/
	switch(eOutputSamplingRate)
    {   
  	    case BAVC_AudioSamplingRate_e32k:
            uiSpdiffmSR = 3; /* 1100 = 12 */
            break;
            
  	    case BAVC_AudioSamplingRate_e44_1k:
            uiSpdiffmSR = 0; /* 0000 = 0 */
            break;
            
  	    case BAVC_AudioSamplingRate_e48k:
            uiSpdiffmSR = 2; /* 0100 = 4 */
            break;
            
        case BAVC_AudioSamplingRate_e96k:
            uiSpdiffmSR = 10; /* 0101 = 5 */
            break;
            
        case BAVC_AudioSamplingRate_e22_05k:
            uiSpdiffmSR = 4; /* 0010 = 2 */
            break;
            
        case BAVC_AudioSamplingRate_e88_2k:
            uiSpdiffmSR = 8; /* 0001 = 1 */
            break;
            
        case BAVC_AudioSamplingRate_e176_4k:
            uiSpdiffmSR = 12; /* 0011 = 3 */
            break;
            
        case BAVC_AudioSamplingRate_e24k:
            uiSpdiffmSR = 6; /* 0110 = 6 */
            break;
            
        case BAVC_AudioSamplingRate_e192k:
            uiSpdiffmSR = 14; /* 0111 = 7 */
            break;
            
        default:
            BDBG_ERR(("BRAP_SPDIFFM_P_PrepareCBITData_isr: Not supported eOutputSamplingRate(%d)",
                eOutputSamplingRate));    
            return BERR_TRACE(BERR_INVALID_PARAMETER);
    }/* switch */
    
 	if(bProfMode)
  	{
   		*pui32ChanDataPtr++ = (0xC000)|(uiSpdiffmSR<<8);
		for(count=0;count<11;count++)
		    *pui32ChanDataPtr++=0x00000000; 
	}
	else
  	{
#if BRAP_SPDIFFM_ASYNC_TX_RE_ENABLE
   		ui32ChanDataLow = (ui32CategoryCode<<8)|(bSwCopyRightAsserted<<2)|
						  (bCompressed<<1);
#else
        ui32ChanDataLow = (ui32CategoryCode<<8)|(bSwCopyRightAsserted<<2);
#endif
   		ui32ChanDataHigh = (ui32ClockAccuracy<<12)|
						   (uiSpdiffmSR<<8)|
						   (bSeparateLR<<4);
   		*pui32ChanDataPtr++ = ui32ChanDataLow;
   		*pui32ChanDataPtr++ = ui32ChanDataHigh; 

            BDBG_MSG(("BRAP_SPDIFFM_P_PrepareCBITData_isr: (%d)(%d)",ui32ChanDataLow,ui32ChanDataHigh));

   		/* Remaining all 10 words are filled with zeros as per the specifications */
   		for(count=0;count<10;count++)
    		*pui32ChanDataPtr++=0x00000000; 
  	}

    BDBG_LEAVE(BRAP_SPDIFFM_P_PrepareCBITData_isr);
    return BERR_SUCCESS;
}
#else
/***************************************************************************
Summary:
    Prepares the CBIT buffer with the SPDIF channel status params
Description:
    NOTE: this PI should be called only by PB and CAP channels. 
    The DSP will program the CBIT buffers for a decode channel
Returns:
    BERR_SUCCESS on success
    Error code on failure

See Also:
	BRAP_SPDIFFM_P_ProgramCBITBuffer_isr
**************************************************************************/
static BERR_Code 
BRAP_SPDIFFM_P_PrepareCBITData_isr(
	uint32_t *pui32CBitData, 			/* [out] CBit buffer */
	BRAP_OP_SpdifChanStatusParams sSpdifChanStatusParams,
										/* SPDIF Channel status params */
	BAVC_AudioSamplingRate eOutputSamplingRate,
										/* Output sampling rate */
	bool                   bCompressed  /* TRUE: input data is compressed  */
)
{
	unsigned int uiSpdiffmSR;
	int count;
	bool bSwCopyRightAsserted;
	bool bProfMode, bSeparateLR,bLinear_PCM,bSampleWorldLength;
	uint32_t ui32ChanDataLow, ui32ChanDataHigh;
	uint32_t ui32CategoryCode, ui32ClockAccuracy;
	uint32_t uiPre_Emphasis,uiChannel_Status_Mode,uiSource_Number;
	uint32_t uiSample_Word_Length,uiOriginal_Sampling_Frequency,uiCGMS_A;
	uint32_t *pui32ChanDataPtr = pui32CBitData;

    BDBG_ENTER (BRAP_SPDIFFM_P_PrepareCBITData_isr);
    
	bProfMode = sSpdifChanStatusParams.bProfessionalMode;
	bLinear_PCM = sSpdifChanStatusParams.bLinear_PCM;
	bSwCopyRightAsserted = sSpdifChanStatusParams.bSwCopyRight;
	bSwCopyRightAsserted ^= 1;
	uiPre_Emphasis = sSpdifChanStatusParams.uiPre_Emphasis;
	uiChannel_Status_Mode =  sSpdifChanStatusParams.uiChannel_Status_Mode;
	ui32CategoryCode = (uint32_t)sSpdifChanStatusParams.ui16CategoryCode;	
	uiSource_Number = sSpdifChanStatusParams.uiSource_Number;
	bSeparateLR = sSpdifChanStatusParams.bSeparateLRChanNum;
	ui32ClockAccuracy =  sSpdifChanStatusParams.ui16ClockAccuracy;
	bSampleWorldLength = sSpdifChanStatusParams.bSampleWorldLength;
	uiSample_Word_Length = sSpdifChanStatusParams.uiSample_Word_Length;
	uiOriginal_Sampling_Frequency = sSpdifChanStatusParams.uiOriginal_Sampling_Frequency;
	uiCGMS_A = sSpdifChanStatusParams.uiCGMS_A;
	
	
	/*  These frequency mapping has been defined based on relation between 
	 	BAVC_AudioSamplingRate and SPDIF sampling frequencies
        Refer IEC 60958-3.
		Note that these are to be stored in bit reversed order	*/
	switch(eOutputSamplingRate)
    {   
  	    case BAVC_AudioSamplingRate_e32k:
            uiSpdiffmSR = 3; /* 1100 = 12 */
            break;
            
  	    case BAVC_AudioSamplingRate_e44_1k:
            uiSpdiffmSR = 0; /* 0000 = 0 */
            break;
            
  	    case BAVC_AudioSamplingRate_e48k:
            uiSpdiffmSR = 2; /* 0100 = 4 */
            break;
            
        case BAVC_AudioSamplingRate_e96k:
            uiSpdiffmSR = 10; /* 0101 = 5 */
            break;
            
        case BAVC_AudioSamplingRate_e22_05k:
            uiSpdiffmSR = 4; /* 0010 = 2 */
            break;
            
        case BAVC_AudioSamplingRate_e88_2k:
            uiSpdiffmSR = 8; /* 0001 = 1 */
            break;
            
        case BAVC_AudioSamplingRate_e176_4k:
            uiSpdiffmSR = 12; /* 0011 = 3 */
            break;
            
        case BAVC_AudioSamplingRate_e24k:
            uiSpdiffmSR = 6; /* 0110 = 6 */
            break;
            
        case BAVC_AudioSamplingRate_e192k:
            uiSpdiffmSR = 14; /* 0111 = 7 */
            break;
            
        default:
            BDBG_ERR(("BRAP_SPDIFFM_P_PrepareCBITData_isr: Not supported eOutputSamplingRate(%d)",
                eOutputSamplingRate));    
            return BERR_TRACE(BERR_INVALID_PARAMETER);
    }/* switch */
    
 	if(bProfMode)
  	{
   		*pui32ChanDataPtr++ = (0xC000)|(uiSpdiffmSR<<8);
		for(count=0;count<11;count++)
		    *pui32ChanDataPtr++=0x00000000; 
	}
	else
  	{
		ui32ChanDataLow = (bProfMode)|
						(bCompressed<<1)|
						  (bSwCopyRightAsserted<<2)|
						(uiPre_Emphasis<<3)|
						(uiChannel_Status_Mode<<6)|
						(ui32CategoryCode<<8);
	
		ui32ChanDataHigh =  (uiSource_Number)|
							(bSeparateLR<<4)|
							(uiSpdiffmSR<<8)|
							(ui32ClockAccuracy<<12);
	
		*pui32ChanDataPtr++ = ui32ChanDataLow;
		*pui32ChanDataPtr++ = ui32ChanDataHigh; 
		BDBG_MSG(("\nBRAP_SPDIFFM_P_PrepareCBITData_isr: FW_CBITS[0] (%x) FW_CBITS[1]  (%x)\n", ui32ChanDataLow,ui32ChanDataHigh));
              
		ui32ChanDataLow = 0;
		ui32ChanDataLow = (bSampleWorldLength)|
						(uiSample_Word_Length<<1)|
						(uiOriginal_Sampling_Frequency<<4)|
						(uiCGMS_A<<6);

		*pui32ChanDataPtr++ = ui32ChanDataLow;
				
		BDBG_MSG(("\nBRAP_SPDIFFM_P_PrepareCBITData_isr: FW_CBITS[2]  (%x)\n",ui32ChanDataLow));                

   		/* Remaining all 9 words are filled with zeros as per the specifications */
   		for(count=0;count<9;count++)
    		*pui32ChanDataPtr++=0x00000000; 
  	}

    BDBG_LEAVE(BRAP_SPDIFFM_P_PrepareCBITData_isr);
    return BERR_SUCCESS;
}
#endif

/***************************************************************************
Summary:
    Configures the HW registers with the CBIT buffer params
Description:
    ISR version of BRAP_SPDIFFM_P_ProgramCBITBuffer()
    NOTE: this PI should be called only by PB and CAP channels. 
    The DSP will program the CBIT buffers for a decode channel
Returns:
    BERR_SUCCESS on success
    Error code on failure

See Also:
	BRAP_SPDIFFM_P_PrepareCBITData_isr
**************************************************************************/
BERR_Code 
BRAP_SPDIFFM_P_ProgramCBITBuffer_isr( 
    BRAP_SPDIFFM_P_Handle   hSpdifFm,   /* [in] SPDIF Formatter handle */
    uint32_t			*pui32CBITBuffer /* [in] SPDIF bit buffer */
)
{
    uint32_t            	ui32CbitArrayOffset;
    uint32_t            	ui32Offset;
    uint32_t 				ui32RegValue; 
    unsigned int 			uiCnt = 0;
#if (BRAP_7440_ARCH == 1)
    uint32_t                ui32CpToggleRate = 0;
    BERR_Code               ret = BERR_SUCCESS;
    unsigned int            uiSR = 0;
#endif

    /* Validate input params */
    BDBG_ASSERT(hSpdifFm);
    BDBG_ASSERT(pui32CBITBuffer);

    /* Write to CBIT buffer */
    switch(hSpdifFm->uiStreamIndex)
    {
        case 0:
  	     BDBG_MSG(("BRAP_SPDIFFM_P_ProgramCBITBuffer_isr: Stream Index 0"));
            ui32CbitArrayOffset = 
                (	BRAP_SPDIFFM_PLAYBACK_MS0_CBIT_BUFFER * 
                    BCHP_AUD_FMM_MS_CTRL_FW_CBITSi_ARRAY_ELEMENT_SIZE / 4);
            break;
        case 1:
  	     BDBG_MSG(("BRAP_SPDIFFM_P_ProgramCBITBuffer_isr: Stream Index 1"));
            ui32CbitArrayOffset = 
                (   BRAP_SPDIFFM_PLAYBACK_MS1_CBIT_BUFFER * 
                    BCHP_AUD_FMM_MS_CTRL_FW_CBITSi_ARRAY_ELEMENT_SIZE / 4);
            break;
        default:
    		BDBG_ERR(("BRAP_SPDIFFM_P_ProgramCBITBuffer_isr: Not supported index (%d)",
				hSpdifFm->uiIndex));
            return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    /* Populate the selected CBIT buffer */
    for(uiCnt = 0; uiCnt < BRAP_SPDIFFM_CBIT_BUFFER_SIZE; uiCnt++)
    {
        BRAP_Write32_isr(hSpdifFm->hRegister, 
                     BCHP_AUD_FMM_MS_CTRL_FW_CBITSi_ARRAY_BASE + 
                     ui32CbitArrayOffset + (uiCnt*4), 
                     pui32CBITBuffer[uiCnt]);
  	     BDBG_MSG(("BRAP_SPDIFFM_P_ProgramCBITBuffer_isr: [%x]",BCHP_AUD_FMM_MS_CTRL_FW_CBITSi_ARRAY_BASE + 
                     ui32CbitArrayOffset + (uiCnt*4)));		
  	     BDBG_MSG(("BRAP_SPDIFFM_P_ProgramCBITBuffer_isr value[%d] : [%x]",uiCnt,pui32CBITBuffer[uiCnt]));	
    }

    /* Program the CBIT Buffer Offset / 4 to the Offset field of 
       BCHP_AUD_FMM_MS_CTRL_FW_CBIT_CTRL_X register */
    ui32Offset = BCHP_AUD_FMM_MS_CTRL_FW_CBITSi_ARRAY_BASE + 
                ui32CbitArrayOffset - BCHP_AUD_FMM_MS_CTRL_FW_STREAM_CTRL_0;

	ui32Offset = ui32Offset/4;

    ui32RegValue = BRAP_Read32_isr(hSpdifFm->hRegister,  
                        BCHP_AUD_FMM_MS_CTRL_FW_CBIT_CTRL_0 + 
                        hSpdifFm->ui32Offset);

    ui32RegValue &= ~(BCHP_MASK(AUD_FMM_MS_CTRL_FW_CBIT_CTRL_0, OFFSET));
	/* Though we dont ever write to CP_TOGGLE_RATE, make sure its cleared here 
    just in case it had a junk value earlier. */ 
    ui32RegValue &= ~(BCHP_MASK(AUD_FMM_MS_CTRL_FW_CBIT_CTRL_0, CP_TOGGLE_RATE));

#if (BRAP_7440_ARCH == 1)
    /* Program CP_TOGGLE_RATE only for 7440 based chips */
    if(hSpdifFm->sParams.uiCpToggleRate == BRAP_SPDIFFM_CP_TOGGLE_RATE_DISABLED)
    {
        /* CP_TOGGLE_RATE disabled - as in default */
        ui32CpToggleRate = 0;
    }
    else if((hSpdifFm->sParams.uiCpToggleRate >= BRAP_SPDIFFM_MIN_CP_TOGGLE_RATE_HZ) &&
       (hSpdifFm->sParams.uiCpToggleRate <= BRAP_SPDIFFM_MAX_CP_TOGGLE_RATE_HZ))
    {
        /* Convert eSR to uiSR */
        ret = BRAP_P_ConvertSR(hSpdifFm->sParams.eSamplingRate, &uiSR);
        if(ret != BERR_SUCCESS)
        {
            return BERR_TRACE(ret);
        }

        /* The Cp toggle rate to be programmed in the register equals 
           sample_rate/(192*cp_toggle_rate*2) */
        ui32CpToggleRate = (((uiSR / 192) / hSpdifFm->sParams.uiCpToggleRate) / 2);
        BDBG_MSG(("uiSR=%d hSpdifFm->sParams.uiCpToggleRate=%d ui32CpToggleRate=%d",
            uiSR, hSpdifFm->sParams.uiCpToggleRate, ui32CpToggleRate));
    }
    else
    {
        BDBG_ERR(("Valid Range (4-10). Invalid CpToggleRate = %d",hSpdifFm->sParams.uiCpToggleRate));
        return BERR_TRACE(BERR_NOT_SUPPORTED);    
    }

    ui32RegValue |= BCHP_FIELD_DATA(AUD_FMM_MS_CTRL_FW_CBIT_CTRL_0, 
                    CP_TOGGLE_RATE, 
                    ui32CpToggleRate);
#endif
    
    ui32RegValue |= BCHP_FIELD_DATA(AUD_FMM_MS_CTRL_FW_CBIT_CTRL_0, 
                        OFFSET, 
                        ui32Offset);

	BRAP_Write32_isr(hSpdifFm->hRegister, 
                 BCHP_AUD_FMM_MS_CTRL_FW_CBIT_CTRL_0 + 
                 hSpdifFm->ui32Offset, 
             	 ui32RegValue);

	BDBG_LEAVE(BRAP_SPDIFFM_P_ProgramCBITBuffer_isr);
    return BERR_SUCCESS;
}


/***************************************************************************
Summary:
    Prepares and programs channel status bits. 
Description:
    NOTE: this PI should be called only by PB and CAP channels. 
    The DSP will program the CBIT buffers for a decode channel
Returns:
    BERR_SUCCESS on success
    Error code on failure
See Also:
	BRAP_SPDIFFM_P_PrepareCBITData_isr, BRAP_SPDIFFM_P_ProgramCBITData_isr
**************************************************************************/
#if (BRAP_7440_ARCH == 1)
BERR_Code BRAP_SPDIFFM_P_ProgramChanStatusBits(
    BRAP_SPDIFFM_P_Handle   hSpdifFm,       /* [in] SPDIF Formatter handle */
    BAVC_AudioSamplingRate  eSamplingRate   /* [in] SPDIF Output SR */
    ) 
{
    BERR_Code ret = BERR_SUCCESS;
    BDBG_ENTER(BRAP_SPDIFFM_P_ProgramChanStatusBits);

    BKNI_EnterCriticalSection();
    ret = BRAP_SPDIFFM_P_ProgramChanStatusBits_isr(hSpdifFm,eSamplingRate);
    BKNI_LeaveCriticalSection();

    BDBG_ENTER(BRAP_SPDIFFM_P_ProgramChanStatusBits);
    return ret;
}
#else
BERR_Code BRAP_SPDIFFM_P_ProgramChanStatusBits(
    BRAP_SPDIFFM_P_Handle   hSpdifFm       /* [in] SPDIF Formatter handle */
    ) 
{
    BERR_Code ret = BERR_SUCCESS;
    BDBG_ENTER(BRAP_SPDIFFM_P_ProgramChanStatusBits);

    BKNI_EnterCriticalSection();
    ret = BRAP_SPDIFFM_P_ProgramChanStatusBits_isr(hSpdifFm);
    BKNI_LeaveCriticalSection();

    BDBG_ENTER(BRAP_SPDIFFM_P_ProgramChanStatusBits);
    return ret;
}
#endif
/***************************************************************************
Summary:
    Prepares and programs channel status bits. 
Description:
    NOTE: this PI should be called only by PB and CAP channels. 
    The DSP will program the CBIT buffers for a decode channel
Returns:
    BERR_SUCCESS on success
    Error code on failure
See Also:
	BRAP_SPDIFFM_P_PrepareCBITData_isr, BRAP_SPDIFFM_P_ProgramCBITData_isr
**************************************************************************/
#if (BRAP_7440_ARCH == 1)
BERR_Code BRAP_SPDIFFM_P_ProgramChanStatusBits_isr(
    BRAP_SPDIFFM_P_Handle   hSpdifFm,       /* [in] SPDIF Formatter handle */
    BAVC_AudioSamplingRate  eSamplingRate   /* [in] SPDIF Output SR */
) 
{
    BERR_Code ret = BERR_SUCCESS;
    uint32_t ui32CBitData[BRAP_SPDIFFM_CBIT_BUFFER_SIZE];
#if (BRAP_7440_ARCH == 1)
	uint32_t  ui32SpdifChStatusBits[BRAP_SPDIFFM_CBIT_BUFFER_SIZE];
	unsigned int uiCnt = 0;
#endif	


    BDBG_ENTER(BRAP_SPDIFFM_P_ProgramChanStatusBits_isr);

    BDBG_ASSERT(hSpdifFm);

    
#if (BRAP_7440_ARCH==1)
	if(true==hSpdifFm->sParams.bUseSpdifPackedChanStatusBits)
	{
		for(uiCnt = 0; uiCnt < BRAP_SPDIFFM_CBIT_BUFFER_SIZE; uiCnt++)
		{
			ui32SpdifChStatusBits[uiCnt] = 0;
		}
		ui32SpdifChStatusBits[0] = hSpdifFm->sParams.sSpdifPackedChanStatusBits.ui32ChStatusBits[0];
		ui32SpdifChStatusBits[1] = hSpdifFm->sParams.sSpdifPackedChanStatusBits.ui32ChStatusBits[1];
		/* Second argument to function BRAP_SPDIFFM_P_ProgramCBITBuffer_isr() should have
		 * C-Bit values initialized in lower 16-bits of the array elements. */
		ui32SpdifChStatusBits[0] = hSpdifFm->sParams.sSpdifPackedChanStatusBits.ui32ChStatusBits[0] & 0xFFFF;
		ui32SpdifChStatusBits[1] = ( hSpdifFm->sParams.sSpdifPackedChanStatusBits.ui32ChStatusBits[0] & 0xFFFF0000 ) >> 16;
		ui32SpdifChStatusBits[2] = hSpdifFm->sParams.sSpdifPackedChanStatusBits.ui32ChStatusBits[1] & 0xFFFF;
		
		ret = BRAP_SPDIFFM_P_ProgramCBITBuffer_isr(hSpdifFm, &(ui32SpdifChStatusBits[0] )); 

		if(ret != BERR_SUCCESS)
		{
			BDBG_ERR(("BRAP_SPDIFFM_P_ProgramChanStatusBits_isr(): ProgramCBITBuffer_isr returned err(%d)",ret));
			return BERR_TRACE(ret);
		}
	}
    else
#endif
    {
        BRAP_SPDIFFM_P_PrepareCBITData_isr(&ui32CBitData[0], 
									   hSpdifFm->sParams.sChanStatusParams,
									   eSamplingRate,
									   hSpdifFm->sParams.bCompressed);
        
        ret = BRAP_SPDIFFM_P_ProgramCBITBuffer_isr(hSpdifFm, &ui32CBitData[0]); 
        if(ret != BERR_SUCCESS)
        {
        	BDBG_ERR(("BRAP_SPDIFFM_P_ProgramChanStatusBits_isr: ProgramCBITBuffer returned err(%d)",ret));
        }
    }

    BDBG_ENTER(BRAP_SPDIFFM_P_ProgramChanStatusBits_isr);
    return ret;
}
#else
BERR_Code BRAP_SPDIFFM_P_ProgramChanStatusBits_isr(
    BRAP_SPDIFFM_P_Handle   hSpdifFm       /* [in] SPDIF Formatter handle */
) 
{
    BERR_Code ret = BERR_SUCCESS;
    uint32_t ui32CBitData[BRAP_SPDIFFM_CBIT_BUFFER_SIZE];
    
    BDBG_ENTER(BRAP_SPDIFFM_P_ProgramChanStatusBits_isr);

    BDBG_ASSERT(hSpdifFm);

    BRAP_SPDIFFM_P_PrepareCBITData_isr(&ui32CBitData[0], 
									   hSpdifFm->sParams.sChanStatusParams,
									   hSpdifFm->sParams.eSamplingRate,
									   hSpdifFm->sParams.bCompressed );


    ret = BRAP_SPDIFFM_P_ProgramCBITBuffer_isr(hSpdifFm, &ui32CBitData[0]); 
    if(ret != BERR_SUCCESS)
    {
    	BDBG_ERR(("BRAP_SPDIFFM_P_ProgramChanStatusBits_isr: ProgramCBITBuffer returned err(%d)",ret));
    }

    BDBG_ENTER(BRAP_SPDIFFM_P_ProgramChanStatusBits_isr);
    return ret;
}
#endif

#if 0 /* TBD later */
BERR_Code 
BRAP_SPDIFFM_P_Bypass ( 
    BRAP_SPDIFFM_P_Handle  hSpdifFm,    /* [in] SPDIF Formatter handle */
    bool            bBypass /* [in] TRUE: Bypass the SPDIF Formatter
                                          FALSE: Dont bypass */
)
{
    /* TODO: Program AUD_FMM_MS_CTRL_USEQ_BYPASS based upon 'bBypass' */
}



/* TODO: For playback only, so inform the DSP F/W about it */
/* TODO: Correctly implement this function after discussing with DSP people */


#endif /*TBD later */


/* End of File */
