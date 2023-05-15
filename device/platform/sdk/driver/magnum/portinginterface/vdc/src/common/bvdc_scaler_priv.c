 /***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvdc_scaler_priv.c $
 * $brcm_Revision: Hydra_Software_Devel/270 $
 * $brcm_Date: 9/5/12 2:07p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vdc/src/common/bvdc_scaler_priv.c $
 * 
 * Hydra_Software_Devel/270   9/5/12 2:07p vanessah
 * SW7425-3864: disable double-buffering in BVN
 * 
 * Hydra_Software_Devel/269   8/15/12 6:21p tdo
 * SW7445-8: Create Appframework emulation environment for 7445 A0
 *
 * Hydra_Software_Devel/268   6/26/12 5:03p vanessah
 * SW7435-249: get rid of scl/dnr back-to-back enable/disable
 *
 * Hydra_Software_Devel/267   6/15/12 10:35a vanessah
 * SW7435-237: reset vnet channel connections
 *
 * Hydra_Software_Devel/266   5/30/12 6:12p tdo
 * SW7425-1210: SCL: Veritical Block Averaging theshold should be base on
 * scaler's veritical tap.
 *
 * Hydra_Software_Devel/265   3/26/12 10:57a yuxiaz
 * SW7425-2753: Fixed BVN errors for 3d conversion.
 *
 * Hydra_Software_Devel/264   2/7/12 1:51p vanessah
 * SW7425-2296: fix the HD/SD vert line depth error
 *
 * Hydra_Software_Devel/263   2/7/12 1:40p vanessah
 * SW7425-2296: fix the HD/SD vert line depth error
 *
 * Hydra_Software_Devel/262   2/7/12 11:05a vanessah
 * SW7425-2296: removal of duplicate vert line store depth variable
 *
 * Hydra_Software_Devel/261   2/6/12 9:35p vanessah
 * SW7425-2296: use scaler to do Orientation conversion
 *
 * Hydra_Software_Devel/260   12/1/11 4:33p yuxiaz
 * SW7425-968, SW7344-95: Merged into mainline.: added independent source
 * clipping of right window in 3D mode.
 *
 * Hydra_Software_Devel/SW7425-968/1   11/14/11 3:07p yuxiaz
 * SW7425-968: Added independent source clipping of right window in CAP
 * and SCL.
 *
 * Hydra_Software_Devel/259   9/8/11 11:12p tdo
 * SW7425-922: VDC bringup for B0
 *
 * Hydra_Software_Devel/258   9/7/11 12:53p tdo
 * SW7425-1210: Undo previous check in, not working with old SCL yet.
 *
 * Hydra_Software_Devel/256   8/30/11 11:12a pntruong
 * SW7425-1210: Updated block averaging theshold to be based on number of
 * tap uses.
 *
 * Hydra_Software_Devel/255   8/22/11 1:16p pntruong
 * SW7425-982: Initialized new field (corn) in dejagging to recommended
 * value.
 *
 * Hydra_Software_Devel/254   8/15/11 4:10p pntruong
 * SW7425-982: Implement de-ringing and de-jagging improvements for SCL.
 *
 * Hydra_Software_Devel/253   5/16/11 12:47p vanessah
 * SW7425-569:fix scale ratio underflow problem
 *
 * Hydra_Software_Devel/252   5/6/11 10:53a pntruong
 * SW7425-414, SW7346-155: Split screen demo boundary should be based on
 * ouptut size instead of input size.
 *
 * Hydra_Software_Devel/251   4/11/11 11:21a vanessah
 * SW7425-178:change the alignment back
 *
 * Hydra_Software_Devel/250   4/6/11 11:52a vanessah
 * SW7425-178:SW7231-63: video freeze on non-linear filter
 *
 * Hydra_Software_Devel/249   1/14/11 2:01p vanessah
 * SW7408-209:  a little bit clean up and negative value fix for 3 stage
 * non-linear fiter
 *
 * Hydra_Software_Devel/248   1/14/11 10:09a vanessah
 * SW7408-209:  3 stage 5 stage non-linear filter compatible issue fix
 *
 * Hydra_Software_Devel/247   1/14/11 9:50a vanessah
 * SW7408-209:  3 stage 5 stage non-linear filter compatible issue fix
 *
 * Hydra_Software_Devel/246   1/4/11 4:08p yuxiaz
 * SW7422-174: Refactor DCX macros.
 *
 * Hydra_Software_Devel/245   12/16/10 11:39p pntruong
 * SW3548-2987: Added 1080p@30hz 3DOU_AS hdmi input support.
 *
 * Hydra_Software_Devel/244   12/9/10 11:21a yuxiaz
 * SW7422-36: Bring up 3D for CAP-SCL case.
 *
 * Hydra_Software_Devel/243   12/6/10 2:51p yuxiaz
 * SW7422-36: Bring up SCL for 3D on 7422.
 *
 * Hydra_Software_Devel/242   11/11/10 7:29p albertl
 * SW7125-364: Fixed BVDC_P_CbIsDirty and added assert to check bitfields
 * in dirty bits fit within union integer representation.  Fixed naming
 * of dirty bits.
 *
 * Hydra_Software_Devel/241   11/11/10 4:15p tdo
 * SW7422-13: Bringup SD SCL for 7422
 *
 * Hydra_Software_Devel/240   11/10/10 6:56p hongtaoz
 * SW7425-13: fixed SCL vertical scaling ratio and offset programming;
 * reuse the legacy fixed point precision for bringup; TODO: improve
 * fixed point math to support higher precision VSCL;
 *
 * Hydra_Software_Devel/239   10/25/10 5:07p tdo
 * SW7422-36: Handle "left only" method for BVN internal data path
 *
 * Hydra_Software_Devel/238   10/25/10 2:57p pntruong
 * SW7420-619: Leave csc dither off until further clarification of usage
 * that does not introduce side-effects.
 *
 * Hydra_Software_Devel/237   9/15/10 7:51p vanessah
 * SW7422-49:  5-zone nonlinear filtering support. backwards compatible
 * with 3-zone compatibility
 *
  * Hydra_Software_Devel/236   8/26/10 5:27p tdo
 * SW7422-57: Add simple vdc support
 *
 * Hydra_Software_Devel/235   7/13/10 11:27a yuxiaz
 * SW3548-2987: Added new 1080p@30hz support for 3DTV.
 *
 * Hydra_Software_Devel/234   6/17/10 5:37p hongtaoz
 * SW3548-2912: added support for pseudo-1080i input for 3DTV; allow API
 * to disable SCL vertical phase shift;
 *
 * Hydra_Software_Devel/233   5/12/10 1:18p pntruong
 * SW7420-721: Used softer coeffs for when mcdi is used.
 *
 * Hydra_Software_Devel/232   5/7/10 7:20p albertl
 * SW7125-364: Changed dirty bits to use union structure to avoid type-pun
 * warnings
 *
 * Hydra_Software_Devel/231   4/19/10 10:20p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic. Move
 * BDBG_OBJECT_ID_DECLARE private header files instead of .c.
 *
 * Hydra_Software_Devel/230   4/15/10 4:54p hongtaoz
 * SW3548-2828: changed the normalized fixed point match precision to
 * accomandate 12-bit picture size for 2160i;
 *
 * Hydra_Software_Devel/229   4/7/10 11:34a tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic.  Rename TLA
 *
 * Hydra_Software_Devel/227   4/5/10 4:13p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic
 *
 * Hydra_Software_Devel/226   2/18/10 4:55p syang
 * SW7420-560: don't override user coeff choice
 *
 * Hydra_Software_Devel/225   2/18/10 4:38p syang
 * SW7420-560: use smoothen vert coeff to improve mcdi weaving with SD src
 *
 * Hydra_Software_Devel/224   2/8/10 5:37p pntruong
 * SW3548-2772: Refresh check of both luma/chroma coeffs set.
 *
 * Hydra_Software_Devel/223   2/1/10 6:42p hongtaoz
 * SW3548-2678, SW3548-2680, SW3548-2681: added HDMI1.4 3D formats support
 * for 3548; added multi-buffer low delay mode support for 1to2 and 2to5
 * rate gap cases; reduced capture buffer allocation for interlaced
 * pulldown case;
 *
 * Hydra_Software_Devel/BCM3D/3   1/29/10 6:56p hongtaoz
 * SW3548-2680, SW3548-2681: removed 1470p custom formats since they are
 * the same as 720p3D formats; added macro to wrap the 3548 specific
 * legacy 3DTV format; reduced window buffer allocation needed for
 * pulldown case; disable hddvi DE if invalid HVstart;
 *
 * Hydra_Software_Devel/BCM3D/2   1/15/10 6:35p hongtaoz
 * SW3548-2680, SW3548-2681: added custom formats to support HDMI 1.4 3D
 * 720p50/60 input passed-thru to external panel processor; reanmed 2160i
 * to custom DVI format;
 *
 * Hydra_Software_Devel/BCM3D/1   12/17/09 6:09p hongtaoz
 * SW3548-2680, SW3548-2681: added HDMI1.4 3D format detection and
 * handling for 11-bit BVN;
 *
 * Hydra_Software_Devel/222   2/1/10 10:11a syang
 * SW7550-230:  get rid of RUL gap among vert coeffs and hrz coeff
 *
 * Hydra_Software_Devel/221   12/8/09 11:45p pntruong
 * SW3548-2657: [PQ] Horizontal scaler filter does not work on just scan
 * mode.(dtv 1080i).
 *
 * Hydra_Software_Devel/220   11/19/09 10:16a pntruong
 * SW7408-13: Initial check in to get 7408 build.
 *
 * Hydra_Software_Devel/219   11/17/09 2:44p rpan
 * SW7468-20: 7468 work.
 *
 * Hydra_Software_Devel/218   10/9/09 5:29p syang
 * SW7405-2516, SW3548-2439, SW7405-3046: rewrite HSCL setinfo_isr and
 * TryXferHrzSclToHscl_isr to do crop correctly; cleanup width alignment
 * for DCX_HSIZE_WORKAROUND
 *
 * Hydra_Software_Devel/217   9/3/09 10:32p pntruong
 * SW3548-2373: Rolled back PR56964.
 *
 * Hydra_Software_Devel/216   9/3/09 3:17p rpan
 * SW3548-2439: Add back the turning off filter restriction.
 *
 * Hydra_Software_Devel/215   9/3/09 3:06p rpan
 * SW3548-2439: Removed a workaround used for determing wheather filter
 * should be disabled.
 *
 * Hydra_Software_Devel/214   9/2/09 6:03p rpan
 * SW3548-2439: Add more restrictions on turning off filter.
 *
 * Hydra_Software_Devel/213   8/26/09 3:27p pntruong
 * SW7340-37: Fixed programming of scaler block average.
 *
 * Hydra_Software_Devel/212   7/30/09 10:35a pntruong
 * PR57216: Added support for scaler split screen demo modes.
 *
 * Hydra_Software_Devel/211   7/21/09 2:18p syang
 * PR 56964: take off the "connect drainer, en and then dis" step  in
 * vnet-drain code
 *
 * Hydra_Software_Devel/210   6/26/09 1:52p darnstein
 * PR56218: compilation for 7342 chipset needed a simple #define.
 *
 * Hydra_Software_Devel/209   6/18/09 5:51p syang
 * PR 55877, PR 55871, PR 56127, PR 55812: update to support 7340 and 7550
 *
 * Hydra_Software_Devel/208   6/11/09 5:52p pntruong
 * PR55926: Initialized new vars.
 *
 * Hydra_Software_Devel/207   6/11/09 4:37p syang
 * PR 55926: make sure changed vert coeff is built into RUL
 *
 * Hydra_Software_Devel/206   6/11/09 4:03p darnstein
 * PR55219: fill in some details for 7340 chipset.
 *
 * Hydra_Software_Devel/205   6/11/09 11:39a syang
 * PR 55926:  avoid using point-sample if init-phase is not int
 *
 * Hydra_Software_Devel/204   6/2/09 3:03p syang
 * PR 55646: 1). smoothly filtering  in field inversion cases; 2).
 * use 0.25 as 1st pixel vertical center for interlaced field
 *
 * Hydra_Software_Devel/203   5/28/09 2:19p yuxiaz
 * PR55364: Disable dithering when any testfeature1 is enabled
 *
 * Hydra_Software_Devel/202   5/21/09 3:01p tdo
 * PR36666: Consider Kell Factor for SCL Coefficient selection
 *
 * Hydra_Software_Devel/201   5/20/09 11:25a tdo
 * PR36666: Consider Kell Factor for SCL Coefficient selection
 *
 * Hydra_Software_Devel/200   4/6/09 3:42p syang
 * PR  53484 , PR 52037: cleanup (rect, scl-ratio, ..) rounding code and
 * AdjustRect code ; add 1 line delay to TV chip "early trigger"; change
 * BVDC_P_DST_CUT_TOP_THRESH to 4 for 3548;
 *
 * Hydra_Software_Devel/199   4/3/09 4:34p yuxiaz
 * PR53460: Optimal 444/422 conversion settings
 *
 * Hydra_Software_Devel/198   3/20/09 3:12p tdo
 * PR53338: Turn off FIR in unity scaling case for chips with new HW
 * coeffs interpolation
 *
 * Hydra_Software_Devel/197   3/20/09 12:27p syang
 * PR53338: remove the typo "-3"
 *
 * Hydra_Software_Devel/196   3/17/09 4:15p tdo
 * PR53338: Fix MCVP branch merge issue
 *
 * Hydra_Software_Devel/195   3/16/09 10:49p tdo
 * PR45785, PR45789: Merge from MCVP branch
 *
 * Hydra_Software_Devel/7420_mcvp/6   3/16/09 1:01p tdo
 * PR45785, PR45789: merge from main branch on 3/16/09
 *
 * Hydra_Software_Devel/194   3/13/09 5:29p syang
 * PR 52376, PR 41481, PR 44041:
 * 1). exec user buf returning to hBuffer before shut-down code in
 * writer_isr; 2). clean up mad-delay handling code and add handling to
 * all settings in pic-node that are delayed by mad; 3). 444 SCL no-
 * longer uses BVN_IN to align-up for half-band; 4). add pic-node rects
 * dump feature as setting lbox_0_scratch to 2 (1 still printing mpeg pic
 * info).
 *
 * Hydra_Software_Devel/7420_mcvp/5   3/2/09 4:21p tdo
 * PR 45785, PR 45789: merge from main branch on 3/02/09
 *
 * Hydra_Software_Devel/193   3/2/09 3:46p yuxiaz
 * PR52287: More update on 422-444 and 444-422 converters.
 *
 * Hydra_Software_Devel/7420_mcvp/4   2/26/09 11:18p tdo
 * PR 45785, PR 45789: merge from main branch on 2/26/09
 *
 * Hydra_Software_Devel/192   2/26/09 6:11p yuxiaz
 * PR52287: 422-444 and 444-422 converters should use decimate/duplicate.
 *
 * Hydra_Software_Devel/7420_mcvp/3   2/26/09 4:10p tdo
 * PR 45785, PR 45789: merge from main branch on 2/26/09
 *
 * Hydra_Software_Devel/191   2/26/09 3:11p rpan
 * PR52289: Sanity check to make sure scaler V sizes are inline.
 *
 * Hydra_Software_Devel/7420_mcvp/2   1/29/09 5:01p syang
 * PR 45785, PR 45789: merge from main branch on 1/29/09
 *
 * Hydra_Software_Devel/190   1/27/09 8:58p tdo
 * PR51627: add VDC 7336 PI support
 *
 * Hydra_Software_Devel/7420_mcvp/1   1/23/09 11:31p syang
 * PR 45785, PR 45789: add PI support for new the new module  MCVP
 * (MCTF+MCDI)
 *
 * Hydra_Software_Devel/189   11/25/08 11:13a syang
 * PR 49562: add reset to output channel ping-pong buf of sub-modules and
 * loopbacks and free-channels
 *
 * Hydra_Software_Devel/188   11/20/08 9:31a syang
 * PR 48970: clean-up module disable and drian handling so that RUL dump
 * is easy to understand
 *
 * Hydra_Software_Devel/187   11/4/08 7:10p pntruong
 * PR48029: Added control for gain/horz/core weight factor for dejagging.
 * Enforced dejagging on only if passed threshold.
 *
 * Hydra_Software_Devel/186   10/28/08 9:14p pntruong
 * PR48028, PR48029: Allow controlling of scaler's dejagging and
 * deringing.
 *
 * Hydra_Software_Devel/185   10/27/08 5:38p syang
 * PR 46477: back off SetEnable in in BuildRul_isr according to GetOps (it
 * causes window to disapper on 7400 D0)
 *
 * Hydra_Software_Devel/184   10/24/08 4:08p syang
 * PR 46477: in BuildRul_isr only disable in shadow regs  according to
 * GetOps (some SetEnable_isr does HW reset)
 *
 * Hydra_Software_Devel/183   10/24/08 11:05a syang
 * PR 46477: in BuildRul_isr reset enable bit according to GetOps
 *
 * Hydra_Software_Devel/182   10/22/08 1:59p hongtaoz
 * PR48143: only build necessary CAP/DNR/HSCL/SCL RULs when their
 * operational flags are enabled; this fixed SCL/HSCL enable error;
 *
 * Hydra_Software_Devel/181   10/20/08 1:47p pntruong
 * PR48032: Apply Kell factor for progressive to interlaced scaling case.
 *
 * Hydra_Software_Devel/180   10/16/08 3:52p tdo
 * PR45802: Support SCL Changes for 7420
 *
 * Hydra_Software_Devel/179   10/9/08 2:51p syang
 * PR 42693:  perform even numebr align for SCL-1/2 according to eId (for
 * 3548 B0)
 *
 * Hydra_Software_Devel/178   10/3/08 10:47a pntruong
 * PR47528: Fined tune the dejagging settings.
 *
 * Hydra_Software_Devel/177   9/24/08 6:19p pntruong
 * PR47113, PR45308: Allow flexibility in selecting builtins and user
 * defined filter coefficients.
 *
 * Hydra_Software_Devel/176   9/24/08 1:16p tdo
 * PR46482: Change FIR programming for SCL block for 3548 B0
 *
 * Hydra_Software_Devel/175   9/23/08 4:03p tdo
 * PR46486: Program 444->422 downsampler and 422->444 upsampler for video
 * scaler for 3548 B0
 *
 * Hydra_Software_Devel/174   9/19/08 3:56p tdo
 * PR46481: Add dejagging to SCL implementation for 3548 B0
 *
 * Hydra_Software_Devel/173   9/17/08 3:47p tdo
 * PR46482: B0:Change FIR programming for SCL block
 *
 * Hydra_Software_Devel/172   9/9/08 9:03p tdo
 * PR45802: Remove unnecessary macros and unused code
 *
 * Hydra_Software_Devel/171   9/2/08 11:53a rpan
 * PR45319: Covert picture node flags to bit-fields to reduce structure
 * size.
 *
 * Hydra_Software_Devel/170   8/22/08 10:27a rpan
 * PR45979: Remove VBI passthru support.
 *
 * Hydra_Software_Devel/169   8/8/08 3:30p yuxiaz
 * PR45484: Enable Dithering in VDC.
 *
 * Hydra_Software_Devel/168   8/4/08 2:32p yuxiaz
 * PR45367: Enable Dering in VDC.
 *
 * Hydra_Software_Devel/167   7/29/08 7:49p pntruong
 * PR40213: Used correct alignment macro.
 *
 * Hydra_Software_Devel/166   7/7/08 1:55p pntruong
 * PR44617: Update 4-taps linebuffer threshold in SCL.
 *
 * Hydra_Software_Devel/165   6/26/08 7:18p pntruong
 * PR44134: Allow user selection of builtin coeffs from 01->26 for
 * increasing of softness -> sharpness.
 *
 * Hydra_Software_Devel/164   4/22/08 5:13p tdo
 * PR41243: SCL Vertical Init Phase Adjustment For i2i 50->60 Rate
 * Conversion
 *
 * Hydra_Software_Devel/163   2/29/08 4:19p yuxiaz
 * PR39158: Implement dithering in various BVN components for 3548.
 *
 * Hydra_Software_Devel/162   2/22/08 2:33p yuxiaz
 * PR39155: Added de-ringing filters in BVN.
 *
 * Hydra_Software_Devel/161   11/19/07 3:49p tdo
 * PR36898: Add VDC PI support for 7335
 *
 * Hydra_Software_Devel/160   11/13/07 12:44p yuxiaz
 * PR34523: 7325 bring up.
 *
 * Hydra_Software_Devel/159   9/20/07 5:43p syang
 * PR 29617: cleanup ulAlgnSrcHSize
 *
 * Hydra_Software_Devel/158   9/19/07 4:57p syang
 * PR 29617:  align offset to even only if 444 is not supported, align up
 * srcHSize only if half-band filter is on
 *
 * Hydra_Software_Devel/157   9/19/07 1:38p syang
 * PR 29617: align SrcHSize to even pixel only if SCL does not support 444
 *
 * Hydra_Software_Devel/156   9/11/07 6:10p syang
 * PR 32094, PR 32621, PR 33942: clean up non-linear scl code and fix it
 * for 7403
 *
 * Hydra_Software_Devel/155   8/31/07 1:46p pntruong
 * PR32487: Updated scaler overview comments.
 *
 * Hydra_Software_Devel/154   7/31/07 2:21p hongtaoz
 * PR33653: mask SCL cropping error;
 *
 * Hydra_Software_Devel/153   7/31/07 1:59p pntruong
 * PR32487: Vertical scaler needs to be programmed to low-pass filter
 * chroma for MAD source.  Added debug to print out name of selected
 * coeffs set.
 *
 * Hydra_Software_Devel/152   7/26/07 12:03p pntruong
 * PR33520: [SCL] Need to update the scale line buffer macro.
 *
 * Hydra_Software_Devel/151   6/8/07 11:32a syang
 * PR 31849: separate drain from disable for MAD, ANR and CAP
 *
 * Hydra_Software_Devel/150   5/19/07 9:43p pntruong
 * PR28897: C0: BVN: move SCALER_ENABLE bit from TOP_CONTROL register to a
 * separate register.  Need to include in rul.
 *
 * Hydra_Software_Devel/149   5/4/07 5:18p pntruong
 * PR28372: Need to trigger selection of new coeffs by added inputtype and
 * index.
 *
 * Hydra_Software_Devel/148   5/2/07 9:46a syang
 * PR 28541:  fix 7440 compile error
 *
 * Hydra_Software_Devel/147   5/1/07 4:42p syang
 * PR 28541: 1) changed non-linear scl api; 2). carry sub-pxl accuracy
 * through the pipe line by SrcHStep and SrcVStep
 *
 * Hydra_Software_Devel/146   4/26/07 2:36p yuxiaz
 * PR27644, PR29767: Added 7405 support: move SCALER_ENABLE bit from
 * TOP_CONTROL register to a separate register.
 *
 * Hydra_Software_Devel/145   4/6/07 11:36a pntruong
 * PR29194: Added the new coefficients.
 *
 * Hydra_Software_Devel/144   4/5/07 4:55p albertl
 * PR29194:  Added BVDC_Window_SetCoefficientIndex and input and index
 * matching support to scaler coefficient tables.
 *
 * Hydra_Software_Devel/143   4/2/07 11:30a syang
 * PR 29243: clean up: take off the obselete old-vnet-code
 *
 * Hydra_Software_Devel/142   3/21/07 4:22p syang
 * PR 22563: add error msg to inform app to change CntrSrcPcnt to avoid HW
 * underflow
 *
 * Hydra_Software_Devel/141   3/5/07 3:35p tdo
 * PR 19774, PR 26113:  Clean TAB dirty bit when it is not supported by
 * the scaler to avoid window timeout
 *
 * Hydra_Software_Devel/140   2/23/07 6:30p tdo
 * PR 27970: Share LPB and FCH if source is the same to eliminate the out
 * of resource error when number of LPB and FCH are limited
 *
 * Hydra_Software_Devel/139   2/16/07 3:38p syang
 * PR 22563: make sure src cntrl percent change take effect
 *
 * Hydra_Software_Devel/138   2/12/07 1:50p pntruong
 * PR27821:  Remove unused lagacy and work-around code (7038) from
 * mainline.
 *
 * Hydra_Software_Devel/137   2/9/07 5:32p syang
 * PR 27054: don't build rul when op=eReleaseHandle. (why we see some
 * module enabled in shadow register when other modules in vnet are not
 * yet???)
 *
 * Hydra_Software_Devel/136   1/22/07 3:14p syang
 * PR 27054: 1). only 1 win build rul, 2). reader's modules don't build
 * rul before video sur enabled; 3). writer's modules don't build rul
 * before (GetReconfiguring_isr==true )
 *
 * Hydra_Software_Devel/135   1/12/07 11:52a syang
 * PR 22563: new scl arch use dest to decide region end
 *
 * Hydra_Software_Devel/134   1/10/07 11:00a syang
 * PR 22569:  add new vnet algorithm code to sub-modules such as scl, mad,
 * dnr, mfd, vfd, cap, ...
 *
 * Hydra_Software_Devel/133   1/5/07 5:45p syang
 * PR 22569:  fix compile error for 7400/7403/7440
 *
 * Hydra_Software_Devel/132   1/4/07 9:20a syang
 * PR 26107: added 7400 B0 to support 1% scale and enable_by _pic
 *
 * Hydra_Software_Devel/131   12/18/06 11:22p pntruong
 * PR22577: Merged back to mainline.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/4   11/22/06 3:52p syang
 * PR 26151: re-write shared resource manager. update letter box, mad, dnr
 * shared-scl acquire/release code accordingly
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/3   10/23/06 2:45p tdo
 * PR 23384: Clean up PEP dirty bit set
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/2   9/27/06 11:28p pntruong
 * PR 23216: fix compile warning with release build.
 *
 * Hydra_Software_Devel/129   9/27/06 3:59p syang
 * PR 24241: add component_only path for 7118's prim vec
 *
 * Hydra_Software_Devel/128   9/22/06 6:03p syang
 * PR 23216: fix compile warning with release build
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/1   9/20/06 12:48p syang
 * PR 20884: add SCL chroma coeff
 *
 * Hydra_Software_Devel/127   9/16/06 7:04p syang
 * PR 23216: partial preparing for moving half band horiz scl to MAD
 *
 * Hydra_Software_Devel/126   8/30/06 5:58p syang
 * PR 22559: turns on SCL reg double buff feature for 3563/7118
 *
 * Hydra_Software_Devel/125   8/28/06 2:22p tdo
 * PR 22574: Move TNT build isr to compositor.  TNT is part of comp_0
 *
 * Hydra_Software_Devel/124   8/24/06 1:58p tdo
 * PR 22574: Add support for TNT block.  Add support for
 * BVDC_P_SUPPORT_TAB and BVDC_P_SUPPORT_TNT macro
 *
 * Hydra_Software_Devel/123   8/23/06 4:23p syang
 * PR 22563: add new smooth non-linear scaling support for 3563; prepare
 * to allow user to set non-linear scl central region percent; move un-
 * known asp-ratio default handling to window's pixel-asp-ratio code
 *
 * Hydra_Software_Devel/122   8/16/06 9:27a syang
 * PR 23023: non-linear scale code is not ready for 3563 yet, turn off
 *
 * Hydra_Software_Devel/121   8/15/06 4:11p syang
 * PR 23023: fix compile warning for 7038
 *
 * Hydra_Software_Devel/120   8/15/06 3:16p syang
 * PR 23023: fix compile error for 7401
 *
 * Hydra_Software_Devel/119   8/15/06 11:42a syang
 * PR 23023: cleanup scale code: align width only when needed, choose
 * vertical coeff basing on correct src height, avoid negative lLeft/lTop
 * (that causes overflow)
 *
 * Hydra_Software_Devel/118   8/7/06 5:33p pntruong
 * PR22577: Initial bringup of VDC.
 *
 * Hydra_Software_Devel/117   7/20/06 10:35a syang
 * PR 22510: added 1 % down-scale support for 7401b0
 *
 * Hydra_Software_Devel/116   7/14/06 6:24p syang
 * PR 22256: correct vertical double clip
 *
 * Hydra_Software_Devel/115   7/14/06 1:53p syang
 * PR 22256: ensure that ulBvbInHSize >= ulSrcHSize + lLeft
 *
 * Hydra_Software_Devel/114   7/11/06 4:21p syang
 * PR 22256: set SCL_0_HORIZ_FIR_LUMA_SRC_PIC_OFFSET according to
 * SCL_0_PIC_OFFSET_HSIZE
 *
 * Hydra_Software_Devel/113   6/16/06 5:29p hongtaoz
 * PR21892: fixed source cropping at SCL for 7401B0;
 *
 * Hydra_Software_Devel/112   6/14/06 1:43p syang
 * PR 21689: add support for 7118
 *
 * Hydra_Software_Devel/111   6/2/06 3:24p yuxiaz
 * PR21892: 7401 B0 bring up: Use VDC_P_SUPPORT_BVB_IN_SIZE. Add
 * BVB_IN_SIZE and PIC_OFFSET for scaler.
 *
 * Hydra_Software_Devel/110   5/26/06 3:42p pntruong
 * PR20642: Refactored handling of hList.
 *
 * Hydra_Software_Devel/109   5/19/06 1:48p pntruong
 * PR21560: Global symbols without BVDC prefix in the BVDC module.  Reduce
 * unnecessary global exported symbols.  Tighten const params.
 *
 * Hydra_Software_Devel/108   3/21/06 11:04a jessem
 * PR 18147: Removed BDBG_ASSERT(ulDstVSize < BFMT_1080I_HEIGHT) in
 * BVDC_P_Scaler_SetInfo_isr to support 1080p output format.
 *
 * Hydra_Software_Devel/107   1/31/06 5:21p pntruong
 * PR17778: Take in changes for new hddvi.
 *
 * Hydra_Software_Devel/103   1/18/06 11:04a hongtaoz
 * PR18233: added src/dst polarity as SCL optimizational condition;
 *
 * Hydra_Software_Devel/102   1/12/06 1:33p hongtaoz
 * PR18233: added mosaic mode support;
 *
 * Hydra_Software_Devel/MosaicMode_Feature_PR18233/2   1/12/06 11:34a hongtaoz
 * PR18233: modified mosaic mode API to be more flexible;
 *
 * Hydra_Software_Devel/101   1/12/06 9:58a tdo
 * PR 16061: Implementing dynamic contrast stretch in the PEP
 *
 * Hydra_Software_Devel/100   10/4/05 9:48a pntruong
 * PR17385: Added support for vertical/horizontal size matching in
 * bvdc_scaler_priv.c for coeffs selection.  Checked for source match +/-
 * delta.
 *
 * Hydra_Software_Devel/99   10/3/05 4:35p pntruong
 * PR17385: Added support for vertical/horizontal size matching in
 * bvdc_scaler_priv.c for coeffs selection.  Fixed top panscan for block-
 * averaging mode.
 *
 * Hydra_Software_Devel/98   9/8/05 1:07p hongtaoz
 * PR16812: SCL rul always programs TOP_CONTROL register; optimized SCL
 * muting rul to the minimum;
 *
 * Hydra_Software_Devel/97   8/18/05 1:18p pntruong
 * PR15757, PR16391, PR16411, PR12519, PR14791, PR15535, PR15206, PR15778:
 * Improved bandwidth for cropping/scaler/capture/playback.   Unified
 * window shutdown sequence for destroy, reconfigure mad/scaler, and/or
 * reconfigure result of source changes.  And miscellances fixes from
 * above PRs.
 *
 * Hydra_Software_Devel/XVDPhase1/XVDPhase1_merge/5   8/17/05 12:43p pntruong
 * PR12519: Take in additional support for 7401.
 *
 * Hydra_Software_Devel/XVDPhase1/XVDPhase1_merge/4   8/2/05 1:07p pntruong
 * PR16484: (1241) Progressive outputs give flickery picture.
 *
 * Hydra_Software_Devel/XVDPhase1/XVDPhase1_merge/3   7/14/05 8:52p pntruong
 * PR12519: Take in changes from mainline before merge back.
 *
 * Hydra_Software_Devel/XVDPhase1/XVDPhase1_merge/2   7/14/05 8:49p pntruong
 * PR12519: Take in changes from mainline before merge back.
 *
 * Hydra_Software_Devel/XVDPhase1/XVDPhase1_merge/1   7/14/05 7:58p pntruong
 * PR12519: Take in changes from mainline before merge back.
 *
 * Hydra_Software_Devel/XVDPhase1/7   6/30/05 2:44p pntruong
 * PR15284, PR15535, PR15757: Used the recommended 32-byte align
 * pitch/stride for optimal memory bandwidth utilization.  Fixed scaling
 * clipping with vertical scaling first.
 *
 * Hydra_Software_Devel/XVDPhase1/6   6/26/05 2:54p pntruong
 * PR15988, PR15989: VDC hangs going from 480i source to 720p source for
 * hddvi source.
 *
 * Hydra_Software_Devel/XVDPhase1/5   6/15/05 1:50p syang
 * PR 15847:  vert 2 tap filter coeff interp HW problem work around: copy
 * phase0 tap2 coeff to tap0
 *
 * Hydra_Software_Devel/XVDPhase1/4   5/17/05 11:42a pntruong
 * PR15321: Continuous quick channel change will result in blank video
 * output, but graphics continue to work.  When reset scaler the tab is
 * also part of the scaler that need to be reset as well.
 *
 * Hydra_Software_Devel/XVDPhase1/3   5/3/05 10:58p pntruong
 * PR12519: Fixed handling of dirty so that event can be set.
 *
 * Hydra_Software_Devel/XVDPhase1/2   5/3/05 10:14p pntruong
 * PR12519: More optimizations.
 *
 * Hydra_Software_Devel/XVDPhase1/1   5/2/05 4:54p pntruong
 * PR12519, PR13121, PR15048, PR15084, PR15100: Dynamically re-allocate
 * capture in bvn path to reduce memory consumption and bandwith.
 *
 * Hydra_Software_Devel/1   5/1/05 4:52p pntruong
 * XVDPhase1 branch off.  Temp storage so we can keep track of changes.
 *
 * Hydra_Software_Devel/90   4/15/05 8:10p hongtaoz
 * PR14691: reset video window's path when shutdown;
 *
 * Hydra_Software_Devel/89   4/14/05 10:24a syang
 * PR 14692: re-organize picture's rectangles, fixed duplicated clips in
 * diff modules, changed to do clip as soon as possible, and added
 * capture clipping for C0
 *
 * Hydra_Software_Devel/88   3/17/05 6:40p pntruong
 * PR14494: Add preliminary software support to 3560 A0.
 *
 * Hydra_Software_Devel/87   3/8/05 7:42p hongtaoz
 * PR12769: implement non-linear horizontal scaling;
 *
 * Hydra_Software_Devel/86   2/11/05 4:32p hongtaoz
 * PR13793, PR12790, PR13321: fixed stress channel change and format
 * switch tests failure; delay the forced trigger when sync-locked mpeg
 * source switches locked compositor; disabled MAD when destroying sync-
 * locked deinterlaced window; temporary turn off scaler's
 * STALL_DRAIN_ENABLE bit on C0 to avoid color swap during stessful test;
 * improved sync-lock assignments;
 *
 * Hydra_Software_Devel/85   2/4/05 2:40p pntruong
 * PR14024: Incorrect clipping occurs after an initial clip has been done.
 *
 ***************************************************************************/
