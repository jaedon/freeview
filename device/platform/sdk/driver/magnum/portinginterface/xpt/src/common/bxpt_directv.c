/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxpt_directv.c $
 * $brcm_Revision: Hydra_Software_Devel/7 $
 * $brcm_Date: 8/21/12 10:26a $
 * $brcm_Revision: Hydra_Software_Devel/7 $
 * $brcm_Date: 8/21/12 10:26a $
 *
 * Porting interface code for the DirecTV section of the data transport core. 
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/xpt/src/core40nm/bxpt_directv.c $
 * 
 * Hydra_Software_Devel/7   8/21/12 10:26a gmullen
 * SW7425-3785: Don't automatically enable PID channels during mesg
 * handling
 * 
 * Hydra_Software_Devel/6   7/13/11 1:37p gmullen
 * SW7346-317: Merged fix to Hydra branch
 * 
 * Hydra_Software_Devel/SW7346-317/1   7/13/11 11:47a gmullen
 * SW7346-317: Added message buffer counting
 * 
 * Hydra_Software_Devel/5   4/15/11 4:49p gmullen
 * SW7425-313: Merged to mainline
 * 
 * Hydra_Software_Devel/SW7425-313/1   4/13/11 2:05p gmullen
 * SW7425-313: Added parser remapping support
 * 
 * Hydra_Software_Devel/4   2/24/11 2:34p rjlewis
 * SW7425-89: DSS needs to use binary mode for timestamp generation.
 * 
 * Hydra_Software_Devel/3   11/9/10 2:57p gmullen
 * SW7422-20: Fixed register overwrite in BXPT_Open(). Changed RS and XC
 * defines; buffer init was not called
 * 
 * Hydra_Software_Devel/2   10/27/10 11:59a gmullen
 * SW7422-20: Ported to 7422
 * 
 * Hydra_Software_Devel/22   8/30/10 5:24p gmullen
 * SW7403-924: Protected access to the PID and SPID table from other
 * modules in the PI
 * 
 * Hydra_Software_Devel/21   7/7/10 10:07a gmullen
 * SW7401-4402: Merged to Hydra_Software_Devel
 * 
 * Hydra_Software_Devel/20   2/8/10 3:03p gmullen
 * SW7408-64: Added missing API.
 * 
 * Hydra_Software_Devel/19   11/23/09 5:14p gmullen
 * SW7468-13: Updated PI and unit test
 * 
 * Hydra_Software_Devel/18   10/29/09 11:06a piyushg
 * SW7400-2559: Add PID2BUFF mapping api's for DirecTv message
 * capture start and stop.
 * 
 * Hydra_Software_Devel/17   10/19/09 5:15p piyushg
 * SW7400-2559: Add PID2BUFF mapping api's for DirecTv message
 * capture start and stop.
 * 
 * Hydra_Software_Devel/16   9/23/09 2:26p gmullen
 * SW7405-2994: Merged fix to Hydra_Software_Devel
 * 
 * Hydra_Software_Devel/SW7405-2994/1   9/8/09 6:56p gmullen
 * SW7405-2994: Stored MPEG mode setting for CONT_COUNT_IGNORE in module
 * handle
 * 
 * Hydra_Software_Devel/15   8/19/09 12:15p piyushg
 * PR56771: Add support for 7342. Hence the previous usage
 * of constant PARSER_REG_STEPSIZE does not work for this chip.
 * Added new function to calculate offset.
 * 
 * Hydra_Software_Devel/14   7/16/08 3:43p gmullen
 * PR37867: Merged playback mux code to Hydra_Software_Devel
 * 
 * Hydra_Software_Devel/13   5/7/08 10:47a gmullen
 * PR42443: Removed call to change band rates when changing parser mode.
 * 
 * Hydra_Software_Devel/12   7/24/07 11:59a gmullen
 * PR33366: Do not write identical config to a PID channel register.
 * 
 * Hydra_Software_Devel/11   12/5/06 3:00p gmullen
 * PR26325: Added support for CAP filtering.
 * 
 * Hydra_Software_Devel/10   12/5/06 2:04p gmullen
 * PR26278: Allow user to configure byte alignment of data in message
 * buffer.
 * 
 * Hydra_Software_Devel/9   8/21/06 1:26p gmullen
 * PR21524: Set FILTER_MODE to Generic AND Special Filter .
 * 
 * Hydra_Software_Devel/8   7/27/06 10:58a gmullen
 * PR15309: BXPT_ConfigurePidChannel() now checks for duplicated PID
 * channels
 * 
 * Hydra_Software_Devel/7   7/20/06 5:30p katrep
 * PR22365: Added better mangement of XC buffer client BW for playback.XC
 * buf for PB is given max BW & BW throtle is done at PB engine.Spare BW
 * enabllag is used to provide more BW for PB if available.
 * 
 * Hydra_Software_Devel/6   7/18/06 10:20a gmullen
 * PR15309: Set GEN_OFFSET to 3 for DirecTV regular message.
 * 
 * Hydra_Software_Devel/5   4/25/06 5:47p gmullen
 * PR18998: Set GEN_OFFSET to 4 for MPT frames.
 * 
 * Hydra_Software_Devel/4   4/24/06 5:48p gmullen
 * PR18998: Set GEN_OFFSET to 3 for AUX messages, 0 for all others.
 * 
 * Hydra_Software_Devel/3   4/13/06 4:41p gmullen
 * PR18998: Incorrect register address used in StartScidChannelRecord.
 * 
 * Hydra_Software_Devel/2   4/13/06 4:01p gmullen
 * PR20914: Ported over Prashant's fix from 7401A0
 * 
 * Hydra_Software_Devel/8   1/6/06 4:41p gmullen
 * PR18489: Changes to other modules to support fix for PR 18489.
 * 
 * Hydra_Software_Devel/7   1/4/06 9:57a gmullen
 * PR15309: Filter mode is now Generic OR Special Filter when capturing
 * DirecTV messages.
 * 
 * Hydra_Software_Devel/6   11/23/05 9:18a gmullen
 * PR15309: More updates.
 * 
 * Hydra_Software_Devel/5   9/21/05 2:17p gmullen
 * PR15309: Added support for AAC HE and AC3+ audio, fixed bug in playback
 * PI, modified RAVE PI to support channel change and reset.
 * 
 * Hydra_Software_Devel/4   8/26/05 2:01p gmullen
 * PR15309: Added DirecTV support, RAVE video support, cleaned up RS and
 * XC buffer code.
 * 
 * Hydra_Software_Devel/3   8/18/05 9:58a gmullen
 * PR15309: Added more DirecTV support.
 * 
 * Hydra_Software_Devel/2   8/12/05 8:57a gmullen
 * PR15309: Added PCR, PCR Offset, DirecTV, RAVE video, and ITB/CDB
 * endianess support. Tested same.
 * 
 * Hydra_Software_Devel/1   7/25/05 5:11p gmullen
 * PR15309: Fixed bugs in numerous files
 * 
 *
 ***************************************************************************/

