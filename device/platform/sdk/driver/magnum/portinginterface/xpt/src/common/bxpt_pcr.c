/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxpt_pcr.c $
 * $brcm_Revision: Hydra_Software_Devel/11 $
 * $brcm_Date: 6/29/12 10:16a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/xpt/src/core40nm/bxpt_pcr.c $
 * 
 * Hydra_Software_Devel/11   6/29/12 10:16a gmullen
 * SW7429-115: Added more debug info
 * 
 * Hydra_Software_Devel/10   6/21/12 10:07a gmullen
 * SW7429-115: Fixed bug in saturation code. Use last PHASE_ERROR if PCR
 * error is reported
 * 
 * Hydra_Software_Devel/9   5/17/12 10:21a gmullen
 * SW7429-115: Downgraded integrator warning mesg to debug
 * 
 * Hydra_Software_Devel/8   5/8/12 10:15a gmullen
 * SW7429-115: Merged to Hydra
 * 
 * Hydra_Software_Devel/SW7429-115/2   4/20/12 10:19a gmullen
 * SW7429-115: Return BERR_Code
 * 
 * Hydra_Software_Devel/SW7429-115/1   4/6/12 2:04p gmullen
 * SW7429-115: Implemented workaround
 * 
 * Hydra_Software_Devel/7   9/12/11 4:36p gmullen
 * SW7344-193: Merged changes to Hydra
 * 
 * Hydra_Software_Devel/SW7344-193/1   9/7/11 4:34p gmullen
 * SW7344-193: Added tri-state logic to jitter API
 * 
 * Hydra_Software_Devel/6   8/31/11 2:30p gmullen
 * SW7408-284: Fixed bad arg in call to BREG_Write32
 * 
 * Hydra_Software_Devel/5   5/16/11 4:58p gmullen
 * SW7408-284: Added support for jitter adjust in PCR hw to PI
 * 
 * Hydra_Software_Devel/4   11/8/10 4:02p gmullen
 * SW7422-20: PI can now generate the required INT IDs
 * 
 * Hydra_Software_Devel/3   11/6/10 4:57p gmullen
 * SW7422-20: Fixed incorrect register address calc
 * 
 * Hydra_Software_Devel/2   10/27/10 2:08p gmullen
 * SW7422-20: Added BXPT_PCR_TimeRef_eHD_DVI_H1 and
 * BXPT_PCR_TimeRef_eHD_DVI_V1 as placeholders
 * 
 * Hydra_Software_Devel/39   9/17/09 2:23p piyushg
 * SW7400-2527: Fix coverity defect.
 * 
 * Hydra_Software_Devel/38   8/12/09 5:48p gmullen
 * PR57174: Incorrect check for macro.
 * 
 * Hydra_Software_Devel/37   8/11/09 10:39a piyushg
 * PR55216: Added initial 7340 XPT support.
 * 
 * Hydra_Software_Devel/36   8/5/09 4:52p piyushg
 * PR55545: Add 7125 XPT PI support
 * Added file element "bxpt_rave_ihex.c".
 * Added file element "bxpt_rave_ihex.h".
 * 
 * Hydra_Software_Devel/35   7/31/09 3:27p piyushg
 * PR56771: Add support for 7342.
 * 
 * Hydra_Software_Devel/34   7/31/09 3:24p gmullen
 * PR54331: Fixed compilation bug in 7405.
 * 
 * Hydra_Software_Devel/33   7/31/09 2:44p gmullen
 * PR54331: Added 35130 to XPT support.
 * 
 * Hydra_Software_Devel/32   7/14/09 10:22a piyushg
 * PR56771: Add XPT PI code for 7342.
 * 
 * Hydra_Software_Devel/31   6/17/09 6:46p gmullen
 * PR56110: Added support.xpt/7550/uif_image/a0
 * 
 * Hydra_Software_Devel/30   6/8/09 3:05p piyushg
 * PR53746: Remove DSS packetmode restriction for
 * BXPT_PCR_GetPhaseError_isr.
 * 
 * Hydra_Software_Devel/29   5/11/09 5:10p piyushg
 * PR53746: Added xpt interface for retrieving phase error at isr time
 * BXPT_PCR_GetPhaseError_isr().
 * 
 * Hydra_Software_Devel/28   4/7/09 5:25p piyushg
 * PR52986: Add support for 7635
 * Added directory element "7635".
 * 
 * Hydra_Software_Devel/27   2/24/09 9:36a gmullen
 * PR 52429: Merged fix from wince_7400 branch
 * 
 * Hydra_Software_Devel/wince_7400/1   2/24/09 9:28a gmullen
 * PR 52429: Block redudant writes to filter coeffs and reference
 * polarity.
 * 
 * Hydra_Software_Devel/26   2/6/09 11:36a gmullen
 * PR51625: Fixed conditional compilation error for 7336.
 * 
 * Hydra_Software_Devel/25   2/5/09 12:12p piyushg
 * PR51625: Added 7336 support
 * 
 * Hydra_Software_Devel/24   1/27/09 1:09p gmullen
 * PR51625: Added 7336 support
 * 
 * Hydra_Software_Devel/23   11/26/08 4:14p gmullen
 * PR47755: Added support for 7420.
 * 
 * Hydra_Software_Devel/22   11/10/08 9:07a gmullen
 * PR48128: Added playback support for 7325/7335. Removed redundant code.
 * 
 * Hydra_Software_Devel/21   9/12/08 2:21p gmullen
 * PR46659: Added 3556/3548 B0 support
 * 
 * Hydra_Software_Devel/20   9/12/08 11:27a gmullen
 * PR46659: Added B0 support
 * 
 * Hydra_Software_Devel/19   9/8/08 12:05p piyushg
 * PR46621: Added default values for BXPT_PCR_TimeRef_eInternal in
 * BXPT_PCR_GetTimeBaseFreqRefDefaults().
 * 
 * Hydra_Software_Devel/18   8/15/08 10:47a gmullen
 * PR45362: Removed calls to BKNI_Enter/LeaveCriticalSectionISR()
 * 
 * Hydra_Software_Devel/17   8/8/08 5:47p gmullen
 * PR45362: Merged SanJose_CDI_Devel changes to Hydra_Software_Devel
 * 
 * Hydra_Software_Devel/16   8/4/08 5:35p gmullen
 * PR45240: Added support for playbacks as a source for
 * BXPT_PCR_SetStreamPcrConfig, for 3556/3548
 * 
 * Hydra_Software_Devel/15   2/27/08 4:16p katrep
 * PR36900: Added playvack support for 7325 and 7335
 * 
 * Hydra_Software_Devel/14   2/5/08 3:10p piyushg
 * PR38634: Change XPT_DPCR0_STC_EXT_CTRL to XPT_DPCR0_REF_PCR
 * in BXPT_PCR_SetStreamPcrConfig function.
 * 
 * Hydra_Software_Devel/13   2/5/08 8:19a gmullen
 * PR39329: Added ISR version of GetLastPcr.
 * 
 * Hydra_Software_Devel/12   10/11/07 9:37a gmullen
 * PR35998: Wrong register used to set Prescale and Inc bitfields.
 * 
 * Hydra_Software_Devel/11   8/15/07 4:52p gmullen
 * PR34082: Added filter coefficients to BXPT_PCR_XptStreamPcrCfg struct.
 * 
 * Hydra_Software_Devel/10   7/18/07 1:38p gmullen
 * PR33106: Added BXPT_PCR_SetCenterFrequency()
 * 
 * Hydra_Software_Devel/9   3/22/07 9:35a gmullen
 * PR28959: Added support for playback streams.
 * 
 * Hydra_Software_Devel/8   12/12/06 12:22p katrep
 * PR25767: program the Packet Mode according to the stream type.
 * 
 * Hydra_Software_Devel/7   11/10/06 5:47p katrep
 * PR25737: Added BXPT_PCR_SetTimeRefTrackRange API.
 * 
 * Hydra_Software_Devel/6   8/18/06 10:13a gmullen
 * PR23345: Added MaxPcrError to Get/SetSettings calls.
 * 
 * Hydra_Software_Devel/5   8/2/06 4:33p gmullen
 * PR23137: Added support to GetPcrStreamCfg
 * 
 * Hydra_Software_Devel/4   8/2/06 3:56p gmullen
 * PR23137: Added support for playback data to PCR block.
 * 
 * Hydra_Software_Devel/3   4/18/06 9:52a gmullen
 * PR18998: Close() call was writing to wrong register.
 * 
 * Hydra_Software_Devel/2   4/12/06 10:34a gmullen
 * PR20631: Added compilation checks for 7401 B0
 * 
 * Hydra_Software_Devel/3   9/26/05 4:40p gmullen
 * PR15309: Removed support for loading user STCs. That support is now in
 * bxpt_pcr_offset.c
 * 
 * Hydra_Software_Devel/2   8/12/05 8:57a gmullen
 * PR15309: Added PCR, PCR Offset, DirecTV, RAVE video, and ITB/CDB
 * endianess support. Tested same.
 * 
 * Hydra_Software_Devel/1   8/4/05 8:14a gmullen
 * PR15309: Added PCR support.
 * 
 ***************************************************************************/

