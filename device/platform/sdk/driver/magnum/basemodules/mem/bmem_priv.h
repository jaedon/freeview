/***************************************************************************
 *     Copyright (c) 2001-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bmem_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/38 $
 * $brcm_Date: 9/21/12 6:47p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/mem/bmem_priv.h $
 * 
 * Hydra_Software_Devel/38   9/21/12 6:47p albertl
 * SW7435-255: Optimized BMEM with AA Tree implementation.
 * 
 * Hydra_Software_Devel/37   7/23/12 5:13p albertl
 * SW7552-303: Removed usage of BMEM_HEAP_ALIGNMENT for cache alignment
 * issues.  Default alignment is now 0 to force user to set a valid
 * alignment.
 * 
 * Hydra_Software_Devel/36   4/20/12 5:49p albertl
 * SW7420-2292: Streamlined BMEM_HEAP_ALIGNMENT platform handling.
 * 
 * Hydra_Software_Devel/35   2/7/12 6:37p albertl
 * SW7425-2345: Changed total allocation and number of allocation tracking
 * to work in eFastest.
 * 
 * Hydra_Software_Devel/34   1/31/12 1:56p erickson
 * SW7346-636: BMEM_P_Heap_CheckCachedAddress must test that pheap-
 * >pvCache is non-NULL
 * 
 * Hydra_Software_Devel/33   9/9/11 7:10p albertl
 * SW7346-201, SW7405-5350: Moved watermark calculation to alloc and free
 * for accurate calculation.  Changed BMEM_CONFIG_SAFE to not use
 * BMEM_FREE_CAREFULLY.
 * 
 * Hydra_Software_Devel/32   9/8/11 4:22p albertl
 * SW7405-5350: Changed block info to be doublely linked.  Implemented
 * faster pbi removal and assigned slower removal and error checking to
 * BMEM_FREE_CAREFULLY.
 * 
 * Hydra_Software_Devel/31   4/25/11 11:37a tdo
 * SW7422-12: Fix default alignment for 7422/7425
 *
 * Hydra_Software_Devel/30   4/12/11 6:11p albertl
 * SW7425-333: Added smart detection of cached address to
 * BMEM_Heap_ConvertAddress_ToOffset.
 *
 * Hydra_Software_Devel/29   3/28/11 5:12p albertl
 * SW7425-247: Incorporated BDBG_OBJECT handle validation.
 *
 * Hydra_Software_Devel/28   1/10/11 3:32p albertl
 * SW7408-193: BMEM_Heapinfo now includes high watermark.  Added
 * BMEM_Heap_ResetHighWatermark().
 *
 * Hydra_Software_Devel/27   8/17/10 5:58p nickh
 * SW7422-12: Add default alignment for 7422
 *
 * Hydra_Software_Devel/26   3/26/10 5:14p albertl
 * SW7405-3979: Changed filename field from string to pointer and removed
 * slow copy operation.
 *
 * Hydra_Software_Devel/25   5/22/09 5:30p albertl
 * PR55389: Replaced uintptr_t with uint32_t.
 *
 * Hydra_Software_Devel/24   5/15/09 8:42a gmohile
 * PR 25109 : define uint32_t for 7401/03
 *
 * Hydra_Software_Devel/23   2/25/09 4:48p erickson
 * PR52471: added const keyword to global data
 *
 * Hydra_Software_Devel/22   2/9/09 4:12p tdo
 * PR50391: Correct default alignment setting for 7420
 *
 * Hydra_Software_Devel/21   8/6/08 11:30a agin
 * PR43531:  Must use 128 byte alignment for 7325.
 *
 * Hydra_Software_Devel/20   7/23/08 2:31p albertl
 * PR43531:  Added 7325 and 7335 to list of chips using default alignment
 * of 2.
 *
 * Hydra_Software_Devel/19   6/11/08 3:41p albertl
 * PR43531:  Changed to use default alignment of 6 for all new chips, and
 * 2 for old chips.
 *
 * Hydra_Software_Devel/18   7/6/07 6:53p albertl
 * PR31066:  Changed BMEM_HEAP_ALIGNMENT to 6 for 7400 B0 and later and
 * 7405.  Added warnings when user specified alignment is less than and
 * forced to BMEM_HEAP_ALIGNMENT.
 *
 * Hydra_Software_Devel/17   5/14/07 1:20p albertl
 * PR30621:  Fixed heaps being added to allocation list but not being
 * removed when freed in some safety configurations.
 *
 * Hydra_Software_Devel/16   3/15/07 7:23p albertl
 * PR28682:  Changed BMEM_GUARD_SIZE_BYTES to be dynamically calculated
 * based on runtime safety config.
 *
 * Hydra_Software_Devel/15   10/12/06 6:54p albertl
 * PR20247:  Fixed all heap safety config field checks to use new
 * pSafetyConfigInfo structure.
 *
 * Hydra_Software_Devel/14   6/21/06 3:11p albertl
 * PR20247:  Moved safety config table to bmem.c  BMEM_P_Heap now uses a
 * BMEM_P_SafetyConfigInfo pointer to track safety configuration settings
 * instead of tracking each configuration separately.
 *
 * Hydra_Software_Devel/13   6/16/06 3:23p albertl
 * PR20247, PR20276, PR20354:  Added the ability to control debug
 * configuration at runtime.  Added address and offset checking to
 * conversion functions.  BMEM_SetCache can now only be called before
 * heaps are allocated from.  Added BMEM_Heap functions.
 *
 * Hydra_Software_Devel/12   10/7/05 3:53p jasonh
 * PR 17374: Allowed GetHeapInfo to return original creation parameters.
 *
 * Hydra_Software_Devel/11   3/10/05 5:05p albertl
 * PR13677:  Both local and system bookkeeping made available at heap
 * creation though functions BMEM_CreateHeapSystem and
 * BMEM_CreateHeapLocal.
 *
 * Hydra_Software_Devel/10   11/2/04 10:32a hongtaoz
 * PR13076: added cached memory support;
 *
 * Hydra_Software_Devel/9   5/26/04 7:01p hongtaoz
 * PR10059: sorted freed heap in size ascending order to avoid memory
 * fragamentation;
 *
 * Hydra_Software_Devel/8   4/1/04 1:48p pntruong
 * PR10201: Added memory monitor support.
 *
 * Hydra_Software_Devel/7   9/15/03 5:22p jasonh
 * Fixed bookkeeping size for local allocations. Added offset and address
 * values to private heap structure. Renamed semaphore macros.
 *
 * Hydra_Software_Devel/6   9/5/03 1:40p jasonh
 * Inserted configuration values from bmem_config.h. Removed unnecessary
 * prototypes.
 *
 * Hydra_Software_Devel/5   9/4/03 7:18p jasonh
 * Commented out unnecessary function.
 *
 * Hydra_Software_Devel/4   9/2/03 1:47p vadim
 * Moved macro BRCM_UNUSED from bmem.h
 *
 * Hydra_Software_Devel/3   9/2/03 1:39p vadim
 * Some magnum updates.
 *
 * Hydra_Software_Devel/2   3/20/03 3:51p erickson
 * renamed all MEM_ to BMEM_
 *
 * Hydra_Software_Devel/1   3/20/03 3:24p erickson
 * initial bmem work, taken from SetTop/memorylib
 *
 * SanJose_DVTSW_Devel\6   4/18/02 11:29a poz
 * Made the pMutex member conditionally compiled.
 *
 * SanJose_DVTSW_Devel\5   4/15/02 3:18p ngibbs
 * Fixed a typo for BMEM_NOT_REENTRANT mode.
 *
 * SanJose_DVTSW_Devel\4   4/15/02 2:27p poz
 * Use BMEM_REENTRANT_CONFIG instead of BMEM_REENTRANT.
 *
 * SanJose_DVTSW_Devel\3   4/15/02 1:27p poz
 * Added prototypes for GetAddres, GetBlockInfo, and DropBlockInfo.
 * Added macro implmentation of these functions for the local memory heap
 * manager.
 * Added pvData member for implementation-specific data.
 * Updated comments, copyright and keywords in header.
 *
 ***************************************************************************/
