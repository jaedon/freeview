/***************************************************************************
 *     Copyright (c) 2004-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxvd_devcmds_priv.c $
 * $brcm_Revision: Hydra_Software_Devel/170 $
 * $brcm_Date: 8/24/12 5:51p $
 *
 * Module Description:
 *   See Module Overview below
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/xvd/src/bxvd_devcmds_priv.c $
 * 
 * Hydra_Software_Devel/170   8/24/12 5:51p davidp
 * SW7425-3618: Replace uncached memory accesses with cached memory
 * accesses.
 * 
 * Hydra_Software_Devel/169   4/5/12 6:08p davidp
 * SW7435-8: Printout FW command when command status is not successful.
 * 
 * Hydra_Software_Devel/168   3/14/12 3:22p davidp
 * SW7435-8: Add more detailed error handling output.
 * 
 * Hydra_Software_Devel/167   1/20/12 3:48p davidp
 * SW7208-175: Add support for 1920 portrait decode.
 * 
 * Hydra_Software_Devel/166   6/27/11 4:13p davidp
 * SW7425-542: Merge devel branch to mainline.
 * 
 * Hydra_Software_Devel/SW7425-542/4   6/14/11 10:14a btosi
 * SW7425-542: cleaned up warning
 * 
 * Hydra_Software_Devel/SW7425-542/3   6/9/11 1:19p btosi
 * SW7425-542: added in support for the Host Memory Interface
 * 
 * Hydra_Software_Devel/SW7425-542/2   6/7/11 5:46p davidp
 * SW7425-542: Use macros to access display info.
 * 
 * Hydra_Software_Devel/SW7425-542/1   5/31/11 3:16p davidp
 * SW7425-542: Add support for Host Interface Memory (ARC Scratch memory)
 * FW API changes.
 * 
 * Hydra_Software_Devel/165   4/4/11 4:37p davidp
 * SW7425-284: Add XPT Rave register info to AVD FW init command.
 * 
 * Hydra_Software_Devel/164   10/15/10 6:37p davidp
 * SW7425-16: Add support for base layer ARC, additional FW api memory
 * arguments.
 * 
 * Hydra_Software_Devel/163   8/3/10 9:06a pblanco
 * SW7400-2857: Reduced BXVD_Open memory footprint by 512 bytes by moving
 * FW command and reply buffers to the XVD handle structure.
 * 
 * Hydra_Software_Devel/162   7/30/10 2:10p pblanco
 * SW7400-2857: Backed out memory reduction changes.
 * 
 * Hydra_Software_Devel/160   4/23/10 8:22a davidp
 * SW7125-365: Create BXVD_FW_CMD and BXVD_FW_RSP union type to fix new
 * compiler strict alias rules.
 * 
 * Hydra_Software_Devel/159   2/17/10 2:51p davidp
 * SW7468-74: Add chip_prod_revision field to FW init command.
 * 
 * Hydra_Software_Devel/158   1/19/10 4:45p davidp
 * SW7405-3667: Clear decoder context bIfBusy flag on FW command timeout
 * and when resetting the decoder.
 * 
 * Hydra_Software_Devel/157   1/6/10 2:28p btosi
 * SW7405-3137: merged branch into mainline
 * 
 * Hydra_Software_Devel/156   12/11/09 1:03p davidp
 * SW7405-3572: Change ChannelClose command argument to use ChannelHandle
 * instead of ChannelId.
 * 
 * Hydra_Software_Devel/155   11/30/09 4:38p btosi
 * SW7405-3245: added BXVD_DBG_* macros.  Map to either BDBG_INSTANCE_* or
 * BDBG_*
 * at compile.
 * 
 * Hydra_Software_Devel/154   8/19/09 5:04p davidp
 * PR56375: Remove BXVD_P_HostCmdSendDecChannelOpenResponseHandler_isr,
 * this routine is nolonger needed.
 * 
 * Hydra_Software_Devel/153   8/7/09 7:32p davidp
 * PR56375: Disable FW MBX polling by default.
 * 
 * Hydra_Software_Devel/152   8/7/09 4:49p davidp
 * PR56375: Add poll FW MBX support for chip bringups.
 * 
 * Hydra_Software_Devel/151   6/25/09 6:05p davidp
 * PR56375: Remove unneeded callback for AVD FW command processing. This
 * fixes the polled debug method of cmd acknowledgement.
 * 
 * Hydra_Software_Devel/150   3/31/09 2:59p davidp
 * PR53685: Debug command string needed to be byte swapped on BE systems.
 * 
 * Hydra_Software_Devel/149   1/27/09 1:52p rayshap
 * PR51271: Add debug print
 * 
 * Hydra_Software_Devel/148   1/21/09 2:01p nilesh
 * PR45052: Converted BDBG_xxx to BDBG_INSTANCE_xxx calls to support
 * multiple channels
 * 
 * Hydra_Software_Devel/147   10/17/08 5:33p davidp
 * PR45799: Add Host L2 interrupt set register address to AVD FW init
 * command.
 * 
 * Hydra_Software_Devel/146   8/5/08 2:23p davidp
 * PR45080: Add support for AVD status block, block address now returned
 * in FW channel open command.
 * 
 * Hydra_Software_Devel/145   8/1/08 4:58p davidp
 * PR43272: Merge PR43272 branch to mainline, new FW memory configuration
 * API.
 * 
 * Hydra_Software_Devel/PR43272/1   7/17/08 12:37p davidp
 * PR43272: Add support for additional FW memory configuration parameters,
 * Direct mode and IL Work list memory.
 * 
 * Hydra_Software_Devel/144   6/10/08 4:29p davidp
 * PR38955: Add bvnf_intr_context_base to init command.
 * 
 * Hydra_Software_Devel/143   5/8/08 4:35p davidp
 * PR40234: Added timeout parameter to BXVD_P_SendDecoderCommand, FW debug
 * log command requires longer timeout.
 * 
 * Hydra_Software_Devel/142   4/25/08 6:20p davidp
 * PR40234: Use BKNI_Memcpy instead of strncpy.
 * 
 * Hydra_Software_Devel/141   4/21/08 10:47a nilesh
 * PR41952: Fixed compiler warning when DEBUG=n
 * 
 * Hydra_Software_Devel/140   4/18/08 3:19p davidp
 * PR40234: Use symbolic constants, code cleanup.
 * 
 * Hydra_Software_Devel/139   4/14/08 5:24p davidp
 * PR40234: Move dbg log memory parameters from FW init cmd to FW dbg log
 * control cmd.
 * 
 * Hydra_Software_Devel/138   4/11/08 4:53p davidp
 * PR40234: Merge decoder debug logging from branch to
 * Hydra_Software_Devel/.
 * 
 * Hydra_Software_Devel/PR40234/1   4/10/08 3:37p davidp
 * PR40234: Add support for decoder debug logging.
 * 
 * Hydra_Software_Devel/137   4/1/08 2:43p davidp
 * PR27168: Remove FW get version command.
 * 
 * Hydra_Software_Devel/136   2/26/08 4:42p davidp
 * PR39937: Remove unused BXVD_P_HostCmdDecChannelSpeed routine.
 * 
 * Hydra_Software_Devel/135   2/13/08 4:59p nilesh
 * PR38570: Added cabac worklist address and size to BDBG messages in
 * ChannelOpen()
 * 
 * Hydra_Software_Devel/134   1/29/08 5:54p davidp
 * PR39098: Remove enable uart macro, functionality moved to BSP code.
 * 
 * Hydra_Software_Devel/133   1/24/08 11:39a nilesh
 * PR38570: Merge CABAC worklist API changes to mainline
 * 
 * Hydra_Software_Devel/PR38570/1   1/21/08 5:11p nilesh
 * PR38570: Added support for cabac worklist buffer and size parameter to
 * AVD ChannelOpen
 * 
 * Hydra_Software_Devel/132   12/20/07 11:58a nilesh
 * PR36790: Fixed problem with XVD not sending channel number in
 * ChannelSpeed command.  Cleaned up BDBG messages.
 * 
 * Hydra_Software_Devel/131   9/12/07 11:50a nilesh
 * PR29915: Fixed error reporting when CPU2Host mailbox register is null
 * after a command response is received from the decoder
 * 
 * Hydra_Software_Devel/130   9/6/07 5:35p nilesh
 * PR29915: Added BERR_TRACE wrapper around all return codes
 * 
 * Hydra_Software_Devel/129   8/21/07 3:04p davidp
 * PR34052: Remove nolonger used register addr pointers from BXVD_Handle,
 * use platform_info.stReg structure to reference registers
 * 
 * Hydra_Software_Devel/128   8/13/07 4:07p nilesh
 * PR29915: Multi-decode merge to mainline
 * 
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/11   7/10/07 6:18p nilesh
 * PR29915: Added 7405 support to mosaic branch
 * 
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/10   7/10/07 10:41a nilesh
 * PR29915: Merged from mainline 07/10/2007
 * 
 * Hydra_Software_Devel/127   6/28/07 3:24p nilesh
 * PR32611: Fixed heaps used for offset->virtual address conversion for
 * data structures returned by firmware
 * 
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/9   6/28/07 1:36p davidp
 * PR29915: Move remaining data structure definitions for
 * bxvd_devcmds_priv.h to bxvd_vdec_api.h
 * 
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/8   6/27/07 4:57p davidp
 * PR29915: Removed unused data types and data structures per code review.
 * 
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/7   6/19/07 12:12p nilesh
 * PR29915: Merge from mainline.
 * 
 * Hydra_Software_Devel/126   5/15/07 1:20p pblanco
 * PR28215: Added support for variable stripe width and multiple.
 * 
 * Hydra_Software_Devel/125   5/14/07 11:30a nilesh
 * PR30249: Merged UOD 2.x changes to mainline
 * 
 * Hydra_Software_Devel/PR30249/1   5/10/07 2:09p nilesh
 * PR30249: Added support for new stripe_multiple parameter in AVD Init
 * 
 * Hydra_Software_Devel/124   4/24/07 1:39p nilesh
 * PR22766: Added support for new FW API to provide FGT buffer address and
 * size
 * 
 * Hydra_Software_Devel/123   4/10/07 2:34p davidp
 * PR29622: Change buffer widht and height being passed to FW, also
 * allocate less picture buffer memory.
 * 
 * Hydra_Software_Devel/122   3/27/07 10:32a pblanco
 * PR27168: Changed hardcoded constants to defined constants.
 * 
 * Hydra_Software_Devel/121   3/15/07 10:25a pblanco
 * PR27168: Removed deprecated FW command event and associated code.
 * 
 * Hydra_Software_Devel/120   3/1/07 3:27p gaurava
 * PR 28215: Removed the old UART frequency/baud params and added stripe
 * width.
 * 
 * Hydra_Software_Devel/119   12/12/06 2:55p davidp
 * PR25443: Merge Simplified FW API branch into mainline
 * 
 * Hydra_Software_Devel/118   12/11/06 2:51p nilesh
 * PR26414: Merge branch PR26414 to mainline
 * 
 * Hydra_Software_Devel/PR26414/2   12/11/06 10:53a nilesh
 * PR26414: Added AVD response callback error handling during a watchdog
 * reset
 * 
 * Hydra_Software_Devel/PR26414/1   12/8/06 12:20p nilesh
 * PR26414: Fixed race condition when SeqHdr interrupt occurred before XVD
 * copied necessary data from the AVD ChannelOpen response for use by the
 * SeqHdr interrupt handler.
 * 
 * Hydra_Software_Devel/117   11/21/06 5:29p gaurava
 * PR 25724: Removed CPB related information from ChannelStatus since they
 * are not available in 7401 and were there only in 7411 chip. 7401 uses
 * RAVE instead that has CDB/ITB.
 * 
 * Hydra_Software_Devel/116   9/28/06 2:14p pblanco
 * PR21690: Added debug message to still picture decode host command.
 * 
 * Hydra_Software_Devel/115   9/22/06 2:22p davidp
 * PR22867: Update FW memory usage table, add secure memory heap support.
 * 
 * Hydra_Software_Devel/114   9/19/06 7:23a pblanco
 * PR21690: Removed obsolete pool info messages from init command.
 * 
 * Hydra_Software_Devel/113   9/12/06 3:39p davidp
 * PR24121: Enter critical section after XVD command done event is
 * signaled and before we test the ifBusy flag in SendDecodeCommand().
 * 
 * Hydra_Software_Devel/112   9/11/06 6:37p davidp
 * PR24121: Add critical section lock around checking/setting device busy
 * flag in BXVD_P_SendDecoderCommand_isr().
 * 
 * Hydra_Software_Devel/111   9/8/06 10:36a nilesh
 * PR23113: Cleaned up code. Added buffer debug info in CMD_INIT
 * 
 * Hydra_Software_Devel/110   9/5/06 3:04p davidp
 * PR22867: Merge secure heap (Cabac) support
 * 
 * Hydra_Software_Devel/xvd_cabac_heap/1   8/30/06 5:10p davidp
 * 22867: Separate FW memory so Cabac bin memory can me moved to secure
 * heap.
 * 
 * Hydra_Software_Devel/109   8/29/06 8:27a pblanco
 * PR22673: Temporarily disabled mailbox interrupts for 7118... poll
 * instead.
 * 
 * Hydra_Software_Devel/108   8/9/06 3:06p davidp
 * PR20394: PCR TimeBase decoder setting now assigned a default value in
 * BXVD_OpenChannel.
 * 
 * Hydra_Software_Devel/107   8/2/06 5:23p davidp
 * PR22071: Replace usleep with BKNI_Sleep.
 * 
 * Hydra_Software_Devel/106   8/1/06 2:54p davidp
 * PR22673: Fix compiler warnings, uInstance unused.
 * 
 * Hydra_Software_Devel/105   7/27/06 8:14p davidp
 * PR22673: Move chip specific send FW command code to platform files.
 * 
 * Hydra_Software_Devel/104   7/20/06 2:23p nilesh
 * PR22673: Created BXVD_P_CONTEXT_PLATFORM macro to allow platform-
 * specific fields in BXVD_P_Context struct
 * 
 * Hydra_Software_Devel/103   7/18/06 12:09a nilesh
 * PR22673: Code restructure for 97400, 97401 B0, and 97118
 * 
 * Hydra_Software_Devel/PR22673/1   7/17/06 2:33p nilesh
 * PR22673: Restructure on 97401
 * 
 * Hydra_Software_Devel/102   7/17/06 1:56p pblanco
 * PR22673: Removed 7401a0 conditionalized code and restructured some of
 * the remaining conditional code.
 * 
 * Hydra_Software_Devel/101   7/6/06 9:41a pblanco
 * PR21943: Fixed conditional compilation for 7401 B0, broken when
 * conditionalizing for 7118.
 * 
 * Hydra_Software_Devel/100   7/5/06 3:20p pblanco
 * PR21943: Conditionalization for 7118
 * 
 * Hydra_Software_Devel/99   6/28/06 11:29a pblanco
 * PR20017: Conditionalize definition of heapInfo in
 * BXVD_P_SendDecoderCommand for A0 only.
 * 
 * Hydra_Software_Devel/98   6/21/06 6:13p davidp
 * PR22259: Add Watchdog support and fix a couple problems related to L2
 * MBX interrupt processing
 * 
 * Hydra_Software_Devel/97   6/21/06 1:52p pblanco
 * PR20017: Added BMEM_ValidateHeap call in BXVD_P_SendDecoderCommand
 * prior to host command address write for 7401b0.
 * 
 * Hydra_Software_Devel/96   6/20/06 5:42p pblanco
 * PR20017: Moved heap info request to A0 conditionalized code.
 * 
 * Hydra_Software_Devel/95   6/15/06 4:06p davidp
 * PR20017: Remove references to mmap'd FW heap
 * 
 * Hydra_Software_Devel/94   6/13/06 10:56a pblanco
 * PR20017: Removed unused address to offset conversion code per Rol
 * Lewis' email.
 * 
 * Hydra_Software_Devel/93   6/12/06 6:09p davidp
 * PR20017: Allocate memory for FW code out of framework heap.
 * 
 * Hydra_Software_Devel/92   6/12/06 10:56a pblanco
 * PR20017: Removed unused host commands for SET_ENDIAN, BUFFERCONFIG and
 * SETCHANNELACTIVE.
 * 
 * Hydra_Software_Devel/91   6/9/06 6:25p davidp
 * PR21846: Fixed compiler warnings caused by gcc option "-O3"
 * 
 * Hydra_Software_Devel/90   6/8/06 4:31p davidp
 * PR20017: Add support for FW/MIPS cmd mailbox interrupt.
 * 
 * Hydra_Software_Devel/89   6/8/06 9:18a pblanco
 * PR20017: Changed chip version test conditionals from A0 and B0 to
 * BCHP_VER_A0 and BCHP_VER_B0 to conform with the rest of magnum.
 * 
 * Hydra_Software_Devel/88   5/31/06 1:33p pblanco
 * PR20017: More changes to support B0 relocatable code.
 * 
 * Hydra_Software_Devel/87   5/26/06 4:37p davidp
 * PR20017: Disable FW mailbox polling for all but 7401 B0
 * 
 * Hydra_Software_Devel/86   5/25/06 3:54p pblanco
 * PR20017: Changed heap reference pointer for B0.
 * 
 * Hydra_Software_Devel/85   5/25/06 3:09p pblanco
 * PR20017: Reloc sanity check in.
 * 
 * Hydra_Software_Devel/84   5/16/06 10:21a pblanco
 * 
 * PR20649: Cleanup for B0. Conditionals must be replaced or removed in
 * later development.
 * 
 * Hydra_Software_Devel/83   5/10/06 11:01a pblanco
 * PR19877: Removed unused host commands identified by Gaurav.
 * 
 * Hydra_Software_Devel/82   5/5/06 3:28p davidp
 * PR21156: Fix 7400 flavor of BXVD_P_ReleasePPB_isr.
 * 
 * Hydra_Software_Devel/81   5/5/06 2:10p davidp
 * PR21156: Add BXVD_P_ReleasePPB_isr and related _isr routines for DM to
 * use at ISR time.
 * 
 * Hydra_Software_Devel/80   5/3/06 9:55a pblanco
 * PR20649: Added polling mode to 7401 SendDecoderCommand in preparation
 * for relocatable FW testing. Default as checked in is set to interrupt
 * mode.
 * 
 * Hydra_Software_Devel/79   4/27/06 4:16p pblanco
 * PR19877: Changed FW busy error return to BXVD_ERR_FW_IS_BUSY instead of
 * the incorrect BERR_INVALID_PARAMETER.
 * 
 * Hydra_Software_Devel/78   4/27/06 9:16a pblanco
 * PR19877: Moved the busy flag clear so that it's reset immediately after
 * a timeout.
 * 
 * Hydra_Software_Devel/77   4/26/06 3:17p pblanco
 * PR19877: Made sure that busy wait flag is cleared before returning
 * through error path.
 * 
 * Hydra_Software_Devel/76   4/17/06 7:58p davidp
 * PR18906: Previous fix lost in merge, version 60 edits were lost
 * 
 * Hydra_Software_Devel/75   4/17/06 4:40p davidp
 * PR20906: Add nowait support for 7400 SendDecoderCommand
 * 
 * Hydra_Software_Devel/74   4/6/06 3:59p pblanco
 * PR19877: Removed code that referenced removed private channel context
 * structure members.
 * 
 * Hydra_Software_Devel/73   4/5/06 2:09p pblanco
 * PR15434: Changed channel open formal parameter from Port to
 * RaveEndianess.
 * 
 * Hydra_Software_Devel/72   3/31/06 4:18p pblanco
 * PR19877: Fix build problems due to latest FW update.
 * 
 * Hydra_Software_Devel/71   3/31/06 3:33p gaurava
 * PR 15434: Removed port from ChannelOpen. Used the same location for
 * telling the decoder that the CDB is in big/little endian format. big-
 * endian is 0 and little_endian is 1.
 * 
 * Hydra_Software_Devel/70   3/28/06 10:26a pblanco
 * PR20274: Added host command calls for BUD and host sparse trick play
 * modes.
 * 
 * Hydra_Software_Devel/69   3/28/06 8:52a pblanco
 * PR19877: Fetch vdec_status_block address at channel open time.
 * 
 * Hydra_Software_Devel/68   3/27/06 6:34p davidp
 * PR20353: Add Decode Still Picture support for 7400
 * 
 * Hydra_Software_Devel/67   3/24/06 1:35p pblanco
 * PR20274: Interim checkin prior to FW implementation.
 * 
 * Hydra_Software_Devel/66   3/22/06 5:16p davidp
 * PR20347: Add L2 interrupt support
 * 
 * Hydra_Software_Devel/65   3/7/06 11:50a pblanco
 * PR19785: Changed host command code for skip picture to reflect enum
 * change. Values are now mapped from XVD to values understood by the
 * firmware.
 * 
 * Hydra_Software_Devel/64   3/6/06 3:24p pblanco
 * PR19785: FW can use the newly specified skip enums directly, no need
 * for the switch statement to map the values.
 * 
 * Hydra_Software_Devel/62   3/6/06 9:27a pblanco
 * PR19785: Implemented host command portion of set skip picture mode.
 * 
 * Hydra_Software_Devel/61   3/6/06 8:47a pblanco
 * PR20017: Check in of Olive's changes after successful weekend
 * regression test.
 * 
 * Hydra_Software_Devel/60   2/28/06 4:36p davidp
 * PR18906: Pass proper field polarity/frame value to VDC for
 * PictureDataRdy interrupt regardless if decode is in progress.:
 * 
 * Hydra_Software_Devel/59   2/21/06 1:46p pblanco
 * PR19785: Implemented BXVD_SetSkipPictureMode API. This also needs FW
 * support.
 * 
 * Hydra_Software_Devel/58   2/17/06 3:17p davidp
 * PR16792: Modify FW API for Still Picture Decode, feature now working:
 * 
 * Hydra_Software_Devel/57   2/2/06 4:04p pblanco
 * PR19123: More code cleanup. Changed all instances of uAvcInstance to
 * uDecoderInstance and removed last remaining references to the old
 * decoder context array.
 * 
 * Hydra_Software_Devel/56   2/2/06 10:59a pblanco
 * PR19123: Switch UART pin mux in send command depending on decoder
 * instance.
 * 
 * Hydra_Software_Devel/55   2/1/06 3:53p pblanco
 * PR19123: Changed "AVC" instance array in XVD context structure to a
 * single instance of type BXVD_DecoderContext. Tested against 7401 with
 * no regressions and against 7400 with no regressions.
 * 
 * Hydra_Software_Devel/54   2/1/06 2:24p pblanco
 * PR19123: Added code to switch pin muxes depending on which decoder
 * instance is active.
 * 
 * Hydra_Software_Devel/53   2/1/06 8:51a pblanco
 * PR19123: More 7400 porting work and bug fixes.
 * 
 * Hydra_Software_Devel/52   1/30/06 4:32p pblanco
 * PR19308: Use correct definitions for local command structures.
 * 
 * Hydra_Software_Devel/51   1/30/06 1:18p pblanco
 * PR19308: Fix command packet being passed to enable trick play.
 *
 * Hydra_Software_Devel/50   1/30/06 9:00a pblanco
 * PR19308: Made sure buffer initializations are correct and that an
 * unsigned long is being passed as the mode value in
 * BXVD_P_HostCmdDecEnableBRCMTrickPlay. Also added debugging messages on
 * input and output.
 * 
 * Hydra_Software_Devel/49   1/27/06 11:04a pblanco
 * PR19123: Conditionalize unistd.h include for 7400. We need to use
 * usleep on this platform for now.
 * 
 * Hydra_Software_Devel/46   1/26/06 1:32p pblanco
 * PR19123: Removed debug printfs and added video memory size to command
 * packet for 7400 FW init.
 * 
 * Hydra_Software_Devel/45   1/25/06 1:42p pblanco
 * PR19123: Fix build error caused by previous check in.
 * 
 * Hydra_Software_Devel/44   1/25/06 11:54a pblanco
 * PR19123: Support for second decoder mailbox address and event.
 * 
 * Hydra_Software_Devel/43   1/24/06 2:51p pblanco
 * PR19123: 7400 firmware bring up debugging changes.
 * 
 * Hydra_Software_Devel/42   1/18/06 2:19p pblanco
 * PR19123: Modifications for 7400. Make sure code builds for this
 * platform.
 * 
 * Hydra_Software_Devel/41   1/16/06 2:11p davidp
 * PR16792: BXVD_Open now uses two heap pointers, one for FW code (2MB)
 * the other for FW picture buffers.:
 * 
 * Hydra_Software_Devel/40   1/10/06 11:18a pblanco
 * PR16052: Added implementation of BXVD_P_HostCmdDecEnableBRCMTrickPlay.
 * 
 * Hydra_Software_Devel/39   12/23/05 6:50p davidp
 * PR16878: Reset Decoder in Watchdog ISR, add BXVD_Open setting for
 * watchdog timer value, pass WD info to FW:
 * 
 * Hydra_Software_Devel/38   12/21/05 5:59p davidp
 * PR16052: Add Still picture support:
 * 
 * Hydra_Software_Devel/37   12/15/05 6:06p davidp
 * PR16052: Support for hold last picture on channel change:
 * 
 * Hydra_Software_Devel/36   12/12/05 3:09p pblanco
 * PR16052: More still picture internals (sanity checkin)
 * 
 * Hydra_Software_Devel/35   12/9/05 3:21p vsilyaev
 * PR 18019: Fixed include files
 * 
 * Hydra_Software_Devel/34   12/9/05 1:31p pblanco
 * PR16052: Fixed a compilation warning.
 * 
 * Hydra_Software_Devel/33   12/9/05 12:29p pblanco
 * PR16052: Added skeleton for still picture host command.
 * 
 * Hydra_Software_Devel/32   11/10/05 8:00a pblanco
 * PR16052: FW now uses heap info to determine load and execution
 * addresses. Also removed unused code that existed when XVD used to
 * create and map its own heap. Corresponding changes were made in
 * related modules also checked in at this time.
 * 
 * Hydra_Software_Devel/32   11/10/05 7:55a pblanco
 * PR16052: FW now uses heap info to determine load and execution
 * addresses. Also removed unused code that existed when XVD used to
 * create and map its own heap. Corresponding changes were made in
 * related modules also checked in at this time.
 * 
 * Hydra_Software_Devel/32   11/10/05 7:53a pblanco
 * PR16052: FW now uses heap info to determine load and execution
 * addresses. Also removed unused code that existed when XVD used to
 * create and map its own heap. Corresponding changes were made in
 * related modules also checked in at this time.
 * 
 * Hydra_Software_Devel/30   11/8/05 1:42p davidp
 * PR17935: Update decoder register definitions to match latest RDB:
 * 
 * Hydra_Software_Devel/29   11/3/05 3:15p pblanco
 * PR16052: Added DM underflow count to status items returned by
 * BXVD_GetChannelStatus.
 * 
 * Hydra_Software_Devel/28   10/25/05 7:58p davidp
 * PR16052: Fix compiler warnings and remove <CR> from report message
 * strings:
 * 
 * Hydra_Software_Devel/27   10/21/05 5:38p davidp
 * PR17706: Set release PPB argument for VDEC channel close command based
 * on channel change mode.:
 * 
 * Hydra_Software_Devel/26   10/20/05 5:30p gaurava
 * PR 16052: Made inner loop firmware relocatable so added the PC address
 * to Init command
 * 
 * Hydra_Software_Devel/25   9/29/05 2:34p pblanco
 * PR16052: Changed inner and outer loop UART from 115200 to 38400.
 * 
 * Hydra_Software_Devel/24   9/26/05 2:27p pblanco
 * PR16052: Passing RUL descriptor mask through to firmware.
 * 
 * Hydra_Software_Devel/22   9/13/05 1:48p pblanco
 * PR16052: Cleaned up decoder flush code.
 * 
 * Hydra_Software_Devel/21   9/7/05 6:01a pblanco
 * PR16052: Fixed bad arguments for BDBG_MSG calls.
 * 
 * Hydra_Software_Devel/20   9/6/05 10:28a pblanco
 * PR16052: Made sure correct addresses were copied from open channel
 * response to channel context.
 * 
 * Hydra_Software_Devel/19   9/2/05 10:05a pblanco
 * PR16052: Fixed compile errors when DEBUG=y.
 * 
 * Hydra_Software_Devel/18   9/2/05 9:11a davidp
 * PR16052: Add support for multiple L3 interrupts:
 * 
 * Hydra_Software_Devel/17   8/29/05 8:38a pblanco
 * PR16052: Support for new FW.
 * 
 * Hydra_Software_Devel/16   8/25/05 5:17p davidp
 * PR16052: Add FW command mailbox interrupt support:
 * 
 * Hydra_Software_Devel/15   8/25/05 1:38p pblanco
 * PR16052: Fixed a potential SEGV that would occur if the response timed
 * out.
 * 
 * Hydra_Software_Devel/14   8/24/05 1:16p pblanco
 * PR16052: Added support for multiple AVC instances and removed debugging
 * printfs.
 * 
 * Hydra_Software_Devel/13   8/23/05 1:46p pblanco
 * PR16052: Additional host command support, code cleanup and bug fixes.
 * 
 * Hydra_Software_Devel/12   8/18/05 2:12p pblanco
 * PR16052: Code fixes for BXVD_OpenChannel and BXVD_StartDecode.
 * 
 * Hydra_Software_Devel/11   8/16/05 1:06p pblanco
 * PR16052: FW download and execution are working... this is a sanity
 * checkin.
 * 
 * Hydra_Software_Devel/10   8/15/05 10:16a pblanco
 * PR16052: Corrected initial command parameters and reformatted some
 * code.
 * 
 * Hydra_Software_Devel/9   8/12/05 7:04a pblanco
 * PR16052: Temporarily commented out some functionality.
 * 
 * Hydra_Software_Devel/8   8/4/05 3:19p ebrakus
 * PR16052: add support for buffer config
 * 
 * Hydra_Software_Devel/7   8/3/05 4:49p ebrakus
 * PR16052: Add support for SetChannelActive and BufferConfig
 * 
 * Hydra_Software_Devel/6   8/3/05 2:13p ebrakus
 * PR16052: Minor cleanup
 * 
 * Hydra_Software_Devel/5   7/29/05 3:03p ebrakus
 * PR16052: add last     remaining commands
 * 
 * Hydra_Software_Devel/4   7/27/05 4:44p ebrakus
 * PR16052: update decoder interface for correct mailbox location
 * 
 * Hydra_Software_Devel/3   7/26/05 1:08p ebrakus
 * PR16052: fix unresolved symbol
 * 
 * Hydra_Software_Devel/2   7/26/05 12:35p ebrakus
 * PR16052: ca hange vdec_api defined values to Mangnum format
 * 
 * Hydra_Software_Devel/1   7/25/05 12:46p ebrakus
 * PR16052: Initial checkin
 * 
 ***************************************************************************/
