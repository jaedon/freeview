/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxpt_pcr_offset_emv.c $
 * $brcm_Revision: Hydra_Software_Devel/20 $
 * $brcm_Date: 5/22/12 3:17p $
 *
 * Porting interface code for the data transport core. 
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/xpt/base2/bxpt_pcr_offset_emv.c $
 * 
 * Hydra_Software_Devel/20   5/22/12 3:17p gmullen
 * SW7401-2177: Added bounds checks for BXPT_PcrOffset_ChangeStcRate()
 * 
 * Hydra_Software_Devel/19   3/30/12 1:29p gmullen
 * SW7435-48: Added external triggers 2 and 3 to GetNRTConfig()
 * 
 * Hydra_Software_Devel/18   3/30/12 11:06a gmullen
 * SW7435-81: Bug in TRIG_MODE selection
 * 
 * Hydra_Software_Devel/17   3/8/12 1:42p gmullen
 * SW7435-53: Merged to Hydra
 * 
 * Hydra_Software_Devel/SW7435-53/1   3/8/12 11:17a gmullen
 * SW7435-53: Fixed hole in timebase mapping
 * 
 * Hydra_Software_Devel/16   8/22/11 5:45p gmullen
 * SW7231-319: Merged to Hydra branch
 * 
 * Hydra_Software_Devel/SW7231-319/2   8/22/11 4:13p gmullen
 * SW7231-319: Added B0 support
 * 
 * Hydra_Software_Devel/SW7231-319/1   8/16/11 2:15p gmullen
 * SW7231-319: Initial support for B0
 * 
 * Hydra_Software_Devel/15   8/3/11 5:54p gmullen
 * SW7231-325: Merged new API to Hydra
 * 
 * Hydra_Software_Devel/SW7231-325/1   8/3/11 4:16p gmullen
 * SW7231-325: Added BXPT_PcrOffset_Acquire()
 * 
 * Hydra_Software_Devel/14   6/22/11 2:39p jgarrett
 * SW7425-654: Correcting value for soft_increment trigger mode
 * 
 * Hydra_Software_Devel/13   6/13/11 9:03p gmullen
 * SW7425-696: Incorrect mask dropped bits 31-16 of PCR Inc value
 * 
 * Hydra_Software_Devel/12   6/13/11 10:06a gmullen
 * SW7425-696: Merged fix to Hydra
 * 
 * Hydra_Software_Devel/SW7425-696/1   6/10/11 11:54a gmullen
 * SW7425-696: For binary mode, use only STC_LO.
 * 
 * Hydra_Software_Devel/11   6/7/11 2:27p gmullen
 * SW7425-653: Return STC INC HI and LO registers
 * 
 * Hydra_Software_Devel/10   6/3/11 4:34p gmullen
 * SW7425-653: Merged changes to Hydra branch
 * 
 * Hydra_Software_Devel/SW7425-653/1   6/2/11 10:35a gmullen
 * SW7425-653: Added non-realtime transcoding support
 * 
 * Hydra_Software_Devel/9   3/30/11 11:14a gmullen
 * SWDTV-6265: Free-running timebase select was incorrect
 * 
 * Hydra_Software_Devel/8   3/22/11 1:42p gmullen
 * SW7425-223: Added code to correctly map STC broadcasts
 * 
 * Hydra_Software_Devel/7   3/2/11 11:33a gmullen
 * SW7425-123: Removed debug mesg
 * 
 * Hydra_Software_Devel/6   2/25/11 2:15p gmullen
 * SW7425-123: Merged fix to mainline
 * 
 * Hydra_Software_Devel/SW7425-123/1   2/24/11 11:37a gmullen
 * SW7425-123: Incorrect define used in BXPT_PcrOffset_GetStc_isr()
 * 
 * Hydra_Software_Devel/5   12/28/10 3:55p gmullen
 * SW7425-15: Get BroadcastMode in _GetSettings()
 * 
 * Hydra_Software_Devel/4   12/21/10 9:24a gmullen
 * SW7425-15: Added STC broadcast mode
 * 
 * Hydra_Software_Devel/3   10/28/10 6:01p gmullen
 * SW7425-15: Ported files
 * 
 * Hydra_Software_Devel/2   10/28/10 2:08p gmullen
 * SW7422-20: Checkin ported files
 * 
 * Hydra_Software_Devel/25   9/28/10 11:20a gmullen
 * SW35230-1354: Default error acquire threshold to 2
 * 
 * Hydra_Software_Devel/24   9/14/10 10:07a gmullen
 * SW35230-1354: Added ONE_ERR_ACQ or TWO_ERR_ACQ support to the PI
 * 
 * Hydra_Software_Devel/23   7/4/10 5:33p gmullen
 * SW7630-81: Added support for QUICK
 * 
 * Hydra_Software_Devel/22   4/23/10 11:21a gmullen
 * SW7405-4254: Route PCR PID channel to RAVE only when stream PCRs are
 * used.
 * 
 * Hydra_Software_Devel/21   2/25/10 1:32p gmullen
 * SW7550-281: Added checks for WhichStc param. Fixed bug in
 * _SetSettings() where StcSelect wasn't used
 * 
 * Hydra_Software_Devel/20   6/8/09 4:40p gmullen
 * PR55611: Added support for selecting which DPCR block drives the STC to
 * _GetSettings() and _SetSettings()
 * 
 * Hydra_Software_Devel/19   2/5/09 3:03p gmullen
 * PR51890: Fixed STC_SEL setting.
 * 
 * Hydra_Software_Devel/18   1/28/09 9:53a gmullen
 * PR51198: Fixed bug in IntId mapping
 * 
 * Hydra_Software_Devel/17   1/20/09 6:01p gmullen
 * PR50689: Added GetIntId API.
 * 
 * Hydra_Software_Devel/16   9/22/08 2:53p gmullen
 * PR47074: Do not disable PID channel if its used by the PCR Offset
 * block.
 * 
 * Hydra_Software_Devel/15   7/9/08 9:23a gmullen
 * PR44665: Fixed compilation bug in BXPT_PcrOffset_CaptureStcOnce_isr
 * 
 * Hydra_Software_Devel/14   7/9/08 9:18a gmullen
 * PR44665: Added BXPT_PcrOffset_CaptureStcOnce() and
 * BXPT_PcrOffset_CaptureStcOnce_isr()
 * 
 * Hydra_Software_Devel/13   4/15/08 8:43a gmullen
 * PR41735: Force offset to invalid state during SetSettings when HostPcrs
 * are used.
 * 
 * Hydra_Software_Devel/12   2/27/08 4:56p katrep
 * PR39387: Disable pcr offset valid bit suring playback. This will
 * prevent generation of valid pcr offset event in the ITB.
 * 
 * Hydra_Software_Devel/11   2/27/08 3:29p piyushg
 * PR39387: Added a new API BXPT_PcrOffset_ForceInvalid to
 * clear the PCR_OFFSET_VALID bit.
 * 
 * Hydra_Software_Devel/10   2/19/08 2:31p gmullen
 * PR38918: Added BXPT_PcrOffset_IsOffsetValid() and
 * BXPT_PcrOffset_IsOffsetValid_isr()
 * 
 * Hydra_Software_Devel/9   8/29/07 1:19p katrep
 * PR34439: Updated BXPT_PcrOffset_RegenOffset_isr API
 * 
 * Hydra_Software_Devel/8   8/17/07 11:26a gmullen
 * PR34143: Changed default offset threshold to 8
 * 
 * Hydra_Software_Devel/7   4/17/07 5:53p gmullen
 * PR26217: Added StcSelect to BXPT_PcrOffset_Settings struct.
 * 
 * Hydra_Software_Devel/6   4/2/07 11:41a gmullen
 * PR28674: Map decode channels to R pipe when enabling channel
 * destination.
 * 
 * Hydra_Software_Devel/5   3/14/07 3:50p gmullen
 * PR28676: Removed call to set PID channel destination
 * 
 * Hydra_Software_Devel/4   3/1/07 11:31a gmullen
 * PR26217: Changed new API names to conform to sw arch spec.
 * 
 * Hydra_Software_Devel/3   2/28/07 1:33p gmullen
 * PR26217: Enable offset generation for PCR PID.
 * 
 * Hydra_Software_Devel/2   2/8/07 4:33p gmullen
 * PR26217: Changed API names for consistency
 * 
 * Hydra_Software_Devel/1   2/6/07 7:05p gmullen
 * PR26217: Added B0 support.
 * 
 *
 ***************************************************************************/

#include "bstd.h"
#include "bkni.h"
#include "bmem.h" 
#include "bxpt_priv.h"
#include "bxpt_pcr_offset.h"
#include "bchp_xpt_pcroffset.h"
#include "bchp_xpt_fe.h"
#include "bchp.h"

#if( BDBG_DEBUG_BUILD == 1 )
BDBG_MODULE( xpt_pcr_offset_emv );
#endif

#define DEFAULT_STC_COUNT_MODE      BXPT_PcrOffset_StcCountMode_eMod300
#define DEFAULT_USE_PCR_TIMEBASE    true
#define DEFAULT_TIME_BASE           0
#define DEFAULT_OFFSET_THRESHOLD    ( 0x08 )
#define DEFAULT_MAX_PCR_ERROR       ( 0xFF )

#define PCROFF_CONTEXT_STEP             ( BCHP_XPT_PCROFFSET_CONTEXT1_PCROFFSET_CTRL - BCHP_XPT_PCROFFSET_CONTEXT0_PCROFFSET_CTRL )
#define STC_CONTEXT_STEP                ( BCHP_XPT_PCROFFSET_STC1_CTRL - BCHP_XPT_PCROFFSET_STC0_CTRL )

