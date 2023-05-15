/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvdc_common_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/624 $
 * $brcm_Date: 10/12/12 3:29p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vdc/src/common/bvdc_common_priv.h $
 * 
 * Hydra_Software_Devel/PROD_HUMAX_URSR/1   5/29/15 5:47p scho
 * SW7425-4944: Video flicker while transition full scren to PiG window.
 * Complete transfer destination clipping to source from SW7346-1200
 * 
 * Hydra_Software_Devel/624   10/12/12 3:29p vanessah
 * SW7445-8: add Cap8 for 7445 and simplify source
 * 
 * Hydra_Software_Devel/623   10/4/12 1:34p vanessah
 * SW7360-37: update 7360 IT version
 * 
 * Hydra_Software_Devel/622   9/12/12 3:30p pntruong
 * SW7425-2526: Fixed 7408 build error.
 * 
 * Hydra_Software_Devel/621   9/7/12 4:34p yuxiaz
 * SW7552-195: Enable BVDC_P_SUPPORT_WIN_CONST_COLOR for 7552 and 7358.
 * 
 * Hydra_Software_Devel/620   8/23/12 1:20p vanessah
 * SW7429-243: add support for 7429B0/7435B0/7548A0
 * 
 * Hydra_Software_Devel/619   8/15/12 6:21p tdo
 * SW7445-8: Create Appframework emulation environment for 7445 A0
 *
 * Hydra_Software_Devel/618   8/14/12 9:37a pntruong
 * SW7344-345: Fix for HW7344-429 is in B1 instead of B0.
 *
 * Hydra_Software_Devel/617   8/13/12 4:01p tdo
 * SW7425-3734: Fix coverity issues
 *
 * Hydra_Software_Devel/616   8/13/12 3:07p yuxiaz
 * SW7425-3626, SW7425-3619: Replace uncache memory access with cache
 * memory access: use the new API names in RDC.
 *
 * Hydra_Software_Devel/615   8/7/12 4:31p vanessah
 * SW7425-3591:  set BVDC_P_MVFD_ALIGNMENT_WORKAROUND for MFD version 12
 * and 13 only
 *
 * Hydra_Software_Devel/614   8/7/12 10:35a vanessah
 * SW7563-9: update 7563/7584/7429b0 MFD version to 14
 *
 * Hydra_Software_Devel/613   8/3/12 12:52p vanessah
 * SW7425-3591: set BVDC_P_MVFD_ALIGNMENT_WORKAROUND for 7425
 *
 * Hydra_Software_Devel/612   7/26/12 5:20p yuxiaz
 * SW7552-312: Component output will be flickering and the color is gray
 * when change display format from PAL to 576i(SECAM).
 *
 * Hydra_Software_Devel/611   7/24/12 3:02p tdo
 * SW7360-29 : Temporarily turn off DAC auto-detection & calibration until
 * it's fully working for 7360 and 7231
 *
 * Hydra_Software_Devel/610   7/17/12 2:06p vanessah
 * SW7435-265: update the ANR version for 7435
 *
 * Hydra_Software_Devel/609   7/13/12 11:24a tdo
 * SW7563-9: Add support for VDC
 *
 * Hydra_Software_Devel/608   7/12/12 1:11p tdo
 * SW7584-21: Update VEC version for 7584
 *
 * Hydra_Software_Devel/607   6/15/12 2:50p vanessah
 * SW7584-21: update GFD version
 *
 * Hydra_Software_Devel/606   6/14/12 3:41p syang
 * SW7425-2172: VFD/GFX with only cmp0 need 6 scratch regs for gfx;
 * prepare to add SetSurface support to MFD
 *
 * Hydra_Software_Devel/605   6/13/12 1:55p darnstein
 * SW7425-3226: alignment for capture buffer should be 4, not 16. Affects
 * mosaic mode.
 *
 * Hydra_Software_Devel/604   6/12/12 5:08p darnstein
 * SW7425-3226: fix macro BVDC_P_IS_ALIGN().
 *
 * Hydra_Software_Devel/603   5/23/12 6:45p syang
 * SW7425-2093: refactor vfd gfx feeding support for deep cleanup and to
 * use gfxsurface sub-module
 *
 * Hydra_Software_Devel/602   5/9/12 10:52a syang
 * SW7425-2093: refactor gfxfeeder code for deep cleanup and potential
 * error fix with GetSurface; use shared gfxsurface to manage gfx surface
 *
 * Hydra_Software_Devel/601   5/2/12 10:56a yuxiaz
 * HW7552-56: Enabled vec stand-alone fixed for 7552B.
 *
 * Hydra_Software_Devel/600   4/27/12 4:02p tdo
 * SW7584-21: Add PI/vdc support for 7584
 *
 * Hydra_Software_Devel/599   4/26/12 11:34a tdo
 * SW7425-2571: Add VDC API to query BVN module error
 *
 * Hydra_Software_Devel/598   4/6/12 7:59p vanessah
 * SW7425-2831: change polarity due to ignore frame flag
 *
 * Hydra_Software_Devel/597   4/3/12 1:29p pntruong
 * HW7346-322, HW7425-470, HW7358-57: Enabled clock-gating.
 *
 * Hydra_Software_Devel/596   3/29/12 3:48p pntruong
 * SW7405-4296, SW7425-2526: Added some initial capabilities query at the
 * vdc top level.
 *
 * Hydra_Software_Devel/595   3/29/12 11:18a tdo
 * SW7435-85: MVP_0 enable error.  Add support for new capture enable
 * control bit.
 *
 * Hydra_Software_Devel/594   3/21/12 1:38p syang
 * SW7425-2699: put mpaa under the control of vdc resource manager; handle
 * correctly both order of enabling/disabling mpaa and turning on/off
 * hdmi/component output
 *
 * Hydra_Software_Devel/593   3/14/12 4:23p pntruong
 * HW7435-347: Compiled out work-around for SW7231-635.
 *
 * Hydra_Software_Devel/592   3/8/12 5:27p vanessah
 * SW7435-31: Sync MADR golden value and removal EoP trigger workaround
 *
 * Hydra_Software_Devel/591   3/5/12 11:17a pntruong
 * HW7344-301: Enabled vec stand-alone fixed for 7344b1.
 *
 * Hydra_Software_Devel/590   2/21/12 4:36p albertl
 * SW7420-339: Re-enabled constant color for supported chips.
 *
 * Hydra_Software_Devel/589   2/17/12 2:13p yuxiaz
 * SW7231-635: Use BVDC_P_SCB_BURST_SIZE.
 *
 * Hydra_Software_Devel/588   2/17/12 11:47a yuxiaz
 * SW7231-635: Update comment.
 *
 * Hydra_Software_Devel/587   2/16/12 5:48p yuxiaz
 * SW7231-635: Added sw workaround for VFD.
 *
 * Hydra_Software_Devel/586   2/15/12 3:16p vanessah
 * SW7360-3: change MADR version to 5, removal the HW workaround
 *
 * Hydra_Software_Devel/585   2/13/12 7:04p darnstein
 * SW7358-227: handle chips that do not have register
 * VF_n.NEG_SYNC_AMPLITUDE_EXTN.
 *
 * Hydra_Software_Devel/584   2/8/12 10:05a pntruong
 * SW7360-3: Initial support.
 *
 * Hydra_Software_Devel/583   1/30/12 1:55p vanessah
 * SW7425-2287: roll back 7425/7231/7346 Bx GFX version due to incomplete
 * HW fixes set
 *
 * Hydra_Software_Devel/582   1/27/12 11:20p vanessah
 * SW7425-2287: remove workaround for HW7425-385 for GFD_VER_5
 *
 * Hydra_Software_Devel/581   1/26/12 12:04p vanessah
 * SW7435-9: change STG version to 3 and gfd versio to 5
 *
 * Hydra_Software_Devel/580   1/13/12 2:51p tdo
 * SW7358-159 , SW7418-55 : Default DAC auto detection ON for
 * 7231/7344/7346Bx
 *
 * Hydra_Software_Devel/579   1/12/12 5:38p vanessah
 * SW7425-2162: change 7425 MADR QM buffer allocation
 *
 * Hydra_Software_Devel/578   1/11/12 3:06p darnstein
 * SW7125-1124: merge DCS Hybrid+ to main branch. DCS almost ready for
 * production.
 *
 * Hydra_Software_Devel/SW7125-1124/5   1/11/12 12:09p darnstein
 * SW7125-1124: merge from main branch
 *
 * Hydra_Software_Devel/SW7125-1124/4   1/2/12 12:23p darnstein
 * SW7125-1124: merge from main branch.
 *
 *
 * Hydra_Software_Devel/SW7125-1124/3   12/1/11 7:08p darnstein
 * SW7125-1124: merge from main branch.
 *
 * Hydra_Software_Devel/SW7125-1124/2   12/1/11 6:39p darnstein
 * SW7125-1124: change definition of IT_VER to identify early version of
 * IT hardware that has PCL_6, PCL_7, and PCL_8 registers.
 *
 * Hydra_Software_Devel/574   12/1/11 11:01a pntruong
 * SW7429-16: Enabled rfm.
 *
 * Hydra_Software_Devel/574   12/1/11 10:48a pntruong
 * SW7429-16: Enabled rfm.
 *
 * Hydra_Software_Devel/573   11/30/11 5:36p yuxiaz
 * SW7425-1182: Set default 8bit 422 format based on LE or BE.
 *
 * Hydra_Software_Devel/572   11/23/11 4:25p tdo
 * SW7435-9: Add support for CMP4-5, GFD4-5, MFD3, VFD5
 *
 * Hydra_Software_Devel/571   11/23/11 11:30a tdo
 * SW7435-9: add support for 7435A0 in VDC
 *
 * Hydra_Software_Devel/570   11/22/11 5:37p yuxiaz
 * SW7420-2135: Update BVDC_P_MULTIBUFFER_RW_TOLERANCE  so it works on
 * both 108MHz and 216MHz system.
 *
 * Hydra_Software_Devel/569   11/22/11 5:09p pntruong
 * SW7429-16: Initial bringup of 7429.
 *
 * Hydra_Software_Devel/569   11/22/11 5:08p pntruong
 * SW7429-16: Initial bringup of 7429.
 *
 * Hydra_Software_Devel/568   11/18/11 3:09p yuxiaz
 * SW7420-2135: Move MAX number of RULs defines to bvdc_common_priv.h
 *
 * Hydra_Software_Devel/567   11/18/11 2:34p yuxiaz
 * SW7420-2135: Updated BVDC_P_MULTIBUFFER_RW_TOLERANCE  base on
 * BVDC_P_MAX_ENTRY_PER_RUL.
 *
 * Hydra_Software_Devel/566   11/18/11 11:41a vanessah
 * SW7425-1743: apply Mosaic mode x_offset  even number restriction to all
 * platforms
 *
 * Hydra_Software_Devel/565   11/10/11 9:16p vanessah
 * SW7425-1115: add MCVP/MADR seamless transaction
 *
 * Hydra_Software_Devel/564   11/4/11 3:23p pntruong
 * SW7231-387: Enabled hddvi for b0 build.
 *
 * Hydra_Software_Devel/563   11/3/11 3:57p tdo
 * SW7344-135: Video lost in Vesa format on HDMI/Component outputs when
 * switching output formats during a playback.  IT stand-alone is not
 * working for 7344B0/7231B0
 *
 * Hydra_Software_Devel/562   10/27/11 4:29p syang
 * SW7425-1429: move per vsync call back from window to display
 *
 * Hydra_Software_Devel/561   10/20/11 2:21a hongtaoz
 * SW7425-1549: added two more loopback vnet paths for B0;
 *
 * Hydra_Software_Devel/560   10/19/11 1:48p vanessah
 * SW7425-1488: MADR RUL delay for NRT transcode
 *
 * Hydra_Software_Devel/559   10/6/11 4:12p pntruong
 * SW7429-16: Initial support.
 *
 * Hydra_Software_Devel/558   10/4/11 12:05p yuxiaz
 * SW7425-972: Bring up mosaic on CMP_2.
 *
 * Hydra_Software_Devel/557   9/28/11 4:16p pntruong
 * SW7425-969: Refactored mcvp/madr code to be multi-instances friendly.
 * Adapt to vnet naming changes.
 *
 * Hydra_Software_Devel/556   9/28/11 4:08p pntruong
 * HW7552-62, HW7428-4, HWBCM7231B0-81, HW7425-938: Corrected the version.
 *
 * Hydra_Software_Devel/555   9/23/11 10:36a pntruong
 * HW7552-62, HW7428-4, HWBCM7231B0-81, HW7425-938: Excluded work-arounds
 * for chips with fix.
 *
 * Hydra_Software_Devel/554   9/22/11 3:32p pntruong
 * SW3548-3090: Pruned ununsed code.
 *
 * Hydra_Software_Devel/553   9/9/11 10:54a pntruong
 * SW7425-31: Rev'ed up hddvi version for hddvi input.
 *
 * Hydra_Software_Devel/551   9/8/11 10:20p tdo
 * SW7425-922: VDC bringup for B0
 *
 * Hydra_Software_Devel/550   9/2/11 3:55p pntruong
 * SW7425-1165: Updated the number of dnr/lbox/mdi cores for b0.
 *
 * Hydra_Software_Devel/549   8/28/11 2:15p yuxiaz
 * SW7425-985, SW7552-100: Merge into mainline.Clean up feeder.
 *
 * Hydra_Software_Devel/SW7425-985/1   8/23/11 4:00p yuxiaz
 * SW7425-985, SW7552-100: Clean up for new MFD design.
 *
 * Hydra_Software_Devel/548   8/17/11 11:49a tdo
 * SW7425-979: Implement the new trigger
 *
 * Hydra_Software_Devel/547   8/15/11 4:09p pntruong
 * SW7425-982, SW7425-988: Rev'ed up hscaler and scaler version to include
 * additional register for derring control.
 *
 * Hydra_Software_Devel/546   8/12/11 2:01p pntruong
 * SW7231-318: Corrected number of vnet_f drains,  and added grpd/hddvi
 * support.
 *
 * Hydra_Software_Devel/545   8/11/11 3:59p pntruong
 * SW7231-318: Initial vdc support with error free build.
 *
 * Hydra_Software_Devel/544   8/8/11 6:25p vanessah
 * SW7425-990: add and/or logic to STG trigger
 *
 * Hydra_Software_Devel/543   8/5/11 2:05p yuxiaz
 * SW7425-985: Get 7425 B0 compiled.
 *
 * Hydra_Software_Devel/542   8/4/11 7:20p tdo
 * SW7425-979: Add support for 7425B0 DAC name change
 *
 * Hydra_Software_Devel/541   8/4/11 9:58a pntruong
 * SW7125-1028: Rollback clocking, as it still fail in some cases.
 *
 * Hydra_Software_Devel/540   7/22/11 11:13p pntruong
 * SW7425-983: Corrected deinterlacer's hscaler threshold.
 *
 * Hydra_Software_Devel/539   7/22/11 10:58a yuxiaz
 * SW7420-1959: Back out support of non-full content mode for PIP, use
 * mosaic APIs instead.
 *
 * Hydra_Software_Devel/538   7/12/11 3:28p yuxiaz
 * SW7358-40: Added second CRC support in VDC.
 *
 * Hydra_Software_Devel/537   6/30/11 6:50p yuxiaz
 * SW7420-1959: Added support of non-full content mode for PIP.
 *
 * Hydra_Software_Devel/536   6/24/11 2:57p albertl
 * SW7420-1966: Cleaned up and reorganized colorspace enums and indicies
 * in display, vdec, and cmp.
 *
 * Hydra_Software_Devel/535   6/22/11 3:07p yuxiaz
 * SW7425-592: Added API to get and set display sample aspect ratio
 * settings.
 *
 * Hydra_Software_Devel/534   6/1/11 11:41a pntruong
 * SW7231-153: Used fixed-rate to get around box detection error with
 * extended size.
 *
 * Hydra_Software_Devel/533   5/12/11 4:24p yuxiaz
 * SW7231-74: Added software workaround for MADR alignment hung.
 *
 * Hydra_Software_Devel/532   5/11/11 4:18p yuxiaz
 * SW7425-348: Fixed the bug when width is not multiple of 4.
 *
 * Hydra_Software_Devel/531   4/27/11 11:37a yuxiaz
 * SW7358-4: Updated VDC flags.
 *
 * Hydra_Software_Devel/530   4/25/11 5:40p yuxiaz
 * SW7425-348: Enable MADR alignment workaround for 7344, 7346 etc.
 *
 * Hydra_Software_Devel/529   4/25/11 3:49p yuxiaz
 * SW7425-348: Added software workaround for MADR alignment.
 *
 * Hydra_Software_Devel/528   4/18/11 4:52p vanessah
 * SW7425-375: fix the loopback source number
 *
 * Hydra_Software_Devel/527   4/1/11 5:26p pntruong
 * SW7425-267: Restricted the work-around for chips with madr only.
 *
 * Hydra_Software_Devel/526   3/31/11 8:21p vanessah
 * SW7422-118: double buffer PI support
 *
 * Hydra_Software_Devel/525   3/31/11 3:30p pntruong
 * SW7425-267: MADR input needs to be multiple of 4.
 *
 * Hydra_Software_Devel/524   3/24/11 2:18p pntruong
 * SW7425-243: Enabled stg's down sample.
 *
 * Hydra_Software_Devel/523   2/25/11 10:21a vanessah
 * SW7425-124: fix compile error for 7358 755 7231 7346 7344 7420 chip
 *
 * Hydra_Software_Devel/522   2/14/11 3:21p yuxiaz
 * SW7358-4: Updated VDC flags.
 *
 * Hydra_Software_Devel/521   1/20/11 2:41p yuxiaz
 * SW7422-97: Clean up macros for HDMI Rate manage
 *
 * Hydra_Software_Devel/520   1/17/11 1:48p pntruong
 * SW7550-645: Add GRPD support in VEC.
 *
 * Hydra_Software_Devel/519   1/12/11 4:34p yuxiaz
 * SW7335-1133: Added more debug messages for VDC heap debugging.
 *
 * Hydra_Software_Devel/518   1/7/11 4:46p yuxiaz
 * SW7422-174: Added DCX support to MCVP and MADR.
 *
 * Hydra_Software_Devel/517   1/4/11 4:06p yuxiaz
 * SW7422-174: Refactor DCX macros.
 *
 * Hydra_Software_Devel/516   12/29/10 3:47p pntruong
 * SW7422-174: Initial support for siob in madr.
 *
 * Hydra_Software_Devel/515   12/28/10 5:02p tdo
 * SW7422-148: Refactor MCVP to address chips w/wo MCTF
 *
 * Hydra_Software_Devel/514   12/23/10 7:11p tdo
 * SW7422-148: Fix compiling error
 *
 * Hydra_Software_Devel/513   12/23/10 5:12p tdo
 * SW7422-148: Refactor MCVP to address chips w/wo MCTF
 *
 * Hydra_Software_Devel/512   12/22/10 3:48p pntruong
 * SW7358-4: Updated window counts.
 *
 * Hydra_Software_Devel/SW7358-4/1   12/22/10 6:15p xhuang
 * SW7358-4: video window 1 is not support in 7358
 *
 * Hydra_Software_Devel/511   12/20/10 3:03p vanessah
 * SW7425-58:  New MCDI feature support
 *
 * Hydra_Software_Devel/510   12/17/10 11:37a pntruong
 * SW7358-17: Merged from 7358 emu bringup branch.
 *
 * Hydra_Software_Devel/509   12/16/10 4:01p yuxiaz
 * SW7422-35: Added new DRAIN HW version. Fixed build error on 7358, 7552,
 * 7231, 7346, 7344.
 *
 * Hydra_Software_Devel/508   12/16/10 1:05p vanessah
 * SW7425-58:  New MCDI feature support
 *
 * Hydra_Software_Devel/507   12/3/10 4:42p tdo
 * SW7422-36: Handle "left only" method for BVN internal data path
 *
 * Hydra_Software_Devel/506   12/2/10 11:50a vanessah
 * SW7335-1010:  MAD buffer corruption
 *
 * Hydra_Software_Devel/505   11/24/10 1:30p darnstein
 * SW7231-2: put 656 and DVO master mode back in for 7231, 7344, 7346.
 * This is for documentation purposes. The macros are not used for newer
 * chipsets.
 *
 * Hydra_Software_Devel/504   11/24/10 1:24p tdo
 * SW7422-52: Bring-up MAD-R
 *
 * Hydra_Software_Devel/502   11/23/10 10:10p tdo
 * SW7422-13: Correct number of suppported MASK DITHER block for 7422
 *
 * Hydra_Software_Devel/501   11/23/10 6:55p darnstein
 * SW7231-2:  No 656 or DVO master mode. Support histogram. No MCVP. All
 * of the above applies to 7344, 7346, 7231.
 *
 * Hydra_Software_Devel/500   11/19/10 6:22p darnstein
 * SW7231-2: No RFM output on 7344. Also, change IT hardware version.
 *
 * Hydra_Software_Devel/499   11/19/10 5:18p darnstein
 * SW7344-14: DNR hardware module is the same as 7422 after all.
 *
 * Hydra_Software_Devel/498   11/18/10 5:41p darnstein
 * SW7231-2: for 7344/7346/7231, change version of HSCL and DNR hardware.
 *
 * Hydra_Software_Devel/497   11/18/10 3:24p vanessah
 * SW7425-29:  STG support for 7425 & 7422
 *
 * Hydra_Software_Devel/496   11/18/10 10:43a vanessah
 * SW7425-29:  STG back off for 7422 compile
 *
 * Hydra_Software_Devel/495   11/18/10 9:54a vanessah
 * SW7425-29:  STG Mbox macro update
 *
 * Hydra_Software_Devel/494   11/16/10 4:07p yuxiaz
 * SW7422-97: Updated basic HDMI TX rate manager settings for 40nm chips.
 *
 * Hydra_Software_Devel/493   11/15/10 6:00p pntruong
 * SW7231-2: Initial support to make 7231 compile.
 *
 * Hydra_Software_Devel/492   11/15/10 4:40p tdo
 * SW7422-36: Handle "left only" method for BVN internal data path
 *
 * Hydra_Software_Devel/491   11/12/10 3:56p pntruong
 * SW7425-31: Takes bvn yuv into account for hddvi input.  Fixed bad debug
 * register read on non-mfd source.  Updated scratch registers
 * availabliity.
 *
 * Hydra_Software_Devel/490   11/11/10 4:15p tdo
 * SW7422-13: Add support for MFD debug with more than 2 LBOX in the BVN
 *
 * Hydra_Software_Devel/489   11/10/10 2:01p pntruong
 * SW7422-94: Disabled clock gating.
 *
 * Hydra_Software_Devel/488   11/10/10 11:40a tdo
 * SW7422-13: Fix IT version for 7422, Fix bus error and segfault with
 * MCVP enable.
 *
 * Hydra_Software_Devel/487   11/9/10 3:48p jessem
 * SW7422-82: Simplified validation for R surface.
 *
 * Hydra_Software_Devel/486   11/8/10 4:59p yuxiaz
 * SW7552-7: Add initial support for 7552.
 *
 * Hydra_Software_Devel/485   11/5/10 3:08p vanessah
 * SW7422-52:  Two kinds of deinterlacers in the same chip
 *
 * Hydra_Software_Devel/484   11/4/10 5:15p darnstein
 * SW7344-7: first cut at 7344 port. Simply copy settings from 7422.
 *
 * Hydra_Software_Devel/483   11/4/10 9:40a vanessah
 * SW7422-43:  bug fix for 7425
 *
 * Hydra_Software_Devel/482   11/3/10 11:47a pntruong
 * SW7340-230: Re-enable clock gating to save power for those chips that
 * has fix.
 *
 * Hydra_Software_Devel/481   11/1/10 4:23p jessem
 * SW7422-82: Added stereo 3D support to VFD as source feature.
 *
 * Hydra_Software_Devel/480   10/25/10 7:17p vanessah
 * SW7422-43:  Handle GFD programming for 3D.
 *
 * Hydra_Software_Devel/479   10/21/10 2:04p pntruong
 * SW7408-117: Added support for 7408B0 CMP changes.
 *
 * Hydra_Software_Devel/478   10/18/10 5:04p vanessah
 * SW7422-52:  Two deinterlacer on the same chip
 *
 * Hydra_Software_Devel/477   10/18/10 9:41a vanessah
 * SW7425-29:  coverity bug
 *
 * Hydra_Software_Devel/476   10/12/10 6:08p hongtaoz
 * SW7425-30: 7425 has 7 fch vnet crossbars;
 *
 * Hydra_Software_Devel/475   10/12/10 3:16p yuxiaz
 * SW7358-4: Updated BVDC_P_SUPPORT_CAP_VER for 7358.
 *
 * Hydra_Software_Devel/474   10/12/10 10:48a yuxiaz
 * SW7405-4936: Added BVDC_P_SUPPORT_COLOR_CLIP for 7342
 *
 * Hydra_Software_Devel/473   10/11/10 12:02p jessem
 * SW7420-173: Increased BVDC_P_MAX_SOURCE_COUNT to 18 reflecting the
 * additoin of VFD 0,1,2,3,4. Transferred surface record defines from GFD
 * and VFD since the GFX and VFD versions.
 *
 * Hydra_Software_Devel/472   10/7/10 11:45a yuxiaz
 * SW7422-38: Handle VDC changes to memory allocation for 3D.
 *
 * Hydra_Software_Devel/471   10/6/10 1:05p pntruong
 * SW7425-31: Initial support for hddvi.
 *
 * Hydra_Software_Devel/470   10/4/10 7:16p vanessah
 * SW7425-33:  Fix breaking 7420
 *
 * Hydra_Software_Devel/469   10/4/10 5:24p hongtaoz
 * SW7425-33: added the 4th compositor support;
 *
 * Hydra_Software_Devel/468   10/1/10 2:52p hongtaoz
 * SW7422-47: updated 7422/25 BVN error handler count;
 *
 * Hydra_Software_Devel/467   9/30/10 4:43p tdo
 * SW7422-51: VDC: Get TNT_H working on 7422
 *
 * Hydra_Software_Devel/466   9/30/10 4:31p tdo
 * SW7422-50: VDC: Get DNR_H working in 7422
 *
 * Hydra_Software_Devel/465   9/30/10 3:17p tdo
 * SW7422-47: VDC: Need to handle re-arragnement of core level interrupts
 *
 * Hydra_Software_Devel/464   9/29/10 11:40a tdo
 * SW7425-33: VDC: Add the 4th Compositor Support for 7422/7425
 *
 * Hydra_Software_Devel/463   9/28/10 11:34p tdo
 * SW7425-33: Remove compiling warning
 *
 * Hydra_Software_Devel/462   9/28/10 10:56p tdo
 * SW7425-33: VDC: Add the 4th Compositor Support for 7422/7425
 *
 * Hydra_Software_Devel/461   9/23/10 10:32a vanessah
 * SW7425-29:  STG support for transcoding
 *
 * Hydra_Software_Devel/460   9/21/10 6:10p yuxiaz
 * SW7422-31, SW7422-32: Handle single and dual buffer 3D cases in MFD.
 *
 * Hydra_Software_Devel/459   9/13/10 4:34p yuxiaz
 * SW7358-4: Added initial VDC support for 7358.
 *
 * Hydra_Software_Devel/458   9/8/10 3:24p hongtaoz
 * SW7425-13: enabled VEC SECAM bypass channels for 7425;
 *
 * Hydra_Software_Devel/457   8/30/10 4:14p tdo
 * SW7425-11, SW7425-13: Add 7425 support for VDC
 *
 * Hydra_Software_Devel/456   8/26/10 5:24p tdo
 * SW7422-57: Add simple vdc support
 *
 * Hydra_Software_Devel/455   8/5/10 11:33a pntruong
 * SW7408-117: Added support for 7408B0 CMP changes.
 *
 * Hydra_Software_Devel/454   8/3/10 1:57p pntruong
 * SW7125-424: Initialized the fields for game mode.  Corrected the
 * deinterlacer version.
 *
 * Hydra_Software_Devel/453   6/14/10 3:36p hongtaoz
 * SW3548-2976: soft init GFD when DCX switches from ON to OFF, and when
 * GFD source is created; added GFD BVB error monitoring to debug build;
 *
 * Hydra_Software_Devel/452   6/7/10 4:43p tdo
 * SW7208-57, SW7125-248: VEC resource management fine tuning
 *
 * Hydra_Software_Devel/451   5/18/10 1:21p pntruong
 * SW3548-2940: Corrected the macro for gfx/hscl testfeature1.
 *
 * Hydra_Software_Devel/450   5/7/10 7:06p albertl
 * SW7125-364: Changed dirty bits to use union structure to avoid type-pun
 * warnings.
 *
 * Hydra_Software_Devel/449   4/21/10 6:12p albertl
 * SW7125-180: Enabled7125  MAD support only for C0 and later.
 *
 * Hydra_Software_Devel/448   4/15/10 4:54p hongtaoz
 * SW3548-2828: changed the normalized fixed point match precision to
 * accomandate 12-bit picture size for 2160i;
 *
 * Hydra_Software_Devel/447   4/9/10 8:57p albertl
 * SW7125-180: Enabling 1080i MAD for 7125.
 *
 * Hydra_Software_Devel/446   4/5/10 3:59p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic
 *
 * Hydra_Software_Devel/445   3/18/10 3:59p syang
 * SW7550-271: added error check for unsupported 7550 GFD1 features
 *
 * Hydra_Software_Devel/444   2/9/10 2:09p rpan
 * SW7400-2642: Added SECAM output for orthogonal VEC.
 *
 * Hydra_Software_Devel/443   2/3/10 6:23p albertl
 * SW7125-180: Updated TESTFEATURE1 defines for 7125.
 *
 * Hydra_Software_Devel/442   2/3/10 1:57p albertl
 * SW7125-180: Changed BVDC_P_SUPPORT_VIDEO_TESTFEATURE1_VER to 3.
 *
 * Hydra_Software_Devel/441   2/2/10 7:33p albertl
 * SW7125-180: Changed 7125 DVI_DVF version to 1.
 *
 * Hydra_Software_Devel/440   2/1/10 6:36p hongtaoz
 * SW3548-2678, SW3548-2680, SW3548-2681: added HDMI1.4 3D formats support
 * for 3548; added multi-buffer low delay mode support for 1to2 and 2to5
 * rate gap cases; reduced capture buffer allocation for interlaced
 * pulldown case;
 *
 * Hydra_Software_Devel/439   1/26/10 5:42p yuxiaz
 * SW7405-3839: Remove the workaround for PR38730
 *
 * Hydra_Software_Devel/438   1/6/10 3:41p rpan
 * SW7468-64: Consolidate VEC modulo count trigger implementation.
 *
 * Hydra_Software_Devel/437   12/29/09 1:05p syang
 * SW7408-13:  add VEC_TRIGGER MODULO_COUNT setting for 7408
 *
 * Hydra_Software_Devel/436   12/23/09 2:02p tdo
 * SW7408-26: Add new TNT support
 *
 * Hydra_Software_Devel/BCM3D/1   12/17/09 6:08p hongtaoz
 * SW3548-2680, SW3548-2681: added HDMI1.4 3D format detection and
 * handling for 11-bit BVN;
 *
 * Hydra_Software_Devel/BCM3D/1   12/17/09 6:08p hongtaoz
 * SW3548-2680, SW3548-2681: added HDMI1.4 3D format detection and
 * handling for 11-bit BVN;
 *
 * Hydra_Software_Devel/435   12/14/09 5:27p yuxiaz
 * SW7408-13: More VDC support for 7408.
 *
 * Hydra_Software_Devel/434   12/7/09 11:16a rpan
 * SW7468-20: Updated 7468 configurations.
 *
 * Hydra_Software_Devel/433   12/1/09 3:54p yuxiaz
 * CR7340-133: BVN MFD Move the pixel saturation control from the test
 * mode register to the feeder control register.
 *
 * Hydra_Software_Devel/432   11/30/09 10:33a rpan
 * SW7468-30: Initial OSCL support.
 *
 * Hydra_Software_Devel/431   11/19/09 10:34a rpan
 * SW7468-31: Comply with the VDC version scheme.
 *
 * Hydra_Software_Devel/430   11/19/09 10:15a pntruong
 * SW7408-13: Initial check in to get 7408 build.
 *
 * Hydra_Software_Devel/429   11/18/09 3:05p rpan
 * SW7468-31: Initial work for 7468 vertical block only DNR.
 *
 * Hydra_Software_Devel/428   11/18/09 9:33a rpan
 * SW7468-20: 7468 DVI.DVF configuration.
 *
 * Hydra_Software_Devel/427   11/17/09 2:44p rpan
 * SW7468-20: 7468 work.
 *
 * Hydra_Software_Devel/426   11/10/09 3:07p rpan
 * SW7468-20: Initial support for 7468.
 *
 * Hydra_Software_Devel/425   11/3/09 4:22p albertl
 * SW7125-51: Enabled TNT for 7125.
 *
 * Hydra_Software_Devel/424   10/27/09 10:53p albertl
 * SW7420-30: Changed mosaic matrices to be loaded statically with xvYcc
 * matrix loaded dynamically as necessary.
 *
 * Hydra_Software_Devel/423   10/23/09 5:16p pntruong
 * SW7340-54, SW7342-48, SW7340-57, SW7340-56, SW7125-36: Temporary don't
 * enable clock gating.
 *
 * Hydra_Software_Devel/422   10/12/09 4:36p rpan
 * SW7405-3159: Moved writer/reader time stamp register from CAP/VFD to
 * RDC scratch registers.
 *
 * Hydra_Software_Devel/421   10/9/09 5:28p syang
 * SW7405-2516, SW3548-2439, SW7405-3046: rewrite HSCL setinfo_isr and
 * TryXferHrzSclToHscl_isr to do crop correctly; cleanup width alignment
 * for DCX_HSIZE_WORKAROUND
 *
 * Hydra_Software_Devel/420   10/8/09 11:16a tdo
 * SW7340-7: Cleanup VCXO macro defines
 *
 * Hydra_Software_Devel/419   10/8/09 9:20a tdo
 * SW7340-7: Fix number of VXCO_RM
 *
 * Hydra_Software_Devel/418   9/24/09 4:00p tdo
 * SW7125-27 : Correct the number of DACs and share SECAM resource for
 * 7125
 *
 * Hydra_Software_Devel/417   9/14/09 2:08p pntruong
 * SW3548-2439: Improved debug messages.
 *
 * Hydra_Software_Devel/416   8/27/09 10:22p pntruong
 * SW7125-10: Fixed GFD version to reflect new rdb update.
 *
 * Hydra_Software_Devel/415   8/27/09 3:54p tdo
 * SW7420-335: Disable compositor constant color feature for all 7420
 *
 * Hydra_Software_Devel/414   8/27/09 2:56p rpan
 * SW7420-335: Disable compositor constant color feature on 7420 B0.
 *
 * Hydra_Software_Devel/413   8/27/09 11:43a darnstein
 * SW7342-4: fix some counts and version numbers for 7340 and 7342.
 *
 * Hydra_Software_Devel/413   8/27/09 11:30a darnstein
 * SW7342-4,SW7340-7: Fix some more counts and version numbers for 7340
 * and 7342.
 *
 * Hydra_Software_Devel/412   8/25/09 7:44p albertl
 * SW7125-10: Initial 7125 support.
 *
 * Hydra_Software_Devel/411   8/25/09 6:37p darnstein
 * SW7340-30: I accidentally polluted the 7336 settings in previous check-
 * in.
 *
 * Hydra_Software_Devel/410   8/25/09 11:55a darnstein
 * SW7340-30: Correct some core counts and versiosn for 7340 chipset.
 *
 * Hydra_Software_Devel/409   7/24/09 6:04p syang
 * PR 56664: increase the delayStart of the low vert freq (<50) fmt from
 * cmp input to 18
 *
 * Hydra_Software_Devel/408   7/22/09 4:06p pntruong
 * PR56792: [config9] Repeated application of scaling (view modes) causes
 * loss of video/graphics on 1080i YPrPb and HDMI source.
 *
 * Hydra_Software_Devel/407   6/25/09 1:30p rpan
 * PR56137: Removed unused resource type CMP.
 *
 * Hydra_Software_Devel/406   6/25/09 12:11p rpan
 * PR56137, PR56138, PR56139, PR56166, PR56167, PR56168: Support for
 * various orthogonal VEC configurations.
 *
 * Hydra_Software_Devel/405   6/18/09 6:45p syang
 * PR 55877, PR 55871, PR 56127: fix scl/mad/hscl compiling for 7342
 *
 * Hydra_Software_Devel/404   6/18/09 6:12p syang
 * PR 55812: fix a merge error
 *
 * Hydra_Software_Devel/403   6/18/09 5:50p syang
 * PR 55877, PR 55871, PR 56127, PR 55812: update to support 7340 and 7550
 *
 * Hydra_Software_Devel/402   6/18/09 3:28p darnstein
 * PR54876: add support for 7342 chipset.
 *
 * Hydra_Software_Devel/401   6/15/09 3:08p darnstein
 * PR55879: fix number of loopbacks and drains for 7340. Also fix number
 * of drains for 7420.
 *
 * Hydra_Software_Devel/400   6/11/09 4:03p darnstein
 * PR55219: fill in some details for 7340 chipset.
 *
 * Hydra_Software_Devel/399   5/4/09 6:00p pntruong
 * PR53954: B2: DVP LVDS spread spectrum feature causes a DC shift in
 * frequency and an irregular spreading.  Compiled out for b2.
 *
 * Hydra_Software_Devel/398   4/17/09 11:57a pntruong
 * PR54064: Refactored common code for defered callback til bvn shutdown.
 * Improved readability and extensibility of dirty bits.
 *
 * Hydra_Software_Devel/397   4/6/09 3:42p syang
 * PR  53484 , PR 52037: cleanup (rect, scl-ratio, ..) rounding code and
 * AdjustRect code ; add 1 line delay to TV chip "early trigger"; change
 * BVDC_P_DST_CUT_TOP_THRESH to 4 for 3548;
 *
 * Hydra_Software_Devel/396   4/2/09 4:59p pntruong
 * PR53756: Relaxed the trigger point to not overflow into the next field.
 *
 * Hydra_Software_Devel/395   3/27/09 4:26p albertl
 * PR52810: Constant color fixed during format changes and re-enabled.
 *
 * Hydra_Software_Devel/394   3/20/09 10:36a pntruong
 * PR53429: Corrected the mad/anr mem saving flag.
 *
 * Hydra_Software_Devel/393   3/19/09 11:35p pntruong
 * PR53429: Corrected the anr/mad/hscl vnet connection.  Removed ununsed
 * bAnrMadCombo flags.
 *
 * Hydra_Software_Devel/392   3/16/09 10:45p tdo
 * PR45785, PR45789: Merge from MCVP branch
 *
 * Hydra_Software_Devel/7420_mcvp/16   3/16/09 1:01p tdo
 * PR45785, PR45789: merge from main branch on 3/16/09
 *
 * Hydra_Software_Devel/391   3/13/09 5:28p syang
 * PR 52376, PR 41481, PR 44041:
 * 1). exec user buf returning to hBuffer before shut-down code in
 * writer_isr; 2). clean up mad-delay handling code and add handling to
 * all settings in pic-node that are delayed by mad; 3). 444 SCL no-
 * longer uses BVN_IN to align-up for half-band; 4). add pic-node rects
 * dump feature as setting lbox_0_scratch to 2 (1 still printing mpeg pic
 * info).
 *
 * Hydra_Software_Devel/7420_mcvp/15   3/11/09 1:10p tdo
 * PR45785, PR45789: merge from main branch on 3/11/09
 *
 * Hydra_Software_Devel/390   3/10/09 11:10a tdo
 * PR52995: Need to set dynamics default to full range
 *
 * Hydra_Software_Devel/7420_mcvp/14   3/4/09 6:06p tdo
 * PR45785, PR45789: 7420 ANR is 8-bit
 *
 * Hydra_Software_Devel/7420_mcvp/13   3/4/09 5:23p tdo
 * PR45785, PR45789: merge from main branch on 3/04/09
 *
 * Hydra_Software_Devel/389   3/4/09 3:04p tdo
 * PR52009: Temporarily disable const color support for 7420
 *
 * Hydra_Software_Devel/7420_mcvp/12   2/26/09 4:10p tdo
 * PR 45785, PR 45789: merge from main branch on 2/26/09
 *
 * Hydra_Software_Devel/388   2/26/09 3:23p albertl
 * PR51925: Added CMP_1 G0 support to 3548.
 *
 * Hydra_Software_Devel/7420_mcvp/11   2/25/09 10:19a tdo
 * PR 45785, PR 45789: fix 7335 compile error
 *
 * Hydra_Software_Devel/7420_mcvp/10   2/23/09 12:17p tdo
 * PR 45785, PR 45789: merge from main branch on 2/23/09
 *
 * Hydra_Software_Devel/387   2/20/09 8:25p albertl
 * PR52009: Enabled window constant color for 7420.  Disabled for all
 * other current platforms.
 *
 * Hydra_Software_Devel/386   2/20/09 1:54p hongtaoz
 * PR51336: 3548B2 HW fixed VDEC short field problem; no need to toggle
 * PAL_SEPARATION for noisy input;
 *
 * Hydra_Software_Devel/385   2/10/09 7:06p albertl
 * PR45798: Updated 7420 cap version to 2 to fix broken capture.
 *
 * Hydra_Software_Devel/7420_mcvp/6   2/9/09 5:18p tdo
 * PR 45785, PR 45789: merge from main branch on 2/9/09
 *
 * Hydra_Software_Devel/384   2/9/09 5:12p tdo
 * PR45804: Add secam work around for 7420 A1 as well
 *
 * Hydra_Software_Devel/7420_mcvp/5   2/9/09 1:17p tdo
 * PR 45785, PR 45789: merge from main branch on 2/9/09
 *
 * Hydra_Software_Devel/383   2/9/09 12:08p rpan
 * PR51599: Updated HDMI PLL settings.
 *
 * Hydra_Software_Devel/7420_mcvp/4   2/6/09 2:22p tdo
 * PR 45785, PR 45789: merge from main branch on 2/6/09
 *
 * Hydra_Software_Devel/382   2/5/09 4:49p yuxiaz
 * PR51160, PR51327: Add Hsize Alignment for DCX Workaround
 *
 * Hydra_Software_Devel/7420_mcvp/3   2/3/09 7:22p syang
 * PR 45785, PR 45789: merge from main branch on 2/3/09
 *
 * Hydra_Software_Devel/381   2/3/09 9:38a rpan
 * PR45804: Implemented another scheme of attaching DVI slave path to
 * analog master path, which resets master path.
 *
 * Hydra_Software_Devel/380   1/31/09 3:32a albertl
 * PR45798: Fixed bugs and enabled mosaic mode csc implementation.
 *
 * Hydra_Software_Devel/379   1/30/09 5:09p yuxiaz
 * PR51713: Reset CAP during DCX mode switch. Clean up DCX software
 * workaround.
 *
 * Hydra_Software_Devel/7420_mcvp/2   1/29/09 5:00p syang
 * PR 45785, PR 45789: merge from main branch on 1/29/09
 *
 * Hydra_Software_Devel/7420_mcvp/2   1/29/09 4:30p syang
 * PR 45785, PR 45789: merge from main branch on 1/29/09
 *
 * Hydra_Software_Devel/378   1/28/09 3:41p yuxiaz
 * PR51335: Remove CAP overflow workaround for B2.
 *
 * Hydra_Software_Devel/377   1/27/09 8:58p tdo
 * PR51627: add VDC 7336 PI support
 *
 * Hydra_Software_Devel/7420_mcvp/1   1/23/09 11:30p syang
 * PR 45785, PR 45789: add PI support for new the new module  MCVP
 * (MCTF+MCDI)
 *
 * Hydra_Software_Devel/376   1/21/09 10:13a rpan
 * PR45804: More HDMI output work.
 *
 * Hydra_Software_Devel/375   1/15/09 6:19p hongtaoz
 * PR50621: repositioned trigger locations to as early as possible for DTV
 * display timing to have enough head room for the longest RULs;
 *
 * Hydra_Software_Devel/374   1/12/09 1:38p albertl
 * PR45798: Added CMP Mosaic CSC implementation for 7420.
 *
 * Hydra_Software_Devel/373   1/6/09 4:18p tdo
 * PR45811: Add HIST support and use correct version of HIST for 7420
 *
 * Hydra_Software_Devel/372   1/5/09 8:12p darnstein
 * PR45737: The 7420 chipset is able to output 1080P@60Hz and 1080P@50Hz.
 *
 * Hydra_Software_Devel/371   12/22/08 11:47p pntruong
 * PR50652: Added support for 7405c0.
 *
 * Hydra_Software_Devel/370   12/10/08 2:04p albertl
 * PR45808: Updated 7420 CSC support and reworked CMP CSC versioning.
 *
 * Hydra_Software_Devel/369   12/9/08 8:51p tdo
 * PR45804: Add Dac support
 *
 * Hydra_Software_Devel/368   12/5/08 11:15a yuxiaz
 * PR45800: Enable Dering Filter for CMP_0 Video Surfaces on 7420.
 *
 * Hydra_Software_Devel/367   12/4/08 6:37p pntruong
 * PR49979: For chips with vdb let app dictate wheather to capture or not
 * for mpeg, since vdc does not have this information.
 *
 * Hydra_Software_Devel/366   12/3/08 7:53p pntruong
 * PR45817: Added hddvi support.
 *
 * Hydra_Software_Devel/365   12/3/08 3:11p rpan
 * PR45804: Added define for orthogonal VEC support.
 *
 * Hydra_Software_Devel/364   11/24/08 4:22p albertl
 * PR49415: Fixed FORWARD_NULL coverity issue.
 *
 * Hydra_Software_Devel/363   11/24/08 11:03a yuxiaz
 * PR49598: Use bandwidth equation to calculate throughput.
 *
 * Hydra_Software_Devel/362   11/17/08 4:11p darnstein
 * PR45819: Update scratch register trickery to support ITU-R 656 output,
 * in addition to analog output.
 *
 * Hydra_Software_Devel/361   11/4/08 12:35p tdo
 * PR45804: Implement and Bringup Orthorgonal VEC
 *
 * Hydra_Software_Devel/360   10/23/08 9:38p tdo
 * PR46484:Initial VDC bringup for 7420
 *
 * Hydra_Software_Devel/359   10/16/08 3:53p tdo
 * PR46484: Bring-up of Application Framework for 7420
 *
 * Hydra_Software_Devel/358   10/3/08 12:01p syang
 * PR42693:  improve BVDC_P_EQ_DELTA to handle negative diff case
 *
 * Hydra_Software_Devel/357   10/3/08 11:00a yuxiaz
 * PR46305, PR46307, PR46309: Use new TestFeature1 function from BSUR.
 * Change BVDC_P_SUPPORT_VIDEO_TestFeature1 to
 * BVDC_P_SUPPORT_VIDEO_TESTFEATURE1.
 *
 * Hydra_Software_Devel/356   10/3/08 9:23a yuxiaz
 * PR46305, PR46307, PR46309: More Video TestFeature1 support.
 *
 * Hydra_Software_Devel/355   9/30/08 6:30p hongtaoz
 * PR46738: fixed MAD/ANR memory saving mode;
 *
 * Hydra_Software_Devel/354   9/30/08 4:56p yuxiaz
 * PR46305, PR46307: Video TestFeature1 support: Fix throughput
 * calculation, combine MAD and ANR set TestFeature1 function.
 *
 * Hydra_Software_Devel/353   9/30/08 12:31p jessem
 * PR 46489: Added TN2T support for 7420.
 *
 * Hydra_Software_Devel/352   9/29/08 3:02p tdo
 * PR47349: Always disable DITH in DCR block for 10-bit chips.  Update new
 * DCR config table.
 *
 * Hydra_Software_Devel/351   9/29/08 1:13p pntruong
 * PR47072: Removed debug printing.
 *
 * Hydra_Software_Devel/350   9/29/08 12:58p pntruong
 * PR47072: Moving 3563 vdc dedicated branch.
 *
 * Hydra_Software_Devel/349   9/26/08 3:23p yuxiaz
 * PR46305, PR46307: More video TestFeature1 support for 3548 B0.
 *
 * Hydra_Software_Devel/348   9/22/08 4:52p yuxiaz
 * PR46305, PR46307, PR46309: Added video TestFeature1 support for 3548 B0.
 *
 * Hydra_Software_Devel/347   9/17/08 8:43p pntruong
 * PR47013: B0: Initial vdc support.
 *
 * Hydra_Software_Devel/346   9/17/08 6:00p albertl
 * PR46482: Changed 3548 B0 to use BVDC_P_SUPPORT_CSC_MAT_COEF_VER version
 * 3.
 *
 * Hydra_Software_Devel/345   9/17/08 3:47p tdo
 * PR46482: B0:Change FIR programming for SCL block
 *
 * Hydra_Software_Devel/344   9/17/08 11:48a pntruong
 * PR47013: Initial vdec b0 build support.
 *
 * Hydra_Software_Devel/343   9/16/08 10:17p pntruong
 * PR46494: B0: Remove 3rd output dac.
 *
 * Hydra_Software_Devel/342   9/16/08 5:58p albertl
 * PR45798:  Updated clear rect defines for 7420.
 *
 * Hydra_Software_Devel/341   9/12/08 4:21p yuxiaz
 * PR46497: Fix CRC register processing in MFD for 3548 B0.
 *
 * Hydra_Software_Devel/340   9/12/08 3:17p tdo
 * PR46745: Use per-picture enable in DNR block for 3548 B0
 *
 * Hydra_Software_Devel/339   9/11/08 11:26a yuxiaz
 * PR46623: Add the code to enabe the endian swap bit for FGT.
 *
 * Hydra_Software_Devel/338   9/9/08 9:03p tdo
 * PR45802: Remove unnecessary macros and unused code
 *
 * Hydra_Software_Devel/337   9/9/08 2:03p tdo
 * PR46492: Remove luma stats from compositor for 3548B0
 *
 * Hydra_Software_Devel/336   9/8/08 5:40p tdo
 * PR45811, PR45810, PR45814: Support DNR changes, support PEP changes,
 * and remove 656in support for 7420
 *
 * Hydra_Software_Devel/335   9/5/08 4:45p tdo
 * PR46484: Bringup appframework for7420
 *
 * Hydra_Software_Devel/334   9/4/08 8:30p pntruong
 * PR46502: Moving 7403 vdc dedicated branch.
 *
 * Hydra_Software_Devel/333   8/21/08 6:03p tdo
 * PR45983: Update DNR version for 7400E0
 *
 * Hydra_Software_Devel/332   8/13/08 3:41p syang
 * PR44041:  add frac bits for  BVDC_P_PRINT_SCLCUT_RECT
 *
 * Hydra_Software_Devel/331   7/29/08 7:49p pntruong
 * PR40213: Used correct alignment macro.
 *
 * Hydra_Software_Devel/330   7/23/08 7:01p tdo
 * PR43508, PR43509: Mapping individual bandgap adjustment for each DAC
 *
 * Hydra_Software_Devel/329   7/9/08 5:02p tdo
 * PR41932: Add support for VDC 7335B0 changes.
 *
 * Hydra_Software_Devel/328   6/24/08 6:57p hongtaoz
 * PR40499: added ANR error debug callback;
 *
 * Hydra_Software_Devel/327   6/17/08 6:02p tdo
 * PR43492: Set proper version of TNT for3548/3556
 *
 * Hydra_Software_Devel/326   6/17/08 5:28p tdo
 * PR39333: Connect HIST at reader side for VDEC source as a work around
 *
 * Hydra_Software_Devel/325   6/11/08 4:24p hongtaoz
 * PR43460: add HSCL for ANR or MAD usage mode; HSCL seems to shield BVB
 * error going to ANR;
 *
 * Hydra_Software_Devel/324   6/9/08 2:42p albertl
 * PR39336:  Fixed BVDC_P_SUPPORT_TER_VEC macro.
 *
 * Hydra_Software_Devel/323   6/7/08 8:21p albertl
 * PR39336:  Added proper second compositor support for 3548 and changed
 * macros to distinguish between compositor and vec support.
 *
 * Hydra_Software_Devel/322   6/5/08 5:03p hongtaoz
 * PR39807: 3548A0 bringup; put SCL in always_enable mode; separate
 * anr+mad combo mode and memory saving mode in macroes;
 *
 * Hydra_Software_Devel/321   6/4/08 5:11p yuxiaz
 * PR39807: VDC bring up on 3548: Turn off MAD/ANR memory saving mode for
 * now.
 *
 * Hydra_Software_Devel/320   6/4/08 1:44p rpan
 * PR39421: Added LVDS PLL settings for 3548/3556.
 *
 * Hydra_Software_Devel/319   5/29/08 11:58p pntruong
 * PR34711: Updated number of adc.
 *
 * Hydra_Software_Devel/318   5/16/08 1:59p rpan
 * PR42109: SECAM variants support.
 *
 * Hydra_Software_Devel/317   4/23/08 1:23p yuxiaz
 * PR39175: Add 7325 B0 support in VDC.
 *
 * Hydra_Software_Devel/316   4/21/08 11:14p pntruong
 * PR34711: Prepared to add new features into vdec.  Removed vdec1
 * support.
 *
 * Hydra_Software_Devel/315   4/8/08 9:11a rpan
 * PR41430: Added 7400 E0 SIMPLE_LUMA_CORE_CTRL support.
 *
 * Hydra_Software_Devel/314   4/2/08 11:41a rpan
 * PR39159: Implemented the VEC/DAC side support for SD video pass-
 * through.
 *
 * Hydra_Software_Devel/313   4/1/08 3:05p rpan
 * PR39337: Implemented CMP OSD configuration.
 *
 * Hydra_Software_Devel/312   3/31/08 2:35p jessem
 * PR 39335: Added CM3D support.
 *
 * Hydra_Software_Devel/311   3/27/08 10:03p pntruong
 * PR41004: Alloc full 32-bit scratch regs from general pool. To fix
 * fe_size read for pip.
 *
 * Hydra_Software_Devel/310   3/27/08 8:23p albertl
 * PR39163:  Fixed compile errors on 3563.
 *
 * Hydra_Software_Devel/309   3/27/08 5:19p albertl
 * PR39163:  Added non-linear csc window macros for 3548.
 *
 * Hydra_Software_Devel/308   3/27/08 5:06p albertl
 * PR39163:  Implemented non-linear xvYCC csc support for 3548.
 *
 * Hydra_Software_Devel/307   3/26/08 7:02p pntruong
 * PR40448: Re-arraged resource info table and added place holder for
 * pcpll resource.
 *
 * Hydra_Software_Devel/306   3/26/08 1:14p hongtaoz
 * PR35398, PR40895: added ulVsyncRate and removed bGameModeDelayLags in
 * the window lipsync callback structure; added BFMT_FREQ_FACTOR to unify
 * Vsync rate and pixel frequency calculation;
 *
 * Hydra_Software_Devel/305   3/21/08 12:21p hongtaoz
 * PR39154, PR39202: updated MAD revision ID and validation for 3548;
 *
 * Hydra_Software_Devel/304   3/21/08 12:10p tdo
 * PR39333: More implementation of HIST block
 *
 * Hydra_Software_Devel/303   3/19/08 7:37p hongtaoz
 * PR39154, PR39202, PR39203: combined ANR and MAD in memory saving mode
 * on 3548; implemented MAD specific change; updated SCL precision for
 * 3548;
 *
 * Hydra_Software_Devel/302   3/19/08 5:44p tdo
 * PR39333: Implement HIST core
 *
 * Hydra_Software_Devel/301   3/14/08 3:51p pntruong
 * PR40456: Already fixed in rdc no need to waste space for being on 256
 * boundary.
 *
 * Hydra_Software_Devel/300   3/5/08 1:42p tdo
 * PR39417: VDC PI support 3556
 *
 * Hydra_Software_Devel/299   2/29/08 4:17p yuxiaz
 * PR39158: Implement dithering in various BVN components for 3548.
 *
 * Hydra_Software_Devel/298   2/28/08 10:11p pntruong
 * PR39154: Resolved clearcase merged.
 *
 * Hydra_Software_Devel/297   2/28/08 6:22p hongtaoz
 * PR39154, PR36273, PR39202: added HSCL block; enabled MAD/ANR for 3548;
 * fixed non-linear scaling when MAD is on;
 *
 * Hydra_Software_Devel/294   2/20/08 4:14p hongtaoz
 * PR39154: added window API to toggle ANR; prepare for combo ANR/MAD
 * feature on 3548;
 *
 * Hydra_Software_Devel/293   2/19/08 4:51p rpan
 * PR39421: Updated bcm3548 error callback count.
 *
 * Hydra_Software_Devel/292   2/19/08 4:36p rpan
 * PR39421: Updated bcm3548 max DAC count.
 *
 * Hydra_Software_Devel/291   2/19/08 3:36p hongtaoz
 * PR39154: re-organize deinterlacer code to be mroe readable;
 *
 * Hydra_Software_Devel/290   2/19/08 9:57a rpan
 * PR39421: Added DVI color converter block support to bcm3548.
 *
 * Hydra_Software_Devel/289   2/15/08 2:33p yuxiaz
 * PR34712, PR39251: Use feeder version to combine all the feeder flags.
 * Regroup feeder support based on feeder version.
 *
 * Hydra_Software_Devel/288   2/14/08 6:05p pntruong
 * PR39321: Implement hardware changes for HD_DVI.
 *
 * Hydra_Software_Devel/287   2/14/08 4:45p tdo
 * PR39331, PR39332: Remove duplicate definition
 *
 * Hydra_Software_Devel/286   2/14/08 4:31p tdo
 * PR39331, PR39332: Regroup PEP block to add support for 3548
 *
 * Hydra_Software_Devel/285   2/14/08 9:49a pntruong
 * PR39421: Removed warnings for2.6.18.0_gcc-4.2 toolchains.
 *
 * Hydra_Software_Devel/284   2/13/08 9:56p pntruong
 * PR39421: Now vdc compiled for 3548.
 *
 * Hydra_Software_Devel/283   2/13/08 6:16p pntruong
 * PR39421: Fixed build errors with updated rdb from cental rdb.
 *
 * Hydra_Software_Devel/282   2/13/08 3:10p pntruong
 * PR39421: Removed unused code to improve readability.
 *
 * Hydra_Software_Devel/281   2/13/08 12:24p rpan
 * PR39421: Got 3548 display files to compile.
 *
 * Hydra_Software_Devel/280   2/12/08 8:17p pntruong
 * PR38929: Make more files compile.
 *
 * Hydra_Software_Devel/279   2/12/08 7:42p tdo
 * PR38771, PR38777, PR39246: Implement changes for DNR/DCR cores for 3548
 *
 * Hydra_Software_Devel/278   2/11/08 4:18p rpan
 * PR39421: Added flag BVDC_P_SUPPORT_DAC_3. 3548 has only 3 DACs.
 *
 * Hydra_Software_Devel/277   2/7/08 9:03a yuxiaz
 * PR39322: Add 656 input support on 3548.
 *
 * Hydra_Software_Devel/276   2/5/08 2:42p yuxiaz
 * PR34712: Added H.264 support through MPEG feeder on 3548.
 *
 * Hydra_Software_Devel/275   1/28/08 10:18a yuxiaz
 * PR38885: Turn off FGT on 7405 B0 so 7415 A0 will be binary compatible
 * with 7405 B0.
 *
 * Hydra_Software_Devel/274   1/24/08 5:27p tdo
 * PR38710: Add blue stretch support
 *
 * Hydra_Software_Devel/273   1/22/08 3:31p yuxiaz
 * PR38885: Added 7405 B0 support.
 *
 * Hydra_Software_Devel/272   1/21/08 5:43p tdo
 * PR38709: Adjust TNT version to support 7400D0
 *
 * Hydra_Software_Devel/271   1/21/08 5:02p tdo
 * PR38709: Correctly use macro define for versions
 *
 * Hydra_Software_Devel/270   1/21/08 4:07p tdo
 * PR38709: [3563D0]: update sharpness/TNT settings
 *
 * Hydra_Software_Devel/269   1/18/08 12:08p hongtaoz
 * PR38756: added MAD to the BVN error debug handling;
 *
 * Hydra_Software_Devel/268   1/13/08 3:27a albertl
 * PR38150: Added 7440 HDMI xvYCC CSC support.
 *
 * Hydra_Software_Devel/267   12/7/07 3:39p pntruong
 * PR37978: Add Intial support for 3563D0.
 *
 * Hydra_Software_Devel/266   12/3/07 2:02p yuxiaz
 * PR37802, PR37704: Added HDMI support on 7325.
 *
 * Hydra_Software_Devel/265   11/28/07 4:47p pntruong
 * PR37175: [VDEC][WXGA] 3Dcomb fails when input is PAL/PAL-N/PAL-M format
 * with WXGA.  Need to allocate correct amount of memory for PAL.
 *
 * Hydra_Software_Devel/264   11/19/07 3:49p tdo
 * PR36898: Add VDC PI support for 7335
 *
 * Hydra_Software_Devel/263   11/15/07 3:52p yuxiaz
 * PR34523: 7325 bring up.
 *
 * Hydra_Software_Devel/262   11/13/07 12:44p yuxiaz
 * PR34523: 7325 bring up.
 *
 * Hydra_Software_Devel/261   11/9/07 5:15p yuxiaz
 * PR34523: Disable BVDC_P_SUPPORT_SCL_ENABLE_BY_PIC on 7325 for now.
 *
 * Hydra_Software_Devel/260   11/2/07 7:33p albertl
 * PR36701:  Added MUD functionality for 7440 C0.
 *
 * Hydra_Software_Devel/259   11/2/07 10:04a syang
 * PR 34773, PR 35313: improve msg for anr/mad buffer alloc
 *
 * Hydra_Software_Devel/258   11/1/07 2:22p pntruong
 * PR34624: Add VDC support for 7400D0.
 *
 * Hydra_Software_Devel/257   10/30/07 2:30p rpan
 * PR34798: Removed support for obsoleted versions of ANR.
 *
 * Hydra_Software_Devel/256   10/29/07 2:21p yuxiaz
 * PR34523: Added PEP support on 7325.
 *
 * Hydra_Software_Devel/255   10/23/07 11:15a yuxiaz
 * PR29569, PR36290: Add FGT support on 7405.
 *
 * Hydra_Software_Devel/PR29569/2   10/19/07 11:31a yuxiaz
 * PR29569: Merge from mainline.
 *
 * Hydra_Software_Devel/254   10/11/07 1:59p albertl
 * PR35135:  Fixed user csc macros and added 10-bit/8-bit video data
 * conversion.
 *
 * Hydra_Software_Devel/PR29569/1   10/12/07 10:48a yuxiaz
 * PR29569: Add FGT support on 7405
 *
 * Hydra_Software_Devel/252   9/27/07 2:00p yuxiaz
 * PR35013, PR34523: Add new CSC support on 7325.
 *
 * Hydra_Software_Devel/251   9/21/07 4:35p yuxiaz
 * PR34523: Add 7325 support for VDC.
 *
 * Hydra_Software_Devel/250   9/21/07 3:59p hongtaoz
 * PR34955: added Progressive Segmented Frame(PsF) 1080p support; fixed
 * 1080p pass-through condition; fixed bvdc_dbg register access error;
 * fixed general progressive capture interlaced playback feeder
 * programming problem;
 *
 * Hydra_Software_Devel/249   9/20/07 1:12a albertl
 * PR35135:  Cleaned up color space matrices and changed them to use same
 * macro system.  Added color space conversion functionality to graphics
 * windows.
 *
 * Hydra_Software_Devel/248   9/6/07 11:16a maivu
 * PR 34624: Added 7400D0 support
 *
 * Hydra_Software_Devel/247   9/4/07 5:32p jessem
 * PR 34590: Removed BVDC_P_Packing enum and replaced with actual packing
 * type defines in bchp_mfd.h Also, redefined
 * BVDC_P_CAP_PIXEL_FORMAT_8BIT422  to BPXL_eY18_Cb8_Y08_Cr8 since the
 * previous format is not supported by the VFD/MFD.
 *
 * Hydra_Software_Devel/246   8/30/07 10:27p pntruong
 * PR34472: Use MASK block to improve the Contour/Quantization noise
 * issue.
 *
 * Hydra_Software_Devel/245   8/30/07 12:04p pntruong
 * PR33614:  Removed vnet reset, and delay auto-trigger to avoid re-enable
 * errors in anr.
 *
 * Hydra_Software_Devel/244   8/24/07 6:28p pntruong
 * PR33256: Make it compilers friendly.
 *
 * Hydra_Software_Devel/243   8/20/07 3:42p albertl
 * PR33832:  Updated 7440 to use new PLL.
 *
 * Hydra_Software_Devel/242   8/17/07 9:59a pntruong
 * PR34059: Need to fix FMT name for 1024x768i_43Hz.
 *
 * Hydra_Software_Devel/PR34059/1   8/17/07 11:40a gorgon
 * PR34059: Need to fix FMT name for 1024x768i_43Hz
 *
 * Hydra_Software_Devel/241   8/14/07 5:39p pntruong
 * PR33499: Video bounce vertically when switch to some special channel.
 *
 * Hydra_Software_Devel/240   8/13/07 5:42p syang
 * PR 33614: add a vnet-reconfig for 3D comb off to on if anr is enabeld
 *
 * Hydra_Software_Devel/239   8/2/07 1:51p pntruong
 * PR31983: C0: Need to enable 10-bit MAD capture mode when source is
 * 480i.
 *
 * Hydra_Software_Devel/238   7/30/07 6:49p albertl
 * PR28869:  Removed VDC dependency on BAVC_MOSAIC_MAX.  Clear rect and
 * mosaic maximums are now dependent on definitions in bvdc_common.h.
 *
 * Hydra_Software_Devel/237   7/25/07 7:56p pntruong
 * PR33411: Source frequence change when change vnet mode with ANR and DNR
 * enable.
 *
 * Hydra_Software_Devel/236   7/20/07 5:01p pntruong
 * PR33256: [VDC] Dynamical RTS settings for Component/HDMI 1080i on 3553
 * is wrong.
 *
 * Hydra_Software_Devel/235   7/16/07 2:39p yuxiaz
 * PR29842: 7405 mosaic mode bring up.
 *
 * Hydra_Software_Devel/234   6/21/07 2:15p yuxiaz
 * PR32023: Added video format tolerance to HDDVI.
 *
 * Hydra_Software_Devel/233   6/21/07 5:21a albertl
 * PR31821:  Changed MAD game modes to allocate only the necessary number
 * of buffers and to reconfigure the vnet when buffer numbers change.
 *
 * Hydra_Software_Devel/232   6/15/07 4:36p syang
 * PR 31944:  add macro BVDC_P_CLEANUP_VNET
 *
 * Hydra_Software_Devel/231   6/11/07 1:54p yuxiaz
 * PR29769: Turn on BVDC_P_SUPPORT_SCL_ENABLE_BY_PIC for 3563 C0 and 7405.
 *
 * Hydra_Software_Devel/230   6/8/07 4:46p yuxiaz
 * PR30015: Added STRIPE_WIDTH_SEL settings in MFD.
 *
 * Hydra_Software_Devel/229   6/8/07 11:30a syang
 * PR 31849: separate drain from disable for MAD, ANR and CAP
 *
 * Hydra_Software_Devel/228   6/1/07 2:42p syang
 * PR 28541: more improve to sub-pixel msg print
 *
 * Hydra_Software_Devel/227   5/23/07 10:52a syang
 * PR 28541: improve sub-pixel msg print
 *
 * Hydra_Software_Devel/226   5/17/07 1:12p syang
 * PR 28550, PR 30732: add PI support for 3563 C0's new MCTF ANR; add
 * bypass mode for ANR
 *
 * Hydra_Software_Devel/225   5/14/07 11:40a pntruong
 * PR30427: Provide API to control black offset in VDEC.
 *
 * Hydra_Software_Devel/224   5/10/07 5:23p albertl
 * PR28892:  Added constant window color support for 3563 C0.
 *
 * Hydra_Software_Devel/223   5/10/07 4:21p yuxiaz
 * PR27644: Add TDAC settings for 7405. Clean up TDAC and QDAC flags.
 *
 * Hydra_Software_Devel/222   5/10/07 11:37a syang
 * PR 29846: remove enum eDrainVnet; clean up
 *
 * Hydra_Software_Devel/221   5/7/07 4:44p yuxiaz
 * PR27644: Enable BVDC_P_SUPPORT_DMISC on 7405.
 *
 * Hydra_Software_Devel/220   5/3/07 7:00p pntruong
 * PR29662: Added c0 dynamics rts settings.
 *
 * Hydra_Software_Devel/219   5/2/07 1:31p syang
 * PR 28541: added a macro
 *
 * Hydra_Software_Devel/218   5/1/07 4:42p syang
 * PR 28541: 1) changed non-linear scl api; 2). carry sub-pxl accuracy
 * through the pipe line by SrcHStep and SrcVStep
 *
 * Hydra_Software_Devel/217   4/26/07 5:57p tdo
 * PR28891, PR29658: C0: BVN: Add DCR (digital contour removal) support in
 * DNR block
 *
 * Hydra_Software_Devel/216   4/26/07 4:47p pntruong
 * PR28897: C0: BVN: move SCALER_ENABLE bit from TOP_CONTROL register to a
 * separate register.  Allow 3563c0 to build.
 *
 * Hydra_Software_Devel/215   4/26/07 3:43p pntruong
 * PR30071: [VDEC]Picture has horizontal shifting then goes back one
 * second later after switch input source to CVBS or NTSC.
 *
 * Hydra_Software_Devel/214   4/26/07 2:36p yuxiaz
 * PR27644, PR29767: Added 7405 support: move SCALER_ENABLE bit from
 * TOP_CONTROL register to a separate register.
 *
 * Hydra_Software_Devel/213   4/24/07 1:45p yuxiaz
 * PR27644: Added 7405 support.
 *
 * Hydra_Software_Devel/212   4/23/07 5:32p pntruong
 * PR29980: C0: Handle new MUX settings for HD_DVI.
 *
 * Hydra_Software_Devel/211   4/19/07 1:47p yuxiaz
 * PR 26052: Add CSC support in MFD on 7400 B0.
 *
 * Hydra_Software_Devel/210   4/18/07 7:03p pntruong
 * PR29782: VDEC: Offset for negative CSC coefficients lower than it
 * should be.
 *
 * Hydra_Software_Devel/209   4/15/07 1:49a albertl
 * PR28869:  Maximum clear rect now defined for each window and chipset.
 *
 * Hydra_Software_Devel/208   4/12/07 1:13p pntruong
 * PR29612: Added delay start to prevent garbage from feeding out on input
 * change or channel change.
 *
 * Hydra_Software_Devel/207   4/9/07 11:23a pntruong
 * PR28266: Provide API to customize DVO CSC.
 *
 * Hydra_Software_Devel/206   4/6/07 12:53p syang
 * PR 28037: auto turn off anr with HW src for 3563 A0 (only)
 *
 * Hydra_Software_Devel/205   4/4/07 3:43p syang
 * PR 29243: change VnetMode def (now use bit-fields)
 *
 * Hydra_Software_Devel/204   4/3/07 6:11p pntruong
 * PR28773: Use RDC read for VDEC_FE_0_FE_FIELD_SIZE to get consistence
 * values.  Use median averaging to remove toggling.  Update statics
 * settings to better match 3560bx.
 *
 * Hydra_Software_Devel/203   3/25/07 2:46p pntruong
 * PR28904: Do pedestal removal (for VDEC source) in CMP.
 *
 ***************************************************************************/
#ifndef BVDC_COMMON_PRIV_H__
#define BVDC_COMMON_PRIV_H__

#include "bkni.h"
#include "bchp_fmisc.h"

#ifdef __cplusplus
extern "C" {
#endif


/***************************************************************************
 * VDC Internal defines
 ***************************************************************************/

/***************************************************************************
 * Chip Features macro to aid conditional compilations!  This is to avoid
 * the many #ifdef (BCHP_CHIP==xxx) through out the vdc.
 ***************************************************************************/
#if (BCHP_CHIP==7400)

#if (BCHP_VER >= BCHP_VER_E0)
#define BVDC_P_SUPPORT_DNR_VER                (2)
#else
#define BVDC_P_SUPPORT_DNR_VER                (1)
#endif

#define BVDC_P_SUPPORT_656_MASTER_MODE        (1)
#define BVDC_P_SUPPORT_DVO_MASTER_MODE        (1)
#define BVDC_P_SUPPORT_VEC_MUXING             (1)
#define BVDC_P_SUPPORT_TDAC_VER               (2)
#define BVDC_P_SUPPORT_QDAC_VER               (1)
#define BVDC_P_SUPPORT_LOOP_BACK              (4)
#define BVDC_P_SUPPORT_DNR                    (2)
#define BVDC_P_SUPPORT_MODULAR_VEC            (1)
#define BVDC_P_SUPPORT_NEW_656_IN_VER         (2)
#define BVDC_P_SUPPORT_VEC_GRPD               (0)
#define BVDC_P_SUPPORT_VEC_SECAM              (1)
#define BVDC_P_SUPPORT_CAP_VER                (1)
#define BVDC_P_SUPPORT_CMP_CRC                (1)

/* source */
#define BVDC_P_SUPPORT_GFD                    (1)
#define BVDC_P_SUPPORT_GFD_VER                (1)
#define BVDC_P_SUPPORT_GFD1_VER               (1)
#define BVDC_P_SUPPORT_MFD                    (2)
#define BVDC_P_SUPPORT_MFD_VER                (3)
#define BVDC_P_SUPPORT_VDEC                   (0)
#define BVDC_P_SUPPORT_VDEC_VER               (0)
#define BVDC_P_SUPPORT_VDEC_PASSTHROUGH       (0)
#define BVDC_P_SUPPORT_HDDVI                  (0) /* (1) Not supported */
#define BVDC_P_SUPPORT_HDDVI_VER              (1)
#define BVDC_P_SUPPORT_DOWNSAMPLE             (0)
#define BVDC_P_SUPPORT_VCR_MODE               (0)

/* BVN */
#define BVDC_P_SUPPORT_PEP                    (1)
#define BVDC_P_SUPPORT_PEP_VER                (1)
#define BVDC_P_SUPPORT_HIST                   (1)
#define BVDC_P_SUPPORT_HIST_VER               (1)

#if (BCHP_VER >= BCHP_VER_D0)
#define BVDC_P_SUPPORT_TAB                    (0)
#define BVDC_P_SUPPORT_TNT                    (1)
#if (BCHP_VER >= BCHP_VER_E0)
#define BVDC_P_SUPPORT_TNT_VER                (4)
#define BVDC_P_SUPPORT_FGT_VER                (1)
#else
#define BVDC_P_SUPPORT_TNT_VER                (2)
#define BVDC_P_SUPPORT_FGT_VER                (0)
#endif
#define BVDC_P_SUPPORT_MASK_DITHER            (3)
#define BVDC_P_SUPPORT_SCL_VER                (2)
#define BVDC_P_SUPPORT_FGT                    (1)
#else
#define BVDC_P_SUPPORT_TAB                    (1)
#define BVDC_P_SUPPORT_TNT                    (0)
#define BVDC_P_SUPPORT_TNT_VER                (0)
#define BVDC_P_SUPPORT_MASK_DITHER            (0)
#define BVDC_P_SUPPORT_SCL_VER                (1)
#define BVDC_P_SUPPORT_FGT                    (0)
#define BVDC_P_SUPPORT_FGT_VER                (0)
#endif

#define BVDC_P_SUPPORT_MAD_VER                (3)
#define BVDC_P_SUPPORT_BOX_DETECT             (2)
#define BVDC_P_SUPPORT_BOX_DETECT_VER         (1)
#define BVDC_P_SUPPORT_CAP                    (4)
#define BVDC_P_SUPPORT_VFD                    (4)
#define BVDC_P_SUPPORT_SCL                    (4)
#define BVDC_P_SUPPORT_HSCL                   (0)
#define BVDC_P_SUPPORT_HSCL_VER               (0)
#define BVDC_P_SUPPORT_HSCL_MAD_HARD_WIRED    (0)
#define BVDC_P_SUPPORT_FMISC_PFRI             (0)
#define BVDC_P_SUPPORT_FMISC_MEMC             (1)
#define BVDC_P_SUPPORT_DMISC                  (0)
#define BVDC_P_SUPPORT_BVN_444                (0)
#define BVDC_P_SUPPORT_BVN_10BIT_444          (0)
#define BVDC_P_SUPPORT_CMP_TEN_TAP_422_TO_444 (1)
#define BVDC_P_SUPPORT_MAD_SRC_1080I          (0)
#define BVDC_P_SUPPORT_ANR_VER                (0)
#define BVDC_P_SUPPORT_ANR                    (0)
#define BVDC_P_SUPPORT_MAD                    (1)
#define BVDC_P_SUPPORT_ANR_MAD_MEM_SAVING_MODE  (0)
#define BVDC_P_SUPPORT_FREE_CHANNEL           (6)
#define BVDC_P_SUPPORT_DRAIN_F                (3)
#define BVDC_P_SUPPORT_DRAIN_B                (1)
#define BVDC_P_SUPPORT_DRAIN_VER              (0)
#define BVDC_P_SUPPORT_DITHER                 (0)
#define BVDC_P_SUPPORT_VEC_DITHER             (0)
#define BVDC_P_SUPPORT_MCVP                   (0)
#define BVDC_P_SUPPORT_MCVP_VER               (0)
#define BVDC_P_SUPPORT_MCDI_VER               (0) /* MCDI HW version */
#define BVDC_P_SUPPORT_MADR                   (0) /* Number of MAD-R HW */
#define BVDC_P_SUPPORT_MADR_VER               (0) /* MAD-R HW version */
#define BVDC_P_SUPPORT_MANR                   (0) /* Number of ANR HW in MCVP */
#define BVDC_P_SUPPORT_MANR_VER               (0) /* ANR in MCVP version */
#define BVDC_P_SUPPORT_OSCL                   (0)
#define BVDC_P_SUPPORT_VIDEO_TESTFEATURE1_VER (0)

/* CMP */
#define BVDC_P_CMP_0_MAX_VIDEO_WINDOW_COUNT   (2)
#define BVDC_P_CMP_0_MAX_GFX_WINDOW_COUNT     (1)
#define BVDC_P_CMP_1_MAX_VIDEO_WINDOW_COUNT   (2)
#define BVDC_P_CMP_1_MAX_GFX_WINDOW_COUNT     (1)
#define BVDC_P_CMP_2_MAX_VIDEO_WINDOW_COUNT   (1)
#define BVDC_P_CMP_2_MAX_GFX_WINDOW_COUNT     (1)
#define BVDC_P_CMP_3_MAX_VIDEO_WINDOW_COUNT   (0) /* Number of CMP_3_Vx */
#define BVDC_P_CMP_3_MAX_GFX_WINDOW_COUNT     (0) /* Number of CMP_3_Gx */
#define BVDC_P_CMP_4_MAX_VIDEO_WINDOW_COUNT   (0) /* Number of CMP_4_Vx */
#define BVDC_P_CMP_4_MAX_GFX_WINDOW_COUNT     (0) /* Number of CMP_4_Gx */
#define BVDC_P_CMP_5_MAX_VIDEO_WINDOW_COUNT   (0) /* Number of CMP_5_Vx */
#define BVDC_P_CMP_5_MAX_GFX_WINDOW_COUNT     (0) /* Number of CMP_5_Gx */
#define BVDC_P_CMP_6_MAX_VIDEO_WINDOW_COUNT   (0) /* Number of CMP_6_Vx */
#define BVDC_P_CMP_6_MAX_GFX_WINDOW_COUNT     (0) /* Number of CMP_6_Gx */
#define BVDC_P_CMP_0_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_0_NON_LINEAR_CSC_V1        (0)
#define BVDC_P_CMP_1_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_1_NON_LINEAR_CSC_V1        (0)
#define BVDC_P_CMP_2_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_3_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_4_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_5_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_6_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_0_CLEAR_RECT_V0            (16)
#define BVDC_P_CMP_0_CLEAR_RECT_V1            (0)
#define BVDC_P_CMP_1_CLEAR_RECT_V0            (16)
#define BVDC_P_CMP_1_CLEAR_RECT_V1            (0)
#define BVDC_P_CMP_2_CLEAR_RECT_V0            (0)
#define BVDC_P_CMP_3_CLEAR_RECT_V0            (0)
#define BVDC_P_CMP_4_CLEAR_RECT_V0            (0)
#define BVDC_P_CMP_5_CLEAR_RECT_V0            (0)
#define BVDC_P_CMP_6_CLEAR_RECT_V0            (0)
#define BVDC_P_SUPPORT_WIN_CONST_COLOR        (0)
#define BVDC_P_SUPPORT_CM3D                   (0)
#define BVDC_P_SUPPORT_CMP_DERINGING          (0)
#define BVDC_P_SUPPORT_CMP_MOSAIC_CSC         (0)

/* csc */
#define BVDC_P_SUPPORT_CMP_DEMO_MODE          (1)
#define BVDC_P_SUPPORT_CSC_MAT_COEF_VER       (0)
#define BVDC_P_SUPPORT_COLOR_CLIP             (0)

/* display */
#define BVDC_P_SUPPORT_EARLY_TRIGGER          (0)
#define BVDC_P_SUPPORT_VBI_ENC_656            (1)
#define BVDC_P_SUPPORT_NEW_SW_INIT            (0)
#define BVDC_P_SUPPORT_4_DACS_PRIM_VEC        (0)
#define BVDC_P_SUPPORT_VEC_VF_VER             (0)
#define BVDC_P_SUPPORT_AMOL_VBI_ENCODE        (1)
#define BVDC_P_SUPPORT_HDMI_OUT               (1)
#define BVDC_P_SUPPORT_DVI_OUT                (1)
#define BVDC_P_SUPPORT_HDMI_RM_VER            (2)
#define BVDC_P_SUPPORT_ITU656_OUT             (1)
#define BVDC_P_SUPPORT_RFM_OUTPUT             (1)
#define BVDC_P_SUPPORT_EXT_SYNC_OUT_MUX       (0)
#define BVDC_P_SUPPORT_EXT_SYNC_PCL_0         (0)

#define BVDC_P_SUPPORT_1080p_60HZ             (0)
#define BVDC_P_SUPPORT_MUD                    (0)
#define BVDC_P_MAX_DACS                       (7)
#define BVDC_P_VEC_HAS_2_DIFFERENT_DVF        (0)
#define BVDC_P_VEC_SUPPORT_DVI_COLOR_CNVT     (0)
#define BVDC_P_ORTHOGONAL_VEC                 (0)
#define BVDC_P_ORTHOGONAL_VEC_VER             (0)
#define BVDC_P_SUPPORT_IT_VER                 (0)
#define BVDC_P_SUPPORT_CLOCK_GATING           (0)
#define BVDC_P_SUPPORT_3D_VIDEO               (0)
#define BVDC_P_SUPPORT_3D_INDEP_SRC_CLIP      (0)
#define BVDC_P_SUPPORT_STG                    (0)

#elif (BCHP_CHIP==7405)

#if (BCHP_VER >= BCHP_VER_C0)
#define BVDC_P_SUPPORT_DNR_VER                (2)
#define BVDC_P_SUPPORT_TDAC_VER               (4)
#define BVDC_P_SUPPORT_FGT                    (0)  /* Turn off FGT on B0 so 7415 A0 will be binary compatible with 7405 B0 */
#define BVDC_P_SUPPORT_FGT_VER                (0)
#elif (BCHP_VER >= BCHP_VER_B0)
#define BVDC_P_SUPPORT_DNR_VER                (1)
#define BVDC_P_SUPPORT_TDAC_VER               (4)
#define BVDC_P_SUPPORT_FGT                    (0)  /* Turn off FGT on B0 so 7415 A0 will be binary compatible with 7405 B0 */
#define BVDC_P_SUPPORT_FGT_VER                (0)
#else
#define BVDC_P_SUPPORT_DNR_VER                (0)
#define BVDC_P_SUPPORT_TDAC_VER               (3)
#define BVDC_P_SUPPORT_FGT                    (1)
#define BVDC_P_SUPPORT_FGT_VER                (0)
#endif

#define BVDC_P_SUPPORT_656_MASTER_MODE        (1)
#define BVDC_P_SUPPORT_DVO_MASTER_MODE        (1)
#define BVDC_P_SUPPORT_VEC_MUXING             (1)
#define BVDC_P_SUPPORT_LOOP_BACK              (4)
#define BVDC_P_SUPPORT_DNR                    (1)
#define BVDC_P_SUPPORT_MODULAR_VEC            (1)
#define BVDC_P_SUPPORT_NEW_656_IN_VER         (2)
#define BVDC_P_SUPPORT_VEC_GRPD               (0)
#define BVDC_P_SUPPORT_VEC_SECAM              (1)
#define BVDC_P_SUPPORT_CAP_VER                (2)
#define BVDC_P_SUPPORT_CMP_CRC                (1)
#define BVDC_P_SUPPORT_DITHER                 (0)
#define BVDC_P_SUPPORT_VEC_DITHER             (0)

/* source */
#define BVDC_P_SUPPORT_GFD                    (1)
#define BVDC_P_SUPPORT_GFD_VER                (1)
#define BVDC_P_SUPPORT_GFD1_VER               (1)
#define BVDC_P_SUPPORT_MFD                    (2)
#define BVDC_P_SUPPORT_MFD_VER                (4)
#define BVDC_P_SUPPORT_VDEC                   (0)
#define BVDC_P_SUPPORT_VDEC_VER               (0)
#define BVDC_P_SUPPORT_VDEC_PASSTHROUGH       (0)
#define BVDC_P_SUPPORT_HDDVI                  (0)
#define BVDC_P_SUPPORT_HDDVI_VER              (1)
#define BVDC_P_SUPPORT_DOWNSAMPLE             (0)
#define BVDC_P_SUPPORT_VCR_MODE               (0)

/* BVN */
#define BVDC_P_SUPPORT_PEP                    (0)
#define BVDC_P_SUPPORT_PEP_VER                (0)
#define BVDC_P_SUPPORT_TAB                    (0)
#define BVDC_P_SUPPORT_TNT                    (0)
#define BVDC_P_SUPPORT_TNT_VER                (0)
#define BVDC_P_SUPPORT_MASK_DITHER            (0)
#define BVDC_P_SUPPORT_HIST                   (0)
#define BVDC_P_SUPPORT_HIST_VER               (0)
#define BVDC_P_SUPPORT_MAD_VER                (3)
#define BVDC_P_SUPPORT_BOX_DETECT             (1)
#define BVDC_P_SUPPORT_BOX_DETECT_VER         (1)
#define BVDC_P_SUPPORT_CAP                    (4)
#define BVDC_P_SUPPORT_VFD                    (4)
#define BVDC_P_SUPPORT_SCL                    (4)
#define BVDC_P_SUPPORT_SCL_VER                (2)
#define BVDC_P_SUPPORT_HSCL                   (0)
#define BVDC_P_SUPPORT_HSCL_VER               (0)
#define BVDC_P_SUPPORT_HSCL_MAD_HARD_WIRED    (0)
#define BVDC_P_SUPPORT_FMISC_PFRI             (0)
#define BVDC_P_SUPPORT_FMISC_MEMC             (0)
#define BVDC_P_SUPPORT_DMISC                  (1)
#define BVDC_P_SUPPORT_BVN_444                (0)
#define BVDC_P_SUPPORT_BVN_10BIT_444          (0)
#define BVDC_P_SUPPORT_CMP_TEN_TAP_422_TO_444 (1)
#define BVDC_P_SUPPORT_MAD_SRC_1080I          (0)
#define BVDC_P_SUPPORT_ANR_VER                (0)
#define BVDC_P_SUPPORT_ANR                    (0)
#define BVDC_P_SUPPORT_MAD                    (1)
#define BVDC_P_SUPPORT_ANR_MAD_MEM_SAVING_MODE  (0)
#define BVDC_P_SUPPORT_FREE_CHANNEL           (6)
#define BVDC_P_SUPPORT_DRAIN_F                (3)
#define BVDC_P_SUPPORT_DRAIN_B                (1)
#define BVDC_P_SUPPORT_DRAIN_VER              (0)
#define BVDC_P_SUPPORT_MCVP                   (0)
#define BVDC_P_SUPPORT_MCVP_VER               (0)
#define BVDC_P_SUPPORT_MCDI_VER               (0) /* MCDI HW version */
#define BVDC_P_SUPPORT_MADR                   (0) /* Number of MAD-R HW */
#define BVDC_P_SUPPORT_MADR_VER               (0) /* MAD-R HW version */
#define BVDC_P_SUPPORT_MANR                   (0) /* Number of ANR HW in MCVP */
#define BVDC_P_SUPPORT_MANR_VER               (0) /* ANR in MCVP version */
#define BVDC_P_SUPPORT_OSCL                   (0)
#define BVDC_P_SUPPORT_VIDEO_TESTFEATURE1_VER (0)

/* CMP */
#define BVDC_P_CMP_0_MAX_VIDEO_WINDOW_COUNT   (2)
#define BVDC_P_CMP_0_MAX_GFX_WINDOW_COUNT     (1)
#define BVDC_P_CMP_1_MAX_VIDEO_WINDOW_COUNT   (2)
#define BVDC_P_CMP_1_MAX_GFX_WINDOW_COUNT     (1)
#define BVDC_P_CMP_2_MAX_VIDEO_WINDOW_COUNT   (0)
#define BVDC_P_CMP_2_MAX_GFX_WINDOW_COUNT     (0)
#define BVDC_P_CMP_3_MAX_VIDEO_WINDOW_COUNT   (0) /* Number of CMP_3_Vx */
#define BVDC_P_CMP_3_MAX_GFX_WINDOW_COUNT     (0) /* Number of CMP_3_Gx */
#define BVDC_P_CMP_4_MAX_VIDEO_WINDOW_COUNT   (0) /* Number of CMP_4_Vx */
#define BVDC_P_CMP_4_MAX_GFX_WINDOW_COUNT     (0) /* Number of CMP_4_Gx */
#define BVDC_P_CMP_5_MAX_VIDEO_WINDOW_COUNT   (0) /* Number of CMP_5_Vx */
#define BVDC_P_CMP_5_MAX_GFX_WINDOW_COUNT     (0) /* Number of CMP_5_Gx */
#define BVDC_P_CMP_6_MAX_VIDEO_WINDOW_COUNT   (0) /* Number of CMP_6_Vx */
#define BVDC_P_CMP_6_MAX_GFX_WINDOW_COUNT     (0) /* Number of CMP_6_Gx */
#define BVDC_P_CMP_0_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_0_NON_LINEAR_CSC_V1        (0)
#define BVDC_P_CMP_1_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_1_NON_LINEAR_CSC_V1        (0)
#define BVDC_P_CMP_2_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_3_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_4_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_5_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_6_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_0_CLEAR_RECT_V0            (16)
#define BVDC_P_CMP_0_CLEAR_RECT_V1            (0)
#define BVDC_P_CMP_1_CLEAR_RECT_V0            (16)
#define BVDC_P_CMP_1_CLEAR_RECT_V1            (0)
#define BVDC_P_CMP_2_CLEAR_RECT_V0            (0)
#define BVDC_P_CMP_3_CLEAR_RECT_V0            (0)
#define BVDC_P_CMP_4_CLEAR_RECT_V0            (0)
#define BVDC_P_CMP_5_CLEAR_RECT_V0            (0)
#define BVDC_P_CMP_6_CLEAR_RECT_V0            (0)
#define BVDC_P_SUPPORT_WIN_CONST_COLOR        (0)
#define BVDC_P_SUPPORT_CM3D                   (0)
#define BVDC_P_SUPPORT_CMP_DERINGING          (0)
#define BVDC_P_SUPPORT_CMP_MOSAIC_CSC         (0)

/* csc */
#define BVDC_P_SUPPORT_CMP_DEMO_MODE          (1)
#define BVDC_P_SUPPORT_CSC_MAT_COEF_VER       (0)
#define BVDC_P_SUPPORT_COLOR_CLIP             (0)

/* display */
#define BVDC_P_SUPPORT_EARLY_TRIGGER          (0)
#define BVDC_P_SUPPORT_VBI_ENC_656            (1)
#define BVDC_P_SUPPORT_NEW_SW_INIT            (0)
#define BVDC_P_SUPPORT_4_DACS_PRIM_VEC        (0)
#define BVDC_P_SUPPORT_VEC_VF_VER             (0)
#define BVDC_P_SUPPORT_AMOL_VBI_ENCODE        (1)
#define BVDC_P_SUPPORT_HDMI_OUT               (1)
#define BVDC_P_SUPPORT_DVI_OUT                (1)
#define BVDC_P_SUPPORT_HDMI_RM_VER            (2)
#define BVDC_P_SUPPORT_ITU656_OUT             (1)
#define BVDC_P_SUPPORT_RFM_OUTPUT             (1)
#define BVDC_P_SUPPORT_EXT_SYNC_OUT_MUX       (0)
#define BVDC_P_SUPPORT_EXT_SYNC_PCL_0         (0)
#define BVDC_P_SUPPORT_QDAC_VER               (0)
#define BVDC_P_SUPPORT_1080p_60HZ             (0)
#define BVDC_P_SUPPORT_MUD                    (0)
#define BVDC_P_MAX_DACS                       (6)
#define BVDC_P_VEC_HAS_2_DIFFERENT_DVF        (0)
#define BVDC_P_VEC_SUPPORT_DVI_COLOR_CNVT     (0)
#define BVDC_P_ORTHOGONAL_VEC                 (0)
#define BVDC_P_ORTHOGONAL_VEC_VER             (0)
#define BVDC_P_SUPPORT_IT_VER                 (0)
#define BVDC_P_SUPPORT_CLOCK_GATING           (0)
#define BVDC_P_SUPPORT_3D_VIDEO               (0)
#define BVDC_P_SUPPORT_3D_INDEP_SRC_CLIP      (0)
#define BVDC_P_SUPPORT_STG                    (0)

#elif (BCHP_CHIP==7325)

#if (BCHP_VER >= BCHP_VER_B0)
#define BVDC_P_SUPPORT_DNR_VER                (2)
#define BVDC_P_SUPPORT_FGT_VER                (1)
#else
#define BVDC_P_SUPPORT_DNR_VER                (0)
#define BVDC_P_SUPPORT_FGT_VER                (0)
#endif

#define BVDC_P_SUPPORT_656_MASTER_MODE        (0)
#define BVDC_P_SUPPORT_DVO_MASTER_MODE        (0)
#define BVDC_P_SUPPORT_VEC_MUXING             (1)

#define BVDC_P_SUPPORT_LOOP_BACK              (3)
#define BVDC_P_SUPPORT_DNR                    (1)
#define BVDC_P_SUPPORT_MODULAR_VEC            (2)
#define BVDC_P_SUPPORT_NEW_656_IN_VER         (0)
#define BVDC_P_SUPPORT_VEC_GRPD               (0)
#define BVDC_P_SUPPORT_VEC_SECAM              (1)
#define BVDC_P_SUPPORT_CAP_VER                (2)
#define BVDC_P_SUPPORT_CMP_CRC                (1)

/* source */
#define BVDC_P_SUPPORT_GFD                    (1)
#define BVDC_P_SUPPORT_GFD_VER                (1)
#define BVDC_P_SUPPORT_GFD1_VER               (1)
#define BVDC_P_SUPPORT_MFD                    (2)
#define BVDC_P_SUPPORT_MFD_VER                (5)
#define BVDC_P_SUPPORT_VDEC                   (0)
#define BVDC_P_SUPPORT_VDEC_VER               (0)
#define BVDC_P_SUPPORT_VDEC_PASSTHROUGH       (0)
#define BVDC_P_SUPPORT_HDDVI                  (0)
#define BVDC_P_SUPPORT_HDDVI_VER              (1)
#define BVDC_P_SUPPORT_DOWNSAMPLE             (0)
#define BVDC_P_SUPPORT_VCR_MODE               (0)

/* BVN */
#define BVDC_P_SUPPORT_PEP                    (1)
#define BVDC_P_SUPPORT_PEP_VER                (1)
#define BVDC_P_SUPPORT_TAB                    (1)
#define BVDC_P_SUPPORT_TNT                    (0)
#define BVDC_P_SUPPORT_TNT_VER                (0)
#define BVDC_P_SUPPORT_MASK_DITHER            (2)
#define BVDC_P_SUPPORT_HIST                   (1)
#define BVDC_P_SUPPORT_HIST_VER               (1)
#define BVDC_P_SUPPORT_MAD_VER                (5)
#define BVDC_P_SUPPORT_BOX_DETECT             (1)
#define BVDC_P_SUPPORT_BOX_DETECT_VER         (1)
#define BVDC_P_SUPPORT_CAP                    (4)
#define BVDC_P_SUPPORT_VFD                    (4)
#define BVDC_P_SUPPORT_SCL                    (4)
#define BVDC_P_SUPPORT_SCL_VER                (2)
#define BVDC_P_SUPPORT_HSCL                   (0)
#define BVDC_P_SUPPORT_HSCL_VER               (0)
#define BVDC_P_SUPPORT_HSCL_MAD_HARD_WIRED    (0)
#define BVDC_P_SUPPORT_FMISC_PFRI             (0)
#define BVDC_P_SUPPORT_FMISC_MEMC             (0)
#define BVDC_P_SUPPORT_DMISC                  (1)
#define BVDC_P_SUPPORT_BVN_444                (0)
#define BVDC_P_SUPPORT_BVN_10BIT_444          (0)
#define BVDC_P_SUPPORT_CMP_TEN_TAP_422_TO_444 (1)
#define BVDC_P_SUPPORT_MAD_SRC_1080I          (0)
#define BVDC_P_SUPPORT_ANR_VER                (0)
#define BVDC_P_SUPPORT_ANR                    (0)
#define BVDC_P_SUPPORT_MAD                    (1)
#define BVDC_P_SUPPORT_ANR_MAD_MEM_SAVING_MODE  (0)
#define BVDC_P_SUPPORT_FREE_CHANNEL           (3)
#define BVDC_P_SUPPORT_DRAIN_F                (1)
#define BVDC_P_SUPPORT_DRAIN_B                (1)
#define BVDC_P_SUPPORT_DRAIN_VER              (0)
#define BVDC_P_SUPPORT_FGT                    (1)
#define BVDC_P_SUPPORT_DITHER                 (0)
#define BVDC_P_SUPPORT_VEC_DITHER             (0)
#define BVDC_P_SUPPORT_MCVP                   (0)
#define BVDC_P_SUPPORT_MCVP_VER               (0)
#define BVDC_P_SUPPORT_MCDI_VER               (0) /* MCDI HW version */
#define BVDC_P_SUPPORT_MADR                   (0) /* Number of MAD-R HW */
#define BVDC_P_SUPPORT_MADR_VER               (0) /* MAD-R HW version */
#define BVDC_P_SUPPORT_MANR                   (0) /* Number of ANR HW in MCVP */
#define BVDC_P_SUPPORT_MANR_VER               (0) /* ANR in MCVP version */
#define BVDC_P_SUPPORT_OSCL                   (0)
#define BVDC_P_SUPPORT_VIDEO_TESTFEATURE1_VER (0)

/* CMP */
#define BVDC_P_CMP_0_MAX_VIDEO_WINDOW_COUNT   (1)
#define BVDC_P_CMP_0_MAX_GFX_WINDOW_COUNT     (1)
#define BVDC_P_CMP_1_MAX_VIDEO_WINDOW_COUNT   (1)
#define BVDC_P_CMP_1_MAX_GFX_WINDOW_COUNT     (1)
#define BVDC_P_CMP_2_MAX_VIDEO_WINDOW_COUNT   (0)
#define BVDC_P_CMP_2_MAX_GFX_WINDOW_COUNT     (0)
#define BVDC_P_CMP_3_MAX_VIDEO_WINDOW_COUNT   (0) /* Number of CMP_3_Vx */
#define BVDC_P_CMP_3_MAX_GFX_WINDOW_COUNT     (0) /* Number of CMP_3_Gx */
#define BVDC_P_CMP_4_MAX_VIDEO_WINDOW_COUNT   (0) /* Number of CMP_4_Vx */
#define BVDC_P_CMP_4_MAX_GFX_WINDOW_COUNT     (0) /* Number of CMP_4_Gx */
#define BVDC_P_CMP_5_MAX_VIDEO_WINDOW_COUNT   (0) /* Number of CMP_5_Vx */
#define BVDC_P_CMP_5_MAX_GFX_WINDOW_COUNT     (0) /* Number of CMP_5_Gx */
#define BVDC_P_CMP_6_MAX_VIDEO_WINDOW_COUNT   (0) /* Number of CMP_6_Vx */
#define BVDC_P_CMP_6_MAX_GFX_WINDOW_COUNT     (0) /* Number of CMP_6_Gx */
#define BVDC_P_CMP_0_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_0_NON_LINEAR_CSC_V1        (0)
#define BVDC_P_CMP_1_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_1_NON_LINEAR_CSC_V1        (0)
#define BVDC_P_CMP_2_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_3_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_4_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_5_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_6_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_0_CLEAR_RECT_V0            (16)
#define BVDC_P_CMP_0_CLEAR_RECT_V1            (0)
#define BVDC_P_CMP_1_CLEAR_RECT_V0            (16)
#define BVDC_P_CMP_1_CLEAR_RECT_V1            (0)
#define BVDC_P_CMP_2_CLEAR_RECT_V0            (0)
#define BVDC_P_CMP_3_CLEAR_RECT_V0            (0)
#define BVDC_P_CMP_4_CLEAR_RECT_V0            (0)
#define BVDC_P_CMP_5_CLEAR_RECT_V0            (0)
#define BVDC_P_CMP_6_CLEAR_RECT_V0            (0)
#define BVDC_P_SUPPORT_WIN_CONST_COLOR        (0)
#define BVDC_P_SUPPORT_CM3D                   (0)
#define BVDC_P_SUPPORT_CMP_DERINGING          (0)
#define BVDC_P_SUPPORT_CMP_MOSAIC_CSC         (0)

/* csc */
#define BVDC_P_SUPPORT_CMP_DEMO_MODE          (1)
#define BVDC_P_SUPPORT_CSC_MAT_COEF_VER       (1)
#define BVDC_P_SUPPORT_COLOR_CLIP             (0)

/* display */
#define BVDC_P_SUPPORT_EARLY_TRIGGER          (0)
#define BVDC_P_SUPPORT_VBI_ENC_656            (1)
#define BVDC_P_SUPPORT_NEW_SW_INIT            (0)
#define BVDC_P_SUPPORT_4_DACS_PRIM_VEC        (0)
#define BVDC_P_SUPPORT_VEC_VF_VER             (1)
#define BVDC_P_SUPPORT_AMOL_VBI_ENCODE        (1)
#define BVDC_P_SUPPORT_HDMI_OUT               (1)
#define BVDC_P_SUPPORT_DVI_OUT                (1)
#define BVDC_P_SUPPORT_HDMI_RM_VER            (3)
#define BVDC_P_SUPPORT_ITU656_OUT             (1)
#define BVDC_P_SUPPORT_RFM_OUTPUT             (1)
#define BVDC_P_SUPPORT_EXT_SYNC_OUT_MUX       (0)
#define BVDC_P_SUPPORT_EXT_SYNC_PCL_0         (0)
#define BVDC_P_SUPPORT_TDAC_VER               (3)
#define BVDC_P_SUPPORT_QDAC_VER               (0)
#define BVDC_P_SUPPORT_1080p_60HZ             (0)
#define BVDC_P_SUPPORT_MUD                    (0)
#define BVDC_P_MAX_DACS                       (6)
#define BVDC_P_VEC_HAS_2_DIFFERENT_DVF        (0)
#define BVDC_P_VEC_SUPPORT_DVI_COLOR_CNVT     (0)
#define BVDC_P_ORTHOGONAL_VEC                 (0)
#define BVDC_P_ORTHOGONAL_VEC_VER             (0)
#define BVDC_P_SUPPORT_IT_VER                 (0)
#define BVDC_P_SUPPORT_CLOCK_GATING           (0)
#define BVDC_P_SUPPORT_3D_VIDEO               (0)
#define BVDC_P_SUPPORT_3D_INDEP_SRC_CLIP      (0)
#define BVDC_P_SUPPORT_STG                    (0)

#elif (BCHP_CHIP==7335)

#if (BCHP_VER >= BCHP_VER_B0)
#define BVDC_P_SUPPORT_DNR_VER                (1)
#define BVDC_P_SUPPORT_VEC_DITHER             (1)
#define BVDC_P_SUPPORT_FGT_VER                (1)
#else
#define BVDC_P_SUPPORT_DNR_VER                (0)
#define BVDC_P_SUPPORT_VEC_DITHER             (0)
#define BVDC_P_SUPPORT_FGT_VER                (0)
#endif

#define BVDC_P_SUPPORT_656_MASTER_MODE        (1)
#define BVDC_P_SUPPORT_DVO_MASTER_MODE        (1)
#define BVDC_P_SUPPORT_VEC_MUXING             (1)
#define BVDC_P_SUPPORT_LOOP_BACK              (4)
#define BVDC_P_SUPPORT_DNR                    (1)
#define BVDC_P_SUPPORT_MODULAR_VEC            (2)
#define BVDC_P_SUPPORT_NEW_656_IN_VER         (0)
#define BVDC_P_SUPPORT_VEC_GRPD               (0)
#define BVDC_P_SUPPORT_VEC_SECAM              (1)
#define BVDC_P_SUPPORT_CAP_VER                (2)
#define BVDC_P_SUPPORT_CMP_CRC                (1)
#define BVDC_P_SUPPORT_CMP_LUMA_AVG_VER       (0)

/* source */
#define BVDC_P_SUPPORT_GFD                    (1)
#define BVDC_P_SUPPORT_GFD_VER                (1)
#define BVDC_P_SUPPORT_GFD1_VER               (1)
#define BVDC_P_SUPPORT_MFD                    (2)
#define BVDC_P_SUPPORT_MFD_VER                (5)
#define BVDC_P_SUPPORT_VDEC                   (0)
#define BVDC_P_SUPPORT_VDEC_VER               (0)
#define BVDC_P_SUPPORT_VDEC_PASSTHROUGH       (0)
#define BVDC_P_SUPPORT_HDDVI                  (0)
#define BVDC_P_SUPPORT_HDDVI_VER              (1)
#define BVDC_P_SUPPORT_DOWNSAMPLE             (0)
#define BVDC_P_SUPPORT_VCR_MODE               (0)

/* BVN */
#define BVDC_P_SUPPORT_PEP                    (1)
#define BVDC_P_SUPPORT_PEP_VER                (1)
#define BVDC_P_SUPPORT_TAB                    (1)
#define BVDC_P_SUPPORT_TNT                    (0)
#define BVDC_P_SUPPORT_TNT_VER                (0)
#define BVDC_P_SUPPORT_MASK_DITHER            (2)
#define BVDC_P_SUPPORT_HIST                   (1)
#define BVDC_P_SUPPORT_HIST_VER               (1)
#define BVDC_P_SUPPORT_MAD_VER                (5)
#define BVDC_P_SUPPORT_BOX_DETECT             (1)
#define BVDC_P_SUPPORT_BOX_DETECT_VER         (1)
#define BVDC_P_SUPPORT_CAP                    (4)
#define BVDC_P_SUPPORT_VFD                    (4)
#define BVDC_P_SUPPORT_SCL                    (4)
#define BVDC_P_SUPPORT_SCL_VER                (2)
#define BVDC_P_SUPPORT_HSCL                   (0)
#define BVDC_P_SUPPORT_HSCL_VER               (0)
#define BVDC_P_SUPPORT_HSCL_MAD_HARD_WIRED    (0)
#define BVDC_P_SUPPORT_FMISC_PFRI             (0)
#define BVDC_P_SUPPORT_FMISC_MEMC             (0)
#define BVDC_P_SUPPORT_DMISC                  (1)
#define BVDC_P_SUPPORT_BVN_444                (0)
#define BVDC_P_SUPPORT_BVN_10BIT_444          (0)
#define BVDC_P_SUPPORT_CMP_TEN_TAP_422_TO_444 (1)
#define BVDC_P_SUPPORT_MAD_SRC_1080I          (0)
#define BVDC_P_SUPPORT_ANR_VER                (0)
#define BVDC_P_SUPPORT_ANR                    (0)
#define BVDC_P_SUPPORT_MAD                    (1)
#define BVDC_P_SUPPORT_ANR_MAD_MEM_SAVING_MODE  (0)
#define BVDC_P_SUPPORT_FREE_CHANNEL           (6)
#define BVDC_P_SUPPORT_DRAIN_F                (3)
#define BVDC_P_SUPPORT_DRAIN_B                (1)
#define BVDC_P_SUPPORT_DRAIN_VER              (0)
#define BVDC_P_SUPPORT_FGT                    (1)
#define BVDC_P_SUPPORT_DITHER                 (0)
#define BVDC_P_SUPPORT_MCVP                   (0)
#define BVDC_P_SUPPORT_MCVP_VER               (0)
#define BVDC_P_SUPPORT_MCDI_VER               (0) /* MCDI HW version */
#define BVDC_P_SUPPORT_MADR                   (0) /* Number of MAD-R HW */
#define BVDC_P_SUPPORT_MADR_VER               (0) /* MAD-R HW version */
#define BVDC_P_SUPPORT_MANR                   (0) /* Number of ANR HW in MCVP */
#define BVDC_P_SUPPORT_MANR_VER               (0) /* ANR in MCVP version */
#define BVDC_P_SUPPORT_OSCL                   (0)
#define BVDC_P_SUPPORT_VIDEO_TESTFEATURE1_VER (0)

/* CMP */
#define BVDC_P_CMP_0_MAX_VIDEO_WINDOW_COUNT   (1)
#define BVDC_P_CMP_0_MAX_GFX_WINDOW_COUNT     (1)
#define BVDC_P_CMP_1_MAX_VIDEO_WINDOW_COUNT   (1)
#define BVDC_P_CMP_1_MAX_GFX_WINDOW_COUNT     (1)
#define BVDC_P_CMP_2_MAX_VIDEO_WINDOW_COUNT   (0)
#define BVDC_P_CMP_2_MAX_GFX_WINDOW_COUNT     (0)
#define BVDC_P_CMP_3_MAX_VIDEO_WINDOW_COUNT   (0) /* Number of CMP_3_Vx */
#define BVDC_P_CMP_3_MAX_GFX_WINDOW_COUNT     (0) /* Number of CMP_3_Gx */
#define BVDC_P_CMP_4_MAX_VIDEO_WINDOW_COUNT   (0) /* Number of CMP_4_Vx */
#define BVDC_P_CMP_4_MAX_GFX_WINDOW_COUNT     (0) /* Number of CMP_4_Gx */
#define BVDC_P_CMP_5_MAX_VIDEO_WINDOW_COUNT   (0) /* Number of CMP_5_Vx */
#define BVDC_P_CMP_5_MAX_GFX_WINDOW_COUNT     (0) /* Number of CMP_5_Gx */
#define BVDC_P_CMP_6_MAX_VIDEO_WINDOW_COUNT   (0) /* Number of CMP_6_Vx */
#define BVDC_P_CMP_6_MAX_GFX_WINDOW_COUNT     (0) /* Number of CMP_6_Gx */
#define BVDC_P_CMP_0_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_0_NON_LINEAR_CSC_V1        (0)
#define BVDC_P_CMP_1_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_1_NON_LINEAR_CSC_V1        (0)
#define BVDC_P_CMP_2_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_3_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_4_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_5_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_6_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_0_CLEAR_RECT_V0            (16)
#define BVDC_P_CMP_0_CLEAR_RECT_V1            (0)
#define BVDC_P_CMP_1_CLEAR_RECT_V0            (16)
#define BVDC_P_CMP_1_CLEAR_RECT_V1            (0)
#define BVDC_P_CMP_2_CLEAR_RECT_V0            (0)
#define BVDC_P_CMP_3_CLEAR_RECT_V0            (0)
#define BVDC_P_CMP_4_CLEAR_RECT_V0            (0)
#define BVDC_P_CMP_5_CLEAR_RECT_V0            (0)
#define BVDC_P_CMP_6_CLEAR_RECT_V0            (0)
#define BVDC_P_SUPPORT_WIN_CONST_COLOR        (0)
#define BVDC_P_SUPPORT_CM3D                   (0)
#define BVDC_P_SUPPORT_CMP_DERINGING          (0)
#define BVDC_P_SUPPORT_CMP_MOSAIC_CSC         (0)

/* csc */
#define BVDC_P_SUPPORT_CMP_DEMO_MODE          (1)
#define BVDC_P_SUPPORT_CSC_MAT_COEF_VER       (1)
#define BVDC_P_SUPPORT_COLOR_CLIP             (0)

/* display */
#define BVDC_P_SUPPORT_EARLY_TRIGGER          (0)
#define BVDC_P_SUPPORT_VBI_ENC_656            (1)
#define BVDC_P_SUPPORT_NEW_SW_INIT            (0)
#define BVDC_P_SUPPORT_4_DACS_PRIM_VEC        (0)
#define BVDC_P_SUPPORT_VEC_VF_VER             (1)
#define BVDC_P_SUPPORT_AMOL_VBI_ENCODE        (1)
#define BVDC_P_SUPPORT_HDMI_OUT               (1)
#define BVDC_P_SUPPORT_DVI_OUT                (1)
#define BVDC_P_SUPPORT_HDMI_RM_VER            (2)
#define BVDC_P_SUPPORT_ITU656_OUT             (1)
#define BVDC_P_SUPPORT_RFM_OUTPUT             (1)
#define BVDC_P_SUPPORT_EXT_SYNC_OUT_MUX       (0)
#define BVDC_P_SUPPORT_EXT_SYNC_PCL_0         (0)
#define BVDC_P_SUPPORT_TDAC_VER               (4)
#define BVDC_P_SUPPORT_QDAC_VER               (0)
#define BVDC_P_SUPPORT_1080p_60HZ             (0)
#define BVDC_P_SUPPORT_MUD                    (0)
#define BVDC_P_MAX_DACS                       (6)
#define BVDC_P_VEC_HAS_2_DIFFERENT_DVF        (0)
#define BVDC_P_VEC_SUPPORT_DVI_COLOR_CNVT     (0)
#define BVDC_P_ORTHOGONAL_VEC                 (0)
#define BVDC_P_ORTHOGONAL_VEC_VER             (0)
#define BVDC_P_SUPPORT_IT_VER                 (0)
#define BVDC_P_SUPPORT_CLOCK_GATING           (0)
#define BVDC_P_SUPPORT_3D_VIDEO               (0)
#define BVDC_P_SUPPORT_3D_INDEP_SRC_CLIP      (0)
#define BVDC_P_SUPPORT_STG                    (0)

#elif (BCHP_CHIP==7336)

#define BVDC_P_SUPPORT_VEC_DITHER             (1)
#define BVDC_P_SUPPORT_FGT_VER                (0)
#define BVDC_P_SUPPORT_656_MASTER_MODE        (1)
#define BVDC_P_SUPPORT_DVO_MASTER_MODE        (1)
#define BVDC_P_SUPPORT_VEC_MUXING             (1)
#define BVDC_P_SUPPORT_LOOP_BACK              (4)
#define BVDC_P_SUPPORT_DNR                    (1)
#define BVDC_P_SUPPORT_DNR_VER                (1)
#define BVDC_P_SUPPORT_MODULAR_VEC            (2)
#define BVDC_P_SUPPORT_NEW_656_IN_VER         (0)
#define BVDC_P_SUPPORT_VEC_GRPD               (0)
#define BVDC_P_SUPPORT_VEC_SECAM              (1)
#define BVDC_P_SUPPORT_CAP_VER                (2)
#define BVDC_P_SUPPORT_CMP_CRC                (1)
#define BVDC_P_SUPPORT_CMP_LUMA_AVG_VER       (0)

/* source */
#define BVDC_P_SUPPORT_GFD                    (1)
#define BVDC_P_SUPPORT_GFD_VER                (1)
#define BVDC_P_SUPPORT_GFD1_VER               (1)
#define BVDC_P_SUPPORT_MFD                    (2)
#define BVDC_P_SUPPORT_MFD_VER                (5)
#define BVDC_P_SUPPORT_VDEC                   (0)
#define BVDC_P_SUPPORT_VDEC_VER               (0)
#define BVDC_P_SUPPORT_VDEC_PASSTHROUGH       (0)
#define BVDC_P_SUPPORT_HDDVI                  (0)
#define BVDC_P_SUPPORT_HDDVI_VER              (1)
#define BVDC_P_SUPPORT_DOWNSAMPLE             (0)
#define BVDC_P_SUPPORT_VCR_MODE               (0)

/* BVN */
#define BVDC_P_SUPPORT_PEP                    (1)
#define BVDC_P_SUPPORT_PEP_VER                (1)
#define BVDC_P_SUPPORT_TAB                    (1)
#define BVDC_P_SUPPORT_TNT                    (0)
#define BVDC_P_SUPPORT_TNT_VER                (0)
#define BVDC_P_SUPPORT_MASK_DITHER            (2)
#define BVDC_P_SUPPORT_HIST                   (1)
#define BVDC_P_SUPPORT_HIST_VER               (1)
#define BVDC_P_SUPPORT_MAD_VER                (5)
#define BVDC_P_SUPPORT_BOX_DETECT             (1)
#define BVDC_P_SUPPORT_BOX_DETECT_VER         (1)
#define BVDC_P_SUPPORT_CAP                    (4)
#define BVDC_P_SUPPORT_VFD                    (4)
#define BVDC_P_SUPPORT_SCL                    (4)
#define BVDC_P_SUPPORT_SCL_VER                (2)
#define BVDC_P_SUPPORT_HSCL                   (0)
#define BVDC_P_SUPPORT_HSCL_VER               (0)
#define BVDC_P_SUPPORT_HSCL_MAD_HARD_WIRED    (0)
#define BVDC_P_SUPPORT_FMISC_PFRI             (0)
#define BVDC_P_SUPPORT_FMISC_MEMC             (0)
#define BVDC_P_SUPPORT_DMISC                  (1)
#define BVDC_P_SUPPORT_BVN_444                (0)
#define BVDC_P_SUPPORT_BVN_10BIT_444          (0)
#define BVDC_P_SUPPORT_CMP_TEN_TAP_422_TO_444 (1)
#define BVDC_P_SUPPORT_MAD_SRC_1080I          (0)
#define BVDC_P_SUPPORT_ANR_VER                (0)
#define BVDC_P_SUPPORT_ANR                    (0)
#define BVDC_P_SUPPORT_MAD                    (1)
#define BVDC_P_SUPPORT_ANR_MAD_MEM_SAVING_MODE  (0)
#define BVDC_P_SUPPORT_FREE_CHANNEL           (6)
#define BVDC_P_SUPPORT_DRAIN_F                (3)
#define BVDC_P_SUPPORT_DRAIN_B                (1)
#define BVDC_P_SUPPORT_DRAIN_VER              (0)
#define BVDC_P_SUPPORT_FGT                    (0)
#define BVDC_P_SUPPORT_DITHER                 (0)
#define BVDC_P_SUPPORT_MCVP                   (0)
#define BVDC_P_SUPPORT_MCVP_VER               (0)
#define BVDC_P_SUPPORT_MCDI_VER               (0) /* MCDI HW version */
#define BVDC_P_SUPPORT_MADR                   (0) /* Number of MAD-R HW */
#define BVDC_P_SUPPORT_MADR_VER               (0) /* MAD-R HW version */
#define BVDC_P_SUPPORT_MANR                   (0) /* Number of ANR HW in MCVP */
#define BVDC_P_SUPPORT_MANR_VER               (0) /* ANR in MCVP version */
#define BVDC_P_SUPPORT_OSCL                   (0)
#define BVDC_P_SUPPORT_VIDEO_TESTFEATURE1_VER (0)

/* CMP */
#define BVDC_P_CMP_0_MAX_VIDEO_WINDOW_COUNT   (1)
#define BVDC_P_CMP_0_MAX_GFX_WINDOW_COUNT     (1)
#define BVDC_P_CMP_1_MAX_VIDEO_WINDOW_COUNT   (1)
#define BVDC_P_CMP_1_MAX_GFX_WINDOW_COUNT     (1)
#define BVDC_P_CMP_2_MAX_VIDEO_WINDOW_COUNT   (0)
#define BVDC_P_CMP_2_MAX_GFX_WINDOW_COUNT     (0)
#define BVDC_P_CMP_3_MAX_VIDEO_WINDOW_COUNT   (0) /* Number of CMP_3_Vx */
#define BVDC_P_CMP_3_MAX_GFX_WINDOW_COUNT     (0) /* Number of CMP_3_Gx */
#define BVDC_P_CMP_4_MAX_VIDEO_WINDOW_COUNT   (0) /* Number of CMP_4_Vx */
#define BVDC_P_CMP_4_MAX_GFX_WINDOW_COUNT     (0) /* Number of CMP_4_Gx */
#define BVDC_P_CMP_5_MAX_VIDEO_WINDOW_COUNT   (0) /* Number of CMP_5_Vx */
#define BVDC_P_CMP_5_MAX_GFX_WINDOW_COUNT     (0) /* Number of CMP_5_Gx */
#define BVDC_P_CMP_6_MAX_VIDEO_WINDOW_COUNT   (0) /* Number of CMP_6_Vx */
#define BVDC_P_CMP_6_MAX_GFX_WINDOW_COUNT     (0) /* Number of CMP_6_Gx */
#define BVDC_P_CMP_0_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_0_NON_LINEAR_CSC_V1        (0)
#define BVDC_P_CMP_1_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_1_NON_LINEAR_CSC_V1        (0)
#define BVDC_P_CMP_2_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_3_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_4_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_5_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_6_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_0_CLEAR_RECT_V0            (16)
#define BVDC_P_CMP_0_CLEAR_RECT_V1            (0)
#define BVDC_P_CMP_1_CLEAR_RECT_V0            (16)
#define BVDC_P_CMP_1_CLEAR_RECT_V1            (0)
#define BVDC_P_CMP_2_CLEAR_RECT_V0            (0)
#define BVDC_P_CMP_3_CLEAR_RECT_V0            (0)
#define BVDC_P_CMP_4_CLEAR_RECT_V0            (0)
#define BVDC_P_CMP_5_CLEAR_RECT_V0            (0)
#define BVDC_P_CMP_6_CLEAR_RECT_V0            (0)
#define BVDC_P_SUPPORT_WIN_CONST_COLOR        (0)
#define BVDC_P_SUPPORT_CM3D                   (0)
#define BVDC_P_SUPPORT_CMP_DERINGING          (0)
#define BVDC_P_SUPPORT_CMP_MOSAIC_CSC         (0)

/* csc */
#define BVDC_P_SUPPORT_CMP_DEMO_MODE          (1)
#define BVDC_P_SUPPORT_CSC_MAT_COEF_VER       (1)
#define BVDC_P_SUPPORT_COLOR_CLIP             (0)

/* display */
#define BVDC_P_SUPPORT_EARLY_TRIGGER          (0)
#define BVDC_P_SUPPORT_VBI_ENC_656            (1)
#define BVDC_P_SUPPORT_NEW_SW_INIT            (0)
#define BVDC_P_SUPPORT_4_DACS_PRIM_VEC        (0)
#define BVDC_P_SUPPORT_VEC_VF_VER             (1)
#define BVDC_P_SUPPORT_AMOL_VBI_ENCODE        (1)
#define BVDC_P_SUPPORT_HDMI_OUT               (1)
#define BVDC_P_SUPPORT_DVI_OUT                (1)
#define BVDC_P_SUPPORT_HDMI_RM_VER            (2)
#define BVDC_P_SUPPORT_ITU656_OUT             (1)
#define BVDC_P_SUPPORT_RFM_OUTPUT             (1)
#define BVDC_P_SUPPORT_EXT_SYNC_OUT_MUX       (0)
#define BVDC_P_SUPPORT_EXT_SYNC_PCL_0         (0)
#define BVDC_P_SUPPORT_TDAC_VER               (4)
#define BVDC_P_SUPPORT_QDAC_VER               (0)
#define BVDC_P_SUPPORT_1080p_60HZ             (0)
#define BVDC_P_SUPPORT_MUD                    (0)
#define BVDC_P_MAX_DACS                       (6)
#define BVDC_P_VEC_HAS_2_DIFFERENT_DVF        (0)
#define BVDC_P_VEC_SUPPORT_DVI_COLOR_CNVT     (0)
#define BVDC_P_ORTHOGONAL_VEC                 (0)
#define BVDC_P_ORTHOGONAL_VEC_VER             (0)
#define BVDC_P_SUPPORT_CLOCK_GATING           (0)
#define BVDC_P_SUPPORT_IT_VER                 (0)
#define BVDC_P_SUPPORT_3D_VIDEO               (0)
#define BVDC_P_SUPPORT_3D_INDEP_SRC_CLIP      (0)
#define BVDC_P_SUPPORT_STG                    (0)

#elif (BCHP_CHIP==7340)

#if (BCHP_VER >= BCHP_VER_B0)
#define BVDC_P_SUPPORT_CLOCK_GATING           (1)
#else
#define BVDC_P_SUPPORT_CLOCK_GATING           (0)
#endif

#define BVDC_P_SUPPORT_FGT                    (0)
#define BVDC_P_SUPPORT_FGT_VER                (0)

#define BVDC_P_SUPPORT_656_MASTER_MODE        (0)
#define BVDC_P_SUPPORT_DVO_MASTER_MODE        (0)
#define BVDC_P_SUPPORT_VEC_MUXING             (1)
#define BVDC_P_SUPPORT_LOOP_BACK              (2)
#define BVDC_P_SUPPORT_DNR                    (1)
#define BVDC_P_SUPPORT_DNR_VER                (3)
#define BVDC_P_SUPPORT_MODULAR_VEC            (2)
#define BVDC_P_SUPPORT_NEW_656_IN_VER         (0)
#define BVDC_P_SUPPORT_VEC_GRPD               (0)
#define BVDC_P_SUPPORT_VEC_SECAM              (1)
#define BVDC_P_SUPPORT_CAP_VER                (2)
#define BVDC_P_SUPPORT_CMP_CRC                (1)
#define BVDC_P_SUPPORT_CMP_LUMA_AVG_VER       (0)

/* source */
#define BVDC_P_SUPPORT_GFD                    (2)
#define BVDC_P_SUPPORT_GFD_VER                (3)
#define BVDC_P_SUPPORT_GFD1_VER               (3)
#define BVDC_P_SUPPORT_MFD                    (1)
#define BVDC_P_SUPPORT_MFD_VER                (9)
#define BVDC_P_SUPPORT_VDEC                   (0)
#define BVDC_P_SUPPORT_VDEC_VER               (0)
#define BVDC_P_SUPPORT_VDEC_PASSTHROUGH       (0)
#define BVDC_P_SUPPORT_HDDVI                  (0)
#define BVDC_P_SUPPORT_HDDVI_VER              (1)
#define BVDC_P_SUPPORT_DOWNSAMPLE             (0)
#define BVDC_P_SUPPORT_VCR_MODE               (0)

/* BVN */
#define BVDC_P_SUPPORT_PEP                    (1)
#define BVDC_P_SUPPORT_PEP_VER                (1)
#define BVDC_P_SUPPORT_TAB                    (1)
#define BVDC_P_SUPPORT_TNT                    (0)
#define BVDC_P_SUPPORT_TNT_VER                (0)
#define BVDC_P_SUPPORT_MASK_DITHER            (2)
#define BVDC_P_SUPPORT_HIST                   (1)
#define BVDC_P_SUPPORT_HIST_VER               (1)
#define BVDC_P_SUPPORT_MAD_VER                (7)
#define BVDC_P_SUPPORT_BOX_DETECT             (1)
#define BVDC_P_SUPPORT_BOX_DETECT_VER         (1)
#define BVDC_P_SUPPORT_CAP                    (2)
#define BVDC_P_SUPPORT_VFD                    (2)
#define BVDC_P_SUPPORT_SCL                    (2)
#define BVDC_P_SUPPORT_SCL_VER                (6)
#define BVDC_P_SUPPORT_HSCL                   (0)
#define BVDC_P_SUPPORT_HSCL_VER               (4)
#define BVDC_P_SUPPORT_HSCL_MAD_HARD_WIRED    (1)
#define BVDC_P_SUPPORT_FMISC_PFRI             (0)
#define BVDC_P_SUPPORT_FMISC_MEMC             (0)
#define BVDC_P_SUPPORT_DMISC                  (1)
#define BVDC_P_SUPPORT_BVN_444                (0)
#define BVDC_P_SUPPORT_BVN_10BIT_444          (0)
#define BVDC_P_SUPPORT_CMP_TEN_TAP_422_TO_444 (1)
#define BVDC_P_SUPPORT_MAD_SRC_1080I          (0)
#define BVDC_P_SUPPORT_ANR_VER                (0)
#define BVDC_P_SUPPORT_ANR                    (0)
#define BVDC_P_SUPPORT_MAD                    (1)
#define BVDC_P_SUPPORT_ANR_MAD_MEM_SAVING_MODE  (1)
#define BVDC_P_SUPPORT_FREE_CHANNEL           (3)
#define BVDC_P_SUPPORT_DRAIN_F                (1)
#define BVDC_P_SUPPORT_DRAIN_B                (1)
#define BVDC_P_SUPPORT_DRAIN_VER              (0)
#define BVDC_P_SUPPORT_DITHER                 (0)
#define BVDC_P_SUPPORT_MCVP                   (0)
#define BVDC_P_SUPPORT_MCVP_VER               (0)
#define BVDC_P_SUPPORT_MCDI_VER               (0) /* MCDI HW version */
#define BVDC_P_SUPPORT_MADR                   (0) /* Number of MAD-R HW */
#define BVDC_P_SUPPORT_MADR_VER               (0) /* MAD-R HW version */
#define BVDC_P_SUPPORT_MANR                   (0) /* Number of ANR HW in MCVP */
#define BVDC_P_SUPPORT_MANR_VER               (0) /* ANR in MCVP version */
#define BVDC_P_SUPPORT_OSCL                   (0)
#define BVDC_P_SUPPORT_VIDEO_TESTFEATURE1_VER (0)

/* CMP */
#define BVDC_P_CMP_0_MAX_VIDEO_WINDOW_COUNT   (1)
#define BVDC_P_CMP_0_MAX_GFX_WINDOW_COUNT     (1)
#define BVDC_P_CMP_1_MAX_VIDEO_WINDOW_COUNT   (1)
#define BVDC_P_CMP_1_MAX_GFX_WINDOW_COUNT     (1)
#define BVDC_P_CMP_2_MAX_VIDEO_WINDOW_COUNT   (0)
#define BVDC_P_CMP_2_MAX_GFX_WINDOW_COUNT     (0)
#define BVDC_P_CMP_3_MAX_VIDEO_WINDOW_COUNT   (0) /* Number of CMP_3_Vx */
#define BVDC_P_CMP_3_MAX_GFX_WINDOW_COUNT     (0) /* Number of CMP_3_Gx */
#define BVDC_P_CMP_4_MAX_VIDEO_WINDOW_COUNT   (0) /* Number of CMP_4_Vx */
#define BVDC_P_CMP_4_MAX_GFX_WINDOW_COUNT     (0) /* Number of CMP_4_Gx */
#define BVDC_P_CMP_5_MAX_VIDEO_WINDOW_COUNT   (0) /* Number of CMP_5_Vx */
#define BVDC_P_CMP_5_MAX_GFX_WINDOW_COUNT     (0) /* Number of CMP_5_Gx */
#define BVDC_P_CMP_6_MAX_VIDEO_WINDOW_COUNT   (0) /* Number of CMP_6_Vx */
#define BVDC_P_CMP_6_MAX_GFX_WINDOW_COUNT     (0) /* Number of CMP_6_Gx */
#define BVDC_P_CMP_0_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_0_NON_LINEAR_CSC_V1        (0)
#define BVDC_P_CMP_1_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_1_NON_LINEAR_CSC_V1        (0)
#define BVDC_P_CMP_2_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_3_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_4_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_5_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_6_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_0_CLEAR_RECT_V0            (16)
#define BVDC_P_CMP_0_CLEAR_RECT_V1            (0)
#define BVDC_P_CMP_1_CLEAR_RECT_V0            (16)
#define BVDC_P_CMP_1_CLEAR_RECT_V1            (0)
#define BVDC_P_CMP_2_CLEAR_RECT_V0            (0)
#define BVDC_P_CMP_3_CLEAR_RECT_V0            (0)
#define BVDC_P_CMP_4_CLEAR_RECT_V0            (0)
#define BVDC_P_CMP_5_CLEAR_RECT_V0            (0)
#define BVDC_P_CMP_6_CLEAR_RECT_V0            (0)
#define BVDC_P_SUPPORT_WIN_CONST_COLOR        (1)
#define BVDC_P_SUPPORT_CM3D                   (0)
#define BVDC_P_SUPPORT_CMP_DERINGING          (1)   /* TODO: ? */
#define BVDC_P_SUPPORT_CMP_MOSAIC_CSC         (0)

/* csc */
#define BVDC_P_SUPPORT_CMP_DEMO_MODE          (2)
#define BVDC_P_SUPPORT_CSC_MAT_COEF_VER       (3)
#define BVDC_P_SUPPORT_COLOR_CLIP             (1)

/* display */
#define BVDC_P_SUPPORT_EARLY_TRIGGER          (0)
#define BVDC_P_SUPPORT_VBI_ENC_656            (0)
#define BVDC_P_SUPPORT_NEW_SW_INIT            (0)
#define BVDC_P_SUPPORT_4_DACS_PRIM_VEC        (0)
#define BVDC_P_SUPPORT_VEC_VF_VER             (1)
#define BVDC_P_SUPPORT_AMOL_VBI_ENCODE        (1)
#define BVDC_P_SUPPORT_HDMI_OUT               (1)
#define BVDC_P_SUPPORT_DVI_OUT                (1)
#define BVDC_P_SUPPORT_HDMI_RM_VER            (4)
#define BVDC_P_SUPPORT_ITU656_OUT             (0)
#define BVDC_P_SUPPORT_RFM_OUTPUT             (0)
#define BVDC_P_SUPPORT_EXT_SYNC_OUT_MUX       (0)
#define BVDC_P_SUPPORT_EXT_SYNC_PCL_0         (0)
#define BVDC_P_SUPPORT_TDAC_VER               (7)
#define BVDC_P_SUPPORT_QDAC_VER               (0)
#define BVDC_P_SUPPORT_1080p_60HZ             (0)
#define BVDC_P_SUPPORT_MUD                    (0)
#define BVDC_P_MAX_DACS                       (6)
#define BVDC_P_VEC_HAS_2_DIFFERENT_DVF        (0)
#define BVDC_P_VEC_SUPPORT_DVI_COLOR_CNVT     (0)
#define BVDC_P_ORTHOGONAL_VEC                 (1)
#define BVDC_P_ORTHOGONAL_VEC_VER             (0)
#define BVDC_P_SUPPORT_HD_DAC                 (1)
#define BVDC_P_SUPPORT_IT_VER                 (1)
#define BVDC_P_SUPPORT_STG_VER                (0)

#define BVDC_P_NUM_SHARED_656                 (0)
#define BVDC_P_NUM_SHARED_DVI                 (1)
#define BVDC_P_NUM_SHARED_STG                 (0)
#define BVDC_P_NUM_SHARED_RF                  (0)
#define BVDC_P_NUM_SHARED_IT                  (2)
#define BVDC_P_NUM_SHARED_VF                  (3)
#define BVDC_P_NUM_SHARED_SDSRC               (2)
#define BVDC_P_NUM_SHARED_HDSRC               (1)
#define BVDC_P_NUM_SHARED_SM                  (2)
/* Note, here SHARED_SECAM/HDSECAM means the secam cross bar.
 * Some secam cross bar may not have a secam module associated with it.
 */
#define BVDC_P_NUM_SHARED_SECAM               (2)
#define BVDC_P_NUM_SHARED_SECAM_HD            (1)
#define BVDC_P_NUM_SHARED_DAC                 BVDC_P_MAX_DACS
#define BVDC_P_SUPPORT_3D_VIDEO               (0)
#define BVDC_P_SUPPORT_3D_INDEP_SRC_CLIP      (0)
#define BVDC_P_SUPPORT_STG                    (0)

#elif (BCHP_CHIP==7342)

#if (BCHP_VER >= BCHP_VER_B0)
#define BVDC_P_SUPPORT_CLOCK_GATING           (1)
#else
#define BVDC_P_SUPPORT_CLOCK_GATING           (0)
#endif

#define BVDC_P_SUPPORT_FGT                    (0)
#define BVDC_P_SUPPORT_FGT_VER                (0)

#define BVDC_P_SUPPORT_656_MASTER_MODE        (0)
#define BVDC_P_SUPPORT_DVO_MASTER_MODE        (1)
#define BVDC_P_SUPPORT_VEC_MUXING             (1)
#define BVDC_P_SUPPORT_LOOP_BACK              (2)
#define BVDC_P_SUPPORT_DNR                    (1)
#define BVDC_P_SUPPORT_DNR_VER                (3)
#define BVDC_P_SUPPORT_MODULAR_VEC            (2)
#define BVDC_P_SUPPORT_NEW_656_IN_VER         (0)
#define BVDC_P_SUPPORT_VEC_GRPD               (0)
#define BVDC_P_SUPPORT_VEC_SECAM              (1)
#define BVDC_P_SUPPORT_CAP_VER                (2)
#define BVDC_P_SUPPORT_CMP_CRC                (1)
#define BVDC_P_SUPPORT_CMP_LUMA_AVG_VER       (0)

/* source */
#define BVDC_P_SUPPORT_GFD                    (2)
#define BVDC_P_SUPPORT_GFD_VER                (3)
#define BVDC_P_SUPPORT_GFD1_VER               (3)
#define BVDC_P_SUPPORT_MFD                    (1)
#define BVDC_P_SUPPORT_MFD_VER                (9)
#define BVDC_P_SUPPORT_VDEC                   (0)
#define BVDC_P_SUPPORT_VDEC_VER               (0)
#define BVDC_P_SUPPORT_VDEC_PASSTHROUGH       (0)
#define BVDC_P_SUPPORT_HDDVI                  (0)
#define BVDC_P_SUPPORT_HDDVI_VER              (1)
#define BVDC_P_SUPPORT_DOWNSAMPLE             (0)
#define BVDC_P_SUPPORT_VCR_MODE               (0)

/* BVN */
#define BVDC_P_SUPPORT_PEP                    (1)
#define BVDC_P_SUPPORT_PEP_VER                (1)
#define BVDC_P_SUPPORT_TAB                    (1)
#define BVDC_P_SUPPORT_TNT                    (0)
#define BVDC_P_SUPPORT_TNT_VER                (0)
#define BVDC_P_SUPPORT_MASK_DITHER            (2)
#define BVDC_P_SUPPORT_HIST                   (1)
#define BVDC_P_SUPPORT_HIST_VER               (1)
#define BVDC_P_SUPPORT_MAD_VER                (7)
#define BVDC_P_SUPPORT_BOX_DETECT             (1)
#define BVDC_P_SUPPORT_BOX_DETECT_VER         (1)
#define BVDC_P_SUPPORT_CAP                    (2)
#define BVDC_P_SUPPORT_VFD                    (2)
#define BVDC_P_SUPPORT_SCL                    (2)
#define BVDC_P_SUPPORT_SCL_VER                (6)
#define BVDC_P_SUPPORT_HSCL                   (0)
#define BVDC_P_SUPPORT_HSCL_VER               (4)
#define BVDC_P_SUPPORT_HSCL_MAD_HARD_WIRED    (1)
#define BVDC_P_SUPPORT_FMISC_PFRI             (0)
#define BVDC_P_SUPPORT_FMISC_MEMC             (0)
#define BVDC_P_SUPPORT_DMISC                  (1)
#define BVDC_P_SUPPORT_BVN_444                (0)
#define BVDC_P_SUPPORT_BVN_10BIT_444          (0)
#define BVDC_P_SUPPORT_CMP_TEN_TAP_422_TO_444 (1)
#define BVDC_P_SUPPORT_MAD_SRC_1080I          (0)
#define BVDC_P_SUPPORT_ANR_VER                (0)
#define BVDC_P_SUPPORT_ANR                    (0)
#define BVDC_P_SUPPORT_MAD                    (1)
#define BVDC_P_SUPPORT_ANR_MAD_MEM_SAVING_MODE  (1)
#define BVDC_P_SUPPORT_FREE_CHANNEL           (3)
#define BVDC_P_SUPPORT_DRAIN_F                (1)
#define BVDC_P_SUPPORT_DRAIN_B                (1)
#define BVDC_P_SUPPORT_DRAIN_VER              (0)
#define BVDC_P_SUPPORT_DITHER                 (0)
#define BVDC_P_SUPPORT_MCVP                   (0)
#define BVDC_P_SUPPORT_MCVP_VER               (0)
#define BVDC_P_SUPPORT_MCDI_VER               (0) /* MCDI HW version */
#define BVDC_P_SUPPORT_MADR                   (0) /* Number of MAD-R HW */
#define BVDC_P_SUPPORT_MADR_VER               (0) /* MAD-R HW version */
#define BVDC_P_SUPPORT_MANR                   (0) /* Number of ANR HW in MCVP */
#define BVDC_P_SUPPORT_MANR_VER               (0) /* ANR in MCVP version */
#define BVDC_P_SUPPORT_OSCL                   (0)
#define BVDC_P_SUPPORT_VIDEO_TESTFEATURE1_VER (0)

/* CMP */
#define BVDC_P_CMP_0_MAX_VIDEO_WINDOW_COUNT   (1)
#define BVDC_P_CMP_0_MAX_GFX_WINDOW_COUNT     (1)
#define BVDC_P_CMP_1_MAX_VIDEO_WINDOW_COUNT   (1)
#define BVDC_P_CMP_1_MAX_GFX_WINDOW_COUNT     (1)
#define BVDC_P_CMP_2_MAX_VIDEO_WINDOW_COUNT   (0)
#define BVDC_P_CMP_2_MAX_GFX_WINDOW_COUNT     (0)
#define BVDC_P_CMP_3_MAX_VIDEO_WINDOW_COUNT   (0) /* Number of CMP_3_Vx */
#define BVDC_P_CMP_3_MAX_GFX_WINDOW_COUNT     (0) /* Number of CMP_3_Gx */
#define BVDC_P_CMP_4_MAX_VIDEO_WINDOW_COUNT   (0) /* Number of CMP_4_Vx */
#define BVDC_P_CMP_4_MAX_GFX_WINDOW_COUNT     (0) /* Number of CMP_4_Gx */
#define BVDC_P_CMP_5_MAX_VIDEO_WINDOW_COUNT   (0) /* Number of CMP_5_Vx */
#define BVDC_P_CMP_5_MAX_GFX_WINDOW_COUNT     (0) /* Number of CMP_5_Gx */
#define BVDC_P_CMP_6_MAX_VIDEO_WINDOW_COUNT   (0) /* Number of CMP_6_Vx */
#define BVDC_P_CMP_6_MAX_GFX_WINDOW_COUNT     (0) /* Number of CMP_6_Gx */
#define BVDC_P_CMP_0_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_0_NON_LINEAR_CSC_V1        (0)
#define BVDC_P_CMP_1_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_1_NON_LINEAR_CSC_V1        (0)
#define BVDC_P_CMP_2_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_3_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_4_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_5_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_6_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_0_CLEAR_RECT_V0            (16)
#define BVDC_P_CMP_0_CLEAR_RECT_V1            (0)
#define BVDC_P_CMP_1_CLEAR_RECT_V0            (16)
#define BVDC_P_CMP_1_CLEAR_RECT_V1            (0)
#define BVDC_P_CMP_2_CLEAR_RECT_V0            (0)
#define BVDC_P_CMP_3_CLEAR_RECT_V0            (0)
#define BVDC_P_CMP_4_CLEAR_RECT_V0            (0)
#define BVDC_P_CMP_5_CLEAR_RECT_V0            (0)
#define BVDC_P_CMP_6_CLEAR_RECT_V0            (0)
#define BVDC_P_SUPPORT_WIN_CONST_COLOR        (1)
#define BVDC_P_SUPPORT_CM3D                   (0)
#define BVDC_P_SUPPORT_CMP_DERINGING          (1)   /* TODO: ? */
#define BVDC_P_SUPPORT_CMP_MOSAIC_CSC         (0)

/* csc */
#define BVDC_P_SUPPORT_CMP_DEMO_MODE          (2)
#define BVDC_P_SUPPORT_CSC_MAT_COEF_VER       (3)
#define BVDC_P_SUPPORT_COLOR_CLIP             (1)

/* display */
#define BVDC_P_SUPPORT_EARLY_TRIGGER          (0)
#define BVDC_P_SUPPORT_VBI_ENC_656            (1)
#define BVDC_P_SUPPORT_NEW_SW_INIT            (0)
#define BVDC_P_SUPPORT_4_DACS_PRIM_VEC        (0)
#define BVDC_P_SUPPORT_VEC_VF_VER             (1)
#define BVDC_P_SUPPORT_AMOL_VBI_ENCODE        (1)
#define BVDC_P_SUPPORT_HDMI_OUT               (1)
#define BVDC_P_SUPPORT_DVI_OUT                (1)
#define BVDC_P_SUPPORT_HDMI_RM_VER            (4)
#define BVDC_P_SUPPORT_ITU656_OUT             (1)
#define BVDC_P_SUPPORT_RFM_OUTPUT             (1)
#define BVDC_P_SUPPORT_EXT_SYNC_OUT_MUX       (0)
#define BVDC_P_SUPPORT_EXT_SYNC_PCL_0         (0)
#define BVDC_P_SUPPORT_TDAC_VER               (7)
#define BVDC_P_SUPPORT_QDAC_VER               (0)
#define BVDC_P_SUPPORT_1080p_60HZ             (0)
#define BVDC_P_SUPPORT_MUD                    (0)
#define BVDC_P_MAX_DACS                       (6)
#define BVDC_P_VEC_HAS_2_DIFFERENT_DVF        (0)
#define BVDC_P_VEC_SUPPORT_DVI_COLOR_CNVT     (0)
#define BVDC_P_ORTHOGONAL_VEC                 (1)
#define BVDC_P_ORTHOGONAL_VEC_VER             (0)
#define BVDC_P_SUPPORT_HD_DAC                 (1)
#define BVDC_P_SUPPORT_IT_VER                 (1)
#define BVDC_P_SUPPORT_STG_VER                (0)

#define BVDC_P_NUM_SHARED_656                 (1)
#define BVDC_P_NUM_SHARED_DVI                 (1)
#define BVDC_P_NUM_SHARED_STG                 (0)
#define BVDC_P_NUM_SHARED_RF                  (0)
#define BVDC_P_NUM_SHARED_IT                  (2)
#define BVDC_P_NUM_SHARED_VF                  (2)
#define BVDC_P_NUM_SHARED_SDSRC               (1)
#define BVDC_P_NUM_SHARED_HDSRC               (1)
#define BVDC_P_NUM_SHARED_SM                  (1)
/* Note, here SHARED_SECAM/HDSECAM means the secam cross bar.
 * Some secam cross bar may not have a secam module associated with it.
 */
#define BVDC_P_NUM_SHARED_SECAM               (1)
#define BVDC_P_NUM_SHARED_SECAM_HD            (1)
#define BVDC_P_NUM_SHARED_DAC                 BVDC_P_MAX_DACS
#define BVDC_P_SUPPORT_3D_VIDEO               (0)
#define BVDC_P_SUPPORT_3D_INDEP_SRC_CLIP      (0)
#define BVDC_P_SUPPORT_STG                    (0)

#elif (BCHP_CHIP==7550)

#if (BCHP_VER >= BCHP_VER_B0)
#define BVDC_P_SUPPORT_VEC_GRPD               (1)
#else
#define BVDC_P_SUPPORT_VEC_GRPD               (0)
#endif

#define BVDC_P_SUPPORT_VEC_DITHER             (1)
#define BVDC_P_SUPPORT_FGT_VER                (0)

#define BVDC_P_SUPPORT_656_MASTER_MODE        (1)
#define BVDC_P_SUPPORT_DVO_MASTER_MODE        (1)
#define BVDC_P_SUPPORT_VEC_MUXING             (1)
#define BVDC_P_SUPPORT_LOOP_BACK              (2)
#define BVDC_P_SUPPORT_DNR                    (0)
#define BVDC_P_SUPPORT_DNR_VER                (0)
#define BVDC_P_SUPPORT_MODULAR_VEC            (2)
#define BVDC_P_SUPPORT_NEW_656_IN_VER         (0)
#define BVDC_P_SUPPORT_VEC_SECAM              (0)
#define BVDC_P_SUPPORT_CAP_VER                (2)
#define BVDC_P_SUPPORT_CMP_CRC                (1)
#define BVDC_P_SUPPORT_CMP_LUMA_AVG_VER       (0)

/* source */
#define BVDC_P_SUPPORT_GFD                    (2)
#define BVDC_P_SUPPORT_GFD_VER                (3)
#define BVDC_P_SUPPORT_GFD1_VER               (0)
#define BVDC_P_SUPPORT_MFD                    (2)
#define BVDC_P_SUPPORT_MFD_VER                (9)
#define BVDC_P_SUPPORT_VDEC                   (0)
#define BVDC_P_SUPPORT_VDEC_VER               (0)
#define BVDC_P_SUPPORT_VDEC_PASSTHROUGH       (0)
#define BVDC_P_SUPPORT_HDDVI                  (0)
#define BVDC_P_SUPPORT_HDDVI_VER              (1)
#define BVDC_P_SUPPORT_DOWNSAMPLE             (0)
#define BVDC_P_SUPPORT_VCR_MODE               (0)

/* BVN */
#define BVDC_P_SUPPORT_PEP                    (0)
#define BVDC_P_SUPPORT_PEP_VER                (0)
#define BVDC_P_SUPPORT_TAB                    (0)
#define BVDC_P_SUPPORT_TNT                    (0)
#define BVDC_P_SUPPORT_TNT_VER                (0)
#define BVDC_P_SUPPORT_MASK_DITHER            (0)
#define BVDC_P_SUPPORT_HIST                   (0)
#define BVDC_P_SUPPORT_HIST_VER               (0)
#define BVDC_P_SUPPORT_MAD_VER                (7)
#define BVDC_P_SUPPORT_BOX_DETECT             (0)
#define BVDC_P_SUPPORT_BOX_DETECT_VER         (0)
#define BVDC_P_SUPPORT_CAP                    (2)
#define BVDC_P_SUPPORT_VFD                    (2)
#define BVDC_P_SUPPORT_SCL                    (2)
#define BVDC_P_SUPPORT_SCL_VER                (6)
#define BVDC_P_SUPPORT_HSCL                   (0)
#define BVDC_P_SUPPORT_HSCL_VER               (4)
#define BVDC_P_SUPPORT_HSCL_MAD_HARD_WIRED    (1)
#define BVDC_P_SUPPORT_FMISC_PFRI             (0)
#define BVDC_P_SUPPORT_FMISC_MEMC             (0)
#define BVDC_P_SUPPORT_DMISC                  (1)
#define BVDC_P_SUPPORT_BVN_444                (0)
#define BVDC_P_SUPPORT_BVN_10BIT_444          (0)
#define BVDC_P_SUPPORT_CMP_TEN_TAP_422_TO_444 (1)
#define BVDC_P_SUPPORT_MAD_SRC_1080I          (0)
#define BVDC_P_SUPPORT_ANR_VER                (0)
#define BVDC_P_SUPPORT_ANR                    (0)
#define BVDC_P_SUPPORT_MAD                    (1)
#define BVDC_P_SUPPORT_ANR_MAD_MEM_SAVING_MODE  (0)
#define BVDC_P_SUPPORT_FREE_CHANNEL           (6)
#define BVDC_P_SUPPORT_DRAIN_F                (1)
#define BVDC_P_SUPPORT_DRAIN_B                (1)
#define BVDC_P_SUPPORT_DRAIN_VER              (0)
#define BVDC_P_SUPPORT_FGT                    (0)
#define BVDC_P_SUPPORT_DITHER                 (0)
#define BVDC_P_SUPPORT_MCVP                   (0)
#define BVDC_P_SUPPORT_MCVP_VER               (0)
#define BVDC_P_SUPPORT_MCDI_VER               (0) /* MCDI HW version */
#define BVDC_P_SUPPORT_MADR                   (0) /* Number of MAD-R HW */
#define BVDC_P_SUPPORT_MADR_VER               (0) /* MAD-R HW version */
#define BVDC_P_SUPPORT_MANR                   (0) /* Number of ANR HW in MCVP */
#define BVDC_P_SUPPORT_MANR_VER               (0) /* ANR in MCVP version */
#define BVDC_P_SUPPORT_OSCL                   (0)
#define BVDC_P_SUPPORT_VIDEO_TESTFEATURE1_VER (0)

/* CMP */
#define BVDC_P_CMP_0_MAX_VIDEO_WINDOW_COUNT   (1)
#define BVDC_P_CMP_0_MAX_GFX_WINDOW_COUNT     (1)
#define BVDC_P_CMP_1_MAX_VIDEO_WINDOW_COUNT   (1)
#define BVDC_P_CMP_1_MAX_GFX_WINDOW_COUNT     (1)
#define BVDC_P_CMP_2_MAX_VIDEO_WINDOW_COUNT   (0)
#define BVDC_P_CMP_2_MAX_GFX_WINDOW_COUNT     (0)
#define BVDC_P_CMP_3_MAX_VIDEO_WINDOW_COUNT   (0) /* Number of CMP_3_Vx */
#define BVDC_P_CMP_3_MAX_GFX_WINDOW_COUNT     (0) /* Number of CMP_3_Gx */
#define BVDC_P_CMP_4_MAX_VIDEO_WINDOW_COUNT   (0) /* Number of CMP_4_Vx */
#define BVDC_P_CMP_4_MAX_GFX_WINDOW_COUNT     (0) /* Number of CMP_4_Gx */
#define BVDC_P_CMP_5_MAX_VIDEO_WINDOW_COUNT   (0) /* Number of CMP_5_Vx */
#define BVDC_P_CMP_5_MAX_GFX_WINDOW_COUNT     (0) /* Number of CMP_5_Gx */
#define BVDC_P_CMP_6_MAX_VIDEO_WINDOW_COUNT   (0) /* Number of CMP_6_Vx */
#define BVDC_P_CMP_6_MAX_GFX_WINDOW_COUNT     (0) /* Number of CMP_6_Gx */
#define BVDC_P_CMP_0_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_0_NON_LINEAR_CSC_V1        (0)
#define BVDC_P_CMP_1_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_1_NON_LINEAR_CSC_V1        (0)
#define BVDC_P_CMP_2_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_3_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_4_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_5_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_6_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_0_CLEAR_RECT_V0            (0)
#define BVDC_P_CMP_0_CLEAR_RECT_V1            (0)
#define BVDC_P_CMP_1_CLEAR_RECT_V0            (0)
#define BVDC_P_CMP_1_CLEAR_RECT_V1            (0)
#define BVDC_P_CMP_2_CLEAR_RECT_V0            (0)
#define BVDC_P_CMP_3_CLEAR_RECT_V0            (0)
#define BVDC_P_CMP_4_CLEAR_RECT_V0            (0)
#define BVDC_P_CMP_5_CLEAR_RECT_V0            (0)
#define BVDC_P_CMP_6_CLEAR_RECT_V0            (0)
#define BVDC_P_SUPPORT_WIN_CONST_COLOR        (1)
#define BVDC_P_SUPPORT_CM3D                   (0)
#define BVDC_P_SUPPORT_CMP_DERINGING          (0)
#define BVDC_P_SUPPORT_CMP_MOSAIC_CSC         (0)

/* csc */
#define BVDC_P_SUPPORT_CMP_DEMO_MODE          (2)
#define BVDC_P_SUPPORT_CSC_MAT_COEF_VER       (3)
#define BVDC_P_SUPPORT_COLOR_CLIP             (0)

/* display */
#define BVDC_P_SUPPORT_EARLY_TRIGGER          (0)
#define BVDC_P_SUPPORT_VBI_ENC_656            (0)
#define BVDC_P_SUPPORT_NEW_SW_INIT            (0)
#define BVDC_P_SUPPORT_4_DACS_PRIM_VEC        (0)
#define BVDC_P_SUPPORT_VEC_VF_VER             (1)
#define BVDC_P_SUPPORT_AMOL_VBI_ENCODE        (1)
#define BVDC_P_SUPPORT_HDMI_OUT               (1)
#define BVDC_P_SUPPORT_DVI_OUT                (1)
#define BVDC_P_SUPPORT_HDMI_RM_VER            (4)
#define BVDC_P_SUPPORT_ITU656_OUT             (0)
#define BVDC_P_SUPPORT_RFM_OUTPUT             (0)
#define BVDC_P_SUPPORT_EXT_SYNC_OUT_MUX       (0)
#define BVDC_P_SUPPORT_EXT_SYNC_PCL_0         (0)
#define BVDC_P_SUPPORT_TDAC_VER               (8)
#define BVDC_P_SUPPORT_QDAC_VER               (0)
#define BVDC_P_SUPPORT_1080p_60HZ             (0)
#define BVDC_P_SUPPORT_MUD                    (0)
#define BVDC_P_MAX_DACS                       (4)
#define BVDC_P_VEC_HAS_2_DIFFERENT_DVF        (0)
#define BVDC_P_VEC_SUPPORT_DVI_COLOR_CNVT     (0)
#define BVDC_P_ORTHOGONAL_VEC                 (1)
#define BVDC_P_ORTHOGONAL_VEC_VER             (0)
#define BVDC_P_SUPPORT_HD_DAC                 (1)
#define BVDC_P_SUPPORT_IT_VER                 (1)
#define BVDC_P_SUPPORT_STG_VER                (0)

#define BVDC_P_NUM_SHARED_656                 (0)
#define BVDC_P_NUM_SHARED_DVI                 (1)
#define BVDC_P_NUM_SHARED_STG                 (0)
#define BVDC_P_NUM_SHARED_RF                  (0)
#define BVDC_P_NUM_SHARED_IT                  (2)
#define BVDC_P_NUM_SHARED_VF                  (2)
#define BVDC_P_NUM_SHARED_SDSRC               (1)
#define BVDC_P_NUM_SHARED_HDSRC               (1)
#define BVDC_P_NUM_SHARED_SM                  (1)
/* Note, here SHARED_SECAM/HDSECAM means the secam cross bar.
 * Some secam cross bar may not have a secam module associated with it.
 */
#define BVDC_P_NUM_SHARED_SECAM               (1)
#define BVDC_P_NUM_SHARED_SECAM_HD            (1)
#define BVDC_P_NUM_SHARED_DAC                 BVDC_P_MAX_DACS
#define BVDC_P_SUPPORT_CLOCK_GATING           (0)
#define BVDC_P_SUPPORT_3D_VIDEO               (0)
#define BVDC_P_SUPPORT_3D_INDEP_SRC_CLIP      (0)
#define BVDC_P_SUPPORT_STG                    (0)

#elif (BCHP_CHIP==7420)
#if (BCHP_VER >= BCHP_VER_B0)
#define BVDC_P_SUPPORT_WIN_CONST_COLOR        (1)
#else
#define BVDC_P_SUPPORT_WIN_CONST_COLOR        (0)
#endif

#define BVDC_P_SUPPORT_656_MASTER_MODE        (0)
#define BVDC_P_SUPPORT_DVO_MASTER_MODE        (1)
#define BVDC_P_SUPPORT_VEC_MUXING             (1)
#define BVDC_P_SUPPORT_TDAC_VER               (6) /* TODO */
#define BVDC_P_SUPPORT_QDAC_VER               (0) /* TODO */
#define BVDC_P_SUPPORT_LOOP_BACK              (2)
#define BVDC_P_SUPPORT_DNR                    (1)
#define BVDC_P_SUPPORT_DNR_VER                (3)
#define BVDC_P_SUPPORT_MODULAR_VEC            (2)
#define BVDC_P_SUPPORT_NEW_656_IN_VER         (0)
#define BVDC_P_SUPPORT_VEC_GRPD               (0)
#define BVDC_P_SUPPORT_VEC_SECAM              (1)
#define BVDC_P_SUPPORT_CAP_VER                (2)
#define BVDC_P_SUPPORT_CMP_CRC                (1)

/* source */
#define BVDC_P_SUPPORT_GFD                    (1)
#define BVDC_P_SUPPORT_GFD_VER                (3)
#define BVDC_P_SUPPORT_GFD1_VER               (3)
#define BVDC_P_SUPPORT_MFD                    (1)
#define BVDC_P_SUPPORT_MFD_VER                (8)
#define BVDC_P_SUPPORT_VDEC                   (0)
#define BVDC_P_SUPPORT_VDEC_VER               (0)
#define BVDC_P_SUPPORT_VDEC_PASSTHROUGH       (0)
#define BVDC_P_SUPPORT_HDDVI                  (1)
#define BVDC_P_SUPPORT_DOWNSAMPLE             (0)
#define BVDC_P_SUPPORT_VCR_MODE               (0)

/* BVN */
#define BVDC_P_SUPPORT_PEP                    (1)
#define BVDC_P_SUPPORT_PEP_VER                (1)
#define BVDC_P_SUPPORT_HIST                   (1)
#define BVDC_P_SUPPORT_HIST_VER               (1)
#define BVDC_P_SUPPORT_TAB                    (0)
#define BVDC_P_SUPPORT_TNT                    (1)
#define BVDC_P_SUPPORT_TNT_VER                (3)
#define BVDC_P_SUPPORT_MASK_DITHER            (3)
#define BVDC_P_SUPPORT_FGT                    (0)
#define BVDC_P_SUPPORT_FGT_VER                (0)
#define BVDC_P_SUPPORT_MAD                    (0)
#define BVDC_P_SUPPORT_MAD_VER                (7)
#define BVDC_P_SUPPORT_BOX_DETECT             (2)
#define BVDC_P_SUPPORT_BOX_DETECT_VER         (1)
#define BVDC_P_SUPPORT_CAP                    (4)
#define BVDC_P_SUPPORT_VFD                    (4)
#define BVDC_P_SUPPORT_SCL                    (4)
#define BVDC_P_SUPPORT_SCL_VER                (4)
#define BVDC_P_SUPPORT_HSCL                   (0)
#define BVDC_P_SUPPORT_HSCL_VER               (2)
#define BVDC_P_SUPPORT_HSCL_MAD_HARD_WIRED    (1)
#define BVDC_P_SUPPORT_FMISC_PFRI             (0)
#define BVDC_P_SUPPORT_FMISC_MEMC             (0) /* TODO */
#define BVDC_P_SUPPORT_DMISC                  (1)
#define BVDC_P_SUPPORT_BVN_444                (0)
#define BVDC_P_SUPPORT_BVN_10BIT_444          (0)
#define BVDC_P_SUPPORT_CMP_TEN_TAP_422_TO_444 (1)
#define BVDC_P_SUPPORT_MAD_SRC_1080I          (1)
#define BVDC_P_SUPPORT_ANR_VER                (0)
#define BVDC_P_SUPPORT_ANR                    (0)
#define BVDC_P_SUPPORT_ANR_MAD_MEM_SAVING_MODE  (1)
#define BVDC_P_SUPPORT_FREE_CHANNEL           (6)
#define BVDC_P_SUPPORT_DRAIN_F                (1)
#define BVDC_P_SUPPORT_DRAIN_B                (1)
#define BVDC_P_SUPPORT_DRAIN_VER              (0)
#define BVDC_P_SUPPORT_DITHER                 (0)
#define BVDC_P_SUPPORT_VEC_DITHER             (0)
#define BVDC_P_SUPPORT_MCVP                   (1) /* Number of MCVP HW */
#define BVDC_P_SUPPORT_MCVP_VER               (1) /* MCVP HW version */
#define BVDC_P_SUPPORT_MCDI_VER               (1) /* MCDI HW version */
#define BVDC_P_SUPPORT_MADR                   (0) /* TODO: Number of MAD-R HW */
#define BVDC_P_SUPPORT_MADR_VER               (0) /* TODO: MAD-R HW version */
#define BVDC_P_SUPPORT_MANR                   (1) /* Number of ANR HW in MCVP */
#define BVDC_P_SUPPORT_MANR_VER               (1) /* ANR in MCVP version */
#define BVDC_P_SUPPORT_OSCL                   (0)
#define BVDC_P_SUPPORT_VIDEO_TESTFEATURE1_VER (0)

/* CMP */
#define BVDC_P_CMP_0_MAX_VIDEO_WINDOW_COUNT   (2)
#define BVDC_P_CMP_0_MAX_GFX_WINDOW_COUNT     (1)
#define BVDC_P_CMP_1_MAX_VIDEO_WINDOW_COUNT   (2)
#define BVDC_P_CMP_1_MAX_GFX_WINDOW_COUNT     (1)
#define BVDC_P_CMP_2_MAX_VIDEO_WINDOW_COUNT   (1)
#define BVDC_P_CMP_2_MAX_GFX_WINDOW_COUNT     (1)
#define BVDC_P_CMP_3_MAX_VIDEO_WINDOW_COUNT   (0) /* Number of CMP_3_Vx */
#define BVDC_P_CMP_3_MAX_GFX_WINDOW_COUNT     (0) /* Number of CMP_3_Gx */
#define BVDC_P_CMP_4_MAX_VIDEO_WINDOW_COUNT   (0) /* Number of CMP_4_Vx */
#define BVDC_P_CMP_4_MAX_GFX_WINDOW_COUNT     (0) /* Number of CMP_4_Gx */
#define BVDC_P_CMP_5_MAX_VIDEO_WINDOW_COUNT   (0) /* Number of CMP_5_Vx */
#define BVDC_P_CMP_5_MAX_GFX_WINDOW_COUNT     (0) /* Number of CMP_5_Gx */
#define BVDC_P_CMP_6_MAX_VIDEO_WINDOW_COUNT   (0) /* Number of CMP_6_Vx */
#define BVDC_P_CMP_6_MAX_GFX_WINDOW_COUNT     (0) /* Number of CMP_6_Gx */
#define BVDC_P_CMP_0_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_0_NON_LINEAR_CSC_V1        (0)
#define BVDC_P_CMP_1_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_1_NON_LINEAR_CSC_V1        (0)
#define BVDC_P_CMP_2_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_3_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_4_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_5_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_6_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_0_CLEAR_RECT_V0            (16)
#define BVDC_P_CMP_0_CLEAR_RECT_V1            (8)
#define BVDC_P_CMP_1_CLEAR_RECT_V0            (16)
#define BVDC_P_CMP_1_CLEAR_RECT_V1            (8)
#define BVDC_P_CMP_2_CLEAR_RECT_V0            (0)
#define BVDC_P_CMP_3_CLEAR_RECT_V0            (0)
#define BVDC_P_CMP_4_CLEAR_RECT_V0            (0)
#define BVDC_P_CMP_5_CLEAR_RECT_V0            (0)
#define BVDC_P_CMP_6_CLEAR_RECT_V0            (0)
#define BVDC_P_SUPPORT_CM3D                   (0)
#define BVDC_P_SUPPORT_CMP_DERINGING          (1)
#define BVDC_P_SUPPORT_CMP_MOSAIC_CSC         (5)

/* csc */
#define BVDC_P_SUPPORT_CMP_DEMO_MODE          (2)
#define BVDC_P_SUPPORT_CSC_MAT_COEF_VER       (3)
#define BVDC_P_SUPPORT_COLOR_CLIP             (1)

/* display */
#if (BCHP_VER <= BCHP_VER_A1)
#define BVDC_P_SUPPORT_HDDVI_VER              (5)
#else
#define BVDC_P_SUPPORT_HDDVI_VER              (6)
#endif

#define BVDC_P_SUPPORT_SEAMLESS_ATTACH        (0)

#define BVDC_P_SUPPORT_EARLY_TRIGGER          (0)
#define BVDC_P_SUPPORT_VBI_ENC_656            (1)
#define BVDC_P_SUPPORT_NEW_SW_INIT            (0)
#define BVDC_P_SUPPORT_4_DACS_PRIM_VEC        (0)
#define BVDC_P_SUPPORT_VEC_VF_VER             (1)
#define BVDC_P_SUPPORT_AMOL_VBI_ENCODE        (1)
#define BVDC_P_SUPPORT_HDMI_OUT               (1)
#define BVDC_P_SUPPORT_DVI_OUT                (1)
#define BVDC_P_SUPPORT_HDMI_RM_VER            (4)
#define BVDC_P_SUPPORT_ITU656_OUT             (1)
#define BVDC_P_SUPPORT_RFM_OUTPUT             (1)
#define BVDC_P_SUPPORT_EXT_SYNC_OUT_MUX       (0)
#define BVDC_P_SUPPORT_EXT_SYNC_PCL_0         (0)

#define BVDC_P_SUPPORT_1080p_60HZ             (1)
#define BVDC_P_SUPPORT_MUD                    (0)
#define BVDC_P_MAX_DACS                       (7)
#define BVDC_P_VEC_HAS_2_DIFFERENT_DVF        (0)
#define BVDC_P_VEC_SUPPORT_DVI_COLOR_CNVT     (0)
#define BVDC_P_ORTHOGONAL_VEC                 (1)
#define BVDC_P_ORTHOGONAL_VEC_VER             (0)
#define BVDC_P_SUPPORT_HD_DAC                 (1)
#define BVDC_P_SUPPORT_IT_VER                 (1)
#define BVDC_P_SUPPORT_STG_VER                (0)

#define BVDC_P_NUM_SHARED_656                 (1)
#define BVDC_P_NUM_SHARED_DVI                 (1)
#define BVDC_P_NUM_SHARED_STG                 (0)
#define BVDC_P_NUM_SHARED_RF                  (1)
#define BVDC_P_NUM_SHARED_IT                  (3)
#define BVDC_P_NUM_SHARED_VF                  (4)
#define BVDC_P_NUM_SHARED_SDSRC               (3)
#define BVDC_P_NUM_SHARED_HDSRC               (1)
#define BVDC_P_NUM_SHARED_SM                  (3)
/* Note, here SHARED_SECAM/HDSECAM means the secam cross bar.
 * Some secam cross bar may not have a secam module associated with it.
 */
#define BVDC_P_NUM_SHARED_SECAM               (3)
#define BVDC_P_NUM_SHARED_SECAM_HD            (1)
#define BVDC_P_NUM_SHARED_DAC                 BVDC_P_MAX_DACS
#define BVDC_P_SUPPORT_CLOCK_GATING           (0)
#define BVDC_P_SUPPORT_3D_VIDEO               (0)
#define BVDC_P_SUPPORT_3D_INDEP_SRC_CLIP      (0)
#define BVDC_P_SUPPORT_STG                    (0)

#elif (BCHP_CHIP==7125)

#define BVDC_P_SUPPORT_VEC_DITHER             (0)
#define BVDC_P_SUPPORT_FGT_VER                (0)

#define BVDC_P_SUPPORT_656_MASTER_MODE        (0)
#define BVDC_P_SUPPORT_DVO_MASTER_MODE        (0)
#define BVDC_P_SUPPORT_VEC_MUXING             (1)
#define BVDC_P_SUPPORT_LOOP_BACK              (2)
#define BVDC_P_SUPPORT_DNR                    (1)
#define BVDC_P_SUPPORT_DNR_VER                (3)
#define BVDC_P_SUPPORT_MODULAR_VEC            (2)
#define BVDC_P_SUPPORT_NEW_656_IN_VER         (0) /* TODO (2) */
#define BVDC_P_SUPPORT_VEC_GRPD               (0)
#define BVDC_P_SUPPORT_VEC_SECAM              (1)
#define BVDC_P_SUPPORT_CAP_VER                (2)
#define BVDC_P_SUPPORT_CMP_CRC                (1)
#define BVDC_P_SUPPORT_CMP_LUMA_AVG_VER       (0)

#if (BCHP_VER >= BCHP_VER_C0)
#define BVDC_P_SUPPORT_CLOCK_GATING           (0)
#define BVDC_P_SUPPORT_VIDEO_TESTFEATURE1_VER (3)
#define BVDC_P_SUPPORT_MAD_SRC_1080I          (1)
#define BVDC_P_SUPPORT_MAD_VER                (8)
#else
#define BVDC_P_SUPPORT_CLOCK_GATING           (0)
#define BVDC_P_SUPPORT_VIDEO_TESTFEATURE1_VER (0)
#define BVDC_P_SUPPORT_MAD_SRC_1080I          (0)
#define BVDC_P_SUPPORT_MAD_VER                (7)
#endif

/* source */
#define BVDC_P_SUPPORT_GFD                    (1)
#define BVDC_P_SUPPORT_GFD_VER                (2)
#define BVDC_P_SUPPORT_GFD1_VER               (2)
#define BVDC_P_SUPPORT_MFD                    (2)
#define BVDC_P_SUPPORT_MFD_VER                (9)
#define BVDC_P_SUPPORT_VDEC                   (0)
#define BVDC_P_SUPPORT_VDEC_VER               (0)
#define BVDC_P_SUPPORT_VDEC_PASSTHROUGH       (0)
#define BVDC_P_SUPPORT_HDDVI                  (0)
#define BVDC_P_SUPPORT_HDDVI_VER              (1)
#define BVDC_P_SUPPORT_DOWNSAMPLE             (0)
#define BVDC_P_SUPPORT_VCR_MODE               (0)

/* BVN */
#define BVDC_P_SUPPORT_PEP                    (1)
#define BVDC_P_SUPPORT_PEP_VER                (1)
#define BVDC_P_SUPPORT_TAB                    (0)
#define BVDC_P_SUPPORT_TNT                    (1)
#define BVDC_P_SUPPORT_TNT_VER                (3)
#define BVDC_P_SUPPORT_MASK_DITHER            (2)
#define BVDC_P_SUPPORT_HIST                   (1)
#define BVDC_P_SUPPORT_HIST_VER               (1)
#define BVDC_P_SUPPORT_BOX_DETECT             (1)
#define BVDC_P_SUPPORT_BOX_DETECT_VER         (1)
#define BVDC_P_SUPPORT_CAP                    (4)
#define BVDC_P_SUPPORT_VFD                    (4)
#define BVDC_P_SUPPORT_SCL                    (4)
#define BVDC_P_SUPPORT_SCL_VER                (6)
#define BVDC_P_SUPPORT_HSCL                   (0)
#define BVDC_P_SUPPORT_HSCL_VER               (4)
#define BVDC_P_SUPPORT_HSCL_MAD_HARD_WIRED    (1)
#define BVDC_P_SUPPORT_FMISC_PFRI             (0)
#define BVDC_P_SUPPORT_FMISC_MEMC             (0)
#define BVDC_P_SUPPORT_DMISC                  (1)
#define BVDC_P_SUPPORT_BVN_444                (0)
#define BVDC_P_SUPPORT_BVN_10BIT_444          (0)
#define BVDC_P_SUPPORT_CMP_TEN_TAP_422_TO_444 (1)
#define BVDC_P_SUPPORT_ANR_VER                (0)
#define BVDC_P_SUPPORT_ANR                    (0)
#define BVDC_P_SUPPORT_MAD                    (1)
#define BVDC_P_SUPPORT_ANR_MAD_MEM_SAVING_MODE  (0)
#define BVDC_P_SUPPORT_FREE_CHANNEL           (3)
#define BVDC_P_SUPPORT_DRAIN_F                (1)
#define BVDC_P_SUPPORT_DRAIN_B                (1)
#define BVDC_P_SUPPORT_DRAIN_VER              (0)
#define BVDC_P_SUPPORT_FGT                    (0)
#define BVDC_P_SUPPORT_DITHER                 (0)
#define BVDC_P_SUPPORT_MCVP                   (0)
#define BVDC_P_SUPPORT_MCVP_VER               (0)
#define BVDC_P_SUPPORT_MCDI_VER               (0) /* MCDI HW version */
#define BVDC_P_SUPPORT_MADR                   (0) /* Number of MAD-R HW */
#define BVDC_P_SUPPORT_MADR_VER               (0) /* MAD-R HW version */
#define BVDC_P_SUPPORT_MANR                   (0) /* Number of ANR HW in MCVP */
#define BVDC_P_SUPPORT_MANR_VER               (0) /* ANR in MCVP version */
#define BVDC_P_SUPPORT_OSCL                   (0)

/* CMP */
#define BVDC_P_CMP_0_MAX_VIDEO_WINDOW_COUNT   (1)
#define BVDC_P_CMP_0_MAX_GFX_WINDOW_COUNT     (1)
#define BVDC_P_CMP_1_MAX_VIDEO_WINDOW_COUNT   (1)
#define BVDC_P_CMP_1_MAX_GFX_WINDOW_COUNT     (1)
#define BVDC_P_CMP_2_MAX_VIDEO_WINDOW_COUNT   (0)
#define BVDC_P_CMP_2_MAX_GFX_WINDOW_COUNT     (0)
#define BVDC_P_CMP_3_MAX_VIDEO_WINDOW_COUNT   (0) /* Number of CMP_3_Vx */
#define BVDC_P_CMP_3_MAX_GFX_WINDOW_COUNT     (0) /* Number of CMP_3_Gx */
#define BVDC_P_CMP_4_MAX_VIDEO_WINDOW_COUNT   (0) /* Number of CMP_4_Vx */
#define BVDC_P_CMP_4_MAX_GFX_WINDOW_COUNT     (0) /* Number of CMP_4_Gx */
#define BVDC_P_CMP_5_MAX_VIDEO_WINDOW_COUNT   (0) /* Number of CMP_5_Vx */
#define BVDC_P_CMP_5_MAX_GFX_WINDOW_COUNT     (0) /* Number of CMP_5_Gx */
#define BVDC_P_CMP_6_MAX_VIDEO_WINDOW_COUNT   (0) /* Number of CMP_6_Vx */
#define BVDC_P_CMP_6_MAX_GFX_WINDOW_COUNT     (0) /* Number of CMP_6_Gx */
#define BVDC_P_CMP_0_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_0_NON_LINEAR_CSC_V1        (0)
#define BVDC_P_CMP_1_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_1_NON_LINEAR_CSC_V1        (0)
#define BVDC_P_CMP_2_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_3_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_4_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_5_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_6_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_0_CLEAR_RECT_V0            (16)
#define BVDC_P_CMP_0_CLEAR_RECT_V1            (0)
#define BVDC_P_CMP_1_CLEAR_RECT_V0            (16)
#define BVDC_P_CMP_1_CLEAR_RECT_V1            (0)
#define BVDC_P_CMP_2_CLEAR_RECT_V0            (0)
#define BVDC_P_CMP_3_CLEAR_RECT_V0            (0)
#define BVDC_P_CMP_4_CLEAR_RECT_V0            (0)
#define BVDC_P_CMP_5_CLEAR_RECT_V0            (0)
#define BVDC_P_CMP_6_CLEAR_RECT_V0            (0)
#define BVDC_P_SUPPORT_WIN_CONST_COLOR        (1)
#define BVDC_P_SUPPORT_CM3D                   (0)
#define BVDC_P_SUPPORT_CMP_DERINGING          (0)
#define BVDC_P_SUPPORT_CMP_MOSAIC_CSC         (0)

/* csc */
#define BVDC_P_SUPPORT_CMP_DEMO_MODE          (2)
#define BVDC_P_SUPPORT_CSC_MAT_COEF_VER       (3)

/* display */
#define BVDC_P_SUPPORT_EARLY_TRIGGER          (0)
#define BVDC_P_SUPPORT_VBI_ENC_656            (0)
#define BVDC_P_SUPPORT_NEW_SW_INIT            (0)
#define BVDC_P_SUPPORT_4_DACS_PRIM_VEC        (0)
#define BVDC_P_SUPPORT_VEC_VF_VER             (1)
#define BVDC_P_SUPPORT_AMOL_VBI_ENCODE        (1)
#define BVDC_P_SUPPORT_HDMI_OUT               (1)
#define BVDC_P_SUPPORT_DVI_OUT                (1)
#define BVDC_P_SUPPORT_HDMI_RM_VER            (4)
#define BVDC_P_SUPPORT_ITU656_OUT             (0)
#define BVDC_P_SUPPORT_RFM_OUTPUT             (1)
#define BVDC_P_SUPPORT_EXT_SYNC_OUT_MUX       (0)
#define BVDC_P_SUPPORT_EXT_SYNC_PCL_0         (0)
#define BVDC_P_SUPPORT_TDAC_VER               (7)
#define BVDC_P_SUPPORT_QDAC_VER               (0)
#define BVDC_P_SUPPORT_1080p_60HZ             (1)
#define BVDC_P_SUPPORT_MUD                    (0)
#define BVDC_P_MAX_DACS                       (6)
#define BVDC_P_VEC_HAS_2_DIFFERENT_DVF        (0)
#define BVDC_P_VEC_SUPPORT_DVI_COLOR_CNVT     (0)
#define BVDC_P_ORTHOGONAL_VEC                 (1)
#define BVDC_P_ORTHOGONAL_VEC_VER             (0)
#define BVDC_P_SUPPORT_HD_DAC                 (1)
#define BVDC_P_SUPPORT_IT_VER                 (1)
#define BVDC_P_SUPPORT_STG_VER                (0)

#define BVDC_P_NUM_SHARED_656                 (0)
#define BVDC_P_NUM_SHARED_DVI                 (1)
#define BVDC_P_NUM_SHARED_STG                 (0)
#define BVDC_P_NUM_SHARED_RF                  (0)
#define BVDC_P_NUM_SHARED_IT                  (2)
#define BVDC_P_NUM_SHARED_VF                  (3)
#define BVDC_P_NUM_SHARED_SDSRC               (2)
#define BVDC_P_NUM_SHARED_HDSRC               (1)
#define BVDC_P_NUM_SHARED_SM                  (2)
/* Note, here SHARED_SECAM/HDSECAM means the secam cross bar.
 * Some secam cross bar may not have a secam module associated with it.
 */
#define BVDC_P_NUM_SHARED_SECAM               (1)
#define BVDC_P_NUM_SHARED_SECAM_HD            (2)
#define BVDC_P_NUM_SHARED_DAC                 BVDC_P_MAX_DACS
#define BVDC_P_SUPPORT_3D_VIDEO               (0)
#define BVDC_P_SUPPORT_3D_INDEP_SRC_CLIP      (0)
#define BVDC_P_SUPPORT_STG                    (0)

#elif (BCHP_CHIP==7468)

#define BVDC_P_SUPPORT_FGT                    (0)
#define BVDC_P_SUPPORT_FGT_VER                (0)

#define BVDC_P_SUPPORT_656_MASTER_MODE        (0)
#define BVDC_P_SUPPORT_DVO_MASTER_MODE        (0)
#define BVDC_P_SUPPORT_VEC_MUXING             (1)
#define BVDC_P_SUPPORT_LOOP_BACK              (2)
#define BVDC_P_SUPPORT_DNR                    (1)
#define BVDC_P_SUPPORT_DNR_VER                (6)
#define BVDC_P_SUPPORT_MODULAR_VEC            (2)
#define BVDC_P_SUPPORT_NEW_656_IN_VER         (0)
#define BVDC_P_SUPPORT_VEC_GRPD               (0)
#define BVDC_P_SUPPORT_VEC_SECAM              (0)
#define BVDC_P_SUPPORT_CAP_VER                (2)
#define BVDC_P_SUPPORT_CMP_CRC                (1)
#define BVDC_P_SUPPORT_CMP_LUMA_AVG_VER       (0)

/* source */
#define BVDC_P_SUPPORT_GFD                    (2)
#define BVDC_P_SUPPORT_GFD_VER                (3)
#define BVDC_P_SUPPORT_GFD1_VER               (3)
#define BVDC_P_SUPPORT_MFD                    (1)
#define BVDC_P_SUPPORT_MFD_VER                (9)
#define BVDC_P_SUPPORT_VDEC                   (0)
#define BVDC_P_SUPPORT_VDEC_VER               (0)
#define BVDC_P_SUPPORT_VDEC_PASSTHROUGH       (0)
#define BVDC_P_SUPPORT_HDDVI                  (0)
#define BVDC_P_SUPPORT_HDDVI_VER              (1)
#define BVDC_P_SUPPORT_DOWNSAMPLE             (0)
#define BVDC_P_SUPPORT_VCR_MODE               (0)

/* BVN */
#define BVDC_P_SUPPORT_PEP                    (1)
#define BVDC_P_SUPPORT_PEP_VER                (1)
#define BVDC_P_SUPPORT_TAB                    (1)
#define BVDC_P_SUPPORT_TNT                    (0)
#define BVDC_P_SUPPORT_TNT_VER                (0)
#define BVDC_P_SUPPORT_MASK_DITHER            (2)
#define BVDC_P_SUPPORT_HIST                   (1)
#define BVDC_P_SUPPORT_HIST_VER               (1)
#define BVDC_P_SUPPORT_MAD_VER                (7)
#define BVDC_P_SUPPORT_BOX_DETECT             (1)
#define BVDC_P_SUPPORT_BOX_DETECT_VER         (1)
#define BVDC_P_SUPPORT_CAP                    (2)
#define BVDC_P_SUPPORT_VFD                    (2)
#define BVDC_P_SUPPORT_SCL                    (2)
#define BVDC_P_SUPPORT_SCL_VER                (6)
#define BVDC_P_SUPPORT_HSCL                   (0)
#define BVDC_P_SUPPORT_HSCL_VER               (4)
#define BVDC_P_SUPPORT_HSCL_MAD_HARD_WIRED    (1)
#define BVDC_P_SUPPORT_FMISC_PFRI             (0)
#define BVDC_P_SUPPORT_FMISC_MEMC             (0)
#define BVDC_P_SUPPORT_DMISC                  (1)
#define BVDC_P_SUPPORT_BVN_444                (0)
#define BVDC_P_SUPPORT_BVN_10BIT_444          (0)
#define BVDC_P_SUPPORT_CMP_TEN_TAP_422_TO_444 (1)
#define BVDC_P_SUPPORT_MAD_SRC_1080I          (0)
#define BVDC_P_SUPPORT_ANR_VER                (0)
#define BVDC_P_SUPPORT_ANR                    (0)
#define BVDC_P_SUPPORT_MAD                    (1)
#define BVDC_P_SUPPORT_ANR_MAD_MEM_SAVING_MODE  (0)
#define BVDC_P_SUPPORT_FREE_CHANNEL           (3)
#define BVDC_P_SUPPORT_DRAIN_F                (1)
#define BVDC_P_SUPPORT_DRAIN_B                (1)
#define BVDC_P_SUPPORT_DRAIN_VER              (0)
#define BVDC_P_SUPPORT_DITHER                 (0)
#define BVDC_P_SUPPORT_MCVP                   (0)
#define BVDC_P_SUPPORT_MCVP_VER               (0)
#define BVDC_P_SUPPORT_MCDI_VER               (0) /* MCDI HW version */
#define BVDC_P_SUPPORT_MADR                   (0) /* Number of MAD-R HW */
#define BVDC_P_SUPPORT_MADR_VER               (0) /* MAD-R HW version */
#define BVDC_P_SUPPORT_MANR                   (0) /* Number of ANR HW in MCVP */
#define BVDC_P_SUPPORT_MANR_VER               (0) /* ANR in MCVP version */
#define BVDC_P_SUPPORT_OSCL                   (1)
#define BVDC_P_SUPPORT_VIDEO_TESTFEATURE1_VER (0)

/* CMP */
#define BVDC_P_CMP_0_MAX_VIDEO_WINDOW_COUNT   (1)
#define BVDC_P_CMP_0_MAX_GFX_WINDOW_COUNT     (1)
#define BVDC_P_CMP_1_MAX_VIDEO_WINDOW_COUNT   (1)
#define BVDC_P_CMP_1_MAX_GFX_WINDOW_COUNT     (1)
#define BVDC_P_CMP_2_MAX_VIDEO_WINDOW_COUNT   (0)
#define BVDC_P_CMP_2_MAX_GFX_WINDOW_COUNT     (0)
#define BVDC_P_CMP_3_MAX_VIDEO_WINDOW_COUNT   (0) /* Number of CMP_3_Vx */
#define BVDC_P_CMP_3_MAX_GFX_WINDOW_COUNT     (0) /* Number of CMP_3_Gx */
#define BVDC_P_CMP_4_MAX_VIDEO_WINDOW_COUNT   (0) /* Number of CMP_4_Vx */
#define BVDC_P_CMP_4_MAX_GFX_WINDOW_COUNT     (0) /* Number of CMP_4_Gx */
#define BVDC_P_CMP_5_MAX_VIDEO_WINDOW_COUNT   (0) /* Number of CMP_5_Vx */
#define BVDC_P_CMP_5_MAX_GFX_WINDOW_COUNT     (0) /* Number of CMP_5_Gx */
#define BVDC_P_CMP_6_MAX_VIDEO_WINDOW_COUNT   (0) /* Number of CMP_6_Vx */
#define BVDC_P_CMP_6_MAX_GFX_WINDOW_COUNT     (0) /* Number of CMP_6_Gx */
#define BVDC_P_CMP_0_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_0_NON_LINEAR_CSC_V1        (0)
#define BVDC_P_CMP_1_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_1_NON_LINEAR_CSC_V1        (0)
#define BVDC_P_CMP_2_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_3_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_4_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_5_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_6_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_0_CLEAR_RECT_V0            (0)
#define BVDC_P_CMP_0_CLEAR_RECT_V1            (0)
#define BVDC_P_CMP_1_CLEAR_RECT_V0            (0)
#define BVDC_P_CMP_1_CLEAR_RECT_V1            (0)
#define BVDC_P_CMP_2_CLEAR_RECT_V0            (0)
#define BVDC_P_CMP_3_CLEAR_RECT_V0            (0)
#define BVDC_P_CMP_4_CLEAR_RECT_V0            (0)
#define BVDC_P_CMP_5_CLEAR_RECT_V0            (0)
#define BVDC_P_CMP_6_CLEAR_RECT_V0            (0)
#define BVDC_P_SUPPORT_WIN_CONST_COLOR        (1)
#define BVDC_P_SUPPORT_CM3D                   (0)
#define BVDC_P_SUPPORT_CMP_DERINGING          (0)
#define BVDC_P_SUPPORT_CMP_MOSAIC_CSC         (0)

/* csc */
#define BVDC_P_SUPPORT_CMP_DEMO_MODE          (2)
#define BVDC_P_SUPPORT_CSC_MAT_COEF_VER       (3)
#define BVDC_P_SUPPORT_COLOR_CLIP             (1)

/* display */
#define BVDC_P_SUPPORT_EARLY_TRIGGER          (0)
#define BVDC_P_SUPPORT_VBI_ENC_656            (0)
#define BVDC_P_SUPPORT_NEW_SW_INIT            (0)
#define BVDC_P_SUPPORT_4_DACS_PRIM_VEC        (0)
#define BVDC_P_SUPPORT_VEC_VF_VER             (0)
#define BVDC_P_SUPPORT_AMOL_VBI_ENCODE        (1)
#define BVDC_P_SUPPORT_HDMI_OUT               (1)
#define BVDC_P_SUPPORT_DVI_OUT                (1)
#define BVDC_P_SUPPORT_HDMI_RM_VER            (4)
#define BVDC_P_SUPPORT_ITU656_OUT             (0)
#define BVDC_P_SUPPORT_RFM_OUTPUT             (0)
#define BVDC_P_SUPPORT_EXT_SYNC_OUT_MUX       (0)
#define BVDC_P_SUPPORT_EXT_SYNC_PCL_0         (0)
#define BVDC_P_SUPPORT_TDAC_VER               (8)
#define BVDC_P_SUPPORT_QDAC_VER               (0)
#define BVDC_P_SUPPORT_1080p_60HZ             (1)
#define BVDC_P_SUPPORT_MUD                    (0)
#define BVDC_P_MAX_DACS                       (4)
#define BVDC_P_VEC_HAS_2_DIFFERENT_DVF        (0)
#define BVDC_P_VEC_SUPPORT_DVI_COLOR_CNVT     (0)
#define BVDC_P_ORTHOGONAL_VEC                 (1)
#define BVDC_P_ORTHOGONAL_VEC_VER             (0)
#define BVDC_P_SUPPORT_HD_DAC                 (1)
#define BVDC_P_SUPPORT_IT_VER                 (2) /* support modulo trigger count */
#define BVDC_P_SUPPORT_STG_VER                (0)

#define BVDC_P_NUM_SHARED_656                 (0)
#define BVDC_P_NUM_SHARED_DVI                 (1)
#define BVDC_P_NUM_SHARED_STG                 (0)
#define BVDC_P_NUM_SHARED_RF                  (0)
#define BVDC_P_NUM_SHARED_IT                  (2)
#define BVDC_P_NUM_SHARED_VF                  (2)
#define BVDC_P_NUM_SHARED_SDSRC               (1)
#define BVDC_P_NUM_SHARED_HDSRC               (1)
#define BVDC_P_NUM_SHARED_SM                  (1)
/* Note, here SHARED_SECAM/HDSECAM means the secam cross bar.
 * Some secam cross bar may not have a secam module associated with it.
 */
#define BVDC_P_NUM_SHARED_SECAM               (0)
#define BVDC_P_NUM_SHARED_SECAM_HD            (2)
#define BVDC_P_NUM_SHARED_DAC                 BVDC_P_MAX_DACS
#define BVDC_P_SUPPORT_CLOCK_GATING           (0)
#define BVDC_P_SUPPORT_3D_VIDEO               (0)
#define BVDC_P_SUPPORT_3D_INDEP_SRC_CLIP      (0)
#define BVDC_P_SUPPORT_STG                    (0)

#elif (BCHP_CHIP==7408)

#if (BCHP_VER >= BCHP_VER_B0)
#define BVDC_P_SUPPORT_CLOCK_GATING           (1)
#define BVDC_P_SUPPORT_CMP_MOSAIC_CSC         (5)
#define BVDC_P_CMP_0_CLEAR_RECT_V0           (16)
#define BVDC_P_CMP_1_CLEAR_RECT_V0           (16)
#else
#define BVDC_P_SUPPORT_CLOCK_GATING           (0)
#define BVDC_P_SUPPORT_CMP_MOSAIC_CSC         (0)
#define BVDC_P_CMP_0_CLEAR_RECT_V0            (0)
#define BVDC_P_CMP_1_CLEAR_RECT_V0            (0)
#endif

#define BVDC_P_SUPPORT_FGT                    (0)
#define BVDC_P_SUPPORT_FGT_VER                (0)

#define BVDC_P_SUPPORT_656_MASTER_MODE        (0)
#define BVDC_P_SUPPORT_DVO_MASTER_MODE        (0)
#define BVDC_P_SUPPORT_VEC_MUXING             (1)
#define BVDC_P_SUPPORT_LOOP_BACK              (2) /* Number of VNET_B_LOOP_BACK_x_SRC */
#define BVDC_P_SUPPORT_DNR                    (1) /* Number of DNR_x */
#define BVDC_P_SUPPORT_DNR_VER                (6)
#define BVDC_P_SUPPORT_MODULAR_VEC            (2)
#define BVDC_P_SUPPORT_NEW_656_IN_VER         (0)
#define BVDC_P_SUPPORT_VEC_GRPD               (0)
#define BVDC_P_SUPPORT_VEC_SECAM              (0) /* SECAM_x present */
#define BVDC_P_SUPPORT_CMP_CRC                (1)
#define BVDC_P_SUPPORT_CMP_LUMA_AVG_VER       (0)

/* source */
#define BVDC_P_SUPPORT_GFD                    (2) /* Number of GFD_x */
#define BVDC_P_SUPPORT_GFD_VER                (3)
#define BVDC_P_SUPPORT_GFD1_VER               (3)
#define BVDC_P_SUPPORT_VFD                    (2) /* Number of VFD_x */
#define BVDC_P_SUPPORT_MFD                    (1) /* Number of MFD_x */
#define BVDC_P_SUPPORT_MFD_VER                (9)
#define BVDC_P_SUPPORT_VDEC                   (0) /* Number of VDEC_x */
#define BVDC_P_SUPPORT_VDEC_VER               (0)
#define BVDC_P_SUPPORT_VDEC_PASSTHROUGH       (0)
#define BVDC_P_SUPPORT_HDDVI                  (0) /* Number of HD_DVI_x */
#define BVDC_P_SUPPORT_HDDVI_VER              (0)
#define BVDC_P_SUPPORT_DOWNSAMPLE             (0)
#define BVDC_P_SUPPORT_VCR_MODE               (0) /* Detect VCR mode shutoff 3:2 pulldown in MAD */

/* BVN */
#define BVDC_P_SUPPORT_PEP                    (0) /* Number of CMP_x_PEP */
#define BVDC_P_SUPPORT_PEP_VER                (0)
#define BVDC_P_SUPPORT_TAB                    (0) /* Number of TAB_x  */
#define BVDC_P_SUPPORT_TNT                    (1)
#define BVDC_P_SUPPORT_TNT_VER                (6)
#define BVDC_P_SUPPORT_MASK_DITHER            (1) /* Number of MASK_x */
#define BVDC_P_SUPPORT_HIST                   (0) /* Number of HISTO_x */
#define BVDC_P_SUPPORT_HIST_VER               (0)
#define BVDC_P_SUPPORT_MAD                    (1)
#define BVDC_P_SUPPORT_MAD_VER                (7)
#define BVDC_P_SUPPORT_BOX_DETECT             (1)
#define BVDC_P_SUPPORT_BOX_DETECT_VER         (1)
#define BVDC_P_SUPPORT_CAP                    (2)
#define BVDC_P_SUPPORT_CAP_VER                (2)
#define BVDC_P_SUPPORT_SCL                    (2)
#define BVDC_P_SUPPORT_SCL_VER                (6)
#define BVDC_P_SUPPORT_HSCL                   (0)
#define BVDC_P_SUPPORT_HSCL_VER               (4)
#define BVDC_P_SUPPORT_HSCL_MAD_HARD_WIRED    (1)
#define BVDC_P_SUPPORT_FMISC_PFRI             (0)
#define BVDC_P_SUPPORT_FMISC_MEMC             (0)
#define BVDC_P_SUPPORT_DMISC                  (1)
#define BVDC_P_SUPPORT_BVN_444                (0)
#define BVDC_P_SUPPORT_BVN_10BIT_444          (0)
#define BVDC_P_SUPPORT_CMP_TEN_TAP_422_TO_444 (1)
#define BVDC_P_SUPPORT_MAD_SRC_1080I          (0)
#define BVDC_P_SUPPORT_ANR                    (0)
#define BVDC_P_SUPPORT_ANR_VER                (0)
#define BVDC_P_SUPPORT_ANR_MAD_MEM_SAVING_MODE (1)
#define BVDC_P_SUPPORT_FREE_CHANNEL           (3) /* Number of VNET_F_FCH_x_SRC */
#define BVDC_P_SUPPORT_DRAIN_F                (1) /* Number of VNET_F_DRAIN_x_SRC */
#define BVDC_P_SUPPORT_DRAIN_B                (1) /* Number of VNET_B_DRAIN_x_SRC */
#define BVDC_P_SUPPORT_DRAIN_VER              (0)
#define BVDC_P_SUPPORT_DITHER                 (0)
#define BVDC_P_SUPPORT_MCVP                   (0)
#define BVDC_P_SUPPORT_MCVP_VER               (0)
#define BVDC_P_SUPPORT_MCDI_VER               (0) /* MCDI HW version */
#define BVDC_P_SUPPORT_MADR                   (0) /* Number of MAD-R HW */
#define BVDC_P_SUPPORT_MADR_VER               (0) /* MAD-R HW version */
#define BVDC_P_SUPPORT_MANR                   (0) /* Number of ANR HW in MCVP */
#define BVDC_P_SUPPORT_MANR_VER               (0) /* ANR in MCVP version */
#define BVDC_P_SUPPORT_OSCL                   (0)
#define BVDC_P_SUPPORT_VIDEO_TESTFEATURE1_VER (0)

/* CMP */
#define BVDC_P_CMP_0_MAX_VIDEO_WINDOW_COUNT   (1) /* Number of CMP_0_Vx */
#define BVDC_P_CMP_0_MAX_GFX_WINDOW_COUNT     (1) /* Number of CMP_0_Gx */
#define BVDC_P_CMP_1_MAX_VIDEO_WINDOW_COUNT   (1) /* Number of CMP_1_Vx */
#define BVDC_P_CMP_1_MAX_GFX_WINDOW_COUNT     (1) /* Number of CMP_1_Gx */
#define BVDC_P_CMP_2_MAX_VIDEO_WINDOW_COUNT   (0) /* Number of CMP_2_Vx */
#define BVDC_P_CMP_2_MAX_GFX_WINDOW_COUNT     (0) /* Number of CMP_2_Gx */
#define BVDC_P_CMP_3_MAX_VIDEO_WINDOW_COUNT   (0) /* Number of CMP_3_Vx */
#define BVDC_P_CMP_3_MAX_GFX_WINDOW_COUNT     (0) /* Number of CMP_3_Gx */
#define BVDC_P_CMP_4_MAX_VIDEO_WINDOW_COUNT   (0) /* Number of CMP_4_Vx */
#define BVDC_P_CMP_4_MAX_GFX_WINDOW_COUNT     (0) /* Number of CMP_4_Gx */
#define BVDC_P_CMP_5_MAX_VIDEO_WINDOW_COUNT   (0) /* Number of CMP_5_Vx */
#define BVDC_P_CMP_5_MAX_GFX_WINDOW_COUNT     (0) /* Number of CMP_5_Gx */
#define BVDC_P_CMP_6_MAX_VIDEO_WINDOW_COUNT   (0) /* Number of CMP_6_Vx */
#define BVDC_P_CMP_6_MAX_GFX_WINDOW_COUNT     (0) /* Number of CMP_6_Gx */
#define BVDC_P_CMP_0_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_0_NON_LINEAR_CSC_V1        (0)
#define BVDC_P_CMP_1_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_1_NON_LINEAR_CSC_V1        (0)
#define BVDC_P_CMP_2_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_3_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_4_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_5_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_6_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_0_CLEAR_RECT_V1            (0)
#define BVDC_P_CMP_1_CLEAR_RECT_V1            (0)
#define BVDC_P_CMP_2_CLEAR_RECT_V0            (0)
#define BVDC_P_CMP_3_CLEAR_RECT_V0            (0)
#define BVDC_P_CMP_4_CLEAR_RECT_V0            (0)
#define BVDC_P_CMP_5_CLEAR_RECT_V0            (0)
#define BVDC_P_CMP_6_CLEAR_RECT_V0            (0)
#define BVDC_P_SUPPORT_WIN_CONST_COLOR        (0)
#define BVDC_P_SUPPORT_CM3D                   (0) /* Number of VIDID_x */
#define BVDC_P_SUPPORT_CMP_DERINGING          (1)

/* csc */
#define BVDC_P_SUPPORT_CMP_DEMO_MODE          (2)
#define BVDC_P_SUPPORT_CSC_MAT_COEF_VER       (3)
#define BVDC_P_SUPPORT_COLOR_CLIP             (0)

/* display */
#define BVDC_P_SUPPORT_EARLY_TRIGGER          (0)
#define BVDC_P_SUPPORT_VBI_ENC_656            (0)
#define BVDC_P_SUPPORT_NEW_SW_INIT            (0)
#define BVDC_P_SUPPORT_4_DACS_PRIM_VEC        (0)
#define BVDC_P_SUPPORT_VEC_VF_VER             (0)
#define BVDC_P_SUPPORT_AMOL_VBI_ENCODE        (1)
#define BVDC_P_SUPPORT_HDMI_OUT               (1)
#define BVDC_P_SUPPORT_DVI_OUT                (1)

#define BVDC_P_SUPPORT_HDMI_RM_VER            (4)
#define BVDC_P_SUPPORT_ITU656_OUT             (0)
#define BVDC_P_SUPPORT_RFM_OUTPUT             (0)
#define BVDC_P_SUPPORT_EXT_SYNC_OUT_MUX       (0)
#define BVDC_P_SUPPORT_EXT_SYNC_PCL_0         (0)
#define BVDC_P_SUPPORT_TDAC_VER               (8)
#define BVDC_P_SUPPORT_QDAC_VER               (0)
#define BVDC_P_SUPPORT_1080p_60HZ             (1) /* Orthogonal VEC and DTV vec */
#define BVDC_P_SUPPORT_MUD                    (0)
#define BVDC_P_MAX_DACS                       (4) /* Number of MISC_DAC_x_CTRL */
#define BVDC_P_VEC_HAS_2_DIFFERENT_DVF        (0)
#define BVDC_P_VEC_SUPPORT_DVI_COLOR_CNVT     (0)
#define BVDC_P_ORTHOGONAL_VEC                 (1) /* 7420-based VEC */
#define BVDC_P_ORTHOGONAL_VEC_VER             (0)
#define BVDC_P_SUPPORT_HD_DAC                 (1)
#define BVDC_P_SUPPORT_IT_VER                 (2) /* support modulo trigger count */
#define BVDC_P_SUPPORT_STG_VER                (0)

#define BVDC_P_NUM_SHARED_656                 (0)
#define BVDC_P_NUM_SHARED_DVI                 (1)
#define BVDC_P_NUM_SHARED_STG                 (0)
#define BVDC_P_NUM_SHARED_RF                  (0)
#define BVDC_P_NUM_SHARED_IT                  (2)
#define BVDC_P_NUM_SHARED_VF                  (2)
#define BVDC_P_NUM_SHARED_SDSRC               (1)
#define BVDC_P_NUM_SHARED_HDSRC               (1)
#define BVDC_P_NUM_SHARED_SM                  (1)
/* Note, here SHARED_SECAM/HDSECAM means the secam cross bar.
 * Some secam cross bar may not have a secam module associated with it.
 */
#define BVDC_P_NUM_SHARED_SECAM               (1) /* Number of VEC_CFG_SECAM_>1_SOURCE */
#define BVDC_P_NUM_SHARED_SECAM_HD            (1) /* Number of VEC_CFG_SECAM_0_SOURCE */
#define BVDC_P_NUM_SHARED_DAC                 BVDC_P_MAX_DACS
#define BVDC_P_SUPPORT_3D_VIDEO               (0)
#define BVDC_P_SUPPORT_3D_INDEP_SRC_CLIP      (0)
#define BVDC_P_SUPPORT_STG                    (0) /* STG HW */

#elif (BCHP_CHIP==7358) || (BCHP_CHIP==7552)

#define BVDC_P_SUPPORT_656_MASTER_MODE        (0)
#define BVDC_P_SUPPORT_DVO_MASTER_MODE        (1)
#define BVDC_P_SUPPORT_VEC_MUXING             (1)
#define BVDC_P_SUPPORT_TDAC_VER               (9) /* TODO */
#define BVDC_P_SUPPORT_QDAC_VER               (0) /* TODO */
#define BVDC_P_SUPPORT_LOOP_BACK              (2)
#define BVDC_P_SUPPORT_DNR                    (1)
#define BVDC_P_SUPPORT_DNR_VER                (7)
#define BVDC_P_SUPPORT_MODULAR_VEC            (2)
#define BVDC_P_SUPPORT_NEW_656_IN_VER         (0)
#define BVDC_P_SUPPORT_VEC_GRPD               (0)
#define BVDC_P_SUPPORT_VEC_SECAM              (1)
#define BVDC_P_SUPPORT_CAP_VER                (4)
#define BVDC_P_SUPPORT_CMP_CRC                (1)
#define BVDC_P_SUPPORT_CMP_LUMA_AVG_VER       (0)

/* source */
#define BVDC_P_SUPPORT_GFD                    (2)
#define BVDC_P_SUPPORT_GFD_VER                (4)
#define BVDC_P_SUPPORT_GFD1_VER               (4)
#define BVDC_P_SUPPORT_MFD                    (1)
#if (BCHP_VER >= BCHP_VER_B0)
#define BVDC_P_SUPPORT_MFD_VER                (12)/* MFD HW version */
#else
#define BVDC_P_SUPPORT_MFD_VER                (11)
#endif
#if (BCHP_CHIP==7358)
#if (BCHP_VER >= BCHP_VER_A1)
#define BVDC_P_SUPPORT_MADR_VER               (4) /* MAD-R HW version */
#define BVDC_P_SUPPORT_CLOCK_GATING           (1) /* Clock gating for power saving */
#else
#define BVDC_P_SUPPORT_MADR_VER               (2) /* MAD-R HW version */
#define BVDC_P_SUPPORT_CLOCK_GATING           (0) /* Clock gating for power saving */
#endif
#define BVDC_P_SUPPORT_IT_VER                 (2)
#else /* (BCHP_CHIP==7358) */
#if (BCHP_VER >= BCHP_VER_B0)
#define BVDC_P_SUPPORT_MADR_VER               (5) /* MAD-R HW version */
#define BVDC_P_SUPPORT_CLOCK_GATING           (1) /* Clock gating for power saving */
#define BVDC_P_SUPPORT_IT_VER                 (3)
#else
#define BVDC_P_SUPPORT_MADR_VER               (2) /* MAD-R HW version */
#define BVDC_P_SUPPORT_CLOCK_GATING           (0) /* Clock gating for power saving */
#define BVDC_P_SUPPORT_IT_VER                 (2)
#endif
#endif
#define BVDC_P_SUPPORT_VDEC                   (0)
#define BVDC_P_SUPPORT_VDEC_VER               (0)
#define BVDC_P_SUPPORT_VDEC_PASSTHROUGH       (0)
#define BVDC_P_SUPPORT_HDDVI                  (0)
#define BVDC_P_SUPPORT_HDDVI_VER              (0)
#define BVDC_P_SUPPORT_DOWNSAMPLE             (0)
#define BVDC_P_SUPPORT_VCR_MODE               (0)

/* BVN */
#define BVDC_P_SUPPORT_PEP                    (1)
#define BVDC_P_SUPPORT_PEP_VER                (1)
#define BVDC_P_SUPPORT_HIST                   (1)
#define BVDC_P_SUPPORT_HIST_VER               (1)
#define BVDC_P_SUPPORT_TAB                    (0)
#define BVDC_P_SUPPORT_TNT                    (1)
#define BVDC_P_SUPPORT_TNT_VER                (6)
#define BVDC_P_SUPPORT_MASK_DITHER            (1)
#define BVDC_P_SUPPORT_FGT                    (0)
#define BVDC_P_SUPPORT_FGT_VER                (0)
#define BVDC_P_SUPPORT_MAD                    (0)
#define BVDC_P_SUPPORT_MAD_VER                (0)
#define BVDC_P_SUPPORT_BOX_DETECT             (1)
#define BVDC_P_SUPPORT_BOX_DETECT_VER         (1)
#define BVDC_P_SUPPORT_CAP                    (2)
#define BVDC_P_SUPPORT_VFD                    (2)
#define BVDC_P_SUPPORT_SCL                    (2)
#define BVDC_P_SUPPORT_SCL_VER                (7)
#define BVDC_P_SUPPORT_HSCL                   (0)
#define BVDC_P_SUPPORT_HSCL_VER               (5)
#define BVDC_P_SUPPORT_HSCL_MAD_HARD_WIRED    (1)
#define BVDC_P_SUPPORT_FMISC_PFRI             (0)
#define BVDC_P_SUPPORT_FMISC_MEMC             (0) /* TODO */
#define BVDC_P_SUPPORT_DMISC                  (0)
#define BVDC_P_SUPPORT_BVN_444                (0)
#define BVDC_P_SUPPORT_BVN_10BIT_444          (0)
#define BVDC_P_SUPPORT_CMP_TEN_TAP_422_TO_444 (1)
#define BVDC_P_SUPPORT_MAD_SRC_1080I          (1)
#define BVDC_P_SUPPORT_ANR_VER                (0)
#define BVDC_P_SUPPORT_ANR                    (0)
#define BVDC_P_SUPPORT_ANR_MAD_MEM_SAVING_MODE  (1)
#define BVDC_P_SUPPORT_FREE_CHANNEL           (3)
#define BVDC_P_SUPPORT_DRAIN_F                (1)
#define BVDC_P_SUPPORT_DRAIN_B                (1)
#define BVDC_P_SUPPORT_DRAIN_VER              (3) /* DRAIN HW version */
#define BVDC_P_SUPPORT_DITHER                 (0)
#define BVDC_P_SUPPORT_VEC_DITHER             (0)

#define BVDC_P_SUPPORT_MCVP                   (1)
#define BVDC_P_SUPPORT_MCVP_VER               (3) /* TODO */
#define BVDC_P_SUPPORT_MCDI_VER               (0) /* TODO: MCDI HW version */
#define BVDC_P_SUPPORT_MADR                   (1) /* TODO: Number of MAD-R HW */
#define BVDC_P_SUPPORT_MANR                   (0) /* TODO: Number of ANR HW in MCVP */
#define BVDC_P_SUPPORT_MANR_VER               (0) /* TODO: ANR in MCVP version */
#define BVDC_P_SUPPORT_OSCL                   (0)
#define BVDC_P_SUPPORT_VIDEO_TESTFEATURE1_VER (4) /* Testfeature HW version */

/* CMP */
#define BVDC_P_CMP_0_MAX_VIDEO_WINDOW_COUNT   (1)
#define BVDC_P_CMP_0_MAX_GFX_WINDOW_COUNT     (1)
#define BVDC_P_CMP_1_MAX_VIDEO_WINDOW_COUNT   (1)
#define BVDC_P_CMP_1_MAX_GFX_WINDOW_COUNT     (1)
#define BVDC_P_CMP_2_MAX_VIDEO_WINDOW_COUNT   (0)
#define BVDC_P_CMP_2_MAX_GFX_WINDOW_COUNT     (0)
#define BVDC_P_CMP_3_MAX_VIDEO_WINDOW_COUNT   (0) /* Number of CMP_3_Vx */
#define BVDC_P_CMP_3_MAX_GFX_WINDOW_COUNT     (0) /* Number of CMP_3_Gx */
#define BVDC_P_CMP_4_MAX_VIDEO_WINDOW_COUNT   (0) /* Number of CMP_4_Vx */
#define BVDC_P_CMP_4_MAX_GFX_WINDOW_COUNT     (0) /* Number of CMP_4_Gx */
#define BVDC_P_CMP_5_MAX_VIDEO_WINDOW_COUNT   (0) /* Number of CMP_5_Vx */
#define BVDC_P_CMP_5_MAX_GFX_WINDOW_COUNT     (0) /* Number of CMP_5_Gx */
#define BVDC_P_CMP_6_MAX_VIDEO_WINDOW_COUNT   (0) /* Number of CMP_6_Vx */
#define BVDC_P_CMP_6_MAX_GFX_WINDOW_COUNT     (0) /* Number of CMP_6_Gx */
#define BVDC_P_CMP_0_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_0_NON_LINEAR_CSC_V1        (0)
#define BVDC_P_CMP_1_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_1_NON_LINEAR_CSC_V1        (0)
#define BVDC_P_CMP_2_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_3_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_4_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_5_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_6_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_0_CLEAR_RECT_V0            (16)
#define BVDC_P_CMP_0_CLEAR_RECT_V1            (0)
#define BVDC_P_CMP_1_CLEAR_RECT_V0            (16)
#define BVDC_P_CMP_1_CLEAR_RECT_V1            (0)
#define BVDC_P_CMP_2_CLEAR_RECT_V0            (0)
#define BVDC_P_CMP_3_CLEAR_RECT_V0            (0)
#define BVDC_P_CMP_4_CLEAR_RECT_V0            (0)
#define BVDC_P_CMP_5_CLEAR_RECT_V0            (0)
#define BVDC_P_CMP_6_CLEAR_RECT_V0            (0)
#define BVDC_P_SUPPORT_WIN_CONST_COLOR        (1)
#define BVDC_P_SUPPORT_CM3D                   (0)
#define BVDC_P_SUPPORT_CMP_DERINGING          (1)
#define BVDC_P_SUPPORT_CMP_MOSAIC_CSC         (5)

/* csc */
#define BVDC_P_SUPPORT_CMP_DEMO_MODE          (2)
#define BVDC_P_SUPPORT_CSC_MAT_COEF_VER       (3)
#define BVDC_P_SUPPORT_COLOR_CLIP             (1)

/* display */
#define BVDC_P_SUPPORT_SEAMLESS_ATTACH        (0)
#define BVDC_P_SUPPORT_EARLY_TRIGGER          (0)
#define BVDC_P_SUPPORT_VBI_ENC_656            (0)
#define BVDC_P_SUPPORT_NEW_SW_INIT            (1)
#define BVDC_P_SUPPORT_4_DACS_PRIM_VEC        (0)
#define BVDC_P_SUPPORT_VEC_VF_VER             (2)
#define BVDC_P_SUPPORT_AMOL_VBI_ENCODE        (1)
#define BVDC_P_SUPPORT_HDMI_OUT               (1)
#define BVDC_P_SUPPORT_DVI_OUT                (1)
#define BVDC_P_SUPPORT_HDMI_RM_VER            (5)
#define BVDC_P_SUPPORT_ITU656_OUT             (0)
#define BVDC_P_SUPPORT_RFM_OUTPUT             (1)
#define BVDC_P_SUPPORT_EXT_SYNC_OUT_MUX       (0)
#define BVDC_P_SUPPORT_EXT_SYNC_PCL_0         (0)

#define BVDC_P_SUPPORT_1080p_60HZ             (1)
#define BVDC_P_SUPPORT_MUD                    (0)
#define BVDC_P_MAX_DACS                       (4)
#define BVDC_P_VEC_HAS_2_DIFFERENT_DVF        (0)
#define BVDC_P_VEC_SUPPORT_DVI_COLOR_CNVT     (0)
#define BVDC_P_ORTHOGONAL_VEC                 (1)
#define BVDC_P_ORTHOGONAL_VEC_VER             (0)
#define BVDC_P_SUPPORT_HD_DAC                 (1)
#define BVDC_P_SUPPORT_STG_VER                (0)

#define BVDC_P_NUM_SHARED_656                 (1)
#define BVDC_P_NUM_SHARED_DVI                 (1)
#define BVDC_P_NUM_SHARED_STG                 (0)
#define BVDC_P_NUM_SHARED_RF                  (1)
#define BVDC_P_NUM_SHARED_IT                  (2)
#define BVDC_P_NUM_SHARED_VF                  (2)
#define BVDC_P_NUM_SHARED_SDSRC               (1)
#define BVDC_P_NUM_SHARED_HDSRC               (1)
#define BVDC_P_NUM_SHARED_SM                  (1)
/* Note, here SHARED_SECAM/HDSECAM means the secam cross bar.
 * Some secam cross bar may not have a secam module associated with it.
 */
#define BVDC_P_NUM_SHARED_SECAM               (1)
#define BVDC_P_NUM_SHARED_SECAM_HD            (2)
#define BVDC_P_NUM_SHARED_DAC                 BVDC_P_MAX_DACS
#define BVDC_P_SUPPORT_3D_VIDEO               (1)
#define BVDC_P_SUPPORT_3D_INDEP_SRC_CLIP      (0)
#define BVDC_P_SUPPORT_STG                    (0)

#elif (BCHP_CHIP==7422) || (BCHP_CHIP==7425)

#define BVDC_P_SUPPORT_656_MASTER_MODE        (0)
#define BVDC_P_SUPPORT_DVO_MASTER_MODE        (1)
#define BVDC_P_SUPPORT_VEC_MUXING             (1)
#define BVDC_P_SUPPORT_QDAC_VER               (0) /* TODO */
#define BVDC_P_SUPPORT_DNR_VER                (7)
#define BVDC_P_SUPPORT_MODULAR_VEC            (2)
#define BVDC_P_SUPPORT_NEW_656_IN_VER         (0)
#define BVDC_P_SUPPORT_VEC_GRPD               (0)
#define BVDC_P_SUPPORT_VEC_SECAM              (1)
#define BVDC_P_SUPPORT_CMP_CRC                (1)

/* source */
#define BVDC_P_SUPPORT_GFD                    (4)
#define BVDC_P_SUPPORT_GFD_VER                (4)
#define BVDC_P_SUPPORT_GFD1_VER               (4)
#define BVDC_P_SUPPORT_MFD                    (2)

#if (BCHP_VER >= BCHP_VER_B0)
#define BVDC_P_SUPPORT_MFD_VER                (13)/* MFD HW version */
#define BVDC_P_SUPPORT_TDAC_VER               (11)/* TDAC/QDAC HW version */
#define BVDC_P_SUPPORT_SCL_VER                (8) /* SCL HW version */
#define BVDC_P_SUPPORT_HSCL_VER               (6) /* HSCL HW version */
#define BVDC_P_SUPPORT_DNR                    (3) /* Number of DNR_x core */
#define BVDC_P_SUPPORT_MCVP                   (3) /* Number of MCVP HW */
#define BVDC_P_SUPPORT_MADR                   (2) /* Number of MAD-R HW */
#define BVDC_P_SUPPORT_BOX_DETECT             (4) /* Number LBOX HW detect */
#define BVDC_P_SUPPORT_LOOP_BACK              (6) /* Number of VNET_B_LOOP_BACK_x_SRC */
#define BVDC_P_SUPPORT_HDDVI_VER              (8)
#define BVDC_P_SUPPORT_MADR_VER               (5) /* MAD-R HW version */
#define BVDC_P_SUPPORT_CAP_VER                (5)
#define BVDC_P_SUPPORT_CLOCK_GATING           (1)
#else
#define BVDC_P_SUPPORT_MFD_VER                (10)/* MFD HW version */
#define BVDC_P_SUPPORT_TDAC_VER               (9) /* TDAC/QDAC HW version */
#define BVDC_P_SUPPORT_SCL_VER                (7) /* SCL HW version */
#define BVDC_P_SUPPORT_HSCL_VER               (5) /* HSCL HW version */
#define BVDC_P_SUPPORT_DNR                    (2) /* Number of DNR_x core */
#define BVDC_P_SUPPORT_MCVP                   (2) /* Number of MCVP HW */
#define BVDC_P_SUPPORT_MADR                   (1) /* Number of MAD-R HW */
#define BVDC_P_SUPPORT_BOX_DETECT             (3) /* Number LBOX HW detect */
#define BVDC_P_SUPPORT_LOOP_BACK              (4) /* Number of VNET_B_LOOP_BACK_x_SRC */
#define BVDC_P_SUPPORT_HDDVI_VER              (7) /* HDDVI HW version */
#define BVDC_P_SUPPORT_MADR_VER               (3) /* MAD-R HW version */
#define BVDC_P_SUPPORT_CAP_VER                (4)
#define BVDC_P_SUPPORT_CLOCK_GATING           (0)
#endif
#define BVDC_P_SUPPORT_BOX_DETECT_VER         (1)
#define BVDC_P_SUPPORT_VDEC                   (0)
#define BVDC_P_SUPPORT_VDEC_VER               (0)
#define BVDC_P_SUPPORT_VDEC_PASSTHROUGH       (0)
#define BVDC_P_SUPPORT_HDDVI                  (1)
#define BVDC_P_SUPPORT_DOWNSAMPLE             (0)
#define BVDC_P_SUPPORT_VCR_MODE               (0)

/* BVN */
#define BVDC_P_SUPPORT_PEP                    (1)
#define BVDC_P_SUPPORT_PEP_VER                (1)
#define BVDC_P_SUPPORT_HIST                   (1)
#define BVDC_P_SUPPORT_HIST_VER               (1)
#define BVDC_P_SUPPORT_TAB                    (0)
#define BVDC_P_SUPPORT_TNT                    (1)
#define BVDC_P_SUPPORT_TNT_VER                (6)
#define BVDC_P_SUPPORT_MASK_DITHER            (1)
#define BVDC_P_SUPPORT_FGT                    (0)
#define BVDC_P_SUPPORT_FGT_VER                (0)
#define BVDC_P_SUPPORT_MAD                    (0)
#define BVDC_P_SUPPORT_MAD_VER                (7)
#define BVDC_P_SUPPORT_CAP                    (5)
#define BVDC_P_SUPPORT_VFD                    (5)
#define BVDC_P_SUPPORT_SCL                    (5)
#define BVDC_P_SUPPORT_HSCL                   (0)
#define BVDC_P_SUPPORT_HSCL_MAD_HARD_WIRED    (1)
#define BVDC_P_SUPPORT_FMISC_PFRI             (0)
#define BVDC_P_SUPPORT_FMISC_MEMC             (0) /* TODO */
#define BVDC_P_SUPPORT_DMISC                  (0)
#define BVDC_P_SUPPORT_BVN_444                (0)
#define BVDC_P_SUPPORT_BVN_10BIT_444          (0)
#define BVDC_P_SUPPORT_CMP_TEN_TAP_422_TO_444 (1)
#define BVDC_P_SUPPORT_MAD_SRC_1080I          (1)
#define BVDC_P_SUPPORT_ANR                    (0)
#define BVDC_P_SUPPORT_ANR_VER                (0)
#define BVDC_P_SUPPORT_ANR_MAD_MEM_SAVING_MODE  (1)
#define BVDC_P_SUPPORT_FREE_CHANNEL           (7)
#define BVDC_P_SUPPORT_DRAIN_F                (3) /* Number of VNET_F_DRAIN_x_SRC */
#define BVDC_P_SUPPORT_DRAIN_B                (1) /* Number of VNET_B_DRAIN_x_SRC */
#define BVDC_P_SUPPORT_DRAIN_VER              (2) /* DRAIN HW version */
#define BVDC_P_SUPPORT_DITHER                 (0)
#define BVDC_P_SUPPORT_VEC_DITHER             (0)
#if (BCHP_CHIP==7422)
#define BVDC_P_SUPPORT_MCDI_VER               (2) /* MCDI HW version */
#define BVDC_P_SUPPORT_STG                    (0) /* Number of STG HW */
#define BVDC_P_NUM_SHARED_STG                 (0)
#define BVDC_P_ORTHOGONAL_VEC_VER             (0)
#define BVDC_P_SUPPORT_IT_VER                 (3)
#define BVDC_P_SUPPORT_3D_INDEP_SRC_CLIP      (0)
#else
#if (BCHP_VER >= BCHP_VER_B0)
#define BVDC_P_SUPPORT_STG                    (2) /* Number of STG HW */
#define BVDC_P_SUPPORT_STG_VER                (2) /* STG HW version */
#define BVDC_P_NUM_SHARED_STG                 (2)
#define BVDC_P_ORTHOGONAL_VEC_VER             (1)
#define BVDC_P_SUPPORT_IT_VER                 (3)
#define BVDC_P_SUPPORT_MCDI_VER               (4) /* MCDI HW version */
#define BVDC_P_SUPPORT_3D_INDEP_SRC_CLIP      (1)
#else
#define BVDC_P_SUPPORT_STG                    (1) /* Number of STG HW */
#define BVDC_P_SUPPORT_STG_VER                (1) /* STG HW version   */
#define BVDC_P_NUM_SHARED_STG                 (1)
#define BVDC_P_ORTHOGONAL_VEC_VER             (0)
#define BVDC_P_SUPPORT_IT_VER                 (2)
#define BVDC_P_SUPPORT_MCDI_VER               (3) /* MCDI HW version */
#define BVDC_P_SUPPORT_3D_INDEP_SRC_CLIP      (0)
#endif
#endif
#define BVDC_P_SUPPORT_MANR                   (1) /* Number of ANR HW in MCVP */
#define BVDC_P_SUPPORT_MANR_VER               (2) /* ANR in MCVP version */
#define BVDC_P_SUPPORT_MCVP_VER               (2) /* MCVP HW version */
#define BVDC_P_SUPPORT_OSCL                   (0)
#define BVDC_P_SUPPORT_VIDEO_TESTFEATURE1_VER (4) /* Testfeature HW version */

/* CMP */
#define BVDC_P_CMP_0_MAX_VIDEO_WINDOW_COUNT   (2)
#define BVDC_P_CMP_0_MAX_GFX_WINDOW_COUNT     (1)
#define BVDC_P_CMP_1_MAX_VIDEO_WINDOW_COUNT   (2)
#define BVDC_P_CMP_1_MAX_GFX_WINDOW_COUNT     (1)
#define BVDC_P_CMP_2_MAX_VIDEO_WINDOW_COUNT   (1)
#define BVDC_P_CMP_2_MAX_GFX_WINDOW_COUNT     (1)
#define BVDC_P_CMP_3_MAX_VIDEO_WINDOW_COUNT   (1) /* Number of CMP_3_Vx */
#define BVDC_P_CMP_3_MAX_GFX_WINDOW_COUNT     (1) /* Number of CMP_3_Gx */
#define BVDC_P_CMP_4_MAX_VIDEO_WINDOW_COUNT   (0) /* Number of CMP_4_Vx */
#define BVDC_P_CMP_4_MAX_GFX_WINDOW_COUNT     (0) /* Number of CMP_4_Gx */
#define BVDC_P_CMP_5_MAX_VIDEO_WINDOW_COUNT   (0) /* Number of CMP_5_Vx */
#define BVDC_P_CMP_5_MAX_GFX_WINDOW_COUNT     (0) /* Number of CMP_5_Gx */
#define BVDC_P_CMP_6_MAX_VIDEO_WINDOW_COUNT   (0) /* Number of CMP_6_Vx */
#define BVDC_P_CMP_6_MAX_GFX_WINDOW_COUNT     (0) /* Number of CMP_6_Gx */
#define BVDC_P_CMP_0_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_0_NON_LINEAR_CSC_V1        (0)
#define BVDC_P_CMP_1_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_1_NON_LINEAR_CSC_V1        (0)
#define BVDC_P_CMP_2_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_3_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_4_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_5_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_6_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_0_CLEAR_RECT_V0            (16)
#define BVDC_P_CMP_0_CLEAR_RECT_V1            (8)
#define BVDC_P_CMP_1_CLEAR_RECT_V0            (16)
#define BVDC_P_CMP_1_CLEAR_RECT_V1            (8)
#define BVDC_P_CMP_2_CLEAR_RECT_V0            (16)
#define BVDC_P_CMP_3_CLEAR_RECT_V0            (0)
#define BVDC_P_CMP_4_CLEAR_RECT_V0            (0)
#define BVDC_P_CMP_5_CLEAR_RECT_V0            (0)
#define BVDC_P_CMP_6_CLEAR_RECT_V0            (0)
#define BVDC_P_SUPPORT_WIN_CONST_COLOR        (1)
#define BVDC_P_SUPPORT_CM3D                   (0)
#define BVDC_P_SUPPORT_CMP_DERINGING          (1)
#define BVDC_P_SUPPORT_CMP_MOSAIC_CSC         (5)

/* csc */
#define BVDC_P_SUPPORT_CMP_DEMO_MODE          (2)
#define BVDC_P_SUPPORT_CSC_MAT_COEF_VER       (3)
#define BVDC_P_SUPPORT_COLOR_CLIP             (1)

/* display */
#define BVDC_P_SUPPORT_SEAMLESS_ATTACH        (0)
#define BVDC_P_SUPPORT_EARLY_TRIGGER          (0)
#define BVDC_P_SUPPORT_VBI_ENC_656            (1)
#define BVDC_P_SUPPORT_NEW_SW_INIT            (1)
#define BVDC_P_SUPPORT_4_DACS_PRIM_VEC        (0)
#define BVDC_P_SUPPORT_VEC_VF_VER             (2)
#define BVDC_P_SUPPORT_AMOL_VBI_ENCODE        (1)
#define BVDC_P_SUPPORT_HDMI_OUT               (1)
#define BVDC_P_SUPPORT_DVI_OUT                (1)
#define BVDC_P_SUPPORT_HDMI_RM_VER            (5)
#define BVDC_P_SUPPORT_ITU656_OUT             (1)
#define BVDC_P_SUPPORT_RFM_OUTPUT             (1)
#define BVDC_P_SUPPORT_EXT_SYNC_OUT_MUX       (0)
#define BVDC_P_SUPPORT_EXT_SYNC_PCL_0         (0)

#define BVDC_P_SUPPORT_1080p_60HZ             (1)
#define BVDC_P_SUPPORT_MUD                    (0)
#define BVDC_P_MAX_DACS                       (4)
#define BVDC_P_VEC_HAS_2_DIFFERENT_DVF        (0)
#define BVDC_P_VEC_SUPPORT_DVI_COLOR_CNVT     (0)
#define BVDC_P_ORTHOGONAL_VEC                 (1)
#define BVDC_P_SUPPORT_HD_DAC                 (1)
#define BVDC_P_NUM_SHARED_656                 (1)
#define BVDC_P_NUM_SHARED_DVI                 (1)
#define BVDC_P_NUM_SHARED_RF                  (1)
#define BVDC_P_NUM_SHARED_IT                  (2)
#define BVDC_P_NUM_SHARED_VF                  (2)
#define BVDC_P_NUM_SHARED_SDSRC               (1)
#define BVDC_P_NUM_SHARED_HDSRC               (1)
#define BVDC_P_NUM_SHARED_SM                  (1)
/* Note, here SHARED_SECAM/HDSECAM means the secam cross bar.
 * Some secam cross bar may not have a secam module associated with it.
 */
#define BVDC_P_NUM_SHARED_SECAM               (1)
#define BVDC_P_NUM_SHARED_SECAM_HD            (2)
#define BVDC_P_NUM_SHARED_DAC                 BVDC_P_MAX_DACS
#define BVDC_P_SUPPORT_3D_VIDEO               (1)

#elif (BCHP_CHIP==7231) || (BCHP_CHIP==7346) || (BCHP_CHIP==7344)

#define BVDC_P_SUPPORT_656_MASTER_MODE        (1) /* Has 656Out and run as master mode */
#define BVDC_P_SUPPORT_DVO_MASTER_MODE        (1) /* Has VEC digital out and run as master mode */
#define BVDC_P_SUPPORT_VEC_MUXING             (1) /* TODO: ??? */
#define BVDC_P_SUPPORT_QDAC_VER               (0) /* TODO: recheck with 7422 */
#define BVDC_P_SUPPORT_LOOP_BACK              (2) /* Number of VNET_B_LOOP_BACK_x_SRC */
#define BVDC_P_SUPPORT_DNR                    (1) /* Number of DNR_x core */
#define BVDC_P_SUPPORT_DNR_VER                (7) /* DNR HW version */
#define BVDC_P_SUPPORT_MODULAR_VEC            (2) /* TODO: phase out? */
#define BVDC_P_SUPPORT_NEW_656_IN_VER         (0) /* TODO: recheck */
#define BVDC_P_SUPPORT_VEC_SECAM              (1) /* Support secam output format */
#define BVDC_P_SUPPORT_CMP_CRC                (1) /* Has CRC HW, see CMP_0_CRC_CTRL */

/* source */
#define BVDC_P_SUPPORT_GFD                    (1) /* Number of GFD HW */
#define BVDC_P_SUPPORT_GFD_VER                (4) /* GFD_0 HW version */
#define BVDC_P_SUPPORT_GFD1_VER               (4) /* GFD_1 HW version */
#define BVDC_P_SUPPORT_MFD                    (2) /* Number of MFD HW */
#if (BCHP_VER >= BCHP_VER_B0)
#if (BCHP_CHIP==7231) || (BCHP_CHIP==7344)

#if ((BCHP_CHIP==7231) && (BCHP_VER >= BCHP_VER_B2) || \
     (BCHP_CHIP==7344) && (BCHP_VER >= BCHP_VER_B1)) /* has BVDC_P_VEC_STANDALONE_BUG_FIXED */
#define BVDC_P_SUPPORT_IT_VER                 (3) /* IT HW version */
#else
#define BVDC_P_SUPPORT_IT_VER                 (2) /* IT HW version */
#endif
#define BVDC_P_SUPPORT_VEC_GRPD               (1) /* Support GRPD hw */
#else /* 7346Bx */
#define BVDC_P_SUPPORT_IT_VER                 (3) /* IT HW version */
#define BVDC_P_SUPPORT_VEC_GRPD               (0) /* Support GRPD hw */
#endif
#if (BCHP_CHIP==7231)
#define BVDC_P_SUPPORT_HDDVI                  (1) /* Number of HDDVI HW */
#define BVDC_P_SUPPORT_HDDVI_VER              (8) /* HDDVI HW version */
#define BVDC_P_SUPPORT_DRAIN_F                (3) /* Number of VNET_F_DRAIN_x_SRC */
#define BVDC_P_SUPPORT_TDAC_VER               (11)/* DAC HW version */
#else  /* 7344 & 7346 only */
#define BVDC_P_SUPPORT_HDDVI                  (0) /* Number of HDDVI HW */
#define BVDC_P_SUPPORT_HDDVI_VER              (0) /* HDDVI HW version */
#define BVDC_P_SUPPORT_DRAIN_F                (2) /* Number of VNET_F_DRAIN_x_SRC */
#define BVDC_P_SUPPORT_TDAC_VER               (12)/* DAC HW version */
#endif /* 7344, 7346, and 7231 */
#define BVDC_P_SUPPORT_MFD_VER                (13)/* MFD HW version */
#define BVDC_P_SUPPORT_SCL_VER                (8) /* SCL HW version */
#define BVDC_P_SUPPORT_HSCL_VER               (6) /* HSCL HW version */
#define BVDC_P_SUPPORT_MADR_VER               (5) /* MAD-R HW version */
#define BVDC_P_SUPPORT_3D_INDEP_SRC_CLIP      (1)
#define BVDC_P_SUPPORT_CAP_VER                (5) /* CAP HW version, see bvdc_capture_priv.h */
#define BVDC_P_SUPPORT_CLOCK_GATING           (1) /* Clock gating for power saving */
#else  /* All Ax */
#define BVDC_P_SUPPORT_HDDVI                  (0) /* Number of HDDVI HW */
#define BVDC_P_SUPPORT_HDDVI_VER              (0) /* HDDVI HW version */
#define BVDC_P_SUPPORT_DRAIN_F                (2) /* Number of VNET_F_DRAIN_x_SRC */
#define BVDC_P_SUPPORT_VEC_GRPD               (0) /* Support GRPD hw */
#define BVDC_P_SUPPORT_MFD_VER                (10)/* MFD HW version */
#define BVDC_P_SUPPORT_TDAC_VER               (9) /* DAC HW version */
#define BVDC_P_SUPPORT_SCL_VER                (7) /* SCL HW version */
#define BVDC_P_SUPPORT_HSCL_VER               (5) /* HSCL HW version */
#define BVDC_P_SUPPORT_IT_VER                 (2) /* IT HW version */
#define BVDC_P_SUPPORT_MADR_VER               (2) /* MAD-R HW version */
#define BVDC_P_SUPPORT_3D_INDEP_SRC_CLIP      (0)
#define BVDC_P_SUPPORT_CAP_VER                (4) /* CAP HW version, see bvdc_capture_priv.h */
#define BVDC_P_SUPPORT_CLOCK_GATING           (0) /* Clock gating for power saving */
#endif
#define BVDC_P_SUPPORT_VDEC                   (0) /* Number of VDEC HW */
#define BVDC_P_SUPPORT_VDEC_VER               (0) /* VDEC HW version */
#define BVDC_P_SUPPORT_VDEC_PASSTHROUGH       (0) /* Allow input passthru VDEC */
#define BVDC_P_SUPPORT_DOWNSAMPLE             (0) /* TODO: phase out */
#define BVDC_P_SUPPORT_VCR_MODE               (0) /* If detect VCR from VDEC bypass MAD */

/* BVN */
#define BVDC_P_SUPPORT_PEP                    (1) /* Number of PEP HW, see CMP_x */
#define BVDC_P_SUPPORT_PEP_VER                (1) /* PEP HW version */
#define BVDC_P_SUPPORT_HIST                   (1) /* Number of HIST HW */
#define BVDC_P_SUPPORT_HIST_VER               (1) /* HIST HW version */
#define BVDC_P_SUPPORT_TAB                    (0) /* Number of TAB HW */
#define BVDC_P_SUPPORT_TNT                    (1) /* Number of TNT HW */
#define BVDC_P_SUPPORT_TNT_VER                (6) /* TNT HW version */
#define BVDC_P_SUPPORT_MASK_DITHER            (1) /* Number of MASK HW */
#define BVDC_P_SUPPORT_FGT                    (0) /* Number of FGT HW */
#define BVDC_P_SUPPORT_FGT_VER                (0) /* FGT HW version */
#define BVDC_P_SUPPORT_MAD                    (0) /* Number of old MAD HW */
#define BVDC_P_SUPPORT_MAD_VER                (0) /* old MAD HW version */
#define BVDC_P_SUPPORT_BOX_DETECT             (2) /* Number LBOX HW detect */
#define BVDC_P_SUPPORT_BOX_DETECT_VER         (1)
#define BVDC_P_SUPPORT_CAP                    (4) /* Number of CAP HW */
#define BVDC_P_SUPPORT_VFD                    (4) /* Number of VFD HW */
#define BVDC_P_SUPPORT_SCL                    (4) /* Number of SCL HW */
#define BVDC_P_SUPPORT_HSCL                   (0) /* Number of stand alone HSCL HW */
#define BVDC_P_SUPPORT_HSCL_MAD_HARD_WIRED    (1) /* HSCL HW hardwired in MAD */
#define BVDC_P_SUPPORT_FMISC_PFRI             (0) /* TODO: phase out */
#define BVDC_P_SUPPORT_FMISC_MEMC             (0) /* TODO: phase out */
#define BVDC_P_SUPPORT_DMISC                  (0) /* TODO: phase out */
#define BVDC_P_SUPPORT_BVN_444                (0) /* BVN Bus is 444 */
#define BVDC_P_SUPPORT_BVN_10BIT_444          (0) /* BVN Bus is 10-bit-444 */
#define BVDC_P_SUPPORT_CMP_TEN_TAP_422_TO_444 (1) /* TODO: clarify */
#define BVDC_P_SUPPORT_MAD_SRC_1080I          (1) /* MAD HW with 1920 line buffers (aka HD MAD) */
#define BVDC_P_SUPPORT_ANR                    (0) /* Number of stand alone ANR HW */
#define BVDC_P_SUPPORT_ANR_VER                (0) /* Stand alone ANR HW version */
#define BVDC_P_SUPPORT_ANR_MAD_MEM_SAVING_MODE (1)/* TODO: phase out */
#define BVDC_P_SUPPORT_FREE_CHANNEL           (5) /* Number of VNET_F_FCH_x_SRC */
#define BVDC_P_SUPPORT_DRAIN_B                (1) /* Number of VNET_B_DRAIN_x_SRC */
#define BVDC_P_SUPPORT_DRAIN_VER              (3) /* DRAIN HW version */
#define BVDC_P_SUPPORT_DITHER                 (0) /* TODO: phase out? */
#define BVDC_P_SUPPORT_VEC_DITHER             (0) /* TODO: phase out? */
#define BVDC_P_SUPPORT_MCVP                   (1) /* TODO: Number of MCVP HW */
#define BVDC_P_SUPPORT_MCVP_VER               (3) /* TODO: MCVP HW version */
#define BVDC_P_SUPPORT_MCDI_VER               (0) /* TODO: MCDI HW version */
#define BVDC_P_SUPPORT_MADR                   (1) /* TODO: Number of MAD-R HW */
#define BVDC_P_SUPPORT_MANR                   (0) /* Number of ANR HW in MCVP */
#define BVDC_P_SUPPORT_MANR_VER               (0) /* ANR in MCVP version */
#define BVDC_P_SUPPORT_OSCL                   (0)
#define BVDC_P_SUPPORT_VIDEO_TESTFEATURE1_VER (4) /* Testfeature HW version */

/* CMP */
#define BVDC_P_CMP_0_MAX_VIDEO_WINDOW_COUNT   (2) /* Number of CMP_0_Vx */
#define BVDC_P_CMP_0_MAX_GFX_WINDOW_COUNT     (1) /* Number of CMP_0_Gx */
#define BVDC_P_CMP_1_MAX_VIDEO_WINDOW_COUNT   (2) /* Number of CMP_1_Vx */
#define BVDC_P_CMP_1_MAX_GFX_WINDOW_COUNT     (1) /* Number of CMP_1_Gx */
#define BVDC_P_CMP_2_MAX_VIDEO_WINDOW_COUNT   (0) /* Number of CMP_2_Vx */
#define BVDC_P_CMP_2_MAX_GFX_WINDOW_COUNT     (0) /* Number of CMP_2_Gx */
#define BVDC_P_CMP_3_MAX_VIDEO_WINDOW_COUNT   (0) /* Number of CMP_3_Vx */
#define BVDC_P_CMP_3_MAX_GFX_WINDOW_COUNT     (0) /* Number of CMP_3_Gx */
#define BVDC_P_CMP_4_MAX_VIDEO_WINDOW_COUNT   (0) /* Number of CMP_4_Vx */
#define BVDC_P_CMP_4_MAX_GFX_WINDOW_COUNT     (0) /* Number of CMP_4_Gx */
#define BVDC_P_CMP_5_MAX_VIDEO_WINDOW_COUNT   (0) /* Number of CMP_5_Vx */
#define BVDC_P_CMP_5_MAX_GFX_WINDOW_COUNT     (0) /* Number of CMP_5_Gx */
#define BVDC_P_CMP_6_MAX_VIDEO_WINDOW_COUNT   (0) /* Number of CMP_6_Vx */
#define BVDC_P_CMP_6_MAX_GFX_WINDOW_COUNT     (0) /* Number of CMP_6_Gx */
#define BVDC_P_CMP_0_NON_LINEAR_CSC_V0        (0) /* TODO: phase out? */
#define BVDC_P_CMP_0_NON_LINEAR_CSC_V1        (0) /* TODO: phase out? */
#define BVDC_P_CMP_1_NON_LINEAR_CSC_V0        (0) /* TODO: phase out? */
#define BVDC_P_CMP_1_NON_LINEAR_CSC_V1        (0) /* TODO: phase out? */
#define BVDC_P_CMP_2_NON_LINEAR_CSC_V0        (0) /* TODO: phase out? */
#define BVDC_P_CMP_3_NON_LINEAR_CSC_V0        (0) /* TODO: phase out? */
#define BVDC_P_CMP_4_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_5_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_6_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_0_CLEAR_RECT_V0            (16)/* Number of clear rect support CMP_0_V0_RECT_ENABLE_MASK */
#define BVDC_P_CMP_0_CLEAR_RECT_V1            (16)/* Number of clear rect support CMP_0_V1_RECT_ENABLE_MASK */
#define BVDC_P_CMP_1_CLEAR_RECT_V0            (16)/* Number of clear rect support CMP_1_V0_RECT_ENABLE_MASK */
#define BVDC_P_CMP_1_CLEAR_RECT_V1            (16)/* Number of clear rect support CMP_1_V1_RECT_ENABLE_MASK */
#define BVDC_P_CMP_2_CLEAR_RECT_V0            (0) /* Number of clear rect support CMP_2_V0_RECT_ENABLE_MASK */
#define BVDC_P_CMP_3_CLEAR_RECT_V0            (0) /* Number of clear rect support CMP_2_V1_RECT_ENABLE_MASK */
#define BVDC_P_CMP_4_CLEAR_RECT_V0            (0)
#define BVDC_P_CMP_5_CLEAR_RECT_V0            (0)
#define BVDC_P_CMP_6_CLEAR_RECT_V0            (0)
#define BVDC_P_SUPPORT_WIN_CONST_COLOR        (1) /* TODO: const color */
#define BVDC_P_SUPPORT_CM3D                   (0) /* Number of CM3D HW */
#define BVDC_P_SUPPORT_CMP_DERINGING          (1) /* DeRinging capable */
#define BVDC_P_SUPPORT_CMP_MOSAIC_CSC         (5)

/* csc */
#define BVDC_P_SUPPORT_CMP_DEMO_MODE          (2)
#define BVDC_P_SUPPORT_CSC_MAT_COEF_VER       (3)
#define BVDC_P_SUPPORT_COLOR_CLIP             (0)

/* display */
#define BVDC_P_SUPPORT_SEAMLESS_ATTACH        (0)
#define BVDC_P_SUPPORT_EARLY_TRIGGER          (0)
#define BVDC_P_SUPPORT_VBI_ENC_656            (1)
#define BVDC_P_SUPPORT_NEW_SW_INIT            (1)
#define BVDC_P_SUPPORT_4_DACS_PRIM_VEC        (0)
#define BVDC_P_SUPPORT_VEC_VF_VER             (2)
#define BVDC_P_SUPPORT_AMOL_VBI_ENCODE        (1)
#define BVDC_P_SUPPORT_HDMI_OUT               (1) /* TODO: phase out? */
#define BVDC_P_SUPPORT_DVI_OUT                (1) /* TODO: phase out? */
#define BVDC_P_SUPPORT_HDMI_RM_VER            (5)
#define BVDC_P_SUPPORT_ITU656_OUT             (1)
#define BVDC_P_SUPPORT_RFM_OUTPUT             (0)
#define BVDC_P_SUPPORT_EXT_SYNC_OUT_MUX       (0)
#define BVDC_P_SUPPORT_EXT_SYNC_PCL_0         (0)

#define BVDC_P_SUPPORT_1080p_60HZ             (1)
#define BVDC_P_SUPPORT_MUD                    (0)
#define BVDC_P_MAX_DACS                       (4) /* Number of DAC output */
#define BVDC_P_VEC_HAS_2_DIFFERENT_DVF        (0) /* TODO: phase out? */
#define BVDC_P_VEC_SUPPORT_DVI_COLOR_CNVT     (0) /* TODO: phase out? */
#define BVDC_P_ORTHOGONAL_VEC                 (1)
#define BVDC_P_ORTHOGONAL_VEC_VER             (0)
#define BVDC_P_SUPPORT_HD_DAC                 (1) /* TODO: phase out? */
#define BVDC_P_SUPPORT_STG_VER                (0)

#define BVDC_P_NUM_SHARED_656                 (1) /* Number VEC 656 HW */
#define BVDC_P_NUM_SHARED_DVI                 (1) /* Number VEC Digital output HW */
#define BVDC_P_NUM_SHARED_STG                 (0) /* Number of STG HW */
#define BVDC_P_NUM_SHARED_RF                  (1)
#define BVDC_P_NUM_SHARED_IT                  (2)
#define BVDC_P_NUM_SHARED_VF                  (2) /* Number VEC's VF HW, VEC_CFG_VF_x_SOURCE */
#define BVDC_P_NUM_SHARED_SDSRC               (1) /* Number VEC's VF HW, VEC_CFG_SDSRC_x_SOURCE */
#define BVDC_P_NUM_SHARED_HDSRC               (1) /* Number VEC's VF HW, VEC_CFG_HDSRC_x_SOURCE */
#define BVDC_P_NUM_SHARED_SM                  (1) /* Number VEC's VF HW, VEC_CFG_SDSRC_x_SOURCE */
/* Note, here SHARED_SECAM/HDSECAM means the secam cross bar.
 * Some secam cross bar may not have a secam module associated with it.
 */
#define BVDC_P_NUM_SHARED_SECAM               (1) /* Number of VEC's SECAM HW, VEC_CFG_SECAM_x_SOURCE */
#define BVDC_P_NUM_SHARED_SECAM_HD            (1) /* Number of VEC's SECAM Passthru HW */
#define BVDC_P_NUM_SHARED_DAC                 BVDC_P_MAX_DACS
#define BVDC_P_SUPPORT_3D_VIDEO               (1) /* 3D Video capable */
#define BVDC_P_SUPPORT_STG                    (0) /* STG HW */

#elif (BCHP_CHIP==7429)

#define BVDC_P_SUPPORT_656_MASTER_MODE        (1) /* Has 656Out and run as master mode */
#define BVDC_P_SUPPORT_DVO_MASTER_MODE        (1) /* Has VEC digital out and run as master mode */
#define BVDC_P_SUPPORT_VEC_MUXING             (1) /* TODO: ??? */
#define BVDC_P_SUPPORT_QDAC_VER               (0) /* TODO: recheck with 7422 */
#define BVDC_P_SUPPORT_LOOP_BACK              (2) /* Number of VNET_B_LOOP_BACK_x_SRC */
#define BVDC_P_SUPPORT_DNR                    (1) /* Number of DNR_x core */
#define BVDC_P_SUPPORT_DNR_VER                (7) /* DNR HW version */
#define BVDC_P_SUPPORT_MODULAR_VEC            (2) /* TODO: phase out? */
#define BVDC_P_SUPPORT_NEW_656_IN_VER         (0) /* TODO: recheck */
#define BVDC_P_SUPPORT_VEC_SECAM              (1) /* Support secam output format */
#define BVDC_P_SUPPORT_CAP_VER                (4) /* CAP HW version, see bvdc_capture_priv.h */
#define BVDC_P_SUPPORT_CMP_CRC                (1) /* Has CRC HW, see CMP_0_CRC_CTRL */

/* source */
#define BVDC_P_SUPPORT_GFD                    (2) /* Number of GFD HW */
#define BVDC_P_SUPPORT_GFD_VER                (5) /* GFD_0 HW version */
#define BVDC_P_SUPPORT_GFD1_VER               (5) /* GFD_1 HW version */
#define BVDC_P_SUPPORT_MFD                    (2) /* Number of MFD HW */
#define BVDC_P_SUPPORT_HDDVI                  (1) /* Number of HDDVI HW */
#define BVDC_P_SUPPORT_HDDVI_VER              (8) /* HDDVI HW version */
#define BVDC_P_SUPPORT_DRAIN_F                (3) /* Number of VNET_F_DRAIN_x_SRC */
#define BVDC_P_SUPPORT_VEC_GRPD               (1) /* Support GRPD hw */
#define BVDC_P_SUPPORT_IT_VER                 (3) /* IT HW version */
#if (BCHP_VER >= BCHP_VER_B0)
#define BVDC_P_SUPPORT_MFD_VER                (14)/* MFD HW version */
#else
#define BVDC_P_SUPPORT_MFD_VER                (13)/* MFD HW version */
#endif
#define BVDC_P_SUPPORT_TDAC_VER               (12)/* DAC HW version */
#define BVDC_P_SUPPORT_SCL_VER                (8) /* SCL HW version */
#define BVDC_P_SUPPORT_HSCL_VER               (6) /* HSCL HW version */
#if (BCHP_VER >= BCHP_VER_B0)
#define BVDC_P_SUPPORT_MADR_VER               (7) /* MAD-R HW version */
#else
#define BVDC_P_SUPPORT_MADR_VER               (5) /* MAD-R HW version */
#endif
#define BVDC_P_SUPPORT_VDEC                   (0) /* Number of VDEC HW */
#define BVDC_P_SUPPORT_VDEC_VER               (0) /* VDEC HW version */
#define BVDC_P_SUPPORT_VDEC_PASSTHROUGH       (0) /* Allow input passthru VDEC */
#define BVDC_P_SUPPORT_DOWNSAMPLE             (0) /* TODO: phase out */
#define BVDC_P_SUPPORT_VCR_MODE               (0) /* If detect VCR from VDEC bypass MAD */

/* BVN */
#define BVDC_P_SUPPORT_PEP                    (1) /* Number of PEP HW, see CMP_x */
#define BVDC_P_SUPPORT_PEP_VER                (1) /* PEP HW version */
#define BVDC_P_SUPPORT_HIST                   (1) /* Number of HIST HW */
#define BVDC_P_SUPPORT_HIST_VER               (1) /* HIST HW version */
#define BVDC_P_SUPPORT_TAB                    (0) /* Number of TAB HW */
#define BVDC_P_SUPPORT_TNT                    (1) /* Number of TNT HW */
#define BVDC_P_SUPPORT_TNT_VER                (6) /* TNT HW version */
#define BVDC_P_SUPPORT_MASK_DITHER            (1) /* Number of MASK HW */
#define BVDC_P_SUPPORT_FGT                    (0) /* Number of FGT HW */
#define BVDC_P_SUPPORT_FGT_VER                (0) /* FGT HW version */
#define BVDC_P_SUPPORT_MAD                    (0) /* Number of old MAD HW */
#define BVDC_P_SUPPORT_MAD_VER                (0) /* old MAD HW version */
#define BVDC_P_SUPPORT_BOX_DETECT             (2) /* Number LBOX HW detect */
#define BVDC_P_SUPPORT_BOX_DETECT_VER         (1)
#define BVDC_P_SUPPORT_CAP                    (4) /* Number of CAP HW */
#define BVDC_P_SUPPORT_VFD                    (4) /* Number of VFD HW */
#define BVDC_P_SUPPORT_SCL                    (4) /* Number of SCL HW */
#define BVDC_P_SUPPORT_HSCL                   (0) /* Number of stand alone HSCL HW */
#define BVDC_P_SUPPORT_HSCL_MAD_HARD_WIRED    (1) /* HSCL HW hardwired in MAD */
#define BVDC_P_SUPPORT_FMISC_PFRI             (0) /* TODO: phase out */
#define BVDC_P_SUPPORT_FMISC_MEMC             (0) /* TODO: phase out */
#define BVDC_P_SUPPORT_DMISC                  (0) /* TODO: phase out */
#define BVDC_P_SUPPORT_BVN_444                (0) /* BVN Bus is 444 */
#define BVDC_P_SUPPORT_BVN_10BIT_444          (0) /* BVN Bus is 10-bit-444 */
#define BVDC_P_SUPPORT_CMP_TEN_TAP_422_TO_444 (1) /* TODO: clarify */
#define BVDC_P_SUPPORT_MAD_SRC_1080I          (1) /* MAD HW with 1920 line buffers (aka HD MAD) */
#define BVDC_P_SUPPORT_ANR                    (0) /* Number of stand alone ANR HW */
#define BVDC_P_SUPPORT_ANR_VER                (0) /* Stand alone ANR HW version */
#define BVDC_P_SUPPORT_ANR_MAD_MEM_SAVING_MODE (1)/* TODO: phase out */
#define BVDC_P_SUPPORT_FREE_CHANNEL           (5) /* Number of VNET_F_FCH_x_SRC */
#define BVDC_P_SUPPORT_DRAIN_B                (2) /* Number of VNET_B_DRAIN_x_SRC */
#define BVDC_P_SUPPORT_DRAIN_VER              (3) /* DRAIN HW version */
#define BVDC_P_SUPPORT_DITHER                 (0) /* TODO: phase out? */
#define BVDC_P_SUPPORT_VEC_DITHER             (0) /* TODO: phase out? */
#define BVDC_P_SUPPORT_MCVP                   (1) /* TODO: Number of MCVP HW */
#define BVDC_P_SUPPORT_MCVP_VER               (3) /* TODO: MCVP HW version */
#define BVDC_P_SUPPORT_MCDI_VER               (0) /* TODO: MCDI HW version */
#define BVDC_P_SUPPORT_MADR                   (1) /* TODO: Number of MAD-R HW */
#define BVDC_P_SUPPORT_MANR                   (0) /* Number of ANR HW in MCVP */
#define BVDC_P_SUPPORT_MANR_VER               (0) /* ANR in MCVP version */
#define BVDC_P_SUPPORT_OSCL                   (0)
#define BVDC_P_SUPPORT_VIDEO_TESTFEATURE1_VER (4) /* Testfeature HW version */

/* CMP */
#define BVDC_P_CMP_0_MAX_VIDEO_WINDOW_COUNT   (2) /* Number of CMP_0_Vx */
#define BVDC_P_CMP_0_MAX_GFX_WINDOW_COUNT     (1) /* Number of CMP_0_Gx */
#define BVDC_P_CMP_1_MAX_VIDEO_WINDOW_COUNT   (2) /* Number of CMP_1_Vx */
#define BVDC_P_CMP_1_MAX_GFX_WINDOW_COUNT     (1) /* Number of CMP_1_Gx */
#define BVDC_P_CMP_2_MAX_VIDEO_WINDOW_COUNT   (0) /* Number of CMP_2_Vx */
#define BVDC_P_CMP_2_MAX_GFX_WINDOW_COUNT     (0) /* Number of CMP_2_Gx */
#define BVDC_P_CMP_3_MAX_VIDEO_WINDOW_COUNT   (0) /* Number of CMP_3_Vx */
#define BVDC_P_CMP_3_MAX_GFX_WINDOW_COUNT     (0) /* Number of CMP_3_Gx */
#define BVDC_P_CMP_4_MAX_VIDEO_WINDOW_COUNT   (0) /* Number of CMP_4_Vx */
#define BVDC_P_CMP_4_MAX_GFX_WINDOW_COUNT     (0) /* Number of CMP_4_Gx */
#define BVDC_P_CMP_5_MAX_VIDEO_WINDOW_COUNT   (0) /* Number of CMP_5_Vx */
#define BVDC_P_CMP_5_MAX_GFX_WINDOW_COUNT     (0) /* Number of CMP_5_Gx */
#define BVDC_P_CMP_6_MAX_VIDEO_WINDOW_COUNT   (0) /* Number of CMP_6_Vx */
#define BVDC_P_CMP_6_MAX_GFX_WINDOW_COUNT     (0) /* Number of CMP_6_Gx */
#define BVDC_P_CMP_0_NON_LINEAR_CSC_V0        (0) /* TODO: phase out? */
#define BVDC_P_CMP_0_NON_LINEAR_CSC_V1        (0) /* TODO: phase out? */
#define BVDC_P_CMP_1_NON_LINEAR_CSC_V0        (0) /* TODO: phase out? */
#define BVDC_P_CMP_1_NON_LINEAR_CSC_V1        (0) /* TODO: phase out? */
#define BVDC_P_CMP_2_NON_LINEAR_CSC_V0        (0) /* TODO: phase out? */
#define BVDC_P_CMP_3_NON_LINEAR_CSC_V0        (0) /* TODO: phase out? */
#define BVDC_P_CMP_4_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_5_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_6_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_0_CLEAR_RECT_V0            (16)/* Number of clear rect support CMP_0_V0_RECT_ENABLE_MASK */
#define BVDC_P_CMP_0_CLEAR_RECT_V1            (16)/* Number of clear rect support CMP_0_V1_RECT_ENABLE_MASK */
#define BVDC_P_CMP_1_CLEAR_RECT_V0            (16)/* Number of clear rect support CMP_1_V0_RECT_ENABLE_MASK */
#define BVDC_P_CMP_1_CLEAR_RECT_V1            (16)/* Number of clear rect support CMP_1_V1_RECT_ENABLE_MASK */
#define BVDC_P_CMP_2_CLEAR_RECT_V0            (0) /* Number of clear rect support CMP_2_V0_RECT_ENABLE_MASK */
#define BVDC_P_CMP_3_CLEAR_RECT_V0            (0) /* Number of clear rect support CMP_2_V1_RECT_ENABLE_MASK */
#define BVDC_P_CMP_4_CLEAR_RECT_V0            (0)
#define BVDC_P_CMP_5_CLEAR_RECT_V0            (0)
#define BVDC_P_CMP_6_CLEAR_RECT_V0            (0)
#define BVDC_P_SUPPORT_WIN_CONST_COLOR        (0) /* TODO: const color */
#define BVDC_P_SUPPORT_CM3D                   (0) /* Number of CM3D HW */
#define BVDC_P_SUPPORT_CMP_DERINGING          (1) /* DeRinging capable */
#define BVDC_P_SUPPORT_CMP_MOSAIC_CSC         (5)

/* csc */
#define BVDC_P_SUPPORT_CMP_DEMO_MODE          (2)
#define BVDC_P_SUPPORT_CSC_MAT_COEF_VER       (3)
#define BVDC_P_SUPPORT_COLOR_CLIP             (0)

/* display */
#define BVDC_P_SUPPORT_SEAMLESS_ATTACH        (0)
#define BVDC_P_SUPPORT_EARLY_TRIGGER          (0)
#define BVDC_P_SUPPORT_VBI_ENC_656            (1)
#define BVDC_P_SUPPORT_NEW_SW_INIT            (1)
#define BVDC_P_SUPPORT_4_DACS_PRIM_VEC        (0)
#define BVDC_P_SUPPORT_VEC_VF_VER             (2)
#define BVDC_P_SUPPORT_AMOL_VBI_ENCODE        (1)
#define BVDC_P_SUPPORT_HDMI_OUT               (1) /* TODO: phase out? */
#define BVDC_P_SUPPORT_DVI_OUT                (1) /* TODO: phase out? */
#define BVDC_P_SUPPORT_HDMI_RM_VER            (5)
#define BVDC_P_SUPPORT_ITU656_OUT             (1)
#define BVDC_P_SUPPORT_RFM_OUTPUT             (1)
#define BVDC_P_SUPPORT_EXT_SYNC_OUT_MUX       (0)
#define BVDC_P_SUPPORT_EXT_SYNC_PCL_0         (0)

#define BVDC_P_SUPPORT_1080p_60HZ             (1)
#define BVDC_P_SUPPORT_MUD                    (0)
#define BVDC_P_MAX_DACS                       (4) /* Number of DAC output */
#define BVDC_P_VEC_HAS_2_DIFFERENT_DVF        (0) /* TODO: phase out? */
#define BVDC_P_VEC_SUPPORT_DVI_COLOR_CNVT     (0) /* TODO: phase out? */
#define BVDC_P_ORTHOGONAL_VEC                 (1)
#define BVDC_P_ORTHOGONAL_VEC_VER             (1)
#define BVDC_P_SUPPORT_HD_DAC                 (1) /* TODO: phase out? */

#define BVDC_P_NUM_SHARED_656                 (1) /* Number VEC 656 HW */
#define BVDC_P_NUM_SHARED_DVI                 (1) /* Number VEC Digital output HW */
#define BVDC_P_NUM_SHARED_STG                 (0) /* Number of STG HW */
#define BVDC_P_NUM_SHARED_RF                  (1) /* Number of RFM output */
#define BVDC_P_NUM_SHARED_IT                  (2) /* Numter VEC's VF HW, VEC_CFG_IT_x_SOURCE */
#define BVDC_P_NUM_SHARED_VF                  (2) /* Number VEC's VF HW, VEC_CFG_VF_x_SOURCE */
#define BVDC_P_NUM_SHARED_SDSRC               (1) /* Number VEC's VF HW, VEC_CFG_SDSRC_x_SOURCE */
#define BVDC_P_NUM_SHARED_HDSRC               (1) /* Number VEC's VF HW, VEC_CFG_HDSRC_x_SOURCE */
#define BVDC_P_NUM_SHARED_SM                  (1) /* Number VEC's VF HW, VEC_CFG_SDSRC_x_SOURCE */
/* Note, here SHARED_SECAM/HDSECAM means the secam cross bar.
 * Some secam cross bar may not have a secam module associated with it.
 */
#define BVDC_P_NUM_SHARED_SECAM               (1) /* Number of VEC's SECAM HW, VEC_CFG_SECAM_x_SOURCE */
#define BVDC_P_NUM_SHARED_SECAM_HD            (1) /* Number of VEC's SECAM Passthru HW */
#define BVDC_P_NUM_SHARED_DAC                 BVDC_P_MAX_DACS
#define BVDC_P_SUPPORT_CLOCK_GATING           (1) /* Clock gating for power saving */
#define BVDC_P_SUPPORT_3D_VIDEO               (1) /* 3D Video capable */
#define BVDC_P_SUPPORT_3D_INDEP_SRC_CLIP      (0) /* Independent source clipping in 3D mode */
#define BVDC_P_SUPPORT_STG                    (0) /* STG HW */
#define BVDC_P_SUPPORT_STG_VER                (0)

#elif (BCHP_CHIP==7435)

#define BVDC_P_SUPPORT_656_MASTER_MODE        (0)
#define BVDC_P_SUPPORT_DVO_MASTER_MODE        (1)
#define BVDC_P_SUPPORT_VEC_MUXING             (1)
#define BVDC_P_SUPPORT_QDAC_VER               (0) /* TODO */
#define BVDC_P_SUPPORT_MODULAR_VEC            (2)
#define BVDC_P_SUPPORT_NEW_656_IN_VER         (0)
#define BVDC_P_SUPPORT_VEC_GRPD               (0)
#define BVDC_P_SUPPORT_VEC_SECAM              (1)
#define BVDC_P_SUPPORT_CMP_CRC                (1)

/* source */
#define BVDC_P_SUPPORT_GFD                    (6)
#define BVDC_P_SUPPORT_GFD_VER                (5)
#define BVDC_P_SUPPORT_GFD1_VER               (5)
#define BVDC_P_SUPPORT_MFD                    (4)
#define BVDC_P_SUPPORT_MFD_VER                (14)/* MFD HW version */
#define BVDC_P_SUPPORT_TDAC_VER               (12)/* TDAC/QDAC HW version */
#define BVDC_P_SUPPORT_DNR                    (4) /* Number of DNR_x core */
#define BVDC_P_SUPPORT_DNR_VER                (7)
#define BVDC_P_SUPPORT_BOX_DETECT             (4) /* Number LBOX HW detect */
#define BVDC_P_SUPPORT_BOX_DETECT_VER         (2)
#define BVDC_P_SUPPORT_HDDVI_VER              (9)
#define BVDC_P_SUPPORT_VDEC                   (0)
#define BVDC_P_SUPPORT_VDEC_VER               (0)
#define BVDC_P_SUPPORT_VDEC_PASSTHROUGH       (0)
#define BVDC_P_SUPPORT_HDDVI                  (1)
#define BVDC_P_SUPPORT_DOWNSAMPLE             (0)
#define BVDC_P_SUPPORT_VCR_MODE               (0)

/* BVN */
#define BVDC_P_SUPPORT_PEP                    (1)
#define BVDC_P_SUPPORT_PEP_VER                (1)
#define BVDC_P_SUPPORT_HIST                   (1)
#define BVDC_P_SUPPORT_HIST_VER               (1)
#define BVDC_P_SUPPORT_TAB                    (0)
#define BVDC_P_SUPPORT_TNT                    (1)
#define BVDC_P_SUPPORT_TNT_VER                (6)
#define BVDC_P_SUPPORT_MASK_DITHER            (1)
#define BVDC_P_SUPPORT_FGT                    (0)
#define BVDC_P_SUPPORT_FGT_VER                (0)
#define BVDC_P_SUPPORT_MAD                    (0)
#define BVDC_P_SUPPORT_MAD_VER                (7)
#define BVDC_P_SUPPORT_CAP                    (6)
#define BVDC_P_SUPPORT_CAP_VER                (6) /* CAP HW version, see bvdc_capture_priv.h */
#define BVDC_P_SUPPORT_VFD                    (6)
#define BVDC_P_SUPPORT_SCL                    (6)
#define BVDC_P_SUPPORT_SCL_VER                (8) /* SCL HW version */
#define BVDC_P_SUPPORT_HSCL                   (0)
#define BVDC_P_SUPPORT_HSCL_VER               (6) /* HSCL HW version */
#define BVDC_P_SUPPORT_HSCL_MAD_HARD_WIRED    (1)
#define BVDC_P_SUPPORT_FMISC_PFRI             (0)
#define BVDC_P_SUPPORT_FMISC_MEMC             (0) /* TODO */
#define BVDC_P_SUPPORT_DMISC                  (0)
#define BVDC_P_SUPPORT_BVN_444                (0)
#define BVDC_P_SUPPORT_BVN_10BIT_444          (0)
#define BVDC_P_SUPPORT_CMP_TEN_TAP_422_TO_444 (1)
#define BVDC_P_SUPPORT_MAD_SRC_1080I          (1)
#define BVDC_P_SUPPORT_ANR                    (0)
#define BVDC_P_SUPPORT_ANR_VER                (0)
#define BVDC_P_SUPPORT_ANR_MAD_MEM_SAVING_MODE  (1)
#define BVDC_P_SUPPORT_LOOP_BACK              (9) /* Number of VNET_B_LOOP_BACK_x_SRC */
#define BVDC_P_SUPPORT_FREE_CHANNEL           (10)
#define BVDC_P_SUPPORT_DRAIN_F                (5) /* Number of VNET_F_DRAIN_x_SRC */
#define BVDC_P_SUPPORT_DRAIN_B                (1) /* Number of VNET_B_DRAIN_x_SRC */
#define BVDC_P_SUPPORT_DRAIN_VER              (2) /* DRAIN HW version */
#define BVDC_P_SUPPORT_DITHER                 (0)
#define BVDC_P_SUPPORT_VEC_DITHER             (0)
#define BVDC_P_SUPPORT_STG                    (4) /* Number of STG HW */
#define BVDC_P_SUPPORT_STG_VER                (3) /* STG HW version */
#define BVDC_P_NUM_SHARED_STG                 (4)
#define BVDC_P_ORTHOGONAL_VEC_VER             (1)
#define BVDC_P_SUPPORT_IT_VER                 (3)
#define BVDC_P_SUPPORT_MCVP                   (5) /* Number of MCVP HW */
#define BVDC_P_SUPPORT_MCVP_VER               (2) /* MCVP HW version */
#define BVDC_P_SUPPORT_MADR                   (4) /* Number of MAD-R HW */
#if (BCHP_VER >= BCHP_VER_B0)
#define BVDC_P_SUPPORT_MADR_VER               (7) /* MAD-R HW version */
#else
#define BVDC_P_SUPPORT_MADR_VER               (6) /* MAD-R HW version */
#endif
#define BVDC_P_SUPPORT_MCDI_VER               (5) /* MCDI HW version */
#define BVDC_P_SUPPORT_MANR                   (1) /* Number of ANR HW in MCVP */
#define BVDC_P_SUPPORT_MANR_VER               (3) /* ANR in MCVP version */
#define BVDC_P_SUPPORT_OSCL                   (0)
#define BVDC_P_SUPPORT_VIDEO_TESTFEATURE1_VER (4) /* Testfeature HW version */

/* CMP */
#define BVDC_P_CMP_0_MAX_VIDEO_WINDOW_COUNT   (2)
#define BVDC_P_CMP_0_MAX_GFX_WINDOW_COUNT     (1)
#define BVDC_P_CMP_1_MAX_VIDEO_WINDOW_COUNT   (2)
#define BVDC_P_CMP_1_MAX_GFX_WINDOW_COUNT     (1)
#define BVDC_P_CMP_2_MAX_VIDEO_WINDOW_COUNT   (1)
#define BVDC_P_CMP_2_MAX_GFX_WINDOW_COUNT     (1)
#define BVDC_P_CMP_3_MAX_VIDEO_WINDOW_COUNT   (1) /* Number of CMP_3_Vx */
#define BVDC_P_CMP_3_MAX_GFX_WINDOW_COUNT     (1) /* Number of CMP_3_Gx */
#define BVDC_P_CMP_4_MAX_VIDEO_WINDOW_COUNT   (1) /* Number of CMP_4_Vx */
#define BVDC_P_CMP_4_MAX_GFX_WINDOW_COUNT     (1) /* Number of CMP_4_Gx */
#define BVDC_P_CMP_5_MAX_VIDEO_WINDOW_COUNT   (1) /* Number of CMP_5_Vx */
#define BVDC_P_CMP_5_MAX_GFX_WINDOW_COUNT     (1) /* Number of CMP_5_Gx */
#define BVDC_P_CMP_6_MAX_VIDEO_WINDOW_COUNT   (0) /* Number of CMP_6_Vx */
#define BVDC_P_CMP_6_MAX_GFX_WINDOW_COUNT     (0) /* Number of CMP_6_Gx */
#define BVDC_P_CMP_0_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_0_NON_LINEAR_CSC_V1        (0)
#define BVDC_P_CMP_1_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_1_NON_LINEAR_CSC_V1        (0)
#define BVDC_P_CMP_2_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_3_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_4_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_5_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_6_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_0_CLEAR_RECT_V0            (16)
#define BVDC_P_CMP_0_CLEAR_RECT_V1            (8)
#define BVDC_P_CMP_1_CLEAR_RECT_V0            (16)
#define BVDC_P_CMP_1_CLEAR_RECT_V1            (8)
#define BVDC_P_CMP_2_CLEAR_RECT_V0            (16)
#define BVDC_P_CMP_3_CLEAR_RECT_V0            (0)
#define BVDC_P_CMP_4_CLEAR_RECT_V0            (0)
#define BVDC_P_CMP_5_CLEAR_RECT_V0            (0)
#define BVDC_P_CMP_6_CLEAR_RECT_V0            (0)
#define BVDC_P_SUPPORT_WIN_CONST_COLOR        (1)
#define BVDC_P_SUPPORT_CM3D                   (0)
#define BVDC_P_SUPPORT_CMP_DERINGING          (1)
#define BVDC_P_SUPPORT_CMP_MOSAIC_CSC         (5)

/* csc */
#define BVDC_P_SUPPORT_CMP_DEMO_MODE          (2)
#define BVDC_P_SUPPORT_CSC_MAT_COEF_VER       (3)
#define BVDC_P_SUPPORT_COLOR_CLIP             (1)

/* display */
#define BVDC_P_SUPPORT_DVI_DVF_VER            (1)
#define BVDC_P_SUPPORT_SEAMLESS_ATTACH        (0)
#define BVDC_P_SUPPORT_EARLY_TRIGGER          (0)
#define BVDC_P_SUPPORT_VBI_ENC_656            (1)
#define BVDC_P_SUPPORT_NEW_SW_INIT            (1)
#define BVDC_P_SUPPORT_4_DACS_PRIM_VEC        (0)
#define BVDC_P_SUPPORT_VEC_VF_VER             (2)
#define BVDC_P_SUPPORT_AMOL_VBI_ENCODE        (1)
#define BVDC_P_SUPPORT_HDMI_OUT               (1)
#define BVDC_P_SUPPORT_DVI_OUT                (1)
#define BVDC_P_SUPPORT_HDMI_RM_VER            (5)
#define BVDC_P_SUPPORT_ITU656_OUT             (1)
#define BVDC_P_SUPPORT_RFM_OUTPUT             (1)
#define BVDC_P_SUPPORT_EXT_SYNC_OUT_MUX       (0)
#define BVDC_P_SUPPORT_EXT_SYNC_PCL_0         (0)

#define BVDC_P_SUPPORT_1080p_60HZ             (1)
#define BVDC_P_SUPPORT_MUD                    (0)
#define BVDC_P_MAX_DACS                       (4)
#define BVDC_P_VEC_HAS_2_DIFFERENT_DVF        (0)
#define BVDC_P_VEC_SUPPORT_DVI_COLOR_CNVT     (0)
#define BVDC_P_SUPPORT_LVDS_PLL_VER           (0)
#define BVDC_P_ORTHOGONAL_VEC                 (1)
#define BVDC_P_SUPPORT_HD_DAC                 (1)
#define BVDC_P_NUM_SHARED_656                 (1)
#define BVDC_P_NUM_SHARED_DVI                 (1)
#define BVDC_P_NUM_SHARED_RF                  (1)
#define BVDC_P_NUM_SHARED_IT                  (2)
#define BVDC_P_NUM_SHARED_VF                  (2)
#define BVDC_P_NUM_SHARED_SDSRC               (1)
#define BVDC_P_NUM_SHARED_HDSRC               (1)
#define BVDC_P_NUM_SHARED_SM                  (1)
/* Note, here SHARED_SECAM/HDSECAM means the secam cross bar.
 * Some secam cross bar may not have a secam module associated with it.
 */
#define BVDC_P_NUM_SHARED_SECAM               (1)
#define BVDC_P_NUM_SHARED_SECAM_HD            (2)
#define BVDC_P_NUM_SHARED_DAC                 BVDC_P_MAX_DACS
#define BVDC_P_SUPPORT_CLOCK_GATING           (1)
#define BVDC_P_SUPPORT_3D_VIDEO               (1)

#elif (BCHP_CHIP==7360)

#define BVDC_P_SUPPORT_656_MASTER_MODE        (0)
#define BVDC_P_SUPPORT_DVO_MASTER_MODE        (1)
#define BVDC_P_SUPPORT_VEC_MUXING             (1)
#define BVDC_P_SUPPORT_TDAC_VER              (10) /* TODO */
#define BVDC_P_SUPPORT_QDAC_VER               (0) /* TODO */
#define BVDC_P_SUPPORT_LOOP_BACK              (2)
#define BVDC_P_SUPPORT_DNR                    (1)
#define BVDC_P_SUPPORT_DNR_VER                (7)
#define BVDC_P_SUPPORT_MODULAR_VEC            (2)
#define BVDC_P_SUPPORT_NEW_656_IN_VER         (0)
#define BVDC_P_SUPPORT_VEC_GRPD               (0)
#define BVDC_P_SUPPORT_VEC_SECAM              (1)
#define BVDC_P_SUPPORT_CAP_VER                (4)
#define BVDC_P_SUPPORT_CMP_CRC                (1)
#define BVDC_P_SUPPORT_CMP_LUMA_AVG_VER       (0)

/* source */
#define BVDC_P_SUPPORT_GFD                    (2)
#define BVDC_P_SUPPORT_GFD_VER                (4)
#define BVDC_P_SUPPORT_GFD1_VER               (4)
#define BVDC_P_SUPPORT_MFD                    (1)
#define BVDC_P_SUPPORT_MFD_VER                (12)/* MFD HW version */
#define BVDC_P_SUPPORT_MADR_VER               (5) /* MAD-R HW version */
#define BVDC_P_SUPPORT_VDEC                   (0)
#define BVDC_P_SUPPORT_VDEC_VER               (0)
#define BVDC_P_SUPPORT_VDEC_PASSTHROUGH       (0)
#define BVDC_P_SUPPORT_HDDVI                  (0)
#define BVDC_P_SUPPORT_HDDVI_VER              (0)
#define BVDC_P_SUPPORT_DOWNSAMPLE             (0)
#define BVDC_P_SUPPORT_VCR_MODE               (0)

/* BVN */
#define BVDC_P_SUPPORT_PEP                    (1)
#define BVDC_P_SUPPORT_PEP_VER                (1)
#define BVDC_P_SUPPORT_HIST                   (1)
#define BVDC_P_SUPPORT_HIST_VER               (1)
#define BVDC_P_SUPPORT_TAB                    (0)
#define BVDC_P_SUPPORT_TNT                    (1)
#define BVDC_P_SUPPORT_TNT_VER                (6)
#define BVDC_P_SUPPORT_MASK_DITHER            (1)
#define BVDC_P_SUPPORT_FGT                    (0)
#define BVDC_P_SUPPORT_FGT_VER                (0)
#define BVDC_P_SUPPORT_MAD                    (0)
#define BVDC_P_SUPPORT_MAD_VER                (0)
#define BVDC_P_SUPPORT_BOX_DETECT             (1)
#define BVDC_P_SUPPORT_BOX_DETECT_VER         (1)
#define BVDC_P_SUPPORT_CAP                    (2)
#define BVDC_P_SUPPORT_VFD                    (2)
#define BVDC_P_SUPPORT_SCL                    (2)
#define BVDC_P_SUPPORT_SCL_VER                (7)
#define BVDC_P_SUPPORT_HSCL                   (0)
#define BVDC_P_SUPPORT_HSCL_VER               (5)
#define BVDC_P_SUPPORT_HSCL_MAD_HARD_WIRED    (1)
#define BVDC_P_SUPPORT_FMISC_PFRI             (0)
#define BVDC_P_SUPPORT_FMISC_MEMC             (0)
#define BVDC_P_SUPPORT_DMISC                  (0)
#define BVDC_P_SUPPORT_BVN_444                (0)
#define BVDC_P_SUPPORT_BVN_10BIT_444          (0)
#define BVDC_P_SUPPORT_CMP_TEN_TAP_422_TO_444 (1)
#define BVDC_P_SUPPORT_MAD_SRC_1080I          (1)
#define BVDC_P_SUPPORT_ANR_VER                (0)
#define BVDC_P_SUPPORT_ANR                    (0)
#define BVDC_P_SUPPORT_ANR_MAD_MEM_SAVING_MODE  (1)
#define BVDC_P_SUPPORT_FREE_CHANNEL           (3)
#define BVDC_P_SUPPORT_DRAIN_F                (1)
#define BVDC_P_SUPPORT_DRAIN_B                (1)
#define BVDC_P_SUPPORT_DRAIN_VER              (3) /* DRAIN HW version */
#define BVDC_P_SUPPORT_DITHER                 (0)
#define BVDC_P_SUPPORT_VEC_DITHER             (0)

#define BVDC_P_SUPPORT_MCVP                   (1)
#define BVDC_P_SUPPORT_MCVP_VER               (3) /* TODO */
#define BVDC_P_SUPPORT_MCDI_VER               (0) /* TODO: MCDI HW version */
#define BVDC_P_SUPPORT_MADR                   (1) /* TODO: Number of MAD-R HW */
#define BVDC_P_SUPPORT_MANR                   (0) /* TODO: Number of ANR HW in MCVP */
#define BVDC_P_SUPPORT_MANR_VER               (0) /* TODO: ANR in MCVP version */
#define BVDC_P_SUPPORT_OSCL                   (0)
#define BVDC_P_SUPPORT_VIDEO_TESTFEATURE1_VER (4) /* Testfeature HW version */

/* CMP */
#define BVDC_P_CMP_0_MAX_VIDEO_WINDOW_COUNT   (1)
#define BVDC_P_CMP_0_MAX_GFX_WINDOW_COUNT     (1)
#define BVDC_P_CMP_1_MAX_VIDEO_WINDOW_COUNT   (1)
#define BVDC_P_CMP_1_MAX_GFX_WINDOW_COUNT     (1)
#define BVDC_P_CMP_2_MAX_VIDEO_WINDOW_COUNT   (0)
#define BVDC_P_CMP_2_MAX_GFX_WINDOW_COUNT     (0)
#define BVDC_P_CMP_3_MAX_VIDEO_WINDOW_COUNT   (0) /* Number of CMP_3_Vx */
#define BVDC_P_CMP_3_MAX_GFX_WINDOW_COUNT     (0) /* Number of CMP_3_Gx */
#define BVDC_P_CMP_4_MAX_VIDEO_WINDOW_COUNT   (0) /* Number of CMP_4_Vx */
#define BVDC_P_CMP_4_MAX_GFX_WINDOW_COUNT     (0) /* Number of CMP_4_Gx */
#define BVDC_P_CMP_5_MAX_VIDEO_WINDOW_COUNT   (0) /* Number of CMP_5_Vx */
#define BVDC_P_CMP_5_MAX_GFX_WINDOW_COUNT     (0) /* Number of CMP_5_Gx */
#define BVDC_P_CMP_6_MAX_VIDEO_WINDOW_COUNT   (0) /* Number of CMP_6_Vx */
#define BVDC_P_CMP_6_MAX_GFX_WINDOW_COUNT     (0) /* Number of CMP_6_Gx */
#define BVDC_P_CMP_0_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_0_NON_LINEAR_CSC_V1        (0)
#define BVDC_P_CMP_1_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_1_NON_LINEAR_CSC_V1        (0)
#define BVDC_P_CMP_2_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_3_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_4_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_5_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_6_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_0_CLEAR_RECT_V0            (16)
#define BVDC_P_CMP_0_CLEAR_RECT_V1            (0)
#define BVDC_P_CMP_1_CLEAR_RECT_V0            (16)
#define BVDC_P_CMP_1_CLEAR_RECT_V1            (0)
#define BVDC_P_CMP_2_CLEAR_RECT_V0            (0)
#define BVDC_P_CMP_3_CLEAR_RECT_V0            (0)
#define BVDC_P_CMP_4_CLEAR_RECT_V0            (0)
#define BVDC_P_CMP_5_CLEAR_RECT_V0            (0)
#define BVDC_P_CMP_6_CLEAR_RECT_V0            (0)
#define BVDC_P_SUPPORT_WIN_CONST_COLOR        (0)
#define BVDC_P_SUPPORT_CM3D                   (0)
#define BVDC_P_SUPPORT_CMP_DERINGING          (1)
#define BVDC_P_SUPPORT_CMP_MOSAIC_CSC         (5)

/* csc */
#define BVDC_P_SUPPORT_CMP_DEMO_MODE          (2)
#define BVDC_P_SUPPORT_CSC_MAT_COEF_VER       (3)
#define BVDC_P_SUPPORT_COLOR_CLIP             (1)

/* display */
#define BVDC_P_SUPPORT_SEAMLESS_ATTACH        (0)
#define BVDC_P_SUPPORT_EARLY_TRIGGER          (0)
#define BVDC_P_SUPPORT_VBI_ENC_656            (0)
#define BVDC_P_SUPPORT_NEW_SW_INIT            (1)
#define BVDC_P_SUPPORT_4_DACS_PRIM_VEC        (0)
#define BVDC_P_SUPPORT_VEC_VF_VER             (2)
#define BVDC_P_SUPPORT_AMOL_VBI_ENCODE        (1)
#define BVDC_P_SUPPORT_HDMI_OUT               (1)
#define BVDC_P_SUPPORT_DVI_OUT                (1)
#define BVDC_P_SUPPORT_HDMI_RM_VER            (5)
#define BVDC_P_SUPPORT_ITU656_OUT             (0)
#define BVDC_P_SUPPORT_RFM_OUTPUT             (1)
#define BVDC_P_SUPPORT_EXT_SYNC_OUT_MUX       (0)
#define BVDC_P_SUPPORT_EXT_SYNC_PCL_0         (0)

#define BVDC_P_SUPPORT_1080p_60HZ             (1)
#define BVDC_P_SUPPORT_MUD                    (0)
#define BVDC_P_MAX_DACS                       (4)
#define BVDC_P_VEC_HAS_2_DIFFERENT_DVF        (0)
#define BVDC_P_VEC_SUPPORT_DVI_COLOR_CNVT     (0)
#define BVDC_P_ORTHOGONAL_VEC                 (1)
#define BVDC_P_ORTHOGONAL_VEC_VER             (0)
#define BVDC_P_SUPPORT_HD_DAC                 (1)
#define BVDC_P_SUPPORT_IT_VER                 (3)
#define BVDC_P_SUPPORT_STG_VER                (0)

#define BVDC_P_NUM_SHARED_656                 (1)
#define BVDC_P_NUM_SHARED_DVI                 (1)
#define BVDC_P_NUM_SHARED_STG                 (0)
#define BVDC_P_NUM_SHARED_RF                  (1)
#define BVDC_P_NUM_SHARED_IT                  (2)
#define BVDC_P_NUM_SHARED_VF                  (2)
#define BVDC_P_NUM_SHARED_SDSRC               (1)
#define BVDC_P_NUM_SHARED_HDSRC               (1)
#define BVDC_P_NUM_SHARED_SM                  (1)
/* Note, here SHARED_SECAM/HDSECAM means the secam cross bar.
 * Some secam cross bar may not have a secam module associated with it.
 */
#define BVDC_P_NUM_SHARED_SECAM               (1)
#define BVDC_P_NUM_SHARED_SECAM_HD            (1)
#define BVDC_P_NUM_SHARED_DAC                 BVDC_P_MAX_DACS
#define BVDC_P_SUPPORT_CLOCK_GATING           (1)
#define BVDC_P_SUPPORT_3D_VIDEO               (1)
#define BVDC_P_SUPPORT_3D_INDEP_SRC_CLIP      (0)
#define BVDC_P_SUPPORT_STG                    (0)

#elif (BCHP_CHIP==7584)

#define BVDC_P_SUPPORT_656_MASTER_MODE        (1) /* Has 656Out and run as master mode */
#define BVDC_P_SUPPORT_DVO_MASTER_MODE        (1) /* Has VEC digital out and run as master mode */
#define BVDC_P_SUPPORT_VEC_MUXING             (1) /* TODO: ??? */
#define BVDC_P_SUPPORT_TDAC_VER               (12)/* DAC HW version */
#define BVDC_P_SUPPORT_QDAC_VER               (0) /* TODO: recheck with 7422 */
#define BVDC_P_SUPPORT_LOOP_BACK              (2) /* Number of VNET_B_LOOP_BACK_x_SRC */
#define BVDC_P_SUPPORT_DNR                    (1) /* Number of DNR_x core */
#define BVDC_P_SUPPORT_DNR_VER                (7) /* DNR HW version */
#define BVDC_P_SUPPORT_MODULAR_VEC            (2) /* TODO: phase out? */
#define BVDC_P_SUPPORT_NEW_656_IN_VER         (0) /* TODO: recheck */
#define BVDC_P_SUPPORT_VEC_GRPD               (1) /* Support GRPD hw */
#define BVDC_P_SUPPORT_VEC_SECAM              (1) /* Support secam output format */
#define BVDC_P_SUPPORT_CMP_CRC                (1) /* Has CRC HW, see CMP_0_CRC_CTRL */

/* source */
#define BVDC_P_SUPPORT_GFD                    (1) /* Number of GFD HW */
#define BVDC_P_SUPPORT_GFD_VER                (5) /* GFD_0 HW version */
#define BVDC_P_SUPPORT_GFD1_VER               (5) /* GFD_1 HW version */
#define BVDC_P_SUPPORT_MFD                    (2) /* Number of MFD HW */
#define BVDC_P_SUPPORT_MFD_VER                (14)/* MFD HW version */
#define BVDC_P_SUPPORT_3D_INDEP_SRC_CLIP      (1)
#define BVDC_P_SUPPORT_CLOCK_GATING           (1) /* Clock gating for power saving */
#define BVDC_P_SUPPORT_VDEC                   (0) /* Number of VDEC HW */
#define BVDC_P_SUPPORT_VDEC_VER               (0) /* VDEC HW version */
#define BVDC_P_SUPPORT_VDEC_PASSTHROUGH       (0) /* Allow input passthru VDEC */
#define BVDC_P_SUPPORT_HDDVI                  (0) /* Number of HDDVI HW */
#define BVDC_P_SUPPORT_HDDVI_VER              (0) /* HDDVI HW version */
#define BVDC_P_SUPPORT_DOWNSAMPLE             (0) /* TODO: phase out */
#define BVDC_P_SUPPORT_VCR_MODE               (0) /* If detect VCR from VDEC bypass MAD */

/* BVN */
#define BVDC_P_SUPPORT_PEP                    (1) /* Number of PEP HW, see CMP_x */
#define BVDC_P_SUPPORT_PEP_VER                (1) /* PEP HW version */
#define BVDC_P_SUPPORT_HIST                   (1) /* Number of HIST HW */
#define BVDC_P_SUPPORT_HIST_VER               (1) /* HIST HW version */
#define BVDC_P_SUPPORT_TAB                    (0) /* Number of TAB HW */
#define BVDC_P_SUPPORT_TNT                    (1) /* Number of TNT HW */
#define BVDC_P_SUPPORT_TNT_VER                (6) /* TNT HW version */
#define BVDC_P_SUPPORT_MASK_DITHER            (1) /* Number of MASK HW */
#define BVDC_P_SUPPORT_FGT                    (0) /* Number of FGT HW */
#define BVDC_P_SUPPORT_FGT_VER                (0) /* FGT HW version */
#define BVDC_P_SUPPORT_MAD                    (0) /* Number of old MAD HW */
#define BVDC_P_SUPPORT_MAD_VER                (0) /* old MAD HW version */
#define BVDC_P_SUPPORT_BOX_DETECT             (2) /* Number LBOX HW detect */
#define BVDC_P_SUPPORT_BOX_DETECT_VER         (2)
#define BVDC_P_SUPPORT_CAP                    (4) /* Number of CAP HW */
#define BVDC_P_SUPPORT_CAP_VER                (5) /* CAP HW version, see bvdc_capture_priv.h */
#define BVDC_P_SUPPORT_VFD                    (4) /* Number of VFD HW */
#define BVDC_P_SUPPORT_SCL                    (4) /* Number of SCL HW */
#define BVDC_P_SUPPORT_SCL_VER                (8) /* SCL HW version */
#define BVDC_P_SUPPORT_HSCL                   (0) /* Number of stand alone HSCL HW */
#define BVDC_P_SUPPORT_HSCL_VER               (6) /* HSCL HW version */
#define BVDC_P_SUPPORT_HSCL_MAD_HARD_WIRED    (1) /* HSCL HW hardwired in MAD */
#define BVDC_P_SUPPORT_FMISC_PFRI             (0) /* TODO: phase out */
#define BVDC_P_SUPPORT_FMISC_MEMC             (0) /* TODO: phase out */
#define BVDC_P_SUPPORT_DMISC                  (0) /* TODO: phase out */
#define BVDC_P_SUPPORT_BVN_444                (0) /* BVN Bus is 444 */
#define BVDC_P_SUPPORT_BVN_10BIT_444          (0) /* BVN Bus is 10-bit-444 */
#define BVDC_P_SUPPORT_CMP_TEN_TAP_422_TO_444 (1) /* TODO: clarify */
#define BVDC_P_SUPPORT_MAD_SRC_1080I          (1) /* MAD HW with 1920 line buffers (aka HD MAD) */
#define BVDC_P_SUPPORT_ANR                    (0) /* Number of stand alone ANR HW */
#define BVDC_P_SUPPORT_ANR_VER                (0) /* Stand alone ANR HW version */
#define BVDC_P_SUPPORT_ANR_MAD_MEM_SAVING_MODE (1)/* TODO: phase out */
#define BVDC_P_SUPPORT_FREE_CHANNEL           (5) /* Number of VNET_F_FCH_x_SRC */
#define BVDC_P_SUPPORT_DRAIN_F                (2) /* Number of VNET_F_DRAIN_x_SRC */
#define BVDC_P_SUPPORT_DRAIN_B                (1) /* Number of VNET_B_DRAIN_x_SRC */
#define BVDC_P_SUPPORT_DRAIN_VER              (2) /* DRAIN HW version */
#define BVDC_P_SUPPORT_DITHER                 (0) /* TODO: phase out? */
#define BVDC_P_SUPPORT_VEC_DITHER             (0) /* TODO: phase out? */
#define BVDC_P_SUPPORT_MCVP                   (1) /* TODO: Number of MCVP HW */
#define BVDC_P_SUPPORT_MCVP_VER               (3) /* TODO: MCVP HW version */
#define BVDC_P_SUPPORT_MCDI_VER               (0) /* TODO: MCDI HW version */
#define BVDC_P_SUPPORT_MADR                   (1) /* TODO: Number of MAD-R HW */
#define BVDC_P_SUPPORT_MADR_VER               (7) /* MAD-R HW version */
#define BVDC_P_SUPPORT_MANR                   (0) /* Number of ANR HW in MCVP */
#define BVDC_P_SUPPORT_MANR_VER               (0) /* ANR in MCVP version */
#define BVDC_P_SUPPORT_OSCL                   (0)
#define BVDC_P_SUPPORT_VIDEO_TESTFEATURE1_VER (4) /* Testfeature HW version */

/* CMP */
#define BVDC_P_CMP_0_MAX_VIDEO_WINDOW_COUNT   (2) /* Number of CMP_0_Vx */
#define BVDC_P_CMP_0_MAX_GFX_WINDOW_COUNT     (1) /* Number of CMP_0_Gx */
#define BVDC_P_CMP_1_MAX_VIDEO_WINDOW_COUNT   (2) /* Number of CMP_1_Vx */
#define BVDC_P_CMP_1_MAX_GFX_WINDOW_COUNT     (1) /* Number of CMP_1_Gx */
#define BVDC_P_CMP_2_MAX_VIDEO_WINDOW_COUNT   (0) /* Number of CMP_2_Vx */
#define BVDC_P_CMP_2_MAX_GFX_WINDOW_COUNT     (0) /* Number of CMP_2_Gx */
#define BVDC_P_CMP_3_MAX_VIDEO_WINDOW_COUNT   (0) /* Number of CMP_3_Vx */
#define BVDC_P_CMP_3_MAX_GFX_WINDOW_COUNT     (0) /* Number of CMP_3_Gx */
#define BVDC_P_CMP_4_MAX_VIDEO_WINDOW_COUNT   (0) /* Number of CMP_4_Vx */
#define BVDC_P_CMP_4_MAX_GFX_WINDOW_COUNT     (0) /* Number of CMP_4_Gx */
#define BVDC_P_CMP_5_MAX_VIDEO_WINDOW_COUNT   (0) /* Number of CMP_5_Vx */
#define BVDC_P_CMP_5_MAX_GFX_WINDOW_COUNT     (0) /* Number of CMP_5_Gx */
#define BVDC_P_CMP_6_MAX_VIDEO_WINDOW_COUNT   (0) /* Number of CMP_6_Vx */
#define BVDC_P_CMP_6_MAX_GFX_WINDOW_COUNT     (0) /* Number of CMP_6_Gx */
#define BVDC_P_CMP_0_NON_LINEAR_CSC_V0        (0) /* TODO: phase out? */
#define BVDC_P_CMP_0_NON_LINEAR_CSC_V1        (0) /* TODO: phase out? */
#define BVDC_P_CMP_1_NON_LINEAR_CSC_V0        (0) /* TODO: phase out? */
#define BVDC_P_CMP_1_NON_LINEAR_CSC_V1        (0) /* TODO: phase out? */
#define BVDC_P_CMP_2_NON_LINEAR_CSC_V0        (0) /* TODO: phase out? */
#define BVDC_P_CMP_3_NON_LINEAR_CSC_V0        (0) /* TODO: phase out? */
#define BVDC_P_CMP_4_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_5_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_6_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_0_CLEAR_RECT_V0            (16)/* Number of clear rect support CMP_0_V0_RECT_ENABLE_MASK */
#define BVDC_P_CMP_0_CLEAR_RECT_V1            (16)/* Number of clear rect support CMP_0_V1_RECT_ENABLE_MASK */
#define BVDC_P_CMP_1_CLEAR_RECT_V0            (16)/* Number of clear rect support CMP_1_V0_RECT_ENABLE_MASK */
#define BVDC_P_CMP_1_CLEAR_RECT_V1            (16)/* Number of clear rect support CMP_1_V1_RECT_ENABLE_MASK */
#define BVDC_P_CMP_2_CLEAR_RECT_V0            (0) /* Number of clear rect support CMP_2_V0_RECT_ENABLE_MASK */
#define BVDC_P_CMP_3_CLEAR_RECT_V0            (0) /* Number of clear rect support CMP_2_V1_RECT_ENABLE_MASK */
#define BVDC_P_CMP_4_CLEAR_RECT_V0            (0)
#define BVDC_P_CMP_5_CLEAR_RECT_V0            (0)
#define BVDC_P_CMP_6_CLEAR_RECT_V0            (0)
#define BVDC_P_SUPPORT_WIN_CONST_COLOR        (1) /* TODO: const color */
#define BVDC_P_SUPPORT_CM3D                   (0) /* Number of CM3D HW */
#define BVDC_P_SUPPORT_CMP_DERINGING          (1) /* DeRinging capable */
#define BVDC_P_SUPPORT_CMP_MOSAIC_CSC         (5)

/* csc */
#define BVDC_P_SUPPORT_CMP_DEMO_MODE          (2)
#define BVDC_P_SUPPORT_CSC_MAT_COEF_VER       (3)
#define BVDC_P_SUPPORT_COLOR_CLIP             (0)

/* display */
#define BVDC_P_SUPPORT_SEAMLESS_ATTACH        (0)
#define BVDC_P_SUPPORT_EARLY_TRIGGER          (0)
#define BVDC_P_SUPPORT_VBI_ENC_656            (1)
#define BVDC_P_SUPPORT_NEW_SW_INIT            (1)
#define BVDC_P_SUPPORT_4_DACS_PRIM_VEC        (0)
#define BVDC_P_SUPPORT_VEC_VF_VER             (2)
#define BVDC_P_SUPPORT_AMOL_VBI_ENCODE        (1)
#define BVDC_P_SUPPORT_HDMI_OUT               (1) /* TODO: phase out? */
#define BVDC_P_SUPPORT_DVI_OUT                (1) /* TODO: phase out? */
#define BVDC_P_SUPPORT_HDMI_RM_VER            (5)
#define BVDC_P_SUPPORT_ITU656_OUT             (1)
#define BVDC_P_SUPPORT_RFM_OUTPUT             (0)
#define BVDC_P_SUPPORT_EXT_SYNC_OUT_MUX       (0)
#define BVDC_P_SUPPORT_EXT_SYNC_PCL_0         (0)

#define BVDC_P_SUPPORT_1080p_60HZ             (1)
#define BVDC_P_SUPPORT_MUD                    (0)
#define BVDC_P_MAX_DACS                       (4) /* Number of DAC output */
#define BVDC_P_VEC_HAS_2_DIFFERENT_DVF        (0) /* TODO: phase out? */
#define BVDC_P_VEC_SUPPORT_DVI_COLOR_CNVT     (0) /* TODO: phase out? */
#define BVDC_P_ORTHOGONAL_VEC                 (1)
#define BVDC_P_ORTHOGONAL_VEC_VER             (1)
#define BVDC_P_SUPPORT_HD_DAC                 (1) /* TODO: phase out? */
#define BVDC_P_SUPPORT_IT_VER                 (3) /* IT HW version */
#define BVDC_P_SUPPORT_STG_VER                (0)

#define BVDC_P_NUM_SHARED_656                 (1) /* Number VEC 656 HW */
#define BVDC_P_NUM_SHARED_DVI                 (1) /* Number VEC Digital output HW */
#define BVDC_P_NUM_SHARED_STG                 (0) /* Number of STG HW */
#define BVDC_P_NUM_SHARED_RF                  (1)
#define BVDC_P_NUM_SHARED_IT                  (2)
#define BVDC_P_NUM_SHARED_VF                  (2) /* Number VEC's VF HW, VEC_CFG_VF_x_SOURCE */
#define BVDC_P_NUM_SHARED_SDSRC               (1) /* Number VEC's VF HW, VEC_CFG_SDSRC_x_SOURCE */
#define BVDC_P_NUM_SHARED_HDSRC               (1) /* Number VEC's VF HW, VEC_CFG_HDSRC_x_SOURCE */
#define BVDC_P_NUM_SHARED_SM                  (1) /* Number VEC's VF HW, VEC_CFG_SDSRC_x_SOURCE */
/* Note, here SHARED_SECAM/HDSECAM means the secam cross bar.
 * Some secam cross bar may not have a secam module associated with it.
 */
#define BVDC_P_NUM_SHARED_SECAM               (1) /* Number of VEC's SECAM HW, VEC_CFG_SECAM_x_SOURCE */
#define BVDC_P_NUM_SHARED_SECAM_HD            (2) /* Number of VEC's SECAM Passthru HW */
#define BVDC_P_NUM_SHARED_DAC                 BVDC_P_MAX_DACS
#define BVDC_P_SUPPORT_3D_VIDEO               (1) /* 3D Video capable */
#define BVDC_P_SUPPORT_STG                    (0) /* STG HW */

#elif (BCHP_CHIP==7563)

#define BVDC_P_SUPPORT_656_MASTER_MODE        (1) /* Has 656Out and run as master mode */
#define BVDC_P_SUPPORT_DVO_MASTER_MODE        (1) /* Has VEC digital out and run as master mode */
#define BVDC_P_SUPPORT_VEC_MUXING             (1) /* TODO: ??? */
#define BVDC_P_SUPPORT_TDAC_VER               (12)/* DAC HW version */
#define BVDC_P_SUPPORT_QDAC_VER               (0) /* TODO: recheck with 7422 */
#define BVDC_P_SUPPORT_DNR                    (1) /* Number of DNR_x core */
#define BVDC_P_SUPPORT_DNR_VER                (7) /* DNR HW version */
#define BVDC_P_SUPPORT_MODULAR_VEC            (2) /* TODO: phase out? */
#define BVDC_P_SUPPORT_NEW_656_IN_VER         (0) /* TODO: recheck */
#define BVDC_P_SUPPORT_VEC_GRPD               (1) /* Support GRPD hw */
#define BVDC_P_SUPPORT_VEC_SECAM              (1) /* Support secam output format */
#define BVDC_P_SUPPORT_CMP_CRC                (1) /* Has CRC HW, see CMP_0_CRC_CTRL */

/* source */
#define BVDC_P_SUPPORT_GFD                    (1) /* Number of GFD HW */
#define BVDC_P_SUPPORT_GFD_VER                (5) /* GFD_0 HW version */
#define BVDC_P_SUPPORT_GFD1_VER               (5) /* GFD_1 HW version */
#define BVDC_P_SUPPORT_MFD                    (1) /* Number of MFD HW */
#define BVDC_P_SUPPORT_MFD_VER                (14)/* MFD HW version */
#define BVDC_P_SUPPORT_3D_INDEP_SRC_CLIP      (1)
#define BVDC_P_SUPPORT_CLOCK_GATING           (1) /* Clock gating for power saving */
#define BVDC_P_SUPPORT_VDEC                   (0) /* Number of VDEC HW */
#define BVDC_P_SUPPORT_VDEC_VER               (0) /* VDEC HW version */
#define BVDC_P_SUPPORT_VDEC_PASSTHROUGH       (0) /* Allow input passthru VDEC */
#define BVDC_P_SUPPORT_HDDVI                  (0) /* Number of HDDVI HW */
#define BVDC_P_SUPPORT_HDDVI_VER              (0) /* HDDVI HW version */
#define BVDC_P_SUPPORT_DOWNSAMPLE             (0) /* TODO: phase out */
#define BVDC_P_SUPPORT_VCR_MODE               (0) /* If detect VCR from VDEC bypass MAD */

/* BVN */
#define BVDC_P_SUPPORT_PEP                    (1) /* Number of PEP HW, see CMP_x */
#define BVDC_P_SUPPORT_PEP_VER                (1) /* PEP HW version */
#define BVDC_P_SUPPORT_HIST                   (1) /* Number of HIST HW */
#define BVDC_P_SUPPORT_HIST_VER               (1) /* HIST HW version */
#define BVDC_P_SUPPORT_TAB                    (0) /* Number of TAB HW */
#define BVDC_P_SUPPORT_TNT                    (1) /* Number of TNT HW */
#define BVDC_P_SUPPORT_TNT_VER                (6) /* TNT HW version */
#define BVDC_P_SUPPORT_MASK_DITHER            (1) /* Number of MASK HW */
#define BVDC_P_SUPPORT_FGT                    (0) /* Number of FGT HW */
#define BVDC_P_SUPPORT_FGT_VER                (0) /* FGT HW version */
#define BVDC_P_SUPPORT_MAD                    (0) /* Number of old MAD HW */
#define BVDC_P_SUPPORT_MAD_VER                (0) /* old MAD HW version */
#define BVDC_P_SUPPORT_BOX_DETECT             (1) /* Number LBOX HW detect */
#define BVDC_P_SUPPORT_BOX_DETECT_VER         (2)
#define BVDC_P_SUPPORT_CAP                    (2) /* Number of CAP HW */
#define BVDC_P_SUPPORT_CAP_VER                (5) /* CAP HW version, see bvdc_capture_priv.h */
#define BVDC_P_SUPPORT_VFD                    (2) /* Number of VFD HW */
#define BVDC_P_SUPPORT_SCL                    (2) /* Number of SCL HW */
#define BVDC_P_SUPPORT_SCL_VER                (8) /* SCL HW version */
#define BVDC_P_SUPPORT_HSCL                   (0) /* Number of stand alone HSCL HW */
#define BVDC_P_SUPPORT_HSCL_VER               (6) /* HSCL HW version */
#define BVDC_P_SUPPORT_HSCL_MAD_HARD_WIRED    (1) /* HSCL HW hardwired in MAD */
#define BVDC_P_SUPPORT_FMISC_PFRI             (0) /* TODO: phase out */
#define BVDC_P_SUPPORT_FMISC_MEMC             (0) /* TODO: phase out */
#define BVDC_P_SUPPORT_DMISC                  (0) /* TODO: phase out */
#define BVDC_P_SUPPORT_BVN_444                (0) /* BVN Bus is 444 */
#define BVDC_P_SUPPORT_BVN_10BIT_444          (0) /* BVN Bus is 10-bit-444 */
#define BVDC_P_SUPPORT_CMP_TEN_TAP_422_TO_444 (1) /* TODO: clarify */
#define BVDC_P_SUPPORT_MAD_SRC_1080I          (1) /* MAD HW with 1920 line buffers (aka HD MAD) */
#define BVDC_P_SUPPORT_ANR                    (0) /* Number of stand alone ANR HW */
#define BVDC_P_SUPPORT_ANR_VER                (0) /* Stand alone ANR HW version */
#define BVDC_P_SUPPORT_ANR_MAD_MEM_SAVING_MODE (1)/* TODO: phase out */
#define BVDC_P_SUPPORT_FREE_CHANNEL           (3) /* Number of VNET_F_FCH_x_SRC */
#define BVDC_P_SUPPORT_DRAIN_F                (1) /* Number of VNET_F_DRAIN_x_SRC */
#define BVDC_P_SUPPORT_DRAIN_B                (1) /* Number of VNET_B_DRAIN_x_SRC */
#define BVDC_P_SUPPORT_DRAIN_VER              (2) /* DRAIN HW version */
#define BVDC_P_SUPPORT_LOOP_BACK              (2) /* Number of VNET_B_LOOP_BACK_x_SRC */
#define BVDC_P_SUPPORT_DITHER                 (0) /* TODO: phase out? */
#define BVDC_P_SUPPORT_VEC_DITHER             (0) /* TODO: phase out? */
#define BVDC_P_SUPPORT_MCVP                   (1) /* TODO: Number of MCVP HW */
#define BVDC_P_SUPPORT_MCVP_VER               (3) /* TODO: MCVP HW version */
#define BVDC_P_SUPPORT_MCDI_VER               (0) /* TODO: MCDI HW version */
#define BVDC_P_SUPPORT_MADR                   (1) /* TODO: Number of MAD-R HW */
#define BVDC_P_SUPPORT_MADR_VER               (5) /* MAD-R HW version */
#define BVDC_P_SUPPORT_MANR                   (0) /* Number of ANR HW in MCVP */
#define BVDC_P_SUPPORT_MANR_VER               (0) /* ANR in MCVP version */
#define BVDC_P_SUPPORT_OSCL                   (0)
#define BVDC_P_SUPPORT_VIDEO_TESTFEATURE1_VER (4) /* Testfeature HW version */

/* CMP */
#define BVDC_P_CMP_0_MAX_VIDEO_WINDOW_COUNT   (1) /* Number of CMP_0_Vx */
#define BVDC_P_CMP_0_MAX_GFX_WINDOW_COUNT     (1) /* Number of CMP_0_Gx */
#define BVDC_P_CMP_1_MAX_VIDEO_WINDOW_COUNT   (1) /* Number of CMP_1_Vx */
#define BVDC_P_CMP_1_MAX_GFX_WINDOW_COUNT     (1) /* Number of CMP_1_Gx */
#define BVDC_P_CMP_2_MAX_VIDEO_WINDOW_COUNT   (0) /* Number of CMP_2_Vx */
#define BVDC_P_CMP_2_MAX_GFX_WINDOW_COUNT     (0) /* Number of CMP_2_Gx */
#define BVDC_P_CMP_3_MAX_VIDEO_WINDOW_COUNT   (0) /* Number of CMP_3_Vx */
#define BVDC_P_CMP_3_MAX_GFX_WINDOW_COUNT     (0) /* Number of CMP_3_Gx */
#define BVDC_P_CMP_4_MAX_VIDEO_WINDOW_COUNT   (0) /* Number of CMP_4_Vx */
#define BVDC_P_CMP_4_MAX_GFX_WINDOW_COUNT     (0) /* Number of CMP_4_Gx */
#define BVDC_P_CMP_5_MAX_VIDEO_WINDOW_COUNT   (0) /* Number of CMP_5_Vx */
#define BVDC_P_CMP_5_MAX_GFX_WINDOW_COUNT     (0) /* Number of CMP_5_Gx */
#define BVDC_P_CMP_6_MAX_VIDEO_WINDOW_COUNT   (0) /* Number of CMP_6_Vx */
#define BVDC_P_CMP_6_MAX_GFX_WINDOW_COUNT     (0) /* Number of CMP_6_Gx */
#define BVDC_P_CMP_6_MAX_VIDEO_WINDOW_COUNT   (0) /* Number of CMP_6_Vx */
#define BVDC_P_CMP_6_MAX_GFX_WINDOW_COUNT     (0) /* Number of CMP_6_Gx */
#define BVDC_P_CMP_0_NON_LINEAR_CSC_V0        (0) /* TODO: phase out? */
#define BVDC_P_CMP_0_NON_LINEAR_CSC_V1        (0) /* TODO: phase out? */
#define BVDC_P_CMP_1_NON_LINEAR_CSC_V0        (0) /* TODO: phase out? */
#define BVDC_P_CMP_1_NON_LINEAR_CSC_V1        (0) /* TODO: phase out? */
#define BVDC_P_CMP_2_NON_LINEAR_CSC_V0        (0) /* TODO: phase out? */
#define BVDC_P_CMP_3_NON_LINEAR_CSC_V0        (0) /* TODO: phase out? */
#define BVDC_P_CMP_4_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_5_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_6_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_0_CLEAR_RECT_V0            (16)/* Number of clear rect support CMP_0_V0_RECT_ENABLE_MASK */
#define BVDC_P_CMP_0_CLEAR_RECT_V1            (16)/* Number of clear rect support CMP_0_V1_RECT_ENABLE_MASK */
#define BVDC_P_CMP_1_CLEAR_RECT_V0            (16)/* Number of clear rect support CMP_1_V0_RECT_ENABLE_MASK */
#define BVDC_P_CMP_1_CLEAR_RECT_V1            (16)/* Number of clear rect support CMP_1_V1_RECT_ENABLE_MASK */
#define BVDC_P_CMP_2_CLEAR_RECT_V0            (0) /* Number of clear rect support CMP_2_V0_RECT_ENABLE_MASK */
#define BVDC_P_CMP_3_CLEAR_RECT_V0            (0) /* Number of clear rect support CMP_2_V1_RECT_ENABLE_MASK */
#define BVDC_P_CMP_4_CLEAR_RECT_V0            (0)
#define BVDC_P_CMP_5_CLEAR_RECT_V0            (0)
#define BVDC_P_CMP_6_CLEAR_RECT_V0            (0)
#define BVDC_P_SUPPORT_WIN_CONST_COLOR        (1) /* TODO: const color */
#define BVDC_P_SUPPORT_CM3D                   (0) /* Number of CM3D HW */
#define BVDC_P_SUPPORT_CMP_DERINGING          (1) /* DeRinging capable */
#define BVDC_P_SUPPORT_CMP_MOSAIC_CSC         (5)

/* csc */
#define BVDC_P_SUPPORT_CMP_DEMO_MODE          (2)
#define BVDC_P_SUPPORT_CSC_MAT_COEF_VER       (3)
#define BVDC_P_SUPPORT_COLOR_CLIP             (0)

/* display */
#define BVDC_P_SUPPORT_SEAMLESS_ATTACH        (0)
#define BVDC_P_SUPPORT_EARLY_TRIGGER          (0)
#define BVDC_P_SUPPORT_VBI_ENC_656            (0)
#define BVDC_P_SUPPORT_NEW_SW_INIT            (1)
#define BVDC_P_SUPPORT_4_DACS_PRIM_VEC        (0)
#define BVDC_P_SUPPORT_VEC_VF_VER             (2)
#define BVDC_P_SUPPORT_AMOL_VBI_ENCODE        (1)
#define BVDC_P_SUPPORT_HDMI_OUT               (1) /* TODO: phase out? */
#define BVDC_P_SUPPORT_DVI_OUT                (1) /* TODO: phase out? */
#define BVDC_P_SUPPORT_HDMI_RM_VER            (5)
#define BVDC_P_SUPPORT_ITU656_OUT             (0)
#define BVDC_P_SUPPORT_RFM_OUTPUT             (0)
#define BVDC_P_SUPPORT_EXT_SYNC_OUT_MUX       (0)
#define BVDC_P_SUPPORT_EXT_SYNC_PCL_0         (0)

#define BVDC_P_SUPPORT_1080p_60HZ             (1)
#define BVDC_P_SUPPORT_MUD                    (0)
#define BVDC_P_MAX_DACS                       (1) /* Number of DAC output */
#define BVDC_P_VEC_HAS_2_DIFFERENT_DVF        (0) /* TODO: phase out? */
#define BVDC_P_VEC_SUPPORT_DVI_COLOR_CNVT     (0) /* TODO: phase out? */
#define BVDC_P_ORTHOGONAL_VEC                 (1)
#define BVDC_P_ORTHOGONAL_VEC_VER             (1)
#define BVDC_P_SUPPORT_HD_DAC                 (1) /* TODO: phase out? */
#define BVDC_P_SUPPORT_IT_VER                 (3) /* IT HW version */
#define BVDC_P_SUPPORT_STG_VER                (0)

#define BVDC_P_NUM_SHARED_656                 (1) /* Number VEC 656 HW */
#define BVDC_P_NUM_SHARED_DVI                 (1) /* Number VEC Digital output HW */
#define BVDC_P_NUM_SHARED_STG                 (0) /* Number of STG HW */
#define BVDC_P_NUM_SHARED_RF                  (1)
#define BVDC_P_NUM_SHARED_IT                  (1)
#define BVDC_P_NUM_SHARED_VF                  (1) /* Number VEC's VF HW, VEC_CFG_VF_x_SOURCE */
#define BVDC_P_NUM_SHARED_SDSRC               (1) /* Number VEC's VF HW, VEC_CFG_SDSRC_x_SOURCE */
#define BVDC_P_NUM_SHARED_HDSRC               (0) /* Number VEC's VF HW, VEC_CFG_HDSRC_x_SOURCE */
#define BVDC_P_NUM_SHARED_SM                  (1) /* Number VEC's VF HW, VEC_CFG_SDSRC_x_SOURCE */
/* Note, here SHARED_SECAM/HDSECAM means the secam cross bar.
 * Some secam cross bar may not have a secam module associated with it.
 */
#define BVDC_P_NUM_SHARED_SECAM               (1) /* Number of VEC's SECAM HW, VEC_CFG_SECAM_x_SOURCE */
#define BVDC_P_NUM_SHARED_SECAM_HD            (0) /* Number of VEC's SECAM Passthru HW */
#define BVDC_P_NUM_SHARED_DAC                 BVDC_P_MAX_DACS
#define BVDC_P_SUPPORT_3D_VIDEO               (1) /* 3D Video capable */
#define BVDC_P_SUPPORT_STG                    (0) /* STG HW */

#elif (BCHP_CHIP==7445)

#define BVDC_P_SUPPORT_656_MASTER_MODE        (0)
#define BVDC_P_SUPPORT_DVO_MASTER_MODE        (1)
#define BVDC_P_SUPPORT_VEC_MUXING             (1)
#define BVDC_P_SUPPORT_QDAC_VER               (0) /* TODO */
#define BVDC_P_SUPPORT_MODULAR_VEC            (2)
#define BVDC_P_SUPPORT_NEW_656_IN_VER         (0)
#define BVDC_P_SUPPORT_VEC_GRPD               (0)
#define BVDC_P_SUPPORT_VEC_SECAM              (1)
#define BVDC_P_SUPPORT_CMP_CRC                (1)

/* source */
#define BVDC_P_SUPPORT_GFD                    (7)
#define BVDC_P_SUPPORT_GFD_VER                (5)
#define BVDC_P_SUPPORT_GFD1_VER               (5)
#define BVDC_P_SUPPORT_MFD                    (6)
#define BVDC_P_SUPPORT_MFD_VER                (14)/* MFD HW version */
#define BVDC_P_SUPPORT_TDAC_VER               (13)/* TDAC/QDAC HW version */
#define BVDC_P_SUPPORT_DNR                    (5) /* Number of DNR_x core */
#define BVDC_P_SUPPORT_DNR_VER                (7)
#define BVDC_P_SUPPORT_BOX_DETECT             (6) /* Number LBOX HW detect */
#define BVDC_P_SUPPORT_BOX_DETECT_VER         (2)
#define BVDC_P_SUPPORT_HDDVI_VER              (9)
#define BVDC_P_SUPPORT_VDEC                   (0)
#define BVDC_P_SUPPORT_VDEC_VER               (0)
#define BVDC_P_SUPPORT_VDEC_PASSTHROUGH       (0)
#define BVDC_P_SUPPORT_HDDVI                  (1)
#define BVDC_P_SUPPORT_DOWNSAMPLE             (0)
#define BVDC_P_SUPPORT_VCR_MODE               (0)

/* BVN */
#define BVDC_P_SUPPORT_PEP                    (1)
#define BVDC_P_SUPPORT_PEP_VER                (1)
#define BVDC_P_SUPPORT_HIST                   (1)
#define BVDC_P_SUPPORT_HIST_VER               (1)
#define BVDC_P_SUPPORT_TAB                    (0)
#define BVDC_P_SUPPORT_TNT                    (1)
#define BVDC_P_SUPPORT_TNT_VER                (6)
#define BVDC_P_SUPPORT_MASK_DITHER            (1)
#define BVDC_P_SUPPORT_FGT                    (0)
#define BVDC_P_SUPPORT_FGT_VER                (0)
#define BVDC_P_SUPPORT_MAD                    (0)
#define BVDC_P_SUPPORT_MAD_VER                (7)
#define BVDC_P_SUPPORT_CAP                    (8)
#define BVDC_P_SUPPORT_CAP_VER                (6) /* CAP HW version, see bvdc_capture_priv.h */
#define BVDC_P_SUPPORT_VFD                    (8)
#define BVDC_P_SUPPORT_SCL                    (8)
#define BVDC_P_SUPPORT_SCL_VER                (9) /* SCL HW version */
#define BVDC_P_SUPPORT_HSCL                   (0)
#define BVDC_P_SUPPORT_HSCL_VER               (6) /* HSCL HW version */
#define BVDC_P_SUPPORT_HSCL_MAD_HARD_WIRED    (1)
#define BVDC_P_SUPPORT_FMISC_PFRI             (0)
#define BVDC_P_SUPPORT_FMISC_MEMC             (0) /* TODO */
#define BVDC_P_SUPPORT_DMISC                  (0)
#define BVDC_P_SUPPORT_BVN_444                (0)
#define BVDC_P_SUPPORT_BVN_10BIT_444          (0)
#define BVDC_P_SUPPORT_CMP_TEN_TAP_422_TO_444 (1)
#define BVDC_P_SUPPORT_MAD_SRC_1080I          (1)
#define BVDC_P_SUPPORT_ANR                    (0)
#define BVDC_P_SUPPORT_ANR_VER                (0)
#define BVDC_P_SUPPORT_ANR_MAD_MEM_SAVING_MODE  (1)
#define BVDC_P_SUPPORT_LOOP_BACK              (10) /* Number of VNET_B_LOOP_BACK_x_SRC */
#define BVDC_P_SUPPORT_FREE_CHANNEL           (14)
#define BVDC_P_SUPPORT_DRAIN_F                (7) /* Number of VNET_F_DRAIN_x_SRC */
#define BVDC_P_SUPPORT_DRAIN_B                (1) /* Number of VNET_B_DRAIN_x_SRC */
#define BVDC_P_SUPPORT_DRAIN_VER              (2) /* DRAIN HW version */
#define BVDC_P_SUPPORT_DITHER                 (0)
#define BVDC_P_SUPPORT_VEC_DITHER             (0)
#define BVDC_P_SUPPORT_STG                    (4) /* Number of STG HW */
#define BVDC_P_SUPPORT_STG_VER                (4) /* STG HW version */
#define BVDC_P_NUM_SHARED_STG                 (4)
#define BVDC_P_ORTHOGONAL_VEC_VER             (1)
#define BVDC_P_SUPPORT_IT_VER                 (3)
#define BVDC_P_SUPPORT_MCVP                   (5) /* Number of MCVP HW */
#define BVDC_P_SUPPORT_MCVP_VER               (2) /* MCVP HW version */
#define BVDC_P_SUPPORT_MADR                   (4) /* Number of MAD-R HW */
#define BVDC_P_SUPPORT_MADR_VER               (6) /* MAD-R HW version */
#define BVDC_P_SUPPORT_MCDI_VER               (5) /* MCDI HW version */
#define BVDC_P_SUPPORT_MANR                   (1) /* Number of ANR HW in MCVP */
#define BVDC_P_SUPPORT_MANR_VER               (3) /* ANR in MCVP version */
#define BVDC_P_SUPPORT_OSCL                   (0)
#define BVDC_P_SUPPORT_VIDEO_TESTFEATURE1_VER (4) /* Testfeature HW version */

/* CMP */
#define BVDC_P_CMP_0_MAX_VIDEO_WINDOW_COUNT   (2)
#define BVDC_P_CMP_0_MAX_GFX_WINDOW_COUNT     (1)
#define BVDC_P_CMP_1_MAX_VIDEO_WINDOW_COUNT   (2)
#define BVDC_P_CMP_1_MAX_GFX_WINDOW_COUNT     (1)
#define BVDC_P_CMP_2_MAX_VIDEO_WINDOW_COUNT   (1)
#define BVDC_P_CMP_2_MAX_GFX_WINDOW_COUNT     (1)
#define BVDC_P_CMP_3_MAX_VIDEO_WINDOW_COUNT   (1) /* Number of CMP_3_Vx */
#define BVDC_P_CMP_3_MAX_GFX_WINDOW_COUNT     (1) /* Number of CMP_3_Gx */
#define BVDC_P_CMP_4_MAX_VIDEO_WINDOW_COUNT   (1) /* Number of CMP_4_Vx */
#define BVDC_P_CMP_4_MAX_GFX_WINDOW_COUNT     (1) /* Number of CMP_4_Gx */
#define BVDC_P_CMP_5_MAX_VIDEO_WINDOW_COUNT   (1) /* Number of CMP_5_Vx */
#define BVDC_P_CMP_5_MAX_GFX_WINDOW_COUNT     (1) /* Number of CMP_5_Gx */
#define BVDC_P_CMP_6_MAX_VIDEO_WINDOW_COUNT   (1) /* Number of CMP_6_Vx */
#define BVDC_P_CMP_6_MAX_GFX_WINDOW_COUNT     (1) /* Number of CMP_6_Gx */
#define BVDC_P_CMP_0_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_0_NON_LINEAR_CSC_V1        (0)
#define BVDC_P_CMP_1_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_1_NON_LINEAR_CSC_V1        (0)
#define BVDC_P_CMP_2_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_3_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_4_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_5_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_6_NON_LINEAR_CSC_V0        (0)
#define BVDC_P_CMP_0_CLEAR_RECT_V0            (16)
#define BVDC_P_CMP_0_CLEAR_RECT_V1            (8)
#define BVDC_P_CMP_1_CLEAR_RECT_V0            (16)
#define BVDC_P_CMP_1_CLEAR_RECT_V1            (8)
#define BVDC_P_CMP_2_CLEAR_RECT_V0            (16)
#define BVDC_P_CMP_3_CLEAR_RECT_V0            (0)
#define BVDC_P_CMP_4_CLEAR_RECT_V0            (0)
#define BVDC_P_CMP_5_CLEAR_RECT_V0            (0)
#define BVDC_P_CMP_6_CLEAR_RECT_V0            (0)
#define BVDC_P_SUPPORT_WIN_CONST_COLOR        (1)
#define BVDC_P_SUPPORT_CM3D                   (0)
#define BVDC_P_SUPPORT_CMP_DERINGING          (1)
#define BVDC_P_SUPPORT_CMP_MOSAIC_CSC         (5)

/* csc */
#define BVDC_P_SUPPORT_CMP_DEMO_MODE          (2)
#define BVDC_P_SUPPORT_CSC_MAT_COEF_VER       (3)
#define BVDC_P_SUPPORT_COLOR_CLIP             (1)

/* display */
#define BVDC_P_SUPPORT_DVI_DVF_VER            (1)
#define BVDC_P_SUPPORT_SEAMLESS_ATTACH        (0)
#define BVDC_P_SUPPORT_EARLY_TRIGGER          (0)
#define BVDC_P_SUPPORT_VBI_ENC_656            (1)
#define BVDC_P_SUPPORT_NEW_SW_INIT            (1)
#define BVDC_P_SUPPORT_4_DACS_PRIM_VEC        (0)
#define BVDC_P_SUPPORT_VEC_VF_VER             (2)
#define BVDC_P_SUPPORT_AMOL_VBI_ENCODE        (1)
#define BVDC_P_SUPPORT_HDMI_OUT               (1)
#define BVDC_P_SUPPORT_DVI_OUT                (1)
#define BVDC_P_SUPPORT_HDMI_RM_VER            (6)
#define BVDC_P_SUPPORT_ITU656_OUT             (1)
#define BVDC_P_SUPPORT_RFM_OUTPUT             (1)
#define BVDC_P_SUPPORT_EXT_SYNC_OUT_MUX       (0)
#define BVDC_P_SUPPORT_EXT_SYNC_PCL_0         (0)

#define BVDC_P_SUPPORT_1080p_60HZ             (1)
#define BVDC_P_SUPPORT_MUD                    (0)
#define BVDC_P_MAX_DACS                       (4)
#define BVDC_P_VEC_HAS_2_DIFFERENT_DVF        (0)
#define BVDC_P_VEC_SUPPORT_DVI_COLOR_CNVT     (0)
#define BVDC_P_SUPPORT_LVDS_PLL_VER           (0)
#define BVDC_P_ORTHOGONAL_VEC                 (1)
#define BVDC_P_SUPPORT_HD_DAC                 (1)
#define BVDC_P_NUM_SHARED_656                 (1)
#define BVDC_P_NUM_SHARED_DVI                 (1)
#define BVDC_P_NUM_SHARED_RF                  (1)
#define BVDC_P_NUM_SHARED_IT                  (2)
#define BVDC_P_NUM_SHARED_VF                  (2)
#define BVDC_P_NUM_SHARED_SDSRC               (1)
#define BVDC_P_NUM_SHARED_HDSRC               (1)
#define BVDC_P_NUM_SHARED_SM                  (1)
/* Note, here SHARED_SECAM/HDSECAM means the secam cross bar.
 * Some secam cross bar may not have a secam module associated with it.
 */
#define BVDC_P_NUM_SHARED_SECAM               (1)
#define BVDC_P_NUM_SHARED_SECAM_HD            (2)
#define BVDC_P_NUM_SHARED_DAC                 BVDC_P_MAX_DACS
#define BVDC_P_SUPPORT_CLOCK_GATING           (1)
#define BVDC_P_SUPPORT_3D_VIDEO               (1)

#else
	#error "Port required for VDC."
#endif

#if (BVDC_P_ORTHOGONAL_VEC!=0)
#define BVDC_P_NUM_SHARED_MPAA                (1)
#else
#define BVDC_P_NUM_SHARED_MPAA                (0)
#endif
#define BVDC_P_SUPPORT_DRAIN_CRC              (1) /* Has of VNET_B_CRC_SRC */

#define BVDC_P_SUPPORT_CMP_CLEAR_RECT_V0      (BVDC_P_CMP_0_CLEAR_RECT_V0 || BVDC_P_CMP_1_CLEAR_RECT_V0 || BVDC_P_CMP_2_CLEAR_RECT_V0 || BVDC_P_CMP_3_CLEAR_RECT_V0 || BVDC_P_CMP_4_CLEAR_RECT_V0 || BVDC_P_CMP_5_CLEAR_RECT_V0 || BVDC_P_CMP_6_CLEAR_RECT_V0)
#define BVDC_P_SUPPORT_CMP_CLEAR_RECT_V1      (BVDC_P_CMP_0_CLEAR_RECT_V1 || BVDC_P_CMP_1_CLEAR_RECT_V1 || BVDC_P_CMP_2_CLEAR_RECT_V1)
#define BVDC_P_SUPPORT_CMP_CLEAR_RECT         \
	(BVDC_P_SUPPORT_CMP_CLEAR_RECT_V0 || BVDC_P_SUPPORT_CMP_CLEAR_RECT_V1)

#define BVDC_P_SUPPORT_CMP_NONLINEAR_CSC_WIN  BVDC_P_CMP_0_NON_LINEAR_CSC_V0 || BVDC_P_CMP_0_NON_LINEAR_CSC_V1 || \
	                                          BVDC_P_CMP_1_NON_LINEAR_CSC_V0 || BVDC_P_CMP_1_NON_LINEAR_CSC_V1 || \
	                                          BVDC_P_CMP_2_NON_LINEAR_CSC_V0 || BVDC_P_CMP_3_NON_LINEAR_CSC_V0 || \
	                                          BVDC_P_CMP_4_NON_LINEAR_CSC_V0 || BVDC_P_CMP_5_NON_LINEAR_CSC_V0 || \
	                                          BVDC_P_CMP_6_NON_LINEAR_CSC_V0

/* cause reconfig vdet after input signal loss and re-lock */
#ifndef BVDC_P_CLEANUP_VNET
#define BVDC_P_CLEANUP_VNET                   (1)
#endif

#define BVDC_P_AUTO_ENABLE_CAPTURE            (1)    /* auto: turn on capture if needed */
#define BVDC_P_OPTIMIZE_MEM_USAGE             (1)    /* auto: best fit allocation */
#define BVDC_P_MAD_SRC_HORZ_THRESHOLD         (1920) /* auto: trigger MAD's hscl threshold */

/* Number of Video+Gfx Windows configuration */
#if (BCHP_CHIP==7325) || (BCHP_CHIP==7335) || (BCHP_CHIP==7336) || \
	(BCHP_CHIP==7340) || (BCHP_CHIP==7342) || (BCHP_CHIP==7550) || \
	(BCHP_CHIP==7125) || (BCHP_CHIP==7468) || (BCHP_CHIP==7408) || \
	(BCHP_CHIP==7358) || (BCHP_CHIP==7552) || (BCHP_CHIP==7360) || \
	(BCHP_CHIP==7563)
#define BVDC_P_WIN_GET_REG_OFFSET(window_id) \
	((BVDC_P_WindowId_eComp0_G0==(window_id)) ? (BCHP_CMP_0_G0_SURFACE_SIZE - BCHP_CMP_0_REVISION) \
	:(BVDC_P_WindowId_eComp1_G0==(window_id)) ? (BCHP_CMP_1_G0_SURFACE_SIZE - BCHP_CMP_1_REVISION) \
	:(BCHP_CMP_0_V0_SURFACE_SIZE - BCHP_CMP_0_REVISION))

#elif (BCHP_CHIP==7405) || (BCHP_CHIP==7231) || (BCHP_CHIP==7346) || \
	  (BCHP_CHIP==7344) || (BCHP_CHIP==7429) || (BCHP_CHIP==7584)
#define BVDC_P_WIN_GET_REG_OFFSET(window_id) \
	((BVDC_P_WindowId_eComp0_V1==(window_id)) ? (BCHP_CMP_0_V1_SURFACE_SIZE - BCHP_CMP_0_REVISION) \
	:(BVDC_P_WindowId_eComp0_G0==(window_id)) ? (BCHP_CMP_0_G0_SURFACE_SIZE - BCHP_CMP_0_REVISION) \
	:(BVDC_P_WindowId_eComp1_V1==(window_id)) ? (BCHP_CMP_1_V1_SURFACE_SIZE - BCHP_CMP_1_REVISION) \
	:(BVDC_P_WindowId_eComp1_G0==(window_id)) ? (BCHP_CMP_1_G0_SURFACE_SIZE - BCHP_CMP_1_REVISION) \
	:(BCHP_CMP_0_V0_SURFACE_SIZE - BCHP_CMP_0_REVISION))

#elif (BCHP_CHIP==7400) || (BCHP_CHIP==7420)
#define BVDC_P_WIN_GET_REG_OFFSET(window_id) \
	((BVDC_P_WindowId_eComp0_V1==(window_id)) ? (BCHP_CMP_0_V1_SURFACE_SIZE - BCHP_CMP_0_REVISION) \
	:(BVDC_P_WindowId_eComp0_G0==(window_id)) ? (BCHP_CMP_0_G0_SURFACE_SIZE - BCHP_CMP_0_REVISION) \
	:(BVDC_P_WindowId_eComp1_V1==(window_id)) ? (BCHP_CMP_1_V1_SURFACE_SIZE - BCHP_CMP_1_REVISION) \
	:(BVDC_P_WindowId_eComp1_G0==(window_id)) ? (BCHP_CMP_1_G0_SURFACE_SIZE - BCHP_CMP_1_REVISION) \
	:(BVDC_P_WindowId_eComp2_G0==(window_id)) ? (BCHP_CMP_2_G0_SURFACE_SIZE - BCHP_CMP_2_REVISION) \
	:(BCHP_CMP_0_V0_SURFACE_SIZE - BCHP_CMP_0_REVISION))

#elif (BCHP_CHIP==7422) || (BCHP_CHIP==7425)
#define BVDC_P_WIN_GET_REG_OFFSET(window_id) \
	((BVDC_P_WindowId_eComp0_V1==(window_id)) ? (BCHP_CMP_0_V1_SURFACE_SIZE - BCHP_CMP_0_REVISION) \
	:(BVDC_P_WindowId_eComp0_G0==(window_id)) ? (BCHP_CMP_0_G0_SURFACE_SIZE - BCHP_CMP_0_REVISION) \
	:(BVDC_P_WindowId_eComp1_V1==(window_id)) ? (BCHP_CMP_1_V1_SURFACE_SIZE - BCHP_CMP_1_REVISION) \
	:(BVDC_P_WindowId_eComp1_G0==(window_id)) ? (BCHP_CMP_1_G0_SURFACE_SIZE - BCHP_CMP_1_REVISION) \
	:(BVDC_P_WindowId_eComp2_G0==(window_id)) ? (BCHP_CMP_2_G0_SURFACE_SIZE - BCHP_CMP_2_REVISION) \
	:(BVDC_P_WindowId_eComp3_G0==(window_id)) ? (BCHP_CMP_3_G0_SURFACE_SIZE - BCHP_CMP_3_REVISION) \
	:(BCHP_CMP_0_V0_SURFACE_SIZE - BCHP_CMP_0_REVISION))
#elif (BCHP_CHIP==7435)
#define BVDC_P_WIN_GET_REG_OFFSET(window_id) \
	((BVDC_P_WindowId_eComp0_V1==(window_id)) ? (BCHP_CMP_0_V1_SURFACE_SIZE - BCHP_CMP_0_REVISION) \
	:(BVDC_P_WindowId_eComp0_G0==(window_id)) ? (BCHP_CMP_0_G0_SURFACE_SIZE - BCHP_CMP_0_REVISION) \
	:(BVDC_P_WindowId_eComp1_V1==(window_id)) ? (BCHP_CMP_1_V1_SURFACE_SIZE - BCHP_CMP_1_REVISION) \
	:(BVDC_P_WindowId_eComp1_G0==(window_id)) ? (BCHP_CMP_1_G0_SURFACE_SIZE - BCHP_CMP_1_REVISION) \
	:(BVDC_P_WindowId_eComp2_G0==(window_id)) ? (BCHP_CMP_2_G0_SURFACE_SIZE - BCHP_CMP_2_REVISION) \
	:(BVDC_P_WindowId_eComp3_G0==(window_id)) ? (BCHP_CMP_3_G0_SURFACE_SIZE - BCHP_CMP_3_REVISION) \
	:(BVDC_P_WindowId_eComp4_G0==(window_id)) ? (BCHP_CMP_4_G0_SURFACE_SIZE - BCHP_CMP_4_REVISION) \
	:(BVDC_P_WindowId_eComp5_G0==(window_id)) ? (BCHP_CMP_5_G0_SURFACE_SIZE - BCHP_CMP_5_REVISION) \
	:(BCHP_CMP_0_V0_SURFACE_SIZE - BCHP_CMP_0_REVISION))
#elif (BCHP_CHIP==7445)
#define BVDC_P_WIN_GET_REG_OFFSET(window_id) \
	((BVDC_P_WindowId_eComp0_V1==(window_id)) ? (BCHP_CMP_0_V1_SURFACE_SIZE - BCHP_CMP_0_REVISION) \
	:(BVDC_P_WindowId_eComp0_G0==(window_id)) ? (BCHP_CMP_0_G0_SURFACE_SIZE - BCHP_CMP_0_REVISION) \
	:(BVDC_P_WindowId_eComp1_V1==(window_id)) ? (BCHP_CMP_1_V1_SURFACE_SIZE - BCHP_CMP_1_REVISION) \
	:(BVDC_P_WindowId_eComp1_G0==(window_id)) ? (BCHP_CMP_1_G0_SURFACE_SIZE - BCHP_CMP_1_REVISION) \
	:(BVDC_P_WindowId_eComp2_G0==(window_id)) ? (BCHP_CMP_2_G0_SURFACE_SIZE - BCHP_CMP_2_REVISION) \
	:(BVDC_P_WindowId_eComp3_G0==(window_id)) ? (BCHP_CMP_3_G0_SURFACE_SIZE - BCHP_CMP_3_REVISION) \
	:(BVDC_P_WindowId_eComp4_G0==(window_id)) ? (BCHP_CMP_4_G0_SURFACE_SIZE - BCHP_CMP_4_REVISION) \
	:(BVDC_P_WindowId_eComp5_G0==(window_id)) ? (BCHP_CMP_5_G0_SURFACE_SIZE - BCHP_CMP_5_REVISION) \
	:(BVDC_P_WindowId_eComp6_G0==(window_id)) ? (BCHP_CMP_6_G0_SURFACE_SIZE - BCHP_CMP_6_REVISION) \
	:(BCHP_CMP_0_V0_SURFACE_SIZE - BCHP_CMP_0_REVISION))
#else
#error "Unknown chip!  Not yet supported in VDC."
#endif

/* Derive! */
#define BVDC_P_SUPPORT_SEC_CMP                (0 != BVDC_P_CMP_1_MAX_GFX_WINDOW_COUNT)
#define BVDC_P_SUPPORT_TER_CMP                (0 != BVDC_P_CMP_2_MAX_GFX_WINDOW_COUNT)

#define BVDC_P_SUPPORT_SEC_VEC                BVDC_P_SUPPORT_SEC_CMP
#define BVDC_P_SUPPORT_TER_VEC                BVDC_P_SUPPORT_TER_CMP
#define BVDC_P_SUPPORT_PRM_VEC_CMPN_ONLY      ((!BVDC_P_SUPPORT_SEC_VEC) && (BVDC_P_MAX_DACS > 4))
#define BVDC_P_SUPPORT_SEC_VEC_CMPN_ONLY      (BVDC_P_SUPPORT_SEC_VEC)

#if (BCHP_CHIP==7420)
#define BVDC_P_SUPPORT_COMPONENT_ONLY          (0)
#else
#define BVDC_P_SUPPORT_COMPONENT_ONLY         (BVDC_P_SUPPORT_PRM_VEC_CMPN_ONLY || BVDC_P_SUPPORT_SEC_VEC_CMPN_ONLY)
#endif

/* TODO: clarify this definition */
#define BVDC_P_SUPPORT_MOSAIC_MODE            BVDC_P_SUPPORT_CMP_CLEAR_RECT
#define BVDC_P_MAX_MOSAIC                     (32)

/* Common private defines */
#define BVDC_P_MAX_COMPOSITOR_COUNT           (7)
#define BVDC_P_MAX_DISPLAY_COUNT              BVDC_P_MAX_COMPOSITOR_COUNT
#define BVDC_P_MAX_SOURCE_COUNT               (28) /* includes VFD0, VFD1, VFD2, VFD3, and VFD4 */
#define BVDC_P_MAX_GFX_WINDOWS                \
	(BVDC_P_CMP_0_MAX_GFX_WINDOW_COUNT + \
	 BVDC_P_CMP_1_MAX_GFX_WINDOW_COUNT + \
	 BVDC_P_CMP_2_MAX_GFX_WINDOW_COUNT + \
	 BVDC_P_CMP_3_MAX_GFX_WINDOW_COUNT + \
	 BVDC_P_CMP_4_MAX_GFX_WINDOW_COUNT + \
	 BVDC_P_CMP_5_MAX_GFX_WINDOW_COUNT + \
	 BVDC_P_CMP_6_MAX_GFX_WINDOW_COUNT)

/* Miscellaneous macros */
#define BVDC_P_EVEN_PIXEL                     (2)   /* even pixel alignment */
#define BVDC_P_PITCH_ALIGN                    (32)  /* 32-byte alignment */
#define BVDC_P_BUFFER_ALIGN                   (4)   /* 4-byte alignment */
#define BVDC_P_FIELD_PER_FRAME                (2)   /* Tf / Bf */
#define BVDC_P_MAX_POLARITY                   (3)   /* TF/BF/Frame */

/* default 8 bit 422, since this is the only format that the 3D module supports. */
#define BVDC_P_CAP_PIXEL_FORMAT_8BIT422       (BPXL_eY18_Cb8_Y08_Cr8)
/* default 8 bit 422 on BE system, since this is the only format that the 3D module supports. */
#define BVDC_P_CAP_PIXEL_FORMAT_8BIT422_BE    (BPXL_eCr8_Y08_Cb8_Y18)
/* default 10 bit 444 */
#define BVDC_P_CAP_PIXEL_FORMAT_10BIT444      (BPXL_eX2_Cr10_Y10_Cb10)
/* default 10 bit 422 */
#define BVDC_P_CAP_PIXEL_FORMAT_10BIT422      \
	(BPXL_eX2_Y010_Cb10_Y110_X2_Cr10_Y010_Cb10_X2_Y110_Cr10_Y010_X2_Cb10_Y110_Cr10)

/* delay VDEC/656's BVB output by some fields to avoid garbage data: */
/* for non-component interface */
#define BVDEC_P_BVB_OUT_SLOW_START_COUNT      (20)
/* for component interface with vert frequency equal or above 50 */
#define BVDEC_P_BVB_OUT_SLOW_START_COUNT_CMP  (5)
/* for component interface with vert frequency less than 50 */
#define BVDEC_P_BVB_OUT_SLOW_START_COUNT_CMP_LOW_FREQ   (18)

/* Auto repeat trigger */
#define BVDC_P_AUTO_TRIGGER_VSYNC_DELAY       (5)   /* in vsync(s) */

/* FGT defines */
/* Horizontal and vertical size (pixels) of each pattern in FGT pattern database. */
#define BVDC_P_FGT_DB_PATTERN_SIZE            (64)
/* Noise pixel pattern size */
#define BVDC_P_FGT_NOISE_PIXEL_PATTERN_TABLE_SIZE  \
	(169*BVDC_P_FGT_DB_PATTERN_SIZE*BVDC_P_FGT_DB_PATTERN_SIZE)

/*****************************************************************************
   SCRATCH registers usage statements:
   ----------------------------------

   The VDC currently uses a lot of scratch registers to communicate between
   register DMA and the CPU. For example, to detect input video lock status
   for the VDEC as well as timestamps for multi-buffering etc.

   It turns out that there are some severe restrictions on which scratch
   registers can be used by the VDC. Specifically, if the block containing the
   register can be reset, we could get a bus error. Please see PR 22437 for
   more details.

   To help remove the possibility of running into this case again, we need one
   central location in the VDC where scratch registers are assigned.

   NOTE: Scratch regsiters from blocks that can be reset should avoid!!!
   ---------------------------------------------------------------------------

   We used the following scratch registers for s/w workaround or debug purpose:

	LBOX_0: one scratch for mpeg0 dual debug purposes; (used up)
	LBOX_1: one scratch for mpeg1 dual debug purposes; (used up)
	GFD_0: both scratches for VBI encoder control workaround; (used up)
	GFD_1: one scratch for VBI encoder control workaround; (one left)
	CMP_0: for nop RUL workaround; (used up)
	HSCL_0: one scratch for tuning the HSCL_TUNE_THRESHOLD
	BMISC:
		BMISC_SCRATCH_0[00:00] - activate rectangle size debug

	So we have the following BVN scratch registers unused:

	VFD_x: two;
	CAP_x: two;
	PEP_CMP_0_V0: one
	MMISC: one
	VNET_F: one;
	VNET_B: one;
	FMISC: one;
	BMISC: one;
	MFD_x: two;
	CMP_x: one for each block;
	MAD_x: one for each;
	GFD_1: one left;
	GFD_2: two;
	SCL_?: two for each;
	MASK_x: one;
	DNR_0: one;
 */
/* Theses are for VBI encoder program synchronization; defined in bavc.h;
#define BAVC_VBI_ENC_0_CTRL_SCRATCH           (BCHP_GFD_0_SCRATCH0)
#define BAVC_VBI_ENC_1_CTRL_SCRATCH           (BCHP_GFD_1_SCRATCH0)
#define BAVC_VBI_ENC_2_CTRL_SCRATCH           (BCHP_GFD_0_SCRATCH1)
 */

/* A scratch register for write only to.  Reading from this register
 * yield unknown result. This is to workaround a RDC NOP bug.
 * It's no harm to have a scratch register here coming from a resetable block
 * since software(CPU) would not access this register. */
#define BVDC_P_SCRATCH_WO_REG                 (BCHP_CMP_0_SCRATCH_REGISTER)

/* These are for mpeg data ready callback debug purpose */
#if BVDC_P_SUPPORT_BOX_DETECT > 0
#include "bchp_lbox_0.h"
#endif

#if BVDC_P_SUPPORT_BOX_DETECT >= 2
#define BVDC_P_MPEG_DEBUG_SCRATCH(id) \
	(((id) == BAVC_SourceId_eMpeg0) ? BCHP_LBOX_0_SCRATCH : \
	((BCHP_LBOX_1_REG_START - BCHP_LBOX_0_REG_START) * id + BCHP_LBOX_0_SCRATCH))
#elif BVDC_P_SUPPORT_BOX_DETECT == 1
#define BVDC_P_MPEG_DEBUG_SCRATCH(id) (BCHP_LBOX_0_SCRATCH)
#elif !BVDC_P_SUPPORT_VDEC /* no VDEC; so we borrow fmisc scratch here */
#define BVDC_P_MPEG_DEBUG_SCRATCH(id) (BCHP_FMISC_SCRATCH_0)
#else
	#error "Port required for VDC!!"
#endif

/*****************************************************************/

/* All Compostior windows */
#define BVDC_P_CMP_0_MAX_WINDOW_COUNT \
	(BVDC_P_CMP_0_MAX_VIDEO_WINDOW_COUNT + BVDC_P_CMP_0_MAX_GFX_WINDOW_COUNT)
#define BVDC_P_CMP_1_MAX_WINDOW_COUNT \
	(BVDC_P_CMP_1_MAX_VIDEO_WINDOW_COUNT + BVDC_P_CMP_1_MAX_GFX_WINDOW_COUNT)
#define BVDC_P_CMP_2_MAX_WINDOW_COUNT \
	(BVDC_P_CMP_2_MAX_VIDEO_WINDOW_COUNT + BVDC_P_CMP_2_MAX_GFX_WINDOW_COUNT)
#define BVDC_P_CMP_3_MAX_WINDOW_COUNT \
	(BVDC_P_CMP_3_MAX_VIDEO_WINDOW_COUNT + BVDC_P_CMP_3_MAX_GFX_WINDOW_COUNT)
#define BVDC_P_CMP_4_MAX_WINDOW_COUNT \
	(BVDC_P_CMP_4_MAX_VIDEO_WINDOW_COUNT + BVDC_P_CMP_4_MAX_GFX_WINDOW_COUNT)
#define BVDC_P_CMP_5_MAX_WINDOW_COUNT \
	(BVDC_P_CMP_5_MAX_VIDEO_WINDOW_COUNT + BVDC_P_CMP_5_MAX_GFX_WINDOW_COUNT)
#define BVDC_P_CMP_6_MAX_WINDOW_COUNT \
	(BVDC_P_CMP_6_MAX_VIDEO_WINDOW_COUNT + BVDC_P_CMP_6_MAX_GFX_WINDOW_COUNT)
#define BVDC_P_MAX_WINDOW_COUNT   (18)

/* For scaler */
#define BVDC_P_NRM_SRC_STEP_F_BITS            (20)/* assume max pic size in 12-bit to normalize to 32-bit fixed point math */
#define BVDC_P_OVER_CENTER_WIDTH              (0xffff)

/* Number of ADC (Analog to Digital Converter)& IBAFE */
#define BVDC_P_MAX_ADC                        (23)

/* Define lip sync delay */
#define BVDC_P_MICROSECOND_PER_FIELD          (1000000/60)
#define BVDC_P_LIP_SYNC_RESET_DELAY           UINT32_C(-1)
#define BVDC_P_LIP_SYNC_VEC_DELAY             (1)
#define BVDC_P_LIP_SYNC_CAP_PLK_DELAY         (1)
#define BVDC_P_LIP_SYNC_CAP_PLK_LOCK_DELAY    (1)
#define BVDC_P_LIP_SYNC_CAP_PLK_SLIP_DELAY    (2)
#define BVDC_P_LIP_SYNC_DEINTERLACED_DELAY    (3)
#define BVDC_P_LIP_SYNC_3DCOMB_DELAY          (2)
#define BVDC_P_LIP_SYNC_TOLERANCE             (BVDC_P_MICROSECOND_PER_FIELD)

/* Multi-buffer count.  Field buffers use for capturing and playback. */
#define BVDC_P_BYPASS_MULTI_BUFFER_COUNT      (0)
#define BVDC_P_SYNC_LOCK_MULTI_BUFFER_COUNT   (2)
#define BVDC_P_SYNC_SLIP_MULTI_BUFFER_COUNT   (4)
#define BVDC_P_MAX_USER_CAPTURE_BUFFER_COUNT (25)
#define BVDC_P_MAX_MULTI_BUFFER_COUNT      \
	(BVDC_P_SYNC_SLIP_MULTI_BUFFER_COUNT + \
	 BVDC_P_LIP_SYNC_VEC_DELAY + \
	 BVDC_P_LIP_SYNC_DEINTERLACED_DELAY + \
	 BVDC_P_LIP_SYNC_3DCOMB_DELAY + \
	 BVDC_P_MAX_USER_CAPTURE_BUFFER_COUNT)

/* Vsync delay */
#define BVDC_P_NO_CAPTURE_VSYNC_DELAY         (0)
#define BVDC_P_FIELD_VSYNC_DELAY              (1)
#define BVDC_P_FRAME_VSYNC_DELAY              (2)

/* Max number of RULs */
#define BVDC_P_MAX_ENTRY_PER_MPEG_RUL         (0x4000) /* dword */
#define BVDC_P_MAX_ENTRY_PER_RUL              (0xC00) /* dword */
#define BVDC_P_MAX_VEC_APPLY_WAIT_TIMEOUT     (2000)  /* ms */
#define BVDC_P_MAX_APPLY_WAIT_TIMEOUT         (2000)  /* ms */
#define BVDC_P_MAX_DESTROY_WAIT_TIMEOUT       (2000)  /* ms */
#define BVDC_P_TRIGGER_LOST_THRESHOLD         (20)    /* vsync */

/* For timestamp use */
#define BVDC_P_CLOCK_RATE                     (27ul)  /* 27Mhz */
#define BVDC_P_TIMER_WRAP                     (1<<30) /* timer wraps at this
                                                         count (is 39.7 seconds) */
#define BVDC_P_MAX_TIMER_VALUE                (BVDC_P_TIMER_WRAP-1)  /* the max timer count is one
                                                                        less than the wrap point */
/* (BVDC_P_MAX_ENTRY_PER_RUL/64)* RDC_Blockout (6.4 for 108MHz, 3.2 for 216MHz) */
#define BVDC_P_MULTIBUFFER_RW_TOLERANCE       (BVDC_P_MAX_ENTRY_PER_RUL/10)

/* 16th of a pixel unit */
#define BVDC_P_16TH_PIXEL_SHIFT               (4)

/* Multi-buffer count for RUL */
#define BVDC_P_MAX_MULTI_RUL_BUFFER_COUNT     (2)

#define BVDC_P_NEXT_RUL_IDX(cur_idx) \
	(((cur_idx) + 1) % BVDC_P_MAX_MULTI_RUL_BUFFER_COUNT)

/* Number of time to write full RUL */
#define BVDC_P_RUL_UPDATE_THRESHOLD           (2)

/* Black YCrCb   Use for background and fixed color. */
#define BVDC_P_YCRCB_BLACK                    (0x108080)

#define BVDC_P_TEST_SQUARE_PXL                (0)

#define BVDC_P_MAX_3DCOMB_SD_BUFFER_COUNT     (8)

/* MAD32 field stores count and format */
#define BVDC_P_MAD_QM_BITS_PER_PIXEL          (2)

/* compile option to decide how many field stores to allocate for MAD */
#ifdef BVDC_MAD_NOT_SUPPORT_TRICK_MODE
	#define BVDC_P_MAD_PIXEL_SD_BUFFER_COUNT  (5)
#else
	#define BVDC_P_MAD_PIXEL_SD_BUFFER_COUNT  (5)
#endif

#if (BVDC_P_SUPPORT_MAD)
#define BVDC_P_MAD_QM_FIELD_STORE_COUNT       (8)  /* MAD */
#define BVDC_P_MCDI_QM_FIELD_STORE_COUNT      (0)  /* MCVP */
#elif (BVDC_P_SUPPORT_MCDI_VER>2)
#define BVDC_P_MAD_QM_FIELD_STORE_COUNT       (4)  /* MADR */
#define BVDC_P_MCDI_QM_FIELD_STORE_COUNT      (8)  /* 7425 MCVP */
#else
#define BVDC_P_MAD_QM_FIELD_STORE_COUNT       (4)  /* MADR */
#define BVDC_P_MCDI_QM_FIELD_STORE_COUNT      (4)  /* 7420 MCVP */
#endif

#if (BVDC_P_SUPPORT_MCDI_VER>2)
#define BVDC_P_MCDI_QM_BUFFER_COUNT        (2)  /* 7420 MCVP */
#else
#define BVDC_P_MCDI_QM_BUFFER_COUNT        (1)  /* 7425 MCVP */
#endif
#define BVDC_P_MAD_QM_BUFFER_COUNT         (1)  /* MAD/MADR */

#if (BVDC_P_SUPPORT_MCDI_VER>2)
#define BVDC_P_MAX_MAD_SD_BUFFER_COUNT \
	(BVDC_P_MAD_PIXEL_SD_BUFFER_COUNT + BVDC_P_MCDI_QM_BUFFER_COUNT)
#else
#define BVDC_P_MAX_MAD_SD_BUFFER_COUNT \
	(BVDC_P_MAD_PIXEL_SD_BUFFER_COUNT + BVDC_P_MAD_QM_BUFFER_COUNT)
#endif

#define BVDC_P_MAX_MCDI_BUFFER_COUNT          (4)

/* TODO: Modify later. */
#define BVDC_P_MAX_2HD_BUFFER_COUNT           (0)

#define BVDC_P_MAX_HD_BUFFER_COUNT \
	(BVDC_P_CMP_0_MAX_VIDEO_WINDOW_COUNT * BVDC_P_SYNC_SLIP_MULTI_BUFFER_COUNT)

#define BVDC_P_MAX_SD_BUFFER_COUNT \
	(BVDC_P_CMP_1_MAX_VIDEO_WINDOW_COUNT * BVDC_P_SYNC_SLIP_MULTI_BUFFER_COUNT + \
	  BVDC_P_CMP_2_MAX_VIDEO_WINDOW_COUNT * BVDC_P_SYNC_SLIP_MULTI_BUFFER_COUNT + \
	  BVDC_P_MAX_MAD_SD_BUFFER_COUNT + BVDC_P_MAX_3DCOMB_SD_BUFFER_COUNT)

#define BVDC_P_DEFAULT_2HD_PIP_BUFFER_COUNT   (0)
#define BVDC_P_DEFAULT_HD_PIP_BUFFER_COUNT    (0)
#define BVDC_P_DEFAULT_SD_PIP_BUFFER_COUNT    (0)

/* System clk!  Use to figure out vertical refresh rate! */
/* Used by 656 and HD_DVI in new trigger mode */
#define BVDC_P_108_SYSCLK                     (108000000) /* 108 Mhz */

/* System clk!  Use to figure out vertical refresh rate! */
/* Used by VDEC in new trigger mode */
#define BVDC_P_216_SYSCLK                     (216000000) /* 216 Mhz */

/* Use for video TestFeature1 */
#define BVDC_P_72_SYSCLK                      (72000000)  /* 72 Mhz */

/* Trigger offset!  Where to trigger after end of field. */
#define BVDC_P_TRIGGER_OFFSET(max_vbi_lines)  ((max_vbi_lines) * 65 / 100)

/* Amount of line delay after EOP */
#define BVDC_P_TRIGGER_DELAY                  (12)

#define BVDC_P_ASPR_MAX                       (50)

#define BVDC_P_SCB_BURST_SIZE                 (256)

/***************************************************************************
 * Macros
***************************************************************************/
/* utility macro: to calculate how many entries from "start" register
   to "end" register inclusively. */
#define BVDC_P_REGS_ENTRIES(start, end)    \
	((((BCHP##_##end) - (BCHP##_##start)) / sizeof(uint32_t)) + 1)

/* define for bit-field */
#define BVDC_P_ON                             (1)
#define BVDC_P_OFF                            (0)

/* define that use dirty for building RUL. */
#define BVDC_P_DIRTY                          (1)
#define BVDC_P_CLEAN                          (0)
#define BVDC_P_ALL_DIRTY_BYTE                 (0xFF)

/* Check if dirty bit is clean or dirty */
#define BVDC_P_IS_DIRTY(pDirty)               ((*pDirty).aulInts[0])
#define BVDC_P_IS_CLEAN(pDirty)               (!BVDC_P_IS_DIRTY(pDirty))
#define BVDC_P_NUM_DIRTY_BITS(pDirty)         (sizeof((*pDirty).stBits) * 8)

/* Dirty bit macros.  Uses unions.  Treats bitfields as 32-bit int arrays for speed. */
/* Currently handles up to 32 dirty bits.  Modify macros below to handle more. */
/* Note: Remember to update debug messages using pDirty->aulInts[x] when changing array size */
#define BVDC_P_DIRTY_INT_ARRAY_SIZE           (1)  /* Size of dirty bit int array representation. 1=32-bits, 2=64-bits, etc.  */
#define BVDC_P_DIRTY_INT_ARRAY_ELEMENT_SIZE   (sizeof(uint32_t) * 8) /* Size of dirty bit int array element in bits*/
#define BVDC_P_CLEAN_ALL_DIRTY(pDirty)        (BKNI_Memset((*pDirty).aulInts, 0, sizeof((*pDirty).aulInts)))

#define BVDC_P_SET_ALL_DIRTY(pDirty)          (BKNI_Memset((*pDirty).aulInts, BVDC_P_ALL_DIRTY_BYTE, sizeof((*pDirty).aulInts)))
#define BVDC_P_OR_ALL_DIRTY(pDst, pSrc)       ((*pDst).aulInts[0] |= (*pSrc).aulInts[0])

/* Callback dirty bit macros. */
#define BVDC_P_CB_IS_DIRTY(pDirty)            (BVDC_P_CbIsDirty(pDirty, sizeof(*pDirty)))
#define BVDC_P_CB_CLEAN_ALL_DIRTY(pDirty)     (BKNI_Memset(pDirty, 0, sizeof(*pDirty)))

/* Check for field delta. */
#define BVDC_P_FIELD_DIFF(currstruct, prevstruct, field) \
	((currstruct)->field != (prevstruct)->field)

/* Print a rect struct. */
#define BVDC_P_PRINT_RECT(name, pRect, bForcePrint) \
	if ((pRect != NULL) && (bForcePrint)) {\
	  BDBG_ERR(("%-8s (x, y, w, h): (%4d, %4d, %4d, %4d) - (%4d)", (name), \
		(pRect)->lLeft, (pRect)->lTop, (pRect)->ulWidth, (pRect)->ulHeight, \
		(pRect)->lLeft_R)); \
	} else if (pRect != NULL) \
	BDBG_MSG(("%-8s (x, y, w, h): (%4d, %4d, %4d, %4d) - (%4d)", (name), \
		(pRect)->lLeft, (pRect)->lTop, (pRect)->ulWidth, (pRect)->ulHeight,  \
		(pRect)->lLeft_R))
#define BVDC_P_PRINT_CLIP(name, pRect, bForcePrint) \
	if ((pRect != NULL) && (bForcePrint)) {\
	  BDBG_ERR(("%-8s (x, y, w, h): (%4d, %4d, %4d, %4d) - (%4d)", (name), \
		(pRect)->ulLeft, (pRect)->ulRight, (pRect)->ulTop, (pRect)->ulBottom, \
		(pRect)->lLeftDelta_R)); \
	} else if (pRect != NULL) \
	  BDBG_MSG(("%-8s (x, y, w, h): (%4d, %4d, %4d, %4d) - (%4d)", (name), \
		(pRect)->ulLeft, (pRect)->ulRight, (pRect)->ulTop, (pRect)->ulBottom, \
		(pRect)->lLeftDelta_R))
#define BVDC_P_PRINT_CNT_RECT(name, pRect, bForcePrint) \
	if ((pRect != NULL) && (bForcePrint)) {\
	  BDBG_ERR(("%-8s (x, y, w, h): (%d+%d/16, %d+%d/16, %4d, %4d) - (%d+%d/16)", \
		(name), (pRect)->lLeft >> 4, (pRect)->lLeft & 0xf, (pRect)->lTop >> 4, \
		(pRect)->lTop & 0xf, (pRect)->ulWidth, (pRect)->ulHeight,  \
		(pRect)->lLeft_R >> 4, (pRect)->lLeft_R & 0xf)); \
	} else if (pRect != NULL) \
	  BDBG_MSG(("%-8s (x, y, w, h): (%d+%d/16, %d+%d/16, %4d, %4d) - (%d+%d/16)", \
		(name), (pRect)->lLeft >> 4, (pRect)->lLeft & 0xf, (pRect)->lTop >> 4, \
		(pRect)->lTop & 0xf, (pRect)->ulWidth, (pRect)->ulHeight,  \
		(pRect)->lLeft_R >> 4, (pRect)->lLeft_R & 0xf))
#define BVDC_P_PRINT_SCLCUT_RECT(name, pRect, bForcePrint) \
	if ((pRect != NULL) && (bForcePrint)) {\
	  BDBG_ERR(("%-8s (x, y, w, h): (%d+%d/64, %d+%d/16384, %4d, %4d) - (%d+%d/64)", \
		(name), (pRect)->lLeft >> 6, (pRect)->lLeft & 0x3f, (pRect)->lTop >> 14, \
		(pRect)->lTop & 0x3fff, (pRect)->ulWidth, (pRect)->ulHeight,  \
		(pRect)->lLeft_R >> 6, (pRect)->lLeft_R & 0x3f)); \
	} else if (pRect != NULL) \
	  BDBG_MSG(("%-8s (x, y, w, h): (%d+%d/64, %d+%d/16384, %4d, %4d) - (%d+%d/64)", \
		(name), (pRect)->lLeft >> 6, (pRect)->lLeft & 0x3f, (pRect)->lTop >> 14, \
		(pRect)->lTop & 0x3fff, (pRect)->ulWidth, (pRect)->ulHeight,  \
		(pRect)->lLeft_R >> 6, (pRect)->lLeft_R & 0x3f))
#define BVDC_P_PRINT_HSCLCUT(name, pPic, bForcePrint) \
	if ((pPic != NULL) && (bForcePrint)) {\
	  BDBG_ERR(("%-8s (left, width): (%d+%d/64, %4d) - (%d+%d/64)", (name), \
		(pPic)->lHsclCutLeft >> 6, (pPic)->lHsclCutLeft & 0x3f, (pPic)->ulHsclCutWidth, \
		(pPic)->lHsclCutLeft_R >> 6, (pPic)->lHsclCutLeft_R & 0x3f)); \
	} else if (pPic != NULL) \
	  BDBG_MSG(("%-8s (left, width): (%d+%d/64, %4d) - (%d+%d/64)", (name), \
		(pPic)->lHsclCutLeft >> 6, (pPic)->lHsclCutLeft & 0x3f, (pPic)->ulHsclCutWidth, \
		(pPic)->lHsclCutLeft_R >> 6, (pPic)->lHsclCutLeft_R & 0x3f))

#ifdef BVDC_DEBUG_BUFFER
#define BVDC_P_BUF_MSG    BDBG_ERR
#else
#define BVDC_P_BUF_MSG    BDBG_MSG
#endif

/* Take absolute value of a number */
#define BVDC_P_ABS(value) (((value)<0) ? (-(value)) : (value))

#define BVDC_P_IS_UNKNOWN_ASPR(asprMode, sarX, sarY)                      \
	((BFMT_AspectRatio_eUnknown == asprMode) ||                               \
	   ((BFMT_AspectRatio_eSAR == asprMode) &&                                \
	    (((0 == sarX) || (0 == sarY)) || ((sarX / sarY) > BVDC_P_ASPR_MAX))))


/* Turn a float to fixed-point */
#define BVDC_P_FLOAT_TO_FIXED(fvalue, i_bit, f_bit) \
	(((uint32_t)(BVDC_P_ABS(fvalue) * (1 << (f_bit)))) | \
	(((fvalue)<0) << ((i_bit) + (f_bit))))

/* Make a point base on numerator, denominator, and fraction bits */
#define BVDC_P_MAKE_FIXED_POINT(num, denom, f_bits) \
	(((num) * (1 << (f_bits))) / (denom))

/* Turn the fixed into two's complements, use int op might cause overflow */
#define BVDC_P_FIXED_A_MINUS_FIXED_B(fixed_a, fixed_b) \
	(((fixed_a) > (fixed_b)) ? ((fixed_a) - (fixed_b)) \
	: (~((fixed_b) - (fixed_a)) + 1))

/* ---------------------------------------------
 * TestFeature1 revision
 * --------------------------------------------- */
/* 7400, 7405, 7325, 7335, 7336, 7340, 7342, 7550,
 * 3548 Ax, 3556 Ax, 7420, 7125 AxBx, 7468, 7408:
 *  No TestFeature1 support. */
#define BVDC_P_VIDEO_TESTFEATURE1_VER_0       (0)

/* 3548 B0B1, 3556 B0B1:
 *  Support TestFeature1 in CAP, MAD and GFD.
 *  Software workaround needed:
 *      BVDC_P_DCX_CAP_OVERFLOW_WORKAROUND
 *      BVDC_P_DCX_CAP_RESET_WORKAROUND
 *      BVDC_P_DCX_VFD_RESET_WORKAROUND
 *      BVDC_P_DCX_HSIZE_WORKAROUND
 */
#define BVDC_P_VIDEO_TESTFEATURE1_VER_1       (1)

/* 3548 B2 and above, 3556 B2 and above:
 *  Support TestFeature1 in CAP, MAD and GFD.
 *  Capture FIFO overflow issue is fixed
 *  Software workaround needed:
 *      BVDC_P_DCX_CAP_RESET_WORKAROUND
 *      BVDC_P_DCX_VFD_RESET_WORKAROUND
 *      BVDC_P_DCX_HSIZE_WORKAROUND
 */
#define BVDC_P_VIDEO_TESTFEATURE1_VER_2       (2)

/* 7125 Cx and above:
 *  Support TestFeature1 in MAD and ANR, no TestFeature1
 *  support in CAP or GFD.
 */
#define BVDC_P_VIDEO_TESTFEATURE1_VER_3       (3)

/* 7422, 7425, 7358, 7552, 7231, 7346, 7344:
 *  Support TestFeature1 in MAD and ANR (MCVP and MADR),
 *  no TestFeature1 support in CAP or GFD.
 * ANR DCD and DCE cross over issue is fixed.
 */
#define BVDC_P_VIDEO_TESTFEATURE1_VER_4       (4)

/* TestFeature1 support in each block */
#define BVDC_P_SUPPORT_VIDEO_TESTFEATURE1_MAD_ANR   \
	 (BVDC_P_SUPPORT_VIDEO_TESTFEATURE1_VER > BVDC_P_VIDEO_TESTFEATURE1_VER_0)

#define BVDC_P_SUPPORT_VIDEO_TESTFEATURE1_CAP   \
	((BVDC_P_SUPPORT_VIDEO_TESTFEATURE1_VER == BVDC_P_VIDEO_TESTFEATURE1_VER_1) || \
	 (BVDC_P_SUPPORT_VIDEO_TESTFEATURE1_VER == BVDC_P_VIDEO_TESTFEATURE1_VER_2))

#define BVDC_P_SUPPORT_VIDEO_TESTFEATURE1_GFD   \
	((BVDC_P_SUPPORT_VIDEO_TESTFEATURE1_VER == BVDC_P_VIDEO_TESTFEATURE1_VER_1) || \
	 (BVDC_P_SUPPORT_VIDEO_TESTFEATURE1_VER == BVDC_P_VIDEO_TESTFEATURE1_VER_2))

/* TestFeature1 software workaround */
/* PR48938 capture FIFO overflow */
#define BVDC_P_DCX_CAP_OVERFLOW_WORKAROUND        \
	 (BVDC_P_SUPPORT_VIDEO_TESTFEATURE1_VER < BVDC_P_VIDEO_TESTFEATURE1_VER_2)

#define BVDC_P_DCX_ANR_CROSS_OVER_WORKAROUND        \
	 (BVDC_P_SUPPORT_VIDEO_TESTFEATURE1_VER < BVDC_P_VIDEO_TESTFEATURE1_VER_4)

#if (BVDC_P_SUPPORT_VIDEO_TESTFEATURE1_VER == 1) || (BVDC_P_SUPPORT_VIDEO_TESTFEATURE1_VER == 2)
/* PR51649: reset cap during mode switch */
#define BVDC_P_DCX_CAP_RESET_WORKAROUND        (1)

/* PR48418: reset vfd during mode switch */
#define BVDC_P_DCX_VFD_RESET_WORKAROUND        (1)

/* PR51159: HSIZE needs to be multiple of 4 when DCX in NOLEFT 422 mode */
#define BVDC_P_DCX_HSIZE_WORKAROUND            (1)

#else
#define BVDC_P_DCX_CAP_RESET_WORKAROUND        (0)
#define BVDC_P_DCX_VFD_RESET_WORKAROUND        (0)
#define BVDC_P_DCX_HSIZE_WORKAROUND            (0)
#endif

/* This needs to match SIOB_0_DCXS_CFG.FIXED_RATE for MADR */
#define BVDC_P_MADR_VARIABLE_RATE              (0)

#if ((BVDC_P_SUPPORT_MADR) && (BVDC_P_SUPPORT_MADR_VER < 5))
/* HW7425-820: HSIZE needs to be multiple of 4.*/
#define BVDC_P_MADR_HSIZE_WORKAROUND           (1)
#endif

#if ((BVDC_P_SUPPORT_MADR) && (BVDC_P_SUPPORT_MADR_VER < 4))
/* HW7425-869: SIOB QM capture doesn't end data transmission
 * correctly when (picture_width/4)*(picture_height+1)%40 > 31 */
#define BVDC_P_MADR_PICSIZE_WORKAROUND         (1)
#endif

#if (BVDC_P_SUPPORT_STG && BVDC_P_SUPPORT_MADR && (BVDC_P_SUPPORT_MADR_VER < 6))
/* HW7425-1244/1255 MADR scb cycle incomplete */
#define BVDC_P_STG_RUL_DELAY_WORKAROUND        (1)
#endif

#if (BVDC_P_SUPPORT_STG && (BVDC_P_SUPPORT_STG_VER < 5))
/* HW7425-1429 STG repeat polarity trigger dependent on STC */
#define BVDC_P_STG_NRT_CADENCE_WORKAROUND        (1)
#endif
#if ((BVDC_P_SUPPORT_MFD_VER >= 12) && \
     (BVDC_P_SUPPORT_MFD_VER <= 13))
/* HW7425-1404: BVN MVFD: YC unpack flow control issue */
#define BVDC_P_MVFD_ALIGNMENT_WORKAROUND       (1)
#endif

#if (BVDC_P_MADR_PICSIZE_WORKAROUND)
#define BVDC_P_MADR_GET_REMAINDER(ulWidth, ulHeight)  \
	( ((ulWidth*(ulHeight+1))/4) % 40 )
#define BVDC_P_MADR_BAD_ALIGNMENT(ulRemainder)   \
	( ulRemainder > 31 )

#if (BVDC_P_MADR_VARIABLE_RATE)
#define BVDC_P_MADR_GET_VARIABLE_RATE_REMAINDER(ulWidth, ulHeight, bpp)   \
	( (ulWidth*ulHeight*bpp + 128) % 2048 )

#define BVDC_P_MADR_VARIABLE_RATE_BAD_ALIGNMENT(ulRemainder, bpp)   \
	( !((ulRemainder==0) || (ulRemainder > (128+bpp))) )

#else
#define BVDC_P_MADR_GET_FIX_RATE_REMAINDER(ulWidth, ulHeight, bpp)   \
	( (ulWidth*ulHeight*bpp) % 2048 )

#define BVDC_P_MADR_FIX_RATE_BAD_ALIGNMENT(ulRemainder, bpp)   \
	( !((ulRemainder==0) || (ulRemainder > bpp)) )

#endif

#endif

#define BVDC_P_NUM_MOSAIC_CSC_TYPE             (6)


/***************************************************************************
 * VDC Internal enums
 ***************************************************************************/
/* capture trigger type */
typedef enum
{
	BVDC_P_CapTriggerType_eDisable = 0,
	BVDC_P_CapTriggerType_eBvbField,
	BVDC_P_CapTriggerType_eLineCompare
} BVDC_P_CapTriggerType;

/* source chroma type for MAD chroma config */
typedef enum
{
	BVDC_P_ChromaType_eChroma422 = 0,
	BVDC_P_ChromaType_eField420,
	BVDC_P_ChromaType_eFrame420,
	BVDC_P_ChromaType_eAuto = 7
} BVDC_P_ChromaType;

/* source YCbCr color space */
typedef enum
{
	BVDC_P_VdecColorSpace_eHdYCrCb,         /* BT.709 */
	BVDC_P_VdecColorSpace_eNtscSdYCrCb = 0, /* BT.601 */
	BVDC_P_VdecColorSpace_ePalSdYCrCb = 0,  /* BT.470 PAL */
	BVDC_P_VdecColorSpace_eFcc,             /* legacy NTSC (1953) */
	BVDC_P_VdecColorSpace_eSmpte_240M,      /* legacy HD(1987) */
	BVDC_P_VdecColorSpace_eYuv,             /* YUV BT.601 */
	BVDC_P_VdecColorSpace_eSdRgb,           /* RGB BT.601 */
	BVDC_P_VdecColorSpace_eSdYPrPb,         /* YPrPb BT.601 */
	BVDC_P_VdecColorSpace_eHdRgb,           /* RGB BT.709 */
	BVDC_P_VdecColorSpace_eHdYPrPb,         /* YPrPb BT.709 */

	BVDC_P_VdecColorSpace_eMax
} BVDC_P_VdecColorSpace;

/* source matrix coefficents */
typedef enum
{
	BVDC_P_MatrixCoefficients_eHdmi_RGB,
	BVDC_P_MatrixCoefficients_eItu_R_BT_709,
	BVDC_P_MatrixCoefficients_eUnknown,
	BVDC_P_MatrixCoefficients_eDvi_Full_Range_RGB,
	BVDC_P_MatrixCoefficients_eFcc,
	BVDC_P_MatrixCoefficients_eItu_R_BT_470_2_BG,
	BVDC_P_MatrixCoefficients_eSmpte_170M,
	BVDC_P_MatrixCoefficients_eSmpte_240M,
	BVDC_P_MatrixCoefficients_eXvYcc_709,
	BVDC_P_MatrixCoefficients_eXvYcc_601,

	BVDC_P_MatrixCoefficients_eMax
} BVDC_P_MatrixCoefficients;

/* internal cmp csc types */
typedef enum BVDC_P_CmpColorSpace
{
	BVDC_P_CmpColorSpace_eHdYCrCb,       /* BT.709 */
	BVDC_P_CmpColorSpace_eNtscSdYCrCb,	 /* BT.601 */
	BVDC_P_CmpColorSpace_ePalSdYCrCb,	 /* BT.470 PAL */
	BVDC_P_CmpColorSpace_eXvYcc_601,
	BVDC_P_CmpColorSpace_eFcc,			 /* legacy NTSC (1953) */
	BVDC_P_CmpColorSpace_eSmpte_240M,    /* legacy HD(1987) */
	BVDC_P_CmpColorSpace_eCustom,        /* custom panels */
	BVDC_P_CmpColorSpace_eMax,
	BVDC_P_CmpColorSpace_eInvalid = BVDC_P_CmpColorSpace_eMax
} BVDC_P_CmpColorSpace;

/* Example modes:
 *
 * (A) [src] -> [...] -> [   ] -> [   ] -> [dst]  no capture nor scaler.
 * (B) [src] -> [scl] -> [   ] -> [   ] -> [dst]  scaler_only or bypass.
 * (C) [src] -> [cap] -> [plk] -> [scl] -> [dst]  capture_before_scaler.
 * (D) [src] -> [scl] -> [cap] -> [plk] -> [dst]  scaler_before_capture.
 * (E) [src] -> [mad] -> [   ] -> [   ] -> [dst]  mad_only.
 * (F) [src] -> [cap] -> [plk] -> [mad] -> [dst]  capture_before_mad.
 * (G) [src] -> [cap] -> [plk] -> [   ] -> [dst]  capture_only.
 *
 * (A) Where we have the source go directly to a compositor.  This mode
 * is mainly for debugging purposes only. This is realized when all vnet
 * mode bits are 0.
 *
 * Currently we have the following conventions:
 * (1). MAD is always before SCL
 * (2). ANR and DNR are always before MAD
 * (3). For ananlog input source, ANR is before DNR; For digital input
 *      source, DNR is before ANR. This follows LIFO rule according to
 *      noise adding and removing.
 */
typedef union
{
	struct
	{
		/* sub-module usage bits */
		uint32_t  bUseCap                : 1;  /* bit 0 */
		uint32_t  bUseScl                : 1;
		uint32_t  bUseHscl               : 1;
		uint32_t  bUseMad                : 1;
		uint32_t  bUseDnr                : 1;  /* bit 4 */
		uint32_t  bUseAnr                : 1;
		uint32_t  bUseMcvp               : 1;
		uint32_t  bUseFgt                : 1;

		uint32_t  bUseHist               : 1;  /* bit 8 */
		uint32_t  bUseHistAtSrc          : 1;

		/* sub-modules order bits */
		uint32_t  bSclBeforeCap          : 1;

		/* bit indicate invalid vnet */
		uint32_t  bInvalid               : 1;

		uint32_t  bUseMvpBypass          : 1;  /* bit 12 */
	} stBits;

	uint32_t aulInts[BVDC_P_DIRTY_INT_ARRAY_SIZE];
} BVDC_P_VnetMode;


/* Uses to index some of the internal table, care must be taken when changing
 * the enum. */
typedef enum
{
	/* Video Window on compositor 0 (Primary) */
	BVDC_P_WindowId_eComp0_V0 = 0,
	BVDC_P_WindowId_eComp0_V1,

	/* Video Window on compositor 1 (Secondary) */
	BVDC_P_WindowId_eComp1_V0,
	BVDC_P_WindowId_eComp1_V1,

	/* Video Window Compositor Bypass */
	BVDC_P_WindowId_eComp2_V0,

	/* Vice */
	BVDC_P_WindowId_eComp3_V0,
	BVDC_P_WindowId_eComp4_V0,
	BVDC_P_WindowId_eComp5_V0,
	BVDC_P_WindowId_eComp6_V0,

	/* Gfx Window on compositor 0 (Primary) */
	BVDC_P_WindowId_eComp0_G0,
	BVDC_P_WindowId_eComp0_G1,
	BVDC_P_WindowId_eComp0_G2,

	/* Gfx Window on compositor 1 (Secondary) */
	BVDC_P_WindowId_eComp1_G0,

	/* Gfx Window on compositor 2 (Tertiary) */
	BVDC_P_WindowId_eComp2_G0,

	/* Gfx Window on compositor 3 (Vice) */
	BVDC_P_WindowId_eComp3_G0,
	BVDC_P_WindowId_eComp4_G0,
	BVDC_P_WindowId_eComp5_G0,
	BVDC_P_WindowId_eComp6_G0,

	/* Must be last */
	BVDC_P_WindowId_eUnknown
} BVDC_P_WindowId;

typedef enum
{
	BVDC_P_FeederId_eMfd0 = 0,
	BVDC_P_FeederId_eMfd1,
	BVDC_P_FeederId_eMfd2,
	BVDC_P_FeederId_eMfd3,
	BVDC_P_FeederId_eMfd4,
	BVDC_P_FeederId_eMfd5,
	BVDC_P_FeederId_eVfd0,
	BVDC_P_FeederId_eVfd1,
	BVDC_P_FeederId_eVfd2,
	BVDC_P_FeederId_eVfd3,
	BVDC_P_FeederId_eVfd4,
	BVDC_P_FeederId_eVfd5,
	BVDC_P_FeederId_eVfd6,
	BVDC_P_FeederId_eVfd7,
	BVDC_P_FeederId_eUnknown
} BVDC_P_FeederId;

typedef enum
{
	BVDC_P_CaptureId_eCap0 = 0,
	BVDC_P_CaptureId_eCap1,
	BVDC_P_CaptureId_eCap2,
	BVDC_P_CaptureId_eCap3,
	BVDC_P_CaptureId_eCap4,
	BVDC_P_CaptureId_eCap5,
	BVDC_P_CaptureId_eCap6,
	BVDC_P_CaptureId_eCap7,
	BVDC_P_CaptureId_eUnknown
} BVDC_P_CaptureId;

typedef enum
{
	BVDC_P_ScalerId_eScl0 = 0,
	BVDC_P_ScalerId_eScl1,
	BVDC_P_ScalerId_eScl2,
	BVDC_P_ScalerId_eScl3,
	BVDC_P_ScalerId_eScl4,
	BVDC_P_ScalerId_eScl5,
	BVDC_P_ScalerId_eScl6,
	BVDC_P_ScalerId_eScl7,
	BVDC_P_ScalerId_eUnknown
} BVDC_P_ScalerId;

typedef enum
{
	BVDC_P_MadId_eMad0,
	BVDC_P_MadId_eMad1,
	BVDC_P_MadId_eUnknown
} BVDC_P_MadId;

typedef enum
{
	BVDC_P_HscalerId_eHscl0 = 0,
	BVDC_P_HscalerId_eHscl1 ,
	BVDC_P_HscalerId_eHscl2 ,
	BVDC_P_HscalerId_eHscl3 ,
	BVDC_P_HscalerId_eHscl4 ,
	BVDC_P_HscalerId_eUnknown
} BVDC_P_HscalerId;

typedef enum
{
	BVDC_P_MudId_eMud0,
	BVDC_P_MudId_eUnknown
} BVDC_P_MudId;

typedef enum
{
	BVDC_P_DnrId_eDnr0 = 0,
	BVDC_P_DnrId_eDnr1,
	BVDC_P_DnrId_eDnr2,
	BVDC_P_DnrId_eDnr3,
	BVDC_P_DnrId_eDnr4,
	BVDC_P_DnrId_eUnknown
} BVDC_P_DnrId;

typedef enum
{
	BVDC_P_AnrId_eAnr0 = 0,
	BVDC_P_AnrId_eAnr1,
	BVDC_P_AnrId_eUnknown
} BVDC_P_AnrId;

typedef enum
{
	BVDC_P_McdiId_eMcdi0 = 0,
	BVDC_P_McdiId_eMcdi1,
	BVDC_P_McdiId_eMcdi2,
	BVDC_P_McdiId_eMcdi3,
	BVDC_P_McdiId_eMcdi4,
	BVDC_P_McdiId_eUnknown
} BVDC_P_McdiId;

typedef enum
{
	BVDC_P_McvpId_eMcvp0 = 0,
	BVDC_P_McvpId_eMcvp1,
	BVDC_P_McvpId_eMcvp2,
	BVDC_P_McvpId_eMcvp3,
	BVDC_P_McvpId_eMcvp4,
	BVDC_P_McvpId_eUnknown
} BVDC_P_McvpId;

typedef enum
{
	BVDC_P_BoxDetectId_eBoxDetect0,
	BVDC_P_BoxDetectId_eBoxDetect1,
	BVDC_P_BoxDetectId_eBoxDetect2,
	BVDC_P_BoxDetectId_eBoxDetect3,
	BVDC_P_BoxDetectId_eBoxDetect4,
	BVDC_P_BoxDetectId_eBoxDetect5,
	BVDC_P_BoxDetectId_eUnknown
} BVDC_P_BoxDetectId;

typedef enum
{
	BVDC_P_FgtId_eFgt0 = 0,
	BVDC_P_FgtId_eUnknown
} BVDC_P_FgtId;

typedef enum
{
	BVDC_P_HistId_eHist0 = 0,
	BVDC_P_HistId_eUnknown
} BVDC_P_HistId;

typedef enum
{
	BVDC_P_FreeChId_eCh0 = 0,
	BVDC_P_FreeChId_eCh1,
	BVDC_P_FreeChId_eCh2,
	BVDC_P_FreeChId_eCh3,
	BVDC_P_FreeChId_eCh4,
	BVDC_P_FreeChId_eCh5,
	BVDC_P_FreeChId_eCh6,
	BVDC_P_FreeChId_eCh7,
	BVDC_P_FreeChId_eCh8,
	BVDC_P_FreeChId_eCh9,
	BVDC_P_FreeChId_eCh10,
	BVDC_P_FreeChId_eCh11,
	BVDC_P_FreeChId_eCh12,
	BVDC_P_FreeChId_eCh13,
	BVDC_P_FreeChId_eUnknown
} BVDC_P_FreeChId;

typedef enum
{
	BVDC_P_LpBckId_eLp0 = 0,
	BVDC_P_LpBckId_eLp1,
	BVDC_P_LpBckId_eLp2,
	BVDC_P_LpBckId_eLp3,
	BVDC_P_LpBckId_eLp4,
	BVDC_P_LpBckId_eLp5,
	BVDC_P_LpBckId_eLp6,
	BVDC_P_LpBckId_eLp7,
	BVDC_P_LpBckId_eLp8,
	BVDC_P_LpBckId_eLp9,
	BVDC_P_LpBckId_eUnknown
} BVDC_P_LpBckId;

typedef enum
{
	BVDC_P_DrainFrnId_eDrain0 = 0,
	BVDC_P_DrainFrnId_eDrain1,
	BVDC_P_DrainFrnId_eDrain2,
	BVDC_P_DrainFrnId_eDrain3,
	BVDC_P_DrainFrnId_eDrain4,
	BVDC_P_DrainFrnId_eDrain5,
	BVDC_P_DrainFrnId_eDrain6,
	BVDC_P_DrainFrnId_eUnknown
} BVDC_P_DrainFrnId;

typedef enum
{
	BVDC_P_DrainBckId_eDrain0 = 0,  /* back drain */
	BVDC_P_DrainBckId_eUnknown
} BVDC_P_DrainBckId;

typedef enum
{
	BVDC_P_DrainCrcId_eCrc0 = 0,  /* crc drain */
	BVDC_P_DrainCrcId_eUnknown
} BVDC_P_DrainCrcId;

typedef enum
{
	BVDC_P_PcPllId_ePll0 = 0,
	BVDC_P_PcPllId_eUnknown
} BVDC_P_PcPllId;

typedef enum
{
	/* The context is newly created and inactive for use.
	 * Next state: eActive */
	BVDC_P_State_eInactive = 0,

	/* Mark it as eCreate.  It will become eActive when apply. */
	BVDC_P_State_eCreate,

	/* Currently in use.  Normal running state.  User call destroy/apply
	 * to mark it as eDestroy. */
	BVDC_P_State_eActive,

	/* Mark for destroy, not applyed yet. Will become eShutDownPending
	 * -> eShutDown -> eInactive. This state prompt the apply to mark it
	 * as eShutDownPending */
	BVDC_P_State_eDestroy,

	/* Mark for destroy, is applyed, harware not disabled yet. Will become
	 * BVDC_P_State_eShutDownRul when a disable RUL is build. This state
	 * prompt the ISR to build disable RUL and mark it as eShutDownRul. */
	BVDC_P_State_eShutDownPending,

	/* Mark for destroy, is applyed, hardware not disabled yet. Will become
	 * eDrainVnet (for writer) or eShutDown (for reader) when hardware is
	 * disabled. This state prompt the ISR to build drain RUL (for writer)
	 * and mark it as eDrainVnet (for writer) or only to mark it as
	 * eShutDown (for reader). */
	BVDC_P_State_eShutDownRul,

	/* Mark for destroy, is applyed, hardware disabled. This state is for
	 * writer only. Will become eShutDown when vnet is drained. This state
	 * prompt the ISR to mark it as eShutDown.
	 * Note: this state is brought back due to PR 31849:  Soft Reset when
	 * CAP is Busy Corrupts Memory. We find that CAP might still have pending
	 * memory write after disabled. This could potentially a problem for
	 * other memory writers such as ANR and MAD too */
	BVDC_P_State_eDrainVnet,

	/* Mark for destroy, is applyed, and harware is disabled. Will become
	 * eInactive. */
	BVDC_P_State_eShutDown

} BVDC_P_State;

/* Rectangle structure */
typedef struct
{
	int32_t      lLeft;
	int32_t      lTop;
	uint32_t     ulWidth;
	uint32_t     ulHeight;

	/* Delta for right eye */
	int32_t      lLeft_R;

} BVDC_P_Rect;


/* Scissor clipping values.  Relative clipping values from the four
 * edges of the rectangle. */
typedef struct
{
	uint32_t     ulLeft;
	uint32_t     ulRight;
	uint32_t     ulTop;
	uint32_t     ulBottom;

	/* Delta for right eye */
	int32_t      lLeftDelta_R;

} BVDC_P_ClipRect;

/* Supported features of vdc; number of compositor/sources/windows */
typedef struct
{
	bool         bCmpBIsBypass;
	bool         abAvailCmp[BVDC_P_MAX_COMPOSITOR_COUNT];
	bool         abAvailSrc[BVDC_P_MAX_SOURCE_COUNT];
	bool         ab3dSrc[BVDC_P_MAX_SOURCE_COUNT];
} BVDC_P_Features;

/* RDC list information.  Since we were going to do a lot of BRDC_List_Get_*
 * and set BRDC_List_Set_*, this would save all those call, and just have
 * the main initial _isr call it and store it in this struct, and call a set
 * when every is done building the RUL. */
typedef struct
{
	bool         bLastExecuted;
	/* mosaic mode could build chained slave RULs, which shouldn't overwrite
	   timestamp captured by master RUL; otherwise, bMasterList is true,
	   and timestamp readback takes place in the RUL; */
	bool         bMasterList;
	uint32_t    *pulStart;
	uint32_t    *pulCurrent;

	/* PsF: mark the playback RUL size; if ISR is missed, chop the RUL size
	   to not to scanout consecutive 1080p from MFD and capture; */
	uint32_t     ulPsfMark;
} BVDC_P_ListInfo;

/* 4x3 Color Conversion Matrix. two's complement fixed point values.
 * Precision see bvdc_csc_priv.h. */
typedef struct
{
	uint16_t     usY0;
	uint16_t     usY1;
	uint16_t     usY2;
	uint16_t     usYAlpha;
	uint16_t     usYOffset;

	uint16_t     usCb0;
	uint16_t     usCb1;
	uint16_t     usCb2;
	uint16_t     usCbAlpha;
	uint16_t     usCbOffset;

	uint16_t     usCr0;
	uint16_t     usCr1;
	uint16_t     usCr2;
	uint16_t     usCrAlpha;
	uint16_t     usCrOffset;

	uint16_t     usCxIntBits;    /* integer fixed point precision for coefficients */
	uint16_t     usCxFractBits;  /* fraction fixed point precision for coefficients */

	uint16_t     usCoIntBits;    /* integer fixed point precision for offset */
	uint16_t     usCoFractBits;  /* fraction fixed point precision for offset */

	uint16_t     usCoVideoBits;  /* video bit format for offset */

} BVDC_P_CscCoeffs;

/* Use for dynamic RTS / Format Table of VDEC. */
typedef struct
{
	uint32_t                        ulAddr;
	uint32_t                        ulData;

} BVDC_P_AddrDataPair;

/* Use for Dither */
typedef struct
{
	/* xxx_DITHER_LFSR_INIT */
	uint32_t                        ulLfsrSeq;
	uint32_t                        ulLfsrValue;
	/* xxx_DITHER_LFSR_CTRL */
	uint32_t                        ulLfsrCtrlT0;
	uint32_t                        ulLfsrCtrlT1;
	uint32_t                        ulLfsrCtrlT2;
	/* xxx_DITHER_CTRL */
	uint32_t                        ulMode;
	uint32_t                        ulCh0Offset;
	uint32_t                        ulCh0Scale;
	uint32_t                        ulCh1Offset;
	uint32_t                        ulCh1Scale;
	uint32_t                        ulCh2Offset;
	uint32_t                        ulCh2Scale;

} BVDC_P_DitherSetting;


/* Use for buffer TestFeature1  */
typedef struct
{
	bool                            bEnable;
	uint32_t                        ulBitsPerPixel;

	uint32_t                        ulPixelPerGroup;
	uint32_t                        ulBitsPerGroup;
	uint32_t                        ulPredictionMode;

} BVDC_P_TestFeature1_Settings;

/***************************************************************************
 * VDC Internal macros
 ***************************************************************************/
/* Not to use a parameter. */
#define BVDC_P_UNUSED(x)                    BSTD_UNUSED(x)

#define BVDC_P_ALIGN_UP(addr, alignment) \
	(((uint32_t)(addr) + (alignment) - 1) & ~((alignment) - 1))

#define BVDC_P_ALIGN_DN(addr, alignment) \
	(((uint32_t)(addr)) & ~((alignment) - 1))

#define BVDC_P_IS_ALIGN(addr, alignment) \
	(((uint32_t)(addr)) == BVDC_P_ALIGN_DN((addr), (alignment)))

#define BVDC_P_DIV_ROUND_UP(a, b) \
	(((a) + ((b) - 1)) / (b))

#define BVDC_P_DIV_ROUND_DN(a, b) \
	((a) / (b))

#define BVDC_P_DIV_ROUND_NEAR(a, b) \
	(((a) + ((b)>>1)) / (b))

/* This macro test state of handle */
#define BVDC_P_STATE_IS_INACTIVE(handle) \
	((NULL != (handle)) && (BVDC_P_State_eInactive==(handle)->eState))

#define BVDC_P_STATE_IS_ACTIVE(handle) \
	((NULL != (handle)) && (BVDC_P_State_eActive   ==(handle)->eState))

#define BVDC_P_STATE_IS_CREATE(handle) \
	((NULL != (handle)) && (BVDC_P_State_eCreate   ==(handle)->eState))

#define BVDC_P_STATE_IS_DESTROY(handle) \
	((NULL != (handle)) && (BVDC_P_State_eDestroy  ==(handle)->eState))

#define BVDC_P_STATE_IS_SHUTDOWNPENDING(handle) \
	((NULL != (handle)) && (BVDC_P_State_eShutDownPending ==(handle)->eState))

#define BVDC_P_STATE_IS_SHUTDOWNRUL(handle) \
	((NULL != (handle)) && (BVDC_P_State_eShutDownRul ==(handle)->eState))

#define BVDC_P_STATE_IS_SHUTDOWN(handle) \
	((NULL != (handle)) && (BVDC_P_State_eShutDown  ==(handle)->eState))

	 /* Take two rectangle pointers and compare src to dst. */
#define BVDC_P_RECT_CMP_EQ(dst, src) \
	(((src)->lLeft   ==(dst)->lLeft   ) && \
	 ((src)->lTop    ==(dst)->lTop    ) && \
	 ((src)->ulWidth ==(dst)->ulWidth ) && \
	 ((src)->ulHeight==(dst)->ulHeight) && \
	 ((src)->lLeft_R ==(dst)->lLeft_R ))

#define BVDC_P_CLIP_RECT_CMP_EQ(dst, src) \
	(((src)->ulLeft  ==(dst)->ulLeft  ) && \
	 ((src)->ulRight ==(dst)->ulRight ) && \
	 ((src)->ulTop   ==(dst)->ulTop   ) && \
	 ((src)->ulBottom==(dst)->ulBottom) && \
	 ((src)->lLeftDelta_R ==(dst)->lLeftDelta_R))

/* Get the next field/frame polarity. */
#define BVDC_P_NEXT_POLARITY(polarity) \
	((BAVC_Polarity_eTopField==(polarity)) ? BAVC_Polarity_eBotField \
	:(BAVC_Polarity_eBotField==(polarity)) ? BAVC_Polarity_eTopField \
	:BAVC_Polarity_eFrame)

/* Get the min / max of two numbers */
#define BVDC_P_MIN(a, b)        (((a) < (b)) ? (a) : (b))
#define BVDC_P_MAX(a, b)        (((a) > (b)) ? (a) : (b))

/* Compare value a == (b +/- delta) */
#define BVDC_P_EQ_DELTA(value_a, value_b, delta) \
	(((value_a) <= ((value_b) + (delta))) && \
	 ((value_b) <= ((value_a) + (delta))))

/* Round off given a threshold and a precision */
#define BVDC_P_ROUND_OFF(a, threshold, precision)  \
	(((a) + (threshold)) / (precision))

/*
 * Build RUL for read/modify/write a register.
 */
#define BVDC_P_RD_MOD_WR_RUL(pulCurrent, ulAndMask, ulOrMask, ulRegAddr) \
do { \
	/* Set up AND/OR mask */ \
	*(pulCurrent)++ = BRDC_OP_IMM_TO_VAR(BRDC_Variable_0); \
	*(pulCurrent)++ = (ulAndMask); \
	*(pulCurrent)++ = BRDC_OP_IMM_TO_VAR(BRDC_Variable_1); \
	*(pulCurrent)++ = (ulOrMask); \
	/* read/modify/write */ \
	*(pulCurrent)++ = BRDC_OP_REG_TO_VAR(BRDC_Variable_2); \
	*(pulCurrent)++ = BRDC_REGISTER(ulRegAddr); \
	*(pulCurrent)++ = BRDC_OP_VAR_AND_VAR_TO_VAR(BRDC_Variable_2, BRDC_Variable_0, BRDC_Variable_2); \
	*(pulCurrent)++ = BRDC_OP_VAR_OR_VAR_TO_VAR (BRDC_Variable_2, BRDC_Variable_1, BRDC_Variable_2); \
	*(pulCurrent)++ = BRDC_OP_VAR_TO_REG(BRDC_Variable_2); \
	*(pulCurrent)++ = BRDC_REGISTER(ulRegAddr); \
} while (0)

/* Build a imm write instruction. */
#define BVDC_P_BUILD_IMM_WR(pulCurrent, ulAddr, ulValue) \
do { \
	*(pulCurrent)++ = BRDC_OP_IMM_TO_REG(); \
	*(pulCurrent)++ = BRDC_REGISTER(ulAddr); \
	*(pulCurrent)++ = (ulValue); \
} while(0)

/* Add in the no ops (real no-op intruction does not work correctly). */
#define BVDC_P_BUILD_NO_OPS(pulCurrent) \
	BVDC_P_BUILD_IMM_WR(pulCurrent, BVDC_P_SCRATCH_WO_REG, 0xbaadf00d);

/* Add in the a force trigger. */
#define BVDC_P_BUILD_IMM_EXEC_OPS(pulCurrent, ulImmTriggerAddr) \
	BVDC_P_BUILD_IMM_WR(pulCurrent, ulImmTriggerAddr, 1);

/* move reg to reg or reg copy*/
#define BVDC_P_BUILD_REG_COPY_OPS(pulCurrent, ulSrcAddr, ulDstAddr) \
do {\
	*(pulCurrent)++ = BRDC_OP_REG_TO_REG(1); \
	*(pulCurrent)++ = BRDC_REGISTER(ulSrcAddr); \
	*(pulCurrent)++ = BRDC_REGISTER(ulDstAddr); \
} while(0)

/* Add in the reset for a BVN module!  Reset procedure is:
 * Write 1 and then 0 to reset. */
#define BVDC_P_BUILD_RESET(pulCurrent, ulResetAddr, ulResetMask) \
do { \
	*(pulCurrent)++ = BRDC_OP_IMM_TO_REG(); \
	*(pulCurrent)++ = BRDC_REGISTER(ulResetAddr); \
	*(pulCurrent)++ = (ulResetMask); \
	*(pulCurrent)++ = BRDC_OP_IMM_TO_REG(); \
	*(pulCurrent)++ = BRDC_REGISTER(ulResetAddr); \
	*(pulCurrent)++ = 0; \
} while(0)

/* Copy table to current list pointer */
#define BVDC_P_ADD_TO_RUL(pulCurrent, s_aulTable) \
{ \
	BKNI_Memcpy(pulCurrent, s_aulTable, sizeof(s_aulTable)); \
	pulCurrent += sizeof(s_aulTable) / sizeof(uint32_t); \
}

/***************************************************************************
 * Generic set/get/compare field
 ***************************************************************************/
/* Get register field value by register name and field name.
 *
 * This is equivalent to:
 * ulFoo = (regvar & BCHP_VDEC_MODE_TV_SYSTEM_MASK) >>
 *    BCHP_VDEC_MODE_TV_SYSTEM_SHIFT);
 *
 * Example:
 *   ulReg = BREG_Read32(hRegister, BCHP_VDEC_MODE);
 *   BVDC_P_GET_FIELD(ulReg, VDEC_MODE, TV_SYSTEM);
 *   BREG_Write32(BCHP_VDEC_MODE, &ulReg).
 *
 */
#define BVDC_P_GET_FIELD(regvar, reg, field) \
	(((regvar) & BCHP_##reg##_##field##_MASK) >> \
	BCHP_##reg##_##field##_SHIFT)

/* Compare a register field by value or name.
 * Example:
 *   ...
 *   ulReg = BREG_Read32(hRegister, BCHP_VDEC_MODE);
 *   if(BVDC_P_COMPARE_FIELD_NAME(ulReg, VDEC_MODE, TV_SYSTEM, 0))
 *   {
 *      BDBG_MSG("Input video is NTSC");
 *   }
 *or
 *   if(BVDC_P_COMPARE_FIELD_NAME(ulReg, VDEC_MODE, TV_SYSTEM, NTSC))
 *   {
 *      BDBG_MSG("Input video is NTSC");
 *   }
 */
#define BVDC_P_COMPARE_FIELD_DATA(regvar, reg, field, data) \
	(BVDC_P_GET_FIELD((regvar), reg, field)==(data))

#define BVDC_P_COMPARE_FIELD_NAME(regvar, reg, field, name) \
	(BVDC_P_GET_FIELD((regvar), reg, field)==BCHP##_##reg##_##field##_##name)

/***************************************************************************
 * VDC Internal data structures
 ***************************************************************************/
/* Forward declarations of different type of source. */
typedef struct BVDC_P_GfxFeederContext   *BVDC_P_GfxFeeder_Handle;
typedef struct BVDC_P_FeederContext      *BVDC_P_Feeder_Handle;
typedef struct BVDC_P_VdecContext        *BVDC_P_Vdec_Handle;
typedef struct BVDC_P_ScalerContext      *BVDC_P_Scaler_Handle;
typedef struct BVDC_P_MadContext         *BVDC_P_Mad_Handle;
typedef struct BVDC_P_HscalerContext     *BVDC_P_Hscaler_Handle;
typedef struct BVDC_P_DnrContext         *BVDC_P_Dnr_Handle;
typedef struct BVDC_P_AnrContext         *BVDC_P_Anr_Handle;
typedef struct BVDC_P_McvpContext        *BVDC_P_Mcvp_Handle;
typedef struct BVDC_P_McdiContext        *BVDC_P_Mcdi_Handle;
typedef struct BVDC_P_BoxDetectContext   *BVDC_P_BoxDetect_Handle;
typedef struct BVDC_P_CaptureContext     *BVDC_P_Capture_Handle;
typedef struct BVDC_P_BufferContext      *BVDC_P_Buffer_Handle;
typedef struct BVDC_P_MudContext         *BVDC_P_Mud_Handle;
typedef struct BVDC_P_VnetContext        *BVDC_P_Vnet_Handle;
typedef struct BVDC_P_HdDviContext       *BVDC_P_HdDvi_Handle;
typedef struct BVDC_P_BufferHeapContext  *BVDC_P_BufferHeap_Handle;
typedef struct BVDC_P_HistContext        *BVDC_P_Hist_Handle;
typedef struct BVDC_P_PepContext         *BVDC_P_Pep_Handle;
typedef struct BVDC_P_656InContext       *BVDC_P_656In_Handle;
typedef struct BVDC_P_ResourceContext    *BVDC_P_Resource_Handle;
typedef struct BVDC_P_DownSampleContext  *BVDC_P_DownSample_Handle;
typedef struct BVDC_P_FgtContext         *BVDC_P_Fgt_Handle;
typedef struct BVDC_P_Cm3dContext        *BVDC_P_Cm3d_Handle;
typedef struct BVDC_P_Source_Info        *BVDC_P_Source_InfoPtr;
typedef struct BVDC_P_PictureNode        *BVDC_P_PictureNodePtr;
typedef struct BVDC_P_BufferHeapNode     *BVDC_P_HeapNodePtr;
typedef struct BVDC_P_BufferHeap_Info    *BVDC_P_HeapInfoPtr;

#ifdef __cplusplus
}
#endif

#endif /* #ifndef BVDC_COMMON_PRIV_H__*/

/* End of file. */