#include "bstd.h"
#include "bxvd_platform.h"
#include "bxvd_priv.h"		
#include "bkni.h"                /* malloc */
#include "bavc.h"

#include "bxvd_priv.h"
#include "bxvd_reg.h"

#define CPU_2_HOST_INT_CLEAR 0x80000000

/* If FW cmd MBX handshake polling desired, enable polling here */
/* #define BXVD_POLL_FW_MBX 1 */

#define FW_CMD_TIMEOUT_NORMAL    1000
#define FW_CMD_TIMEOUT_DEBUGCMD  1500
#define B0_POLL_DELAY 6000

#if BSTD_CPU_ENDIAN == BSTD_ENDIAN_BIG 
#define BXVD_P_SWAP32(a) (((a&0xFF)<<24)|((a&0xFF00)<<8)|((a&0xFF0000)>>8)|((a&0xFF000000)>>24))

static int _strlen(const char *s) 
{
   int i=0;
   while (*s++) i++;
   return i;
}
#endif

BDBG_MODULE(BXVD_CMDS);

/* TODO: Find the proper values for items tagged with this define*/
#define TBD_VALUE (0)

/******************************************************************************
Summary:
  This function is used to send commands to the AVC decoder.

Description:
  General function for sending commands to AVC decoder.  Used by 
  functions that handle specific commands.

Input:
  hXvd       - XVD instance handle
  uInstance  - Decoder instance. Always 0 for 7401, 7118 0 or 1 for 7400
  *pCmd      - pointer to command structure to send to AVC
  *pRsp      - pointer to buffer to receive response.

Returns:
  BERR_SUCCESS
  BERR_INVALID_PARAMETER - decoder related failure
  
See Also:

******************************************************************************/