#define PCROFF_CTRL_OFFSET              ( BCHP_XPT_PCROFFSET_CONTEXT0_PCROFFSET_CTRL -      BCHP_XPT_PCROFFSET_CONTEXT0_PCROFFSET_CTRL )
#define PCROFF_THRESHOLD_OFFSET         ( BCHP_XPT_PCROFFSET_CONTEXT0_OFFSET_THRESHOLD -    BCHP_XPT_PCROFFSET_CONTEXT0_PCROFFSET_CTRL )
#define PCROFF_MAX_ERROR_OFFSET         ( BCHP_XPT_PCROFFSET_CONTEXT0_OFFSET_MAX_ERROR -    BCHP_XPT_PCROFFSET_CONTEXT0_PCROFFSET_CTRL )
#define PCROFF_PID_CHAN_OFFSET          ( BCHP_XPT_PCROFFSET_CONTEXT0_PP_PCR_PID_CH_NUM -   BCHP_XPT_PCROFFSET_CONTEXT0_PCROFFSET_CTRL )
#define PCROFF_OFFSET_OFFSET            ( BCHP_XPT_PCROFFSET_CONTEXT0_OFFSET -              BCHP_XPT_PCROFFSET_CONTEXT0_PCROFFSET_CTRL )
#define PCROFF_SPLICE_RDPTR_OFFSET      ( BCHP_XPT_PCROFFSET_CONTEXT0_SPLICE_RD_PTR -       BCHP_XPT_PCROFFSET_CONTEXT0_PCROFFSET_CTRL )
#define PCROFF_SPLICE_WRPTR_OFFSET      ( BCHP_XPT_PCROFFSET_CONTEXT0_SPLICE_WR_PTR -       BCHP_XPT_PCROFFSET_CONTEXT0_PCROFFSET_CTRL )
#define PCROFF_SPLICE_CHAN01_OFFSET     ( BCHP_XPT_PCROFFSET_CONTEXT0_SPLICE_PID_CH0_CH1 -  BCHP_XPT_PCROFFSET_CONTEXT0_PCROFFSET_CTRL )
#define PCROFF_SPLICE_CHAN23_OFFSET     ( BCHP_XPT_PCROFFSET_CONTEXT0_SPLICE_PID_CH2_CH3 -  BCHP_XPT_PCROFFSET_CONTEXT0_PCROFFSET_CTRL )
#define PCROFF_SPLICE_CHAN45_OFFSET     ( BCHP_XPT_PCROFFSET_CONTEXT0_SPLICE_PID_CH4_CH5 -  BCHP_XPT_PCROFFSET_CONTEXT0_PCROFFSET_CTRL )
#define PCROFF_SPLICE_CHAN67_OFFSET     ( BCHP_XPT_PCROFFSET_CONTEXT0_SPLICE_PID_CH6_CH7 -  BCHP_XPT_PCROFFSET_CONTEXT0_PCROFFSET_CTRL )
#define PCROFF_SPLICE_STATUS_OFFSET     ( BCHP_XPT_PCROFFSET_CONTEXT0_SPLICE_STATUS -       BCHP_XPT_PCROFFSET_CONTEXT0_PCROFFSET_CTRL )
#define PCROFF_OFFSET_STATE             ( BCHP_XPT_PCROFFSET_CONTEXT0_OFFSET_STATE -        BCHP_XPT_PCROFFSET_CONTEXT0_PCROFFSET_CTRL )

#define PCROFF_OFFSET_STATUS0           ( BCHP_XPT_PCROFFSET_CONTEXT0_OFFSET_STATUS_0 -     BCHP_XPT_PCROFFSET_CONTEXT0_PCROFFSET_CTRL )
#define PCROFF_OFFSET_STATUS1           ( BCHP_XPT_PCROFFSET_CONTEXT0_OFFSET_STATUS_1 -     BCHP_XPT_PCROFFSET_CONTEXT0_PCROFFSET_CTRL )
#define PCROFF_OFFSET_STATUS2           ( BCHP_XPT_PCROFFSET_CONTEXT0_OFFSET_STATUS_2 -     BCHP_XPT_PCROFFSET_CONTEXT0_PCROFFSET_CTRL )
#define PCROFF_OFFSET_STATUS3           ( BCHP_XPT_PCROFFSET_CONTEXT0_OFFSET_STATUS_3 -     BCHP_XPT_PCROFFSET_CONTEXT0_PCROFFSET_CTRL )
#define PCROFF_OFFSET_STATUS4           ( BCHP_XPT_PCROFFSET_CONTEXT0_OFFSET_STATUS_4 -     BCHP_XPT_PCROFFSET_CONTEXT0_PCROFFSET_CTRL )
#define PCROFF_OFFSET_STATUS5           ( BCHP_XPT_PCROFFSET_CONTEXT0_OFFSET_STATUS_5 -     BCHP_XPT_PCROFFSET_CONTEXT0_PCROFFSET_CTRL )
#define PCROFF_OFFSET_STATUS6           ( BCHP_XPT_PCROFFSET_CONTEXT0_OFFSET_STATUS_6 -     BCHP_XPT_PCROFFSET_CONTEXT0_PCROFFSET_CTRL )
#define PCROFF_OFFSET_STATUS7           ( BCHP_XPT_PCROFFSET_CONTEXT0_OFFSET_STATUS_7 -     BCHP_XPT_PCROFFSET_CONTEXT0_PCROFFSET_CTRL )
#define PCROFF_OFFSET_STATUS8           ( BCHP_XPT_PCROFFSET_CONTEXT0_OFFSET_STATUS_8 -     BCHP_XPT_PCROFFSET_CONTEXT0_PCROFFSET_CTRL )
#define PCROFF_OFFSET_STATUS9           ( BCHP_XPT_PCROFFSET_CONTEXT0_OFFSET_STATUS_9 -     BCHP_XPT_PCROFFSET_CONTEXT0_PCROFFSET_CTRL )
#define PCROFF_OFFSET_STATUS10          ( BCHP_XPT_PCROFFSET_CONTEXT0_OFFSET_STATUS_10 -    BCHP_XPT_PCROFFSET_CONTEXT0_PCROFFSET_CTRL )
#define PCROFF_OFFSET_STATUS11          ( BCHP_XPT_PCROFFSET_CONTEXT0_OFFSET_STATUS_11 -    BCHP_XPT_PCROFFSET_CONTEXT0_PCROFFSET_CTRL )
#define PCROFF_OFFSET_STATUS12          ( BCHP_XPT_PCROFFSET_CONTEXT0_OFFSET_STATUS_12 -    BCHP_XPT_PCROFFSET_CONTEXT0_PCROFFSET_CTRL )
#define PCROFF_OFFSET_STATUS13          ( BCHP_XPT_PCROFFSET_CONTEXT0_OFFSET_STATUS_13 -    BCHP_XPT_PCROFFSET_CONTEXT0_PCROFFSET_CTRL )
#define PCROFF_OFFSET_STATUS14          ( BCHP_XPT_PCROFFSET_CONTEXT0_OFFSET_STATUS_14 -    BCHP_XPT_PCROFFSET_CONTEXT0_PCROFFSET_CTRL )
#define PCROFF_OFFSET_STATUS15          ( BCHP_XPT_PCROFFSET_CONTEXT0_OFFSET_STATUS_15 -    BCHP_XPT_PCROFFSET_CONTEXT0_PCROFFSET_CTRL )

#define PCROFF_OFFSET_INVALID           ( BCHP_XPT_PCROFFSET_CONTEXT0_OFFSET_VALID -        BCHP_XPT_PCROFFSET_CONTEXT0_PCROFFSET_CTRL )

#define PID_CHNL_STEPSIZE       ( 4 )

static uint32_t GetStcRegAddr( 
    uint32_t Ctx0Addr,
    unsigned WhichStc
    );

static void SetSplicePidChannel(
    BXPT_PcrOffset_Handle hPcrOff,
    unsigned ChannelNum,
    unsigned Pid
    );

static uint32_t MapPcrToTimebase(
    unsigned WhichPcr
    )
{
#if BXPT_PCR_OFFSET_STC_SKIP
    if (WhichPcr < STC_FREE_RUNNING)
        return WhichPcr;
    else 
        return WhichPcr + 1;
#else
    return WhichPcr;
#endif
}

static unsigned MapTimebaseToPcr(
    uint32_t Timebase
    )
{
#if BXPT_PCR_OFFSET_STC_SKIP
    if (Timebase > STC_FREE_RUNNING)
        return Timebase - 1;
    else
        return Timebase;
#else
    return Timebase;
#endif
}

BERR_Code BXPT_PcrOffset_GetTotalChannels(
    BXPT_Handle hXpt,               /* [in] The transport handle */
    unsigned int *TotalChannels     /* [out] The number of Pcr offset channels. */
    )
{
    BERR_Code ExitCode = BERR_SUCCESS;

    BSTD_UNUSED( hXpt );                                              

    /* PCR Offset channels are mapped one-to-one to the RAVE channels. */
    *TotalChannels = BXPT_NUM_PCR_OFFSET_CHANNELS;

    return( ExitCode );
}
 
BERR_Code BXPT_PcrOffset_GetChannelDefSettings(
    BXPT_Handle hXpt,           /* [in] The transport handle - need chip info */
    unsigned int ChannelNo,                 /* [in] Which pcr offset module */
    BXPT_PcrOffset_Defaults *Defaults   /* [out] The default settings of a pcr module */
    )
{
    BERR_Code ExitCode = BERR_SUCCESS;

    BSTD_UNUSED( hXpt );
    BDBG_ASSERT( Defaults );

    if( ChannelNo >= BXPT_NUM_PCR_OFFSET_CHANNELS )
    {
        BDBG_ERR(( "ChannelNo %u out of range!", ChannelNo ));
        ExitCode = BERR_TRACE( BXPT_ERR_NO_AVAILABLE_RESOURCES );
        goto Done;
    }

    Defaults->UsePcrTimeBase = DEFAULT_USE_PCR_TIMEBASE;
    Defaults->WhichPcr = DEFAULT_TIME_BASE;
    Defaults->OffsetThreshold = DEFAULT_OFFSET_THRESHOLD;
    Defaults->MaxPcrError = DEFAULT_MAX_PCR_ERROR;
    
    Defaults->WhichStc = ChannelNo % BXPT_NUM_STCS;
    Defaults->ConsecutiveErrorThreshold = 2;

    Done:
    return( ExitCode );
}

