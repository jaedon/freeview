/***************************************************************************
 *     Copyright (c) 2004-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxvd_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/284 $
 * $brcm_Date: 10/8/12 4:57p $
 *
 * Module Description:
 *   See Module Overview below.
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/xvd/src/bxvd_priv.h $
 * 
 * Hydra_Software_Devel/284   10/8/12 4:57p davidp
 * SW7445-16: Add support for HEVC (H.265) Video decoding.
 * 
 * Hydra_Software_Devel/283   8/24/12 5:51p davidp
 * SW7425-3618: Replace uncached memory accesses with cached memory
 * accesses.
 * 
 * Hydra_Software_Devel/282   8/20/12 9:12a btosi
 * SW7405-4736: added wrappers for queue macros to aid with debug
 * 
 * Hydra_Software_Devel/281   7/9/12 10:03a btosi
 * SW7425-1064: fixed up comments
 * 
 * Hydra_Software_Devel/280   6/25/12 3:26p btosi
 * SW7425-1064: merged XMO changes from branch
 * 
 * Hydra_Software_Devel/279   6/25/12 2:40p btosi
 * SW7425-1064: merged XMO changes from branch
 * 
 * Hydra_Software_Devel/SW7425-1064/5   6/20/12 10:07a btosi
 * SW7425-1064: merging latest mainline changes
 * 
 * Hydra_Software_Devel/278   6/18/12 5:33p davidp
 * SW7435-164: Remove unneeded include file.
 * 
 * Hydra_Software_Devel/277   6/14/12 4:02p btosi
 * SW7425-3177: changed when the BAVC_VideoCompressionStd enum is mapped
 * to a BXVD_P_PPB_Protocol value
 * 
 * Hydra_Software_Devel/276   6/13/12 11:18a btosi
 * SW7425-3177: added support frame rates e10,e15,e20.  Reintroduced
 * BXVD_P_PPB_Protocol since the AVD and bavc.h protocol enums have
 * diverged.
 * 
 * Hydra_Software_Devel/275   5/7/12 6:23p davidp
 * SW7405-5655: If FW Dram logging previously started, restart during
 * watchdog processing.
 * 
 * Hydra_Software_Devel/274   4/19/12 5:22p davidp
 * SW7346-761: Add support routines to manage channel context free list.
 * 
 * Hydra_Software_Devel/273   2/16/12 6:12p davidp
 * SW7425-2419: Add BXVD_P_DestroyInterrupts().
 * 
 * Hydra_Software_Devel/272   2/9/12 7:57p davidp
 * SW7425-2284: Add BXVD_P_InitChannel to be used in BXVD_OpenChannel.
 * 
 * Hydra_Software_Devel/271   2/2/12 3:02p davidp
 * SW7425-2024: Validate decode protocol specified in BXVD_StartDecode is
 * supported on platform.
 * 
 * Hydra_Software_Devel/270   1/20/12 3:47p davidp
 * SW7208-175: Add support for 1920 portrait decode.
 * 
 * Hydra_Software_Devel/269   12/1/11 1:12p pblanco
 * SW7425-1780: Implemented picture id support for transcode userdata.
 * 
 * Hydra_Software_Devel/268   11/10/11 2:37p davidp
 * SW7425-1599: Add bSVCProgressiveOnly flag to BXVD_ChannelSettings.
 * 
 * Hydra_Software_Devel/267   10/20/11 3:21p davidp
 * SW7425-1564: Add support for SVC interlaced decode.
 * 
 * Hydra_Software_Devel/266   9/27/11 6:40p davidp
 * SW7340-279: Allocate Still Channel context in BXVD_Open.
 * 
 * Hydra_Software_Devel/265   7/28/11 3:35p davidp
 * SW7552-75: Add BCHP_GetFeature call to determins AVD core clock freq on
 * platforms where freq varies.
 * 
 * Hydra_Software_Devel/264   7/20/11 3:27p davidp
 * SW7231-127: Add BXVD_Standby and BXVD_Resume support.
 * 
 * Hydra_Software_Devel/263   6/27/11 4:13p davidp
 * SW7425-542: Merge devel branch to mainline.
 * 
 * Hydra_Software_Devel/SW7425-542/4   6/14/11 3:40p davidp
 * SW7425-542: Add pDisplayInfo pointer to Xvd Handle.
 * 
 * Hydra_Software_Devel/SW7425-542/3   6/9/11 3:38p davidp
 * SW7425-542: Remove unnecessary chip specific macro.
 * 
 * Hydra_Software_Devel/SW7425-542/2   6/9/11 1:19p btosi
 * SW7425-542: added in support for the Host Memory Interface
 * 
 * Hydra_Software_Devel/SW7425-542/1   5/31/11 3:59p davidp
 * SW7425-542: Add support for Host Interface Memory (ARC Scratch memory)
 * FW API changes.
 * 
 * Hydra_Software_Devel/262   3/25/11 3:56p davidp
 * SW7405-5101: Add checks to verify AVD code space, CDB/ITB and memory
 * used is in memory region below 768mb for ARC300 based cores.
 * 
 * Hydra_Software_Devel/261   12/9/10 5:13p davidp
 * SW7422-22: Fix SVC support, allocate FW memory for AVD.
 * 
 * Hydra_Software_Devel/260   10/27/10 5:32p davidp
 * SW7422-22: Add ARC600 boot callback support.
 * 
 * Hydra_Software_Devel/259   10/15/10 6:35p davidp
 * SW7425-16: Add support for base layer ARC, new memory config,
 * additional FW api memory arguments.
 * 
 * Hydra_Software_Devel/258   10/7/10 3:31p davidp
 * SW7422-22: Add BXVD private DDR3 constant definition.
 * 
 * Hydra_Software_Devel/257   10/6/10 2:00p davidp
 * SW7405-4923: Set XVD handle type to invalid when context is freed.
 * 
 * Hydra_Software_Devel/256   10/5/10 10:24a davidp
 * SW7405-4923: Resolve latest coverity run issues for non-debug builds.
 * 
 * Hydra_Software_Devel/255   8/3/10 2:03p btosi
 * SW7400-2870: added BXVD_[Set/Get]PlaybackRate(_isr)
 * 
 * Hydra_Software_Devel/254   8/3/10 9:06a pblanco
 * SW7400-2857: Reduced BXVD_Open memory footprint by 512 bytes by moving
 * FW command and reply buffers to the XVD handle structure.
 * 
 * Hydra_Software_Devel/253   3/24/10 3:00p davidp
 * SW7405-3137: Save and restore extended CDB read register address for
 * decode restart cases.
 * 
 * Hydra_Software_Devel/252   2/25/10 4:24p nilesh
 * SW7405-2993: XDM Merge
 * 
 * Hydra_Software_Devel/251   2/17/10 2:50p davidp
 * SW7468-74: Add support for IP licensing, save chip product revision
 * register address in xvd handle.
 * 
 * Hydra_Software_Devel/250   1/28/10 4:39p davidp
 * SW7550-173: Set AVD PCache mode register based on DDR type and memory
 * data width.
 * 
 * Hydra_Software_Devel/249   1/8/10 11:50a btosi
 * SW7405-3137: added support for extension data callback
 * 
 * Hydra_Software_Devel/248   1/6/10 2:28p btosi
 * SW7405-3137: merged branch into mainline
 * 
 * Hydra_Software_Devel/247   12/11/09 1:03p davidp
 * SW7405-3572: Change ChannelClose command argument to use ChannelHandle
 * instead of ChannelId.
 * 
 * Hydra_Software_Devel/246   12/1/09 9:48a btosi
 * SW7405-3245: added BXVD_DBG_* macros.  Map to either BDBG_INSTANCE_* or
 * BDBG_* at compile time.
 * 
 * Hydra_Software_Devel/245   11/30/09 4:31p btosi
 * SW7405-3245: added BXVD_DBG_* macros.  Map to either BDBG_INSTANCE_* or
 * BDBG_* at compile.
 * 
 * Hydra_Software_Devel/244   9/30/09 3:15p nilesh
 * SW7405-3085: Target source polarity synchronization now only occurs if
 * there are multiple active decodes targeted for the same display
 * interrupt.
 *
 * Hydra_Software_Devel/243   9/29/09 2:11p nilesh
 * SW7405-3085: The target source polarity for interlaced mosaic channels
 * on a progressive display is now synchronized
 *
 * Hydra_Software_Devel/242   9/16/09 5:00p davidp
 * SW3548-2458: Add BXVD_P_RESET_CORE macro to reset individual blocks in
 * AVD core.
 *
 * Hydra_Software_Devel/241   6/30/09 4:20p davidp
 * PR56374: Power down AVD related clocks in BXVD_Close.
 *
 * Hydra_Software_Devel/240   6/4/09 4:05p davidp
 * PR54107: Add AVD_PCacheRegVal to Xvd_handle to save/restore PCache
 * setting across resets.
 *
 * Hydra_Software_Devel/239   6/1/09 6:35p davidp
 * PR55198: Add dynamic power management support to most STB platforms.
 *
 * Hydra_Software_Devel/238   5/26/09 3:34p nilesh
 * PR52424: Added BXVD_Interrupt_eTSMResult callback and
 * BXVD_GetPPBParameterQueueInfo() function
 *
 * Hydra_Software_Devel/237   3/31/09 5:30p davidp
 * PR52344: Add bAVC51Enable flag to channel settings to use more memory
 * for AVC 5.1 compliant streams.
 *
 * Hydra_Software_Devel/236   3/26/09 6:13p nilesh
 * PR53580: Cleaned up DM structs
 *
 * Hydra_Software_Devel/235   3/12/09 11:32a nilesh
 * PR52898: bIgnoreCadenceMatch and eDisplayFieldMode are now sticky
 * parameters attached to the picture
 *
 * Hydra_Software_Devel/234   3/2/09 1:33p nilesh
 * PR52659: Changed DNR non-AVC inter default from 1024 to 964 as
 * documented in
 * http://www.sj.broadcom.com/projects/dvt/Chip_Architecture/Video/Releas
 * ed/DNR_bpe.doc
 *
 * Hydra_Software_Devel/233   2/23/09 2:40p btosi
 * PR52205: added BXVD_Interrupt_eDecodeError callback
 *
 * Hydra_Software_Devel/232   2/4/09 5:09p rayshap
 * PR51835: BXVD_GetGopTimeCode should make it obvious when no timecode is
 * present
 *
 * Hydra_Software_Devel/231   2/4/09 4:18p nilesh
 * PR51720: Frame advance now uses multiple field advances across
 * sequential vsyncs to keep the cadence fed to the MAD happy.
 *
 * Hydra_Software_Devel/230   2/4/09 2:18p pblanco
 * PR51740: Remove ulUserDataQueue member from BXVD_P_Channel structure.
 *
 * Hydra_Software_Devel/229   2/3/09 5:58p nilesh
 * PR51134: Improved DM output cadence during decoder trick mode
 * transitions
 *
 * Hydra_Software_Devel/228   1/27/09 1:49p nilesh
 * PR51621: Resolved possible global variable naming conflict by either
 * explicitly declaring globals as static (where possible) or adding
 * BXVD_P_ prefix
 *
 * Hydra_Software_Devel/227   1/14/09 3:04p btosi
 * PR50623: added uiSwPcrOffset and bUseHwPcrOffset to BXVD_P_TSMCommands
 *
 * Hydra_Software_Devel/226   12/19/08 12:04p btosi
 * PR50241: added support for "OpenChannel" initialization, cleaned up DM
 * API
 *
 * Hydra_Software_Devel/225   12/17/08 2:33p btosi
 * PR50241: removed the extraneous DM function declarations
 *
 * Hydra_Software_Devel/224   12/9/08 10:18a btosi
 * PR38467: enhanced comment for BXVD_DMInfo
 *
 * Hydra_Software_Devel/223   11/21/08 11:49a btosi
 * PR48843: added in support for uiPicturesReceivedCount
 *
 * Hydra_Software_Devel/222   11/20/08 5:12p davidp
 * PR49533: Add bPreserveCounters boolean to channel structure to not
 * decoder counters for certain startDecode situations.
 *
 * Hydra_Software_Devel/221   11/5/08 3:25p nilesh
 * PR48693: Added BTMR support
 *
 * Hydra_Software_Devel/220   10/28/08 12:30p davidp
 * PR47278: Add support for ExcessDirMode, allocate more direct memory for
 * certain AVC streams when flag is set.
 *
 * Hydra_Software_Devel/219   10/23/08 5:08p davidp
 * PR48295: Stripe width of 256 not currently supported. Do not allow QCIF
 * and CIF still picture decodes.
 *
 * Hydra_Software_Devel/218   10/9/08 11:02a nilesh
 * PR47494: Merged BXVD_Interrupt_ePPBParameters callback support
 *
 * Hydra_Software_Devel/PR47494/1   10/8/08 1:28p nilesh
 * PR47494: Added BXVD_Interrupt_ePPBParameters callback support
 *
 * Hydra_Software_Devel/217   10/8/08 11:25a nilesh
 * PR47692: RequestSTC now returns current coded PTS if re-enabled in the
 * middle of the clip
 *
 * Hydra_Software_Devel/216   10/6/08 11:52a btosi
 * PR46567: modified BXVD_DMInfo
 *
 * Hydra_Software_Devel/215   10/3/08 10:11a nilesh
 * PR47517: Timing Marker detection logic fixed to work with single PPB
 * clip decode.  Default AVD timing marker is now used as comparison with
 * first PPB.
 *
 * Hydra_Software_Devel/214   10/2/08 6:44p davidp
 * PR46543: Add power management support, hibernate field to XVD context.
 *
 * Hydra_Software_Devel/213   9/18/08 2:03p davidp
 * PR46650: Fix MemCfgMode enum field names.
 *
 * Hydra_Software_Devel/212   9/17/08 4:27p nilesh
 * PR43585: Fixed seamless clip handling
 *
 * Hydra_Software_Devel/211   9/8/08 6:42p davidp
 * PR46650: Add BXVD_P_GET_MEMORY_CONFIG macro and BXVD_P_ValidateHeaps to
 * sanity check heaps passed to BXVD_Open.
 *
 * Hydra_Software_Devel/210   8/5/08 3:56p nilesh
 * PR45322: BXVD_DMDBG_P_DumpYUV() no longer uses hard coded stripe width
 *
 * Hydra_Software_Devel/209   8/5/08 2:23p davidp
 * PR45080: Add support for AVD status block, block address now returned
 * in FW channel open command.
 *
 * Hydra_Software_Devel/208   8/1/08 4:57p davidp
 * PR43272: Merge PR43272 branch to mainline, new FW memory configuration
 * API.
 *
 * Hydra_Software_Devel/PR43272/1   7/17/08 12:36p davidp
 * PR43272: Add support for additional FW memory configuration parameters,
 * Direct mode and IL Work list memory.
 *
 * Hydra_Software_Devel/207   7/29/08 4:01p nilesh
 * PR37222: BXVD_GetNextPTS() returns an error if the next picture is not
 * available on the delivery queue
 *
 * Hydra_Software_Devel/206   7/29/08 3:01p nilesh
 * PR37222: Added BXVD_GetNextPTS() support
 *
 * Hydra_Software_Devel/205   7/29/08 12:41p pblanco
 * PR45230: Fixed typo in userdata context structure.
 *
 * Hydra_Software_Devel/204   7/29/08 11:22a pblanco
 * PR45230: Added forward error member to multidecode version of userdata
 * context structure.
 *
 * Hydra_Software_Devel/203   7/24/08 12:20p btosi
 * PR39962: added BXVD_DMLocalPTSInfo to aid in cleaning up PTSInfo
 * structure
 *
 * Hydra_Software_Devel/202   7/8/08 1:49p btosi
 * PR38467: added support for PPBReceived callback
 *
 * Hydra_Software_Devel/201   7/1/08 4:44p nilesh
 * PR43585: Update to allow bxvd_dispmgr_dvd.c to compile
 *
 * Hydra_Software_Devel/200   7/1/08 2:31p nilesh
 * PR43585: Merge DVD Development branch to mainline
 *
 * Hydra_Software_Devel/PR43585/3   6/26/08 11:09a nilesh
 * PR43585: ClipStart event now only occurs if the current clip has
 * actually started displaying
 *
 * Hydra_Software_Devel/PR43585/2   6/23/08 8:23p nilesh
 * PR43585: Add Marker valid bit to handle marker transition during start-
 * up
 *
 * Hydra_Software_Devel/PR43585/1   6/20/08 5:04p nilesh
 * PR43585: Added initial clip support (eRequestSTC, eClipStart,
 * eClipStop, and ePictureMarker)
 *
 * Hydra_Software_Devel/199   6/12/08 5:30p nilesh
 * PR43585: Updated multi-decode XVD to compile with 7440
 *
 * Hydra_Software_Devel/198   5/21/08 1:51p pblanco
 * PR42910: Added support for handle types and moved the contents of
 * bxvd_userdata_priv.h to this module."
 *
 * Hydra_Software_Devel/197   5/8/08 3:29p nilesh
 * PR38467: Merge DMv2 to mainline
 *
 * Hydra_Software_Devel/196   4/11/08 4:53p davidp
 * PR40234: Merge decoder debug logging from branch to
 * Hydra_Software_Devel/.
 *
 * Hydra_Software_Devel/PR40234/1   4/10/08 3:37p davidp
 * PR40234: Add support for decoder debug logging.
 *
 * Hydra_Software_Devel/195   4/3/08 2:17p nilesh
 * PR41276: Fixed possible out of bounds condition for StripeWidth and
 * StripeMultiple
 *
 * Hydra_Software_Devel/194   4/1/08 2:42p davidp
 * PR27168: Remove FW get version command.
 *
 * Hydra_Software_Devel/193   3/21/08 3:45p btosi
 * PR40010: for DQT, revamped the handling of GOP "tails"
 *
 * Hydra_Software_Devel/192   3/18/08 3:30p btosi
 * PR40009: removed iReleaseStallCnt from BXVD_P_DQTContext
 *
 * Hydra_Software_Devel/191   3/7/08 10:38a nilesh
 * PR40349: Update memory tables according to AVD Mosaic API doc v2.19
 *
 * Hydra_Software_Devel/190   1/29/08 6:47p davidp
 * PR39098: Add PFRI data register item to xvd handle to be saved and
 * restore on core rev i0.
 *
 * Hydra_Software_Devel/189   1/24/08 11:40a nilesh
 * PR38570: Merge CABAC worklist API changes to mainline
 *
 * Hydra_Software_Devel/PR38570/1   1/21/08 5:12p nilesh
 * PR38570: Added support for cabac worklist buffer and size parameter to
 * AVD ChannelOpen
 *
 * Hydra_Software_Devel/188   1/15/08 5:21p davidp
 * PR38735: Add BXVD_LinkDecoders/UnlinkDecoders and associated data
 * structures.
 *
 * Hydra_Software_Devel/187   1/8/08 2:35p pblanco
 * PR38593: Added support for AVS userdata handling
 *
 * Hydra_Software_Devel/186   11/8/07 10:49a btosi
 * PR32273: added support bIgnorePcrDiscontinuity
 *
 * Hydra_Software_Devel/185   11/6/07 1:38p btosi
 * PR30310: added BXVD_P_AVD_NO_CONSTRAINTS
 *
 * Hydra_Software_Devel/184   11/6/07 12:36p nilesh
 * PR36741: Added XVD status reporting via BDBG console messages
 *
 * Hydra_Software_Devel/183   11/6/07 10:50a nilesh
 * PR36735: XVD : Need to add support for Adaptive System Time Management
 * (ASTM)
 *
 * Hydra_Software_Devel/182   10/22/07 10:28a btosi
 * PR30130: merged DQT constants and structures into mainline
 *
 * Hydra_Software_Devel/181   9/26/07 3:20p parijat
 * PR35190: Need to add generic fix for PPB leaks in DM
 *
 * Hydra_Software_Devel/180   9/21/07 10:33a btosi
 * PR34980: added uiVsyncCount to BXVD_DMInfo
 *
 * Hydra_Software_Devel/179   8/30/07 12:21p nilesh
 * PR34430: Channel specific heaps are now used for address conversion
 *
 * Hydra_Software_Devel/178   8/29/07 1:25p rayshap
 * PR33893: Add support for conditional display manager trickmode
 * transition
 *
 * Hydra_Software_Devel/177   8/29/07 1:08p nilesh
 * PR34430: Added support for channel specific memory heaps
 *
 * Hydra_Software_Devel/176   8/22/07 3:49p nilesh
 * PR29915: The FW memory config table for compatibility mode is now
 * generated on-the-fly at runtime
 *
 * Hydra_Software_Devel/175   8/21/07 3:04p davidp
 * PR34052: Remove nolonger used register addr pointers from BXVD_Handle,
 * use platform_info.stReg structure to reference registers
 *
 * Hydra_Software_Devel/174   8/20/07 4:02p nilesh
 * PR29915: Fixed secure heap memory allocation
 *
 * Hydra_Software_Devel/173   8/14/07 10:01p parijat
 * PR32701: Implement & test 1080p pass-thru on Brutus
 *
 * Hydra_Software_Devel/172   8/13/07 4:08p nilesh
 * PR29915: Multi-decode merge to mainline
 *
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/24   8/9/07 3:50p nilesh
 * PR29915: Updated memory tables according to AVD Mosaic Doc v2.11 (Aug
 * 9, 2007)
 *
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/23   7/26/07 4:00p nilesh
 * PR29915: Add MPEG2 w/o BTP support
 *
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/22   7/25/07 1:37p nilesh
 * PR29915: Cleaned up bxvd_priv.h and bxvd_vdec_info.h constants to match
 * XVD coding style
 *
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/20   7/20/07 2:22p pblanco
 * PR29915: Changed value of bottom field first flag from 0x0080 to 0x0010
 * to refct its value in bxvd_vdec_info.h
 *
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/19   7/19/07 5:17p nilesh
 * PR29915: Fixed stripe_width value in still picture buffer
 *
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/18   7/19/07 5:14p nilesh
 * PR29915: Update BXVD_GetBufferConfig() and BXVD_P_ParseDecodeMode() to
 * handle BXVD_DecodeMode_eCustom
 *
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/17   7/13/07 5:15p nilesh
 * PR29915: Updated memory tables based on v2.9 of Mosaic API doc
 *
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/16   7/10/07 6:18p nilesh
 * PR29915: Added 7405 support to mosaic branch
 *
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/15   7/10/07 10:41a nilesh
 * PR29915: Merged from mainline 07/10/2007
 *
 * Hydra_Software_Devel/167   6/28/07 5:58p vijeth
 * PR 32432: Enhance get channel status
 *
 * Hydra_Software_Devel/166   6/22/07 9:30a btosi
 * PR30310: added "bReversePlayback" to "BXVD_P_PVRCommands"
 *
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/14   7/3/07 3:47p nilesh
 * PR29915: bxvd_priv.h cleanup
 *
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/13   7/3/07 12:09p nilesh
 * PR29915: Added FGT support
 *
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/12   6/28/07 1:36p davidp
 * PR29915: Move remaining data structure definitions for
 * bxvd_devcmds_priv.h to bxvd_vdec_api.h
 *
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/11   6/19/07 12:55p nilesh
 * PR29915: BXVD_Settings decode mode list is now copied locally
 *
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/10   6/19/07 12:13p nilesh
 * PR29915: Merge from mainline
 *
 * Hydra_Software_Devel/165   6/12/07 12:37p rayshap
 * PR31988: Add support for enable/disable of 3-2 pulldown
 *
 * Hydra_Software_Devel/164   6/4/07 7:44p parijat
 * PR31700: Video jitter at stream wrap when TSM is enabled (does not
 * happen with 7411)
 *
 * Hydra_Software_Devel/163   5/23/07 12:43p nilesh
 * PR29874: Consolidate TSM threshold code
 *
 * Hydra_Software_Devel/162   5/23/07 5:42p parijat
 * PR30755: Problem decoding AVC stream A3VID12_151.btz, looks like
 * single-field
 *
 * Hydra_Software_Devel/161   5/22/07 5:16p parijat
 * PR30242: (According to latest AVD PPB doc, DM will get valid progSeq
 * values from AVD firmware for all algorithms except AVC & the value
 * will be derived as per the document from apt ppb fields. So, the fix
 * is not required for algorithms other than AVC. For AVC, even progFrame
 * info is in general not available. So, the fix doesn't have relevance
 * for AVC.)
 *
 * Hydra_Software_Devel/160   5/21/07 7:13p vijeth
 * PR 31433 : Frame Forward for 3:2 pulldown stream
 *
 * Hydra_Software_Devel/159   5/17/07 5:03p parijat
 * PR30011: Frame rewind to frame advance to play transition can cause
 * video to freeze on AVC
 *
 * Hydra_Software_Devel/158   5/15/07 1:19p pblanco
 * PR28215: Added support for variable stripe width and multiple.
 *
 * Hydra_Software_Devel/157   5/14/07 11:30a nilesh
 * PR30249: Merged UOD 2.x changes to mainline
 *
 * Hydra_Software_Devel/156   5/3/07 10:03p parijat
 * PR30242: DM needs to derive "progressive sequence" info using
 * progressive frame info for algorithms other than MPEG & VC1
 *
 * Hydra_Software_Devel/155   5/2/07 2:55p vijeth
 * PR 30382: Marker represents the Beginning of a Clip and not the end.
 *
 * Hydra_Software_Devel/154   4/24/07 2:52p rayshap
 * PR30086: Increase VC1 discard threshold to 10 seconds
 *
 * Hydra_Software_Devel/153   4/24/07 1:39p nilesh
 * PR22766: Added support for new FW API to provide FGT buffer address and
 * size
 *
 * Hydra_Software_Devel/152   4/11/07 7:07p vijeth
 * PR 29434 : Display Manager - NF002 disk - Simple Seemless Playback test
 * fails - drops pictures
 *
 * Hydra_Software_Devel/151   4/11/07 12:15p parijat
 * PR27835: Removed static variable & added eSourcePolarityPrev to
 * stDMContextState structure
 *
 * Hydra_Software_Devel/150   4/11/07 12:01p parijat
 * PR28082: Changed bFirstPPBForCurrentChannelHasBeenDetected from static
 * to state variable
 *
 * Hydra_Software_Devel/149   3/26/07 1:54p pblanco
 * PR27168: Added new constant definitions.
 *
 * Hydra_Software_Devel/148   3/22/07 11:51a pblanco
 * PR28629: Added GOP timecode mask and shift constants.
 *
 * Hydra_Software_Devel/147   3/20/07 12:09p pblanco
 * PR28629: Added support for BXVD_GetGopTimeCode.
 *
 * Hydra_Software_Devel/146   3/6/07 3:29p parijat
 * PR28082: XVD : DM in PI : New callback which provides picture sizes
 * immediately after channel open/change, required
 *
 * Hydra_Software_Devel/145   3/2/07 1:36p nilesh
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
 * Hydra_Software_Devel/144   2/6/07 11:08a nilesh
 * PR27575: Fix 7440 Watchdog and make watchdog more robust on all
 * platforms.
 *
 * Hydra_Software_Devel/PR27575/1   2/2/07 2:12p nilesh
 * PR27575: Fix 7440 Watchdog
 *
 * Hydra_Software_Devel/142   1/23/07 1:40p pblanco
 * PR27283: New UART initialization support.
 *
 * Hydra_Software_Devel/141   12/29/06 5:25p vijeth
 * PR 26847: XVD : DM in PI : Wrong #defines used for couple of DNR
 * parameters for AVC
 *
 * Hydra_Software_Devel/140   12/13/06 7:39p davidp
 * PR25443: Remove deprecated data structures.
 *
 * Hydra_Software_Devel/139   12/12/06 3:00p davidp
 * PR25443: Merge Simplified FW API branch into mainline
 *
 * Hydra_Software_Devel/138   12/6/06 11:11a pblanco
 * PR26199: Altered change based on Gaurav's input. There is now a
 * separate define for VC1 streams (4 secs.) and MPEG is back to 2 secs.
 *
 * Hydra_Software_Devel/137   12/6/06 10:21a pblanco
 * PR26199: Changed mpeg discard threshold from 2 to 4 seconds to
 * accomodate vc1 streams.
 *
 * Hydra_Software_Devel/136   11/17/06 4:50p nilesh
 * PR25868: Merged authenticated image support to mainline
 *
 * Hydra_Software_Devel/PR25868/2   11/16/06 3:38p nilesh
 * PR25868: Added authenticated image support
 *
 * Hydra_Software_Devel/PR25868/1   11/14/06 4:01p nilesh
 * PR25868: Added pre-relocated image support
 *
 * Hydra_Software_Devel/135   11/13/06 9:32p vijeth
 * PR 19436 : IFrameDetect needed for HITS
 *
 * Hydra_Software_Devel/134   11/13/06 7:18p vijeth
 * PR 24597: Input / Output frame rate mismatch causes endless XVD phase
 * interrupts
 *
 * Hydra_Software_Devel/133   11/9/06 4:43p davidp
 * PR25698: Explicitly save XPT Rave CDB_Read register address being used
 * for current decode to be restored during watchdog processing.
 *
 * Hydra_Software_Devel/132   11/3/06 6:35p vijeth
 * PR 24492 : During transition from Pause -> Play there is a Jump in the
 * Video
 *
 * Hydra_Software_Devel/131   10/19/06 7:16p vijeth
 * PR 23237 : 7440: Add DisplayMgr support
 *
 * Hydra_Software_Devel/130   10/17/06 11:22p vijeth
 * PR 23237 : 7440: Add DisplayMgr support
 *
 * Hydra_Software_Devel/DM7440/4   10/17/06 9:25p vijeth
 * PR 23237 : 7440: Add DisplayMgr support
 *
 * Hydra_Software_Devel/DM7440/3   10/16/06 2:09p vijeth
 * PR 23237 : 7440: Add DisplayMgr support
 *
 * Hydra_Software_Devel/DM7440/2   10/11/06 11:44a vijeth
 * PR 23237 : 7440: Add DisplayMgr support
 *
 * Hydra_Software_Devel/DM7440/1   10/9/06 2:45p vijeth
 * PR 23237: 7440: Add DisplayMgr support ( Making changes)
 *
 * Hydra_Software_Devel/129   10/2/06 3:00p gaurava
 * PR 24597: Do not enable DM Logging because it kills the system.
 *
 * Hydra_Software_Devel/128   9/29/06 3:40p vijeth
 * PR 24597 : Input / Output frame rate mismatch causes endless XVD phase
 * interrupts
 *
 * Hydra_Software_Devel/127   9/26/06 9:54p vijeth
 * PR 24559: Display Manager should not consider the first picture after
 * ChannelOpen as late for performance measurements
 *
 * Hydra_Software_Devel/126   9/22/06 2:21p davidp
 * PR22867: Update FW memory usage table, add secure memory heap support.
 *
 * Hydra_Software_Devel/125   9/5/06 3:05p davidp
 * PR22867: Merge secure heap (Cabac) support
 *
 * Hydra_Software_Devel/xvd_cabac_heap/1   8/30/06 5:10p davidp
 * 22867: Separate FW memory so Cabac bin memory can me moved to secure
 * heap.
 *
 * Hydra_Software_Devel/124   8/9/06 3:06p davidp
 * PR20394: PCR TimeBase decoder setting now assigned a default value in
 * BXVD_OpenChannel.
 *
 * Hydra_Software_Devel/123   8/3/06 7:42p parijat
 * PR 21703: video output stuttering when input stream is 50Hz and output
 * is 60Hz
 *
 * Hydra_Software_Devel/122   8/1/06 5:58p davidp
 * PR22673: Add verify watchdog fired platform routine.
 *
 * Hydra_Software_Devel/121   7/26/06 4:50p davidp
 * PR22673: Cleanup more platform specific code, add hHWMemHeap element in
 * Xvd handle.
 *
 * Hydra_Software_Devel/120   7/20/06 2:22p nilesh
 * PR22673: Created BXVD_P_CONTEXT_PLATFORM macro to allow platform-
 * specific fields in BXVD_P_Context struct
 *
 * Hydra_Software_Devel/119   7/19/06 2:26p pblanco
 * PR22673: Moved conditional defines from bxvd_priv.h to the appropriate
 * platformspecific header files.
 *
 * Hydra_Software_Devel/118   7/18/06 12:11a nilesh
 * PR22673: Code restructure for 97400, 97401 B0, and 97118
 *
 * Hydra_Software_Devel/PR22673/2   7/17/06 2:51p nilesh
 * PR22673: Restructure on 97400
 *
 * Hydra_Software_Devel/PR22673/1   7/17/06 2:34p nilesh
 * PR22673: Restructure on 97401
 *
 * Hydra_Software_Devel/117   7/14/06 10:21a pblanco
 * PR21703: Also 22389 and 21580... 50Hz -> 60Hz
 *
 * Hydra_Software_Devel/116   7/13/06 4:56p davidp
 * PR22673: Add BXVD_P_SetWatchdog_7401B0 routine, remove some 7401A0
 * support.
 *
 * Hydra_Software_Devel/115   7/12/06 4:48p davidp
 * PR20017: Cleanup BXVD_Open, add platform specific setup routines
 *
 * Hydra_Software_Devel/114   7/11/06 5:06p davidp
 * PR20017: Add FW inner loop Watchdog support, also add platform specific
 * SetupInterrupts routines
 *
 * Hydra_Software_Devel/113   7/6/06 9:40a pblanco
 * PR21943: Fixed conditional compilation for 7401 B0, broken when
 * conditionalizing for 7118.
 *
 * Hydra_Software_Devel/112   7/5/06 3:20p pblanco
 * PR21943: Conditionalization for 7118
 *
 * Hydra_Software_Devel/111   5/31/06 11:01a vijeth
 * PR 21144: XVD to tell VDC a field repeat
 *
 * Hydra_Software_Devel/110   5/26/06 1:54p davidp
 * PR21740: Store userdata context in channel context, convert userdata
 * addr using private memory heap info.
 *
 * Hydra_Software_Devel/109   5/25/06 3:10p pblanco
 * PR20017: Reloc sanity check in.
 *
 * Hydra_Software_Devel/108   5/24/06 1:20p pblanco
 * PR20017: Decreased inner loop address offset from 0x000c0000 to
 * 0x000a0000.
 *
 * Hydra_Software_Devel/107   5/24/06 11:15a pblanco
 * PR20017: Added a defined constant for inner loop address offset.
 *
 * Hydra_Software_Devel/106   5/24/06 10:00a pblanco
 * PR20017: Added inner and outer loop end-of-code members to XVD context
 * structure.
 *
 * Hydra_Software_Devel/105   5/11/06 6:08p davidp
 * PR21532: Created new BXVD_[Un]InstallDeviceInterruptCallback to handle
 * device instance interrupts for DecodeStillPicture and Watchdog.
 *
 * Hydra_Software_Devel/104   5/10/06 11:01a pblanco
 * PR19877: Removed unused host command prototypes identified by Gaurav.
 *
 * Hydra_Software_Devel/103   5/5/06 3:29p davidp
 * PR20079: Reduce 7400 FW shared memory from 16mb to 8mb per decoder.
 *
 * Hydra_Software_Devel/102   5/5/06 2:10p davidp
 * PR21156: Add BXVD_P_ReleasePPB_isr and related _isr routines for DM to
 * use at ISR time.
 *
 * Hydra_Software_Devel/101   5/4/06 11:32p vijeth
 * PR 17879 : Support new DNR feature in XVD
 *
 * Hydra_Software_Devel/Test_PR17879_SupportDNR/1   5/3/06 9:44p vijeth
 * PR 17879 : Support new DNR feature in XVD
 *
 * Hydra_Software_Devel/100   4/24/06 2:11p davidp
 * PR18043: Add support for Sequence Header interrupt processing
 *
 * Hydra_Software_Devel/99   4/18/06 11:04a pblanco
 * PR19123: Use calculated values for page register initialization.
 *
 * Hydra_Software_Devel/98   4/17/06 6:27p davidp
 * PR20916: Save and restore channelChangeMode during disableForFlush and
 * Flush processing
 *
 * Hydra_Software_Devel/97   4/17/06 3:55p pblanco
 * PR19123: Added definitions for page register initialization values.
 *
 * Hydra_Software_Devel/96   4/14/06 2:17p davidp
 * PR19600: Move 7400 AVD FW load address to 62MB
 *
 * Hydra_Software_Devel/95   4/12/06 12:50p davidp
 * PR19600: Move 7401 FW load address to 48mb, also decrease size to 1mb
 *
 * Hydra_Software_Devel/94   4/6/06 3:52p pblanco
 * PR19877: Removed unused members from private channel context structure.
 *
 * Hydra_Software_Devel/93   4/6/06 12:59p pblanco
 * PR20625: Removed the now unused bUserMute flag from the channel context
 * structure.
 *
 * Hydra_Software_Devel/92   4/5/06 2:09p pblanco
 * PR15434: Changed channel open formal parameter from Port to
 * RaveEndianess.
 *
 * Hydra_Software_Devel/91   3/29/06 2:28p pblanco
 * PR19099: Added separate discard threshold defines for AVC and MPEG.
 *
 * Hydra_Software_Devel/90   3/29/06 2:08p pblanco
 * PR19877: Added BXVD_P_GetSeqHdr function prototype.
 *
 * Hydra_Software_Devel/89   3/29/06 12:45p vijeth
 * PR 19284: add BXVD_Interrupt_eNewPtsStcOffset for higher precision
 * lipsync
 *
 * Hydra_Software_Devel/89   3/29/06 12:43p vijeth
 * PR 19284: add BXVD_Interrupt_eNewPtsStcOffset for higher precision
 * lipsync
 *
 * Hydra_Software_Devel/89   3/29/06 12:42p vijeth
 * PR 19284: add BXVD_Interrupt_eNewPtsStcOffset for higher precision
 * lipsync
 *
 * Hydra_Software_Devel/88   3/28/06 6:14p davidp
 * PR20353: Add decoder private memory base address to luma/chroma buffer
 * address being returned to app.
 *
 * Hydra_Software_Devel/87   3/27/06 6:35p davidp
 * PR20353: Add Decode Still Picture support for 7400
 *
 * Hydra_Software_Devel/86   3/27/06 4:22p pblanco
 * PR19877: Added address for vdec status block to private channel context
 * structure. This is used to get GOP time codes sequence header.
 *
 * Hydra_Software_Devel/85   3/27/06 3:13p pblanco
 * PR19877: Removed unused member declaration of type BXVD_VideoParams.
 *
 * Hydra_Software_Devel/84   3/24/06 1:35p pblanco
 * PR20274: Interim checkin prior to FW implementation.
 *
 * Hydra_Software_Devel/83   3/23/06 8:57a pblanco
 * PR19877: Added bPreserveState to private channel context data.
 *
 * Hydra_Software_Devel/82   3/22/06 5:16p davidp
 * PR20347: Add L2 interrupt support
 *
 * Hydra_Software_Devel/81   3/21/06 5:20p vijeth
 * PR 20202: Need initial removal delay option for elementary stream
 * playback
 * PR 20243: XVD DM should print message when vsync interrupt is missed
 *
 * Hydra_Software_Devel/80   3/15/06 4:09p pblanco
 * PR20197: The channel context structure contained a member that was
 * incorrectly being set for display field mode. The correct member is in
 * the channel state structure. The incorrect member has been removed to
 * eliminate future confusion.
 *
 * Hydra_Software_Devel/79   3/6/06 1:21p pblanco
 * PR16785: Changed skip mode command prototype to use new skip mode enum.
 *
 * Hydra_Software_Devel/78   2/28/06 4:36p davidp
 * PR18906: Pass proper field polarity/frame value to VDC for
 * PictureDataRdy interrupt regardless if decode is in progress.:
 *
 * Hydra_Software_Devel/77   2/28/06 10:31a pblanco
 * PR19877: Additional structure and code cleanup.
 *
 * Hydra_Software_Devel/76   2/22/06 11:37a pblanco
 * PR19785: Re-checked in to fix ClearCase 0 length file problem.
 *
 * Hydra_Software_Devel/74   2/21/06 1:46p pblanco
 * PR19785: Implemented BXVD_SetSkipPictureMode API. This also needs FW
 * support.
 *
 * Hydra_Software_Devel/73   2/17/06 3:17p davidp
 * PR16792: Modify FW API for Still Picture Decode, feature now working:
 *
 * Hydra_Software_Devel/72   2/15/06 1:56p pblanco
 * PR18545: Extended user data types for 7401/7400 in the same manner Mai
 * did for 7411.
 *
 * Hydra_Software_Devel/71   2/14/06 1:11p pblanco
 * PR19123: Moved static data from bxvd.c interrupt routines to XVD
 * context structure.
 *
 * Hydra_Software_Devel/70   2/2/06 4:04p pblanco
 * PR19123: More code cleanup. Changed all instances of uAvcInstance to
 * uDecoderInstance and removed last remaining references to the old
 * decoder context array.
 *
 * Hydra_Software_Devel/69   2/1/06 3:54p pblanco
 * PR19123: Changed "AVC" instance array in XVD context structure to a
 * single instance of type BXVD_DecoderContext. Tested against 7401 with
 * no regressions and against 7400 with no regressions.
 *
 * Hydra_Software_Devel/68   1/31/06 1:35p pblanco
 * PR19123: Make sure there are two AVC instance structures for 7400.
 *
 * Hydra_Software_Devel/67   1/30/06 1:40p pblanco
 * PR19123: Fixed function prototype for trick mode enable.
 *
 * Hydra_Software_Devel/66   1/30/06 8:58a pblanco
 * PR19308: Changed function prototype for
 * BXVD_P_HostCmdDecEnableBRCMTrickPlay to take an unsigned long value
 * instead of a boolean.
 *
 * Hydra_Software_Devel/65   1/26/06 4:03p davidp
 * PR19123: Remove printf debug messages, clean up compiler warnings:
 *
 * Hydra_Software_Devel/64   1/25/06 6:52p davidp
 * PR19123: Add platform specific definition of FS_MAP_SIZE:
 *
 * Hydra_Software_Devel/63   1/24/06 2:51p pblanco
 * PR19123: 7400 firmware bring up debugging changes.
 *
 * Hydra_Software_Devel/62   1/20/06 9:31a pblanco
 * PR19123: Change Reset7401 to the more generic Reset740x.
 *
 * Hydra_Software_Devel/61   1/16/06 4:38p pblanco
 * PR19099: Added default discard threshold value definition.
 *
 * Hydra_Software_Devel/60   1/16/06 1:27p davidp
 * PR16792: BXVD_Open now uses two heap pointers, one for FW code (2MB)
 * the other for FW picture buffers.:
 *
 * Hydra_Software_Devel/59   1/10/06 11:17a pblanco
 * PR16052: Added function prototype for
 * BXVD_P_HostCmdDecEnableBRCMTrickPlay.
 *
 * Hydra_Software_Devel/58   1/3/06 10:51a pblanco
 * PR16052: Added BXVD_PVR_BTPMode member to BXVD_P_PVRCommands.
 *
 * Hydra_Software_Devel/57   12/23/05 6:50p davidp
 * PR16878: Reset Decoder in Watchdog ISR, add BXVD_Open setting for
 * watchdog timer value, pass WD info to FW:
 *
 * Hydra_Software_Devel/56   12/21/05 5:59p davidp
 * PR16052: Add Still picture support:
 *
 * Hydra_Software_Devel/55   12/12/05 8:19p davidp
 * PR16052: Add PTS related callbacks:
 *
 * Hydra_Software_Devel/54   12/12/05 3:09p pblanco
 * PR16052: More still picture internals (sanity checkin)
 *
 * Hydra_Software_Devel/53   12/9/05 12:28p pblanco
 * PR16052: Added prototype for still picture decode host command.
 *
 * Hydra_Software_Devel/52   12/7/05 5:26p davidp
 * PR16878: Add XVD Watchdog routines:
 *
 * Hydra_Software_Devel/51   12/2/05 12:32p pblanco
 * PR16052: Removed BXVD_FilmTypeDetectionMode from referring structure.
 *
 * Hydra_Software_Devel/50   12/2/05 9:58a pblanco
 * PR16052: Moved BXVD_ReleasePPB to BXVD_P_ReleasePPB.
 *
 * Hydra_Software_Devel/49   12/1/05 1:44p pblanco
 * PR16052: Changed definition of BXVD_P_UserData so that the forward
 * pointer is a
 * unsigned long.
 *
 * Hydra_Software_Devel/48   11/28/05 2:30p pblanco
 * PR16052: Added BXVD_IS_MPEG and BXVD_IS_AVC macros.
 *
 * Hydra_Software_Devel/47   11/22/05 1:15p pblanco
 * PR16052: Added new user data related defines and enums.
 *
 * Hydra_Software_Devel/46   11/18/05 9:19a pblanco
 * PR16052: Added PPB field polarity flag mask define.
 *
 * Hydra_Software_Devel/45   11/15/05 11:14a pblanco
 * PR16052: Removed old queueing code. User data will implement a private
 * version.
 *
 * Hydra_Software_Devel/44   11/11/05 9:47a pblanco
 * PR16052: Added user data queue pointer to channel context.
 *
 * Hydra_Software_Devel/43   11/10/05 8:52a pblanco
 * PR17567: Adding correct display override to XVD.
 *
 * Hydra_Software_Devel/42   11/10/05 7:53a pblanco
 * PR16052: FW now uses heap info to determine load and execution
 * addresses. Also removed unused code that existed when XVD used to
 * create and map its own heap. Corresponding changes were made in
 * related modules also checked in at this time.
 *
 * Hydra_Software_Devel/41   11/4/05 3:54p davidp
 * PR16052: Add call to BXVD_P_ReleasePPBonCloseChannel to StopDecode:
 *
 * Hydra_Software_Devel/40   10/28/05 9:36p vijeth
 * PR 17483: Added the STCinvalid flag
 * Added support to count the number of underflows,
 *
 * Hydra_Software_Devel/39   10/27/05 7:06p vijeth
 * PR 17483: Added code for the DM to remember the STC_Offset and
 * STC_Offset_valid information across the DM runs. For different
 * Pictures.
 *
 * Hydra_Software_Devel/38   10/20/05 5:25p pblanco
 * PR16052: Removed all C++ style comments
 *
 * Hydra_Software_Devel/37   10/18/05 3:36p ssavekar
 * PR 16052: New check-in for XVD
 * Added code for excluding OS calls from all normal compilations.
 *
 * Hydra_Software_Devel/36   10/14/05 7:08p ssavekar
 * PR 16052: New check-in for XVD
 * Added defines related to display manager logs and a few other defines
 * for enabling logs in "BXVD_P_PictureDataReady_isr()". All these
 * defines are disabled hence no logs will be printed.
 *
 * Hydra_Software_Devel/35   10/7/05 3:29p pblanco
 * PR16052: Added new function prototypes for queue management API.
 *
 * Hydra_Software_Devel/34   10/6/05 5:18p ssavekar
 * PR 16052: New check-in for XVD
 * Added variables for clean channel change.
 *
 * Hydra_Software_Devel/33   9/27/05 2:44p davidp
 * PR16052: Fix SH_MEM_AREA definition:
 *
 * Hydra_Software_Devel/32   9/26/05 2:25p pblanco
 * PR16052: Added RUL descriptor mask to channel open prototype.
 *
 * Hydra_Software_Devel/30   9/20/05 3:52p ssavekar
 * PR 16052: New check-in for XVD
 * Added a data structure for holding RUL ID mask values.
 *
 * Hydra_Software_Devel/29   9/16/05 8:37a pblanco
 * PR16052: Moved decoder control functions from bxvd.c to private
 * functions here.
 *
 * Hydra_Software_Devel/28   9/14/05 1:15p pblanco
 * PR16052: Added a BXVD_DisplayFieldType to the channel context.
 *
 * Hydra_Software_Devel/27   9/13/05 1:49p pblanco
 * PR16052: Cleaned up decoder flush code.
 *
 * Hydra_Software_Devel/26   9/9/05 7:26a pblanco
 * PR16052: Removed "AlmostDone" interrupt from channel structure.
 *
 * Hydra_Software_Devel/25   9/8/05 4:52p ssavekar
 * PR 16052: New check-in for XVD
 * Changes for DM integration with XVD and FW.
 *
 * Hydra_Software_Devel/24   9/6/05 1:07p pblanco
 * PR16052: Removed prototype for BXVD_P_GetFWVBase.
 *
 * Hydra_Software_Devel/23   9/6/05 10:26a pblanco
 * PR16052: Added new function prototypes for physical to virtual address
 * translation and moved some defines from bxvd_priv.c
 *
 * Hydra_Software_Devel/22   8/29/05 8:37a pblanco
 * PR16052: Support for new FW.
 *
 * Hydra_Software_Devel/21   8/25/05 9:34a pblanco
 * PR16052: Added prototypes for ChannelDrop and ReleasePicture.
 *
 * Hydra_Software_Devel/20   8/24/05 1:13p pblanco
 * PR16052: Added new function prototypes and a new member for AVC
 * instance to the XVD context structure.
 *
 * Hydra_Software_Devel/19   8/23/05 2:04p pblanco
 * PR16052: Additional host command support, code cleanup and bug fixes.
 *
 * Hydra_Software_Devel/19   8/23/05 1:46p pblanco
 * PR16052: Additional host command support, code cleanup and bug fixes.
 *
 * Hydra_Software_Devel/17   8/18/05 2:13p pblanco
 * PR16052: Code fixes for BXVD_OpenChannel and BXVD_StartDecode.
 *
 * Hydra_Software_Devel/16   8/5/05 12:24a ssavekar
 * PR 16052: New check-in for XVD
 * Added data structures and callback handles for "DataReadyISR" that in
 * turn calls DM in structure "BXVD_P_Channel".
 *
 * Hydra_Software_Devel/15   8/2/05 5:24a ssavekar
 * PR 16052: New check-in for XVD
 * Changes after a code walk-through.
 *
 * Hydra_Software_Devel/14   7/30/05 12:57a ssavekar
 * PR 16052: New check-in for XVD
 * Modified a few data structures for XVD and 7401 FW integration. Removed
 * all the PPB related data structures. These data structures are placed
 * in a separate file "bxvd_vdec_info.h"
 *
 * Hydra_Software_Devel/13   7/26/05 11:33a pblanco
 * PR16052: Changed remaining 7411 variable tags to 7401.
 *
 * Hydra_Software_Devel/12   7/25/05 12:24p ebrakus
 * PR16052: Modified BABV_Handle to include Avc contexts
 *
 * Hydra_Software_Devel/11   7/22/05 11:36a pblanco
 * PR16052: Code cleanup and data structure factoring.
 *
 * Hydra_Software_Devel/10   7/20/05 3:17p pblanco
 * PR16052: Additional development.
 *
 * Hydra_Software_Devel/9   7/19/05 3:09p pblanco
 * PR16052: Additional development.
 *
 * Hydra_Software_Devel/8   7/18/05 3:18p pblanco
* PR16052: Additional code development.
 *
 * Hydra_Software_Devel/6   7/15/05 1:07p pblanco
 * PR16052: Clean build with new code and data.
 *
 * Hydra_Software_Devel/5   7/14/05 10:53a pblanco
 * PR16052: Fix compilation errors caused by merge.
 *
 * Hydra_Software_Devel/4   7/13/05 4:19p pblanco
 * PR16052: Formatting and name fixups for merge.
 *
 * Hydra_Software_Devel/3   7/13/05 2:45p pblanco
 * PR16052: Merging with Santosh.
 *
 * Hydra_Software_Devel/1   7/7/05 8:56a pblanco
 * PR16052: Added
 *
 *****************************************************************************/
