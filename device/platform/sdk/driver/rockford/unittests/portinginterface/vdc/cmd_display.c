/***************************************************************************
 *     Copyright (c) 2004-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: cmd_display.c $
 * $brcm_Revision: Hydra_Software_Devel/149 $
 * $brcm_Date: 10/10/12 5:05p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/portinginterface/vdc/cmd_display.c $
 * 
 * Hydra_Software_Devel/149   10/10/12 5:05p vanessah
 * SW7445-10: add emu support for 7445
 * 
 * Hydra_Software_Devel/148   10/8/12 3:30p syang
 * SW7346-572: use BTST_P_ApplyChanges to get uniform error handling;
 * avoid holding hVdcCfgLock too long
 * 
 * Hydra_Software_Devel/147   9/25/12 5:35p vanessah
 * SW7425-2501: vdc_test clean up
 * 
 * Hydra_Software_Devel/146   9/12/12 2:42p tdo
 * SW7346-1028: BHDM module needs to destroy timers
 *
 * Hydra_Software_Devel/145   9/11/12 10:42a vanessah
 * SW7405-5689: SW7425-2501: DCS usage cleanup in vdc_test
 *
 * Hydra_Software_Devel/144   9/5/12 1:16p vanessah
 * SW7425-2501: removal of vcxo initialization into appframework
 *
 * Hydra_Software_Devel/143   8/21/12 3:02p tdo
 * SW7429-238: Set rfm to unused when disable
 *
 * Hydra_Software_Devel/142   7/31/12 3:02p tdo
 * SW7563-9: Add support for VDC
 *
 * Hydra_Software_Devel/141   7/11/12 1:46p vanessah
 * SW7425-3460: rate tracking test
 *
 * Hydra_Software_Devel/140   7/9/12 5:16p vanessah
 * SW7425-2501: removal 3548/3556 support
 *
 * Hydra_Software_Devel/139   5/23/12 11:31a vanessah
 * SW7360-23: add 7360 a0 support
 *
 * Hydra_Software_Devel/138   5/21/12 5:41p darnstein
 * HW7552-167: add more Macrovision custom options. Required by Rovi.
 *
 * Hydra_Software_Devel/137   5/16/12 5:09p vanessah
 * SW7425-467: fix custom fmt error
 *
 * Hydra_Software_Devel/136   5/3/12 1:32p tdo
 * SW7584-21: Add PI/vdc support for 7584
 *
 * Hydra_Software_Devel/135   4/10/12 3:44p tdo
 * SW7435-97: Format change on the SD path with progressive stream cause
 * HD path to flicker
 *
 * Hydra_Software_Devel/134   4/5/12 6:18p darnstein
 * SW7405-5639: Both the old and new DCS APIs are now available in the
 * interface file bvdc_dcs.h.
 *
 * Hydra_Software_Devel/133   3/5/12 9:26a yuxiaz
 * SW7425-2526: Added tests for runtime capabilities query.
 *
 * Hydra_Software_Devel/132   1/19/12 3:27p tdo
 * SW7435-9: Bringup VDC_TEST_ONLY for 7435 emulation
 *
 * Hydra_Software_Devel/131   1/17/12 9:10a darnstein
 * SW7125-1124: test of DCS locking had extraneous call to turn DCS off.
 *
 * Hydra_Software_Devel/130   1/11/12 3:08p darnstein
 * SW7125-1124: merge DCS Hybrid+ to main branch. DCS almost ready for
 * production.
 *
 * Hydra_Software_Devel/SW7125-1124/5   12/19/11 5:41p darnstein
 * SW7125-1124: merge from main branch.
 *
 * Hydra_Software_Devel/SW7125-1124/4   12/14/11 4:45p darnstein
 * SW7125-1124: merge from main branch.
 *
 *
 * Hydra_Software_Devel/SW7125-1124/3   12/13/11 6:23p darnstein
 * SW7125-1124: DCS LOCK ON function added to API.
 *
 * Hydra_Software_Devel/SW7125-1124/2   12/8/11 2:37p darnstein
 * SW7125-1124: merge from main branch.
 *
 * Hydra_Software_Devel/SW7125-1124/1   12/1/11 4:02p darnstein
 * SW7125-1124: adapt to Hybrid+ DCS.
 *
 * Hydra_Software_Devel/126   11/29/11 11:40a vanessah
 * SW7425-1783: add unittest for stg picture id call back
 *
 * Hydra_Software_Devel/125   11/18/11 2:35p pntruong
 * SW7344-239: Extended "dacs" command to include filtered cvbs output for
 * external rfm.
 *
 * Hydra_Software_Devel/124   9/30/11 5:16p darnstein
 * SW7400-2665: merge from JIRA branch.
 *
 * Hydra_Software_Devel/SW7400-2665/1   9/28/11 7:29p darnstein
 * SW7400-2665: Before printing out VEC IT microcode identity, wait a
 * while. This is necessary because there is a variable delay until BVDC
 * loads new microcode into registers.
 *
 * Hydra_Software_Devel/123   9/22/11 4:10p vanessah
 * SW7425-691: No DAC change if it is disabled
 *
 * Hydra_Software_Devel/122   9/20/11 3:02p yuxiaz
 * SW7425-1326: Fixed window size setting when switching between 3dLR and
 * 3dOU for disp3d command.
 *
 * Hydra_Software_Devel/121   8/25/11 10:44a tdo
 * SW7420-2030: Add apply changes error tracking capability
 *
 * Hydra_Software_Devel/120   8/24/11 9:06a vanessah
 * SW7425-630: remove compile warning msg
 *
 * Hydra_Software_Devel/119   8/17/11 9:56a vanessah
 * SW7425-922: b0 vdc_test bringup. VDC_TEST_ONLY for emulation
 *
 * Hydra_Software_Devel/118   7/18/11 11:28a vanessah
 * SW7425-835: SW7425-923: fix ViCE2 channel id bug + B0 STG
 *
 * Hydra_Software_Devel/117   7/11/11 1:20p albertl
 * SW7408-291: Updated tests with output selection.
 *
 * Hydra_Software_Devel/116   7/6/11 6:41p albertl
 * SW7408-291, SW7425-745:: Added tests for vf filter settings and
 * selective output mute.
 *
 * Hydra_Software_Devel/115   5/2/11 9:22a vanessah
 * SW7425-467: fix customer fmt for vdc_test
 *
 * Hydra_Software_Devel/114   3/24/11 5:11p tdo
 * SW7422-13: Turn off DACs for 3D formats
 *
 * Hydra_Software_Devel/113   2/7/11 5:23p vanessah
 * SW7425-29: STG toggel
 *
 * Hydra_Software_Devel/112   1/27/11 5:30p vanessah
 * SW7425-32: use BXVD refresh rate to sync XVD output
 *
 * Hydra_Software_Devel/111   1/26/11 4:57p vanessah
 * SW7425-32: Add custom format frame rate for STG display
 *
 * Hydra_Software_Devel/110   1/20/11 5:08p tdo
 * SW7422-13: Correctly set window size for 3D cases
 *
 * Hydra_Software_Devel/109   12/21/10 11:08a tdo
 * SW7422-28: Adjust to 3D format size change.  Add test for single MEMC
 * scheme.
 *
 * Hydra_Software_Devel/108   12/14/10 1:04p tdo
 * SW7422-13: Set correct window size in 3D full resolution cases
 *
 * Hydra_Software_Devel/107   12/13/10 12:05p tdo
 * SW7422-13: Add BHDM 3D setting
 *
 * Hydra_Software_Devel/105   12/8/10 5:48p tdo
 * SW7422-13: Add new command to set display 3D orientation
 *
 * Hydra_Software_Devel/104   12/7/10 2:49p tdo
 * SW7422-13: Add new command to set display 3D orientation
 *
 * Hydra_Software_Devel/103   12/1/10 6:13p tdo
 * SW7422-44: Handle 3D changes for display
 *
 * Hydra_Software_Devel/102   11/30/10 11:37p hongtaoz
 * SW7425-32: added custom format test for STG display;
 *
 * Hydra_Software_Devel/101   11/24/10 1:24p hongtaoz
 * SW7425-29: digital display sets digital format size;
 *
 * Hydra_Software_Devel/100   11/24/10 12:56p pntruong
 * SW7552-17: Added initial 7552 support.
 *
 * Hydra_Software_Devel/99   11/17/10 8:18a pntruong
 * SW7231-16: Fixed 7400 build error.
 *
 * Hydra_Software_Devel/98   11/16/10 5:00p pntruong
 * SW7231-16: Initial support for 7231.
 *
 * Hydra_Software_Devel/97   11/15/10 3:05p vanessah
 * SW7550-577:  fix color space error for HDMI
 *
 * Hydra_Software_Devel/96   11/12/10 7:27a pntruong
 * SW7425-31: Fixed more build errors.
 *
 * Hydra_Software_Devel/95   11/12/10 6:59a pntruong
 * SW7425-31: More build errors.
 *
 * Hydra_Software_Devel/94   11/11/10 11:56p pntruong
 * SW7425-31: Fixed 7125 build errors.
 *
 * Hydra_Software_Devel/93   11/11/10 6:45p pntruong
 * SW7425-31: Refactored hdm/lvds/hdr/dvo and added support hdmi input on
 * stb chips.  Removed dead codes.
 *
 * Hydra_Software_Devel/92   11/11/10 10:06a yuxiaz
 * SW7422-77: Fixed DAC settings for 3D formats.
 *
 * Hydra_Software_Devel/91   10/18/10 3:51p tdo
 * SW7550-598: Fix segfault when toggling hdm output for 7420
 *
 * Hydra_Software_Devel/90   10/15/10 2:22p vanessah
 * SW7358-15:  VDC_test compile w.r.t DMA
 *
 * Hydra_Software_Devel/89   10/12/10 5:50p vanessah
 * SW7550-598:  EC fix
 *
 * Hydra_Software_Devel/88   10/12/10 12:02p vanessah
 * SW7550-598:  Localize display setting
 *
 * Hydra_Software_Devel/85   10/4/10 11:32p hongtaoz
 * SW7425-33, SW7425-29: added cmp3 test; added STG setting test; STG
 * master should not set DACs;
 *
 * Hydra_Software_Devel/84   9/27/10 4:06p hongtaoz
 * SW7425-32: added VDC_TEST_ONLY compile option;
 *
 * Hydra_Software_Devel/83   9/23/10 3:33p tdo
 * SW7422-13: add 7422 support to vdc_test
 *
 * Hydra_Software_Devel/82   9/7/10 3:19p yuxiaz
 * SW7422-28: Add 3D formats to FMT plus additional data structures
 *
 * Hydra_Software_Devel/81   7/26/10 5:01p yuxiaz
 * SW3548-2987: Added new 1080p@30hz support for 3DTV.
 *
 * Hydra_Software_Devel/80   6/17/10 5:58p hongtaoz
 * SW3548-2912: added support for pseudo-1080i input for 3DTV; allow API
 * to disable SCL vertical phase shift; added 540p60/50 custom formats to
 * test pseudo-1080i 3DTV;
 *
 * Hydra_Software_Devel/79   4/27/10 5:16p darnstein
 * SW7335-696: add option to output PAL-I.
 *
 * Hydra_Software_Devel/78   4/19/10 4:36p hongtaoz
 * SW3548-2828: added 720p3D display formats to vdc unit test;
 *
 * Hydra_Software_Devel/77   4/14/10 2:46p darnstein
 * SW7420-543: Uncomment some code related to 480P@54 MHz.
 *
 * Hydra_Software_Devel/76   4/7/10 4:32p darnstein
 * SW7420-543: Analog 480P output is always sampled at 54Mhz.
 *
 * Hydra_Software_Devel/75   3/1/10 3:53p darnstein
 * SW7400-2620,SW7400-2700: Don't change DVI-DTG toggle bits anymore.
 * These are now managed by BVDC.
 *
 * Hydra_Software_Devel/74   2/24/10 6:49p darnstein
 * SW7400-2620: For this JIRA, program the DVI_DTG.DTG_CONFIG.TOGGLE_DVI_H
 * and DVI_DTG.DTG_CONFIG.TOGGLE_DVI_V register bitfields.
 *
 * Hydra_Software_Devel/73   2/23/10 6:27p darnstein
 * SW7400-2620: HDMI pixel replication disabled temporarily. Waiting for
 * PREREL labels to get updated so that nightly builds can finish.
 *
 * Hydra_Software_Devel/72   2/23/10 3:59p darnstein
 * SW7400-2620: previous checkin caused HDMI functions to be called even
 * if environment does not support HDMI.
 *
 * Hydra_Software_Devel/71   2/23/10 3:08p darnstein
 * SW7400-2620: When setting HDMI pixel repetition, do function calls in
 * the order specified by Anthony Le. This required me to remove the
 * bApplyImmediately (not) option.
 *
 * Hydra_Software_Devel/70   2/1/10 6:45p hongtaoz
 * SW3548-2678, SW3548-2680, SW3548-2681: added HDMI1.4 3D formats support
 * for 3548; added multi-buffer low delay mode support for 1to2 and 2to5
 * rate gap cases; reduced capture buffer allocation for interlaced
 * pulldown case;
 *
 * Hydra_Software_Devel/BCM3D/3   1/29/10 7:02p hongtaoz
 * SW3548-2680, SW3548-2681: ermoved 1470p custom formats; wrapped 2160i
 * custom format with compile flag for legacy 3DTV chipsets;
 *
 * Hydra_Software_Devel/BCM3D/2   1/15/10 6:38p hongtaoz
 * SW3548-2680, SW3548-2681: added custom formats to support HDMI 1.4 3D
 * 720p50/60 input passed-thru to external panel processor; reanmed 2160i
 * to custom DVI format;
 *
 * Hydra_Software_Devel/BCM3D/1   12/17/09 6:29p hongtaoz
 * SW3548-2680, SW3548-2681: added new special display format 2160i48 to
 * support HDMi1.4 3D format for 11-bit BVN; added BHDR frontend config
 * to support 3D video testing;
 *
 * Hydra_Software_Devel/69   12/16/09 3:27p tdo
 * SW7342-126: Add command to turn on/off DACs
 *
 * Hydra_Software_Devel/68   10/16/09 5:22p darnstein
 * SW7405-3191: Back out all changes in pixel frequency handling. New
 * design coming soon.
 *
 * Hydra_Software_Devel/67   10/15/09 5:04p darnstein
 * SW7420-376: restore an include file that was removed.
 *
 * Hydra_Software_Devel/66   10/15/09 4:59p darnstein
 * SW7405-3191: Pixel frequency is now defined as both a bitmask, and an
 * enum. The bitmask option is DEPRECATED.
 *
 * Hydra_Software_Devel/65   10/14/09 4:47p tdo
 * SW7420-376: Bringup appframework for 7410
 *
 * Hydra_Software_Devel/64   10/1/09 7:45p darnstein
 * SW7405-2612: add support for three new VESA display formats.
 *
 * Hydra_Software_Devel/63   9/29/09 10:54a tdo
 * SW7420-360: Fix Dac assignment for cvbs command
 *
 * Hydra_Software_Devel/62   8/31/09 4:17p darnstein
 * SW7342-26: BFramework_GetDacAssignments() replaced by #define's.
 *
 * Hydra_Software_Devel/61   8/27/09 7:09p darnstein
 * SW7342-26: DAC assignments to reference board go through
 * BFramework_GetDacAssignments().
 *
 * Hydra_Software_Devel/60   8/5/09 4:13p syang
 * PR 55812: update 7550 DAC mapping
 *
 * Hydra_Software_Devel/59   7/31/09 6:48p syang
 * PR 55812:  add 7550 support
 *
 * Hydra_Software_Devel/58   7/21/09 4:25p rpan
 * PR56037: Keep the default behavior that DACs are disabled during VESA
 * output.
 *
 * Hydra_Software_Devel/57   7/21/09 4:23p rpan
 * PR56037: Initial effort for analog VESA outputs.
 *
 * Hydra_Software_Devel/56   7/10/09 5:53p rpan
 * PR55188: Convert scanf() to BTST_P_Scanf().
 *
 * Hydra_Software_Devel/55   6/4/09 9:23p pntruong
 * PR48884: Fixed build warnings on 93549.
 *
 * Hydra_Software_Devel/54   6/2/09 3:22p darnstein
 * PR48884: More resistance to invalid user inputs.
 *
 * Hydra_Software_Devel/53   6/2/09 3:00p syang
 * PR 55646: call BXVD_SetMonitorRefreshRate for 60Hz disp format
 *
 * Hydra_Software_Devel/52   6/1/09 2:15p darnstein
 * PR48884: prevent more crashes when user specifies bad video
 * configuration on VEC. These changes specifically support 7420.
 *
 * Hydra_Software_Devel/51   5/29/09 2:22p darnstein
 * PR48884: improve text formatting from the "udmp" command.
 *
 * Hydra_Software_Devel/50   5/27/09 6:47p darnstein
 * PR48884: The "udmp" command tests video format BFMT_VideoFmt_eCustom2
 * along with all the others. Sure, it generates error messages.
 *
 * Hydra_Software_Devel/49   5/27/09 6:40p darnstein
 * PR48884: after "udmp" command finishes its job, it puts the VEC into a
 * simple state that is unlikely to fail: NTSC video, no HDMI, no ITU-R
 * 656.
 *
 * Hydra_Software_Devel/48   5/26/09 6:37p darnstein
 * PR48884: The "udmp" command now tries every video format defined by
 * BFMT.
 *
 * Hydra_Software_Devel/47   5/15/09 10:30a rpan
 * PR55188: Added support for capturing and repeating comamnds.
 *
 * Hydra_Software_Devel/46   5/4/09 4:25p rpan
 * PR53106: Added test case for new format 720p25/30Hz.
 *
 * Hydra_Software_Devel/45   4/7/09 10:02a tdo
 * PR53373, PR53239: Add HDMI deep color support and remove usage of
 * private definitions in vdc unittest
 *
 * Hydra_Software_Devel/44   3/30/09 4:14p rpan
 * PR53677: Added command for NTSC->PAL picture drop mode selection.
 *
 * Hydra_Software_Devel/43   3/30/09 3:21p hongtaoz
 * PR53037, PR53038: added forced synclock mode and double-buffer support;
 * added more VEC locking rates support;
 *
 * Hydra_Software_Devel/42   3/17/09 3:57p darnstein
 * PR49987: Try to get VCXO stuff right for 7405.
 *
 * Hydra_Software_Devel/41   3/17/09 1:58p darnstein
 * PR53265: Remove BVBI, BVBIlib software. It never worked.
 *
 * Hydra_Software_Devel/40   3/11/09 9:47p pntruong
 * PR51930, PR52072, PR52364: Added "spread spectrum" - ss toggle command.
 *
 * Hydra_Software_Devel/39   3/5/09 4:41p yuxiaz
 * PR52179: Video Rate Change callback invoked continously
 *
 * Hydra_Software_Devel/38   2/3/09 2:38p rpan
 * PR45804: Reset hHdm handle to NULL after HDMI being turned off.
 *
 * Hydra_Software_Devel/37   2/3/09 9:46a rpan
 * PR45804: Default 7420 HDMI output color space to RGB.
 *
 * Hydra_Software_Devel/36   1/15/09 6:23p hongtaoz
 * PR50621: repositioned trigger locations to as early as possible for DTV
 * display timing to have enough head room for the longest RULs;
 * corrected custom format ulTopActive value to reflect the microcode
 * timing;
 *
 * Hydra_Software_Devel/35   1/6/09 5:31p darnstein
 * PR45737: Add capability to change output format to 1080P@50Hz.
 *
 * Hydra_Software_Devel/34   11/13/08 8:35p pntruong
 * PR48884: Fxied build errors.
 *
 * Hydra_Software_Devel/33   11/13/08 5:39p darnstein
 * PR48884: microcode dump command "udmp" is ready for evaluation.
 *
 * Hydra_Software_Devel/32   11/12/08 4:36p darnstein
 * PR40806: Conform to programming standards in
 * BVDC_Display_GetItUcodeInfo().
 *
 * Hydra_Software_Devel/31   11/12/08 3:12p darnstein
 * PR40806: Dump VEC IT microcode info from the "cf" command. This might
 * be temporary.
 *
 * Hydra_Software_Devel/30   11/7/08 6:47p darnstein
 * PR46484: Command ddmp dumps tables of register values from BVDC display
 * software.
 *
 * Hydra_Software_Devel/29   8/5/08 8:12p pntruong
 * PR39163: Added xvycc toggle.
 *
 * Hydra_Software_Devel/28   6/26/08 4:49p rpan
 * PR39529: Added uCode and RM settings for 1080p48Hz panel.
 *
 * Hydra_Software_Devel/27   6/23/08 8:12p pntruong
 * PR34239:  Allow dynamically loading of vec custom timing.
 *
 * Hydra_Software_Devel/26   6/19/08 2:35p rpan
 * PR43712: Add SCART-SVideo test case.
 *
 * Hydra_Software_Devel/25   6/19/08 12:24p rpan
 * PR43712: Add SCART test cases.
 *
 * Hydra_Software_Devel/24   6/17/08 2:29p rpan
 * PR43668: Adopt the new LVD PI for 3548/3556.
 *
 * Hydra_Software_Devel/23   6/9/08 1:56p rpan
 * PR39529: Back out FMT_VideoFmt_e1080p_48Hz change. Will use
 * FMT_Video_eCustom to test 48Hz panel.
 *
 * Hydra_Software_Devel/22   6/9/08 10:52a rpan
 * PR39529: Added 1080p48 test case.
 *
 * Hydra_Software_Devel/21   6/7/08 3:13p rpan
 * PR39337: Some cleanup for OSD setting tests.
 *
 * Hydra_Software_Devel/20   6/7/08 3:10p rpan
 * PR39337: Added compositor OSD setting test cases.
 *
 * Hydra_Software_Devel/19   6/6/08 11:24a rpan
 * PR42109: Added tests for all SECAM variants.
 *
 * Hydra_Software_Devel/18   6/4/08 8:37p albertl
 * PR39807:  Added correct HDMI default colorspace for 3548/3549 and 3556.
 *
 * Hydra_Software_Devel/17   5/27/08 7:22p hongtaoz
 * PR42248, PR42494: added display alignment test;
 *
 * Hydra_Software_Devel/16   3/10/08 8:35p darnstein
 * PR25971: first try at DCS for 7400
 *
 * Hydra_Software_Devel/15   2/11/08 9:21p pntruong
 * PR39177: [VEC] Need new display api to specify framedrop at runtime.
 *
 * Hydra_Software_Devel/14   1/22/08 7:25p albertl
 * PR36606:  Changed scale factor rounding to be done only when windows
 * exist.
 *
 * Hydra_Software_Devel/13   11/29/07 3:16p rpan
 * PR36606: Set scale factor rounding for 480p and NTSC display format to
 * improve frequency response.
 *
 * Hydra_Software_Devel/12   10/30/07 7:28p pntruong
 * PR34239: Allow dynamically loading of vec custom timing.
 *
 * Hydra_Software_Devel/11   10/25/07 6:58p pntruong
 * PR34857: Fixed selection of eCustom1.
 *
 * Hydra_Software_Devel/10   10/19/07 5:05p rpan
 * PR36026: Verified that enabling RFM does not necessarily have to drive
 * a CVBS DAC.
 *
 * Hydra_Software_Devel/9   10/19/07 11:27a yuxiaz
 * PR36240: invalid pixel clock rate from VDC callback to HDMI.
 *
 * Hydra_Software_Devel/8   10/16/07 2:44p rpan
 * PR34857: First version after merging the latest xvd_vdc_test and
 * mvd_vdc_test as of Oct. 16, 2007, 12pm.
 *
 *
 * Hydra_Software_Devel/1   9/21/07 4:40p rpan
 * PR34857: Initial revision.
 *
 *
 ***************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "config.h"
#include "test_util.h"
#include "commands.h"
#include "mpg_decoder.h"
#include "bvdc_dbg.h"
#include "bkni_multi.h"
#include "bmem_debug.h"
#include "bdbg.h"
#include "bstd_defs.h"
#include "bvdc.h"
#include"framework_board_bvdc.h"

BDBG_MODULE(BTST);

static void BTST_P_SetDacs(
	BTST_P_Context *pContext, bool bEnable, BFMT_VideoFmt eFmt );

/* TODO: This is currently specific to chipset!  Please look at bfmt_custom.c  */
typedef struct
{
	/* Use for searching a matching one! */
	uint32_t                    ulPixelClkRate;
	uint32_t                    ulMDiv;
	uint32_t                    ulNDiv; /* Offset: 9.22 format */
	uint32_t                    ulRDiv;
	uint32_t                    ulSampleInc;
	uint32_t                    ulNumerator;
	uint32_t                    ulDenominator;
	uint32_t                    ulVcoRange;
	uint32_t                    ulLinkDivCtrl;
	uint32_t                    ulP2;
	const char                 *pchRate;
} BFMT_P_RateInfo;

