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
 * $brcm_Revision: Hydra_Software_Devel/48 $
 * $brcm_Date: 5/21/12 3:10p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/xpt/7400/bxpt_pcr.c $
 * 
 * Hydra_Software_Devel/48   5/21/12 3:10p gmullen
 * SW7401-3748: Added runtime param check for PCR_INC and PRESCALE
 * 
 * Hydra_Software_Devel/47   2/2/12 5:34p gmullen
 * SW7420-2236: Merged to Hydra
 * 
 * Hydra_Software_Devel/SW7420-2236/1   2/2/12 5:30p gmullen
 * SW7420-2236: Changed to runtime checks for valid timerefs
 * 
 * Hydra_Software_Devel/46   9/12/11 5:03p gmullen
 * SW7408-284: Merged changes to Hydra
 * 
 * Hydra_Software_Devel/SW7408-284/2   9/7/11 4:35p gmullen
 * SW7408-284: Changed jitter settings to an array
 * 
 * Hydra_Software_Devel/SW7408-284/1   9/7/11 1:51p gmullen
 * SW7408-284: Changed jitter adjust API to 'tri-date'
 * 
 * Hydra_Software_Devel/45   8/31/11 2:28p gmullen
 * SW7408-284: Fixed bad arg in call to BREG_Write32
 * 
 * Hydra_Software_Devel/44   5/24/11 10:17a gmullen
 * SW7408-284: Fixed compiler complaint about Done label
 * 
 * Hydra_Software_Devel/43   5/16/11 4:52p gmullen
 * SW7408-284: Added support for jitter adjust in PCR hw to PI
 * 
 * Hydra_Software_Devel/42   1/18/11 11:36a rjlewis
 * SW7420-1383: Added support for BDBG_OBJECT interface
 * 
 * Hydra_Software_Devel/41   11/9/10 8:37a gmullen
 * SW7422-20: Fixed compilation error for legacy chips
 * 
 * Hydra_Software_Devel/40   11/8/10 4:02p gmullen
 * SW7422-20: PI can now generate the required INT IDs
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

#if BXPT_P_MAX_PCRS > 1 
#include "bchp_xpt_dpcr1.h"
#endif

#include "bxpt_priv.h"
  
BDBG_MODULE( xpt_pcr );


#define XPR_PCR_DEF_MAX_PCR_ERROR  100  

/* Support for automatically scaling the number of DPCR blocks. */
#if BXPT_DPCR_GLOBAL_PACKET_PROC_CTRL
    #include "bchp_xpt_dpcr_pp.h"

    #ifdef BCHP_XPT_DPCR1_PID_CH
    #define BXPT_PCR_P_REGOFFSET  (BCHP_XPT_DPCR1_PID_CH - BCHP_XPT_DPCR0_PID_CH)
    #else
    #define BXPT_PCR_P_REGOFFSET  (BCHP_XPT_DPCR0_PHASE_ERROR_CLAMP - BCHP_XPT_DPCR0_PID_CH)
    #endif
#else
    #ifdef BCHP_XPT_DPCR1_PID
    #define BXPT_PCR_P_REGOFFSET  (BCHP_XPT_DPCR1_PID - BCHP_XPT_DPCR0_PID)
    #else
    #define BXPT_PCR_P_REGOFFSET  (BCHP_XPT_DPCR0_SOFT_PCR_EXT - BCHP_XPT_DPCR0_PID)
    #endif
#endif

#define RESET_FILTA   7 
#define RESET_FILTB   8
#define RESET_FILTC   3 

static BERR_Code BXPT_PCR_P_Lock2Source( BXPT_PCR_Handle hPcr, BXPT_PCR_TimeRef eTimeRef, BXPT_PCR_TimebaseFreqRefConfig *Timebasefreq );
static BERR_Code SetPcrIncAndPrescale( BXPT_PCR_Handle hPcr, const BXPT_PCR_TimebaseFreqRefConfig *Timebasefreq );
static void GetPcrIncAndPrescale( BXPT_PCR_Handle hPcr, BXPT_PCR_TimebaseFreqRefConfig  *TimebaseFreqConfig );