#ifndef BMEM_PRIV_H__
#define BMEM_PRIV_H__

#include "blst_aa_tree.h"

#ifndef BMEM_CONFIG_H__
#include "bmem_config.h"  /* for BMEM_GUARD_SIZE_BYTES, BMEM_FILENAME_LENGTH */
#endif

#if (BMEM_REENTRANT_CONFIG == BMEM_REENTRANT)
#include "bkni_multi.h" /* for semaphores */
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************
 * The macros below allow configuration of typical defaults. There should
 * be no need to change them, but they're pulled out here just in case.
 **************************************************************************/

/*
 * BMEM_MIN_ALLOCATION_SIZE (modifyable)
 *
 * When splitting up a free block for allocation, any left-overs smaller than
 * BMEM_MIN_BLOCK_SIZE plus the bookkeping size will become part of the
 * allocated block instead of made into a free block. This may seem
 * wasteful, but it reduces overall heap fragmentation.
 *
 * If this value is set to 0, then all left-overs large enough to be a free
 * or allocated block will be split up as individual blocks (even if it
 * can only hold a zero size allocation).
 *
 * The default value is 128 bytes.
 */
#define BMEM_MIN_ALLOCATION_SIZE  128

/*
 * BMEM_GUARD_BYTE (modifyable)
 *
 * Guard bytes are set to this value when a block is created. In addition to
 * the formal guard areas, the back scrap and front scrap are filled with
 * this value. The default is 0xac (for "All Clear").
 */
#define BMEM_GUARD_BYTE      0xac

/*
 * BMEM_CLEAR_BYTE (modifyable)
 *
 * When a block is freed, the data bytes are set to this value.
 *
 * By default, the value is zero since on most systems 0x00000000 is an
 * invalid address which is trapped.
 *
 */
#define BMEM_CLEAR_BYTE      0x00

