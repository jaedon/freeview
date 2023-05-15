/***************************************************************************
 *     Copyright (c) 2004-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxvd_intr.c $
 * $brcm_Revision: Hydra_Software_Devel/84 $
 * $brcm_Date: 9/21/12 4:04p $
 *
 * Module Description:
 *   See code
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/xvd/src/bxvd_intr.c $
 * 
 * Hydra_Software_Devel/84   9/21/12 4:04p davidp
 * SW7346-963: Access FW picture queues using cached memory space, fix
 * still picture release issue.
 * 
 * Hydra_Software_Devel/83   8/20/12 9:12a btosi
 * SW7405-4736: added wrappers for queue macros to aid with debug
 * 
 * Hydra_Software_Devel/82   7/11/12 3:52p davidp
 * SW7425-3419: Set BXVD_StillPictureBuffer Display Width and Height to
 * image width and height if not Display Width and Height not specified
 * in PPB,
 * 
 * Hydra_Software_Devel/81   7/3/12 4:22p davidp
 * SW7425-3419: Add support for DisplayWidth and DisplayHeight in
 * BXVD_StillPictureBuffer struct.
 * 
 * Hydra_Software_Devel/80   6/18/12 4:27p davidp
 * SW7435-164: Remove un-needed include file.
 * 
 * Hydra_Software_Devel/79   10/20/11 4:30p davidp
 * SW7335-1364: Add bDecodeError to BXVD_StillPictureBuffers, set true if
 * error occured during still decode.
 * 
 * Hydra_Software_Devel/78   6/8/11 1:04p btosi
 * SW7425-542: moved the delivery/release queue management macros to the
 * platform files
 * 
 * Hydra_Software_Devel/77   7/13/10 4:25p davidp
 * SW7405-4628: Remove seq header callback support, add stereo seq error
 * callback support.
 * 
 * Hydra_Software_Devel/76   2/25/10 4:24p nilesh
 * SW7405-2993: XDM Merge
 * 
 * Hydra_Software_Devel/75   1/6/10 2:28p btosi
 * SW7405-3137: merged branch into mainline
 * 
 * Hydra_Software_Devel/74   11/30/09 4:41p btosi
 * SW7405-3245: added BXVD_DBG_* macros.  Map to either BDBG_INSTANCE_* or
 * BDBG_* at compile.
 * 
 * Hydra_Software_Devel/73   10/7/09 10:28a nilesh
 * SW7405-2991: Added VC1 source size --> display size SAR-based scaling
 * 
 * Hydra_Software_Devel/72   9/30/09 3:15p nilesh
 * SW7405-3085: Target source polarity synchronization now only occurs if
 * there are multiple active decodes targeted for the same display
 * interrupt.
 *
 * Hydra_Software_Devel/71   9/29/09 2:11p nilesh
 * SW7405-3085: The target source polarity for interlaced mosaic channels
 * on a progressive display is now synchronized
 *
 * Hydra_Software_Devel/70   9/3/09 3:04p davidp
 * SW7405-2929: Remove debug printf.
 *
 * Hydra_Software_Devel/69   9/3/09 12:22p davidp
 * SW7405-2929: Return aspect ratio related data in
 * BXVD_StillPictureBuffers.
 *
 * Hydra_Software_Devel/68   9/1/09 4:47p davidp
 * SW7405-2963: Add support to printout AVD FW log in PDR_isr.
 *
 * Hydra_Software_Devel/67   6/25/09 6:05p davidp
 * PR56375: Remove unneeded callback for AVD FW command processing. This
 * fixes the polled debug method of cmd acknowledgement.
 *
 * Hydra_Software_Devel/66   3/26/09 6:13p nilesh
 * PR53580: Cleaned up DM structs
 *
 * Hydra_Software_Devel/65   1/27/09 1:48p nilesh
 * PR51621: Resolved possible global variable naming conflict by either
 * explicitly declaring globals as static (where possible) or adding
 * BXVD_P_ prefix
 *
 * Hydra_Software_Devel/64   1/21/09 3:11p nilesh
 * PR45052: Converted BDBG_xxx to BDBG_INSTANCE_xxx calls to support
 * multiple channels
 *
 * Hydra_Software_Devel/63   12/19/08 12:04p btosi
 * PR50241: added support for "OpenChannel" initialization, cleaned up DM
 * API
 *
 * Hydra_Software_Devel/62   12/17/08 2:33p btosi
 * PR50241: removed the extraneous DM function declarations
 *
 * Hydra_Software_Devel/61   8/20/08 4:24p davidp
 * PR45783: Set VDC picture parameter ChannelId to VDCRectangleNum after
 * calling DM.
 *
 * Hydra_Software_Devel/60   6/25/08 2:57p davidp
 * PR42177: Remove reset740x call in watchdog isr.
 *
 * Hydra_Software_Devel/59   5/14/08 12:31p davidp
 * PR21390: Add support for Aegis VICH interrupts.
 *
 * Hydra_Software_Devel/58   3/25/08 5:20p davidp
 * PR40316: Merge branch PR40316 changes to mainline, update CABAC bin
 * sizes per FW doc.
 *
 * Hydra_Software_Devel/PR40316/1   3/18/08 6:12p davidp
 * PR40316: FW now quietly stops after each still picture is decoded.
 * Remove explicit stop, release PPB in DecodeStillPicture routine.
 *
 * Hydra_Software_Devel/57   3/17/08 5:30p davidp
 * PR40316: Add still picture source type and buffer type to
 * BXVD_StillPictureBuffers data being returned in callback.
 *
 * Hydra_Software_Devel/56   1/15/08 5:21p davidp
 * PR38735: Add BXVD_LinkDecoders/UnlinkDecoders and associated data
 * structures.
 *
 * Hydra_Software_Devel/55   9/19/07 1:12p davidp
 * PR35058: Test hXvd for NULL before dereferencing in
 * BXVD_P_WatchdogInterrupt_isr.
 *
 * Hydra_Software_Devel/54   9/11/07 3:30p davidp
 * PR22327: Once a decode is started on a channel, DM must be called, even
 * if the decoder is stopped on that channel.
 *
 * Hydra_Software_Devel/53   8/30/07 12:20p nilesh
 * PR34430: Channel specific heaps are now used for address conversion
 *
 * Hydra_Software_Devel/52   8/22/07 11:50a nilesh
 * PR34240: Support moving eDisplayInterrupt and uiVDCRectangleNumber
 * settings from BXVD_ChannelSettings to BXVD_DecodeSettings
 *
 * Hydra_Software_Devel/51   8/21/07 7:15p nilesh
 * PR22327: Changed PicDataReady interrupt behavior to the following:
 *   - If no channels are open or no channels are decoding, XVD delivers
 * a single muted picture to the VDC callback that has the correct
 * polarity
 *   - Otherwise, a picture list will contain pictures of only channels
 * that are actively decoding
 *
 * Hydra_Software_Devel/50   8/21/07 4:21p davidp
 * PR34052: Access PCROffset_STC register thru platformInfo.stReg
 * structure.
 *
 * Hydra_Software_Devel/49   8/21/07 3:04p davidp
 * PR34052: Remove nolonger used register addr pointers from BXVD_Handle,
 * use platform_info.stReg structure to reference registers
 *
 * Hydra_Software_Devel/48   8/13/07 4:08p nilesh
 * PR29915: Multi-decode merge to mainline
 *
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/16   8/10/07 2:27p davidp
 * PR29915: Prevent inserting two BAVC_XVD_Picture items with the same
 * rectangle number in picture list being sent back to app for VDC.
 *
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/15   8/10/07 1:02p davidp
 * PR29915: Move VDCRectangleNum from BXVD_StartDecode settings to
 * BXVD_OpenChannel settings.
 *
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/14   8/7/07 3:45p davidp
 * PR29915: Order picture list being sent back to app for VDC to use.
 *
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/13   7/25/07 1:36p nilesh
 * PR29915: Cleaned up bxvd_priv.h and bxvd_vdec_info.h constants to match
 * XVD coding style
 *
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/12   7/24/07 12:55p nilesh
 * PR29915: Added BXVD_DeviceInterrupt_eDisplayInterrupt support from
 * mainline
 *
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/11   7/19/07 5:17p nilesh
 * PR29915: Fixed stripe_width value in still picture buffer
 *
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/10   6/28/07 1:36p davidp
 * PR29915: Move remaining data structure definitions for
 * bxvd_devcmds_priv.h to bxvd_vdec_api.h
 *
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/9   6/27/07 4:58p davidp
 * PR29915: Removed unused data types and data structures per code review.
 *
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/8   6/25/07 6:32p davidp
 * PR29915: Finish adding support for 2nd display device for each decoder
 *
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/7   6/19/07 12:12p nilesh
 * PR29915: Merge from mainline
 *
 * Hydra_Software_Devel/47   5/29/07 5:43p rayshap
 * PR31653: Fix error handling
 *
 * Hydra_Software_Devel/46   5/29/07 5:39p rayshap
 * PR31653: Add return on error condition for early exit
 *
 * Hydra_Software_Devel/45   5/24/07 3:54p nilesh
 * PR30249: Move display interrupt callback to end of Picture Data Ready
 * Interrupt handler
 *
 * Hydra_Software_Devel/44   5/14/07 11:30a nilesh
 * PR30249: Merged UOD 2.x changes to mainline
 *
 * Hydra_Software_Devel/PR30249/1   5/1/07 3:44p nilesh
 * PR30249: Changes to support UOD 2.x development
 *
 *
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/6   6/14/07 1:16p davidp
 * PR29915: Add support for 2nd display device interrupt.
 *
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/5   6/1/07 6:17p davidp
 * PR29915: Add multi-decode support to watchdog processing
 *
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/4   6/1/07 2:20p nilesh
 * PR29915: Still picture release is now delayed and occurs on
 * BXVD_StopDecode()
 *
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/3   5/29/07 5:30p nilesh
 * PR29915: Updated still picture compatibility mode to match discussed
 * design
 *
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/2   5/22/07 5:01p nilesh
 * PR29915: Added still picture support according to FW mosaic API
 * document v2.3 (TODO: Add compatibility mode)
 *
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/1   5/8/07 4:01p davidp
 * PR29915: Add support for multi-decode (channels)
 *
 * Hydra_Software_Devel/43   3/28/07 1:25p pblanco
 * PR27168: Changed hardwired constants to defined constants.
 *
 * Hydra_Software_Devel/42   3/27/07 2:03p pblanco
 * PR27168: Removed deprecated bxvd_mem.h include.
 *
 * Hydra_Software_Devel/41   3/15/07 10:37a pblanco
 * PR27168: Fixed compiler warning caused by previous checkin.
 *
 * Hydra_Software_Devel/40   3/15/07 10:25a pblanco
 * PR27168: Removed deprecated FW command event and associated code.
 *
 * Hydra_Software_Devel/39   12/14/06 3:05p davidp
 * PR25443: Signal FW Cmd done event at the end of mbx_isr routine.
 *
 * Hydra_Software_Devel/38   12/13/06 7:16p davidp
 * PR25443: Remove deprecated API routines and associated data structures.
 *
 * Hydra_Software_Devel/37   12/12/06 3:00p davidp
 * PR25443: Merge Simplified FW API branch into mainline
 *
 * Hydra_Software_Devel/36   12/11/06 2:52p nilesh
 * PR26414: Merge branch PR26414 to mainline
 *
 * Hydra_Software_Devel/PR26414/1   12/8/06 12:20p nilesh
 * PR26414: Fixed race condition when SeqHdr interrupt occurred before XVD
 * copied necessary data from the AVD ChannelOpen response for use by the
 * SeqHdr interrupt handler.
 *
 * Hydra_Software_Devel/35   11/17/06 4:51p nilesh
 * PR25868: Merged authenticated image support to mainline
 *
 * Hydra_Software_Devel/PR25868/2   11/17/06 3:48p nilesh
 * PR25868: Added NULL handle error checking in watchdog routines
 *
 * Hydra_Software_Devel/PR25868/1   11/16/06 3:38p nilesh
 * PR25868: Added authenticated image support
 *
 * Hydra_Software_Devel/34   10/27/06 5:23p davidp
 * PR25098: Fix compiler warning
 *
 * Hydra_Software_Devel/33   10/27/06 4:57p davidp
 * PR25098: Only set mute flag in xvd_to_vdc info structure if channel
 * state is set to mute when decoder is stopped.
 *
 * Hydra_Software_Devel/32   10/13/06 3:20p davidp
 * PR24448: Remove C++ style comment prefix.
 *
 * Hydra_Software_Devel/31   10/12/06 3:01p davidp
 * PR24448: Prevent DM from being called during BXVD_ProcessWathchdog
 *
 * Hydra_Software_Devel/30   9/12/06 3:39p davidp
 * PR24121: Enter critical section after XVD command done event is
 * signaled and before we test the ifBusy flag in SendDecodeCommand().
 *
 * Hydra_Software_Devel/29   8/1/06 5:58p davidp
 * PR22673: Add verify watchdog fired platform routine.
 *
 * Hydra_Software_Devel/28   7/31/06 7:50p davidp
 * PR22673: Add picture buffer address macros for chip specific use.
 *
 * Hydra_Software_Devel/27   7/18/06 12:10a nilesh
 * PR22673: Code restructure for 97400, 97401 B0, and 97118
 *
 * Hydra_Software_Devel/PR22673/1   7/17/06 2:33p nilesh
 * PR22673: Restructure on 97401
 *
 * Hydra_Software_Devel/26   7/17/06 1:56p pblanco
 * PR22673: Removed 7401a0 conditionalized code and restructured some of
 * the remaining conditional code.
 *
 * Hydra_Software_Devel/25   7/11/06 5:06p davidp
 * PR20017: Add FW inner loop Watchdog support, also add platform specific
 * SetupInterrupts routines
 *
 * Hydra_Software_Devel/24   7/6/06 9:41a pblanco
 * PR21943: Fixed conditional compilation for 7401 B0, broken when
 * conditionalizing for 7118.
 *
 * Hydra_Software_Devel/23   7/5/06 3:21p pblanco
 * PR21943: Conditionalization for 7118
 *
 * Hydra_Software_Devel/22   7/5/06 11:40a davidp
 * PR20017: Fix unref warning for 7400 build
 *
 * Hydra_Software_Devel/21   6/29/06 6:55p davidp
 * PR20017: Enable 7401 B0 watchdog support
 *
 * Hydra_Software_Devel/20   6/21/06 6:13p davidp
 * PR22259: Add Watchdog support and fix a couple problems related to L2
 * MBX interrupt processing
 *
 * Hydra_Software_Devel/19   6/15/06 4:07p davidp
 * PR20017: Remove references to mmap'd FW heap
 *
 * Hydra_Software_Devel/18   6/14/06 5:18p davidp
 * PR20017: Add 7401 B0 support for MBX, SeqHdr and DecodeStillPicture FW
 * generated interrupts.
 *
 * Hydra_Software_Devel/17   6/8/06 4:31p davidp
 * PR20017: Add support for FW/MIPS cmd mailbox interrupt.
 *
 * Hydra_Software_Devel/16   6/8/06 9:18a pblanco
 * PR20017: Changed chip version test conditionals from A0 and B0 to
 * BCHP_VER_A0 and BCHP_VER_B0 to conform with the rest of magnum.
 *
 * Hydra_Software_Devel/14   5/22/06 5:58p davidp
 * PR21712: Use 7400 decoder specific watchdog timers.
 *
 * Hydra_Software_Devel/12   5/11/06 6:08p davidp
 * PR21532: Created new BXVD_[Un]InstallDeviceInterruptCallback to handle
 * device instance interrupts for DecodeStillPicture and Watchdog.
 *
 * Hydra_Software_Devel/11   5/5/06 2:10p davidp
 * PR21156: Add BXVD_P_ReleasePPB_isr and related _isr routines for DM to
 * use at ISR time.
 *
 * Hydra_Software_Devel/10   4/28/06 2:46p davidp
 * PR20537: Decoder FW was not decoding the last slice.
 *
 * Hydra_Software_Devel/9   4/26/06 1:03p pblanco
 * PR20649: Added a temporary definition for BCHP_XPT_PCROFFSET_STC until
 * the RDB include files are straightened out.
 *
 * Hydra_Software_Devel/8   4/24/06 2:11p davidp
 * PR18043: Add support for Sequence Header interrupt processing
 *
 * Hydra_Software_Devel/7   4/17/06 4:40p davidp
 * PR20906: Add nowait support for 7400 SendDecoderCommand
 *
 * Hydra_Software_Devel/6   4/4/06 3:28p davidp
 * PR20353: Add capture still picture debug support for 7400
 *
 * Hydra_Software_Devel/5   3/28/06 6:14p davidp
 * PR20353: Add decoder private memory base address to luma/chroma buffer
 * address being returned to app.
 *
 * Hydra_Software_Devel/4   3/27/06 6:33p davidp
 * PR20353: Add Decode Still Picture support for 7400
 *
 * Hydra_Software_Devel/3   3/22/06 5:16p davidp
 * PR20347: Add L2 interrupt support
 *
 * Hydra_Software_Devel/2   3/8/06 12:48p pblanco
 * PR20077: Cleaned up some of the code... formatting only.
 *
 * Hydra_Software_Devel/1   3/8/06 11:18a pblanco
 * PR20077: Initial checkin.
 *
 ***************************************************************************/
