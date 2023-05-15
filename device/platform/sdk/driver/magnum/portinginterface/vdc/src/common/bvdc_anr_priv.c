/***************************************************************************
 *     Copyright (c) 2004-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvdc_anr_priv.c $
 * $brcm_Revision: Hydra_Software_Devel/156 $
 * $brcm_Date: 7/17/12 2:03p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vdc/src/common/bvdc_anr_priv.c $
 * 
 * Hydra_Software_Devel/156   7/17/12 2:03p vanessah
 * SW7435-265: remove duplicate ANR register programming
 * 
 * Hydra_Software_Devel/155   12/1/11 4:30p yuxiaz
 * SW7425-968, SW7344-95: Merged into mainline.: added independent source
 * clipping of right window in 3D mode.
 * 
 * Hydra_Software_Devel/SW7425-968/1   11/3/11 2:28p yuxiaz
 * SW7425-968: Use BVDC_P_Rect in VDC.
 * 
 * Hydra_Software_Devel/154   6/20/11 5:48p tdo
 * SW7425-104: Rename the old repeat field flag to match avc structure and
 * use the new added bRepeatField flag for ANR.
 *
 * Hydra_Software_Devel/153   4/19/11 6:18p tdo
 * SW7425-104: Update table to increase ANR strength and make changes so
 * that both K1 and K2 used with slider changes (lo, med, hi)
 *
 * Hydra_Software_Devel/152   3/15/11 4:08p tdo
 * SW7425-104: Add logic to drop frame when repeat
 *
 * Hydra_Software_Devel/151   2/18/11 4:51p tdo
 * SW7425-104: Update 8-bit Noise threadhold tables
 *
 * Hydra_Software_Devel/150   2/18/11 11:25a tdo
 * SW7425-104: Enable HD_ANR_MCTF_0_TOP_CTRL.RANGE_CHK
 *
 * Hydra_Software_Devel/149   2/17/11 6:32p tdo
 * SW7425-104: Fix compiler warning.  Make use of repeat flag to disable
 * ANR cap for 7422/7425 onward.
 *
 * Hydra_Software_Devel/148   2/17/11 4:14p tdo
 * SW7425-104: Update the ANR software algorithm for optimal use of new
 * ANR hardware.  Add new agorithm.
 *
 * Hydra_Software_Devel/147   2/16/11 6:05p tdo
 * SW7425-104: Update the ANR software algorithm for optimal use of new
 * ANR hardware.  Refactor.
 *
 * Hydra_Software_Devel/146   1/12/11 4:33p yuxiaz
 * SW7335-1133: Added more debug messages for VDC heap debugging.
 *
 * Hydra_Software_Devel/145   1/7/11 4:45p yuxiaz
 * SW7422-174: Added DCX support to MCVP and MADR.
 *
 * Hydra_Software_Devel/144   1/4/11 4:06p yuxiaz
 * SW7422-174: Refactor DCX macros.
 *
 * Hydra_Software_Devel/143   12/29/10 3:47p pntruong
 * SW7422-174: Initial support for siob in madr.
 *
 * Hydra_Software_Devel/142   12/23/10 5:12p tdo
 * SW7422-148: Refactor MCVP to address chips w/wo MCTF
 *
 * Hydra_Software_Devel/141   12/8/10 5:43p tdo
 * SW7422-36: Correctly program MCVP 3D mode based on source and display
 * orientation
 *
 * Hydra_Software_Devel/140   12/3/10 4:42p tdo
 * SW7422-36: Handle "left only" method for BVN internal data path
 *
 * Hydra_Software_Devel/139   11/5/10 3:07p vanessah
 * SW7422-52:  Two kinds of deinterlacers in the same chip
 *
 * Hydra_Software_Devel/138   6/18/10 8:50p rpan
 * SW7400-2763: Fixed compile errors.
 *
 * Hydra_Software_Devel/137   6/18/10 4:18p rpan
 * SW7400-2763: New BVN resource management.
 *
 * Hydra_Software_Devel/136   5/7/10 7:07p albertl
 * SW7125-364: Changed dirty bits to use union structure to avoid type-pun
 * warnings.
 *
 * Hydra_Software_Devel/135   4/19/10 10:11p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic. Move
 * BDBG_OBJECT_ID_DECLARE private header files instead of .c.
 *
 * Hydra_Software_Devel/134   4/7/10 11:23a tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic.  Rename TLA
 *
 * Hydra_Software_Devel/133   4/5/10 3:57p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic
 *
 * Hydra_Software_Devel/132   2/5/10 5:03p tdo
 * SW7420-567: Use internal flag to check for HD/SD instead
 *
 * Hydra_Software_Devel/131   2/5/10 4:54p tdo
 * SW7420-567: Use proper macro to check for HD/SD
 *
 * Hydra_Software_Devel/130   2/5/10 1:52p tdo
 * SW7420-567: Update MCTF register settings
 *
 * Hydra_Software_Devel/129   12/18/09 9:18a pntruong
 * SW3556-918: Restricted to use pAnrIn when enabled or valid only.
 *
 * Hydra_Software_Devel/128   12/16/09 5:25p pntruong
 * SW3556-918: Chroma is shifted to up direction about one line.
 *
 * Hydra_Software_Devel/127   12/9/09 3:52p pntruong
 * SW3556-880: Need to re-evaluate maaf filter on input change that may
 * not trigger bvn reconfiguration.
 *
 * Hydra_Software_Devel/126   10/21/09 2:56p pntruong
 * SW3548-2499: Undo temporary work-around.
 *
 * Hydra_Software_Devel/125   10/20/09 4:42p hongtaoz
 * HW3548-47: disable MCTF trick mode support for now;
 *
 * Hydra_Software_Devel/124   10/12/09 10:51a syang
 * SW3548-2499: change 255*3/4 to 255*95/100 so that MCTF effect is turned
 * off faster when scene changed
 *
 * Hydra_Software_Devel/123   9/3/09 10:30p pntruong
 * SW3548-2373: Rolled back PR56964.
 *
 * Hydra_Software_Devel/122   7/21/09 2:17p syang
 * PR 56964: take off the "connect drainer, en and then dis" step  in
 * vnet-drain code
 *
 * Hydra_Software_Devel/121   7/20/09 4:38p syang
 * PR 56663: reset mad together with anr during vnet reconfig even if mad
 * is not in use
 *
 * Hydra_Software_Devel/120   6/4/09 1:59p yuxiaz
 * PR55330, PR55323, PR53197, PR55158, PR55604: Need to use separate CAP
 * and VFD buffers in ANR when DCX is enabled.
 *
 * Hydra_Software_Devel/119   5/15/09 3:59p syang
 * PR 55146: update ANR filtering tables, improve ANR algorithm  for
 * picture quality
 *
 * Hydra_Software_Devel/118   4/17/09 11:57a pntruong
 * PR54064: Refactored common code for defered callback til bvn shutdown.
 * Improved readability and extensibility of dirty bits.
 *
 * Hydra_Software_Devel/117   4/8/09 11:49a syang
 * PR 51795:  use macro BVDC_P_NOISE_LVL_AVG_SIZE for aulNoiseLevelHistory
 * init
 *
 * Hydra_Software_Devel/116   4/3/09 4:33p yuxiaz
 * PR53460: Optimal 444/422 conversion settings
 *
 * Hydra_Software_Devel/115   3/17/09 4:15p tdo
 * PR53338: Fix MCVP branch merge issue
 *
 * Hydra_Software_Devel/114   3/17/09 2:13a tdo
 * PR45785, PR45789: Fix compiling warnings
 *
 * Hydra_Software_Devel/113   3/17/09 2:02a tdo
 * PR45785, PR45789: Fix compiling errors
 *
 * Hydra_Software_Devel/112   3/17/09 12:57a tdo
 * PR45785, PR45789: Fix compiling errors
 *
 * Hydra_Software_Devel/111   3/16/09 10:43p tdo
 * PR45785, PR45789: Merge from MCVP branch
 *
 * Hydra_Software_Devel/7420_mcvp/10   3/10/09 10:02p tdo
 * PR45785, PR45789: 7420 ANR is 8-bit
 *
 * Hydra_Software_Devel/7420_mcvp/9   3/4/09 6:05p tdo
 * PR45785, PR45789: 7420 ANR is 8-bit
 *
 * Hydra_Software_Devel/7420_mcvp/8   3/2/09 4:21p tdo
 * PR 45785, PR 45789: merge from main branch on 3/02/09
 *
 * Hydra_Software_Devel/110   3/2/09 3:45p yuxiaz
 * PR52287: More update on 422-444 and 444-422 converters.
 *
 * Hydra_Software_Devel/7420_mcvp/7   2/26/09 11:17p tdo
 * PR 45785, PR 45789: merge from main branch on 2/26/09
 *
 * Hydra_Software_Devel/109   2/26/09 6:44p pntruong
 * PR52287: fixed 7405 build errors.
 *
 * Hydra_Software_Devel/108   2/26/09 6:09p yuxiaz
 * PR52287: 422-444 and 444-422 converters should use decimate/duplicate.
 *
 * Hydra_Software_Devel/7420_mcvp/6   2/24/09 2:01p syang
 * PR 51795:  merge from main branch on 2/24/09
 *
 * Hydra_Software_Devel/107   2/24/09 1:43p syang
 * PR 51795:  separate AlphLowThd value into AlphLowThdMC and
 * AlphLowThdNMC
 *
 * Hydra_Software_Devel/7420_mcvp/5   2/19/09 10:27a syang
 * PR 51795: merge from main branch
 *
 * Hydra_Software_Devel/106   2/19/09 10:19a syang
 * PR 51795:  make ANR take effect immediately after scene change
 *
 * Hydra_Software_Devel/7420_mcvp/4   2/18/09 7:03p syang
 * PR 45785, PR 45789: fix 3556 compile error
 *
 * Hydra_Software_Devel/7420_mcvp/3   2/11/09 11:30a tdo
 * PR 45785, PR 45789: merge from main branch on 2/11/09
 *
 * Hydra_Software_Devel/105   2/10/09 11:25a syang
 * PR 51795: use macro, rather than hard coded 16, for the number of
 * NOISE_LVL_HISTORY to average
 *
 * Hydra_Software_Devel/7420_mcvp/2   2/3/09 7:21p syang
 * PR 45785, PR 45789: merge from main branch on 2/3/09
 *
 * Hydra_Software_Devel/104   2/3/09 4:11p syang
 * PR 51795: change to use averaged snr value
 *
 * Hydra_Software_Devel/7420_mcvp/1   1/23/09 11:29p syang
 * PR 45785, PR 45789: add PI support for new the new module  MCVP
 * (MCTF+MCDI)
 *
 * Hydra_Software_Devel/101   1/19/09 8:21p pntruong
 * PR51300: Arrange reading of anr status to not conflict with reset rul.
 *
 * Hydra_Software_Devel/100   1/9/09 2:24p syang
 * PR 50758: call Anr_BuildRul_SrcInit_isr if anr internal PxlFmt changes,
 * rather than according to pPicComRulInfo->PicDirty
 *
 * Hydra_Software_Devel/99   1/8/09 4:09p syang
 * PR 49579, PR 50867: 1). set MC_NM K1 to 0 for bypass; 2). set diff
 * AND_0.EDGE_TH and AND_0.CONTENT_TH for HD and SD; 3). use diff K
 * table for RF, SD, ED, and HD
 *
 * Hydra_Software_Devel/98   12/23/08 5:45p hongtaoz
 * PR49674: Garbage dot or line showing for interlace format during adjust
 * H-overscan value if ANR level is higher; turn off MCTF filtering when
 * ANR is under hard_start; ANR statistics feedback control should be
 * called on every VSYNC;
 *
 * Hydra_Software_Devel/97   11/25/08 11:10a syang
 * PR 49562: add reset to output channel ping-pong buf of sub-modules and
 * loopbacks and free-channels
 *
 * Hydra_Software_Devel/96   11/21/08 1:05p syang
 * PR 49516: update per JJW's request on 11/21/08
 *
 * Hydra_Software_Devel/95   11/20/08 3:40p syang
 * PR 49516: 1) Improve the SNR detection algorithm; 2).  Update
 * BVDC_P_AnrKValue s_ulAnrKValue[] table;  as requested
 *
 * Hydra_Software_Devel/94   11/18/08 7:55p pntruong
 * PR49053: Corrected the interpretation of drdb firstline.
 *
 * Hydra_Software_Devel/93   11/14/08 5:25p pntruong
 * PR49053: Temporary fix for secam flickering.
 *
 * Hydra_Software_Devel/92   11/14/08 4:19p syang
 * PR 48839: 1). update as PR requested; 2).  handle RUL loss (without
 * exec)
 *
 * Hydra_Software_Devel/91   10/24/08 10:42a syang
 * PR 46477: program ANR bvb-in-size into RUL as it changes dynamically
 *
 * Hydra_Software_Devel/90   10/22/08 6:54p hongtaoz
 * PR46738: reset MAD SIOB_MODE back to MAD only mode if ANR is disabled
 * and not in mem saving mode to avoid hanging MAD next time;
 *
 * Hydra_Software_Devel/89   10/22/08 4:38p hongtaoz
 * PR48136, PR46738: enable TF_OUT_CAP for MCTF_ONLY mode; added
 * workaround (K1=0) for ANR bypass mode;
 *
 * Hydra_Software_Devel/88   10/17/08 7:54p yuxiaz
 * PR46305: Fixed assertion in BVDC_P_Anr_BuildRul_SrcInit_isr.
 *
 * Hydra_Software_Devel/87   10/8/08 12:17p pntruong
 * PR46735: B0: Need SECAM Dr/Db adjustment implemented.
 *
 * Hydra_Software_Devel/86   10/3/08 10:59a yuxiaz
 * PR46305, PR46307, PR46309: Use new TestFeature1 function from BSUR.
 * Change BVDC_P_SUPPORT_VIDEO_TestFeature1 to
 * BVDC_P_SUPPORT_VIDEO_TESTFEATURE1.
 *
 * Hydra_Software_Devel/85   10/3/08 9:22a yuxiaz
 * PR46305, PR46307, PR46309: More Video TestFeature1 support.
 *
 * Hydra_Software_Devel/84   10/1/08 3:55p hongtaoz
 * PR47490, PR46826: disable dithering when ANR is enabled; 3548B0 moved
 * MAD/ANR dither to MAD_SIOB_0 block;
 *
 * Hydra_Software_Devel/83   9/30/08 6:29p hongtaoz
 * PR46738: fixed MAD/ANR memory saving mode;
 *
 * Hydra_Software_Devel/82   9/30/08 4:55p yuxiaz
 * PR46305, PR46307: Video TestFeature1 support: Fix throughput
 * calculation, combine MAD and ANR set TestFeature1 function.
 *
 * Hydra_Software_Devel/81   9/26/08 3:22p yuxiaz
 * PR46305, PR46307: More video TestFeature1 support for 3548 B0.
 *
 * Hydra_Software_Devel/80   9/25/08 11:58a syang
 * PR 45560: change " if (((lSnDb - pAnr->lSnDb) > 6) || ((pAnr->lSnDb -
 * lSnDb) > 6))" to if (((lSnDb - pAnr->lSnDb) > 10) || ((pAnr->lSnDb -
 * lSnDb) > 10)) aacording to Jorge's request
 *
 * Hydra_Software_Devel/79   9/24/08 11:11a syang
 * PR 46741: fix compile warning
 *
 * Hydra_Software_Devel/78   9/24/08 11:05a syang
 * PR 46741:  add MCTF_0_CONT_0_ALPHA_LOW_THRESHOLD.MC
 *
 * Hydra_Software_Devel/77   9/24/08 10:03a syang
 * PR 46742, PR 46739:  add maaf, add drain in anr when mad is not used
 *
 * Hydra_Software_Devel/76   9/23/08 1:42p syang
 * PR 46737:  Remove dithering feature from MCTF for 3548 B0
 *
 * Hydra_Software_Devel/75   9/23/08 12:44p pntruong
 * PR47013: Fixed typo.
 *
 * Hydra_Software_Devel/73   9/17/08 8:42p pntruong
 * PR47013: B0: Initial vdc support.
 *
 * Hydra_Software_Devel/72   9/16/08 3:25p pntruong
 * PR44921: Synced with central rdb.
 *
 * Hydra_Software_Devel/71   9/2/08 6:04p pntruong
 * PR46314, PR46316: Updated 3dcomb memory allocation, and added buffers
 * alloc debug messages.
 *
 * Hydra_Software_Devel/70   8/12/08 3:32p syang
 * PR 45560: clean-up history effect for both top and bottom field after
 * AND detects a big snr change (likely content change)
 *
 * Hydra_Software_Devel/69   8/8/08 3:27p yuxiaz
 * PR45484: Enable Dithering in VDC.
 *
 * Hydra_Software_Devel/68   8/6/08 11:51a yuxiaz
 * PR45367: Enable Dering in VDC.
 *
 * Hydra_Software_Devel/67   8/4/08 5:02p syang
 * PR 45395: modify MC_FILTER_COST_PARAME and VERY_HIGH_NOISE_LEVEL
 * setting as requested
 *
 * Hydra_Software_Devel/66   7/21/08 12:48p syang
 * PR 44755: fix compile error for 3563 C0
 *
 * Hydra_Software_Devel/65   7/18/08 1:42p syang
 * PR 44755:   1). added 10-bits mode; 2).  fixed a error with div for
 * NOISE_LEVEL[k]
 *
 * Hydra_Software_Devel/64   6/11/08 4:37p pntruong
 * PR 34847: Synced up with new rdb.
 *
 * Hydra_Software_Devel/63   6/5/08 5:03p hongtaoz
 * PR39807: 3548A0 bringup; put SCL in always_enable mode; separate
 * anr+mad combo mode and memory saving mode in macroes;
 *
 * Hydra_Software_Devel/62   5/19/08 5:03p yuxiaz
 * PR40122: Use new 10-bit 422 40-bit packed format for ANR on 3548.
 *
 * Hydra_Software_Devel/61   3/20/08 10:49a pntruong
 * PR39154, PR39202, PR39203: Fixed merged error.
 *
 * Hydra_Software_Devel/60   3/19/08 7:34p hongtaoz
 * PR39154, PR39202, PR39203: combined ANR and MAD in memory saving mode
 * on 3548; implemented MAD specific change; updated SCL precision for
 * 3548;
 *
 * Hydra_Software_Devel/59   2/29/08 4:17p yuxiaz
 * PR39158: Implement dithering in various BVN components for 3548.
 *
 * Hydra_Software_Devel/58   2/28/08 6:42p hongtaoz
 * PR39154, PR36273, PR39202: added HSCL block; enabled MAD/ANR for 3548;
 * fixed non-linear scaling when MAD is on;
 *
 * Hydra_Software_Devel/57   2/22/08 2:31p yuxiaz
 * PR39155: Added de-ringing filters in BVN.
 *
 * Hydra_Software_Devel/56   2/14/08 9:49a pntruong
 * PR39421: Removed warnings for2.6.18.0_gcc-4.2 toolchains.
 *
 * Hydra_Software_Devel/55   11/21/07 6:06p syang
 * PR36424: merge from PR36424 branch: When SNR of the scene changes
 * dramatically, increase alpha of the IIR filter to increase the fading
 * rate of the old scene.
 *
 * Hydra_Software_Devel/54   11/9/07 1:28p pntruong
 * PR33797: Use the correct bvb status to detect for short fields.
 *
 * Hydra_Software_Devel/53   11/5/07 8:30p pntruong
 * PR33797: Added stub for non-anr chips.
 *
 * Hydra_Software_Devel/52   11/5/07 8:05p pntruong
 * PR36462: PR33011:If CVBS signal doesn't contain burst signal, need S/W
 * assume a default format
 * PR36563:for RF channel, mute the video 20 vysnc when video format
 * change. This is to fix the transition that cause by RF auto detection:
 * PAL-M/PAL-Nc/NTSC
 * PR33797:to fix the transtion and anr hanging up during the switch
 * channel
 * PR36462: decrease the delay for VCR detection
 *
 * Hydra_Software_Devel/51   11/2/07 10:05a syang
 * PR 34773, PR 35313: improve msg for anr/mad buffer alloc
 *
 * Hydra_Software_Devel/50   10/30/07 2:29p rpan
 * PR34798: Removed support for obsoleted versions of ANR.
 *
 * Hydra_Software_Devel/49   8/13/07 6:13p syang
 * PR 33827: release ANR mem only if no win is connected
 *
 * Hydra_Software_Devel/48   8/13/07 5:45p syang
 * PR 33614: take off AND reset as 3D comb toggle because it does  not
 * solve the problem
 *
 * Hydra_Software_Devel/47   8/8/07 6:36p syang
 * PR 32440: reset AND when 3D com toggles and SrcInit is called due to
 * src dirty
 *
 * Hydra_Software_Devel/46   8/7/07 5:22p syang
 * PR 32440: add AND reset if 3D com toggles
 *
 * Hydra_Software_Devel/45   7/20/07 5:02p syang
 * PR 32384: still need to alloc buf for bypass mode
 *
 * Hydra_Software_Devel/44   7/19/07 2:39p tdo
 * PR33034: Modify DNR API to extend MNR/BNR/DCR filter level
 *
 * Hydra_Software_Devel/43   7/17/07 5:32p syang
 * PR 33051: avoid hard_start if not necessary
 *
 * Hydra_Software_Devel/42   7/11/07 7:44p syang
 * PR 31944: dynamically adjust alpha_low_thd
 *
 * Hydra_Software_Devel/41   7/5/07 6:47p syang
 * PR 32440: don't call SrcInit_isr if RulOp_eEnable bit is off (---assert
 * buf ptr NULL)
 *
 * Hydra_Software_Devel/40   7/3/07 4:20p syang
 * PR 32384: don't call SrcInit for bypass mode because it uses buf ptr
 *
 * Hydra_Software_Devel/39   7/3/07 2:37p syang
 * PR 32384: don't alloc mem for bypass mode
 *
 * Hydra_Software_Devel/38   6/28/07 5:09p pntruong
 * PR32503: Updated to use correct heap id for oversampling mode.
 *
 * Hydra_Software_Devel/37   6/28/07 3:58p syang
 * PR 32576: always re-calculate mad/anr bufHeapId inside
 * NeedReconfiguring_isr if mad/anr is to be sued
 *
 * Hydra_Software_Devel/36   6/22/07 5:14p syang
 * PR 32384: change to use top level HD_ANR_ENABLE_BYPASS for bypass
 *
 * Hydra_Software_Devel/35   6/21/07 3:27p syang
 * PR 32440: use pSource->bPictureChanged to decide if we re-decide
 * MAD/ANR heapId and re-init ANR
 *
 * Hydra_Software_Devel/34   6/21/07 10:20a syang
 * PR 31944: fix 7401 compile error; update A0 code to take positive
 * SnDbAdjust  as increasing strength
 *
 * Hydra_Software_Devel/33   6/20/07 5:36p syang
 * PR 31944: 1). add a new user defined struct ptr to ANR configure 2).
 * update SNR calculation code to match Jorge's script
 *
 * Hydra_Software_Devel/32   6/13/07 2:25p syang
 * PR 31944: update 3563 A0 to use new ANR API
 *
 * Hydra_Software_Devel/31   6/13/07 9:47a syang
 * PR 31944: put ANR K value table inro seperate file; change to use
 * SnDbAdj from 5 level adj
 *
 * Hydra_Software_Devel/30   6/8/07 1:47p syang
 * PR31813: use TOP_CTRL_STOP_ON_FIELD_COMPLETION to bring back ANR effect
 * after RUL exec trigering time changed
 *
 * Hydra_Software_Devel/29   6/8/07 11:29a syang
 * PR 31849: separate drain from disable for MAD, ANR and CAP
 *
 * Hydra_Software_Devel/28   6/5/07 5:27p syang
 * PR 31840: take off un-used assert in ReleaseConnect to allow ANR/DNR
 * shared by 2 wins
 *
 * Hydra_Software_Devel/27   6/4/07 5:56p syang
 * PR 28550: 1). read AND reg every vsync; 2). user S/N adj change from 2
 * to 3; 3). improve MSG
 *
 * Hydra_Software_Devel/26   5/30/07 3:39p syang
 * PR 28550: scaled AND threshold for HD src
 *
 * Hydra_Software_Devel/25   5/25/07 6:06p syang
 * PR 28550: add more SNL granuarity for K1 setting, looks good now
 *
 * Hydra_Software_Devel/24   5/22/07 4:15p syang
 * PR 28550: ensure demo mode setting take effect
 *
 * Hydra_Software_Devel/23   5/22/07 3:06p syang
 * PR 28550: change to set TOP_CTRL as CONTINUOUS_FOR_TEST to void cvbs
 * src hanging with ANR
 *
 * Hydra_Software_Devel/22   5/17/07 1:12p syang
 * PR 28550, PR 30732: add PI support for 3563 C0's new MCTF ANR; add
 * bypass mode for ANR
 *
 * Hydra_Software_Devel/21   5/10/07 2:45p yuxiaz
 * PR 28895, PR 28898, PR 29846 , PR 28550: Fixed compile error on 7405.
 *
 * Hydra_Software_Devel/20   5/10/07 9:56a syang
 * PR 28895, PR 28898, PR 29846 , PR 28550: 1). add frame for C0 ANR PI
 * support; 2). move MAD/ANR buf alloc/free to MAD/ANR; 3). remove
 * drainVnet state
 *
 * Hydra_Software_Devel/19   3/1/07 2:03p syang
 * PR 22569: turn off msg
 *
 * Hydra_Software_Devel/18   2/28/07 4:48p syang
 * PR 22569: 1). tune anr dynamic seting;  2). add anr demo mode; 3). use
 * hResource in SubRul
 *
 * Hydra_Software_Devel/17   2/23/07 6:26p tdo
 * PR 27970: Share LPB and FCH if source is the same to eliminate the out
 * of resource error when number of LPB and FCH are limited
 *
 * Hydra_Software_Devel/16   2/16/07 7:12p syang
 * PR 22569:  fixed calcu accuracy and register field override
 *
 * Hydra_Software_Devel/15   2/16/07 4:43p syang
 * PR 22569: fix build error for 7401
 *
 * Hydra_Software_Devel/14   2/16/07 3:40p syang
 * PR 22569: 1). added adaptive filter setting; 2). added filter level
 *
 * Hydra_Software_Devel/13   2/1/07 3:53p syang
 * PR 27054: fix compile error for 7403
 *
 * Hydra_Software_Devel/12   2/1/07 12:12p syang
 * PR 22569: update BuildRul_isr to use pSrcOut
 *
 * Hydra_Software_Devel/11   1/22/07 3:14p syang
 * PR 27054: 1). only 1 win build rul, 2). reader's modules don't build
 * rul before video sur enabled; 3). writer's modules don't build rul
 * before (GetReconfiguring_isr==true )
 *
 * Hydra_Software_Devel/10   1/16/07 11:23a syang
 * PR 22569: ANR works with all internal VFDs and CAPs on
 *
 * Hydra_Software_Devel/9   1/15/07 10:38a syang
 * PR 22569: correct VSIZE setting, bypass mode work now.
 *
 * Hydra_Software_Devel/8   1/14/07 5:52p syang
 * PR 22569: move "one time" init to 1st RUL of new vnet , init in _create
 * will be overrided by reset
 *
 * Hydra_Software_Devel/7   1/13/07 2:33p syang
 * PR 22569: update according to anr_and_vpp.bss' change; disable
 * feeders/caps temporally
 *
 * Hydra_Software_Devel/6   1/11/07 7:02p syang
 * PR 22569: 1st time check in after anr building vnet correctly and not
 * hang the sys. But display is still static
 *
 * Hydra_Software_Devel/5   1/10/07 10:40a syang
 * PR 22569: correct a comments only
 *
 * Hydra_Software_Devel/4   1/5/07 5:45p syang
 * PR 22569:  fix compile error for 7400/7403/7440
 *
 * Hydra_Software_Devel/3   1/5/07 4:25p syang
 * PR 22569: almost completed coding for new vnet build/shutdown algorithm
 * (by each sub-modules)
 *
 * Hydra_Software_Devel/2   12/22/06 11:49a syang
 * PR 22569:  1st time have simple_vdc compiled successfully
 *
 * Hydra_Software_Devel/1   12/21/06 3:09p syang
 * PR 22569: init version
 *
 ***************************************************************************/
