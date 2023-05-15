/***************************************************************************
 *     Copyright (c) 2003-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxpt_directv_pcroffset.c $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 10/28/10 2:08p $
 *
 * Porting interface code for the data transport core. 
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/xpt/base2/bxpt_directv_pcroffset.c $
 * 
 * Hydra_Software_Devel/2   10/28/10 2:08p gmullen
 * SW7422-20: Checkin ported files
 * 
 * Hydra_Software_Devel/15   1/25/10 11:54a gmullen
 * SW7408-64: Added DirecTV support
 * 
 * Hydra_Software_Devel/14   11/23/09 5:14p gmullen
 * SW7468-13: Updated PI and unit test
 * 
 * Hydra_Software_Devel/13   9/9/09 8:05a piyushg
 * SW7630-30: Add 7630 XPT PI support.
 * Added directory element "7630".
 * 
 * Hydra_Software_Devel/12   8/11/09 10:39a piyushg
 * PR55216: Added initial 7340 XPT support.
 * 
 * Hydra_Software_Devel/11   8/5/09 4:51p piyushg
 * PR55545: Add 7125 XPT PI support
 * Added file element "bxpt_rave_ihex.c".
 * Added file element "bxpt_rave_ihex.h".
 * 
 * Hydra_Software_Devel/PR55545/1   7/24/09 10:46a mward
 * PR55548: Adapted for 7125 based on 7342, 7340.
 * 
 * Hydra_Software_Devel/10   7/14/09 10:21a piyushg
 * PR56771: Add XPT PI code for 7342.
 * 
 * Hydra_Software_Devel/9   4/7/09 5:25p piyushg
 * PR52986: Add support for 7635
 * Added directory element "7635".
 * 
 * Hydra_Software_Devel/8   1/27/09 1:09p gmullen
 * PR51625: Added 7336 support
 * 
 * Hydra_Software_Devel/7   11/26/08 4:14p gmullen
 * PR47755: Added support for 7420.
 * 
 * Hydra_Software_Devel/6   3/26/08 11:23a gmullen
 * PR38954: Added 3548 support to XPT PI.
 * 
 * Hydra_Software_Devel/5   11/28/07 11:07a gmullen
 * PR36900: Added 7335 support.
 * 
 * Hydra_Software_Devel/4   10/30/07 3:11p gmullen
 * PR35018: Added support for 7325
 * 
 * Hydra_Software_Devel/3   2/14/07 1:10p gmullen
 * PR27642: Added support for 7405.
 * 
 * Hydra_Software_Devel/2   2/6/07 7:05p gmullen
 * PR26217: Added B0 support.
 * 
 * Hydra_Software_Devel/1   1/16/06 2:08p gmullen
 * PR18998: Initial checkin for 7400
 * 
 * Hydra_Software_Devel/1   9/6/05 5:12p gmullen
 * PR15309: Added support for DirecTV in PCR offset block.
 * 
 * 
 *
 ***************************************************************************/

#include "bstd.h"
#include "bxpt_priv.h"
#include "bxpt_directv_pcroffset.h"

#if( BDBG_DEBUG_BUILD == 1 )
BDBG_MODULE( xpt_directv_pcroffset );
#endif

#include "bchp_xpt_pcroffset.h"

#define DIRECTV_CTRL_OFFSET			( BCHP_XPT_PCROFFSET_CONTEXT0_PCROFFSET_CTRL - BCHP_XPT_PCROFFSET_CONTEXT0_PCROFFSET_CTRL )

BERR_Code BXPT_DirecTv_PcrOffset_SetPcrMode( 
	BXPT_PcrOffset_Handle hChannel,		 /* [in] The channel handle */
	BXPT_PcrMode Mode
	)
{
	uint32_t Reg;

	BERR_Code ExitCode = BERR_SUCCESS;

	BDBG_ASSERT( hChannel );

	Reg = BREG_Read32( hChannel->hReg, hChannel->BaseAddr + DIRECTV_CTRL_OFFSET );
	Reg &= ~( 
   		BCHP_MASK( XPT_PCROFFSET_CONTEXT0_PCROFFSET_CTRL, PACKET_MODE ) 
	);
	Reg |= (
		BCHP_FIELD_DATA( XPT_PCROFFSET_CONTEXT0_PCROFFSET_CTRL, PACKET_MODE, Mode == BXPT_PcrMode_eDirecTv ? 1 : 0 )
	);
	BREG_Write32( hChannel->hReg, hChannel->BaseAddr + DIRECTV_CTRL_OFFSET, Reg );

	return( ExitCode );
}