BERR_Code BXVD_P_SendDecoderCommand(
   BXVD_Handle hXvd,
   uint32_t       uInstance,
   const BXVD_Cmd *pCmd,
   BXVD_Rsp       *pRsp,
   uint32_t       uiTimeout
   )
{
   BERR_Code         eStatus = BERR_SUCCESS;
   BXVD_P_RspGeneric *pGenericRsp;       /* Used to extract the status field */

   volatile uint32_t uInBox;

#if BXVD_POLL_FW_MBX
   int i;
   volatile uint32_t uIntrVal;
#endif

   BDBG_ENTER(BXVD_P_SendDecoderCommand);

   BDBG_ASSERT(pCmd);
   BDBG_ASSERT(pRsp);

   uInstance = uInstance;

   BKNI_EnterCriticalSection();

   /* Check to if we're already waiting for a reply from this decoder */
   if (hXvd->stDecoderContext.bIfBusy)
   {
      BKNI_LeaveCriticalSection();
      BXVD_DBG_MSG(hXvd, ("Decoder is busy... exiting"));
      return BERR_TRACE(BXVD_ERR_FW_IS_BUSY);
   }

   /* Set device busy */
   hXvd->stDecoderContext.bIfBusy = 1;	
   BKNI_LeaveCriticalSection();

   /* Clear out any previously completed FW command done events */
   BKNI_ResetEvent(hXvd->stDecoderContext.hFWCmdDoneEvent);

   /* write the command parameters to the AVC  */
   BKNI_Memcpy((void*)(hXvd->stDecoderContext.ulCmdBufferAddr),
	       (void*)(pCmd),
	       sizeof(BXVD_Cmd));

   eStatus = BMEM_Heap_FlushCache(hXvd->hCodeHeap, 
                             (void*)(hXvd->stDecoderContext.ulCmdBufferAddr),
                             sizeof(BXVD_Cmd));

   if (eStatus != BERR_SUCCESS) 
   {
      return BERR_TRACE(eStatus);
   }
	
#if BXVD_POLL_FW_MBX
   /* zero out the response mailbox*/
   BXVD_P_WRITE_FWRSP_MBX(hXvd, uInstance, 0);
#endif

   BXVD_P_WRITE_FWCMD_TO_MBX(hXvd, uInstance, hXvd->uiCmdBufferVector);

#if BXVD_POLL_FW_MBX

   /* poll for response */
   uInBox = 0;
   for ( i = 0; i < 200; i++)
   {
      BKNI_Sleep(1);

      uInBox = BXVD_Reg_Read32(hXvd, hXvd->stPlatformInfo.stReg.uiDecode_OuterCPU2HostMailbox); 

      if (uInBox !=0) 
	 break;
   }

   BXVD_DBG_MSG(hXvd, ("Decoder:%d, mbx:%08x", uInstance, uInBox));

   uIntrVal = BXVD_Reg_Read32(hXvd, hXvd->stPlatformInfo.stReg.uiDecode_OuterInterruptClear);

   BXVD_DBG_MSG(hXvd, ("IntGen_clr:%08x\n", uIntrVal));

   /* See if the firmware timed out before the response */
   if ( i >= 200)
   {
      BXVD_DBG_ERR(hXvd, ("BXVD_P_SendDecoderCommand timed out waiting for response"));
   }

#else

   /* Interrupt driven implementation */

   /* wait for response from FW */
   uInBox = 0;

   eStatus = BERR_TRACE(BKNI_WaitForEvent(hXvd->stDecoderContext.hFWCmdDoneEvent, uiTimeout));

   if(BERR_TIMEOUT == eStatus)
   {
      BXVD_DBG_ERR(hXvd, ("FW command: %08x response timed out", pCmd->cmd));
      hXvd->stDecoderContext.bIfBusy = 0;
      return BERR_TRACE(eStatus);
   }

   /* get the response from the AVC  */

   uInBox = BXVD_Reg_Read32(hXvd, hXvd->stPlatformInfo.stReg.uiDecode_OuterCPU2HostMailbox);

#endif

   /* Clear device busy */
   hXvd->stDecoderContext.bIfBusy = 0;	
   
   /* get the response from the AVC  */
   if (uInBox)
   {
      eStatus = BMEM_Heap_FlushCache(hXvd->hCodeHeap, 
                                     (void*)(hXvd->stDecoderContext.ulCmdBufferAddr),
                                     sizeof(BXVD_Rsp));

      if (eStatus != BERR_SUCCESS) 
      {
         return BERR_TRACE(eStatus);
      }

      BKNI_Memcpy((void*)(pRsp),
        (void*)(hXvd->stDecoderContext.ulCmdBufferAddr),
        sizeof(BXVD_Rsp));


     /* make sure it is the correct command */
      if (((pCmd->cmd) | BXVD_CMD_RESPONSE) != (pRsp->cmd)) 
      {
         BDBG_LEAVE(BXVD_P_SendDecoderCommand);
         return BERR_TRACE(BERR_INVALID_PARAMETER);
      }

      pGenericRsp = (BXVD_P_RspGeneric *)(pRsp);

      /* Check for command error */
      if ( pGenericRsp->ulStatus )
      {
         BDBG_ERR(("AVD firmware command: %08x error return %d", pCmd->cmd, pGenericRsp->ulStatus ));
         BDBG_LEAVE(BXVD_P_SendDecoderCommand);
         return BERR_TRACE(BERR_INVALID_PARAMETER);
      }

   }
   else
   {
      BXVD_DBG_ERR(hXvd, ("FW RSP MBX PTR NULL"));
      return BERR_TRACE(BERR_UNKNOWN);
   }
   
   BKNI_ResetEvent(hXvd->stDecoderContext.hFWCmdDoneEvent);
   
   BDBG_LEAVE(BXVD_P_SendDecoderCommand);
   return BERR_TRACE(eStatus);
}

