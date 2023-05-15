/***************************************************************************
 *     Copyright (c) 2007-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvdc_mud_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/7 $
 * $brcm_Date: 4/7/10 11:33a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vdc/7038/bvdc_mud_priv.h $
 * 
 * Hydra_Software_Devel/7   4/7/10 11:33a tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic.  Rename TLA
 *
 * Hydra_Software_Devel/6   4/5/10 4:12p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic
 *
 * Hydra_Software_Devel/5   12/22/07 3:38a albertl
 * PR36701:  Integrated and rewrote andover mud code.  Fixed mud
 * transitional issues.
 *
 * Hydra_Software_Devel/2   11/6/07 2:16p pntruong
 * PR36701: Fixed build errors, and ill constructed rul instructions.
 *
 * Hydra_Software_Devel/1   11/5/07 5:04p albertl
 * PR36701:  Initial revision.
 *
 ***************************************************************************/
#ifndef BVDC_MUD_PRIV_H__
#define BVDC_MUD_PRIV_H__

#include "bvdc.h"
#include "blst_circleq.h"
#include "bvdc_priv.h"
#include "bvdc_display_priv.h"

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************
 *  Defines
 ****************************************************************/
#define BVDC_P_MUD_MAX_HD_BUFFER_COUNT 2
#define BVDC_P_MUD_ADDR_ALIGN_SHIFT 2
#define BVDC_P_MUD_TRIGGER_DELAY 0x10000

/****************************************************************
 *  Macros
 ****************************************************************/
#define BVDC_P_MudBuffer_GetDeviceOffset(pMudBufferNode)   \
	pMudBufferNode->pHeapNode->ulDeviceOffset

#define BVDC_P_MudBuffer_GetNextNode(pMudBufferNode)   \
	BLST_CQ_NEXT(pMudBufferNode, link)

#define BVDC_P_MudBuffer_GetBufferSize(pMudBufferNode)   \
	pMudBufferNode->pHeapNode->pHeapInfo->ulBufSize

/* supported MUD formats */
#define BVDC_P_MUD_SUPPORTS_FORMAT(fmt) \
	((fmt == BFMT_VideoFmt_e1080i) | \
	 (fmt == BFMT_VideoFmt_e1080p_24Hz) | \
	 (fmt == BFMT_VideoFmt_e1080p_30Hz))

/***************************************************************************
 * Structures
 ***************************************************************************/

/***************************************************************************
 * BRDC_P_Slot_Head
 *      Head of the double Link List for slot
 ***************************************************************************/
typedef struct BVDC_P_MudBuffer_Head  BVDC_P_MudBuffer_Head;
BLST_CQ_HEAD(BVDC_P_MudBuffer_Head, BVDC_P_MudBufferNode);

typedef struct BVDC_P_MudBufferNode
{
	BLST_CQ_ENTRY(BVDC_P_MudBufferNode)  link;                 /* doubly-linked list support */

	uint32_t                             ulBufferId;
	BVDC_P_HeapNodePtr                   pHeapNode;            /* Hold capture memory */
	BVDC_P_Mud_Handle                    hMud;

} BVDC_P_MudBufferNode;

typedef struct BVDC_P_MudContext
{
	BDBG_OBJECT(BVDC_MUD)

	BVDC_P_MadId                         eId;
	BVDC_DisplayId                       eDispId;
	bool                                 bEnable;
	bool                                 bActive;
	uint16_t                             usMudStartDelay;

	BVDC_Heap_Handle                     hHeap;
	BVDC_P_MudBuffer_Head               *pBufList;             /* Double link list */
	BVDC_P_HeapNodePtr                   apHeapNode[BVDC_P_MUD_MAX_HD_BUFFER_COUNT];
	uint32_t                             ulBufCnt;

	BVDC_P_Resource_Handle               hResource;

	BVDC_P_MudBufferNode                *pCurCaptureBuf;
	BVDC_P_MudBufferNode                *pCurFeederBuf;

	bool                                 bPicField;
	bool                                 bPicFieldBottom;

	uint16_t                             usFeederFrmRepeat;

} BVDC_P_MudContext;

/***************************************************************************
 * Mud private functions
 ***************************************************************************/

BERR_Code BVDC_P_Mud_Create
	( BVDC_P_Mud_Handle                *phMud,
	  BVDC_P_MudId                      eMudId);

void BVDC_P_Mud_Destroy
	( BVDC_P_Mud_Handle                 hMud);

BERR_Code BVDC_P_Mud_AcquireConnect
	( BVDC_P_Mud_Handle                *phMud,
	  BVDC_P_Resource_Handle            hResource,
	  BVDC_DisplayId                    eDispId,
	  BVDC_Heap_Handle                  hHeap );

BERR_Code BVDC_P_Mud_ReleaseConnect_isr
	( BVDC_P_Mud_Handle                *phMud );

BERR_Code BVDC_P_Mud_BufferCreate_isr
	( BVDC_P_Mud_Handle                 hMud,
	  const BVDC_P_DisplayContext      *pDisplay );

BERR_Code BVDC_P_Mud_BufferDestroy_isr
	( BVDC_P_Mud_Handle                 hMud );

void BVDC_P_Mud_Enable_isr
	( BVDC_P_Mud_Handle                 hMud,
	  BVDC_P_ListInfo                  *pList,
	  const BVDC_P_DisplayInfo         *pDispInfo,
	  bool                              bEnable );

BERR_Code BVDC_P_Mud_BuildRul_isr
	( BVDC_P_Mud_Handle                 hMud,
	  BVDC_P_ListInfo                  *pList,
	  const BVDC_P_DisplayContext      *pDisplay,
	  BAVC_Polarity                     eFieldPolarity );

#ifdef __cplusplus
}
#endif

#endif /* #ifndef BVDC_MUD_PRIV_H__ */
/* End of file. */
