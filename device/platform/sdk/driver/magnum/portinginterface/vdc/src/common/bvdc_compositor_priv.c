/***************************************************************************
 *     Copyright (c) 2003-2013, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvdc_compositor_priv.c $
 * $brcm_Revision: Hydra_Software_Devel/SW7346-1085/1 $
 * $brcm_Date: 2/13/13 11:16p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vdc/src/common/bvdc_compositor_priv.c $
 * 
 * Hydra_Software_Devel/351   6/13/13 1:39p pntruong
 * SW7425-4893: Corrected window index for mosaic csc.
 * 
 * Hydra_Software_Devel/349   4/2/13 3:00p pntruong
 * SW7425-4579: Keep track of mosaic on a per window context.
 *
 * Hydra_Software_Devel/SW7346-1085/1   2/13/13 11:16p scho
 * SW7346-1085:  Included eSmpte_240M as hd colorspace.
 * 
 * Hydra_Software_Devel/331   8/28/12 11:22a vanessah
 * SW7435-319: fix the initial state synslip cmp/disp size mismatch
 * 
 * Hydra_Software_Devel/330   8/15/12 6:21p tdo
 * SW7445-8: Create Appframework emulation environment for 7445 A0
 *
 * Hydra_Software_Devel/329   8/13/12 4:02p tdo
 * SW7425-3734: Fix coverity issues
 *
 * Hydra_Software_Devel/328   8/13/12 3:07p yuxiaz
 * SW7425-3626, SW7425-3619: Replace uncache memory access with cache
 * memory access: use the new API names in RDC.
 *
 * Hydra_Software_Devel/327   8/8/12 4:22p vanessah
 * SW7425-3704: add gfx xcode channelchange flag
 *
 * Hydra_Software_Devel/326   6/29/12 3:28p vanessah
 * SW7425-3359: FNRT flags support to ViCE MBox
 *
 * Hydra_Software_Devel/325   6/20/12 12:21p darnstein
 * SW7405-5643: ARIB option takes precedence over NTSC-704 option.
 *
 * Hydra_Software_Devel/324   6/6/12 4:51p tdo
 * SW7552-279: video(color or brightness) is abnormal after change channel
 * to NTSC SD program when brightness/contrast/saturation/Hue is not
 * default value
 *
 * Hydra_Software_Devel/323   5/3/12 9:24p vanessah
 * SW7425-2873: transcode path fmt stress test
 *
 * Hydra_Software_Devel/322   5/1/12 3:05p vanessah
 * SW7425-2643: add bar data support
 *
 * Hydra_Software_Devel/321   4/12/12 4:11p vanessah
 * SW7425-2831: undo the changes
 *
 * Hydra_Software_Devel/320   4/6/12 8:00p vanessah
 * SW7425-2831: change polarity due to ignore frame flag
 *
 * Hydra_Software_Devel/319   3/27/12 11:42a yuxiaz
 * SW7425-2753: Back out previous change. Treat display orientation change
 * as format change to flush all the buffers in video windows to avoid
 * BVN errors.
 *
 * Hydra_Software_Devel/318   3/26/12 10:55a yuxiaz
 * SW7425-2753: Fixed BVN errors for 3d conversion.
 *
 * Hydra_Software_Devel/317   2/23/12 5:21p tdo
 * SW7429-71: Fix splitscreen demo mode boundary issue
 *
 * Hydra_Software_Devel/316   2/23/12 11:28a vanessah
 * SW7425-1783: freeze picture id only when nrt ignore picture
 *
 * Hydra_Software_Devel/315   2/23/12 10:17a vanessah
 * SW7425-2431: fix custom fmt stress test
 *
 * Hydra_Software_Devel/314   1/30/12 4:05p vanessah
 * SW7425-2239: add  bChannelChange in VCE Mbox
 *
 * Hydra_Software_Devel/313   1/23/12 3:31p tdo
 * SW7435-9: Add support for CMP4/5
 *
 * Hydra_Software_Devel/312   1/16/12 11:10a vanessah
 * SW7425-2084: add bLast flage to VCD Mbox
 *
 * Hydra_Software_Devel/311   12/1/11 4:31p yuxiaz
 * SW7425-968, SW7344-95: Merged into mainline.: added independent source
 * clipping of right window in 3D mode.
 *
 * Hydra_Software_Devel/310   11/29/11 11:17a vanessah
 * SW7425-1783: add user data support in VDC PI
 *
 * Hydra_Software_Devel/309   11/23/11 4:24p tdo
 * SW7435-9: Add support for CMP4-5, GFD4-5, MFD3, VFD5
 *
 * Hydra_Software_Devel/308   10/10/11 3:17p pntruong
 * SW7425-1439: Fixed initialization of cmp reset/mask addr.
 *
 * Hydra_Software_Devel/307   8/31/11 11:17a syang
 * SW7425-1170: pxlAspRatio passed to ViCE2 is corrected to x<<16 | y
 * format
 *
 * Hydra_Software_Devel/306   8/27/11 7:22p hongtaoz
 * SW7425-1132: replaced slip2lock boolean flag with integer counter to
 * fix a timing sensitive hang conditiion when NRT sync-locked window is
 * brought up;
 *
 * Hydra_Software_Devel/305   8/26/11 5:38p syang
 * SW7425-1170: merge from branch SW7572-1170
 *
 * Hydra_Software_Devel/SW7425-1170/3   8/26/11 4:21p syang
 * SW7425-1170: refactor pixel aspect ratio related code
 *
 * Hydra_Software_Devel/SW7425-1170/2   8/24/11 6:55p vanessah
 * SW7425-1170: ASP code clean
 *
 * Hydra_Software_Devel/SW7425-1170/1   8/23/11 6:30p vanessah
 * SW7425-1170: Aspect ratio cleanup
 *
 * Hydra_Software_Devel/304   8/21/11 5:37p hongtaoz
 * SW7425-592: init CMP SAR X&Y with nonzero value;
 *
 * Hydra_Software_Devel/303   8/3/11 9:37a vanessah
 * SW7425-1047: ViCE MBox ulPicId temporary solution.
 *
 * Hydra_Software_Devel/302   8/2/11 7:32p vanessah
 * SW7425-465: optimization of delay counter
 *
 * Hydra_Software_Devel/301   7/28/11 5:21p vanessah
 * SW7425-465: add canvas size debug msg
 *
 * Hydra_Software_Devel/300   7/22/11 10:58a yuxiaz
 * SW7420-1959: Back out support of non-full content mode for PIP, use
 * mosaic APIs instead.
 *
 * Hydra_Software_Devel/299   7/18/11 5:31p pntruong
 * SW7231-284, SW7346-295, SW7405-5339: Refactored master framerate
 * tracking, so that it can also handle validation.
 *
 * Hydra_Software_Devel/298   7/18/11 11:19a vanessah
 * SW7425-835: SW7425-923: fix ViCE2 channel id bug + B0 STG
 *
 * Hydra_Software_Devel/297   6/30/11 6:51p yuxiaz
 * SW7420-1959: Added support of non-full content mode for PIP.
 *
 * Hydra_Software_Devel/296   6/28/11 2:56p pntruong
 * SW7405-5394: Added more error messages for incorrect usage of master
 * framerate tracking.
 *
 * Hydra_Software_Devel/295   6/24/11 2:58p albertl
 * SW7420-1966: Cleaned up and reorganized colorspace enums and indicies
 * in display, vdec, and cmp.
 *
 * Hydra_Software_Devel/294   6/22/11 3:07p yuxiaz
 * SW7425-592: Added API to get and set display sample aspect ratio
 * settings.
 *
 * Hydra_Software_Devel/293   6/20/11 5:49p tdo
 * SW7425-104: Rename the old repeat field flag to match avc structure and
 * use the new added bRepeatField flag for ANR.
 *
 * Hydra_Software_Devel/292   6/17/11 5:06p vanessah
 * SW7425-686: Enable cmp in init stage
 *
 * Hydra_Software_Devel/291   6/10/11 5:49p hongtaoz
 * SW7425-704: add NRT mode support to VDC; moved the STG meta data
 * passing from window writer isr to reader isr; added bStallStc flag
 * support;
 *
 * Hydra_Software_Devel/290   6/9/11 9:29a pntruong
 * SW7400-3034: Corrected the error code for z-order settings.
 *
 * Hydra_Software_Devel/289   5/31/11 11:08a vanessah
 * SW7425-465: fix auto_vbi test segmentation fault error
 *
 * Hydra_Software_Devel/288   5/27/11 6:36p vanessah
 * SW7425-465: fix video transcoding resolution switch transition artifact
 *
 * Hydra_Software_Devel/287   5/11/11 3:50p pntruong
 * SW7231-140: Correctly compute the MFD fieldswap flag.
 *
 * Hydra_Software_Devel/286   2/7/11 6:02p vanessah
 * SW7425-46: STG Mbox data programming
 *
 * Hydra_Software_Devel/285   2/7/11 5:25p vanessah
 * SW7425-46: add MFD mute to the skip picture to start quality comparison
 *
 * Hydra_Software_Devel/284   12/21/10 11:05a tdo
 * SW7422-28: Adjust to 3D format size change
 *
 * Hydra_Software_Devel/283   12/9/10 5:50p tdo
 * SW7422-44: Bring up 3D full resolution formats
 *
 * Hydra_Software_Devel/282   12/8/10 3:28p yuxiaz
 * SW7422-44: bring up half resolution 3d display.
 *
 * Hydra_Software_Devel/281   12/1/10 6:10p tdo
 * SW7422-44: Handle 3D changes for display
 *
 * Hydra_Software_Devel/280   11/16/10 6:50p tdo
 * SW7422-92: Need display 3D orientation override function for VDC
 *
 * Hydra_Software_Devel/279   11/11/10 7:27p albertl
 * SW7125-364: Fixed BVDC_P_CbIsDirty and added assert to check bitfields
 * in dirty bits fit within union integer representation.  Fixed naming
 * of dirty bits.
 *
 * Hydra_Software_Devel/278   10/28/10 6:15p tdo
 * SW7422-42:  Update compositor 3D programming.
 *
 * Hydra_Software_Devel/277   10/27/10 9:20a vanessah
 * SW7425-46:  Digital resolution instead of the analog one
 *
 * Hydra_Software_Devel/276   10/22/10 7:01p vanessah
 * SW7425-46:  ViCE2 meta data mailbox register
 *
 * Hydra_Software_Devel/275   10/4/10 5:24p hongtaoz
 * SW7425-33: added the 4th compositor support;
 *
 * Hydra_Software_Devel/274   9/29/10 11:41a tdo
 * SW7425-33: VDC: Add the 4th Compositor Support for 7422/7425
 *
 * Hydra_Software_Devel/273   8/26/10 5:24p tdo
 * SW7422-57: Add simple vdc support
 *
 * Hydra_Software_Devel/272   8/5/10 11:32a pntruong
 * SW7408-117: Added support for 7408B0 CMP changes.
 *
 * Hydra_Software_Devel/271   5/7/10 7:08p albertl
 * SW7125-364: Changed dirty bits to use union structure to avoid type-pun
 * warnings.
 *
 * Hydra_Software_Devel/270   4/19/10 10:13p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic. Move
 * BDBG_OBJECT_ID_DECLARE private header files instead of .c.
 *
 * Hydra_Software_Devel/269   4/7/10 11:27a tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic.  Rename TLA
 *
 * Hydra_Software_Devel/268   4/5/10 4:00p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic
 *
 * Hydra_Software_Devel/267   3/17/10 12:01p syang
 * SW7405-4046: set canvas ctrl after sur ctrl; handle gfx win vnet the
 * same as video; ensure that reader and writer vnetState match in sync-
 * locked case; aligned width up to even for stride in feeder and cap;
 * assert mosaic mode doesn't co-exist with dest cut;
 *
 * Hydra_Software_Devel/266   1/5/10 11:08a rpan
 * SW7468-30: Moved OSCL enable after CMP reset.
 *
 * Hydra_Software_Devel/265   12/21/09 2:44p rpan
 * SW7468-30: 1) When OSCL is enabled for 1080p display, top and bottom
 * RDC slots are used for building RULs.
 * 2) Addressed the video format stored at hCompistor being overwritten
 * issue.
 *
 * Hydra_Software_Devel/264   12/15/09 1:56p rpan
 * SW7468-30: Brought up 1080p output on 7468.
 *
 * Hydra_Software_Devel/263   11/30/09 3:23p rpan
 * SW7468-30: More OSCL support.
 *
 * Hydra_Software_Devel/262   11/16/09 5:03p yuxiaz
 * SW7405-3406: Clean up bMosaicMode and  bMosaicClearOutside.
 *
 * Hydra_Software_Devel/261   10/27/09 10:53p albertl
 * SW7420-30: Changed mosaic matrices to be loaded statically with xvYcc
 * matrix loaded dynamically as necessary.
 *
 * Hydra_Software_Devel/260   10/22/09 2:23p yuxiaz
 * SW7405-3200: Destroy PIP window causes momentary flashes on the display
 * in mosaic mode.
 *
 * Hydra_Software_Devel/259   10/9/09 5:28p syang
 * SW7405-2516, SW3548-2439, SW7405-3046: rewrite HSCL setinfo_isr and
 * TryXferHrzSclToHscl_isr to do crop correctly; cleanup width alignment
 * for DCX_HSIZE_WORKAROUND
 *
 * Hydra_Software_Devel/258   8/13/09 4:15p syang
 * PR 55812: handle non-supported PEP the same way as 7405
 *
 * Hydra_Software_Devel/257   8/13/09 2:17p syang
 * PR 55812: avoid to call Pep_BuildRul_isr for 7550
 *
 * Hydra_Software_Devel/256   7/15/09 12:14p syang
 * PR 56605: correct sur/disp size msg
 *
 * Hydra_Software_Devel/255   6/18/09 5:53p syang
 * PR 55812: add 7550 support
 *
 * Hydra_Software_Devel/254   5/5/09 4:03p pntruong
 * PR54640: Calling BVDC_ApplyChanges() directly after
 * BVDC_Compositor_Create() causes assert failure.  Defer validation of
 * compositor until display is available.
 *
 * Hydra_Software_Devel/253   4/17/09 11:56a pntruong
 * PR54064: Refactored common code for defered callback til bvn shutdown.
 * Improved readability and extensibility of dirty bits.
 *
 * Hydra_Software_Devel/252   3/30/09 2:21p hongtaoz
 * PR53037, PR53038: Implement the VEC Locking/Alignments For Memory
 * Reduction; Implement Double-Capture-Buffer Algorithm Under VEC Locking
 * Scheme
 *
 * Hydra_Software_Devel/251   3/10/09 11:10a tdo
 * PR52995: Need to set dynamics default to full range
 *
 * Hydra_Software_Devel/250   1/31/09 3:30a albertl
 * PR45798: Fixed bugs and enabled mosaic mode csc implementation.
 *
 * Hydra_Software_Devel/249   1/12/09 3:06p albertl
 * PR45798: Fixed build issues with CMP Mosaic CSC changes.
 *
 * Hydra_Software_Devel/248   1/12/09 1:38p albertl
 * PR45798: Added CMP Mosaic CSC implementation for 7420.
 *
 * Hydra_Software_Devel/247   12/18/08 8:45p pntruong
 * PR50502: Used correct display/source rate for computing the number of
 * buffers.
 *
 * Hydra_Software_Devel/246   12/10/08 2:04p albertl
 * PR45808: Updated 7420 CSC support and reworked CMP CSC versioning.
 *
 * Hydra_Software_Devel/245   12/5/08 11:15a yuxiaz
 * PR45800: Enable Dering Filter for CMP_0 Video Surfaces on 7420.
 *
 * Hydra_Software_Devel/244   12/4/08 4:46p rpan
 * PR45804: Pass source related info from compositor to VEC in one shot.
 *
 * Hydra_Software_Devel/243   12/4/08 3:46p rpan
 * PR45804: Made changes from source follow the new event driven sheme.
 *
 * Hydra_Software_Devel/242   12/3/08 4:39p rpan
 * PR45804: Initial effort for the orthogonal VEC support.
 *
 * Hydra_Software_Devel/241   11/12/08 3:33p tdo
 * PR48642: Fix compiling error
 *
 * Hydra_Software_Devel/240   11/12/08 3:12p tdo
 * PR48642: Provide clipping rect for histo luma region
 *
 * Hydra_Software_Devel/239   10/16/08 3:54p tdo
 * PR46484: Bring-up of Application Framework for 7420
 *
 * Hydra_Software_Devel/238   10/15/08 11:57a syang
 * PR 47891: en cmp after sur are configured
 *
 * Hydra_Software_Devel/237   10/9/08 5:27p syang
 * PR 46891: add _isr to the name of func used in _isr context
 *
 * Hydra_Software_Devel/236   10/7/08 4:06p syang
 * PR 41898: 1). move video sur enabling (in RUL) from cmp to win together
 * with other modules in  readerBuilRul;  2). get rid of muteReader; 3).
 * consolidate sur constant color usage impl with vnet start/shut-down
 *
 * Hydra_Software_Devel/235   10/3/08 5:02p albertl
 * PR47279: Reapply color clip settings on compositor reset caused by
 * window destroy.
 *
 * Hydra_Software_Devel/234   10/2/08 2:25p albertl
 * PR47279: Converted color clip settings from input format 16.16 to
 * hardware format of 3.8.
 *
 * Hydra_Software_Devel/233   9/24/08 7:50p darnstein
 * PR46949: DCS ON/OFF no longer "flashes." This is the 97400 check in.
 *
 * Hydra_Software_Devel/232   9/23/08 5:21p albertl
 * PR47279: Added bExtendedWhite and bExtendedBlack back to
 * BVDC_ColorClipSettings.
 *
 * Hydra_Software_Devel/231   9/17/08 4:54p albertl
 * PR46491: Fixed additional build errors.  Replaced chip version defines
 * with BVDC_P_SUPPORT_CSC_MAT_COEF_VER.
 *
 * Hydra_Software_Devel/230   9/16/08 5:38p albertl
 * PR46491: Fixed build errors.  Cleaned up 3568 A0 ifdefs.
 *
 * Hydra_Software_Devel/229   9/9/08 9:02p tdo
 * PR45802: Remove unnecessary macros and unused code
 *
 * Hydra_Software_Devel/228   9/9/08 2:03p albertl
 * PR46491:  Updated code to handle renamed CMP registers for 3548 b0.
 *
 * Hydra_Software_Devel/227   9/5/08 4:44p tdo
 * PR46484: Bringup appframework for7420
 *
 * Hydra_Software_Devel/226   9/4/08 8:30p pntruong
 * PR46502: Moving 7403 vdc dedicated branch.
 *
 * Hydra_Software_Devel/225   9/4/08 2:27p rpan
 * PR45319: Further cleaned up multi-buffering stuff from other modules.
 *
 * Hydra_Software_Devel/224   9/2/08 11:53a rpan
 * PR45319: Covert picture node flags to bit-fields to reduce structure
 * size.
 *
 * Hydra_Software_Devel/223   8/22/08 1:32p rpan
 * PR45979: Set display size and surface size at the same spot.
 *
 * Hydra_Software_Devel/222   8/22/08 10:26a rpan
 * PR45979: Remove VBI passthru support.
 *
 * Hydra_Software_Devel/221   8/8/08 3:27p yuxiaz
 * PR45484: Enable Dithering in VDC.
 *
 * Hydra_Software_Devel/220   8/1/08 2:01p rpan
 * PR45319: Downgrade the compositor display size and surface size
 * mismatch report mechanism from assertion to printing an error message.
 *
 * Hydra_Software_Devel/219   7/31/08 2:04p rpan
 * PR45139: Compositor V1 display and surface sizes are checked as well.
 *
 * Hydra_Software_Devel/218   7/31/08 1:57p rpan
 * PR45139: Compositor display size must not be greater than its surface
 * size.
 *
 * Hydra_Software_Devel/217   7/9/08 6:12p pntruong
 * PR44306: CM3D and CMP are tied together need to reprogram CM3D when CMP
 * is reset.
 *
 * Hydra_Software_Devel/216   6/7/08 8:26p albertl
 * PR39336, PR39163:  Fixed bugs in second compositor support.  Added
 * dirty bits mechanism to compositor for colorclip.
 *
 * Hydra_Software_Devel/215   5/28/08 7:06p hongtaoz
 * PR43082: MPEG source interrupt field swap logic needs to consider total
 * BVN delay for lipsync purpose;
 *
 * Hydra_Software_Devel/214   5/21/08 4:02p albertl
 * PR39163:  Fixed build error for 7325.
 *
 * Hydra_Software_Devel/213   5/21/08 3:16p albertl
 * PR39163:  Fixed compile warning on non CMP color clip capable chips.
 *
 * Hydra_Software_Devel/212   5/21/08 1:54p albertl
 * PR39163:  Updated CMP csc tables to use 10 bit offset values.
 * Corrected to use proper csc tables in conversions involving ntsc, pal,
 * and smpte 240M.  Moved colorclip from window to compositor and changed
 * BVDC_Window_Set/GetColorClip to BVDC_Compositor_Set/GetColorClip.
 *
 * Hydra_Software_Devel/211   5/12/08 2:58p jessem
 * PR 39335: Removed conditional when calling BVDC_P_Cm3d_BuildRul_isr in
 * BVDC_P_Compositor_BuildRul_isr.
 *
 * Hydra_Software_Devel/210   5/2/08 11:01a jessem
 * PR 39335: Re-enabled call to BVDC_P_Cm3d_BuildRul_isr.
 *
 * Hydra_Software_Devel/209   4/22/08 10:55a jessem
 * PR 39335: Disabled call to BVDC_P_Cm3d_BuildRul_isr temporarily.
 *
 * Hydra_Software_Devel/208   4/16/08 4:10p jessem
 * PR 39335: Changed call to BVDC_P_Window_BuildCm3dRul_isr.
 *
 * Hydra_Software_Devel/207   4/8/08 6:31p albertl
 * PR39336:  Added register remap workarounds to allow reuse of CMP0 code
 * on CMP1 for 3548 A0.
 *
 * Hydra_Software_Devel/206   4/1/08 3:06p rpan
 * PR39337: Implemented CMP OSD configuration.
 *
 * Hydra_Software_Devel/205   3/31/08 2:35p jessem
 * PR 39335: Added CM3D support.
 *
 * Hydra_Software_Devel/204   3/27/08 10:28p pntruong
 * PR39163: Fixed build errors on 7401.
 *
 * Hydra_Software_Devel/203   3/27/08 8:23p albertl
 * PR39163:  Fixed compile errors on 3563.
 *
 * Hydra_Software_Devel/202   3/27/08 5:07p albertl
 * PR39163:  Implemented non-linear xvYCC csc support for 3548.
 *
 * Hydra_Software_Devel/201   3/26/08 1:14p hongtaoz
 * PR35398, PR40895: added ulVsyncRate and removed bGameModeDelayLags in
 * the window lipsync callback structure; added BFMT_FREQ_FACTOR to unify
 * Vsync rate and pixel frequency calculation;
 *
 * Hydra_Software_Devel/200   3/19/08 5:43p tdo
 * PR39333: Implement HIST core
 *
 * Hydra_Software_Devel/199   3/4/08 2:37p rpan
 * PR40144: Moved TNT ring suppression enabling to another location.
 *
 * Hydra_Software_Devel/198   3/4/08 11:34a rpan
 * PR40144: Always enable TNT ring suppression if chip supports it.
 *
 * Hydra_Software_Devel/197   3/3/08 1:59p yuxiaz
 * PR39158: Implement dithering in various BVN components for 3548.
 *
 * Hydra_Software_Devel/196   2/21/08 4:59p pntruong
 * PR39244: Need drain debugging hooked up to HD_DVI, VDEC, and 656in.
 * Improved code readability.
 *
 * Hydra_Software_Devel/195   1/15/08 10:12a tdo
 * PR37550: Fix coverity issue
 *
 * Hydra_Software_Devel/194   11/28/07 2:01p tdo
 * PR37413: Provide new Api to set offset and size of histogram window and
 * read corresponding parameters from specific window
 *
 * Hydra_Software_Devel/193   11/19/07 3:17p hongtaoz
 * PR37388: fixed preemption issue with compositor Zorder validation;
 *
 * Hydra_Software_Devel/192   10/30/07 7:11p pntruong
 * PR34239: Allow dynamically loading of vec custom timing.
 *
 * Hydra_Software_Devel/190   9/27/07 2:01p yuxiaz
 * PR35013, PR34523: Add new CSC support on 7325.
 *
 * Hydra_Software_Devel/189   9/24/07 3:42p hongtaoz
 * PR34955: track and clean up the sync-slipped mpeg PIP source during
 * display format switch;
 *
 * Hydra_Software_Devel/188   9/21/07 3:59p hongtaoz
 * PR34955: added Progressive Segmented Frame(PsF) 1080p support; fixed
 * 1080p pass-through condition; fixed bvdc_dbg register access error;
 * fixed general progressive capture interlaced playback feeder
 * programming problem;
 *
 * Hydra_Software_Devel/187   8/15/07 8:39p pntruong
 * PR28892: Fixed window surface field id.  It should be base on the
 * current display field id.
 *
 * Hydra_Software_Devel/186   6/28/07 7:24p hongtaoz
 * PR32130, PR29189: added buffer game mode delay control; fixed picture
 * timestamp update bug when isr reorder occurs; invalid timestamp/drift
 * delay should disappear now; lipsync delay reflects the correct game
 * mode effect;
 *
 * Hydra_Software_Devel/VDC_Game_Mode/1   6/26/07 2:21p hongtaoz
 * PR32130: added window game mode buffer delay control;
 *
 * Hydra_Software_Devel/185   6/15/07 5:07p tdo
 * PR30613: Use separate control bit for CSC compute.
 *
 * Hydra_Software_Devel/184   6/13/07 3:05p tdo
 * PR30613: Incorporate the LAB contrast, brightness and saturation into
 * the CMP CSC calculation per customer requests
 *
 * Hydra_Software_Devel/183   6/12/07 11:18a tdo
 * PR30613: Move CSC calculations into bvdc_csc_priv.c
 *
 * Hydra_Software_Devel/182   6/8/07 11:31a syang
 * PR 31849: separate drain from disable for MAD, ANR and CAP
 *
 * Hydra_Software_Devel/181   5/10/07 5:24p albertl
 * PR28892:  Added constant window color support for 3563 C0.
 *
 * Hydra_Software_Devel/180   4/23/07 1:37p hongtaoz
 * PR30085: removed source slots cleanup due to the field swap since we
 * always assign single RUL to both t/b slots; turn off slots exec
 * tracking for the source slots under cleanup to prevent losing the
 * critical RUL;
 *
 * Hydra_Software_Devel/179   4/9/07 11:23a pntruong
 * PR28266: Provide API to customize DVO CSC.
 *
 * Hydra_Software_Devel/178   4/5/07 2:00p pntruong
 * PR28538, PR28539, PR28540, PR25462: Average pixel luma and customized
 * deinterlacer support.
 *
 * Hydra_Software_Devel/PR28538/1   4/3/07 9:32p xhuang
 * PR25462: Support APL setting
 *
 * Hydra_Software_Devel/177   3/1/07 11:20p albertl
 * PR24587:  Added support for compositor blenders that are not arraigned
 * in order.
 *
 * Hydra_Software_Devel/175   3/1/07 2:40p hongtaoz
 * PR28305: decrement ulMosaicAdjust count to avoid RUL always programming
 * ClearRect registers;
 *
 * Hydra_Software_Devel/174   2/22/07 4:48p pntruong
 * PR25235: Fill in the correct vertical frequency for mpeg source, that
 * is based on the vertical rate of the driven display.
 *
 * Hydra_Software_Devel/173   2/21/07 1:28p jessem
 * PR 23218, PR 27698, PR 27758: PR 27698: Added support for
 * 23.976/24/25/30/50Hz progressive sources displayed as 59.94/60/50Hz
 * interlaced.
 *
 * Hydra_Software_Devel/172   2/20/07 6:09p tdo
 * PR 27576: Cleanup PEP related code.  Init PEP context with window init.
 * Move TNT context inside of PEP context.
 *
 * Hydra_Software_Devel/171   2/8/07 7:11p albertl
 * PR26112:  Updated CMP_0_CMP_OUT_CTRL for 7400 b0.
 *
 * Hydra_Software_Devel/170   2/8/07 6:19p jessem
 * PR 23218, PR 27698: Temporarily backed out the changes. Need to revise
 * the algorithm.
 *
 * Hydra_Software_Devel/169   2/8/07 1:41p jessem
 * PR 23218: Added support for 50Hz sources displayed as 59.94/60Hz
 * interlaced.
 *
 * Hydra_Software_Devel/168   2/7/07 3:09p jessem
 * PR 27698: Added support for 23.976/24/25/30Hz sources displayed as
 * 59.94/60/50Hz interlaced.
 *
 * Hydra_Software_Devel/167   2/1/07 6:03p pntruong
 * PR25462: Need to support luma average reporting in compostior.
 *
 * Hydra_Software_Devel/166   1/30/07 10:53a pntruong
 * PR23173: Fixed linux build error for 97400.
 *
 * Hydra_Software_Devel/165   1/30/07 10:38a pntruong
 * PR23173: Enabled the CRC in the correct placed, and don't need to write
 * it every vsync.  Only when cmp is reset.
 *
 * Hydra_Software_Devel/164   1/19/07 5:36p tdo
 * PR 23212:  add the missing PEP dirty bit clear to avoid window timeout
 *
 * Hydra_Software_Devel/163   1/19/07 5:26p hongtaoz
 * PR23212: clean pep dirty bits for bypass window to avoid window
 * timeout;
 *
 * Hydra_Software_Devel/162   1/18/07 8:37p tdo
 * PR 23212:  bringup PEP algorithm.  Force rebuild PEP RUL when
 * compositor reset
 *
 * Hydra_Software_Devel/161   1/10/07 4:28p tdo
 * PR 17994, PR 26113: Add demo mode support for CMP 1 and CMP 2 for 7400
 * B0
 *
 * Hydra_Software_Devel/160   12/18/06 11:22p pntruong
 * PR22577: Merged back to mainline.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/12   12/11/06 5:21p albertl
 * PR24587:  Added 3rd GFX Feeder source.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/11   12/6/06 5:40p pntruong
 * PR25462: Need to support luma average reporting in compostior.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/10   11/16/06 1:58p hongtaoz
 * PR25668: merge in vdc support for 7403;
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/9   11/15/06 5:05p albertl
 * PR24587:  Added downsample source for 7440.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/8   11/13/06 3:33p hongtaoz
 * PR20716: separate displays/compsoitors for loops;
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/7   11/3/06 4:13p pntruong
 * PR25113: Show blue screen on HDMI without affecting component output
 * when HDCP fails.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/6   10/26/06 5:09p hongtaoz
 * PR24995: merge to 3563 branch;
 *
 * Hydra_Software_Devel/156   10/26/06 4:46p hongtaoz
 * PR24995: 720/704 transition issue;
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/5   10/25/06 10:20p pntruong
 * PR24433, PR22866, PR23032: enable the support of 704-sample 480i with
 * the latest microcode;
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/4   10/19/06 6:58p hongtaoz
 * PR23260: add dvo master mode support; add support for cmp/display
 * connection at create time; restructured display code to be scalable
 * for 7400B support;
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/3   10/3/06 5:06p hongtaoz
 * PR22561: always enable full ranged cmp YCrCb out;
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/2   9/29/06 2:16p tdo
 * PR 24608:  Add 240p mode support
 *
 * Hydra_Software_Devel/153   9/28/06 2:41p hongtaoz
 * PR18329: use ten taps filtering if available to do 422 to 444;
 *
 * Hydra_Software_Devel/152   9/27/06 8:05p albertl
 * PR23117:  Restored changes lost by clearcase in last checkin.
 *
 * Hydra_Software_Devel/151   9/26/06 7:31p albertl
 * PR23117:  Added reg writes for additional gfx windows.
 *
 * Hydra_Software_Devel/150   9/20/06 11:57a hongtaoz
 * PR21955, PR22866, PR24346: the same 7401B0 s/w workaround should still
 * applies to C0 since there is no VEC change;
 *
 * Hydra_Software_Devel/149   9/11/06 6:33p hongtaoz
 * PR22866: temporarily disable 704-sample feature for 7401B0;
 *
 * Hydra_Software_Devel/148   9/7/06 5:14p tdo
 * PR 17994, PR 23383:  Add CSC demo mode support
 *
 * Hydra_Software_Devel/147   8/29/06 11:00a hongtaoz
 * PR23246: add new QDAC support for 3563; centralize some chip-specific
 * display macroes into bvdc_common_priv.h;
 *
 * Hydra_Software_Devel/146   8/28/06 2:20p tdo
 * PR 22574:  Move TNT build isr to compositor.  TNT is part of comp_0
 *
 * Hydra_Software_Devel/145   8/18/06 4:01p albertl
 * PR23117:  Added 7440 support.
 *
 * Hydra_Software_Devel/144   8/15/06 6:15p hongtaoz
 * PR18233, PR20716: added support for mosaic mode - missing channels;
 * assume picture list's channel id are in ascending order; single
 * channel will not result in full-screen automatically;
 *
 * Hydra_Software_Devel/143   8/14/06 7:55p pntruong
 * PR23177: RDC module bringup.  More cleanup of nested ifdefs.
 *
 * Hydra_Software_Devel/142   8/7/06 5:26p pntruong
 * PR22577: Initial bringup of VDC.
 *
 * Hydra_Software_Devel/141   7/17/06 1:00p hongtaoz
 * PR22713: increased the synlock transfer semaphore count; display _isr
 * cleans up source slots only when the semaphore count is 0;
 *
 * Hydra_Software_Devel/140   7/17/06 12:02p hongtaoz
 * PR22713: decrement ulTransferLock semaphore count at the receiving
 * display isr as a bakcup in case source/display _isr calls disorder and
 * the semaphore is stuck forever;
 *
 * Hydra_Software_Devel/139   6/16/06 5:46p hongtaoz
 * PR21804: disable mosaic mode if no picture list is passed from decoder
 * for smoother transition;
 *
 * Hydra_Software_Devel/138   6/14/06 6:53p hongtaoz
 * PR20716, PR21804: implement modified version of mosaic mode;
 *
 * Hydra_Software_Devel/137   6/14/06 1:34p syang
 * PR 21689: add support for 7118
 *
 * Hydra_Software_Devel/136   5/26/06 3:37p pntruong
 * PR20642: Refactored handling of hList.
 *
 * Hydra_Software_Devel/135   4/20/06 5:50p pntruong
 * PR20750: Please update bavc.h to reflect presence of third VEC.
 *
 * Hydra_Software_Devel/134   4/18/06 11:47a tdo
 * PR 21012: remove compiler warnings
 *
 * Hydra_Software_Devel/133   4/4/06 1:48p hongtaoz
 * PR20403: add triple displays for 7400;
 *
 * Hydra_Software_Devel/132   3/30/06 9:22p hongtaoz
 * PR20520, PR17512: checked RUL execution status when CMP's color space
 * is dirty; count twice of execution status in case it's interlaced
 * slots;
 *
 * Hydra_Software_Devel/131   2/2/06 12:12p hongtaoz
 * PR19082: bring up 7400;
 *
 * Hydra_Software_Devel/130   1/17/06 4:17p hongtaoz
 * PR19082: first compiled for 7400;
 *
 * Hydra_Software_Devel/129   1/13/06 10:34a hongtaoz
 * PR16061: always call PEP isr for cmp0_v0 to clear dirty bits promptly
 * to avoid channel change failure;
 *
 * Hydra_Software_Devel/128   1/12/06 10:36a yuxiaz
 * PR18476: Added new functions BVDC_Window_SetLumaKey and
 * BVDC_Window_GetLumaKey for luma key support.
 *
 * Hydra_Software_Devel/127   1/12/06 9:57a tdo
 * PR 16061: Implementing dynamic contrast stretch in the PEP
 *
 * Hydra_Software_Devel/126   12/21/05 1:33p syang
 * PR 15191: upadated gfd color matrix according to main video win's color
 * matrix
 *
 * Hydra_Software_Devel/125   11/29/05 4:56p hongtaoz
 * PR16298, PR16781, PR17804, PR18025, PR18232: implemented 704-sample
 * NTSC analog output; added two more Macrovision control modes; added
 * RGB SCART mode (G channel w/o sync) DAC config for Macrovision
 * validation test;
 *
 * Hydra_Software_Devel/124   11/3/05 4:58p hongtaoz
 * PR17200: collapse window z-order to real blender id; added number of
 * vwin check;
 *
 * Hydra_Software_Devel/123   10/19/05 7:40p hongtaoz
 * PR17512: set color flags to reload matrices when cmp is reset;
 *
 * Hydra_Software_Devel/122   10/19/05 3:17p hongtaoz
 * PR17512: added separate flag to build cmp color matrix RUL; window csc
 * dirty bit initiates redetermine window color matrix logic;
 *
 * Hydra_Software_Devel/121   10/19/05 12:03p hongtaoz
 * PR17512: window color dirty bit should only be reset when window is
 * enabled to adapt to new window shutdown process;
 *
 * Hydra_Software_Devel/120   9/14/05 6:12p pntruong
 * PR17019: Debug messages report continually dirty surface.
 *
 * Hydra_Software_Devel/119   8/29/05 3:29p hongtaoz
 * PR16812, PR15495: build both T/B RULs for sync-slipped playback side
 * muting RUL and not touch window playback side before the new vnet RUL
 * is executed; restore window's current shutdown state without affecting
 * user setting;
 *
 * Hydra_Software_Devel/118   8/18/05 1:13p pntruong
 * PR15757, PR16391, PR16411, PR12519, PR14791, PR15535, PR15206, PR15778:
 * Improved bandwidth for cropping/scaler/capture/playback.   Unified
 * window shutdown sequence for destroy, reconfigure mad/scaler, and/or
 * reconfigure result of source changes.  And miscellances fixes from
 * above PRs.
 *
 * Hydra_Software_Devel/XVDPhase1/XVDPhase1_merge/9   8/17/05 2:37p pntruong
 * PR12519: Take in additional support for 7401.
 *
 * Hydra_Software_Devel/XVDPhase1/XVDPhase1_merge/8   8/17/05 12:43p pntruong
 * PR12519: Take in additional support for 7401.
 *
 * Hydra_Software_Devel/XVDPhase1/XVDPhase1_merge/7   8/16/05 2:01p pntruong
 * PR12519: Take in additional fixes from mainline.
 *
 * Hydra_Software_Devel/XVDPhase1/XVDPhase1_merge/6   8/15/05 5:34p hongtaoz
 * PR16142: merged 656in vbi pass-through support from main-line;
 *
 * Hydra_Software_Devel/XVDPhase1/XVDPhase1_merge/5   8/12/05 5:37p pntruong
 * PR12519: Improved error recovery, by draining vnet swtiches after
 * destroy for vnet reconfiguration, destroy, source reconfiguration, or
 * short fileds introduced by input block.
 *
 * Hydra_Software_Devel/XVDPhase1/XVDPhase1_merge/4   7/21/05 6:25p pntruong
 * PR12519: Make reset code more consistent with other modules.
 *
 * Hydra_Software_Devel/XVDPhase1/XVDPhase1_merge/3   7/21/05 6:22p pntruong
 * PR12519: Make reset code more consistent with other modules.
 *
 * Hydra_Software_Devel/XVDPhase1/XVDPhase1_merge/2   7/20/05 4:13p pntruong
 * PR15186: Removed unnecessary display frame rate change during channel
 * change.
 *
 * Hydra_Software_Devel/XVDPhase1/XVDPhase1_merge/1   7/14/05 7:57p pntruong
 * PR12519: Take in changes from mainline before merge back.
 *
 * Hydra_Software_Devel/XVDPhase1/5   5/11/05 11:22a pntruong
 * PR15275: Added clipping support for HD_DVI.
 *
 * Hydra_Software_Devel/XVDPhase1/4   5/6/05 4:00p pntruong
 * PR13121, PR12519: Added support for xvd hd/sd simul mode.
 *
 * Hydra_Software_Devel/XVDPhase1/3   5/3/05 9:06p pntruong
 * PR15084: Still need to build rul for shutdown state.
 *
 * Hydra_Software_Devel/XVDPhase1/2   5/3/05 8:33p pntruong
 * PR15084: Additional work on hddvi input format change.
 *
 * Hydra_Software_Devel/XVDPhase1/1   5/2/05 4:54p pntruong
 * PR12519, PR13121, PR15048, PR15084, PR15100: Dynamically re-allocate
 * capture in bvn path to reduce memory consumption and bandwith.
 *
 * Hydra_Software_Devel/2   5/1/05 11:40p pntruong
 * Need to handle when dirty bit is raise.
 *
 * Hydra_Software_Devel/1   5/1/05 4:51p pntruong
 * XVDPhase1 branch off.  Temp storage so we can keep track of changes.
 *
 * Hydra_Software_Devel/106   4/15/05 8:09p hongtaoz
 * PR14691: swap the field triggering to fix thefield inversion for 2-
 * buffer mpeg windows; delay the forced triggering when triggering base
 * of a mpeg src transfers to a different vec even it's a slipped PIP
 * window;
 *
 * Hydra_Software_Devel/105   4/8/05 3:38p pntruong
 * PR14018, PR14011, PR14450, PR14648:  Vec to generate fake trigger when
 * source is pulled or not configured correctly or any condition that
 * cause source to lose trigger.  This vec's fake trigger enable the
 * applychanges to go thru, and does not result in timeout.
 *
 * Hydra_Software_Devel/104   3/18/05 6:30p pntruong
 * PR14494: Basic vdc up and running.  Background and passed output format
 * switch test.
 *
 * Hydra_Software_Devel/103   3/11/05 9:21a pntruong
 * PR14112, PR14046: Fixed hd/sd channel change for progressive mode.
 *
 * Hydra_Software_Devel/102   3/9/05 4:50p pntruong
 * PR14112, PR14046: Support out of order destroys for "cloned" windows in
 * VDC. and While in mirror mode with PIP, VDC times out when toggling
 * between analog and digital channel.
 *
 * Hydra_Software_Devel/101   2/28/05 10:55a pntruong
 * PR13575: Window destroy timeout at channel change.
 *
 ***************************************************************************/