/*
 * BMEM_FILENAME_LENGTH (modifyable)
 *
 * The length of the filename to be stored for each block. Only used when
 * BMEM_TRACK_FILE_AND_LINE is defined. Make this a multiple of 4 minus 1,
 * like the default of 127.
 */
#define BMEM_FILENAME_LENGTH 127

/**************************************************************************
 * There is even less need to modify the macros from here on down.
 **************************************************************************/

/*
 * BMEM_CHECK_ALL_GUARDS (modifyable)
 *
 * Whenever a block is freed or allocated, the list of allocated and freed
 * blocks is walked and the guard values checked to make sure they haven't
 * been touched.
 *
 * If you choose BMEM_CHECK_ALL_GUARDS, then you get BMEM_FREE_CAREFULLY
 * as well.
 *
 * This can be very slow, and is undesireable in production code.
 *
 */
#define BMEM_CHECK_ALL_GUARDS_FASTEST false
#define BMEM_CHECK_ALL_GUARDS_NORMAL  false
#define BMEM_CHECK_ALL_GUARDS_TRACK   false
#define BMEM_CHECK_ALL_GUARDS_SAFE    true
#define BMEM_CHECK_ALL_GUARDS_SAFEST  true

#if (BMEM_SAFETY_CONFIG==BMEM_CONFIG_FASTEST)
	#define BMEM_CHECK_ALL_GUARDS BMEM_CHECK_ALL_GUARDS_FASTEST
#elif (BMEM_SAFETY_CONFIG==BMEM_CONFIG_NORMAL)
	#define BMEM_CHECK_ALL_GUARDS BMEM_CHECK_ALL_GUARDS_NORMAL
#elif (BMEM_SAFETY_CONFIG==BMEM_CONFIG_TRACK)
	#define BMEM_CHECK_ALL_GUARDS BMEM_CHECK_ALL_GUARDS_TRACK
#elif (BMEM_SAFETY_CONFIG==BMEM_CONFIG_SAFE)
	#define BMEM_CHECK_ALL_GUARDS BMEM_CHECK_ALL_GUARDS_SAFE
#elif (BMEM_SAFETY_CONFIG==BMEM_CONFIG_SAFEST)
	#define BMEM_CHECK_ALL_GUARDS BMEM_CHECK_ALL_GUARDS_SAFEST
#else
	#define BMEM_CHECK_ALL_GUARDS BMEM_CHECK_ALL_GUARDS_SAFEST
#endif

/*
 * BMEM_FREE_CAREFULLY (modifyable)
 *
 * When a block is freed, the heap manager can check to see if it is in the
 * list of allocated blocks before freeing it. This is slow (as it
 * iterates over a linked list to find the block).
 *
 * Debug environments may want this check, production ones likely don't
 * want the overhead.
 *
 * If you want careful frees, define BMEM_FREE_CAREFULLY.
 * If you choose BMEM_CHECK_ALL_GUARDS, then you get BMEM_FREE_CAREFULLY
 * as well.
 *
 */
#define BMEM_FREE_CAREFULLY_FASTEST (false || BMEM_CHECK_ALL_GUARDS_FASTEST)
#define BMEM_FREE_CAREFULLY_NORMAL  (false || BMEM_CHECK_ALL_GUARDS_NORMAL)
#define BMEM_FREE_CAREFULLY_TRACK   (false || BMEM_CHECK_ALL_GUARDS_TRACK)
#define BMEM_FREE_CAREFULLY_SAFE    (false || BMEM_CHECK_ALL_GUARDS_SAFE)
#define BMEM_FREE_CAREFULLY_SAFEST  (true  || BMEM_CHECK_ALL_GUARDS_SAFEST)

#if (BMEM_SAFETY_CONFIG==BMEM_CONFIG_FASTEST)
	#define BMEM_FREE_CAREFULLY BMEM_FREE_CAREFULLY_FASTEST
#elif (BMEM_SAFETY_CONFIG==BMEM_CONFIG_NORMAL)
	#define BMEM_FREE_CAREFULLY BMEM_FREE_CAREFULLY_NORMAL
#elif (BMEM_SAFETY_CONFIG==BMEM_CONFIG_TRACK)
	#define BMEM_FREE_CAREFULLY BMEM_FREE_CAREFULLY_TRACK
#elif (BMEM_SAFETY_CONFIG==BMEM_CONFIG_SAFE)
	#define BMEM_FREE_CAREFULLY BMEM_FREE_CAREFULLY_SAFE
#elif (BMEM_SAFETY_CONFIG==BMEM_CONFIG_SAFEST)
	#define BMEM_FREE_CAREFULLY BMEM_FREE_CAREFULLY_SAFEST
#else
	#define BMEM_FREE_CAREFULLY BMEM_FREE_CAREFULLY_SAFEST
#endif

/*
 * BMEM_FREE_CLEAR (modifyable)
 *
 * When a block is freed, the heap manager can fill the data space with
 * a byte value. This is done to guarantee that data isn't used after it
 * has been freed.
 *
 * Debug environments may want this check, production ones likely don't
 * want the overhead.
 *
 * If you want to clear out freed data, define BMEM_FREE_CLEAR.
 */