/* Support different format timings.  Application loadable module */
static const uint32_t s_vec_tb_noprim_dvim1366x768p_60hz_bvb_input_bss[] =
{
	0x0064A001, /*  64 */
	0x00650009, /*  65 */
	0x00656001, /*  66 */
	0x0065C00F, /*  67 */
	0x00668300, /*  68 */
	0x0065C007, /*  69 */
	0x00662001, /*  70 */
	0x00840000, /*  71 */
	0x00000000, /*  72 */
	0x00000000, /*  73 */
	0x00284038, /*  74 */
	0x002D400C, /*  75 */
	0x0024406A, /*  76 */
	0x0034554B, /*  77 */
	0x00000000, /*  78 */
	0x00000000, /*  79 */
	0x00244038, /*  80 */
	0x0025400C, /*  81 */
	0x0024406A, /*  82 */
	0x0034554B, /*  83 */
	0x00000000, /*  84 */
	0x00000000, /*  85 */
	0x00244038, /*  86 */
	0x0021400C, /*  87 */
	0x0020406A, /*  88 */
	0x0030554B, /*  89 */
	0x00000000, /*  90 */
	0x00000000, /*  91 */
	0x00204038, /*  92 */
	0x0021400C, /*  93 */
	0x0020406A, /*  94 */
	0x0030554B, /*  95 */
	0x00000000, /*  96 */
	0x00000000, /*  97 */
	0x00204038, /*  98 */
	0x0021400C, /*  99 */
	0x0020406A, /* 100 */
	0x00305540, /* 101 */
	0x00000000, /* 102 */
	0x00000000, /* 103 */
	0x00202038, /* 104 */
	0x0021200C, /* 105 */
	0x0020206A, /* 106 */
	0x0032354B, /* 107 */
	0x00000000, /* 108 */
	0x00000000, /* 109 */
	0x00202038, /* 110 */
	0x0021200C, /* 111 */
	0x0020206A, /* 112 */
	0x00323540, /* 113 */
	0x00000000, /* 114 */
	0x00000000, /* 115 */
	0x00000000, /* 116 */
	0x00000000, /* 117 */
	0x00000000, /* 118 */
	0x00000000, /* 119 */
	0x00000000, /* 120 */
	0x00000000, /* 121 */
	0x00000000, /* 122 */
	0x00000000, /* 123 */
	0x00000000, /* 124 */
	0x00000000, /* 125 */
	0x00072507, /* 126 */
	0x005D4D0E, /* 127 */
};

#if (1==BTST_VDC_ONLY)
#define  BXVD_MONITOR_REFRESH_RATE_23_976Hz              0
#define  BXVD_MONITOR_REFRESH_RATE_24Hz                  0
#define  BXVD_MONITOR_REFRESH_RATE_25Hz                  0
#define  BXVD_MONITOR_REFRESH_RATE_29_97Hz               0
#define  BXVD_MONITOR_REFRESH_RATE_30Hz                  0
#define  BXVD_MONITOR_REFRESH_RATE_48Hz                  0
#define  BXVD_MONITOR_REFRESH_RATE_50Hz                  0
#define  BXVD_MONITOR_REFRESH_RATE_59_94Hz               0
#define  BXVD_MONITOR_REFRESH_RATE_60Hz                  0
#endif
/* The following default settings are from 3563C0 1366x768 DVO bringup:
' Pixel Clock = h_total * v_total * refresh_rate
'             = 1540 * 802 * 60
'             = 74104800 Hz
' VCO = 2074934400.00 Hz
BCM3563.LVDS_PHY_0.LVDS_PLL_CFG.PLL_FEEDBACK_PRE_DIVIDER = 1 ' (p2)
BCM3563.LVDS_PHY_0.LVDS_PLL_CFG.PLL_INPUT_PRE_DIVIDER = 2    ' (p1)
BCM3563.LVDS_PHY_0.LVDS_PLL_CTL_2.ndiv_mode = 1
BCM3563.LVDS_PHY_0.LVDS_PLL_CTL_1 = &h382C24A0&
BCM3563.DVPO_0.FIFO_CTL.MASTER_OR_SLAVE_N = 1
 */
static const BFMT_P_RateInfo s_stDvoRmTbl0 =
	{BFMT_PXL_74_25MHz,           2, 0x13365CEF,  99, 2,  11327,    19649,  1,  1,  1, "74.10"}; /* 74.10 MHz */

/* The following default settings are from 3563C0 1366x768 DVO bringup:
' Pixel Clock = h_total * v_total * refresh_rate
'             = 1540 * 802 * 60 * 1000 / 1001
'             = 74030769 Hz
' VCO = 2072861532.00 Hz
BCM3563.LVDS_PHY_0.LVDS_PLL_CFG.PLL_INPUT_PRE_DIVIDER = 2    ' (p1)
BCM3563.LVDS_PHY_0.LVDS_PLL_CTL_2.ndiv_mode = 1
BCM3563.LVDS_PHY_0.LVDS_PLL_CTL_1 = &h382C24A0&
BCM3563.DVPO_0.FIFO_CTL.MASTER_OR_SLAVE_N = 1
 */

static const BFMT_P_RateInfo s_stDvoRmTbl1 =
	{BFMT_PXL_74_25MHz_DIV_1_001,           2, 0x13317316,  112, 2,  368,    401,  1,  1,  1, "74.03"}; /* 74.03 MHz */

static BFMT_CustomFormatInfo s_stCustomInfo0 =
{
	(void*)s_vec_tb_noprim_dvim1366x768p_60hz_bvb_input_bss,
	(void*)&s_stDvoRmTbl0,
	(void*)&s_stDvoRmTbl1
};

/*
    DTRAM values for Custom 1366x768p

    h_total = h_valid + hsync_width + h_frontporch + h_backporch
            = 1366 + 12 + 56 + 106
            = 1540

    v_total = v_valid + vsync_width + v_frontporch + v_backporch
            = 768 + 10 + 8 + 16
            = 802

    ulTopActive = vsync_width + v_backporch + 1
            = 10 + 16 + 1
            = 27

    Pixel clock frequency in Hz:
    pixel_clk_Hz = h_total * v_total * 60 * 1000 / 1001
                 = 1540 * 802 * 60 * 1000 / 1001
                 = 74030769 Hz for 59.94Hz refresh

    line_time    = 1001/(60*802*1000)
                 = 20.802 us

    pixel_clk_Hz = h_total * v_total * 60
                 = 1540 * 802 * 60
                 = 74104800 Hz for 60Hz refresh

    line_time    = 1/(60*802)
                 = 20.781 us
*/
/* Default custom formats: 1366x768p 60Hz and 50Hz; */
/* 59.94/60 Hz */
static const BFMT_VideoInfo s_stFormatInfoCustom2 =
{
	BFMT_VideoFmt_eCustom2,
	1366,
	768,
	1366,
	768,
	1540,
	802,
	27,
	0,
	0,
	0,
	BFMT_VERT_59_94Hz | BFMT_VERT_60Hz,
	6000,
	BFMT_PXL_148_5MHz_DIV_1_001 | BFMT_PXL_148_5MHz,
	false,
	BFMT_AspectRatio_e16_9,
	BFMT_Orientation_e2D,
	7403,
	"BFMT_VideoFmt_eCustom2",
	&s_stCustomInfo0
};


/* Rate manager and uCode for 1080p48Hz panel
 *
 */
#define BTST_P_DTRAM_TABLE_SIZE 64
/* Support different format timings.  Application loadable module */
static const uint32_t g_DtRamBVBInput_CUSTOM_1920x1080p_T2200x1125[BTST_P_DTRAM_TABLE_SIZE] =
{
    0x0064A001, /*  64 */
    0x00650004, /*  65 */
    0x00656001, /*  66 */
    0x0065C003, /*  67 */
    0x00668438, /*  68 */
    0x0065C023, /*  69 */
    0x00662001, /*  70 */
    0x00840000, /*  71 */
    0x00000000, /*  72 */
    0x00000000, /*  73 */
    0x002840C8, /*  74 */
    0x002D402C, /*  75 */
    0x00244024, /*  76 */
    0x00345775, /*  77 */
    0x00000000, /*  78 */
    0x00000000, /*  79 */
    0x002440C8, /*  80 */
    0x0025402C, /*  81 */
    0x00244024, /*  82 */
    0x00345775, /*  83 */
    0x00000000, /*  84 */
    0x00000000, /*  85 */
    0x002440C8, /*  86 */
    0x0021402C, /*  87 */
    0x00204024, /*  88 */
    0x00305775, /*  89 */
    0x00000000, /*  90 */
    0x00000000, /*  91 */
    0x002040C8, /*  92 */
    0x0021402C, /*  93 */
    0x00204024, /*  94 */
    0x00305775, /*  95 */
    0x00000000, /*  96 */
    0x00000000, /*  97 */
    0x002040C8, /*  98 */
    0x0021402C, /*  99 */
    0x00204024, /* 100 */
    0x0030576A, /* 101 */
    0x00000000, /* 102 */
    0x00000000, /* 103 */
    0x002020C8, /* 104 */
    0x0021202C, /* 105 */
    0x00202024, /* 106 */
    0x00323775, /* 107 */
    0x00000000, /* 108 */
    0x00000000, /* 109 */
    0x002020C8, /* 110 */
    0x0021202C, /* 111 */
    0x00202024, /* 112 */
    0x0032376A, /* 113 */
    0x00000000, /* 114 */
    0x00000000, /* 115 */
    0x00000000, /* 116 */
    0x00000000, /* 117 */
    0x00000000, /* 118 */
    0x00000000, /* 119 */
    0x00000000, /* 120 */
    0x00000000, /* 121 */
    0x00000000, /* 122 */
    0x00000000, /* 123 */
    0x00000000, /* 124 */
    0x00000000, /* 125 */
    0x00062608, /* 126 */
    0x005D605D, /* 127 */
};


/*

target = bcm3548

' Pixel Clock = h_total * v_total * refresh_rate
'             = 2200 * 1125 * 48
'             = 118800000 Hz
' VCO = 1663200000.00 Hz
' M = 1, N = 61.600000, P1 = 2; P2 = 1
' For RDIV=112, Rate Manager sees 7425000.00 Hz
' Sample_Inc = 3, NUM/DEN = 7/11
'
DVPO_RM_0.RATE_RATIO.DENOMINATOR = 11
DVPO_RM_0.SAMPLE_INC.NUMERATOR = 7
DVPO_RM_0.SAMPLE_INC.SAMPLE_INC = 3
DVPO_RM_0.OFFSET = &H0F666666&
DVPO_RM_0.FORMAT.SHIFT = 2
' LINKDIV_CTRL = 0: odd/even mode, link clock = 1/2 pixel clock (for dual-link LVDS)
' LINKDIV_CTRL = 1: normal mode, link clock = pixel clock (for single-link LVDS)
LVDS_PHY_0.LVDS_PLL_CFG.LINKDIV_CTRL = 0
LVDS_PHY_0.LVDS_PLL_CFG.PLL_R_DIV = 112
LVDS_PHY_0.LVDS_PLL_CFG.PLL_VCO_RANGE = 1
LVDS_PHY_0.LVDS_PLL_CFG.PLL_M_DIV = 1
LVDS_PHY_0.LVDS_PLL_CFG.PLL_FEEDBACK_PRE_DIVIDER = 1 ' (p2)
LVDS_PHY_0.LVDS_PLL_CFG.PLL_INPUT_PRE_DIVIDER = 2    ' (p1)
LVDS_PHY_0.LVDS_PLL_CTL_1 = &h38289860&
LVDS_PHY_0.LVDS_PLL_CTL_2 = &h000A4001&
DVPO_0.FIFO_CTL.MASTER_OR_SLAVE_N = 1

*/


static const BFMT_P_RateInfo s_stDvoRmTbl0_48Hz =
	{BFMT_PXL_148_5MHz, 		  1, 0x0F666666,  112, 3,  7,	11,	1,	0,	1, "118.8"}; /* 118.8 MHz */

static BFMT_CustomFormatInfo s_stCustomInfo_48Hz =
{
	(void*)g_DtRamBVBInput_CUSTOM_1920x1080p_T2200x1125,
	(void*)&s_stDvoRmTbl0_48Hz,
	(void*)&s_stDvoRmTbl0_48Hz,
};


/*
	DTRAM values for Custom 1920x1080p

    h_total = h_valid + hsync_width + h_backporch + h_frontporch
            = 1920 + 44 + 200 + 36
            = 2200

    v_total = v_valid + vsync_width + v_backporch + v_frontporch
            = 1080 + 5 + 36 + 4
            = 1125

    ulTopActive = vsync_width + v_backporch + 1
            = 5 + 36 + 1
            = 42

	Pixel clock frequency in Hz:
	pixel_clk_Hz = h_total * v_total * 48Hz
				 = 2200 * 1125 * 48Hz
				 = 118800000 Hz for 48Hz refresh

	line_time	 = 1/(48*1125)
				 = 18.519 us

*/

static const BFMT_VideoInfo s_stFormatInfoCustom_48Hz =
{
	BFMT_VideoFmt_eCustom2,
	1920,
	1080,
	1920,
	1080,
	2200,
	1125,
	42,
	0,
	0,
	0,
	BFMT_VERT_48Hz,
	4800,
	BFMT_PXL_148_5MHz | BFMT_PXL_148_5MHz_DIV_1_001,
	false,
	BFMT_AspectRatio_e16_9,
	BFMT_Orientation_e2D,
	11880,
	"BFMT_VideoFmt_eCustom2",
	&s_stCustomInfo_48Hz
};

/* Rate manager and uCode for 540p@60 panel
 *
 */
/* Support different format timings.  Application loadable module */

const uint32_t g_DtRamBVBInput_CUSTOM_1920x540p_T2200x562[] =
{
    0x0064A001, /*  64 */
    0x00650001, /*  65 */
    0x00656001, /*  66 */
    0x0065C011, /*  67 */
    0x0066821C, /*  68 */
    0x0065C001, /*  69 */
    0x00662001, /*  70 */
    0x00840000, /*  71 */
    0x00000000, /*  72 */
    0x00000000, /*  73 */
    0x00284024, /*  74 */
    0x002D402C, /*  75 */
    0x002440C8, /*  76 */
    0x00345775, /*  77 */
    0x00000000, /*  78 */
    0x00000000, /*  79 */
    0x00244024, /*  80 */
    0x0025402C, /*  81 */
    0x002440C8, /*  82 */
    0x00345775, /*  83 */
    0x00000000, /*  84 */
    0x00000000, /*  85 */
    0x00244024, /*  86 */
    0x0021402C, /*  87 */
    0x002040C8, /*  88 */
    0x00305775, /*  89 */
    0x00000000, /*  90 */
    0x00000000, /*  91 */
    0x00204024, /*  92 */
    0x0021402C, /*  93 */
    0x002040C8, /*  94 */
    0x00305775, /*  95 */
    0x00000000, /*  96 */
    0x00000000, /*  97 */
    0x00204024, /*  98 */
    0x0021402C, /*  99 */
    0x002040C8, /* 100 */
    0x0030576A, /* 101 */
    0x00000000, /* 102 */
    0x00000000, /* 103 */
    0x00202024, /* 104 */
    0x0021202C, /* 105 */
    0x002020C8, /* 106 */
    0x00323775, /* 107 */
    0x00000000, /* 108 */
    0x00000000, /* 109 */
    0x00202024, /* 110 */
    0x0021202C, /* 111 */
    0x002020C8, /* 112 */
    0x0032376A, /* 113 */
    0x00000000, /* 114 */
    0x00000000, /* 115 */
    0x00000000, /* 116 */
    0x00000000, /* 117 */
    0x00000000, /* 118 */
    0x00000000, /* 119 */
    0x00000000, /* 120 */
    0x00000000, /* 121 */
    0x00000000, /* 122 */
    0x00000000, /* 123 */
    0x00000000, /* 124 */
    0x00000000, /* 125 */
    0x00061410, /* 126 */
    0x005D5E2A, /* 127 */
};