#ifndef BXVD_PRIV_H__
#define BXVD_PRIV_H__

#include "bxvd.h"
#include "bkni.h"
#include "breg_mem.h"
#include "bxvd_userdata.h"
#include "bxvd_pvr.h"
#include "bchp_bvnf_intr2_3.h"
#include "bxvd_vdec_info.h"
#include "bxvd_vdec_api.h"
#include "bxvd_memory_priv.h"
#include "bxvd_status.h"
#include "bxvd_dip.h"
#include "bxdm_dih.h"
#include "bxdm_pp.h"

/* needed to get pb_lib.c to compile. 
 * better to change pi_util.h?
 */
#include "bxvd_decoder.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Supported stripe width values */
#define BXVD_P_STRIPE_WIDTH_NUM 2

/* Supported stripe multiple values */
#define BXVD_P_STRIPE_MULTIPLE_NUM 3

#define BXVD_MAX_VIDEO_CHANNELS  16

/***********************************************************************
 *  Private macros
 ***********************************************************************/

/* SW7405-3245: at compile time select either the BDBG_INSTANCE_* or 
 * BDBG_* macros.  The BDBG_* macros are substantially faster.
 */
#if BXVD_P_USE_INSTANCE_MACROS
#define BXVD_DBG_MSG(instance, format) BDBG_INSTANCE_MSG(instance, format)
#define BXVD_DBG_WRN(instance, format) BDBG_INSTANCE_WRN(instance, format)
#define BXVD_DBG_ERR(instance, format) BDBG_INSTANCE_ERR(instance, format)
#else
#define BXVD_DBG_MSG(instance, format)    \
   BSTD_UNUSED(instance);                 \
   BDBG_MSG(format)                       \
   