#include "bstd.h"
#include "bkni.h" 
#include "bxpt_pcr.h"

#if BXPT_NUM_PCRS > 1 
#include "bchp_xpt_dpcr1.h"
#endif
#include "bchp_xpt_dpcr_pp.h"
#include "bxpt_priv.h"
  
BDBG_MODULE( xpt_pcr );

#define XPR_PCR_DEF_MAX_PCR_ERROR  100  

/* Support for automatically scaling the number of DPCR blocks. */
#ifdef BCHP_XPT_DPCR1_PID_CH
    #define BXPT_PCR_P_REGOFFSET  (BCHP_XPT_DPCR1_PID_CH - BCHP_XPT_DPCR0_PID_CH)
#else
    #define BXPT_PCR_P_REGOFFSET  (BCHP_XPT_DPCR0_PHASE_ERROR_CLAMP - BCHP_XPT_DPCR0_PID_CH)
#endif

#define RESET_FILTA   7 
#define RESET_FILTB   8
#define RESET_FILTC   3 
#define RESET_TRACK_RANGE   3 

static BERR_Code BXPT_PCR_P_Lock2Source( BXPT_PCR_Handle hPcr, BXPT_PCR_TimeRef eTimeRef, BXPT_PCR_TimebaseFreqRefConfig *Timebasefreq );
static void SetPcrIncAndPrescale( BXPT_PCR_Handle hPcr, const BXPT_PCR_TimebaseFreqRefConfig *Timebasefreq );
static void GetPcrIncAndPrescale( BXPT_PCR_Handle hPcr, BXPT_PCR_TimebaseFreqRefConfig  *TimebaseFreqConfig );


BERR_Code BXPT_PCR_P_Lock2Source(
    BXPT_PCR_Handle hPcr, 
    BXPT_PCR_TimeRef eTimeRef,
    BXPT_PCR_TimebaseFreqRefConfig *Timebasefreq
    )
{
    uint32_t      Reg;
    uint8_t     FiltA, FiltB, FiltC;
    BERR_Code     ret = BERR_SUCCESS;
    BREG_Handle   hRegister = hPcr->hRegister;

    /* Always allow the PRESCALE and PCR_INC to be adjusted */
    if (Timebasefreq && eTimeRef != BXPT_PCR_TimeRef_eXpt)
    {
        SetPcrIncAndPrescale( hPcr, Timebasefreq );
    }

    /* 
    ** Don't write to the loop filter or polarity bitfields if the values aren't being 
    ** changed. It can cause a glitch that interferes with the HDCP authentication. 
    */
    if( Timebasefreq )
    {
        FiltA = Timebasefreq->FiltA;
        FiltB = Timebasefreq->FiltB;
        FiltC = Timebasefreq->FiltC;
    }
    else
    {
        FiltA = RESET_FILTA;
        FiltB = RESET_FILTB;
        FiltC = RESET_FILTC;
    }
    
#if BXPT_HAS_DPCR_INTEGRATOR_WORKAROUND
    hPcr->FiltB = FiltB;
    hPcr->FiltC = FiltC;
#endif

    Reg = BREG_Read32(hRegister, BCHP_XPT_DPCR0_LOOP_CTRL+ hPcr->RegOffset);

    if( BCHP_GET_FIELD_DATA( Reg, XPT_DPCR0_LOOP_CTRL, TIME_REF ) != eTimeRef 
    || BCHP_GET_FIELD_DATA( Reg, XPT_DPCR0_LOOP_CTRL, FILT_C ) != FiltC
    || BCHP_GET_FIELD_DATA( Reg, XPT_DPCR0_LOOP_CTRL, FILT_B ) != FiltB
    || BCHP_GET_FIELD_DATA( Reg, XPT_DPCR0_LOOP_CTRL, FILT_A ) != FiltA
    )
    {
        /* First Set Ref_Polarity to 0-None */
        Reg = BREG_Read32(hRegister, BCHP_XPT_DPCR0_LOOP_CTRL+ hPcr->RegOffset);
        Reg &= ~(BCHP_MASK( XPT_DPCR0_LOOP_CTRL, REF_POLARITY ));
        BREG_Write32( hRegister, BCHP_XPT_DPCR0_LOOP_CTRL + hPcr->RegOffset, Reg);
    
        /* do the follow 2 steps if time reference is non-xpt*/
        /* 1) change frequency time reference in XPT_DPCR0_REF_PCR*/
        /* 2) Set the packetmode to 1 for non_xpt reference*/
            /*
            Does it mean that we are in direct TV mode?  
            Reg = BREG_Read32(hRegister, BCHP_XPT_DPCR0_CTRL + hPcr->RegOffset);
            Reg &= ~(BCHP_MASK( XPT_DPCR0_CTRL, PCR_PACKET_MODE ));
            Reg |= BCHP_FIELD_DATA(XPT_DPCR0_CTRL, PCR_PACKET_MODE,1);
            BREG_Write32( hRegister, BCHP_XPT_DPCR0_CTRL + hPcr->RegOffset, Reg); 
            */      
    
        Reg = BREG_Read32(hRegister, BCHP_XPT_DPCR0_CTRL + hPcr->RegOffset);
        Reg &= ~(BCHP_MASK( XPT_DPCR0_CTRL, PCR_PACKET_MODE ));
        Reg |= BCHP_FIELD_DATA( XPT_DPCR0_CTRL, PCR_PACKET_MODE, hPcr->DMode == true ? 1 : 0 );
        BREG_Write32( hRegister, BCHP_XPT_DPCR0_CTRL + hPcr->RegOffset, Reg);       
    
        /* Set the Time_Ref */
        /* Change the loop constants, if they where given. Otherwise, reload the reset defaults. */
        Reg = BREG_Read32(hRegister, BCHP_XPT_DPCR0_LOOP_CTRL+ hPcr->RegOffset);
        Reg &= ~(
            BCHP_MASK( XPT_DPCR0_LOOP_CTRL, TIME_REF ) |
            BCHP_MASK( XPT_DPCR0_LOOP_CTRL, FILT_C ) |
            BCHP_MASK( XPT_DPCR0_LOOP_CTRL, FILT_B ) |
            BCHP_MASK( XPT_DPCR0_LOOP_CTRL, FILT_A )
        );
        Reg |= (
            BCHP_FIELD_DATA( XPT_DPCR0_LOOP_CTRL, TIME_REF, eTimeRef ) |
            BCHP_FIELD_DATA( XPT_DPCR0_LOOP_CTRL, FILT_C, FiltC ) |
            BCHP_FIELD_DATA( XPT_DPCR0_LOOP_CTRL, FILT_B, FiltB ) |
            BCHP_FIELD_DATA( XPT_DPCR0_LOOP_CTRL, FILT_A, FiltA )
        );
        BREG_Write32( hRegister, BCHP_XPT_DPCR0_LOOP_CTRL + hPcr->RegOffset, Reg);
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)
        BDBG_MSG(( "DPCR%u TIME_REF 0x%X, FILT_A 0x%X, FILT_B 0x%X, FILT_C 0x%X", 
            hPcr->ChannelNo, eTimeRef, FiltA, FiltB, FiltC ));
#endif
        /* Set Ref_Polarity to 1*/ 
        Reg = BREG_Read32(hRegister, BCHP_XPT_DPCR0_LOOP_CTRL+ hPcr->RegOffset);
        Reg &= ~(BCHP_MASK( XPT_DPCR0_LOOP_CTRL, REF_POLARITY ));
        Reg |= BCHP_FIELD_DATA( XPT_DPCR0_LOOP_CTRL, REF_POLARITY, 1);
        BREG_Write32( hRegister, BCHP_XPT_DPCR0_LOOP_CTRL + hPcr->RegOffset, Reg);

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)
#if BXPT_HAS_DPCR_INTEGRATOR_WORKAROUND
        hPcr->ChangingToNonPcrSource = BXPT_PCR_TimeRef_eXpt != eTimeRef ? true : false;
#endif
#endif
    }

    return ret;
}


BERR_Code BXPT_PCR_GetTotalChannels(
    BXPT_Handle hXpt,           
    unsigned int *TotalChannels     
    )
{
    BERR_Code  ret = BERR_SUCCESS;
    *TotalChannels = hXpt->MaxPcrs;
    if(!(hXpt->MaxPcrs))
    {
        BDBG_ERR(("No PCR Channels Available!"));
        ret = BERR_TRACE(BERR_INVALID_PARAMETER);
    }
    return ret; 
}


