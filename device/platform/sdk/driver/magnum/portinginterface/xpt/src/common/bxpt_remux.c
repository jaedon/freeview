/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxpt_remux.c $
 * $brcm_Revision: Hydra_Software_Devel/5 $
 * $brcm_Date: 9/7/12 10:45a $
 *
 * Porting interface code for the data transport core. 
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/xpt/src/core40nm/bxpt_remux.c $
 * 
 * Hydra_Software_Devel/5   9/7/12 10:45a gmullen
 * SW7346-1024: Added missing input band clock support
 * 
 * Hydra_Software_Devel/4   2/13/12 4:00p gmullen
 * SW7231-658: Fixed array overrun
 * 
 * Hydra_Software_Devel/3   5/12/11 4:59p gmullen
 * SW7231-128: Merged to mainline
 * 
 * Hydra_Software_Devel/SW7231-128/1   5/10/11 1:40p gmohile
 * SW7231-128 : Add power management support
 * 
 * Hydra_Software_Devel/2   10/28/10 2:08p gmullen
 * SW7422-20: Checkin ported files
 * 
 * Hydra_Software_Devel/38   8/30/10 5:24p gmullen
 * SW7403-924: Protected access to the PID and SPID table from other
 * modules in the PI
 * 
 * Hydra_Software_Devel/37   7/29/10 1:59p gmullen
 * SW7420-904: Added RMX_NULL_PKT_THRESHOLD
 * 
 * Hydra_Software_Devel/36   7/27/10 6:13p gmullen
 * SW7420-903: Corrected code to match hw default
 * 
 * Hydra_Software_Devel/35   4/23/10 8:54a gmullen
 * SW3556-1102: Added param to control NULL packet insertion
 * 
 * Hydra_Software_Devel/34   2/18/10 9:35a gmullen
 * SW3548-2790: Added API for controlling PCR jitter adjust
 * 
 * Hydra_Software_Devel/33   9/9/09 8:07a piyushg
 * SW7630-30: Add 7630 XPT PI support.
 * Added directory element "7630".
 * 
 * Hydra_Software_Devel/32   8/11/09 10:40a piyushg
 * PR55216: Added initial 7340 XPT support.
 * 
 * Hydra_Software_Devel/31   8/5/09 4:52p piyushg
 * PR55545: Add 7125 XPT PI support
 * Added file element "bxpt_rave_ihex.c".
 * Added file element "bxpt_rave_ihex.h".
 * 
 * Hydra_Software_Devel/PR55545/1   7/24/09 10:50a mward
 * PR55548: Adapted for 7125 based on 7342, 7340.
 * 
 * Hydra_Software_Devel/30   7/14/09 10:22a piyushg
 * PR56771: Add XPT PI code for 7342.
 * 
 * Hydra_Software_Devel/29   6/18/09 8:07a gmullen
 * PR56110: Fixed build errors for other chips.
 * 
 * Hydra_Software_Devel/28   6/17/09 6:46p gmullen
 * PR56110: Added support.xpt/7550/uif_image/a0
 * 
 * Hydra_Software_Devel/27   4/16/09 7:20p piyushg
 * PR53728: Each of the XC buffer clients (Remux, RAVE, and the Mesg
 * filters) would get a separate API to set
 * their input bitrates. That API would take a client handle and the ID of
 * the parser band. The XPT PI would
 * then walk backwards through the pipeline, increasing only the bitrates
 * for the XC and RS buffers that it
 * needs to.
 * 
 * Hydra_Software_Devel/26   4/14/09 2:50p piyushg
 * PR52851: Fix coverity defect. Break statement missing.
 * Also fix PR52843.
 * 
 * Hydra_Software_Devel/26   4/14/09 2:47p piyushg
 * PR52851: Fix coverity defect. Break statement missing.
 * Also fix PR52843.
 * 
 * Hydra_Software_Devel/25   4/7/09 5:25p piyushg
 * PR52986: Add support for 7635
 * Added directory element "7635".
 * 
 * Hydra_Software_Devel/24   3/26/09 6:11p gmullen
 * PR53579: Fixed BCHP mismatches between C and H files.
 * 
 * Hydra_Software_Devel/23   3/6/09 3:18p piyushg
 * PR52841: Fixed Coverity defect. Also fix PR52842, 52843,
 * 52850, 52851.
 * 
 * Hydra_Software_Devel/22   1/29/09 10:47a gmullen
 * PR50362: Added workaround for hardware bug (PR 51706).
 * 
 * Hydra_Software_Devel/21   1/27/09 1:10p gmullen
 * PR51625: Added 7336 support
 * 
 * Hydra_Software_Devel/20   1/27/09 10:43a gmullen
 * PR51620: Changed AddPidChannel and PacketSubControl to static.
 * 
 * Hydra_Software_Devel/19   1/12/09 4:43p gmullen
 * PR50362: Fixed addressing bug. Some parts have the remux IO registers
 * outside of the remux block.
 * 
 * Hydra_Software_Devel/18   11/26/08 4:15p gmullen
 * PR47755: Added support for 7420.
 * 
 * Hydra_Software_Devel/17   11/4/08 4:43p gmullen
 * PR48062: Removed deadcode.
 * 
 * Hydra_Software_Devel/16   10/20/08 11:11a gmullen
 * PR48062: Fixed coverity deadcode warning (I hope...)
 * 
 * Hydra_Software_Devel/15   6/30/08 10:13a gmullen
 * PR44320: Enabled missing features in remux PI
 * 
 * Hydra_Software_Devel/14   5/5/08 3:07p gmullen
 * PR42437: 7325, 3556, and 3548 only have 2 playback channels.
 * 
 * Hydra_Software_Devel/13   3/26/08 11:24a gmullen
 * PR38954: Added 3548 support to XPT PI.
 * 
 * Hydra_Software_Devel/12   2/22/08 11:09a gmullen
 * PR39874: Disabled XC buffer pause before disabling remux.
 * 
 * Hydra_Software_Devel/11   11/28/07 12:12p gmullen
 * PR36900: Added 7335 support
 * 
 * Hydra_Software_Devel/10   10/30/07 3:53p gmullen
 * PR35018: Removed unsupported struct members from
 * BXPT_Remux_GetChannelDefaultSettings
 * 
 * Hydra_Software_Devel/9   10/30/07 3:12p gmullen
 * PR35018: Added support for 7325
 * 
 * Hydra_Software_Devel/8   11/17/06 5:34p gmullen
 * PR26034: Fixed BXPT_Remux_AllocPidMap().
 * 
 * Hydra_Software_Devel/7   11/2/06 8:40a gmullen
 * PR23189: Added 3563 support.
 * 
 * Hydra_Software_Devel/6   7/17/06 5:19p gmullen
 * PR20624: Added code for 7118.
 * 
 * Hydra_Software_Devel/5   6/2/06 4:23p gmullen
 * PR21868: Added bitfields to BXPT_Remux_ChannelSettings struct.
 * 
 * Hydra_Software_Devel/4   4/13/06 4:02p gmullen
 * PR20914: Ported over Prashant's fix from 7401A0
 * 
 * Hydra_Software_Devel/3   3/28/06 9:06a gmullen
 * PR18998: Added SetAllPassMode
 * 
 * Hydra_Software_Devel/2   3/16/06 5:10p gmullen
 * PR18998: Added support for reading data from R-pipe (security related).
 * 
 * Hydra_Software_Devel/1   1/16/06 2:09p gmullen
 * PR18998: Initial checkin for 7400
 * 
 * Hydra_Software_Devel/4   1/6/06 4:41p gmullen
 * PR18489: Changes to other modules to support fix for PR 18489.
 * 
 * Hydra_Software_Devel/3   12/2/05 3:24p gmullen
 * PR15309: Saved updates for ccase server move
 * 
 * Hydra_Software_Devel/2   11/8/05 10:22a gmullen
 * PR15309: Updated.
 * 
 * Hydra_Software_Devel/1   7/28/05 3:40p gmullen
 * PR15309: Initial version for building.
 * 
 *
 ***************************************************************************/

