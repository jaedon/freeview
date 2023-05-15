/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvdc_window_priv.h $
* $brcm_Revision: Hydra_Software_Devel/376 $
* $brcm_Date: 8/16/12 2:45p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vdc/src/common/bvdc_window_priv.h $
 * 
 * Hydra_Software_Devel/389   5/9/13 5:08p yuxiaz
 * SW7425-4785: Clear rect is not shown for all mosaic windows. Optimize
 * mosaic zorder code.
 * 
 * Hydra_Software_Devel/376   8/16/12 2:45p tdo
 * SW7445-8: Create Appframework emulation environment for 7445 A0
 *
 * Hydra_Software_Devel/375   8/15/12 6:20p tdo
 * SW7445-8: Create Appframework emulation environment for 7445 A0
 *
 * Hydra_Software_Devel/374   6/29/12 3:31p vanessah
 * SW7425-3359: FNRT flags support to ViCE MBox
 *
 * Hydra_Software_Devel/373   6/14/12 3:44p syang
 * SW7425-2172: VFD/GFX with only cmp0 need 6 scratch regs for gfx;
 * prepare to add SetSurface support to MFD
 *
 * Hydra_Software_Devel/372   6/6/12 4:52p tdo
 * SW7552-279: video(color or brightness) is abnormal after change channel
 * to NTSC SD program when brightness/contrast/saturation/Hue is not
 * default value
 *
 * Hydra_Software_Devel/371   5/31/12 4:04p vanessah
 * SW7425-2643: adding bar data support for scaling etc
 *
 * Hydra_Software_Devel/370   3/30/12 3:34p pntruong
 * SW7420-2286: Added tri-state option for pq policies for mad.
 *
 * Hydra_Software_Devel/369   2/17/12 2:13p yuxiaz
 * SW7231-635: Use BVDC_P_SCB_BURST_SIZE.
 *
 * Hydra_Software_Devel/368   2/17/12 11:19a vanessah
 * SW7425-2327: add Bar data handling to VDC
 *
 * Hydra_Software_Devel/367   2/16/12 1:16p pntruong
 * SW7231-587: Correctly propagate the original pts value to get buffer.
 *
 * Hydra_Software_Devel/366   2/15/12 3:48p pntruong
 * SW7231-587: Correctly propagate the original pts value to get buffer.
 *
 * Hydra_Software_Devel/365   2/14/12 1:37p vanessah
 * SW7425-2020: add meta data delay when deinterlacer on/off
 *
 * Hydra_Software_Devel/364   12/19/11 4:33p vanessah
 * SW7425-2020: Meta data include deinterlacer delay
 *
 * Hydra_Software_Devel/363   12/16/11 10:29a hongtaoz
 * SW7425-1169: count and resume the dropped non-ignore pics during BVN
 * reconfig to keep the NRT transcode AV sync;
 *
 * Hydra_Software_Devel/362   12/1/11 4:34p yuxiaz
 * SW7425-968, SW7344-95: Merged into mainline.: added independent source
 * clipping of right window in 3D mode.
 *
 * Hydra_Software_Devel/361   11/23/11 4:24p tdo
 * SW7435-9: Add support for CMP4-5, GFD4-5, MFD3, VFD5
 *
 * Hydra_Software_Devel/360   8/31/11 5:39p pntruong
 * SW7425-973:  Implement bandwidth restriction for madr.
 *
 * Hydra_Software_Devel/359   8/31/11 11:24a syang
 * SW7425-1170: pxlAspRatio passed to ViCE2 is corrected to x<<16 | y
 * format
 *
 * Hydra_Software_Devel/358   8/26/11 5:39p syang
 * SW7425-1170: merge from branch SW7572-1170
 *
 * Hydra_Software_Devel/SW7425-1170/2   8/26/11 4:23p syang
 * SW7425-1170: refactor pixel aspect ratio related code
 *
 * Hydra_Software_Devel/SW7425-1170/1   8/24/11 6:54p vanessah
 * SW7425-1170: ASP code clean
 *
 * Hydra_Software_Devel/357   8/25/11 11:03a pntruong
 * SW7425-1190, SW7425-630: Improved documentation and renamed parameter
 * to better fit functionality.
 *
 * Hydra_Software_Devel/356   7/8/11 4:27p yuxiaz
 * SW7420-1959: Updated comment for mosaic mode.
 *
 * Hydra_Software_Devel/355   6/24/11 3:49p albertl
 * SW7420-1966: Cleaned up and reorganized colorspace enums and indicies
 * in display, vdec, and cmp.
 *
 * Hydra_Software_Devel/354   6/2/11 11:28a vanessah
 * SW7425-630: Further fix API and setDisplaySize CurInfo NewInfo
 * confusion.
 *
 * Hydra_Software_Devel/353   5/27/11 3:38p yuxiaz
 * SW7425-415: Clean up buffer allocation and orientation settings for 3d
 * video support.
 *
 * Hydra_Software_Devel/352   5/17/11 5:15p vanessah
 * SW7425-404: further code clean-up on GFX Display format update
 *
 * Hydra_Software_Devel/351   5/12/11 4:25p yuxiaz
 * SW7231-74: Added software workaround for MADR alignment hung.
 *
 * Hydra_Software_Devel/350   4/25/11 3:50p yuxiaz
 * SW7425-348: Added software workaround for MADR alignment.
 *
 * Hydra_Software_Devel/349   4/21/11 6:46p yuxiaz
 * SW7425-411: Temporary fix for the MVC playback segfault. Need to clean
 * up later.
 *
 * Hydra_Software_Devel/348   4/8/11 1:57p yuxiaz
 * SW7425-236: Fixed flashing when source orientation changes.
 *
 * Hydra_Software_Devel/347   3/9/11 3:34p darnstein
 * SW7405-3191: (comment only) document units for ulCapRate.
 *
 * Hydra_Software_Devel/346   3/3/11 2:11p pntruong
 * SW7400-3008: Need to release unused/acquired resources that does not go
 * thru vnet shutdown path.
 *
 * Hydra_Software_Devel/345   1/18/11 4:54p vanessah
 * SW7422-43:  3D graphics support
 *
 * Hydra_Software_Devel/344   1/13/11 3:15p yuxiaz
 * SW7422-53: Added mosaic support to PIP.
 *
 * Hydra_Software_Devel/343   1/4/11 4:09p yuxiaz
 * SW7422-174: Refactor DCX macros.
 *
 * Hydra_Software_Devel/342   11/11/10 7:33p albertl
 * SW7125-364: Fixed BVDC_P_CbIsDirty and added assert to check bitfields
 * in dirty bits fit within union integer representation.  Fixed naming
 * of dirty bits.
 *
 * Hydra_Software_Devel/341   11/10/10 4:37p albertl
 * SW7550-610: Extended BVDC_Window_SetCoefficientIndex to work for
 * graphics windows.
 *
 * Hydra_Software_Devel/340   10/29/10 3:03p tdo
 * SW7422-40: Update GetBuffer() capability in VDC with 3D case
 *
 * Hydra_Software_Devel/339   10/29/10 10:59a yuxiaz
 * SW7422-31, SW7422-32, SW7422-39: More for 3D Video support.
 *
 * Hydra_Software_Devel/338   10/28/10 6:15p tdo
 * SW7422-42:  Update compositor 3D programming.
 *
 * Hydra_Software_Devel/337   10/19/10 4:07p yuxiaz
 * SW7420-1190: Make mosaic scratch buffer to be per window base.
 *
 * Hydra_Software_Devel/336   10/11/10 1:03p jessem
 * SW7420-173: Added support for VFD as source feature.
 *