BERR_Code BXPT_PCR_GetChannelDefSettings(
    BXPT_Handle hXpt,                       /* [in] The transport handle - need chip info */
    unsigned int WhichPcr,                      /* [In] Which pcr module */
    BXPT_PCR_DefSettings *pcrSettings       /* [out] The default settings of a pcr module */
    )
{
    BERR_Code  ret = BERR_SUCCESS;

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)
    BKNI_Memset( (void *) pcrSettings, 0, sizeof(*pcrSettings) );
#endif
    if ( WhichPcr >= hXpt->MaxPcrs )
    {
        ret = BERR_TRACE( BERR_INVALID_PARAMETER );
        BDBG_ERR(("Pcr number %d exceeds max PCR channels!",WhichPcr));
        return ret;
    }   
    /* the current default setting for all the PCR blocks are the same*/
    pcrSettings -> PcrTwoErrReaquireEn = 1; 
    pcrSettings -> MaxPcrError = XPR_PCR_DEF_MAX_PCR_ERROR;
    return ret;
}


BERR_Code BXPT_PCR_Open(
    BXPT_Handle hXpt,
    unsigned int WhichPcr,
    BXPT_PCR_DefSettings *pcrSettings,
    BXPT_PCR_Handle *hPcr
    )
{
    BXPT_PCR_Handle     handle;
    BREG_Handle         hRegister = hXpt->hRegister;
    uint32_t            Reg;
    BERR_Code           ret = BERR_SUCCESS;

    BDBG_ASSERT( pcrSettings ); 
    if ( WhichPcr >= BXPT_NUM_PCRS )
    {
        ret = BERR_TRACE(BERR_INVALID_PARAMETER);
        BDBG_ERR(("Pcr number %d exceeds max PCR channels!",WhichPcr));
        return ret;
    }       
    /* allocate pcr channel handle */
    handle = (BXPT_P_PcrHandle_Impl * )BKNI_Malloc( sizeof(BXPT_P_PcrHandle_Impl));     
    if ( handle == NULL )
    {
        ret = BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
        return ret;
    }
        
    handle->hRegister = hXpt->hRegister;
    handle->ChannelNo = WhichPcr;
    handle->RegOffset = WhichPcr * BXPT_PCR_P_REGOFFSET;
    handle->DMode = false;
    handle->vhXpt = ( void * ) hXpt;
    
    hXpt->PcrHandles[WhichPcr] = handle;
    *hPcr = handle;

    /* set the default settings */
    BREG_Write32( hRegister, BCHP_XPT_DPCR0_MAX_PCR_ERROR + handle->RegOffset, pcrSettings->MaxPcrError);

    Reg = BREG_Read32(hRegister, BCHP_XPT_DPCR0_CTRL + handle->RegOffset);
    Reg &= ~(BCHP_MASK( XPT_DPCR0_CTRL, PCR_TWO_ERR_REACQUIRE_EN ));
    Reg |=  BCHP_FIELD_DATA( XPT_DPCR0_CTRL, PCR_TWO_ERR_REACQUIRE_EN, pcrSettings->PcrTwoErrReaquireEn); 
    BREG_Write32( hRegister, BCHP_XPT_DPCR0_CTRL + handle->RegOffset, Reg);

    /* This reg is shared by all DPCR instances, so init only on the first open. */
    if( !hXpt->DpcrRefCount++ ) 
    {
        BREG_Write32( hRegister, BCHP_XPT_DPCR_PP_PP_CTRL, 0 );
    }

#if BXPT_HAS_DPCR_INTEGRATOR_WORKAROUND
    handle->FiltB = RESET_FILTB;
    handle->FiltC = RESET_FILTC;
    handle->TrackRange = RESET_TRACK_RANGE;
    handle->Accum = 0;
    handle->InErrState = false;
    handle->PcrCount = 0;
    handle->PcrThreshold = 0;

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)
    handle->ChangingToNonPcrSource = false;
    handle->PhaseErrorDelay = 0;
    handle->PcrInc = 249856;
#endif

    Reg = BREG_Read32( handle->hRegister, BCHP_XPT_DPCR0_LOOP_CTRL + handle->RegOffset );
    Reg |= BCHP_FIELD_DATA( XPT_DPCR0_LOOP_CTRL, FREEZE_INTEGRATOR, 1 ); 
    BREG_Write32( handle->hRegister, BCHP_XPT_DPCR0_LOOP_CTRL + handle->RegOffset, Reg );
#endif
 
    BREG_Write32( handle->hRegister, BCHP_XPT_DPCR0_INTR_STATUS_REG, 0 );

    return ret;
}


BERR_Code BXPT_PCR_Close(
    BXPT_PCR_Handle hPcr
    )
{
    uint32_t      Reg;
    BERR_Code     ret = BERR_SUCCESS;
    BREG_Handle   hRegister = hPcr->hRegister;
    BXPT_Handle   lhXpt = ( BXPT_Handle ) hPcr->vhXpt;

    lhXpt->PcrHandles[ hPcr->ChannelNo ] = ( void * ) NULL;

    if( ! --lhXpt->DpcrRefCount ) 
    {
        BREG_Write32( hRegister, BCHP_XPT_DPCR_PP_PP_CTRL, 0 );
    }

    /* set pid_valid_bit to 0 in case it is still active*/
    Reg = BREG_Read32(hRegister, BCHP_XPT_DPCR0_PID_CH  + hPcr->RegOffset);
    Reg &= ~(BCHP_MASK( XPT_DPCR0_PID_CH , PCR_PID_CH_VALID)); 
    BREG_Write32( hRegister, BCHP_XPT_DPCR0_PID_CH + hPcr->RegOffset, Reg);

    BKNI_Free(hPcr);
    return ret;
}


BERR_Code   BXPT_PCR_GetStreamPcrConfig( 
    BXPT_PCR_Handle hPcr,                         
    BXPT_PCR_XptStreamPcrCfg *PcrCfg             
    )
{
    BREG_Handle         hRegister = hPcr->hRegister;
    BXPT_Handle hXpt = (BXPT_Handle) hPcr->vhXpt;
    uint32_t            Reg;
    uint32_t StreamSelect;
    BERR_Code           ret = BERR_SUCCESS;

    BDBG_ENTER(BXPT_PCR_GetStreamPcrConfig);

    BSTD_UNUSED( StreamSelect );

    PcrCfg->JitterTimestamp = hXpt->JitterTimestamp[ hPcr->ChannelNo ];
    PcrCfg->PbJitterDisable = hXpt->PbJitterDisable[ hPcr->ChannelNo ];
    PcrCfg->LiveJitterDisable = hXpt->LiveJitterDisable[ hPcr->ChannelNo ];

    Reg = BREG_Read32(hRegister, BCHP_XPT_DPCR0_PID_CH  + hPcr->RegOffset);
    PcrCfg->PidChannel = BCHP_GET_FIELD_DATA(Reg, XPT_DPCR0_PID_CH , PCR_PID_CH);
    PcrCfg->MaxPcrError = BREG_Read32( hRegister, BCHP_XPT_DPCR0_MAX_PCR_ERROR + hPcr->RegOffset );
    ret = BXPT_PCR_GetfreqRefConfig( hPcr, &PcrCfg->TimebaseCfg );

    BDBG_LEAVE(BXPT_PCR_GetStreamPcrConfig);

    return ret;
}

BERR_Code   BXPT_PCR_SetStreamPcrConfig( 
    BXPT_PCR_Handle hPcr,                               
    const BXPT_PCR_XptStreamPcrCfg *PcrCfg
    )
{
    BERR_Code ret = BERR_SUCCESS;
 
    BKNI_EnterCriticalSection();
    ret = BXPT_PCR_SetStreamPcrConfig_isr( hPcr, PcrCfg );
    BKNI_LeaveCriticalSection();
    return ret;
}