#include "bstd.h"
#include "bxpt.h"
#include "bxpt_directv.h"
#include "bdbg.h"
#include "bkni.h"
#include "bxpt_priv.h"

#include "bchp_xpt_fe.h"

#if BXPT_HAS_MESG_BUFFERS
#include "bchp_xpt_msg.h"
#endif

#if BXPT_HAS_PACKETSUB
#include "bchp_xpt_psub.h"
#include "bxpt_packetsub.h"
#endif

#define MSG_BUF_CTRL1_STEPSIZE	( 4 )
#define PID_CHANNEL_STEPSIZE	( 4 )
#define CAP_FILT_REG_STEPSIZE	( 4 )
#define SPID_CHNL_STEPSIZE		( 4 )
#define DEFAULT_PEAK_RATE  		( 25000000 )

BDBG_MODULE( xpt_directv );

#if BXPT_HAS_IB_PID_PARSERS

BERR_Code BXPT_DirecTv_SetParserBandMode( 
	BXPT_Handle hXpt, 	   	/* [Input] Handle for this transport */
	unsigned int Band, 			/* [Input] Which parser band */
	BXPT_ParserMode Mode	/* [Input] Which mode (packet format) is being used. */
	)
{
	uint32_t Reg, RegAddr;

	BERR_Code ExitCode = BERR_SUCCESS;

	BDBG_ASSERT( hXpt );

	/* Sanity check on the arguments. */
	if( Band >= hXpt->MaxPidParsers )
	{
		/* Bad PID channel number. Complain. */
		BDBG_ERR(( "Band %lu is out of range!", ( unsigned long ) Band ));
		ExitCode = BERR_TRACE(BERR_INVALID_PARAMETER);
	}
	else
	{
		/* The parser config registers are at consecutive addresses. */
        RegAddr = BXPT_P_GetParserCtrlRegAddr( hXpt, Band, BCHP_XPT_FE_MINI_PID_PARSER0_CTRL1 );
		Reg = BREG_Read32( hXpt->hRegister, RegAddr );

		Reg &= ~( 
			BCHP_MASK( XPT_FE_MINI_PID_PARSER0_CTRL1, PARSER_PACKET_TYPE ) |
			BCHP_MASK( XPT_FE_MINI_PID_PARSER0_CTRL1, PARSER_TIMESTAMP_MODE ) |
			BCHP_MASK( XPT_FE_MINI_PID_PARSER0_CTRL1, PARSER_PKT_LENGTH )
			);

		if( Mode == BXPT_ParserMode_eMpeg )
		{
			Reg |= (
				BCHP_FIELD_DATA( XPT_FE_MINI_PID_PARSER0_CTRL1, PARSER_PACKET_TYPE, 0 ) |
				BCHP_FIELD_DATA( XPT_FE_MINI_PID_PARSER0_CTRL1, PARSER_TIMESTAMP_MODE, 2 ) |
				BCHP_FIELD_DATA( XPT_FE_MINI_PID_PARSER0_CTRL1, PARSER_PKT_LENGTH, 188 )
			);
			BREG_Write32( hXpt->hRegister, RegAddr, Reg );
		}
		else if ( Mode == BXPT_ParserMode_eDirecTv )
		{
			Reg |= (
				BCHP_FIELD_DATA( XPT_FE_MINI_PID_PARSER0_CTRL1, PARSER_PACKET_TYPE, 1 ) |
				BCHP_FIELD_DATA( XPT_FE_MINI_PID_PARSER0_CTRL1, PARSER_TIMESTAMP_MODE, 3 ) |
				BCHP_FIELD_DATA( XPT_FE_MINI_PID_PARSER0_CTRL1, PARSER_PKT_LENGTH, 130 )
			);
			BREG_Write32( hXpt->hRegister, RegAddr, Reg );
		}
		else
		{
			/* Unsupported parser mode. Complain. */
			BDBG_ERR(( "Unsupported parser mode %d!", ( unsigned long ) Mode ));
			ExitCode = BERR_TRACE(BERR_INVALID_PARAMETER);
		}
	}

	return( ExitCode );
}