#include "bstd.h"                /* standard types */
#include "bdbg.h"                /* Dbglib */
#include "bkni.h"                /* malloc */
#include "btmr.h"
#include "bxvd_platform.h"
#include "bxvd.h"
#include "bavc.h"
#include "bxvd_priv.h"
#include "bxvd_intr.h"
#include "bxvd_reg.h"
#include "bxvd_decoder.h"

#if BXVD_P_FW_DEBUG_DRAM_LOGGING
#include "bxvd_dbg.h"

#define BXVD_P_LOG_ENTRY_SIZE 80

static char LogBuf[BXVD_P_LOG_ENTRY_SIZE+4];
#endif

BDBG_MODULE(BXVD_INTR);

static void BXVD_S_SetAspectRatio(
   BXVD_P_PPB* pPPB,
   BXVD_StillPictureBuffers *pStillPicBuf
   )
{
   BXDM_Picture stXdmPicture;

   BDBG_ENTER(BXVD_S_SetAspectRatio);

   BXVD_Decoder_P_ComputeAspectRatio( pPPB, &stXdmPicture );

   pStillPicBuf->eAspectRatio = stXdmPicture.stAspectRatio.eAspectRatio;
   pStillPicBuf->uiSampleAspectRatioX = stXdmPicture.stAspectRatio.uiSampleAspectRatioX;
   pStillPicBuf->uiSampleAspectRatioY = stXdmPicture.stAspectRatio.uiSampleAspectRatioY;

   if ( false == stXdmPicture.stAspectRatio.bValid )
   {
      BDBG_MSG(("Unknown protocol/aspect ratio: %d/%#x", pPPB->protocol, pPPB->aspect_ratio));
   }

   BDBG_LEAVE(BXVD_S_SetAspectRatio);
   return;

} /* end of BXVD_S_SetAspectRatio() */


