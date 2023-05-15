/***************************************************************************
 *	   Copyright (c) 2004-2012, Broadcom Corporation
 *	   All Rights Reserved
 *	   Confidential Property of Broadcom Corporation
 *
 *	THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *	AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *	EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxvd_userdata.c $
 * $brcm_Revision: Hydra_Software_Devel/127 $
 * $brcm_Date: 8/24/12 5:51p $
 *
 * Module Description:
 *	 This module controls and returns the User Data coming in the stream 
 * and captured by the decoder.
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/xvd/src/bxvd_userdata.c $
 * 
 * Hydra_Software_Devel/127   8/24/12 5:51p davidp
 * SW7425-3618: Replace uncached memory accesses with cached memory
 * accesses.
 * 
 * Hydra_Software_Devel/126   6/12/12 12:04p pblanco
 * SW7405-5668: Removed redundant code identified by Ray.
 * 
 * Hydra_Software_Devel/125   6/11/12 10:29a pblanco
 * SW7405-5668: Moved setting of BXVD_FLATTEN_USERDATA,
 * BXVD_BREAK_ON_TYPE_CHANGE and BXVD_USERDATA_EXTRA_DEBUG from the
 * bxvd_userdata.c source to the build environment.
 * 
 * Hydra_Software_Devel/124   6/11/12 8:48a pblanco
 * SW7405-5667: Fixed potential pointer dereference issue found by
 * Coverity v6.
 * 
 * Hydra_Software_Devel/123   12/1/11 2:50p pblanco
 * SW7425-1780: Previous checkin did not propagate queued value to AVC.
 * 
 * Hydra_Software_Devel/122   12/1/11 1:12p pblanco
 * SW7425-1780: Implemented picture id support for transcode userdata.
 * 
 * Hydra_Software_Devel/121   7/20/11 3:04p davidp
 * SW7420-2001: Reorder header file includes.
 * 
 * Hydra_Software_Devel/120   4/30/10 1:43p pblanco
 * SW7400-2753: Fix kernel oops when user data packet size exceeds maximum
 * buffer size.
 * 
 * Hydra_Software_Devel/119   3/25/10 11:50a pblanco
 * SWGIGGSVIZIO-4: Added NULL pointer check before BKNI_Memcpy.
 * 
 * Hydra_Software_Devel/118   3/18/10 11:44p pblanco
 * SW3548-2845: Changed BXVD_P_Userdata_QueueRemove to
 * BXVD_P_Userdata_QueueRemove_isr and changed its scope to static.
 * 
 * Hydra_Software_Devel/117   3/13/10 10:53a davidp
 * SW7400-2704: Userdaa header type nolong bit field.
 * 
 * Hydra_Software_Devel/116   3/3/10 5:27p davidp
 * SW7400-2704: Add SEI message frame packing support.
 * 
 * Hydra_Software_Devel/115   2/25/10 4:24p nilesh
 * SW7405-2993: XDM Merge
 * 
 * Hydra_Software_Devel/114   2/18/10 3:54p pblanco
 * SW7405-3939: Implemented and tested repeat first field fix.
 * 
 * Hydra_Software_Devel/113   11/30/09 4:41p btosi
 * SW7405-3245: added BXVD_DBG_* macros.  Map to either BDBG_INSTANCE_* or
 * BDBG_* at compile.
 * 
 * Hydra_Software_Devel/112   10/30/09 11:29a btosi
 * SW7405-3257: added support for measuring execution times
 * 
 * Hydra_Software_Devel/111   8/14/09 4:51p pblanco
 * PR27168: Just check returned address value for 0, not the error return
 * code. This is for compatibility with the single decode branch.
 * 
 * Hydra_Software_Devel/110   8/14/09 10:44a pblanco
 * PR27168: Check for an error from the offset to address convertion
 * routine and return without enqueing the packet.
 * 
 * Hydra_Software_Devel/109   7/23/09 2:57p pblanco
 * PR27168: Changed decimal output in extended debugging messages to
 * unsigned.
 * 
 * Hydra_Software_Devel/108   7/23/09 11:07a pblanco
 * PR27168: Remove unnecessary memory clear in read ISR.
 * 
 * Hydra_Software_Devel/107   7/23/09 8:45a pblanco
 * PR27168: Added "interpolated" PTS display to extra debugging output.
 * 
 * Hydra_Software_Devel/106   7/22/09 1:47p pblanco
 * PR27168: Added PTS to extra debugging output.
 * 
 * Hydra_Software_Devel/105   2/4/09 9:01a pblanco
 * PR51740: Remove extraneous BKNI_Free.
 * 
 * Hydra_Software_Devel/104   1/21/09 2:02p nilesh
 * PR45052: Converted BDBG_xxx to BDBG_INSTANCE_xxx calls to support
 * multiple channels
 * 
 * Hydra_Software_Devel/103   10/20/08 1:45p pblanco
 * PR48063: Fix dead code error reported by Coverity.
 * 
 * Hydra_Software_Devel/102   8/29/08 2:06p pblanco
 * PR45230: Modify BXVD_Userdata_Close to support a critical section.
 * 
 * Hydra_Software_Devel/101   7/31/08 4:33p pblanco
 * PR45230: Check for queue overflow in read ISR and set
 * bErrorBufferOverflow flag appropriately.
 * 
 * Hydra_Software_Devel/100   7/29/08 10:25a pblanco
 * PR45230: Moved static global error to the userdata context structure.
 * 
 * Hydra_Software_Devel/99   7/11/08 2:56p pblanco
 * PR29915: Fixed compiler warnings when built in PROXY mode
 * 
 * Hydra_Software_Devel/98   7/1/08 11:55a pblanco
 * PR44387: Removed code that allowed a callback with a NULL userdata
 * pointer.
 * 
 * Hydra_Software_Devel/97   6/16/08 1:20p pblanco
 * PR29915: Added better error handling and recovery to enqueueing and
 * read_isr routines.
 * 
 * Hydra_Software_Devel/96   6/3/08 9:27a pblanco
 * PR42910: Added handle type checking to externally visable APIs.
 * 
 * Hydra_Software_Devel/95   5/21/08 2:51p pblanco
 * PR42910: Add handle type to handle initialization.
 * 
 * Hydra_Software_Devel/94   5/21/08 1:50p pblanco
 * PR42910: Removed include of bxvd_userdata_priv.h. Its contents are now
 * in bxvd_priv.h
 * 
 * Hydra_Software_Devel/93   4/16/08 9:10a pblanco
 * PR35059: Fixed most recent Coverity issues
 * 
 * Hydra_Software_Devel/92   4/14/08 9:45a pblanco
 * PR41311: Added settings argument to (currently unused) QueueRemove in
 * code conditionalized for flattened userdata packets
 * 
 * Hydra_Software_Devel/91   4/9/08 9:56a pblanco
 * PR41311: Merge in Min's change from single decode branch.
 * 
 * Hydra_Software_Devel/90   4/8/08 11:39a pblanco
 * PR41311: Merged remainder of settable parameter changes from single
 * decode branch
 * 
 * Hydra_Software_Devel/89   4/7/08 7:08p pblanco
 * PR41311: Merged parameter settability changes from single decode branch
 * 
 * Hydra_Software_Devel/88   4/4/08 4:32p pblanco
 * PR41311: Backed out queue item size change. Queue depth can still be
 * adjusted
 * 
 * Hydra_Software_Devel/87   4/3/08 4:33p pblanco
 * PR41311: Made queue depth and item size runtime settable parameters
 * 
 * Hydra_Software_Devel/86   3/12/08 11:39a pblanco
 * PR40262: Fixed do/while loop logic error discovered by Thompson during
 * a Klockwork run.
 * 
 * Hydra_Software_Devel/85   1/8/08 2:35p pblanco
 * PR38593: Added support for AVS userdata handling
 * 
 * Hydra_Software_Devel/84   10/11/07 10:47a pblanco
 * PR35991: Fixed missing deallocation in error path found by Coverity
 * 
 * Hydra_Software_Devel/83   9/27/07 9:00a pblanco
 * PR29915: Added VC1 header code
 * 
 * Hydra_Software_Devel/82   9/19/07 8:47a pblanco
 * PR35059: Fixed potential code problems found by Coverity
 * 
 * Hydra_Software_Devel/81   9/14/07 9:21a btosi
 * PR29915: removed BERR_TRACE from BXVD_P_Userdata_EnqueueDataPointer()
 * 
 * Hydra_Software_Devel/80   9/12/07 9:26a pblanco
 * PR29915: Added BERR_TRACE calls back where appropriate. Removed
 * redundant BERR_Code declarations in a couple of functions.
 * 
 * Hydra_Software_Devel/79   9/10/07 2:39p pblanco
 * PR29915: Removed BERR_TRACE around all returns. A side effect of this
 * macro caused errors
 * 
 * Hydra_Software_Devel/78   9/10/07 1:04p pblanco
 * PR34636: Increase size of user data item from 512 to 2048 for all
 * platforms
 * 
 * Hydra_Software_Devel/77   9/6/07 5:36p nilesh
 * PR29915: Added BERR_TRACE wrapper around all return codes
 * 
 * Hydra_Software_Devel/76   8/13/07 4:08p nilesh
 * PR29915: Multi-decode merge to mainline
 * 
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/2   7/25/07 1:37p nilesh
 * PR29915: Cleaned up bxvd_priv.h and bxvd_vdec_info.h constants to match
 * XVD coding style
 * 
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/1   7/3/07 3:47p nilesh
 * PR29915: bxvd_priv.h cleanup
 * 
 * Hydra_Software_Devel/75   5/15/07 4:01p pblanco
 * PR27168: Move pointer assignment to body of do loop in enqueue.
 * 
 * Hydra_Software_Devel/74   5/14/07 3:36p pblanco
 * PR27168: Fixed check for NULL next pointer in enqueue routine.
 * 
 * Hydra_Software_Devel/73   4/11/07 1:34p pblanco
 * PR27168: Changed BDBG_WRN messages to BDBG_MSG calls in
 * EnqueueDataPointer.
 * 
 * Hydra_Software_Devel/72   4/11/07 11:34a pblanco
 * PR28732: Userdata read now returns a packet at a time instead of a
 * flattened list. This insures that the proper userdata type is returned
 * with the data.
 * 
 * Hydra_Software_Devel/71   4/9/07 12:59p pblanco
 * PR27168: Enhanced debugging output in userdata read routine.
 * 
 * Hydra_Software_Devel/70   4/2/07 3:31p pblanco
 * PR27168: Fixed typo in userdata content output debug message.
 * 
 * Hydra_Software_Devel/69   3/29/07 3:03p pblanco
 * PR27168: Cleaned up debug output and made it generally more useful.
 * 
 * Hydra_Software_Devel/68   3/27/07 2:02p pblanco
 * PR27168: Removed deprecated bxvd_mem.h include.
 * 
 * Hydra_Software_Devel/67   2/21/07 9:12a pblanco
 * PR26433: Set formatting to standard agreed upon within the XVD group on
 * 2/20/07.
 * 
 * Hydra_Software_Devel/66   2/20/07 1:46p pblanco
 * PR27683: Use VDEC_FLAG_PTS_PRESENT instead of hardwired 0x20.
 * 
 * Hydra_Software_Devel/65   2/15/07 3:51p pblanco
 * PR27683: Added PTS and PTS valid flag to userdata.
 * 
 * Hydra_Software_Devel/64   12/14/06 3:06p davidp
 * PR25443: Copy userdata on longword aligned boundary.
 * 
 * Hydra_Software_Devel/63   12/13/06 7:18p davidp
 * PR25443: Read all currently queued data in BXVD_Userdata_Read_isr().
 * 
 * Hydra_Software_Devel/62   12/12/06 3:02p davidp
 * PR25443: Merge Simplified FW API branch into mainline
 * 
 * Hydra_Software_Devel/61   12/11/06 1:21p pblanco
 * PR26433: Added comments to code and cleaned up formatting.
 * 
 * Hydra_Software_Devel/60   10/19/06 12:29p davidp
 * PR25021: Add BSTD_UNUSED(xvdInterruptCallBack) to
 * UninstallInterruptCallback routine.
 * 
 * Hydra_Software_Devel/59   9/8/06 11:13a pblanco
 * PR24149: Untested fix for occasional kernel error due to unaligned
 * pointer.
 * 
 * Hydra_Software_Devel/58   8/1/06 5:55p davidp
 * PR22967: Return userdata to FW if app callback is not installed.
 * 
 * Hydra_Software_Devel/57   7/26/06 1:01p davidp
 * PR22967: Userdata buffers are now always returned to FW.
 * 
 * Hydra_Software_Devel/56   7/21/06 12:02p pblanco
 * PR22673: Fixed bug referencing the proper area for userdata memory.
 * 
 * Hydra_Software_Devel/55   7/21/06 9:49a pblanco
 * PR22673: Added userdata offset to address conversion code.
 * 
 * Hydra_Software_Devel/54   7/18/06 12:11a nilesh
 * PR22673: Code restructure for 97400, 97401 B0, and 97118
 * 
 * Hydra_Software_Devel/PR22673/1   7/17/06 2:34p nilesh
 * PR22673: Restructure on 97401
 * 
 * Hydra_Software_Devel/53   7/6/06 9:40a pblanco
 * PR21943: Fixed conditional compilation for 7401 B0, broken when
 * conditionalizing for 7118.
 * 
 * Hydra_Software_Devel/52   7/5/06 3:20p pblanco
 * PR21943: Conditionalization for 7118
 * 
 * Hydra_Software_Devel/51   6/15/06 4:06p davidp
 * PR20017: Remove references to mmap'd FW heap
 * 
 * Hydra_Software_Devel/50   6/14/06 9:19a pblanco
 * PR20017: Insured that enqueue routine error path returns the userdata
 * buffer to FW via the correct heap based on chip type and version.
 * 
 * Hydra_Software_Devel/49   6/13/06 12:35p pblanco
 * PR20017: Conditionalize buffer setup for B0
 * 
 * Hydra_Software_Devel/48   6/13/06 9:48a pblanco
 * PR20017: Make sure userdata handle is NULLed on close.
 * 
 * Hydra_Software_Devel/47   6/9/06 6:25p davidp
 * PR21846: Fixed compiler warnings caused by gcc option "-O3"
 * 
 * Hydra_Software_Devel/46   6/9/06 1:16p pblanco
 * PR20017: Removed unused parameters from uninstall callback function.
 * 
 * Hydra_Software_Devel/45   5/26/06 1:54p davidp
 * PR21740: Store userdata context in channel context, convert userdata
 * addr using private memory heap info.
 * 
 * Hydra_Software_Devel/44   5/9/06 3:00p pblanco
 * PR19877: Added debugging messages to user data module.
 * 
 * Hydra_Software_Devel/43   2/15/06 1:56p pblanco
 * PR18545: Extended user data types for 7401/7400 in the same manner Mai
 * did for 7411.
 * 
 * Hydra_Software_Devel/42   2/14/06 12:59p pblanco
 * PR19566: We no longer attempt to queue a NULL data pointer.
 * BXVD_P_Userdata_EnqueueDataPointer now returns a
 * BXVD_ERR_USERDATA_NONE in this case.
 * 
 * Hydra_Software_Devel/41   1/26/06 4:03p davidp
 * PR19123: Remove printf debug messages, clean up compiler warnings:
 * 
 * Hydra_Software_Devel/40   1/18/06 3:18p davidp
 * PR16792: Static FW buffers are part of FW Code heap, use proper heap
 * for bmem address conversion routines.:
 * 
 * Hydra_Software_Devel/39   1/18/06 1:41p pblanco
 * PR19123: Modifications for 7400 port. Conditionally include proper
 * bchp_740x.h file depending on platform.
 * 
 * Hydra_Software_Devel/38   1/16/06 1:28p davidp
 * PR16792: BXVD_Open now uses two heap pointers, one for FW code (2MB)
 * the other for FW picture buffers.:
 * 
 * Hydra_Software_Devel/37   1/10/06 10:07a pblanco
 * PR16052: Added addistional error checking and recovery to enqueue data
 * function.
 * 
 * Hydra_Software_Devel/36   1/3/06 4:28p darnstein
 * PR18545: Eliminate userdata_type enum for DSS. Simplify remaining
 * enums. This was the consensus reached with David Erickson today, after
 * consulting with Bill Fassl.
 * 
 * Hydra_Software_Devel/35   12/23/05 10:02a pblanco
 * PR18797: Fixed uninitialized variable issue.
 * 
 * Hydra_Software_Devel/34   12/20/05 10:23a pblanco
 * PR18545: Changed user data types to new definitions and added H264 &
 * DSS recognition.
 * 
 * Hydra_Software_Devel/33   12/14/05 9:59a pblanco
 * PR16052: Changed install/uninstall callback functions to accept a
 * standard BINT_CallbackFunc type.
 * 
 * Hydra_Software_Devel/32   12/9/05 3:19p vsilyaev
 * PR 18019: Fixed include files
 * 
 * Hydra_Software_Devel/31   12/9/05 10:59a pblanco
 * PR16052: Fix potential back to back buffer release problem in read
 * code.
 * 
 * Hydra_Software_Devel/30   12/8/05 12:59p pblanco
 * PR16052: Increased queue depth from 64 to 128.
 * 
 * Hydra_Software_Devel/29   12/8/05 10:33a pblanco
 * PR16052: BXVD_P_EnqueueDataPointer now handles queue overflow condition
 * properly.
 * 
 * Hydra_Software_Devel/26   12/6/05 2:50p pblanco
 * PR18411: Modified BXVD_Userdata_Read_isr so that user data buffers are
 * returned properly to the firmware.
 * 
 * Hydra_Software_Devel/25   12/5/05 3:41p pblanco
 * PR16052: More debugging messages.
 * 
 * Hydra_Software_Devel/24   12/5/05 9:55a pblanco
 * PR16052: Restored byte swapping for little endian mode and added some
 * BDBG_MSGs.
 * 
 * Hydra_Software_Devel/22   11/28/05 3:48p pblanco
 * PR16052: Incorporated Mai's latest changes to 7411 bxvd_userdata.c
 * 
 * Hydra_Software_Devel/21   11/23/05 12:31p pblanco
 * PR16052: Pre-holiday/power shutdown sanity check in.
 * 
 * Hydra_Software_Devel/19   11/22/05 9:46a pblanco
 * PR16052: Code changes required to user data after debugging with
 * Brutus.
 * 
 * Hydra_Software_Devel/18   11/18/05 12:36p pblanco
 * PR16052: Make sure that install callback disables user data by default.
 * 
 * Hydra_Software_Devel/17   11/18/05 9:23a pblanco
 * PR16052: Added code to set field polarity and pulldown flags in user
 * providedffer.
 * 
 * Hydra_Software_Devel/12   11/14/05 2:47p pblanco
 * PR16052: Temporarily remove assert checking for NULL user data pointer
 * in BXVD_P_Userdata_EnqueueDataPointer.
 * 
 * Hydra_Software_Devel/11   11/14/05 1:24p pblanco
 * PR16052: More 7401 specific code changes.
 * 
 * Hydra_Software_Devel/10   11/11/05 9:48a pblanco
 * PR16052: Added channel handle argument to
 * BXVD_P_Userdata_EnqueueDataPointer.
 * 
 * Hydra_Software_Devel/9   11/11/05 7:27a pblanco
 * PR16052: Added skeleton for BXVD_P_Userdata_EnqueueDataPointer so DM
 * can begin integration.
 * 
 * Hydra_Software_Devel/8   11/9/05 9:23a pblanco
 * PR16052: Additional changes to converge on the 7401 model of user data
 * acquisition.
 * 
 * Hydra_Software_Devel/7   10/20/05 9:24p pblanco
 * PR16052: Added brute force queue flush to user data enable.
 * 
 * Hydra_Software_Devel/6   10/20/05 8:52p pblanco
 * PR16052: Added enable API. TODO: flush queue before re-enabling
 * callback
 * 
 * Hydra_Software_Devel/5   10/7/05 3:29p pblanco
 * PR16052: Ported existing 7411 code to the 7401 model as we know it
 * today.
 * 
 * Hydra_Software_Devel/4   9/21/05 5:44p davidp
 * PR16052:  Add additional func parameter to BXVD_CallbackFunc
 * definition.:
 * 
 * Hydra_Software_Devel/3   8/23/05 10:22a pblanco
 * PR16052: Removed include of bxvd_temp_defs.h
 * 
 * Hydra_Software_Devel/2   7/7/05 4:13p pblanco
 * PR16052: Check in after fixing CC problems.
 * 
 * Hydra_Software_Devel/1   7/7/05 10:42a pblanco
 * PR16052: Added
 * 
 * 
 ***************************************************************************/