#include "bstd.h"
#include "bxpt_priv.h"
#include "bxpt_remux.h"
#include "bxpt_remux_priv.h"
#include "bchp_xpt_fe.h"

#if BCHP_PWR_SUPPORT
#include "bchp_pwr.h"
#endif

#include "bchp_xpt_rmx0.h"

#include "bxpt.h"
#include "bchp_xpt_rmx0_io.h"

#if BXPT_NUM_REMULTIPLEXORS > 1
	#include "bchp_xpt_rmx1_io.h"
    #include "bchp_xpt_rmx1.h"
#endif

#if( BDBG_DEBUG_BUILD == 1 )
BDBG_MODULE( xpt_remux );
#endif

#define BXPT_P_REMUX_DEFAULT_OUTPUT_CLOCK   	BXPT_RemuxClock_e81Mhz 
#define BXPT_P_REMUX_DEFAULT_HOLD_LEVEL			BXPT_RemuxHoldLevel_eNever
#define BXPT_P_REMUX_DEFAULT_TIMEBASE			BXPT_RemuxTimeBase_e27Mhz
#define BXPT_P_REMUX_DEFAULT_PACKET_DELAY		( 0 )
#define BXPT_P_REMUX_DEFAULT_USE_PCR_TIMEBASE 	false
#define BXPT_P_REMUX_DEFAULT_WHICH_PCR 			( 0 )
#define BXPT_P_REMUX_PCR_BAND_A_DEFAULT			( 0x1F )
#define BXPT_P_REMUX_PCR_BAND_B_DEFAULT			( 0x1F )
#define BXPT_P_REMUX_PARALLEL_EN_DEFAULT		false
#define BXPT_P_REMUX_PARALLEL_INV_CLK_DEFAULT	false
#define BXPT_P_REMUX_PARALLEL_INV_SYNC_DEFAULT	false
#define BXPT_P_REMUX_INV_CLK_DEFAULT			false
#define BXPT_P_REMUX_INV_SYNC_DEFAULT			false
#define BXPT_P_REMUX_BYTE_SYNC_DEFAULT			false