#include "bstd.h"
#include "bkni.h"
#include "bvdc.h"
#include "brdc.h"
#include "bvdc_scaler_priv.h"
#include "bvdc_buffer_priv.h"
#include "bvdc_source_priv.h"
#include "bvdc_window_priv.h"
#include "bvdc_compositor_priv.h"
#include "bvdc_pep_priv.h"
#include "bvdc_vnet_priv.h"
#include "bchp_mmisc.h"



BDBG_MODULE(BVDC_SCL);
BDBG_OBJECT_ID(BVDC_SCL);

/* Dither settings for Scaler */
#define BVDC_P_DITHER_SCL_LFSR_VALUE                     (0x70001)
#define BVDC_P_DITHER_SCL0_HORZ_LFSR_CTRL_T0             (0x1)
#define BVDC_P_DITHER_SCL0_HORZ_LFSR_CTRL_T1             (0x6)
#define BVDC_P_DITHER_SCL0_HORZ_LFSR_CTRL_T2             (0x7)
#define BVDC_P_DITHER_SCL0_VERT_LFSR_CTRL_T0             (0x4)
#define BVDC_P_DITHER_SCL0_VERT_LFSR_CTRL_T1             (0x4)
#define BVDC_P_DITHER_SCL0_VERT_LFSR_CTRL_T2             (0x7)

#define BVDC_P_DITHER_SCL1_HORZ_LFSR_CTRL_T0             (0x3)
#define BVDC_P_DITHER_SCL1_HORZ_LFSR_CTRL_T1             (0x3)
#define BVDC_P_DITHER_SCL1_HORZ_LFSR_CTRL_T2             (0x7)
#define BVDC_P_DITHER_SCL1_VERT_LFSR_CTRL_T0             (0x4)
#define BVDC_P_DITHER_SCL1_VERT_LFSR_CTRL_T1             (0x1)
#define BVDC_P_DITHER_SCL1_VERT_LFSR_CTRL_T2             (0x7)

#define BVDC_P_DITHER_SCL2_HORZ_LFSR_CTRL_T0             (0x6)
#define BVDC_P_DITHER_SCL2_HORZ_LFSR_CTRL_T1             (0x5)
#define BVDC_P_DITHER_SCL2_HORZ_LFSR_CTRL_T2             (0x6)
#define BVDC_P_DITHER_SCL2_VERT_LFSR_CTRL_T0             (0x3)
#define BVDC_P_DITHER_SCL2_VERT_LFSR_CTRL_T1             (0x5)
#define BVDC_P_DITHER_SCL2_VERT_LFSR_CTRL_T2             (0x6)

/* SW7420-560, SW7420-721: use smoothen vertical coefficient to improve weaving */
#if (BVDC_P_SUPPORT_MCVP_VER == 1)
static const uint32_t s_fir_04taps_08_v[] =
{
	0x0BC8021C, 0x0BA40128, 0x0AD40080, 0x09980018,
	0x081C3FE4, 0x067C3FD4, 0x04DC3FD0, 0x03643FD0,
	BVDC_P_SCL_LAST
};

static const uint32_t s_fir_04taps_10_v[] =
{
	0x0CE00190, 0x0CAC00A4, 0x0BA40010, 0x0A243FC4,
	0x08503FB0, 0x06643FB4, 0x048C3FC0, 0x02E83FC8,
	BVDC_P_SCL_LAST
};

static const BVDC_P_FirCoeffTbl s_aSmoothVertCoeffTbl[] =
{
	BVDC_P_CT_USER_SELECTABLE(1, BVDC_P_CT_4_TAP, s_fir_04taps_08_v), /* SD */
	BVDC_P_CT_USER_SELECTABLE(2, BVDC_P_CT_4_TAP, s_fir_04taps_10_v), /* HD */
};
#endif


/***************************************************************************
 * {private}
 *
 */
BERR_Code BVDC_P_Scaler_Create
	( BVDC_P_Scaler_Handle            *phScaler,
	  BVDC_P_ScalerId                  eScalerId,
	  BVDC_P_Resource_Handle           hResource,
	  BREG_Handle                      hReg )
{
	BVDC_P_ScalerContext *pScaler;

	BDBG_ENTER(BVDC_P_Scaler_Create);

	BDBG_ASSERT(phScaler);

	/* Use: to see messages */
	/* BDBG_SetModuleLevel("BVDC_SCL", BDBG_eMsg); */

	/* The handle will be NULL if create fails. */
	*phScaler = NULL;

	/* Alloc the context. */
	pScaler = (BVDC_P_ScalerContext*)
		(BKNI_Malloc(sizeof(BVDC_P_ScalerContext)));
	if(!pScaler)
	{
		return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
	}

	/* Clear out the context and set defaults. */
	BKNI_Memset((void*)pScaler, 0x0, sizeof(BVDC_P_ScalerContext));
	BDBG_OBJECT_SET(pScaler, BVDC_SCL);

	pScaler->eId          = eScalerId;
#if (BVDC_P_SUPPORT_SCL >= 2)
	pScaler->ulRegOffset  = BVDC_P_SCL_GET_REG_OFFSET(eScalerId);
#else
	pScaler->ulRegOffset  = 0;
#endif

	pScaler->hReg = hReg;

	/* Init to the default filter coeffficient tables. */
	BVDC_P_GetFirCoeffs_isr(&pScaler->pHorzFirCoeffTbl, &pScaler->pVertFirCoeffTbl);
	BVDC_P_GetChromaFirCoeffs_isr(&pScaler->pChromaHorzFirCoeffTbl, &pScaler->pChromaVertFirCoeffTbl);

	/* Scaler reset address */
#if BVDC_P_SUPPORT_NEW_SW_INIT
	pScaler->ulResetRegAddr = BCHP_MMISC_SW_INIT;
	pScaler->ulResetMask    = BCHP_MMISC_SW_INIT_SCL_0_MASK << (pScaler->eId);
	pScaler->ulVnetResetAddr  = BCHP_MMISC_VNET_B_CHANNEL_SW_INIT;
	pScaler->ulVnetResetMask  = BCHP_MMISC_VNET_B_CHANNEL_SW_INIT_SCL_0_MASK <<
		(pScaler->eId - BVDC_P_ScalerId_eScl0);
#else
	pScaler->ulResetRegAddr = BCHP_MMISC_SOFT_RESET;
	pScaler->ulResetMask    = BCHP_MMISC_SOFT_RESET_SCL_0_MASK << (pScaler->eId);
#if BCHP_MMISC_VNET_B_CHANNEL_RESET
	pScaler->ulVnetResetAddr = BCHP_MMISC_VNET_B_CHANNEL_RESET;
	pScaler->ulVnetResetMask = BCHP_MMISC_VNET_B_CHANNEL_RESET_SCL_0_RESET_MASK <<
		(pScaler->eId - BVDC_P_ScalerId_eScl0);
#endif
#endif
#if (BVDC_P_SUPPORT_TAB)
	if(BVDC_P_ScalerId_eScl0 == pScaler->eId)
	{
#ifdef BCHP_MMISC_SOFT_RESET_TAB_MASK
		pScaler->ulResetMask |= BCHP_MMISC_SOFT_RESET_TAB_MASK;
#else
		pScaler->ulResetMask |= BCHP_MMISC_SOFT_RESET_TAB_0_MASK;
#endif
	}
#endif

	/* init the SubRul sub-module */
	BVDC_P_SubRul_Init(&(pScaler->SubRul), BVDC_P_Scaler_MuxAddr(pScaler),
		BVDC_P_Scaler_PostMuxValue(pScaler), BVDC_P_DrainMode_eBack, 0, hResource);

	/* All done. now return the new fresh context to user. */
	*phScaler = (BVDC_P_Scaler_Handle)pScaler;

	BDBG_LEAVE(BVDC_P_Scaler_Create);
	return BERR_SUCCESS;
}


/***************************************************************************
 * {private}
 *
 */
void BVDC_P_Scaler_Destroy
	( BVDC_P_Scaler_Handle             hScaler )
{
	BDBG_ENTER(BVDC_P_Scaler_Destroy);
	BDBG_OBJECT_ASSERT(hScaler, BVDC_SCL);

	BDBG_OBJECT_DESTROY(hScaler, BVDC_SCL);
	/* Release context in system memory */
	BKNI_Free((void*)hScaler);

	BDBG_LEAVE(BVDC_P_Scaler_Destroy);
	return;
}


/***************************************************************************
 * {private}
 *
 */
