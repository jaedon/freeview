/***************************************************************************
 *     Copyright (c) 2003-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxpt_directv_remux.c $
 * $brcm_Revision: Hydra_Software_Devel/12 $
 * $brcm_Date: 1/18/11 11:36a $
 *
 * Porting interface code for the data transport core. 
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/xpt/7400/bxpt_directv_remux.c $
 * 
 * Hydra_Software_Devel/12   1/18/11 11:36a rjlewis
 * SW7420-1383: Added support for BDBG_OBJECT interface
 * 
 * Hydra_Software_Devel/11   9/9/09 8:06a piyushg
 * SW7630-30: Add 7630 XPT PI support.
 * Added directory element "7630".
 * 
 * Hydra_Software_Devel/10   8/11/09 10:39a piyushg
 * PR55216: Added initial 7340 XPT support.
 * 
 * Hydra_Software_Devel/9   8/5/09 4:52p piyushg
 * PR55545: Add 7125 XPT PI support
 * Added file element "bxpt_rave_ihex.c".
 * Added file element "bxpt_rave_ihex.h".
 * 
 * Hydra_Software_Devel/PR55545/1   7/24/09 10:47a mward
 * PR55548: Adapted for 7125 based on 7342, 7340.
 * 
 * Hydra_Software_Devel/8   7/14/09 10:21a piyushg
 * PR56771: Add XPT PI code for 7342.
 * 
 * Hydra_Software_Devel/7   4/7/09 5:25p piyushg
 * PR52986: Add support for 7635
 * Added directory element "7635".
 * 
 * Hydra_Software_Devel/6   1/27/09 1:09p gmullen
 * PR51625: Added 7336 support
 * 
 * Hydra_Software_Devel/5   11/26/08 4:14p gmullen
 * PR47755: Added support for 7420.
 * 
 * Hydra_Software_Devel/4   3/26/08 11:23a gmullen
 * PR38954: Added 3548 support to XPT PI.
 * 
 * Hydra_Software_Devel/3   11/28/07 11:15a gmullen
 * PR36900: Added 7335 support
 * 
 * Hydra_Software_Devel/2   10/30/07 3:11p gmullen
 * PR35018: Added support for 7325
 * 
 * Hydra_Software_Devel/2   12/2/05 3:23p gmullen
 * PR15309: Saved updates for ccase server move
 * 
 * Hydra_Software_Devel/1   7/28/05 3:40p gmullen
 * PR15309: Initial version for building.
 * 
 *
 ***************************************************************************/

#include "bstd.h"
#include "bxpt_priv.h"
#include "bxpt_directv_remux.h"	 
#include "bxpt_remux_private.h"
#include "bchp_xpt_rmx0.h"

BDBG_MODULE( xpt_directv_remux );