BERR_Code   BXPT_PCR_SetStreamPcrConfig_isr( 
    BXPT_PCR_Handle hPcr,                               
    const BXPT_PCR_XptStreamPcrCfg *PcrCfg
    )
{
    BREG_Handle         hRegister = hPcr->hRegister;
    BXPT_Handle hXpt = (BXPT_Handle) hPcr->vhXpt;
    uint32_t            Reg;
    BERR_Code           ret = BERR_SUCCESS;
    uint32_t            tempStreamSelect = 0;
    unsigned JitterTimestamp, PbJitterDisable, LiveJitterDisable;

    BDBG_ENTER(BXPT_PCR_SetStreamPcrConfig_isr);

    BSTD_UNUSED( tempStreamSelect );

    if( PcrCfg->JitterTimestamp >= BXPT_PCR_JitterTimestampMode_eMax ) 
    {
        BDBG_ERR(( "Invalid jitter timestamp mode %u", PcrCfg->JitterTimestamp ));
        ret = BERR_TRACE(BERR_INVALID_PARAMETER);
        goto Done;
    }

    Reg = BREG_Read32(hRegister, BCHP_XPT_DPCR_PP_PP_CTRL );
    JitterTimestamp = BCHP_GET_FIELD_DATA( Reg, XPT_DPCR_PP_PP_CTRL , PP_JITTER_TIMESTAMP_MODE );
    PbJitterDisable = BCHP_GET_FIELD_DATA( Reg, XPT_DPCR_PP_PP_CTRL , PP_PLAYBACK_PCR_JITTER_DIS );
    LiveJitterDisable = BCHP_GET_FIELD_DATA( Reg, XPT_DPCR_PP_PP_CTRL , PP_LIVE_PCR_JITTER_DIS );

    if( PcrCfg->JitterTimestamp != BXPT_PCR_JitterTimestampMode_eAuto )
    {
        JitterTimestamp = PcrCfg->JitterTimestamp;
    }
    hXpt->JitterTimestamp[ hPcr->ChannelNo ] = PcrCfg->JitterTimestamp;

    if( PcrCfg->PbJitterDisable != BXPT_PCR_JitterCorrection_eAuto )
    {
        PbJitterDisable = PcrCfg->PbJitterDisable;
    }
    hXpt->PbJitterDisable[ hPcr->ChannelNo ] = PcrCfg->PbJitterDisable;

    if( PcrCfg->LiveJitterDisable != BXPT_PCR_JitterCorrection_eAuto )
    {
        LiveJitterDisable = PcrCfg->LiveJitterDisable;
    }
    hXpt->LiveJitterDisable[ hPcr->ChannelNo ] = PcrCfg->LiveJitterDisable;

    Reg = BREG_Read32(hRegister, BCHP_XPT_DPCR_PP_PP_CTRL );
    Reg &= ~(
        BCHP_MASK( XPT_DPCR_PP_PP_CTRL , PP_JITTER_TIMESTAMP_MODE ) |
        BCHP_MASK( XPT_DPCR_PP_PP_CTRL , PP_PLAYBACK_PCR_JITTER_DIS ) |
        BCHP_MASK( XPT_DPCR_PP_PP_CTRL , PP_LIVE_PCR_JITTER_DIS )
        ); 
    Reg |= ( 
        BCHP_FIELD_DATA( XPT_DPCR_PP_PP_CTRL, PP_JITTER_TIMESTAMP_MODE, JitterTimestamp ) |
        BCHP_FIELD_DATA( XPT_DPCR_PP_PP_CTRL, PP_PLAYBACK_PCR_JITTER_DIS, PbJitterDisable ) |
        BCHP_FIELD_DATA( XPT_DPCR_PP_PP_CTRL, PP_LIVE_PCR_JITTER_DIS, LiveJitterDisable )
        );
    BREG_Write32( hRegister, BCHP_XPT_DPCR_PP_PP_CTRL, Reg );

    Reg = BREG_Read32(hRegister, BCHP_XPT_DPCR0_PID_CH  + hPcr->RegOffset);
    Reg &= ~(
        BCHP_MASK( XPT_DPCR0_PID_CH , PCR_PID_CH_VALID ) |
        BCHP_MASK( XPT_DPCR0_PID_CH , PCR_PID_CH )
        ); 
    Reg |= BCHP_FIELD_DATA( XPT_DPCR0_PID_CH, PCR_PID_CH, PcrCfg->PidChannel );
    BREG_Write32( hRegister, BCHP_XPT_DPCR0_PID_CH + hPcr->RegOffset, Reg);
    BREG_Write32( hRegister, BCHP_XPT_DPCR0_MAX_PCR_ERROR + hPcr->RegOffset, PcrCfg->MaxPcrError );
   
    /* Set the timebase select to xpt source */
    ret = BERR_TRACE( BXPT_PCR_P_Lock2Source( hPcr, BXPT_PCR_TimeRef_eXpt, NULL ));

#if BXPT_HAS_DPCR_INTEGRATOR_WORKAROUND
    hPcr->FiltB = PcrCfg->TimebaseCfg.FiltB;
    hPcr->FiltC = PcrCfg->TimebaseCfg.FiltC;
#endif

    Reg = BREG_Read32(hRegister, BCHP_XPT_DPCR0_LOOP_CTRL+ hPcr->RegOffset);
    Reg &= ~(
        BCHP_MASK( XPT_DPCR0_LOOP_CTRL, FILT_C ) |
        BCHP_MASK( XPT_DPCR0_LOOP_CTRL, FILT_B ) |
        BCHP_MASK( XPT_DPCR0_LOOP_CTRL, FILT_A )
    );
    Reg |= (
        BCHP_FIELD_DATA( XPT_DPCR0_LOOP_CTRL, FILT_C, PcrCfg->TimebaseCfg.FiltC ) |
        BCHP_FIELD_DATA( XPT_DPCR0_LOOP_CTRL, FILT_B, PcrCfg->TimebaseCfg.FiltB ) |
        BCHP_FIELD_DATA( XPT_DPCR0_LOOP_CTRL, FILT_A, PcrCfg->TimebaseCfg.FiltA )
    );
    BREG_Write32( hRegister, BCHP_XPT_DPCR0_LOOP_CTRL + hPcr->RegOffset, Reg);

    SetPcrIncAndPrescale( hPcr, &PcrCfg->TimebaseCfg );

    /* Set PCR Valid bit to 1 */
    Reg = BREG_Read32(hRegister, BCHP_XPT_DPCR0_PID_CH  + hPcr->RegOffset);
    Reg |= BCHP_FIELD_DATA( XPT_DPCR0_PID_CH, PCR_PID_CH_VALID, 1 );
    BREG_Write32( hRegister, BCHP_XPT_DPCR0_PID_CH + hPcr->RegOffset, Reg);

    Done:
    BDBG_LEAVE(BXPT_PCR_SetStreamPcrConfig_isr);

    return ret;
}

void BXPT_PCR_RefreshPcrPid( 
    BXPT_PCR_Handle hPcr              
    )
{
    BKNI_EnterCriticalSection();
    BXPT_PCR_RefreshPcrPid_isr(hPcr);
    BKNI_LeaveCriticalSection();
    return ;
}


void BXPT_PCR_RefreshPcrPid_isr( 
    BXPT_PCR_Handle hPcr               /*[in] The pcr handle  */
    )
{
    uint32_t Reg;
    
    Reg = BREG_Read32(hPcr->hRegister, BCHP_XPT_DPCR0_PID_CH  + hPcr->RegOffset);
    Reg &= ~(BCHP_MASK( XPT_DPCR0_PID_CH , PCR_PID_CH_VALID )); 
    Reg |= BCHP_FIELD_DATA( XPT_DPCR0_PID_CH, PCR_PID_CH_VALID, 1 );
    BREG_Write32( hPcr->hRegister, BCHP_XPT_DPCR0_PID_CH + hPcr->RegOffset, Reg);
    return;
}

BERR_Code   BXPT_PCR_GetLastPcr( 
    BXPT_PCR_Handle hPcr,             
    uint32_t *p_pcrHi,            
    uint32_t *p_pcrLo             
    )
{
    BERR_Code ret = BERR_SUCCESS;

    BKNI_EnterCriticalSection();
    ret = BXPT_PCR_GetLastPcr_isr(hPcr, p_pcrHi, p_pcrLo);
    BKNI_LeaveCriticalSection();
    return ret;
}

BERR_Code   BXPT_PCR_GetLastPcr_isr( 
    BXPT_PCR_Handle hPcr,             
    uint32_t *p_pcrHi,            
    uint32_t *p_pcrLo             
    )
{
    BREG_Handle         hRegister = hPcr->hRegister;
    uint32_t            Reg;
    BERR_Code           ret = BERR_SUCCESS;
    uint32_t            Offset = hPcr->RegOffset;
    bool                PacketMode;

    BDBG_ENTER(BXPT_PCR_GetLastPcr);
    
    Reg = BREG_Read32(hRegister, BCHP_XPT_DPCR0_CTRL + Offset);
    PacketMode = (bool) BCHP_GET_FIELD_DATA(Reg, XPT_DPCR0_CTRL, PCR_PACKET_MODE);
    if ( PacketMode )
    {
        BDBG_ERR(("Unsupported PI for this mode"));
        ret = BERR_TRACE(BERR_INVALID_PARAMETER);
        return ret;
    }   
    *p_pcrHi = BREG_Read32(hRegister,BCHP_XPT_DPCR0_LAST_PCR_HI + Offset);
    *p_pcrLo = BREG_Read32(hRegister,BCHP_XPT_DPCR0_LAST_PCR_LO + Offset);

    BDBG_LEAVE(BXPT_PCR_GetLastPcr);

    return ret;

}

