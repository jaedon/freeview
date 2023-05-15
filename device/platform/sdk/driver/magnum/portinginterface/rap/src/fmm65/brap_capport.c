/***************************************************************************
*     Copyright (c) 2004-2012, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: brap_capport.c $
* $brcm_Revision: Hydra_Software_Devel/17 $
* $brcm_Date: 5/17/12 3:27p $
*
* Module Description:
*   Module name: CAPPORT
*   This file contains the implementation of all APIs for the Capture Port
*   abstraction.
*   
* Revision History:
*
* $brcm_Log: /magnum/portinginterface/rap/base3/brap_capport.c $
* 
* Hydra_Software_Devel/17   5/17/12 3:27p sgadara
* SW7435-174: [7125] Extending Destination Sample Rate info return
* support to Ringbuffer destinations
* 
* Hydra_Software_Devel/16   4/20/11 3:42p sgadara
* SW7420-1756: [7420] Implementing code review comments --> Change the
* Pointer validity check function to a macro and name it aptly
* 
* Hydra_Software_Devel/15   6/23/10 1:13p gautamk
* SW7405-3063: [7405] Changing WRN to MSG
* 
* Hydra_Software_Devel/14   12/17/09 9:23p sgadara
* SW3556-904: [3548,3556] Adding feature of "Fixed Ringbuffer Capture at
* 48khz sample rate"
* --> Fixing compilation error
* 
* Hydra_Software_Devel/13   12/17/09 9:15p sgadara
* SW3556-904: [3548,3556] Adding feature of "Fixed Ringbuffer Capture at
* 48khz sample rate"
* 
* Hydra_Software_Devel/12   12/15/09 11:43a gautamk
* SW7405-3063: [7405] Checking Malloc against NULL, Because 0xDEADDATA
* and 0xDATADEAD can be valid address.
* 
* Hydra_Software_Devel/11   11/24/09 4:37p gautamk
* SW7405-3063: [7405] Fixing internal capture for 7405.
* 
* Hydra_Software_Devel/10   11/18/09 2:24p gautamk
* SW7405-3063: [7405] Not enabling INS_NOACK, Because On 7405 platform,
* Enabling this leads to movement of DSTCH Fifo ptrs,
* even if data is not available. This leads initial zeros.
* 
* Hydra_Software_Devel/9   11/14/09 2:03p sgadara
* SW3556-832 : [3548,3556] Adding CIT-PI interface for the clock
* programming of loopback capture port(Ringbuffer Destination) by FW.
* 
* Hydra_Software_Devel/8   10/16/09 4:06p gautamk
* SW7405-3063: [7340] Adding support for Internal capture
* 
* Hydra_Software_Devel/7   6/12/09 3:44p gautamk
* PR53302: [7405] Merging cleanup code to mainline
* 
* Hydra_Software_Devel/6   6/9/09 12:33p gautamk
* PR55572:[7405] Before accessing any pointer location ccheck if it is
* 0XDA7ADEAD or 0xDEADDA7A in addition to NULL.
* 
* Hydra_Software_Devel/5   2/2/09 3:43p gautamk
* PR51618: [7405] Changing the global variables as per following rule.
* 1) make them static
* 2) if you cannot make them static because they must be shared between
* multiple .c files in your module, you must rename them to include the
* BXXX_ magnum prefix for your module.
* 
* Hydra_Software_Devel/4   10/20/08 2:26p gautamk
* PR47961: [7405] Fixing warning and compilation for 7405 platform due to
* changes of this PR.
* 
* Hydra_Software_Devel/3   10/18/08 2:46a speter
* PR 47388: [3548,3556] Fixing the problem of internal capture port
* requiring PLL0 for its operation
* 
* Hydra_Software_Devel/2   10/16/08 1:16a speter
* PR 46655: [3548,3556] Changing the ADC related code for B0
* 
* Hydra_Software_Devel/1   7/21/08 2:18p speter
* PR 44984: [3548,3556,7405] Adding files from base2 which have new
* Architecture specific changes and cannot be merged back in base2
* 
* Hydra_Software_Devel/Rap_NewFwArch/16   7/18/08 5:08p speter
* PR 38950: [3548,3556] Fixing the ADC part with unknown sampling rate
* 
* Hydra_Software_Devel/Rap_NewFwArch/15   7/18/08 11:15a speter
* PR 44780: [3548,3556] Support for HDMI In
* 
* Hydra_Software_Devel/Rap_NewFwArch/14   7/16/08 4:47p speter
* PR 43454: [7405] Fixing Compilation warnings in kernel mode
* 
* Hydra_Software_Devel/Rap_NewFwArch/13   7/9/08 10:12a speter
* PR 44660:[3548,3556] Merging the Phase 1.5 release branch code to main
* line of development
* 
* Hydra_Software_Devel/Rap_NewFwArch/12   6/30/08 3:38p sushmit
* PR 38783: [7405] Re-checking in I2S Capture changes for 7405 NEWFWARCH
* 
* Hydra_Software_Devel/Rap_NewFwArch/11   6/24/08 7:03a speter
* PR 38950: [3548,3556] Adding support for SPDIF In without DSP
* 
* Hydra_Software_Devel/Rap_NewFwArch/10   6/16/08 9:20p speter
* PR 42110: [3548,3556] Fixing the ADC code to take in AC1 instead of AC0
* in programming VCXO.
* 
* Hydra_Software_Devel/Rap_NewFwArch/9   6/14/08 7:34a speter
* PR 42110: [3548,3556] Adding ADC support
* 
* Hydra_Software_Devel/Rap_NewFwArch/8   6/11/08 7:33a speter
* PR38950: [3548,3556] Changing generic use of BTSC as RF Audio
* 
* Hydra_Software_Devel/Rap_NewFwArch/7   5/20/08 4:19p speter
* PR 42110: [3548,3556] ADC code update
* 
* Hydra_Software_Devel/Rap_NewFwArch/6   5/20/08 1:53p speter
* PR 42110: [3548,3556] Adding ADC support
* 
* Hydra_Software_Devel/Rap_NewFwArch/5   5/9/08 3:02p speter
* PR 38950: [3548,3556] Fixing the code for 3548 family
* 
* Hydra_Software_Devel/Rap_NewFwArch/4   5/5/08 11:25a speter
* PR 38950: [3548] Making 3548_FAMILY macro and using it for both 3548
* and 3556 cases.
* 
* Hydra_Software_Devel/Rap_NewFwArch/3   4/28/08 4:36p speter
* PR 38950: [3548] Fixing compilation for 3548/3556.
* 
* Hydra_Software_Devel/Rap_NewFwArch/2   4/22/08 2:16p gautamk
* PR41994: [7405] Using BCHP_VER_ Correctly...
* 
* Hydra_Software_Devel/Rap_NewFwArch/1   4/8/08 2:10p gautamk
* PR34648: [7405] Merging wdog code to mainline
* 
* Hydra_Software_Devel/Rap_NewFwArch/RAP_NEWFWARCH_WDOG/1   3/28/08 4:05p gautamk
* PR34648: [7405] adding code for watchdog recovery in a branch
* 
* Hydra_Software_Devel/20   9/30/07 1:54p sushmit
* PR35140: [7405] Develop 7325 Raptor PI for Bringup
* 
* Hydra_Software_Devel/19   8/2/07 11:05p kagrawal
* PR 33737: [7440] Ungrouping capport during stop
* 
* Hydra_Software_Devel/18   7/3/07 7:02p bmishra
* PR 23951: Merging the changes required for Autotrap
* 
* Hydra_Software_Devel/17   6/14/07 11:23a sushmit
* PR 27646: Removing redundunt preprocessor directive
* 
* Hydra_Software_Devel/16   6/12/07 10:02p sushmit
* PR 27646: Updating 7405 PI
* 
* Hydra_Software_Devel/15   6/12/07 9:13p sushmit
* PR 27646: Updating 7405 FMM PI
* 
* Hydra_Software_Devel/14   5/28/07 1:33p bhanus
* PR 30069 : Fixed Coverity Issues.
* 
* Hydra_Software_Devel/13   5/24/07 2:32p sushmit
* PR 27646: 7405 Compilation & linking
* 
* Hydra_Software_Devel/12   5/14/07 5:25p kagrawal
* PR 30069: [7440] Added following support:
* - BRAP_P_GetBaseSR(), BRAP_P_GetPLLForSR(), BRAP_P_IsInternalCapPort()
* - Added AM changes for allocating Fs Timing Source
* - Allocating internal capPort in OpenCapPath()
* - Removed internal capPort copy and remove hack from
* BRAP_P_StartFmmModuleHandles
* - Populating sInPathProp->uiPathIdx for SM and XCD
* 
* Hydra_Software_Devel/11   5/11/07 8:58p sushmit
* PR 27646: Getting 7405 PI compiled
* 
* Hydra_Software_Devel/10   4/4/07 5:02p kagrawal
* PR 28561: [7440, 3563] Modified capport module to support multiple
* instances of open and start by using open/start counts. Removed
* eState.
* 
* Hydra_Software_Devel/9   4/1/07 6:51a bhanus
* PR 28934 :[7440 B0] Added code for CAP Port Grouping.
* 
* Hydra_Software_Devel/8   3/15/07 2:11a bhanus
* PR 28040 : Modified for 7440 B0 Porting
* 
* Hydra_Software_Devel/7   2/24/07 2:10p bhanus
* PR 28040 : Modified code for 7440 B0 porting
* 
* Hydra_Software_Devel/6   2/9/07 6:19p bhanus
* PR 25607 : Fixed Compilation warnings
* 
* Hydra_Software_Devel/5   12/29/06 1:48a nitinb
* PR 26766: Fixed compilation errors
* 
* Hydra_Software_Devel/4   12/29/06 1:08a nitinb
* PR 26766: 1. (3563-specific) If capport is BTSC, starting BTSC decoder
*   from function BRAP_CAPPORT_P_HWConfig and enabling BTSC cap stream
*   in function BRAP_CAPPORT_P_Start
* 2. (3563-specific) If capport is BTSC, stopping BTSC decoder and
*   disabling BTSC capture stream in function BRAP_CAPPORT_P_Stop
* 
* Hydra_Software_Devel/3   12/7/06 1:27p sushmit
* PR 25346: Merging 3563 to Hydra
* 
* Hydra_Software_Devel/RAP_3563_Merge/1   12/4/06 1:56p sushmit
* PR 24073: Fixing initialization warnings.
* 
* Hydra_Software_Devel/2   10/6/06 7:55p kagrawal
* PR 24717: Merged latest from the 7440 devel branch
* 
* Hydra_Software_Devel/7440_Devel_Branch/2   9/20/06 4:28p bhanus
* PR 20654: Removed BRAP_P_State_eStopped
* 
* Hydra_Software_Devel/7440_Devel_Branch/1   8/31/06 4:14p bhanus
* PR 20654 : Fixed Compilation errors.
* 
* Hydra_Software_Devel/1   8/28/06 4:34p speter
* PR 20654: Adding capture related files
***************************************************************************/
#include "brap.h"
#include "brap_priv.h"