#include "bstd.h"
#include "bkni.h"
#include "brdc.h"
#include "bvdc.h"
#include "bchp_vnet_f.h"
#include "bvdc_resource_priv.h"
#include "bvdc_source_priv.h"
#include "bvdc_window_priv.h"
#include "bvdc_buffer_priv.h"
#include "bvdc_anr_priv.h"
#include "bvdc_compositor_priv.h"
#include "bvdc_capture_priv.h"

#if (BVDC_P_SUPPORT_ANR) || (BVDC_P_SUPPORT_MANR)

#include "bchp_hd_anr_mctf_0.h"
#include "bchp_hd_anr_and_0.h"
#include "bchp_mmisc.h"
#if (BVDC_P_SUPPORT_ANR)
#include "bchp_hd_anr_0.h"
#endif

#if BVDC_P_SUPPORT_DMISC
#include "bchp_dmisc.h"
#endif

#if (BVDC_P_SUPPORT_VIDEO_TESTFEATURE1_MAD_ANR)
#if (BVDC_P_SUPPORT_ANR)
#include "bchp_mad_siob_0.h"
#elif (BVDC_P_SUPPORT_MANR)
#include "bchp_siob_0.h"
#endif
#endif


BDBG_MODULE(BVDC_ANR);
BDBG_OBJECT_ID(BVDC_ANR);

/***************************************************************************
 *
 * The followings are exported to other sub-modules inside VDC
 *
 ***************************************************************************/

#define BVDC_P_ANR_MSG_ON              0
#if (BVDC_P_ANR_MSG_ON==1)
#define BDBG_P_ANR_MSG    BDBG_ERR
#else
#define BDBG_P_ANR_MSG(a)
#endif

/***************************************************************************
 *
 ***************************************************************************/
#if (BVDC_P_SUPPORT_MANR_VER >= BVDC_P_MANR_VER_2)
#define HW3548_47_IS_FIXED                            (1)
#else
#define HW3548_47_IS_FIXED                            (0)
#endif

/***************************************************************************
 * {private}
 *
 * BVDC_P_Anr_Create
 *
 * called by BVDC_Open only
 */
BERR_Code BVDC_P_Anr_Create
	( BVDC_P_Anr_Handle *         phAnr,
	  BVDC_P_AnrId                eAnrId,
	  BREG_Handle                 hRegister,
	  BVDC_P_Resource_Handle      hResource )
{
	BVDC_P_AnrContext  *pAnr;
	BERR_Code  eResult = BERR_SUCCESS;

	BDBG_ENTER(BVDC_P_Anr_Create);

	if(eAnrId == BVDC_P_AnrId_eUnknown)
		return eResult;
	/* Debug level are control by app.  Only needed to turn on for debugging
	 * purpose only. */
	/*BDBG_SetModuleLevel("BVDC_ANR", BDBG_eMsg);*/

	/* in case creation failed */
	BDBG_ASSERT(phAnr);

	pAnr = (BVDC_P_AnrContext *) (BKNI_Malloc(sizeof(BVDC_P_AnrContext)));
	if( pAnr )
	{
		/* init the context */
		BKNI_Memset((void*)pAnr, 0x0, sizeof(BVDC_P_AnrContext));
		BDBG_OBJECT_SET(pAnr, BVDC_ANR);
		pAnr->eId = eAnrId;
		pAnr->hRegister = hRegister;
		pAnr->eWinId = BVDC_P_SRC_INVALID;
		pAnr->eBufHeapId = BVDC_P_BufferHeapId_eUnknown;
		pAnr->ulRegOffset = 0;
		/* init the SubRul sub-module */
		BVDC_P_SubRul_Init(&(pAnr->SubRul), BVDC_P_Anr_MuxAddr(pAnr),
			BVDC_P_Anr_PostMuxValue(pAnr), BVDC_P_DrainMode_eBack,
			BVDC_P_ANR_NUM_ACCUM, hResource);

		/* default settings for up sampler and down sampler */
		pAnr->stUpSampler.bUnbiasedRound = true;
		pAnr->stUpSampler.eFilterType    = BVDC_422To444Filter_eTenTaps;
		pAnr->stUpSampler.eRingRemoval   = BVDC_RingSuppressionMode_eNormal;

		pAnr->stDnSampler.eFilterType    = BVDC_444To422Filter_eStandard;
		pAnr->stDnSampler.eRingRemoval   = BVDC_RingSuppressionMode_eNormal;

		/* no memory sharing by default */
		pAnr->bMemSaving    = false;
		pAnr->ePxlFmt       = BVDC_P_CAP_PIXEL_FORMAT_8BIT422;
		pAnr->bBypassFilter = false;

		BKNI_Memset((void*)&pAnr->stTestFeature1, 0x0, sizeof(BVDC_P_TestFeature1_Settings));

		*phAnr = pAnr;
	}
	else
	{
		eResult = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
		*phAnr = NULL;
	}

	BDBG_LEAVE(BVDC_P_Anr_Create);
	return eResult;
}

/***************************************************************************
 * {private}
 *
 * BVDC_P_Anr_Destroy
 *
 * called by BVDC_Close only
 */
BERR_Code BVDC_P_Anr_Destroy
	( BVDC_P_Anr_Handle          hAnr )
{
	BERR_Code  eResult = BERR_SUCCESS;

	BDBG_ENTER(BVDC_P_Anr_Destroy);
	BDBG_OBJECT_ASSERT(hAnr, BVDC_ANR);

	BDBG_OBJECT_DESTROY(hAnr, BVDC_ANR);
	/* it is gone afterwards !!! */
	BKNI_Free((void*)hAnr);

	BDBG_LEAVE(BVDC_P_Anr_Destroy);
	return BERR_TRACE(eResult);
}

#if (BVDC_P_SUPPORT_ANR)

/***************************************************************************
 * {private}
 *
 * BVDC_P_Anr_AcquireConnect
 *
 * It is called by BVDC_Window_Validate after changing from diable anr to
 * enable anr.
 */
BERR_Code BVDC_P_Anr_AcquireConnect
	( BVDC_P_Anr_Handle                 hAnr,
	  BVDC_Heap_Handle                  hHeap,
	  BVDC_P_WindowId                   eWinId,
	  const BVDC_Anr_Settings          *pAnrSetting,
	  BVDC_P_Source_InfoPtr             pCurSrcInfo)
{
	BERR_Code  eResult = BERR_SUCCESS;

	BDBG_ENTER(BVDC_P_Anr_AcquireConnect);

	hAnr->eWinId = eWinId;
	hAnr->hHeap = hHeap;
	hAnr->pAnrSetting = pAnrSetting;
	hAnr->pCurSrcInfo = pCurSrcInfo;

	BDBG_LEAVE(BVDC_P_Anr_AcquireConnect);
	return BERR_TRACE(eResult);
}

/***************************************************************************
 * {private}
 *
 * BVDC_P_Anr_ReleaseConnect_isr
 *
 * It is called after window decided that anr is no-longer used by HW in its
 * vnet mode (i.e. it is really shut down and teared off from vnet).
 */
BERR_Code BVDC_P_Anr_ReleaseConnect_isr
	( BVDC_P_Anr_Handle         *phAnr )
{
	BERR_Code  eResult = BERR_SUCCESS;

	BDBG_ENTER(BVDC_P_Anr_ReleaseConnect_isr);

	/* handle validation */
	if (NULL != *phAnr)
	{
		BDBG_OBJECT_ASSERT(*phAnr, BVDC_ANR);

		/* another win might still using it */
		BVDC_P_Resource_ReleaseHandle_isr(
			BVDC_P_SubRul_GetResourceHandle_isr(&(*phAnr)->SubRul),
			BVDC_P_ResourceType_eAnr, (void *)(*phAnr));

		/* this makes win to stop calling anr code */
		*phAnr = NULL;
	}

	BDBG_LEAVE(BVDC_P_Anr_ReleaseConnect_isr);
	return BERR_TRACE(eResult);
}

/***************************************************************************
 * {private}
 *
 * BVDC_P_Anr_SetVnetAllocBuf_isr
 *
 * Called by BVDC_P_*_BuildRul_isr to setup for joinning into vnet (including
 * optionally acquiring loop-back) and allocate buffers
 */