void BVDC_P_Scaler_Init
	( BVDC_P_Scaler_Handle             hScaler,
	  BVDC_Window_Handle               hWindow )
{
	uint32_t  ulHorzLfsrCtrlT0, ulHorzLfsrCtrlT1, ulHorzLfsrCtrlT2, ulHorzLfsrValue;
	uint32_t  ulVertLfsrCtrlT0, ulVertLfsrCtrlT1, ulVertLfsrCtrlT2, ulVertLfsrValue;
	uint32_t  ulReg;
	uint32_t  ulTaps;

	BDBG_ENTER(BVDC_P_Scaler_Init);
	BDBG_OBJECT_ASSERT(hScaler, BVDC_SCL);

	hScaler->ulUpdateAll   = BVDC_P_RUL_UPDATE_THRESHOLD;
	hScaler->ulUpdateCoeff = BVDC_P_RUL_UPDATE_THRESHOLD;

	/* Clear out shadow registers. */
	BKNI_Memset((void*)hScaler->aulRegs, 0x0, sizeof(hScaler->aulRegs));

	hScaler->hWindow = hWindow;

	/* Initialize state. */
	hScaler->bInitial          = true;
	hScaler->pPrevVertFirCoeff = NULL;

#if (BVDC_P_SUPPORT_SCL_VER >= BVDC_P_SUPPORT_SCL_VER_4)
	ulReg = BREG_Read32(hScaler->hReg, BCHP_SCL_0_HW_CONFIGURATION + hScaler->ulRegOffset);

	hScaler->bDeJagging = BCHP_GET_FIELD_DATA(ulReg, SCL_0_HW_CONFIGURATION, DEJAGGING) ? true : false;
	hScaler->bDeRinging = BCHP_GET_FIELD_DATA(ulReg, SCL_0_HW_CONFIGURATION, DERINGING) ? true : false;

	ulTaps = BCHP_GET_FIELD_DATA(ulReg, SCL_0_HW_CONFIGURATION, HORIZ_TAPS);
	switch(ulTaps)
	{
		case BCHP_SCL_0_HW_CONFIGURATION_HORIZ_TAPS_HORIZ_8_TAPS:
			hScaler->ulHorzTaps = BVDC_P_CT_8_TAP;
			break;
		case BCHP_SCL_0_HW_CONFIGURATION_HORIZ_TAPS_HORIZ_12_TAPS:
			hScaler->ulHorzTaps = BVDC_P_CT_12_TAP;
			break;
		case BCHP_SCL_0_HW_CONFIGURATION_HORIZ_TAPS_HORIZ_16_TAPS:
			hScaler->ulHorzTaps = BVDC_P_CT_16_TAP;
			break;
		default:
			BDBG_ERR(("SCL[%d] Unknown horizontal tap configuration detected: %d", hScaler->eId, ulTaps));
			BDBG_ASSERT(0);
			break;
	}

	ulTaps = BCHP_GET_FIELD_DATA(ulReg, SCL_0_HW_CONFIGURATION, VERT_TAPS);
	switch(ulTaps)
	{
		case BCHP_SCL_0_HW_CONFIGURATION_VERT_TAPS_VERT_4_TAPS:
			hScaler->ulVertTaps = BVDC_P_CT_4_TAP;
			hScaler->ulVertBlkAvgThreshold = BVDC_P_SCL_4TAP_BLK_AVG_VERT_THRESHOLD;
			break;
		case BCHP_SCL_0_HW_CONFIGURATION_VERT_TAPS_VERT_6_TAPS:
			hScaler->ulVertTaps = BVDC_P_CT_6_TAP;
			hScaler->ulVertBlkAvgThreshold = BVDC_P_SCL_6TAP_BLK_AVG_VERT_THRESHOLD;
			break;
		case BCHP_SCL_0_HW_CONFIGURATION_VERT_TAPS_VERT_8_TAPS:
			hScaler->ulVertTaps = BVDC_P_CT_8_TAP;
			hScaler->ulVertBlkAvgThreshold = BVDC_P_SCL_8TAP_BLK_AVG_VERT_THRESHOLD;
			break;
		default:
			BDBG_ERR(("SCL[%d] Unknown vertical tap configuration detected: %d", hScaler->eId, ulTaps));
			BDBG_ASSERT(0);
			break;
	}

#else
	hScaler->bDeJagging = false;
	hScaler->bDeRinging = false;
	hScaler->ulHorzTaps = BVDC_P_CT_8_TAP;
	hScaler->ulVertTaps = BVDC_P_CT_4_TAP;
	hScaler->ulVertBlkAvgThreshold = BVDC_P_SCL_4TAP_BLK_AVG_VERT_THRESHOLD;

	BSTD_UNUSED(ulReg);
	BSTD_UNUSED(ulTaps);
#endif

	/* Scaler intial states */
	/* Scaler with SD line buffer */
#if (BVDC_P_SUPPORT_SCL_VER >= BVDC_P_SUPPORT_SCL_VER_4)
	hScaler->ulVertLineDepth = BCHP_GET_FIELD_DATA(ulReg, SCL_0_HW_CONFIGURATION, LINE_STORE_DEPTH);
#else
	/* scaler 0/1 HD scaler*/
	hScaler->ulVertLineDepth = (hScaler->eId <= 1)?
		BVDC_P_SCL_HD_SCL_LINE_BUFFER : BVDC_P_SCL_SD_SCL_LINE_BUFFER;
#endif

	/* default settings for up sampler and down sampler */
	hScaler->stUpSampler.bUnbiasedRound = false;
	hScaler->stUpSampler.eFilterType    = BVDC_422To444Filter_eTenTaps;
	hScaler->stUpSampler.eRingRemoval   = BVDC_RingSuppressionMode_eNormal;

	hScaler->stDnSampler.eFilterType    = BVDC_444To422Filter_eDecimate;
	hScaler->stDnSampler.eRingRemoval   = BVDC_RingSuppressionMode_eNormal;

	/* Dither */
	if(hScaler->eId == BVDC_P_ScalerId_eScl1)
	{
		ulHorzLfsrCtrlT0 = BVDC_P_DITHER_SCL1_HORZ_LFSR_CTRL_T0;
		ulHorzLfsrCtrlT1 = BVDC_P_DITHER_SCL1_HORZ_LFSR_CTRL_T1;
		ulHorzLfsrCtrlT2 = BVDC_P_DITHER_SCL1_HORZ_LFSR_CTRL_T2;
		ulVertLfsrCtrlT0 = BVDC_P_DITHER_SCL1_VERT_LFSR_CTRL_T0;
		ulVertLfsrCtrlT1 = BVDC_P_DITHER_SCL1_VERT_LFSR_CTRL_T1;
		ulVertLfsrCtrlT2 = BVDC_P_DITHER_SCL1_VERT_LFSR_CTRL_T2;
	}
	else if(hScaler->eId == BVDC_P_ScalerId_eScl2)
	{
		ulHorzLfsrCtrlT0 = BVDC_P_DITHER_SCL2_HORZ_LFSR_CTRL_T0;
		ulHorzLfsrCtrlT1 = BVDC_P_DITHER_SCL2_HORZ_LFSR_CTRL_T1;
		ulHorzLfsrCtrlT2 = BVDC_P_DITHER_SCL2_HORZ_LFSR_CTRL_T2;
		ulVertLfsrCtrlT0 = BVDC_P_DITHER_SCL2_VERT_LFSR_CTRL_T0;
		ulVertLfsrCtrlT1 = BVDC_P_DITHER_SCL2_VERT_LFSR_CTRL_T1;
		ulVertLfsrCtrlT2 = BVDC_P_DITHER_SCL2_VERT_LFSR_CTRL_T2;
	}
	else
	{
		ulHorzLfsrCtrlT0 = BVDC_P_DITHER_SCL0_HORZ_LFSR_CTRL_T0;
		ulHorzLfsrCtrlT1 = BVDC_P_DITHER_SCL0_HORZ_LFSR_CTRL_T1;
		ulHorzLfsrCtrlT2 = BVDC_P_DITHER_SCL0_HORZ_LFSR_CTRL_T2;
		ulVertLfsrCtrlT0 = BVDC_P_DITHER_SCL0_VERT_LFSR_CTRL_T0;
		ulVertLfsrCtrlT1 = BVDC_P_DITHER_SCL0_VERT_LFSR_CTRL_T1;
		ulVertLfsrCtrlT2 = BVDC_P_DITHER_SCL0_VERT_LFSR_CTRL_T2;
	}
	ulHorzLfsrValue  = BVDC_P_DITHER_SCL_LFSR_VALUE;
	ulVertLfsrValue  = BVDC_P_DITHER_SCL_LFSR_VALUE;

	BVDC_P_Dither_Init(&hScaler->stHorzDither, ulHorzLfsrCtrlT0,
		ulHorzLfsrCtrlT1, ulHorzLfsrCtrlT2, ulHorzLfsrValue);
	BVDC_P_Dither_Init(&hScaler->stVertDither, ulVertLfsrCtrlT0,
		ulVertLfsrCtrlT1, ulVertLfsrCtrlT2, ulVertLfsrValue);

#if (BVDC_P_SUPPORT_SCL_VER >= BVDC_P_SUPPORT_SCL_VER_3 && \
     BVDC_P_SUPPORT_SCL_VER <= BVDC_P_SUPPORT_SCL_VER_8)
	BVDC_P_SCL_GET_REG_DATA(SCL_0_HORIZ_DITHER_LFSR_INIT) &= ~(
		BCHP_MASK(SCL_0_HORIZ_DITHER_LFSR_INIT, SEQ) |
		BCHP_MASK(SCL_0_HORIZ_DITHER_LFSR_INIT, VALUE));
	BVDC_P_SCL_GET_REG_DATA(SCL_0_HORIZ_DITHER_LFSR_INIT) |=  (
		BCHP_FIELD_DATA(SCL_0_HORIZ_DITHER_LFSR_INIT, SEQ,   hScaler->stHorzDither.ulLfsrSeq  ) |
		BCHP_FIELD_DATA(SCL_0_HORIZ_DITHER_LFSR_INIT, VALUE, hScaler->stHorzDither.ulLfsrValue));

	BVDC_P_SCL_GET_REG_DATA(SCL_0_HORIZ_DITHER_LFSR_CTRL) &= ~(
		BCHP_MASK(SCL_0_HORIZ_DITHER_LFSR_CTRL, T0) |
		BCHP_MASK(SCL_0_HORIZ_DITHER_LFSR_CTRL, T1) |
		BCHP_MASK(SCL_0_HORIZ_DITHER_LFSR_CTRL, T2));
	BVDC_P_SCL_GET_REG_DATA(SCL_0_HORIZ_DITHER_LFSR_CTRL) |=  (
		BCHP_FIELD_DATA(SCL_0_HORIZ_DITHER_LFSR_CTRL, T0, hScaler->stHorzDither.ulLfsrCtrlT0) |
		BCHP_FIELD_DATA(SCL_0_HORIZ_DITHER_LFSR_CTRL, T1, hScaler->stHorzDither.ulLfsrCtrlT1) |
		BCHP_FIELD_DATA(SCL_0_HORIZ_DITHER_LFSR_CTRL, T2, hScaler->stHorzDither.ulLfsrCtrlT2));

	BVDC_P_SCL_GET_REG_DATA(SCL_0_HORIZ_DITHER_CTRL) &= ~(
		BCHP_MASK(SCL_0_HORIZ_DITHER_CTRL, MODE      ) |
		BCHP_MASK(SCL_0_HORIZ_DITHER_CTRL, OFFSET_CH2) |
		BCHP_MASK(SCL_0_HORIZ_DITHER_CTRL, SCALE_CH2 ) |
		BCHP_MASK(SCL_0_HORIZ_DITHER_CTRL, OFFSET_CH1) |
		BCHP_MASK(SCL_0_HORIZ_DITHER_CTRL, SCALE_CH1 ) |
		BCHP_MASK(SCL_0_HORIZ_DITHER_CTRL, OFFSET_CH0) |
		BCHP_MASK(SCL_0_HORIZ_DITHER_CTRL, SCALE_CH0 ));
	BVDC_P_SCL_GET_REG_DATA(SCL_0_HORIZ_DITHER_CTRL) |=  (
		BCHP_FIELD_DATA(SCL_0_HORIZ_DITHER_CTRL, MODE,       hScaler->stHorzDither.ulMode     ) |
		BCHP_FIELD_DATA(SCL_0_HORIZ_DITHER_CTRL, OFFSET_CH2, hScaler->stHorzDither.ulCh1Offset) |
		BCHP_FIELD_DATA(SCL_0_HORIZ_DITHER_CTRL, SCALE_CH2,  hScaler->stHorzDither.ulCh1Scale ) |
		BCHP_FIELD_DATA(SCL_0_HORIZ_DITHER_CTRL, OFFSET_CH1, hScaler->stHorzDither.ulCh1Offset) |
		BCHP_FIELD_DATA(SCL_0_HORIZ_DITHER_CTRL, SCALE_CH1,  hScaler->stHorzDither.ulCh1Scale ) |
		BCHP_FIELD_DATA(SCL_0_HORIZ_DITHER_CTRL, OFFSET_CH0, hScaler->stHorzDither.ulCh0Offset) |
		BCHP_FIELD_DATA(SCL_0_HORIZ_DITHER_CTRL, SCALE_CH0,  hScaler->stHorzDither.ulCh0Scale ));

	BVDC_P_SCL_GET_REG_DATA(SCL_0_VERT_DITHER_LFSR_INIT) &= ~(
		BCHP_MASK(SCL_0_VERT_DITHER_LFSR_INIT, SEQ) |
		BCHP_MASK(SCL_0_VERT_DITHER_LFSR_INIT, VALUE));
	BVDC_P_SCL_GET_REG_DATA(SCL_0_VERT_DITHER_LFSR_INIT) |=  (
		BCHP_FIELD_DATA(SCL_0_VERT_DITHER_LFSR_INIT, SEQ,   hScaler->stVertDither.ulLfsrSeq  ) |
		BCHP_FIELD_DATA(SCL_0_VERT_DITHER_LFSR_INIT, VALUE, hScaler->stVertDither.ulLfsrValue));

	BVDC_P_SCL_GET_REG_DATA(SCL_0_VERT_DITHER_LFSR_CTRL) &= ~(
		BCHP_MASK(SCL_0_VERT_DITHER_LFSR_CTRL, T0) |
		BCHP_MASK(SCL_0_VERT_DITHER_LFSR_CTRL, T1) |
		BCHP_MASK(SCL_0_VERT_DITHER_LFSR_CTRL, T2));
	BVDC_P_SCL_GET_REG_DATA(SCL_0_VERT_DITHER_LFSR_CTRL) |=  (
		BCHP_FIELD_DATA(SCL_0_VERT_DITHER_LFSR_CTRL, T0, hScaler->stVertDither.ulLfsrCtrlT0) |
		BCHP_FIELD_DATA(SCL_0_VERT_DITHER_LFSR_CTRL, T1, hScaler->stVertDither.ulLfsrCtrlT1) |
		BCHP_FIELD_DATA(SCL_0_VERT_DITHER_LFSR_CTRL, T2, hScaler->stVertDither.ulLfsrCtrlT2));

	BVDC_P_SCL_GET_REG_DATA(SCL_0_VERT_DITHER_CTRL) &= ~(
		BCHP_MASK(SCL_0_VERT_DITHER_CTRL, MODE      ) |
		BCHP_MASK(SCL_0_VERT_DITHER_CTRL, OFFSET_CH2) |
		BCHP_MASK(SCL_0_VERT_DITHER_CTRL, SCALE_CH2 ) |
		BCHP_MASK(SCL_0_VERT_DITHER_CTRL, OFFSET_CH1) |
		BCHP_MASK(SCL_0_VERT_DITHER_CTRL, SCALE_CH1 ) |
		BCHP_MASK(SCL_0_VERT_DITHER_CTRL, OFFSET_CH0) |
		BCHP_MASK(SCL_0_VERT_DITHER_CTRL, SCALE_CH0 ));
	BVDC_P_SCL_GET_REG_DATA(SCL_0_VERT_DITHER_CTRL) |=  (
		BCHP_FIELD_DATA(SCL_0_VERT_DITHER_CTRL, MODE,       hScaler->stVertDither.ulMode     ) |
		BCHP_FIELD_DATA(SCL_0_VERT_DITHER_CTRL, OFFSET_CH2, hScaler->stVertDither.ulCh1Offset) |
		BCHP_FIELD_DATA(SCL_0_VERT_DITHER_CTRL, SCALE_CH2,  hScaler->stVertDither.ulCh1Scale ) |
		BCHP_FIELD_DATA(SCL_0_VERT_DITHER_CTRL, OFFSET_CH1, hScaler->stVertDither.ulCh1Offset) |
		BCHP_FIELD_DATA(SCL_0_VERT_DITHER_CTRL, SCALE_CH1,  hScaler->stVertDither.ulCh1Scale ) |
		BCHP_FIELD_DATA(SCL_0_VERT_DITHER_CTRL, OFFSET_CH0, hScaler->stVertDither.ulCh0Offset) |
		BCHP_FIELD_DATA(SCL_0_VERT_DITHER_CTRL, SCALE_CH0,  hScaler->stVertDither.ulCh0Scale ));
#endif

#if (BVDC_P_SUPPORT_BVN_444 != 0)
/* only TV chips use 444, settop box chips always use 422;
 * and future TV chips use 444 for SCL1/2/3 too */
#if (BVDC_P_SUPPORT_SCL_VER == BVDC_P_SUPPORT_SCL_VER_5)
	if (BVDC_P_ScalerId_eScl0 < hScaler->eId)
		hScaler->ulSrcHrzAlign  = 2;  /* even for 4:2:2 */
	else
		hScaler->ulSrcHrzAlign  = 1;
#else
	hScaler->ulSrcHrzAlign  = 1;
#endif
#else
	hScaler->ulSrcHrzAlign  = 2;
#endif

	BDBG_LEAVE(BVDC_P_Scaler_Init);
	return;
}

/***************************************************************************
 * {private}
 *
 * BVDC_P_Scaler_BuildRul_DrainVnet_isr
 *
 * called by BVDC_P_Scaler_BuildRul_isr after resetting to drain the module and
 * its pre-patch FreeCh or LpBack
 */
static void BVDC_P_Scaler_BuildRul_DrainVnet_isr
	( BVDC_P_Scaler_Handle           hScaler,
	  BVDC_P_ListInfo               *pList )
{
	BDBG_OBJECT_ASSERT(hScaler, BVDC_SCL);
	/* reset sub and connect the module to a drain */
	BVDC_P_SubRul_StartDrain_isr(&(hScaler->SubRul), pList,
	    hScaler->ulResetRegAddr, hScaler->ulResetMask);

	/* disconnect the module from the drain; reset it again; drain the
	 * free-channel or loop-back used to to patch the vnet; and also
	 * reset the channel ping-pong buf of the module and the patcher. */
	BVDC_P_SubRul_FinishDrain_isr(&(hScaler->SubRul), pList,
		hScaler->ulResetRegAddr, hScaler->ulResetMask,
		hScaler->ulVnetResetAddr, hScaler->ulVnetResetMask);
}

/***************************************************************************
 * {private}
 *
 */
void BVDC_P_Scaler_BuildRul_isr
	( const BVDC_P_Scaler_Handle       hScaler,
	  BVDC_P_ListInfo                 *pList,
	  BVDC_P_State                     eVnetState,
	  BVDC_P_PicComRulInfo            *pPicComRulInfo )
{
	BVDC_P_Window_DirtyBits *pCurDirty;
	uint32_t  ulRulOpsFlags;

	BDBG_ENTER(BVDC_P_Scaler_BuildRul_isr);
	BDBG_OBJECT_ASSERT(hScaler, BVDC_SCL);
	BDBG_OBJECT_ASSERT(hScaler->hWindow, BVDC_WIN);
	pCurDirty = &hScaler->hWindow->stCurInfo.stDirty;

	/* currently this is only for vnet building / tearing-off*/

	ulRulOpsFlags = BVDC_P_SubRul_GetOps_isr(
		&(hScaler->SubRul), pPicComRulInfo->eWin, eVnetState, pList->bLastExecuted);

	if ((0 == ulRulOpsFlags) ||
		(ulRulOpsFlags & BVDC_P_RulOp_eReleaseHandle))
		return;
	else if (ulRulOpsFlags & BVDC_P_RulOp_eDisable)
	{
		BVDC_P_SubRul_DropOffVnet_isr(&(hScaler->SubRul), pList);
		BVDC_P_Scaler_SetEnable_isr(hScaler, false);
#if (BVDC_P_SUPPORT_SCL_VER > BVDC_P_SUPPORT_SCL_VER_1)
		BVDC_P_SCL_WRITE_TO_RUL(SCL_0_ENABLE, pList->pulCurrent);
#endif
	}

	/* If rul failed to execute last time we'd re reprogrammed possible
	 * missing registers. */
	if((!pList->bLastExecuted)|| (hScaler->bInitial))
	{
		hScaler->ulUpdateAll = BVDC_P_RUL_UPDATE_THRESHOLD;
	}

	/* Kludge: Currently bvdc_hddvi_priv.c will reset the scaler when it
	 * bring up which will put the scaler and the tab in a reset state.  Hence
	 * anytime when we detect scaler reset (by setting hScaler->ulUpdateAll)
	 * will make the bTabAdjust dirty as well to bring it out of the reset. */
	if((hScaler->ulUpdateAll) &&
	   (BVDC_P_ScalerId_eScl0 == hScaler->eId))
	{
		pCurDirty->stBits.bTabAdjust = BVDC_P_DIRTY;
	}

	/* TAB is part of SCL0 & V0 */
	if(pCurDirty->stBits.bTabAdjust)
	{
		if(BVDC_P_ScalerId_eScl0 == hScaler->eId)
		{
			BVDC_P_Tab_BuildRul_isr(hScaler->hWindow, pList);
		}
		else
		{
			pCurDirty->stBits.bTabAdjust = BVDC_P_CLEAN;
		}
	}

	/* reset */
	if(hScaler->bInitial)
	{
		/* BVDC_P_BUILD_RESET(pList->pulCurrent,
			hScaler->ulResetRegAddr, hScaler->ulResetMask); */
		hScaler->bInitial = false;
	}

	if (ulRulOpsFlags & BVDC_P_RulOp_eEnable)
	{
		/* Add scaler registers to RUL using block write */
		if(hScaler->ulUpdateAll)
		{
			hScaler->ulUpdateAll--;
			hScaler->ulUpdateCoeff = 0; /* no need to update coeff alone */
			/* optimize scaler mute RUL */
#if (BVDC_P_SUPPORT_SCL_VER <= BVDC_P_SUPPORT_SCL_VER_6)
			BVDC_P_SCL_BLOCK_WRITE_TO_RUL(SCL_0_BVB_IN_SIZE, SCL_0_HORIZ_DEST_PIC_REGION_2_END, pList->pulCurrent);
#else
			BVDC_P_SCL_BLOCK_WRITE_TO_RUL(SCL_0_BVB_IN_SIZE, SCL_0_HORIZ_DEST_PIC_REGION_3_END, pList->pulCurrent);
#endif

#if (BVDC_P_SUPPORT_SCL_VER <= BVDC_P_SUPPORT_SCL_VER_3)
			BVDC_P_SCL_BLOCK_WRITE_TO_RUL(SCL_0_VERT_FIR_COEFF_PHASE0_00_01, SCL_0_HORIZ_FIR_CHROMA_COEFF_PHASE7_06_07, pList->pulCurrent);
#elif (BVDC_P_SUPPORT_SCL_VER == BVDC_P_SUPPORT_SCL_VER_4)
			BVDC_P_SCL_BLOCK_WRITE_TO_RUL(SCL_0_VERT_FIR_LUMA_COEFF_PHASE0_00_01,  SCL_0_VERT_FIR_CHROMA_COEFF_PHASE7_00_01, pList->pulCurrent);
			BVDC_P_SCL_BLOCK_WRITE_TO_RUL(SCL_0_HORIZ_FIR_LUMA_COEFF_PHASE0_00_01, SCL_0_HORIZ_FIR_CHROMA_COEFF_PHASE7_04_05, pList->pulCurrent);
#elif (BVDC_P_SUPPORT_SCL_VER == BVDC_P_SUPPORT_SCL_VER_5)
			BVDC_P_SCL_BLOCK_WRITE_TO_RUL(SCL_0_VERT_FIR_LUMA_COEFF_PHASE0_00_01,  SCL_0_VERT_FIR_CHROMA_COEFF_PHASE7_02, pList->pulCurrent);
			BVDC_P_SCL_BLOCK_WRITE_TO_RUL(SCL_0_HORIZ_FIR_LUMA_COEFF_PHASE0_00_01, SCL_0_HORIZ_FIR_CHROMA_COEFF_PHASE7_04_05, pList->pulCurrent);
#elif (BVDC_P_SUPPORT_SCL_VER == BVDC_P_SUPPORT_SCL_VER_6)
			BVDC_P_SCL_BLOCK_WRITE_TO_RUL(SCL_0_VERT_FIR_LUMA_COEFF_PHASE0_00_01,  SCL_0_VERT_FIR_CHROMA_COEFF_PHASE7_00_01, pList->pulCurrent);
			BVDC_P_SCL_BLOCK_WRITE_TO_RUL(SCL_0_HORIZ_FIR_LUMA_COEFF_PHASE0_00_01, SCL_0_HORIZ_FIR_CHROMA_COEFF_PHASE7_02_03, pList->pulCurrent);
#elif (BVDC_P_SUPPORT_SCL_VER >= BVDC_P_SUPPORT_SCL_VER_7)
			BVDC_P_SCL_BLOCK_WRITE_TO_RUL(SCL_0_VERT_FIR_LUMA_COEFF_PHASE0_00_01,  SCL_0_VERT_FIR_CHROMA_COEFF_PHASE7_00_01, pList->pulCurrent);
			BVDC_P_SCL_BLOCK_WRITE_TO_RUL(SCL_0_HORIZ_FIR_LUMA_COEFF_PHASE0_00_01, SCL_0_HORIZ_FIR_CHROMA_COEFF_PHASE7_04_05, pList->pulCurrent);
			BVDC_P_SCL_WRITE_TO_RUL(SCL_0_VIDEO_3D_MODE, pList->pulCurrent);
#endif

#if (BVDC_P_SUPPORT_SCL_VER >= BVDC_P_SUPPORT_SCL_VER_3)
#if (BVDC_P_SUPPORT_SCL_VER <= BVDC_P_SUPPORT_SCL_VER_8)
			BVDC_P_SCL_BLOCK_WRITE_TO_RUL(SCL_0_HORIZ_DITHER_LFSR_INIT, SCL_0_VERT_DITHER_CTRL,
				pList->pulCurrent);
#endif
			if(hScaler->bDeRinging)
			{
				BVDC_P_SCL_BLOCK_WRITE_TO_RUL(SCL_0_DERINGING, SCL_0_DERING_DEMO_SETTING,
					pList->pulCurrent);
			}
#endif

#if (BVDC_P_SUPPORT_SCL_VER >= BVDC_P_SUPPORT_SCL_VER_4)
			if(hScaler->bDeJagging)
			{
				BVDC_P_SCL_BLOCK_WRITE_TO_RUL(SCL_0_DEJAGGING, SCL_0_DEJAGGING_DEMO_SETTING,
					pList->pulCurrent);
			}
#endif
#if (BVDC_P_SUPPORT_SCL_VER == BVDC_P_SUPPORT_SCL_VER_5)
			BVDC_P_SCL_WRITE_TO_RUL(SCL_0_DS_CONFIGURATION, pList->pulCurrent);
			BVDC_P_SCL_WRITE_TO_RUL(SCL_0_US_422_TO_444_CONV, pList->pulCurrent);
#endif
			BVDC_P_SCL_BLOCK_WRITE_TO_RUL(SCL_0_TOP_CONTROL, SCL_0_HORIZ_CONTROL, pList->pulCurrent);
		}
		else
		{
			/* update coeff alone, likely due a tmp field inversion */
			if (hScaler->ulUpdateCoeff)
			{
				hScaler->ulUpdateCoeff--;
#if (BVDC_P_SUPPORT_SCL_VER <= BVDC_P_SUPPORT_SCL_VER_3)
				BVDC_P_SCL_BLOCK_WRITE_TO_RUL(SCL_0_VERT_FIR_COEFF_PHASE0_00_01, SCL_0_VERT_FIR_CHROMA_COEFF_PHASE7_02_03, pList->pulCurrent);
#elif (BVDC_P_SUPPORT_SCL_VER == BVDC_P_SUPPORT_SCL_VER_4 || BVDC_P_SUPPORT_SCL_VER >= BVDC_P_SUPPORT_SCL_VER_7)
				BVDC_P_SCL_BLOCK_WRITE_TO_RUL(SCL_0_VERT_FIR_LUMA_COEFF_PHASE0_00_01, SCL_0_VERT_FIR_CHROMA_COEFF_PHASE7_00_01, pList->pulCurrent);
#elif (BVDC_P_SUPPORT_SCL_VER == BVDC_P_SUPPORT_SCL_VER_5)
				BVDC_P_SCL_BLOCK_WRITE_TO_RUL(SCL_0_VERT_FIR_LUMA_COEFF_PHASE0_00_01, SCL_0_VERT_FIR_CHROMA_COEFF_PHASE7_02, pList->pulCurrent);
#endif
			}

			/* Update these register on every vsync. */
#if (BVDC_P_SUPPORT_SCL_VER <= BVDC_P_SUPPORT_SCL_VER_6)
			BVDC_P_SCL_WRITE_TO_RUL(SCL_0_VERT_FIR_SRC_PIC_OFFSET, pList->pulCurrent);
#else
			BVDC_P_SCL_BLOCK_WRITE_TO_RUL(SCL_0_VERT_FIR_SRC_PIC_OFFSET_INT, SCL_0_VERT_FIR_INIT_PIC_STEP, pList->pulCurrent);
#endif
			BVDC_P_SCL_WRITE_TO_RUL(SCL_0_TOP_CONTROL, pList->pulCurrent);
		}

#if (BVDC_P_SUPPORT_SCL_VER > BVDC_P_SUPPORT_SCL_VER_1)
		BVDC_P_SCL_WRITE_TO_RUL(SCL_0_ENABLE, pList->pulCurrent);
#endif

		/* join in vnet after enable. note: its src mux is initialed as disabled */
		if (ulRulOpsFlags & BVDC_P_RulOp_eVnetInit)
		{
			BVDC_P_SubRul_JoinInVnet_isr(&(hScaler->SubRul), pList);
		}
	}

	else if (ulRulOpsFlags & BVDC_P_RulOp_eDrainVnet)
	{
		BVDC_P_Scaler_BuildRul_DrainVnet_isr(hScaler, pList);
	}

	BDBG_LEAVE(BVDC_P_Scaler_BuildRul_isr);
	return;
}