BDBG_MODULE (rap_capport);

static const BRAP_CAPPORT_P_Params defCapPortParam = 
{
    BRAP_INVALID_VALUE,             /* uiInputFciId */
    BRAP_InputBitsPerSample_e16,    /* eInputBitsPerSample */
    BRAP_INVALID_VALUE,             /* uiGrpId */
    BRAP_RM_P_INVALID_INDEX,        /* uiFsTmgSrc */
    BRAP_OP_Pll_eMax,               /* ePll */
    BAVC_AudioSamplingRate_eUnknown /* eSamplingRate */
};

/***************************************************************************
Summary:
    This function programs the Clock to drive the Capture Port.

Description:
    The Sample Rate of the loopback capture(Internal Capture port) is set by 
    programming the PLL registers.
    
Returns:
    BERR_SUCCESS else error
    
See Also:
    BRAP_OP_P_ProgramOutputClock

**************************************************************************/
static BERR_Code 
BRAP_CAPPORT_P_ProgramCaptureClock(
    BRAP_CAPPORT_P_Handle   hCapPort,      /* [in] Capture Port Handle */
    unsigned int            uiFsTmgSrcId,   /* [in] FS Timing Source Id */
    BRAP_OP_Pll 			ePll,           /* [in] PLL to be associated */
    BAVC_AudioSamplingRate 	eSamplingRate   /* [in] Sampling rate of the data at 
                                               CapPort or FCI sinker driven by 
                                               this FS timing source */
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
)
{
    BERR_Code  ret = BERR_SUCCESS;

    BDBG_ENTER (BRAP_CAPPORT_P_GetDefaultParams);
    BDBG_ASSERT (pDefParams);

    *pDefParams = defCapPortParam;

    BDBG_LEAVE(BRAP_CAPPORT_P_GetDefaultParams);
    return ret;    
}

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
)
{
	BERR_Code               ret = BERR_SUCCESS;
    BRAP_CAPPORT_P_Handle   hCapPort = NULL;

    BDBG_ENTER (BRAP_CAPPORT_P_Open);
    
    BDBG_MSG (("BRAP_CAPPORT_P_Open:hFmm=0x%x, eCapPort=%d,No settings",
                                                               hFmm, eCapPort));

    /* Check all input parameters to the function. Return error if
     * - FMM handle is NULL
     * - Given index exceeds maximum no. of Capture Port
     * - Pointer to Settings structure is NULL
     * - Pointer to Capture Port handle is NULL     */
     
    BDBG_ASSERT (hFmm);
    BDBG_ASSERT (phCapPort);
    
    if (BRAP_FMM_P_GetWatchdogRecoveryFlag (hFmm) == false)
    {   /* If not in WatchDog recovery */    
        BDBG_ASSERT (pSettings);
    }
    BSTD_UNUSED(pSettings);
    if (eCapPort >= BRAP_CapInputPort_eMax)
    {
        return BERR_TRACE (BERR_NOT_SUPPORTED);
    }       

    if (BRAP_FMM_P_GetWatchdogRecoveryFlag (hFmm) == false)
    {   
        /* If not in WatchDog recovery */  
        if(BRAP_P_IsPointerValid((void *)hFmm->hCapPort[eCapPort]))
        {
            /* This capport was previoulsy opened by another audio channel.
            So this time, just increment the open count. Dont change the 
            settings etc. */
            hCapPort = hFmm->hCapPort[eCapPort];
     
            *phCapPort = hCapPort;
            hCapPort->uiOpenCnt++;
            BDBG_MSG (("BRAP_CAPPORT_P_Open: Capture port %d was already open." 
                        "New open count = %d", eCapPort, hCapPort->uiOpenCnt));
            BDBG_LEAVE (BRAP_CAPPORT_P_Open);
            return ret;
        }
    
        /* Allocate memory for the Capture Port handle. Fill in params 
           in the Capture Port handle. */
    
        /* Allocate Capture port handle */
        hCapPort = (BRAP_CAPPORT_P_Handle) BKNI_Malloc (sizeof(BRAP_CAPPORT_P_Object));
        if ( NULL==hCapPort)
        {
            return BERR_TRACE (BERR_OUT_OF_SYSTEM_MEMORY);
        }
        
        /* Clear the handle memory */
        BKNI_Memset (hCapPort, 0, sizeof(BRAP_CAPPORT_P_Object));

        /* Initialise known elements in Capture Port handle */
        hCapPort->hChip = hFmm->hChip;
        hCapPort->hRegister = hFmm->hRegister;
        hCapPort->hHeap = hFmm->hHeap;
        hCapPort->hInt = hFmm->hInt;
        hCapPort->hFmm = hFmm;
        hCapPort->eCapPort = eCapPort;
        hCapPort->uiOpenCnt = 1;
        hCapPort->uiStartCnt = 0;        
    }
    else
    {
        hCapPort = *phCapPort;
    }

    if (BRAP_FMM_P_GetWatchdogRecoveryFlag(hFmm) == false)
    {   /* If not in WatchDog recovery */ 

        /* Store Capture Port handle inside the FMM handle */
        hFmm->hCapPort[eCapPort] = hCapPort;
    
        *phCapPort = hCapPort;
    }   /* End: If not in WatchDog recovery */        
    
    BDBG_MSG(("BRAP_CAPPORT_P_Open: handle=0x%x", hCapPort)); 
    BDBG_LEAVE (BRAP_CAPPORT_P_Open);
    return ret;
}

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
)
{
    BERR_Code ret = BERR_SUCCESS;

    BDBG_ENTER (BRAP_CAPPORT_P_Close);
    BDBG_ASSERT (hCapPort);
    
    BDBG_MSG (("BRAP_CAPPORT_P_Close(): hCapPort=0x%x, eCapPort=%d ", 
        hCapPort, hCapPort->eCapPort));

    if(0 == hCapPort->uiOpenCnt)
    { 
        /* This should never happen. If it does, it means the system
           has gone into an invalid state!!!!*/
        BDBG_ERR (("BRAP_CAPPORT_P_Open: Capture port %d has NO open"
            " audio channels!!!", hCapPort->eCapPort));
        return BERR_TRACE (BERR_NOT_SUPPORTED);
    }
    hCapPort->uiOpenCnt--;

    if(0 == hCapPort->uiOpenCnt)
    {   
        /* If this was the last open audio channel on this capture port,
           free the handle etc */
        /* Remove referrence to this Capture Port from the parent FMM */ 
        hCapPort->hFmm->hCapPort[hCapPort->eCapPort] = NULL;

        /* Free the Capture Port Handle memory*/
        BKNI_Free (hCapPort); 
    }             

    BDBG_LEAVE (BRAP_CAPPORT_P_Close);
    return ret;
}

