/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvdc_bufferheap_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/22 $
 * $brcm_Date: 1/12/12 4:34p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vdc/7038/bvdc_bufferheap_priv.h $
 * 
 * Hydra_Software_Devel/22   1/12/12 4:34p yuxiaz
 * SW7552-181: Adjust buffer size based on alignment.
 * 
 * Hydra_Software_Devel/21   4/19/10 10:11p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic. Move
 * BDBG_OBJECT_ID_DECLARE private header files instead of .c.
 *
 * Hydra_Software_Devel/20   4/7/10 11:26a tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic.  Rename TLA
 *
 * Hydra_Software_Devel/19   4/5/10 3:58p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic
 *
 * Hydra_Software_Devel/18   6/4/09 2:00p yuxiaz
 * PR55330, PR55323, PR53197, PR55158, PR55604: Need to use separate CAP
 * and VFD buffers in ANR when DCX is enabled.
 *
 * Hydra_Software_Devel/17   10/30/07 7:10p pntruong
 * PR34239: Allow dynamically loading of vec custom timing.
 *
 * Hydra_Software_Devel/16   7/27/07 9:32a yuxiaz
 * PR32421: VDC Should Compute the Heap Size Needed for MAD and ANR for
 * Various Sources.
 *
 * Hydra_Software_Devel/15   1/24/07 9:05p albertl
 * PR22237:  Updated BMEM calls to use new BMEM_Heap functions.
 *
 * Hydra_Software_Devel/14   12/18/06 11:36p pntruong
 * PR22577: Merged back to mainline.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/5   10/26/06 3:17p yuxiaz
 * PR25181: Merge in fix from main branch: VDC Buffer Allocation Fragments
 * HD Buffer Pool and Causes Out of Memory Assertion.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/4   9/27/06 12:01p yuxiaz
 * PR20875: Use best fit search to get buffer heap id. Integrate with
 * dynamic buffer allocation.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/3   9/21/06 9:44a yuxiaz
 * PR20875: Integrate buffer heap with pixel format in BVDC_Heap_Settings.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/2   9/20/06 12:54p pntruong
 * PR23222, PR23225:  Defer window and source heap assignments until user
 * create.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/1   9/19/06 10:34p pntruong
 * PR23222, PR23225: Fixed compile errors for heap based created.
 *
 * Hydra_Software_Devel/12   8/10/06 2:27p yuxiaz
 * PR20875: Consolidate BVN pixel format to support 10 bit 444 bvn path.
 *
 * Hydra_Software_Devel/11   4/26/06 1:51p yuxiaz
 * PR20598: Allocate continous nodes from child node list.
 *
 * Hydra_Software_Devel/10   4/24/06 4:04p yuxiaz
 * PR15161: Fixed VDC _isr naming violations in bvdc_buffer_priv.c and
 * bvdc_bufferheap_priv.c.
 *
 * Hydra_Software_Devel/9   4/14/06 2:31p yuxiaz
 * PR20598: Add support to break larger buffer into smaller buffers in
 * VDC.
 *
 * Hydra_Software_Devel/8   3/28/06 1:23p yuxiaz
 * PR20344: Added double HD buffer support in VDC.
 *
 * Hydra_Software_Devel/7   11/2/05 9:58a jessem
 * PR 17395: Added support for SD Pip buffer use.
 *
 * Hydra_Software_Devel/6   8/18/05 1:12p pntruong
 * PR15757, PR16391, PR16411, PR12519, PR14791, PR15535, PR15206, PR15778:
 * Improved bandwidth for cropping/scaler/capture/playback.   Unified
 * window shutdown sequence for destroy, reconfigure mad/scaler, and/or
 * reconfigure result of source changes.  And miscellances fixes from
 * above PRs.
 *
 * Hydra_Software_Devel/XVDPhase1/1   6/30/05 2:44p pntruong
 * PR15284: Used the recommended 32-byte align pitch/stride for optimal
 * memory bandwidth utilization.
 *
 * Hydra_Software_Devel/5   4/1/05 5:13p pntruong
 * PR14686: Refresh mad mstart addresses on re-enable and fixed vdec 3d
 * comb mstart alignment.
 *
 * Hydra_Software_Devel/4   1/6/05 2:09p yuxiaz
 * PR13313: Added HD PIP buffer support in VDC.
 *
 * Hydra_Software_Devel/3   12/15/04 3:26p yuxiaz
 * PR13569: Add support for syncronization with XVD for HD_DVI. More work
 * needed.
 *
 * Hydra_Software_Devel/2   12/1/04 4:24p yuxiaz
 * PR10855: Add support to use HD buffers for SD.
 *
 * Hydra_Software_Devel/1   11/19/04 2:40p yuxiaz
 * PR 10855: Initial revision.
 *
 *
 ***************************************************************************/
