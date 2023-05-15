/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvdc_capture_priv.c $
 * $brcm_Revision: Hydra_Software_Devel/153 $
 * $brcm_Date: 10/16/12 5:18p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vdc/src/common/bvdc_capture_priv.c $
 * 
 * Hydra_Software_Devel/153   10/16/12 5:18p vanessah
 * SW7445-8: add CAP_7 support
 * 
 * Hydra_Software_Devel/152   8/15/12 6:20p tdo
 * SW7445-8: Create Appframework emulation environment for 7445 A0
 *
 * Hydra_Software_Devel/151   7/31/12 4:55p yuxiaz
 * SW7425-1835: Use RDC slot timestamp for capture and playback timestamp
 * in multi-buffering algo.
 *
 * Hydra_Software_Devel/150   5/30/12 5:06p syang
 * SW7425-2093: SW7425-2093: extra ApplyChange with 1st SetSurface no
 * longer needed; vfd/cap time stamp related scratch registers now share
 * with vfd's surface manager's; added vfd resource check; more clean up
 *
 * Hydra_Software_Devel/149   3/29/12 11:18a tdo
 * SW7435-85: MVP_0 enable error.  Add support for new capture enable
 * control bit.
 *
 * Hydra_Software_Devel/148   1/18/12 5:21p tdo
 * SW7435-9: Add support for CAP_5, MFD_3, VFD_5
 *
 * Hydra_Software_Devel/147   12/1/11 4:31p yuxiaz
 * SW7425-968, SW7344-95: Merged into mainline.: added independent source
 * clipping of right window in 3D mode.
 *
 * Hydra_Software_Devel/146   11/30/11 3:29p yuxiaz
 * SW7425-1182, SW7231-469: Fixed BYTE_ORDER settings in CAP and VFD on
 * both LE and BE system.
 *
 * Hydra_Software_Devel/145   10/19/11 3:30p yuxiaz
 * SW7425-1182: Merge into mainline: Add support for more pixel formats
 * for surfaces accessed by CAP/MFD
 *
 * Hydra_Software_Devel/SW7425-1182/2   10/19/11 11:03a yuxiaz
 * SW7425-1182: Only need to convert on little endian system.
 *
 * Hydra_Software_Devel/SW7425-1182/1   10/18/11 3:19p yuxiaz
 * SW7425-1182: Clean up CAP_0_BYTE_ORDER settings for chipset doesn't
 * support BYTE_ORDER in MFD.
 *
 * Hydra_Software_Devel/144   12/16/10 11:39p pntruong
 * SW3548-2987: Added 1080p@30hz 3DOU_AS hdmi input support.
 *
 * Hydra_Software_Devel/143   12/10/10 4:45p yuxiaz
 * SW7422-39: Always use dual pointer mode for 3D in CAP and VFD.
 *
 * Hydra_Software_Devel/142   11/12/10 4:18p yuxiaz
 * SW7425-41: Fix 2d-3d conversion.
 *
 * Hydra_Software_Devel/141   11/11/10 7:56p pntruong
 * SW7425-51: Fixed 7468 build errors.
 *
 * Hydra_Software_Devel/140   11/11/10 7:12p hongtaoz
 * SW7425-51: reduced combo trigger use in VDC to 1 per MFD source;
 *
 * Hydra_Software_Devel/139   10/29/10 10:58a yuxiaz
 * SW7422-31, SW7422-32, SW7422-39: More for 3D Video support.
 *
 * Hydra_Software_Devel/138   10/19/10 4:06p yuxiaz
 * SW7420-1190: Make mosaic scratch buffer to be per window base.
 *
 * Hydra_Software_Devel/137   10/7/10 11:45a yuxiaz
 * SW7422-38: Handle VDC changes to memory allocation for 3D.
 *
 * Hydra_Software_Devel/136   9/30/10 4:04p yuxiaz
 * SW7422-39: More on 3D video support in CAP and VFD.
 *
 * Hydra_Software_Devel/135   9/29/10 5:21p yuxiaz
 * SW7422-39: Added 3D video support in CAP and VFD.
 *
 * Hydra_Software_Devel/134   8/26/10 5:24p tdo
 * SW7422-57: Add simple vdc support
 *
 * Hydra_Software_Devel/133   7/30/10 12:09p pntruong
 * SW3556-1162: Teletext low signal check.  Give some more room for
 * unfiltered lines.
 *
 * Hydra_Software_Devel/132   7/13/10 11:26a yuxiaz
 * SW3548-2987: Added new 1080p@30hz support for 3DTV.
 *
 * Hydra_Software_Devel/131   6/18/10 8:50p rpan
 * SW7400-2763: Fixed compile errors.
 *
 * Hydra_Software_Devel/130   6/4/10 2:26p pntruong
 * SW3548-2814: removed warnings.
 *
 * Hydra_Software_Devel/129   4/19/10 10:12p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic. Move
 * BDBG_OBJECT_ID_DECLARE private header files instead of .c.
 *
 * Hydra_Software_Devel/128   4/7/10 11:26a tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic.  Rename TLA
 *
 * Hydra_Software_Devel/127   4/5/10 3:58p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic
 *
 * Hydra_Software_Devel/126   3/17/10 12:01p syang
 * SW7405-4046: set canvas ctrl after sur ctrl; handle gfx win vnet the
 * same as video; ensure that reader and writer vnetState match in sync-
 * locked case; aligned width up to even for stride in feeder and cap;
 * assert mosaic mode doesn't co-exist with dest cut;
 *
 * Hydra_Software_Devel/125   2/3/10 6:23p albertl
 * SW7125-180: Updated TESTFEATURE1 defines for 7125.
 *
 * Hydra_Software_Devel/124   2/1/10 6:36p hongtaoz
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
 * Hydra_Software_Devel/BCM3D/2   1/15/10 6:34p hongtaoz
 * SW3548-2680, SW3548-2681: added custom formats to support HDMI 1.4 3D
 * 720p50/60 input passed-thru to external panel processor; reanmed 2160i
 * to custom DVI format;
 *
 * Hydra_Software_Devel/BCM3D/1   12/17/09 6:08p hongtaoz
 * SW3548-2680, SW3548-2681: added HDMI1.4 3D format detection and
 * handling for 11-bit BVN;
 *
 * Hydra_Software_Devel/123   12/15/09 8:25p pntruong
 * SW3556-886: TTX operation is wrong in SECAM signal.  Dynamically turn
 * on/off secam filter in sop/eop. Also use vfd clipping for this case.
 *
 * Hydra_Software_Devel/122   11/16/09 5:03p yuxiaz
 * SW7405-3406: Clean up bMosaicMode and  bMosaicClearOutside.
 *
 * Hydra_Software_Devel/121   10/12/09 4:51p rpan
 * SW7405-3159: Free up RDC scratch register allocated for time stamp
 * purpose when VFD/CAP gets destroyed.
 *
 * Hydra_Software_Devel/120   10/12/09 4:35p rpan
 * SW7405-3159: Moved writer/reader time stamp register from CAP/VFD to
 * RDC scratch registers.
 *
 * Hydra_Software_Devel/119   9/3/09 10:30p pntruong
 * SW3548-2373: Rolled back PR56964.
 *
 * Hydra_Software_Devel/118   8/6/09 2:11p hongtaoz
 * PR57098: cap test feature prediction mode should be programmed after
 * it's determined;
 *
 * Hydra_Software_Devel/117   7/21/09 2:17p syang
 * PR 56964: take off the "connect drainer, en and then dis" step  in
 * vnet-drain code
 *
 * Hydra_Software_Devel/116   7/15/09 12:12p syang
 * PR 56605: 1). feeder/cap calculate stride the same way (without even
 * pix align); 2). do even width/left align only if necessary
 *
 * Hydra_Software_Devel/115   5/28/09 2:18p yuxiaz
 * PR55364: Disable dithering when any testfeature1 is enabled
 *
 * Hydra_Software_Devel/114   2/26/09 6:10p yuxiaz
 * PR52287: 422-444 and 444-422 converters should use decimate/duplicate.
 *
 * Hydra_Software_Devel/113   2/24/09 11:44a yuxiaz
 * PR51231: Set OFLOW_AVOID for DCX.
 *
 * Hydra_Software_Devel/112   2/17/09 2:45p yuxiaz
 * PR52193: Use new throughput equation, since no rate smooth fifo between
 * BVN and VEC on 3548/3556.
 *
 * Hydra_Software_Devel/111   1/30/09 5:09p yuxiaz
 * PR51713: Reset CAP during DCX mode switch. Clean up DCX software
 * workaround.
 *
 * Hydra_Software_Devel/110   1/28/09 3:40p yuxiaz
 * PR51335: Remove CAP overflow workaround for B2.
 *
 * Hydra_Software_Devel/109   1/16/09 6:12p yuxiaz
 * PR51231: Limit buffer size to 4Kbits to workaround cap overflow bug.
 *
 * Hydra_Software_Devel/108   11/25/08 11:10a syang
 * PR 49562: add reset to output channel ping-pong buf of sub-modules and
 * loopbacks and free-channels
 *
 * Hydra_Software_Devel/107   11/24/08 11:02a yuxiaz
 * PR49598: Use bandwidth equation to calculate throughput.
 *
 * Hydra_Software_Devel/106   11/20/08 9:30a syang
 * PR 48970: clean-up module disable and drian handling so that RUL dump
 * is easy to understand
 *
 * Hydra_Software_Devel/105   10/27/08 5:36p syang
 * PR 46477: back off SetEnable in in BuildRul_isr according to GetOps (it
 * causes window to disapper on 7400 D0)
 *
 * Hydra_Software_Devel/104   10/24/08 4:06p syang
 * PR 46477: in BuildRul_isr only disable in shadow regs  according to
 * GetOps (some SetEnable_isr does HW reset)
 *
 * Hydra_Software_Devel/103   10/24/08 11:04a syang
 * PR 46477: in BuildRul_isr reset enable bit according to GetOps
 *
 * Hydra_Software_Devel/102   10/22/08 1:58p hongtaoz
 * PR48143: only build necessary CAP/DNR/HSCL/SCL RULs when their
 * operational flags are enabled; this fixed SCL/HSCL enable error;
 *
 * Hydra_Software_Devel/101   10/17/08 4:34p yuxiaz
 * PR46305: Fix compile error.
 *
 * Hydra_Software_Devel/100   10/17/08 3:25p yuxiaz
 * PR46305: Bring up testfeature1 on 3548 B0.
 *
 * Hydra_Software_Devel/99   10/17/08 11:20a yuxiaz
 * PR46305: Bring up testfeature1 on 3548 B0.
 *
 * Hydra_Software_Devel/98   10/3/08 10:59a yuxiaz
 * PR46305, PR46307, PR46309: Use new TestFeature1 function from BSUR.
 * Change BVDC_P_SUPPORT_VIDEO_TestFeature1 to
 * BVDC_P_SUPPORT_VIDEO_TESTFEATURE1.
 *
 * Hydra_Software_Devel/97   10/3/08 9:23a yuxiaz
 * PR46305, PR46307, PR46309: More Video TestFeature1 support.
 *
 * Hydra_Software_Devel/96   9/30/08 4:56p yuxiaz
 * PR46305, PR46307: Video TestFeature1 support: Fix throughput
 * calculation, combine MAD and ANR set TestFeature1 function.
 *
 * Hydra_Software_Devel/95   9/26/08 3:23p yuxiaz
 * PR46305, PR46307: More video TestFeature1 support for 3548 B0.
 *
 * Hydra_Software_Devel/94   9/22/08 6:04p pntruong
 * PR46305, PR46307, PR46309: Fixed build errors.
 *
 * Hydra_Software_Devel/93   9/22/08 4:51p yuxiaz
 * PR46305, PR46307, PR46309: Added video TestFeature1 support for 3548 B0.
 *
 * Hydra_Software_Devel/92   9/9/08 9:02p tdo
 * PR45802: Remove unnecessary macros and unused code
 *
 * Hydra_Software_Devel/91   8/8/08 3:27p yuxiaz
 * PR45484: Enable Dithering in VDC.
 *
 * Hydra_Software_Devel/90   7/29/08 7:48p pntruong
 * PR40213: Used correct alignment macro.
 *
 * Hydra_Software_Devel/89   2/29/08 4:17p yuxiaz
 * PR39158: Implement dithering in various BVN components for 3548.
 *
 * Hydra_Software_Devel/88   2/22/08 2:31p yuxiaz
 * PR39155: Added de-ringing filters in BVN.
 *
 * Hydra_Software_Devel/87   9/23/07 3:18p hongtaoz
 * PR34955: fixed capture timestamp reg address;
 *
 * Hydra_Software_Devel/86   9/4/07 6:59p pntruong
 * PR33795: NTSC video bouncing every 17secs on 3553. PC input is 8bit 422
 * format on 3552.
 *
 * Hydra_Software_Devel/85   9/4/07 5:28p jessem
 * PR 34590: Added the other 4 8-bit 422 pixel formats that weren't
 * handled perviously. Also, corrected pixel byte ordering to reflect
 * actual ordering as  indicated by the pixel format name.
 *
 * Hydra_Software_Devel/84   7/16/07 2:38p yuxiaz
 * PR29842: 7405 mosaic mode bring up.
 *
 * Hydra_Software_Devel/PROD_LGChicago_3563/1   8/6/07 7:42p gjesse
 * PR33795: PC input is 8bit 422 format on 3552.
 *
 * Hydra_Software_Devel/83   6/8/07 11:30a syang
 * PR 31849: separate drain from disable for MAD, ANR and CAP
 *
 * Hydra_Software_Devel/80   4/4/07 3:42p syang
 * PR 29243: change VnetMode def (now use bit-fields)
 *
 * Hydra_Software_Devel/79   4/2/07 11:26a syang
 * PR 29243: clean up: take off the obselete old-vnet-code
 *
 * Hydra_Software_Devel/78   2/23/07 6:28p tdo
 * PR 27970: Share LPB and FCH if source is the same to eliminate the out
 * of resource error when number of LPB and FCH are limited
 *
 * Hydra_Software_Devel/77   2/9/07 5:31p syang
 * PR 27054: don't build rul when op=eReleaseHandle. (why we see some
 * module enabled in shadow register when other modules in vnet are not
 * yet???)
 *
 * Hydra_Software_Devel/76   2/5/07 4:36p pntruong
 * PR26677, PR23236:  Allow capture of 1600x1200p.
 *
 * Hydra_Software_Devel/75   1/22/07 3:20p syang
 * PR 27054: only 1 win build rul
 *
 * Hydra_Software_Devel/74   1/13/07 2:00p yuxiaz
 * PR20875: Remove BVDC_P_SUPPORT_BVN_10BIT_422, which is used to test 10
 * bit support on 3560B0.
 *
 * Hydra_Software_Devel/73   1/10/07 10:58a syang
 * PR 22569:  add new vnet algorithm code to sub-modules such as scl, mad,
 * dnr, mfd, vfd, cap, ...
 *
 * Hydra_Software_Devel/72   12/18/06 11:22p pntruong
 * PR22577: Merged back to mainline.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/5   11/16/06 1:52p hongtaoz
 * PR25668: merge in vdc support for 7403;
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/4   11/13/06 3:31p hongtaoz
 * PR20716: separate displays/compsoitors for loops;
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/3   11/9/06 2:14p yuxiaz
 * PR23249: Fixed VxWorks warnings on 97401.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/2   9/28/06 4:15p yuxiaz
 * PR20875: Added 10bit BVN support for 3560 B0.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/1   9/27/06 4:18p pntruong
 * PR23222, PR23225:  Added vdec's input validations.
 *
 * Hydra_Software_Devel/69   9/18/06 4:17p hongtaoz
 * PR24334: support up to 1080p capture;
 *
 * Hydra_Software_Devel/68   8/28/06 1:44p yuxiaz
 * PR20875: Renamed BVDC_P_SUPPORT_10BIT_BVN to
 * BVDC_P_SUPPORT_BVN_10BIT_444. Removed BVDC_P_BVNDataMode, use window
 * and buffer node's pixel format. Integrated with new 10 bit pixel
 * formats.
 *
 * Hydra_Software_Devel/67   8/21/06 3:30p yuxiaz
 * PR20875: Added 10bit BVN support for capture and feeder.
 *
 * Hydra_Software_Devel/66   8/15/06 6:15p hongtaoz
 * PR18233, PR20716: added support for mosaic mode - missing channels;
 * assume picture list's channel id are in ascending order; single
 * channel will not result in full-screen automatically;
 *
 * Hydra_Software_Devel/65   8/10/06 2:28p yuxiaz
 * PR20875: Consolidate BVN pixel format to support 10 bit 444 bvn path.
 *
 * Hydra_Software_Devel/64   8/7/06 3:52p hongtaoz
 * PR18233: mosaic mode doesn't build timestamp capture rul for slave
 * slots; capture drain by top/left clip to generate capture trigger at
 * the end of input picture to avoid incomplete mfd scanouts;
 *
 * Hydra_Software_Devel/63   7/21/06 3:32p hongtaoz
 * PR22811: consolidate scratch registers assignments;
 *
 * Hydra_Software_Devel/62   7/7/06 4:12p yuxiaz
 * PR 22296, PR 22108, PR22109: Removed
 * BVDC_Window_InstallLipSyncDelayCallback, added new generic window
 * callback function BVDC_Window_InstallCallback for lip sync.
 *
 * Hydra_Software_Devel/61   6/16/06 5:46p hongtaoz
 * PR21804: disable mosaic mode if no picture list is passed from decoder
 * for smoother transition;
 *
 * Hydra_Software_Devel/60   6/15/06 4:44p hongtaoz
 * PR20716, PR21804: support individual mosaic rectangles visibility;
 *
 * Hydra_Software_Devel/59   6/14/06 6:54p hongtaoz
 * PR20716, PR21804: implement modified version of mosaic mode;
 *
 * Hydra_Software_Devel/58   6/2/06 3:24p yuxiaz
 * PR21892: 7401 B0 bring up: Use VDC_P_SUPPORT_BVB_IN_SIZE. Add
 * BVB_IN_SIZE and PIC_OFFSET for scaler.
 *
 * Hydra_Software_Devel/57   6/1/06 4:54p yuxiaz
 * PR21892: 7401 B0 bring up: added in cap_0_bvb_in_size.
 *
 * Hydra_Software_Devel/56   5/26/06 3:37p pntruong
 * PR20642: Refactored handling of hList.
 *
 * Hydra_Software_Devel/55   5/15/06 12:22p jessem
 * PR 17530: Added instruction to capture RUL such that the timer register
 * is read during RUL execution time instead of using BTMR_ReadTimer_isr.
 * Added BTMR_TimerRegister field to BVDC_P_Capture_Context.
 *
 * Hydra_Software_Devel/54   5/10/06 3:15p jessem
 * PR 17530: Added BTMR support. BVDC_P_Capture_BuildRul_isr stores
 * timestamp for use by BVDC_P_Window_Writer_isr. BVDC_P_Capture_Create
 * takes in 2 new parameters:BREG_Handle and BTMR_TimerHandle.
 *
 * Hydra_Software_Devel/53   1/27/06 1:17p jessem
 * PR 18084, PR 18021: Replaced BVDC_P_Capture_SetPacking_isr with
 * BVDC_P_Capture_SetCapturePixelFormat_isr to determine the correct
 * video feeder packing format from a given pixel format. This allows
 * apps to change the pixel format on the fly.
 *
 * Hydra_Software_Devel/52   1/12/06 1:32p hongtaoz
 * PR18233: added mosaic mode support;
 *
 * Hydra_Software_Devel/MosaicMode_Feature_PR18233/3   1/12/06 11:34a hongtaoz
 * PR18233: modified mosaic mode API to be more flexible;
 *
 * Hydra_Software_Devel/MosaicMode_Feature_PR18233/2   12/16/05 5:05p hongtaoz
 * PR18233: added simul windows mosaic mode support;
 *
 * Hydra_Software_Devel/MosaicMode_Feature_PR18233/1   12/8/05 6:50p hongtaoz
 * PR18233: add Mosaic mode support (single window working);
 *
 * Hydra_Software_Devel/51   8/18/05 1:13p pntruong
 * PR15757, PR16391, PR16411, PR12519, PR14791, PR15535, PR15206, PR15778:
 * Improved bandwidth for cropping/scaler/capture/playback.   Unified
 * window shutdown sequence for destroy, reconfigure mad/scaler, and/or
 * reconfigure result of source changes.  And miscellances fixes from
 * above PRs.
 *
 * Hydra_Software_Devel/XVDPhase1/XVDPhase1_merge/1   7/14/05 7:57p pntruong
 * PR12519: Take in changes from mainline before merge back.
 *
 * Hydra_Software_Devel/XVDPhase1/1   6/30/05 2:44p pntruong
 * PR15284: Used the recommended 32-byte align pitch/stride for optimal
 * memory bandwidth utilization.
 *
 * Hydra_Software_Devel/49   4/29/05 12:14p hongtaoz
 * PR14798: added vbi pass-through support for 7038Cx and 3560;
 *
 * Hydra_Software_Devel/48   4/15/05 7:58p hongtaoz
 * PR14691: reset video window's path when shutdown and create;
 *
 * Hydra_Software_Devel/47   4/14/05 6:46p syang
 * PR 14692: use BVDC_P_SUPPORT_CAPTURE_CLIP macro
 *
 * Hydra_Software_Devel/46   4/14/05 10:23a syang
 * PR 14692: re-organize picture's rectangles, fixed duplicated clips in
 * diff modules, changed to do clip as soon as possible, and added
 * capture clipping for C0
 *
 * Hydra_Software_Devel/45   12/2/04 4:28p pntruong
 * PR12854: Repeated display format changes causes loss of sync and
 * flashing video.
 *
 * Hydra_Software_Devel/44   11/23/04 8:53p pntruong
 * PR13076, PR11749: Video jitter under heavy system load.  Added RUL
 * execution check to reduce number of programmed registers.
 *
 * Hydra_Software_Devel/43   11/19/04 2:43p yuxiaz
 * PR 10855: Allow user to specify number of capture buffers in VDC
 * default settings. Additional work needed.
 *
 * Hydra_Software_Devel/42   11/10/04 2:38p hongtaoz
 * PR13163: simplified vbi pass-through code;
 *
 * Hydra_Software_Devel/41   11/2/04 5:28p pntruong
 * PR13076: Added cached memory support for rul.
 *
 * Hydra_Software_Devel/40   10/13/04 7:59p hongtaoz
 * PR12162: added VBI pass-thru support with software workaround for
 * B0/B1;
 *
 * Hydra_Software_Devel/39   10/8/04 3:27p yuxiaz
 * PR9076: Change isr functions to _isr.
 *
 * Hydra_Software_Devel/38   7/16/04 6:51p hongtaoz
 * PR11168: adding VBI pass through support;
 *
 * Hydra_Software_Devel/37   7/15/04 5:45p pntruong
 * PR11887: Simplify the computation of rectangles in VDC.
 *
 * Hydra_Software_Devel/36   7/6/04 8:28a pntruong
 * PR9957: Prevent create/destroy from effecting hardware in VDC.
 *
 * Hydra_Software_Devel/35   6/22/04 2:10p yuxiaz
 * PR 11612: Fixed mutil-buffering after mode switching. Fixed pitch for
 * capture and playback.
 *
 * Hydra_Software_Devel/34   6/17/04 6:48p hongtaoz
 * PR11389: analog sourced video window does not handle AllDestination
 * mode aspect ratio correction or source clipping properly;
 *
 * Hydra_Software_Devel/33   5/24/04 5:09p jasonh
 * PR 11189: Merge down from B0 to main-line
 *
 * Hydra_Software_Devel/Refsw_Devel_7038_B0/1   5/21/04 6:57p hongtaoz
 * PR10944: added MAD32 support of 480i->480p for mpeg and analog source;
 * PR11103: added two default HD/SD color matrix enum settings into vdc
 * default settings;
 *
 * Hydra_Software_Devel/32   3/30/04 6:49p pntruong
 * PR10380: Replace set field macros with mask and set operation.
 * Additional optimization to code.
 *
 * Hydra_Software_Devel/31   3/11/04 4:30p pntruong
 * PR10058: Capture needs to reverse byte order on big endian system.
 *
 * Hydra_Software_Devel/30   3/2/04 4:08p hongtaoz
 * PR9950: When Single Mpeg Feeder feeds two displays, the sync-slipped
 * window close cause the sync-locked window hang. BVDC_P_Capture_Destroy
 * shall not directly write capture engine ctrl register to disable it,
 * otherwise causes the upstream BVN path to hang.
 *
 * Hydra_Software_Devel/29   2/26/04 3:20p pntruong
 * PR9901: Need to properly propagate error in VDC.
 *
 * Hydra_Software_Devel/28   2/25/04 2:27p pntruong
 * PR9790: BVDC_P_Scaler_SetInfo_isr will fail with floating point
 * exception if decode window size is set to 1,1.  Checked for odd pixel
 * error.
 *
 * Hydra_Software_Devel/27   1/14/04 4:38p yuxiaz
 * PR 9076: Change isr functions to _isr.
 *
 * Hydra_Software_Devel/26   1/7/04 6:31p pntruong
 * PR 9106: Added progressive Rul/Slot for each display.
 *
 * Hydra_Software_Devel/25   12/19/03 4:35p pntruong
 * PR9073: Fix interrupt abstraction for MVD to VDC.
 *
 * Hydra_Software_Devel/24   12/17/03 3:58p pntruong
 * PR 8905: Fixed overrun capture vsize.
 *
 * Hydra_Software_Devel/23   12/8/03 9:49p pntruong
 * PR 8841:  Fixed progressive output.  Added scale up coefficients.
 *
 * Hydra_Software_Devel/22   11/14/03 2:26p pntruong
 * Added capture buffers for multi-buffering (preliminary).
 *
 * Hydra_Software_Devel/21   11/7/03 10:02a pntruong
 * Removed useless error check.
 *
 * Hydra_Software_Devel/20   10/31/03 3:48p yuxiaz
 * Added pCaptureOutput, changed source width and source height to
 * stSourceRect in BVDC_P_PictureNode.
 *
 * Hydra_Software_Devel/19   10/30/03 6:20p pntruong
 * Used vnet mode.
 *
 * Hydra_Software_Devel/18   10/16/03 4:17p pntruong
 * Fixed usage of BDBG_ENTER(), BDBG_LEAVE().
 *
 * Hydra_Software_Devel/17   10/14/03 11:19a pntruong
 * Warning police.
 *
 * Hydra_Software_Devel/16   10/7/03 3:25p pntruong
 * Changed destroy to have a void return type.
 *
 * Hydra_Software_Devel/15   10/6/03 4:15p pntruong
 * Renamed BVDC_P_Source_Info to BVDC_P_Source_InfoPtr to comply with
 * linux build.
 *
 * Hydra_Software_Devel/14   10/6/03 2:48p yuxiaz
 * Added bvdc_common_priv.h, clean up private include files.
 *
 * Hydra_Software_Devel/13   9/26/03 1:39p pntruong
 * Added memory handle for BMEM to converting to offset.
 *
 * Hydra_Software_Devel/12   9/25/03 5:54p pntruong
 * Simplified rectangle declarations.
 *
 * Hydra_Software_Devel/11   9/10/03 6:31p pntruong
 * Added set info.
 *
 * Hydra_Software_Devel/10   9/8/03 4:26p pntruong
 * Added BVDC_P_Packing for video feeder and capture.
 *
 * Hydra_Software_Devel/9   9/5/03 5:00p pntruong
 * Removed unneeded code.
 *
 * Hydra_Software_Devel/8   9/5/03 5:00p pntruong
 * Used unify unsigned int and uint32_t, so we won't have to do uncessarry
 * converting.
 *
 * Hydra_Software_Devel/7   8/29/03 3:42p pntruong
 * Removed warnings.
 *
 * Hydra_Software_Devel/6   8/22/03 12:00p pntruong
 * Added create and destroy.
 *
 * Hydra_Software_Devel/5   7/28/03 11:29a pntruong
 * added BDBG_MODULE(BVDC).
 *
 * Hydra_Software_Devel/4   7/17/03 5:53p pntruong
 * Added source handle stuffs.  Checkin for Steven to add gfx feeder
 * source.
 *
 * Hydra_Software_Devel/3   7/10/03 10:34a pntruong
 * Changed BRDC_List_GetAddress to BRDC_List_GetStartAddress.
 *
 * Hydra_Software_Devel/2   7/2/03 5:10p pntruong
 * Checkin for the long weekend.
 *
 * Hydra_Software_Devel/1   6/27/03 6:19p pntruong
 * Initial version with mostly stub private functions.
 *
 ***************************************************************************/