void BXVD_P_AVD_MBX_isr(void *pvXvd, int iParam2)
{
   BXVD_DecoderContext  *pDecCntx = pvXvd;

   BDBG_ENTER(BXVD_P_AVD_MBX_isr);
   BSTD_UNUSED(iParam2);

   BXVD_DBG_MSG(pDecCntx->hXvd, ("MBX_isr"));

   BKNI_SetEvent(pDecCntx->hFWCmdDoneEvent);

   BDBG_LEAVE(BXVD_P_AVD_MBX_isr);
}

void BXVD_P_AVD_StillPictureRdy_isr(void *pvXvd, int iParam2)
{
   BXVD_DecoderContext   *pDecCntx = pvXvd;
   BXVD_Handle   hXvd = pDecCntx->hXvd;
   BXVD_ChannelHandle hXvdCh = NULL;
   struct BXVD_P_InterruptCallbackInfo *pCallback;
   BXVD_P_PictureDeliveryQueue *pPictureDeliveryQueue;
   BXVD_P_PPB *pPPB = NULL;
   BXVD_StillPictureBuffers stAppStillPictBuffers;

   if (hXvd->bStillChannelAllocated)
   {
      /* Find the still channel handle */
      hXvdCh = hXvd->ahChannel[hXvd->uiStillChannelNum];
   }

   BDBG_ENTER(BXVD_P_AVD_StillPictureRdy_isr);
   BSTD_UNUSED(iParam2);

   if (hXvdCh != NULL)
   {
      uint32_t uiDeliveryQReadOffset;

      BXVD_P_DELIVERY_QUEUE_GET_ADDR( hXvdCh, pPictureDeliveryQueue );
      BXVD_P_DELIVERY_QUEUE_GET_READ_OFFSET( hXvdCh, uiDeliveryQReadOffset );

#if BXVD_P_FW_HIM_API
      BERR_TRACE(BMEM_Heap_FlushCache(hXvdCh->hSystemHeap, (void *)pPictureDeliveryQueue, sizeof(BXVD_P_PictureDeliveryQueue)));
#endif
      /* Get picture off of delivery queue */
      BDBG_ASSERT(hXvdCh->bStillPictureToRelease == false);

      /* Decoder stops after still picture is decoded */
      hXvdCh->eDecoderState = BXVD_P_DecoderState_eNotActive;

      hXvd->auiActiveDecodes[hXvdCh->sChSettings.eChannelMode][hXvdCh->eDisplayInterrupt]--;

      hXvdCh->bStillPictureToRelease = true;
      hXvdCh->uiStillDisplayElementOffset = (uint32_t)pPictureDeliveryQueue->display_elements[uiDeliveryQReadOffset - BXVD_P_INITIAL_OFFSET_DISPLAY_QUEUE];
      BMEM_ConvertOffsetToAddress(hXvdCh->hSystemHeap,
				  hXvdCh->uiStillDisplayElementOffset,
				  (void*) &pPPB);

      BXVD_P_INCREMENT_2BASED_OFFSET( uiDeliveryQReadOffset, 1 );

      BXVD_P_DELIVERY_QUEUE_SET_READ_OFFSET( hXvdCh, uiDeliveryQReadOffset );

      pCallback = &hXvd->stDeviceInterruptCallbackInfo[BXVD_DeviceInterrupt_eDecodedStillBufferReady];

      if (pCallback && pCallback->BXVD_P_pAppIntCallbackPtr)
      {
         switch (pPPB->protocol)
         {
            case BAVC_VideoCompressionStd_eMPEG2:
            case BAVC_VideoCompressionStd_eMPEG1:
            case BAVC_VideoCompressionStd_eMPEG2DTV:
            case BAVC_VideoCompressionStd_eMPEG2_DSS_PES:

               stAppStillPictBuffers.ulDisplayWidth = pPPB->other.mpeg.display_horizontal_size;
               stAppStillPictBuffers.ulDisplayHeight = pPPB->other.mpeg.display_vertical_size;
               break;

            case BAVC_VideoCompressionStd_eVC1:
            case BAVC_VideoCompressionStd_eVC1SimpleMain:
         
               stAppStillPictBuffers.ulDisplayWidth = pPPB->other.vc1.display_horizontal_size;
               stAppStillPictBuffers.ulDisplayHeight = pPPB->other.vc1.display_vertical_size;
               break;

            case BAVC_VideoCompressionStd_eVP8:

               stAppStillPictBuffers.ulDisplayWidth = pPPB->other.vp8.display_horizontal_size; 
               stAppStillPictBuffers.ulDisplayHeight = pPPB->other.vp8.display_vertical_size;
               break;

            default:
               stAppStillPictBuffers.ulDisplayWidth = pPPB->video_width;
               stAppStillPictBuffers.ulDisplayHeight = pPPB->video_height;
               break;
         }

         stAppStillPictBuffers.ulImageWidth = pPPB->video_width;
         stAppStillPictBuffers.ulImageHeight = pPPB->video_height;

         switch (pPPB->flags & BXVD_P_PPB_FLAG_SOURCE_TYPE_MASK)
         {
            case BXVD_P_PPB_FLAG_PROGRESSIVE_SRC:
               stAppStillPictBuffers.eSourceType = BXVD_SourceType_eProgressive;
               break;
            case BXVD_P_PPB_FLAG_INTERLACED_SRC:
               stAppStillPictBuffers.eSourceType = BXVD_SourceType_eInterlaced;
               break;
            case BXVD_P_PPB_FLAG_UNKNOWN_SRC:
               stAppStillPictBuffers.eSourceType = BXVD_SourceType_eUnknown;
               break;
         }

         switch (pPPB->flags & BXVD_P_PPB_FLAG_BUFFER_TYPE_MASK)
         {
            case BXVD_P_PPB_FLAG_FRAME:
               stAppStillPictBuffers.eBufferType = BXVD_BufferType_eFrame;
               break;

            case BXVD_P_PPB_FLAG_FIELDPAIR:
               stAppStillPictBuffers.eBufferType = BXVD_BufferType_eFieldPair;
               break;

            case BXVD_P_PPB_FLAG_TOPFIELD:
               stAppStillPictBuffers.eBufferType = BXVD_BufferType_eTopField;
               break;

            case BXVD_P_PPB_FLAG_BOTTOMFIELD:
               stAppStillPictBuffers.eBufferType = BXVD_BufferType_eBotField;
               break;
         }

         if (stAppStillPictBuffers.ulDisplayWidth != 0)
         {
            BXVD_DBG_MSG(hXvdCh, ("Still Pic: Display Width: %d, Display Height %d", 
                                  stAppStillPictBuffers.ulDisplayWidth,
                                  stAppStillPictBuffers.ulDisplayHeight));
         }

         BXVD_DBG_MSG(hXvdCh, ("Still Pic: Width: %d, Height %d", pPPB->video_width, pPPB->video_height));

         BXVD_DBG_MSG(hXvdCh, ("Still Pic: Source type : %d, Buffer type %d", (pPPB->flags & 0x0c), (pPPB->flags & 0x03)));

         BMEM_ConvertOffsetToAddress( hXvdCh->hPictureHeap, (uint32_t)pPPB->luma_video_address, (void *)&stAppStillPictBuffers.pLumaBuffer);
         BMEM_ConvertOffsetToAddress( hXvdCh->hPictureHeap, (uint32_t)pPPB->chroma_video_address, (void *)&stAppStillPictBuffers.pChromaBuffer);

         stAppStillPictBuffers.ulStripedWidth = BXVD_P_StripeWidthLUT[hXvd->uiDecode_StripeWidth];
         stAppStillPictBuffers.ulLumaStripedHeight = pPPB->luma_stripe_height;
         stAppStillPictBuffers.ulChromaStripedHeight = pPPB->chroma_stripe_height;

         BXVD_DBG_MSG(hXvdCh, ("Still Pic: LumaStripeHgt: %d, ChromaStripeHgt %d", pPPB->luma_stripe_height, pPPB->chroma_stripe_height));

         BXVD_S_SetAspectRatio(pPPB, &stAppStillPictBuffers);

         /* Set decode error when error bit set in PPB */
         stAppStillPictBuffers.bDecodeError = ((pPPB->flags & BXVD_P_PPB_FLAG_DECODE_ERROR) == BXVD_P_PPB_FLAG_DECODE_ERROR);

         pCallback->BXVD_P_pAppIntCallbackPtr(pCallback->pParm1,
                                              pCallback->parm2,
                                              &stAppStillPictBuffers);

         /* The release of the still picture on the release queue is
          * delayed until the next still picture decode to ensure the
          * contents of this still picture remain intact. See
          * BXVD_StopDecode() for the release code */
      }
   }
   BDBG_LEAVE(BXVD_P_AVD_StillPictureRdy_isr);
}