* Hydra_Software_Devel/335   10/7/10 11:47a yuxiaz
* SW7422-38: Handle VDC changes to memory allocation for 3D.
*
* Hydra_Software_Devel/334   10/4/10 5:26p hongtaoz
* SW7425-33: added the 4th compositor support;
*
* Hydra_Software_Devel/333   9/2/10 1:38p vanessah
* SW7422-52:  MAD + MCVP both on 7422
*
* Hydra_Software_Devel/332   8/26/10 7:41p albertl
* SW7405-4556, SW7405-4515: Changed RGB attenuation to use inverse of
* user matrix C.
*
* Hydra_Software_Devel/331   8/26/10 2:36p vanessah
* SW7125-556: SW7420-965:  Support for MAD 4 fields 1 delay and MCVP game
* mode
*
* Hydra_Software_Devel/SW7420-965/1   8/20/10 1:30p vanessah
* SW7420-965:  MCVP game mode support
*
* Hydra_Software_Devel/330   6/18/10 4:25p rpan
* SW7400-2763: New BVN resource management.
*
 * Hydra_Software_Devel/329   5/7/10 7:21p albertl
 * SW7125-364: Changed dirty bits to use union structure to avoid type-pun
 * warnings
 *
 * Hydra_Software_Devel/328   4/23/10 6:00p tdo
 * SW7401-3828: Support AFD=2, 3 for asia market. Add the AFD clipping for
 * SAR that has equivalent DAR of 4x3 and 16x9.
 *
 * Hydra_Software_Devel/327   4/19/10 10:25p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic. Move
 * BDBG_OBJECT_ID_DECLARE private header files instead of .c.
 *
 * Hydra_Software_Devel/326   4/15/10 4:54p hongtaoz
 * SW3548-2828: changed the normalized fixed point match precision to
 * accomandate 12-bit picture size for 2160i;
 *
 * Hydra_Software_Devel/325   4/7/10 11:39a tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic.  Rename TLA
 *
 * Hydra_Software_Devel/324   4/5/10 4:18p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic
 *
 * Hydra_Software_Devel/323   3/17/10 12:05p syang
 * SW7405-4046: set canvas ctrl after sur ctrl; handle gfx win vnet the
 * same as video; ensure that reader and writer vnetState match in sync-
 * locked case; aligned width up to even for stride in feeder and cap;
 * assert mosaic mode doesn't co-exist with dest cut;
 *
 * Hydra_Software_Devel/322   2/11/10 11:01a pntruong
 * SW7405-3829: Moved shrinkwidth to mad settings.
 *
 * Hydra_Software_Devel/321   12/23/09 2:04p tdo
 * SW7408-26: Add new TNT support
 *
 * Hydra_Software_Devel/320   12/22/09 5:14p tdo
 * SW7401-3828: Add AFD support
 *
 * Hydra_Software_Devel/319   12/2/09 3:05p yuxiaz
 * SW7405-3120: Added support for overlapping mosaic videos with z-
 * ordering
 *
 * Hydra_Software_Devel/318   11/24/09 5:48p yuxiaz
 * SW7420-462: Added new tuning functions for Tuning Database.
 *
 * Hydra_Software_Devel/317   11/16/09 5:04p yuxiaz
 * SW7405-3406: Clean up bMosaicMode and  bMosaicClearOutside.
 *
 * Hydra_Software_Devel/316   10/27/09 10:53p albertl
 * SW7420-30: Changed mosaic matrices to be loaded statically with xvYcc
 * matrix loaded dynamically as necessary.
 *
 * Hydra_Software_Devel/315   10/26/09 3:59p syang
 * SW7340-60: add CapOut, VfdOut and WinIn into madDeleay handlling
 *
 * Hydra_Software_Devel/314   10/2/09 4:51p yuxiaz
 * SW7405-3068: BVN hang in mosaic mode.
 *
 * Hydra_Software_Devel/313   6/2/09 3:04p syang
 * PR 55646:  smoothly filtering  in field inversion cases
 *
 * Hydra_Software_Devel/312   5/13/09 5:20p syang
 * PR53688: flush content or cutting-before-mad change for mad trick mode
 *
 * Hydra_Software_Devel/311   5/12/09 9:11a syang
 * PR53688: rewrite AdjustPicRepeatBit_isr for syc-lock win
 *
 * Hydra_Software_Devel/310   5/6/09 11:23a syang
 * PR53037, PR53038, PR53687, PR53688: cleanup AdjustForMadDelay_isr for
 * readabilty
 *
 * Hydra_Software_Devel/309   4/28/09 1:42p rpan
 * PR47621: Minor user capture buffer implementation clean-up.
 *
 * Hydra_Software_Devel/308   4/6/09 7:18p hongtaoz
 * PR53037, PR53038, PR53687, PR53688: reset sw MAD delay pipeline in
 * window context when display format switch and MAD is at vnet reader
 * side to prevent BVN error;
 *
 * Hydra_Software_Devel/307   4/6/09 4:34p hongtaoz
 * PR53037, PR53038, PR53687, PR53688: rewrote the
 * BVDC_P_Window_AdjustForMadDelay_isr to truely pipeline the picture
 * info to adapt to various deinterlacer delays;
 *
 * Hydra_Software_Devel/306   4/6/09 3:43p syang
 * PR  53484 , PR 52037: cleanup (rect, scl-ratio, ..) rounding code and
 * AdjustRect code ; add 1 line delay to TV chip "early trigger"; change
 * BVDC_P_DST_CUT_TOP_THRESH to 4 for 3548;
 *
 * Hydra_Software_Devel/305   4/3/09 4:35p yuxiaz
 * PR53460: Optimal 444/422 conversion settings
 *
 * Hydra_Software_Devel/304   3/30/09 2:24p hongtaoz
 * PR53037, PR53038: Implement the VEC Locking/Alignments For Memory
 * Reduction; Implement Double-Capture-Buffer Algorithm Under VEC Locking
 * Scheme
 *
 * Hydra_Software_Devel/303   3/19/09 11:36p pntruong
 * PR53429: Corrected the anr/mad/hscl vnet connection.  Removed ununsed
 * bAnrMadCombo flags.
 *
 * Hydra_Software_Devel/302   3/18/09 9:53a yuxiaz
 * PR52840, PR53325: Fixed Coverity Defect issues.
 *
 * Hydra_Software_Devel/301   3/16/09 10:52p tdo
 * PR45785, PR45789: Merge from MCVP branch
 *
 * Hydra_Software_Devel/7420_mcvp/7   3/16/09 1:03p tdo
 * PR45785, PR45789: merge from main branch on 3/16/09
 *
 * Hydra_Software_Devel/300   3/13/09 5:30p syang
 * PR 52376, PR 41481, PR 44041:
 * 1). exec user buf returning to hBuffer before shut-down code in
 * writer_isr; 2). clean up mad-delay handling code and add handling to
 * all settings in pic-node that are delayed by mad; 3). 444 SCL no-
 * longer uses BVN_IN to align-up for half-band; 4). add pic-node rects
 * dump feature as setting lbox_0_scratch to 2 (1 still printing mpeg pic
 * info).
 *
 * Hydra_Software_Devel/7420_mcvp/6   3/2/09 4:22p tdo
 * PR 45785, PR 45789: merge from main branch on 3/02/09
 *
 * Hydra_Software_Devel/299   3/2/09 3:47p yuxiaz
 * PR52287: More update on 422-444 and 444-422 converters.
 *
 * Hydra_Software_Devel/7420_mcvp/5   2/26/09 4:10p tdo
 * PR 45785, PR 45789: merge from main branch on 2/26/09
 *
 * Hydra_Software_Devel/298   2/26/09 3:23p albertl
 * PR51925: Added CMP_1 G0 support to 3548.
 *
 * Hydra_Software_Devel/7420_mcvp/4   2/18/09 11:56a syang
 * PR 45785, PR 45789: merge from main branch on 2/18/09
 *
 * Hydra_Software_Devel/297   2/13/09 11:16a yuxiaz
 * PR52030: MAD QM do not use DCX logic.
 *
 * Hydra_Software_Devel/7420_mcvp/3   2/11/09 11:32a tdo
 * PR 45785, PR 45789: merge from main branch on 2/11/09
 *
 * Hydra_Software_Devel/296   2/10/09 11:17a yuxiaz
 * PR51614: VDC global symbol naming convention violations
 *
 * Hydra_Software_Devel/7420_mcvp/2   2/3/09 7:23p syang
 * PR 45785, PR 45789: merge from main branch on 2/3/09
 *
 * Hydra_Software_Devel/295   2/2/09 1:28a albertl
 * PR45798: Fixed missing prototype compiler warning.
 *
 * Hydra_Software_Devel/294   1/31/09 3:31a albertl
 * PR45798: Fixed bugs and enabled mosaic mode csc implementation.
 *
 * Hydra_Software_Devel/7420_mcvp/1   1/23/09 11:32p syang
 * PR 45785, PR 45789: add PI support for new the new module  MCVP
 * (MCTF+MCDI)
 *
 * Hydra_Software_Devel/293   1/16/09 11:44a yuxiaz
 * PR51160: Updated throughput calculation for MAD/ANR.
 *
 * Hydra_Software_Devel/292   1/15/09 1:16p tdo
 * PR46529, PR51020: Implement const delay for histogram for dynamic
 * contrast use, and avoid histogram update when source freezed
 *
 * Hydra_Software_Devel/291   1/12/09 1:39p albertl
 * PR45798: Added CMP Mosaic CSC implementation for 7420.
 *
 * Hydra_Software_Devel/290   1/6/09 11:47a rpan
 * PR49634: Use the trick mode flag passed by decoder instead of trying to
 * detect it.
 *
 * Hydra_Software_Devel/289   1/6/09 9:56a rpan
 * PR50759: Take the capture buffer held by user into count when re-
 * allocating buffers.
 *
 * Hydra_Software_Devel/288   12/10/08 2:02p albertl
 * PR45808: Updated 7420 CSC support and reworked non-linear window and
 * CMP CSC versioning.
 *
 * Hydra_Software_Devel/287   12/9/08 8:52p tdo
 * PR45740: Support New BVN Bandwidth Equation for 7420
 *
 * Hydra_Software_Devel/286   12/3/08 5:04p jessem
 * PR 48992: Added Chroma Histogram API support.
 *
 * Hydra_Software_Devel/285   11/24/08 11:03a yuxiaz
 * PR49598: Use bandwidth equation to calculate throughput.
 *
 * Hydra_Software_Devel/284   11/20/08 2:51p rpan
 * PR47337: Added trick mode detection. Replaced feedback based capture
 * polarity prediction mechanism with a non-feedback based mechanism.
 * This would help reduce latency.
 *
 * Hydra_Software_Devel/283   11/12/08 3:14p tdo
 * PR48642: Provide clipping rect for histo luma region
 *
 * Hydra_Software_Devel/282   11/10/08 5:02p syang
 * PR 41898: remove un-needed bools from win-context and dirty bits
 *
 * Hydra_Software_Devel/281   10/28/08 9:16p pntruong
 * PR48028, PR48029: Allow controlling of scaler's dejagging and
 * deringing.
 *
 * Hydra_Software_Devel/280   10/23/08 5:56p albertl
 * PR47318, PR47814: Fixed overflow when calculating RGB attenuation
 * offsets.  Rewrote portions of RGB attenuation code and fixed various
 * offset related issues.
 *
 * Hydra_Software_Devel/279   10/23/08 1:59p tdo
 * PR46529: Implement a consistent delay for the dynamic contrast
 * algorithm
 *
 * Hydra_Software_Devel/278   10/16/08 3:06p syang
 * PR 47896:  fix the addr error for V1 sur register
 *
 * Hydra_Software_Devel/277   10/9/08 5:30p syang
 * PR 46891: add _isr to the name of func used in _isr context
 *
 * Hydra_Software_Devel/276   10/7/08 4:05p syang
 * PR 41898: 1). move video sur enabling (in RUL) from cmp to win together
 * with other modules in  readerBuilRul;  2). get rid of muteReader; 3).
 * consolidate sur constant color usage impl with vnet start/shut-down
 *
 * Hydra_Software_Devel/275   10/3/08 11:01a yuxiaz
 * PR46305, PR46307, PR46309: Use new TestFeature1 function from BSUR.
 * Change BVDC_P_SUPPORT_VIDEO_TestFeature1 to
 * BVDC_P_SUPPORT_VIDEO_TESTFEATURE1.
 *
 * Hydra_Software_Devel/274   10/3/08 9:26a yuxiaz
 * PR46305, PR46307, PR46309: More Video TestFeature1 support.
 *
 * Hydra_Software_Devel/273   10/1/08 8:34p pntruong
 * PR47303, PR46309: Reused the TestFeature1 type in surface.
 *
 * Hydra_Software_Devel/272   9/30/08 4:57p yuxiaz
 * PR46305, PR46307: Video TestFeature1 support: Fix throughput
 * calculation, combine MAD and ANR set TestFeature1 function.
 *
 * Hydra_Software_Devel/271   9/30/08 12:35p jessem
 * PR 46489: Re-added TN2T support using tighter compiler directive to
 * compile only when BVDC_P_SUPPORT_TNT_VER >= 5.
 *
 * Hydra_Software_Devel/270   9/29/08 6:37p pntruong
 * PR 46489: Rollbacked all the build errors introduced.  Todo: need to
 * properly factored out the code for tnt and tnt new.
 *
 * Hydra_Software_Devel/268   9/29/08 12:56p pntruong
 * PR47072: Moving 3563 vdc dedicated branch.
 *
 * Hydra_Software_Devel/267   9/26/08 3:26p yuxiaz
 * PR46305, PR46307: More video TestFeature1 support for 3548 B0.
 *
 * Hydra_Software_Devel/266   9/24/08 6:19p pntruong
 * PR47113, PR45308: Allow flexibility in selecting builtins and user
 * defined filter coefficients.
 *
 * Hydra_Software_Devel/265   9/22/08 9:29p pntruong
 * PR47160: Added additional case for capture as frame.  Refactored logic
 * and trigger computation on change.
 *
 * Hydra_Software_Devel/264   9/22/08 4:53p yuxiaz
 * PR46305, PR46307, PR46309: Added video TestFeature1 support for 3548 B0.
 *
 * Hydra_Software_Devel/263   9/18/08 3:06p pntruong
 * PR46477: B0: Remove workaround dealing with VDEC/BVB hookup.
 *
 * Hydra_Software_Devel/262   9/12/08 10:45a rpan
 * PR46794: Improved handling of increasing from 3 to 5 buffers or
 * decreasing from 5 to 3 buffers.
 *
 * Hydra_Software_Devel/261   9/5/08 4:28p rpan
 * PR46100: Re-implemented 3 buffers for 1080p24/25/30 source to
 * 1080p48/50/60 display to match the algorithm.
 *
 * Hydra_Software_Devel/260   9/4/08 2:28p rpan
 * PR45319: Further cleaned up multi-buffering stuff from other modules.
 *
 * Hydra_Software_Devel/259   9/4/08 1:25p rpan
 * PR45319: Moved multi-buffering related stuff from window to buffer.
 *
 * Hydra_Software_Devel/258   8/22/08 10:29a rpan
 * PR45979: Remove VBI passthru support.
 *
 * Hydra_Software_Devel/257   7/11/08 4:47p rpan
 * PR43614: Bandwidth equation API change.
 *
 * Hydra_Software_Devel/256   6/20/08 3:44p tdo
 * PR43662 , PR43645, PR38841: Fix mosaic and SD to HD channel change due
 * to adjust rectangle being ignored when picture is muted.
 *
 * Hydra_Software_Devel/255   6/17/08 5:29p tdo
 * PR39333: Connect HIST at reader side for VDEC source as a work around
 *
 * Hydra_Software_Devel/254   6/17/08 10:59a rpan
 * PR43614: Moved bandwidth equation parameters to BVDC_P_Window_info
 * structure. Changed factor to 1000.
 *
 * Hydra_Software_Devel/253   6/16/08 5:00p rpan
 * PR43614: Added API for user to set/get bandwidth equation parameters.
 *
 * Hydra_Software_Devel/252   5/30/08 1:40p tdo
 * PR38362, PR38678, PR38841, PR38842: More fine tuning of the BVN HW
 * limitation implementation.
 *
 * Hydra_Software_Devel/251   5/29/08 1:30p tdo
 * PR38362, PR38678, PR38841, PR38842: Enforce Hardware Minimum Size
 * Limitation for BVN Blocks and Partition Between Destination and Source
 * Clipping To Support 2x1 Window Size
 *
 * Hydra_Software_Devel/250   5/28/08 7:06p hongtaoz
 * PR43082: MPEG source interrupt field swap logic needs to consider total
 * BVN delay for lipsync purpose;
 *
 * Hydra_Software_Devel/249   5/23/08 3:57p rpan
 * PR43006: At least five buffers required if a window is sync slip, 50Hz
 * source ->60 Hz interlaced display, and feeder goes directly to
 * compositor and VEC.
 *
 * Hydra_Software_Devel/248   5/21/08 1:56p albertl
 * PR39163:  Updated CMP csc tables to use 10 bit offset values.
 * Corrected to use proper csc tables in conversions involving ntsc, pal,
 * and smpte 240M.  Moved colorclip from window to compositor and changed
 * BVDC_Window_Set/GetColorClip to BVDC_Compositor_Set/GetColorClip.
 *
 * Hydra_Software_Devel/247   5/12/08 2:54p jessem
 * PR 39335: Removed bCm3dEnable and added bCm3dRegionEnable and
 * bCm3dOverlapRegionEnable in BVDC_P_Window_Info. Added CM3D demo
 * support to BVDC_P_Window_ValidateChanges.
 *
 * Hydra_Software_Devel/246   5/6/08 4:01p jessem
 * PR 39335: Added bCm3dEnable to BVDC_P_Window_Info.
 *
 * Hydra_Software_Devel/245   4/25/08 10:27a rpan
 * PR37429: Restore the forced capturing as frame logic.
 *
 * Hydra_Software_Devel/244   4/17/08 3:07p rpan
 * PR37429: Deleted the forced capturing as frame logic.
 *
 * Hydra_Software_Devel/243   4/4/08 4:51p jessem
 * PR 39335: Removed  bCm3dOvlpRegionChange and bCm3dRegionChange in
 * BVDC_P_Window_Context. Applied setting of dirty bits in "Set"
 * function. Corrected implementation of CM3D RULs.
 *
 * Hydra_Software_Devel/242   3/31/08 2:40p jessem
 * PR 39335: Added CM3D support.
 *
 * Hydra_Software_Devel/241   3/27/08 7:42p albertl
 * PR39163:  Implemented non-linear get matrix functions.
 *
 * Hydra_Software_Devel/240   3/27/08 5:10p albertl
 * PR39163:  Implemented non-linear xvYCC csc support for 3548.
 *
 * Hydra_Software_Devel/239   3/21/08 5:09p tdo
 * PR39333: More Hist implementation for 3548
 *
 * Hydra_Software_Devel/238   3/20/08 10:55a pntruong
 * PR39154, PR39202, PR39203: Removed unsed mad pixel format field,
 * already moved to mad context.
 *
 * Hydra_Software_Devel/237   3/19/08 7:49p hongtaoz
 * PR39154, PR39202, PR39203: combined ANR and MAD in memory saving mode
 * on 3548; implemented MAD specific change; updated SCL precision for
 * 3548;
 *
 * Hydra_Software_Devel/236   3/19/08 5:42p tdo
 * PR39333: Implement HIST core
 *
 * Hydra_Software_Devel/235   2/28/08 10:12p pntruong
 * PR39154: Resolved clearcase merged.
 *
 * Hydra_Software_Devel/234   2/28/08 6:24p hongtaoz
 * PR39154, PR36273, PR39202: added HSCL block; enabled MAD/ANR for 3548;
 * fixed non-linear scaling when MAD is on;
 *
 * Hydra_Software_Devel/232   2/20/08 4:16p hongtaoz
 * PR39154: added window API to toggle ANR; prepare for combo ANR/MAD
 * feature on 3548;
 *
 * Hydra_Software_Devel/231   2/19/08 9:41p pntruong
 * PR33613: Removed hallucination tabs.
 *
 * Hydra_Software_Devel/230   2/7/08 11:52a jessem
 * PR 33613: Reverted back to to version without the modular mutlibuffer.
 * Found a problem with HDDVI sources.
 *
 * Hydra_Software_Devel/228   1/24/08 5:26p tdo
 * PR38710: Add blue stretch support
 *
 * Hydra_Software_Devel/227   1/23/08 11:13a tdo
 * PR38710: Rename blue stretch and green stretch to blue boost and green
 * boost to match their functionality
 *
 * Hydra_Software_Devel/226   11/28/07 2:01p tdo
 * PR37413: Provide new Api to set offset and size of histogram window and
 * read corresponding parameters from specific window
 *
 * Hydra_Software_Devel/225   11/5/07 11:48a syang
 * PR 29617:  Set SCL's top/left cut with vsync delay according to MAD
 *
 * Hydra_Software_Devel/224   10/23/07 11:18a yuxiaz
 * PR29569, PR36290: Add FGT support on 7405.
 *
 * Hydra_Software_Devel/PR29569/3   10/22/07 5:16p yuxiaz
 * PR29569: More FGT work.
 *
 * Hydra_Software_Devel/PR29569/2   10/22/07 3:22p yuxiaz
 * PR29569: Merge from mainline.
 *
 * Hydra_Software_Devel/223   10/22/07 11:28a pntruong
 * PR35941: Added option to allow smooth transition or bandwidth
 * optimization.
 *
 * Hydra_Software_Devel/222   8/30/07 10:30p pntruong
 * PR34472: Use MASK block to improve the Contour/Quantization noise
 * issue.
 *
 * Hydra_Software_Devel/221   8/30/07 3:16p pntruong
 * PR34348: MAD optimizing Low Angle Settings for reduce gray noise.
 *
 * Hydra_Software_Devel/PR34348/1   8/29/07 4:11p xhuang
 * PR34348:add low angle customer settings
 *
 * Hydra_Software_Devel/220   8/3/07 12:14a pntruong
 * PR33410: Don't toggle scaler on VNET patch when user switch the view
 * mode.  Removed release build warnings.
 *
 * Hydra_Software_Devel/219   8/2/07 1:54p pntruong
 * PR31983: C0: Need to enable 10-bit MAD capture mode when source is
 * 480i.
 *
 * Hydra_Software_Devel/218   7/30/07 6:51p albertl
 * PR28869:  Removed VDC dependency on BAVC_MOSAIC_MAX.  Clear rect and
 * mosaic maximums are now dependent on definitions in bvdc_common.h.
 *
 * Hydra_Software_Devel/217   7/30/07 2:10p hongtaoz
 * PR26785, PR33560, PR33020: added MAD game mode hard start; removed old
 * transition workaround;
 *
 * Hydra_Software_Devel/216   7/19/07 4:28p yuxiaz
 * PR32593:  Back out previous change.
 *
 * Hydra_Software_Devel/215   7/19/07 4:22p yuxiaz
 * PR32593:  Put back version 214.
 *
 * Hydra_Software_Devel/214   7/12/07 6:20p albertl
 * PR32593:  Added missing case for BAVC_SourceId_eDs0 and added fixes to
 * 24 to 60 Hz conversion requiring an extra buffer and a wider gap
 * between reader and writer.
 *
 * Hydra_Software_Devel/213   6/28/07 7:27p hongtaoz
 * PR32130, PR29189: added buffer game mode delay control; fixed picture
 * timestamp update bug when isr reorder occurs; invalid timestamp/drift
 * delay should disappear now; lipsync delay reflects the correct game
 * mode effect;
 *
 * Hydra_Software_Devel/VDC_Game_Mode/3   6/28/07 5:39p hongtaoz
 * PR32130: added game mode clock adjustment; added restriction of fast
 * adjustment for 1080p in 1080p out to avoid flash caused by bandwidth
 * overshoot when accelerating;
 *
 * Hydra_Software_Devel/VDC_Game_Mode/2   6/26/07 11:39p hongtaoz
 * PR32130: improved VDC internal handling of fine/coarse adjustment
 * switch for PC formats and 59/60 slip;
 *
 * Hydra_Software_Devel/VDC_Game_Mode/1   6/26/07 2:23p hongtaoz
 * PR32130: added window game mode buffer delay control;
 *
 * Hydra_Software_Devel/212   6/21/07 3:28p syang
 * PR 32440: use pSource->bPictureChanged to decide if we re-decide
 * MAD/ANR heapId and re-init ANR
 *
 * Hydra_Software_Devel/211   6/21/07 5:29a albertl
 * PR31821:  Changed MAD game modes to allocate only the necessary number
 * of buffers and to reconfigure the vnet when buffer numbers change.
 *
 * Hydra_Software_Devel/210   6/13/07 3:04p tdo
 * PR30613: Incorporate the LAB contrast, brightness and saturation into
 * the CMP CSC calculation per customer requests
 *
 * Hydra_Software_Devel/209   6/12/07 11:18a tdo
 * PR30613: Move CSC calculations into bvdc_csc_priv.c
 *
 * Hydra_Software_Devel/208   5/31/07 3:26p tdo
 * PR28978: C0: Provide API to realize CMS function
 *
 * Hydra_Software_Devel/207   5/25/07 6:49p hongtaoz
 * PR31576: updated lipsync delay buffer addition algorithm; avoid back
 * motion and inconsistent window reader programming;
 *
 * Hydra_Software_Devel/206   5/17/07 3:45p pntruong
 * PR30364: Provide API to control MAD up/down sampling.
 *
 * Hydra_Software_Devel/205   5/15/07 5:18p albertl
 * PR30702:  Tweaked min and max csc values to more usable ranges.
 *
 * Hydra_Software_Devel/204   5/14/07 3:17p albertl
 * PR30702:  Expanded contrast, brightness, saturation and hue ranges.
 *
 * Hydra_Software_Devel/203   5/10/07 10:00a syang
 * PR 28895, PR 28898, PR 29846 , PR 28550: 1). add frame for C0 ANR PI
 * support; 2). move MAD/ANR buf alloc/free to MAD/ANR; 3). remove
 * drainVnet state
 *
 * Hydra_Software_Devel/202   5/8/07 4:38p albertl
 * PR30292:  Contrast extended to Cb and Cr.
 *
 * Hydra_Software_Devel/201   5/3/07 7:02p pntruong
 * PR29662: Added c0 dynamics rts settings.
 *
 * Hydra_Software_Devel/200   5/1/07 4:44p syang
 * PR 28541: 1) changed non-linear scl api; 2). carry sub-pxl accuracy
 * through the pipe line by SrcHStep and SrcVStep
 *
 * Hydra_Software_Devel/198   4/15/07 1:54a albertl
 * PR28869:  Maximum clear rect now defined for each window and chipset.
 *
 * Hydra_Software_Devel/197   4/12/07 5:36p tdo
 * PR28837: Provide API to customize LAB LUT
 *
 * Hydra_Software_Devel/196   4/9/07 11:27a pntruong
 * PR28266: Provide API to customize DVO CSC.
 *
 * Hydra_Software_Devel/195   4/5/07 4:58p albertl
 * PR29194:  Added BVDC_Window_SetCoefficientIndex and input and index
 * matching support to scaler coefficient tables.
 *
 * Hydra_Software_Devel/194   4/5/07 2:05p pntruong
 * PR28538, PR28539, PR28540, PR25462: Average pixel luma and customized
 * deinterlacer support.
 *
 * Hydra_Software_Devel/PR28538/2   4/3/07 9:34p xhuang
 * PR25462: Support APL setting
 *
 * Hydra_Software_Devel/PR28538/1   3/28/07 6:43p xhuang
 * PR28538:Add new api for 3:2 pulldown, 2:2 pulldown and mad setting.
 * Also for PR28539,PR28540
 *
 * Hydra_Software_Devel/193   4/4/07 3:47p syang
 * PR 29243: change VnetMode def (now use bit-fields)
 *
 * Hydra_Software_Devel/192   3/29/07 4:18p tdo
 * PR 28404: Implement user set contrast stretch configuration
 *
 * Hydra_Software_Devel/191   3/28/07 6:15p tdo
 * PR28392, PR28282: Implement get color matrix function
 *
 * Hydra_Software_Devel/190   3/28/07 3:27p tdo
 * PR 28403: Provide API to customize TNT for sharpness
 *
 * Hydra_Software_Devel/189   3/26/07 3:27p tdo
 * PR 28401: Provide API to customize CAB LUT and enable/disable CAB
 *
 * Hydra_Software_Devel/188   3/25/07 1:46p yuxiaz
 * PR25181: VDC Buffer Allocation Fragments fix.
 *
 * Hydra_Software_Devel/187   3/23/07 4:38p tdo
 * PR 28282: Provide API to customize CMP CSC
 *
 * Hydra_Software_Devel/186   3/20/07 4:49p hongtaoz
 * PR22434: added mosaic channel frame rate tracking API;
 *
 * Hydra_Software_Devel/185   3/18/07 5:39p pntruong
 * PR28779: Fixed unnecessary vnet shutdown (mad buffers re-alloc) when
 * clipping causing a flashed.
 *
 * Hydra_Software_Devel/184   3/5/07 6:44p syang
 * PR 28298: move DNR's Acquire/Release/SetInfo to window (to be
 * consistant with other sub-modules)
 *
 * Hydra_Software_Devel/183   2/28/07 3:11p pntruong
 * PR27908: [MPEG/1080i] When tuner program is 1080i format, after
 * scaning, the video is bad, and is splited to 2 half.
 *
 * Hydra_Software_Devel/182   2/21/07 1:29p jessem
 * PR 23218, PR 27698, PR 27758: PR 27698: Added support for
 * 23.976/24/25/30/50Hz progressive sources displayed as 59.94/60/50Hz
 * interlaced.
 *
 * Hydra_Software_Devel/181   2/20/07 11:32a pntruong
 * PR22574: Reformat to improve readablity.
 *
 * Hydra_Software_Devel/180   1/22/07 3:44p tdo
 * PR 22574: Remove compiler warnings
 *
 * Hydra_Software_Devel/179   1/19/07 2:54p tdo
 * PR 23377:  Dynamically allocate proper MAD buffer size as source
 * changes
 *
 * Hydra_Software_Devel/178   1/10/07 4:45p yuxiaz
 * PR 26313: Rename BVDC_SplitScreenSettings to
 * BVDC_Window_SplitScreenSettings.
 *
 * Hydra_Software_Devel/177   1/10/07 3:32p yuxiaz
 * PR 26313: Change DNR API to source based API: Remove
 * BVDC_Window_SetDnrFilterLevel and BVDC_Window_GetDnrFilterLevel. Add
 * BVDC_Source_SetDnrFilterLevel, BVDC_Source_GetDnrFilterLevel,
 * BVDC_Source_SetSplitScreenMode and BVDC_Source_GetSplitScreenMode.
 *
 * Hydra_Software_Devel/176   1/5/07 4:27p syang
 * PR 22569: almost completed coding for new vnet build/shutdown algorithm
 * (by each sub-modules)
 *
 * Hydra_Software_Devel/175   12/22/06 11:58a syang
 * PR 22569: prepare to add anr support
 *
 * Hydra_Software_Devel/174   12/19/06 3:55p hongtaoz
 * PR25668: added 7403 VDC support; further consolidated BCHP_CHIP macro
 * usage;
 *
 * Hydra_Software_Devel/173   12/18/06 11:21p pntruong
 * PR22577: Merged back to mainline.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/26   12/18/06 7:36p albertl
 * PR24587:  Fixed compile error for 7400.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/25   12/15/06 6:18p albertl
 * PR24587:  Rewrote window offset and index calculations.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/24   12/12/06 5:53p pntruong
 * PR24587: Fixed build errors from previous checked in.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/23   12/12/06 2:28a albertl
 * PR24587:   Changed BVDC_P_WIN_GET_REG_IDX to support assymetric
 * compositors.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/22   11/22/06 3:54p syang
 * PR 26151: re-write shared resource manager. update letter box, mad, dnr
 * shared-scl acquire/release code accordingly
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/21   11/16/06 2:01p hongtaoz
 * PR25668: merge in vdc support for 7403;
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/20   11/13/06 3:36p hongtaoz
 * PR20716: separate displays/compsoitors for loops;
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/19   11/8/06 3:54p yuxiaz
 * PR23202: Added dynamic RTS support for MAD and ANR.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/18   11/8/06 2:27p jessem
 * PR 22987, PR 17846: Re-merge in changes that was previously backed out.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/17   11/3/06 2:15p yuxiaz
 * PR23202: Added dynamic RTS support for CAP and VFD.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/16   11/2/06 2:39p yuxiaz
 * PR23202: Dynamic RTS support in VDC.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/15   10/26/06 3:18p yuxiaz
 * PR25181: Merge in fix from main branch: VDC Buffer Allocation Fragments
 * HD Buffer Pool and Causes Out of Memory Assertion.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/14   10/25/06 2:34p yuxiaz
 * PR20875: Remove bCapDataMode, use bPixelFormat instead.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/13   10/20/06 1:42p pntruong
 * PR23521:  Removed duplicate code, and move game mode into existing
 * deinterlace settings structures.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/12   10/19/06 3:54p albertl
 * PR23521:  Added MAD game mode functionality.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/11   10/18/06 9:31p albertl
 * PR23210:  Added color temp and offset functionality.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/10   10/18/06 12:21p pntruong
 * PR23222: VDEC - ATSC input support.  Separate out colorspace conversion
 * code.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/9   10/11/06 2:01p pntruong
 * PR 22987, PR 17846: Back out previous changes as it still need further
 * validations.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/6   9/28/06 10:28a pntruong
 * PR20875, PR23717:  Used the correct vnetmode for determining buffer id.
 * Simplified mad buffers allocations.  Used context pointer for statics
 * functions to reduce over head.   Refactored resource tables for ease
 * of reading.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/5   9/27/06 12:01p yuxiaz
 * PR20875: Use best fit search to get buffer heap id. Integrate with
 * dynamic buffer allocation.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/4   9/21/06 10:52a jessem
 * PR 23717: Added dynamic MAD buffer support.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/3   9/20/06 10:21p tdo
 * PR 22574: Move sharpness calculation to user context to optimize
 * TAB/TNT isr time
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/2   9/20/06 12:58p pntruong
 * PR23222, PR23225:  Defer window and source heap assignments until user
 * create.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/1   9/19/06 1:48p pntruong
 * PR23222, PR23225: VDEC - ATSC & PC input support.
 *
 * Hydra_Software_Devel/167   9/13/06 7:08p albertl
 * PR18913:  Cleaned up chip specific MTH and fixed point code.
 *
 * Hydra_Software_Devel/166   9/11/06 5:20p jessem
 * PR 22987: Temporarily backed out of feature that adds dealy buffers
 * without muting the buffers.
 *
 * Hydra_Software_Devel/165   9/1/06 2:12p yuxiaz
 * PR20875: 10 bit 444 BVN support: added switch between different capture
 * mdoes.
 *
 * Hydra_Software_Devel/164   8/30/06 11:26p tdo
 * PR 17994, PR 23383:  Add support for DNR demo mode
 *
 * Hydra_Software_Devel/163   8/30/06 6:00p tdo
 * PR 17994, PR 23213, PR 23383:  Implement demo mode support for the PEP.
 *
 * Hydra_Software_Devel/162   8/29/06 3:29p tdo
 * PR 23212: Add new APIs for contrast stretch, color clip, auto flesh,
 * green stretch and blue stretch.
 *
 * Hydra_Software_Devel/161   8/28/06 3:43p tdo
 * PR 23212: Add support for auto flesh, blue stretch, and green boost
 *
 * Hydra_Software_Devel/160   8/28/06 2:45p tdo
 * PR 22574: Remove BVDC_P_SUPPORT_TAB and BVDC_P_SUPPORT_TNT
 *
 * Hydra_Software_Devel/159   8/28/06 1:45p yuxiaz
 * PR20875: Renamed BVDC_P_SUPPORT_10BIT_BVN to
 * BVDC_P_SUPPORT_BVN_10BIT_444. Removed BVDC_P_BVNDataMode, use window
 * and buffer node's pixel format. Integrated with new 10 bit pixel
 * formats.
 *
 * Hydra_Software_Devel/158   8/24/06 1:59p tdo
 * PR 22574: Add support for TNT block.  Add support for
 * BVDC_P_SUPPORT_TAB and BVDC_P_SUPPORT_TNT macro
 *
 * Hydra_Software_Devel/157   8/23/06 4:25p syang
 * PR 22563: add new smooth non-linear scaling support for 3563; prepare
 * to allow user to set non-linear scl central region percent; move un-
 * known asp-ratio default handling to window's pixel-asp-ratio code
 *
 * Hydra_Software_Devel/156   8/21/06 4:01p pntruong
 * PR21805: Need a new window ID added to BVDC_Window_Create().
 *
 * Hydra_Software_Devel/155   8/18/06 5:23p albertl
 * PR23117:  Added 7440 support.
 *
 * Hydra_Software_Devel/154   8/18/06 4:31p yuxiaz
 * PR20875: Added BVDC_P_SUPPORT_10BIT_BVN. Added raster change detection
 * for source. Added BVN data mode change detection for window.
 *
 * Hydra_Software_Devel/153   8/15/06 6:17p hongtaoz
 * PR18233, PR20716: added support for mosaic mode - missing channels;
 * assume picture list's channel id are in ascending order; single
 * channel will not result in full-screen automatically;
 *
 * Hydra_Software_Devel/152   8/11/06 10:10a jessem
 * PR 22987, PR 22004: Added support that avoids invalidation of the
 * buffers when ONLY the VSYNC delay has changed.
 *
 * Hydra_Software_Devel/151   8/7/06 5:39p pntruong
 * PR22577: Initial bringup of VDC.
 *
 * Hydra_Software_Devel/150   7/31/06 6:11p hongtaoz
 * PR22499: added DCR filter setting as a part of DNR block;
 *
 * Hydra_Software_Devel/149   7/27/06 4:22p yuxiaz
 * PR22412: Added BVDC_Window_SetForceCapture and
 * BVDC_Window_GetForceCapture.
 *
 * Hydra_Software_Devel/148   7/17/06 5:58p pntruong
 * PR18233: Moved MOSAIC_MAX to avc since other PIs need to access the
 * definition.
 *
 * Hydra_Software_Devel/147   7/7/06 4:15p yuxiaz
 * PR 22296, PR 22108, PR22109: Removed
 * BVDC_Window_InstallLipSyncDelayCallback, added new generic window
 * callback function BVDC_Window_InstallCallback for lip sync.
 *
 * Hydra_Software_Devel/146   6/14/06 6:52p hongtaoz
 * PR20716, PR21804: implement modified version of mosaic mode;
 *
 * Hydra_Software_Devel/145   6/14/06 1:46p syang
 * PR 21689: add support for 7118
 *
 * Hydra_Software_Devel/144   6/13/06 4:34p tdo
 * PR 21535: Create a context for PEP block to store static private data
 *
 * Hydra_Software_Devel/143   6/5/06 1:59p yuxiaz
 * PR21892: 7401 B0 bring up: change coeff macros for 7401B0, 7400 and
 * 3560B0.
 *
 * Hydra_Software_Devel/142   5/26/06 3:44p pntruong
 * PR20642: Refactored handling of hList.
 *
 * Hydra_Software_Devel/140   5/19/06 2:30p hongtaoz
 * PR17514: added bypass filter enum for DNR setting; disable extreme
 * filter for 7401a0 due to PR16299; fixed range clamping for DNR
 * settings; added DNR demo mode setting in test api;
 *
 * Hydra_Software_Devel/139   5/19/06 1:49p pntruong
 * PR21560: Global symbols without BVDC prefix in the BVDC module.  Reduce
 * unnecessary global exported symbols.  Tighten const params.
 *
 * Hydra_Software_Devel/138   5/10/06 3:30p jessem
 * PR 17530: Added reader/writer buffer ordering algorithm when associated
 * ISRs are out of order using timestamps. Added new functions:
 * BVDC_P_Window_UpdateTimestamps_isr,
 * BVDC_P_Window_CheckWriterIsrOrder_isr, and
 * BVDC_P_Window_CheckReaderIsrOrder_isr. Modified calls to
 * BVDC_P_Feeder_Create to reflect new function signature. Modified calls
 * to BVDC_P_Capture_Create to reflect new function signature.
 *
 * Hydra_Software_Devel/137   4/20/06 5:50p pntruong
 * PR20750: Please update bavc.h to reflect presence of third VEC.
 *
 * Hydra_Software_Devel/136   4/4/06 1:49p hongtaoz
 * PR20403: add triple displays for 7400;
 *
 * Hydra_Software_Devel/135   3/7/06 4:06p syang
 * PR 19670: added 7438 support
 *
 * Hydra_Software_Devel/134   3/1/06 9:44a jessem
 * PR19582: Added call to BVDC_P_Source_MpegGetStatus_isr in
 * BVDC_P_Window_Reader_isr to verify if the source has been disconnected
 * or not (bVideoDetect boolean) and if so, send NULL for the capture
 * buffer to the  user app. Also, added bMute to BVDC_P_WindowContext to
 * flag BVDC_P_Window_CapturePicture_isr to work in conjunction with the
 * above.
 *
 * Hydra_Software_Devel/133   2/24/06 9:50p albertl
 * PR18913:  Updated to use BMTH_FIX library.
 *
 * Hydra_Software_Devel/132   2/23/06 2:33p tdo
 * PR 16061: Add filter for max, mid, and min.  Change some variable names
 * to be more meaningful.  Change the max point to 94 and 95 to reduce
 * the affect of some white letters suddenly appear on screen.  Increase
 * precision of some incremental variables
 *
 * Hydra_Software_Devel/131   2/22/06 12:14p hongtaoz
 * PR19082: added shared DNR support for 7400;
 *
 * Hydra_Software_Devel/130   2/14/06 4:25p hongtaoz
 * PR19082: CSC coeffs precision is increased by one bit for 7400, so
 * should shift sin value;
 *
 * Hydra_Software_Devel/129   2/13/06 11:09a pntruong
 * PR13275, PR19258: Back out uncessary feature.  Already available with
 * window source clipping.
 *
 * Hydra_Software_Devel/128   2/8/06 3:00p hongtaoz
 * PR19082: updated CSC coef precision for 7400;
 *
 * Hydra_Software_Devel/127   2/8/06 10:40a yuxiaz
 * PR13275: Added new functions VDC_Window_Set704Sample4Vdec
 * and VDC_Window_Get704Sample4Vdec for support of 704 pixels output from
 * the VDEC on a CCIR-656 port.
 *
 * Hydra_Software_Devel/125   1/27/06 2:59p yuxiaz
 * PR 19157, PR 19138: Added BFMT_AspectRatio_eSAR support for AVC.
 * Removed unused eAspectRatio in BVDC_P_PictureNode.
 *
 * Hydra_Software_Devel/124   1/27/06 1:11p jessem
 * PR 18021: Added support for changing a window's pixel format to
 * BVDC_P_Window_Writer/Reader_isr and BVDC_P_ApplyChanges_isr
 *
 * Hydra_Software_Devel/123   1/17/06 4:19p hongtaoz
 * PR19082: first compiled for 7400;
 *
 * Hydra_Software_Devel/122   1/12/06 1:34p hongtaoz
 * PR18233: added mosaic mode support;
 *
 * Hydra_Software_Devel/MosaicMode_Feature_PR18233/3   1/12/06 11:35a hongtaoz
 * PR18233: modified mosaic mode API to be more flexible;
 *
 * Hydra_Software_Devel/121   1/12/06 10:36a yuxiaz
 * PR18476: Added new functions BVDC_Window_SetLumaKey and
 * BVDC_Window_GetLumaKey for luma key support.
 *
 * Hydra_Software_Devel/120   1/12/06 10:08a tdo
 * PR 16061: Implementing dynamic contrast stretch in the PEP
 *
 * Hydra_Software_Devel/120   1/12/06 9:58a tdo
 * PR 16061: Implementing dynamic contrast stretch in the PEP
 *
 * Hydra_Software_Devel/MosaicMode_Feature_PR18233/2   12/22/05 5:50p hongtaoz
 * PR18233: merged changes from main branch;
 *
 * Hydra_Software_Devel/MosaicMode_Feature_PR18233/1   12/8/05 6:51p hongtaoz
 * PR18233: add Mosaic mode support (single window working);
 *
 * Hydra_Software_Devel/117   11/23/05 5:28p hongtaoz
 * PR18180: added 7401 support for 2 CMP's;
 *
 * Hydra_Software_Devel/116   11/10/05 6:14p jessem
 * PR 17972, PR 18072: Changed return value of
 * BVDC_P_Window_CapturePicture_isr().
 *
 * Hydra_Software_Devel/115   11/3/05 4:59p hongtaoz
 * PR17200: collapse window z-order to real blender id; added number of
 * vwin check;
 *
 * Hydra_Software_Devel/114   10/31/05 6:26p hongtaoz
 * PR17514: add DNR support in VDC;
 *
 * Hydra_Software_Devel/113   9/12/05 2:59p jessem
 * PR 12739: Added support for user capture of fields and/or frames. New
 * functions are BVDC_P_Window_CapturePicture_isr(),
 * BVDC_P_Windwo_ReleasePciture_isr(), and
 * BVDC_P_Window_CheckForUnReturnedUserCapturedBuffer_isr().
 *
 * Hydra_Software_Devel/112   9/6/05 11:00a syang
 * PR 16868: re-org rect code to pass U4.13 fix pt PixAspR value around;
 * corrected to (SrcDspW/H) for mpeg/7411 src aspect ratio
 *
 * Hydra_Software_Devel/111   8/26/05 3:16p hongtaoz
 * PR16812, PR15495: vnet reconfig RUL needs to get executed before sync-
 * slipped playback side unmutes; mpeg source needs to build double RULs
 * during window shutdown and vnet update process; due to that, VEC IT
 * block better not stall input;
 *
 * Hydra_Software_Devel/110   8/19/05 2:51p hongtaoz
 * PR15888: adding support for dynamic switch on/off shared scaler on
 * bypass window;
 *
 * Hydra_Software_Devel/109   8/18/05 1:21p pntruong
 * PR15757, PR16391, PR16411, PR12519, PR14791, PR15535, PR15206, PR15778:
 * Improved bandwidth for cropping/scaler/capture/playback.   Unified
 * window shutdown sequence for destroy, reconfigure mad/scaler, and/or
 * reconfigure result of source changes.  And miscellances fixes from
 * above PRs.
 *
 * Hydra_Software_Devel/XVDPhase1/XVDPhase1_merge/3   8/16/05 2:02p pntruong
 * PR12519: Take in additional fixes from mainline.
 *
 * Hydra_Software_Devel/XVDPhase1/XVDPhase1_merge/2   8/2/05 1:08p pntruong
 * PR16484: (1241) Progressive outputs give flickery picture.
 *
 * Hydra_Software_Devel/XVDPhase1/XVDPhase1_merge/1   7/15/05 5:19p pntruong
 * PR12519: Prepared for merging into the main-line.  No need for capture
 * iff fullscreen mpeg going to cmp_0_v0.
 *
 * Hydra_Software_Devel/XVDPhase1/4   6/22/05 3:07p pntruong
 * PR15820, PR15563, PR15685: Added pixel frequency to aid compute
 * bandwith equation.
 *
 * Hydra_Software_Devel/XVDPhase1/3   5/20/05 11:07a pntruong
 * PR 15183: Add sharpness control to the PEP and remove the old TAB
 * setting.
 *
 * Hydra_Software_Devel/XVDPhase1/2   5/3/05 8:34p pntruong
 * PR15084: Additional work on hddvi input format change.
 *
 * Hydra_Software_Devel/XVDPhase1/1   5/2/05 4:55p pntruong
 * PR12519, PR13121, PR15048, PR15084, PR15100: Dynamically re-allocate
 * capture in bvn path to reduce memory consumption and bandwith.
 *
 * Hydra_Software_Devel/2   5/2/05 12:57p pntruong
 * Initial support for decoding of 480i for hddvi.
 *
 * Hydra_Software_Devel/1   5/1/05 4:55p pntruong
 * XVDPhase1 branch off.  Temp storage so we can keep track of changes.
 *
 * Hydra_Software_Devel/101   4/22/05 6:34p hongtaoz
 * PR14585, PR14868: allow mad/scaler to be switched between capture and
 * playback side to support correct usage mode of BVN;
 *
 * Hydra_Software_Devel/100   4/21/05 5:50p hongtaoz
 * PR14494: mute playback when vnet mode has MAD at reader side and window
 * is sync-slipped;
 *
 * Hydra_Software_Devel/99   4/19/05 5:26p yuxiaz
 * PR14875: Fix for HD_DVI source format switch. Added code to properly
 * shut down all the windows connected to HD_DVI source then reset HD_DVI
 * format. When bring the windows back up, bring up writer first, then
 * the reader.
 *
 * Hydra_Software_Devel/98   4/8/05 10:54a yuxiaz
 * PR14510: Added new shut down state to make sure VDC executed the RUL
 * that shutdown writer's modules when window is destroyed.
 *
 * Hydra_Software_Devel/97   4/1/05 5:13p pntruong
 * PR14686: Refresh mad mstart addresses on re-enable and fixed vdec 3d
 * comb mstart alignment.
 *
 * Hydra_Software_Devel/96   3/29/05 4:30p hongtaoz
 * PR14494: reserve the precision of COEF3 at calculation;
 *
 * Hydra_Software_Devel/95   3/29/05 12:56p hongtaoz
 * PR14494: fixed COEF3 macro for 3560 contrast/saturation adjust;
 *
 * Hydra_Software_Devel/94   3/28/05 5:35p hongtaoz
 * PR14494: added support for 3560 brightness/hue/saturation adjustment;
 *
 * Hydra_Software_Devel/93   3/22/05 7:34p hongtaoz
 * PR14494, PR14521: updated cmp csc matrix precision for 3560;
 *
 * Hydra_Software_Devel/92   3/17/05 6:42p pntruong
 * PR14494: Add preliminary software support to 3560 A0.
 *
 * Hydra_Software_Devel/91   3/11/05 9:23a pntruong
 * PR14112, PR14046: Fixed hd/sd channel change for progressive mode.
 *
 * Hydra_Software_Devel/90   3/8/05 7:43p hongtaoz
 * PR12769: implement non-linear horizontal scaling;
 *
 * Hydra_Software_Devel/89   2/28/05 10:55a pntruong
 * PR13575: Window destroy timeout at channel change.
 *
 ***************************************************************************/
