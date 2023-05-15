/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvdc_bufferheap_priv.c $
 * $brcm_Revision: Hydra_Software_Devel/59 $
 * $brcm_Date: 8/14/12 1:58p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vdc/src/common/bvdc_bufferheap_priv.c $
 * 
 * Hydra_Software_Devel/59   8/14/12 1:58p yuxiaz
 * SW7550-802: Fix coverity errors.
 * 
 * Hydra_Software_Devel/58   8/13/12 4:01p tdo
 * SW7425-3734: Fix coverity issues
 *
 * Hydra_Software_Devel/57   8/3/12 12:53p vanessah
 * SW7425-3591: make the buffer size align
 *
 * Hydra_Software_Devel/56   7/11/12 3:26p yuxiaz
 * SW7344-344: Unmark previously marked nodes when not enough buffer nodes
 * available.
 *
 * Hydra_Software_Devel/55   2/16/12 5:48p yuxiaz
 * SW7231-635: Added sw workaround for VFD.
 *
 * Hydra_Software_Devel/54   1/12/12 4:34p yuxiaz
 * SW7552-181: Adjust buffer size based on alignment.
 *
 * Hydra_Software_Devel/53   1/9/12 11:12a pntruong
 * SW7552-181: Added temp work-around for alignment and buffer re-used.
 *
 * Hydra_Software_Devel/52   12/1/11 12:44p yuxiaz
 * SW7420-1157: Added DCX workaround for non-pip mem allocation.
 *
 * Hydra_Software_Devel/51   5/16/11 1:49p yuxiaz
 * SW7405-5328: Fixed memory leak.
 *
 * Hydra_Software_Devel/50   3/31/11 3:30p pntruong
 * SW7425-267: MADR input needs to be multiple of 4.
 *
 * Hydra_Software_Devel/49   1/12/11 4:33p yuxiaz
 * SW7335-1133: Added more debug messages for VDC heap debugging.
 *
 * Hydra_Software_Devel/48   10/15/10 12:41p tdo
 * SW7420-1157: Take DCX workaround into account for PIP mem allocation
 *
 * Hydra_Software_Devel/47   4/19/10 10:11p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic. Move
 * BDBG_OBJECT_ID_DECLARE private header files instead of .c.
 *
 * Hydra_Software_Devel/46   4/7/10 11:25a tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic.  Rename TLA
 *
 * Hydra_Software_Devel/45   4/5/10 3:58p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic
 *
 * Hydra_Software_Devel/44   6/4/09 1:59p yuxiaz
 * PR55330, PR55323, PR53197, PR55158, PR55604: Need to use separate CAP
 * and VFD buffers in ANR when DCX is enabled.
 *
 * Hydra_Software_Devel/43   5/15/09 6:46p pntruong
 * PR53654: Avoid no flash on digital source changes up to 720p.  Need
 * compile option BVDC_NO_FLASH=1.
 *
 * Hydra_Software_Devel/42   2/13/09 4:52p pntruong
 * PR51848: Used existing print format in bvdc.c.
 *
 * Hydra_Software_Devel/41   2/13/09 2:53p jessem
 * PR 51848: Added detailed information when buffer allocation fails.
 * Error message wil print out the number, type and format of the buffer
 * needed and the the number of buffers allocated. It will also print out
 * the contents of the BVDC_Heap_Settings struct used to allocate memory.
 *
 * Hydra_Software_Devel/40   1/12/09 3:04p yuxiaz
 * PR50931: Change BDBG_WRN to BDBG_MSG in
 * BVDC_P_BufferHeap_AllocateNodes_isr.
 *
 * Hydra_Software_Devel/39   1/12/09 9:22a yuxiaz
 * PR49598: Back out previous change, takes too much time to initialize
 * memory.
 *
 * Hydra_Software_Devel/38   11/24/08 11:02a yuxiaz
 * PR49598: Use bandwidth equation to calculate throughput.
 *
 * Hydra_Software_Devel/37   9/29/08 1:33p yuxiaz
 * PR47370: Use BVDC_P_BUF_MSG for buffer related messages.
 *
 * Hydra_Software_Devel/36   9/2/08 6:04p pntruong
 * PR46314, PR46316: Updated 3dcomb memory allocation, and added buffers
 * alloc debug messages.
 *
 * Hydra_Software_Devel/35   7/29/08 7:48p pntruong
 * PR40213: Used correct alignment macro.
 *
 * Hydra_Software_Devel/34   3/7/08 9:14p pntruong
 * PR37836, PR39921: Added more helpful info for out of vdc heap to ease
 * debugging.
 *
 * Hydra_Software_Devel/33   10/30/07 7:09p pntruong
 * PR34239: Allow dynamically loading of vec custom timing.
 *
 * Hydra_Software_Devel/32   7/27/07 9:32a yuxiaz
 * PR32421: VDC Should Compute the Heap Size Needed for MAD and ANR for
 * Various Sources.
 *
 * Hydra_Software_Devel/31   7/6/07 3:37p hongtaoz
 * PR32777: capture buffer heap alignment issue;
 *
 * Hydra_Software_Devel/30   1/24/07 9:05p albertl
 * PR22237:  Updated BMEM calls to use new BMEM_Heap functions.
 *
 * Hydra_Software_Devel/29   12/18/06 11:37p pntruong
 * PR22577: Merged back to mainline.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/6   11/3/06 4:07p pntruong
 * PR23222:: Hush linux warnings.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/6   11/3/06 4:01p pntruong
 * PR23222:: Hush linux warnings.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/5   10/26/06 3:16p yuxiaz
 * PR25181: Merge in fix from main branch: VDC Buffer Allocation Fragments
 * HD Buffer Pool and Causes Out of Memory Assertion.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/4   9/27/06 12:00p yuxiaz
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
 * Hydra_Software_Devel/26   8/28/06 1:43p yuxiaz
 * PR20875: Renamed BVDC_P_SUPPORT_10BIT_BVN to
 * BVDC_P_SUPPORT_BVN_10BIT_444. Removed BVDC_P_BVNDataMode, use window
 * and buffer node's pixel format. Integrated with new 10 bit pixel
 * formats.
 *
 * Hydra_Software_Devel/25   8/10/06 2:27p yuxiaz
 * PR20875: Consolidate BVN pixel format to support 10 bit 444 bvn path.
 *
 * Hydra_Software_Devel/24   8/8/06 1:29p jessem
 * PR 23249: Changed ulPitch to unsigned int uiPitch in
 * BVDC_P_BufferHeap_GetHeapSize.
 *
 * Hydra_Software_Devel/23   6/14/06 5:07p albertl
 * PR20354:  Only do address conversion if ulPrimaryBufCnt is not zero.
 *
 * Hydra_Software_Devel/22   4/26/06 1:50p yuxiaz
 * PR20598: Allocate continous nodes from child node list.
 *
 * Hydra_Software_Devel/21   4/24/06 4:04p yuxiaz
 * PR15161: Fixed VDC _isr naming violations in bvdc_buffer_priv.c and
 * bvdc_bufferheap_priv.c.
 *
 * Hydra_Software_Devel/20   4/24/06 11:02a yuxiaz
 * PR20598: Fixed compiler warnings.
 *
 * Hydra_Software_Devel/19   4/14/06 2:31p yuxiaz
 * PR20598: Add support to break larger buffer into smaller buffers in
 * VDC.
 *
 * Hydra_Software_Devel/18   4/5/06 2:57p yuxiaz
 * PR20344: More work for 2HD buffer support: modify BVDC_Settings
 * : remove bUseHDBuffer4SD, change bNtscOnly_SD to eBufferFormat_SD.
 *
 * Hydra_Software_Devel/17   3/28/06 1:22p yuxiaz
 * PR20344: Added double HD buffer support in VDC.
 *
 * Hydra_Software_Devel/16   2/21/06 4:26p yuxiaz
 * PR19258: Need to increase memory for 1366x768 case. Added
 * eBufferFormat_HD in BVDC_Settings.
 *
 * Hydra_Software_Devel/15   2/13/06 11:09a pntruong
 * PR13275, PR19258: Back out uncessary feature.  Already available with
 * window source clipping.
 *
 * Hydra_Software_Devel/14   2/8/06 7:37p hongtaoz
 * PR19082: zero size buffer heap alloc would return NULL pointer; no need
 * to assert;
 *
 * Hydra_Software_Devel/13   2/6/06 2:32p yuxiaz
 * PR19258: Need to increase memory for 1366x768 case. Add
 * bBufferIs1080I_HD in BVDC_Settings to set HD buffer to 1080i or 768p.
 *
 * Hydra_Software_Devel/12   11/11/05 2:48p jessem
 * PR 17865: Modified BVDC_P_BufferHeap_Allocate to accommodate the use of
 * other available heaps when a particular heap is already being used.
 *
 * Hydra_Software_Devel/11   11/2/05 9:58a jessem
 * PR 17395: Added support for SD Pip buffer use.
 *
 * Hydra_Software_Devel/10   8/18/05 1:12p pntruong
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
 * Hydra_Software_Devel/9   4/1/05 5:13p pntruong
 * PR14686: Refresh mad mstart addresses on re-enable and fixed vdec 3d
 * comb mstart alignment.
 *
 * Hydra_Software_Devel/8   2/2/05 4:07p pntruong
 * PR12342: Removed Metrowerks build errors and warnings.
 *
 * Hydra_Software_Devel/7   1/12/05 9:15a yuxiaz
 * PR13313: Add more checking when free node.
 *
 * Hydra_Software_Devel/6   1/6/05 2:09p yuxiaz
 * PR13313: Added HD PIP buffer support in VDC.
 *
 * Hydra_Software_Devel/5   12/15/04 3:26p yuxiaz
 * PR13569: Add support for syncronization with XVD for HD_DVI. More work
 * needed.
 *
 * Hydra_Software_Devel/4   12/9/04 11:31a syang
 * PR 13092:  change ntsc_only buffer size to 483 lines for 480p
 *
 * Hydra_Software_Devel/3   12/1/04 4:24p yuxiaz
 * PR10855: Add support to use HD buffers for SD.
 *
 * Hydra_Software_Devel/2   11/30/04 11:19a yuxiaz
 * PR10855: Fixed compile warning.
 *
 * Hydra_Software_Devel/1   11/19/04 2:40p yuxiaz
 * PR 10855: Initial revision.
 *
 *
 ***************************************************************************/