#define BMEM_FREE_CLEAR_FASTEST false
#define BMEM_FREE_CLEAR_NORMAL  false
#define BMEM_FREE_CLEAR_TRACK   false
#define BMEM_FREE_CLEAR_SAFE    false
#define BMEM_FREE_CLEAR_SAFEST  true

#if (BMEM_SAFETY_CONFIG==BMEM_CONFIG_FASTEST)
	#define BMEM_FREE_CLEAR BMEM_FREE_CLEAR_FASTEST
#elif (BMEM_SAFETY_CONFIG==BMEM_CONFIG_NORMAL)
	#define BMEM_FREE_CLEAR BMEM_FREE_CLEAR_NORMAL
#elif (BMEM_SAFETY_CONFIG==BMEM_CONFIG_TRACK)
	#define BMEM_FREE_CLEAR BMEM_FREE_CLEAR_TRACK
#elif (BMEM_SAFETY_CONFIG==BMEM_CONFIG_SAFE)
	#define BMEM_FREE_CLEAR BMEM_FREE_CLEAR_SAFE
#elif (BMEM_SAFETY_CONFIG==BMEM_CONFIG_SAFEST)
	#define BMEM_FREE_CLEAR BMEM_FREE_CLEAR_SAFEST
#else
	#define BMEM_FREE_CLEAR BMEM_FREE_CLEAR_SAFEST
#endif


/*
 * BMEM_GUARD_SIZE (modifyable)
 *
 * At the front and back of all blocks (both free and allocated) are a series
 * of guard bytes. These allow detection of buffer over- and under-runs.
 *
 * If you have a system which has under- or over-runs, making this value
 * large will protect the heap manager's data structures.
 *
 * The number of 4-byte words in these series is set by BMEM_GUARD_SIZE.
 */
#define BMEM_GUARD_UNIT_BYTES 4
#define BMEM_GUARD_SIZE_FASTEST 0
#define BMEM_GUARD_SIZE_NORMAL  1
#define BMEM_GUARD_SIZE_TRACK   1
#define BMEM_GUARD_SIZE_SAFE    2
#define BMEM_GUARD_SIZE_SAFEST  8
#define BMEM_GUARD_SIZE_DEFAULT 8

#if (BMEM_SAFETY_CONFIG==BMEM_CONFIG_FASTEST)
	#define BMEM_GUARD_SIZE        BMEM_GUARD_SIZE_FASTEST
#elif (BMEM_SAFETY_CONFIG==BMEM_CONFIG_NORMAL)
	#define BMEM_GUARD_SIZE        BMEM_GUARD_SIZE_NORMAL
#elif (BMEM_SAFETY_CONFIG==BMEM_CONFIG_TRACK)
	#define BMEM_GUARD_SIZE        BMEM_GUARD_SIZE_TRACK
#elif (BMEM_SAFETY_CONFIG==BMEM_CONFIG_SAFE)
	#define BMEM_GUARD_SIZE        BMEM_GUARD_SIZE_SAFE
#elif (BMEM_SAFETY_CONFIG==BMEM_CONFIG_SAFEST)
	#define BMEM_GUARD_SIZE        BMEM_GUARD_SIZE_SAFEST
#else
	#define BMEM_GUARD_SIZE        BMEM_GUARD_SIZE_SAFEST
#endif

#define BMEM_GUARD_SIZE_BYTES (BMEM_GUARD_UNIT_BYTES * pheap->pSafetyConfigInfo->iGuardSize)

#if 0
/*
 * BMEM_CHECK_DISORDER (modifyable)
 *
 * This turns on a consistency check during frees which checks to see if
 * list of blocks are ordered by size.
 *
 * This is more of a check of the heap manager itself than of programs
 * which are running with this manager, so it's off for everything but
 * the safest configurations.
 */
#define BMEM_CHECK_DISORDER_FASTEST false
#define BMEM_CHECK_DISORDER_NORMAL  false
#define BMEM_CHECK_DISORDER_TRACK   false
#define BMEM_CHECK_DISORDER_SAFE    false
#define BMEM_CHECK_DISORDER_SAFEST  true

#if (BMEM_SAFETY_CONFIG==BMEM_CONFIG_FASTEST)
	#define BMEM_CHECK_DISORDER BMEM_CHECK_DISORDER_FASTEST
#elif (BMEM_SAFETY_CONFIG==BMEM_CONFIG_NORMAL)
	#define BMEM_CHECK_DISORDER BMEM_CHECK_DISORDER_NORMAL
#elif (BMEM_SAFETY_CONFIG==BMEM_CONFIG_TRACK)
	#define BMEM_CHECK_DISORDER BMEM_CHECK_DISORDER_TRACK
#elif (BMEM_SAFETY_CONFIG==BMEM_CONFIG_SAFE)
	#define BMEM_CHECK_DISORDER BMEM_CHECK_DISORDER_SAFE
#elif (BMEM_SAFETY_CONFIG==BMEM_CONFIG_SAFEST)
	#define BMEM_CHECK_DISORDER BMEM_CHECK_DISORDER_SAFEST