#define BXVD_DBG_WRN(instance, format)    \
   BSTD_UNUSED(instance);                 \
   BDBG_WRN(format)                       \

#define BXVD_DBG_ERR(instance, format)    \
   BSTD_UNUSED(instance);                 \
   BDBG_ERR(format)                       \

#endif

#define BXVD_IS_MPEG(protocol) ( \
    (BAVC_VideoCompressionStd_eMPEG2 == (protocol)) || \
    (BAVC_VideoCompressionStd_eMPEG2DTV == (protocol)) || \
    (BAVC_VideoCompressionStd_eMPEG1 == (protocol)))

#define BXVD_IS_AVC(protocol) ( \
    (BAVC_VideoCompressionStd_eH264 == (protocol)) || \
    (BAVC_VideoCompressionStd_eH261== (protocol)) || \
    (BAVC_VideoCompressionStd_eH263== (protocol)))

#define BXVD_IS_AVS(protocol) (BAVC_VideoCompressionStd_eAVS == (protocol))

#define BXVD_P_RESET_CORE( hXvd, uiReg, uiResetMask, string)               \
{                                                                          \
   uint32_t uiRegVal, uiPollCnt;                                           \
   bool bDone;                                                             \
                                                                           \
   uiRegVal = BXVD_Reg_Read32( hXvd, uiReg);                               \
                                                                           \
   uiRegVal |= uiResetMask;                                                \
                                                                           \
   BXVD_Reg_Write32( hXvd, uiReg, uiRegVal);                               \
                                                                           \
   bDone = false;                                                          \
   uiPollCnt = 1;                                                          \
                                                                           \
   while (!bDone)                                                          \
   {                                                                       \
      uiRegVal = BXVD_Reg_Read32( hXvd, uiReg);                            \
                                                                           \
      if (((uiRegVal & uiResetMask) != uiResetMask)                        \
          || (uiPollCnt++ > 100))                                          \
      {                                                                    \
         bDone = true;                                                     \
      }                                                                    \
   }                                                                       \
   BXVD_DBG_MSG(hXvd, ("%s reset pollcnt: %d\n", string, uiPollCnt));      \
}