#include "bstd.h"
#include "bkni.h"
#include "bvdc.h"
#include "bvdc_bufferheap_priv.h"
#include "bvdc_priv.h"
#include "bvdc_displayfmt_priv.h"


BDBG_MODULE(BVDC_HEAP);
BDBG_OBJECT_ID(BVDC_BFH);

/* TODO: Rework to support generic buffer broken down. */
#define BVDC_P_MEMC_ALIGNMENT  (1)
#define BVDC_P_PITCH_ALIGNMENT (1)
#define BVDC_P_HSIZE_ALIGNMENT (1)

/***************************************************************************
 *
 */
static void BVDC_P_BufferHeap_DumpHeapNode_isr
	( const BVDC_P_BufferHeapNode     *pBufferHeapNode )
{
#if (BDBG_DEBUG_BUILD)
	/* ulBufIndex (ulNodeCntPerParent eOrigBufHeapId) (Continous)
	 *       (Used - ulNumChildNodeUsed) at ulDeviceOffset (pvBufAddr) */
	BDBG_MSG(("\t- Node %2d (%s) (%s) (%s - %d) at 0x%lx (0x%lx)",
	pBufferHeapNode->ulBufIndex,
	BVDC_P_BUFFERHEAP_GET_HEAP_ID_NAME(pBufferHeapNode->eOrigBufHeapId),
	pBufferHeapNode->bContinous? "c" : "n",
	pBufferHeapNode->bUsed ? "x" : " ",
	pBufferHeapNode->ulNumChildNodeUsed,
	pBufferHeapNode->ulDeviceOffset,
	pBufferHeapNode->pvBufAddr));
#else
	BSTD_UNUSED(pBufferHeapNode); /* hush warning. */
#endif
	return;
}

/***************************************************************************
 *
 */
static  void BVDC_P_BufferHeap_DumpHeapInfo_isr
	( const BVDC_P_BufferHeap_Info    *pHeapInfo )
{
	BVDC_P_BufferHeapNode *pBufferHeapNode;

	BDBG_MSG((""));
	BVDC_P_BUF_MSG(("%s Heap",
		BVDC_P_BUFFERHEAP_GET_HEAP_ID_NAME(pHeapInfo->eBufHeapId)));

	if( pHeapInfo->pParentHeapInfo )
	{
		BVDC_P_BUF_MSG(("\tParent Heap        = %s",
			BVDC_P_BUFFERHEAP_GET_HEAP_ID_NAME(pHeapInfo->pParentHeapInfo->eBufHeapId)));
	}
	else
	{
		BVDC_P_BUF_MSG(("\tParent Heap        = NULL"));
	}

	if( pHeapInfo->pChildHeapInfo )
	{
		BVDC_P_BUF_MSG(("\tChild Heap         = %s",
			BVDC_P_BUFFERHEAP_GET_HEAP_ID_NAME(pHeapInfo->pChildHeapInfo->eBufHeapId)));
	}
	else
	{
		BVDC_P_BUF_MSG(("\tChild Heap         = NULL"));
	}

	BVDC_P_BUF_MSG(("\tulNodeCntPerParent = 0x%lx:", pHeapInfo->ulNodeCntPerParent));
	BVDC_P_BUF_MSG(("\tulPrimaryBufCnt    = %d:",    pHeapInfo->ulPrimaryBufCnt));
	BVDC_P_BUF_MSG(("\tulTotalBufCnt      = %d:",    pHeapInfo->ulTotalBufCnt));
	BVDC_P_BUF_MSG(("\tulBufSize          = %d:",    pHeapInfo->ulBufSize));
	BDBG_MSG(("\tpvHeapStartAddr    = 0x%lx:", pHeapInfo->pvHeapStartAddr));
	BDBG_MSG(("\tulBufUsed          = 0x%d:",  pHeapInfo->ulBufUsed));
	BDBG_MSG(("\tNode list:"));

	if( !pHeapInfo->pBufList )
	{
		return;
	}

	pBufferHeapNode = BLST_Q_FIRST(pHeapInfo->pBufList);
	while( pBufferHeapNode )
	{
		BVDC_P_BufferHeap_DumpHeapNode_isr(pBufferHeapNode);
		pBufferHeapNode= BLST_Q_NEXT(pBufferHeapNode, link);
	}

	return;
}

/***************************************************************************
 *
 */
static void BVDC_P_BufferHeap_Dump_isr
	( const BVDC_P_BufferHeapContext  *pBufferHeap )
{
	uint32_t i;

	for( i = 0; i < BVDC_P_BufferHeapId_eCount; i++ )
	{
		BVDC_P_BufferHeap_DumpHeapInfo_isr(&pBufferHeap->astHeapInfo[i]);
	}

	return;
}

/***************************************************************************
 *
 */
uint32_t BVDC_P_BufferHeap_GetHeapSize
	( const BFMT_VideoInfo       *pHeapFmtInfo,
	  BPXL_Format                 ePixelFmt,
	  uint32_t                    ulAdditionalLines,
	  bool                        bPip,
	  uint32_t                   *pulUnalignedBufSize,
	  uint32_t                   *pulWidth,
	  uint32_t                   *pulHeight )
{
	unsigned int uiPitch;
	uint32_t  ulWidth, ulHeight, ulBufSize, ulUnalignedBufSize;

	/* Get width and Height */
	ulWidth   = pHeapFmtInfo->ulWidth;
	ulHeight  = (pHeapFmtInfo->bInterlaced) ?
		((pHeapFmtInfo->ulHeight + 1)/ 2) : (pHeapFmtInfo->ulHeight);

	if( bPip )
	{
		ulWidth  /= 2;
		ulHeight /= 2;
	}
	else
	{
		ulHeight += ulAdditionalLines;
	}

	/* Get buf size wo alignment */
	/* Get pitch */
	BPXL_GetBytesPerNPixels(ePixelFmt, ulWidth, &uiPitch);
	ulUnalignedBufSize = uiPitch * ulHeight;

	/* Here starts alignment */
#if BVDC_P_HSIZE_ALIGNMENT
#if (BVDC_P_DCX_HSIZE_WORKAROUND || BVDC_P_MADR_HSIZE_WORKAROUND)
	/* the "+ 1" is for potential cap left/top align down, and
	 * the 4 is for DCX_HSIZE_WORKAROUND */
	ulWidth  = BVDC_P_ALIGN_UP(ulWidth  + 1, 4);
	ulHeight = BVDC_P_ALIGN_UP(ulHeight + 1, 2);
#endif
#endif

	/* Get pitch */
	BPXL_GetBytesPerNPixels(ePixelFmt, ulWidth, &uiPitch);
	/* Make sure 32 byte aligned */
#if BVDC_P_PITCH_ALIGNMENT
	uiPitch = BVDC_P_ALIGN_UP(uiPitch, BVDC_P_PITCH_ALIGN);
#endif
	ulBufSize = uiPitch * ulHeight;

#if (BVDC_P_MVFD_ALIGNMENT_WORKAROUND)
	ulBufSize += 4;
#endif

	/* Memc alignment requirement */
#if BVDC_P_MEMC_ALIGNMENT
	ulBufSize = BVDC_P_ALIGN_UP(ulBufSize, BVDC_P_HEAP_ALIGN_BYTES);
#endif

	if(pulWidth)
	{
		*pulWidth = ulWidth;
	}

	if(pulHeight)
	{
		*pulHeight = ulHeight;
	}

	if(pulUnalignedBufSize)
	{
		*pulUnalignedBufSize = ulUnalignedBufSize;
	}

	return ulBufSize;
}

/***************************************************************************
 * Fill out const properties:
 *    eBufHeapId, ulBufSize, ulPrimaryBufCnt, pParentHeapInfo,
 *    ulNodeCntPerParent
 */