void BXVD_P_WatchdogInterrupt_isr(void *pvXvd, int param2)
{
   BXVD_Handle hXvd = pvXvd;
   BXVD_ChannelHandle hXvdCh;

   uint32_t  chanNum;
   bool watchdog_fired;

   struct BXVD_P_InterruptCallbackInfo *pCallback;

   BDBG_ENTER(BXVD_P_WatchdogInterrupt_isr);
   BXVD_DBG_MSG(hXvd, ("BXVD Watchdog fired!!!"));

   if (hXvd != NULL)
   {
      watchdog_fired = BXVD_P_VERIFY_WATCHDOG_FIRED(hXvd, param2);

      if (watchdog_fired)
      {
         hXvd->eAVDBootMode = BXVD_AVDBootMode_eWatchdog;

         for ( chanNum = 0; chanNum < BXVD_MAX_VIDEO_CHANNELS; chanNum++)
         {
            hXvdCh = hXvd->ahChannel[chanNum];

            if (hXvdCh != NULL)
            {
               /* Only reset DM on Video channels, not Still Picture channel */
               if (hXvdCh->sChSettings.eChannelMode == BXVD_ChannelMode_eVideo)
               {
                  BXDM_PictureProvider_WatchdogReset_isr(hXvdCh->hPictureProvider);
               }
            }
         }

         /* Notify application if watchdog callback is registered */
         pCallback = &hXvd->stDeviceInterruptCallbackInfo[BXVD_DeviceInterrupt_eWatchdog];

	 if (pCallback->BXVD_P_pAppIntCallbackPtr)
	 {
	    pCallback->BXVD_P_pAppIntCallbackPtr(pCallback->pParm1,
						 pCallback->parm2,
						 0);
	 }
      }
   }
   BDBG_LEAVE(BXVD_P_WatchdogInterrupt_isr);
}


