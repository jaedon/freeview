/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvdc_buffer_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/190 $
 * $brcm_Date: 11/15/12 11:48a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vdc/src/common/bvdc_buffer_priv.h $
 * 
 * Hydra_Software_Devel/190   11/15/12 11:48a syang
 * SW7231-1049, SW7425-1835: turn off  BVDC_P_USE_RDC_TIMESTAMP due to the
 * tearing issue
 * 
 * Hydra_Software_Devel/189   9/20/12 4:56p yuxiaz
 * SW7425-1835, SW7425-3928: Reenable BVDC_P_USE_RDC_TIMESTAMP. Fixed
 * drift delay using RDC timestamp.
 * 
 * Hydra_Software_Devel/188   9/17/12 5:01p yuxiaz
 * SW7425-3928: Turn off BVDC_P_USE_RDC_TIMESTAMP. Need to review multi-
 * buffering algo using timestamp from RDC slot.
 * 
 * Hydra_Software_Devel/187   7/31/12 4:55p yuxiaz
 * SW7425-1835: Use RDC slot timestamp for capture and playback timestamp
 * in multi-buffering algo.
 * 
 * Hydra_Software_Devel/186   6/29/12 3:28p vanessah
 * SW7425-3359: FNRT flags support to ViCE MBox
 * 
 * Hydra_Software_Devel/185   5/1/12 3:04p vanessah
 * SW7425-2643: add bar data support
 * 
 * Hydra_Software_Devel/184   2/6/12 9:35p vanessah
 * SW7425-2296: use scaler to do Orientation conversion
 * 
 * Hydra_Software_Devel/183   1/30/12 4:04p vanessah
 * SW7425-2239: add  bChannelChange in VCE Mbox
 * 
 * Hydra_Software_Devel/182   1/16/12 11:09a vanessah
 * SW7425-2084: add bLast flage to VCD Mbox
 * 
 * Hydra_Software_Devel/181   12/13/11 4:58p yuxiaz
 * SW7425-1875: Added support for source buffer selection in 3d mode.
 * 
 * Hydra_Software_Devel/180   12/1/11 4:31p yuxiaz
 * SW7425-968, SW7344-95: Merged into mainline.: added independent source
 * clipping of right window in 3D mode.
 * 
 * Hydra_Software_Devel/179   11/29/11 11:16a vanessah
 * SW7425-1783: add user data support in VDC PI
 * 
 * Hydra_Software_Devel/178   10/27/11 4:30p yuxiaz
 * SW7405-5520: SD display will show wrong full screen flash when toggling
 * size and visibility with simultaneous HD display.
 * 
 * Hydra_Software_Devel/177   8/31/11 11:17a syang
 * SW7425-1170: pxlAspRatio passed to ViCE2 is corrected to x<<16 | y
 * format
 * 
 * Hydra_Software_Devel/176   8/26/11 5:37p syang
 * SW7425-1170: merge from branch SW7572-1170
 * 
 * Hydra_Software_Devel/SW7425-1170/3   8/26/11 4:20p syang
 * SW7425-1170: refactor pixel aspect ratio related code
 * 
 * Hydra_Software_Devel/SW7425-1170/2   8/24/11 6:55p vanessah
 * SW7425-1170: ASP code clean
 * 
 * Hydra_Software_Devel/SW7425-1170/1   8/23/11 6:13p vanessah
 * SW7425-1170: Aspect ratio cleanup
 * 
 * Hydra_Software_Devel/175   7/18/11 11:19a vanessah
 * SW7425-835: SW7425-923: fix ViCE2 channel id bug + B0 STG
 * 
 * Hydra_Software_Devel/174   6/20/11 5:49p tdo
 * SW7425-104: Rename the old repeat field flag to match avc structure and
 * use the new added bRepeatField flag for ANR.
 *
 * Hydra_Software_Devel/173   6/10/11 5:49p hongtaoz
 * SW7425-704: add NRT mode support to VDC; moved the STG meta data
 * passing from window writer isr to reader isr; added bStallStc flag
 * support;
 *
 * Hydra_Software_Devel/172   5/27/11 3:37p yuxiaz
 * SW7425-415: Clean up buffer allocation and orientation settings for 3d
 * video support.
 *
 * Hydra_Software_Devel/171   4/8/11 1:56p yuxiaz
 * SW7425-236: Fixed flashing when source orientation changes.
 *
 * Hydra_Software_Devel/170   3/15/11 4:22p yuxiaz
 * SW7400-2882: Added support to enable / disable multi-buffering logging
 * for a specific window.
 *
 * Hydra_Software_Devel/169   2/9/11 3:59p yuxiaz
 * SW7400-2882: Ported multi-buffing event logging scheme to VDC. Move
 * related APIs to bvdc_dbg.h
 *
 * Hydra_Software_Devel/168   2/7/11 6:10p vanessah
 * SW7425-46: Mbox meta data programming
 *
 * Hydra_Software_Devel/167   2/7/11 5:24p vanessah
 * SW7425-46: add MFD mute to the skip picture to start quality comparison
 *
 * Hydra_Software_Devel/166   12/8/10 5:44p tdo
 * SW7422-36: Correctly program MCVP 3D mode based on source and display
 * orientation
 *
 * Hydra_Software_Devel/165   10/29/10 3:01p tdo
 * SW7422-40: Update GetBuffer() capability in VDC with 3D case
 *
 * Hydra_Software_Devel/164   10/22/10 7:01p vanessah
 * SW7425-46:  ViCE2 meta data mailbox register
 *
 * Hydra_Software_Devel/163   9/29/10 5:21p yuxiaz
 * SW7422-39: Added 3D video support in CAP and VFD.
 *
 * Hydra_Software_Devel/162   9/15/10 11:22a yuxiaz
 * SW7422-30: Handle forced capture modes and orientation programming in
 * MFD.
 *
 * Hydra_Software_Devel/161   8/6/10 11:41a rpan
 * SW7400-2882: Integrated the multi-buffering event logging scheme.
 *
 * Hydra_Software_Devel/160   8/3/10 5:15p rpan
 * SW7420-895: Back out the "Report buffer delay based on writer and
 * reader node gap" change.
 *
 * Hydra_Software_Devel/159   8/2/10 11:15a rpan
 * SW7420-895: Report buffer delay based on writer and reader node gap.
 *
 * Hydra_Software_Devel/158   5/7/10 7:08p albertl
 * SW7125-364: Changed dirty bits to use union structure to avoid type-pun
 * warnings.
 *
 * Hydra_Software_Devel/157   4/15/10 4:53p hongtaoz
 * SW3548-2828: changed the normalized fixed point match precision to
 * accomandate 12-bit picture size for 2160i;
 *
 * Hydra_Software_Devel/156   4/7/10 11:25a tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic.  Rename TLA
 *
 * Hydra_Software_Devel/155   4/5/10 3:58p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic
 *
 * Hydra_Software_Devel/154   2/1/10 6:35p hongtaoz
 * SW3548-2678, SW3548-2680, SW3548-2681: added HDMI1.4 3D formats support
 * for 3548; added multi-buffer low delay mode support for 1to2 and 2to5
 * rate gap cases; reduced capture buffer allocation for interlaced
 * pulldown case;
 *
 * Hydra_Software_Devel/BCM3D/1   12/17/09 6:08p hongtaoz
 * SW3548-2678: added game mode delay support for 1to2 and 2to5 src/disp
 * rate ratioes;
 *
 * Hydra_Software_Devel/153   1/28/10 11:10a syang
 * SW7405-3829: add option for app to shrink src width to use deinterlace
 *
 * Hydra_Software_Devel/152   10/9/09 5:28p syang
 * SW7405-2516, SW3548-2439, SW7405-3046: rewrite HSCL setinfo_isr and
 * TryXferHrzSclToHscl_isr to do crop correctly; cleanup width alignment
 * for DCX_HSIZE_WORKAROUND
 *
 * Hydra_Software_Devel/151   6/2/09 3:03p syang
 * PR 55646:  smoothly filtering  in field inversion cases
 *
 * Hydra_Software_Devel/150   3/16/09 10:44p tdo
 * PR45785, PR45789: Merge from MCVP branch
 *
 * Hydra_Software_Devel/7420_mcvp/1   3/16/09 1:00p tdo
 * PR45785, PR45789: merge from main branch on 3/16/09
 *
 * Hydra_Software_Devel/149   3/13/09 5:28p syang
 * PR 52376, PR 41481, PR 44041:
 * 1). exec user buf returning to hBuffer before shut-down code in
 * writer_isr; 2). clean up mad-delay handling code and add handling to
 * all settings in pic-node that are delayed by mad; 3). 444 SCL no-
 * longer uses BVN_IN to align-up for half-band; 4). add pic-node rects
 * dump feature as setting lbox_0_scratch to 2 (1 still printing mpeg pic
 * info).
 *
 * Hydra_Software_Devel/148   1/15/09 1:15p tdo
 * PR46529, PR51020: Implement const delay for histogram for dynamic
 * contrast use, and avoid histogram update when source freezed
 *
 * Hydra_Software_Devel/147   11/20/08 2:51p rpan
 * PR47337: Added trick mode detection. Replaced feedback based capture
 * polarity prediction mechanism with a non-feedback based mechanism.
 * This would help reduce latency.
 *
 * Hydra_Software_Devel/146   10/23/08 1:58p tdo
 * PR46529: Implement a consistent delay for the dynamic contrast
 * algorithm
 *
 * Hydra_Software_Devel/145   10/8/08 12:17p pntruong
 * PR46735: B0: Need SECAM Dr/Db adjustment implemented.
 *
 * Hydra_Software_Devel/144   10/6/08 4:43p rpan
 * PR47593: When returning a user capture buffer, search the user used
 * buffer nodes  for matching the returned picture node.
 *
 * Hydra_Software_Devel/143   10/6/08 1:11p pntruong
 * PR47595, PR47072: Fixed pixel formats for cap/mad.
 *
 * Hydra_Software_Devel/142   10/3/08 9:23a yuxiaz
 * PR46305, PR46307, PR46309: More Video TestFeature1 support.
 *
 * Hydra_Software_Devel/141   9/26/08 3:23p yuxiaz
 * PR46305, PR46307: More video TestFeature1 support for 3548 B0.
 *
 * Hydra_Software_Devel/140   9/25/08 2:26p tdo
 * PR46529: Implement a consistent delay for histogram data for the
 * dynamic contrast algorithm
 *
 * Hydra_Software_Devel/139   9/25/08 10:46a rpan
 * PR45319: 1) Add 2 more cases that writer can skip one field;
 * 2) Changed Writer(Reader)RateGap to WriterVsReaderRateCode enum for
 * better readablity.
 *
 * Hydra_Software_Devel/138   9/23/08 3:16p rpan
 * PR46865: Flag to indicate whether doing frame capture is now a field of
 * window context.
 *
 * Hydra_Software_Devel/137   9/22/08 4:51p yuxiaz
 * PR46305, PR46307, PR46309: Added video TestFeature1 support for 3548 B0.
 *
 * Hydra_Software_Devel/136   9/17/08 9:16a rpan
 * PR46920: Enhanced how reader matches picture's polarity with VEC's.
 *
 * Hydra_Software_Devel/135   9/15/08 1:39p rpan
 * PR46865: Multi-buffering writer's skip step (1 frame or 2 fields) now
 * bases on whether pictures are captured as frame.
 *
 * Hydra_Software_Devel/134   9/12/08 10:44a rpan
 * PR46794: Improved handling of increasing from 3 to 5 buffers or
 * decreasing from 5 to 3 buffers.
 *
 * Hydra_Software_Devel/133   9/11/08 4:44p rpan
 * PR46793: Re-implemented how buffer nodes get added and released.
 *
 * Hydra_Software_Devel/132   9/10/08 4:48p rpan
 * PR45319: Initialize window object's current reader and writer node to
 * buffer's current reader and write node.
 *
 * Hydra_Software_Devel/131   9/4/08 2:27p rpan
 * PR45319: Further cleaned up multi-buffering stuff from other modules.
 *
 * Hydra_Software_Devel/130   9/4/08 1:25p rpan
 * PR45319: Moved multi-buffering related stuff from window to buffer.
 *
 * Hydra_Software_Devel/129   9/2/08 11:53a rpan
 * PR45319: Covert picture node flags to bit-fields to reduce structure
 * size.
 *
 * Hydra_Software_Devel/128   8/22/08 10:26a rpan
 * PR45979: Remove VBI passthru support.
 *
 * Hydra_Software_Devel/127   8/20/08 2:58p rpan
 * PR45381: Reworked the logic of returning user capture buffer node.
 *
 * Hydra_Software_Devel/126   8/12/08 10:15a rpan
 * PR45381: 1) Added more restriction on which picture node can be
 * returned as user capture buffer; 2) Adding/Removing user capture
 * buffer should be affect vsync delay.
 *
 * Hydra_Software_Devel/125   5/22/08 5:00p rpan
 * PR42965: Abstract rate gap calculation into a function.
 *
 * Hydra_Software_Devel/124   4/22/08 5:13p tdo
 * PR41243: SCL Vertical Init Phase Adjustment For i2i 50->60 Rate
 * Conversion
 *
 * Hydra_Software_Devel/123   2/28/08 6:21p hongtaoz
 * PR39154, PR36273, PR39202: added HSCL block; enabled MAD/ANR for 3548;
 * fixed non-linear scaling when MAD is on;
 *
 * Hydra_Software_Devel/122   2/25/08 6:07p jessem
 * PR 39294: Removed ulDstVertRate from BVDC_P_PictureNode.  Used most
 * current hDisplay->stCurInfo.ulVertFreq for  ulDstVertRate in
 * BVDC_P_Buffer_GetNextReaderNode_isr and
 * BVDC_P_Buffer_GetNextWriterNode_isr
 *
 * Hydra_Software_Devel/121   2/20/08 4:14p hongtaoz
 * PR39154: added window API to toggle ANR; prepare for combo ANR/MAD
 * feature on 3548;
 *
 * Hydra_Software_Devel/120   2/19/08 9:41p pntruong
 * PR33613: Removed hallucination tabs.
 *
 * Hydra_Software_Devel/119   2/7/08 11:51a jessem
 * PR 33613: Reverted back to to version without the modular mutlibuffer.
 * Found a problem with HDDVI sources.
 *
 * Hydra_Software_Devel/117   1/13/08 3:26a albertl
 * PR38150: Added 7440 HDMI xvYCC CSC support.
 *
 * Hydra_Software_Devel/116   11/5/07 11:46a syang
 * PR 29617:  Set SCL's top/left cut with vsync delay according to MAD
 *
 * Hydra_Software_Devel/115   10/23/07 11:15a yuxiaz
 * PR29569, PR36290: Add FGT support on 7405.
 *
 * Hydra_Software_Devel/PR29569/1   10/12/07 10:48a yuxiaz
 * PR29569: Add FGT support on 7405
 *
 * Hydra_Software_Devel/114   8/17/07 9:58a pntruong
 * PR34059: Need to fix FMT name for 1024x768i_43Hz.
 *
 * Hydra_Software_Devel/113   8/2/07 5:33p syang
 * PR 33237: corrected MAD horizontal scale setup
 *
 * Hydra_Software_Devel/112   7/23/07 12:50p jessem
 * PR 32553, PR 33335: Added support for  interlaced with reader rate gap
 * condition (43Hz interlaced PC source). Removed forced pulldown scheme.
 *
 * Hydra_Software_Devel/111   6/27/07 1:46p pntruong
 * PR32503: Cleanup unused code.  Prepare for 480i/480p vdec oversample.
 *
 * Hydra_Software_Devel/110   6/21/07 3:27p syang
 * PR 32440: use pSource->bPictureChanged to decide if we re-decide
 * MAD/ANR heapId and re-init ANR
 *
 * Hydra_Software_Devel/109   5/1/07 4:41p syang
 * PR 28541: 1) changed non-linear scl api; 2). carry sub-pxl accuracy
 * through the pipe line by SrcHStep and SrcVStep
 *
 * Hydra_Software_Devel/108   5/1/07 10:33a hongtaoz
 * PR30075, PR30170: simplified buffer release algorithm; always release
 * from the last active buffer; refactored window buffer code; the
 * minimum clear rect size is required for bClearMosaicOutside;
 *
 * Hydra_Software_Devel/107   4/12/07 11:44a jessem
 * PR 29770: Removed all references to BVDC_P_Last_Buffer_Action_Invalid.
 *
 * Hydra_Software_Devel/106   4/4/07 3:42p syang
 * PR 29243: change VnetMode def (now use bit-fields)
 *
 * Hydra_Software_Devel/105   3/14/07 3:35p jessem
 * PR 28746: Added check to ascertain that total buffer delay when
 * lipsync delay is added is maintained. The buffer delay has a +-1
 * field/frame tolerance.
 *
 * Hydra_Software_Devel/104   2/20/07 2:55p jessem
 * PR25235: Removed the use of rate mask and replaced with actual value of
 * source frame and display refresh rates.
 *
 * Hydra_Software_Devel/103   12/22/06 11:54a syang
 * PR 22569: added BVDC_P_PicComRulInfo as 1st step to do more data
 * isolating between modules. Picture will not be passed to module in
 * future
 *
 * Hydra_Software_Devel/102   12/18/06 11:28p pntruong
 * PR22577: Merged back to mainline.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/10   12/18/06 10:28a jessem
 * PR 25105: Modified BVDC_P_Buffer_ReleasePictureNodes_isr such that the
 * added lipsync delay buffers are released to prevent defragmentation of
 * HD buffers used for SD purposes. Previously, arbitrary buffers were
 * released.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/9   11/8/06 3:10p jessem
 * PR 25422: Enhanced the decision to move the multibuffer read pointer by
 * the writer ISR and vice-versa by using a tolerance (in usecs) based on
 * the largest RUL size and its execution time.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/8   11/8/06 2:26p jessem
 * PR 22987, PR 17846: Re-merge in changes that was previously backed out.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/7   10/20/06 1:41p pntruong
 * PR23521:  Removed duplicate code, and move game mode into existing
 * deinterlace settings structures.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/6   10/19/06 3:53p albertl
 * PR23521:  Added MAD game mode functionality.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/5   10/11/06 2:01p pntruong
 * PR 22987, PR 17846: Back out previous changes as it still need further
 * validations.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/2   9/29/06 2:16p tdo
 * PR 24608:  Add 240p mode support
 *
 * Hydra_Software_Devel/97   9/16/06 7:05p syang
 * PR 23216: partial preparing for moving half band horiz scl to MAD
 *
 * Hydra_Software_Devel/96   9/11/06 5:20p jessem
 * PR 22987: Temporarily backed out of feature that adds dealy buffers
 * without muting the buffers.
 *
 * Hydra_Software_Devel/95   8/28/06 1:43p yuxiaz
 * PR20875: Renamed BVDC_P_SUPPORT_10BIT_BVN to
 * BVDC_P_SUPPORT_BVN_10BIT_444. Removed BVDC_P_BVNDataMode, use window
 * and buffer node's pixel format. Integrated with new 10 bit pixel
 * formats.
 *
 * Hydra_Software_Devel/94   8/23/06 4:22p syang
 * PR 22563: add new smooth non-linear scaling support for 3563; prepare
 * to allow user to set non-linear scl central region percent; move un-
 * known asp-ratio default handling to window's pixel-asp-ratio code
 *
 * Hydra_Software_Devel/93   8/21/06 3:30p yuxiaz
 * PR20875: Added 10bit BVN support for capture and feeder.
 *
 * Hydra_Software_Devel/92   8/15/06 6:15p hongtaoz
 * PR18233, PR20716: added support for mosaic mode - missing channels;
 * assume picture list's channel id are in ascending order; single
 * channel will not result in full-screen automatically;
 *
 * Hydra_Software_Devel/91   8/11/06 10:11a jessem
 * PR 22987, PR 22004: Added support that avoids invalidation of the
 * buffers when ONLY the VSYNC delay has changed.
 *
 * Hydra_Software_Devel/90   7/26/06 9:35a jessem
 * PR 19820: Changed ulSameNodeCnt and ulPrevBufferId to ulRepeatFieldCnt
 * and ulPrevField respectively and removed bChangedPulldownMode  in
 * BVDC_P_BufferContext. Changed multibuffer algorithm in handling 3:2
 * pulldown for 24Hz sources displayed at 60Hz to sync forced repeats
 * with same field occurrence.
 *
 * Hydra_Software_Devel/89   7/24/06 1:27p jessem
 * PR 22384: Removed ulSrcVertRateMask from BVDC_P_PictureNode.
 *
 * Hydra_Software_Devel/88   7/13/06 10:54a jessem
 * PR 22389: Added BVDC_P_Buffer_SetCurWriterNode_isr.
 *
 * Hydra_Software_Devel/87   7/7/06 4:12p yuxiaz
 * PR 22296, PR 22108, PR22109: Removed
 * BVDC_Window_InstallLipSyncDelayCallback, added new generic window
 * callback function BVDC_Window_InstallCallback for lip sync.
 *
 * Hydra_Software_Devel/86   6/28/06 12:22p jessem
 * PR 19820, PR 17554: Added support for 23.976Hz/24Hz/30Hz and 25Hz
 * sources that need to be displayed as 60Hz and 50Hz respectively. One
 * caveat, though, is that the 23.976Hz and 24Hz sources need to be
 * displayed as 60Hz  progressive or 23.976Hz/24Hz; otherwise, there will
 * field inversion issues on the display.
 *
 * Hydra_Software_Devel/85   6/5/06 6:20p hongtaoz
 * PR21714: MAD config for chroma source;
 *
 * Hydra_Software_Devel/84   6/5/06 2:34p pntruong
 * PR14206: Pip display jitters while doing BCM trickmodes(Rew, ff) on
 * recorded HD stream.
 *
 * Hydra_Software_Devel/83   5/3/06 5:29p hongtaoz
 * PR13740: mpeg window uses the repeat flag passed from decoder;
 *
 * Hydra_Software_Devel/82   4/24/06 4:04p yuxiaz
 * PR15161: Fixed VDC _isr naming violations in bvdc_buffer_priv.c and
 * bvdc_bufferheap_priv.c.
 *
 * Hydra_Software_Devel/81   3/29/06 5:32p jessem
 * PR 20422: Adjusted indented space.
 *
 * Hydra_Software_Devel/80   1/31/06 5:22p pntruong
 * PR17778: Take in changes for new hddvi.
 *
 * Hydra_Software_Devel/76   1/27/06 2:58p yuxiaz
 * PR 19157, PR 19138: Added BFMT_AspectRatio_eSAR support for AVC.
 * Removed unused eAspectRatio in BVDC_P_PictureNode.
 *
 * Hydra_Software_Devel/75   1/27/06 1:13p jessem
 * PR 18021: Added support for changing a window's pixel format to
 * BVDC_P_Return/ExtractBuffer_isr.
 *
 * Hydra_Software_Devel/74   1/13/06 4:39p pntruong
 * PR18320: Allow mute constant color when unglug source.
 *
 * Hydra_Software_Devel/73   1/6/06 1:31p pntruong
 * PR18907: Unable to change mute color when already muted.  Also removed
 * not need state in picture.
 *
 * Hydra_Software_Devel/72   10/31/05 6:26p hongtaoz
 * PR17514: add DNR support in VDC;
 *
 * Hydra_Software_Devel/71   10/11/05 8:14a jessem
 * PR 15193: Modified BVDC_P_Buffer_GetNextWriterNode_isr and
 * BVDC_P_Buffer_GetNextReaderNode_isr to use 4 buffers for sync slip
 * mode.
 *
 * Hydra_Software_Devel/70   9/12/05 2:45p jessem
 * PR 12739: Added support for user capture of field and/or frames. New
 * functions are BVDC_P_Buffer_ReturnBuffer_isr() and
 * BVDC_P_Buffer_ExtractBuffer_isr(). Also, added new macros
 * BVDC_P_Buffer_GetNextActiveAndNotUsedByUserNode and
 * BVDC_P_Buffer_GetPrevActiveAndNotUsedByUserNode.
 *
 * Hydra_Software_Devel/69   6/16/05 10:50a syang
 * PR 15794: force MAD off if display is interleaved and
 * (SrcCntH==SclOutH) && SyncLocked
 *
 * Hydra_Software_Devel/68   4/20/05 2:21p pntruong
 * PR14872: Please add counter to BVDC_BUF Repeat messages for long term
 * testing.
 *
 * Hydra_Software_Devel/67   4/14/05 10:22a syang
 * PR 14692: re-organize picture's rectangles, fixed duplicated clips in
 * diff modules, changed to do clip as soon as possible, and added
 * capture clipping for C0
 *
 * Hydra_Software_Devel/66   3/28/05 7:58p pntruong
 * PR14117: VCR mode - recommended register settings.
 *
 * Hydra_Software_Devel/65   3/8/05 7:42p hongtaoz
 * PR12769: implement non-linear horizontal scaling;
 *
 ***************************************************************************/