#if ((BRAP_3548_FAMILY == 1))
/***************************************************************************
Summary:
    Configures the HW registers for the Capture Port

Description:

Returns:
    BERR_SUCCESS on success
    Error code on failure
See Also:
    BRAP_CAPPORT_P_Start
**************************************************************************/
static BERR_Code BRAP_CAPPORT_P_HWConfig ( 
    BRAP_CAPPORT_P_Handle   hCapPort      /* [in] Capture Port Handle */)
{
    BERR_Code           ret = BERR_SUCCESS;
    uint32_t            ui32RegVal = 0;
    BREG_Handle         hRegister = NULL;
    BRAP_CAPPORT_P_Params sParams;

#if(BRAP_48KHZ_RINGBUFFER_DESTINATION == 1) 
    uint32_t            ui32FsOffset = 0;
#endif

    BDBG_ENTER (BRAP_CAPPORT_P_HWConfig);
    BDBG_ASSERT (hCapPort);

    BDBG_MSG (("BRAP_CAPPORT_P_HWConfig(): hCapPort=0x%x, Index=%d \n", 
                                hCapPort, hCapPort->eCapPort));
  
    hRegister = hCapPort->hRegister;
    sParams = hCapPort->sParams;    
    
    switch(hCapPort->eCapPort)
    {
#if ((BRAP_3548_FAMILY == 1) )
        case BRAP_CapInputPort_eSpdif:
        case BRAP_CapInputPort_eHdmi:            
            ret = BRAP_SPDIFRX_P_Start ( hCapPort->hFmm->hRap );
            if( BERR_SUCCESS!=ret )
                return BERR_TRACE( ret );
            break;

        case BRAP_CapInputPort_eRfAudio:
            ret = BRAP_RFAUDIO_P_Start( hCapPort );
            if( BERR_SUCCESS!=ret )
                return BERR_TRACE( ret );
            break;

        case BRAP_CapInputPort_eAdc:
            /* Program the PLL1 which is specific to ADC */
            ui32RegVal = BRAP_Read32(hRegister,BCHP_VCXO_CTL_MISC_AC1_MACRO);
            ui32RegVal &= ~(BCHP_MASK (VCXO_CTL_MISC_AC1_MACRO, MACRO_SELECT));
            switch (sParams.eSamplingRate)
            {
                case BAVC_AudioSamplingRate_e32k:
                case BAVC_AudioSamplingRate_e64k:
                case BAVC_AudioSamplingRate_e128k:
                    /* All 32kHz family of sampling rates are achieved using 32kHz 
                       as the base sampling rate. Using PLL channel select 1x, 2x or 3x 
                       to achieve 32kHz, 64kHz or 128kHz */
                    ui32RegVal |= (BCHP_FIELD_ENUM (VCXO_CTL_MISC_AC1_MACRO, MACRO_SELECT, Mult_of_32000));
                    break;
                    
                case BAVC_AudioSamplingRate_e44_1k:
                case BAVC_AudioSamplingRate_e88_2k:
                case BAVC_AudioSamplingRate_e176_4k:
                    /* All 44.1kHz family of sampling rates are achieved using 44.1kHz 
                       as the base sampling rate. Using PLL channel select 1x, 2x or 3x 
                       to achieve 44.1kHz, 88.2kHz or 176.4kHz */
                    ui32RegVal |= (BCHP_FIELD_ENUM (VCXO_CTL_MISC_AC1_MACRO, MACRO_SELECT, Mult_of_44100));
                    break;
                    
                case BAVC_AudioSamplingRate_e48k:
                case BAVC_AudioSamplingRate_e96k:
                case BAVC_AudioSamplingRate_e192k:                    
                    /* All 48kHz family of sampling rates are achieved using 48kHz as 
                       the base sampling rate. Using PLL channel select 1x, 2x or 3x to 
                       achieve 48kHz*/
                    ui32RegVal |= (BCHP_FIELD_ENUM (VCXO_CTL_MISC_AC1_MACRO, MACRO_SELECT, Mult_of_48000));
                    break;
                    
                default:
                    BDBG_MSG (("Incorrect Sampling Rate %d supplied. Forcing it to 48kHz", sParams.eSamplingRate));
                    sParams.eSamplingRate = BAVC_AudioSamplingRate_e48k;
                    ui32RegVal |= (BCHP_FIELD_ENUM (VCXO_CTL_MISC_AC1_MACRO, MACRO_SELECT, Mult_of_48000));
            }
            BRAP_Write32(hRegister,BCHP_VCXO_CTL_MISC_AC1_MACRO, ui32RegVal);

            ui32RegVal = BRAP_Read32(hRegister,BCHP_VCXO_CTL_MISC_AC1_CONTROL);
            ui32RegVal &= ~(BCHP_MASK (VCXO_CTL_MISC_AC1_CONTROL, REFERENCE_SELECT));
            ui32RegVal |= (BCHP_FIELD_ENUM (VCXO_CTL_MISC_AC1_CONTROL, REFERENCE_SELECT, VCXO_1));
            BRAP_Write32(hRegister,BCHP_VCXO_CTL_MISC_AC1_CONTROL, ui32RegVal);            
            
            /* ADCCIC needs to be put in Normal Operation */
            ui32RegVal = BRAP_Read32 (hRegister, BCHP_ADCCIC_CTRL_RESET);
            ui32RegVal &= ~( (BCHP_MASK (ADCCIC_CTRL_RESET, SYNC_RESET_FIFO)) |
                             (BCHP_MASK (ADCCIC_CTRL_RESET, SYNC_RESET))); 
            ui32RegVal |= (BCHP_FIELD_ENUM (ADCCIC_CTRL_RESET, SYNC_RESET_FIFO, Normal_operation));
            ui32RegVal |= (BCHP_FIELD_ENUM (ADCCIC_CTRL_RESET, SYNC_RESET, Normal_operation));            
            BRAP_Write32 (hRegister, BCHP_ADCCIC_CTRL_RESET, ui32RegVal);            
            
            ui32RegVal = BRAP_Read32(hRegister,BCHP_ADCCIC_CTRL_ADC_MUX_SEL);
            ui32RegVal &= ~(BCHP_MASK (ADCCIC_CTRL_ADC_MUX_SEL, MUX_SEL)); 

            switch (hCapPort->hFmm->hRap->sInputSettings[hCapPort->eCapPort].sCapPortParams.uCapPortParams.sAdcParams.uiInputSelect)
            {
                case 0:
                    ui32RegVal |= (BCHP_FIELD_ENUM(ADCCIC_CTRL_ADC_MUX_SEL, MUX_SEL, Channel_1));        
                    break;

                case 1:
                    ui32RegVal |= (BCHP_FIELD_ENUM(ADCCIC_CTRL_ADC_MUX_SEL, MUX_SEL, Channel_2));        
                    break;

                case 2:
                    ui32RegVal |= (BCHP_FIELD_ENUM(ADCCIC_CTRL_ADC_MUX_SEL, MUX_SEL, Channel_3));        
                    break;

                case 3:
                    ui32RegVal |= (BCHP_FIELD_ENUM(ADCCIC_CTRL_ADC_MUX_SEL, MUX_SEL, Channel_4));        
                    break;

                case 4:
                    ui32RegVal |= (BCHP_FIELD_ENUM(ADCCIC_CTRL_ADC_MUX_SEL, MUX_SEL, Channel_5));        
                    break;

                case 5:
                    ui32RegVal |= (BCHP_FIELD_ENUM(ADCCIC_CTRL_ADC_MUX_SEL, MUX_SEL, Channel_6));        
                    break;

                case 6:
                    ui32RegVal |= (BCHP_FIELD_ENUM(ADCCIC_CTRL_ADC_MUX_SEL, MUX_SEL, Channel5));        
                    break;

                case 7:
                    ui32RegVal |= (BCHP_FIELD_ENUM(ADCCIC_CTRL_ADC_MUX_SEL, MUX_SEL, Channel6));        
                    break;

                default:
					BDBG_ERR (("BRAP_CAPPORT_P_HWConfig: "
							   "ADC uiInputSelect %d is not supported", 
							   hCapPort->hFmm->hRap->sInputSettings[hCapPort->eCapPort].sCapPortParams.uCapPortParams.sAdcParams.uiInputSelect));
					return BERR_TRACE (BERR_NOT_SUPPORTED);   
            }
            BRAP_Write32(hRegister,BCHP_ADCCIC_CTRL_ADC_MUX_SEL, ui32RegVal);

            /* Start the ADC */
            ui32RegVal = BRAP_Read32(hRegister,BCHP_ADCCIC_CTRL_CONFIG);
            ui32RegVal &= ~(BCHP_MASK (ADCCIC_CTRL_CONFIG, 	ADC_CIC_OUTFIFO_ENA)); 
            ui32RegVal |= (BCHP_FIELD_ENUM(ADCCIC_CTRL_CONFIG, ADC_CIC_OUTFIFO_ENA, enable));
            BRAP_Write32(hRegister,BCHP_ADCCIC_CTRL_CONFIG,ui32RegVal);            
            break;
            
#endif
        case BRAP_CapInputPort_eExtI2s0:
            ui32RegVal = BRAP_Read32(hRegister,BCHP_AUD_FMM_IOP_CTRL_I2SIN_CFG0);

            ui32RegVal &= ~( (BCHP_MASK (AUD_FMM_IOP_CTRL_I2SIN_CFG0, BITS_PER_SAMPLE)) 
                     | (BCHP_MASK (AUD_FMM_IOP_CTRL_I2SIN_CFG0, DATA_JUSTIFICATION)) 
                     | (BCHP_MASK (AUD_FMM_IOP_CTRL_I2SIN_CFG0, DATA_ALIGNMENT)) 
                     | (BCHP_MASK (AUD_FMM_IOP_CTRL_I2SIN_CFG0, LRCK_POLARITY)) 
                     | (BCHP_MASK (AUD_FMM_IOP_CTRL_I2SIN_CFG0, SCLK_POLARITY)));

            switch(sParams.eInputBitsPerSample)
			{
				case BRAP_InputBitsPerSample_e16:
					ui32RegVal |= (BCHP_FIELD_ENUM(AUD_FMM_IOP_CTRL_I2SIN_CFG0, 
											BITS_PER_SAMPLE, Bitwidth16));
					break;
				case BRAP_InputBitsPerSample_e18:
					ui32RegVal |= (BCHP_FIELD_ENUM(AUD_FMM_IOP_CTRL_I2SIN_CFG0, 
											BITS_PER_SAMPLE, Bitwidth18));
					break;
				case BRAP_InputBitsPerSample_e20:
					ui32RegVal |= (BCHP_FIELD_ENUM(AUD_FMM_IOP_CTRL_I2SIN_CFG0, 
											BITS_PER_SAMPLE, Bitwidth20));
					break;
				case BRAP_InputBitsPerSample_e24:
					ui32RegVal |= (BCHP_FIELD_ENUM(AUD_FMM_IOP_CTRL_I2SIN_CFG0, 
											BITS_PER_SAMPLE, Bitwidth24));
					break;
				case BRAP_InputBitsPerSample_e32:
					ui32RegVal |= (BCHP_FIELD_ENUM(AUD_FMM_IOP_CTRL_I2SIN_CFG0, 
											BITS_PER_SAMPLE, Bitwidth32));
					break;
				default:
					BDBG_ERR (("BRAP_CAPPORT_P_HWConfig: "
							   "I2S Input Bits/Sample %d is not supported", 
							   sParams.eInputBitsPerSample));
					return BERR_TRACE (BERR_NOT_SUPPORTED);
					break;
			}

			/* Data Justification */
			if (hCapPort->hFmm->hRap->sInputSettings[hCapPort->eCapPort].sCapPortParams.uCapPortParams.sInputI2sParams.bLsbAtLRClk == true)
			{
				ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_IOP_CTRL_I2SIN_CFG0, 
												DATA_JUSTIFICATION, LSB));
			}
			else 
			{
				ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_IOP_CTRL_I2SIN_CFG0, 
												DATA_JUSTIFICATION, MSB));
			}

			/* Data Alignment */
			if (hCapPort->hFmm->hRap->sInputSettings[hCapPort->eCapPort].sCapPortParams.uCapPortParams.sInputI2sParams.bAlignedToLRClk == false)
			{
				ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_IOP_CTRL_I2SIN_CFG0, 
												DATA_ALIGNMENT, Delayed));
			}
			else 
			{
				ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_IOP_CTRL_I2SIN_CFG0, 
												DATA_ALIGNMENT, Aligned));
			}

			/* S Clock Polarity to set to Falling_aligned_with_sdata */
			ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_IOP_CTRL_I2SIN_CFG0, 
							SCLK_POLARITY, Falling_aligned_with_sdata));
			
			/* LR Clock Polarity */
			if (hCapPort->hFmm->hRap->sInputSettings[hCapPort->eCapPort].sCapPortParams.uCapPortParams.sInputI2sParams.bLRClkPolarity == true)
			{
				ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_IOP_CTRL_I2SIN_CFG0, 
												LRCK_POLARITY, High_for_left));
			}
			else 
			{
				ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_IOP_CTRL_I2SIN_CFG0, 
												LRCK_POLARITY, Low_for_left));
			}

            BRAP_Write32(hRegister,BCHP_AUD_FMM_IOP_CTRL_I2SIN_CFG0,ui32RegVal);

            break;
            
        case BRAP_CapInputPort_eIntCapPort0:
            ui32RegVal = BRAP_Read32(hRegister,BCHP_AUD_FMM_IOP_CTRL_LOOPBACK_CFG);

            ui32RegVal &= ~( (BCHP_MASK (AUD_FMM_IOP_CTRL_LOOPBACK_CFG, LOOPBACK0_INS)) 
                     | (BCHP_MASK (AUD_FMM_IOP_CTRL_LOOPBACK_CFG, TMG_SRC_SEL0)) 
                     | (BCHP_MASK (AUD_FMM_IOP_CTRL_LOOPBACK_CFG, IGNORE_FIRST_UNDERFLOW0)));
            /* Program LOOPBACK0_INS, TMG_SRC_SEL0, IGNORE_FIRST_UNDERFLOW0 */

            BRAP_Write32(hRegister,BCHP_AUD_FMM_IOP_CTRL_LOOPBACK_CFG,ui32RegVal);