#include "bstd.h"
#include "bvdc.h"
#include "brdc.h"
#include "bkni.h"
#include "bvdc_priv.h"
#include "bvdc_common_priv.h"
#include "bvdc_window_priv.h"
#include "bvdc_compositor_priv.h"
#include "bvdc_capture_priv.h"
#include "bvdc_buffer_priv.h"
#include "bvdc_vnet_priv.h"
#include "bvdc_window_priv.h"
#include "bvdc_source_priv.h"
#include "bvdc_feeder_priv.h"
#include "bvdc_mad_priv.h"
#include "bchp_bmisc.h"
#include "bchp_timer.h"


BDBG_MODULE(BVDC_CAP);
BDBG_OBJECT_ID(BVDC_CAP);

/* Dither settings for CAP */
#define BVDC_P_DITHER_CAP_LFSR_VALUE                 (0x763E6)
#define BVDC_P_DITHER_CAP_LFSR_CTRL_T1               (0x7)
#define BVDC_P_DITHER_CAP_LFSR_CTRL_T2               (0x7)
#define BVDC_P_DITHER_CAP0_LFSR_CTRL_T0              (0x7)
#define BVDC_P_DITHER_CAP1_LFSR_CTRL_T0              (0x6)
#define BVDC_P_DITHER_CAP2_LFSR_CTRL_T0              (0x1)