#ifndef BVDC_BUFFER_PRIV_H__
#define BVDC_BUFFER_PRIV_H__

#include "bavc.h"
#include "bvdc.h"
#include "bpxl.h"
#include "blst_circleq.h"
#include "bvdc_common_priv.h"
#include "bvdc_bufferheap_priv.h"
#if (BVDC_BUF_LOG == 1)
#include "bvdc_dbg.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*
 * When both source and display are interlaced and the picture is not captured
 * as frame, reader has to repeat if it is about to catch up writer. There
 * are 2 possible algorithms to handle this:
 *
 * Algorithm 1: Reader does next-repeat-next-repeat. This will result in 2 field
 * inversion and 2 spreaded repeats over 4 fields.
 *
 * Algorithm 2: Reader does next-repeat-repeat-next. This will result in one picture
 * node being consectively displayed 3 times (2 repeats) but only 1 field inversion.
 *
 * Algorithm 1 favors video with a lot of motion because the 2 repeats are speaded to 4
 * fields. Algorithm 2 favors video with less motion because of only 1 field inversion.
 *
 * Below is a diagram shows the two scenario:
 *
 * VEC polarity     T	B	T	B	T	B	T	B	T	B
 * Captured buffer	T0	B0	T1	B1	T2	B2	T3	B3	T4	B4
 *
 * After reader displayed B0 and is about to move to T1, it detects that it's catching up writer.
 * Algorithm 1 will generate the following reader sequence:
 *
 * Reader (1)       T0	B0	B0	T1	T1	B1	T2	B2	T3	B3
 *
 * There are 2 field inversion at the second B0 and the first T1. Both B0 and T1 are repeated.
 *
 * Algorithm 2 will generate the following sequence:
 *
 * Reader (2) 		T0	B0	B0	B0	T1	B1	T2	B2	T3	B3
 *
 * Only one field inversion at the second B0.  However B0 is repeated twice.
 */

