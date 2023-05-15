/***************************************************************************
 *     Copyright (c) 2004-2006, Broad-com Corporation
 *     All Rights Reserved
 *     Confidential Property of Broad-com Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROAD-COM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxvd_priv.c $
 * $brcm_Revision: Hydra_Software_Devel/222 $
 * $brcm_Date: 10/8/12 4:57p $
 *
 * Module Description:
 *   See Module Overview below.
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/xvd/src/bxvd_priv.c $
 * 
 * Hydra_Software_Devel/222   10/8/12 4:57p davidp
 * SW7445-16: Add support for HEVC (H.265) Video decoding.
 * 
 * Hydra_Software_Devel/221   8/24/12 5:51p davidp
 * SW7425-3618: Replace uncached memory accesses with cached memory
 * accesses.
 * 
 * Hydra_Software_Devel/220   7/27/12 11:30a btosi
 * SW7425-1064: added support for BXVD_Decoder_OpenChannel
 * 
 * Hydra_Software_Devel/219   7/9/12 4:36p davidp
 * SW7425-3384: Add StripeWidth check to fix coverity issue.
 * 
 * Hydra_Software_Devel/218   6/18/12 5:34p davidp
 * SW7435-164: Remove unneeded include file.
 * 
 * Hydra_Software_Devel/217   6/14/12 4:02p btosi
 * SW7425-3177: changed when the BAVC_VideoCompressionStd enum is mapped
 * to a BXVD_P_PPB_Protocol value
 * 
 * Hydra_Software_Devel/216   6/13/12 11:15a btosi
 * SW7425-3177: added support frame rates e10,e15,e20.  Reintroduced
 * BXVD_P_PPB_Protocol since the AVD and bavc.h protocol enums have
 * diverged.
 * 
 * Hydra_Software_Devel/215   5/30/12 5:18p davidp
 * SW7425-3028: Add VP8 decode still picture support.
 * 
 * Hydra_Software_Devel/214   4/19/12 5:22p davidp
 * SW7346-761: Add support routines to manage channel context free list.
 * 
 * Hydra_Software_Devel/213   3/26/12 1:55p btosi
 * SW7425-2536: removed
 * BXDM_PictureProvider_GetDefaultDecoderInterface_isr
 * 
 * Hydra_Software_Devel/212   3/16/12 11:58a btosi
 * SW7425-2536: added support for displayInterruptStatus_isr
 * 
 * Hydra_Software_Devel/211   2/16/12 6:12p davidp
 * SW7425-2419: Add BXVD_P_DestroyInterrupts().
 * 
 * Hydra_Software_Devel/210   2/9/12 7:57p davidp
 * SW7425-2284: Add BXVD_P_InitChannel to be used in BXVD_OpenChannel.
 * 
 * Hydra_Software_Devel/209   2/2/12 3:02p davidp
 * SW7425-2024: Validate decode protocol specified in BXVD_StartDecode is
 * supported on platform.
 * 
 * Hydra_Software_Devel/208   1/20/12 3:47p davidp
 * SW7208-175: Add support for 1920 portrait decode.
 * 
 * Hydra_Software_Devel/207   12/14/11 5:05p davidp
 * FWAVDTEST-31: Update AVS memory config table.
 * 
 * Hydra_Software_Devel/206   11/10/11 2:37p davidp
 * SW7425-1599: Add bSVCProgressiveOnly flag to BXVD_ChannelSettings.
 * 
 * Hydra_Software_Devel/205   10/20/11 3:21p davidp
 * SW7425-1564: Add support for SVC interlaced decode.
 * 
 * Hydra_Software_Devel/204   9/27/11 6:40p davidp
 * SW7340-279: Allocate Still Channel context in BXVD_Open.
 * 
 * Hydra_Software_Devel/203   9/20/11 3:56p davidp
 * SW7346-420: Add memory config support for VP8, RV9 and Spark protocols.
 * 
 * Hydra_Software_Devel/202   7/20/11 3:27p davidp
 * SW7231-127: Add BXVD_Standby and BXVD_Resume support.
 * 
 * Hydra_Software_Devel/201   6/30/11 4:06p davidp
 * SW7425-826: Clear vsync interrupt when vsync interrupt mask is
 * registered with XVD.
 * 
 * Hydra_Software_Devel/200   6/27/11 4:13p davidp
 * SW7425-542: Merge devel branch to mainline.
 * 
 * Hydra_Software_Devel/SW7425-542/1   6/9/11 4:15p davidp
 * SW7425-542: Increase context memory size for extended PPB.
 * 
 * Hydra_Software_Devel/199   4/20/11 4:25p davidp
 * SW7231-127: Disable BXVD_P_GetHibernateState on platforms that use the
 * BCHP_PWR routines.
 * 
 * Hydra_Software_Devel/198   3/25/11 3:56p davidp
 * SW7405-5101: Add checks to verify AVD code space, CDB/ITB and memory
 * used is in memory region below 768mb for ARC300 based cores.
 * 
 * Hydra_Software_Devel/197   2/16/11 4:09p davidp
 * SW7422-22: Increase FW context memory for Rev K core support.
 * 
 * Hydra_Software_Devel/196   12/9/10 5:13p davidp
 * SW7422-22: Fix SVC support, allocate FW memory for AVD.
 * 
 * Hydra_Software_Devel/195   11/30/10 5:59p davidp
 * SW7422-22: Free FW interlayer picture buffers in BXVD_P_FreeFWMem.
 * 
 * Hydra_Software_Devel/194   11/23/10 6:12p davidp
 * SW7422-22: Fix SVC support.
 * 
 * Hydra_Software_Devel/193   10/18/10 5:39p davidp
 * SW7425-16: Fix compiler warning.
 * 
 * Hydra_Software_Devel/192   10/18/10 4:54p davidp
 * SW7425-16: Fix compiler warning.
 * 
 * Hydra_Software_Devel/191   10/15/10 6:35p davidp
 * SW7425-16: Add support for base layer ARC, new memory config,
 * additional FW api memory arguments.
 * 
 * Hydra_Software_Devel/190   10/6/10 2:00p davidp
 * SW7405-4923: Set XVD handle type to invalid when context is freed.
 * 
 * Hydra_Software_Devel/189   10/5/10 10:24a davidp
 * SW7405-4923: Resolve latest coverity run issues for non-debug builds.
 * 
 * Hydra_Software_Devel/188   9/9/10 5:27p davidp
 * SW7420-972: Only include bchp_pwr.h on platforms that support bchp_pwr.
 * 
 * Hydra_Software_Devel/187   9/9/10 2:59p davidp
 * SW7420-972: Use BCHP_PWR routines for dynamic power management in XVD.
 * 
 * Hydra_Software_Devel/186   7/30/10 2:10p pblanco
 * SW7400-2857: Backed out memory reduction changes.
 * 
 * Hydra_Software_Devel/184   4/1/10 3:13p davidp
 * SW7405-4160: Output DBG warning message if power management is not
 * supported on the platform when code is built to use dymanic PM.
 * 
 * Hydra_Software_Devel/183   3/8/10 5:46p davidp
 * SW7405-3849: Increase CABAC memory allocated for AVC and MPEG2 stills.
 * 
 * Hydra_Software_Devel/182   1/19/10 4:45p davidp
 * SW7405-3667: Clear decoder context bIfBusy flag on FW command timeout
 * and when resetting the decoder.
 * 
 * Hydra_Software_Devel/181   1/6/10 2:28p btosi
 * SW7405-3137: merged branch into mainline
 * 
 * Hydra_Software_Devel/180   11/30/09 4:41p btosi
 * SW7405-3245: added BXVD_DBG_* macros.  Map to either BDBG_INSTANCE_* or
 * BDBG_* at compile.
 * 
 * Hydra_Software_Devel/179   9/29/09 4:06p davidp
 * SW7405-3049: Add 1k to context memory for AVC HD/SD buffers to support
 * AVC in AVI decode.
 * 
 * Hydra_Software_Devel/178   7/6/09 3:35p davidp
 * PR56374: Turn on clocks in BXVD_Close to access AVD FW and AVD
 * registers.
 * 
 * Hydra_Software_Devel/177   6/30/09 4:20p davidp
 * PR56374: Power down AVD related clocks in BXVD_Close.
 * 
 * Hydra_Software_Devel/176   6/25/09 6:05p davidp
 * PR56375: Remove unneeded callback for AVD FW command processing. This
 * fixes the polled debug method of cmd acknowledgement.
 * 
 * Hydra_Software_Devel/175   6/1/09 11:13p davidp
 * PR55198: Add conditional around BXVD_P_Hibernate.
 * 
 * Hydra_Software_Devel/174   6/1/09 6:35p davidp
 * PR55198: Add dynamic power management support to most STB platforms.
 * 
 * Hydra_Software_Devel/173   5/27/09 4:38p davidp
 * PR50178: Keep AVD clocks enabled when closing still compatibility
 * channel.
 * 
 * Hydra_Software_Devel/172   4/24/09 12:36p davidp
 * PR43272: Update BluRay memory configuration picture block size and
 * count.
 * 
 * Hydra_Software_Devel/171   4/9/09 5:33p davidp
 * PR52344: Fix blk size and blk count table lookup.
 * 
 * Hydra_Software_Devel/170   3/31/09 5:30p davidp
 * PR52344: Add bAVC51Enable flag to channel settings to use more memory
 * for AVC 5.1 compliant streams.
 * 
 * Hydra_Software_Devel/169   2/26/09 3:27p nilesh
 * PR52516: Added const specifier to static sChannelFWMemCfg_bExcessDir to
 * adhere to magnum architecture rules
 * 
 * Hydra_Software_Devel/168   1/27/09 1:48p nilesh
 * PR51621: Resolved possible global variable naming conflict by either
 * explicitly declaring globals as static (where possible) or adding
 * BXVD_P_ prefix
 * 
 * Hydra_Software_Devel/167   1/21/09 2:02p nilesh
 * PR45052: Converted BDBG_xxx to BDBG_INSTANCE_xxx calls to support
 * multiple channels
 * 
 * Hydra_Software_Devel/166   11/26/08 1:54p davidp
 * PR49602: Allocate CABAC bin buffer 256 byte aligned.
 * 
 * Hydra_Software_Devel/165   11/21/08 6:30p davidp
 * PR48684: Fix compiler warning message.
 * 
 * Hydra_Software_Devel/164   11/19/08 5:15p davidp
 * PR48684: Add debug messages to indicate the still picture decode
 * resolution.
 * 
 * Hydra_Software_Devel/163   11/14/08 1:42p davidp
 * PR48684: Remove decode code from BXVD_P_GetStillDecodeFWMemSize.
 * 
 * Hydra_Software_Devel/162   11/12/08 6:06p davidp
 * PR48264: Increase cabac bin HD and SD memory size.
 * 
 * Hydra_Software_Devel/161   11/10/08 11:06a davidp
 * PR48684: Flag coverity deadcode failure as false.
 * 
 * Hydra_Software_Devel/160   10/28/08 12:30p davidp
 * PR47278: Add support for ExcessDirMode, allocate more direct memory for
 * certain AVC streams when flag is set.
 * 
 * Hydra_Software_Devel/159   10/23/08 5:08p davidp
 * PR48295: Stripe width of 256 not currently supported. Do not allow QCIF
 * and CIF still picture decodes.
 * 
 * Hydra_Software_Devel/158   9/18/08 2:03p davidp
 * PR46650: Fix MemCfgMode enum field names, change BDBG_WRN to BDBG_MSG.
 * 
 * Hydra_Software_Devel/157   9/10/08 4:21p davidp
 * PR46650: Output a warning when separate picture heap is specified with
 * a zero picture memory size.
 * 
 * Hydra_Software_Devel/156   9/8/08 6:42p davidp
 * PR46650: Add BXVD_P_GET_MEMORY_CONFIG macro and BXVD_P_ValidateHeaps to
 * sanity check heaps passed to BXVD_Open.
 * 
 * Hydra_Software_Devel/155   8/28/08 4:54p davidp
 * PR44341: Add coverity overide comments for false positive coverity
 * defects.
 * 
 * Hydra_Software_Devel/154   8/20/08 5:03p davidp
 * PR43272: Init local vars in
 * BXVD_P_InitializeFWMemConfigCompatibilityModeTable().
 * 
 * Hydra_Software_Devel/153   8/5/08 3:56p nilesh
 * PR45322: BXVD_DMDBG_P_DumpYUV() no longer uses hard coded stripe width
 * 
 * Hydra_Software_Devel/152   8/4/08 4:07p davidp
 * PR43272: Update AVC still picture memory table.
 * 
 * Hydra_Software_Devel/151   8/1/08 4:57p davidp
 * PR43272: Merge PR43272 branch to mainline, new FW memory configuration
 * API.
 *
 * Hydra_Software_Devel/PR43272/1   7/17/08 12:36p davidp
 * PR43272: Add support for additional FW memory configuration parameters,
 * Direct mode and IL Work list memory.
 * 
 * Hydra_Software_Devel/150   4/21/08 11:13a nilesh
 * PR41276: Added explicit bounds check for StripeMultiple before using it
 * to index into an array
 * 
 * Hydra_Software_Devel/149   4/3/08 2:17p nilesh
 * PR41276: Fixed possible out of bounds condition for StripeWidth and
 * StripeMultiple
 * 
 * Hydra_Software_Devel/148   4/1/08 2:41p davidp
 * PR27168: Add routine description for
 * BXVD_P_SetupStillPictureCompatibilityMode
 * 
 * Hydra_Software_Devel/147   3/25/08 5:20p davidp
 * PR40316: Merge branch PR40316 changes to mainline, update CABAC bin
 * sizes per FW doc.
 * 
 * Hydra_Software_Devel/146   3/18/08 1:58p davidp
 * PR40316: Reduce cabac memory size for HD AVC still picture decode.
 * 
 * Hydra_Software_Devel/145   3/17/08 4:41p davidp
 * PR40316: Temporarily increase the cabac size for AVC HD Still decode.
 * 
 * Hydra_Software_Devel/144   3/7/08 10:38a nilesh
 * PR40349: Update memory tables according to AVD Mosaic API doc v2.19
 * 
 * Hydra_Software_Devel/143   2/13/08 5:06p nilesh
 * PR39469: Update memory tables according to AVD Mosaic API doc v2.18
 * 
 * Hydra_Software_Devel/142   1/31/08 12:40p nilesh
 * PR39174: Updated memory tables to match AVD Mosaic API doc v2.17.
 * Restructured memory size calculation code.
 * 
 * Hydra_Software_Devel/141   1/24/08 11:40a nilesh
 * PR38570: Merge CABAC worklist API changes to mainline
 * 
 * Hydra_Software_Devel/PR38570/2   1/22/08 2:16p nilesh
 * PR38570: Fixed compatibility mode memory table for still picture modes
 * 
 * Hydra_Software_Devel/PR38570/1   1/21/08 5:12p nilesh
 * PR38570: Added support for cabac worklist buffer and size parameter to
 * AVD ChannelOpen
 * 
 * Hydra_Software_Devel/140   1/8/08 11:56a nilesh
 * PR38570: Updated memory table to match AVD Mosaic API document v2.15
 * 
 * Hydra_Software_Devel/139   11/19/07 3:37p nilesh
 * PR36916: Updated context sizes based on AVD Mosaic API document v2.13
 * 
 * Hydra_Software_Devel/138   11/6/07 2:19p davidp
 * PR27168: Remove use of SETUP_HEAP macro and associated routines per
 * code review
 * 
 * Hydra_Software_Devel/137   9/28/07 12:22p nilesh
 * PR27168: BXVD_P_ParseDecodeMode now checks for invalid decode mode in
 * list
 * 
 * Hydra_Software_Devel/136   9/6/07 5:35p nilesh
 * PR29915: Added BERR_TRACE wrapper around all return codes
 * 
 * Hydra_Software_Devel/135   9/4/07 10:10a nilesh
 * PR29915: Fix error handling in BXVD_OpenChannel() when any error
 * occurs.  BXVD_CloseChannel() is now called to free any allocated
 * memory before returning an error.
 * 
 * Hydra_Software_Devel/134   8/31/07 3:24p nilesh
 * PR29915: Fix boot result code propogation
 * 
 * Hydra_Software_Devel/133   8/31/07 11:41a nilesh
 * PR29915: Fix FGT support for DVD platforms
 * 
 * Hydra_Software_Devel/132   8/30/07 12:20p nilesh
 * PR34430: Channel specific heaps are now used for address conversion
 * 
 * Hydra_Software_Devel/131   8/29/07 4:38p nilesh
 * PR34430: XVD now supports a general memory size of 0 specified at
 * BXVD_Open
 * 
 * Hydra_Software_Devel/130   8/29/07 1:08p nilesh
 * PR34430: Added support for channel specific memory heaps
 * 
 * Hydra_Software_Devel/129   8/22/07 3:49p nilesh
 * PR29915: The FW memory config table for compatibility mode is now
 * generated on-the-fly at runtime
 * 
 * Hydra_Software_Devel/128   8/21/07 3:04p davidp
 * PR34052: Remove nolonger used register addr pointers from BXVD_Handle,
 * use platform_info.stReg structure to reference registers
 * 
 * Hydra_Software_Devel/127   8/20/07 4:03p nilesh
 * PR29915: Fixed size of Atom A for odd multiple of 64
 * 
 * Hydra_Software_Devel/126   8/20/07 4:02p nilesh
 * PR29915: Fixed secure heap memory allocation
 * 
 * Hydra_Software_Devel/125   8/13/07 4:08p nilesh
 * PR29915: Multi-decode merge to mainline
 * 
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/25   8/9/07 4:06p nilesh
 * PR29915: Updated AVC QCIF (1.1) Cabac Size
 * 
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/24   8/9/07 3:50p nilesh
 * PR29915: Updated memory tables according to AVD Mosaic Doc v2.11 (Aug
 * 9, 2007)
 * 
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/23   7/26/07 4:00p nilesh
 * PR29915: Add MPEG2 w/o BTP support
 * 
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/22   7/19/07 5:17p nilesh
 * PR29915: Fixed stripe_width value in still picture buffer
 * 
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/21   7/19/07 5:14p nilesh
 * PR29915: Update BXVD_GetBufferConfig() and BXVD_P_ParseDecodeMode() to
 * handle BXVD_DecodeMode_eCustom
 * 
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/20   7/19/07 2:43p nilesh
 * PR29915: Cleaned up variable naming style
 * 
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/19   7/17/07 11:35a nilesh
 * PR29915: Fixed incorrect atom size for MPEG2 QCIF
 * 
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/18   7/13/07 5:15p nilesh
 * PR29915: Updated memory tables based on v2.9 of Mosaic API doc
 * 
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/17   7/10/07 5:18p nilesh
 * PR29915: Fixed bookkeeping related to still picture channel allocation
 * when using new API
 * 
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/16   7/3/07 12:09p nilesh
 * PR29915: Added FGT support
 * 
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/15   6/28/07 1:36p davidp
 * PR29915: Move remaining data structure definitions for
 * bxvd_devcmds_priv.h to bxvd_vdec_api.h
 * 
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/14   6/19/07 2:33p nilesh
 * PR29915: Changed VideoCmprCount -> uiVideoCmprCount to match notation
 * of other fields in BXVD_ChannelSettings
 * 
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/13   6/19/07 12:13p nilesh
 * PR29915: Merge from mainline
 * 
 * Hydra_Software_Devel/124   6/5/07 1:40p davidp
 * PR31721: Clear edge triggered BVNF register after FW is reloaded and
 * started.
 * 
 * Hydra_Software_Devel/123   4/24/07 1:39p nilesh
 * PR22766: Added support for new FW API to provide FGT buffer address and
 * size
 * 
 * Hydra_Software_Devel/122   3/27/07 2:02p pblanco
 * PR27168: Removed deprecated bxvd_mem.h include.
 * 
 * Hydra_Software_Devel/121   3/2/07 1:36p nilesh
 * PR26188: Merged 7400B0 bring-up branch to mainline.  Cleaned up heap
 * naming and usage.
 * 
 * Hydra_Software_Devel/PR26188/3   2/28/07 4:27p nilesh
 * PR26188: Fixed incorrect calculation of FW Context base phy addr on
 * 7400B0
 * 
 * Hydra_Software_Devel/PR26188/2   2/27/07 6:03p nilesh
 * PR26188: Remove hack to set picture buffer address on 7400B0
 * 
 * Hydra_Software_Devel/PR26188/1   2/27/07 4:05p nilesh
 * PR26188: 7400B0 Bring-Up
 * 
 * Hydra_Software_Devel/120   2/26/07 8:47p nilesh
 * PR26188: 7400B0 Bring Up - Hack to get private video memory addresses
 * sent to fw correctly
 * 
 * Hydra_Software_Devel/119   2/6/07 11:07a nilesh
 * PR27575: Fix 7440 Watchdog and make watchdog more robust on all
 * platforms.
 * 
 * Hydra_Software_Devel/PR27575/1   2/2/07 2:12p nilesh
 * PR27575: Fix 7440 Watchdog
 * 
 * Hydra_Software_Devel/117   12/12/06 3:00p davidp
 * PR25443: Merge Simplified FW API branch into mainline
 * 
 * Hydra_Software_Devel/116   12/11/06 2:51p nilesh
 * PR26414: Merge branch PR26414 to mainline
 * 
 * Hydra_Software_Devel/PR26414/1   12/11/06 10:53a nilesh
 * PR26414: Added AVD response callback error handling during a watchdog
 * reset
 * 
 * Hydra_Software_Devel/115   11/17/06 4:50p nilesh
 * PR25868: Merged authenticated image support to mainline
 * 
 * Hydra_Software_Devel/PR25868/1   11/16/06 3:35p nilesh
 * PR25868: Added error checking
 * 
 * Hydra_Software_Devel/114   10/20/06 3:16p davidp
 * PR25038: Enabled HD still picture decode when BXVD_DecodeMode_eAll is
 * in decode mode list.
 * 
 * Hydra_Software_Devel/113   9/22/06 2:22p davidp
 * PR22867: Update FW memory usage table, add secure memory heap support.
 * 
 * Hydra_Software_Devel/112   9/5/06 3:05p davidp
 * PR22867: Merge secure heap (Cabac) support
 * 
 * Hydra_Software_Devel/xvd_cabac_heap/1   8/30/06 5:10p davidp
 * 22867: Separate FW memory so Cabac bin memory can me moved to secure
 * heap.
 * 
 * Hydra_Software_Devel/111   7/19/06 11:22a pblanco
 * PR22673: Added the remainder of bxvd_priv.c conditionalized code to
 * platform and core modules.
 * 
 * Hydra_Software_Devel/110   7/18/06 3:30p pblanco
 * PR22673: Moved FW load to platform appropriate modules for 7400, 7401
 * and 7118.
 * 
 * Hydra_Software_Devel/109   7/18/06 10:16a pblanco
 * PR22673: Added additional comments to clarify the boundaries of the
 * 7400, 7401 and 7118 conditional code.
 * 
 * Hydra_Software_Devel/108   7/18/06 12:11a nilesh
 * PR22673: Code restructure for 97400, 97401 B0, and 97118
 * 
 * Hydra_Software_Devel/PR22673/2   7/17/06 2:51p nilesh
 * PR22673: Restructure on 97400
 * 
 * Hydra_Software_Devel/PR22673/1   7/17/06 2:33p nilesh
 * PR22673: Restructure on 97401
 * 
 * Hydra_Software_Devel/107   7/17/06 1:56p pblanco
 * PR22673: Removed 7401a0 conditionalized code and restructured some of
 * the remaining conditional code.
 * 
 * Hydra_Software_Devel/106   7/13/06 4:56p davidp
 * PR22673: Add BXVD_P_SetWatchdog_7401B0 routine, remove some 7401A0
 * support.
 * 
 * Hydra_Software_Devel/105   7/13/06 1:46p davidp
 * PR20017: Add 7118 L2 AVD interrupt definition include file
 * 
 * Hydra_Software_Devel/104   7/12/06 4:48p davidp
 * PR20017: Cleanup BXVD_Open, add platform specific setup routines
 * 
 * Hydra_Software_Devel/103   7/11/06 5:06p davidp
 * PR20017: Add FW inner loop Watchdog support, also add platform specific
 * SetupInterrupts routines
 * 
 * Hydra_Software_Devel/102   7/6/06 9:55a pblanco
 * PR21943: Conditionalized for 7118. Somehow this was missed yesterday.
 * 
 * Hydra_Software_Devel/100   6/28/06 4:45p davidp
 * PR20017: FW end of code register is actually the size of the FW code
 * region.
 * 
 * Hydra_Software_Devel/99   6/22/06 1:47p pblanco
 * PR22301: Removed include of bmem_priv.h
 * 
 * Hydra_Software_Devel/98   6/13/06 10:58a pblanco
 * PR22093: Made suggested changes based on the PR details.
 * 
 * Hydra_Software_Devel/97   6/12/06 6:09p davidp
 * PR20017: Allocate memory for FW code out of framework heap.
 * 
 * Hydra_Software_Devel/96   6/9/06 6:25p davidp
 * PR21846: Fixed compiler warnings caused by gcc option "-O3"
 * 
 * Hydra_Software_Devel/95   6/8/06 9:18a pblanco
 * PR20017: Changed chip version test conditionals from A0 and B0 to
 * BCHP_VER_A0 and BCHP_VER_B0 to conform with the rest of magnum.
 * 
 * Hydra_Software_Devel/94   5/26/06 4:37p davidp
 * PR20017: Disable FW mailbox polling for all but 7401 B0
 * 
 * Hydra_Software_Devel/93   5/25/06 4:17p pblanco
 * PR20017: Changed global IO register from CPUREGS to CPUREGS2.
 * 
 * Hydra_Software_Devel/92   5/25/06 3:52p pblanco
 * PR20017: Change heap referenced by B0 communication vectors.
 * 
 * Hydra_Software_Devel/91   5/25/06 3:09p pblanco
 * PR20017: Reloc sanity check in.
 * 
 * Hydra_Software_Devel/90   5/24/06 12:11p pblanco
 * PR20017: Removed offset from inner loop end of code address. It's
 * already offset by the relocation engine.
 * 
 * Hydra_Software_Devel/89   5/24/06 11:16a pblanco
 * PR20017: Code now uses defined constant for inner loop address offset
 * instead of a hardwired value.
 * 
 * Hydra_Software_Devel/88   5/24/06 10:07a pblanco
 * PR20017: Pull relocation address from XVD context structure.
 * 
 * Hydra_Software_Devel/87   5/24/06 10:02a pblanco
 * PR20017: Added code necessary to enable execution of relocated FW.
 * 
 * Hydra_Software_Devel/86   5/23/06 4:05p pblanco
 * PR20017: Pull in bchp_decode_cpuregs.h
 * 
 * Hydra_Software_Devel/85   5/23/06 2:41p pblanco
 * PR20017: Conditionalized the relocatable vs non-relocatable FW load by
 * chip revision type.
 * 
 * Hydra_Software_Devel/84   5/18/06 9:54a pblanco
 * PR21612: Made sure we don't attempt to use an unassigned data pointer
 * and removed the dereference from the pointer increment.
 * 
 * Hydra_Software_Devel/83   5/16/06 10:20a pblanco
 * PR20649: Cleanup for B0. Conditionals must be replaced or removed in
 * later development.
 * 
 * Hydra_Software_Devel/82   5/15/06 1:43p pblanco
 * PR20017: Moved external references for relocatable FW arrays to
 * bxvd_img.h
 * 
 * Hydra_Software_Devel/81   5/11/06 3:03p pblanco
 * PR20017: Fixed relocatable FW load addresses.
 * 
 * Hydra_Software_Devel/80   5/10/06 4:18p pblanco
 * PR20017: Changed from header based FW code to C source base.
 * 
 * Hydra_Software_Devel/79   5/10/06 11:01a pblanco
 * PR19877: Removed unused host commands identified by Gaurav.
 * 
 * Hydra_Software_Devel/78   5/5/06 2:10p davidp
 * PR21156: Add BXVD_P_ReleasePPB_isr and related _isr routines for DM to
 * use at ISR time.
 * 
 * Hydra_Software_Devel/77   5/5/06 1:23p pblanco
 * PR20017: Incremental relocation code checkin.
 * 
 * Hydra_Software_Devel/76   5/4/06 3:01p pblanco
 * PR20017: Initial relocatable test code on 7401a0. Checked in with
 * "normal" code paths active (no relocation).
 * 
 * Hydra_Software_Devel/75   4/26/06 11:51a pblanco
 * PR20649: Added temporary definition of
 * BCHP_DECODE_CPUREGS2_REG_CPU_INT_BASE_1 until the name is fixed in
 * CHP. It is _INST_ instead of _INT_ in rdb/b0.
 * 
 * Hydra_Software_Devel/74   4/24/06 2:11p davidp
 * PR18043: Add support for Sequence Header interrupt processing
 * 
 * Hydra_Software_Devel/73   4/17/06 3:57p pblanco
 * PR1923: Modified BXVD_P_SVChipEnableExecution to use page register init
 * value definitions instead of hard coded values.
 * 
 * Hydra_Software_Devel/72   4/12/06 12:49p davidp
 * PR19600: Move 7401 FW load address to 48mb, also decrease size to 1mb
 * 
 * Hydra_Software_Devel/71   3/29/06 2:07p pblanco
 * PR19877: Added BXVD_P_GetSeqHdr API.
 * 
 * Hydra_Software_Devel/70   3/22/06 5:16p davidp
 * PR20347: Add L2 interrupt support
 * 
 * Hydra_Software_Devel/69   3/6/06 8:47a pblanco
 * PR20017: Check in of Olive's changes after successful weekend
 * regression test.
 * 
 * Hydra_Software_Devel/67   2/13/06 3:38p pblanco
 * PR19123: Disable enabling of L2 interrupts during chip initialization.
 * 
 * Hydra_Software_Devel/66   2/2/06 4:04p pblanco
 * PR19123: More code cleanup. Changed all instances of uAvcInstance to
 * uDecoderInstance and removed last remaining references to the old
 * decoder context array.
 * 
 * Hydra_Software_Devel/65   2/2/06 1:29p pblanco
 * PR19085: Moved manually generated register define from bchp_7401.h to
 * source.
 * 
 * Hydra_Software_Devel/64   2/2/06 10:56a pblanco
 * PR19123: Only reset the appropriate instance of the decoder on init.
 * 
 * Hydra_Software_Devel/63   2/1/06 3:54p pblanco
 * PR19123: Changed "AVC" instance array in XVD context structure to a
 * single instance of type BXVD_DecoderContext. Tested against 7401 with
 * no regressions and against 7400 with no regressions.
 * 
 * Hydra_Software_Devel/62   1/30/06 2:42p pblanco
 * PR19123: Clean up decoder instance usage throughout code.
 * 
 * Hydra_Software_Devel/61   1/27/06 4:16p pblanco
 * PR19123: Reversed order of shim register initialization (DB & PFRI) as
 * requested by Olive. Also removed some unused GPIO initialization code.
 * 
 * Hydra_Software_Devel/60   1/27/06 1:38p pblanco
 * PR19123: Modified GPIO pin-mux setup to work with new kernel spin. Now
 * all UARTs are where they should be.
 * 
 * Hydra_Software_Devel/59   1/26/06 5:34p davidp
 * PR19123: Remove OS specific include file usage:
 * 
 * Hydra_Software_Devel/58   1/26/06 4:03p davidp
 * PR19123: Remove printf debug messages, clean up compiler warnings:
 * 
 * Hydra_Software_Devel/57   1/25/06 6:53p davidp
 * PR19123: Add platform specific definition of FS_MAP_SIZE:
 * 
 * Hydra_Software_Devel/56   1/25/06 11:51a pblanco
 * PR19123: Added debugging printfs for 7400 (will be removed later).
 * 
 * Hydra_Software_Devel/54   1/24/06 2:51p pblanco
 * PR19123: 7400 firmware bring up debugging changes.
 * 
 * Hydra_Software_Devel/53   1/23/06 1:46p pblanco
 * PR19123: Added correct page register initialization based on Ray's
 * calculations.
 * 
 * Hydra_Software_Devel/52   1/20/06 11:01a pblanco
 * PR19123: Fix link error caused by 7400 external definitions.
 * 
 * Hydra_Software_Devel/51   1/20/06 10:53a pblanco
 * PR19123: Support for 7400 decoder instances.
 * 
 * Hydra_Software_Devel/50   1/18/06 2:09p pblanco
 * PR19123: Modifications for 7400 port so code will build for xvd_test on
 * that platform.
 * 
 * Hydra_Software_Devel/49   1/18/06 10:04a pblanco
 * PR19123: Initial porting work for 7400 FW loading and initialization.
 * 
 * Hydra_Software_Devel/48   1/16/06 1:28p davidp
 * PR16792: BXVD_Open now uses two heap pointers, one for FW code (2MB)
 * the other for FW picture buffers.:
 * 
 * Hydra_Software_Devel/47   1/13/06 11:42a pblanco
 * PR16052: Initial 7400 porting changes.
 * 
 * Hydra_Software_Devel/46   1/9/06 12:42p pblanco
 * PR16052: Removed unused code and did some general code cleanup before
 * 7400 port.
 * 
 * Hydra_Software_Devel/45   12/22/05 1:30p pblanco
 * PR16052: Temporary fix to send still picture decode flag to FW.
 * 
 * Hydra_Software_Devel/44   12/9/05 3:19p vsilyaev
 * PR 18019: Fixed include files
 * 
 * Hydra_Software_Devel/43   12/2/05 9:58a pblanco
 * PR16052: Moved BXVD_ReleasePPB to BXVD_P_ReleasePPB.
 * 
 * Hydra_Software_Devel/42   11/28/05 3:06p davidp
 * PR18263: Remove addition unused posix include files:
 * 
 * Hydra_Software_Devel/41   11/28/05 2:58p davidp
 * PR18263: OS dependant include files removed, not needed:
 * 
 * Hydra_Software_Devel/40   11/15/05 11:15a pblanco
 * PR16052: Removed queueing code and disabled XVD control of ARC UARTS.
 * 
 * Hydra_Software_Devel/39   11/10/05 1:35p pblanco
 * PR16052: Turn off FW relocation.
 * 
 * Hydra_Software_Devel/38   11/10/05 7:52a pblanco
 * PR16052: FW now uses heap info to determine load and execution
 * addresses. Also removed unused code that existed when XVD used to
 * create and map its own heap. Corresponding changes were made in
 * related modules also checked in at this time.
 * 
 * Hydra_Software_Devel/37   11/8/05 1:00p pblanco
 * PR16052: Changed register defines to new values. Also some (commented
 * out) changes for relocatable FW.
 * 
 * Hydra_Software_Devel/36   11/4/05 9:17a pblanco
 * PR16052: Conditionalized return value declaration for XVD UART control.
 * 
 * Hydra_Software_Devel/35   11/1/05 8:09a pblanco
 * PR16052: Moved bxvd_arc_load.h include into conditional section.
 * 
 * Hydra_Software_Devel/34   10/26/05 10:16a davidp
 * PR16052: Cleaned up compiler warnings:
 * 
 * Hydra_Software_Devel/33   10/25/05 7:58p davidp
 * PR16052: Fix compiler warnings and remove <CR> from report message
 * strings:
 * 
 * Hydra_Software_Devel/32   10/21/05 6:43a pblanco
 * PR16052: This checkin contains a temporary fix for ARC UART control
 * until it is implemented in the app layer. It also contains a new
 * version of the relocatable firmware code which can be conditionalized
 * out until we're comfortable with all the issues. By default this code
 * is built with relocatable FW off and the temporary UART code on.
 * 
 * Hydra_Software_Devel/31   10/19/05 7:04a pblanco
 * PR17672: Removed code that overwrote I2C pin mux settings.
 * 
 * Hydra_Software_Devel/30   10/11/05 6:39p davidp
 * PR17374: Remove mmap calls, let application allocate FW memory regions:
 * 
 * Hydra_Software_Devel/29   10/7/05 3:30p pblanco
 * PR16052: Added queue management APIs.
 * 
 * Hydra_Software_Devel/28   10/4/05 9:50a pblanco
 * PR16052: Added some BDBG_MSG output to dump start of FW and shared
 * memory.
 * 
 * Hydra_Software_Devel/27   9/29/05 5:59p pblanco
 * PR16052: Checked in Dave's changes to reduce debug output.
 * 
 * Hydra_Software_Devel/26   9/26/05 6:35p davidp
 * PR16052: Use BMEM_CreateHeapSystem to force BMEM to not use the created
 * heap for bookkeeping structure.:
 * 
 * Hydra_Software_Devel/25   9/26/05 2:26p pblanco
 * PR16052: Added experimental code to convert vaddr to physical offset.
 * 
 * Hydra_Software_Devel/24   9/22/05 2:58p davidp
 * PR16052: Create BMEM heap for the FW/XVD shared memory block to allow
 * VDC to use BMEM calls to translate VM/Phys addresses:
 * 
 * Hydra_Software_Devel/23   9/16/05 8:36a pblanco
 * PR16052: Moved decoder control functions pause, resume and reset from
 * bxvd.c to private (_P_) functions.
 * 
 * Hydra_Software_Devel/22   9/8/05 8:55p davidp
 * PR16052: Add AVD reset to BXVD_P_Reset7401 routine:
 * 
 * Hydra_Software_Devel/20   9/6/05 1:09p pblanco
 * PR16052: Changed BXVD_P_GetFWVOffset to determine if the physical
 * address argument is in the FW or shared memory area and return the
 * appropriate virtual address.
 * 
 * Hydra_Software_Devel/19   9/6/05 10:25a pblanco
 * PR16052: Added new function protoypes for physical to virtual address
 * translation and moved some defines from bxvd_priv.c
 * 
 * Hydra_Software_Devel/18   9/2/05 10:05a pblanco
 * PR16052: Fixed compile errors when DEBUG=y.
 * 
 * Hydra_Software_Devel/17   8/31/05 3:04p pblanco
 * PR16052: Moved the shared memory area up in memory.
 * 
 * Hydra_Software_Devel/16   8/29/05 8:37a pblanco
 * PR16052: Support for new FW.
 * 
 * Hydra_Software_Devel/15   8/25/05 1:37p pblanco
 * PR16052: mmap failure now returns a BXVD error code instead of -1.
 * 
 * Hydra_Software_Devel/14   8/24/05 1:14p pblanco
 * PR16052: Added support for multiple AVC instances and cleaned up some
 * code.
 * 
 * Hydra_Software_Devel/13   8/18/05 2:12p pblanco
 * PR16052: Code fixes for BXVD_OpenChannel and BXVD_StartDecode.
 * 
 * Hydra_Software_Devel/12   8/16/05 1:06p pblanco
 * PR16052: FW download and execution are working... this is a sanity
 * checkin.
 * 
 * Hydra_Software_Devel/11   8/12/05 7:03a pblanco
 * PR16052: Sanity checkin so Dave can build the latest test harness code.
 * 
 * Hydra_Software_Devel/10   8/5/05 3:46p pblanco
 * PR16052: Many changes to support firmware downloading.
 * 
 * Hydra_Software_Devel/9   7/26/05 11:34a pblanco
 * PR16052: Changed DRAM configuration code after talking to Gaurav.
 * 
 * Hydra_Software_Devel/8   7/26/05 10:29a pblanco
 * PR16052: Added DRAM setup and chip reset code.
 * 
 * Hydra_Software_Devel/7   7/25/05 4:01p pblanco
 * PR16052: Code changes per review with Jason. Moved locally defined
 * error code to bxvd_error.h
 * 
 * Hydra_Software_Devel/6   7/25/05 12:26p ebrakus
 * PR16052: use Avc contexts
 * 
 * Hydra_Software_Devel/5   7/22/05 11:36a pblanco
 * PR16052: Code cleanup and data structure factoring.
 * 
 * Hydra_Software_Devel/4   7/15/05 1:08p pblanco
 * PR16052: Clean build with new code and data.
 * 
 * Hydra_Software_Devel/3   7/13/05 12:33p pblanco
 * PR16052: Changed ARC memory and register references to XVD.
 * 
 * Hydra_Software_Devel/1   7/13/05 8:40a pblanco
 * PR16052: Initial check in with some code additions.
 * 
 ***************************************************************************/