#include "bstd.h"				 /* standard types */
#include "bavc.h"				 /* for userdata */
#include "bdbg.h"				 /* Dbglib */
#include "bkni.h"				 /* malloc */
#include "bxvd.h"
#include "bxvd_platform.h"
#include "bxvd_priv.h"
#include "bxvd_userdata.h"
#include "bxvd_vdec_info.h"

BDBG_MODULE(BXVD_USERDATA);

/* Default settings. */
static const BXVD_Userdata_Settings s_stUserdataDefaultSettings =
{
   (4 * 1024),   /* default 4 kbyte user data size */
   BXVD_P_USERDATA_QUEUE_MAX,
   BXVD_P_USERDATA_ITEM_SIZE
};

/* Initialize the userdata read queue */
BERR_Code BXVD_P_Userdata_QueueInitialize(QUEUE_MGR *queue, BXVD_Userdata_Settings stUDSettings)
{
   int i;

   BDBG_ENTER(BXVD_P_Userdata_QueueInitialize);

#ifdef BXVD_USERDATA_EXTRA_DEBUG
   BKNI_Printf("<<< maxDataSize: %d >>>\n", stUDSettings.maxDataSize);
   BKNI_Printf("<<< maxQueueDepth: %d >>>\n", stUDSettings.maxQueueDepth);
   BKNI_Printf("<<< maxQueueItemSize: %d >>>\n", stUDSettings.maxQueueItemSize);
#endif

   queue->queue_data = (struct data *)BKNI_Malloc(stUDSettings.maxQueueDepth*sizeof(struct data));
   if (queue->queue_data == NULL)
      return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);

   BKNI_Memset(queue->queue_data, 0, stUDSettings.maxQueueDepth*sizeof(struct data));

   for (i = 0; i < stUDSettings.maxQueueDepth; i++)
   {
      queue->queue_data[i].uUserData = (unsigned char *)BKNI_Malloc(stUDSettings.maxQueueItemSize*sizeof(unsigned char));
      BKNI_Memset(queue->queue_data[i].uUserData, 0, stUDSettings.maxQueueItemSize);
   }

   queue->ulQueueDepth = 0;
   queue->ulReadPtr = BXVD_P_USERDATA_QUEUE_START;
   queue->ulWritePtr = BXVD_P_USERDATA_QUEUE_START;
   queue->ulNextPtr = BXVD_P_USERDATA_QUEUE_START;

   BDBG_LEAVE(BXVD_P_Userdata_QueueInitialize);
   return BERR_SUCCESS;
}