#if (BRAP_3548_FAMILY == 1)
            /* Program the Input FCI Id */
            ui32RegVal = BRAP_Read32(hRegister,BCHP_AUD_FMM_IOP_CTRL_MIX_IOP_IDMAP98);
            ui32RegVal &= ~ BCHP_MASK(AUD_FMM_IOP_CTRL_MIX_IOP_IDMAP98, STREAM9);
            ui32RegVal |= BCHP_FIELD_DATA(AUD_FMM_IOP_CTRL_MIX_IOP_IDMAP98, STREAM9, sParams.uiInputFciId);
            BRAP_Write32(hRegister,BCHP_AUD_FMM_IOP_CTRL_MIX_IOP_IDMAP98, ui32RegVal);

#else
            /* Program the Input FCI Id */
            ui32RegVal = BRAP_Read32(hRegister,BCHP_AUD_FMM_IOP_CTRL_MIX_IOP_IDMAP98);
            ui32RegVal &= ~ BCHP_MASK(AUD_FMM_IOP_CTRL_MIX_IOP_IDMAP98, STREAM8);
            ui32RegVal |= BCHP_FIELD_DATA(AUD_FMM_IOP_CTRL_MIX_IOP_IDMAP98, STREAM8, sParams.uiInputFciId);
            BRAP_Write32(hRegister,BCHP_AUD_FMM_IOP_CTRL_MIX_IOP_IDMAP98, ui32RegVal);            
#endif            
            break;
        case BRAP_CapInputPort_eIntCapPort1:
            ui32RegVal = BRAP_Read32(hRegister,BCHP_AUD_FMM_IOP_CTRL_LOOPBACK_CFG);

            ui32RegVal &= ~( (BCHP_MASK (AUD_FMM_IOP_CTRL_LOOPBACK_CFG, LOOPBACK1_INS)) 
                     | (BCHP_MASK (AUD_FMM_IOP_CTRL_LOOPBACK_CFG, TMG_SRC_SEL1)) 
                     | (BCHP_MASK (AUD_FMM_IOP_CTRL_LOOPBACK_CFG, IGNORE_FIRST_UNDERFLOW1)));
            /* Program LOOPBACK1_INS, TMG_SRC_SEL1, IGNORE_FIRST_UNDERFLOW1 */

            BRAP_Write32(hRegister,BCHP_AUD_FMM_IOP_CTRL_LOOPBACK_CFG,ui32RegVal);

#if (BRAP_3548_FAMILY == 1)
            /* Program the Input FCI Id */
            ui32RegVal = BRAP_Read32(hRegister,BCHP_AUD_FMM_IOP_CTRL_MIX_IOP_IDMAPba);
            ui32RegVal &= ~ BCHP_MASK(AUD_FMM_IOP_CTRL_MIX_IOP_IDMAPba, STREAM10);
            ui32RegVal |= BCHP_FIELD_DATA(AUD_FMM_IOP_CTRL_MIX_IOP_IDMAPba, STREAM10, sParams.uiInputFciId);
            BRAP_Write32(hRegister,BCHP_AUD_FMM_IOP_CTRL_MIX_IOP_IDMAPba, ui32RegVal);
#else
            /* Program the Input FCI Id */
            ui32RegVal = BRAP_Read32(hRegister,BCHP_AUD_FMM_IOP_CTRL_MIX_IOP_IDMAP98);
            ui32RegVal &= ~ BCHP_MASK(AUD_FMM_IOP_CTRL_MIX_IOP_IDMAP98, STREAM9);
            ui32RegVal |= BCHP_FIELD_DATA(AUD_FMM_IOP_CTRL_MIX_IOP_IDMAP98, STREAM9, sParams.uiInputFciId);
            BRAP_Write32(hRegister,BCHP_AUD_FMM_IOP_CTRL_MIX_IOP_IDMAP98, ui32RegVal);
#endif            
            break;
        case BRAP_CapInputPort_eIntCapPort2:
            ui32RegVal = BRAP_Read32(hRegister,BCHP_AUD_FMM_IOP_CTRL_LOOPBACK_CFG);

            ui32RegVal &= ~( (BCHP_MASK (AUD_FMM_IOP_CTRL_LOOPBACK_CFG, LOOPBACK2_INS)) 
                     | (BCHP_MASK (AUD_FMM_IOP_CTRL_LOOPBACK_CFG, TMG_SRC_SEL2)) 
                     | (BCHP_MASK (AUD_FMM_IOP_CTRL_LOOPBACK_CFG, IGNORE_FIRST_UNDERFLOW2)));
            /* Program LOOPBACK2_INS, TMG_SRC_SEL2, IGNORE_FIRST_UNDERFLOW2 */

            BRAP_Write32(hRegister,BCHP_AUD_FMM_IOP_CTRL_LOOPBACK_CFG,ui32RegVal);

#if (BRAP_3548_FAMILY == 1)
            /* Program the Input FCI Id */
            ui32RegVal = BRAP_Read32(hRegister,BCHP_AUD_FMM_IOP_CTRL_MIX_IOP_IDMAPba);
            ui32RegVal &= ~ BCHP_MASK(AUD_FMM_IOP_CTRL_MIX_IOP_IDMAPba, STREAM11);
            ui32RegVal |= BCHP_FIELD_DATA(AUD_FMM_IOP_CTRL_MIX_IOP_IDMAPba, STREAM11, sParams.uiInputFciId);
            BRAP_Write32(hRegister,BCHP_AUD_FMM_IOP_CTRL_MIX_IOP_IDMAPba, ui32RegVal);
#else
            /* Program the Input FCI Id */
            ui32RegVal = BRAP_Read32(hRegister,BCHP_AUD_FMM_IOP_CTRL_MIX_IOP_IDMAPba);
            ui32RegVal &= ~ BCHP_MASK(AUD_FMM_IOP_CTRL_MIX_IOP_IDMAPba, STREAM10);
            ui32RegVal |= BCHP_FIELD_DATA(AUD_FMM_IOP_CTRL_MIX_IOP_IDMAPba, STREAM10, sParams.uiInputFciId);
            BRAP_Write32(hRegister,BCHP_AUD_FMM_IOP_CTRL_MIX_IOP_IDMAPba, ui32RegVal);
#endif            
            
            break;
        case BRAP_CapInputPort_eIntCapPort3:
            ui32RegVal = BRAP_Read32(hRegister,BCHP_AUD_FMM_IOP_CTRL_LOOPBACK_CFG);

            ui32RegVal &= ~( (BCHP_MASK (AUD_FMM_IOP_CTRL_LOOPBACK_CFG, LOOPBACK3_INS)) 
                     | (BCHP_MASK (AUD_FMM_IOP_CTRL_LOOPBACK_CFG, TMG_SRC_SEL3)) 
                     | (BCHP_MASK (AUD_FMM_IOP_CTRL_LOOPBACK_CFG, IGNORE_FIRST_UNDERFLOW3)));
            /* Program LOOPBACK3_INS, TMG_SRC_SEL3, IGNORE_FIRST_UNDERFLOW3 */

            BRAP_Write32(hRegister,BCHP_AUD_FMM_IOP_CTRL_LOOPBACK_CFG,ui32RegVal);

#if (BRAP_3548_FAMILY == 1)
            /* Program the Input FCI Id */
            ui32RegVal = BRAP_Read32(hRegister,BCHP_AUD_FMM_IOP_CTRL_MIX_IOP_IDMAPdc);
            ui32RegVal &= ~ BCHP_MASK(AUD_FMM_IOP_CTRL_MIX_IOP_IDMAPdc, STREAM12);
            ui32RegVal |= BCHP_FIELD_DATA(AUD_FMM_IOP_CTRL_MIX_IOP_IDMAPdc, STREAM12, sParams.uiInputFciId);
            BRAP_Write32(hRegister,BCHP_AUD_FMM_IOP_CTRL_MIX_IOP_IDMAPdc, ui32RegVal);
#else
            /* Program the Input FCI Id */
            ui32RegVal = BRAP_Read32(hRegister,BCHP_AUD_FMM_IOP_CTRL_MIX_IOP_IDMAPba);
            ui32RegVal &= ~ BCHP_MASK(AUD_FMM_IOP_CTRL_MIX_IOP_IDMAPba, STREAM11);
            ui32RegVal |= BCHP_FIELD_DATA(AUD_FMM_IOP_CTRL_MIX_IOP_IDMAPba, STREAM11, sParams.uiInputFciId);
            BRAP_Write32(hRegister,BCHP_AUD_FMM_IOP_CTRL_MIX_IOP_IDMAPba, ui32RegVal);
