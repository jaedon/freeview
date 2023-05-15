/***************************************************************************
 *     Copyright (c) 2007-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvdc_mud_priv.c $
 * $brcm_Revision: Hydra_Software_Devel/11 $
 * $brcm_Date: 1/12/11 4:35p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vdc/7038/bvdc_mud_priv.c $
 * 
 * Hydra_Software_Devel/11   1/12/11 4:35p yuxiaz
 * SW7335-1133: Added more debug messages for VDC heap debugging.
 * 
 * Hydra_Software_Devel/10   4/7/10 11:33a tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic.  Rename TLA
 *
 * Hydra_Software_Devel/9   4/5/10 4:11p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic
 *
 * Hydra_Software_Devel/8   1/25/08 8:32p albertl
 * PR36701:  Set mud buffer create to always use 2HD.  Mud buffers now
 * destroyed when display is closed.
 *
 * Hydra_Software_Devel/7   12/22/07 3:50a albertl
 * PR36701:  Fixed transitions between MUD supported formats.  Removed
 * debug printf.
 *
 * Hydra_Software_Devel/6   12/22/07 3:38a albertl
 * PR36701:  Integrated and rewrote andover mud code.  Fixed mud
 * transitional issues.
 *
 * Hydra_Software_Devel/dev_bdvd_pr6758/1   12/10/07 11:13a nprao
 * MUD works fine with this code
 *
 * Hydra_Software_Devel/3   11/6/07 2:16p pntruong
 * PR36701: Fixed build errors, and ill constructed rul instructions.
 *
 * Hydra_Software_Devel/1   11/5/07 5:03p albertl
 * PR36701:  Initial revision.
 *
 ***************************************************************************/
#include "bstd.h"
#include "bkni.h"
#include "bvdc.h"
#include "brdc.h"
#include "bvdc_priv.h"
#include "bvdc_common_priv.h"
#include "bvdc_bufferheap_priv.h"
#include "bvdc_displayfmt_priv.h"
#include "bvdc_mud_priv.h"

#if BVDC_P_SUPPORT_MUD
#include "bchp_mud_core.h"

BDBG_MODULE(BVDC_MUD);
BDBG_OBJECT_ID(BVDC_MUD);

#define BVDC_P_MUD_DEBUG 0


/***************************************************************************
 * {private}
 *
 */
BERR_Code BVDC_P_Mud_Create
	( BVDC_P_Mud_Handle                *phMud,
	  BVDC_P_MudId                      eMudId )
{
	BVDC_P_MudContext *pMud;
	uint32_t i;

	BDBG_ENTER(BVDC_P_Mud_Create);

	BDBG_ASSERT(phMud);

	/* (1) Alloc the context. */
	pMud = (BVDC_P_MudContext*)
		(BKNI_Malloc(sizeof(BVDC_P_MudContext)));
	if(!pMud)
	{
		return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
	}

	/* Clear out the context and set defaults. */
	BKNI_Memset((void*)pMud, 0x0, sizeof(BVDC_P_MudContext));
	BDBG_OBJECT_SET(pMud, BVDC_MUD);

	for(i = 0; i < BVDC_P_MUD_MAX_HD_BUFFER_COUNT; i++)
	{
		pMud->apHeapNode[i] = NULL;
	}

	pMud->pBufList = (BVDC_P_MudBuffer_Head*)BKNI_Malloc(sizeof(BVDC_P_MudBuffer_Head));
	if(!pMud->pBufList)
	{
		BDBG_OBJECT_DESTROY(pMud, BVDC_MUD);
		BKNI_Free(pMud);
		return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
	}
	BLST_CQ_INIT(pMud->pBufList);

	pMud->eId             = eMudId;
	pMud->pCurCaptureBuf  = NULL;
	pMud->pCurFeederBuf   = NULL;
	pMud->ulBufCnt        = BVDC_P_MUD_MAX_HD_BUFFER_COUNT;
	pMud->bEnable         = false;
	pMud->bActive         = false;
	pMud->usMudStartDelay = 0;

	/* Create mud buffer nodes */
	for(i = 0; i < pMud->ulBufCnt; i++)
	{
		BVDC_P_MudBufferNode *pMudBufferNode =
			(BVDC_P_MudBufferNode*)BKNI_Malloc(sizeof(BVDC_P_MudBufferNode));

		if(!pMudBufferNode)
		{
			while(i--)
			{
				pMudBufferNode = BLST_CQ_FIRST(pMud->pBufList);
				BLST_CQ_REMOVE_HEAD(pMud->pBufList, link);
				BKNI_Free(pMudBufferNode);
			}
			BKNI_Free(pMud->pBufList);
			return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
		}

		/* Clear out, insert it into the list. */
		BKNI_Memset((void*)pMudBufferNode, 0x0, sizeof(BVDC_P_MudBufferNode));

		/* Initialize non-changing fields. */
		pMudBufferNode->hMud       = (BVDC_P_Mud_Handle)pMud;
		pMudBufferNode->pHeapNode  = NULL;
		pMudBufferNode->ulBufferId = i;

		BLST_CQ_INSERT_TAIL(pMud->pBufList, pMudBufferNode, link);
	}


	/* All done. now return the new fresh context to user. */
	*phMud = (BVDC_P_Mud_Handle)pMud;

	BDBG_LEAVE(BVDC_P_Mud_Create);
	return BERR_SUCCESS;
}