/* Clear the userdata queue. Just calls queue initialize */
BERR_Code BXVD_P_Userdata_QueueClear(QUEUE_MGR *queue, BXVD_Userdata_Settings stUDSettings)
{
   int i;
   queue->ulQueueDepth = 0;
   queue->ulReadPtr = BXVD_P_USERDATA_QUEUE_START;
   queue->ulWritePtr = BXVD_P_USERDATA_QUEUE_START;
   queue->ulNextPtr = BXVD_P_USERDATA_QUEUE_START;

   for (i = 0; i < stUDSettings.maxQueueDepth; i++)
      BKNI_Memset(queue->queue_data[i].uUserData, 0, stUDSettings.maxQueueItemSize);

   return BERR_SUCCESS;
}

BERR_Code BXVD_P_Userdata_QueueDestroy(QUEUE_MGR *queue, BXVD_Userdata_Settings stUDSettings)
{
   int i;

   queue->ulQueueDepth = 0;
   queue->ulReadPtr = BXVD_P_USERDATA_QUEUE_START;
   queue->ulWritePtr = BXVD_P_USERDATA_QUEUE_START;
   queue->ulNextPtr = BXVD_P_USERDATA_QUEUE_START;

   for (i = 0; i < stUDSettings.maxQueueDepth; i++)
      BKNI_Free(queue->queue_data[i].uUserData);

   if (queue->queue_data)
      BKNI_Free(queue->queue_data);

   return BERR_SUCCESS;
}