static void BVDC_P_BufferHeap_GetHeapOrder
	( BVDC_P_BufferHeapContext      *pBufferHeap )
{
	uint32_t i = 0;

	uint32_t ulSDBufSize, ulSDBufWidth, ulSDBufHeight;
	uint32_t ulHDBufSize, ulHDBufWidth, ulHDBufHeight;
	uint32_t ul2HDBufSize, ul2HDBufWidth, ul2HDBufHeight;

	uint32_t ulSDPipBufSize, ulSDPipBufWidth, ulSDPipBufHeight;
	uint32_t ulHDPipBufSize, ulHDPipBufWidth, ulHDPipBufHeight;
	uint32_t ul2HDPipBufSize, ul2HDPipBufWidth, ul2HDPipBufHeight;

	uint32_t ulUnalignedSDBufSize, ulUnalignedHDBufSize, ulUnaligned2HDBufSize;
	uint32_t ulUnalignedSDPipBufSize, ulUnalignedHDPipBufSize, ulUnaligned2HDPipBufSize;

	/* Heap settings */
	const BVDC_Heap_Settings *pSettings = &pBufferHeap->stSettings;
	const BFMT_VideoInfo *pSDFmt = BFMT_GetVideoFormatInfoPtr(pSettings->eBufferFormat_SD);
	const BFMT_VideoInfo *pHDFmt = BFMT_GetVideoFormatInfoPtr(pSettings->eBufferFormat_HD);
	const BFMT_VideoInfo *p2HDFmt = BFMT_GetVideoFormatInfoPtr(pSettings->eBufferFormat_2HD);

	/* Get buffer size */
	ulSDBufSize = BVDC_P_BufferHeap_GetHeapSize(pSDFmt, pSettings->ePixelFormat_SD,
		pSettings->ulAdditionalLines_SD, false, &ulUnalignedSDBufSize,
		&ulSDBufWidth, &ulSDBufHeight);

	ulHDBufSize = BVDC_P_BufferHeap_GetHeapSize(pHDFmt, pSettings->ePixelFormat_HD,
		pSettings->ulAdditionalLines_HD, false, &ulUnalignedHDBufSize,
		&ulHDBufWidth, &ulHDBufHeight);

	ul2HDBufSize = BVDC_P_BufferHeap_GetHeapSize(p2HDFmt, pSettings->ePixelFormat_2HD,
		pSettings->ulAdditionalLines_2HD, false, &ulUnaligned2HDBufSize,
		&ul2HDBufWidth, &ul2HDBufHeight);

	ulSDPipBufSize = BVDC_P_BufferHeap_GetHeapSize(pSDFmt, pSettings->ePixelFormat_SD,
		0, true, &ulUnalignedSDPipBufSize, &ulSDPipBufWidth, &ulSDPipBufHeight);

	ulHDPipBufSize = BVDC_P_BufferHeap_GetHeapSize(pHDFmt, pSettings->ePixelFormat_HD,
		0, true, &ulUnalignedHDPipBufSize, &ulHDPipBufWidth, &ulHDPipBufHeight);

	ul2HDPipBufSize = BVDC_P_BufferHeap_GetHeapSize(p2HDFmt, pSettings->ePixelFormat_2HD,
		0, true, &ulUnaligned2HDPipBufSize, &ul2HDPipBufWidth, &ul2HDPipBufHeight);

	/* Remember the size so that we can assignment them correctly. */
	pBufferHeap->aulHeapSize[BVDC_P_BufferHeapId_eSD]      = ulSDBufSize;
	pBufferHeap->aulHeapSize[BVDC_P_BufferHeapId_eHD]      = ulHDBufSize;
	pBufferHeap->aulHeapSize[BVDC_P_BufferHeapId_e2HD]     = ul2HDBufSize;
	pBufferHeap->aulHeapSize[BVDC_P_BufferHeapId_eSD_Pip]  = ulSDPipBufSize;
	pBufferHeap->aulHeapSize[BVDC_P_BufferHeapId_eHD_Pip]  = ulHDPipBufSize;
	pBufferHeap->aulHeapSize[BVDC_P_BufferHeapId_e2HD_Pip] = ul2HDPipBufSize;

	/* Make sure SD_Pip  < SD, HD_Pip  < HD and 2HD_Pip < 2HD */
	BDBG_ASSERT(ulSDPipBufSize  < ulSDBufSize);
	BDBG_ASSERT(ulHDPipBufSize  < ulHDBufSize);
	BDBG_ASSERT(ul2HDPipBufSize < ul2HDBufSize);

	/* Sort the order of buffers */
	pBufferHeap->astHeapInfo[i].eBufHeapId   = BVDC_P_BufferHeapId_e2HD;
	pBufferHeap->astHeapInfo[i].ulPrimaryBufCnt = pSettings->ulBufferCnt_2HD;
	pBufferHeap->astHeapInfo[i].ulWidth      = ul2HDBufWidth;
	pBufferHeap->astHeapInfo[i].ulHeight     = ul2HDBufHeight;
	pBufferHeap->astHeapInfo[i].ulBufSize    = ul2HDBufSize;
	pBufferHeap->astHeapInfo[i].ulBufSizeByFmt = ulUnaligned2HDBufSize;
	pBufferHeap->aulIndex[BVDC_P_BufferHeapId_e2HD] = i++;

	if( ulHDBufSize > ul2HDPipBufSize )
	{
		/* 2HD > HD > 2HD_Pip */
		pBufferHeap->astHeapInfo[i].eBufHeapId   = BVDC_P_BufferHeapId_eHD;
		pBufferHeap->astHeapInfo[i].ulPrimaryBufCnt = pSettings->ulBufferCnt_HD;
		pBufferHeap->astHeapInfo[i].ulWidth      = ulHDBufWidth;
		pBufferHeap->astHeapInfo[i].ulHeight     = ulHDBufHeight;
		pBufferHeap->astHeapInfo[i].ulBufSize    = ulHDBufSize;
		pBufferHeap->astHeapInfo[i].ulBufSizeByFmt = ulUnalignedHDBufSize;
		pBufferHeap->aulIndex[BVDC_P_BufferHeapId_eHD] = i++;

		pBufferHeap->astHeapInfo[i].eBufHeapId   = BVDC_P_BufferHeapId_e2HD_Pip;
		pBufferHeap->astHeapInfo[i].ulPrimaryBufCnt = pSettings->ulBufferCnt_2HD_Pip;
		pBufferHeap->astHeapInfo[i].ulWidth      = ul2HDPipBufWidth;
		pBufferHeap->astHeapInfo[i].ulHeight     = ul2HDPipBufHeight;
		pBufferHeap->astHeapInfo[i].ulBufSize    = ul2HDPipBufSize;
		pBufferHeap->astHeapInfo[i].ulBufSizeByFmt = ulUnaligned2HDPipBufSize;
		pBufferHeap->aulIndex[BVDC_P_BufferHeapId_e2HD_Pip] = i++;
	}
	else
	{
		/* 2HD > 2HD_Pip > HD */
		pBufferHeap->astHeapInfo[i].eBufHeapId   = BVDC_P_BufferHeapId_e2HD_Pip;
		pBufferHeap->astHeapInfo[i].ulPrimaryBufCnt = pSettings->ulBufferCnt_2HD_Pip;
		pBufferHeap->astHeapInfo[i].ulWidth      = ul2HDPipBufWidth;
		pBufferHeap->astHeapInfo[i].ulHeight     = ul2HDPipBufHeight;
		pBufferHeap->astHeapInfo[i].ulBufSize    = ul2HDPipBufSize;
		pBufferHeap->astHeapInfo[i].ulBufSizeByFmt = ulUnaligned2HDPipBufSize;
		pBufferHeap->aulIndex[BVDC_P_BufferHeapId_e2HD_Pip] = i++;

		pBufferHeap->astHeapInfo[i].eBufHeapId   = BVDC_P_BufferHeapId_eHD;
		pBufferHeap->astHeapInfo[i].ulPrimaryBufCnt = pSettings->ulBufferCnt_HD;
		pBufferHeap->astHeapInfo[i].ulWidth      = ulHDBufWidth;
		pBufferHeap->astHeapInfo[i].ulHeight     = ulHDBufHeight;
		pBufferHeap->astHeapInfo[i].ulBufSize    = ulHDBufSize;
		pBufferHeap->astHeapInfo[i].ulBufSizeByFmt = ulUnalignedHDBufSize;
		pBufferHeap->aulIndex[BVDC_P_BufferHeapId_eHD] = i++;
	}

	if( ulSDBufSize > ulHDPipBufSize )
	{
		/* SD > HD_Pip > SD_Pip */
		pBufferHeap->astHeapInfo[i].eBufHeapId   = BVDC_P_BufferHeapId_eSD;
		pBufferHeap->astHeapInfo[i].ulPrimaryBufCnt = pSettings->ulBufferCnt_SD;
		pBufferHeap->astHeapInfo[i].ulWidth      = ulSDBufWidth;
		pBufferHeap->astHeapInfo[i].ulHeight     = ulSDBufHeight;
		pBufferHeap->astHeapInfo[i].ulBufSize    = ulSDBufSize;
		pBufferHeap->astHeapInfo[i].ulBufSizeByFmt = ulUnalignedSDBufSize;
		pBufferHeap->aulIndex[BVDC_P_BufferHeapId_eSD] = i++;

		pBufferHeap->astHeapInfo[i].eBufHeapId   = BVDC_P_BufferHeapId_eHD_Pip;
		pBufferHeap->astHeapInfo[i].ulPrimaryBufCnt = pSettings->ulBufferCnt_HD_Pip;
		pBufferHeap->astHeapInfo[i].ulWidth      = ulHDPipBufWidth;
		pBufferHeap->astHeapInfo[i].ulHeight     = ulHDPipBufHeight;
		pBufferHeap->astHeapInfo[i].ulBufSize    = ulHDPipBufSize;
		pBufferHeap->astHeapInfo[i].ulBufSizeByFmt = ulUnalignedHDPipBufSize;
		pBufferHeap->aulIndex[BVDC_P_BufferHeapId_eHD_Pip] = i++;
	}
	else
	{
		/* HD_Pip > SD > SD_Pip */
		pBufferHeap->astHeapInfo[i].eBufHeapId   = BVDC_P_BufferHeapId_eHD_Pip;
		pBufferHeap->astHeapInfo[i].ulPrimaryBufCnt = pSettings->ulBufferCnt_HD_Pip;
		pBufferHeap->astHeapInfo[i].ulWidth      = ulHDPipBufWidth;
		pBufferHeap->astHeapInfo[i].ulHeight     = ulHDPipBufHeight;
		pBufferHeap->astHeapInfo[i].ulBufSize    = ulHDPipBufSize;
		pBufferHeap->astHeapInfo[i].ulBufSizeByFmt = ulUnalignedHDPipBufSize;
		pBufferHeap->aulIndex[BVDC_P_BufferHeapId_eHD_Pip] = i++;

		pBufferHeap->astHeapInfo[i].eBufHeapId   = BVDC_P_BufferHeapId_eSD;
		pBufferHeap->astHeapInfo[i].ulPrimaryBufCnt = pSettings->ulBufferCnt_SD;
		pBufferHeap->astHeapInfo[i].ulWidth      = ulSDBufWidth;
		pBufferHeap->astHeapInfo[i].ulHeight     = ulSDBufHeight;
		pBufferHeap->astHeapInfo[i].ulBufSize    = ulSDBufSize;
		pBufferHeap->astHeapInfo[i].ulBufSizeByFmt = ulUnalignedSDBufSize;
		pBufferHeap->aulIndex[BVDC_P_BufferHeapId_eSD] = i++;
	}

	pBufferHeap->astHeapInfo[i].eBufHeapId   = BVDC_P_BufferHeapId_eSD_Pip;
	pBufferHeap->astHeapInfo[i].ulPrimaryBufCnt = pSettings->ulBufferCnt_SD_Pip;
	pBufferHeap->astHeapInfo[i].ulWidth      = ulSDPipBufWidth;
	pBufferHeap->astHeapInfo[i].ulHeight     = ulSDPipBufHeight;
	pBufferHeap->astHeapInfo[i].ulBufSize    = ulSDPipBufSize;
	pBufferHeap->astHeapInfo[i].ulBufSizeByFmt = ulUnalignedSDPipBufSize;
	pBufferHeap->aulIndex[BVDC_P_BufferHeapId_eSD_Pip] = i++;

	for( i = 0; i < BVDC_P_BufferHeapId_eCount; i++ )
	{
		/* Initalize settings */
		pBufferHeap->astHeapInfo[i].ulBufUsed     = 0;
		pBufferHeap->astHeapInfo[i].hBufferHeap   = pBufferHeap;
		pBufferHeap->astHeapInfo[i].ulTotalBufCnt = pBufferHeap->astHeapInfo[i].ulPrimaryBufCnt;

		if( i == 0 )
		{
			pBufferHeap->astHeapInfo[i].pParentHeapInfo    = NULL;
			pBufferHeap->astHeapInfo[i].ulNodeCntPerParent = 0;
		}
		else
		{
			pBufferHeap->astHeapInfo[i].pParentHeapInfo
				= &pBufferHeap->astHeapInfo[i-1];
			pBufferHeap->astHeapInfo[i].ulNodeCntPerParent
				= BVDC_P_MAX(
				pBufferHeap->astHeapInfo[i-1].ulBufSize / pBufferHeap->astHeapInfo[i].ulBufSize,
				pBufferHeap->astHeapInfo[i-1].ulBufSizeByFmt / pBufferHeap->astHeapInfo[i].ulBufSizeByFmt);
		}

		if( i == BVDC_P_BufferHeapId_eCount - 1 )
		{
			pBufferHeap->astHeapInfo[i].pChildHeapInfo = NULL;
		}
		else
		{
			pBufferHeap->astHeapInfo[i].pChildHeapInfo = &pBufferHeap->astHeapInfo[i+1];
		}
	}

	/* Adjust buf size for alignment: from bottom up */
	for( i = BVDC_P_BufferHeapId_eCount - 1; i > 0; i-- )
	{
		if(pBufferHeap->astHeapInfo[i].pParentHeapInfo)
		{
			uint32_t   ulNewBufSize;

			/* TODO: adjust width and height? ulWidth and ulHeigh are only
			 * used in bvdc_bufferheap_priv.c, not need to adjust now */
			ulNewBufSize = pBufferHeap->astHeapInfo[i].ulNodeCntPerParent *
				pBufferHeap->astHeapInfo[i].ulBufSize;

#if BVDC_P_MEMC_ALIGNMENT
			ulNewBufSize = BVDC_P_ALIGN_UP(ulNewBufSize, BVDC_P_HEAP_ALIGN_BYTES);
#endif

			pBufferHeap->astHeapInfo[i].pParentHeapInfo->ulBufSize =
				BVDC_P_MAX(ulNewBufSize,
				pBufferHeap->astHeapInfo[i].pParentHeapInfo->ulBufSize);

		}
	}

	return;
}