#include "bstd.h"
#include "bkni.h"          /* For malloc */
#include "bmem.h"
#include "bxvd.h"
#include "bxvd_platform.h"
#include "bxvd_priv.h"
#include "bxvd_reg.h"
#include "bxvd_errors.h"
#include "bxvd_intr.h"

#ifdef BCHP_PWR_SUPPORT
#include "bchp_pwr.h"
#endif

#define BXVD_P_TEST_CHUNK 16

/*
 * For testing during initial bringup, we may not want to send an init host
 * command, so setting this define to 0 will skip the init.
 */
#define FW_INIT 1

/* Temporary until this is properly defined */
#ifndef BCHP_DECODE_CPUREGS2_REG_CPU_INT_BASE_1
#define BCHP_DECODE_CPUREGS2_REG_CPU_INT_BASE_1 0x00800f8c
#endif

/* AVD Memory requirements based on "AVD Core Mosaic Mode API", Rev
 * 2.19, March 3, 2008 */

#define BXVD_P_STRIPE_VIDEO_ATOM_NUM 11

static const uint32_t sVideoAtomSize[BXVD_P_STRIPE_WIDTH_NUM][BXVD_P_STRIPE_MULTIPLE_NUM][BXVD_P_STRIPE_VIDEO_ATOM_NUM] =
{
   {
      {3194880,  688128,  172032,  159744, 2064384, 6389760, 5898240, 12779520, 6389760, 3207168, 13631488}, /* 0/0 (64/32) */
      {3276800,  688128,  196608,  221184, 2064384, 6389760, 6553600, 12779520, 6881280, 3203072, 13631488}, /* 0/1 (64/64) */
      {3440640,  786432,  294912,  294912, 2359296, 6881280, 6553600, 13762560, 7864320, 3342336, 13631488}  /* 0/2 (64/128) */
   },   
   {
      {3194880,  688128,  172032,  196608, 2064384, 6389760, 5898240, 12779520, 6389760, 3391488, 13631488}, /* 1/0 (128/32) */
      {3276800,  688128,  196608,  294912, 2064384, 6389760, 6553600, 12779520, 6881280, 3391488, 13631488}, /* 1/1 (128/64) */
      {3440640,  786432,  294912,  393216, 2359296, 6881280, 6553600, 13762560, 7864320, 3538944, 13631488}  /* 1/2 (128/128) */
   }
};