/***************************************************************************
 * {private}
 *
 */
BERR_Code BVDC_P_Capture_Create
	( BVDC_P_Capture_Handle           *phCapture,
	  BRDC_Handle                      hRdc,
	  BREG_Handle                      hRegister,
	  BVDC_P_CaptureId                 eCaptureId,
	  BTMR_TimerHandle                 hTimer,
	  BVDC_P_Resource_Handle           hResource )
{
	BVDC_P_CaptureContext *pCapture;

	BDBG_ENTER(BVDC_P_Capture_Create);

	BDBG_ASSERT(phCapture);
	BDBG_ASSERT(hRegister);

	/* The handle will be NULL if create fails. */
	*phCapture = NULL;

	/* Alloc the context. */
	pCapture = (BVDC_P_CaptureContext*)
		(BKNI_Malloc(sizeof(BVDC_P_CaptureContext)));
	if(!pCapture)
	{
		return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
	}

	/* Clear out the context and set defaults. */
	BKNI_Memset((void*)pCapture, 0x0, sizeof(BVDC_P_CaptureContext));
	BDBG_OBJECT_SET(pCapture, BVDC_CAP);

	pCapture->eId                  = eCaptureId;
	pCapture->eTrig                = BRDC_Trigger_eCap0Trig0 + eCaptureId * 2;
	pCapture->hRegister            = hRegister;
	pCapture->hTimer               = hTimer;
	pCapture->hRdc                 = hRdc;
	pCapture->ulTimestamp          = 0;
	BTMR_GetTimerRegisters(pCapture->hTimer, &pCapture->stTimerReg);

	switch(eCaptureId)
	{
	case BVDC_P_CaptureId_eCap0:
		pCapture->ulRegOffset = 0;
		break;
#if BCHP_CAP_1_REG_START
	case BVDC_P_CaptureId_eCap1:
		pCapture->ulRegOffset = BCHP_CAP_1_REG_START - BCHP_CAP_0_REG_START;
		break;
#endif

#if BCHP_CAP_2_REG_START
	case BVDC_P_CaptureId_eCap2:
		pCapture->ulRegOffset = BCHP_CAP_2_REG_START - BCHP_CAP_0_REG_START;
		break;
#endif

#if BCHP_CAP_3_REG_START
	case BVDC_P_CaptureId_eCap3:
		pCapture->ulRegOffset = BCHP_CAP_3_REG_START - BCHP_CAP_0_REG_START;
		break;
#endif

#if BCHP_CAP_4_REG_START
	case BVDC_P_CaptureId_eCap4:
		pCapture->ulRegOffset = BCHP_CAP_4_REG_START - BCHP_CAP_0_REG_START;
		break;
#endif

#if BCHP_CAP_5_REG_START
	case BVDC_P_CaptureId_eCap5:
		pCapture->ulRegOffset = BCHP_CAP_5_REG_START - BCHP_CAP_0_REG_START;
		break;
#endif

#if BCHP_CAP_6_REG_START
	case BVDC_P_CaptureId_eCap6:
		pCapture->ulRegOffset = BCHP_CAP_6_REG_START - BCHP_CAP_0_REG_START;
		break;
#endif

#if BCHP_CAP_7_REG_START
	case BVDC_P_CaptureId_eCap7:
		pCapture->ulRegOffset = BCHP_CAP_7_REG_START - BCHP_CAP_0_REG_START;
		break;
#endif
	default:
		BDBG_ASSERT(0);
	}

	/* Capture reset address */
#if BVDC_P_SUPPORT_NEW_SW_INIT
	pCapture->ulResetRegAddr = BCHP_BMISC_SW_INIT;
	pCapture->ulResetMask    = BCHP_BMISC_SW_INIT_CAP_0_MASK << (pCapture->eId);
#else
	pCapture->ulResetRegAddr = BCHP_BMISC_SOFT_RESET;
	pCapture->ulResetMask    = BCHP_BMISC_SOFT_RESET_CAP_0_MASK << (pCapture->eId);
#endif

	/* init the SubRul sub-module */
	BVDC_P_SubRul_Init(&(pCapture->SubRul), BVDC_P_Capture_MuxAddr(pCapture),
		0, BVDC_P_DrainMode_eNone, 0, hResource);

	/* All done. now return the new fresh context to user. */
	*phCapture = (BVDC_P_Capture_Handle)pCapture;

	BDBG_LEAVE(BVDC_P_Capture_Create);
	return BERR_SUCCESS;
}


/***************************************************************************
 * {private}
 *
 */
void BVDC_P_Capture_Destroy
	( BVDC_P_Capture_Handle            hCapture )
{
	BDBG_ENTER(BVDC_P_Capture_Destroy);
	BDBG_OBJECT_ASSERT(hCapture, BVDC_CAP);

	/* Turn off the capture. */
	BVDC_P_CAP_GET_REG_DATA(CAP_0_CTRL) &= ~(
#if (BVDC_P_SUPPORT_CAP_VER >= BVDC_P_CAP_VER_6)
		BCHP_MASK(CAP_0_CTRL, ENABLE_CTRL) |
#endif
		BCHP_MASK(CAP_0_CTRL, CAP_ENABLE));

	BVDC_P_CAP_GET_REG_DATA(CAP_0_CTRL) |=  (
#if (BVDC_P_SUPPORT_CAP_VER >= BVDC_P_CAP_VER_6)
		BCHP_FIELD_ENUM(CAP_0_CTRL, ENABLE_CTRL, ENABLE_BY_PICTURE) |
#endif
		BCHP_FIELD_ENUM(CAP_0_CTRL, CAP_ENABLE, DISABLE));

	BDBG_OBJECT_DESTROY(hCapture, BVDC_CAP);
	/* Release context in system memory */
	BKNI_Free((void*)hCapture);

	BDBG_LEAVE(BVDC_P_Capture_Destroy);
	return;
}


/***************************************************************************
 * {private}
 *
 */
