/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvdc_window.c $
* $brcm_Revision: Hydra_Software_Devel/335 $
* $brcm_Date: 10/15/12 2:55p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vdc/src/common/bvdc_window.c $
 * 
 * Hydra_Software_Devel/335   10/15/12 2:55p pntruong
 * SW7231-1033: Correctly toggle dirty bit for clearrect/mosaic function.
 * Isolated the SW7425-2236 work-around for mosaic only.
 *
 * Hydra_Software_Devel/334   10/5/12 4:45p yuxiaz
 * SW7425-3928: Added synclock status in window callback.
 *
 * Hydra_Software_Devel/333   8/16/12 2:45p tdo
 * SW7445-8: Create Appframework emulation environment for 7445 A0
 *
 * Hydra_Software_Devel/332   8/16/12 2:41p yuxiaz
 * SW7425-3619: Add const to read only settings.
 *
 * Hydra_Software_Devel/331   8/9/12 11:20a syang
 * SW7425-2172: add SetSurface support for MFD gfx feeding
 *
 * Hydra_Software_Devel/330   7/27/12 3:46p vanessah
 * SW7425-3150: adding Bypass Video Processing support
 *
 * Hydra_Software_Devel/329   7/12/12 4:03p pntruong
 * SW7422-40, SW7425-3479: Fixed typo in function "bvdc_window_getbuffer".
 *
 * Hydra_Software_Devel/328   6/14/12 3:57p syang
 * SW7425-2172: VFD/GFX with only cmp0 need 6 scratch regs for gfx;
 * prepare to add SetSurface support to MFD
 *
 * Hydra_Software_Devel/327   5/31/12 4:24p syang
 * SW7425-2093: handle the case that some vfd is marked as "false" for src
 * creation
 *
 * Hydra_Software_Devel/326   5/30/12 4:54p syang
 * SW7425-2093: extra ApplyChange with 1st SetSurface no longer needed;
 * vfd/cap time stamp related scratch registers now share with vfd's
 * surface manager's; added vfd resource check; more clean up
 *
 * Hydra_Software_Devel/325   5/11/12 9:24a tdo
 * SW7425-3025: VDC should fail PQ functions that aren't support on the
 * silicon.  Fix coverity issue
 *
 * Hydra_Software_Devel/324   5/10/12 5:13p tdo
 * SW7425-3025: VDC should fail PQ functions that aren't support on the
 * silicon
 *
 * Hydra_Software_Devel/323   4/10/12 11:35a pntruong
 * SW7435-80: Reduced BVDC_Open-time allocations from BMEM based on
 * maximum usage.
 *
 * Hydra_Software_Devel/322   3/30/12 3:35p pntruong
 * SW7420-2286: Added tri-state option for pq policies for mad.
 *
 * Hydra_Software_Devel/321   3/8/12 11:52a yuxiaz
 * SW7425-2546: Fixed warning msg.
 *
 * Hydra_Software_Devel/320   3/2/12 2:24p yuxiaz
 * SW7425-2526: Added runtime query capabilities for compositor, window
 * and display.
 *
 * Hydra_Software_Devel/319   12/2/11 9:34a yuxiaz
 * SW7425-968, SW7344-95: Fixed compile warning and coverity errors.
 *
 * Hydra_Software_Devel/318   12/1/11 4:34p yuxiaz
 * SW7425-968, SW7344-95: Merged into mainline.: added independent source
 * clipping of right window in 3D mode.
 *
 * Hydra_Software_Devel/317   11/30/11 5:36p yuxiaz
 * SW7425-1182: Set default 8bit 422 format based on LE or BE.
 *
 * Hydra_Software_Devel/316   11/23/11 4:24p tdo
 * SW7435-9: Add support for CMP4-5, GFD4-5, MFD3, VFD5
 *
 * Hydra_Software_Devel/315   10/27/11 4:29p syang
 * SW7425-1429: move per vsync call back from window to display
 *
 * Hydra_Software_Devel/314   10/24/11 4:29p syang
 * SW7425-1429: check in after debug
 *
 * Hydra_Software_Devel/313   10/19/11 3:30p yuxiaz
 * SW7425-1182: Merge into mainline: Add support for more pixel formats
 * for surfaces accessed by CAP/MFD
 *
 * Hydra_Software_Devel/SW7425-1182/1   10/18/11 3:18p yuxiaz
 * SW7425-1182: Clean up packing_type settings.
 *
 * Hydra_Software_Devel/312   9/8/11 10:21a syang
 * SW7425-1265: honor back to back new setting
 *
 * Hydra_Software_Devel/311   8/31/11 5:39p pntruong
 * SW7425-973:  Implement bandwidth restriction for madr.
 *
 * Hydra_Software_Devel/310   8/26/11 5:39p syang
 * SW7425-1170: merge from branch SW7572-1170
 *
 * Hydra_Software_Devel/SW7425-1170/2   8/26/11 4:22p syang
 * SW7425-1170: refactor pixel aspect ratio related code
 *
 * Hydra_Software_Devel/SW7425-1170/1   8/24/11 6:56p vanessah
 * SW7425-1170: ASP code clean
 *
 * Hydra_Software_Devel/309   8/25/11 11:03a pntruong
 * SW7425-1190, SW7425-630: Improved documentation and renamed parameter
 * to better fit functionality.
 *
 * Hydra_Software_Devel/308   8/23/11 12:06p pntruong
 * SW7425-630: Corrected the get BVDC_Window_GetDstRightRect.
 *
 * Hydra_Software_Devel/307   8/5/11 8:25p albertl
 * SW7425-745: Added blanking to compositor color for all outputs and all
 * colorspaces.
 *
 * Hydra_Software_Devel/306   7/22/11 11:31a vanessah
 * SW7425-923: remove ANR restriction
 *
 * Hydra_Software_Devel/305   6/2/11 11:24a vanessah
 * SW7425-630: Further fix API and setDisplaySize CurInfo NewInfo
 * confusion.
 *
 * Hydra_Software_Devel/304   1/4/11 4:08p yuxiaz
 * SW7422-174: Refactor DCX macros.
 *
 * Hydra_Software_Devel/303   12/23/10 5:13p tdo
 * SW7422-148: Refactor MCVP to address chips w/wo MCTF
 *
 * Hydra_Software_Devel/302   12/7/10 2:42p tdo
 * SW7422-42:  Add BVDC_Display_GetOrientation function
 *
 * Hydra_Software_Devel/301   11/11/10 12:44p tdo
 * SW7422-13: Only turn on ANR when mode is set to enable
 *
 * Hydra_Software_Devel/300   11/10/10 4:37p albertl
 * SW7550-610: Extended BVDC_Window_SetCoefficientIndex to work for
 * graphics windows.
 *
 * Hydra_Software_Devel/299   10/29/10 3:02p tdo
 * SW7422-40: Update GetBuffer() capability in VDC with 3D case
 *
 * Hydra_Software_Devel/298   10/28/10 6:15p tdo
 * SW7422-42:  Update compositor 3D programming.
 *
 * Hydra_Software_Devel/297   10/20/10 4:11p yuxiaz
 * SW7420-1190: Put back generic VDC drian buffer for mosaic mode. Only
 * allocate drain buffer in window if it does not use main VDC heap.
 *
 * Hydra_Software_Devel/296   10/19/10 4:07p yuxiaz
 * SW7420-1190: Make mosaic scratch buffer to be per window base.
 *
 * Hydra_Software_Devel/295   10/18/10 11:00a yuxiaz
 * SW7422-39: Added BAVC_SourceId_eMpeg2 support in VDC.
 *
 * Hydra_Software_Devel/294   10/15/10 2:44p pntruong
 * SW7422-52: Fixed bug that not allow mad to be turned off.
 *
 * Hydra_Software_Devel/293   10/11/10 12:51p jessem
 * SW7420-173: Added support for VFD as source feature.
 *