#endif            
            
            break;
        default:
            BDBG_ASSERT (0);
    }

    /* Clock programming for Internal Capture ports */
    switch(hCapPort->eCapPort)
    {            
        case BRAP_CapInputPort_eIntCapPort0:
        case BRAP_CapInputPort_eIntCapPort1:
        case BRAP_CapInputPort_eIntCapPort2:
        case BRAP_CapInputPort_eIntCapPort3:

#if (BRAP_3548_FAMILY == 1)
            /* Internal capture ports require VCXO_0 to be programmed in PLL0 */
            ui32RegVal = BRAP_Read32(hRegister,BCHP_VCXO_CTL_MISC_AC0_CONTROL);
            ui32RegVal &= ~(BCHP_MASK (VCXO_CTL_MISC_AC0_CONTROL, REFERENCE_SELECT));
            ui32RegVal |= (BCHP_FIELD_ENUM (VCXO_CTL_MISC_AC0_CONTROL, REFERENCE_SELECT, VCXO_0));
            BRAP_Write32(hRegister,BCHP_VCXO_CTL_MISC_AC0_CONTROL, ui32RegVal);  

#if(BRAP_48KHZ_RINGBUFFER_DESTINATION == 1) 
            /* If this macro==0 clk programming is done by FW */
            ui32FsOffset = 4 * hCapPort->sParams.uiFsTmgSrc;

            ui32RegVal = 0;
            ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_OP_CTRL_MCLK_CFG_FSi, MCLK_RATE, MCLK_is_256fs);            
            ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_OP_CTRL_MCLK_CFG_FSi, PLLCLKSEL, Mclk_gen0);
            BRAP_Write32 (hRegister, BCHP_AUD_FMM_OP_CTRL_MCLK_CFG_FSi_ARRAY_BASE + ui32FsOffset, 
                          ui32RegVal);            

            ui32RegVal = 0;
            ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_OP_CTRL_MCLK_GEN_CFGi, TB_SEL, Timebase_0);            
            ui32RegVal |= BCHP_FIELD_DATA(AUD_FMM_OP_CTRL_MCLK_GEN_CFGi, NCO_DELTA, 0x200);            
            ui32RegVal |= BCHP_FIELD_DATA(AUD_FMM_OP_CTRL_MCLK_GEN_CFGi, NCO_MODULUS, 0x465);
            BRAP_Write32 (hRegister, BCHP_AUD_FMM_OP_CTRL_MCLK_GEN_CFGi_ARRAY_BASE + ui32FsOffset, 
                          ui32RegVal);    
#endif

#endif
            break;
        default:
            /* Do nothing */
            break;
    }/* switch eCapPort */      


    BDBG_LEAVE (BRAP_CAPPORT_P_HWConfig);
    return ret;
}

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
)
{
    BERR_Code       ret = BERR_SUCCESS;
    uint32_t        ui32RegVal = 0;
    BREG_Handle     hRegister = NULL;

    BDBG_ENTER (BRAP_CAPPORT_P_Start);
    BDBG_ASSERT (hCapPort);

    if (BRAP_FMM_P_GetWatchdogRecoveryFlag (hCapPort->hFmm) == false)
    {   
        /* If not in WatchDog recovery */  
        BDBG_ASSERT (pParams);

        if (hCapPort->uiStartCnt > 0)
        {
            /* This Outport was previoulsy started by another audio channel.
               So this time, just increment the start count. Dont change the 
               settings etc. */
            /* Also, start count cannot exceed the open count!! */
            if (hCapPort->uiStartCnt >= hCapPort->uiOpenCnt)
            {
                BDBG_ERR (("BRAP_CAPPORT_P_Start: Cannot start capport %d. "
                        "Start Count (%d) >= Open Count (%d)",
                        hCapPort->eCapPort, hCapPort->uiStartCnt, hCapPort->uiOpenCnt));
                return BERR_TRACE (BERR_NOT_SUPPORTED);
            }
        
            hCapPort->uiStartCnt ++;
            BDBG_MSG (("BRAP_CAPPORT_P_Start: CapPort %d was already started." 
                        "New start count = %d", 
                        hCapPort->eCapPort, hCapPort->uiStartCnt));

            BDBG_LEAVE (BRAP_CAPPORT_P_Start);
            return ret;
        }
    
        /* Store the start parameters inside the handle */
        hCapPort->sParams = *pParams;

        BDBG_MSG (("BRAP_CAPPORT_P_Start(): hCapPort=0x%x, Type=%d, \n"
				"hCapPort->sParams.sExtParams.bLsbAtLRClk=%d\n"
				"hCapPort->sParams.sExtParams.bAlignedToLRClk=%d\n"
                "hCapPort->sParams.sExtParams.bLRClkPolarity=%d\n"
                "hCapPort->sParams.uiInputFciId =%d\n"
                "hCapPort->sParams.eInputBitsPerSample=%d\n",
                hCapPort, hCapPort->eCapPort,
				hCapPort->hFmm->hRap->sInputSettings[hCapPort->eCapPort].sCapPortParams.uCapPortParams.sInputI2sParams.bLsbAtLRClk, 
				hCapPort->hFmm->hRap->sInputSettings[hCapPort->eCapPort].sCapPortParams.uCapPortParams.sInputI2sParams.bAlignedToLRClk, 
				hCapPort->hFmm->hRap->sInputSettings[hCapPort->eCapPort].sCapPortParams.uCapPortParams.sInputI2sParams.bLRClkPolarity, 
				hCapPort->sParams.uiInputFciId, 				
				hCapPort->sParams.eInputBitsPerSample)); 
        

        BDBG_MSG (("hCapPort->sParams.uiGrpId = %d",hCapPort->sParams.uiGrpId));

#if ((BRAP_7405_FAMILY == 1))
        BDBG_MSG (("hCapPort->sParams.uiFsTmgSrc = %d \n"
                   "hCapPort->sParams.ePll = %d\n"
                   "hCapPort->sParams.eSamplingRate = %d \n",
                   hCapPort->sParams.uiFsTmgSrc, 
                   hCapPort->sParams.ePll,
                   hCapPort->sParams.eSamplingRate));
#endif /* 7405 */        
    
    }   /* End: If not in WatchDog recovery */
   
    hRegister = hCapPort->hRegister;

    /* Configure Capture Port Hardware */
    BRAP_CAPPORT_P_HWConfig (hCapPort);
    
	/* Enable Capture Port */
	ui32RegVal = BRAP_Read32 (hRegister, BCHP_AUD_FMM_IOP_CTRL_CAP_CFG );

    switch(hCapPort->eCapPort)
    {
#if ( (BRAP_3548_FAMILY == 1) )
        case BRAP_CapInputPort_eRfAudio:
            ui32RegVal &= ~( (BCHP_MASK (AUD_FMM_IOP_CTRL_CAP_CFG,IGNORE_FIRST_OVERFLOW5))
                            |(BCHP_MASK (AUD_FMM_IOP_CTRL_CAP_CFG,ENA5)));

            ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_IOP_CTRL_CAP_CFG,
                                            IGNORE_FIRST_OVERFLOW5,Ignore);
            ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_IOP_CTRL_CAP_CFG,ENA5,Enable);
            break;

        case BRAP_CapInputPort_eSpdif:
        case BRAP_CapInputPort_eHdmi:            
            ui32RegVal &= ~( (BCHP_MASK (AUD_FMM_IOP_CTRL_CAP_CFG,IGNORE_FIRST_OVERFLOW6))
                            |(BCHP_MASK (AUD_FMM_IOP_CTRL_CAP_CFG,ENA6)));

            ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_IOP_CTRL_CAP_CFG,
                                            IGNORE_FIRST_OVERFLOW6,Ignore);
            ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_IOP_CTRL_CAP_CFG,ENA6,Enable);
            break;
            
        case BRAP_CapInputPort_eAdc:
#if (BCHP_VER==BCHP_VER_A0)
            ui32RegVal &= ~( (BCHP_MASK (AUD_FMM_IOP_CTRL_CAP_CFG,IGNORE_FIRST_OVERFLOW7))
                            |(BCHP_MASK (AUD_FMM_IOP_CTRL_CAP_CFG,ENA7)));

            ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_IOP_CTRL_CAP_CFG,
                                            IGNORE_FIRST_OVERFLOW7,Ignore);
            ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_IOP_CTRL_CAP_CFG,ENA7,Enable);
#else
            ui32RegVal &= ~( (BCHP_MASK (AUD_FMM_IOP_CTRL_CAP_CFG,IGNORE_FIRST_OVERFLOW10))
                            |(BCHP_MASK (AUD_FMM_IOP_CTRL_CAP_CFG,ENA10)));

            ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_IOP_CTRL_CAP_CFG,
                                            IGNORE_FIRST_OVERFLOW10,Ignore);
            ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_IOP_CTRL_CAP_CFG,ENA10,Enable);

#endif
            break;            