#include "bstd.h"
#include "bkni.h"
#include "bvdc.h"
#include "brdc.h"
#include "bvdc_priv.h"
#include "bvdc_compositor_priv.h"
#include "bvdc_window_priv.h"
#include "bvdc_source_priv.h"
#include "bvdc_display_priv.h"
#include "bvdc_displayfmt_priv.h"
#include "bchp_bmisc.h"
#include "bvdc_pep_priv.h"
#include "bchp_mmisc.h"

#if BVDC_P_SUPPORT_CM3D
#include "bvdc_cm3d_priv.h"
#endif

#if BVDC_P_SUPPORT_OSCL
#include "bvdc_oscl_priv.h"
#endif

#if (BVDC_P_SUPPORT_NEW_SW_INIT) /* beyond 7420 */
#define BVDC_P_MAKE_CMP(pCmp, id)                                     \
{                                                                     \
	(pCmp)->ulCoreResetAddr = BCHP_BMISC_SW_INIT;                     \
	(pCmp)->ulCoreResetMask = BCHP_BMISC_SW_INIT_CMP_##id##_MASK;     \
}
#else
#define BVDC_P_MAKE_CMP(pCmp, id)                                     \
{                                                                     \
	(pCmp)->ulCoreResetAddr = BCHP_BMISC_SOFT_RESET;                  \
	(pCmp)->ulCoreResetMask = BCHP_BMISC_SOFT_RESET_CMP_##id##_MASK;  \
}
#endif

/* Dither settings for COMP */
#define BVDC_P_DITHER_CMP_LFSR_VALUE                 (0xE0F82)
#define BVDC_P_DITHER_CMP_V0_LFSR_CTRL_T0            (0x6)
#define BVDC_P_DITHER_CMP_V0_LFSR_CTRL_T1            (0x3)
#define BVDC_P_DITHER_CMP_V0_LFSR_CTRL_T2            (0x6)
#define BVDC_P_DITHER_CMP_V1_LFSR_CTRL_T0            (0x4)
#define BVDC_P_DITHER_CMP_V1_LFSR_CTRL_T1            (0x3)
#define BVDC_P_DITHER_CMP_V1_LFSR_CTRL_T2            (0x6)

BDBG_MODULE(BVDC_CMP);
BDBG_FILE_MODULE(BVDC_CMP_SIZE);
BDBG_OBJECT_ID(BVDC_CMP);

/* INDEX: by compositor id. */
static const BVDC_P_Compositor_Features s_aHydraCompositorFeatures
	[BVDC_P_MAX_COMPOSITOR_COUNT] =
{
	/* Compositor 0 (Primary) */
	{BVDC_P_CMP_0_MAX_VIDEO_WINDOW_COUNT,
	 BVDC_P_CMP_0_MAX_GFX_WINDOW_COUNT,
	 BVDC_P_CMP_0_MAX_WINDOW_COUNT},

	/* Compositor 1 (Secondary) */
	{BVDC_P_CMP_1_MAX_VIDEO_WINDOW_COUNT,
	 BVDC_P_CMP_1_MAX_GFX_WINDOW_COUNT,
	 BVDC_P_CMP_1_MAX_WINDOW_COUNT},

	/* Compositor Bypass */
	{BVDC_P_CMP_2_MAX_VIDEO_WINDOW_COUNT,
	 BVDC_P_CMP_2_MAX_GFX_WINDOW_COUNT,
	 BVDC_P_CMP_2_MAX_WINDOW_COUNT},

	/* VICE */
	{BVDC_P_CMP_3_MAX_VIDEO_WINDOW_COUNT,
	 BVDC_P_CMP_3_MAX_GFX_WINDOW_COUNT,
	 BVDC_P_CMP_3_MAX_WINDOW_COUNT},

	/* VICE */
	{BVDC_P_CMP_4_MAX_VIDEO_WINDOW_COUNT,
	 BVDC_P_CMP_4_MAX_GFX_WINDOW_COUNT,
	 BVDC_P_CMP_4_MAX_WINDOW_COUNT},

	/* VICE */
	{BVDC_P_CMP_5_MAX_VIDEO_WINDOW_COUNT,
	 BVDC_P_CMP_5_MAX_GFX_WINDOW_COUNT,
	 BVDC_P_CMP_5_MAX_WINDOW_COUNT},

	/* VICE */
	{BVDC_P_CMP_6_MAX_VIDEO_WINDOW_COUNT,
	 BVDC_P_CMP_6_MAX_GFX_WINDOW_COUNT,
	 BVDC_P_CMP_6_MAX_WINDOW_COUNT}
};


#if (BVDC_P_SUPPORT_CSC_MAT_COEF_VER == 2)
typedef struct BVDC_P_Compositor_Cmp1RegMapInfo
{
	uint32_t ulCmp0RegAddr;
	uint32_t ulCmp1RegAddr;
} BVDC_P_Compositor_Cmp1RegMapInfo;