/***************************************************************************
 *  {secret}
 *	BXVD_P_HostCmdSendInit
 *	Command causes the AVC decoder to initialize.
 *  All internal state and any partially completed operations
 *  will be lost. 
 ***************************************************************************/
BERR_Code BXVD_P_HostCmdSendInit
(
 BXVD_Handle  hXvd,                /* XVD context */
 uint32_t     uDecoderInstance,    /* AVC instance */
 uint32_t     eRaveEndianess       /* Endianess of data in Rave CDB/ITB */
 )
{
   BXVD_Cmd             *pCmd;		
   BXVD_Rsp             *pRsp;
   BERR_Code            eStatus = BERR_SUCCESS; /* Status return */

   BXVD_Cmd_Initialize *pInit;		       /* Pointer to command */
   BXVD_Rsp_Initialize *pInitRsp;              /* Pointer to response */

   /* Clear out the context and set defaults. */
   pCmd = &hXvd->FWCmd.cmd;
   pRsp = &hXvd->FWRsp.rsp;

   BKNI_Memset((void*)pCmd, BXVD_P_MEM_ZERO, sizeof(BXVD_Cmd));
   BKNI_Memset((void*)pRsp, BXVD_P_MEM_ZERO, sizeof(BXVD_Rsp));

   pInit = &hXvd->FWCmd.init;
   pInitRsp = &hXvd->FWRsp.init;
  
   /* Command ID */
   pInit->command = BXVD_CMD_INITIALIZE;
  
   /* parameters */
   pInit->cdb_little_endian = eRaveEndianess;
   pInit->stripe_width = hXvd->uiDecode_StripeWidth;
   pInit->stripe_height = hXvd->uiDecode_StripeMultiple;
   pInit->bvnf_intr_context_base = hXvd->stPlatformInfo.stReg.uiBvnf_Intr2_3_AvdStatus;
   pInit->host_L2_intr_set = hXvd->stPlatformInfo.stReg.uiAvd_CPUL2InterruptSet;
   pInit->chip_prod_revision = hXvd->uiChip_ProductRevision;
   pInit->rave_context_reg_size = BXVD_P_RAVE_CONTEXT_SIZE;
   pInit->rave_cx_hold_clr_status = BXVD_P_RAVE_CX_HOLD_CLR_STATUS;
   pInit->rave_packet_count = BXVD_P_RAVE_PACKET_COUNT; 

   BXVD_DBG_MSG(hXvd, ("BXVD_CMD_INITIALIZE:"));
   BXVD_DBG_MSG(hXvd, (" command: %#x", pInit->command));
   BXVD_DBG_MSG(hXvd, (" cdb_little_endian: %d", pInit->cdb_little_endian));
   BXVD_DBG_MSG(hXvd, (" stripe_width: %d", pInit->stripe_width));
   BXVD_DBG_MSG(hXvd, (" stripe_height: %d", pInit->stripe_height));
   BXVD_DBG_MSG(hXvd, (" bvnf_intr_context base: %08x", pInit->bvnf_intr_context_base));
   BXVD_DBG_MSG(hXvd, (" host_L2_intr_set: %08x", pInit->host_L2_intr_set));
   BXVD_DBG_MSG(hXvd, (" chip_prod_revision: %08x", pInit->chip_prod_revision));
   BXVD_DBG_MSG(hXvd, (" rave_context_reg_size: %08x", pInit->rave_context_reg_size));
   BXVD_DBG_MSG(hXvd, (" rave_cx_hold_clr_status: %08x", pInit->rave_cx_hold_clr_status));
   BXVD_DBG_MSG(hXvd, (" rave_packet_count: %08x", pInit->rave_packet_count));

   eStatus = BERR_TRACE(BXVD_P_SendDecoderCommand(hXvd, 
						  uDecoderInstance, 
						  pCmd,
						  pRsp,
                                                  FW_CMD_TIMEOUT_NORMAL));

   BXVD_DBG_MSG(hXvd, ("BXVD_RSP_INITIALIZE:"));
   BXVD_DBG_MSG(hXvd, (" command: %#x", pInitRsp->command));
   BXVD_DBG_MSG(hXvd, (" status: %#x", pInitRsp->status));            
   BXVD_DBG_MSG(hXvd, (" sw_version: %#x", pInitRsp->sw_version));


   BXVD_P_DBG_MSG_DISP_INFO_OFFSET(hXvd, pInitRsp);

   if( eStatus != BERR_SUCCESS )
   {
      return BERR_TRACE(eStatus);
   }
   else if (!BXVD_P_IS_FW_VERSION_VALID(pInitRsp))
   {
      BXVD_DBG_WRN(hXvd, ("Incompatible FW version"));

      eStatus = BERR_INVALID_PARAMETER;
   }

   /* 
    * Save the parameters passed back from the firmware.
    */

   BXVD_P_SAVE_DISPLAY_INFO(hXvd, pInitRsp);

   hXvd->sRevisionInfo.ulDecoderFwRev = pInitRsp->sw_version;

   return BERR_TRACE(eStatus);
}