void BVDC_P_Anr_SetVnetAllocBuf_isr
	( BVDC_P_Anr_Handle          hAnr,
	  uint32_t                   ulSrcMuxValue,
	  BVDC_P_VnetPatch           eVnetPatchMode,
	  BVDC_P_BufferHeapId        eBufHeapId,
	  uint32_t                   ulBufCnt,
	  bool                       bMemSaving )
{
	BERR_Code  eResult = BERR_SUCCESS;
	int  ii;

	BDBG_ENTER(BVDC_P_Anr_SetVnetAllocBuf_isr);
	BDBG_OBJECT_ASSERT(hAnr, BVDC_ANR);

	/* set up for joining vnet, including acquiring loop-back */
	BVDC_P_SubRul_SetVnet_isr(&(hAnr->SubRul), ulSrcMuxValue, eVnetPatchMode);

	/* allocate internally used buffers. Currently this is done during VNET reconfig
	 * since MAD and ANR are two separate BVN routable blocks!
	 * note: it is needed even if currently we are in bypass mode, because
	 * later on we won't recofigure vnet as user change to enable mode.
	 * note: ANR might be already in use by another win might */
	/* 3548 memory saving mode will share deinterlacer's memory for ANR */
#if BVDC_P_SUPPORT_ANR_MAD_MEM_SAVING_MODE
	if(!bMemSaving)
#endif
	{
		BVDC_P_BUF_MSG(("Anr[%d] alloc %d buffers (%s)", hAnr->eId, ulBufCnt,
			BVDC_P_BUFFERHEAP_GET_HEAP_ID_NAME(eBufHeapId)));

		for (ii=0; ii<(int)ulBufCnt; ii++)
		{
			if (NULL == hAnr->apHeapNode[ii])
			{
				eResult = BVDC_P_BufferHeap_AllocateBuffers_isr(
					hAnr->hHeap, &(hAnr->apHeapNode[ii]), 1, false,
					eBufHeapId, BVDC_P_BufferHeapId_eUnknown);
				/* Not enough memory, dump out configuration */
				if(eResult == BERR_OUT_OF_DEVICE_MEMORY)
				{
					BDBG_ERR(("Anr[%d] Not enough memory for ANR! Configuration:", hAnr->eId));
					BDBG_ERR(("Anr[%d] ulBufCnt[%d]: %d", hAnr->eId, ii, ulBufCnt));
				}
				BDBG_ASSERT(BERR_SUCCESS == eResult);
			}
		}
	}

	/* save the flag */
	hAnr->bMemSaving = bMemSaving;
	hAnr->eBufHeapId = eBufHeapId;

	BDBG_ASSERT(BERR_SUCCESS == eResult);

	BDBG_LEAVE(BVDC_P_Anr_SetVnetAllocBuf_isr);
}

/***************************************************************************
 * {private}
 *
 * BVDC_P_Anr_UnsetVnetFreeBuf_isr
 *
 * called by BVDC_P_Window_UnsetWriter(Reader)Vnet_isr to to release the
 * potentially used loop-back, and free buffers
 */
void BVDC_P_Anr_UnsetVnetFreeBuf_isr
	( BVDC_P_Anr_Handle          hAnr )
{
	int  ii;

	BDBG_ENTER(BVDC_P_Anr_UnsetVnetFreeBuf_isr);
	BDBG_OBJECT_ASSERT(hAnr, BVDC_ANR);

	/* release free-channel or loop-back */
	BVDC_P_SubRul_UnsetVnet_isr(&(hAnr->SubRul));

	/* free internally used buffers if no win is using this ANR. */
	if (0 == BVDC_P_SubRul_GetWinsActFlags(&(hAnr->SubRul)))
	{
		for (ii = 0; ii < 4 *BVDC_P_ANR_BUFFER_COUNT; ii++)
		{
			if (hAnr->apHeapNode[ii])
			{
				BVDC_P_BUF_MSG(("Anr[%d] free  %d buffers (%s)", hAnr->eId, 1,
					BVDC_P_BUFFERHEAP_GET_HEAP_ID_NAME(hAnr->apHeapNode[ii]->pHeapInfo->eBufHeapId)));

				BVDC_P_BufferHeap_FreeBuffers_isr(
					hAnr->hHeap, &(hAnr->apHeapNode[ii]), 1, false);
				hAnr->apHeapNode[ii] = NULL;
			}
		}
	}

	BDBG_LEAVE(BVDC_P_Anr_UnsetVnetFreeBuf_isr);
}

/***************************************************************************
 * {private}
 *
 * BVDC_P_Anr_BuildRul_DrainVnet_isr
 *
 * called by BVDC_P_Anr_BuildRul_isr after resetting to drain the module and
 * its pre-patch FreeCh or LpBack
 */
static void BVDC_P_Anr_BuildRul_DrainVnet_isr
	( BVDC_P_Anr_Handle              hAnr,
	  BVDC_P_PictureNode            *pPicture,
	  BVDC_P_ListInfo               *pList )
{
	uint32_t  ulRegOffset;
	uint32_t  ulResetMask;
	uint32_t  ulChnResetReg = 0, ulChnResetMask = 0;
	bool  bMemSaving;

	BDBG_OBJECT_ASSERT(hAnr, BVDC_ANR);

#if (BVDC_P_SUPPORT_ANR_VER >= BVDC_P_ANR_VER_3)
	bMemSaving = hAnr->bMemSaving;
	/* force to enable MCTF_0_MAD_OUT1_DRAIN */
	hAnr->bMemSaving = false;
#else
	BSTD_UNUSED(bMemSaving);
#endif

	ulRegOffset = hAnr->ulRegOffset;
	ulResetMask = BCHP_MMISC_SOFT_RESET_ANR_0_MASK;

	/* reset sub and connect the module to a drain */
	BVDC_P_SubRul_StartDrain_isr(&(hAnr->SubRul), pList,
		BCHP_MMISC_SOFT_RESET, ulResetMask);

	/* enable and disable anr to drain */
	BVDC_P_Anr_BuildRul_SetEnable_isr(hAnr, pPicture, pList, true,  BAVC_Polarity_eFrame, false, false);
	BVDC_P_Anr_BuildRul_SetEnable_isr(hAnr, pPicture, pList, false, BAVC_Polarity_eFrame, false, false);

	/* disconnect the module from the drain; reset it again; drain the
	 * free-channel or loop-back used to to patch the vnet; and also
	 * reset the channel ping-pong buf of the module and the patcher. */
#ifdef BCHP_MMISC_VNET_B_CHANNEL_RESET
	ulChnResetReg  = BCHP_MMISC_VNET_B_CHANNEL_RESET;
	ulChnResetMask = BCHP_MMISC_VNET_B_CHANNEL_RESET_ANR_0_RESET_MASK <<
		(hAnr->eId - BVDC_P_AnrId_eAnr0);
#endif
	BVDC_P_SubRul_FinishDrain_isr(&(hAnr->SubRul), pList,
		BCHP_MMISC_SOFT_RESET, ulResetMask, ulChnResetReg, ulChnResetMask);

#if (BVDC_P_SUPPORT_ANR_VER >= BVDC_P_ANR_VER_3)
	/* restore in case this shutdown is decided by win buildRul due to src/pic muted */
	hAnr->bMemSaving = bMemSaving;

	/* if mad is off, mad's vfd/cap and siob are shared by anr, they needs reset too */
	if (!bMemSaving)
	{
		BVDC_P_SUBRUL_ONE_REG(pList, BCHP_DMISC_SOFT_RESET, 0, BCHP_DMISC_SOFT_RESET_MAD_0_MASK);
		BVDC_P_SUBRUL_ONE_REG(pList, BCHP_DMISC_SOFT_RESET, 0, 0);
	}
#endif
}

#if (BVDC_P_SUPPORT_ANR_MAAF)
static void BVDC_P_Anr_BuildRul_AutoCtrl_isr
	( BVDC_P_Anr_Handle              hAnr,
	  BVDC_P_ListInfo               *pList,
	  const BVDC_P_PicComRulInfo    *pPicComRulInfo,
	  const BVDC_P_Rect             *pAnrIn )
{
	bool bDrTf = pPicComRulInfo->bDrTf;

	BDBG_OBJECT_ASSERT(hAnr, BVDC_ANR);

	if((pAnrIn->lTop/2) & 1)
	{
		bDrTf = !bDrTf;
	}

	/* 0 = Db, 1 = Dr */
	hAnr->ulAutoCtrlReg &= ~(
		BCHP_MASK(HD_ANR_MCTF_0_AUTO_CTRL, FIRST_LINE));

	hAnr->ulAutoCtrlReg |=  (
		BCHP_FIELD_DATA(HD_ANR_MCTF_0_AUTO_CTRL, FIRST_LINE, bDrTf));

	BVDC_P_SUBRUL_ONE_REG(pList, BCHP_HD_ANR_MCTF_0_AUTO_CTRL,
		hAnr->ulRegOffset, hAnr->ulAutoCtrlReg);
	return;
}
#endif

/***************************************************************************
 * {private}
 *
 * BVDC_P_Anr_BuildRul_isr
 *
 * called by BVDC_Window_BuildRul_isr at every src or vec vsync (depending on
 * whether reader side or writer side is using this module)
 *
 * Input:
 *    eVnetState - reader or writer window/vnet state
 *    pPicComRulInfo - the PicComRulInfo that is the shared Picture info by
 *      all sub-modules when they build rul.
 */
void BVDC_P_Anr_BuildRul_isr
	( BVDC_P_Anr_Handle              hAnr,
	  BVDC_P_ListInfo               *pList,
	  BVDC_P_State                   eVnetState,
	  BVDC_P_PictureNode            *pPicture )
{
	BVDC_P_PicComRulInfo  *pPicComRulInfo;
	const BVDC_P_Rect  *pAnrIn;
	BVDC_P_AnrDirtyBits  *pSwDirty, *pHwDirty;

	uint32_t  ulRulOpsFlags;
	uint32_t  ulHSize, ulVSize;
	bool  bInit, bEnable;

	BDBG_OBJECT_ASSERT(hAnr, BVDC_ANR);
	pPicComRulInfo = &(pPicture->PicComRulInfo);
	pAnrIn = pPicture->pAnrIn;

	ulRulOpsFlags = BVDC_P_SubRul_GetOps_isr(
		&(hAnr->SubRul), pPicComRulInfo->eWin, eVnetState, pList->bLastExecuted);
	bEnable = (ulRulOpsFlags & BVDC_P_RulOp_eEnable);
	bInit = (ulRulOpsFlags & BVDC_P_RulOp_eVnetInit);

	pSwDirty = &hAnr->stSwDirty;
	pHwDirty = &hAnr->stHwDirty;

	/* input size to anr might change due to dynamic src change or letter box
	 * cut without vnet reconfiguration */
	if (bEnable)
	{
		ulHSize = pAnrIn->ulWidth;
		ulVSize = (BAVC_Polarity_eFrame == pPicComRulInfo->eSrcOrigPolarity)?
			pAnrIn->ulHeight: pAnrIn->ulHeight / 2;

		if ((ulHSize != hAnr->ulHSize) || (ulVSize != hAnr->ulVSize))
		{
			hAnr->ulHSize = ulHSize;
			hAnr->ulVSize = ulVSize;

			/* inform SetEnable_isr to set hard_start */
			pSwDirty->stBits.bSize = BVDC_P_DIRTY;
		}

		/* need to rebuild changes into RUL if last RUL is not executed */
		if(false == pList->bLastExecuted)
		{
			BVDC_P_OR_ALL_DIRTY(pSwDirty, pHwDirty);
		}
	}

	/* if we re-set vnet, or if src video format changes (HdDvi), we need to
	 * do SrcInit, such as reset buffer size and stride */
	if ((bInit || pSwDirty->stBits.bSize || pSwDirty->stBits.bPxlFmt || pPicComRulInfo->PicDirty.stBits.bInputFormat) &&
	    (ulRulOpsFlags & BVDC_P_RulOp_eEnable))
	{
		BVDC_P_Anr_BuildRul_SrcInit_isr(hAnr, pList, pPicture);
	}

	/* periodic statistics read and dynamic setting: per VSYNC basis! */
	if ((ulRulOpsFlags & BVDC_P_RulOp_eStatisInit)
#if (BVDC_P_SUPPORT_ANR_VER < BVDC_P_ANR_VER_3)
		&&
		(BVDC_FilterMode_eBypass != hAnr->pAnrSetting->eMode)
#endif
		)
	{
		BVDC_P_Anr_BuildRul_StatisRead_isr(hAnr, pList, bInit);
	}

	if (bEnable)
	{
		BVDC_P_Anr_BuildRul_SetEnable_isr(hAnr, pPicture, pList, true,
			pPicComRulInfo->eSrcOrigPolarity,
			(pPicture->stFlags.bPictureRepeatFlag || pPicture->stFlags.bRepeatField), bInit);

		/* join in vnet after enable. note: its src mux is initialed as disabled */
		if (ulRulOpsFlags & BVDC_P_RulOp_eVnetInit)
		{
			BVDC_P_SubRul_JoinInVnet_isr(&(hAnr->SubRul), pList);
		}

		/* changes has been built to RUL, but not executed yet */
		*pHwDirty = *pSwDirty;
		BVDC_P_CLEAN_ALL_DIRTY(pSwDirty);
	}
	else if (ulRulOpsFlags & BVDC_P_RulOp_eDisable)
	{
		BVDC_P_SubRul_DropOffVnet_isr(&(hAnr->SubRul), pList);
		BVDC_P_Anr_BuildRul_SetEnable_isr(hAnr, pPicture, pList, false,
			pPicComRulInfo->eSrcOrigPolarity, false, bInit);
	}
	else if (ulRulOpsFlags & BVDC_P_RulOp_eDrainVnet)
	{
		BVDC_P_Anr_BuildRul_DrainVnet_isr(hAnr, pPicture, pList);
	}

#if (BVDC_P_SUPPORT_ANR_MAAF)
	/* Build every vsync when */
	if ((bEnable) &&
	    (hAnr->bEnableMaaf) &&
	    (BVDC_FilterMode_eBypass != hAnr->pAnrSetting->eMode))
	{
		BVDC_P_Anr_BuildRul_AutoCtrl_isr(hAnr, pList, pPicComRulInfo, pPicture->pAnrIn);
	}
#endif

	return;
}

#endif

/***************************************************************************
 * {private}
 *
 * BVDC_P_Anr_SetDemoMode_isr
 *
 * called by BVDC_Window_ApplyChanges to set anr demo mode
 */
BERR_Code BVDC_P_Anr_SetDemoMode_isr
	( BVDC_P_Anr_Handle            hAnr,
	  BVDC_SplitScreenMode         eDemoMode )
{
	BDBG_OBJECT_ASSERT(hAnr, BVDC_ANR);
	BDBG_ASSERT(eDemoMode <= BVDC_SplitScreenMode_eRight);
	hAnr->eDemoMode = eDemoMode;

	return BERR_TRACE(BERR_SUCCESS);
}


/***************************************************************************
 *
 * The followings are func used inside ANR/MCVP
 *
 ***************************************************************************/
/* These macros are used in BVDC_P_Anr_BuildRul_SetEnable_isr() and */
/* BVDC_P_Anr_BuildRul_StatisRead_isr() */
#define BVDC_P_ANR_NEED_HARD_START(hAnr)   \
	(bInit || (hAnr)->stSwDirty.stBits.bPrevBypass || (hAnr)->stSwDirty.stBits.bSize || (hAnr)->stSwDirty.stBits.bPxlFmt)

/* These macros are used in BVDC_P_Anr_BuildRul_SrcInit_isr() and */
/* BVDC_P_Anr_BuildRul_StatisRead_isr() */
#define BVDC_P_AndThdScl_FRAC       11

/* These macros are used in BVDC_P_Anr_BuildRul_SrcInit_isr() and */
/* BVDC_P_Anr_BuildRul_SetEnable_isr() */
#define BVDC_P_MC_ALPHA_CALC_PARAME_INIT_SETTING     0x0708010e
#define BVDC_P_NMC_ALPHA_CALC_PARAME_INIT_SETTING    0x0013014c
#define BVDC_P_MC_NMC_ALPHA_CALC_PARAME_INIT_SETTING 0x0080010e

/* These macros are used in BVDC_P_Anr_BuildRul_SrcInit_isr() */
#define BVDC_P_MAAF_ALPHA_CALC_PARAME_K0    332
#define BVDC_P_MAAF_ALPHA_CALC_PARAME_K1    196
#define BVDC_P_MAAF_ALPHA_THRESH_MIN        0
#define BVDC_P_MAAF_ALPHA_THRESH_MAX        256
#define BVDC_P_MAAF_PIX_PARAME_THR_SLOPE    10
#define BVDC_P_MAAF_PIX_PARAME_CHROMA_THR   48
#define BVDC_P_MAAF_PIX_PARAME_LUMA_THR     16
#define BVDC_P_MAAF_ALPHA_THOLD_2D_MIN      0
#define BVDC_P_MAAF_ALPHA_THOLD_2D_MAX      1024

/* These macros are used in BVDC_P_Anr_BuildRul_StatisRead_isr() */
#define NUM_NOISY_SAMPLE_THD        (9000) /*threshold for noisy samples: set to be 9000 for picture size of 720x486 or 720x480 */
#define NUMDIFF_THD                 (6000)
#define NUM_NOISY_SAMPLE_THD_BIG    (125000)
#define BVDC_P_MAKE_FIX16_16(a)     ((int)(((a) * (1 << 16))))
/* turn off MCTF filter for a few vsyncs when noise level changes abruptly or hard_start */
#define BVDC_P_ANR_CLEAN_VSYNCS      4
#define BVDC_P_SNR_MIN              (20)

/***************************************************************************
 * This table is generated basing on NOISE_LEVEL_DB > 60, 59, ..., 20
 * where NOISE_LEVEL_DB is the value of
 * f = (20*log(255*25.6 *4/NOISE_LEVEL_HISTORY)/log(10))
 * where "*4" should theoretically be "*15". "*15" is added to Shawn's original
 * formula because AND did not divide NOISE_LEVEL_k_MSB/LSB by 15. Here we use
 * "*4" only because experiment shows it looks good.
 * Sony content mapping:
 *    Sony S/N:  off   40  37  34    30   20
 *    Ours:      ~40  ~40 ~40  <20  <20  <20
 */
#if (BVDC_P_SUPPORT_ANR)

#define BVDC_P_SNR_MAX              (60)   /* no noise */
#if (BVDC_P_SUPPORT_ANR_VER < BVDC_P_ANR_VER_2)
#define VERY_HIGH_NOISE_LEVEL       (8726)  /*set to 21db noise level expectation, which is out of range of the test*/
static uint16_t s_usNoiseLvlThresh[] = {
	2465, 2197, 1958, 1745, 1555, 1386, 1235, 1101, 981, 875,   /* SNR: 20 -> 29 */
	780,  695,  619,  552,  492,  438,  391,  348,  310, 277,   /* SNR: 30 -> 39 */
	247,  220,  196,  175,  156,  139,  124,  110,  98,  87,    /* SNR: 40 -> 49 */
	78,   69,   62,   55,   49,   44,   39,   35,   31,  28,    /* SNR: 50 -> 59 */
	25 };                                                       /* SNR: 60 */
#else
#define VERY_HIGH_NOISE_LEVEL       (10) /* cause to not do any filter to avoid side effect, it does not help anywhere */
static uint16_t s_usNoiseLvlThresh[] = {
	10476,9336, 8321, 7416, 6610, 5891, 5250, 4679, 4170,3717,  /* SNR: 20->29 */
	3313, 2952, 2465, 2197, 1958, 1745, 1555, 1386, 1235,1101,  /* SNR: 30->39 */
	981,  875,  780,  695,  619,  552,  492,  438,  391, 348,   /* SNR: 40->49 */
	310,  277,  247,  220,  196,  175,  156,  139,  124, 110,   /* SNR: 50->59 */
	98 } ;                                                      /* SNR: 60 */
#endif

#else /* BVDC_P_SUPPORT_MANR */