static const BVDC_P_Compositor_Cmp1RegMapInfo s_aulCmp1RegMap[] =
{
	{BCHP_CMP_0_V0_MC_DITHER_CTRL,        BCHP_CMP_1_V0_DITHER_CTRL},
	{BCHP_CMP_0_V0_MC_DITHER_LFSR_INIT,   BCHP_CMP_1_V0_DITHER_LFSR_INIT},
	{BCHP_CMP_0_V0_MC_DITHER_LFSR_CTRL,   BCHP_CMP_1_V0_DITHER_LFSR_CTRL},
	{BCHP_CMP_0_V0_RECT_SIZEi_ARRAY_BASE, BCHP_CMP_1_V0_RECT_SIZEi_ARRAY_BASE},
	{BCHP_CMP_0_V0_RECT_COLOR,            BCHP_CMP_1_V0_RECT_COLOR},
	{BCHP_CMP_0_CMP_LUMA_AVG_WIN_OFFSET,  BCHP_CMP_1_CMP_LUMA_AVG_WIN_OFFSET},
	{BCHP_CMP_0_LUMA_AVG_CTRL,            BCHP_CMP_1_LUMA_AVG_CTRL},
	{BCHP_CMP_0_CMP_OUT_CTRL,             BCHP_CMP_1_CMP_OUT_CTRL},
	{BCHP_CMP_0_SCRATCH_REGISTER,         BCHP_CMP_1_SCRATCH_REGISTER},
	{BCHP_CMP_0_G0_SURFACE_SIZE,          BCHP_CMP_1_G0_SURFACE_SIZE},
	{BCHP_CMP_0_V0_CB_KEYING,             BCHP_CMP_1_V0_CB_KEYING},
	{BCHP_CMP_0_V0_BVB_IN_STATUS_CLEAR,   BCHP_CMP_1_V0_BVB_IN_STATUS_CLEAR},
};

#define BVDC_P_CMP_CMP1REGMAP_SIZE (uint32_t)(sizeof(s_aulCmp1RegMap)/sizeof(BVDC_P_Compositor_Cmp1RegMapInfo))


/***************************************************************************
 * {private}
 *
 */
uint32_t BVDC_P_Compositor_GetCmpRegAddr_isr(BVDC_CompositorId eId, uint32_t ulRegAddr)
{
	uint32_t i = 0;

	if (eId != BVDC_CompositorId_eCompositor1)
	{
		return ulRegAddr;
	}

	for (i = 0; i < BVDC_P_CMP_CMP1REGMAP_SIZE; i++)
	{
		if (ulRegAddr >= s_aulCmp1RegMap[i].ulCmp0RegAddr)
		{
			return ((ulRegAddr - s_aulCmp1RegMap[i].ulCmp0RegAddr) + s_aulCmp1RegMap[i].ulCmp1RegAddr) -
				   BVDC_P_CMP_GET_REG_OFFSET(BVDC_CompositorId_eCompositor1);
		}
	}

	return ulRegAddr;
}
#endif


/***************************************************************************
 * {private}
 *
 */
BERR_Code BVDC_P_Compositor_Create
	( BVDC_Handle                      hVdc,
	  BVDC_Compositor_Handle          *phCompositor,
	  BVDC_CompositorId                eCompositorId )
{
	uint32_t i;
	BVDC_P_CompositorContext *pCompositor;
	BERR_Code eStatus = BERR_SUCCESS;

	BDBG_ENTER(BVDC_P_Compositor_Create);
	BDBG_ASSERT(phCompositor);
	BDBG_OBJECT_ASSERT(hVdc, BVDC_VDC);

	/* (1) Alloc the context. */
	pCompositor = (BVDC_P_CompositorContext*)
		(BKNI_Malloc(sizeof(BVDC_P_CompositorContext)));
	if(!pCompositor)
	{
		return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
	}

	/* Clear out the context and set defaults. */
	BKNI_Memset((void*)pCompositor, 0x0, sizeof(BVDC_P_CompositorContext));
	BDBG_OBJECT_SET(pCompositor, BVDC_CMP);

	/* Initialize non-changing states.  These are not to be changed by runtime. */
	pCompositor->eId          = eCompositorId;
	pCompositor->pFeatures    = &s_aHydraCompositorFeatures[eCompositorId];
	pCompositor->ulRegOffset  = BVDC_P_CMP_GET_REG_OFFSET(eCompositorId);
	pCompositor->hVdc         = hVdc;
	pCompositor->bIsBypass    = (
		(hVdc->pFeatures->bCmpBIsBypass) &&
		(BVDC_CompositorId_eCompositor2 == pCompositor->eId));

	/* all windows should have been disconnected. */
	for(i = 0; i < BVDC_P_MAX_WINDOW_COUNT; i++)
	{
		pCompositor->ahWindow[i] = NULL;
	}

	/* (2) Create RDC List */
	for(i = 0; i < BVDC_P_CMP_MAX_LIST_COUNT; i++)
	{
		BRDC_List_Create(hVdc->hRdc, BVDC_P_MAX_ENTRY_PER_RUL,
			&pCompositor->ahList[i]);
	}

	/* (3) Create RDC Slot */
	for(i = 0; i < BVDC_P_CMP_MAX_SLOT_COUNT; i++)
	{
		BRDC_SlotId eSlotId;
		BRDC_Slot_Create(hVdc->hRdc, &pCompositor->ahSlot[i]);
		BRDC_Slot_GetId(pCompositor->ahSlot[i], &eSlotId);
		BDBG_MSG(("Compositor[%d] uses slot[%d]", pCompositor->eId, eSlotId));
	}

	/* (4) Create Slot interrupt callback */
	for(i = 0; i < BVDC_P_CMP_MAX_SLOT_COUNT; i++)
	{
		eStatus = BINT_CreateCallback(&pCompositor->ahCallback[i], hVdc->hInterrupt,
			BRDC_Slot_GetIntId(pCompositor->ahSlot[i]),
			BVDC_P_CompositorDisplay_isr, (void*)pCompositor, i);
		if(BERR_SUCCESS != eStatus)
		{
			return BERR_TRACE(eStatus);
		}
	}

	/* Child windows for given compositor. */
	if(BVDC_CompositorId_eCompositor0 == pCompositor->eId)
	{
		BVDC_P_Window_Create(pCompositor, &pCompositor->ahWindow[BVDC_P_WindowId_eComp0_V0],
			BVDC_P_WindowId_eComp0_V0);
		BVDC_P_Window_Create(pCompositor, &pCompositor->ahWindow[BVDC_P_WindowId_eComp0_V1],
			BVDC_P_WindowId_eComp0_V1);
		BVDC_P_Window_Create(pCompositor, &pCompositor->ahWindow[BVDC_P_WindowId_eComp0_G0],
			BVDC_P_WindowId_eComp0_G0);
		BVDC_P_Window_Create(pCompositor, &pCompositor->ahWindow[BVDC_P_WindowId_eComp0_G1],
			BVDC_P_WindowId_eComp0_G1);
		BVDC_P_Window_Create(pCompositor, &pCompositor->ahWindow[BVDC_P_WindowId_eComp0_G2],
			BVDC_P_WindowId_eComp0_G2);

		/* reset address and mask and various init. */
		BVDC_P_MAKE_CMP(pCompositor, 0);
	}
#if (BVDC_P_CMP_1_MAX_WINDOW_COUNT)
	else if(BVDC_CompositorId_eCompositor1 == pCompositor->eId)
	{
		BVDC_P_Window_Create(pCompositor, &pCompositor->ahWindow[BVDC_P_WindowId_eComp1_V0],
			BVDC_P_WindowId_eComp1_V0);
		BVDC_P_Window_Create(pCompositor, &pCompositor->ahWindow[BVDC_P_WindowId_eComp1_V1],
			BVDC_P_WindowId_eComp1_V1);
		BVDC_P_Window_Create(pCompositor, &pCompositor->ahWindow[BVDC_P_WindowId_eComp1_G0],
			BVDC_P_WindowId_eComp1_G0);

		/* reset address and mask and various init. */
		BVDC_P_MAKE_CMP(pCompositor, 1);
	}
#endif
#if (BVDC_P_CMP_2_MAX_WINDOW_COUNT)
	else if(BVDC_CompositorId_eCompositor2 == pCompositor->eId)
	{
		BVDC_P_Window_Create(pCompositor, &pCompositor->ahWindow[BVDC_P_WindowId_eComp2_V0],
			BVDC_P_WindowId_eComp2_V0);

		if(!pCompositor->bIsBypass)
		{
			BVDC_P_Window_Create(pCompositor, &pCompositor->ahWindow[BVDC_P_WindowId_eComp2_G0],
				BVDC_P_WindowId_eComp2_G0);
		}

		/* reset address and mask and various init. */
		BVDC_P_MAKE_CMP(pCompositor, 2);
	}
#endif
#if (BVDC_P_CMP_3_MAX_WINDOW_COUNT)
	else if(BVDC_CompositorId_eCompositor3 == pCompositor->eId)
	{
		BVDC_P_Window_Create(pCompositor, &pCompositor->ahWindow[BVDC_P_WindowId_eComp3_V0],
			BVDC_P_WindowId_eComp3_V0);
		BVDC_P_Window_Create(pCompositor, &pCompositor->ahWindow[BVDC_P_WindowId_eComp3_G0],
			BVDC_P_WindowId_eComp3_G0);

		/* reset address and mask and various init. */
		BVDC_P_MAKE_CMP(pCompositor, 3);
	}
#endif
#if (BVDC_P_CMP_4_MAX_WINDOW_COUNT)
	else if(BVDC_CompositorId_eCompositor4 == pCompositor->eId)
	{
		BVDC_P_Window_Create(pCompositor, &pCompositor->ahWindow[BVDC_P_WindowId_eComp4_V0],
			BVDC_P_WindowId_eComp4_V0);
		BVDC_P_Window_Create(pCompositor, &pCompositor->ahWindow[BVDC_P_WindowId_eComp4_G0],
			BVDC_P_WindowId_eComp4_G0);

		/* reset address and mask and various init. */
		BVDC_P_MAKE_CMP(pCompositor, 4);
	}
#endif
#if (BVDC_P_CMP_5_MAX_WINDOW_COUNT)
	else if(BVDC_CompositorId_eCompositor5 == pCompositor->eId)
	{
		BVDC_P_Window_Create(pCompositor, &pCompositor->ahWindow[BVDC_P_WindowId_eComp5_V0],
			BVDC_P_WindowId_eComp5_V0);
		BVDC_P_Window_Create(pCompositor, &pCompositor->ahWindow[BVDC_P_WindowId_eComp5_G0],
			BVDC_P_WindowId_eComp5_G0);

		/* reset address and mask and various init. */
		BVDC_P_MAKE_CMP(pCompositor, 5);
	}
#endif

#if (BVDC_P_CMP_6_MAX_WINDOW_COUNT)
	else if(BVDC_CompositorId_eCompositor6 == pCompositor->eId)
	{
		BVDC_P_Window_Create(pCompositor, &pCompositor->ahWindow[BVDC_P_WindowId_eComp6_V0],
			BVDC_P_WindowId_eComp6_V0);
		BVDC_P_Window_Create(pCompositor, &pCompositor->ahWindow[BVDC_P_WindowId_eComp6_G0],
			BVDC_P_WindowId_eComp6_G0);

		/* reset address and mask and various init. */
		BVDC_P_MAKE_CMP(pCompositor, 6);
	}
#endif

	/* (5) Added this compositor to hVdc */
	hVdc->ahCompositor[eCompositorId] = (BVDC_Compositor_Handle)pCompositor;

	/* All done. now return the new fresh context to user. */
	*phCompositor = (BVDC_Compositor_Handle)pCompositor;

	BDBG_LEAVE(BVDC_P_Compositor_Create);
	return BERR_SUCCESS;
}


/***************************************************************************
 * {private}
 *
 */
void BVDC_P_Compositor_Destroy
	( BVDC_Compositor_Handle           hCompositor )
{
	uint32_t i;

	BDBG_ENTER(BVDC_P_Compositor_Destroy);
	BDBG_OBJECT_ASSERT(hCompositor, BVDC_CMP);
	BDBG_OBJECT_ASSERT(hCompositor->hVdc, BVDC_VDC);

	/* At this point application should have disable all the
	 * callbacks &slots */

	/* [5] Removed this compositor from hVdc */
	hCompositor->hVdc->ahCompositor[hCompositor->eId] = NULL;

	/* Child windows for this compositor. */
	if(BVDC_CompositorId_eCompositor0 == hCompositor->eId)
	{
		BVDC_P_Window_Destroy(hCompositor->ahWindow[BVDC_P_WindowId_eComp0_V0]);
		BVDC_P_Window_Destroy(hCompositor->ahWindow[BVDC_P_WindowId_eComp0_V1]);
		BVDC_P_Window_Destroy(hCompositor->ahWindow[BVDC_P_WindowId_eComp0_G0]);
		BVDC_P_Window_Destroy(hCompositor->ahWindow[BVDC_P_WindowId_eComp0_G1]);
		BVDC_P_Window_Destroy(hCompositor->ahWindow[BVDC_P_WindowId_eComp0_G2]);
	}
	else if(BVDC_CompositorId_eCompositor1 == hCompositor->eId)
	{
		BVDC_P_Window_Destroy(hCompositor->ahWindow[BVDC_P_WindowId_eComp1_V0]);
		BVDC_P_Window_Destroy(hCompositor->ahWindow[BVDC_P_WindowId_eComp1_V1]);
		BVDC_P_Window_Destroy(hCompositor->ahWindow[BVDC_P_WindowId_eComp1_G0]);
	}
	else if(BVDC_CompositorId_eCompositor2 == hCompositor->eId)
	{
		BVDC_P_Window_Destroy(hCompositor->ahWindow[BVDC_P_WindowId_eComp2_V0]);
		if(!hCompositor->hVdc->pFeatures->bCmpBIsBypass)
		{
			BVDC_P_Window_Destroy(hCompositor->ahWindow[BVDC_P_WindowId_eComp2_G0]);
		}
	}
	else if(BVDC_CompositorId_eCompositor3 == hCompositor->eId)
	{
		BVDC_P_Window_Destroy(hCompositor->ahWindow[BVDC_P_WindowId_eComp3_V0]);
		BVDC_P_Window_Destroy(hCompositor->ahWindow[BVDC_P_WindowId_eComp3_G0]);
	}
	else if(BVDC_CompositorId_eCompositor4 == hCompositor->eId)
	{
		BVDC_P_Window_Destroy(hCompositor->ahWindow[BVDC_P_WindowId_eComp4_V0]);
		BVDC_P_Window_Destroy(hCompositor->ahWindow[BVDC_P_WindowId_eComp4_G0]);
	}
	else if(BVDC_CompositorId_eCompositor5 == hCompositor->eId)
	{
		BVDC_P_Window_Destroy(hCompositor->ahWindow[BVDC_P_WindowId_eComp5_V0]);
		BVDC_P_Window_Destroy(hCompositor->ahWindow[BVDC_P_WindowId_eComp5_G0]);
	}
	else if(BVDC_CompositorId_eCompositor6 == hCompositor->eId)
	{
		BVDC_P_Window_Destroy(hCompositor->ahWindow[BVDC_P_WindowId_eComp6_V0]);
		BVDC_P_Window_Destroy(hCompositor->ahWindow[BVDC_P_WindowId_eComp6_G0]);
	}

	/* [4] Create Slot interrupt callback */
	for(i = 0; i < BVDC_P_CMP_MAX_SLOT_COUNT; i++)
	{
		BINT_DestroyCallback(hCompositor->ahCallback[i]);
	}

	/* [3] Destroy RDC Slot */
	for(i = 0; i < BVDC_P_CMP_MAX_SLOT_COUNT; i++)
	{
		BRDC_Slot_Destroy(hCompositor->ahSlot[i]);
	}

	/* [2] Destroy RDC List */
	for(i = 0; i < BVDC_P_CMP_MAX_LIST_COUNT; i++)
	{
		BRDC_List_Destroy(hCompositor->ahList[i]);
	}

	BDBG_OBJECT_DESTROY(hCompositor, BVDC_CMP);
	/* [1] Release context in system memory */
	BKNI_Free((void*)hCompositor);

	BDBG_LEAVE(BVDC_P_Compositor_Destroy);
	return;
}


/***************************************************************************
 * {private}
 *
 * By application calling BVDC_Compositor_Create().
 */