/*

target = bcm3548

' Pixel Clock = h_total * v_total * refresh_rate
'             = 2200 * 562 * 60
'             = 74184000 Hz
' VCO = 2077152000.00 Hz
' M = 2, N = 76.931556, P1 = 2; P2 = 1
' For RDIV=112, Rate Manager sees 9273000.00 Hz
' Sample_Inc = 2, NUM/DEN = 2818/3091
'
DVPO_RM_0.RATE_RATIO.DENOMINATOR = 3091
DVPO_RM_0.SAMPLE_INC.NUMERATOR = 2818
DVPO_RM_0.SAMPLE_INC.SAMPLE_INC = 2
DVPO_RM_0.OFFSET = &H133B9E9B&
DVPO_RM_0.FORMAT.SHIFT = 3
' LINKDIV_CTRL = 0: odd/even mode, link clock = 1/2 pixel clock (for dual-link LVDS)
' LINKDIV_CTRL = 1: normal mode, link clock = pixel clock (for single-link LVDS)
LVDS_PHY_0.LVDS_PLL_CFG.LINKDIV_CTRL = 0
LVDS_PHY_0.LVDS_PLL_CFG.PLL_R_DIV = 112
LVDS_PHY_0.LVDS_PLL_CFG.PLL_VCO_RANGE = 1
LVDS_PHY_0.LVDS_PLL_CFG.PLL_M_DIV = 2
LVDS_PHY_0.LVDS_PLL_CFG.PLL_FEEDBACK_PRE_DIVIDER = 1 ' (p2)
LVDS_PHY_0.LVDS_PLL_CFG.PLL_INPUT_PRE_DIVIDER = 2    ' (p1)
LVDS_PHY_0.LVDS_PLL_CTL_1 = &h3800A7E0&
LVDS_PHY_0.LVDS_PLL_CTL_2 = &h19154001&
DVPO_0.FIFO_CTL.MASTER_OR_SLAVE_N = 1
*/


static const BFMT_P_RateInfo s_stDvoRmTbl0_1920x540p_60Hz =
	{BFMT_PXL_74_25MHz, 		  2, 0x133B9E9B,  112, 2,  2818,	3091,	1,	0,	1, "74.18"}; /* 74.18 MHz */

/*

target = bcm3548

' Pixel Clock = h_total * v_total * refresh_rate
'             = 2200 * 562 * 60 * 1000 / 1001
'             = 74109890 Hz
' VCO = 2075076920.00 Hz
' M = 2, N = 76.854701, P1 = 2; P2 = 1
' For RDIV=112, Rate Manager sees 9263736.25 Hz
' Sample_Inc = 2, NUM/DEN = 257/281
'
DVPO_RM_0.RATE_RATIO.DENOMINATOR = 281
DVPO_RM_0.SAMPLE_INC.NUMERATOR = 257
DVPO_RM_0.SAMPLE_INC.SAMPLE_INC = 2
DVPO_RM_0.OFFSET = &H1336B36A&
DVPO_RM_0.FORMAT.SHIFT = 3
' LINKDIV_CTRL = 0: odd/even mode, link clock = 1/2 pixel clock (for dual-link LVDS)
' LINKDIV_CTRL = 1: normal mode, link clock = pixel clock (for single-link LVDS)
LVDS_PHY_0.LVDS_PLL_CFG.LINKDIV_CTRL = 0
LVDS_PHY_0.LVDS_PLL_CFG.PLL_R_DIV = 112
LVDS_PHY_0.LVDS_PLL_CFG.PLL_VCO_RANGE = 1
LVDS_PHY_0.LVDS_PLL_CFG.PLL_M_DIV = 2
LVDS_PHY_0.LVDS_PLL_CFG.PLL_FEEDBACK_PRE_DIVIDER = 1 ' (p2)
LVDS_PHY_0.LVDS_PLL_CFG.PLL_INPUT_PRE_DIVIDER = 2    ' (p1)
LVDS_PHY_0.LVDS_PLL_CTL_1 = &h3800A7E0&
LVDS_PHY_0.LVDS_PLL_CTL_2 = &h19154001&
DVPO_0.FIFO_CTL.MASTER_OR_SLAVE_N = 1
*/
static const BFMT_P_RateInfo s_stDvoRmTbl1_1920x540p_59Hz =
	{BFMT_PXL_74_25MHz_DIV_1_001, 2, 0x1336B36,  112, 2,  257,	281,	1,	0,	1, "74.11"}; /* 74.11 MHz */

static BFMT_CustomFormatInfo s_stCustomInfo1920x540p_60Hz =
{
	(void*)g_DtRamBVBInput_CUSTOM_1920x540p_T2200x562,
	(void*)&s_stDvoRmTbl0_1920x540p_60Hz,
	(void*)&s_stDvoRmTbl1_1920x540p_59Hz,
};


/*
    DTRAM values for Custom 1920x540p

    h_total = h_valid + hsync_width + h_frontporch + h_backporch
            = 1920 + 44 + 36 + 200
            = 2200

    v_total = v_valid + vsync_width + v_frontporch + v_backporch
            = 540 + 2 + 2 + 18
            = 562

    Pixel clock frequency in Hz:
    pixel_clk_Hz = h_total * v_total * 60 * 1000 / 1001
                 = 2200 * 562 * 60 * 1000 / 1001
                 = 74109890 Hz for 59.94Hz refresh

    line_time    = 1001/(60*562*1000)
                 = 29.686 us

    pixel_clk_Hz = h_total * v_total * 60
                 = 2200 * 562 * 60
                 = 74184000 Hz for 60Hz refresh

    line_time    = 1/(60*562)
                 = 29.656 us

*/

static const BFMT_VideoInfo s_stFormatInfo1920x540p_60Hz =
{
	BFMT_VideoFmt_eCustom2,
	1920,
	540,
	1920,
	540,
	2200,
	562,
	21,
	0,
	0,
	0,
	BFMT_VERT_60Hz | BFMT_VERT_59_94Hz,
	6000,
	BFMT_PXL_74_25MHz | BFMT_PXL_74_25MHz_DIV_1_001,
	false,
	BFMT_AspectRatio_e16_9,
	BFMT_Orientation_e2D,
	7418,
	"BFMT_VideoFmt_eCustom2",
	&s_stCustomInfo1920x540p_60Hz
};

/*

target = bcm3548

' Pixel Clock = h_total * v_total * refresh_rate
'             = 2200 * 562 * 50
'             = 61820000 Hz
' VCO = 1730960000.00 Hz
' M = 2, N = 64.109630, P1 = 2; P2 = 1
' For RDIV=112, Rate Manager sees 7727500.00 Hz
' Sample_Inc = 3, NUM/DEN = 1527/3091
'
DVPO_RM_0.RATE_RATIO.DENOMINATOR = 3091
DVPO_RM_0.SAMPLE_INC.NUMERATOR = 1527
DVPO_RM_0.SAMPLE_INC.SAMPLE_INC = 3
DVPO_RM_0.OFFSET = &H1007042B&
DVPO_RM_0.FORMAT.SHIFT = 3
' LINKDIV_CTRL = 0: odd/even mode, link clock = 1/2 pixel clock (for dual-link LVDS)
' LINKDIV_CTRL = 1: normal mode, link clock = pixel clock (for single-link LVDS)
LVDS_PHY_0.LVDS_PLL_CFG.LINKDIV_CTRL = 0
LVDS_PHY_0.LVDS_PLL_CFG.PLL_R_DIV = 112
LVDS_PHY_0.LVDS_PLL_CFG.PLL_VCO_RANGE = 1
LVDS_PHY_0.LVDS_PLL_CFG.PLL_M_DIV = 2
LVDS_PHY_0.LVDS_PLL_CFG.PLL_FEEDBACK_PRE_DIVIDER = 1 ' (p2)
LVDS_PHY_0.LVDS_PLL_CFG.PLL_INPUT_PRE_DIVIDER = 2    ' (p1)
LVDS_PHY_0.LVDS_PLL_CTL_1 = &h3800A7E0&
LVDS_PHY_0.LVDS_PLL_CTL_2 = &h19154001&
DVPO_0.FIFO_CTL.MASTER_OR_SLAVE_N = 1
*/
static const BFMT_P_RateInfo s_stDvoRmTbl1_1920x540p_50Hz =
	{BFMT_PXL_74_25MHz, 2, 0x1007042B,  112, 3,  1527,	3091,	1,	0,	1, "61.82"}; /* 61.82 MHz */

static BFMT_CustomFormatInfo s_stCustomInfo1920x540p_50Hz =
{
	(void*)g_DtRamBVBInput_CUSTOM_1920x540p_T2200x562,
	(void*)&s_stDvoRmTbl1_1920x540p_50Hz,
	(void*)&s_stDvoRmTbl1_1920x540p_50Hz,
};

static const BFMT_VideoInfo s_stFormatInfo1920x540p_50Hz =
{
	BFMT_VideoFmt_eCustom2,
	1920,
	540,
	1920,
	540,
	2200,
	562,
	21,
	0,
	0,
	0,
	BFMT_VERT_50Hz,
	5000,
	BFMT_PXL_60_375MHz,
	false,
	BFMT_AspectRatio_e16_9,
	BFMT_Orientation_e2D,
	6182,
	"BFMT_VideoFmt_eCustom2",
	&s_stCustomInfo1920x540p_50Hz
};




/***************************************************************************
 *
 */
void BTST_P_PrintRfmStatus
	( BTST_P_Context          *pContext )
{
#if (BTST_P_SUPPORT_RFM_OUTPUT)
	BRFM_Status  stStatus;

	if (!pContext->hRfm)
	{
		goto done;
	}

	BRFM_GetStatus( pContext->hRfm, &stStatus );
	BDBG_MSG(("Output channel     = %d", stStatus.outputChannel+2));
	BDBG_MSG(("Mute               = %d", stStatus.isMuteEnable));
	BDBG_MSG(("RFM status signals = %08x", stStatus.rfmStatusSignals));

done:
#else
	BSTD_UNUSED(pContext);
#endif
	return;
}

/***************************************************************************
 *
 */
void BTST_P_ChgDispFmt_Interactive
	( BTST_P_Context          *pContext )
{
	BTST_P_ChgDispFmt (pContext);

	if(pContext->bApplyImmediately)
	{
		BVDC_Display_ItUcodeInfo dInfo;
		BVDC_Display_Handle hDisplay  =
			pContext->ahDisplay[pContext->iSelectedComp];

		/* Give new microcode time to load (next VEC trigger) */
		BKNI_Sleep (5);

		BVDC_Display_GetItUcodeInfo (hDisplay, &dInfo);
		printf (
			"\nExecuting VEC IT microcode:\n"
			"Analog	%08x %08x\n"
			"656	%08x %08x\n"
			"DVI	%08x %08x\n",
			dInfo.ulAnalogTimestamp, dInfo.ulAnalogChecksum,
			dInfo.ulI656Timestamp, dInfo.ulI656Checksum,
			dInfo.ulDviTimestamp, dInfo.ulDviChecksum);
	}
}

/***************************************************************************
 *
 */
void BTST_P_ChgDispFmt
	( BTST_P_Context          *pContext )
{
	BFMT_VideoFmt eFmt;
	const BFMT_VideoInfo *pFmtInfo;
	BVDC_Display_Handle hDisplay  = pContext->ahDisplay[pContext->iSelectedComp];
	BVDC_Window_Handle hWindow = pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].hWindow;
	BTST_MDEC_ChannelHandle hMpgChn;
	int i;
	BVDC_Window_Status stWinStatus;
	BVDC_DisplayTg   eMasterTg         = pContext->eMasterTg[pContext->iSelectedComp];
	uint32_t                   ulHSize, ulVSize;
	bool                       bDTg;
	uint32_t                   ulFmtVert;
	BVDC_DacOutput eDacOutput;
	static BFMT_VideoInfo s_stFormatInfoCustom =
	{
		BFMT_VideoFmt_eCustom2,
		352,
		288,
		352,
		288,
		352,
		288,
		0,
		0,
		0,
		0,
		BFMT_VERT_50Hz,
		5000,
		BFMT_PXL_27MHz,
		false,
		BFMT_AspectRatio_e4_3,
		BFMT_Orientation_e2D,
		2700,
		"BFMT_VideoFmt_eCustom2",
		NULL
	};

	if(!hDisplay)
	{
		goto done;
	}

	if(!pContext->peNewFormat)
	{
		BVDC_Display_GetVideoFormat( hDisplay, &eFmt );
		printf("Enter new format \
			\n\t480i(0), 480p(1), 720p(2), 1080i(3), \
			\n\tNTSC_J(4), PAL_G(5), PALI(6), PAL_M(7), PAL_N(8), PAL_NC(9), \
			\n\t576p_50Hz(10), 720p_24Hz(11), 720p_25Hz (12), 720p_30Hz (13), \
			\n\t720p_50Hz(14), 1080i_50Hz(15), \
			\n\t1080p_24Hz(16), 1080p_25Hz(17), 1080p_30Hz(18),\
			\n\t1080p_50Hz(19), 1080p_60Hz(20), t1250i_50Hz(21), ");
		printf (" \
			\n\tDVI_640x480p(22), DVI_800x600p(23), DVI_1024x768p(24), \
			\n\tDVI_1280x720p(25), DVI_1280x768p(26), DVI_1366x768p(27),\
			\n\tSECAM_L(28), SECAM_B(29), SECAM_G(30), SECAM_D(31), \
			\n\tSECAM_K(32), SECAM_H(33), custom0(34), custom1(35), \
			\n\tcustom2(36), DVI_640x480p_CVT(37), DVI_1280x800p_60Hz(38), \
			\n\tDVI_1280x1024p_60Hz(39), DVI_1360x768p_60Hz(40), \
			\n\tDVI_1366x768p_60Hz(41), DVI_1440x900p(42), 1920x2160i_48Hz(43),\
			\n\t720p60_3D(44), 720p50_3D(45), 1920x2160i_60Hz(46), ");
		printf (" \
			\n\t1080p24_3D(47), 1080p30_3D(48), custom(99): ");

		BTST_P_Scanf(pContext, "%d", &i);

		ulFmtVert = BXVD_MONITOR_REFRESH_RATE_59_94Hz;


		if     ( 0 == i){eFmt = BFMT_VideoFmt_eNTSC  ;}
		else if( 1 == i){eFmt = BFMT_VideoFmt_e480p  ;}
		else if( 2 == i){eFmt = BFMT_VideoFmt_e720p  ;}
		else if( 3 == i){eFmt = BFMT_VideoFmt_e1080i ;}
		else if( 4 == i){eFmt = BFMT_VideoFmt_eNTSC_J;}
		else if( 5 == i){eFmt = BFMT_VideoFmt_ePAL_G ;                ulFmtVert = BXVD_MONITOR_REFRESH_RATE_50Hz;}
		else if( 6 == i){eFmt = BFMT_VideoFmt_ePAL_I ;                ulFmtVert = BXVD_MONITOR_REFRESH_RATE_50Hz;}
		else if( 7 == i){eFmt = BFMT_VideoFmt_ePAL_M;                 ulFmtVert = BXVD_MONITOR_REFRESH_RATE_60Hz;}
		else if( 8 == i){eFmt = BFMT_VideoFmt_ePAL_N;                 ulFmtVert = BXVD_MONITOR_REFRESH_RATE_50Hz;}
		else if( 9 == i){eFmt = BFMT_VideoFmt_ePAL_NC;                ulFmtVert = BXVD_MONITOR_REFRESH_RATE_50Hz;}
		else if(10 == i){eFmt = BFMT_VideoFmt_e576p_50Hz;             ulFmtVert = BXVD_MONITOR_REFRESH_RATE_50Hz;}
		else if(11 == i){eFmt = BFMT_VideoFmt_e720p_24Hz;             ulFmtVert = BXVD_MONITOR_REFRESH_RATE_24Hz;}
		else if(12 == i){eFmt = BFMT_VideoFmt_e720p_25Hz;             ulFmtVert = BXVD_MONITOR_REFRESH_RATE_25Hz;}
		else if(13 == i){eFmt = BFMT_VideoFmt_e720p_30Hz;             ulFmtVert = BXVD_MONITOR_REFRESH_RATE_30Hz;}
		else if(14 == i){eFmt = BFMT_VideoFmt_e720p_50Hz;             ulFmtVert = BXVD_MONITOR_REFRESH_RATE_50Hz;}
		else if(15 == i){eFmt = BFMT_VideoFmt_e1080i_50Hz;            ulFmtVert = BXVD_MONITOR_REFRESH_RATE_50Hz;}
		else if(16 == i){eFmt = BFMT_VideoFmt_e1080p_24Hz;            ulFmtVert = BXVD_MONITOR_REFRESH_RATE_24Hz;}
#if BFMT_LEGACY_3DTV_SUPPORT
		else if(43 == i){eFmt = BFMT_VideoFmt_eCUSTOM1920x2160i_48Hz; ulFmtVert = BXVD_MONITOR_REFRESH_RATE_48Hz;}
		else if(46 == i){eFmt = BFMT_VideoFmt_eCUSTOM1920x2160i_60Hz; ulFmtVert = BXVD_MONITOR_REFRESH_RATE_60Hz;}
#endif
        else if(44 == i){eFmt = BFMT_VideoFmt_e720p_60Hz_3DOU_AS;     ulFmtVert = BXVD_MONITOR_REFRESH_RATE_60Hz;}
        else if(45 == i){eFmt = BFMT_VideoFmt_e720p_50Hz_3DOU_AS;     ulFmtVert = BXVD_MONITOR_REFRESH_RATE_50Hz;}
        else if(47 == i){eFmt = BFMT_VideoFmt_e1080p_24Hz_3DOU_AS;    ulFmtVert = BXVD_MONITOR_REFRESH_RATE_24Hz;}
        else if(48 == i){eFmt = BFMT_VideoFmt_e1080p_30Hz_3DOU_AS;    ulFmtVert = BXVD_MONITOR_REFRESH_RATE_30Hz;}
		else if(17 == i){eFmt = BFMT_VideoFmt_e1080p_25Hz;            ulFmtVert = BXVD_MONITOR_REFRESH_RATE_25Hz;}
		else if(18 == i){eFmt = BFMT_VideoFmt_e1080p_30Hz;            ulFmtVert = BXVD_MONITOR_REFRESH_RATE_30Hz;}
		else if(19 == i){eFmt = BFMT_VideoFmt_e1080p_50Hz;}
		else if(20 == i){eFmt = BFMT_VideoFmt_e1080p;}
		else if(21 == i){eFmt = BFMT_VideoFmt_e1250i_50Hz;}
		else if(22 == i){eFmt = BFMT_VideoFmt_eDVI_640x480p;}
		else if(23 == i){eFmt = BFMT_VideoFmt_eDVI_800x600p;}
		else if(24 == i){eFmt = BFMT_VideoFmt_eDVI_1024x768p;}
		else if(25 == i){eFmt = BFMT_VideoFmt_eDVI_1280x720p;}
		else if(26 == i){eFmt = BFMT_VideoFmt_eDVI_1280x768p;}
		else if(27 == i){eFmt = BFMT_VideoFmt_eCUSTOM_1366x768p;}
		else if(28 == i){eFmt = BFMT_VideoFmt_eSECAM_L;}
		else if(29 == i){eFmt = BFMT_VideoFmt_eSECAM_B;}
		else if(30 == i){eFmt = BFMT_VideoFmt_eSECAM_G;}
		else if(31 == i){eFmt = BFMT_VideoFmt_eSECAM_D;}
		else if(32 == i){eFmt = BFMT_VideoFmt_eSECAM_K;}
		else if(33 == i){eFmt = BFMT_VideoFmt_eSECAM_H;}
		else if(34 == i){eFmt = BFMT_VideoFmt_eCustom0;}
		else if(35 == i){eFmt = BFMT_VideoFmt_eCustom1;}
		else if(36 == i){eFmt = BFMT_VideoFmt_eCustom2;}
		else if(37 == i){eFmt = BFMT_VideoFmt_eDVI_640x480p_CVT;}
		else if(38 == i){eFmt = BFMT_VideoFmt_eDVI_1280x800p_60Hz;}
		else if(39 == i){eFmt = BFMT_VideoFmt_eDVI_1280x1024p_60Hz;}
		else if(40 == i){eFmt = BFMT_VideoFmt_eDVI_1360x768p_60Hz;}
		else if(41 == i){eFmt = BFMT_VideoFmt_eDVI_1366x768p_60Hz;}
		else if(42 == i){eFmt = BFMT_VideoFmt_eDVI_1440x900p_60Hz;}
		else if(99 == i){/* STG custom format */
			int w, h;
			int vFreq = 0;
			uint32_t ulFreq;
			bool bInterlaced, bDiv1001;
			BFMT_AspectRatio eAspR;
			printf("Set the format w and h:\n ");
			BTST_P_Scanf(pContext, "%d %d", &w, &h);
			printf("bInterlaced?: [0=n; 1=y]\n ");
			BTST_P_Scanf(pContext, "%d", &bInterlaced);
			printf("Set the format vsync rate (Hz):\n ");
			printf("\n ( 0) 24Hz ( 1) 25Hz ( 2) 30Hz ( 3) 48Hz ");
			printf("\n ( 4) 50Hz ( 5) 60Hz \n");
			BTST_P_Scanf(pContext, "%d ", &ulFreq);
			printf("\n ulFreq = %d", ulFreq);

			if((ulFreq == 0) || (ulFreq == 2) ||(ulFreq == 5))
			{
				printf("\nSet the frame rate divided by 1.001 (Hz):\n ");
				BTST_P_Scanf(pContext, "%d ", &bDiv1001);
			}
			else
				bDiv1001 = false;

			switch(ulFreq)
			{
				case (0):
				{
					vFreq = 24;
					ulFmtVert = bDiv1001?BXVD_MONITOR_REFRESH_RATE_23_976Hz: BXVD_MONITOR_REFRESH_RATE_24Hz;
					break;
				}
				case (1):
				{
					vFreq = 25;
					ulFmtVert = BXVD_MONITOR_REFRESH_RATE_25Hz;
					break;
				}
				case (2):
				{
					vFreq = 30;
					ulFmtVert = bDiv1001?BXVD_MONITOR_REFRESH_RATE_29_97Hz: BXVD_MONITOR_REFRESH_RATE_30Hz;
					break;
				}
				case (3):
				{
					vFreq = 48;
					ulFmtVert = BXVD_MONITOR_REFRESH_RATE_48Hz;
					break;
				}
				case (4):
				{
					vFreq = 50;
					ulFmtVert = BXVD_MONITOR_REFRESH_RATE_50Hz;
					break;
				}
				case (5):
				default:
				{
					vFreq = 60;
					ulFmtVert = bDiv1001?BXVD_MONITOR_REFRESH_RATE_59_94Hz: BXVD_MONITOR_REFRESH_RATE_60Hz;
					break;
				}
			}

			printf("Set the format aspect ratio: 0) Unknown; 1) Square Pixel; 2) 4:3;\n ");
			printf("3) 16:9; 4) 2.21:1; 5) 15:9; 6) SAR;\n");
			BTST_P_Scanf(pContext, "%d", &eAspR);
			s_stFormatInfoCustom.bInterlaced        = bInterlaced;
			s_stFormatInfoCustom.ulDigitalWidth     = w;
			s_stFormatInfoCustom.ulDigitalHeight    = h;
			s_stFormatInfoCustom.ulWidth            = w;
			s_stFormatInfoCustom.ulHeight           = h;
			s_stFormatInfoCustom.ulScanWidth        = w;
			s_stFormatInfoCustom.ulScanHeight       = h;
			s_stFormatInfoCustom.ulVertFreq         = (uint32_t)(vFreq * BFMT_FREQ_FACTOR / ((bDiv1001)? 1.001 : 1));
			s_stFormatInfoCustom.ulPxlFreq          = w*h*vFreq / (1000000/BFMT_FREQ_FACTOR);
			s_stFormatInfoCustom.ulPxlFreqMask      = BFMT_PXL_27MHz |BFMT_PXL_27MHz_MUL_1_001;
			s_stFormatInfoCustom.eAspectRatio       = (eAspR > BFMT_AspectRatio_eSAR)? BFMT_AspectRatio_eSquarePxl : eAspR;

			eFmt = BFMT_VideoFmt_eCustom2;
		}
		else return;

		/* only set refresh rate for xvd with sync lock window */
		for(i = 0; i < BTST_P_MAX_WINDOW; i++)
		{
			hWindow = pContext->aWinContext[pContext->iSelectedComp][i].hWindow;
			if(hWindow)
			{
				BVDC_Window_GetStatus(hWindow, &stWinStatus);
				if(stWinStatus.bSyncLock == true)
				{
					BTST_MDEC_Id eSelectedMDec = pContext->aulSelectedMDec[pContext->iSelectedComp][i];
					uint32_t iSelectedMDecCh = pContext->aulSelectedMDecCh[pContext->iSelectedComp][i];
					hMpgChn = pContext->astMDecInfo[eSelectedMDec].ahMDecChn[iSelectedMDecCh];
					if (IS_XVD(eSelectedMDec) && hMpgChn)
					{
#if (BFramework_NUM_XVD_CORES > 0) && !BTST_VDC_ONLY
						BXVD_SetMonitorRefreshRate((BXVD_ChannelHandle)hMpgChn, ulFmtVert);
#endif
					}
				}
			}
		}
	}
	else
	{
		eFmt = *(pContext->peNewFormat);
	}

	hWindow = pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].hWindow;
	/* This is to address PR36606 (multiburst test frequency response issue)
	 */
	if (((eFmt == BFMT_VideoFmt_e480p) || (eFmt == BFMT_VideoFmt_eNTSC)) && hWindow)
	{
		BVDC_Window_SetScaleFactorRounding(hWindow, 3, 3);
	}

	if(eFmt == BFMT_VideoFmt_eCustom2)
	{
		BVDC_Display_SetCustomVideoFormat(hDisplay, &s_stFormatInfoCustom);
		pContext->apFmtInfo[pContext->iSelectedComp] = pFmtInfo =
			&s_stFormatInfoCustom;
	}
	else
	{
		BVDC_Display_SetVideoFormat(hDisplay, eFmt);
		pContext->apFmtInfo[pContext->iSelectedComp] = pFmtInfo =
			BFMT_GetVideoFormatInfoPtr(eFmt);
	}