#endif

#if BXPT_HAS_MESG_BUFFERS

void BXPT_DirecTv_SaveMptFlag( 
	BXPT_Handle hXpt, 	   	/* [in] Handle for this transport */
	bool Enable 			/* [in] Enable or disable flag saving. */
	)
{
	uint32_t Reg, RegAddr;
	unsigned Index;

	BDBG_ASSERT( hXpt );

	for( Index = 0; Index < BXPT_NUM_PID_CHANNELS; Index++ )
	{
		RegAddr = BCHP_XPT_MSG_BUF_CTRL1_TABLE_i_ARRAY_BASE + ( Index * MSG_BUF_CTRL1_STEPSIZE );
		Reg = BREG_Read32( hXpt->hRegister, RegAddr );
		Reg &= ~( BCHP_MASK( XPT_MSG_BUF_CTRL1_TABLE_i, DIRECTV_SAVE_FLAGS ) );
		Reg |= ( BCHP_FIELD_DATA( XPT_MSG_BUF_CTRL1_TABLE_i, DIRECTV_SAVE_FLAGS, Enable == true ? 1 : 0 ) );
		BREG_Write32( hXpt->hRegister, RegAddr, Reg );
	}
}

void BXPT_DirecTv_SetPesStreamIdBoundaries( 
	BXPT_Handle hXpt, 	   	/* [in] Handle for this transport */
	unsigned int UpperId,		/* [in] The upper stream id. */
	unsigned int LowerId			/* [in] The lower stream id. */
	)
{
	uint32_t Reg;

	BDBG_ASSERT( hXpt );

	Reg = BREG_Read32( hXpt->hRegister, BCHP_XPT_MSG_PES_CTRL1 );

	Reg &= ~( 
		BCHP_MASK( XPT_MSG_PES_CTRL1, PES_STREAM_ID_HI ) |
		BCHP_MASK( XPT_MSG_PES_CTRL1, PES_STREAM_ID_LO ) 
		);
	
	Reg |= ( 
		BCHP_FIELD_DATA( XPT_MSG_PES_CTRL1, PES_STREAM_ID_HI, UpperId ) |
		BCHP_FIELD_DATA( XPT_MSG_PES_CTRL1, PES_STREAM_ID_LO, LowerId ) 
		);
	
	BREG_Write32( hXpt->hRegister, BCHP_XPT_MSG_PES_CTRL1, Reg );
}