/***************************************************************************
 *  {secret}
 *  BXVD_P_Userdata_QueueInsert
 *  Adds data to the specified circular queue
 */
BERR_Code BXVD_P_Userdata_QueueInsert(QUEUE_MGR *queue,
                                      int protocol,
                                      unsigned long ulUserDataAddr,
                                      long          lUserDataSize,
                                      unsigned long ulFlags,
                                      unsigned long ulPulldown,
                                      unsigned long ulPTS,
                                      uint32_t uiDecodePictureId,
				      BXVD_Userdata_Settings stUDSettings)
{
   BDBG_ENTER(BXVD_P_Userdata_QueueInsert);

   /* Make sure the queue pointers are valid */
   if ((queue->ulWritePtr < BXVD_P_USERDATA_QUEUE_START) || 
       (queue->ulWritePtr >= stUDSettings.maxQueueDepth))
   {
      return BERR_TRACE(BXVD_ERR_QUEUE_CORRUPTED);
   }

   /* Fill in the queue's next pointer */
   queue->ulNextPtr = queue->ulWritePtr+1;

   /* Wrap around */
   if (queue->ulNextPtr == stUDSettings.maxQueueDepth)
   {
      queue->ulNextPtr = BXVD_P_USERDATA_QUEUE_START;
   }

   /* Check for queue overflow */
   if (queue->ulNextPtr == queue->ulReadPtr)
   {
      return BERR_TRACE(BXVD_ERR_QUEUE_FULL);
   }

   /* Write value to queue */
   queue->queue_data[queue->ulWritePtr].protocol = protocol;
   queue->queue_data[queue->ulWritePtr].ulFlags = ulFlags;
   queue->queue_data[queue->ulWritePtr].ulPulldown = ulPulldown;
   queue->queue_data[queue->ulWritePtr].ulPTS = ulPTS;
   queue->queue_data[queue->ulWritePtr].uiDecodePictureId = uiDecodePictureId;

   BKNI_Memset((unsigned char *)(queue->queue_data[queue->ulWritePtr].uUserData),
               0x0, 
               stUDSettings.maxQueueItemSize);

#if 0
   BKNI_Printf("lUserDataSize: %ld\n", lUserDataSize);
#endif

   if (lUserDataSize > stUDSettings.maxQueueItemSize)
   {
      BKNI_Printf("lUserSataSize(%lu) > maxQueueItemSize(%d) Truncating to maxQueueItemSize and copying with bErrorBufferOverflow set to true\n",
		  lUserDataSize, stUDSettings.maxQueueItemSize);
      BKNI_Memcpy((unsigned char *)(queue->queue_data[queue->ulWritePtr].uUserData), 
		  (void *)ulUserDataAddr,
		  stUDSettings.maxQueueItemSize);
      queue->ulWritePtr = queue->ulNextPtr;
      queue->ulQueueDepth++;
      return BERR_TRACE(BXVD_ERR_USERDATA_ITEM_TOO_LARGE);
   }
   else
   {
   BKNI_Memcpy((unsigned char *)(queue->queue_data[queue->ulWritePtr].uUserData), 
               (void *)ulUserDataAddr,
               lUserDataSize);
   }

   queue->ulWritePtr = queue->ulNextPtr;
   queue->ulQueueDepth++;

   return BERR_SUCCESS;
}

/***************************************************************************
 *  {secret}
 *  BXVD_P_Userdata_QueueRemove_isr
 *  Remove next entry from specified circular queue.
 */
static BERR_Code BXVD_P_Userdata_QueueRemove_isr(QUEUE_MGR *queue,
                                                 int *protocol,
                                                 unsigned long *udp,
                                                 unsigned long *ulFlags,
                                                 unsigned long *ulPulldown,
                                                 unsigned long *ulPTS,
                                                 uint32_t *uiDecodePictureId,
                                                 BXVD_Userdata_Settings stUDSettings)
{
   BDBG_ENTER(BXVD_P_Userdata_QueueRemove_isr);

   /* Make sure there is data in the queue */
   if (queue->ulReadPtr == queue->ulWritePtr)
   {
      return BXVD_ERR_QUEUE_EMPTY;
   }


   /* Check the queue pointers for validity */
   if ((queue->ulWritePtr < BXVD_P_USERDATA_QUEUE_START) || 
       (queue->ulWritePtr >= stUDSettings.maxQueueDepth))
   {
      return BERR_TRACE(BXVD_ERR_QUEUE_CORRUPTED);
   }

   /* Return the userdata information from the queue */
   *protocol = queue->queue_data[queue->ulReadPtr].protocol;
   *udp = (unsigned long)queue->queue_data[queue->ulReadPtr].uUserData;
   *ulFlags = queue->queue_data[queue->ulReadPtr].ulFlags;
   *ulPulldown = queue->queue_data[queue->ulReadPtr].ulPulldown;
   *ulPTS = queue->queue_data[queue->ulReadPtr].ulPTS;
   *uiDecodePictureId = queue->queue_data[queue->ulReadPtr].uiDecodePictureId;

   /* Increment the userdata read pointer and decrement the depth */
   queue->ulReadPtr++;
   queue->ulQueueDepth--;

   /* Check for wrap around */
   if (queue->ulReadPtr == stUDSettings.maxQueueDepth)
   {
      queue->ulReadPtr = BXVD_P_USERDATA_QUEUE_START;
   }

   BDBG_LEAVE(BXVD_P_Userdata_QueueRemove_isr);
   return BERR_SUCCESS;
}