BERR_Code BXPT_PcrOffset_Open(
    BXPT_Handle hXpt,                  /* [in] The transport handle*/
    unsigned int ChannelNo,            /* [in] Which pcr offset module */
    BXPT_PcrOffset_Defaults *Defaults, /* [in] The default setting */
    BXPT_PcrOffset_Handle *hChannel    /* [out] The channel handle */
    )
{
    BXPT_PcrOffset_Handle lhPcrOff;
    uint32_t Reg, RegAddr;
    unsigned Index;

    BERR_Code ExitCode = BERR_SUCCESS;
    uint32_t TimeBase = STC_FREE_RUNNING;
    unsigned OneErrAcq = 0;
    unsigned TwoErrAcq = 0;

    BDBG_ASSERT( hXpt );
    BDBG_ASSERT( Defaults );
    BDBG_ASSERT( hChannel );

    if( ChannelNo >= BXPT_NUM_PCR_OFFSET_CHANNELS )
    {
        BDBG_ERR(( "ChannelNo %u out of range!", ChannelNo ));
        ExitCode = BERR_TRACE( BXPT_ERR_NO_AVAILABLE_RESOURCES );
        goto Done;
    }

    if( Defaults->WhichStc >= BXPT_NUM_STCS )
    {
        ExitCode = BERR_TRACE( BXPT_ERR_NO_AVAILABLE_RESOURCES );
        goto Done;
    }

    /* Allocate memory for the handle. */
    lhPcrOff = BKNI_Malloc( sizeof( BXPT_P_PcrOffset_Impl ) );
    if( lhPcrOff == NULL )
    {
        BDBG_ERR(( "BKNI_Malloc() failed!" ));
        ExitCode = BERR_TRACE( BERR_OUT_OF_SYSTEM_MEMORY );
        goto Done;
    }
         
    lhPcrOff->BaseAddr = BCHP_XPT_PCROFFSET_CONTEXT0_PCROFFSET_CTRL + ( ChannelNo * PCROFF_CONTEXT_STEP );
    lhPcrOff->hChip = hXpt->hChip;
    lhPcrOff->hReg = hXpt->hRegister;
    lhPcrOff->hMem = hXpt->hMemory;
    lhPcrOff->lvXpt = ( void * ) hXpt;
    lhPcrOff->ChannelNo = ChannelNo;
    lhPcrOff->PidChannelNum = 0;
    lhPcrOff->WhichStc = Defaults->WhichStc;
    lhPcrOff->UseHostPcrs = false;

    /* Map the PCR Offset to an STC. */
    Reg = BREG_Read32( lhPcrOff->hReg, lhPcrOff->BaseAddr + PCROFF_CTRL_OFFSET );
    Reg &= ~( 
        BCHP_MASK( XPT_PCROFFSET_CONTEXT0_PCROFFSET_CTRL, STC_SEL ) 
    );
    Reg |= (
        BCHP_FIELD_DATA( XPT_PCROFFSET_CONTEXT0_PCROFFSET_CTRL, STC_SEL, Defaults->WhichStc )
    );
    BREG_Write32( lhPcrOff->hReg, lhPcrOff->BaseAddr + PCROFF_CTRL_OFFSET, Reg );

    if( Defaults->UsePcrTimeBase )
        TimeBase = MapPcrToTimebase( Defaults->WhichPcr );
    RegAddr = GetStcRegAddr( BCHP_XPT_PCROFFSET_STC0_TIMEBASE_SEL, Defaults->WhichStc );
    Reg = BREG_Read32( lhPcrOff->hReg, RegAddr );
    Reg &= ~( 
        BCHP_MASK( XPT_PCROFFSET_STC0_TIMEBASE_SEL, TIMEBASE_SEL ) 
    );
    Reg |= (
        BCHP_FIELD_DATA( XPT_PCROFFSET_STC0_TIMEBASE_SEL, TIMEBASE_SEL, TimeBase )
    );
    BREG_Write32( lhPcrOff->hReg, RegAddr, Reg );
    lhPcrOff->CurrentTimeBase = TimeBase;

    Reg = BREG_Read32( lhPcrOff->hReg, lhPcrOff->BaseAddr + PCROFF_THRESHOLD_OFFSET );
    Reg &= ~( 
        BCHP_MASK( XPT_PCROFFSET_CONTEXT0_OFFSET_THRESHOLD, OFFSET_THRESHOLD ) 
    );
    Reg |= (
        BCHP_FIELD_DATA( XPT_PCROFFSET_CONTEXT0_OFFSET_THRESHOLD, OFFSET_THRESHOLD, Defaults->OffsetThreshold )
    );
    BREG_Write32( lhPcrOff->hReg, lhPcrOff->BaseAddr + PCROFF_THRESHOLD_OFFSET, Reg );

    Reg = BREG_Read32( lhPcrOff->hReg, lhPcrOff->BaseAddr + PCROFF_MAX_ERROR_OFFSET );
    Reg &= ~( 
        BCHP_MASK( XPT_PCROFFSET_CONTEXT0_OFFSET_MAX_ERROR, MAX_ERROR ) 
    );
    Reg |= (
        BCHP_FIELD_DATA( XPT_PCROFFSET_CONTEXT0_OFFSET_MAX_ERROR, MAX_ERROR, Defaults->MaxPcrError )
    );
    BREG_Write32( lhPcrOff->hReg, lhPcrOff->BaseAddr + PCROFF_MAX_ERROR_OFFSET, Reg );

    BXPT_PcrOffset_ClearQueue( lhPcrOff );

    Reg = BREG_Read32( lhPcrOff->hReg, lhPcrOff->BaseAddr + PCROFF_PID_CHAN_OFFSET );
    Reg &= ~( BCHP_MASK( XPT_PCROFFSET_CONTEXT0_PP_PCR_PID_CH_NUM, PCR_PID_CH_NUM ) );
    Reg |= ( BCHP_FIELD_DATA( XPT_PCROFFSET_CONTEXT0_PP_PCR_PID_CH_NUM, PCR_PID_CH_NUM, 0xFE ) );
    BREG_Write32( lhPcrOff->hReg, lhPcrOff->BaseAddr + PCROFF_PID_CHAN_OFFSET, Reg );

    /* Walk through the PID Config table and clear any index that's using this Offset */
    for( Index = 0; Index < BXPT_NUM_PID_CHANNELS; Index++ )
    {
        RegAddr = BCHP_XPT_PCROFFSET_PID_CONFIG_TABLE_i_ARRAY_BASE + Index * PID_CHNL_STEPSIZE;
        Reg = BREG_Read32( hXpt->hRegister, RegAddr );
        if( BCHP_GET_FIELD_DATA( Reg, XPT_PCROFFSET_PID_CONFIG_TABLE_i, OFFSET_INDEX ) ==  ChannelNo )
            BREG_Write32( hXpt->hRegister, RegAddr, 0 );
    }

    Reg = BREG_Read32( lhPcrOff->hReg, lhPcrOff->BaseAddr + PCROFF_CTRL_OFFSET );
    Reg &= ~( 
        BCHP_MASK( XPT_PCROFFSET_CONTEXT0_PCROFFSET_CTRL, TWO_ERR_ACQ ) |
        BCHP_MASK( XPT_PCROFFSET_CONTEXT0_PCROFFSET_CTRL, ONE_ERR_ACQ ) 
    );
    if( Defaults->ConsecutiveErrorThreshold == 1 )
    {
        OneErrAcq = 1;
    }
    else if( Defaults->ConsecutiveErrorThreshold >= 2 )
    {
        TwoErrAcq = 1;
    }
    Reg |= (
        BCHP_FIELD_DATA( XPT_PCROFFSET_CONTEXT0_PCROFFSET_CTRL, TWO_ERR_ACQ, TwoErrAcq ) |
        BCHP_FIELD_DATA( XPT_PCROFFSET_CONTEXT0_PCROFFSET_CTRL, ONE_ERR_ACQ, OneErrAcq )
    );
    BREG_Write32( lhPcrOff->hReg, lhPcrOff->BaseAddr + PCROFF_CTRL_OFFSET, Reg );

    /* Note that no PID channels are using this Offset block */
    lhPcrOff->PidChannelNum = BXPT_NUM_PID_CHANNELS;

    hXpt->PcrOffsets[ ChannelNo ].Handle = lhPcrOff;
    hXpt->PcrOffsets[ ChannelNo ].Allocated = true;
    *hChannel = lhPcrOff;
 
    Done:
    return( ExitCode );
}

BERR_Code BXPT_PcrOffset_Close(
    BXPT_PcrOffset_Handle hChannel   /* [in] The channel handle */
    )
{
    BXPT_Handle hXpt;
    uint32_t Reg;

    BERR_Code ExitCode = BERR_SUCCESS;

    BDBG_ASSERT( hChannel );


    Reg = BREG_Read32( hChannel->hReg, hChannel->BaseAddr + PCROFF_CTRL_OFFSET );
    Reg &= ~( 
        BCHP_MASK( XPT_PCROFFSET_CONTEXT0_PCROFFSET_CTRL, CTXT_ENABLE ) 
    );
    BREG_Write32( hChannel->hReg, hChannel->BaseAddr + PCROFF_CTRL_OFFSET, Reg );

    hXpt = ( BXPT_Handle ) hChannel->lvXpt;
    hXpt->PcrOffsets[ hChannel->ChannelNo ].Allocated = false;
    BKNI_Free( hChannel );

    return( ExitCode );
}

BERR_Code BXPT_PcrOffset_GetSettings(
    BXPT_PcrOffset_Handle hChannel,
    BXPT_PcrOffset_Settings *Settings
    )
{
    uint32_t Reg, RegAddr;
    uint32_t Timebase;

    BERR_Code ExitCode = BERR_SUCCESS;

    BDBG_ASSERT( hChannel );
    BDBG_ASSERT( Settings );

    Settings->PidChannelNum = hChannel->PidChannelNum;
    Settings->UseHostPcrs = hChannel->UseHostPcrs;

    RegAddr = GetStcRegAddr( BCHP_XPT_PCROFFSET_STC0_TIMEBASE_SEL, hChannel->WhichStc );
    Reg = BREG_Read32( hChannel->hReg, RegAddr );
    Timebase = BCHP_GET_FIELD_DATA( Reg, XPT_PCROFFSET_STC0_TIMEBASE_SEL, TIMEBASE_SEL );

	if( Timebase == STC_FREE_RUNNING )
    {
        Settings->UsePcrTimeBase = false;
    }
    else
    {
        Settings->UsePcrTimeBase = true;
        Settings->WhichPcr = MapTimebaseToPcr( Timebase );
    }            

    RegAddr = GetStcRegAddr( BCHP_XPT_PCROFFSET_STC0_CTRL, hChannel->WhichStc );
    Reg = BREG_Read32( hChannel->hReg, RegAddr );
    Settings->CountMode = BCHP_GET_FIELD_DATA( Reg, XPT_PCROFFSET_STC0_CTRL, MODE ) == 1 ? 
        BXPT_PcrOffset_StcCountMode_eBinary : BXPT_PcrOffset_StcCountMode_eMod300;

#if BXPT_HAS_TSMUX
    Settings->BroadcastMode = BCHP_GET_FIELD_DATA( Reg, XPT_PCROFFSET_STC0_CTRL, BROADCAST_MODE );
#endif

    Reg = BREG_Read32( hChannel->hReg, hChannel->BaseAddr + PCROFF_THRESHOLD_OFFSET );
    Settings->OffsetThreshold = BCHP_GET_FIELD_DATA( Reg, XPT_PCROFFSET_CONTEXT0_OFFSET_THRESHOLD, OFFSET_THRESHOLD );

    Reg = BREG_Read32( hChannel->hReg, hChannel->BaseAddr + PCROFF_MAX_ERROR_OFFSET );
    Settings->MaxPcrError = BCHP_GET_FIELD_DATA( Reg, XPT_PCROFFSET_CONTEXT0_OFFSET_MAX_ERROR, MAX_ERROR );

    Reg = BREG_Read32( hChannel->hReg, hChannel->BaseAddr + PCROFF_CTRL_OFFSET );
    Settings->StcSelect = BCHP_GET_FIELD_DATA( Reg, XPT_PCROFFSET_CONTEXT0_PCROFFSET_CTRL, STC_SEL );

    if( BCHP_GET_FIELD_DATA( Reg, XPT_PCROFFSET_CONTEXT0_PCROFFSET_CTRL, ONE_ERR_ACQ ) )
        Settings->ConsecutiveErrorThreshold = 1;
    if( BCHP_GET_FIELD_DATA( Reg, XPT_PCROFFSET_CONTEXT0_PCROFFSET_CTRL, TWO_ERR_ACQ ) )
        Settings->ConsecutiveErrorThreshold = 2;

    return( ExitCode );
}