/***************************************************************************
 *
 */
static BERR_Code BVDC_P_BufferHeap_BuildPrimaryNodeList
	( BMEM_Heap_Handle             hMemory,
	  BVDC_P_BufferHeap_Info      *pHeapInfo )
{
	void                  *pvBufAddr = pHeapInfo->pvHeapStartAddr;
	uint32_t               i, ulDeviceOffset;
	uint32_t               ulBufSize = pHeapInfo->ulBufSize;
	uint32_t               ulPrimaryBufCnt = pHeapInfo->ulPrimaryBufCnt;
	BERR_Code              err = BERR_SUCCESS;
	BVDC_P_BufferHeapNode *pBufferHeapNode;

	if (ulPrimaryBufCnt)
	{
		/* Convert address to device offset from the original device base. */
		err = BMEM_Heap_ConvertAddressToOffset(hMemory, pvBufAddr, &ulDeviceOffset);
		if (err != BERR_SUCCESS)
		{
			return BERR_TRACE(err);
		}
	}

	for(i = 0; i < ulPrimaryBufCnt; i++)
	{
		pBufferHeapNode = (BVDC_P_BufferHeapNode *)BKNI_Malloc(
			sizeof(BVDC_P_BufferHeapNode));

		if(!pBufferHeapNode)
		{
			/* Free the already allocated memory */
			BMEM_Heap_Free(hMemory, pHeapInfo->pvHeapStartAddr);

			while(i--)
			{
				pBufferHeapNode = BLST_Q_FIRST(pHeapInfo->pBufList);
				BLST_Q_REMOVE_HEAD(pHeapInfo->pBufList, link);
				BKNI_Free(pBufferHeapNode);
			}
			BKNI_Free(pHeapInfo->pBufList);
			return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
		}

		/* Clear out, insert it into the list. */
		BKNI_Memset((void*)pBufferHeapNode, 0x0, sizeof(BVDC_P_BufferHeapNode));

		/* Initialize fields. */
		pBufferHeapNode->ulBufIndex   = i;
		pBufferHeapNode->pHeapInfo    = pHeapInfo;
		pBufferHeapNode->bUsed        = false;
		pBufferHeapNode->pvBufAddr    = pvBufAddr;
		pBufferHeapNode->ulDeviceOffset = ulDeviceOffset;
		pBufferHeapNode->pParentNode    = NULL;
		pBufferHeapNode->pFirstChildNode = NULL;
		pBufferHeapNode->ulNumChildNodeUsed = 0;
		pBufferHeapNode->bContinous = true;
		pBufferHeapNode->eOrigBufHeapId = pHeapInfo->eBufHeapId;

		BLST_Q_INSERT_TAIL(pHeapInfo->pBufList, pBufferHeapNode, link);
		ulDeviceOffset += ulBufSize;
		pvBufAddr = (uint8_t *)pvBufAddr + ulBufSize;
		BDBG_ASSERT(BVDC_P_IS_ALIGN(pvBufAddr,BVDC_P_HEAP_ALIGN_BYTES));
	}

	return err;
}

/***************************************************************************
 *
 */