#if (BTST_P_SUPPORT_HDM)
	/* Update for the Hdmi if enabled. */
	if((pContext->hHdm) &&
	   (BAVC_MatrixCoefficients_eUnknown != pContext->stHdmiSettings.eColorimetry))
	{
		BAVC_MatrixCoefficients eColorPrimaries;
		BVDC_Display_GetHdmiConfiguration(hDisplay, BVDC_Hdmi_0, &eColorPrimaries);
		if(eColorPrimaries != BAVC_MatrixCoefficients_eUnknown)
		{
			pContext->stHdmiSettings.eInputVideoFmt = eFmt;
			pContext->stHdmiSettings.eOutputPort =
				(eFmt == BFMT_VideoFmt_eCUSTOM_1366x768p) ?
				BHDM_OutputPort_eDVI : BHDM_OutputPort_eHDMI;
			pContext->stHdmiSettings.eColorimetry = BFMT_IS_VESA_MODE(eFmt) ?
				BAVC_MatrixCoefficients_eDvi_Full_Range_RGB :
				((pContext->stHdmiSettings.eColorimetry == BAVC_MatrixCoefficients_eItu_R_BT_709)
				? BAVC_MatrixCoefficients_eItu_R_BT_709 : BAVC_MatrixCoefficients_eHdmi_RGB);

			BVDC_Display_SetHdmiConfiguration(hDisplay, BVDC_Hdmi_0,
				pContext->stHdmiSettings.eColorimetry);

			if(BFMT_IS_3D_MODE(eFmt))
			{
				BHDM_GetVendorSpecificInfoFrame(pContext->hHdm, &pContext->stHdmiSettings.stVendorSpecificInfoFrame);
				pContext->stHdmiSettings.stVendorSpecificInfoFrame.eHdmiVideoFormat = BAVC_HDMI_VSInfoFrame_HDMIVideoFormat_e3DFormat;
				pContext->stHdmiSettings.stVendorSpecificInfoFrame.e3DStructure = BAVC_HDMI_VSInfoFrame_3DStructure_eFramePacking;
			}
			BHDM_EnableDisplay(pContext->hHdm, &pContext->stHdmiSettings);
		}
	}
#endif

	/* digital master TG sets digital format size */
	bDTg      =  BTST_P_USED_DIGTRIG (eMasterTg);

	ulHSize = bDTg? pFmtInfo->ulDigitalWidth :pFmtInfo->ulWidth;
	ulVSize = bDTg? pFmtInfo->ulDigitalHeight:pFmtInfo->ulHeight;

	if(!BFMT_IS_3D_MODE(pFmtInfo->eVideoFmt))
	{
		/* half resolution 3D cases */
		BFMT_Orientation eOrientation;
		BVDC_Display_GetOrientation(hDisplay, &eOrientation);
		if(eOrientation == BFMT_Orientation_e3D_LeftRight)
			ulHSize = ulHSize / 2;
		else if(eOrientation == BFMT_Orientation_e3D_OverUnder)
			ulVSize = ulVSize / 2;
	}

	/* Update Windows to take in new format dimensions. */
	for(i = 0; i < (BTST_P_MAX_WINDOW - 1); i++)
	{
		if(pContext->aWinContext[pContext->iSelectedComp][i].hWindow)
		{
			int iVidSrcId = pContext->aWinContext[pContext->iSelectedComp][i].iVidSrcId;

			/* don't rescale if gfx window */
			if (!(pContext->aSrcContext[iVidSrcId].hGfxSurface))
			{
				BVDC_Window_SetDstRect(pContext->aWinContext[pContext->iSelectedComp][i].hWindow,
					0, 0,
					ulHSize  / (i + 1),
					ulVSize / (i + 1));

				BVDC_Window_SetScalerOutput(pContext->aWinContext[pContext->iSelectedComp][i].hWindow,
					0, 0,
					ulHSize  / (i + 1),
					ulVSize / (i + 1));
			}
		}
	}

	if(!pContext->abMasterStg[pContext->iSelectedComp])
	{
		/* not using DAC0/1/2 */
		if(BVDC_CompositorId_eCompositor0==pContext->iSelectedComp)
		{
			/* format is HD or secondary display in used, don't control DAC0, 1, 2 */
			if(IS_HD(eFmt) || (pContext->ahDisplay[1]))
			{
				BVDC_Display_SetDacConfiguration(hDisplay,
					BFramework_Dac_Composite_0 | BFramework_Dac_Svideo_Chroma |
						BFramework_Dac_Svideo_Luma,
					BVDC_DacOutput_eUnused);
				BVDC_Display_GetDacConfiguration (hDisplay, BFramework_Dac_Component_y, &eDacOutput);
				if(BVDC_DacOutput_eUnused != eDacOutput)
				{
					BVDC_Display_SetDacConfiguration(
						hDisplay, BFramework_Dac_Component_pr, BVDC_DacOutput_ePr);
					BVDC_Display_SetDacConfiguration(
						hDisplay, BFramework_Dac_Component_y, BVDC_DacOutput_eY);
					BVDC_Display_SetDacConfiguration(
						hDisplay, BFramework_Dac_Component_pb, BVDC_DacOutput_ePb);
				}
			}
			else
			{

#if !BTST_P_SUPPORT_ORTHOGONAL_VEC
				if(pContext->stSettings.bVecSwap)
#endif
				{
					BVDC_Display_SetDacConfiguration( hDisplay,
						BFramework_Dac_Composite_0, BVDC_DacOutput_eComposite);
#if (BFramework_Dac_Svideo_Luma != 0)
					BVDC_Display_SetDacConfiguration( hDisplay,
						BFramework_Dac_Svideo_Chroma,
						BVDC_DacOutput_eSVideo_Chroma);
					BVDC_Display_SetDacConfiguration( hDisplay,
						BFramework_Dac_Svideo_Luma,
						BVDC_DacOutput_eSVideo_Luma);
#endif
				}
				BVDC_Display_GetDacConfiguration (hDisplay, BFramework_Dac_Component_y, &eDacOutput);
				if(BVDC_DacOutput_eUnused != eDacOutput)
				{
					BVDC_Display_SetDacConfiguration(
						hDisplay, BFramework_Dac_Component_pr, BVDC_DacOutput_ePr);
					BVDC_Display_SetDacConfiguration(
						hDisplay, BFramework_Dac_Component_y, BVDC_DacOutput_eY);
					BVDC_Display_SetDacConfiguration(
						hDisplay, BFramework_Dac_Component_pb, BVDC_DacOutput_ePb);
				}
			}
		}

		if(BFMT_IS_VESA_MODE(eFmt) ||
		   BFMT_IS_3D_MODE(eFmt)   ||
		   (eFmt == BFMT_VideoFmt_eCUSTOM_1366x768p))
		{
			printf("DVI VESA mode or 3D mode! Will turn off the DAC outputs!\n");
			BVDC_Display_SetDacConfiguration(hDisplay,
				BFramework_Dac_Svideo_Luma  | BFramework_Dac_Svideo_Chroma |
				BFramework_Dac_Composite_0  | BFramework_Dac_Composite_1 |
				BFramework_Dac_Component_y  | BFramework_Dac_Component_pr |
				BFramework_Dac_Component_pb,
				BVDC_DacOutput_eUnused);
		}
	} /* end of if(!abMasterStg[i]) */

	if(pContext->bApplyImmediately)
	{
		if(BERR_SUCCESS != BTST_P_ApplyChanges(pContext))
		{
			printf("Stress format change failed!\n");
			pContext->peNewFormat = NULL;
		}
	}

done:
	return;
}


/***************************************************************************
 *
 */
void BTST_P_RgbOutput
	( BTST_P_Context          *pContext )
{
	int bSync;
	BFMT_VideoFmt eFmt;
	BVDC_Display_Handle hDisplay  = pContext->ahDisplay[pContext->iSelectedComp];
	BVDC_Display_Handle hDisplay1 = pContext->ahDisplay[1];
	BVDC_DacOutput eDacPr;

	if(!hDisplay)
	{
		goto done;
	}

	/* not using DAC0/1/2 */
	BVDC_Display_GetVideoFormat(hDisplay, &eFmt);

	BVDC_Display_GetDacConfiguration(
		hDisplay, BFramework_Dac_Component_pr, &eDacPr);

	/* format is HD or secondary display in used, don't control DAC0, 1, 2 */
	if(IS_HD(eFmt) || (hDisplay1))
	{
		BVDC_Display_SetDacConfiguration(hDisplay,
			BFramework_Dac_Svideo_Luma | BFramework_Dac_Svideo_Chroma |
				BFramework_Dac_Composite_0,
			BVDC_DacOutput_eUnused);
	}

	if(eDacPr == BVDC_DacOutput_ePr)
	{
		printf("Do you want G channel to carry SYNC pulses?[1=y/0=n] ");
		BTST_P_Scanf(pContext, "%d", &bSync);
		BVDC_Display_SetDacConfiguration(
			hDisplay, BFramework_Dac_Component_pr, BVDC_DacOutput_eRed);
		BVDC_Display_SetDacConfiguration(
			hDisplay, BFramework_Dac_Component_pb, BVDC_DacOutput_eBlue);
		BVDC_Display_SetDacConfiguration(
			hDisplay, BFramework_Dac_Component_y, bSync?
			BVDC_DacOutput_eGreen : BVDC_DacOutput_eGreen_NoSync);
		printf("setting DAC outputs as RGB.\n");
	}
	else
	{
		BVDC_Display_SetDacConfiguration(
			hDisplay, BFramework_Dac_Component_pr, BVDC_DacOutput_ePr);
		BVDC_Display_SetDacConfiguration(
			hDisplay, BFramework_Dac_Component_y, BVDC_DacOutput_eY);
		BVDC_Display_SetDacConfiguration(
			hDisplay, BFramework_Dac_Component_pb, BVDC_DacOutput_ePb);
		printf("setting DAC outputs as YPrPb.\n");
	}

	if(pContext->bApplyImmediately)
	{
		BTST_P_ApplyChanges(pContext);
	}

done:
	return;
}


/***************************************************************************
 *
 */
void BTST_P_CvbsOutput
	( BTST_P_Context          *pContext )
{
	BFMT_VideoFmt eFmt;
	BVDC_Display_Handle hDisplay  = pContext->ahDisplay[pContext->iSelectedComp];
	BVDC_DacOutput eDac0;

	if(!hDisplay)
	{
		goto done;
	}

	BVDC_Display_GetVideoFormat(hDisplay, &eFmt);
	BVDC_Display_GetDacConfiguration(
		hDisplay, BFramework_Dac_Component_pr, &eDac0);


	if(!IS_HD(eFmt) && (eDac0 == BVDC_DacOutput_ePr))
	{
		BVDC_Display_SetDacConfiguration(
			hDisplay, BFramework_Dac_Component_pr, BVDC_DacOutput_eComposite);
#if (BFramework_Dac_Svideo_Luma != 0)
		BVDC_Display_SetDacConfiguration(
			hDisplay, BFramework_Dac_Component_y,
			BVDC_DacOutput_eSVideo_Luma);
		BVDC_Display_SetDacConfiguration(
			hDisplay, BFramework_Dac_Component_pb,
			BVDC_DacOutput_eSVideo_Chroma);
#endif
		printf("setting DAC outputs as Svideo-CVBS.\n");
	}
	else
	{
		BVDC_Display_SetDacConfiguration(
			hDisplay, BFramework_Dac_Component_pr, BVDC_DacOutput_ePr);
		BVDC_Display_SetDacConfiguration(
			hDisplay, BFramework_Dac_Component_y, BVDC_DacOutput_eY);
		BVDC_Display_SetDacConfiguration(
			hDisplay, BFramework_Dac_Component_pb, BVDC_DacOutput_ePb);
		printf("setting DAC outputs as YPrPb.\n");
	}

	if(pContext->bApplyImmediately)
	{
		BTST_P_ApplyChanges(pContext);
	}

done:
	return;
}


/***************************************************************************
 *
 */