/* Only stripe widths of 64 and 128 are supported. */ 
const uint32_t BXVD_P_StripeWidthLUT[BXVD_P_STRIPE_WIDTH_NUM] =
{
   64,
   128
};

typedef enum BXVD_VideoProtocol
{
   BXVD_VideoProtocol_eAVC,   /* AVC, H264 */
   BXVD_VideoProtocol_eMPEG2, /* MPEG2, H261, H263, MPEG1, MPEG2DTV, MPEG2_DSS_PES */
   BXVD_VideoProtocol_eVC1,   /* VC1, VC1-SM */
   BXVD_VideoProtocol_eMPEG4, /* MPEG2 Part 2, DivX */
   BXVD_VideoProtocol_eAVS,   /* AVS */
   BXVD_VideoProtocol_eVP8,   /* VP8 */
   BXVD_VideoProtocol_eMVC,   /* Multi View Coding */
   BXVD_VideoProtocol_eSVC,   /* Scaleable Video Coding */
   BXVD_VideoProtocol_eHEVC,  /* High Efficiency Video Coding */
   
   /* Add new protocols ABOVE this line */
   BXVD_VideoProtocol_eMax
} BXVD_VideoProtocol;

static const BXVD_P_FWMemConfig_V2 sChannelFWMemCfg[BXVD_VideoProtocol_eMax][BXVD_DecodeResolution_eMaxModes] =
{  /* Context, InnerLoop WL, DirectMode, Cabac bin,  Cabac WL,  Vid Blk (index),  Blk Cnt */

   /* BXVD_VideoProtocol_eAVC */
   { 
      {261440, 1572864, 1638400, 5210112, 131072, BXVD_P_VideoAtomIndex_eA, 7}, /* AVC HD */
      {261440,  786432,  388800, 2084864,  65536, BXVD_P_VideoAtomIndex_eE, 3}, /* AVC SD */
      {261440,  393216,  221760,  417792,  16384, BXVD_P_VideoAtomIndex_eC, 9}, /* AVC CIF */
      {261440,  196608,   44000,  106496,  16384, BXVD_P_VideoAtomIndex_eD, 4}  /* AVC QCIF */
   },

   /* BXVD_VideoProtocol_eMPEG2 */
   { 
      {154944, 65536, 0, 2097152, 131072, BXVD_P_VideoAtomIndex_eA, 6}, /* MPEG2 HD w/ BTP */
      {154944, 65536, 0, 1048576,  65536, BXVD_P_VideoAtomIndex_eB, 6}, /* MPEG2 SD w/ BTP */
      {154944, 16384, 0,  312448,  16384, BXVD_P_VideoAtomIndex_eC, 5}, /* MPEG2 CIF */
      {154944, 16384, 0,   54656,  16384, BXVD_P_VideoAtomIndex_eD, 2}  /* MPEG2 QCIF */
   },

   /* BXVD_VideoProtocol_eVC1 */
   { 
      {154944,	917504,	103680, 3906176, 131072, BXVD_P_VideoAtomIndex_eA, 5}, /* VC1 HD */
      {154944,  204800,  21632, 1562496,  65536, BXVD_P_VideoAtomIndex_eB, 5}, /* VC1 SD */
      {154944,  102400,   5888,  312448,  16384, BXVD_P_VideoAtomIndex_eC, 5}, /* VC1 CIF */
      {154944,  102400,   1792,   54656,  16384, BXVD_P_VideoAtomIndex_eD, 2}  /* VC1 QCIF */
   },

   /* BXVD_VideoProtocol_eMPEG4 */
   { 
      {154944, 98304, 168960, 3906176, 131072, BXVD_P_VideoAtomIndex_eA, 5}, /* MPEG4/DivX HD */
      {154944, 98304,  34624, 1562496,  65536, BXVD_P_VideoAtomIndex_eB, 5}, /* MPEG4/DivX SD */
      {154944, 20480,   9024,  312448,  16384, BXVD_P_VideoAtomIndex_eC, 5}, /* MPEG4/DivX CIF */
      {154944, 20480,	2560,   54656,  16384, BXVD_P_VideoAtomIndex_eD, 2}  /* MPEG4/DivX QCIF */
   },

   /* BXVD_VideoProtocol_eAVS */
   { 
      {154944, 65536, 587520, 3906176, 131072, BXVD_P_VideoAtomIndex_eA, 5}, /* AVS HD */
      {154944, 65536, 116600, 1562496,  65536, BXVD_P_VideoAtomIndex_eB, 5}, /* AVS SD */
      {154944, 16384,  28512,  312448,  16384, BXVD_P_VideoAtomIndex_eC, 5}, /* AVS CIF */
      {154944, 16384,   7128,   54656,  16384, BXVD_P_VideoAtomIndex_eD, 2}  /* AVS QCIF */
   },

   /* BXVD_VideoProtocol_eVP8 */
   { 
      {154944, 917504, 2774420, 5210112, 131072, BXVD_P_VideoAtomIndex_eA, 5}, /* VP8 HD */
      {154944, 204800,   21632, 1562496,  65536, BXVD_P_VideoAtomIndex_eB, 5}, /* VP8 SD */
      {154944, 102400,    5888,  312448,  16384, BXVD_P_VideoAtomIndex_eC, 5}, /* VP8 CIF */
      {154944, 102400,    1792,   54656,  16384, BXVD_P_VideoAtomIndex_eD, 2}  /* VP8 QCIF */
   }
};

static const BXVD_P_FWMemConfig_V2 sChannelStillFWMemCfg[BXVD_VideoProtocol_eMax][BXVD_DecodeResolution_eSD+1] =
{  /* Context, InnerLoop WL, DirectMode, Cabac bin,  Cabac WL,  Vid Blk (index),  Blk Cnt */

   /* BXVD_VideoProtocol_eAVC */
   {
      { 261440, 131072, 0, 921600, 8192, BXVD_P_VideoAtomIndex_eA, 1}, /* AVC HD Still */
      { 261440,  32768, 0, 409600, 8192, BXVD_P_VideoAtomIndex_eB, 1}  /* AVC SD Still */
   },
   
   /* BXVD_VideoProtocol_eMPEG2 */
   {
      { 154944, 1024, 0, 921600, 8192, BXVD_P_VideoAtomIndex_eA, 1}, /* MPEG2 HD Still */
      { 154944, 1024, 0, 204800, 8192, BXVD_P_VideoAtomIndex_eB, 1}  /* MPEG2 SD Still */
   },

   /* BXVD_VideoProtocol_eVC1 */
   {
      { 154944, 8192, 0, 307200, 8192, BXVD_P_VideoAtomIndex_eA, 1}, /* VC1 HD Still */
      { 154944, 8192, 0, 204800, 8192, BXVD_P_VideoAtomIndex_eB, 1}  /* VC1 SD Still */
   },

   /* BXVD_VideoProtocol_eMPEG4 */
   {
      { 154944, 1024, 0, 307200, 8192, BXVD_P_VideoAtomIndex_eA, 1}, /* MPEG4/DivX HD Still */
      { 154944, 1024, 0, 204800, 8192, BXVD_P_VideoAtomIndex_eB, 1}  /* MPEG4/DivX SD Still */
   },

   /* BXVD_VideoProtocol_eAVS */
   {
      { 154944, 1024, 0, 307200, 8192, BXVD_P_VideoAtomIndex_eA, 1}, /* AVS HD Still */
      { 154944, 1024, 0, 204800, 8192, BXVD_P_VideoAtomIndex_eB, 1}  /* AVS SD Still */
   },

   /* BXVD_VideoProtocol_eVP8 */
   {
      { 154944, 131072, 1109768, 1572864, 131072, BXVD_P_VideoAtomIndex_eA, 1}, /* VP8 HD Still */
      { 154944,  32768, 1109768,  614400,  65536, BXVD_P_VideoAtomIndex_eB, 1}  /* VP8 SD Still */
   },
};


static const BXVD_P_FWMemConfig_V2 sChannelFWMemCfg_BluRay[2] =
{ /* Context, InnerLoop WL, DirectMode, Cabac bin,  Cabac WL,  Vid Blk (index),  Blk Cnt */

   /* BXVD_VideoProtocol_eAVC for BluRay*/
   {261440, 786432, 1638400, 4194304, 131072, BXVD_P_VideoAtomIndex_eA, 7}, /* AVC HD */
   {261440, 524288,  388800, 2084864,  65536, BXVD_P_VideoAtomIndex_eE, 3}  /* AVC SD */
};

static const BXVD_P_FWMemConfig_V2 sChannelFWMemCfg_AVC41[1] =
{ 
   /* BXVD_VideoProtocol_eAVC 4.1 */
   {261440, 3145728, 1638400, 13107200, 131072, BXVD_P_VideoAtomIndex_eA, 7} /* AVC 4.1 HD */
};

static const BXVD_P_FWMemConfig_V2 sChannelFWMemCfg_AVC51[1] =
{ 
   /* BXVD_VideoProtocol_eAVC 5.1 */
   {261440, 3145728, 5570560, 13107200, 131072, BXVD_P_VideoAtomIndex_eF, 9} /* AVC 5.1 HD */
};

/* Support AVC streams that require more direct memory to be decoded. The streams are 
   non compliant, however other STBs in the world decode them, so our customer would
   like this chip to decode the streams as well */

static const BXVD_P_FWMemConfig_V2 sChannelFWMemCfg_bExcessDir[2] =
{ 
   /* Context, IL WL, DirectMode, Cabac bin,  Cabac WL,  Vid Blk (index),  Blk Cnt */

   {261440, 1572864, (1638400*2), 5210112, 131072, BXVD_P_VideoAtomIndex_eA, 7}, /* AVC HD */
   {261440,  786432,  (388800*2), 2084864,  65536, BXVD_P_VideoAtomIndex_eE, 3}, /* AVC SD */
};

static const BXVD_P_FWMemConfig_V2 sChannelFWMemCfg_MVC[1] =
{ 
   /* BXVD_VideoProtocol_eMVC 4.1 */
   {522880, 1572864, 3276800, 10420224, 131072, BXVD_P_VideoAtomIndex_eF, 7} /* MVC 4.1 HD */
};


static const BXVD_P_FWMemConfig_SVC sChannelFWMemCfg_SVC[3] =
{
   /* Context, IL WL,  DirectMode,     Inter Video,          Inter MV,  Cabac bin,  Cabac WL,  Vid Blk (index),      Blk Cnt */
   {522880,   3145728,  1638400,   BXVD_P_VideoAtomIndex_eI,  2219520,   9961472,    131072,   BXVD_P_VideoAtomIndex_eA, 7}, /* Interlaced */
   {522880,   3145728,  1638400,   BXVD_P_VideoAtomIndex_eG,  2219520,   9961472,    131072,   BXVD_P_VideoAtomIndex_eA, 7}, /* Progressive */
   {522880,   3145728,  1638400,   BXVD_P_VideoAtomIndex_eH,  2219520,   9961472,    131072,   BXVD_P_VideoAtomIndex_eA, 7}, /* 3D */
};

static const BXVD_P_FWMemConfig_V2 sChannelFWMemCfg_HEVC[1] =
{ 
   /* BXVD_VideoProtocol_eHEVC */
   {(420000+200000) /*307200*/, 3145728, 3276800, 13107200, 262144, BXVD_P_VideoAtomIndex_eZ, 7} /* HEVC */
};

BDBG_MODULE(BXVD_PRIV);

void BXVD_P_ValidateHeaps(BXVD_Handle        hXvd,
                          BXVD_P_MemCfgMode  eMemCfgMode)
{
   /* UMA memory configuration */
   if (eMemCfgMode == BXVD_P_MemCfgMode_eUMA)
   {
      if (hXvd->hSystemHeap != hXvd->hPictureHeap)
      {
         BXVD_DBG_MSG(hXvd, ("UMA Mode, Picture buffers using separate heap"));
      }
      else
      {
         BXVD_DBG_MSG(hXvd, ("UMA Mode, Picture buffers using system heap as expected."));
      }
   } 

   /* NON-UMA memory configuration */
   else if (eMemCfgMode == BXVD_P_MemCfgMode_eNONUMA)
   {
      if (hXvd->hSystemHeap == hXvd->hPictureHeap)
      {
         BXVD_DBG_WRN(hXvd, ("****NON UMA MODE Error: Picture buffers should not use system heap"));
      }
      else
      {
         BXVD_DBG_MSG(hXvd, ("NON-UMA Mode, Picture buffers using separate heap as expected."));
      }
   }

   /* UNKNOWN memory configuration */
   else
   {
      if (hXvd->hSystemHeap == hXvd->hPictureHeap)
      {
         BXVD_DBG_MSG(hXvd, ("UMA Mode, Picture buffers using system heap."));
      }
      else
      {
         BXVD_DBG_MSG(hXvd, ("UNKNOWN UMA/NON UMA Mode, Picture buffers using separate heap."));
      }
   }

   /* If private picture heap is specified, but the specified picture heap size is 0, output a warning. 
    * This use of the API is most likely not intentional. Non zero general heap size indicates custom
    * memory decode mode. Old memory API method (compatibility mode) will have a zero picture heap size. */

   if ((hXvd->hSystemHeap != hXvd->hPictureHeap) && 
       (hXvd->stSettings.stFWMemConfig.uiPictureHeapSize == 0) &&
       (hXvd->stSettings.stFWMemConfig.uiGeneralHeapSize != 0))
   {
      BXVD_DBG_WRN(hXvd, ("Separate picture heap specified with picture memory size of 0. Pictures are NOT expected to be displayed."));
   }
}


bool BXVD_P_IsDecodeProtocolSupported(BXVD_Handle               hXvd,
                                      BAVC_VideoCompressionStd  eVideoCmprStd )
{
   bool rc = false;

   /* Platform may be capable to decode RV9, but it may have been disabled by OTP */
   if (eVideoCmprStd == BAVC_VideoCompressionStd_eRV9) 
   {
      if (hXvd->bRV9Capable)
      {
         rc = true;
      }
   }

   else if (eVideoCmprStd == BAVC_VideoCompressionStd_eSVC) 
   {
      if (hXvd->bSVCCapable)
      {
         rc = true;
      }
   }

   else if (eVideoCmprStd >= BAVC_VideoCompressionStd_eMPEG4Part2)
   {
      if (BXVD_P_CREATE_PROTOCOLS_MASK(eVideoCmprStd) & hXvd->uiSupportedProtocolsMask)
      {
         rc = true;
      }
   }

   /* All others are supported on all platforms */
   else if (eVideoCmprStd < BAVC_VideoCompressionStd_eMPEG4Part2)
   {
      rc = true;
   }

   return rc;
}

BERR_Code BXVD_P_MapToAVDProtocolEnum( BXVD_Handle               hXvd,
                                       BAVC_VideoCompressionStd  eVideoCmprStd,
                                       BXVD_P_PPB_Protocol *     peProtocol )
{
   BERR_Code rc = BERR_SUCCESS;

   /* SW7425-3177: The enums used to define the video protocol have diverged between AVD 
    * and bavc.h, i.e. BXVD_P_PPB_Protocol is no longer in sync with BAVC_VideoCompressionStd.
    * In addition to verifying that a protocol is supported, map eVideoCmprStd
    * to a BXVD_P_PPB_Protocol value.
    */
   switch( eVideoCmprStd )
   {
      case BAVC_VideoCompressionStd_eH264:            
         *peProtocol = BXVD_P_PPB_Protocol_eH264; 
         break; 
#if BXVD_P_HVD_PRESENT 
      case BAVC_VideoCompressionStd_eH265:            
         *peProtocol = BXVD_P_PPB_Protocol_eH265; 
         break; 
#endif
      case BAVC_VideoCompressionStd_eMPEG2:           
         *peProtocol = BXVD_P_PPB_Protocol_eMPEG2; 
         break; 
      case BAVC_VideoCompressionStd_eH261:            
         *peProtocol = BXVD_P_PPB_Protocol_eH261; 
         break; 
      case BAVC_VideoCompressionStd_eH263:            
         *peProtocol = BXVD_P_PPB_Protocol_eH263; 
         break; 
      case BAVC_VideoCompressionStd_eVC1:             
         *peProtocol = BXVD_P_PPB_Protocol_eVC1; 
         break; 
      case BAVC_VideoCompressionStd_eMPEG1:           
         *peProtocol = BXVD_P_PPB_Protocol_eMPEG1; 
         break; 
      case BAVC_VideoCompressionStd_eMPEG2DTV:        
         *peProtocol = BXVD_P_PPB_Protocol_eMPEG2DTV; 
         break; 
      case BAVC_VideoCompressionStd_eVC1SimpleMain:   
         *peProtocol = BXVD_P_PPB_Protocol_eVC1SimpleMain; 
         break; 
      case BAVC_VideoCompressionStd_eMPEG4Part2:      
         *peProtocol = BXVD_P_PPB_Protocol_eMPEG4Part2; 
         break; 
      case BAVC_VideoCompressionStd_eAVS:             
         *peProtocol = BXVD_P_PPB_Protocol_eAVS; 
         break; 
      case BAVC_VideoCompressionStd_eMPEG2_DSS_PES:   
         *peProtocol = BXVD_P_PPB_Protocol_eMPEG2_DSS_PES; 
         break; 
      case BAVC_VideoCompressionStd_eSVC:             
         *peProtocol = BXVD_P_PPB_Protocol_eSVC; 
         break; 
      case BAVC_VideoCompressionStd_eSVC_BL:          
         *peProtocol = BXVD_P_PPB_Protocol_eSVC_BL; 
         break; 
      case BAVC_VideoCompressionStd_eMVC:             
         *peProtocol = BXVD_P_PPB_Protocol_eMVC; 
         break; 
      case BAVC_VideoCompressionStd_eVP6:             
         *peProtocol = BXVD_P_PPB_Protocol_eVP6; 
         break; 
      case BAVC_VideoCompressionStd_eVP7:             
         *peProtocol = BXVD_P_PPB_Protocol_eVP7; 
         break; 
      case BAVC_VideoCompressionStd_eVP8:             
         *peProtocol = BXVD_P_PPB_Protocol_eVP8; 
         break; 
      case BAVC_VideoCompressionStd_eRV9:             
         *peProtocol = BXVD_P_PPB_Protocol_eRV9; 
         break; 
      case BAVC_VideoCompressionStd_eSPARK:           
         *peProtocol = BXVD_P_PPB_Protocol_eSPARK; 
         break; 

      /* Since BXVD_P_IsDecodeProtocolSupported was called prior to 
       * this routine this is unlikely to happen.
       */
      case BAVC_VideoCompressionStd_eMOTION_JPEG:
      default:
         BXVD_DBG_ERR(hXvd, ("BXVD_P_IsDecodeProtocolSupported:: unsupported video protocol: 0x%x", eVideoCmprStd));
         *peProtocol = BXVD_P_PPB_Protocol_eH264; /* set the protocol to something. */
         rc = BERR_INVALID_PARAMETER;
         break;
   }

   return BERR_TRACE(rc);
}