/***************************************************************************
 * {private}
 *
 */
void BVDC_P_Mud_Destroy
	( BVDC_P_Mud_Handle             hMud )
{
	BVDC_P_MudBufferNode *pMudBufferNode;

	BDBG_ENTER(BVDC_P_Mud_Destroy);
	BDBG_OBJECT_ASSERT(hMud, BVDC_MUD);

	/* Free mud buffer node memory */
	while(hMud->ulBufCnt--)
	{
		pMudBufferNode = BLST_CQ_FIRST(hMud->pBufList);
		BLST_CQ_REMOVE_HEAD(pMud->pBufList, link);
		BKNI_Free(pMudBufferNode);
	}
	BKNI_Free(hMud->pBufList);

	BDBG_OBJECT_DESTROY(hMud, BVDC_MUD);
	/* [1] Free the context. */
	BKNI_Free((void*)hMud);

	BDBG_LEAVE(BVDC_P_Mud_Destroy);
	return;
}


/***************************************************************************
 * {private}
 *
 * BVDC_P_Mud_AcquireConnect
 *
 * It is called by BVDC_Display_ValidateChanges after changing from disable mud to
 * enable mud.
 */
BERR_Code BVDC_P_Mud_AcquireConnect
	( BVDC_P_Mud_Handle                *phMud,
	  BVDC_P_Resource_Handle            hResource,
	  BVDC_DisplayId                    eDispId,
	  BVDC_Heap_Handle                  hHeap )
{
	BERR_Code  eResult = BERR_SUCCESS;
#if BDBG_DEBUG_BUILD
	uint32_t  ulAcquireCntr;
#endif

	BDBG_ENTER(BVDC_P_Mud_AcquireConnect);

	BDBG_ASSERT(NULL != phMud);

	/* acquire a HW module */
	BKNI_EnterCriticalSection();

	eResult = BVDC_P_Resource_AcquireHandle_isr(hResource,
		BVDC_P_ResourceType_eMud, 0, eDispId, (void **)phMud);
#if BDBG_DEBUG_BUILD
	ulAcquireCntr = BVDC_P_Resource_GetHandleAcquireCntr_isr(
		hResource, BVDC_P_ResourceType_eMud, (void *)*phMud);
#endif
	BKNI_LeaveCriticalSection();
	if((BERR_SUCCESS != eResult) || (NULL == *phMud))
		goto Done;

	BDBG_OBJECT_ASSERT(*phMud, BVDC_MUD);
	BDBG_ASSERT((1 == ulAcquireCntr) ||
				((1 <  ulAcquireCntr) && (eDispId == (*phMud)->eDispId)));

	(*phMud)->eDispId = eDispId;
	(*phMud)->hResource = hResource;
	(*phMud)->hHeap = hHeap;

Done:

	if(BERR_SUCCESS != eResult)
		BVDC_P_Mud_ReleaseConnect_isr(phMud);

	BDBG_LEAVE(BVDC_P_Mud_AcquireConnect);
	return BERR_TRACE(eResult);
}