void BVDC_P_Capture_Init
	( BVDC_P_Capture_Handle            hCapture )
{
	uint32_t  ulLfsrCtrlT0, ulLfsrCtrlT1, ulLfsrCtrlT2, ulLfsrValue;

	BDBG_ENTER(BVDC_P_Capture_Init);
	BDBG_OBJECT_ASSERT(hCapture, BVDC_CAP);

	/* Clear out shadow registers. */
	BKNI_Memset((void*)hCapture->aulRegs, 0x0, sizeof(hCapture->aulRegs));

	BVDC_P_CAP_GET_REG_DATA(CAP_0_RX_CTRL) &= ~(
		BCHP_MASK(CAP_0_RX_CTRL, PADDING_MODE));

	BVDC_P_CAP_GET_REG_DATA(CAP_0_RX_CTRL) |= (
		BCHP_FIELD_ENUM(CAP_0_RX_CTRL, PADDING_MODE, DISABLE));

	/* Initialize state. */
	hCapture->bInitial = true;
	hCapture->eCapDataMode = BVDC_P_Capture_DataMode_e8Bit422;

	hCapture->stDnSampler.eFilterType = BVDC_444To422Filter_eStandard;
	hCapture->stDnSampler.eRingRemoval = BVDC_RingSuppressionMode_eDisable;

	/* dither */
	if(hCapture->eId == BVDC_P_CaptureId_eCap1)
	{
		ulLfsrCtrlT0 = BVDC_P_DITHER_CAP1_LFSR_CTRL_T0;
	}
	else if(hCapture->eId == BVDC_P_CaptureId_eCap2)
	{
		ulLfsrCtrlT0 = BVDC_P_DITHER_CAP2_LFSR_CTRL_T0;
	}
	else
	{
		ulLfsrCtrlT0 = BVDC_P_DITHER_CAP0_LFSR_CTRL_T0;
	}
	ulLfsrValue  = BVDC_P_DITHER_CAP_LFSR_VALUE;
	ulLfsrCtrlT1 = BVDC_P_DITHER_CAP_LFSR_CTRL_T1;
	ulLfsrCtrlT2 = BVDC_P_DITHER_CAP_LFSR_CTRL_T2;

	BVDC_P_Dither_Init(&hCapture->stDither, ulLfsrCtrlT0,
		ulLfsrCtrlT1, ulLfsrCtrlT2, ulLfsrValue);

#if (BVDC_P_SUPPORT_CAP_VER == BVDC_P_CAP_VER_3)
	BVDC_P_CAP_GET_REG_DATA(CAP_0_DITHER_LFSR_INIT) &= ~(
		BCHP_MASK(CAP_0_DITHER_LFSR_INIT, SEQ) |
		BCHP_MASK(CAP_0_DITHER_LFSR_INIT, VALUE));
	BVDC_P_CAP_GET_REG_DATA(CAP_0_DITHER_LFSR_INIT) |=  (
		BCHP_FIELD_DATA(CAP_0_DITHER_LFSR_INIT, SEQ,   hCapture->stDither.ulLfsrSeq  ) |
		BCHP_FIELD_DATA(CAP_0_DITHER_LFSR_INIT, VALUE, hCapture->stDither.ulLfsrValue));

	BVDC_P_CAP_GET_REG_DATA(CAP_0_DITHER_LFSR_CTRL) &= ~(
		BCHP_MASK(CAP_0_DITHER_LFSR_CTRL, T0) |
		BCHP_MASK(CAP_0_DITHER_LFSR_CTRL, T1) |
		BCHP_MASK(CAP_0_DITHER_LFSR_CTRL, T2));
	BVDC_P_CAP_GET_REG_DATA(CAP_0_DITHER_LFSR_CTRL) |=  (
		BCHP_FIELD_DATA(CAP_0_DITHER_LFSR_CTRL, T0, hCapture->stDither.ulLfsrCtrlT0) |
		BCHP_FIELD_DATA(CAP_0_DITHER_LFSR_CTRL, T1, hCapture->stDither.ulLfsrCtrlT1) |
		BCHP_FIELD_DATA(CAP_0_DITHER_LFSR_CTRL, T2, hCapture->stDither.ulLfsrCtrlT2));

	BVDC_P_CAP_GET_REG_DATA(CAP_0_DITHER_CTRL) &= ~(
		BCHP_MASK(CAP_0_DITHER_CTRL, MODE      ) |
		BCHP_MASK(CAP_0_DITHER_CTRL, OFFSET_CH1) |
		BCHP_MASK(CAP_0_DITHER_CTRL, SCALE_CH1 ) |
		BCHP_MASK(CAP_0_DITHER_CTRL, OFFSET_CH0) |
		BCHP_MASK(CAP_0_DITHER_CTRL, SCALE_CH0 ));
	BVDC_P_CAP_GET_REG_DATA(CAP_0_DITHER_CTRL) |=  (
		BCHP_FIELD_DATA(CAP_0_DITHER_CTRL, MODE,       hCapture->stDither.ulMode     ) |
		BCHP_FIELD_DATA(CAP_0_DITHER_CTRL, OFFSET_CH1, hCapture->stDither.ulCh1Offset) |
		BCHP_FIELD_DATA(CAP_0_DITHER_CTRL, SCALE_CH1,  hCapture->stDither.ulCh1Scale ) |
		BCHP_FIELD_DATA(CAP_0_DITHER_CTRL, OFFSET_CH0, hCapture->stDither.ulCh0Offset) |
		BCHP_FIELD_DATA(CAP_0_DITHER_CTRL, SCALE_CH0,  hCapture->stDither.ulCh0Scale ));
#endif

#if (BVDC_P_SUPPORT_VIDEO_TESTFEATURE1_CAP)

	BKNI_Memset((void*)&hCapture->stTestFeature1, 0x0,
		sizeof(BVDC_P_TestFeature1_Settings));

	if(hCapture->eId == BVDC_P_CaptureId_eCap0)
	{
		/* CAP_0_DCE_PRED_CFG */
		BVDC_P_CAP_GET_REG_DATA(CAP_0_DCE_PRED_CFG) &= ~(
			BCHP_MASK(CAP_0_DCE_PRED_CFG, ENABLE          ) |
			BCHP_MASK(CAP_0_DCE_PRED_CFG, CONVERT_RGB     ) |
			BCHP_MASK(CAP_0_DCE_PRED_CFG, PREDICTION_MODE ) |
			BCHP_MASK(CAP_0_DCE_PRED_CFG, EDGE_PRED_ENA   ) |
			BCHP_MASK(CAP_0_DCE_PRED_CFG, LEFT_PRED_ENA   ) |
			BCHP_MASK(CAP_0_DCE_PRED_CFG, ABCD_PRED_ENA   ) |
			BCHP_MASK(CAP_0_DCE_PRED_CFG, LS_PRED_ENA     ));
		BVDC_P_CAP_GET_REG_DATA(CAP_0_DCE_PRED_CFG) |=  (
			BCHP_FIELD_ENUM(CAP_0_DCE_PRED_CFG, ENABLE,          Disable    ) |
			BCHP_FIELD_ENUM(CAP_0_DCE_PRED_CFG, CONVERT_RGB,     Disable    ) |
			BCHP_FIELD_ENUM(CAP_0_DCE_PRED_CFG, PREDICTION_MODE, Normal_Mode) |
			BCHP_FIELD_ENUM(CAP_0_DCE_PRED_CFG, EDGE_PRED_ENA,   Enable     ) |
			BCHP_FIELD_ENUM(CAP_0_DCE_PRED_CFG, LEFT_PRED_ENA,   Enable     ) |
			BCHP_FIELD_ENUM(CAP_0_DCE_PRED_CFG, ABCD_PRED_ENA,   Enable     ) |
			BCHP_FIELD_ENUM(CAP_0_DCE_PRED_CFG, LS_PRED_ENA,     Enable     ));

#if (BVDC_P_DCX_CAP_OVERFLOW_WORKAROUND)
		/* Limit buffer size to 4Kbits to workaround cap overflow bug */
		/* CAP_0_DCE_COMPR_CFG2 */
		BVDC_P_CAP_GET_REG_DATA(CAP_0_DCE_COMPR_CFG2) &= ~(
			BCHP_MASK(CAP_0_DCE_COMPR_CFG2, OFLOW_AVOID       ) |
			BCHP_MASK(CAP_0_DCE_COMPR_CFG2, RC_EDGE_FACTOR    ) |
			BCHP_MASK(CAP_0_DCE_COMPR_CFG2, QUANT_INCR_LIMIT1 ) |
			BCHP_MASK(CAP_0_DCE_COMPR_CFG2, QUANT_INCR_LIMIT0 ));
		BVDC_P_CAP_GET_REG_DATA(CAP_0_DCE_COMPR_CFG2) |=  (
			BCHP_FIELD_DATA(CAP_0_DCE_COMPR_CFG2, OFLOW_AVOID,       11 ) |
			BCHP_FIELD_DATA(CAP_0_DCE_COMPR_CFG2, RC_EDGE_FACTOR,    6  ) |
			BCHP_FIELD_DATA(CAP_0_DCE_COMPR_CFG2, QUANT_INCR_LIMIT1, 11 ) |
			BCHP_FIELD_DATA(CAP_0_DCE_COMPR_CFG2, QUANT_INCR_LIMIT0, 11 ));

		/* CAP_0_DCE_RANGE0 */
		BVDC_P_CAP_GET_REG_DATA(CAP_0_DCE_RANGE0) &= ~(
			BCHP_MASK(CAP_0_DCE_RANGE0, BPG_OFFSET ) |
			BCHP_MASK(CAP_0_DCE_RANGE0, MAX_QP     ) |
			BCHP_MASK(CAP_0_DCE_RANGE0, MIN_QP     ) |
			BCHP_MASK(CAP_0_DCE_RANGE0, THRESHOLD  ));
		BVDC_P_CAP_GET_REG_DATA(CAP_0_DCE_RANGE0) |=  (
			BCHP_FIELD_DATA(CAP_0_DCE_RANGE0, BPG_OFFSET, 20  ) |
			BCHP_FIELD_DATA(CAP_0_DCE_RANGE0, MAX_QP,     2   ) |
			BCHP_FIELD_DATA(CAP_0_DCE_RANGE0, MIN_QP,     0   ) |
			BCHP_FIELD_DATA(CAP_0_DCE_RANGE0, THRESHOLD,  448));

		/* CAP_0_DCE_RANGE1 */
		BVDC_P_CAP_GET_REG_DATA(CAP_0_DCE_RANGE1) &= ~(
			BCHP_MASK(CAP_0_DCE_RANGE1, BPG_OFFSET ) |
			BCHP_MASK(CAP_0_DCE_RANGE1, MAX_QP     ) |
			BCHP_MASK(CAP_0_DCE_RANGE1, MIN_QP     ) |
			BCHP_MASK(CAP_0_DCE_RANGE1, THRESHOLD  ));
		BVDC_P_CAP_GET_REG_DATA(CAP_0_DCE_RANGE1) |=  (
			BCHP_FIELD_DATA(CAP_0_DCE_RANGE1, BPG_OFFSET, 10  ) |
			BCHP_FIELD_DATA(CAP_0_DCE_RANGE1, MAX_QP,     2   ) |
			BCHP_FIELD_DATA(CAP_0_DCE_RANGE1, MIN_QP,     0   ) |
			BCHP_FIELD_DATA(CAP_0_DCE_RANGE1, THRESHOLD,  896));

		/* CAP_0_DCE_RANGE2 */
		BVDC_P_CAP_GET_REG_DATA(CAP_0_DCE_RANGE2) &= ~(
			BCHP_MASK(CAP_0_DCE_RANGE2, BPG_OFFSET ) |
			BCHP_MASK(CAP_0_DCE_RANGE2, MAX_QP     ) |
			BCHP_MASK(CAP_0_DCE_RANGE2, MIN_QP     ) |
			BCHP_MASK(CAP_0_DCE_RANGE2, THRESHOLD  ));
		BVDC_P_CAP_GET_REG_DATA(CAP_0_DCE_RANGE2) |=  (
			BCHP_FIELD_DATA(CAP_0_DCE_RANGE2, BPG_OFFSET, 0   ) |
			BCHP_FIELD_DATA(CAP_0_DCE_RANGE2, MAX_QP,     2   ) |
			BCHP_FIELD_DATA(CAP_0_DCE_RANGE2, MIN_QP,     1   ) |
			BCHP_FIELD_DATA(CAP_0_DCE_RANGE2, THRESHOLD,  1344));

		/* CAP_0_DCE_RANGE3 */
		BVDC_P_CAP_GET_REG_DATA(CAP_0_DCE_RANGE3) &= ~(
			BCHP_MASK(CAP_0_DCE_RANGE3, BPG_OFFSET ) |
			BCHP_MASK(CAP_0_DCE_RANGE3, MAX_QP     ) |
			BCHP_MASK(CAP_0_DCE_RANGE3, MIN_QP     ) |
			BCHP_MASK(CAP_0_DCE_RANGE3, THRESHOLD  ));
		BVDC_P_CAP_GET_REG_DATA(CAP_0_DCE_RANGE3) |=  (
			BCHP_FIELD_DATA(CAP_0_DCE_RANGE3, BPG_OFFSET, 0xfe) |
			BCHP_FIELD_DATA(CAP_0_DCE_RANGE3, MAX_QP,     3   ) |
			BCHP_FIELD_DATA(CAP_0_DCE_RANGE3, MIN_QP,     2   ) |
			BCHP_FIELD_DATA(CAP_0_DCE_RANGE3, THRESHOLD,  1792));

		/* CAP_0_DCE_RANGE4 */
		BVDC_P_CAP_GET_REG_DATA(CAP_0_DCE_RANGE4) &= ~(
			BCHP_MASK(CAP_0_DCE_RANGE4, BPG_OFFSET ) |
			BCHP_MASK(CAP_0_DCE_RANGE4, MAX_QP     ) |
			BCHP_MASK(CAP_0_DCE_RANGE4, MIN_QP     ) |
			BCHP_MASK(CAP_0_DCE_RANGE4, THRESHOLD  ));
		BVDC_P_CAP_GET_REG_DATA(CAP_0_DCE_RANGE4) |=  (
			BCHP_FIELD_DATA(CAP_0_DCE_RANGE4, BPG_OFFSET, 0xfc) |
			BCHP_FIELD_DATA(CAP_0_DCE_RANGE4, MAX_QP,     3   ) |
			BCHP_FIELD_DATA(CAP_0_DCE_RANGE4, MIN_QP,     2   ) |
			BCHP_FIELD_DATA(CAP_0_DCE_RANGE4, THRESHOLD,  2688));

		/* CAP_0_DCE_RANGE5 */
		BVDC_P_CAP_GET_REG_DATA(CAP_0_DCE_RANGE5) &= ~(
			BCHP_MASK(CAP_0_DCE_RANGE5, BPG_OFFSET ) |
			BCHP_MASK(CAP_0_DCE_RANGE5, MAX_QP     ) |
			BCHP_MASK(CAP_0_DCE_RANGE5, MIN_QP     ) |
			BCHP_MASK(CAP_0_DCE_RANGE5, THRESHOLD  ));
		BVDC_P_CAP_GET_REG_DATA(CAP_0_DCE_RANGE5) |=  (
			BCHP_FIELD_DATA(CAP_0_DCE_RANGE5, BPG_OFFSET, 0xfc) |
			BCHP_FIELD_DATA(CAP_0_DCE_RANGE5, MAX_QP,     7   ) |
			BCHP_FIELD_DATA(CAP_0_DCE_RANGE5, MIN_QP,     3   ) |
			BCHP_FIELD_DATA(CAP_0_DCE_RANGE5, THRESHOLD,  2912));

		/* CAP_0_DCE_RANGE6 */
		BVDC_P_CAP_GET_REG_DATA(CAP_0_DCE_RANGE6) &= ~(
			BCHP_MASK(CAP_0_DCE_RANGE6, BPG_OFFSET ) |
			BCHP_MASK(CAP_0_DCE_RANGE6, MAX_QP     ) |
			BCHP_MASK(CAP_0_DCE_RANGE6, MIN_QP     ) |
			BCHP_MASK(CAP_0_DCE_RANGE6, THRESHOLD  ));
		BVDC_P_CAP_GET_REG_DATA(CAP_0_DCE_RANGE6) |=  (
			BCHP_FIELD_DATA(CAP_0_DCE_RANGE6, BPG_OFFSET, 0xfa) |
			BCHP_FIELD_DATA(CAP_0_DCE_RANGE6, MAX_QP,     9   ) |
			BCHP_FIELD_DATA(CAP_0_DCE_RANGE6, MIN_QP,     3   ) |
			BCHP_FIELD_DATA(CAP_0_DCE_RANGE6, THRESHOLD,  3136));

		/* CAP_0_DCE_RANGE7 */
		BVDC_P_CAP_GET_REG_DATA(CAP_0_DCE_RANGE7) &= ~(
			BCHP_MASK(CAP_0_DCE_RANGE7, BPG_OFFSET ) |
			BCHP_MASK(CAP_0_DCE_RANGE7, MAX_QP     ) |
			BCHP_MASK(CAP_0_DCE_RANGE7, MIN_QP     ) |
			BCHP_MASK(CAP_0_DCE_RANGE7, THRESHOLD  ));
		BVDC_P_CAP_GET_REG_DATA(CAP_0_DCE_RANGE7) |=  (
			BCHP_FIELD_DATA(CAP_0_DCE_RANGE7, BPG_OFFSET, 0xfa) |
			BCHP_FIELD_DATA(CAP_0_DCE_RANGE7, MAX_QP,     10   ) |
			BCHP_FIELD_DATA(CAP_0_DCE_RANGE7, MIN_QP,     6   ) |
			BCHP_FIELD_DATA(CAP_0_DCE_RANGE7, THRESHOLD,  3416));

		/* CAP_0_DCE_RANGE8 */
		BVDC_P_CAP_GET_REG_DATA(CAP_0_DCE_RANGE8) &= ~(
			BCHP_MASK(CAP_0_DCE_RANGE8, BPG_OFFSET ) |
			BCHP_MASK(CAP_0_DCE_RANGE8, MAX_QP     ) |
			BCHP_MASK(CAP_0_DCE_RANGE8, MIN_QP     ) |
			BCHP_MASK(CAP_0_DCE_RANGE8, THRESHOLD  ));
		BVDC_P_CAP_GET_REG_DATA(CAP_0_DCE_RANGE8) |=  (
			BCHP_FIELD_DATA(CAP_0_DCE_RANGE8, BPG_OFFSET, 0xf8) |
			BCHP_FIELD_DATA(CAP_0_DCE_RANGE8, MAX_QP,     11   ) |
			BCHP_FIELD_DATA(CAP_0_DCE_RANGE8, MIN_QP,     6   ) |
			BCHP_FIELD_DATA(CAP_0_DCE_RANGE8, THRESHOLD,  3584));

		/* CAP_0_DCE_RANGE9 */
		BVDC_P_CAP_GET_REG_DATA(CAP_0_DCE_RANGE9) &= ~(
			BCHP_MASK(CAP_0_DCE_RANGE9, BPG_OFFSET ) |
			BCHP_MASK(CAP_0_DCE_RANGE9, MAX_QP     ) |
			BCHP_MASK(CAP_0_DCE_RANGE9, MIN_QP     ) |
			BCHP_MASK(CAP_0_DCE_RANGE9, THRESHOLD  ));
		BVDC_P_CAP_GET_REG_DATA(CAP_0_DCE_RANGE9) |=  (
			BCHP_FIELD_DATA(CAP_0_DCE_RANGE9, BPG_OFFSET, 0xf6) |
			BCHP_FIELD_DATA(CAP_0_DCE_RANGE9, MAX_QP,     11   ) |
			BCHP_FIELD_DATA(CAP_0_DCE_RANGE9, MIN_QP,     7   ) |
			BCHP_FIELD_DATA(CAP_0_DCE_RANGE9, THRESHOLD,  3752));

		/* CAP_0_DCE_RANGE10 */
		BVDC_P_CAP_GET_REG_DATA(CAP_0_DCE_RANGE10) &= ~(
			BCHP_MASK(CAP_0_DCE_RANGE10, BPG_OFFSET ) |
			BCHP_MASK(CAP_0_DCE_RANGE10, MAX_QP     ) |
			BCHP_MASK(CAP_0_DCE_RANGE10, MIN_QP     ) |
			BCHP_MASK(CAP_0_DCE_RANGE10, THRESHOLD  ));
		BVDC_P_CAP_GET_REG_DATA(CAP_0_DCE_RANGE10) |=  (
			BCHP_FIELD_DATA(CAP_0_DCE_RANGE10, BPG_OFFSET, 0xf6) |
			BCHP_FIELD_DATA(CAP_0_DCE_RANGE10, MAX_QP,     12   ) |
			BCHP_FIELD_DATA(CAP_0_DCE_RANGE10, MIN_QP,     9   ) |
			BCHP_FIELD_DATA(CAP_0_DCE_RANGE10, THRESHOLD,  3920));

		/* CAP_0_DCE_RANGE11 */
		BVDC_P_CAP_GET_REG_DATA(CAP_0_DCE_RANGE11) &= ~(
			BCHP_MASK(CAP_0_DCE_RANGE11, BPG_OFFSET ) |
			BCHP_MASK(CAP_0_DCE_RANGE11, MAX_QP     ) |
			BCHP_MASK(CAP_0_DCE_RANGE11, MIN_QP     ) |
			BCHP_MASK(CAP_0_DCE_RANGE11, THRESHOLD  ));
		BVDC_P_CAP_GET_REG_DATA(CAP_0_DCE_RANGE11) |=  (
			BCHP_FIELD_DATA(CAP_0_DCE_RANGE11, BPG_OFFSET, 0xf4) |
			BCHP_FIELD_DATA(CAP_0_DCE_RANGE11, MAX_QP,     14   ) |
			BCHP_FIELD_DATA(CAP_0_DCE_RANGE11, MIN_QP,     12   ) |
			BCHP_FIELD_DATA(CAP_0_DCE_RANGE11, THRESHOLD,  3976));

		/* CAP_0_DCE_RANGE12 */
		BVDC_P_CAP_GET_REG_DATA(CAP_0_DCE_RANGE12) &= ~(
			BCHP_MASK(CAP_0_DCE_RANGE12, BPG_OFFSET ) |
			BCHP_MASK(CAP_0_DCE_RANGE12, MAX_QP     ) |
			BCHP_MASK(CAP_0_DCE_RANGE12, MIN_QP     ) |
			BCHP_MASK(CAP_0_DCE_RANGE12, THRESHOLD  ));
		BVDC_P_CAP_GET_REG_DATA(CAP_0_DCE_RANGE12) |=  (
			BCHP_FIELD_DATA(CAP_0_DCE_RANGE12, BPG_OFFSET, 0xf4) |
			BCHP_FIELD_DATA(CAP_0_DCE_RANGE12, MAX_QP,     15   ) |
			BCHP_FIELD_DATA(CAP_0_DCE_RANGE12, MIN_QP,     14   ) |
			BCHP_FIELD_DATA(CAP_0_DCE_RANGE12, THRESHOLD,  4032));

		/* CAP_0_DCE_RANGE13 */
		BVDC_P_CAP_GET_REG_DATA(CAP_0_DCE_RANGE13) &= ~(
			BCHP_MASK(CAP_0_DCE_RANGE13, BPG_OFFSET ) |
			BCHP_MASK(CAP_0_DCE_RANGE13, MAX_QP     ) |
			BCHP_MASK(CAP_0_DCE_RANGE13, MIN_QP     ) |
			BCHP_MASK(CAP_0_DCE_RANGE13, THRESHOLD  ));
		BVDC_P_CAP_GET_REG_DATA(CAP_0_DCE_RANGE13) |=  (
			BCHP_FIELD_DATA(CAP_0_DCE_RANGE13, BPG_OFFSET, 0xf4) |
			BCHP_FIELD_DATA(CAP_0_DCE_RANGE13, MAX_QP,     16   ) |
			BCHP_FIELD_DATA(CAP_0_DCE_RANGE13, MIN_QP,     15   ) |
			BCHP_FIELD_DATA(CAP_0_DCE_RANGE13, THRESHOLD,  4060));

		/* CAP_0_DCE_RANGE14 */
		BVDC_P_CAP_GET_REG_DATA(CAP_0_DCE_RANGE14) &= ~(
			BCHP_MASK(CAP_0_DCE_RANGE14, BPG_OFFSET ) |
			BCHP_MASK(CAP_0_DCE_RANGE14, MAX_QP     ) |
			BCHP_MASK(CAP_0_DCE_RANGE14, MIN_QP     ));
		BVDC_P_CAP_GET_REG_DATA(CAP_0_DCE_RANGE14) |=  (
			BCHP_FIELD_DATA(CAP_0_DCE_RANGE14, BPG_OFFSET, 0xf4) |
			BCHP_FIELD_DATA(CAP_0_DCE_RANGE14, MAX_QP,     17   ) |
			BCHP_FIELD_DATA(CAP_0_DCE_RANGE14, MIN_QP,     17   ));
#endif
	}
#endif

	BDBG_LEAVE(BVDC_P_Capture_Init);
	return;
}