void BTST_P_ToggleHsync
	( BTST_P_Context          *pContext )
{
	BFMT_VideoFmt eFmt;
	BVDC_Display_Handle hDisplay  = pContext->ahDisplay[pContext->iSelectedComp];
	BVDC_Display_Handle hDisplay1 = pContext->ahDisplay[1];
	BVDC_DacOutput eDacHsync;
	BVDC_DacOutput eDacChroma, eDacComp;

	if(!hDisplay || (hDisplay == hDisplay1))
	{
		printf("Invalid sync setting!\n");
		goto done;
	}

	BVDC_Display_GetDacConfiguration(
		hDisplay, BFramework_Dac_Svideo_Luma, &eDacHsync);
	BVDC_Display_GetVideoFormat(hDisplay, &eFmt);

	if(eDacHsync != BVDC_DacOutput_eHsync)
	{
		printf("Enable external sync.\n");
		BVDC_Display_SetDacConfiguration(
			hDisplay, BFramework_Dac_Svideo_Luma, BVDC_DacOutput_eHsync);
		BVDC_Display_SetDacConfiguration(hDisplay,
			BFramework_Dac_Svideo_Chroma | BFramework_Dac_Composite_0,
			BVDC_DacOutput_eUnused);
		if(hDisplay1)
		{
			BVDC_Display_SetDacConfiguration(
				hDisplay1, BFramework_Dac_Svideo_Luma, BVDC_DacOutput_eUnused);
		}
	}
	else
	{
		printf("Disable external sync.\n");
		if(IS_HD(eFmt))
		{
			BVDC_Display_SetDacConfiguration(hDisplay,
			BFramework_Dac_Svideo_Luma | BFramework_Dac_Svideo_Chroma |
				BFramework_Dac_Composite_0,
			BVDC_DacOutput_eUnused);
		}
		else
		{
			if(hDisplay1)
			{
				BVDC_Display_GetDacConfiguration(
					hDisplay1, BFramework_Dac_Svideo_Chroma, &eDacChroma);
				BVDC_Display_GetDacConfiguration(
					hDisplay1, BFramework_Dac_Composite_0, &eDacComp);
				if(eDacChroma == BVDC_DacOutput_eUnused)
				{/* if display 1 was not using DAC2, take Svideo back; otherwise return it */
					BVDC_Display_SetDacConfiguration( hDisplay,
						BFramework_Dac_Svideo_Luma,
						BVDC_DacOutput_eSVideo_Luma);
					BVDC_Display_SetDacConfiguration( hDisplay,
						BFramework_Dac_Svideo_Chroma,
						BVDC_DacOutput_eSVideo_Chroma);
				}
				else
				{
					BVDC_Display_SetDacConfiguration( hDisplay,
						BFramework_Dac_Svideo_Luma, BVDC_DacOutput_eUnused);
					BVDC_Display_SetDacConfiguration( hDisplay1,
						BFramework_Dac_Svideo_Luma,
						BVDC_DacOutput_eSVideo_Luma);
				}

				if(eDacComp == BVDC_DacOutput_eUnused)
				{/* if display 1 was not using DAC0, then take it back */
					BVDC_Display_SetDacConfiguration(hDisplay,
						BFramework_Dac_Composite_0, BVDC_DacOutput_eComposite);
				}
			}
			else
			{
				BVDC_Display_SetDacConfiguration( hDisplay,
					BFramework_Dac_Composite_0,
					BVDC_DacOutput_eComposite);
				BVDC_Display_SetDacConfiguration( hDisplay,
					BFramework_Dac_Svideo_Chroma,
					BVDC_DacOutput_eSVideo_Chroma);
				BVDC_Display_SetDacConfiguration( hDisplay,
					BFramework_Dac_Svideo_Luma,
					BVDC_DacOutput_eSVideo_Luma);
			}
		}
	}

	if(pContext->bApplyImmediately)
	{
		BTST_P_ApplyChanges(pContext);
	}

done:
	return;
}

/***************************************************************************
 *
 */
void BTST_P_ChgDispAlign
	( BTST_P_Context          *pContext )
{
	int iSelect;
	BVDC_Display_AlignmentSettings stAlignCfg;
	BVDC_Display_Handle hTargetDisplay;
	BVDC_Display_Handle hDisplay  = pContext->ahDisplay[pContext->iSelectedComp];

	if(!hDisplay)
	{
		goto done;
	}

	BVDC_Display_GetAlignment(hDisplay, &hTargetDisplay, & stAlignCfg);
	printf("Current Display %d: Target 0x%p, Align settings: %d\n",
		pContext->iSelectedComp, (void*)hTargetDisplay, stAlignCfg.eDirection);

	printf("Do you want alignment? [0-N/1-Y]\n");
	BTST_P_Scanf(pContext, "%d", &iSelect);
	if(iSelect)
	{
		printf("Which target display? [0/1/2]\n");
		BTST_P_Scanf(pContext, "%d", &iSelect);
		iSelect = (iSelect>2) ? 2 : iSelect;
		hTargetDisplay = pContext->ahDisplay[iSelect];
		printf("Which align direction? [0-auto; 1-faster; 2-slower]\n");
		BTST_P_Scanf(pContext, "%d", &iSelect);
		iSelect = (iSelect>2) ? 0 : iSelect;
		stAlignCfg.eDirection = iSelect;
		BVDC_Display_SetAlignment(hDisplay, hTargetDisplay, &stAlignCfg);
	}
	else
	{
		BVDC_Display_SetAlignment(hDisplay, hTargetDisplay, NULL);
	}

	if(pContext->bApplyImmediately)
	{
		BTST_P_ApplyChanges(pContext);
	}

done:
	return;
}


/***************************************************************************
 *
 */
void BTST_P_ChgDispArc
	( BTST_P_Context          *pContext )
{
	BFMT_AspectRatio eAspectRatio;
	BAVC_VbiPath eVbiPath;
	BVDC_Display_Handle hDisplay  = pContext->ahDisplay[pContext->iSelectedComp];

	if(!hDisplay)
	{
		goto done;
	}

	BVDC_Display_GetAspectRatio(hDisplay, &eAspectRatio);
	eAspectRatio = (BFMT_AspectRatio_e4_3 == eAspectRatio)
		? BFMT_AspectRatio_e16_9 : BFMT_AspectRatio_e4_3;
	BVDC_Display_SetAspectRatio(hDisplay, eAspectRatio);

	BVDC_Display_GetVbiPath(hDisplay, &eVbiPath);
	printf("Compositor[%d] -> Display[%d]\n", pContext->iSelectedComp, eVbiPath);

	if(pContext->bApplyImmediately)
	{
		BTST_P_ApplyChanges(pContext);
	}

done:
	return;
}

/***************************************************************************
 *
 */
void BTST_P_ChgDispTimebase
	( BTST_P_Context          *pContext )
{
	BAVC_Timebase eTimebase;
	uint32_t i;
	BVDC_Display_Handle hDisplay  = pContext->ahDisplay[pContext->iSelectedComp];

	if(!hDisplay)
	{
		goto done;
	}

	BVDC_Display_GetTimebase(hDisplay, &eTimebase);
	printf("Current Display %d: timebase %d\n",
		pContext->iSelectedComp, eTimebase);

	printf("Which timebase do you want it to use?\n");
	BTST_P_Scanf(pContext, "%u", &i);
	eTimebase = (i>2) ? 2 : i;
	BVDC_Display_SetTimebase(hDisplay, eTimebase);

	if(pContext->bApplyImmediately)
	{
		BTST_P_ApplyChanges(pContext);
	}

done:
	return;
}


/***************************************************************************
 *
 */
void BTST_P_ToggleXvYcc
	( BTST_P_Context          *pContext )
{
	bool bXvycc = false;
	BVDC_Display_Handle hDisplay  = pContext->ahDisplay[pContext->iSelectedComp];

	if(!hDisplay)
	{
		goto done;
	}

	/* not using DAC0/1/2 */
	BVDC_Display_GetHdmiXvYcc(hDisplay, &bXvycc);
	bXvycc = bXvycc ? false : true;
	BVDC_Display_SetHdmiXvYcc(hDisplay,  bXvycc);

	if(pContext->bApplyImmediately)
	{
		BTST_P_ApplyChanges(pContext);
	}

done:
	return;
}


/***************************************************************************
 *
 */
void BTST_P_ToggleSS
	( BTST_P_Context          *pContext )
{
	BVDC_Display_DvoSettings stDvoSettings;
	BVDC_Display_Handle hDisplay  = pContext->ahDisplay[pContext->iSelectedComp];

	if(!hDisplay)
	{
		goto done;
	}

	BVDC_Display_GetDvoConfiguration(hDisplay, &stDvoSettings);
	stDvoSettings.stSpreadSpectrum.bEnable = !stDvoSettings.stSpreadSpectrum.bEnable;
	BVDC_Display_SetDvoConfiguration(hDisplay, &stDvoSettings);

	if(pContext->bApplyImmediately)
	{
		BTST_P_ApplyChanges(pContext);
	}

done:
	return;
}


/***************************************************************************
 *
 */
void BTST_P_ChgBgColor
	( BTST_P_Context          *pContext )
{
	uint32_t ulR, ulG, ulB;
	BVDC_Compositor_Handle hCompositor = pContext->ahCompositor[pContext->iSelectedComp];

	if(!hCompositor)
	{
		goto done;
	}

	printf("Enter new background color (0xRR, 0xGG, 0xBB):");
	BTST_P_Scanf(pContext, "%x %x %x", &ulR, &ulG, &ulB);
	ulR = BTST_P_MIN(ulR, BTST_P_BG_COLOR_RANGE_MAX);
	ulG = BTST_P_MIN(ulG, BTST_P_BG_COLOR_RANGE_MAX);
	ulB = BTST_P_MIN(ulB, BTST_P_BG_COLOR_RANGE_MAX);
	BVDC_Compositor_SetBackgroundColor(hCompositor, ulR, ulG, ulB);

	if(pContext->bApplyImmediately)
	{
		BTST_P_ApplyChanges(pContext);
	}

done:
	return;
}


/***************************************************************************
 *
 */
void BTST_P_Toggle656Out
	( BTST_P_Context          *pContext )
{
	bool b656Out;
	BAVC_Timebase eTimeBase;
	int iChoice = 0;
	BVDC_Display_Handle hDisplay = pContext->ahDisplay[pContext->iSelectedComp];

	if(!hDisplay)
	{
		goto done;
	}

	BVDC_Display_Get656Configuration(hDisplay, BVDC_Itur656Output_0, &b656Out);
	BVDC_Display_Set656Configuration(hDisplay, BVDC_Itur656Output_0, !b656Out);
	printf("656 is %s.\n", (!b656Out) ? "enabled" : "disabled");

	if(pContext->iSelectedComp < 2)
	{
		BVDC_Display_GetTimebase(hDisplay, &eTimeBase);
	}
	else
	{
		/* bypass display uses timebase2 according to the baseline clock usage mode; */
		eTimeBase = pContext->iSelectedComp;
	}

	printf("Which VCXO do you want 656 output clock to be locked?\n");
	BTST_P_Scanf(pContext, "%d", &iChoice);
	if(iChoice > 1) iChoice = 1;

	if(pContext->bApplyImmediately)
	{
		BTST_P_ApplyChanges(pContext);
	}

done:
	return;
}


/***************************************************************************
 *
 */
void BTST_P_ToggleRfmOut
	( BTST_P_Context          *pContext )
{
	BVDC_RfmOutput eRfmOut;
	uint32_t ulRfmConstVal;
	BVDC_Display_Handle hDisplay = pContext->ahDisplay[pContext->iSelectedComp];

	if(!hDisplay)
	{
		goto done;
	}

	/* To verify PR36206. Output RFM doesn't need to set composite DAC. */
	/* BVDC_Display_SetDacConfiguration(hDisplay, BRCM_DAC_COMPOSITE_0, BVDC_DacOutput_eUnused); */

	if(pContext->bApplyImmediately)
	{
		BTST_P_OpenRfm(pContext);
		BTST_P_PrintRfmStatus(pContext);
	}

	BVDC_Display_GetRfmConfiguration(hDisplay, BVDC_Rfm_0, &eRfmOut, &ulRfmConstVal);
	eRfmOut = (BVDC_RfmOutput_eCVBS == eRfmOut)
		? BVDC_RfmOutput_eUnused : BVDC_RfmOutput_eCVBS;
	printf("RFM is %s.\n", (BVDC_RfmOutput_eCVBS == eRfmOut) ? "enabled" : "disabled");
	BVDC_Display_SetRfmConfiguration(hDisplay, BVDC_Rfm_0, eRfmOut, ulRfmConstVal);

	if(pContext->bApplyImmediately)
	{
		BTST_P_ApplyChanges(pContext);
	}

done:
	return;
}

/***************************************************************************
 *
 */
void BTST_P_ToggleStgOut
	( BTST_P_Context          *pContext )
{
	bool bEnable;
	BVDC_Display_StgSettings stStgSettings;
    int iChoice;
	BVDC_Display_Handle hDisplay = pContext->ahDisplay[pContext->iSelectedComp];
	BVDC_Display_Settings *pDispSettings = &pContext->stDispSettings[pContext->iSelectedComp];
	BVDC_DisplayTg      eMasterTg;

	if(!hDisplay)
	{
		goto done;
	}

	BVDC_Display_GetStgConfiguration(hDisplay, &bEnable, &stStgSettings);
	if(!bEnable)
		printf("STG is disabled.\n");
	else
		printf("STG is enabled in %s mode.\n", (stStgSettings.bNonRealTime) ? "non-realtime" : "realtime");
	printf("Do you want to enable STG? [1=y/0=n]" );
	scanf("%d", &iChoice);
	stStgSettings.bNonRealTime = false;
    bEnable = iChoice;

	eMasterTg = pDispSettings->eMasterTg;
	if(bEnable && (BTST_P_USED_STG(eMasterTg)))
	{
		printf("Do you want STG in non-realtime mode? [1=y/0=n]" );
		scanf("%d", &iChoice);
		stStgSettings.bNonRealTime = iChoice;
	}
	BVDC_Display_SetStgConfiguration(hDisplay, bEnable, &stStgSettings);

	if(pContext->bApplyImmediately)
	{
		BTST_P_ApplyChanges(pContext);
	}

done:
	return;
}

#if (!BTST_VDC_ONLY)
   static void BTST_P_RateChangeCb_isr
	   ( void							 *pvhContext,
		 int							  iParam2,
		 void							 *pvVdcData )

  {
	  BVDC_Display_CallbackData *pDisplayCbData = (BVDC_Display_CallbackData *)pvVdcData;
	  BAVC_VdcDisplay_Info *pVdcData = &pDisplayCbData->sDisplayInfo;

	  /* print rate update information provided in pVdcData */
	  if(pDisplayCbData->stMask.bRateChange)
	  {
		 printf("\nRM Setting new rate = 0x%x, refresh rate = 0x%0x\n",
		 	pVdcData->ulPixelClkRate, pVdcData->ulVertRefreshRate);
	  }

	  BSTD_UNUSED(iParam2);
	  BSTD_UNUSED(pvhContext);
	  return;
  }
#endif

  void BTST_P_ToggleRateCb
	  ( BTST_P_Context				*pContext )
  {
#if (!BTST_VDC_ONLY)
	  BVDC_Display_Handle hDisplay = pContext->ahDisplay[pContext->iSelectedComp];
	  bool *pbRateCb =
		  &(pContext->abRateCb[pContext->iSelectedComp]);

	  if(!hDisplay)
	  {
		  return;
	  }

	  if(*pbRateCb)
	  {
		  printf("Disabled rate callback.\n");
		  /*BVDC_Display_UnInstallRateChangeCallback(hDisplay);*/
		  BDBG_SetModuleLevel("BVDC_DISP", BDBG_eWrn);
		  BVDC_Display_InstallCallback(
			  hDisplay,
			  NULL,
			  (void*)pContext,
			  BHDM_Callback_Type_eVideoChange);
	  }
	  else
	  {
		  BVDC_Display_CallbackSettings  stSettings;

		  printf("Enabled STG callback.\n");

		  BVDC_Display_GetCallbackSettings(hDisplay, &stSettings);
		  stSettings.stMask.bRateChange = true;
		  BVDC_Display_SetCallbackSettings(hDisplay, &stSettings);
		  BDBG_SetModuleLevel("BVDC_DISP", BDBG_eMsg);

		  BVDC_Display_InstallCallback(
			  hDisplay,
			  BTST_P_RateChangeCb_isr,
			  (void*)pContext,
			  BHDM_Callback_Type_eVideoChange);
	  }

	  /* Toggle */
	  *pbRateCb = !(*pbRateCb);

	  if(pContext->bApplyImmediately)
	  {
		  BTST_P_ApplyChanges(pContext);
	  }
#else
	BSTD_UNUSED(pContext);
#endif
	  return;
  }
#if (!BTST_VDC_ONLY)
  static void BTST_P_StgPictureIdUpdateCb_isr
	  ( void							*pvhContext,
		int 							 iParam2,
		void							*pvVdcData )

 {
	 BVDC_Display_CallbackData *pDisplayCbData = (BVDC_Display_CallbackData *)pvVdcData;
	 uint32_t ulStgPictureId    = pDisplayCbData->ulStgPictureId;
	 uint32_t ulDecodePictureId = pDisplayCbData->ulDecodePictureId;
	 BTST_P_Context              *pContext = (BTST_P_Context *)pvhContext;

	 /* Call stg picture id update with the information
	  * provided in pVdcData */
	 if(pDisplayCbData->stMask.bStgPictureId)
	 {
		 printf("Setting new stg picture id for stg (%d) = 0x%x, decode picture id = 0x%0x\n",
			  pContext->iSelectedComp, ulStgPictureId, ulDecodePictureId);
	 }

	 BSTD_UNUSED(iParam2);
	 return;
 }
#endif

 void BTST_P_ToggleStgCb
	 ( BTST_P_Context              *pContext )
 {
#if (!BTST_VDC_ONLY)
	 BVDC_Display_Handle hDisplay = pContext->ahDisplay[pContext->iSelectedComp];
	 bool *pbStgCb =
		 &(pContext->abStgCb[pContext->iSelectedComp]);
	 BVDC_Display_Settings *pDispSettings = &pContext->stDispSettings[pContext->iSelectedComp];
	 BVDC_DisplayTg      eMasterTg = pDispSettings->eMasterTg;;

	 if((!hDisplay) || (!BTST_P_USED_STG(eMasterTg)))
	 {
		 return;
	 }

	 if(*pbStgCb)
	 {
		 printf("Disabled STG callback.\n");
		 /*BVDC_Display_UnInstallRateChangeCallback(hDisplay);*/
		if (BTST_P_USED_STG(eMasterTg))
			 BVDC_Display_InstallCallback(
				 hDisplay,
				 NULL,
				 (void*)pContext,
				 BHDM_Callback_Type_eVideoChange);

	 }
	 else
	 {
		 BVDC_Display_CallbackSettings	stSettings;

		 printf("Enabled STG callback.\n");

		 BVDC_Display_GetCallbackSettings(hDisplay, &stSettings);
		 stSettings.stMask.bStgPictureId = true;
		 BVDC_Display_SetCallbackSettings(hDisplay, &stSettings);

		 if (BTST_P_USED_STG(eMasterTg))
			 BVDC_Display_InstallCallback(
				 hDisplay,
				 BTST_P_StgPictureIdUpdateCb_isr,
				 (void*)pContext,
				 BHDM_Callback_Type_eVideoChange);

	 }

	 /* Toggle */
	 *pbStgCb = !(*pbStgCb);

	 if(pContext->bApplyImmediately)
	 {
		 BTST_P_ApplyChanges(pContext);
	 }
	 return;
#else
	BSTD_UNUSED(pContext);
#endif
 }

 /***************************************************************************
  *
  */
#if (BTST_P_SUPPORT_HDM)
 static void BTST_P_HdmiRateChangeCb_isr
	( void                         *pvhHdm,
	  int                           iParm2,
	  void                         *pvVdcData )
{
	BVDC_Display_CallbackData *pDisplayCbData = (BVDC_Display_CallbackData *)pvVdcData;
	BAVC_VdcDisplay_Info *pVdcData = &pDisplayCbData->sDisplayInfo;

	/* Call HDMI and update its rate manager with the information
	 * provided in pVdcData */
	if(pDisplayCbData->stMask.bRateChange)
	{
		printf("Setting new rate for hdmi = 0x%x, refresh rate = 0x%0x\n",
			 pVdcData->ulPixelClkRate, pVdcData->ulVertRefreshRate);

		BHDM_AudioVideoRateChangeCB_isr((void*)pvhHdm, iParm2, pVdcData);
	}
	return;
}
#endif


/***************************************************************************
  *
  */
void BTST_P_HdmiRgbOutput
	( BTST_P_Context          *pContext )
{
#if (BTST_P_SUPPORT_HDM) || (BTST_P_SUPPORT_LVD)
	int i;
	BVDC_Display_Handle hDisplay  = pContext->ahDisplay[pContext->iSelectedComp];
	BAVC_MatrixCoefficients eColorMatrix;

	printf("Which HDMI color space? (0) RGB; (1) BT709; (2)full range RGB: ");
	BTST_P_Scanf(pContext, "%d", &i);
	switch(i)
	{
	case 0:
		eColorMatrix = BAVC_MatrixCoefficients_eHdmi_RGB;
		break;
	case 1:
		eColorMatrix = BAVC_MatrixCoefficients_eItu_R_BT_709;
		break;
	case 2:
	default:
		eColorMatrix = BAVC_MatrixCoefficients_eDvi_Full_Range_RGB;
		break;
	}

#if (!BTST_P_SUPPORT_LVD)
	pContext->stHdmiSettings.eColorimetry = eColorMatrix;
#endif

	BVDC_Display_SetHdmiConfiguration(hDisplay, BVDC_Hdmi_0, eColorMatrix);

	if(pContext->bApplyImmediately)
	{
		BTST_P_ApplyChanges(pContext);
	}
#else
	BSTD_UNUSED(pContext);
#endif
	return;
}