#endif
        case BRAP_CapInputPort_eExtI2s0:
            ui32RegVal &= ~( (BCHP_MASK (AUD_FMM_IOP_CTRL_CAP_CFG,IGNORE_FIRST_OVERFLOW4))
                            |(BCHP_MASK (AUD_FMM_IOP_CTRL_CAP_CFG,ENA4)));

            ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_IOP_CTRL_CAP_CFG,
                                            IGNORE_FIRST_OVERFLOW4,Ignore);
            ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_IOP_CTRL_CAP_CFG,ENA4,Enable);
            break;
        case BRAP_CapInputPort_eIntCapPort0:
            ui32RegVal &= ~( (BCHP_MASK (AUD_FMM_IOP_CTRL_CAP_CFG,IGNORE_FIRST_OVERFLOW0))
                            |(BCHP_MASK (AUD_FMM_IOP_CTRL_CAP_CFG,ENA0)));

            ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_IOP_CTRL_CAP_CFG,
                                            IGNORE_FIRST_OVERFLOW0,Ignore);
            ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_IOP_CTRL_CAP_CFG,ENA0,Enable);
            break;
        case BRAP_CapInputPort_eIntCapPort1:
            ui32RegVal &= ~( (BCHP_MASK (AUD_FMM_IOP_CTRL_CAP_CFG,IGNORE_FIRST_OVERFLOW1))
                            |(BCHP_MASK (AUD_FMM_IOP_CTRL_CAP_CFG,ENA1)));

            ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_IOP_CTRL_CAP_CFG,
                                            IGNORE_FIRST_OVERFLOW1,Ignore);
            ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_IOP_CTRL_CAP_CFG,ENA1,Enable);
            break;
        case BRAP_CapInputPort_eIntCapPort2:
            ui32RegVal &= ~( (BCHP_MASK (AUD_FMM_IOP_CTRL_CAP_CFG,IGNORE_FIRST_OVERFLOW2))
                            |(BCHP_MASK (AUD_FMM_IOP_CTRL_CAP_CFG,ENA2)));

            ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_IOP_CTRL_CAP_CFG,
                                            IGNORE_FIRST_OVERFLOW2,Ignore);
            ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_IOP_CTRL_CAP_CFG,ENA2,Enable);
            break;
        case BRAP_CapInputPort_eIntCapPort3:
            ui32RegVal &= ~( (BCHP_MASK (AUD_FMM_IOP_CTRL_CAP_CFG,IGNORE_FIRST_OVERFLOW3))
                            |(BCHP_MASK (AUD_FMM_IOP_CTRL_CAP_CFG,ENA3)));

            ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_IOP_CTRL_CAP_CFG,
                                            IGNORE_FIRST_OVERFLOW3,Ignore);
            ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_IOP_CTRL_CAP_CFG,ENA3,Enable);
            break;
        default:
            BDBG_ASSERT(0);
    }
    
	BRAP_Write32 (hRegister, BCHP_AUD_FMM_IOP_CTRL_CAP_CFG, ui32RegVal);

    /* Set the start count to 1 */
    hCapPort->uiStartCnt = 1;

    BDBG_LEAVE (BRAP_CAPPORT_P_Start);
    return ret;
}

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
)
{
    BERR_Code       ret = BERR_SUCCESS;
    uint32_t        ui32RegVal = 0;
    BREG_Handle     hRegister = NULL;

    BDBG_ENTER(BRAP_CAPPORT_P_Stop);
    BDBG_ASSERT (hCapPort);

    BDBG_MSG (("BRAP_CAPPORT_P_Stop(): hCapPort=0x%x, eCapPort=%d ", 
        hCapPort, hCapPort->eCapPort));

    if(0 == hCapPort->uiStartCnt)
    { 
        BDBG_WRN (("BRAP_CAPPORT_P_Stop: CapPort %d has NO active"
            "audio channels. Ignoring this call to _Stop()!",
            hCapPort->eCapPort));
        BDBG_LEAVE (BRAP_CAPPORT_P_Stop);
        return ret;
    }
    
    hCapPort->uiStartCnt --;
    BDBG_MSG (("BRAP_CAPPORT_P_Stop: CapPort %d new start count =%d",
        hCapPort->eCapPort, hCapPort->uiStartCnt));
    if (hCapPort->uiStartCnt != 0)
    {
        BDBG_MSG(("BRAP_CAPPORT_P_Stop: So do nothing!!", 
            hCapPort->eCapPort, hCapPort->uiStartCnt));        
        BDBG_LEAVE (BRAP_OP_P_Stop);
        return ret;
    }

    hRegister = hCapPort->hRegister;


    /* Disable Capture Port */
	ui32RegVal = BRAP_Read32 (hRegister, BCHP_AUD_FMM_IOP_CTRL_CAP_CFG );

    switch(hCapPort->eCapPort)
    {
#if ((BRAP_3548_FAMILY == 1) )
        case BRAP_CapInputPort_eRfAudio:
            ui32RegVal &= ~(BCHP_MASK (AUD_FMM_IOP_CTRL_CAP_CFG,ENA5));
            ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_IOP_CTRL_CAP_CFG,ENA5,Disable);
            
            ret = BRAP_RFAUDIO_P_Stop( hCapPort );
            if( BERR_SUCCESS!=ret )
                return BERR_TRACE( ret );
            break;

        case BRAP_CapInputPort_eSpdif:
        case BRAP_CapInputPort_eHdmi:            
            ui32RegVal &= ~(BCHP_MASK (AUD_FMM_IOP_CTRL_CAP_CFG,ENA6));
            ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_IOP_CTRL_CAP_CFG,ENA6,Disable);

            ret = BRAP_SPDIFRX_P_Stop (hCapPort->hFmm->hRap);
            if( BERR_SUCCESS!=ret )
                return BERR_TRACE( ret );            
            break;   

        case BRAP_CapInputPort_eAdc:
            /* Stop the ADC */
            ui32RegVal = BRAP_Read32(hRegister,BCHP_ADCCIC_CTRL_CONFIG);
            ui32RegVal &= ~(BCHP_MASK (ADCCIC_CTRL_CONFIG, 	ADC_CIC_OUTFIFO_ENA)); 
            ui32RegVal |= (BCHP_FIELD_ENUM(ADCCIC_CTRL_CONFIG, ADC_CIC_OUTFIFO_ENA, disable));
            BRAP_Write32(hRegister,BCHP_ADCCIC_CTRL_CONFIG,ui32RegVal);          
            
            ui32RegVal = BRAP_Read32 (hRegister, BCHP_AUD_FMM_IOP_CTRL_CAP_CFG );
#if (BCHP_VER==BCHP_VER_A0)
            ui32RegVal &= ~(BCHP_MASK (AUD_FMM_IOP_CTRL_CAP_CFG,ENA7));
            ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_IOP_CTRL_CAP_CFG,ENA7,Disable);
#else
            ui32RegVal &= ~(BCHP_MASK (AUD_FMM_IOP_CTRL_CAP_CFG,ENA10));
            ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_IOP_CTRL_CAP_CFG,ENA10,Disable);
#endif
            break;              
#endif
        case BRAP_CapInputPort_eExtI2s0:
            ui32RegVal &= ~(BCHP_MASK (AUD_FMM_IOP_CTRL_CAP_CFG,ENA4));
            ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_IOP_CTRL_CAP_CFG,ENA4,Disable);
            break;
        case BRAP_CapInputPort_eIntCapPort0:
            ui32RegVal &= ~(BCHP_MASK (AUD_FMM_IOP_CTRL_CAP_CFG,ENA0));
            ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_IOP_CTRL_CAP_CFG,ENA0,Disable);
            break;
        case BRAP_CapInputPort_eIntCapPort1:
            ui32RegVal &= ~(BCHP_MASK (AUD_FMM_IOP_CTRL_CAP_CFG,ENA1));
            ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_IOP_CTRL_CAP_CFG,ENA1,Disable);
            break;
        case BRAP_CapInputPort_eIntCapPort2:
            ui32RegVal &= ~(BCHP_MASK (AUD_FMM_IOP_CTRL_CAP_CFG,ENA2));
            ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_IOP_CTRL_CAP_CFG,ENA2,Disable);
            break;
        case BRAP_CapInputPort_eIntCapPort3:
            ui32RegVal &= ~(BCHP_MASK (AUD_FMM_IOP_CTRL_CAP_CFG,ENA3));
            ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_IOP_CTRL_CAP_CFG,ENA3,Disable);
            break;
        default:
            BDBG_ASSERT(0);
    }
    
	BRAP_Write32 (hRegister, BCHP_AUD_FMM_IOP_CTRL_CAP_CFG, ui32RegVal);

    BDBG_LEAVE (BRAP_CAPPORT_P_Stop);
    return ret;
}
#else

/***************************************************************************
Summary:
    Configures the HW registers for the Capture Port

Description:

Returns:
    BERR_SUCCESS on success
    Error code on failure
See Also:
    BRAP_CAPPORT_P_Start
**************************************************************************/
static BERR_Code BRAP_CAPPORT_P_HWConfig ( 
    BRAP_CAPPORT_P_Handle   hCapPort      /* [in] Capture Port Handle */)
{
    BERR_Code           ret = BERR_SUCCESS;
    uint32_t            ui32RegVal = 0, ui32LoopBackStreamId =0;
    uint32_t            ui32FciStreamId = 0;
    BREG_Handle         hRegister = NULL;
    BRAP_CAPPORT_P_Params sParams;

    BDBG_ENTER (BRAP_CAPPORT_P_HWConfig);
    BDBG_ASSERT (hCapPort);

    BDBG_MSG (("BRAP_CAPPORT_P_HWConfig(): hCapPort=0x%x, Index=%d \n", 
                                hCapPort, hCapPort->eCapPort));
  
    hRegister = hCapPort->hRegister;
    sParams = hCapPort->sParams;    
    
    switch(hCapPort->eCapPort)
    {
        case BRAP_CapInputPort_eExtI2s0:
            ui32RegVal = BRAP_Read32(hRegister,BCHP_AUD_FMM_IOP_CTRL_I2SIN_CFG0);

            ui32RegVal &= ~( (BCHP_MASK (AUD_FMM_IOP_CTRL_I2SIN_CFG0, BITS_PER_SAMPLE)) 
                     | (BCHP_MASK (AUD_FMM_IOP_CTRL_I2SIN_CFG0, DATA_JUSTIFICATION)) 
                     | (BCHP_MASK (AUD_FMM_IOP_CTRL_I2SIN_CFG0, DATA_ALIGNMENT)) 
                     | (BCHP_MASK (AUD_FMM_IOP_CTRL_I2SIN_CFG0, LRCK_POLARITY)) 
                     | (BCHP_MASK (AUD_FMM_IOP_CTRL_I2SIN_CFG0, SCLK_POLARITY)));

            switch(sParams.eInputBitsPerSample)
			{
				case BRAP_InputBitsPerSample_e16:
					ui32RegVal |= (BCHP_FIELD_ENUM(AUD_FMM_IOP_CTRL_I2SIN_CFG0, 
											BITS_PER_SAMPLE, Bitwidth16));
					break;
				case BRAP_InputBitsPerSample_e18:
					ui32RegVal |= (BCHP_FIELD_ENUM(AUD_FMM_IOP_CTRL_I2SIN_CFG0, 
											BITS_PER_SAMPLE, Bitwidth18));
					break;
				case BRAP_InputBitsPerSample_e20:
					ui32RegVal |= (BCHP_FIELD_ENUM(AUD_FMM_IOP_CTRL_I2SIN_CFG0, 
											BITS_PER_SAMPLE, Bitwidth20));
					break;
				case BRAP_InputBitsPerSample_e24:
					ui32RegVal |= (BCHP_FIELD_ENUM(AUD_FMM_IOP_CTRL_I2SIN_CFG0, 
											BITS_PER_SAMPLE, Bitwidth24));
					break;
				case BRAP_InputBitsPerSample_e32:
					ui32RegVal |= (BCHP_FIELD_ENUM(AUD_FMM_IOP_CTRL_I2SIN_CFG0, 
											BITS_PER_SAMPLE, Bitwidth32));
					break;
				default:
					BDBG_ERR (("BRAP_CAPPORT_P_HWConfig: "
							   "I2S Input Bits/Sample %d is not supported", 
							   sParams.eInputBitsPerSample));
					return BERR_TRACE (BERR_NOT_SUPPORTED);
					break;
			}

			/* Data Justification */
			if (hCapPort->hFmm->hRap->sInputSettings[hCapPort->eCapPort].sCapPortParams.uCapPortParams.sInputI2sParams.bLsbAtLRClk == true)
			{
				ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_IOP_CTRL_I2SIN_CFG0, 
												DATA_JUSTIFICATION, LSB));
			}
			else 
			{
				ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_IOP_CTRL_I2SIN_CFG0, 
												DATA_JUSTIFICATION, MSB));
			}

			/* Data Alignment */
			if (hCapPort->hFmm->hRap->sInputSettings[hCapPort->eCapPort].sCapPortParams.uCapPortParams.sInputI2sParams.bAlignedToLRClk == false)
			{
				ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_IOP_CTRL_I2SIN_CFG0, 
												DATA_ALIGNMENT, Delayed));
			}
			else 
			{
				ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_IOP_CTRL_I2SIN_CFG0, 
												DATA_ALIGNMENT, Aligned));
			}

			/* S Clock Polarity to set to Falling_aligned_with_sdata */
			ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_IOP_CTRL_I2SIN_CFG0, 
							SCLK_POLARITY, Falling_aligned_with_sdata));
			
			/* LR Clock Polarity */
			if (hCapPort->hFmm->hRap->sInputSettings[hCapPort->eCapPort].sCapPortParams.uCapPortParams.sInputI2sParams.bLRClkPolarity == true)
			{
				ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_IOP_CTRL_I2SIN_CFG0, 
												LRCK_POLARITY, High_for_left));
			}
			else 
			{
				ui32RegVal |= (BCHP_FIELD_ENUM (AUD_FMM_IOP_CTRL_I2SIN_CFG0, 
												LRCK_POLARITY, Low_for_left));
			}

            BRAP_Write32(hRegister,BCHP_AUD_FMM_IOP_CTRL_I2SIN_CFG0,ui32RegVal);

            break;