/***************************************************************************
 *  {secret}
 * BXVD_P_Userdata_EnqueueDataPointer
 */
BERR_Code BXVD_P_Userdata_EnqueueDataPointer(BXVD_ChannelHandle hXvdCh,
                                             int protocol,
                                             unsigned long p_UserData,
                                             unsigned long ulFlags,
                                             unsigned long ulPulldown,
                                             unsigned long ulPTS,
                                             uint32_t uiDecodePictureId)
{
   BERR_Code rc = BERR_SUCCESS;
   unsigned long ulUserDataAddr;
   UD_HDR *pHdrInfo = (UD_HDR *)NULL;

   BDBG_ENTER(BXVD_P_Userdata_EnqueueDataPointer);

   /* Initialize global error to success */
   if (hXvdCh->pUserData)
      hXvdCh->pUserData->errForwardError = BERR_SUCCESS;

   /*	
    * If the global user data context pointer isn't initialized, return 
    * an error.
    */
   if (hXvdCh->pUserData == 0)
   {
      BXVD_DBG_MSG(hXvdCh, ("BXVD_P_Userdata_EnqueueDataPointer: not initialized"));
      return BXVD_ERR_USERDATA_UNINITED;
   }
		
   /*
    * If the user data callback is disabled, return an error. This is actually
    * more of a warning than an indication of something wrong in the userdata
    * subsystem.
    */
   if (hXvdCh->pUserData->bCallbackEnabled == false)
   {
      BXVD_DBG_MSG(hXvdCh, ("BXVD_P_Userdata_EnqueueDataPointer: callback disabled"));
      return BXVD_ERR_USERDATA_DISABLED;
   }
		
   /* If no userdata callback is installed, return the buffer and signal an
    * error.
    */
   if (hXvdCh->pUserData->fUserdataCallback_isr == NULL)
   {
      BXVD_DBG_MSG(hXvdCh, ("BXVD_P_Userdata_EnqueueDataPointer: callback not installed"));
      return BXVD_ERR_USERDATA_DISABLED;
   }

   /*
    * If the user data pointer is NULL return a no data error.
    */
   if (p_UserData == 0)
   {
      BXVD_DBG_MSG(hXvdCh, ("BXVD_P_Userdata_EnqueueDataPointer: no data"));
      return BERR_TRACE(BXVD_ERR_USERDATA_NONE);
   }


   /* 
    * Get the protocol type and user data pointer from DM and convert it to
    * a virtual address before enqueueing.
    */
   ulUserDataAddr = 0;
   BXVD_P_CONVERT_UD_OFF2ADDR(hXvdCh->pUserData,
			      p_UserData,
			      &ulUserDataAddr);

   ulUserDataAddr = (unsigned long)BXVD_P_CALC_CACHED_FW_CONTEXT_ADDRS(ulUserDataAddr, hXvdCh);

   if (ulUserDataAddr == 0)
      return BERR_TRACE(BXVD_ERR_USERDATA_INVALID);


   /* Loop through user data following the next pointer until the last 
    * (or a single) packet is found. We call the application UD callback
    * each time a packet is found, converted and copied.
    */
   do
   {
      BERR_TRACE(BMEM_Heap_FlushCache(hXvdCh->hSystemHeap, (void *)ulUserDataAddr, sizeof(UD_HDR)));

      /* Extract the header information */
      pHdrInfo = (UD_HDR *)ulUserDataAddr;
      if (pHdrInfo == NULL)
      {
	 BXVD_DBG_MSG(hXvdCh, ("BXVD_P_Userdata_EnqueueDataPointer: bad userdata pointer"));
	 return BXVD_ERR_USERDATA_INVALID;
      }

      BERR_TRACE(BMEM_Heap_FlushCache(hXvdCh->hSystemHeap, (void *)ulUserDataAddr, ((sizeof(UD_HDR)+((pHdrInfo->size+3))) & ~3)));
      /* 
       * Enqueue the data. The uiDecodePictureId member was added for transcode 
       * userdata support. Jira: SW7425-1780
       */
      rc = BXVD_P_Userdata_QueueInsert(&((hXvdCh->pUserData)->queue),
                                       protocol,
                                       ulUserDataAddr,
                                       ((sizeof(UD_HDR)+((pHdrInfo->size+3))) & ~3), /* Make sure we copy long words, endianess issue */
                                       ulFlags,
				       ulPulldown,
                                       ulPTS,
                                       uiDecodePictureId,
				       hXvdCh->pUserData->sUserdataSettings);
      if (rc != BERR_SUCCESS)
      {
	 BXVD_DBG_ERR(hXvdCh, ("Could not enqueue user data packet"));
	 hXvdCh->pUserData->errForwardError = rc;
	 goto doCallback;
	 /*return rc;*/
      }
			
      /* Get the next user data packet, if any */
      pHdrInfo = (UD_HDR *)ulUserDataAddr;

      if (pHdrInfo->next)
      {
	 BXVD_P_CONVERT_UD_OFF2ADDR(hXvdCh->pUserData,
				    (unsigned long)pHdrInfo->next,
				    &ulUserDataAddr);
	 if (ulUserDataAddr == 0)
	    return BERR_TRACE(BXVD_ERR_USERDATA_INVALID);

         ulUserDataAddr = (unsigned long)BXVD_P_CALC_CACHED_FW_CONTEXT_ADDRS(ulUserDataAddr, hXvdCh);
      }
#if 0
      if (pHdrInfo)
      {
	 if (pHdrInfo->next)
	 {
	    BXVD_P_CONVERT_UD_OFF2ADDR(hXvdCh->pUserData,
				       (unsigned long)pHdrInfo->next,
				       &ulUserDataAddr);
	    if (ulUserDataAddr == 0)
	       return BERR_TRACE(BXVD_ERR_USERDATA_INVALID);
	 }
      }
#endif
   } while (pHdrInfo->next);

doCallback:

   /* Invoke application UD read callback */
   if (hXvdCh->pUserData->fUserdataCallback_isr)
   {
      hXvdCh->pUserData->fUserdataCallback_isr(hXvdCh->pUserData->pParm1,
					       hXvdCh->pUserData->parm2);
   }

   BDBG_LEAVE(BXVD_P_Userdata_EnqueueDataPointer);
   return rc;
}

/***************************************************************************
 * Get userdata default settings. Currently this is only the default
 * userdata buffer size (4K).
 ***************************************************************************/
BERR_Code BXVD_Userdata_GetDefaultSettings(BXVD_Userdata_Settings *pDefSettings)
{
   BDBG_ENTER(BXVD_Userdata_GetDefaultSettings);
   BDBG_ASSERT(pDefSettings);
		
   *pDefSettings = s_stUserdataDefaultSettings;
		
   BDBG_LEAVE(BXVD_Userdata_GetDefaultSettings);
   return BERR_SUCCESS;
}

/***************************************************************************
 * Open an instance of the userdata module.
 ***************************************************************************/