#else
	#define BMEM_CHECK_DISORDER BMEM_CHECK_DISORDER_SAFEST
#endif
#else
	#define BMEM_CHECK_DISORDER false
#endif

/*
 * BMEM_CHECK_GUARD_ON_FREE (modifyable)
 *
 * When a block is freed, the guard values are checked to make sure they
 * haven't been touched.
 *
 */
#define BMEM_CHECK_GUARD_ON_FREE_FASTEST false
#define BMEM_CHECK_GUARD_ON_FREE_NORMAL  true
#define BMEM_CHECK_GUARD_ON_FREE_TRACK   true
#define BMEM_CHECK_GUARD_ON_FREE_SAFE    true
#define BMEM_CHECK_GUARD_ON_FREE_SAFEST  true

#if (BMEM_SAFETY_CONFIG==BMEM_CONFIG_FASTEST)
	#define BMEM_CHECK_GUARD_ON_FREE BMEM_CHECK_GUARD_ON_FREE_FASTEST
#elif (BMEM_SAFETY_CONFIG==BMEM_CONFIG_NORMAL)
	#define BMEM_CHECK_GUARD_ON_FREE BMEM_CHECK_GUARD_ON_FREE_NORMAL
#elif (BMEM_SAFETY_CONFIG==BMEM_CONFIG_TRACK)
	#define BMEM_CHECK_GUARD_ON_FREE BMEM_CHECK_GUARD_ON_FREE_TRACK
#elif (BMEM_SAFETY_CONFIG==BMEM_CONFIG_SAFE)
	#define BMEM_CHECK_GUARD_ON_FREE BMEM_CHECK_GUARD_ON_FREE_SAFE
#elif (BMEM_SAFETY_CONFIG==BMEM_CONFIG_SAFEST)
	#define BMEM_CHECK_GUARD_ON_FREE BMEM_CHECK_GUARD_ON_FREE_SAFEST
#else
	#define BMEM_CHECK_GUARD_ON_FREE BMEM_CHECK_GUARD_ON_FREE_SAFEST
#endif


/*
 * BMEM_TRACK_FILE_AND_LINE (modifyable)
 *
 * Whenever a block is allocated, record the file and line number given
 * for later reporting or debugging.
 *
 * This takes quite a bit of extra space which is likley undesireable in
 * production code.
 *
 */
#define BMEM_TRACK_FILE_AND_LINE_FASTEST false
#define BMEM_TRACK_FILE_AND_LINE_NORMAL  false
#define BMEM_TRACK_FILE_AND_LINE_TRACK   true
#define BMEM_TRACK_FILE_AND_LINE_SAFE    true
#define BMEM_TRACK_FILE_AND_LINE_SAFEST  true

#if (BMEM_SAFETY_CONFIG==BMEM_CONFIG_FASTEST)
	#define BMEM_TRACK_FILE_AND_LINE BMEM_TRACK_FILE_AND_LINE_FASTEST
#elif (BMEM_SAFETY_CONFIG==BMEM_CONFIG_NORMAL)
	#define BMEM_TRACK_FILE_AND_LINE BMEM_TRACK_FILE_AND_LINE_NORMAL
#elif (BMEM_SAFETY_CONFIG==BMEM_CONFIG_TRACK)
	#define BMEM_TRACK_FILE_AND_LINE BMEM_TRACK_FILE_AND_LINE_TRACK
#elif (BMEM_SAFETY_CONFIG==BMEM_CONFIG_SAFE)
	#define BMEM_TRACK_FILE_AND_LINE BMEM_TRACK_FILE_AND_LINE_SAFE
#elif (BMEM_SAFETY_CONFIG==BMEM_CONFIG_SAFEST)
	#define BMEM_TRACK_FILE_AND_LINE BMEM_TRACK_FILE_AND_LINE_SAFEST
#else
	#define BMEM_TRACK_FILE_AND_LINE BMEM_TRACK_FILE_AND_LINE_SAFEST
#endif


/**************************************************************************
 **************************************************************************
 * Modifying anything from here down can cause blindness and sterility
 **************************************************************************
 **************************************************************************/

/*
 * BMEM_TRACK_ALLOCATIONS (do not modify, derived)
 *
 * For debugging purposes, you may wish to keep a list of all of the
 * allocated blocks in the system. This uses space in the block header and
 * is slower since list management must be done during allocation and
 * deallocation.
 *
 * This is required for BMEM_FREE_CAREFULLY and BMEM_CHECK_ALL_GUARDS to work.
 * It's also needed to track allocation files and lines.
 *
 */