/***************************************************************************
 * {private}
 *
 * BVDC_P_Mud_ReleaseConnect_isr
 *
 * It is called after display decided that mud is no-longer used.
 */
BERR_Code BVDC_P_Mud_ReleaseConnect_isr
	( BVDC_P_Mud_Handle         *phMud )
{
	BERR_Code  eResult = BERR_SUCCESS;

	BDBG_ENTER(BVDC_P_Mud_ReleaseConnect_isr);

	/* handle validation */
	if(NULL != *phMud)
	{
		BDBG_OBJECT_ASSERT(*phMud, BVDC_MUD);
		BDBG_ASSERT(pMud->hResource);

		BVDC_P_Resource_ReleaseHandle_isr(
			(*phMud)->hResource, BVDC_P_ResourceType_eMud, (void *)(*phMud));

		/* this makes disp to stop calling mud code */
		*phMud = NULL;
	}

	BDBG_LEAVE(BVDC_P_Mud_ReleaseConnect_isr);
	return BERR_TRACE(eResult);
}


/***************************************************************************
 * {private}
 *
 */
static void BVDC_P_Mud_Init_isr
	( BVDC_P_Mud_Handle                 hMud,
	  BVDC_P_ListInfo                  *pList,
	  const BVDC_P_DisplayInfo         *pDispInfo )
{
	uint32_t ulBufOffset;
	const BVDC_Display_DvoSettings *pDvoCfg = &pDispInfo->stDvoCfg;

	bool  bIsProgressive = VIDEO_FORMAT_IS_PROGRESSIVE(pDispInfo->pFmtInfo->eVideoFmt);
	bool  bMudFilterEnable = (pDvoCfg->eMudFilterMode == BVDC_MudFilterMode_eDisable)? false : true;

	BDBG_OBJECT_ASSERT(hMud, BVDC_MUD);

	hMud->usFeederFrmRepeat = 0;

	/* MUD_CORE_RESET (RW) */
	/* do soft reset */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_MUD_CORE_RESET);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(MUD_CORE_RESET, HARD_RESET, 0) |
		BCHP_FIELD_DATA(MUD_CORE_RESET, SOFT_RESET, 1);

	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_MUD_CORE_RESET);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(MUD_CORE_RESET, SOFT_RESET, 0);

	/* MUD_CORE_ENABLE (RW) */
	/* reset enable bits */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_MUD_CORE_ENABLE);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(MUD_CORE_ENABLE, FORCE_START_FRAMER, 0) |
		BCHP_FIELD_DATA(MUD_CORE_ENABLE, FORCE_ADDR_FEED,    0) |
		BCHP_FIELD_DATA(MUD_CORE_ENABLE, FORCE_ADDR_CAPTURE, 0) |
		BCHP_FIELD_DATA(MUD_CORE_ENABLE, ENABLE_FRAMER,      0) |
		BCHP_FIELD_DATA(MUD_CORE_ENABLE, ENABLE_FEED,        0) |
		BCHP_FIELD_DATA(MUD_CORE_ENABLE, ENABLE_CAPTURE,     0);

	/* MUD_CORE_CONTROL (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_MUD_CORE_CONTROL);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(MUD_CORE_CONTROL, INVERT_INPUT_DE,    pDvoCfg->eDePolarity) |
		BCHP_FIELD_DATA(MUD_CORE_CONTROL, INVERT_INPUT_VSYNC, pDvoCfg->eVsPolarity) |
		BCHP_FIELD_DATA(MUD_CORE_CONTROL, INVERT_INPUT_HSYNC, pDvoCfg->eHsPolarity) |
		BCHP_FIELD_DATA(MUD_CORE_CONTROL, CLK_FREQ_SEL,       0                   ) |
		BCHP_FIELD_DATA(MUD_CORE_CONTROL, CLK_DISABLE,        0                   ) |
		BCHP_FIELD_DATA(MUD_CORE_CONTROL, COLORBAR_RGB,       0                   ) |
		BCHP_FIELD_DATA(MUD_CORE_CONTROL, GENERATE_CONST,     0                   ) |
		BCHP_FIELD_DATA(MUD_CORE_CONTROL, GENERATE_ENABLE,    0                   ) |
		BCHP_FIELD_DATA(MUD_CORE_CONTROL, LINE_FILTER_SEL,    bMudFilterEnable    ) |
		BCHP_FIELD_DATA(MUD_CORE_CONTROL, LINE_DOUBLE_ENABLE, !bIsProgressive     );

	BDBG_MSG(("[BVDC_P_Mud_Init_isr] LINE_FILTER_SEL = %d, LINE_DOUBLE_ENABLE = %d", bMudFilterEnable, !bIsProgressive));

	/* MUD_CORE_BUFFER_SIZE (RW) */
	/* write buffer size */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_MUD_CORE_BUFFER_SIZE);
	*pList->pulCurrent++ = BCHP_FIELD_DATA(MUD_CORE_BUFFER_SIZE, BUFFER_SIZE, BVDC_P_MudBuffer_GetBufferSize(hMud->pCurCaptureBuf));

	/* MUD_CORE_STATUS (RW) */
	/* MUD_CORE_INT_ENABLE (RW) */
	/* MUD_CORE_SIZE_ACTIVE (RW) */

	/* MUD_CORE_SIZE_FRAME (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_MUD_CORE_SIZE_FRAME);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(MUD_CORE_SIZE_FRAME, FRAME_WIDTH, 2200) |
		BCHP_FIELD_DATA(MUD_CORE_SIZE_FRAME, FRAME_HEIGHT, 1125);

	/* MUD_CORE_BLANK_DATA (RW) */
	/* MUD_CORE_CONST_COLOR (RW) */

	/* MUD_CORE_HORIZ_HSYNC (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_MUD_CORE_HORIZ_HSYNC);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(MUD_CORE_HORIZ_HSYNC, INVERT_SYNC, pDvoCfg->eHsPolarity) |
		BCHP_FIELD_DATA(MUD_CORE_HORIZ_HSYNC, FIRST_PIXEL, 88                  ) |
		BCHP_FIELD_DATA(MUD_CORE_HORIZ_HSYNC, LAST_PIXEL,  131                 );

	/* MUD_CORE_HORIZ_VSYNC (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_MUD_CORE_HORIZ_VSYNC);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(MUD_CORE_HORIZ_VSYNC, INVERT_SYNC, pDvoCfg->eVsPolarity) |
		BCHP_FIELD_DATA(MUD_CORE_HORIZ_VSYNC, FIRST_PIXEL, 88                  ) |
		BCHP_FIELD_DATA(MUD_CORE_HORIZ_VSYNC, LAST_PIXEL,  87                  );

	/* MUD_CORE_VERT_VSYNC (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_MUD_CORE_VERT_VSYNC);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(MUD_CORE_VERT_VSYNC, FIRST_LINE, 0) |
		BCHP_FIELD_DATA(MUD_CORE_VERT_VSYNC, LAST_LINE,  5);

	/* MUD_CORE_HORIZ_DE (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_MUD_CORE_HORIZ_DE);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(MUD_CORE_HORIZ_DE, INVERT_SYNC, pDvoCfg->eDePolarity) |
		BCHP_FIELD_DATA(MUD_CORE_HORIZ_DE, FIRST_PIXEL, 280                 ) |
		BCHP_FIELD_DATA(MUD_CORE_HORIZ_DE, LAST_PIXEL,  2199                );

	/* MUD_CORE_VERT_DE (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_MUD_CORE_VERT_DE);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(MUD_CORE_VERT_DE, FIRST_LINE, 41  ) |
		BCHP_FIELD_DATA(MUD_CORE_VERT_DE, LAST_LINE,  1120);

	/* MUD_CORE_FRM_START_DELAY (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_MUD_CORE_FRM_START_DELAY);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(MUD_CORE_FRM_START_DELAY, FRAMER_TRIGGER,     0) |
		BCHP_FIELD_DATA(MUD_CORE_FRM_START_DELAY, FRAMER_DELAY_COUNT, 0);

	/* MUD_CORE_FRM_LINE_NUMBER (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_MUD_CORE_FRM_LINE_NUMBER);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(MUD_CORE_FRM_LINE_NUMBER, LINE_NUMBER, 16);

	/* MUD_CORE_CKSUM_DVI (RO) */
	/* MUD_CORE_CKSUM_CAP (RO) */
	/* MUD_CORE_CKSUM_FEED (RO) */
	/* MUD_CORE_CKSUM_DVO (RO) */

	/* MUD_CORE_CAPTURE_ADDR (RW) */
	/* write first and second addresses to capture */
	ulBufOffset = BVDC_P_MudBuffer_GetDeviceOffset(hMud->pCurCaptureBuf) >> BVDC_P_MUD_ADDR_ALIGN_SHIFT;

	return;
}


