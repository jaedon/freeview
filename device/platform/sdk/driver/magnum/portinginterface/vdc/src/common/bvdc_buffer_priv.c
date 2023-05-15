/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvdc_buffer_priv.c $
 * $brcm_Revision: Hydra_Software_Devel/255 $
 * $brcm_Date: 10/10/12 11:27a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vdc/src/common/bvdc_buffer_priv.c $
 * 
 * Hydra_Software_Devel/255   10/10/12 11:27a jessem
 * SW7425-3748: Reduced buffer count from 5 to 4 for 50-to-60 and 60-to-50
 * modes.
 *
 * Hydra_Software_Devel/254   9/20/12 4:56p yuxiaz
 * SW7425-1835, SW7425-3928: Reenable BVDC_P_USE_RDC_TIMESTAMP. Fixed
 * drift delay using RDC timestamp.
 *
 * Hydra_Software_Devel/253   8/10/12 4:37p yuxiaz
 * SW7425-3745, SW7425-1835: Fixed segfault for non-mpeg progressive
 * source.
 *
 * Hydra_Software_Devel/252   7/31/12 4:55p yuxiaz
 * SW7425-1835: Use RDC slot timestamp for capture and playback timestamp
 * in multi-buffering algo.
 *
 * Hydra_Software_Devel/251   3/8/12 11:49a yuxiaz
 * SW7425-2546: Fixed warning msg.
 *
 * Hydra_Software_Devel/250   2/6/12 9:33p vanessah
 * SW7425-2296: use scaler to do Orientation conversion
 *
 * Hydra_Software_Devel/249   2/2/12 4:42p vanessah
 * SW7425-2309: initialization pHeapNode_R to NULL for 2D orientation
 *
 * Hydra_Software_Devel/248   12/1/11 4:30p yuxiaz
 * SW7425-968, SW7344-95: Merged into mainline.: added independent source
 * clipping of right window in 3D mode.
 *
 * Hydra_Software_Devel/SW7425-968/1   11/16/11 3:12p yuxiaz
 * SW7425-968: Added left_R calculation in VDC rects.
 *
 * Hydra_Software_Devel/247   6/20/11 5:49p tdo
 * SW7425-104: Rename the old repeat field flag to match avc structure and
 * use the new added bRepeatField flag for ANR.
 *
 * Hydra_Software_Devel/246   6/10/11 5:49p hongtaoz
 * SW7425-704: add NRT mode support to VDC; moved the STG meta data
 * passing from window writer isr to reader isr; added bStallStc flag
 * support;
 *
 * Hydra_Software_Devel/245   5/10/11 11:04a yuxiaz
 * SW7425-415: Clean up buffer allocation for 3D.
 *
 * Hydra_Software_Devel/244   5/2/11 11:58a yuxiaz
 * SW7405-5279: Fixed crash in BVDC_P_Buffer_ExtractBuffer_isr when there
 * is no capture buffer.
 *
 * Hydra_Software_Devel/243   4/28/11 3:13p yuxiaz
 * SW7420-1186: Merge to mainline: Field reverse seen sometimes on
 * composite video.
 *
 * Hydra_Software_Devel/SW7420-1186/1   4/8/11 3:42p yuxiaz
 * SW7420-1186: Field reverse seen sometimes on composite video.
 *
 * Hydra_Software_Devel/242   4/8/11 1:56p yuxiaz
 * SW7425-236: Fixed flashing when source orientation changes.
 *
 * Hydra_Software_Devel/241   3/22/11 1:40p yuxiaz
 * SW7400-2882, SW7420-1186: Fixed index increment for multi-buffer log.
 *
 * Hydra_Software_Devel/241   3/22/11 11:23a yuxiaz
 * SW7400-2882, SW7420-1186: Fixed index for multi-buffering log.
 *
 * Hydra_Software_Devel/240   3/16/11 11:19a yuxiaz
 * SW7400-2882, SW7420-1186: Updated multi-buffering log.
 *
 * Hydra_Software_Devel/239   3/15/11 4:22p yuxiaz
 * SW7400-2882: Added support to enable / disable multi-buffering logging
 * for a specific window.
 *
 * Hydra_Software_Devel/238   2/9/11 3:59p yuxiaz
 * SW7400-2882: Ported multi-buffing event logging scheme to VDC. Move
 * related APIs to bvdc_dbg.h
 *
 * Hydra_Software_Devel/237   10/29/10 3:01p tdo
 * SW7422-40: Update GetBuffer() capability in VDC with 3D case
 *
 * Hydra_Software_Devel/236   10/29/10 10:58a yuxiaz
 * SW7422-31, SW7422-32, SW7422-39: More for 3D Video support.
 *
 * Hydra_Software_Devel/235   9/29/10 5:21p yuxiaz
 * SW7422-39: Added 3D video support in CAP and VFD.
 *
 * Hydra_Software_Devel/234   9/15/10 11:22a yuxiaz
 * SW7422-30: Handle forced capture modes and orientation programming in
 * MFD.
 *
 * Hydra_Software_Devel/233   8/12/10 3:00p rpan
 * SW7400-2882: Read VDC timer register directly to avoid a restriction
 * from accessing exlusive timer via BTMR API.
 *
 * Hydra_Software_Devel/232   8/6/10 11:41a rpan
 * SW7400-2882: Integrated the multi-buffering event logging scheme.
 *
 * Hydra_Software_Devel/231   8/3/10 5:15p rpan
 * SW7420-895: Back out the "Report buffer delay based on writer and
 * reader node gap" change.
 *
 * Hydra_Software_Devel/230   8/2/10 11:26a rpan
 * SW7420-895: Report buffer delay based on writer and reader node gap.
 *
 * Hydra_Software_Devel/229   6/18/10 4:19p rpan
 * SW7400-2763: New BVN resource management.
 *
 * Hydra_Software_Devel/228   5/7/10 7:08p albertl
 * SW7125-364: Changed dirty bits to use union structure to avoid type-pun
 * warnings.
 *
 * Hydra_Software_Devel/227   4/19/10 10:11p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic. Move
 * BDBG_OBJECT_ID_DECLARE private header files instead of .c.
 *
 * Hydra_Software_Devel/226   4/7/10 11:24a tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic.  Rename TLA
 *
 * Hydra_Software_Devel/225   4/5/10 3:58p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic
 *
 * Hydra_Software_Devel/224   2/4/10 1:29p yuxiaz
 * SW7405-3874: Segfault when running lip sync stress test.
 *
 * Hydra_Software_Devel/223   2/1/10 6:35p hongtaoz
 * SW3548-2678, SW3548-2680, SW3548-2681: added HDMI1.4 3D formats support
 * for 3548; added multi-buffer low delay mode support for 1to2 and 2to5
 * rate gap cases; reduced capture buffer allocation for interlaced
 * pulldown case;
 *
 * Hydra_Software_Devel/BCM3D/2   1/4/10 4:13p hongtaoz
 * SW3548-2678, SW3548-2680, SW3548-2681: disabled multi-buffer
 * alternative movement if reader/writer have rate gap and are within RUL
 * timestamp tolerance range;
 *
 * Hydra_Software_Devel/BCM3D/1   12/17/09 6:07p hongtaoz
 * SW3548-2678: added game mode delay support for 1to2 and 2to5 src/disp
 * rate ratioes;
 *
 * Hydra_Software_Devel/222   1/28/10 11:10a syang
 * SW7405-3829: add option for app to shrink src width to use deinterlace
 *
 * Hydra_Software_Devel/221   1/15/10 2:42p rpan
 * SW3556-1000: Skip two fields when MAD is at reader side to avoid it
 * getting into spatial-only mode.
 *
 * Hydra_Software_Devel/220   12/2/09 4:16p rpan
 * SW7401-4347: Remove obsoleted debug code.
 *
 * Hydra_Software_Devel/219   10/19/09 5:57p yuxiaz
 * SW7405-3115: Use BLST_CQ functions to remove and insert buffer node.
 *
 * Hydra_Software_Devel/218   8/18/09 4:14p rpan
 * PR57098: Enforce rate gap lower boundry when doing progressive display.
 *
 * Hydra_Software_Devel/217   8/3/09 8:26p pntruong
 * PR57098: The dcx should is to be reset in the init, but for invalidate
 * it needs to stay with the current dcx settings.
 *
 * Hydra_Software_Devel/216   7/31/09 3:25p yuxiaz
 * PR57098: Disable DCX for fixed color feed. Reset DCX settings in
 * BVDC_P_Buffer_Invalidate_isr.
 *
 * Hydra_Software_Devel/215   6/2/09 3:03p syang
 * PR 55646:  smoothly filtering  in field inversion cases
 *
 * Hydra_Software_Devel/214   4/27/09 6:41p rpan
 * PR54630, PR47621: Added matching user specified ePixelFmt into the
 * valid user capture buffer criteria. Cleaned up dirty bits related to
 * user capture buffer handling a bit.
 *
 * Hydra_Software_Devel/213   3/31/09 3:34p hongtaoz
 * PR53087, PR53038, PR53687, PR53688: increased double-buffer timestamp
 * tolerance with the VEC alignment error;
 *
 * Hydra_Software_Devel/212   3/30/09 2:21p hongtaoz
 * PR53037, PR53038: Implement the VEC Locking/Alignments For Memory
 * Reduction; Implement Double-Capture-Buffer Algorithm Under VEC Locking
 * Scheme
 *
 * Hydra_Software_Devel/211   3/13/09 5:28p syang
 * PR 52376, PR 41481, PR 44041:
 * 1). exec user buf returning to hBuffer before shut-down code in
 * writer_isr; 2). clean up mad-delay handling code and add handling to
 * all settings in pic-node that are delayed by mad; 3). 444 SCL no-
 * longer uses BVN_IN to align-up for half-band; 4). add pic-node rects
 * dump feature as setting lbox_0_scratch to 2 (1 still printing mpeg pic
 * info).
 *
 * Hydra_Software_Devel/210   1/30/09 11:42a rpan
 * PR47337: Set cadence matching to off for picture node initialization
 * and invalidation to make system more robust.
 *
 * Hydra_Software_Devel/209   1/28/09 1:37p rpan
 * PR45319: Fixed a code typo. The comments didn't match code logic before
 * because of this mistake.
 *
 * Hydra_Software_Devel/208   1/27/09 3:48p tdo
 * PR51473: Skip checking for cadence matching when buffer is muted
 *
 * Hydra_Software_Devel/207   12/18/08 8:45p pntruong
 * PR50502: Used correct display/source rate for computing the number of
 * buffers.
 *
 * Hydra_Software_Devel/206   12/5/08 10:36a pntruong
 * Pr50026: [VDC:HDMI] Kylin crashes on 1024x768@43 hz with HDMI input.
 *
 * Hydra_Software_Devel/205   11/20/08 2:51p rpan
 * PR47337: Added trick mode detection. Replaced feedback based capture
 * polarity prediction mechanism with a non-feedback based mechanism.
 * This would help reduce latency.
 *
 * Hydra_Software_Devel/204   10/17/08 3:55p rpan
 * PR47368: Fixed a time stamp related cut-n-paste error.
 *
 * Hydra_Software_Devel/203   10/6/08 1:11p pntruong
 * PR47595, PR47072: Fixed pixel formats for cap/mad.
 *
 * Hydra_Software_Devel/202   10/3/08 9:23a yuxiaz
 * PR46305, PR46307, PR46309: More Video TestFeature1 support.
 *
 * Hydra_Software_Devel/201   9/29/08 12:57p pntruong
 * PR47072: Moving 3563 vdc dedicated branch.
 *
 * Hydra_Software_Devel/200   9/26/08 3:54p rpan
 * PR47368: When writer ISR moves the reader node predicted VEC polarity
 * is used.
 *
 * Hydra_Software_Devel/199   9/26/08 3:23p yuxiaz
 * PR46305, PR46307: More video TestFeature1 support for 3548 B0.
 *
 * Hydra_Software_Devel/198   9/25/08 3:05p rpan
 * PR45636: Fixed a cut-n-paste error.
 *
 * Hydra_Software_Devel/197   9/25/08 11:11a rpan
 * PR45636: Implemented support for cases that decoder convert frame rate
 * by dropping or repeating fields.
 *
 * Hydra_Software_Devel/196   9/25/08 10:46a rpan
 * PR45319: 1) Add 2 more cases that writer can skip one field;
 * 2) Changed Writer(Reader)RateGap to WriterVsReaderRateCode enum for
 * better readablity.
 *
 * Hydra_Software_Devel/195   9/23/08 4:12p rpan
 * PR46794: Improved comments.
 *
 * Hydra_Software_Devel/194   9/23/08 3:16p rpan
 * PR46865: Flag to indicate whether doing frame capture is now a field of
 * window context.
 *
 * Hydra_Software_Devel/193   9/23/08 9:33a rpan
 * PR46865: Added more conditions for incrementing buffer counts.
 *
 * Hydra_Software_Devel/192   9/22/08 9:30p pntruong
 * PR47160: Added additional case for capture as frame.  Refactored logic
 * and trigger computation on change.
 *
 * Hydra_Software_Devel/191   9/22/08 4:50p yuxiaz
 * PR46305, PR46307, PR46309: Added video TestFeature1 support for 3548 B0.
 *
 * Hydra_Software_Devel/190   9/18/08 9:12a rpan
 * PR46920: Temporarily disable polarity check in reader's function for
 * progressive source and interlaced display case.
 *
 * Hydra_Software_Devel/189   9/17/08 9:16a rpan
 * PR46920: Enhanced how reader matches picture's polarity with VEC's.
 *
 * Hydra_Software_Devel/188   9/15/08 1:39p rpan
 * PR46865: Multi-buffering writer's skip step (1 frame or 2 fields) now
 * bases on whether pictures are captured as frame.
 *
 * Hydra_Software_Devel/187   9/12/08 5:01p rpan
 * PR45381: Mark user capture buffer node as inactive.
 *
 * Hydra_Software_Devel/186   9/12/08 10:44a rpan
 * PR46794: Improved handling of increasing from 3 to 5 buffers or
 * decreasing from 5 to 3 buffers.
 *
 * Hydra_Software_Devel/185   9/11/08 4:43p rpan
 * PR46793: Re-implemented how buffer nodes get added and released.
 *
 * Hydra_Software_Devel/184   9/10/08 4:48p rpan
 * PR45319: Initialize window object's current reader and writer node to
 * buffer's current reader and write node.
 *
 * Hydra_Software_Devel/183   9/8/08 9:50a rpan
 * PR46100: Added more checks to make sure interlaced display won't be
 * affected.
 *
 * Hydra_Software_Devel/182   9/5/08 4:28p rpan
 * PR46100: Re-implemented 3 buffers for 1080p24/25/30 source to
 * 1080p48/50/60 display to match the algorithm.
 *
 * Hydra_Software_Devel/181   9/5/08 2:16p rpan
 * PR46100: 3 buffers for 1080p24/25/30 source to 1080p48/50/60 display.
 *
 * Hydra_Software_Devel/180   9/4/08 2:27p rpan
 * PR45319: Further cleaned up multi-buffering stuff from other modules.
 *
 * Hydra_Software_Devel/179   9/4/08 1:24p rpan
 * PR45319: Moved multi-buffering related stuff from window to buffer.
 *
 * Hydra_Software_Devel/178   9/2/08 11:52a rpan
 * PR45319: Covert picture node flags to bit-fields to reduce structure
 * size.
 *
 * Hydra_Software_Devel/177   8/22/08 10:26a rpan
 * PR45979: Remove VBI passthru support.
 *
 * Hydra_Software_Devel/176   8/21/08 11:03a rpan
 * PR45381: Mute the returned picture node.
 *
 * Hydra_Software_Devel/175   8/20/08 2:58p rpan
 * PR45381: Reworked the logic of returning user capture buffer node.
 *
 * Hydra_Software_Devel/174   8/13/08 4:23p rpan
 * PR44752: Keep current reader buffer and picture node returned by
 * BVDC_P_Buffer_GetNextReaderNode_isr() in sync.
 *
 * Hydra_Software_Devel/173   8/12/08 10:15a rpan
 * PR45381: 1) Added more restriction on which picture node can be
 * returned as user capture buffer; 2) Adding/Removing user capture
 * buffer should be affect vsync delay.
 *
 * Hydra_Software_Devel/172   5/28/08 9:08a rpan
 * PR43006: Refer to current info data structure instead of user supplied
 * new info data structure.
 *
 * Hydra_Software_Devel/171   5/23/08 3:57p rpan
 * PR43006: At least five buffers required if a window is sync slip, 50Hz
 * source ->60 Hz interlaced display, and feeder goes directly to
 * compositor and VEC.
 *
 * Hydra_Software_Devel/170   5/22/08 4:59p rpan
 * PR42965: Abstract rate gap calculation into a function.
 *
 * Hydra_Software_Devel/169   4/22/08 5:12p tdo
 * PR41243: SCL Vertical Init Phase Adjustment For i2i 50->60 Rate
 * Conversion
 *
 * Hydra_Software_Devel/168   4/17/08 2:53p yuxiaz
 * PR40957: Refactor out the dynamics capture pixel format computation
 * code.
 *
 * Hydra_Software_Devel/167   3/26/08 1:14p hongtaoz
 * PR35398, PR40895: added ulVsyncRate and removed bGameModeDelayLags in
 * the window lipsync callback structure; added BFMT_FREQ_FACTOR to unify
 * Vsync rate and pixel frequency calculation;
 *
 * Hydra_Software_Devel/166   3/21/08 10:42a yuxiaz
 * PR38929: Set default capture pixel format to
 * BVDC_P_CAP_PIXEL_FORMAT_8BIT422 on 3548.
 *
 * Hydra_Software_Devel/165   2/29/08 10:23a hongtaoz
 * PR38275: when multi-buffer's read/write pointers are on the same
 * buffer, BVDC_P_Buffer_GetCurrentDelay returns 0 to help advancing
 * writer first;
 *
 * Hydra_Software_Devel/164   2/25/08 6:06p jessem
 * PR 39294: Removed ulDstVertRate from BVDC_P_PictureNode.  Used most
 * current hDisplay->stCurInfo.ulVertFreq for  ulDstVertRate in
 * BVDC_P_Buffer_GetNextReaderNode_isr and
 * BVDC_P_Buffer_GetNextWriterNode_isr
 *
 * Hydra_Software_Devel/163   2/25/08 4:10p jessem
 * PR 39294: Used most current hDisplay->stCurInfo.ulVertFreq for
 * ulDstVertRate in BVDC_P_Buffer_GetNextReaderNode_isr. Reset pWindow-
 * >bBufferCntDecremented in BVDC_P_Window_DetermineVnetMode.
 *
 * Hydra_Software_Devel/PR39294/2   2/25/08 3:45p jessem
 * PR 39294: Merged in main line changes.
 *
 * Hydra_Software_Devel/PR39294/1   2/19/08 6:21p jessem
 * PR 39294: Used most current hDisplay->stCurInfo.ulVertFreq for
 * ulDstVertRate.
 *
  * Hydra_Software_Devel/162   2/21/08 6:18p hongtaoz
 * PR39828: accurate control of the sync-slipped multi-buffer [-1, +1]
 * field drift range;
 *
 * Hydra_Software_Devel/161   2/20/08 4:14p hongtaoz
 * PR39154: added window API to toggle ANR; prepare for combo ANR/MAD
 * feature on 3548;
 *