void BVDC_P_Compositor_Init
	( BVDC_Compositor_Handle           hCompositor )
{
	uint32_t i;
	BVDC_P_Compositor_Info *pNewInfo;
	BVDC_P_Compositor_Info *pCurInfo;
	BVDC_P_Compositor_DirtyBits *pNewDirty;
	uint32_t  ulLfsrCtrlT0, ulLfsrCtrlT1, ulLfsrCtrlT2, ulLfsrValue;

	BDBG_ENTER(BVDC_P_Compositor_Init);
	BDBG_OBJECT_ASSERT(hCompositor, BVDC_CMP);
	BDBG_OBJECT_ASSERT(hCompositor->hVdc, BVDC_VDC);

	/* Init to default state. */
	hCompositor->bInitial            = true;
	hCompositor->eState              = BVDC_P_State_eInactive;
	hCompositor->ulActiveVideoWindow = 0;
	hCompositor->ulActiveGfxWindow   = 0;
	hCompositor->bUserAppliedChanges = false;
	hCompositor->hSyncLockWin        = NULL;
	hCompositor->hSyncLockSrc        = NULL;
	hCompositor->hForceTrigPipSrc    = NULL;
	hCompositor->ulVdecPhase         = 0;
	hCompositor->b240pMode           = false;
	hCompositor->ePhaseAdjustMode    = BVDC_PhaseAdjustMode_eDisable;
	hCompositor->ulSlip2Lock         = 0;
	hCompositor->hDownSample         = NULL;
	hCompositor->ulDelayCnt          = 0;

	/* used for NRT mode transcode: default true to freeze STC and not get encoded */
	hCompositor->bIgnorePicture = true;
	hCompositor->bStallStc      = true;
	/* used for xcode channel change */
	hCompositor->bChannelChange = true;
	hCompositor->bGfxChannelChange = true;


	/* reinitialized ruls. */
	for(i = 0; i < BVDC_P_CMP_MAX_LIST_COUNT; i++)
	{
		BVDC_P_BuildNoOpsRul_isr(hCompositor->ahList[i]);
	}

	/* Assign fresh new no-op list. */
	for(i = 0; i < BVDC_P_CMP_MAX_SLOT_COUNT; i++)
	{
		/* Initialized rul indexing. */
		hCompositor->aulRulIdx[i] = 0;
		BRDC_Slot_SetList_isr(hCompositor->ahSlot[i],
			hCompositor->ahList[i * BVDC_P_MAX_MULTI_RUL_BUFFER_COUNT]);
	}

	/* Compositor's output framerate from selected master window */
	hCompositor->eSrcFRateCode = hCompositor->hVdc->stSettings.eDisplayFrameRate;
	hCompositor->bFullRate     = BVDC_P_IS_FULL_FRAMRATE(hCompositor->eSrcFRateCode);

	/* Dither */
	ulLfsrCtrlT0 = BVDC_P_DITHER_CMP_V0_LFSR_CTRL_T0;
	ulLfsrCtrlT1 = BVDC_P_DITHER_CMP_V0_LFSR_CTRL_T1;
	ulLfsrCtrlT2 = BVDC_P_DITHER_CMP_V0_LFSR_CTRL_T2;
	ulLfsrValue  = BVDC_P_DITHER_CMP_LFSR_VALUE;
	BVDC_P_Dither_Init(&hCompositor->stV0Dither,
		ulLfsrCtrlT0, ulLfsrCtrlT1, ulLfsrCtrlT2, ulLfsrValue);

	/* Clear out shadow registers. */
	BKNI_Memset((void*)hCompositor->aulRegs, 0x0, sizeof(hCompositor->aulRegs));

	/* Default compositor settings. */
	BVDC_P_CMP_GET_REG_DATA(CMP_0_CANVAS_CTRL) &= ~(
		BCHP_MASK(CMP_0_CANVAS_CTRL, ENABLE) |
		BCHP_MASK(CMP_0_CANVAS_CTRL, ENABLE_CTRL));
	BVDC_P_CMP_GET_REG_DATA(CMP_0_CANVAS_CTRL) |=  (
		BCHP_FIELD_ENUM(CMP_0_CANVAS_CTRL, ENABLE, ENABLE) |
		BCHP_FIELD_ENUM(CMP_0_CANVAS_CTRL, ENABLE_CTRL, STOP_ON_FIELD_COMPLETION));

	/* Default blender settings, no blending */
	BVDC_P_CMP_GET_REG_DATA(CMP_0_BLEND_0_CTRL) &= ~(
		BCHP_MASK(CMP_0_BLEND_0_CTRL, BLEND_SOURCE));
	BVDC_P_CMP_GET_REG_DATA(CMP_0_BLEND_0_CTRL) |=  (
		BCHP_FIELD_ENUM(CMP_0_BLEND_0_CTRL, BLEND_SOURCE, BACKGROUND_BYPASS));

	BVDC_P_CMP_GET_REG_DATA(CMP_0_BLEND_1_CTRL) &= ~(
		BCHP_MASK(CMP_0_BLEND_1_CTRL, BLEND_SOURCE));
	BVDC_P_CMP_GET_REG_DATA(CMP_0_BLEND_1_CTRL) |=  (
		BCHP_FIELD_ENUM(CMP_0_BLEND_1_CTRL, BLEND_SOURCE, BACKGROUND_BYPASS));

#if (BVDC_P_SUPPORT_DITHER)
	/* CMP_0_V0_MC_DITHER_CTRL */
	BVDC_P_CMP_GET_REG_DATA(CMP_0_V0_MC_DITHER_CTRL) &= ~(
		BCHP_MASK(CMP_0_V0_MC_DITHER_CTRL, MODE      ) |
		BCHP_MASK(CMP_0_V0_MC_DITHER_CTRL, OFFSET_CH2) |
		BCHP_MASK(CMP_0_V0_MC_DITHER_CTRL, SCALE_CH2 ) |
		BCHP_MASK(CMP_0_V0_MC_DITHER_CTRL, OFFSET_CH1) |
		BCHP_MASK(CMP_0_V0_MC_DITHER_CTRL, SCALE_CH1 ) |
		BCHP_MASK(CMP_0_V0_MC_DITHER_CTRL, OFFSET_CH0) |
		BCHP_MASK(CMP_0_V0_MC_DITHER_CTRL, SCALE_CH0 ));
	BVDC_P_CMP_GET_REG_DATA(CMP_0_V0_MC_DITHER_CTRL) |=  (
		BCHP_FIELD_DATA(CMP_0_V0_MC_DITHER_CTRL, MODE,       hCompositor->stV0Dither.ulMode     ) |
		BCHP_FIELD_DATA(CMP_0_V0_MC_DITHER_CTRL, OFFSET_CH2, hCompositor->stV0Dither.ulCh1Offset) |
		BCHP_FIELD_DATA(CMP_0_V0_MC_DITHER_CTRL, SCALE_CH2,  hCompositor->stV0Dither.ulCh1Scale ) |
		BCHP_FIELD_DATA(CMP_0_V0_MC_DITHER_CTRL, OFFSET_CH1, hCompositor->stV0Dither.ulCh1Offset) |
		BCHP_FIELD_DATA(CMP_0_V0_MC_DITHER_CTRL, SCALE_CH1,  hCompositor->stV0Dither.ulCh1Scale ) |
		BCHP_FIELD_DATA(CMP_0_V0_MC_DITHER_CTRL, OFFSET_CH0, hCompositor->stV0Dither.ulCh0Offset) |
		BCHP_FIELD_DATA(CMP_0_V0_MC_DITHER_CTRL, SCALE_CH0,  hCompositor->stV0Dither.ulCh0Scale ));

	/* CMP_0_V0_MC_DITHER_LFSR_INIT */
	BVDC_P_CMP_GET_REG_DATA(CMP_0_V0_MC_DITHER_LFSR_INIT) &= ~(
		BCHP_MASK(CMP_0_V0_MC_DITHER_LFSR_INIT, SEQ) |
		BCHP_MASK(CMP_0_V0_MC_DITHER_LFSR_INIT, VALUE));
	BVDC_P_CMP_GET_REG_DATA(CMP_0_V0_MC_DITHER_LFSR_INIT) |=  (
		BCHP_FIELD_DATA(CMP_0_V0_MC_DITHER_LFSR_INIT, SEQ,   hCompositor->stV0Dither.ulLfsrSeq  ) |
		BCHP_FIELD_DATA(CMP_0_V0_MC_DITHER_LFSR_INIT, VALUE, hCompositor->stV0Dither.ulLfsrValue));

	/* CMP_0_V0_MC_DITHER_LFSR_CTRL */
	BVDC_P_CMP_GET_REG_DATA(CMP_0_V0_MC_DITHER_LFSR_CTRL) &= ~(
		BCHP_MASK(CMP_0_V0_MC_DITHER_LFSR_CTRL, T0) |
		BCHP_MASK(CMP_0_V0_MC_DITHER_LFSR_CTRL, T1) |
		BCHP_MASK(CMP_0_V0_MC_DITHER_LFSR_CTRL, T2));
	BVDC_P_CMP_GET_REG_DATA(CMP_0_V0_MC_DITHER_LFSR_CTRL) |=  (
		BCHP_FIELD_DATA(CMP_0_V0_MC_DITHER_LFSR_CTRL, T0, hCompositor->stV0Dither.ulLfsrCtrlT0) |
		BCHP_FIELD_DATA(CMP_0_V0_MC_DITHER_LFSR_CTRL, T1, hCompositor->stV0Dither.ulLfsrCtrlT1) |
		BCHP_FIELD_DATA(CMP_0_V0_MC_DITHER_LFSR_CTRL, T2, hCompositor->stV0Dither.ulLfsrCtrlT2));

#if (BVDC_P_CMP_0_MAX_VIDEO_WINDOW_COUNT > 1)
#if (BVDC_P_SUPPORT_CSC_MAT_COEF_VER >= 3)
	/* Dither */
	ulLfsrCtrlT0 = BVDC_P_DITHER_CMP_V1_LFSR_CTRL_T0;
	ulLfsrCtrlT1 = BVDC_P_DITHER_CMP_V1_LFSR_CTRL_T1;
	ulLfsrCtrlT2 = BVDC_P_DITHER_CMP_V1_LFSR_CTRL_T2;
	ulLfsrValue  = BVDC_P_DITHER_CMP_LFSR_VALUE;
	BVDC_P_Dither_Init(&hCompositor->stV1Dither,
		ulLfsrCtrlT0, ulLfsrCtrlT1, ulLfsrCtrlT2, ulLfsrValue);

	/* CMP_0_V1_DITHER_CTRL */
	BVDC_P_CMP_GET_REG_DATA(CMP_0_V1_MC_DITHER_CTRL) &= ~(
		BCHP_MASK(CMP_0_V1_MC_DITHER_CTRL, MODE      ) |
		BCHP_MASK(CMP_0_V1_MC_DITHER_CTRL, OFFSET_CH2) |
		BCHP_MASK(CMP_0_V1_MC_DITHER_CTRL, SCALE_CH2 ) |
		BCHP_MASK(CMP_0_V1_MC_DITHER_CTRL, OFFSET_CH1) |
		BCHP_MASK(CMP_0_V1_MC_DITHER_CTRL, SCALE_CH1 ) |
		BCHP_MASK(CMP_0_V1_MC_DITHER_CTRL, OFFSET_CH0) |
		BCHP_MASK(CMP_0_V1_MC_DITHER_CTRL, SCALE_CH0 ));
	BVDC_P_CMP_GET_REG_DATA(CMP_0_V1_MC_DITHER_CTRL) |=  (
		BCHP_FIELD_DATA(CMP_0_V1_MC_DITHER_CTRL, MODE,       hCompositor->stV1Dither.ulMode     ) |
		BCHP_FIELD_DATA(CMP_0_V1_MC_DITHER_CTRL, OFFSET_CH2, hCompositor->stV1Dither.ulCh1Offset) |
		BCHP_FIELD_DATA(CMP_0_V1_MC_DITHER_CTRL, SCALE_CH2,  hCompositor->stV1Dither.ulCh1Scale ) |
		BCHP_FIELD_DATA(CMP_0_V1_MC_DITHER_CTRL, OFFSET_CH1, hCompositor->stV1Dither.ulCh1Offset) |
		BCHP_FIELD_DATA(CMP_0_V1_MC_DITHER_CTRL, SCALE_CH1,  hCompositor->stV1Dither.ulCh1Scale ) |
		BCHP_FIELD_DATA(CMP_0_V1_MC_DITHER_CTRL, OFFSET_CH0, hCompositor->stV1Dither.ulCh0Offset) |
		BCHP_FIELD_DATA(CMP_0_V1_MC_DITHER_CTRL, SCALE_CH0,  hCompositor->stV1Dither.ulCh0Scale ));

	/* CMP_0_V1_MC_DITHER_LFSR_INIT  */
	BVDC_P_CMP_GET_REG_DATA(CMP_0_V1_MC_DITHER_LFSR_INIT ) &= ~(
		BCHP_MASK(CMP_0_V1_MC_DITHER_LFSR_INIT , SEQ) |
		BCHP_MASK(CMP_0_V1_MC_DITHER_LFSR_INIT , VALUE));
	BVDC_P_CMP_GET_REG_DATA(CMP_0_V1_MC_DITHER_LFSR_INIT ) |=  (
		BCHP_FIELD_DATA(CMP_0_V1_MC_DITHER_LFSR_INIT , SEQ,   hCompositor->stV1Dither.ulLfsrSeq  ) |
		BCHP_FIELD_DATA(CMP_0_V1_MC_DITHER_LFSR_INIT , VALUE, hCompositor->stV1Dither.ulLfsrValue));

	/* CMP_0_V1_MC_DITHER_LFSR_CTRL */
	BVDC_P_CMP_GET_REG_DATA(CMP_0_V1_MC_DITHER_LFSR_CTRL) &= ~(
		BCHP_MASK(CMP_0_V1_MC_DITHER_LFSR_CTRL, T0) |
		BCHP_MASK(CMP_0_V1_MC_DITHER_LFSR_CTRL, T1) |
		BCHP_MASK(CMP_0_V1_MC_DITHER_LFSR_CTRL, T2));
	BVDC_P_CMP_GET_REG_DATA(CMP_0_V1_MC_DITHER_LFSR_CTRL) |=  (
		BCHP_FIELD_DATA(CMP_0_V1_MC_DITHER_LFSR_CTRL, T0, hCompositor->stV1Dither.ulLfsrCtrlT0) |
		BCHP_FIELD_DATA(CMP_0_V1_MC_DITHER_LFSR_CTRL, T1, hCompositor->stV1Dither.ulLfsrCtrlT1) |
		BCHP_FIELD_DATA(CMP_0_V1_MC_DITHER_LFSR_CTRL, T2, hCompositor->stV1Dither.ulLfsrCtrlT2));
#else
	/* Dither */
	ulLfsrCtrlT0 = BVDC_P_DITHER_CMP_V1_LFSR_CTRL_T0;
	ulLfsrCtrlT1 = BVDC_P_DITHER_CMP_V1_LFSR_CTRL_T1;
	ulLfsrCtrlT2 = BVDC_P_DITHER_CMP_V1_LFSR_CTRL_T2;
	ulLfsrValue  = BVDC_P_DITHER_CMP_LFSR_VALUE;
	BVDC_P_Dither_Init(&hCompositor->stV1Dither,
		ulLfsrCtrlT0, ulLfsrCtrlT1, ulLfsrCtrlT2, ulLfsrValue);

	/* CMP_0_V1_DITHER_CTRL */
	BVDC_P_CMP_GET_REG_DATA(CMP_0_V1_DITHER_CTRL) &= ~(
		BCHP_MASK(CMP_0_V1_DITHER_CTRL, MODE      ) |
		BCHP_MASK(CMP_0_V1_DITHER_CTRL, OFFSET_CH2) |
		BCHP_MASK(CMP_0_V1_DITHER_CTRL, SCALE_CH2 ) |
		BCHP_MASK(CMP_0_V1_DITHER_CTRL, OFFSET_CH1) |
		BCHP_MASK(CMP_0_V1_DITHER_CTRL, SCALE_CH1 ) |
		BCHP_MASK(CMP_0_V1_DITHER_CTRL, OFFSET_CH0) |
		BCHP_MASK(CMP_0_V1_DITHER_CTRL, SCALE_CH0 ));
	BVDC_P_CMP_GET_REG_DATA(CMP_0_V1_DITHER_CTRL) |=  (
		BCHP_FIELD_DATA(CMP_0_V1_DITHER_CTRL, MODE,       hCompositor->stV1Dither.ulMode     ) |
		BCHP_FIELD_DATA(CMP_0_V1_DITHER_CTRL, OFFSET_CH2, hCompositor->stV1Dither.ulCh1Offset) |
		BCHP_FIELD_DATA(CMP_0_V1_DITHER_CTRL, SCALE_CH2,  hCompositor->stV1Dither.ulCh1Scale ) |
		BCHP_FIELD_DATA(CMP_0_V1_DITHER_CTRL, OFFSET_CH1, hCompositor->stV1Dither.ulCh1Offset) |
		BCHP_FIELD_DATA(CMP_0_V1_DITHER_CTRL, SCALE_CH1,  hCompositor->stV1Dither.ulCh1Scale ) |
		BCHP_FIELD_DATA(CMP_0_V1_DITHER_CTRL, OFFSET_CH0, hCompositor->stV1Dither.ulCh0Offset) |
		BCHP_FIELD_DATA(CMP_0_V1_DITHER_CTRL, SCALE_CH0,  hCompositor->stV1Dither.ulCh0Scale ));

	/* CMP_0_V1_DITHER_LFSR_INIT  */
	BVDC_P_CMP_GET_REG_DATA(CMP_0_V1_DITHER_LFSR_INIT ) &= ~(
		BCHP_MASK(CMP_0_V1_DITHER_LFSR_INIT , SEQ) |
		BCHP_MASK(CMP_0_V1_DITHER_LFSR_INIT , VALUE));
	BVDC_P_CMP_GET_REG_DATA(CMP_0_V1_DITHER_LFSR_INIT ) |=  (
		BCHP_FIELD_DATA(CMP_0_V1_DITHER_LFSR_INIT , SEQ,   hCompositor->stV1Dither.ulLfsrSeq  ) |
		BCHP_FIELD_DATA(CMP_0_V1_DITHER_LFSR_INIT , VALUE, hCompositor->stV1Dither.ulLfsrValue));

	/* CMP_0_V1_DITHER_LFSR_CTRL */
	BVDC_P_CMP_GET_REG_DATA(CMP_0_V1_DITHER_LFSR_CTRL) &= ~(
		BCHP_MASK(CMP_0_V1_DITHER_LFSR_CTRL, T0) |
		BCHP_MASK(CMP_0_V1_DITHER_LFSR_CTRL, T1) |
		BCHP_MASK(CMP_0_V1_DITHER_LFSR_CTRL, T2));
	BVDC_P_CMP_GET_REG_DATA(CMP_0_V1_DITHER_LFSR_CTRL) |=  (
		BCHP_FIELD_DATA(CMP_0_V1_DITHER_LFSR_CTRL, T0, hCompositor->stV1Dither.ulLfsrCtrlT0) |
		BCHP_FIELD_DATA(CMP_0_V1_DITHER_LFSR_CTRL, T1, hCompositor->stV1Dither.ulLfsrCtrlT1) |
		BCHP_FIELD_DATA(CMP_0_V1_DITHER_LFSR_CTRL, T2, hCompositor->stV1Dither.ulLfsrCtrlT2));
#endif
#endif
#endif

#if BVDC_P_CMP_0_MAX_WINDOW_COUNT > 2
	BVDC_P_CMP_GET_REG_DATA(CMP_0_BLEND_2_CTRL) &= ~(
		BCHP_MASK(CMP_0_BLEND_2_CTRL, BLEND_SOURCE));
	BVDC_P_CMP_GET_REG_DATA(CMP_0_BLEND_2_CTRL) |=  (
		BCHP_FIELD_ENUM(CMP_0_BLEND_2_CTRL, BLEND_SOURCE, BACKGROUND_BYPASS));

#if BVDC_P_CMP_0_MAX_WINDOW_COUNT > 3
	BVDC_P_CMP_GET_REG_DATA(CMP_0_BLEND_3_CTRL) &= ~(
		BCHP_MASK(CMP_0_BLEND_3_CTRL, BLEND_SOURCE));
	BVDC_P_CMP_GET_REG_DATA(CMP_0_BLEND_3_CTRL) |=  (
		BCHP_FIELD_ENUM(CMP_0_BLEND_3_CTRL, BLEND_SOURCE, BACKGROUND_BYPASS));

#if BVDC_P_CMP_0_MAX_WINDOW_COUNT > 4
	BVDC_P_CMP_GET_REG_DATA(CMP_0_BLEND_4_CTRL) &= ~(
		BCHP_MASK(CMP_0_BLEND_4_CTRL, BLEND_SOURCE));
	BVDC_P_CMP_GET_REG_DATA(CMP_0_BLEND_4_CTRL) |=  (
		BCHP_FIELD_ENUM(CMP_0_BLEND_4_CTRL, BLEND_SOURCE, BACKGROUND_BYPASS));
#endif
#endif
#endif

	/* PR 11368: Configuring compositors for multi-tap filtering, for
	 * eliminating distortion when using all-pass bandwidth filters. */
#if !BVDC_P_SUPPORT_BVN_10BIT_444
	BVDC_P_CMP_GET_REG_DATA(CMP_0_V0_SURFACE_CTRL) &= ~(
		BCHP_MASK(CMP_0_V0_SURFACE_CTRL, FILT_CTRL));
	BVDC_P_CMP_GET_REG_DATA(CMP_0_V0_SURFACE_CTRL) |=  (
#if BVDC_P_SUPPORT_CMP_TEN_TAP_422_TO_444
		BCHP_FIELD_ENUM(CMP_0_V0_SURFACE_CTRL, FILT_CTRL, TEN_TAPS_FILTERING));
#else
		BCHP_FIELD_ENUM(CMP_0_V0_SURFACE_CTRL, FILT_CTRL, MULTI_TAPS_FILTERING));
#endif

#if BVDC_P_SUPPORT_CMP_DERINGING
	BVDC_P_CMP_GET_REG_DATA(CMP_0_V0_SURFACE_CTRL) &= ~(
		BCHP_MASK(CMP_0_V0_SURFACE_CTRL, DERING_EN));
	BVDC_P_CMP_GET_REG_DATA(CMP_0_V0_SURFACE_CTRL) |=  (
		BCHP_FIELD_ENUM(CMP_0_V0_SURFACE_CTRL, DERING_EN, ENABLE));
#endif

#if BVDC_P_CMP_0_MAX_VIDEO_WINDOW_COUNT > 1
	BVDC_P_CMP_GET_REG_DATA(CMP_0_V1_SURFACE_CTRL) &= ~(
		BCHP_MASK(CMP_0_V1_SURFACE_CTRL, FILT_CTRL));
	BVDC_P_CMP_GET_REG_DATA(CMP_0_V1_SURFACE_CTRL) |=  (
#if BVDC_P_SUPPORT_CMP_TEN_TAP_422_TO_444
		BCHP_FIELD_ENUM(CMP_0_V1_SURFACE_CTRL, FILT_CTRL, TEN_TAPS_FILTERING));
#else
		BCHP_FIELD_ENUM(CMP_0_V1_SURFACE_CTRL, FILT_CTRL, MULTI_TAPS_FILTERING));
#endif  /* BVDC_P_SUPPORT_CMP_TEN_TAP_422_TO_444 */

#if BVDC_P_SUPPORT_CMP_DERINGING
	BVDC_P_CMP_GET_REG_DATA(CMP_0_V1_SURFACE_CTRL) &= ~(
		BCHP_MASK(CMP_0_V1_SURFACE_CTRL, DERING_EN));
	BVDC_P_CMP_GET_REG_DATA(CMP_0_V1_SURFACE_CTRL) |=  (
		BCHP_FIELD_ENUM(CMP_0_V1_SURFACE_CTRL, DERING_EN, ENABLE));
#endif

#endif  /* BVDC_P_CMP_0_MAX_VIDEO_WINDOW_COUNT > 1 */

#endif  /* !BVDC_P_SUPPORT_BVN_10BIT_444 */

	/* disable range clipping */
	BVDC_P_CMP_GET_REG_DATA(CMP_0_CMP_OUT_CTRL) &= ~(
		BCHP_MASK(CMP_0_CMP_OUT_CTRL, OUT_TO_DS_CTRL) |
		BCHP_MASK(CMP_0_CMP_OUT_CTRL, OUT_TO_VEC_CTRL) |
		BCHP_MASK(CMP_0_CMP_OUT_CTRL, CLIP_CTRL));
	BVDC_P_CMP_GET_REG_DATA(CMP_0_CMP_OUT_CTRL) |=  (
#if BVDC_P_SUPPORT_DOWNSAMPLE
		BCHP_FIELD_ENUM(CMP_0_CMP_OUT_CTRL, OUT_TO_DS_CTRL, ENABLE) |
#else
		BCHP_FIELD_ENUM(CMP_0_CMP_OUT_CTRL, OUT_TO_DS_CTRL, DISABLE) |
#endif
		BCHP_FIELD_ENUM(CMP_0_CMP_OUT_CTRL, OUT_TO_VEC_CTRL, ENABLE) |
		BCHP_FIELD_ENUM(CMP_0_CMP_OUT_CTRL, CLIP_CTRL, DISABLE));

#if BVDC_P_SUPPORT_CMP_CRC
	BVDC_P_CMP_GET_REG_DATA(CMP_0_CRC_CTRL) &= ~(
		BCHP_MASK(CMP_0_CRC_CTRL, INIT_VALUE) |
		BCHP_MASK(CMP_0_CRC_CTRL, PROBE_RATE) |
		BCHP_MASK(CMP_0_CRC_CTRL, CLEAR) |
		BCHP_MASK(CMP_0_CRC_CTRL, ENABLE));
	BVDC_P_CMP_GET_REG_DATA(CMP_0_CRC_CTRL) |=  (
		BCHP_FIELD_DATA(CMP_0_CRC_CTRL, INIT_VALUE, 0) |
		BCHP_FIELD_ENUM(CMP_0_CRC_CTRL, PROBE_RATE, ONE_PICTURE_PERIOD) |
		BCHP_FIELD_ENUM(CMP_0_CRC_CTRL, CLEAR,      CLEAR) |
		BCHP_FIELD_DATA(CMP_0_CRC_CTRL, ENABLE,     1));
#endif

	/* Initial new/current public states */
	pNewInfo = &hCompositor->stNewInfo;
	pCurInfo = &hCompositor->stCurInfo;

	/* Clear out user's states. */
	BKNI_Memset((void*)pNewInfo, 0x0, sizeof(BVDC_P_Compositor_Info));
	BKNI_Memset((void*)pCurInfo, 0x0, sizeof(BVDC_P_Compositor_Info));

	pNewDirty = &pNewInfo->stDirty;
	BDBG_ASSERT(sizeof(pNewDirty->stBits) <= sizeof(pNewDirty->aulInts));

	/* default values. */
	pNewInfo->pFmtInfo = BFMT_GetVideoFormatInfoPtr(
		hCompositor->hVdc->stSettings.eVideoFormat);

	pNewInfo->ulBgColorYCrCb = BVDC_P_YCRCB_BLACK;

	pNewInfo->stLumaRect.stRegion.ulLeft   = 0;
	pNewInfo->stLumaRect.stRegion.ulRight  = 0;
	pNewInfo->stLumaRect.stRegion.ulTop    = 0;
	pNewInfo->stLumaRect.stRegion.ulBottom = 0;
	pNewInfo->bLumaRectUserSet             = false;

	pNewInfo->stColorClipSettings.ulCrYSlopeA = 0;
	pNewInfo->stColorClipSettings.ulCrYSlopeB = 0;
	pNewInfo->stColorClipSettings.ulCbYSlopeA = 0;
	pNewInfo->stColorClipSettings.ulCbYSlopeB = 0;
	pNewInfo->stColorClipSettings.ulCrJoint = 0;
	pNewInfo->stColorClipSettings.ulCbJoint = 0;
	pNewInfo->stColorClipSettings.eColorClipMode = BVDC_ColorClipMode_None;

	/* Make sure it a non-null pointer for window creation. */
	pCurInfo->pFmtInfo = BFMT_GetVideoFormatInfoPtr(
		hCompositor->hVdc->stSettings.eVideoFormat);

	BDBG_LEAVE(BVDC_P_Compositor_Init);
	return;
}


/* Miscellaneous functions. */
/***************************************************************************
 * {private}
 *
 * Determined if new user settings are valid.  It validates all new
 * settings of windows within this compositor.
 */
static BERR_Code BVDC_P_Compositor_Validate
	( const BVDC_Compositor_Handle     hCompositor )
{
	uint32_t i, ulPhaseAdjCount, ulMasterCount, ulGameModeCount;
	BVDC_P_WindowId aeBlenderWindId[BVDC_P_WindowId_eUnknown];
	uint32_t ulBlenderId = 0;
	uint32_t ulVidWinCnt = 0;
	BERR_Code eStatus = BERR_SUCCESS;

	BDBG_OBJECT_ASSERT(hCompositor, BVDC_CMP);

	/* Defer validations until display handle is created. */
	if(!hCompositor->hDisplay)
	{
		return BERR_SUCCESS;
	}

	/* Get the new validated format from BVDC_P_Display! */
#if BVDC_P_SUPPORT_OSCL
	/* Certain chipsets rely on OSCL (output sclaer) within compositor to
	 * to achieve 1080p output. When in this mode, VEC is running at 1080p, but
	 * the whole BVN is configured as if the output format is 1080i.
	 * We use OSCL to convert the interlaced picture to frame then render it
	 * to VEC.
	 */
	if ((BFMT_VideoFmt_e1080p == hCompositor->hDisplay->stNewInfo.pFmtInfo->eVideoFmt) ||
		(BFMT_VideoFmt_e1080p_50Hz== hCompositor->hDisplay->stNewInfo.pFmtInfo->eVideoFmt))
	{
		hCompositor->stNewInfo.pFmtInfo =
			BFMT_GetVideoFormatInfoPtr((BFMT_VideoFmt_e1080p == hCompositor->hDisplay->stNewInfo.pFmtInfo->eVideoFmt) ?
				BFMT_VideoFmt_e1080i : BFMT_VideoFmt_e1080i_50Hz);
	}
	else
	{
		hCompositor->stNewInfo.pFmtInfo = hCompositor->hDisplay->stNewInfo.pFmtInfo;
	}

#else
	hCompositor->stNewInfo.pFmtInfo = hCompositor->hDisplay->stNewInfo.pFmtInfo;
#endif

	/* keep track of which blender get used */
	for(i = 0; i < BVDC_P_CMP_MAX_BLENDER; i++)
	{
		hCompositor->abBlenderUsed[i] = false;
	}
	ulPhaseAdjCount = 0;
	ulMasterCount = 0;
	ulGameModeCount = 0;

	/* Make sure all the changes to these windows are valid. */
	for(i = 0; i < BVDC_P_MAX_WINDOW_COUNT; i++)
	{
		if(BVDC_P_STATE_IS_ACTIVE(hCompositor->ahWindow[i]) ||
		   BVDC_P_STATE_IS_CREATE(hCompositor->ahWindow[i]))
		{
			uint8_t ucZOrder = hCompositor->ahWindow[i]->stNewInfo.ucZOrder;

			/* to make coverity check silent */
			if(ucZOrder > BVDC_Z_ORDER_MAX)
			{
				BDBG_ERR(("Window[%d]'s Z-order (%d) out of bound.",
					hCompositor->ahWindow[i]->eId, ucZOrder));
				return BERR_TRACE(BERR_INVALID_PARAMETER);
			}

			/* Did user messed up with the window ordering. */
			/*BDBG_MSG(("Window[%d][%d]'s z-order = %d", hCompositor->eId, i, ucZOrder));*/
			if(hCompositor->abBlenderUsed[ucZOrder])
			{
				BDBG_ERR(("Window[%d]'s Z-order (%d) duplicated.",
					hCompositor->ahWindow[i]->eId, ucZOrder));
				return BERR_TRACE(BVDC_ERR_DUPLICATED_ZORDER);
			}
			hCompositor->abBlenderUsed[ucZOrder] = true;
			aeBlenderWindId[ucZOrder] = hCompositor->ahWindow[i]->eId;

			/* Window validation is where the bulk of the work going to be. */
			eStatus = BVDC_P_Window_ValidateChanges(hCompositor->ahWindow[i],
				hCompositor->stNewInfo.pFmtInfo);
			if(BERR_SUCCESS != eStatus)
			{
				return BERR_TRACE(eStatus);
			}

			if(BVDC_P_WIN_IS_VIDEO_WINDOW(hCompositor->ahWindow[i]->eId))
			{
				ulVidWinCnt++;
			}

			if(BVDC_PhaseAdjustMode_eDisable !=
			   hCompositor->ahWindow[i]->stNewInfo.ePhaseAdjustMode)
			{
				ulPhaseAdjCount++;
			}

			if(hCompositor->ahWindow[i]->stNewInfo.bUseSrcFrameRate)
			{
				ulMasterCount++;
			}

			if(hCompositor->ahWindow[i]->stNewInfo.stGameDelaySetting.bEnable)
			{
				ulGameModeCount++;
			}
		}
	}

	/* Check number of video windows in a given compositor.  */
	if(ulVidWinCnt > BVDC_P_CMP_0_MAX_VIDEO_WINDOW_COUNT)
	{
		return BERR_TRACE(BVDC_ERR_WINDOW_NOT_AVAILABLE);
	}

	/* Check phase adjustment mode settings.  Only one analog source
	 * can be adjust with a given compositor.  */
	if(ulPhaseAdjCount > 1)
	{
		return BERR_TRACE(BVDC_ERR_MULTI_PHASE_ADJUST_VALUES);
	}

	/* Check master frame rate settings.  Only one video source
	 * can be master of a given compositor.  */
	if(ulMasterCount > 1)
	{
		BDBG_ERR(("CMP[%d] Can not enable multiple master framerate tracking windows",
			hCompositor->eId));
		return BERR_TRACE(BVDC_ERR_INVALID_FRAMERATE_USE);
	}

	/* Check game mode settings.  Only one video source
	 * can be in game mode of a given compositor.  */
	if(ulGameModeCount > 1)
	{
		return BERR_TRACE(BVDC_ERR_INVALID_MODE_PATH);
	}

	/* re-map the zorder back to real blender id; this is a collapse process;
	 * Note: the real blender id always starts from 0; this is to resolve the
	 *       problem that different hw may have different number of blenders,
	 *       while we still allow user to set various Zorders limited by the
	 *       same BVDC_Z_ORDER_MAX; */
	for(i = 0; i < BVDC_P_CMP_MAX_BLENDER; i++)
	{
		if(hCompositor->abBlenderUsed[i])
		{
			hCompositor->abBlenderUsed[i] = false;
			hCompositor->abBlenderUsed[ulBlenderId] = true;
			hCompositor->ahWindow[aeBlenderWindId[i]]->ulBlenderId =
				ulBlenderId++;
		}
	}

	return eStatus;
}


