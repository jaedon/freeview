/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxpt_pcr_offset.c $
 * $brcm_Revision: Hydra_Software_Devel/31 $
 * $brcm_Date: 5/22/12 2:36p $
 *
 * Porting interface code for the data transport core. 
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/xpt/7400/bxpt_pcr_offset.c $
 * 
 * Hydra_Software_Devel/31   5/22/12 2:36p gmullen
 * SW7401-2177: Added bounds checks for BXPT_PcrOffset_ChangeStcRate()
 * 
 * Hydra_Software_Devel/30   1/18/11 11:36a rjlewis
 * SW7420-1383: Added support for BDBG_OBJECT interface
 * 
 * Hydra_Software_Devel/29   8/30/10 5:24p gmullen
 * SW7403-924: Protected access to the PID and SPID table from other
 * modules in the PI
 * 
 * Hydra_Software_Devel/28   4/23/10 11:21a gmullen
 * SW7405-4254: Route PCR PID channel to RAVE only when stream PCRs are
 * used.
 * 
 * Hydra_Software_Devel/27   7/8/09 4:27p gmullen
 * PR56110: Fixed compilation error for older PCR Offset hw.
 * 
 * Hydra_Software_Devel/26   6/8/09 4:40p gmullen
 * PR55611: Added support for selecting which DPCR block drives the STC to
 * _GetSettings() and _SetSettings()
 * 
 * Hydra_Software_Devel/25   9/22/08 2:53p gmullen
 * PR47074: Do not disable PID channel if its used by the PCR Offset
 * block.
 * 
 * Hydra_Software_Devel/24   7/9/08 9:18a gmullen
 * PR44665: Added BXPT_PcrOffset_CaptureStcOnce() and
 * BXPT_PcrOffset_CaptureStcOnce_isr()
 * 
 * Hydra_Software_Devel/23   5/15/08 9:25a gmullen
 * PR42537: Added BXPT_PcrOffset_ReloadPcrPidChannel_isr()
 * 
 * Hydra_Software_Devel/22   4/15/08 8:43a gmullen
 * PR41735: Force offset to invalid state during SetSettings when HostPcrs
 * are used.
 * 
 * Hydra_Software_Devel/21   2/28/08 9:22a gmullen
 * PR39387: Fixed compilation bug: register name changed from one chip to
 * another.
 * 
 * Hydra_Software_Devel/20   2/27/08 5:02p piyushg
 * PR39387: Added a new API BXPT_PcrOffset_ForceInvalid to
 * clear the PCR_OFFSET_VALID bit.
 * 
 * Hydra_Software_Devel/19   2/27/08 3:28p piyushg
 * PR39387: Added a new API BXPT_PcrOffset_ForceInvalid to
 * clear the PCR_OFFSET_VALID bit.
 * 
 * Hydra_Software_Devel/18   2/19/08 2:31p gmullen
 * PR38918: Added BXPT_PcrOffset_IsOffsetValid() and
 * BXPT_PcrOffset_IsOffsetValid_isr()
 * 
 * Hydra_Software_Devel/17   8/17/07 11:23a gmullen
 * PR34143: Changed offset threshold default to 8
 * 
 * Hydra_Software_Devel/16   6/5/07 8:52p gmullen
 * PR31834: PCR jitter correction can now be disabled.
 * 
 * Hydra_Software_Devel/15   4/2/07 11:41a gmullen
 * PR28674: Map decode channels to R pipe when enabling channel
 * destination.
 * 
 * Hydra_Software_Devel/14   3/13/07 11:03a gmullen
 * PR28674: Don't set PID channel destination in SetSettings() call.
 * 
 * Hydra_Software_Devel/13   2/14/07 1:10p gmullen
 * PR27642: Added support for 7405.
 * 
 * Hydra_Software_Devel/12   2/6/07 7:05p gmullen
 * PR26217: Added B0 support.
 * 
 * Hydra_Software_Devel/11   11/21/06 3:02p gmullen
 * PR26073: Added BXPT_PcrOffset_RegenOffset_isr()
 * 
 * Hydra_Software_Devel/10   9/15/06 3:04p gmullen
 * PR15309: Fixed compiler warnings re unused variables.
 * 
 * Hydra_Software_Devel/9   8/18/06 10:13a gmullen
 * PR23345: Added MaxPcrError to Get/SetSettings calls.
 * 
 * Hydra_Software_Devel/8   8/8/06 10:58a gmullen
 * PR23345: Added MaxPcrError to BXPT_PcrOffset_Defaults.
 * 
 * Hydra_Software_Devel/7   4/24/06 3:44p gmullen
 * PR20631: Wrong header being included for 7401B0 builds
 * 
 * Hydra_Software_Devel/6   4/12/06 10:34a gmullen
 * PR20631: Added compilation checks for 7401 B0
 * 
 * Hydra_Software_Devel/5   3/31/06 12:16p gmullen
 * PR20565: PCR PID channel number needs a non-0xFF initial value
 * 
 * Hydra_Software_Devel/4   3/31/06 11:30a gmullen
 * PR20565: Added UseHostPcrs to settings struct.
 * 
 * Hydra_Software_Devel/3   2/10/06 12:06p katrep
 * PR19583: added isr flavours of various pcr offset routines, similar to
 * 7401, pcrlib should now compile okay
 * 
 * Hydra_Software_Devel/2   2/1/06 10:19a gmullen
 * PR18998: Fixed overflow issue in RAVE ITB/CDB, added support for PB
 * channels.
 * 
 * Hydra_Software_Devel/1   1/16/06 2:09p gmullen
 * PR18998: Initial checkin for 7400
 * 
 * Hydra_Software_Devel/14   1/9/06 1:45p gmullen
 * PR15309: Check for EnableRequested in SetSettings()
 * 
 * Hydra_Software_Devel/13   1/6/06 4:41p gmullen
 * PR18489: Changes to other modules to support fix for PR 18489.
 * 
 * Hydra_Software_Devel/12   11/14/05 9:11a gmullen
 * PR15309: OffsetThreshold may now be set through Defaults or Get/Set
 * methods.
 * 
 * Hydra_Software_Devel/11   11/11/05 5:59p gmullen
 * PR15309: Set XPT_PCROFFSET_OFFSET_THRESHOLD to 0.
 * 
 * Hydra_Software_Devel/10   10/28/05 4:39p gmullen
 * PR17859: Added BXPT_PcrOffset_GetOffset().
 * 
 * Hydra_Software_Devel/9   10/28/05 12:08p gmullen
 * PR15309: BXPT_PcrOffset_SetSettings() shouldn't configure SPID table
 * when PidChannelNum == BXPT_PCROFFSET_USE_HOST_PCRS .
 * 
 * Hydra_Software_Devel/8   9/26/05 3:51p gmullen
 * PR15309: Added BXPT_PcrOffset_SetStc() .
 * 
 * Hydra_Software_Devel/7   9/26/05 1:37p gmullen
 * PR15309: Moved CountMode from Defaults struct to Settings struct, to
 * reduce overhead during channel change,
 * 
 * Hydra_Software_Devel/6   9/21/05 2:17p gmullen
 * PR15309: Added support for AAC HE and AC3+ audio, fixed bug in playback
 * PI, modified RAVE PI to support channel change and reset.
 * 
 * Hydra_Software_Devel/5   9/2/05 4:58p gmullen
 * PR15309: Fixed bug in PCR Offset support.		  
 * 
 * Hydra_Software_Devel/4   8/31/05 10:58a gmullen
 * PR15309: Fixed bugs in PSI extraction and MPEG audio support.
 * 
 * Hydra_Software_Devel/3   8/12/05 8:57a gmullen
 * PR15309: Added PCR, PCR Offset, DirecTV, RAVE video, and ITB/CDB
 * endianess support. Tested same.
 * 
 * Hydra_Software_Devel/2   8/4/05 8:14a gmullen
 * PR15309: Added PCR support.
 * 
 * Hydra_Software_Devel/1   7/28/05 3:40p gmullen
 * PR15309: Initial version for building.
 * 
 *
 ***************************************************************************/

