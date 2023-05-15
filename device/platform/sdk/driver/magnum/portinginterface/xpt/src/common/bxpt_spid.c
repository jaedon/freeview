/***************************************************************************
 *     Copyright (c) 2003-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxpt_spid.c $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 10/28/10 2:08p $
 *
 * Porting interface code for the data transport core. 
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/xpt/base2/bxpt_spid.c $
 * 
 * Hydra_Software_Devel/2   10/28/10 2:08p gmullen
 * SW7422-20: Checkin ported files
 * 
 * Hydra_Software_Devel/14   8/30/10 5:24p gmullen
 * SW7403-924: Protected access to the PID and SPID table from other
 * modules in the PI
 * 
 * Hydra_Software_Devel/13   6/17/09 6:46p gmullen
 * PR56110: Added support.xpt/7550/uif_image/a0
 * 
 * Hydra_Software_Devel/12   4/7/09 5:26p piyushg
 * PR52986: Add support for 7635
 * Added directory element "7635".
 * 
 * Hydra_Software_Devel/11   1/27/09 1:10p gmullen
 * PR51625: Added 7336 support
 * 
 * Hydra_Software_Devel/10   11/26/08 4:15p gmullen
 * PR47755: Added support for 7420.
 * 
 * Hydra_Software_Devel/9   3/26/08 11:24a gmullen
 * PR38954: Added 3548 support to XPT PI.
 * 
 * Hydra_Software_Devel/8   11/28/07 12:14p gmullen
 * PR36900: Added 7335 support
 * 
 * Hydra_Software_Devel/7   10/30/07 3:12p gmullen
 * PR35018: Added support for 7325
 * 
 * Hydra_Software_Devel/6   2/14/07 1:10p gmullen
 * PR27642: Added support for 7405.
 * 
 * Hydra_Software_Devel/5   2/6/07 7:05p gmullen
 * PR26217: Added B0 support.
 * 
 * Hydra_Software_Devel/4   11/21/06 3:59p gmullen
 * PR26109: Ported files to 7403.
 * 
 * Hydra_Software_Devel/3   8/17/06 6:01p katrep
 * PR23114: Added Support for 7440 chip
 * 
 * Hydra_Software_Devel/2   3/31/06 4:21p gmullen
 * PR 18998: Finished implementation.
 * 
 * Hydra_Software_Devel/1   7/28/05 3:40p gmullen
 * PR15309: Initial version for building.
 * 
 *
 ***************************************************************************/

#include "bstd.h"
#include "bxpt_priv.h"
#include "bxpt_spid.h"
#include "bchp_xpt_fe.h"

#include "bxpt.h"

#if( BDBG_DEBUG_BUILD == 1 )
BDBG_MODULE( xpt_spid );
#endif

BERR_Code BXPT_Spid_ConfigureChannel(
	BXPT_Handle hXpt, 	   			/* [in] Handle for this transport */
	unsigned int ChannelNum, 	/* [in] Which secondary channel to configure. */
    unsigned int Spid, 				/* [in] Secondary PID to use. */
	BXPT_Spid_eChannelMode Mode 	/* [in] Mode for secondary PIDs on this channel. */
	)
{
	uint32_t Reg, RegAddr;

	BERR_Code ExitCode = BERR_SUCCESS;

	BDBG_ASSERT( hXpt );

	/* Sanity check on the arguments. */
	if( Spid >= 0x2000 )
	{								  
		/* Bad PID. Complain. */
		BDBG_ERR(( "Spid %lu is out of range!", ( unsigned long ) Spid ));
		ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
	}
	else if( ChannelNum >= hXpt->MaxPidChannels )
	{
		/* Bad PID channel number. Complain. */
		BDBG_ERR(( "PidChannelNum %lu is out of range!", ( unsigned long ) ChannelNum ));
		ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
	}
	else							 		  
	{
		RegAddr = BCHP_XPT_FE_SPID_TABLE_i_ARRAY_BASE + ( 4 * ChannelNum );
		Reg = BREG_Read32( hXpt->hRegister, RegAddr );

		Reg &= ~( 
			BCHP_MASK( XPT_FE_SPID_TABLE_i, SPID_MODE ) |
            BCHP_MASK( XPT_FE_SPID_TABLE_i, PID_FUNCTIONS_SPID_CHANNEL_PID ) 
			);

		Reg |= ( 
			BCHP_FIELD_DATA( XPT_FE_SPID_TABLE_i, SPID_MODE, Mode ) |
			BCHP_FIELD_DATA( XPT_FE_SPID_TABLE_i, PID_FUNCTIONS_SPID_CHANNEL_PID, Spid )
			);
		BREG_Write32( hXpt->hRegister, RegAddr, Reg );
	}

	return( ExitCode );
}