/***************************************************************************
 * {private}
 *
 * BVDC_P_Capture_BuildRul_DrainVnet_isr
 *
 * called by BVDC_P_Capture_BuildRul_isr after resetting to drain the module and
 * its pre-patch FreeCh or LpBack
 */
static void BVDC_P_Capture_BuildRul_DrainVnet_isr(
	BVDC_P_Capture_Handle              hCapture,
	BVDC_P_ListInfo                   *pList )
{
	/* reset sub */
	BVDC_P_SubRul_StartDrain_isr(&(hCapture->SubRul), pList,
		hCapture->ulResetRegAddr, hCapture->ulResetMask);

	/* drain the free-channel or loop-back used to to patch the vnet. */
	BVDC_P_SubRul_FinishDrain_isr(&(hCapture->SubRul), pList,
		hCapture->ulResetRegAddr, hCapture->ulResetMask, 0, 0);
}

/***************************************************************************
 * {private}
 *
 */
void BVDC_P_Capture_BuildRul_isr
	( const BVDC_P_Capture_Handle      hCapture,
	  BVDC_P_ListInfo                 *pList,
	  BVDC_P_State                     eVnetState,
	  BVDC_P_PicComRulInfo            *pPicComRulInfo )
{
	uint32_t  ulRulOpsFlags;

	BDBG_ENTER(BVDC_P_Capture_BuildRul_isr);
	BDBG_OBJECT_ASSERT(hCapture, BVDC_CAP);

	/* currently this is only for vnet building / tearing-off*/

	ulRulOpsFlags = BVDC_P_SubRul_GetOps_isr(
		&(hCapture->SubRul), pPicComRulInfo->eWin, eVnetState, pList->bLastExecuted);

	if ((0 == ulRulOpsFlags) ||
		(ulRulOpsFlags & BVDC_P_RulOp_eReleaseHandle))
		return;
	else if (ulRulOpsFlags & BVDC_P_RulOp_eDisable)
	{
		BVDC_P_SubRul_DropOffVnet_isr(&(hCapture->SubRul), pList);
		BVDC_P_Capture_SetEnable_isr(hCapture, false);
		/* Capture does not auto shut-off, need RDC to turn it off. */
		BVDC_P_CAP_WRITE_TO_RUL(CAP_0_CTRL, pList->pulCurrent);
	}

	/* reset */
	if(hCapture->bInitial)
	{
#if (BVDC_P_DCX_CAP_RESET_WORKAROUND)
		BVDC_P_BUILD_RESET(pList->pulCurrent,
			hCapture->ulResetRegAddr, hCapture->ulResetMask);
#endif
		hCapture->bInitial = false;
	}

	if (ulRulOpsFlags & BVDC_P_RulOp_eEnable)
	{
#if (BVDC_P_SUPPORT_VIDEO_TESTFEATURE1_CAP)
		if(hCapture->eId == BVDC_P_CaptureId_eCap0)
		{
			BVDC_P_CAP_BLOCK_WRITE_TO_RUL(CAP_0_DCE_PRED_CFG,
				CAP_0_DCE_COMPR_CFG1 , pList->pulCurrent);

#if (BVDC_P_DCX_CAP_OVERFLOW_WORKAROUND)
			BVDC_P_CAP_BLOCK_WRITE_TO_RUL(CAP_0_DCE_COMPR_CFG2,
				CAP_0_DCE_RANGE14 , pList->pulCurrent);
#endif
		}
#endif

#if (BVDC_P_SUPPORT_CAP_VER == BVDC_P_CAP_VER_0)
		BVDC_P_CAP_WRITE_TO_RUL(CAP_0_PIC_OFFSET, pList->pulCurrent);
		BVDC_P_CAP_WRITE_TO_RUL(CAP_0_PIC_SIZE, pList->pulCurrent);
		BVDC_P_CAP_BLOCK_WRITE_TO_RUL(CAP_0_MSTART, CAP_0_RX_CTRL , pList->pulCurrent);
#elif (BVDC_P_SUPPORT_CAP_VER == BVDC_P_CAP_VER_1)
		BVDC_P_CAP_BLOCK_WRITE_TO_RUL(CAP_0_PIC_OFFSET, CAP_0_BVB_IN_SIZE , pList->pulCurrent);
		BVDC_P_CAP_WRITE_TO_RUL(CAP_0_PIC_SIZE, pList->pulCurrent);
		BVDC_P_CAP_BLOCK_WRITE_TO_RUL(CAP_0_MSTART, CAP_0_RX_CTRL , pList->pulCurrent);
#elif (BVDC_P_SUPPORT_CAP_VER == BVDC_P_CAP_VER_2)
		BVDC_P_CAP_BLOCK_WRITE_TO_RUL(CAP_0_PIC_SIZE, CAP_0_PITCH, pList->pulCurrent);
		BVDC_P_CAP_BLOCK_WRITE_TO_RUL(CAP_0_BYTE_ORDER, CAP_0_LINE_CMP_TRIG_1_CFG, pList->pulCurrent);
#elif (BVDC_P_SUPPORT_CAP_VER == BVDC_P_CAP_VER_3)
		BVDC_P_CAP_BLOCK_WRITE_TO_RUL(CAP_0_PIC_SIZE, CAP_0_PITCH, pList->pulCurrent);
		BVDC_P_CAP_BLOCK_WRITE_TO_RUL(CAP_0_MODE, CAP_0_DITHER_CTRL, pList->pulCurrent);
#elif (BVDC_P_SUPPORT_CAP_VER >= BVDC_P_CAP_VER_4)
		BVDC_P_CAP_BLOCK_WRITE_TO_RUL(CAP_0_PIC_SIZE, CAP_0_PITCH, pList->pulCurrent);
		BVDC_P_CAP_BLOCK_WRITE_TO_RUL(CAP_0_MODE, CAP_0_LINE_CMP_TRIG_1_CFG, pList->pulCurrent);
#endif

#if (!BVDC_P_USE_RDC_TIMESTAMP)
		/* only the master RUL needs timestamp */
		if(pList->bMasterList)
		{
			/* Read and store timestamp */
			*pList->pulCurrent++ = BRDC_OP_REG_TO_VAR(BRDC_Variable_0);
			*pList->pulCurrent++ = BRDC_REGISTER(hCapture->stTimerReg.status);
			*pList->pulCurrent++ = BRDC_OP_IMM_TO_VAR(BRDC_Variable_1);
			*pList->pulCurrent++ = BCHP_TIMER_TIMER0_STAT_COUNTER_VAL_MASK;
			*pList->pulCurrent++ = BRDC_OP_VAR_AND_VAR_TO_VAR(BRDC_Variable_0, BRDC_Variable_1, BRDC_Variable_2);
			*pList->pulCurrent++ = BRDC_OP_VAR_TO_REG(BRDC_Variable_2);
			*pList->pulCurrent++ = BRDC_REGISTER(hCapture->ulTimestampRegAddr);
		}
#endif

		/* must be the last */
		BVDC_P_CAP_WRITE_TO_RUL(CAP_0_CTRL, pList->pulCurrent);

		/* join in vnet after enable. note: its src mux is initialed as disabled */
		if (ulRulOpsFlags & BVDC_P_RulOp_eVnetInit)
		{
			BVDC_P_SubRul_JoinInVnet_isr(&(hCapture->SubRul), pList);
		}
	}

	else if (ulRulOpsFlags & BVDC_P_RulOp_eDrainVnet)
	{
		BVDC_P_Capture_BuildRul_DrainVnet_isr(hCapture, pList);
	}

	BDBG_LEAVE(BVDC_P_Capture_BuildRul_isr);
	return;
}


/***************************************************************************
 * {private}
 *
 */