#if ((BRAP_7420_FAMILY==1) && (!((BCHP_CHIP == 7420) && (BCHP_VER == A0))))
        case BRAP_CapInputPort_eIntCapPort0:
            ui32LoopBackStreamId = 0;
            ui32FciStreamId = 10;
            break;
        case BRAP_CapInputPort_eIntCapPort1:
            ui32LoopBackStreamId = 1;
            ui32FciStreamId = 11;
            break;
        case BRAP_CapInputPort_eIntCapPort2:
            ui32LoopBackStreamId = 2;
            ui32FciStreamId = 12;
            break;
        case BRAP_CapInputPort_eIntCapPort3:
            ui32LoopBackStreamId = 3;
            ui32FciStreamId = 13;
            break;
#else
        case BRAP_CapInputPort_eIntCapPort0:
            ui32LoopBackStreamId = 0;
            ui32FciStreamId = 9;
            break;
        case BRAP_CapInputPort_eIntCapPort1:
            ui32LoopBackStreamId = 1;
            ui32FciStreamId = 10;
            break;
        case BRAP_CapInputPort_eIntCapPort2:
            ui32LoopBackStreamId = 2;
            ui32FciStreamId = 11;
            break;
        case BRAP_CapInputPort_eIntCapPort3:
            ui32LoopBackStreamId = 3;
            ui32FciStreamId = 12;
            break;
#endif
#if (BRAP_7405_FAMILY != 1)            
        case BRAP_CapInputPort_eIntCapPort4:
            ui32LoopBackStreamId = 4;
            ui32FciStreamId = 13;
            break;
        case BRAP_CapInputPort_eIntCapPort5:
            ui32LoopBackStreamId = 5;
            ui32FciStreamId = 14;
            break;
        case BRAP_CapInputPort_eIntCapPort6:
            ui32LoopBackStreamId = 6;
            ui32FciStreamId = 15;
            break;
        case BRAP_CapInputPort_eIntCapPort7:
            ui32LoopBackStreamId = 7;
            ui32FciStreamId = 16;
            break;
#endif            
        default:
            BDBG_ASSERT (0);
    }

    if(BRAP_CapInputPort_eExtI2s0 != hCapPort->eCapPort)
    {
        ui32RegVal = BRAP_Read32(hRegister,(BCHP_AUD_FMM_IOP_CTRL_LOOPBACK_CFGi_ARRAY_BASE+(ui32LoopBackStreamId*4)));

        ui32RegVal &= ~( (BCHP_MASK (AUD_FMM_IOP_CTRL_LOOPBACK_CFGi, INS_NOACK)) 
                 | (BCHP_MASK (AUD_FMM_IOP_CTRL_LOOPBACK_CFGi, INSERT_CTL))
                 | (BCHP_MASK (AUD_FMM_IOP_CTRL_LOOPBACK_CFGi, TMG_SRC_SEL)) 
                 | (BCHP_MASK (AUD_FMM_IOP_CTRL_LOOPBACK_CFGi, IGNORE_FIRST_UNDERFLOW)));
        /* Program LOOPBACK0_INS, TMG_SRC_SEL, IGNORE_FIRST_UNDERFLOW0 */


#if (BRAP_7405_FAMILY != 1)
/*On 7405 platform, Enabling this leads to movement of DSTCH Fifo ptrs, 
even if data is not available. This leads initial zeros.*/
        ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_IOP_CTRL_LOOPBACK_CFGi, INS_NOACK, Enable);