#define BMEM_TRACK_ALLOCATIONS_FASTEST (BMEM_FREE_CAREFULLY_FASTEST) || (BMEM_CHECK_ALL_GUARDS_FASTEST) || (BMEM_TRACK_FILE_AND_LINE_FASTEST)
#define BMEM_TRACK_ALLOCATIONS_NORMAL  (BMEM_FREE_CAREFULLY_NORMAL)  || (BMEM_CHECK_ALL_GUARDS_NORMAL)  || (BMEM_TRACK_FILE_AND_LINE_NORMAL)
#define BMEM_TRACK_ALLOCATIONS_TRACK   (BMEM_FREE_CAREFULLY_TRACK)   || (BMEM_CHECK_ALL_GUARDS_TRACK)   || (BMEM_TRACK_FILE_AND_LINE_TRACK)
#define BMEM_TRACK_ALLOCATIONS_SAFE    (BMEM_FREE_CAREFULLY_SAFE)    || (BMEM_CHECK_ALL_GUARDS_SAFE)    || (BMEM_TRACK_FILE_AND_LINE_SAFE)
#define BMEM_TRACK_ALLOCATIONS_SAFEST  (BMEM_FREE_CAREFULLY_SAFEST)  || (BMEM_CHECK_ALL_GUARDS_SAFEST)  || (BMEM_TRACK_FILE_AND_LINE_SAFEST)

#define BMEM_TRACK_ALLOCATIONS (BMEM_FREE_CAREFULLY) || (BMEM_CHECK_ALL_GUARDS) || (BMEM_TRACK_FILE_AND_LINE)

/*
 * BMEM_FRONT_BOOKKEEPING_SIZE_LOCAL/SYSTEM (do not modify, derived)
 *
 * Handy macro which defines how much space is required at the front in
 * order to hold bookkeeping data and the guard.
 *
 * With local bookkeeping:
 *    [align scrap][block info [guard]][...data...][guard][back scrap]
 *                 ^------------------^
 *                 BMEM_FRONT_BOOKKEEPING_SIZE_LOCAL
 *
 * With system bookkeeping:
 *    [align scrap][guard][...data...][guard][back scrap]
 *                 ^-----^
 *                 BMEM_FRONT_BOOKKEEPING_SIZE_SYSTEM
 */
#define BMEM_FRONT_BOOKKEEPING_SIZE_LOCAL (sizeof(BMEM_P_BlockInfo)+BMEM_GUARD_SIZE_BYTES)
#define BMEM_FRONT_BOOKKEEPING_SIZE_SYSTEM (BMEM_GUARD_SIZE_BYTES)

/*
 * BMEM_BACK_BOOKKEEPING_SIZE (do not modify, derived)
 *
 * Handy macro which defines how much space is required at the back for the
 * guard.
 *
 * [align scrap][block info [guard]][...data...][guard][back scrap]
 *                                              ^-----^
 *                                              BMEM_BACK_BOOKKEEPING_SIZE
 */
#define BMEM_BACK_BOOKKEEPING_SIZE (BMEM_GUARD_SIZE_BYTES)

/*
 * BMEM_BOOKKEEPING_SIZE_LOCAL/SYSTEM (do not modify, derived)
 *
 * The number of bytes needed just to track the block. No actual blocks can
 * be smaller than this size. It is the overhead for each block. A zero size
 * allocation will create a block of this size.
 */
#define BMEM_BOOKKEEPING_SIZE_LOCAL (BMEM_FRONT_BOOKKEEPING_SIZE_LOCAL+BMEM_BACK_BOOKKEEPING_SIZE)
#define BMEM_BOOKKEEPING_SIZE_SYSTEM (BMEM_FRONT_BOOKKEEPING_SIZE_SYSTEM+BMEM_BACK_BOOKKEEPING_SIZE)

/*
 * BMEM_MIN_BLOCK_SIZE_LOCAL/SYSTEM (do not modify, derived)
 *
 * When splitting up a free block for allocation, any left-overs smaller than
 * BMEM_MIN_BLOCK_SIZE_LOCAL/SYSTEM will become part of the allocated block.
 * This may seem wasteful, but it reduces overall heap fragmentation.
 *
 * If this value is set to BMEM_BOOKKEEPING_SIZE_LOCAL/SYSTEM, then all
 * left-overs large enough to be a free or allocated block will be split up as
 * individual blocks. It cannot be set below BMEM_BACK_BOOKKEEPING_SIZE.
 *
 * The default value is big enough for bookkeeping and a 128 byte
 * allocation.
 */
#define BMEM_MIN_BLOCK_SIZE_LOCAL  (BMEM_BOOKKEEPING_SIZE_LOCAL + BMEM_MIN_ALLOCATION_SIZE)
#define BMEM_MIN_BLOCK_SIZE_SYSTEM  (BMEM_BOOKKEEPING_SIZE_SYSTEM + BMEM_MIN_ALLOCATION_SIZE)

/*
 * BMEM_HEAP_ALIGNMENT (do not modify)
 *
 * The heap requires a certain alignment since its data structures
 * are contained in the heap itself. The largest item in the BMEM_Heap and
 * BMEM_BlockInfo structures are pointers and uint32_ts, so a 4-byte alignment
 * is required.
 *
 * The default value is 6, which means the last six bits of the address will
 * be zero, which will yield 64-byte alignment.
 *
 * For older chips, the default value is 2, which yields a
 * 4-byte alignment.
 */
#define BMEM_HEAP_ALIGNMENT         2
#define BMEM_HEAP_ALIGNMENT_MASK    ((1L<<BMEM_HEAP_ALIGNMENT)-1)