#define BVDC_P_SNR_MAX              (80)   /* no noise */
static uint16_t s_usNoiseLvlThreshSD[] = {
	  1350, 1265, 1165, 1070, 970, 870, 770, 620, 520, 470,  /* SNR: 20 -> 29 */
	   445,  420,  395,  370, 345, 320, 300, 280, 260, 240,  /* SNR: 30 -> 39 */
	   220,  200,  183,  170, 153, 145, 113, 108, 103,  98,  /* SNR: 40 -> 49 */
	    93,   88,   83,   78,  75,  73,  70,  68,  65,  63,  /* SNR: 50 -> 59 */
	    60,   58,   55,   53,  50,  48,  45,  43,  40,  38,  /* SNR: 60 -> 69 */
	    35,   33,   30,   28,  25,  23,  20,  18,  15,  13,  /* SNR: 70 -> 79 */
	    10 };                                                /* SNR: 80 */
static uint16_t s_usNoiseLvlThreshHD[] = {
	   425,  375,  325,  275, 225, 213, 200, 190, 180, 170,  /* SNR: 20 -> 29 */
	   163,  155,  150,  145, 140, 135, 130, 125, 120, 115,  /* SNR: 30 -> 39 */
	   110,  105,   96,   90,  81,  78,  61,  59,  56,  54,  /* SNR: 40 -> 49 */
	    51,   49,   46,   44,  43,  41,  40,  39,  38,  36,  /* SNR: 50 -> 59 */
	    35,   34,   33,   31,  30,  29,  28,  26,  25,  24,  /* SNR: 60 -> 69 */
	    23,   21,   20,   19,  18,  16,  15,  14,  13,  11,  /* SNR: 70 -> 79 */
	    10 };                                                /* SNR: 80 */
#if (BVDC_P_SUPPORT_MANR_VER == BVDC_P_MANR_VER_1)
#define JJW                          (0)
#define VERY_HIGH_NOISE_LEVEL        (8726)  /*set to 21db noise level expectation, which is out of range of the test*/
#else
#define JJW                          (1)
#define VERY_HIGH_NOISE_LEVEL        (10) /* cause to not do any filter to avoid side effect, it does not help anywhere */
#endif

#endif

/* This function is currently shared between stand alone ANR and ANR inside MCVP */
void BVDC_P_Anr_BuildRul_SrcInit_isr
	( BVDC_P_Anr_Handle              hAnr,
	  BVDC_P_ListInfo               *pList,
	  BVDC_P_PictureNode            *pPicture)
{
	uint32_t ulRegOffset;
	uint32_t ulHSize, ulVSize, ulStride, ulBufSize;
	uint32_t ulTopCapBufAddr=0, ulTopVfdBufAddr=0, ulBotCapBufAddr=0, ulBotVfdBufAddr=0;
	uint32_t ulDemoSetting;
	int  ii;

	BDBG_OBJECT_ASSERT(hAnr, BVDC_ANR);
	ulRegOffset = hAnr->ulRegOffset;

	ulHSize = hAnr->ulHSize;
	ulVSize = hAnr->ulVSize;

	hAnr->bEnableMaaf = VIDEO_FORMAT_IS_SECAM(hAnr->pCurSrcInfo->pFmtInfo->eVideoFmt);

	/* AND Init  */
	BVDC_P_SUBRUL_START_BLOCK(pList, BCHP_HD_ANR_AND_0_AND_MODE, ulRegOffset,
		BVDC_P_REGS_ENTRIES(HD_ANR_AND_0_AND_MODE, HD_ANR_AND_0_CONTENT_TH));
	*pList->pulCurrent++ = 0x01; /*AND_0.AND_MODE */
#if (BVDC_P_SUPPORT_ANR_VER < BVDC_P_ANR_VER_2) || \
	(BVDC_P_SUPPORT_MANR)
	/* 8-bit mode:*/
	*pList->pulCurrent++ = 0x0000; /*AND_0.NOISE_LOWER_TH_0 '0 */
	*pList->pulCurrent++ = 0x001C; /*AND_0.NOISE_LOWER_TH_1 '28 */
	*pList->pulCurrent++ = 0x0039; /*AND_0.NOISE_LOWER_TH_2 '57 */
	*pList->pulCurrent++ = 0x0072; /*AND_0.NOISE_LOWER_TH_3 '114 */
	*pList->pulCurrent++ = 0x00E4; /*AND_0.NOISE_LOWER_TH_4 '228 */
	*pList->pulCurrent++ = 0x0037; /*AND_0.NOISE_UPPER_TH_0 '55 */
	*pList->pulCurrent++ = 0x006D; /*AND_0.NOISE_UPPER_TH_1 '109 */
	*pList->pulCurrent++ = 0x00DA; /*AND_0.NOISE_UPPER_TH_2 '218 */
	*pList->pulCurrent++ = 0x01B0; /*AND_0.NOISE_UPPER_TH_3 '432 */
	*pList->pulCurrent++ = 0x035F; /*AND_0.NOISE_UPPER_TH_4 '863 */
	if(hAnr->pCurSrcInfo->pVdcFmt->bHd)
	{
		/* HD */
		*pList->pulCurrent++ = 0x07;  /*JJW*/ /*AND_0.EDGE_TH */
		*pList->pulCurrent++ = 0x0C;  /*JJW*/ /*AND_0.CONTENT_TH '&h14& in chip sim*/
	}
	else
	{
		/* SD or ED */
		*pList->pulCurrent++ = 0x0F;  /*JJW*/ /*AND_0.EDGE_TH */
		*pList->pulCurrent++ = 0x54;  /*JJW*/ /*AND_0.CONTENT_TH '&h14& in chip sim*/
	}
#else
	/*10-bit mode:*/
	*pList->pulCurrent++ = 0x0000; /*AND_0.NOISE_LOWER_TH_0 '0*/
	*pList->pulCurrent++ = 0x0069; /*AND_0.NOISE_LOWER_TH_1 '105*/
	*pList->pulCurrent++ = 0x00D1; /*AND_0.NOISE_LOWER_TH_2 '209*/
	*pList->pulCurrent++ = 0x01A2; /*AND_0.NOISE_LOWER_TH_3 '418*/
	*pList->pulCurrent++ = 0x0342; /*AND_0.NOISE_LOWER_TH_4 '834*/
	*pList->pulCurrent++ = 0x00E0; /*AND_0.NOISE_UPPER_TH_0 '224*/
	*pList->pulCurrent++ = 0x01BF; /*AND_0.NOISE_UPPER_TH_1 '447*/
	*pList->pulCurrent++ = 0x037C; /*AND_0.NOISE_UPPER_TH_2 '892*/
	*pList->pulCurrent++ = 0x06F5; /*AND_0.NOISE_UPPER_TH_3 '1781*/
	*pList->pulCurrent++ = 0x0DE0; /*AND_0.NOISE_UPPER_TH_4 '3552*/
	if(hAnr->pCurSrcInfo->pVdcFmt->bHd)
	{
		/* HD */
		*pList->pulCurrent++ = 0x1E;  /*JJW*/ /*AND_0.EDGE_TH */
		*pList->pulCurrent++ = 0x54;  /*JJW*/ /*AND_0.CONTENT_TH '&h14& in chip sim*/
	}
	else
	{
		/* SD or ED */
		*pList->pulCurrent++ = 0x78;   /*JJW*/ /*AND_0.EDGE_TH */
		*pList->pulCurrent++ = 0x150;  /*JJW*/ /*AND_0.CONTENT_TH '&h14& in chip sim*/
	}
#endif

	/* MCTF init */
	if(hAnr->stTestFeature1.bEnable)
	{
		ulBufSize = (ulHSize * ulVSize * hAnr->stTestFeature1.ulBitsPerPixel)
			>> BVDC_P_TestFeature1_FRACTIONAL_SHIFT;
		ulBufSize += 16*1024 + 64;
		/* In bytes */
		ulBufSize /= 8;
	}
	else
	{
		BPXL_GetBytesPerNPixels(hAnr->ePxlFmt, ulHSize, &ulStride);
		ulBufSize = ulVSize * ulStride;
	}

	ulDemoSetting =
		((BVDC_SplitScreenMode_eDisable == hAnr->eDemoMode)?
		 BCHP_FIELD_ENUM(HD_ANR_MCTF_0_CONT_0_DEMO_SETTING, CTRL, DISABLE) :
		 BCHP_FIELD_ENUM(HD_ANR_MCTF_0_CONT_0_DEMO_SETTING, CTRL, ENABLE))    |
		((BVDC_SplitScreenMode_eLeft == hAnr->eDemoMode)?
		 BCHP_FIELD_ENUM(HD_ANR_MCTF_0_CONT_0_DEMO_SETTING, DEMO_L_R, LEFT) :
		 BCHP_FIELD_ENUM(HD_ANR_MCTF_0_CONT_0_DEMO_SETTING, DEMO_L_R, RIGHT)) |
		BCHP_FIELD_DATA(HD_ANR_MCTF_0_CONT_0_DEMO_SETTING, DEMO_BOUNDARY, ulHSize / 2);

	/* memory saving mode will share deinterlacer's memory! */
#if BVDC_P_SUPPORT_ANR_MAD_MEM_SAVING_MODE
	if(!hAnr->bMemSaving)
#endif
	{
		bool  bInterlaced;
		/* Memory must be enough! */
		BDBG_ASSERT(hAnr->apHeapNode[0]->pHeapInfo->ulBufSize >= ulBufSize);

		bInterlaced = (BAVC_Polarity_eFrame != pPicture->PicComRulInfo.eSrcOrigPolarity);

#if (BVDC_P_DCX_ANR_CROSS_OVER_WORKAROUND)
		if(hAnr->stTestFeature1.bEnable)
		{
			if(hAnr->bSplitBuf)
			{
				uint32_t   ulHeapSize;

				/* Split anr buffers */
				BVDC_P_BufferHeap_GetHeapSizeById(hAnr->hHeap, hAnr->eBufHeapId, &ulHeapSize);
				ulTopCapBufAddr = BVDC_P_BUFFERHEAP_GetDeviceOffset(hAnr->apHeapNode[0]);
				ulTopVfdBufAddr = ulTopCapBufAddr + ulHeapSize / 2;
				ulBotCapBufAddr = bInterlaced
					? BVDC_P_BUFFERHEAP_GetDeviceOffset(hAnr->apHeapNode[1])
					: BVDC_P_BUFFERHEAP_GetDeviceOffset(hAnr->apHeapNode[0]);
				ulBotVfdBufAddr = ulBotCapBufAddr + ulHeapSize / 2;
			}
			else
			{
				if(bInterlaced)
				{
					/* interlaced */
					ulTopCapBufAddr = BVDC_P_BUFFERHEAP_GetDeviceOffset(hAnr->apHeapNode[0]);
					ulTopVfdBufAddr = BVDC_P_BUFFERHEAP_GetDeviceOffset(hAnr->apHeapNode[1]);
					ulBotCapBufAddr = BVDC_P_BUFFERHEAP_GetDeviceOffset(hAnr->apHeapNode[2]);
					ulBotVfdBufAddr = BVDC_P_BUFFERHEAP_GetDeviceOffset(hAnr->apHeapNode[3]);
				}
				else
				{
					/* progressive */
					ulTopCapBufAddr = BVDC_P_BUFFERHEAP_GetDeviceOffset(hAnr->apHeapNode[0]);
					ulTopVfdBufAddr = BVDC_P_BUFFERHEAP_GetDeviceOffset(hAnr->apHeapNode[1]);
					ulBotCapBufAddr = ulTopCapBufAddr;
					ulBotVfdBufAddr = ulTopVfdBufAddr;
				}
			}
		}
		else
#endif
		{
			ulTopCapBufAddr = BVDC_P_BUFFERHEAP_GetDeviceOffset(hAnr->apHeapNode[0]);
			ulTopVfdBufAddr = ulTopCapBufAddr;
			ulBotCapBufAddr = bInterlaced
				? BVDC_P_BUFFERHEAP_GetDeviceOffset(hAnr->apHeapNode[1])
				: BVDC_P_BUFFERHEAP_GetDeviceOffset(hAnr->apHeapNode[0]);
			ulBotVfdBufAddr = ulBotCapBufAddr;
		}

#if (BVDC_P_SUPPORT_ANR_VER == BVDC_P_ANR_VER_3) && (BCHP_MAD_SIOB_0_MODE)
		/* MAD_SIOB_0_MODE: programmed by MAD if mem saving mode! */
		BVDC_P_SUBRUL_ONE_REG(pList, BCHP_MAD_SIOB_0_MODE, ulRegOffset,
				/* 2 - SEL_MAD_MCTF; 1 - SEL_MCTF_ONLY; 0 - SEL_MAD_ONLY; */
				BCHP_FIELD_ENUM(MAD_SIOB_0_MODE, MAD_MCTF_SEL, SEL_MCTF_ONLY));
#endif
	}

	/* the following are coded according to mctf_regs.scr */
	BVDC_P_SUBRUL_START_BLOCK(pList, BCHP_HD_ANR_MCTF_0_CONT_0_MC_FILTER_COST_PARAME, ulRegOffset,
		BVDC_P_REGS_ENTRIES(HD_ANR_MCTF_0_CONT_0_MC_FILTER_COST_PARAME, HD_ANR_MCTF_0_CONT_0_MC_ALPHA_CALC_PARAME));
	*pList->pulCurrent++ = 0x01d403a8;   /* MC_FILTER_COST_PARAME */
	*pList->pulCurrent++ = BVDC_P_MC_ALPHA_CALC_PARAME_INIT_SETTING;   /* MC_ALPHA_CALC_PARAME */

	BVDC_P_SUBRUL_START_BLOCK(pList, BCHP_HD_ANR_MCTF_0_CONT_0_NMC_FILTER_COST_PARAME, ulRegOffset,
		BVDC_P_REGS_ENTRIES(HD_ANR_MCTF_0_CONT_0_NMC_FILTER_COST_PARAME, HD_ANR_MCTF_0_CONT_0_BOTTOM_MSTART_1));
	*pList->pulCurrent++ = 0x01d403a8;   /* NMC_FILTER_COST_PARAME */
	*pList->pulCurrent++ = BVDC_P_NMC_ALPHA_CALC_PARAME_INIT_SETTING;   /* NMC_ALPHA_CALC_PARAME */
	*pList->pulCurrent++ = 0x00000075;   /* MV_CONFID_PARAME */
	*pList->pulCurrent++ = BVDC_P_MC_NMC_ALPHA_CALC_PARAME_INIT_SETTING;   /* MC_NMC_ALPHA_CALC_PARAME */
	*pList->pulCurrent++ = 0x0000001c;   /* ALPHA_LOW_THRESHOLD */
	*pList->pulCurrent++ =               /* BVB_IN_SIZE */
		BCHP_FIELD_DATA(HD_ANR_MCTF_0_CONT_0_BVB_IN_SIZE, HSIZE, ulHSize) |
		BCHP_FIELD_DATA(HD_ANR_MCTF_0_CONT_0_BVB_IN_SIZE, VSIZE, ulVSize);
	*pList->pulCurrent++ = ulDemoSetting;/* DEMO_SETTING */
	*pList->pulCurrent++ = ulTopCapBufAddr; /* FRAME_OR_TOP_MSTART_0 */
	*pList->pulCurrent++ = ulTopVfdBufAddr; /* FRAME_OR_TOP_MSTART_1 */
	*pList->pulCurrent++ = ulBotCapBufAddr; /* BOTTOM_MSTART_0 */
	*pList->pulCurrent++ = ulBotVfdBufAddr; /* BOTTOM_MSTART_1 */

#if (BVDC_P_SUPPORT_ANR_VER == BVDC_P_ANR_VER_2)
	BVDC_P_SUBRUL_ONE_REG(pList, BCHP_HD_ANR_MCTF_0_CONT_0_PITCH, ulRegOffset, ulStride);  /* PITCH */
#endif

	/* control registers */
	BVDC_P_SUBRUL_ONE_REG(pList, BCHP_HD_ANR_MCTF_0_CONT_0_ME_CTRL, ulRegOffset, 0x3);  /* SEL_7X5 */
	BVDC_P_SUBRUL_ONE_REG(pList, BCHP_HD_ANR_MCTF_0_CONT_0_MC_CTRL, ulRegOffset, 0x3);  /* SEL_7X5 */
	BVDC_P_SUBRUL_ONE_REG(pList, BCHP_HD_ANR_MCTF_0_CONT_0_NMC_CTRL, ulRegOffset, 0x7); /* SSD(?)|SEL_7X5*/

	hAnr->ulTopCtrlReg =
#if (BVDC_P_SUPPORT_MANR_VER >= BVDC_P_MANR_VER_2)
		BCHP_FIELD_DATA(HD_ANR_MCTF_0_TOP_CTRL, BVB_VIDEO,
			BVDC_P_VNET_USED_ANR_AT_WRITER(pPicture->stVnetMode) ?
			pPicture->eSrcOrientation : pPicture->eDispOrientation) |
#endif
#if (BVDC_P_SUPPORT_ANR_VER >= BVDC_P_ANR_VER_3 || BVDC_P_SUPPORT_MANR)
#if (BCHP_HD_ANR_MCTF_0_TOP_CTRL_SCB_MODE_SEL_SHIFT)
		BCHP_FIELD_DATA(HD_ANR_MCTF_0_TOP_CTRL, SCB_MODE_SEL,
						(BPXL_IS_YCbCr422_10BIT_FORMAT(hAnr->ePxlFmt) ||
						 BPXL_IS_YCbCr422_10BIT_PACKED_FORMAT(hAnr->ePxlFmt)) ? 1 : 0   ) |
#endif
		BCHP_FIELD_DATA(HD_ANR_MCTF_0_TOP_CTRL, TF_OUT_CAP, !hAnr->bMemSaving ) |
#elif (BVDC_P_SUPPORT_ANR_VER >= BVDC_P_ANR_VER_2)
		BCHP_FIELD_DATA(HD_ANR_MCTF_0_TOP_CTRL, MEM_SAVING, hAnr->bMemSaving) |
		BCHP_FIELD_DATA(HD_ANR_MCTF_0_TOP_CTRL, SCB_MODE_SEL,
			(BPXL_IS_YCbCr422_10BIT_FORMAT(hAnr->ePxlFmt) ||
			 BPXL_IS_YCbCr422_10BIT_PACKED_FORMAT(hAnr->ePxlFmt)) ? 1 : 0   ) |
#endif
#if BVDC_P_SUPPORT_MANR
		BCHP_FIELD_ENUM(HD_ANR_MCTF_0_TOP_CTRL, RANGE_CHK,    ENABLE        ) |
#endif
		BCHP_FIELD_ENUM(HD_ANR_MCTF_0_TOP_CTRL, UPDATE_SEL,   NORMAL        ) |
		BCHP_FIELD_ENUM(HD_ANR_MCTF_0_TOP_CTRL, CONTEXT_CTRL, AUTO          ) |
		BCHP_FIELD_ENUM(HD_ANR_MCTF_0_TOP_CTRL, ENABLE_CTRL,  STOP_ON_FIELD_COMPLETION);

	BVDC_P_SUBRUL_START_BLOCK(pList, BCHP_HD_ANR_MCTF_0_VIP_OUTPUT_DRAIN, ulRegOffset,
		BVDC_P_REGS_ENTRIES(HD_ANR_MCTF_0_VIP_OUTPUT_DRAIN, HD_ANR_MCTF_0_TOP_CTRL));
	*pList->pulCurrent++ = 0x0;  /* VIP_OUTPUT_DRAIN:CTRL_DISABLE */
	*pList->pulCurrent++ = 0x0;  /* SCAD_OUTPUT_DRAIN:CTRL_DISABLE */
	*pList->pulCurrent++ = 0x0;  /* AND_OUTPUT_DRAIN:CTRL_DISABLE */
	*pList->pulCurrent++ = hAnr->ulTopCtrlReg; /* TOP_CTRL */

	BVDC_P_SUBRUL_ONE_REG(pList, BCHP_HD_ANR_MCTF_0_MANUAL_CTRL, ulRegOffset, 0x0);

#if (BVDC_P_SUPPORT_ANR_MAAF)
	/* MAAF Init */
	if (hAnr->bEnableMaaf)
	{
		BVDC_P_SUBRUL_START_BLOCK(pList, BCHP_HD_ANR_MCTF_0_CONT_0_MAAF_ALPHA_CALC_PARAME, ulRegOffset,
			BVDC_P_REGS_ENTRIES(HD_ANR_MCTF_0_CONT_0_MAAF_ALPHA_CALC_PARAME, HD_ANR_MCTF_0_CONT_0_MAAF_PIX_PARAME));
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(HD_ANR_MCTF_0_CONT_0_MAAF_ALPHA_CALC_PARAME, K0, BVDC_P_MAAF_ALPHA_CALC_PARAME_K0) |
			BCHP_FIELD_DATA(HD_ANR_MCTF_0_CONT_0_MAAF_ALPHA_CALC_PARAME, K1, BVDC_P_MAAF_ALPHA_CALC_PARAME_K1);
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(HD_ANR_MCTF_0_CONT_0_MAAF_ALPHA_THRESHOLD, MIN, BVDC_P_MAAF_ALPHA_THRESH_MIN) |
			BCHP_FIELD_DATA(HD_ANR_MCTF_0_CONT_0_MAAF_ALPHA_THRESHOLD, MAX, BVDC_P_MAAF_ALPHA_THRESH_MAX);
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(HD_ANR_MCTF_0_CONT_0_MAAF_ALPHA_THOLD_2D, MIN, BVDC_P_MAAF_ALPHA_THOLD_2D_MIN) |
			BCHP_FIELD_DATA(HD_ANR_MCTF_0_CONT_0_MAAF_ALPHA_THOLD_2D, MAX, BVDC_P_MAAF_ALPHA_THOLD_2D_MAX);
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(HD_ANR_MCTF_0_CONT_0_MAAF_PIX_PARAME, LUMA_THR,   BVDC_P_MAAF_PIX_PARAME_LUMA_THR)   |
			BCHP_FIELD_DATA(HD_ANR_MCTF_0_CONT_0_MAAF_PIX_PARAME, CHROMA_THR, BVDC_P_MAAF_PIX_PARAME_CHROMA_THR) |
			BCHP_FIELD_DATA(HD_ANR_MCTF_0_CONT_0_MAAF_PIX_PARAME, THR_SLOPE,  BVDC_P_MAAF_PIX_PARAME_THR_SLOPE);
	}

	/* MAAAF */
	BVDC_P_SUBRUL_ONE_REG(pList, BCHP_HD_ANR_MCTF_0_MAAF_CTRL, ulRegOffset,
		((hAnr->bEnableMaaf) ?
		  BCHP_FIELD_ENUM(HD_ANR_MCTF_0_MAAF_CTRL, ENABLE, ENABLE) :
		  BCHP_FIELD_ENUM(HD_ANR_MCTF_0_MAAF_CTRL, ENABLE, DISABLE)));
#endif

	/* AND threshold numbers, used in StatisRead */
	hAnr->ulAndThdScl = ((ulHSize * ulVSize) << BVDC_P_AndThdScl_FRAC) / (720 * 240);
	hAnr->lSnDb = BVDC_P_SNR_MAX;

	/* as previous NOISE_LEVEL_HISTORY reference, used when current noise level reading is out of range */
	for (ii=0; ii<BVDC_P_NOISE_LVL_AVG_SIZE; ii++)
		hAnr->aulNoiseLevelHistory[ii] = 0;
	hAnr->ulIndex = 0; /* current index to the above noise level history array */
	hAnr->ulNoiseLevelHistorySum = 0;

	hAnr->ulNoiseNumHistorySum = 1024 * 8;
	for(ii = 0; ii<BVDC_P_NOISE_NUM_AVG_SIZE; ii++)
		hAnr->aulNoiseNumHistory[ii] = 1024;
	hAnr->ulIndex2 = 0;
	hAnr->uPreviousLowNoise = 0;
}