BERR_Code   BXPT_PCR_GetPhaseError_isr( 
    BXPT_PCR_Handle hPcr,             
    int32_t *p_error
    )
{
    BREG_Handle         hRegister = hPcr->hRegister;
    uint32_t            Reg;
    bool                sign;
    uint32_t            magnitude;
    BERR_Code           ret = BERR_SUCCESS;
    uint32_t            Offset = hPcr->RegOffset;

    BDBG_ENTER(BXPT_PCR_GetPhaseError_isr);
    
    Reg = BREG_Read32(hRegister,BCHP_XPT_DPCR0_PHASE_ERROR + Offset);
    Reg = BCHP_GET_FIELD_DATA(Reg, XPT_DPCR0_PHASE_ERROR, PHASE_ERROR);

	/* stored as 2's complement */
    sign = (bool)((Reg & 0x100000) >> 20); /* sign bit is bit 20 */
        magnitude = Reg;
    if (sign) magnitude = ~Reg + 1; /* 2's complement */
    magnitude &= 0x0fffff; /* magnitude is 19 to 0 */

    /*  bits 10:1 of pcr base - stc base are stored in 19:10 here. 
	bit 9 has bit 0 of base diff. bits 8:0 have extension diff. */
	*p_error = (sign ? -1 : 1) * (magnitude >> 9);
    BDBG_LEAVE(BXPT_PCR_GetPhaseError_isr);

    return ret;

}


BERR_Code   BXPT_PCR_GetStc( 
    BXPT_PCR_Handle hPcr,            
    uint32_t *p_stcHi,          
    uint32_t *p_stcLo            
    )
{
    BREG_Handle         hRegister = hPcr->hRegister;
    uint32_t            Reg;
    BERR_Code           ret = BERR_SUCCESS;
    uint32_t            Offset = hPcr->RegOffset;
    bool                PacketMode;

    BDBG_ENTER(BXPT_PCR_GetStc);
    Reg = BREG_Read32(hRegister, BCHP_XPT_DPCR0_CTRL + Offset);
    PacketMode = (bool) BCHP_GET_FIELD_DATA(Reg, XPT_DPCR0_CTRL, PCR_PACKET_MODE);
    if ( PacketMode )
    {
        BDBG_ERR(("Unsupported PI for this mode"));
        ret = BERR_TRACE(BERR_INVALID_PARAMETER);
        return ret;
    }   
    *p_stcHi = BREG_Read32(hRegister,BCHP_XPT_DPCR0_STC_HI+Offset);
    *p_stcLo = BREG_Read32(hRegister,BCHP_XPT_DPCR0_STC_LO+Offset);
    BDBG_LEAVE(BXPT_PCR_GetStc);

    return ret;
}


BERR_Code   BXPT_PCR_SetStcExtRateControl( 
    BXPT_PCR_Handle hPcr,                                            
    const BXPT_PCR_STCExtRateConfig  *StcExtRateCfg 
    )
{

    uint32_t            Reg;
    uint32_t            Offset = hPcr->RegOffset;
    BERR_Code           ret = BERR_SUCCESS;
    BREG_Handle         hRegister = hPcr->hRegister;

    BDBG_ENTER(BXPT_PCR_SetStcExtRateControl);

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)
    BDBG_MSG(( "StcExtRateControl: Prescale %lu, Inc %lu", StcExtRateCfg->Prescale, StcExtRateCfg->Inc ));
#endif
    Reg = BREG_Read32(hRegister, BCHP_XPT_DPCR0_STC_EXT_CTRL+Offset);

    Reg &= ~(
        BCHP_MASK( XPT_DPCR0_STC_EXT_CTRL, PRESCALE ) |
        BCHP_MASK( XPT_DPCR0_STC_EXT_CTRL, INC_VAL )
        );

    Reg |= (
        BCHP_FIELD_DATA(XPT_DPCR0_STC_EXT_CTRL,PRESCALE,StcExtRateCfg->Prescale)|
        BCHP_FIELD_DATA(XPT_DPCR0_STC_EXT_CTRL,INC_VAL,StcExtRateCfg->Inc)
        );
    BREG_Write32( hRegister, BCHP_XPT_DPCR0_STC_EXT_CTRL+Offset, Reg);

    BDBG_LEAVE(BXPT_PCR_SetStcExtRateControl);

    return ret;
}

BERR_Code   BXPT_PCR_ConfigNonStreamTimeBase( 
    BXPT_PCR_Handle hPcr,                          
    BXPT_PCR_TimeRef  eNonStreamTimeBaseCfg,  
    BXPT_PCR_TimebaseFreqRefConfig *Timebasefreq
    )
{
    BERR_Code  ret = BERR_SUCCESS;

    BDBG_ENTER(BXPT_PCR_ConfigNonStreamTimeBase);
    BDBG_ASSERT( Timebasefreq );    
    if (eNonStreamTimeBaseCfg == BXPT_PCR_TimeRef_eXpt) 
    {
        ret = BERR_TRACE(BERR_INVALID_PARAMETER);
        goto Done;
    }   
        
    if ( eNonStreamTimeBaseCfg >= BXPT_PCR_TimeRef_eMax )
    {
        BDBG_ERR(( "Unsupported BXPT_PCR_TimeRef" )); 
        ret = BERR_TRACE(BERR_INVALID_PARAMETER);
        goto Done;
    }   
    BXPT_PCR_P_Lock2Source( hPcr,eNonStreamTimeBaseCfg, Timebasefreq);

    Done:
    BDBG_LEAVE(BXPT_PCR_ConfigNonStreamTimeBase);
    return ret;

}


BERR_Code   BXPT_PCR_GetTimeBaseFreqRefDefaults( 
    BXPT_PCR_Handle hPcr,                          
    BXPT_PCR_TimeRef TimeBase,  
    BXPT_PCR_TimebaseFreqRefConfig *Def
    )
{
    BERR_Code  ret = BERR_SUCCESS;

    BDBG_ENTER(BXPT_PCR_GetTimeBaseFreqRefDefaults);
    BDBG_ASSERT( Def ); 
    BSTD_UNUSED( hPcr );

    Def->Prescale = 0;  
    Def->Inc = 1;       
    Def->FiltA = RESET_FILTA;       
    Def->FiltB = RESET_FILTB;       
    Def->FiltC = RESET_FILTC;       

    switch( TimeBase )
    {
        case BXPT_PCR_TimeRef_eI656_Vl:         /* Lock to ITU 656 VSync */
        Def->Prescale = 0;  
        Def->Inc = 450450;      
        Def->FiltA = 7;     
        Def->FiltB = 4;     
        Def->FiltC = 1; 
        break;  

        case BXPT_PCR_TimeRef_eI656_Fl:         /* Lock to ITU 656 Frame Sync */
        Def->Prescale = 0;  
        Def->Inc = 900900;      
        Def->FiltA = 7;     
        Def->FiltB = 4;     
        Def->FiltC = 1; 
        break;  

	    case BXPT_PCR_TimeRef_eInternal:
        Def->Prescale = 243;  
        Def->Inc = 249856;      
        Def->FiltA = 7;     
        Def->FiltB = 4;     
        Def->FiltC = 1;
		break;

        default:    /* Use defaults set before the switch() */
        break;
    }

    BDBG_LEAVE(BXPT_PCR_GetTimeBaseFreqRefDefaults);

    return ret;
}                                        

BERR_Code   BXPT_PCR_GetfreqRefConfig( 
    BXPT_PCR_Handle hPcr,                               
    BXPT_PCR_TimebaseFreqRefConfig  *TimebaseFreqConfig 
    )
{
    uint32_t            Reg;
    BREG_Handle         hRegister = hPcr->hRegister;

    BERR_Code  ret = BERR_SUCCESS;

    BDBG_ENTER(BXPT_PCR_GetfreqRefConfig);

    GetPcrIncAndPrescale( hPcr, TimebaseFreqConfig );

    Reg = BREG_Read32( hRegister, BCHP_XPT_DPCR0_LOOP_CTRL + hPcr->RegOffset );
    TimebaseFreqConfig->FiltA = BCHP_GET_FIELD_DATA( Reg, XPT_DPCR0_LOOP_CTRL, FILT_A );
    TimebaseFreqConfig->FiltB = BCHP_GET_FIELD_DATA( Reg, XPT_DPCR0_LOOP_CTRL, FILT_B );
    TimebaseFreqConfig->FiltC = BCHP_GET_FIELD_DATA( Reg, XPT_DPCR0_LOOP_CTRL, FILT_C );

    BDBG_LEAVE(BXPT_PCR_GetfreqRefConfig);

    return ret;

}