#define SPID_CHNL_STEPSIZE	( 4 )

static BERR_Code AddPidChannel(
	BXPT_Remux_Handle hRmx,				
	unsigned int PidChannelNum,
	bool UseRPipe 			
	);

BERR_Code BXPT_Remux_GetTotalChannels(
	BXPT_Handle hXpt, 	   		/* [in] Handle for this transport */
	unsigned int *TotalChannels		/* [out] The number of remux channels. */
	)
{
	BERR_Code ExitCode = BERR_SUCCESS;

	BDBG_ASSERT( hXpt );
	BSTD_UNUSED( hXpt ); 
	
	*TotalChannels = BXPT_NUM_REMULTIPLEXORS;

	return( ExitCode );
}

BERR_Code BXPT_Remux_GetChannelDefaultSettings(
	BXPT_Handle hXpt, 	   		/* [in] Handle for this transport */
	unsigned int ChannelNo,			/* [in] Which channel to get defaults from. */
	BXPT_Remux_ChannelSettings *ChannelSettings /* [out] The defaults */
	)
{
	BERR_Code ExitCode = BERR_SUCCESS;

	BDBG_ASSERT( hXpt );
	BDBG_ASSERT( ChannelSettings );
	BSTD_UNUSED( hXpt ); 

	if( ChannelNo > BXPT_NUM_REMULTIPLEXORS )
	{
		/* Bad PID channel number. Complain. */
		BDBG_ERR(( "ChannelNo %lu is out of range!", ( unsigned long ) ChannelNo ));
		ExitCode = BERR_INVALID_PARAMETER;
	}
	else
	{
		ChannelSettings->OutputClock = BXPT_P_REMUX_DEFAULT_OUTPUT_CLOCK;
		ChannelSettings->PacketDelayCount = BXPT_P_REMUX_DEFAULT_PACKET_DELAY;
		ChannelSettings->ParallelEn = BXPT_P_REMUX_PARALLEL_EN_DEFAULT;			
		ChannelSettings->ParallelInvertClk = BXPT_P_REMUX_PARALLEL_INV_CLK_DEFAULT;		
		ChannelSettings->ParallelInvertSync = BXPT_P_REMUX_PARALLEL_INV_SYNC_DEFAULT;	
		ChannelSettings->InvertClk = BXPT_P_REMUX_INV_CLK_DEFAULT;		
		ChannelSettings->InvertSync = BXPT_P_REMUX_INV_SYNC_DEFAULT;	
		ChannelSettings->ByteSync = BXPT_P_REMUX_BYTE_SYNC_DEFAULT;	
        ChannelSettings->PauseEn = false;
		ChannelSettings->InsertNullPackets = true;
		ChannelSettings->NullPacketInsertionThreshold = 16;
	}

	return( ExitCode );
}