/***************************************************************************
 * {private}
 *
 */
static void BVDC_P_Scaler_SetHorizFirCoeff
	( BVDC_P_Scaler_Handle             hScaler,
	  const uint32_t                  *pulHorzFirCoeff )
{
	int i;
#if ((BVDC_P_SUPPORT_SCL_VER_4==BVDC_P_SUPPORT_SCL_VER) || \
     (BVDC_P_SUPPORT_SCL_VER_7==BVDC_P_SUPPORT_SCL_VER) || \
     (BVDC_P_SUPPORT_SCL_VER_8==BVDC_P_SUPPORT_SCL_VER))
	int j = 0;
#endif

	BDBG_OBJECT_ASSERT(hScaler, BVDC_SCL);
	/* write 32 hor entries into registers */
	for(i = 0; (pulHorzFirCoeff) && (*pulHorzFirCoeff != BVDC_P_SCL_LAST); i++)
	{
#if (BVDC_P_SUPPORT_SCL_VER <= BVDC_P_SUPPORT_SCL_VER_3)
		hScaler->aulRegs[BVDC_P_SCL_GET_REG_IDX(SCL_0_HORIZ_FIR_COEFF_PHASE0_00_01) + i] =
			*pulHorzFirCoeff;
#else
		hScaler->aulRegs[BVDC_P_SCL_GET_REG_IDX(SCL_0_HORIZ_FIR_LUMA_COEFF_PHASE0_00_01) + i] =
			*pulHorzFirCoeff;
#endif
		pulHorzFirCoeff++;

#if ((BVDC_P_SUPPORT_SCL_VER_4==BVDC_P_SUPPORT_SCL_VER) || \
     (BVDC_P_SUPPORT_SCL_VER_7==BVDC_P_SUPPORT_SCL_VER) || \
     (BVDC_P_SUPPORT_SCL_VER_8==BVDC_P_SUPPORT_SCL_VER))
		j++;
		if (j == 2 && hScaler->ulHorzTaps == BVDC_P_CT_8_TAP)
		{
			i++;   /* padding 0x00000000 to table */
			j = 0;
		}
#endif
	}
}


/***************************************************************************
 * {private}
 *
 */
static void BVDC_P_Scaler_SetChromaHorizFirCoeff
	( BVDC_P_Scaler_Handle             hScaler,
	  const uint32_t                  *pulHorzFirCoeff )
{
	int i;
#if ((BVDC_P_SUPPORT_SCL_VER_4==BVDC_P_SUPPORT_SCL_VER) || \
     (BVDC_P_SUPPORT_SCL_VER_7==BVDC_P_SUPPORT_SCL_VER) || \
     (BVDC_P_SUPPORT_SCL_VER_8==BVDC_P_SUPPORT_SCL_VER))
	int j = 0;
#endif

	BDBG_OBJECT_ASSERT(hScaler, BVDC_SCL);
	/* write 32 hor entries into registers */
	for(i = 0; (pulHorzFirCoeff) && (*pulHorzFirCoeff != BVDC_P_SCL_LAST); i++)
	{
#if (BVDC_P_SUPPORT_SCL_VER <= BVDC_P_SUPPORT_SCL_VER_3)
		hScaler->aulRegs[BVDC_P_SCL_GET_REG_IDX(SCL_0_HORIZ_FIR_CHROMA_COEFF_PHASE0_00_01) + i] =
			*pulHorzFirCoeff;
#else
		hScaler->aulRegs[BVDC_P_SCL_GET_REG_IDX(SCL_0_HORIZ_FIR_CHROMA_COEFF_PHASE0_00_01) + i] =
			*pulHorzFirCoeff;
#endif
		pulHorzFirCoeff++;

#if ((BVDC_P_SUPPORT_SCL_VER_4==BVDC_P_SUPPORT_SCL_VER) || \
     (BVDC_P_SUPPORT_SCL_VER_7==BVDC_P_SUPPORT_SCL_VER) || \
     (BVDC_P_SUPPORT_SCL_VER_8==BVDC_P_SUPPORT_SCL_VER))
		j++;
		if (j == 2 && hScaler->ulHorzTaps == BVDC_P_CT_8_TAP)
		{
			i++;   /* padding 0x00000000 to table */
			j = 0;
		}
#endif
	}
}

/***************************************************************************
 * {private}
 *
 */
static void BVDC_P_Scaler_SetVertFirCoeff
	( BVDC_P_Scaler_Handle             hScaler,
	  const uint32_t                  *pulVertFirCoeff )
{
	int i;

	BDBG_OBJECT_ASSERT(hScaler, BVDC_SCL);
	/* write ver entries into registers */
	for(i = 0; (pulVertFirCoeff) && (*pulVertFirCoeff != BVDC_P_SCL_LAST); i++)
	{
#if (BVDC_P_SUPPORT_SCL_VER <= BVDC_P_SUPPORT_SCL_VER_3)
		hScaler->aulRegs[BVDC_P_SCL_GET_REG_IDX(SCL_0_VERT_FIR_COEFF_PHASE0_00_01) + i] =
			*pulVertFirCoeff;
#else
		hScaler->aulRegs[BVDC_P_SCL_GET_REG_IDX(SCL_0_VERT_FIR_LUMA_COEFF_PHASE0_00_01) + i] =
			*pulVertFirCoeff;
#endif
		pulVertFirCoeff++;
	}
}


/***************************************************************************
 * {private}
 *
 */
static void BVDC_P_Scaler_SetChromaVertFirCoeff
	( BVDC_P_Scaler_Handle             hScaler,
	  const uint32_t                  *pulVertFirCoeff )
{
	int i;

	BDBG_OBJECT_ASSERT(hScaler, BVDC_SCL);
	/* write ver entries into registers */
	for(i = 0; (pulVertFirCoeff) && (*pulVertFirCoeff != BVDC_P_SCL_LAST); i++)
	{
#if (BVDC_P_SUPPORT_SCL_VER <= BVDC_P_SUPPORT_SCL_VER_3)
		hScaler->aulRegs[BVDC_P_SCL_GET_REG_IDX(SCL_0_VERT_FIR_CHROMA_COEFF_PHASE0_00_01) + i] =
			*pulVertFirCoeff;
#else
		hScaler->aulRegs[BVDC_P_SCL_GET_REG_IDX(SCL_0_VERT_FIR_CHROMA_COEFF_PHASE0_00_01) + i] =
			*pulVertFirCoeff;
#endif
		pulVertFirCoeff++;
	}
}


/***************************************************************************
 * {private}
 * The effect of (bPrgrssvIntrlcChange==true) is that point sampling coeff
 * will not be choosen.
 */
const BVDC_P_FirCoeffTbl* BVDC_P_SelectFirCoeff_isr
	( const BVDC_P_FirCoeffTbl         *pFirstCoeffEntry,
	  uint32_t                          ulCtIndex,
	  BVDC_P_CtInput                    eCtInputType,
	  BAVC_Polarity                     eSrcOrigPolarity,
	  uint32_t                          ulDownRatio,
	  uint32_t                          ulTapMode,
	  uint32_t                          ulSrcSize,
	  uint32_t                          ulDstSize )
{
	const BVDC_P_FirCoeffTbl *pCoeffs = NULL; /* Return this */
	const BVDC_P_FirCoeffTbl *pCurTable = pFirstCoeffEntry;
	BVDC_P_CtRaster eCtRaster = (BAVC_Polarity_eFrame == eSrcOrigPolarity)
		? BVDC_P_CtRaster_ePro : BVDC_P_CtRaster_eInt;

	/* Iterate thru to find first matching table */
	while((BVDC_P_CtLutOp_eLast != pCurTable->eCtLutOp) && (!pCoeffs))
	{
		if (pCurTable->eCtLutOp == BVDC_P_CtLutOp_eAlways)
		{
			pCoeffs = pCurTable;
		}
		else if((pCurTable->eCtLutOp == BVDC_P_CtLutOp_eUserSelectable) &&
		        (pCurTable->ulTapMode == ulTapMode) &&
		        (pCurTable->ulCtIndex == ulCtIndex))
		{
			pCoeffs = pCurTable;
		}
		else if ((pCurTable->ulCtIndex == ulCtIndex) &&
			     (pCurTable->ulTapMode == ulTapMode) &&
			     ((pCurTable->eCtInputType == eCtInputType) ||             /* Matched input type or any. */
			      (pCurTable->eCtInputType == BVDC_P_CtInput_eAny)) &&
			     ((pCurTable->eCtRaster == eCtRaster) ||                   /* Matched raster type or any. */
			      (pCurTable->eCtRaster == BVDC_P_CtRaster_eAny)))
		{
			switch(pCurTable->eCtLutOp)
			{
			case BVDC_P_CtLutOp_eGreaterThan:
				if(ulDownRatio > pCurTable->ulDownRatio)
				{
					pCoeffs = pCurTable;
				}
				break;

			case BVDC_P_CtLutOp_eGreaterThanEqual:
				if(ulDownRatio >= pCurTable->ulDownRatio)
				{
					pCoeffs = pCurTable;
				}
				break;

			case BVDC_P_CtLutOp_eLessThan:
				if(ulDownRatio < pCurTable->ulDownRatio)
				{
					pCoeffs = pCurTable;
				}
				break;

			case BVDC_P_CtLutOp_eEqual:
				/* If using size match, not ratio */
				if(pCurTable->ulDownRatio == BVDC_P_CT_UNUSED)
				{
					if((ulSrcSize <= pCurTable->ulSrcSize + pCurTable->ulDelta) &&
						(ulSrcSize >= pCurTable->ulSrcSize - pCurTable->ulDelta) &&
						(ulDstSize <= pCurTable->ulDstSize + pCurTable->ulDelta) &&
						(ulDstSize >= pCurTable->ulDstSize - pCurTable->ulDelta))
					{
						pCoeffs = pCurTable;
					}
				}
				else
				{
					if(ulDownRatio == pCurTable->ulDownRatio)
					{
						pCoeffs = pCurTable;
					}
				}
				break;

			case BVDC_P_CtLutOp_eLessThanEqual:
				if(ulDownRatio <= pCurTable->ulDownRatio)
				{
					pCoeffs = pCurTable;
				}
				break;

			default:
				pCoeffs = NULL;
				break;
			}
		}
		++pCurTable;
	}

	BDBG_ASSERT(pCoeffs);

	return pCoeffs;
}


/***************************************************************************
 * {private}
 *
 * The following notes illustrate the capabilities of the scaler.  It
 * mainly shows different modes scaler operates to achieve the desire
 * scaling ratio with the best quality.
 *
 * Sx = in/out.  For example src=1920 dst=720 then Sx = 1920/720 or 2.66.
 * Hence this is what Sx means:
 *   Sx >  1 Scale down
 *   Sx <  1 Scale up
 *   Sx == 1 Unity.
 *
 * Actuall Sx tells how much we're scaling down.  For example
 * Sx = 4 means we're scaling down 4 times.
 *
 * [[ Horizontal Scaling ]]
 *
 * {BVB_in} --> [ HWF_0 ] --> [ HWF_1 ] --> [ FIR ] --> {BVB_out}
 *
 *  in:out        HWF_0         HWF_1         FIR
 *
 * Sx <= 4         1:1           1:1          Sx' = Sx / (1 * 1)
 * Sx >  4         2:1           1:1          Sx' = Sx / (2 * 1)
 * Sx >  8         2:1           2:1          Sx' = Sx / (2 * 2)
 *
 * Sx' is the new horizontal ratio, and this is the value that goes into
 * horizontal FIR ratio register.
 *
 * Sx' must be [32.0, 0.125].
 *
 *
 * [[ Vertical Scaling ]]
 *
 * {BVB_in} --> [ BLK_AVG ]      -->        [ FIR ] --> {BVB_out}
 *
 *  in:out        BLK_AVG                     FIR
 *
 * L >= 1280 (2-tap)
 *  Sy <= 2       1:1                         Sy' = Sy
 *  Sy >  2       (n+1):1                     Sy' = Sy / (n+1)
 *
 * L < 1280  (4-tap)
 *  Sy <= 4       1:1                         Sy' = Sy
 *  Sy >  4   (n+1):1                         Sy' = Sy / (n+1)
 *
 * Sy' is the new vertical ratio, and this is the value that goes into
 *  vertical FIR ratio register.
 *
 * Sy' must be [2, 0.013125] (for 2-tap)
 * Sy' must be [4, 0.013125] (for 4-tap)
 *
 * L is input lenght to vertical scaler in pixels.  Notes that if the
 *  horzontal scaler is before vertical scaler L is equal to the output of
 *  horzontal.  (if horizontal is downscale it will be first).
 *
 * n is the BAVG_BLK_SIZE.  n must be [0, 15].
 *
 *
 * [[ Conclusion ]]
 *  With the above information the theoretical scaling capacities are:
 *
 *  Sx = 32:1 to 1:32
 *
 *  Sy = 32:1 to 1:32 (for 2-tap)
 *     = 64:1 to 1:32 (for 4-tap)
 */
static BERR_Code BVDC_P_Scaler_CalVertInitPhase_isr
	( BVDC_P_Scaler_Handle             hScaler,
	  const BAVC_Polarity              eSrcPolarity,
	  const BAVC_Polarity              eDstPolarity,
	  uint32_t                         ulVrtStep,
 	  uint32_t                         ulPanScanTop,
 	  uint32_t                        *pulLargestBlkAvgSize )
{
	uint32_t ulVertInitPhase; /* FIXED: S6.6 */
	uint32_t ulFirVrtStep;
	uint32_t ulANom      = (eDstPolarity == BAVC_Polarity_eBotField) ? 3 : 1;
	uint32_t ulADenom    = (eDstPolarity == BAVC_Polarity_eFrame) ? 2 : 4;
	uint32_t ulBNom      = (eSrcPolarity == BAVC_Polarity_eBotField) ? 3 : 1;
	uint32_t ulBDenom    = (eSrcPolarity == BAVC_Polarity_eFrame) ? 2 : 4;
	bool     bPhaseShift = ((hScaler->bHandleFldInv) &&
	                        (eSrcPolarity != BAVC_Polarity_eFrame) &&
	                        (eDstPolarity != BAVC_Polarity_eFrame));
	uint32_t ulVsr       = BVDC_P_SCL_V_RATIO_TRUNC(ulVrtStep);

	ulVsr = BVDC_P_V_INIT_PHASE_RATIO_ADJ(ulVsr);

	/* Compute Vertical FIR initial phase.
	 * Do we want scaler to invert the field?  To invert field we will
	 * need to change the vertical initial phase a according to the given
	 * formula for doing TF2BF, and BF2TF.
	 * NOTE:
	 *   - this formula is based on field middle line alignment; */
	hScaler->aaulInitPhase[eSrcPolarity][eDstPolarity] =
		BVDC_P_FIXED_A_MINUS_FIXED_B(ulVsr * ulANom / ulADenom, BVDC_P_V_INIT_PHASE_1_POINT_0 * ulBNom / ulBDenom) + ulPanScanTop;
	if(bPhaseShift && (0 == (ulVsr & (BVDC_P_SCL_V_RATIO_ONE - 1))))
		hScaler->aaulInitPhase[eSrcPolarity][eDstPolarity] -= BVDC_P_V_INIT_PHASE_0_POINT_25;
	ulVertInitPhase = hScaler->hWindow->stCurInfo.stSclSettings.bSclVertPhaseIgnore? 0 :
		hScaler->aaulInitPhase[eSrcPolarity][eDstPolarity];
	ulFirVrtStep = ulVrtStep;
	hScaler->aulBlkAvgSize[eSrcPolarity][eDstPolarity] = 0;
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)
       while((hScaler->aulBlkAvgSize[eSrcPolarity][eDstPolarity] <= BVDC_P_SCL_MAX_BLK_AVG) &&
                       (ulFirVrtStep > ((hScaler->ulVertBlkAvgThreshold / 2) + ulVertInitPhase + (1 << BVDC_P_SCL_V_RATIO_F_BITS)))
#else
	while((hScaler->aulBlkAvgSize[eSrcPolarity][eDstPolarity] <= BVDC_P_SCL_MAX_BLK_AVG) &&
		  (ulFirVrtStep > ((hScaler->ulVertBlkAvgThreshold / 2) + ulVertInitPhase + 1))
#endif
		 )
	{
		hScaler->aulBlkAvgSize[eSrcPolarity][eDstPolarity] += 1;
		ulFirVrtStep = (ulVrtStep / (hScaler->aulBlkAvgSize[eSrcPolarity][eDstPolarity] + 1));
		ulFirVrtStep = BVDC_P_SCL_V_RATIO_TRUNC(ulFirVrtStep);
		ulFirVrtStep = BVDC_P_V_INIT_PHASE_RATIO_ADJ(ulFirVrtStep);

		hScaler->aaulInitPhase[eSrcPolarity][eDstPolarity] =
			BVDC_P_FIXED_A_MINUS_FIXED_B(ulFirVrtStep* ulANom / ulADenom, BVDC_P_V_INIT_PHASE_1_POINT_0 * ulBNom / ulBDenom) +
			(ulPanScanTop / (hScaler->aulBlkAvgSize[eSrcPolarity][eDstPolarity] + 1));
		if(bPhaseShift && (0 == (ulFirVrtStep & (BVDC_P_SCL_V_RATIO_ONE - 1))))
			hScaler->aaulInitPhase[eSrcPolarity][eDstPolarity] -= BVDC_P_V_INIT_PHASE_0_POINT_25;
		ulVertInitPhase = hScaler->hWindow->stCurInfo.stSclSettings.bSclVertPhaseIgnore? 0 :
			hScaler->aaulInitPhase[eSrcPolarity][eDstPolarity];
	}

	if(*pulLargestBlkAvgSize < hScaler->aulBlkAvgSize[eSrcPolarity][eDstPolarity])
	{
		*pulLargestBlkAvgSize = hScaler->aulBlkAvgSize[eSrcPolarity][eDstPolarity];
	}

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)
       if(hScaler->aulBlkAvgSize[eSrcPolarity][eDstPolarity] > BVDC_P_SCL_MAX_BLK_AVG &&
                       (ulFirVrtStep > ((hScaler->ulVertBlkAvgThreshold / 2) + ulVertInitPhase + (1 << BVDC_P_SCL_V_RATIO_F_BITS))))