#define BXVD_P_IS_FW_VERSION_VALID(pInitRsp)                              \
   (((pInitRsp->sw_version >> 8) & 0xff) == BXVD_P_CURRENT_MAJOR_VERSION)

#define BXVD_P_CREATE_PROTOCOLS_MASK(eVidCmprStd) \
   ( 1 << (eVidCmprStd - BAVC_VideoCompressionStd_eMPEG4Part2))

#define BXVD_P_REVE_DECODE_PROTOCOLS_MASK BXVD_P_CREATE_PROTOCOLS_MASK(BAVC_VideoCompressionStd_eMPEG2_DSS_PES)

#define BXVD_P_REVH_DECODE_PROTOCOLS_MASK (BXVD_P_REVE_DECODE_PROTOCOLS_MASK | \
                                           BXVD_P_CREATE_PROTOCOLS_MASK(BAVC_VideoCompressionStd_eMPEG4Part2))

#define BXVD_P_REVI_DECODE_PROTOCOLS_MASK (BXVD_P_REVH_DECODE_PROTOCOLS_MASK | \
                                           BXVD_P_CREATE_PROTOCOLS_MASK( BAVC_VideoCompressionStd_eAVS))

#define BXVD_P_REVJ_DECODE_PROTOCOLS_MASK (BXVD_P_REVI_DECODE_PROTOCOLS_MASK | \
                                           BXVD_P_CREATE_PROTOCOLS_MASK( BAVC_VideoCompressionStd_eMVC))

