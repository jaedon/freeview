/***************************************************************************
 *     Copyright (c) 2006-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: apetest_board.c $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 7/10/12 9:25a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/portinginterface/ape/97422/common/apetest_board.c $
 * 
 * Hydra_Software_Devel/1   7/10/12 9:25a jgarrett
 * SW7425-3457: Adding ape_decode application
 *
 ***************************************************************************/
 
/**************************** Include Files**********************************/ 
#include "apetest_common.h"

BDBG_MODULE(apetest_board);


/* ------------------------ Global Variables -------------------------------- */
/* used by bgfx_image.c, ... ? */
BCHP_Handle                     g_hChip      = NULL;
BREG_Handle                     g_hRegister  = NULL;
BMEM_Handle                     g_hMemory    = NULL;
BINT_Handle                     g_hInterrupt = NULL;
BTMR_Handle                     g_hTmr       = NULL;



BERR_Code APETEST_Board_Init(int argc, char **argv, APETEST_BoardInfo * boardInfo)
{

#if BDBG_DEBUG_BUILD
    APETEST_InitModuleDebug();
#endif

	RET_ERR( BSystem_Init( argc, argv, &boardInfo->sysInfo ) );
	RET_ERR( BFramework_Init( &boardInfo->sysInfo , &boardInfo->frmInfo ) );

	/* used by bgfx_image.c, ...*/
	g_hChip      = boardInfo->frmInfo.hChp;
	g_hRegister  = boardInfo->frmInfo.hReg;
	g_hMemory    = boardInfo->frmInfo.hMem;
	g_hInterrupt = boardInfo->frmInfo.hInt;
	g_hTmr       = boardInfo->frmInfo.hTmr;

    return BERR_SUCCESS;
}


BERR_Code APETEST_Board_UnInit(APETEST_BoardInfo * boardInfo)
{

    /* terminate */
	BFramework_Uninit(&boardInfo->frmInfo );
	BSystem_Uninit(&boardInfo->sysInfo);

    return BERR_SUCCESS;
}
