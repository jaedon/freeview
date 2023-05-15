/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxpt_packetsub.c $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 10/1/12 10:00a $
 *
 * Porting interface code for the data transport core. 
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/xpt/src/core28nm/bxpt_packetsub.c $
 * 
 * Hydra_Software_Devel/1   10/1/12 10:00a gmullen
 * SW7445-17: Created core28nm file tree
 * 
 * Hydra_Software_Devel/9   8/2/12 4:31p gmullen
 * SW7425-3617: Merged to Hydra
 * 
 * Hydra_Software_Devel/SW7425-3617/1   8/1/12 4:04p gmullen
 * SW7425-3617: PI will access device RAM through cached addresses
 * 
 * Hydra_Software_Devel/8   1/12/12 2:40p gmullen
 * SW7425-2138: Fixed incorrect parser band index
 * 
 * Hydra_Software_Devel/7   12/16/11 2:10p gmullen
 * SW7405-5573: Check that packetsub channel is not already open
 * 
 * Hydra_Software_Devel/6   12/15/11 9:50a gmullen
 * SW7425-1589: Merged to Hydra_Software_Devel
 * 
 * Hydra_Software_Devel/SW7425-1589/1   12/12/11 4:35p gmullen
 * SW7425-1589: Added BXPT_PacketSub_GetEobIntId()
 * 
 * Hydra_Software_Devel/5   8/22/11 5:45p gmullen
 * SW7231-319: Merged to Hydra branch
 * 
 * Hydra_Software_Devel/SW7231-319/1   8/16/11 2:15p gmullen
 * SW7231-319: Initial support for B0
 * 
 * Hydra_Software_Devel/4   5/12/11 4:59p gmullen
 * SW7231-128: Merged to mainline
 * 
 * Hydra_Software_Devel/SW7231-128/1   5/10/11 1:40p gmohile
 * SW7231-128 : Add power management support
 * 
 * Hydra_Software_Devel/3   1/25/11 3:10p rjlewis
 * SW7420-879: supply a means to provide an output rate default on open.
 * Get defaults provides the default value.
 * 
 * Hydra_Software_Devel/2   10/28/10 2:08p gmullen
 * SW7422-20: Checkin ported files
 * 
 * Hydra_Software_Devel/15   8/17/10 4:52p gmullen
 * SW7403-924: Merged to Hydra_Software_Devel
 * 
 * Hydra_Software_Devel/SW7403-924/5   8/17/10 4:42p gmullen
 * SW7403-924: Reduce MAX_PID_CHANNELS when PSUB PID channels are not
 * changed
 * 
 * Hydra_Software_Devel/SW7403-924/4   8/11/10 2:12p gmullen
 * SW7403-924: Set MAX_PID_CHANNEL to 249 during all PID/SPID writes.
 * 
 * Hydra_Software_Devel/SW7403-924/3   8/11/10 10:53a gmullen
 * SW7403-924: New method: change band_num in PSUB
 * 
 * Hydra_Software_Devel/SW7403-924/2   8/10/10 1:51p gmullen
 * SW7403-924: Set max PID channel before writing to PID/SPID table
 * 
 * Hydra_Software_Devel/SW7403-924/1   8/9/10 9:07a gmullen
 * SW7403-924: See the JIRA for the workaround notes
 * 
 * Hydra_Software_Devel/14   5/14/10 5:53p gmullen
 * SW7420-750: Merged change to Hydra_Software_Devel
 * 
 * Hydra_Software_Devel/SW7420-750/1   5/14/10 4:37p gmullen
 * SW7420-750: Added playback support to BXPT_PacketSub_SetPidChanNum()
 * 
 * Hydra_Software_Devel/13   6/17/09 6:46p gmullen
 * PR56110: Added support.xpt/7550/uif_image/a0
 * 
 * Hydra_Software_Devel/12   4/7/09 5:25p piyushg
 * PR52986: Add support for 7635
 * Added directory element "7635".
 * 
 * Hydra_Software_Devel/11   1/27/09 1:09p gmullen
 * PR51625: Added 7336 support
 * 
 * Hydra_Software_Devel/10   12/15/08 2:36p gmullen
 * PR48908: Removed power management code from XPT PI.
 * 
 * Hydra_Software_Devel/9   11/26/08 4:14p gmullen
 * PR47755: Added support for 7420.
 * 
 * Hydra_Software_Devel/8   10/28/08 4:58p gmullen
 * PR46544: Added power management support, default to disabled for now.
 * 
 * Hydra_Software_Devel/7   3/26/08 11:24a gmullen
 * PR38954: Added 3548 support to XPT PI.
 * 
 * Hydra_Software_Devel/6   11/28/07 11:24a gmullen
 * PR36900: Added 7335 support
 * 
 * Hydra_Software_Devel/5   10/30/07 3:11p gmullen
 * PR35018: Added support for 7325
 * 
 * Hydra_Software_Devel/4   7/17/06 5:19p gmullen
 * PR20624: Added code for 7118.
 * 
 * Hydra_Software_Devel/3   7/10/06 9:32a gmullen
 * PR18998: Fixed void * dereference warning.
 * 
 * Hydra_Software_Devel/2   2/1/06 10:19a gmullen
 * PR18998: Fixed overflow issue in RAVE ITB/CDB, added support for PB
 * channels.
 * 
 * Hydra_Software_Devel/1   1/16/06 2:09p gmullen
 * PR18998: Initial checkin for 7400
 * 
 * Hydra_Software_Devel/3   1/6/06 2:22p gmullen
 * PR18489: Added PID destination workaround for hardware PR 18919.
 * 
 * Hydra_Software_Devel/2   12/2/05 3:24p gmullen
 * PR15309: Saved updates for ccase server move
 * 
 * Hydra_Software_Devel/1   7/28/05 3:40p gmullen
 * PR15309: Initial version for building.
 * 
 *
 ***************************************************************************/

#include "bstd.h"
#include "bxpt_priv.h"
#include "bxpt_packetsub.h"
#include "bkni.h"

#if BCHP_PWR_SUPPORT
#include "bchp_pwr.h"
#endif

#include "bchp_xpt_fe.h"
#include "bchp_int_id_xpt_bus_if.h"
#include "bchp_xpt_psub.h"