BERR_Code BXPT_Remux_OpenChannel(
	BXPT_Handle hXpt, 	   					/* [in] Handle for this transport */
	BXPT_Remux_Handle *hRmx,			/* [out] Handle for opened remux channel */
	unsigned int ChannelNo,						/* [in] Which channel to open. */
	BXPT_Remux_ChannelSettings *ChannelSettings /* [in] The defaults to use */
	)
{
	BERR_Code ExitCode = BERR_SUCCESS;
	BXPT_Remux_Handle lhRmx = NULL;

	BDBG_ASSERT( hXpt );
	BDBG_ASSERT( ChannelSettings );							 

	if( ChannelNo > BXPT_NUM_REMULTIPLEXORS )
	{
		/* Bad PID channel number. Complain. */
		BDBG_ERR(( "ChannelNo %lu is out of range!", ( unsigned long ) ChannelNo ));
		ExitCode = BERR_INVALID_PARAMETER;
	}
    else if (ChannelSettings->OutputClock >= BXPT_RemuxClock_eIb0 
    && (ChannelSettings->OutputClock - BXPT_RemuxClock_eIb0) >= BXPT_NUM_INPUT_BANDS ) 
    {
		BDBG_ERR(( "OutputClock %lu is out of range!", ( unsigned long ) ChannelSettings->OutputClock ));
		ExitCode = BERR_INVALID_PARAMETER;
	}
	else
	{
		uint32_t Reg;
		uint32_t BaseAddr = 0;

		/* 
		** Use the address of the first register in the remux block as the 
		** base address of the entire block. 
		*/
		switch( ChannelNo )
		{
			case 0: 
			BaseAddr = BCHP_XPT_RMX0_CTRL; 
			break;

#if BXPT_NUM_REMULTIPLEXORS > 1
			case 1: 
			    BaseAddr = BCHP_XPT_RMX1_CTRL; 
			    break;
#endif			    
			default:
			    BDBG_ERR(( "Internal error" ));
			    goto Done;
		}
		
		lhRmx = &hXpt->RemuxHandles[ ChannelNo ];
		lhRmx->vhXpt = ( void * ) hXpt;
		lhRmx->hChip = hXpt->hChip;
		lhRmx->hRegister = hXpt->hRegister;
		lhRmx->hMemory = hXpt->hMemory;
		lhRmx->BaseAddr = BaseAddr;
		lhRmx->ChannelNo = ChannelNo;
		lhRmx->Running = false;

#ifdef BCHP_PWR_RESOURCE_XPT_REMUX
	BCHP_PWR_AcquireResource(hXpt->hChip, BCHP_PWR_RESOURCE_XPT_REMUX);
#endif

		/* Use MPEG defaults. The BXPT_DirecTv_Remux APIs can change that. */
		Reg = BXPT_Remux_P_ReadReg( lhRmx, BCHP_XPT_RMX0_CTRL );
		Reg &= ~(
			BCHP_MASK( XPT_RMX0_CTRL, RMX_PKT_LENGTH ) |
			BCHP_MASK( XPT_RMX0_CTRL, RMX_PKT_MODE ) 
		);
		Reg |= (
			BCHP_FIELD_DATA( XPT_RMX0_CTRL, RMX_PKT_LENGTH, 188 ) |
			BCHP_FIELD_DATA( XPT_RMX0_CTRL, RMX_PKT_MODE, 0 ) 
		);
		BXPT_Remux_P_WriteReg( lhRmx, BCHP_XPT_RMX0_CTRL, Reg );

		Reg = BXPT_Remux_P_ReadReg( lhRmx, BCHP_XPT_RMX0_IO_FORMAT );
		Reg &= ~(
			BCHP_MASK( XPT_RMX0_IO_FORMAT, RMXP_INVERT_CLK ) |
			BCHP_MASK( XPT_RMX0_IO_FORMAT, RMXP_INVERT_SYNC ) |
			BCHP_MASK( XPT_RMX0_IO_FORMAT, RMXP_ENABLE ) |
			BCHP_MASK( XPT_RMX0_IO_FORMAT, RMX_INVERT_SYNC ) |
			BCHP_MASK( XPT_RMX0_IO_FORMAT, RMX_INVERT_CLK ) |
			BCHP_MASK( XPT_RMX0_IO_FORMAT, RMX_BYTE_SYNC ) |
			BCHP_MASK( XPT_RMX0_IO_FORMAT, RMX_MUTE )
		);
		Reg |= (
			BCHP_FIELD_DATA( XPT_RMX0_IO_FORMAT, RMXP_INVERT_CLK, ChannelSettings->ParallelInvertClk ) |
			BCHP_FIELD_DATA( XPT_RMX0_IO_FORMAT, RMXP_INVERT_SYNC, ChannelSettings->ParallelInvertSync ) |
			BCHP_FIELD_DATA( XPT_RMX0_IO_FORMAT, RMXP_ENABLE, ChannelSettings->ParallelEn ) |
			BCHP_FIELD_DATA( XPT_RMX0_IO_FORMAT, RMX_INVERT_SYNC, ChannelSettings->InvertSync ) |
			BCHP_FIELD_DATA( XPT_RMX0_IO_FORMAT, RMX_INVERT_CLK, ChannelSettings->InvertClk ) |
			BCHP_FIELD_DATA( XPT_RMX0_IO_FORMAT, RMX_BYTE_SYNC, ChannelSettings->ByteSync ) |
			BCHP_FIELD_DATA( XPT_RMX0_IO_FORMAT, RMX_MUTE, 0 )
		);
		BXPT_Remux_P_WriteReg( lhRmx, BCHP_XPT_RMX0_IO_FORMAT, Reg );
	
		Reg = BXPT_Remux_P_ReadReg( lhRmx, BCHP_XPT_RMX0_IO_PKT_DLY_CNT );
		Reg &= ~(
			BCHP_MASK( XPT_RMX0_IO_PKT_DLY_CNT, RMX_PKT_DLY_CNT )
		);
		Reg |= (
			BCHP_FIELD_DATA( XPT_RMX0_IO_PKT_DLY_CNT, RMX_PKT_DLY_CNT, ChannelSettings->PacketDelayCount )
		);
		BXPT_Remux_P_WriteReg( lhRmx, BCHP_XPT_RMX0_IO_PKT_DLY_CNT, Reg );

		/* Load the defaults. */
		Reg = BXPT_Remux_P_ReadReg( lhRmx, BCHP_XPT_RMX0_CTRL );
		Reg &= ~(
#ifdef BCHP_XPT_RMX0_CTRL_RMX_CLK_SEL_MSB_MASK
			BCHP_MASK( XPT_RMX0_CTRL, RMX_CLK_SEL_MSB ) |
#endif
			BCHP_MASK( XPT_RMX0_CTRL, RMX_CLK_SEL ) |
			BCHP_MASK( XPT_RMX0_CTRL, RMX_NULL_PKT_THRESHOLD ) |
			BCHP_MASK( XPT_RMX0_CTRL, RMX_NULL_PKT_DIS )
		);
		Reg |= (
#ifdef BCHP_XPT_RMX0_CTRL_RMX_CLK_SEL_MSB_MASK
			BCHP_FIELD_DATA( XPT_RMX0_CTRL, RMX_CLK_SEL_MSB, ChannelSettings->OutputClock >> 4 ) |
#endif
			BCHP_FIELD_DATA( XPT_RMX0_CTRL, RMX_CLK_SEL, ChannelSettings->OutputClock & 0xF ) |
			BCHP_FIELD_DATA( XPT_RMX0_CTRL, RMX_NULL_PKT_THRESHOLD, ChannelSettings->NullPacketInsertionThreshold ) |
			BCHP_FIELD_DATA( XPT_RMX0_CTRL, RMX_NULL_PKT_DIS, ChannelSettings->InsertNullPackets ? false : true )
		);
		BXPT_Remux_P_WriteReg( lhRmx, BCHP_XPT_RMX0_CTRL, Reg );

#ifdef BCHP_PWR_RESOURCE_XPT_REMUX
		BCHP_PWR_ReleaseResource(hXpt->hChip, BCHP_PWR_RESOURCE_XPT_REMUX);
#endif

		lhRmx->Opened = true;
 		*hRmx = lhRmx;
	}

	Done:
	return( ExitCode );
}