#else
	if(hScaler->aulBlkAvgSize[eSrcPolarity][eDstPolarity] > BVDC_P_SCL_MAX_BLK_AVG &&
	   (ulFirVrtStep > ((hScaler->ulVertBlkAvgThreshold / 2) + ulVertInitPhase + 1)))
#endif
	{
		return BERR_UNKNOWN;
	}

	return BERR_SUCCESS;
}


void BVDC_P_Scaler_SetInfo_isr
	( BVDC_P_Scaler_Handle             hScaler,
	  const BVDC_P_PictureNodePtr      pPicture )
{
	uint32_t ulSrcHSize;               /* really scaled src width in pixel unit */
	uint32_t ulSrcVSize;               /* really scaled src height, in row unit */
	uint32_t ulDstHSize;               /* Dst width in pixel unit */
	uint32_t ulDstVSize;               /* Dst height, in row unit */
	uint32_t ulAlgnSrcHSize;           /* src width into the 1st one of half band or FIR, pixel unit */
	uint32_t ulAlgnSrcVSize;           /* src height into the 1st one of block avrg or FIR, row unit */
	uint32_t ulBvbInHSize;             /* input bvb width in pixel unit */
	uint32_t ulBvbInVSize;             /* input bvb height, in row unit */
	uint32_t ulPicOffsetLeft;          /* horizontal Pan/Scan part cut by PIC_OFFSET, pixel unit */
#if (BVDC_P_SUPPORT_SCL_VER >= BVDC_P_SUPPORT_SCL_VER_8)
	uint32_t ulPicOffsetLeft_R;         /* horizontal Pan/Scan part cut by PIC_OFFSET_R, pixel unit */
#endif
	uint32_t ulPicOffsetTop;           /* vertical Pan/Scan part cut by PIC_OFFSET, row unit */
	uint32_t ulPanScanLeft;            /* horizontal Pan/Scan vector in S11.6 format */
	uint32_t ulPanScanTop;             /* Vertical Pan/Scan vector in S11.14 format */
	uint32_t ulFirSrcHSize;            /* FIR Horizontal input size for size matching to select coeff */
	uint32_t ulFirDstHSize;            /* FIR Horizontal output size for size matching to select coeff */
	uint32_t ulFirSrcVSize;            /* FIR Vertical input size for size matching to select coeff */
	uint32_t ulFirDstVSize;            /* FIR Vertical output size for size matching to select coeff */
	const BVDC_P_FirCoeffTbl *pHorzFirCoeff;
	uint32_t ulNrmHrzStep;              /* Total horizontal src step per dest pixel, U12.20 */
	uint32_t ulNrmVrtSrcStep;
	uint32_t ulHrzStep = 0;             /* Total horizontal src step per dest pixel, HW reg format */
	uint32_t ulVrtStep = 0;             /* Total vertical src step per dest pixel, HW reg format */
	uint32_t ulFirHrzStep = 0;          /* FIR hrz src step per dest pixel, HW reg fmt, for coeff select */
	uint32_t ulFirVrtStep = 0;          /* FIR vertical src step per dest pixel */
	uint32_t ulFirHrzStepInit = 0;      /* FIR hrz src step per dest pixel, HW reg fmt, for init phase */
	uint32_t ulVertSclSrcWidth = 0;     /* Adjusted src width after horz scaler, in pixel unit */
	uint32_t ulVertInitPhase       = 0; /* FIXED: S6.6 */
	uint32_t ulBlkAvgSize = 0;
	int32_t  lHrzPhsAccInit = 0;
	uint32_t ulMaxX, ulMaxY;
	uint32_t ulVsr = 0;
	BVDC_P_Rect  *pSclIn, *pSclOut, *pSclCut;

	BDBG_ENTER(BVDC_P_Scaler_SetInfo_isr);
	BDBG_OBJECT_ASSERT(hScaler, BVDC_SCL);
	BDBG_OBJECT_ASSERT(hScaler->hWindow, BVDC_WIN);
	BDBG_OBJECT_ASSERT(hScaler->hWindow->stCurInfo.hSource, BVDC_SRC);
	pSclIn = pPicture->pSclIn;
	pSclCut = &pPicture->stSclCut;
	pSclOut = pPicture->pSclOut;

	/* any following info changed -> re-calculate SCL settings */
	/* TODO: This need optimization */
	if(!BVDC_P_RECT_CMP_EQ(pSclOut, &hScaler->stPrevSclOut)   ||
	   !BVDC_P_RECT_CMP_EQ(pSclCut, &hScaler->stPrevSclCut)   ||
	   !BVDC_P_RECT_CMP_EQ(pSclIn,  &hScaler->stPrevSclIn)    ||
	   (hScaler->hWindow->stCurInfo.stSclSettings.bSclVertDejagging       != hScaler->stSclSettings.bSclVertDejagging      ) ||
	   (hScaler->hWindow->stCurInfo.stSclSettings.bSclHorzLumaDeringing   != hScaler->stSclSettings.bSclHorzLumaDeringing  ) ||
	   (hScaler->hWindow->stCurInfo.stSclSettings.bSclVertLumaDeringing   != hScaler->stSclSettings.bSclVertLumaDeringing  ) ||
	   (hScaler->hWindow->stCurInfo.stSclSettings.bSclHorzChromaDeringing != hScaler->stSclSettings.bSclHorzChromaDeringing) ||
	   (hScaler->hWindow->stCurInfo.stSclSettings.bSclVertChromaDeringing != hScaler->stSclSettings.bSclVertChromaDeringing) ||
	   (hScaler->hWindow->stCurInfo.stSclSettings.ulSclDejaggingCore      != hScaler->stSclSettings.ulSclDejaggingCore     ) ||
	   (hScaler->hWindow->stCurInfo.stSclSettings.ulSclDejaggingGain      != hScaler->stSclSettings.ulSclDejaggingGain     ) ||
	   (hScaler->hWindow->stCurInfo.stSclSettings.ulSclDejaggingHorz      != hScaler->stSclSettings.ulSclDejaggingHorz     ) ||
	   (hScaler->hWindow->stCurInfo.stCtIndex.ulSclHorzLuma   != hScaler->stCtIndex.ulSclHorzLuma   ) ||
	   (hScaler->hWindow->stCurInfo.stCtIndex.ulSclHorzChroma != hScaler->stCtIndex.ulSclHorzChroma ) ||
	   (hScaler->hWindow->stCurInfo.stCtIndex.ulSclVertLuma   != hScaler->stCtIndex.ulSclVertLuma   ) ||
	   (hScaler->hWindow->stCurInfo.stCtIndex.ulSclVertChroma != hScaler->stCtIndex.ulSclVertChroma ) ||
	   (hScaler->hWindow->stCurInfo.stSplitScreenSetting.eDeJagging != hScaler->ePrevDeJagging) ||
	   (hScaler->hWindow->stCurInfo.stSplitScreenSetting.eDeRinging != hScaler->ePrevDeRinging) ||
	   (hScaler->hWindow->stCurInfo.hSource->stCurInfo.eCtInputType  != hScaler->ePrevCtInputType)   ||
	   (pPicture->ulNonlinearSrcWidth    != hScaler->ulPrevNonlinearSrcWidth)    ||
	   (pPicture->ulNonlinearSclOutWidth != hScaler->ulPrevNonlinearSclOutWidth) ||
#if (BVDC_P_SUPPORT_3D_VIDEO)
		(pPicture->eSrcOrientation        != hScaler->ePrevSrcOrientation)    ||
		(pPicture->eDispOrientation       != hScaler->ePrevDispOrientation)   ||
#endif
	   ((pPicture->eSrcPolarity == BAVC_Polarity_eFrame) !=
	    (hScaler->ePrevSrcPolarity == BAVC_Polarity_eFrame)) ||
	   ((pPicture->eDstPolarity == BAVC_Polarity_eFrame) !=
	    (hScaler->ePrevDstPolarity == BAVC_Polarity_eFrame)) ||
#if (BVDC_P_SUPPORT_SCL_VER > BVDC_P_SUPPORT_SCL_VER_1)
	   !BVDC_P_SCL_COMPARE_FIELD_DATA(SCL_0_ENABLE, SCALER_ENABLE, 1) ||
#else
	   !BVDC_P_SCL_COMPARE_FIELD_DATA(SCL_0_TOP_CONTROL, SCALER_ENABLE, 1) ||
#endif
	   (pPicture->stFlags.bHandleFldInv != hScaler->bHandleFldInv))
	{
		uint32_t ulFirVrtStepWithKellFactor;

		/* for next "dirty" check */
		hScaler->stPrevSclIn  = *pSclIn;
		hScaler->stPrevSclOut = *pSclOut;
		hScaler->stPrevSclCut = *pSclCut;
		hScaler->stCtIndex    = hScaler->hWindow->stCurInfo.stCtIndex;
		hScaler->stSclSettings = hScaler->hWindow->stCurInfo.stSclSettings;
		hScaler->ulPrevNonlinearSrcWidth = pPicture->ulNonlinearSrcWidth;
		hScaler->ulPrevNonlinearSclOutWidth = pPicture->ulNonlinearSclOutWidth;
		hScaler->ePrevSrcPolarity = pPicture->eSrcPolarity;
		hScaler->ePrevDstPolarity = pPicture->eDstPolarity;
		hScaler->ePrevSrcOrientation  = pPicture->eSrcOrientation;
		hScaler->ePrevDispOrientation = pPicture->eDispOrientation;
		hScaler->ePrevDeJagging = hScaler->hWindow->stCurInfo.stSplitScreenSetting.eDeJagging;
		hScaler->ePrevDeRinging = hScaler->hWindow->stCurInfo.stSplitScreenSetting.eDeRinging;
		hScaler->ePrevCtInputType = hScaler->hWindow->stCurInfo.hSource->stCurInfo.eCtInputType;
		hScaler->ulUpdateAll = BVDC_P_RUL_UPDATE_THRESHOLD;
		hScaler->bHandleFldInv = pPicture->stFlags.bHandleFldInv;

		/* -----------------------------------------------------------------------
		 * 1). Init some regitsers first, they might be modified later basing on
		 * specific conditions
		 */

		/* scaler panscan will be combined with init phase */
		BVDC_P_SCL_GET_REG_DATA(SCL_0_SRC_PIC_HORIZ_PAN_SCAN) &= ~(
			BCHP_MASK(SCL_0_SRC_PIC_HORIZ_PAN_SCAN, OFFSET));
		BVDC_P_SCL_GET_REG_DATA(SCL_0_SRC_PIC_VERT_PAN_SCAN) &= ~(
			BCHP_MASK(SCL_0_SRC_PIC_VERT_PAN_SCAN, OFFSET));

		/* Horizontal scaler settings (and top control)!  Choose scaling order,
		 * and how much to decimate data. */
		BVDC_P_SCL_GET_REG_DATA(SCL_0_TOP_CONTROL) &= ~(
#if (BVDC_P_SUPPORT_SCL_VER == BVDC_P_SUPPORT_SCL_VER_1)
			BCHP_MASK(SCL_0_TOP_CONTROL, SCALER_ENABLE) |
#endif
#if (BVDC_P_SUPPORT_SCL_VER == BVDC_P_SUPPORT_SCL_VER_3)
			BCHP_MASK(SCL_0_TOP_CONTROL, DERING_DEMO_MODE) |
#endif
			BCHP_MASK(SCL_0_TOP_CONTROL, FILTER_ORDER ));

		BVDC_P_SCL_GET_REG_DATA(SCL_0_TOP_CONTROL) |=  (
			BCHP_FIELD_ENUM(SCL_0_TOP_CONTROL, UPDATE_SEL,   ALWAYS_UPDATE) |
#if (BVDC_P_SUPPORT_SCL_VER != BVDC_P_SUPPORT_SCL_VER_3)
		/* disable it for now due to robustness issue for 3548 Ax */
		BCHP_FIELD_ENUM(SCL_0_TOP_CONTROL, ENABLE_CTRL,  ENABLE_BY_PICTURE) |
#endif
#if (BVDC_P_SUPPORT_SCL_VER == BVDC_P_SUPPORT_SCL_VER_1)
			BCHP_FIELD_ENUM(SCL_0_TOP_CONTROL, SCALER_ENABLE, ON       ) |
#endif
#if (BVDC_P_SUPPORT_SCL_VER == BVDC_P_SUPPORT_SCL_VER_3)
			BCHP_FIELD_ENUM(SCL_0_TOP_CONTROL, DERING_DEMO_MODE, DISABLE) |
#endif
			BCHP_FIELD_ENUM(SCL_0_TOP_CONTROL, FILTER_ORDER, VERT_FIRST));

#if (BVDC_P_SUPPORT_SCL_VER > BVDC_P_SUPPORT_SCL_VER_1)
		BVDC_P_SCL_GET_REG_DATA(SCL_0_ENABLE) &= ~(
			BCHP_MASK(SCL_0_ENABLE, SCALER_ENABLE));
		BVDC_P_SCL_GET_REG_DATA(SCL_0_ENABLE) |=
			BCHP_FIELD_ENUM(SCL_0_ENABLE, SCALER_ENABLE, ON);
#endif

		/* HW half band filters are initialized as OFF */
		BVDC_P_SCL_GET_REG_DATA(SCL_0_HORIZ_CONTROL) &= ~(
			BCHP_MASK(SCL_0_HORIZ_CONTROL, HWF0_ENABLE) |
			BCHP_MASK(SCL_0_HORIZ_CONTROL, HWF1_ENABLE) |
			BCHP_MASK(SCL_0_HORIZ_CONTROL, FIR_ENABLE)  |
			BCHP_MASK(SCL_0_HORIZ_CONTROL, MASK_HSCL_LONG_LINE)  |
			BCHP_MASK(SCL_0_HORIZ_CONTROL, MASK_HSCL_SHORT_LINE) |
			BCHP_MASK(SCL_0_HORIZ_CONTROL, STALL_DRAIN_ENABLE));
		BVDC_P_SCL_GET_REG_DATA(SCL_0_HORIZ_CONTROL) |=  (
			BCHP_FIELD_ENUM(SCL_0_HORIZ_CONTROL, HWF0_ENABLE, OFF) |
			BCHP_FIELD_ENUM(SCL_0_HORIZ_CONTROL, HWF1_ENABLE, OFF) |
			BCHP_FIELD_ENUM(SCL_0_HORIZ_CONTROL, FIR_ENABLE,  ON ) |
			BCHP_FIELD_ENUM(SCL_0_HORIZ_CONTROL, MASK_HSCL_LONG_LINE,  ON) |
			BCHP_FIELD_ENUM(SCL_0_HORIZ_CONTROL, MASK_HSCL_SHORT_LINE, ON) |
			BCHP_FIELD_ENUM(SCL_0_HORIZ_CONTROL, STALL_DRAIN_ENABLE, OFF));

		/* -----------------------------------------------------------------------
		 * 2). Need to calculate the horizontal scaling factors before src width
		 * alignment and init phase can be decided
		 */

		/* output size */
		ulDstHSize = pSclOut->ulWidth;
		ulDstVSize = (BAVC_Polarity_eFrame==pPicture->eDstPolarity
#if BFMT_LEGACY_3DTV_SUPPORT
			&& (!VIDEO_FORMAT_IS_CUSTOM_1080P3D(pPicture->hBuffer->hWindow->hCompositor->stCurInfo.pFmtInfo->eVideoFmt))
#endif
			)? pSclOut->ulHeight : pSclOut->ulHeight / BVDC_P_FIELD_PER_FRAME;

		BVDC_P_SCL_GET_REG_DATA(SCL_0_DEST_PIC_SIZE) &= ~(
			BCHP_MASK(SCL_0_DEST_PIC_SIZE, HSIZE) |
			BCHP_MASK(SCL_0_DEST_PIC_SIZE, VSIZE));
		BVDC_P_SCL_GET_REG_DATA(SCL_0_DEST_PIC_SIZE) |=  (
			BCHP_FIELD_DATA(SCL_0_DEST_PIC_SIZE, HSIZE, ulDstHSize) |
			BCHP_FIELD_DATA(SCL_0_DEST_PIC_SIZE, VSIZE, ulDstVSize));

		/* the src size really scaled and output */
		ulSrcHSize = pSclCut->ulWidth;
		ulSrcVSize = (BAVC_Polarity_eFrame == pPicture->eSrcPolarity)?
			pSclCut->ulHeight :
			pSclCut->ulHeight / BVDC_P_FIELD_PER_FRAME;

		/* pan scan:  left format S11.6, top format S11.14 */
		ulPanScanTop  = (BAVC_Polarity_eFrame == pPicture->eSrcPolarity)?
			pSclCut->lTop :
			pSclCut->lTop / BVDC_P_FIELD_PER_FRAME;
		ulPanScanLeft = pSclCut->lLeft;

		/* separate the amount cut by SCL_0_PIC_OFFSET and FIR_LUMA_SRC_PIC_OFFSET */
		ulPicOffsetLeft = (ulPanScanLeft >> 6) & ~(hScaler->ulSrcHrzAlign - 1);
		ulPicOffsetTop  = (ulPanScanTop  >> 14);
#if (BVDC_P_SUPPORT_SCL_VER >= BVDC_P_SUPPORT_SCL_VER_8)
		ulPicOffsetLeft_R = (pSclCut->lLeft_R >> 6) & ~(hScaler->ulSrcHrzAlign - 1);
#endif

		ulPanScanLeft -= (ulPicOffsetLeft << 6);
		ulPanScanTop  -= (ulPicOffsetTop  << 14);

		/* the src size that get into the first scaler sub-modules (e.g. HW half-band
		 * filter if it is scaled down a lot): it includes the FIR_LUMA_SRC_PIC_OFFSET,
		 * but not the SCL_0_PIC_OFFSET, it has to be rounded-up for alignment */
		ulMaxX = ulPanScanLeft + (ulSrcHSize << 6);
		ulMaxY = ulPanScanTop  + (ulSrcVSize << 14);
		ulAlgnSrcHSize = ((ulMaxX + ((1<< 6) - 1)) >>  6);
		ulAlgnSrcVSize = ((ulMaxY + ((1<<14) - 1)) >> 14);
		ulAlgnSrcHSize = BVDC_P_ALIGN_UP(ulAlgnSrcHSize, hScaler->ulSrcHrzAlign);

		/* Init the input/output horizontal/vertical size of FIRs */
		ulFirSrcHSize = ulSrcHSize;
		ulFirDstHSize = ulDstHSize;
		ulFirSrcVSize = ulSrcVSize;
		ulFirDstVSize = ulDstVSize;

		/* this is the src width into vertical scaler */
		ulVertSclSrcWidth = ulSrcHSize;

		/* for linear scaling mode, horizontal scaling may turn on HW filters */
		if(0 == pPicture->ulNonlinearSclOutWidth)
		{
			/* Horizantal step HW reg uses U5.17 in older arch, U5.26 after smooth non-linear is
			 * suported. Since CPU uses 32 bits int, calc step with 26 bits frac needs special
			 * handling (see the delta calcu in the case of nonlinear scaling). It is the step
			 * delta and internal step accum reg, not the initial step value, that really need 26
			 * frac bits, therefore we use 20 bits for trade off */
			ulNrmHrzStep = pPicture->ulNrmHrzSrcStep;    /* U12.20 */
			ulFirHrzStep = ulHrzStep = BVDC_P_SCL_H_STEP_NRM_TO_SPEC(ulNrmHrzStep); /* U4.17, U5.17, U5.26 */

			/* Use Hard-Wired-Filters to assist FIR extreme scale down */
			if(ulHrzStep > BVDC_P_SCL_1ST_DECIMATION_THRESHOLD)
			{
				BVDC_P_SCL_GET_REG_DATA(SCL_0_HORIZ_CONTROL) &= ~(
					BCHP_MASK(SCL_0_HORIZ_CONTROL, HWF0_ENABLE));
				BVDC_P_SCL_GET_REG_DATA(SCL_0_HORIZ_CONTROL) |=  (
					BCHP_FIELD_ENUM(SCL_0_HORIZ_CONTROL, HWF0_ENABLE, ON));
				ulFirHrzStep /= BVDC_P_SCL_HORZ_HWF_FACTOR;
				ulFirSrcHSize /= BVDC_P_SCL_HORZ_HWF_FACTOR;
				ulPanScanLeft /= BVDC_P_SCL_HORZ_HWF_FACTOR;
				if (1 != hScaler->ulSrcHrzAlign)
					ulAlgnSrcHSize = BVDC_P_ALIGN_UP(ulAlgnSrcHSize, 2 * hScaler->ulSrcHrzAlign);

				if(ulHrzStep > BVDC_P_SCL_2ND_DECIMATION_THRESHOLD)
				{
					BVDC_P_SCL_GET_REG_DATA(SCL_0_HORIZ_CONTROL) &= ~(
						BCHP_MASK(SCL_0_HORIZ_CONTROL, HWF1_ENABLE));
					BVDC_P_SCL_GET_REG_DATA(SCL_0_HORIZ_CONTROL) |=  (
						BCHP_FIELD_ENUM(SCL_0_HORIZ_CONTROL, HWF1_ENABLE, ON));
					ulFirHrzStep /= BVDC_P_SCL_HORZ_HWF_FACTOR;
					ulFirSrcHSize /= BVDC_P_SCL_HORZ_HWF_FACTOR;
					ulPanScanLeft /= BVDC_P_SCL_HORZ_HWF_FACTOR;
					if (1 != hScaler->ulSrcHrzAlign)
						ulAlgnSrcHSize = BVDC_P_ALIGN_UP(ulAlgnSrcHSize, 4 * hScaler->ulSrcHrzAlign);
				}
			}
#if (BVDC_P_SUPPORT_SCL_VER >= BVDC_P_SUPPORT_SCL_VER_4)
			else if (((1<<BVDC_P_NRM_SRC_STEP_F_BITS) == ulNrmHrzStep) &&
			         (0 == ulPanScanLeft) &&
			         (ulSrcHSize == ulDstHSize) &&
			         (!hScaler->stCtIndex.ulSclHorzLuma) &&
			         (!hScaler->stCtIndex.ulSclHorzChroma))
			{
				/* unity scale and no phase shift, turn off FIR for accuracy */
				BVDC_P_SCL_GET_REG_DATA(SCL_0_HORIZ_CONTROL) &= ~(
					BCHP_MASK(SCL_0_HORIZ_CONTROL, FIR_ENABLE));
				BVDC_P_SCL_GET_REG_DATA(SCL_0_HORIZ_CONTROL) |=  (
					BCHP_FIELD_ENUM(SCL_0_HORIZ_CONTROL, FIR_ENABLE,  OFF));
			}
#endif

			ulFirHrzStepInit = ulFirHrzStep;

			/* set step size and region_0 end */
			BVDC_P_SCL_SET_HORZ_RATIO(ulFirHrzStep);
			BVDC_P_SCL_SET_HORZ_REGION02(0, ulDstHSize, 0)
		}
		else
		{
			/* 5 Zone nonlinear function, backwards compatible with 3-Zone nonlinear*/
			BVDC_P_Scaler_5ZoneNonLinear(hScaler, pPicture, &ulFirHrzStep, &ulHrzStep,&ulFirHrzStepInit);
		}

		/* -----------------------------------------------------------------------
		 * 3). Now we can set src size, offset and bvb size
		 */
		ulBvbInHSize = pSclIn->ulWidth;
		ulBvbInVSize = (BAVC_Polarity_eFrame==pPicture->eSrcPolarity)?
			(pSclIn->ulHeight) :
			(pSclIn->ulHeight) / BVDC_P_FIELD_PER_FRAME;

		/* in older chips, align ulBvbInHSize up if ulAlgnSrcHSize has been aligned
		 * up due to half-band.
		 * note: align ulBvbInHSize up might cause "short line" error, that is OK
		 * and scl input module would patch. If we don't align up, SCL might hang */
		if (1 != hScaler->ulSrcHrzAlign)
			ulBvbInHSize  = BVDC_P_MAX(ulBvbInHSize, ulAlgnSrcHSize + ulPicOffsetLeft);
		else
			ulAlgnSrcHSize = BVDC_P_MIN(ulAlgnSrcHSize, ulBvbInHSize - ulPicOffsetLeft);

		/* without this we might see flash when we move up with dst cut if MAD is disabled? */
		/*ulBvbInVSize  = BVDC_P_MAX(ulBvbInVSize, ulAlgnSrcVSize + ulPicOffsetTop);*/

		/* Make sure scaler BVN in V size, picture offset and src picture size are inline.
		 * Otherwise we may run into short field error --- a long delay to patch and to
		 * cause.enable_error with next enabling.
		 */
		ulAlgnSrcVSize = BVDC_P_MIN(ulAlgnSrcVSize, ulBvbInVSize - ulPicOffsetTop);

		BVDC_P_SCL_GET_REG_DATA(SCL_0_PIC_OFFSET) &= ~(
			BCHP_MASK(SCL_0_PIC_OFFSET, HSIZE) |
			BCHP_MASK(SCL_0_PIC_OFFSET, VSIZE));
		BVDC_P_SCL_GET_REG_DATA(SCL_0_PIC_OFFSET) |=  (
			BCHP_FIELD_DATA(SCL_0_PIC_OFFSET, HSIZE, ulPicOffsetLeft) |
			BCHP_FIELD_DATA(SCL_0_PIC_OFFSET, VSIZE, ulPicOffsetTop));

#if (BVDC_P_SUPPORT_SCL_VER >= BVDC_P_SUPPORT_SCL_VER_8)
		BVDC_P_SCL_GET_REG_DATA(SCL_0_PIC_OFFSET_R) &= ~(
			BCHP_MASK(SCL_0_PIC_OFFSET_R, ENABLE) |
			BCHP_MASK(SCL_0_PIC_OFFSET_R, HSIZE));
		BVDC_P_SCL_GET_REG_DATA(SCL_0_PIC_OFFSET_R) |=  (
			BCHP_FIELD_DATA(SCL_0_PIC_OFFSET_R, ENABLE, (ulPicOffsetLeft != ulPicOffsetLeft_R)) |
			BCHP_FIELD_DATA(SCL_0_PIC_OFFSET_R, HSIZE, ulPicOffsetLeft_R));
#endif

#ifdef BVDC_P_SUPPORT_3D_VIDEO
		if(pPicture->eSrcOrientation != pPicture->eOrigSrcOrientation)
		{
			ulBvbInHSize <<=
				((pPicture->eOrigSrcOrientation == BFMT_Orientation_e3D_LeftRight) &&
				 (pPicture->eSrcOrientation == BFMT_Orientation_e2D));
		}
#endif
		BVDC_P_SCL_GET_REG_DATA(SCL_0_BVB_IN_SIZE) &= ~(
			BCHP_MASK(SCL_0_BVB_IN_SIZE, HSIZE) |
			BCHP_MASK(SCL_0_BVB_IN_SIZE, VSIZE));
		BVDC_P_SCL_GET_REG_DATA(SCL_0_BVB_IN_SIZE) |=  (
			BCHP_FIELD_DATA(SCL_0_BVB_IN_SIZE, HSIZE, ulBvbInHSize) |
			BCHP_FIELD_DATA(SCL_0_BVB_IN_SIZE, VSIZE, ulBvbInVSize));

		/* SRC_PIC_SIZE should include FIR_LUMA_SRC_PIC_OFFSET and align */
		BVDC_P_SCL_GET_REG_DATA(SCL_0_SRC_PIC_SIZE) &= ~(
			BCHP_MASK(SCL_0_SRC_PIC_SIZE, HSIZE) |
			BCHP_MASK(SCL_0_SRC_PIC_SIZE, VSIZE));
		BVDC_P_SCL_GET_REG_DATA(SCL_0_SRC_PIC_SIZE) |=  (
			BCHP_FIELD_DATA(SCL_0_SRC_PIC_SIZE, HSIZE, ulAlgnSrcHSize) |
			BCHP_FIELD_DATA(SCL_0_SRC_PIC_SIZE, VSIZE, ulAlgnSrcVSize));

		/* If it is horizontally scaled down, we do horizontal scale first */
		if(ulHrzStep >= BVDC_P_SCL_H_RATIO_ONE)
		{
			BVDC_P_SCL_GET_REG_DATA(SCL_0_TOP_CONTROL) &= ~(
				BCHP_MASK(SCL_0_TOP_CONTROL, FILTER_ORDER ));
			BVDC_P_SCL_GET_REG_DATA(SCL_0_TOP_CONTROL) |=  (
				BCHP_FIELD_ENUM(SCL_0_TOP_CONTROL, FILTER_ORDER, HORIZ_FIRST));

			ulVertSclSrcWidth = ulDstHSize;
		}

		/* -----------------------------------------------------------------------
		 * 4). Now we compute vertical scale factor
		 */
		/* calculate the overall Scaling Factors for V */
		/* NOTE: the scaling source should have clipped the pan/scan offset. */
		ulNrmVrtSrcStep = (BAVC_Polarity_eFrame == pPicture->eSrcPolarity)?
			pPicture->ulNrmVrtSrcStep : (pPicture->ulNrmVrtSrcStep >> 1);  /* U12.20 */
		ulNrmVrtSrcStep = (BAVC_Polarity_eFrame == pPicture->eDstPolarity
#if BFMT_LEGACY_3DTV_SUPPORT
			&& (!VIDEO_FORMAT_IS_CUSTOM_1080P3D(pPicture->hBuffer->hWindow->hCompositor->stCurInfo.pFmtInfo->eVideoFmt))
#endif
			)? ulNrmVrtSrcStep : (ulNrmVrtSrcStep << 1); /* U12.20 */

		ulFirVrtStep = ulVrtStep = ulNrmVrtSrcStep >>
			(BVDC_P_NRM_SRC_STEP_F_BITS - BVDC_P_SCL_V_RATIO_F_BITS);    /* U12.14 */

#if (BVDC_P_SUPPORT_SCL_VER < BVDC_P_SUPPORT_SCL_VER_4)
		/* Might need to tablelized this for different chips. */
		BVDC_P_SCL_GET_REG_DATA(SCL_0_VERT_CONTROL) &= ~(
			BCHP_MASK(SCL_0_VERT_CONTROL, MODE));
		if(ulVertSclSrcWidth <= hScaler->ulVertLineDepth)
		{
			if(ulVrtStep > BVDC_P_SCL_4TAP_BLK_AVG_VERT_THRESHOLD)
			{
				/* Extreme scale down */
				BVDC_P_SCL_GET_REG_DATA(SCL_0_VERT_CONTROL) |=  (
					BCHP_FIELD_ENUM(SCL_0_VERT_CONTROL, MODE, AV4));
			}
			else
			{
				BVDC_P_SCL_GET_REG_DATA(SCL_0_VERT_CONTROL) |=  (
					BCHP_FIELD_ENUM(SCL_0_VERT_CONTROL, MODE, FIR4));
			}
			hScaler->ulVertTaps = BVDC_P_CT_4_TAP;
		}
		else
		{
			if(ulVrtStep > BVDC_P_SCL_2TAP_BLK_AVG_VERT_THRESHOLD)
			{
				/* Extreme scale down */
				BVDC_P_SCL_GET_REG_DATA(SCL_0_VERT_CONTROL) |=  (
					BCHP_FIELD_ENUM(SCL_0_VERT_CONTROL, MODE, AV2));
			}
			else
			{
				BVDC_P_SCL_GET_REG_DATA(SCL_0_VERT_CONTROL) |=  (
					BCHP_FIELD_ENUM(SCL_0_VERT_CONTROL, MODE, FIR2));
			}
			hScaler->ulVertTaps = BVDC_P_CT_2_TAP;
		}
#else
		BVDC_P_SCL_GET_REG_DATA(SCL_0_VERT_CONTROL) &= ~(
			BCHP_MASK(SCL_0_VERT_CONTROL, SEL_4TAP_IN_FIR8 ) |
			BCHP_MASK(SCL_0_VERT_CONTROL, FIR_ENABLE       ));
		BVDC_P_SCL_GET_REG_DATA(SCL_0_VERT_CONTROL) |=  (
			BCHP_FIELD_ENUM(SCL_0_VERT_CONTROL, SEL_4TAP_IN_FIR8, DISABLE) |
			BCHP_FIELD_ENUM(SCL_0_VERT_CONTROL, FIR_ENABLE,       ON    ));
#endif

		/* Use block-averging to assist FIR extreme scale down.
		 * Increase ulFirVrtStep until tolerable range or until
		 * we exhausted block average scaling. */
#if (BVDC_P_SUPPORT_SCL_VER < BVDC_P_SUPPORT_SCL_VER_4)
		if(BVDC_P_SCL_COMPARE_FIELD_NAME(SCL_0_VERT_CONTROL, MODE, AV2) ||
		   BVDC_P_SCL_COMPARE_FIELD_NAME(SCL_0_VERT_CONTROL, MODE, AV4))
#endif
		{

#if (BVDC_P_SUPPORT_SCL_VER < BVDC_P_SUPPORT_SCL_VER_4)
			if(ulVertSclSrcWidth < hScaler->ulVertLineDepth)
				hScaler->ulVertBlkAvgThreshold = BVDC_P_SCL_4TAP_BLK_AVG_VERT_THRESHOLD;
			else
				hScaler->ulVertBlkAvgThreshold = BVDC_P_SCL_2TAP_BLK_AVG_VERT_THRESHOLD;
#endif

			if(pPicture->eSrcPolarity != BAVC_Polarity_eFrame)
			{
				/* src = T/B */
				if(pPicture->eDstPolarity == BAVC_Polarity_eFrame)
				{
					/* src = T/B, dst = F => pick max out of the 2 */
					if(BERR_SUCCESS != BVDC_P_Scaler_CalVertInitPhase_isr(hScaler,
						BAVC_Polarity_eTopField, BAVC_Polarity_eFrame,    ulVrtStep, ulPanScanTop, &ulBlkAvgSize))
						BDBG_WRN(("Max BlkAvg but fail constraint %dx%d(T) => %dx%d(F)",
							pSclIn->ulWidth,  pSclIn->ulHeight,
							pSclOut->ulWidth, pSclOut->ulHeight));
					if(BERR_SUCCESS != BVDC_P_Scaler_CalVertInitPhase_isr(hScaler,
						BAVC_Polarity_eBotField, BAVC_Polarity_eFrame,    ulVrtStep, ulPanScanTop, &ulBlkAvgSize))
						BDBG_WRN(("Max BlkAvg but fail constraint %dx%d(B) => %dx%d(F)",
							pSclIn->ulWidth,  pSclIn->ulHeight,
							pSclOut->ulWidth, pSclOut->ulHeight));
					BDBG_MSG(("T/B => F: largest ulBlkAvgSize = %d", ulBlkAvgSize));
				}
				else
				{
					/* src = T/B, dst = T/B => pick max out of the 4 */
					if(BERR_SUCCESS != BVDC_P_Scaler_CalVertInitPhase_isr(hScaler,
						BAVC_Polarity_eTopField, BAVC_Polarity_eTopField, ulVrtStep, ulPanScanTop, &ulBlkAvgSize))
						BDBG_WRN(("Max BlkAvg but fail constraint %dx%d(T) => %dx%d(T)",
							pSclIn->ulWidth,  pSclIn->ulHeight,
							pSclOut->ulWidth, pSclOut->ulHeight));
					if(BERR_SUCCESS != BVDC_P_Scaler_CalVertInitPhase_isr(hScaler,
						BAVC_Polarity_eBotField, BAVC_Polarity_eTopField, ulVrtStep, ulPanScanTop, &ulBlkAvgSize))
						BDBG_WRN(("Max BlkAvg but fail constraint %dx%d(B) => %dx%d(T)",
							pSclIn->ulWidth,  pSclIn->ulHeight,
							pSclOut->ulWidth, pSclOut->ulHeight));
					if(BERR_SUCCESS != BVDC_P_Scaler_CalVertInitPhase_isr(hScaler,
						BAVC_Polarity_eTopField, BAVC_Polarity_eBotField, ulVrtStep, ulPanScanTop, &ulBlkAvgSize))
						BDBG_WRN(("Max BlkAvg but fail constraint %dx%d(T) => %dx%d(B)",
							pSclIn->ulWidth,  pSclIn->ulHeight,
							pSclOut->ulWidth, pSclOut->ulHeight));
					if(BERR_SUCCESS != BVDC_P_Scaler_CalVertInitPhase_isr(hScaler,
						BAVC_Polarity_eBotField, BAVC_Polarity_eBotField, ulVrtStep, ulPanScanTop, &ulBlkAvgSize))
						BDBG_WRN(("Max BlkAvg but fail constraint %dx%d(B) => %dx%d(B)",
							pSclIn->ulWidth,  pSclIn->ulHeight,
							pSclOut->ulWidth, pSclOut->ulHeight));
					BDBG_MSG(("T/B => T/B: largest ulBlkAvgSize = %d", ulBlkAvgSize));
				}
			}
			else
			{
				/* src = F */
				if(pPicture->eDstPolarity != BAVC_Polarity_eFrame)
				{
					/* src = F, dst = T/B => pick max out of the 2 */
					if(BERR_SUCCESS != BVDC_P_Scaler_CalVertInitPhase_isr(hScaler,
						BAVC_Polarity_eFrame,    BAVC_Polarity_eTopField, ulVrtStep, ulPanScanTop, &ulBlkAvgSize))
						BDBG_WRN(("Max BlkAvg but fail constraint %dx%d(F) => %dx%d(T)",
							pSclIn->ulWidth,  pSclIn->ulHeight,
							pSclOut->ulWidth, pSclOut->ulHeight));
					if(BERR_SUCCESS != BVDC_P_Scaler_CalVertInitPhase_isr(hScaler,
						BAVC_Polarity_eFrame,    BAVC_Polarity_eBotField, ulVrtStep, ulPanScanTop, &ulBlkAvgSize))
						BDBG_WRN(("Max BlkAvg but fail constraint %dx%d(F) => %dx%d(B)",
							pSclIn->ulWidth,  pSclIn->ulHeight,
							pSclOut->ulWidth, pSclOut->ulHeight));
					BDBG_MSG(("F => T/B: largest ulBlkAvgSize = %d", ulBlkAvgSize));
				}
				else
				{
					if(BERR_SUCCESS != BVDC_P_Scaler_CalVertInitPhase_isr(hScaler,
						BAVC_Polarity_eFrame,    BAVC_Polarity_eFrame,    ulVrtStep, ulPanScanTop, &ulBlkAvgSize))
						BDBG_WRN(("Max BlkAvg but fail constraint %dx%d(F) => %dx%d(F)",
							pSclIn->ulWidth,  pSclIn->ulHeight,
							pSclOut->ulWidth, pSclOut->ulHeight));
					BDBG_MSG(("F => F: largest ulBlkAvgSize = %d", ulBlkAvgSize));
				}
			}
			ulFirVrtStep = (ulVrtStep / (ulBlkAvgSize + 1));

			/* Block averaging is another cheap way of downscaling. */
			ulPanScanTop  /= (ulBlkAvgSize + 1);
			ulFirSrcVSize /= (ulBlkAvgSize + 1);
		}

		BVDC_P_SCL_GET_REG_DATA(SCL_0_VERT_CONTROL) &= ~(
			BCHP_MASK(SCL_0_VERT_CONTROL, BAVG_BLK_SIZE));
		BVDC_P_SCL_GET_REG_DATA(SCL_0_VERT_CONTROL) |=  (
			BCHP_FIELD_DATA(SCL_0_VERT_CONTROL, BAVG_BLK_SIZE, ulBlkAvgSize));

		ulFirVrtStep = BVDC_P_SCL_V_RATIO_TRUNC(ulFirVrtStep);
		BVDC_P_SCL_SET_VERT_RATIO(ulFirVrtStep);

		/* -----------------------------------------------------------------------
		 * 5). set coeffs for both horizontal and vertical
		 */
		pHorzFirCoeff = BVDC_P_SelectFirCoeff_isr(hScaler->pHorzFirCoeffTbl,
			hScaler->hWindow->stCurInfo.stCtIndex.ulSclHorzLuma,
			hScaler->hWindow->stCurInfo.hSource->stCurInfo.eCtInputType,
			pPicture->PicComRulInfo.eSrcOrigPolarity, ulFirHrzStep,
			hScaler->ulHorzTaps, ulFirSrcHSize, ulFirDstHSize);
		BDBG_MSG(("Luma H Coeffs  : %s", pHorzFirCoeff->pchName));
		BVDC_P_Scaler_SetHorizFirCoeff(hScaler, pHorzFirCoeff->pulCoeffs);

		pHorzFirCoeff = BVDC_P_SelectFirCoeff_isr(hScaler->pChromaHorzFirCoeffTbl,
			hScaler->hWindow->stCurInfo.stCtIndex.ulSclHorzChroma,
			hScaler->hWindow->stCurInfo.hSource->stCurInfo.eCtInputType,
			pPicture->PicComRulInfo.eSrcOrigPolarity, ulFirHrzStep,
			hScaler->ulHorzTaps, ulFirSrcHSize, ulFirDstHSize);
		BDBG_MSG(("Chroma H Coeffs: %s", pHorzFirCoeff->pchName));
		BVDC_P_Scaler_SetChromaHorizFirCoeff(hScaler, pHorzFirCoeff->pulCoeffs);

		/* PR48032: Account for Kell Factor.  In the case of P -> I.
		 * Treat Destination as (KELL_FACTOR * Destination_lines * 2).  For
		 * example desintaion is:
		 *  1280x720p -> 1920x1080i, then it's (540 lines * 2 * KELL_FACTOR).
		 * = (540 * 2 * 0.70).  In PI term it's
		 * Sy = in_y / out_y;
		 * Sy = 1280 / (540 * 2 * KELL_FACTOR).  So to account for KELL factor
		 * We needed
		 */
		/* If scale factor (Sy = DstH / SrcH) is within range (Sy <= 1.5 and
		 * Sy != 1), use Kell factor filter, otherwise no filter (PR36666).
		 * Internally, Sy = in_y / out_y = SrcH / DstH * 2 (for interlace case)
		 * therefore the range is Sy <= 1/1.5 or Sy <= 0.6667 and Sy != 1
		 */
		ulFirVrtStepWithKellFactor = (
			(BAVC_Polarity_eFrame == pPicture->eSrcPolarity) &&
			(BAVC_Polarity_eFrame != pPicture->eDstPolarity) &&
			((ulVrtStep / 2) >= BVDC_P_SCL_V_RATIO_KELL_RANGE &&
			 (ulVrtStep / 2) != BVDC_P_SCL_V_RATIO_ONE))
			? BVDC_P_APPLY_KELL_FACTOR(ulFirVrtStep)     /* P -> I w/ kell */
			: ulFirVrtStep;                              /* normal */

		/* We need to choose good fir coefficients base on adjusted
		 * fir scale down ratio, src, dst size (of FIR). */
		hScaler->pVertFirCoeff = BVDC_P_SelectFirCoeff_isr(hScaler->pVertFirCoeffTbl,
			hScaler->hWindow->stCurInfo.stCtIndex.ulSclVertLuma,
			hScaler->hWindow->stCurInfo.hSource->stCurInfo.eCtInputType,
			pPicture->PicComRulInfo.eSrcOrigPolarity, ulFirVrtStepWithKellFactor,
			hScaler->ulVertTaps, ulFirSrcVSize, ulFirDstVSize);
		hScaler->pChromaVertFirCoeff = BVDC_P_SelectFirCoeff_isr(hScaler->pChromaVertFirCoeffTbl,
			hScaler->hWindow->stCurInfo.stCtIndex.ulSclVertChroma,
			hScaler->hWindow->stCurInfo.hSource->stCurInfo.eCtInputType,
			pPicture->PicComRulInfo.eSrcOrigPolarity, ulFirVrtStepWithKellFactor,
			hScaler->ulVertTaps, ulFirSrcVSize, ulFirDstVSize);

		/* to void point sampling if init phase has fraction */
		if((BVDC_P_SCL_V_RATIO_ONE == ulFirVrtStepWithKellFactor) &&
		   (!hScaler->hWindow->stCurInfo.stCtIndex.ulSclVertLuma)&&
		   (!hScaler->hWindow->stCurInfo.stCtIndex.ulSclVertChroma))
		{
			/* using "ulFirVrtStepWithKellFactor - 1" leads to use a 1toN coeffient */
			hScaler->pFracInitPhaseVertFirCoeff = BVDC_P_SelectFirCoeff_isr(hScaler->pVertFirCoeffTbl,
				hScaler->hWindow->stCurInfo.stCtIndex.ulSclVertLuma,
				hScaler->hWindow->stCurInfo.hSource->stCurInfo.eCtInputType,
				pPicture->PicComRulInfo.eSrcOrigPolarity, ulFirVrtStepWithKellFactor - 1,
				hScaler->ulVertTaps, ulFirSrcVSize, ulFirDstVSize);
			hScaler->pChromaFracInitPhaseVertFirCoeff = BVDC_P_SelectFirCoeff_isr(hScaler->pChromaVertFirCoeffTbl,
				hScaler->hWindow->stCurInfo.stCtIndex.ulSclVertChroma,
				hScaler->hWindow->stCurInfo.hSource->stCurInfo.eCtInputType,
				pPicture->PicComRulInfo.eSrcOrigPolarity, ulFirVrtStepWithKellFactor - 1,
				hScaler->ulVertTaps, ulFirSrcVSize, ulFirDstVSize);

			/* if field inversion, therefore sub-pixel phase, is expected, point sampling
			 * should be completely avoided. This is true src and display are interlaced and
			 * "50 to 60Hz conversion is performed, or 60 to 50Hz conversion is performed */
			if(hScaler->bHandleFldInv)
			{
				hScaler->pVertFirCoeff = hScaler->pFracInitPhaseVertFirCoeff;
				hScaler->pChromaVertFirCoeff = hScaler->pChromaFracInitPhaseVertFirCoeff;
			}
		}
		else
		{
			/* in this case regular coeffs are not point sampling */
			hScaler->pFracInitPhaseVertFirCoeff = hScaler->pVertFirCoeff;
			hScaler->pChromaFracInitPhaseVertFirCoeff = hScaler->pChromaVertFirCoeff;
		}

		/* SW7420-560, SW7420-721: use smoothen vertical coefficient to improve weaving */
#if (BVDC_P_SUPPORT_MCVP_VER == 1)
		if((BVDC_P_VNET_USED_MAD(hScaler->hWindow->stMcvpMode)) && /* mcdi used -> interlaced src */
		   (ulFirVrtStepWithKellFactor <= BVDC_P_SCL_V_RATIO_ONE)) /* scl up/unity */
		{
			BDBG_MSG(("use smoothen vert coeff to improve mcdi weaving"));
			if (0 == hScaler->hWindow->stCurInfo.stCtIndex.ulSclVertLuma)
			{
				hScaler->pVertFirCoeff = hScaler->pFracInitPhaseVertFirCoeff =
					(pSclIn->ulHeight <= BFMT_PAL_HEIGHT)
					? &s_aSmoothVertCoeffTbl[0] : &s_aSmoothVertCoeffTbl[1];
			}
			if (0 == hScaler->hWindow->stCurInfo.stCtIndex.ulSclVertChroma)
			{
				hScaler->pChromaVertFirCoeff = hScaler->pChromaFracInitPhaseVertFirCoeff =
					(pSclIn->ulHeight <= BFMT_PAL_HEIGHT)
					? &s_aSmoothVertCoeffTbl[0] : &s_aSmoothVertCoeffTbl[1];
			}
		}
#endif

		/* DEJAGGING/DERINGING settigns */
#if (BVDC_P_SUPPORT_SCL_VER >= BVDC_P_SUPPORT_SCL_VER_4)
		if(hScaler->bDeJagging)
		{
			bool bDeJagging = (
				(hScaler->stSclSettings.bSclVertDejagging) &&
				(ulFirVrtStepWithKellFactor < BVDC_P_SCL_DEJAGGING_ON_THESHOLD));

			BVDC_P_SCL_GET_REG_DATA(SCL_0_DEJAGGING) = (
#if (BVDC_P_SUPPORT_SCL_VER >= BVDC_P_SUPPORT_SCL_VER_8)
				BCHP_FIELD_DATA(SCL_0_DEJAGGING, CORN, 8                                        ) |
#endif
				BCHP_FIELD_DATA(SCL_0_DEJAGGING, HORIZ,hScaler->stSclSettings.ulSclDejaggingHorz) | /* U2.3 */
				BCHP_FIELD_DATA(SCL_0_DEJAGGING, GAIN, hScaler->stSclSettings.ulSclDejaggingGain) | /* U2.3 */
				BCHP_FIELD_DATA(SCL_0_DEJAGGING, CORE, hScaler->stSclSettings.ulSclDejaggingCore) |
				BCHP_FIELD_DATA(SCL_0_DEJAGGING, DEMO_MODE,
					(BVDC_SplitScreenMode_eDisable != hScaler->hWindow->stCurInfo.stSplitScreenSetting.eDeJagging)) |
				((bDeJagging)
				? BCHP_FIELD_ENUM(SCL_0_DEJAGGING, VERT_DEJAGGING, ON)
				: BCHP_FIELD_ENUM(SCL_0_DEJAGGING, VERT_DEJAGGING, OFF))                         );

			BVDC_P_SCL_GET_REG_DATA(SCL_0_DEJAGGING_DEMO_SETTING) = (
				((BVDC_SplitScreenMode_eLeft == hScaler->hWindow->stCurInfo.stSplitScreenSetting.eDeJagging)
				? BCHP_FIELD_ENUM(SCL_0_DEJAGGING_DEMO_SETTING, DEMO_L_R, LEFT )
				: BCHP_FIELD_ENUM(SCL_0_DEJAGGING_DEMO_SETTING, DEMO_L_R, RIGHT)         )|
				BCHP_FIELD_DATA(SCL_0_DEJAGGING_DEMO_SETTING, DEMO_BOUNDARY, ulDstHSize/2));
		}
#endif

#if (BVDC_P_SUPPORT_SCL_VER >= BVDC_P_SUPPORT_SCL_VER_3)
		if(hScaler->bDeRinging)
		{
			BVDC_P_SCL_GET_REG_DATA(SCL_0_DERINGING) = (
#if (BVDC_P_SUPPORT_SCL_VER >= BVDC_P_SUPPORT_SCL_VER_4)
				BCHP_FIELD_DATA(SCL_0_DERINGING, DEMO_MODE,
					(BVDC_SplitScreenMode_eDisable != hScaler->hWindow->stCurInfo.stSplitScreenSetting.eDeRinging)) |
#endif
#if (BVDC_P_SUPPORT_SCL_VER >= BVDC_P_SUPPORT_SCL_VER_8)
				((hScaler->stSclSettings.bSclVertChromaDeringing)
				? BCHP_FIELD_ENUM(SCL_0_DERINGING, VERT_CHROMA_PASS_FIRST_RING, ENABLE)
				: BCHP_FIELD_ENUM(SCL_0_DERINGING, VERT_CHROMA_PASS_FIRST_RING, DISABLE) ) |
				((hScaler->stSclSettings.bSclVertLumaDeringing)
				? BCHP_FIELD_ENUM(SCL_0_DERINGING, VERT_LUMA_PASS_FIRST_RING, ENABLE)
				: BCHP_FIELD_ENUM(SCL_0_DERINGING, VERT_LUMA_PASS_FIRST_RING, DISABLE)   ) |
				((hScaler->stSclSettings.bSclHorzChromaDeringing)
				? BCHP_FIELD_ENUM(SCL_0_DERINGING, HORIZ_CHROMA_PASS_FIRST_RING, ENABLE)
				: BCHP_FIELD_ENUM(SCL_0_DERINGING, HORIZ_CHROMA_PASS_FIRST_RING, DISABLE)) |
				((hScaler->stSclSettings.bSclHorzLumaDeringing)
				? BCHP_FIELD_ENUM(SCL_0_DERINGING, HORIZ_LUMA_PASS_FIRST_RING, ENABLE)
				: BCHP_FIELD_ENUM(SCL_0_DERINGING, HORIZ_LUMA_PASS_FIRST_RING, DISABLE)  ) |
#endif
				((hScaler->stSclSettings.bSclVertChromaDeringing)
				? BCHP_FIELD_ENUM(SCL_0_DERINGING, VERT_CHROMA_DERINGING, ON)
				: BCHP_FIELD_ENUM(SCL_0_DERINGING, VERT_CHROMA_DERINGING, OFF) ) |
				((hScaler->stSclSettings.bSclVertLumaDeringing)
				? BCHP_FIELD_ENUM(SCL_0_DERINGING, VERT_LUMA_DERINGING, ON)
				: BCHP_FIELD_ENUM(SCL_0_DERINGING, VERT_LUMA_DERINGING, OFF)   ) |
				((hScaler->stSclSettings.bSclHorzChromaDeringing)
				? BCHP_FIELD_ENUM(SCL_0_DERINGING, HORIZ_CHROMA_DERINGING, ON)
				: BCHP_FIELD_ENUM(SCL_0_DERINGING, HORIZ_CHROMA_DERINGING, OFF)) |
				((hScaler->stSclSettings.bSclHorzLumaDeringing)
				? BCHP_FIELD_ENUM(SCL_0_DERINGING, HORIZ_LUMA_DERINGING, ON)
				: BCHP_FIELD_ENUM(SCL_0_DERINGING, HORIZ_LUMA_DERINGING, OFF)) );

			BVDC_P_SCL_GET_REG_DATA(SCL_0_DERING_DEMO_SETTING) =  (
				((BVDC_SplitScreenMode_eLeft == hScaler->hWindow->stCurInfo.stSplitScreenSetting.eDeRinging)
				? BCHP_FIELD_ENUM(SCL_0_DERING_DEMO_SETTING, DEMO_L_R, LEFT )
				: BCHP_FIELD_ENUM(SCL_0_DERING_DEMO_SETTING, DEMO_L_R, RIGHT)  )|
				BCHP_FIELD_DATA(SCL_0_DERING_DEMO_SETTING, DEMO_BOUNDARY, ulDstHSize/2));
		}
#endif

		/* -----------------------------------------------------------------------
		 * 6). set init phase for both horizontal and vertical
		 */
		/* Compute the phase accumulate intial value in S11.6 or S5.26 */
		lHrzPhsAccInit = BVDC_P_FIXED_A_MINUS_FIXED_B(
			BVDC_P_H_INIT_PHASE_RATIO_ADJ(ulFirHrzStepInit) / 2,
			BVDC_P_H_INIT_PHASE_0_POINT_5);

		BVDC_P_SCL_GET_REG_DATA(SCL_0_HORIZ_FIR_LUMA_SRC_PIC_OFFSET) &= ~(
			BCHP_MASK(SCL_0_HORIZ_FIR_LUMA_SRC_PIC_OFFSET, VALUE));
		BVDC_P_SCL_GET_REG_DATA(SCL_0_HORIZ_FIR_CHROMA_SRC_PIC_OFFSET) &= ~(
			BCHP_MASK(SCL_0_HORIZ_FIR_CHROMA_SRC_PIC_OFFSET, VALUE));
		BVDC_P_SCL_GET_REG_DATA(SCL_0_HORIZ_FIR_LUMA_SRC_PIC_OFFSET) |=  (
			BCHP_FIELD_DATA(SCL_0_HORIZ_FIR_LUMA_SRC_PIC_OFFSET, VALUE,
			ulPanScanLeft));
		BVDC_P_SCL_GET_REG_DATA(SCL_0_HORIZ_FIR_CHROMA_SRC_PIC_OFFSET) |=  (
			BCHP_FIELD_DATA(SCL_0_HORIZ_FIR_CHROMA_SRC_PIC_OFFSET, VALUE,
			ulPanScanLeft));

		BVDC_P_SCL_GET_REG_DATA(SCL_0_HORIZ_FIR_INIT_PHASE_ACC) &= ~(
			BCHP_MASK(SCL_0_HORIZ_FIR_INIT_PHASE_ACC, SIZE));
		BVDC_P_SCL_GET_REG_DATA(SCL_0_HORIZ_FIR_INIT_PHASE_ACC) |=  (
			BCHP_FIELD_DATA(SCL_0_HORIZ_FIR_INIT_PHASE_ACC, SIZE,
			lHrzPhsAccInit));

		/* Compute Vertical FIR initial phase.
		 * Do we want scaler to invert the field?  To invert field we will
		 * need to change the vertical initial phase a according to the given
		 * formula for doing TF2BF, and BF2TF.
		 * NOTE:
		 *   - this formula is based on field middle line alignment; */
		ulVsr = BVDC_P_V_INIT_PHASE_RATIO_ADJ(ulFirVrtStep);

		/* to FRM */
		hScaler->aaulInitPhase[BAVC_Polarity_eFrame   ][BAVC_Polarity_eFrame   ] =
			BVDC_P_FIXED_A_MINUS_FIXED_B(ulVsr / 2, BVDC_P_V_INIT_PHASE_1_POINT_0 / 2) + ulPanScanTop;

		hScaler->aaulInitPhase[BAVC_Polarity_eTopField][BAVC_Polarity_eFrame   ] =
			BVDC_P_FIXED_A_MINUS_FIXED_B(ulVsr / 2, BVDC_P_V_INIT_PHASE_1_POINT_0 / 4) + ulPanScanTop;

		hScaler->aaulInitPhase[BAVC_Polarity_eBotField][BAVC_Polarity_eFrame   ] =
			BVDC_P_FIXED_A_MINUS_FIXED_B(ulVsr / 2, BVDC_P_V_INIT_PHASE_1_POINT_0 * 3 / 4) + ulPanScanTop;

		/* to TF */
		hScaler->aaulInitPhase[BAVC_Polarity_eFrame   ][BAVC_Polarity_eTopField] =
			BVDC_P_FIXED_A_MINUS_FIXED_B(ulVsr / 4, BVDC_P_V_INIT_PHASE_1_POINT_0 / 2) + ulPanScanTop;

		hScaler->aaulInitPhase[BAVC_Polarity_eTopField][BAVC_Polarity_eTopField] =
			BVDC_P_FIXED_A_MINUS_FIXED_B(ulVsr / 4, BVDC_P_V_INIT_PHASE_1_POINT_0 / 4) + ulPanScanTop;

		hScaler->aaulInitPhase[BAVC_Polarity_eBotField][BAVC_Polarity_eTopField] =
			BVDC_P_FIXED_A_MINUS_FIXED_B(ulVsr / 4, BVDC_P_V_INIT_PHASE_1_POINT_0 * 3 / 4) + ulPanScanTop;

		/* to BF */
		hScaler->aaulInitPhase[BAVC_Polarity_eFrame   ][BAVC_Polarity_eBotField] =
			BVDC_P_FIXED_A_MINUS_FIXED_B(ulVsr * 3 / 4, BVDC_P_V_INIT_PHASE_1_POINT_0 / 2) + ulPanScanTop;

		hScaler->aaulInitPhase[BAVC_Polarity_eTopField][BAVC_Polarity_eBotField] =
			BVDC_P_FIXED_A_MINUS_FIXED_B(ulVsr * 3 / 4, BVDC_P_V_INIT_PHASE_1_POINT_0 / 4) + ulPanScanTop;

		hScaler->aaulInitPhase[BAVC_Polarity_eBotField][BAVC_Polarity_eBotField] =
			BVDC_P_FIXED_A_MINUS_FIXED_B(ulVsr * 3 / 4, BVDC_P_V_INIT_PHASE_1_POINT_0 * 3 / 4) + ulPanScanTop;

		/*
		 * Shift vertical init phase by 1/4 line when:
		 * 1) it is a 50Hz to 60Hz conversion and
		 * 2) input and output are both interlaced without MAD and
		 * 3) vertical scaling src_step is integer number
		 * note: interger ratio scale up might need similar treat to avoid periodic
		 * integer initial phase, but interger ratio scale up is unlikely practically
		 */
		if ((hScaler->bHandleFldInv) &&
			(0 == (ulFirVrtStep & (BVDC_P_SCL_V_RATIO_ONE - 1))))
		{
			BDBG_MSG(("shift phase -0.25"));
			hScaler->aaulInitPhase[BAVC_Polarity_eTopField][BAVC_Polarity_eTopField] -= BVDC_P_V_INIT_PHASE_0_POINT_25;
			hScaler->aaulInitPhase[BAVC_Polarity_eTopField][BAVC_Polarity_eBotField] -= BVDC_P_V_INIT_PHASE_0_POINT_25;
			hScaler->aaulInitPhase[BAVC_Polarity_eBotField][BAVC_Polarity_eTopField] -= BVDC_P_V_INIT_PHASE_0_POINT_25;
			hScaler->aaulInitPhase[BAVC_Polarity_eBotField][BAVC_Polarity_eBotField] -= BVDC_P_V_INIT_PHASE_0_POINT_25;
		}
	}

	/* Vertical intial phase */
	ulVertInitPhase = hScaler->hWindow->stCurInfo.stSclSettings.bSclVertPhaseIgnore? 0 :
		hScaler->aaulInitPhase[pPicture->eSrcPolarity][pPicture->eDstPolarity];
#if (BVDC_P_SUPPORT_SCL_VER <= BVDC_P_SUPPORT_SCL_VER_6)
	BVDC_P_SCL_GET_REG_DATA(SCL_0_VERT_FIR_SRC_PIC_OFFSET) &= ~(
		BCHP_MASK(SCL_0_VERT_FIR_SRC_PIC_OFFSET, VALUE));
	BVDC_P_SCL_GET_REG_DATA(SCL_0_VERT_FIR_SRC_PIC_OFFSET) |=  (
		BCHP_FIELD_DATA(SCL_0_VERT_FIR_SRC_PIC_OFFSET, VALUE, ulVertInitPhase));
#else
	/* integer part of the offset */
	BVDC_P_SCL_GET_REG_DATA(SCL_0_VERT_FIR_SRC_PIC_OFFSET_INT) &= ~(
		BCHP_MASK(SCL_0_VERT_FIR_SRC_PIC_OFFSET_INT, VALUE));
	BVDC_P_SCL_GET_REG_DATA(SCL_0_VERT_FIR_SRC_PIC_OFFSET_INT) |=  (
		BCHP_FIELD_DATA(SCL_0_VERT_FIR_SRC_PIC_OFFSET_INT, VALUE,
		(ulVertInitPhase >> BVDC_P_SCL_V_INIT_PHASE_F_BITS)));

	/* fractional part;
	   TODO: improve fixed point math to support higher precision offset */
	BVDC_P_SCL_GET_REG_DATA(SCL_0_VERT_FIR_SRC_PIC_OFFSET_FRAC) &= ~(
		BCHP_MASK(SCL_0_VERT_FIR_SRC_PIC_OFFSET_FRAC, VALUE));
	BVDC_P_SCL_GET_REG_DATA(SCL_0_VERT_FIR_SRC_PIC_OFFSET_FRAC) |=  (
		BCHP_FIELD_DATA(SCL_0_VERT_FIR_SRC_PIC_OFFSET_FRAC, VALUE,
		(ulVertInitPhase << BVDC_P_SCL_V_RATIO_F_BITS_EXT)));
#endif

	/* set vertical coeff change */
	if ((0 == (ulVertInitPhase & (BVDC_P_V_INIT_PHASE_1_POINT_0 - 1))) &&
	    ((hScaler->pPrevVertFirCoeff != hScaler->pVertFirCoeff) ||
	     (hScaler->pPrevChromaVertFirCoeff != hScaler->pChromaVertFirCoeff)))
	{
		/* init phase does not have fraction, use regular coeff */
		hScaler->ulUpdateCoeff = BVDC_P_RUL_UPDATE_THRESHOLD;
		hScaler->pPrevVertFirCoeff = hScaler->pVertFirCoeff;
		hScaler->pPrevChromaVertFirCoeff = hScaler->pChromaVertFirCoeff;
		BVDC_P_Scaler_SetVertFirCoeff(hScaler, hScaler->pVertFirCoeff->pulCoeffs);
		BVDC_P_Scaler_SetChromaVertFirCoeff(hScaler, hScaler->pChromaVertFirCoeff->pulCoeffs);
		BDBG_MSG(("Luma V Coeffs  : %s", hScaler->pVertFirCoeff->pchName));
		BDBG_MSG(("Chroma V Coeffs: %s", hScaler->pChromaVertFirCoeff->pchName));
	}
	else if ((0 != (ulVertInitPhase & (BVDC_P_V_INIT_PHASE_1_POINT_0 - 1))) &&
	         ((hScaler->pPrevVertFirCoeff != hScaler->pFracInitPhaseVertFirCoeff) ||
	          (hScaler->pPrevChromaVertFirCoeff != hScaler->pChromaFracInitPhaseVertFirCoeff)))
	{
		/* init phase has fraction, use special coeff for init-phase with fraction
		 * to avoid point sampling */
		hScaler->ulUpdateCoeff = BVDC_P_RUL_UPDATE_THRESHOLD;
		hScaler->pPrevVertFirCoeff = hScaler->pFracInitPhaseVertFirCoeff;
		hScaler->pPrevChromaVertFirCoeff = hScaler->pChromaFracInitPhaseVertFirCoeff;
		BVDC_P_Scaler_SetVertFirCoeff(hScaler, hScaler->pFracInitPhaseVertFirCoeff->pulCoeffs);
		BVDC_P_Scaler_SetChromaVertFirCoeff(hScaler, hScaler->pChromaFracInitPhaseVertFirCoeff->pulCoeffs);
		BDBG_MSG(("Luma V Coeffs  : %s", hScaler->pFracInitPhaseVertFirCoeff->pchName));
		BDBG_MSG(("Chroma V Coeffs: %s", hScaler->pChromaFracInitPhaseVertFirCoeff->pchName));
	}

	BDBG_ASSERT((hScaler->pPrevVertFirCoeff == hScaler->pVertFirCoeff) ||
	            (hScaler->pPrevVertFirCoeff == hScaler->pFracInitPhaseVertFirCoeff));

	BDBG_ASSERT(hScaler->pPrevVertFirCoeff);

#if (BVDC_P_SUPPORT_SCL_VER == BVDC_P_SUPPORT_SCL_VER_5)
	if(hScaler->hWindow->stCurInfo.hSource->bSrcIs444)
	{
		if(BVDC_P_VNET_USED_SCALER_AT_READER(hScaler->hWindow->stVnetMode))
		{
			/* SRC -> CAP -> VFD -> SCL */
			hScaler->stDnSampler.eFilterType = BVDC_444To422Filter_eDecimate;
		}
		else
		{
			/* SRC -> ANR (?) -> MAD (?) -> SCL -> CAP */
			if(BVDC_P_VNET_USED_ANR(hScaler->hWindow->stVnetMode) ||
			   BVDC_P_VNET_USED_MAD(hScaler->hWindow->stVnetMode))
			{
				hScaler->stDnSampler.eFilterType = BVDC_444To422Filter_eDecimate;
			}
			else
			{
				hScaler->stDnSampler.eFilterType = BVDC_444To422Filter_eStandard;
			}
		}
	}
	else
	{
		hScaler->stDnSampler.eFilterType = BVDC_444To422Filter_eDecimate;
	}

	if((hScaler->stUpSampler.eFilterType == BVDC_422To444Filter_eTenTaps) ||
	   (hScaler->stUpSampler.eFilterType == BVDC_422To444Filter_eSixTaps))
	{
		BVDC_P_SCL_GET_REG_DATA(SCL_0_US_422_TO_444_CONV) = (
			BCHP_FIELD_ENUM(SCL_0_US_422_TO_444_CONV, RING_SUPPION_MODE, DOUBLE) |
			BCHP_FIELD_ENUM(SCL_0_US_422_TO_444_CONV, RING_SUPPION,      ENABLE) |
			BCHP_FIELD_DATA(SCL_0_US_422_TO_444_CONV, UNBIASED_ROUND_ENABLE,
				hScaler->stUpSampler.bUnbiasedRound) |
			BCHP_FIELD_DATA(SCL_0_US_422_TO_444_CONV, FILT_CTRL, hScaler->stUpSampler.eFilterType));
	}
	else
	{
		BVDC_P_SCL_GET_REG_DATA(SCL_0_US_422_TO_444_CONV) = (
			BCHP_FIELD_ENUM(SCL_0_US_422_TO_444_CONV, RING_SUPPION_MODE, NORMAL)  |
			BCHP_FIELD_ENUM(SCL_0_US_422_TO_444_CONV, RING_SUPPION,      DISABLE) |
			BCHP_FIELD_DATA(SCL_0_US_422_TO_444_CONV, UNBIASED_ROUND_ENABLE,
				hScaler->stUpSampler.bUnbiasedRound) |
			BCHP_FIELD_DATA(SCL_0_US_422_TO_444_CONV, FILT_CTRL, hScaler->stUpSampler.eFilterType));
	}

	BVDC_P_SCL_GET_REG_DATA(SCL_0_DS_CONFIGURATION) =  (
		BCHP_FIELD_DATA(SCL_0_DS_CONFIGURATION, FILTER_TYPE,
			hScaler->stDnSampler.eFilterType) |
		BCHP_FIELD_DATA(SCL_0_DS_CONFIGURATION, RING_SUPPRESSION,
			hScaler->stDnSampler.eRingRemoval));
#endif

#if (BVDC_P_SUPPORT_SCL_VER >= BVDC_P_SUPPORT_SCL_VER_3 && \
	 BVDC_P_SUPPORT_SCL_VER <= BVDC_P_SUPPORT_SCL_VER_8)