BERR_Code BXVD_Userdata_Open(BXVD_ChannelHandle            hXvdCh,
			     BXVD_Userdata_Handle         *phUserData,
			     const BXVD_Userdata_Settings *pDefSettings)
{
   BERR_Code               eStatus = BERR_SUCCESS;
   BXVD_P_UserDataContext *pUserdata = NULL;
  
   BDBG_ENTER(BXVD_Userdata_Open);
		
   BDBG_ASSERT(hXvdCh);
   BDBG_ASSERT(phUserData);
   BSTD_UNUSED(pDefSettings);
  
   /* Return null handle if we fail to create one */
   *phUserData = NULL;
  
   /* Allocate user data handle */
   pUserdata = (BXVD_P_UserDataContext*)(BKNI_Malloc(sizeof(BXVD_P_UserDataContext)));

   if(!pUserdata)
   {
      return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
   }
   /* Clear out the context and set defaults. */
   BKNI_Memset((void*)pUserdata, 0x0, sizeof(BXVD_P_UserDataContext));
		
   /* Set the handle type */
   pUserdata->eHandleType = BXVD_P_HandleType_Userdata;

   /* Take in default settings. */
   pUserdata->sUserdataSettings = (pDefSettings) ? 
      *pDefSettings : s_stUserdataDefaultSettings;

   /* Initialize userdata parameters */
   pUserdata->hXvdCh = hXvdCh;
   pUserdata->bCallbackEnabled = false;
   pUserdata->fUserdataCallback_isr = NULL;
   pUserdata->errForwardError = BERR_SUCCESS;
  
   /* Allocate the userdata work buffer */
   pUserdata->pBfr = BKNI_Malloc(pUserdata->sUserdataSettings.maxDataSize);
   BDBG_ASSERT((uint32_t)pUserdata->pBfr%4==0);
   if(!pUserdata->pBfr)
   {
      BKNI_Free(pUserdata);
      return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
   }
  
   /* All done. Return the new Channel context to user and set the global
    * user data handle
    */
   *phUserData = hXvdCh->pUserData = (BXVD_Userdata_Handle)pUserdata;
		
   /* Initialize the userdata queue */
   eStatus = BXVD_P_Userdata_QueueInitialize(&(hXvdCh->pUserData->queue), pUserdata->sUserdataSettings);

#ifdef BXVD_FLATTEN_USERDATA
   BXVD_DBG_MSG(hXvdCh, ("Userdata will be delivered as coalesced packets"));
#else
   BXVD_DBG_MSG(hXvdCh, ("Userdata will be delivered as single packets"));
#endif

   BDBG_LEAVE(BXVD_Userdata_Open);
   return eStatus;
}

/***************************************************************************
* Close a previously opened userdata instance
****************************************************************************/
BERR_Code BXVD_Userdata_Close(BXVD_Userdata_Handle hUserData)
{
   BERR_Code eStatus = BERR_SUCCESS;

   BDBG_ENTER(BXVD_Userdata_Close);
   BDBG_ASSERT(hUserData);

   /* Check handle type for correctness */
   if (hUserData->eHandleType != BXVD_P_HandleType_Userdata)
   {
      BDBG_ERR(("Invalid handle type passed to function"));
      return BERR_TRACE(BXVD_ERR_INVALID_HANDLE);
   }

   /* Clear internal state inside a critical section */
   BKNI_EnterCriticalSection();
   hUserData->bCallbackEnabled = false;
   hUserData->fUserdataCallback_isr = NULL;
   hUserData->hXvdCh->pUserData = NULL;
   BKNI_LeaveCriticalSection();


   BXVD_P_Userdata_QueueDestroy(&hUserData->queue,  hUserData->sUserdataSettings);

   /*
    * Release all allocated buffers
    */
   BKNI_Free(hUserData->pBfr);
   BKNI_Free(hUserData);
   hUserData = NULL;
  
   BDBG_LEAVE(BXVD_Userdata_Close);
   return eStatus;
}

/***************************************************************************
	* Read user data. Non-isr version
****************************************************************************/
BERR_Code BXVD_Userdata_Read(BXVD_Userdata_Handle   hUserData,
			     BAVC_USERDATA_info    *pUserDataInfo)
{
   BERR_Code status;

   /* Check handle type for correctness */
   if (hUserData->eHandleType != BXVD_P_HandleType_Userdata)
   {
      BDBG_ERR(("Invalid handle type passed to function"));
      return BERR_TRACE(BXVD_ERR_INVALID_HANDLE);
   }

   BKNI_EnterCriticalSection();
   status = BXVD_Userdata_Read_isr(hUserData, pUserDataInfo);
   BKNI_LeaveCriticalSection();
   return status;
}
	