static BERR_Code BVDC_P_Capture_SetPictureRect_isr
	( BVDC_P_Capture_Handle            hCapture,
	  BVDC_P_Rect                     *pCapIn,
	  BVDC_P_Rect                     *pCapOut,
	  BAVC_Polarity                    eScanType )
{
	uint32_t ulWidth = pCapOut->ulWidth;
	uint32_t ulHeight = pCapOut->ulHeight;
	int32_t lLeft = pCapOut->lLeft;
	int32_t lTop = pCapOut->lTop;
	uint32_t ulCapInWidth = pCapIn->ulWidth;
	uint32_t ulCapInHeight = pCapIn->ulHeight;
#if (BVDC_P_SUPPORT_CAP_VER >= BVDC_P_CAP_VER_5)
	int32_t lLeft_R = pCapOut->lLeft_R;
#endif

	BDBG_ENTER(BVDC_P_Capture_SetPictureRect_isr);
	BDBG_OBJECT_ASSERT(hCapture, BVDC_CAP);

	/* for even pixel align, we can only drop pixel, not expand pixel width */
#if BVDC_P_SUPPORT_BVN_444
	if ((BVDC_P_Capture_DataMode_e8Bit422  == hCapture->eCapDataMode) ||
		(BVDC_P_Capture_DataMode_e10Bit422 == hCapture->eCapDataMode))
	{
		ulWidth = ulWidth & ~1;
	}
#else
	ulWidth = ulWidth & ~1;
	lLeft  = lLeft  & ~1;
	ulCapInWidth = ulCapInWidth & ~1;
#if (BVDC_P_SUPPORT_CAP_VER >= BVDC_P_CAP_VER_5)
	lLeft_R  = lLeft_R  & ~1;
#endif
#endif

	if(BAVC_Polarity_eFrame != eScanType)
	{
		ulHeight /= BVDC_P_FIELD_PER_FRAME;
		lTop /= BVDC_P_FIELD_PER_FRAME;
		ulCapInHeight /= BVDC_P_FIELD_PER_FRAME;
	}

	/* set capture size */
	BVDC_P_CAP_GET_REG_DATA(CAP_0_PIC_SIZE) &= ~(
		BCHP_MASK(CAP_0_PIC_SIZE, HSIZE) |
		BCHP_MASK(CAP_0_PIC_SIZE, VSIZE));

	BVDC_P_CAP_GET_REG_DATA(CAP_0_PIC_SIZE) |=  (
		BCHP_FIELD_DATA(CAP_0_PIC_SIZE, HSIZE, ulWidth) |
		BCHP_FIELD_DATA(CAP_0_PIC_SIZE, VSIZE, ulHeight));

	BVDC_P_CAP_GET_REG_DATA(CAP_0_PIC_OFFSET) &= ~(
		BCHP_MASK(CAP_0_PIC_OFFSET, HSIZE) |
		BCHP_MASK(CAP_0_PIC_OFFSET, VSIZE));

	BVDC_P_CAP_GET_REG_DATA(CAP_0_PIC_OFFSET) |=  (
		BCHP_FIELD_DATA(CAP_0_PIC_OFFSET, HSIZE, lLeft) |
		BCHP_FIELD_DATA(CAP_0_PIC_OFFSET, VSIZE, lTop));

#if (BVDC_P_SUPPORT_CAP_VER >= BVDC_P_CAP_VER_5)
	BVDC_P_CAP_GET_REG_DATA(CAP_0_PIC_OFFSET_R) &= ~(
		BCHP_MASK(CAP_0_PIC_OFFSET_R, ENABLE) |
		BCHP_MASK(CAP_0_PIC_OFFSET_R, HSIZE));

	BVDC_P_CAP_GET_REG_DATA(CAP_0_PIC_OFFSET_R) |=  (
		BCHP_FIELD_DATA(CAP_0_PIC_OFFSET_R, ENABLE, (lLeft != lLeft_R)) |
		BCHP_FIELD_DATA(CAP_0_PIC_OFFSET_R, HSIZE,  lLeft_R));
#endif

	BVDC_P_CAP_GET_REG_DATA(CAP_0_BVB_IN_SIZE) &= ~(
		BCHP_MASK(CAP_0_BVB_IN_SIZE, HSIZE) |
		BCHP_MASK(CAP_0_BVB_IN_SIZE, VSIZE));

	BVDC_P_CAP_GET_REG_DATA(CAP_0_BVB_IN_SIZE) |=  (
		BCHP_FIELD_DATA(CAP_0_BVB_IN_SIZE, HSIZE, ulCapInWidth) |
		BCHP_FIELD_DATA(CAP_0_BVB_IN_SIZE, VSIZE, ulCapInHeight));

	BDBG_LEAVE(BVDC_P_Capture_SetPictureRect_isr);
	return BERR_SUCCESS;
}


/***************************************************************************
 * {private}
 *
 */
BERR_Code BVDC_P_Capture_SetBuffer_isr
	( BVDC_P_Capture_Handle            hCapture,
	  uint32_t                         ulDeviceAddr,
	  uint32_t                         ulDeviceAddr_R,
	  uint32_t                         ulPitch )
{
	BDBG_ENTER(BVDC_P_Capture_SetBuffer_isr);
	BDBG_OBJECT_ASSERT(hCapture, BVDC_CAP);

#if (BVDC_P_SUPPORT_CAP_VER < BVDC_P_CAP_VER_4)
	BSTD_UNUSED(ulDeviceAddr_R);
#endif

	/* This should always be true! */
	BDBG_ASSERT(BVDC_P_IS_ALIGN(ulPitch, BVDC_P_PITCH_ALIGN));
	BDBG_ASSERT(BVDC_P_IS_ALIGN(ulDeviceAddr, BVDC_P_BUFFER_ALIGN));

	/* set mstart */
	BVDC_P_CAP_GET_REG_DATA(CAP_0_MSTART) &= ~(
		BCHP_MASK(CAP_0_MSTART, MSTART));

	BVDC_P_CAP_GET_REG_DATA(CAP_0_MSTART) |=  (
		BCHP_FIELD_DATA(CAP_0_MSTART, MSTART, ulDeviceAddr));

#if (BVDC_P_SUPPORT_CAP_VER >= BVDC_P_CAP_VER_4)
	/* set mstart */
	BVDC_P_CAP_GET_REG_DATA(CAP_0_MSTART_R) &= ~(
		BCHP_MASK(CAP_0_MSTART_R, MSTART));

	BVDC_P_CAP_GET_REG_DATA(CAP_0_MSTART_R) |=  (
		BCHP_FIELD_DATA(CAP_0_MSTART_R, MSTART, ulDeviceAddr_R));
#endif

	/* set pitch */
	BVDC_P_CAP_GET_REG_DATA(CAP_0_PITCH) &= ~(
		BCHP_MASK(CAP_0_PITCH, PITCH));

	BVDC_P_CAP_GET_REG_DATA(CAP_0_PITCH) |=  (
		BCHP_FIELD_DATA(CAP_0_PITCH, PITCH, ulPitch));


	BDBG_LEAVE(BVDC_P_Capture_SetBuffer_isr);
	return BERR_SUCCESS;
}


/***************************************************************************
 * {private}
 * This replaces the deprecated BVDC_P_Capture_SetPacking_isr.
 */
static BERR_Code BVDC_P_Capture_SetPixelFormat_isr
	( BVDC_P_Capture_Handle            hCapture,
	  const BVDC_P_PictureNodePtr      pPicture )
{
	BPXL_Format    ePxlFormat;
#if(BVDC_P_SUPPORT_BVN_10BIT_444)
	BVDC_P_Capture_DataMode  eCapDataMode;
#endif

	BDBG_ENTER(BVDC_P_Capture_SetPixelFormat_isr);
	BDBG_OBJECT_ASSERT(hCapture, BVDC_CAP);

	ePxlFormat = pPicture->ePixelFormat;

	/* set byte order */
	BVDC_P_CAP_GET_REG_DATA(CAP_0_BYTE_ORDER) &= ~(
		BCHP_MASK(CAP_0_BYTE_ORDER, BYTE_3_SEL) |
		BCHP_MASK(CAP_0_BYTE_ORDER, BYTE_2_SEL) |
		BCHP_MASK(CAP_0_BYTE_ORDER, BYTE_1_SEL) |
		BCHP_MASK(CAP_0_BYTE_ORDER, BYTE_0_SEL));

	/* Always program CAP_0_BYTE_ORDER according to pixel format */
	switch(ePxlFormat)
	{
	default:
	case BPXL_eY18_Cb8_Y08_Cr8:
		BVDC_P_CAP_GET_REG_DATA(CAP_0_BYTE_ORDER) |=  (
			BCHP_FIELD_ENUM(CAP_0_BYTE_ORDER, BYTE_3_SEL, CR) |
			BCHP_FIELD_ENUM(CAP_0_BYTE_ORDER, BYTE_2_SEL, Y0) |
			BCHP_FIELD_ENUM(CAP_0_BYTE_ORDER, BYTE_1_SEL, CB) |
			BCHP_FIELD_ENUM(CAP_0_BYTE_ORDER, BYTE_0_SEL, Y1));
		break;

	case BPXL_eY08_Cb8_Y18_Cr8:
		BVDC_P_CAP_GET_REG_DATA(CAP_0_BYTE_ORDER) |=  (
			BCHP_FIELD_ENUM(CAP_0_BYTE_ORDER, BYTE_3_SEL, CR) |
			BCHP_FIELD_ENUM(CAP_0_BYTE_ORDER, BYTE_2_SEL, Y1) |
			BCHP_FIELD_ENUM(CAP_0_BYTE_ORDER, BYTE_1_SEL, CB) |
			BCHP_FIELD_ENUM(CAP_0_BYTE_ORDER, BYTE_0_SEL, Y0));
		break;

	case BPXL_eY18_Cr8_Y08_Cb8:
		BVDC_P_CAP_GET_REG_DATA(CAP_0_BYTE_ORDER) |=  (
			BCHP_FIELD_ENUM(CAP_0_BYTE_ORDER, BYTE_3_SEL, CB) |
			BCHP_FIELD_ENUM(CAP_0_BYTE_ORDER, BYTE_2_SEL, Y0) |
			BCHP_FIELD_ENUM(CAP_0_BYTE_ORDER, BYTE_1_SEL, CR) |
			BCHP_FIELD_ENUM(CAP_0_BYTE_ORDER, BYTE_0_SEL, Y1));
		break;

	case BPXL_eY08_Cr8_Y18_Cb8:
		BVDC_P_CAP_GET_REG_DATA(CAP_0_BYTE_ORDER) |=  (
			BCHP_FIELD_ENUM(CAP_0_BYTE_ORDER, BYTE_3_SEL, CB) |
			BCHP_FIELD_ENUM(CAP_0_BYTE_ORDER, BYTE_2_SEL, Y1) |
			BCHP_FIELD_ENUM(CAP_0_BYTE_ORDER, BYTE_1_SEL, CR) |
			BCHP_FIELD_ENUM(CAP_0_BYTE_ORDER, BYTE_0_SEL, Y0));
			break;

	case BPXL_eCr8_Y18_Cb8_Y08:
		BVDC_P_CAP_GET_REG_DATA(CAP_0_BYTE_ORDER) |=  (
			BCHP_FIELD_ENUM(CAP_0_BYTE_ORDER, BYTE_3_SEL, Y0) |
			BCHP_FIELD_ENUM(CAP_0_BYTE_ORDER, BYTE_2_SEL, CB) |
			BCHP_FIELD_ENUM(CAP_0_BYTE_ORDER, BYTE_1_SEL, Y1) |
			BCHP_FIELD_ENUM(CAP_0_BYTE_ORDER, BYTE_0_SEL, CR));
		break;

	case BPXL_eCr8_Y08_Cb8_Y18:
		BVDC_P_CAP_GET_REG_DATA(CAP_0_BYTE_ORDER) |=  (
			BCHP_FIELD_ENUM(CAP_0_BYTE_ORDER, BYTE_3_SEL, Y1) |
			BCHP_FIELD_ENUM(CAP_0_BYTE_ORDER, BYTE_2_SEL, CB) |
			BCHP_FIELD_ENUM(CAP_0_BYTE_ORDER, BYTE_1_SEL, Y0) |
			BCHP_FIELD_ENUM(CAP_0_BYTE_ORDER, BYTE_0_SEL, CR));
			break;

	case BPXL_eCb8_Y18_Cr8_Y08:
		BVDC_P_CAP_GET_REG_DATA(CAP_0_BYTE_ORDER) |=  (
			BCHP_FIELD_ENUM(CAP_0_BYTE_ORDER, BYTE_3_SEL, Y0) |
			BCHP_FIELD_ENUM(CAP_0_BYTE_ORDER, BYTE_2_SEL, CR) |
			BCHP_FIELD_ENUM(CAP_0_BYTE_ORDER, BYTE_1_SEL, Y1) |
			BCHP_FIELD_ENUM(CAP_0_BYTE_ORDER, BYTE_0_SEL, CB));
		break;

	case BPXL_eCb8_Y08_Cr8_Y18:
		BVDC_P_CAP_GET_REG_DATA(CAP_0_BYTE_ORDER) |=  (
			BCHP_FIELD_ENUM(CAP_0_BYTE_ORDER, BYTE_3_SEL, Y1) |
			BCHP_FIELD_ENUM(CAP_0_BYTE_ORDER, BYTE_2_SEL, CR) |
			BCHP_FIELD_ENUM(CAP_0_BYTE_ORDER, BYTE_1_SEL, Y0) |
			BCHP_FIELD_ENUM(CAP_0_BYTE_ORDER, BYTE_0_SEL, CB));
		break;
	}

#if(BVDC_P_SUPPORT_BVN_10BIT_444)
	if(ePxlFormat == BVDC_P_CAP_PIXEL_FORMAT_10BIT422)
	{
		eCapDataMode = BVDC_P_Capture_DataMode_e10Bit422;
	}
	else if(ePxlFormat == BVDC_P_CAP_PIXEL_FORMAT_10BIT444)
	{
		eCapDataMode = BVDC_P_Capture_DataMode_e10Bit444;
	}
	else
	{
		eCapDataMode = BVDC_P_Capture_DataMode_e8Bit422;
	}
	hCapture->eCapDataMode = eCapDataMode;

	BVDC_P_CAP_GET_REG_DATA(CAP_0_MODE) &= ~(
		BCHP_MASK(CAP_0_MODE, DATA_MODE));

	BVDC_P_CAP_GET_REG_DATA(CAP_0_MODE) |=  (
		BCHP_FIELD_DATA(CAP_0_MODE, DATA_MODE, eCapDataMode));
#endif

	BDBG_LEAVE(BVDC_P_Capture_SetPixelFormat_isr);
	return BERR_SUCCESS;
}


#if (BVDC_P_SUPPORT_CAP_VER >= BVDC_P_CAP_VER_4)
/***************************************************************************
 * {private}
 */
static BERR_Code BVDC_P_Capture_Set3DFormat_isr
	( BVDC_P_Capture_Handle            hCapture,
	  const BVDC_P_PictureNodePtr      pPicture )
{
	BDBG_ENTER(BVDC_P_Capture_Set3DFormat_isr);
	BDBG_OBJECT_ASSERT(hCapture, BVDC_CAP);

	/* CAP_0_MODE */
	BVDC_P_CAP_GET_REG_DATA(CAP_0_MODE) &= ~(
		BCHP_MASK(CAP_0_MODE, MEM_VIDEO) |
		BCHP_MASK(CAP_0_MODE, BVB_VIDEO));

	/* Always use dual pointer for 3D mode */
	if(pPicture->eSrcOrientation == BFMT_Orientation_e2D)
	{
		BVDC_P_CAP_GET_REG_DATA(CAP_0_MODE) |=  (
			BCHP_FIELD_DATA(CAP_0_MODE, MEM_VIDEO, pPicture->eSrcOrientation) |
			BCHP_FIELD_DATA(CAP_0_MODE, BVB_VIDEO, pPicture->eSrcOrientation));
	}
	else
	{
		BVDC_P_CAP_GET_REG_DATA(CAP_0_MODE) |=  (
			BCHP_FIELD_ENUM(CAP_0_MODE, MEM_VIDEO, MODE_3D_DUAL_POINTER) |
			BCHP_FIELD_DATA(CAP_0_MODE, BVB_VIDEO, pPicture->eSrcOrientation));
	}

	BDBG_LEAVE(BVDC_P_Capture_Set3DFormat_isr);
	return BERR_SUCCESS;
}
#endif