BERR_Code BXPT_PcrOffset_SetSettings(
    BXPT_PcrOffset_Handle hChannel,
    const BXPT_PcrOffset_Settings *Settings
    )
{
    uint32_t Reg, RegAddr;

    BERR_Code ExitCode = BERR_SUCCESS;
	uint32_t TimeBase = STC_FREE_RUNNING;
    unsigned OneErrAcq = 0;
    unsigned TwoErrAcq = 0;

    BDBG_ASSERT( hChannel );
    BDBG_ASSERT( Settings );

    if( Settings->StcSelect >= BXPT_NUM_STCS )
    {
        ExitCode = BERR_TRACE( BXPT_ERR_NO_AVAILABLE_RESOURCES );
        goto Done;
    }
    hChannel->WhichStc = Settings->StcSelect;

    /* Need to disable the PCR Offset block before doing anything. See the RDB */
    Reg = BREG_Read32( hChannel->hReg, hChannel->BaseAddr + PCROFF_CTRL_OFFSET );
    Reg &= ~( 
        BCHP_MASK( XPT_PCROFFSET_CONTEXT0_PCROFFSET_CTRL, CTXT_ENABLE ) 
    );  
    BREG_Write32( hChannel->hReg, hChannel->BaseAddr + PCROFF_CTRL_OFFSET, Reg );

	if( Settings->UsePcrTimeBase )
		TimeBase = MapPcrToTimebase( Settings->WhichPcr );
    RegAddr = GetStcRegAddr( BCHP_XPT_PCROFFSET_STC0_TIMEBASE_SEL, hChannel->WhichStc );
    Reg = BREG_Read32( hChannel->hReg, RegAddr );
    Reg &= ~( 
        BCHP_MASK( XPT_PCROFFSET_STC0_TIMEBASE_SEL, TIMEBASE_SEL ) 
    );
    Reg |= (
        BCHP_FIELD_DATA( XPT_PCROFFSET_STC0_TIMEBASE_SEL, TIMEBASE_SEL, TimeBase )
    );
    BREG_Write32( hChannel->hReg, RegAddr, Reg );
    hChannel->CurrentTimeBase = TimeBase;

    RegAddr = GetStcRegAddr( BCHP_XPT_PCROFFSET_STC0_CTRL, hChannel->WhichStc );
    Reg = BREG_Read32( hChannel->hReg, RegAddr );
    Reg &= ~( 
        BCHP_MASK( XPT_PCROFFSET_STC0_CTRL, MODE ) 
    );
    Reg |= (
        BCHP_FIELD_DATA( XPT_PCROFFSET_STC0_CTRL, MODE, Settings->CountMode )
    );
    BREG_Write32( hChannel->hReg, RegAddr, Reg );

#if BXPT_HAS_TSMUX
    Reg = BREG_Read32( hChannel->hReg, RegAddr );
    Reg &= ~( 
        BCHP_MASK( XPT_PCROFFSET_STC0_CTRL, BROADCAST_MODE ) 
    );
    Reg |= (
        BCHP_FIELD_DATA( XPT_PCROFFSET_STC0_CTRL, BROADCAST_MODE, Settings->BroadcastMode )
    );
    BREG_Write32( hChannel->hReg, RegAddr, Reg );
#endif

    Reg = BREG_Read32( hChannel->hReg, hChannel->BaseAddr + PCROFF_PID_CHAN_OFFSET );
    Reg &= ~( 
        BCHP_MASK( XPT_PCROFFSET_CONTEXT0_PP_PCR_PID_CH_NUM, PCR_PID_CH_NUM ) 
    );

    hChannel->PidChannelNum = Settings->PidChannelNum;
    hChannel->UseHostPcrs = Settings->UseHostPcrs;
    if( Settings->UseHostPcrs == true )
    {
        Reg |= ( BCHP_FIELD_DATA( XPT_PCROFFSET_CONTEXT0_PP_PCR_PID_CH_NUM, PCR_PID_CH_NUM, 0xFF ) );
        BXPT_PcrOffset_ForceInvalid( hChannel );
    }
    else
    {
        Reg |= ( BCHP_FIELD_DATA( XPT_PCROFFSET_CONTEXT0_PP_PCR_PID_CH_NUM, PCR_PID_CH_NUM, Settings->PidChannelNum ) );

        /* Enable PCR Offset in the PID channel config table */
        BXPT_PcrOffset_EnableOffset( hChannel, Settings->PidChannelNum, false, true );
    }

    BREG_Write32( hChannel->hReg, hChannel->BaseAddr + PCROFF_PID_CHAN_OFFSET, Reg );

    Reg = BREG_Read32( hChannel->hReg, hChannel->BaseAddr + PCROFF_MAX_ERROR_OFFSET );
    Reg &= ~( 
        BCHP_MASK( XPT_PCROFFSET_CONTEXT0_OFFSET_MAX_ERROR, MAX_ERROR ) 
    );
    Reg |= (
        BCHP_FIELD_DATA( XPT_PCROFFSET_CONTEXT0_OFFSET_MAX_ERROR, MAX_ERROR, Settings->MaxPcrError )
    );
    BREG_Write32( hChannel->hReg, hChannel->BaseAddr + PCROFF_MAX_ERROR_OFFSET, Reg );

    /* 
    ** Need to enable data on this PID channel to go RAVE. PCR Offset ignores it otherwise. To
    ** resolve PR 28674, enable the R pipe. All data destined for the decoders will be on the R pipe.
    */
    if( Settings->UseHostPcrs == false )
    {
    	BXPT_P_SetPidChannelDestination( (BXPT_Handle) hChannel->lvXpt, Settings->PidChannelNum, 5, true );
    }

    Reg = BREG_Read32( hChannel->hReg, hChannel->BaseAddr + PCROFF_THRESHOLD_OFFSET );
    Reg &= ~( 
        BCHP_MASK( XPT_PCROFFSET_CONTEXT0_OFFSET_THRESHOLD, OFFSET_THRESHOLD ) 
    );                                                                             
    Reg |= (
        BCHP_FIELD_DATA( XPT_PCROFFSET_CONTEXT0_OFFSET_THRESHOLD, OFFSET_THRESHOLD, Settings->OffsetThreshold )
    );
    BREG_Write32( hChannel->hReg, hChannel->BaseAddr + PCROFF_THRESHOLD_OFFSET, Reg );

    /* Put this offset context into acquire mode */
    BXPT_PcrOffset_Acquire( hChannel );

    /* Enable the context after the config is done. */
    Reg = BREG_Read32( hChannel->hReg, hChannel->BaseAddr + PCROFF_CTRL_OFFSET );
    Reg &= ~( 
        BCHP_MASK( XPT_PCROFFSET_CONTEXT0_PCROFFSET_CTRL, STC_SEL ) |
        BCHP_MASK( XPT_PCROFFSET_CONTEXT0_PCROFFSET_CTRL, CTXT_ENABLE ) 
    );
    Reg |= (
        BCHP_FIELD_DATA( XPT_PCROFFSET_CONTEXT0_PCROFFSET_CTRL, STC_SEL, Settings->StcSelect ) |
        BCHP_FIELD_DATA( XPT_PCROFFSET_CONTEXT0_PCROFFSET_CTRL, CTXT_ENABLE, 1 )
    );
    BREG_Write32( hChannel->hReg, hChannel->BaseAddr + PCROFF_CTRL_OFFSET, Reg );

    Reg = BREG_Read32( hChannel->hReg, hChannel->BaseAddr + PCROFF_CTRL_OFFSET );
    Reg &= ~( 
        BCHP_MASK( XPT_PCROFFSET_CONTEXT0_PCROFFSET_CTRL, TWO_ERR_ACQ ) |
        BCHP_MASK( XPT_PCROFFSET_CONTEXT0_PCROFFSET_CTRL, ONE_ERR_ACQ ) 
    );
    if( Settings->ConsecutiveErrorThreshold == 1 )
    {
        OneErrAcq = 1;
    }
    else if( Settings->ConsecutiveErrorThreshold >= 2 )
    {
        TwoErrAcq = 1;
    }
    Reg |= (
        BCHP_FIELD_DATA( XPT_PCROFFSET_CONTEXT0_PCROFFSET_CTRL, TWO_ERR_ACQ, TwoErrAcq ) |
        BCHP_FIELD_DATA( XPT_PCROFFSET_CONTEXT0_PCROFFSET_CTRL, ONE_ERR_ACQ, OneErrAcq )
    );
    BREG_Write32( hChannel->hReg, hChannel->BaseAddr + PCROFF_CTRL_OFFSET, Reg );

    Done:
    return( ExitCode );
}

BERR_Code BXPT_PcrOffset_ChangeStcRate(
    BXPT_PcrOffset_Handle hChannel,     /* [in] The channel handle */
    unsigned Increment,                 /* [in] STC increment value */
    unsigned Prescale                   /* [in] STC prescale value */
    )
{
    uint32_t Reg, RegAddr;

    BERR_Code ExitCode = BERR_SUCCESS;

    BDBG_ASSERT( hChannel );

    if( Increment > BXPT_MAX_STC_INCREMENT )
    {
        BDBG_ERR(( "Increment %u out of range. Max is %u", Increment, BXPT_MAX_STC_INCREMENT ));
        ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
        goto Done;
    }

    if( Prescale > BXPT_MAX_STC_PRESCALE )
    {
        BDBG_ERR(( "Prescale %u out of range. Max is %u", Prescale, BXPT_MAX_STC_PRESCALE ));
        ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
        goto Done;
    }

    RegAddr = GetStcRegAddr( BCHP_XPT_PCROFFSET_STC0_RATE_CTRL, hChannel->WhichStc );
    Reg = BREG_Read32( hChannel->hReg, RegAddr );
    Reg &= ~( 
        BCHP_MASK( XPT_PCROFFSET_STC0_RATE_CTRL, PRESCALE ) |
        BCHP_MASK( XPT_PCROFFSET_STC0_RATE_CTRL, INCREMENT ) 
    );
    Reg |= (
        BCHP_FIELD_DATA( XPT_PCROFFSET_STC0_RATE_CTRL, PRESCALE, Prescale ) |
        BCHP_FIELD_DATA( XPT_PCROFFSET_STC0_RATE_CTRL, INCREMENT, Increment )
    );
    BREG_Write32( hChannel->hReg, RegAddr, Reg );

    Done:
    return( ExitCode );
}