void BXPT_Remux_CloseChannel(
	BXPT_Remux_Handle hRmx	/* [in] Handle for the channel to close*/
	)
{
	uint32_t Reg;

	BDBG_ASSERT( hRmx );

	/* Stop remuxing, stop the remapping logic, and packet subbing. */
	Reg = BXPT_Remux_P_ReadReg( hRmx, BCHP_XPT_RMX0_CTRL );
	Reg &= ~(
		BCHP_MASK( XPT_RMX0_CTRL, RMX_BYPASS ) |
		BCHP_MASK( XPT_RMX0_CTRL, RMX_ENABLE )
	);
	BXPT_Remux_P_WriteReg( hRmx, BCHP_XPT_RMX0_CTRL, Reg );
	hRmx->Opened = false;
}

BERR_Code BXPT_Remux_AddPidChannelToRemux( 
	BXPT_Remux_Handle hRmx,				/* [in] Handle for the remux channel */
	BXPT_RemuxInput RemuxInput, 		/* [in] Which remux input */
	unsigned int PidChannelNum 			/* [in] Which PID channel. */
	)
{
	BDBG_ASSERT( hRmx );
	BSTD_UNUSED( RemuxInput );

	return AddPidChannel( hRmx, PidChannelNum, false ); 
}

BERR_Code BXPT_Remux_AddRPipePidChannelToRemux( 
	BXPT_Remux_Handle hRmx,				/* [in] Handle for the remux channel */
	BXPT_RemuxInput RemuxInput, 		/* [in] Which remux input */
	unsigned int PidChannelNum 			/* [in] Which PID channel. */
	)
{
	BDBG_ASSERT( hRmx );
	BSTD_UNUSED( RemuxInput );

	return AddPidChannel( hRmx, PidChannelNum, true ); 
}