void BXPT_DirecTv_SetStartcodeChecking( 
	BXPT_Handle hXpt, 	   	/* [in] Handle for this transport */
	bool EnableChecking		/* [in] Enable checking, or not. */
	)
{
	uint32_t Reg;

	BDBG_ASSERT( hXpt );

	Reg = BREG_Read32( hXpt->hRegister, BCHP_XPT_MSG_PES_CTRL1 );

	Reg &= ~( 
		BCHP_MASK( XPT_MSG_PES_CTRL1, PSC_CHECK_MODE ) 
		);
	
	Reg |= ( 
		BCHP_FIELD_DATA( XPT_MSG_PES_CTRL1, PSC_CHECK_MODE, EnableChecking == true ? 1 : 0  ) 
		);
	
	BREG_Write32( hXpt->hRegister, BCHP_XPT_MSG_PES_CTRL1, Reg );
}


BERR_Code BXPT_Mesg_StartDirecTvMessageCapture( 
	BXPT_Handle hXpt, 							/* [in] Handle for this transport */
	unsigned int PidChannelNum, 				/* [in] Which PID channel. */
	unsigned int MesgBufferNum,                 /* [in] Which Message Buffer. */
	BXPT_DirecTvMessageType MessageType,		/* [in] What type of DirecTV messages. */
	const BXPT_PsiMessageSettings *Settings 	/* [in] PID, band, and filters to use. */
	)
{
    BXPT_DirecTvMessageOptions Options;

    Options.Flags = BXPT_DirecTvMessageFlags_eSaveFirst;
    return BXPT_Mesg_StartDirecTvMessageCaptureWithOptions( hXpt, PidChannelNum, MesgBufferNum, MessageType, Settings, &Options );
}

extern void ConfigPid2BufferMap( BXPT_Handle hXpt, unsigned int PidChannelNum, unsigned int BufferNumber, bool enableIt);