#include "bstd.h"
#include "bkni.h"
#include "bmem.h" 
#include "bxpt_priv.h"
#include "bxpt_pcr_offset.h"
#include "bchp_xpt_pcroffset0.h"

#if BXPT_P_MAX_PCR_OFFSET_CHANNELS > 1 
#include "bchp_xpt_pcroffset1.h"
#endif

#include "bchp_xpt_fe.h"

#include "bxpt.h"
#if BXPT_HAS_PACKETSUB
#include "bchp_xpt_psub.h"
#include "bxpt_packetsub.h"
#endif

#if( BCHP_CHIP == 7400 && BCHP_VER >= BCHP_VER_B0 ) || ( BCHP_CHIP == 7405 )
    #error "Build bxpt_pcr_offset_emv.c for this chip"
#endif

BDBG_MODULE( xpt_pcr_offset );


#define DEFAULT_STC_COUNT_MODE 		BXPT_PcrOffset_StcCountMode_eMod300
#define DEFAULT_USE_PCR_TIMEBASE	true
#define DEFAULT_TIME_BASE			0
#define DEFAULT_OFFSET_THRESHOLD	( 0x08 )
#define DEFAULT_MAX_PCR_ERROR       ( 0xFF )

/* XPT_PCROFFSET0_STC_TIMEBASE_SEL.TIMEBASE_SEL value for free-running STC */	
#define STC_FREE_RUNNING			( 4 )

#define STC_RATE_CTRL_OFFSET	        ( BCHP_XPT_PCROFFSET0_STC_RATE_CTRL - 	BCHP_XPT_PCROFFSET0_PP_PCR_PID_CH )
#define STC_CTRL_OFFSET			        ( BCHP_XPT_PCROFFSET0_STC_CTRL - 		BCHP_XPT_PCROFFSET0_PP_PCR_PID_CH )
#define PCR_PARSER_BAND_OFFSET	        ( BCHP_XPT_PCROFFSET0_PP_PARSER_BAND - 	BCHP_XPT_PCROFFSET0_PP_PCR_PID_CH )
#define STC_COUNTER_OFFSET 		        ( BCHP_XPT_PCROFFSET0_STC - 				BCHP_XPT_PCROFFSET0_PP_PCR_PID_CH )
#define TIMEBASE_SEL_OFFSET 	        ( BCHP_XPT_PCROFFSET0_STC_TIMEBASE_SEL - BCHP_XPT_PCROFFSET0_PP_PCR_PID_CH )
#define OFFSET_THRESHOLD_OFFSET	        ( BCHP_XPT_PCROFFSET0_OFFSET_THRESHOLD - BCHP_XPT_PCROFFSET0_PP_PCR_PID_CH )
#define PCR_OFFSET_OFFSET 		        ( BCHP_XPT_PCROFFSET0_OFFSET - 			BCHP_XPT_PCROFFSET0_PP_PCR_PID_CH )
#define SPLICE_CTRL_OFFSET 		        ( BCHP_XPT_PCROFFSET0_SPLICE_CTRL - 		BCHP_XPT_PCROFFSET0_PP_PCR_PID_CH )
#define SPLICE_STATUS_OFFSET 	        ( BCHP_XPT_PCROFFSET0_SPLICE_STATUS - 	BCHP_XPT_PCROFFSET0_PP_PCR_PID_CH )
#define SPLICE_RD_PTR_OFFSET 	        ( BCHP_XPT_PCROFFSET0_SPLICE_RD_PTR - 	BCHP_XPT_PCROFFSET0_PP_PCR_PID_CH )
#define SPLICE_WR_PTR_OFFSET 	        ( BCHP_XPT_PCROFFSET0_SPLICE_WR_PTR - 	BCHP_XPT_PCROFFSET0_PP_PCR_PID_CH )
#define SPLICE_QUEUE_OFFSET 	        ( BCHP_XPT_PCROFFSET0_SPLICE_PID_CH0 - 	BCHP_XPT_PCROFFSET0_PP_PCR_PID_CH )
#define OFFSET_MAX_ERROR_OFFSET         ( BCHP_XPT_PCROFFSET0_OFFSET_MAX_ERROR - BCHP_XPT_PCROFFSET0_PP_PCR_PID_CH )
#define OFFSET_CTRL_OFFSET              ( BCHP_XPT_PCROFFSET0_OFFSET_CTRL - BCHP_XPT_PCROFFSET0_PP_PCR_PID_CH )
#define PCR_OFFSET_INVALID_OFFSET       ( BCHP_XPT_PCROFFSET0_OFFSET_INVALID - BCHP_XPT_PCROFFSET0_PP_PCR_PID_CH )
#define STC_CAPTURE_PCR_ONCE_OFFSET     ( BCHP_XPT_PCROFFSET0_STC_CAPTURE_PCR_ONCE - BCHP_XPT_PCROFFSET0_PP_PCR_PID_CH )