/***************************************************************************
 *  {secret}
 *	BXVD_P_HostCmdSendInit
 *	Command causes the AVC decoder to initialize.
 *  All internal state and any partially completed operations
 *  will be lost. 
 ***************************************************************************/
BERR_Code BXVD_P_HostCmdSendConfig
(
 BXVD_Handle  hXvd,              /* XVD context */
 uint32_t     uiVecIndex,        /* Display device index */
 uint32_t     uiInterruptMask    /* RUL done mask for specified display */
)
{
   BXVD_Cmd             *pCmd;
   BXVD_Rsp             *pRsp;

   BERR_Code            eStatus = BERR_SUCCESS; /* Status return */

   BXVD_Cmd_Config *pCfgCmd;   /* Pointer to command */
   BXVD_Rsp_Config *pCfgRsp;   /* Pointer to response */

   /* Clear out the context and set defaults. */
   pCmd = &hXvd->FWCmd.cmd;
   pRsp = &hXvd->FWRsp.rsp;

   BKNI_Memset((void*)pCmd, BXVD_P_MEM_ZERO, sizeof(BXVD_Cmd));
   BKNI_Memset((void*)pRsp, BXVD_P_MEM_ZERO, sizeof(BXVD_Rsp));

   pCfgCmd = &hXvd->FWCmd.config;
   pCfgRsp = &hXvd->FWRsp.config;
  
   /* Command ID */
   pCfgCmd->command = BXVD_CMD_CONFIG;
  
   /* parameters */
   pCfgCmd->vec_index = uiVecIndex;
   pCfgCmd->interrupt_mask = uiInterruptMask;

   BXVD_DBG_MSG(hXvd, ("BXVD_CMD_CONFIG:"));
   BXVD_DBG_MSG(hXvd, (" vec_index: %d", pCfgCmd->vec_index));
   BXVD_DBG_MSG(hXvd, (" interrupt_mask: %#x", pCfgCmd->interrupt_mask));

   eStatus = BERR_TRACE(BXVD_P_SendDecoderCommand(hXvd, 
						  hXvd->uDecoderInstance, 
						  pCmd,
						  pRsp,
                                                  FW_CMD_TIMEOUT_NORMAL));

   BXVD_DBG_MSG(hXvd, ("BXVD_RSP_CONFIG:"));
   BXVD_DBG_MSG(hXvd, (" command: %#x", pCfgRsp->command));
   BXVD_DBG_MSG(hXvd, (" status: %#x", pCfgRsp->status));            

   return BERR_TRACE(eStatus);
}

/***************************************************************************
 *  {secret}
 *	BXVD_P_HostCmdSendDecChannelOpen
 ***************************************************************************/