#ifndef BVDC_WINDOW_PRIV_H__
#define BVDC_WINDOW_PRIV_H__

#include "bvdc.h"
#include "bvdc_common_priv.h"
#include "bvdc_buffer_priv.h"
#include "bmth_fix.h"
#include "bvdc_pep_priv.h"
#include "bvdc_tnt_priv.h"
#include "bvdc_subrul_priv.h"
#include "bvdc_csc_priv.h"

#if BVDC_P_SUPPORT_CM3D
#include "bvdc_cm3d_priv.h"
#endif

#if BVDC_P_SUPPORT_SEC_CMP
#include "bchp_cmp_1.h"
#endif

#if BVDC_P_SUPPORT_TER_CMP
#include "bchp_cmp_2.h"
#endif

#if BVDC_P_CMP_3_MAX_VIDEO_WINDOW_COUNT
#include "bchp_cmp_3.h"
#endif

#if BVDC_P_CMP_4_MAX_VIDEO_WINDOW_COUNT
#include "bchp_cmp_4.h"
#endif

#if BVDC_P_CMP_5_MAX_VIDEO_WINDOW_COUNT
#include "bchp_cmp_5.h"
#endif

#if BVDC_P_CMP_6_MAX_VIDEO_WINDOW_COUNT
#include "bchp_cmp_6.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