BERR_Code BXPT_PcrOffset_FreezeStc(
    BXPT_PcrOffset_Handle hChannel,     /* [in] The channel handle */
    bool Freeze                         /* [in] STC is frozen if true, unfrozen if false */
    )
{
    uint32_t Reg, RegAddr;

    BERR_Code ExitCode = BERR_SUCCESS;

    BDBG_ASSERT( hChannel );

    RegAddr = GetStcRegAddr( BCHP_XPT_PCROFFSET_STC0_CTRL, hChannel->WhichStc );
    Reg = BREG_Read32( hChannel->hReg, RegAddr );
    Reg &= ~( 
        BCHP_MASK( XPT_PCROFFSET_STC0_CTRL, FREEZE ) 
    );
    Reg |= (
        BCHP_FIELD_DATA( XPT_PCROFFSET_STC0_CTRL, FREEZE, Freeze == true ? 1 : 0 )
    );
    BREG_Write32( hChannel->hReg, RegAddr, Reg );

    return( ExitCode );
}

uint32_t BXPT_PcrOffset_GetStc(
    BXPT_PcrOffset_Handle hChannel      /* [in] The channel handle */
    )
{
    uint32_t Stc;

    BKNI_EnterCriticalSection();
    Stc = BXPT_PcrOffset_GetStc_isr( hChannel );
    BKNI_LeaveCriticalSection();
    return Stc;
}

uint32_t BXPT_PcrOffset_GetStc_isr(
    BXPT_PcrOffset_Handle hChannel      /* [in] The channel handle */
    )
{
    uint32_t Reg, RegAddr, Stc;

    BDBG_ASSERT( hChannel );

#ifdef BCHP_XPT_PCROFFSET_STC0
    RegAddr = GetStcRegAddr( BCHP_XPT_PCROFFSET_STC0, hChannel->WhichStc );
    Reg = BREG_Read32( hChannel->hReg, RegAddr );
    Stc = BCHP_GET_FIELD_DATA( Reg, XPT_PCROFFSET_STC0, COUNT );
#else
    {
        bool Mode;

        RegAddr = GetStcRegAddr( BCHP_XPT_PCROFFSET_STC0_CTRL, hChannel->WhichStc );
        Reg = BREG_Read32( hChannel->hReg, RegAddr );
        Mode = BCHP_GET_FIELD_DATA( Reg, XPT_PCROFFSET_STC0_CTRL, MODE );

        if( Mode ) 
        {
            /* Binary */
            RegAddr = GetStcRegAddr( BCHP_XPT_PCROFFSET_STC0_LO, hChannel->WhichStc );
            Reg = BREG_Read32( hChannel->hReg, RegAddr );
            Stc = BCHP_GET_FIELD_DATA( Reg, XPT_PCROFFSET_STC0_LO, COUNT );
        }
        else
        {
            /* Mod300 */
            RegAddr = GetStcRegAddr( BCHP_XPT_PCROFFSET_STC0_HI, hChannel->WhichStc );
            Reg = BREG_Read32( hChannel->hReg, RegAddr );
            Stc = BCHP_GET_FIELD_DATA( Reg, XPT_PCROFFSET_STC0_HI, COUNT ) << 22;

            RegAddr = GetStcRegAddr( BCHP_XPT_PCROFFSET_STC0_LO, hChannel->WhichStc );
            Reg = BREG_Read32( hChannel->hReg, RegAddr );
            Stc |= (BCHP_GET_FIELD_DATA( Reg, XPT_PCROFFSET_STC0_LO, COUNT ) >> 10);
        }
    }
#endif

    return Stc;
}

BERR_Code BXPT_PcrOffset_SetStc(
    BXPT_PcrOffset_Handle hChannel,     /* [in] The channel handle */
    uint32_t NewStc
    )
{
    BERR_Code Status;

    BKNI_EnterCriticalSection();
    Status = BXPT_PcrOffset_SetStc_isr( hChannel, NewStc );
    BKNI_LeaveCriticalSection();
    return Status;
}

BERR_Code BXPT_PcrOffset_SetStc_isr(
    BXPT_PcrOffset_Handle hChannel,     /* [in] The channel handle */
    uint32_t NewStc
    )
{
    uint32_t RegAddr;

    BERR_Code ExitCode = BERR_SUCCESS;

    BDBG_ASSERT( hChannel );

#ifdef BCHP_XPT_PCROFFSET_STC0
    RegAddr = GetStcRegAddr( BCHP_XPT_PCROFFSET_STC0, hChannel->WhichStc );
    BREG_Write32( hChannel->hReg, RegAddr, NewStc );
#else
    {
        bool Mode;
        uint32_t Reg;

        RegAddr = GetStcRegAddr( BCHP_XPT_PCROFFSET_STC0_CTRL, hChannel->WhichStc );
        Reg = BREG_Read32( hChannel->hReg, RegAddr );
        Mode = BCHP_GET_FIELD_DATA( Reg, XPT_PCROFFSET_STC0_CTRL, MODE );

        if( Mode ) 
        {
            /* Binary */
            RegAddr = GetStcRegAddr( BCHP_XPT_PCROFFSET_STC0_LO, hChannel->WhichStc );
            BREG_Write32( hChannel->hReg, RegAddr, NewStc );
        }
        else
        {
            /* Mod300 */
            RegAddr = GetStcRegAddr( BCHP_XPT_PCROFFSET_STC0_HI, hChannel->WhichStc );
            BREG_Write32( hChannel->hReg, RegAddr, (NewStc >> 22) & 0x3FF );
            RegAddr = GetStcRegAddr( BCHP_XPT_PCROFFSET_STC0_LO, hChannel->WhichStc );
            BREG_Write32( hChannel->hReg, RegAddr, (NewStc << 10) & 0xFFFFFC00 );
        }
    }
#endif

    return( ExitCode );
}

uint32_t BXPT_PcrOffset_GetOffset(
    BXPT_PcrOffset_Handle hChannel          /* [in] The channel handle */
    )
{
    uint32_t Offset;

    BKNI_EnterCriticalSection();
    Offset = BXPT_PcrOffset_GetOffset_isr( hChannel );
    BKNI_LeaveCriticalSection();
    return Offset;
}

uint32_t BXPT_PcrOffset_GetOffset_isr(
    BXPT_PcrOffset_Handle hChannel          /* [in] The channel handle */
    )
{
    uint32_t Reg;

    BDBG_ASSERT( hChannel );

    Reg = BREG_Read32( hChannel->hReg, hChannel->BaseAddr + PCROFF_OFFSET_OFFSET );
    return BCHP_GET_FIELD_DATA( Reg, XPT_PCROFFSET_CONTEXT0_OFFSET, PCR_OFFSET );
}

BERR_Code BXPT_PcrOffset_SetOffset(
    BXPT_PcrOffset_Handle hChannel,     /* [in] The channel handle */
    uint32_t UserPcrOffset                  /* [in] User-supplied PCR Offset */
    )
{
    BERR_Code Status;

    BKNI_EnterCriticalSection();
    Status = BXPT_PcrOffset_SetOffset_isr( hChannel, UserPcrOffset );
    BKNI_LeaveCriticalSection();
    return Status;
}

BERR_Code BXPT_PcrOffset_SetOffset_isr(
    BXPT_PcrOffset_Handle hChannel,     /* [in] The channel handle */
    uint32_t UserPcrOffset                  /* [in] User-supplied PCR Offset */
    )
{
    uint32_t Reg;

    BERR_Code ExitCode = BERR_SUCCESS;

    BDBG_ASSERT( hChannel );

    Reg = BREG_Read32( hChannel->hReg, hChannel->BaseAddr + PCROFF_OFFSET_OFFSET );
    Reg &= ~( 
        BCHP_MASK( XPT_PCROFFSET_CONTEXT0_OFFSET, PCR_OFFSET ) 
    );
    Reg |= (
        BCHP_FIELD_DATA( XPT_PCROFFSET_CONTEXT0_OFFSET, PCR_OFFSET, UserPcrOffset )
    );
    BREG_Write32( hChannel->hReg, hChannel->BaseAddr + PCROFF_OFFSET_OFFSET, Reg );

    return( ExitCode );
}

BERR_Code BXPT_PcrOffset_FreeRun(
    BXPT_PcrOffset_Handle hChannel,     /* [in] The channel handle */
    bool FreeRun                        /* [in] Free run if true. */
    )
{
    uint32_t Reg, RegAddr;

    uint32_t NewTimeBase = STC_FREE_RUNNING;
    BERR_Code ExitCode = BERR_SUCCESS;

    BDBG_ASSERT( hChannel );

    if( FreeRun == false )
        NewTimeBase = hChannel->CurrentTimeBase;

    RegAddr = GetStcRegAddr( BCHP_XPT_PCROFFSET_STC0_TIMEBASE_SEL, hChannel->WhichStc );
    Reg = BREG_Read32( hChannel->hReg, RegAddr );
    Reg &= ~( 
        BCHP_MASK( XPT_PCROFFSET_STC0_TIMEBASE_SEL, TIMEBASE_SEL ) 
    );
    Reg |= (
        BCHP_FIELD_DATA( XPT_PCROFFSET_STC0_TIMEBASE_SEL, TIMEBASE_SEL, NewTimeBase )
    );
    BREG_Write32( hChannel->hReg, RegAddr, Reg );

    return( ExitCode );
}