BERR_Code BXVD_P_SetupFWSubHeap(BXVD_Handle hXvd)
{
   BERR_Code rc = BERR_SUCCESS;

   hXvd->SubGenMem = NULL;
   hXvd->SubSecureMem = NULL;
   hXvd->SubPicMem = NULL;

   if (hXvd->uiFWGenMemSize != 0)
   {
      BXVD_DBG_MSG(hXvd, ("Creating BXVD_Memory sub-heaps: Gen VA: %08x, PA: %08x, Size:%08x", 
                               hXvd->uiFWGenMemVirtAddr, hXvd->uiFWGenMemPhyAddr, hXvd->uiFWGenMemSize));

      rc = BERR_TRACE(BXVD_P_Memory_Open(hXvd, &hXvd->SubGenMem, (void *) hXvd->uiFWGenMemVirtAddr, 
                                         hXvd->uiFWGenMemPhyAddr,  hXvd->uiFWGenMemSize,
                                         BXVD_P_Memory_Protection_eDontProtect));
      if (rc != BERR_SUCCESS)
      {
         return BERR_TRACE(rc);
      }
   }

   if (hXvd->uiFWCabacMemSize != 0)
   {
      BXVD_DBG_MSG(hXvd, ("Creating BXVD_Memory sub-heaps: Secure VA: %08x, PA: %08x, Size:%08x",
                               hXvd->uiFWCabacMemVirtAddr, hXvd->uiFWCabacMemPhyAddr, hXvd->uiFWCabacMemSize));

      rc = BERR_TRACE(BXVD_P_Memory_Open(hXvd, &hXvd->SubSecureMem, (void *) hXvd->uiFWCabacMemVirtAddr, 
                                         hXvd->uiFWCabacMemPhyAddr,  hXvd->uiFWCabacMemSize, 
                                         BXVD_P_Memory_Protection_eDontProtect));
      if (rc != BERR_SUCCESS)
      {
         return BERR_TRACE(rc);
      }
   }

   if (hXvd->uiFWPicMemSize != 0)
   {
      BXVD_DBG_MSG(hXvd, ("Creating BXVD_Memory sub-heaps: Pic VA: %08x, PA: %08x, Size:%08x",
                               hXvd->uiFWPicMemVirtAddr, hXvd->uiFWPicMemPhyAddr, hXvd->uiFWPicMemSize));

      rc = BERR_TRACE(BXVD_P_Memory_Open(hXvd, &hXvd->SubPicMem, (void *) hXvd->uiFWPicMemVirtAddr, 
                                         hXvd->uiFWPicMemPhyAddr,  hXvd->uiFWPicMemSize,
                                         BXVD_P_Memory_Protection_eDontProtect));
      if (rc != BERR_SUCCESS)
      {
         return BERR_TRACE(rc);
      }
   }

   BXVD_DBG_MSG(hXvd, ("BXVD_Memory sub-heaps created successfully"));
   return BERR_TRACE(rc);

}

BERR_Code BXVD_P_TeardownFWSubHeap(BXVD_Handle hXvd)
{
   BERR_Code rc = BERR_SUCCESS;

   if (hXvd->SubGenMem != NULL)
   {
      rc = BERR_TRACE(BXVD_P_Memory_Close(hXvd->SubGenMem));
      if (rc != BERR_SUCCESS)
      {
         return BERR_TRACE(rc);
      }
      hXvd->SubGenMem = NULL;
   }

   if (hXvd->SubSecureMem != NULL)
   {
      rc = BERR_TRACE(BXVD_P_Memory_Close(hXvd->SubSecureMem));
      if (rc != BERR_SUCCESS)
      {
         return BERR_TRACE(rc);
      }
      hXvd->SubSecureMem = NULL;
   }

   if (hXvd->SubPicMem != NULL)
   {
      rc = BERR_TRACE(BXVD_P_Memory_Close(hXvd->SubPicMem));
      if (rc != BERR_SUCCESS)
      {
         return BERR_TRACE(rc);
      }
      hXvd->SubPicMem = NULL;
   }
  
   return BERR_TRACE(rc);

}


BERR_Code BXVD_P_GetDecodeFWMemSize(BXVD_Handle hXvd,
                                    BXVD_DecodeResolution eDecodeResolution,
                                    BAVC_VideoCompressionStd aeVideoCmprStd[],
                                    uint32_t  uiVideoCmprCount,
                                    const BXVD_ChannelSettings *pChSettings,
                                    BXVD_P_DecodeFWMemSize     *pstDecodeFWMemSize)
{

   uint32_t i;
   uint32_t genMemReq = 0;
   uint32_t cabacMemReq = 0;
   uint32_t vidBlkIndex = 0;   
   uint32_t vidBlkSizeLookedUp = 0;   
   uint32_t vidBlkCountLookedUp = 0;   
   uint32_t vidBlkSizeReq = 0;   
   uint32_t vidBlkCountReq = 0;
   uint32_t cabacWorklistMemReq = 0;
   uint32_t directModeMemReq = 0;
   uint32_t innerLoopWorklistMemReq = 0;

   BXVD_VideoProtocol eVideoProtocol_TableIndex;

   BXVD_P_FWMemConfig_V2 *pChannelFWMemCfg; 
   BXVD_P_FWMemConfig_SVC *pSVCChannelFWMemCfg;


   BDBG_ASSERT(hXvd->uiDecode_StripeWidth < BXVD_P_STRIPE_WIDTH_NUM);
   BDBG_ASSERT(hXvd->uiDecode_StripeMultiple < BXVD_P_STRIPE_MULTIPLE_NUM);

   /* Set SVD BL optional sizes to zero */
   pstDecodeFWMemSize->uiFWInterLayerPicSize = 0;
   pstDecodeFWMemSize->uiFWInterLayerMVSize = 0;

   for (i = 0; i < uiVideoCmprCount; i++)
   {
      switch (aeVideoCmprStd[i])
      {
         case BAVC_VideoCompressionStd_eH264:
         case BAVC_VideoCompressionStd_eRV9: 
            BXVD_DBG_MSG(hXvd, ("Video Protocol Memory Config: AVC"));
            eVideoProtocol_TableIndex = BXVD_VideoProtocol_eAVC;
            break;

         case BAVC_VideoCompressionStd_eMPEG2:
         case BAVC_VideoCompressionStd_eH261:
         case BAVC_VideoCompressionStd_eH263:
         case BAVC_VideoCompressionStd_eMPEG1:
         case BAVC_VideoCompressionStd_eMPEG2DTV:
         case BAVC_VideoCompressionStd_eMPEG2_DSS_PES:
         case BAVC_VideoCompressionStd_eSPARK:
            BXVD_DBG_MSG(hXvd, ("Video Protocol Memory Config: MPEG2"));
            eVideoProtocol_TableIndex = BXVD_VideoProtocol_eMPEG2;
            break;
            
         case BAVC_VideoCompressionStd_eVC1:
         case BAVC_VideoCompressionStd_eVC1SimpleMain:
            BXVD_DBG_MSG(hXvd, ("Video Protocol Memory Config: VC1"));
            eVideoProtocol_TableIndex = BXVD_VideoProtocol_eVC1;
            break;
            
         case BAVC_VideoCompressionStd_eMPEG4Part2:
            BXVD_DBG_MSG(hXvd, ("Video Protocol Memory Config: MPEG4"));
            eVideoProtocol_TableIndex = BXVD_VideoProtocol_eMPEG4;
            break;
            
         case BAVC_VideoCompressionStd_eAVS:
            BXVD_DBG_MSG(hXvd, ("Video Protocol Memory Config: AVS"));
            eVideoProtocol_TableIndex = BXVD_VideoProtocol_eAVS;
            break;  

         case BAVC_VideoCompressionStd_eMVC:
            BXVD_DBG_MSG(hXvd, ("Video Protocol Memory Config: MVC"));
            eVideoProtocol_TableIndex = BXVD_VideoProtocol_eMVC;
            break;  

         case BAVC_VideoCompressionStd_eSVC:
            BXVD_DBG_MSG(hXvd, ("Video Protocol Memory Config: SVC"));
            eVideoProtocol_TableIndex = BXVD_VideoProtocol_eSVC;
            break;  

         case BAVC_VideoCompressionStd_eVP6:
         case BAVC_VideoCompressionStd_eVP7:
         case BAVC_VideoCompressionStd_eVP8:
            BXVD_DBG_MSG(hXvd, ("Video Protocol Memory Config: VP8"));
            eVideoProtocol_TableIndex = BXVD_VideoProtocol_eVP8;
            break;  

#if BXVD_P_HVD_PRESENT 
         case BAVC_VideoCompressionStd_eH265:
            BXVD_DBG_MSG(hXvd, ("Video Protocol Memory Config: HEVC"));
            eVideoProtocol_TableIndex = BXVD_VideoProtocol_eHEVC;
            break;  
#endif

         default:
            BXVD_DBG_WRN(hXvd, ("Video Protocol: Unknown - Defaulting to AVC Memory Config!"));
            eVideoProtocol_TableIndex = BXVD_VideoProtocol_eAVC;
            break;              
      }
      
      switch (eDecodeResolution)
      {
         case BXVD_DecodeResolution_eHD:
            BXVD_DBG_MSG(hXvd, ("Video Resolution: HD"));
            break;

         case BXVD_DecodeResolution_eSD:
            BXVD_DBG_MSG(hXvd, ("Video Resolution: SD"));
            break;            
            
         case BXVD_DecodeResolution_eCIF:
            BXVD_DBG_MSG(hXvd, ("Video Resolution: CIF"));
            break;            
            
         case BXVD_DecodeResolution_eQCIF:
            BXVD_DBG_MSG(hXvd, ("Video Resolution: QCIF"));
            break;                
            
         default:
            BXVD_DBG_WRN(hXvd, ("Video Resolution: Unknown - Defaulting to HD!"));
            eDecodeResolution = BXVD_DecodeResolution_eHD;
            break;
      }

      /* Default SVC memory configuration is entry [0]. */
      pSVCChannelFWMemCfg = (BXVD_P_FWMemConfig_SVC *)&(sChannelFWMemCfg_SVC[0]);

      /* Default non SVC memory configuration entry */
      pChannelFWMemCfg = (BXVD_P_FWMemConfig_V2 *)&(sChannelFWMemCfg[eVideoProtocol_TableIndex][eDecodeResolution]);

      /* AVC 4.1 enabled */
      if (pChSettings->bAVC41Enable && 
          (eDecodeResolution == BXVD_DecodeResolution_eHD) && 
          (eVideoProtocol_TableIndex== BXVD_VideoProtocol_eAVC))
      {
         pChannelFWMemCfg = (BXVD_P_FWMemConfig_V2 *)&sChannelFWMemCfg_AVC41;

         vidBlkIndex = pChannelFWMemCfg->video_block_size_index;
         vidBlkSizeLookedUp = sVideoAtomSize[hXvd->uiDecode_StripeWidth][hXvd->uiDecode_StripeMultiple][vidBlkIndex];
         vidBlkCountLookedUp = pChannelFWMemCfg->video_block_count;
      }

      /* AVC 5.1 enabled */
      else if (pChSettings->bAVC51Enable && 
               (eDecodeResolution == BXVD_DecodeResolution_eHD) && 
               (eVideoProtocol_TableIndex == BXVD_VideoProtocol_eAVC))
      {
         pChannelFWMemCfg = (BXVD_P_FWMemConfig_V2 *)&sChannelFWMemCfg_AVC51;

         vidBlkIndex = pChannelFWMemCfg->video_block_size_index;
         vidBlkSizeLookedUp = sVideoAtomSize[hXvd->uiDecode_StripeWidth][hXvd->uiDecode_StripeMultiple][vidBlkIndex];
         vidBlkCountLookedUp = pChannelFWMemCfg->video_block_count;
      }

      /* BluRay enabled AVC */
      else if (pChSettings->bBluRayEnable && (eVideoProtocol_TableIndex == BXVD_VideoProtocol_eAVC) &&
               ((eDecodeResolution == BXVD_DecodeResolution_eHD) ||
                (eDecodeResolution == BXVD_DecodeResolution_eSD)))
      {
         pChannelFWMemCfg = (BXVD_P_FWMemConfig_V2 *)&(sChannelFWMemCfg_BluRay[eDecodeResolution]);

         vidBlkIndex = pChannelFWMemCfg->video_block_size_index;
         vidBlkSizeLookedUp = sVideoAtomSize[hXvd->uiDecode_StripeWidth][hXvd->uiDecode_StripeMultiple][vidBlkIndex];
         vidBlkCountLookedUp = pChannelFWMemCfg->video_block_count;
      }

      /* Excess direct memory mode enabled AVC */
      else if (pChSettings->bExcessDirModeEnable && (eVideoProtocol_TableIndex == BXVD_VideoProtocol_eAVC) &&
               ((eDecodeResolution == BXVD_DecodeResolution_eHD) ||
                (eDecodeResolution == BXVD_DecodeResolution_eSD)))
      {
         pChannelFWMemCfg = (BXVD_P_FWMemConfig_V2 *)&(sChannelFWMemCfg_bExcessDir[eDecodeResolution]);

         vidBlkIndex = pChannelFWMemCfg->video_block_size_index;
         vidBlkSizeLookedUp = sVideoAtomSize[hXvd->uiDecode_StripeWidth][hXvd->uiDecode_StripeMultiple][vidBlkIndex];
         vidBlkCountLookedUp = pChannelFWMemCfg->video_block_count;
      }

      /* MVC */
      else if (eVideoProtocol_TableIndex == BXVD_VideoProtocol_eMVC) 
      {
         pChannelFWMemCfg = (BXVD_P_FWMemConfig_V2 *)&(sChannelFWMemCfg_MVC);

         vidBlkIndex = pChannelFWMemCfg->video_block_size_index;
         vidBlkSizeLookedUp = sVideoAtomSize[hXvd->uiDecode_StripeWidth][hXvd->uiDecode_StripeMultiple][vidBlkIndex];
         vidBlkCountLookedUp = pChannelFWMemCfg->video_block_count;
      }

      /* HEVC */
      else if (eVideoProtocol_TableIndex == BXVD_VideoProtocol_eHEVC) 
      {
         pChannelFWMemCfg = (BXVD_P_FWMemConfig_V2 *)&(sChannelFWMemCfg_HEVC);

         vidBlkIndex = pChannelFWMemCfg->video_block_size_index;
         vidBlkSizeLookedUp = sVideoAtomSize[hXvd->uiDecode_StripeWidth][hXvd->uiDecode_StripeMultiple][vidBlkIndex];
         vidBlkCountLookedUp = pChannelFWMemCfg->video_block_count;
      }

      /* SVC */
      else if ((eVideoProtocol_TableIndex == BXVD_VideoProtocol_eSVC) && (hXvd->bSVCCapable == true))
      {
         if (pChSettings->bSVC3DModeEnable)
         {
            pSVCChannelFWMemCfg = (BXVD_P_FWMemConfig_SVC *)&(sChannelFWMemCfg_SVC[2]);
         }
         else if (pChSettings->bSVCProgressiveOnly == true)
         {
            pSVCChannelFWMemCfg = (BXVD_P_FWMemConfig_SVC *)&(sChannelFWMemCfg_SVC[1]);
         }

         vidBlkIndex = pSVCChannelFWMemCfg->video_block_size_index;
         vidBlkSizeLookedUp = sVideoAtomSize[hXvd->uiDecode_StripeWidth][hXvd->uiDecode_StripeMultiple][vidBlkIndex];
         vidBlkCountLookedUp = pSVCChannelFWMemCfg->video_block_count;

#if BXVD_P_ILS_BUFFERS_INTERNAL
         pstDecodeFWMemSize->uiFWInterLayerPicSize = 0;
         pstDecodeFWMemSize->uiFWInterLayerMVSize = 0; 
#else
         vidBlkIndex = pSVCChannelFWMemCfg->inter_layer_video_size_index;
         
         if (hXvd->stSettings.bInterLayerBandwidthOptimized)
         {
            /* Multiply by 1.5 for Bandwidth optimization */
            pstDecodeFWMemSize->uiFWInterLayerPicSize = 
               ((sVideoAtomSize[hXvd->uiDecode_StripeWidth][hXvd->uiDecode_StripeMultiple][vidBlkIndex] * 15) / 10);

            pstDecodeFWMemSize->uiFWInterLayerMVSize = ( (pSVCChannelFWMemCfg->inter_layer_mv_size * 15 ) / 10);
         }
         else
         {
            pstDecodeFWMemSize->uiFWInterLayerPicSize = 
               sVideoAtomSize[hXvd->uiDecode_StripeWidth][hXvd->uiDecode_StripeMultiple][vidBlkIndex];

            pstDecodeFWMemSize->uiFWInterLayerMVSize =  pSVCChannelFWMemCfg->inter_layer_mv_size;
         }
#endif
      }

      /* Normal memory configuration */
      else
      {
         pChannelFWMemCfg = (BXVD_P_FWMemConfig_V2 *)&(sChannelFWMemCfg[eVideoProtocol_TableIndex][eDecodeResolution]);

         vidBlkIndex = sChannelFWMemCfg[eVideoProtocol_TableIndex][eDecodeResolution].video_block_size_index;

         /* Select Atom size "AT" instead of size "A" if in 1920 Portrait mode for certain decode protocols. */
         if ((pChSettings->b1920PortraitModeEnable == true) && 
             (pChSettings->bBluRayEnable == false) && (vidBlkIndex == BXVD_P_VideoAtomIndex_eA) && 
             (eVideoProtocol_TableIndex != BXVD_VideoProtocol_eSVC))
            {
               vidBlkIndex = BXVD_P_VideoAtomIndex_eAT;
            }

         vidBlkSizeLookedUp = sVideoAtomSize[hXvd->uiDecode_StripeWidth][hXvd->uiDecode_StripeMultiple][vidBlkIndex];
         vidBlkCountLookedUp = sChannelFWMemCfg[eVideoProtocol_TableIndex][eDecodeResolution].video_block_count;
      }

      if((eVideoProtocol_TableIndex == BXVD_VideoProtocol_eSVC) && (hXvd->bSVCCapable == true))
      {
         /* Use SVC memory config entry */
         if (pSVCChannelFWMemCfg->context_memory_size > genMemReq)
         {
            genMemReq = pSVCChannelFWMemCfg->context_memory_size;
         }
      
         if (pSVCChannelFWMemCfg->cabac_bin_size > cabacMemReq)
         {
            cabacMemReq = pSVCChannelFWMemCfg->cabac_bin_size;
         }

         if (pSVCChannelFWMemCfg->cabac_wl_size > cabacWorklistMemReq)
         {
            cabacWorklistMemReq = pSVCChannelFWMemCfg->cabac_wl_size;
         }

         if (pSVCChannelFWMemCfg->direct_mode_size > directModeMemReq)
         {
            directModeMemReq = pSVCChannelFWMemCfg->direct_mode_size;
         }

         if (pSVCChannelFWMemCfg->inner_loop_wl_size > innerLoopWorklistMemReq)
         {
            innerLoopWorklistMemReq = pSVCChannelFWMemCfg->inner_loop_wl_size;
         }
      }
      else
      {
         /* Use NON SVC memory config entry */
         if (pChannelFWMemCfg->general_memory_size > genMemReq)
         {
            genMemReq = pChannelFWMemCfg->general_memory_size;
         }
      
         if (pChannelFWMemCfg->cabac_bin_size > cabacMemReq)
         {
            cabacMemReq = pChannelFWMemCfg->cabac_bin_size;
         }

         if (pChannelFWMemCfg->cabac_wl_size > cabacWorklistMemReq)
         {
            cabacWorklistMemReq = pChannelFWMemCfg->cabac_wl_size;
         }

         if (pChannelFWMemCfg->direct_mode_size > directModeMemReq)
         {
            directModeMemReq = pChannelFWMemCfg->direct_mode_size;
         }

         if (pChannelFWMemCfg->inner_loop_wl_size > innerLoopWorklistMemReq)
         {
            innerLoopWorklistMemReq = pChannelFWMemCfg->inner_loop_wl_size;
         }

      }
      /* Check bounds of stripe multiple */
      if (hXvd->uiDecode_StripeMultiple >= BXVD_P_STRIPE_MULTIPLE_NUM)
      {
         BXVD_DBG_ERR(hXvd, ("Unsupported stripe multiple: %d", hXvd->uiDecode_StripeMultiple));
         return BERR_TRACE(BERR_INVALID_PARAMETER);
      }

      /* Handle BTP Mode */
      if (!pChSettings->bMPEG2BTPEnable && 
          ((eVideoProtocol_TableIndex == BXVD_VideoProtocol_eMPEG2) &&
           ((eDecodeResolution == BXVD_DecodeResolution_eHD) ||
            (eDecodeResolution == BXVD_DecodeResolution_eSD))))
      {
         /* Reduce block count by 1 if we do not want BTP capability
          * in MPEG2 HD/SD */
         vidBlkCountLookedUp -= 1;
      }

      if ((vidBlkSizeLookedUp * vidBlkCountLookedUp) > 
          (vidBlkSizeReq * vidBlkCountReq))
      {         
         vidBlkSizeReq = vidBlkSizeLookedUp;
         vidBlkCountReq = vidBlkCountLookedUp;
      }
   }

#if BXVD_P_PPB_EXTENDED
   /* Context memory needs to be increased by 32k when using extended PPB */ 
   genMemReq += (32 * 1024);
#endif

   pstDecodeFWMemSize->uiFWContextSize = genMemReq;
   pstDecodeFWMemSize->uiFWCabacSize = cabacMemReq;
   pstDecodeFWMemSize->uiFWPicBlockSize = vidBlkSizeReq;
   pstDecodeFWMemSize->uiFWPicBlockCount = vidBlkCountReq;
   pstDecodeFWMemSize->uiFWCabacWorklistSize = cabacWorklistMemReq;

   pstDecodeFWMemSize->uiFWDirectModeSize = directModeMemReq;
   pstDecodeFWMemSize->uiFWInnerLoopWorklistSize = innerLoopWorklistMemReq;

   BXVD_DBG_MSG(hXvd, ("ContextSize:0x%x (%d)", genMemReq, genMemReq));
   BXVD_DBG_MSG(hXvd, ("cabacMemReq:0x%x (%d)", cabacMemReq, cabacMemReq));
   BXVD_DBG_MSG(hXvd, ("VidBlockSize:0x%x (%d) , BlkCnt:%d", vidBlkSizeReq, vidBlkSizeReq, vidBlkCountReq));
   BXVD_DBG_MSG(hXvd, ("cabacWorklistMemReq:0x%x (%d)", cabacWorklistMemReq, cabacWorklistMemReq));
   BXVD_DBG_MSG(hXvd, ("directModeMemSize: 0x%x (%d)", directModeMemReq, directModeMemReq));
   BXVD_DBG_MSG(hXvd, ("innerLoopWorklistSize: 0x%x (%d)", innerLoopWorklistMemReq, innerLoopWorklistMemReq));
   return BERR_TRACE(BERR_SUCCESS);
}

