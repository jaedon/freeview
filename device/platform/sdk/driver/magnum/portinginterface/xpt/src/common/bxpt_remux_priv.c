/***************************************************************************
 *     Copyright (c) 2003-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxpt_remux_priv.c $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 10/28/10 2:08p $
 *
 * Implemtation of the remux module private functions.
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/xpt/base2/bxpt_remux_priv.c $
 * 
 * Hydra_Software_Devel/2   10/28/10 2:08p gmullen
 * SW7422-20: Checkin ported files
 * 
 * Hydra_Software_Devel/9   9/9/09 8:07a piyushg
 * SW7630-30: Add 7630 XPT PI support.
 * Added directory element "7630".
 * 
 * Hydra_Software_Devel/8   8/11/09 10:40a piyushg
 * PR55216: Added initial 7340 XPT support.
 * 
 * Hydra_Software_Devel/7   8/5/09 4:52p piyushg
 * PR55545: Add 7125 XPT PI support
 * Added file element "bxpt_rave_ihex.c".
 * Added file element "bxpt_rave_ihex.h".
 * 
 * Hydra_Software_Devel/PR55545/1   7/24/09 10:50a mward
 * PR55548: Adapted for 7125 based on 7342, 7340.
 * 
 * Hydra_Software_Devel/6   7/14/09 10:22a piyushg
 * PR56771: Add XPT PI code for 7342.
 * 
 * Hydra_Software_Devel/5   4/7/09 5:26p piyushg
 * PR52986: Add support for 7635
 * Added directory element "7635".
 * 
 * Hydra_Software_Devel/4   1/27/09 1:10p gmullen
 * PR51625: Added 7336 support
 * 
 * Hydra_Software_Devel/3   1/12/09 4:43p gmullen
 * PR50362: Fixed addressing bug. Some parts have the remux IO registers
 * outside of the remux block.
 * 
 * Hydra_Software_Devel/2   11/2/06 8:40a gmullen
 * PR23189: Added 3563 support.
 * 
 * Hydra_Software_Devel/1   11/8/05 10:06a gmullen
 * PR15309: Added private functions for remux.
 * 
 *
 ***************************************************************************/

#include "bstd.h"
#include "bxpt_remux_priv.h"
#include "bdbg.h"
#include "bkni.h"
#include "bxpt_priv.h"

#include "bchp_xpt_rmx0_io.h"
#include "bchp_xpt_rmx1_io.h"

#include "bchp_xpt_rmx0.h"

static uint32_t ComputeRegAddress(
	BXPT_Remux_Handle hRmx,			/* [in] Handle for the remux channel */
	uint32_t Reg0Addr
    )
{
    uint32_t RegAddr;

    switch( Reg0Addr )
    {
        case BCHP_XPT_RMX0_IO_FORMAT:
        if( hRmx->ChannelNo )
            RegAddr = BCHP_XPT_RMX1_IO_FORMAT;
        else
            RegAddr = BCHP_XPT_RMX0_IO_FORMAT;
        break;

        case BCHP_XPT_RMX0_IO_PKT_DLY_CNT:
        if( hRmx->ChannelNo )
            RegAddr = BCHP_XPT_RMX1_IO_PKT_DLY_CNT;
        else
            RegAddr = BCHP_XPT_RMX0_IO_PKT_DLY_CNT;
        break;

        default:
        RegAddr = Reg0Addr - BCHP_XPT_RMX0_CTRL + hRmx->BaseAddr;
        break;
    }
    
    return RegAddr;
}

void BXPT_Remux_P_WriteReg(
	BXPT_Remux_Handle hRmx,			/* [in] Handle for the remux channel */
	uint32_t Reg0Addr,
	uint32_t RegVal
	)
{
	BREG_Write32( hRmx->hRegister, ComputeRegAddress( hRmx, Reg0Addr ), RegVal );
}		

uint32_t BXPT_Remux_P_ReadReg(
	BXPT_Remux_Handle hRmx,			/* [in] Handle for the remux channel */
	uint32_t Reg0Addr
	)
{
	return( BREG_Read32( hRmx->hRegister, ComputeRegAddress( hRmx, Reg0Addr ) ));
}