BERR_Code BXPT_Spid_P_ConfigureChannelFilter(
	BXPT_Handle hXpt, 	   			/* [in] Handle for this transport */
	unsigned int ChannelNum,    	/* [in] Which secondary channel to configure. */
	BXPT_Spid_eChannelFilter Filter /* [in] Set a filter to use with this channel */
	)
{
	uint32_t Reg, RegAddr,i;

	BERR_Code ExitCode = BERR_SUCCESS;

	BDBG_ASSERT( hXpt );

	if( ChannelNum >= hXpt->MaxPidChannels )
	{
		/* Bad PID channel number. Complain. */
		BDBG_ERR(( "PidChannelNum %lu is out of range!", ( unsigned long ) ChannelNum ));
		ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
	}
	else							 		  
	{
		RegAddr = BCHP_XPT_FE_SPID_TABLE_i_ARRAY_BASE + ( 4 * ChannelNum );
		Reg = BREG_Read32( hXpt->hRegister, RegAddr );

        i = BCHP_GET_FIELD_DATA( Reg, XPT_FE_SPID_TABLE_i, SPID_MODE );

        /* throw an error if SPID channel is being used for SPID functions */
        if(i > BXPT_Spid_eChannelMode_Disable && i <= BXPT_Spid_eChannelMode_Remap)
        {
            BDBG_ERR(("PidChannelNum %lu already in use for SPID functions!",( unsigned long ) ChannelNum ));
            ExitCode = BERR_TRACE( BERR_UNKNOWN );
        }
		Reg &= ~( 
			BCHP_MASK( XPT_FE_SPID_TABLE_i, SPID_MODE ) |
			BCHP_MASK( XPT_FE_SPID_TABLE_i, STREAM_ID_RANGE_STREAM_ID_HI ) |
            BCHP_MASK( XPT_FE_SPID_TABLE_i, STREAM_ID_RANGE_STREAM_ID_LO )
			);

		Reg |= BCHP_FIELD_DATA( XPT_FE_SPID_TABLE_i, SPID_MODE, Filter.Mode );

        switch(Filter.Mode)
        {
        case BXPT_Spid_eChannelFilterMode_StreamId:
            Reg |= BCHP_FIELD_DATA( XPT_FE_SPID_TABLE_i, STREAM_ID_FILTER_STREAM_ID, Filter.FilterConfig.StreamId );
            break;
        case BXPT_Spid_eChannelFilterMode_StreamIdRange:
            Reg |= ( BCHP_FIELD_DATA( XPT_FE_SPID_TABLE_i, STREAM_ID_RANGE_STREAM_ID_HI, Filter.FilterConfig.StreamIdRange.Hi ) |
                     BCHP_FIELD_DATA( XPT_FE_SPID_TABLE_i, STREAM_ID_RANGE_STREAM_ID_LO, Filter.FilterConfig.StreamIdRange.Lo )
                     );
            break;
        case BXPT_Spid_eChannelFilterMode_StreamIdExtension:
            Reg |= ( BCHP_FIELD_DATA( XPT_FE_SPID_TABLE_i, STREAM_ID_EXTENSION_FILTER_STREAM_ID, Filter.FilterConfig.StreamIdAndExtension.Id ) |
                     BCHP_FIELD_DATA( XPT_FE_SPID_TABLE_i, STREAM_ID_EXTENSION_FILTER_STREAM_ID_EXTENSION, Filter.FilterConfig.StreamIdAndExtension.Extension )
                     );
            break;
        case BXPT_Spid_eChannelFilterMode_SubStreamId:
            Reg |= ( BCHP_FIELD_DATA( XPT_FE_SPID_TABLE_i, SUBSTREAM_ID_FILTER_STREAM_ID, Filter.FilterConfig.StreamIdAndSubStreamId.Id ) |
                     BCHP_FIELD_DATA( XPT_FE_SPID_TABLE_i, SUBSTREAM_ID_FILTER_SUBSTREAM_ID, Filter.FilterConfig.StreamIdAndSubStreamId.SubStreamId )
                     );
            break;
        case BXPT_Spid_eChannelFilterMode_Disable:
            break;
        default:
            BDBG_ERR(("Invalid filter mode %lu",Filter.Mode));
            ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
        }
        BREG_Write32( hXpt->hRegister, RegAddr, Reg );
	}

	return( ExitCode );
}