void BVDC_P_Anr_BuildRul_SetEnable_isr
	( BVDC_P_Anr_Handle              hAnr,
	  BVDC_P_PictureNode            *pPicture,
	  BVDC_P_ListInfo               *pList,
	  bool                           bEnable,
	  BAVC_Polarity                  eSrcNxtFldId,
	  bool                           bRepeat,
	  bool                           bInit )
{
	uint32_t  ulRegOffset, ulDemoSetting;
	uint32_t  ulTopCtrl;
#if (!HW3548_47_IS_FIXED)
	BSTD_UNUSED(bRepeat);
#endif

	BDBG_OBJECT_ASSERT(hAnr, BVDC_ANR);
	ulRegOffset = hAnr->ulRegOffset;
	ulTopCtrl = hAnr->ulTopCtrlReg;

	if ((bEnable)
#if (BVDC_P_SUPPORT_ANR_VER < BVDC_P_ANR_VER_3)
		&&
		(BVDC_FilterMode_eBypass != hAnr->pAnrSetting->eMode)
#endif
		)
	{
		/* AND */
		BVDC_P_SUBRUL_ONE_REG(pList, BCHP_HD_ANR_AND_0_AND_ENABLE, ulRegOffset, 0x1); /* enable */

		/* MCTF */
		ulDemoSetting =
			((BVDC_SplitScreenMode_eDisable == hAnr->eDemoMode)?
			 BCHP_FIELD_ENUM(HD_ANR_MCTF_0_CONT_0_DEMO_SETTING, CTRL, DISABLE) :
			 BCHP_FIELD_ENUM(HD_ANR_MCTF_0_CONT_0_DEMO_SETTING, CTRL, ENABLE))    |
			((BVDC_SplitScreenMode_eLeft == hAnr->eDemoMode)?
			 BCHP_FIELD_ENUM(HD_ANR_MCTF_0_CONT_0_DEMO_SETTING, DEMO_L_R, LEFT) :
			 BCHP_FIELD_ENUM(HD_ANR_MCTF_0_CONT_0_DEMO_SETTING, DEMO_L_R, RIGHT)) |
			BCHP_FIELD_DATA(HD_ANR_MCTF_0_CONT_0_DEMO_SETTING, DEMO_BOUNDARY, hAnr->ulHSize / 2);
		BVDC_P_SUBRUL_ONE_REG(pList, BCHP_HD_ANR_MCTF_0_CONT_0_DEMO_SETTING,
			ulRegOffset, ulDemoSetting);

		/* ulAutoCtrlReg is also saved for later used */
		hAnr->ulAutoCtrlReg =
			(BVDC_P_ANR_NEED_HARD_START(hAnr) ?
			 BCHP_FIELD_DATA(HD_ANR_MCTF_0_AUTO_CTRL, HARD_START, 1 ) :
			 BCHP_FIELD_DATA(HD_ANR_MCTF_0_AUTO_CTRL, HARD_START, 0 )               ) |
#if (BVDC_P_SUPPORT_ANR)
			(BCHP_FIELD_DATA(HD_ANR_MCTF_0_AUTO_CTRL, CONTEXT,    0 )               ) |
#endif
			((BAVC_Polarity_eBotField == eSrcNxtFldId) ?
			 BCHP_FIELD_ENUM(HD_ANR_MCTF_0_AUTO_CTRL, PIC_TYPE, BOTTOM_FIELD) :
			 BCHP_FIELD_ENUM(HD_ANR_MCTF_0_AUTO_CTRL, PIC_TYPE, FRAME_OR_TOP_FIELD) );
		BVDC_P_SUBRUL_ONE_REG(pList, BCHP_HD_ANR_MCTF_0_AUTO_CTRL, ulRegOffset, hAnr->ulAutoCtrlReg);

#if (BVDC_P_SUPPORT_ANR_VER >= BVDC_P_ANR_VER_3 || BVDC_P_SUPPORT_MANR)
		BVDC_P_SUBRUL_ONE_REG(pList, BCHP_HD_ANR_MCTF_0_MAD_OUT1_DRAIN, ulRegOffset,
			((hAnr->bMemSaving)?
			 BCHP_FIELD_ENUM(HD_ANR_MCTF_0_MAD_OUT1_DRAIN, CTRL, DISABLE) :
			 BCHP_FIELD_ENUM(HD_ANR_MCTF_0_MAD_OUT1_DRAIN, CTRL, ENABLE)));

		/* PR48136: ANR bypass workaround - tune off filter effect by setting blendor factor K1 as 0 */
		if(BVDC_FilterMode_eBypass == hAnr->pAnrSetting->eMode)
		{
			BVDC_P_SUBRUL_ONE_REG(pList, BCHP_HD_ANR_MCTF_0_CONT_0_MC_ALPHA_CALC_PARAME, ulRegOffset,
				BVDC_P_MC_ALPHA_CALC_PARAME_INIT_SETTING &
				(~(BCHP_MASK(HD_ANR_MCTF_0_CONT_0_MC_ALPHA_CALC_PARAME, K1)) ));
			BVDC_P_SUBRUL_ONE_REG(pList, BCHP_HD_ANR_MCTF_0_CONT_0_NMC_ALPHA_CALC_PARAME, ulRegOffset,
				BVDC_P_NMC_ALPHA_CALC_PARAME_INIT_SETTING &
				(~(BCHP_MASK(HD_ANR_MCTF_0_CONT_0_NMC_ALPHA_CALC_PARAME, K1)) ));
			BVDC_P_SUBRUL_ONE_REG(pList, BCHP_HD_ANR_MCTF_0_CONT_0_MC_NMC_ALPHA_CALC_PARAME, ulRegOffset,
				BVDC_P_MC_NMC_ALPHA_CALC_PARAME_INIT_SETTING &
				(~(BCHP_MASK(HD_ANR_MCTF_0_CONT_0_MC_NMC_ALPHA_CALC_PARAME, K1)) ));
		}

#if (HW3548_47_IS_FIXED)
		/* mctf trick mode suport */
		ulTopCtrl =
			(hAnr->ulTopCtrlReg & ~(BCHP_MASK(HD_ANR_MCTF_0_TOP_CTRL, TF_OUT_CAP))) |
			BCHP_FIELD_DATA(HD_ANR_MCTF_0_TOP_CTRL, TF_OUT_CAP,
				(bRepeat || hAnr->bMemSaving)? 0 : 1);
#endif
#else
	BSTD_UNUSED(bRepeat);
#endif
		if(!bInit)
			BVDC_P_SUBRUL_ONE_REG(pList, BCHP_HD_ANR_MCTF_0_TOP_CTRL, ulRegOffset, ulTopCtrl);
		BVDC_P_SUBRUL_ONE_REG(pList, BCHP_HD_ANR_MCTF_0_ENABLE, ulRegOffset,
			BCHP_FIELD_ENUM(HD_ANR_MCTF_0_ENABLE, ENABLE, ON));

#if (BVDC_P_SUPPORT_VIDEO_TESTFEATURE1_MAD_ANR)
		/* Programe TestFeature1 here only in !bMemSaving mode, otherwise, programe
		 * it in MAD */
		if(!hAnr->bMemSaving)
		{
			BVDC_P_TestFeature1_Settings   *pTestFeature1 = &hAnr->stTestFeature1;

#if (BVDC_P_SUPPORT_MAD)
			BVDC_P_SUBRUL_START_BLOCK(pList, BCHP_MAD_SIOB_0_DCX_PRED_CFG, ulRegOffset,
				((BCHP_MAD_SIOB_0_DCX_COMPR_CFG1 - BCHP_MAD_SIOB_0_DCX_PRED_CFG) / 4) + 1);

			/* MAD_SIOB_0_DCX_PRED_CFG */
			*pList->pulCurrent++ = (
				BCHP_FIELD_DATA(MAD_SIOB_0_DCX_PRED_CFG, ENABLE,          pTestFeature1->bEnable    ) |
				BCHP_FIELD_ENUM(MAD_SIOB_0_DCX_PRED_CFG, CONVERT_RGB,     Disable ) |
				BCHP_FIELD_DATA(MAD_SIOB_0_DCX_PRED_CFG, PREDICTION_MODE, pTestFeature1->ulPredictionMode) |
				BCHP_FIELD_ENUM(MAD_SIOB_0_DCX_PRED_CFG, EDGE_PRED_ENA,   Enable  ) |
				BCHP_FIELD_ENUM(MAD_SIOB_0_DCX_PRED_CFG, LEFT_PRED_ENA,   Enable  ) |
				BCHP_FIELD_ENUM(MAD_SIOB_0_DCX_PRED_CFG, ABCD_PRED_ENA,   Enable  ) |
				BCHP_FIELD_ENUM(MAD_SIOB_0_DCX_PRED_CFG, LS_PRED_ENA,     Enable  ));

			/* MAD_SIOB_0_DCX_COMPR_CFG1 */
			*pList->pulCurrent++ = (
				BCHP_FIELD_DATA(MAD_SIOB_0_DCX_COMPR_CFG1, PIXELS_PER_GROUP,  pTestFeature1->ulPixelPerGroup) |
				BCHP_FIELD_DATA(MAD_SIOB_0_DCX_COMPR_CFG1, TGT_OFFSET_HI,     0xfa   ) |
				BCHP_FIELD_DATA(MAD_SIOB_0_DCX_COMPR_CFG1, TGT_OFFSET_LO,     12     ) |
				BCHP_FIELD_DATA(MAD_SIOB_0_DCX_COMPR_CFG1, TGT_BPG, pTestFeature1->ulBitsPerGroup));
#elif (BVDC_P_SUPPORT_MCVP)
			BVDC_P_SUBRUL_START_BLOCK(pList, BCHP_SIOB_0_DCX_PRED_CFG, ulRegOffset,
				((BCHP_SIOB_0_DCX_COMPR_CFG1 - BCHP_SIOB_0_DCX_PRED_CFG) / 4) + 1);

			/* SIOB_0_DCX_PRED_CFG */
			*pList->pulCurrent++ = (
				BCHP_FIELD_DATA(SIOB_0_DCX_PRED_CFG, ENABLE,          pTestFeature1->bEnable    ) |
				BCHP_FIELD_ENUM(SIOB_0_DCX_PRED_CFG, CONVERT_RGB,     Disable ) |
				BCHP_FIELD_DATA(SIOB_0_DCX_PRED_CFG, PREDICTION_MODE, pTestFeature1->ulPredictionMode) |
				BCHP_FIELD_ENUM(SIOB_0_DCX_PRED_CFG, EDGE_PRED_ENA,   Enable  ) |
				BCHP_FIELD_ENUM(SIOB_0_DCX_PRED_CFG, LEFT_PRED_ENA,   Enable  ) |
				BCHP_FIELD_ENUM(SIOB_0_DCX_PRED_CFG, ABCD_PRED_ENA,   Enable  ) |
				BCHP_FIELD_ENUM(SIOB_0_DCX_PRED_CFG, LS_PRED_ENA,     Enable  ));

			/* SIOB_0_DCX_COMPR_CFG1 */
			*pList->pulCurrent++ = (
				BCHP_FIELD_DATA(SIOB_0_DCX_COMPR_CFG1, PIXELS_PER_GROUP,  pTestFeature1->ulPixelPerGroup) |
				BCHP_FIELD_DATA(SIOB_0_DCX_COMPR_CFG1, TGT_OFFSET_HI,     0xfa   ) |
				BCHP_FIELD_DATA(SIOB_0_DCX_COMPR_CFG1, TGT_OFFSET_LO,     12     ) |
				BCHP_FIELD_DATA(SIOB_0_DCX_COMPR_CFG1, TGT_BPG, pTestFeature1->ulBitsPerGroup));
#endif
		}
#endif

#ifdef BCHP_HD_ANR_0_DOWNSAMPLE_CTL
		/* 444 <-> 422 */
		BDBG_OBJECT_ASSERT(pPicture->hBuffer->hWindow, BVDC_WIN);
		BDBG_OBJECT_ASSERT(pPicture->hBuffer->hWindow->stCurInfo.hSource, BVDC_SRC);

		if(pPicture->hBuffer->hWindow->stCurInfo.hSource->bSrcIs444)
		{
			if(BVDC_P_VNET_USED_ANR_AT_WRITER(pPicture->hBuffer->hWindow->stVnetMode))
			{
				/* SRC -> ANR */
				hAnr->stDnSampler.eFilterType = BVDC_444To422Filter_eStandard;
			}
			else
			{
				/* SRC -> CAP -> VFD -> ANR */
				if(pPicture->hBuffer->hWindow->stCurResource.hCapture->eCapDataMode == BVDC_P_Capture_DataMode_e10Bit444)
				{
					hAnr->stDnSampler.eFilterType = BVDC_444To422Filter_eStandard;
				}
				else
				{
					hAnr->stDnSampler.eFilterType = BVDC_444To422Filter_eDecimate;
				}
			}
		}
		else
		{
			hAnr->stDnSampler.eFilterType = BVDC_444To422Filter_eDecimate;
		}

		BVDC_P_SUBRUL_ONE_REG(pList, BCHP_HD_ANR_0_DOWNSAMPLE_CTL, ulRegOffset,
			BCHP_FIELD_DATA(HD_ANR_0_DOWNSAMPLE_CTL, FILTER_TYPE,
				hAnr->stDnSampler.eFilterType) |
			((BVDC_444To422Filter_eStandard == hAnr->stDnSampler.eFilterType)
			? BCHP_FIELD_ENUM(HD_ANR_0_DOWNSAMPLE_CTL, RING_SUPPRESSION, ENABLE)
			: BCHP_FIELD_ENUM(HD_ANR_0_DOWNSAMPLE_CTL, RING_SUPPRESSION, DISABLE)));

		if((hAnr->stUpSampler.eFilterType == BVDC_422To444Filter_eTenTaps) ||
		   (hAnr->stUpSampler.eFilterType == BVDC_422To444Filter_eSixTaps))
		{
			BVDC_P_SUBRUL_ONE_REG(pList, BCHP_HD_ANR_0_UPSAMPLE_CTL, ulRegOffset,
				BCHP_FIELD_ENUM(HD_ANR_0_UPSAMPLE_CTL, RING_SUPPION_MODE, DOUBLE) |
				BCHP_FIELD_ENUM(HD_ANR_0_UPSAMPLE_CTL, RING_SUPPION,      ENABLE) |
				BCHP_FIELD_DATA(HD_ANR_0_UPSAMPLE_CTL, UNBIASED_ROUND_ENABLE,
					hAnr->stUpSampler.bUnbiasedRound) |
				BCHP_FIELD_DATA(HD_ANR_0_UPSAMPLE_CTL, FILT_CTRL,
					hAnr->stUpSampler.eFilterType));
		}
		else
		{
			BVDC_P_SUBRUL_ONE_REG(pList, BCHP_HD_ANR_0_UPSAMPLE_CTL, ulRegOffset,
				BCHP_FIELD_ENUM(HD_ANR_0_UPSAMPLE_CTL, RING_SUPPION_MODE, NORMAL)  |
				BCHP_FIELD_ENUM(HD_ANR_0_UPSAMPLE_CTL, RING_SUPPION,      DISABLE) |
				BCHP_FIELD_DATA(HD_ANR_0_UPSAMPLE_CTL, UNBIASED_ROUND_ENABLE,
					hAnr->stUpSampler.bUnbiasedRound) |
				BCHP_FIELD_DATA(HD_ANR_0_UPSAMPLE_CTL, FILT_CTRL,
					hAnr->stUpSampler.eFilterType));
		}
#else
	BSTD_UNUSED(pPicture);
#endif

		/* top level */
#ifdef BCHP_HD_ANR_0_HD_ANR_CTRL
#if (BVDC_P_SUPPORT_ANR_VER >= BVDC_P_ANR_VER_2 || BVDC_P_SUPPORT_MANR)
		/* no double buffer control; Enable MCTF statistics;
		   user may dynamicaly switch on/off output filtering; */
		BVDC_P_SUBRUL_ONE_REG(pList, BCHP_HD_ANR_0_HD_ANR_CTRL, ulRegOffset,
			BCHP_FIELD_ENUM(HD_ANR_0_HD_ANR_CTRL, UPDATE_SEL, UPDATE_BY_PICTURE   ) | /* 1 is disabled */
			BCHP_FIELD_ENUM(HD_ANR_0_HD_ANR_CTRL, BYPASS_MCTF_DATA_ENABLE, ENABLE ) |
			BCHP_FIELD_DATA(HD_ANR_0_HD_ANR_CTRL, BYPASS_OUTPUT_DATA_ENABLE, hAnr->bBypassFilter) |
			BCHP_FIELD_DATA(HD_ANR_0_HD_ANR_CTRL, HD_ANR_ENABLE, !hAnr->bBypassFilter));
#else
		BVDC_P_SUBRUL_ONE_REG(pList, BCHP_HD_ANR_0_HD_ANR_CTRL, ulRegOffset,
			BCHP_FIELD_ENUM(HD_ANR_0_HD_ANR_CTRL, HD_ANR_ENABLE, NORMAL));
#endif
#endif

		/* next pic on-longer needs hard-start */
		hAnr->stSwDirty.stBits.bPrevBypass = BVDC_P_CLEAN;
	}
	else
	{
#ifdef BCHP_HD_ANR_0_HD_ANR_CTRL
#if (BVDC_P_SUPPORT_ANR_VER >= BVDC_P_ANR_VER_2 || BVDC_P_SUPPORT_MANR)
		/* no double buffer control; disable nosie detection; bypass filter */
		BVDC_P_SUBRUL_ONE_REG(pList, BCHP_HD_ANR_0_HD_ANR_CTRL, ulRegOffset,
			BCHP_FIELD_ENUM(HD_ANR_0_HD_ANR_CTRL, UPDATE_SEL, UPDATE_BY_PICTURE   ) | /* 1 is disabled */
			BCHP_FIELD_ENUM(HD_ANR_0_HD_ANR_CTRL, BYPASS_MCTF_DATA_ENABLE,   DISABLE) |
			BCHP_FIELD_ENUM(HD_ANR_0_HD_ANR_CTRL, BYPASS_OUTPUT_DATA_ENABLE, ENABLE ) |
			BCHP_FIELD_ENUM(HD_ANR_0_HD_ANR_CTRL, HD_ANR_ENABLE, BYPASS));
#else
		BVDC_P_SUBRUL_ONE_REG(pList, BCHP_HD_ANR_0_HD_ANR_CTRL, ulRegOffset,
			BCHP_FIELD_ENUM(HD_ANR_0_HD_ANR_CTRL, HD_ANR_ENABLE, BYPASS));
#endif
#endif

		BVDC_P_SUBRUL_ONE_REG(pList, BCHP_HD_ANR_MCTF_0_ENABLE, ulRegOffset,
			BCHP_FIELD_ENUM(HD_ANR_MCTF_0_ENABLE, ENABLE, OFF));
		BVDC_P_SUBRUL_ONE_REG(pList, BCHP_HD_ANR_AND_0_AND_ENABLE, ulRegOffset, 0x2);  /* drain */

#if (BVDC_P_SUPPORT_ANR_VER == BVDC_P_ANR_VER_3) && (BCHP_MAD_SIOB_0_MODE)
		if(!hAnr->bMemSaving)
		{
			/* MAD_SIOB_0_MODE: when ANR is disabled, MUST put SIOB back to default mode!
			   This is only necessary when it was MCTF-only mode; otherwise MAD will program SIOB block. */
			BVDC_P_SUBRUL_ONE_REG(pList, BCHP_MAD_SIOB_0_MODE, ulRegOffset,
				/* 2 - SEL_MAD_MCTF; 1 - SEL_MCTF_ONLY; 0 - SEL_MAD_ONLY; */
				BCHP_FIELD_ENUM(MAD_SIOB_0_MODE, MAD_MCTF_SEL, SEL_MAD_ONLY));
		}
#endif
		/* next pic will do hard-start if it changes to not bypass */
		hAnr->stSwDirty.stBits.bPrevBypass = BVDC_P_DIRTY;
	}
}