/***************************************************************************
 * {private}
 *
 */
BERR_Code BVDC_P_Mud_BufferCreate_isr
	( BVDC_P_Mud_Handle                 hMud,
	  const BVDC_P_DisplayContext      *pDisplay )
{
	uint32_t i;
	BVDC_P_MudBufferNode *pMudBufferNode;
	BERR_Code  eResult = BERR_SUCCESS;
	BVDC_P_BufferHeapId eBufHeapId = BVDC_P_BufferHeapId_e2HD;

	BSTD_UNUSED(pDisplay);
	BDBG_ENTER(BVDC_P_Mud_BufferCreate_isr);
	BDBG_OBJECT_ASSERT(hMud, BVDC_MUD);

	/* allocate buffers */
	BDBG_MSG(("Mud selects %d (%s) mud buffers", hMud->ulBufCnt,
		BVDC_P_BUFFERHEAP_GET_HEAP_ID_NAME(eBufHeapId)));
	eResult = BVDC_P_BufferHeap_AllocateBuffers_isr(hMud->hHeap,
		hMud->apHeapNode, hMud->ulBufCnt, false,
		eBufHeapId, BVDC_P_BufferHeapId_eUnknown);
	/* Not enough memory, dump out configuration */
	if(eResult == BERR_OUT_OF_DEVICE_MEMORY)
	{
		BDBG_ERR(("MAD[%d] Not enough memory for MUD! Configuration:", hMud->eId));
		BDBG_ERR(("MAD[%d] BufferCnt: %d", hMud->eId, hMud->ulBufCnt));
	}

	BDBG_ASSERT(BERR_SUCCESS == eResult);

	/* Assign buffer to mud buffer nodes */
	pMudBufferNode = BLST_CQ_FIRST(hMud->pBufList);

	for(i = 0; i < hMud->ulBufCnt; i++)
	{
		pMudBufferNode->pHeapNode = hMud->apHeapNode[i];
		BDBG_ASSERT(pMudBufferNode->pHeapNode);

		pMudBufferNode = BLST_CQ_NEXT(pMudBufferNode, link);
	}

	hMud->pCurCaptureBuf = BLST_CQ_FIRST(hMud->pBufList);

	BDBG_LEAVE(BVDC_P_Mud_BufferCreate_isr);
	return BERR_SUCCESS;
}