#define BVDC_P_REPEAT_ALGORITHM_ONE  1 /* Default algorithm. Define this macro to 0 to enable
                                        * algorithm 2
                                        */

/***************************************************************************
 * Private macros
 ***************************************************************************/
/* check parameters */
#define BVDC_P_Buffer_GetDeviceOffset(pPictureNode)   \
	pPictureNode->pHeapNode->ulDeviceOffset

#define BVDC_P_Buffer_GetDeviceOffset_R(pPictureNode)   \
	pPictureNode->pHeapNode_R->ulDeviceOffset

#define BVDC_P_Buffer_GetNextNode(pPictureNode)   \
	BLST_CQ_NEXT(pPictureNode, link)

#define BVDC_P_Buffer_GetPrevNode(pPictureNode)   \
	BLST_CQ_PREV(pPictureNode, link)

#define BVDC_P_Buffer_GetNextActiveNode(pNextNode, pNode) \
{                                                         \
	pNextNode = BLST_CQ_NEXT(pNode, link);                \
	if(pNode->hBuffer->ulActiveBufCnt)                    \
	{                                                     \
		while( !pNextNode->stFlags.bActiveNode )                  \
			pNextNode = BLST_CQ_NEXT(pNextNode, link);    \
	}                                                     \
}

#define BVDC_P_Buffer_GetPrevActiveNode(pPrevNode, pNode) \
{                                                         \
	pPrevNode = BLST_CQ_PREV(pNode, link);                \
	if(pNode->hBuffer->ulActiveBufCnt)                    \
	{                                                     \
		while( !pPrevNode->stFlags.bActiveNode )                  \
			pPrevNode = BLST_CQ_PREV(pPrevNode, link);    \
	}                                                     \
}