BERR_Code AddPidChannel(
	BXPT_Remux_Handle hRmx,				
	unsigned int PidChannelNum,
	bool UseRPipe 			
	)
{
	BERR_Code ExitCode = BERR_SUCCESS;

	if( PidChannelNum >= BXPT_NUM_PID_CHANNELS )
	{
		BDBG_ERR(( "PidChannelNum %lu is out of range!", ( unsigned long ) PidChannelNum ));
		ExitCode = BERR_INVALID_PARAMETER;
	}
	else
	{
		uint32_t PipeShift;

		/* Set the PID channels enable bit. */
		switch( hRmx->ChannelNo )
		{
			case 0:
			PipeShift = 0;		/* G pipe */
			break;

			case 1:
			PipeShift = 2;		/* G pipe */
			break;

			default:
			BDBG_ERR(( "ChannelNo %lu invalid!", ( unsigned long ) hRmx->ChannelNo ));
			ExitCode = BERR_INVALID_PARAMETER;
			goto Done;
		}

		if( UseRPipe == true )
			PipeShift++;

		BXPT_P_SetPidChannelDestination( (BXPT_Handle) hRmx->vhXpt, PidChannelNum, PipeShift, true );
	}

	Done:
	return( ExitCode );	    
}

BERR_Code BXPT_Remux_DoRemux( 
	BXPT_Remux_Handle hRmx,	/* [in] Handle for the remux channel */
	bool Enable 				/* [in] Remux on if TRUE, or off if FALSE. */
	)
{
	uint32_t Reg;
	BXPT_Handle hXpt;

	BERR_Code ExitCode = BERR_SUCCESS;
#ifdef BCHP_PWR_RESOURCE_XPT_REMUX
	unsigned wasEnabled;
#endif
	BDBG_ASSERT( hRmx );

#ifdef BCHP_PWR_RESOURCE_XPT_REMUX
	Reg = BXPT_Remux_P_ReadReg(hRmx, BCHP_XPT_RMX0_CTRL);
	wasEnabled = BCHP_GET_FIELD_DATA(Reg, XPT_RMX0_CTRL, RMX_ENABLE);
	/* only change refcnt if changing state */
	if (!wasEnabled && Enable) {
	    BCHP_PWR_AcquireResource(hRmx->hChip, BCHP_PWR_RESOURCE_XPT_REMUX);
	}
#endif

	hXpt = (BXPT_Handle) hRmx->vhXpt;
	Reg = BXPT_Remux_P_ReadReg( hRmx, BCHP_XPT_RMX0_CTRL );
	Reg &= ~( BCHP_MASK( XPT_RMX0_CTRL, RMX_ENABLE ) );
	
	if( Enable )
	{
	    Reg |= BCHP_FIELD_DATA( XPT_RMX0_CTRL, RMX_ENABLE, 1 );
	    BXPT_Remux_P_WriteReg( hRmx, BCHP_XPT_RMX0_CTRL, Reg );
	}
	else
	{
	    BXPT_Remux_P_WriteReg( hRmx, BCHP_XPT_RMX0_CTRL, Reg );
	}    
	
#ifdef BCHP_PWR_RESOURCE_XPT_REMUX
	if (wasEnabled && !Enable) {
	    BCHP_PWR_ReleaseResource(hRmx->hChip, BCHP_PWR_RESOURCE_XPT_REMUX);
	}	
#endif
	
	hRmx->Running = Enable;

	return( ExitCode );
}