BERR_Code BXPT_DirecTvRemux_SetMode( 
	BXPT_Remux_Handle hRmx,		/* [in] Handle for the remux channel */
	BXPT_RemuxMode Mode			/* [in] Selects the mode. */
	)
{
 	uint32_t Reg;

	BERR_Code ExitCode = BERR_SUCCESS;

	BDBG_OBJECT_ASSERT(hRmx, bxpt_t);

#if ( BCHP_CHIP == 7325 ) || ( BCHP_CHIP == 7335 ) || ( BCHP_CHIP == 7336  ) || ( BCHP_CHIP == 3548 ) || ( BCHP_CHIP == 3556 ) || \
    ( BCHP_CHIP == 7420 ) || ( BCHP_CHIP == 7635 ) || ( BCHP_CHIP == 7342 )  || ( BCHP_CHIP == 7125) || ( BCHP_CHIP == 7340 ) || \
	( BCHP_CHIP == 7630 )
	Reg = BXPT_Remux_P_ReadReg( hRmx, BCHP_XPT_RMX0_CTRL );
	Reg &= ~(
		BCHP_MASK( XPT_RMX0_CTRL, RMX_PKT_LENGTH ) |
		BCHP_MASK( XPT_RMX0_CTRL, RMX_PKT_MODE )
	);

	if( Mode == BXPT_RemuxMode_eDirecTv )
	{
		Reg |= (
			BCHP_FIELD_DATA( XPT_RMX0_CTRL, RMX_PKT_LENGTH, 130 ) |
			BCHP_FIELD_DATA( XPT_RMX0_CTRL, RMX_PKT_MODE, 1 )
		);
	}
	else
	{
		Reg |= (
			BCHP_FIELD_DATA( XPT_RMX0_CTRL, RMX_PKT_LENGTH, 188 ) |
			BCHP_FIELD_DATA( XPT_RMX0_CTRL, RMX_PKT_MODE, 0 )
		);
	}

	BXPT_Remux_P_WriteReg( hRmx, BCHP_XPT_RMX0_CTRL, Reg );
#else
	Reg = BXPT_Remux_P_ReadReg( hRmx, BCHP_XPT_RMX0_FORMAT );
	Reg &= ~(
		BCHP_MASK( XPT_RMX0_FORMAT, RMX_PKT_LENGTH ) |
		BCHP_MASK( XPT_RMX0_FORMAT, RMX_PKT_MODE )
	);

	if( Mode == BXPT_RemuxMode_eDirecTv )
	{
		Reg |= (
			BCHP_FIELD_DATA( XPT_RMX0_FORMAT, RMX_PKT_LENGTH, 130 ) |
			BCHP_FIELD_DATA( XPT_RMX0_FORMAT, RMX_PKT_MODE, 1 )
		);
	}
	else
	{
		Reg |= (
			BCHP_FIELD_DATA( XPT_RMX0_FORMAT, RMX_PKT_LENGTH, 188 ) |
			BCHP_FIELD_DATA( XPT_RMX0_FORMAT, RMX_PKT_MODE, 0 )
		);
	}

	BXPT_Remux_P_WriteReg( hRmx, BCHP_XPT_RMX0_FORMAT, Reg );
#endif
	return( ExitCode );	    
}

#if ( BCHP_CHIP == 7325 ) || ( BCHP_CHIP == 7335 ) || ( BCHP_CHIP == 7336  ) || ( BCHP_CHIP == 3548 ) || ( BCHP_CHIP == 3556 ) || \
    ( BCHP_CHIP == 7420 ) || ( BCHP_CHIP == 7635 ) || ( BCHP_CHIP == 7342 )  || ( BCHP_CHIP == 7125) || ( BCHP_CHIP == 7340 ) || \
	( BCHP_CHIP == 7630 )
    /* These chips don't have packet sub in the remux. The dedicated PSUB module is used instead */
#else
BERR_Code BXPT_Remux_PsubMatchHdField( 
	BXPT_Remux_Handle hRmx,	/* [in] Handle for the remux channel */
	int WhichTable,		/* [in] The remux packet sub to set. */
	bool MatchHd, 		  /* [in] Enable or disable HD match requirement */
	uint8_t HdCompValue	  /* [in] Value HD field must match. */
	)
{
	BERR_Code ExitCode = BERR_SUCCESS;

	BDBG_OBJECT_ASSERT(hRmx, bxpt_t);

	if( WhichTable >= BXPT_P_MAX_REMUX_PSUB_TABLES )
	{
		BDBG_ERR(( "WhichTable %lu is out of range!", ( unsigned long ) WhichTable ));
		ExitCode = BERR_TRACE(BERR_INVALID_PARAMETER);
	}
	else
	{
		uint32_t Reg;
		
		uint32_t CtrlRegAddr = 0;

		switch( WhichTable )
		{
			case 0:
			CtrlRegAddr = BCHP_XPT_RMX0_PSUB1_CTRL;
			break;

			case 1:
			CtrlRegAddr = BCHP_XPT_RMX0_PSUB2_CTRL;
			break;
		}

		Reg = BXPT_Remux_P_ReadReg( hRmx, CtrlRegAddr );
		Reg &= ~(
			BCHP_MASK( XPT_RMX0_PSUB1_CTRL, RMX_PSUB1_HD_EN ) |
			BCHP_MASK( XPT_RMX0_PSUB1_CTRL, RMX_PSUB1_HD )
		);
		Reg |= (
			BCHP_FIELD_DATA( XPT_RMX0_PSUB1_CTRL, RMX_PSUB1_HD_EN, MatchHd == true ? 1 : 0 ) |
			BCHP_FIELD_DATA( XPT_RMX0_PSUB1_CTRL, RMX_PSUB1_HD, HdCompValue )
		);
		BXPT_Remux_P_WriteReg( hRmx, CtrlRegAddr, Reg );
	}

	return( ExitCode );	    
}
#endif