static BERR_Code BVDC_P_BufferHeap_BuildChildNodeList
	( BMEM_Heap_Handle             hMemory,
	  BVDC_P_BufferHeap_Info      *pHeapInfo )
{
	uint32_t                i;
	uint32_t                ulBufSize, ulDeviceOffset;
	BVDC_P_BufferHeapNode  *pBufferHeapNode, *pParentHeapNode;
	BVDC_P_BufferHeap_Info *pParentHeapInfo = pHeapInfo->pParentHeapInfo;
	void                   *pvBufAddr;

	if( !pParentHeapInfo )
	{
		return BERR_SUCCESS;
	}

	ulBufSize = pHeapInfo->ulBufSize;
	pParentHeapNode = BLST_Q_FIRST(pParentHeapInfo->pBufList);

	while( pParentHeapNode )
	{
		/* Break a larger buffer into smaller buffers */
		pvBufAddr      = pParentHeapNode->pvBufAddr;
		ulDeviceOffset = pParentHeapNode->ulDeviceOffset;

		for(i = 0; i < pHeapInfo->ulNodeCntPerParent; i++ )
		{
			pBufferHeapNode = (BVDC_P_BufferHeapNode *)BKNI_Malloc(
				sizeof(BVDC_P_BufferHeapNode));

			if(!pBufferHeapNode)
			{
				i = pHeapInfo->ulTotalBufCnt;
				while(i--)
				{
					pBufferHeapNode = BLST_Q_FIRST(pHeapInfo->pBufList);
					BLST_Q_REMOVE_HEAD(pHeapInfo->pBufList, link);
					BKNI_Free(pBufferHeapNode);
				}
				BKNI_Free(pHeapInfo->pBufList);
				BMEM_Heap_Free(hMemory, pHeapInfo->pvHeapStartAddr);
				return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
			}

			/* Clear out, insert it into the list. */
			BKNI_Memset((void*)pBufferHeapNode, 0x0, sizeof(BVDC_P_BufferHeapNode));

			/* Initialize fields. */
			pBufferHeapNode->ulBufIndex   = pHeapInfo->ulTotalBufCnt;
			pBufferHeapNode->pHeapInfo    = pHeapInfo;
			pBufferHeapNode->bUsed        = false;
			pBufferHeapNode->pvBufAddr    = pvBufAddr;
			pBufferHeapNode->ulDeviceOffset = ulDeviceOffset;
			pBufferHeapNode->pParentNode    = pParentHeapNode;
			pBufferHeapNode->eOrigBufHeapId = pParentHeapNode->eOrigBufHeapId;

			if( i == 0 )
			{
				pParentHeapNode->pFirstChildNode = pBufferHeapNode;
				pBufferHeapNode->bContinous = pParentHeapNode->bContinous;
				if( (pHeapInfo->ulPrimaryBufCnt != 0) &&
				    (pBufferHeapNode->ulBufIndex == pHeapInfo->ulPrimaryBufCnt) )
				{
					pBufferHeapNode->bContinous = false;
				}
			}
			else
			{
				pBufferHeapNode->bContinous = true;
			}

			BLST_Q_INSERT_TAIL(pHeapInfo->pBufList, pBufferHeapNode, link);
			ulDeviceOffset += ulBufSize;
			pvBufAddr = (uint8_t *)pvBufAddr + ulBufSize;
			BDBG_ASSERT(BVDC_P_IS_ALIGN(pvBufAddr,BVDC_P_HEAP_ALIGN_BYTES));

			pHeapInfo->ulTotalBufCnt++;
		}
		pParentHeapNode = BLST_Q_NEXT(pParentHeapNode, link);
	}

	return BERR_SUCCESS;
}


/***************************************************************************
 *
 */
static BERR_Code BVDC_P_BufferHeap_CreateHeapInfo
	( BMEM_Heap_Handle             hMemory,
	  BVDC_P_BufferHeap_Info      *pHeapInfo )
{
	void                  *pvBufAddr;
	uint32_t               ulBufSize = pHeapInfo->ulBufSize;
	uint32_t               ulPrimaryBufCnt = pHeapInfo->ulPrimaryBufCnt;
	BERR_Code              err = BERR_SUCCESS;

	BDBG_MSG(("Create %7s Heap size: %8d (%d, %d)",
		BVDC_P_BUFFERHEAP_GET_HEAP_ID_NAME(pHeapInfo->eBufHeapId),
		ulBufSize, pHeapInfo->ulWidth, pHeapInfo->ulHeight));

	/* (1) Create device memory */
	pHeapInfo->pvHeapStartAddr = BMEM_Heap_AllocAligned(hMemory,
		ulBufSize * ulPrimaryBufCnt, BVDC_P_HEAP_MEMORY_ALIGNMENT, 0);
	if( !pHeapInfo->pvHeapStartAddr && (0 != ulPrimaryBufCnt) )
	{
		BDBG_ERR(("Not enough device memory[%d]!", ulBufSize * ulPrimaryBufCnt));
		BDBG_ASSERT(0);
		return BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
	}
	pvBufAddr = pHeapInfo->pvHeapStartAddr;

	/* (2) Create buffer heap node list */
	pHeapInfo->pBufList =
		(BVDC_P_BufferHeap_Head *)BKNI_Malloc(sizeof(BVDC_P_BufferHeap_Head));
	if( !pHeapInfo->pBufList )
	{
		BMEM_Heap_Free(hMemory, pHeapInfo->pvHeapStartAddr);
		return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
	}
	BLST_Q_INIT(pHeapInfo->pBufList);

	/* (3) Create buffer heap primitive nodes of its own */
	BVDC_P_BufferHeap_BuildPrimaryNodeList(hMemory, pHeapInfo);

	/* (4) Create buffer heap child nodes */
	BVDC_P_BufferHeap_BuildChildNodeList(hMemory, pHeapInfo);

	return err;
}


/***************************************************************************
 *
 */
