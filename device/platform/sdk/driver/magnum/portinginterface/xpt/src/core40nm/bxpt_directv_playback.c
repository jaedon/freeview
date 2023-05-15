/***************************************************************************
 *     Copyright (c) 2003-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxpt_directv_playback.c $
 * $brcm_Revision: Hydra_Software_Devel/3 $
 * $brcm_Date: 10/28/10 6:01p $
 *
 * Porting interface code for the data transport core. 
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/xpt/base2/bxpt_directv_playback.c $
 * 
 * Hydra_Software_Devel/3   10/28/10 6:01p gmullen
 * SW7425-15: Ported files
 * 
 * Hydra_Software_Devel/2   10/28/10 2:08p gmullen
 * SW7422-20: Checkin ported files
 * 
 * Hydra_Software_Devel/2   9/21/05 2:17p gmullen
 * PR15309: Added support for AAC HE and AC3+ audio, fixed bug in playback
 * PI, modified RAVE PI to support channel change and reset.
 * 
 * Hydra_Software_Devel/1   7/28/05 3:40p gmullen
 * PR15309: Initial version for building.
 * 
 *
 ***************************************************************************/

#include "bstd.h"
#include "bxpt_priv.h"
#include "bxpt_playback.h"
#include "bxpt_directv_playback.h"

#if( BDBG_DEBUG_BUILD == 1 )
BDBG_MODULE( xpt_directv_playback );
#endif

#include "bchp_xpt_pb0.h"

BERR_Code BXPT_DirecTvPlayback_SetParserBandMode( 
	BXPT_Playback_Handle hPb,		/* [in] Handle for the playback channel */
	BXPT_ParserMode Mode					/* [in] Which mode (packet format) is being used. */
	)
{
	uint32_t Reg;

	BERR_Code ExitCode = BERR_SUCCESS;

	BDBG_ASSERT( hPb );

	Reg = BXPT_Playback_P_ReadReg( hPb, BCHP_XPT_PB0_PARSER_CTRL1 );
	Reg &= ~( 
		BCHP_MASK( XPT_PB0_PARSER_CTRL1, PARSER_PACKET_TYPE ) |
		BCHP_MASK( XPT_PB0_PARSER_CTRL1, PARSER_PKT_LENGTH )
		);
	if( Mode == BXPT_ParserMode_eMpeg )
	{
		Reg |= (
			BCHP_FIELD_DATA( XPT_PB0_PARSER_CTRL1, PARSER_PACKET_TYPE, 0 ) |
			BCHP_FIELD_DATA( XPT_PB0_PARSER_CTRL1, PARSER_PKT_LENGTH, 188 )
		);
	}
	else 
	{
		Reg |= (
			BCHP_FIELD_DATA( XPT_PB0_PARSER_CTRL1, PARSER_PACKET_TYPE, 1 ) |
			BCHP_FIELD_DATA( XPT_PB0_PARSER_CTRL1, PARSER_PKT_LENGTH, 130 )
		);
	}
	BXPT_Playback_P_WriteReg( hPb, BCHP_XPT_PB0_PARSER_CTRL1, Reg ); 

	return( ExitCode );
}

BERR_Code BXPT_DirecTvPlayback_GetSyncThresholds( 
	BXPT_Playback_Handle PlaybackHandle,	/* [in] Handle for the playback channel */
	unsigned int *SyncInCount,			/* [out] In-sync threshold. */
	unsigned int *SyncOutCount			/* [out] Out-of-sync threshold. */
	)
{
	uint32_t Reg;

	BERR_Code ExitCode = BERR_SUCCESS;

	BDBG_ASSERT( PlaybackHandle );

	/* Get the sync in/out counts. */
	Reg = BXPT_Playback_P_ReadReg( PlaybackHandle, BCHP_XPT_PB0_CTRL3 );
	*SyncInCount = ( uint8_t ) ( BCHP_GET_FIELD_DATA( Reg, XPT_PB0_CTRL3, DIRECTV_SYNC_IN_CNT ) & 0xFF );
	*SyncOutCount = ( uint8_t ) ( BCHP_GET_FIELD_DATA( Reg, XPT_PB0_CTRL3, DIRECTV_SYNC_OUT_CNT ) & 0xFF );
	
	return( ExitCode );
}


BERR_Code BXPT_DirecTvPlayback_SetSyncThresholds( 
	BXPT_Playback_Handle PlaybackHandle,	/* [in] Handle for the playback channel */
	unsigned int SyncInCount,			/* [in] In-sync threshold. */
	unsigned int SyncOutCount			/* [in] Out-of-sync threshold. */
	)
{
	uint32_t Reg;

	BERR_Code ExitCode = BERR_SUCCESS;

	Reg = BXPT_Playback_P_ReadReg( PlaybackHandle, BCHP_XPT_PB0_CTRL3 );

	Reg &= ~( 
		BCHP_MASK( XPT_PB0_CTRL3, DIRECTV_SYNC_IN_CNT ) |
		BCHP_MASK( XPT_PB0_CTRL3, DIRECTV_SYNC_OUT_CNT )
		);
	Reg |= ( 
		BCHP_FIELD_DATA( XPT_PB0_CTRL3, DIRECTV_SYNC_IN_CNT, SyncInCount ) |
		BCHP_FIELD_DATA( XPT_PB0_CTRL3, DIRECTV_SYNC_OUT_CNT, SyncOutCount ) 
		);
	
	BXPT_Playback_P_WriteReg( PlaybackHandle, BCHP_XPT_PB0_CTRL3, Reg ); 

	return( ExitCode );
}