* Hydra_Software_Devel/292   9/28/10 10:56p tdo
* SW7425-33: VDC: Add the 4th Compositor Support for 7422/7425
*
* Hydra_Software_Devel/291   9/3/10 4:04p vanessah
* SW7420-52:  autotest_vbi, vdc_test error
*
* Hydra_Software_Devel/290   9/3/10 2:21p vanessah
* SW7422-52:  MAD + MCVP on the same chip, coverity and check error
*
* Hydra_Software_Devel/287   8/26/10 8:08p albertl
* SW7405-4556, SW7405-4515: Changed RGB attenuation to use inverse of
* user matrix C.
*
* Hydra_Software_Devel/286   8/26/10 4:27p vanessah
* SW7420-965:  Game mode enum consistency
*
* Hydra_Software_Devel/285   8/26/10 2:37p vanessah
* SW7125-556: SW7420-965:  Support for MAD 4 fields 1 delay and MCVP game
* mode
*
* Hydra_Software_Devel/SW7420-965/1   8/20/10 1:29p vanessah
* SW7420-965:  MCVP game mode support
*
* Hydra_Software_Devel/284   8/2/10 5:02p tdo
* SW3548-2817, SW3548-2707: Add more error messages
*
* Hydra_Software_Devel/283   7/27/10 5:21p tdo
* SW3548-2817, SW3548-2707: More bug fixes
*
* Hydra_Software_Devel/282   7/16/10 11:12a tdo
* SW3548-2817, SW3548-2707: More bug fixes
*
* Hydra_Software_Devel/281   7/13/10 1:43p tdo
* SW3548-2817, SW3548-2707: take into account bInterpolateTables flag
* when checking for user errors
*
* Hydra_Software_Devel/280   7/1/10 2:16p tdo
* SW3548-2817, SW3548-2707: more fine tuning
*
* Hydra_Software_Devel/279   7/1/10 7:56a tdo
* SW3548-2817, SW3548-2707: Fix coverity error
*
* Hydra_Software_Devel/278   6/30/10 9:57p tdo
* SW3548-2817, SW3548-2707: Add validation code for invalid data from
* user tables
*
* Hydra_Software_Devel/277   6/18/10 4:24p rpan
* SW7400-2763: New BVN resource management.
*
* Hydra_Software_Devel/276   6/17/10 5:37p hongtaoz
* SW3548-2912: added support for pseudo-1080i input for 3DTV; allow API
* to disable SCL vertical phase shift;
*
 * Hydra_Software_Devel/275   5/7/10 7:21p albertl
 * SW7125-364: Changed dirty bits to use union structure to avoid type-pun
 * warnings
 *
 * Hydra_Software_Devel/274   4/19/10 10:24p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic. Move
 * BDBG_OBJECT_ID_DECLARE private header files instead of .c.
 *
 * Hydra_Software_Devel/273   4/7/10 11:38a tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic.  Rename TLA
 *
 * Hydra_Software_Devel/272   4/5/10 4:18p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic
 *
 * Hydra_Software_Devel/271   3/25/10 4:09p pntruong
 * SW3548-2858: Need looks at all changed fields.
 *
 * Hydra_Software_Devel/270   2/25/10 4:09p yuxiaz
 * SW7405-1601: VDC should expose adjusted scale-out rects after aspect
 * ratio cut-out
 *
 * Hydra_Software_Devel/269   2/11/10 11:00a pntruong
 * SW7405-3829: Moved shrinkwidth to mad settings.
 *
 * Hydra_Software_Devel/268   2/1/10 6:43p hongtaoz
 * SW3548-2678, SW3548-2680, SW3548-2681: added HDMI1.4 3D formats support
 * for 3548; added multi-buffer low delay mode support for 1to2 and 2to5
 * rate gap cases; reduced capture buffer allocation for interlaced
 * pulldown case;
 *
 * Hydra_Software_Devel/BCM3D/1   12/17/09 6:09p hongtaoz
 * SW3548-2678: added game mode delay support for 1to2 and 2to5 src/disp
 * rate ratioes;
 *
 * Hydra_Software_Devel/267   1/28/10 11:11a syang
 * SW7405-3829: add option for app to shrink src width to use deinterlace
 *
 * Hydra_Software_Devel/266   1/21/10 9:27a pntruong
 * SW3548-2676: Allowed support of 10-bits in pip path if applicable.
 *
 * Hydra_Software_Devel/265   12/23/09 2:03p tdo
 * SW7408-26: Add new TNT support
 *
 * Hydra_Software_Devel/264   12/22/09 5:15p tdo
 * SW7401-3828: Add AFD support
 *
 * Hydra_Software_Devel/263   12/17/09 2:15p yuxiaz
 * SW3548-2676: Include bvdc_feeder_priv.h
 *
 * Hydra_Software_Devel/262   12/17/09 2:02p yuxiaz
 * SW3548-2676: Only returns warning for unsupported pixel formats on old
 * chipset.
 *
 * Hydra_Software_Devel/261   12/2/09 3:05p yuxiaz
 * SW7405-3120: Added support for overlapping mosaic videos with z-
 * ordering
 *
 * Hydra_Software_Devel/260   11/16/09 5:04p yuxiaz
 * SW7405-3406: Clean up bMosaicMode and  bMosaicClearOutside.
 *
 * Hydra_Software_Devel/259   11/10/09 4:32p tdo
 * SW7400-2574: Adjust TNT based on input and output format
 *
 * Hydra_Software_Devel/258   10/26/09 4:44p pntruong
 * SW3548-1649, SW7420-22: Used new scl-cap bias function, and corrected
 * default for force capture to match rts doc.
 *
 * Hydra_Software_Devel/257   8/28/09 1:25p yuxiaz
 * SW3556-672: BVDC_Window_GetDeinterlaceDefaultConfiguration should
 * Return Dynamic Defaults.
 *
 * Hydra_Software_Devel/256   8/7/09 12:08p yuxiaz
 * PR56325: Add new API to get window sync lock status.
 *
 * Hydra_Software_Devel/255   7/30/09 10:36a pntruong
 * PR57216: Added support for scaler split screen demo modes.
 *
 * Hydra_Software_Devel/254   7/15/09 5:20p pntruong
 * PR55237: Added options to avoid black flashes on dst and/or src change.
 *
 * Hydra_Software_Devel/253   6/19/09 5:57p darnstein
 * PR55225: add support for 7342 chipset.
 *
 * Hydra_Software_Devel/252   6/15/09 5:17p pntruong
 * PR56026: Don't restrict MAD to tie to CMP_0 only.
 *
 * Hydra_Software_Devel/251   6/11/09 4:01p darnstein
 * PR55878: fill in some details for 7340 chipset.
 *
 * Hydra_Software_Devel/250   6/9/09 11:50a syang
 * PR54181: change lbox enabling WRN to MSG
 *
 * Hydra_Software_Devel/249   5/14/09 3:27p tdo
 * PR38710: Only apply blue stretch property to chipsets that support it
 *
 * Hydra_Software_Devel/248   4/28/09 1:42p rpan
 * PR47621: Minor user capture buffer implementation clean-up.
 *
 * Hydra_Software_Devel/247   4/24/09 6:40p tdo
 * PR54181: Take into account user clipping parameters in source scanout
 * rect calculation.
 *
 * Hydra_Software_Devel/246   4/7/09 6:51p tdo
 * PR53553: Only allow ANR setting to windows from compositor 0
 *
 * Hydra_Software_Devel/245   4/3/09 4:35p yuxiaz
 * PR53460: Optimal 444/422 conversion settings
 *
 * Hydra_Software_Devel/244   3/30/09 2:22p hongtaoz
 * PR53037, PR53038: Implement the VEC Locking/Alignments For Memory
 * Reduction; Implement Double-Capture-Buffer Algorithm Under VEC Locking
 * Scheme
 *
 * Hydra_Software_Devel/243   3/26/09 5:58p tdo
 * PR53553: Only allow ANR settings on main window
 *
 * Hydra_Software_Devel/242   3/24/09 3:58p tdo
 * PR53553: Set ANR dirty bit when SNR value adjusted
 *
 * Hydra_Software_Devel/241   3/19/09 11:36p pntruong
 * PR53429: Corrected the anr/mad/hscl vnet connection.  Removed ununsed
 * bAnrMadCombo flags.
 *
 * Hydra_Software_Devel/240   3/16/09 10:52p tdo
 * PR45785, PR45789: Merge from MCVP branch
 *
 * Hydra_Software_Devel/7420_mcvp/5   3/4/09 5:24p tdo
 * PR45785, PR45789: merge from main branch on 3/04/09
 *
 * Hydra_Software_Devel/239   3/4/09 4:10p tdo
 * PR51106: Support table base dynamic contrast algorithm.  Change
 * precision of level threshold for histogram data.
 *
 * Hydra_Software_Devel/7420_mcvp/4   2/26/09 11:18p tdo
 * PR 45785, PR 45789: merge from main branch on 2/26/09
 *
 * Hydra_Software_Devel/238   2/26/09 6:12p yuxiaz
 * PR52287: 422-444 and 444-422 converters should use decimate/duplicate.
 *
 * Hydra_Software_Devel/7420_mcvp/3   2/11/09 11:31a tdo
 * PR 45785, PR 45789: merge from main branch on 2/11/09
 *
 * Hydra_Software_Devel/237   2/11/09 11:04a tdo
 * PR50391: Add smoothscaling for 7420
 *
 * Hydra_Software_Devel/7420_mcvp/2   2/5/09 10:53a syang
 * PR 45785, PR 45789: merge from main branch on 2/5/09
 *
 * Hydra_Software_Devel/236   2/3/09 7:15p tdo
 * PR51627: add VDC 7336 PI support
 *
 * Hydra_Software_Devel/7420_mcvp/1   1/23/09 11:32p syang
 * PR 45785, PR 45789: add PI support for new the new module  MCVP
 * (MCTF+MCDI)
 *
 * Hydra_Software_Devel/235   1/21/09 8:31p pntruong
 * PR51419: Corrected the pixel validation for anr.
 *
 * Hydra_Software_Devel/234   1/16/09 6:28p hongtaoz
 * PR51079: added new deinterlacer custom chroma settings;
 *
 * Hydra_Software_Devel/233   1/6/09 11:24p pntruong
 * PR49391, PR49398: Don't zero the madsettings's pinters.  It is used to
 * indicate interested default settings.
 *
 * Hydra_Software_Devel/232   1/5/09 2:06p rpan
 * PR50411: Removed the unnecessary
 * BVDC_Window_SetBandwidthEquationParams() delta value check.
 *
 * Hydra_Software_Devel/231   12/10/08 2:00p albertl
 * PR45808: Updated 7420 CSC support and reworked non-linear window and
 * CMP CSC versioning.
 *
 * Hydra_Software_Devel/230   12/2/08 1:46p yuxiaz
 * PR49377: Coverity Defect ID:13248 REVERSE_INULL
 *
 * Hydra_Software_Devel/229   12/1/08 7:51p pntruong
 * PR49391, PR49398: Initialized get storage to avoid unitialized usage.
 *
 * Hydra_Software_Devel/228   11/19/08 6:14p tdo
 * PR49461: Add validation checking for support window in functions to set
 * sharpness
 *
 * Hydra_Software_Devel/227   11/12/08 3:14p tdo
 * PR48642: Provide clipping rect for histo luma region
 *
 * Hydra_Software_Devel/226   11/10/08 8:22p pntruong
 * PR41898: Need to trigger change for new pixel format.
 *
 * Hydra_Software_Devel/225   11/10/08 5:01p syang
 * PR 41898: remove un-needed bools from win-context and dirty bits
 *
 * Hydra_Software_Devel/224   11/5/08 1:37p yuxiaz
 * PR48618: BVDC_Window_GetBuffer can crash if no picture present
 *
 * Hydra_Software_Devel/223   11/4/08 7:10p pntruong
 * PR48029: Added control for gain/horz/core weight factor for dejagging.
 * Enforced dejagging on only if passed threshold.
 *
 * Hydra_Software_Devel/222   10/28/08 9:15p pntruong
 * PR48028, PR48029: Allow controlling of scaler's dejagging and
 * deringing.
 *
 * Hydra_Software_Devel/221   10/24/08 2:45a albertl
 * PR48023:  Added BVDC_Display_SetDvoAttenuationRGB and
 * BVDC_Display_GetDvoAttenuationRGB, enabling CSC adjustment on DVO
 * path.  Further cleaned up CSC matrix multiplication code.
 *
 * Hydra_Software_Devel/220   10/23/08 5:52p albertl
 * PR47318, PR47814: Fixed overflow when calculating RGB attenuation
 * offsets.  Rewrote portions of RGB attenuation code and fixed various
 * offset related issues.
 *
 * Hydra_Software_Devel/219   10/18/08 12:00p jessem
 * PR 46489: Removed assert in BVDC_Windwo_SetSharpnessConfig.
 *
 * Hydra_Software_Devel/218   10/7/08 9:54a jessem
 * PR 47304, PR 46489: Added TNT interpolation table support and private
 * functions.
 *
 * Hydra_Software_Devel/217   10/3/08 11:01a yuxiaz
 * PR46305, PR46307, PR46309: Use new TestFeature1 function from BSUR.
 * Change BVDC_P_SUPPORT_VIDEO_TestFeature1 to
 * BVDC_P_SUPPORT_VIDEO_TESTFEATURE1.
 *
 * Hydra_Software_Devel/216   10/3/08 9:25a yuxiaz
 * PR46305, PR46307, PR46309: More Video TestFeature1 support.
 *
 * Hydra_Software_Devel/215   10/2/08 10:11a pntruong
 * PR47303, PR46309: Fixed warnings.
 *
 * Hydra_Software_Devel/214   10/1/08 8:33p pntruong
 * PR47303, PR46309: Reused the TestFeature1 type in surface.
 *
 * Hydra_Software_Devel/213   10/1/08 5:05p yuxiaz
 * PR47303, PR46309: Add video TestFeature1 support for GFD on 3548 B0.
 *
 * Hydra_Software_Devel/212   9/30/08 12:35p jessem
 * PR 46489: Re-added TN2T support using tighter compiler directive to
 * compile only when BVDC_P_SUPPORT_TNT_VER >= 5.
 *
 * Hydra_Software_Devel/210   9/29/08 4:38p jessem
 * PR 46489: Added TN2T support.
 *
 * Hydra_Software_Devel/209   9/26/08 3:25p yuxiaz
 * PR46305, PR46307: More video TestFeature1 support for 3548 B0.
 *
 * Hydra_Software_Devel/208   9/24/08 6:19p pntruong
 * PR47113, PR45308: Allow flexibility in selecting builtins and user
 * defined filter coefficients.
 *
 * Hydra_Software_Devel/207   9/22/08 4:53p yuxiaz
 * PR46305, PR46307, PR46309: Added video TestFeature1 support for 3548 B0.
 *
 * Hydra_Software_Devel/206   9/18/08 3:06p pntruong
 * PR46477: B0: Remove workaround dealing with VDEC/BVB hookup.
 *
 * Hydra_Software_Devel/205   8/22/08 11:50a rpan
 * PR45979: Assert if the two VBI passthru APIs being called.
 *
 * Hydra_Software_Devel/204   8/22/08 10:29a rpan
 * PR45979: Remove VBI passthru support.
 *
 * Hydra_Software_Devel/203   8/21/08 10:07a rpan
 * PR45620: Changing pixel format should not force reconfig vnet.
 *
 * Hydra_Software_Devel/202   7/30/08 11:36a tdo
 * PR45181: Implement New Dynamic Contrast Algorithm
 *
 * Hydra_Software_Devel/201   7/11/08 4:47p rpan
 * PR43614: Bandwidth equation API change.
 *
 * Hydra_Software_Devel/200   6/18/08 3:07p syang
 * PR35549: 1). src-pending when new win is created; 2). don't do
 * BufferHeap_AllocateBuffers as bReConfigSrc
 *
 * Hydra_Software_Devel/199   6/17/08 10:59a rpan
 * PR43614: Moved bandwidth equation parameters to BVDC_P_Window_info
 * structure. Changed factor to 1000.
 *
 * Hydra_Software_Devel/198   6/16/08 4:59p rpan
 * PR43614: Added API for user to set/get bandwidth equation parameters.
 *
 * Hydra_Software_Devel/197   5/30/08 1:38p tdo
 * PR38362, PR38678, PR38841, PR38842: More fine tuning of the BVN HW
 * limitation implementation.
 *
 * Hydra_Software_Devel/196   5/21/08 1:55p albertl
 * PR39163:  Updated CMP csc tables to use 10 bit offset values.
 * Corrected to use proper csc tables in conversions involving ntsc, pal,
 * and smpte 240M.  Moved colorclip from window to compositor and changed
 * BVDC_Window_Set/GetColorClip to BVDC_Compositor_Set/GetColorClip.
 *
 * Hydra_Software_Devel/195   5/19/08 5:03p yuxiaz
 * PR40122: Use new 10-bit 422 40-bit packed format for ANR on 3548.
 *
 * Hydra_Software_Devel/194   5/19/08 2:25p yuxiaz
 * PR40122: Use new 10-bit 422 packed pixel format for MAD on 3548.
 *
 * Hydra_Software_Devel/193   5/15/08 3:40p yuxiaz
 * PR42712: Coverity Defect ID:9043 OVERRUN_STATIC bvdc_window.c
 *
 * Hydra_Software_Devel/192   5/8/08 6:09p tdo
 * PR39331: Add LAB table control and demo mode.  Also, force LAB table to
 * be loaded in one vsync with the enable to avoid using uninitialized
 * LAB table.
 *
 * Hydra_Software_Devel/191   5/2/08 9:14p albertl
 * PR42395:  Fixed fixed point MTH overflow in
 * BVDC_P_Csc_ColorTempToAttenuationRGB.
 *
 * Hydra_Software_Devel/190   5/2/08 10:53a jessem
 * PR 39335: Added CM3D demo.
 *
 * Hydra_Software_Devel/189   4/22/08 7:40p albertl
 * PR39163:  Fixed user matrix set functions to apply to stNewInfo.
 *
 * Hydra_Software_Devel/188   4/11/08 2:12p tdo
 * PR39333: Properly handle Hist in vnet
 *
 * Hydra_Software_Devel/187   4/8/08 6:11p albertl
 * PR39163:  Updated BVDC_Window_SetColorClip and BVDC_Window_GetColorClip
 * interface to use settings structure.
 *
 * Hydra_Software_Devel/186   4/3/08 2:07p tdo
 * PR39333: Add error checking for invalid bin number
 *
 * Hydra_Software_Devel/185   3/27/08 7:41p albertl
 * PR39163:  Implemented non-linear get matrix functions.
 *
 * Hydra_Software_Devel/184   3/27/08 5:08p albertl
 * PR39163:  Implemented non-linear xvYCC csc support for 3548.
 *
 * Hydra_Software_Devel/183   3/21/08 5:09p tdo
 * PR39333: More Hist implementation for 3548
 *
 * Hydra_Software_Devel/182   3/21/08 12:21p hongtaoz
 * PR39154, PR39202: updated MAD revision ID and validation for 3548;
 *
 * Hydra_Software_Devel/181   3/20/08 9:43a pntruong
 * PR39154, PR39202, PR39203: Fixed merged errors.
 *
 * Hydra_Software_Devel/180   3/19/08 7:47p hongtaoz
 * PR39154, PR39202, PR39203: combined ANR and MAD in memory saving mode
 * on 3548; implemented MAD specific change; updated SCL precision for
 * 3548;
 *
 * Hydra_Software_Devel/179   3/19/08 5:43p tdo
 * PR39333: Implement HIST core
 *
 * Hydra_Software_Devel/178   3/13/08 10:26a yuxiaz
 * PR40517: Fixed KLOCWORK errors.
 *
 * Hydra_Software_Devel/177   3/4/08 2:26p yuxiaz
 * PR40187: Fixed the assertion in BVDC_P_Window_Writer_isr when
 * BVDC_Window_SetDeinterlaceConfiguration is called with bDeinterlace is
 * false and a null pointer for pMadSettings.
 *
 * Hydra_Software_Devel/176   2/28/08 10:12p pntruong
 * PR39154: Resolved clearcase merged.
 *
 * Hydra_Software_Devel/175   2/26/08 4:52p jessem
 * PR 34590, PR 39456: Removed ENDIAN treatment of pixel format of
 * picture in BVDC_Window_GetBuffer.
 *
 * Hydra_Software_Devel/174   2/20/08 4:15p hongtaoz
 * PR39154: added window API to toggle ANR; prepare for combo ANR/MAD
 * feature on 3548;
 *
 * Hydra_Software_Devel/173   2/14/08 4:33p tdo
 * PR39331, PR39332: Regroup PEP block to add support for 3548
 *
 * Hydra_Software_Devel/172   2/13/08 3:11p pntruong
 * PR39421: Removed unused code to improve readability.
 *
 * Hydra_Software_Devel/171   1/24/08 5:49p tdo
 * PR38710: Fix compiling error for chips that do not have PEP support
 *
 * Hydra_Software_Devel/170   1/24/08 5:27p tdo
 * PR38710: Add blue stretch support
 *
 * Hydra_Software_Devel/169   1/23/08 11:13a tdo
 * PR38710: Rename blue stretch and green stretch to blue boost and green
 * boost to match their functionality
 *
 * Hydra_Software_Devel/168   1/18/08 11:20a hongtaoz
 * PR38736: added force_spatial 3-field MAD game mode; re-grouped MAD game
 * modes definition and added more comments;
 *
 * Hydra_Software_Devel/167   1/9/08 10:46a yuxiaz
 * PR38613, PR38614: Enable force capture and smooth scaling on 7325 and
 * 7335.
 *
 * Hydra_Software_Devel/166   11/28/07 2:02p tdo
 * PR37413: Provide new Api to set offset and size of histogram window and
 * read corresponding parameters from specific window
 *
 * Hydra_Software_Devel/165   11/14/07 10:56a rpan
 * PR31084: Consider NULL pointers as valid values when getting
 * VDEC/Deinterlace default settings.
 *
 * Hydra_Software_Devel/164   11/13/07 5:35p rpan
 * PR31084: Added support for getting VDEC default settings.
 *
 * Hydra_Software_Devel/163   11/13/07 4:07p rpan
 * PR31084: Added support for getting default de-interlace settings.
 *
 * Hydra_Software_Devel/162   10/23/07 11:18a yuxiaz
 * PR29569, PR36290: Add FGT support on 7405.
 *
 * Hydra_Software_Devel/PR29569/4   10/22/07 5:16p yuxiaz
 * PR29569: More FGT work.
 *
 * Hydra_Software_Devel/PR29569/3   10/22/07 3:21p yuxiaz
 * PR29569: Merge from mainline.
 *
 * Hydra_Software_Devel/161   10/22/07 11:27a pntruong
 * PR35941: Added option to allow smooth transition or bandwidth
 * optimization.
 *
 * Hydra_Software_Devel/160   10/11/07 2:10p albertl
 * PR35135:  Fixed user csc macros and added 10-bit/8-bit video data
 * conversion.
 *
 * Hydra_Software_Devel/159   10/2/07 11:26a pntruong
 * PR35136: Some cleanup of unused code.
 *
 * Hydra_Software_Devel/158   9/28/07 12:59p pntruong
 * PR35135: Don't use the csc from window as it's not context safe.
 *
 * Hydra_Software_Devel/157   9/20/07 1:22a albertl
 * PR35135:  Cleaned up color space matrices and changed them to use same
 * macro system.  Added color space conversion functionality to graphics
 * windows.
 *
 * Hydra_Software_Devel/156   9/4/07 5:40p jessem
 * PR 34590: Added check for unsupported 8-bit 422 VFD/MFD pixel formats
 * in BVDC_Window_SetPixelFormat(). If this is used, CAP will be
 * programmed accordingly but the VFD will not work which will result in
 * a pink/green video.
 *
 * Hydra_Software_Devel/155   8/30/07 10:28p pntruong
 * PR34472: Use MASK block to improve the Contour/Quantization noise
 * issue.
 *
 * Hydra_Software_Devel/154   8/30/07 3:14p pntruong
 * PR34348: MAD optimizing Low Angle Settings for reduce gray noise.
 *
 * Hydra_Software_Devel/PR34348/1   8/29/07 4:07p xhuang
 * PR34348:add low angle customer settings
 *
 * Hydra_Software_Devel/153   8/7/07 12:44p tdo
 * PR33792: Addtional Parameter for MAD chroma settings
 *
 * Hydra_Software_Devel/152   7/30/07 6:50p albertl
 * PR28869:  Removed VDC dependency on BAVC_MOSAIC_MAX.  Clear rect and
 * mosaic maximums are now dependent on definitions in bvdc_common.h.
 *
 * Hydra_Software_Devel/151   7/23/07 6:44p hongtaoz
 * PR33345: added 3-field 0-delay MAD game mode;
 *
 * Hydra_Software_Devel/150   7/10/07 6:11p tdo
 * PR32853: Extending the range of CMS saturation and hue gains to match
 * BBS3.0 tool
 *
 * Hydra_Software_Devel/149   6/28/07 7:26p hongtaoz
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
 * Hydra_Software_Devel/VDC_Game_Mode/2   6/26/07 2:28p hongtaoz
 * PR32130: added game mode buffer delay control;
 *
 * Hydra_Software_Devel/VDC_Game_Mode/1   6/26/07 2:22p hongtaoz
 * PR32130: added window game mode buffer delay control;
 *
 * Hydra_Software_Devel/148   6/18/07 11:39a tdo
 * PR30613: Fix typo in CMS range chacking for hue gain
 *
 * Hydra_Software_Devel/147   6/12/07 11:19a tdo
 * PR30613: Move CSC calculations into bvdc_csc_priv.c
 *
 * Hydra_Software_Devel/146   6/11/07 1:07p tdo
 * PR30613: Modify dynamic contrast stretch prototype to meet customer
 * requirement
 *
 * Hydra_Software_Devel/145   6/7/07 2:36p tdo
 * PR29965: Alternating CAB and LAB table programming using RUL in every
 * other vsync
 *
 * Hydra_Software_Devel/144   6/6/07 11:38a tdo
 * PR28978: Fix range for hue gain
 *
 * Hydra_Software_Devel/143   5/31/07 3:26p tdo
 * PR28978: C0: Provide API to realize CMS function
 *
 * Hydra_Software_Devel/142   5/18/07 4:22p albertl
 * PR28894:  Added new 3-field low bandwidth MAD game mode for 3563 C0.
 *
 * Hydra_Software_Devel/141   5/17/07 3:44p pntruong
 * PR30364: Provide API to control MAD up/down sampling.
 *
 * Hydra_Software_Devel/140   5/1/07 4:44p syang
 * PR 28541: 1) changed non-linear scl api; 2). carry sub-pxl accuracy
 * through the pipe line by SrcHStep and SrcVStep
 *
 * Hydra_Software_Devel/139   4/15/07 1:51a albertl
 * PR28869:  Maximum clear rect now defined for each window and chipset.
 *
 * Hydra_Software_Devel/138   4/12/07 5:37p tdo
 * PR28837: Provide API to customize LAB LUT
 *
 * Hydra_Software_Devel/137   4/9/07 11:26a pntruong
 * PR28266: Provide API to customize DVO CSC.
 *
 * Hydra_Software_Devel/136   4/5/07 11:26p pntruong
 * PR28395, PR28538, PR28539, PR28540: Vdec customizations, and
 * deinterlacer customizations.
 *
 * Hydra_Software_Devel/135   4/5/07 6:12p tdo
 * PR25462: add support for luma average reporting in the PEP
 *
 * Hydra_Software_Devel/134   4/5/07 4:57p albertl
 * PR29194:  Added BVDC_Window_SetCoefficientIndex and input and index
 * matching support to scaler coefficient tables.
 *
 * Hydra_Software_Devel/133   4/5/07 2:04p pntruong
 * PR28538, PR28539, PR28540, PR25462: Average pixel luma and customized
 * deinterlacer support.
 *
 * Hydra_Software_Devel/132   3/29/07 4:19p tdo
 * PR 28404: Implement user set contrast stretch configuration
 *
 * Hydra_Software_Devel/PR28538/3   4/3/07 9:33p xhuang
 * PR25462: Support APL setting
 *
 * Hydra_Software_Devel/PR28538/2   3/29/07 3:07p xhuang
 * PR28538:modify to get correct current setting of mad
 *
 * Hydra_Software_Devel/PR28538/1   3/28/07 6:42p xhuang
 * PR28538:Add new api for 3:2 pulldown, 2:2 pulldown and mad setting.
 * Also for PR28539,PR28540
 *
 * Hydra_Software_Devel/131   3/28/07 6:16p tdo
 * PR28392, PR28282: Implement get color matrix function
 *
 * Hydra_Software_Devel/130   3/28/07 4:10p tdo
 * PR28403: Move user sharpness config dirty bit set out of validation
 * code for optimization
 *
 * Hydra_Software_Devel/129   3/28/07 3:28p tdo
 * PR 28403: Provide API to customize TNT for sharpness
 *
 * Hydra_Software_Devel/128   3/26/07 3:28p tdo
 * PR 28401: Provide API to customize CAB LUT and enable/disable CAB
 *
 * Hydra_Software_Devel/127   3/25/07 2:09p yuxiaz
 * PR25181: VDC Buffer Allocation Fragments fix: set dirty bit directly.
 *
 * Hydra_Software_Devel/126   3/25/07 1:45p yuxiaz
 * PR25181: VDC Buffer Allocation Fragments fix.
 *
 * Hydra_Software_Devel/125   3/23/07 4:39p tdo
 * PR 28282: Provide API to customize CMP CSC
 *
 * Hydra_Software_Devel/124   3/20/07 4:48p hongtaoz
 * PR22434: added mosaic channel frame rate tracking API;
 *
 * Hydra_Software_Devel/123   3/13/07 8:58a maivu
 * PR 27216: Fixed compiler warnings for Vxworks
 *
 * Hydra_Software_Devel/122   3/1/07 11:17p albertl
 * PR24587:  Added support for compositor blenders that are not arraigned
 * in order.
 *
 * Hydra_Software_Devel/120   2/16/07 3:44p syang
 * PR 22569: add anr filter level;  add back even line allign plus out-of-
 * range check
 *
 * Hydra_Software_Devel/119   1/16/07 1:45p yuxiaz
 * PR27067: Use window Id in BVDC_Window_Create.
 *
 * Hydra_Software_Devel/118   1/15/07 3:30p yuxiaz
 * PR27067: Use window Id in BVDC_Window_Create.
 *
 * Hydra_Software_Devel/117   1/10/07 4:44p yuxiaz
 * PR 26313: Rename BVDC_SplitScreenSettings to
 * BVDC_Window_SplitScreenSettings.
 *
 * Hydra_Software_Devel/116   1/10/07 4:28p tdo
 * PR 17994, PR 26113: Add demo mode support for CMP 1 and CMP 2 for 7400
 * B0
 *
 * Hydra_Software_Devel/115   1/10/07 3:32p yuxiaz
 * PR 26313: Change DNR API to source based API: Remove
 * BVDC_Window_SetDnrFilterLevel and BVDC_Window_GetDnrFilterLevel. Add
 * BVDC_Source_SetDnrFilterLevel, BVDC_Source_GetDnrFilterLevel,
 * BVDC_Source_SetSplitScreenMode and BVDC_Source_GetSplitScreenMode.
 *
 * Hydra_Software_Devel/114   1/5/07 4:26p syang
 * PR 22569: almost completed coding for new vnet build/shutdown algorithm
 * (by each sub-modules)
 *
 * Hydra_Software_Devel/113   1/4/07 12:03p yuxiaz
 * PR 26890: Added VDC_Source_GetDefaultSettings,
 * BVDC_Window_GetDefaultSettings and BVDC_Compositor_GetDefaultSettings.
 *
 * Hydra_Software_Devel/112   12/18/06 11:24p pntruong
 * PR22577: Merged back to mainline.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/17   12/11/06 5:25p albertl
 * PR24587:  Added 3rd GFX Feeder source.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/16   11/29/06 4:27p syang
 * PR 26151: release shred resource after it is not used by HW
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/15   11/27/06 10:33a syang
 * PR 26151:  only release shared scl as win destroy
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/14   11/22/06 3:53p syang
 * PR 26151: re-write shared resource manager. update letter box, mad, dnr
 * shared-scl acquire/release code accordingly
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/13   11/15/06 5:09p albertl
 * PR24587:  Added downsample source for 7440.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/12   11/13/06 5:05p hongtaoz
 * PR20716: added Get function for mosaic config;
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/11   11/13/06 3:35p hongtaoz
 * PR20716: separate displays/compsoitors for loops;
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/10   11/1/06 10:49a jessem
 * PR 23748: Added game mode support.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/9   10/20/06 1:41p pntruong
 * PR23521:  Removed duplicate code, and move game mode into existing
 * deinterlace settings structures.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/8   10/19/06 3:53p albertl
 * PR23521:  Added MAD game mode functionality.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/7   10/18/06 9:30p albertl
 * PR23210:  Added color temp and offset functionality.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/6   10/12/06 5:46p tdo
 * PR 23377:  Add support for MAD 3:2 and 2:2 reverse pulldown
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/5   9/20/06 6:21p tdo
 * PR 23213, PR 23383: Fix error in user input validation for demo mode.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/4   9/20/06 3:54p tdo
 * PR 23213, PR 23383: Using new API for demo mode
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/3   9/20/06 12:57p pntruong
 * PR23222, PR23225:  Defer window and source heap assignments until user
 * create.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/2   9/19/06 4:36p pntruong
 * PR23222, PR23225: VDEC - ATSC & PC input support.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/1   9/19/06 1:47p pntruong
 * PR23222, PR23225: VDEC - ATSC & PC input support.
 *
 * Hydra_Software_Devel/111   9/13/06 4:27p syang
 * PR 23331: add BVDC_Window_SetHorizScale; add more desc to the APIs
 * related feeding gfx surf to mfd and cap video to gfx surf.
 *
 * Hydra_Software_Devel/110   9/7/06 5:12p tdo
 * PR 17994, PR 23213, PR 23383:  Add input validation checks for demo
 * mode controls
 *
 * Hydra_Software_Devel/109   9/1/06 3:40p yuxiaz
 * PR20875 : Added more error checking in BVDC_Window_SetPixelFormat.
 *
 * Hydra_Software_Devel/108   8/31/06 12:29p syang
 * PR 23977: report error when mad is called for 7118
 *
 * Hydra_Software_Devel/107   8/30/06 5:59p tdo
 * PR 17994, PR 23213, PR 23383:  Implement demo mode support for the PEP.
 *
 * Hydra_Software_Devel/106   8/29/06 3:28p tdo
 * PR 23212: Add new APIs for contrast stretch, color clip, auto flesh,
 * green stretch and blue stretch.
 *
 * Hydra_Software_Devel/105   8/15/06 6:17p hongtaoz
 * PR18233, PR20716: added support for mosaic mode - missing channels;
 * assume picture list's channel id are in ascending order; single
 * channel will not result in full-screen automatically;
 *
 * Hydra_Software_Devel/104   7/31/06 6:11p hongtaoz
 * PR22499: added DCR filter setting as a part of DNR block;
 *
 * Hydra_Software_Devel/103   7/27/06 4:21p yuxiaz
 * PR22412: Added BVDC_Window_SetForceCapture and
 * BVDC_Window_GetForceCapture.
 *
 * Hydra_Software_Devel/102   7/18/06 1:34p syang
 * PR 21963: ensure ulWindows copied from src new info to cur info, to
 * avoid  win bUserAppliedChanges and src bPictureChanged being always
 * set true with ApplyChanges (that causes extra busy with brutus replay
 * with gfx updating frequently )
 *
 * Hydra_Software_Devel/101   7/17/06 5:57p pntruong
 * PR18233: Moved MOSAIC_MAX to avc since other PIs need to access the
 * definition.
 *
 * Hydra_Software_Devel/100   7/7/06 4:15p yuxiaz
 * PR 22296, PR 22108, PR22109: Removed
 * BVDC_Window_InstallLipSyncDelayCallback, added new generic window
 * callback function BVDC_Window_InstallCallback for lip sync.
 *
 * Hydra_Software_Devel/99   7/7/06 3:47p jessem
 * PR 21445: Removed
 * pWindow->stNewInfo.stDirty.stBits.bUserReleaseBuffer = BVDC_P_DIRTY. This is
 * now set in BVDC_P_Window_ReleasePicture_isr.
 *
 * Hydra_Software_Devel/99   7/7/06 2:43p jessem
 * PR 21445: Removed
 * pWindow->stNewInfo.stDirty.stBits.bUserReleaseBuffer = BVDC_P_DIRTY. This is
 * now set in BVDC_P_Window_ReleasePicture_isr.
 *
 * Hydra_Software_Devel/98   6/14/06 6:52p hongtaoz
 * PR20716, PR21804: implement modified version of mosaic mode;
 *
 * Hydra_Software_Devel/97   5/23/06 5:41p hongtaoz
 * PR17514: support correct usage mode of DNR; now DNR is available for
 * any video window and shared between mpeg sources; only one mpeg source
 * can acquire the DNR at a time;
 *
 * Hydra_Software_Devel/96   5/19/06 2:29p hongtaoz
 * PR17514: added bypass filter enum for DNR setting; disable extreme
 * filter for 7401a0 due to PR16299; fixed range clamping for DNR
 * settings; added DNR demo mode setting in test api;
 *
 * Hydra_Software_Devel/95   4/20/06 5:50p pntruong
 * PR20750: Please update bavc.h to reflect presence of third VEC.
 *
 * Hydra_Software_Devel/94   4/12/06 4:24p jessem
 * PR19972: Added big endian mapping of pixel format in
 * BVDC_Window_GetBuffer.
 *
 * Hydra_Software_Devel/92   4/4/06 1:50p hongtaoz
 * PR20403: add triple displays for 7400;
 *
 * Hydra_Software_Devel/91   3/29/06 5:34p jessem
 * PR 20422: Reassigned pWindow->stCurInfo.pBufferToUser to a local
 * variable in BVDC_Window_GetBuffer to avoid race condition.
 *
 * Hydra_Software_Devel/90   2/13/06 11:09a pntruong
 * PR13275, PR19258: Back out uncessary feature.  Already available with
 * window source clipping.
 *
 * Hydra_Software_Devel/89   2/8/06 10:39a yuxiaz
 * PR13275: Added new functions VDC_Window_Set704Sample4Vdec
 * and VDC_Window_Get704Sample4Vdec for support of 704 pixels output from
 * the VDEC on a CCIR-656 port.
 *
 * Hydra_Software_Devel/88   1/27/06 1:08p jessem
 * PR 18021: Added BVDC_Window_SetPixelFormat.
 *
 * Hydra_Software_Devel/87   1/27/06 10:37a pntruong
 * PR19218: ATSC display corrupted.  Synclock source with multiple window
 * must be capture when it's not in cmp_0 to avoid accessive bandwidth
 * the other slip capture paths.
 *
 * Hydra_Software_Devel/86   1/12/06 1:34p hongtaoz
 * PR18233: added mosaic mode support;
 *
 * Hydra_Software_Devel/MosaicMode_Feature_PR18233/3   1/12/06 11:35a hongtaoz
 * PR18233: modified mosaic mode API to be more flexible;
 *
 * Hydra_Software_Devel/85   1/12/06 10:36a yuxiaz
 * PR18476: Added new functions BVDC_Window_SetLumaKey and
 * BVDC_Window_GetLumaKey for luma key support.
 *
 * Hydra_Software_Devel/84   11/10/05 6:04p jessem
 * PR 17972, PR 18072: Modified BVDC_Window_GetBuffer to handle new
 * changes in BVDC_P_Window_CapturePicture_isr().
 *
 * Hydra_Software_Devel/83   10/31/05 6:26p hongtaoz
 * PR17514: add DNR support in VDC;
 *
 * Hydra_Software_Devel/82   10/28/05 11:45a tdo
 * PR 17788:  Add 656 input support for 7038Cx
 *
 * Hydra_Software_Devel/81   9/12/05 4:33p jessem
 * PR 12739: Fixed bug in BVDC_Window_Destroy that causes leaked resource
 * error.
 *
 * Hydra_Software_Devel/80   9/12/05 4:26p jessem
 * PR 12739: Cleaned up debug code.
 *
 * Hydra_Software_Devel/79   9/12/05 3:01p jessem
 * PR 12739: Renamed
 * BVDC_P_Window_CheckForNonReturnedUserCapturedBuffer_isr to
 * BVDC_P_Window_CheckForUnReturnedUserCapturedBuffer_isr.
 *
 * Hydra_Software_Devel/78   9/12/05 2:51p jessem
 * PR 12739: Added 3 functions to support user capture of fields and/or
 * frames: BVDC_Window_SetUserCaptureBufferCount(),
 * BVDC_Window_GetBuffer(), and BVDC_Window_ReturnBuffer(). Also modified
 * Window destroy to check for unreturned user capture buffers.
 *
 * Hydra_Software_Devel/77   9/6/05 7:03p hongtaoz
 * PR16812, PR15888: mute writer blocks and muxe earlier; window blocks
 * shutdown and normal programming order are reverse; not to re-init
 * window shared resource at create time;
 *
 * Hydra_Software_Devel/76   9/1/05 6:23p jasonh
 * PR 16967: Removing XVD source
 *
 * Hydra_Software_Devel/75   8/22/05 5:25p hongtaoz
 * PR12192, PR15888: make MAD sharable between the two video windows of
 * main display; reduced unnecessary window shutdown initiation if it's
 * already in the middle of shutdown; build both slots RULs for analog
 * progressive source format during shutdown to workaround unexpected
 * trigger pattern;
 *
 * Hydra_Software_Devel/74   8/19/05 2:57p hongtaoz
 * PR15888: adding support for dynamic switch on/off shared scaler on
 * bypass window;
 *
 * Hydra_Software_Devel/73   8/18/05 1:20p pntruong
 * PR15757, PR16391, PR16411, PR12519, PR14791, PR15535, PR15206, PR15778:
 * Improved bandwidth for cropping/scaler/capture/playback.   Unified
 * window shutdown sequence for destroy, reconfigure mad/scaler, and/or
 * reconfigure result of source changes.  And miscellances fixes from
 * above PRs.
 *
 * Hydra_Software_Devel/XVDPhase1/XVDPhase1_merge/3   8/16/05 2:02p pntruong
 * PR12519: Take in additional fixes from mainline.
 *
 * Hydra_Software_Devel/XVDPhase1/XVDPhase1_merge/2   8/15/05 5:37p pntruong
 * PR14791: Remove unneeded function from VDC.
 *
 * Hydra_Software_Devel/XVDPhase1/XVDPhase1_merge/1   8/2/05 1:07p pntruong
 * PR16484: (1241) Progressive outputs give flickery picture.
 *
 * Hydra_Software_Devel/XVDPhase1/2   5/20/05 11:07a pntruong
 * PR 15183: Add sharpness control to the PEP and remove the old TAB
 * setting.
 *
 * Hydra_Software_Devel/XVDPhase1/1   5/2/05 4:55p pntruong
 * PR12519, PR13121, PR15048, PR15084, PR15100: Dynamically re-allocate
 * capture in bvn path to reduce memory consumption and bandwith.
 *
 * Hydra_Software_Devel/3   5/2/05 2:43p pntruong
 * Temporary turn off deinterlacer.
 *
 * Hydra_Software_Devel/2   5/2/05 12:57p pntruong
 * Initial support for decoding of 480i for hddvi.
 *
 * Hydra_Software_Devel/1   5/1/05 4:52p pntruong
 * XVDPhase1 branch off.  Temp storage so we can keep track of changes.
 *
 * Hydra_Software_Devel/69   3/18/05 6:32p pntruong
 * PR14494: Basic vdc up and running.  Background and passed output format
 * switch test.
 *
 * Hydra_Software_Devel/68   3/17/05 6:42p pntruong
 * PR14494: Add preliminary software support to 3560 A0.
 *
 * Hydra_Software_Devel/67   2/4/05 4:56p hongtaoz
 * PR13793: added compile option to use scaler_3 for bypass window;
 *
 * Hydra_Software_Devel/66   1/31/05 11:33a yuxiaz
 * PR13955: Enable HD_DVI on 7038C.
 *
 * Hydra_Software_Devel/65   1/28/05 9:18a pntruong
 * PR13321: Stress test failed (MAIN/PIP swap and format change).
 *
 * Hydra_Software_Devel/64   1/26/05 2:30p tdo
 * PR13526: Add VDEC support for C0
 *
 ***************************************************************************/
#include "bstd.h"                 /* standard types */
#include "bkni.h"                 /* memcpy calls */
#include "bvdc.h"                 /* Video display */
#include "bvdc_priv.h"
#include "bvdc_mad_priv.h"
#include "bvdc_window_priv.h"
#include "bvdc_source_priv.h"
#include "bvdc_compositor_priv.h"
#include "bvdc_gfxfeeder_priv.h"  /* for blend factor validation */
#include "bvdc_hist_priv.h"
#include "bvdc_csc_priv.h"
#include "bvdc_tnt_priv.h"
#include "bvdc_feeder_priv.h"

BDBG_MODULE(BVDC_WIN);