#define BVDC_P_Buffer_GetNextActiveAndNotUsedByUserNode(pNextNode, pNode) \
{                                                         \
	pNextNode = BLST_CQ_NEXT(pNode, link);                \
	if(pNode->hBuffer->ulActiveBufCnt)                    \
	{                                                     \
		while((!pNextNode->stFlags.bActiveNode) || (pNextNode->stFlags.bUsedByUser))                 \
			pNextNode = BLST_CQ_NEXT(pNextNode, link);    \
	}                                                     \
}

#define BVDC_P_Buffer_GetPrevActiveAndNotUsedByUserNode(pPrevNode, pNode) \
{                                                         \
	pPrevNode = BLST_CQ_PREV(pNode, link);                \
	if(pNode->hBuffer->ulActiveBufCnt)                    \
	{                                                     \
		while((!pPrevNode->stFlags.bActiveNode) || (pPrevNode->stFlags.bUsedByUser))                  \
			pPrevNode = BLST_CQ_PREV(pPrevNode, link);    \
	}                                                     \
}

#define BVDC_P_Buffer_GetNextUsedByUserNode(pNextNode, pNode) \
{                                                         \
	pNextNode = BLST_CQ_NEXT(pNode, link);                \
	while(!pNextNode->stFlags.bUsedByUser)                 \
		pNextNode = BLST_CQ_NEXT(pNextNode, link);    \
}