/* Size of an individual packetsub module register space, in bytes */
#ifdef BCHP_XPT_PSUB_PSUB1_CTRL0 
#define PACKET_SUB_REGISTER_STEP	( BCHP_XPT_PSUB_PSUB1_CTRL0 - BCHP_XPT_PSUB_PSUB0_CTRL0 )
#else
#define PACKET_SUB_REGISTER_STEP	( BCHP_XPT_PSUB_PSUB0_STAT2 - BCHP_XPT_PSUB_PSUB0_CTRL0 )
#endif

#define BXPT_P_PSUB_DEFAULT_PACKET_LEN		( 188 )
#define BXPT_P_PSUB_DEFAULT_BAND_NUM		( 0 )
#define BXPT_P_PSUB_DEFAULT_DMA_PRIORITY	BXPT_PacketSubDmaPriority_eLow
#define BXPT_P_MAX_PSUB_OUTPUT_RATE         ( 1000000 )
#define BXPT_NUM_OUTPUT_RATE_REGISTER_VALUE ( 65535 )

#if( BDBG_DEBUG_BUILD == 1 )
BDBG_MODULE( xpt_packetsub );
#endif

static void BXPT_PacketSub_P_WriteReg(
	BXPT_PacketSub_Handle hPSub,	/* [in] Handle for the channel. */
	uint32_t Reg0Addr,
	uint32_t RegVal
	);

static uint32_t BXPT_PacketSub_P_ReadReg(
	BXPT_PacketSub_Handle hPSub,	/* [in] Handle for the channel. */
	uint32_t Reg0Addr
	);

BERR_Code BXPT_PacketSub_GetTotalChannels(
	BXPT_Handle hXpt, 	   		/* [in] Handle for this transport */
	unsigned int *TotalChannels		/* [out] The number of PacketSub channels. */
	)
{
	BERR_Code ExitCode = BERR_SUCCESS;

	BDBG_ASSERT( hXpt );
	
	*TotalChannels = hXpt->MaxPacketSubs;

	return( ExitCode );
}

BERR_Code BXPT_PacketSub_GetChannelDefaultSettings(
	BXPT_Handle hXpt, 	   		/* [in] Handle for this transport */
	unsigned int ChannelNo,			/* [in] Which channel to get defaults from. */
	BXPT_PacketSub_ChannelSettings *ChannelSettings /* [out] The defaults */
	)
{
	BERR_Code ExitCode = BERR_SUCCESS;

	BDBG_ASSERT( hXpt );
	BDBG_ASSERT( ChannelSettings );

	if( ChannelNo >= hXpt->MaxPacketSubs )
	{
		/* Bad PID channel number. Complain. */
		BDBG_ERR(( "ChannelNo %lu is out of range!", ( unsigned long ) ChannelNo ));
		ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
	}
	else
	{
		ChannelSettings->PacketLen = BXPT_P_PSUB_DEFAULT_PACKET_LEN;
        ChannelSettings->ForcedInsertionEn = false;
	}

	ChannelSettings->OutputRate = 0xBC * 1649; /* this will produce a register value of 0xBC the hardware default */

	return( ExitCode );
}

BERR_Code BXPT_PacketSub_OpenChannel(
	BXPT_Handle hXpt, 	   						/* [in] Handle for this transport */
	BXPT_PacketSub_Handle *hPSub,				/* [out] Handle for opened packet sub channel */
	unsigned int ChannelNo,							/* [in] Which channel to open. */
	BXPT_PacketSub_ChannelSettings *ChannelSettings /* [in] The defaults to use */
	)
{
	BERR_Code ExitCode = BERR_SUCCESS;
	BXPT_PacketSub_Handle hLocal = ( BXPT_PacketSub_Handle ) NULL;

	BDBG_ASSERT( hXpt );
	BDBG_ASSERT( ChannelSettings );

	if( ChannelNo >= BXPT_NUM_PACKETSUBS )
	{
		/* Bad playback channel number. Complain. */
		BDBG_ERR(( "ChannelNo %lu is out of range!", ( unsigned long ) ChannelNo ));
		ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
	}
    else if( hXpt->PacketSubHandles[ ChannelNo ].Opened )
    {
		BDBG_ERR(( "PacketSub channel %u already opened.", ChannelNo ));
		ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
    }
	else
	{
		uint32_t Reg;

		/* 
		** Use the address of the first register in the packet sub block as the 
		** base address of the entire block. 
		*/
		uint32_t BaseAddr = BCHP_XPT_PSUB_PSUB0_CTRL0 + ( ChannelNo * PACKET_SUB_REGISTER_STEP ); 

		/* Create the packet sub channel handle. */
		hLocal = &hXpt->PacketSubHandles[ ChannelNo ];
		hLocal->hChip = hXpt->hChip;
		hLocal->hRegister = hXpt->hRegister;
		hLocal->hMemory = hXpt->hMemory;
		hLocal->BaseAddr = BaseAddr;
		hLocal->ChannelNo = ChannelNo;
		hLocal->LastDescriptor_Cached = NULL;
		hLocal->Running = false;
        hLocal->vhXpt = (void *) hXpt;

		/* Do a sanity check on the defaults they passed in, then load them. */
		if( ChannelSettings->PacketLen > 255 )
		{
			BDBG_ERR(( "PacketLen %lu is out of range!. Clamped to 255.", 
				( unsigned long ) ChannelSettings->PacketLen ));
			ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
			ChannelSettings->PacketLen = 255;
		}

		/* Use their value for a default output rate.  Don't fail if this value is out of range */
		BXPT_PacketSub_SetOutputRate( hLocal, ChannelSettings->OutputRate );

		Reg = BXPT_PacketSub_P_ReadReg( hLocal, BCHP_XPT_PSUB_PSUB0_CTRL0 );
		Reg &= ~( 
			BCHP_MASK( XPT_PSUB_PSUB0_CTRL0, PACKET_LENGTH ) |
			BCHP_MASK( XPT_PSUB_PSUB0_CTRL0, FORCED_INSERTION_EN ) 
			);
		Reg |= ( 
			BCHP_FIELD_DATA( XPT_PSUB_PSUB0_CTRL0, PACKET_LENGTH, ChannelSettings->PacketLen ) |
			BCHP_FIELD_DATA( XPT_PSUB_PSUB0_CTRL0, FORCED_INSERTION_EN, ChannelSettings->ForcedInsertionEn == true ? 1 : 0 )
			);
		BXPT_PacketSub_P_WriteReg( hLocal, BCHP_XPT_PSUB_PSUB0_CTRL0, Reg ); 

		Reg = BXPT_PacketSub_P_ReadReg( hLocal, BCHP_XPT_PSUB_PSUB0_CTRL1 );
		Reg &= ~( 
			BCHP_MASK( XPT_PSUB_PSUB0_CTRL1, DATA_ENDIAN_CTRL ) 
			);
		Reg |= ( 
			BCHP_FIELD_DATA( XPT_PSUB_PSUB0_CTRL1, DATA_ENDIAN_CTRL, hXpt->IsLittleEndian == true ? 1 : 0 ) 
			);
		BXPT_PacketSub_P_WriteReg( hLocal, BCHP_XPT_PSUB_PSUB0_CTRL1, Reg ); 
		hLocal->Opened = true;
 	}

 	*hPSub = hLocal;
	return( ExitCode );
}