BERR_Code BXPT_PcrOffset_PushPidChannel(
    BXPT_PcrOffset_Handle hChannel,     /* [in] The channel handle */
    unsigned int PidChannel             /* [in] Channel carrying the PID to splice. */
    )
{
    uint32_t RdPtr, WrPtr, Reg;
    BERR_Code ExitCode = BERR_SUCCESS;

    BDBG_ASSERT( hChannel );

    if( BXPT_PcrOffset_GetQueueDepth( hChannel ) == MAX_PCR_SPLICING_ENTRIES )
    {
        BDBG_ERR(( "Splicing stack full!" ));
        ExitCode = BERR_TRACE( BXPT_ERR_NO_AVAILABLE_RESOURCES );
        goto Done;
    }

    Reg = BREG_Read32( hChannel->hReg, hChannel->BaseAddr + PCROFF_SPLICE_RDPTR_OFFSET );
    RdPtr = BCHP_GET_FIELD_DATA( Reg, XPT_PCROFFSET_CONTEXT0_SPLICE_RD_PTR, NEXT_PID_CH_RD_PTR );
            
    Reg = BREG_Read32( hChannel->hReg, hChannel->BaseAddr + PCROFF_SPLICE_WRPTR_OFFSET );
    WrPtr = BCHP_GET_FIELD_DATA( Reg, XPT_PCROFFSET_CONTEXT0_SPLICE_WR_PTR, NEXT_PID_CH_WR_PTR );       

    SetSplicePidChannel( hChannel, WrPtr, PidChannel );

    WrPtr = ( WrPtr + 1 ) % BXPT_P_SPLICING_QUEUE_SIZE;
    BREG_Write32( hChannel->hReg, hChannel->BaseAddr + PCROFF_SPLICE_WRPTR_OFFSET, WrPtr );

    /* Set the enable, just in case. Should happen automatically when RD != WR */
    Reg = BREG_Read32( hChannel->hReg, hChannel->BaseAddr + PCROFF_CTRL_OFFSET );
    Reg &= ~( 
        BCHP_MASK( XPT_PCROFFSET_CONTEXT0_PCROFFSET_CTRL, SPLICE_ENABLE ) 
    );
    Reg |= (
        BCHP_FIELD_DATA( XPT_PCROFFSET_CONTEXT0_PCROFFSET_CTRL, SPLICE_ENABLE, 1 )
    );
    BREG_Write32( hChannel->hReg, hChannel->BaseAddr + PCROFF_CTRL_OFFSET, Reg );
    
    Done:
    return( ExitCode );
}

BERR_Code BXPT_PcrOffset_ClearQueue(
    BXPT_PcrOffset_Handle hChannel      /* [in] The channel handle */
    )
{
    uint32_t Reg;

    BERR_Code ExitCode = BERR_SUCCESS;

    BDBG_ASSERT( hChannel );

    Reg = BREG_Read32( hChannel->hReg, hChannel->BaseAddr + PCROFF_CTRL_OFFSET );
    Reg &= ~( 
        BCHP_MASK( XPT_PCROFFSET_CONTEXT0_PCROFFSET_CTRL, SPLICE_ENABLE ) 
    );
    BREG_Write32( hChannel->hReg, hChannel->BaseAddr + PCROFF_CTRL_OFFSET, Reg );

    Reg = BREG_Read32( hChannel->hReg, hChannel->BaseAddr + PCROFF_SPLICE_RDPTR_OFFSET );
    Reg &= ~( 
        BCHP_MASK( XPT_PCROFFSET_CONTEXT0_SPLICE_RD_PTR, NEXT_PID_CH_RD_PTR ) 
    );
    BREG_Write32( hChannel->hReg, hChannel->BaseAddr + PCROFF_SPLICE_RDPTR_OFFSET, Reg );

    Reg = BREG_Read32( hChannel->hReg, hChannel->BaseAddr + PCROFF_SPLICE_WRPTR_OFFSET );
    Reg &= ~( 
        BCHP_MASK( XPT_PCROFFSET_CONTEXT0_SPLICE_WR_PTR, NEXT_PID_CH_WR_PTR ) 
    );
    BREG_Write32( hChannel->hReg, hChannel->BaseAddr + PCROFF_SPLICE_WRPTR_OFFSET, Reg );

    BREG_Write32( hChannel->hReg, hChannel->BaseAddr + PCROFF_SPLICE_CHAN01_OFFSET, 0 );
    BREG_Write32( hChannel->hReg, hChannel->BaseAddr + PCROFF_SPLICE_CHAN23_OFFSET, 0 );
    BREG_Write32( hChannel->hReg, hChannel->BaseAddr + PCROFF_SPLICE_CHAN45_OFFSET, 0 );
    BREG_Write32( hChannel->hReg, hChannel->BaseAddr + PCROFF_SPLICE_CHAN67_OFFSET, 0 );

    return( ExitCode );
}

unsigned BXPT_PcrOffset_GetCountdown(
    BXPT_PcrOffset_Handle hChannel      /* [in] The channel handle */
    )
{
    uint32_t Reg;

    BDBG_ASSERT( hChannel );

    Reg = BREG_Read32( hChannel->hReg, hChannel->BaseAddr + PCROFF_SPLICE_STATUS_OFFSET );
    return BCHP_GET_FIELD_DATA( Reg, XPT_PCROFFSET_CONTEXT0_SPLICE_STATUS, COUNT );
}

unsigned BXPT_PcrOffset_GetQueueDepth(
    BXPT_PcrOffset_Handle hChannel      /* [in] The channel handle */
    )
{
    uint32_t RdPtr, WrPtr, Reg;

    unsigned Depth = 0;

    BDBG_ASSERT( hChannel );

    Reg = BREG_Read32( hChannel->hReg, hChannel->BaseAddr + PCROFF_SPLICE_RDPTR_OFFSET );
    RdPtr = BCHP_GET_FIELD_DATA( Reg, XPT_PCROFFSET_CONTEXT0_SPLICE_RD_PTR, NEXT_PID_CH_RD_PTR );
            
    Reg = BREG_Read32( hChannel->hReg, hChannel->BaseAddr + PCROFF_SPLICE_WRPTR_OFFSET );
    WrPtr = BCHP_GET_FIELD_DATA( Reg, XPT_PCROFFSET_CONTEXT0_SPLICE_WR_PTR, NEXT_PID_CH_WR_PTR );       

    if( RdPtr < WrPtr )
        Depth = WrPtr - RdPtr;
    else if( WrPtr < RdPtr )
        Depth = BXPT_P_SPLICING_QUEUE_SIZE - RdPtr + WrPtr;

    return( Depth );
}

void BXPT_PcrOffset_RegenOffset_isr(
    BXPT_PcrOffset_Handle hPcrOff
    )
{
    uint32_t Addr;
    uint32_t EndRegister = PCROFF_OFFSET_STATUS7;

    BDBG_ASSERT( hPcrOff );

    /* Old Code
    Addr = hPcrOff->BaseAddr + PCROFF_OFFSET_OFFSET; 
    BREG_Write32( hPcrOff->hReg, Addr, BREG_Read32( hPcrOff->hReg, Addr ) );
    */

#ifdef BCHP_XPT_PCROFFSET_CONTEXT0_OFFSET_STATUS_15
    EndRegister = PCROFF_OFFSET_STATUS15;
#endif

    for(Addr=(hPcrOff->BaseAddr + PCROFF_OFFSET_STATUS0); Addr <= (hPcrOff->BaseAddr + EndRegister); Addr+=4)
        BREG_Write32( hPcrOff->hReg, Addr, 0 );
}

BERR_Code BXPT_PcrOffset_EnableOffset(
    BXPT_PcrOffset_Handle hPcrOff,
    uint32_t PidChannelNum,
    bool FixedOffsetEn,
    bool JitterAdjustEn
    )
{
    uint32_t Reg, RegAddr;

    BERR_Code ExitCode = BERR_SUCCESS;

    BDBG_ASSERT( hPcrOff );

    if( PidChannelNum >= BXPT_NUM_PID_CHANNELS )
    {
        BDBG_ERR(( "PidChannelNum %u out of range!", PidChannelNum ));
        ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
        goto Done;
    }

    RegAddr = BCHP_XPT_PCROFFSET_PID_CONFIG_TABLE_i_ARRAY_BASE + PidChannelNum * PID_CHNL_STEPSIZE;
    Reg = BREG_Read32( hPcrOff->hReg, RegAddr );

    Reg &= ~( 
        BCHP_MASK( XPT_PCROFFSET_PID_CONFIG_TABLE_i, JITTER_DIS ) |
        BCHP_MASK( XPT_PCROFFSET_PID_CONFIG_TABLE_i, FIXED_OFFSET_EN ) |
        BCHP_MASK( XPT_PCROFFSET_PID_CONFIG_TABLE_i, PCROFFSET_EN ) |
        BCHP_MASK( XPT_PCROFFSET_PID_CONFIG_TABLE_i, OFFSET_INDEX ) 
    );
    Reg |= (
        BCHP_FIELD_DATA( XPT_PCROFFSET_PID_CONFIG_TABLE_i, JITTER_DIS, JitterAdjustEn == true ? 0 : 1 ) |
        BCHP_FIELD_DATA( XPT_PCROFFSET_PID_CONFIG_TABLE_i, FIXED_OFFSET_EN, FixedOffsetEn == true ? 1 : 0 ) |
        BCHP_FIELD_DATA( XPT_PCROFFSET_PID_CONFIG_TABLE_i, PCROFFSET_EN, 1 ) |
        BCHP_FIELD_DATA( XPT_PCROFFSET_PID_CONFIG_TABLE_i, OFFSET_INDEX, hPcrOff->ChannelNo )
    );

    BREG_Write32( hPcrOff->hReg, RegAddr, Reg );

    Done:
    return ExitCode;    
}                                                                        

void BXPT_PcrOffset_DisableOffset(
    BXPT_PcrOffset_Handle hPcrOff,
    uint32_t PidChannelNum
    )
{
    uint32_t Reg, RegAddr;

    BDBG_ASSERT( hPcrOff );

    if( PidChannelNum >= BXPT_NUM_PID_CHANNELS )
    {
        BDBG_ERR(( "PidChannelNum %u out of range!", PidChannelNum ));
        return;
    }

    RegAddr = BCHP_XPT_PCROFFSET_PID_CONFIG_TABLE_i_ARRAY_BASE + PidChannelNum * PID_CHNL_STEPSIZE;
    Reg = BREG_Read32( hPcrOff->hReg, RegAddr );

    Reg &= ~( 
        BCHP_MASK( XPT_PCROFFSET_PID_CONFIG_TABLE_i, JITTER_DIS ) |
        BCHP_MASK( XPT_PCROFFSET_PID_CONFIG_TABLE_i, FIXED_OFFSET_EN ) |
        BCHP_MASK( XPT_PCROFFSET_PID_CONFIG_TABLE_i, PCROFFSET_EN ) |
        BCHP_MASK( XPT_PCROFFSET_PID_CONFIG_TABLE_i, OFFSET_INDEX ) 
    );

    BREG_Write32( hPcrOff->hReg, RegAddr, Reg );
}

void SetSplicePidChannel(
    BXPT_PcrOffset_Handle hPcrOff,
    unsigned ChannelNum,
    unsigned Pid
    )
{
    uint32_t Reg, RegAddr, Shift;

    /*
    ** Each splice register holds entries for 2 channels, so divide the 
    ** channel number by 2 to figure out which reg holds the channel we're after. 
    ** Then, mult by 4 since each channel is 4 bytes wide. Add in the physical
    ** address of the first register, and we're done.
    */
    RegAddr = hPcrOff->BaseAddr + 4 * ( ChannelNum / 2 );
    RegAddr += PCROFF_SPLICE_CHAN01_OFFSET;
    Reg = BREG_Read32( hPcrOff->hReg, RegAddr );

    /* Of the two channels in the reg, the lower numbered channel is in lower half of the word */
    Shift = ChannelNum % 2 ? 0 : 16;
    Reg &= ( 0xFF << Shift );
    Reg |= ( Pid << Shift );
    BREG_Write32( hPcrOff->hReg, RegAddr, Reg );
}