/***************************************************************************
 * {private}
 *
 */
BERR_Code BVDC_P_Mud_BufferDestroy_isr
	( BVDC_P_Mud_Handle                 hMud )
{
	uint32_t i;
	BVDC_P_MudBufferNode *pMudBufferNode;

	BDBG_ENTER(BVDC_P_Mud_BufferDestroy_isr);
	BDBG_OBJECT_ASSERT(hMud, BVDC_MUD);

	/* Release memory for mud if allocated */
	if(NULL != hMud->apHeapNode[0])
	{
		BVDC_P_BufferHeap_FreeBuffers_isr(hMud->hHeap, hMud->apHeapNode,
			hMud->ulBufCnt, false);
		hMud->apHeapNode[0] = NULL;
	}

	pMudBufferNode = BLST_CQ_FIRST(hMud->pBufList);

	for(i = 0; i < hMud->ulBufCnt; i++)
	{
		pMudBufferNode->pHeapNode = NULL;

		pMudBufferNode = BLST_CQ_NEXT(pMudBufferNode, link);
	}

	BDBG_LEAVE(BVDC_P_Mud_BufferDestroy_isr);
	return BERR_SUCCESS;
}


/***************************************************************************
 * {private}
 *
 */
void BVDC_P_Mud_SetEnable_isr
	( BVDC_P_Mud_Handle                 hMud,
	  BVDC_P_ListInfo                  *pList )
{
	BDBG_ENTER(BVDC_P_Mud_SetEnable_isr);
	BDBG_OBJECT_ASSERT(hMud, BVDC_MUD);

	if (hMud->bEnable)
	{
		/* MUD_CORE_FRM_START_DELAY (RW) */
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_MUD_CORE_FRM_START_DELAY);
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(MUD_CORE_FRM_START_DELAY, FRAMER_TRIGGER,     1) |
			BCHP_FIELD_DATA(MUD_CORE_FRM_START_DELAY, FRAMER_DELAY_COUNT, BVDC_P_MUD_TRIGGER_DELAY);
		BDBG_MSG(("Programming a delay of %Xh", BVDC_P_MUD_TRIGGER_DELAY));
	}

	/* MUD_CORE_ENABLE (RW) */
	/* enable framer, feeder, and capture */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_MUD_CORE_ENABLE);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(MUD_CORE_ENABLE, FORCE_START_FRAMER, 0) |
		BCHP_FIELD_DATA(MUD_CORE_ENABLE, ENABLE_FRAMER,  hMud->bEnable) |
		BCHP_FIELD_DATA(MUD_CORE_ENABLE, ENABLE_FEED,    hMud->bEnable) |
		BCHP_FIELD_DATA(MUD_CORE_ENABLE, ENABLE_CAPTURE, hMud->bEnable);
	BDBG_MSG(("Enabled MUD capture, feeder and framer"));

	BDBG_LEAVE(BVDC_P_Mud_SetEnable_isr);

  return;
}