/***************************************************************************
  *
  */
void BTST_P_ToggleFrameDrop
	( BTST_P_Context          *pContext )
{
	bool bFrameDrop;
	BVDC_Display_Handle hDisplay  = pContext->ahDisplay[pContext->iSelectedComp];

	BVDC_Display_GetForceFrameDrop(hDisplay, &bFrameDrop);
	bFrameDrop = bFrameDrop ? false : true;

	BVDC_Display_SetForceFrameDrop(hDisplay, bFrameDrop);

	if(pContext->bApplyImmediately)
	{
		BTST_P_ApplyChanges(pContext);
	}

	return;
}


/***************************************************************************
 *
 */
static void BTST_P_ToggleHdmiOut_priv
	( BTST_P_Context          *pContext,
	  bool                     bApplyImmediately )
{
#if BTST_P_SUPPORT_HDM || BTST_P_SUPPORT_LVD
	BVDC_Display_Handle hDisplay = pContext->ahDisplay[pContext->iSelectedComp];
	BFMT_VideoFmt eFmt;

#if BTST_P_SUPPORT_LVD
	BAVC_MatrixCoefficients eColorMatrix;

	if((!hDisplay) ||
	   ((!pContext->hLvd) &
		(!pContext->bApplyImmediately)))
	{
		goto done;
	}
#else
	if((!hDisplay) ||
	   ((!pContext->hHdm) &
		(!pContext->bApplyImmediately)))
	{
		goto done;
	}
#endif

	BVDC_Display_GetHdmiConfiguration(hDisplay, BVDC_Hdmi_0,
		&pContext->stHdmiSettings.eColorimetry);

	pContext->stHdmiSettings.eColorimetry =
		(pContext->stHdmiSettings.eColorimetry != BAVC_MatrixCoefficients_eUnknown)
		? BAVC_MatrixCoefficients_eUnknown : BAVC_MatrixCoefficients_eHdmi_RGB;

	if(pContext->bApplyImmediately)
	{
		if(pContext->stHdmiSettings.eColorimetry != BAVC_MatrixCoefficients_eUnknown)
		{
			BTST_P_OpenHdmi(pContext);
		}
	}

#if (BTST_P_SUPPORT_LVD)
	BVDC_Display_GetHdmiConfiguration(hDisplay, BVDC_Hdmi_0,
		&eColorMatrix);

	eColorMatrix =
		(eColorMatrix != BAVC_MatrixCoefficients_eUnknown)
		? BAVC_MatrixCoefficients_eUnknown : BAVC_MatrixCoefficients_eHdmi_RGB;

	BVDC_Display_GetVideoFormat( hDisplay, &eFmt );

	if (BERR_SUCCESS != BVDC_Display_SetHdmiConfiguration(hDisplay, BVDC_Hdmi_0, eColorMatrix))
	{
		goto done;
	}
#else
	BVDC_Display_GetVideoFormat( hDisplay, &eFmt );
	pContext->stHdmiSettings.eOutputPort =
		((eFmt == BFMT_VideoFmt_eCUSTOM_1366x768p) ||
		 (eFmt == BFMT_VideoFmt_eCustom0) ||
		 (eFmt == BFMT_VideoFmt_eCustom1)) ?
		BHDM_OutputPort_eDVI : BHDM_OutputPort_eHDMI;

	if(pContext->stHdmiSettings.eColorimetry != BAVC_MatrixCoefficients_eUnknown)
	{
		pContext->stHdmiSettings.eInputVideoFmt = eFmt;
	}

	if (BERR_SUCCESS != BVDC_Display_SetHdmiConfiguration(hDisplay, BVDC_Hdmi_0, pContext->stHdmiSettings.eColorimetry))
	{
		goto done;
	}
#endif

	BTST_P_ApplyChanges(pContext);

#if (BTST_P_SUPPORT_HDM)
	/* Now it's time to enable/disable HDMI display. */
	if(BAVC_MatrixCoefficients_eUnknown != pContext->stHdmiSettings.eColorimetry)
	{
		/* Install Callback to provide automatic notification of Video Rate changes in VDC to HDMI */
		if(!pContext->abHdmiCb[pContext->iSelectedComp])
		{
			BVDC_Display_InstallCallback(hDisplay,
				(BVDC_CallbackFunc_isr) BHDM_AudioVideoRateChangeCB_isr,
				(void*) pContext->hHdm, BHDM_Callback_Type_eVideoChange) ;
			BDBG_MSG(("Video Rate Change Callback installed")) ;
			pContext->abHdmiCb[pContext->iSelectedComp] = true;
		}

		if(BFMT_IS_3D_MODE(eFmt))
		{
			BHDM_GetVendorSpecificInfoFrame(pContext->hHdm, &pContext->stHdmiSettings.stVendorSpecificInfoFrame);
			pContext->stHdmiSettings.stVendorSpecificInfoFrame.eHdmiVideoFormat = BAVC_HDMI_VSInfoFrame_HDMIVideoFormat_e3DFormat;
			pContext->stHdmiSettings.stVendorSpecificInfoFrame.e3DStructure = BAVC_HDMI_VSInfoFrame_3DStructure_eFramePacking;
			BHDM_SetVendorSpecificInfoFrame(pContext->hHdm, &pContext->stHdmiSettings.stVendorSpecificInfoFrame);
		}

		BHDM_EnableDisplay(pContext->hHdm, &pContext->stHdmiSettings);
	}
	else
	{
		/* remove VideoRateChangeCB if installed */
		if(pContext->abHdmiCb[pContext->iSelectedComp])
		{
			BVDC_Display_InstallCallback(hDisplay,
				NULL, (void*) pContext->hHdm, BHDM_Callback_Type_eVideoChange) ;
			pContext->abHdmiCb[pContext->iSelectedComp] = false ;
			/* does not take effect until Apply Changes */
		}
		BHDM_DisableDisplay(pContext->hHdm);
	}

	if(pContext->bApplyImmediately)
	{
		if(pContext->stHdmiSettings.eColorimetry == BAVC_MatrixCoefficients_eUnknown)
		{
			BTST_P_CloseHdmi(pContext);
		}
	}

	if(bApplyImmediately)
	{
		BTST_P_ApplyChanges(pContext);
	}
#else
	BSTD_UNUSED(bApplyImmediately);
#endif
#else
	BDBG_MSG(("HDMI Output not supported.")) ;
	BSTD_UNUSED(bApplyImmediately);
	BSTD_UNUSED(pContext);
	goto done;
#endif

done:
#if (BTST_P_SUPPORT_HDMI)
	if (pContext->stHdmiSettings.eColorimetry == BAVC_MatrixCoefficients_eUnknown)
		pContext->hHdm = NULL;
#endif
	return;
}


/***************************************************************************
 *
 */
void BTST_P_ToggleHdmiOut
	( BTST_P_Context          *pContext)
{

	BVDC_Display_Settings  *pDispSetting = &pContext->stDispSettings[pContext->iSelectedComp];
	bool                   bMasterTgDvi;

	bMasterTgDvi = (pDispSetting->eMasterTg == BVDC_DisplayTg_eDviDtg);

	BTST_P_ToggleHdmiOut_priv (pContext, pContext->bApplyImmediately);
#if BTST_P_SUPPORT_HDM
	if(pContext->hHdm)
		BHDM_SetHdmiDataTransferMode(pContext->hHdm, bMasterTgDvi);
#endif
	return;
}

/***************************************************************************
 *
 */
void BTST_P_ToggleHdmiSync
	( BTST_P_Context          *pContext )
{
#if (BTST_P_SUPPORT_HDM)
	bool bSyncOnly;
	BVDC_Display_Handle hDisplay = pContext->ahDisplay[pContext->iSelectedComp];

	if((!hDisplay) ||
	   (!pContext->hHdm))
	{
		goto done;
	}

	BVDC_Display_GetHdmiSyncOnly(hDisplay, &bSyncOnly);
	BVDC_Display_SetHdmiSyncOnly(hDisplay, !bSyncOnly);
	printf("bSyncOnly = %d\n", !bSyncOnly);

	if(pContext->bApplyImmediately)
	{
		BTST_P_ApplyChanges(pContext);
	}

done:
#else
	BSTD_UNUSED(pContext);
#endif

	return;
}

/***************************************************************************
 *
 */
void BTST_P_ToggleHdmiCb
	( BTST_P_Context          *pContext )
{
#if (BTST_P_SUPPORT_HDM)
	BVDC_Display_Handle hDisplay = pContext->ahDisplay[pContext->iSelectedComp];
	bool *pbHdmiCb =
		&(pContext->abHdmiCb[pContext->iSelectedComp]);

	if((!hDisplay) ||
	   (!pContext->hHdm))
	{
		goto done;
	}

	if(*pbHdmiCb)
	{
		printf("Disabled Hdmi callback.\n");
		/*BVDC_Display_UnInstallRateChangeCallback(hDisplay);*/
		BVDC_Display_InstallCallback(
			hDisplay,
			NULL,
			(void*)pContext->hHdm,
			BHDM_Callback_Type_eVideoChange);
	}
	else
	{
		BVDC_Display_CallbackSettings  stSettings;

		printf("Enabled Hdmi callback.\n");

		BVDC_Display_GetCallbackSettings(hDisplay, &stSettings);
		stSettings.stMask.bRateChange = true;
		BVDC_Display_SetCallbackSettings(hDisplay, &stSettings);

		BVDC_Display_InstallCallback(
			hDisplay,
			BTST_P_HdmiRateChangeCb_isr,
			(void*)pContext->hHdm,
			BHDM_Callback_Type_eVideoChange);
	}

	/* Toggle */
	*pbHdmiCb = !(*pbHdmiCb);

	if(pContext->bApplyImmediately)
	{
		BTST_P_ApplyChanges(pContext);
	}
#else
	BSTD_UNUSED(pContext);
	goto done;
#endif

done:
	return;
}


/* NTSC config 0: 4-line; 1: 2-line; 2: Test01; 3: Test02; */
#define MACROVISION_NTSC_TEST_CONFIGURATIONS    4
/* PAL  config 0: AgcOnly; 1: Test01; 2: Test02; */
#define MACROVISION_PAL_TEST_CONFIGURATIONS     3

/***************************************************************************
 *
 */
void BTST_P_SetMacrovision
	( BTST_P_Context          *pContext )
{
#if MACROVISION_SUPPORT
	int iChoice;
	int iRgb;
	int iMode, iConfig;/* for customized type */
	BFMT_VideoFmt eFormat;
	BVDC_MacrovisionType eType = BVDC_MacrovisionType_eNoProtection;
	BVDC_Display_Handle hDisplay = pContext->ahDisplay[pContext->iSelectedComp];
	BVDC_CpcTable pCpcTbl_Ntsc[MACROVISION_NTSC_TEST_CONFIGURATIONS] = {
		{0x3, 0xe},	{0x3, 0xe}, {0x3, 0xe}, {0x3, 0xe} };
	BVDC_CpsTable pCpsTbl_Ntsc[MACROVISION_NTSC_TEST_CONFIGURATIONS] = {
		/* AGC+4Lines table */
		{0xD, 0x7, 0x2, 0xB, 0x0, 0x9, 0x5, 0xB,
 		0x6, 0x6, 0xD, 0xB, 0x9, 0x0, 0x3, 0xF,
 		0xC, 0x0, 0x0, 0x0, 0x0, 0x0, 0xF, 0x0,
 		0xF, 0x6, 0x0, 0xD, 0x0, 0x2, 0xC, 0xF, 0xF},
		/* AGC+2Lines table */
		{0xD, 0xD, 0x2, 0x3, 0x2, 0x9, 0x1, 0x3,
 		0xC, 0x6, 0xD, 0xB, 0x9, 0x0, 0x3, 0xF,
 		0xC, 0x0, 0x0, 0x0, 0x0, 0x0, 0xF, 0x0,
 		0xF, 0x6, 0x0, 0xD, 0x0, 0x2, 0xC, 0xF, 0xF},
		/* Test01 table */
		{0xD, 0x7, 0x2, 0xB, 0x0, 0xD, 0x5, 0xB,
 		0x7, 0x6, 0x5, 0xC, 0x8, 0xC, 0x7, 0xF,
 		0xC, 0x7, 0xE, 0x0, 0x7, 0x0, 0xF, 0x0,
 		0xE, 0x9, 0x1, 0xA, 0xF, 0x9, 0xA, 0xF, 0x0},
		/* Test02 table */
		{0x2, 0xF, 0x5, 0x4, 0xD, 0x7, 0x6, 0x5,
 		0x3, 0x9, 0x2, 0x5, 0x7, 0x5, 0xB, 0x5,
 		0xC, 0x6, 0xD, 0xC, 0xF, 0x2, 0x3, 0x1,
 		0x3, 0x7, 0x0, 0x1, 0x1, 0xD, 0x5, 0xF, 0x0}
		};
	BVDC_CpcTable pCpcTbl_Pal[MACROVISION_PAL_TEST_CONFIGURATIONS] = {
		{0x3, 0x6},	{0x3, 0xe}, {0x3, 0xe} };
	BVDC_CpsTable pCpsTbl_Pal[MACROVISION_PAL_TEST_CONFIGURATIONS] = {
		/* AGC only table */
		{0x5, 0xA, 0x4, 0x5, 0x5, 0x2, 0x2, 0xA,
 		0x8, 0x7, 0x3, 0xD, 0x5, 0x0, 0x1, 0xF,
 		0xF, 0x0, 0x1, 0x5, 0x4, 0xF, 0xE, 0x7,
 		0xE, 0x6, 0x0, 0x4, 0x0, 0x2, 0x4, 0x5, 0x5},
		/* Test01 table(same as AGC only) */
		{0x5, 0xA, 0x4, 0x5, 0x5, 0x2, 0x2, 0xA,
 		0x8, 0x7, 0x3, 0xD, 0x5, 0x0, 0x1, 0xF,
 		0xF, 0x0, 0x1, 0x5, 0x4, 0xF, 0xE, 0x7,
 		0xE, 0x6, 0x0, 0x4, 0x0, 0x2, 0x4, 0x5, 0x5},
		/* Test02 table */
		{0x5, 0xA, 0x4, 0x5, 0x5, 0x2, 0x2, 0xA,
 		0xB, 0x8, 0xD, 0x2, 0xA, 0xF, 0xC, 0x6,
 		0x3, 0x1, 0xF, 0x4, 0x3, 0x5, 0x3, 0xA,
 		0x3, 0xF, 0x0, 0xB, 0xE, 0x1, 0xA, 0xE, 0x1}
		};

	if(!hDisplay)
	{
		goto done;
	}

	printf("Please select Macrovision type:\n");
	printf("None(0) Type 1_________(1) Type 2_________(2) Type 3_________(3)\n");
	printf("        Type 1 with RGB(4) Type 2 with RGB(5) Type 3 with RGB(6)\n");
	printf("Test01(7) Test02(8) Custom(9)\n");
	BTST_P_Scanf(pContext, "%d", &iChoice);
	switch(iChoice)
	{
	case 0: break;

	case 1: eType = BVDC_MacrovisionType_eAgcOnly; break;
	case 2: eType = BVDC_MacrovisionType_eAgc2Lines; break;
	case 3: eType = BVDC_MacrovisionType_eAgc4Lines; break;
	case 4: eType = BVDC_MacrovisionType_eAgcOnly_Rgb; break;
	case 5: eType = BVDC_MacrovisionType_eAgc2Lines_Rgb; break;
	case 6: eType = BVDC_MacrovisionType_eAgc4Lines_Rgb; break;
	case 7: eType = BVDC_MacrovisionType_eTest01; break;
	case 8: eType = BVDC_MacrovisionType_eTest02; break;
	case 9: eType = BVDC_MacrovisionType_eCustomized;
		BVDC_Display_GetVideoFormat(hDisplay, &eFormat);
		switch(eFormat)
		{
		case BFMT_VideoFmt_eNTSC:
		case BFMT_VideoFmt_eNTSC_J:
		case BFMT_VideoFmt_ePAL_M:
			printf("Please select test configuration: (0) 4-line (1) 2-line (2) Test01 (3) Test02\n");
			BTST_P_Scanf(pContext, "%d", &iConfig);
			if(iConfig > 3) iConfig = 0;
			printf("Please select CPC[1:0] value:\n");
			printf ("(0) 00;  (1) 02;  (2) 03;  (3) 43;  (4) 63;  (5) 80;  (6) 83;  (7) C3;\n");
			printf ("(8) E1;  (9) E2; (10) E3; (11) F3\n");
			BTST_P_Scanf(pContext, "%d", &iMode);
			printf("Do you want to turn on MV at RGB outputs? (0) No; (1) Yes;\n");
			BTST_P_Scanf(pContext, "%d", &iRgb);
			switch(iMode)
			{
			case 0: pCpcTbl_Ntsc[iConfig][1] = 0;   pCpcTbl_Ntsc[iConfig][0] = 0; break;
			case 1: pCpcTbl_Ntsc[iConfig][1] = 0;   pCpcTbl_Ntsc[iConfig][0] = 2; break;
			case 2: pCpcTbl_Ntsc[iConfig][1] = 0;   pCpcTbl_Ntsc[iConfig][0] = 3; break;
			case 3: pCpcTbl_Ntsc[iConfig][1] = 4;   pCpcTbl_Ntsc[iConfig][0] = 3; break;
			case 4: pCpcTbl_Ntsc[iConfig][1] = 6;   pCpcTbl_Ntsc[iConfig][0] = 3; break;
			case 5: pCpcTbl_Ntsc[iConfig][1] = 8;   pCpcTbl_Ntsc[iConfig][0] = 0; break;
			case 6: pCpcTbl_Ntsc[iConfig][1] = 8;   pCpcTbl_Ntsc[iConfig][0] = 3; break;
			case 7: pCpcTbl_Ntsc[iConfig][1] = 0xc; pCpcTbl_Ntsc[iConfig][0] = 3; break;
			case 8: pCpcTbl_Ntsc[iConfig][1] = 0xe; pCpcTbl_Ntsc[iConfig][0] = 1; break;
			case 9: pCpcTbl_Ntsc[iConfig][1] = 0xe; pCpcTbl_Ntsc[iConfig][0] = 2; break;
			case 10: pCpcTbl_Ntsc[iConfig][1] = 0xe; pCpcTbl_Ntsc[iConfig][0] = 3; break;
			case 11: pCpcTbl_Ntsc[iConfig][1] = 0xf; pCpcTbl_Ntsc[iConfig][0] = 3; break;
			default: pCpcTbl_Ntsc[iConfig][1] = 0xe; pCpcTbl_Ntsc[iConfig][0] = 3; break;
			}
			pCpsTbl_Ntsc[iConfig][5] = (pCpsTbl_Ntsc[iConfig][5] & (~4)) | ((iRgb != 0) << 2);
			BVDC_Display_SetMacrovisionTable(hDisplay, pCpcTbl_Ntsc[iConfig], pCpsTbl_Ntsc[iConfig]);
			break;

		case BFMT_VideoFmt_ePAL_B:
		case BFMT_VideoFmt_ePAL_B1:
		case BFMT_VideoFmt_ePAL_D:
		case BFMT_VideoFmt_ePAL_D1:
		case BFMT_VideoFmt_ePAL_G:
		case BFMT_VideoFmt_ePAL_H:
		case BFMT_VideoFmt_ePAL_I:
		case BFMT_VideoFmt_ePAL_K:
		case BFMT_VideoFmt_ePAL_N:
		case BFMT_VideoFmt_ePAL_NC:
		case BFMT_VideoFmt_eSECAM:
			printf("Please select test configuration: (0) AGC-only (1) Test01 (2) Test02\n");
			BTST_P_Scanf(pContext, "%d", &iConfig);
			if(iConfig > 2) iConfig = 0;
			printf("Please select CPC[1:0] value:\n");
			printf ("(0) 00;  (1) 02;  (2) 03;  (3) 43;  (4) 63;  (5) 80;  (6) 83;  (7) C3;\n");
			printf ("(8) E1;  (9) E2; (10) E3; (11) F3\n");
			BTST_P_Scanf(pContext, "%d", &iMode);
			printf("Do you want to turn on MV at RGB outputs? (0) No; (1) Yes;\n");
			BTST_P_Scanf(pContext, "%d", &iRgb);
			switch(iMode)
			{
			case 0: pCpcTbl_Pal[iConfig][1] = 0;   pCpcTbl_Pal[iConfig][0] = 0; break;
			case 1: pCpcTbl_Pal[iConfig][1] = 0;   pCpcTbl_Pal[iConfig][0] = 2; break;
			case 2: pCpcTbl_Pal[iConfig][1] = 0;   pCpcTbl_Pal[iConfig][0] = 3; break;
			case 3: pCpcTbl_Pal[iConfig][1] = 4;   pCpcTbl_Pal[iConfig][0] = 3; break;
			case 4: pCpcTbl_Pal[iConfig][1] = 6;   pCpcTbl_Pal[iConfig][0] = 3; break;
			case 5: pCpcTbl_Pal[iConfig][1] = 8;   pCpcTbl_Pal[iConfig][0] = 0; break;
			case 6: pCpcTbl_Pal[iConfig][1] = 8;   pCpcTbl_Pal[iConfig][0] = 3; break;
			case 7: pCpcTbl_Pal[iConfig][1] = 0xc; pCpcTbl_Pal[iConfig][0] = 3; break;
			case 8: pCpcTbl_Pal[iConfig][1] = 0xe; pCpcTbl_Pal[iConfig][0] = 1; break;
			case 9: pCpcTbl_Pal[iConfig][1] = 0xe; pCpcTbl_Pal[iConfig][0] = 2; break;
			case 10: pCpcTbl_Pal[iConfig][1] = 0xe; pCpcTbl_Pal[iConfig][0] = 3; break;
			case 11: pCpcTbl_Pal[iConfig][1] = 0xf; pCpcTbl_Pal[iConfig][0] = 3; break;
			default: pCpcTbl_Pal[iConfig][1] = 0xe; pCpcTbl_Pal[iConfig][0] = 3; break;
			}
			pCpsTbl_Pal[iConfig][5] = (pCpsTbl_Pal[iConfig][5] & (~4)) | ((iRgb != 0) << 2);
			BVDC_Display_SetMacrovisionTable(hDisplay, pCpcTbl_Pal[iConfig], pCpsTbl_Pal[iConfig]);
			break;
		default: /* 480p/576p set AGC only */
			BVDC_Display_SetMacrovisionTable(hDisplay, pCpcTbl_Pal[0], pCpsTbl_Pal[0]);
			break;
		}
		break;
	default: printf("Wrong choice; will set No-Protection.\n");
		return;
	}
	BVDC_Display_SetMacrovisionType(hDisplay, eType);

	if(pContext->bApplyImmediately)
	{
		BTST_P_ApplyChanges(pContext);
	}

done:
#else
	BSTD_UNUSED(pContext);
#endif
	return;
}