/***************************************************************************
 * {private}
 *
 * Determined if new user settings are valid.  It validates all new
 * settings of windows within this compositor.
 */
BERR_Code BVDC_P_Compositor_ValidateChanges
	( const BVDC_Compositor_Handle     ahCompositor[] )
{
	BERR_Code eStatus;
	int i;

	BDBG_ENTER(BVDC_P_Compositor_ValidateChanges);

	for(i = 0; i < BVDC_P_MAX_COMPOSITOR_COUNT; i++)
	{
		if(BVDC_P_STATE_IS_ACTIVE(ahCompositor[i]) ||
		   BVDC_P_STATE_IS_CREATE(ahCompositor[i]) ||
		   BVDC_P_STATE_IS_DESTROY(ahCompositor[i]))
		{
			eStatus = BVDC_P_Compositor_Validate(ahCompositor[i]);
			if(BERR_SUCCESS != eStatus)
			{
				return BERR_TRACE(eStatus);
			}
		}
	}

	BDBG_LEAVE(BVDC_P_Compositor_ValidateChanges);
	return BERR_SUCCESS;
}


/***************************************************************************
 * {private}
 *
 * All new window/compositor states are good.  Now we only need to apply
 * those changes when building the next RUL.
 */
void BVDC_P_Compositor_ApplyChanges_isr
	( BVDC_Compositor_Handle           hCompositor )
{
	BVDC_P_Compositor_Info *pNewInfo;
	BVDC_P_Compositor_Info *pCurInfo;
	BVDC_P_Compositor_DirtyBits *pNewDirty;
	BVDC_P_Compositor_DirtyBits *pCurDirty;
	BVDC_ClipRect *pNewRec;
	BVDC_ClipRect *pCurRec;
	int i;

	BDBG_ENTER(BVDC_P_Compositor_ApplyChanges_isr);
	BDBG_OBJECT_ASSERT(hCompositor, BVDC_CMP);
	BDBG_OBJECT_ASSERT(hCompositor->hVdc, BVDC_VDC);

	/* To reduce the amount of typing */
	pNewInfo = &hCompositor->stNewInfo;
	pCurInfo = &hCompositor->stCurInfo;

	pNewDirty = &pNewInfo->stDirty;
	pCurDirty = &pCurInfo->stDirty;

	/* State transition for display/compositor. */
	if(BVDC_P_STATE_IS_CREATE(hCompositor))
	{
		BDBG_MSG(("Compositor%d activated.", hCompositor->eId));
		hCompositor->eState = BVDC_P_State_eActive;
		BVDC_P_SET_ALL_DIRTY(pNewDirty);

		/* Check to make sure if this compositor need to handle checking for
		 * source lost. */
		if(!hCompositor->hVdc->hCmpCheckSource)
		{
			hCompositor->hVdc->hCmpCheckSource = hCompositor;
		}
	}
	else if(BVDC_P_STATE_IS_DESTROY(hCompositor))
	{
		BDBG_MSG(("Compositor%d de-activated", hCompositor->eId));
		hCompositor->eState = BVDC_P_State_eInactive;

		if (hCompositor->hDownSample)
		{
			BDBG_ERR(("Downsample source using this compositor still open."));
		}

		/* Check to make sure if this compositor need to delegate handle
		 * checking for source lost to another compositor. */
		if(hCompositor == hCompositor->hVdc->hCmpCheckSource)
		{
			hCompositor->hVdc->hCmpCheckSource = NULL;
			for(i = 0; i < BVDC_P_MAX_COMPOSITOR_COUNT; i++)
			{
				if(BVDC_P_STATE_IS_ACTIVE(hCompositor->hVdc->ahCompositor[i]) &&
				   hCompositor->hVdc->ahCompositor[i]->hDisplay &&
				   BVDC_P_STATE_IS_ACTIVE(hCompositor->hVdc->ahCompositor[i]->hDisplay))
				{
					hCompositor->hVdc->hCmpCheckSource = hCompositor->hVdc->ahCompositor[i];
				}
			}
		}
	}
	else if(BVDC_P_STATE_IS_INACTIVE(hCompositor))
	{
		goto done;
	}

	/* Compositor is always re-enable every vsync. */
	BVDC_P_CMP_GET_REG_DATA(CMP_0_CANVAS_CTRL) &= ~(
		BCHP_MASK(CMP_0_CANVAS_CTRL, ENABLE));
	BVDC_P_CMP_GET_REG_DATA(CMP_0_CANVAS_CTRL) |=  (
		BCHP_FIELD_ENUM(CMP_0_CANVAS_CTRL, ENABLE, ENABLE));

	hCompositor->ePhaseAdjustMode = BVDC_PhaseAdjustMode_eDisable;

	/* Applychanges for each windows. */
	for(i = 0; i < BVDC_P_MAX_WINDOW_COUNT; i++)
	{
		if(BVDC_P_STATE_IS_ACTIVE(hCompositor->ahWindow[i]) ||
		   BVDC_P_STATE_IS_CREATE(hCompositor->ahWindow[i]) ||
		   BVDC_P_STATE_IS_DESTROY(hCompositor->ahWindow[i]))
		{
			BVDC_P_Window_ApplyChanges_isr(hCompositor->ahWindow[i]);

			/* To Do: Pip window delay */
			if((hCompositor->ahWindow[i]->bAdjRectsDirty)
				&&(BVDC_P_DISPLAY_USED_STG(hCompositor->hDisplay->eMasterTg)
				&&(pNewInfo->pFmtInfo->bInterlaced==pCurInfo->pFmtInfo->bInterlaced)
				&&(pNewInfo->pFmtInfo->ulVertFreq==pCurInfo->pFmtInfo->ulVertFreq)))
			{
				hCompositor->ulDelayCnt = hCompositor->ahWindow[i]->ulTotalBvnDelay;
			}
		}
	}
	hCompositor->bDspAspRatDirty = false;

	/* defer compositor size -- number of lines. */
	/* set bgcolor */
	BVDC_P_CMP_SET_REG_DATA(CMP_0_BG_COLOR, pNewInfo->ulBgColorYCrCb);

	pNewRec = &pNewInfo->stLumaRect.stRegion;
	pCurRec = &pCurInfo->stLumaRect.stRegion;

	/* Any changes in compositor property. */
	if((BVDC_P_IS_DIRTY(pNewDirty)) ||
	   (pCurInfo->pFmtInfo != pNewInfo->pFmtInfo) ||
	   (pCurInfo->eOrientation != pNewInfo->eOrientation) ||
	   (pCurInfo->ulBgColorYCrCb != pNewInfo->ulBgColorYCrCb) ||
	   (pCurRec->ulLeft   != pNewRec->ulLeft) ||
	   (pCurRec->ulRight  != pNewRec->ulRight) ||
	   (pCurRec->ulTop    != pNewRec->ulTop) ||
	   (pCurRec->ulBottom != pNewRec->ulBottom))
	{
		/* Must be careful here not to globble current dirty bits set by compositor,
		 * but rather OR them together. */
		BVDC_P_OR_ALL_DIRTY(pNewDirty, pCurDirty);

		/* Update current info here to let windows wr/rd sides see consistent info. */
		hCompositor->stCurInfo = hCompositor->stNewInfo ;

		/* Clear dirty bits since they're already OR'ed into current. */
		if ((NULL!=hCompositor->hDisplay) &&
			(BVDC_P_DISPLAY_USED_STG(hCompositor->hDisplay->eMasterTg)))
		{
			if(!hCompositor->ulDelayCnt)
			{
				BVDC_P_CLEAN_ALL_DIRTY(pNewDirty);
				hCompositor->bUserAppliedChanges = true;
			}
		}
		else
		{
			BVDC_P_CLEAN_ALL_DIRTY(pNewDirty);
			hCompositor->bUserAppliedChanges = true;
		}
	}

done:
	BDBG_LEAVE(BVDC_P_Compositor_ApplyChanges_isr);
	return;
}


/***************************************************************************
 * {private}
 *
 */
void BVDC_P_Compositor_AbortChanges
	( BVDC_Compositor_Handle           hCompositor )
{
	int i;

	BDBG_ENTER(BVDC_P_Compositor_AbortChanges);
	BDBG_OBJECT_ASSERT(hCompositor, BVDC_CMP);

	/* Cancel the setting user set to the new state. */
	hCompositor->stNewInfo = hCompositor->stCurInfo;

	/* Applychanges for each windows. */
	for(i = 0; i < BVDC_P_MAX_WINDOW_COUNT; i++)
	{
		if(BVDC_P_STATE_IS_ACTIVE(hCompositor->ahWindow[i]) ||
		   BVDC_P_STATE_IS_CREATE(hCompositor->ahWindow[i]) ||
		   BVDC_P_STATE_IS_DESTROY(hCompositor->ahWindow[i]))
		{
			BDBG_OBJECT_ASSERT(hCompositor->ahWindow[i], BVDC_WIN);
			hCompositor->ahWindow[i]->stNewInfo = hCompositor->ahWindow[i]->stCurInfo;
		}
	}

	BDBG_LEAVE(BVDC_P_Compositor_AbortChanges);
	return;
}


/***************************************************************************
 * {private}
 *
 * Configure a color space conversion table inside a compositor.
 */
static void BVDC_P_Window_BuildCscRul_isr
	( BVDC_Compositor_Handle           hCompositor,
	  uint32_t                         ulCscStartReg,
	  const BVDC_P_CscCoeffs          *pCscCoeffs,
	  BVDC_P_ListInfo                 *pList)
{
	uint32_t ulBlockSize;

#if (BVDC_P_SUPPORT_CSC_MAT_COEF_VER >= 3)
	ulBlockSize = BVDC_P_REGS_ENTRIES(CMP_0_V0_R0_MC_COEFF_C00, CMP_0_V0_R0_MC_COEFF_C23);
#elif (BVDC_P_SUPPORT_CSC_MAT_COEF_VER >= 2)
	ulBlockSize = BVDC_P_REGS_ENTRIES(CMP_0_V0_MC_COEFF_C00, CMP_0_V0_MC_COEFF_C23);
#elif (BVDC_P_SUPPORT_CSC_MAT_COEF_VER >= 1)
	ulBlockSize = BVDC_P_REGS_ENTRIES(CMP_0_V0_COEFF_C00, CMP_0_V0_COEFF_C23);
#else
	ulBlockSize = BVDC_P_REGS_ENTRIES(CMP_0_V0_COEFF_C01_C00, CMP_0_V0_COEFF_C23_C22);
#endif

	BDBG_ENTER(BVDC_P_Window_BuildCscRul_isr);
	BDBG_OBJECT_ASSERT(hCompositor, BVDC_CMP);

	*pList->pulCurrent++ = BRDC_OP_IMMS_TO_REGS( ulBlockSize );
#if (BVDC_P_SUPPORT_CSC_MAT_COEF_VER == 2)
	*pList->pulCurrent++ = BRDC_REGISTER( BVDC_P_Compositor_GetCmpRegAddr_isr(hCompositor->eId, ulCscStartReg) +  hCompositor->ulRegOffset);
#else
	*pList->pulCurrent++ = BRDC_REGISTER( ulCscStartReg + hCompositor->ulRegOffset);
#endif

#if (BVDC_P_SUPPORT_CSC_MAT_COEF_VER >= 3)
	/* [ c00, c01 c02 c03 ] */
	*pList->pulCurrent++ = BCHP_FIELD_DATA(CMP_0_V0_R0_MC_COEFF_C00, COEFF_MUL, pCscCoeffs->usY0);
	*pList->pulCurrent++ = BCHP_FIELD_DATA(CMP_0_V0_R0_MC_COEFF_C01, COEFF_MUL, pCscCoeffs->usY1);
	*pList->pulCurrent++ = BCHP_FIELD_DATA(CMP_0_V0_R0_MC_COEFF_C02, COEFF_MUL, pCscCoeffs->usY2);
	*pList->pulCurrent++ = BCHP_FIELD_DATA(CMP_0_V0_R0_MC_COEFF_C03, COEFF_ADD, pCscCoeffs->usYOffset);

	/* [ c10, c11 c12 c13 ] */
	*pList->pulCurrent++ = BCHP_FIELD_DATA(CMP_0_V0_R0_MC_COEFF_C10, COEFF_MUL, pCscCoeffs->usCb0);
	*pList->pulCurrent++ = BCHP_FIELD_DATA(CMP_0_V0_R0_MC_COEFF_C11, COEFF_MUL, pCscCoeffs->usCb1);
	*pList->pulCurrent++ = BCHP_FIELD_DATA(CMP_0_V0_R0_MC_COEFF_C12, COEFF_MUL, pCscCoeffs->usCb2);
	*pList->pulCurrent++ = BCHP_FIELD_DATA(CMP_0_V0_R0_MC_COEFF_C13, COEFF_ADD, pCscCoeffs->usCbOffset);

	/* [ c20, c21 c22 c23 ] */
	*pList->pulCurrent++ = BCHP_FIELD_DATA(CMP_0_V0_R0_MC_COEFF_C20, COEFF_MUL, pCscCoeffs->usCr0);
	*pList->pulCurrent++ = BCHP_FIELD_DATA(CMP_0_V0_R0_MC_COEFF_C21, COEFF_MUL, pCscCoeffs->usCr1);
	*pList->pulCurrent++ = BCHP_FIELD_DATA(CMP_0_V0_R0_MC_COEFF_C22, COEFF_MUL, pCscCoeffs->usCr2);
	*pList->pulCurrent++ = BCHP_FIELD_DATA(CMP_0_V0_R0_MC_COEFF_C23, COEFF_ADD, pCscCoeffs->usCrOffset);

#elif (BVDC_P_SUPPORT_CSC_MAT_COEF_VER >= 2)
	/* [ c00, c01 c02 c03 ] */
	*pList->pulCurrent++ = BCHP_FIELD_DATA(CMP_0_V0_MC_COEFF_C00, COEFF_MUL, pCscCoeffs->usY0);
	*pList->pulCurrent++ = BCHP_FIELD_DATA(CMP_0_V0_MC_COEFF_C01, COEFF_MUL, pCscCoeffs->usY1);
	*pList->pulCurrent++ = BCHP_FIELD_DATA(CMP_0_V0_MC_COEFF_C02, COEFF_MUL, pCscCoeffs->usY2);
	*pList->pulCurrent++ = BCHP_FIELD_DATA(CMP_0_V0_MC_COEFF_C03, COEFF_ADD, pCscCoeffs->usYOffset);

	/* [ c10, c11 c12 c13 ] */
	*pList->pulCurrent++ = BCHP_FIELD_DATA(CMP_0_V0_MC_COEFF_C10, COEFF_MUL, pCscCoeffs->usCb0);
	*pList->pulCurrent++ = BCHP_FIELD_DATA(CMP_0_V0_MC_COEFF_C11, COEFF_MUL, pCscCoeffs->usCb1);
	*pList->pulCurrent++ = BCHP_FIELD_DATA(CMP_0_V0_MC_COEFF_C12, COEFF_MUL, pCscCoeffs->usCb2);
	*pList->pulCurrent++ = BCHP_FIELD_DATA(CMP_0_V0_MC_COEFF_C13, COEFF_ADD, pCscCoeffs->usCbOffset);

	/* [ c20, c21 c22 c23 ] */
	*pList->pulCurrent++ = BCHP_FIELD_DATA(CMP_0_V0_MC_COEFF_C20, COEFF_MUL, pCscCoeffs->usCr0);
	*pList->pulCurrent++ = BCHP_FIELD_DATA(CMP_0_V0_MC_COEFF_C21, COEFF_MUL, pCscCoeffs->usCr1);
	*pList->pulCurrent++ = BCHP_FIELD_DATA(CMP_0_V0_MC_COEFF_C22, COEFF_MUL, pCscCoeffs->usCr2);
	*pList->pulCurrent++ = BCHP_FIELD_DATA(CMP_0_V0_MC_COEFF_C23, COEFF_ADD, pCscCoeffs->usCrOffset);

#elif (BVDC_P_SUPPORT_CSC_MAT_COEF_VER >= 1)
	/* [ c00, c01 c02 c03 ] */
	*pList->pulCurrent++ = BCHP_FIELD_DATA(CMP_0_V0_COEFF_C00, COEFF_MUL, pCscCoeffs->usY0);
	*pList->pulCurrent++ = BCHP_FIELD_DATA(CMP_0_V0_COEFF_C01, COEFF_MUL, pCscCoeffs->usY1);
	*pList->pulCurrent++ = BCHP_FIELD_DATA(CMP_0_V0_COEFF_C02, COEFF_MUL, pCscCoeffs->usY2);
	*pList->pulCurrent++ = BCHP_FIELD_DATA(CMP_0_V0_COEFF_C03, COEFF_ADD, pCscCoeffs->usYOffset);

	/* [ c10, c11 c12 c13 ] */
	*pList->pulCurrent++ = BCHP_FIELD_DATA(CMP_0_V0_COEFF_C10, COEFF_MUL, pCscCoeffs->usCb0);
	*pList->pulCurrent++ = BCHP_FIELD_DATA(CMP_0_V0_COEFF_C11, COEFF_MUL, pCscCoeffs->usCb1);
	*pList->pulCurrent++ = BCHP_FIELD_DATA(CMP_0_V0_COEFF_C12, COEFF_MUL, pCscCoeffs->usCb2);
	*pList->pulCurrent++ = BCHP_FIELD_DATA(CMP_0_V0_COEFF_C13, COEFF_ADD, pCscCoeffs->usCbOffset);

	/* [ c20, c21 c22 c23 ] */
	*pList->pulCurrent++ = BCHP_FIELD_DATA(CMP_0_V0_COEFF_C20, COEFF_MUL, pCscCoeffs->usCr0);
	*pList->pulCurrent++ = BCHP_FIELD_DATA(CMP_0_V0_COEFF_C21, COEFF_MUL, pCscCoeffs->usCr1);
	*pList->pulCurrent++ = BCHP_FIELD_DATA(CMP_0_V0_COEFF_C22, COEFF_MUL, pCscCoeffs->usCr2);
	*pList->pulCurrent++ = BCHP_FIELD_DATA(CMP_0_V0_COEFF_C23, COEFF_ADD, pCscCoeffs->usCrOffset);
#else
	/* [ c00, c01 c02 c03 ] */
	*pList->pulCurrent++ = BCHP_FIELD_DATA(CMP_0_V0_COEFF_C01_C00, COEFF_C0, pCscCoeffs->usY0) |
	                       BCHP_FIELD_DATA(CMP_0_V0_COEFF_C01_C00, COEFF_C1, pCscCoeffs->usY1);
	*pList->pulCurrent++ = BCHP_FIELD_DATA(CMP_0_V0_COEFF_C03_C02, COEFF_C2, pCscCoeffs->usY2) |
	                       BCHP_FIELD_DATA(CMP_0_V0_COEFF_C03_C02, COEFF_C3, pCscCoeffs->usYOffset);

	/* [ c10, c11 c12 c13 ] */
	*pList->pulCurrent++ = BCHP_FIELD_DATA(CMP_0_V0_COEFF_C11_C10, COEFF_C0, pCscCoeffs->usCb0) |
	                       BCHP_FIELD_DATA(CMP_0_V0_COEFF_C11_C10, COEFF_C1, pCscCoeffs->usCb1);
	*pList->pulCurrent++ = BCHP_FIELD_DATA(CMP_0_V0_COEFF_C13_C12, COEFF_C2, pCscCoeffs->usCb2) |
	                       BCHP_FIELD_DATA(CMP_0_V0_COEFF_C13_C12, COEFF_C3, pCscCoeffs->usCbOffset);

	/* [ c20, c21 c22 c23 ] */
	*pList->pulCurrent++ = BCHP_FIELD_DATA(CMP_0_V0_COEFF_C21_C20, COEFF_C0, pCscCoeffs->usCr0) |
	                       BCHP_FIELD_DATA(CMP_0_V0_COEFF_C21_C20, COEFF_C1, pCscCoeffs->usCr1);
	*pList->pulCurrent++ = BCHP_FIELD_DATA(CMP_0_V0_COEFF_C23_C22, COEFF_C2, pCscCoeffs->usCr2) |
	                       BCHP_FIELD_DATA(CMP_0_V0_COEFF_C23_C22, COEFF_C3, pCscCoeffs->usCrOffset);
#endif
	*pList->pulCurrent += ulBlockSize;

	BDBG_LEAVE(BVDC_P_Window_BuildCscRul_isr);
	return;
}


#if (BVDC_P_SUPPORT_COLOR_CLIP)
/*************************************************************************
 *  {secret}
 *	BVDC_P_ColorClip_BuildRul_isr
 *	Builds Color Clip block
 **************************************************************************/