void BXPT_PacketSub_CloseChannel(
	BXPT_PacketSub_Handle hPSub	/* [in] Handle for the channel to close*/
	)
{
	uint32_t Reg;

	BDBG_ASSERT( hPSub );

	Reg = BXPT_PacketSub_P_ReadReg( hPSub, BCHP_XPT_PSUB_PSUB0_STAT0 );
	Reg &= ~( 
		BCHP_MASK( XPT_PSUB_PSUB0_STAT0, WAKE_MODE ) |
		BCHP_MASK( XPT_PSUB_PSUB0_STAT0, RUN ) |
		BCHP_MASK( XPT_PSUB_PSUB0_STAT0, WAKE ) 
		);
	Reg |= (
		BCHP_FIELD_DATA( XPT_PSUB_PSUB0_STAT0, WAKE_MODE, 0 ) |
		BCHP_FIELD_DATA( XPT_PSUB_PSUB0_STAT0, RUN, 0 ) |
		BCHP_FIELD_DATA( XPT_PSUB_PSUB0_STAT0, WAKE, 0 ) 
	);
	BXPT_PacketSub_P_WriteReg( hPSub, BCHP_XPT_PSUB_PSUB0_STAT0, Reg );
	
	hPSub->Opened = false;
}


unsigned int BXPT_PacketSub_GetPidChanNum( 
	BXPT_PacketSub_Handle hPSub 	/* [in] Handle for the channel. */
	)
{
    unsigned int PidChannelNum;
	BXPT_Handle hXpt; 	   						/* [in] Handle for this transport */

	BDBG_ASSERT( hPSub );

    hXpt = (BXPT_Handle) hPSub->vhXpt;
    BXPT_AllocPidChannel( hXpt, false, &PidChannelNum );

    return PidChannelNum;
}

BERR_Code BXPT_PacketSub_SetPidChanNum( 
	BXPT_PacketSub_Handle hPSub,	/* [in] Handle for the channel. */
	unsigned int PidChanNum,		/* [in] Which PID channel to assign the output to. */
	unsigned int BandNum			/* [in] Which band number to assign the output to */
	)
{
	uint32_t Reg;

	BERR_Code ExitCode = BERR_SUCCESS;
    bool IsPbBand = BXPT_P_IS_PB( BandNum );

	BDBG_ASSERT( hPSub );

    BXPT_P_CLEAR_PB_FLAG( BandNum );  

    if( PidChanNum >= BXPT_NUM_PID_CHANNELS )
	{
		/* Bad PID channel number. Complain. */
		BDBG_ERR(( "PidChanNum %lu is out of range!", ( unsigned long ) PidChanNum ));
		ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
	}
	else if( !IsPbBand && BandNum > BXPT_NUM_PID_PARSERS )
	{
		BDBG_ERR(( "Input band BandNum %lu is out of range!", ( unsigned long ) BandNum ));
		ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
	}
	else if( IsPbBand && BandNum > BXPT_NUM_PLAYBACKS )
	{
		BDBG_ERR(( "Playback band BandNum %lu is out of range!", ( unsigned long ) BandNum ));
		ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
	}
	else
	{
        /* For playbacks, band numbers are mapped beginning at an architecture-specific offset. */
        BandNum = IsPbBand ? (BandNum + BXPT_PB_PARSER_BAND_BASE) : BandNum;  

#ifdef BCHP_XPT_PSUB_PSUB0_CTRL1_OUTPUT_CH_NUM_MASK
        Reg = BXPT_PacketSub_P_ReadReg( hPSub, BCHP_XPT_PSUB_PSUB0_CTRL0 );
		Reg &= ~( 
			BCHP_MASK( XPT_PSUB_PSUB0_CTRL0, BAND_SEL ) 
			);
		Reg |= ( 
			BCHP_FIELD_DATA( XPT_PSUB_PSUB0_CTRL0, BAND_SEL, BandNum )  
			);
		BXPT_PacketSub_P_WriteReg( hPSub, BCHP_XPT_PSUB_PSUB0_CTRL0, Reg ); 

        Reg = BXPT_PacketSub_P_ReadReg( hPSub, BCHP_XPT_PSUB_PSUB0_CTRL1 );
		Reg &= ~( 
			BCHP_MASK( XPT_PSUB_PSUB0_CTRL1, OUTPUT_CH_NUM ) 
			);
		Reg |= ( 
			BCHP_FIELD_DATA( XPT_PSUB_PSUB0_CTRL1, OUTPUT_CH_NUM, PidChanNum ) 
			);
		BXPT_PacketSub_P_WriteReg( hPSub, BCHP_XPT_PSUB_PSUB0_CTRL1, Reg ); 
#else
        Reg = BXPT_PacketSub_P_ReadReg( hPSub, BCHP_XPT_PSUB_PSUB0_CTRL0 );
		Reg &= ~( 
			BCHP_MASK( XPT_PSUB_PSUB0_CTRL0, BAND_SEL ) | 
			BCHP_MASK( XPT_PSUB_PSUB0_CTRL0, OUTPUT_CH_NUM ) 
			);
		Reg |= ( 
			BCHP_FIELD_DATA( XPT_PSUB_PSUB0_CTRL0, BAND_SEL, BandNum ) | 
			BCHP_FIELD_DATA( XPT_PSUB_PSUB0_CTRL0, OUTPUT_CH_NUM, PidChanNum ) 
			);
		BXPT_PacketSub_P_WriteReg( hPSub, BCHP_XPT_PSUB_PSUB0_CTRL0, Reg ); 
#endif
	}

	return( ExitCode );
}