BLST_AA_TREE_HEAD(BMEM_P_SizeTree, BMEM_P_BlockInfo);
BLST_AA_TREE_HEAD(BMEM_P_AddrTree, BMEM_P_BlockInfo);

/***************************************************************************
 * BMEM_P_BlockInfo - Holds the bookkeeping for a block of memory
 *
 * Structure of a block:
 *
 * [align scrap][block info [guard]][...data...][guard][back scrap]
 * ^--------------------------------------------------------------^
 *                              ulSize
 * ^-----------^                                       ^----------^
 * ulFrontScrap                                        ulBackScrap
 *
 */

typedef struct BMEM_P_BlockInfo BMEM_P_BlockInfo;
struct BMEM_P_BlockInfo
{
	BMEM_Heap_Handle pHeap;        /* Heap this block blongs to */
	size_t           ulSize;       /* Total size including guards and scrap */
	size_t           ulFrontScrap; /* Space at front left over from alignment */
	size_t           ulBackScrap;  /* Space at back left over to reduce fragmentation */
#ifdef BMEM_TRACK_FILE_AND_LINE
	uint32_t         ulLine;
	const char      *pchFilename;
#endif
	BLST_AA_TREE_ENTRY(BMEM_P_BlockInfo) stAddrNode;
	BLST_AA_TREE_ENTRY(BMEM_P_BlockInfo) stSizeNode;
};


typedef struct BMEM_P_SafetyConfigInfo
{
	uint32_t eSafetyConfig;
	bool     bTrackAllocations;
	bool     bFreeClear;
	bool     bFreeCarefully;
	uint32_t iGuardSize;
	bool     bCheckGuardOnFree;
	bool     bCheckAllGuards;
	bool     bTrackFileAndLine;
} BMEM_P_SafetyConfigInfo;

BDBG_OBJECT_ID_DECLARE(BMEM_Heap);

/***************************************************************************
 * BMEM_Heap
 *
 * Structure of a local heap:
 *
 * [BMEM_Heap][scrap][block][free block][block]...[block][scrap]
 *           pStart                                    pEnd
 *                         pFreeTop
 *
 * pAllocTop may point to any allocated block. It isn't ordered. pFreeTop
 * will always point to the first free block.
 *
 * The two scraps might be created depending on the alignment of the chunk
 * of memory given to the manager and the alignement required.
 *
 * The structure of a system heap is the same except the heap information
 * (BMEM_Heap) isn't kept at the front of the heap but in system memory
 * instead.
 *
 */
typedef struct BMEM_P_Heap
{
    BDBG_OBJECT(BMEM_Heap)
	BLST_S_ENTRY(BMEM_P_Heap) link;
	BMEM_ModuleHandle hMem;

	struct BMEM_P_AddrTree   stFreeTopAddrTree;  /* Tree holding free blocks sorted by address */
	struct BMEM_P_SizeTree   stFreeTopSizeTree;	 /* Tree holding free blocks sorted by size */
	struct BMEM_P_AddrTree   stAllocTopAddrTree; /* Tree holding allocated blocks sorted by address */

	uint32_t ulAlignMask;            /* Mask to use to create aligned blocks */
	unsigned int  uiAlignment;       /* Heap alignment (power of 2). */

	uint8_t *pStart;                 /* Pointer to first usable available byte in heap */
	uint8_t *pEnd;                   /* Pointer to last usable available byte in heap */

	uint32_t ulNumErrors;            /* Counts number of errors detected */

#if (BMEM_REENTRANT_CONFIG == BMEM_REENTRANT)
	BKNI_MutexHandle pMutex;         /* Semaphore to lock this heap if BMEM_REENTRANT */
#endif

	void *pvData;                    /* Pointer to private data allocated by heap manager */

	void *pvHeap;                    /* Original starting address passed during heap create */
	void *pvCache;                   /* Cached start address of the heap. */
	uint32_t ulOffset;               /* Device offset passed during heap create */
	size_t zSize;                    /* Size of the heap in bytes. */
	uint32_t ulNumAllocated;         /* Number of allocated blocks */
	uint32_t ulNumFree;              /* Number of free blocks */
	size_t ulTotalAllocated;         /* Total allocated memory */
	size_t ulTotalFree;              /* Total free memory */
	size_t ulHighWatermark;          /* High memory watermark (max memory used at once) */

	const BMEM_P_SafetyConfigInfo *pSafetyConfigInfo;
	uint32_t ulGuardSizeBytes;       /* Guard band size */

	uint32_t eBookKeeping;

	bool bCacheLocked;               /* Locks cache mapping from being changed once heap is allocated from */

	BMEM_FlushCallback pfFlushCb;    /* Callback used to flush cache at task time */
	BMEM_FlushCallback pfFlushCb_isr;/* Callback used to flush cache at isr time */

	BMEM_MonitorInterface *monitor;  /* Installed memory monitor */

	uint32_t (*pGetAddressFunc) (BMEM_Handle, BMEM_P_BlockInfo *);
	BMEM_P_BlockInfo* (*pGetBlockInfoFunc) (BMEM_Handle, uint32_t);
	void (*pDropBlockInfoFunc) (BMEM_Handle, BMEM_P_BlockInfo *);
	void (*pDestroyHeapFunc) (BMEM_Handle);
}
BMEM_P_Heap;