* Hydra_Software_Devel/160   2/7/08 11:50a jessem
 * PR 33613: Reverted back to to version without the modular mutlibuffer.
 * Found a problem with HDDVI sources.
 *
 * Hydra_Software_Devel/157   1/29/08 9:27a rpan
 * PR39068: Fixed a couple of multi-buffering implementation bugs.
 *
 * Hydra_Software_Devel/156   1/18/08 3:27p rpan
 * PR38275: Fixed a lipsync delay adjustment  issue.
 *
 * Hydra_Software_Devel/155   1/8/08 10:20a rpan
 * PR38234: Update buffer delay count along with needed buffer count so
 * that they are in sync.
 *
 * Hydra_Software_Devel/154   11/20/07 11:07p tdo
 * PR36889: Remove strictly alias warnings
 *
 * Hydra_Software_Devel/153   11/13/07 3:14p pntruong
 * PR35364: Need to re-initialized Qp value on init or invalidate buffer
 * to prevent source with non-stream based Qp to retain old ones.
 *
 * Hydra_Software_Devel/152   10/23/07 11:14a yuxiaz
 * PR29569, PR36290: Add FGT support on 7405.
 *
 * Hydra_Software_Devel/PR29569/1   10/12/07 10:48a yuxiaz
 * PR29569: Add FGT support on 7405
 *
 * Hydra_Software_Devel/151   8/17/07 9:57a pntruong
 * PR34059: Need to fix FMT name for 1024x768i_43Hz.
 *
 * Hydra_Software_Devel/150   7/24/07 4:44p jessem
 * PR 33335: Fixed bug in checking if next reader node and/or next-next
 * reader node is equal to the current writer node when the next reader
 * node's polarity doesn't match the VEC polarity.
 *
 * Hydra_Software_Devel/149   7/23/07 12:51p jessem
 * PR 32553, PR 33335: Added support for  interlaced with reader rate gap
 * condition (43Hz interlaced PC source). Removed forced pulldown scheme.
 *
 * Hydra_Software_Devel/148   7/17/07 4:29p albertl
 * PR32593:  Removed extra buffer fix for 24 to 60 Hz conversion and fixed
 * bug moving reader node too far forward.
 *
 * Hydra_Software_Devel/147   7/12/07 6:10p albertl
 * PR32593:  Added missing case for BAVC_SourceId_eDs0 and added fixes to
 * 24 to 60 Hz conversion requiring an extra buffer and a wider gap
 * between reader and writer.
 *
 * Hydra_Software_Devel/146   6/28/07 7:24p hongtaoz
 * PR32130, PR29189: added buffer game mode delay control; fixed picture
 * timestamp update bug when isr reorder occurs; invalid timestamp/drift
 * delay should disappear now; lipsync delay reflects the correct game
 * mode effect;
 *
 * Hydra_Software_Devel/145   6/26/07 1:45p syang
 * PR 32444: 1). reader don't push writer's pic node if src doesn't feed;
 * 2). next writer pic node returned by buf are marked as muted and are
 * un-muted by writer_isr
 *
 * Hydra_Software_Devel/144   6/15/07 12:15p jessem
 * PR 31858: Removed forced repeat execution in the check for next node ==
 * writer or next next node == writer and for vec polarity == frame in
 * BVDC_P_Buffer_GetNextReader_isr. Also, added comments for
 * clarification of 3:2 and 2:2 pulldown algorithm for interlaced
 * displays.
 *
 * Hydra_Software_Devel/143   5/25/07 6:48p hongtaoz
 * PR31576: updated lipsync delay buffer addition algorithm; avoid back
 * motion and inconsistent window reader programming;
 *
 * Hydra_Software_Devel/142   5/1/07 10:33a hongtaoz
 * PR30075, PR30170: simplified buffer release algorithm; always release
 * from the last active buffer; refactored window buffer code; the
 * minimum clear rect size is required for bClearMosaicOutside;
 *
 * Hydra_Software_Devel/141   4/12/07 11:33a jessem
 * PR 29770: Initialized eLastBuffAction in BVDC_P_BufferContext to
 * BVDC_P_Last_Buffer_Action_Reader_Moved instead of
 * BVDC_P_Last_Buffer_Action_Invalid in BVDC_P_Buffer_Init and
 * BVDC_P_Buffer_Invalidate_isr. This ensures that the last buffer action
 * is in a known state at startup and during channel changes. This is
 * especially helpful when the VEC timing for both primary and secondary
 * displays are in sync at startup (and during channel changes) that may
 * lead to incorrect multibuffer timestamp tolerance operation. An
 * incorrect operation results in video jitter.
 *
 * Hydra_Software_Devel/140   4/12/07 10:38a jessem
 * PR 29765: Added BKNI_Free(pBuffer->pBufList) and moved
 * BKNI_Free(pBuffer) after the while loop in BVDC_P_Buffer_Create().
 *
 * Hydra_Software_Devel/139   4/4/07 3:42p syang
 * PR 29243: change VnetMode def (now use bit-fields)
 *
 * Hydra_Software_Devel/138   3/22/07 11:00a jessem
 * PR 28902: Added stricter check for non-standard source rates when
 * marking a picture as repeat for MAD. So if a source rate is less than
 * 59.94Hz and the display rate is 60Hz a repeat picture will be tagged
 * as such even if the reader rate gap is zero.
 *
 * Hydra_Software_Devel/137   3/21/07 2:06p jessem
 * PR 28902: Mark current playback picture as repeat if the current reader
 * encroaches on the writer and if the VEC polarity is frame and if a
 * reader rate gap exists. See BVDC_P_Buffer_GetNextReader_isr.
 *
 * Hydra_Software_Devel/136   3/19/07 10:52a jessem
 * PR 28868: Removed check for desired caotured picture equalliing the
 * current writer in BVDC_P_Buffer_ExtractBuffer_isr.
 *
 * Hydra_Software_Devel/135   3/14/07 3:35p jessem
 * PR 28746: Added check to ascertain that total buffer delay when
 * lipsync delay is added is maintained. The buffer delay has a +-1
 * field/frame tolerance.
 *
 * Hydra_Software_Devel/134   2/28/07 11:37a pntruong
 * PR28149: Multibuffer Reader/Writer iRateGap toggling on ifd input.
 *
 * Hydra_Software_Devel/133   2/22/07 10:34p pntruong
 * PR20875: Initialized ePixelFormat on window create to avoid residue,
 * that won't trigger data capture mode.
 *
 * Hydra_Software_Devel/132   2/22/07 1:49p jessem
 * PR 27758: Added check for source rates that are outside the 23.976Hz
 * and 85Hz range. During source transitions, the source's ulVertFreq is
 * not in the valid range and affects the rate gap and buffer count
 * algorithm.
 *
 * Hydra_Software_Devel/131   2/21/07 1:28p jessem
 * PR 23218, PR 27698, PR 27758: PR 27698: Added support for
 * 23.976/24/25/30/50Hz progressive sources displayed as 59.94/60/50Hz
 * interlaced.
 *
 * Hydra_Software_Devel/130   2/20/07 2:55p jessem
 * PR25235: Removed the use of rate mask and replaced with actual value of
 * source frame and display refresh rates.
 *
 * Hydra_Software_Devel/129   2/8/07 1:20p jessem
 * PR 23218: Added check for 50p-59.94/60Hz rate relationship.
 *
 * Hydra_Software_Devel/128   2/8/07 9:39a jessem
 * PR 27738: Put in fix to handle progressive display formats.
 *
 * Hydra_Software_Devel/127   2/2/07 4:03p hongtaoz
 * PR27586: enable MAD for scaled-down window;
 *
 ***************************************************************************/
#include "bstd.h"
#include "bkni.h"
#include "bmem.h"
#include "bvdc_buffer_priv.h"
#include "bvdc_vnet_priv.h"
#include "bvdc_compositor_priv.h"
#include "bvdc_display_priv.h"
#include "bvdc_source_priv.h"
#include "bvdc_displayfmt_priv.h"
#include "bvdc_common_priv.h"
#include "bvdc_window_priv.h"
#include "bvdc_capture_priv.h"
#include "bvdc_feeder_priv.h"


BDBG_MODULE(BVDC_BUF);
BDBG_OBJECT_ID(BVDC_BUF);

#if (BVDC_BUF_LOG == 1)
/***************************************************************************
 */

/* Logging Legend

   Depends on the event, each logging entry has the following format:
   * uuuuuuuu:bbbb:mx:y[...W.R]z:vvvvvvvv
     + uuuuuuuu: Reference timestamp (microsecond)
     + bbbbb:    Event number index in the circular buffer
     + mx:       Event m of window x
       = Ax:y    Window x writer skips 2 fields, y:pCurWriterBuf->ulBufferId
       = Bx:y    Window x writer skips 1 field/frame, y:pCurWriterBuf->ulBufferId
       = Cx:y    Window x reader repeat 1 field/frame, y:pCurReaderBuf->ulBufferId
       = Dx:y    Window x reader repeat for polarity, y:pCurReaderBuf->ulBufferId
       = Ex:y    Window x new writer IRQ at position y
       = Fx:y    Window x new reader IRQ at position y
     + [...W.R]: Position of reader and writer pointers
     + z:        Number of active buffers
     + vvvvvvvv: VDC timestamp

   * uuuuuuuu:bbbb:mx:yyyyyyyy:zzzzz
     + uuuuuuuu: Reference timestamp (microsecond)
     + bbbbb:    Event number index in the circular buffer
     + mx:
       = Hx:     Reader latency on window x
       = Ix:     Writer latency on window x
     + yyyyyyyy: Time delta
     + zzzzz:    Number of captured fields so far

   * uuuuuuuu:bbbb:Jx:R=y W=z
     + uuuuuuuu: Reference timestamp (microsecond)
     + bbbbb:    Event number index in the circular buffer
     + Jx:       Invalidate on window x
     + R=y:      Set reader to position y
     + W=z       Set writer to position z

   * uuuuuuuu:bbbb:mx:yyyyy
     + uuuuuuuu: Reference timestamp (microsecond)
     + bbbbb:    Event number index in the circular buffer
     + mx:
       = Kx:     Reader of window x moved by writer due to misordered ISR (type 1)
       = Ox:     Reader of window x moved by writer due to misordered ISR (type 2)
       = Lx:     Writer of window x moved by reader due to misordered ISR (type 1)
       = Px:     Writer of window x moved by reader due to misordered ISR (type 2)
     + yyyyy:    Number of captured fields

   * uuuuuuuu:bbbb:mx:cnt=y
     + uuuuuuuu:  Reference timestamp (microsecond)
     + bbbbb:     Event number index in the circular buffer
     + mx:
       = Mx:cnt=y Added buffer entry on window x to count=y
       = Nx:cnt=y Removed buffer entry on window x to count=y

   * uuuuuuuu:bbbb:Qx:cccccccc:pppppppp
     + uuuuuuuu:  Reference timestamp (microsecond)
     + bbbbb:     Event number index in the circular buffer
     + Qx:        Timestamp update on window x
     + cccccccc:  VDC capture timestamp (microsecond)
     + pppppppp:  VDC playback timestamp (microsecond)
 */

#define BVDC_P_BUF_LOG_ENTRIES         2048                             /* Number of log entries - must be of powers of 2 */
#define BVDC_P_BUF_LOG_ENTRIES_MASK    (2048-1)                         /* Mask of the number of log entries */
#define BVDC_P_BUF_LOG_ENTRIES_PAD     64                               /* Number of events to dump before and after the last skip/repeat event */
#define BVDC_P_BUF_LOG_MAX_OUTPUT      (BVDC_P_BUF_LOG_ENTRIES_PAD<<1)  /* Maximum number of log entries at each round */
#define BVDC_P_BUF_LOG_BUFFERS_NUM_MAX 16                               /* Maximum number buffer entries of eacn VDC window */
#define BVDC_P_BUF_LOG_TRACE_WINDOW_0  0                                /* The first VDC window to trace */
#define BVDC_P_BUF_LOG_TRACE_WINDOW_1  1                                /* The second VDC window to trace */

/* This index implementation assumes little-endian configuration.
   This allows vdclog_dump to only output the recent events.
   In the 32-bit value ulIdx, the upper 16-bit is the wraparound counter,
   and the lower 16-bit is the actual index to the vdclog event buffer. */
#define BVDC_P_BUF_LOG_IDX_INCREMENT(stIdx) \
                                    stIdx.ulIdx++;                                        \
                                    if(stIdx.stIdxPart.usIdxEvent >= BVDC_P_BUF_LOG_ENTRIES)  \
                                    {                                                   \
                                        stIdx.stIdxPart.usIdxEvent = 0;                   \
                                        stIdx.stIdxPart.usIdxRound++;                     \
                                    }
#define BVDC_P_BUF_LOG_IDX_ADD(stIdx, uValueAdd)                                              \
                                    stIdx.ulIdx += uValueAdd;                                 \
                                    if(stIdx.stIdxPart.usIdxEvent >= BVDC_P_BUF_LOG_ENTRIES)      \
                                    {                                                       \
                                        stIdx.stIdxPart.usIdxEvent &= BVDC_P_BUF_LOG_ENTRIES_MASK;\
                                        stIdx.stIdxPart.usIdxRound++;                         \
                                    }
#define BVDC_P_BUF_LOG_IDX_SUB(stIdx, uValueSub)              \
                                    stIdx.ulIdx -= uValueSub; \
                                    stIdx.stIdxPart.usIdxEvent &= BVDC_P_BUF_LOG_ENTRIES_MASK

typedef union  {
    uint32_t ulIdx;
    struct  {
        uint16_t usIdxEvent;
        uint16_t usIdxRound;
    } stIdxPart;
} BVDC_P_BufLogIdx;

typedef struct {
    uint32_t    ulWindowId;
    uint32_t    ulBufferId;
    char        cRW[BVDC_P_BUF_LOG_BUFFERS_NUM_MAX];
    uint32_t    ulBufferNum;
    uint32_t    ulBufferTS;
    uint32_t    ulInfo;
} BVDC_P_BufLogSkipRepeat;

typedef struct  {
    uint32_t    ulTimeDiff;
    uint32_t    ulWindowId;
    uint32_t    ulNumCapField;
} BVDC_P_BufLogTimeStamp;

typedef struct {
    uint32_t    ulU0;
    uint32_t    ulU1;
    uint32_t    ulU2;
    uint32_t    ulU3;
} BVDC_P_BufLogGeneric;

typedef struct BVDC_P_BufLog {
    char        cLogType;                                      /* See the logging legend above... */
    uint32_t    ulTime;                                        /* Timestamp from the free-running timer (microseconds) */
    union {
        BVDC_P_BufLogSkipRepeat   stSkipRepeat;                /* Skip/Repeat event - events {A, B, C, D} */
        BVDC_P_BufLogTimeStamp    stTimeStamp;                 /* Latency event */
        BVDC_P_BufLogGeneric      stGeneric;                   /* Generic event */
    } BufLogData;
} BVDC_P_BufLog;


static BVDC_P_BufLog          astBufLog[BVDC_P_BUF_LOG_ENTRIES];      /* The circular buffer */
static BVDC_P_BufLogIdx       stBufLog_IdxW = {0};                    /* Writer pointer */
static BVDC_P_BufLogIdx       stBufLog_IdxLastSkipRepeat={0};         /* The position of the last Skip/Repeat event */
static BVDC_BufLogState       eBufLogState = BVDC_BufLogState_eReset; /* State of the logging */
static BVDC_CallbackFunc_isr  pfBufLogCallback = NULL;                /* User registered callback function */
static void                   *pvBufLogCallbackParm1 = NULL;          /* First user callback parameter */
static int                    iBufLogParm2 = 0;                       /* Second user callback parameter */
static BTMR_TimerRegisters    stTmrRegs={0,0};                        /* VDC timer registers */
static bool                   bstBufLogEnable[BVDC_P_WindowId_eComp0_G0]
	= {true, true, true, true, true, true};

void BVDC_P_Buffer_SetLogStateAndDumpTrigger
	( BVDC_BufLogState                 eLogState,
  	  const BVDC_CallbackFunc_isr 	   pfCallback,
	  void							   *pvParm1,
	  int 							   iParm2 )
{
	/* Store user settings */
	eBufLogState = eLogState;
	pfBufLogCallback = pfCallback;
	pvBufLogCallbackParm1 = pvParm1;
	iBufLogParm2 = iParm2;
}


/*
 * Outputs the log, from BVDC_P_BUF_LOG_ENTRIES_PAD events prior to the first skip/repeat event through BVDC_P_BUF_LOG_ENTRIES_PAD events
 * after the last skip/repeat event.
 * It tries to catch up with the stBufLog_IdxLastSkipRepeat index, that moves whenever another skip/repeat event occurs.
 */