#if (BVDC_P_SUPPORT_ANR)

static int32_t BVDC_P_Anr_BinarySearch_isr
	( uint32_t                       ulNoiseLvlHistory )
{
	uint32_t  ulMin, ulMid, ulMax;

	ulMin = BVDC_P_SNR_MIN;
	ulMax = BVDC_P_SNR_MAX;
	while (ulMax > ulMin)
	{
		ulMid = (ulMax + ulMin) / 2;
		if (s_usNoiseLvlThresh[ulMid - BVDC_P_SNR_MIN] > ulNoiseLvlHistory)
			ulMin = ulMid + 1;
		else
			ulMax = ulMid;
	}

	return ulMax;
}

/***************************************************************************
 * {private}
 *
 * BVDC_P_Anr_BuildRul_StatisRead_isr
 *
 * It is called by BVDC_P_Anr_BuildRul_isr to read statistics HW registers
 * and then to dynamically adjust the filter strength.
 *
 * This happens at every multiple of BVDC_P_ANR_NUM_ACCUM fields or frames.
 *
 *'Noise Lower/Upper Thresholds are set in ANR_AND_VPP.bss
 * user_adj = InputBox("Filter Strength Adjustment:" & vbLF & "0: Normal" &
 * vbLF & "+1: Stronger Filter" & vbLF & "-1: Weaker Filter", "ANR Detection &
 * Filtering", 0)
 *
 */
void BVDC_P_Anr_BuildRul_StatisRead_isr
	( BVDC_P_Anr_Handle              hAnr,
	  BVDC_P_ListInfo               *pList,
	  bool                           bInit)
{
	BREG_Handle  hRegister;
	BVDC_P_Source_Info  *pCurSrcInfo;
	uint32_t  ulRegOffset;
	uint32_t  NOISY_SAMPLES[5];             /*number of noisy samples */
	uint32_t  NOISE_LEVEL_MSB[5];           /* noise level msb of noise ranges 0 to 4 */
	uint32_t  NOISE_LEVEL[5];               /* noise level lsb of noise ranges 0 to 4 */
	uint32_t  NOISE_LEVEL_HISTORY, NOISE_LEVEL_HISTORY_AVG;
	uint32_t  ulNoiseLvlReadHigh, ulNoiseLvlHigh, ulNoiseLvlReadResidue;
	uint32_t  ulNoiseLvlReadLow, ulNoiseLvlLow;
	int i, k, m;
	int32_t  lSnDb;
	uint32_t  ulNumNoisySampleThdBig, ulNumDiffThd, ulNumNoisySampleThd, ulAndThdScl;
	const BVDC_P_AnrKValue  *pK1, *pK2;

	BDBG_OBJECT_ASSERT(hAnr, BVDC_ANR);
	pCurSrcInfo = hAnr->pCurSrcInfo;

	/* ulAndThdScl is less than ((1920 * 1088) << 11) / (720 * 240) = 0x60B6 */
	ulAndThdScl = hAnr->ulAndThdScl;
	ulNumNoisySampleThd = (NUM_NOISY_SAMPLE_THD * ulAndThdScl) >> BVDC_P_AndThdScl_FRAC;
	ulNumNoisySampleThdBig = (NUM_NOISY_SAMPLE_THD_BIG * ulAndThdScl) >> BVDC_P_AndThdScl_FRAC;
	ulNumDiffThd = (NUMDIFF_THD * ulAndThdScl) >> BVDC_P_AndThdScl_FRAC;
	BDBG_P_ANR_MSG(("ulAndThdScl 0x%x.%03lx", ulAndThdScl >> BVDC_P_AndThdScl_FRAC,
					ulAndThdScl & ((1 << BVDC_P_AndThdScl_FRAC) - 1)));

	hRegister = hAnr->hRegister;
	ulRegOffset = hAnr->ulRegOffset;

	/* Read and keep the BVB status to be extracted and used by other modules */
	hAnr->ulBvbStatus = BREG_Read32( hRegister, BCHP_HD_ANR_MCTF_0_BVB_IN_STATUS + ulRegOffset);

	/* 'collect values of noisy samples and noise levels so that they dont change during script */
	NOISY_SAMPLES[0] = BREG_Read32( hRegister, BCHP_HD_ANR_AND_0_NOISY_SAMPLE_NUM_BIN_0 + ulRegOffset );
	NOISY_SAMPLES[1] = BREG_Read32( hRegister, BCHP_HD_ANR_AND_0_NOISY_SAMPLE_NUM_BIN_1 + ulRegOffset );
	NOISY_SAMPLES[2] = BREG_Read32( hRegister, BCHP_HD_ANR_AND_0_NOISY_SAMPLE_NUM_BIN_2 + ulRegOffset );
	NOISY_SAMPLES[3] = BREG_Read32( hRegister, BCHP_HD_ANR_AND_0_NOISY_SAMPLE_NUM_BIN_3 + ulRegOffset );
	NOISY_SAMPLES[4] = BREG_Read32( hRegister, BCHP_HD_ANR_AND_0_NOISY_SAMPLE_NUM_BIN_4 + ulRegOffset );
	NOISE_LEVEL_MSB[0] = BREG_Read32( hRegister, BCHP_HD_ANR_AND_0_NOISE_LEVEL_0_MSB + ulRegOffset );
	NOISE_LEVEL_MSB[1] = BREG_Read32( hRegister, BCHP_HD_ANR_AND_0_NOISE_LEVEL_1_MSB + ulRegOffset );
	NOISE_LEVEL_MSB[2] = BREG_Read32( hRegister, BCHP_HD_ANR_AND_0_NOISE_LEVEL_2_MSB + ulRegOffset );
	NOISE_LEVEL_MSB[3] = BREG_Read32( hRegister, BCHP_HD_ANR_AND_0_NOISE_LEVEL_3_MSB + ulRegOffset );
	NOISE_LEVEL_MSB[4] = BREG_Read32( hRegister, BCHP_HD_ANR_AND_0_NOISE_LEVEL_4_MSB + ulRegOffset );
	NOISE_LEVEL[0] = BREG_Read32( hRegister, BCHP_HD_ANR_AND_0_NOISE_LEVEL_0_LSB + ulRegOffset );
	NOISE_LEVEL[1] = BREG_Read32( hRegister, BCHP_HD_ANR_AND_0_NOISE_LEVEL_1_LSB + ulRegOffset );
	NOISE_LEVEL[2] = BREG_Read32( hRegister, BCHP_HD_ANR_AND_0_NOISE_LEVEL_2_LSB + ulRegOffset );
	NOISE_LEVEL[3] = BREG_Read32( hRegister, BCHP_HD_ANR_AND_0_NOISE_LEVEL_3_LSB + ulRegOffset );
	NOISE_LEVEL[4] = BREG_Read32( hRegister, BCHP_HD_ANR_AND_0_NOISE_LEVEL_4_LSB + ulRegOffset );

	/* ------------------------------------------------------------
	 * (1).  compute NOISE_LEVEL_HISTORY
	 */
	for (k = 0; k < 5; k++) /* averaging function */
	{
		if (NOISY_SAMPLES[k] > 0)
		{
			/* multiple 8 for 3 bits fraction */
			BDBG_P_ANR_MSG(("NL Reg %d: MSB %d, LSB %d, NumSam %d -> %d",
					  k, NOISE_LEVEL_MSB[k], NOISE_LEVEL[k], NOISY_SAMPLES[k],
					  (((NOISE_LEVEL_MSB[k] << (32 - 5)) | (NOISE_LEVEL[k] >> 5)) / NOISY_SAMPLES[k]) << 8));
			/* NOISE_LEVEL_MSB[k] has max 5 bits, NOISY_SAMPLES[k] has max 1920*1080, 21 bits
			 * ulNoiseLvlReadHigh < 3+5+22 = 30 bits, ulNoiseLvlReadLow < 21+10 = 31 bits */
			ulNoiseLvlReadHigh = 8 * ((NOISE_LEVEL_MSB[k] << (32 - 10)) | (NOISE_LEVEL[k] >> 10));
			ulNoiseLvlHigh = ulNoiseLvlReadHigh / NOISY_SAMPLES[k];
			ulNoiseLvlReadResidue = ulNoiseLvlReadHigh - ulNoiseLvlHigh * NOISY_SAMPLES[k];
			ulNoiseLvlReadLow = (ulNoiseLvlReadResidue << 10) + 8 * (NOISE_LEVEL[k] & ((1 << 10) - 1));
			ulNoiseLvlLow = ulNoiseLvlReadLow / NOISY_SAMPLES[k];
			NOISE_LEVEL[k] = (ulNoiseLvlHigh << 10) + ulNoiseLvlLow;
			BDBG_P_ANR_MSG(("Avrg NoiseLvlRegSet %d: %d", k, NOISE_LEVEL[k]));
		}
	}

	i = 0;
	while (i < 5)
	{
		if (NOISY_SAMPLES[i] < ulNumNoisySampleThd)
			i = i + 1;
		else
			break;
	}

	if (5 == i)
	{
		/* when there is no bin with samples that exceed the NUM_NOISY_SAMPLE_THD, the noise level
		 * of the picture is considered out of range so try with lower sample thresholds */

		uint32_t ulSampleThdScl = 5;

		i = 0;
		while (i < 5)
		{
			if (NOISY_SAMPLES[i] * ulSampleThdScl < ulNumNoisySampleThd)
				i = i + 1;
			else
				break;
		}

		if (5 == i)
		{
			/* when there is no bin with samples that exceed the NUM_NOISY_SAMPLE_THD, the noise level
			 * of the picture is still considered out of range so use previous noise level */
			 if ( hAnr->ulIndex == 0)
				NOISE_LEVEL_HISTORY = hAnr->aulNoiseLevelHistory[BVDC_P_NOISE_LVL_AVG_SIZE-1];
			 else
				NOISE_LEVEL_HISTORY = hAnr->aulNoiseLevelHistory[hAnr->ulIndex-1]; /* JJW use prev noise level */
		}
		else
		{
			k = i;
			m = 0;
			NOISE_LEVEL_HISTORY = 0;

			while ((k < 5) && (m < 3))
			{
				if( NOISY_SAMPLES[k] * ulSampleThdScl >= ulNumNoisySampleThd )
				{
					NOISE_LEVEL_HISTORY = NOISE_LEVEL_HISTORY + NOISE_LEVEL[k];
					k = k + 1;
					m = m + 1;
				}
				else
					break;
			}

			if ((m == 3) && (k < 5) && (i < 5))
			{
				while ((k < 5) && (i < 5) &&
					   ((NOISY_SAMPLES[i] * ulSampleThdScl - ulNumNoisySampleThd) < ulNumDiffThd) &&
					   (NOISY_SAMPLES[k] * ulSampleThdScl > ulNumNoisySampleThdBig))
				{
					NOISE_LEVEL_HISTORY = NOISE_LEVEL_HISTORY - NOISE_LEVEL[i];
					i = i + 1;
					NOISE_LEVEL_HISTORY = NOISE_LEVEL_HISTORY + NOISE_LEVEL[k];
					k = k + 1;
					if ((k >= 5) || (i >= 5))
						break;
				}
			}

			NOISE_LEVEL_HISTORY = NOISE_LEVEL_HISTORY / m;
		}
	}
	else
	{
		k = i;
		m = 0;
		NOISE_LEVEL_HISTORY = 0;

		while ((k < 5) && (m < 3))
		{
			if( NOISY_SAMPLES[k] >= ulNumNoisySampleThd )
			{
				NOISE_LEVEL_HISTORY = NOISE_LEVEL_HISTORY + NOISE_LEVEL[k];
				k = k + 1;
				m = m + 1;
			}
			else
				break;
		}
		/*BDBG_MSG(("BINS, %d, %d, %d, %d, %d, %d, %d, %d ",
			NOISY_SAMPLES[0], NOISY_SAMPLES[1], NOISY_SAMPLES[2],
			NOISY_SAMPLES[3], NOISY_SAMPLES[4],m,i,k ));*/

		if ((m == 3) && (k < 5) && (i < 5))
		{
			while ((k < 5) && (i < 5) &&
				   ((NOISY_SAMPLES[i]-ulNumNoisySampleThd) < ulNumDiffThd) &&
				   (NOISY_SAMPLES[k] > ulNumNoisySampleThdBig))
			{
				NOISE_LEVEL_HISTORY = NOISE_LEVEL_HISTORY - NOISE_LEVEL[i];
				i = i + 1;
				NOISE_LEVEL_HISTORY = NOISE_LEVEL_HISTORY + NOISE_LEVEL[k];
				k = k + 1;
				if ((k >= 5) || (i >= 5))
					break;
			}
		}

		NOISE_LEVEL_HISTORY = NOISE_LEVEL_HISTORY / m;
	}

	/* ------------------------------------------------------------
	 * (2).  setup to turn off the effect of MCTF filtering for a few vsync
	 * when SNR changes dramatically, or when ANR is under hard_start
	 */

	/* get the non-averaged SNR value for drastic SNR change detect */
	lSnDb = BVDC_P_Anr_BinarySearch_isr(NOISE_LEVEL_HISTORY);

	/* inform to turn off the effect of MCTF filtering for a few vsync,
	 * when SNR changes dramatically, or when ANR is under hard_start */
	if (((lSnDb - hAnr->lSnDb) > 10) || ((hAnr->lSnDb - lSnDb) > 10) ||
		(BVDC_P_ANR_NEED_HARD_START(hAnr)))
	{
		int  ii;

		/* snr change > 10 dB, likely scene changes, re-init for immediate anr effect */
		hAnr->ulCleanCntr = BVDC_P_ANR_CLEAN_VSYNCS;
		for (ii = 0; ii < BVDC_P_NOISE_LVL_AVG_SIZE; ii++)
			hAnr->aulNoiseLevelHistory[ii] = NOISE_LEVEL_HISTORY;
		hAnr->ulIndex = 0; /* current index to the above noise level history array */
		hAnr->ulNoiseLevelHistorySum = NOISE_LEVEL_HISTORY * BVDC_P_NOISE_LVL_AVG_SIZE;
	}

	/* store for next statisRead */
	hAnr->lSnDb = lSnDb;

	/* ------------------------------------------------------------
	 * (3).  compute SNR from the averaged NOISE_LEVEL_HISTORY
	 */

	/* average noise level starts */
	hAnr->ulNoiseLevelHistorySum -= hAnr->aulNoiseLevelHistory[hAnr->ulIndex];
	hAnr->ulNoiseLevelHistorySum += NOISE_LEVEL_HISTORY;
	hAnr->aulNoiseLevelHistory[hAnr->ulIndex] = NOISE_LEVEL_HISTORY;

	NOISE_LEVEL_HISTORY_AVG =
		hAnr->ulNoiseLevelHistorySum / BVDC_P_NOISE_LVL_AVG_SIZE;
	lSnDb = BVDC_P_Anr_BinarySearch_isr(NOISE_LEVEL_HISTORY_AVG);
	BDBG_MSG(("NoiseLevel: Avg %d / NonAvg %d, SNR: Avg %ddB / NonAvg %ddB",
		NOISE_LEVEL_HISTORY_AVG, NOISE_LEVEL_HISTORY, lSnDb, hAnr->lSnDb));

	/* use BVDC_P_NOISE_LVL_AVG_SIZE instead of the hard-coded number to control the index */
	if (hAnr->ulIndex < BVDC_P_NOISE_LVL_AVG_SIZE - 1)
		hAnr->ulIndex++;
	else
		hAnr->ulIndex = 0;

	/* ------------------------------------------------------------
	 * (4).  set alpha low threshold values and K values to HW
	 */

	/*get the K values before user adjustment*/
	pK1 = BVDC_P_Anr_GetKValue_isr((uint32_t)lSnDb, pCurSrcInfo->eCtInputType,
		pCurSrcInfo->pFmtInfo, hAnr->pAnrSetting->pvUserInfo);

	/* apply user adjust */
	lSnDb -= hAnr->pAnrSetting->iSnDbAdjust;
	lSnDb = BVDC_P_MAX(0, lSnDb);
	/* BDBG_MSG(("After adjust, s/n is around %ddB", lSnDb)); */

	/* TODO: 1). get rid of 3563 code;
	 *       2). use hAnr->bBypassFilter to inform SetEnable to uniformly bypass
	 *       3). update K value only when it needs change, how about alpha_low_thresh?
	 *       4). add dirty bit for anr internal cap format changes, Call SrcInit for it.
	 *           maybe add a macro to set anr->PxlFmt, and it sets dirty bit inside.
	 */

	/* get ALPHA_LOW_THRESHOLD to registers */
	pK2 = BVDC_P_Anr_GetKValue_isr((uint32_t)lSnDb, pCurSrcInfo->eCtInputType,
		pCurSrcInfo->pFmtInfo, hAnr->pAnrSetting->pvUserInfo);

	hAnr->bBypassFilter = pK1->bBypassFilter;

#if (BVDC_P_SUPPORT_ANR_VER >= BVDC_P_ANR_VER_3)
	if (hAnr->ulCleanCntr > 0)
	{
		hAnr->ulCleanCntr --;
		/* turn off the effct of MCTF:  */
		BVDC_P_SUBRUL_ONE_REG(pList, BCHP_HD_ANR_MCTF_0_CONT_0_ALPHA_LOW_THRESHOLD, ulRegOffset,
			BCHP_FIELD_DATA(HD_ANR_MCTF_0_CONT_0_ALPHA_LOW_THRESHOLD, MC,    (255*75/100)) |
			BCHP_FIELD_DATA(HD_ANR_MCTF_0_CONT_0_ALPHA_LOW_THRESHOLD, VALUE, (255*75/100)));
	}
	else
	{
		BVDC_P_SUBRUL_ONE_REG(pList, BCHP_HD_ANR_MCTF_0_CONT_0_ALPHA_LOW_THRESHOLD, ulRegOffset,
			BCHP_FIELD_DATA(HD_ANR_MCTF_0_CONT_0_ALPHA_LOW_THRESHOLD, MC,    pK2->ulAlphLowThdMC) |
			BCHP_FIELD_DATA(HD_ANR_MCTF_0_CONT_0_ALPHA_LOW_THRESHOLD, VALUE, pK2->ulAlphLowThdNMC));
	}
#else
	if (hAnr->ulCleanCntr > 0)
	{
		hAnr->ulCleanCntr --;
		/* turn off the effct of MCTF */
		BVDC_P_SUBRUL_ONE_REG(pList, BCHP_HD_ANR_MCTF_0_CONT_0_ALPHA_LOW_THRESHOLD, ulRegOffset,
			BCHP_FIELD_DATA(HD_ANR_MCTF_0_CONT_0_ALPHA_LOW_THRESHOLD, VALUE, (255*75/100)));
	}
	else
	{
		BVDC_P_SUBRUL_ONE_REG(pList, BCHP_HD_ANR_MCTF_0_CONT_0_ALPHA_LOW_THRESHOLD, ulRegOffset,
			BCHP_FIELD_DATA(HD_ANR_MCTF_0_CONT_0_ALPHA_LOW_THRESHOLD, VALUE, pK2->ulAlphLowThdNMC));
	}
#endif

	BVDC_P_SUBRUL_ONE_REG(pList, BCHP_HD_ANR_MCTF_0_CONT_0_MC_ALPHA_CALC_PARAME, ulRegOffset,
		BCHP_FIELD_DATA(HD_ANR_MCTF_0_CONT_0_MC_ALPHA_CALC_PARAME,     K0, pK2->ulMcK0) |
		BCHP_FIELD_DATA(HD_ANR_MCTF_0_CONT_0_MC_ALPHA_CALC_PARAME,     K1, pK2->ulMcK1));
	BVDC_P_SUBRUL_ONE_REG(pList, BCHP_HD_ANR_MCTF_0_CONT_0_NMC_ALPHA_CALC_PARAME, ulRegOffset,
		BCHP_FIELD_DATA(HD_ANR_MCTF_0_CONT_0_NMC_ALPHA_CALC_PARAME,    K0, pK2->ulNonMcK0) |
		BCHP_FIELD_DATA(HD_ANR_MCTF_0_CONT_0_NMC_ALPHA_CALC_PARAME,    K1, pK2->ulNonMcK1));
	BVDC_P_SUBRUL_ONE_REG(pList, BCHP_HD_ANR_MCTF_0_CONT_0_MC_NMC_ALPHA_CALC_PARAME, ulRegOffset,
		BCHP_FIELD_DATA(HD_ANR_MCTF_0_CONT_0_MC_NMC_ALPHA_CALC_PARAME, K0, pK2->ulFinalK0) |
		BCHP_FIELD_DATA(HD_ANR_MCTF_0_CONT_0_MC_NMC_ALPHA_CALC_PARAME, K1, pK2->ulFinalK1));

	return;
}