#define PID_CHNL_STEPSIZE 		( 4 )                              

/* 
** Some parts may only instantiate 1 PCR Offset block. If BCHP_XPT_PCROFFSET1_PP_PCR_PID_CH is defined, we 
** have at least two blocks. 
*/
#ifdef BCHP_XPT_PCROFFSET1_PP_PCR_PID_CH
#define REGISTER_SPACE_STEP 	( BCHP_XPT_PCROFFSET1_PP_PCR_PID_CH - BCHP_XPT_PCROFFSET0_PP_PCR_PID_CH ) 
#else
#define REGISTER_SPACE_STEP 	( BCHP_XPT_PCROFFSET0_STC - BCHP_XPT_PCROFFSET0_PP_PCR_PID_CH ) 
#endif

BERR_Code BXPT_PcrOffset_GetTotalChannels(
	BXPT_Handle hXpt,           	/* [in] The transport handle */
	unsigned int *TotalChannels     /* [out] The number of Pcr offset channels. */
	)
{
	BERR_Code ExitCode = BERR_SUCCESS;

	BSTD_UNUSED( hXpt );                                              

	/* PCR Offset channels are mapped one-to-one to the RAVE channels. */
	*TotalChannels = BXPT_P_MAX_PCR_OFFSET_CHANNELS;

	return( ExitCode );
}
 
BERR_Code BXPT_PcrOffset_GetChannelDefSettings(
	BXPT_Handle hXpt,                     	/* [in] The transport handle - need chip info */
	unsigned int ChannelNo,					/* [in] Which pcr offset module */
	BXPT_PcrOffset_Defaults *Defaults	/* [out] The default settings of a pcr module */
	)
{
	BERR_Code ExitCode = BERR_SUCCESS;

    BDBG_OBJECT_ASSERT(hXpt, bxpt_t);
	BDBG_ASSERT( Defaults );

	if( ChannelNo >= BXPT_P_MAX_PCR_OFFSET_CHANNELS )
	{
		BDBG_ERR(( "ChannelNo %u out of range!", ChannelNo ));
		ExitCode = BERR_TRACE( BXPT_ERR_NO_AVAILABLE_RESOURCES );
		goto Done;
	}

	Defaults->UsePcrTimeBase = DEFAULT_USE_PCR_TIMEBASE;
	Defaults->WhichPcr = DEFAULT_TIME_BASE;
	Defaults->OffsetThreshold = DEFAULT_OFFSET_THRESHOLD;
    Defaults->MaxPcrError = DEFAULT_MAX_PCR_ERROR;

	Done:
	return( ExitCode );
}