void BVDC_P_Buffer_DumpLog(void)
{
    int              iTmp;
    char             szStringTmp[128];
    uint32_t         ulBufCtr = 0, ulOutputCtr = 0;
    BVDC_P_BufLog    *pstSrc = (BVDC_P_BufLog*)&(astBufLog[0]);
    BVDC_P_BufLogIdx stBufLog_IdxRLast, stBufLog_IdxR;
    uint16_t         usBufLogR;

    /* compute the starting point */
    stBufLog_IdxR = stBufLog_IdxLastSkipRepeat;
    BVDC_P_BUF_LOG_IDX_SUB(stBufLog_IdxR, BVDC_P_BUF_LOG_ENTRIES_PAD);

    /* header marker */
    BDBG_ERR(("BRCM===%08x:%08x:%08x===>", stBufLog_IdxR.ulIdx, stBufLog_IdxLastSkipRepeat.ulIdx, stBufLog_IdxW.ulIdx));
    while(1)
    {
        usBufLogR = stBufLog_IdxR.stIdxPart.usIdxEvent;
        iTmp = BKNI_Snprintf(&(szStringTmp[0]), 128,
                       "%010u:%04u:%c",
                       (pstSrc + usBufLogR)->ulTime,
                       usBufLogR,
                       (pstSrc + usBufLogR)->cLogType);

        if ((pstSrc + usBufLogR)->cLogType <= ((eBufLogState == BVDC_BufLogState_eAutomaticReduced) ? 'D' : 'G'))
        {
            iTmp += BKNI_Snprintf(&(szStringTmp[iTmp]), 128-iTmp,
                            "%u:%u[",
                            (pstSrc + usBufLogR)->BufLogData.stSkipRepeat.ulWindowId,
                            (pstSrc + usBufLogR)->BufLogData.stSkipRepeat.ulBufferId);
            for(ulBufCtr=0; ulBufCtr<((pstSrc + usBufLogR)->BufLogData.stSkipRepeat.ulBufferNum); ulBufCtr++)
            {
                iTmp += BKNI_Snprintf(&(szStringTmp[iTmp]), 128-iTmp,
                                "%c",
                                (pstSrc + usBufLogR)->BufLogData.stSkipRepeat.cRW[ulBufCtr]);
            }
            iTmp += BKNI_Snprintf(&(szStringTmp[iTmp]), 128-iTmp,
                            "]%u:%08x:%012u",
                            (pstSrc + usBufLogR)->BufLogData.stSkipRepeat.ulBufferNum,
                            (pstSrc + usBufLogR)->BufLogData.stSkipRepeat.ulInfo,
                            (pstSrc + usBufLogR)->BufLogData.stSkipRepeat.ulBufferTS);
            if ((pstSrc + usBufLogR)->cLogType <= 'D')
            {
                iTmp += BKNI_Snprintf(&(szStringTmp[iTmp]), 128-iTmp, "<===");  /* mark the skip/repeat event to ease reading of the logs */
            }
            BDBG_ERR(("%s", szStringTmp));

        }
        else if(eBufLogState != BVDC_BufLogState_eAutomaticReduced)
        {
            if((pstSrc + usBufLogR)->cLogType == 'J')
            {
                BDBG_ERR(("%s%u:R=%u W=%u",
                          szStringTmp,
                          (pstSrc + usBufLogR)->BufLogData.stGeneric.ulU0,
                          (pstSrc + usBufLogR)->BufLogData.stGeneric.ulU1,
                          (pstSrc + usBufLogR)->BufLogData.stGeneric.ulU2));

            }
            else if( ((pstSrc + usBufLogR)->cLogType == 'K') || ((pstSrc + usBufLogR)->cLogType == 'L') ||
                     ((pstSrc + usBufLogR)->cLogType == 'O') || ((pstSrc + usBufLogR)->cLogType == 'P') )
            {
                BDBG_ERR(("%s%u:%u:%012u:%012u",
                          szStringTmp,
                          (pstSrc + usBufLogR)->BufLogData.stGeneric.ulU0,
                          (pstSrc + usBufLogR)->BufLogData.stGeneric.ulU1,
                          (pstSrc + usBufLogR)->BufLogData.stGeneric.ulU2,
                          (pstSrc + usBufLogR)->BufLogData.stGeneric.ulU3));

            }
            else if( ((pstSrc + usBufLogR)->cLogType == 'M') || ((pstSrc + usBufLogR)->cLogType == 'N') )
            {
                BDBG_ERR(("%s%u:cnt=%u",
                          szStringTmp,
                          (pstSrc + usBufLogR)->BufLogData.stGeneric.ulU0,
                          (pstSrc + usBufLogR)->BufLogData.stGeneric.ulU1));

            }
            else if( ((pstSrc + usBufLogR)->cLogType == 'Q') )
            {
                BDBG_ERR(("%s%u:%012u:%012u",
                          szStringTmp,
                          (pstSrc + usBufLogR)->BufLogData.stGeneric.ulU0,
                          (pstSrc + usBufLogR)->BufLogData.stGeneric.ulU1,
                          (pstSrc + usBufLogR)->BufLogData.stGeneric.ulU2));

            }
            else
            {
                BDBG_ERR(("%s%u:%u:%u",
                          szStringTmp,
                          (pstSrc + usBufLogR)->BufLogData.stTimeStamp.ulWindowId,
                          (pstSrc + usBufLogR)->BufLogData.stTimeStamp.ulTimeDiff,
                          (pstSrc + usBufLogR)->BufLogData.stTimeStamp.ulNumCapField));
            }
        }

        ulOutputCtr++;
        if(ulOutputCtr >= BVDC_P_BUF_LOG_MAX_OUTPUT)
        {
            BDBG_ERR(("Reached output limit"));
            break;
        }
        else
        {
            /* compute the last entry - match to recent buffer entries only */
            stBufLog_IdxRLast = stBufLog_IdxLastSkipRepeat;
            BVDC_P_BUF_LOG_IDX_ADD(stBufLog_IdxRLast, BVDC_P_BUF_LOG_ENTRIES_PAD);

            /* adjust to the latest buffer entries only if necessary */
            if( stBufLog_IdxRLast.ulIdx > stBufLog_IdxW.ulIdx )
            {
                stBufLog_IdxRLast = stBufLog_IdxW;
            }

            /* break when stBufLog_IdxLastSkipRepeat doesn't move anymore */
            if(stBufLog_IdxR.ulIdx == stBufLog_IdxRLast.ulIdx)
            {
                break;
            }
            BVDC_P_BUF_LOG_IDX_INCREMENT(stBufLog_IdxR);
        }
    }
    BDBG_ERR(("<===BRCM"));                                   /* footer marker */
}


static void BVDC_P_BufLogAddEvent_isr( int type,
            	                       uint32_t v1,
                                       uint32_t v2,
                                       uint32_t v3,
                                       uint32_t v4,
                                       const BVDC_P_Buffer_Handle hBuffer
                                     )
{
    int      i, j;
	bool bSkipRepeatEvent = false;
    uint16_t usBufLogW;

    if(eBufLogState == BVDC_BufLogState_eReset)
        return;

	if(!bstBufLogEnable[v1])
	{
		return;
	}

    BVDC_P_BUF_LOG_IDX_INCREMENT(stBufLog_IdxW);
    usBufLogW = stBufLog_IdxW.stIdxPart.usIdxEvent;

	if (0 == stTmrRegs.status)
		BTMR_GetTimerRegisters(hBuffer->hWindow->hCompositor->hVdc->hTimer, &stTmrRegs);

    astBufLog[usBufLogW].cLogType = type;
    BTMR_ReadTimer_isr(hBuffer->hWindow->hCompositor->hVdc->hTimer, &(astBufLog[usBufLogW].ulTime));

    if (type <= 'G')                                            /* skip / repeat */
    {
        BVDC_P_PictureNode *pPicture;

        astBufLog[usBufLogW].BufLogData.stSkipRepeat.ulWindowId = v1;
        astBufLog[usBufLogW].BufLogData.stSkipRepeat.ulBufferId = v2;
        astBufLog[usBufLogW].BufLogData.stSkipRepeat.ulBufferNum = hBuffer->ulActiveBufCnt;
        astBufLog[usBufLogW].BufLogData.stSkipRepeat.ulBufferTS = v4;
        astBufLog[usBufLogW].BufLogData.stSkipRepeat.ulInfo = v3;

        pPicture = BLST_CQ_FIRST(hBuffer->pBufList);

        BKNI_Memset((void*)(astBufLog[usBufLogW].BufLogData.stSkipRepeat.cRW), ' ', sizeof(char) * BVDC_P_BUF_LOG_BUFFERS_NUM_MAX);

		/* ulBufCnt is the max number of buffer that could be allocated.  Usually it is 4.
            We go through the list and find the active ones.  cRW[index] cannot use i as index. */
        for(i = 0, j=0; i < (int)hBuffer->ulBufCnt; i++)
        {
            if(pPicture->stFlags.bActiveNode)
            {
                astBufLog[usBufLogW].BufLogData.stSkipRepeat.cRW[j++] = (pPicture == hBuffer->pCurReaderBuf) ? 'R'
                                                              : ((pPicture == hBuffer->pCurWriterBuf) ? 'W' : 'x');

                if (j==BVDC_P_BUF_LOG_BUFFERS_NUM_MAX)
                    break;
            }
            pPicture = BVDC_P_Buffer_GetNextNode(pPicture);
        }

        if(type <= 'D')                                         /* only trigger the dumps on skip/repeat events */
            bSkipRepeatEvent = true;
    }
    else if (type <= 'I')
    {
        astBufLog[usBufLogW].BufLogData.stTimeStamp.ulWindowId = v1;
        astBufLog[usBufLogW].BufLogData.stTimeStamp.ulTimeDiff = v2;
        astBufLog[usBufLogW].BufLogData.stTimeStamp.ulNumCapField = v3;
    }
    else
    {
        astBufLog[usBufLogW].BufLogData.stGeneric.ulU0 = v1;
        astBufLog[usBufLogW].BufLogData.stGeneric.ulU1 = v2;
        astBufLog[usBufLogW].BufLogData.stGeneric.ulU2 = v3;
        astBufLog[usBufLogW].BufLogData.stGeneric.ulU3 = v4;
    }

    if(bSkipRepeatEvent)
    {
        stBufLog_IdxLastSkipRepeat = stBufLog_IdxW;
        if(eBufLogState >= BVDC_BufLogState_eAutomatic)
        {
            if(pfBufLogCallback)
                pfBufLogCallback(pvBufLogCallbackParm1, iBufLogParm2, NULL);
            else
                BDBG_ERR(("BufLog-AutoTrigger: User callback function unavailable"));
        }
    }
}


void BVDC_P_Buffer_SetManualTrigger(void)
{
    if(pfBufLogCallback)
    {
        /* Compute the dump-starting point for the manual trigger */
        stBufLog_IdxLastSkipRepeat = stBufLog_IdxW;
        BVDC_P_BUF_LOG_IDX_SUB(stBufLog_IdxLastSkipRepeat, BVDC_P_BUF_LOG_ENTRIES_PAD);

        /* Execute the callback */
        pfBufLogCallback(pvBufLogCallbackParm1, iBufLogParm2, NULL);
    }
    else
    {
        BDBG_ERR(("BufLog-ManualTrigger: User callback function unavailable"));
    }
}

void BVDC_P_Buffer_EnableBufLog
	( BVDC_P_WindowId         eId,
	  bool                    bEnable)
{
	bstBufLogEnable[eId] = bEnable;
	BDBG_MSG(("BVDC_P_Buffer_EnableBufLog: %d %d", eId, bEnable));
}

#endif


/* VEC alignment may have 200 usecs error, RUL sampling of timestamp may have
   another +/-100 usecs error; worst case double-buffer reader/writer pointers
   timestamps relative error may be up to ~400usecs; */
#define BVDC_P_DOUBLE_BUFFER_TS_TOLERANCE   (\
	2 * BVDC_P_MULTIBUFFER_RW_TOLERANCE + BVDC_P_USEC_ALIGNMENT_THRESHOLD)

static void BVDC_P_Buffer_UpdateTimestamps_isr
	( BVDC_Window_Handle               hWindow,
	  const BAVC_Polarity              eSrcPolarity,
	  const BAVC_Polarity              eDispPolarity );

static void BVDC_P_Buffer_CheckWriterIsrOrder_isr
	( BVDC_Window_Handle               hWindow,
	  const BAVC_Polarity              eSrcPolarity,
	  const BAVC_Polarity              eDispPolarity );

static void BVDC_P_Buffer_MoveSyncSlipWriterNode_isr
	( BVDC_Window_Handle               hWindow,
	  const BAVC_Polarity              eSrcPolarity);

static void BVDC_P_Buffer_CheckReaderIsrOrder_isr
	( BVDC_Window_Handle               hWindow,
	  const BAVC_Polarity              eSrcPolarity,
	  const BAVC_Polarity              eDispPolarity );

static void BVDC_P_Buffer_MoveSyncSlipReaderNode_isr
	( BVDC_Window_Handle               hWindow,
	  const BAVC_Polarity              eVecPolarity);


/***************************************************************************
 *
 */
static BVDC_P_PictureNode *BVDC_P_GetWriterByDelay
	( BVDC_P_PictureNode     *pCurReader,
	  uint32_t                ulVsyncDelay )
{
	uint32_t             i = 0;
	BVDC_P_PictureNode  *pTempNode = pCurReader;

	while( i++ < ulVsyncDelay )
	{
		BVDC_P_Buffer_GetNextActiveNode(pTempNode, pTempNode);
	}

	return pTempNode;
}


/***************************************************************************
 *
 */
static BVDC_P_PictureNode *BVDC_P_GetReaderByDelay
	 ( BVDC_P_PictureNode             *pCurWriter,
	   uint32_t                        ulVsyncDelay )
{
	uint32_t i = 0;
	BVDC_P_PictureNode  *pTempNode = pCurWriter;

	while( i++ < ulVsyncDelay )
	{
		BVDC_P_Buffer_GetPrevActiveNode(pTempNode, pTempNode);
	}

	return pTempNode;
}

/***************************************************************************
  * {private}
  * To get the current reader->writer buffer delay;
  */
static uint32_t BVDC_P_Buffer_GetCurrentDelay
	( BVDC_Window_Handle               hWindow )
{
	BVDC_P_PictureNode		*pTempNode;

	uint32_t delay_count;

	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	BDBG_OBJECT_ASSERT(hWindow->hBuffer, BVDC_BUF);

	/* So far there are 3 cases that we allow reader and writer point
	 * to the same buffer node:
	 *
	 * 1) game mode: buffer delay is 0;
	 * 2) progressive pull down: buffer delay is the count between reader and writer;
	 * 3) flag bRepeatCurrReader is set when adding buffer nodes: buffer delay is 0.
	 */
	if (hWindow->hBuffer->pCurReaderBuf == hWindow->hBuffer->pCurWriterBuf)
	{
		bool bProgressivePullDown;

		/* buffer delay is always 0 if bRepeatCurrReader is set */
		if (hWindow->bRepeatCurrReader)
			return 0;
		/* Check whether it is caused by progress pull down */
		BVDC_P_Buffer_CalculateRateGap(hWindow->stCurInfo.hSource->ulVertFreq,
			hWindow->hCompositor->stCurInfo.pFmtInfo->ulVertFreq,
			&hWindow->hBuffer->eWriterVsReaderRateCode, &hWindow->hBuffer->eReaderVsWriterRateCode);

		/* forced synclocked double-buffer might have interlaced pulldown reader overlapped with writer pointer */
		bProgressivePullDown =	VIDEO_FORMAT_IS_PROGRESSIVE(hWindow->stCurInfo.hSource->stCurInfo.pFmtInfo->eVideoFmt) &&
								(hWindow->hBuffer->eWriterVsReaderRateCode == BVDC_P_WrRate_NotFaster) &&
								(hWindow->hBuffer->eReaderVsWriterRateCode == BVDC_P_WrRate_2TimesFaster);

		if (!bProgressivePullDown)
			return 0;
	}

	delay_count = 1;
	BVDC_P_Buffer_GetNextActiveNode(pTempNode, hWindow->hBuffer->pCurReaderBuf);

	while (pTempNode != hWindow->hBuffer->pCurWriterBuf)
	{
		BVDC_P_Buffer_GetNextActiveNode(pTempNode, pTempNode);
		delay_count++;
	}

	return delay_count;
}


/***************************************************************************
 * {private}
 *
 */