BERR_Code BXPT_Remux_RemovePidChannelFromRemux( 
	BXPT_Remux_Handle hRmx,			/* [in] Handle for the remux channel */
	BXPT_RemuxInput RemuxInput, 	/* [in] Which remux input */
	unsigned int PidChannelNum 			/* [in] Which PID channel. */
	)
{
	BERR_Code ExitCode = BERR_SUCCESS;

	BDBG_ASSERT( hRmx );
	BSTD_UNUSED( RemuxInput );

	if( PidChannelNum >= BXPT_NUM_PID_CHANNELS )
	{
		BDBG_ERR(( "PidChannelNum %lu is out of range!", ( unsigned long ) PidChannelNum ));
		ExitCode = BERR_INVALID_PARAMETER;
	}
	else
	{
		uint32_t PipeShift;

		/* Clear the PID channels enable bit. */
		switch( hRmx->ChannelNo )
		{
			case 0:
			PipeShift = 0;		/* G pipe */
			break;

			case 1:
			PipeShift = 2;		/* G pipe */
			break;

			default:
			BDBG_ERR(( "ChannelNo %lu invalid!", ( unsigned long ) hRmx->ChannelNo ));
			ExitCode = BERR_INVALID_PARAMETER;
			goto Done;
		}

		BXPT_P_SetPidChannelDestination( (BXPT_Handle) hRmx->vhXpt, PidChannelNum, PipeShift, false );
	}

	Done:
	return( ExitCode );	    
}

BERR_Code BXPT_Remux_GetPidChannel( 
	BXPT_Remux_Handle hRmx,			/* [in] Handle for the remux channel */
	BXPT_RemuxInput RemuxInput, 			/* [in] Which remux input */
	unsigned int PidChannelNum, 			/* [in] Which PID channel. */
	bool *Enable 						/* [out] Where to put channel status. */
	)
{
	BERR_Code ExitCode = BERR_SUCCESS;

	BDBG_ASSERT( hRmx );
	BSTD_UNUSED( RemuxInput );

	*Enable = false;

	if( PidChannelNum > BXPT_NUM_PID_CHANNELS )
	{
		BDBG_ERR(( "PidChannelNum %lu is out of range!", ( unsigned long ) PidChannelNum ));
		ExitCode = BERR_INVALID_PARAMETER;
		*Enable = false;
	}
	else
	{
		uint32_t Reg, RegAddr, PipeShift;

		/* Set the PID channels enable bit. */
		RegAddr = BCHP_XPT_FE_SPID_TABLE_i_ARRAY_BASE + ( PidChannelNum * SPID_CHNL_STEPSIZE );
		Reg = BREG_Read32( hRmx->hRegister, RegAddr );
		
		switch( hRmx->ChannelNo )
		{
			case 0:
			PipeShift = 24;		/* G pipe */
			break;

			case 1:
			PipeShift = 24 + 2;		/* G pipe */
			break;

			default:
			BDBG_ERR(( "ChannelNo %lu invalid!", ( unsigned long ) hRmx->ChannelNo ));
			ExitCode = BERR_INVALID_PARAMETER;
			goto Done;
		}

		if( Reg & ( 3ul << PipeShift ) )
			*Enable = true;
		else
			*Enable = false;
	}

	Done:
	return( ExitCode );	    
}

BERR_Code BXPT_Remux_RemoveAllPidChannel( 
	BXPT_Remux_Handle hRmx,			/* [in] Handle for the remux channel */
	BXPT_RemuxInput RemuxInput 			/* [in] Which remux input */
	)
{
	unsigned PidChannelNum;
 	uint32_t PipeShift;

	BERR_Code ExitCode = BERR_SUCCESS;

	BDBG_ASSERT( hRmx );
	BSTD_UNUSED( RemuxInput );

	/* Clear all the PID channels' enable bit. */
	for( PidChannelNum = 0; PidChannelNum < BXPT_NUM_PID_CHANNELS; PidChannelNum++ )
	{
		switch( hRmx->ChannelNo )
		{
			case 0:
			PipeShift = 0;		/* G pipe */
			break;

			case 1:
			PipeShift = 2;		/* G pipe */
			break;

			default:
			BDBG_ERR(( "ChannelNo %lu invalid!", ( unsigned long ) hRmx->ChannelNo ));
			ExitCode = BERR_INVALID_PARAMETER;
			goto Done;
		}

		BXPT_P_SetPidChannelDestination( (BXPT_Handle) hRmx->vhXpt, PidChannelNum, PipeShift, false );
	}

	Done:
	return( ExitCode );
}