/* max clear rect: index by eWinId */
static const uint32_t s_aulWinMaxClearRect[] =
{
	BVDC_P_CMP_0_CLEAR_RECT_V0,  /* BVDC_P_WindowId_eComp0_V0 = 0 */
	BVDC_P_CMP_0_CLEAR_RECT_V1,  /* BVDC_P_WindowId_eComp0_V1 */
	BVDC_P_CMP_1_CLEAR_RECT_V0,  /* BVDC_P_WindowId_eComp1_V0 */
	BVDC_P_CMP_1_CLEAR_RECT_V1,  /* BVDC_P_WindowId_eComp1_V1 */
	BVDC_P_CMP_2_CLEAR_RECT_V0,  /* BVDC_P_WindowId_eComp2_V0 */
	BVDC_P_CMP_3_CLEAR_RECT_V0,  /* BVDC_P_WindowId_eComp3_V0 */
	BVDC_P_CMP_4_CLEAR_RECT_V0,  /* BVDC_P_WindowId_eComp4_V0 */
	BVDC_P_CMP_5_CLEAR_RECT_V0,  /* BVDC_P_WindowId_eComp5_V0 */
	BVDC_P_CMP_6_CLEAR_RECT_V0,  /* BVDC_P_WindowId_eComp6_V0 */
	0,                           /* BVDC_P_WindowId_eComp0_G0 */
	0,                           /* BVDC_P_WindowId_eComp0_G1 */
	0,                           /* BVDC_P_WindowId_eComp0_G2 */
	0,                           /* BVDC_P_WindowId_eComp1_G0 */
	0,                           /* BVDC_P_WindowId_eComp2_G0 */
	0,                           /* BVDC_P_WindowId_eComp3_G0 */
	0,                           /* BVDC_P_WindowId_eComp4_G0 */
	0,                           /* BVDC_P_WindowId_eComp5_G0 */
	0                            /* BVDC_P_WindowId_eComp6_G0 */
};

/***************************************************************************
 *
 */
BERR_Code BVDC_Window_GetDefaultSettings
	( BVDC_WindowId                    eWindowId,
	  BVDC_Window_Settings            *pDefSettings )
{
	BSTD_UNUSED(eWindowId);

	/* Clear out first */
	BKNI_Memset(pDefSettings, 0, sizeof(BVDC_Window_Settings));

	/* Initialized */
	pDefSettings->hHeap = NULL;
	pDefSettings->ulMaxMosaicRect = s_aulWinMaxClearRect[eWindowId];

	/* if this is true, then sync-slipped window may have double-capture-buffer
	   instead of multi-buffer operation; this works with VEC locking; */
	pDefSettings->bForceSyncLock = false;

	/* KLUDGE: PR53654, no flash for source format smaller than 720p.  This
	 * currently temporary assumption, and will be going thru a public api
	 * that allow application to specify the minimun buffer allow that will
	 * not flash.
	 * Maintain flag for backward compatibility, user specify no flag but
	 * did not specify format we default to 720p. */
	#ifndef BVDC_NO_FLASH
	#define BVDC_NO_FLASH (0)
	#endif
	#if (BVDC_NO_FLASH)
	if(!pDefSettings->pMinSrcFmt)
	{
		pDefSettings->pMinSrcFmt = BFMT_GetVideoFormatInfoPtr(BFMT_VideoFmt_e720p);
	}
	#endif

	return BERR_SUCCESS;
}


/***************************************************************************
 *
 */
BERR_Code BVDC_Window_Create
	( BVDC_Compositor_Handle           hCompositor,
	  BVDC_Window_Handle              *phWindow,
	  BVDC_WindowId                    eId,
	  BVDC_Source_Handle               hSource,
	  const BVDC_Window_Settings      *pDefSettings )
{
	BVDC_Window_Handle hWindow;
	BERR_Code eRet = BERR_SUCCESS;

	BDBG_ENTER(BVDC_Window_Create);

	BDBG_ASSERT(hCompositor);
	BDBG_ASSERT(phWindow);
	BDBG_ASSERT(hSource);

	/* Check internally if we created. */
	BDBG_OBJECT_ASSERT(hSource, BVDC_SRC);
	BDBG_OBJECT_ASSERT(hCompositor, BVDC_CMP);

	/* Make sure the source/compositor is active or created. */
	if(BVDC_P_STATE_IS_INACTIVE(hSource) ||
	   BVDC_P_STATE_IS_DESTROY(hSource) ||
	   BVDC_P_STATE_IS_INACTIVE(hCompositor) ||
	   BVDC_P_STATE_IS_DESTROY(hCompositor))
	{
		BDBG_ERR(("Compositor and/or Source are not active."));
		return BERR_TRACE(BVDC_ERR_WINDOW_NOT_AVAILABLE);
	}

	/* select the corresponding private handle created at BVDC_Open */
	eRet = BVDC_P_Window_GetPrivHandle(hCompositor, eId, hSource->eId, &hWindow);
	if (BERR_SUCCESS != eRet)
	{
		return BERR_TRACE(eRet);
	}

	/* Check if the window is already created or not. */
	if(!BVDC_P_STATE_IS_INACTIVE(hWindow))
	{
		BDBG_ERR(("Window[%d] state = %d", hWindow->eId, hWindow->eState));
		return BERR_TRACE(BVDC_ERR_WINDOW_ALREADY_CREATED);
	}

	BDBG_MSG(("Creating window%d", hWindow->eId));
	BDBG_ASSERT(BVDC_P_STATE_IS_INACTIVE(hWindow));

	if (BVDC_P_SRC_IS_GFX(hSource->eId))
	{
		/* this makes gfx surface be validated in next ApplyChanges */
		hSource->hGfxFeeder->hWindow = hWindow;
	}
	else if (BVDC_P_SRC_IS_VFD(hSource->eId))
	{
		BAVC_SourceId  eSrcId;

		/* this VFD should not already been used by some window */
		if (hSource->hVfdFeeder->hWindow)
		{
			BDBG_ERR(("feeder %d is already used by Window[%d]",
				hSource->eId - BAVC_SourceId_eVfd0, hWindow->eId));
			return BERR_TRACE(BVDC_ERR_RESOURCE_NOT_AVAILABLE);
		}

		/* is user provided vfd the right one for this window? */
		eSrcId = BAVC_SourceId_eVfd0 + (hWindow->pResource->ePlayback - BVDC_P_FeederId_eVfd0);
		if (hSource->eId != eSrcId)
		{
			BDBG_ERR(("feeder %d is not the proper VFD to feed Window[%d]",
				hSource->eId, hWindow->eId));
			return BERR_TRACE(BVDC_ERR_SOURCE_WINDOW_MISMATCH);
		}

		/* this makes gfx surface be validated in next ApplyChanges and
		 * marks the vfd as being used */
		hSource->hVfdFeeder->hWindow = hWindow;
	}
	else /* video src, but not vfd */
	{
		BVDC_P_Feeder_Handle  hVfdFeeder;
		BAVC_SourceId  eSrcId;

		/* the assigned playback should not be already in use for gfx surface */
		eSrcId = BAVC_SourceId_eVfd0 + (hWindow->pResource->ePlayback - BVDC_P_FeederId_eVfd0);
		if (hWindow->hCompositor->hVdc->ahSource[eSrcId])
		{
			hVfdFeeder = hWindow->hCompositor->hVdc->ahSource[eSrcId]->hVfdFeeder;
			if (hVfdFeeder->hWindow)
			{
				BDBG_ERR(("Window[%d]'s playback feeder %d is in use for gfx",
					hWindow->eId, hWindow->pResource->ePlayback - BVDC_P_FeederId_eVfd0));
				return BERR_TRACE(BVDC_ERR_RESOURCE_NOT_AVAILABLE);
			}

			/* mark the VFD as being in use */
			hVfdFeeder->hWindow = hWindow;
		}

		if (BVDC_P_SRC_IS_MPEG(hSource->eId) && hSource->hMpegFeeder->bGfxSrc)
		{
			hSource->hMpegFeeder->hWindow = hWindow;
		}
	}

	/* Reinitialize context.  But not make it active until apply. */
	*phWindow = hWindow;

	/* Which heap to use? */
	hWindow->hHeap = ((pDefSettings) && (pDefSettings->hHeap))
		? pDefSettings->hHeap : hCompositor->hVdc->hBufferHeap;

	/* Default settings */
	if(pDefSettings)
	{
		hWindow->stSettings = *pDefSettings;
		/* Do not allow override of number of rect this is read-only
		 * information. */
		if(hWindow->eId < BVDC_P_WindowId_eUnknown)
		{
			hWindow->stSettings.ulMaxMosaicRect = s_aulWinMaxClearRect[hWindow->eId];
		}
		else
		{
			BDBG_ASSERT(hWindow->eId < BVDC_P_WindowId_eUnknown);
		}
		hWindow->hCompositor->hDisplay->stNewInfo.bBypassVideoProcess =
			pDefSettings->bBypassVideoProcessings;
#if BVDC_P_SUPPORT_STG
		if(BVDC_P_DISPLAY_USED_STG(hWindow->hCompositor->hDisplay->eMasterTg))
			hWindow->hCompositor->hDisplay->stNewInfo.stDirty.stBits.bStgEnable =
				hWindow->hCompositor->hDisplay->stNewInfo.bBypassVideoProcess ^
				hWindow->hCompositor->hDisplay->stCurInfo.bBypassVideoProcess;
#endif

	}
	else
	{
		BVDC_Window_GetDefaultSettings(hWindow->eId, &hWindow->stSettings);
	}

#if BVDC_P_SUPPORT_MOSAIC_MODE
	if((pDefSettings) && (pDefSettings->hHeap))
	{
		/* Allocate drain buffer from window heap */
		if(hWindow->stSettings.ulMaxMosaicRect)
		{
			/* allocate drain buffer (2 pixels deep) for mosaic mode support */
			hWindow->pvNullBufAddr = BMEM_Heap_AllocAligned(
				pDefSettings->hHeap->hMem,
				16, /* 2 pixels wide, in case 10-bit 4:2:2 capture rounding; */
				4,  /* 16 bytes aligned for capture engine */
				0   /* no boundary */);
			BERR_TRACE(BMEM_Heap_ConvertAddressToOffset(
				pDefSettings->hHeap->hMem,
				hWindow->pvNullBufAddr, &hWindow->ulNullBufOffset));
		}
		else
		{
			hWindow->pvNullBufAddr = NULL;
			hWindow->ulNullBufOffset = 0;
		}
	}
	else
	{
		/* Use the drain buffer from VDC main heap. Set pvNullBufAddr to NULL
		 * so it will not be free in BVDC_Window_Destroy */
		hWindow->pvNullBufAddr = NULL;
		hWindow->ulNullBufOffset = hCompositor->hVdc->ulVdcNullBufOffset;
	}
#endif

	/* Inititial window fields */
	BVDC_P_Window_Init(*phWindow, hSource);

	/* Keep count of new number of windows that connected to source! */
	hSource->stNewInfo.ulWindows++;
	hSource->stNewInfo.stDirty.stBits.bWindowNum = BVDC_P_DIRTY;
	hSource->stNewInfo.stDirty.stBits.bAddWin = BVDC_P_DIRTY;

	/* Mark as create, awating for apply. */
	hWindow->eState = BVDC_P_State_eCreate;

	BDBG_MSG(("Window[%d] is BVDC_P_State_eCreate", hWindow->eId));

	BDBG_LEAVE(BVDC_Window_Create);
	return BERR_SUCCESS;
}


/***************************************************************************
 *
 */
BERR_Code BVDC_Window_Destroy
	( BVDC_Window_Handle               hWindow )
{
	BERR_Code eRet = BERR_SUCCESS;
	BVDC_Source_Handle  hSource;

	BDBG_ENTER(BVDC_Window_Destroy);

	/* Return if trying to free a NULL handle. */
	if(!hWindow)
	{
		goto done;
	}

	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	if(BVDC_P_STATE_IS_DESTROY(hWindow) ||
	   BVDC_P_STATE_IS_INACTIVE(hWindow))
	{
		goto done;
	}

	BVDC_P_Window_DisableBoxDetect(hWindow);

	hSource = hWindow->stCurInfo.hSource;

	BKNI_EnterCriticalSection();
	if (BVDC_P_STATE_IS_CREATE(hWindow))
	{
		hWindow->eState = BVDC_P_State_eInactive;
		BDBG_MSG(("Window[%d] is BVDC_P_State_eInactive", hWindow->eId));

		hSource = hWindow->stNewInfo.hSource;

		/* Keep count of new number of windows that connected to source! */
		hSource->stNewInfo.ulWindows--;
		hSource->stNewInfo.stDirty.stBits.bWindowNum = BVDC_P_DIRTY;
	}
	else if (BVDC_P_STATE_IS_ACTIVE(hWindow))
	{
		/* check if user-captured buffers, if any, were returned */
		if (BVDC_P_Window_CheckForUnReturnedUserCapturedBuffer_isr(hWindow))
		{
			eRet = BVDC_ERR_USER_STILL_HAS_CAPTURE_BUFFER;
		}

		/* Keep count of new number of windows that connected to source! */
		hSource->stNewInfo.ulWindows--;
		hWindow->stNewInfo.hSource->stNewInfo.stDirty.stBits.bWindowNum = BVDC_P_DIRTY;
		hWindow->eState = BVDC_P_State_eDestroy;
		BDBG_MSG(("Window[%d] is BVDC_P_State_eDestroy", hWindow->eId));
	}
	BKNI_LeaveCriticalSection();

#if BVDC_P_SUPPORT_MOSAIC_MODE
	if(hWindow->pvNullBufAddr)
	{
		/* free drain buffer */
		BMEM_Heap_Free(hWindow->hHeap->hMem, hWindow->pvNullBufAddr);
	}
#endif

	if (BVDC_P_SRC_IS_GFX(hSource->eId))
	{
		/* stop validating this gfd */
		hSource->hGfxFeeder->hWindow = NULL;
	}
	else if (BVDC_P_SRC_IS_VFD(hSource->eId))
	{
		/* mark the vfd as NOT being in use and stop validating this vfd */
		hSource->hVfdFeeder->hWindow = NULL;
	}
	else if (BVDC_P_SRC_IS_VIDEO(hSource->eId))
	{
		BAVC_SourceId  eSrcId;

		/* mark the vfd as NOT being in use */
		eSrcId = BAVC_SourceId_eVfd0 + (hWindow->pResource->ePlayback - BVDC_P_FeederId_eVfd0);
		if (hWindow->hCompositor->hVdc->ahSource[eSrcId])
		{
			hWindow->hCompositor->hVdc->ahSource[eSrcId]->hVfdFeeder->hWindow = NULL;
		}

		if (BVDC_P_SRC_IS_MPEG(hSource->eId))
		{
			/* mark the mfd as NOT being in use and stop validating this mfd */
			hSource->hMpegFeeder->hWindow = NULL;
		}
	}

done:
	BDBG_LEAVE(BVDC_Window_Destroy);
	return eRet;
}


/***************************************************************************
 *
 */
BERR_Code BVDC_Window_SetSrcClip
	( BVDC_Window_Handle               hWindow,
	  uint32_t                         ulLeft,
	  uint32_t                         ulRight,
	  uint32_t                         ulTop,
	  uint32_t                         ulBottom )
{
	BDBG_ENTER(BVDC_Window_SetSrcClip);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	/* set new value */
	hWindow->stNewInfo.stSrcClip.ulLeft   = ulLeft;
	hWindow->stNewInfo.stSrcClip.ulRight  = ulRight;
	hWindow->stNewInfo.stSrcClip.ulTop    = ulTop;
	hWindow->stNewInfo.stSrcClip.ulBottom = ulBottom;

	BDBG_LEAVE(BVDC_Window_SetSrcClip);
	return BERR_SUCCESS;
}



/***************************************************************************
 *
 */