BERR_Code BXPT_PacketSub_SetForcedOutput( 
	BXPT_PacketSub_Handle hPSub,	/* [in] Handle for the channel. */
	bool Enable			/* [in] Force output immediately if TRUE */
	)
{
	uint32_t Reg;

	BERR_Code ExitCode = BERR_SUCCESS;

	BDBG_ASSERT( hPSub );

	Reg = BXPT_PacketSub_P_ReadReg( hPSub, BCHP_XPT_PSUB_PSUB0_CTRL0 );
	Reg &= ~( 
		BCHP_MASK( XPT_PSUB_PSUB0_CTRL0, FORCED_OUTPUT_ENABLE ) 
		);
	Reg |= ( 
		BCHP_FIELD_DATA( XPT_PSUB_PSUB0_CTRL0, FORCED_OUTPUT_ENABLE, Enable == true ? 1 : 0 ) 
		);
	BXPT_PacketSub_P_WriteReg( hPSub, BCHP_XPT_PSUB_PSUB0_CTRL0, Reg ); 

	return( ExitCode );
}

BERR_Code BXPT_PacketSub_SetForcedInsertion( 
	BXPT_PacketSub_Handle hPSub,	/* [in] Handle for the channel. */
	bool Enable			/* [in] Force output immediately if TRUE */
	)
{
	uint32_t Reg;

	BERR_Code ExitCode = BERR_SUCCESS;

	BDBG_ASSERT( hPSub );

    Reg = BXPT_PacketSub_P_ReadReg( hPSub, BCHP_XPT_PSUB_PSUB0_CTRL0 );
    Reg &= ~( 
        BCHP_MASK( XPT_PSUB_PSUB0_CTRL0, FORCED_INSERTION_EN ) 
        );
    Reg |= ( 
        BCHP_FIELD_DATA( XPT_PSUB_PSUB0_CTRL0, FORCED_INSERTION_EN, Enable == true ? 1 : 0 )
        );
    BXPT_PacketSub_P_WriteReg( hPSub, BCHP_XPT_PSUB_PSUB0_CTRL0, Reg ); 

	return( ExitCode );
}

BERR_Code BXPT_PacketSub_SetFullRateOutput( 
	BXPT_PacketSub_Handle hPSub,	/* [in] Handle for the channel. */
	bool Enable			/* [in] Use full rate if TRUE */
	)
{
	uint32_t Reg;

	BERR_Code ExitCode = BERR_SUCCESS;

	BDBG_ASSERT( hPSub );

	Reg = BXPT_PacketSub_P_ReadReg( hPSub, BCHP_XPT_PSUB_PSUB0_CTRL0 );
	Reg &= ~( 
		BCHP_MASK( XPT_PSUB_PSUB0_CTRL0, FULL_RATE_OUTPUT_ENABLE ) 
		);
	Reg |= ( 
		BCHP_FIELD_DATA( XPT_PSUB_PSUB0_CTRL0, FULL_RATE_OUTPUT_ENABLE, Enable == true ? 1 : 0 ) 
		);
	BXPT_PacketSub_P_WriteReg( hPSub, BCHP_XPT_PSUB_PSUB0_CTRL0, Reg ); 

	return( ExitCode );
}

BERR_Code BXPT_PacketSub_SetOutputRate( 
	BXPT_PacketSub_Handle hPSub,	/* [in] Handle for the channel. */
	uint32_t OutputRate	/* [in] The output rate, in bits/second */
	)
{
	uint32_t Reg;
 	uint32_t NewRate;

	BERR_Code ExitCode = BERR_SUCCESS;

	BDBG_ASSERT( hPSub );

    /*
        See SW7420-879 for details. The time between each packet substitution (sec) = 0.912 / OUTPUT_RATE. 
        We are given the insertion rate in (the OutputRate argument) bits/sec, and we need to solve for 
        the OUTPUT_RATE register setting.
            packets/sec = OUTPUT_RATE/0.912
            bits/sec = (188*8) bits/packet * OUTPUT_RATE/0.912
            OUTPUT_RATE = bits/sec * 0.912/1504
            OUTPUT_RATE = OutputRate / 1649
    */

	if( OutputRate > BXPT_P_MAX_PSUB_OUTPUT_RATE )
	{
		BDBG_ERR(( "OutputRate %lu is out of range! Clamped to %lu", 
			( unsigned long ) OutputRate, BXPT_P_MAX_PSUB_OUTPUT_RATE ));
        NewRate = 65535;    /* Max value this bitfield can hold */
		ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
	}
	else
    {
        NewRate = (uint32_t) OutputRate / 1649;
    }
    if( NewRate == 0 )
        NewRate = 1;    /* Handle round-down condition */

	Reg = BXPT_PacketSub_P_ReadReg( hPSub, BCHP_XPT_PSUB_PSUB0_CTRL1 );
	Reg &= ~( 
		BCHP_MASK( XPT_PSUB_PSUB0_CTRL1, OUTPUT_RATE ) 
		);
	Reg |= ( 
		BCHP_FIELD_DATA( XPT_PSUB_PSUB0_CTRL1, OUTPUT_RATE, NewRate ) 
		);
	BXPT_PacketSub_P_WriteReg( hPSub, BCHP_XPT_PSUB_PSUB0_CTRL1, Reg ); 

	return( ExitCode );
}