/***************************************************************************
	* Read user data. ISR version
****************************************************************************/
BERR_Code BXVD_Userdata_Read_isr(BXVD_Userdata_Handle   hUserData,
				 BAVC_USERDATA_info *pUserDataInfo)
{
   int protocol;
   uint32_t uiDecodePictureId;
   unsigned long ulFlags, ulPulldown, ulPTS;
   BERR_Code       eStatus = BERR_SUCCESS;
   size_t	  offset;
   uint8_t	  *pDataBfr;
   unsigned 	  entries;
   unsigned long   ulUserDataAddr;
#ifdef BXVD_FLATTEN_USERDATA
   bool            bMoreUserdata;
#endif

   UD_HDR *pHdr;

   BDBG_ENTER(BXVD_Userdata_Read);
   BDBG_ASSERT(hUserData);
   BDBG_ASSERT(pUserDataInfo);
   
   protocol = 0;
   ulUserDataAddr = 0;
   uiDecodePictureId = 0;
   ulFlags = ulPulldown = ulPTS = 0;

   /* Check handle type for correctness */
   if (hUserData->eHandleType != BXVD_P_HandleType_Userdata)
   {
      BDBG_ERR(("Invalid handle type passed to function"));
      return BERR_TRACE(BXVD_ERR_INVALID_HANDLE);
   }

   /*
    * Clear overflow flag. We'll set it if a queue full condition is 
    * detected below.
    */
   pUserDataInfo->bErrorBufferOverflow = false;

   /* 
    * If there was an error detected in the enqueuing routine.
    * forward it to the user callback unless it was a queue overflow
    */
   if (hUserData->errForwardError == BXVD_ERR_QUEUE_FULL)
   {
      BDBG_ERR(("Queue full condition detected, bErrorBufferOverflow set"));
      pUserDataInfo->bErrorBufferOverflow = true;      
   }
   else if (hUserData->errForwardError != BERR_SUCCESS)
   {
      BDBG_ERR(("Error detected in enqueuing routine"));
      return BERR_TRACE(hUserData->errForwardError);
   }

   /* Clear out the user data info structure */
   BKNI_Memset((void *)pUserDataInfo, 0, sizeof(*pUserDataInfo));

   /* Get the userdata from the queue */
   if (BXVD_P_Userdata_QueueRemove_isr(&(hUserData->queue), 
                                       &protocol, 
                                       &ulUserDataAddr,
                                       &ulFlags, 
                                       &ulPulldown,
                                       &ulPTS,
                                       &uiDecodePictureId,
                                       hUserData->sUserdataSettings)
       == BXVD_ERR_QUEUE_EMPTY)
   {
      return BXVD_ERR_USERDATA_NONE;
   }

   /*
    * Overflow condition is checked by DM, so we can set overflow flag
    * to false here.
    */
   pUserDataInfo->bErrorBufferOverflow = false;
		
   /* Set field polarity flag */
   pUserDataInfo->bTopFieldFirst =
      (ulFlags & BXVD_P_PPB_FLAG_BOTTOM_FIRST) ? false : true;
		
   /* Set repeat flag */
   pUserDataInfo->bRepeatFirstField =
      ((ulPulldown == BXVD_P_PPB_PullDown_eTopBottomTop) ||
       (ulPulldown == BXVD_P_PPB_PullDown_eBottomTopBottom) ||
       (ulPulldown == BXVD_P_PPB_PullDown_eFrameX2) ||
       (ulPulldown == BXVD_P_PPB_PullDown_eFrameX3) ||
       (ulPulldown == BXVD_P_PPB_PullDown_eFrameX4)) ? true : false;
		
   /*
    * Copy the user data buffer pointer to the info structure and make a 
    * local copy for processing
    */
   pUserDataInfo->pUserDataBuffer = hUserData->pBfr;
   pDataBfr = (uint8_t *)hUserData->pBfr;

   /* Set up parsing loop initial conditions */
   offset = 0;
#ifdef BXVD_FLATTEN_USERDATA
   bMoreUserdata = true;
   /* Parse and format the user data */
   while( bMoreUserdata )
   {
#endif
      /* Pass on the PTS and PTS valid	 flag */
      pUserDataInfo->ui32PTS = ulPTS;
      pUserDataInfo->bPTSValid = (ulFlags&BXVD_P_PPB_FLAG_PTS_PRESENT)?true : false;

      /* Pass on the decode picture id */
      pUserDataInfo->ulDecodePictureId = uiDecodePictureId;

      /* Get Userdata info */
      pHdr = (UD_HDR *)ulUserDataAddr;

      /* 
       * Parse the user data.
       */
      /* Valid for mpeg2 only */
      if (pHdr->type & BXVD_P_PPB_MPEG_USERDATA_TYPE_I)
         pUserDataInfo->ePicCodingType = BAVC_USERDATA_PictureCoding_eI;
      else if (pHdr->type & BXVD_P_PPB_MPEG_USERDATA_TYPE_P)
         pUserDataInfo->ePicCodingType = BAVC_USERDATA_PictureCoding_eP;
      else
         pUserDataInfo->ePicCodingType = BAVC_USERDATA_PictureCoding_eB;
					
      /* Get userdata type */
      if (BXVD_IS_AVC(protocol))
      {
         pUserDataInfo->eUserDataType = BAVC_USERDATA_Type_eSEI;
      }
      else if (BXVD_IS_MPEG(protocol))
      {
         if (pHdr->type & BXVD_P_PPB_MPEG_USERDATA_TYPE_SEQ)
            pUserDataInfo->eUserDataType = BAVC_USERDATA_Type_eSeq;
         else if (pHdr->type & BXVD_P_PPB_MPEG_USERDATA_TYPE_GOP)
            pUserDataInfo->eUserDataType = BAVC_USERDATA_Type_eGOP;
         else
            pUserDataInfo->eUserDataType = BAVC_USERDATA_Type_ePicture;
      }
      else if (BXVD_IS_AVS(protocol))
      {
         pUserDataInfo->eUserDataType = BAVC_USERDATA_Type_eFrame;
      }
      else   /* VC1 */
      {
         if (pHdr->type & BXVD_P_PPB_VC1_USERDATA_TYPE_SEQ)
            pUserDataInfo->eUserDataType = BAVC_USERDATA_Type_eSeq;
         else if (pHdr->type & BXVD_P_PPB_VC1_USERDATA_TYPE_ENTRYPOINT)
            pUserDataInfo->eUserDataType = BAVC_USERDATA_Type_eEntryPoint;
         else if (pHdr->type & BXVD_P_PPB_VC1_USERDATA_TYPE_FLD)
            pUserDataInfo->eUserDataType = BAVC_USERDATA_Type_eField;
         else if (pHdr->type & BXVD_P_PPB_VC1_USERDATA_TYPE_FRM)
            pUserDataInfo->eUserDataType = BAVC_USERDATA_Type_eFrame;
         else
            pUserDataInfo->eUserDataType = BAVC_USERDATA_Type_eSlice;
      }
					
      /* mpeg2 and avc uses the same it fields defs */
      if (pHdr->type & BXVD_P_PPB_MPEG_USERDATA_TYPE_TOP)
         pUserDataInfo->eSourcePolarity = BAVC_Polarity_eTopField;
      else if (pHdr->type & BXVD_P_PPB_MPEG_USERDATA_TYPE_BOT)
         pUserDataInfo->eSourcePolarity = BAVC_Polarity_eBotField;
      else
         pUserDataInfo->eSourcePolarity = BAVC_Polarity_eFrame;
					
      /* Check the user data packet size */
      if(offset+4 > (size_t)hUserData->sUserdataSettings.maxDataSize)
      {
         BDBG_WRN(("user data packet is too big %u+%u(%u)[%u]", 
                   offset, 
                   4, 
                   offset+4, 
                   hUserData->sUserdataSettings.maxDataSize));
							
         eStatus = BERR_TRACE(BXVD_ERR_USERDATA_USRBFROFL);
         goto consume;
      }
					
      /* Check buffer alignment */
      if ((uint32_t)pDataBfr % 4)
      {
         BDBG_WRN(("user data buffer is unaligned"));
         eStatus = BERR_TRACE(BXVD_ERR_USERDATA_INVALID);
         goto consume;
							
      }
					
      /* Create simulated data headers based on protocol */
      if (BXVD_IS_AVC(protocol))
      {
         /* 	simulate NAL and SEI header */
         pDataBfr[offset++] = 0x00;
         pDataBfr[offset++] = 0x00;
         pDataBfr[offset++] = 0x00;
         pDataBfr[offset++] = 0x00;
							
         pDataBfr[offset++] = 0x01;
         pDataBfr[offset++] = 0x06;

         if (pHdr->type == BXVD_P_PPB_H264_USERDATA_TYPE_REGISTERED)
         {
            pDataBfr[offset++] = (uint8_t) BXVD_USERDATA_H264_TYPE_REGISTERED;
         }
         else if (pHdr->type == BXVD_P_PPB_H264_USERDATA_TYPE_FRAME_PACK)
         {
            pDataBfr[offset++] = (uint8_t) BXVD_USERDATA_H264_TYPE_FRAME_PACK;
         }
         else
         {
            pDataBfr[offset++] = (uint8_t) BXVD_USERDATA_H264_TYPE_UNREGISTERED;
         }

         pDataBfr[offset++] = (uint8_t)pHdr->size;
      } 
      else if (protocol == BAVC_VideoCompressionStd_eVC1)
      {
         pDataBfr[offset++] = 0x00;
         pDataBfr[offset++] = 0x00;
         pDataBfr[offset++] = 0x01;
         pDataBfr[offset++] = 0x1E;
      }
      else 
      {
         /* <MPEG-2> An extra 0 is prepended to keep data aligned to 32 bits */
         pDataBfr[offset++] = 0x00;
         pDataBfr[offset++] = 0x00;
         pDataBfr[offset++] = 0x00;
         pDataBfr[offset++] = 0x01;
      }
					
      /* Check the user data packet size again after header creation */
      if(offset+pHdr->size >
         (size_t)hUserData->sUserdataSettings.maxDataSize)
      {
         BDBG_WRN(("user data packet is too big %u+%u(%u)[%u]",
                   offset, 
                   pHdr->size, 
                   offset+pHdr->size, 
                   hUserData->sUserdataSettings.maxDataSize));
							
         eStatus = BERR_TRACE(BXVD_ERR_USERDATA_USRBFROFL);
         goto consume;
      }
					
      /* Get number of 32 bit entries */
      entries = (pHdr->size+3)>>2; 
					
      /* Copy segment of user data after verifying source and destination pointers */
      if (&(pDataBfr[offset]) == NULL || (void *)(ulUserDataAddr + sizeof(BXVD_P_UserData)) == NULL)
      {
	 BDBG_WRN(("Attempt to dereference a NULL user data buffer", NULL));
	 eStatus = BERR_TRACE(BXVD_ERR_USERDATA_INVALID);
	 goto consume;
      }
      BKNI_Memcpy((void *)&(pDataBfr[offset]),
                  (void *)(ulUserDataAddr + sizeof(BXVD_P_UserData)),
                  entries*4);

#if BSTD_CPU_ENDIAN == BSTD_ENDIAN_LITTLE
      { /* convert endianess */
         uint32_t data;
         unsigned i;
         for(i=0;i<entries;i++) 
         {
            data = ((uint32_t *)(pDataBfr+offset))[i];
            data = ((data >> 24) & 0xFF) |
               ((data >> 8) & 0xFF00) |
               ((data & 0xFF00) << 8) |
               ((data & 0xFF) << 24);
            ((uint32_t *)(pDataBfr+offset))[i] = data;
         }
      }
#elif 	BSTD_CPU_ENDIAN == BSTD_ENDIAN_BIG
/* do nothing */
#else	
#error	 "Not supported"
#endif

      /* Adjust packet size */
      offset += pHdr->size;
					
      /* Align to 32 bits and pad with 0 */
      switch(offset%4)
      {
         case 1:
            pDataBfr[offset++] = 0x00;
            /* Fallthrough */
         case 2:
            pDataBfr[offset++] = 0x00;
            /* Fallthrough */
         case 3:
            pDataBfr[offset++] = 0x00;
            /* Fallthrough */
         default:
            break;
      }
#ifdef BXVD_FLATTEN_USERDATA
      /* Get the userdata from the queue */
      if (BXVD_P_Userdata_QueueRemove_isr(&(hUserData->queue), 
                                          &protocol, 
                                          &ulUserDataAddr,
                                          &ulFlags, 
                                          &ulPulldown,
                                          &ulPTS,
                                          &uiDecodePictureId,
                                          hUserData->sUserdataSettings)
	  == BXVD_ERR_QUEUE_EMPTY)
      {
         bMoreUserdata = false;
      }
#ifdef BXVD_BREAK_ON_TYPE_CHANGE
      else
      {
         UD_HDR *tmp;
         tmp = (UD_HDR *)ulUserDataAddr;
         if (tmp->type != pHdr->type)
         {
            BDBG_MSG(("tmp->type : pHdr->type", tmp->type, pHdr->type));
            break;
         }
      }
#endif
   }
#endif
   /* Set the new packet size in user data info struct */
   pUserDataInfo->ui32UserDataBufSize = offset;

#ifdef BXVD_USERDATA_EXTRA_DEBUG
   {
      uint32_t x;
      BXVD_PTSInfo ptsInfo;
      BKNI_Printf("ui32UserDataBufSize = 0x%x (%d)\n", offset, offset);
      BKNI_Printf("pHdr->type = 0x%x (%d)\n", pHdr->type, pHdr->type);
      if (ulPTS == 0)
      {
	 BXVD_GetPTS_isr(hUserData->hXvdCh, &ptsInfo);
	 BKNI_Printf("Interpolated running PTS = 0x%x (%u) - ", 
		     ptsInfo.ui32RunningPTS,
		     ptsInfo.ui32RunningPTS);
	 BKNI_Printf("Interpolated effective PTS = 0x%x (%u)\n", 
		     ptsInfo.ui32EffectivePTS,
		     ptsInfo.ui32EffectivePTS);

      }
      else
	 BKNI_Printf("PTS = 0x%x (%d)\n", ulPTS, ulPTS);
      BKNI_Printf("pUserDataInfo->eUserDataType = 0x%x (%d)\n", 
                  pUserDataInfo->eUserDataType, 
                  pUserDataInfo->eUserDataType);
      for (x = 0; x < offset; x++)
      {
         if (!((x+1)%45))
            BKNI_Printf("<end>\n");
         BKNI_Printf("%2.2x ", pDataBfr[x]);
      }
      BKNI_Printf("<end>\n");
      BKNI_Printf("actual size: 0x%x (%d)\n", x, x);
      BKNI_Printf("------------------------------------------------------\n"); 
   }
#endif

  consume:
			
   BDBG_LEAVE(BXVD_Userdata_Read);
   return eStatus;
}