BDBG_OBJECT_ID_DECLARE(BVDC_BFH);

/***************************************************************************
 * Private register cracking macros
 ***************************************************************************/
/* Gets the offset of a register from the start of the window */
#define BVDC_P_WIN_GET_REG_WIN_OFFSET(reg) \
	(BCHP##_##reg - BCHP_CMP_0_V0_SURFACE_SIZE)

#define BVDC_P_WIN_GET_REG_IDX(reg) \
	((BVDC_P_WIN_GET_REG_WIN_OFFSET(reg) + hWindow->ulRegOffset) / \
	sizeof(uint32_t))

/* All of the video & graphics windows control registers are part of a
 * compositor. */

/* Get/Set reg data */
#define BVDC_P_WIN_GET_REG_DATA(reg) \
	(hWindow->hCompositor->aulRegs[BVDC_P_WIN_GET_REG_IDX(reg)])
#define BVDC_P_WIN_SET_REG_DATA(reg, data) \
	(BVDC_P_WIN_GET_REG_DATA(reg) = (uint32_t)(data))

#define BVDC_P_WIN_GET_REG_DATA_I(idx, reg) \
	(hWindow->hCompositor->aulRegs[BVDC_P_WIN_GET_REG_IDX(reg) + (idx)])

/* Get field */
#define BVDC_P_WIN_GET_FIELD_NAME(reg, field) \
	(BVDC_P_GET_FIELD(BVDC_P_WIN_GET_REG_DATA(reg), reg, field))

/* Compare field */
#define BVDC_P_WIN_COMPARE_FIELD_DATA(reg, field, data) \
	(BVDC_P_COMPARE_FIELD_DATA(BVDC_P_WIN_GET_REG_DATA(reg), reg, field, (data)))
#define BVDC_P_WIN_COMPARE_FIELD_NAME(reg, field, name) \
	(BVDC_P_COMPARE_FIELD_NAME(BVDC_P_WIN_GET_REG_DATA(reg), reg, field, name))

/* This macro does a write into RUL (write, addr, data). 3 dwords. */
#define BVDC_P_WIN_WRITE_TO_RUL(reg, addr_ptr) \
{ \
	*addr_ptr++ = BRDC_OP_IMM_TO_REG(); \
	*addr_ptr++ = BRDC_REGISTER(BVDC_P_WIN_GET_REG_WIN_OFFSET(reg) + hWindow->ulRegOffset + \
                  BCHP_CMP_0_REVISION + hCompositor->ulRegOffset); \
	*addr_ptr++ = BVDC_P_WIN_GET_REG_DATA(reg); \
}

#define BVDC_P_WIN_WRITE_IMM_TO_RUL(reg, imm, addr_ptr) \
{ \
	*addr_ptr++ = BRDC_OP_IMM_TO_REG(); \
	*addr_ptr++ = BRDC_REGISTER(BVDC_P_WIN_GET_REG_WIN_OFFSET(reg) + hWindow->ulRegOffset + \
                  BCHP_CMP_0_REVISION + hCompositor->ulRegOffset); \
	*addr_ptr++ = (imm); \
}

/* short hand */
#define BVDC_P_WIN_IS_VIDEO_WINDOW(window_id) \
	((BVDC_P_WindowId_eComp0_V0==window_id) || \
	 (BVDC_P_WindowId_eComp0_V1==window_id) || \
	 (BVDC_P_WindowId_eComp1_V0==window_id) || \
	 (BVDC_P_WindowId_eComp1_V1==window_id) || \
	 (BVDC_P_WindowId_eComp2_V0==window_id) || \
	 (BVDC_P_WindowId_eComp3_V0==window_id) || \
	 (BVDC_P_WindowId_eComp4_V0==window_id) || \
	 (BVDC_P_WindowId_eComp5_V0==window_id) || \
	 (BVDC_P_WindowId_eComp6_V0==window_id))

#define BVDC_P_WIN_IS_GFX_WINDOW(window_id) \
	((BVDC_P_WindowId_eComp0_G0==window_id) || \
	 (BVDC_P_WindowId_eComp0_G1==window_id) || \
	 (BVDC_P_WindowId_eComp0_G2==window_id) || \
	 (BVDC_P_WindowId_eComp1_G0==window_id) || \
	 (BVDC_P_WindowId_eComp2_G0==window_id) || \
	 (BVDC_P_WindowId_eComp3_G0==window_id) || \
	 (BVDC_P_WindowId_eComp4_G0==window_id) || \
	 (BVDC_P_WindowId_eComp5_G0==window_id) || \
	 (BVDC_P_WindowId_eComp6_G0==window_id))

extern const bool s_BVDC_abWinNonLinearTbl[];
extern const BVDC_P_MatrixCoefficients s_aCMP_MosaicCscType_To_ClrSpace_MapTbl[];
extern const BVDC_P_CmpColorSpace s_aCMP_ClrSpace_To_MosaicCscType_MapTbl[];

#define BVDC_P_WIN_IS_NONLINEAR_CSC_WIN(window_id) s_BVDC_abWinNonLinearTbl[window_id]
#define BVDC_P_WIN_MOSAIC_CSC_IDX_INVALID          BVDC_P_SUPPORT_CMP_MOSAIC_CSC
#define BVDC_P_WIN_MOSAIC_CSC_MAP_SIZE             BVDC_P_MatrixCoefficients_eMax

/* Bandwidth equation decision oscillations! */
#define BVDC_P_BW_RATE_FACTOR                    (1000)
#define BVDC_P_BW_BASE 			                 (1000)
#define BVDC_P_BW_DEFAULT_DELTA                  (10)    /* 1% */

/* Scaler output min. */
#define BVDC_P_WIN_SCL_OUTPUT_H_MIN           (8)
#define BVDC_P_WIN_SCL_OUTPUT_V_MIN           (1)

#define BVDC_P_WIN_VFD_OUTPUT_H_MIN          (16)
#define BVDC_P_WIN_VFD_OUTPUT_V_MIN           (2)
#define BVDC_P_WIN_CAP_INPUT_H_MIN           (16)
#define BVDC_P_WIN_CAP_INPUT_V_MIN            (1)
#define BVDC_P_WIN_MAD_INPUT_H_MIN           (58)
#define BVDC_P_WIN_MAD_INPUT_V_MIN            (8)
#define BVDC_P_WIN_DNR_INPUT_H_MIN           (17)
#define BVDC_P_WIN_DNR_INPUT_V_MIN            (5)
#define BVDC_P_WIN_ANR_INPUT_H_MIN           (32)
#define BVDC_P_WIN_ANR_INPUT_V_MIN            (3)
#define BVDC_P_WIN_FGT_INPUT_H_MIN            (2)
#define BVDC_P_WIN_FGT_INPUT_V_MIN            (1)
#define BVDC_P_WIN_DST_OUTPUT_H_MIN           (2)
#define BVDC_P_WIN_DST_OUTPUT_V_MIN           (1)

/* Mosaic mode min */
#define BVDC_P_WIN_MOSAIC_OUTPUT_H_MIN        (16)
#define BVDC_P_WIN_MOSAIC_OUTPUT_V_MIN        (6)

/* This is used in the assymmetric bandwidth equation */
#define BVDC_P_DIV_CEIL(x, y)   ((x + y - 1) / y)


#define BVDC_P_TestFeature1_FRACTIONAL_SHIFT  (1)
#define BVDC_P_TestFeature1_THRESHOLD         \
	((BVDC_P_72_SYSCLK / 1000000) * BFMT_FREQ_FACTOR)

/* Non-linear scaling central region ratio */
#define BVDC_P_CENTRAL_REGION_WEIGHT_PERCENTAGE  (50)

/* low pass filter defines */
#define BVDC_P_LPF_FIXED_SHIFT                   (9)
#define BVDC_P_LPF_GAIN                          (170)
#define BVDC_P_LPF_ROLLOVER                      (0x400)
#define BVDC_P_LPF_ROLLOVER_OFFSET \
	((BVDC_P_LPF_ROLLOVER << BVDC_P_LPF_FIXED_SHIFT) * BVDC_P_LPF_GAIN)

#define BVDC_P_USE_BMTH_FIX                      (0)

typedef enum
{
	BVDC_P_BufHeapType_eCapture = 0,
	BVDC_P_BufHeapType_eMad_Pixel,
	BVDC_P_BufHeapType_eMad_QM,
	BVDC_P_BufHeapType_eAnr

} BVDC_P_BufHeapType;

typedef enum
{
	/* Only need to allocate buffer for left */
	BVDC_P_BufHeapAllocMode_eLeftOnly = 0,
	/* Allocate 1 buffer for both left and right */
	BVDC_P_BufHeapAllocMode_eLRCombined,
	/* Allocate 2 buffers, 1 for left, 1 for right */
	BVDC_P_BufHeapAllocMode_eLRSeparate

} BVDC_P_BufHeapAllocMode;

/* Use for dynamic RTS. */
typedef struct
{
	bool bRequireCapture;
	bool bRequirePlayback;
	bool bRequireScaler;
	bool bRequireMad32;
	bool bRequirePep;
	BVDC_P_CaptureId eCapture;
	BVDC_P_FeederId  ePlayback;
	BVDC_P_ScalerId  eScaler;
	BVDC_P_ScalerId  eMad32;
	BVDC_P_WindowId  eWinId;

} BVDC_P_ResourceRequire;

/* Use for acquiring shared resouces Scaler/Capture/Playback/.. for a window. */
typedef struct
{
	uint32_t  ulCap;
	uint32_t  ulVfd;
	uint32_t  ulScl;
	uint32_t  ulMad;
	uint32_t  ulAnr;
} BVDC_P_ResourceFeature;

/* Active_Format Description */
typedef struct
{
	uint32_t                           ulAfd;
	uint32_t                           aulHClip[3];
	uint32_t                           aulVClip[3];
	const char                        *pchDesc;

} BVDC_P_Afd;

#define BVDC_P_MAKE_AFD(ulAfd, ulHClip0, ulVClip0, ulHClip1, ulVClip1, ulHClip2, ulVClip2, pchDesc) \
{                                                                   \
	(ulAfd),                                                        \
	{(ulHClip0),(ulHClip1), (ulHClip2)},                            \
	{(ulVClip0),(ulVClip1), (ulVClip2)},                            \
	(pchDesc)                                                       \
}

/* Bar Data Structure */
#define BVDC_P_WINDOW_ISBARDATA(ulAfd, eBarDataType) \
	(((ulAfd == 0) || (ulAfd == 4)) &&  \
	(eBarDataType != BAVC_BarDataType_eInvalid))


/****************************************************************************
 * Window dirty bits for building RUL.  New and other flags should be moving
 * as needed.
 *
 */
typedef union
{
	struct
	{
	/* Most important dirty bits. */
	uint32_t                 bRecAdjust               : 1;  /* 0 */
	uint32_t                 bReDetVnet               : 1;

	/* Color space conversion for Contrast, Hue, Brightness, and Saturation. */
	/* and/or user customize CSC */
	uint32_t                 bCscAdjust               : 1;

	/* Pep: Tab, Cab, Lab */
	uint32_t                 bTabAdjust               : 1;
	uint32_t                 bTntAdjust               : 1;  /* 4 */
	uint32_t                 bLabAdjust               : 1;
	uint32_t                 bCabAdjust               : 1;
	uint32_t                 bDitAdjust               : 1;

	/* Luma Key */
	uint32_t                 bLumaKeyAdjust           : 1;  /* 8 */

	/* Dirty bits for destroy and cleanup windows, bShutdown must be taken
	 * care first before handling bDestroy, bSrcPending, or bReConfigVnet. */
	uint32_t                 bShutdown                : 1;
	uint32_t                 bDestroy                 : 1;
	uint32_t                 bSrcPending              : 1;
	uint32_t                 bReConfigVnet            : 1;  /* 12 */

	/* Mad is dirty. */
	uint32_t                 bDeinterlace             : 1;

	/* Anr is dirty. */
	uint32_t                 bAnrAdjust               : 1;

	/* shared SCL is dirty */
	uint32_t                 bSharedScl               : 1;

	/* User capture */
	uint32_t                 bUserCaptureBuffer       : 1;  /* 16 */
	uint32_t                 bUserReleaseBuffer       : 1;

	/* mosaic mode */
	uint32_t                 bMosaicMode              : 1;

	/* dynamic buffer allocation */
	uint32_t                 bReallocBuffers          : 1;

	/* box detect */
	uint32_t                 bBoxDetect               : 1;  /* 20 */

	/* Histogram rect */
	uint32_t                 bHistoRect               : 1;

	/* FGT */
	uint32_t                 bFgt                     : 1;


	/* General user change that need flag to copy */
	uint32_t                 bMiscCtrl                : 1;

	/* Coefficient index */
	uint32_t                 bCtIndex                 : 1;  /* 24 */


#if BVDC_P_SUPPORT_CM3D
	uint32_t                 bCm3dRegionAdj           : 1;
	uint32_t                 bCm3dOverlapRegionAdj    : 1;
#endif

#if BVDC_P_SUPPORT_CM3D
	/* Chroma Histogram rect */
	uint32_t                 bChromaHistoRect         : 1;
#endif

#if (BVDC_P_SUPPORT_VIDEO_TESTFEATURE1_CAP)
	/* Video TestFeature1 is dirty. */
	uint32_t                 bTestFeature1            : 1;  /* 28 */
#endif

	uint32_t                 bBufAllocMode            : 1;
	uint32_t                 b3D                      : 1; /* 3D Stereoscopic Video/Graphics */
	uint32_t                 bStg                     : 1; /* output to STG/ViCE triggerred */

	} stBits;

	uint32_t aulInts [BVDC_P_DIRTY_INT_ARRAY_SIZE];
} BVDC_P_Window_DirtyBits;


/***************************************************************************
Summary:
	This structure describes MAD configurations.

Description:

	bReverse32Pulldown - turn on/off 3:2 reverse pulldown;
	bReverse22Pulldown - turn on/off 2:2 reverse pulldown;

	maybe more config ...

See Also:
	BVDC_Window_SetDeinterlaceConfiguration
***************************************************************************/

typedef struct
{
	BVDC_MadGameMode                   eGameMode;

	bool                               bReverse32Pulldown;
	BVDC_Deinterlace_Reverse32Settings stReverse32Settings;

	bool                               bReverse22Pulldown;
	BVDC_Deinterlace_Reverse22Settings stReverse22Settings;

	BVDC_Deinterlace_ChromaSettings    stChromaSettings;

	BVDC_Deinterlace_MotionSettings    stMotionSettings;

	/* down/up sampling. */
	BVDC_422To444UpSampler             stUpSampler;
	BVDC_444To422DnSampler             stDnSampler;

	/* low angles */
	BVDC_Deinterlace_LowAngleSettings  stLowAngles;

	BPXL_Format                        ePixelFmt;

	BVDC_Video_TestFeature1_Settings   stTestFeature1;
	bool                               bShrinkWidth;
	BVDC_Mode                          ePqEnhancement;

	/* might add other modes */
} BVDC_P_Deinterlace_Settings;


/***************************************************************************
Summary:
	Window information structure.

Description:
	This is a window public structure that holds the all the input
	parameters.

***************************************************************************/
typedef struct
{
	/* Display window attributes */
	bool                          bVisible;             /* Window visibility */
	bool                          bDeinterlace;         /* Enable de-interlacing */
	bool                          bForceCapture;        /* Enable force capture */
	uint8_t                       ucZOrder;             /* Depth ordering */
	uint8_t                       ucAlpha;              /* Window alpha value of window */
	uint8_t                       ucConstantAlpha;      /* Constant alpha blending */
	BVDC_BlendFactor              eFrontBlendFactor;    /* front blending factor */
	BVDC_BlendFactor              eBackBlendFactor;     /* back blending factor. */
	BVDC_AspectRatioMode          eAspectRatioMode;     /* Aspect Ratio Correction Mode */
	uint32_t                      ulNonlinearSrcWidth;  /* for nonlinear scale mode */
	uint32_t                      ulNonlinearSclOutWidth;/* for nonlinear scale mode */
	BVDC_PhaseAdjustMode          ePhaseAdjustMode;     /* VEC/VDEC phase adjustment mode */
	bool                          bUseSrcFrameRate;     /* Use source framerate to drive display. */
	bool                          bCscRgbMatching;      /* color space conversion with RGB matching */
	unsigned int                  uiVsyncDelayOffset;   /* offset added to default */

	BPXL_Format                   ePixelFormat;
	BVDC_P_Deinterlace_Settings   stMadSettings;
	bool                          bRev32Custom;
	bool                          bRev22Custom;
	bool                          bChromaCustom;
	bool                          bMotionCustom;
	BVDC_Video_TestFeature1_Settings     stCapTestFeature1; /* TestFeature1 for cap/vfd */

	/* For user capture */
	/* number of capture buffers added to running total of needed buffers */
	unsigned int                  uiCaptureBufCnt;
	BVDC_P_PictureNode           *pBufferFromUser;

	/* Source cissors clipping rectangle (in pixel unit) */
	BVDC_P_ClipRect               stSrcClip;

	/* Scaler output rectangle.(in pixel unit)
	 * lLeft, lTop: specify where the stDstRect on scaler output rectangle.
	 * ulWidth, ulHeight: specify the dimension of scaler output. */
	BVDC_P_Rect                   stScalerOutput;

	/* Window display size on canvas.(in pixel unit)
	 * lLeft, lTop: specify where on canvas.
	 * ulWidth, ulHeight: specify the dimension of window. */
	BVDC_P_Rect                   stDstRect;

	/* X-offset for the right window in 3D mode */
	int32_t                       lRWinXOffsetDelta;

	/* User AFD settings */
	BVDC_AfdSettings              stAfdSettings;

	/* User pan scan settings */
	BVDC_PanScanType              ePanScanType;         /* Pan scan type */
	int32_t                       lUserHorzPanScan;     /* User horizontal pan scan */
	int32_t                       lUserVertPanScan;     /* User vertical pan scan */

	/* scale factor rounding tolerance */
	uint32_t                      ulHrzSclFctRndToler;
	uint32_t                      ulVrtSclFctRndToler;

	/* box detect */
	bool                          bBoxDetect;
	bool                          bAutoCutBlack;
	BVDC_Window_BoxDetectCallback_isr BoxDetectCallBack;
	void *                        pvBoxDetectParm1;
	int                           iBoxDetectParm2;

	/* What source is this window use. */
	BVDC_Source_Handle            hSource;

	/* The following is for sharpness */
	bool                          bSharpnessEnable;
	int16_t                       sSharpness;
	bool                          bUserSharpnessConfig;
	BVDC_SharpnessSettings        stSharpnessConfig;

#if (BVDC_P_SUPPORT_TNT_VER == 5)            /* TNT2 HW base */
	BVDC_P_SharpnessData          stSharpnessPrivData;
#else
	uint32_t                      ulLumaGain;
	uint32_t                      ulSharpnessPeakSetting;
	uint32_t                      ulSharpnessPeakScale;
#endif

	/* These attributes are used for contrast stretch */
	bool                          bContrastStretch;
	BVDC_ContrastStretch          stContrastStretch;
	/* These attributes are used for blue stretch */
	bool                          bBlueStretch;
	BVDC_BlueStretch              stBlueStretch;
	bool                          bUserLabLuma;
	bool                          bUserLabCbCr;
	uint32_t                      aulLabLumaTbl[BVDC_P_LAB_TABLE_SIZE];
	uint32_t                      aulLabCbTbl[BVDC_P_LAB_TABLE_SIZE];
	uint32_t                      aulLabCrTbl[BVDC_P_LAB_TABLE_SIZE];
	/* These are the CAB parameters */
	bool                          bUserCabEnable;
	uint32_t                      ulFleshtone;
	uint32_t                      ulGreenBoost;
	uint32_t                      ulBlueBoost;
	uint32_t                      aulCabTable[BVDC_P_CAB_TABLE_SIZE];
	BVDC_ColorBar                 stSatGain;
	BVDC_ColorBar                 stHueGain;

#if BVDC_P_SUPPORT_CM3D
	/* CM3D attributes */
	bool                          bCm3dRegionEnable;
	bool                          bCm3dOverlapRegionEnable;
	BVDC_P_Cm3dRegionSettings     stRegionConfig;
	BVDC_P_Cm3dOvlpRegionSettings stOvlpRegionConfig;

	/* Chroma Hist Rect setting */
	bool                          bChromaHistEnable;
	BVDC_ChromaSettings           stChromaRect;

#endif

	/* This is used for demo mode */
	BVDC_Window_SplitScreenSettings stSplitScreenSetting;

	/* User CSC */
	bool                           bUserCsc;
	bool                           bUserCscA;
	bool                           bUserCscB;
	bool                           bUserCscC;
	uint32_t                       ulUserShift;
	uint32_t                       ulUserShiftA;
	uint32_t                       ulUserShiftB;
	uint32_t                       ulUserShiftC;
	int32_t                        pl32_Matrix[BVDC_CSC_COEFF_COUNT];
	int32_t                        pl32_MatrixA[BVDC_CSC_COEFF_COUNT];
	int32_t                        pl32_MatrixB[BVDC_CSC_COEFF_COUNT];
	int32_t                        pl32_MatrixC[BVDC_CSC_COEFF_COUNT];

	/* FGT */
	bool                           bFgt;
	BVDC_Fgt_Settings              stFgtSettings;

	/* State for shutting down window to transfer resource. */
	BVDC_P_State                  eReaderState;
	BVDC_P_State                  eWriterState;

	/* Generic Callback */
	BVDC_CallbackFunc_isr         pfGenCallback;
	void                         *pvGenCallbackParm1;
	int                           iGenCallbackParm2;

	/* PEP: Dithering */
	BVDC_DitherSettings           stDither;

	/* Color adjustment attributes */
	int16_t                       sColorTemp;
	int16_t                       sContrast;
	int16_t                       sSaturation;
	int16_t                       sHue;
	int16_t                       sBrightness;
	int32_t                       lAttenuationR;
	int32_t                       lAttenuationG;
	int32_t                       lAttenuationB;
	int32_t                       lOffsetR;
	int32_t                       lOffsetG;
	int32_t                       lOffsetB;

	/* Luma keying attributes */
	bool                          bLumaKey;
	uint8_t                       ucLumaKeyMask;
	uint8_t                       ucLumaKeyHigh;
	uint8_t                       ucLumaKeyLow;

	/* Luma Rect setting */
	bool                          bHistEnable;
	bool                          bHistAtSrc;
	bool                          bLumaRectUserSet;
	BVDC_LumaSettings             stLumaRect;

	/* MosaicMode: sub-rectangles placement and size; */
	/* bClearRect:
	 *      Enables ClearRect. This flag alone doesn't enable mosaic mode.
	 *      It's set by bEnable parameter in BVDC_Window_SetMosaicConfiguration
	 * bMosaicMode:
	 *      Enables video channels in Mosaics mode. It's set by bVideoInMosaics
	 *      in BVDC_MosaicConfiguration
	 *
	 * Mosaic mode:
	 *      bClearRect = true, bMosaicMode = true
	 * ClearRect mode:
	 *      bClearRect = true, bMosaicMode = false
	 */
	bool                          bClearRect;
	bool                          bMosaicMode;
	bool                          bClearRectByMaskColor;
	uint32_t                      ulClearRectAlpha;
	uint32_t                      ulMaskColorYCrCb;
	uint32_t                      ulMosaicCount;
	uint32_t                      ulMosaicTrackChannelId;
	bool                          abMosaicVisible[BVDC_P_MAX_MOSAIC];
	BVDC_P_Rect                   astMosaicRect[BVDC_P_MAX_MOSAIC];
	uint8_t                       aucMosaicZOrder[BVDC_P_MAX_MOSAIC];

	/* SCL: Fir coeff table Index, and related */
	BVDC_CoefficientIndex         stCtIndex;
	BVDC_Scaler_Settings          stSclSettings;

	/* Game mode delay setting */
	BVDC_Window_GameModeSettings  stGameDelaySetting;

	/* ANR settings */
	bool                          bAnr;/* Enable ANR */
	BVDC_Anr_Settings             stAnrSettings;

	/* Dirty bits of windows. */
	BVDC_P_Window_DirtyBits       stDirty;

	/* Parameters for bandwidth equation
	 */
	uint32_t                      ulBandwidthDelta;
	BVDC_SclCapBias               eSclCapBias;
} BVDC_P_Window_Info;

/***************************************************************************
Summary:
	This macro defines the polarity of the "eLastSrcPolarity" and
	"eLastLastSrcPolarity" fields before the arrival of real source
	pictures.

	Ideally such an enum should be added to BAVC_Polarity. But that
	may cause changes to other modules. So we keep it internal to VDC.
	This is only used by cadence handling.

***************************************************************************/
#define BAVC_Polarity_eInvalid    0xFF

/***************************************************************************
Summary:
	Window cadence handling structure.

***************************************************************************/
typedef struct
{
	bool                          bForceAltCap; /* Alternate capture cadence */
	BAVC_Polarity                 eLastCapPolarity;
	bool                          bReaderCadMatching; /* Cadence matching agaist VEC */
	bool                          bDecoderRateCov; /* 50Hz<->60Hz coversion by decoder */
	bool                          bTrickMode;
	bool                          bHandleFldInv;  /* smooth scl for expected field inversion */
} BVDC_P_Window_CadenceHandling;


/***************************************************************************
Summary:
	Rectangles and scaling setup delayed by MAD

***************************************************************************/
typedef struct
{
	BVDC_P_Rect                   stSclCut;
	BVDC_P_Rect                   stSclOut;

	BVDC_P_Rect                   stCapOut;
	BVDC_P_Rect                   stVfdOut;

	BVDC_P_Rect                   stWinIn;
	BVDC_P_Rect                   stWinOut;

	uint32_t                      ulNrmHrzSrcStep; /* normalized hrz scl factor in fixed point format */
	uint32_t                      ulNrmVrtSrcStep; /* normalized vrt scl factor in fixed point format */
	uint32_t                      ulNonlinearSrcWidth;/* in pxl unit */
	uint32_t                      ulNonlinearSclOutWidth;/* in pxl unit */
	uint32_t                      ulCentralRegionSclOutWidth;/* in pxl unit */

	BVDC_LumaStatus               stHistData;
	uint32_t                      ulHistSize;
	uint32_t                      ulOrigPTS;

#if (BVDC_P_SUPPORT_STG)
	bool                          bIgnorePicture;
	bool                          bChannelChange;
	bool                          bLast;
	uint32_t                      ulDecodePictureId;
	uint32_t                      ulStgPxlAspRatio_x_y;
	BAVC_USERDATA_PictureCoding   ePictureType;
	BAVC_BarDataType              eBarDataType;
	uint32_t                      ulTopLeftBarValue;
	uint32_t                      ulBotRightBarValue;
	bool                          bPreChargePicture;
	bool                          bEndofChunk;
	uint32_t                      ulChunkId;
#endif
} BVDC_P_Window_MadDelayed;

typedef struct
{
	BVDC_P_Scaler_Handle          hScaler;
	BVDC_P_Capture_Handle         hCapture;
	BVDC_P_Feeder_Handle          hPlayback;
	BVDC_P_Pep_Handle             hPep;
	BVDC_P_Dnr_Handle             hDnr;
	BVDC_P_Fgt_Handle             hFgt;
	BVDC_P_Hist_Handle            hHist;
	BVDC_P_Mcvp_Handle            hMcvp;
	BVDC_P_Hscaler_Handle         hHscaler;
	BVDC_P_Mad_Handle             hMad32;
	BVDC_P_Anr_Handle             hAnr;
#if BVDC_P_SUPPORT_CM3D
	BVDC_P_Cm3d_Handle            hCm3d;
#endif
	BVDC_P_BoxDetect_Handle       hBoxDetect;
} BVDC_P_Window_Resource;

/***************************************************************************
 * Window Context
 ***************************************************************************/
typedef struct BVDC_P_WindowContext
{
	BDBG_OBJECT(BVDC_WIN)

	/* public fields that expose thru API. */
	BVDC_P_Window_Info            stNewInfo;
	BVDC_P_Window_Info            stCurInfo;

	/* Window default setting takin from creat */
	BVDC_Window_Settings          stSettings;

	/* The three rectangles in the following struct allow user-defined
	 * dynamic aspect ratio correction.
	 * When the AspectRatioMode is Bypass, user-defined intercepting source data ready
	 * callback can do real-time aspect ratio correction by calling the three window
	 * rectangle _isr functions to set the following rectangles, which will be validated
	 * and reflected into the stCurInfo structure; otherwise they are normally not used. */
	BVDC_P_Window_Info            stIsrInfo;

	/* Set to true when new & old validated by apply changes */
	bool                          bUserAppliedChanges;
	BVDC_P_VnetMode               stVnetMode;
	BVDC_P_VnetMode               stMcvpMode;  /* for hscl, anr and mcdi in mcvp */

	/* Shadow registers and context in parent compositor. */
	BVDC_P_WindowId               eId;
	BVDC_P_State                  eState;
	uint32_t                      ulRegOffset; /* V0_CMP_0, V1_CMP_0, and V0_CMP_1.
	                                            * G0_CMP_0, G0_CMP_1.
	                                            * Offset from the start of the compositor
	                                            * the window is on */

	/* Use to determine Vnet mode. */
	bool                          bDstFullScreen;
	bool                          bCapture;
	bool                          bSyncLockSrc;
	uint32_t                      ulCapRate; /* In units of
	                                            (MHz * BFMT_FREQ_FACTOR) */
	uint32_t                      ulFeedRate;

	/* eBufferHeapIdRequest defines the buffer size. eBufferHeapIdPrefer is
	 * the prefered buffer heap Id. The actual buffer heap used
	 * maybe different if the prefered one is not available. */
	BVDC_P_BufferHeapId           eBufferHeapIdRequest;
	BVDC_P_BufferHeapId           eBufferHeapIdPrefer;
	uint32_t                      ulBufDelay; /* total capture buffer delay in Vsyncs */
	BVDC_P_Buffer_Handle          hBuffer;

	/* Internal VDC or App handed down. */
	BVDC_Heap_Handle              hHeap;

	/* BVN video processing delay until display, use to config lipsync. */
	uint32_t                      ulBufCntNeeded;
	uint32_t                      ulPrevBufCntNeeded;
	bool                          bBufCntNeededChanged;
	/* Buffers allocated for multi-buffering and user capture */
	uint32_t                      ulBufCntAllocated;
	BVDC_P_BufHeapAllocMode       eBufAllocMode;
	BVDC_P_BufHeapAllocMode       ePrevBufAllocMode;
	bool                          bRightBufAllocated;

	/* Progressive mode buffer count support */
	bool                          bBufferCntDecremented;
	bool                          bBufferCntDecrementedForPullDown;

	/* needed for interlace with rate gap mode since this mode requires 1 extra buffer */
	bool                          bBufferCntIncremented;

	/* Using the other hardware block.  This could either be part of the
	 * source handle or window.  For now we treat of it as part of the
	 * window.  These will be created when we create window context. */
	BVDC_P_Window_Resource        stCurResource;
	BVDC_P_Window_Resource        stNewResource;
	bool                          bAllocResource;
	const BVDC_P_ResourceRequire *pResource;
	const BVDC_P_ResourceFeature *pResourceFeature;

	/* box detect stuff */
	uint32_t                      ulBoxDetectCallBckCntr;

	/* Created from this Window */
	BVDC_Compositor_Handle        hCompositor;

	/* event handle to be set when a window state transitions from
	 * Shutdown to Inactive */
	BKNI_EventHandle              hDestroyDoneEvent;
	bool                          bSetDestroyEventPending;

	/* Event to nofify that changes has been applied to hardware. */
	BKNI_EventHandle              hAppliedDoneEvent;
	bool                          bSetAppliedEventPending;

	/* user pan scan display size calculated at ValidateChanges */
	uint32_t                      ulAutoDispHorizontalSize; /* calculated src display width */
	uint32_t                      ulAutoDispVerticalSize;   /* calculated src display height */
	uint32_t                      ulFieldSrcWidth;   /* for calcu user disp size */
	uint32_t                      ulFieldSrcHeight;  /* for calcu user disp size */
	uint32_t                      eFieldSrcAspectR;  /* for calcu user disp size */
	uint16_t                      uiSampleAspectRatioX;  /* for calcu pixel aspect ratio */
	uint16_t                      uiSampleAspectRatioY;  /* for calcu pixel aspect ratio */

	/* stSrcCnt, stAdjSclOut, AdjDstRect are adjusted src clip, scaler-out
	 * and dst rectangles, according to box auto cut, pan-scan, aspect ratio
	 * correction, and scale factor rounding. They are private info, and are
	 * the ones finally used to build picture nodes and RULs. They are updated
	 * at every vsync for mpeg or hddvi src, or when box auto cut is enabled,
	 * and once at the vsync after ApplyChanges for the other video cases (e.g.
	 * analog src without box auto cut).
	 *
	 * stSrcCnt's lTop and lLeft values are in U27.4 fixed point format; but
	 * ulWidth and ulHeight are in pixel unit */
	BVDC_P_Rect                   stSrcCnt;
	BVDC_P_Rect                   stAdjSclOut;
	BVDC_P_Rect                   stAdjDstRect;
	BVDC_P_Rect                   stPrevSrcCnt; /* intermim, for optimization */
	uint32_t                      ulNonlinearSrcWidth;/* for non-linear scaling */
	uint32_t                      ulNonlinearSclOutWidth;/* for non-linear scaling */
	uint32_t                      ulCentralRegionSclOutWidth;/* for non-linear scaling */
	uint32_t                      ulNrmHrzSrcStep;  /* normalized scl factor: U11.21 format */
	uint32_t                      ulNrmVrtSrcStep;  /* normalized scl factor: U11.21 format */
	bool                          bAdjRectsDirty; /* for optimization */
	BAVC_MatrixCoefficients       ePrevMatrixCoeffs;

	/* pixel aspect ratio sent to ViCE2 */
	uint32_t                      ulStgPxlAspRatio_x_y; /* PxlAspR_x<<16 | PxlAspR_y */

	/* for MAD and ANR buffer reallocation */
	BVDC_P_BufferHeapId           eAnrHeapId;
	BVDC_P_BufferHeapId           eMadPixelHeapId;  /* Mad Pixel Field buffer ID */
	BVDC_P_BufferHeapId           eMadQmHeapId;     /* Mad Quantized Motion Field buffer ID */
	uint16_t                      usMadPixelBufferCnt;
	uint16_t                      usAnrPixelBufferCnt;

	/* for SclCut setting delay according to MAD pixel output vsync delay */
	bool                          bResetMadDelaySwPipe;
	uint32_t                      ulDeferIdxWr; /* writer index to stMadDelayed */
	uint32_t                      ulDeferIdxRd; /* reader index to stMadDelayed */
	BVDC_P_Window_MadDelayed      stMadDelayed[4]; /* circle buf for mad-delayed rects and scl stuff */

	/* to overide pic bRepeatField to false when clipping before MAD changed */
	int32_t                       lPrevSrcOutLeft;  /* srcOut.lLeft last vsync */
	int32_t                       lPrevSrcOutTop;  /* srcOut.lTop last vsync */
	int32_t                       lPrevCapOutLeft;  /* CapOut.lLeft last vsync */
	int32_t                       lPrevCapOutTop;  /* CapOut.lTop last vsync */
	uint32_t                      ulMadFlushCntr; /* to flush change out during freeze */

	/* Vdec phase low pass filter attributes */
	int32_t                       lRawVdecPhase;
	bool                          bVdecFlipPhase;
	int32_t                       lLpfLastResult;
	int32_t                       lLpfLastVdecPhase;
	int32_t                       alLpfSamples[3];
	/* to synchronize the src vnet RUL change with syncslip playback unmuting */
	uint32_t                      ulVnetUpdateCnt;

	/* real blender used for this window */
	uint32_t                      ulBlenderId;
	uint32_t                      ulPrevBlenderId;

	/* to mute BVDC_P_WindowId_eCompB_V0 */
	bool                          bMuteBypass;

	/* for capture buffer use*/
	unsigned int                  uiAvailCaptureBuffers;
	bool                          bMute;

	/* Lip sync */
	bool                          bRepeatCurrReader;
	BVDC_P_PictureNode           *pCurWriterNode;
	BVDC_P_PictureNode           *pCurReaderNode;
	BVDC_Window_CallbackSettings  stCallbackSettings;
	uint32_t                      ulTotalBvnDelay; /* total BVN delay in Vsyncs from input to output */

	/* Game mode buffer delay */
	uint32_t                      ulCurBufDelay; /* absolute delay from capture to playback in usec */
	int32_t                       lCurGameModeLag;/* ulCurBufDelay - game mode target */
	bool                          bAdjGameModeClock;
	bool                          bCoarseAdjClock;
	bool                          bFastAdjClock;

	/* Window information prepare to hand back to user when it's changed. */
	BVDC_Window_CallbackData      stCallbackData;

	/* MosaicMode: clear rect mask set */
	uint32_t                      ulMosaicRectSet;
	bool                          bClearRectSupport;

	/* sub-struct to manage vnet for the compositor's win src */
	BVDC_P_SubRulContext          stWinOutVnet;

	/* Capture pictures as frames. This is needed when source and display
	 * rates and polarities differ.  Are we doing pulldown? */
	bool                          bDoPulldown;
	bool                          bFrameCapture;

	/* CMP CSC */
	BVDC_P_CscCoeffs              stPriCsc;
	BVDC_P_CscCoeffs              stPriCscA;
	BVDC_P_CscCoeffs              stPriCscB;
	BVDC_P_CscCoeffs              stPriCscC;
	BVDC_P_CscCoeffs              stSecCsc;
	BVDC_P_CscCoeffs              stSecCscB;
	BVDC_P_CscCoeffs              stSecCscC;

	BVDC_P_CscCoeffs              stInvUserCscC;

#if BVDC_P_SUPPORT_CMP_MOSAIC_CSC
	BVDC_P_CscCoeffs              astMosaicCscList[BVDC_P_SUPPORT_CMP_MOSAIC_CSC];
	BAVC_MatrixCoefficients       aeMosaicClrSpaceList[BVDC_P_MAX_MOSAIC];
	BAVC_MatrixCoefficients       aePrevMosaicClrSpaceList[BVDC_P_MAX_MOSAIC];

	/* Size should match number of BAVC_MatrixCoefficients enums */
	BVDC_P_CmpColorSpace          aeClrSpaceToMosaicCscMap[BVDC_P_WIN_MOSAIC_CSC_MAP_SIZE];
	BAVC_MatrixCoefficients       aeMosaicCscToClrSpaceMap[BVDC_P_SUPPORT_CMP_MOSAIC_CSC];
#endif

	/* These are the final values that used to calculate the CSC */
	/* These values are user setting modified by the custom picture */
	/* enhancement algorithms */
	int16_t                       sFinalContrast;
	int16_t                       sFinalSaturation;
	int16_t                       sFinalBrightness;

	/* FGT SEI data is only valid for 1 frame.
	 * Remove later when XVD can hold SEI data longer */
	uint32_t                      ulFgtSeiBufAddr;

	/* This flag is to indicate if the new HIST hardware is available */
	/* This is chip specific */
	bool                          bHistAvail;

	/* This flag indicate if the bandwidth equation is symmetric or not */
	bool                          bSclCapSymmetric;

#if BVDC_P_SUPPORT_CM3D
	/* This flag is to indicate if the new Chroma Histogram hardware is available */
	/* This is chip specific */
	bool                          bChromaHistAvail;
#endif

	/* Fields for cadence handling.
	 */
	BVDC_P_Window_CadenceHandling stCadHndl;

#if BVDC_P_SUPPORT_MOSAIC_MODE
	/* capture drain buffer for mosaic mode */
	void                          *pvNullBufAddr;
	uint32_t                       ulNullBufOffset;
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)
	uint32_t                       aulMosaicZOrderIndex[BVDC_P_MAX_MOSAIC];
#endif
#endif

	uint32_t                       ulDropCntNonIgnoredPics;
} BVDC_P_WindowContext;