/***************************************************************************
 *
 */
void BTST_P_SetDcs
	( BTST_P_Context          *pContext )
{
#if DCS_SUPPORT
	int iChoice;
	BVDC_Display_Handle hDisplay = pContext->ahDisplay[pContext->iSelectedComp];

	if(!hDisplay)
	{
		goto done;
	}

	printf("Please select DCS mode:\n");
	printf("Off(0) ON1(1) ON2(2) ON3(3) LOCK(4)\n");
	BTST_P_Scanf(pContext, "%d", &iChoice);
	switch(iChoice)
	{
	case 0:
	case 1:
	case 2:
	case 3:
		BVDC_Display_SetDcsMode(hDisplay, iChoice);
		break;
	case 4:
		BVDC_Display_DcsLock(hDisplay);
		break;
	default:
		printf("Wrong choice; no change made\n");
		return;
		break;
	}

	if(pContext->bApplyImmediately)
	{
		BTST_P_ApplyChanges(pContext);
	}

done:
#else
	BSTD_UNUSED(pContext);
#endif
	return;
}

/***************************************************************************
 *
 */
void BTST_P_SetDac
	( BTST_P_Context          *pContext )
{
	BVDC_Display_Handle hDisplay = pContext->ahDisplay[pContext->iSelectedComp];
	BVDC_DacOutput eDacOutput;
	int iNewDacOutput;

	if(!hDisplay)
	{
		goto done;
	}

	printf("(%2d) SVideo_Luma   (%2d) SVideo_Chroma   (%2d) Composite    \n", BVDC_DacOutput_eSVideo_Luma, BVDC_DacOutput_eSVideo_Chroma, BVDC_DacOutput_eComposite);
	printf("(%2d) Red           (%2d) Green           (%2d) Blue         \n", BVDC_DacOutput_eRed, BVDC_DacOutput_eGreen, BVDC_DacOutput_eBlue);
	printf("(%2d) Y             (%2d) Pr              (%2d) Pb           \n", BVDC_DacOutput_eY, BVDC_DacOutput_ePr, BVDC_DacOutput_ePb);
	printf("(%2d) Hsync         (%2d) Green_NoSync    (%2d) FilteredCvbs \n", BVDC_DacOutput_eHsync, BVDC_DacOutput_eGreen_NoSync, BVDC_DacOutput_eFilteredCvbs);
	printf("(%2d) Unusded                                                \n", BVDC_DacOutput_eUnused);

	BVDC_Display_GetDacConfiguration(hDisplay, BFramework_Dac_Composite_0, &eDacOutput);
	printf("Dac_Composite_0 [Current = %d]: ", eDacOutput);
	scanf("%d", &iNewDacOutput);
	BVDC_Display_SetDacConfiguration(hDisplay, BFramework_Dac_Composite_0, (BVDC_DacOutput)iNewDacOutput);

	BVDC_Display_GetDacConfiguration(hDisplay, BFramework_Dac_Composite_1, &eDacOutput);
	printf("Dac_Composite_1 [Current = %d]: ", eDacOutput);
	scanf("%d", &iNewDacOutput);
	BVDC_Display_SetDacConfiguration(hDisplay, BFramework_Dac_Composite_1, (BVDC_DacOutput)iNewDacOutput);

	BVDC_Display_GetDacConfiguration(hDisplay, BFramework_Dac_Svideo_Luma, &eDacOutput);
	printf("Dac_Svideo_Luma [Current = %d]: ", eDacOutput);
	scanf("%d", &iNewDacOutput);
	BVDC_Display_SetDacConfiguration(hDisplay, BFramework_Dac_Svideo_Luma, (BVDC_DacOutput)iNewDacOutput);

	BVDC_Display_GetDacConfiguration(hDisplay, BFramework_Dac_Svideo_Chroma, &eDacOutput);
	printf("Dac_Svideo_Chroma [Current = %d]: ", eDacOutput);
	scanf("%d", &iNewDacOutput);
	BVDC_Display_SetDacConfiguration(hDisplay, BFramework_Dac_Svideo_Chroma, (BVDC_DacOutput)iNewDacOutput);

	BVDC_Display_GetDacConfiguration(hDisplay, BFramework_Dac_Component_y, &eDacOutput);
	printf("Dac_Component_y [Current = %d]: ", eDacOutput);
	scanf("%d", &iNewDacOutput);
	BVDC_Display_SetDacConfiguration(hDisplay, BFramework_Dac_Component_y, (BVDC_DacOutput)iNewDacOutput);

	BVDC_Display_GetDacConfiguration(hDisplay, BFramework_Dac_Component_pr, &eDacOutput);
	printf("Dac_Component_pr [Current = %d]: ", eDacOutput);
	scanf("%d", &iNewDacOutput);
	BVDC_Display_SetDacConfiguration(hDisplay, BFramework_Dac_Component_pr, (BVDC_DacOutput)iNewDacOutput);

	BVDC_Display_GetDacConfiguration(hDisplay, BFramework_Dac_Component_pb, &eDacOutput);
	printf("Dac_Component_pb [Current = %d]: ", eDacOutput);
	scanf("%d", &iNewDacOutput);
	BVDC_Display_SetDacConfiguration(hDisplay, BFramework_Dac_Component_pb, (BVDC_DacOutput)iNewDacOutput);

	if(pContext->bApplyImmediately)
	{
		BTST_P_ApplyChanges(pContext);
	}

done:
	return;

}

/***************************************************************************
 *
 */
void BTST_P_SetMpaaDeci
	( BTST_P_Context          *pContext )
{
	uint32_t ulEnMpaaHdmi, ulEnMpaa656, ulEnMpaaCmpnt;
	BVDC_Display_Handle hDisplay = pContext->ahDisplay[pContext->iSelectedComp];

	if(!hDisplay)
	{
		goto done;
	}

	printf("Please set MPAA decimation (0 or 1) for (Hdmi, 656, Component):\n");
	BTST_P_Scanf(pContext, "%u %u %u", &ulEnMpaaHdmi, &ulEnMpaa656, &ulEnMpaaCmpnt);
	BVDC_Display_SetMpaaDecimation(hDisplay, BVDC_MpaaDeciIf_eHdmi,
		BVDC_Hdmi_0, ulEnMpaaHdmi? true: false);
	BVDC_Display_SetMpaaDecimation(hDisplay, BVDC_MpaaDeciIf_eComponent,
		BVDC_Cmpnt_0, ulEnMpaaCmpnt? true: false);

	if(pContext->bApplyImmediately)
	{
		BTST_P_ApplyChanges(pContext);
	}

done:
	return;
}

/***************************************************************************
 *
 */
void BTST_P_SetHdmiDropLines
	( BTST_P_Context          *pContext )
{
	uint32_t ulHdmiDropLines;
    BFMT_VideoFmt eFmt = BFMT_VideoFmt_eNTSC;
	BVDC_Display_Handle hDisplay = pContext->ahDisplay[pContext->iSelectedComp];
	int i;

	if(!hDisplay)
	{
		goto done;
	}

	printf("Please select format to configure hdmi line drop for:\n");
	printf("NTSC(0), NTSC_J(1), 480p(2), PAL_M(3)\n");
	BTST_P_Scanf(pContext, "%d", (int*)&i) ;

	if     (0 == i){eFmt = BFMT_VideoFmt_eNTSC;}
	else if(1 == i){eFmt = BFMT_VideoFmt_eNTSC_J;}
	else if(2 == i){eFmt = BFMT_VideoFmt_e480p;}
	else if(3 == i){eFmt = BFMT_VideoFmt_ePAL_M;}

	BVDC_Display_GetHdmiDropLines(hDisplay, BVDC_Hdmi_0, eFmt, &ulHdmiDropLines);

	printf("Current lines dropped: %d.  Please enter lines to drop:\n", ulHdmiDropLines);
	BTST_P_Scanf(pContext, "%d", (int*)&ulHdmiDropLines) ;

	BVDC_Display_SetHdmiDropLines(hDisplay, BVDC_Hdmi_0, eFmt, ulHdmiDropLines);

	if(pContext->bApplyImmediately)
	{
		BTST_P_ApplyChanges(pContext);
	}

done:
	return;
}

/***************************************************************************
 *
 */
void BTST_P_SetHdmiDeepColor
	( BTST_P_Context          *pContext )
{
	BAVC_HDMI_BitsPerPixel eColorDepth;
	BAVC_HDMI_PixelRepetition ePxlRep;
	BVDC_Display_Handle hDisplay = pContext->ahDisplay[pContext->iSelectedComp];
	int i;

	if(!hDisplay)
	{
		goto done;
	}

	BVDC_Display_GetHdmiColorDepth(hDisplay, &eColorDepth);
	printf("Please select color depth [Current = %d]:\n", eColorDepth);
	printf("24bit(0), 30bit(1), 36bit(2): ");
	BTST_P_Scanf(pContext, "%d", (int *)&i);
	eColorDepth = i;
	BVDC_Display_SetHdmiColorDepth(hDisplay, eColorDepth);

	BVDC_Display_GetHdmiPixelRepetition(hDisplay, &ePxlRep);
	printf("Please select pixel repeptition [Current = %d]:\n", ePxlRep);
	printf("None(0), 2x(1), 4x(3): ");
	BTST_P_Scanf(pContext, "%d", (int *)&i);
	ePxlRep = i;
#if (BTST_P_SUPPORT_HDM)
	BHDM_SetPixelRepetition (pContext->hHdm, ePxlRep);
#endif

	BVDC_Display_SetHdmiPixelRepetition(hDisplay, ePxlRep);
	BTST_P_ApplyChanges(pContext);

#if (BTST_P_SUPPORT_HDM)
	pContext->stHdmiSettings.ePixelRepetition = ePxlRep;
	pContext->stHdmiSettings.bForceEnableDisplay = true;
	BHDM_EnableDisplay(pContext->hHdm, &pContext->stHdmiSettings);
#endif

done:
	return;
}

/***************************************************************************
 *
 */
void BTST_P_CustomDisplay
	( BTST_P_Context          *pContext)
{
	pContext->bCustomDisplay ^= true;
	printf("%s customized display create\n", pContext->bCustomDisplay? "Enable":"Disable");
}


/***************************************************************************
 *
 */
void BTST_P_LoadMicrocode
	( BTST_P_Context          *pContext )
{
	BVDC_Display_Handle hDisplay = pContext->ahDisplay[pContext->iSelectedComp];
	int customFmt = 0;

	if(!hDisplay)
	{
		goto done;
	}

	printf("Select the custom video format, WXGA[0], 1080pHD_48Hz[1], 540p60[2], 540p50[3]:\n ");
	BTST_P_Scanf(pContext, "%d", &customFmt);

	switch (customFmt)
	{
	case 0:
		printf("Load WXGA settings \n ");

		BVDC_Display_SetCustomVideoFormat(hDisplay, &s_stFormatInfoCustom2);
		pContext->apFmtInfo[pContext->iSelectedComp] = &s_stFormatInfoCustom2;
		break;
	case 1:
		printf("Load 1080p@48Hz settings \n ");

		BVDC_Display_SetCustomVideoFormat(hDisplay, &s_stFormatInfoCustom_48Hz);
		pContext->apFmtInfo[pContext->iSelectedComp] = &s_stFormatInfoCustom_48Hz;
        break;
    case 2:
		printf("Load 540p@60Hz settings \n ");

		BVDC_Display_SetCustomVideoFormat(hDisplay, &s_stFormatInfo1920x540p_60Hz);
		pContext->apFmtInfo[pContext->iSelectedComp] = &s_stFormatInfo1920x540p_60Hz;
        break;
    case 3:
		printf("Load 540p@50Hz settings \n ");
		BVDC_Display_SetCustomVideoFormat(hDisplay, &s_stFormatInfo1920x540p_50Hz);
		pContext->apFmtInfo[pContext->iSelectedComp] = &s_stFormatInfo1920x540p_50Hz;
        break;
    default:
		printf("Invalid custom video format %d\n", customFmt);
		return;
	}

	if(pContext->bApplyImmediately)
	{
		BTST_P_ApplyChanges(pContext);
	}

done:
	return;
}

/***************************************************************************
 *
 */
void BTST_P_SetOsdConfig
	( BTST_P_Context          *pContext )
{
	BVDC_OsdSettings	osdSettings;
	BVDC_Compositor_Handle hCompositor = pContext->ahCompositor[pContext->iSelectedComp];
	int i;

	if(!hCompositor)
	{
		goto done;
	}

	printf("Please set OSD threshold for compositor[%d], [0-255]:\n", pContext->iSelectedComp);
	BTST_P_Scanf(pContext, "%d", (int*)&i) ;

	if ((i < 0) || (i > 255))
	{
		printf("%d is invalid OSD threshold value\n", i);
		return;
	}

	osdSettings.ucOsdSelectThreshold = (uint8_t) i;

	BVDC_Compositor_SetOsdConfiguration(hCompositor, &osdSettings );

	if(pContext->bApplyImmediately)
	{
		BTST_P_ApplyChanges(pContext);
	}

done:
	return;
}

/***************************************************************************
 *
 */
void BTST_P_GetOsdConfig
	( BTST_P_Context          *pContext )
{
	BVDC_OsdSettings	osdSettings;
	BVDC_Compositor_Handle hCompositor = pContext->ahCompositor[pContext->iSelectedComp];

	if(!hCompositor)
	{
		goto done;
	}

	BVDC_Compositor_GetOsdConfiguration(hCompositor, &osdSettings );

	printf("Compositor[%d] OSD threshold value is %d\n", pContext->iSelectedComp, osdSettings.ucOsdSelectThreshold);
done:
	return;
}

#ifndef MAX
#define MAX(a,b) \
	((a) > (b) ? (a) : (b))