/***************************************************************************
 * {private}
 * This replaces the deprecated BVDC_P_Capture_SetPacking_isr.
 */
static BERR_Code BVDC_P_Capture_SetDither_isr
	( BVDC_P_Capture_Handle            hCapture,
	  const BVDC_P_PictureNodePtr      pPicture )
{
	BVDC_Window_Handle  hWindow;

	BDBG_ENTER(BVDC_P_Capture_SetDither_isr);
	BDBG_OBJECT_ASSERT(hCapture, BVDC_CAP);
	hWindow = pPicture->hBuffer->hWindow;
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	BDBG_OBJECT_ASSERT(hWindow->stCurInfo.hSource, BVDC_SRC);

#if (BVDC_P_SUPPORT_VIDEO_TESTFEATURE1_CAP)
	/* Dither */
	if((hWindow->stCurInfo.stCapTestFeature1.bEnable) ||
	   (BVDC_P_VNET_USED_ANR_AT_READER(hWindow->stVnetMode) &&
	    hWindow->stCurResource.hAnr && hWindow->stCurResource.hAnr->stTestFeature1.bEnable) ||
	   (BVDC_P_VNET_USED_MAD_AT_READER(hWindow->stVnetMode) &&
	    hWindow->stCurResource.hMad32 && hWindow->stCurResource.hMad32->stTestFeature1.bEnable))
	{
		/* Dither off when:
		 *  1) TestFeature1 is enabled on CAP
		 *  2) TestFeature1 is enabled on ANR for CAP->VFD->ANR
		 *  3) TestFeature1 is enabled on MAD for CAP->VFD->MAD
		 */
		hCapture->stDither.ulMode = 0;
	}
	else
	{
		hCapture->stDither.ulMode = 2;
	}

	BVDC_P_CAP_GET_REG_DATA(CAP_0_DITHER_CTRL) &= ~(
		BCHP_MASK(CAP_0_DITHER_CTRL, MODE));
	BVDC_P_CAP_GET_REG_DATA(CAP_0_DITHER_CTRL) |=
		BCHP_FIELD_DATA(CAP_0_DITHER_CTRL, MODE, hCapture->stDither.ulMode);
#endif

#if(BVDC_P_SUPPORT_BVN_10BIT_444)
	/* Downsample settings */
	if( hCapture->eCapDataMode != BVDC_P_Capture_DataMode_e10Bit444 )
	{
		hCapture->stDnSampler.eRingRemoval = hWindow->stCurInfo.hSource->bSrcIs444;
		if(hWindow->stCurInfo.hSource->bSrcIs444)
		{
			if(BVDC_P_VNET_USED_SCALER_AT_READER(hWindow->stVnetMode))
			{
				/* SRC -> CAP */
				hCapture->stDnSampler.eFilterType = BVDC_444To422Filter_eStandard;
			}
			else
			{
				/* SRC -> ANR (?) -> MAD (?) -> SCL -> CAP */
				if(BVDC_P_VNET_USED_ANR(hWindow->stVnetMode) ||
				   BVDC_P_VNET_USED_MAD(hWindow->stVnetMode) ||
				   (BVDC_P_VNET_USED_SCALER(hWindow->stVnetMode) &&
				   (hCapture->eId != BVDC_P_CaptureId_eCap0)))
				{
					hCapture->stDnSampler.eFilterType = BVDC_444To422Filter_eDecimate;
				}
				else
				{
					hCapture->stDnSampler.eFilterType = BVDC_444To422Filter_eStandard;
				}
			}
		}
		else
		{
			hCapture->stDnSampler.eFilterType = BVDC_444To422Filter_eDecimate;
		}

		BVDC_P_CAP_GET_REG_DATA(CAP_0_DS_CONFIGURATION) &= ~(
			BCHP_MASK(CAP_0_DS_CONFIGURATION, FILTER_TYPE)  |
			BCHP_MASK(CAP_0_DS_CONFIGURATION, RING_SUPPRESSION));

		BVDC_P_CAP_GET_REG_DATA(CAP_0_DS_CONFIGURATION) |=  (
			BCHP_FIELD_DATA(CAP_0_DS_CONFIGURATION, FILTER_TYPE,
				hCapture->stDnSampler.eFilterType) |
			BCHP_FIELD_DATA(CAP_0_DS_CONFIGURATION, RING_SUPPRESSION,
				hCapture->stDnSampler.eRingRemoval));
	}

#endif

	BDBG_LEAVE(BVDC_P_Capture_SetDither_isr);
	return BERR_SUCCESS;
}

/***************************************************************************
 * {private}
 * TODO: only support line_compare type non-mask mode now;
 *
 */
static BERR_Code BVDC_P_Capture_SetTrigger_isr
	( BVDC_P_Capture_Handle            hCapture,
	  BVDC_P_CapTriggerType            eTrigType )
{
	BDBG_ENTER(BVDC_P_Capture_SetTrigger_isr);
	BDBG_OBJECT_ASSERT(hCapture, BVDC_CAP);

	/* Turn off both capture trig0/1. */
	BVDC_P_CAP_GET_REG_DATA(CAP_0_TRIG_CTRL) &= ~(
		BCHP_MASK(CAP_0_TRIG_CTRL, TRIG_0_SEL) |
		BCHP_MASK(CAP_0_TRIG_CTRL, TRIG_1_SEL));

	if(BVDC_P_CapTriggerType_eBvbField == eTrigType)
	{
		/* turn on trig 0 */
		BVDC_P_CAP_GET_REG_DATA(CAP_0_TRIG_CTRL) |=  (
			BCHP_FIELD_DATA(CAP_0_TRIG_CTRL, TRIG_0_SEL, eTrigType));

		BVDC_P_CAP_GET_REG_DATA(CAP_0_BVB_TRIG_0_CFG) &= ~(
			BCHP_MASK(CAP_0_BVB_TRIG_0_CFG, TRIG_CFG));
		BVDC_P_CAP_GET_REG_DATA(CAP_0_BVB_TRIG_0_CFG) |=  (
			BCHP_FIELD_ENUM(CAP_0_BVB_TRIG_0_CFG, TRIG_CFG, END_OF_PICTURE));
	}

	BDBG_LEAVE(BVDC_P_Capture_SetTrigger_isr);
	return BERR_SUCCESS;
}

/***************************************************************************
 * {private}
 *
 */
BERR_Code BVDC_P_Capture_SetEnable_isr
	( BVDC_P_Capture_Handle            hCapture,
	  bool                             bEnable )
{
	BDBG_ENTER(BVDC_P_Capture_SetEnable_isr);
	BDBG_OBJECT_ASSERT(hCapture, BVDC_CAP);

	/* Turn on/off the capture. */
	BVDC_P_CAP_GET_REG_DATA(CAP_0_CTRL) &= ~(
#if (BVDC_P_SUPPORT_CAP_VER >= BVDC_P_CAP_VER_6)
		BCHP_MASK(CAP_0_CTRL, ENABLE_CTRL) |
#endif
		BCHP_MASK(CAP_0_CTRL, CAP_ENABLE));

	if(bEnable)
	{
		BVDC_P_CAP_GET_REG_DATA(CAP_0_CTRL) |=  (
#if (BVDC_P_SUPPORT_CAP_VER >= BVDC_P_CAP_VER_6)
			BCHP_FIELD_ENUM(CAP_0_CTRL, ENABLE_CTRL, ENABLE_BY_PICTURE) |
#endif
			BCHP_FIELD_ENUM(CAP_0_CTRL, CAP_ENABLE, ENABLE));
	}
	else
	{
		BVDC_P_CAP_GET_REG_DATA(CAP_0_CTRL) |=  (
#if (BVDC_P_SUPPORT_CAP_VER >= BVDC_P_CAP_VER_6)
			BCHP_FIELD_ENUM(CAP_0_CTRL, ENABLE_CTRL, ENABLE_BY_PICTURE) |
#endif
			BCHP_FIELD_ENUM(CAP_0_CTRL, CAP_ENABLE, DISABLE));
		BVDC_P_Capture_SetTrigger_isr(hCapture, BVDC_P_CapTriggerType_eDisable);
	}

	BDBG_LEAVE(BVDC_P_Capture_SetEnable_isr);
	return BERR_SUCCESS;
}

#if (BVDC_P_SUPPORT_VIDEO_TESTFEATURE1_CAP)
/***************************************************************************
 * {private}
 *
 */
BERR_Code BVDC_P_Capture_SetTestFeature1_isr
	( BVDC_P_Capture_Handle            hCapture,
	  BVDC_Window_Handle               hWindow,
	  const BVDC_P_PictureNodePtr      pPicture )
{
	BVDC_P_TestFeature1_Settings    *pTestFeature1;
	const BFMT_VideoInfo           *pSrcFmtInfo, *pDstFmtInfo;
	uint32_t    ulSrcThroughput, ulDstThroughput, ulMaxThroughput;
	uint32_t    ulSrcFmtRasterX, ulSrcFmtPixelClk;
	uint32_t    ulVecFmtRasterX, ulVecFmtPixelClk;

	BDBG_ENTER(BVDC_P_Capture_SetTestFeature1_isr);
	BDBG_OBJECT_ASSERT(hCapture, BVDC_CAP);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	BDBG_OBJECT_ASSERT(hWindow->stCurInfo.hSource, BVDC_SRC);
	BDBG_OBJECT_ASSERT(hWindow->hCompositor, BVDC_CMP);

	pTestFeature1 = &pPicture->stCapTestFeature1;

#if (BVDC_P_DCX_CAP_RESET_WORKAROUND)
	if(pTestFeature1->bEnable != hCapture->stTestFeature1.bEnable)
	{
		hCapture->bInitial = true;
	}

	hCapture->stTestFeature1 = pPicture->stCapTestFeature1;
#endif

	if(!pTestFeature1->bEnable)
		goto Done;

	if(hCapture->eCapDataMode == BVDC_P_Capture_DataMode_e10Bit444)
	{
		/* Capture is 444 */
		pTestFeature1->ulPixelPerGroup = 2;
	}
	else
	{
		/* Capture is 422 */
		pTestFeature1->ulPixelPerGroup = 4;
	}
	pTestFeature1->ulBitsPerGroup = (pTestFeature1->ulBitsPerPixel *
		pTestFeature1->ulPixelPerGroup) >> BVDC_P_TestFeature1_FRACTIONAL_SHIFT;

	/* PREDICTION_MODE */
	pSrcFmtInfo = hWindow->stCurInfo.hSource->stCurInfo.pFmtInfo;
	pDstFmtInfo = hWindow->hCompositor->stCurInfo.pFmtInfo;

	/* TODO: Use data in window directly when it's available */
	ulSrcFmtPixelClk = pSrcFmtInfo->ulPxlFreq;
	ulVecFmtPixelClk = pDstFmtInfo->ulPxlFreq;
	ulSrcFmtRasterX  = pSrcFmtInfo->ulScanWidth;
	ulVecFmtRasterX  = pDstFmtInfo->ulScanWidth;

	if(BVDC_P_VNET_USED_SCALER_AT_WRITER(hWindow->stVnetMode))
	{
		/* SRC -> SCL -> CAP -> VFD -> VEC
		 * feeder rate = (feederX) / (VecFmtRasterX / VecFmtPixelClk)
		 */
		ulSrcThroughput = hWindow->ulCapRate;
#if (BVDC_P_SUPPORT_VIDEO_TESTFEATURE1_VER <= 2)
		/* No rate smooth fifo between BVN and VEC on 3548/3556 */
		ulDstThroughput = ulVecFmtPixelClk;
#else
		ulDstThroughput = (pPicture->pVfdOut->ulWidth * ulVecFmtPixelClk) / ulVecFmtRasterX;
#endif
	}
	else if(BVDC_P_VNET_USED_SCALER_AT_READER(hWindow->stVnetMode))
	{
		/* SRC -> CAP -> VFD -> SCL -> VEC
		 * capture rate = (captureX) / (SrcFmtRasterX / SrcFmtPixelClk)
		 */
		ulSrcThroughput = (pPicture->pCapIn->ulWidth * ulSrcFmtPixelClk) / ulSrcFmtRasterX;
		ulDstThroughput = hWindow->ulFeedRate;
		}
		else
		{
			/* Data rate can be smoothed across each line because VEC is a fixed
			 * rate data consumer and there is enough rate-smoothing FIFOs in the
			 * path that can help with line-smoothing. So
			 * ulThroughput =  picture_width x HSYNC_frequency_in_Hz
			 * HSYNC_frequency_in_Hz = pFmtInfo->VSYNC_frequency_in_Hz x
			 * (pFmtInfo->Total_Height >> pFmtInfo->bInterlaced)
		 */
		ulSrcThroughput = (pPicture->pCapIn->ulWidth * ulSrcFmtPixelClk) / ulSrcFmtRasterX;
		ulDstThroughput = (pPicture->pVfdOut->ulWidth * ulVecFmtPixelClk) / ulVecFmtRasterX;
	}

	ulMaxThroughput = (ulSrcThroughput > ulDstThroughput)
		? ulSrcThroughput : ulDstThroughput;

	if(ulMaxThroughput > BVDC_P_TestFeature1_THRESHOLD)
	{
		pTestFeature1->ulPredictionMode = 1;
	}
	else
	{
		pTestFeature1->ulPredictionMode = 0;
	}

	/* CAP_0_DCE_COMPR_CFG1 */
	BVDC_P_CAP_GET_REG_DATA(CAP_0_DCE_COMPR_CFG1) &= ~(
		BCHP_MASK(CAP_0_DCE_COMPR_CFG1, PIXELS_PER_GROUP ) |
		BCHP_MASK(CAP_0_DCE_COMPR_CFG1, TGT_OFFSET_HI ) |
		BCHP_MASK(CAP_0_DCE_COMPR_CFG1, TGT_OFFSET_LO ) |
		BCHP_MASK(CAP_0_DCE_COMPR_CFG1, TGT_BPG          ));
	BVDC_P_CAP_GET_REG_DATA(CAP_0_DCE_COMPR_CFG1) |=  (
		BCHP_FIELD_DATA(CAP_0_DCE_COMPR_CFG1, PIXELS_PER_GROUP,  pTestFeature1->ulPixelPerGroup) |
		BCHP_FIELD_DATA(CAP_0_DCE_COMPR_CFG1, TGT_OFFSET_HI,     0xfa   ) |
		BCHP_FIELD_DATA(CAP_0_DCE_COMPR_CFG1, TGT_OFFSET_LO,     12     ) |
		BCHP_FIELD_DATA(CAP_0_DCE_COMPR_CFG1, TGT_BPG, pTestFeature1->ulBitsPerGroup));

Done:
	/* CAP_0_DCE_PRED_CFG */
	BVDC_P_CAP_GET_REG_DATA(CAP_0_DCE_PRED_CFG) &= ~(
		BCHP_MASK(CAP_0_DCE_PRED_CFG, ENABLE         ) |
		BCHP_MASK(CAP_0_DCE_PRED_CFG, PREDICTION_MODE));
	BVDC_P_CAP_GET_REG_DATA(CAP_0_DCE_PRED_CFG) |=  (
		BCHP_FIELD_DATA(CAP_0_DCE_PRED_CFG, ENABLE, pTestFeature1->bEnable) |
		BCHP_FIELD_DATA(CAP_0_DCE_PRED_CFG, PREDICTION_MODE, pTestFeature1->ulPredictionMode));

	BDBG_LEAVE(BVDC_P_Capture_SetTestFeature1_isr);
	return BERR_SUCCESS;
}
#endif