/***************************************************************************
 * Captured Picture Info
 ***************************************************************************/
typedef struct
{
	/* the virtual address of the buffer of the corresponding picture */
	void                         *pvBufAddr;
	void                         *pvBufAddr_R;
	BFMT_Orientation              eDispOrientation;
	uint32_t                      ulHeight;
	uint32_t                      ulWidth;
	uint32_t                      ulPitch;
	BPXL_Format                   ePxlFmt;
	uint32_t                      ulNumVbiLines;
	BAVC_Polarity                 ePolarity;
	uint32_t                      ulOrigPTS;
	BVDC_P_PictureNode           *pPicture;

} BVDC_P_Window_CapturedPicture;


/***************************************************************************
 * Window private functions
 ***************************************************************************/
BERR_Code BVDC_P_Window_Create
	( BVDC_Compositor_Handle           hCompositor,
	  BVDC_Window_Handle              *phWindow,
	  BVDC_P_WindowId                  eWindowId );

void BVDC_P_Window_Destroy
	( BVDC_Window_Handle               hWindow );

void BVDC_P_Window_Init
	( BVDC_Window_Handle               hWindow,
	  BVDC_Source_Handle               hSource );

void BVDC_P_Window_Sharpness_Init
	( BVDC_Window_Handle               hWindow,
	  BVDC_SharpnessSettings          *pSharpnessConfig );