static void BVDC_P_Buffer_InitPictureNode
	( BVDC_P_PictureNode              *pPicture )
{
	/* Fill-in the default info for each node. */
	pPicture->pHeapNode            = NULL;
	pPicture->pHeapNode_R          = NULL;
	BKNI_Memset((void*)&pPicture->stFlags, 0x0, sizeof(BVDC_P_PicNodeFlags));
	pPicture->stFlags.bMute           = true;
	pPicture->stFlags.bCadMatching    = false;

	BVDC_P_CLEAN_ALL_DIRTY(&(pPicture->stVnetMode));
	pPicture->stVnetMode.stBits.bInvalid  = BVDC_P_ON;
	pPicture->eFrameRateCode       = BAVC_FrameRateCode_e29_97;
	pPicture->eMatrixCoefficients  = BAVC_MatrixCoefficients_eSmpte_170M;
	pPicture->eDisplayPolarity     = BAVC_Polarity_eTopField;
	pPicture->eSrcPolarity         = BAVC_Polarity_eTopField;
	pPicture->eSrcOrientation      = BFMT_Orientation_e2D;
	pPicture->eDispOrientation     = BFMT_Orientation_e2D;
	pPicture->PicComRulInfo.eSrcOrigPolarity = BAVC_Polarity_eTopField;
	pPicture->eDstPolarity         = BAVC_Polarity_eTopField;
	pPicture->hSurface             = NULL;
	pPicture->hSurface_R           = NULL;

	pPicture->ulVdecPhase          = 0;

	pPicture->stSrcOut.lLeft       = 0;
	pPicture->stSrcOut.lLeft_R     = 0;
	pPicture->stSrcOut.lTop        = 0;
	pPicture->stSrcOut.ulWidth     = BFMT_NTSC_WIDTH;
	pPicture->stSrcOut.ulHeight    = BFMT_NTSC_HEIGHT;

	pPicture->stSclOut             = pPicture->stSrcOut;
	pPicture->stWinOut             = pPicture->stSrcOut;
	pPicture->stSclCut             = pPicture->stSrcOut;
	pPicture->stCapOut             = pPicture->stSrcOut;
	pPicture->stVfdOut             = pPicture->stSrcOut;
	pPicture->stMadOut             = pPicture->stSrcOut;

	pPicture->pSrcOut              = &pPicture->stSrcOut;
	pPicture->pSclOut              = &pPicture->stSclOut;
	pPicture->pWinOut              = &pPicture->stWinOut;
	pPicture->pVfdOut              = &pPicture->stVfdOut;
	pPicture->pVfdIn               = &pPicture->stVfdOut;

	pPicture->pDnrOut              = NULL;
	pPicture->pAnrOut              = NULL;
	pPicture->pMadOut              = NULL;
	pPicture->pCapOut              = NULL;
	pPicture->pDnrIn               = NULL;
	pPicture->pAnrIn               = NULL;
	pPicture->pMadIn               = NULL;
	pPicture->pSclIn               = NULL;
	pPicture->pCapIn               = NULL;

	pPicture->pFgtIn               = NULL;
	pPicture->pFgtOut              = NULL;
	pPicture->pWinIn               = NULL;

	pPicture->ulCaptureTimestamp   = 0;
	pPicture->ulPlaybackTimestamp  = 0;
	pPicture->ulFgtSeiBufAddr      = 0;
	pPicture->ulIdrPicID           = 0;
	pPicture->ulPicOrderCnt        = 0;

	/* used for NRT mode transcode: default true to freeze STC and not get encoded */
	pPicture->bIgnorePicture       = true;
	pPicture->bStallStc            = true;

	BKNI_Memset((void*)&pPicture->stCapTestFeature1,
		0x0, sizeof(BVDC_P_TestFeature1_Settings));

	return;
}


/***************************************************************************
 * {private}
 *
 * This function creates a multi-buffer list with zero (0) node.
 */
BERR_Code BVDC_P_Buffer_Create
	( const BVDC_Window_Handle         hWindow,
	  BVDC_P_Buffer_Handle            *phBuffer )
{
	uint32_t i;
	BVDC_P_BufferContext *pBuffer;

	BDBG_ENTER(BVDC_P_Buffer_Create);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	/* BDBG_SetModuleLevel("BVDC_BUF", BDBG_eMsg);  */

	/* (1) Create buffer context */
	pBuffer = (BVDC_P_BufferContext*)BKNI_Malloc(sizeof(BVDC_P_BufferContext));
	if(!pBuffer)
	{
		return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
	}

	/* Clear out context */
	BKNI_Memset((void*)pBuffer, 0x0, sizeof(BVDC_P_BufferContext));
	BDBG_OBJECT_SET(pBuffer, BVDC_BUF);

	/* These fields do not change during runtime. */
	pBuffer->hWindow      = hWindow;
	pBuffer->ulBufCnt     = BVDC_P_MAX_MULTI_BUFFER_COUNT;

	/* (2) Create buffer head */
	pBuffer->pBufList =
		(BVDC_P_Buffer_Head*)BKNI_Malloc(sizeof(BVDC_P_Buffer_Head));
	if(!pBuffer->pBufList)
	{
		BDBG_OBJECT_DESTROY(pBuffer, BVDC_BUF);
		BKNI_Free(pBuffer);
		return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
	}
	BLST_CQ_INIT(pBuffer->pBufList);

	/* (3) Create picture nodes */
	for(i = 0; i < pBuffer->ulBufCnt; i++)
	{
		BVDC_P_PictureNode *pPicture =
			(BVDC_P_PictureNode*)BKNI_Malloc(sizeof(BVDC_P_PictureNode));
		if(!pPicture)
		{
			while(i--)
			{
				pPicture = BLST_CQ_FIRST(pBuffer->pBufList);
				BLST_CQ_REMOVE_HEAD(pBuffer->pBufList, link);
				BKNI_Free(pPicture);
			}
			BDBG_OBJECT_DESTROY(pBuffer, BVDC_BUF);
			BKNI_Free(pBuffer->pBufList);
			BKNI_Free(pBuffer);
			return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
		}

		/* Clear out, insert it into the list. */
		BKNI_Memset((void*)pPicture, 0x0, sizeof(BVDC_P_PictureNode));

		/* Initialize non-changing fields. */
		pPicture->hBuffer    = (BVDC_P_Buffer_Handle)pBuffer;
		pPicture->ulBufferId = i;

		BLST_CQ_INSERT_TAIL(pBuffer->pBufList, pPicture, link);
	}
	/* All done. now return the new fresh context to user. */
	*phBuffer = (BVDC_P_Buffer_Handle)pBuffer;

	BDBG_LEAVE(BVDC_P_Buffer_Create);
	return BERR_SUCCESS;
}

/***************************************************************************
 * {private}
 *
 */
BERR_Code BVDC_P_Buffer_Destroy
	( BVDC_P_Buffer_Handle             hBuffer )
{
	BVDC_P_PictureNode         *pPicture;

	BDBG_ENTER(BVDC_P_Buffer_Destroy);
	BDBG_OBJECT_ASSERT(hBuffer, BVDC_BUF);

	/* [3] Free memory for individual buffer node */
	while(hBuffer->ulBufCnt--)
	{
		pPicture = BLST_CQ_FIRST(hBuffer->pBufList);
		BLST_CQ_REMOVE_HEAD(hBuffer->pBufList, link);
		BKNI_Free(pPicture);
	}

	/* [2] Free memory for buffer head. */
	BKNI_Free((void*)hBuffer->pBufList);

	BDBG_OBJECT_DESTROY(hBuffer, BVDC_BUF);
	/* [1] Free memory for main context. */
	BKNI_Free((void*)hBuffer);

	BDBG_LEAVE(BVDC_P_Buffer_Destroy);
	return BERR_SUCCESS;
}


/***************************************************************************
 * {private}
 *
 */
void BVDC_P_Buffer_Init
	( BVDC_P_Buffer_Handle             hBuffer )
{
	uint32_t i;
	BVDC_P_PictureNode   *pPicture;

	BDBG_ENTER(BVDC_P_Buffer_Init);
	BDBG_OBJECT_ASSERT(hBuffer, BVDC_BUF);

	/* Re-Initialize fields that may changes during previous run. */
	hBuffer->bSyncLock        = false;
	hBuffer->ulSkipCnt        = 0;
	hBuffer->ulNumCapField    = 0;
	hBuffer->ulActiveBufCnt   = 0;
	hBuffer->ulVsyncDelay     = 0;
	hBuffer->eWriterVsReaderRateCode  = BVDC_P_WrRate_NotFaster;
	hBuffer->eReaderVsWriterRateCode  = BVDC_P_WrRate_NotFaster;
	hBuffer->eLastBuffAction  = BVDC_P_Last_Buffer_Action_Reader_Moved;

	/* For reader and writer ISR ordering */
	hBuffer->bWriterNodeMovedByReader = false;
	hBuffer->bReaderNodeMovedByWriter = false;
	hBuffer->ulPrevWriterTimestamp = 0;
	hBuffer->ulCurrWriterTimestamp = 0;
	hBuffer->ulPrevReaderTimestamp = 0;
	hBuffer->ulCurrReaderTimestamp = 0;
	hBuffer->bReaderWrapAround = false;
	hBuffer->bWriterWrapAround = false;

#if (BVDC_P_USE_RDC_TIMESTAMP)
	hBuffer->ulMaxTimestamp = BRDC_GetTimerMaxValue(hBuffer->hWindow->hCompositor->hVdc->hRdc);
#else
	hBuffer->ulMaxTimestamp = BTMR_ReadTimerMax();
#endif

	hBuffer->ulGameDelaySamplePeriod = 1;
	hBuffer->ulGameDelaySampleCnt    = 0;

	/* Keep track of skip/repeat statistics */
	hBuffer->ulSkipStat       = 0;
	hBuffer->ulRepeatStat     = 0;

	/* Default for reader and writer. */
	hBuffer->pCurReaderBuf    = BLST_CQ_FIRST(hBuffer->pBufList);
	hBuffer->pCurWriterBuf    = hBuffer->pCurReaderBuf;

	/* Initialize all the picture nodes. */
	pPicture = hBuffer->pCurReaderBuf;
	for(i = 0; i < hBuffer->ulBufCnt; i++)
	{
		BVDC_P_Buffer_InitPictureNode(pPicture);
		pPicture = BVDC_P_Buffer_GetNextNode(pPicture);
	}

	BKNI_Memset((void*)hBuffer->aBufAdded, 0x0, sizeof(hBuffer->aBufAdded));
	hBuffer->iLastAddedBufIndex = 0;

#if BVDC_P_REPEAT_ALGORITHM_ONE
	hBuffer->bRepeatForGap = false;
#endif

	BDBG_LEAVE(BVDC_P_Buffer_Init);
}


/***************************************************************************
 * {private}
 *
 * Add additioanl picture node to the buffer context.
 *
 * ahSurface is the array of all the surface allocated for the buffer/window.
 * Totally ulSurfaceCount surfaces will be added. The index of surfaces added
 * are:
 * hBuffer->ulActiveBufCnt ... hBuffer->ulActiveBufCnt + ulSurfaceCount - 1
 */