void BXPT_PCR_FreezeIntegrator( 
    BXPT_PCR_Handle hPcr,    /* [in] The pcr handle  */
    bool Freeze              /* [in] Freeze integrator if true, run if false. */
    )
{
#if BXPT_HAS_DPCR_INTEGRATOR_WORKAROUND
    BDBG_MSG(( "BXPT_PCR_FreezeIntegrator() calls are ignored." ));
    BSTD_UNUSED(( hPcr ));
    BSTD_UNUSED(( Freeze ));
#else
    uint32_t Reg;
        
    Reg = BREG_Read32( hPcr->hRegister, BCHP_XPT_DPCR0_LOOP_CTRL + hPcr->RegOffset );

    Reg &= ~( BCHP_MASK( XPT_DPCR0_LOOP_CTRL, FREEZE_INTEGRATOR ));
    Reg |= BCHP_FIELD_DATA( XPT_DPCR0_LOOP_CTRL, FREEZE_INTEGRATOR, Freeze == true ? 1 : 0 ); 
    BREG_Write32( hPcr->hRegister, BCHP_XPT_DPCR0_LOOP_CTRL + hPcr->RegOffset, Reg );

    /* Freezing the integrator requires stopping PCR processing, so mark the PCR PID as invalid */
    Reg = BREG_Read32(hPcr->hRegister, BCHP_XPT_DPCR0_PID_CH  + hPcr->RegOffset);
    Reg &= ~(BCHP_MASK( XPT_DPCR0_PID_CH , PCR_PID_CH_VALID )); 
    Reg |= BCHP_FIELD_DATA( XPT_DPCR0_PID_CH, PCR_PID_CH_VALID, Freeze == true ? 0 : 1 );
    BREG_Write32( hPcr->hRegister, BCHP_XPT_DPCR0_PID_CH + hPcr->RegOffset, Reg);
#endif 
}

void BXPT_PCR_SetCenterFrequency( 
    BXPT_PCR_Handle hPcr,       /* [in] The pcr handle  */
    uint32_t CenterFreq         /* [in] Center frequency */
    )
{                                            
    uint32_t Reg;
        
    Reg = BREG_Read32( hPcr->hRegister, BCHP_XPT_DPCR0_CENTER + hPcr->RegOffset );

    Reg &= ~( BCHP_MASK( XPT_DPCR0_CENTER, CENTER ));
    Reg |= BCHP_FIELD_DATA( XPT_DPCR0_CENTER, CENTER, CenterFreq ); 

    BREG_Write32( hPcr->hRegister, BCHP_XPT_DPCR0_CENTER + hPcr->RegOffset, Reg );
}

void BXPT_PCR_SetTimeRefTrackRange( 
    BXPT_PCR_Handle hPcr,                          
    BXPT_PCR_RefTrackRange TrackRange
    )
{
    uint32_t Reg;
        
    Reg = BREG_Read32( hPcr->hRegister, BCHP_XPT_DPCR0_LOOP_CTRL + hPcr->RegOffset );

    Reg &= ~( BCHP_MASK( XPT_DPCR0_LOOP_CTRL, TRACK_RANGE ));
    Reg |= BCHP_FIELD_DATA( XPT_DPCR0_LOOP_CTRL, TRACK_RANGE, TrackRange ); 

    BREG_Write32( hPcr->hRegister, BCHP_XPT_DPCR0_LOOP_CTRL + hPcr->RegOffset, Reg );

}

void BXPT_PCR_SetPhaseErrorClampRange( 
	BXPT_PCR_Handle hPcr, 					       
	BXPT_PCR_PhaseErrorClampRange ClampRange
    )
{
    uint32_t Reg;
        
    Reg = BREG_Read32( hPcr->hRegister, BCHP_XPT_DPCR0_PHASE_ERROR_CLAMP + hPcr->RegOffset );
    Reg &= ~( BCHP_MASK( XPT_DPCR0_PHASE_ERROR_CLAMP, PHASE_ERROR_CLAMP_RANGE ));
    Reg |= BCHP_FIELD_DATA( XPT_DPCR0_PHASE_ERROR_CLAMP, PHASE_ERROR_CLAMP_RANGE, ClampRange ); 
    BREG_Write32( hPcr->hRegister, BCHP_XPT_DPCR0_PHASE_ERROR_CLAMP + hPcr->RegOffset, Reg );
}

static void SetPcrIncAndPrescale( 
    BXPT_PCR_Handle hPcr, 
    const BXPT_PCR_TimebaseFreqRefConfig *Timebasefreq
    )
{
    uint32_t Reg;
    BREG_Handle         hRegister = hPcr->hRegister;
   
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)
    BDBG_MSG(( "Prescale %lu, Inc %lu", Timebasefreq->Prescale, Timebasefreq->Inc ));
#endif
    Reg = BREG_Read32(hRegister, BCHP_XPT_DPCR0_REF_PCR_PRESCALE + hPcr->RegOffset);
    Reg &= ~(
        BCHP_MASK( XPT_DPCR0_REF_PCR_PRESCALE, PRESCALE ) 
        );
    Reg |= (
        BCHP_FIELD_DATA( XPT_DPCR0_REF_PCR_PRESCALE, PRESCALE, Timebasefreq->Prescale )
        );
    BREG_Write32( hRegister, BCHP_XPT_DPCR0_REF_PCR_PRESCALE + hPcr->RegOffset, Reg);                                                   

    Reg = BREG_Read32(hRegister, BCHP_XPT_DPCR0_REF_PCR_INC + hPcr->RegOffset);
    Reg &= ~(
        BCHP_MASK( XPT_DPCR0_REF_PCR_INC, PCR_INC )                
        );
    Reg |= (
        BCHP_FIELD_DATA( XPT_DPCR0_REF_PCR_INC, PCR_INC, Timebasefreq->Inc )
        );
    BREG_Write32( hRegister, BCHP_XPT_DPCR0_REF_PCR_INC + hPcr->RegOffset, Reg);
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)
#if BXPT_HAS_DPCR_INTEGRATOR_WORKAROUND
    hPcr->PcrInc = Timebasefreq->Inc;
#endif
#endif
}

static void GetPcrIncAndPrescale(
    BXPT_PCR_Handle hPcr, 
    BXPT_PCR_TimebaseFreqRefConfig  *TimebaseFreqConfig 
    )
{
    uint32_t            Reg;
    BREG_Handle         hRegister = hPcr->hRegister;

    Reg = BREG_Read32(hRegister, BCHP_XPT_DPCR0_REF_PCR_INC + hPcr->RegOffset);
    TimebaseFreqConfig->Inc =  BCHP_GET_FIELD_DATA( Reg, XPT_DPCR0_REF_PCR_INC,PCR_INC );
    Reg = BREG_Read32(hRegister, BCHP_XPT_DPCR0_REF_PCR_PRESCALE + hPcr->RegOffset);
    TimebaseFreqConfig->Prescale = BCHP_GET_FIELD_DATA( Reg, XPT_DPCR0_REF_PCR_PRESCALE, PRESCALE );
}


BERR_Code BXPT_PCR_GetIntId(
    unsigned WhichPcr, 
    BXPT_PCR_IntName Name,
    BINT_Id *IntId
    )
{
    uint32_t RegAddr;   
    BERR_Code ExitCode = BERR_SUCCESS;

    BDBG_ASSERT( IntId ); 

    if(WhichPcr >= BXPT_NUM_PCRS) 
    {
        BDBG_ERR(("Pcr number %u exceeds max PCR channels (%u)!",WhichPcr, BXPT_NUM_PCRS));
        ExitCode = BERR_TRACE(BERR_INVALID_PARAMETER);
        goto Done;
    }

    switch( Name ) 
    {
        case BXPT_PCR_IntName_ePhaseCompare:
        case BXPT_PCR_IntName_eTwoPcrErrors:
        RegAddr = BCHP_XPT_DPCR0_INTR_STATUS_REG + ( WhichPcr * BXPT_PCR_P_REGOFFSET );
        *IntId = BCHP_INT_ID_CREATE( RegAddr, Name );
        break;
    
        default:
        BDBG_ERR(( "Unsupported interrupt enum %u", (unsigned) Name ));
        ExitCode = BERR_TRACE(BERR_INVALID_PARAMETER);
        break;
    }

    Done:
    return( ExitCode );
}

#if BXPT_HAS_DPCR_INTEGRATOR_WORKAROUND