void BVDC_P_Window_BuildRul_isr
	( BVDC_Window_Handle               hWindow,
	  BVDC_P_ListInfo                 *pList,
	  BAVC_Polarity                    eNextFieldId,
	  bool                             bBuildWriter,
	  bool                             bBuildReader,
	  bool                             bBuildCanvasCtrl );

BERR_Code BVDC_P_Window_ValidateChanges
	( const BVDC_Window_Handle         hWindow,
	  const BFMT_VideoInfo            *pDstFmtInfo );

BERR_Code BVDC_P_Window_ApplyChanges_isr
	( BVDC_Window_Handle               hWindow );

void BVDC_P_Window_ValidateRects_isr
	( const BVDC_Window_Handle         hWindow,
	  const BAVC_MVD_Field            *pFieldData );

void BVDC_P_Window_InitMuteRec_isr
	( const BVDC_Window_Handle         hWindow,
	  const BFMT_VideoInfo            *pDstFmtInfo,
	  const BAVC_MVD_Field            *pMvdFieldData,
	  uint32_t                         ulRectIdx );

void BVDC_P_Window_AdjustRectangles_isr
	( const BVDC_Window_Handle         hWindow,
	  const BAVC_MVD_Field            *pMvdFieldData,
	  const BAVC_VDC_HdDvi_Picture    *pXvdFieldData,
	  uint32_t                         ulPicCnt );