/***************************************************************************
 * {private}
 *
 */
BERR_Code BVDC_P_Capture_SetInfo_isr
	( BVDC_P_Capture_Handle            hCapture,
	  BVDC_Window_Handle               hWindow,
	  const BVDC_P_PictureNodePtr      pPicture,
	  uint32_t                         ulRectIdx,
	  bool                             bLastPic )
{
	unsigned int uiPitch;
	uint32_t ulPitch;
	uint32_t ulStartAddr, ulStartAddr_R = 0;
	BAVC_Polarity eCapturePolarity;
	BVDC_P_Rect  *pCapOut, *pCapIn;
	bool bDrain = false;
	const BVDC_P_Window_Info *pUserInfo = &hWindow->stCurInfo;

	BDBG_OBJECT_ASSERT(hCapture, BVDC_CAP);
	BDBG_ASSERT(BVDC_P_VNET_USED_CAPTURE(pPicture->stVnetMode));
	pCapIn  = pPicture->pCapIn;
	pCapOut = pPicture->pCapOut;
	BDBG_ASSERT(pCapIn->ulWidth  >= pCapOut->ulWidth  + pCapOut->lLeft);
	BDBG_ASSERT(pCapIn->ulHeight >= pCapOut->ulHeight + pCapOut->lTop);

	/* Pitch is the total of bytes per line.
	 * MosaicMode: the capture pitch is equal to the playback pitch! */
	BPXL_GetBytesPerNPixels(pPicture->ePixelFormat,
		pUserInfo->bMosaicMode ?
			((pPicture->pVfdIn->ulWidth + 1) & ~0x1) : ((pCapOut->ulWidth + 1) & ~0x1),
		&uiPitch);
	ulPitch = BVDC_P_ALIGN_UP(uiPitch, BVDC_P_PITCH_ALIGN);

	eCapturePolarity =
		(BVDC_P_VNET_USED_SCALER_AT_WRITER(pPicture->stVnetMode)
		? pPicture->eDstPolarity : pPicture->eSrcPolarity);

#if BFMT_LEGACY_3DTV_SUPPORT
	/* SW workaround might forces eDstPolarity as eFrame in window for this 3D format */
	if(VIDEO_FORMAT_IS_CUSTOM_1080P3D(hWindow->hCompositor->stCurInfo.pFmtInfo->eVideoFmt) &&
		BVDC_P_VNET_USED_SCALER_AT_WRITER(pPicture->stVnetMode))
	{
		eCapturePolarity = BAVC_Polarity_eTopField;
	}
#endif

	BVDC_P_Capture_SetEnable_isr(hCapture, true);

	BVDC_P_Capture_SetPixelFormat_isr(hCapture, pPicture);

#if (BVDC_P_SUPPORT_CAP_VER >= BVDC_P_CAP_VER_4)
	BVDC_P_Capture_Set3DFormat_isr(hCapture, pPicture);
#endif

	/* After eCapDataMode is set in BVDC_P_Capture_SetPixelFormat_isr */
	BVDC_P_Capture_SetDither_isr(hCapture, pPicture);

	/* if source sends extra vbi pass-thru lines, we need to capture it also; */
	BVDC_P_Capture_SetPictureRect_isr(hCapture, pCapIn, pCapOut, eCapturePolarity);

	ulStartAddr = BVDC_P_Buffer_GetDeviceOffset(pPicture);

#if (BVDC_P_SUPPORT_CAP_VER >= BVDC_P_CAP_VER_4)
	if(hWindow->eBufAllocMode == BVDC_P_BufHeapAllocMode_eLRSeparate)
	{
		ulStartAddr_R = BVDC_P_Buffer_GetDeviceOffset_R(pPicture);
	}
	else if(hWindow->eBufAllocMode == BVDC_P_BufHeapAllocMode_eLRCombined)
	{
		uint32_t  ulBufHeapSize;

		BVDC_P_BufferHeap_GetHeapSizeById(
			hWindow->hCompositor->hVdc->hBufferHeap,
			hWindow->eBufferHeapIdRequest, &ulBufHeapSize);

		ulStartAddr_R = ulStartAddr + ulBufHeapSize / 2;
	}
#endif

#if BVDC_P_SUPPORT_MOSAIC_MODE
	/* MosaicMode: calculate the starting address of sub-window capture; */
	if(pUserInfo->bMosaicMode)
	{
		/* for the chained sub-RULs, enable capture trigger; disable at last;
		   note in simul mode, we still need to enable cap trig when current
		   capture is the last mosaic for the current window in case the other
		   window has more mosaics to capture, i.e. combo trig is enabled; */
		/* this is to assure the rest of the pictures list got drained; */
		/* don't overwrite captured window; drain the source to save bandwidth; */
		if((ulRectIdx >= pUserInfo->ulMosaicCount) ||
		   (!pUserInfo->abMosaicVisible[ulRectIdx]))
		{
			bDrain = true;
		}
		else
		{
			unsigned int  uiByteOffset;
			uint32_t  ulMosaicIdx = BVDC_P_MIN(ulRectIdx, pUserInfo->ulMosaicCount - 1);

			BPXL_GetBytesPerNPixels(pPicture->ePixelFormat,
				pUserInfo->astMosaicRect[ulMosaicIdx].lLeft,
				&uiByteOffset);

			ulStartAddr += ulPitch * (pUserInfo->astMosaicRect[ulMosaicIdx].lTop >>
				 (eCapturePolarity != BAVC_Polarity_eFrame)) + uiByteOffset;
#if (BVDC_P_SUPPORT_CAP_VER >= BVDC_P_CAP_VER_4)
			ulStartAddr_R += ulPitch * (pUserInfo->astMosaicRect[ulMosaicIdx].lTop >>
				 (eCapturePolarity != BAVC_Polarity_eFrame)) + uiByteOffset;
#endif
		}
	}
	else
	{
		/* this is to assure the rest of the pictures list got drained; */
		/* don't overwrite captured window; drain the source; */
		if(!pUserInfo->bMosaicMode && ulRectIdx)
		{
			bDrain = true;
		}
	}

	if(pUserInfo->hSource->stCurInfo.bMosaicMode && !bLastPic)
	{
		BVDC_P_Capture_SetTrigger_isr(hCapture, BVDC_P_CapTriggerType_eBvbField);
	}
	else
	{
#if (BVDC_P_VDEC_SECAM_TTD_BYPASS_FILTER_IN_VBI)
		if(VIDEO_FORMAT_IS_SECAM(pUserInfo->hSource->stCurInfo.pFmtInfo->eVideoFmt))
		{
			/* SW3556-886, SW3556-1162  : unfilftered lines after SAV and before EAV */
			#define BVDC_P_SAV_EAV_UNFILTERED_LINES          (2)

			uint32_t ulHeight  = BVDC_P_CAP_GET_FIELD_NAME(CAP_0_BVB_IN_SIZE, VSIZE);
			uint32_t ulTurnOn  = BVDC_P_SAV_EAV_UNFILTERED_LINES;
			uint32_t ulTurnOff = ulHeight - BVDC_P_SAV_EAV_UNFILTERED_LINES;

			/* turn on trig 0 */
			BVDC_P_CAP_GET_REG_DATA(CAP_0_TRIG_CTRL) |=  (
				BCHP_FIELD_ENUM(CAP_0_TRIG_CTRL, TRIG_0_SEL, LINE_COMPARE /*BVB_TRIGGER */));

			BVDC_P_CAP_GET_REG_DATA(CAP_0_BVB_TRIG_0_CFG) &= ~(
				BCHP_MASK(CAP_0_BVB_TRIG_0_CFG, TRIG_CFG));
			BVDC_P_CAP_GET_REG_DATA(CAP_0_BVB_TRIG_0_CFG) |=  (
				BCHP_FIELD_ENUM(CAP_0_BVB_TRIG_0_CFG, TRIG_CFG, START_OF_PICTURE));

			BVDC_P_CAP_GET_REG_DATA(CAP_0_LINE_CMP_TRIG_0_CFG) &= ~(
				BCHP_MASK(CAP_0_LINE_CMP_TRIG_0_CFG, POS) |
				BCHP_MASK(CAP_0_LINE_CMP_TRIG_0_CFG, MASK));
			BVDC_P_CAP_GET_REG_DATA(CAP_0_LINE_CMP_TRIG_0_CFG) |=  (
				BCHP_FIELD_DATA(CAP_0_LINE_CMP_TRIG_0_CFG, POS,  ulTurnOn)|
				BCHP_FIELD_ENUM(CAP_0_LINE_CMP_TRIG_0_CFG, MASK, COMPARE_POS));

			/* turn off trig 1 */
			BVDC_P_CAP_GET_REG_DATA(CAP_0_TRIG_CTRL) |=  (
				BCHP_FIELD_ENUM(CAP_0_TRIG_CTRL, TRIG_1_SEL, LINE_COMPARE /*LINE_COMPARE */));

			BVDC_P_CAP_GET_REG_DATA(CAP_0_BVB_TRIG_1_CFG) &= ~(
				BCHP_MASK(CAP_0_BVB_TRIG_1_CFG, TRIG_CFG));
			BVDC_P_CAP_GET_REG_DATA(CAP_0_BVB_TRIG_1_CFG) |=  (
				BCHP_FIELD_ENUM(CAP_0_BVB_TRIG_1_CFG, TRIG_CFG, END_OF_PICTURE));

			BVDC_P_CAP_GET_REG_DATA(CAP_0_LINE_CMP_TRIG_1_CFG) &= ~(
				BCHP_MASK(CAP_0_LINE_CMP_TRIG_1_CFG, POS) |
				BCHP_MASK(CAP_0_LINE_CMP_TRIG_1_CFG, MASK));
			BVDC_P_CAP_GET_REG_DATA(CAP_0_LINE_CMP_TRIG_1_CFG) |=  (
				BCHP_FIELD_DATA(CAP_0_LINE_CMP_TRIG_1_CFG, POS,  ulTurnOff)|
				BCHP_FIELD_ENUM(CAP_0_LINE_CMP_TRIG_1_CFG, MASK, COMPARE_POS));
		}
		else
		{
			BVDC_P_Capture_SetTrigger_isr(hCapture, BVDC_P_CapTriggerType_eDisable);
		}
#else
		BVDC_P_Capture_SetTrigger_isr(hCapture, BVDC_P_CapTriggerType_eDisable);
#endif
	}

	if(bDrain)
	{
		ulStartAddr   = hWindow->ulNullBufOffset;
		ulStartAddr_R = hWindow->ulNullBufOffset;

		BVDC_P_CAP_GET_REG_DATA(CAP_0_PIC_SIZE) =  (
			BCHP_FIELD_DATA(CAP_0_PIC_SIZE, HSIZE, 2) |
			BCHP_FIELD_DATA(CAP_0_PIC_SIZE, VSIZE, 1));
		/* clip the top/left region of input data to have capture engine to
		   generate trigger at correct timing (end of input picture); */
		BVDC_P_CAP_GET_REG_DATA(CAP_0_PIC_OFFSET) =  (
			BCHP_FIELD_DATA(CAP_0_PIC_OFFSET, HSIZE,
				BVDC_P_CAP_GET_FIELD_NAME(CAP_0_BVB_IN_SIZE, HSIZE) - 2) |
			BCHP_FIELD_DATA(CAP_0_PIC_OFFSET, VSIZE,
				BVDC_P_CAP_GET_FIELD_NAME(CAP_0_BVB_IN_SIZE, VSIZE) - 1));
	}
#else
	BSTD_UNUSED(bLastPic);
	BSTD_UNUSED(ulRectIdx);
	BSTD_UNUSED(bDrain);
#endif

	BVDC_P_Capture_SetBuffer_isr(hCapture, ulStartAddr, ulStartAddr_R, ulPitch);

#if (BVDC_P_SUPPORT_VIDEO_TESTFEATURE1_CAP)
	if(hCapture->eId == BVDC_P_CaptureId_eCap0)
	{
		BVDC_P_Capture_SetTestFeature1_isr(hCapture, hWindow, pPicture);
	}
#endif

	return BERR_SUCCESS;
}

/* End of file. */
