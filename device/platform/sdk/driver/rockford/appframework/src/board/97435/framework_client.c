/***************************************************************************
*     Copyright (c) 2003-2010, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: framework_client.c $
*
* Module Description: Framework initialization header file
*
* Revision History:
*
* $brcm_Log: /rockford/appframework/src/board/97422/framework_client.c $
* 
* Hydra_Software_Devel/3   10/15/10 3:10p hongtaoz
* SW7425-1, SW7425-10: added MEMC1 support to emulation framwork;
* 
* Hydra_Software_Devel/2   9/28/10 2:56p hongtaoz
* SW7425-10: adding generic interrupt polling support for all PI modules;
* 
* Hydra_Software_Devel/1   6/22/10 4:01p vanessah
* SW7422-12:  add source control framework_board.c
* 
* 
***************************************************************************/

/* magnum includes */
#include "bstd.h"
#include "bkni.h"
#include "bchp.h"
#include "btst_kni.h"

/* framework includes */
#include "framework_board.h"

/* chip specifics */
#include "platform.h"

/* system specific */
#include "client.h"

BDBG_MODULE( FRAMEWORK );

#define TestError(e, s) {\
	eStatus = e;\
	if (eStatus != BERR_SUCCESS)\
	{\
		BDBG_ERR(( s, __FILE__, __LINE__ ));\
		goto Error;\
	}\
}


/***************************************************************************/
int BFrmWorkBoard_Init(
	const BSystem_Info *pSysInfo,
	BFramework_Info    *pFrameworkInfo )
{
	BERR_Code             eStatus = BERR_SUCCESS;
	BFrmWorkBoard_Info   *pFrmWorkBoardInfo;
	BMEM_Heap_Settings    mem_heap_settings;

	BSTD_UNUSED(pSysInfo);

	/* allocate board specific frame work context */
	pFrmWorkBoardInfo = (BFrmWorkBoard_Info *)(BKNI_Malloc(sizeof(BFrmWorkBoard_Info)));
	if(NULL == pFrmWorkBoardInfo)
	{
		return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
	}
	
	BKNI_Memset(pFrmWorkBoardInfo, 0x0, sizeof(BFrmWorkBoard_Info));
	pFrameworkInfo->hFrmWorkBoard = pFrmWorkBoardInfo;

	/* Memory heap for 16-bit DDR_1: 1-on-1 mapping, 256-byte aligned */
	mem_heap_settings.eSafetyConfig = BMEM_CONFIG_FASTEST;
	mem_heap_settings.eBookKeeping  = BMEM_BOOKKEEPING_SYSTEM;
	mem_heap_settings.uiAlignment = 8;
	mem_heap_settings.pCachedAddress = NULL;

	eStatus = BMEM_Heap_Create(pFrameworkInfo->hMemModule,
		pSysInfo->bmemAddress1, pSysInfo->bmemOffset1, pSysInfo->bmemSize1, &mem_heap_settings, 
		&pFrmWorkBoardInfo->hMemc1);
	return eStatus;
}

void BFrmWorkBoard_Uninit( BFramework_Info *pFrameworkInfo )
{
	BFrmWorkBoard_Info  *pFrmWorkBoardInfo;
	pFrmWorkBoardInfo = pFrameworkInfo->hFrmWorkBoard;
	if (NULL == pFrmWorkBoardInfo )
		return;
	if ( pFrmWorkBoardInfo->hMemc1)
	{
		BMEM_Heap_Destroy(pFrmWorkBoardInfo->hMemc1);
	}
	BKNI_Free( pFrameworkInfo->hFrmWorkBoard );
	pFrameworkInfo->hFrmWorkBoard = NULL;
}

/* End of file */