#ifndef BVDC_BUFFERHEAP_PRIV_H__
#define BVDC_BUFFERHEAP_PRIV_H__

#include "bvdc.h"
#include "blst_queue.h"
#include "bvdc_common_priv.h"

#ifdef __cplusplus
extern "C" {
#endif

BDBG_OBJECT_ID_DECLARE(BVDC_VDC);

/***************************************************************************
 * Private macros
 ***************************************************************************/
#define BVDC_P_BUFFERHEAP_HEAP_IS_2HD(eBufHeapId) \
	(eBufHeapId == BVDC_P_BufferHeapId_e2HD)

#define BVDC_P_BUFFERHEAP_HEAP_IS_HD(eBufHeapId) \
	(eBufHeapId == BVDC_P_BufferHeapId_eHD)

#define BVDC_P_BUFFERHEAP_HEAP_IS_SD(eBufHeapId) \
	(eBufHeapId == BVDC_P_BufferHeapId_eSD)

#define BVDC_P_BUFFERHEAP_HEAP_IS_2HD_PIP(eBufHeapId) \
	(eBufHeapId == BVDC_P_BufferHeapId_e2HD_Pip)

#define BVDC_P_BUFFERHEAP_HEAP_IS_HD_PIP(eBufHeapId) \
	(eBufHeapId == BVDC_P_BufferHeapId_eHD_Pip)

#define BVDC_P_BUFFERHEAP_HEAP_IS_SD_PIP(eBufHeapId) \
	(eBufHeapId == BVDC_P_BufferHeapId_eSD_Pip)

#define BVDC_P_BUFFERHEAP_GET_HEAP_ID_NAME(eBufHeapId)      \
	BVDC_P_BUFFERHEAP_HEAP_IS_2HD(eBufHeapId)     ? "2HD"     :    \
	BVDC_P_BUFFERHEAP_HEAP_IS_HD(eBufHeapId)      ? "HD"      :    \
	BVDC_P_BUFFERHEAP_HEAP_IS_SD(eBufHeapId)      ? "SD"      :    \
	BVDC_P_BUFFERHEAP_HEAP_IS_2HD_PIP(eBufHeapId) ? "2HD_Pip" :    \
	BVDC_P_BUFFERHEAP_HEAP_IS_HD_PIP(eBufHeapId)  ? "HD_Pip"  :    \
	BVDC_P_BUFFERHEAP_HEAP_IS_SD_PIP(eBufHeapId)  ? "SD_Pip"  : "Unknown"


#define BVDC_P_BUFFERHEAP_GetDeviceOffset(pHeapNode)   \
	pHeapNode->ulDeviceOffset

/***************************************************************************
 * Internal defines
 ***************************************************************************/
#define BVDC_P_HEAP_MEMORY_ALIGNMENT       (8)
#define BVDC_P_HEAP_ALIGN_BYTES            (256)


/***************************************************************************
 * Enums
 ***************************************************************************/
typedef enum BVDC_P_BufferHeapId
{
	BVDC_P_BufferHeapId_eSD = 0,
	BVDC_P_BufferHeapId_eHD,
	BVDC_P_BufferHeapId_e2HD,
	BVDC_P_BufferHeapId_eSD_Pip,
	BVDC_P_BufferHeapId_eHD_Pip,
	BVDC_P_BufferHeapId_e2HD_Pip,

	BVDC_P_BufferHeapId_eCount,             /* Counter. Do not use! */
	BVDC_P_BufferHeapId_eUnknown

} BVDC_P_BufferHeapId;

/***************************************************************************
 * BVDC_P_BufferHeap_Head
 ***************************************************************************/
typedef struct BVDC_P_BufferHeap_Head  BVDC_P_BufferHeap_Head;
BLST_Q_HEAD(BVDC_P_BufferHeap_Head, BVDC_P_BufferHeapNode);


/***************************************************************************
 * BVDC_P_BufferHeapNode
 ***************************************************************************/
typedef struct BVDC_P_BufferHeapNode
{
	/* Node info: linked-list bookeeping */
	BLST_Q_ENTRY(BVDC_P_BufferHeapNode)  link;

	BVDC_P_HeapInfoPtr                   pHeapInfo;   /* heap the node belongs to */

	void                                *pvBufAddr;   /* Virtual address */
	uint32_t                             ulDeviceOffset; /* Device offset */
	uint32_t                             ulBufIndex;  /* index to heap's bufferlist */
	BVDC_P_BufferHeapId                  eOrigBufHeapId; /* which buffer the node comes from */
	bool                                 bUsed;       /* node is used or not */
	bool                                 bContinous;  /* continuous with prev node in bufferlist */

	BVDC_P_HeapNodePtr                   pParentNode; /* parent node */
	BVDC_P_HeapNodePtr                   pFirstChildNode; /* first child node */
	uint32_t                             ulNumChildNodeUsed; /* num of child nodes used */

} BVDC_P_BufferHeapNode;


/***************************************************************************
 * BVDC_P_BufferHeap_Info
 ***************************************************************************/
typedef struct BVDC_P_BufferHeap_Info
{
	BVDC_P_BufferHeap_Handle   hBufferHeap;

	/* Const properties */
	BVDC_P_BufferHeapId        eBufHeapId;
	uint32_t                   ulWidth;
	uint32_t                   ulHeight;
	/* real size including alignment */
	uint32_t                   ulBufSize;
	/* size by format without alignment, only used to decide how to split buffers */
	uint32_t                   ulBufSizeByFmt;
	uint32_t                   ulPrimaryBufCnt;
	uint32_t                   ulTotalBufCnt;
	uint32_t                   ulNodeCntPerParent; /* # nodes = 1 parent node */
	BVDC_P_HeapInfoPtr         pParentHeapInfo;
	BVDC_P_HeapInfoPtr         pChildHeapInfo;
	/* Virtual address of start of the primary heap */
	void                      *pvHeapStartAddr;

	BVDC_P_BufferHeap_Head    *pBufList;
	uint32_t                   ulBufUsed;

} BVDC_P_BufferHeap_Info;


/***************************************************************************
 * BVDC_P_BufferHeapContext
 ***************************************************************************/
typedef struct BVDC_P_BufferHeapContext
{
	BDBG_OBJECT(BVDC_BFH)

	/* Handed down by created */
	BVDC_Handle                      hVdc;          /* Created from this Vdc */
	BMEM_Heap_Handle                 hMem;          /* corresponding heap. */

	/* Heap Settings */
	BVDC_Heap_Settings               stSettings;

	/* Heapsize of the different flavorites of buffers. */
	uint32_t                         aulHeapSize[BVDC_P_BufferHeapId_eCount];

	/* Index of astHeapInfo by BVDC_P_BufferHeapId */
	uint32_t                         aulIndex[BVDC_P_BufferHeapId_eCount];
	BVDC_P_BufferHeap_Info           astHeapInfo[BVDC_P_BufferHeapId_eCount];

} BVDC_P_BufferHeapContext;


/***************************************************************************
 * Memory private functions
 ***************************************************************************/
BERR_Code BVDC_P_BufferHeap_Create
	( const BVDC_Handle                hVdc,
	  BVDC_Heap_Handle                *phHeap,
	  BMEM_Heap_Handle                 hMem,
	  const BVDC_Heap_Settings        *pSettings );

BERR_Code BVDC_P_BufferHeap_Destroy
	( BVDC_P_BufferHeap_Handle         hBufferHeap );

void BVDC_P_BufferHeap_Init
	( BVDC_P_BufferHeap_Handle         hBufferHeap );

BERR_Code BVDC_P_BufferHeap_AllocateBuffers_isr
	( BVDC_P_BufferHeap_Handle         hBufferHeap,
	  BVDC_P_HeapNodePtr               apHeapNode[],
	  uint32_t                         ulCount,
	  bool                             bContinuous,
	  BVDC_P_BufferHeapId              eBufferHeapId,
	  BVDC_P_BufferHeapId              eBufferHeapIdPrefer );

BERR_Code BVDC_P_BufferHeap_FreeBuffers_isr
	( BVDC_P_BufferHeap_Handle         hBufferHeap,
	  BVDC_P_HeapNodePtr               apHeapNode[],
	  uint32_t                         ulCount,
	  bool                             bContinuous );

uint32_t BVDC_P_BufferHeap_GetHeapSize
	( const BFMT_VideoInfo            *pFmtInfo,
	  BPXL_Format                      ePixelFmt,
	  uint32_t                         ulAdditionalLines,
	  bool                             bPip,
	  uint32_t                        *pulUnalignedBufSize,
	  uint32_t                        *pulWidth,
	  uint32_t                        *pulHeight );

void BVDC_P_BufferHeap_GetHeapIdBySize
	( const BVDC_P_BufferHeapContext  *pHeap,
	  uint32_t                         ulSize,
	  BVDC_P_BufferHeapId             *peBufHeapId );

void BVDC_P_BufferHeap_GetHeapSizeById
	( const BVDC_P_BufferHeapContext  *pHeap,
	  BVDC_P_BufferHeapId              eBufHeapId,
	  uint32_t                        *pulBufHeapSize );

#ifdef __cplusplus
}
#endif

#endif /* #ifndef BVDC_BUFFERHEAP_PRIV_H__*/

/* End of file. */