BERR_Code BXPT_PacketSub_PauseChannel( 
	BXPT_PacketSub_Handle hPSub,	/* [in] Handle for the channel. */
	bool Pause			/* [in] Pause channel if TRUE, continue if FALSE */
	)
{
	uint32_t Reg;

	BERR_Code ExitCode = BERR_SUCCESS;

	BDBG_ASSERT( hPSub );

	Reg = BXPT_PacketSub_P_ReadReg( hPSub, BCHP_XPT_PSUB_PSUB0_CTRL0 );
	Reg &= ~( 
		BCHP_MASK( XPT_PSUB_PSUB0_CTRL0, PAUSE ) 
		);
	Reg |= ( 
		BCHP_FIELD_DATA( XPT_PSUB_PSUB0_CTRL0, PAUSE, Pause == true ? 1 : 0 ) 
		);
	BXPT_PacketSub_P_WriteReg( hPSub, BCHP_XPT_PSUB_PSUB0_CTRL0, Reg ); 

	return( ExitCode );
}

BERR_Code BXPT_PacketSub_CreateDesc( 
	BXPT_Handle hXpt, 	   					/* [in] Handle for this transport */
	BXPT_PacketSub_Descriptor * const Desc,		/* [in] Descriptor to initialize */ 
	uint8_t *Buffer, 							/* [in] Data buffer. */
	uint32_t BufferLength, 						/* [in] Size of buffer (in bytes). */
	bool IntEnable, 							/* [in] Interrupt when done? */
	BXPT_PacketSub_Descriptor * const NextDesc 	/* [in] Next descriptor, or NULL */
	)
{
	uint32_t BufferPhysicalAddr;
	uint32_t ThisDescPhysicalAddr;
    BXPT_PacketSub_Descriptor *CachedDescPtr;

	BERR_Code ExitCode = BERR_SUCCESS;

	BDBG_ASSERT( hXpt );
	BDBG_ASSERT( Desc );
	BDBG_ASSERT( Buffer );

	/* Get the physical address for this buffer. Verify its on a 4-byte boundary*/
	BMEM_ConvertAddressToOffset( hXpt->hMemory, ( void * ) Buffer, &BufferPhysicalAddr );
	if( BufferPhysicalAddr % 4 )
	{
		BDBG_ERR(( "Buffer is not 32-bit aligned!" ));
		ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
		
		/* Force the alignment. */
		BufferPhysicalAddr += ( BufferPhysicalAddr % 4 );
	}

	/* Verify that the buffer length is multiple of 4 bytes (i.e. a word). */
	if( BufferLength % 4 )
	{
		BDBG_ERR(( "BufferLength is not 32-bit aligned!" ));
		ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
		
		/* Force the alignment. */
		BufferLength += ( BufferLength % 4 );
	}

	/* Verify that the descriptor we're creating sits on a 16-byte boundary. */
	BMEM_ConvertAddressToOffset( hXpt->hMemory, ( void * ) Desc, &ThisDescPhysicalAddr );
	if( ThisDescPhysicalAddr % 16 )
	{
		BDBG_ERR(( "Desc is not 32-bit aligned!" ));
		ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
	}

    BMEM_ConvertAddressToCached(hXpt->hMemory, (void *) Desc, (void **) &CachedDescPtr);

	/* Load the descriptor's buffer address, length, and flags. */
	CachedDescPtr->BufferStartAddr = BufferPhysicalAddr;
	CachedDescPtr->BufferLength = BufferLength;

	/* Clear everything, then set the ones we want below. */
	CachedDescPtr->Flags = 0;																	   

	if( IntEnable == true )
		CachedDescPtr->Flags |= TRANS_DESC_INT_FLAG;

	/* Load the pointer to the next descriptor in the chain, if there is one. */
	if( NextDesc != 0 )
	{
		/* There is a another descriptor in the chain after this one. */
		uint32_t NextDescPhysAddr;

		BMEM_ConvertAddressToOffset( hXpt->hMemory, ( void * ) NextDesc, &NextDescPhysAddr );
		if( NextDescPhysAddr % 16 )
		{
			BDBG_ERR(( "NextDescDesc is not 32-bit aligned!" ));
			ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
		}

		/* Next descriptor address must be 16-byte aligned. */
		NextDescPhysAddr &= ~( 0xF );
		CachedDescPtr->NextDescAddr = NextDescPhysAddr;
	}
	else				  
	{
		/* There is NOT another descriptor. Set the Last Descriptor bit. */
		CachedDescPtr->NextDescAddr = TRANS_DESC_LAST_DESCR_IND;
	}

    BMEM_FlushCache(hXpt->hMemory, CachedDescPtr, sizeof (BXPT_PacketSub_Descriptor) );
	return( ExitCode );
}