BERR_Code BXPT_Mesg_StartDirecTvMessageCaptureWithOptions( 
	BXPT_Handle hXpt, 							/* [in] Handle for this transport */
	unsigned int PidChannelNum, 				/* [in] Which PID channel. */
	unsigned int MesgBufferNum,                 /* [in] Which Message Buffer. */
	BXPT_DirecTvMessageType MessageType,		/* [in] What type of DirecTV messages. */
	const BXPT_PsiMessageSettings *Settings, 	/* [in] PID, band, and filters to use. */
    const BXPT_DirecTvMessageOptions *Options   /* [in] Additional options for message capture */
	)
{
	BERR_Code ExitCode = BERR_SUCCESS;

	BDBG_ASSERT( hXpt );
	BDBG_ASSERT( Settings );
	BDBG_ASSERT( Options );

	/* Sanity check on the arguments. */
	if( PidChannelNum > hXpt->MaxPidChannels )	   
	{
		/* Bad parser band number. Complain. */
		BDBG_ERR(( "PidChannelNum %lu is out of range!", ( unsigned long ) PidChannelNum ));
		ExitCode = BERR_TRACE(BERR_INVALID_PARAMETER);
	}
	else if( hXpt->MesgBufferIsInitialized[ MesgBufferNum ] == false )
    {
        BDBG_ERR(( "Message buffer for this channel not configured!" ));
        ExitCode = BERR_TRACE( BXPT_ERR_MESG_BUFFER_NOT_CONFIGURED );
    }
	else if ( MesgBufferNum > BXPT_NUM_MESG_BUFFERS ) {
        /* Bad Message Buffer number. Complain. */
        BDBG_ERR(( "Message Buffer Number %lu is out of range!", ( unsigned long ) MesgBufferNum ));
        ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
	}
	else if ( hXpt->Pid2BuffMappingOn == true )
	{
		uint32_t Reg, RegAddr;
		unsigned int GroupSel, OutputMode, GenOffset, SpecialSel, SpecialType;

		ExitCode = BXPT_P_GetGroupSelect( Settings->Bank, &GroupSel );
		if( ExitCode != BERR_SUCCESS )
			goto Done;

		/* Enable the filters for this PID channel. */
		BREG_Write32( hXpt->hRegister, BCHP_XPT_MSG_GEN_FILT_EN_i_ARRAY_BASE + ( MesgBufferNum * GEN_FILT_EN_STEP ), Settings->FilterEnableMask );		

		/* Configure the message buffer for capturing messages. */
		switch( MessageType )
		{
			case BXPT_DirecTvMessageType_eMpt:
			GenOffset = 4;
			OutputMode = 0x0A; 
            SpecialSel = 0;
            SpecialType = 0;
			break;

			case BXPT_DirecTvMessageType_eAuxOnlyPackets: 
			GenOffset = 3;
			OutputMode = 0x08; 
            SpecialSel = 0;
            SpecialType = 0;
			break;
	
			case BXPT_DirecTvMessageType_eRegular_CapFilter0: 
			case BXPT_DirecTvMessageType_eRegular_CapFilter1: 
			case BXPT_DirecTvMessageType_eRegular_CapFilter2: 
			case BXPT_DirecTvMessageType_eRegular_CapFilter3: 
			case BXPT_DirecTvMessageType_eRegular_CapFilter4: 
			GenOffset = 3;
			OutputMode = 0x09; 

            /* Determine which CAP filter is to used. */
            SpecialSel = MessageType - BXPT_DirecTvMessageType_eRegular_CapFilter0;
            SpecialType = 2;
			break;
			 
			/* Regular mode is also the default case. */
			default:
			case BXPT_DirecTvMessageType_eRegular: 
			GenOffset = 3;
			OutputMode = 0x09; 
            SpecialSel = 0;
            SpecialType = 0;
			break;
		}
	
		/* Select the bank the filters are in. */
		RegAddr = BCHP_XPT_MSG_BUF_CTRL2_TABLE_i_ARRAY_BASE + ( MesgBufferNum * PID_CTRL2_TABLE_STEP );
		Reg = BREG_Read32( hXpt->hRegister, RegAddr );
		Reg &= ~(
			BCHP_MASK( XPT_MSG_BUF_CTRL2_TABLE_i, GEN_OFFSET ) |
			BCHP_MASK( XPT_MSG_BUF_CTRL2_TABLE_i, SPECIAL_TYPE ) |
			BCHP_MASK( XPT_MSG_BUF_CTRL2_TABLE_i, SKIP_BYTE2 ) |
			BCHP_MASK( XPT_MSG_BUF_CTRL2_TABLE_i, GEN_GRP_SEL ) |
			BCHP_MASK( XPT_MSG_BUF_CTRL2_TABLE_i, FILTER_MODE ) 
		);
		Reg |= (
			BCHP_FIELD_DATA( XPT_MSG_BUF_CTRL2_TABLE_i, GEN_OFFSET, GenOffset ) |
			BCHP_FIELD_DATA( XPT_MSG_BUF_CTRL2_TABLE_i, SPECIAL_TYPE, SpecialType ) |
			BCHP_FIELD_DATA( XPT_MSG_BUF_CTRL2_TABLE_i, SKIP_BYTE2, 1 ) |
			BCHP_FIELD_DATA( XPT_MSG_BUF_CTRL2_TABLE_i, GEN_GRP_SEL, GroupSel ) |
			BCHP_FIELD_DATA( XPT_MSG_BUF_CTRL2_TABLE_i, FILTER_MODE, 0 )
		);
		BREG_Write32( hXpt->hRegister, RegAddr, Reg );
	
		RegAddr = BCHP_XPT_MSG_BUF_CTRL1_TABLE_i_ARRAY_BASE + ( MesgBufferNum * PID_CTRL1_TABLE_STEP );
		Reg = BREG_Read32( hXpt->hRegister, RegAddr );
		Reg &= ~(
			BCHP_MASK( XPT_MSG_BUF_CTRL1_TABLE_i, ERR_CK_MODE ) |
			BCHP_MASK( XPT_MSG_BUF_CTRL1_TABLE_i, ERR_CK_DIS ) |
			BCHP_MASK( XPT_MSG_BUF_CTRL1_TABLE_i, SPECIAL_SEL ) |
			BCHP_MASK( XPT_MSG_BUF_CTRL1_TABLE_i, ALIGN_MODE ) |
			BCHP_MASK( XPT_MSG_BUF_CTRL1_TABLE_i, DIRECTV_SAVE_FLAGS ) |
			BCHP_MASK( XPT_MSG_BUF_CTRL1_TABLE_i, DATA_OUTPUT_MODE ) 
		);
	
		/* Configure the message buffer for capturing PSI messages. */
		Reg |= (
			BCHP_FIELD_DATA( XPT_MSG_BUF_CTRL1_TABLE_i, SPECIAL_SEL, SpecialSel ) |
			BCHP_FIELD_DATA( XPT_MSG_BUF_CTRL1_TABLE_i, ALIGN_MODE, Settings->ByteAlign == true ? 0 : 1 ) |
			BCHP_FIELD_DATA( XPT_MSG_BUF_CTRL1_TABLE_i, DIRECTV_SAVE_FLAGS, Options->Flags ) |
			BCHP_FIELD_DATA( XPT_MSG_BUF_CTRL1_TABLE_i, DATA_OUTPUT_MODE, OutputMode ) 
		);

		/* CRC checks can be disabled on a per-PID channel basis. Do this here if requested in the settings struct. */
		Reg |= (
			BCHP_FIELD_DATA( XPT_MSG_BUF_CTRL1_TABLE_i, ERR_CK_MODE, 0 ) |
			BCHP_FIELD_DATA( XPT_MSG_BUF_CTRL1_TABLE_i, ERR_CK_DIS, Settings->CrcDisable == true ? 1 : 0 ) 
		);
		BREG_Write32( hXpt->hRegister, RegAddr, Reg );

		/* Enable data from the PID channel to the mesg block. */
		BXPT_P_SetPidChannelDestination( hXpt, PidChannelNum, 6, true );

		/* Set this PID channel as allocated, in case they forced the channel assignment. */
		hXpt->PidChannelTable[ PidChannelNum ].IsAllocated = true;

        /* Enable data from the PID channel to the mesg block. */
    	if(hXpt->PidChannelTable[ PidChannelNum ].MessageBuffercount == 0)
       	{
            /* Configure the PID channel. */
            ExitCode = BXPT_ConfigurePidChannel( hXpt, PidChannelNum, Settings->Pid, Settings->Band );
            if( ExitCode != BERR_SUCCESS )
                goto Done;
       	}

        ConfigPid2BufferMap( hXpt, PidChannelNum, MesgBufferNum, true);

        /* Enable the PID channel. */ 
#if BXPT_MESG_DONT_AUTO_ENABLE_PID_CHANNEL
        /* do nothing. must be explicitly enabled. */
#else
    	if(hXpt->PidChannelTable[ PidChannelNum ].MessageBuffercount == 0)
    	{
            ExitCode = BERR_TRACE( BXPT_EnablePidChannel( hXpt, PidChannelNum ) );
    	}
#endif

    	hXpt->PidChannelTable[ PidChannelNum ].MessageBuffercount++;
	}
	else
	{
        /* Wrong function call. Complain. */
        BDBG_ERR(( "Pid2BuffMapping is OFF. You might need to call BXPT_SetPid2Buff() to turn it ON" ));
        ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
	}

	Done:
	return( ExitCode );
}