#define BXVD_P_REVK_DECODE_PROTOCOLS_MASK (BXVD_P_REVJ_DECODE_PROTOCOLS_MASK | \
                                           BXVD_P_CREATE_PROTOCOLS_MASK( BAVC_VideoCompressionStd_eSVC))

#define BXVD_P_REVL_DECODE_PROTOCOLS_MASK (BXVD_P_REVK_DECODE_PROTOCOLS_MASK | \
                                           BXVD_P_CREATE_PROTOCOLS_MASK( BAVC_VideoCompressionStd_eVP7) | \
                                           BXVD_P_CREATE_PROTOCOLS_MASK( BAVC_VideoCompressionStd_eVP8) | \
                                           BXVD_P_CREATE_PROTOCOLS_MASK( BAVC_VideoCompressionStd_eRV9) | \
                                           BXVD_P_CREATE_PROTOCOLS_MASK( BAVC_VideoCompressionStd_eSPARK))

#define BXVD_P_REVM_DECODE_PROTOCOLS_MASK (BXVD_P_REVL_DECODE_PROTOCOLS_MASK | \
                                           BXVD_P_CREATE_PROTOCOLS_MASK( BAVC_VideoCompressionStd_eVP6))

#if BXVD_P_HVD_PRESENT 
#define BXVD_P_REVN_DECODE_PROTOCOLS_MASK (BXVD_P_REVM_DECODE_PROTOCOLS_MASK | \
                                           BXVD_P_CREATE_PROTOCOLS_MASK( BAVC_VideoCompressionStd_eH265))
#endif

#define BXVD_P_CALC_CACHED_FW_CONTEXT_ADDRS(uiVA, hXvdCh)  \
   (((uint32_t)(uiVA) - (uint32_t)hXvdCh->stDecodeFWBaseAddrs.pvFWContextVABase ) + \
    (uint32_t)(hXvdCh->stDecodeFWBaseAddrs.pvFWContextCachedBase))

/* Wrapper macros to help debug issues with shared memory and the HIM, in particular the release and 
 * delivery queue offsets.  These macros check that only one thread is trying to access the read/write
 * offsets at a time.  They don't check accesses to the queue's themselves.  
 * The appropriate code gets compiled in when "BXVD_P_VERIFY_QUEUE_OFFSETS" is defined.
 * When the debug code is not compiled in, the macros simply revert to those defined in "bxvd_core_avd_revXXX.h"
 */

/*
#define BXVD_P_VERIFY_QUEUE_OFFSETS 1
*/

#ifdef BXVD_P_VERIFY_QUEUE_OFFSETS
/*
 * Macros for managing the delivery queue.
 */
#define BXVD_P_DELIVERY_QUEUE_GET_READ_OFFSET( _hXvdCh_, _uiReadOffset_ )                 \
{                                                                                         \
   if ( 0 != _hXvdCh_->pXvd->uiSharedMemoryRefCnt )                                       \
   {                                                                                      \
      uint32_t uiInstanceId = _hXvdCh_->ulChannelNum & 0xF;                               \
      uiInstanceId |= ( _hXvdCh_->pXvd->uDecoderInstance & 0xF ) << 4 ;                   \
                                                                                          \
      BXVD_DBG_ERR(_hXvdCh_,("   [%02x.xxx] BXVD_P_DELIVERY_QUEUE_GET_READ_OFFSET: uiSharedMemoryRefCnt:%d should have been 0",   \
                                  uiInstanceId, _hXvdCh_->pXvd->uiSharedMemoryRefCnt));   \
   }                                                                                      \
   _hXvdCh_->pXvd->uiSharedMemoryRefCnt++;                                                \
   BXVD_AVD_P_DELIVERY_QUEUE_GET_READ_OFFSET( _hXvdCh_, _uiReadOffset_ )                  \
   _hXvdCh_->pXvd->uiSharedMemoryRefCnt--;                                                \
}

#define BXVD_P_DELIVERY_QUEUE_SET_READ_OFFSET( _hXvdCh_, _uiReadOffset_ )                 \
{                                                                                         \
   if ( 0 != _hXvdCh_->pXvd->uiSharedMemoryRefCnt )                                       \
   {                                                                                      \
      uint32_t uiInstanceId = _hXvdCh_->ulChannelNum & 0xF;                               \
      uiInstanceId |= ( _hXvdCh_->pXvd->uDecoderInstance & 0xF ) << 4 ;                   \
                                                                                          \
      BXVD_DBG_ERR(_hXvdCh_,("   [%02x.xxx] BXVD_P_DELIVERY_QUEUE_SET_READ_OFFSET: uiSharedMemoryRefCnt:%d should have been 0",   \
                                  uiInstanceId, _hXvdCh_->pXvd->uiSharedMemoryRefCnt));   \
   }                                                                                      \
   _hXvdCh_->pXvd->uiSharedMemoryRefCnt++;                                                \
   BXVD_AVD_P_DELIVERY_QUEUE_SET_READ_OFFSET( _hXvdCh_, _uiReadOffset_ )                  \
   _hXvdCh_->pXvd->uiSharedMemoryRefCnt--;                                                \
}

#define BXVD_P_DELIVERY_QUEUE_GET_WRITE_OFFSET( _hXvdCh_, _uiWriteOffset_ )               \
{                                                                                         \
   if ( 0 != _hXvdCh_->pXvd->uiSharedMemoryRefCnt )                                       \
   {                                                                                      \
      uint32_t uiInstanceId = _hXvdCh_->ulChannelNum & 0xF;                               \
      uiInstanceId |= ( _hXvdCh_->pXvd->uDecoderInstance & 0xF ) << 4 ;                   \
                                                                                          \
      BXVD_DBG_ERR(_hXvdCh_,("   [%02x.xxx] BXVD_P_DELIVERY_QUEUE_GET_WRITE_OFFSET: uiSharedMemoryRefCnt:%d should have been 0",   \
                                  uiInstanceId, _hXvdCh_->pXvd->uiSharedMemoryRefCnt));   \
   }                                                                                      \
   _hXvdCh_->pXvd->uiSharedMemoryRefCnt++;                                                \
   BXVD_AVD_P_DELIVERY_QUEUE_GET_WRITE_OFFSET( _hXvdCh_, _uiWriteOffset_ )                \
   _hXvdCh_->pXvd->uiSharedMemoryRefCnt--;                                                \
}

/*
 * Macros for managing the release queue.
 */
#define BXVD_P_RELEASE_QUEUE_GET_READ_OFFSET( _hXvdCh_, _uiReadOffset_ )                  \
{                                                                                         \
   if ( 0 != _hXvdCh_->pXvd->uiSharedMemoryRefCnt )                                       \
   {                                                                                      \
      uint32_t uiInstanceId = _hXvdCh_->ulChannelNum & 0xF;                               \
      uiInstanceId |= ( _hXvdCh_->pXvd->uDecoderInstance & 0xF ) << 4 ;                   \
                                                                                          \
      BXVD_DBG_ERR(_hXvdCh_,("   [%02x.xxx] BXVD_P_RELEASE_QUEUE_GET_READ_OFFSET: uiSharedMemoryRefCnt:%d should have been 0",   \
                                  uiInstanceId, _hXvdCh_->pXvd->uiSharedMemoryRefCnt));   \
   }                                                                                      \
   _hXvdCh_->pXvd->uiSharedMemoryRefCnt++;                                                \
   BXVD_AVD_P_RELEASE_QUEUE_GET_READ_OFFSET( _hXvdCh_, _uiReadOffset_ )                   \
   _hXvdCh_->pXvd->uiSharedMemoryRefCnt--;                                                \
}

#define BXVD_P_RELEASE_QUEUE_GET_WRITE_OFFSET( _hXvdCh_, _uiWriteOffset_ )                \
{                                                                                         \
   if ( 0 != _hXvdCh_->pXvd->uiSharedMemoryRefCnt )                                       \
   {                                                                                      \
      uint32_t uiInstanceId = _hXvdCh_->ulChannelNum & 0xF;                               \
      uiInstanceId |= ( _hXvdCh_->pXvd->uDecoderInstance & 0xF ) << 4 ;                   \
                                                                                          \
      BXVD_DBG_ERR(_hXvdCh_,("   [%02x.xxx] BXVD_P_RELEASE_QUEUE_GET_WRITE_OFFSET: uiSharedMemoryRefCnt:%d should have been 0",   \
                                  uiInstanceId, _hXvdCh_->pXvd->uiSharedMemoryRefCnt));   \
   }                                                                                      \
   _hXvdCh_->pXvd->uiSharedMemoryRefCnt++;                                                \
   BXVD_AVD_P_RELEASE_QUEUE_GET_WRITE_OFFSET( _hXvdCh_, _uiWriteOffset_ )                 \
   _hXvdCh_->pXvd->uiSharedMemoryRefCnt--;                                                \
}

#define BXVD_P_RELEASE_QUEUE_SET_WRITE_OFFSET( _hXvdCh_, _uiWriteOffset_ )                \
{                                                                                         \
   if ( 0 != _hXvdCh_->pXvd->uiSharedMemoryRefCnt )                                       \
   {                                                                                      \
      uint32_t uiInstanceId = _hXvdCh_->ulChannelNum & 0xF;                               \
      uiInstanceId |= ( _hXvdCh_->pXvd->uDecoderInstance & 0xF ) << 4 ;                   \
                                                                                          \
      BXVD_DBG_ERR(_hXvdCh_,("   [%02x.xxx] BXVD_P_RELEASE_QUEUE_SET_WRITE_OFFSET: uiSharedMemoryRefCnt:%d should have been 0",   \
                                  uiInstanceId, _hXvdCh_->pXvd->uiSharedMemoryRefCnt));   \
   }                                                                                      \
   _hXvdCh_->pXvd->uiSharedMemoryRefCnt++;                                                \
   BXVD_AVD_P_RELEASE_QUEUE_SET_WRITE_OFFSET( _hXvdCh_, _uiWriteOffset_ )                 \
   _hXvdCh_->pXvd->uiSharedMemoryRefCnt--;                                                \
}