/***************************************************************************
 * Internal defines
 ***************************************************************************/
#define BVDC_P_BUFFER_PRINT_DBG_MSG           (0)
#define BVDC_P_BUFFER_PRINT_DBG_MSG_CNT       (20)
#define BVDC_P_BUF_DBG_MSG_SIZE               ((200) * sizeof(uint32_t))

#define BVDC_P_BUFFER_NUM_FIELD_CAPTURE_B4_DISPLAY      (5)

/* Progressive detection threshold */
#define BVDC_P_BUFFER_PROG_DETECTION_THRESHOLD          (5)

/* 60Hz go into progressive mode with rate gap! */
#define BVDC_P_BUFFER_PROGRESSIVE_MODE_60_HZ_TRIGGER      (60)
#define BVDC_P_BUFFER_PROGRESSIVE_MODE_59_94_HZ_TRIGGER   (59)

/* Maximum active picture nodes of each buffer linked list */
#define BVDC_P_BUFFER_MAX_ACTIVE_NODES_PER_BUFT			  (24)

#ifdef BCHP_RDC_desc_0_tm_snapshot
#define BVDC_P_USE_RDC_TIMESTAMP         (0)
#else
#define BVDC_P_USE_RDC_TIMESTAMP         (0)
#endif

/***************************************************************************
 * Enums
 ***************************************************************************/
typedef enum BVDC_P_Last_Buffer_Action
{
	BVDC_P_Last_Buffer_Action_Reader_Moved,
	BVDC_P_Last_Buffer_Action_Writer_Moved
} BVDC_P_Last_Buffer_Action;

/****************************************************************************
 * dirty bits for sub-modules to build RUL.
 */
typedef union
{
	struct{
		uint32_t      bInputFormat      : 1; /* from user for VDEC
											  * from user or XVD for HD_DVI */
		uint32_t      bSrcPicChange     : 1; /* either src detected change or ApplyChange called */

	} stBits;

	uint32_t aulInts [BVDC_P_DIRTY_INT_ARRAY_SIZE];
} BVDC_P_PicDirtyBits;


/***************************************************************************
 * BVDC_P_PicComRulInfo
 *
 * Contains the pic info that are shared by all sub-modules when they build
 * RUL
 *
 * TODO: all sub-modules' BuildRul_isr should use the shared struct
 * BVDC_P_PicComRulInfo and a module specific BVDC_P_Pic*RulInfo as input
 * parameters.
 ***************************************************************************/
typedef struct BVDC_P_PicComRulInfo
{
	/* used by a module to remember the 1st window that actively uses it and
	 * therefore it should build the rul for the module */
	BVDC_P_WindowId                eWin;

	/* used by sub-modules to easily decide what init it needs to do when
	 * they build its rul at this picture cycle */
	BVDC_P_PicDirtyBits            PicDirty;

	/* Original source polarity. Updated every field. Used by multi-buffering. */
	BAVC_Polarity                  eSrcOrigPolarity;

	bool                           bDrTf; /* Secam Dr/Db is on Topfield */

} BVDC_P_PicComRulInfo;