BERR_Code BXPT_PcrOffset_Open(
	BXPT_Handle hXpt,				   /* [in] The transport handle*/
	unsigned int ChannelNo,			   /* [in] Which pcr offset module */
	BXPT_PcrOffset_Defaults *Defaults, /* [in] The default setting */
	BXPT_PcrOffset_Handle *hChannel	   /* [out] The channel handle */
	)
{
	BXPT_PcrOffset_Handle lhPcrOff;
	uint32_t Reg;

	BERR_Code ExitCode = BERR_SUCCESS;
	uint32_t TimeBase = STC_FREE_RUNNING;

    BDBG_OBJECT_ASSERT(hXpt, bxpt_t);
	BDBG_ASSERT( Defaults );
	BDBG_ASSERT( hChannel );

	if( ChannelNo >= BXPT_P_MAX_PCR_OFFSET_CHANNELS )
	{
		BDBG_ERR(( "ChannelNo %u out of range!", ChannelNo ));
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
		 
	lhPcrOff->BaseAddr = BCHP_XPT_PCROFFSET0_PP_PCR_PID_CH + ( ChannelNo * REGISTER_SPACE_STEP );
	lhPcrOff->hChip = hXpt->hChip;
	lhPcrOff->hReg = hXpt->hRegister;
	lhPcrOff->hMem = hXpt->hMemory;
	lhPcrOff->lvXpt = ( void * ) hXpt;
	lhPcrOff->ChannelNo = ChannelNo;
	lhPcrOff->PidChannelNum = 0;
    lhPcrOff->UseHostPcrs = false;

    BDBG_OBJECT_SET(lhPcrOff, bxpt_t);

	if( Defaults->UsePcrTimeBase )
		TimeBase = Defaults->WhichPcr;
	Reg = BREG_Read32( lhPcrOff->hReg, lhPcrOff->BaseAddr + TIMEBASE_SEL_OFFSET );
	Reg &= ~( 
   		BCHP_MASK( XPT_PCROFFSET0_STC_TIMEBASE_SEL, TIMEBASE_SEL ) 
	);
	Reg |= (
		BCHP_FIELD_DATA( XPT_PCROFFSET0_STC_TIMEBASE_SEL, TIMEBASE_SEL, TimeBase )
	);
	BREG_Write32( lhPcrOff->hReg, lhPcrOff->BaseAddr + TIMEBASE_SEL_OFFSET, Reg );
	lhPcrOff->CurrentTimeBase = TimeBase;

	Reg = BREG_Read32( lhPcrOff->hReg, lhPcrOff->BaseAddr + OFFSET_THRESHOLD_OFFSET );
	Reg &= ~( 
   		BCHP_MASK( XPT_PCROFFSET0_OFFSET_THRESHOLD, OFFSET_THRESHOLD ) 
	);
	Reg |= (
		BCHP_FIELD_DATA( XPT_PCROFFSET0_OFFSET_THRESHOLD, OFFSET_THRESHOLD, Defaults->OffsetThreshold )
	);
	BREG_Write32( lhPcrOff->hReg, lhPcrOff->BaseAddr + OFFSET_THRESHOLD_OFFSET, Reg );

	Reg = BREG_Read32( lhPcrOff->hReg, lhPcrOff->BaseAddr + OFFSET_MAX_ERROR_OFFSET );
	Reg &= ~( 
   		BCHP_MASK( XPT_PCROFFSET0_OFFSET_MAX_ERROR, MAX_ERROR ) 
	);
	Reg |= (
		BCHP_FIELD_DATA( XPT_PCROFFSET0_OFFSET_MAX_ERROR, MAX_ERROR, Defaults->MaxPcrError )
	);
	BREG_Write32( lhPcrOff->hReg, lhPcrOff->BaseAddr + OFFSET_MAX_ERROR_OFFSET, Reg );

	BXPT_PcrOffset_ClearQueue( lhPcrOff );

	Reg = BREG_Read32( lhPcrOff->hReg, lhPcrOff->BaseAddr );
	Reg &= ~( BCHP_MASK( XPT_PCROFFSET0_PP_PCR_PID_CH, CH_NUM ) );
	Reg |= ( BCHP_FIELD_DATA( XPT_PCROFFSET0_PP_PCR_PID_CH, CH_NUM, 0xFE ) );
	BREG_Write32( lhPcrOff->hReg, lhPcrOff->BaseAddr, Reg );

	hXpt->PcrOffsets[ ChannelNo ].Handle = lhPcrOff;
	hXpt->PcrOffsets[ ChannelNo ].Allocated = true;

	*hChannel = lhPcrOff;

	Done:
	return( ExitCode );
}

BERR_Code BXPT_PcrOffset_Close(
	BXPT_PcrOffset_Handle hChannel	 /* [in] The channel handle */
	)
{
	BXPT_Handle hXpt;

	BERR_Code ExitCode = BERR_SUCCESS;

    BDBG_OBJECT_ASSERT(hChannel, bxpt_t);

	hXpt = ( BXPT_Handle ) hChannel->lvXpt;
	hXpt->PcrOffsets[ hChannel->ChannelNo ].Allocated = false;

    BDBG_OBJECT_DESTROY(hChannel, bxpt_t);
	BKNI_Free( hChannel );

	return( ExitCode );
}

BERR_Code BXPT_PcrOffset_GetSettings(
	BXPT_PcrOffset_Handle hChannel,
	BXPT_PcrOffset_Settings *Settings
	)
{
	uint32_t Reg;
    uint32_t Timebase;

	BERR_Code ExitCode = BERR_SUCCESS;
    
    BDBG_OBJECT_ASSERT(hChannel, bxpt_t);

	Settings->PidChannelNum = hChannel->PidChannelNum;

#if 1
        Settings->UseHostPcrs = hChannel->UseHostPcrs;
#else
	Reg = BREG_Read32( hChannel->hReg, hChannel->BaseAddr );
	if( BCHP_GET_FIELD_DATA( Reg, XPT_PCROFFSET0_PP_PCR_PID_CH, CH_NUM ) == 0xFF )
		Settings->UseHostPcrs = true;
	else
		Settings->UseHostPcrs = false;
#endif

	Reg = BREG_Read32( hChannel->hReg, hChannel->BaseAddr + TIMEBASE_SEL_OFFSET );
    Timebase = BCHP_GET_FIELD_DATA( Reg, XPT_PCROFFSET0_STC_TIMEBASE_SEL, TIMEBASE_SEL );
	if( Timebase == BCHP_XPT_PCROFFSET0_STC_TIMEBASE_SEL_TIMEBASE_SEL_stc_free_run )
    {
        Settings->UsePcrTimeBase = false;
    }
    else
    {
        Settings->UsePcrTimeBase = true;
        Settings->WhichPcr = Timebase;
    }            

	Reg = BREG_Read32( hChannel->hReg, hChannel->BaseAddr + STC_CTRL_OFFSET );
	Settings->CountMode = BCHP_GET_FIELD_DATA( Reg, XPT_PCROFFSET0_STC_CTRL, MODE ) == 1 ? 
		BXPT_PcrOffset_StcCountMode_eBinary : BXPT_PcrOffset_StcCountMode_eMod300;

	Reg = BREG_Read32( hChannel->hReg, hChannel->BaseAddr + OFFSET_THRESHOLD_OFFSET );
	Settings->OffsetThreshold = BCHP_GET_FIELD_DATA( Reg, XPT_PCROFFSET0_OFFSET_THRESHOLD, OFFSET_THRESHOLD );

	Reg = BREG_Read32( hChannel->hReg, hChannel->BaseAddr + OFFSET_MAX_ERROR_OFFSET );
	Settings->MaxPcrError = BCHP_GET_FIELD_DATA( Reg, XPT_PCROFFSET0_OFFSET_MAX_ERROR, MAX_ERROR );

	Reg = BREG_Read32( hChannel->hReg, hChannel->BaseAddr + OFFSET_CTRL_OFFSET );
	Settings->TimestampDisable = BCHP_GET_FIELD_DATA( Reg, XPT_PCROFFSET0_OFFSET_CTRL, TIMESTAMP_DIS ) ? true : false;

	return( ExitCode );
}

BERR_Code BXPT_PcrOffset_SetSettings(
	BXPT_PcrOffset_Handle hChannel,
	const BXPT_PcrOffset_Settings *Settings
	)
{
	uint32_t Reg, BandNum, RegAddr;
	BXPT_Handle hXpt;

	uint32_t TimeBase = STC_FREE_RUNNING;
	BERR_Code ExitCode = BERR_SUCCESS;

#if BXPT_HAS_PACKETSUB && BXPT_PSUB_PID_TABLE_WORKAROUND
	BXPT_P_PacketSubCfg PsubCfg;
#endif

    BDBG_OBJECT_ASSERT(hChannel, bxpt_t);

	Reg = BREG_Read32( hChannel->hReg, hChannel->BaseAddr + STC_CTRL_OFFSET );
	Reg &= ~( 
   		BCHP_MASK( XPT_PCROFFSET0_STC_CTRL, MODE ) 
	);
	Reg |= (
		BCHP_FIELD_DATA( XPT_PCROFFSET0_STC_CTRL, MODE, Settings->CountMode )
	);
	BREG_Write32( hChannel->hReg, hChannel->BaseAddr + STC_CTRL_OFFSET, Reg );

	Reg = BREG_Read32( hChannel->hReg, hChannel->BaseAddr );
	Reg &= ~( 
   		BCHP_MASK( XPT_PCROFFSET0_PP_PCR_PID_CH, CH_NUM ) 
	);

	if( Settings->UsePcrTimeBase )
		TimeBase = Settings->WhichPcr;
	Reg = BREG_Read32( hChannel->hReg, hChannel->BaseAddr + TIMEBASE_SEL_OFFSET );
	Reg &= ~( 
   		BCHP_MASK( XPT_PCROFFSET0_STC_TIMEBASE_SEL, TIMEBASE_SEL ) 
	);
	Reg |= (
		BCHP_FIELD_DATA( XPT_PCROFFSET0_STC_TIMEBASE_SEL, TIMEBASE_SEL, TimeBase )
	);
	BREG_Write32( hChannel->hReg, hChannel->BaseAddr + TIMEBASE_SEL_OFFSET, Reg );
	hChannel->CurrentTimeBase = TimeBase;

	hChannel->PidChannelNum = Settings->PidChannelNum;
        hChannel->UseHostPcrs = Settings->UseHostPcrs;
	if( Settings->UseHostPcrs == true )
    {
		Reg |= ( BCHP_FIELD_DATA( XPT_PCROFFSET0_PP_PCR_PID_CH, CH_NUM, 0xFF ) );
        BXPT_PcrOffset_ForceInvalid( hChannel );
    }
	else
		Reg |= ( BCHP_FIELD_DATA( XPT_PCROFFSET0_PP_PCR_PID_CH, CH_NUM, Settings->PidChannelNum ) );

	BREG_Write32( hChannel->hReg, hChannel->BaseAddr, Reg );

	Reg = BREG_Read32( hChannel->hReg, hChannel->BaseAddr + OFFSET_MAX_ERROR_OFFSET );
	Reg &= ~( 
   		BCHP_MASK( XPT_PCROFFSET0_OFFSET_MAX_ERROR, MAX_ERROR ) 
	);
	Reg |= (
		BCHP_FIELD_DATA( XPT_PCROFFSET0_OFFSET_MAX_ERROR, MAX_ERROR, Settings->MaxPcrError )
	);
	BREG_Write32( hChannel->hReg, hChannel->BaseAddr + OFFSET_MAX_ERROR_OFFSET, Reg );

	Reg = BREG_Read32( hChannel->hReg, hChannel->BaseAddr + OFFSET_CTRL_OFFSET );
	Reg &= ~( 
   		BCHP_MASK( XPT_PCROFFSET0_OFFSET_CTRL, TIMESTAMP_DIS ) 
	);
	Reg |= (
		BCHP_FIELD_DATA( XPT_PCROFFSET0_OFFSET_CTRL, TIMESTAMP_DIS, Settings->TimestampDisable == true ? 1 : 0 )
	);
	BREG_Write32( hChannel->hReg, hChannel->BaseAddr + OFFSET_CTRL_OFFSET, Reg );

	/* Get the parser type and number from the PID channel we were given. */
   	RegAddr = BCHP_XPT_FE_PID_TABLE_i_ARRAY_BASE + ( Settings->PidChannelNum * PID_CHNL_STEPSIZE );

#if BXPT_HAS_PACKETSUB && BXPT_PSUB_PID_TABLE_WORKAROUND
	BXPT_PacketSub_P_SaveCfg( hXpt, Settings->PidChannelNum, &PsubCfg );
#endif
	Reg = BREG_Read32( hChannel->hReg, RegAddr );
#if BXPT_HAS_PACKETSUB && BXPT_PSUB_PID_TABLE_WORKAROUND
	BXPT_PacketSub_P_RestoreCfg( hXpt, Settings->PidChannelNum, &PsubCfg );
#endif

	hXpt = (BXPT_Handle) hChannel->lvXpt;

#if 0
	if( BCHP_GET_FIELD_DATA( Reg, XPT_FE_PID_TABLE_i, PID_CHANNEL_ENABLE ) 
	|| hXpt->PidChannelTable[ Settings->PidChannelNum ].EnableRequested == true )
#else
	if( 1 )
#endif
	{
		/* Playback and Input parser fields are actually the same bits in the PID table. */
		BandNum = BCHP_GET_FIELD_DATA( Reg, XPT_FE_PID_TABLE_i, INPUT_BAND_PARSER_PID_CHANNEL_INPUT_SELECT );
		if( BCHP_GET_FIELD_DATA( Reg, XPT_FE_PID_TABLE_i, PLAYBACK_FE_SEL ) )
			BandNum += 16;		/* Playback bands start at 16. */

		Reg = BREG_Read32( hChannel->hReg, hChannel->BaseAddr + PCR_PARSER_BAND_OFFSET );
		Reg &= ~( 
	   		BCHP_MASK( XPT_PCROFFSET0_PP_PARSER_BAND, BAND_NUM ) 
		);
		Reg |= (
			BCHP_FIELD_DATA( XPT_PCROFFSET0_PP_PARSER_BAND, BAND_NUM, BandNum )
		);
		BREG_Write32( hChannel->hReg, hChannel->BaseAddr + PCR_PARSER_BAND_OFFSET, Reg );

        /* 
        ** Need to enable data on this PID channel to go RAVE. PCR Offset ignores it otherwise. To
        ** resolve PR 28674, enable the R pipe. All data destined for the decoders will be on the R pipe.
        */
        if( Settings->UseHostPcrs == false )
        {
	    BXPT_P_SetPidChannelDestination( (BXPT_Handle) hChannel->lvXpt, Settings->PidChannelNum, 5, true );
        }

		Reg = BREG_Read32( hChannel->hReg, hChannel->BaseAddr + OFFSET_THRESHOLD_OFFSET );
		Reg &= ~( 
	   		BCHP_MASK( XPT_PCROFFSET0_OFFSET_THRESHOLD, OFFSET_THRESHOLD ) 
		);																			   
		Reg |= (
			BCHP_FIELD_DATA( XPT_PCROFFSET0_OFFSET_THRESHOLD, OFFSET_THRESHOLD, Settings->OffsetThreshold )
		);
		BREG_Write32( hChannel->hReg, hChannel->BaseAddr + OFFSET_THRESHOLD_OFFSET, Reg );
	}
	else
	{
		BDBG_ERR(( "PID channel not enabled!" ));
		ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
	}

	return( ExitCode );
}

BERR_Code BXPT_PcrOffset_ChangeStcRate(
	BXPT_PcrOffset_Handle hChannel,	 	/* [in] The channel handle */
	unsigned Increment,					/* [in] STC increment value */
	unsigned Prescale					/* [in] STC prescale value */
	)
{
	uint32_t Reg;

	BERR_Code ExitCode = BERR_SUCCESS;

    BDBG_OBJECT_ASSERT(hChannel, bxpt_t);

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

	Reg = BREG_Read32( hChannel->hReg, hChannel->BaseAddr + STC_RATE_CTRL_OFFSET );
	Reg &= ~( 
   		BCHP_MASK( XPT_PCROFFSET0_STC_RATE_CTRL, PRESCALE ) |
   		BCHP_MASK( XPT_PCROFFSET0_STC_RATE_CTRL, INCREMENT ) 
	);
	Reg |= (
		BCHP_FIELD_DATA( XPT_PCROFFSET0_STC_RATE_CTRL, PRESCALE, Prescale ) |
		BCHP_FIELD_DATA( XPT_PCROFFSET0_STC_RATE_CTRL, INCREMENT, Increment )
	);
	BREG_Write32( hChannel->hReg, hChannel->BaseAddr + STC_RATE_CTRL_OFFSET, Reg );

    Done:
	return( ExitCode );
}

BERR_Code BXPT_PcrOffset_FreezeStc(
	BXPT_PcrOffset_Handle hChannel,	 	/* [in] The channel handle */
	bool Freeze							/* [in] STC is frozen if true, unfrozen if false */
	)
{
	uint32_t Reg;

	BERR_Code ExitCode = BERR_SUCCESS;

    BDBG_OBJECT_ASSERT(hChannel, bxpt_t);

	Reg = BREG_Read32( hChannel->hReg, hChannel->BaseAddr + STC_CTRL_OFFSET );
	Reg &= ~( 
   		BCHP_MASK( XPT_PCROFFSET0_STC_CTRL, FREEZE ) 
	);
	Reg |= (
		BCHP_FIELD_DATA( XPT_PCROFFSET0_STC_CTRL, FREEZE, Freeze == true ? 1 : 0 )
	);
	BREG_Write32( hChannel->hReg, hChannel->BaseAddr + STC_CTRL_OFFSET, Reg );

	return( ExitCode );
}

uint32_t BXPT_PcrOffset_GetStc(
	BXPT_PcrOffset_Handle hChannel	 	/* [in] The channel handle */
	)
{
	uint32_t Reg;

    BDBG_OBJECT_ASSERT(hChannel, bxpt_t);

	Reg = BREG_Read32( hChannel->hReg, hChannel->BaseAddr + STC_COUNTER_OFFSET );
	return BCHP_GET_FIELD_DATA( Reg, XPT_PCROFFSET0_STC, COUNT );
}

uint32_t BXPT_PcrOffset_GetStc_isr(
	BXPT_PcrOffset_Handle hChannel	 	/* [in] The channel handle */
	)
{
	uint32_t Reg;

    BDBG_OBJECT_ASSERT(hChannel, bxpt_t);

	Reg = BREG_Read32( hChannel->hReg, hChannel->BaseAddr + STC_COUNTER_OFFSET );
	return BCHP_GET_FIELD_DATA( Reg, XPT_PCROFFSET0_STC, COUNT );
}



BERR_Code BXPT_PcrOffset_SetStc(
	BXPT_PcrOffset_Handle hChannel,	 	/* [in] The channel handle */
	uint32_t NewStc
	)
{
	uint32_t Reg;

	BERR_Code ExitCode = BERR_SUCCESS;

    BDBG_OBJECT_ASSERT(hChannel, bxpt_t);

	Reg = BREG_Read32( hChannel->hReg, hChannel->BaseAddr + STC_COUNTER_OFFSET );
	Reg &= ~( 
   		BCHP_MASK( XPT_PCROFFSET0_STC, COUNT ) 
	);
	Reg |= (
		BCHP_FIELD_DATA( XPT_PCROFFSET0_STC, COUNT, NewStc )
	);
	BREG_Write32( hChannel->hReg, hChannel->BaseAddr + STC_COUNTER_OFFSET, Reg );

	return( ExitCode );
}

BERR_Code BXPT_PcrOffset_SetStc_isr(
	BXPT_PcrOffset_Handle hChannel,	 	/* [in] The channel handle */
	uint32_t NewStc
	)
{
    BDBG_OBJECT_ASSERT(hChannel, bxpt_t);
    return BXPT_PcrOffset_SetStc(hChannel,NewStc);
}

uint32_t BXPT_PcrOffset_GetOffset(
	BXPT_PcrOffset_Handle hChannel	 		/* [in] The channel handle */
	)
{
	uint32_t Reg;

    BDBG_OBJECT_ASSERT(hChannel, bxpt_t);

	Reg = BREG_Read32( hChannel->hReg, hChannel->BaseAddr + PCR_OFFSET_OFFSET );

	return BCHP_GET_FIELD_DATA( Reg, XPT_PCROFFSET0_OFFSET, PCR_OFFSET );
}

uint32_t BXPT_PcrOffset_GetOffset_isr(
	BXPT_PcrOffset_Handle hChannel	 		/* [in] The channel handle */
	)
{
	uint32_t Reg;

    BDBG_OBJECT_ASSERT(hChannel, bxpt_t);

	Reg = BREG_Read32( hChannel->hReg, hChannel->BaseAddr + PCR_OFFSET_OFFSET );

	return BCHP_GET_FIELD_DATA( Reg, XPT_PCROFFSET0_OFFSET, PCR_OFFSET );
}

BERR_Code BXPT_PcrOffset_SetOffset(
	BXPT_PcrOffset_Handle hChannel,	 	/* [in] The channel handle */
	uint32_t UserPcrOffset					/* [in] User-supplied PCR Offset */
	)
{
	uint32_t Reg;

	BERR_Code ExitCode = BERR_SUCCESS;

    BDBG_OBJECT_ASSERT(hChannel, bxpt_t);

	Reg = BREG_Read32( hChannel->hReg, hChannel->BaseAddr + PCR_OFFSET_OFFSET );
	Reg &= ~( 
   		BCHP_MASK( XPT_PCROFFSET0_OFFSET, PCR_OFFSET ) 
	);
	Reg |= (
		BCHP_FIELD_DATA( XPT_PCROFFSET0_OFFSET, PCR_OFFSET, UserPcrOffset )
	);
	BREG_Write32( hChannel->hReg, hChannel->BaseAddr + PCR_OFFSET_OFFSET, Reg );

	return( ExitCode );
}

BERR_Code BXPT_PcrOffset_SetOffset_isr(
	BXPT_PcrOffset_Handle hChannel,	 	/* [in] The channel handle */
	uint32_t UserPcrOffset					/* [in] User-supplied PCR Offset */
	)
{
    BDBG_OBJECT_ASSERT(hChannel, bxpt_t);
	return BXPT_PcrOffset_SetOffset(hChannel,UserPcrOffset);
}

BERR_Code BXPT_PcrOffset_FreeRun(
	BXPT_PcrOffset_Handle hChannel,		/* [in] The channel handle */
	bool FreeRun						/* [in] Free run if true. */
	)
{
	uint32_t Reg;

	uint32_t NewTimeBase = STC_FREE_RUNNING;
	BERR_Code ExitCode = BERR_SUCCESS;

    BDBG_OBJECT_ASSERT(hChannel, bxpt_t);

	if( FreeRun == false )
		NewTimeBase	= hChannel->CurrentTimeBase;

	Reg = BREG_Read32( hChannel->hReg, hChannel->BaseAddr + TIMEBASE_SEL_OFFSET );
	Reg &= ~( 
   		BCHP_MASK( XPT_PCROFFSET0_STC_TIMEBASE_SEL, TIMEBASE_SEL ) 
	);
	Reg |= (
		BCHP_FIELD_DATA( XPT_PCROFFSET0_STC_TIMEBASE_SEL, TIMEBASE_SEL, NewTimeBase )
	);
	BREG_Write32( hChannel->hReg, hChannel->BaseAddr + TIMEBASE_SEL_OFFSET, Reg );

	return( ExitCode );
}

BERR_Code BXPT_PcrOffset_PushPidChannel(
	BXPT_PcrOffset_Handle hChannel,	 	/* [in] The channel handle */
	unsigned int PidChannel 		   	/* [in] Channel carrying the PID to splice. */
	)
{
	uint32_t RdPtr, WrPtr, Reg;
	BERR_Code ExitCode = BERR_SUCCESS;

    BDBG_OBJECT_ASSERT(hChannel, bxpt_t);

	if( BXPT_PcrOffset_GetQueueDepth( hChannel ) == MAX_PCR_SPLICING_ENTRIES )
	{
		BDBG_ERR(( "Splicing stack full!" ));
		ExitCode = BERR_TRACE( BXPT_ERR_NO_AVAILABLE_RESOURCES );
		goto Done;
	}

	Reg = BREG_Read32( hChannel->hReg, hChannel->BaseAddr + SPLICE_RD_PTR_OFFSET );
	RdPtr = BCHP_GET_FIELD_DATA( Reg, XPT_PCROFFSET0_SPLICE_RD_PTR, NEXT_PID_CH_RD_PTR );
			
	Reg = BREG_Read32( hChannel->hReg, hChannel->BaseAddr + SPLICE_WR_PTR_OFFSET );
	WrPtr = BCHP_GET_FIELD_DATA( Reg, XPT_PCROFFSET0_SPLICE_WR_PTR, NEXT_PID_CH_WR_PTR );		

	BREG_Write32( hChannel->hReg, hChannel->BaseAddr + SPLICE_QUEUE_OFFSET + ( WrPtr * 4 ), PidChannel );
	WrPtr = ( WrPtr + 1 ) % BXPT_P_SPLICING_QUEUE_SIZE;
	BREG_Write32( hChannel->hReg, hChannel->BaseAddr + SPLICE_WR_PTR_OFFSET, WrPtr );

	/* Set the enable, just in case. Should happen automatically when RD != WR */
	Reg = BREG_Read32( hChannel->hReg, hChannel->BaseAddr + SPLICE_CTRL_OFFSET );
	Reg &= ~( 
   		BCHP_MASK( XPT_PCROFFSET0_SPLICE_CTRL, ENABLE ) 
	);
	Reg |= (
		BCHP_FIELD_DATA( XPT_PCROFFSET0_SPLICE_CTRL, ENABLE, 1 )
	);
	BREG_Write32( hChannel->hReg, hChannel->BaseAddr + SPLICE_CTRL_OFFSET, Reg );
	
	Done:
	return( ExitCode );
}

BERR_Code BXPT_PcrOffset_ClearQueue(
	BXPT_PcrOffset_Handle hChannel	 	/* [in] The channel handle */
	)
{
	uint32_t Reg, Index;

	BERR_Code ExitCode = BERR_SUCCESS;

    BDBG_OBJECT_ASSERT(hChannel, bxpt_t);

	Reg = BREG_Read32( hChannel->hReg, hChannel->BaseAddr + SPLICE_CTRL_OFFSET );
	Reg &= ~( 
   		BCHP_MASK( XPT_PCROFFSET0_SPLICE_CTRL, ENABLE ) 
	);
	BREG_Write32( hChannel->hReg, hChannel->BaseAddr + SPLICE_CTRL_OFFSET, Reg );

	Reg = BREG_Read32( hChannel->hReg, hChannel->BaseAddr + SPLICE_RD_PTR_OFFSET );
	Reg &= ~( 
   		BCHP_MASK( XPT_PCROFFSET0_SPLICE_RD_PTR, NEXT_PID_CH_RD_PTR ) 
	);
	BREG_Write32( hChannel->hReg, hChannel->BaseAddr + SPLICE_RD_PTR_OFFSET, Reg );

	Reg = BREG_Read32( hChannel->hReg, hChannel->BaseAddr + SPLICE_WR_PTR_OFFSET );
	Reg &= ~( 
   		BCHP_MASK( XPT_PCROFFSET0_SPLICE_WR_PTR, NEXT_PID_CH_WR_PTR ) 
	);
	BREG_Write32( hChannel->hReg, hChannel->BaseAddr + SPLICE_WR_PTR_OFFSET, Reg );

	for( Index = 0; Index < BXPT_P_SPLICING_QUEUE_SIZE; Index++ )
		BREG_Write32( hChannel->hReg, hChannel->BaseAddr + SPLICE_QUEUE_OFFSET + ( 4 * Index ), 0 );

	return( ExitCode );
}

unsigned BXPT_PcrOffset_GetCountdown(
	BXPT_PcrOffset_Handle hChannel	 	/* [in] The channel handle */
	)
{
	uint32_t Reg;

    BDBG_OBJECT_ASSERT(hChannel, bxpt_t);

	Reg = BREG_Read32( hChannel->hReg, hChannel->BaseAddr + SPLICE_STATUS_OFFSET );
	return BCHP_GET_FIELD_DATA( Reg, XPT_PCROFFSET0_SPLICE_STATUS, COUNT );
}

unsigned BXPT_PcrOffset_GetQueueDepth(
	BXPT_PcrOffset_Handle hChannel	 	/* [in] The channel handle */
	)
{
	uint32_t RdPtr, WrPtr, Reg;

	unsigned Depth = 0;

    BDBG_OBJECT_ASSERT(hChannel, bxpt_t);

	Reg = BREG_Read32( hChannel->hReg, hChannel->BaseAddr + SPLICE_RD_PTR_OFFSET );
	RdPtr = BCHP_GET_FIELD_DATA( Reg, XPT_PCROFFSET0_SPLICE_RD_PTR, NEXT_PID_CH_RD_PTR );
			
	Reg = BREG_Read32( hChannel->hReg, hChannel->BaseAddr + SPLICE_WR_PTR_OFFSET );
	WrPtr = BCHP_GET_FIELD_DATA( Reg, XPT_PCROFFSET0_SPLICE_WR_PTR, NEXT_PID_CH_WR_PTR );		

	if( RdPtr < WrPtr )
		Depth = WrPtr - RdPtr;
	else if( WrPtr < RdPtr )
		Depth = BXPT_P_SPLICING_QUEUE_SIZE - RdPtr + WrPtr;

	return( Depth );
}

void BXPT_PcrOffset_ReloadPcrPidChannel_isr(
	BXPT_PcrOffset_Handle hPcrOff
	)
{
    BDBG_OBJECT_ASSERT(hPcrOff, bxpt_t);
   
    /* 
    ** Read the XPT_PCROFFSET0_PP_PCR_PID_CH reg, then immediately write it back. This should 
    ** force PCR offset generation.
    */
    BREG_Write32( hPcrOff->hReg, hPcrOff->BaseAddr, BREG_Read32( hPcrOff->hReg, hPcrOff->BaseAddr ) );
}

void BXPT_PcrOffset_RegenOffset_isr(
	BXPT_PcrOffset_Handle hPcrOff
	)
{
    uint32_t Addr;

    BDBG_OBJECT_ASSERT(hPcrOff, bxpt_t);
   
    Addr = hPcrOff->BaseAddr + PCR_OFFSET_OFFSET; 
    BREG_Write32( hPcrOff->hReg, Addr, BREG_Read32( hPcrOff->hReg, Addr ) );
}

bool BXPT_PcrOffset_IsOffsetValid(
    BXPT_PcrOffset_Handle hPcrOff
    )
{
    bool Status;

    BDBG_OBJECT_ASSERT(hPcrOff, bxpt_t);

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

    BDBG_OBJECT_ASSERT(hPcrOff, bxpt_t);

	Reg = BREG_Read32( hPcrOff->hReg, hPcrOff->BaseAddr + PCR_OFFSET_INVALID_OFFSET );
	Status = BCHP_GET_FIELD_DATA( Reg, XPT_PCROFFSET0_OFFSET_INVALID, PCR_OFFSET_INVALID ) == 1 ? false : true;

    return( Status );
}


void  BXPT_PcrOffset_ForceInvalid( BXPT_PcrOffset_Handle hPcrOff)
{
    BDBG_OBJECT_ASSERT(hPcrOff, bxpt_t);

	BKNI_EnterCriticalSection();
	BXPT_PcrOffset_ForceInvalid_isr( hPcrOff );
	BKNI_LeaveCriticalSection();
	return;
}

void  BXPT_PcrOffset_ForceInvalid_isr( BXPT_PcrOffset_Handle hPcrOff)
{
    uint32_t Reg;

    BDBG_OBJECT_ASSERT(hPcrOff, bxpt_t);

	Reg = BREG_Read32( hPcrOff->hReg, hPcrOff->BaseAddr + PCR_OFFSET_INVALID_OFFSET );
	Reg &= ( BCHP_FIELD_DATA( XPT_PCROFFSET0_OFFSET_INVALID, PCR_OFFSET_INVALID, 0x1 ) );
	BREG_Write32( hPcrOff->hReg, hPcrOff->BaseAddr + PCR_OFFSET_INVALID_OFFSET, Reg );

	return;
}

void BXPT_PcrOffset_CaptureStcOnce(
    BXPT_PcrOffset_Handle hPcrOff,      /* [in] Handle for the PCR Offset channel to use */
	bool Enable							/* [in] true to capture once, false to disable a pending capture */
    )
{
    BDBG_OBJECT_ASSERT(hPcrOff, bxpt_t);

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
	uint32_t Reg;

    BDBG_OBJECT_ASSERT(hPcrOff, bxpt_t);

	Reg = BREG_Read32( hPcrOff->hReg, hPcrOff->BaseAddr + STC_CAPTURE_PCR_ONCE_OFFSET );
	Reg &= ~( 
   		BCHP_MASK( XPT_PCROFFSET0_STC_CAPTURE_PCR_ONCE, ENABLE ) 
	);
	Reg |= (
		BCHP_FIELD_DATA( XPT_PCROFFSET0_STC_CAPTURE_PCR_ONCE, ENABLE, Enable == true ? 1 : 0 )
	);
	BREG_Write32( hPcrOff->hReg, hPcrOff->BaseAddr + STC_CAPTURE_PCR_ONCE_OFFSET, Reg );
}

bool BXPT_P_PcrOffset_IsPidChannelInUse(
    BXPT_Handle hXpt,               /* [in] The transport handle */
    uint32_t PidChannelNum              /* [in] Which PID channel to disable offsets for */
    )
{
    uint32_t ChannelNum;

    BDBG_OBJECT_ASSERT(hXpt, bxpt_t);

    for( ChannelNum = 0; ChannelNum < BXPT_P_MAX_PCR_OFFSET_CHANNELS; ChannelNum++ )
    {
	    uint32_t PidChannelReg = BREG_Read32( hXpt->hRegister, BCHP_XPT_PCROFFSET0_PP_PCR_PID_CH + ( ChannelNum * REGISTER_SPACE_STEP ) );

        /* The PID channel is mapped AND the offset context is enabled. */
	    if( BCHP_GET_FIELD_DATA( PidChannelReg, XPT_PCROFFSET0_PP_PCR_PID_CH, CH_NUM ) == PidChannelNum )
        {
            return true;
        }
    }

    return false;
}