#else /* BVDC_P_SUPPORT_MANR */

static int32_t BVDC_P_Anr_BinarySearch_isr
	( BVDC_P_Anr_Handle              hAnr,
	  uint32_t                       ulNoiseLvlHistory )
{
	uint32_t  ulMin, ulMid, ulMax;
	uint16_t *pauNoiseLvlThresh;

	if (hAnr->ulAndThdScl >= (2 << BVDC_P_AndThdScl_FRAC))
	{
		pauNoiseLvlThresh = s_usNoiseLvlThreshHD;
	}
	else
	{
		pauNoiseLvlThresh = s_usNoiseLvlThreshSD;
	}

	ulMin = BVDC_P_SNR_MIN;
	ulMax = BVDC_P_SNR_MAX;
	while (ulMax > ulMin)
	{
		ulMid = (ulMax + ulMin) / 2;
		if (pauNoiseLvlThresh[ulMid - BVDC_P_SNR_MIN] > ulNoiseLvlHistory)
			ulMin = ulMid + 1;
		else
			ulMax = ulMid;
	}

	return ulMax;
}

/***************************************************************************
 * {private}
 *
 * BVDC_P_Anr_BuildRul_StatisRead_isr
 *
 * It is called by BVDC_P_Anr_BuildRul_isr to read statistics HW registers
 * and then to dynamically adjust the filter strength.
 *
 * This happens at every multiple of BVDC_P_ANR_NUM_ACCUM fields or frames.
 *
 *'Noise Lower/Upper Thresholds are set in ANR_AND_VPP.bss
 * user_adj = InputBox("Filter Strength Adjustment:" & vbLF & "0: Normal" &
 * vbLF & "+1: Stronger Filter" & vbLF & "-1: Weaker Filter", "ANR Detection &
 * Filtering", 0)
 *
 */