BERR_Code BXVD_P_GetStillDecodeFWMemSize(BXVD_Handle hXvd,
                                         BXVD_DecodeResolution eDecodeResolution,
                                         BAVC_VideoCompressionStd aeVideoCmprStd[], 
                                         uint32_t  uiVideoCmprCount,
                                         BXVD_P_DecodeFWMemSize       *pstDecodeFWMemSize)
{
   uint32_t i;
   uint32_t genMemReq = 0;
   uint32_t cabacMemSizeLookedUp = 0;
   uint32_t cabacMemReq = 0;
   uint32_t vidBlkSizeLookedUp = 0;
   uint32_t vidBlkSizeReq = 0;
   uint32_t vidBlkCountReq = 0;
   uint32_t cabacWorklistMemSizeLookedUp = 0;
   uint32_t cabacWorklistMemReq = 0;
   uint32_t directModeMemReq = 0;
   uint32_t innerLoopWorklistMemReq = 0;

   BXVD_VideoProtocol eVideoProtocol;

   BDBG_ASSERT(hXvd->uiDecode_StripeWidth < BXVD_P_STRIPE_WIDTH_NUM);
   BDBG_ASSERT(hXvd->uiDecode_StripeMultiple < BXVD_P_STRIPE_MULTIPLE_NUM);             

   if ((eDecodeResolution != BXVD_DecodeResolution_eHD) &&
       (eDecodeResolution != BXVD_DecodeResolution_eSD))
   {
      /* coverity[dead_error_line: FALSE] */
      BXVD_DBG_ERR(hXvd, ("Unsupported Still Picture Decode Resolution: %d", eDecodeResolution));
      return BERR_TRACE(BERR_INVALID_PARAMETER);
   }

   /* Set SVD BL optional sizes to zero, these are not used in still decode. */
   pstDecodeFWMemSize->uiFWInterLayerPicSize = 0;
   pstDecodeFWMemSize->uiFWInterLayerMVSize = 0;

   for (i = 0; i < uiVideoCmprCount; i++)
   {
      switch (aeVideoCmprStd[i])
      {
         case BAVC_VideoCompressionStd_eH264:
            BXVD_DBG_MSG(hXvd, ("Still Protocol: AVC"));
            eVideoProtocol = BXVD_VideoProtocol_eAVC;
            break;

         case BAVC_VideoCompressionStd_eMPEG2:
         case BAVC_VideoCompressionStd_eH261:
         case BAVC_VideoCompressionStd_eH263:
         case BAVC_VideoCompressionStd_eMPEG1:
         case BAVC_VideoCompressionStd_eMPEG2DTV:
         case BAVC_VideoCompressionStd_eMPEG2_DSS_PES:
            BXVD_DBG_MSG(hXvd, ("Still Protocol: MPEG2"));
            eVideoProtocol = BXVD_VideoProtocol_eMPEG2;
            break;
            
         case BAVC_VideoCompressionStd_eVC1:
         case BAVC_VideoCompressionStd_eVC1SimpleMain:
            BXVD_DBG_MSG(hXvd, ("Still Protocol: VC1"));
            eVideoProtocol = BXVD_VideoProtocol_eVC1;
            break;
            
         case BAVC_VideoCompressionStd_eMPEG4Part2:
            BXVD_DBG_MSG(hXvd, ("Still Protocol: MPEG4"));
            eVideoProtocol = BXVD_VideoProtocol_eMPEG4;
            break;
            
         case BAVC_VideoCompressionStd_eAVS:
            BXVD_DBG_MSG(hXvd, ("Still Protocol: AVS"));
            eVideoProtocol = BXVD_VideoProtocol_eAVS;
            break;  

         case BAVC_VideoCompressionStd_eVP8:
            BXVD_DBG_MSG(hXvd, ("Still Protocol: VP8"));
            eVideoProtocol = BXVD_VideoProtocol_eVP8;
            break;  

         default:
            BXVD_DBG_WRN(hXvd, ("Still Protocol: Unknown - Defaulting to AVC!"));
            eVideoProtocol = BXVD_VideoProtocol_eAVC;
            break;              
      }

      if (eDecodeResolution == BXVD_DecodeResolution_eHD)
      {
         BXVD_DBG_MSG(hXvd, ("Still Resolution: HD"));
      }
      else
      {
         BXVD_DBG_MSG(hXvd, ("Still Resolution: SD"));
      }
      
      if (sChannelStillFWMemCfg[eVideoProtocol][eDecodeResolution].general_memory_size > genMemReq)
      {
         genMemReq = sChannelStillFWMemCfg[eVideoProtocol][eDecodeResolution].general_memory_size;
      }

      if (sChannelStillFWMemCfg[eVideoProtocol][eDecodeResolution].inner_loop_wl_size > innerLoopWorklistMemReq)
      {
         innerLoopWorklistMemReq = sChannelStillFWMemCfg[eVideoProtocol][eDecodeResolution].inner_loop_wl_size;
      }

      cabacMemSizeLookedUp = sChannelStillFWMemCfg[eVideoProtocol][eDecodeResolution].cabac_bin_size;
      cabacWorklistMemSizeLookedUp = sChannelStillFWMemCfg[eVideoProtocol][eDecodeResolution].cabac_wl_size;

      if (sChannelStillFWMemCfg[eVideoProtocol][eDecodeResolution].direct_mode_size > directModeMemReq)
      {
         directModeMemReq = sChannelStillFWMemCfg[eVideoProtocol][eDecodeResolution].direct_mode_size;
      }

      /* Handle Still Picture Compatibility Mode */
      if (hXvd->bStillPictureCompatibilityMode && 
          (eVideoProtocol == BXVD_VideoProtocol_eMPEG2))
      {         
         /* In still picture compatibility mode, to reduce memory
            requirements, we do not allocate a separate cabac.
            Instead, we pass in the CDB as the cabac buffer. */
         cabacMemSizeLookedUp = 0;
      }
      
      if ((cabacMemSizeLookedUp + cabacWorklistMemSizeLookedUp) > 
          (cabacMemReq + cabacWorklistMemReq))
      {
         cabacMemReq = cabacMemSizeLookedUp;
         cabacWorklistMemReq = cabacWorklistMemSizeLookedUp;
      }

      /* Check bounds of stripe multiple and stripe width */
      if (hXvd->uiDecode_StripeMultiple >= BXVD_P_STRIPE_MULTIPLE_NUM)
      {
         BXVD_DBG_ERR(hXvd, ("Unsupported stripe multiple: %d", hXvd->uiDecode_StripeMultiple));
         return BERR_TRACE(BERR_INVALID_PARAMETER);
      }

      if (hXvd->uiDecode_StripeWidth >= BXVD_P_STRIPE_WIDTH_NUM)
      {
         BXVD_DBG_ERR(hXvd, ("Unsupported stripe width: %d", hXvd->uiDecode_StripeWidth));
         return BERR_TRACE(BERR_INVALID_PARAMETER);
      }

      vidBlkSizeLookedUp = sVideoAtomSize[hXvd->uiDecode_StripeWidth][hXvd->uiDecode_StripeMultiple][sChannelStillFWMemCfg[eVideoProtocol][eDecodeResolution].video_block_size_index];
      if ( (vidBlkSizeLookedUp * 
            sChannelStillFWMemCfg[eVideoProtocol][eDecodeResolution].video_block_count) > 
           (vidBlkSizeReq * vidBlkCountReq))
      {
         vidBlkSizeReq = vidBlkSizeLookedUp;
         vidBlkCountReq =  sChannelStillFWMemCfg[eVideoProtocol][eDecodeResolution].video_block_count;
      }
   }

#if BXVD_P_PPB_EXTENDED
   /* Context memory needs to be increased by 32k when using extended PPB */ 
   genMemReq += (32 * 1024);
#endif

   pstDecodeFWMemSize->uiFWContextSize = genMemReq;
   pstDecodeFWMemSize->uiFWCabacSize = cabacMemReq;
   pstDecodeFWMemSize->uiFWPicBlockSize = vidBlkSizeReq;
   pstDecodeFWMemSize->uiFWPicBlockCount = vidBlkCountReq;
   pstDecodeFWMemSize->uiFWCabacWorklistSize = cabacWorklistMemReq;
   pstDecodeFWMemSize->uiFWDirectModeSize = directModeMemReq;
   pstDecodeFWMemSize->uiFWInnerLoopWorklistSize = innerLoopWorklistMemReq;

   BXVD_DBG_MSG(hXvd, ("ContextSize:0x%x (%d)", genMemReq, genMemReq));
   BXVD_DBG_MSG(hXvd, ("cabacMemReq:0x%x (%d)", cabacMemReq, cabacMemReq));
   BXVD_DBG_MSG(hXvd, ("VidBlockSize:0x%x (%d) , BlkCnt:%d", vidBlkSizeReq, vidBlkSizeReq, vidBlkCountReq));
   BXVD_DBG_MSG(hXvd, ("cabacWorklistMemReq:0x%x (%d)", cabacWorklistMemReq, cabacWorklistMemReq));
   BXVD_DBG_MSG(hXvd, ("directModeMemSize: 0x%x (%d)", directModeMemReq, directModeMemReq));
   BXVD_DBG_MSG(hXvd, ("innerLoopWorklistSize: 0x%x (%d)", innerLoopWorklistMemReq, innerLoopWorklistMemReq));

   return BERR_TRACE(BERR_SUCCESS);
}