BERR_Code BXPT_Mesg_StopDirecTvMessageCapture( 
	BXPT_Handle hXpt, 	   		/* [in] Handle for this transport */
	unsigned int ScidChannelNum,		/* [in] Which SCID channel. */
	unsigned int MesgBufferNum     /* [in] Which Message Buffer. */
	)
{
	BDBG_ASSERT( hXpt );

	return( BERR_TRACE(BXPT_Mesg_StopPidChannelRecord( hXpt, ScidChannelNum, MesgBufferNum )) );
}


BERR_Code BXPT_DirecTv_SetCapPattern( 
	BXPT_Handle hXpt, 	  		/* [in] Handle for this transport */
	unsigned AddressFilter,  	/* [in] Which address filter gets the pattern. */
	uint32_t Pattern  	 	 	/* [in] The pattern to load. */
	)
{
	uint32_t RegAddr;

	BERR_Code ExitCode = BERR_SUCCESS;

	BDBG_ASSERT( hXpt );

	if( AddressFilter > BXPT_NUM_CAP_FILTERS )
	{
		BDBG_ERR(( "AddressFilter %lu is out of range!", ( unsigned long ) AddressFilter ));
		ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
	}
	else
	{
		RegAddr = BCHP_XPT_MSG_C_MATCH0 + ( AddressFilter * CAP_FILT_REG_STEPSIZE );
		BREG_Write32( hXpt->hRegister, RegAddr, Pattern );
	}
	
	return( ExitCode );
}