#define BXVD_P_RELEASE_QUEUE_GET_SHADOW_WRITE_OFFSET( _hXvdCh_, _uiWriteOffset_ )         \
{                                                                                         \
   if ( 0 != _hXvdCh_->pXvd->uiSharedMemoryRefCnt )                                       \
   {                                                                                      \
      uint32_t uiInstanceId = _hXvdCh_->ulChannelNum & 0xF;                               \
      uiInstanceId |= ( _hXvdCh_->pXvd->uDecoderInstance & 0xF ) << 4 ;                   \
                                                                                          \
      BXVD_DBG_ERR(_hXvdCh_,("   [%02x.xxx] BXVD_P_RELEASE_QUEUE_GET_SHADOW_WRITE_OFFSET: uiSharedMemoryRefCnt:%d should have been 0",   \
                                  uiInstanceId, _hXvdCh_->pXvd->uiSharedMemoryRefCnt));   \
   }                                                                                      \
   _hXvdCh_->pXvd->uiSharedMemoryRefCnt++;                                                \
   BXVD_AVD_P_RELEASE_QUEUE_GET_SHADOW_WRITE_OFFSET( _hXvdCh_, _uiWriteOffset_ )          \
   _hXvdCh_->pXvd->uiSharedMemoryRefCnt--;                                                \
}

#define BXVD_P_RELEASE_QUEUE_SET_SHADOW_WRITE_OFFSET( _hXvdCh_, _uiWriteOffset_ )         \
{                                                                                         \
   if ( 0 != _hXvdCh_->pXvd->uiSharedMemoryRefCnt )                                       \
   {                                                                                      \
      uint32_t uiInstanceId = _hXvdCh_->ulChannelNum & 0xF;                               \
      uiInstanceId |= ( _hXvdCh_->pXvd->uDecoderInstance & 0xF ) << 4 ;                   \
                                                                                          \
      BXVD_DBG_ERR(_hXvdCh_,("   [%02x.xxx] BXVD_P_RELEASE_QUEUE_SET_SHADOW_WRITE_OFFSET: uiSharedMemoryRefCnt:%d should have been 0",   \
                                  uiInstanceId, _hXvdCh_->pXvd->uiSharedMemoryRefCnt));   \
   }                                                                                      \
   _hXvdCh_->pXvd->uiSharedMemoryRefCnt++;                                                \
   BXVD_AVD_P_RELEASE_QUEUE_SET_SHADOW_WRITE_OFFSET( _hXvdCh_, _uiWriteOffset_ )          \
   _hXvdCh_->pXvd->uiSharedMemoryRefCnt--;                                                \
}

/*
 * Macros for accessing the drop count.
 */
#define BXVD_P_SET_DROP_COUNT( _hXvdCh_, _count_ )                                        \
{                                                                                         \
   if ( 0 != _hXvdCh_->pXvd->uiSharedMemoryRefCnt )                                       \
   {                                                                                      \
      uint32_t uiInstanceId = _hXvdCh_->ulChannelNum & 0xF;                               \
      uiInstanceId |= ( _hXvdCh_->pXvd->uDecoderInstance & 0xF ) << 4 ;                   \
                                                                                          \
      BXVD_DBG_ERR(_hXvdCh_,("   [%02x.xxx] BXVD_P_SET_DROP_COUNT: uiSharedMemoryRefCnt:%d should have been 0",   \
                                  uiInstanceId, _hXvdCh_->pXvd->uiSharedMemoryRefCnt));   \
   }                                                                                      \
   _hXvdCh_->pXvd->uiSharedMemoryRefCnt++;                                                \
   BXVD_AVD_P_SET_DROP_COUNT( _hXvdCh_, _count_ )                                         \
   _hXvdCh_->pXvd->uiSharedMemoryRefCnt--;                                                \
}

#define BXVD_P_GET_DROP_COUNT( _hXvdCh_, _count_ )                                        \
{                                                                                         \
   if ( 0 != _hXvdCh_->pXvd->uiSharedMemoryRefCnt )                                       \
   {                                                                                      \
      uint32_t uiInstanceId = _hXvdCh_->ulChannelNum & 0xF;                               \
      uiInstanceId |= ( _hXvdCh_->pXvd->uDecoderInstance & 0xF ) << 4 ;                   \
                                                                                          \
      BXVD_DBG_ERR(_hXvdCh_,("   [%02x.xxx] BXVD_P_GET_DROP_COUNT: uiSharedMemoryRefCnt:%d should have been 0",   \
                                  uiInstanceId, _hXvdCh_->pXvd->uiSharedMemoryRefCnt));   \
   }                                                                                      \
   _hXvdCh_->pXvd->uiSharedMemoryRefCnt++;                                                \
   BXVD_AVD_P_GET_DROP_COUNT( _hXvdCh_, _count_ )                                         \
   _hXvdCh_->pXvd->uiSharedMemoryRefCnt--;                                                \
}

/*
 * Macro for retrieving the AVD status block.
 */
#define BXVD_P_GET_VIDEO_DECODER_STATUS( _hXvdCh_, _ulStatus_ )                           \
{                                                                                         \
   if ( 0 != _hXvdCh_->pXvd->uiSharedMemoryRefCnt )                                       \
   {                                                                                      \
      uint32_t uiInstanceId = _hXvdCh_->ulChannelNum & 0xF;                               \
      uiInstanceId |= ( _hXvdCh_->pXvd->uDecoderInstance & 0xF ) << 4 ;                   \
                                                                                          \
      BXVD_DBG_ERR(_hXvdCh_,("   [%02x.xxx] BXVD_P_GET_VIDEO_DECODER_STATUS: uiSharedMemoryRefCnt:%d should have been 0",   \
                                  uiInstanceId, _hXvdCh_->pXvd->uiSharedMemoryRefCnt));   \
   }                                                                                      \
   _hXvdCh_->pXvd->uiSharedMemoryRefCnt++;                                                \
   BXVD_AVD_P_GET_VIDEO_DECODER_STATUS( _hXvdCh_, _ulStatus_ )                            \
   _hXvdCh_->pXvd->uiSharedMemoryRefCnt--;                                                \
}

#else
/* When the debug code is not compiled in, the macros simply revert to those
 * defined in "bxvd_core_avd_revxxx.h".
 */

/* Macros for managing the delivery queue. */
#define BXVD_P_DELIVERY_QUEUE_GET_READ_OFFSET   BXVD_AVD_P_DELIVERY_QUEUE_GET_READ_OFFSET
#define BXVD_P_DELIVERY_QUEUE_SET_READ_OFFSET   BXVD_AVD_P_DELIVERY_QUEUE_SET_READ_OFFSET
#define BXVD_P_DELIVERY_QUEUE_GET_WRITE_OFFSET  BXVD_AVD_P_DELIVERY_QUEUE_GET_WRITE_OFFSET

/* Macros for managing the release queue. */
#define BXVD_P_RELEASE_QUEUE_GET_READ_OFFSET    BXVD_AVD_P_RELEASE_QUEUE_GET_READ_OFFSET
#define BXVD_P_RELEASE_QUEUE_GET_WRITE_OFFSET   BXVD_AVD_P_RELEASE_QUEUE_GET_WRITE_OFFSET
#define BXVD_P_RELEASE_QUEUE_SET_WRITE_OFFSET   BXVD_AVD_P_RELEASE_QUEUE_SET_WRITE_OFFSET
#define BXVD_P_RELEASE_QUEUE_GET_SHADOW_WRITE_OFFSET  BXVD_AVD_P_RELEASE_QUEUE_GET_SHADOW_WRITE_OFFSET
#define BXVD_P_RELEASE_QUEUE_SET_SHADOW_WRITE_OFFSET  BXVD_AVD_P_RELEASE_QUEUE_SET_SHADOW_WRITE_OFFSET

/* Macros for accessing the drop count. */
#define BXVD_P_SET_DROP_COUNT    BXVD_AVD_P_SET_DROP_COUNT
#define BXVD_P_GET_DROP_COUNT    BXVD_AVD_P_GET_DROP_COUNT

/* Macro for retrieving the AVD status block. */
#define BXVD_P_GET_VIDEO_DECODER_STATUS   BXVD_AVD_P_GET_VIDEO_DECODER_STATUS

#endif

/* Since these macros don't access the HIM, they map straight through. */
#define BXVD_P_DELIVERY_QUEUE_GET_ADDR    BXVD_AVD_P_DELIVERY_QUEUE_GET_ADDR
#define BXVD_P_RELEASE_QUEUE_GET_ADDR     BXVD_AVD_P_RELEASE_QUEUE_GET_ADDR
#define BXVD_P_GET_QUEUE_DEPTH            BXVD_AVD_P_GET_QUEUE_DEPTH
#define BXVD_P_INCREMENT_2BASED_OFFSET    BXVD_AVD_P_INCREMENT_2BASED_OFFSET


/***********************************************************************
 *  Private Enums
 ***********************************************************************/

/* Private state to indicate decode state */
typedef enum BXVD_P_DecoderState
{
  BXVD_P_DecoderState_eNotActive=0,    /* Initial state */
  BXVD_P_DecoderState_eActive          /* StartDecode executed */
} BXVD_P_DecoderState;

typedef enum BXVD_P_HandleType
{
   BXVD_P_HandleType_XvdMain,
   BXVD_P_HandleType_XvdChannel,
   BXVD_P_HandleType_Userdata,
   BXVD_P_HandleType_Invalid
} BXVD_P_HandleType;

typedef enum BXVD_P_MemCfgMode
{
   BXVD_P_MemCfgMode_eUMA,
   BXVD_P_MemCfgMode_eNONUMA,
   BXVD_P_MemCfgMode_eUNKNOWN
} BXVD_P_MemCfgMode;

typedef enum BXVD_P_PowerState
{
   BXVD_P_PowerState_eOn,
   BXVD_P_PowerState_eClkOff,
   BXVD_P_PowerState_ePwrOff
} BXVD_P_PowerState;

#define  BXVD_P_OUTER_WATCHDOG 0

/* Interrupt related definitions */
#define BXVD_P_INTR_CLEAR   0xffffffff
#define BXVD_P_INTR_OL_MASK 0x40000000

/* AVD PFRI Data Width related definitions */
#define BXVD_P_PFRI_DATA_DDR2 0
#define BXVD_P_PFRI_DATA_DDR3 1

typedef enum BXVD_P_PFRI_Data_Width
{
   BXVD_P_PFRI_Data_Width_e16Bit,
   BXVD_P_PFRI_Data_Width_e32Bit,
   BXVD_P_PFRI_Data_Width_e64Bit
} BXVD_P_PFRI_Data_Width ;

/* AVD PCache Mode related definitions */
typedef enum BXVD_P_PCache_XGran
{
   BXVD_P_PCache_XGran_e8Bytes,
   BXVD_P_PCache_XGran_e16Bytes,
   BXVD_P_PCache_XGran_e32Bytes
} BXVD_P_PCache_XGran;

typedef enum BXVD_P_PCache_YGran
{
   BXVD_P_PCache_YGran_e1Line,
   BXVD_P_PCache_YGran_e2Lines,
   BXVD_P_PCache_YGran_e4Lines
} BXVD_P_PCache_YGran;

/* Relocation related definitions */
#define BXVD_P_RELF_DEFAULT_CODE_BASE 0x00000000

/* Pre rev K core has a memory access limit of 768 mb */
#define BXVD_P_ARC300_RAM_LIMIT 0x30000000

/* General definitions */
#define BXVD_P_MEM_ZERO 0x00000000

/* If the firmware does not have a time code, or the protocol is not
   MPEG, the internal time code will be set thus */
#define BXVD_P_INVALID_TIMECODE 0xFFFFFFFF

/*
** Passed as a flag in the "channel mode" when
** calling BXVD_P_HostCmdSendDecChannelOpen().
** For best peformance, the bit should be set when DQT is enabled.
**
**      if bit[3] == 0 then it is legacy restricted mode
**      if bit[3] == 1 then it is new all capable AVD mode.
*/
#define BXVD_P_AVD_NO_CONSTRAINTS 0x8


#if BXVD_P_SVD_PRESENT
#define BXVD_P_ARCS_PER_DECODER 3
#else
#define BXVD_P_ARCS_PER_DECODER 2
#endif

typedef struct BXVD_P_CallBackRequests
{
  bool bPauseUntoPTS;
  bool bDisplayUntoPTS;
  bool bPTS1Match;
  bool bPTS2Match;
  bool bPresentationStart;
  bool bPresentationStop;
  bool bMarkerSeen;

/*PR28082*/
  bool bMarker;
  bool bPPBReceived;
} BXVD_P_CallBackRequests;

/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

/***********************************************************************
 *  Private Structures
 ***********************************************************************/
/* Contains XVD interrupt callbacks and user params */
typedef struct BXVD_P_InterruptCallbackInfo
{
  BXVD_IntCallbackFunc BXVD_P_pAppIntCallbackPtr ;

  void *pParm1;
  int  parm2;
} BXVD_P_InterruptCallbackInfo ;

typedef struct BXVD_P_FWMemConfig
{
   uint32_t general_memory_size;
   uint32_t cabac_bin_size;
   uint32_t video_block_size;
   uint32_t video_block_count;
} BXVD_P_FWMemConfig;