#endif
        ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_IOP_CTRL_LOOPBACK_CFGi, INSERT_CTL, No_insert);
        ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_IOP_CTRL_LOOPBACK_CFGi, IGNORE_FIRST_UNDERFLOW, Ignore);
        switch(sParams.uiFsTmgSrc)
        {
            case 0:
                ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_IOP_CTRL_LOOPBACK_CFGi, 
                    TMG_SRC_SEL, Fs0);
                break;
            case 1:
                ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_IOP_CTRL_LOOPBACK_CFGi, 
                    TMG_SRC_SEL, Fs1);
                break;
            case 2:
                ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_IOP_CTRL_LOOPBACK_CFGi, 
                    TMG_SRC_SEL, Fs2);
                break;
            case 3:
                ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_IOP_CTRL_LOOPBACK_CFGi, 
                    TMG_SRC_SEL, Fs3);
                break;                    
            default:
                BDBG_ASSERT(0);                
        }/* switch uiFsTmgSrc */

        BRAP_Write32(hRegister,(BCHP_AUD_FMM_IOP_CTRL_LOOPBACK_CFGi_ARRAY_BASE+(ui32LoopBackStreamId*4)),ui32RegVal);

        /* Program the Input FCI Id */
        ui32RegVal = BRAP_Read32(hRegister , 
            (BCHP_AUD_FMM_IOP_CTRL_FCI_CFGi_ARRAY_BASE + (ui32FciStreamId*4)));

        ui32RegVal &= ~(BCHP_MASK(AUD_FMM_IOP_CTRL_FCI_CFGi, ID));
        ui32RegVal |= BCHP_FIELD_DATA(AUD_FMM_IOP_CTRL_FCI_CFGi, ID,sParams.uiInputFciId);

        BRAP_Write32(hRegister , (BCHP_AUD_FMM_IOP_CTRL_FCI_CFGi_ARRAY_BASE + 
                                (ui32FciStreamId*4)),ui32RegVal);


        /* Program the clock that drives the external capture port */
        ret = BRAP_CAPPORT_P_ProgramCaptureClock(hCapPort, 
                                       sParams.uiFsTmgSrc, 
                                       sParams.ePll, 
                                       sParams.eSamplingRate);
        if(BERR_SUCCESS != ret)
        {
            return BERR_TRACE(ret);
        }

    }

    BDBG_LEAVE (BRAP_CAPPORT_P_HWConfig);
    return ret;
}

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
)
{
    BERR_Code       ret = BERR_SUCCESS;
    uint32_t        ui32RegVal = 0, ui32StreamId = 0;
    BREG_Handle     hRegister = NULL;

    BDBG_ENTER (BRAP_CAPPORT_P_Start);
    BDBG_ASSERT (hCapPort);

    if (BRAP_FMM_P_GetWatchdogRecoveryFlag (hCapPort->hFmm) == false)
    {   
        /* If not in WatchDog recovery */  
        BDBG_ASSERT (pParams);

        if (hCapPort->uiStartCnt > 0)
        {
            /* This Outport was previoulsy started by another audio channel.
               So this time, just increment the start count. Dont change the 
               settings etc. */
            /* Also, start count cannot exceed the open count!! */
            if (hCapPort->uiStartCnt >= hCapPort->uiOpenCnt)
            {
                BDBG_ERR (("BRAP_CAPPORT_P_Start: Cannot start capport %d. "
                        "Start Count (%d) >= Open Count (%d)",
                        hCapPort->eCapPort, hCapPort->uiStartCnt, hCapPort->uiOpenCnt));
                return BERR_TRACE (BERR_NOT_SUPPORTED);
            }
        
            hCapPort->uiStartCnt ++;
            BDBG_MSG (("BRAP_CAPPORT_P_Start: CapPort %d was already started." 
                        "New start count = %d", 
                        hCapPort->eCapPort, hCapPort->uiStartCnt));

            BDBG_LEAVE (BRAP_CAPPORT_P_Start);
            return ret;
        }
        
        /* Store the start parameters inside the handle */
        hCapPort->sParams = *pParams;

        BDBG_MSG (("BRAP_CAPPORT_P_Start(): hCapPort=0x%x, Type=%d, \n"
				"hCapPort->sParams.sExtParams.bLsbAtLRClk=%d\n"
				"hCapPort->sParams.sExtParams.bAlignedToLRClk=%d\n"
                "hCapPort->sParams.sExtParams.bLRClkPolarity=%d\n"
                "hCapPort->sParams.uiInputFciId =%d\n"
                "hCapPort->sParams.eInputBitsPerSample=%d\n"
                "hCapPort->sParams.uiFsTmgSrc = %d \n"
                "hCapPort->sParams.ePll = %d\n"
                "hCapPort->sParams.eSamplingRate = %d \n",
                hCapPort, hCapPort->eCapPort,
				hCapPort->hFmm->hRap->sInputSettings[hCapPort->eCapPort].sCapPortParams.uCapPortParams.sInputI2sParams.bLsbAtLRClk, 
				hCapPort->hFmm->hRap->sInputSettings[hCapPort->eCapPort].sCapPortParams.uCapPortParams.sInputI2sParams.bAlignedToLRClk, 
				hCapPort->hFmm->hRap->sInputSettings[hCapPort->eCapPort].sCapPortParams.uCapPortParams.sInputI2sParams.bLRClkPolarity, 
				hCapPort->sParams.uiInputFciId, 				
				hCapPort->sParams.eInputBitsPerSample,
                hCapPort->sParams.uiFsTmgSrc, 
                hCapPort->sParams.ePll,
                hCapPort->sParams.eSamplingRate));
    }   /* End: If not in WatchDog recovery */
   
    hRegister = hCapPort->hRegister;

    /* Configure Capture Port Hardware */
    BRAP_CAPPORT_P_HWConfig (hCapPort);
    
	/* Enable Capture Port */
    switch(hCapPort->eCapPort)
    {
        case BRAP_CapInputPort_eExtI2s0:
#if (BRAP_7405_FAMILY == 1)
            ui32StreamId = 4;
#else
            ui32StreamId = 8;
#endif
            break;
        case BRAP_CapInputPort_eIntCapPort0:
            ui32StreamId = 0;
            break;
        case BRAP_CapInputPort_eIntCapPort1:
            ui32StreamId = 1;
            break;
        case BRAP_CapInputPort_eIntCapPort2:
            ui32StreamId = 2;
            break;
        case BRAP_CapInputPort_eIntCapPort3:
            ui32StreamId = 3;
            break;
#if (BRAP_7405_FAMILY != 1)
        case BRAP_CapInputPort_eIntCapPort4:
            ui32StreamId = 4;
            break;
        case BRAP_CapInputPort_eIntCapPort5:
            ui32StreamId = 5;
            break;
        case BRAP_CapInputPort_eIntCapPort6:
            ui32StreamId = 6;
            break;
        case BRAP_CapInputPort_eIntCapPort7:
            ui32StreamId = 7;
            break;
#endif            
        default:
            BDBG_ASSERT(0);
    }

    ui32RegVal = BRAP_Read32 (hRegister, 
              (BCHP_AUD_FMM_IOP_CTRL_CAP_CFGi_ARRAY_BASE+(ui32StreamId*4)));
    
    ui32RegVal &= ~( (BCHP_MASK (AUD_FMM_IOP_CTRL_CAP_CFGi,IGNORE_FIRST_OVERFLOW))
                    |(BCHP_MASK (AUD_FMM_IOP_CTRL_CAP_CFGi,ENA))
                    |(BCHP_MASK (AUD_FMM_IOP_CTRL_CAP_CFGi,GROUP)));

    ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_IOP_CTRL_CAP_CFGi,
                                    IGNORE_FIRST_OVERFLOW,Ignore);

    /* Grouping of cap ports */
    ui32RegVal |= BCHP_FIELD_DATA(AUD_FMM_IOP_CTRL_CAP_CFGi,GROUP,
                                                          pParams->uiGrpId); 
    ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_IOP_CTRL_CAP_CFGi,ENA,Enable);
	BRAP_Write32 (hRegister, (BCHP_AUD_FMM_IOP_CTRL_CAP_CFGi_ARRAY_BASE+
                                            (ui32StreamId*4)), ui32RegVal);

    
    if (BRAP_FMM_P_GetWatchdogRecoveryFlag (hCapPort->hFmm) == false)
    {   
        /* Set the start count to 1 */
        hCapPort->uiStartCnt = 1;
    }

    BDBG_LEAVE (BRAP_CAPPORT_P_Start);
    return ret;
}

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
)
{
    BERR_Code       ret = BERR_SUCCESS;
    uint32_t        ui32RegVal = 0, ui32StreamId =0;
    BREG_Handle     hRegister = NULL;

    BDBG_ENTER(BRAP_CAPPORT_P_Stop);
    BDBG_ASSERT (hCapPort);

    BDBG_MSG (("BRAP_CAPPORT_P_Stop(): hCapPort=0x%x, eCapPort=%d ", 
        hCapPort, hCapPort->eCapPort));

    if(0 == hCapPort->uiStartCnt)
    { 
        BDBG_MSG (("BRAP_CAPPORT_P_Stop: CapPort %d has NO active"
            "audio channels. Ignoring this call to _Stop()!",
            hCapPort->eCapPort));
        BDBG_LEAVE (BRAP_CAPPORT_P_Stop);
        return ret;
    }
    
    hCapPort->uiStartCnt --;
    BDBG_MSG (("BRAP_CAPPORT_P_Stop: CapPort %d new start count =%d",
        hCapPort->eCapPort, hCapPort->uiStartCnt));
    if (hCapPort->uiStartCnt != 0)
    {
        BDBG_MSG(("BRAP_CAPPORT_P_Stop: So do nothing!!", 
            hCapPort->eCapPort, hCapPort->uiStartCnt));        
        BDBG_LEAVE (BRAP_OP_P_Stop);
        return ret;
    }
    hRegister = hCapPort->hRegister;

    /* Disable Capture Port */
	switch(hCapPort->eCapPort)
    {
        case BRAP_CapInputPort_eExtI2s0:
#if (BRAP_7405_FAMILY == 1)
            ui32StreamId = 4;
#else
            ui32StreamId = 8;
#endif
            break;
        case BRAP_CapInputPort_eIntCapPort0:
            ui32StreamId = 0;
            break;
        case BRAP_CapInputPort_eIntCapPort1:
            ui32StreamId = 1;
            break;
        case BRAP_CapInputPort_eIntCapPort2:
            ui32StreamId = 2;
            break;
        case BRAP_CapInputPort_eIntCapPort3:
            ui32StreamId = 3;
            break;
#if (BRAP_7405_FAMILY != 1)            
        case BRAP_CapInputPort_eIntCapPort4:
            ui32StreamId = 4;
            break;
        case BRAP_CapInputPort_eIntCapPort5:
            ui32StreamId = 5;
            break;
        case BRAP_CapInputPort_eIntCapPort6:
            ui32StreamId = 6;
            break;
        case BRAP_CapInputPort_eIntCapPort7:
            ui32StreamId = 7;
            break;
#endif            
        default:
            BDBG_ASSERT(0);
    }

    ui32RegVal = BRAP_Read32 (hRegister, 
              (BCHP_AUD_FMM_IOP_CTRL_CAP_CFGi_ARRAY_BASE+(ui32StreamId*4)));
    
    ui32RegVal &= ~( (BCHP_MASK (AUD_FMM_IOP_CTRL_CAP_CFGi,ENA))
                    |(BCHP_MASK (AUD_FMM_IOP_CTRL_CAP_CFGi,GROUP)));

    /* Grouping of cap ports */
    ui32RegVal |= BCHP_FIELD_DATA(AUD_FMM_IOP_CTRL_CAP_CFGi,GROUP,ui32StreamId); 
    ui32RegVal |= BCHP_FIELD_ENUM(AUD_FMM_IOP_CTRL_CAP_CFGi,ENA,Disable);
    
	BRAP_Write32 (hRegister, (BCHP_AUD_FMM_IOP_CTRL_CAP_CFGi_ARRAY_BASE+
                                            (ui32StreamId*4)), ui32RegVal);

    BDBG_LEAVE (BRAP_CAPPORT_P_Stop);
    return ret;
}

#endif

/***************************************************************************
Summary: Refer to the Prototype
***************************************************************************/
static BERR_Code BRAP_CAPPORT_P_ProgramCaptureClock(
    BRAP_CAPPORT_P_Handle   hCapPort,      /* [in] Capture Port Handle */
    unsigned int            uiFsTmgSrcId,   /* [in] FS Timing Source Id */
    BRAP_OP_Pll 			ePll,           /* [in] PLL to be associated */
    BAVC_AudioSamplingRate 	eSamplingRate   /* [in] Sampling rate of the data at 
                                               CapPort or FCI sinker driven by 
                                               this FS timing source */
)
{
    BERR_Code       ret = BERR_SUCCESS;

    BDBG_ENTER(BRAP_CAPPORT_P_ProgramCaptureClock);
    BDBG_ASSERT(hCapPort);
    
    /* Program the MCLK and 1x, 2x or 4x for FS timing source */
    ret = BRAP_P_ConfigureFsTimingSource(hCapPort->hRegister, 
                                   uiFsTmgSrcId, 
                                   ePll, 
                                   eSamplingRate);
    if(BERR_SUCCESS != ret)
    {
        return BERR_TRACE(ret);
    }

    /* Pass the Capture Sample Rate Info to the upper SW layer */
    if(true == BRAP_P_IsInternalCapPort(hCapPort->eCapPort))
    {
        bool    bFound=false;
        unsigned uiAssocId=0, uiDstIndex=0;
        BRAP_OutputChannelPair  eChPair = BRAP_OutputChannelPair_eMax;
        
        for(uiAssocId=0; uiAssocId<BRAP_MAX_ASSOCIATED_GROUPS; uiAssocId++)
        {
            for(uiDstIndex=0; uiDstIndex < BRAP_P_MAX_DST_PER_RAPCH; uiDstIndex++)
            {
                if(BRAP_AudioDst_eRingBuffer == hCapPort->hFmm->hRap->sAssociatedCh[uiAssocId].sDstDetails[uiDstIndex].sExtDstDetails.eAudioDst)
                {
                    for(eChPair = 0; eChPair < BRAP_RM_P_MAX_OP_CHANNEL_PAIRS; eChPair++)
                    {
                        if(hCapPort->eCapPort == hCapPort->hFmm->hRap->sAssociatedCh[uiAssocId].sDstDetails[uiDstIndex].sExtDstDetails.uDstDetails.sRBufDetails.eCapPort[eChPair])
                        {
                            bFound = true;
                            if(hCapPort->hFmm->hRap->sAssociatedCh[uiAssocId].sDstDetails[uiDstIndex].bSampleRateChangeCallbackEnabled == true)
                            {
                                BKNI_EnterCriticalSection();
                                BRAP_P_DestinationSampleRateChange_isr(
                                    (void *)(&(hCapPort->hFmm->hRap->sAssociatedCh[uiAssocId].sDstDetails[uiDstIndex]))
                                    ,(unsigned int)eSamplingRate);
                                BKNI_LeaveCriticalSection();
                            }
                            break;                            
                        }
                    }
                }
                if(bFound == true)
                    break;
            }
            if(bFound == true)
                break;            
        }
    }
    
    BDBG_LEAVE (BRAP_CAPPORT_P_ProgramCaptureClock);
    return ret;    
}

/* End of File */