/***************************************************************************
 * Install the userdata read interrupt callback
 ***************************************************************************/
BERR_Code BXVD_Userdata_InstallInterruptCallback
(
   BXVD_Userdata_Handle    hUserData,
   BINT_CallbackFunc       xvdInterruptCallBack,
   void                   *pParm1,
   int                     parm2
   )
{
   BDBG_ENTER(BXVD_Userdata_InstallInterruptCallback);
  
   BDBG_ASSERT(hUserData);
   BDBG_ASSERT(xvdInterruptCallBack);
   BSTD_UNUSED(parm2);

   /* Check handle type for correctness */
   if (hUserData->eHandleType != BXVD_P_HandleType_Userdata)
   {
      BDBG_ERR(("Invalid handle type passed to function"));
      return BERR_TRACE(BXVD_ERR_INVALID_HANDLE);
   }

   hUserData->fUserdataCallback_isr = xvdInterruptCallBack;
   hUserData->pParm1 = pParm1;
   hUserData->parm2  = parm2;
  
   hUserData->bCallbackEnabled = false;
		
   BDBG_LEAVE(BXVD_Userdata_InstallInterruptCallback);
   return BERR_SUCCESS;
}

/***************************************************************************
 * Uninstall the read interrupt callback
 ***************************************************************************/
BERR_Code BXVD_Userdata_UninstallInterruptCallback
(
   BXVD_Userdata_Handle    hUserData,
   BINT_CallbackFunc       xvdInterruptCallBack
   )
{
   BDBG_ENTER(BXVD_Userdata_UninstallInterruptCallback);
		
   BDBG_ASSERT(hUserData);
   BSTD_UNUSED(xvdInterruptCallBack);

   /* Check handle type for correctness */
   if (hUserData->eHandleType != BXVD_P_HandleType_Userdata)
   {
      BDBG_ERR(("Invalid handle type passed to function"));
      return BERR_TRACE(BXVD_ERR_INVALID_HANDLE);
   }

   hUserData->bCallbackEnabled = false;
		
   hUserData->fUserdataCallback_isr = NULL;
   hUserData->pParm1 = 0;
   hUserData->parm2  = 0;
  
   BDBG_LEAVE(BXVD_Userdata_UninstallInterruptCallback);
   return BERR_SUCCESS;
}

/***************************************************************************
 * Enable userdata
 ***************************************************************************/
BERR_Code BXVD_Userdata_Enable
(
   BXVD_Userdata_Handle     hUserData,
   bool                     bEnable
   )
{
   BDBG_ENTER(BXVD_Userdata_Enable);
   BDBG_ASSERT(hUserData);
		
   /* Check handle type for correctness */
   if (hUserData->eHandleType != BXVD_P_HandleType_Userdata)
   {
      BDBG_ERR(("Invalid handle type passed to function"));
      return BERR_TRACE(BXVD_ERR_INVALID_HANDLE);
   }

   hUserData->bCallbackEnabled = bEnable;

   /* If user data is being enabled, clear the existing queue */
   if (bEnable == true)
   {
      BXVD_P_Userdata_QueueClear(&(hUserData->queue), hUserData->sUserdataSettings);
   }
		
   BDBG_LEAVE(BXVD_Userdata_Enable);
   return BERR_SUCCESS;
}
/* End of File */