static BERR_Code BVDC_P_BufferHeap_DestroyHeapInfo
	( BMEM_Heap_Handle             hMemory,
	  BVDC_P_BufferHeap_Info      *pHeapInfo )
{
	uint32_t               i;
	BVDC_P_BufferHeapNode *pBufferHeapNode;

	/* [3/4] Free buffer heap nodes */
	for( i = 0; i < pHeapInfo->ulTotalBufCnt; i++ )
	{
		pBufferHeapNode = BLST_Q_FIRST(pHeapInfo->pBufList);
		BLST_Q_REMOVE_HEAD(pHeapInfo->pBufList, link);
		BKNI_Free(pBufferHeapNode);
	}

	/* [2] Free buffer heap node list */
	BKNI_Free((void*)pHeapInfo->pBufList);

	/* [1] Free device memory */
	BMEM_Heap_Free(hMemory, pHeapInfo->pvHeapStartAddr);

	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
 static void BVDC_P_BufferHeap_InitHeapInfo
	( BVDC_P_BufferHeap_Info      *pHeapInfo )
{
	uint32_t                  i;
	BVDC_P_BufferHeapNode    *pBufferHeapNode;

	pBufferHeapNode = BLST_Q_FIRST(pHeapInfo->pBufList);
	for(i = 0; i < pHeapInfo->ulTotalBufCnt; i++)
	{
		pBufferHeapNode->bUsed = false;
		pBufferHeapNode= BLST_Q_NEXT(pBufferHeapNode, link);
	}

	pHeapInfo->ulBufUsed = 0;
	return;
}

/***************************************************************************
 *
 */
static BERR_Code BVDC_P_BufferHeap_MarkParentNode_isr
	( BVDC_P_BufferHeap_Info      *pParentHeapInfo,
	  BVDC_P_BufferHeapNode       *pParentHeapNode )
{
	if( !pParentHeapNode || !pParentHeapInfo )
	{
		return BERR_SUCCESS;
	}

	pParentHeapNode->ulNumChildNodeUsed++;
	if( !pParentHeapNode->bUsed )
	{
		pParentHeapNode->bUsed = true;
		pParentHeapInfo->ulBufUsed++;

		BDBG_MSG(("Mark parent node: "));
		BVDC_P_BufferHeap_DumpHeapNode_isr(pParentHeapNode);

		BVDC_P_BufferHeap_MarkParentNode_isr(
			pParentHeapInfo->pParentHeapInfo,
			pParentHeapNode->pParentNode);
	}

	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
static BERR_Code BVDC_P_BufferHeap_MarkChildNodes_isr
	( BVDC_P_BufferHeap_Info      *pChildHeapInfo,
	  BVDC_P_BufferHeapNode       *pFirstChildNode )
{
	uint32_t                 i = 0;
	BVDC_P_BufferHeapNode   *pTempNode = pFirstChildNode;

	if( !pFirstChildNode )
	{
		return BERR_SUCCESS;
	}

	BDBG_MSG(("Mark child nodes"));
	while(i++ < pChildHeapInfo->ulNodeCntPerParent)
	{
		pTempNode->bUsed = true;
		pChildHeapInfo->ulBufUsed++;
		pTempNode->pParentNode->ulNumChildNodeUsed++;
		BVDC_P_BufferHeap_DumpHeapNode_isr(pTempNode);

		BVDC_P_BufferHeap_MarkChildNodes_isr(pChildHeapInfo->pChildHeapInfo,
			pTempNode->pFirstChildNode);

		pTempNode = BLST_Q_NEXT(pTempNode, link);
	}

	return BERR_SUCCESS;
}

/***************************************************************************
 * Mark a Heap node and its parent node and child nodes to be used.
 * This is called after a heap node is found
 */
static BERR_Code BVDC_P_BufferHeap_MarkNode_isr
	( BVDC_P_BufferHeap_Info      *pHeapInfo,
	  BVDC_P_BufferHeapNode       *pBufferHeapNode )
{
	/* Mark the found node */
	pBufferHeapNode->bUsed = true;
	pHeapInfo->ulBufUsed++;
	BDBG_MSG(("Mark node"));
	BVDC_P_BufferHeap_DumpHeapNode_isr(pBufferHeapNode);

	/* Mark parent node and parent heap */
	BVDC_P_BufferHeap_MarkParentNode_isr(pHeapInfo->pParentHeapInfo,
		pBufferHeapNode->pParentNode);

	/* Mark child node and child heap */
	BVDC_P_BufferHeap_MarkChildNodes_isr(pHeapInfo->pChildHeapInfo,
		pBufferHeapNode->pFirstChildNode);

	return BERR_SUCCESS;
}

/***************************************************************************
 * Unmark a Heap node and its parent node to be used.
 * This is called after a heap node is freed
 */
static BERR_Code BVDC_P_BufferHeap_UnmarkParentNode_isr
	( BVDC_P_BufferHeapNode       *pParentHeapNode )
{
	BVDC_P_BufferHeap_Info  *pParentHeapInfo;

	if( !pParentHeapNode )
	{
		return BERR_SUCCESS;
	}

	pParentHeapInfo = pParentHeapNode->pHeapInfo;
	pParentHeapNode->ulNumChildNodeUsed--;
	BDBG_MSG(("Check parent node: "));
	BVDC_P_BufferHeap_DumpHeapNode_isr(pParentHeapNode);

	if( pParentHeapNode->ulNumChildNodeUsed == 0 )
	{
		if( pParentHeapNode->bUsed )
		{
			pParentHeapNode->bUsed = false;
			pParentHeapInfo->ulBufUsed--;
			BDBG_MSG(("Unmark parent node: "));
			BVDC_P_BufferHeap_DumpHeapNode_isr(pParentHeapNode);
		}

		BVDC_P_BufferHeap_UnmarkParentNode_isr(pParentHeapNode->pParentNode);
	}

	return BERR_SUCCESS;
}

/***************************************************************************
 * Unmark a Heap node and its parent node to be used.
 * This is called after a heap node is freed
 */
static BERR_Code BVDC_P_BufferHeap_UnmarkChildNodes_isr
	( BVDC_P_BufferHeapNode       *pFirstChildNode )
{
	uint32_t                 i = 0;
	BVDC_P_BufferHeap_Info  *pChildHeapInfo;
	BVDC_P_BufferHeapNode   *pTempNode = pFirstChildNode;

	if( !pFirstChildNode )
	{
		return BERR_SUCCESS;
	}

	if( pFirstChildNode )
	{
		BDBG_MSG(("Unmark child nodes"));
		pChildHeapInfo = pFirstChildNode->pHeapInfo;
		while(i++ < pChildHeapInfo->ulNodeCntPerParent)
		{
			pTempNode->bUsed = false;
			pChildHeapInfo->ulBufUsed--;
			pTempNode->pParentNode->ulNumChildNodeUsed--;
			BVDC_P_BufferHeap_DumpHeapNode_isr(pTempNode);
			BVDC_P_BufferHeap_UnmarkChildNodes_isr(pTempNode->pFirstChildNode);
			pTempNode = BLST_Q_NEXT(pTempNode, link);
		}
	}

	return BERR_SUCCESS;
}


/***************************************************************************
 * Unmark a Heap node and its parent node to be used.
 * This is called after a heap node is freed
 */
static BERR_Code BVDC_P_BufferHeap_UnmarkNode_isr
	( BVDC_P_BufferHeapNode       *pBufferHeapNode )
{
	BVDC_P_BufferHeap_Info  *pHeapInfo = pBufferHeapNode->pHeapInfo;

	/* Unmark the found node */
	pBufferHeapNode->bUsed = false;
	/*
	pBufferHeapNode->ulNumChildNodeUsed = 0;
	*/
	pHeapInfo->ulBufUsed--;
	BDBG_MSG(("UnMark node"));
	BVDC_P_BufferHeap_DumpHeapNode_isr(pBufferHeapNode);

	/* Unmark parent node and parent heap */
	BVDC_P_BufferHeap_UnmarkParentNode_isr(pBufferHeapNode->pParentNode);

	/* Unmark child node and child heap */
	BVDC_P_BufferHeap_UnmarkChildNodes_isr(pBufferHeapNode->pFirstChildNode);

	return BERR_SUCCESS;
}


/***************************************************************************
 *
 */
static BERR_Code BVDC_P_BufferHeap_AllocateNodes_isr
	( BVDC_P_BufferHeap_Info      *pHeapInfo,
	  BVDC_P_HeapNodePtr          *apHeapNode,
	  uint32_t                     ulCount,
	  BVDC_P_BufferHeapId          eBufferHeapIdPrefer )
{
	bool                     bMatchNode = false;
	uint32_t                 i;
	BERR_Code                err = BERR_SUCCESS;
	BVDC_P_BufferHeapNode   *pBufferHeapNode;
	BVDC_P_BufferHeap_Head  *pBufList;

	BDBG_ASSERT(apHeapNode);

	/* Get the first node in list */
	pBufList = pHeapInfo->pBufList;
	pBufferHeapNode = BLST_Q_FIRST(pBufList);

	for( i = 0; i < ulCount; i++ )
	{
		pBufferHeapNode = BLST_Q_FIRST(pBufList);

		/* Can use any node for BVDC_P_BufferHeapId_eUnknown */
		if(pBufferHeapNode)
		{
			bMatchNode = (eBufferHeapIdPrefer == BVDC_P_BufferHeapId_eUnknown) |
			             (eBufferHeapIdPrefer == pBufferHeapNode->eOrigBufHeapId);
		}

		while( pBufferHeapNode && (pBufferHeapNode->bUsed || !bMatchNode))
		{
			pBufferHeapNode= BLST_Q_NEXT(pBufferHeapNode, link);

			if(pBufferHeapNode)
			{
				/* Can use any node for BVDC_P_BufferHeapId_eUnknown */
				bMatchNode = (eBufferHeapIdPrefer == BVDC_P_BufferHeapId_eUnknown) |
				             (eBufferHeapIdPrefer == pBufferHeapNode->eOrigBufHeapId);
			}
		}

		/* Find the node */
		if( pBufferHeapNode )
		{
			BDBG_MSG(("Find an available node:"));
			BVDC_P_BufferHeap_DumpHeapNode_isr(pBufferHeapNode);

			/* Mark the node and its parent node */
			BVDC_P_BufferHeap_MarkNode_isr(pHeapInfo, pBufferHeapNode);
			apHeapNode[i] = pBufferHeapNode;
		}
		else if(eBufferHeapIdPrefer != BVDC_P_BufferHeapId_eUnknown)
		{
			BDBG_MSG(("Can not find prefered %s for %s buffers ",
				BVDC_P_BUFFERHEAP_GET_HEAP_ID_NAME(eBufferHeapIdPrefer),
				BVDC_P_BUFFERHEAP_GET_HEAP_ID_NAME(pHeapInfo->eBufHeapId)));

			/* unmark nodes previously marked */
			while(i)
			{
				BVDC_P_BufferHeap_UnmarkNode_isr(apHeapNode[--i]);
			}
			return BERR_INVALID_PARAMETER;
		}
		else
		{
			BDBG_ERR(("Can not find an available node"));
			/* unmark nodes previously marked */
			while(i)
			{
				BVDC_P_BufferHeap_UnmarkNode_isr(apHeapNode[--i]);
			}
			return BERR_TRACE(BERR_INVALID_PARAMETER);
		}
	}

	return err;

}

/***************************************************************************
 *
 */
static BERR_Code BVDC_P_BufferHeap_FreeNode_isr
	( BVDC_P_HeapNodePtr           pHeapNode )
{
	BVDC_P_BufferHeapNode   *pTempNode;
	BVDC_P_BufferHeap_Info  *pHeapInfo = pHeapNode->pHeapInfo;

	BDBG_MSG(("Free node:"));
	BVDC_P_BufferHeap_DumpHeapNode_isr(pHeapNode);

	/* Find the node first */
	pTempNode = BLST_Q_FIRST(pHeapInfo->pBufList);
	while( pTempNode &&
		 (pTempNode->ulDeviceOffset != pHeapNode->ulDeviceOffset) )
	{
		pTempNode= BLST_Q_NEXT(pTempNode, link);
	}

	if( pTempNode && pTempNode->bUsed )
	{
		/* Unmark the node and its parent node */
		BVDC_P_BufferHeap_UnmarkNode_isr(pTempNode);
	}
	else
	{
		BDBG_ERR(("Can not find a matching node"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	BDBG_ASSERT(pHeapInfo->ulBufUsed <= pHeapInfo->ulTotalBufCnt);

	return BERR_SUCCESS;
}


/***************************************************************************
 *
 */
static BERR_Code BVDC_P_BufferHeap_AllocateContNodes_isr
	( BVDC_P_BufferHeap_Info          *pHeapInfo,
	  BVDC_P_HeapNodePtr              *pHeapNode,
	  uint32_t                         ulCount )
{
	bool                     bFound = false, bNext = true;
	uint32_t                 i = ulCount;
	BERR_Code                err = BERR_SUCCESS;
	BVDC_P_BufferHeapNode   *pBufferHeapNodeStart, *pBufferHeapTempNode;
	BVDC_P_BufferHeap_Head  *pBufList;

	BDBG_ASSERT(pHeapNode);

	/* Get the first node in list */
	pBufList = pHeapInfo->pBufList;
	pBufferHeapNodeStart = BLST_Q_FIRST(pBufList);

	while( pBufferHeapNodeStart && !bFound )
	{
		/* Get the first node */
		while( pBufferHeapNodeStart && pBufferHeapNodeStart->bUsed )
		{
			pBufferHeapNodeStart= BLST_Q_NEXT(pBufferHeapNodeStart, link);
		}

		if( !pBufferHeapNodeStart )
		{
			break;
		}

		BDBG_MSG(("Find first available node:"));
		BVDC_P_BufferHeap_DumpHeapNode_isr(pBufferHeapNodeStart);

		bNext = true;
		i = ulCount - 1;
		pBufferHeapTempNode = pBufferHeapNodeStart;
		while( i-- )
		{
			pBufferHeapTempNode= BLST_Q_NEXT(pBufferHeapTempNode, link);
			if( !pBufferHeapTempNode )
			{
				bNext  = false;
				break;
			}

			if( pBufferHeapTempNode->bUsed )
			{
				BDBG_MSG(("Find a non-available node:"));
				BVDC_P_BufferHeap_DumpHeapNode_isr(pBufferHeapTempNode);
				bNext  = false;
				break;
			}
			else if( !pBufferHeapTempNode->bContinous )
			{
				BDBG_MSG(("Find a non-continous node:"));
				BVDC_P_BufferHeap_DumpHeapNode_isr(pBufferHeapTempNode);
				bNext  = false;
				break;
			}
			else
			{
				BDBG_MSG(("Find next available  node:"));
				BVDC_P_BufferHeap_DumpHeapNode_isr(pBufferHeapTempNode);
			}
		}

		if( !bNext  )
		{
			BDBG_MSG(("Restart ..."));
			pBufferHeapNodeStart = pBufferHeapTempNode;
		}
		else
		{
			bFound = true;
		}

	}

	if( pBufferHeapNodeStart && bFound )
	{
		BDBG_MSG((" "));
		BDBG_MSG(("Find %d available nodes ", ulCount));
		pBufferHeapTempNode = pBufferHeapNodeStart;
		for( i = 0; i < ulCount; i++ )
		{
			/* Mark the node and its parent node */
			BVDC_P_BufferHeap_MarkNode_isr(pHeapInfo, pBufferHeapTempNode);

			pBufferHeapTempNode= BLST_Q_NEXT(pBufferHeapTempNode, link);
		}

		*pHeapNode = pBufferHeapNodeStart;

	}
	else
	{
		BDBG_ERR(("Can not find an available node "));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	return err;
}

/***************************************************************************
 *
 */
static BERR_Code BVDC_P_BufferHeap_FreeContNodes_isr
	( BVDC_P_HeapNodePtr              *pHeapNode,
	  uint32_t                         ulCount )
{
	uint32_t                 i;
	BVDC_P_BufferHeap_Head  *pBufList;
	BVDC_P_BufferHeapNode   *pTempNode;
	BVDC_P_BufferHeap_Info  *pHeapInfo;

	if( !pHeapNode )
		return BERR_SUCCESS;

	pHeapInfo = pHeapNode[0]->pHeapInfo;
	pBufList = pHeapInfo->pBufList;

	/* Find the node first */
	pTempNode = BLST_Q_FIRST(pBufList);
	while( pTempNode &&
		 (pTempNode->ulDeviceOffset != (*pHeapNode)->ulDeviceOffset) )
	{
		pTempNode= BLST_Q_NEXT(pTempNode, link);
	}

	if( pTempNode )
	{
		for(i = 0; i < ulCount; i++ )
		{
			BDBG_MSG(("Free node:"));
			BVDC_P_BufferHeap_DumpHeapNode_isr(pTempNode);

			if( pTempNode && pTempNode->bUsed )
			{
				/* Unmark the node and its parent node */
				BVDC_P_BufferHeap_UnmarkNode_isr(pTempNode);
			}
			else
			{
				BDBG_ERR(("Can not find a matching node"));
				return BERR_TRACE(BERR_INVALID_PARAMETER);
			}

			pTempNode= BLST_Q_NEXT(pTempNode, link);
		}
	}
	else
	{
		BDBG_ERR(("Can not find a matching node"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	BDBG_ASSERT(pHeapInfo->ulBufUsed <= pHeapInfo->ulTotalBufCnt);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_P_BufferHeap_Create
	( const BVDC_Handle           hVdc,
	  BVDC_Heap_Handle           *phHeap,
	  BMEM_Heap_Handle            hMem,
	  const BVDC_Heap_Settings   *pSettings )
{
	uint32_t                  i;
	BERR_Code                 err = BERR_SUCCESS;
	BVDC_P_BufferHeapContext *pBufferHeap;

	BDBG_ENTER(BVDC_P_BufferHeap_Create);
	BDBG_OBJECT_ASSERT(hVdc, BVDC_VDC);

	/* BDBG_SetModuleLevel("BVDC_HEAP", BDBG_eMsg);  */

	/* (1) Create memory heap context */
	pBufferHeap = (BVDC_P_BufferHeapContext *)BKNI_Malloc(sizeof(BVDC_P_BufferHeapContext));
	if( !pBufferHeap )
	{
		return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
	}
	/* Clear out context */
	BKNI_Memset((void*)pBufferHeap, 0x0, sizeof(BVDC_P_BufferHeapContext));
	BDBG_OBJECT_SET(pBufferHeap, BVDC_BFH);

	/* Initialize fields. */
	pBufferHeap->hVdc         = hVdc;
	pBufferHeap->hMem         = hMem;
	pBufferHeap->stSettings   = *pSettings;

	/* Get const properties */
	BVDC_P_BufferHeap_GetHeapOrder(pBufferHeap);

	for( i = 0; i < BVDC_P_BufferHeapId_eCount; i++ )
	{
		/* (2) Create heap info */
		err = BERR_TRACE(BVDC_P_BufferHeap_CreateHeapInfo(
			pBufferHeap->hMem, &(pBufferHeap->astHeapInfo[i])));

		if( err != BERR_SUCCESS )
		{
			/* Free already allocated memory */
			while(i--)
			{
				BVDC_P_BufferHeap_DestroyHeapInfo(
					pBufferHeap->hMem, &(pBufferHeap->astHeapInfo[i]));
			}
			/* Free memory context */
			BDBG_OBJECT_DESTROY(pBufferHeap, BVDC_BFH);
			BKNI_Free((void*)pBufferHeap);
			return BERR_TRACE(err);
		}
	}

	/* All done. now return the new fresh context to user. */
	*phHeap = (BVDC_P_BufferHeap_Handle)pBufferHeap;

	/* What's the output */
	BVDC_P_BufferHeap_Dump_isr(pBufferHeap);

	BDBG_LEAVE(BVDC_P_BufferHeap_Create);
	return err;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_P_BufferHeap_Destroy
	( BVDC_P_BufferHeap_Handle      hBufferHeap )
{
	uint32_t                  i;

	BDBG_ENTER(BVDC_P_BufferHeap_Destroy);
	BDBG_OBJECT_ASSERT(hBufferHeap, BVDC_BFH);

	for( i = 0; i < BVDC_P_BufferHeapId_eCount; i++ )
	{
		/* [2] Free heap info */
		BVDC_P_BufferHeap_DestroyHeapInfo(hBufferHeap->hMem,
			&(hBufferHeap->astHeapInfo[i]));
	}

	BDBG_OBJECT_DESTROY(hBufferHeap, BVDC_BFH);
	/* [1] Free memory context */
	BKNI_Free((void*)hBufferHeap);

	BDBG_LEAVE(BVDC_P_BufferHeap_Destroy);
	return BERR_SUCCESS;
}


/***************************************************************************
 *
 */
void BVDC_P_BufferHeap_Init
	( BVDC_P_BufferHeap_Handle     hBufferHeap )
{
	uint32_t                  i;

	BDBG_ENTER(BVDC_P_BufferHeap_Init);
	BDBG_OBJECT_ASSERT(hBufferHeap, BVDC_BFH);

	/* Re-Initialize fields that may changes during previous run. */
	for( i = 0; i < BVDC_P_BufferHeapId_eCount; i++ )
	{
		BVDC_P_BufferHeap_InitHeapInfo(&(hBufferHeap->astHeapInfo[i]));
	}

	BDBG_LEAVE(BVDC_P_BufferHeap_Init);
	return;
}

/***************************************************************************
 * eBufferHeapIdPrefer is where the user prefer buffer node original comes
 * from.
 * example: User prefers a SD buffer node comes from HD buffer heap. It will
 * try to find a break up SD node from HD buffer. If not found, it will loose
 * the requirement and set eBufferHeapIdPrefer to BVDC_P_BufferHeapId_eUnknown
 * and try to find a SD node from any buffer.
 *
 * if eBufferHeapIdPrefer is BVDC_P_BufferHeapId_eUnknown, the node can come from
 * any buffer heap.
 */
BERR_Code BVDC_P_BufferHeap_AllocateBuffers_isr
	( BVDC_P_BufferHeap_Handle         hBufferHeap,
	  BVDC_P_HeapNodePtr               apHeapNode[],
	  uint32_t                         ulCount,
	  bool                             bContinuous,
	  BVDC_P_BufferHeapId              eBufferHeapId,
	  BVDC_P_BufferHeapId              eBufferHeapIdPrefer )
{
	uint32_t                  ulIndex;
	uint32_t                  ulBufferAvailable;
	BERR_Code                 err = BERR_SUCCESS;
	BVDC_P_BufferHeap_Info   *pHeapInfo;

	BDBG_ENTER(BVDC_P_BufferHeap_AllocateBuffers_isr);
	BDBG_OBJECT_ASSERT(hBufferHeap, BVDC_BFH);

	if( eBufferHeapId >= BVDC_P_BufferHeapId_eCount )
	{
		BDBG_ERR(("Not supported"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	BDBG_MSG(("Allocate %d %s (prefered %s) buffers ", ulCount,
		BVDC_P_BUFFERHEAP_GET_HEAP_ID_NAME(eBufferHeapId),
		BVDC_P_BUFFERHEAP_GET_HEAP_ID_NAME(eBufferHeapIdPrefer)));

	ulIndex = hBufferHeap->aulIndex[eBufferHeapId];
	pHeapInfo = &(hBufferHeap->astHeapInfo[ulIndex]);

	BDBG_ASSERT(pHeapInfo->eBufHeapId == eBufferHeapId);

	/* Get buffer heap */
	ulBufferAvailable = pHeapInfo->ulTotalBufCnt - pHeapInfo->ulBufUsed;

	if( ulCount > ulBufferAvailable )
	{
		const BFMT_VideoInfo *pFmtInfo;
		const BVDC_Heap_Settings *pHeap = &hBufferHeap->stSettings;

		BDBG_ERR(("App needs to alloc more memory! Needs [%d] %s (%s prefered) buffers and has [%d] %s buffers.",
			ulCount,
			BVDC_P_BUFFERHEAP_GET_HEAP_ID_NAME(eBufferHeapId),
			BVDC_P_BUFFERHEAP_GET_HEAP_ID_NAME(eBufferHeapIdPrefer),
			ulBufferAvailable, BVDC_P_BUFFERHEAP_GET_HEAP_ID_NAME(eBufferHeapId)));

		BDBG_ERR(("Heap settings:"));
		pFmtInfo = BFMT_GetVideoFormatInfoPtr(pHeap->eBufferFormat_2HD);
		if(pFmtInfo == NULL)
			return BERR_TRACE(BERR_INVALID_PARAMETER);
		BDBG_ERR(("--------2HD---------"));
		BDBG_ERR(("ulBufferCnt     = %d", pHeap->ulBufferCnt_2HD));
		BDBG_ERR(("ulBufferCnt_Pip = %d", pHeap->ulBufferCnt_2HD_Pip));
		BDBG_ERR(("eBufferFormat   = %s", pFmtInfo->pchFormatStr));
		BDBG_ERR(("ePixelFormat    = %s", BPXL_ConvertFmtToStr(pHeap->ePixelFormat_2HD)));

		pFmtInfo = BFMT_GetVideoFormatInfoPtr(pHeap->eBufferFormat_HD);
		if(pFmtInfo == NULL)
			return BERR_TRACE(BERR_INVALID_PARAMETER);
		BDBG_ERR(("---------HD---------"));
		BDBG_ERR(("ulBufferCnt     = %d", pHeap->ulBufferCnt_HD));
		BDBG_ERR(("ulBufferCnt_Pip = %d", pHeap->ulBufferCnt_HD_Pip));
		BDBG_ERR(("eBufferFormat   = %s", pFmtInfo->pchFormatStr));
		BDBG_ERR(("ePixelFormat    = %s", BPXL_ConvertFmtToStr(pHeap->ePixelFormat_HD)));

		pFmtInfo = BFMT_GetVideoFormatInfoPtr(pHeap->eBufferFormat_SD);
		if(pFmtInfo == NULL)
			return BERR_TRACE(BERR_INVALID_PARAMETER);
		BDBG_ERR(("---------SD---------"));
		BDBG_ERR(("ulBufferCnt     = %d", pHeap->ulBufferCnt_SD));
		BDBG_ERR(("ulBufferCnt_Pip = %d", pHeap->ulBufferCnt_SD_Pip));
		BDBG_ERR(("eBufferFormat   = %s", pFmtInfo->pchFormatStr));
		BDBG_ERR(("ePixelFormat    = %s", BPXL_ConvertFmtToStr(pHeap->ePixelFormat_SD)));
		BDBG_ERR(("--------------------"));

		return BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
	}

	if( bContinuous )
	{
		BDBG_ASSERT(eBufferHeapIdPrefer == BVDC_P_BufferHeapId_eUnknown);
		err = BERR_TRACE(BVDC_P_BufferHeap_AllocateContNodes_isr(
			pHeapInfo, apHeapNode, ulCount));
	}
	else
	{
		if(eBufferHeapIdPrefer == BVDC_P_BufferHeapId_eUnknown)
		{
			err = BERR_TRACE(BVDC_P_BufferHeap_AllocateNodes_isr(
				pHeapInfo, apHeapNode, ulCount, eBufferHeapIdPrefer));
		}
		else
		{
			/* Try to find prefered nodes first */
			err = BVDC_P_BufferHeap_AllocateNodes_isr(
				pHeapInfo, apHeapNode, ulCount, eBufferHeapIdPrefer);

			/* Didn't find prefered nodes, find them from other heap buffer */
			if(err != BERR_SUCCESS)
			{
				BDBG_MSG(("Try to find any %d %s buffers ", ulCount,
					BVDC_P_BUFFERHEAP_GET_HEAP_ID_NAME(eBufferHeapId)));
				err = BERR_TRACE(BVDC_P_BufferHeap_AllocateNodes_isr(
					pHeapInfo, apHeapNode, ulCount, BVDC_P_BufferHeapId_eUnknown));
			}
		}
	}

	BDBG_LEAVE(BVDC_P_BufferHeap_AllocateBuffers_isr);
	return err;
}


/***************************************************************************
 *
 */
BERR_Code BVDC_P_BufferHeap_FreeBuffers_isr
	( BVDC_P_BufferHeap_Handle         hBufferHeap,
	  BVDC_P_HeapNodePtr               apHeapNode[],
	  uint32_t                         ulCount,
	  bool                             bContinuous )
{
	BERR_Code                 err = BERR_SUCCESS;

	BDBG_ENTER(BVDC_P_BufferHeap_FreeBuffers_isr);
	BDBG_OBJECT_ASSERT(hBufferHeap, BVDC_BFH);

	BDBG_MSG(("Free %d buffers ", ulCount));

	if( bContinuous )
	{
		err = BERR_TRACE(BVDC_P_BufferHeap_FreeContNodes_isr(
			apHeapNode, ulCount));
	}
	else
	{
		uint32_t i;

		for( i = 0; i < ulCount; i++ )
		{
			err = BERR_TRACE(BVDC_P_BufferHeap_FreeNode_isr(apHeapNode[i]));
			if( err != BERR_SUCCESS )
				return err;
		}
	}

	BDBG_LEAVE(BVDC_P_BufferHeap_FreeBuffers_isr);
	return err;
}

/***************************************************************************
 * Find the best fit buffer
 */
void BVDC_P_BufferHeap_GetHeapIdBySize
	( const BVDC_P_BufferHeapContext  *pHeap,
	  uint32_t                         ulSize,
	  BVDC_P_BufferHeapId             *peBufHeapId )
{
	uint32_t                  i;
	BVDC_P_BufferHeapId       eIdIndex;
	BVDC_P_BufferHeapId       eBufferHeapId = BVDC_P_BufferHeapId_eUnknown;

	/* astHeapInfo are stored from big to small */
	for(i = 0; i < BVDC_P_BufferHeapId_eCount; i++)
	{
		eIdIndex =
			pHeap->astHeapInfo[BVDC_P_BufferHeapId_eCount - i -1].eBufHeapId;
		if(pHeap->aulHeapSize[eIdIndex] >= ulSize)
		{
			eBufferHeapId = eIdIndex;
			break;
		}
	}

	if(eBufferHeapId == BVDC_P_BufferHeapId_eUnknown)
	{
		BDBG_WRN(("Can not find a buffer heap to fit 0x%lx", ulSize));
	}

	if(peBufHeapId)
		*peBufHeapId = eBufferHeapId;

}


void BVDC_P_BufferHeap_GetHeapSizeById
	( const BVDC_P_BufferHeapContext  *pHeap,
	  BVDC_P_BufferHeapId              eBufHeapId,
	  uint32_t                        *pulBufHeapSize )
{
	uint32_t   i, ulBufSize = 0;

	if(eBufHeapId == BVDC_P_BufferHeapId_eUnknown)
	{
		BDBG_WRN(("Unknown buffer heap Id: %d", eBufHeapId));
	}

	/* astHeapInfo are stored from big to small */
	for(i = 0; i < BVDC_P_BufferHeapId_eCount; i++)
	{
		if(pHeap->astHeapInfo[i].eBufHeapId == eBufHeapId)
		{
			ulBufSize = pHeap->astHeapInfo[i].ulBufSize;
			break;
		}
	}

	if(pulBufHeapSize)
		*pulBufHeapSize = ulBufSize;

}

/* End of file. */