static int64_t Saturate38(
    int64_t Input,
    unsigned OutputSizeSelect
    )
{
    int64_t Output = 0;
    int64_t Input37 = (Input & ((int64_t) 1 << 37)) >> 37;
    int64_t OnesCheck = 0;
    int64_t OnesSaturation = 0;
    int64_t ZerosSaturation = 0;
    int64_t MsbMask = 0;
    int64_t LsbMask = 0;
    
    switch( OutputSizeSelect )
    {
        case 0:     /* saturate the 38-bit input to a 31-bit two's complement number */
        OnesCheck =       (int64_t)(0x1FC000) << 16;       /* Bits 36:30 are 1 */
        OnesSaturation =  (int64_t)(0xFFFFFFFF) << 32 | 0xC0000000;    /* Was (0x0FC000) << 16 */
        ZerosSaturation = (int64_t)(0x003FFF) << 16 | 0xFFFF;
        MsbMask =         (int64_t)(0xFFFFFFFF) << 32; 
        LsbMask =         (int64_t)(0x003FFF) << 16 | 0xFFFF;        
        break;

        case 1:     /* saturate the 38-bit input to a 32-bit two's complement number */
        OnesCheck =       (int64_t)(0x1F8000) << 16;       /* Bits 36:31 are 1 */
        OnesSaturation =  (int64_t)(0xFFFFFFFF) << 32 | 0x80000000;   /* Was (0x0F8000) << 16 */
        ZerosSaturation = (int64_t)(0x007FFF) << 16 | 0xFFFF;
        MsbMask =         (int64_t)(0xFFFFFFFE) << 32; 
        LsbMask =         (int64_t)(0x007FFF) << 16 | 0xFFFF;        
        break;

        case 2:     /* saturate the 38-bit input to a 33-bit two's complement number */
        OnesCheck =       (int64_t)(0x1F0000) << 16;       /* Bits 36:32 are 1 */
        OnesSaturation =  (int64_t)(0xFFFFFFFF) << 32;        /* Was (0x0F0000) << 16 */
        ZerosSaturation = (int64_t)(0x00FFFF) << 16 | 0xFFFF;
        MsbMask =         (int64_t)(0xFFFFFFFC) << 32; 
        LsbMask =         (int64_t)(0x00FFFF) << 16 | 0xFFFF;        
        break;

        case 3:     /* saturate the 38-bit input to a 34-bit two's complement number */
        OnesCheck =       (int64_t)(0x1E0000) << 16;       /* Bits 36:33 are 1 */
        OnesSaturation =  (int64_t)(0xFFFFFFFE) << 32;        /* Was (0x0E0000) << 16. Need to extend sign to 64 bits */
        ZerosSaturation = (int64_t)(0x01FFFF) << 16 | 0xFFFF;
        MsbMask =         (int64_t)(0xFFFFFFF8) << 32; 
        LsbMask =         (int64_t)(0x01FFFF) << 16 | 0xFFFF;        
        break;

        case 4:     /* saturate the 38-bit input to a 35-bit two's complement number */
        OnesCheck =       (int64_t)(0x1C0000) << 16;       
        OnesSaturation =  (int64_t)(0xFFFFFFFC) << 32;      /* Was (0x0C0000) << 16. */
        ZerosSaturation = (int64_t)(0x03FFFF) << 16 | 0xFFFF;
        MsbMask =         (int64_t)(0xFFFFFFF0) << 32; 
        LsbMask =         (int64_t)(0x03FFFF) << 16 | 0xFFFF;        
        break;

        case 5:     /* Use as a hook for 38-bit to 36-bit. */
        default:    /* All other values, saturate the 38-bit input to a 36-bit two's complement number */
        OnesCheck =       (int64_t)(0x180000) << 16;       /* Bits 36:35 are 1 */
        OnesSaturation =  (int64_t)(0xFFFFFFF8) << 32;       /* Was (0x080000) << 16 */
        ZerosSaturation = (int64_t)(0x07FFFF) << 16 | 0xFFFF;
        MsbMask =         (int64_t)(0xFFFFFFE0) << 32;    
        LsbMask =         (int64_t)(0x07FFFF) << 16 | 0xFFFF;
        break;
    }
   
    if( (!Input37 && (Input & OnesCheck)) || (Input37 && ( (Input & OnesCheck) != OnesCheck)) )
    {
        /* Do saturation */
        if( Input37 )
        {
            Output = OnesSaturation;
        }
        else
        {
            Output = ZerosSaturation;
        }
    }
    else
    {
        /* No saturation */
        Output = (Input & MsbMask) >> 2;
        Output |= Input & LsbMask;
    }

    return Output;
}

static int64_t CorrectLeakyIntegrator(
    BXPT_PCR_Handle hPcr,
    int64_t PcrThreshold,
    int64_t Accum,
	uint8_t TrackRange,
	uint8_t FiltB,
	uint8_t FiltC
    )
{
    int64_t NewAccum, PcrCTemp, PcrB, PcrB_PreSat, PcrB2, PcrCNew, PcrC2, PcrC3, PcrC4;

    if( PcrThreshold & (1 << 20) )
    {
        PcrThreshold |= (int64_t)(0xFFFFFFFF) << 32; 
        PcrThreshold |= (int64_t) 0xFFF00000;
    }
    BDBG_MSG(( "DPCR%u: Accum %lld, PHASE_ERROR %lld (0x%08lX)", hPcr->ChannelNo, hPcr->Accum, PcrThreshold, PcrThreshold ));

    PcrB = PcrThreshold << (15 - FiltB);   /* hPcr->FiltB is gainB */   
    PcrB_PreSat =  PcrB << 2;
    PcrB2 = Saturate38(PcrB_PreSat, 5); /* TrackRange == 5 forces 38-bit to 36-bit */     
    BDBG_MSG(( "DPCR%u: PcrB %lld, PcrB_PreSat %lld, PcrB2 %lld", hPcr->ChannelNo, PcrB, PcrB_PreSat, PcrB2 ));

    PcrCTemp = Accum << 5;
    PcrC2 = PcrCTemp >> (8 + FiltC);
    if( 7 == FiltC )
    {
        PcrC3 = PcrCTemp;
    }
    else
    {
        int64_t PcrC2_28_0 = PcrC2; /*  & 0x1FFFFFFF;*/
        PcrC3 = PcrCTemp - PcrC2_28_0;
    }
    BDBG_MSG(( "DPCR%u: PcrC2 %lld, PcrC3 %lld", hPcr->ChannelNo, PcrC2, PcrC3 ));

    PcrC4 = (int64_t) Saturate38( PcrC3, (uint64_t) TrackRange );
    PcrCNew = PcrB2 + PcrC4;
    NewAccum = PcrCNew;
    BREG_Write32( hPcr->hRegister, BCHP_XPT_DPCR0_ACCUM_VALUE + hPcr->RegOffset, NewAccum >> 5);

    BDBG_MSG(( "DPCR%u: PcrC4 %lld, Accum(new) %lld, Accum(new) >> 5 %lld, ACCUM reg 0x%08lX", 
        hPcr->ChannelNo,
        PcrC4, Accum, NewAccum >> 5, 
        BREG_Read32( hPcr->hRegister, BCHP_XPT_DPCR0_ACCUM_VALUE + hPcr->RegOffset) ));
    return NewAccum >> 5;
}