#if (BVDC_P_SUPPORT_VIDEO_TESTFEATURE1_VER)
	{
		uint32_t ulHorzDitherMode = hScaler->stHorzDither.ulMode;
		uint32_t ulVertDitherMode = hScaler->stVertDither.ulMode;

		/* Dither off when:
		 * 1) TestFeature1 is enabled on CAP for SCL->CAP */
		if((BVDC_P_VNET_USED_SCALER_AT_WRITER(hScaler->hWindow->stVnetMode) &&
			hScaler->hWindow->stCurInfo.stCapTestFeature1.bEnable))
		{
			ulHorzDitherMode = 0;
			ulVertDitherMode = 0;
		}

		BVDC_P_SCL_GET_REG_DATA(SCL_0_HORIZ_DITHER_CTRL) &= ~(
			BCHP_MASK(SCL_0_HORIZ_DITHER_CTRL, MODE));
		BVDC_P_SCL_GET_REG_DATA(SCL_0_HORIZ_DITHER_CTRL) |=  (
			BCHP_FIELD_DATA(SCL_0_HORIZ_DITHER_CTRL, MODE, ulHorzDitherMode));

		BVDC_P_SCL_GET_REG_DATA(SCL_0_VERT_DITHER_CTRL) &= ~(
			BCHP_MASK(SCL_0_VERT_DITHER_CTRL, MODE));
		BVDC_P_SCL_GET_REG_DATA(SCL_0_VERT_DITHER_CTRL) |=  (
			BCHP_FIELD_DATA(SCL_0_VERT_DITHER_CTRL, MODE, ulVertDitherMode));
	}