BERR_Code BXVD_P_AllocateFWMem(BXVD_Handle hXvd,
                               BXVD_ChannelHandle hXvdCh,
                               BXVD_P_DecodeFWMemSize  *pstDecodeFWMemSize,
                               BXVD_P_DecodeFWBaseAddrs *pstDecodeFWBaseAddrs)
{
   uint32_t uiFWGenMemSize;
   uint32_t uiFWPicMemSize;
   uint32_t uiFWCabacMemSize;

   uint32_t *uiContextVirtAddr;
   uint32_t *uiPicBufVirtAddr;
   uint32_t *uiCabacVirtAddr;

   uint32_t uiMemBlockAddr;
   uint32_t uiTempPicMemAlignedSize;

   BERR_Code rc;

   pstDecodeFWBaseAddrs->uiFWContextBase = 0;
   pstDecodeFWBaseAddrs->uiFWPicBlockBase = 0;
   pstDecodeFWBaseAddrs->uiFWCabacBase = 0;
   pstDecodeFWBaseAddrs->uiFWCabacWorklistBase = 0;
   pstDecodeFWBaseAddrs->uiFWInterLayerPicBase = 0;
   pstDecodeFWBaseAddrs->uiFWInterLayerMVBase = 0;

   BXVD_DBG_MSG(hXvdCh, ("SubGenMem : %08x", hXvd->SubGenMem)); 
   BXVD_DBG_MSG(hXvdCh, ("SubSecureMem : %08x", hXvd->SubSecureMem)); 
   BXVD_DBG_MSG(hXvdCh, ("SubPicMem : %08x", hXvd->SubPicMem)); 
   BXVD_DBG_MSG(hXvdCh, ("Context Size: %d, Direct mode Size: %d, Inner Loop Work list size: %d", 
                         pstDecodeFWMemSize->uiFWContextSize, pstDecodeFWMemSize->uiFWDirectModeSize, 
                         pstDecodeFWMemSize->uiFWInnerLoopWorklistSize));

   BXVD_DBG_MSG(hXvdCh, ("Cabac Size: %d, Cabac Worklist Size: %d", 
                         pstDecodeFWMemSize->uiFWCabacSize, pstDecodeFWMemSize->uiFWCabacWorklistSize));

   BXVD_DBG_MSG(hXvdCh, ("Block Size: %d, Block Count: %d",
                         pstDecodeFWMemSize->uiFWPicBlockSize, pstDecodeFWMemSize->uiFWPicBlockCount));

   BXVD_DBG_MSG(hXvdCh, ("Inter Layer Pic Size: %d", pstDecodeFWMemSize->uiFWInterLayerPicSize));
   BXVD_DBG_MSG(hXvdCh, ("Inter Layer MV Size: %d", pstDecodeFWMemSize->uiFWInterLayerMVSize));


   /* Initialize FW Gen Mem Size to contain at least the context, Direct mode and IL Work list buffers */
   uiFWGenMemSize = pstDecodeFWMemSize->uiFWContextSize + pstDecodeFWMemSize->uiFWDirectModeSize +
      pstDecodeFWMemSize->uiFWInnerLoopWorklistSize + pstDecodeFWMemSize->uiFWInterLayerMVSize;

   uiFWPicMemSize = pstDecodeFWMemSize->uiFWPicBlockSize * pstDecodeFWMemSize->uiFWPicBlockCount;
   uiFWCabacMemSize = pstDecodeFWMemSize->uiFWCabacSize + pstDecodeFWMemSize->uiFWCabacWorklistSize;

   /* Allocate picture memory from picture heap, if it exists */ 
   if (hXvdCh->sChSettings.hChannelPictureHeap != NULL)
   {
      BXVD_DBG_MSG(hXvdCh, ("Allocating %d bytes of picture memory from CHANNEL picture heap", uiFWPicMemSize));
      uiPicBufVirtAddr = (uint32_t *)BMEM_AllocAligned(hXvdCh->sChSettings.hChannelPictureHeap, 
                                                       uiFWPicMemSize,
                                                       12,
                                                       0);

      if (uiPicBufVirtAddr == NULL)
      {
	 BXVD_DBG_ERR(hXvdCh, ("Picture memory allocation failure!"));
	 return BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
      }
      
      BMEM_ConvertAddressToOffset(hXvdCh->sChSettings.hChannelPictureHeap, uiPicBufVirtAddr, &(pstDecodeFWBaseAddrs->uiFWPicBlockBase));

      if (pstDecodeFWMemSize->uiFWInterLayerPicSize != 0)
      {
         uiPicBufVirtAddr = (uint32_t *)BMEM_AllocAligned(hXvdCh->sChSettings.hChannelPictureHeap, 
                                                          pstDecodeFWMemSize->uiFWInterLayerPicSize,
                                                          12,
                                                          0);

         if (uiPicBufVirtAddr == NULL)
         {
            BXVD_DBG_ERR(hXvdCh, ("Inter Layer Picture memory allocation failure!"));
            return BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
         }
      
         BMEM_ConvertAddressToOffset(hXvdCh->sChSettings.hChannelPictureHeap, uiPicBufVirtAddr, &(pstDecodeFWBaseAddrs->uiFWInterLayerPicBase));
      }

      hXvdCh->hPictureHeap = hXvdCh->sChSettings.hChannelPictureHeap;
   }
   else if (hXvd->SubPicMem != NULL)
   {
      BXVD_DBG_MSG(hXvdCh, ("Allocating %d bytes of picture memory from DEVICE picture heap", uiFWPicMemSize));
      uiPicBufVirtAddr = (uint32_t *)BXVD_P_Memory_Allocate(hXvd->SubPicMem, uiFWPicMemSize, 12, 0);
      
      if (uiPicBufVirtAddr == NULL)
      {
	 BXVD_DBG_ERR(hXvdCh, ("Picture memory allocation failure!"));
	 return BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
      }
      
      
      BMEM_ConvertAddressToOffset(hXvd->hPictureHeap, uiPicBufVirtAddr, &(pstDecodeFWBaseAddrs->uiFWPicBlockBase));

      if (pstDecodeFWMemSize->uiFWInterLayerPicSize != 0)
      {
         BXVD_DBG_MSG(hXvdCh, ("Allocating %d bytes of picture memory from DEVICE picture heap",
                               pstDecodeFWMemSize->uiFWInterLayerPicSize));

         uiPicBufVirtAddr = (uint32_t *)BXVD_P_Memory_Allocate(hXvd->SubPicMem, pstDecodeFWMemSize->uiFWInterLayerPicSize, 12, 0);
      
         if (uiPicBufVirtAddr == NULL)
         {
            BXVD_DBG_ERR(hXvdCh, ("Picture memory allocation failure!"));
            return BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
         }
      
         BMEM_ConvertAddressToOffset(hXvd->hPictureHeap, uiPicBufVirtAddr, &(pstDecodeFWBaseAddrs->uiFWInterLayerPicBase));
      }

      hXvdCh->hPictureHeap = hXvd->hPictureHeap;
   }
   else
   {
      if (pstDecodeFWMemSize->uiFWInterLayerPicSize != 0)
      {
         /* Need to make sure Interlayer picture memory starts on 4k boundary. Force normal Picture memory
          * buffer to end on 4k boundary */
         uiTempPicMemAlignedSize = ((uiFWPicMemSize + 4095) / 4096) * 4096;
         
         uiFWGenMemSize += uiTempPicMemAlignedSize + pstDecodeFWMemSize->uiFWInterLayerPicSize;
      }
      else
      {
         uiFWGenMemSize += uiFWPicMemSize;
      }

      hXvdCh->hPictureHeap = hXvd->hSystemHeap;
   }
   
   /* Allocate cabac from secure heap, if it exists */
   if (uiFWCabacMemSize != 0) 
   {
      if (hXvdCh->sChSettings.hChannelCabacHeap != NULL)
      {
         BXVD_DBG_MSG(hXvdCh, ("Allocating %d bytes of cabac memory from CHANNEL cabac heap", uiFWCabacMemSize));
         uiCabacVirtAddr = (uint32_t *)BMEM_AllocAligned(hXvdCh->sChSettings.hChannelCabacHeap, 
                                                         uiFWCabacMemSize,
                                                         8,
                                                         0);
         if (uiCabacVirtAddr == NULL)
         {
            BXVD_DBG_ERR(hXvdCh, ("Secure memory allocation failure!"));
            return BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
         }
         
         BMEM_ConvertAddressToOffset(hXvdCh->sChSettings.hChannelCabacHeap, uiCabacVirtAddr, &(pstDecodeFWBaseAddrs->uiFWCabacBase));
         hXvdCh->hCabacHeap = hXvdCh->sChSettings.hChannelCabacHeap;

         /* Pre rev K cores CABAC must be in memory lower than 768 MB */ 
#if !BXVD_P_AVD_ARC600
         if ((pstDecodeFWBaseAddrs->uiFWCabacBase + uiFWCabacMemSize) >= BXVD_P_ARC300_RAM_LIMIT)
         {
            BXVD_DBG_ERR(hXvdCh, ("Cabac buffer (%0x) allocated in memory greater than 768MB!", pstDecodeFWBaseAddrs->uiFWCabacBase));
            return BERR_TRACE(BERR_INVALID_PARAMETER);
         }
#endif
         /* Assign CABAC worklist base at end of CABAC buffer */
         pstDecodeFWBaseAddrs->uiFWCabacWorklistBase = pstDecodeFWBaseAddrs->uiFWCabacBase + pstDecodeFWMemSize->uiFWCabacSize;
      }
      else if (hXvd->SubSecureMem != NULL)
      {
         BXVD_DBG_MSG(hXvdCh, ("Allocating %d bytes of cabac memory from DEVICE cabac heap", uiFWCabacMemSize));
         uiCabacVirtAddr = (uint32_t *)BXVD_P_Memory_Allocate(hXvd->SubSecureMem, uiFWCabacMemSize, 8, 0);
         
         if (uiCabacVirtAddr == NULL)
         {
            BXVD_DBG_ERR(hXvdCh, ("Secure memory allocation failure!"));
            return BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
         }
         BMEM_ConvertAddressToOffset(hXvd->hCabacHeap, uiCabacVirtAddr, &(pstDecodeFWBaseAddrs->uiFWCabacBase));

         /* Pre rev K cores CABAC must be in memory lower than 768 MB */ 
#if !BXVD_P_AVD_ARC600
         if ((pstDecodeFWBaseAddrs->uiFWCabacBase + uiFWCabacMemSize) > BXVD_P_ARC300_RAM_LIMIT)
         {
            BXVD_DBG_ERR(hXvdCh, ("Cabac buffer allocated in memory greater than 768MB!"));
            return BERR_TRACE(BERR_INVALID_PARAMETER);
         }
#endif
         hXvdCh->hCabacHeap = hXvd->hCabacHeap;

         /* Assign CABAC worklist base at end of CABAC buffer */
         pstDecodeFWBaseAddrs->uiFWCabacWorklistBase = pstDecodeFWBaseAddrs->uiFWCabacBase + pstDecodeFWMemSize->uiFWCabacSize;
      }
      else
      {
         uiFWGenMemSize += uiFWCabacMemSize;
         hXvdCh->hCabacHeap = hXvd->hSystemHeap;
      }
   }

   /* Allocate general heap */
   if (hXvdCh->sChSettings.hChannelSystemHeap != NULL)
   {
      BXVD_DBG_MSG(hXvdCh, ("Allocating %d bytes of system memory from CHANNEL system heap", uiFWGenMemSize));
      uiContextVirtAddr = (uint32_t *)BMEM_AllocAligned(hXvdCh->sChSettings.hChannelSystemHeap, 
                                                      uiFWGenMemSize,
                                                      12,
                                                      0);
      
      if (uiContextVirtAddr == NULL)
      {
         BXVD_DBG_ERR(hXvdCh, ("General memory allocation failure!"));
         return BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
      }
      
      BMEM_ConvertAddressToOffset(hXvdCh->sChSettings.hChannelSystemHeap, uiContextVirtAddr, &uiMemBlockAddr);
      hXvdCh->hSystemHeap = hXvdCh->sChSettings.hChannelSystemHeap;
   }
   else
   {
      BXVD_DBG_MSG(hXvdCh, ("Allocating %d bytes of system memory from DEVICE system heap", uiFWGenMemSize));
      uiContextVirtAddr = (uint32_t *)BXVD_P_Memory_Allocate(hXvd->SubGenMem, uiFWGenMemSize, 12, 0); 

      if (uiContextVirtAddr == NULL)
      {
         BXVD_DBG_ERR(hXvdCh, ("General memory allocation failure!"));
         return BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
      }

      BMEM_ConvertAddressToOffset(hXvd->hSystemHeap, (void *)uiContextVirtAddr, &uiMemBlockAddr); 

      /* Pre rev K core general memory must be in region lower than 768 MB */ 
#if !BXVD_P_AVD_ARC600
      if ((uiMemBlockAddr + uiFWGenMemSize) >= BXVD_P_ARC300_RAM_LIMIT)
         {
            BXVD_DBG_ERR(hXvdCh, ("General AVD Firmware memory (%0x) allocated in region greater than 768MB!", uiMemBlockAddr));
            return BERR_TRACE(BERR_INVALID_PARAMETER);
         }
#endif

      hXvdCh->hSystemHeap = hXvd->hSystemHeap;
   }

   /* Assign picture buffer from general heap if it is not already allocated */
   if ((uiFWPicMemSize != 0) && (pstDecodeFWBaseAddrs->uiFWPicBlockBase == 0))
   {
      pstDecodeFWBaseAddrs->uiFWPicBlockBase = uiMemBlockAddr;

      if( pstDecodeFWMemSize->uiFWInterLayerPicSize != 0)
      {
         /* Need to make sure Interlayer picture memory starts on 4k boundary. Force normal Picture memory
          * buffer to end on 4k boundary */
         uiTempPicMemAlignedSize = ((uiFWPicMemSize + 4095) / 4096) * 4096;
         uiMemBlockAddr += uiTempPicMemAlignedSize;

         pstDecodeFWBaseAddrs->uiFWInterLayerPicBase = uiMemBlockAddr;
         uiMemBlockAddr += pstDecodeFWMemSize->uiFWInterLayerPicSize;
      }
      else
      {
         uiMemBlockAddr += uiFWPicMemSize;
      }
   }

   /* Assign cabac buffer from general heap if it is not already allocated */
   if ((pstDecodeFWMemSize->uiFWCabacSize != 0) && (pstDecodeFWBaseAddrs->uiFWCabacBase == 0))
   {
      pstDecodeFWBaseAddrs->uiFWCabacBase = uiMemBlockAddr;      
      uiMemBlockAddr += pstDecodeFWMemSize->uiFWCabacSize;
   }

   /* Assign cabac worklist buffer from general heap if it is not already allocated */
   if ((pstDecodeFWMemSize->uiFWCabacWorklistSize != 0) && (pstDecodeFWBaseAddrs->uiFWCabacWorklistBase == 0))
   {
      pstDecodeFWBaseAddrs->uiFWCabacWorklistBase = uiMemBlockAddr;      
      uiMemBlockAddr += pstDecodeFWMemSize->uiFWCabacWorklistSize;
   }
   
   /* Assign context buffer */
   if ((pstDecodeFWMemSize->uiFWContextSize != 0) && (pstDecodeFWBaseAddrs->uiFWContextBase == 0))
   {
      pstDecodeFWBaseAddrs->uiFWContextBase = uiMemBlockAddr;      
      uiMemBlockAddr += pstDecodeFWMemSize->uiFWContextSize;

      BMEM_ConvertOffsetToAddress(hXvdCh->hSystemHeap, uiMemBlockAddr, &pstDecodeFWBaseAddrs->pvFWContextVABase);

      rc = BMEM_Heap_ConvertAddressToCached(hXvdCh->hSystemHeap, pstDecodeFWBaseAddrs->pvFWContextVABase,
                                            &pstDecodeFWBaseAddrs->pvFWContextCachedBase);

      if (rc != BERR_SUCCESS)
      {
         return BERR_TRACE(rc);
      }
   }

   /* Assign Direct mode buffer */
   if (pstDecodeFWMemSize->uiFWDirectModeSize != 0)
   {
      pstDecodeFWBaseAddrs->uiFWDirectModeBase = uiMemBlockAddr;
      uiMemBlockAddr += pstDecodeFWMemSize->uiFWDirectModeSize;
   }

   /* Assign inner loop work list buffer address */
   if (pstDecodeFWMemSize->uiFWInnerLoopWorklistSize != 0)
   {
      pstDecodeFWBaseAddrs->uiFWInnerLoopWorklistBase = uiMemBlockAddr;
      uiMemBlockAddr += pstDecodeFWMemSize->uiFWInnerLoopWorklistSize;
   }

   if ( pstDecodeFWMemSize->uiFWInterLayerMVSize != 0)
   {
      pstDecodeFWBaseAddrs->uiFWInterLayerMVBase = uiMemBlockAddr;
   }

   
   BXVD_DBG_MSG(hXvdCh, ("FWContextBase PA: %08x", pstDecodeFWBaseAddrs->uiFWContextBase));   
   BXVD_DBG_MSG(hXvdCh, ("FWPicBase PA: %08x", pstDecodeFWBaseAddrs->uiFWPicBlockBase));   
   BXVD_DBG_MSG(hXvdCh, ("FWCabacBase PA: %08x", pstDecodeFWBaseAddrs->uiFWCabacBase));
   BXVD_DBG_MSG(hXvdCh, ("FWDirectModeBase PA: %08x", pstDecodeFWBaseAddrs->uiFWDirectModeBase));
   BXVD_DBG_MSG(hXvdCh, ("FWInnerLoopWorklistBase PA: %08x", pstDecodeFWBaseAddrs->uiFWInnerLoopWorklistBase));
   BXVD_DBG_MSG(hXvdCh, ("FWInterLayerPicBase PA: %08x", pstDecodeFWBaseAddrs->uiFWInterLayerPicBase));
   BXVD_DBG_MSG(hXvdCh, ("FWInterLayerMVBase PA: %08x", pstDecodeFWBaseAddrs->uiFWInterLayerMVBase));

   return BERR_TRACE(BERR_SUCCESS); 
}


BERR_Code BXVD_P_FreeFWMem(BXVD_Handle hXvd,
                           BXVD_ChannelHandle hXvdCh,
                           uint32_t uiFWContextBase, 
                           uint32_t uiFWCabacBase, 
                           uint32_t uiFWPicBlockBase,
                           uint32_t uiFWCabacWorklistBase,
                           uint32_t uiFWInterLayerPicBase)
{

   uint32_t *uiContextVirtAddr;
   uint32_t *uiPicBufVirtAddr;
   uint32_t *uiCabacVirtAddr;

   uint32_t uiMemBlockAddr;

   bool bPicBufFreed = false;
   bool bCabacFreed = false;
   BERR_Code rc = BERR_SUCCESS;

   BSTD_UNUSED(uiFWCabacWorklistBase);

   BXVD_DBG_MSG(hXvdCh, ("SubGenMem : %08x", hXvd->SubGenMem)); 
   BXVD_DBG_MSG(hXvdCh, ("SubSecureMem : %08x", hXvd->SubSecureMem)); 
   BXVD_DBG_MSG(hXvdCh, ("SubPicMem : %08x", hXvd->SubPicMem)); 

   if (uiFWPicBlockBase)
   {
      BXVD_DBG_MSG(hXvdCh, ("Picture buffers in specified heap, Free sub-allocation\n"));

      if (hXvdCh->sChSettings.hChannelPictureHeap != NULL)
      {
         /* Free picture buffer sub-allocated block */
         BMEM_ConvertOffsetToAddress(hXvdCh->sChSettings.hChannelPictureHeap, uiFWPicBlockBase, (void *)&uiPicBufVirtAddr);
         
         rc = BMEM_Free(hXvdCh->sChSettings.hChannelPictureHeap, uiPicBufVirtAddr);
         
         if ( rc != BERR_SUCCESS)
         {
            BXVD_DBG_ERR(hXvdCh, ("failed to free sub allocated Picture memory"));
            return BERR_TRACE(BERR_LEAKED_RESOURCE);
         }

         if (uiFWInterLayerPicBase != 0)
         {

            /* Free inter layer picture buffer sub-allocated block */
            BMEM_ConvertOffsetToAddress(hXvdCh->sChSettings.hChannelPictureHeap, uiFWInterLayerPicBase, (void *)&uiPicBufVirtAddr);
         
            rc = BMEM_Free(hXvdCh->sChSettings.hChannelPictureHeap, uiPicBufVirtAddr);
         
            if ( rc != BERR_SUCCESS)
            {
               BXVD_DBG_ERR(hXvdCh, ("failed to free sub allocated Inter Layer Picture memory"));
               return BERR_TRACE(BERR_LEAKED_RESOURCE);
            }
         }

         bPicBufFreed = true;
      }
      else if (hXvd->SubPicMem != NULL)
      {
         /* Free picture buffer sub-allocated block */
         BMEM_ConvertOffsetToAddress(hXvd->hPictureHeap, uiFWPicBlockBase, (void *)&uiPicBufVirtAddr);
         
         rc = BXVD_P_Memory_Free(hXvd->SubPicMem, uiPicBufVirtAddr);
         
         if ( rc != BERR_SUCCESS)
         {
            BXVD_DBG_ERR(hXvdCh, ("failed to free sub allocated Picture memory"));
            return BERR_TRACE(BERR_LEAKED_RESOURCE);
         }

         /* If interlayer picture memory is allocated, free it */
         if (uiFWInterLayerPicBase != 0)
         {
            /* Free inter layer picture buffer sub-allocated block */
            BMEM_ConvertOffsetToAddress(hXvd->hPictureHeap, uiFWInterLayerPicBase, (void *)&uiPicBufVirtAddr);
         
            rc = BXVD_P_Memory_Free(hXvd->SubPicMem, uiPicBufVirtAddr);
         
            if ( rc != BERR_SUCCESS)
            {
               BXVD_DBG_ERR(hXvdCh, ("failed to free sub allocated Inter Layer Picture memory"));
               return BERR_TRACE(BERR_LEAKED_RESOURCE);
            }
         }

         bPicBufFreed = true;
      }
   }

   if (uiFWCabacBase)
   {      
      BXVD_DBG_MSG(hXvdCh, ("Cabac in Secure memory, Free sub-allocation \n"));

      if (hXvdCh->sChSettings.hChannelCabacHeap != NULL)
      {
         BMEM_ConvertOffsetToAddress(hXvdCh->sChSettings.hChannelCabacHeap, uiFWCabacBase, (void *)&uiCabacVirtAddr);
         
         rc = BMEM_Free(hXvdCh->sChSettings.hChannelCabacHeap, uiCabacVirtAddr);
         
         if ( rc != BERR_SUCCESS)
         {
            BXVD_DBG_ERR(hXvdCh, ("failed to free sub allocated CABAC memory"));
            return BERR_TRACE(BERR_LEAKED_RESOURCE);
         }         
         bCabacFreed = true;
      }      
      else if (hXvd->SubSecureMem != NULL)
      {
         BMEM_ConvertOffsetToAddress(hXvd->hCabacHeap, uiFWCabacBase, (void *)&uiCabacVirtAddr);
         
         rc = BXVD_P_Memory_Free(hXvd->SubSecureMem, uiCabacVirtAddr);
         
         if ( rc != BERR_SUCCESS)
         {
            BXVD_DBG_ERR(hXvdCh, ("failed to free sub allocated CABAC memory"));
            return BERR_TRACE(BERR_LEAKED_RESOURCE);
         }         
         bCabacFreed = true;
      }
   }

   BXVD_DBG_MSG(hXvdCh, ("Free Context sub-allocation\n"));      
   if (uiFWPicBlockBase && !bPicBufFreed)
   {      
      uiMemBlockAddr = uiFWPicBlockBase;
   }
   else if (uiFWCabacBase && !bCabacFreed)
   {
      uiMemBlockAddr = uiFWCabacBase; 
   }
   else
   {
      uiMemBlockAddr = uiFWContextBase;
   }
   
   if (uiFWContextBase)
   {
      /* Free context memory, this also could contain the picture and
       * cabac memory if not in their own heaps */ 
      if (hXvdCh->sChSettings.hChannelSystemHeap != NULL)
      {
         BMEM_ConvertOffsetToAddress(hXvdCh->sChSettings.hChannelSystemHeap, uiMemBlockAddr, (void *)&uiContextVirtAddr);
         
         rc = BMEM_Free(hXvdCh->sChSettings.hChannelSystemHeap, uiContextVirtAddr);
         
         if (rc != BERR_SUCCESS)
         {
            BXVD_DBG_ERR(hXvdCh, ("failed to free sub allocated Context memory"));
            return BERR_TRACE(BERR_LEAKED_RESOURCE);
         }    
      }
      else
      {
         BMEM_ConvertOffsetToAddress(hXvd->hSystemHeap, uiMemBlockAddr, (void *)&uiContextVirtAddr);
         
         rc = BXVD_P_Memory_Free(hXvd->SubGenMem, uiContextVirtAddr);
         
         if (rc != BERR_SUCCESS)
         {
            BXVD_DBG_ERR(hXvdCh, ("failed to free sub allocated Context memory"));
            return BERR_TRACE(BERR_LEAKED_RESOURCE);
         }               
      }
   }
   
   return BERR_TRACE(rc); 
}

/*
 * This function maps FW and shared memory, loads the inner and outer loop
 * FW images and kick starts the outer loop ARC.
 */
BERR_Code BXVD_P_ChipInit(BXVD_Handle hXvd, uint32_t uDecoderInstance)
{
   uint32_t     *p_FirmWareVBase;
   uint32_t     ui_PhysicalBase;
   BERR_Code rc;

   p_FirmWareVBase = (void *)hXvd->uiFWMemVirtAddr; 
   ui_PhysicalBase = hXvd->uiFWMemPhyAddr; 

   rc = BXVD_P_FW_LOAD(hXvd, uDecoderInstance);
   if (rc != BERR_SUCCESS) return BERR_TRACE(rc);

   /* Enable 740x operation */

   BXVD_DBG_MSG(hXvd, ("Enabling chip execution"));
   rc = BXVD_P_CHIP_ENABLE(hXvd);
   if (rc != BERR_SUCCESS) return BERR_TRACE(rc);

   BXVD_DBG_MSG(hXvd, ("Shared memory start addr: 0x%x", hXvd->uiFWGenMemPhyAddr));

#if FW_INIT
   rc = BXVD_P_HostCmdSendInit(hXvd,
			       uDecoderInstance,
			       hXvd->stSettings.eRaveEndianess
                               );

   if (rc != BERR_SUCCESS) return BERR_TRACE(rc);
#endif
   return BERR_TRACE(BERR_SUCCESS);
}