BERR_Code BVDC_P_Buffer_AddPictureNodes_isr
	( BVDC_P_Buffer_Handle             hBuffer,
	  BVDC_P_HeapNodePtr               apHeapNode[],
	  BVDC_P_HeapNodePtr               apHeapNode_R[],
	  uint32_t                         ulSurfaceCount,
	  uint32_t                         ulBufDelay,
	  bool                             bSyncLock,
	  bool                             bInvalidate)
{
	uint32_t                    i;
	BVDC_P_PictureNode         *pPicture;
	BERR_Code                   err = BERR_SUCCESS;

#if (!BVDC_P_SUPPORT_3D_VIDEO)
	BSTD_UNUSED(apHeapNode_R);
#endif

	BDBG_ENTER(BVDC_P_Buffer_AddPictureNodes_isr);
	BDBG_OBJECT_ASSERT(hBuffer, BVDC_BUF);

	if(ulSurfaceCount + hBuffer->ulActiveBufCnt > BVDC_P_MAX_MULTI_BUFFER_COUNT)
	{
		BDBG_ERR(("More than MAX!"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* Update delay */
	hBuffer->ulVsyncDelay = ulBufDelay;

	/* 1) always add the new buffer node right after the current writer*/
	for(i = 0; i < ulSurfaceCount; i++)
	{
		BDBG_ASSERT(apHeapNode);
		BDBG_ASSERT(apHeapNode[i]);

		pPicture = BVDC_P_Buffer_GetNextNode(hBuffer->pCurWriterBuf);

		while(pPicture->stFlags.bActiveNode || pPicture->stFlags.bUsedByUser)
		{
			pPicture = BVDC_P_Buffer_GetNextNode(pPicture);
		}

		/* Get a non active node */
		pPicture->pHeapNode        = apHeapNode[i];
#if (BVDC_P_SUPPORT_3D_VIDEO)
		if(apHeapNode_R)
			pPicture->pHeapNode_R      = apHeapNode_R[i];
		else
			pPicture->pHeapNode_R      = NULL;
#endif
		pPicture->stFlags.bActiveNode      = true;
		pPicture->stFlags.bMute            = true;
		pPicture->eDisplayPolarity = BAVC_Polarity_eTopField;
		pPicture->eSrcOrientation  = BFMT_Orientation_e2D;
		pPicture->eDispOrientation = BFMT_Orientation_e2D;

		/* We will have to reposition this newly allocated picture node.
		 */
		/* Take the node out from the buffer chain.
		 */
		BLST_CQ_REMOVE(hBuffer->pBufList, pPicture, link);

		/* Add this node back to the chain. Place it to be
		 * the one right after the current writer.
		 */
		BLST_CQ_INSERT_AFTER(hBuffer->pBufList, hBuffer->pCurWriterBuf, pPicture, link);

		hBuffer->ulActiveBufCnt++;
		hBuffer->aBufAdded[hBuffer->iLastAddedBufIndex] = pPicture;
		hBuffer->iLastAddedBufIndex++;

		/* Buffer initialization, point current writer to a active buffer */
		if (!hBuffer->pCurWriterBuf->stFlags.bActiveNode)
			hBuffer->pCurWriterBuf = pPicture;

#if (BVDC_BUF_LOG == 1)
		BVDC_P_BufLogAddEvent_isr('M',
							hBuffer->hWindow->eId,
							hBuffer->ulActiveBufCnt,
							0,
							0,
							hBuffer);
#else

		BDBG_MSG(("Add buffer heap node 0x%lx (%s %2d) to B%d",
			apHeapNode[i],
			BVDC_P_BUFFERHEAP_GET_HEAP_ID_NAME(apHeapNode[i]->pHeapInfo->eBufHeapId),
			apHeapNode[i]->ulBufIndex, pPicture->ulBufferId));
#if (BVDC_P_SUPPORT_3D_VIDEO)
		if(apHeapNode_R)
		{
			BDBG_MSG(("Add Right buffer heap node 0x%lx (%s %2d) to B%d",
				apHeapNode_R[i],
				BVDC_P_BUFFERHEAP_GET_HEAP_ID_NAME(apHeapNode_R[i]->pHeapInfo->eBufHeapId),
				apHeapNode_R[i]->ulBufIndex, pPicture->ulBufferId));
		}
#endif
#endif
	}

	hBuffer->bSyncLock = bSyncLock;

	if (bInvalidate)
	{
		hBuffer->pCurReaderBuf = BVDC_P_GetReaderByDelay(
			hBuffer->pCurWriterBuf, ulBufDelay);
	}
	else
	{
		/* 2) Set repeat current reader flag until buffer delay is reached;
		   force reader repeat until the reader/writer buffer delay catches up
		   with hBuffer->ulVsyncDelay; resume normal movement of buffer pointers
		   afterwards. */
		hBuffer->hWindow->bRepeatCurrReader = true;
	}

	BDBG_LEAVE(BVDC_P_Buffer_AddPictureNodes_isr);
	return err;
}


/***************************************************************************
 *
 */
static BVDC_P_PictureNode *BVDC_P_Buffer_LastAddedNonUserCaptureNode
	( BVDC_P_Buffer_Handle             hBuffer )
{
	int tempIdx, i;
	BVDC_P_PictureNode *pPicture;

	BDBG_OBJECT_ASSERT(hBuffer, BVDC_BUF);

	/* Locate the last added but not used for user capture buffer
	 * node.
	 */
	tempIdx = hBuffer->iLastAddedBufIndex - 1;

	while(hBuffer->aBufAdded[tempIdx]->stFlags.bUsedByUser)
	{
		BDBG_ASSERT(tempIdx);
		tempIdx--;
	}

	pPicture = hBuffer->aBufAdded[tempIdx];

	/* Shift all the nodes before this one down so that
	 * there is no hole in aBufAdded[] array.
	 */
	for (i = tempIdx; i < (hBuffer->iLastAddedBufIndex - 1); i++)
		hBuffer->aBufAdded[i] = hBuffer->aBufAdded[i+1];

	return pPicture;

}



/***************************************************************************
 * {private}
 *
 * Release picture nodes from the buffer context.
 *
 * ahSurface is the array of all the surface allocated for the buffer/window.
 * Totally ulSurfaceCount surfaces will be released. The index of surfaces
 * released are:
 * pBuffer->ulActiveBufCnt - ulSurfaceCount... pBuffer->ulActiveBufCnt - 1
 *
 */
BERR_Code BVDC_P_Buffer_ReleasePictureNodes_isr
	( BVDC_P_Buffer_Handle             hBuffer,
	  BVDC_P_HeapNodePtr               apHeapNode[],
	  BVDC_P_HeapNodePtr               apHeapNode_R[],
	  uint32_t                         ulSurfaceCount,
	  uint32_t                         ulBufDelay)
{
	uint32_t                    i;
	BVDC_P_PictureNode         *pBufferToRemove;
	BERR_Code                   err = BERR_SUCCESS;

#if (!BVDC_P_SUPPORT_3D_VIDEO)
	BSTD_UNUSED(apHeapNode_R);
#endif

	BDBG_ENTER(BVDC_P_Buffer_ReleasePictureNodes_isr);
	BDBG_OBJECT_ASSERT(hBuffer, BVDC_BUF);

	if(hBuffer->ulActiveBufCnt < ulSurfaceCount)
	{
		BDBG_ERR(("Less than MIN!"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	hBuffer->ulVsyncDelay = ulBufDelay;

	/* Always remove the last added buffer node, but it
	 * must not be marked as user capture buffer
	 */
	for(i = 0; i < ulSurfaceCount; i++)
	{
		BDBG_ASSERT(hBuffer->iLastAddedBufIndex);

		pBufferToRemove = BVDC_P_Buffer_LastAddedNonUserCaptureNode(hBuffer);
		pBufferToRemove->stFlags.bActiveNode = false;
		apHeapNode[i] = pBufferToRemove->pHeapNode;
#if (BVDC_P_SUPPORT_3D_VIDEO)
		if(apHeapNode_R)
			apHeapNode_R[i] = pBufferToRemove->pHeapNode_R;
#endif

#if (BVDC_BUF_LOG == 1)
		hBuffer->iLastAddedBufIndex--;
		hBuffer->ulActiveBufCnt--;

		BVDC_P_BufLogAddEvent_isr('N',
							hBuffer->hWindow->eId,
							hBuffer->ulActiveBufCnt,
							0,
							0,
							hBuffer);
#else
		BDBG_MSG(("Release buffer heap node 0x%lx (%s %2d) to B%d",
			pBufferToRemove->pHeapNode,
			BVDC_P_BUFFERHEAP_GET_HEAP_ID_NAME(apHeapNode[i]->pHeapInfo->eBufHeapId),
			pBufferToRemove->pHeapNode->ulBufIndex, pBufferToRemove->ulBufferId));
#if (BVDC_P_SUPPORT_3D_VIDEO)
		if(apHeapNode_R)
		{
			BDBG_MSG(("Release Right buffer heap node 0x%lx (%s %2d) to B%d",
				pBufferToRemove->pHeapNode_R,
				BVDC_P_BUFFERHEAP_GET_HEAP_ID_NAME(apHeapNode_R[i]->pHeapInfo->eBufHeapId),
				pBufferToRemove->pHeapNode->ulBufIndex, pBufferToRemove->ulBufferId));
		}
#endif

		hBuffer->iLastAddedBufIndex--;
		hBuffer->ulActiveBufCnt--;
#endif
	}

	if(hBuffer->ulActiveBufCnt)
	{
		/* Current reader and writer nodes might have been released
		 * during the above process. So we have to reposition them.
		 */
		while(!hBuffer->pCurWriterBuf->stFlags.bActiveNode)
			BVDC_P_Buffer_GetNextActiveNode(hBuffer->pCurWriterBuf, hBuffer->pCurWriterBuf);

		hBuffer->pCurReaderBuf = BVDC_P_GetReaderByDelay(
			hBuffer->pCurWriterBuf, ulBufDelay);
	}
	else
	{
		hBuffer->pCurReaderBuf = BLST_CQ_FIRST(hBuffer->pBufList);
		hBuffer->pCurWriterBuf = hBuffer->pCurReaderBuf;
	}

	BDBG_LEAVE(BVDC_P_Buffer_ReleasePictureNodes_isr);
	return err;
}



#if (BVDC_P_SUPPORT_3D_VIDEO)
/***************************************************************************
 * {private}
 * Add or free picture node for right capture buffer.
 *
 */
BERR_Code BVDC_P_Buffer_SetRightBufferPictureNodes_isr
	( BVDC_P_Buffer_Handle             hBuffer,
	  BVDC_P_HeapNodePtr               apHeapNode_R[],
	  uint32_t                         ulCount,
	  bool                             bAdd)
{
	uint32_t                    i;
	BVDC_P_PictureNode         *pPicture;
	BERR_Code                   err = BERR_SUCCESS;

	BDBG_ENTER(BVDC_P_Buffer_SetRightBufferPictureNodes_isr);
	BDBG_OBJECT_ASSERT(hBuffer, BVDC_BUF);

	BDBG_ASSERT(ulCount == hBuffer->ulActiveBufCnt);

	/* Start with current writer*/
	pPicture = hBuffer->pCurWriterBuf;

	if(bAdd)
	{
		for(i = 0; i < ulCount; i++)
		{
			/* Get next active node */
			while(!pPicture->stFlags.bActiveNode || pPicture->pHeapNode_R)
			{
				BVDC_P_Buffer_GetNextActiveNode(pPicture, pPicture);
			}
			pPicture->pHeapNode_R = apHeapNode_R[i];
			BVDC_P_BUF_MSG(("Add Right buffer heap node 0x%lx (%s %2d) to B%d (%d)",
				apHeapNode_R[i],
				BVDC_P_BUFFERHEAP_GET_HEAP_ID_NAME(apHeapNode_R[i]->pHeapInfo->eBufHeapId),
				apHeapNode_R[i]->ulBufIndex, pPicture->ulBufferId,
				pPicture->stFlags.bMute));
		}
	}
	else
	{
		for(i = 0; i < ulCount; i++)
		{
			/* Get next active node */
			while(!pPicture->stFlags.bActiveNode || (pPicture->pHeapNode_R == NULL))
			{
				BVDC_P_Buffer_GetNextActiveNode(pPicture, pPicture);
			}
			BVDC_P_BUF_MSG(("Free Right buffer heap node 0x%lx (%s %2d) from B%d",
				pPicture->pHeapNode_R,
				BVDC_P_BUFFERHEAP_GET_HEAP_ID_NAME(pPicture->pHeapNode_R->pHeapInfo->eBufHeapId),
				pPicture->pHeapNode_R->ulBufIndex, pPicture->ulBufferId));
			apHeapNode_R[i] = pPicture->pHeapNode_R;
			pPicture->pHeapNode_R = NULL;
		}
	}


	BDBG_LEAVE(BVDC_P_Buffer_SetRightBufferPictureNodes_isr);
	return err;
}

#endif

/***************************************************************************
 * {private}
 *
 */
BERR_Code BVDC_P_Buffer_Invalidate_isr
	( BVDC_P_Buffer_Handle             hBuffer )
{
	uint32_t                    ulCount;
	BVDC_P_PictureNode         *pTempNode;

	BDBG_ENTER(BVDC_P_Buffer_Invalidate_isr);
	BDBG_OBJECT_ASSERT(hBuffer, BVDC_BUF);

	BDBG_MSG(("Invalidate buffer nodes"));
	/* Invalidate all capture data */
	pTempNode = hBuffer->pCurReaderBuf;
	for( ulCount = 0; ulCount < hBuffer->ulBufCnt; ulCount++ )
	{
		pTempNode->eDstPolarity               = BAVC_Polarity_eTopField;
		pTempNode->eDisplayPolarity           = BAVC_Polarity_eTopField;
		pTempNode->eOrigSrcOrientation        = BFMT_Orientation_e2D;
		pTempNode->eSrcOrientation            = BFMT_Orientation_e2D;
		pTempNode->eDispOrientation           = BFMT_Orientation_e2D;
		pTempNode->stFlags.bMute              = true;
		pTempNode->stFlags.bMuteMad           = false;
		pTempNode->stFlags.bPictureRepeatFlag = false;
		pTempNode->stFlags.bRepeatField       = false;
		pTempNode->stFlags.bCadMatching       = false;
		pTempNode->ulAdjQp                    = 0;
		pTempNode->ulCaptureTimestamp         = 0;
		pTempNode->ulPlaybackTimestamp        = 0;
		pTempNode = BVDC_P_Buffer_GetNextNode(pTempNode);
	}

	/* reset capture number */
	hBuffer->ulNumCapField  = 0;
	hBuffer->ulSkipCnt      = 0;
	hBuffer->pCurWriterBuf = BVDC_P_GetWriterByDelay(
		hBuffer->pCurReaderBuf, hBuffer->ulVsyncDelay);
	hBuffer->eLastBuffAction = BVDC_P_Last_Buffer_Action_Reader_Moved;
	hBuffer->bWriterNodeMovedByReader = false;
	hBuffer->bReaderNodeMovedByWriter = false;
	hBuffer->ulPrevWriterTimestamp = 0;
	hBuffer->ulCurrWriterTimestamp = 0;
	hBuffer->ulPrevReaderTimestamp = 0;
	hBuffer->ulCurrReaderTimestamp = 0;

#if BVDC_P_REPEAT_ALGORITHM_ONE
	hBuffer->bRepeatForGap = false;
#endif

#if (BVDC_BUF_LOG == 1)
	BVDC_P_BufLogAddEvent_isr('J',
						hBuffer->hWindow->eId,
						hBuffer->pCurReaderBuf->ulBufferId,
						hBuffer->pCurWriterBuf->ulBufferId,
						0,
						hBuffer);
#else

	BDBG_MSG(("Set reader buffer node to B%d", hBuffer->pCurReaderBuf->ulBufferId));
	BDBG_MSG(("Set writer buffer node to B%d", hBuffer->pCurWriterBuf->ulBufferId));
#endif

	BDBG_LEAVE(BVDC_P_Buffer_Invalidate_isr);
	return BERR_SUCCESS;
}


/***************************************************************************
 *
 */
BVDC_P_PictureNode* BVDC_P_Buffer_GetPrevWriterNode_isr
	( const BVDC_P_Buffer_Handle       hBuffer )
{
	BVDC_P_PictureNode     *pPrevNode;

	BDBG_OBJECT_ASSERT(hBuffer, BVDC_BUF);

	if(!hBuffer->ulActiveBufCnt)
		return hBuffer->pCurWriterBuf;

	BVDC_P_Buffer_GetPrevActiveNode(pPrevNode, hBuffer->pCurWriterBuf);
	return pPrevNode;
}

/***************************************************************************
 *
 */
void BVDC_P_Buffer_SetCurWriterNode_isr
	( const BVDC_P_Buffer_Handle       hBuffer,
	  BVDC_P_PictureNode              *pPicture )
{
	BDBG_OBJECT_ASSERT(hBuffer, BVDC_BUF);

	hBuffer->pCurWriterBuf = pPicture;
}

/***************************************************************************
 *
 */
void BVDC_P_Buffer_SetCurReaderNode_isr
	( const BVDC_P_Buffer_Handle       hBuffer,
	  BVDC_P_PictureNode              *pPicture )
{
	BDBG_OBJECT_ASSERT(hBuffer, BVDC_BUF);

	hBuffer->pCurReaderBuf = pPicture;
}

/***************************************************************************
 *
 */
void  BVDC_P_Buffer_ReturnBuffer_isr
	( BVDC_P_Buffer_Handle            hBuffer,
	  BVDC_P_PictureNode             *pPicture )
{
	BVDC_P_PictureNode *pTempNode, *pNextTempNode;

	BDBG_OBJECT_ASSERT(hBuffer, BVDC_BUF);

	pPicture->stFlags.bUsedByUser = false;
	pPicture->hSurface = NULL;
	pPicture->stFlags.bMute = true;

	/* TODO: if returned buf heapId is not the same, then free it and re-alloc
	 * a buf with right heapId */

	/* If the chain has no active node, it means we are not doing
	 * capture any more. Then mark the picture node as not used by user
	 * and not active.
	 */
	if (hBuffer->ulActiveBufCnt == 0)
	{
		pPicture->stFlags.bActiveNode = false;
	}
	else
	{
		/* We will have to reposition this returned picture node.
		 * It can not be inserted between reader and writer since
		 * that will affect vsync delay. So we insert it right
		 * after the current writer.
		 */

		/* Take the node out from the buffer chain.
		 */
		BLST_CQ_REMOVE(hBuffer->pBufList, pPicture, link);

		/* Add this node back to the chain. Place it to be
		 * the one right after the current writer.
		 */
		pNextTempNode = BVDC_P_Buffer_GetNextNode(hBuffer->pCurWriterBuf);
		pPicture->eDstPolarity = pNextTempNode->eDstPolarity;
		BLST_CQ_INSERT_AFTER(hBuffer->pBufList, hBuffer->pCurWriterBuf, pPicture, link);

		/* Increment active buffer count */
		pPicture->stFlags.bActiveNode = true;
		hBuffer->ulActiveBufCnt++;

		/* Toggle the picture node destination polarity pointed
		 * to by the nodes after newly adde node but before the reader. This
		 * is necessary to keep the field prediction correct.
		 */
		BVDC_P_Buffer_GetNextActiveAndNotUsedByUserNode(pTempNode, pPicture);

		while (pTempNode != hBuffer->pCurReaderBuf)
		{
			pTempNode->eDstPolarity = BVDC_P_NEXT_POLARITY(pTempNode->eDstPolarity);
			BVDC_P_Buffer_GetNextActiveAndNotUsedByUserNode(pNextTempNode, pTempNode);
			pTempNode = pNextTempNode;
		} ;
	}

	return;
}


/***************************************************************************
 *
 */
BERR_Code  BVDC_P_Buffer_ExtractBuffer_isr
	( BVDC_P_Buffer_Handle            hBuffer,
	  BVDC_P_PictureNode            **ppPicture )
{
	BVDC_P_PictureNode *pTempNode = NULL, *pNextTempNode = NULL, *pPrevWriterNode = NULL;

	BDBG_OBJECT_ASSERT(hBuffer, BVDC_BUF);

	/* Criterion for such a node:
	 * 1) Active
	 * 2) Not used by user
	 * 3) Not current writer buffer
	 * 4) Not the previous writer buffer which could be in use
	 * 5) Not between reader and writer because taking one of those out will affect
	 *	  lipsync delay.
	 * 6) The buffer is not muted.
	 * 7) The buffer's pixel format matches with user specified.
	 *
	 * The one after current writer should satisfy the above criterion.
	 */

	BVDC_P_Buffer_GetNextActiveAndNotUsedByUserNode(pTempNode, hBuffer->pCurWriterBuf);
	BVDC_P_Buffer_GetPrevActiveAndNotUsedByUserNode(pPrevWriterNode, hBuffer->pCurWriterBuf);

	if ((pTempNode == hBuffer->pCurWriterBuf) || (pTempNode == pPrevWriterNode)
		|| (pTempNode == hBuffer->pCurReaderBuf) || pTempNode->stFlags.bMute
		|| (pTempNode->ePixelFormat != hBuffer->hWindow->stCurInfo.ePixelFormat)
		|| (hBuffer->ulActiveBufCnt ==0)
	)
	{
		BDBG_MSG(("No user capture buffer available! Window %d ", hBuffer->hWindow->eId));
#if 0
		BDBG_ERR(( "current writer ID %d, previous writer ID %d, current reader ID %d, next writer ID %d ",
					hBuffer->pCurWriterBuf->ulBufferId, pPrevWriterNode->ulBufferId,
					hBuffer->pCurReaderBuf->ulBufferId, pTempNode->ulBufferId));

		BDBG_ERR(("   Dump out the whole buffer chain	 "	));
		while(pTempNode != hBuffer->pCurWriterBuf)
		{
			BDBG_ERR(("Buffer Id = %d ", pTempNode->ulBufferId));
			BVDC_P_Buffer_GetNextActiveAndNotUsedByUserNode(pNextTempNode, pTempNode);
			pTempNode = pNextTempNode;
		}

		BDBG_ASSERT(0);
#endif
		return BVDC_ERR_NO_AVAIL_CAPTURE_BUFFER;
	}


	/* Mark picture node as currently used by user */
	pTempNode->stFlags.bUsedByUser = true;
	pTempNode->stFlags.bActiveNode = false;

	/* need to be set after the gfx surface has been created */
	pTempNode->hSurface = NULL;

	*ppPicture = pTempNode;

	/* Decrement active buffer count. */
	hBuffer->ulActiveBufCnt--;

	/* Toggle the picture node destination polarity pointed
	 * to by the next writer and the nodes after it but before the reader. This
	 * is necessary to keep the field prediction correct.
	 */
	BVDC_P_Buffer_GetNextActiveAndNotUsedByUserNode(pTempNode, hBuffer->pCurWriterBuf);

	while (pTempNode != hBuffer->pCurReaderBuf)
	{
		pTempNode->eDstPolarity = BVDC_P_NEXT_POLARITY(pTempNode->eDstPolarity);
		BVDC_P_Buffer_GetNextActiveAndNotUsedByUserNode(pNextTempNode, pTempNode);
		pTempNode = pNextTempNode;
	} ;

	return BERR_SUCCESS;
}


/***************************************************************************
 * The main multi-buffering algorithm
 ***************************************************************************/

/***************************************************************************
 *
 */
static void BVDC_P_Buffer_UpdateWriterTimestamps_isr
	( BVDC_Window_Handle               hWindow,
	  const BAVC_Polarity              eFieldId )
{
	uint32_t ulTimestamp;
#if (BVDC_P_USE_RDC_TIMESTAMP)
	uint32_t           ulOtherSlotTimestamp;
	BRDC_Slot_Handle   hCaptureSlot, hOtherCaptureSlot;
#endif

#if (!BVDC_P_USE_RDC_TIMESTAMP)
	BSTD_UNUSED(eFieldId);
#endif

	BDBG_ENTER(BVDC_P_Buffer_UpdateWriterTimestamps_isr);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	BDBG_OBJECT_ASSERT(hWindow->hBuffer, BVDC_BUF);

#if (BVDC_P_USE_RDC_TIMESTAMP)
	if(eFieldId == BAVC_Polarity_eFrame)
	{
		if(BVDC_P_SRC_IS_MPEG(hWindow->stCurInfo.hSource->eId))
		{
			/* Progressive format use frame slot */
			hCaptureSlot = hWindow->stCurInfo.hSource->ahSlot[BAVC_Polarity_eFrame];
#if defined(HUMAX_PLATFORM_BASE)
			if (hCaptureSlot != NULL)
				ulTimestamp = BRDC_Slot_GetTimerSnapshot_isr(hCaptureSlot);
			else
				BDBG_ERR(("hCaptureSlot is NULL"));
#else
			ulTimestamp = BRDC_Slot_GetTimerSnapshot_isr(hCaptureSlot);
#endif
		}
		else
		{
			/* Non-Mpeg sources use 2 slots */
			/* Progressive format use top field slot */
			hCaptureSlot = hWindow->stCurInfo.hSource->ahSlot[BAVC_Polarity_eTopField];
#if defined(HUMAX_PLATFORM_BASE)
			if (hCaptureSlot != NULL)
				ulTimestamp = BRDC_Slot_GetTimerSnapshot_isr(hCaptureSlot);
			else
				BDBG_ERR(("hCaptureSlot is NULL"));
#else
			ulTimestamp = BRDC_Slot_GetTimerSnapshot_isr(hCaptureSlot);
#endif
		}
	}
	else
	{
		/* get time stamp from both slots, choose the later one */
		hCaptureSlot = hWindow->stCurInfo.hSource->ahSlot[eFieldId];
		hOtherCaptureSlot = hWindow->stCurInfo.hSource->ahSlot[BVDC_P_NEXT_POLARITY(eFieldId)];

#if defined(HUMAX_PLATFORM_BASE)
		if (hCaptureSlot != NULL)
			ulTimestamp = BRDC_Slot_GetTimerSnapshot_isr(hCaptureSlot);
		else
			BDBG_ERR(("hOtherCaptureSlot is NULL"));
#else
		ulTimestamp = BRDC_Slot_GetTimerSnapshot_isr(hCaptureSlot);
#endif
#if defined(HUMAX_PLATFORM_BASE)
		if (hOtherCaptureSlot != NULL)
			ulTimestamp = BRDC_Slot_GetTimerSnapshot_isr(hOtherCaptureSlot);
		else
			BDBG_ERR(("hOtherCaptureSlot is NULL"));
#else
		ulOtherSlotTimestamp = BRDC_Slot_GetTimerSnapshot_isr(hOtherCaptureSlot);
#endif
		if(ulOtherSlotTimestamp > ulTimestamp)
		{
			ulTimestamp = ulOtherSlotTimestamp;
		}
	}
#else

	ulTimestamp = BREG_Read32(hWindow->stCurResource.hCapture->hRegister, hWindow->stCurResource.hCapture->ulTimestampRegAddr);
	/* convert to microseconds based on a 27MHz clock since direct regsiter reads are tick values */
	ulTimestamp /= BVDC_P_CLOCK_RATE;
#endif


#if (BVDC_BUF_LOG == 1)
	BVDC_P_BufLogAddEvent_isr('Q',
                        hWindow->eId,
                        ulTimestamp,
                        0xAABB, /* Writer time stamp mark */
                        0,
                        hWindow->hBuffer);
#endif

	if (ulTimestamp != hWindow->hBuffer->ulCurrWriterTimestamp)
	{
		uint32_t ulTempCurWriterTs = hWindow->hBuffer->ulCurrWriterTimestamp;

		if (hWindow->hBuffer->bWriterWrapAround)
		{
			ulTempCurWriterTs -= hWindow->hBuffer->ulMaxTimestamp;
			hWindow->hBuffer->bWriterWrapAround = false;
		}

		hWindow->hBuffer->ulPrevWriterTimestamp = ulTempCurWriterTs;
		hWindow->hBuffer->ulCurrWriterTimestamp = ulTimestamp;
	}

	/* Apply correction to wraparound, if necssary. */
	if (hWindow->hBuffer->ulCurrWriterTimestamp < hWindow->hBuffer->ulPrevWriterTimestamp )
	{
		hWindow->hBuffer->ulCurrWriterTimestamp   += hWindow->hBuffer->ulMaxTimestamp;
		hWindow->hBuffer->bWriterWrapAround = true;
	}

	BDBG_LEAVE(BVDC_P_Buffer_UpdateWriterTimestamps_isr);
	return;
}

/***************************************************************************
 *
 */
static void BVDC_P_Buffer_UpdateReaderTimestamps_isr
	( BVDC_Window_Handle               hWindow,
	  const BAVC_Polarity              eFieldId )
{
	uint32_t ulTimestamp;
#if (BVDC_P_USE_RDC_TIMESTAMP)
	uint32_t           ulOtherSlotTimestamp;
	BRDC_Slot_Handle   hPlaybackSlot, hOtherPlaybackSlot;
#endif

#if (!BVDC_P_USE_RDC_TIMESTAMP)
	BSTD_UNUSED(eFieldId);
#endif

	BDBG_ENTER(BVDC_P_Buffer_UpdateReaderTimestamps_isr);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	BDBG_OBJECT_ASSERT(hWindow->hBuffer, BVDC_BUF);

#if (BVDC_P_USE_RDC_TIMESTAMP)
	/* display uses 2 slots */
	if(eFieldId == BAVC_Polarity_eFrame)
	{
		/* Progressive format use top field slot */
		hPlaybackSlot = hWindow->hCompositor->ahSlot[BAVC_Polarity_eTopField];
		ulTimestamp = BRDC_Slot_GetTimerSnapshot_isr(hPlaybackSlot);
	}
	else
	{
		/* get time stamp from both slots, choose the later one */
		hPlaybackSlot = hWindow->hCompositor->ahSlot[eFieldId];
		hOtherPlaybackSlot = hWindow->hCompositor->ahSlot[BVDC_P_NEXT_POLARITY(eFieldId)];

		ulTimestamp = BRDC_Slot_GetTimerSnapshot_isr(hPlaybackSlot);
		ulOtherSlotTimestamp = BRDC_Slot_GetTimerSnapshot_isr(hOtherPlaybackSlot);
		if(ulOtherSlotTimestamp > ulTimestamp)
		{
			ulTimestamp = ulOtherSlotTimestamp;
		}
	}
#else

	ulTimestamp = BREG_Read32(hWindow->stCurResource.hPlayback->hRegister,
		hWindow->stCurResource.hPlayback->ulTimestampRegAddr);

	/* convert to microseconds based on a 27MHz clock since direct regsiter reads are tick values */
	ulTimestamp /= BVDC_P_CLOCK_RATE;
#endif

#if (BVDC_BUF_LOG == 1)
		BVDC_P_BufLogAddEvent_isr('Q',
							hWindow->eId,
							0xBBAA, /* Reader time stamp mark */
							ulTimestamp,
							0,
							hWindow->hBuffer);
#endif

	if (ulTimestamp != hWindow->hBuffer->ulCurrReaderTimestamp)
	{
		uint32_t ulTempCurReaderTs = hWindow->hBuffer->ulCurrReaderTimestamp;

		if (hWindow->hBuffer->bReaderWrapAround)
		{
			ulTempCurReaderTs -= hWindow->hBuffer->ulMaxTimestamp;
			hWindow->hBuffer->bReaderWrapAround = false;
		}

		hWindow->hBuffer->ulPrevReaderTimestamp = ulTempCurReaderTs;
		hWindow->hBuffer->ulCurrReaderTimestamp = ulTimestamp;
	}

	/* Apply correction to wraparound, if necssary. */
	if (hWindow->hBuffer->ulCurrReaderTimestamp < hWindow->hBuffer->ulPrevReaderTimestamp)
	{
		hWindow->hBuffer->ulCurrReaderTimestamp   += hWindow->hBuffer->ulMaxTimestamp;
		hWindow->hBuffer->bReaderWrapAround = true;
	}

	BDBG_LEAVE(BVDC_P_Buffer_UpdateReaderTimestamps_isr);
	return;
}

/***************************************************************************
 * When a pic node is returned by BVDC_P_Buffer_GetNextWriterNode_isr, it is
 * marked as muted for safe. Window_Writer_isr will mark it as un-muted when
 * it is sure that the pic node is valid: the rect ptr is initialized, and
 * the video pixels are really captured.
 */
BVDC_P_PictureNode* BVDC_P_Buffer_GetNextWriterNode_isr
	( BVDC_Window_Handle               hWindow,
	  const BAVC_Polarity              eSrcPolarity )
{
	BVDC_P_PictureNode      *pNextNode;
	uint32_t                 ulTimeStamp;

	BDBG_ENTER(BVDC_P_Buffer_GetNextWriterNode_isr);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	BDBG_OBJECT_ASSERT(hWindow->hBuffer, BVDC_BUF);

	if(!hWindow->hBuffer->ulActiveBufCnt)
	{
		hWindow->hBuffer->pCurWriterBuf->stFlags.bMute = true;
		goto done;
	}

	hWindow->hBuffer->ulNumCapField++;

	/* ----------------------------------
	 * sync lock case
	 *-----------------------------------*/
	/* Move both reader and writer at same time if sync locked */
	if(hWindow->hBuffer->bSyncLock)
	{
		/* Don't need to check state */
		BVDC_P_Buffer_GetNextActiveAndNotUsedByUserNode(pNextNode, hWindow->hBuffer->pCurWriterBuf);
		pNextNode->stFlags.bMute  = true;
		hWindow->hBuffer->pCurWriterBuf = pNextNode;

		/* Move reader */
		/* if just added lipsync delay, repeat until the delay number is reached */
		if(hWindow->hBuffer->hWindow->bRepeatCurrReader)
		{
			uint32_t ulCurDelay = BVDC_P_Buffer_GetCurrentDelay(hWindow);
			if(ulCurDelay >= hWindow->hBuffer->ulVsyncDelay)
			{
				hWindow->hBuffer->hWindow->bRepeatCurrReader = false;
			}

			/* repeat if current delay less or equal to desired delay */
			if(ulCurDelay <= hWindow->hBuffer->ulVsyncDelay)
			{
				BDBG_MSG(("Win%d current buffer delay = %d, expect %d",
					hWindow->hBuffer->hWindow->eId, ulCurDelay, hWindow->hBuffer->ulVsyncDelay));
				goto done;
			}
		}
		BVDC_P_Buffer_GetNextActiveAndNotUsedByUserNode(pNextNode, hWindow->hBuffer->pCurReaderBuf);
		hWindow->hBuffer->pCurReaderBuf = pNextNode;
		goto done;
	}
	/* forced sync-lock double-buffer works under VEC locking scheme */
	else if(hWindow->stSettings.bForceSyncLock)
	{
		uint32_t ulTimestampDiff;

		/* Note the reduced memory mode (VEC locking) will use timestamp to avoid sticky tearing:
		   1) Every writer/reader isr will update its own ts;
		   2) if writer or reader isr finds that its updated ts is close to the counterpart's, then
		      it means its counterpart was just serviced before itself; in this case, if my next move
		      steps onto its counterpart, then next field/frame will get tearing, so pause my move
		      right here to prevent tearing;
		 */
		/* 1) update writer timestamp */
		BVDC_P_Buffer_UpdateWriterTimestamps_isr(hWindow, eSrcPolarity);

		/* 2) get the delta ts = |w_ts - r_ts|; */
		if(hWindow->hBuffer->ulCurrWriterTimestamp > hWindow->hBuffer->ulCurrReaderTimestamp)
		{
			ulTimestampDiff = hWindow->hBuffer->ulCurrWriterTimestamp - hWindow->hBuffer->ulCurrReaderTimestamp;
		}
		else
		{
			ulTimestampDiff = hWindow->hBuffer->ulCurrReaderTimestamp - hWindow->hBuffer->ulCurrWriterTimestamp;
		}

		BVDC_P_Buffer_GetNextActiveAndNotUsedByUserNode(pNextNode, hWindow->hBuffer->pCurWriterBuf);

		/* If a) delta Ts is small (reader isr was just served for the aligned vsync), and
		      b) writer will step on reader, and
		      c) src/display vsync rates are similar,
		   then don't move writer pointer since it could tear; */
		if( (ulTimestampDiff > BVDC_P_DOUBLE_BUFFER_TS_TOLERANCE) ||
			(hWindow->hBuffer->pCurReaderBuf != pNextNode) ||
			!BVDC_P_EQ_DELTA(hWindow->stCurInfo.hSource->ulVertFreq,
			hWindow->hCompositor->hDisplay->stCurInfo.ulVertFreq, 50))
		{
			pNextNode->stFlags.bMute  = true;
			hWindow->hBuffer->pCurWriterBuf = pNextNode;
		}
		else
		{
			BDBG_MSG((">>> Pause writer to avoid tearing!"));
		}
#ifdef	BVDC_DEBUG_FORCE_SYNC_LOCK
		BDBG_MSG(("Win%d W(%d), R(%d), p(%d), ts_w=%d",
			hWindow->eId, hWindow->hBuffer->pCurWriterBuf->ulBufferId,
			hWindow->hBuffer->pCurReaderBuf->ulBufferId,
			eSrcPolarity, ulTimestampDiff));
#endif
		goto done;
	}

	/* ----------------------------------
	 * sync slip case
	 *-----------------------------------*/
	/* if src has lost signal, writer vnet will shut down, we should avoid to force
	 * reader's pic node, otherwise reader might start to use a un-initialized pic
	 * node.
	 * if src is set as constant or repeat mode, also can't move reader's pic
	 * node. */
	if((hWindow->stCurInfo.hSource->bStartFeed) &&
	   (BVDC_MuteMode_eDisable == hWindow->stCurInfo.hSource->stCurInfo.eMuteMode))
	{
		BVDC_P_Buffer_CheckWriterIsrOrder_isr(hWindow,
			hWindow->stCurInfo.hSource->eNextFieldId,
			BVDC_P_NEXT_POLARITY(hWindow->hBuffer->pCurReaderBuf->eDisplayPolarity));
	}

	if (!hWindow->hBuffer->bWriterNodeMovedByReader)
		BVDC_P_Buffer_MoveSyncSlipWriterNode_isr(hWindow, eSrcPolarity); /* Update current writer node */
	else
		hWindow->hBuffer->bWriterNodeMovedByReader = false; /* clear the flag */

	/* Update picture timestamp */
	ulTimeStamp = hWindow->hBuffer->ulCurrWriterTimestamp;
	while( ulTimeStamp > hWindow->hBuffer->ulMaxTimestamp )
		ulTimeStamp -= hWindow->hBuffer->ulMaxTimestamp;
	hWindow->hBuffer->pCurWriterBuf->ulCaptureTimestamp = ulTimeStamp;

done:

#if (BVDC_BUF_LOG == 1)
	BVDC_P_BufLogAddEvent_isr('E',
		hWindow->eId,
		hWindow->hBuffer->pCurWriterBuf->ulBufferId,
		0,
		hWindow->hBuffer->ulCurrWriterTimestamp,
		hWindow->hBuffer);
#endif

	BDBG_LEAVE(BVDC_P_Buffer_GetNextWriterNode_isr);
	return (hWindow->hBuffer->pCurWriterBuf);
}


/***************************************************************************
 *
 */
static void BVDC_P_Buffer_UpdateTimestamps_isr
	( BVDC_Window_Handle               hWindow,
	  const BAVC_Polarity              eSrcPolarity,
	  const BAVC_Polarity              eDispPolarity )
{
	BDBG_ENTER(BVDC_P_Buffer_UpdateTimestamps_isr);

	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	BVDC_P_Buffer_UpdateReaderTimestamps_isr(hWindow, eDispPolarity);
	BVDC_P_Buffer_UpdateWriterTimestamps_isr(hWindow, eSrcPolarity);

	BDBG_LEAVE(BVDC_P_Buffer_UpdateTimestamps_isr);
	return;
}

/***************************************************************************
 *
 */
static void BVDC_P_Buffer_CheckWriterIsrOrder_isr
	( BVDC_Window_Handle               hWindow,
	  const BAVC_Polarity              eSrcPolarity,
	  const BAVC_Polarity              eDispPolarity )
{
	uint32_t ulTimestampDiff;

	BDBG_ENTER(BVDC_P_Buffer_CheckWriterIsrOrder_isr);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	BDBG_OBJECT_ASSERT(hWindow->hBuffer, BVDC_BUF);

	BVDC_P_Buffer_UpdateTimestamps_isr(hWindow, eSrcPolarity, eDispPolarity);

	if(hWindow->hBuffer->ulCurrWriterTimestamp > hWindow->hBuffer->ulCurrReaderTimestamp)
	{
		ulTimestampDiff = hWindow->hBuffer->ulCurrWriterTimestamp - hWindow->hBuffer->ulCurrReaderTimestamp;
	}
	else
	{
		ulTimestampDiff = hWindow->hBuffer->ulCurrReaderTimestamp - hWindow->hBuffer->ulCurrWriterTimestamp;
	}

	if (ulTimestampDiff > BVDC_P_MULTIBUFFER_RW_TOLERANCE)
	{
		/* Compare RUL timestamps */
		if (hWindow->hBuffer->ulCurrReaderTimestamp < hWindow->hBuffer->ulCurrWriterTimestamp) /* reader is ahead of writer */
		{
			/* Validate against ISR timestamps. */
			if (hWindow->stCurResource.hPlayback->ulTimestamp != hWindow->hBuffer->ulCurrReaderTimestamp)
			{
				if (!hWindow->hBuffer->bReaderNodeMovedByWriter)
				{
					BVDC_P_Buffer_MoveSyncSlipReaderNode_isr(hWindow, eDispPolarity);
					hWindow->hBuffer->bReaderNodeMovedByWriter = true;

#if (BVDC_BUF_LOG == 1)
					BVDC_P_BufLogAddEvent_isr('K',
										hWindow->eId,
										hWindow->hBuffer->ulNumCapField,
										hWindow->hBuffer->ulCurrWriterTimestamp,
										hWindow->hBuffer->ulCurrReaderTimestamp,
										hWindow->hBuffer);
#else

					BDBG_MSG(("(A) W: %d writer ISR moved reader due to misordered ISR",
						hWindow->hBuffer->ulNumCapField));
#endif
				}
				else
				{
					BDBG_MSG(("(A) Writer ISR can only move the reader once."));
				}
			}
		}
	}
	else
	{
		/* This only makes sense if there is no rate gap btw rd/wr */
		if ((hWindow->hBuffer->eLastBuffAction == BVDC_P_Last_Buffer_Action_Writer_Moved) &&
			(hWindow->hBuffer->eWriterVsReaderRateCode == hWindow->hBuffer->eReaderVsWriterRateCode))
		{
			BVDC_P_Buffer_MoveSyncSlipReaderNode_isr(hWindow, eDispPolarity);
			hWindow->hBuffer->bReaderNodeMovedByWriter = true;
#if (BVDC_BUF_LOG == 1)
			BVDC_P_BufLogAddEvent_isr('O',
								hWindow->eId,
								hWindow->hBuffer->ulNumCapField,
								hWindow->hBuffer->ulCurrWriterTimestamp,
								hWindow->hBuffer->ulCurrReaderTimestamp,
								hWindow->hBuffer);
#else

			BDBG_MSG(("(B) W: %d writer ISR moved reader due to misordered ISR",
				hWindow->hBuffer->ulNumCapField));
#endif
		}
		else
		{
			BDBG_MSG(("(B) Writer ISR can only move the reader once."));
		}
	}

	/* Update Capture ISR timestamp */
	if (hWindow->hBuffer->bWriterWrapAround)
	{
		hWindow->stCurResource.hCapture->ulTimestamp = hWindow->hBuffer->ulCurrWriterTimestamp - hWindow->hBuffer->ulMaxTimestamp;
	}
	else
	{
		hWindow->stCurResource.hCapture->ulTimestamp = hWindow->hBuffer->ulCurrWriterTimestamp;
	}

	BDBG_LEAVE(BVDC_P_Buffer_CheckWriterIsrOrder_isr);
	return;
}

static void BVDC_P_Buffer_MoveSyncSlipWriterNode_isr
	( BVDC_Window_Handle               hWindow,
	  const BAVC_Polarity              eSrcPolarity )
{
	BVDC_P_PictureNode         *pNextNode, *pNextNextNode, *pPrevNode;
	uint32_t                    ulSrcVertRate, ulDstVertRate;
	bool                        bSkip;
	uint32_t                    ulGap;
	uint32_t                    ulPrevBufCntNeeded;
	bool                        bProgressivePullDown = false;

	BDBG_ENTER(BVDC_P_Buffer_MoveSyncSlipWriterNode_isr);

	BSTD_UNUSED(eSrcPolarity);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	BDBG_OBJECT_ASSERT(hWindow->hBuffer, BVDC_BUF);

	if(hWindow->hBuffer->ulNumCapField < BVDC_P_BUFFER_NUM_FIELD_CAPTURE_B4_DISPLAY)
		goto done;

	/* keep track of the last buffer buffer action	*/
	hWindow->hBuffer->eLastBuffAction = BVDC_P_Last_Buffer_Action_Writer_Moved;

	/* Get next writer buffer */
	BVDC_P_Buffer_GetPrevActiveAndNotUsedByUserNode(pPrevNode, hWindow->hBuffer->pCurWriterBuf);
	BVDC_P_Buffer_GetNextActiveAndNotUsedByUserNode(pNextNode, hWindow->hBuffer->pCurWriterBuf);

	/* Get writer buffer after next. Needed for progressive mode. */
	BVDC_P_Buffer_GetNextActiveAndNotUsedByUserNode(pNextNextNode, pNextNode);

	/* Determine rate gap */
	BVDC_P_Buffer_CalculateRateGap(hWindow->stCurInfo.hSource->ulVertFreq,
		hWindow->hCompositor->stCurInfo.pFmtInfo->ulVertFreq,
		&hWindow->hBuffer->eWriterVsReaderRateCode, &hWindow->hBuffer->eReaderVsWriterRateCode);

	/* determine the timestamp sampling period for game mode */
	if((BVDC_P_ROUND_OFF(hWindow->stCurInfo.hSource->ulVertFreq,
		(BFMT_FREQ_FACTOR/2), BFMT_FREQ_FACTOR) << 1) ==
		BVDC_P_ROUND_OFF(hWindow->hCompositor->stCurInfo.pFmtInfo->ulVertFreq,
		(BFMT_FREQ_FACTOR/2), BFMT_FREQ_FACTOR))
	{
		/* 1) 1to2 src/disp rate ratio */
		hWindow->hBuffer->ulGameDelaySamplePeriod = 2;
	}
	else if((BVDC_P_ROUND_OFF(hWindow->stCurInfo.hSource->ulVertFreq,
		(BFMT_FREQ_FACTOR/2), BFMT_FREQ_FACTOR) * 5) ==
		BVDC_P_ROUND_OFF(hWindow->hCompositor->stCurInfo.pFmtInfo->ulVertFreq,
		(BFMT_FREQ_FACTOR/2), BFMT_FREQ_FACTOR) * 2)
	{
		/* 2) 2to5 src/disp rate ratio */
		hWindow->hBuffer->ulGameDelaySamplePeriod = 5;
	}
	else
	{
		/* 3) default */
		hWindow->hBuffer->ulGameDelaySamplePeriod = 1;
	}

	if (hWindow->stCurInfo.uiVsyncDelayOffset)
	{
		/* Calculate gap between reader and writer. This guarantees that the
		 * delay between the writer and the reader will not exceed the
		 * desired delay (vysnc delay + 1).
		 * Note, this decision is before advancing the write pointer! */
		ulGap = BVDC_P_Buffer_GetCurrentDelay(hWindow);
		bSkip = (ulGap > (hWindow->hBuffer->ulVsyncDelay)) ? true : false;
	}
	else
	{
		bSkip = false;
	}

	/* When displaying 1080p24/25/30 source as 1080p48/50/60, we cut the number
	 * of buffer to 3 to save memory. The algorithm allows writer to catch up
	 * reader and both of them point to the same buffer node.
	 *
	 * Note: This may cause video tearing if reader somehow misses interrupts.
	 */
	bProgressivePullDown =  VIDEO_FORMAT_IS_PROGRESSIVE(hWindow->stCurInfo.hSource->stCurInfo.pFmtInfo->eVideoFmt) &&
							(hWindow->hBuffer->eWriterVsReaderRateCode == BVDC_P_WrRate_NotFaster) &&
							(hWindow->hBuffer->eReaderVsWriterRateCode == BVDC_P_WrRate_2TimesFaster);

	/* Check if next node is reader OR if we are in full progressive mode. */
	if(((pNextNode == hWindow->hBuffer->pCurReaderBuf) && (!bProgressivePullDown)) ||
	   ((hWindow->hBuffer->eWriterVsReaderRateCode > BVDC_P_WrRate_NotFaster) && (pNextNextNode == hWindow->hBuffer->pCurReaderBuf)) ||
	   bSkip)
	{
		hWindow->hBuffer->ulSkipStat++;

		/* Skip one frame if capture as frame, or one field if capture as field but display is progressive and
		 * MAD is not at reader side. Otherwise, skip two fields.
		 * Note: If MAD is at reader side, skipping one field will break the MAD input TBTB cadence. This
		 * will result in MAD falling back to spatial-only mode.
		 */
		if(!((hWindow->bFrameCapture) ||
			 (VIDEO_FORMAT_IS_PROGRESSIVE(hWindow->hCompositor->stCurInfo.pFmtInfo->eVideoFmt) &&
			  (!BVDC_P_VNET_USED_MAD_AT_READER(hWindow->stVnetMode)))))
		{
			hWindow->hBuffer->pCurWriterBuf = pPrevNode;
#if (BVDC_BUF_LOG == 1)
			BVDC_P_BufLogAddEvent_isr('A',
								hWindow->eId,
								hWindow->hBuffer->pCurWriterBuf->ulBufferId,
								hWindow->hBuffer->ulSkipStat,
								hWindow->hBuffer->ulCurrWriterTimestamp,
								hWindow->hBuffer);
#else

			BDBG_MSG(("Win%d W: \tSkip 2 fields, num of cap fields %d, total %d",
				     hWindow->eId, hWindow->hBuffer->ulNumCapField, hWindow->hBuffer->ulSkipStat));
#endif
		}
		else
		{
#if (BVDC_BUF_LOG == 1)
			BVDC_P_BufLogAddEvent_isr('B',
								hWindow->eId,
								hWindow->hBuffer->pCurWriterBuf->ulBufferId,
								hWindow->hBuffer->ulSkipStat,
								hWindow->hBuffer->ulCurrWriterTimestamp,
								hWindow->hBuffer);
#else

			BDBG_MSG(("Win%d W: \tSkip 1 frame/field, num of cap frames/field %d, total %d",
				     hWindow->eId, hWindow->hBuffer->ulNumCapField, hWindow->hBuffer->ulSkipStat));
#endif
		}
	}
	else
	{
		hWindow->hBuffer->pCurWriterBuf = pNextNode;
	}

	hWindow->hBuffer->pCurWriterBuf->stFlags.bMute = true;

	ulPrevBufCntNeeded = hWindow->hBuffer->hWindow->ulBufCntNeeded;

	/* Check to see if buffer count was reduced due to progressive display format. If so and
	 * the reader or writer rate gaps is 1, increment the buffer cnt. */
	if(!hWindow->hCompositor->stCurInfo.pFmtInfo->bInterlaced || bProgressivePullDown)
	{
		if ((hWindow->hBuffer->eWriterVsReaderRateCode == hWindow->hBuffer->eReaderVsWriterRateCode) || bProgressivePullDown)
		{
			if (!hWindow->bBufferCntDecremented)
			{
				if (hWindow->bBufferCntIncremented)
				{
					/* From N+1 buffers to the N buffers first */
					hWindow->ulBufCntNeeded --;
					hWindow->ulBufDelay--;
					hWindow->hBuffer->ulVsyncDelay--;

					hWindow->bBufferCntIncremented = false;
				}

				/* From N buffers to N-1 buffers */
				hWindow->ulBufCntNeeded --;
				if (hWindow->hBuffer->eWriterVsReaderRateCode == hWindow->hBuffer->eReaderVsWriterRateCode)
				{
					hWindow->ulBufDelay--;
					hWindow->hBuffer->ulVsyncDelay--;
				}
				else /* Progressive pull down */
				{
					hWindow->bBufferCntDecrementedForPullDown = true;
				}

				hWindow->bBufferCntDecremented = true;
				BDBG_MSG(("Decrementing buffer count from %d to %d due progressive display format",
					ulPrevBufCntNeeded, hWindow->ulBufCntNeeded));
			}
		}
		else if (hWindow->bBufferCntDecremented)
		{
			hWindow->ulBufCntNeeded ++;

			if (!hWindow->bBufferCntDecrementedForPullDown)
			{
				hWindow->ulBufDelay++;
				hWindow->hBuffer->ulVsyncDelay++;
			}
			else
			{
				hWindow->bBufferCntDecrementedForPullDown = false;
			}
			hWindow->bBufferCntDecremented = false;
			BDBG_MSG(("Incrementing buffer count from %d to %d due to rate gap",
				ulPrevBufCntNeeded, hWindow->ulBufCntNeeded));
		}
	}

	/* If source/dest relationship requreis a writer gap, capture as interlaced and interlaced display,
	 * increment the number of buffers. */
	ulSrcVertRate = BVDC_P_ROUND_OFF(hWindow->stCurInfo.hSource->ulVertFreq,
		(BFMT_FREQ_FACTOR/2), BFMT_FREQ_FACTOR);
	ulDstVertRate = BVDC_P_ROUND_OFF(hWindow->hCompositor->stCurInfo.pFmtInfo->ulVertFreq,
		(BFMT_FREQ_FACTOR/2), BFMT_FREQ_FACTOR);
	if ((!hWindow->bFrameCapture) && (!VIDEO_FORMAT_IS_PROGRESSIVE(hWindow->hCompositor->stCurInfo.pFmtInfo->eVideoFmt))
		 && ((hWindow->hBuffer->eWriterVsReaderRateCode > BVDC_P_WrRate_Faster)))
	{
		if (!hWindow->bBufferCntIncremented)
		{
			if (hWindow->bBufferCntDecremented)
			{
				/* From N-1 buffers to N buffers first */
				hWindow->ulBufCntNeeded ++;
				if (!hWindow->bBufferCntDecrementedForPullDown)
				{
					hWindow->ulBufDelay++;
					hWindow->hBuffer->ulVsyncDelay++;
				}
				else
				{
					hWindow->bBufferCntDecrementedForPullDown = false;
				}
				hWindow->bBufferCntDecremented = false;
			}

			/* From N buffers to N+1 buffers */
			hWindow->ulBufCntNeeded++;
			hWindow->ulBufDelay++;
			hWindow->hBuffer->ulVsyncDelay++;
			hWindow->bBufferCntIncremented = true;
			BDBG_MSG(("Incrementing buffer count from %d to %d ",
					ulPrevBufCntNeeded, hWindow->ulBufCntNeeded));
		}
	}
	else
	{
		if (hWindow->bBufferCntIncremented)
		{
			hWindow->ulBufCntNeeded--;
			hWindow->ulBufDelay--;
			hWindow->hBuffer->ulVsyncDelay--;
			hWindow->bBufferCntIncremented = false;
			BDBG_MSG(("Decrementing buffer count from %d to %d ",
				ulPrevBufCntNeeded, hWindow->ulBufCntNeeded));
		}
	}

	/* set dirty bit */
	if (hWindow->ulBufCntNeeded != ulPrevBufCntNeeded)
	{
		hWindow->stCurInfo.stDirty.stBits.bReallocBuffers = BVDC_P_DIRTY;
	}

done:

	BDBG_LEAVE(BVDC_P_Buffer_MoveSyncSlipWriterNode_isr);

	return;
}


/***************************************************************************
 * {private}
 *
 */
BVDC_P_PictureNode* BVDC_P_Buffer_GetNextReaderNode_isr
	( BVDC_Window_Handle          hWindow,
	  const BAVC_Polarity         eVecPolarity )
{
	uint32_t                 ulTimeStamp;

	BDBG_ENTER(BVDC_P_Buffer_GetNextReaderNode_isr);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	BDBG_OBJECT_ASSERT(hWindow->hBuffer, BVDC_BUF);

	if(!hWindow->hBuffer->ulActiveBufCnt)
	{
		goto done;
	}

	/* ----------------------------------
	 * sync lock case
	 *-----------------------------------*/
	/* Move both reader and writer at same time if sync locked */
	if(hWindow->hBuffer->bSyncLock)
	{
		goto done;
	}
	/* Forced sync-lock under VEC locking scheme */
	else if(hWindow->stSettings.bForceSyncLock)
	{
		uint32_t ulTimestampDiff;
		BVDC_P_PictureNode         *pNextNode;
		uint32_t ulCurDelay;

		/* lipsync delay is enforced later */
		hWindow->bRepeatCurrReader = false;
		ulCurDelay = BVDC_P_Buffer_GetCurrentDelay(hWindow);

		/* Note the reduced memory mode (VEC locking) will use timestamp to avoid sticky tearing:
		   1) Every writer/reader isr will update its own ts;
		   2) if writer or reader isr finds that its updated ts is close to the counterpart's, then
		      it means its counterpart was just serviced before itself; in this case, if my next move
		      steps onto its counterpart, then next field/frame will get tearing, so pause my move
		      right here to prevent tearing;
		 */
		/* 1) update reader timestamp */
		BVDC_P_Buffer_UpdateReaderTimestamps_isr(hWindow, eVecPolarity);

		/* 2) get the deltaTs between w_ts and r_ts; */
		if(hWindow->hBuffer->ulCurrWriterTimestamp > hWindow->hBuffer->ulCurrReaderTimestamp)
		{
			ulTimestampDiff = hWindow->hBuffer->ulCurrWriterTimestamp - hWindow->hBuffer->ulCurrReaderTimestamp;
		}
		else
		{
			ulTimestampDiff = hWindow->hBuffer->ulCurrReaderTimestamp - hWindow->hBuffer->ulCurrWriterTimestamp;
		}

		/* get next reader */
		BVDC_P_Buffer_GetNextActiveAndNotUsedByUserNode(pNextNode, hWindow->hBuffer->pCurReaderBuf);

		/* If a) delta Ts is small (writer isr was just served for the aligned vsync), and
		      b) reader will step on writer, and
		      c) src/display vsync rates are similar,
		   then don't move reader pointer since it could tear; */
		if( (ulTimestampDiff > BVDC_P_DOUBLE_BUFFER_TS_TOLERANCE) ||
			(hWindow->hBuffer->pCurWriterBuf != pNextNode) ||
			!BVDC_P_EQ_DELTA(hWindow->stCurInfo.hSource->ulVertFreq,
			hWindow->hCompositor->hDisplay->stCurInfo.ulVertFreq, 50))
		{
			if((/* 1->1 sync-slipped */
				BVDC_P_EQ_DELTA(hWindow->stCurInfo.hSource->ulVertFreq, hWindow->hCompositor->hDisplay->stCurInfo.ulVertFreq, 50)
			    ) ||
			   (/* 24->60; 60Hz sync-slipped master */
			   (hWindow->hBuffer->pCurWriterBuf == hWindow->hBuffer->pCurReaderBuf) &&
			   (2397 == hWindow->stCurInfo.hSource->ulVertFreq) &&
			   (5994 == hWindow->hCompositor->hDisplay->stCurInfo.ulVertFreq) &&
			   (((ulTimestampDiff >= BVDC_P_USEC_ONE_VSYNC_INTERVAL(hWindow->hCompositor->hDisplay->stCurInfo.ulVertFreq) -
				  BVDC_P_DOUBLE_BUFFER_TS_TOLERANCE) &&
				 (ulTimestampDiff <= BVDC_P_USEC_ONE_VSYNC_INTERVAL(hWindow->hCompositor->hDisplay->stCurInfo.ulVertFreq) +
				  BVDC_P_DOUBLE_BUFFER_TS_TOLERANCE)) ||
			    ((ulTimestampDiff >= 3 * BVDC_P_USEC_ONE_VSYNC_INTERVAL(hWindow->hCompositor->hDisplay->stCurInfo.ulVertFreq) / 2 -
				  BVDC_P_DOUBLE_BUFFER_TS_TOLERANCE) &&
				 (ulTimestampDiff <= 3 * BVDC_P_USEC_ONE_VSYNC_INTERVAL(hWindow->hCompositor->hDisplay->stCurInfo.ulVertFreq) / 2 +
				  BVDC_P_DOUBLE_BUFFER_TS_TOLERANCE)))) ||
			   (/* 1->2 (25/30->50/60); SD sync-slipped master */
			   (hWindow->hBuffer->pCurWriterBuf == hWindow->hBuffer->pCurReaderBuf) &&
			   (2*hWindow->stCurInfo.hSource->ulVertFreq == hWindow->hCompositor->hDisplay->stCurInfo.ulVertFreq) &&
			   ((ulTimestampDiff >= BVDC_P_USEC_ONE_VSYNC_INTERVAL(hWindow->hCompositor->hDisplay->stCurInfo.ulVertFreq) -
				 BVDC_P_DOUBLE_BUFFER_TS_TOLERANCE) &&
				(ulTimestampDiff <= BVDC_P_USEC_ONE_VSYNC_INTERVAL(hWindow->hCompositor->hDisplay->stCurInfo.ulVertFreq) +
				 BVDC_P_DOUBLE_BUFFER_TS_TOLERANCE))))
			{
				/* make sure lipsync delay enforced in case of missed isr */
				if(((ulTimestampDiff > BVDC_P_DOUBLE_BUFFER_TS_TOLERANCE) &&
				    (ulCurDelay >= hWindow->hBuffer->ulVsyncDelay)) ||
				   ((ulTimestampDiff <= BVDC_P_DOUBLE_BUFFER_TS_TOLERANCE) &&
				   (ulCurDelay > hWindow->hBuffer->ulVsyncDelay || ulCurDelay==0)))
				{
					hWindow->hBuffer->pCurReaderBuf = pNextNode;
				}
				else
				{
					BDBG_MSG(("== Pause reader to keep desired lipsync delay! curDly=%d, tgtDly=%d",
						ulCurDelay, hWindow->hBuffer->ulVsyncDelay));
				}
			}
		}
		else
		{
			BDBG_MSG(("|| Pause reader to avoid tearing!"));
		}
#ifdef	BVDC_DEBUG_FORCE_SYNC_LOCK
		BDBG_MSG(("Win%d \tR(%d), W(%d), s(%d)->d(%d)->v(%d), TS [%d, %d] [%d, %d], ts_r=%d, srcFreq=%d, dispFreq=%d",
			hWindow->eId, hWindow->hBuffer->pCurReaderBuf->ulBufferId, hWindow->hBuffer->pCurWriterBuf->ulBufferId,
			hWindow->hBuffer->pCurReaderBuf->eSrcPolarity, hWindow->hBuffer->pCurReaderBuf->eDstPolarity, eVecPolarity,
			hWindow->hCompositor->hDisplay->ulTsSampleCount, hWindow->hCompositor->hDisplay->ulTsSamplePeriod,
			hWindow->hCompositor->hDisplay->ulCurrentTs,
			hWindow->stCurInfo.hSource->hSyncLockCompositor->hDisplay->ulCurrentTs, ulTimestampDiff,
			hWindow->stCurInfo.hSource->ulVertFreq, hWindow->hCompositor->hDisplay->stCurInfo.ulVertFreq));
#endif
		goto done;
	}

	/* ----------------------------------
	 * starts sync slip case
	 *-----------------------------------*/
	if(hWindow->stCurInfo.hSource->bStartFeed)
	{
		/* Check if reader ISR is executing out of order */
		BVDC_P_Buffer_CheckReaderIsrOrder_isr(hWindow,
			hWindow->stCurInfo.hSource->eNextFieldId,
			BVDC_P_NEXT_POLARITY(hWindow->hBuffer->pCurReaderBuf->eDisplayPolarity));
	}

	if (!hWindow->hBuffer->bReaderNodeMovedByWriter)
		BVDC_P_Buffer_MoveSyncSlipReaderNode_isr(hWindow, eVecPolarity); /* Update current writer node */
	else
		hWindow->hBuffer->bReaderNodeMovedByWriter = false; /* clear the flag */

	/* Update picture timestamp */
	ulTimeStamp = hWindow->hBuffer->ulCurrReaderTimestamp;
	while( ulTimeStamp > hWindow->hBuffer->ulMaxTimestamp )
		ulTimeStamp -= hWindow->hBuffer->ulMaxTimestamp;
	hWindow->hBuffer->pCurReaderBuf->ulPlaybackTimestamp = ulTimeStamp;

done:

#if (BVDC_BUF_LOG == 1)
	BVDC_P_BufLogAddEvent_isr('F',
						hWindow->eId,
						hWindow->hBuffer->pCurReaderBuf->ulBufferId,
						0,
						hWindow->hBuffer->ulCurrReaderTimestamp,
						hWindow->hBuffer);
#endif

	/* NOTE: The reader and writer should not be pointing to the same node at this point;
	 * otherwise, there will be tearing or unexpected video glitches. The only situation
	 * where R == W is during the execution of 1/2 field delay game mode.
	 */

	BDBG_LEAVE(BVDC_P_Buffer_GetNextReaderNode_isr);
	return (hWindow->hBuffer->pCurReaderBuf);
}

/***************************************************************************
 *
 */
static void BVDC_P_Buffer_CheckReaderIsrOrder_isr
	( BVDC_Window_Handle               hWindow,
	  const BAVC_Polarity              eSrcPolarity,
	  const BAVC_Polarity              eDispPolarity )
{
	uint32_t ulTimestampDiff;

	BDBG_ENTER(BVDC_P_Buffer_CheckReaderIsrOrder_isr);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	BDBG_OBJECT_ASSERT(hWindow->hBuffer, BVDC_BUF);

	BVDC_P_Buffer_UpdateTimestamps_isr(hWindow, eSrcPolarity, eDispPolarity);

	if(hWindow->hBuffer->ulCurrWriterTimestamp > hWindow->hBuffer->ulCurrReaderTimestamp)
	{
		ulTimestampDiff = hWindow->hBuffer->ulCurrWriterTimestamp - hWindow->hBuffer->ulCurrReaderTimestamp;
	}
	else
	{
		ulTimestampDiff = hWindow->hBuffer->ulCurrReaderTimestamp - hWindow->hBuffer->ulCurrWriterTimestamp;
	}

	if (ulTimestampDiff > BVDC_P_MULTIBUFFER_RW_TOLERANCE)
	{
		if (hWindow->hBuffer->ulCurrReaderTimestamp > hWindow->hBuffer->ulCurrWriterTimestamp) /* writer is ahead of reader */
		{
			/* Check if reader ISR is out of order. */
			if (hWindow->stCurResource.hCapture->ulTimestamp != hWindow->hBuffer->ulCurrWriterTimestamp)
			{
				if (!hWindow->hBuffer->bWriterNodeMovedByReader)
				{
					BVDC_P_Buffer_MoveSyncSlipWriterNode_isr(hWindow, eSrcPolarity);
					hWindow->hBuffer->bWriterNodeMovedByReader = true;

#if (BVDC_BUF_LOG == 1)
					BVDC_P_BufLogAddEvent_isr('L',
										hWindow->eId,
										hWindow->hBuffer->ulNumCapField,
										hWindow->hBuffer->ulCurrReaderTimestamp,
										hWindow->hBuffer->ulCurrWriterTimestamp,
										hWindow->hBuffer);
#else

					BDBG_MSG(("(A) R: %d reader ISR moved writer due to ISR misorder", hWindow->hBuffer->ulNumCapField));
#endif
				}
				else
				{
					BDBG_MSG(("(A) Reader ISR can only move the writer once."));
				}
			}
		}
	}
	else
	{
		/* This only makes sense if there is no rate gap btw rd/wr */
		if ((hWindow->hBuffer->eLastBuffAction == BVDC_P_Last_Buffer_Action_Reader_Moved) &&
			(hWindow->hBuffer->eWriterVsReaderRateCode == hWindow->hBuffer->eReaderVsWriterRateCode))
		{
			BVDC_P_Buffer_MoveSyncSlipWriterNode_isr(hWindow, eSrcPolarity);
			hWindow->hBuffer->bWriterNodeMovedByReader = true;
#if (BVDC_BUF_LOG == 1)
			BVDC_P_BufLogAddEvent_isr('P',
								hWindow->eId,
								hWindow->hBuffer->ulNumCapField,
								hWindow->hBuffer->ulCurrReaderTimestamp,
								hWindow->hBuffer->ulCurrWriterTimestamp,
								hWindow->hBuffer);
#else

			BDBG_MSG(("(B) R: %d reader ISR moved writer due to ISR misorder", hWindow->hBuffer->ulNumCapField));
#endif
		}
		else
		{
			BDBG_MSG(("(B) Reader ISR can only move the writer once."));
		}
	}

	/* Update Feeder ISR timestamp */
	if (hWindow->hBuffer->bReaderWrapAround)
	{
		hWindow->stCurResource.hPlayback->ulTimestamp = hWindow->hBuffer->ulCurrReaderTimestamp - hWindow->hBuffer->ulMaxTimestamp;
	}
	else
	{
		hWindow->stCurResource.hPlayback->ulTimestamp = hWindow->hBuffer->ulCurrReaderTimestamp;
	}

	BDBG_LEAVE(BVDC_P_Buffer_CheckReaderIsrOrder_isr);
	return;
}


static void BVDC_P_Buffer_MoveSyncSlipReaderNode_isr
	( BVDC_Window_Handle               hWindow,
	  const BAVC_Polarity              eVecPolarity )
{
	BVDC_P_PictureNode         *pNextNode, *pNextNextNode, *pPrevNode, *pCurNode;
	bool                        bRepeat;
	uint32_t                    ulGap;

	BDBG_ENTER(BVDC_P_Buffer_MoveSyncSlipReaderNode_isr);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	BDBG_OBJECT_ASSERT(hWindow->hBuffer, BVDC_BUF);

	if(hWindow->hBuffer->ulNumCapField < BVDC_P_BUFFER_NUM_FIELD_CAPTURE_B4_DISPLAY)
		goto done;

	/* keep track of the last buffer buffer action	*/
	hWindow->hBuffer->eLastBuffAction = BVDC_P_Last_Buffer_Action_Reader_Moved;

	/* ----------------------------------
	 * if lipsync delay was adjusted and the expected delay has not been reached;
	 *-----------------------------------*/
	if(hWindow->hBuffer->hWindow->bRepeatCurrReader)
	{
		uint32_t ulCurDelay = BVDC_P_Buffer_GetCurrentDelay(hWindow);
		if(ulCurDelay >= hWindow->hBuffer->ulVsyncDelay)
		{
			hWindow->hBuffer->hWindow->bRepeatCurrReader = false;
		}

		/* repeat if current delay less or equal to desired */
		if(ulCurDelay <= hWindow->hBuffer->ulVsyncDelay)
		{
			BDBG_MSG(("Win%d current buffer delay = %d, expect %d",
				hWindow->hBuffer->hWindow->eId, ulCurDelay, hWindow->hBuffer->ulVsyncDelay));
			goto done;
		}
	}

	/* Get next reader buffer */
	BVDC_P_Buffer_GetPrevActiveAndNotUsedByUserNode(pPrevNode, hWindow->hBuffer->pCurReaderBuf);
	BVDC_P_Buffer_GetNextActiveAndNotUsedByUserNode(pNextNode, hWindow->hBuffer->pCurReaderBuf);

	/* Get reader buffer after next */
	BVDC_P_Buffer_GetNextActiveAndNotUsedByUserNode(pNextNextNode, pNextNode);

	if (hWindow->stCurInfo.uiVsyncDelayOffset)
	{
		/* Calculate gap between reader and writer. This guarantees that the
		 * delay between the writer and the reader will not be less than the
		 * desired delay (vysnc delay - 1).
		 * Note, this decision is before advancing the read pointer! */
		ulGap = BVDC_P_Buffer_GetCurrentDelay(hWindow);
		bRepeat = (ulGap < (hWindow->hBuffer->ulVsyncDelay + (VIDEO_FORMAT_IS_PROGRESSIVE(hWindow->hCompositor->stCurInfo.pFmtInfo->eVideoFmt) ? 1: 0))) ?
					true : false;
	}
	else
	{
		bRepeat = false;
	}

	/* Check if we are encroaching the writer */
	if(((pNextNode == hWindow->hBuffer->pCurWriterBuf) &&
		!hWindow->stCurInfo.stGameDelaySetting.bEnable) ||
	   ((hWindow->hBuffer->eReaderVsWriterRateCode > BVDC_P_WrRate_NotFaster) && (pNextNextNode == hWindow->hBuffer->pCurWriterBuf)) ||
	     bRepeat)
	{
		/* Repeat reader. We may have to bear a field inversion here if both
		 * source and display are interlaced and pictures are not captured as
		 * frame.
		 */
#if BVDC_P_REPEAT_ALGORITHM_ONE
			hWindow->hBuffer->bRepeatForGap = true;
#endif
			hWindow->hBuffer->ulRepeatStat++;
			hWindow->hBuffer->pCurReaderBuf->stFlags.bPictureRepeatFlag = true;
#if (BVDC_BUF_LOG == 1)
			BVDC_P_BufLogAddEvent_isr('C',
								hWindow->eId,
								hWindow->hBuffer->pCurReaderBuf->ulBufferId,
								hWindow->hBuffer->ulRepeatStat,
								hWindow->hBuffer->ulCurrReaderTimestamp,
								hWindow->hBuffer);
#else
			BDBG_MSG(("Win%d R: \t Repeat one field for r-w gap, total %d",
				    hWindow->hBuffer->hWindow->eId, hWindow->hBuffer->ulRepeatStat));
#endif
	}
	else
	{
		/* Advance the reader anyway first */
		pCurNode = hWindow->hBuffer->pCurReaderBuf;
		hWindow->hBuffer->pCurReaderBuf = pNextNode;

		/* Now we need to decide if reader's polarity satisfies VEC.
		 */
		if (!hWindow->hBuffer->pCurReaderBuf->stFlags.bMute && hWindow->hBuffer->pCurReaderBuf->stFlags.bCadMatching && (hWindow->hBuffer->pCurReaderBuf->eDstPolarity != eVecPolarity))
		{
#if BVDC_P_REPEAT_ALGORITHM_ONE
			/* If polarity mismatch is caused by a reader repeat to avoid catching up writer, then
			 * move reader to next field anyway to spread 2 repeats over 4 fields.
			 * Bear a field inversion. Reader bvdc_buffer_priv.h for details.
			 */
			if((!hWindow->hBuffer->bRepeatForGap) ||
			   ((pNextNode == hWindow->hBuffer->pCurWriterBuf) && (!hWindow->stCurInfo.stGameDelaySetting.bEnable)) ||
			   ((hWindow->hBuffer->eReaderVsWriterRateCode > BVDC_P_WrRate_NotFaster) && (pNextNextNode == hWindow->hBuffer->pCurWriterBuf)))
#endif
			{
				hWindow->hBuffer->ulRepeatStat++;

#if (BVDC_BUF_LOG == 1)
				BVDC_P_BufLogAddEvent_isr('D',
									hWindow->eId,
									hWindow->hBuffer->pCurReaderBuf->ulBufferId,
									(eVecPolarity << 16) | (hWindow->hBuffer->pCurReaderBuf->PicComRulInfo.eSrcOrigPolarity << 8) | hWindow->hBuffer->pCurReaderBuf->eDstPolarity,
									hWindow->hBuffer->ulCurrReaderTimestamp,
									hWindow->hBuffer);
#else
				BDBG_MSG(("Win%d R: \t Repeat for polarity, src I, VEC %d. orig src p %d, cap %d, total %d",
						hWindow->hBuffer->hWindow->eId, eVecPolarity, hWindow->hBuffer->pCurReaderBuf->PicComRulInfo.eSrcOrigPolarity,
						hWindow->hBuffer->pCurReaderBuf->eDstPolarity, hWindow->hBuffer->ulRepeatStat));
#endif

				/* Repeat one field so that next field will match VEC polarity.
				 * We bear a field inversion here.
				 */
				/* restore the current reader buffer */
				hWindow->hBuffer->pCurReaderBuf = pCurNode;
				hWindow->hBuffer->pCurReaderBuf->stFlags.bPictureRepeatFlag = true;
			}
		}

#if BVDC_P_REPEAT_ALGORITHM_ONE
		hWindow->hBuffer->bRepeatForGap = false;
#endif
	}

done:
	BDBG_LEAVE(BVDC_P_Buffer_MoveSyncSlipReaderNode_isr);
	return ;
}


/***************************************************************************
 *
 */
void BVDC_P_Buffer_CalculateRateGap
	( const uint32_t         ulSrcVertFreq,
	  const uint32_t         ulDispVertFreq,
	  BVDC_P_WrRateCode     *peWriterVsReaderRateCode,
	  BVDC_P_WrRateCode     *peReaderVsWriterRateCode )
{
	uint32_t	ulDstVertRate, ulSrcVertRate;

	/* 29.97Hz and 30Hz, 59.94Hz and 60Hz are considered
	 * the same rate after round off. */
	ulDstVertRate = BVDC_P_ROUND_OFF(ulDispVertFreq,
		(BFMT_FREQ_FACTOR/2), BFMT_FREQ_FACTOR);
	ulSrcVertRate = BVDC_P_ROUND_OFF(ulSrcVertFreq,
		(BFMT_FREQ_FACTOR/2), BFMT_FREQ_FACTOR);

	/* Preset writer and reader rate gap */
	*peWriterVsReaderRateCode = BVDC_P_WrRate_NotFaster;
	*peReaderVsWriterRateCode = BVDC_P_WrRate_Faster;

	if(ulDstVertRate == ulSrcVertRate)
	{
		*peWriterVsReaderRateCode = BVDC_P_WrRate_NotFaster;
		*peReaderVsWriterRateCode = BVDC_P_WrRate_NotFaster;
	}
	else
	{
		if(ulSrcVertRate > ulDstVertRate)
		{
			*peReaderVsWriterRateCode = BVDC_P_WrRate_NotFaster;
			*peWriterVsReaderRateCode = ((ulSrcVertRate / ulDstVertRate) >= 2)
				? BVDC_P_WrRate_2TimesFaster : BVDC_P_WrRate_Faster;
		}
		else
		{
			*peWriterVsReaderRateCode = BVDC_P_WrRate_NotFaster;
			*peReaderVsWriterRateCode = ((ulDstVertRate / ulSrcVertRate) >= 2)
				? BVDC_P_WrRate_2TimesFaster : BVDC_P_WrRate_Faster;
		}
	}

	return;
}

/***************************************************************************
 *
 */
uint32_t BVDC_P_Buffer_CalculateBufDelay
	( BVDC_P_PictureNode   *pPicture )
{
	return ((pPicture->ulPlaybackTimestamp < pPicture->ulCaptureTimestamp) ?
		    (pPicture->ulPlaybackTimestamp + pPicture->hBuffer->ulMaxTimestamp - pPicture->ulCaptureTimestamp)
		    : (pPicture->ulPlaybackTimestamp - pPicture->ulCaptureTimestamp));
}

/***************************************************************************
 *
 */
BVDC_P_PictureNode * BVDC_P_Buffer_GetCurWriterNode
	( const BVDC_P_Buffer_Handle       hBuffer )
{
	BDBG_OBJECT_ASSERT(hBuffer, BVDC_BUF);
	return (hBuffer->pCurWriterBuf);
}

/***************************************************************************
 *
 */
BVDC_P_PictureNode * BVDC_P_Buffer_GetCurReaderNode
	( const BVDC_P_Buffer_Handle       hBuffer )
{
	BDBG_OBJECT_ASSERT(hBuffer, BVDC_BUF);
	return (hBuffer->pCurReaderBuf);
}

/* End of file. */