/*
Summary:
This structure defines the head element for creating
a linked list.
*/
typedef struct BMEM_HeapList BMEM_HeapList;
BLST_S_HEAD(BMEM_HeapList, BMEM_P_Heap);

typedef struct BMEM_P_AddrKey
{
	uint32_t         ulAddr;
} BMEM_P_AddrKey;

typedef struct BMEM_P_SizeKey
{
	uint32_t         ulAddr;
	uint32_t         ulSize;
} BMEM_P_SizeKey;

BDBG_OBJECT_ID_DECLARE(BMEM);

/***************************************************************************
 * BMEM_P_Mem
 *
 */
typedef struct BMEM_P_Mem
{
    BDBG_OBJECT(BMEM)
	BMEM_HeapList HeapList;
} BMEM_P_Mem;


#define BMEM_HEAP_ALIGN(pv) ((((uint32_t)(pv))+BMEM_HEAP_ALIGNMENT_MASK) & ~BMEM_HEAP_ALIGNMENT_MASK)

#define BMEM_CONFIG_DEFAULT 0

extern const BMEM_P_SafetyConfigInfo BMEM_P_SafetyConfigTbl[];
extern const uint32_t BMEM_P_SafetyTableSize;


/*
 * Functions to fiddle with the guard bytes
 */
void BMEM_P_FillGuard(BMEM_Handle pheap, BMEM_P_BlockInfo *pbi);
BERR_Code BMEM_P_CheckGuard(BMEM_Handle pheap, BMEM_P_BlockInfo *pbi);

#define BMEM_P_GetAddress(pheap, pbi) ((*(pheap->pGetAddressFunc))(pheap, pbi))
#define BMEM_P_GetBlockInfo(pheap, addr) ((*(pheap->pGetBlockInfoFunc))(pheap, addr))
#define BMEM_P_DropBlockInfo(pheap, pbi) ((*(pheap->pDropBlockInfoFunc))(pheap, pbi))
#define BMEM_P_Heap_CheckAddress(pheap, addr) ((addr >= pheap->pStart) && (addr <= pheap->pEnd))
#define BMEM_P_Heap_CheckCachedAddress(pheap, addr) (pheap->pvCache && (addr >= (uint8_t*)pheap->pvCache) && (addr < (uint8_t*)pheap->pvCache+pheap->zSize))

/*
 * These functions are provided by the local heap manager
 */
BERR_Code BMEM_P_LocalCreateHeap(BMEM_Handle *ppHeap, void *pvHeap, uint32_t ulOffset, size_t zSize, BMEM_Heap_Settings *pHeapSettings);
uint32_t BMEM_P_LocalGetAddress(BMEM_Handle pheap, BMEM_P_BlockInfo *pbi);
BMEM_P_BlockInfo *BMEM_P_LocalGetBlockInfo(BMEM_Handle pheap, uint32_t addr);
void BMEM_P_LocalDropBlockInfo(BMEM_Handle pheap, BMEM_P_BlockInfo *pbi);
void BMEM_P_LocalDestroyHeap(BMEM_Handle Heap);

/*
 * These functions are provided by the system heap manager
 */
BERR_Code BMEM_P_SystemCreateHeap(BMEM_Handle *ppHeap, void *pvHeap, uint32_t ulOffset, size_t zSize, BMEM_Heap_Settings *pHeapSettings);
uint32_t BMEM_P_SystemGetAddress(BMEM_Handle pheap, BMEM_P_BlockInfo *pbi);
BMEM_P_BlockInfo *BMEM_P_SystemGetBlockInfo(BMEM_Handle pheap, uint32_t addr);
void BMEM_P_SystemDropBlockInfo(BMEM_Handle pheap, BMEM_P_BlockInfo *pbi);
void BMEM_P_SystemDestroyHeap(BMEM_Handle Heap);

int BMEM_P_Addr_Map_Cmp(const BMEM_P_BlockInfo *pbi, const BMEM_P_AddrKey *pKey);
int BMEM_P_Size_Map_Cmp(const BMEM_P_BlockInfo *pbi, const BMEM_P_SizeKey *pKey);

#if (BMEM_REENTRANT_CONFIG == BMEM_REENTRANT)

#define BMEM_P_GET_SEMAPHORE(heap)     BKNI_AcquireMutex((heap)->pMutex)
#define BMEM_P_RELEASE_SEMAPHORE(heap) BKNI_ReleaseMutex((heap)->pMutex)

#else /* BMEM_NOT_REENTRANT */

#define BMEM_P_GET_SEMAPHORE(heap)		(BERR_SUCCESS)
#define BMEM_P_RELEASE_SEMAPHORE(heap)

#endif /* if (BMEM_REENTRANT_CONFIG == BMEM_REENTRANT) */

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* #ifndef BMEM_PRIV_H__ */

/* End of File */