typedef struct BXVD_P_DecodeFWMemSize
{
   uint32_t  uiFWContextSize;
   uint32_t  uiFWCabacSize;
   uint32_t  uiFWPicBlockSize;
   uint32_t  uiFWPicBlockCount;
   uint32_t  uiFWCabacWorklistSize;
   uint32_t  uiFWDirectModeSize;
   uint32_t  uiFWInnerLoopWorklistSize;
   uint32_t  uiFWInterLayerPicSize;
   uint32_t  uiFWInterLayerMVSize;
} BXVD_P_DecodeFWMemSize;

typedef struct BXVD_P_DecodeFWBaseAddrs 
{
   uint32_t  uiFWContextBase;           /* FW Context memory physical base address */
   void      *pvFWContextVABase;        /* FW Context memory uncached virtual base address */
   void      *pvFWContextCachedBase;    /* FW Context memory cached base address */
   uint32_t  uiFWCabacBase;             /* FW Cabac memory base address */
   uint32_t  uiFWPicBlockBase;          /* FW Picture Block Base */
   uint32_t  uiFWCabacWorklistBase;     /* FW Cabac worklist memory base address */
   uint32_t  uiFWInnerLoopWorklistBase; /* FW Inner loop worklist memory base address */
   uint32_t  uiFWDirectModeBase;        /* FW Direct Mode memory base address */
   uint32_t  uiFWInterLayerPicBase;     /* FW Inter Layer Pic memory base address */
   uint32_t  uiFWInterLayerMVBase;      /* FW Inter Layer Motion Vector memory base address */
} BXVD_P_DecodeFWBaseAddrs;

/* Internal XVD context data */
typedef struct BXVD_P_Context
{
  /* Interface handles */
  BCHP_Handle hChip;
  BREG_Handle hReg;

  BMEM_Handle hSystemHeap; /* General Heap used to allocate structures */
  BMEM_Handle hCodeHeap; /* Firmware Code Heap used to load firmware */
  BMEM_Handle hPictureHeap; /* Picture Buffer Heap used by firmware to store decoded pictures */
  BMEM_Handle hCabacHeap; /* Cabac Buffer Heap used by firmware as CABAC bin buffer */

  BINT_Handle hInterrupt;

  /* XVD default settings */
  BXVD_Settings stSettings;

  /* Contexts for AVC decoders */
  uint32_t uDecoderInstance;
  BXVD_DecoderContext stDecoderContext;

  /* Platform-specific fields defined in platform header */
  BXVD_P_CONTEXT_PLATFORM

  /* FW revision information */
  BXVD_RevisionInfo sRevisionInfo;

  /* For Crc */
  unsigned long        ulDebugDeliveryQueueAddr;
  unsigned long        ulDebugReleaseQueueAddr;

  /* Channel information including count and open handles */
  uint16_t             uiOpenChannels;
  BXVD_ChannelHandle   *ahChannel;

  BLST_S_HEAD(FreeChannelContext, BXVD_P_Channel) FreeChannelListHead;

  /* Suballocated FW memory heaps */
  BXVD_P_MemoryHandle  SubGenMem;
  BXVD_P_MemoryHandle  SubSecureMem;
  BXVD_P_MemoryHandle  SubPicMem;

  /* Pointers to FW/XVD code memory where the FW is loaded */
  uint32_t             uiFWMemVirtAddr;
  uint32_t             uiFWMemPhyAddr;
  uint32_t             uiFWMemSize;
  uint8_t              bFWMemAllocated;
  BXVD_AVDBootMode     eAVDBootMode;

  /* Some platforms determine AVD core clock freq at runtime */
  uint32_t             uiAVDCoreFreq;

  /* Instruction base addresses for outer and inner loop */
  uint32_t             uiOuterLoopInstructionBase;
  uint32_t             uiInnerLoopInstructionBase;

  /* End of code addresses for outer and inner loop */
  uint32_t             uiOuterLoopEOC;
  uint32_t             uiInnerLoopEOC;
#if BXVD_P_SVD_PRESENT
  /* Base Layer Instruction base and end of code */
  uint32_t             uiBaseInstructionBase;
  uint32_t             uiBaseEOC;
#endif

  /* Outer loop communication vector. This is the same as uiOuterLoopEOC */
  uint32_t             uiCmdBufferVector;

  /* Pointers to FW/XVD shared memory for Picture buffers and FW internal data structs */
  uint32_t             uiFWContextPhyAddr;
  uint32_t             uiFWGenMemVirtAddr;
  uint32_t             uiFWGenMemPhyAddr;
  uint32_t             uiFWGenMemSize;

  /* Pointers to Decoder debug log buffers */
  uint32_t             uiDecoderDbgBufVirtAddr;
  uint32_t             uiDecoderDbgBufPhyAddr;

  bool                 bFWDbgLoggingStarted;

  /* Size and count of FW Picture buffers */
  uint32_t             uiFWPicMemVirtAddr;
  uint32_t             uiFWPicMemPhyAddr;
  uint32_t             uiFWPicMemSize;

  /* Pointers to FW/XVD Cabac Bin memory */
  uint32_t             uiFWCabacMemVirtAddr;
  uint32_t             uiFWCabacMemPhyAddr;
  uint32_t             uiFWCabacMemSize;

  /* Register offset and mask values */
  uint32_t             uiXPT_PCROffset_STC;

  uint32_t             uiDecode_SDStripeWidthRegVal;
  uint32_t             uiDecode_IPShimPfriRegVal;
  uint32_t             uiDecode_StripeWidth;
  uint32_t             uiDecode_StripeMultiple;
  uint32_t             uiDecode_PFRIDataRegVal;
  uint32_t             uiAVD_PCacheRegVal;
  uint32_t             uiChip_ProductRevision;

  /* PR18233: Mosaic mode support */
  BAVC_XVD_Picture     *pVDCPictureBuffers;
  BAVC_XVD_Picture     *pPictureListVec0;
  BAVC_XVD_Picture     *pPictureListVec1;

  /* Linked Decoder support */
  BXVD_Handle            hXvd_Secondary;
  BXVD_DisplayInterrupt  Secondary_eDisplayInterrupt;

  /* Still picture buffer support */
  unsigned long  watchdog_timer_addr;
  unsigned long  watchdog_write_value;

#if BXVD_P_FW_HIM_API
  uint32_t uiDisplayInfo0_Offset;
  uint32_t uiDisplayInfo1_Offset;
#endif

  BXVD_P_DisplayInfo  *pDisplayInfo0;
  BXVD_P_DisplayInfo  *pDisplayInfo1;

  BXVD_P_InterruptCallbackInfo stDeviceInterruptCallbackInfo[BXVD_DeviceInterrupt_eMaxInterrupts] ;

  BXVD_ChannelHandle   hStillChannel;
  bool bStillChannelAllocated;
  uint32_t uiStillChannelNum;
  bool bStillPictureCompatibilityMode;
  bool bStillHDCapable;

  uint32_t uiSupportedProtocolsMask;

  bool bSVCCapable;
  bool bRV9Capable;

  bool bAllocDecodeModeList;

  bool bHibernate;
  BXVD_P_PowerState PowerStateCurrent;
  BXVD_P_PowerState PowerStateSaved;

  BXVD_P_FWMemConfig stFWMemCfg[BXVD_DecodeMode_eMaxModes];

  BXVD_StatusHandle hXvdStatus;

  BXVD_P_HandleType eHandleType;

  BTMR_TimerHandle hTimer;

  uint32_t auiDisplayInterruptCount[BXVD_DisplayInterrupt_eMax];
  uint32_t auiActiveDecodes[BXVD_ChannelMode_eMax][BXVD_DisplayInterrupt_eMax];

  BXDM_DisplayInterruptHandler_Handle hXdmDih[BXVD_DisplayInterrupt_eMax];
  BXDM_DisplayInterruptHandler_Handle hAppXdmDih[BXVD_DisplayInterrupt_eMax];
  BXDM_DisplayInterruptHandler_Handle hLinkedDecoderXdmDih;
  BXVD_DisplayInterruptProvider_P_ChannelHandle hXvdDipCh[BXVD_DisplayInterrupt_eMax];

#if BXVD_P_AVD_ARC600
  BAFL_FirmwareInfo astFWBootInfo[BXVD_P_ARCS_PER_DECODER];
#endif

  /* FW command and response buffers */
  BXVD_FW_Cmd FWCmd;
  BXVD_FW_Rsp FWRsp;


#ifdef BXVD_P_VERIFY_QUEUE_OFFSETS
   /* to help debug delivery/release queue issues */
   uint32_t uiSharedMemoryRefCnt;
#endif

} BXVD_P_Context;

/* The buffer configuration for the channel */
typedef struct BXVD_P_BufferConfig
{
  BXVD_P_DMS2DMInfo AVD_DMS2PI_Buffer ;
  BXVD_P_DM2DMSInfo *pAVD_PI2DMS_Buffer ;

  uint16_t ui32_HardwareNmbx ;
  uint16_t ui32_HardwareNmby ;

} BXVD_P_BufferConfig;

struct BXVD_P_IntCallback {
  BXVD_IntCallbackFunc callback;
  void *pParm1;
  int	parm2;
};


/* Circular queue */
#define BXVD_P_USERDATA_QUEUE_START 0
#define BXVD_P_USERDATA_QUEUE_MAX   32

#define BXVD_P_USERDATA_ITEM_SIZE   2048

/*
 * uiDecodePictureId added per Jira SW7425-1780
 */
typedef struct _QUEUE_MGR_
{
      int ulReadPtr;
      int ulWritePtr;
      int ulNextPtr;
      int ulQueueDepth;
      struct data
      {
            int           protocol;
            unsigned long ulFlags;
            unsigned long ulPulldown;
            unsigned long ulPTS;
            unsigned char *uUserData;
            uint32_t uiDecodePictureId;
      } *queue_data;
} QUEUE_MGR;


/* The internal structure for the User Data handle */
typedef struct BXVD_P_UserDataContext
{
      BXVD_ChannelHandle     hXvdCh;
      BINT_CallbackHandle    hCbUserdata;
      BXVD_Userdata_Settings sUserdataSettings;
      void                   *pBfr; /* user data buffer */
      bool                   bCallbackEnabled;
      /* Userdata callback registered */
      BINT_CallbackFunc	     fUserdataCallback_isr;
      void		     *pParm1;
      int 		     parm2;
      QUEUE_MGR              queue;
      BXVD_P_HandleType      eHandleType;
      BERR_Code              errForwardError;
} BXVD_P_UserDataContext ;


/*
 * Called by DM when valid user data is available in the PPB.
 */
BERR_Code BXVD_P_Userdata_EnqueueDataPointer(BXVD_ChannelHandle hXvdCh,
                                             int           protocol,
                                             unsigned long p_UserData,
                                             unsigned long ulFlags,
                                             unsigned long ulPulldown,
                                             unsigned long ulPTS,
                                             uint32_t uiSerialNumber);


#if BXVD_P_FW_HIM_API

typedef struct BXVD_P_HIM_Offsets
{
   unsigned long ulByteOffset;      /* byte offset into HIM, returned by AVD. */
   unsigned long ulWordOffset;      /* ulByteOffset divided by 4, XVD always read/writes a word. */
   unsigned long ulByteShift;       /* used to shift the data to the appropriate byte. */
   unsigned long ulByteMask;        /* mask for the byte of interest. */
   unsigned long ulInverseMask;     /* inverse of the ulByteMask, to assist with read-modify-write operations. */
   unsigned long ulBytesPerValue;   /* Indicates if this a 1, 2, 3, or 4 byte value. */

} BXVD_P_HIM_Offsets;

/* Contains the addresses and offsets needed to access the AVD
 * Delivery and Release queues.
 */
typedef struct BXVD_P_AVD_Queue
{
   unsigned long        ulQueueOffset;
   BXVD_P_HIM_Offsets   stReadIndex;
   BXVD_P_HIM_Offsets   stWriteIndex;
   BXVD_P_HIM_Offsets   stShadowWriteIndex;     /* For the release queue. */

} BXVD_P_AVD_Queue;

#endif /* #if BXVD_P_FW_HIM_API */

/* SW7425-1064: Support for the XMO MergeOmatic filter. */
typedef enum BXVD_P_ChannelType
{
   BXVD_P_ChannelType_eStandard = 0,
   BXVD_P_ChannelType_eBase,
   BXVD_P_ChannelType_eEnhanced,

   BXVD_P_ChannelType_eMax
} BXVD_P_ChannelType;