static void BVDC_P_Compositor_BuildColorClipRul_isr
	( BVDC_Compositor_Handle           hCompositor,
	  BVDC_P_ListInfo                 *pList)
{
	BVDC_ColorClipSettings *pColorClipSettings;

	BDBG_ENTER(BVDC_P_Compositor_BuildColorClipRul_isr);
	BDBG_OBJECT_ASSERT(hCompositor, BVDC_CMP);

	pColorClipSettings = &hCompositor->stCurInfo.stColorClipSettings;

		BDBG_MSG(("Color Clip Mode = %d", pColorClipSettings->eColorClipMode));

	/* Build RUL */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_CMP_0_COLOR_CLIP_CTRL);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(CMP_0_COLOR_CLIP_CTRL, CLIP_COUNT_RESET, 0) |
		BCHP_FIELD_DATA(CMP_0_COLOR_CLIP_CTRL, ENABLE,
			(pColorClipSettings->eColorClipMode == BVDC_ColorClipMode_None) ? 0 : 1) |
		BCHP_FIELD_DATA(CMP_0_COLOR_CLIP_CTRL, WHITE_DISABLE,
			(pColorClipSettings->eColorClipMode == BVDC_ColorClipMode_White ||
			 pColorClipSettings->eColorClipMode == BVDC_ColorClipMode_Both) ? 0 : 1) |
		BCHP_FIELD_DATA(CMP_0_COLOR_CLIP_CTRL, BLACK_DISABLE,
			(pColorClipSettings->eColorClipMode == BVDC_ColorClipMode_Black ||
			 pColorClipSettings->eColorClipMode == BVDC_ColorClipMode_Both) ? 0 : 1) |
		BCHP_FIELD_DATA(CMP_0_COLOR_CLIP_CTRL, EXTENDED_WHITE_RANGE,
			pColorClipSettings->bExtendedWhite ? 1 : 0) |
		BCHP_FIELD_DATA(CMP_0_COLOR_CLIP_CTRL, EXTENDED_BLACK_RANGE,
			pColorClipSettings->bExtendedBlack ? 1 : 0);

	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_CMP_0_CB_LUMA_SLOPE);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(CMP_0_CB_LUMA_SLOPE, SLOPE_B, (pColorClipSettings->ulCbYSlopeB >> 8) & 0x7FF) |
		BCHP_FIELD_DATA(CMP_0_CB_LUMA_SLOPE, SLOPE_A, (pColorClipSettings->ulCbYSlopeA >> 8) & 0x7FF);

	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_CMP_0_CR_LUMA_SLOPE);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(CMP_0_CR_LUMA_SLOPE, SLOPE_B, (pColorClipSettings->ulCrYSlopeB >> 8) & 0x7FF) |
		BCHP_FIELD_DATA(CMP_0_CR_LUMA_SLOPE, SLOPE_A, (pColorClipSettings->ulCrYSlopeA >> 8) & 0x7FF);

	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_CMP_0_CB_CR_SLOPE_JOINT);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(CMP_0_CB_CR_SLOPE_JOINT, CB_SLOPE_JOINT, pColorClipSettings->ulCbJoint) |
		BCHP_FIELD_DATA(CMP_0_CB_CR_SLOPE_JOINT, CR_SLOPE_JOINT, pColorClipSettings->ulCrJoint);

	BDBG_LEAVE(BVDC_P_ColorClip_BuildRul_isr);
	return;
}
#endif


/***************************************************************************
 * {private}
 *
 * This function adds the CMP_x registers into pList.  This functions
 * assumes that the shadowed-registers have been updated, and dirty bits
 * are set approriately.
 */
static void BVDC_P_Compositor_BuildRul_isr
	( BVDC_Compositor_Handle           hCompositor,
	  BVDC_P_ListInfo                 *pList,
	  BAVC_Polarity                    eFieldId )
{
	BVDC_P_Window_DirtyBits *pV0CurDirty;
	BVDC_P_WindowId eV0Id, eV1Id = BVDC_P_WindowId_eComp0_V1;

	BDBG_ENTER(BVDC_P_Compositor_BuildRul_isr);
	BDBG_OBJECT_ASSERT(hCompositor, BVDC_CMP);
	BSTD_UNUSED(eFieldId);

	if(hCompositor->bIsBypass)
	{
		/* Bypass compositor (dummy one) has nothing to program except
		 * video path controlling. */
		pV0CurDirty = &(hCompositor->ahWindow[BVDC_P_WindowId_eComp2_V0]->stCurInfo.stDirty);

		/* CSC already handle in 656out clear dirty bit now. */
		pV0CurDirty->stBits.bCscAdjust = BVDC_P_CLEAN;
		pV0CurDirty->stBits.bCabAdjust = BVDC_P_CLEAN;
		pV0CurDirty->stBits.bLabAdjust = BVDC_P_CLEAN;
		pV0CurDirty->stBits.bTntAdjust = BVDC_P_CLEAN;

#if BVDC_P_SUPPORT_CM3D
		pV0CurDirty->stBits.bCm3dRegionAdj = BVDC_P_CLEAN;
		pV0CurDirty->stBits.bCm3dOverlapRegionAdj = BVDC_P_CLEAN;
#endif

		return;
	}

	eV0Id =
		(hCompositor->eId == BVDC_CompositorId_eCompositor1) ? BVDC_P_WindowId_eComp1_V0 :
		((hCompositor->eId == BVDC_CompositorId_eCompositor0) ? BVDC_P_WindowId_eComp0_V0 :
		((hCompositor->eId == BVDC_CompositorId_eCompositor3) ? BVDC_P_WindowId_eComp3_V0 :
		((hCompositor->eId == BVDC_CompositorId_eCompositor4) ? BVDC_P_WindowId_eComp4_V0 :
		((hCompositor->eId == BVDC_CompositorId_eCompositor5) ? BVDC_P_WindowId_eComp5_V0 :
		((hCompositor->eId == BVDC_CompositorId_eCompositor6) ? BVDC_P_WindowId_eComp6_V0 :
			BVDC_P_WindowId_eComp2_V0)))));
	if(hCompositor->pFeatures->ulMaxVideoWindow > 1)
	{
		eV1Id = hCompositor->eId? BVDC_P_WindowId_eComp1_V1 : BVDC_P_WindowId_eComp0_V1;
	}

	/* reset */
	if(hCompositor->bInitial)
	{
		/* Make sure these get re-programm when reset. */
		hCompositor->ulCscAdjust[eV0Id] = BVDC_P_RUL_UPDATE_THRESHOLD;
		hCompositor->ulLumaKeyAdjust[eV0Id] = BVDC_P_RUL_UPDATE_THRESHOLD;
		hCompositor->bCscDemoCompute[eV0Id] = true;

		if(hCompositor->pFeatures->ulMaxVideoWindow > 1)
		{
			hCompositor->ulCscAdjust[eV1Id] = BVDC_P_RUL_UPDATE_THRESHOLD;
			hCompositor->ulLumaKeyAdjust[eV1Id] = BVDC_P_RUL_UPDATE_THRESHOLD;
			hCompositor->bCscDemoCompute[eV1Id] = true;
		}
		BVDC_P_BUILD_RESET(pList->pulCurrent,
			hCompositor->ulCoreResetAddr, hCompositor->ulCoreResetMask);

#if BVDC_P_SUPPORT_CMP_CRC
		/* Also make sure to enable CRC */
		BVDC_P_CMP_WRITE_TO_RUL(CMP_0_CRC_CTRL, pList->pulCurrent);
#endif

		BVDC_P_CMP_WRITE_TO_RUL(CMP_0_CMP_OUT_CTRL, pList->pulCurrent);
	}

#if BVDC_P_SUPPORT_OSCL
	/* Build OSCL RUL if it presents */
	if (BVDC_CompositorId_eCompositor0 == hCompositor->eId)
	{
		BVDC_P_OSCL_BuildRul_isr(hCompositor, pList, eFieldId);
	}
#endif

	if(eV0Id == BVDC_P_WindowId_eComp0_V0)
	{
		BVDC_P_Pep_BuildRul_isr(hCompositor->ahWindow[eV0Id], pList, hCompositor->bInitial);
#if BVDC_P_SUPPORT_CM3D
		BVDC_P_Cm3d_BuildRul_isr(hCompositor->ahWindow[eV0Id], pList, hCompositor->bInitial);
#endif
	}

	/* canvas, bgcolor and blender setting */
	BVDC_P_CMP_BLOCK_WRITE_TO_RUL(CMP_0_CANVAS_SIZE, CMP_0_BG_COLOR, pList->pulCurrent);
	BVDC_P_CMP_WRITE_TO_RUL(CMP_0_BLEND_0_CTRL, pList->pulCurrent);

#if BVDC_P_CMP_0_MAX_WINDOW_COUNT > 1
	if(hCompositor->pFeatures->ulMaxWindow > 1)
	{
		BVDC_P_CMP_WRITE_TO_RUL(CMP_0_BLEND_1_CTRL, pList->pulCurrent);
	}
#endif

#if BVDC_P_CMP_0_MAX_WINDOW_COUNT > 2
	if(hCompositor->pFeatures->ulMaxWindow > 2)
	{
		BVDC_P_CMP_WRITE_TO_RUL(CMP_0_BLEND_2_CTRL, pList->pulCurrent);
	}
#endif

#if BVDC_P_CMP_0_MAX_WINDOW_COUNT > 3
	if(hCompositor->pFeatures->ulMaxWindow > 3)
	{
		BVDC_P_CMP_WRITE_TO_RUL(CMP_0_BLEND_3_CTRL, pList->pulCurrent);
	}
#endif

#if BVDC_P_CMP_0_MAX_WINDOW_COUNT > 4
	if(hCompositor->pFeatures->ulMaxWindow > 4)
	{
		BVDC_P_CMP_WRITE_TO_RUL(CMP_0_BLEND_4_CTRL, pList->pulCurrent);
	}
#endif

#if BVDC_P_SUPPORT_3D_VIDEO
	BVDC_P_CMP_WRITE_TO_RUL(CMP_0_CMP_CTRL, pList->pulCurrent);
#endif

	/* TODO:  Optimize with dirty bits */
#if (BVDC_P_SUPPORT_CMP_LUMA_AVG_VER > 0)
	BVDC_P_CMP_WRITE_TO_RUL(CMP_0_LUMA_AVG_CTRL,           pList->pulCurrent);
	BVDC_P_CMP_WRITE_TO_RUL(CMP_0_CMP_LUMA_AVG_WIN_OFFSET, pList->pulCurrent);
	BVDC_P_CMP_WRITE_TO_RUL(CMP_0_CMP_LUMA_AVG_WIN_SIZE,   pList->pulCurrent);
#endif

#if BVDC_P_SUPPORT_CMP_DEMO_MODE
	BVDC_P_CMP_WRITE_TO_RUL(CMP_0_CSC_DEMO_SETTING, pList->pulCurrent);
#endif

#if BVDC_P_SUPPORT_WIN_CONST_COLOR
	/* window constant color settings */
	BVDC_P_CMP_WRITE_TO_RUL(CMP_0_V0_CONST_COLOR, pList->pulCurrent);

#if BVDC_P_CMP_0_MAX_VIDEO_WINDOW_COUNT > 1
	if(hCompositor->pFeatures->ulMaxVideoWindow > 1)
	{
		BVDC_P_CMP_WRITE_TO_RUL(CMP_0_V1_CONST_COLOR, pList->pulCurrent);
	}
#endif
#endif

	/* Vwin0 */
	if(BVDC_P_CMP_COMPARE_FIELD_DATA(CMP_0_V0_SURFACE_CTRL, SURFACE_ENABLE, 1))
	{
		if (((BVDC_P_CMP_GET_FIELD_NAME(CMP_0_V0_DISPLAY_SIZE, HSIZE) +
			  BVDC_P_CMP_GET_FIELD_NAME(CMP_0_V0_SURFACE_OFFSET, X_OFFSET)) >
			 (BVDC_P_CMP_GET_FIELD_NAME(CMP_0_V0_SURFACE_SIZE, HSIZE))) ||
			((BVDC_P_CMP_GET_FIELD_NAME(CMP_0_V0_DISPLAY_SIZE, VSIZE) +
			  BVDC_P_CMP_GET_FIELD_NAME(CMP_0_V0_SURFACE_OFFSET, Y_OFFSET)) >
			 (BVDC_P_CMP_GET_FIELD_NAME(CMP_0_V0_SURFACE_SIZE, VSIZE))))
		{
			uint32_t  ulDspW, ulCutLeft, ulDspH, ulCutTop;

			BDBG_ERR(("Invalid CMP V0 disp size [H:%d, V:%d] vs surf size [H:%d, V:%d] - [X:%d, Y:%d]",
					  BVDC_P_CMP_GET_FIELD_NAME(CMP_0_V0_DISPLAY_SIZE, HSIZE),
					  BVDC_P_CMP_GET_FIELD_NAME(CMP_0_V0_DISPLAY_SIZE, VSIZE),
					  BVDC_P_CMP_GET_FIELD_NAME(CMP_0_V0_SURFACE_SIZE, HSIZE),
					  BVDC_P_CMP_GET_FIELD_NAME(CMP_0_V0_SURFACE_SIZE, VSIZE),
					  BVDC_P_CMP_GET_FIELD_NAME(CMP_0_V0_SURFACE_OFFSET, X_OFFSET),
					  BVDC_P_CMP_GET_FIELD_NAME(CMP_0_V0_SURFACE_OFFSET, Y_OFFSET)));

			ulDspW = BVDC_P_CMP_GET_FIELD_NAME(CMP_0_V0_DISPLAY_SIZE, HSIZE);
			ulCutLeft = BVDC_P_CMP_GET_FIELD_NAME(CMP_0_V0_SURFACE_OFFSET, X_OFFSET);
			ulDspH = BVDC_P_CMP_GET_FIELD_NAME(CMP_0_V0_DISPLAY_SIZE, VSIZE);
			ulCutTop = BVDC_P_CMP_GET_FIELD_NAME(CMP_0_V0_SURFACE_OFFSET, Y_OFFSET);

			if ((BVDC_P_CMP_GET_FIELD_NAME(CMP_0_V0_DISPLAY_SIZE, HSIZE) +
				 BVDC_P_CMP_GET_FIELD_NAME(CMP_0_V0_SURFACE_OFFSET, X_OFFSET)) >
				(BVDC_P_CMP_GET_FIELD_NAME(CMP_0_V0_SURFACE_SIZE, HSIZE)))
			{
				ulDspW = BVDC_P_CMP_GET_FIELD_NAME(CMP_0_V0_SURFACE_SIZE, HSIZE);
				ulCutLeft = 0;
			}
			if ((BVDC_P_CMP_GET_FIELD_NAME(CMP_0_V0_DISPLAY_SIZE, VSIZE) +
				 BVDC_P_CMP_GET_FIELD_NAME(CMP_0_V0_SURFACE_OFFSET, Y_OFFSET)) >
				(BVDC_P_CMP_GET_FIELD_NAME(CMP_0_V0_SURFACE_SIZE, VSIZE)))
			{
				ulDspH = BVDC_P_CMP_GET_FIELD_NAME(CMP_0_V0_SURFACE_SIZE, VSIZE);
				ulCutTop = 0;
			}

			BVDC_P_CMP_GET_REG_DATA(CMP_0_V0_DISPLAY_SIZE) =
				BCHP_FIELD_DATA(CMP_0_V0_DISPLAY_SIZE, HSIZE, ulDspW) |
				BCHP_FIELD_DATA(CMP_0_V0_DISPLAY_SIZE, VSIZE, ulDspH);
			BVDC_P_CMP_GET_REG_DATA(CMP_0_V0_SURFACE_OFFSET) =
				BCHP_FIELD_DATA(CMP_0_V0_SURFACE_OFFSET, X_OFFSET, ulCutLeft) |
				BCHP_FIELD_DATA(CMP_0_V0_SURFACE_OFFSET, Y_OFFSET, ulCutTop);
		}

		BVDC_P_CMP_BLOCK_WRITE_TO_RUL(CMP_0_V0_SURFACE_SIZE, CMP_0_V0_CANVAS_OFFSET, pList->pulCurrent);

#if BVDC_P_SUPPORT_3D_VIDEO
		BVDC_P_CMP_WRITE_TO_RUL(CMP_0_V0_CANVAS_X_OFFSET_R, pList->pulCurrent);
#endif

#if (BVDC_P_SUPPORT_DITHER)
		BVDC_P_CMP_WRITE_TO_RUL(CMP_0_V0_MC_DITHER_CTRL, pList->pulCurrent);
		BVDC_P_CMP_WRITE_TO_RUL(CMP_0_V0_MC_DITHER_LFSR_INIT, pList->pulCurrent);
		BVDC_P_CMP_WRITE_TO_RUL(CMP_0_V0_MC_DITHER_LFSR_CTRL, pList->pulCurrent);
#endif

#if BVDC_P_SUPPORT_CMP_CLEAR_RECT_V0
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)
		if(hCompositor->ulMosaicAdjust[eV0Id] || hCompositor->bInitial)
		{
			if(hCompositor->ahWindow[eV0Id]->stCurInfo.bClearRect)
			{
				BVDC_P_CMP_WRITE_TO_RUL(CMP_0_V0_RECT_COLOR, pList->pulCurrent);
				BVDC_P_CMP_BLOCK_WRITE_TO_RUL(CMP_0_V0_RECT_TOP_CTRL, CMP_0_V0_RECT_ENABLE_MASK, pList->pulCurrent);
				BVDC_P_CMP_RECT_BLOCK_WRITE_TO_RUL(CMP_0_V0_RECT_SIZEi_ARRAY_BASE,
					hCompositor->ahWindow[eV0Id]->stCurInfo.ulMosaicCount, pList->pulCurrent);
				BVDC_P_CMP_RECT_BLOCK_WRITE_TO_RUL(CMP_0_V0_RECT_OFFSETi_ARRAY_BASE,
					hCompositor->ahWindow[eV0Id]->stCurInfo.ulMosaicCount, pList->pulCurrent);
			}
			else
			{
				*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
#if (BVDC_P_SUPPORT_CSC_MAT_COEF_VER == 2)
				*pList->pulCurrent++ = BRDC_REGISTER( BVDC_P_Compositor_GetCmpRegAddr_isr(hCompositor->eId, BCHP_CMP_0_V0_RECT_TOP_CTRL) + hCompositor->ulRegOffset);
#else
				*pList->pulCurrent++ = BRDC_REGISTER(BCHP_CMP_0_V0_RECT_TOP_CTRL + hCompositor->ulRegOffset);
#endif
				*pList->pulCurrent++ = 0;
			}

			if(pList->bLastExecuted && hCompositor->ulMosaicAdjust[eV0Id])
			{
				hCompositor->ulMosaicAdjust[eV0Id]--;
			}
		}
#else
		if(hCompositor->ulMosaicAdjust || hCompositor->bInitial)
		{
			if(hCompositor->ahWindow[eV0Id]->stCurInfo.bClearRect)
			{
				BVDC_P_CMP_WRITE_TO_RUL(CMP_0_V0_RECT_COLOR, pList->pulCurrent);
				BVDC_P_CMP_BLOCK_WRITE_TO_RUL(CMP_0_V0_RECT_TOP_CTRL, CMP_0_V0_RECT_ENABLE_MASK, pList->pulCurrent);
				BVDC_P_CMP_RECT_BLOCK_WRITE_TO_RUL(CMP_0_V0_RECT_SIZEi_ARRAY_BASE,
					hCompositor->ahWindow[eV0Id]->stCurInfo.ulMosaicCount, pList->pulCurrent);
				BVDC_P_CMP_RECT_BLOCK_WRITE_TO_RUL(CMP_0_V0_RECT_OFFSETi_ARRAY_BASE,
					hCompositor->ahWindow[eV0Id]->stCurInfo.ulMosaicCount, pList->pulCurrent);
			}
			else
			{
				*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
#if (BVDC_P_SUPPORT_CSC_MAT_COEF_VER == 2)
				*pList->pulCurrent++ = BRDC_REGISTER( BVDC_P_Compositor_GetCmpRegAddr_isr(hCompositor->eId, BCHP_CMP_0_V0_RECT_TOP_CTRL) + hCompositor->ulRegOffset);
#else
				*pList->pulCurrent++ = BRDC_REGISTER(BCHP_CMP_0_V0_RECT_TOP_CTRL + hCompositor->ulRegOffset);
#endif
				*pList->pulCurrent++ = 0;
			}

			if(pList->bLastExecuted && hCompositor->ulMosaicAdjust)
			{
				hCompositor->ulMosaicAdjust--;
			}
		}
#endif
#endif

		if(hCompositor->ulCscAdjust[eV0Id])
		{
			BVDC_Window_Handle hWindow = hCompositor->ahWindow[eV0Id];
			BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

			if(hCompositor->bCscCompute[eV0Id])
			{
#if BVDC_P_SUPPORT_CMP_MOSAIC_CSC
				if(hCompositor->ahWindow[eV0Id]->stCurInfo.bClearRect)
				{
					BVDC_P_Window_CalculateMosaicCsc_isr(hCompositor->ahWindow[eV0Id]);
				}
				else
#endif
					BVDC_P_Window_CalculateCsc_isr(hCompositor->ahWindow[eV0Id]);
				hCompositor->bCscCompute[eV0Id] = false;
			}
			if(hCompositor->bCscDemoCompute[eV0Id])
			{
				BVDC_P_Window_SetSecCscDemo_isr(hCompositor->ahWindow[eV0Id]);
				hCompositor->bCscDemoCompute[eV0Id] = false;
			}

#if (BVDC_P_SUPPORT_CSC_MAT_COEF_VER >= 3)
#if BVDC_P_SUPPORT_CMP_MOSAIC_CSC
			/* TODO: Add dirty bits */
			/* Mosaic mode with CMP mosaic cscs. */
			if(hCompositor->ahWindow[eV0Id]->stCurInfo.bClearRect)
			{
				uint32_t i;
				int32_t ulBlockOffset = BVDC_P_REGS_ENTRIES(CMP_0_V0_R0_MC_COEFF_C00, CMP_0_V0_R0_MC_COEFF_C23) * sizeof(int32_t);

				for (i = 0; i < BVDC_P_SUPPORT_CMP_MOSAIC_CSC; i++)
				{
					BVDC_P_Window_BuildCscRul_isr(hCompositor, BCHP_CMP_0_V0_R0_MC_COEFF_C00 + (i * ulBlockOffset),
						&hWindow->astMosaicCscList[i], pList);
				}

				/* Write mosaic csc indices. */
				BVDC_P_CMP_WRITE_TO_RUL(CMP_0_V0_RECT_CSC_INDEX_0 , pList->pulCurrent);

				if(hCompositor->ahWindow[eV0Id]->stSettings.ulMaxMosaicRect > 8)
				{
					BVDC_P_CMP_WRITE_TO_RUL(CMP_0_V0_RECT_CSC_INDEX_1 , pList->pulCurrent);
				}
			}
			else
#endif
			{
#if BVDC_P_SUPPORT_CMP_NONLINEAR_CSC_WIN
				if (BVDC_P_WIN_IS_NONLINEAR_CSC_WIN(eV0Id))
				{
					BVDC_P_Window_BuildCscRul_isr(hCompositor, BCHP_CMP_0_V0_R0_MA_COEFF_C00, &hWindow->stPriCscA, pList);
					BVDC_P_Window_BuildCscRul_isr(hCompositor, BCHP_CMP_0_V0_R0_MB_COEFF_C00, &hWindow->stPriCscB, pList);
					BVDC_P_Window_BuildCscRul_isr(hCompositor, BCHP_CMP_0_V0_R0_MC_COEFF_C00, &hWindow->stPriCscC, pList);
				}
				else
#endif
				{
					BVDC_P_Window_BuildCscRul_isr(hCompositor, BCHP_CMP_0_V0_R0_MC_COEFF_C00, &hWindow->stPriCsc, pList);
				}
			}
#elif (BVDC_P_SUPPORT_CSC_MAT_COEF_VER >= 2)
#if BVDC_P_SUPPORT_CMP_NONLINEAR_CSC_WIN
			if (BVDC_P_WIN_IS_NONLINEAR_CSC_WIN(eV0Id))
			{
				BVDC_P_Window_BuildCscRul_isr(hCompositor, BCHP_CMP_0_V0_MA_COEFF_C00, &hWindow->stPriCscA, pList);
				BVDC_P_Window_BuildCscRul_isr(hCompositor, BCHP_CMP_0_V0_MB_COEFF_C00, &hWindow->stPriCscB, pList);
				BVDC_P_Window_BuildCscRul_isr(hCompositor, BCHP_CMP_0_V0_MC_COEFF_C00, &hWindow->stPriCscC, pList);
			}
			else
#endif
			{
				BVDC_P_Window_BuildCscRul_isr(hCompositor, BCHP_CMP_0_V0_MA_COEFF_C00, &hWindow->stPriCsc, pList);
			}
#elif (BVDC_P_SUPPORT_CSC_MAT_COEF_VER >= 1)
			BVDC_P_Window_BuildCscRul_isr(hCompositor, BCHP_CMP_0_V0_COEFF_C00, &hWindow->stPriCsc, pList);
#else
			BVDC_P_Window_BuildCscRul_isr(hCompositor, BCHP_CMP_0_V0_COEFF_C01_C00, &hWindow->stPriCsc, pList);
#endif

#if BVDC_P_SUPPORT_CMP_DEMO_MODE
			/* TODO: add support for CMP_1 demo mode 7401B0,C0 */
#if ((BCHP_CHIP == 7400) && (BCHP_VER >= BCHP_VER_B0)) || (BCHP_CHIP == 7420)
			if(eV0Id == BVDC_P_WindowId_eComp0_V0 ||
				eV0Id == BVDC_P_WindowId_eComp1_V0 ||
				eV0Id == BVDC_P_WindowId_eComp2_V0)
#else
			if(eV0Id == BVDC_P_WindowId_eComp0_V0)
#endif
			{
#if (BVDC_P_SUPPORT_CSC_MAT_COEF_VER >= 3)
#if BVDC_P_SUPPORT_CMP_NONLINEAR_CSC_WIN
				if (BVDC_P_WIN_IS_NONLINEAR_CSC_WIN(eV0Id))
				{
					BVDC_P_Window_BuildCscRul_isr(hCompositor, BCHP_CMP_0_V0_R1_MB_COEFF_C00, &hWindow->stSecCscB, pList);
					BVDC_P_Window_BuildCscRul_isr(hCompositor, BCHP_CMP_0_V0_R1_MC_COEFF_C00, &hWindow->stSecCscC, pList);
				}
				else
#endif
				{
					BVDC_P_Window_BuildCscRul_isr(hCompositor, BCHP_CMP_0_V0_R1_MC_COEFF_C00, &hWindow->stSecCsc, pList);
				}
#elif (BVDC_P_SUPPORT_CSC_MAT_COEF_VER >= 2)
#if BVDC_P_SUPPORT_CMP_NONLINEAR_CSC_WIN
				if (BVDC_P_WIN_IS_NONLINEAR_CSC_WIN(eV0Id))
				{
					BVDC_P_Window_BuildCscRul_isr(hCompositor, BCHP_CMP_0_SEC_V0_MB_COEFF_C00, &hWindow->stSecCscB, pList);
					BVDC_P_Window_BuildCscRul_isr(hCompositor, BCHP_CMP_0_SEC_V0_MC_COEFF_C00, &hWindow->stSecCscC, pList);
				}
				else
#endif
				{
					BVDC_P_Window_BuildCscRul_isr(hCompositor, BCHP_CMP_0_SEC_V0_MB_COEFF_C00, &hWindow->stSecCsc, pList);
				}
#elif (BVDC_P_SUPPORT_CSC_MAT_COEF_VER >= 1)
				BVDC_P_Window_BuildCscRul_isr(hCompositor, BCHP_CMP_0_SEC_V0_COEFF_C00, &hWindow->stSecCsc, pList);
#else
				BVDC_P_Window_BuildCscRul_isr(hCompositor, BCHP_CMP_0_SEC_V0_COEFF_C01_C00, &hWindow->stSecCsc, pList);
#endif
			}
#endif
			if(pList->bLastExecuted)
			{
				hCompositor->ulCscAdjust[eV0Id]--;
			}
		}
	}

	/* Lume key */
	if( hCompositor->ulLumaKeyAdjust[eV0Id] )
	{
		if(pList->bLastExecuted)
		{
			hCompositor->ulLumaKeyAdjust[eV0Id]--;
		}
		BVDC_P_CMP_WRITE_TO_RUL(CMP_0_V0_LUMA_KEYING, pList->pulCurrent);
	}