/***************************************************************************
 * {private}
 *
 */
void BVDC_P_Mud_Enable_isr
	( BVDC_P_Mud_Handle                 hMud,
	  BVDC_P_ListInfo                  *pList,
	  const BVDC_P_DisplayInfo         *pDispInfo,
	  bool                              bEnable )
{
	BDBG_ENTER(BVDC_P_Mud_Enable_isr);
	BDBG_MSG(("In BVDC_P_Mud_SetEnable_isr, bEnable = %d", bEnable));
	BDBG_OBJECT_ASSERT(hMud, BVDC_MUD);

	hMud->bEnable = bEnable;
	hMud->bActive = false;

	if(bEnable)
	{
		BVDC_P_Mud_Init_isr(hMud, pList, pDispInfo);
		hMud->usMudStartDelay = 1;
	}
	else
	{
		/* Call BVDC_P_Mud_SetEnable_isr here if turning Mud off.
		 * BVDC_P_Mud_SetEnable_isr will be called in BVDC_P_Mud_BuildRul_isr
		 * if turning Mud on. */
		BVDC_P_Mud_SetEnable_isr(hMud, pList);
	}

	BDBG_LEAVE(BVDC_P_Mud_Enable_isr);

  return;
}


/***************************************************************************
 * {private}
 *
 */