/*
 * Reset 740x SDRAM parameters.
 */
BERR_Code BXVD_P_Reset740x(BXVD_Handle  hXvd, uint32_t uDecoderInstance)
{
   BERR_Code rc;

   BSTD_UNUSED(uDecoderInstance);
  
   /* Reset Video Decoder */
   rc = BXVD_P_RESET_CHIP(hXvd);

   /* AVD has been reset, clear device busy */
   hXvd->stDecoderContext.bIfBusy = 0;	

   return BERR_TRACE(rc);
}

/***************************************************************************
  BXVD_P_StillPictureEnabled: Enable still pictures
****************************************************************************/
BERR_Code BXVD_P_StillPictureEnabled(BXVD_Handle hXvd, bool *pEnableState, BXVD_DecodeMode *pSPDecodeMode)
{
   BXVD_DecodeMode *pTMPDecodeMode;

   BDBG_ASSERT(hXvd);

   BXVD_DBG_MSG(hXvd, ("SPEnabled?"));

   *pSPDecodeMode = BXVD_DecodeMode_eTerminateList;

   pTMPDecodeMode = (BXVD_DecodeMode *)hXvd->stSettings.pDecodeModes;
	
   *pEnableState = false;

   while (*pTMPDecodeMode !=  BXVD_DecodeMode_eTerminateList)
   {
      BXVD_DBG_MSG(hXvd, ("\tDecodemode:%x", *pTMPDecodeMode));

      if (*pTMPDecodeMode >= BXVD_DecodeMode_eMaxModes)
      {
	 return BERR_TRACE(BERR_INVALID_PARAMETER);
      }

      if ((*pTMPDecodeMode == BXVD_DecodeMode_eStill_SD) && (*pSPDecodeMode != BXVD_DecodeMode_eStill_HD))
      {
	 *pEnableState = true;
	 *pSPDecodeMode = BXVD_DecodeMode_eStill_SD;
      }
      else if ((*pTMPDecodeMode == BXVD_DecodeMode_eStill_HD) || (*pTMPDecodeMode == BXVD_DecodeMode_eAll))
      {
	 *pEnableState = true;
	 *pSPDecodeMode = BXVD_DecodeMode_eStill_HD;
      }
      pTMPDecodeMode++;
   }

   return BERR_TRACE(BERR_SUCCESS);
}

/***************************************************************************
  BXVD_P_ParseDecodeMode: Return buffer size info for indicated decode mode
****************************************************************************/
BERR_Code BXVD_P_ParseDecodeMode(BXVD_Handle hXvd,
                                 BXVD_DecodeMode *pDecodeModeList,
                                 BXVD_DecodeMode *eDecodeMode)
{
   int32_t PicBufSize = 0;
   
   BXVD_DecodeMode *pTmpDecMode = pDecodeModeList;

   BDBG_ASSERT(pDecodeModeList);
   BDBG_ASSERT(eDecodeMode);

   *eDecodeMode = BXVD_DecodeMode_eTerminateList;

   while ((*pTmpDecMode != BXVD_DecodeMode_eTerminateList))
   {
      int32_t videoMemSize;

      if (*pTmpDecMode >= BXVD_DecodeMode_eMaxModes)
      {
         BXVD_DBG_ERR(hXvd, ("Invalid decode mode in list"));
	 return BERR_TRACE(BERR_INVALID_PARAMETER);
      }
      
      if ((*eDecodeMode == BXVD_DecodeMode_eCustom) &&
          !((*pTmpDecMode == BXVD_DecodeMode_eCustom) ||
            (*pTmpDecMode == BXVD_DecodeMode_eStill_HD) ||
            (*pTmpDecMode == BXVD_DecodeMode_eStill_SD)))
      {
         /* If eCustom is already set, we need to return an error if
          * any other decode mode (other than stills) is in the list.
          * We only support the following three combinations:
            1) Any number of BXVD_DecodeMode_eXXX other than eCustom
            2) eCustom and (eStill_HD and/or eStill_SD)
            3) eCustom only
         */
         BXVD_DBG_ERR(hXvd, ("Invalid combination in decode mode list"));
         return BERR_TRACE(BERR_INVALID_PARAMETER);
      }
      
      if ((*pTmpDecMode == BXVD_DecodeMode_eCustom) &&
          ((*eDecodeMode != BXVD_DecodeMode_eTerminateList) &&
           (*eDecodeMode != BXVD_DecodeMode_eCustom) &&
           (*eDecodeMode != BXVD_DecodeMode_eStill_HD) &&
           (*eDecodeMode != BXVD_DecodeMode_eStill_SD)))
      {
         /* If the current decode mode in the list is eCustom, but
          * some other decode mode (other than stills) is set, then we
          * also need to return an error. We only support the
          * following three combinations:
            1) Any combination of BXVD_DecodeMode_eXXX (other than eCustom)
            2) eCustom and (eStill_HD and/or eStill_SD)
            3) eCustom only 
         */
         BXVD_DBG_ERR(hXvd, ("Invalid combination in decode mode list"));
         return BERR_TRACE(BERR_INVALID_PARAMETER);         
      }
      
      if (*pTmpDecMode == BXVD_DecodeMode_eCustom)
      {
         /* eCustom takes precendence over all other decode modes */
         *eDecodeMode = *pTmpDecMode;
      }
      else if (*eDecodeMode != BXVD_DecodeMode_eCustom) /* Only change the decode mode if it not already eCustom */
      {      
         videoMemSize = hXvd->stFWMemCfg[*pTmpDecMode].video_block_size * hXvd->stFWMemCfg[*pTmpDecMode].video_block_count;
         
         if ( PicBufSize < videoMemSize)
         {
            PicBufSize = videoMemSize;
            *eDecodeMode = *pTmpDecMode;
         }
      }
			
      pTmpDecMode++; /* Point to next item in list */
   }

   if (*eDecodeMode == BXVD_DecodeMode_eTerminateList)
   {
      /* In case list is bogus, return max size */
      *eDecodeMode = BXVD_DecodeMode_eAll;
   }

   return BERR_TRACE(BERR_SUCCESS);
}

BERR_Code BXVD_P_Boot(BXVD_Handle hXvd)
{
   BERR_Code rc;

   /* Reset 740x */
   rc = BERR_TRACE(BXVD_P_Reset740x(hXvd, hXvd->uDecoderInstance));
   if(rc != BERR_SUCCESS) 
   { 
      return BERR_TRACE(rc);
   }
	
   /* Initialize interrupts */
   rc = BXVD_P_SETUP_INTERRUPTS(hXvd);
   if (rc != BERR_SUCCESS)
   {
      return BERR_TRACE(rc);
   }
	
   /* Initialize the inner and outer loop firmware. */
   rc = BXVD_P_ChipInit(hXvd, hXvd->uDecoderInstance);
   if(rc != BERR_SUCCESS) 
   { 
      return BERR_TRACE(rc);
   }
	
   /* Start the watchdog after the decoder starts */
   rc = BXVD_P_SETUP_WATCHDOG(hXvd);
   if (rc != BERR_SUCCESS)
   {
      return BERR_TRACE(rc);
   }
	
   return BERR_TRACE(BERR_SUCCESS);
}

static const BAVC_VideoCompressionStd  StillCmprStdList[] =
{
   BAVC_VideoCompressionStd_eMPEG2
};

/*
 * Description: 
 *
 *   When an applications is using the multi-decode API but specifying 
 * video decode modes from the single decode api, a channel is opened
 * for still picture decode. This routine opens the channel for MPEG2
 * still picture decoding if XVD has determined still picture compatibility 
 * mode is being used. 
 */

BERR_Code BXVD_P_SetupStillPictureCompatibilityMode(BXVD_Handle hXvd)
{
   BERR_Code rc;
   BXVD_ChannelHandle hXvdCh;

   /* coverity[var_decl: FALSE] */
   BXVD_ChannelSettings sChSettings;

   uint32_t uiStillChannelNum = BXVD_MAX_VIDEO_CHANNELS - 1;

   if (hXvd->bStillPictureCompatibilityMode)
   {
      if (hXvd->uiOpenChannels >= BXVD_MAX_VIDEO_CHANNELS) 
      {
         BXVD_DBG_ERR(hXvd, ("Still Picture Compatibility Mode: No channels available for the implicit creation of still picture channel"));
         return BERR_TRACE(BERR_INVALID_PARAMETER);
      }

      if (hXvd->ahChannel[uiStillChannelNum] != NULL)
      {
         BXVD_DBG_ERR(hXvd, ("Still Picture Compatibility Mode: Channel[%d] not available for the implicit creation of still picture channel", uiStillChannelNum));
         return BERR_TRACE(BERR_INVALID_PARAMETER);            
      }
      /* coverity[uninit_use_in_call: FALSE] */
      BXVD_GetChannelDefaultSettings(hXvd,
                                     uiStillChannelNum,
                                     &sChSettings);

      /* Settings for MPEG2 Still Decode */
      sChSettings.peVideoCmprStdList = (BAVC_VideoCompressionStd*) &StillCmprStdList;
      sChSettings.uiVideoCmprCount = 1;
      sChSettings.eChannelMode = BXVD_ChannelMode_eStill;

      if (hXvd->bStillHDCapable)
      {
         sChSettings.eDecodeResolution = BXVD_DecodeResolution_eHD;
      }

      else
      {
         sChSettings.eDecodeResolution = BXVD_DecodeResolution_eSD;
      }

      rc = BXVD_OpenChannel(
         hXvd,
         &hXvdCh,
         uiStillChannelNum,
         &sChSettings);

      if (rc != BERR_SUCCESS)
      {
         BXVD_DBG_ERR(hXvd, ("Still Picture Compatibility Mode: Error opening still channel[%d]", uiStillChannelNum));
         return BERR_TRACE(rc);
      }

      BXVD_DBG_WRN(hXvd, ("Still Picture Compatibility Mode: created implicit still picture channel [%d]",                   
                               hXvdCh->ulChannelNum));
   }

   return BERR_TRACE(BERR_SUCCESS);
}

BERR_Code BXVD_P_TeardownStillPictureCompatibilityMode(BXVD_Handle hXvd)
{
   BERR_Code rc;

   if ((hXvd->bStillPictureCompatibilityMode) && (hXvd->ahChannel != NULL))
   {
      if (hXvd->ahChannel[hXvd->uiStillChannelNum] != NULL)
      {         
         rc = BXVD_CloseChannel(hXvd->ahChannel[hXvd->uiStillChannelNum]);
         if (rc != BERR_SUCCESS)
         {
            BXVD_DBG_ERR(hXvd, ("Still Picture Compatibility Mode: Error closing still channel[%d]", hXvd->uiStillChannelNum));
            return BERR_TRACE(rc);
         }

#if BXVD_P_POWER_MANAGEMENT
         /* Wake up the decoder, may have been put to sleep closing the still channel  */
         BXVD_P_SetHibernateState(hXvd, false);
#endif         
      }
   }
   return BERR_TRACE(BERR_SUCCESS);
}

BERR_Code BXVD_P_InitializeFWMemConfigCompatibilityModeTable(BXVD_Handle hXvd)
{
   BERR_Code rc;
   BXVD_DecodeResolution eDecodeResolution;
   BAVC_VideoCompressionStd aeVideoCmprStd[1];
   BXVD_DecodeMode eDecodeModeIndex = 0;
   BXVD_ChannelSettings ChSettings;
   BXVD_P_DecodeFWMemSize stDecodeFWMemSize;

   /* BXVD_DecodeMode_eAVC_HD */
   eDecodeResolution = BXVD_DecodeResolution_eHD;
   aeVideoCmprStd[0] = BAVC_VideoCompressionStd_eH264;

   ChSettings.bMPEG2BTPEnable = true;
   ChSettings.bAVC41Enable = false;
   ChSettings.bAVC51Enable = false;
   ChSettings.bBluRayEnable = false;
   ChSettings.bExcessDirModeEnable = false;
   ChSettings.bSVC3DModeEnable = false;
   ChSettings.bSVCProgressiveOnly = false;
   ChSettings.b1920PortraitModeEnable = false;

   rc = BXVD_P_GetDecodeFWMemSize(hXvd,
                                  eDecodeResolution,
                                  aeVideoCmprStd,
                                  1,
                                  &ChSettings,
                                  &stDecodeFWMemSize);

   hXvd->stFWMemCfg[BXVD_DecodeMode_eAVC_HD].general_memory_size = stDecodeFWMemSize.uiFWContextSize;
   hXvd->stFWMemCfg[BXVD_DecodeMode_eAVC_HD].cabac_bin_size  = stDecodeFWMemSize.uiFWCabacSize;
   hXvd->stFWMemCfg[BXVD_DecodeMode_eAVC_HD].video_block_size = stDecodeFWMemSize.uiFWPicBlockSize;
   hXvd->stFWMemCfg[BXVD_DecodeMode_eAVC_HD].video_block_count =  stDecodeFWMemSize.uiFWPicBlockCount;

   hXvd->stFWMemCfg[BXVD_DecodeMode_eAVC_HD].cabac_bin_size += stDecodeFWMemSize.uiFWCabacWorklistSize;

   hXvd->stFWMemCfg[BXVD_DecodeMode_eAVC_HD].general_memory_size += 
      stDecodeFWMemSize.uiFWDirectModeSize + stDecodeFWMemSize.uiFWInnerLoopWorklistSize;

   if (rc != BERR_SUCCESS)
   {
      return BERR_TRACE(rc);
   }
   
  
   /* BXVD_DecodeMode_eAVC_SD */
   eDecodeResolution = BXVD_DecodeResolution_eSD;
   aeVideoCmprStd[0] = BAVC_VideoCompressionStd_eH264;
   rc = BXVD_P_GetDecodeFWMemSize(hXvd,
                                  eDecodeResolution,
                                  aeVideoCmprStd,
                                  1,
                                  &ChSettings,
                                  &stDecodeFWMemSize);

   hXvd->stFWMemCfg[BXVD_DecodeMode_eAVC_SD].general_memory_size = stDecodeFWMemSize.uiFWContextSize;
   hXvd->stFWMemCfg[BXVD_DecodeMode_eAVC_SD].cabac_bin_size  = stDecodeFWMemSize.uiFWCabacSize;
   hXvd->stFWMemCfg[BXVD_DecodeMode_eAVC_SD].video_block_size = stDecodeFWMemSize.uiFWPicBlockSize;
   hXvd->stFWMemCfg[BXVD_DecodeMode_eAVC_SD].video_block_count =  stDecodeFWMemSize.uiFWPicBlockCount;

   hXvd->stFWMemCfg[BXVD_DecodeMode_eAVC_SD].cabac_bin_size += stDecodeFWMemSize.uiFWCabacWorklistSize;

   hXvd->stFWMemCfg[BXVD_DecodeMode_eAVC_SD].general_memory_size += 
      stDecodeFWMemSize.uiFWDirectModeSize + stDecodeFWMemSize.uiFWInnerLoopWorklistSize;

   if (rc != BERR_SUCCESS)
   {
      return BERR_TRACE(rc);
   }
                                  
   /* BXVD_DecodeMode_eMPEG2_HD */
   eDecodeResolution = BXVD_DecodeResolution_eHD;
   aeVideoCmprStd[0] = BAVC_VideoCompressionStd_eMPEG2;
   rc = BXVD_P_GetDecodeFWMemSize(hXvd,
                                  eDecodeResolution,
                                  aeVideoCmprStd,
                                  1,
                                  &ChSettings,
                                  &stDecodeFWMemSize);

   hXvd->stFWMemCfg[BXVD_DecodeMode_eMPEG2_HD].video_block_size = stDecodeFWMemSize.uiFWPicBlockSize;
   hXvd->stFWMemCfg[BXVD_DecodeMode_eMPEG2_HD].video_block_count =  stDecodeFWMemSize.uiFWPicBlockCount;

   hXvd->stFWMemCfg[BXVD_DecodeMode_eMPEG2_HD].general_memory_size = stDecodeFWMemSize.uiFWContextSize + 
      stDecodeFWMemSize.uiFWDirectModeSize + stDecodeFWMemSize.uiFWInnerLoopWorklistSize;

   hXvd->stFWMemCfg[BXVD_DecodeMode_eMPEG2_HD].cabac_bin_size = 
      stDecodeFWMemSize.uiFWCabacSize + stDecodeFWMemSize.uiFWCabacWorklistSize;

   if (rc != BERR_SUCCESS)
   {
      return BERR_TRACE(rc);
   }

   /* BXVD_DecodeMode_eMPEG2_SD */
   eDecodeResolution = BXVD_DecodeResolution_eSD;
   aeVideoCmprStd[0] = BAVC_VideoCompressionStd_eMPEG2;
   rc = BXVD_P_GetDecodeFWMemSize(hXvd,
                                  eDecodeResolution,
                                  aeVideoCmprStd,
                                  1,
                                  &ChSettings,
                                  &stDecodeFWMemSize);

   hXvd->stFWMemCfg[BXVD_DecodeMode_eMPEG2_SD].video_block_size = stDecodeFWMemSize.uiFWPicBlockSize;
   hXvd->stFWMemCfg[BXVD_DecodeMode_eMPEG2_SD].video_block_count =  stDecodeFWMemSize.uiFWPicBlockCount;

   hXvd->stFWMemCfg[BXVD_DecodeMode_eMPEG2_SD].general_memory_size = stDecodeFWMemSize.uiFWContextSize + 
      stDecodeFWMemSize.uiFWDirectModeSize + stDecodeFWMemSize.uiFWInnerLoopWorklistSize;

   hXvd->stFWMemCfg[BXVD_DecodeMode_eMPEG2_SD].cabac_bin_size = 
      stDecodeFWMemSize.uiFWCabacSize + stDecodeFWMemSize.uiFWCabacWorklistSize;

   if (rc != BERR_SUCCESS)
   {
      return BERR_TRACE(rc);
   }
   
   /* BXVD_DecodeMode_eVC1_HD */
   eDecodeResolution = BXVD_DecodeResolution_eHD;
   aeVideoCmprStd[0] = BAVC_VideoCompressionStd_eVC1;
   rc = BXVD_P_GetDecodeFWMemSize(hXvd,
                                  eDecodeResolution,
                                  aeVideoCmprStd,
                                  1,
                                  &ChSettings,
                                  &stDecodeFWMemSize);

   hXvd->stFWMemCfg[BXVD_DecodeMode_eVC1_HD].video_block_size = stDecodeFWMemSize.uiFWPicBlockSize;
   hXvd->stFWMemCfg[BXVD_DecodeMode_eVC1_HD].video_block_count =  stDecodeFWMemSize.uiFWPicBlockCount;

   hXvd->stFWMemCfg[BXVD_DecodeMode_eVC1_HD].general_memory_size = stDecodeFWMemSize.uiFWContextSize + 
      stDecodeFWMemSize.uiFWDirectModeSize + stDecodeFWMemSize.uiFWInnerLoopWorklistSize;

   hXvd->stFWMemCfg[BXVD_DecodeMode_eVC1_HD].cabac_bin_size = 
      stDecodeFWMemSize.uiFWCabacSize + stDecodeFWMemSize.uiFWCabacWorklistSize;

   if (rc != BERR_SUCCESS)
   {
      return BERR_TRACE(rc);
   }

   /* BXVD_DecodeMode_eVC1_SD */
   eDecodeResolution = BXVD_DecodeResolution_eSD;
   aeVideoCmprStd[0] = BAVC_VideoCompressionStd_eVC1;
   rc = BXVD_P_GetDecodeFWMemSize(hXvd,
                                  eDecodeResolution,
                                  aeVideoCmprStd,
                                  1,
                                  &ChSettings,
                                  &stDecodeFWMemSize);

   hXvd->stFWMemCfg[BXVD_DecodeMode_eVC1_SD].video_block_size = stDecodeFWMemSize.uiFWPicBlockSize;
   hXvd->stFWMemCfg[BXVD_DecodeMode_eVC1_SD].video_block_count =  stDecodeFWMemSize.uiFWPicBlockCount;

   hXvd->stFWMemCfg[BXVD_DecodeMode_eVC1_SD].general_memory_size = stDecodeFWMemSize.uiFWContextSize + 
      stDecodeFWMemSize.uiFWDirectModeSize + stDecodeFWMemSize.uiFWInnerLoopWorklistSize;

   hXvd->stFWMemCfg[BXVD_DecodeMode_eVC1_SD].cabac_bin_size = 
      stDecodeFWMemSize.uiFWCabacSize + stDecodeFWMemSize.uiFWCabacWorklistSize;

   if (rc != BERR_SUCCESS)
   {
      return BERR_TRACE(rc);
   }

   /* BXVD_DecodeMode_eStill_HD */
   eDecodeResolution = BXVD_DecodeResolution_eHD;
   aeVideoCmprStd[0] = BAVC_VideoCompressionStd_eMPEG2;
   rc = BXVD_P_GetStillDecodeFWMemSize(hXvd,
                                       eDecodeResolution,
                                       aeVideoCmprStd,
                                       1,
                                       &stDecodeFWMemSize);

   hXvd->stFWMemCfg[BXVD_DecodeMode_eStill_HD].video_block_size = stDecodeFWMemSize.uiFWPicBlockSize;
   hXvd->stFWMemCfg[BXVD_DecodeMode_eStill_HD].video_block_count =  stDecodeFWMemSize.uiFWPicBlockCount;

   hXvd->stFWMemCfg[BXVD_DecodeMode_eStill_HD].general_memory_size = 
      stDecodeFWMemSize.uiFWContextSize + stDecodeFWMemSize.uiFWInnerLoopWorklistSize;

   /* Use reduced memory still footprint for compatibility table */
   hXvd->stFWMemCfg[BXVD_DecodeMode_eStill_HD].cabac_bin_size = stDecodeFWMemSize.uiFWCabacWorklistSize;

   if (rc != BERR_SUCCESS)
   {
      return BERR_TRACE(rc);
   }

   /* BXVD_DecodeMode_eStill_SD */
   eDecodeResolution = BXVD_DecodeResolution_eSD;
   aeVideoCmprStd[0] = BAVC_VideoCompressionStd_eMPEG2;
   rc = BXVD_P_GetStillDecodeFWMemSize(hXvd,
                                       eDecodeResolution,
                                       aeVideoCmprStd,
                                       1,
                                       &stDecodeFWMemSize);

   hXvd->stFWMemCfg[BXVD_DecodeMode_eStill_SD].video_block_size = stDecodeFWMemSize.uiFWPicBlockSize;
   hXvd->stFWMemCfg[BXVD_DecodeMode_eStill_SD].video_block_count =  stDecodeFWMemSize.uiFWPicBlockCount;

   hXvd->stFWMemCfg[BXVD_DecodeMode_eStill_SD].general_memory_size = 
      stDecodeFWMemSize.uiFWContextSize + stDecodeFWMemSize.uiFWInnerLoopWorklistSize;

   /* Use reduced memory still footprint for compatibility table */
   hXvd->stFWMemCfg[BXVD_DecodeMode_eStill_SD].cabac_bin_size = stDecodeFWMemSize.uiFWCabacWorklistSize;

   if (rc != BERR_SUCCESS)
   {
      return BERR_TRACE(rc);
   }

   /* BXVD_DecodeMode_eAll */
   /* Set the BXVD_DecodeMode_eAll entry to the decode mode that requires the most memory */
   for (eDecodeModeIndex = BXVD_DecodeMode_eAVC_HD; eDecodeModeIndex <= BXVD_DecodeMode_eStill_SD; eDecodeModeIndex++)
   {
      if ((hXvd->stFWMemCfg[eDecodeModeIndex].general_memory_size + 
           hXvd->stFWMemCfg[eDecodeModeIndex].cabac_bin_size + 
           (hXvd->stFWMemCfg[eDecodeModeIndex].video_block_size *
            hXvd->stFWMemCfg[eDecodeModeIndex].video_block_count)) >          
          (hXvd->stFWMemCfg[BXVD_DecodeMode_eAll].general_memory_size + 
           hXvd->stFWMemCfg[BXVD_DecodeMode_eAll].cabac_bin_size + 
           (hXvd->stFWMemCfg[BXVD_DecodeMode_eAll].video_block_size *
            hXvd->stFWMemCfg[BXVD_DecodeMode_eAll].video_block_count)))
      {
         hXvd->stFWMemCfg[BXVD_DecodeMode_eAll] = hXvd->stFWMemCfg[eDecodeModeIndex];
      }
   }
          
   for (eDecodeModeIndex = BXVD_DecodeMode_eTerminateList; eDecodeModeIndex <= BXVD_DecodeMode_eStill_SD; eDecodeModeIndex++)
   {
      BXVD_DBG_MSG(hXvd, ("Decode Mode %d: { %8d, %8d, %8d, %2d }",
                               eDecodeModeIndex,
                               hXvd->stFWMemCfg[eDecodeModeIndex].general_memory_size,
                               hXvd->stFWMemCfg[eDecodeModeIndex].cabac_bin_size,
                               hXvd->stFWMemCfg[eDecodeModeIndex].video_block_size,
                               hXvd->stFWMemCfg[eDecodeModeIndex].video_block_count));      
   }

   return BERR_TRACE(BERR_SUCCESS);
}