#endif
void BTST_P_DispUcodeDump( BTST_P_Context  *pContext )
{
	size_t iFmt;
	FILE* fHandle;
	char fName[128];
	BERR_Code eErr;
	BAVC_MatrixCoefficients eColorMatrix;
	size_t diflen;
	const char* pchFormatStr;
	size_t maxlen = 0;
	BVDC_Display_Handle hDisplay =
		pContext->ahDisplay[pContext->iSelectedComp];

	/* Determine maximum length of video format names. This is used to make
	 * output neat. */
	for (iFmt = 0 ; iFmt < BFMT_VideoFmt_eMaxCount ; ++iFmt)
	{
		if (iFmt == BFMT_VideoFmt_eCustom2)
		{
			pchFormatStr = "BFMT_VideoFmt_eCustom2";
		}
		else
		{
			BFMT_VideoInfo videoInfo;
			if ((eErr = BFMT_GetVideoFormatInfo (iFmt, &videoInfo)) !=
				BERR_SUCCESS)
				continue;
			pchFormatStr = videoInfo.pchFormatStr;
		}
		maxlen = MAX (maxlen, strlen (pchFormatStr));
	}

	printf ("Enter destination file (%d characers max): ", sizeof(fName));
	fflush (stdout);
	if (scanf ("%s", fName) != 1) return;
	if ((fHandle = fopen (fName, "a")) == 0x0)
	{
		fprintf (stderr, "ERROR: could not open file %s: ", fName);
		perror ("");
		return;
	}
	fprintf (fHandle,
		"\n"
		"Begin microcode inventory:\n"
		"-------------------------\n");

	for (iFmt = 0 ; iFmt < BFMT_VideoFmt_eMaxCount ; ++iFmt)
	{
		int iTry;
		BVDC_Display_ItUcodeInfo dInfo;
		bool bAnalog = true;
		bool b656 = true;
		bool bDvi = true;

		if (iFmt == BFMT_VideoFmt_eCustom2)
		{
			pchFormatStr = "BFMT_VideoFmt_eCustom2";
		}
		else
		{
			BFMT_VideoInfo videoInfo;
			if ((eErr = BFMT_GetVideoFormatInfo (iFmt, &videoInfo)) !=
				BERR_SUCCESS)
				continue;
			pchFormatStr = videoInfo.pchFormatStr;
		}
		diflen = maxlen - strlen (pchFormatStr);

		fflush (stderr);
		fprintf (stderr, "Trying %s...\n", pchFormatStr);
		fprintf (stderr, "------------------------------------------\n");
		fflush (stderr);

		fprintf (fHandle, "%s ", pchFormatStr);
		while (diflen > 0)
		{
			putc (' ', fHandle);
			--diflen;
		}
		eErr = BVDC_Display_SetVideoFormat(hDisplay, iFmt);
		if (eErr != BERR_SUCCESS)
		{
			fprintf (fHandle, "<fail: set video format: %08x>\n", eErr);
			fflush (fHandle);
			continue;
		}
		BTST_P_SetDacs (pContext, true, iFmt);
		eErr =
			BVDC_Display_Set656Configuration(
				hDisplay, BVDC_Itur656Output_0, true);
		if (eErr != BERR_SUCCESS) b656 = false;

		BDBG_ASSERT (BVDC_Display_GetHdmiConfiguration(
			hDisplay, BVDC_Hdmi_0, &eColorMatrix) == BERR_SUCCESS);
		if (BAVC_MatrixCoefficients_eUnknown == eColorMatrix)
		{
			BTST_P_ToggleHdmiOut_priv (pContext, false);
		}
		for (iTry = 0 ; iTry < 3 ; ++iTry)
		{
			eErr = BTST_P_ApplyChanges(pContext);
			switch (eErr)
			{
			case BERR_SUCCESS:
				iTry = 3;
				break;
			case BVDC_ERR_FORMAT_NOT_SUPPORT_ANALOG_OUTPUT:
			case BVDC_ERR_INVALID_DAC_SETTINGS:
			case BVDC_ERR_VIDEOFMT_OUTPUT_MISMATCH:
				bAnalog = false;
				BTST_P_SetDacs (pContext, false, iFmt);
				break;
			case BVDC_ERR_INVALID_656OUT_MODE:
			case BVDC_ERR_INVALID_656OUT_USE:
				b656 = false;
				BDBG_ASSERT (BVDC_Display_Set656Configuration(
					hDisplay, BVDC_Itur656Output_0, false) == BERR_SUCCESS);
				break;
			case BVDC_ERR_INVALID_HDMI_MODE:
			case BVDC_ERR_INVALID_HDMI_USE:
				bDvi = false;
				BTST_P_ToggleHdmiOut_priv (pContext, false);
				break;
			case BERR_INVALID_PARAMETER:
				/* Most severe error: nothing will work. */
				bAnalog = false;
				b656 = false;
				bDvi = false;
				iTry = 3;
				break;
			default:
				fprintf (stderr, "ERROR: BTST_P_ApplyChanges returned %08x\n",
					eErr);
				fflush (stderr);
				goto done;
				break;
			}
		}

		if (eErr != BERR_SUCCESS)
		{
			fprintf (fHandle, "<fail: apply changes: %08x>\n", eErr);
			fflush (fHandle);
			continue;
		}

		BVDC_Display_GetItUcodeInfo (hDisplay, &dInfo);
		if (bAnalog)
		{
			fprintf (fHandle, "Analog %06x %06x  ",
				dInfo.ulAnalogTimestamp, dInfo.ulAnalogChecksum);
		}
		else
		{
			fprintf (fHandle, "Analog ------ ------  ");
		}
		if (b656)
		{
			fprintf (fHandle, "656 %06x %06x  ",
				dInfo.ulI656Timestamp, dInfo.ulI656Checksum);
		}
		else
		{
			fprintf (fHandle, "656 ------ ------  ");
		}
		if (bDvi)
		{
			fprintf (fHandle, "DVI %06x %06x  ",
				dInfo.ulDviTimestamp, dInfo.ulDviChecksum);
		}
		else
		{
			fprintf (fHandle, "DVI ------ ------  ");
		}
		fprintf (fHandle, "\n");
		fflush (fHandle);
	}

	fprintf (fHandle, "-------------------------\n");
	fclose (fHandle);

done:
	/* Pick a simple state for VEC */
	if ((eErr = BVDC_Display_SetVideoFormat(hDisplay, BFMT_VideoFmt_eNTSC)) !=
		BERR_SUCCESS)
	{
		fprintf (stderr, "ERROR: BVDC_Display_SetVideoFormat fails\n");
		return;
	}
	BTST_P_SetDacs (pContext, true, BFMT_VideoFmt_eNTSC);
	if ((eErr = BVDC_Display_Set656Configuration(
		hDisplay, BVDC_Itur656Output_0, false)) != BERR_SUCCESS)
	{
		fprintf (stderr, "ERROR: BVDC_Display_Set656Configuration fails\n");
		return;
	}
	if ((eErr = BVDC_Display_GetHdmiConfiguration(
		hDisplay, BVDC_Hdmi_0, &eColorMatrix)) != BERR_SUCCESS)
	{
		fprintf (stderr, "ERROR: BVDC_Display_GetHdmiConfiguration fails\n");
		return;
	}
	if (BAVC_MatrixCoefficients_eUnknown != eColorMatrix)
	{
		BTST_P_ToggleHdmiOut (pContext);
	}
	if ((eErr = BTST_P_ApplyChanges(pContext)) != BERR_SUCCESS)
	{
		fprintf (stderr, "ERROR: BTST_P_ApplyChanges fails\n");
		return;
	}
}

static void BTST_P_SetDacs(
	BTST_P_Context *pContext, bool bEnable, BFMT_VideoFmt eFmt )
{
	BVDC_Display_Handle hDisplay =
		pContext->ahDisplay[pContext->iSelectedComp];

	if (bEnable)
	{
		if(BVDC_CompositorId_eCompositor0==pContext->iSelectedComp)
		{
			/* format is HD or secondary display in used, don't control
			 * DAC0, 1, 2 */
			if(IS_HD(eFmt) || (pContext->ahDisplay[1]))
			{
				BVDC_Display_SetDacConfiguration(
					hDisplay,
					BFramework_Dac_Composite_0 | BFramework_Dac_Svideo_Chroma |
						BFramework_Dac_Svideo_Luma,
					BVDC_DacOutput_eUnused);
				BVDC_Display_SetDacConfiguration(
					hDisplay, BFramework_Dac_Component_pr, BVDC_DacOutput_ePr);
				BVDC_Display_SetDacConfiguration(
					hDisplay, BFramework_Dac_Component_y, BVDC_DacOutput_eY);
				BVDC_Display_SetDacConfiguration(
					hDisplay, BFramework_Dac_Component_pb, BVDC_DacOutput_ePb);
			}
			else
			{

				if(pContext->stSettings.bVecSwap)
				{
					BVDC_Display_SetDacConfiguration(
						hDisplay, BFramework_Dac_Composite_0,
						BVDC_DacOutput_eComposite);
					BVDC_Display_SetDacConfiguration(
						hDisplay, BFramework_Dac_Svideo_Chroma,
						BVDC_DacOutput_eSVideo_Chroma);
					BVDC_Display_SetDacConfiguration(
						hDisplay, BFramework_Dac_Svideo_Luma,
						BVDC_DacOutput_eSVideo_Luma);
				}

				BVDC_Display_SetDacConfiguration(
					hDisplay, BFramework_Dac_Component_pr, BVDC_DacOutput_ePr);
				BVDC_Display_SetDacConfiguration(
					hDisplay, BFramework_Dac_Component_y, BVDC_DacOutput_eY);
				BVDC_Display_SetDacConfiguration(
					hDisplay, BFramework_Dac_Component_pb, BVDC_DacOutput_ePb);
			}
		}

		if(BFMT_IS_VESA_MODE(eFmt) ||
		   BFMT_IS_3D_MODE(eFmt)   ||
		   (eFmt == BFMT_VideoFmt_eCUSTOM_1366x768p))
		{
			printf("DVI VESA mode or 3D mode! Will turn off the DAC outputs!\n");
			BVDC_Display_SetDacConfiguration(
				hDisplay,
				BFramework_Dac_Svideo_Luma | BFramework_Dac_Svideo_Chroma |
					BFramework_Dac_Composite_0 | BFramework_Dac_Composite_1 |
					BFramework_Dac_Component_y | BFramework_Dac_Component_pr |
					BFramework_Dac_Component_pb,
				BVDC_DacOutput_eUnused);
		}
	}
	else
	{
		BVDC_Display_SetDacConfiguration(
			hDisplay,
			BFramework_Dac_Svideo_Luma | BFramework_Dac_Svideo_Chroma |
				BFramework_Dac_Composite_0 | BFramework_Dac_Composite_1 |
				BFramework_Dac_Component_y | BFramework_Dac_Component_pr |
				BFramework_Dac_Component_pb,
			BVDC_DacOutput_eUnused);
	}
}

#ifdef BVDC_P_DISPLAY_DUMP
void BTST_P_DispTblDump( BTST_P_Context  *pContext )
{
	BVDC_Display_DumpTables();
	BVDC_Display_MV_DumpTables();
}
#endif

void BTST_P_Display3D
	( BTST_P_Context  *pContext )
{
	BVDC_Display_Handle hDisplay = pContext->ahDisplay[pContext->iSelectedComp];
	BVDC_Window_Handle  hWindow = pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].hWindow;
	BFMT_Orientation    eOrientation, eOldOrientation ;
	int32_t             lRightWinOffset;
	int32_t lLeft, lTop;
	uint32_t ulLeft, ulTop, ulWidth, ulHeight;

	if(!hDisplay)
	{
		goto done;
	}

	BVDC_Display_GetOrientation(hDisplay, &eOldOrientation);
	BVDC_Window_GetDstRect(hWindow, &lLeft, &lTop, &ulWidth, &ulHeight);
	BVDC_Window_GetScalerOutput(hWindow, &ulLeft, &ulTop, &ulWidth, &ulHeight);

	printf("Enter Orientation [Current = %d]: 2D(0),LR(1),OU(2) ", eOldOrientation) ;
	BTST_P_Scanf(pContext, "%d", &eOrientation) ;
	BVDC_Display_SetOrientation(hDisplay, eOrientation);

	if(!hWindow)
	{
		goto done;
	}

	if(eOrientation != BFMT_Orientation_e2D)
	{
		BVDC_Window_GetDstRightRect(hWindow, &lRightWinOffset);
		printf("Enter right window left offset [Current = %d]: ", lRightWinOffset);
		BTST_P_Scanf(pContext, "%d", &lRightWinOffset) ;
		BVDC_Window_SetDstRightRect(hWindow, lRightWinOffset);
	}

	if(eOldOrientation == BFMT_Orientation_e2D)
	{
		if(eOrientation == BFMT_Orientation_e3D_LeftRight)
		{
			/* 2d -> 3dLR */
			ulWidth = ulWidth / 2;
		}
		else if(eOrientation == BFMT_Orientation_e3D_OverUnder)
		{
			/* 2d -> 3dOU */
			ulHeight = ulHeight / 2;
		}
	}
	else if(eOldOrientation == BFMT_Orientation_e3D_LeftRight)
	{
		if(eOrientation == BFMT_Orientation_e2D)
		{
			/* 3dLR -> 2d */
			ulWidth = ulWidth * 2;
		}
		else if(eOrientation == BFMT_Orientation_e3D_OverUnder)
		{
			/* 3dLR -> 3dOU */
			ulWidth = ulWidth * 2;
			ulHeight = ulHeight / 2;
		}
	}
	else if(eOldOrientation == BFMT_Orientation_e3D_OverUnder)
	{
		if(eOrientation == BFMT_Orientation_e2D)
		{
			/* 3dOU -> 2d */
			ulHeight = ulHeight * 2;
		}
		else if(eOrientation == BFMT_Orientation_e3D_LeftRight)
		{
			/* 3dOU -> 3dLR */
			ulWidth = ulWidth / 2;
			ulHeight = ulHeight * 2;
		}
	}

	BVDC_Window_SetDstRect(hWindow, lLeft, lTop, ulWidth, ulHeight);
	BVDC_Window_SetScalerOutput(hWindow, ulLeft, ulTop, ulWidth, ulHeight);

#if (BTST_P_SUPPORT_HDM)
	if(pContext->hHdm)
	{
		BHDM_GetVendorSpecificInfoFrame(pContext->hHdm, &pContext->stHdmiSettings.stVendorSpecificInfoFrame);
		if(eOrientation == BFMT_Orientation_e2D)
			pContext->stHdmiSettings.stVendorSpecificInfoFrame.eHdmiVideoFormat = BAVC_HDMI_VSInfoFrame_HDMIVideoFormat_eNone;
		else
		{
			pContext->stHdmiSettings.stVendorSpecificInfoFrame.eHdmiVideoFormat = BAVC_HDMI_VSInfoFrame_HDMIVideoFormat_e3DFormat;
			if(eOrientation == BFMT_Orientation_e3D_LeftRight)
				pContext->stHdmiSettings.stVendorSpecificInfoFrame.e3DStructure = BAVC_HDMI_VSInfoFrame_3DStructure_eSidexSideHalf;
			else if(eOrientation == BFMT_Orientation_e3D_OverUnder)
				pContext->stHdmiSettings.stVendorSpecificInfoFrame.e3DStructure = BAVC_HDMI_VSInfoFrame_3DStructure_eTopAndBottom;
			else
				pContext->stHdmiSettings.stVendorSpecificInfoFrame.e3DStructure = BAVC_HDMI_VSInfoFrame_3DStructure_eFieldAlternative;
		}
		BHDM_SetVendorSpecificInfoFrame(pContext->hHdm, &pContext->stHdmiSettings.stVendorSpecificInfoFrame);
	}
#endif

	if(pContext->bApplyImmediately)
	{
		BTST_P_ApplyChanges(pContext);
	}

done:
	return;
}


void BTST_P_Display3DSrcBufSelect
	( BTST_P_Context  *pContext )
{
	BVDC_Display_Handle hDisplay = pContext->ahDisplay[pContext->iSelectedComp];
	BVDC_3dSourceBufferSelect   e3dSrcBufSel;

	if(!hDisplay)
	{
		goto done;
	}

	BVDC_Display_Get3dSourceBufferSelect(hDisplay, &e3dSrcBufSel);

	printf("Enter 3d Conversion Mode [Current = %d]: LR(0), LL(1), RR(2) ", e3dSrcBufSel) ;
	BTST_P_Scanf(pContext, "%d", &e3dSrcBufSel) ;
	BVDC_Display_Set3dSourceBufferSelect(hDisplay, e3dSrcBufSel);

	if(pContext->bApplyImmediately)
	{
		BTST_P_ApplyChanges(pContext);
	}

done:
	return;
}

#if BTST_P_SUPPORT_ORTHOGONAL_VEC
#define BTST_P_VF_FILTER_SIZE 10
#else
#define BTST_P_VF_FILTER_SIZE 5
#endif

void BTST_P_ChgVfFilter
	( BTST_P_Context  *pContext )
{
	BVDC_Display_Handle hDisplay = pContext->ahDisplay[pContext->iSelectedComp];

	uint32_t ulEnable;
	bool bEnable;
	bool bOverride;
	uint32_t ulOutput;
	uint32_t ulChannel;
	BVDC_DacOutput eDacOutput;
	uint32_t ulNumFilterRegs = BTST_P_VF_FILTER_SIZE;
	uint32_t aulFilterRegs[BTST_P_VF_FILTER_SIZE];
	BVDC_DisplayOutput eDisplayOutput;
	uint32_t i;

	if(!hDisplay)
	{
		goto done;
	}

	printf("Enter output type: Component(0), Cvbs(1), SVideo(2): ");
	BTST_P_Scanf(pContext, "%u", &ulOutput);
	switch(ulOutput)
	{
	case 0:
		eDisplayOutput = BVDC_DisplayOutput_eComponent;
		break;
	case 1:
		eDisplayOutput = BVDC_DisplayOutput_eComposite;
		break;
	case 2:
		eDisplayOutput = BVDC_DisplayOutput_eSVideo;
		break;
	default:
		printf("Invalid output\n");
		goto done;
		break;
	}

	printf("Enter DAC channel: (0)SVideo Luma, (1)Svideo Chroma, (2)Composite,\n");
	printf("                   (3)Red, (4)Green, (5)Blue,\n");
	printf("                   (6)Y, (7)Pr, (8)Pb, (9)Green NoSync\n");
	BTST_P_Scanf(pContext, "%u", &ulChannel);

	switch(ulChannel)
	{
	case 0:
		eDacOutput = BVDC_DacOutput_eSVideo_Luma;
		break;
	case 1:
		eDacOutput = BVDC_DacOutput_eSVideo_Chroma;
		break;
	case 2:
		eDacOutput = BVDC_DacOutput_eComposite;
		break;
	case 3:
		eDacOutput = BVDC_DacOutput_eRed;
		break;
	case 4:
		eDacOutput = BVDC_DacOutput_eGreen;
		break;
	case 5:
		eDacOutput = BVDC_DacOutput_eBlue;
		break;
	case 6:
		eDacOutput = BVDC_DacOutput_eY;
		break;
	case 7:
		eDacOutput = BVDC_DacOutput_ePr;
		break;
	case 8:
		eDacOutput = BVDC_DacOutput_ePb;
		break;
	case 9:
		eDacOutput = BVDC_DacOutput_eGreen_NoSync;
		break;

	default:
		return;
		break;
	}

	BVDC_Display_GetVfFilter(hDisplay,
							 eDisplayOutput,
		                     ulChannel,
							&bOverride,
	            (uint32_t *)&aulFilterRegs,
	                         ulNumFilterRegs );

	printf("Current Vf Filter: %s\n", (
		bOverride) ? "User VF Filter" : "Interal VF Filter");
	printf("Filter register size: %d\n", ulNumFilterRegs);

	for (i = 0; i < ulNumFilterRegs; i++)
	{
		printf("0x%.8x ", aulFilterRegs[i]);
#if BTST_P_SUPPORT_ORTHOGONAL_VEC
		if(i == 4)
			printf("\n");
#endif
	}
	printf("\n\n");

	printf("Override VF Filter? [yes=1/no=0]: ");
	BTST_P_Scanf(pContext, "%u", &ulEnable);
	bEnable = (ulEnable == 1) ? true : false;

	for(i = 0; i < ulNumFilterRegs; i++)
	{
		aulFilterRegs[i] = 0;
	}

	if(bEnable)
	{
		printf("Enter VF Filter register values:\n");
		for (i=0; i < ulNumFilterRegs; i++)
		{
			BTST_P_Scanf(pContext, "%u", &aulFilterRegs[i]);
		}
	}

	BVDC_Display_SetVfFilter(hDisplay,
	                         eDisplayOutput,
	                         ulChannel,
	                         ulEnable,
	                         aulFilterRegs,
	                         ulNumFilterRegs );

	if(pContext->bApplyImmediately)
	{
		BTST_P_ApplyChanges(pContext);
	}

done:
	return;
}

void BTST_P_MuteOutput
	( BTST_P_Context  *pContext )
{
	BVDC_Display_Handle hDisplay = pContext->ahDisplay[pContext->iSelectedComp];

	BVDC_MuteMode eMuteMode;
	uint32_t ulMute;
	uint32_t ulOutput;
	BVDC_DisplayOutput eDisplayOutput;

	if(!hDisplay)
	{
		goto done;
	}

	printf("Enter output type: Component(0), Cvbs(1), SVideo(2), Dvo(3), 656(4): ");
	BTST_P_Scanf(pContext, "%u", &ulOutput);
	switch(ulOutput)
	{
	case 0:
		eDisplayOutput = BVDC_DisplayOutput_eComponent;
		break;
	case 1:
		eDisplayOutput = BVDC_DisplayOutput_eComposite;
		break;
	case 2:
		eDisplayOutput = BVDC_DisplayOutput_eSVideo;
		break;
	case 3:
		eDisplayOutput = BVDC_DisplayOutput_eDvo;
		break;
	case 4:
		eDisplayOutput = BVDC_DisplayOutput_e656;
		break;
	default:
		printf("Invalid output\n");
		goto done;
		break;
	}

	printf("Please enter mute setting: Enable(1), Disable(0)\n");
	BVDC_Display_GetMuteMode(hDisplay, eDisplayOutput, &eMuteMode);
	printf("Mute setting [%s]: ", (eMuteMode == BVDC_MuteMode_eConst) ? "Enable" : "Disable");

	BTST_P_Scanf(pContext, "%u", &ulMute);
	eMuteMode = (ulMute) ? BVDC_MuteMode_eConst : BVDC_MuteMode_eDisable;
	BVDC_Display_SetMuteMode(hDisplay, eDisplayOutput, eMuteMode);

	if(pContext->bApplyImmediately)
	{
		BTST_P_ApplyChanges(pContext);
	}

done:
	return;
}


/***************************************************************************
 *
 */
void BTST_P_DisplayGetCap
	( BTST_P_Context          *pContext )
{
	BVDC_Display_Handle hDisplay = pContext->ahDisplay[pContext->iSelectedComp];
	BVDC_Display_Capabilities   stCapabilities;

	if(!hDisplay)
	{
		goto done;
	}

	BVDC_Display_GetCapabilities(hDisplay, &stCapabilities);

done:
	return;
}


/***************************************************************************
 *
 */
void BTST_P_CompGetCap
	( BTST_P_Context          *pContext )
{
	BVDC_Compositor_Handle hCompositor = pContext->ahCompositor[pContext->iSelectedComp];
	BVDC_Compositor_Capabilities   stCapabilities;

	if(!hCompositor)
	{
		goto done;
	}

	BVDC_Compositor_GetCapabilities(hCompositor, &stCapabilities);

done:
	return;
}
/* end of file */