void BVDC_P_Window_UpdateState_isr
	( BVDC_Window_Handle               hWindow );

void BVDC_P_Window_UpdateUserState_isr
	( BVDC_Window_Handle               hWindow );

void BVDC_P_Window_AlignPreMadRect_isr
	( int32_t                          lMin,
	  int32_t                          lMin_R,
	  uint32_t                         ulSize,
	  uint32_t                         ulFullSize,
	  uint32_t                         ulMinFracBits,
	  uint32_t                         ulAlignUnit,
	  int32_t                         *plNewMin,
	  int32_t                         *plNewMin_R,
	  uint32_t                        *pulNewSize );

#if (BVDC_P_MADR_PICSIZE_WORKAROUND)
void BVDC_P_Window_PreMadAdjustWidth_isr
	( uint32_t                  ulPicWidth,
	  uint32_t                  ulPicHeight,
	  uint32_t                  ulBitsPerPixel,
	  uint32_t                 *pulNewPicWidth );
#endif

void BVDC_P_Window_GetSourceContentRect_isr
	( const BVDC_Window_Handle         hWindow,
	  const BAVC_MVD_Field            *pMvdFieldData,
	  const BAVC_VDC_HdDvi_Picture    *pXvdFieldData,
	  int32_t                         *plWinXMin,
	  int32_t                         *plWinYMin,
	  int32_t                         *plWinXMin_R,
	  int32_t                         *plWinXMax,
	  int32_t                         *plWinYMax );