/***************************************************************************
Summary:
	A structure representing the flags used by a picture node. .

****************************************************************************/
typedef struct
{
	uint32_t    bActiveNode               : 1;        /* Whether the node is active or not */
	uint32_t    bFormatChange             : 1;        /* Source information */
	uint32_t    bMute                     : 1;        /* Used by video feeder, not mpeg feeder */
	uint32_t    bMuteFixedColor           : 1;        /* Used by video feeder, not mpeg feeder */
	uint32_t    bVcrHeadSwitch            : 1;        /* Used by video feeder, not mpeg feeder */
	uint32_t    bMuteMad                  : 1;        /* Used by video feeder, not mpeg feeder */
	uint32_t    b240pMode                 : 1;        /* VDEC 240p mode */
	uint32_t    bPictureRepeatFlag        : 1;        /* previous field polarity, for trick mode support */
	uint32_t    bRepeatField              : 1;        /* previous field or frame for anr */
	uint32_t    bUsedByUser               : 1;        /* for user capture */
	uint32_t    bHandleFldInv             : 1;        /* force smooth scl for expected field inversion */
	uint32_t	bCadMatching			  : 1;        /* Match captured polarity against VEC polarity */
} BVDC_P_PicNodeFlags;



/***************************************************************************
 * BVDC_P_PictureNode
 *
 * Contains data in capture buffering. Data are used to build RUL.
 ***************************************************************************/
typedef struct BVDC_P_PictureNode
{
	/* Node info: linked-list bookeeping */
	BLST_CQ_ENTRY(BVDC_P_PictureNode)  link;                 /* doubly-linked list support */
	uint32_t                           ulBufferId;           /* Buffer ID */

	BVDC_P_Buffer_Handle               hBuffer;
	BVDC_P_HeapNodePtr                 pHeapNode;            /* Hold capture memory */

	/* Hold capture memory for Right buffer */
	BVDC_P_HeapNodePtr                 pHeapNode_R;
	/* Capture BVB input orientation */
	BFMT_Orientation                   eOrigSrcOrientation;
	/* Capture BVB input orientation, could be overwrite by Scaler 3D->2D conversion*/
	BFMT_Orientation                   eSrcOrientation;
	/* Video feeder BVB output orientation */
	BFMT_Orientation                   eDispOrientation;
	BVDC_3dSourceBufferSelect          e3dSrcBufSel;

	/* Field inversion */
	BAVC_Polarity                      eDisplayPolarity;

	BAVC_FrameRateCode                 eFrameRateCode;       /* Frame rate code.
	                                                          * Updated every format change.
	                                                          * Used by scaler?
	                                                          */
	BAVC_MatrixCoefficients            eMatrixCoefficients;  /* Matrix coefficients.
	                                                          * Updated every format change.
	                                                          * Used by scaler?
	                                                          */
	BAVC_TransferCharacteristics       eTransferCharacteristics; /* transfer characteristics */

	BAVC_Polarity                      eSrcPolarity;         /* Top, bottom field or progressive.
	                                                          * Updated every field.
	                                                          * Used by MAD, video feeder and scaler.
	                                                          */
	BAVC_Polarity                      eDstPolarity;         /* Top, bottom field or progressive.*/
	uint32_t                           ulFgtSeiBufAddr;      /* FGT SEI buffer address */
	uint32_t                           ulIdrPicID;           /* FGT Picture Order Count Offset.*/
	uint32_t                           ulPicOrderCnt;        /* FGT Picture Order Count */


	/*Mailbox metadata for ViCE2 particularly*/
	uint32_t                           ulOrigPTS;
	int32_t                            iHorzPanScan;
	int32_t                            iVertPanScan;
	uint32_t                           ulDispHorzSize;
	uint32_t                           ulDispVertSize;
	uint32_t                           ulDecodePictureId;
	uint32_t                           ulStgPxlAspRatio_x_y; /* PxlAspRatio_x<<16 | PxlAspRatio_y */

	BAVC_USERDATA_PictureCoding        ePictureType;
	bool                               bIgnorePicture;
	bool                               bStallStc;
	bool                               bLast;
	bool                               bChannelChange;
	bool                               bValidAfd;
	uint32_t                           ulAfd;
	BAVC_BarDataType                   eBarDataType;
	uint32_t                           ulTopLeftBarValue;
	uint32_t                           ulBotRightBarValue;
	/*  Fast non real time (FNRT) meta data support */
	bool                              bPreChargePicture;
	bool                              bEndofChunk;
	uint32_t                          ulChunkId;

	/* Working rectangles for mpeg feeder, scaler, capture and the following video
	 * feeder:
	 *    Source content rectangle, and user's scaler-out and destination rectangles
	 * are adjusted by BVDC_P_Window_AdjustRectangles_isr().  For a majority of time if
	 * there are no user's changes nor source's stream changes, the rectangles will
	 * stay the same.
	 *    Both scan (i.e. mpeg feeder), capture, video feeder, and compositor
	 * could perform clip. However, only scaler can clip to sub-pixel level.
	 * Mad will not perform any cut.
	 *    Basing on the order defined by vnet mode, we do clip as early as
	 * possible.
	 *    In the case that the previous modules could not perform the needed clip
	 * completely, the following modules clip the rest. For example, scan might
	 * not be able to do clip completely becase another display share the mpeg
	 * source, then the following module should complete the clipping. Another
	 * example is that capture and feeder can only clip to pixel boundary, sub-
	 * pixel clipping should be done by the following scaler.
	 *    Pan-Scan is handled uniformly with other clipping. Sub-pixel pan-scan
	 * is considered as sub-pixel clip.
	 *    stSclCut.lLeft is for horizontal sub-pixel clipping. It is units of 1/16th
	 * of a pixel in S11.6 format. Bit 6 is the pixel grid, bits 5:0 are 1/16
	 * pixel grid. Even number of pixels are handled in feeder. It is updated every
	 * field and is used by scaler.
	 *    stSclCut.lTop is for vertical sub-pixel clipping.  It is units of 1/16th of
	 * a pixel in S11.14 format. Bits 3:0 are 1/16 pixel grid. Down to pixels are handled
	 * in feeder. Updated every field.  Used by scaler. */
	BVDC_P_Rect                        stSrcOut;             /* What mfd/vdec/hddvi output. */
	BVDC_P_Rect                        stSclOut;             /* What scaler output. */
	BVDC_P_Rect                        stWinIn;              /* vsur size and L/T cut*/
	BVDC_P_Rect                        stWinOut;             /* vdisp size and pos */
	BVDC_P_Rect                        stSclCut;             /* rect for scaler to pickup src cnt */
	BVDC_P_Rect                        stCapOut;             /* rect for cap out, with cut */
	BVDC_P_Rect                        stVfdOut;             /* rect for vfd out, with cut */
	BVDC_P_Rect                        stMadOut;             /* What MAD output. */

	/* Point to one of the above rectangles.
	 *    Clip in mpeg feeder is specified by pSrcOut, clip in scaler is specified
	 * by stSclCut, clip in capture is specified by pCapOut, clip in feeder is
	 * specified by pVfdOut, and clip in compositor is specified by pWinIn. pSrcOut,
	 * pSclIn, pCapIn, pVfdIn only specify the input width and height. */
	BVDC_P_Rect                       *pSrcOut;              /* -> stSrcOut */

	BVDC_P_Rect                       *pDnrIn;               /* -> pSrcOut */
	BVDC_P_Rect                       *pDnrOut;              /* -> pDnrIn */

	BVDC_P_Rect                       *pHsclIn;              /* -> pSrcOut */
	BVDC_P_Rect                       *pHsclOut;             /* -> Hrz scaled */

	BVDC_P_Rect                       *pAnrIn;               /* -> pSrcOut, pVfdOut, pHsclOut */
	BVDC_P_Rect                       *pAnrOut;              /* -> pAnrIn */

	BVDC_P_Rect                       *pMadIn;               /* -> pSrcOut, pHsclOut, pVfdOut */
	BVDC_P_Rect                       *pMadOut;              /* -> Hrz scaled w/ framebased */

	BVDC_P_Rect                       *pSclIn;               /* -> pSrcOut, pVfdOut, pMadOut */
	BVDC_P_Rect                       *pSclOut;              /* -> stScalerOutRect */

	BVDC_P_Rect                       *pCapIn;               /* -> pSrcOut, pMadOut, pSclOut */
	BVDC_P_Rect                       *pCapOut;              /* -> pCapIn */

	BVDC_P_Rect                       *pVfdIn;               /* -> pCapOut */
	BVDC_P_Rect                       *pVfdOut;              /* -> pSclOut, stVfdOutput */

	BVDC_P_Rect                       *pFgtIn;               /* -> pSrcOut, pSclOut, pVfdOut */
	BVDC_P_Rect                       *pFgtOut;              /* -> pFgtIn */

	BVDC_P_Rect                       *pWinIn;               /* -> pSrcOut, pSclOut, pVfdOut */
	BVDC_P_Rect                       *pWinOut;              /* -> stDstOutput */

	/* Dest information */
	BVDC_P_VnetMode                    stVnetMode;           /* Dictates what resources to use. */


	/* VEC/VDEC phase adjustment */
	uint32_t                           ulVdecPhase;

	/* Adjusted Quantization Paramter for DNR */
	uint32_t                           ulAdjQp;

	/* for scaling in SCL */
	uint32_t                           ulNrmHrzSrcStep; /* normalized hrz scl factor in fixed point format */
	uint32_t                           ulNrmVrtSrcStep; /* normalized vrt scl factor in fixed point format */
	uint32_t                           ulNonlinearSrcWidth;/* in pxl unit */
	uint32_t                           ulNonlinearSclOutWidth;/* in pxl unit */
	uint32_t                           ulCentralRegionSclOutWidth;/* in pxl unit */

	/* TODO: Use hSclCut */
	/* for h-scaling before deinterlacing */
	uint32_t                           ulHsclNrmHrzSrcStep; /* normalized hrz scl factor in fixed point format */
	int32_t                            lHsclCutLeft; /* S11.6, same fmt as SclCut->lLeft */
	uint32_t                           ulHsclCutWidth; /* similar to SclCut->ulWidth */
	int32_t                            lHsclCutLeft_R; /* S11.6, same fmt as SclCut->lLeft_R */

	BSUR_Surface_Handle                hSurface;
	BSUR_Surface_Handle                hSurface_R;
	BPXL_Format                        ePixelFormat;

	/* to be used by MAD chroma config (phase 3 & beyond) */
	BVDC_P_ChromaType                  eChromaType;

	/* Lip sync */
	uint32_t                           ulCaptureTimestamp;
	uint32_t                           ulPlaybackTimestamp;

	/* MosaicMode: ClearRect bit mask set (each bit corresponds to a mosaic rectangle, where
	   value 1 for enabled rect; value 0 for missing/invisible channel) */
	uint32_t                           ulMosaicRectSet;

	BVDC_P_TestFeature1_Settings        stCapTestFeature1;

	/* pic info shared by all sub-modules when they build rul
	 *
	 * TODO: all sub-modules' BuildRul_isr should use the shared struct
	 * BVDC_P_PicComRulInfo and a module specific BVDC_P_Pic*RulInfo as input
	 * parameters, in order to have clear boundary / interface between sub-
	 * modules.
	 */
	BVDC_P_PicComRulInfo               PicComRulInfo;

	/* bit flags used by picture node */
	BVDC_P_PicNodeFlags                stFlags;

	BVDC_LumaStatus                    stCurHistData;
	uint32_t                           ulCurHistSize;

	uint8_t                            ucAlpha;
} BVDC_P_PictureNode;