BERR_Code BXPT_Remux_AddPcrOffset( 
	BXPT_Remux_Handle hRmx,		/* [in] Handle for the remux channel */
    bool AddOffsetEn,		    /* [in] Enable/disable the PCR correction */
    uint32_t FixedPcrOffset			/* [in] The PCR correction offset, if enabled */		
	)
{
	uint32_t Reg;

	BERR_Code ret = BERR_SUCCESS;

	if( AddOffsetEn )
 	{
		Reg = BXPT_Remux_P_ReadReg( hRmx, BCHP_XPT_RMX0_FIXED_OFFSET );
		Reg &= ~BCHP_MASK( XPT_RMX0_FIXED_OFFSET, RMX_FIXED_OFFSET );
		Reg |= BCHP_FIELD_DATA( XPT_RMX0_FIXED_OFFSET, RMX_FIXED_OFFSET, FixedPcrOffset );
   		BXPT_Remux_P_WriteReg( hRmx, BCHP_XPT_RMX0_FIXED_OFFSET, Reg );
	}

	Reg = BXPT_Remux_P_ReadReg( hRmx, BCHP_XPT_RMX0_PCR_CTRL );
	Reg &= ~BCHP_MASK( XPT_RMX0_PCR_CTRL, RMX_FIXED_OFFSET_EN );
	Reg |= BCHP_FIELD_DATA( XPT_RMX0_PCR_CTRL, RMX_FIXED_OFFSET_EN, AddOffsetEn == true ? 1 : 0 );	
	BXPT_Remux_P_WriteReg( hRmx, BCHP_XPT_RMX0_PCR_CTRL, Reg );

	return ret;
}

void BXPT_Remux_SetPcrJitterAdj( 
	BXPT_Remux_Handle hRmx,		/* [in] Handle for the remux channel */
	bool ForPlayback,		/* [in] true = control adjustment for playback, false = control for live */
    	bool EnableAdjust		    /* [in] Enable/disable the PCR jitter adjust */
	)
{
	uint32_t Reg;

	Reg = BXPT_Remux_P_ReadReg( hRmx, BCHP_XPT_RMX0_PCR_CTRL );
	if( ForPlayback )
	{
		/* HW uses reverse-logic */
		Reg &= ~BCHP_MASK( XPT_RMX0_PCR_CTRL, RMX_PLAYBACK_PCR_JITTER_DIS );
		Reg |= BCHP_FIELD_DATA( XPT_RMX0_PCR_CTRL, RMX_PLAYBACK_PCR_JITTER_DIS, EnableAdjust == true ? 0 : 1 );	
	}
	else
	{
		/* HW uses reverse-logic */
		Reg &= ~BCHP_MASK( XPT_RMX0_PCR_CTRL, RMX_LIVE_PCR_JITTER_DIS );
		Reg |= BCHP_FIELD_DATA( XPT_RMX0_PCR_CTRL, RMX_LIVE_PCR_JITTER_DIS, EnableAdjust == true ? 0 : 1 );	
	}
	BXPT_Remux_P_WriteReg( hRmx, BCHP_XPT_RMX0_PCR_CTRL, Reg );
}


BERR_Code BXPT_Remux_SetBypassMode( 
	BXPT_Remux_Handle hRmx,			/* [in] Handle for the remux channel */
	bool Enable
	)
{
	BERR_Code ExitCode = BERR_SUCCESS;
	uint32_t Reg = BXPT_Remux_P_ReadReg( hRmx, BCHP_XPT_RMX0_CTRL );

	if( Enable )
		Reg |= ( BCHP_FIELD_DATA( XPT_RMX0_CTRL, RMX_BYPASS, 1 ) );
	else
		Reg &= ~( BCHP_MASK( XPT_RMX0_CTRL, RMX_BYPASS ) );
	BXPT_Remux_P_WriteReg( hRmx, BCHP_XPT_RMX0_CTRL, Reg );

	return( ExitCode );	    
}

#if defined(HUMAX_PLATFORM_BASE) 
BERR_Code BXPT_GetAllPassStartPIDChannel(uint32_t *startPidChannel)
{
	BERR_Code status = BERR_SUCCESS;

	if(startPidChannel == NULL)
		return BERR_TRACE( BERR_INVALID_PARAMETER );


	*startPidChannel = BXPT_PB_ALLPASS_PID_CHANNEL_START;

	return status;
}
#endif