uint32_t GetStcRegAddr( 
    uint32_t Ctx0Addr,
    unsigned WhichStc
    )
{
    return Ctx0Addr + STC_CONTEXT_STEP * WhichStc;
}

bool BXPT_PcrOffset_IsOffsetValid(
    BXPT_PcrOffset_Handle hPcrOff
    )
{
    bool Status;

    BKNI_EnterCriticalSection();
    Status = BXPT_PcrOffset_IsOffsetValid_isr( hPcrOff );
    BKNI_LeaveCriticalSection();
    return Status;
}

bool BXPT_PcrOffset_IsOffsetValid_isr(
    BXPT_PcrOffset_Handle hPcrOff
    )
{
    uint32_t Reg;
    bool Status;

    BDBG_ASSERT( hPcrOff );

    Reg = BREG_Read32( hPcrOff->hReg, hPcrOff->BaseAddr + PCROFF_OFFSET_INVALID );
    Status = BCHP_GET_FIELD_DATA( Reg, XPT_PCROFFSET_CONTEXT0_OFFSET_VALID, PCR_OFFSET_VALID ) == 1 ? true : false;

    return( Status );
}

void  BXPT_PcrOffset_ForceInvalid( BXPT_PcrOffset_Handle hPcrOff)
{
    BKNI_EnterCriticalSection();
    BXPT_PcrOffset_ForceInvalid_isr( hPcrOff );
    BKNI_LeaveCriticalSection();
    return;
}

void  BXPT_PcrOffset_ForceInvalid_isr( BXPT_PcrOffset_Handle hPcrOff)
{
    uint32_t Reg;

    BDBG_ASSERT( hPcrOff );
    Reg = BREG_Read32( hPcrOff->hReg, hPcrOff->BaseAddr + PCROFF_OFFSET_INVALID );
    Reg &= ( BCHP_FIELD_DATA( XPT_PCROFFSET_CONTEXT0_OFFSET_VALID, PCR_OFFSET_VALID, 0x0 ) );
    BREG_Write32( hPcrOff->hReg, hPcrOff->BaseAddr + PCROFF_OFFSET_INVALID, Reg );

    return;
}

void BXPT_PcrOffset_CaptureStcOnce(
    BXPT_PcrOffset_Handle hPcrOff,      /* [in] Handle for the PCR Offset channel to use */
	bool Enable							/* [in] true to capture once, false to disable a pending capture */
    )
{
	BKNI_EnterCriticalSection();
	BXPT_PcrOffset_CaptureStcOnce_isr( hPcrOff, Enable );
	BKNI_LeaveCriticalSection();
	return;
}

void BXPT_PcrOffset_CaptureStcOnce_isr(
    BXPT_PcrOffset_Handle hPcrOff,      /* [in] Handle for the PCR Offset channel to use */
	bool Enable							/* [in] true to capture once, false to disable a pending capture */
    )
{
    uint32_t RegAddr, Reg;

    RegAddr = GetStcRegAddr( BCHP_XPT_PCROFFSET_STC0_CAPTURE_PCR_ONCE, hPcrOff->WhichStc );
    Reg = BREG_Read32( hPcrOff->hReg, RegAddr );
    Reg &= ~( 
        BCHP_MASK( XPT_PCROFFSET_STC0_CAPTURE_PCR_ONCE, ENABLE ) 
    );
    Reg |= (
        BCHP_FIELD_DATA( XPT_PCROFFSET_STC0_CAPTURE_PCR_ONCE, ENABLE, Enable == true ? 1 : 0 )
    );
    BREG_Write32( hPcrOff->hReg, RegAddr, Reg );
}

bool BXPT_P_PcrOffset_IsPidChannelInUse(
    BXPT_Handle hXpt,               /* [in] The transport handle */
    uint32_t PidChannelNum              /* [in] Which PID channel to disable offsets for */
    )
{
    unsigned ChannelNum;

    for( ChannelNum = 0; ChannelNum < BXPT_NUM_PCR_OFFSET_CHANNELS; ChannelNum++ )
    {
	    uint32_t PidChannelReg = BREG_Read32( hXpt->hRegister, BCHP_XPT_PCROFFSET_CONTEXT0_PP_PCR_PID_CH_NUM + ( ChannelNum * PCROFF_CONTEXT_STEP ) );
        uint32_t ControlReg = BREG_Read32( hXpt->hRegister, BCHP_XPT_PCROFFSET_CONTEXT0_PCROFFSET_CTRL + ( ChannelNum * PCROFF_CONTEXT_STEP ) );

        /* The PID channel is mapped AND the offset context is enabled. */
	    if( BCHP_GET_FIELD_DATA( PidChannelReg, XPT_PCROFFSET_CONTEXT0_PP_PCR_PID_CH_NUM, PCR_PID_CH_NUM ) == PidChannelNum 
	    && BCHP_GET_FIELD_DATA( ControlReg, XPT_PCROFFSET_CONTEXT0_PCROFFSET_CTRL, CTXT_ENABLE ) )
        {
            return true;
        }
    }

    return false;
}

BERR_Code BXPT_PcrOffset_GetIntId(
    BXPT_PcrOffset_Handle hChannel,
    BXPT_PcrOffsetIntName Name,
    BINT_Id *IntId
    )
{
    uint32_t RegAddr = BCHP_XPT_PCROFFSET_INTERRUPT0_STATUS;
    uint32_t IntShift = 0;

    BERR_Code ExitCode = BERR_SUCCESS;

    BDBG_ASSERT( hChannel );

    switch( hChannel->ChannelNo )
    {
        case 0:
        RegAddr = BCHP_XPT_PCROFFSET_INTERRUPT0_STATUS;
        IntShift = 0;
        break;

        case 1:
        RegAddr = BCHP_XPT_PCROFFSET_INTERRUPT0_STATUS;
        IntShift = 8;
        break;

        case 2:
        RegAddr = BCHP_XPT_PCROFFSET_INTERRUPT0_STATUS;
        IntShift = 16;
        break;

        case 3:
        RegAddr = BCHP_XPT_PCROFFSET_INTERRUPT0_STATUS;
        IntShift = 24;
        break;

        case 4:
        RegAddr = BCHP_XPT_PCROFFSET_INTERRUPT1_STATUS;
        IntShift = 0;
        break;

        case 5:
        RegAddr = BCHP_XPT_PCROFFSET_INTERRUPT1_STATUS;
        IntShift = 8;
        break;

        case 6:
        RegAddr = BCHP_XPT_PCROFFSET_INTERRUPT1_STATUS;
        IntShift = 16;
        break;

        case 7:
        RegAddr = BCHP_XPT_PCROFFSET_INTERRUPT1_STATUS;
        IntShift = 24;
        break;
    
        case 8:
        RegAddr = BCHP_XPT_PCROFFSET_INTERRUPT2_STATUS;
        IntShift = 0;
        break;

        case 9:
        RegAddr = BCHP_XPT_PCROFFSET_INTERRUPT2_STATUS;
        IntShift = 8;
        break;

        case 10:
        RegAddr = BCHP_XPT_PCROFFSET_INTERRUPT2_STATUS;
        IntShift = 16;
        break;

        case 11:
        RegAddr = BCHP_XPT_PCROFFSET_INTERRUPT2_STATUS;
        IntShift = 24;
        break;
    
        case 12:
        RegAddr = BCHP_XPT_PCROFFSET_INTERRUPT3_STATUS;
        IntShift = 0;
        break;

        case 13:
        RegAddr = BCHP_XPT_PCROFFSET_INTERRUPT3_STATUS;
        IntShift = 8;
        break;

        case 14:
        RegAddr = BCHP_XPT_PCROFFSET_INTERRUPT3_STATUS;
        IntShift = 16;
        break;

        case 15:
        RegAddr = BCHP_XPT_PCROFFSET_INTERRUPT3_STATUS;
        IntShift = 24;
        break;

        default:
        BDBG_ERR(( "Unsupported offset channel" ));
        ExitCode = BERR_TRACE( BXPT_ERR_NO_AVAILABLE_RESOURCES );
        goto Done;
    }

    *IntId = BCHP_INT_ID_CREATE( RegAddr, Name + IntShift );

    Done:
    return( ExitCode );
}

void BXPT_P_PcrOffset_ModuleInit( 
    BXPT_Handle hXpt 
    )
{
    unsigned Reg = 
        BCHP_FIELD_DATA( XPT_PCROFFSET_STC_BROADCAST_SEL, STC_BCAST0_SEL, 0 )

#if BXPT_NUM_STCS > 1
        | BCHP_FIELD_DATA( XPT_PCROFFSET_STC_BROADCAST_SEL, STC_BCAST1_SEL, 1 )
#endif

#if BXPT_NUM_STCS > 2
        | BCHP_FIELD_DATA( XPT_PCROFFSET_STC_BROADCAST_SEL, STC_BCAST2_SEL, 2 )
#endif

#if BXPT_NUM_STCS > 3
        | BCHP_FIELD_DATA( XPT_PCROFFSET_STC_BROADCAST_SEL, STC_BCAST3_SEL, 3 )
#endif

#if BXPT_NUM_STCS > 4
        | BCHP_FIELD_DATA( XPT_PCROFFSET_STC_BROADCAST_SEL, STC_BCAST4_SEL, 4 )
#endif

#if BXPT_NUM_STCS > 5
        | BCHP_FIELD_DATA( XPT_PCROFFSET_STC_BROADCAST_SEL, STC_BCAST5_SEL, 5 )
#endif

#if BXPT_NUM_STCS > 6
        | BCHP_FIELD_DATA( XPT_PCROFFSET_STC_BROADCAST_SEL, STC_BCAST6_SEL, 6 )
#endif

#if BXPT_NUM_STCS > 7
        | BCHP_FIELD_DATA( XPT_PCROFFSET_STC_BROADCAST_SEL, STC_BCAST7_SEL, 7 )
#endif
    ;

#if BXPT_NUM_STCS > 8
    #error "PCR Offset init needs updating"
#endif

    BREG_Write32( hXpt->hRegister, BCHP_XPT_PCROFFSET_STC_BROADCAST_SEL, Reg );
}

#if BXPT_HAS_TSMUX