#endif
#endif

	/* 3D support */
#if (BVDC_P_SUPPORT_SCL_VER >= BVDC_P_SUPPORT_SCL_VER_7)
	BVDC_P_SCL_GET_REG_DATA(SCL_0_VIDEO_3D_MODE) &= ~(
		BCHP_MASK(SCL_0_VIDEO_3D_MODE, BVB_VIDEO));
	BVDC_P_SCL_GET_REG_DATA(SCL_0_VIDEO_3D_MODE) |=  (
		BVDC_P_VNET_USED_SCALER_AT_WRITER(pPicture->stVnetMode) ?
		BCHP_FIELD_DATA(SCL_0_VIDEO_3D_MODE, BVB_VIDEO, pPicture->eSrcOrientation) :
		BCHP_FIELD_DATA(SCL_0_VIDEO_3D_MODE, BVB_VIDEO, pPicture->eDispOrientation));
#endif

	/* Printing out ratio in float format would be nice, but PI
	* code does permit float. */
	if(BVDC_P_RUL_UPDATE_THRESHOLD == hScaler->ulUpdateAll)
	{
		BDBG_MSG(("-------------------------"));
		BDBG_MSG(("Scaler[%d]         : %dx%d to %dx%d", hScaler->eId,
			pSclIn->ulWidth,  pSclIn->ulHeight,
			pSclOut->ulWidth, pSclOut->ulHeight));
		BDBG_MSG(("Scaler[%d]'clip    : %dx%d to %dx%d", hScaler->eId,
			pSclCut->ulWidth,  pSclCut->ulHeight,
			pSclOut->ulWidth, pSclOut->ulHeight));
		BDBG_MSG(("ulHrzStep         : %-8lx", ulHrzStep));
		BDBG_MSG(("ulVrtStep         : %-8lx", ulVrtStep));
		BDBG_MSG(("ulFirHrzStep      : %-8lx", ulFirHrzStep));
		BDBG_MSG(("ulFirVrtStep      : %-8lx", ulFirVrtStep));
#if (BVDC_P_SUPPORT_SCL_VER < BVDC_P_SUPPORT_SCL_VER_4)
		BDBG_MSG(("Vertical Mode     : %s",
			(BVDC_P_SCL_COMPARE_FIELD_NAME(SCL_0_VERT_CONTROL, MODE, BYPASS)) ? "Bypass" :
			(BVDC_P_SCL_COMPARE_FIELD_NAME(SCL_0_VERT_CONTROL, MODE, FIR2  )) ? "FIR2" :
			(BVDC_P_SCL_COMPARE_FIELD_NAME(SCL_0_VERT_CONTROL, MODE, FIR4  )) ? "FIR4" :
			(BVDC_P_SCL_COMPARE_FIELD_NAME(SCL_0_VERT_CONTROL, MODE, AV2   )) ? "AV2" :
			(BVDC_P_SCL_COMPARE_FIELD_NAME(SCL_0_VERT_CONTROL, MODE, AV4   )) ? "AV4" : "Unknown"));
#endif
		BDBG_MSG(("ulBlkAvgSize      : %d", ulBlkAvgSize));
		BDBG_MSG(("ulVertSclSrcWidth : %d", ulVertSclSrcWidth));
		BDBG_MSG(("HWF_0             : %s",
			BVDC_P_SCL_COMPARE_FIELD_NAME(SCL_0_HORIZ_CONTROL, HWF0_ENABLE, ON) ? "On" : "Off"));
		BDBG_MSG(("HWF_1             : %s",
			BVDC_P_SCL_COMPARE_FIELD_NAME(SCL_0_HORIZ_CONTROL, HWF1_ENABLE, ON) ? "On" : "Off"));
		BDBG_MSG(("H_PanScan         : %-8lx", pSclCut->lLeft));
		BDBG_MSG(("V_PanScan         : %-8lx", pSclCut->lTop));
		BDBG_MSG(("H_InitPhase       : %-8lx", lHrzPhsAccInit));
		BDBG_MSG(("V_InitPhase       : %-8lx", ulVertInitPhase));
		BDBG_MSG(("SrcPolarity       : %d", pPicture->eSrcPolarity));
		BDBG_MSG(("DstPolarity       : %d", pPicture->eDstPolarity));
	}

	BDBG_LEAVE(BVDC_P_Scaler_SetInfo_isr);
	return;
}