#endif

BERR_Code BXPT_DirecTv_ConfigHdFiltering( 
	BXPT_Handle hXpt, 	  				/* [in] Handle for this transport */
	unsigned int PidChannelNum,  		/* [in] Which PID channel. */
	bool EnableFilter, 					/* [in] Which SCID channel. */
	BXPT_HdFilterMode FilterMode		/* [in] HD values to filter on. Ignored if EnableFilter == false */
	)
{
	BERR_Code ExitCode = BERR_SUCCESS;

	BDBG_ASSERT( hXpt );

	/* Sanity check on the arguments. */
	if( PidChannelNum >= hXpt->MaxPidChannels )	   
	{
		/* Bad parser band number. Complain. */
		BDBG_ERR(( "PidChannelNum %lu is out of range!", ( unsigned long ) PidChannelNum ));
		ExitCode = BERR_TRACE(BERR_INVALID_PARAMETER);
	}
	else
	{
		uint32_t Reg;
        unsigned OldHdEn = 255;
        unsigned OldFilterMode = 255;

		uint32_t RegAddr = BCHP_XPT_FE_PID_TABLE_i_ARRAY_BASE + ( PID_CHANNEL_STEPSIZE * PidChannelNum );

#ifdef ENABLE_PLAYBACK_MUX
		/*gain access to the pid table*/
		BKNI_EnterCriticalSection();
#endif /*ENABLE_PLAYBACK_MUX*/

		Reg = BREG_Read32( hXpt->hRegister, RegAddr );
        OldHdEn = BCHP_GET_FIELD_DATA( Reg, XPT_FE_PID_TABLE_i, ENABLE_HD_FILTER );
        OldFilterMode = BCHP_GET_FIELD_DATA( Reg, XPT_FE_PID_TABLE_i, HD_FILT_EN_HD_FILTER_TYPE );

		if( EnableFilter == true )
		{
			Reg &= ~( 
				BCHP_MASK( XPT_FE_PID_TABLE_i, ENABLE_HD_FILTER ) |
				BCHP_MASK( XPT_FE_PID_TABLE_i, HD_FILT_EN_HD_FILTER_TYPE ) 
				);
			Reg |= ( 
				BCHP_FIELD_DATA( XPT_FE_PID_TABLE_i, ENABLE_HD_FILTER, 1 ) |
				BCHP_FIELD_DATA( XPT_FE_PID_TABLE_i, HD_FILT_EN_HD_FILTER_TYPE, FilterMode ) 
				);
		}
		else
		{
			Reg &= ~( 
				BCHP_MASK( XPT_FE_PID_TABLE_i, ENABLE_HD_FILTER ) 
				);
		}

        if( EnableFilter != OldHdEn || FilterMode != OldFilterMode )
        {
    		BREG_Write32( hXpt->hRegister, RegAddr, Reg );		
        }

#ifdef ENABLE_PLAYBACK_MUX
		/*leave pid table protected area*/
		BKNI_LeaveCriticalSection();
#endif /*ENABLE_PLAYBACK_MUX*/
	}

	return( ExitCode );
}


/* end of file */