BERR_Code BXVD_P_InitChannel(BXVD_ChannelHandle  hXvdCh)
{
   BXDM_Decoder_Interface stDecoderInterface;
   void *pPrivateContext;

   BXVD_3DSetting st3DSettings;

   BXVD_Handle hXvd;

   BERR_Code rc;

   hXvd = hXvdCh->pXvd;

   /* Initialize channel settings */
   hXvdCh->eDecoderState = BXVD_P_DecoderState_eNotActive;
   hXvdCh->bPreserveState = false;

   hXvdCh->eCurrentSkipMode = BXVD_SkipMode_eDecode_IPB;

   rc = BXDM_PictureProvider_SetTimerHandle_isr(hXvdCh->hPictureProvider, hXvd->hTimer);

   if (rc != BERR_SUCCESS)
   {
      return BERR_TRACE(rc);
   }

   /* SW7425-1064: Initialize the XVD decoder.  Previously this was done in BXVD_Decoder_GetDMInterface.*/
   BXVD_Decoder_OpenChannel( hXvdCh );

   rc = BXVD_Decoder_GetDMInterface(
      hXvdCh,
      &stDecoderInterface,
      &pPrivateContext
      );

   if (rc != BERR_SUCCESS)
   {
      return BERR_TRACE(rc);
   }

   rc = BXDM_PictureProvider_SetDecoderInterface_isr(
      hXvdCh->hPictureProvider,
      &stDecoderInterface,
      pPrivateContext);

   if (rc != BERR_SUCCESS)
   {
   
      return BERR_TRACE(rc);
   }

   /* SW7422-72: set the default 3D behavior to support legacy MVC systems.
    * The original MVC behavior on the DVD chips was to set "pNext" in the MFD structure 
    * of frame 0 to point to frame 1.  By setting the default here, the older 
    * DVD platforms will get this behavior without having to change any code.
    */
   rc = BXVD_GetDefault3D( hXvdCh, &st3DSettings );

   if (rc != BERR_SUCCESS)
   {
      return BERR_TRACE(rc);
   }

   rc = BXVD_Set3D_isr( hXvdCh, &st3DSettings );

   if (rc != BERR_SUCCESS)
   {
      return BERR_TRACE(rc);
   }

   rc = BXVD_SetPictureDropMode_isr(hXvdCh, hXvdCh->sChSettings.ePictureDropMode );

   if (rc != BERR_SUCCESS)
   {
      return BERR_TRACE(rc);
   }

   rc = BXVD_SetPtsStcDiffThreshold_isr(hXvdCh, hXvdCh->sChSettings.uiVsyncDiffThreshDefault);

   if (rc != BERR_SUCCESS)
   {
      BXVD_CloseChannel(hXvdCh);
      return BERR_TRACE(rc);
   }

   rc = BXVD_SetMonitorRefreshRate_isr(hXvdCh, hXvdCh->sChSettings.ui32MonitorRefreshRate);

   if (rc != BERR_SUCCESS)
   {
      return BERR_TRACE(rc);
   }

   rc = BXVD_Set1080pScanMode_isr(hXvdCh, hXvdCh->sChSettings.e1080pScanMode);

   if (rc != BERR_SUCCESS)
   {
      return BERR_TRACE(rc);
   }

   return rc;
}


BERR_Code BXVD_P_DestroyInterrupts(BXVD_Handle hXvd)
{
   BXVD_DisplayInterrupt eDisplayInterrupt;

   BERR_Code rc;

   if (hXvd->stDecoderContext.pCbAVC_MBX_ISR)
   {
      rc = BINT_DestroyCallback(hXvd->stDecoderContext.pCbAVC_MBX_ISR);
      if (BERR_SUCCESS != rc)
      {
         return BERR_TRACE(rc);
      }
   }

   if (hXvd->stDecoderContext.pCbAVC_StillPicRdy_ISR)
   {
      rc = BINT_DestroyCallback(hXvd->stDecoderContext.pCbAVC_StillPicRdy_ISR);
      if (BERR_SUCCESS != rc)
      {
         return BERR_TRACE(rc);
      }
   }

   if (hXvd->stDecoderContext.pCbAVC_Watchdog_ISR)
   {
      rc = BINT_DestroyCallback(hXvd->stDecoderContext.pCbAVC_Watchdog_ISR);
      if (BERR_SUCCESS != rc)
      {
         return BERR_TRACE(rc);
      }
   }

   if (hXvd->stDecoderContext.pCbAVC_VICHReg_ISR)
   {
      rc = BINT_DestroyCallback(hXvd->stDecoderContext.pCbAVC_VICHReg_ISR);
      if (BERR_SUCCESS != rc)
      {
         return BERR_TRACE(rc);
      }
   }

   if (hXvd->stDecoderContext.pCbAVC_VICHSCB_ISR)
   {
      rc = BINT_DestroyCallback(hXvd->stDecoderContext.pCbAVC_VICHSCB_ISR);
      if (BERR_SUCCESS != rc)
      {
         return BERR_TRACE(rc);
      }
   }

   if (hXvd->stDecoderContext.pCbAVC_VICHInstrRd_ISR)
   {
      rc = BINT_DestroyCallback(hXvd->stDecoderContext.pCbAVC_VICHInstrRd_ISR);
      if (BERR_SUCCESS != rc)
      {
         return BERR_TRACE(rc);
      }
   }

#if BXVD_P_AVD_ARC600
   if (hXvd->stDecoderContext.pCbAVC_VICHILInstrRd_ISR)
   {
      rc = BINT_DestroyCallback(hXvd->stDecoderContext.pCbAVC_VICHILInstrRd_ISR);
      if (BERR_SUCCESS != rc)
      {
         return BERR_TRACE(rc);
      }
   }

   if (hXvd->bSVCCapable == true)
   {
      if (hXvd->stDecoderContext.pCbAVC_VICHBLInstrRd_ISR)
      {
         rc = BINT_DestroyCallback(hXvd->stDecoderContext.pCbAVC_VICHBLInstrRd_ISR);
         if (BERR_SUCCESS != rc)
         {
            return BERR_TRACE(rc);
         }
      }
   }
#endif

   if (hXvd->stDecoderContext.pCbAVC_StereoSeqError_ISR)
   {
      rc = BINT_DestroyCallback(hXvd->stDecoderContext.pCbAVC_StereoSeqError_ISR);
      if (BERR_SUCCESS != rc)
      {
         return BERR_TRACE(rc);
      }
   }

   for ( eDisplayInterrupt = 0; eDisplayInterrupt < BXVD_DisplayInterrupt_eMax; eDisplayInterrupt++ )
   {
      rc = BXVD_DisplayInterruptProvider_P_CloseChannel( hXvd->hXvdDipCh[eDisplayInterrupt] );
      if (rc != BERR_SUCCESS)
      {
         return BERR_TRACE(rc);
      }

      rc = BXDM_DisplayInterruptHandler_Destroy( hXvd->hXdmDih[eDisplayInterrupt] );
      if (rc != BERR_SUCCESS)
      {
         return BERR_TRACE(rc);
      }
   }

   return rc;
}

void BXVD_P_FreeXVDContext(BXVD_Handle hXvd)
{
   /* Destroy the FW command event */
   if (hXvd->stDecoderContext.hFWCmdDoneEvent)
   {
      BKNI_DestroyEvent(hXvd->stDecoderContext.hFWCmdDoneEvent);
   }

   /* Free the channel handle array */
   if (hXvd->ahChannel)
   {
      BKNI_Free(hXvd->ahChannel);
   }

   /* Free saved channel context structs */
   BXVD_P_FreeAllocatedChannelHandles(hXvd);

   /* Free the Still Picture Channel Handle */
   if (hXvd->hStillChannel)
   {
      BKNI_Free(hXvd->hStillChannel);
   }

   /* Free the multi-decode picture list */
   if (hXvd->pVDCPictureBuffers)
   {
      BKNI_Free(hXvd->pVDCPictureBuffers);
   }

   /* Free decode mode list (if allocated) */
   if (hXvd->bAllocDecodeModeList)
   {
      BKNI_Free(hXvd->stSettings.pDecodeModes);
      hXvd->stSettings.pDecodeModes = NULL;
      hXvd->bAllocDecodeModeList = false;
   }

   /* Destroy Timer */
   if (hXvd->hTimer)
   {
      BTMR_DestroyTimer(hXvd->hTimer);
   }

   /* Set eHandleType to invalid to prevent handle from being used again */
   hXvd->eHandleType = BXVD_P_HandleType_Invalid;

   /* Release the main context */
   BKNI_Free(hXvd);
}


BERR_Code BXVD_P_InitFreeChannelList(BXVD_Handle hXvd)
{
   uint32_t i;
   BXVD_ChannelHandle phXvdCh[BXVD_MAX_VIDEO_CHANNELS];

   BLST_S_INIT(&(hXvd->FreeChannelListHead));

   if ( hXvd->stSettings.uiNumPreAllocateChannels > BXVD_MAX_VIDEO_CHANNELS)
   {
      BXVD_DBG_ERR(hXvd, ("Number of pre-allocated channels greater than max video channels: %d", 
                          hXvd->stSettings.uiNumPreAllocateChannels));

      return (BERR_TRACE(BERR_INVALID_PARAMETER));
   }

   /* Pre-allocate channels if specified */ 
   for (i = 0; i < hXvd->stSettings.uiNumPreAllocateChannels; i++)
   {
      BXVD_P_GetChannelHandle(hXvd, &phXvdCh[i]);

      if (phXvdCh[i] == NULL)
      {
         return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
      }
   }

   /* Keep pre-allocaed channels */
   for (i = 0; i < hXvd->stSettings.uiNumPreAllocateChannels; i++)
   {
      BXVD_P_KeepChannelHandle(hXvd, phXvdCh[i]);
   }

   return BERR_SUCCESS;
}

void BXVD_P_GetChannelHandle(BXVD_Handle hXvd,
                             BXVD_ChannelHandle *phXvdCh)
{
   *phXvdCh = BLST_S_FIRST(&(hXvd->FreeChannelListHead));

   /* If free channel in list, use it. If not, allocate another one */
   if (*phXvdCh) 
   {
      BLST_S_REMOVE_HEAD(&(hXvd->FreeChannelListHead), link);
   }
   else 
   {
      *phXvdCh = (BXVD_P_Channel*)(BKNI_Malloc(sizeof(BXVD_P_Channel)));
   }
}

void BXVD_P_KeepChannelHandle(BXVD_Handle hXvd,
                              BXVD_ChannelHandle hXvdCh)
{
   /* Save channel handle to be used again */
   BLST_S_INSERT_HEAD(&(hXvd->FreeChannelListHead), hXvdCh, link);
}

void BXVD_P_FreeAllocatedChannelHandles(BXVD_Handle hXvd)
{
   BXVD_ChannelHandle hXvdCh;

   while ((hXvdCh = BLST_S_FIRST(&(hXvd->FreeChannelListHead))) != NULL)
   {
      BLST_S_REMOVE_HEAD(&(hXvd->FreeChannelListHead), link);

      /* Free channel handle */
      BKNI_Free(hXvdCh);
   }
}

#if BXVD_P_POWER_MANAGEMENT
#if !BCHP_PWR_RESOURCE_AVD0

void BXVD_P_GetHibernateState(BXVD_Handle hXvd, bool *bHibernateState)
{
   uint32_t uiRegVal;

   /* Assume clocks are on */
   *bHibernateState = false;

   /* Check to if PM reg pointers are NULL */
   if ( (hXvd->stPlatformInfo.stReg.uiClkGen_CoreClkCtrl == 0) &&
        (hXvd->stPlatformInfo.stReg.uiVCXO_AVDCtrl == 0) &&
        (hXvd->stPlatformInfo.stReg.uiClkGen_SCBClkCtrl == 0) &&
        (hXvd->stPlatformInfo.stReg.uiClkGen_GISBClkCtrl == 0) )
   {
      BXVD_DBG_WRN(hXvd, ("XVD Powermanagement is not supported on this platform!"));

      return;
   }

   /* Test each clock, if any one is off, set hibernate state to true. */

   uiRegVal = BXVD_Reg_Read32(hXvd, hXvd->stPlatformInfo.stReg.uiClkGen_CoreClkCtrl);

   if ( uiRegVal & hXvd->stPlatformInfo.stReg.uiClkGen_CoreClkCtrl_PwrDnMask)
   {
      *bHibernateState = true;
   }

   if (hXvd->stPlatformInfo.stReg.uiVCXO_AVDCtrl != (uint32_t)NULL)
   {
      uiRegVal = BXVD_Reg_Read32(hXvd, hXvd->stPlatformInfo.stReg.uiVCXO_AVDCtrl);

      if (uiRegVal & hXvd->stPlatformInfo.stReg.uiVCXO_AVDCtrl_PwrDnMask)
      {
         *bHibernateState = true;
      }
   }
   
   uiRegVal = BXVD_Reg_Read32(hXvd, hXvd->stPlatformInfo.stReg.uiClkGen_SCBClkCtrl);

   if ( uiRegVal & hXvd->stPlatformInfo.stReg.uiClkGen_SCBClkCtrl_PwrDnMask)
   {
      *bHibernateState = true;
   }

   uiRegVal = BXVD_Reg_Read32(hXvd, hXvd->stPlatformInfo.stReg.uiClkGen_GISBClkCtrl);

   if ( uiRegVal &hXvd->stPlatformInfo.stReg.uiClkGen_GISBClkCtrl_PwrDnMask)
   {
      *bHibernateState = true;
   }
}
#endif

void BXVD_P_SetHibernateState(BXVD_Handle hXvd, bool bHibernateState)
{

   if ((bHibernateState == false) && (hXvd->bHibernate == true))
   {
      BXVD_DBG_MSG(hXvd, ("SetHibernate(false), acquire power resource AVD%d", hXvd->uDecoderInstance));

      BXVD_P_SET_POWER_STATE(hXvd, BXVD_P_PowerState_eOn);

      hXvd->bHibernate = false;
   }
   else if ((bHibernateState == true) && (hXvd->bHibernate == false))
   {
      BXVD_DBG_MSG(hXvd, ("SetHibernate(true), release power resource AVD%d", hXvd->uDecoderInstance));

      BXVD_P_SET_POWER_STATE(hXvd, BXVD_P_PowerState_eClkOff);

      hXvd->bHibernate = true;
   }
}

#endif