/* The internal structure for the Channel handle */
typedef struct BXVD_P_Channel
{
  uint32_t                 ulChannelNum;      /* channel number */
  bool                     bPreserveState;    /* Do not reset PVR context */
  bool                     bPreserveCounters; /* Do not reset decoder counters */

  /* FW info */
  BXVD_P_DecodeFWMemSize   stDecodeFWMemSize;   /* FW Memory sizes allocated */ 
  BXVD_P_DecodeFWBaseAddrs stDecodeFWBaseAddrs; /* FW Memory base addresses */

#if BXVD_P_FW_HIM_API
  BXVD_P_AVD_Queue         stDeliveryQueue;     /* data associated with the AVD delivery queue */
  BXVD_P_AVD_Queue         stReleaseQueue;      /* data associated with the AVD release queue */
  BXVD_P_HIM_Offsets       stDropCountIndex;    /* Host Interface Memory offsets for the drop count. */
  BXVD_P_HIM_Offsets       stStatusBlockIndex;  /* Host Interface Memory offsets for the AVD status word(s) */
#else
  unsigned long            ulPicBuf;          /* Picture delivery queue addr */
  unsigned long            ulPicRelBuf;       /* Picture release queue addr */
  unsigned long            ulPicInfoBuf;      /* unused? */
  unsigned long            ulPicInfoRelBuf;   /* offset to Release Queue shadow write offset and drop count. */
#endif

  unsigned long            ulUserDataRelBuf;

  unsigned long            ulAvdStatusBlock;  /* AVD Status block address */

  BXVD_P_UserDataContext   *pUserData;
  BXVD_P_DecoderState      eDecoderState;
  BXVD_P_Context           *pXvd;             /* main handle */
  BXVD_DecodeSettings      sDecodeSettings;
  unsigned long            ulXptCDB_Read;     /* Current Rave CDB_Read register */
  unsigned long            aulXptCDB_Read_Extended[BXVD_NUM_EXT_RAVE_CONTEXT];     /* Current Extended Rave CDB_Read register */
  BXVD_ChannelSettings     sChSettings;
  BAVC_VideoCompressionStd asVideoCmprStdList[BAVC_VideoCompressionStd_eMax];

  BXVD_DisplayInterrupt    eDisplayInterrupt;   /* The FW Display Device interrupt */

  /* Structure to keep all the MVD interrupt callbacks and its params */
  BXVD_P_InterruptCallbackInfo stInterruptCallbackInfo[BXVD_Interrupt_eMaxInterrupts] ;

  bool bStillPictureToRelease; /* True if there's a pending still picture to release */
  uint32_t uiStillDisplayElementOffset; /* Last still picture offset (to be released) */
  bool bDecoderChannelOpened; /* True if decoder channel has been opened */

  BMEM_Handle hSystemHeap; /* General Heap used to allocate structures for this channel */
  BMEM_Handle hPictureHeap; /* Picture Buffer Heap used by firmware to store decoded pictures for this channel */
  BMEM_Handle hCabacHeap; /* Cabac Buffer Heap used by firmware as CABAC bin buffer for this channel */

  BXVD_P_HandleType eHandleType;

   /* TODO: Move back to stChannelState? */
  BXVD_P_CallBackRequests  stCallbackReq;
  BXVD_P_BufferConfig      stChBufferConfig ;
  BXDM_PictureProvider_ChannelChangeSettings stSavedChannelChangeSettings;
  BXVD_SkipMode eCurrentSkipMode;

  BXDM_PictureProvider_Handle hPictureProvider;

  BXVD_P_Decoder_Context stDecoderContext;

  BXDM_Picture_Rate stSavedPlaybackRate;
  bool bSavedPlaybackRateValid;

  /* SW7400-2870: paused by BXVD_PVR_EnablePause.  As opposed to being paused by
   * calling BXVD_SetPlaybackRate(_isr) with a numerator of '0'.
   */
  bool bPauseActive;

  bool bProgressiveStream_7411;
  BXVD_PictureParameterInfo stPictureParameterInfo;

  uint32_t uiPPBSerialNumber;
  BXVD_StillContentInterpolationMode eSavedSPIM;

  /* SW7425-1064: Support for the XMO MergeOmatic filter. */

  BXDM_PictureProvider_XMO_Handle hXmo;   /* Handle for the XMO filter. */

  BXVD_ChannelHandle hXvdChLinked;        /* Used to link two channels together. */
  BXVD_P_ChannelType eChannelType;        /* eStandard, eBase or eEnhanced,*/

#if BXVD_P_USE_TWO_DECODERS
  /* SW7425-1064: when using two decoders with the XMO, both decoders need to be
   * driven off the same interrupts.  The interrupts settings for the second decoder
   * will be saved in BXVD_StartDecode, and then restored in BXVD_StopDecode.
   */
  BXVD_DisplayInterruptProvider_P_InterruptSettings stEnhancedInterruptConfig;
#endif

  BLST_S_ENTRY(BXVD_P_Channel) link; /* Used to keep list of available channel structures */

} BXVD_P_Channel;

/* This may be unnecessary and removed later */
typedef struct
{
  unsigned long next;
  unsigned long ulType;
  unsigned long ulSize;
} BXVD_P_UserData;

typedef enum BXVD_P_VideoAtomIndex
{
   BXVD_P_VideoAtomIndex_eA = 0,
   BXVD_P_VideoAtomIndex_eB,
   BXVD_P_VideoAtomIndex_eC,
   BXVD_P_VideoAtomIndex_eD,
   BXVD_P_VideoAtomIndex_eE,
   BXVD_P_VideoAtomIndex_eF,
   BXVD_P_VideoAtomIndex_eG,
   BXVD_P_VideoAtomIndex_eH,
   BXVD_P_VideoAtomIndex_eI,
   BXVD_P_VideoAtomIndex_eAT,
   BXVD_P_VideoAtomIndex_eZ,

   /* Add more enums ABOVE this line */
   BXVD_P_VideoAtomIndex_eMax
} BXVD_P_VideoAtomIndex;

typedef struct BXVD_P_FWMemConfig_V2
{
      unsigned long general_memory_size;
      unsigned long inner_loop_wl_size;
      unsigned long direct_mode_size;
      unsigned long cabac_bin_size;
      unsigned long cabac_wl_size;
      BXVD_P_VideoAtomIndex video_block_size_index;
      unsigned long video_block_count;
} BXVD_P_FWMemConfig_V2;

typedef struct BXVD_P_FWMemConfig_SVC
{
      unsigned long context_memory_size;
      unsigned long inner_loop_wl_size;
      unsigned long direct_mode_size;
      unsigned long inter_layer_video_size_index;
      unsigned long inter_layer_mv_size;
      unsigned long cabac_bin_size;
      unsigned long cabac_wl_size;
      BXVD_P_VideoAtomIndex video_block_size_index;
      unsigned long video_block_count;
} BXVD_P_FWMemConfig_SVC;

/***********************************************************************
 * Private functions
 ***********************************************************************/

BERR_Code BXVD_P_SetupFWSubHeap(BXVD_Handle hXvd);
BERR_Code BXVD_P_TeardownFWSubHeap(BXVD_Handle hXvd);

BERR_Code BXVD_P_GetDecodeFWMemSize
(
 BXVD_Handle hXvd,
 BXVD_DecodeResolution eDecodeResolution,
 BAVC_VideoCompressionStd aeVideoCmprStd[],
 uint32_t   uiVideoCmprCount,
 const BXVD_ChannelSettings *pChSettings,
 BXVD_P_DecodeFWMemSize       *pstDecodeFWMemSize
);

BERR_Code BXVD_P_GetStillDecodeFWMemSize
(
 BXVD_Handle hXvd,
 BXVD_DecodeResolution eDecodeResolution,
 BAVC_VideoCompressionStd aeVideoCmprStd[],
 uint32_t   uiVideoCmprCount,
 BXVD_P_DecodeFWMemSize       *pstDecodeFWMemSize
);

BERR_Code BXVD_P_AllocateFWMem
(
  BXVD_Handle hXvd,
  BXVD_ChannelHandle hXvdCh,
  BXVD_P_DecodeFWMemSize *pstDecodeFWMemSize,
  BXVD_P_DecodeFWBaseAddrs *pstDecodeFWBaseAddrs
);


BERR_Code BXVD_P_FreeFWMem
(
  BXVD_Handle hXvd,
  BXVD_ChannelHandle hXvdCh,
  uint32_t uiFWContextBase,
  uint32_t uiFWCabacBase,
  uint32_t uiFWPicBlockBase,
  uint32_t uiFWCabacWorklistBase,
  uint32_t uiFWInterLayerPicBase
);

void BXVD_P_ValidateHeaps
(
  BXVD_Handle        hXvd,
  BXVD_P_MemCfgMode  eMemCfgMode
);

BERR_Code BXVD_P_StillPictureEnabled(BXVD_Handle hXvd, bool *pEnableState, BXVD_DecodeMode *pSPDecodeMode);

/***************************************************************************
Summary:
    Releases the indicated PPB via FW.

Description:
    This API releases the PPB indicated in the ppb argument.

Returns:
    BERR_SUCCESS

See Also:
    None

****************************************************************************/
BERR_Code BXVD_P_ReleasePPB(BXVD_Handle hXvd,	uint32_t ppb);

BERR_Code BXVD_P_ReleasePPB_isr(BXVD_Handle hXvd,	uint32_t ppb);


/***************************************************************************
Name:
        BXVD_P_ParseDecodeMode

Description:
    This routine parses the list of decoder modes looking for the mode that requires the largest picture buffers.

Returns:
    None:

See Also:
    None

****************************************************************************/

BERR_Code BXVD_P_ParseDecodeMode
(
  BXVD_Handle hXvd,
  BXVD_DecodeMode *pDecodeModeList,  /* List of decoder modes */
  BXVD_DecodeMode *eDecodeMode     /* Enum of mode in provided list that requires the largest buffer */
);

BERR_Code BXVD_P_Boot
(
  BXVD_Handle hXvd
);

BERR_Code BXVD_P_SetupStillPictureCompatibilityMode
(
   BXVD_Handle hXvd
);

BERR_Code BXVD_P_TeardownStillPictureCompatibilityMode
(
   BXVD_Handle hXvd
);

BERR_Code BXVD_P_InitializeFWMemConfigCompatibilityModeTable
(
   BXVD_Handle hXvd
);

/* AVD DEVICE COMMANDS */
BERR_Code BXVD_P_HostCmdSendInit
(
 BXVD_Handle  hXvd,                /* XVD context */
 uint32_t     uDecoderInstance,    /* AVC instance */
 uint32_t     eRaveEndianess       /* Endianess of data in Rave CDB/ITB */
);

BERR_Code BXVD_P_HostCmdSendConfig
(
 BXVD_Handle  hXvd,              /* XVD context */
 uint32_t     uiVecIndex,        /* Display device index */
 uint32_t     uiInterruptMask    /* RUL done mask for specified display */
);

BERR_Code BXVD_P_HostCmdSendDecChannelOpen
(
 BXVD_Handle         hXvd,
 BXVD_ChannelHandle  hXvdCh,
 bool                bStillPictureMode,
 uint32_t            uiMaxResolution,
 BXVD_P_DecodeFWMemSize  *pstDecodeFWMemSize,
 BXVD_P_DecodeFWBaseAddrs *pstDecodeFWBaseAddrs
);

BERR_Code BXVD_P_HostCmdSendDecChannelClose
(
 BXVD_Handle         hXvd,
 BXVD_ChannelHandle  hXvdCh
);

BERR_Code BXVD_P_HostCmdSendDecChannelStart
(
  BXVD_Handle         hXvd,
  uint32_t            ulChannelNumber,
  uint32_t            eProtocol,
  uint32_t            eChannelMode,
  uint32_t            ulRaveContextBase,
  uint32_t            aulRaveContextBaseExt[],
  uint32_t            ulVecIndex
);


BERR_Code BXVD_P_HostCmdSendDecChannelStop
(
   BXVD_Handle        hXvd,
   uint32_t           ulChannelNum
);


/***************************************************************************
Summary:
    Set the skip picture mode.

Description:
    This API instructs the FW to skip I, IP or no frames

Returns:
    BERR_SUCCESS

See Also:
    None

****************************************************************************/
BERR_Code BXVD_P_HostCmdSetSkipPictureMode
(
 BXVD_Handle    hXvd,
 uint32_t       ulChannelNum,
 BXVD_SkipMode  eSkipMode
);


/***************************************************************************
 *  Summary
 *
 *     BXVD_P_HostCmdDbgLogControl: Send debug log control command
 *
 *  Description
 *
 *     The decoders outer loop debug output can be logged to memory for XVD
 *     to read and pass to the application. This routines starts and stops
 *     the logging of the debug data to the log buffer.
 *
 ***************************************************************************/
BERR_Code BXVD_P_HostCmdDbgLogControl
(
   BXVD_Handle    hXvd,
   bool           logStart
);

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
);


BERR_Code BXVD_P_ChipInit(BXVD_Handle hXvd, 
                          uint32_t uDecoderInstance);

BERR_Code BXVD_P_Reset740x(BXVD_Handle hXvd, 
                           uint32_t uDecoderInstance);

BERR_Code BXVD_P_InitChannel(BXVD_ChannelHandle  hXvdCh);

BERR_Code BXVD_P_DestroyInterrupts(BXVD_Handle hXvd);

void BXVD_P_FreeXVDContext(BXVD_Handle hXvd);

BERR_Code BXVD_P_InitFreeChannelList(BXVD_Handle hXvd);

void BXVD_P_GetChannelHandle(BXVD_Handle hXvd,
                             BXVD_ChannelHandle *phXvdCh);

void BXVD_P_KeepChannelHandle(BXVD_Handle hXvd,
                              BXVD_ChannelHandle hXvdCh);

void BXVD_P_FreeAllocatedChannelHandles(BXVD_Handle hXvd);

#if (BXVD_P_POWER_MANAGEMENT)

void BXVD_P_GetHibernateState(BXVD_Handle hXvd,
                              bool *bHibernateState);

void BXVD_P_SetHibernateState(BXVD_Handle hXvd,
                              bool bHibernateState);

#endif


bool BXVD_P_IsDecodeProtocolSupported(BXVD_Handle               hXvd,
                                      BAVC_VideoCompressionStd  eVideoCmprStd );

BERR_Code BXVD_P_MapToAVDProtocolEnum( BXVD_Handle               hXvd,
                                       BAVC_VideoCompressionStd  eVideoCmprStd,
                                       BXVD_P_PPB_Protocol *     peProtocol );

extern const uint32_t BXVD_P_StripeWidthLUT[];

#ifdef __cplusplus
}
#endif

#endif /* BXVD_PRIV_H__ */

/* End of file. */