void BVDC_P_Anr_BuildRul_StatisRead_isr
	( BVDC_P_Anr_Handle              hAnr,
	  BVDC_P_ListInfo               *pList,
	  bool                           bInit)
{
	BREG_Handle  hRegister;
	BVDC_P_Source_Info  *pCurSrcInfo;
	uint32_t  ulRegOffset;
	uint32_t  NOISY_SAMPLES[5];             /*number of noisy samples */
	uint32_t  NOISE_LEVEL_MSB[5];           /* noise level msb of noise ranges 0 to 4 */
	uint32_t  NOISE_LEVEL[5];               /* noise level lsb of noise ranges 0 to 4 */
	uint32_t  NOISE_LEVEL_HISTORY, NOISE_LEVEL_HISTORY_AVG;
	uint32_t  ulNoiseLvlReadHigh, ulNoiseLvlHigh, ulNoiseLvlReadResidue;
	uint32_t  ulNoiseLvlReadLow, ulNoiseLvlLow;
	int i, k, m;
	int32_t  lSnDb;
	uint32_t  ulNumNoisySampleThdBig, ulNumDiffThd, ulNumNoisySampleThd, ulAndThdScl;
	const BVDC_P_AnrKValue  *pK1, *pK2;
	int ratio = 0;
	int avg_ratio = 0;
	bool bLowNoise = false;
	bool bNoiseDrop;

	BDBG_OBJECT_ASSERT(hAnr, BVDC_ANR);
	BSTD_UNUSED(bInit);
	pCurSrcInfo = hAnr->pCurSrcInfo;

	/* ulAndThdScl is less than ((1920 * 1088) << 11) / (720 * 240) = 0x60B6 */
	ulAndThdScl = hAnr->ulAndThdScl;
	ulNumNoisySampleThd = (NUM_NOISY_SAMPLE_THD * ulAndThdScl) >> BVDC_P_AndThdScl_FRAC;
	ulNumNoisySampleThdBig = (NUM_NOISY_SAMPLE_THD_BIG * ulAndThdScl) >> BVDC_P_AndThdScl_FRAC;
	ulNumDiffThd = (NUMDIFF_THD * ulAndThdScl) >> BVDC_P_AndThdScl_FRAC;
	BDBG_P_ANR_MSG(("ulAndThdScl 0x%x.%03lx", ulAndThdScl >> BVDC_P_AndThdScl_FRAC,
					ulAndThdScl & ((1 << BVDC_P_AndThdScl_FRAC) - 1)));

	hRegister = hAnr->hRegister;
	ulRegOffset = hAnr->ulRegOffset;

	/* Read and keep the BVB status to be extracted and used by other modules */
	hAnr->ulBvbStatus = BREG_Read32( hRegister, BCHP_HD_ANR_MCTF_0_BVB_IN_STATUS + ulRegOffset);

	/* 'collect values of noisy samples and noise levels so that they dont change during script */
	NOISY_SAMPLES[0] = BREG_Read32( hRegister, BCHP_HD_ANR_AND_0_NOISY_SAMPLE_NUM_BIN_0 + ulRegOffset );
	NOISY_SAMPLES[1] = BREG_Read32( hRegister, BCHP_HD_ANR_AND_0_NOISY_SAMPLE_NUM_BIN_1 + ulRegOffset );
	NOISY_SAMPLES[2] = BREG_Read32( hRegister, BCHP_HD_ANR_AND_0_NOISY_SAMPLE_NUM_BIN_2 + ulRegOffset );
	NOISY_SAMPLES[3] = BREG_Read32( hRegister, BCHP_HD_ANR_AND_0_NOISY_SAMPLE_NUM_BIN_3 + ulRegOffset );
	NOISY_SAMPLES[4] = BREG_Read32( hRegister, BCHP_HD_ANR_AND_0_NOISY_SAMPLE_NUM_BIN_4 + ulRegOffset );
	NOISE_LEVEL_MSB[0] = BREG_Read32( hRegister, BCHP_HD_ANR_AND_0_NOISE_LEVEL_0_MSB + ulRegOffset );
	NOISE_LEVEL_MSB[1] = BREG_Read32( hRegister, BCHP_HD_ANR_AND_0_NOISE_LEVEL_1_MSB + ulRegOffset );
	NOISE_LEVEL_MSB[2] = BREG_Read32( hRegister, BCHP_HD_ANR_AND_0_NOISE_LEVEL_2_MSB + ulRegOffset );
	NOISE_LEVEL_MSB[3] = BREG_Read32( hRegister, BCHP_HD_ANR_AND_0_NOISE_LEVEL_3_MSB + ulRegOffset );
	NOISE_LEVEL_MSB[4] = BREG_Read32( hRegister, BCHP_HD_ANR_AND_0_NOISE_LEVEL_4_MSB + ulRegOffset );
	NOISE_LEVEL[0] = BREG_Read32( hRegister, BCHP_HD_ANR_AND_0_NOISE_LEVEL_0_LSB + ulRegOffset );
	NOISE_LEVEL[1] = BREG_Read32( hRegister, BCHP_HD_ANR_AND_0_NOISE_LEVEL_1_LSB + ulRegOffset );
	NOISE_LEVEL[2] = BREG_Read32( hRegister, BCHP_HD_ANR_AND_0_NOISE_LEVEL_2_LSB + ulRegOffset );
	NOISE_LEVEL[3] = BREG_Read32( hRegister, BCHP_HD_ANR_AND_0_NOISE_LEVEL_3_LSB + ulRegOffset );
	NOISE_LEVEL[4] = BREG_Read32( hRegister, BCHP_HD_ANR_AND_0_NOISE_LEVEL_4_LSB + ulRegOffset );

	/* ------------------------------------------------------------
	 * (1).  compute NOISE_LEVEL_HISTORY
	 */
	for (k = 0; k < 5; k++) /* averaging function */
	{
		if (NOISY_SAMPLES[k] > 0)
		{
			/* multiple 8 for 3 bits fraction */
			BDBG_P_ANR_MSG(("NL Reg %d: MSB %d, LSB %d, NumSam %d -> %d",
					  k, NOISE_LEVEL_MSB[k], NOISE_LEVEL[k], NOISY_SAMPLES[k],
					  (((NOISE_LEVEL_MSB[k] << (32 - 5)) | (NOISE_LEVEL[k] >> 5)) / NOISY_SAMPLES[k]) << 8));
			/* NOISE_LEVEL_MSB[k] has max 5 bits, NOISY_SAMPLES[k] has max 1920*1080, 21 bits
			 * ulNoiseLvlReadHigh < 3+5+22 = 30 bits, ulNoiseLvlReadLow < 21+10 = 31 bits */
			ulNoiseLvlReadHigh = 8 * ((NOISE_LEVEL_MSB[k] << (32 - 10)) | (NOISE_LEVEL[k] >> 10));
			ulNoiseLvlHigh = ulNoiseLvlReadHigh / NOISY_SAMPLES[k];
			ulNoiseLvlReadResidue = ulNoiseLvlReadHigh - ulNoiseLvlHigh * NOISY_SAMPLES[k];
			ulNoiseLvlReadLow = (ulNoiseLvlReadResidue << 10) + 8 * (NOISE_LEVEL[k] & ((1 << 10) - 1));
			ulNoiseLvlLow = ulNoiseLvlReadLow / NOISY_SAMPLES[k];
			NOISE_LEVEL[k] = (ulNoiseLvlHigh << 10) + ulNoiseLvlLow;
			BDBG_P_ANR_MSG(("Avrg NoiseLvlRegSet %d: %d", k, NOISE_LEVEL[k]));
		}
	}

	i = 0;
	while (i < 5)
	{
		if (NOISY_SAMPLES[i] < ulNumNoisySampleThd)
			i = i + 1;
		else
			break;
	}

	if (5 == i)
	{
#if (JJW == 0)
		/* when there is no bin with samples that exceed the NUM_NOISY_SAMPLE_THD, the noise level
		 * of the picture is considered out of range and is indicated as very noisy */
		NOISE_LEVEL_HISTORY = VERY_HIGH_NOISE_LEVEL;
#else /*JJW*/
		/* when there is no bin with samples that exceed the NUM_NOISY_SAMPLE_THD, the noise level
		 * of the picture is considered out of range so try with lower sample thresholds */

		uint32_t ulSampleThdScl = 5;

		i = 0;
		while (i < 5)
		{
			if (NOISY_SAMPLES[i] * ulSampleThdScl < ulNumNoisySampleThd)
				i = i + 1;
			else
				break;
		}

		if (5 == i)
		{
			/* when there is no bin with samples that exceed the NUM_NOISY_SAMPLE_THD, the noise level
			 * of the picture is still considered out of range so use previous noise level */
			 if ( hAnr->ulIndex == 0)
				NOISE_LEVEL_HISTORY = hAnr->aulNoiseLevelHistory[BVDC_P_NOISE_LVL_AVG_SIZE-1];
			 else
				NOISE_LEVEL_HISTORY = hAnr->aulNoiseLevelHistory[hAnr->ulIndex-1]; /* JJW use prev noise level */
		}
		else
		{
			k = i;
			m = 0;
			NOISE_LEVEL_HISTORY = 0;

			while ((k < 5) && (m < 3))
			{
				if( NOISY_SAMPLES[k] * ulSampleThdScl >= ulNumNoisySampleThd )
				{
					NOISE_LEVEL_HISTORY = NOISE_LEVEL_HISTORY + NOISE_LEVEL[k];
					k = k + 1;
					m = m + 1;
				}
				else
					break;
			}

			if ((m == 3) && (k < 5) && (i < 5))
			{
				while ((k < 5) && (i < 5) &&
					   ((NOISY_SAMPLES[i] * ulSampleThdScl - ulNumNoisySampleThd) < ulNumDiffThd) &&
					   (NOISY_SAMPLES[k] * ulSampleThdScl > ulNumNoisySampleThdBig))
				{
					NOISE_LEVEL_HISTORY = NOISE_LEVEL_HISTORY - NOISE_LEVEL[i];
					i = i + 1;
					NOISE_LEVEL_HISTORY = NOISE_LEVEL_HISTORY + NOISE_LEVEL[k];
					k = k + 1;
					if ((k >= 5) || (i >= 5))
						break;
				}
			}

			NOISE_LEVEL_HISTORY = NOISE_LEVEL_HISTORY / m;
		}
#endif /*JJW*/
	}
	else
	{
		int weight = 0;

		for (k = 0; k < 5; k++)
		{
			if( NOISY_SAMPLES[k] >= ulNumNoisySampleThd )
			{
				if( NOISY_SAMPLES[k] >= NOISY_SAMPLES[weight] )
				{
					weight = k;
				}
			}
		}

		k = i;
		m = 0;
		NOISE_LEVEL_HISTORY = 0;

		while ((k < 5) && (m < 5))
		{
			if( NOISY_SAMPLES[k] >= ulNumNoisySampleThd )
			{
				if( k == weight )
				{
					NOISE_LEVEL_HISTORY = NOISE_LEVEL_HISTORY + (NOISE_LEVEL[k]*2);
					m = m + 1;
				}
				else
				{
					NOISE_LEVEL_HISTORY = NOISE_LEVEL_HISTORY + NOISE_LEVEL[k];
				}
				k = k + 1;
				m = m + 1;
			}
			else
				break;
		}
		/*BDBG_MSG(("BINS, %d, %d, %d, %d, %d, %d, %d, %d ",
			NOISY_SAMPLES[0], NOISY_SAMPLES[1], NOISY_SAMPLES[2],
			NOISY_SAMPLES[3], NOISY_SAMPLES[4],m,i,k ));*/

		if ((m == 3) && (k < 5) && (i < 5))
		{
			while ((k < 5) && (i < 5) &&
				   ((NOISY_SAMPLES[i]-ulNumNoisySampleThd) < ulNumDiffThd) &&
				   (NOISY_SAMPLES[k] > ulNumNoisySampleThdBig))
			{
				NOISE_LEVEL_HISTORY = NOISE_LEVEL_HISTORY - NOISE_LEVEL[i];
				i = i + 1;
				NOISE_LEVEL_HISTORY = NOISE_LEVEL_HISTORY + NOISE_LEVEL[k];
				k = k + 1;
				if ((k >= 5) || (i >= 5))
					break;
			}
		}

		NOISE_LEVEL_HISTORY = NOISE_LEVEL_HISTORY / m;
	}

	/* ------------------------------------------------------------
	 * (2).  setup to turn off the effect of MCTF filtering for a few vsync
	 * when SNR changes dramatically, or when ANR is under hard_start
	 */

	/* get the non-averaged SNR value for drastic SNR change detect */
	lSnDb = BVDC_P_Anr_BinarySearch_isr(hAnr, NOISE_LEVEL_HISTORY);

#if 0   /* scene change detection logic should be removed */
	/* inform to turn off the effect of MCTF filtering for a few vsync,
	 * when SNR changes dramatically, or when ANR is under hard_start */
	if (((lSnDb - hAnr->lSnDb) > 10) || ((hAnr->lSnDb - lSnDb) > 10) ||
		(BVDC_P_ANR_NEED_HARD_START(hAnr)))
	{
		int  ii;

		/* snr change > 10 dB, likely scene changes, re-init for immediate anr effect */
		hAnr->ulCleanCntr = BVDC_P_ANR_CLEAN_VSYNCS;
		for (ii = 0; ii < BVDC_P_NOISE_LVL_AVG_SIZE; ii++)
			hAnr->aulNoiseLevelHistory[ii] = NOISE_LEVEL_HISTORY;
		hAnr->ulIndex = 0; /* current index to the above noise level history array */
		hAnr->ulNoiseLevelHistorySum = NOISE_LEVEL_HISTORY * BVDC_P_NOISE_LVL_AVG_SIZE;
	}
#endif

	/* store for next statisRead */
	hAnr->lSnDb = lSnDb;

	/* Additional agorithm */
	if((NOISY_SAMPLES[0]+NOISY_SAMPLES[1]+NOISY_SAMPLES[2]+NOISY_SAMPLES[3]+NOISY_SAMPLES[4]) == 0)
		ratio = 1024;
	else
		ratio = (1024*NOISY_SAMPLES[0])/(NOISY_SAMPLES[0]+NOISY_SAMPLES[1]+NOISY_SAMPLES[2]+NOISY_SAMPLES[3]+NOISY_SAMPLES[4]);
	hAnr->ulNoiseNumHistorySum -= hAnr->aulNoiseNumHistory[hAnr->ulIndex2];
	hAnr->ulNoiseNumHistorySum += ratio;
	hAnr->aulNoiseNumHistory[hAnr->ulIndex2] = ratio;
	avg_ratio = hAnr->ulNoiseNumHistorySum / BVDC_P_NOISE_NUM_AVG_SIZE;
	if(hAnr->ulIndex2 < BVDC_P_NOISE_NUM_AVG_SIZE - 1)
		hAnr->ulIndex2++;
	else
		hAnr->ulIndex2 = 0;

	bLowNoise = (ratio > 1024);  /* (16 * 1024) >> 4 */
	if (bLowNoise)
		hAnr->uPreviousLowNoise = hAnr->uPreviousLowNoise + bLowNoise;
	else
		hAnr->uPreviousLowNoise = 0;
	bNoiseDrop = bLowNoise && (hAnr->uPreviousLowNoise <= 2);

	/* ------------------------------------------------------------
	 * (3).  compute SNR from the averaged NOISE_LEVEL_HISTORY
	 */

	/*if(ratio <= avg_ratio)*/
	if(!bNoiseDrop)
	{
		/* average noise level starts */
		hAnr->ulNoiseLevelHistorySum -= hAnr->aulNoiseLevelHistory[hAnr->ulIndex];
		hAnr->ulNoiseLevelHistorySum += NOISE_LEVEL_HISTORY;
		hAnr->aulNoiseLevelHistory[hAnr->ulIndex] = NOISE_LEVEL_HISTORY;

		/* use BVDC_P_NOISE_LVL_AVG_SIZE instead of the hard-coded number to control the index */
		if (hAnr->ulIndex < BVDC_P_NOISE_LVL_AVG_SIZE - 1)
			hAnr->ulIndex++;
		else
			hAnr->ulIndex = 0;
	}

	NOISE_LEVEL_HISTORY_AVG =
		hAnr->ulNoiseLevelHistorySum / BVDC_P_NOISE_LVL_AVG_SIZE;
	lSnDb = BVDC_P_Anr_BinarySearch_isr(hAnr, NOISE_LEVEL_HISTORY_AVG);
	BDBG_MSG(("NoiseLevel: Avg %d / NonAvg %d, SNR: Avg %ddB / NonAvg %ddB",
		NOISE_LEVEL_HISTORY_AVG, NOISE_LEVEL_HISTORY, lSnDb, hAnr->lSnDb));

	/* ------------------------------------------------------------
	 * (4).  set alpha low threshold values and K values to HW
	 */

	/*get the K values before user adjustment*/
	pK1 = BVDC_P_Anr_GetKValue_isr((uint32_t)lSnDb, pCurSrcInfo->eCtInputType,
		pCurSrcInfo->pFmtInfo, hAnr->pAnrSetting->pvUserInfo);

	/* apply user adjust */
	lSnDb -= hAnr->pAnrSetting->iSnDbAdjust;
	lSnDb = BVDC_P_MAX(0, lSnDb);
	/* BDBG_MSG(("After adjust, s/n is around %ddB", lSnDb)); */

	/* TODO: 1). get rid of 3563 code;
	 *       2). use hAnr->bBypassFilter to inform SetEnable to uniformly bypass
	 *       3). update K value only when it needs change, how about alpha_low_thresh?
	 *       4). add dirty bit for anr internal cap format changes, Call SrcInit for it.
	 *           maybe add a macro to set anr->PxlFmt, and it sets dirty bit inside.
	 */

	/* get ALPHA_LOW_THRESHOLD to registers */
	pK2 = BVDC_P_Anr_GetKValue_isr((uint32_t)lSnDb, pCurSrcInfo->eCtInputType,
		pCurSrcInfo->pFmtInfo, hAnr->pAnrSetting->pvUserInfo);

	hAnr->bBypassFilter = pK1->bBypassFilter;

	if (hAnr->ulCleanCntr > 0)
	{
		hAnr->ulCleanCntr --;
		/* turn off the effct of MCTF:  */
		BVDC_P_SUBRUL_ONE_REG(pList, BCHP_HD_ANR_MCTF_0_CONT_0_ALPHA_LOW_THRESHOLD, ulRegOffset,
			BCHP_FIELD_DATA(HD_ANR_MCTF_0_CONT_0_ALPHA_LOW_THRESHOLD, MC,    (255*75/100)) |
			BCHP_FIELD_DATA(HD_ANR_MCTF_0_CONT_0_ALPHA_LOW_THRESHOLD, VALUE, (255*75/100)));
	}
	else
	{
		BVDC_P_SUBRUL_ONE_REG(pList, BCHP_HD_ANR_MCTF_0_CONT_0_ALPHA_LOW_THRESHOLD, ulRegOffset,
			BCHP_FIELD_DATA(HD_ANR_MCTF_0_CONT_0_ALPHA_LOW_THRESHOLD, MC,    pK2->ulAlphLowThdMC) |
			BCHP_FIELD_DATA(HD_ANR_MCTF_0_CONT_0_ALPHA_LOW_THRESHOLD, VALUE, pK2->ulAlphLowThdNMC));
	}

	BVDC_P_SUBRUL_ONE_REG(pList, BCHP_HD_ANR_MCTF_0_CONT_0_MC_ALPHA_CALC_PARAME, ulRegOffset,
		BCHP_FIELD_DATA(HD_ANR_MCTF_0_CONT_0_MC_ALPHA_CALC_PARAME,     K0, pK2->ulMcK0) |
		BCHP_FIELD_DATA(HD_ANR_MCTF_0_CONT_0_MC_ALPHA_CALC_PARAME,     K1, pK2->ulMcK1));
	BVDC_P_SUBRUL_ONE_REG(pList, BCHP_HD_ANR_MCTF_0_CONT_0_NMC_ALPHA_CALC_PARAME, ulRegOffset,
		BCHP_FIELD_DATA(HD_ANR_MCTF_0_CONT_0_NMC_ALPHA_CALC_PARAME,    K0, pK2->ulNonMcK0) |
		BCHP_FIELD_DATA(HD_ANR_MCTF_0_CONT_0_NMC_ALPHA_CALC_PARAME,    K1, pK2->ulNonMcK1));
#if (BVDC_P_SUPPORT_MANR_VER >= BVDC_P_MANR_VER_2)
	BVDC_P_SUBRUL_ONE_REG(pList, BCHP_HD_ANR_MCTF_0_CONT_0_MC_CH_ALPHA_CALC_PARAME, ulRegOffset,
		BCHP_FIELD_DATA(HD_ANR_MCTF_0_CONT_0_MC_CH_ALPHA_CALC_PARAME,     K0, pK2->ulMcK0_CH) |
		BCHP_FIELD_DATA(HD_ANR_MCTF_0_CONT_0_MC_CH_ALPHA_CALC_PARAME,     K1, pK2->ulMcK1_CH));
	BVDC_P_SUBRUL_ONE_REG(pList, BCHP_HD_ANR_MCTF_0_CONT_0_NMC_CH_ALPHA_CALC_PARAME, ulRegOffset,
		BCHP_FIELD_DATA(HD_ANR_MCTF_0_CONT_0_NMC_CH_ALPHA_CALC_PARAME,    K0, pK2->ulNonMcK0_CH) |
		BCHP_FIELD_DATA(HD_ANR_MCTF_0_CONT_0_NMC_CH_ALPHA_CALC_PARAME,    K1, pK2->ulNonMcK1_CH));
#endif
	BVDC_P_SUBRUL_ONE_REG(pList, BCHP_HD_ANR_MCTF_0_CONT_0_MC_NMC_ALPHA_CALC_PARAME, ulRegOffset,
		BCHP_FIELD_DATA(HD_ANR_MCTF_0_CONT_0_MC_NMC_ALPHA_CALC_PARAME, K0, pK2->ulFinalK0) |
		BCHP_FIELD_DATA(HD_ANR_MCTF_0_CONT_0_MC_NMC_ALPHA_CALC_PARAME, K1, pK2->ulFinalK1));

	if (hAnr->ulAndThdScl >= (2 << BVDC_P_AndThdScl_FRAC))
	{
		/* HD */
		BVDC_P_SUBRUL_ONE_REG(pList, BCHP_HD_ANR_MCTF_0_CONT_0_WIN_COST_ADJUST, ulRegOffset,
			BCHP_FIELD_DATA(HD_ANR_MCTF_0_CONT_0_WIN_COST_ADJUST, MC,  0) |
			BCHP_FIELD_DATA(HD_ANR_MCTF_0_CONT_0_WIN_COST_ADJUST, NMC, 1));
	}
	else
	{
		/* SD */
		BVDC_P_SUBRUL_ONE_REG(pList, BCHP_HD_ANR_MCTF_0_CONT_0_WIN_COST_ADJUST, ulRegOffset,
			BCHP_FIELD_DATA(HD_ANR_MCTF_0_CONT_0_WIN_COST_ADJUST, MC,  0) |
			BCHP_FIELD_DATA(HD_ANR_MCTF_0_CONT_0_WIN_COST_ADJUST, NMC, 0));
	}

	return;
}

#endif

/***************************************************************************/
/* No support for any anr */
#else

#include "bvdc_errors.h"

BDBG_MODULE(BVDC_ANR);
BDBG_OBJECT_ID(BVDC_ANR);

BERR_Code BVDC_P_Anr_Create
	( BVDC_P_Anr_Handle *          phAnr,
	  BVDC_P_AnrId                 eAnrId,
	  BREG_Handle                  hRegister,
	  BVDC_P_Resource_Handle       hResource )
{
	BDBG_ASSERT(phAnr);
	*phAnr = NULL;
	BSTD_UNUSED(eAnrId);
	BSTD_UNUSED(hRegister);
	BSTD_UNUSED(hResource);
	return BERR_TRACE(BVDC_ERR_ANR_HW_NOT_AVAILABLE);
}

BERR_Code BVDC_P_Anr_Destroy
	( BVDC_P_Anr_Handle            hAnr )
{
	BSTD_UNUSED(hAnr);
	return BERR_SUCCESS;
}

BERR_Code BVDC_P_Anr_SetDemoMode_isr
	( BVDC_P_Anr_Handle            hAnr,
	  BVDC_SplitScreenMode         eDemoMode )
{
	BSTD_UNUSED(hAnr);
	BSTD_UNUSED(eDemoMode);
	return BERR_SUCCESS;
}

void BVDC_P_Anr_BuildRul_SrcInit_isr
	( BVDC_P_AnrContext             *pAnr,
	  BVDC_P_ListInfo               *pList,
	  BVDC_P_PictureNode            *pPicture )
{
	BSTD_UNUSED(pAnr);
	BSTD_UNUSED(pList);
	BSTD_UNUSED(pPicture);
}

void BVDC_P_Anr_BuildRul_SetEnable_isr
	( BVDC_P_AnrContext             *pAnr,
	  BVDC_P_PictureNode            *pPicture,
	  BVDC_P_ListInfo               *pList,
	  bool                           bEnable,
	  BAVC_Polarity                  eSrcNxtFldId,
	  bool                           bRepeat,
	  bool                           bInit )
{
	BSTD_UNUSED(pAnr);
	BSTD_UNUSED(pPicture);
	BSTD_UNUSED(pList);
	BSTD_UNUSED(bEnable);
	BSTD_UNUSED(eSrcNxtFldId);
	BSTD_UNUSED(bRepeat);
	BSTD_UNUSED(bInit);
}

void BVDC_P_Anr_BuildRul_StatisRead_isr
	( BVDC_P_AnrContext             *pAnr,
	  BVDC_P_ListInfo               *pList,
	  bool                           bInit)
{
	BSTD_UNUSED(pAnr);
	BSTD_UNUSED(pList);
	BSTD_UNUSED(bInit);
}
#endif  /* #if (BVDC_P_SUPPORT_ANR) || (BVDC_P_SUPPORT_MANR) */

#if !(BVDC_P_SUPPORT_ANR)
BERR_Code BVDC_P_Anr_AcquireConnect
	( BVDC_P_Anr_Handle                 hAnr,
	  BVDC_Heap_Handle                  hHeap,
	  BVDC_P_WindowId                   eWinId,
	  const BVDC_Anr_Settings          *pAnrSetting,
	  BVDC_P_Source_InfoPtr             pCurSrcInfo)
{
	BSTD_UNUSED(hAnr);
	BSTD_UNUSED(hHeap);
	BSTD_UNUSED(eWinId);
	BSTD_UNUSED(pAnrSetting);
	BSTD_UNUSED(pCurSrcInfo);
	return BERR_SUCCESS;
}

BERR_Code BVDC_P_Anr_ReleaseConnect_isr
	( BVDC_P_Anr_Handle         *phAnr )
{
	BSTD_UNUSED(phAnr);
	return BERR_SUCCESS;
}

void BVDC_P_Anr_BuildRul_isr(
	BVDC_P_Anr_Handle              hAnr,
	BVDC_P_ListInfo               *pList,
	BVDC_P_State                   eVnetState,
	BVDC_P_PictureNode            *pPicture )
{
	BSTD_UNUSED(hAnr);
	BSTD_UNUSED(pList);
	BSTD_UNUSED(eVnetState);
	BSTD_UNUSED(pPicture);
}

void BVDC_P_Anr_SetVnetAllocBuf_isr
	( BVDC_P_Anr_Handle          hAnr,
	  uint32_t                   ulSrcMuxValue,
	  BVDC_P_VnetPatch           eVnetPatchMode,
	  BVDC_P_BufferHeapId        eBufHeapId,
	  uint32_t                   ulBufCnt,
	  bool                       bMemSaving )
{
	BSTD_UNUSED(hAnr);
	BSTD_UNUSED(ulSrcMuxValue);
	BSTD_UNUSED(eVnetPatchMode);
	BSTD_UNUSED(eBufHeapId);
	BSTD_UNUSED(ulBufCnt);
	BSTD_UNUSED(bMemSaving);
}

void BVDC_P_Anr_UnsetVnetFreeBuf_isr
	( BVDC_P_Anr_Handle          hAnr )
{
	BSTD_UNUSED(hAnr);
}

#endif  /*#if !(BVDC_P_SUPPORT_ANR)*/

/* End of file. */