BERR_Code BVDC_Window_SetSrcRightClip
	( BVDC_Window_Handle               hWindow,
	  int32_t                          lLeftDelta )
{
	BERR_Code eRet = BERR_SUCCESS;

#if (!BVDC_P_SUPPORT_3D_INDEP_SRC_CLIP)
	BSTD_UNUSED(lLeftDelta);
#endif

	BDBG_ENTER(BVDC_Window_SetSrcRightClip);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

#if (!BVDC_P_SUPPORT_3D_INDEP_SRC_CLIP)
	BDBG_ERR(("Independent source clipping not support on this chip!"));
	eRet = BVDC_ERR_3D_INDEP_SRC_CLIP_NOT_SUPPORTED;
#else
	/* set new value */
	hWindow->stNewInfo.stSrcClip.lLeftDelta_R = lLeftDelta;
#endif

	BDBG_LEAVE(BVDC_Window_SetSrcRightClip);
	return BERR_TRACE(eRet);
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Window_GetSrcRightClip
	( const BVDC_Window_Handle         hWindow,
	  int32_t                         *plLeftDelta )
{
	BDBG_ENTER(BVDC_Window_GetSrcRightClip);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	if(plLeftDelta)
	{
		*plLeftDelta = hWindow->stCurInfo.stSrcClip.lLeftDelta_R;
	}

	BDBG_LEAVE(BVDC_Window_GetSrcRightClip);
	return BERR_SUCCESS;
}


/***************************************************************************
 *
 */
BERR_Code BVDC_Window_SetSrcClip_isr
	( BVDC_Window_Handle               hWindow,
	  uint32_t                         ulLeft,
	  uint32_t                         ulRight,
	  uint32_t                         ulTop,
	  uint32_t                         ulBottom )
{
	BDBG_ENTER(BVDC_Window_SetSrcClip_isr);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	/* set isr value and validate it later */
	hWindow->stIsrInfo.stSrcClip.ulLeft   = ulLeft;
	hWindow->stIsrInfo.stSrcClip.ulRight  = ulRight;
	hWindow->stIsrInfo.stSrcClip.ulTop    = ulTop;
	hWindow->stIsrInfo.stSrcClip.ulBottom = ulBottom;

	BDBG_LEAVE(BVDC_Window_SetSrcClip_isr);
	return BERR_SUCCESS;
}


/***************************************************************************
 *
 */
BERR_Code BVDC_Window_GetSrcClip
	( const BVDC_Window_Handle         hWindow,
	  uint32_t                        *pulLeft,
	  uint32_t                        *pulRight,
	  uint32_t                        *pulTop,
	  uint32_t                        *pulBottom )
{
	BDBG_ENTER(BVDC_Window_GetSrcClip);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	/* get value */
	if(pulLeft)
	{
		*pulLeft = hWindow->stCurInfo.stSrcClip.ulLeft;
	}

	if(pulRight)
	{
		*pulRight = hWindow->stCurInfo.stSrcClip.ulRight;
	}

	if(pulTop)
	{
		*pulTop = hWindow->stCurInfo.stSrcClip.ulTop;
	}

	if(pulBottom)
	{
		*pulBottom = hWindow->stCurInfo.stSrcClip.ulBottom;
	}

	BDBG_LEAVE(BVDC_Window_GetSrcClip);
	return BERR_SUCCESS;
}


/***************************************************************************
 *
 */
BERR_Code BVDC_Window_SetDstRect
	( BVDC_Window_Handle               hWindow,
	  int32_t                          lLeft,
	  int32_t                          lTop,
	  uint32_t                         ulWidth,
	  uint32_t                         ulHeight )
{
	BDBG_ENTER(BVDC_Window_SetDstRect);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	/* Dst Rect cannot be smaller than 2x1. */
	if(ulWidth < BVDC_P_WIN_DST_OUTPUT_H_MIN ||
	   ulHeight < BVDC_P_WIN_DST_OUTPUT_V_MIN)
	{
		BDBG_ERR(("Min Dst Rect is %dx%d",
			BVDC_P_WIN_DST_OUTPUT_H_MIN,
			BVDC_P_WIN_DST_OUTPUT_V_MIN));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* set new value */
	hWindow->stNewInfo.stDstRect.lLeft    = lLeft;
	hWindow->stNewInfo.stDstRect.lTop     = lTop;
	hWindow->stNewInfo.stDstRect.ulWidth  = ulWidth;
	hWindow->stNewInfo.stDstRect.ulHeight = ulHeight;

	BDBG_LEAVE(BVDC_Window_SetDstRect);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Window_SetDstRect_isr
	( BVDC_Window_Handle               hWindow,
	  int32_t                          lLeft,
	  int32_t                          lTop,
	  uint32_t                         ulWidth,
	  uint32_t                         ulHeight )
{
	BDBG_ENTER(BVDC_Window_SetDstRect_isr);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	/* set isr value and validate it later */
	hWindow->stIsrInfo.stDstRect.lLeft    = lLeft;
	hWindow->stIsrInfo.stDstRect.lTop     = lTop;
	hWindow->stIsrInfo.stDstRect.ulWidth  = ulWidth;
	hWindow->stIsrInfo.stDstRect.ulHeight = ulHeight;

	BDBG_LEAVE(BVDC_Window_SetDstRect_isr);
	return BERR_SUCCESS;
}


/***************************************************************************
 *
 */
BERR_Code BVDC_Window_GetDstRect
	( const BVDC_Window_Handle         hWindow,
	  int32_t                         *plLeftScreen,
	  int32_t                         *plTopScreen,
	  uint32_t                        *pulWidth,
	  uint32_t                        *pulHeight )
{
	BDBG_ENTER(BVDC_Window_GetDstRect);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	/* get value */
	if(plLeftScreen)
	{
		*plLeftScreen = hWindow->stCurInfo.stDstRect.lLeft;
	}

	if(plTopScreen)
	{
		*plTopScreen = hWindow->stCurInfo.stDstRect.lTop;
	}

	if(pulWidth)
	{
		*pulWidth = hWindow->stCurInfo.stDstRect.ulWidth;
	}

	if(pulHeight)
	{
		*pulHeight = hWindow->stCurInfo.stDstRect.ulHeight;
	}

	BDBG_LEAVE(BVDC_Window_GetDstRect);
	return BERR_SUCCESS;
}


/***************************************************************************
 *
 */
BERR_Code BVDC_Window_SetScalerOutput
	( BVDC_Window_Handle               hWindow,
	  uint32_t                         ulLeft,
	  uint32_t                         ulTop,
	  uint32_t                         ulWidth,
	  uint32_t                         ulHeight )
{
	BDBG_ENTER(BVDC_Window_SetScalerOutput);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	/* Preliminary checking that does not depend on until applychanges. */
	if((ulLeft > ulWidth) ||
	   (ulTop  > ulHeight))
	{
		BDBG_ERR(("Top, Left must be inside rectangle dimension"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* set new value */
	hWindow->stNewInfo.stScalerOutput.lLeft    = ulLeft;
	hWindow->stNewInfo.stScalerOutput.lTop     = ulTop;
	hWindow->stNewInfo.stScalerOutput.ulWidth  = ulWidth;
	hWindow->stNewInfo.stScalerOutput.ulHeight = ulHeight;

	BDBG_LEAVE(BVDC_Window_SetScalerOutput);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Window_SetScalerOutput_isr
	( BVDC_Window_Handle               hWindow,
	  uint32_t                         ulLeft,
	  uint32_t                         ulTop,
	  uint32_t                         ulWidth,
	  uint32_t                         ulHeight )
{
	BDBG_ENTER(BVDC_Window_SetScalerOutput_isr);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	/* Preliminary checking. */
	if((ulLeft > ulWidth) ||
	   (ulTop  > ulHeight))
	{
		BDBG_ERR(("Top, Left must be inside rectangle dimension"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* set isr value and validate it later */
	hWindow->stIsrInfo.stScalerOutput.lLeft    = ulLeft;
	hWindow->stIsrInfo.stScalerOutput.lTop     = ulTop;
	hWindow->stIsrInfo.stScalerOutput.ulWidth  = ulWidth;
	hWindow->stIsrInfo.stScalerOutput.ulHeight = ulHeight;

	BDBG_LEAVE(BVDC_Window_SetScalerOutput_isr);
	return BERR_SUCCESS;
}


/***************************************************************************
 *
 */
BERR_Code BVDC_Window_GetScalerOutput
	( const BVDC_Window_Handle         hWindow,
	  uint32_t                        *pulLeft,
	  uint32_t                        *pulTop,
	  uint32_t                        *pulWidth,
	  uint32_t                        *pulHeight )
{
	BDBG_ENTER(BVDC_Window_GetScalerOutput);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	/* get value */
	if(pulLeft)
	{
		*pulLeft = hWindow->stCurInfo.stScalerOutput.lLeft;
	}

	if(pulTop)
	{
		*pulTop = hWindow->stCurInfo.stScalerOutput.lTop;
	}

	if(pulWidth)
	{
		*pulWidth = hWindow->stCurInfo.stScalerOutput.ulWidth;
	}

	if(pulHeight)
	{
		*pulHeight = hWindow->stCurInfo.stScalerOutput.ulHeight;
	}


	BDBG_LEAVE(BVDC_Window_GetScalerOutput);
	return BERR_SUCCESS;
}


/***************************************************************************
 *
 */
BERR_Code BVDC_Window_SetAlpha
	( BVDC_Window_Handle               hWindow,
	  uint8_t                          ucAlpha )
{
	BDBG_ENTER(BVDC_Window_SetAlpha);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	/* set new value */
	hWindow->stNewInfo.ucAlpha = ucAlpha;

	BDBG_LEAVE(BVDC_Window_SetAlpha);
	return BERR_SUCCESS;
}


/***************************************************************************
 *
 */
BERR_Code BVDC_Window_GetAlpha
	( const BVDC_Window_Handle         hWindow,
	  uint8_t                         *pucAlpha )
{
	BDBG_ENTER(BVDC_Window_GetAlpha);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	/* set new value */
	if(pucAlpha)
	{
		*pucAlpha = hWindow->stCurInfo.ucAlpha;
	}

	BDBG_LEAVE(BVDC_Window_GetAlpha);
	return BERR_SUCCESS;
}


/***************************************************************************
 *
 */
BERR_Code BVDC_Window_SetBlendFactor
	( BVDC_Window_Handle               hWindow,
	  BVDC_BlendFactor                 eSrcBlendFactor,
	  BVDC_BlendFactor                 eDstBlendFactor,
	  uint8_t                          ucConstantAlpha )
{
	BERR_Code  eResult;

	BDBG_ENTER(BVDC_Window_SetBlendFactor);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	/* For gfx window need to restrict blending factor. */
	if( BVDC_P_WIN_IS_GFX_WINDOW(hWindow->eId) )
	{
		eResult = BVDC_P_GfxFeeder_ValidateBlend( eSrcBlendFactor,
												  eDstBlendFactor,
												  ucConstantAlpha );
		if ( BERR_SUCCESS != eResult )
		{
			return BERR_TRACE(eResult);
		}
	}

	/* set new value */
	hWindow->stNewInfo.ucConstantAlpha   = ucConstantAlpha;
	hWindow->stNewInfo.eFrontBlendFactor = eSrcBlendFactor;
	hWindow->stNewInfo.eBackBlendFactor  = eDstBlendFactor;

	BDBG_LEAVE(BVDC_Window_SetBlendFactor);
	return BERR_SUCCESS;
}


/***************************************************************************
 *
 */
BERR_Code BVDC_Window_GetBlendFactor
	( const BVDC_Window_Handle         hWindow,
	  uint8_t                         *pucConstantAlpha,
	  BVDC_BlendFactor                *peSrcBlendFactor,
	  BVDC_BlendFactor                *peDstBlendFactor )
{
	BDBG_ENTER(BVDC_Window_GetBlendFactor);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	/* set new value */
	if(pucConstantAlpha)
	{
		*pucConstantAlpha = hWindow->stCurInfo.ucConstantAlpha;
	}

	if(peSrcBlendFactor)
	{
		*peSrcBlendFactor = hWindow->stCurInfo.eFrontBlendFactor;
	}

	if(peDstBlendFactor)
	{
		*peDstBlendFactor = hWindow->stCurInfo.eBackBlendFactor;
	}

	BDBG_LEAVE(BVDC_Window_GetBlendFactor);
	return BERR_SUCCESS;
}


/***************************************************************************
 *
 */
BERR_Code BVDC_Window_SetZOrder
	( BVDC_Window_Handle               hWindow,
	  uint8_t                          ucZOrder )
{
	BDBG_ENTER(BVDC_Window_SetZOrder);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	/* Preliminary checking that does not depend on until applychanges. */
	if(ucZOrder > BVDC_Z_ORDER_MAX)
	{
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* set new value */
	hWindow->stNewInfo.ucZOrder = ucZOrder;

	BDBG_LEAVE(BVDC_Window_SetZOrder);
	return BERR_SUCCESS;
}


/***************************************************************************
 *
 */
BERR_Code BVDC_Window_GetZOrder
	( const BVDC_Window_Handle         hWindow,
	  uint8_t                         *pucZOrder )
{
	BDBG_ENTER(BVDC_Window_GetZOrder);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	/* set new value */
	if(pucZOrder)
	{
		*pucZOrder = hWindow->stCurInfo.ucZOrder;
	}

	BDBG_LEAVE(BVDC_Window_GetZOrder);
	return BERR_SUCCESS;
}


/***************************************************************************
 *
 */
BERR_Code BVDC_Window_SetVisibility
	( BVDC_Window_Handle               hWindow,
	  bool                             bVisible )
{
	BDBG_ENTER(BVDC_Window_SetVisibility);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	/* set new value */
	hWindow->stNewInfo.bVisible = bVisible;

	BDBG_LEAVE(BVDC_Window_SetVisibility);
	return BERR_SUCCESS;
}


/***************************************************************************
 *
 */
BERR_Code BVDC_Window_GetVisibility
	( const BVDC_Window_Handle         hWindow,
	  bool                            *pbVisible )
{
	BDBG_ENTER(BVDC_Window_GetVisibility);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	/* set new value */
	if(pbVisible)
	{
		*pbVisible = hWindow->stCurInfo.bVisible;
	}

	BDBG_LEAVE(BVDC_Window_GetVisibility);
	return BERR_SUCCESS;
}


/***************************************************************************
 *
 */
BERR_Code BVDC_Window_SetDeinterlaceConfiguration
	( BVDC_Window_Handle               hWindow,
	  bool                             bDeinterlace,
	  const BVDC_Deinterlace_Settings *pMadSettings )
{
	BVDC_P_Deinterlace_Settings *pNewMad;
	const BVDC_P_Deinterlace_Settings *pCurMad;
	BERR_Code    eResult;

	BDBG_ENTER(BVDC_Window_SetDeinterlaceConfiguration);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	pNewMad = &hWindow->stNewInfo.stMadSettings;
	pCurMad = &hWindow->stCurInfo.stMadSettings;

	/* set new value */
	hWindow->stNewInfo.bDeinterlace = bDeinterlace;

	if(bDeinterlace && pMadSettings)
	{
		eResult = BVDC_P_Window_SetMcvp_DeinterlaceConfiguration(hWindow, bDeinterlace, pMadSettings);
		if(eResult != BERR_SUCCESS)
		{
			return BERR_TRACE(BERR_NOT_SUPPORTED);
		}

		/*Mad: validate game mode */
		eResult = BVDC_P_Window_SetMad_DeinterlaceConfiguration(hWindow, bDeinterlace, pMadSettings);
		if(eResult != BERR_SUCCESS)
		{
			return BERR_TRACE(BERR_NOT_SUPPORTED);
		}

		pNewMad->eGameMode = pMadSettings->eGameMode;
		pNewMad->ePqEnhancement = pMadSettings->ePqEnhancement;
		pNewMad->bShrinkWidth = pMadSettings->bShrinkWidth;
		pNewMad->bReverse32Pulldown = pMadSettings->bReverse32Pulldown;
		pNewMad->bReverse22Pulldown = pMadSettings->bReverse22Pulldown;

#if (BVDC_P_SUPPORT_VIDEO_TESTFEATURE1_MAD_ANR)
		pNewMad->stTestFeature1.bEnable = pMadSettings->stVideoTestFeature1.bEnable;
		pNewMad->stTestFeature1.ulBitsPerPixel = pMadSettings->stVideoTestFeature1.ulBitsPerPixel;
#else
		BDBG_MSG(("This chip will ignore MAD TestFeature1 user setting!"));
#endif

		if(pMadSettings->pChromaSettings)
		{
			pNewMad->stChromaSettings = *(pMadSettings->pChromaSettings);
			hWindow->stNewInfo.bChromaCustom = true;
		}
		else
		{
			hWindow->stNewInfo.bChromaCustom = false;
		}

		if(pMadSettings->pMotionSettings)
		{
			pNewMad->stMotionSettings= *(pMadSettings->pMotionSettings);
			hWindow->stNewInfo.bMotionCustom = true;
		}
		else
		{
			hWindow->stNewInfo.bMotionCustom = false;
		}

		if(pMadSettings->pReverse32Settings)
		{
			pNewMad->stReverse32Settings = *(pMadSettings->pReverse32Settings);
			hWindow->stNewInfo.bRev32Custom = true;
		}
		else
		{
			hWindow->stNewInfo.bRev32Custom= false;
		}

		if(pMadSettings->pReverse22Settings)
		{
			pNewMad->stReverse22Settings = *(pMadSettings->pReverse22Settings);
			hWindow->stNewInfo.bRev22Custom = true;
		}
		else
		{
			hWindow->stNewInfo.bRev22Custom = false;
		}

		/* Use the customized Up/Down sampler filter settings */
		if(pMadSettings->pUpSampler)
		{
			pNewMad->stUpSampler = *(pMadSettings->pUpSampler);
		}
		else
		{
			BVDC_P_Mad_Init_Custom(&pNewMad->stUpSampler, NULL, NULL);
		}
		if(pMadSettings->pDnSampler)
		{
			pNewMad->stDnSampler = *(pMadSettings->pDnSampler);
		}
		else
		{
			BVDC_P_Mad_Init_Custom(NULL, &pNewMad->stDnSampler, NULL);
		}

		/* low angles setting */
		if(pMadSettings->pLowAngles)
		{
			pNewMad->stLowAngles = *(pMadSettings->pLowAngles);
		}
		else
		{
			BVDC_P_Mad_Init_Custom(NULL, NULL, &pNewMad->stLowAngles);
		}
	}

	/* On/Off toggle and game mode change will be handled separately */
	if( (bDeinterlace != hWindow->stCurInfo.bDeinterlace) ||
	  (bDeinterlace && pMadSettings &&
	  (pNewMad->eGameMode != pCurMad->eGameMode ||
	   pNewMad->ePixelFmt != pCurMad->ePixelFmt ||
	   pNewMad->stTestFeature1.bEnable != pCurMad->stTestFeature1.bEnable ||
	   pNewMad->stTestFeature1.ulBitsPerPixel  != pCurMad->stTestFeature1.ulBitsPerPixel ||
	   pNewMad->ePqEnhancement != pCurMad->ePqEnhancement||
	   pNewMad->bShrinkWidth != pCurMad->bShrinkWidth ||
	   pNewMad->bReverse22Pulldown != pCurMad->bReverse22Pulldown ||
	   pNewMad->bReverse32Pulldown != pCurMad->bReverse32Pulldown ||
	   pNewMad->stChromaSettings.bChromaField420EdgeDetMode != pCurMad->stChromaSettings.bChromaField420EdgeDetMode ||
	   pNewMad->stChromaSettings.bChromaField420InitPhase != pCurMad->stChromaSettings.bChromaField420InitPhase ||
	   pNewMad->stChromaSettings.eChroma422InverseTelecineMode!= pCurMad->stChromaSettings.eChroma422InverseTelecineMode ||
	   pNewMad->stChromaSettings.eChroma422MotionAdaptiveMode != pCurMad->stChromaSettings.eChroma422MotionAdaptiveMode ||
	   pNewMad->stChromaSettings.eChromaField420InvMethod != pCurMad->stChromaSettings.eChromaField420InvMethod ||
	   pNewMad->stChromaSettings.eChroma420MotionMode != pCurMad->stChromaSettings.eChroma420MotionMode ||
	   pNewMad->stChromaSettings.eChroma422MotionMode != pCurMad->stChromaSettings.eChroma422MotionMode ||
	   pNewMad->stChromaSettings.eChroma420MotionAdaptiveMode != pCurMad->stChromaSettings.eChroma420MotionAdaptiveMode ||
	   pNewMad->stChromaSettings.bMS_3548 != pCurMad->stChromaSettings.bMS_3548 ||
	   pNewMad->stChromaSettings.bMT_3548 != pCurMad->stChromaSettings.bMT_3548 ||
	   pNewMad->stChromaSettings.ulMaxXChroma != pCurMad->stChromaSettings.ulMaxXChroma ||
	   pNewMad->stMotionSettings.bEnableQmK != pCurMad->stMotionSettings.bEnableQmK ||
	   pNewMad->stMotionSettings.bEnableQmL != pCurMad->stMotionSettings.bEnableQmL ||
	   pNewMad->stMotionSettings.bEnableQmM != pCurMad->stMotionSettings.bEnableQmM ||
	   pNewMad->stMotionSettings.eSmMode != pCurMad->stMotionSettings.eSmMode ||
	   pNewMad->stMotionSettings.eTmMode != pCurMad->stMotionSettings.eTmMode ||
	   pNewMad->stReverse22Settings.ulBwvLuma22Threshold != pCurMad->stReverse22Settings.ulBwvLuma22Threshold ||
	   pNewMad->stReverse22Settings.ulEnterLockLevel != pCurMad->stReverse22Settings.ulEnterLockLevel ||
	   pNewMad->stReverse22Settings.ulLockSatLevel != pCurMad->stReverse22Settings.ulLockSatLevel ||
	   pNewMad->stReverse22Settings.ulNonMatchMatchRatio != pCurMad->stReverse22Settings.ulNonMatchMatchRatio ||
	   pNewMad->stReverse32Settings.ulBadEditLevel != pCurMad->stReverse32Settings.ulBadEditLevel ||
	   pNewMad->stReverse32Settings.ulBwvLuma32Threshold != pCurMad->stReverse32Settings.ulBwvLuma32Threshold ||
	   pNewMad->stReverse32Settings.ulEnterLockLevel != pCurMad->stReverse32Settings.ulEnterLockLevel ||
	   pNewMad->stReverse32Settings.ulLockSatLevel != pCurMad->stReverse32Settings.ulLockSatLevel ||
	   pNewMad->stReverse32Settings.ulPhaseMatchThreshold != pCurMad->stReverse32Settings.ulPhaseMatchThreshold ||
	   pNewMad->stReverse32Settings.ulRepfPxlCorrectLevel!= pCurMad->stReverse32Settings.ulRepfPxlCorrectLevel ||
	   pNewMad->stReverse32Settings.ulRepfVetoLevel != pCurMad->stReverse32Settings.ulRepfVetoLevel ||
	   hWindow->stNewInfo.bChromaCustom != hWindow->stCurInfo.bChromaCustom ||
	   hWindow->stNewInfo.bMotionCustom != hWindow->stCurInfo.bMotionCustom ||
	   hWindow->stNewInfo.bRev32Custom != hWindow->stCurInfo.bRev32Custom ||
	   hWindow->stNewInfo.bRev22Custom != hWindow->stCurInfo.bRev22Custom ||
	   pNewMad->stUpSampler.eFilterType != pCurMad->stUpSampler.eFilterType ||
	   pNewMad->stUpSampler.eRingRemoval != pCurMad->stUpSampler.eRingRemoval ||
	   pNewMad->stUpSampler.bUnbiasedRound != pCurMad->stUpSampler.bUnbiasedRound ||
	   pNewMad->stDnSampler.eFilterType != pCurMad->stDnSampler.eFilterType ||
	   pNewMad->stDnSampler.eRingRemoval != pCurMad->stDnSampler.eRingRemoval ||
	   pNewMad->stLowAngles.ulLaControlDirRatio != pCurMad->stLowAngles.ulLaControlDirRatio ||
	   pNewMad->stLowAngles.ulLaControlRangeLimitScale != pCurMad->stLowAngles.ulLaControlRangeLimitScale ||
	   pNewMad->stLowAngles.ulLaMinNorthStrength != pCurMad->stLowAngles.ulLaMinNorthStrength)) )
	{
		hWindow->stNewInfo.stDirty.stBits.bDeinterlace = BVDC_P_DIRTY;
	}

#if (!(BVDC_P_SUPPORT_MAD || BVDC_P_SUPPORT_MCVP))
	BSTD_UNUSED(pMadSettings);

	if(bDeinterlace)
	{
		BDBG_ERR(("This chipset doesn't support deinterlace!"));
		return BERR_TRACE(BVDC_ERR_MAD_NOT_SUPPORTED);
	}

#endif

	BDBG_LEAVE(BVDC_Window_SetDeinterlaceConfiguration);
	return BERR_SUCCESS;
}


/***************************************************************************
 *
 */
BERR_Code BVDC_Window_GetDeinterlaceConfiguration
	( const BVDC_Window_Handle         hWindow,
	  bool                            *pbDeinterlace,
	  BVDC_Deinterlace_Settings       *pMadSettings )
{
	const BVDC_P_Deinterlace_Settings *pCurMadSettings;

	BDBG_ENTER(BVDC_Window_GetDeinterlaceConfiguration);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	/* Cur Info */
	pCurMadSettings = &hWindow->stCurInfo.stMadSettings;

	/* set new value */
	if(pbDeinterlace)
	{
		*pbDeinterlace = hWindow->stCurInfo.bDeinterlace;
	}

	if(pMadSettings)
	{
		pMadSettings->eGameMode = pCurMadSettings->eGameMode;
		pMadSettings->ePxlFormat = pCurMadSettings->ePixelFmt;
		pMadSettings->ePqEnhancement = pCurMadSettings->ePqEnhancement;
		pMadSettings->bShrinkWidth       = pCurMadSettings->bShrinkWidth;
		pMadSettings->bReverse22Pulldown = pCurMadSettings->bReverse22Pulldown;
		pMadSettings->bReverse32Pulldown = pCurMadSettings->bReverse32Pulldown;

		pMadSettings->stVideoTestFeature1.bEnable = pCurMadSettings->stTestFeature1.bEnable;
		pMadSettings->stVideoTestFeature1.ulBitsPerPixel = pCurMadSettings->stTestFeature1.ulBitsPerPixel;

		/* Customized structure are NULL */
		pMadSettings->pChromaSettings    = NULL;
		pMadSettings->pMotionSettings    = NULL;
		pMadSettings->pReverse32Settings = NULL;
		pMadSettings->pReverse22Settings = NULL;
		pMadSettings->pUpSampler         = NULL;
		pMadSettings->pDnSampler         = NULL;
		pMadSettings->pLowAngles         = NULL;
	}

	BDBG_LEAVE(BVDC_Window_GetDeinterlaceConfiguration);
	return BERR_SUCCESS;
}


/***************************************************************************
 *
 */
BERR_Code BVDC_Window_GetDeinterlaceDefaultConfiguration
	( const BVDC_Window_Handle         hWindow,
	  BVDC_Deinterlace_Settings       *pMadSettings )
{
	BDBG_ENTER(BVDC_Window_GetDeinterlaceDefaultConfiguration);
	/* sanity check input parameter as much as we can */
	BDBG_ASSERT(pMadSettings);
	BSTD_UNUSED(hWindow);

	/* Get static default */
	BVDC_P_Mad_Init_Default(
		&pMadSettings->eGameMode,
		&pMadSettings->ePxlFormat,
		&pMadSettings->ePqEnhancement,
		&pMadSettings->stVideoTestFeature1,
		&pMadSettings->bShrinkWidth,
		&pMadSettings->bReverse32Pulldown,
		 pMadSettings->pReverse32Settings,
		&pMadSettings->bReverse22Pulldown,
		 pMadSettings->pReverse22Settings,
		 pMadSettings->pChromaSettings,
		 pMadSettings->pMotionSettings);

	BVDC_P_Mad_Init_Custom(
		pMadSettings->pUpSampler,
		pMadSettings->pDnSampler,
		pMadSettings->pLowAngles);

	/* Get dynamic default */
	BVDC_P_Mad_Init_DynamicDefault(hWindow,
		pMadSettings->pReverse32Settings,
		pMadSettings->pReverse22Settings,
		pMadSettings->pChromaSettings);


	BDBG_LEAVE(BVDC_Window_GetDeinterlaceDefaultConfiguration);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Window_SetVbiPassThrough
	( BVDC_Window_Handle               hWindow,
	  bool                             bAllowVbiPassThrough,
	  uint32_t                         ulAdditionalVbiLines )
{
	BDBG_ERR(("BVDC_Window_SetVbiPassThrough() is no longer supported. "));
	BDBG_ASSERT(0);
	BSTD_UNUSED(hWindow);
	BSTD_UNUSED(bAllowVbiPassThrough);
	BSTD_UNUSED(ulAdditionalVbiLines);
	return BERR_NOT_SUPPORTED;
}


/***************************************************************************
 *
 */
BERR_Code BVDC_Window_GetVbiPassThrough
	( const BVDC_Window_Handle         hWindow,
	  bool                            *pbAllowVbiPassThrough,
	  uint32_t                        *pulAdditionalVbiLines )
{
	BDBG_ERR(("BVDC_Window_GetVbiPassThrough() is no longer supported. "));
	BDBG_ASSERT(0);
	BSTD_UNUSED(hWindow);
	BSTD_UNUSED(pbAllowVbiPassThrough);
	BSTD_UNUSED(pulAdditionalVbiLines);
	return BERR_NOT_SUPPORTED;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Window_SetAfdSettings
	( BVDC_Window_Handle               hWindow,
	  const BVDC_AfdSettings          *pAfdSettings )
{
	BDBG_ENTER(BVDC_Window_SetAfdSettings);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	/* set new value */
	hWindow->stNewInfo.stAfdSettings = *pAfdSettings;

	BDBG_LEAVE(BVDC_Window_SetAfdSettings);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Window_GetAfdSettings
	( const BVDC_Window_Handle         hWindow,
	  BVDC_AfdSettings                *pAfdSettings )
{
	BDBG_ENTER(BVDC_Window_GetAfdSettings);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	/* set new value */
	if(pAfdSettings)
	{
		*pAfdSettings = hWindow->stCurInfo.stAfdSettings;
	}

	BDBG_LEAVE(BVDC_Window_GetAfdSettings);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Window_SetPanScanType
	( BVDC_Window_Handle               hWindow,
	  BVDC_PanScanType                 ePanScanType )
{
	BDBG_ENTER(BVDC_Window_SetPanScanType);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	/* set new value */
	hWindow->stNewInfo.ePanScanType = ePanScanType;

	BDBG_LEAVE(BVDC_Window_SetPanScanType);
	return BERR_SUCCESS;
}


/***************************************************************************
 *
 */
BERR_Code BVDC_Window_GetPanScanType
	( const BVDC_Window_Handle         hWindow,
	  BVDC_PanScanType                *pePanScanType )
{
	BDBG_ENTER(BVDC_Window_GetPanScanType);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	/* set new value */
	if(pePanScanType)
	{
		*pePanScanType = hWindow->stCurInfo.ePanScanType;
	}

	BDBG_LEAVE(BVDC_Window_GetPanScanType);
	return BERR_SUCCESS;
}


/***************************************************************************
 *
 */
BERR_Code BVDC_Window_SetUserPanScan
	( BVDC_Window_Handle               hWindow,
	  int32_t                          lHorzPanScan,
	  int32_t                          lVertPanScan )
{
	BDBG_ENTER(BVDC_Window_SetUserPanScan);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	/* set new value */
	hWindow->stNewInfo.lUserHorzPanScan = lHorzPanScan;
	hWindow->stNewInfo.lUserVertPanScan = lVertPanScan;

	BDBG_LEAVE(BVDC_Window_SetUserPanScan);
	return BERR_SUCCESS;
}


/***************************************************************************
 *
 */
BERR_Code BVDC_Window_GetUserPanScan
	( const BVDC_Window_Handle         hWindow,
	  int32_t                         *plHorzPanScan,
	  int32_t                         *plVertPanScan )
{
	BDBG_ENTER(BVDC_Window_GetPanScanType);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	if(plHorzPanScan)
	{
		*plHorzPanScan = hWindow->stCurInfo.lUserHorzPanScan;
	}

	if(plVertPanScan)
	{
		*plVertPanScan = hWindow->stCurInfo.lUserVertPanScan;
	}

	BDBG_LEAVE(BVDC_Window_GetPanScanType);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Window_EnableBoxDetect
	( BVDC_Window_Handle                hWindow,
	  BVDC_Window_BoxDetectCallback_isr BoxDetectCallBack,
	  void  *                           pvParm1,
  	  int                               iParm2,
	  bool                              bAutoCutBlack )
{
	BERR_Code  eResult;

	BDBG_ENTER(BVDC_Window_EnableBoxDetect);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	BDBG_MSG(("LBox Detect might not work with extreme source clipping due to bandwidth issue"));
	eResult = BVDC_P_Window_EnableBoxDetect( hWindow,
											 BoxDetectCallBack, pvParm1, iParm2,
											 bAutoCutBlack );

	BDBG_LEAVE(BVDC_Window_EnableBoxDetect);
	return BERR_TRACE(eResult);
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Window_DisableBoxDetect
	( BVDC_Window_Handle                hWindow )
{
	BERR_Code  eResult;

	BDBG_ENTER(BVDC_Window_DisableBoxDetect);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	eResult = BVDC_P_Window_DisableBoxDetect( hWindow );

	BDBG_LEAVE(BVDC_Window_DisableBoxDetect);
	return BERR_TRACE(eResult);
}


/***************************************************************************
 *
 */
BERR_Code BVDC_Window_SetAspectRatioMode
	( BVDC_Window_Handle               hWindow,
	  BVDC_AspectRatioMode             eAspectRatioMode )
{
	BDBG_ENTER(BVDC_Window_SetAspectRatioMode);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	/* set new value */
	hWindow->stNewInfo.eAspectRatioMode = eAspectRatioMode;

	BDBG_LEAVE(BVDC_Window_SetAspectRatioMode);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Window_GetAspectRatioMode
	( const BVDC_Window_Handle         hWindow,
	  BVDC_AspectRatioMode            *peAspectRatioMode )
{
	BDBG_ENTER(BVDC_Window_GetAspectRatioMode);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	if(peAspectRatioMode)
	{
		*peAspectRatioMode = hWindow->stCurInfo.eAspectRatioMode;
	}

	BDBG_LEAVE(BVDC_Window_GetAspectRatioMode);
	return BERR_SUCCESS;
}


/***************************************************************************
 *
 */
BERR_Code BVDC_Window_SetNonLinearScl(
	  BVDC_Window_Handle               hWindow,
	  uint32_t                         ulNonLinearSrcWidth,
	  uint32_t                         ulNonLinearSclOutWidth)
{
	BDBG_ENTER(BVDC_Window_SetNonLinearScl);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	/* set new value */
	hWindow->stNewInfo.ulNonlinearSrcWidth = ulNonLinearSrcWidth;
	hWindow->stNewInfo.ulNonlinearSclOutWidth = ulNonLinearSclOutWidth;

	BDBG_LEAVE(BVDC_Window_SetNonLinearScl);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Window_GetNonLinearScl
	( const BVDC_Window_Handle         hWindow,
	  uint32_t                        *pulNonLinearSrcWidth,
	  uint32_t                        *pulNonLinearSclOutWidth)
{
	BDBG_ENTER(BVDC_Window_GetHorizScale);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	if(pulNonLinearSrcWidth)
	{
		*pulNonLinearSrcWidth = hWindow->stCurInfo.ulNonlinearSrcWidth;
	}
	if(pulNonLinearSclOutWidth)
	{
		*pulNonLinearSclOutWidth = hWindow->stCurInfo.ulNonlinearSclOutWidth;
	}

	BDBG_LEAVE(BVDC_Window_GetHorizScale);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Window_SetScaleFactorRounding
	( BVDC_Window_Handle               hWindow,
	  uint32_t                         ulHrzTolerance,
	  uint32_t                         ulVrtTolerance )
{
	BDBG_ENTER(BVDC_Window_SetScaleFactorRounding);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	/* set new value */
	hWindow->stNewInfo.ulHrzSclFctRndToler = ulHrzTolerance;
	hWindow->stNewInfo.ulVrtSclFctRndToler = ulVrtTolerance;

	BDBG_LEAVE(BVDC_Window_SetScaleFactorRounding);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Window_GetScaleFactorRounding
	( BVDC_Window_Handle               hWindow,
	  uint32_t                        *pulHrzTolerance,
	  uint32_t                        *pulVrtTolerance )
{
	BDBG_ENTER(BVDC_Window_GetScaleFactorRounding);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	if(pulHrzTolerance)
	{
		*pulHrzTolerance = hWindow->stCurInfo.ulHrzSclFctRndToler;
	}
	if(pulVrtTolerance)
	{
		*pulVrtTolerance = hWindow->stCurInfo.ulVrtSclFctRndToler;
	}

	BDBG_LEAVE(BVDC_Window_GetScaleFactorRounding);
	return BERR_SUCCESS;
}


/***************************************************************************
 *
 */
BERR_Code BVDC_Window_SetPhaseAdjustMode
	( BVDC_Window_Handle               hWindow,
	  BVDC_PhaseAdjustMode             ePhaseAdjustMode )
{
	BDBG_ENTER(BVDC_Window_SetPhaseAdjustMode);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	if(ePhaseAdjustMode > BVDC_PhaseAdjustMode_eTrackAndClamp)
	{
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* set new value */
	hWindow->stNewInfo.ePhaseAdjustMode = ePhaseAdjustMode;

	BDBG_LEAVE(BVDC_Window_SetPhaseAdjustMode);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Window_GetPhaseAdjustMode
	( const BVDC_Window_Handle         hWindow,
	  BVDC_PhaseAdjustMode            *pePhaseAdjustMode )
{
	BDBG_ENTER(BVDC_Window_GetPhaseAdjustMode);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	if(pePhaseAdjustMode)
	{
		*pePhaseAdjustMode = hWindow->stCurInfo.ePhaseAdjustMode;
	}

	BDBG_LEAVE(BVDC_Window_GetPhaseAdjustMode);
	return BERR_SUCCESS;
}


/***************************************************************************
 *
 */
BERR_Code BVDC_Window_SetMasterFrameRate
	( BVDC_Window_Handle               hWindow,
	  bool                             bUseSrcFrameRate )
{
	BDBG_ENTER(BVDC_Window_SetMasterFrameRate);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	/* set new value */
	hWindow->stNewInfo.bUseSrcFrameRate = bUseSrcFrameRate;

	BDBG_LEAVE(BVDC_Window_SetMasterFrameRate);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Window_GetMasterFrameRate
	( const BVDC_Window_Handle         hWindow,
	  bool                            *pbUseSrcFrameRate )
{
	BDBG_ENTER(BVDC_Window_GetMasterFrameRate);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	if(pbUseSrcFrameRate)
	{
		*pbUseSrcFrameRate = hWindow->stCurInfo.bUseSrcFrameRate;
	}

	BDBG_LEAVE(BVDC_Window_GetMasterFrameRate);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Window_SetRgbMatching
	( BVDC_Window_Handle               hWindow,
	  bool                             bRgbMatching )
{
	BDBG_ENTER(BVDC_Window_SetRgbMatching);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	/* set new value */
	hWindow->stNewInfo.bCscRgbMatching = bRgbMatching;

	BDBG_LEAVE(BVDC_Window_SetRgbMatching);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Window_GetRgbMatching
	( const BVDC_Window_Handle         hWindow,
	  bool                            *pbRgbMatching )
{
	BDBG_ENTER(BVDC_Window_GetRgbMatching);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	if(pbRgbMatching)
	{
		*pbRgbMatching = hWindow->stCurInfo.bCscRgbMatching;
	}

	BDBG_LEAVE(BVDC_Window_GetRgbMatching);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Window_SetContrast
	( BVDC_Window_Handle               hWindow,
	  int16_t                          sContrast )
{
	BDBG_ENTER(BVDC_Window_SetContrast);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	hWindow->stNewInfo.sContrast = sContrast;

	BDBG_LEAVE(BVDC_Window_SetContrast);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Window_GetContrast
	( BVDC_Window_Handle               hWindow,
	  int16_t                         *psContrast )
{
	BDBG_ENTER(BVDC_Window_GetContrast);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	if(psContrast)
	{
		*psContrast = hWindow->stCurInfo.sContrast;
	}

	BDBG_LEAVE(BVDC_Window_GetContrast);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Window_SetSaturation
	( BVDC_Window_Handle               hWindow,
	  int16_t                          sSaturation )
{
	BDBG_ENTER(BVDC_Window_SetSaturation);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	hWindow->stNewInfo.sSaturation = sSaturation;

	BDBG_LEAVE(BVDC_Window_SetSaturation);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Window_GetSaturation
	( BVDC_Window_Handle               hWindow,
	  int16_t                         *psSaturation )
{
	BDBG_ENTER(BVDC_Window_GetSaturation);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	if(psSaturation)
	{
		*psSaturation = hWindow->stCurInfo.sSaturation;
	}

	BDBG_LEAVE(BVDC_Window_GetSaturation);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Window_SetHue
	( BVDC_Window_Handle               hWindow,
	  int16_t                          sHue )
{
	BDBG_ENTER(BVDC_Window_SetHue);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	hWindow->stNewInfo.sHue = sHue;

	BDBG_LEAVE(BVDC_Window_SetHue);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Window_GetHue
	( BVDC_Window_Handle               hWindow,
	  int16_t                         *psHue )
{
	BDBG_ENTER(BVDC_Window_GetHue);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	if(psHue)
	{
		*psHue = hWindow->stCurInfo.sHue;
	}

	BDBG_LEAVE(BVDC_Window_GetHue);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Window_SetBrightness
	( BVDC_Window_Handle               hWindow,
	  int16_t                          sBrightness )
{
	BDBG_ENTER(BVDC_Window_SetBrightness);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	hWindow->stNewInfo.sBrightness = sBrightness;

	BDBG_LEAVE(BVDC_Window_SetBrightness);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Window_GetBrightness
	( BVDC_Window_Handle               hWindow,
	  int16_t                         *psBrightness )
{
	BDBG_ENTER(BVDC_Window_GetBrightness);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	if(psBrightness)
	{
		*psBrightness = hWindow->stCurInfo.sBrightness;
	}

	BDBG_LEAVE(BVDC_Window_GetBrightness);
	return BERR_SUCCESS;
}

#if (BVDC_P_SUPPORT_TNT_VER >= 6)            /* TNT-R HW base */
BERR_Code BVDC_Window_SetSharpnessConfig
	( BVDC_Window_Handle               hWindow,
	  const BVDC_SharpnessSettings    *pSharpnessSettings )
{
	BSTD_UNUSED(hWindow);
	BSTD_UNUSED(pSharpnessSettings);
	return BERR_TRACE(BVDC_ERR_TNT_WINDOW_NOT_SUPPORTED);
}


BERR_Code BVDC_Window_GetSharpnessConfig
	( BVDC_Window_Handle               hWindow,
	  BVDC_SharpnessSettings          *pSharpnessSettings )
{
	BSTD_UNUSED(hWindow);
	BSTD_UNUSED(pSharpnessSettings);
	return BERR_TRACE(BVDC_ERR_TNT_WINDOW_NOT_SUPPORTED);
}
#elif (BVDC_P_SUPPORT_TNT_VER == 5)            /* TNT2 HW base */
BERR_Code BVDC_Window_SetSharpnessConfig
	( BVDC_Window_Handle               hWindow,
	  const BVDC_SharpnessSettings    *pSharpnessSettings )

{
	BERR_Code err = BERR_SUCCESS;

	BDBG_ENTER(BVDC_Window_SetSharpnessConfig);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	/* Only support main display's main window */
	if(hWindow->eId != BVDC_P_WindowId_eComp0_V0)
	{
		return BERR_TRACE(BVDC_ERR_TNT_WINDOW_NOT_SUPPORTED);
	}

	if(pSharpnessSettings != NULL)
	{
		err = BVDC_P_Tnt_ValidateSharpnessSettings(pSharpnessSettings);
		if (err != BERR_SUCCESS)
		{
			return err;
		}
		else
		{
			/* set new value */
			hWindow->stNewInfo.bUserSharpnessConfig = true;
			hWindow->stNewInfo.stSharpnessConfig = *pSharpnessSettings;

			BVDC_P_Tnt_StoreSharpnessSettings(hWindow, pSharpnessSettings);
		}
	}
	else
	{
		hWindow->stNewInfo.bUserSharpnessConfig = false;
	}

	hWindow->stNewInfo.stDirty.stBits.bTntAdjust = BVDC_P_DIRTY;
	hWindow->stNewInfo.sSharpness = 0;

	BDBG_LEAVE(BVDC_Window_SetSharpnessConfig);
	return err;

}


BERR_Code BVDC_Window_GetSharpnessConfig
	( BVDC_Window_Handle               hWindow,
	  BVDC_SharpnessSettings          *pSharpnessSettings )
{
	BERR_Code err = BERR_SUCCESS;

	BDBG_ENTER(BVDC_Window_GetSharpnessConfig);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	BDBG_ASSERT(pSharpnessSettings);

	/* Only support main display's main window */
	if(hWindow->eId != BVDC_P_WindowId_eComp0_V0)
	{
		return BERR_TRACE(BVDC_ERR_TNT_WINDOW_NOT_SUPPORTED);
	}

	if(pSharpnessSettings)
	{
		*pSharpnessSettings = hWindow->stCurInfo.stSharpnessConfig;
	}

	BDBG_LEAVE(BVDC_Window_GetSharpnessConfig);
	return err;

}

#else

/***************************************************************************
 *
 */
BERR_Code BVDC_Window_SetSharpnessConfig
	( BVDC_Window_Handle               hWindow,
	  const BVDC_SharpnessSettings    *pSharpnessSettings )
{
	BDBG_ENTER(BVDC_Window_SetSharpnessConfig);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	/* Only support main display's main window */
	if(hWindow->eId != BVDC_P_WindowId_eComp0_V0)
	{
		return BERR_TRACE(BVDC_ERR_TNT_WINDOW_NOT_SUPPORTED);
	}

	if(pSharpnessSettings != NULL)
	{
		/* set new value */
		hWindow->stNewInfo.bUserSharpnessConfig = true;
		hWindow->stNewInfo.stSharpnessConfig = *pSharpnessSettings;
	}
	else
	{
		hWindow->stNewInfo.bUserSharpnessConfig = false;
	}

	hWindow->stNewInfo.stDirty.stBits.bTntAdjust = BVDC_P_DIRTY;
	hWindow->stNewInfo.sSharpness = 0;

	BDBG_LEAVE(BVDC_Window_SetSharpnessConfig);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
 BERR_Code BVDC_Window_GetSharpnessConfig
	( BVDC_Window_Handle               hWindow,
	  BVDC_SharpnessSettings          *pSharpnessSettings )
{
	BDBG_ENTER(BVDC_Window_GetSharpnessConfig);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	if(pSharpnessSettings)
	{
		*pSharpnessSettings = hWindow->stCurInfo.stSharpnessConfig;
		if(!hWindow->stCurInfo.bUserSharpnessConfig)
		{
			BKNI_EnterCriticalSection();
			pSharpnessSettings->ulLumaCtrlGain = hWindow->stCurResource.hPep->ulLumaChromaGain;
			pSharpnessSettings->ulChromaCtrlGain = hWindow->stCurResource.hPep->ulLumaChromaGain;
			BKNI_LeaveCriticalSection();
		}
	}

	BDBG_LEAVE(BVDC_Window_GetSharpnessConfig);
	return BERR_SUCCESS;
}
#endif

/***************************************************************************
 *
 */
BERR_Code BVDC_Window_SetSharpness
	( BVDC_Window_Handle               hWindow,
	  bool                             bSharpnessEnable,
	  int16_t                          sSharpness )
{
	BDBG_ENTER(BVDC_Window_SetSharpness);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	/* Only support main display's main window */
	if(hWindow->eId != BVDC_P_WindowId_eComp0_V0)
	{
		return BERR_TRACE(BVDC_ERR_TNT_WINDOW_NOT_SUPPORTED);
	}

	hWindow->stNewInfo.bSharpnessEnable = bSharpnessEnable;
	hWindow->stNewInfo.sSharpness = sSharpness;

	BDBG_LEAVE(BVDC_Window_SetSharpness);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Window_GetSharpness
	( BVDC_Window_Handle               hWindow,
	  bool                            *pbSharpnessEnable,
	  int16_t                         *psSharpness )
{
	BDBG_ENTER(BVDC_Window_GetSharpness);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	if(psSharpness)
	{
		*psSharpness = hWindow->stCurInfo.sSharpness;
	}

	if(pbSharpnessEnable)
	{
		*pbSharpnessEnable = hWindow->stCurInfo.bSharpnessEnable;
	}

	BDBG_LEAVE(BVDC_Window_GetSharpness);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Window_SetColorTemp
	(BVDC_Window_Handle hWindow,
	 int16_t sColorTemp)
{
	int32_t lAttenuationR;
	int32_t lAttenuationG;
	int32_t lAttenuationB;

	BDBG_ENTER(BVDC_Window_SetColorTemp);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	hWindow->stNewInfo.sColorTemp = sColorTemp;

	if(BVDC_P_WIN_IS_GFX_WINDOW(hWindow->eId))
	{
		BVDC_P_CscCoeffs stCscCoeffs = BVDC_P_MAKE_GFD_CSC
			( 1.00000, 0.00000, 0.00000, 0.00000, 0.00000,
			  0.00000, 1.00000, 0.00000, 0.00000, 0.00000,
			  0.00000, 0.00000, 1.00000, 0.00000, 0.00000 );

		BVDC_P_Csc_ColorTempToAttenuationRGB(sColorTemp,
			&lAttenuationR, &lAttenuationG, &lAttenuationB, &stCscCoeffs);
	}
	else
	{
		BVDC_P_CscCoeffs stCscCoeffs = BVDC_P_MAKE_CMP_CSC
			( 1.00000, 0.00000, 0.00000, 0.00000,
			  0.00000, 1.00000, 0.00000, 0.00000,
			  0.00000, 0.00000, 1.00000, 0.00000 );

		BVDC_P_Csc_ColorTempToAttenuationRGB(sColorTemp,
			&lAttenuationR, &lAttenuationG, &lAttenuationB, &stCscCoeffs);
	}

	hWindow->stNewInfo.lAttenuationR = lAttenuationR;
	hWindow->stNewInfo.lAttenuationG = lAttenuationG;
	hWindow->stNewInfo.lAttenuationB = lAttenuationB;
	hWindow->stNewInfo.lOffsetR = 0;
	hWindow->stNewInfo.lOffsetG = 0;
	hWindow->stNewInfo.lOffsetB = 0;

	BDBG_LEAVE(BVDC_Window_SetColorTemp);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Window_GetColorTemp
	( BVDC_Window_Handle               hWindow,
	  int16_t                         *psColorTemp )
{
	BDBG_ENTER(BVDC_Window_GetColorTemp);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	if(psColorTemp)
	{
		*psColorTemp= hWindow->stCurInfo.sColorTemp;
	}

	BDBG_LEAVE(BVDC_Window_GetColorTemp);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Window_SetAttenuationRGB
	(BVDC_Window_Handle hWindow,
	  int32_t   nAttentuationR,
	  int32_t   nAttentuationG,
	  int32_t   nAttentuationB,
	  int32_t   nOffsetR,
	  int32_t   nOffsetG,
	  int32_t   nOffsetB)
{
	int32_t ulShiftBits;

	BDBG_ENTER(BVDC_Window_SetAttenuationRGB);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	ulShiftBits = BVDC_P_WIN_IS_GFX_WINDOW(hWindow->eId)?
		          BVDC_P_CSC_GFD_CX_F_BITS - 11 :
		          BVDC_P_CSC_CMP_CX_F_BITS - 11;

	hWindow->stNewInfo.lAttenuationR = nAttentuationR << ulShiftBits;
	hWindow->stNewInfo.lAttenuationG = nAttentuationG << ulShiftBits;
	hWindow->stNewInfo.lAttenuationB = nAttentuationB << ulShiftBits;
	hWindow->stNewInfo.lOffsetR      = nOffsetR       << ulShiftBits;
	hWindow->stNewInfo.lOffsetG      = nOffsetG       << ulShiftBits;
	hWindow->stNewInfo.lOffsetB      = nOffsetB       << ulShiftBits;

	BDBG_LEAVE(BVDC_Window_SetAttenuationRGB);

	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Window_GetAttenuationRGB
	( BVDC_Window_Handle               hWindow,
	  int32_t                         *plAttenuationR,
	  int32_t                         *plAttenuationG,
	  int32_t                         *plAttenuationB,
	  int32_t                         *plOffsetR,
	  int32_t                         *plOffsetG,
	  int32_t                         *plOffsetB )
{
	int32_t ulShiftBits;

	BDBG_ENTER(BVDC_Window_GetAttenuationRGB);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	ulShiftBits = BVDC_P_WIN_IS_GFX_WINDOW(hWindow->eId)?
	              BVDC_P_CSC_GFD_CX_F_BITS - 11 :
	              BVDC_P_CSC_CMP_CX_F_BITS - 11;

	(*plAttenuationR) = hWindow->stCurInfo.lAttenuationR >> ulShiftBits;
	(*plAttenuationG) = hWindow->stCurInfo.lAttenuationG >> ulShiftBits;
	(*plAttenuationB) = hWindow->stCurInfo.lAttenuationB >> ulShiftBits;
	(*plOffsetR)      = hWindow->stCurInfo.lOffsetR      >> ulShiftBits;
	(*plOffsetG)      = hWindow->stCurInfo.lOffsetG      >> ulShiftBits;
	(*plOffsetB)      = hWindow->stCurInfo.lOffsetB      >> ulShiftBits;

	BDBG_LEAVE(BVDC_Window_GetAttenuationRGB);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Window_SetAutoFlesh
	( BVDC_Window_Handle               hWindow,
	  uint32_t                         ulFleshtone )
{
	BDBG_ENTER(BVDC_Window_SetAutoFlesh);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	/* only support main display's main window */
	if(hWindow->eId != BVDC_P_WindowId_eComp0_V0)
	{
		return BERR_TRACE(BVDC_ERR_PEP_WINDOW_NOT_SUPPORT);
	}
	if(ulFleshtone > BVDC_P_PEP_MAX_CAB_SETTING_GRANUALITY)
	{
		return BERR_TRACE(BVDC_ERR_PEP_INVALID_PARAMETER);
	}

	hWindow->stNewInfo.ulFleshtone = ulFleshtone;

	BDBG_LEAVE(BVDC_Window_SetAutoFlesh);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Window_GetAutoFlesh
	( const BVDC_Window_Handle          hWindow,
	  uint32_t                         *pulFleshtone )
{
	BDBG_ENTER(BVDC_Window_GetAutoFlesh);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	if(pulFleshtone)
	{
		*pulFleshtone = hWindow->stCurInfo.ulFleshtone;
	}

	BDBG_LEAVE(BVDC_Window_GetAutoFlesh);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Window_SetGreenBoost
	( BVDC_Window_Handle               hWindow,
	  uint32_t                         ulGreenBoost )
{
	BDBG_ENTER(BVDC_Window_SetGreenBoost);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	/* only support main display's main window */
	if(hWindow->eId != BVDC_P_WindowId_eComp0_V0)
	{
		return BERR_TRACE(BVDC_ERR_PEP_WINDOW_NOT_SUPPORT);
	}
	if(ulGreenBoost > BVDC_P_PEP_MAX_CAB_SETTING_GRANUALITY)
	{
		return BERR_TRACE(BVDC_ERR_PEP_INVALID_PARAMETER);
	}

	hWindow->stNewInfo.ulGreenBoost = ulGreenBoost;

	BDBG_LEAVE(BVDC_Window_SetGreenBoost);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Window_GetGreenBoost
	( const BVDC_Window_Handle          hWindow,
	  uint32_t                         *pulGreenBoost )
{
	BDBG_ENTER(BVDC_Window_GetGreenBoost);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	if(pulGreenBoost)
	{
		*pulGreenBoost= hWindow->stCurInfo.ulGreenBoost;
	}

	BDBG_LEAVE(BVDC_Window_GetGreenBoost);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Window_SetBlueBoost
	( BVDC_Window_Handle               hWindow,
	  uint32_t                         ulBlueBoost )
{
	BDBG_ENTER(BVDC_Window_SetBlueBoost);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	/* only support main display's main window */
	if(hWindow->eId != BVDC_P_WindowId_eComp0_V0)
	{
		return BERR_TRACE(BVDC_ERR_PEP_WINDOW_NOT_SUPPORT);
	}
	if(ulBlueBoost > BVDC_P_PEP_MAX_CAB_SETTING_GRANUALITY)
	{
		return BERR_TRACE(BVDC_ERR_PEP_INVALID_PARAMETER);
	}

	hWindow->stNewInfo.ulBlueBoost = ulBlueBoost;

	BDBG_LEAVE(BVDC_Window_SetBlueBoost);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Window_GetBlueBoost
	( const BVDC_Window_Handle          hWindow,
	  uint32_t                         *pulBlueBoost )
{
	BDBG_ENTER(BVDC_Window_GetBlueBoost);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	if(pulBlueBoost)
	{
		*pulBlueBoost = hWindow->stCurInfo.ulBlueBoost;
	}

	BDBG_LEAVE(BVDC_Window_GetBlueBoost);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Window_SetCmsControl
	( const BVDC_Window_Handle          hWindow,
	  const BVDC_ColorBar              *pSaturationGain,
	  const BVDC_ColorBar              *pHueGain )
{
	BDBG_ENTER(BVDC_Window_SetCmsControl);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	/* only support main display's main window */
	if(hWindow->eId != BVDC_P_WindowId_eComp0_V0)
	{
		return BERR_TRACE(BVDC_ERR_PEP_WINDOW_NOT_SUPPORT);
	}
	if(pSaturationGain &&
	   !BVDC_P_PEP_CMS_SAT_WITHIN_RANGE(pSaturationGain))
	{
		return BERR_TRACE(BVDC_ERR_PEP_INVALID_PARAMETER);
	}
	if(pHueGain &&
	   !BVDC_P_PEP_CMS_HUE_WITHIN_RANGE(pHueGain))
	{
		return BERR_TRACE(BVDC_ERR_PEP_INVALID_PARAMETER);
	}

	if(pSaturationGain)
	{
		hWindow->stNewInfo.stSatGain = *pSaturationGain;
	}
	if(pHueGain)
	{
		hWindow->stNewInfo.stHueGain = *pHueGain;
	}

	BDBG_LEAVE(BVDC_Window_SetCmsControl);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Window_GetCmsControl
	( const BVDC_Window_Handle          hWindow,
	  BVDC_ColorBar                    *pSaturationGain,
	  BVDC_ColorBar                    *pHueGain )
{
	BDBG_ENTER(BVDC_Window_GetCmsControl);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	if(pSaturationGain)
	{
		*pSaturationGain = hWindow->stCurInfo.stSatGain;
	}

	if(pHueGain)
	{
		*pHueGain = hWindow->stCurInfo.stHueGain;
	}

	BDBG_LEAVE(BVDC_Window_GetCmsControl);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Window_EnableContrastStretch
	( BVDC_Window_Handle               hWindow,
	  bool                             bEnable )
{
	BDBG_ENTER(BVDC_Window_EnableContrastStretch);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	/* only support main display's main window */
	if(hWindow->eId != BVDC_P_WindowId_eComp0_V0)
	{
		return BERR_TRACE(BVDC_ERR_PEP_WINDOW_NOT_SUPPORT);
	}

	hWindow->stNewInfo.bContrastStretch   = bEnable;

	BDBG_LEAVE(BVDC_Window_SetContrastStretch);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Window_SetContrastStretch
	( BVDC_Window_Handle               hWindow,
	  const BVDC_ContrastStretch      *pContrastStretch )
{
	BDBG_ENTER(BVDC_Window_SetContrastStretch);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	/* only support main display's main window */
	if(hWindow->eId != BVDC_P_WindowId_eComp0_V0)
	{
		return BERR_TRACE(BVDC_ERR_PEP_WINDOW_NOT_SUPPORT);
	}

	/*
	* Validating user data:
	* - All values in the table should be 0-1023.
	* - The first column should start at 64 or less, and finish at 940 or more,
	*   and should always be increasing (avoids the zero problem we have found here).
	* - The values along each row (excluding the first column) should always be increasing.
	* - The values in the IRE table should always be increasing, with increments
	*   of at least 16 between each entry.
	*/
	if(pContrastStretch && pContrastStretch->aulDcTable1[0] != 0)
	{
		uint32_t i, j = 0;
		for(i = 0; i < BVDC_DC_TABLE_ROWS * BVDC_DC_TABLE_COLS; i++)
		{
			if(pContrastStretch->aulDcTable1[i] > BVDC_P_PEP_MAX_LUMA_VALUE ||
				(pContrastStretch->bInterpolateTables &&
				pContrastStretch->aulDcTable2[i] > BVDC_P_PEP_MAX_LUMA_VALUE))
			{
				BDBG_MSG(("No value in the table should be greater than %d", BVDC_P_PEP_MAX_LUMA_VALUE));
				BDBG_MSG(("aulDcTable1[%d] = %d, bInterpolateTables = %d, aulDcTable2[%d] = %d",
					i, pContrastStretch->aulDcTable1[i], pContrastStretch->bInterpolateTables,
					i, pContrastStretch->aulDcTable2[i]));
				return BERR_TRACE(BVDC_ERR_PEP_INVALID_PARAMETER);
			}

			if(j == 1 &&
				(pContrastStretch->aulDcTable1[i] > BVDC_P_PEP_BLACK_LUMA_VALUE ||
				(pContrastStretch->bInterpolateTables &&
				pContrastStretch->aulDcTable2[i] > BVDC_P_PEP_BLACK_LUMA_VALUE)))
			{
				BDBG_MSG(("The first point on each curve should be %d or less (black level)", BVDC_P_PEP_BLACK_LUMA_VALUE));
				BDBG_MSG(("aulDcTable1[%d] = %d, bInterpolateTables = %d, aulDcTable2[%d] = %d",
					i, pContrastStretch->aulDcTable1[i], pContrastStretch->bInterpolateTables,
					i, pContrastStretch->aulDcTable2[i]));
				return BERR_TRACE(BVDC_ERR_PEP_INVALID_PARAMETER);
			}

			if(j > 0 && j < (BVDC_DC_TABLE_COLS - 1) &&
				(i < (BVDC_DC_TABLE_ROWS * BVDC_DC_TABLE_COLS - 1)) &&
				(pContrastStretch->aulDcTable1[i] > pContrastStretch->aulDcTable1[i + 1] ||
				(pContrastStretch->bInterpolateTables &&
				pContrastStretch->aulDcTable2[i] > pContrastStretch->aulDcTable2[i + 1])))
			{
				BDBG_MSG(("Each point on a curve should be greater than or equal to the previous point on the curve (monotonically increasing along the curve)"));
				BDBG_MSG(("aulDcTable1[%d] = %d, aulDcTable1[%d] = %d, bInterpolateTables = %d, aulDcTable2[%d] = %d, aulDcTable2[%d] = %d",
					i, pContrastStretch->aulDcTable1[i], i+1, pContrastStretch->aulDcTable1[i+1],
					pContrastStretch->bInterpolateTables,
					i, pContrastStretch->aulDcTable2[i], i+1, pContrastStretch->aulDcTable2[i + 1]));
				return BERR_TRACE(BVDC_ERR_PEP_INVALID_PARAMETER);
			}

			j++;
			if(j == BVDC_DC_TABLE_COLS)
			{
				if(pContrastStretch->aulDcTable1[i] < BVDC_P_PEP_WHITE_LUMA_VALUE ||
					(pContrastStretch->bInterpolateTables &&
					pContrastStretch->aulDcTable2[i] < BVDC_P_PEP_WHITE_LUMA_VALUE))
				{
					BDBG_MSG(("The last point on each curve should be %d or more (white level)", BVDC_P_PEP_WHITE_LUMA_VALUE));
					BDBG_MSG(("aulDcTable1[%d] = %d, bInterpolateTables = %d, aulDcTable2[%d] = %d",
						i, pContrastStretch->aulDcTable1[i], pContrastStretch->bInterpolateTables,
						i, pContrastStretch->aulDcTable2[i]));
					return BERR_TRACE(BVDC_ERR_PEP_INVALID_PARAMETER);
				}
				j = 0;
			}
		}

		for(i = 1; i < BVDC_DC_TABLE_COLS - 1; i++)
		{
			if((pContrastStretch->alIreTable[i] - pContrastStretch->alIreTable[i - 1]) < 16)
			{
				BDBG_MSG(("Steps between values in the IRE table must be at least 16"));
				BDBG_MSG(("alIreTable[%d] = %d, alIreTable[%d] = %d",
					i, pContrastStretch->alIreTable[i], i - 1, pContrastStretch->alIreTable[i - 1]));
				return BERR_TRACE(BVDC_ERR_PEP_INVALID_PARAMETER);
			}
		}
	}

	if(pContrastStretch)
	{
		hWindow->stNewInfo.stContrastStretch = *pContrastStretch;
	}

	hWindow->stNewInfo.stDirty.stBits.bLabAdjust = BVDC_P_DIRTY;

	BDBG_LEAVE(BVDC_Window_SetContrastStretch);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Window_GetContrastStretch
	( const BVDC_Window_Handle          hWindow,
	  BVDC_ContrastStretch             *pContrastStretch )
{
	BDBG_ENTER(BVDC_Window_GetContrastStretch);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	if(pContrastStretch)
	{
		*pContrastStretch = hWindow->stCurInfo.stContrastStretch;
	}

	BDBG_LEAVE(BVDC_Window_GetContrastStretch);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Window_SetBlueStretch
	( BVDC_Window_Handle               hWindow,
	  const BVDC_BlueStretch          *pBlueStretch )
{
	BDBG_ENTER(BVDC_Window_SetBlueStretch);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	/* only support main display's main window */
	if(hWindow->eId != BVDC_P_WindowId_eComp0_V0)
	{
		return BERR_TRACE(BVDC_ERR_PEP_WINDOW_NOT_SUPPORT);
	}

	if(pBlueStretch && BVDC_P_SUPPORT_PEP_VER == BVDC_P_SUPPORT_PEP_VER_3)
	{
		hWindow->stNewInfo.stBlueStretch = *pBlueStretch;
		hWindow->stNewInfo.bBlueStretch = true;
	}
	else
	{
		hWindow->stNewInfo.bBlueStretch = false;
	}

	BDBG_LEAVE(BVDC_Window_SetBlueStretch);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Window_GetBlueStretch
	( const BVDC_Window_Handle          hWindow,
	  BVDC_BlueStretch                 *pBlueStretch )
{
	BDBG_ENTER(BVDC_Window_GetBlueStretch);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	if(pBlueStretch)
	{
		*pBlueStretch = hWindow->stCurInfo.stBlueStretch;
	}

	BDBG_LEAVE(BVDC_Window_GetBlueStretch);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Window_SetSplitScreenMode
	( BVDC_Window_Handle                      hWindow,
	  const BVDC_Window_SplitScreenSettings  *pSplitScreen )
{
	BVDC_Window_SplitScreenSettings *pSetting;

	BDBG_ENTER(BVDC_Window_SetSplitScreenMode);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	pSetting = &hWindow->stNewInfo.stSplitScreenSetting;

#if (BCHP_CHIP == 7400) && (BCHP_VER >= BCHP_VER_B0)
	if((pSplitScreen->eHue             != BVDC_SplitScreenMode_eDisable ||
	    pSplitScreen->eContrast        != BVDC_SplitScreenMode_eDisable ||
	    pSplitScreen->eBrightness      != BVDC_SplitScreenMode_eDisable ||
	    pSplitScreen->eColorTemp       != BVDC_SplitScreenMode_eDisable) &&
	   (hWindow->eId != BVDC_P_WindowId_eComp0_V0 &&
	    hWindow->eId != BVDC_P_WindowId_eComp1_V0 &&
	    hWindow->eId != BVDC_P_WindowId_eComp2_V0))
#else
	if((pSplitScreen->eHue             != BVDC_SplitScreenMode_eDisable ||
	    pSplitScreen->eContrast        != BVDC_SplitScreenMode_eDisable ||
	    pSplitScreen->eBrightness      != BVDC_SplitScreenMode_eDisable ||
	    pSplitScreen->eColorTemp       != BVDC_SplitScreenMode_eDisable) &&
	   (hWindow->eId != BVDC_P_WindowId_eComp0_V0))
#endif
	{
		return BERR_TRACE(BVDC_ERR_PEP_WINDOW_NOT_SUPPORT);
	}

	if((pSplitScreen->eAutoFlesh       != BVDC_SplitScreenMode_eDisable ||
	    pSplitScreen->eSharpness       != BVDC_SplitScreenMode_eDisable ||
	    pSplitScreen->eBlueBoost       != BVDC_SplitScreenMode_eDisable ||
	    pSplitScreen->eGreenBoost      != BVDC_SplitScreenMode_eDisable ||
	    pSplitScreen->eCms             != BVDC_SplitScreenMode_eDisable ||
	    pSplitScreen->eContrastStretch != BVDC_SplitScreenMode_eDisable ||
	    pSplitScreen->eBlueStretch     != BVDC_SplitScreenMode_eDisable) &&
	   (hWindow->eId != BVDC_P_WindowId_eComp0_V0))
	{
		return BERR_TRACE(BVDC_ERR_PEP_WINDOW_NOT_SUPPORT);
	}

	/* Since Auto Flesh, Green Boost and Blue Boost are controlled */
	/* by CAB, their setting can't be different */
	if((pSplitScreen->eAutoFlesh != pSplitScreen->eBlueBoost)  ||
	   (pSplitScreen->eAutoFlesh != pSplitScreen->eGreenBoost) ||
	   (pSplitScreen->eBlueBoost != pSplitScreen->eGreenBoost))
	{
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

#if (BVDC_P_SUPPORT_PEP_VER == BVDC_P_SUPPORT_PEP_VER_3)
	/* Since contrast stretch and blue stretch are controlled by LAB, */
	/* their setting can't be different */
	if(pSplitScreen->eContrastStretch != pSplitScreen->eBlueStretch)
	{
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}
#endif

	/* User can select which features of the display (hue, brightness, contrast,  */
	/* or colortemp) to apply to the split screen demo mode but they have */
	/* to be applied to the same side of the screen */
	if(pSplitScreen->eHue != BVDC_SplitScreenMode_eDisable &&
	   ((pSplitScreen->eBrightness != BVDC_SplitScreenMode_eDisable &&
	     pSplitScreen->eBrightness != pSplitScreen->eHue) ||
	    (pSplitScreen->eContrast != BVDC_SplitScreenMode_eDisable &&
	     pSplitScreen->eContrast != pSplitScreen->eHue) ||
	    (pSplitScreen->eColorTemp != BVDC_SplitScreenMode_eDisable &&
	     pSplitScreen->eColorTemp != pSplitScreen->eHue)))
	{
		BDBG_ERR(("Hue=%d, Brightness=%d, Contrast=%d, ColorTemp=%d",
			pSplitScreen->eHue, pSplitScreen->eBrightness, pSplitScreen->eContrast, pSplitScreen->eColorTemp));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}
	if(pSplitScreen->eBrightness != BVDC_SplitScreenMode_eDisable &&
	   ((pSplitScreen->eHue != BVDC_SplitScreenMode_eDisable &&
	     pSplitScreen->eHue != pSplitScreen->eBrightness) ||
	    (pSplitScreen->eContrast != BVDC_SplitScreenMode_eDisable &&
	     pSplitScreen->eContrast != pSplitScreen->eBrightness) ||
		(pSplitScreen->eColorTemp != BVDC_SplitScreenMode_eDisable &&
	     pSplitScreen->eColorTemp != pSplitScreen->eBrightness)))
	{
		BDBG_ERR(("Brightness=%d, Hue=%d, Contrast=%d, ColorTemp=%d",
			pSplitScreen->eBrightness, pSplitScreen->eHue, pSplitScreen->eContrast, pSplitScreen->eColorTemp));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}
	if(pSplitScreen->eContrast != BVDC_SplitScreenMode_eDisable &&
	   ((pSplitScreen->eHue != BVDC_SplitScreenMode_eDisable &&
	     pSplitScreen->eHue != pSplitScreen->eContrast) ||
	    (pSplitScreen->eBrightness != BVDC_SplitScreenMode_eDisable &&
	     pSplitScreen->eBrightness != pSplitScreen->eContrast) ||
	    (pSplitScreen->eColorTemp != BVDC_SplitScreenMode_eDisable &&
	     pSplitScreen->eColorTemp != pSplitScreen->eContrast)))
	{
		BDBG_ERR(("Contrast=%d, Hue=%d, Brightness=%d, ColorTemp=%d",
			pSplitScreen->eContrast, pSplitScreen->eHue, pSplitScreen->eBrightness, pSplitScreen->eColorTemp));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}
	if(pSplitScreen->eColorTemp != BVDC_SplitScreenMode_eDisable &&
	   ((pSplitScreen->eHue != BVDC_SplitScreenMode_eDisable &&
	     pSplitScreen->eHue != pSplitScreen->eColorTemp) ||
	    (pSplitScreen->eBrightness != BVDC_SplitScreenMode_eDisable &&
	     pSplitScreen->eBrightness != pSplitScreen->eColorTemp) ||
	    (pSplitScreen->eContrast != BVDC_SplitScreenMode_eDisable &&
	     pSplitScreen->eContrast != pSplitScreen->eColorTemp)))
	{
		BDBG_ERR(("ColorTemp=%d, Hue=%d, Brightness=%d, Contrast=%d",
			pSplitScreen->eColorTemp, pSplitScreen->eContrast, pSplitScreen->eHue, pSplitScreen->eBrightness));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	if(pSplitScreen->eCm3d != BVDC_SplitScreenMode_eDisable &&
	   (pSplitScreen->eCms != BVDC_SplitScreenMode_eDisable ||
	    pSplitScreen->eAutoFlesh != BVDC_SplitScreenMode_eDisable ||
	    pSplitScreen->eBlueBoost != BVDC_SplitScreenMode_eDisable ||
	    pSplitScreen->eGreenBoost != BVDC_SplitScreenMode_eDisable ||
	    pSplitScreen->eBlueStretch != BVDC_SplitScreenMode_eDisable))
	{
		BDBG_ERR(("Cms, AutoFlesh, BlueBoost, GreenBoost, and BlueStretch cannot be enabled if CM3D is enabled."));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	if(pSplitScreen->eCm3d != BVDC_SplitScreenMode_eDisable &&
	   pSplitScreen->eContrastStretch != BVDC_SplitScreenMode_eDisable &&
	   pSplitScreen->eCm3d != pSplitScreen->eContrastStretch)
	{
		BDBG_ERR(("Demo setting for CM3D and dynamic contrast need to be same when enabled."));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* Indicate require copy of new bits */
	if((pSetting->eDeJagging != pSplitScreen->eDeJagging) ||
	   (pSetting->eDeRinging != pSplitScreen->eDeRinging))
	{
		hWindow->stNewInfo.stDirty.stBits.bMiscCtrl = BVDC_P_DIRTY;
	}

	*pSetting = *pSplitScreen;

	BDBG_LEAVE(BVDC_Window_SetSplitScreenMode);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Window_GetSplitScreenMode
	( const BVDC_Window_Handle          hWindow,
	  BVDC_Window_SplitScreenSettings  *pSplitScreen )
{
	BDBG_ENTER(BVDC_Window_GetSplitScreenMode);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	if(pSplitScreen)
	{
		*pSplitScreen = hWindow->stCurInfo.stSplitScreenSetting;
	}

	BDBG_LEAVE(BVDC_Window_GetSplitScreenMode);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Window_SetForceCapture
	( BVDC_Window_Handle               hWindow,
	  bool                             bForceCapture )
{
	BDBG_ENTER(BVDC_Window_SetForceCapture);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	hWindow->stNewInfo.bForceCapture = bForceCapture;

	BDBG_LEAVE(BVDC_Window_SetForceCapture);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Window_GetForceCapture
	( const BVDC_Window_Handle          hWindow,
	  bool                             *pbForceCapture )
{
	BDBG_ENTER(BVDC_Window_GetForceCapture);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	if(pbForceCapture)
	{
		*pbForceCapture = hWindow->stCurInfo.bForceCapture;
	}

	BDBG_LEAVE(BVDC_Window_GetForceCapture);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Window_GetMaxDelay
	( const BVDC_Window_Handle         hWindow,
	  unsigned int                    *puiMaxVsyncDelay )
{
	BDBG_ENTER(BVDC_Window_GetMaxDelay);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	/* Base on if this window has deinterlacer, and/or capture. */
	if(puiMaxVsyncDelay)
	{
		*puiMaxVsyncDelay  = BVDC_P_LIP_SYNC_VEC_DELAY;
		*puiMaxVsyncDelay += (hWindow->pResource->bRequirePlayback)
			? BVDC_P_LIP_SYNC_CAP_PLK_SLIP_DELAY : 0;
		*puiMaxVsyncDelay += (hWindow->pResourceFeature->ulMad != BVDC_P_Able_eInvalid)
			? BVDC_P_LIP_SYNC_DEINTERLACED_DELAY : 0;
	}

	BDBG_LEAVE(BVDC_Window_GetMaxDelay);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Window_SetDelayOffset
	( BVDC_Window_Handle               hWindow,
	  unsigned int                     uiVsyncDelayOffset )
{
	BDBG_ENTER(BVDC_Window_SetDelayOffset);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	hWindow->stNewInfo.uiVsyncDelayOffset = uiVsyncDelayOffset;

	BDBG_LEAVE(BVDC_Window_SetDelayOffset);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Window_GetDelayOffset
	( const BVDC_Window_Handle         hWindow,
	  unsigned int                    *puiVsyncDelayOffset )
{
	BDBG_ENTER(BVDC_Window_GetDelayOffset);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	if( puiVsyncDelayOffset )
	{
		*puiVsyncDelayOffset = hWindow->stCurInfo.uiVsyncDelayOffset;
	}

	BDBG_LEAVE(BVDC_Window_GetDelayOffset);
	return BERR_SUCCESS;
}


/***************************************************************************
 *
 */
BERR_Code BVDC_Window_SetMosaicConfiguration
	( BVDC_Window_Handle               hWindow,
	  bool                             bEnable,
	  const BVDC_MosaicConfiguration  *pSettings )
{
#if BVDC_P_SUPPORT_MOSAIC_MODE
	BDBG_ENTER(BVDC_Window_SetMosaicConfiguration);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	/* preliminary error checking */
	if(bEnable && (!pSettings || !hWindow->bClearRectSupport || hWindow->stNewInfo.hSource->hVdc->stSettings.bDisableMosaic))
	{
		if(!pSettings)
		{
			BDBG_ERR(("Please provide Moasic configuration settings!"));
		}
		if(!hWindow->bClearRectSupport)
		{
			BDBG_ERR(("This window[%d] does not support Mosaic Mode!", hWindow->eId));
		}
		if(hWindow->stNewInfo.hSource->hVdc->stSettings.bDisableMosaic)
		{
			BDBG_ERR(("This window[%d] mosaic disabled by BVDC_Settings.", hWindow->eId));
		}
		return BERR_TRACE(BVDC_ERR_INVALID_MOSAIC_MODE);
	}

	hWindow->stNewInfo.bClearRect = bEnable;
	if(bEnable)
	{
		uint32_t ulARGB;

		/* 8-bit settings for now */
		if((pSettings->ulClearRectAlpha > 255) ||
		   (pSettings->bClearRectByMaskColor &&
 		    (pSettings->ulMaskColorBlue  |
 		     pSettings->ulMaskColorGreen |
 		     pSettings->ulMaskColorRed) > 255))
		{
			return BERR_TRACE(BVDC_ERR_INVALID_MOSAIC_MODE);
		}

		if(!BVDC_P_SRC_IS_MPEG(hWindow->stNewInfo.hSource->eId) &&
		   pSettings->bVideoInMosaics)
		{
			BDBG_ERR(("Video in Mosaic Mode must be MPEG window!"));
			return(BVDC_ERR_INVALID_MOSAIC_MODE);
		}

		hWindow->stNewInfo.stDirty.stBits.bMosaicMode = BVDC_P_DIRTY;
		hWindow->stNewInfo.bMosaicMode   = pSettings->bVideoInMosaics;
		hWindow->stNewInfo.bClearRectByMaskColor = pSettings->bClearRectByMaskColor;
		hWindow->stNewInfo.ulClearRectAlpha = pSettings->ulClearRectAlpha;
		ulARGB = BPXL_MAKE_PIXEL(BPXL_eA8_R8_G8_B8,
			0x00, pSettings->ulMaskColorRed, pSettings->ulMaskColorGreen,
			pSettings->ulMaskColorBlue);
		BPXL_ConvertPixel_RGBtoYCbCr(BPXL_eA8_Y8_Cb8_Cr8, BPXL_eA8_R8_G8_B8,
			ulARGB, (unsigned int*)&hWindow->stNewInfo.ulMaskColorYCrCb);
	}
	else
	{
		hWindow->stNewInfo.bMosaicMode = false;
	}
	BDBG_LEAVE(BVDC_Window_SetMosaicConfiguration);
#else
	BDBG_ERR(("This chipset doesn't support Mosaic Mode!"));
	BSTD_UNUSED(hWindow);
	BSTD_UNUSED(bEnable);
	BSTD_UNUSED(pSettings);
#endif
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Window_GetMosaicConfiguration
	( const BVDC_Window_Handle         hWindow,
	  bool                            *pbEnable,
	  BVDC_MosaicConfiguration        *pSettings )
{
#if BVDC_P_SUPPORT_MOSAIC_MODE
	BDBG_ENTER(BVDC_Window_GetMosaicConfiguration);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	/* set new value */
	if(pbEnable)
	{
		*pbEnable = hWindow->stCurInfo.bClearRect;
	}

	if(pSettings)
	{
		unsigned int uiARGB;

		/* To make sure thing get initialize */
		BKNI_Memset(pSettings, 0, sizeof(*pSettings));

		pSettings->bVideoInMosaics = hWindow->stCurInfo.bMosaicMode;
		pSettings->bClearRectByMaskColor = hWindow->stCurInfo.bClearRectByMaskColor;
		pSettings->ulClearRectAlpha = hWindow->stCurInfo.ulClearRectAlpha;
		BPXL_ConvertPixel_YCbCrtoRGB(BPXL_eA8_R8_G8_B8, BPXL_eA8_Y8_Cb8_Cr8,
			hWindow->stNewInfo.ulMaskColorYCrCb, 0, 0, &uiARGB);
		pSettings->ulMaskColorRed   = BPXL_GET_COMPONENT(BPXL_eA8_R8_G8_B8, uiARGB, 2);
		pSettings->ulMaskColorGreen = BPXL_GET_COMPONENT(BPXL_eA8_R8_G8_B8, uiARGB, 1);
		pSettings->ulMaskColorBlue  = BPXL_GET_COMPONENT(BPXL_eA8_R8_G8_B8, uiARGB, 0);
	}

	BDBG_LEAVE(BVDC_Window_GetMosaicConfiguration);
#else
	BDBG_ERR(("This chipset doesn't support Mosaic Mode!"));
	BSTD_UNUSED(hWindow);
	BSTD_UNUSED(pbEnable);
	BSTD_UNUSED(pSettings);
#endif
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Window_SetMosaicRectsVisibility
	( BVDC_Window_Handle               hWindow,
	  uint32_t                         ulRectsCount,
	  const bool                       abRectsVisible[] )
{
#if BVDC_P_SUPPORT_MOSAIC_MODE
	BDBG_ENTER(BVDC_Window_SetMosaicRectsVisibility);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	/* preliminary error checking */
	if((ulRectsCount > hWindow->stSettings.ulMaxMosaicRect) ||
	   (ulRectsCount && !abRectsVisible))
	{
		return BERR_TRACE(BVDC_ERR_INVALID_MOSAIC_MODE);
	}
	if(!hWindow->bClearRectSupport)
	{
		BDBG_ERR(("This window[%d] does not support Mosaic Mode!", hWindow->eId));
		return BERR_TRACE(BVDC_ERR_INVALID_MOSAIC_MODE);
	}
	hWindow->stNewInfo.ulMosaicCount = ulRectsCount;

	if(ulRectsCount)
	{
		BKNI_Memcpy((void*)hWindow->stNewInfo.abMosaicVisible, (void*)abRectsVisible,
			sizeof(bool) * ulRectsCount);
	}
	BDBG_LEAVE(BVDC_Window_SetMosaicRectsVisibility);
#else
	BDBG_ERR(("This chipset doesn't support Mosaic Mode!"));
	BSTD_UNUSED(hWindow);
	BSTD_UNUSED(ulRectsCount);
	BSTD_UNUSED(abRectsVisible);
#endif
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Window_SetMosaicDstRects
	( BVDC_Window_Handle               hWindow,
	  uint32_t                         ulMosaicCount,
	  const BVDC_Rect                  astRect[] )
{
#if BVDC_P_SUPPORT_MOSAIC_MODE
	uint32_t   i;

	BDBG_ENTER(BVDC_Window_SetMosaicDstRects);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	/* preliminary error checking */
	if((ulMosaicCount > hWindow->stSettings.ulMaxMosaicRect) ||
	   (ulMosaicCount && !astRect))
	{
		return BERR_TRACE(BVDC_ERR_INVALID_MOSAIC_MODE);
	}
	if(!hWindow->bClearRectSupport)
	{
		BDBG_ERR(("This window[%d] does not support Mosaic Mode!", hWindow->eId));
		return BERR_TRACE(BVDC_ERR_INVALID_MOSAIC_MODE);
	}

	for(i = 0; i < ulMosaicCount; i++)
	{
		hWindow->stNewInfo.astMosaicRect[i].lLeft    = astRect[i].lLeft;
		hWindow->stNewInfo.astMosaicRect[i].lLeft_R  = astRect[i].lLeft;
		hWindow->stNewInfo.astMosaicRect[i].lTop     = astRect[i].lTop;
		hWindow->stNewInfo.astMosaicRect[i].ulWidth  = astRect[i].ulWidth;
		hWindow->stNewInfo.astMosaicRect[i].ulHeight = astRect[i].ulHeight;
	}

	BDBG_LEAVE(BVDC_Window_SetMosaicDstRects);
#else
	BDBG_ERR(("This chipset doesn't support Mosaic Mode!"));
	BSTD_UNUSED(hWindow);
	BSTD_UNUSED(ulMosaicCount);
	BSTD_UNUSED(astRect);
#endif
	return BERR_SUCCESS;
}


/***************************************************************************
 *
 */
BERR_Code BVDC_Window_SetMosaicRectsZOrder
	( BVDC_Window_Handle               hWindow,
	  uint32_t                         ulRectsCount,
	  const uint8_t                    aucZOrder[] )
{
#if BVDC_P_SUPPORT_MOSAIC_MODE
	BDBG_ENTER(BVDC_Window_SetMosaicRectsZOrder);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	/* preliminary error checking */
	if((ulRectsCount > hWindow->stSettings.ulMaxMosaicRect) ||
	   (ulRectsCount && !aucZOrder))
	{
		return BERR_TRACE(BVDC_ERR_INVALID_MOSAIC_MODE);
	}
	if(!hWindow->bClearRectSupport)
	{
		BDBG_ERR(("This window[%d] does not support Mosaic Mode!", hWindow->eId));
		return BERR_TRACE(BVDC_ERR_INVALID_MOSAIC_MODE);
	}
	hWindow->stNewInfo.ulMosaicCount = ulRectsCount;

	if(ulRectsCount)
	{
		BKNI_Memcpy((void*)hWindow->stNewInfo.aucMosaicZOrder, (void*)aucZOrder,
			sizeof(uint8_t) * ulRectsCount);
	}

	BDBG_LEAVE(BVDC_Window_SetMosaicRectsZOrder);

#else
	BDBG_ERR(("This chipset doesn't support Mosaic Mode!"));
	BSTD_UNUSED(hWindow);
	BSTD_UNUSED(ulRectsCount);
	BSTD_UNUSED(aucZOrder);
#endif
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Window_SetMosaicTrack
	( BVDC_Window_Handle               hWindow,
	  uint32_t                         ulChannelId )
{
#if BVDC_P_SUPPORT_MOSAIC_MODE
	BDBG_ENTER(BVDC_Window_SetMosaicTrack);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	/* preliminary error checking */
	if(ulChannelId >= hWindow->stSettings.ulMaxMosaicRect)
	{
		return BERR_TRACE(BVDC_ERR_INVALID_MOSAIC_MODE);
	}
	if(!hWindow->bClearRectSupport)
	{
		BDBG_ERR(("This window[%d] does not support Mosaic Mode!", hWindow->eId));
		return BERR_TRACE(BVDC_ERR_INVALID_MOSAIC_MODE);
	}

	hWindow->stNewInfo.ulMosaicTrackChannelId = ulChannelId;

	BDBG_LEAVE(BVDC_Window_SetMosaicTrack);
#else
	BDBG_ERR(("This chipset doesn't support Mosaic Mode!"));
	BSTD_UNUSED(hWindow);
	BSTD_UNUSED(ulChannelId);
#endif
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Window_GetMosaicTrack
	( BVDC_Window_Handle               hWindow,
	  uint32_t                        *pulChannelId )
{
#if BVDC_P_SUPPORT_MOSAIC_MODE
	BDBG_ENTER(BVDC_Window_GetMosaicTrack);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	if(!hWindow->bClearRectSupport)
	{
		BDBG_ERR(("This window[%d] does not support Mosaic Mode!", hWindow->eId));
		return BERR_TRACE(BVDC_ERR_INVALID_MOSAIC_MODE);
	}

	if(pulChannelId)
	{
		*pulChannelId = hWindow->stCurInfo.ulMosaicTrackChannelId;
	}

	BDBG_LEAVE(BVDC_Window_GetMosaicTrack);
#else
	BDBG_ERR(("This chipset doesn't support Mosaic Mode!"));
	BSTD_UNUSED(hWindow);
	BSTD_UNUSED(pulChannelId);
#endif
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Window_SetCallbackSettings
		( BVDC_Window_Handle                  hWindow,
		  const BVDC_Window_CallbackSettings   *pSettings )
{
	BVDC_Window_CallbackSettings *pDefSettings;

	BDBG_ENTER(BVDC_Window_SetCallbackSettings);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	if(!pSettings)
	{
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	pDefSettings = &hWindow->stCallbackSettings;
	pDefSettings->stMask.bVsyncDelay = pSettings->stMask.bVsyncDelay;
	pDefSettings->stMask.bDriftDelay = pSettings->stMask.bDriftDelay;
	pDefSettings->stMask.bRectAdjust = pSettings->stMask.bRectAdjust;
	pDefSettings->stMask.bSyncLock   = pSettings->stMask.bSyncLock;
	pDefSettings->stMask.bGameModeDelay = pSettings->stMask.bGameModeDelay;
	pDefSettings->ulLipSyncTolerance = pSettings->ulLipSyncTolerance;
	pDefSettings->ulGameModeReadWritePhaseDiff = pSettings->ulGameModeReadWritePhaseDiff;
	pDefSettings->ulGameModeTolerance = pSettings->ulGameModeTolerance;

	BDBG_LEAVE(BVDC_Window_SetCallbackSettings);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Window_GetCallbackSettings
		( BVDC_Window_Handle                  hWindow,
		  BVDC_Window_CallbackSettings       *pSettings )
{
	BDBG_ENTER(BVDC_Window_GetCallbackSettings);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	if(pSettings)
	{
		*pSettings = hWindow->stCallbackSettings;
	}

	BDBG_LEAVE(BVDC_Window_GetCallbackSettings);
	return BERR_SUCCESS;
}


/***************************************************************************
 *
 */
BERR_Code BVDC_Window_InstallCallback
	( BVDC_Window_Handle               hWindow,
	  const BVDC_CallbackFunc_isr      pfCallback,
	  void                            *pvParm1,
	  int                              iParm2 )
{
	BDBG_ENTER(BVDC_Window_InstallCallback);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	/* Store the new infos */
	hWindow->stNewInfo.pfGenCallback      = pfCallback;
	hWindow->stNewInfo.pvGenCallbackParm1 = pvParm1;
	hWindow->stNewInfo.iGenCallbackParm2  = iParm2;

	BDBG_LEAVE(BVDC_Window_InstallCallback);
	return BERR_SUCCESS;
}


/***************************************************************************
 *
 */
BERR_Code BVDC_Window_SetPixelFormat
	( BVDC_Window_Handle        hWindow,
	  BPXL_Format                ePixelFormat)
{
	BDBG_ENTER(BVDC_Window_SetPixelFormat);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	if(!BPXL_IS_YCbCr422_FORMAT(ePixelFormat)
#if (BVDC_P_SUPPORT_BVN_10BIT_444)
	 && !BPXL_IS_YCbCr422_10BIT_FORMAT(ePixelFormat)
	 && !BPXL_IS_YCbCr444_10BIT_FORMAT(ePixelFormat)
#endif
	)
	{
		BDBG_ERR(("Pixel format %s is not supported by Video feeders",
			BPXL_ConvertFmtToStr(ePixelFormat)));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

#if (!BVDC_P_MFD_SUPPORT_BYTE_ORDER)
#if (BSTD_CPU_ENDIAN == BSTD_ENDIAN_LITTLE)
	if ((ePixelFormat == BPXL_eY08_Cr8_Y18_Cb8) ||
		(ePixelFormat == BPXL_eCr8_Y08_Cb8_Y18) ||
		(ePixelFormat == BPXL_eY08_Cb8_Y18_Cr8) ||
		(ePixelFormat == BPXL_eCb8_Y08_Cr8_Y18))
#else
	if ((ePixelFormat == BPXL_eCb8_Y18_Cr8_Y08) ||
		(ePixelFormat == BPXL_eY18_Cb8_Y08_Cr8) ||
		(ePixelFormat == BPXL_eCr8_Y18_Cb8_Y08) ||
		(ePixelFormat == BPXL_eY18_Cr8_Y08_Cb8))
#endif
	{
		BDBG_WRN(("Pixel format %s is not supported by Video feeders",
			BPXL_ConvertFmtToStr(ePixelFormat)));
		BDBG_WRN(("Video display has wrong color, data in capture buffer is correct"));
	}
#endif

	hWindow->stNewInfo.ePixelFormat = ePixelFormat;

	if(ePixelFormat != hWindow->stCurInfo.ePixelFormat)
	{
		hWindow->stNewInfo.stDirty.stBits.bMiscCtrl = BVDC_P_DIRTY;
	}

	BDBG_LEAVE(BVDC_Window_SetPixelFormat);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Window_SetUserCaptureBufferCount
	( BVDC_Window_Handle        hWindow,
	  unsigned int              uiCaptureBufferCount )
{
	BDBG_ENTER(BVDC_Window_SetUserCaptureBufferCount);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	if (uiCaptureBufferCount > BVDC_P_MAX_USER_CAPTURE_BUFFER_COUNT)
	{
		return(BERR_TRACE(BVDC_ERR_CAPTURE_BUFFERS_MORE_THAN_MAX));
	}

	hWindow->stNewInfo.uiCaptureBufCnt = uiCaptureBufferCount;

	/* set dirty bit */
	hWindow->stNewInfo.stDirty.stBits.bUserCaptureBuffer = BVDC_P_DIRTY;

	BDBG_LEAVE(BVDC_Window_SetUserCaptureBufferCount);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Window_GetBuffer
	( BVDC_Window_Handle         hWindow,
	  BVDC_Window_CapturedImage *pCapturedImage )
{
	BERR_Code eRet = BERR_SUCCESS;
	BVDC_P_Window_CapturedPicture stCaptPic;
	BPXL_Format ePxlFmt;
	BSUR_Surface_Settings *pSurSettings = NULL;
#if (BVDC_P_SUPPORT_VIDEO_TESTFEATURE1_CAP)
	BSUR_Surface_Settings stSurSettings;
#endif

	BDBG_ENTER(BVDC_Window_GetBuffer);
	BDBG_ASSERT(pCapturedImage);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	BKNI_EnterCriticalSection();
	eRet = BVDC_P_Window_CapturePicture_isr(hWindow, &stCaptPic);
	BKNI_LeaveCriticalSection();

	ePxlFmt = stCaptPic.ePxlFmt;
	if (eRet == BERR_SUCCESS)
	{
#if (BVDC_P_SUPPORT_VIDEO_TESTFEATURE1_CAP)
		BKNI_Memset(&stSurSettings, 0, sizeof(BSUR_Surface_Settings));
		pSurSettings = &stSurSettings;
		pSurSettings->stTestFeature1Settings.bEnable = stCaptPic.pPicture->stCapTestFeature1.bEnable;
		pSurSettings->stTestFeature1Settings.ulBitsPerPixel = stCaptPic.pPicture->stCapTestFeature1.ulBitsPerPixel;
#endif
		/* Create surface */
		eRet = BSUR_Surface_Create(hWindow->hHeap->hMem,
			stCaptPic.ulWidth, stCaptPic.ulHeight, stCaptPic.ulPitch,
			stCaptPic.pvBufAddr, ePxlFmt, NULL, 0, pSurSettings,
			&pCapturedImage->hCaptureBuffer);
		if (eRet == BERR_SUCCESS)
		{
			pCapturedImage->eCapturePolarity = stCaptPic.ePolarity;
			pCapturedImage->ulCaptureNumVbiLines = stCaptPic.ulNumVbiLines;
			/* Store surface in picture node */
			stCaptPic.pPicture->hSurface = pCapturedImage->hCaptureBuffer;
#if (BVDC_P_SUPPORT_3D_VIDEO)
			if(stCaptPic.pvBufAddr_R != NULL)
			{
				eRet = BSUR_Surface_Create(hWindow->hHeap->hMem,
					stCaptPic.ulWidth, stCaptPic.ulHeight, stCaptPic.ulPitch,
					stCaptPic.pvBufAddr_R, ePxlFmt, NULL, 0, pSurSettings,
					&pCapturedImage->hRCaptureBuffer);
				if (eRet == BERR_SUCCESS)
				{
					/* Store right surface in picture node */
					stCaptPic.pPicture->hSurface_R = pCapturedImage->hRCaptureBuffer;
					pCapturedImage->eOutOrientation = stCaptPic.eDispOrientation;
				}
			}
			else
#endif
			{
				pCapturedImage->hRCaptureBuffer = NULL;
			}
		}
		else
		{
			pCapturedImage->hCaptureBuffer = NULL;
			pCapturedImage->hRCaptureBuffer = NULL;
			pCapturedImage->hCaptureBuffer = NULL;
			pCapturedImage->eCapturePolarity = BAVC_Polarity_eTopField;
			pCapturedImage->ulCaptureNumVbiLines = 0;
			BDBG_ERR(("Failed to create surface from captured picture"));
			return BERR_TRACE(eRet);
		}
	}
	else
	{
		pCapturedImage->hCaptureBuffer = NULL;
		pCapturedImage->hRCaptureBuffer = NULL;
	}

	BDBG_LEAVE(BVDC_Window_GetBuffer);
	return BERR_TRACE(eRet);
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Window_ReturnBuffer
	( BVDC_Window_Handle               hWindow,
	  BVDC_Window_CapturedImage       *pCapturedImage )
{
	BERR_Code eRet = BERR_SUCCESS;

	BDBG_ENTER(BVDC_Window_ReturnBuffer);
	BDBG_ASSERT(pCapturedImage);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	BKNI_EnterCriticalSection();
	eRet = BVDC_P_Window_ReleasePicture_isr(hWindow, pCapturedImage->hCaptureBuffer);
	BKNI_LeaveCriticalSection();

	if (eRet != BERR_SUCCESS)
		return BERR_TRACE(eRet);
	else
	{
		BDBG_MSG(("destroying surface"));
		/* Destroy surface */
		BSUR_Surface_Destroy(pCapturedImage->hCaptureBuffer);
	}

#if (BVDC_P_SUPPORT_3D_VIDEO)
	if(pCapturedImage->hRCaptureBuffer)
	{
		BKNI_EnterCriticalSection();
		eRet = BVDC_P_Window_ReleasePicture_isr(hWindow, pCapturedImage->hRCaptureBuffer);
		BKNI_LeaveCriticalSection();

		if (eRet != BERR_SUCCESS)
			return BERR_TRACE(eRet);
		else
		{
			BDBG_MSG(("destroying right surface"));
			/* Destroy surface */
			BSUR_Surface_Destroy(pCapturedImage->hRCaptureBuffer);
		}
	}
#endif

	BDBG_LEAVE(BVDC_Window_ReturnBuffer);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Window_SetLumaKey
	( BVDC_Window_Handle               hWindow,
	  bool                             bLumaKey,
	  uint8_t                          ucLumaKeyMask,
	  uint8_t                          ucLumaKeyHigh,
	  uint8_t                          ucLumaKeyLow )
{
	BERR_Code             err = BERR_SUCCESS;

	BDBG_ENTER(BVDC_Window_SetLumaKey);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	if( (hWindow->eId == BVDC_P_WindowId_eComp0_V0) ||
	    (hWindow->eId == BVDC_P_WindowId_eComp0_V1) ||
	    (hWindow->eId == BVDC_P_WindowId_eComp1_V0) ||
	    (hWindow->eId == BVDC_P_WindowId_eComp1_V1) )
	{
		hWindow->stNewInfo.bLumaKey      = bLumaKey;
		hWindow->stNewInfo.ucLumaKeyMask = ucLumaKeyMask;
		hWindow->stNewInfo.ucLumaKeyHigh = ucLumaKeyHigh;
		hWindow->stNewInfo.ucLumaKeyLow  = ucLumaKeyLow;
	}
	else
	{
		err = BERR_TRACE(BERR_NOT_SUPPORTED);
		BDBG_ERR(("Luma key not supported for window[%d]", hWindow->eId));
	}

	BDBG_LEAVE(BVDC_Window_SetLumaKey);
	return err;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Window_GetLumaKey
	( BVDC_Window_Handle               hWindow,
	  bool                            *pbLumaKey,
	  uint8_t                         *pucLumaKeyMask,
	  uint8_t                         *pucLumaKeyHigh,
	  uint8_t                         *pucLumaKeyLow )
{
	BDBG_ENTER(BVDC_Window_GetLumaKey);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	if(pbLumaKey)
	{
		*pbLumaKey = hWindow->stCurInfo.bLumaKey;
	}

	if(pucLumaKeyMask)
	{
		*pucLumaKeyMask = hWindow->stCurInfo.ucLumaKeyMask;
	}

	if(pucLumaKeyHigh)
	{
		*pucLumaKeyHigh = hWindow->stCurInfo.ucLumaKeyHigh;
	}

	if(pucLumaKeyLow)
	{
		*pucLumaKeyLow = hWindow->stCurInfo.ucLumaKeyLow;
	}


	BDBG_LEAVE(BVDC_Window_GetLumaKey);
	return BERR_SUCCESS;

}

/***************************************************************************
 *
 */
BERR_Code BVDC_Window_SetColorMatrix
	( BVDC_Window_Handle               hWindow,
	  bool                             bOverride,
	  const int32_t                    pl32_Matrix[BVDC_CSC_COEFF_COUNT],
	  uint32_t                         ulShift )
{
	BDBG_ENTER(BVDC_Window_SetColorMatrix);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	if (BVDC_P_WIN_IS_NONLINEAR_CSC_WIN(hWindow->eId))
	{
		return BERR_TRACE(BERR_NOT_SUPPORTED);
	}

	if (bOverride)
	{
		BVDC_P_Window_SetColorMatrix(hWindow, pl32_Matrix, hWindow->stNewInfo.pl32_Matrix);
	}

	hWindow->stNewInfo.bUserCsc = bOverride;
	hWindow->stNewInfo.ulUserShift = ulShift;

	/* set dirty bit */
	hWindow->stNewInfo.stDirty.stBits.bCscAdjust = BVDC_P_DIRTY;

	BDBG_LEAVE(BVDC_Window_SetColorMatrix);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Window_SetColorMatrixNonLinearA
	( BVDC_Window_Handle               hWindow,
	  bool                             bOverride,
	  const int32_t                    pl32_Matrix[BVDC_CSC_COEFF_COUNT],
	  uint32_t                         ulShift )
{
	BDBG_ENTER(BVDC_Window_SetColorMatrix);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	if (!BVDC_P_WIN_IS_NONLINEAR_CSC_WIN(hWindow->eId))
	{
		return BERR_TRACE(BERR_NOT_SUPPORTED);
	}

	BVDC_P_Window_SetColorMatrix(hWindow, pl32_Matrix, hWindow->stNewInfo.pl32_MatrixA);

	hWindow->stNewInfo.bUserCscA = bOverride;
	hWindow->stNewInfo.ulUserShiftA = ulShift;

	/* set dirty bit */
	hWindow->stNewInfo.stDirty.stBits.bCscAdjust = BVDC_P_DIRTY;

	BDBG_LEAVE(BVDC_Window_SetColorMatrix);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Window_SetColorMatrixNonLinearB
	( BVDC_Window_Handle               hWindow,
	  bool                             bOverride,
	  const int32_t                    pl32_Matrix[BVDC_CSC_COEFF_COUNT],
	  uint32_t                         ulShift )
{
	BDBG_ENTER(BVDC_Window_SetColorMatrix);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	if (!BVDC_P_WIN_IS_NONLINEAR_CSC_WIN(hWindow->eId))
	{
		return BERR_TRACE(BERR_NOT_SUPPORTED);
	}

	BVDC_P_Window_SetColorMatrix(hWindow, pl32_Matrix, hWindow->stNewInfo.pl32_MatrixB);

	hWindow->stNewInfo.bUserCscB = bOverride;
	hWindow->stNewInfo.ulUserShiftB = ulShift;

	/* set dirty bit */
	hWindow->stNewInfo.stDirty.stBits.bCscAdjust = BVDC_P_DIRTY;

	BDBG_LEAVE(BVDC_Window_SetColorMatrix);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Window_SetColorMatrixNonLinearC
	( BVDC_Window_Handle               hWindow,
	  bool                             bOverride,
	  const int32_t                    pl32_Matrix[BVDC_CSC_COEFF_COUNT],
	  uint32_t                         ulShift )
{
	BDBG_ENTER(BVDC_Window_SetColorMatrix);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	BVDC_P_Window_SetColorMatrix(hWindow, pl32_Matrix, hWindow->stNewInfo.pl32_MatrixC);

	if (bOverride)
	{
		/* calculate Inverse Matrix C here for use later in RGB attenuation calculations */
		if(BVDC_P_WIN_IS_GFX_WINDOW(hWindow->eId))
		{
			BVDC_P_CscCoeffs stCscCoeffs = BVDC_P_MAKE_GFD_CSC
				( 1.00000, 0.00000, 0.00000, 0.00000, 0.00000,
				  0.00000, 1.00000, 0.00000, 0.00000, 0.00000,
				  0.00000, 0.00000, 1.00000, 0.00000, 0.00000 );
			BVDC_P_Csc_FromMatrix_isr(&stCscCoeffs, pl32_Matrix, ulShift);
			BVDC_P_Csc_MatrixInverse(&stCscCoeffs, &hWindow->stInvUserCscC);
		}
		else
		{
			BVDC_P_CscCoeffs stCscCoeffs = BVDC_P_MAKE_CMP_CSC
				( 1.00000, 0.00000, 0.00000, 0.00000,
				  0.00000, 1.00000, 0.00000, 0.00000,
				  0.00000, 0.00000, 1.00000, 0.00000 );
			BVDC_P_Csc_FromMatrix_isr(&stCscCoeffs, pl32_Matrix, ulShift);
			BVDC_P_Csc_MatrixInverse(&stCscCoeffs, &hWindow->stInvUserCscC);
		}
	}
	hWindow->stNewInfo.bUserCscC = bOverride;
	hWindow->stNewInfo.ulUserShiftC = ulShift;

	/* set dirty bit */
	hWindow->stNewInfo.stDirty.stBits.bCscAdjust = BVDC_P_DIRTY;

	BDBG_LEAVE(BVDC_Window_SetColorMatrix);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Window_GetColorMatrix
	( BVDC_Window_Handle               hWindow,
	  bool                            *pbOverride,
	  int32_t                          pl32_Matrix[BVDC_CSC_COEFF_COUNT],
	  uint32_t                        *pulShift )
{
	BDBG_ENTER(BVDC_Window_GetColorMatrix);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	if (BVDC_P_WIN_IS_NONLINEAR_CSC_WIN(hWindow->eId))
	{
		return BERR_TRACE(BERR_NOT_SUPPORTED);
	}

	BVDC_P_Window_GetColorMatrix(hWindow, hWindow->stNewInfo.bUserCsc, &hWindow->stPriCsc,
		                         hWindow->stCurInfo.pl32_Matrix, pl32_Matrix);

	*pbOverride = hWindow->stNewInfo.bUserCsc;
	*pulShift =  (*pbOverride)? hWindow->stNewInfo.ulUserShift : BVDC_P_FIX_POINT_SHIFT;

	BDBG_LEAVE(BVDC_Window_GetColorMatrix);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Window_GetColorMatrixNonLinearA
	( BVDC_Window_Handle               hWindow,
	  bool                            *pbOverride,
	  int32_t                          pl32_Matrix[BVDC_CSC_COEFF_COUNT],
	  uint32_t                        *pulShift )
{
	BDBG_ENTER(BVDC_Window_GetColorMatrix);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	if (!BVDC_P_WIN_IS_NONLINEAR_CSC_WIN(hWindow->eId))
	{
		return BERR_TRACE(BERR_NOT_SUPPORTED);
	}

	BVDC_P_Window_GetColorMatrix(hWindow, hWindow->stNewInfo.bUserCsc, &hWindow->stPriCscA,
		                         hWindow->stCurInfo.pl32_MatrixA, pl32_Matrix);

	*pbOverride = hWindow->stNewInfo.bUserCscA;
	*pulShift =  (*pbOverride)? hWindow->stNewInfo.ulUserShiftA : BVDC_P_FIX_POINT_SHIFT;

	BDBG_LEAVE(BVDC_Window_GetColorMatrix);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Window_GetColorMatrixNonLinearB
	( BVDC_Window_Handle               hWindow,
	  bool                            *pbOverride,
	  int32_t                          pl32_Matrix[BVDC_CSC_COEFF_COUNT],
	  uint32_t                        *pulShift )
{
	BDBG_ENTER(BVDC_Window_GetColorMatrix);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	if (!BVDC_P_WIN_IS_NONLINEAR_CSC_WIN(hWindow->eId))
	{
		return BERR_TRACE(BERR_NOT_SUPPORTED);
	}

	BVDC_P_Window_GetColorMatrix(hWindow, hWindow->stNewInfo.bUserCscB, &hWindow->stPriCscB,
		                         hWindow->stCurInfo.pl32_MatrixB, pl32_Matrix);

	*pbOverride = hWindow->stNewInfo.bUserCscB;
	*pulShift =  (*pbOverride)? hWindow->stNewInfo.ulUserShiftB : BVDC_P_FIX_POINT_SHIFT;

	BDBG_LEAVE(BVDC_Window_GetColorMatrix);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Window_GetColorMatrixNonLinearC
	( BVDC_Window_Handle               hWindow,
	  bool                            *pbOverride,
	  int32_t                          pl32_Matrix[BVDC_CSC_COEFF_COUNT],
	  uint32_t                        *pulShift )
{
	BDBG_ENTER(BVDC_Window_GetColorMatrix);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	if (!BVDC_P_WIN_IS_NONLINEAR_CSC_WIN(hWindow->eId))
	{
		return BERR_TRACE(BERR_NOT_SUPPORTED);
	}

	BVDC_P_Window_GetColorMatrix(hWindow, hWindow->stNewInfo.bUserCscC, &hWindow->stPriCscC,
		                         hWindow->stCurInfo.pl32_MatrixC, pl32_Matrix);

	*pbOverride = hWindow->stNewInfo.bUserCscC;
	*pulShift =  (*pbOverride)? hWindow->stNewInfo.ulUserShiftC : BVDC_P_FIX_POINT_SHIFT;

	BDBG_LEAVE(BVDC_Window_GetColorMatrix);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Window_LoadCabTable
	( BVDC_Window_Handle               hWindow,
	  const uint32_t                  *pulCabTable,
	  uint32_t                         ulOffset,
	  uint32_t                         ulSize )
{
	BDBG_ENTER(BVDC_Window_LoadCabTable);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	/* only support main display's main window */
	if(hWindow->eId != BVDC_P_WindowId_eComp0_V0)
	{
		return BERR_TRACE(BVDC_ERR_PEP_WINDOW_NOT_SUPPORT);
	}

	/* Check for valid offset and size */
	if((pulCabTable != NULL) && ((ulOffset + ulSize) > BVDC_P_CAB_TABLE_SIZE))
	{
		return BERR_TRACE(BVDC_ERR_PEP_INVALID_PARAMETER);
	}

	if(pulCabTable != NULL)
	{
		uint32_t id;

		/* set new value */
		hWindow->stNewInfo.bUserCabEnable = true;

		/* copy pulCabTable to internal table */
		for(id = 0; id < ulSize; id++)
		{
			*(hWindow->stNewInfo.aulCabTable + (id + ulOffset)) =
				*(pulCabTable + id);
			BDBG_MSG(("User CabTable[%d]= 0x%x", id + ulOffset, hWindow->stNewInfo.aulCabTable[id+ulOffset]));
		}
	}
	else
	{
		hWindow->stNewInfo.bUserCabEnable = false;
	}

	BDBG_LEAVE(BVDC_Window_LoadCabTable);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Window_LoadLabTableCustomized
	( BVDC_Window_Handle               hWindow,
	  const uint32_t                  *pulLumaTable,
	  const uint32_t                  *pulCbTable,
	  const uint32_t                  *pulCrTable,
	  uint32_t                         ulOffset,
	  uint32_t                         ulSize )
{
	uint32_t id;

	BDBG_ENTER(BVDC_Window_LoadLabTableCustomized);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	/* only support main display's main window */
	if(hWindow->eId != BVDC_P_WindowId_eComp0_V0)
	{
		return BERR_TRACE(BVDC_ERR_PEP_WINDOW_NOT_SUPPORT);
	}

	/* Check for valid offset and size */
	if((pulLumaTable != NULL || pulCbTable != NULL || pulCrTable != NULL) &&
	   ((ulOffset + ulSize) > BVDC_P_LAB_TABLE_SIZE))
	{
		return BERR_TRACE(BVDC_ERR_PEP_INVALID_PARAMETER);
	}

	/* Check if user provides both Cb and Cr tables or not */
	if((pulCbTable != NULL && pulCrTable == NULL) ||
	   (pulCbTable == NULL && pulCrTable != NULL))
	{
		BDBG_ERR(("Need to provide both Cb and Cr tables"));
		return BERR_TRACE(BVDC_ERR_PEP_INVALID_PARAMETER);
	}

	hWindow->stNewInfo.bUserLabLuma = (pulLumaTable != NULL) ? true : false;
	hWindow->stNewInfo.bUserLabCbCr = (pulCbTable != NULL) ? true : false;

	/* copy to internal tables */
	for(id = 0; id < ulSize; id++)
	{
		if(pulLumaTable != NULL)
		{
			*(hWindow->stNewInfo.aulLabLumaTbl + (id + ulOffset)) =
				*(pulLumaTable + id);
			BDBG_MSG(("User LumTable[%d]= 0x%x", id + ulOffset, hWindow->stNewInfo.aulLabLumaTbl[id+ulOffset]));
		}
		if(pulCbTable != NULL && pulCrTable != NULL)
		{
			*(hWindow->stNewInfo.aulLabCbTbl + (id + ulOffset)) =
				*(pulCbTable + id);
			*(hWindow->stNewInfo.aulLabCrTbl + (id + ulOffset)) =
				*(pulCrTable + id);
			BDBG_MSG(("User CbTable[%d]= 0x%x, CrTable[%d] = 0x%x",
				id + ulOffset, hWindow->stNewInfo.aulLabCbTbl[id+ulOffset],
				id + ulOffset, hWindow->stNewInfo.aulLabCrTbl[id+ulOffset]));
		}
	}

	BDBG_LEAVE(BVDC_Window_LoadLabTableCustomized);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Window_LoadLabTable
	( BVDC_Window_Handle               hWindow,
	  const uint32_t                  *pulLabTable,
	  uint32_t                         ulOffset,
	  uint32_t                         ulSize )
{
	BDBG_ENTER(BVDC_Window_LoadLabTable);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

#if (BVDC_P_SUPPORT_PEP && BVDC_P_SUPPORT_PEP_VER <= BVDC_P_SUPPORT_PEP_VER_3)
	/* only support main display's main window */
	if(hWindow->eId != BVDC_P_WindowId_eComp0_V0)
	{
		return BERR_TRACE(BVDC_ERR_PEP_WINDOW_NOT_SUPPORT);
	}

	/* Check for valid offset and size */
	if((pulLabTable != NULL) && ((ulOffset + ulSize) > BVDC_P_LAB_TABLE_SIZE))
	{
		return BERR_TRACE(BVDC_ERR_PEP_INVALID_PARAMETER);
	}

	if(pulLabTable != NULL)
	{
		uint32_t id;
		uint32_t *pulLumaTable;
		uint32_t *pulCbTable;
		uint32_t *pulCrTable;

		pulLumaTable = (uint32_t *)(BKNI_Malloc(BVDC_P_LAB_TABLE_SIZE * sizeof(uint32_t)));
		pulCbTable = (uint32_t *)(BKNI_Malloc(BVDC_P_LAB_TABLE_SIZE * sizeof(uint32_t)));
		pulCrTable = (uint32_t *)(BKNI_Malloc(BVDC_P_LAB_TABLE_SIZE * sizeof(uint32_t)));

		/* break out the data into luma, Cb and Cr tables */
		for(id = 0; id < ulSize; id++)
		{
			*(pulLumaTable + (id + ulOffset)) =
				BCHP_GET_FIELD_DATA(*(pulLabTable + id), PEP_CMP_0_V0_LAB_LUT_DATA_i, LUMA_DATA);
			*(pulCbTable + (id + ulOffset)) =
				BCHP_GET_FIELD_DATA(*(pulLabTable + id), PEP_CMP_0_V0_LAB_LUT_DATA_i, CB_OFFSET);
			*(pulCrTable + (id + ulOffset)) =
				BCHP_GET_FIELD_DATA(*(pulLabTable + id), PEP_CMP_0_V0_LAB_LUT_DATA_i, CR_OFFSET);
			BDBG_MSG(("User Luma[%d]= 0x%x, Cb = 0x%x, Cr = 0x%x",
				id + ulOffset, pulLumaTable[id+ulOffset],
				pulCbTable[id+ulOffset], pulCrTable[id+ulOffset]));
		}
		/* Calling the new function to load Lab table */
		BVDC_Window_LoadLabTableCustomized(hWindow, pulLumaTable, pulCbTable, pulCrTable, ulOffset, ulSize);

		BKNI_Free((uint32_t *)pulLumaTable);
		BKNI_Free((uint32_t *)pulCbTable);
		BKNI_Free((uint32_t *)pulCrTable);
	}
	else
	{
		BVDC_Window_LoadLabTableCustomized(hWindow, NULL, NULL, NULL, ulOffset, ulSize);
	}
#else
	BSTD_UNUSED(pulLabTable);
	BSTD_UNUSED(ulOffset);
	BSTD_UNUSED(ulSize);
#endif

	BDBG_LEAVE(BVDC_Window_LoadLabTable);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Window_SetLumaStatsConfiguration
	( BVDC_Window_Handle               hWindow,
	  const BVDC_LumaSettings         *pLumaSettings )
{
	BERR_Code             err = BERR_SUCCESS;

	BDBG_ENTER(BVDC_Window_SetLumaStatsConfiguration);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	if((hWindow->eId != BVDC_P_WindowId_eComp0_V0) && (!hWindow->bHistAvail))
	{
		return BERR_TRACE(BERR_NOT_SUPPORTED);
	}

	if(pLumaSettings && pLumaSettings->eNumBins >= BVDC_HistBinSelect_eInvalid)
	{
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	if(pLumaSettings &&
	   (pLumaSettings->stRegion.ulLeft   > BVDC_P_CLIPRECT_PERCENT ||
	    pLumaSettings->stRegion.ulRight  > BVDC_P_CLIPRECT_PERCENT ||
	    pLumaSettings->stRegion.ulTop    > BVDC_P_CLIPRECT_PERCENT ||
	    pLumaSettings->stRegion.ulBottom > BVDC_P_CLIPRECT_PERCENT ||
	    pLumaSettings->stRegion.ulLeft + pLumaSettings->stRegion.ulRight  > BVDC_P_CLIPRECT_PERCENT ||
	    pLumaSettings->stRegion.ulTop  + pLumaSettings->stRegion.ulBottom > BVDC_P_CLIPRECT_PERCENT))
	{
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	if(pLumaSettings)
	{
		hWindow->stNewInfo.stLumaRect = *pLumaSettings;
		hWindow->stNewInfo.bLumaRectUserSet = true;
	}
	else
	{
		hWindow->stNewInfo.bLumaRectUserSet = false;
	}

	BDBG_LEAVE(BVDC_Window_SetLumaStatsConfiguration);
	return err;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Window_GetLumaStatsConfiguration
	( BVDC_Window_Handle               hWindow,
	  BVDC_LumaSettings               *pLumaSettings )
{
	BDBG_ENTER(BVDC_Window_GetLumaStatsConfiguration);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	if(pLumaSettings)
	{
		*pLumaSettings = hWindow->stCurInfo.stLumaRect;
	}

	BDBG_LEAVE(BVDC_Window_GetLumaStatsConfiguration);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Window_GetLumaStatus
	( const BVDC_Window_Handle         hWindow,
	  BVDC_LumaStatus                 *pLumaStatus )
{
	BDBG_ENTER(BVDC_Window_GetLumaStatus);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	if((hWindow->eId != BVDC_P_WindowId_eComp0_V0) && (!hWindow->bHistAvail))
	{
		return BERR_TRACE(BERR_NOT_SUPPORTED);
	}

	/* set return value */
	if(pLumaStatus)
	{
#if (BVDC_P_SUPPORT_HIST)
		if(hWindow->bHistAvail)
		{
			/* For chipsets that have new hist hardware, only return */
			/* histogram data if hist is enabled by users or dynamic contrast */
			if(hWindow->stCurInfo.bHistEnable)
			{
				BVDC_P_Hist_GetHistogramData(hWindow, pLumaStatus);
			}
			else
			{
				BDBG_ERR(("HIST is not enable, cannot retrieve histogram data"));
				return BERR_TRACE(BERR_NOT_SUPPORTED);
			}
		}
		else
		{
			/* Get Min/Max/Histogram from PEP */
			BVDC_P_Pep_GetLumaStatus(hWindow, pLumaStatus);
		}
#endif
	}

	BDBG_LEAVE(BVDC_Window_GetLumaStatus);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Window_SetCoefficientIndex
	( BVDC_Window_Handle               hWindow,
	  const BVDC_CoefficientIndex     *pCtIndex )
{
	BDBG_ENTER(BVDC_Window_SetCoefficientIndex);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	if(pCtIndex)
	{
		hWindow->stNewInfo.stCtIndex = *pCtIndex;

		/* Flag change */
		if((pCtIndex->ulSclHorzLuma    != hWindow->stCurInfo.stCtIndex.ulSclHorzLuma   ) ||
		   (pCtIndex->ulSclVertLuma    != hWindow->stCurInfo.stCtIndex.ulSclVertLuma   ) ||
		   (!BVDC_P_WIN_IS_GFX_WINDOW(hWindow->eId) &&
			((pCtIndex->ulSclHorzChroma  != hWindow->stCurInfo.stCtIndex.ulSclHorzChroma ) ||
			 (pCtIndex->ulSclVertChroma  != hWindow->stCurInfo.stCtIndex.ulSclVertChroma ) ||
			 (pCtIndex->ulMadHorzLuma    != hWindow->stCurInfo.stCtIndex.ulMadHorzLuma   ) ||
			 (pCtIndex->ulMadHorzChroma  != hWindow->stCurInfo.stCtIndex.ulMadHorzChroma ) ||
			 (pCtIndex->ulHsclHorzLuma   != hWindow->stCurInfo.stCtIndex.ulHsclHorzLuma  ) ||
			 (pCtIndex->ulHsclHorzChroma != hWindow->stCurInfo.stCtIndex.ulHsclHorzChroma))))
		{
			hWindow->stNewInfo.stDirty.stBits.bCtIndex = BVDC_P_DIRTY;
		}
	}

	BDBG_LEAVE(BVDC_Window_SetCoefficientIndex);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Window_SetGameModeDelay
	( BVDC_Window_Handle               hWindow,
	  const BVDC_Window_GameModeSettings    *pstGameModeDelay )
{
	#define BVDC_P_GAME_MODE_MIN_BUF_DELAY     (1000) /* in usecs */

	BDBG_ENTER(BVDC_Window_SetGameModeDelay);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	if(!pstGameModeDelay)
	{
		hWindow->stNewInfo.stGameDelaySetting.bEnable = false;
	}
	else
	{
		if(pstGameModeDelay->bEnable)
		{
			if(pstGameModeDelay->ulBufferDelayTarget < pstGameModeDelay->ulBufferDelayTolerance)
			{
				BDBG_ERR(("Game mode delay target[%d] < tolerance[%d]!",
					pstGameModeDelay->ulBufferDelayTarget, pstGameModeDelay->ulBufferDelayTolerance));
				return BERR_TRACE(BERR_INVALID_PARAMETER);
			}
			if(pstGameModeDelay->ulBufferDelayTarget < BVDC_P_GAME_MODE_MIN_BUF_DELAY)
			{
				BDBG_ERR(("Game mode delay target[%d] < %d usecs, too small!",
					pstGameModeDelay->ulBufferDelayTarget, BVDC_P_GAME_MODE_MIN_BUF_DELAY));
				return BERR_TRACE(BERR_INVALID_PARAMETER);
			}
			if(pstGameModeDelay->ulBufferDelayTarget >=
				(hWindow->hBuffer->ulActiveBufCnt) * 1000000 * BFMT_FREQ_FACTOR / hWindow->stCurInfo.hSource->ulVertFreq)
			{
				BDBG_ERR(("Game mode delay target[%d usec] > %d fields, too large!",
					pstGameModeDelay->ulBufferDelayTarget,
					hWindow->hBuffer->ulActiveBufCnt-1));
				return BERR_TRACE(BERR_INVALID_PARAMETER);
			}
		}
		hWindow->stNewInfo.stGameDelaySetting = *pstGameModeDelay;
	}

	BDBG_LEAVE(BVDC_Window_SetGameModeDelay);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Window_GetGameModeDelay
	( BVDC_Window_Handle               hWindow,
	  BVDC_Window_GameModeSettings    *pstGameModeDelay )
{
	BDBG_ENTER(BVDC_Window_GetGameModeDelay);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	if(!pstGameModeDelay)
	{
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}
	*pstGameModeDelay = hWindow->stCurInfo.stGameDelaySetting;

	BDBG_LEAVE(BVDC_Window_GetGameModeDelay);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Window_SetDitherConfiguration
	( BVDC_Window_Handle               hWindow,
	  const BVDC_DitherSettings       *pDither )
{
	BDBG_ENTER(BVDC_Window_SetDitherConfiguration);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	/* only support main display's main window */
	if(hWindow->eId != BVDC_P_WindowId_eComp0_V0)
	{
		return BERR_TRACE(BVDC_ERR_PEP_WINDOW_NOT_SUPPORT);
	}

	/* set new values */
	hWindow->stNewInfo.stDither = *pDither;

	/* set dirty bit */
	if((pDither->bReduceSmooth != hWindow->stCurInfo.stDither.bReduceSmooth) ||
	   (pDither->bSmoothEnable != hWindow->stCurInfo.stDither.bSmoothEnable) ||
	   (pDither->ulSmoothLimit != hWindow->stCurInfo.stDither.ulSmoothLimit))
	{
		hWindow->stNewInfo.stDirty.stBits.bDitAdjust = BVDC_P_DIRTY;
	}

	BDBG_LEAVE(BVDC_Window_SetDitherConfiguration);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Window_GetDitherConfiguration
	( const BVDC_Window_Handle         hWindow,
	  BVDC_DitherSettings             *pDither )
{
	BDBG_ENTER(BVDC_Window_GetDitherConfiguration);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	/* only support main display's main window */
	if(hWindow->eId != BVDC_P_WindowId_eComp0_V0)
	{
		return BERR_TRACE(BVDC_ERR_PEP_WINDOW_NOT_SUPPORT);
	}

	/* set new values */
	if(pDither)
	{
		*pDither = hWindow->stNewInfo.stDither;
	}

	BDBG_LEAVE(BVDC_Window_GetDitherConfiguration);
	return BERR_SUCCESS;

}

/***************************************************************************
 *
 */
BERR_Code BVDC_Window_SetFgtConfiguration
	( BVDC_Window_Handle               hWindow,
	  const BVDC_Fgt_Settings         *pFgtSettings )
{
	BERR_Code  eStatus = BERR_SUCCESS;
	BVDC_P_Window_Info   *pNewInfo, *pCurInfo;

	BDBG_ENTER(BVDC_Window_SetFgtConfiguration);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	BDBG_OBJECT_ASSERT(hWindow->hCompositor->hVdc, BVDC_VDC);
	BDBG_ASSERT(pFgtSettings);

	pNewInfo = &hWindow->stNewInfo;
	pCurInfo = &hWindow->stCurInfo;

#if (BVDC_P_SUPPORT_FGT)
	if(pFgtSettings->bEnable)
	{
		if(!hWindow->hCompositor->hVdc->stSettings.bEnableFgt)
		{
			BDBG_ERR(("FGT is disabled in BVDC_Settings!"));
			eStatus = BERR_TRACE(BVDC_ERR_FGT_NOT_SUPPORTED);
		}

		if(!BVDC_P_SRC_IS_MPEG(hWindow->stNewInfo.hSource->eId))
		{
			BDBG_ERR(("Video must be MPEG window!"));
			eStatus = BERR_TRACE(BVDC_ERR_FGT_NOT_SUPPORTED);
		}
	}

	/* configuration */
	pNewInfo->stFgtSettings = *pFgtSettings;
	pNewInfo->bFgt = pFgtSettings->bEnable;

	if((pCurInfo->bFgt != pNewInfo->bFgt) ||
	   (pCurInfo->stFgtSettings.bEnable != pNewInfo->stFgtSettings.bEnable))
	{
		pNewInfo->stDirty.stBits.bFgt = BVDC_P_DIRTY;
	}

#else
	BSTD_UNUSED(pFgtSettings);
	BDBG_ERR(("This chipset doesn't support FGT!"));
	eStatus = BERR_TRACE(BVDC_ERR_FGT_NOT_SUPPORTED);
#endif

	BDBG_LEAVE(BVDC_Window_SetFgtConfiguration);
	return eStatus;
}


/***************************************************************************
 *
 */
BERR_Code BVDC_Window_GetFgtConfiguration
	( const BVDC_Window_Handle         hWindow,
	  BVDC_Fgt_Settings               *pFgtSettings )
{
	BERR_Code  eStatus = BERR_SUCCESS;

	BDBG_ENTER(BVDC_Window_GetFgtConfiguration);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

#if (BVDC_P_SUPPORT_FGT)
	if(pFgtSettings)
	{
		*pFgtSettings = hWindow->stCurInfo.stFgtSettings;
	}
#else
	BSTD_UNUSED(pFgtSettings);
	BDBG_ERR(("This chipset doesn't support FGT!"));
	eStatus = BERR_TRACE(BVDC_ERR_FGT_NOT_SUPPORTED);
#endif

	BDBG_LEAVE(BVDC_Window_GetFgtConfiguration);
	return eStatus;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Window_SetAnrConfiguration
	( BVDC_Window_Handle               hWindow,
	  const BVDC_Anr_Settings         *pAnrSettings )
{
	BERR_Code  eStatus = BERR_SUCCESS;

	BDBG_ENTER(BVDC_Window_SetAnrConfiguration);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

#if (BVDC_P_SUPPORT_ANR) || (BVDC_P_SUPPORT_MANR)
	/* only support main display's main window */
	if(!pAnrSettings )
	{
		eStatus = BERR_TRACE(BERR_INVALID_PARAMETER);
	}
	else
	{
		BVDC_P_Window_Info *pNewInfo = &hWindow->stNewInfo;
		const BVDC_P_Window_Info *pCurInfo = &hWindow->stCurInfo;
		BVDC_Anr_Settings  *pNewAnr;
		const BVDC_Anr_Settings  *pCurAnr;

		pNewAnr = &pNewInfo->stAnrSettings;
		pCurAnr = &pCurInfo->stAnrSettings;

		if(pAnrSettings->eMode)
		{
#if(BVDC_P_SUPPORT_ANR_VER >= 2 || BVDC_P_SUPPORT_MANR)
			if(!BPXL_IS_YCbCr422_FORMAT(pAnrSettings->ePxlFormat) &&
			   !BPXL_IS_YCbCr422_10BIT_FORMAT(pAnrSettings->ePxlFormat) &&
			   !BPXL_IS_YCbCr422_10BIT_PACKED_FORMAT(pAnrSettings->ePxlFormat))
			{
				BDBG_ERR(("This chip only supports 8-bit or 10-bit 4:2:2 ANR pixel format!"));
				return BERR_TRACE(BVDC_ERR_MAD_NOT_SUPPORTED);
			}
#elif(BVDC_P_SUPPORT_ANR_VER < 2)
			if(!BPXL_IS_YCbCr422_FORMAT(pAnrSettings->ePxlFormat))
			{
				BDBG_WRN(("This chip only supports 8-bit 4:2:2 ANR pixel format!"));
				return BERR_TRACE(BERR_INVALID_PARAMETER);
			}
#endif /* BVDC_P_SUPPORT_ANR_VER < 2 */
		}

		/* configuration */
		pNewInfo->bAnr = (BVDC_FilterMode_eEnable == pAnrSettings->eMode);
		pNewInfo->stAnrSettings = *pAnrSettings;

#if (BVDC_P_SUPPORT_VIDEO_TESTFEATURE1_MAD_ANR)
		/* TestFeature1 */
		pNewAnr->stVideoTestFeature1.bEnable = pAnrSettings->stVideoTestFeature1.bEnable;
		pNewAnr->stVideoTestFeature1.ulBitsPerPixel = pAnrSettings->stVideoTestFeature1.ulBitsPerPixel;
#else
		BDBG_MSG(("This chip will ignore ANR TestFeature1 user setting!"));
#endif

		/* Dirty bit set */
		if((pAnrSettings->eMode       != pCurInfo->stAnrSettings.eMode) ||
		   (pAnrSettings->iSnDbAdjust != pCurInfo->stAnrSettings.iSnDbAdjust) ||
		   (pAnrSettings->pvUserInfo  != pCurInfo->stAnrSettings.pvUserInfo) ||
		   (pAnrSettings->stVideoTestFeature1.bEnable != pCurAnr->stVideoTestFeature1.bEnable) ||
		   (pAnrSettings->stVideoTestFeature1.ulBitsPerPixel != pCurAnr->stVideoTestFeature1.ulBitsPerPixel) ||
		   (pAnrSettings->ePxlFormat  != pCurInfo->stAnrSettings.ePxlFormat))
		{
			pNewInfo->stDirty.stBits.bAnrAdjust = BVDC_P_DIRTY;
		}
	}
#else
	BSTD_UNUSED(pAnrSettings);
	eStatus = BERR_TRACE(BVDC_ERR_ANR_HW_NOT_AVAILABLE);
#endif

	BDBG_LEAVE(BVDC_Window_SetAnrConfiguration);
	return eStatus;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Window_GetAnrConfiguration
	( BVDC_Window_Handle               hWindow,
	  BVDC_Anr_Settings               *pAnrSettings )
{
	BERR_Code  eStatus = BERR_SUCCESS;

	BDBG_ENTER(BVDC_Window_GetAnrConfiguration);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

#if (BVDC_P_SUPPORT_ANR) || (BVDC_P_SUPPORT_MANR)
	if(pAnrSettings)
	{
		*pAnrSettings = hWindow->stCurInfo.stAnrSettings;
	}
	else
	{
		eStatus = BERR_TRACE(BERR_INVALID_PARAMETER);
	}
#else
	BSTD_UNUSED(pAnrSettings);
	eStatus = BERR_TRACE(BVDC_ERR_ANR_HW_NOT_AVAILABLE);
#endif
	BDBG_LEAVE(BVDC_Window_GetAnrConfiguration);
	return eStatus;
}


/***************************************************************************
 *
 */

BERR_Code BVDC_Window_SetBandwidthEquationParams
	( BVDC_Window_Handle               hWindow,
	  const uint32_t                   ulDelta,
	  const BVDC_SclCapBias	           eSclCapBias)
{
	BERR_Code  eStatus = BERR_SUCCESS;

	BDBG_ENTER(BVDC_Window_SetBandwidthEquationParams);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	hWindow->stNewInfo.eSclCapBias = eSclCapBias;
	hWindow->stNewInfo.ulBandwidthDelta = ulDelta;

	BDBG_LEAVE(BVDC_Window_SetBandwidthEquationParams);
	return eStatus;
}


/***************************************************************************
 *
 */

BERR_Code BVDC_Window_GetBandwidthEquationParams
	( BVDC_Window_Handle               hWindow,
	  uint32_t                         *pulDelta,
	  BVDC_SclCapBias                  *peSclCapBias)
{
	BERR_Code  eStatus = BERR_SUCCESS;

	BDBG_ENTER(BVDC_Window_GetBandwidthEquationParams);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	*pulDelta = hWindow->stCurInfo.ulBandwidthDelta;
	*peSclCapBias = hWindow->stCurInfo.eSclCapBias;

	BDBG_LEAVE(BVDC_Window_GetBandwidthEquationParams);
	return eStatus;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Window_SetTestFeature1
	( BVDC_Window_Handle                       hWindow,
	  const BVDC_Video_TestFeature1_Settings  *pTestFeature1Settings )
{
	BVDC_Video_TestFeature1_Settings         *pNewTestFeature1;
	const BVDC_Video_TestFeature1_Settings   *pCurTestFeature1;

	BDBG_ENTER(BVDC_Window_SetTestFeature1);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	pNewTestFeature1 = &hWindow->stNewInfo.stCapTestFeature1;
	pCurTestFeature1 = &hWindow->stCurInfo.stCapTestFeature1;

#if (!BVDC_P_SUPPORT_VIDEO_TESTFEATURE1_CAP)
	BSTD_UNUSED(pTestFeature1Settings);
	BDBG_WRN(("Video TestFeature1 is not supported on this chip!"));
#else
	BDBG_ASSERT(pTestFeature1Settings);

	/* only support main display's window */
	if((pTestFeature1Settings->bEnable) &&
	   (hWindow->eId != BVDC_P_WindowId_eComp0_V0))
	{
		return BERR_TRACE(BVDC_ERR_VIDEO_TestFeature1_NON_MAIN_WINDOW);
	}

	/* set new value */
	pNewTestFeature1->bEnable = pTestFeature1Settings->bEnable;
	pNewTestFeature1->ulBitsPerPixel = pTestFeature1Settings->ulBitsPerPixel;

	if((pNewTestFeature1->bEnable        != pCurTestFeature1->bEnable) ||
	   (pNewTestFeature1->ulBitsPerPixel != pCurTestFeature1->ulBitsPerPixel))
	{
		hWindow->stNewInfo.stDirty.stBits.bTestFeature1 = BVDC_P_DIRTY;
	}
#endif

	BDBG_LEAVE(BVDC_Window_SetTestFeature1);
	return BERR_SUCCESS;

}

/***************************************************************************
 *
 */
BERR_Code BVDC_Window_GetTestFeature1
	( BVDC_Window_Handle                       hWindow,
	  BVDC_Video_TestFeature1_Settings        *pTestFeature1Settings )
{
	const BVDC_Video_TestFeature1_Settings *pCurTestFeature1;

	BDBG_ENTER(BVDC_Window_GetTestFeature1);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	/* Cur Info */
	pCurTestFeature1 = &hWindow->stCurInfo.stCapTestFeature1;

	/* set new value */
	if(pTestFeature1Settings)
	{
		pTestFeature1Settings->bEnable        = pCurTestFeature1->bEnable;
		pTestFeature1Settings->ulBitsPerPixel = pCurTestFeature1->ulBitsPerPixel;
	}

	BDBG_LEAVE(BVDC_Window_GetTestFeature1);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Window_SetScalerConfiguration
	( BVDC_Window_Handle               hWindow,
	  const BVDC_Scaler_Settings      *pScalerSettings )
{
	BDBG_ENTER(BVDC_Window_SetScalerConfiguration);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	if(pScalerSettings)
	{
		const BVDC_Scaler_Settings *pCurSclSettings = &hWindow->stCurInfo.stSclSettings;
		hWindow->stNewInfo.stSclSettings = *pScalerSettings;

		/* Flag change */
		if((pScalerSettings->bSclVertDejagging       != pCurSclSettings->bSclVertDejagging      ) ||
		   (pScalerSettings->bSclHorzLumaDeringing   != pCurSclSettings->bSclHorzLumaDeringing  ) ||
		   (pScalerSettings->bSclVertLumaDeringing   != pCurSclSettings->bSclVertLumaDeringing  ) ||
		   (pScalerSettings->bSclHorzChromaDeringing != pCurSclSettings->bSclHorzChromaDeringing) ||
		   (pScalerSettings->bSclVertChromaDeringing != pCurSclSettings->bSclVertChromaDeringing) ||
		   (pScalerSettings->bSclVertPhaseIgnore     != pCurSclSettings->bSclVertPhaseIgnore    ) ||
		   (pScalerSettings->ulSclDejaggingCore      != pCurSclSettings->ulSclDejaggingCore     ) ||
		   (pScalerSettings->ulSclDejaggingGain      != pCurSclSettings->ulSclDejaggingGain     ) ||
		   (pScalerSettings->ulSclDejaggingHorz      != pCurSclSettings->ulSclDejaggingHorz     ))
		{
			hWindow->stNewInfo.stDirty.stBits.bMiscCtrl = BVDC_P_DIRTY;
		}
	}

	BDBG_LEAVE(BVDC_Window_SetScalerConfiguration);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Window_GetScalerConfiguration
	( BVDC_Window_Handle               hWindow,
	  BVDC_Scaler_Settings            *pScalerSettings )
{
	BDBG_ENTER(BVDC_Window_GetScalerConfiguration);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	/* set new value */
	if(pScalerSettings)
	{
		*pScalerSettings = hWindow->stCurInfo.stSclSettings;
	}

	BDBG_LEAVE(BVDC_Window_GetScalerConfiguration);
	return BERR_SUCCESS;
}


/***************************************************************************
 *
 */
BERR_Code BVDC_Window_GetStatus
	( const BVDC_Window_Handle         hWindow,
	  BVDC_Window_Status              *pWindowStatus )
{
	BDBG_ENTER(BVDC_Window_GetStatus);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	/* set new value */
	if(pWindowStatus)
	{
		pWindowStatus->bSyncLock = hWindow->bSyncLockSrc;
	}

	BDBG_LEAVE(BVDC_Window_GetStatus);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Window_SetDstRightRect
	( const BVDC_Window_Handle         hWindow,
	  int32_t                          lRWinXOffsetDelta)
{
	BDBG_ENTER(BVDC_Window_SetDstRightRect);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	/* set new value */
	hWindow->stNewInfo.lRWinXOffsetDelta  = lRWinXOffsetDelta;
	hWindow->stNewInfo.stDirty.stBits.b3D = BVDC_P_DIRTY;

	BDBG_LEAVE(BVDC_Window_SetDstRightRect);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Window_GetDstRightRect
	( const BVDC_Window_Handle         hWindow,
	  int32_t                         *plRWinXOffsetDelta )
{
	BDBG_ENTER(BVDC_Window_GetDstRightRect);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	/* set new value */
	if(plRWinXOffsetDelta)
	{
		*plRWinXOffsetDelta = hWindow->stCurInfo.lRWinXOffsetDelta;
	}

	BDBG_LEAVE(BVDC_Window_GetDstRightRect);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Window_GetCapabilities
	( BVDC_Window_Handle               hWindow,
	  BVDC_Window_Capabilities        *pCapabilities )
{
	BSTD_UNUSED(hWindow);
	BSTD_UNUSED(pCapabilities);

	return BERR_SUCCESS;
}
/* End of File */