void BXPT_PcrOffset_GetNRTConfig(
    BXPT_PcrOffset_Handle hPcrOffset,   /* [in] The channel handle */
    BXPT_PcrOffset_NRTConfig *Config
    )
{
    uint32_t Reg;
    unsigned WindowDivisor;

    BDBG_ASSERT( hPcrOffset );
    BDBG_ASSERT( Config );

    Reg = BREG_Read32( hPcrOffset->hReg, GetStcRegAddr( BCHP_XPT_PCROFFSET_STC0_CTRL, hPcrOffset->WhichStc ) );
    Config->PairedStc = BCHP_GET_FIELD_DATA( Reg, XPT_PCROFFSET_STC0_CTRL, STC_AV_SEL );
    Config->EnableAvWindowComparison = BCHP_GET_FIELD_DATA( Reg, XPT_PCROFFSET_STC0_CTRL, AV_WINDOW_EN ) ? true : false;

    /* The units used in AV_WINDOW change with the channels' mode (either MOD300 or Binary).
    ** For MOD300, the units are 45kH ticks, whereas in binary they are 27 MHz. We want to scale this so 
    ** that the AvWindow is expressed as mS of delay. */ 
    WindowDivisor = BCHP_GET_FIELD_DATA( Reg, XPT_PCROFFSET_STC0_CTRL, MODE ) ? 27000 : 45;
    Config->AvWindow = BCHP_GET_FIELD_DATA( 
        BREG_Read32( hPcrOffset->hReg, GetStcRegAddr( BCHP_XPT_PCROFFSET_STC0_AV_WINDOW, hPcrOffset->WhichStc ) ), 
        XPT_PCROFFSET_STC0_AV_WINDOW, AV_WINDOW ) / WindowDivisor;
    
    Config->ExternalTriggerNum = 0xFFFF;
    switch( BCHP_GET_FIELD_DATA( Reg, XPT_PCROFFSET_STC0_CTRL, TRIG_MODE ) ) 
    {
        case 0: 
        Config->TriggerMode = BXPT_PcrOffset_StcTriggerMode_eTimebase;
        break;

        case 1: 
        Config->TriggerMode = BXPT_PcrOffset_StcTriggerMode_eExternalTrig;
        Config->ExternalTriggerNum = 0;
        break;
    
        case 2: 
        Config->TriggerMode = BXPT_PcrOffset_StcTriggerMode_eExternalTrig;
        Config->ExternalTriggerNum = 1;
        break;
    
        case 3: 
        Config->TriggerMode = BXPT_PcrOffset_StcTriggerMode_eSoftIncrement;
        break;

        case 4: 
        Config->TriggerMode = BXPT_PcrOffset_StcTriggerMode_eExternalTrig;
        Config->ExternalTriggerNum = 2;
        break;
    
        case 5: 
        Config->TriggerMode = BXPT_PcrOffset_StcTriggerMode_eExternalTrig;
        Config->ExternalTriggerNum = 3;
        break;
    
        default:
        BDBG_ERR(( "Unknown TRIG_MODE. Defaulting to timebase" ));
        Config->TriggerMode = BXPT_PcrOffset_StcTriggerMode_eTimebase;
        break;
    }

    Config->StcIncrement = (uint64_t) BCHP_GET_FIELD_DATA( 
        BREG_Read32( hPcrOffset->hReg, GetStcRegAddr( BCHP_XPT_PCROFFSET_STC0_INC_HI, hPcrOffset->WhichStc ) ), 
        XPT_PCROFFSET_STC0_INC_HI, INCREMENT_HI ) << 32;
    Config->StcIncrement |= (uint64_t) BCHP_GET_FIELD_DATA( 
        BREG_Read32( hPcrOffset->hReg, GetStcRegAddr( BCHP_XPT_PCROFFSET_STC0_INC_LO, hPcrOffset->WhichStc ) ), 
        XPT_PCROFFSET_STC0_INC_LO, INCREMENT_LO );
}

BERR_Code BXPT_PcrOffset_SetNRTConfig(
    BXPT_PcrOffset_Handle hPcrOffset,   /* [in] The channel handle */
    const BXPT_PcrOffset_NRTConfig *Config
    )
{
    uint32_t Reg, RegAddr;
    unsigned WindowMultiplier;

    BERR_Code ExitCode = BERR_SUCCESS;
    unsigned TriggerMode = 0;

    BDBG_ASSERT( hPcrOffset );
    BDBG_ASSERT( Config );

    switch( Config->TriggerMode ) 
    {
        case BXPT_PcrOffset_StcTriggerMode_eTimebase: 
        TriggerMode = 0;
        break;

        case BXPT_PcrOffset_StcTriggerMode_eExternalTrig: 
        if( BXPT_MAX_EXTERNAL_TRIGS <= Config->ExternalTriggerNum ) 
        {
            BDBG_ERR(( "Invalid external trigger number", Config->ExternalTriggerNum ));
            ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
            goto Done;
        }
        if( Config->ExternalTriggerNum < 2 )
            TriggerMode = Config->ExternalTriggerNum + 1;    /* Trigs 0 and 1 map to TRIG_MODE 1 and 2 */
        else
            TriggerMode = Config->ExternalTriggerNum + 2;    /* Trigs 2 and up map to TRIG_MODE 4 and up */
        break;
    
        case BXPT_PcrOffset_StcTriggerMode_eSoftIncrement: 
        TriggerMode = 3;
        break;
    
        default:
        case BXPT_PcrOffset_StcTriggerMode_eMax: 
        BDBG_ERR(( "Unknown TriggerMode" ));
        ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
        goto Done;
        break;
    }

    RegAddr = GetStcRegAddr( BCHP_XPT_PCROFFSET_STC0_CTRL, hPcrOffset->WhichStc );
    Reg = BREG_Read32( hPcrOffset->hReg, RegAddr );
    Reg &= ~( 
        BCHP_MASK( XPT_PCROFFSET_STC0_CTRL, TRIG_MODE ) |
        BCHP_MASK( XPT_PCROFFSET_STC0_CTRL, STC_AV_SEL ) |
        BCHP_MASK( XPT_PCROFFSET_STC0_CTRL, AV_WINDOW_EN ) 
    );
    Reg |= (
        BCHP_FIELD_DATA( XPT_PCROFFSET_STC0_CTRL, TRIG_MODE, TriggerMode ) |
        BCHP_FIELD_DATA( XPT_PCROFFSET_STC0_CTRL, STC_AV_SEL, Config->PairedStc ) |
        BCHP_FIELD_DATA( XPT_PCROFFSET_STC0_CTRL, AV_WINDOW_EN, Config->EnableAvWindowComparison ? 1 : 0 )
    );
    BREG_Write32( hPcrOffset->hReg, RegAddr, Reg );

    /* The units used in AV_WINDOW change with the channels' mode (either MOD300 or Binary).
    ** For MOD300, the units are 45kH ticks, whereas in binary they are 27 MHz. We want to scale this so 
    ** that the AvWindow is expressed as mS of delay. */ 
    WindowMultiplier = BCHP_GET_FIELD_DATA( Reg, XPT_PCROFFSET_STC0_CTRL, MODE ) ? 27000 : 45;
    RegAddr = GetStcRegAddr( BCHP_XPT_PCROFFSET_STC0_AV_WINDOW, hPcrOffset->WhichStc );
    Reg = BREG_Read32( hPcrOffset->hReg, RegAddr );
    Reg &= ~( 
        BCHP_MASK( XPT_PCROFFSET_STC0_AV_WINDOW, AV_WINDOW ) 
    );
    Reg |= (
        BCHP_FIELD_DATA( XPT_PCROFFSET_STC0_AV_WINDOW, AV_WINDOW, Config->AvWindow * WindowMultiplier )
    );
    BREG_Write32( hPcrOffset->hReg, RegAddr, Reg );

    if( BXPT_PcrOffset_StcTriggerMode_eSoftIncrement != Config->TriggerMode )
    {
        RegAddr = GetStcRegAddr( BCHP_XPT_PCROFFSET_STC0_INC_HI, hPcrOffset->WhichStc );
        Reg = BREG_Read32( hPcrOffset->hReg, RegAddr );
        Reg &= ~( 
            BCHP_MASK( XPT_PCROFFSET_STC0_INC_HI, INCREMENT_HI ) 
        );
        Reg |= (
            BCHP_FIELD_DATA( XPT_PCROFFSET_STC0_INC_HI, INCREMENT_HI, (uint32_t) (Config->StcIncrement >> 32) )
        );
        BREG_Write32( hPcrOffset->hReg, RegAddr, Reg );

        RegAddr = GetStcRegAddr( BCHP_XPT_PCROFFSET_STC0_INC_LO, hPcrOffset->WhichStc );
        Reg = BREG_Read32( hPcrOffset->hReg, RegAddr );
        Reg &= ~( 
            BCHP_MASK( XPT_PCROFFSET_STC0_INC_LO, INCREMENT_LO ) 
        );
        Reg |= (
            BCHP_FIELD_DATA( XPT_PCROFFSET_STC0_INC_LO, INCREMENT_LO, (uint32_t) (Config->StcIncrement & 0xFFFFFFFF) )
        );
        BREG_Write32( hPcrOffset->hReg, RegAddr, Reg );
    }

    Done:
    return ExitCode;
}

void BXPT_PcrOffset_GetSoftIncrementRegisterOffsets(
    BXPT_PcrOffset_Handle hPcrOffset,   /* [in] The channel handle */
    BAVC_Xpt_StcSoftIncRegisters *RegMap
    )
{
    BDBG_ASSERT( hPcrOffset );
    BDBG_ASSERT( RegMap );

    RegMap->StcIncLo = GetStcRegAddr( BCHP_XPT_PCROFFSET_STC0_INC_LO, hPcrOffset->WhichStc );
    RegMap->StcIncHi = GetStcRegAddr( BCHP_XPT_PCROFFSET_STC0_INC_HI, hPcrOffset->WhichStc );
    RegMap->IncTrigger = GetStcRegAddr( BCHP_XPT_PCROFFSET_STC0_INC_TRIG, hPcrOffset->WhichStc );
}

#endif


void BXPT_PcrOffset_Acquire(
    BXPT_PcrOffset_Handle hChannel
    )
{
    uint32_t Reg;

    BDBG_ASSERT( hChannel );

    Reg = BREG_Read32( hChannel->hReg, hChannel->BaseAddr + PCROFF_OFFSET_STATE );
    Reg &= ~( 
        BCHP_MASK( XPT_PCROFFSET_CONTEXT0_OFFSET_STATE, OFFSET_STATE ) 
    );
    Reg |= (
        BCHP_FIELD_DATA( XPT_PCROFFSET_CONTEXT0_OFFSET_STATE, OFFSET_STATE, 1 )
    );
    BREG_Write32( hChannel->hReg, hChannel->BaseAddr + PCROFF_OFFSET_STATE, Reg );
}