void BXVD_P_VidInstrChkr_isr(void *pvXvd, int param2)
{
   BXVD_Handle hXvd = pvXvd;

   struct BXVD_P_InterruptCallbackInfo *pCallback;

   BDBG_ENTER(BXVD_P_VidInstrChkr_isr);

   BXVD_DBG_MSG(hXvd, ("BXVD Video Instruction Checker triggered!!!"));

   if (hXvd != NULL)
   {
      pCallback = &hXvd->stDeviceInterruptCallbackInfo[BXVD_DeviceInterrupt_eVidInstrChecker];

      if (pCallback->BXVD_P_pAppIntCallbackPtr)
      {
         pCallback->BXVD_P_pAppIntCallbackPtr(pCallback->pParm1,
                                              pCallback->parm2,
                                              (void *)&param2);
      }
   }
   BDBG_LEAVE(BXVD_P_VidInstrChkr_isr);
}


void BXVD_P_StereoSeqError_isr(void *pvXvd, 
                               int param2)
{
   BXVD_Handle hXvd = pvXvd;

   struct BXVD_P_InterruptCallbackInfo *pCallback;

   BDBG_ENTER(BXVD_P_StereoSeqError_isr);

   BXVD_DBG_MSG(hXvd, ("BXVD Stereo Sequence Error!!!"));

   if (hXvd != NULL)
   {
      pCallback = &hXvd->stDeviceInterruptCallbackInfo[BXVD_DeviceInterrupt_eStereoSeqError];

      if (pCallback->BXVD_P_pAppIntCallbackPtr)
      {
         pCallback->BXVD_P_pAppIntCallbackPtr(pCallback->pParm1,
                                              pCallback->parm2,
                                              (void *)&param2);
      }
   }

   BDBG_LEAVE(BXVD_P_StereoSeqError_isr);
}