BERR_Code BXPT_PacketSub_AddDescriptors( 
	BXPT_PacketSub_Handle hPSub,	/* [in] Handle for the channel. */
	BXPT_PacketSub_Descriptor *LastDesc, 	/* [in] Last descriptor in new chain */
	BXPT_PacketSub_Descriptor *FirstDesc 	/* [in] First descriptor in new chain */
	)
{
	uint32_t ChanFinished, Reg;
	uint32_t RunBit;

	BERR_Code ExitCode = BERR_SUCCESS;

	BDBG_ASSERT( hPSub );
	BDBG_ASSERT( LastDesc );
	BDBG_ASSERT( FirstDesc );

	BDBG_MSG(("Adding Desc Addr 0x%08lX to Packet Sub Channel %d", ( unsigned long ) FirstDesc, 
		hPSub->ChannelNo ));

	Reg = BXPT_PacketSub_P_ReadReg( hPSub, BCHP_XPT_PSUB_PSUB0_STAT0 ); 
	ChanFinished = BCHP_GET_FIELD_DATA( Reg, XPT_PSUB_PSUB0_STAT0, FINISHED );

	if( ChanFinished )
	{
		/* Channel has finished, so start over at the beginning of the chain. */
		Reg &= ~ ( BCHP_MASK( XPT_PSUB_PSUB0_STAT0, WAKE_MODE ) );
		Reg |= BCHP_FIELD_DATA( XPT_PSUB_PSUB0_STAT0, WAKE_MODE, 1 ); 	   
		hPSub->LastDescriptor_Cached = NULL;
	}
	else
	{
		/* Channel has NOT finished, so start over at the end of the chain. */
		Reg &= ~ ( BCHP_MASK( XPT_PSUB_PSUB0_STAT0, WAKE_MODE ) );
		Reg |= BCHP_FIELD_DATA( XPT_PSUB_PSUB0_STAT0, WAKE_MODE, 0 ); 	   
	}
	BXPT_PacketSub_P_WriteReg( hPSub, BCHP_XPT_PSUB_PSUB0_STAT0, Reg );

	/* Do we already have a list going? */
	if( hPSub->LastDescriptor_Cached )
	{
		uint32_t DescPhysAddr;	  

		/* 
		** Yes, there is list already. Append this descriptor to the last descriptor, 
		** then set the wake bit. 
		*/
		BXPT_PacketSub_Descriptor *LastDescriptor_Cached = ( BXPT_PacketSub_Descriptor * ) hPSub->LastDescriptor_Cached; 
		
		Reg = BXPT_PacketSub_P_ReadReg( hPSub, BCHP_XPT_PSUB_PSUB0_STAT0 ); 
		RunBit = BCHP_GET_FIELD_DATA( Reg, XPT_PSUB_PSUB0_STAT0, RUN );

		/* Set the last descriptor in the chain to point to the descriptor we're adding. */
		BMEM_ConvertAddressToOffset( hPSub->hMemory, ( void * ) FirstDesc, &DescPhysAddr );
		LastDescriptor_Cached->NextDescAddr = ( uint32_t ) DescPhysAddr;
        BMEM_FlushCache(hPSub->hMemory, LastDescriptor_Cached, sizeof (BXPT_PacketSub_Descriptor) );

		/* If the channel is running, we need to set the wake bit to let the hardware know we added a new buffer */
		if( RunBit )
		{
			/* PR 16985: Need to write 0 after writing a 1 */
			Reg |= BCHP_FIELD_DATA( XPT_PSUB_PSUB0_STAT0, WAKE, 1 );
			BXPT_PacketSub_P_WriteReg( hPSub, BCHP_XPT_PSUB_PSUB0_STAT0, Reg );

			Reg |= BCHP_FIELD_DATA( XPT_PSUB_PSUB0_STAT0, WAKE, 0 );
			BXPT_PacketSub_P_WriteReg( hPSub, BCHP_XPT_PSUB_PSUB0_STAT0, Reg );
		}
	}
	else
	{
		/* 
		** If this is the first descriptor (the channel has not been started) 
		** then load the address into the first descriptor register 
		*/
		uint32_t DescPhysAddr;	  

		/* This is our first descriptor, so we must load the first descriptor register */
		BMEM_ConvertAddressToOffset( hPSub->hMemory, ( void * ) FirstDesc, &DescPhysAddr );

		Reg = BXPT_PacketSub_P_ReadReg( hPSub, BCHP_XPT_PSUB_PSUB0_CTRL2 );

		Reg &= ~( BCHP_MASK( XPT_PSUB_PSUB0_CTRL2, FIRST_DESC_ADDR ) );

		/* 
		** The descriptor address field in the hardware register is wants the address
		** in 16-byte blocks. See the RDB HTML for details. So, we must shift the 
		** address 4 bits to the right before writing it to the hardware. Note that
		** the RDB macros will shift the value 4 bits to the left, since the address
		** bitfield starts at bit 4. Confusing, but thats what the hardware and the
		** RDB macros require to make this work.
		*/
		DescPhysAddr >>= 4;
		Reg |= BCHP_FIELD_DATA( XPT_PSUB_PSUB0_CTRL2, FIRST_DESC_ADDR, DescPhysAddr );  
		BXPT_PacketSub_P_WriteReg( hPSub, BCHP_XPT_PSUB_PSUB0_CTRL2, Reg );

		/*
		** If this channel has been started, we need to kick off the hardware
		** by setting the RUN bit. 
		*/
		if( hPSub->Running == true )
		{
			Reg = BXPT_PacketSub_P_ReadReg( hPSub, BCHP_XPT_PSUB_PSUB0_STAT0 ); 
			RunBit = BCHP_GET_FIELD_DATA( Reg, XPT_PSUB_PSUB0_STAT0, RUN );

			if( RunBit )
			{
				/* 
				** Since the channel was already running in hardware, this means that we
				** are reloading the first descriptor address due to the channel
				** finishing before a new descriptor was added.  Therefore
				** we use the wake bit (as we previously set the WAKE_MODE above.
				*/
				Reg &= ~( BCHP_MASK( XPT_PSUB_PSUB0_STAT0, WAKE ) );
				Reg |= BCHP_FIELD_DATA( XPT_PSUB_PSUB0_STAT0, WAKE, 1 );  
			}
			else
			{
				Reg &= ~( BCHP_MASK( XPT_PSUB_PSUB0_STAT0, RUN ) );
				Reg |= BCHP_FIELD_DATA( XPT_PSUB_PSUB0_STAT0, RUN, 1 );  
			}

			BXPT_PacketSub_P_WriteReg( hPSub, BCHP_XPT_PSUB_PSUB0_STAT0, Reg );
		}
	}

	/* This descriptor is always the new last descriptor */
    BMEM_ConvertAddressToCached(hPSub->hMemory, (void *) LastDesc, (void **) &hPSub->LastDescriptor_Cached);

	return( ExitCode );
}

BERR_Code BXPT_PacketSub_GetCurrentDescriptorAddress(
	BXPT_PacketSub_Handle hPSub,			/* [in] Handle for the channel. */
	BXPT_PacketSub_Descriptor **LastDesc		/* [in] Address of the current descriptor. */
	)
{
	uint32_t Reg, CurrentDescAddr;
	void *UserDescAddr;

	BERR_Code ExitCode = BERR_SUCCESS;

	BDBG_ASSERT( hPSub );

	Reg = BXPT_PacketSub_P_ReadReg( hPSub, BCHP_XPT_PSUB_PSUB0_STAT1 );
	CurrentDescAddr = BCHP_GET_FIELD_DATA( Reg, XPT_PSUB_PSUB0_STAT1, CURR_DESC_ADDR );
	CurrentDescAddr <<= 4;	/* Convert to byte-address. */
	BERR_TRACE( BMEM_ConvertOffsetToAddress( hPSub->hMemory, CurrentDescAddr, ( void ** ) &UserDescAddr ) );
	*LastDesc = ( BXPT_PacketSub_Descriptor * ) UserDescAddr;

	return( ExitCode );
}