/***************************************************************************
 * {private}
 *
 */
void BVDC_P_Scaler_SetEnable_isr
	( BVDC_P_Scaler_Handle             hScaler,
	  bool                             bEnable )
{
	BDBG_OBJECT_ASSERT(hScaler, BVDC_SCL);

#if (BVDC_P_SUPPORT_SCL_VER > BVDC_P_SUPPORT_SCL_VER_1)
	if(!BVDC_P_SCL_COMPARE_FIELD_DATA(SCL_0_ENABLE, SCALER_ENABLE, bEnable))
#else
	if(!BVDC_P_SCL_COMPARE_FIELD_DATA(SCL_0_TOP_CONTROL, SCALER_ENABLE, bEnable))
#endif
	{
		hScaler->ulUpdateAll = BVDC_P_RUL_UPDATE_THRESHOLD;
	}

	/* Turn on/off the scaler. */
#if (BVDC_P_SUPPORT_SCL_VER > BVDC_P_SUPPORT_SCL_VER_1)
	BVDC_P_SCL_GET_REG_DATA(SCL_0_ENABLE) &= ~(
		BCHP_MASK(SCL_0_ENABLE, SCALER_ENABLE));

	BVDC_P_SCL_GET_REG_DATA(SCL_0_ENABLE) |=  (bEnable
		? BCHP_FIELD_ENUM(SCL_0_ENABLE, SCALER_ENABLE, ON)
		: BCHP_FIELD_ENUM(SCL_0_ENABLE, SCALER_ENABLE, OFF));
#else
	BVDC_P_SCL_GET_REG_DATA(SCL_0_TOP_CONTROL) &= ~(
		BCHP_MASK(SCL_0_TOP_CONTROL, SCALER_ENABLE));

	BVDC_P_SCL_GET_REG_DATA(SCL_0_TOP_CONTROL) |=  (bEnable
		? BCHP_FIELD_ENUM(SCL_0_TOP_CONTROL, SCALER_ENABLE, ON)
		: BCHP_FIELD_ENUM(SCL_0_TOP_CONTROL, SCALER_ENABLE, OFF));
#endif

	return;
}


/***************************************************************************
* {private}
*
*/
void BVDC_P_Scaler_5ZoneNonLinear
	( BVDC_P_Scaler_Handle             hScaler,
	  const BVDC_P_PictureNodePtr      pPicture,
	  uint32_t                         *pFirHrzStep,
	  uint32_t                         *pHrzStep,
	  uint32_t                         *pFirHrzStepInit)
{
	/* 5-region smooth non-liear horizontal scaling:
	*  0. we handle both aspect ratio expand and shrink, also between 480i 4:3 and 16:9
	*  1. horizontal divide 5-region in DEST domain;
	*  2. central region might not be aspect ratio correct;
	*  3. scale factor is constant in central region, then linearly changes to edge
	*  4. scaling coeffs are selected by central region;
	*  5. no extreme horizontal scaledown (h_ratio < 8) to avoid hw filtering;
	*  6. no scale-factor rounding;
	*  7. no overall aspect ratio correction or source cliping;
	*  8. no vbi pass-through;
	*/
	uint32_t ulDstWidthRgn02, ulSrcWidthRgn02;
	uint32_t ulDstWidthRgn1 , ulSrcWidthRgn1;
	uint32_t ulDstWidthRgn3, ulSrcWidthRgn3;
	uint32_t ulSrcWidth, ulSclSrcWidth, ulDstWidth;
	uint32_t ulNumer, ulDenom;
	uint32_t ulNrmStep1;
	uint32_t ulDlt, ulDltInt, ulDltFrac, ulDltRem;
	uint32_t ulStep1, ulStepInit, ulStepRem, ulStepFrac, ulStep;
	uint32_t r1;
	bool bDltNeg, bNegUnderflow;

	/* 1) region sizes: dst region ends must be even number according to HW */
	ulSrcWidth      = pPicture->pSclIn->ulWidth;
	ulDstWidth      = pPicture->pSclOut->ulWidth;
	ulSrcWidthRgn02 = pPicture->ulNonlinearSrcWidth;
	ulDstWidthRgn02 = pPicture->ulNonlinearSclOutWidth;
	ulSrcWidthRgn1  = ulSrcWidth - (ulSrcWidthRgn02 << 1);
	ulDstWidthRgn1  = pPicture->ulCentralRegionSclOutWidth;
#if (BVDC_P_SUPPORT_SCL_VER<BVDC_P_SUPPORT_SCL_VER_7)
	ulDstWidthRgn3  = 0;
	ulSrcWidthRgn3  = 0;
#else
#define PIXELMASK                        (0xFFFFFFFE)
	ulDstWidthRgn3  = (ulDstWidthRgn02>>2) & PIXELMASK ;
	ulSrcWidthRgn3  = (ulSrcWidthRgn02>>2) & PIXELMASK;
	ulDstWidthRgn02 = ulDstWidthRgn02 - ulDstWidthRgn3;
	ulSrcWidthRgn02 = ulSrcWidthRgn02 - ulSrcWidthRgn3;
#endif
	BDBG_MSG(("Src Width %4d 0: %4d 1: %4d 3 %4d", ulSrcWidth, ulSrcWidthRgn02, ulSrcWidthRgn1, ulSrcWidthRgn3));
	BDBG_MSG(("Des Width %4d 0: %4d 1: %4d 3 %4d", ulDstWidth, ulDstWidthRgn02, ulDstWidthRgn1, ulDstWidthRgn3));

	ulNrmStep1     = pPicture->ulNrmHrzSrcStep;

	/*@@@ Any HW limitation to be validated here */
	if (ulDstWidthRgn02 < 3)
		{
			ulDstWidthRgn1 =
				BVDC_P_MAX((3 - ulDstWidthRgn02), ulDstWidthRgn1) - (3 - ulDstWidthRgn02);
			ulDstWidthRgn02 = 3;
		}

	/* 2). calculate the delta: src_step's step increase/decrease in region 0 and 2.
	*
	* Lets use notation as the following
	* sw:		src width
	* dw:		scaler output width
	* sw n1/0/1/2/3:	src width of region n1 0, 1, 2 3
	* dw n1/0/1/2/3:	scaler output width of region n1, 0, 1, 2, 3
	* r1:		U5.26, scale ratio (really the src step) in region 1, i.e. ulNrmHrzStep
	* ri:		U5.26  scale ration in region n1 and 3.
	* dlt:		S1.26, delta, src_step's 2-step inc/dec at even pixels in region 0 and 2
	*
	* Since step increases on even pixel only in HW, we assume dw0 is even (enforced by win code),
	*	ri + dlt * (dw0/2) = r1
	* and
	*	ri * ulDstWidth + (r1 - ri) * (ulDstWidthRg1 + ulDstWidthRgn02)
	*	= dw*AvgScaleFactor = sw.
	* i.e
	*	 		(sw - r1*(ulDstWidthRgn02 + ulDstWidthRgn1))
	*	ri = --------------------------------------------------
	*			2*ulDstWidthRgn3 + ulDstWidthRgn02
	*
	* Therefore
	*		   2*(r1* ulDstWidth - sw)
	*	dlt =  ---------------------------------------------------------------------
	*			  ulDstWidthRgn02 * (2*ulDstWidthRgn3 + ulDstWidthRgn02)
	*
	* Note that dlt could be negative, but we must have ri >= 0. This leads some user's
	* set to non-linear src/sclOut region invalid.
	*/
	ulSrcWidth    = (ulSrcWidth     << BVDC_P_NRM_SRC_STEP_F_BITS);
	r1            = (ulSrcWidthRgn1 << BVDC_P_NRM_SRC_STEP_F_BITS)/ulDstWidthRgn1;

	/*scale ratio ri of region 0 and 2*/
	ulSclSrcWidth = r1 * (ulDstWidthRgn1 + ulDstWidthRgn02);
	ulDenom       = 2* ulDstWidthRgn3 + ulDstWidthRgn02;
	ulNumer       = ulSrcWidth > ulSclSrcWidth ?
		(ulSrcWidth - ulSclSrcWidth):
		(ulSclSrcWidth - ulSrcWidth);

	ulStepInit    = ulNumer /ulDenom;
	ulStepRem     = ulNumer - ulStepInit*ulDenom;
	ulStepFrac    = (ulStepInit &((1 << BVDC_P_NRM_SRC_STEP_F_BITS) - 1))
		<<(BVDC_P_SCL_H_R_DLT_F_BITS - BVDC_P_NRM_SRC_STEP_F_BITS);
	ulStepInit    = ulStepInit >> BVDC_P_NRM_SRC_STEP_F_BITS;
	ulStepFrac    = ulStepFrac |((ulStepRem << (BVDC_P_SCL_H_R_DLT_F_BITS - BVDC_P_NRM_SRC_STEP_F_BITS))/ulDenom);
	ulStep        = (ulStepInit << BVDC_P_SCL_H_R_DLT_F_BITS) | ulStepFrac;

	/*delta calculation*/
	ulSclSrcWidth = ulDstWidth * r1;
	ulDenom       = (ulDstWidthRgn02*(2*ulDstWidthRgn3 + ulDstWidthRgn02));

	bDltNeg       = ulSclSrcWidth < ulSrcWidth;

	ulNumer       = bDltNeg? (ulSrcWidth - ulSclSrcWidth): (ulSclSrcWidth - ulSrcWidth);
	ulNumer       = (ulNumer <<1);
	ulDltInt      = ulNumer/ulDenom;            /*Int*2^20 to avoid accuracy loss*/
	ulDltRem      = ulNumer - ulDltInt*ulDenom;
	ulDltFrac     = ulDltInt & ((1 << BVDC_P_NRM_SRC_STEP_F_BITS) - 1);
	ulDltInt      = ulDltInt >> BVDC_P_NRM_SRC_STEP_F_BITS;
	ulDltRem      = ulDltRem << (BVDC_P_SCL_H_R_DLT_F_BITS - BVDC_P_NRM_SRC_STEP_F_BITS);
	ulDltFrac     = (ulDltRem / ulDenom)|(ulDltFrac <<(BVDC_P_SCL_H_R_DLT_F_BITS - BVDC_P_NRM_SRC_STEP_F_BITS));
	ulDlt         = (ulDltInt << (BVDC_P_SCL_H_R_DLT_F_BITS + BCHP_SCL_0_HORIZ_FIR_DEST_PIC_REGION_0_STEP_DELTA_SIZE_SHIFT))
		+ ulDltFrac;
	/* Underflow control */
	ulStep1 = pPicture->ulNrmHrzSrcStep << (BVDC_P_SCL_H_RATIO_F_BITS - BVDC_P_NRM_SRC_STEP_F_BITS);
	r1 <<= (BVDC_P_SCL_H_R_DLT_F_BITS - BVDC_P_NRM_SRC_STEP_F_BITS);
	bNegUnderflow = (!bDltNeg) && (r1 <(ulDlt*ulDstWidthRgn02/2));
	BDBG_MSG(("Underflow %1d Step1 %8x Thd %8x %8x %8x", bNegUnderflow, ulStep1, ulDstWidthRgn02, (ulDlt*ulDstWidthRgn02/2)));

	if(bNegUnderflow )
	{
		ulDlt  = 0;
		ulStep = ulSrcWidth /ulDstWidth;
		ulStep <<= (BVDC_P_SCL_H_R_DLT_F_BITS - BVDC_P_NRM_SRC_STEP_F_BITS);
		ulStep1 = ulStep;
		BDBG_ERR(("Need to set non-linear area bigger to avoid underflow %8d %d Step1 %8d ",
			(ulSrcWidth>>BVDC_P_NRM_SRC_STEP_F_BITS), ulDstWidth, (ulStep1>>BVDC_P_SCL_H_RATIO_F_BITS) ));
	}

	BDBG_MSG(("r1  %8x Step: %8x Int: %8x Frac %8x", r1, ulStep, ulStepInit, ulStepFrac));
	BDBG_MSG(("Delta: %8x Int: %8x Frac %8x bDltNeg %1d", ulDlt, ulDltInt, ulDltFrac, bDltNeg));

	/*set register*/
	BVDC_P_SCL_SET_HORZ_STEP_MISC(ulDstWidthRgn02 + ulDstWidthRgn1 + ulDstWidthRgn3, ulStep);

#if (BVDC_P_SUPPORT_SCL_VER>=BVDC_P_SUPPORT_SCL_VER_7)
	BVDC_P_SCL_SET_HORZ_REGION3N1(N1,  ulDstWidthRgn3);
	BVDC_P_SCL_SET_HORZ_REGION3N1(3,   ulDstWidth);
#endif

	if(!bDltNeg)
	{
		BVDC_P_SCL_SET_HORZ_REGION02 (0,   ulDstWidthRgn3 + ulDstWidthRgn02,	                 ulDlt);
		BVDC_P_SCL_SET_HORZ_REGION02 (2,   ulDstWidthRgn3 + ulDstWidthRgn1 + 2*ulDstWidthRgn02, -ulDlt);
	}
	else
	{
		BVDC_P_SCL_SET_HORZ_REGION02 (0,   ulDstWidthRgn3 + ulDstWidthRgn02,	                -ulDlt);
		BVDC_P_SCL_SET_HORZ_REGION02 (2,   ulDstWidthRgn3 + ulDstWidthRgn1 + 2*ulDstWidthRgn02,  ulDlt);
	}

	/* used outside */
	*pFirHrzStep = *pHrzStep = ulStep1;
	*pFirHrzStepInit = (ulStepInit << BVDC_P_SCL_H_R_DLT_F_BITS) | ulStepFrac;
}

/***************************************************************************
* {private}
*
* This funtion decide whether to Overwrite Src orientation if input
* buffer length surpass the vertical scaler line buffer depth
*/

bool BVDC_P_Scaler_Validate_VertDepth_isr
	(BVDC_Window_Handle                 hWindow,
	 const BVDC_P_Scaler_Handle         hScaler)
{
#if BVDC_P_SUPPORT_3D_VIDEO
	BFMT_Orientation                   eSrcOrientation;
	BFMT_Orientation                   eDstOrientation;
	BFMT_Orientation                   eOrientation;
	bool                               bOrientationLR;
	uint32_t                           ulSclVertDepth;
	uint32_t                           ulSclSrcWidth, ulSclDstWidth;
	BVDC_Compositor_Handle             hCompositor;
	BVDC_P_PictureNode                *pPicture;


	ulSclDstWidth = hWindow->stCurInfo.stScalerOutput.ulWidth;
	ulSclSrcWidth = hWindow->stSrcCnt.ulWidth;

	hCompositor = hWindow->hCompositor;
	pPicture = hWindow->pCurWriterNode;

	eSrcOrientation = pPicture->eSrcOrientation;
	eDstOrientation = pPicture->eDispOrientation;
	eOrientation =
		BVDC_P_VNET_USED_SCALER_AT_WRITER(hWindow->stVnetMode) ?
		eSrcOrientation : eDstOrientation;


	bOrientationLR = (eOrientation == BFMT_Orientation_e3D_LeftRight);
	ulSclVertDepth = hScaler->ulVertLineDepth;

	if(((ulSclSrcWidth << bOrientationLR) > ulSclVertDepth) &&
		((ulSclDstWidth<< bOrientationLR) > ulSclVertDepth))
	{
		BDBG_MSG(("Overwrite SrcOrientation 3D -> 2D: Scl [%d] line buffer length cannot be larger than %d ",
			hScaler->eId, hScaler->ulVertLineDepth, ulSclSrcWidth, ulSclDstWidth));
		return (false);
	}
#else
	BSTD_UNUSED(hWindow);
	BSTD_UNUSED(hScaler);
#endif
	return (true);
}

/* End of file. */