BERR_Code BXVD_P_HostCmdSendDecChannelOpen
(
	BXVD_Handle         hXvd,
	BXVD_ChannelHandle  hXvdCh,
        bool                bStillPictureMode,
        uint32_t            uiMaxResolution,
        BXVD_P_DecodeFWMemSize  *pstDecodeFWMemSize,
        BXVD_P_DecodeFWBaseAddrs *pstDecodeFWBaseAddrs
)
{
   BXVD_Cmd             *pCmd;
   BXVD_Rsp             *pRsp;
   
   BERR_Code        eStatus = BERR_SUCCESS;/* Status return       */
   BXVD_Cmd_ChannelOpen	*pChOpen;	/* Pointer to command  */
   BXVD_Rsp_ChannelOpen	*pChOpenRsp;	/* Pointer to response */

   uint32_t            uiStatusBlkAddr;

   /* Clear out the context and set defaults. */
   pCmd = &hXvd->FWCmd.cmd;
   pRsp = &hXvd->FWRsp.rsp;
   BKNI_Memset((void*)pCmd, BXVD_P_MEM_ZERO, sizeof(BXVD_Cmd));
   BKNI_Memset((void*)pRsp, BXVD_P_MEM_ZERO, sizeof(BXVD_Rsp));

   pChOpen = &hXvd->FWCmd.channelOpen;
   pChOpenRsp = &hXvd->FWRsp.channelOpen;

   /* Command ID */
   pChOpen->command = BXVD_CMD_CHANNELOPEN;
   pChOpen->channel_number = hXvdCh->ulChannelNum;

   pChOpen->still_picture_mode = (unsigned long) bStillPictureMode;

   pChOpen->max_resolution_enum = uiMaxResolution;

   pChOpen->context_memory_base = pstDecodeFWBaseAddrs->uiFWContextBase; 
   pChOpen->context_memory_size = pstDecodeFWMemSize->uiFWContextSize; 

   pChOpen->video_memory_base = pstDecodeFWBaseAddrs->uiFWPicBlockBase;
   pChOpen->video_block_size = pstDecodeFWMemSize->uiFWPicBlockSize;
   pChOpen->video_block_count = pstDecodeFWMemSize->uiFWPicBlockCount;

   pChOpen->cabac_memory_base = pstDecodeFWBaseAddrs->uiFWCabacBase; 
   pChOpen->cabac_memory_size = pstDecodeFWMemSize->uiFWCabacSize; 

   pChOpen->cabac_wl_base = pstDecodeFWBaseAddrs->uiFWCabacWorklistBase; 
   pChOpen->cabac_wl_size = pstDecodeFWMemSize->uiFWCabacWorklistSize; 

   pChOpen->direct_mode_storage_base = pstDecodeFWBaseAddrs->uiFWDirectModeBase;
   pChOpen->direct_mode_storage_size = pstDecodeFWMemSize->uiFWDirectModeSize;

   pChOpen->il_wl_base = pstDecodeFWBaseAddrs->uiFWInnerLoopWorklistBase;
   pChOpen->il_wl_size = pstDecodeFWMemSize->uiFWInnerLoopWorklistSize; 
   
   pChOpen->bl_mv_store_base = pstDecodeFWBaseAddrs->uiFWInterLayerMVBase;
   pChOpen->bl_mv_store_size = pstDecodeFWMemSize->uiFWInterLayerMVSize;

   pChOpen->bl_video_store_base = pstDecodeFWBaseAddrs->uiFWInterLayerPicBase;
   pChOpen->bl_video_store_size = pstDecodeFWMemSize->uiFWInterLayerPicSize;

   /* parameters */

   BXVD_DBG_MSG(hXvd, ("BXVD_CMD_CHANNELOPEN:"));
   BXVD_DBG_MSG(hXvd, (" command: %#x", pChOpen->command));
   BXVD_DBG_MSG(hXvd, (" channel_number: %d", pChOpen->channel_number));
   BXVD_DBG_MSG(hXvd, (" max_resolution_enum: %d", pChOpen->max_resolution_enum));
   BXVD_DBG_MSG(hXvd, (" still_picture_mode: %d", pChOpen->still_picture_mode));
   BXVD_DBG_MSG(hXvd, (" context_memory_base: %#x", pChOpen->context_memory_base));
   BXVD_DBG_MSG(hXvd, (" context_memory_size: %#x", pChOpen->context_memory_size));
   BXVD_DBG_MSG(hXvd, (" video_memory_base: %#x", pChOpen->video_memory_base));
   BXVD_DBG_MSG(hXvd, (" video_block_size: %#x", pChOpen->video_block_size));
   BXVD_DBG_MSG(hXvd, (" video_block_count: %d", pChOpen->video_block_count));
   BXVD_DBG_MSG(hXvd, (" cabac_memory_base: %#x", pChOpen->cabac_memory_base));
   BXVD_DBG_MSG(hXvd, (" cabac_memory_size: %#x", pChOpen->cabac_memory_size));
   BXVD_DBG_MSG(hXvd, (" bl_mv_store_base: %#x", pChOpen->bl_mv_store_base));
   BXVD_DBG_MSG(hXvd, (" bl_mv_store_size: %#x", pChOpen->bl_mv_store_size));
   BXVD_DBG_MSG(hXvd, (" cabac_worklist_base: %#x", pChOpen->cabac_wl_base));
   BXVD_DBG_MSG(hXvd, (" cabac_worklist_size: %#x", pChOpen->cabac_wl_size));
   BXVD_DBG_MSG(hXvd, (" direct_mode_storage_base: %#x", pChOpen->direct_mode_storage_base));
   BXVD_DBG_MSG(hXvd, (" direct_mode_storage_size: %#x", pChOpen->direct_mode_storage_size));
   BXVD_DBG_MSG(hXvd, (" il_wl_base: %#x", pChOpen->il_wl_base));
   BXVD_DBG_MSG(hXvd, (" il_wl_size: %#x", pChOpen->il_wl_size));
   BXVD_DBG_MSG(hXvd, (" bl_video_store_base: %#x", pChOpen->bl_video_store_base));
   BXVD_DBG_MSG(hXvd, (" bl_video_store_size: %#x", pChOpen->bl_video_store_size));

   eStatus = BERR_TRACE(BXVD_P_SendDecoderCommand(hXvd,
                                                  hXvd->uDecoderInstance, 
                                                  pCmd,
                                                  pRsp,
                                                  FW_CMD_TIMEOUT_NORMAL));

   BXVD_DBG_MSG(hXvd, ("BXVD_RSP_CHANNELOPEN:"));
   BXVD_DBG_MSG(hXvd, (" command: %#x", pChOpenRsp->command));
   BXVD_DBG_MSG(hXvd, (" status: %#x", pChOpenRsp->status));
   BXVD_DBG_MSG(hXvd, (" picture_delivery_buffer: %#x", pChOpenRsp->picture_delivery_buffer));
   BXVD_DBG_MSG(hXvd, (" picture_release_buffer: %#x", pChOpenRsp->picture_release_buffer));

   /* Save the return info of interest. */

#if BXVD_P_FW_HIM_API

   /* Initialize the delivery queue parameters.
    */
   BKNI_Memset( &(hXvdCh->stDeliveryQueue), 0, sizeof ( BXVD_P_AVD_Queue ) );

   hXvdCh->stDeliveryQueue.ulQueueOffset              = pChOpenRsp->picture_delivery_buffer;

   hXvdCh->stDeliveryQueue.stReadIndex.ulByteOffset   = pChOpenRsp->delivery_q_read_byte_offset;
   BXVD_P_HOSTINTERFACEMEMORY_COOK_OFFSETS( hXvdCh->stDeliveryQueue.stReadIndex, 1 );

   hXvdCh->stDeliveryQueue.stWriteIndex.ulByteOffset  = pChOpenRsp->delivery_q_write_byte_offset;
   BXVD_P_HOSTINTERFACEMEMORY_COOK_OFFSETS( hXvdCh->stDeliveryQueue.stWriteIndex, 1 );

   /* Initialize the release queue parameters.
    */
   BKNI_Memset( &(hXvdCh->stReleaseQueue), 0, sizeof ( BXVD_P_AVD_Queue ) );

   hXvdCh->stReleaseQueue.ulQueueOffset               = pChOpenRsp->picture_release_buffer;

   hXvdCh->stReleaseQueue.stReadIndex.ulByteOffset    = pChOpenRsp->release_q_read_byte_offset;
   BXVD_P_HOSTINTERFACEMEMORY_COOK_OFFSETS( hXvdCh->stReleaseQueue.stReadIndex, 1 );
   
   hXvdCh->stReleaseQueue.stWriteIndex.ulByteOffset   = pChOpenRsp->release_q_write_byte_offset;
   BXVD_P_HOSTINTERFACEMEMORY_COOK_OFFSETS( hXvdCh->stReleaseQueue.stWriteIndex, 1 );

   hXvdCh->stReleaseQueue.stShadowWriteIndex.ulByteOffset   = pChOpenRsp->shadow_write_byte_offset;
   BXVD_P_HOSTINTERFACEMEMORY_COOK_OFFSETS( hXvdCh->stReleaseQueue.stShadowWriteIndex, 1 );

   /* Initialize the drop count parameters.
    */
   BKNI_Memset( &(hXvdCh->stDropCountIndex), 0, sizeof ( BXVD_P_HIM_Offsets ) );

   hXvdCh->stDropCountIndex.ulByteOffset        = pChOpenRsp->drop_count_byte_offset;
   BXVD_P_HOSTINTERFACEMEMORY_COOK_OFFSETS( hXvdCh->stDropCountIndex, 2 );

   /* Initialize the AVD status block parameters.
    */
   BKNI_Memset( &(hXvdCh->stStatusBlockIndex), 0, sizeof ( BXVD_P_HIM_Offsets ) );
   
   hXvdCh->stStatusBlockIndex.ulByteOffset      = pChOpenRsp->avd_status_addr;
   BXVD_P_HOSTINTERFACEMEMORY_COOK_OFFSETS( hXvdCh->stStatusBlockIndex, 4 );

   /* unused */
   uiStatusBlkAddr = 0 ;
   hXvdCh->ulAvdStatusBlock = 0;

#else
   BXVD_DBG_MSG(hXvd, (" dms_release_buffer: %#x", pChOpenRsp->dm_return_address));
   BXVD_DBG_MSG(hXvd, (" avd_status_address = %#x", pChOpenRsp->avd_status_address));
        
   hXvdCh->ulPicBuf           = pChOpenRsp->picture_delivery_buffer;
   hXvdCh->ulPicRelBuf        = pChOpenRsp->picture_release_buffer;
   hXvdCh->ulPicInfoRelBuf    = pChOpenRsp->dm_return_address; 
 
   uiStatusBlkAddr            = (uint32_t) pChOpenRsp->avd_status_address;

   BMEM_ConvertOffsetToAddress(hXvd->hSystemHeap, 
                               uiStatusBlkAddr,
                               (void *)&hXvdCh->ulAvdStatusBlock);
#endif
        

   hXvdCh->bDecoderChannelOpened = true;
       	
   return BERR_TRACE(eStatus);
}