#if BVDC_P_CMP_0_MAX_VIDEO_WINDOW_COUNT > 1
	if(hCompositor->pFeatures->ulMaxVideoWindow > 1)
	{
		/* Vwin1 */
		if(BVDC_P_CMP_COMPARE_FIELD_DATA(CMP_0_V1_SURFACE_CTRL, SURFACE_ENABLE, 1))
		{
			if (((BVDC_P_CMP_GET_FIELD_NAME(CMP_0_V1_DISPLAY_SIZE, HSIZE) +
				  BVDC_P_CMP_GET_FIELD_NAME(CMP_0_V1_SURFACE_OFFSET, X_OFFSET)) >
				 (BVDC_P_CMP_GET_FIELD_NAME(CMP_0_V1_SURFACE_SIZE, HSIZE))) ||
				((BVDC_P_CMP_GET_FIELD_NAME(CMP_0_V1_DISPLAY_SIZE, VSIZE) +
				  BVDC_P_CMP_GET_FIELD_NAME(CMP_0_V1_SURFACE_OFFSET, Y_OFFSET)) >
				 (BVDC_P_CMP_GET_FIELD_NAME(CMP_0_V1_SURFACE_SIZE, VSIZE))))
			{
				uint32_t  ulDspW, ulCutLeft, ulDspH, ulCutTop;

				BDBG_ERR(("Invalid CMP V0 disp size [H:%d, V:%d] vs surf size [H:%d, V:%d] - [X:%d, Y:%d]",
					BVDC_P_CMP_GET_FIELD_NAME(CMP_0_V1_DISPLAY_SIZE, HSIZE),
					BVDC_P_CMP_GET_FIELD_NAME(CMP_0_V1_DISPLAY_SIZE, VSIZE),
					BVDC_P_CMP_GET_FIELD_NAME(CMP_0_V1_SURFACE_SIZE, HSIZE),
					BVDC_P_CMP_GET_FIELD_NAME(CMP_0_V1_SURFACE_SIZE, VSIZE),
					BVDC_P_CMP_GET_FIELD_NAME(CMP_0_V1_SURFACE_OFFSET, X_OFFSET),
					BVDC_P_CMP_GET_FIELD_NAME(CMP_0_V1_SURFACE_OFFSET, Y_OFFSET)));

				ulDspW = BVDC_P_CMP_GET_FIELD_NAME(CMP_0_V1_DISPLAY_SIZE, HSIZE);
				ulCutLeft = BVDC_P_CMP_GET_FIELD_NAME(CMP_0_V1_SURFACE_OFFSET, X_OFFSET);
				ulDspH = BVDC_P_CMP_GET_FIELD_NAME(CMP_0_V1_DISPLAY_SIZE, VSIZE);
				ulCutTop = BVDC_P_CMP_GET_FIELD_NAME(CMP_0_V1_SURFACE_OFFSET, Y_OFFSET);

				if ((BVDC_P_CMP_GET_FIELD_NAME(CMP_0_V1_DISPLAY_SIZE, HSIZE) +
					 BVDC_P_CMP_GET_FIELD_NAME(CMP_0_V1_SURFACE_OFFSET, X_OFFSET)) >
					(BVDC_P_CMP_GET_FIELD_NAME(CMP_0_V1_SURFACE_SIZE, HSIZE)))
				{
					ulDspW = BVDC_P_CMP_GET_FIELD_NAME(CMP_0_V1_SURFACE_SIZE, HSIZE);
					ulCutLeft = 0;
				}
				if ((BVDC_P_CMP_GET_FIELD_NAME(CMP_0_V1_DISPLAY_SIZE, VSIZE) +
					 BVDC_P_CMP_GET_FIELD_NAME(CMP_0_V1_SURFACE_OFFSET, Y_OFFSET)) >
					(BVDC_P_CMP_GET_FIELD_NAME(CMP_0_V1_SURFACE_SIZE, VSIZE)))
				{
					ulDspH = BVDC_P_CMP_GET_FIELD_NAME(CMP_0_V1_SURFACE_SIZE, VSIZE);
					ulCutTop = 0;
				}

				BVDC_P_CMP_GET_REG_DATA(CMP_0_V1_DISPLAY_SIZE) =
					BCHP_FIELD_DATA(CMP_0_V1_DISPLAY_SIZE, HSIZE, ulDspW) |
					BCHP_FIELD_DATA(CMP_0_V1_DISPLAY_SIZE, VSIZE, ulDspH);
				BVDC_P_CMP_GET_REG_DATA(CMP_0_V1_SURFACE_OFFSET) =
					BCHP_FIELD_DATA(CMP_0_V1_SURFACE_OFFSET, X_OFFSET, ulCutLeft) |
					BCHP_FIELD_DATA(CMP_0_V1_SURFACE_OFFSET, Y_OFFSET, ulCutTop);
			}

			BVDC_P_CMP_BLOCK_WRITE_TO_RUL(CMP_0_V1_SURFACE_SIZE, CMP_0_V1_CANVAS_OFFSET, pList->pulCurrent);
#if BVDC_P_SUPPORT_3D_VIDEO
			BVDC_P_CMP_WRITE_TO_RUL(CMP_0_V1_CANVAS_X_OFFSET_R, pList->pulCurrent);
#endif

#if (BVDC_P_SUPPORT_DITHER)
#if (BVDC_P_SUPPORT_CSC_MAT_COEF_VER == 2)
		BVDC_P_CMP_BLOCK_WRITE_TO_RUL(CMP_0_V1_DITHER_CTRL, CMP_0_V1_DITHER_LFSR_CTRL, pList->pulCurrent);
#else
		BVDC_P_CMP_BLOCK_WRITE_TO_RUL(CMP_0_V1_MC_DITHER_CTRL, CMP_0_V1_MC_DITHER_LFSR_CTRL, pList->pulCurrent);
#endif
#endif

#if BVDC_P_SUPPORT_CMP_CLEAR_RECT_V1
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)
		if(hCompositor->ulMosaicAdjust[eV1Id] || hCompositor->bInitial)
			{
				if(hCompositor->ahWindow[eV1Id]->stCurInfo.bClearRect)
				{
					BVDC_P_CMP_WRITE_TO_RUL(CMP_0_V1_RECT_COLOR, pList->pulCurrent);
					BVDC_P_CMP_BLOCK_WRITE_TO_RUL(CMP_0_V1_RECT_TOP_CTRL, CMP_0_V1_RECT_ENABLE_MASK, pList->pulCurrent);
					BVDC_P_CMP_RECT_BLOCK_WRITE_TO_RUL(CMP_0_V1_RECT_SIZEi_ARRAY_BASE,
						hCompositor->ahWindow[eV1Id]->stCurInfo.ulMosaicCount, pList->pulCurrent);
					BVDC_P_CMP_RECT_BLOCK_WRITE_TO_RUL(CMP_0_V1_RECT_OFFSETi_ARRAY_BASE,
						hCompositor->ahWindow[eV1Id]->stCurInfo.ulMosaicCount, pList->pulCurrent);
				}
				else
				{
					*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
#if (BVDC_P_SUPPORT_CSC_MAT_COEF_VER == 2)
					*pList->pulCurrent++ = BRDC_REGISTER( BVDC_P_Compositor_GetCmpRegAddr_isr(pCompsitor->eId, BCHP_CMP_0_V1_RECT_TOP_CTRL) + hCompositor->ulRegOffset);
#else
					*pList->pulCurrent++ = BRDC_REGISTER(BCHP_CMP_0_V1_RECT_TOP_CTRL + hCompositor->ulRegOffset);
#endif
					*pList->pulCurrent++ = 0;
				}

				if(pList->bLastExecuted && hCompositor->ulMosaicAdjust[eV1Id])
				{
					hCompositor->ulMosaicAdjust[eV1Id]--;
				}
			}
#else
		if(hCompositor->ulMosaicAdjust || hCompositor->bInitial)
			{
				if(hCompositor->ahWindow[eV1Id]->stCurInfo.bClearRect)
				{
					BVDC_P_CMP_WRITE_TO_RUL(CMP_0_V1_RECT_COLOR, pList->pulCurrent);
					BVDC_P_CMP_BLOCK_WRITE_TO_RUL(CMP_0_V1_RECT_TOP_CTRL, CMP_0_V1_RECT_ENABLE_MASK, pList->pulCurrent);
					BVDC_P_CMP_RECT_BLOCK_WRITE_TO_RUL(CMP_0_V1_RECT_SIZEi_ARRAY_BASE,
						hCompositor->ahWindow[eV1Id]->stCurInfo.ulMosaicCount, pList->pulCurrent);
					BVDC_P_CMP_RECT_BLOCK_WRITE_TO_RUL(CMP_0_V1_RECT_OFFSETi_ARRAY_BASE,
						hCompositor->ahWindow[eV1Id]->stCurInfo.ulMosaicCount, pList->pulCurrent);
				}
				else
				{
					*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
#if (BVDC_P_SUPPORT_CSC_MAT_COEF_VER == 2)
					*pList->pulCurrent++ = BRDC_REGISTER( BVDC_P_Compositor_GetCmpRegAddr_isr(pCompsitor->eId, BCHP_CMP_0_V1_RECT_TOP_CTRL) + hCompositor->ulRegOffset);
#else
					*pList->pulCurrent++ = BRDC_REGISTER(BCHP_CMP_0_V1_RECT_TOP_CTRL + hCompositor->ulRegOffset);
#endif
					*pList->pulCurrent++ = 0;
				}

				if(pList->bLastExecuted && hCompositor->ulMosaicAdjust)
				{
					hCompositor->ulMosaicAdjust--;
				}
			}
#endif
#endif
			if(hCompositor->ulCscAdjust[eV1Id])
			{
				BVDC_Window_Handle hWindow = hCompositor->ahWindow[eV1Id];
				BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

				if(hCompositor->bCscCompute[eV1Id])
				{
#if BVDC_P_SUPPORT_CMP_MOSAIC_CSC
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)
					if(hCompositor->ahWindow[eV0Id]->stCurInfo.bClearRect)
						BVDC_P_Window_CalculateMosaicCsc_isr(hCompositor->ahWindow[eV0Id]);
#else
					if(hCompositor->ahWindow[eV1Id]->stCurInfo.bClearRect)
						BVDC_P_Window_CalculateMosaicCsc_isr(hCompositor->ahWindow[eV1Id]);
#endif
					else
#endif
						BVDC_P_Window_CalculateCsc_isr(hCompositor->ahWindow[eV1Id]);
					hCompositor->bCscCompute[eV1Id] = false;
				}
				if(hCompositor->bCscDemoCompute[eV1Id])
				{
					BVDC_P_Window_SetSecCscDemo_isr(hCompositor->ahWindow[eV1Id]);
					hCompositor->bCscDemoCompute[eV1Id] = false;
				}

#if (BVDC_P_SUPPORT_CSC_MAT_COEF_VER >= 3)
#if BVDC_P_SUPPORT_CMP_MOSAIC_CSC
				/* TODO: Add dirty bits */
				/* Mosaic mode with CMP mosaic cscs. */
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)
				if(hCompositor->ahWindow[eV1Id]->stCurInfo.bClearRect)
#else
				if(hCompositor->ahWindow[eV0Id]->stCurInfo.bClearRect)
#endif
				{
					uint32_t i;
					int32_t ulBlockOffset = BVDC_P_REGS_ENTRIES(CMP_0_V1_R0_MC_COEFF_C00, CMP_0_V1_R0_MC_COEFF_C23) * sizeof(int32_t);

					for (i = 0; i < BVDC_P_SUPPORT_CMP_MOSAIC_CSC; i++)
					{
						BVDC_P_Window_BuildCscRul_isr(hCompositor, BCHP_CMP_0_V1_R0_MC_COEFF_C00 + (i * ulBlockOffset),
							&hWindow->astMosaicCscList[i], pList);
					}

					/* Write mosaic csc indices. */
					BVDC_P_CMP_WRITE_TO_RUL(CMP_0_V1_RECT_CSC_INDEX_0 , pList->pulCurrent);
				}
				else
#endif
				{
					/* TODO: add support for 7420 here */
					BVDC_P_Window_BuildCscRul_isr(hCompositor, BCHP_CMP_0_V1_R0_MC_COEFF_C00, &hWindow->stPriCsc, pList);
#if BVDC_P_SUPPORT_CMP_MOSAIC_CSC
					BVDC_P_Window_BuildCscRul_isr(hCompositor, BCHP_CMP_0_V1_R1_MC_COEFF_C00, &hWindow->stSecCsc, pList);
#endif
				}
#elif (BVDC_P_SUPPORT_CSC_MAT_COEF_VER >= 1)
				BVDC_P_Window_BuildCscRul_isr(hCompositor, BCHP_CMP_0_V1_COEFF_C00, &hWindow->stPriCsc, pList);
#else
				BVDC_P_Window_BuildCscRul_isr(hCompositor, BCHP_CMP_0_V1_COEFF_C01_C00, &hWindow->stPriCsc, pList);
#endif

				if(pList->bLastExecuted)
				{
					hCompositor->ulCscAdjust[eV1Id]--;
				}
			}
		}

		/* Lume key */
		if( hCompositor->ulLumaKeyAdjust[eV1Id] )
		{
			if(pList->bLastExecuted)
			{
				hCompositor->ulLumaKeyAdjust[eV1Id]--;
			}
			BVDC_P_CMP_WRITE_TO_RUL(CMP_0_V1_LUMA_KEYING, pList->pulCurrent);
		}
	}
#endif

	/* Gwin0 */
	if(BVDC_P_CMP_COMPARE_FIELD_DATA(CMP_0_G0_SURFACE_CTRL, ENABLE, 1))
	{
		BVDC_P_CMP_BLOCK_WRITE_TO_RUL(CMP_0_G0_SURFACE_SIZE, CMP_0_G0_CANVAS_OFFSET, pList->pulCurrent);
#if BVDC_P_SUPPORT_3D_VIDEO
		BVDC_P_CMP_WRITE_TO_RUL(CMP_0_G0_CANVAS_X_OFFSET_R, pList->pulCurrent);
#endif
	}

#if BVDC_P_CMP_0_MAX_GFX_WINDOW_COUNT > 1
	if(hCompositor->pFeatures->ulMaxGfxWindow > 1)
	{
		/* Gwin1 */
		if(BVDC_P_CMP_COMPARE_FIELD_DATA(CMP_0_G1_SURFACE_CTRL, ENABLE, 1))
		{
			BVDC_P_CMP_BLOCK_WRITE_TO_RUL(CMP_0_G1_SURFACE_SIZE, CMP_0_G1_CANVAS_OFFSET, pList->pulCurrent);
		}
	}
#endif

#if BVDC_P_CMP_0_MAX_GFX_WINDOW_COUNT > 2
	if(hCompositor->pFeatures->ulMaxGfxWindow > 2)
	{
		/* Gwin2 */
		if(BVDC_P_CMP_COMPARE_FIELD_DATA(CMP_0_G2_SURFACE_CTRL, ENABLE, 1))
		{
			BVDC_P_CMP_BLOCK_WRITE_TO_RUL(CMP_0_G2_SURFACE_SIZE, CMP_0_G2_CANVAS_OFFSET, pList->pulCurrent);
		}
	}
#endif

#if (BVDC_P_SUPPORT_CSC_MAT_COEF_VER >= 2)
	if(hCompositor->stCurInfo.stDirty.stBits.bColorClip || hCompositor->bInitial)
	{
		/* CMP color clip configuration is only supported by compositor 0 of certain
		 * chipsets.
		 */
#if (BVDC_P_SUPPORT_COLOR_CLIP)
		if(hCompositor->eId == BVDC_CompositorId_eCompositor0)
			BVDC_P_Compositor_BuildColorClipRul_isr(hCompositor, pList);
#endif
		hCompositor->stCurInfo.stDirty.stBits.bColorClip = BVDC_P_CLEAN;
	}
#endif

	if(hCompositor->bInitial)
	{
		hCompositor->bInitial = false;
	}

	BDBG_LEAVE(BVDC_P_Compositor_BuildRul_isr);
	return;
}


/***************************************************************************
 * {private}
 *
 */
void BVDC_P_Compositor_BuildSyncLockRul_isr
	( BVDC_Compositor_Handle           hCompositor,
	  BVDC_P_ListInfo                 *pList,
	  BAVC_Polarity                    eFieldId )
{
	int i;
	bool abForceSlot[BVDC_P_MAX_SOURCE_COUNT];
	BDBG_ENTER(BVDC_P_Compositor_BuildSyncLockRul_isr);
	BDBG_OBJECT_ASSERT(hCompositor, BVDC_CMP);

	hCompositor->hForceTrigPipSrc = NULL;

	/* to handle multiple windows coming from the same mpg source */
	for(i = 0; i < BVDC_P_MAX_SOURCE_COUNT; i++)
	{
		abForceSlot[i] = false;
	}

	/* Add an entry into the RUL with sole purpose of force trigger the
	 * the source slot.  Take advantage of the way register layout, each
	 * slot immediate trigger is 16 bytes apart.
	 *
	 * There are two cases where we want to force trigger the source's
	 * slot RUL.
	 *
	 * (1) The mpeg sync-lock source the LOCKED to this compositor.
	 * (2) The other mpeg source's that hasn't connected to any compositor. */
	for(i = 0; i < BVDC_P_MAX_WINDOW_COUNT; i++)
	{
		BVDC_Source_Handle hSource;

		/* SKIP: If it's just created or inactive no need to build ruls. */
		if(!hCompositor->ahWindow[i] ||
			BVDC_P_STATE_IS_CREATE(hCompositor->ahWindow[i]) ||
			BVDC_P_STATE_IS_INACTIVE(hCompositor->ahWindow[i]))
		{
			continue;
		}

		BDBG_OBJECT_ASSERT(hCompositor->ahWindow[i], BVDC_WIN);
		if((hCompositor->ahWindow[i]->bUserAppliedChanges) &&
		   (hCompositor->ahWindow[i]->stCurInfo.hSource == NULL))
		{
			hSource = hCompositor->ahWindow[i]->stNewInfo.hSource;
		}
		else
		{
			hSource = hCompositor->ahWindow[i]->stCurInfo.hSource;
		}
		BDBG_OBJECT_ASSERT(hSource, BVDC_SRC);

		/* Force trigger mpeg slot:
		 * 1) MPEG Sync-lock source required force trigger.
		 * 2) MPEG PIP source not locking to any compositor yet. */
		if((BVDC_P_SRC_IS_MPEG(hSource->eId)) &&
		   (!abForceSlot[hSource->eId]) &&
		   ((hSource->hSyncLockCompositor == hCompositor) ||
		    (hSource->hSyncLockCompositor == NULL)))
		{
			/* Note, we invert the field polarity if it's locked with a captured mpeg src
			 *   to adapt to 2-field multi-buffering, which puts read pointer one field
			 *   behind write pointer; the trigger field swap only makes sense with
			 *   active window and interlaced formats;
			 * PR43082: This flag needs to reflect the total BVN delay of the sync-locked
			 * window to achieve precise lipsync goal with HD/SD simul mode; */
			/* FIELDSWAP 1) detection; */
			if(BVDC_P_STATE_IS_ACTIVE(hCompositor->hSyncLockWin))
			{
				hSource->bFieldSwap = (hCompositor->hSyncLockWin->ulTotalBvnDelay & 1);
				BDBG_MSG(("CMP[%d], SRC[%d], WIN[%d], bFieldSwap[%d], ulTotalBvnDelay[%d]",
					hCompositor->eId,
					hSource->eId,
					hCompositor->hSyncLockWin->eId,
					hSource->bFieldSwap,
					hCompositor->hSyncLockWin->ulTotalBvnDelay));
			}

			/* Force source slot to execute. */
			if((hSource->hSyncLockCompositor == NULL) && (hSource->ulTransferLock))
			{
				BDBG_MSG(("Don't force trigger the PIP window yet"));
				BVDC_P_BUILD_NO_OPS(pList->pulCurrent);

				/* Note: this is a backup to clear the transfer lock semaphore, in case
				   the critical source _isr, responsible to clear semaphore, is called
				   too early (_isr call disorder issue) such that it doesn't see the
				   semaphore, and source disconnection display _isr sets semaphore later
				   and stops building force trigger RUL for that mpeg source; that mpeg
				   source could lose driving interrupt forever! */
				if(--hSource->ulTransferLock == 0)
				{
					/* clean up source slots to prepare for the trigger transfer */
					BVDC_P_Source_CleanupSlots_isr(hSource);
				}
				BDBG_MSG(("CMP[%d] decrements SRC[%d]'s ulTransferLock semaphore count %d",
					hCompositor->eId, hSource->eId, hSource->ulTransferLock));
			}
			else
			{
				abForceSlot[hSource->eId] = true;
				/* FIELDSWAP 3) force field swapped trigger; */
				BVDC_P_BUILD_IMM_EXEC_OPS(pList->pulCurrent, hSource->aulImmTriggerAddr[
					hSource->bFieldSwap ? BVDC_P_NEXT_POLARITY(eFieldId) : eFieldId]);

				/* PsF: source refresh rate is frame rate */
				/* The refresh rate of this source is driven by refresh rate
				 * of this display */
				if(!hSource->stCurInfo.bPsfEnable)
				{
					/* this value may differentiate 59.94 vs 60 for HD formats that support
					    multiple frame rates. */
					hSource->ulVertFreq = hCompositor->hDisplay->stCurInfo.ulVertFreq;
				}
				hSource->ulDispVsyncFreq = hCompositor->hDisplay->stCurInfo.ulVertFreq;
			}

			/* Assume:
			   1) two mpg sources at most in system;
			   2) mpg source can only be sync slipped if it's in the PIP of a sync-locked
			      compositor; */
			if(hSource->hSyncLockCompositor == NULL)
			{
				hCompositor->hForceTrigPipSrc = hSource;
			}
		}
	}

	BDBG_LEAVE(BVDC_P_Compositor_BuildSyncLockRul_isr);
	return;
}