/***************************************************************************
 * BRDC_P_Slot_Head
 *      Head of the double Link List for slot
 ***************************************************************************/
typedef struct BVDC_P_Buffer_Head  BVDC_P_Buffer_Head;
BLST_CQ_HEAD(BVDC_P_Buffer_Head, BVDC_P_PictureNode);

/***************************************************************************
 * Writer vs Reader rate enum
 *
 * This enum defines the three cases when comparing writer rate with
 * reader rate.
 *
 * Note: Round off is applied when calculating rate. So 59.97Hz and 60Hz
 * are considered same rate.
 *
 ***************************************************************************/
typedef enum _BVDC_P_WrRateCode
{
	BVDC_P_WrRate_NotFaster = 0,  /* Same rate or slower */
	BVDC_P_WrRate_Faster, /* Faster but not 2 times or more.
	                       * i.e. writer = 50Hz, reader = 60Hz
	                       */
	BVDC_P_WrRate_2TimesFaster /* 2 times or even more faster.
	                            * i.e. writer = 24Hz, reader = 60Hz
	                            * This usually is the pulldown case
	                            */

} BVDC_P_WrRateCode;

/***************************************************************************
 * Buffer Context
 *
 * Contains data per window. Each buffer context contains a list of buffer
 * nodes, a write pointer and a read pointer.
 ***************************************************************************/