/***************************************************************************
 *  {secret}
 *	BXVD_P_HostCmdSendDecChannelClose
 ***************************************************************************/
BERR_Code BXVD_P_HostCmdSendDecChannelClose
(
   BXVD_Handle         hXvd,
   BXVD_ChannelHandle  hXvdCh
)
{
   BXVD_Cmd             *pCmd;
   BXVD_Rsp             *pRsp;

   BERR_Code        eStatus  = BERR_SUCCESS; /* Status return */

   BXVD_Cmd_ChannelClose *pChClose;	     /* Pointer to command  */
   BXVD_Rsp_ChannelClose *pChCloseRsp;      /* Pointer to response */

   /* Clear out the context and set defaults. */
   pCmd = &hXvd->FWCmd.cmd;
   pRsp = &hXvd->FWRsp.rsp;
   BKNI_Memset((void*)pCmd, BXVD_P_MEM_ZERO, sizeof(BXVD_Cmd));
   BKNI_Memset((void*)pRsp, BXVD_P_MEM_ZERO, sizeof(BXVD_Rsp));

   pChClose = &hXvd->FWCmd.channelClose;
   pChCloseRsp = &hXvd->FWRsp.channelClose;

   /* Command ID */
   pChClose->command = BXVD_CMD_CHANNELCLOSE;
   pChClose->channel_number = hXvdCh->ulChannelNum;
	
   BXVD_DBG_MSG(hXvd, ("BXVD_CMD_CHANNELCLOSE:"));
   BXVD_DBG_MSG(hXvd, (" command: %#x", pChClose->command));
   BXVD_DBG_MSG(hXvd, (" channel_number: %d", pChClose->channel_number));

   eStatus = BERR_TRACE(BXVD_P_SendDecoderCommand(hXvd, 
						  hXvd->uDecoderInstance, 
						  pCmd,
						  pRsp,
                                                  FW_CMD_TIMEOUT_NORMAL));
   
   BXVD_DBG_MSG(hXvd, ("BXVD_RSP_CHANNELCLOSE:"));
   BXVD_DBG_MSG(hXvd, (" command: %#x", pChCloseRsp->command));
   BXVD_DBG_MSG(hXvd, (" status: %#x", pChCloseRsp->status));            
   
   hXvdCh->bDecoderChannelOpened = false;

   return BERR_TRACE(eStatus);
}

/***************************************************************************
 *  {secret}
 *	BXVD_P_HostCmdSendDecChannelOpen
 ***************************************************************************/
BERR_Code BXVD_P_HostCmdSendDecChannelStart
(
   BXVD_Handle         hXvd,
   uint32_t            ulChannelNumber,
   uint32_t            eProtocol,
   uint32_t            eChannelMode,
   uint32_t            ulRaveContextBase,
   uint32_t            aulRaveContextBaseExt[],
   uint32_t            ulVecIndex
   )
{
   BXVD_Cmd             *pCmd;
   BXVD_Rsp             *pRsp;

   BERR_Code        eStatus = BERR_SUCCESS;/* Status return       */
   BXVD_Cmd_ChannelStart	*pChStart;	/* Pointer to command  */
   BXVD_Rsp_ChannelStart	*pChStartRsp;	/* Pointer to response */

   /* Clear out the context and set defaults. */
   pCmd = &hXvd->FWCmd.cmd;
   pRsp = &hXvd->FWRsp.rsp;
   BKNI_Memset((void*)pCmd, 0x0, sizeof(BXVD_Cmd));
   BKNI_Memset((void*)pRsp, 0x0, sizeof(BXVD_Rsp));

   pChStart = &hXvd->FWCmd.channelStart;
   pChStartRsp = &hXvd->FWRsp.channelStart;

   /* Command ID */
   pChStart->command = BXVD_CMD_CHANNELSTART;
	
   /* parameters */
   pChStart->protocol = eProtocol;	

   pChStart->channel_mode = eChannelMode;
   pChStart->channel_number = ulChannelNumber;;

   pChStart->rave_ctxt_base = ulRaveContextBase;
   pChStart->rave_ctxt_base_ext = aulRaveContextBaseExt[0];
   pChStart->vec_index = ulVecIndex;

   BXVD_DBG_MSG(hXvd, ("BXVD_CMD_CHANNELSTART:"));
   BXVD_DBG_MSG(hXvd, (" command: %#x", pChStart->command));
   BXVD_DBG_MSG(hXvd, (" protocol: %d", pChStart->protocol));
   BXVD_DBG_MSG(hXvd, (" channel_mode: %#x", pChStart->channel_mode));
   BXVD_DBG_MSG(hXvd, (" vec_index: %d", pChStart->vec_index));
   BXVD_DBG_MSG(hXvd, (" channel_number: %d", pChStart->channel_number));
   BXVD_DBG_MSG(hXvd, (" rave_ctxt_base: %#x", pChStart->rave_ctxt_base));
   BXVD_DBG_MSG(hXvd, (" rave_ctxt_base_ext: %#x", pChStart->rave_ctxt_base_ext));

   eStatus = BERR_TRACE(BXVD_P_SendDecoderCommand(hXvd,
                                                  hXvd->uDecoderInstance, 
                                                  pCmd,
                                                  pRsp,
                                                  FW_CMD_TIMEOUT_NORMAL));

   BXVD_DBG_MSG(hXvd, ("BXVD_RSP_CHANNELSTART:"));
   BXVD_DBG_MSG(hXvd, (" command: %#x", pChStartRsp->command));
   BXVD_DBG_MSG(hXvd, (" status: %#x", pChStartRsp->status));            
   BXVD_DBG_MSG(hXvd, (" frames_outstanding: %d", pChStartRsp->frames_outstanding));

   return BERR_TRACE(eStatus);
}



/***************************************************************************
 *  {secret}
 *	BXVD_P_HostCmdSendDecChannelStop
 ***************************************************************************/
BERR_Code BXVD_P_HostCmdSendDecChannelStop
(
	BXVD_Handle        hXvd,
	uint32_t           ulChannelNum
)
{
   BXVD_Cmd             *pCmd;
   BXVD_Rsp             *pRsp;

   BERR_Code        eStatus  = BERR_SUCCESS; /* Status return */

   BXVD_Cmd_ChannelStop *pChStop;	     /* Pointer to command  */
   BXVD_Rsp_ChannelStop *pChStopRsp;      /* Pointer to response */

   BSTD_UNUSED(ulChannelNum);

   /* Clear out the context and set defaults. */
   pCmd = &hXvd->FWCmd.cmd;
   pRsp = &hXvd->FWRsp.rsp;
   BKNI_Memset((void*)pCmd, BXVD_P_MEM_ZERO, sizeof(BXVD_Cmd));
   BKNI_Memset((void*)pRsp, BXVD_P_MEM_ZERO, sizeof(BXVD_Rsp));

   pChStop = &hXvd->FWCmd.channelStop;
   pChStopRsp = &hXvd->FWRsp.channelStop;

   /* Command ID */
   pChStop->command = BXVD_CMD_CHANNELSTOP;
   pChStop->channel_number = ulChannelNum;

   BXVD_DBG_MSG(hXvd, ("BXVD_CMD_CHANNELSTOP:"));
   BXVD_DBG_MSG(hXvd, (" command: %#x", pChStop->command));
   BXVD_DBG_MSG(hXvd, (" channel_number: %d", pChStop->channel_number));

   eStatus = BERR_TRACE(BXVD_P_SendDecoderCommand(hXvd, 
						  hXvd->uDecoderInstance,
						  pCmd,
						  pRsp,
                                                  FW_CMD_TIMEOUT_NORMAL));

   BXVD_DBG_MSG(hXvd, ("BXVD_RSP_CHANNELSTOP:"));
   BXVD_DBG_MSG(hXvd, (" command: %#x", pChStopRsp->command));
   BXVD_DBG_MSG(hXvd, (" status: %#x", pChStopRsp->status));            
   
   return BERR_TRACE(eStatus);
}