BERR_Code BVDC_P_Mud_BuildRul_isr
	( BVDC_P_Mud_Handle                 hMud,
	  BVDC_P_ListInfo                  *pList,
	  const BVDC_P_DisplayContext      *pDisplay,
	  BAVC_Polarity                     eFieldPolarity )
{
	const BVDC_P_DisplayInfo *pNewInfo = &pDisplay->stNewInfo;
	const BFMT_VideoInfo *pFmtInfo = pNewInfo->pFmtInfo;

	uint32_t i = 0;
	uint32_t ulBufOffset;

	BDBG_ENTER(BVDC_P_Mud_BuildRul_isr);
	BDBG_ASSERT(pDisplay);
	BDBG_ASSERT(pList);
	BDBG_OBJECT_ASSERT(hMud, BVDC_MUD);

#if BVDC_P_MUD_DEBUG
	{
		uint32_t ulReg[16];

		BREG_Write32(pDisplay->hVdc->hRegister, BCHP_MUD_CORE_STATUS, 0xffffffff);

		ulReg[0] = BREG_Read32(pDisplay->hVdc->hRegister, BCHP_MUD_CORE_CAPTURE_ADDR);
		ulReg[1] = BREG_Read32(pDisplay->hVdc->hRegister, BCHP_MUD_CORE_FEEDER_ADDR);
		ulReg[2] = BREG_Read32(pDisplay->hVdc->hRegister, BCHP_MUD_CORE_STATUS);
		BDBG_MSG(("MUD_CORE_STATUS %08Xh, Current: %08Xh %08Xh", ulReg[2], ulReg[0], ulReg[1]));
		if (ulReg[2] & 0xFFF)
		{
			BDBG_ERR((">>>>>>>>>>>>>>> ERROR  (status = %08Xh) <<<<<<<<<<<<<<<", ulReg[2]));
		}
	}
#endif

	hMud->bPicField = (eFieldPolarity != BAVC_Polarity_eFrame);
	hMud->bPicFieldBottom = (eFieldPolarity == BAVC_Polarity_eBotField);

	/* calculate feeder frame repeat */
	switch (pFmtInfo->eVideoFmt)
	{
		case BFMT_VideoFmt_e1080i:
			hMud->usFeederFrmRepeat = 1;
			break;
		case BFMT_VideoFmt_e1080p_24Hz:
			/* handle 3:2 pulldown */
			if(hMud->usFeederFrmRepeat == 3)
			{
				hMud->usFeederFrmRepeat = 2;
			}
			else
			{
				hMud->usFeederFrmRepeat = 3;
			}
			break;
		case BFMT_VideoFmt_e1080p_30Hz:
			hMud->usFeederFrmRepeat = 2;
			break;
		default:
			break;
	}

	hMud->pCurFeederBuf = hMud->pCurCaptureBuf;
	hMud->pCurCaptureBuf = BVDC_P_MudBuffer_GetNextNode(hMud->pCurCaptureBuf);

	/* write next buffer address to capture */
	ulBufOffset = BVDC_P_MudBuffer_GetDeviceOffset(hMud->pCurCaptureBuf) >> BVDC_P_MUD_ADDR_ALIGN_SHIFT;
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_MUD_CORE_CAPTURE_ADDR);
	*pList->pulCurrent++ = BCHP_FIELD_DATA(MUD_CORE_CAPTURE_ADDR, BUFFER_ADDR, ulBufOffset) |
						   BCHP_FIELD_DATA(MUD_CORE_CAPTURE_ADDR, PIC_FIELD, hMud->bPicField) |
						   BCHP_FIELD_DATA(MUD_CORE_CAPTURE_ADDR, PIC_FIELD_BOTTOM, hMud->bPicFieldBottom);

	/* write previous buffer address to feeder */
	ulBufOffset = BVDC_P_MudBuffer_GetDeviceOffset(hMud->pCurFeederBuf) >> BVDC_P_MUD_ADDR_ALIGN_SHIFT;

	/* write to feeder address repeatedly to repeat frames */
	for (i = 0; i < hMud->usFeederFrmRepeat; i++)
	{
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_MUD_CORE_FEEDER_ADDR);
		*pList->pulCurrent++ = BCHP_FIELD_DATA(MUD_CORE_FEEDER_ADDR, BUFFER_ADDR, ulBufOffset) |
							   BCHP_FIELD_DATA(MUD_CORE_FEEDER_ADDR, PIC_FIELD, hMud->bPicField) |
							   BCHP_FIELD_DATA(MUD_CORE_FEEDER_ADDR, PIC_FIELD_BOTTOM, (hMud->bPicField) ? (!hMud->bPicFieldBottom) : 0);
	}

	if (hMud->usMudStartDelay > 0)
	{
		hMud->usMudStartDelay--;
	}
	else if (!hMud->bActive)
	{
		BVDC_P_Mud_SetEnable_isr(hMud, pList);
		hMud->bActive = true;
	}

	BDBG_LEAVE(BVDC_P_Mud_BuildRul_isr);
	return BERR_SUCCESS;
}


#endif /* BVDC_SUPPORT_MUD */

/* End of file. */