typedef struct BVDC_P_BufferContext
{
	BDBG_OBJECT(BVDC_BUF)

	BVDC_Window_Handle             hWindow;       /* Created from this window */

	bool                           bSyncLock;

	uint32_t                       ulActiveBufCnt; /* Number of active buffers */
	uint32_t                       ulBufCnt;      /* Number of buffers */
	uint32_t                       ulSkipCnt;

	/* Skip and Repeat statistic */
	uint32_t                       ulSkipStat;
	uint32_t                       ulRepeatStat;

	/* This is the delay between writer and reader */
	uint32_t                       ulVsyncDelay;

	BVDC_P_Buffer_Head            *pBufList;      /* Double link list */

	BVDC_P_PictureNode            *pCurWriterBuf; /* Write buffer pointer.
	                                               * Points to capture buffer when
	                                               * capture is enabled.
	                                               */
	BVDC_P_PictureNode            *pCurReaderBuf; /* Read buffer pointer.
	                                               * Points to playback buffer when
	                                               * capture is enabled.
	                                               */

	uint32_t                       ulNumCapField; /* Number of field captured */


	/* for progressive mode rates that are not 5060 Hz (VESA formats) */
	BVDC_P_WrRateCode              eWriterVsReaderRateCode;
	BVDC_P_WrRateCode              eReaderVsWriterRateCode;

	/* game mode buffer delay sampling interval */
	uint32_t                       ulGameDelaySamplePeriod;
	uint32_t                       ulGameDelaySampleCnt;

	BAVC_Polarity                  ePrevFieldId;

	/* indicates which buffer action executed last */
	BVDC_P_Last_Buffer_Action      eLastBuffAction;

	/* to track reader and writer isr firing order */
	uint32_t                      ulPrevWriterTimestamp;
	uint32_t                      ulCurrWriterTimestamp;
	uint32_t                      ulPrevReaderTimestamp;
	uint32_t                      ulCurrReaderTimestamp;
	uint32_t                      ulMaxTimestamp;
	bool                          bReaderWrapAround;
	bool                          bWriterWrapAround;
	bool 			  			  bReaderNodeMovedByWriter;
	bool				          bWriterNodeMovedByReader;

	/* This array keeps the order of the active picture nodes being added to the
	 * buffer linked list. When releasing a picture node, it will be
	 * removed using the reverse order of when it was added. Basically
	 * a picture node is last in and first out. This is to help
	 * buffer heap fragmentation.
	 */
	BVDC_P_PictureNode			 *aBufAdded[BVDC_P_BUFFER_MAX_ACTIVE_NODES_PER_BUFT];
	int							  iLastAddedBufIndex;

#if BVDC_P_REPEAT_ALGORITHM_ONE
	/* a flag to indicate that reader repeats so for not catching up writer
	 */
    bool                          bRepeatForGap;
#endif

} BVDC_P_BufferContext;


/***************************************************************************
 * Buffer private functions
 ***************************************************************************/
BERR_Code BVDC_P_Buffer_Create
	( const BVDC_Window_Handle         hWindow,
	  BVDC_P_Buffer_Handle            *phBuffer );

BERR_Code BVDC_P_Buffer_Destroy
	( BVDC_P_Buffer_Handle             hBuffer );

void BVDC_P_Buffer_Init
	( BVDC_P_Buffer_Handle             hBuffer );

BERR_Code BVDC_P_Buffer_AddPictureNodes_isr
	( BVDC_P_Buffer_Handle             hBuffer,
	  BVDC_P_HeapNodePtr               apHeapNode[],
	  BVDC_P_HeapNodePtr               apHeapNode_R[],
	  uint32_t                         ulSurfaceCount,
	  uint32_t                         ulBufDelay,
	  bool                             bSyncLock,
	  bool                             bInvalidate );

BERR_Code BVDC_P_Buffer_ReleasePictureNodes_isr
	( BVDC_P_Buffer_Handle             hBuffer,
	  BVDC_P_HeapNodePtr               apHeapNode[],
	  BVDC_P_HeapNodePtr               apHeapNode_R[],
	  uint32_t                         ulSurfaceCount,
	  uint32_t                         ulBufDelay );

BERR_Code BVDC_P_Buffer_Invalidate_isr
	( BVDC_P_Buffer_Handle             hBuffer );

BVDC_P_PictureNode* BVDC_P_Buffer_GetPrevWriterNode_isr
	( const BVDC_P_Buffer_Handle       hBuffer );

BVDC_P_PictureNode* BVDC_P_Buffer_GetNextWriterNode_isr
	( BVDC_Window_Handle     hWindow,
	  const BAVC_Polarity    eSrcPolarity);

BVDC_P_PictureNode* BVDC_P_Buffer_GetNextReaderNode_isr
	( BVDC_Window_Handle     hWindow,
	  const BAVC_Polarity    eVecPolarity);

void BVDC_P_Buffer_SetCurWriterNode_isr
	( const BVDC_P_Buffer_Handle       hBuffer,
	  BVDC_P_PictureNode              *pPicture);

void BVDC_P_Buffer_SetCurReaderNode_isr
	( const BVDC_P_Buffer_Handle       hBuffer,
	  BVDC_P_PictureNode              *pPicture);

void  BVDC_P_Buffer_ReturnBuffer_isr
	( BVDC_P_BufferContext   *pBuffer,
	BVDC_P_PictureNode *pPicture);

BERR_Code  BVDC_P_Buffer_ExtractBuffer_isr
	( BVDC_P_BufferContext   *pBuffer,
	BVDC_P_PictureNode   **ppPicture);

#define BVDC_P_Buffer_GetCurrWriterNode_isr(hBuffer) \
    ((hBuffer)->pCurWriterBuf)

#define BVDC_P_Buffer_GetCurrReaderNode_isr(hBuffer) \
    ((hBuffer)->pCurReaderBuf)

void BVDC_P_Buffer_CalculateRateGap
	( const uint32_t        ulSrcVertFreq,
	  const uint32_t        ulDispVertFreq,
	  BVDC_P_WrRateCode    *peWriterVsReaderRateCode,
	  BVDC_P_WrRateCode    *peReaderVsWriterRateCode);

uint32_t BVDC_P_Buffer_CalculateBufDelay
	( BVDC_P_PictureNode   *pPicture);

BVDC_P_PictureNode * BVDC_P_Buffer_GetCurWriterNode
	( const BVDC_P_Buffer_Handle       hBuffer);

BVDC_P_PictureNode * BVDC_P_Buffer_GetCurReaderNode
	( const BVDC_P_Buffer_Handle       hBuffer);

#if (BVDC_P_SUPPORT_3D_VIDEO)
BERR_Code BVDC_P_Buffer_SetRightBufferPictureNodes_isr
	( BVDC_P_Buffer_Handle             hBuffer,
	  BVDC_P_HeapNodePtr               apHeapNode_R[],
	  uint32_t                         ulCount,
	  bool                             bAdd);
#endif

#if (BVDC_BUF_LOG == 1)
void BVDC_P_Buffer_SetLogStateAndDumpTrigger
	( BVDC_BufLogState                 eLogState,
  	  const BVDC_CallbackFunc_isr 	   pfCallback,
	  void							   *pvParm1,
	  int 							   iParm2 );

void BVDC_P_Buffer_DumpLog(void);

void BVDC_P_Buffer_SetManualTrigger(void);

void BVDC_P_Buffer_EnableBufLog
	( BVDC_P_WindowId                   eId,
	  bool                              bEnable);

#endif


#ifdef __cplusplus
}
#endif

#endif /* #ifndef BVDC_BUFFER_PRIV_H__*/

/* End of file. */
