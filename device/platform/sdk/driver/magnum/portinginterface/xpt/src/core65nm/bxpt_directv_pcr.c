/***************************************************************************
 *     Copyright (c) 2003-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxpt_directv_pcr.c $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 1/18/11 11:36a $
 *
 * Porting interface code for the data transport core. 
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/xpt/7400/bxpt_directv_pcr.c $
 * 
 * Hydra_Software_Devel/2   1/18/11 11:36a rjlewis
 * SW7420-1383: Added support for BDBG_OBJECT interface
 * 
 * Hydra_Software_Devel/2   9/6/05 5:12p gmullen
 * PR15309: Added support for DirecTV in PCR offset block.
 * 
 * Hydra_Software_Devel/1   7/28/05 3:39p gmullen
 * PR15309: Initial version for building.
 * 
 *
 ***************************************************************************/

#include "bstd.h"
#include "bxpt_priv.h"						   
#include "bxpt_directv_pcr.h"
#include "bchp_xpt_dpcr0.h"

BDBG_MODULE( xpt_directv_pcr );

BERR_Code BXPT_PCR_DirecTv_SetPcrMode( 
	BXPT_PCR_Handle hPcr, 			   /* [In]The Pcr handle */
	BXPT_PcrMode Mode
	)
{
	BREG_Handle			hRegister;
	uint32_t            Reg;
	BERR_Code		    ret = BERR_SUCCESS;
	uint32_t            Offset;

    BDBG_OBJECT_ASSERT(hPcr, bxpt_t);

	hRegister = hPcr->hRegister;
	Offset = hPcr->RegOffset;

	Reg = BREG_Read32( hRegister, BCHP_XPT_DPCR0_CTRL + Offset );
	Reg &= ~( BCHP_MASK( XPT_DPCR0_CTRL, PCR_PACKET_MODE ) );
	Reg |= BCHP_FIELD_DATA( XPT_DPCR0_CTRL, PCR_PACKET_MODE, Mode == BXPT_PcrMode_eDirecTv ? 1 : 0 );
	BREG_Write32( hRegister, BCHP_XPT_DPCR0_CTRL + Offset, Reg );

	hPcr->DMode = Mode == BXPT_PcrMode_eDirecTv ? true : false;
		
	return ret;
}

BERR_Code	BXPT_PCR_DirecTv_GetLastPcr( 
	BXPT_PCR_Handle hPcr, 			   /* [in]The Pcr handle */
	uint32_t *pPcr      			  /*[out] 32 bits of RTS*/
	)
{
	BREG_Handle			hRegister;
	uint32_t            RegHi, RegLo;
	BERR_Code		    ret = BERR_SUCCESS;
	uint32_t            Offset;

	BDBG_ENTER( BXPT_PCR_DirecTv_GetLastPcr );
    BDBG_OBJECT_ASSERT(hPcr, bxpt_t);

	hRegister = hPcr->hRegister;
	Offset = hPcr->RegOffset;

	RegHi = BREG_Read32( hRegister, BCHP_XPT_DPCR0_LAST_PCR_HI + Offset );
   	RegLo = BREG_Read32( hRegister, BCHP_XPT_DPCR0_LAST_PCR_LO + Offset );
	*pPcr = ( RegHi << 10 ) | RegLo;

	BDBG_LEAVE( BXPT_PCR_DirecTv_GetLastPcr );

	return ret;

}

BERR_Code	BXPT_PCR_DirecTv_GetStc( 
	BXPT_PCR_Handle hPcr,          /* [in]The Pcr handle */
	uint32_t *pStcHi               /*[out] 32 bits of RTS*/
	)
{
	BREG_Handle			hRegister;
	uint32_t            RegHi, RegLo;
	BERR_Code		    ret = BERR_SUCCESS;
	uint32_t            Offset;

	BDBG_ENTER( BXPT_PCR_DirecTv_GetStc );
    BDBG_OBJECT_ASSERT(hPcr, bxpt_t);

	hRegister = hPcr->hRegister;
	Offset = hPcr->RegOffset;

	RegHi = BREG_Read32( hRegister, BCHP_XPT_DPCR0_STC_HI + Offset );
	RegLo = BREG_Read32( hRegister, BCHP_XPT_DPCR0_STC_LO + Offset );
	*pStcHi = ( RegHi << 10 ) | RegLo;

	BDBG_LEAVE( BXPT_PCR_DirecTv_GetStc );

	return ret;
}