/***************************************************************************
 *	{secret}
 *	BXVD_P_HostCmdSetSkipPictureMode
 ***************************************************************************/
BERR_Code BXVD_P_HostCmdSetSkipPictureMode
(
   BXVD_Handle    hXvd,
   uint32_t       ulChannelNum,
   BXVD_SkipMode  eSkipMode
)
{
   BERR_Code        eStatus = BERR_SUCCESS;
   BXVD_Cmd             *pCmd;
   BXVD_Rsp             *pRsp;

   BXVD_Cmd_ChannelSpeed *pChanSpeed;
   BXVD_Rsp_ChannelSpeed *pChanSpeedRsp;

   /* Clear out the context and set defaults. */
   pCmd = &hXvd->FWCmd.cmd;
   pRsp = &hXvd->FWRsp.rsp;
   BKNI_Memset((void*)pCmd, BXVD_P_MEM_ZERO, sizeof(BXVD_Cmd));
   BKNI_Memset((void*)pRsp, BXVD_P_MEM_ZERO, sizeof(BXVD_Rsp));

   pChanSpeed = &hXvd->FWCmd.channelSpeed;
   pChanSpeedRsp = &hXvd->FWRsp.channelSpeed;

   pChanSpeed->command = BXVD_CMD_CHANNELSPEED;
   pChanSpeed->channel_number = ulChannelNum;

   /*
    * Map XVD skip modes to values used by the firmware.
    */

   switch(eSkipMode)
   {
      case BXVD_SkipMode_eDecode_I_Only:
	 pChanSpeed->speed = VDEC_SPEED_IONLY;   
	 break;
      case BXVD_SkipMode_eDecode_IP_Only:
	 pChanSpeed->speed = VDEC_SPEED_IPONLY;  
	 break;
      case BXVD_SkipMode_eDecode_IPB:
	 pChanSpeed->speed = VDEC_SPEED_NORMAL;  
	 break;
      case BXVD_SkipMode_eDecode_Ref_Only:
	 pChanSpeed->speed = VDEC_SPEED_REFONLY; 
	 break;
      default:
	 return BERR_TRACE(BERR_INVALID_PARAMETER);
   };

   BXVD_DBG_MSG(hXvd, ("BXVD_CMD_CHANNELSPEED:"));
   BXVD_DBG_MSG(hXvd, (" command: %#x", pChanSpeed->command));
   BXVD_DBG_MSG(hXvd, (" channel_number: %d", pChanSpeed->channel_number));
   BXVD_DBG_MSG(hXvd, (" speed: %d", pChanSpeed->speed));

   eStatus = BERR_TRACE(BXVD_P_SendDecoderCommand(hXvd,
						  hXvd->uDecoderInstance,
						  pCmd,
						  pRsp,
                                                  FW_CMD_TIMEOUT_NORMAL));

   BXVD_DBG_MSG(hXvd, ("BXVD_RSP_CHANNELSPEED:"));
   BXVD_DBG_MSG(hXvd, (" command: %#x", pChanSpeedRsp->command));
   BXVD_DBG_MSG(hXvd, (" status: %#x", pChanSpeedRsp->status));            

   return BERR_TRACE(eStatus);
}


/***************************************************************************
 *  Summary 
 *
 *     BXVD_P_HostCmdDramLogControl: Send debug log control command
 *
 *  Description
 *
 *     The decoders outer loop debug output can be loggef to memory for XVD 
 *     to read and pass to the application. This routines starts and stops
 *     the logging of the debug data to the log buffer.
 *
 ***************************************************************************/
BERR_Code BXVD_P_HostCmdDbgLogControl
(
   BXVD_Handle    hXvd,
   bool           logStart
)
{
   BERR_Code        eStatus = BERR_SUCCESS;
   BXVD_Cmd             *pCmd;
   BXVD_Rsp             *pRsp;

   BXVD_Cmd_DbgLogControl *pDbgLog;
   BXVD_Rsp_DbgLogControl *pDbgLogRsp;

   /* Clear out the context and set defaults. */
   pCmd = &hXvd->FWCmd.cmd;
   pRsp = &hXvd->FWRsp.rsp;
   BKNI_Memset((void*)pCmd, BXVD_P_MEM_ZERO, sizeof(BXVD_Cmd));
   BKNI_Memset((void*)pRsp, BXVD_P_MEM_ZERO, sizeof(BXVD_Rsp));

   pDbgLog = &hXvd->FWCmd.dbgLogControl;
   pDbgLogRsp = &hXvd->FWRsp.dbgLogControl;

   pDbgLog->command = BXVD_CMD_DBGLOGCONTROL;
   pDbgLog->logStart = (uint32_t) logStart;

   pDbgLog->dbglog_memory_base = hXvd->uiDecoderDbgBufPhyAddr;
   pDbgLog->dbglog_memory_size = hXvd->stSettings.uiDecoderDebugLogBufferSize;


   BXVD_DBG_MSG(hXvd, ("BXVD_CMD_DBGLogControl:"));
   BXVD_DBG_MSG(hXvd, (" command: %#x", pDbgLog->command));
   BXVD_DBG_MSG(hXvd, (" logStart: %d", pDbgLog->logStart));
   BXVD_DBG_MSG(hXvd, (" dbglog_mem_base: %08x", pDbgLog->dbglog_memory_base));
   BXVD_DBG_MSG(hXvd, (" dbglog_mem_size: %08x", pDbgLog->dbglog_memory_size));

   eStatus = BERR_TRACE(BXVD_P_SendDecoderCommand(hXvd,
						  hXvd->uDecoderInstance,
						  pCmd,
						  pRsp,
                                                  FW_CMD_TIMEOUT_NORMAL));

   BXVD_DBG_MSG(hXvd, ("BXVD_RSP_DbgLogControl"));
   BXVD_DBG_MSG(hXvd, (" command: %#x", pDbgLogRsp->command));
   BXVD_DBG_MSG(hXvd, (" status: %#x", pDbgLogRsp->status));            

   return BERR_TRACE(eStatus);
}


/***************************************************************************
 *  Summary 
 *
 *     BXVD_P_HostCmdDbgLogCommand: Send debug command
 *
 *  Description
 *
 *     The decoders outer loop will process debug command, the output for
 *     the command is written to the debug log buffer. This routine sends
 *     the outer loop the debug command string. 
 *
 ***************************************************************************/
BERR_Code BXVD_P_HostCmdDbgLogCommand
(
   BXVD_Handle    hXvd,
   char           *pCommand
)
{
   BERR_Code        eStatus = BERR_SUCCESS;
   BXVD_Cmd             *pCmd;
   BXVD_Rsp             *pRsp;

   BXVD_Cmd_DbgLogCommand *pDbgCmd;
   BXVD_Rsp_DbgLogCommand *pDbgCmdRsp;

#if BSTD_CPU_ENDIAN == BSTD_ENDIAN_BIG
   int32_t  len, i;
   uint32_t *pTmpCmd, *pTmpLogCmd;
#endif

   /* Clear out the context and set defaults. */
   pCmd = &hXvd->FWCmd.cmd;
   pRsp = &hXvd->FWRsp.rsp;
   BKNI_Memset((void*)pCmd, BXVD_P_MEM_ZERO, sizeof(BXVD_Cmd));
   BKNI_Memset((void*)pRsp, BXVD_P_MEM_ZERO, sizeof(BXVD_Rsp));

   pDbgCmd = &hXvd->FWCmd.dbgLogCommand;
   pDbgCmdRsp = &hXvd->FWRsp.dbgLogCommand;

   pDbgCmd->command = BXVD_CMD_DBGLOGCOMMAND;

#if BSTD_CPU_ENDIAN == BSTD_ENDIAN_LITTLE
   BKNI_Memcpy(&pDbgCmd->logCmd[0], pCommand, sizeof(pDbgCmd->logCmd));
#else
   len = _strlen(pCommand)/4;

   pTmpCmd = (uint32_t *)pCommand;
   pTmpLogCmd = (uint32_t *)&pDbgCmd->logCmd[0];

   for ( i = 0 ; i <= len ; i++ ) 
   {
      *pTmpLogCmd = BXVD_P_SWAP32(*pTmpCmd);
      pTmpCmd++;
      pTmpLogCmd++;
   }
#endif

   BXVD_DBG_MSG(hXvd, ("BXVD_CMD_Dbg_LOG_CMD:"));
   BXVD_DBG_MSG(hXvd, (" command: %#x", pDbgCmd->command));
   BXVD_DBG_MSG(hXvd, (" logCmd: %s", &pDbgCmd->logCmd[0]));

   eStatus = BERR_TRACE(BXVD_P_SendDecoderCommand(hXvd,
						  hXvd->uDecoderInstance,
						  pCmd,
						  pRsp,
                                                  FW_CMD_TIMEOUT_DEBUGCMD));

   BXVD_DBG_MSG(hXvd, ("BXVD_RSP_DbgLogCommand"));
   BXVD_DBG_MSG(hXvd, (" command: %#x", pDbgCmdRsp->command));
   BXVD_DBG_MSG(hXvd, (" status: %#x", pDbgCmdRsp->status));            

   return BERR_TRACE(eStatus);
}

/* End of File */