BERR_Code BXPT_PacketSub_CheckHeadDescriptor( 
	BXPT_PacketSub_Handle hPSub,	/* [in] Handle for the channel. */
	BXPT_PacketSub_Descriptor *Desc, 	/* [in] Descriptor to check. */
	bool *InUse, 						/* [out] Is descriptor in use? */
	uint32_t *BufferSize 				/* [out] Size of the buffer (in bytes). */
	)
{
	uint32_t Reg, ChanBusy, CurrentDescAddr, CandidateDescPhysAddr;

	BERR_Code ExitCode = BERR_SUCCESS;

	BDBG_ASSERT( hPSub );

	/* 
	** Check if the current descriptor being processed by the
	** playback hardware is the first on our hardware list
	** (which means this descriptor is still being used)
	*/
	Reg = BXPT_PacketSub_P_ReadReg( hPSub, BCHP_XPT_PSUB_PSUB0_STAT1 );

	CurrentDescAddr = BCHP_GET_FIELD_DATA( Reg, XPT_PSUB_PSUB0_STAT1, CURR_DESC_ADDR );
	CurrentDescAddr <<= 4;	/* Convert to byte-address. */

	Reg = BXPT_PacketSub_P_ReadReg( hPSub, BCHP_XPT_PSUB_PSUB0_STAT0 );
	ChanBusy = BCHP_GET_FIELD_DATA( Reg, XPT_PSUB_PSUB0_STAT0, BUSY );
	
	BMEM_ConvertAddressToOffset( hPSub->hMemory, ( void * ) Desc, &CandidateDescPhysAddr );

	if( CurrentDescAddr == CandidateDescPhysAddr )
	{
		if( ChanBusy )
		{
			/* The candidate descriptor is being used by hardware. */
			*InUse = true;
		}
		else
		{
			*InUse = false;
		}
	}
	else
	{
		/* 
		** The candidate descriptor isn't being processed. If this is the head descriptor
		** we can conclude that the hardware is finished with the descriptor. 
		*/
		*InUse = false;
	}

	if( *InUse == false )
	{
		if( ChanBusy )
		{
            BXPT_PacketSub_Descriptor *CachedDescPtr;

            BMEM_ConvertAddressToCached(hPSub->hMemory, (void *) Desc, (void **) &CachedDescPtr);
			*BufferSize = CachedDescPtr->BufferLength;
		}
		else
		{
			/* 
			** Since there is valid data in the record channel even after it is stopped,
			** we are unable to detect if we are done or not with a specific descriptor
			** after the record channel has been halted.
			** This check needs to be performed at a higher level
			*/
			*BufferSize = 0;
			*InUse = true;
		}
	}
	else
	{
		*BufferSize = 0;
	}

	return( ExitCode );
}

BERR_Code BXPT_PacketSub_StartChannel( 
	BXPT_PacketSub_Handle hPSub	/* [in] Handle for the channel. */
	)
{
	uint32_t Reg;

	BERR_Code ExitCode = BERR_SUCCESS;

	BDBG_ASSERT( hPSub );

	BDBG_MSG(( "Starting Packet Sub channel %d", ( unsigned long ) hPSub->ChannelNo )); 

	if( hPSub->Running == true )
	{
		BDBG_ERR(( "Packet Sub channel %d cannot be started because it's already running!", 
			( unsigned long ) hPSub->ChannelNo )); 
		ExitCode = BERR_TRACE( BXPT_ERR_CHANNEL_ALREADY_RUNNING );
	}

#ifdef BCHP_PWR_RESOURCE_XPT_PACKETSUB
	if( hPSub->Running == false ) 
	{   
		BCHP_PWR_AcquireResource(hPSub->hChip, BCHP_PWR_RESOURCE_XPT_PACKETSUB);
	}
#endif

	Reg = BXPT_PacketSub_P_ReadReg( hPSub, BCHP_XPT_PSUB_PSUB0_CTRL0 ); 
	Reg |= BCHP_FIELD_DATA( XPT_PSUB_PSUB0_CTRL0, PSUB_ENABLE, 1 );  
	BXPT_PacketSub_P_WriteReg( hPSub, BCHP_XPT_PSUB_PSUB0_CTRL0, Reg );

	/* Check if we have buffers already loaded for this channel */
	if( hPSub->LastDescriptor_Cached )
	{
		/* Since we already have some buffers loaded, we can start the pvr channel */
		Reg = BXPT_PacketSub_P_ReadReg( hPSub, BCHP_XPT_PSUB_PSUB0_STAT0 ); 
		Reg |= BCHP_FIELD_DATA( XPT_PSUB_PSUB0_STAT0, RUN, 1 );  
		BXPT_PacketSub_P_WriteReg( hPSub, BCHP_XPT_PSUB_PSUB0_STAT0, Reg );
	}	

	hPSub->Running = true;

	return( ExitCode );
}