BERR_Code BXPT_PCR_P_Integrator( 
    BXPT_Handle hXpt
    )
{
    int PcrNum;

    BERR_Code ExitCode = BERR_SUCCESS;

    BDBG_ASSERT( hXpt ); 

    for( PcrNum = 0; PcrNum < BXPT_NUM_PCRS && hXpt->PcrHandles[ PcrNum ]; PcrNum++ )
    {
        int64_t PcrThreshold;
        uint32_t Reg;
        unsigned PcrCount;

        bool PcrErrSeen = false;
        BXPT_PCR_Handle hPcr = hXpt->PcrHandles[ PcrNum ];

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)
#if DEBUG_DPCR_INTEGRATOR_WORKAROUND
        /* Don't run the workaround. Instead, just print these values: ACCUM_VALUE, PHASE_ERROR, PCR_COUNT, INTR_STATUS_REG */
        BDBG_MSG(( "DPCR%u ACCUM: 0x%08lX, PHASE_ERROR: 0x%08lX, PCR_COUNT 0x%08lX, INTR_STATUS_REG 0x%08lX", PcrNum, 
            BREG_Read32( hPcr->hRegister, BCHP_XPT_DPCR0_ACCUM_VALUE + hPcr->RegOffset), 
            BCHP_GET_FIELD_DATA( BREG_Read32( hPcr->hRegister, BCHP_XPT_DPCR0_PHASE_ERROR + hPcr->RegOffset), XPT_DPCR0_PHASE_ERROR, PHASE_ERROR),
            BCHP_GET_FIELD_DATA( BREG_Read32( hPcr->hRegister, BCHP_XPT_DPCR0_PCR_COUNT + hPcr->RegOffset), XPT_DPCR0_PCR_COUNT, PCR_COUNT),
            BREG_Read32( hPcr->hRegister, BCHP_XPT_DPCR0_INTR_STATUS_REG + hPcr->RegOffset) 
        ));
#else
        if( hPcr->ChangingToNonPcrSource )
        {   
            BDBG_MSG(( "Changing to non-PCR source" ));
            hPcr->ChangingToNonPcrSource = false;
            hPcr->PhaseErrorDelay = (long) ((2 * hPcr->PcrInc * 38) / 1000); /* PhaseErrorDelay should be in mS */
        }
        if( hPcr->PhaseErrorDelay )
        {
            /* This is based on the Nexus thread calling once every 2 mS */
            hPcr->PhaseErrorDelay -= 2;
            if( hPcr->PhaseErrorDelay <= 0 )
            {
                BDBG_MSG(( "Finished change to non-PCR source" ));
                hPcr->PhaseErrorDelay = 0;
            }
            return ExitCode;
        }                           
        
        /* Note: this number is stored in hw as 2's complement format. See BXPT_PCR_GetPhaseError_isr() */
        Reg = BREG_Read32( hPcr->hRegister, BCHP_XPT_DPCR0_PHASE_ERROR + hPcr->RegOffset );
        PcrThreshold = BCHP_GET_FIELD_DATA(Reg, XPT_DPCR0_PHASE_ERROR, PHASE_ERROR);
        BDBG_MSG(( "DPCR%u latest PHASE_ERROR %lld (0x%08lX), Saved PHASE_ERROR %lld (0x%08lX)",  
            PcrNum, PcrThreshold, PcrThreshold, hPcr->PcrThreshold, hPcr->PcrThreshold ));    

        Reg = BREG_Read32( hPcr->hRegister, BCHP_XPT_DPCR0_PCR_COUNT + hPcr->RegOffset );
        BDBG_MSG(( "DPCR%u latest PCR_COUNT: 0x%08lX", PcrNum, Reg ));
        PcrCount = BCHP_GET_FIELD_DATA(Reg, XPT_DPCR0_PCR_COUNT, PCR_COUNT);

        /* Don't use the current PHASE_ERROR if a PCR ERROR occurred. */
        Reg = BREG_Read32( hPcr->hRegister, BCHP_XPT_DPCR0_INTR_STATUS_REG + hPcr->RegOffset );
        BDBG_MSG(( "DPCR%u INTR_STATUS_REG: 0x%08lX", PcrNum, Reg ));
        PcrErrSeen = BCHP_GET_FIELD_DATA(Reg, XPT_DPCR0_INTR_STATUS_REG, ONE_PCR_ERROR ) 
            || BCHP_GET_FIELD_DATA(Reg, XPT_DPCR0_INTR_STATUS_REG, TWO_PCR_ERROR );

        if( hPcr->InErrState )
        {
            /* Leave the error state if we've seen at least 2 PCRs since we entered */
            if( PcrCount >= (hPcr->PcrCount + 2) % 256)
            {
                BDBG_MSG(( "DPCR%u leaving PCR ERROR state", PcrNum ));
                Reg = BREG_Read32( hPcr->hRegister, BCHP_XPT_DPCR0_INTR_STATUS_REG + hPcr->RegOffset );
                Reg &= ~(
                    BCHP_MASK( XPT_DPCR0_INTR_STATUS_REG, ONE_PCR_ERROR ) |
                    BCHP_MASK( XPT_DPCR0_INTR_STATUS_REG, TWO_PCR_ERROR ) 
                );
                BREG_Write32( hPcr->hRegister, BCHP_XPT_DPCR0_INTR_STATUS_REG + hPcr->RegOffset, Reg ); 
                hPcr->InErrState = false;
            }
            else
            {
                PcrThreshold = hPcr->PcrThreshold;  /* Use the last valid PHASE_ERROR */
            }
        }
        else if( PcrErrSeen && !hPcr->InErrState )
        {
            BDBG_MSG(( "DPCR%u entering PCR ERROR state", PcrNum ));
            hPcr->InErrState = true;
            hPcr->PcrCount = PcrCount;
            PcrThreshold = hPcr->PcrThreshold;  /* Use the last valid PHASE_ERROR */
        }

        if( !hPcr->InErrState )
        {
            hPcr->PcrThreshold = PcrThreshold;  /* Save this PHASE_ERROR in case we get a PCR_ERROR before the next execution. */
        }    

        if( !hPcr->Accum && !PcrThreshold )
        {
            BDBG_MSG(( "No integrator correction done for DPCR%u", PcrNum ));
            continue; /* Don't bother when both the last Accum and the current Phase Error are 0 */
        }

        hPcr->Accum = CorrectLeakyIntegrator( hPcr, PcrThreshold, hPcr->Accum, hPcr->TrackRange, 
            hPcr->FiltB, hPcr->FiltC );
#endif /* DEBUG_DPCR_INTEGRATOR_WORKAROUND */
#else
        /* Note: this number is stored in hw as 2's complement format. See BXPT_PCR_GetPhaseError_isr() */
        Reg = BREG_Read32( hPcr->hRegister, BCHP_XPT_DPCR0_PHASE_ERROR + hPcr->RegOffset );
        PcrThreshold = BCHP_GET_FIELD_DATA(Reg, XPT_DPCR0_PHASE_ERROR, PHASE_ERROR);
        BDBG_MSG(( "DPCR%u latest PHASE_ERROR %lld (0x%08lX), Saved PHASE_ERROR %lld (0x%08lX)",  
            PcrNum, PcrThreshold, PcrThreshold, hPcr->PcrThreshold, hPcr->PcrThreshold ));    

        Reg = BREG_Read32( hPcr->hRegister, BCHP_XPT_DPCR0_PCR_COUNT + hPcr->RegOffset );
        BDBG_MSG(( "DPCR%u latest PCR_COUNT: 0x%08lX", PcrNum, Reg ));
        PcrCount = BCHP_GET_FIELD_DATA(Reg, XPT_DPCR0_PCR_COUNT, PCR_COUNT);

        /* Don't use the current PHASE_ERROR if a PCR ERROR occurred. */
        Reg = BREG_Read32( hPcr->hRegister, BCHP_XPT_DPCR0_INTR_STATUS_REG + hPcr->RegOffset );
        BDBG_MSG(( "DPCR%u INTR_STATUS_REG: 0x%08lX", PcrNum, Reg ));
        PcrErrSeen = BCHP_GET_FIELD_DATA(Reg, XPT_DPCR0_INTR_STATUS_REG, ONE_PCR_ERROR ) 
            || BCHP_GET_FIELD_DATA(Reg, XPT_DPCR0_INTR_STATUS_REG, TWO_PCR_ERROR );

        if( hPcr->InErrState )
        {
            /* Leave the error state if we've seen at least 2 PCRs since we entered */
            if( PcrCount >= (hPcr->PcrCount + 2) % 256)
            {
                BDBG_MSG(( "DPCR%u leaving PCR ERROR state", PcrNum ));
                Reg = BREG_Read32( hPcr->hRegister, BCHP_XPT_DPCR0_INTR_STATUS_REG + hPcr->RegOffset );
                Reg &= ~(
                    BCHP_MASK( XPT_DPCR0_INTR_STATUS_REG, ONE_PCR_ERROR ) |
                    BCHP_MASK( XPT_DPCR0_INTR_STATUS_REG, TWO_PCR_ERROR ) 
                );
                BREG_Write32( hPcr->hRegister, BCHP_XPT_DPCR0_INTR_STATUS_REG + hPcr->RegOffset, Reg ); 
                hPcr->InErrState = false;
            }
            else
            {
                PcrThreshold = hPcr->PcrThreshold;  /* Use the last valid PHASE_ERROR */
            }
        }
        else if( PcrErrSeen && !hPcr->InErrState )
        {
            BDBG_MSG(( "DPCR%u entering PCR ERROR state", PcrNum ));
            hPcr->InErrState = true;
            hPcr->PcrCount = PcrCount;
            PcrThreshold = hPcr->PcrThreshold;  /* Use the last valid PHASE_ERROR */
        }

        if( !hPcr->InErrState )
        {
            hPcr->PcrThreshold = PcrThreshold;  /* Save this PHASE_ERROR in case we get a PCR_ERROR before the next execution. */
        }    

        if( !hPcr->Accum && !PcrThreshold )
        {
            BDBG_MSG(( "No integrator correction done for DPCR%u", PcrNum ));
            continue; /* Don't bother when both the last Accum and the current Phase Error are 0 */
        }

        hPcr->Accum = CorrectLeakyIntegrator( hPcr, PcrThreshold, hPcr->Accum, hPcr->TrackRange, 
            hPcr->FiltB, hPcr->FiltC );
#endif
    }

    return ExitCode;
}

#endif