void BVDC_P_Window_SetDisplaySize_isr
	( BVDC_Window_Handle               hWindow,
	  const BVDC_P_Rect               *pDstRect,
	  BAVC_Polarity                    eScanType,
	  uint32_t                         ulRWinXOffsetulRWinXOffset);

BERR_Code BVDC_P_Window_EnableBoxDetect
	( BVDC_Window_Handle               hWindow,
	  BVDC_Window_BoxDetectCallback_isr  BoxDetectCallBack,
	  void                            *pvParm1,
	  int                              iParm2,
	  bool                             bAutoCutBlack );

BERR_Code BVDC_P_Window_DisableBoxDetect
	( BVDC_Window_Handle               hWindow );

BERR_Code BVDC_P_Window_GetPrivHandle
	( BVDC_Compositor_Handle           hCompositor,
	  BVDC_WindowId                    eWindowId,
	  BAVC_SourceId                    eSrcId,
	  BVDC_Window_Handle              *phWindow );

/* For gfx applychanges. */
void BVDC_P_Window_GetNewRectangles
	( const BVDC_Window_Handle         hWindow,
	  const BVDC_P_ClipRect **         ppClipRect,
	  const BVDC_P_Rect **             ppSclOutRect,
	  const BVDC_P_Rect **             ppDstRect );

void BVDC_P_Window_GetNewRectangles_isr
	( const BVDC_Window_Handle         hWindow,
	  const BVDC_P_ClipRect **         ppClipRect,
	  const BVDC_P_Rect **             ppSclOutRect,
	  const BVDC_P_Rect **             ppDstRect );

void BVDC_P_Window_GetCurrentRectangles_isr
	( const BVDC_Window_Handle         hWindow,
	  const BVDC_P_ClipRect **         ppClipRect,
	  const BVDC_P_Rect **             ppSclOutRect,
	  const BVDC_P_Rect **             ppDstRect );

void BVDC_P_Window_GetCurrentRectangles
	( const BVDC_Window_Handle         hWindow,
	  const BVDC_P_ClipRect **         ppClipRect,
	  const BVDC_P_Rect **             ppSclOutRect,
	  const BVDC_P_Rect **             ppDstRect );

void BVDC_P_Window_GetCurrentDestColorSpace
	( const BVDC_Window_Handle         hWindow,
	  BVDC_P_CmpColorSpace            *peDestColorSpace );

void BVDC_P_Window_GetNewWindowAlpha
	( const BVDC_Window_Handle         hWindow,
	  uint8_t                         *pucWindowAlpha );

void BVDC_P_Window_GetNewDispOrientation
	( const BVDC_Window_Handle         hWindow,
	  BFMT_Orientation                 *pOrientation);

void BVDC_P_Window_GetNewBlendFactor
	( const BVDC_Window_Handle         hWindow,
	  BVDC_BlendFactor                *peFrontBlendFactor,
	  BVDC_BlendFactor                *peBackBlendFactor,
	  uint8_t                         *pucConstantAlpha );

void BVDC_P_Window_GetNewScanType
	( const BVDC_Window_Handle         hWindow,
	  bool                            *pbInterlaced );

/* For RUL building. */
void BVDC_P_Window_Writer_isr
	( BVDC_Window_Handle               hWindow,
	  const BAVC_MVD_Field            *pMvdFieldData,
	  const BAVC_VDC_HdDvi_Picture    *pXvdFieldData,
	  const BAVC_Polarity              eFieldId,
	  BVDC_P_ListInfo                 *pList,
	  uint32_t                         ulPictureIndex );

void BVDC_P_Window_Reader_isr
	( BVDC_Window_Handle               hWindow,
	  const BAVC_Polarity              eFieldId,
	  BVDC_P_ListInfo                 *pList );

bool BVDC_P_Window_GetReconfiguring_isr
	( BVDC_Window_Handle               hWindow );

void BVDC_P_Window_SetReconfiguring_isr
	( BVDC_Window_Handle               hWindow,
	  bool                             bSrcPending,
	  bool                             bReConfigVnet );

BERR_Code BVDC_P_Window_CapturePicture_isr
	( BVDC_Window_Handle               hWindow,
	 BVDC_P_Window_CapturedPicture    *pCapturedPic );

BERR_Code BVDC_P_Window_ReleasePicture_isr
	( BVDC_Window_Handle hWindow,
	  BSUR_Surface_Handle              hCaptureBuffer );

bool BVDC_P_Window_CheckForUnReturnedUserCapturedBuffer_isr
	( BVDC_Window_Handle               hWindow );

BERR_Code BVDC_P_Window_ColorTempToAttenuationRGB
	( int16_t                          sColorTemp,
	  int32_t                         *plAttenuationR,
	  int32_t                         *plAttenuationG,
	  int32_t                         *plAttenuationB );

BERR_Code BVDC_P_Window_SetColorMatrix
	( BVDC_Window_Handle               hWindow,
	  const int32_t                    pl32_UserMatrix[BVDC_CSC_COEFF_COUNT],
	  int32_t                          pl32_WinMatrix[BVDC_CSC_COEFF_COUNT] );

BERR_Code BVDC_P_Window_GetColorMatrix
	( BVDC_Window_Handle               hWindow,
	  bool                             bOverride,
	  BVDC_P_CscCoeffs                *pCscCoeffsWin,
	  int32_t                          pl32_MatrixWin[BVDC_CSC_COEFF_COUNT],
	  int32_t                          pl32_Matrix[BVDC_CSC_COEFF_COUNT] );

void BVDC_P_Window_CalculateCsc_isr
	( BVDC_Window_Handle               hWindow );

void BVDC_P_Window_SetSecCscDemo_isr
	( BVDC_Window_Handle               hWindow );

	BERR_Code BVDC_P_Window_SetMcvp_DeinterlaceConfiguration
		(BVDC_Window_Handle               hWindow,
		bool                             bDeinterlace,
		const BVDC_Deinterlace_Settings *pMadSettings);

	BERR_Code BVDC_P_Window_SetMad_DeinterlaceConfiguration
		(BVDC_Window_Handle               hWindow,
		bool                             bDeinterlace,
		const BVDC_Deinterlace_Settings *pMadSettings);

void BVDC_P_Window_CalculateMosaicCsc_isr
	( BVDC_Window_Handle               hWindow );

BERR_Code BVDC_P_Window_ConfigureMadGameMode_isr
	( const BVDC_Window_Handle         hWindow);

void BVDC_P_Window_SetInvalidVnetMode
	( BVDC_P_VnetMode                  *pVnetMode );

#if (BVDC_P_SUPPORT_VIDEO_TESTFEATURE1_MAD_ANR)
BERR_Code BVDC_P_Window_SetMadAnrTestFeature1_isr
	( BVDC_Window_Handle                       hWindow,
	  BPXL_Format                              ePxlFormat,
	  const BVDC_P_Rect                       *pRect,
	  const BVDC_P_PictureNodePtr              pPicture,
	  const BVDC_Video_TestFeature1_Settings  *pUserVideoTestFeature1,
	  bool                                     bWriter,
	  BVDC_P_TestFeature1_Settings             *pNewTestFeature1 );
#endif

#ifdef __cplusplus
}
#endif

#endif /* #ifndef BVDC_WINDOW_PRIV_H__ */
/* End of file. */