BERR_Code BXPT_PacketSub_StopChannel( 
	BXPT_PacketSub_Handle hPSub	/* [in] Handle for the channel. */
	)
{
	uint32_t Reg, ChanBusy, WaitCount;

	BERR_Code ExitCode = BERR_SUCCESS;

	BDBG_ASSERT( hPSub );

	BDBG_MSG(( "Stopping Packet Sub channel %d", ( unsigned long ) hPSub->ChannelNo )); 

	if( hPSub->Running == false )
	{
		BDBG_ERR(( "Packet Sub channel %d cannot be stopped because it's not running!", 
			( unsigned long ) hPSub->ChannelNo )); 
		ExitCode = BERR_TRACE( BXPT_ERR_CHANNEL_ALREADY_STOPPED );
	}

	/* Stop the channel hardware */
	Reg = BXPT_PacketSub_P_ReadReg( hPSub, BCHP_XPT_PSUB_PSUB0_STAT0 ); 
	Reg &= ~( BCHP_MASK( XPT_PSUB_PSUB0_STAT0, RUN ) );
	BXPT_PacketSub_P_WriteReg( hPSub, BCHP_XPT_PSUB_PSUB0_STAT0, Reg );

	Reg = BXPT_PacketSub_P_ReadReg( hPSub, BCHP_XPT_PSUB_PSUB0_CTRL0 ); 
	Reg &= ~( BCHP_MASK( XPT_PSUB_PSUB0_CTRL0, PSUB_ENABLE ) );
	BXPT_PacketSub_P_WriteReg( hPSub, BCHP_XPT_PSUB_PSUB0_CTRL0, Reg );

	BKNI_Sleep( 1 );
																						  
	/* Clear the first desc addr (for cleaner debugging) */
	Reg = BXPT_PacketSub_P_ReadReg( hPSub, BCHP_XPT_PSUB_PSUB0_CTRL2 );
	Reg &= ~( BCHP_MASK( XPT_PSUB_PSUB0_CTRL2, FIRST_DESC_ADDR ) );
	BXPT_PacketSub_P_WriteReg( hPSub, BCHP_XPT_PSUB_PSUB0_CTRL2, Reg );

    WaitCount = 100;
    do
    {
	    Reg = BXPT_PacketSub_P_ReadReg( hPSub, BCHP_XPT_PSUB_PSUB0_STAT0 );
	    ChanBusy = BCHP_GET_FIELD_DATA( Reg, XPT_PSUB_PSUB0_STAT0, BUSY );
	    if( ChanBusy )
	    {
            WaitCount--;
            if( !WaitCount )
            {
		        BDBG_ERR(("Busy is still set when Packet Sub chan %d has been stopped!", 
			        ( unsigned long ) hPSub->ChannelNo ));
                return BERR_TRACE( BERR_TIMEOUT );
            }
            
            BKNI_Sleep( 1 );
	    }
    }
    while( ChanBusy );

	hPSub->LastDescriptor_Cached = NULL;

#ifdef BCHP_PWR_RESOURCE_XPT_PACKETSUB
	if (hPSub->Running==true) 
	{
		BCHP_PWR_ReleaseResource(hPSub->hChip, BCHP_PWR_RESOURCE_XPT_PACKETSUB);
	}
#endif

	hPSub->Running = false;

	return( ExitCode );
}

BERR_Code BXPT_PacketSub_GetChannelStatus(
	BXPT_PacketSub_Handle hPSub,			/* [in] Handle for the channel. */
	BXPT_PacketSub_ChannelStatus *Status 	/* [out] Channel status. */
	)
{
	uint32_t Reg;

	BERR_Code ExitCode = BERR_SUCCESS;

	BDBG_ASSERT( hPSub );
	BDBG_ASSERT( Status );

	Reg = BXPT_PacketSub_P_ReadReg( hPSub, BCHP_XPT_PSUB_PSUB0_STAT0 );
	Status->Finished = BCHP_GET_FIELD_DATA( Reg, XPT_PSUB_PSUB0_STAT0, FINISHED ) ? true : false;
	Status->Busy = BCHP_GET_FIELD_DATA( Reg, XPT_PSUB_PSUB0_STAT0, BUSY ) ? true : false;
	Status->Run = BCHP_GET_FIELD_DATA( Reg, XPT_PSUB_PSUB0_STAT0, RUN ) ? true : false;
	
	return( ExitCode ); 
}

void BXPT_PacketSub_P_WriteReg(
	BXPT_PacketSub_Handle hPSub,	/* [in] Handle for the channel. */
	uint32_t Reg0Addr,
	uint32_t RegVal
	)
{
 	/* 
 	** The address is the offset of the register from the beginning of the 
 	** block, plus the base address of the block ( which changes from 
 	** channel to channel ). 
 	*/
 	uint32_t RegAddr = Reg0Addr - BCHP_XPT_PSUB_PSUB0_CTRL0 + hPSub->BaseAddr;

	BREG_Write32( hPSub->hRegister, RegAddr, RegVal );
}		


uint32_t BXPT_PacketSub_P_ReadReg(
	BXPT_PacketSub_Handle hPSub,	/* [in] Handle for the channel. */
	uint32_t Reg0Addr
	)
{
 	/* 
 	** The address is the offset of the register from the beginning of the 
 	** block, plus the base address of the block ( which changes from 
 	** channel to channel ). 
 	*/
 	uint32_t RegAddr = Reg0Addr - BCHP_XPT_PSUB_PSUB0_CTRL0 + hPSub->BaseAddr;

	return( BREG_Read32( hPSub->hRegister, RegAddr ));
}

BERR_Code BXPT_PacketSub_GetEobIntId(
	BXPT_PacketSub_Handle hPSub,			/* [in] Handle for the channel. */
    BINT_Id *IntId
    )
{
	BDBG_ASSERT( hPSub );
	BDBG_ASSERT( IntId );

    switch( hPSub->ChannelNo )
    {
        case 0: *IntId = BCHP_INT_ID_PSUB0_EOB_INT; break;    
        case 1: *IntId = BCHP_INT_ID_PSUB1_EOB_INT; break;    
        case 2: *IntId = BCHP_INT_ID_PSUB2_EOB_INT; break;
                
#ifdef BCHP_INT_ID_PSUB3_EOB_INT
        case 3: *IntId = BCHP_INT_ID_PSUB3_EOB_INT; break;    
#endif

#ifdef BCHP_INT_ID_PSUB4_EOB_INT
        case 4: *IntId = BCHP_INT_ID_PSUB4_EOB_INT; break;    
#endif

#ifdef BCHP_INT_ID_PSUB5_EOB_INT
        case 5: *IntId = BCHP_INT_ID_PSUB5_EOB_INT; break;    
#endif

#ifdef BCHP_INT_ID_PSUB6_EOB_INT
        case 6: *IntId = BCHP_INT_ID_PSUB6_EOB_INT; break;    
#endif

#ifdef BCHP_INT_ID_PSUB7_EOB_INT
        case 7: *IntId = BCHP_INT_ID_PSUB7_EOB_INT; break;    
#endif

        default: 
            return BERR_TRACE( BERR_INVALID_PARAMETER );    
    }

    return BERR_SUCCESS;
}

/* end of file */