/***************************************************************************
 * {private}
 *
 */
bool BVDC_P_Compositor_BuildSyncSlipRul_isr
	( BVDC_Compositor_Handle           hCompositor,
	  BVDC_P_ListInfo                 *pList,
	  BAVC_Polarity                    eFieldId,
	  bool                             bBuildCanvasCtrl )
{
	int i;
	bool bBuilldBothTopBotSlots = false;

	BDBG_ENTER(BVDC_P_Compositor_BuildSyncSlipRul_isr);
	BDBG_OBJECT_ASSERT(hCompositor, BVDC_CMP);
	BDBG_OBJECT_ASSERT(hCompositor->hDisplay, BVDC_DSP);

	/* Now update windows state (including setInfo) for next picture to be displayed. */
	BVDC_P_Compositor_WindowsReader_isr(hCompositor, eFieldId, pList);

	/* Build sync slip Rul.  Basically this build playback portion of
	 * RUL.  Most likely it will be the VEC, CMP, VFDs, GFD, [SCLs], and
	 * VNET.  If no window is active (just bgcolor) it will just build the
	 * VEC, CMP, and VNET. */
	BVDC_P_Vec_BuildRul_isr(hCompositor->hDisplay, pList, eFieldId);

	/* Build Compositor RUL */
	BVDC_P_Compositor_BuildRul_isr(hCompositor, pList, eFieldId);

	/* Build related video backend blocks that associated with active windows.
	 * Only build the READER portion of the windows. */
	for(i = 0; i < BVDC_P_MAX_WINDOW_COUNT; i++)
	{
		/* SKIP: If it's just created or inactive no need to build ruls. */
		if(!hCompositor->ahWindow[i] ||
		   BVDC_P_STATE_IS_CREATE(hCompositor->ahWindow[i]) ||
		   BVDC_P_STATE_IS_INACTIVE(hCompositor->ahWindow[i]))
		{
			continue;
		}

		BDBG_OBJECT_ASSERT(hCompositor->ahWindow[i], BVDC_WIN);
		/* we need to keep building the sliped window and the newly
		 * transfered locked window on the consistent vec trigger; */
		if(!hCompositor->ahWindow[i]->bSyncLockSrc || hCompositor->ulSlip2Lock)
		{
			/* Note: sync-slipped window reader shutdown needs to build both slots to
			 * make both top/bot slots consistent in case next time the isr is not
			 * serviced on time, the staled playback RUL could unmute when the
			 * capture side is disconnecting vnet;  */
			if(!hCompositor->ahWindow[i]->bSyncLockSrc &&
			   BVDC_P_SRC_IS_VIDEO(hCompositor->ahWindow[i]->stCurInfo.hSource->eId))
			{
				bBuilldBothTopBotSlots |= (BVDC_P_State_eShutDownRul == hCompositor->ahWindow[i]->stCurInfo.eReaderState);
			}

			/* Only need to build the READER part of the slip. */
			BVDC_P_Window_BuildRul_isr(hCompositor->ahWindow[i], pList, eFieldId,
				false, /* writer*/
				true,  /* reader */
				false  /* CanvasCtrl */ );
		}
	}

	/* convas should not be enabled until all surfaces are configured and enabled,
	 * notice that syncLock window RUL is built after BuildSyncSlipRul_isr is called */
	if (bBuildCanvasCtrl)
	{
		BVDC_P_CMP_WRITE_TO_RUL(CMP_0_CANVAS_CTRL, pList->pulCurrent);
	}

	BDBG_LEAVE(BVDC_P_Compositor_BuildSyncSlipRul_isr);
	return bBuilldBothTopBotSlots;
}

/***************************************************************************
 * {private}
 *
 * to be called for syncLock case since syncLock window RUL is built after
 * BuildSyncSlipRul_isr is called
 */
void BVDC_P_Compositor_BuildConvasCtrlRul_isr
	( BVDC_Compositor_Handle           hCompositor,
	  BVDC_P_ListInfo                 *pList )
{
	BDBG_ENTER(BVDC_P_Compositor_BuildConvasCtrlRul_isr);
	BDBG_OBJECT_ASSERT(hCompositor, BVDC_CMP);

	BVDC_P_CMP_WRITE_TO_RUL(CMP_0_CANVAS_CTRL, pList->pulCurrent);

	BDBG_LEAVE(BVDC_P_Compositor_BuildConvasCtrlRul_isr);
}

/***************************************************************************
 * {private}
 *
 */
#if !BVDC_P_ORTHOGONAL_VEC
BERR_Code BVDC_P_Compositor_GetOutputInfo_isr
	( const BVDC_Compositor_Handle     hCompositor,
	  bool                            *pbFullRate,
	  BVDC_PhaseAdjustMode            *pePhaseAdjustMode,
	  uint32_t                        *pulVdecPhase,
	  bool                            *pb240pMode )
{
	BDBG_ENTER(BVDC_P_Compositor_GetOutputInfo_isr);
	BDBG_OBJECT_ASSERT(hCompositor, BVDC_CMP);

	if(pbFullRate)
	{
		*pbFullRate = hCompositor->bFullRate;
	}

	if(pePhaseAdjustMode)
	{
		*pePhaseAdjustMode = hCompositor->ePhaseAdjustMode;
	}

	if(pulVdecPhase)
	{
		*pulVdecPhase = hCompositor->ulVdecPhase;
	}

	if(pb240pMode)
	{
		*pb240pMode = hCompositor->b240pMode;
	}

	BDBG_LEAVE(BVDC_P_Compositor_GetOutputInfo_isr);
	return BERR_SUCCESS;
}
#endif

/***************************************************************************
 * {private}
 *
 */
BERR_Code BVDC_P_Compositor_AssignTrigger_isr
	( BVDC_Compositor_Handle           hCompositor,
	  BRDC_Trigger                     eTopTrigger,
	  BRDC_Trigger                     eBotTrigger )
{
	BDBG_ENTER(BVDC_P_Compositor_AssignTrigger_isr);
	BDBG_OBJECT_ASSERT(hCompositor, BVDC_CMP);

	BRDC_Slot_ExecuteOnTrigger_isr(hCompositor->ahSlot[BAVC_Polarity_eTopField],
		eTopTrigger, true);
	BRDC_Slot_ExecuteOnTrigger_isr(hCompositor->ahSlot[BAVC_Polarity_eBotField],
		eBotTrigger, true);

	BDBG_LEAVE(BVDC_P_Compositor_AssignTrigger_isr);
	return BERR_SUCCESS;
}

/**************************************************************************
 * This function will call the READER of all the windows hold by this compositor,
 * and determine the input color space to VEC, also select the video window
 * color space conversion matrix if necessary.
 */
void BVDC_P_Compositor_WindowsReader_isr
	( BVDC_Compositor_Handle           hCompositor,
	  BAVC_Polarity                    eNextFieldId,
	  BVDC_P_ListInfo                 *pList )
{
	int                       i;
	uint32_t                  ulVSize, ulHSize;
	BVDC_Window_Handle        hWindow;
	bool                      bWidthTrim = true;
	BVDC_DisplayTg            eMasterTg;
	bool                      bDTg ;
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)
	bool					  bBgCsc = false;
#endif

#if BVDC_P_ORTHOGONAL_VEC
	BVDC_P_Display_SrcInfo    stSrcInfo;
#endif
	bool bOutputXvYcc =       hCompositor->hDisplay->stCurInfo.bXvYcc;
	const BFMT_VideoInfo     *pFmtInfo;

	BDBG_OBJECT_ASSERT(hCompositor, BVDC_CMP);

	/* hack to disable xvYCC CMP output and always load non xvYCC Matrix C */
	/* bOutputXvYcc = true   : always load special xvYCC matrices */
	/* bOutputXvYcc = false  : don't load special xvYCC matrices */
	if(!hCompositor->bIsBypass)
	{
		hCompositor->eCmpColorSpace = VIDEO_FORMAT_CMP_COLORSPACE(hCompositor->stCurInfo.pFmtInfo->eVideoFmt, bOutputXvYcc);
	}
	else
	{
		/* set up cmp colorspace for bypass 656.
		   the color space conversion would be done inside 656 encoder. */
		for(i = 0; i < BVDC_P_MAX_WINDOW_COUNT; i++)
		{
			hWindow = hCompositor->ahWindow[i];
			BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

			if(BVDC_P_WIN_IS_VIDEO_WINDOW(hWindow->eId))
			{
				switch(hWindow->hBuffer->pCurReaderBuf->eMatrixCoefficients)
				{
				/* HD */
				case BAVC_MatrixCoefficients_eItu_R_BT_709:
					hCompositor->eCmpColorSpace = BVDC_P_CmpColorSpace_eHdYCrCb;
					break;
				/* 1953 NTSC SD */
				case BAVC_MatrixCoefficients_eFCC:
					hCompositor->eCmpColorSpace = BVDC_P_CmpColorSpace_eFcc;
					break;
				/* PAL SD */
				case BAVC_MatrixCoefficients_eItu_R_BT_470_2_BG:
					hCompositor->eCmpColorSpace = BVDC_P_CmpColorSpace_eNtscSdYCrCb;
					break;
				/* NTSC SD */
				case BAVC_MatrixCoefficients_eSmpte_170M:
					hCompositor->eCmpColorSpace = BVDC_P_CmpColorSpace_ePalSdYCrCb;
					break;
				/* 1987 ATSC HD */
				case BAVC_MatrixCoefficients_eSmpte_240M:
					hCompositor->eCmpColorSpace = BVDC_P_CmpColorSpace_eSmpte_240M;
					break;
				default:
					hCompositor->eCmpColorSpace = BVDC_P_CmpColorSpace_eHdYCrCb;
				}

				break;
			}
			else
			{
				hCompositor->eCmpColorSpace = VIDEO_FORMAT_27Mhz(hCompositor->stCurInfo.pFmtInfo->ulPxlFreqMask)
														? BVDC_P_CmpColorSpace_eNtscSdYCrCb : BVDC_P_CmpColorSpace_eHdYCrCb;
			}
		}
	}

	/* second pass: to adjust non-vbi-pass-thru window position;
	   Note: adjustment is done in the second pass in case vwin0 has no pass-thru,
	         but vwin1 has; */
	for(i = 0; i < BVDC_P_MAX_WINDOW_COUNT; i++)
	{
		/* SKIP: If it's just created or inactive no need to program. */
		if(!hCompositor->ahWindow[i] ||
			BVDC_P_STATE_IS_CREATE(hCompositor->ahWindow[i]) ||
			BVDC_P_STATE_IS_INACTIVE(hCompositor->ahWindow[i]))
		{
			continue;
		}

		hWindow = hCompositor->ahWindow[i];
		BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
		BVDC_P_Window_Reader_isr(hCompositor->ahWindow[i], eNextFieldId, pList);

		/* turn off 704-sample feature if any window is larger than 704 width */
		if(BVDC_P_WIN_GET_FIELD_NAME(CMP_0_V0_DISPLAY_SIZE, HSIZE) > 704)
		{
			bWidthTrim = false;
		}
	}

	/* Turn off 704-sample feature if DCS. */
	/* TODO: remove this restriction */
#if DCS_SUPPORT
	if (
		VIDEO_FORMAT_SUPPORTS_DCS (
			hCompositor->stCurInfo.pFmtInfo->eVideoFmt))
	{
		bWidthTrim = false;
	}
#endif

	/* We do not have a 704-sample ARIB array, so this is another case where we
	 * have to remove this feature */
	if (hCompositor->hDisplay->bArib480p)
	{
		bWidthTrim = false;
	}

	/* set up display sourceinfo */
	/* detect SD 704-sample input: black at both edges, all windows are <= 704 width; */
	bWidthTrim = (
		(bWidthTrim) &&
		(hCompositor->stCurInfo.ulBgColorYCrCb == BVDC_P_YCRCB_BLACK) &&
		(VIDEO_FORMAT_IS_SD(hCompositor->stCurInfo.pFmtInfo->eVideoFmt)));

#if BVDC_P_ORTHOGONAL_VEC
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)
	bBgCsc = (hCompositor->hDisplay->stCurInfo.eCmpColorSpace != hCompositor->eCmpColorSpace);
#endif
	stSrcInfo.bWidthTrimmed = bWidthTrim;
	stSrcInfo.eCmpColorSpace = hCompositor->eCmpColorSpace;
	stSrcInfo.b240pMode = hCompositor->b240pMode;
	stSrcInfo.ePhaseAdjustMode = hCompositor->ePhaseAdjustMode;
	stSrcInfo.ulVdecPhase = hCompositor->ulVdecPhase;
	stSrcInfo.bFullRate = hCompositor->bFullRate;
	BVDC_P_Display_SetSourceInfo(hCompositor->hDisplay, &stSrcInfo);
#else
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)
	bBgCsc = (hCompositor->hDisplay->stNewInfo.eCmpColorSpace != hCompositor->eCmpColorSpace);
#endif
	if (hCompositor->hDisplay->stCurInfo.bWidthTrimmed != bWidthTrim)
	{
		hCompositor->hDisplay->stCurInfo.stDirty.stBits.bWidthTrim = BVDC_P_DIRTY;
	}
	hCompositor->hDisplay->stCurInfo.bWidthTrimmed = bWidthTrim;
	hCompositor->hDisplay->stNewInfo.bWidthTrimmed = bWidthTrim;
	hCompositor->hDisplay->stNewInfo.eCmpColorSpace = hCompositor->eCmpColorSpace;
#endif

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)
	/* Update BG colorspace to match base on src/disp changes */
	if(bBgCsc)
	{
		uint32_t ulBgColorYCrCb;
		unsigned int uiARGB = BPXL_MAKE_PIXEL(BPXL_eA8_R8_G8_B8, 0x00,
			hCompositor->stCurInfo.ucRed,
			hCompositor->stCurInfo.ucGreen,
			hCompositor->stCurInfo.ucBlue);

		if((BVDC_P_CmpColorSpace_eHdYCrCb != hCompositor->eCmpColorSpace) &&
		   (BVDC_P_CmpColorSpace_eSmpte_240M != hCompositor->eCmpColorSpace))
		{
			BPXL_ConvertPixel_RGBtoYCbCr(BPXL_eA8_Y8_Cb8_Cr8, BPXL_eA8_R8_G8_B8,
				uiARGB, (unsigned int*)&ulBgColorYCrCb);
		}
		else
		{
			BPXL_ConvertPixel_RGBtoHdYCbCr(BPXL_eA8_Y8_Cb8_Cr8, BPXL_eA8_R8_G8_B8,
				uiARGB, (unsigned int*)&ulBgColorYCrCb);
		}

		hCompositor->stNewInfo.ulBgColorYCrCb = ulBgColorYCrCb;
		hCompositor->stCurInfo.ulBgColorYCrCb = ulBgColorYCrCb;
		BVDC_P_CMP_SET_REG_DATA(CMP_0_BG_COLOR, ulBgColorYCrCb);
	}
#endif
	/* set compositor size -- number of lines. */
	eMasterTg = hCompositor->hDisplay->eMasterTg;
	bDTg      =  BVDC_P_DISPLAY_USED_DIGTRIG(eMasterTg);


	pFmtInfo = hCompositor->stCurInfo.pFmtInfo;

#if BVDC_P_SUPPORT_STG
	if(hCompositor->ulDelayCnt)
	{
		pFmtInfo = hCompositor->hDisplay->pPrevFmtInfo;
	}
	if((hCompositor->bInitial) && 
		BVDC_P_DISPLAY_USED_STG(eMasterTg))
		pFmtInfo = hCompositor->stNewInfo.pFmtInfo;
#endif

	ulHSize   = bDTg? pFmtInfo->ulDigitalWidth :pFmtInfo->ulWidth;
	ulVSize   = bDTg? pFmtInfo->ulDigitalHeight:pFmtInfo->ulHeight;
	ulVSize >>= (eNextFieldId != BAVC_Polarity_eFrame);
	BDBG_MODULE_MSG(BVDC_CMP_SIZE,("Canvas %d Delay %d %4d x %4d", hCompositor->eId, hCompositor->ulDelayCnt, ulHSize, ulVSize));
	/* Handle 3d case */
	if(!BFMT_IS_3D_MODE(hCompositor->stCurInfo.pFmtInfo->eVideoFmt))
	{
		if(hCompositor->stCurInfo.eOrientation == BFMT_Orientation_e3D_LeftRight)
		{
			ulHSize = ulHSize / 2;
		}
		else if(hCompositor->stCurInfo.eOrientation == BFMT_Orientation_e3D_OverUnder)
		{
			ulVSize = ulVSize / 2;
		}
	}

	BVDC_P_CMP_GET_REG_DATA(CMP_0_CANVAS_SIZE) = (
		BCHP_FIELD_DATA(CMP_0_CANVAS_SIZE, HSIZE, ulHSize) |
		BCHP_FIELD_DATA(CMP_0_CANVAS_SIZE, VSIZE, ulVSize));

#if BVDC_P_SUPPORT_3D_VIDEO
	{
		BFMT_Orientation eOrientation;
		if(BFMT_IS_3D_MODE(hCompositor->stCurInfo.pFmtInfo->eVideoFmt))
			eOrientation = hCompositor->stCurInfo.pFmtInfo->eOrientation;
		else
			eOrientation = hCompositor->stCurInfo.eOrientation;

		BVDC_P_CMP_GET_REG_DATA(CMP_0_CMP_CTRL) = (
			BCHP_FIELD_DATA(CMP_0_CMP_CTRL, BVB_VIDEO, eOrientation));
	}
#endif

	/* Luma Average for this compositor. */
#if (BVDC_P_SUPPORT_CMP_LUMA_AVG_VER > 0)
	{
		BVDC_P_Rect stRect;

		BVDC_P_CalculateRect_isr(&hCompositor->stCurInfo.stLumaRect.stRegion,
			ulHSize,
			(bDTg? hCompositor->stCurInfo.pFmtInfo->ulDigitalHeight:
			hCompositor->stCurInfo.pFmtInfo->ulHeight),
			hCompositor->stCurInfo.pFmtInfo->bInterlaced,
			&stRect);

	BVDC_P_CMP_GET_REG_DATA(CMP_0_LUMA_AVG_CTRL) = (
		BCHP_FIELD_DATA(CMP_0_LUMA_AVG_CTRL, V0_LUMA_AVG_CTRL,      1) |
		BCHP_FIELD_DATA(CMP_0_LUMA_AVG_CTRL, CMP_OUT_LUMA_AVG_CTRL, 1));

	BVDC_P_CMP_GET_REG_DATA(CMP_0_CMP_LUMA_AVG_WIN_OFFSET) = (
		BCHP_FIELD_DATA(CMP_0_CMP_LUMA_AVG_WIN_OFFSET, X_OFFSET, stRect.lLeft) |
		BCHP_FIELD_DATA(CMP_0_CMP_LUMA_AVG_WIN_OFFSET, Y_OFFSET, stRect.lTop));
	BVDC_P_CMP_GET_REG_DATA(CMP_0_CMP_LUMA_AVG_WIN_SIZE) = (
		BCHP_FIELD_DATA(CMP_0_CMP_LUMA_AVG_WIN_SIZE, HSIZE, stRect.ulWidth) |
		BCHP_FIELD_DATA(CMP_0_CMP_LUMA_AVG_WIN_SIZE, VSIZE, stRect.ulHeight));
}
#endif

	BDBG_LEAVE(BVDC_P_Compositor_WindowsReader_isr);
	return;
}

void BVDC_P_Compositor_SetMBoxMetaData_isr
	( const BVDC_P_PictureNode        *pPicture,
	  BVDC_Compositor_Handle           hCompositor )
{
#if (BVDC_P_SUPPORT_STG)
	BDBG_ENTER(BVDC_P_Compositor_SetMBoxMetaData);
	BDBG_OBJECT_ASSERT(hCompositor,     BVDC_CMP);

	if(NULL!=pPicture)
	{
		hCompositor->ulOrigPTS               = pPicture->ulOrigPTS;

		hCompositor->uiHorizontalPanScan     = pPicture->iHorzPanScan;
		hCompositor->uiVerticalPanScan       = pPicture->iVertPanScan;

		hCompositor->ulDisplayHorizontalSize = pPicture->ulDispHorzSize;
		hCompositor->ulDisplayVerticalSize   = pPicture->ulDispVertSize;

		hCompositor->ePictureType            = pPicture->ePictureType;

		hCompositor->eSourcePolarity         = pPicture->eSrcPolarity;
		hCompositor->bPictureRepeatFlag      = pPicture->stFlags.bPictureRepeatFlag;
		hCompositor->bIgnorePicture          = pPicture->bIgnorePicture;
		hCompositor->bStallStc               = pPicture->bStallStc;
		hCompositor->bLast                   = pPicture->bLast;
		hCompositor->bChannelChange          = pPicture->bChannelChange;
		hCompositor->ulStgPxlAspRatio_x_y    = pPicture->ulStgPxlAspRatio_x_y;
		hCompositor->ulDecodePictureId       = pPicture->ulDecodePictureId;
		hCompositor->bValidAfd               = pPicture->bValidAfd;
		hCompositor->ulAfd                   = pPicture->ulAfd;
		hCompositor->eBarDataType            = pPicture->eBarDataType;
		hCompositor->ulTopLeftBarValue       = pPicture->ulTopLeftBarValue;
		hCompositor->ulBotRightBarValue      = pPicture->ulBotRightBarValue;
		hCompositor->bPreChargePicture       = pPicture->bPreChargePicture;
		hCompositor->bEndofChunk             = pPicture->bEndofChunk;
		hCompositor->ulChunkId               = pPicture->ulChunkId;
	}

	BDBG_LEAVE(BVDC_P_Compositor_SetMBoxMetaData);
#else
	BSTD_UNUSED(pPicture);
	BSTD_UNUSED(hCompositor);
#endif
}

/* End of file. */