static BERR_Code BXPT_PCR_P_Lock2Source(
    BXPT_PCR_Handle hPcr, 
    BXPT_PCR_TimeRef eTimeRef,
    BXPT_PCR_TimebaseFreqRefConfig *Timebasefreq
    )
{
    uint32_t      Reg;
    uint8_t     FiltA, FiltB, FiltC;
    BERR_Code     ret = BERR_SUCCESS;
    BREG_Handle   hRegister = hPcr->hRegister;

    if( eTimeRef >= BXPT_PCR_TimeRef_eHD_DVI_V0 )
    {
        BDBG_ERR(("Unsupported eTimeRef %u!", eTimeRef ));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    /* Always allow the PRESCALE and PCR_INC to be adjusted */
    if (Timebasefreq && eTimeRef != BXPT_PCR_TimeRef_eXpt)
    {
        ret = SetPcrIncAndPrescale( hPcr, Timebasefreq );
        if( BERR_SUCCESS != ret )
          return BERR_TRACE( ret );
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
    
        /* Set Ref_Polarity to 1*/ 
        Reg = BREG_Read32(hRegister, BCHP_XPT_DPCR0_LOOP_CTRL+ hPcr->RegOffset);
        Reg &= ~(BCHP_MASK( XPT_DPCR0_LOOP_CTRL, REF_POLARITY ));
        Reg |= BCHP_FIELD_DATA( XPT_DPCR0_LOOP_CTRL, REF_POLARITY, 1);
        BREG_Write32( hRegister, BCHP_XPT_DPCR0_LOOP_CTRL + hPcr->RegOffset, Reg);
    }

    return ret;
}


BERR_Code BXPT_PCR_GetTotalChannels(
    BXPT_Handle hXpt,           
    unsigned int *TotalChannels     
    )
{
    BERR_Code  ret = BERR_SUCCESS;

    BDBG_OBJECT_ASSERT(hXpt, bxpt_t);

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

    BDBG_OBJECT_ASSERT(hXpt, bxpt_t);

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

    BDBG_OBJECT_ASSERT(hXpt, bxpt_t);
    BDBG_ASSERT( pcrSettings ); 

    if ( WhichPcr >= hXpt->MaxPcrs )
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

	BDBG_OBJECT_SET(handle, bxpt_t);

    *hPcr = handle;

    /* set the default settings */
    BREG_Write32( hRegister, BCHP_XPT_DPCR0_MAX_PCR_ERROR + handle->RegOffset, pcrSettings->MaxPcrError);

    Reg = BREG_Read32(hRegister, BCHP_XPT_DPCR0_CTRL + handle->RegOffset);
    Reg &= ~(BCHP_MASK( XPT_DPCR0_CTRL, PCR_TWO_ERR_REACQUIRE_EN ));
    Reg |=  BCHP_FIELD_DATA( XPT_DPCR0_CTRL, PCR_TWO_ERR_REACQUIRE_EN, pcrSettings->PcrTwoErrReaquireEn); 
    BREG_Write32( hRegister, BCHP_XPT_DPCR0_CTRL + handle->RegOffset, Reg);

#if BXPT_DPCR_GLOBAL_PACKET_PROC_CTRL
    /* This reg is shared by all DPCR instances, so init only on the first open. */
    if( !hXpt->DpcrRefCount++ ) 
    {
        BREG_Write32( hRegister, BCHP_XPT_DPCR_PP_PP_CTRL, 0 );
    }
#endif

    return ret;
}


BERR_Code BXPT_PCR_Close(
    BXPT_PCR_Handle hPcr
    )
{
    uint32_t      Reg;
    BERR_Code     ret = BERR_SUCCESS;
    BREG_Handle   hRegister;
    BXPT_Handle   lhXpt;

    BDBG_OBJECT_ASSERT(hPcr, bxpt_t);

    hRegister = hPcr->hRegister;
    lhXpt = ( BXPT_Handle ) hPcr->vhXpt;

    lhXpt->PcrHandles[ hPcr->ChannelNo ] = ( void * ) NULL;

#if BXPT_DPCR_GLOBAL_PACKET_PROC_CTRL
    if( ! --lhXpt->DpcrRefCount ) 
    {
        BREG_Write32( hRegister, BCHP_XPT_DPCR_PP_PP_CTRL, 0 );
    }

    /* set pid_valid_bit to 0 in case it is still active*/
    Reg = BREG_Read32(hRegister, BCHP_XPT_DPCR0_PID_CH  + hPcr->RegOffset);
    Reg &= ~(BCHP_MASK( XPT_DPCR0_PID_CH , PCR_PID_CH_VALID)); 
    BREG_Write32( hRegister, BCHP_XPT_DPCR0_PID_CH + hPcr->RegOffset, Reg);
#else
    /* set pid_valid_bit to 0 in case it is still active*/
    Reg = BREG_Read32(hRegister, BCHP_XPT_DPCR0_PID  + hPcr->RegOffset);
    Reg &= ~(BCHP_MASK( XPT_DPCR0_PID , PCR_PID_VALID)); 
    BREG_Write32( hRegister, BCHP_XPT_DPCR0_PID + hPcr->RegOffset, Reg);
#endif

	BDBG_OBJECT_DESTROY(hPcr, bxpt_t);
    BKNI_Free(hPcr);
    return ret;
}


BERR_Code   BXPT_PCR_GetStreamPcrConfig( 
    BXPT_PCR_Handle hPcr,                         
    BXPT_PCR_XptStreamPcrCfg *PcrCfg             
    )
{
    BREG_Handle         hRegister;
    uint32_t            Reg;
    uint32_t            StreamSelect;
    BERR_Code           ret = BERR_SUCCESS;

    BDBG_ENTER(BXPT_PCR_GetStreamPcrConfig);
    BDBG_OBJECT_ASSERT(hPcr, bxpt_t);

    hRegister = hPcr->hRegister;

#if BXPT_DPCR_GLOBAL_PACKET_PROC_CTRL
    BSTD_UNUSED( StreamSelect );

    Reg = BREG_Read32(hRegister, BCHP_XPT_DPCR0_PID_CH  + hPcr->RegOffset);
    PcrCfg->PidChannel = BCHP_GET_FIELD_DATA(Reg, XPT_DPCR0_PID_CH , PCR_PID_CH);

    {
        BXPT_Handle hXpt = (BXPT_Handle) hPcr->vhXpt;

        PcrCfg->JitterTimestamp = hXpt->JitterTimestamp[ hPcr->ChannelNo ];
        PcrCfg->PbJitterDisable = hXpt->PbJitterDisable[ hPcr->ChannelNo ];
        PcrCfg->LiveJitterDisable = hXpt->LiveJitterDisable[ hPcr->ChannelNo ];
    }
#else
    Reg = BREG_Read32(hRegister, BCHP_XPT_DPCR0_PID + hPcr->RegOffset);
    PcrCfg->Pid = BCHP_GET_FIELD_DATA(Reg, XPT_DPCR0_PID, PCR_PID);

    Reg = BREG_Read32(hRegister, BCHP_XPT_DPCR0_CTRL + hPcr->RegOffset);
    StreamSelect = BCHP_GET_FIELD_DATA( Reg, XPT_DPCR0_CTRL, PCR_STREAM_SELECT );
    if( StreamSelect <= BXPT_P_MAX_INPUT_BANDS )
    {
        PcrCfg->eStreamSelect = BXPT_DataSource_eInputBand;
        PcrCfg->WhichSource  = StreamSelect;
    }

    #if ( BCHP_CHIP == 7401 ) && ( BCHP_VER >= BCHP_VER_B0 ) \
    || ( BCHP_CHIP == 7118 ) || ( BCHP_CHIP == 7440 ) || ( BCHP_CHIP == 7400 ) || ( BCHP_CHIP == 7405 ) \
    || ( BCHP_CHIP == 7325 ) || ( BCHP_CHIP == 7335 ) || ( BCHP_CHIP == 7336  ) || ( BCHP_CHIP == 3556 ) || ( BCHP_CHIP == 3548 ) || ( BCHP_CHIP == 7342 )  || ( BCHP_CHIP == 7125) || (BCHP_CHIP == 7340 )
        /* Playback channels start at 8 in the PCR select register */
    else if( StreamSelect <= BXPT_P_MAX_PLAYBACKS + 8 )
    {
        PcrCfg->eStreamSelect = BXPT_DataSource_ePlayback;
        PcrCfg->WhichSource = StreamSelect - 8;
    }
    #endif

    else
    {
        BDBG_ERR(( "Invalid PCR_STREAM_SELECT! "));

        /* Default to 0 */
        PcrCfg->eStreamSelect = BXPT_DataSource_eInputBand;
        PcrCfg->WhichSource  = 0;
    }
#endif

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
 
    BDBG_OBJECT_ASSERT(hPcr, bxpt_t);

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
    BREG_Handle         hRegister;
    uint32_t            Reg;
    BERR_Code           ret = BERR_SUCCESS;
    uint32_t            tempStreamSelect = 0;

    BDBG_ENTER(BXPT_PCR_SetStreamPcrConfig_isr);
    BDBG_OBJECT_ASSERT(hPcr, bxpt_t);

    hRegister = hPcr->hRegister;

#if BXPT_DPCR_GLOBAL_PACKET_PROC_CTRL
    BSTD_UNUSED( tempStreamSelect );

    if( PcrCfg->JitterTimestamp >= BXPT_PCR_JitterTimestampMode_eMax ) 
    {
        BDBG_ERR(( "Invalid jitter timestamp mode %u", PcrCfg->JitterTimestamp ));
        ret = BERR_TRACE(BERR_INVALID_PARAMETER);
        goto Done;
    }

    {
        BXPT_Handle hXpt = (BXPT_Handle) hPcr->vhXpt;
        
        unsigned JitterTimestamp, PbJitterDisable, LiveJitterDisable;

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
    }

    Reg = BREG_Read32(hRegister, BCHP_XPT_DPCR0_PID_CH  + hPcr->RegOffset);
    Reg &= ~(
        BCHP_MASK( XPT_DPCR0_PID_CH , PCR_PID_CH_VALID ) |
        BCHP_MASK( XPT_DPCR0_PID_CH , PCR_PID_CH )
        ); 
    Reg |= BCHP_FIELD_DATA( XPT_DPCR0_PID_CH, PCR_PID_CH, PcrCfg->PidChannel );
    BREG_Write32( hRegister, BCHP_XPT_DPCR0_PID_CH + hPcr->RegOffset, Reg);
#else
    /* Set PCR Pid and set Valid bit 0*/
    Reg = BREG_Read32(hRegister, BCHP_XPT_DPCR0_PID + hPcr->RegOffset);
    Reg &= ~(
        BCHP_MASK( XPT_DPCR0_PID, PCR_PID) |
        BCHP_MASK( XPT_DPCR0_PID, PCR_PID_VALID) 
        );
    Reg |= BCHP_FIELD_DATA( XPT_DPCR0_PID, PCR_PID, PcrCfg->Pid); 
    BREG_Write32( hRegister, BCHP_XPT_DPCR0_PID + hPcr->RegOffset, Reg);
        
    /* Assign the rest of config */
    switch( PcrCfg->eStreamSelect )
    {
        case BXPT_DataSource_eInputBand:
        if( PcrCfg->WhichSource > BXPT_P_MAX_INPUT_BANDS )
            BDBG_ERR(( "Invalid input band requested!" ));
        else
            tempStreamSelect = ( uint32_t ) PcrCfg->WhichSource;
        break;

#if ( BCHP_CHIP == 7401 ) && ( BCHP_VER >= BCHP_VER_B0 ) \
|| ( BCHP_CHIP == 7118 ) || ( BCHP_CHIP == 7440 ) || ( BCHP_CHIP == 7400 ) || ( BCHP_CHIP == 7405 ) \
|| ( BCHP_CHIP == 7325 ) || ( BCHP_CHIP == 7335 ) || ( BCHP_CHIP == 7336  ) || ( BCHP_CHIP == 3556 ) || ( BCHP_CHIP == 3548 ) || ( BCHP_CHIP == 7342 )  || ( BCHP_CHIP == 7125) || (BCHP_CHIP == 7340 )
        case BXPT_DataSource_ePlayback:
        if( PcrCfg->WhichSource > BXPT_P_MAX_PLAYBACKS )
            BDBG_ERR(( "Invalid playback band requested!" ));
        else
            tempStreamSelect = ( uint32_t ) PcrCfg->WhichSource + 0x08;
        break;
#endif

        default:
        BDBG_ERR(( "Invalid eStreamSelect!" ));
        ret = BERR_TRACE(BERR_INVALID_PARAMETER);
        goto Done;
        break;
    }

    Reg = BREG_Read32(hRegister, BCHP_XPT_DPCR0_CTRL + hPcr->RegOffset);
    Reg &= ~(
        BCHP_MASK( XPT_DPCR0_CTRL, PCR_STREAM_SELECT) 
        );
    Reg |= (
        BCHP_FIELD_DATA( XPT_DPCR0_CTRL, PCR_STREAM_SELECT, tempStreamSelect) 
        );
    BREG_Write32( hRegister, BCHP_XPT_DPCR0_CTRL + hPcr->RegOffset, Reg);
#endif

    BREG_Write32( hRegister, BCHP_XPT_DPCR0_MAX_PCR_ERROR + hPcr->RegOffset, PcrCfg->MaxPcrError );
   
    /* Set the timebase select to xpt source */
    ret = BERR_TRACE( BXPT_PCR_P_Lock2Source( hPcr, BXPT_PCR_TimeRef_eXpt, NULL ));

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
#if BXPT_DPCR_GLOBAL_PACKET_PROC_CTRL
    Reg = BREG_Read32(hRegister, BCHP_XPT_DPCR0_PID_CH  + hPcr->RegOffset);
    Reg |= BCHP_FIELD_DATA( XPT_DPCR0_PID_CH, PCR_PID_CH_VALID, 1 );
    BREG_Write32( hRegister, BCHP_XPT_DPCR0_PID_CH + hPcr->RegOffset, Reg);
#else
    Reg = BREG_Read32(hRegister, BCHP_XPT_DPCR0_PID + hPcr->RegOffset);
    Reg |= BCHP_FIELD_DATA(XPT_DPCR0_PID, PCR_PID_VALID, 1); 
    BREG_Write32( hRegister, BCHP_XPT_DPCR0_PID + hPcr->RegOffset, Reg);
#endif

    Done:
    BDBG_LEAVE(BXPT_PCR_SetStreamPcrConfig_isr);

    return ret;
}

void BXPT_PCR_RefreshPcrPid( 
    BXPT_PCR_Handle hPcr              
    )
{
    BDBG_OBJECT_ASSERT(hPcr, bxpt_t);

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
    
    BDBG_OBJECT_ASSERT(hPcr, bxpt_t);

#if BXPT_DPCR_GLOBAL_PACKET_PROC_CTRL
    Reg = BREG_Read32(hPcr->hRegister, BCHP_XPT_DPCR0_PID_CH  + hPcr->RegOffset);
    Reg &= ~(BCHP_MASK( XPT_DPCR0_PID_CH , PCR_PID_CH_VALID )); 
    Reg |= BCHP_FIELD_DATA( XPT_DPCR0_PID_CH, PCR_PID_CH_VALID, 1 );
    BREG_Write32( hPcr->hRegister, BCHP_XPT_DPCR0_PID_CH + hPcr->RegOffset, Reg);
#else
    Reg = BREG_Read32(hPcr->hRegister, BCHP_XPT_DPCR0_PID + hPcr->RegOffset);
    Reg &= ~( BCHP_MASK( XPT_DPCR0_PID, PCR_PID_VALID ));
    Reg |= BCHP_FIELD_DATA(XPT_DPCR0_PID, PCR_PID_VALID, 1); 
    BREG_Write32( hPcr->hRegister, BCHP_XPT_DPCR0_PID + hPcr->RegOffset, Reg);
#endif
    return;
}

BERR_Code   BXPT_PCR_GetLastPcr( 
    BXPT_PCR_Handle hPcr,             
    uint32_t *p_pcrHi,            
    uint32_t *p_pcrLo             
    )
{
    BERR_Code ret = BERR_SUCCESS;

    BDBG_OBJECT_ASSERT(hPcr, bxpt_t);

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
    BREG_Handle         hRegister;
    uint32_t            Reg;
    BERR_Code           ret = BERR_SUCCESS;
    uint32_t            Offset;
    bool                PacketMode;

    BDBG_ENTER(BXPT_PCR_GetLastPcr);
    BDBG_OBJECT_ASSERT(hPcr, bxpt_t);

    hRegister = hPcr->hRegister;
    Offset = hPcr->RegOffset;

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
    BREG_Handle         hRegister;
    uint32_t            Reg;
    bool                sign;
    uint32_t            magnitude;
    BERR_Code           ret = BERR_SUCCESS;
    uint32_t            Offset;

    BDBG_ENTER(BXPT_PCR_GetPhaseError_isr);
    BDBG_OBJECT_ASSERT(hPcr, bxpt_t);

    hRegister = hPcr->hRegister;
    Offset = hPcr->RegOffset;

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
    BREG_Handle         hRegister;
    uint32_t            Reg;
    BERR_Code           ret = BERR_SUCCESS;
    uint32_t            Offset;
    bool                PacketMode;

    BDBG_ENTER(BXPT_PCR_GetStc);
    BDBG_OBJECT_ASSERT(hPcr, bxpt_t);

    hRegister = hPcr->hRegister;
    Offset = hPcr->RegOffset;

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
    uint32_t            Offset;
    BERR_Code           ret = BERR_SUCCESS;
    BREG_Handle         hRegister;

    BDBG_ENTER(BXPT_PCR_SetStcExtRateControl);
    BDBG_OBJECT_ASSERT(hPcr, bxpt_t);

#if !BXPT_HAS_32BIT_PRESCALE
    if ( StcExtRateCfg->Prescale >= 0xFF )
    {
        BDBG_ERR(("Prescale %u exceeds max (%u)!",StcExtRateCfg->Prescale, 0xFF));
        ret = BERR_TRACE(BERR_INVALID_PARAMETER);
        goto Done;
    }

    if ( StcExtRateCfg->Inc >= 0xFFFFFF )
    {
        BDBG_ERR(("Inc %u exceeds max (%u)!",StcExtRateCfg->Inc, 0xFFFFFF ));
        ret = BERR_TRACE(BERR_INVALID_PARAMETER);
        goto Done;
    }
#endif

    hRegister = hPcr->hRegister;
    Offset = hPcr->RegOffset;

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

#if !BXPT_HAS_32BIT_PRESCALE
    Done:
#endif

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
    BDBG_OBJECT_ASSERT(hPcr, bxpt_t);
    BDBG_ASSERT( Timebasefreq );    

    if (eNonStreamTimeBaseCfg == BXPT_PCR_TimeRef_eXpt) 
    {
        ret = BERR_TRACE(BERR_INVALID_PARAMETER);
        return ret;
    }   
        
    ret = BXPT_PCR_P_Lock2Source( hPcr,eNonStreamTimeBaseCfg, Timebasefreq);

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
    BDBG_OBJECT_ASSERT(hPcr, bxpt_t);
    BDBG_ASSERT( Def ); 

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
    BREG_Handle         hRegister;

    BERR_Code  ret = BERR_SUCCESS;

    BDBG_ENTER(BXPT_PCR_GetfreqRefConfig);
    BDBG_OBJECT_ASSERT(hPcr, bxpt_t);

    hRegister = hPcr->hRegister;

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
    uint32_t Reg;
        
    BDBG_OBJECT_ASSERT(hPcr, bxpt_t);

    Reg = BREG_Read32( hPcr->hRegister, BCHP_XPT_DPCR0_LOOP_CTRL + hPcr->RegOffset );
    Reg &= ~( BCHP_MASK( XPT_DPCR0_LOOP_CTRL, FREEZE_INTEGRATOR ));
    Reg |= BCHP_FIELD_DATA( XPT_DPCR0_LOOP_CTRL, FREEZE_INTEGRATOR, Freeze == true ? 1 : 0 ); 
    BREG_Write32( hPcr->hRegister, BCHP_XPT_DPCR0_LOOP_CTRL + hPcr->RegOffset, Reg );

    /* Freezing the integrator requires stopping PCR processing, so mark the PCR PID as invalid */
#if BXPT_DPCR_GLOBAL_PACKET_PROC_CTRL
    Reg = BREG_Read32(hPcr->hRegister, BCHP_XPT_DPCR0_PID_CH  + hPcr->RegOffset);
    Reg &= ~(BCHP_MASK( XPT_DPCR0_PID_CH , PCR_PID_CH_VALID )); 
    Reg |= BCHP_FIELD_DATA( XPT_DPCR0_PID_CH, PCR_PID_CH_VALID, Freeze == true ? 0 : 1 );
    BREG_Write32( hPcr->hRegister, BCHP_XPT_DPCR0_PID_CH + hPcr->RegOffset, Reg);
#else
    Reg = BREG_Read32( hPcr->hRegister, BCHP_XPT_DPCR0_PID + hPcr->RegOffset);
    Reg &= ~(BCHP_MASK( XPT_DPCR0_PID, PCR_PID_VALID)); 
    Reg |= BCHP_FIELD_DATA(XPT_DPCR0_PID, PCR_PID_VALID, Freeze == true ? 0 : 1 ); 
    BREG_Write32( hPcr->hRegister, BCHP_XPT_DPCR0_PID + hPcr->RegOffset, Reg);
#endif
}

void BXPT_PCR_SetCenterFrequency( 
    BXPT_PCR_Handle hPcr,       /* [in] The pcr handle  */
    uint32_t CenterFreq         /* [in] Center frequency */
    )
{                                            
    uint32_t Reg;
        
    BDBG_OBJECT_ASSERT(hPcr, bxpt_t);

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
        
    BDBG_OBJECT_ASSERT(hPcr, bxpt_t);

    Reg = BREG_Read32( hPcr->hRegister, BCHP_XPT_DPCR0_LOOP_CTRL + hPcr->RegOffset );

    Reg &= ~( BCHP_MASK( XPT_DPCR0_LOOP_CTRL, TRACK_RANGE ));
    Reg |= BCHP_FIELD_DATA( XPT_DPCR0_LOOP_CTRL, TRACK_RANGE, TrackRange ); 

    BREG_Write32( hPcr->hRegister, BCHP_XPT_DPCR0_LOOP_CTRL + hPcr->RegOffset, Reg );

}

#if ( BCHP_CHIP == 3548  && BCHP_VER >= BCHP_VER_B0 ) || ( BCHP_CHIP == 3556  && BCHP_VER >= BCHP_VER_B0 ) \
    || ( BCHP_CHIP == 7336 ) || ( BCHP_CHIP == 7342 )  || ( BCHP_CHIP == 7125)

void BXPT_PCR_SetPhaseErrorClampRange( 
	BXPT_PCR_Handle hPcr, 					       
	BXPT_PCR_PhaseErrorClampRange ClampRange
    )
{
    uint32_t Reg;
        
    BDBG_OBJECT_ASSERT(hPcr, bxpt_t);

    Reg = BREG_Read32( hPcr->hRegister, BCHP_XPT_DPCR0_PHASE_ERROR_CLAMP + hPcr->RegOffset );
    Reg &= ~( BCHP_MASK( XPT_DPCR0_PHASE_ERROR_CLAMP, PHASE_ERROR_CLAMP_RANGE ));
    Reg |= BCHP_FIELD_DATA( XPT_DPCR0_PHASE_ERROR_CLAMP, PHASE_ERROR_CLAMP_RANGE, ClampRange ); 
    BREG_Write32( hPcr->hRegister, BCHP_XPT_DPCR0_PHASE_ERROR_CLAMP + hPcr->RegOffset, Reg );
}

#endif

static BERR_Code SetPcrIncAndPrescale( 
    BXPT_PCR_Handle hPcr, 
    const BXPT_PCR_TimebaseFreqRefConfig *Timebasefreq
    )
{
    uint32_t Reg;
    BREG_Handle hRegister;
    BERR_Code           ret = BERR_SUCCESS;
   
    BDBG_OBJECT_ASSERT(hPcr, bxpt_t);
    hRegister = hPcr->hRegister;

#if BXPT_HAS_32BIT_PRESCALE
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
#else
    if ( Timebasefreq->Prescale >= 0xFF )
    {
        BDBG_ERR(("Prescale %u exceeds max (%u)!",Timebasefreq->Prescale, 0xFF));
        ret = BERR_TRACE(BERR_INVALID_PARAMETER);
        goto Done;
    }

    if ( Timebasefreq->Inc >= 0xFFFFFF )
    {
        BDBG_ERR(("Inc %u exceeds max (%u)!",Timebasefreq->Inc, 0xFFFFFF ));
        ret = BERR_TRACE(BERR_INVALID_PARAMETER);
        goto Done;
    }

    Reg = BREG_Read32(hRegister, BCHP_XPT_DPCR0_REF_PCR + hPcr->RegOffset);
    Reg &= ~(
        BCHP_MASK( XPT_DPCR0_REF_PCR, PRESCALE ) |
        BCHP_MASK( XPT_DPCR0_REF_PCR, PCR_INC )                
        );

    Reg |= (
        BCHP_FIELD_DATA(XPT_DPCR0_REF_PCR,PRESCALE,Timebasefreq->Prescale) |
        BCHP_FIELD_DATA(XPT_DPCR0_REF_PCR,PCR_INC,Timebasefreq->Inc)
        );
    BREG_Write32( hRegister, BCHP_XPT_DPCR0_REF_PCR + hPcr->RegOffset, Reg);

    Done:
#endif

    return ret;
}

static void GetPcrIncAndPrescale(
    BXPT_PCR_Handle hPcr, 
    BXPT_PCR_TimebaseFreqRefConfig  *TimebaseFreqConfig 
    )
{
    uint32_t            Reg;
    BREG_Handle         hRegister;

    BDBG_OBJECT_ASSERT(hPcr, bxpt_t);
    hRegister = hPcr->hRegister;

#if BXPT_HAS_32BIT_PRESCALE
    Reg = BREG_Read32(hRegister, BCHP_XPT_DPCR0_REF_PCR_INC + hPcr->RegOffset);
    TimebaseFreqConfig->Inc =  BCHP_GET_FIELD_DATA( Reg, XPT_DPCR0_REF_PCR_INC,PCR_INC );
    Reg = BREG_Read32(hRegister, BCHP_XPT_DPCR0_REF_PCR_PRESCALE + hPcr->RegOffset);
    TimebaseFreqConfig->Prescale = BCHP_GET_FIELD_DATA( Reg, XPT_DPCR0_REF_PCR_PRESCALE, PRESCALE );
#else 
    Reg = BREG_Read32(hRegister, BCHP_XPT_DPCR0_REF_PCR + hPcr->RegOffset);
    TimebaseFreqConfig->Prescale = BCHP_GET_FIELD_DATA(Reg,XPT_DPCR0_REF_PCR,PRESCALE);
    TimebaseFreqConfig->Inc =  BCHP_GET_FIELD_DATA(Reg,XPT_DPCR0_REF_PCR,PCR_INC);
#endif
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

    if ( WhichPcr >= BXPT_P_MAX_PCRS )
    {
        BDBG_ERR(("Pcr number %u exceeds max PCR channels (%u)!",WhichPcr, BXPT_P_MAX_PCRS));
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

