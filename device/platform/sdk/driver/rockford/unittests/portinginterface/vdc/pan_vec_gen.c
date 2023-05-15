/***************************************************************************
 *     Copyright (c) 2002-2007, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: pan_vec_gen.c $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 9/17/07 4:38p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/portinginterface/vdc/pan_vec_gen.c $
 * 
 * Hydra_Software_Devel/1   9/17/07 4:38p rpan
 * PR34857: Initial revision. Ported from xvd_vdc_test (09/17/2007).
 * 
 *
 ***************************************************************************/
#include "bstd.h"           /* standard types */
#include "bdbg.h"
#include "bkni.h"           /* kernel interface */
#include "bavc.h"
#include "test_util.h"

BDBG_MODULE(static_buffer);

/*
 * Test move state: for window movement test
 */
typedef enum TEST_EMoveState
{
	eMOVE_RIGHT = 0,
	eMOVE_LEFT,
	eMOVE_UP,
	eMOVE_DOWN
} TEST_EMoveState;

typedef struct TEST_PanVecGen
{
	TEST_EMoveState  eMoveState;
	int16_t          iXOffsetMax;
	int16_t          iYOffsetMax;
	int16_t          iLeft;  /* in 1/16th pixel unit */
	int16_t          iTop;   /* in 1/16th pixel unit */
} TEST_PanVecGen;

/* return 0 for success, -1 for failure */
int TEST_PanVecGen_Create( TEST_PanVecGen_Handle *phPanVecGen,
						   uint32_t ulSrcWidth,
						   uint32_t ulSrcHeight,
						   uint32_t ulDspWidth,
						   uint32_t ulDspHeight )
{
	TEST_PanVecGen  * pPanVecGen;

	BDBG_ASSERT(NULL != phPanVecGen);
	pPanVecGen = (TEST_PanVecGen *) BKNI_Malloc(sizeof(TEST_PanVecGen));
	if (NULL == pPanVecGen)
	{
		return -1;
	}

	pPanVecGen->eMoveState = eMOVE_RIGHT;
	pPanVecGen->iXOffsetMax = (ulSrcWidth  - ulDspWidth)  << (4-1);
	pPanVecGen->iYOffsetMax = (ulSrcHeight - ulDspHeight) << (4-1);
	pPanVecGen->iLeft = -pPanVecGen->iXOffsetMax;
	pPanVecGen->iTop  = -pPanVecGen->iYOffsetMax;

	*phPanVecGen = pPanVecGen;
	
	return 0;
}

extern bool g_bPausePanScan;

/* return 0 for success, -1 for failure */
void TEST_PanVecGen_GetPanVector(TEST_PanVecGen_Handle hPanVecGen,
								 BAVC_Polarity eFldPlrt,
								 int16_t * piPanTop, int16_t * piPanLeft)
{
	TEST_PanVecGen  * pPanVecGen = (TEST_PanVecGen *) hPanVecGen;

	BDBG_ASSERT(NULL != hPanVecGen);
	BDBG_ASSERT(NULL != piPanTop);
	BDBG_ASSERT(NULL != piPanLeft);
	
	switch(pPanVecGen->eMoveState)
	{
	case eMOVE_RIGHT:
		if(((eFldPlrt == BAVC_Polarity_eTopField) || (eFldPlrt == BAVC_Polarity_eFrame)) 
		   && !g_bPausePanScan )
			pPanVecGen->iLeft += 8; /* half pixel movement */
		if (pPanVecGen->iLeft > pPanVecGen->iXOffsetMax)
		{
			pPanVecGen->iLeft -= 8; /* recover */
			pPanVecGen->eMoveState = eMOVE_DOWN;
			pPanVecGen->iTop += 8; /* half pixel movement */
			BDBG_WRN(("source moves down from (0x%x, 0x%x)\n", pPanVecGen->iLeft, pPanVecGen->iTop));
		}
		break;
		
	case eMOVE_DOWN:
		if(((eFldPlrt == BAVC_Polarity_eTopField) || (eFldPlrt == BAVC_Polarity_eFrame)) 
		   && !g_bPausePanScan)
			pPanVecGen->iTop += 8; /* half pixel movement */
		if ((pPanVecGen->iTop) > pPanVecGen->iYOffsetMax)
		{
			pPanVecGen->iTop -= 8; /* recover */
			pPanVecGen->eMoveState = eMOVE_LEFT;
			pPanVecGen->iLeft -= 8; /* half pixel movement */
			BDBG_WRN(("source moves left from (0x%x, 0x%x)\n", pPanVecGen->iLeft, pPanVecGen->iTop));
		}
		break;
		
	case eMOVE_LEFT:
		if(((eFldPlrt == BAVC_Polarity_eTopField) || (eFldPlrt == BAVC_Polarity_eFrame)) 
		   && !g_bPausePanScan)
			pPanVecGen->iLeft -= 8; /* half pixel movement */
		if ((pPanVecGen->iLeft) < (-pPanVecGen->iXOffsetMax))
		{
			pPanVecGen->iLeft += 8; /* recover */
			pPanVecGen->eMoveState = eMOVE_UP;
			pPanVecGen->iTop -= 8; /* half pixel movement */
			BDBG_WRN(("source moves up from (0x%x, 0x%x)\n", pPanVecGen->iLeft, pPanVecGen->iTop));
		}
		break;
		
	case eMOVE_UP:
		if(((eFldPlrt == BAVC_Polarity_eTopField) || (eFldPlrt == BAVC_Polarity_eFrame)) 
		   && !g_bPausePanScan)
		{
			pPanVecGen->iTop -= 8; /* half pixel movement */
		}
		if ((pPanVecGen->iTop) < (-pPanVecGen->iYOffsetMax))
		{
			pPanVecGen->iTop += 8; /* recover */
			pPanVecGen->eMoveState = eMOVE_RIGHT;
			pPanVecGen->iLeft += 8; /* half pixel movement */
			BDBG_WRN(("source moves right from (0x%x, 0x%x)\n", pPanVecGen->iLeft, pPanVecGen->iTop));
		}
		break;
		
	default:
		BDBG_ERR(("wrong move state"));
		break;
	}

	*piPanTop  = pPanVecGen->iTop;
	*piPanLeft = pPanVecGen->iLeft;
}


/* end of file */
