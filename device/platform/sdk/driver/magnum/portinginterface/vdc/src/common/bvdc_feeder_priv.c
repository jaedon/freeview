/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvdc_feeder_priv.c $
 * $brcm_Revision: Hydra_Software_Devel/323 $
 * $brcm_Date: 10/25/12 2:30p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vdc/src/common/bvdc_feeder_priv.c $
 * 
 * Hydra_Software_Devel/332   7/16/13 6:16p vanessah
 * SW7425-4963: fix the mute color 3D programming
 * 
 * Hydra_Software_Devel/323   10/25/12 2:30p pntruong
 * SW7425-4108: MJPEG decode displays garbage.  Need to program the
 * surface address for the callback path (xdm->vdc) as well.
 *
 * Hydra_Software_Devel/322   9/6/12 2:41p syang
 * SW7425-2172: don't validate if no surface is set yet
 *
 * Hydra_Software_Devel/321   8/28/12 4:50p yuxiaz
 * SW7360-26: MFD_0_DATA_MODE is added with 3D.
 *
 * Hydra_Software_Devel/320   8/28/12 1:39p yuxiaz
 * SW7360-26: Only program MRE registers for MFD0.
 *
 * Hydra_Software_Devel/319   8/15/12 6:21p tdo
 * SW7445-8: Create Appframework emulation environment for 7445 A0
 *
 * Hydra_Software_Devel/318   8/14/12 1:59p yuxiaz
 * SW7550-802: Fix coverity errors.
 *
 * Hydra_Software_Devel/317   8/9/12 1:05p syang
 * SW7425-2172: copy mfd reg values from sw record to HW differently
 * accoeding to MFD_VER
 *
 * Hydra_Software_Devel/316   8/9/12 11:18a syang
 * SW7425-2172: add SetSurface support for MFD gfx feeding
 *
 * Hydra_Software_Devel/315   8/7/12 10:34a vanessah
 * SW7563-9: MRE3D disable for 7563
 *
 * Hydra_Software_Devel/314   7/31/12 4:55p yuxiaz
 * SW7425-1835: Use RDC slot timestamp for capture and playback timestamp
 * in multi-buffering algo.
 *
 * Hydra_Software_Devel/313   6/15/12 10:34a vanessah
 * SW7435-237: reset vnet channel connections
 *
 * Hydra_Software_Devel/312   6/14/12 8:38p syang
 * SW7425-2172: free scratch reg for mfd
 *
 * Hydra_Software_Devel/311   6/14/12 3:42p syang
 * SW7425-2172: VFD/GFX with only cmp0 need 6 scratch regs for gfx;
 * prepare to add SetSurface support to MFD
 *
 * Hydra_Software_Devel/310   6/8/12 1:42p yuxiaz
 * SW7425-3026: Rename BVDC_P_Feeder_GetMpegOutOrientation to
 * BVDC_P_Feeder_GetMpegOutOrientation_isr, fixed compile warning.
 *
 * Hydra_Software_Devel/309   6/7/12 3:26p yuxiaz
 * SW7425-3026: Optimize 3D video support.
 *
 * Hydra_Software_Devel/308   5/31/12 11:07p syang
 * SW7425-2093: don't write to shared timeStamp scratch reg if the vfd is
 * feeding gfx
 *
 * Hydra_Software_Devel/307   5/30/12 5:11p syang
 * SW7425-2093: extra ApplyChange with 1st SetSurface no longer needed;
 * vfd/cap time stamp related scratch registers now share with vfd's
 * surface manager's; added vfd resource check; more clean up
 *
 * Hydra_Software_Devel/305   5/29/12 2:07p syang
 * SW7425-2093: for the moment delay scratch register alloc for VFD to
 * Feeder_Init
 *
 * Hydra_Software_Devel/302   5/23/12 6:46p syang
 * SW7425-2093: refactor vfd gfx feeding support for deep cleanup and to
 * use gfxsurface sub-module
 *
 * Hydra_Software_Devel/301   5/9/12 10:52a syang
 * SW7425-2093: refactor gfxfeeder code for deep cleanup and potential
 * error fix with GetSurface; use shared gfxsurface to manage gfx surface
 *
 * Hydra_Software_Devel/300   3/8/12 11:50a yuxiaz
 * SW7425-2546: Fixed warning msg.
 *
 * Hydra_Software_Devel/299   2/29/12 2:54p yuxiaz
 * SW7425-2418: Clean up code for mfd feeds from gfx surface
 *
 * Hydra_Software_Devel/298   2/17/12 2:13p yuxiaz
 * SW7231-635: Use BVDC_P_SCB_BURST_SIZE.
 *
 * Hydra_Software_Devel/297   2/16/12 5:49p yuxiaz
 * SW7231-635: Added sw workaround for VFD.
 *
 * Hydra_Software_Devel/296   2/14/12 4:48p yuxiaz
 * SW7425-2181: Do not need to check eChrominanceInterpolationMode for
 * graphics surface.
 *
 * Hydra_Software_Devel/295   2/8/12 9:35a yuxiaz
 * SW7425-2181: Fixed coverity issue.
 *
 * Hydra_Software_Devel/294   2/7/12 5:14p yuxiaz
 * SW7425-2181: Add basic VDC support to interface with output from SID
 * for MJPEG usage mode.
 *
 * Hydra_Software_Devel/293   1/18/12 5:21p tdo
 * SW7435-9: Add support for CAP_5, MFD_3, VFD_5
 *
 * Hydra_Software_Devel/292   12/13/11 4:58p yuxiaz
 * SW7425-1875: Added support for source buffer selection in 3d mode.
 *
 * Hydra_Software_Devel/291   12/1/11 4:32p yuxiaz
 * SW7425-968, SW7344-95: Merged into mainline.: added independent source
 * clipping of right window in 3D mode.
 *
 * Hydra_Software_Devel/290   11/30/11 3:30p yuxiaz
 * SW7425-1182, SW7231-469: Fixed BYTE_ORDER settings in CAP and VFD on
 * both LE and BE system.
 *
 * Hydra_Software_Devel/289   11/8/11 2:15p tdo
 * SW7425-1544: Cannot switch between half-res 3DTV source orientations
 * without NEXUS_VideoWindow_Remove/AddInput.  Need to reset BVB_VIDEO to
 * 2D mode during fix color to get rid of MFD error.
 *
 * Hydra_Software_Devel/288   10/19/11 3:30p yuxiaz
 * SW7425-1182: Merge into mainline: Add support for more pixel formats
 * for surfaces accessed by CAP/MFD
 *
 * Hydra_Software_Devel/SW7425-1182/2   10/18/11 3:51p yuxiaz
 * SW7425-1182: Clean up MFD_0_BYTE_ORDER settings.
 *
 * Hydra_Software_Devel/SW7425-1182/1   10/18/11 1:43p yuxiaz
 * SW7425-1182: Clean up packing_type settings.
 *
 * Hydra_Software_Devel/287   9/30/11 3:03p yuxiaz
 * SW7425-985: Fixed luma and chroma address calculation when feeding out
 * bottom field on MVFD to fix staircases.
 *
 * Hydra_Software_Devel/286   9/28/11 12:15p yuxiaz
 * SW7425-985: Fix artifacts at bottom of screen on new feeder.
 *
 * Hydra_Software_Devel/285   9/27/11 5:21p yuxiaz
 * SW7425-967: Bring up MRE on 7425 B0.
 *
 * Hydra_Software_Devel/284   9/23/11 3:18p yuxiaz
 * SW7425-985: Added flag BVDC_P_MFD_ENABLE_SKIP_FIRST_LINE to enable
 * SKIP_FIRST_LINE.
 *
 * Hydra_Software_Devel/283   9/8/11 3:34p yuxiaz
 * SW7425-985: Only program MRE for MFD_0. Set InitPhase based on chroma
 * interpolation mode.
 *
 * Hydra_Software_Devel/282   9/2/11 2:52p yuxiaz
 * SW7425-985, SW7552-100: Bring up new MFD on emulator.
 *
 * Hydra_Software_Devel/281   9/2/11 1:07p yuxiaz
 * SW7425-985, SW7425-967: Fixed settings for MFD_0_MRE3D_LINE_ADDR_1_HP .
 *
 * Hydra_Software_Devel/280   9/1/11 9:48p yuxiaz
 * SW7425-985, SW7425-967: Added MRE support in MFD.
 *
 * Hydra_Software_Devel/279   9/1/11 2:23p yuxiaz
 * SW7425-985, SW7552-100: Bring up new MFD on emulator.
 *
 * Hydra_Software_Devel/278   8/30/11 2:03p yuxiaz
 * SW7425-985, SW7552-100: Fixed compile warning.
 *
 * Hydra_Software_Devel/277   8/30/11 9:32a yuxiaz
 * SW7425-985, SW7552-100: Added init_phase settings. Fixed coverity
 * issue.
 *
 * Hydra_Software_Devel/276   8/28/11 2:16p yuxiaz
 * SW7425-985, SW7552-100: Merge into mainline.Clean up feeder.
 *
 * Hydra_Software_Devel/SW7425-985/3   8/26/11 3:00p yuxiaz
 * SW7425-985, SW7552-100: Added support for new MFD.
 *
 * Hydra_Software_Devel/SW7425-985/2   8/25/11 3:40p yuxiaz
 * SW7425-985, SW7552-100: More clean up for feeder.
 *
 * Hydra_Software_Devel/SW7425-985/1   8/23/11 4:00p yuxiaz
 * SW7425-985, SW7552-100: Clean up for new MFD design.
 *
 * Hydra_Software_Devel/275   8/11/11 3:59p pntruong
 * SW7231-318: Initial vdc support with error free build.
 *
 * Hydra_Software_Devel/274   8/5/11 2:05p yuxiaz
 * SW7425-985: Get 7425 B0 compiled.
 *
 * Hydra_Software_Devel/273   8/1/11 5:29p yuxiaz
 * SW7425-932: Fixed SCB_CLIENT_SEL setting: When running "tav" test at
 * 1080p60 the first CRC captured is 0xFFFFFFFF.
 *
 * Hydra_Software_Devel/272   7/18/11 5:32p pntruong
 * SW7231-284, SW7346-295, SW7405-5339: Refactored master framerate
 * tracking, so that it can also handle validation.
 *
 * Hydra_Software_Devel/271   7/13/11 4:24p yuxiaz
 * SW7405-5355: Remove VDC warnings.
 *
 * Hydra_Software_Devel/270   7/12/11 3:28p yuxiaz
 * SW7358-40: Added second CRC support in VDC.
 *
 * Hydra_Software_Devel/269   6/24/11 3:48p albertl
 * SW7420-1966: Cleaned up and reorganized colorspace enums and indicies
 * in display, vdec, and cmp.
 *
 * Hydra_Software_Devel/268   6/17/11 11:26a yuxiaz
 * SW7425-698: MFD always use dual pointer mode for 3D. Fixed right buffer
 * address alignment.
 *
 * Hydra_Software_Devel/267   6/13/11 1:52p yuxiaz
 * SW7405-5355: Remove VDC warnings.
 *
 * Hydra_Software_Devel/266   5/24/11 4:36p yuxiaz
 * SW7425-415: Always set MFD_0_PICTURE0_LINE_ADDR_0_R.
 *
 * Hydra_Software_Devel/265   4/27/11 11:37a yuxiaz
 * SW7358-4: Updated VDC flags.
 *
 * Hydra_Software_Devel/264   4/20/11 3:09p tdo
 * SW7425-365: Remove compiler warnings with new kernel build
 *
 * Hydra_Software_Devel/263   4/4/11 1:28p pntruong
 * SW7422-48, SW7231-87, SW7231-88, SW7231-73, SW7425-183: Correct select
 * the rts client for streams less than or equal to 1080i@60hz.  This is
 * in reference to jira SW7422-48.
 *
 * Hydra_Software_Devel/262   1/6/11 3:22p pntruong
 * SW7422-181: Pink/green garbage at top of display in MVC decode.
 * Refactored output/polarity programming.
 *
 * Hydra_Software_Devel/261   12/16/10 11:39p pntruong
 * SW3548-2987: Added 1080p@30hz 3DOU_AS hdmi input support.
 *
 * Hydra_Software_Devel/260   12/15/10 11:38a yuxiaz
 * SW7422-48: Double MFD throughput for 3D source.
 *
 * Hydra_Software_Devel/259   12/10/10 4:45p yuxiaz
 * SW7422-39: Always use dual pointer mode for 3D in CAP and VFD.
 *
 * Hydra_Software_Devel/258   12/10/10 10:32a yuxiaz
 * SW7422-39: 3D bring up on 7422.
 *
 * Hydra_Software_Devel/257   11/29/10 2:49p hongtaoz
 * SW7422-113: sw_init MFD when in fixed color mode;
 *
 * Hydra_Software_Devel/256   11/24/10 1:41p jessem
 * SW7422-82: Fixed run-time bugs.
 *
 * Hydra_Software_Devel/255   11/10/10 10:19a yuxiaz
 * SW7422-39: Address offset for PICTURE0_LINE_ADDR_0_R is same for MFD
 * and VFD on 7422.
 *
 * Hydra_Software_Devel/254   11/9/10 4:27p jessem
 * SW7422-82: Fixed compile errors.
 *
 * Hydra_Software_Devel/253   11/9/10 3:48p jessem
 * SW7422-82: Simplified validation for R surface.
 *
 * Hydra_Software_Devel/252   11/2/10 11:22a jessem
 * SW7422-82: Fixed coverity error.
 *
 * Hydra_Software_Devel/251   11/1/10 10:29p tdo
 * SW7422-82: Fix compiling error
 *
 * Hydra_Software_Devel/250   11/1/10 4:23p jessem
 * SW7422-82: Added stereo 3D support to VFD as source feature.
 *
 * Hydra_Software_Devel/249   11/1/10 1:12p vanessah
 * SW7422-43:  merge 2D into 3D with a unified interface, change input
 * data structure as const
 *
 * Hydra_Software_Devel/248   10/29/10 5:05p vanessah
 * SW7422-43:  3D graphics render,merge 2D into 3D with a unified
 * interface
 *
 * Hydra_Software_Devel/247   10/29/10 3:02p tdo
 * SW7422-40: Update GetBuffer() capability in VDC with 3D case
 *
 * Hydra_Software_Devel/246   10/29/10 10:58a yuxiaz
 * SW7422-31, SW7422-32, SW7422-39: More for 3D Video support.
 *
 * Hydra_Software_Devel/245   10/25/10 7:18p vanessah
 * SW7422-43:  Handle GFD programming for 3D.
 *
 * Hydra_Software_Devel/244   10/14/10 6:39p hongtaoz
 * SW7425-30, SW7422-30: fixed feeder VSIZE programming for interlaced
 * scanout;
 *
 * Hydra_Software_Devel/243   10/14/10 10:27a yuxiaz
 * SW7420-173: Fixed coverity erros.
 *
 * Hydra_Software_Devel/242   10/13/10 1:52p yuxiaz
 * SW7422-48: Add SCB Client select for MFD.
 *
 * Hydra_Software_Devel/241   10/12/10 2:45p jessem
 * SW7420-173: Fixed coverity erros.
 *
 * Hydra_Software_Devel/240   10/12/10 11:57a jessem
 * SW7420-173: Fixed build error.
 *
 * Hydra_Software_Devel/239   10/12/10 11:51a jessem
 * SW7420-173: Fixed compile warnings.
 *
 * Hydra_Software_Devel/238   10/11/10 5:20p jessem
 * SW7420-173: Cleaned up compile warnings.
 *
 * Hydra_Software_Devel/237   10/11/10 12:26p jessem
 * SW7420-173: Added support for using VFD as  a  source.
 *
 * Hydra_Software_Devel/236   9/29/10 5:22p yuxiaz
 * SW7422-39: Added 3D video support in CAP and VFD.
 *
 * Hydra_Software_Devel/235   9/28/10 11:35p tdo
 * SW7425-33: Remove compiling warning
 *
 * Hydra_Software_Devel/234   9/24/10 2:31p pntruong
 * SW3548-3087: Added work-around for fixed color when in yuv444 mode.
 *
 * Hydra_Software_Devel/233   9/21/10 6:10p yuxiaz
 * SW7422-31, SW7422-32: Handle single and dual buffer 3D cases in MFD.
 *
 * Hydra_Software_Devel/232   9/15/10 11:38a yuxiaz
 * SW7422-30: Handle forced capture modes and orientation programming in
 * MFD.
 *
 * Hydra_Software_Devel/231   9/15/10 11:22a yuxiaz
 * SW7422-30: Handle forced capture modes and orientation programming in
 * MFD.
 *
 * Hydra_Software_Devel/230   9/1/10 1:21p yuxiaz
 * SW7422-39: Clean up register offset in MFD and VFD.
 *
 * Hydra_Software_Devel/229   9/1/10 9:40a yuxiaz
 * SW7422-39: Fixed build error on 3549.
 *
 * Hydra_Software_Devel/228   8/31/10 5:00p yuxiaz
 * SW7422-39: Clean up feeder programming.
 *
 * Hydra_Software_Devel/227   8/26/10 5:25p tdo
 * SW7422-57: Add simple vdc support
 *
 * Hydra_Software_Devel/226   7/13/10 11:27a yuxiaz
 * SW3548-2987: Added new 1080p@30hz support for 3DTV.
 *
 * Hydra_Software_Devel/225   4/19/10 10:14p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic. Move
 * BDBG_OBJECT_ID_DECLARE private header files instead of .c.
 *
 * Hydra_Software_Devel/224   4/7/10 11:29a tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic.  Rename TLA
 *
 * Hydra_Software_Devel/223   4/5/10 4:53p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic
 *
 * Hydra_Software_Devel/222   4/5/10 4:05p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic
 *
 * Hydra_Software_Devel/221   3/17/10 12:02p syang
 * SW7405-4046: set canvas ctrl after sur ctrl; handle gfx win vnet the
 * same as video; ensure that reader and writer vnetState match in sync-
 * locked case; aligned width up to even for stride in feeder and cap;
 * assert mosaic mode doesn't co-exist with dest cut;
 *
 * Hydra_Software_Devel/220   2/8/10 4:53p yuxiaz
 * SW7325-687: Reserved field must be written with 0 on chips without
 * VFD_0_FEEDER_CNTL_IMAGE_FORMAT field.
 *
 * Hydra_Software_Devel/219   2/3/10 6:49p albertl
 * SW7125-180: Changed to test if TESTFEATURE1 is supported in addition to
 * version number for mad.  Changed define to use capture's define in
 * feeder code.
 *
 * Hydra_Software_Devel/218   2/3/10 6:43p tdo
 * SW7125-180: Fix 7420 compiling error
 *
 * Hydra_Software_Devel/217   2/3/10 6:23p albertl
 * SW7125-180: Updated TESTFEATURE1 defines for 7125.
 *
 * Hydra_Software_Devel/216   2/2/10 2:39p yuxiaz
 * SW3548-2678, SW3548-2680, SW3548-2681: Fixed progressive display format
 * on 7405.
 *
 * Hydra_Software_Devel/215   2/1/10 6:38p hongtaoz
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
 * Hydra_Software_Devel/BCM3D/1   12/17/09 6:09p hongtaoz
 * SW3548-2680, SW3548-2681: added HDMI1.4 3D format detection and
 * handling for 11-bit BVN;
 *
 * Hydra_Software_Devel/214   12/17/09 2:02p yuxiaz
 * SW3548-2676: Only returns warning for unsupported pixel formats on old
 * chipset.
 *
 * Hydra_Software_Devel/213   12/1/09 3:54p yuxiaz
 * CR7340-133: BVN MFD Move the pixel saturation control from the test
 * mode register to the feeder control register.
 *
 * Hydra_Software_Devel/212   11/6/09 3:28p yuxiaz
 * SW7420-117: Added support for Newer Chroma Sample Location Types.
 *
 * Hydra_Software_Devel/211   10/12/09 4:51p rpan
 * SW7405-3159: Free up RDC scratch register allocated for time stamp
 * purpose when VFD/CAP gets destroyed.
 *
 * Hydra_Software_Devel/210   10/12/09 4:36p rpan
 * SW7405-3159: Moved writer/reader time stamp register from CAP/VFD to
 * RDC scratch registers.
 *
 * Hydra_Software_Devel/209   10/5/09 3:21p tdo
 * SW7405-3044: Undo previous check in.  Will do size alignment in Mpeg
 * source data ready so that it can be consistent downstream
 *
 * Hydra_Software_Devel/208   10/5/09 2:17p tdo
 * SW7405-3044: Set MFD HSIZE to even number
 *
 * Hydra_Software_Devel/207   9/3/09 10:31p pntruong
 * SW3548-2373: Rolled back PR56964.
 *
 * Hydra_Software_Devel/206   8/19/09 8:57a yuxiaz
 * PR52995: Need to set dynamics default to full range
 *
 * Hydra_Software_Devel/205   8/10/09 5:44p hongtaoz
 * PR57098: disable test feature1 when VFD fixed color on; don't modify
 * pPicture since it may be repeated;
 *
 * Hydra_Software_Devel/204   7/31/09 3:26p yuxiaz
 * PR57098: Disable DCX for fixed color feed. Reset DCX settings in
 * BVDC_P_Buffer_Invalidate_isr.
 *
 * Hydra_Software_Devel/203   7/21/09 2:17p syang
 * PR 56964: take off the "connect drainer, en and then dis" step  in
 * vnet-drain code
 *
 * Hydra_Software_Devel/202   7/15/09 12:12p syang
 * PR 56605: 1). feeder/cap calculate stride the same way (without even
 * pix align); 2). do even width/left align only if necessary
 *
 * Hydra_Software_Devel/201   6/11/09 4:03p darnstein
 * PR55219: fill in some details for 7340 chipset.
 *
 * Hydra_Software_Devel/200   4/6/09 1:54p yuxiaz
 * PR46252: Need to expose 444 format in static buffers case.
 *
 * Hydra_Software_Devel/199   4/3/09 4:33p yuxiaz
 * PR53460: Optimal 444/422 conversion settings
 *
 * Hydra_Software_Devel/198   3/25/09 5:54p pntruong
 * PR52524: Used the index determined by upstream block.
 *
 * Hydra_Software_Devel/197   3/18/09 9:52a yuxiaz
 * PR52840, PR53325: Fixed Coverity Defect issues.
 *
 * Hydra_Software_Devel/196   3/10/09 11:10a tdo
 * PR52995: Need to set dynamics default to full range
 *
 * Hydra_Software_Devel/195   3/9/09 1:28p pntruong
 * PR52524: Added support for hdmi rgb quantization of full range and
 * limited range.
 *
 * Hydra_Software_Devel/194   3/2/09 3:46p yuxiaz
 * PR52287: More update on 422-444 and 444-422 converters.
 *
 * Hydra_Software_Devel/193   2/26/09 7:05p pntruong
 * PR52287: Removed warnings.
 *
 * Hydra_Software_Devel/192   2/26/09 6:10p yuxiaz
 * PR52287: 422-444 and 444-422 converters should use decimate/duplicate.
 *
 * Hydra_Software_Devel/191   2/24/09 2:27p pntruong
 * PR52433: Vertical pink lines cause byderinging enabled in 7420 code.
 * DERING_EN can only be turned on iff CHROMA_REPOSITION_ENABLE=1.
 *
 * Hydra_Software_Devel/190   2/24/09 11:44a yuxiaz
 * PR51231: Set OFLOW_AVOID for DCX.
 *
 * Hydra_Software_Devel/189   2/20/09 6:50p tdo
 * PR45800: Enable MFD deringing by default
 *
 * Hydra_Software_Devel/188   1/31/09 3:33a albertl
 * PR45798: Fixed bugs and enabled mosaic mode csc implementation.
 *
 * Hydra_Software_Devel/187   1/30/09 5:09p yuxiaz
 * PR51713: Reset CAP during DCX mode switch. Clean up DCX software
 * workaround.
 *
 * Hydra_Software_Devel/186   1/28/09 3:41p yuxiaz
 * PR51335: Remove CAP overflow workaround for B2.
 *
 * Hydra_Software_Devel/185   1/16/09 6:12p yuxiaz
 * PR51231: Limit buffer size to 4Kbits to workaround cap overflow bug.
 *
 * Hydra_Software_Devel/184   12/23/08 7:35p albertl
 * PR50399: Fixed mosaic mfd cscs broken by
 * BVDC_P_Compositor_GetCscTable_isr API change.
 *
 * Hydra_Software_Devel/183   12/8/08 5:02p yuxiaz
 * PR48329: Coverity defects issues on 7405
 *
 * Hydra_Software_Devel/182   12/5/08 11:16a yuxiaz
 * PR45800: Enable Dering Filter for CMP_0 Video Surfaces on 7420.
 *
 * Hydra_Software_Devel/181   12/4/08 3:09p yuxiaz
 * PR45815: Added MFD support on 7420.
 *
 * Hydra_Software_Devel/180   11/25/08 11:12a syang
 * PR 49562: add reset to output channel ping-pong buf of sub-modules and
 * loopbacks and free-channels
 *
 * Hydra_Software_Devel/179   11/24/08 6:19p albertl
 * PR49413: Changed BVDC_P_Feeder_SetMpegInfo_isr to return error when
 * stripe width is bad in order to fix coverity warning.
 *
 * Hydra_Software_Devel/178   11/20/08 9:30a syang
 * PR 48970: clean-up module disable and drian handling so that RUL dump
 * is easy to understand
 *
 * Hydra_Software_Devel/177   10/27/08 5:37p syang
 * PR 46477: back off SetEnable in in BuildRul_isr according to GetOps (it
 * causes window to disapper on 7400 D0)
 *
 * Hydra_Software_Devel/176   10/24/08 4:07p syang
 * PR 46477: in BuildRul_isr only disable in shadow regs  according to
 * GetOps (some SetEnable_isr does HW reset)
 *
 * Hydra_Software_Devel/175   10/24/08 10:44a syang
 * PR 46477: in BuildRul_isr reset enable bit according to GetOps
 *
 * Hydra_Software_Devel/174   10/23/08 7:13p albertl
 * PR47318: Fixed change of interface for
 * BVDC_P_Compositor_GetCscTable_isr.
 *
 * Hydra_Software_Devel/173   10/23/08 11:12a yuxiaz
 * PR46305: Reset VFd during mode switch.
 *
 * Hydra_Software_Devel/172   10/17/08 4:34p yuxiaz
 * PR46305: Fix compile error.
 *
 * Hydra_Software_Devel/171   10/17/08 3:25p yuxiaz
 * PR46305: Bring up testfeature1 on 3548 B0.
 *
 * Hydra_Software_Devel/170   10/3/08 11:00a yuxiaz
 * PR46305, PR46307, PR46309: Use new TestFeature1 function from BSUR.
 * Change BVDC_P_SUPPORT_VIDEO_TestFeature1 to
 * BVDC_P_SUPPORT_VIDEO_TESTFEATURE1.
 *
 * Hydra_Software_Devel/169   10/3/08 9:24a yuxiaz
 * PR46305, PR46307, PR46309: More Video TestFeature1 support.
 *
 * Hydra_Software_Devel/168   9/29/08 12:57p pntruong
 * PR47072: Moving 3563 vdc dedicated branch.
 *
 * Hydra_Software_Devel/167   9/26/08 3:24p yuxiaz
 * PR46305, PR46307: More video TestFeature1 support for 3548 B0.
 *
 * Hydra_Software_Devel/166   9/22/08 6:05p pntruong
 * PR46305, PR46307, PR46309: Fixed build errors.
 *
 * Hydra_Software_Devel/165   9/22/08 4:52p yuxiaz
 * PR46305, PR46307, PR46309: Added video TestFeature1 support for 3548 B0.
 *
 * Hydra_Software_Devel/164   9/15/08 3:11p yuxiaz
 * PR46495: Resolve register re-arrangment for MFD/VFD on 3548 B0.
 *
 * Hydra_Software_Devel/163   9/12/08 4:22p yuxiaz
 * PR46497: Fix CRC register processing in MFD for 3548 B0.
 *
 * Hydra_Software_Devel/162   8/5/08 1:52p yuxiaz
 * PR45367: Enable Dering in VDC.
 *
 * Hydra_Software_Devel/161   7/29/08 7:49p pntruong
 * PR40213: Used correct alignment macro.
 *
 * Hydra_Software_Devel/160   7/9/08 4:25p rpan
 * PR44625: A few registers were not initialized.
 *
 * Hydra_Software_Devel/159   6/10/08 3:35p syang
 * PR 39209:  fix small issue of LBOX test with gfx input through MFD
 *
 * Hydra_Software_Devel/158   6/3/08 2:53p yuxiaz
 * PR43247: MFD: CRC Status Register Should be Updated on the "Go" Bit.
 *
 * Hydra_Software_Devel/157   5/16/08 9:39p yuxiaz
 * PR39251: Fix 10bit 444 and 10bit 422 capture on 3548.
 *
 * Hydra_Software_Devel/156   4/23/08 1:26p yuxiaz
 * PR41200: support s/w jpeg decoding and displaying
 *
 * Hydra_Software_Devel/155   3/20/08 4:22p yuxiaz
 * PR39251: Separate RUL build for MFD and VFD.
 *
 * Hydra_Software_Devel/154   3/12/08 2:26p yuxiaz
 * PR40434: Fixed KLOCWORK errors.
 *
 * Hydra_Software_Devel/153   2/22/08 2:32p yuxiaz
 * PR39155: Added de-ringing filters in BVN.
 *
 * Hydra_Software_Devel/152   2/19/08 3:44p yuxiaz
 * PR39251: Added new packed format for MFD and VDC on 3548.
 *
 * Hydra_Software_Devel/151   2/15/08 2:33p yuxiaz
 * PR34712, PR39251: Use feeder version to combine all the feeder flags.
 * Regroup feeder support based on feeder version.
 *
 * Hydra_Software_Devel/150   2/13/08 9:56p pntruong
 * PR39421: Now vdc compiled for 3548.
 *
 * Hydra_Software_Devel/149   1/24/08 5:10p pntruong
 * PR38832: Video decoding and display (noise stream) failed in customer's
 * QA testing.  Need ensure parameters from decoder to mfd is valid.
 *
 * Hydra_Software_Devel/148   1/13/08 3:30a albertl
 * PR38150: Added 7440 HDMI xvYCC CSC support.
 *
 * Hydra_Software_Devel/147   10/30/07 7:17p pntruong
 * PR34239: Allow dynamically loading of vec custom timing.
 *
 * Hydra_Software_Devel/146   10/12/07 10:28a syang
 * PR 35094: coverity (CHECKED_RETURN) fix
 *
 * Hydra_Software_Devel/145   10/8/07 2:39p pntruong
 * PR34855: Initial VDC bringup.
 *
 * Hydra_Software_Devel/144   9/27/07 2:01p yuxiaz
 * PR35013, PR34523: Add new CSC support on 7325.
 *
 * Hydra_Software_Devel/143   9/23/07 3:25p hongtaoz
 * PR34955: fixed interlaced vfd scanout programming for odd frame height
 * (480p);
 *
 * Hydra_Software_Devel/142   9/21/07 3:59p hongtaoz
 * PR34955: added Progressive Segmented Frame(PsF) 1080p support; fixed
 * 1080p pass-through condition; fixed bvdc_dbg register access error;
 * fixed general progressive capture interlaced playback feeder
 * programming problem;
 *
 * Hydra_Software_Devel/141   9/20/07 5:07p yuxiaz
 * PR35135: Fixed build errors on 7405 from csc refactoring.
 *
 * Hydra_Software_Devel/140   9/4/07 5:36p jessem
 * PR 34590: Removed use of BVDC_P_Packing and used packing type defines
 * in bchp_mfd.hinstead. Added the unsupported pixel formats to
 * BVDC_P_Feeder_SetPixelFormat_isr() and BVDC_P_Feeder_SetMpegInfo_isr()
 * to use the packing type specified for their opposite endian  pixel
 * formats.
 *
 * Hydra_Software_Devel/139   7/9/07 4:22p yuxiaz
 * PR30015: 7405 64-bit UMA: program chroma and luma line stride based on
 * stripe width.
 *
 * Hydra_Software_Devel/138   6/22/07 3:09p yuxiaz
 * PR30015: Added STRIPE_WIDTH_SEL settings in MFD.
 *
 * Hydra_Software_Devel/137   6/8/07 4:46p yuxiaz
 * PR30015: Added STRIPE_WIDTH_SEL settings in MFD.
 *
 * Hydra_Software_Devel/136   6/8/07 11:31a syang
 * PR 31849: separate drain from disable for MAD, ANR and CAP
 *
 * Hydra_Software_Devel/135   5/31/07 1:42p yuxiaz
 * PR30015: Added support in VDC to accept strip width as input parameter
 * from XVD.
 *
 * Hydra_Software_Devel/134   5/7/07 6:03p pntruong
 * PR28899: C0: BVN: VFD 10-bit 444 to 422 Mode Transition Requires Soft
 * Reset.
 *
 * Hydra_Software_Devel/133   4/29/07 9:44p hongtaoz
 * PR30177: fixed color scanout also needs to set IMAGE_FORMAT correctly;
 *
 * Hydra_Software_Devel/132   4/19/07 1:47p yuxiaz
 * PR 26052: Add CSC support in MFD on 7400 B0.
 *
 * Hydra_Software_Devel/131   4/4/07 3:44p syang
 * PR 29243: change VnetMode def (now use bit-fields)
 *
 ***************************************************************************/
#include "bstd.h"
#include "bkni.h"
#include "brdc.h"
#include "bsur.h"
#include "bvdc.h"
#include "bvdc_priv.h"
#include "bvdc_common_priv.h"
#include "bvdc_compositor_priv.h"
#include "bvdc_window_priv.h"
#include "bvdc_feeder_priv.h"
#include "bvdc_gfxfeeder_priv.h"
#include "bvdc_buffer_priv.h"
#include "bvdc_source_priv.h"
#include "bvdc_window_priv.h"
#include "bvdc_vnet_priv.h"

#include "bchp_mfd_0.h"
#include "bchp_vfd_0.h"
#include "bchp_fmisc.h"
#include "bchp_timer.h"
#include "bchp_mmisc.h"
BDBG_MODULE(BVDC_VFD);
BDBG_OBJECT_ID(BVDC_FDR);

/* HW7422-776: MFD stalls when switched from AVC format to fixed color mode */
#if (BVDC_P_SUPPORT_MFD_VER == BVDC_P_MFD_VER_10)
#define BVDC_P_MFD_FIXED_COLOR_WORKAROUND    (1)
#else
#define BVDC_P_MFD_FIXED_COLOR_WORKAROUND    (0)
#endif

#define BVDC_P_MFD_ENABLE_SKIP_FIRST_LINE    (0)

/****************************************************************************/
/* Making an entry for stripe width configurations                          */
/****************************************************************************/
#define BVDC_P_FEEDER_MAKE_STRIPE_WIDTH(eStripeWidth, ulStripeWidth, ulShift) \
{                                                                           \
	(BAVC_StripeWidth_##eStripeWidth),                                      \
	(ulStripeWidth),                                                        \
	(ulShift)                                                                \
}

/* Stripe width config table. */
static const BVDC_P_Feeder_StripeWidthConfig s_aStripeWidthCfgTbl[] =
{
	/*                             eStripeWidth   ulStripeWidth, ulShift */
	BVDC_P_FEEDER_MAKE_STRIPE_WIDTH(e64Byte,      64,            6       ),
	BVDC_P_FEEDER_MAKE_STRIPE_WIDTH(e128Byte,     128,           7       ),
	BVDC_P_FEEDER_MAKE_STRIPE_WIDTH(e256Byte,     256,           8       )
};

#define BVDC_P_FEEDER_STRIPE_WIDTH_CONFIG_TABLE_CNT       \
	(sizeof(s_aStripeWidthCfgTbl) / sizeof(BVDC_P_Feeder_StripeWidthConfig))

#if (BVDC_P_MFD_SUPPORT_3D_VIDEO)
static const BVDC_P_Feeder_VideoFormatMode s_hwOrientation[] =
{
	BVDC_P_Feeder_VideoFormatMode_e2D,  /* BFMT_Orientation_e2D */
	BVDC_P_Feeder_VideoFormatMode_e3DLR, /* BFMT_Orientation_e3D_LeftRight */
	BVDC_P_Feeder_VideoFormatMode_e3DOU, /* BFMT_Orientation_e3D_OverUnder */
	BVDC_P_Feeder_VideoFormatMode_e3DDP, /* BFMT_Orientation_e3D_Left */
	BVDC_P_Feeder_VideoFormatMode_e3DDP, /* BFMT_Orientation_e3D_Right */
	BVDC_P_Feeder_VideoFormatMode_e3DMR  /* BFMT_Orientation_eLeftRight_Enhanced */
};
#endif

/***************************************************************************
 * Static functions
 ***************************************************************************/
static void BVDC_P_Feeder_SetFixedColor_isr
	( BVDC_P_Feeder_Handle             hFeeder,
	  BAVC_Polarity                    ePolarity,
	  const uint32_t                   ulMuteColorYCrCb );

static BERR_Code BVDC_P_Feeder_SetVertWindow_isr
	( BVDC_P_Feeder_Handle             hFeeder,
	  const BAVC_Polarity              eSourcePolarity,
	  const uint32_t                   ulTop,
	  const uint32_t                   ulHeight );

#if (BVDC_P_MFD_SUPPORT_CSC)
static void BVDC_P_Feeder_SetCsc_isr
	( BVDC_P_Feeder_Handle             hFeeder,
	  const BAVC_MVD_Field            *pFieldData );
#endif


/***************************************************************************
 * {private}
 *
 */
BERR_Code BVDC_P_Feeder_Create
	( BVDC_P_Feeder_Handle            *phFeeder,
	  BRDC_Handle                      hRdc,
	  const BREG_Handle                hReg,
	  const BVDC_P_FeederId            eFeederId,
	  BTMR_TimerHandle                 hTimer,
	  BVDC_Source_Handle               hSource,
	  BVDC_P_Resource_Handle           hResource,
	  bool                             b3dSrc )
{
	BERR_Code              err = BERR_SUCCESS;
	BVDC_P_FeederContext  *pFeeder;

	BDBG_ENTER(BVDC_P_Feeder_Create);

	/* check parameters */
	if(eFeederId >= BVDC_P_FeederId_eUnknown)
	{
		BDBG_ERR(("Invalid parameter"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* Alloc the window context. */
	pFeeder = (BVDC_P_FeederContext*)(BKNI_Malloc(sizeof(BVDC_P_FeederContext)));
	if(!pFeeder)
	{
		return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
	}

	/* Clear out the context and set defaults. */
	BKNI_Memset((void*)pFeeder, 0x0, sizeof(BVDC_P_FeederContext));
	BDBG_OBJECT_SET(pFeeder, BVDC_FDR);

	/* Set up and initialization. */
	pFeeder->eId                   = eFeederId;
	pFeeder->eImageFormat          = BVDC_P_Feeder_ImageFormat_ePacked;
	pFeeder->ePxlFormat            = BPXL_INVALID;
	pFeeder->eInputOrientation     = BFMT_Orientation_e2D;
	pFeeder->eOutputOrientation    = BFMT_Orientation_e2D;
	pFeeder->hRdc                  = hRdc;
	pFeeder->hRegister             = hReg;
	pFeeder->hTimer                = hTimer;
	pFeeder->hSource               = hSource;
	pFeeder->ulTimestamp           = 0;
	BTMR_GetTimerRegisters(pFeeder->hTimer, &pFeeder->stTimerReg);

	/* Feeder reset address */
#if BVDC_P_SUPPORT_NEW_SW_INIT
	pFeeder->ulResetRegAddr = BCHP_FMISC_SW_INIT;
#else
	pFeeder->ulResetRegAddr = BCHP_FMISC_SOFT_RESET;
#endif

	switch( eFeederId )
	{
	case BVDC_P_FeederId_eMfd0:
		pFeeder->ulRegOffset = 0;
		pFeeder->ulVfd0RegOffset = 0;

#if BVDC_P_SUPPORT_NEW_SW_INIT
		pFeeder->ulResetMask = BCHP_FMISC_SW_INIT_MFD_0_MASK;
#else
		pFeeder->ulResetMask = BCHP_FMISC_SOFT_RESET_MFD_0_MASK;
#endif
		pFeeder->eImageFormat = BVDC_P_Feeder_ImageFormat_eAVC_MPEG;
		break;

	case BVDC_P_FeederId_eVfd0:
		pFeeder->ulRegOffset = BCHP_VFD_0_REG_START - BCHP_MFD_0_REG_START;
		pFeeder->ulVfd0RegOffset = 0;

#if BVDC_P_SUPPORT_NEW_SW_INIT
		pFeeder->ulResetMask = BCHP_FMISC_SW_INIT_VFD_0_MASK;
#else
		pFeeder->ulResetMask = BCHP_FMISC_SOFT_RESET_VFD_0_MASK;
#endif
		break;

#if BCHP_VFD_1_REG_START
	case BVDC_P_FeederId_eVfd1:
		pFeeder->ulRegOffset = BCHP_VFD_1_REG_START - BCHP_MFD_0_REG_START;
		pFeeder->ulVfd0RegOffset = BCHP_VFD_1_REG_START - BCHP_VFD_0_REG_START;

#if BVDC_P_SUPPORT_NEW_SW_INIT
		pFeeder->ulResetMask = BCHP_FMISC_SW_INIT_VFD_1_MASK;
#else
		pFeeder->ulResetMask = BCHP_FMISC_SOFT_RESET_VFD_1_MASK;
#endif
		break;
#endif

#if BCHP_VFD_2_REG_START
	case BVDC_P_FeederId_eVfd2:
		pFeeder->ulRegOffset = BCHP_VFD_2_REG_START - BCHP_MFD_0_REG_START;
		pFeeder->ulVfd0RegOffset = BCHP_VFD_2_REG_START - BCHP_VFD_0_REG_START;

#if BVDC_P_SUPPORT_NEW_SW_INIT
		pFeeder->ulResetMask = BCHP_FMISC_SW_INIT_VFD_2_MASK;
#else
		pFeeder->ulResetMask = BCHP_FMISC_SOFT_RESET_VFD_2_MASK;
#endif
		break;
#endif

#if BCHP_MFD_1_REG_START
	case BVDC_P_FeederId_eMfd1:
		pFeeder->ulRegOffset = BCHP_MFD_1_REG_START - BCHP_MFD_0_REG_START;
		pFeeder->ulVfd0RegOffset = 0;

#if BVDC_P_SUPPORT_NEW_SW_INIT
		pFeeder->ulResetMask = BCHP_FMISC_SW_INIT_MFD_1_MASK;
#else
		pFeeder->ulResetMask = BCHP_FMISC_SOFT_RESET_MFD_1_MASK;
#endif
		pFeeder->eImageFormat = BVDC_P_Feeder_ImageFormat_eAVC_MPEG;
		break;
#endif

#if BCHP_MFD_2_REG_START
	case BVDC_P_FeederId_eMfd2:
		pFeeder->ulRegOffset = BCHP_MFD_2_REG_START - BCHP_MFD_0_REG_START;
		pFeeder->ulVfd0RegOffset = 0;

#if BVDC_P_SUPPORT_NEW_SW_INIT
		pFeeder->ulResetMask = BCHP_FMISC_SW_INIT_MFD_2_MASK;
#else
		pFeeder->ulResetMask = BCHP_FMISC_SOFT_RESET_MFD_2_MASK;
#endif
		pFeeder->eImageFormat = BVDC_P_Feeder_ImageFormat_eAVC_MPEG;
		break;
#endif

#if BCHP_MFD_3_REG_START
	case BVDC_P_FeederId_eMfd3:
		pFeeder->ulRegOffset = BCHP_MFD_3_REG_START - BCHP_MFD_0_REG_START;
		pFeeder->ulVfd0RegOffset = 0;

#if BVDC_P_SUPPORT_NEW_SW_INIT
		pFeeder->ulResetMask = BCHP_FMISC_SW_INIT_MFD_3_MASK;
#else
		pFeeder->ulResetMask = BCHP_FMISC_SOFT_RESET_MFD_3_MASK;
#endif
		pFeeder->eImageFormat = BVDC_P_Feeder_ImageFormat_eAVC_MPEG;
		break;
#endif

#if BCHP_MFD_4_REG_START
	case BVDC_P_FeederId_eMfd4:
		pFeeder->ulRegOffset = BCHP_MFD_4_REG_START - BCHP_MFD_0_REG_START;
		pFeeder->ulVfd0RegOffset = 0;

#if BVDC_P_SUPPORT_NEW_SW_INIT
		pFeeder->ulResetMask = BCHP_FMISC_SW_INIT_MFD_4_MASK;
#else
		pFeeder->ulResetMask = BCHP_FMISC_SOFT_RESET_MFD_4_MASK;
#endif
		pFeeder->eImageFormat = BVDC_P_Feeder_ImageFormat_eAVC_MPEG;
		break;
#endif

#if BCHP_MFD_5_REG_START
	case BVDC_P_FeederId_eMfd5:
		pFeeder->ulRegOffset = BCHP_MFD_5_REG_START - BCHP_MFD_0_REG_START;
		pFeeder->ulVfd0RegOffset = 0;

#if BVDC_P_SUPPORT_NEW_SW_INIT
		pFeeder->ulResetMask = BCHP_FMISC_SW_INIT_MFD_5_MASK;
#else
		pFeeder->ulResetMask = BCHP_FMISC_SOFT_RESET_MFD_5_MASK;
#endif
		pFeeder->eImageFormat = BVDC_P_Feeder_ImageFormat_eAVC_MPEG;
		break;
#endif

#if BCHP_VFD_3_REG_START
	case BVDC_P_FeederId_eVfd3:
		pFeeder->ulRegOffset = BCHP_VFD_3_REG_START - BCHP_MFD_0_REG_START;
		pFeeder->ulVfd0RegOffset = BCHP_VFD_3_REG_START - BCHP_VFD_0_REG_START;

#if BVDC_P_SUPPORT_NEW_SW_INIT
		pFeeder->ulResetMask = BCHP_FMISC_SW_INIT_VFD_3_MASK;
#else
		pFeeder->ulResetMask = BCHP_FMISC_SOFT_RESET_VFD_3_MASK;
#endif
		break;
#endif

#if BCHP_VFD_4_REG_START
	case BVDC_P_FeederId_eVfd4:
		pFeeder->ulRegOffset = BCHP_VFD_4_REG_START - BCHP_MFD_0_REG_START;
		pFeeder->ulVfd0RegOffset = BCHP_VFD_4_REG_START - BCHP_VFD_0_REG_START;

#if BVDC_P_SUPPORT_NEW_SW_INIT
		pFeeder->ulResetMask = BCHP_FMISC_SW_INIT_VFD_4_MASK;
#else
		pFeeder->ulResetMask = BCHP_FMISC_SOFT_RESET_VFD_4_MASK;
#endif
		break;
#endif

#if BCHP_VFD_5_REG_START
	case BVDC_P_FeederId_eVfd5:
		pFeeder->ulRegOffset = BCHP_VFD_5_REG_START - BCHP_MFD_0_REG_START;
		pFeeder->ulVfd0RegOffset = BCHP_VFD_5_REG_START - BCHP_VFD_0_REG_START;

#if BVDC_P_SUPPORT_NEW_SW_INIT
		pFeeder->ulResetMask = BCHP_FMISC_SW_INIT_VFD_5_MASK;
#else
		pFeeder->ulResetMask = BCHP_FMISC_SOFT_RESET_VFD_5_MASK;
#endif
		break;
#endif

#if BCHP_VFD_6_REG_START
	case BVDC_P_FeederId_eVfd6:
		pFeeder->ulRegOffset = BCHP_VFD_6_REG_START - BCHP_MFD_0_REG_START;
		pFeeder->ulVfd0RegOffset = BCHP_VFD_6_REG_START - BCHP_VFD_0_REG_START;

#if BVDC_P_SUPPORT_NEW_SW_INIT
		pFeeder->ulResetMask = BCHP_FMISC_SW_INIT_VFD_6_MASK;
#else
		pFeeder->ulResetMask = BCHP_FMISC_SOFT_RESET_VFD_6_MASK;
#endif
		break;
#endif

#if BCHP_VFD_7_REG_START
	case BVDC_P_FeederId_eVfd7:
		pFeeder->ulRegOffset = BCHP_VFD_7_REG_START - BCHP_MFD_0_REG_START;
		pFeeder->ulVfd0RegOffset = BCHP_VFD_7_REG_START - BCHP_VFD_0_REG_START;

#if BVDC_P_SUPPORT_NEW_SW_INIT
		pFeeder->ulResetMask = BCHP_FMISC_SW_INIT_VFD_7_MASK;
#else
		pFeeder->ulResetMask = BCHP_FMISC_SOFT_RESET_VFD_7_MASK;
#endif
		break;
#endif

	default:
			BDBG_ASSERT(false);
	}

#if BVDC_P_SUPPORT_NEW_SW_INIT
	pFeeder->ulVnetResetAddr = BCHP_MMISC_VNET_F_CHANNEL_SW_INIT;
#elif BCHP_MMISC_VNET_F_CHANNEL_RESET
	pFeeder->ulVnetResetAddr = BCHP_MMISC_VNET_F_CHANNEL_RESET;
#endif
	if(BVDC_P_Feeder_IS_MPEG(pFeeder))
	{
#if BVDC_P_SUPPORT_NEW_SW_INIT
		pFeeder->ulVnetResetMask = BCHP_MMISC_VNET_F_CHANNEL_SW_INIT_MFD_0_MASK<<(eFeederId - BVDC_P_FeederId_eMfd0);
#elif BCHP_MMISC_VNET_F_CHANNEL_RESET
		pFeeder->ulVnetResetMask = BCHP_MMISC_VNET_F_CHANNEL_RESET_MFD_0_RESET_MASK<<(eFeederId - BVDC_P_FeederId_eMfd0);
#endif
	}
	else
	{
#if BVDC_P_SUPPORT_NEW_SW_INIT
		pFeeder->ulVnetResetMask = BCHP_MMISC_VNET_F_CHANNEL_SW_INIT_VFD_0_MASK<<(eFeederId - BVDC_P_FeederId_eVfd0);
#elif BCHP_MMISC_VNET_F_CHANNEL_RESET
		pFeeder->ulVnetResetMask = BCHP_MMISC_VNET_F_CHANNEL_RESET_VFD_0_RESET_MASK<<(eFeederId - BVDC_P_FeederId_eVfd0);
#endif
	}
#if (BVDC_P_MFD_NEED_CRC_WORKAROUND)
	/* MPEG feeder only */
	if(BVDC_P_Feeder_IS_MPEG(pFeeder))
	{
		pFeeder->ulLumaCrcRegAddr = BRDC_AllocScratchReg(pFeeder->hRdc);
		if(!pFeeder->ulLumaCrcRegAddr)
		{
			BDBG_ERR(("Not enough scratch registers for Luma Crc!"));
			BDBG_OBJECT_DESTROY(pFeeder, BVDC_FDR);
			BKNI_Free((void*)pFeeder);
			return BERR_TRACE(BERR_INVALID_PARAMETER);
		}

		pFeeder->ulChromaCrcRegAddr = BRDC_AllocScratchReg(pFeeder->hRdc);
		if(!pFeeder->ulChromaCrcRegAddr)
		{
			BDBG_ERR(("Not enough scratch registers for Chroma Crc!"));
			BDBG_OBJECT_DESTROY(pFeeder, BVDC_FDR);
			BKNI_Free((void*)pFeeder);
			return BERR_TRACE(BERR_INVALID_PARAMETER);
		}
	}
#endif
	/* init the SubRul sub-module */
	BVDC_P_SubRul_Init(&(pFeeder->SubRul), 0 /* 0 means it has no src mux */,
		BVDC_P_Feeder_PostMuxValue(pFeeder), BVDC_P_DrainMode_eFront, 0, hResource);

	/* for VFD, hFeeder->hSource is NOT NULL iff hFeeder is created by
	 * BVDC_P_Source_Create */
	if(BVDC_P_Feeder_IS_MPEG(pFeeder) ||
	   (BVDC_P_Feeder_IS_VFD(pFeeder) && pFeeder->hSource))
	{
		/* init stuff in surface manager */
		pFeeder->stGfxSurface.hRegister = hReg;
		pFeeder->stGfxSurface.eSrcId = BAVC_SourceId_eVfd0 + (eFeederId - BVDC_P_FeederId_eVfd0);
		pFeeder->stGfxSurface.ulHwMainSurAddrReg =
			BCHP_MFD_0_PICTURE0_LINE_ADDR_0 + pFeeder->ulRegOffset;

		/* allocate shadow registers for surface manager from scratch pool */
		BVDC_P_GfxSurface_AllocShadowRegs(
			&pFeeder->stGfxSurface, pFeeder->hRdc, b3dSrc);
		BDBG_MSG(("alloc regs for feeder %d", pFeeder->eId));
	}

	/* All done. now return the new fresh context to user. */
	*phFeeder = (BVDC_P_Feeder_Handle)pFeeder;

	BDBG_LEAVE(BVDC_P_Feeder_Create);
	return err;
}

/***************************************************************************
 * {private}
 *
 */
void BVDC_P_Feeder_Destroy
	( BVDC_P_Feeder_Handle             hFeeder )
{
	BDBG_ENTER(BVDC_P_Feeder_Destroy);
	BDBG_OBJECT_ASSERT(hFeeder, BVDC_FDR);

#if (BVDC_P_MFD_NEED_CRC_WORKAROUND)
	if(BVDC_P_Feeder_IS_MPEG(hFeeder))
	{
		BRDC_FreeScratchReg(hFeeder->hRdc, hFeeder->ulLumaCrcRegAddr);
		BRDC_FreeScratchReg(hFeeder->hRdc, hFeeder->ulChromaCrcRegAddr);
	}
#endif

	/* for VFD, hFeeder->hSource is NOT NULL iff hFeeder is created by
	 * BVDC_P_Source_Create */
	if(BVDC_P_Feeder_IS_MPEG(hFeeder) ||
	   (BVDC_P_Feeder_IS_VFD(hFeeder) && hFeeder->hSource))
	{
		/* free surface address shadow registers back to scratch pool */
		BVDC_P_GfxSurface_FreeShadowRegs(
			&hFeeder->stGfxSurface, hFeeder->hRdc);
	}

	BDBG_OBJECT_DESTROY(hFeeder, BVDC_FDR);
	BKNI_Free((void*)hFeeder);

	BDBG_LEAVE(BVDC_P_Feeder_Destroy);
	return;
}


/***************************************************************************
 * {private}
 *
 */
void BVDC_P_Feeder_Init
	( BVDC_P_Feeder_Handle             hFeeder,
	  BMEM_Heap_Handle                 hMem,
	  bool                             bGfxSrc )
{
	BVDC_P_PictureNode *pPicture;

	BDBG_ENTER(BVDC_P_Feeder_Init);
	BDBG_OBJECT_ASSERT(hFeeder, BVDC_FDR);

	/* Clear out shadow registers. */
	BKNI_Memset((void*)hFeeder->aulRegs, 0x0, sizeof(hFeeder->aulRegs));

	/* Assign the new heap */
	hFeeder->hMem = hMem;

	hFeeder->stUpSampler.bUnbiasedRound = false;
	hFeeder->stUpSampler.eFilterType    = BVDC_422To444Filter_eTenTaps;
	hFeeder->stUpSampler.eRingRemoval   = BVDC_RingSuppressionMode_eDisable;
	hFeeder->ulThroughput = 0;
#if (BVDC_P_MFD_ENABLE_SKIP_FIRST_LINE)
	hFeeder->bSkipFirstLine = true;
#else
	hFeeder->bSkipFirstLine = false;
#endif

	if(BVDC_P_Feeder_IS_MPEG(hFeeder))
	{
		/* set init flag for MFD only */
		hFeeder->bInitial = true;
		/* enable CRC, in display mode and load seed on every vsync
		 * TODO: support N-field CRC checking; */
		BVDC_P_MFD_GET_REG_DATA(MFD_0_CRC_CTRL ) =
			BCHP_FIELD_ENUM(MFD_0_CRC_CTRL , ENABLE, ON)    |
			BCHP_FIELD_ENUM(MFD_0_CRC_CTRL , MODE, DISPLAY) |
			BCHP_FIELD_ENUM(MFD_0_CRC_CTRL , LOAD_CRC_SEED, AT_SOF);

		/* Initial seed value should be all '1's */
		BVDC_P_MFD_GET_REG_DATA(MFD_0_CRC_SEED) = 0xFFFFFFFF;

#if (BVDC_P_SUPPORT_MFD_VER >= BVDC_P_MFD_VER_3) && (BVDC_P_SUPPORT_MFD_VER <= BVDC_P_MFD_VER_5)
		BVDC_P_MFD_GET_REG_DATA(MFD_0_US_422_TO_444) =
			BCHP_FIELD_ENUM(MFD_0_US_422_TO_444, UNBIASED_ROUND_ENABLE, DISABLE)	|
			BCHP_FIELD_ENUM(MFD_0_US_422_TO_444, FILT_CTRL, CHROMA_DUPLICATION);

		BVDC_P_MFD_GET_REG_DATA(MFD_0_CSC_CNTL) =
			BCHP_FIELD_ENUM(MFD_0_CSC_CNTL, CSC_ENABLE, OFF);

		BVDC_P_MFD_GET_REG_DATA(MFD_0_DS_CNTL) =
			BCHP_FIELD_ENUM(MFD_0_DS_CNTL, RING_SUPPRESSION, DISABLE)	|
			BCHP_FIELD_ENUM(MFD_0_DS_CNTL, FILTER_TYPE, FILTER_TYPE_DECIMATE);
#endif

#if ((BVDC_P_SUPPORT_MFD_VER >= BVDC_P_MFD_VER_13) && (BVDC_P_MFD_SUPPORT_3D_VIDEO_MRE))
		if(hFeeder->eId == BVDC_P_FeederId_eMfd0)
		{
			/* MRE filter settings */
			BVDC_P_MFD_GET_REG_DATA(MFD_0_MRE3D_SYN_FILT_CTRL) =
				BCHP_FIELD_DATA(MFD_0_MRE3D_SYN_FILT_CTRL, AFFINE_XFRM_OFFSET,      128  ) |
				BCHP_FIELD_DATA(MFD_0_MRE3D_SYN_FILT_CTRL, AFFINE_XFRM_SLOPE_SCALE, 0x400) |
				BCHP_FIELD_DATA(MFD_0_MRE3D_SYN_FILT_CTRL, HP_INIT_PHASE,           3    ) |
				BCHP_FIELD_DATA(MFD_0_MRE3D_SYN_FILT_CTRL, LP_INIT_PHASE,           0    );

			BVDC_P_MFD_GET_REG_DATA(MFD_0_MRE3D_LP_PH0_COEFF_0_1) =
				BCHP_FIELD_DATA(MFD_0_MRE3D_LP_PH0_COEFF_0_1, COEFF_0, 0 ) |
				BCHP_FIELD_DATA(MFD_0_MRE3D_LP_PH0_COEFF_0_1, COEFF_1, 0 );
			BVDC_P_MFD_GET_REG_DATA(MFD_0_MRE3D_LP_PH0_COEFF_2_3) =
				BCHP_FIELD_DATA(MFD_0_MRE3D_LP_PH0_COEFF_2_3, COEFF_2, 0   ) |
				BCHP_FIELD_DATA(MFD_0_MRE3D_LP_PH0_COEFF_2_3, COEFF_3, -93 );
			BVDC_P_MFD_GET_REG_DATA(MFD_0_MRE3D_LP_PH0_COEFF_4_5) =
				BCHP_FIELD_DATA(MFD_0_MRE3D_LP_PH0_COEFF_4_5, COEFF_4, 605 ) |
				BCHP_FIELD_DATA(MFD_0_MRE3D_LP_PH0_COEFF_4_5, COEFF_5, 605 );
			BVDC_P_MFD_GET_REG_DATA(MFD_0_MRE3D_LP_PH0_COEFF_6_7) =
				BCHP_FIELD_DATA(MFD_0_MRE3D_LP_PH0_COEFF_6_7, COEFF_6, -93 ) |
				BCHP_FIELD_DATA(MFD_0_MRE3D_LP_PH0_COEFF_6_7, COEFF_7, 0   );
			BVDC_P_MFD_GET_REG_DATA(MFD_0_MRE3D_LP_PH0_COEFF_8) =
				BCHP_FIELD_DATA(MFD_0_MRE3D_LP_PH0_COEFF_8, COEFF_8, 0 );

			BVDC_P_MFD_GET_REG_DATA(MFD_0_MRE3D_LP_PH1_COEFF_0_1) =
				BCHP_FIELD_DATA(MFD_0_MRE3D_LP_PH1_COEFF_0_1, COEFF_0, 0 ) |
				BCHP_FIELD_DATA(MFD_0_MRE3D_LP_PH1_COEFF_0_1, COEFF_1, 0 );
			BVDC_P_MFD_GET_REG_DATA(MFD_0_MRE3D_LP_PH1_COEFF_2_3) =
				BCHP_FIELD_DATA(MFD_0_MRE3D_LP_PH1_COEFF_2_3, COEFF_2, 0   ) |
				BCHP_FIELD_DATA(MFD_0_MRE3D_LP_PH1_COEFF_2_3, COEFF_3, -59 );
			BVDC_P_MFD_GET_REG_DATA(MFD_0_MRE3D_LP_PH1_COEFF_4_5) =
				BCHP_FIELD_DATA(MFD_0_MRE3D_LP_PH1_COEFF_4_5, COEFF_4, 1142 ) |
				BCHP_FIELD_DATA(MFD_0_MRE3D_LP_PH1_COEFF_4_5, COEFF_5, -59 );
			BVDC_P_MFD_GET_REG_DATA(MFD_0_MRE3D_LP_PH1_COEFF_6_7) =
				BCHP_FIELD_DATA(MFD_0_MRE3D_LP_PH1_COEFF_6_7, COEFF_6, 0 ) |
				BCHP_FIELD_DATA(MFD_0_MRE3D_LP_PH1_COEFF_6_7, COEFF_7, 0   );
			BVDC_P_MFD_GET_REG_DATA(MFD_0_MRE3D_LP_PH1_COEFF_8) =
				BCHP_FIELD_DATA(MFD_0_MRE3D_LP_PH1_COEFF_8, COEFF_8, 0 );

			BVDC_P_MFD_GET_REG_DATA(MFD_0_MRE3D_HP_PH0_COEFF_0_1) =
				BCHP_FIELD_DATA(MFD_0_MRE3D_HP_PH0_COEFF_0_1, COEFF_0, 0 ) |
				BCHP_FIELD_DATA(MFD_0_MRE3D_HP_PH0_COEFF_0_1, COEFF_1, 0 );
			BVDC_P_MFD_GET_REG_DATA(MFD_0_MRE3D_HP_PH0_COEFF_2_3) =
				BCHP_FIELD_DATA(MFD_0_MRE3D_HP_PH0_COEFF_2_3, COEFF_2, 0   ) |
				BCHP_FIELD_DATA(MFD_0_MRE3D_HP_PH0_COEFF_2_3, COEFF_3, 17 );
			BVDC_P_MFD_GET_REG_DATA(MFD_0_MRE3D_HP_PH0_COEFF_4_5) =
				BCHP_FIELD_DATA(MFD_0_MRE3D_HP_PH0_COEFF_4_5, COEFF_4, -273 ) |
				BCHP_FIELD_DATA(MFD_0_MRE3D_HP_PH0_COEFF_4_5, COEFF_5, -273 );
			BVDC_P_MFD_GET_REG_DATA(MFD_0_MRE3D_HP_PH0_COEFF_6_7) =
				BCHP_FIELD_DATA(MFD_0_MRE3D_HP_PH0_COEFF_6_7, COEFF_6, -17 ) |
				BCHP_FIELD_DATA(MFD_0_MRE3D_HP_PH0_COEFF_6_7, COEFF_7, 0   );
			BVDC_P_MFD_GET_REG_DATA(MFD_0_MRE3D_HP_PH0_COEFF_8) =
				BCHP_FIELD_DATA(MFD_0_MRE3D_HP_PH0_COEFF_8, COEFF_8, 0 );

			BVDC_P_MFD_GET_REG_DATA(MFD_0_MRE3D_HP_PH1_COEFF_0_1) =
				BCHP_FIELD_DATA(MFD_0_MRE3D_HP_PH1_COEFF_0_1, COEFF_0, 0 ) |
				BCHP_FIELD_DATA(MFD_0_MRE3D_HP_PH1_COEFF_0_1, COEFF_1, 0 );
			BVDC_P_MFD_GET_REG_DATA(MFD_0_MRE3D_HP_PH1_COEFF_2_3) =
				BCHP_FIELD_DATA(MFD_0_MRE3D_HP_PH1_COEFF_2_3, COEFF_2, 0   ) |
				BCHP_FIELD_DATA(MFD_0_MRE3D_HP_PH1_COEFF_2_3, COEFF_3, -80 );
			BVDC_P_MFD_GET_REG_DATA(MFD_0_MRE3D_HP_PH1_COEFF_4_5) =
				BCHP_FIELD_DATA(MFD_0_MRE3D_HP_PH1_COEFF_4_5, COEFF_4, 617 ) |
				BCHP_FIELD_DATA(MFD_0_MRE3D_HP_PH1_COEFF_4_5, COEFF_5, -80 );
			BVDC_P_MFD_GET_REG_DATA(MFD_0_MRE3D_HP_PH1_COEFF_6_7) =
				BCHP_FIELD_DATA(MFD_0_MRE3D_HP_PH1_COEFF_6_7, COEFF_6, 0 ) |
				BCHP_FIELD_DATA(MFD_0_MRE3D_HP_PH1_COEFF_6_7, COEFF_7, 0   );
			BVDC_P_MFD_GET_REG_DATA(MFD_0_MRE3D_HP_PH1_COEFF_8) =
				BCHP_FIELD_DATA(MFD_0_MRE3D_HP_PH1_COEFF_8, COEFF_8, 0 );

			BVDC_P_MFD_GET_REG_DATA(MFD_0_MRE3D_AVG_FILT_CTRL) =
				BCHP_FIELD_DATA(MFD_0_MRE3D_AVG_FILT_CTRL, AVG_FILT_ENABLE,   0    ) |
				BCHP_FIELD_DATA(MFD_0_MRE3D_AVG_FILT_CTRL, ALPHA_DENOM_SCALE, 0x200) |
				BCHP_FIELD_DATA(MFD_0_MRE3D_AVG_FILT_CTRL, IN_MAX_ABS_THRESH, 0x4  ) |
				BCHP_FIELD_DATA(MFD_0_MRE3D_AVG_FILT_CTRL, IN_ABS_AVG_THRESH, 0x8  );
		}
#endif

	}

	BVDC_P_MFD_GET_REG_DATA(MFD_0_TEST_MODE_CNTL) = 0x0;

#if (BVDC_P_SUPPORT_VIDEO_TESTFEATURE1_CAP)
	BKNI_Memset((void*)&hFeeder->stTestFeature1, 0x0,
		sizeof(BVDC_P_TestFeature1_Settings));
#endif

	hFeeder->bGfxSrc = bGfxSrc;
	if(bGfxSrc)
	{
		/* Init picture node used when VFD is used to input gfx surface and
		 * works as an intial source of a video window. MFD only uses
		 * pPicture->eSrcPolarity in BVDC_Feeder_BuildRulGfxSurAddr_isr */
		pPicture = &hFeeder->stPicture;

		BKNI_Memset((void*)pPicture, 0x0, sizeof(BVDC_P_PictureNode));

		pPicture->pHeapNode            = NULL;
		BKNI_Memset((void*)&pPicture->stFlags, 0x0, sizeof(BVDC_P_PicNodeFlags));
		pPicture->stFlags.bMute           = true;
		pPicture->stFlags.bCadMatching    = false;

		BVDC_P_CLEAN_ALL_DIRTY(&pPicture->stVnetMode);
		pPicture->stVnetMode.stBits.bInvalid  = BVDC_P_ON;
		pPicture->eFrameRateCode       = BAVC_FrameRateCode_e29_97;
		pPicture->eMatrixCoefficients  = BAVC_MatrixCoefficients_eSmpte_170M;
		pPicture->eDisplayPolarity     = BAVC_Polarity_eTopField;
		pPicture->eSrcPolarity         = BAVC_Polarity_eTopField;
		pPicture->PicComRulInfo.eSrcOrigPolarity = BAVC_Polarity_eTopField;
		pPicture->eDstPolarity         = BAVC_Polarity_eTopField;
		pPicture->hSurface             = NULL;
		pPicture->hSurface_R           = NULL;

		pPicture->stSrcOut.lLeft       = 0;
		pPicture->stSrcOut.lLeft_R     = 0;
		pPicture->stSrcOut.lTop        = 0;
		pPicture->stSrcOut.ulWidth     = BFMT_NTSC_WIDTH;
		pPicture->stSrcOut.ulHeight    = BFMT_NTSC_HEIGHT;

		pPicture->stSclOut             = pPicture->stSrcOut;
		pPicture->stWinOut             = pPicture->stSrcOut;
		pPicture->stSclCut             = pPicture->stSrcOut;
		pPicture->stVfdOut             = pPicture->stSrcOut;

		pPicture->pSrcOut              = &pPicture->stSrcOut;

		pPicture->pSclIn               = &pPicture->stSrcOut;
		pPicture->pSclOut              = &pPicture->stSclOut;
		pPicture->pWinOut              = &pPicture->stWinOut;

		pPicture->pVfdIn               = &pPicture->stSrcOut;
		pPicture->pVfdOut              = &pPicture->stSrcOut;

		/* init surface manager */
		BVDC_P_GfxSurface_Init(&hFeeder->stGfxSurface);
	}

	BDBG_LEAVE(BVDC_P_Feeder_Init);
	return;
}

/***************************************************************************
 * {private}
 *
 * BVDC_P_Feeder_BuildRul_DrainVnet_isr
 *
 * called by BVDC_P_Feeder_BuildRul_isr after resetting to drain the module and
 * its pre-patch FreeCh or LpBack
 */
static void BVDC_P_Feeder_BuildRul_DrainVnet_isr(
	BVDC_P_Feeder_Handle           hFeeder,
	BVDC_P_ListInfo               *pList )
{

	if(hFeeder->eId < BVDC_P_FeederId_eVfd0)
	{
		/* Need to re-enable CRC for MFD */
		hFeeder->bInitial = true;
	}

	/* reset sub and connect the module to a drain */
	BVDC_P_SubRul_StartDrain_isr(&(hFeeder->SubRul), pList,
		hFeeder->ulResetRegAddr, hFeeder->ulResetMask);

	/* disconnect the module from the drain; reset it again; and
	 * reset the channel ping-pong buf */
	BVDC_P_SubRul_FinishDrain_isr(&(hFeeder->SubRul), pList,
		hFeeder->ulResetRegAddr, hFeeder->ulResetMask,
		hFeeder->ulVnetResetAddr, hFeeder->ulVnetResetMask);
}

static void BVDC_Feeder_BuildRulGfxSurAddr_isr
	( BVDC_P_Feeder_Handle             hFeeder,
	  BVDC_P_ListInfo                 *pList );

/***************************************************************************
 * {private}
 *
 */
static void BVDC_P_Feeder_BuildRulMfd_isr
	( BVDC_P_Feeder_Handle             hFeeder,
	  BVDC_P_ListInfo                 *pList )
{
	BDBG_ASSERT(BVDC_P_Feeder_IS_MPEG(hFeeder));

#if (BVDC_P_SUPPORT_MFD_VER >= BVDC_P_MFD_VER_3) && (BVDC_P_SUPPORT_MFD_VER <= BVDC_P_MFD_VER_5)

	BVDC_P_MFD_BLOCK_WRITE_TO_RUL(MFD_0_RANGE_EXP_REMAP_CNTL, MFD_0_DS_CNTL,
		pList->pulCurrent);

#elif (BVDC_P_SUPPORT_MFD_VER == BVDC_P_MFD_VER_6)
	BVDC_P_MFD_BLOCK_WRITE_TO_RUL(MFD_0_BYTE_ORDER,
		MFD_0_CHROMA_REPOSITION_DERING_ENABLE, pList->pulCurrent);

	BVDC_P_MFD_BLOCK_WRITE_TO_RUL(MFD_0_DATA_MODE, MFD_0_RANGE_EXP_REMAP_CNTL,
		pList->pulCurrent);

#elif (BVDC_P_SUPPORT_MFD_VER <= BVDC_P_MFD_VER_9)
	BVDC_P_MFD_BLOCK_WRITE_TO_RUL(MFD_0_PICTURE0_LINE_ADDR_1,
		MFD_0_CHROMA_NMBY, pList->pulCurrent);

	BVDC_P_MFD_WRITE_TO_RUL(MFD_0_CHROMA_REPOSITION_DERING_ENABLE, pList->pulCurrent);
	BVDC_P_MFD_WRITE_TO_RUL(MFD_0_RANGE_EXP_REMAP_CNTL, pList->pulCurrent);

#elif (BVDC_P_SUPPORT_MFD_VER <= BVDC_P_MFD_VER_12)
	BVDC_P_MFD_BLOCK_WRITE_TO_RUL(MFD_0_PICTURE0_LINE_ADDR_1,
		MFD_0_CHROMA_NMBY, pList->pulCurrent);

	BVDC_P_MFD_WRITE_TO_RUL(MFD_0_CHROMA_REPOSITION_DERING_ENABLE, pList->pulCurrent);
	BVDC_P_MFD_WRITE_TO_RUL(MFD_0_RANGE_EXP_REMAP_CNTL, pList->pulCurrent);
	BVDC_P_MFD_BLOCK_WRITE_TO_RUL(MFD_0_PICTURE0_LINE_ADDR_0_R,
		MFD_0_PICTURE0_LINE_ADDR_1_R, pList->pulCurrent);

#elif ((BVDC_P_SUPPORT_MFD_VER >= BVDC_P_MFD_VER_13) && (BVDC_P_MFD_SUPPORT_3D_VIDEO_MRE))
	BVDC_P_MFD_BLOCK_WRITE_TO_RUL(MFD_0_PICTURE0_LINE_ADDR_1,
		MFD_0_CHROMA_NMBY, pList->pulCurrent);

	BVDC_P_MFD_WRITE_TO_RUL(MFD_0_CHROMA_REPOSITION_DERING_ENABLE, pList->pulCurrent);
	BVDC_P_MFD_WRITE_TO_RUL(MFD_0_RANGE_EXP_REMAP_CNTL, pList->pulCurrent);
	BVDC_P_MFD_BLOCK_WRITE_TO_RUL(MFD_0_PICTURE0_LINE_ADDR_0_R,
		MFD_0_PICTURE0_LINE_ADDR_1_R, pList->pulCurrent);

	if(hFeeder->eId == BVDC_P_FeederId_eMfd0)
	{
		BVDC_P_MFD_BLOCK_WRITE_TO_RUL(MFD_0_MRE3D_LINE_ADDR_0_HP,
			MFD_0_MRE3D_LINE_ADDR_1_R_HP, pList->pulCurrent);

		BVDC_P_MFD_BLOCK_WRITE_TO_RUL(MFD_0_MRE3D_SYN_FILT_CTRL,
			MFD_0_MRE3D_AVG_FILT_CTRL, pList->pulCurrent);
	}

#endif

#if (BVDC_P_MFD_NEED_CRC_WORKAROUND)
	/* Read and store CRC values. Need to read before feeder is enabled */
	*pList->pulCurrent++ = BRDC_OP_REG_TO_REG( 1 );
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_MFD_0_LUMA_CRC + hFeeder->ulRegOffset);
	*pList->pulCurrent++ = BRDC_REGISTER(hFeeder->ulLumaCrcRegAddr);

	*pList->pulCurrent++ = BRDC_OP_REG_TO_REG( 1 );
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_MFD_0_CHROMA_CRC + hFeeder->ulRegOffset);
	*pList->pulCurrent++ = BRDC_REGISTER(hFeeder->ulChromaCrcRegAddr);

#endif

	BVDC_P_MFD_WRITE_TO_RUL(MFD_0_TEST_MODE_CNTL, pList->pulCurrent);

	if (hFeeder->bGfxSrc)
	{
#if (BVDC_P_SUPPORT_MFD_VER<=BVDC_P_MFD_VER_6)
		BVDC_P_MFD_BLOCK_WRITE_TO_RUL(MFD_0_FEEDER_CNTL, MFD_0_CHROMA_NMBY,
			pList->pulCurrent);
#else
		BVDC_P_MFD_BLOCK_WRITE_TO_RUL(MFD_0_FEEDER_CNTL, MFD_0_BYTE_ORDER,
			pList->pulCurrent);
#endif

		BVDC_Feeder_BuildRulGfxSurAddr_isr(hFeeder, pList);

		BVDC_P_MFD_WRITE_TO_RUL(MFD_0_PIC_FEED_CMD, pList->pulCurrent);
	}
	else
	{
		BVDC_P_MFD_BLOCK_WRITE_TO_RUL(MFD_0_FEEDER_CNTL, MFD_0_PIC_FEED_CMD,
			pList->pulCurrent);
	}

	return;
}


#if (BVDC_P_SUPPORT_VIDEO_TESTFEATURE1_CAP)
/***************************************************************************
 * {private}
 *
 */
static void BVDC_P_Feeder_BuildRulVfdDcx_isr
	( BVDC_P_Feeder_Handle             hFeeder,
	  BVDC_P_ListInfo                 *pList )
	{
		BVDC_P_TestFeature1_Settings   *pTestFeature1 = &hFeeder->stTestFeature1;

		/* DCx is not in MFD !!!.
		 * Can not use shadow registers settings in MFD. Use the settings saved
		 * in hFeeder->pTestFeature1 */
	BVDC_P_SUBRUL_START_BLOCK(pList, BCHP_VFD_0_DCD_PRED_CFG, hFeeder->ulVfd0RegOffset,
			((BCHP_VFD_0_DCD_COMPR_CFG1 - BCHP_VFD_0_DCD_PRED_CFG) / 4) + 1);

		/* VFD_0_DCD_PRED_CFG */
		*pList->pulCurrent++ = (
			BCHP_FIELD_DATA(VFD_0_DCD_PRED_CFG, ENABLE,
				pTestFeature1->bEnable &&
				/* disable DCD for fixed color */
				BVDC_P_MFD_COMPARE_FIELD_DATA(MFD_0_FEEDER_CNTL, FIXED_COLOUR_ENABLE, 0)) |
			BCHP_FIELD_ENUM(VFD_0_DCD_PRED_CFG, CONVERT_RGB,     Disable ) |
			BCHP_FIELD_DATA(VFD_0_DCD_PRED_CFG, PREDICTION_MODE, pTestFeature1->ulPredictionMode) |
			BCHP_FIELD_ENUM(VFD_0_DCD_PRED_CFG, EDGE_PRED_ENA,   Enable  ) |
			BCHP_FIELD_ENUM(VFD_0_DCD_PRED_CFG, LEFT_PRED_ENA,   Enable  ) |
			BCHP_FIELD_ENUM(VFD_0_DCD_PRED_CFG, ABCD_PRED_ENA,   Enable  ) |
			BCHP_FIELD_ENUM(VFD_0_DCD_PRED_CFG, LS_PRED_ENA,     Enable  ));

		/* VFD_0_DCD_COMPR_CFG1 */
		*pList->pulCurrent++ = (
			BCHP_FIELD_DATA(VFD_0_DCD_COMPR_CFG1, PIXELS_PER_GROUP,  pTestFeature1->ulPixelPerGroup) |
			BCHP_FIELD_DATA(VFD_0_DCD_COMPR_CFG1, TGT_OFFSET_HI,     0xfa   ) |
			BCHP_FIELD_DATA(VFD_0_DCD_COMPR_CFG1, TGT_OFFSET_LO,     12     ) |
			BCHP_FIELD_DATA(VFD_0_DCD_COMPR_CFG1, TGT_BPG, pTestFeature1->ulBitsPerGroup));

#if (BVDC_P_DCX_CAP_OVERFLOW_WORKAROUND)
		/* Limit buffer size to 4Kbits to workaround cap overflow bug */
	BVDC_P_SUBRUL_START_BLOCK(pList, BCHP_VFD_0_DCD_COMPR_CFG2, hFeeder->ulVfd0RegOffset,
			((BCHP_VFD_0_DCD_RANGE14 - BCHP_VFD_0_DCD_COMPR_CFG2) / 4) + 1);

		/* VFD_0_DCD_COMPR_CFG2 */
		*pList->pulCurrent++ = (
			BCHP_FIELD_DATA(VFD_0_DCD_COMPR_CFG2, OFLOW_AVOID,       11 ) |
			BCHP_FIELD_DATA(VFD_0_DCD_COMPR_CFG2, RC_EDGE_FACTOR,    6  ) |
			BCHP_FIELD_DATA(VFD_0_DCD_COMPR_CFG2, QUANT_INCR_LIMIT1, 11 ) |
			BCHP_FIELD_DATA(VFD_0_DCD_COMPR_CFG2, QUANT_INCR_LIMIT0, 11 ));

		/* VFD_0_DCD_RANGE0 */
		*pList->pulCurrent++ = (
			BCHP_FIELD_DATA(VFD_0_DCD_RANGE0, BPG_OFFSET, 20  ) |
			BCHP_FIELD_DATA(VFD_0_DCD_RANGE0, MAX_QP,     2   ) |
			BCHP_FIELD_DATA(VFD_0_DCD_RANGE0, MIN_QP,     0   ) |
			BCHP_FIELD_DATA(VFD_0_DCD_RANGE0, THRESHOLD,  448));

		/* VFD_0_DCD_RANGE1 */
		*pList->pulCurrent++ = (
			BCHP_FIELD_DATA(VFD_0_DCD_RANGE1, BPG_OFFSET, 10  ) |
			BCHP_FIELD_DATA(VFD_0_DCD_RANGE1, MAX_QP,     2   ) |
			BCHP_FIELD_DATA(VFD_0_DCD_RANGE1, MIN_QP,     0   ) |
			BCHP_FIELD_DATA(VFD_0_DCD_RANGE1, THRESHOLD,  896));

		/* VFD_0_DCD_RANGE2 */
		*pList->pulCurrent++ = (
			BCHP_FIELD_DATA(VFD_0_DCD_RANGE2, BPG_OFFSET, 0   ) |
			BCHP_FIELD_DATA(VFD_0_DCD_RANGE2, MAX_QP,     2   ) |
			BCHP_FIELD_DATA(VFD_0_DCD_RANGE2, MIN_QP,     1   ) |
			BCHP_FIELD_DATA(VFD_0_DCD_RANGE2, THRESHOLD,  1344));

		/* VFD_0_DCD_RANGE3 */
		*pList->pulCurrent++ = (
			BCHP_FIELD_DATA(VFD_0_DCD_RANGE3, BPG_OFFSET, 0xfe) |
			BCHP_FIELD_DATA(VFD_0_DCD_RANGE3, MAX_QP,     3   ) |
			BCHP_FIELD_DATA(VFD_0_DCD_RANGE3, MIN_QP,     2   ) |
			BCHP_FIELD_DATA(VFD_0_DCD_RANGE3, THRESHOLD,  1792));

		/* VFD_0_DCD_RANGE4 */
		*pList->pulCurrent++ = (
			BCHP_FIELD_DATA(VFD_0_DCD_RANGE4, BPG_OFFSET, 0xfc) |
			BCHP_FIELD_DATA(VFD_0_DCD_RANGE4, MAX_QP,     3   ) |
			BCHP_FIELD_DATA(VFD_0_DCD_RANGE4, MIN_QP,     2   ) |
			BCHP_FIELD_DATA(VFD_0_DCD_RANGE4, THRESHOLD,  2688));

		/* VFD_0_DCD_RANGE5 */
		*pList->pulCurrent++ = (
			BCHP_FIELD_DATA(VFD_0_DCD_RANGE5, BPG_OFFSET, 0xfc) |
			BCHP_FIELD_DATA(VFD_0_DCD_RANGE5, MAX_QP,     7   ) |
			BCHP_FIELD_DATA(VFD_0_DCD_RANGE5, MIN_QP,     3   ) |
			BCHP_FIELD_DATA(VFD_0_DCD_RANGE5, THRESHOLD,  2912));

		/* VFD_0_DCD_RANGE6 */
		*pList->pulCurrent++ = (
			BCHP_FIELD_DATA(VFD_0_DCD_RANGE6, BPG_OFFSET, 0xfa) |
			BCHP_FIELD_DATA(VFD_0_DCD_RANGE6, MAX_QP,     9   ) |
			BCHP_FIELD_DATA(VFD_0_DCD_RANGE6, MIN_QP,     3   ) |
			BCHP_FIELD_DATA(VFD_0_DCD_RANGE6, THRESHOLD,  3136));

		/* VFD_0_DCD_RANGE7 */
		*pList->pulCurrent++ = (
			BCHP_FIELD_DATA(VFD_0_DCD_RANGE7, BPG_OFFSET, 0xfa) |
			BCHP_FIELD_DATA(VFD_0_DCD_RANGE7, MAX_QP,     10   ) |
			BCHP_FIELD_DATA(VFD_0_DCD_RANGE7, MIN_QP,     6   ) |
			BCHP_FIELD_DATA(VFD_0_DCD_RANGE7, THRESHOLD,  3416));

		/* VFD_0_DCD_RANGE8 */
		*pList->pulCurrent++ = (
			BCHP_FIELD_DATA(VFD_0_DCD_RANGE8, BPG_OFFSET, 0xf8) |
			BCHP_FIELD_DATA(VFD_0_DCD_RANGE8, MAX_QP,     11   ) |
			BCHP_FIELD_DATA(VFD_0_DCD_RANGE8, MIN_QP,     6   ) |
			BCHP_FIELD_DATA(VFD_0_DCD_RANGE8, THRESHOLD,  3584));

		/* VFD_0_DCD_RANGE9 */
		*pList->pulCurrent++ = (
			BCHP_FIELD_DATA(VFD_0_DCD_RANGE9, BPG_OFFSET, 0xf6) |
			BCHP_FIELD_DATA(VFD_0_DCD_RANGE9, MAX_QP,     11   ) |
			BCHP_FIELD_DATA(VFD_0_DCD_RANGE9, MIN_QP,     7   ) |
			BCHP_FIELD_DATA(VFD_0_DCD_RANGE9, THRESHOLD,  3752));

		/* VFD_0_DCD_RANGE10 */
		*pList->pulCurrent++ = (
			BCHP_FIELD_DATA(VFD_0_DCD_RANGE10, BPG_OFFSET, 0xf6) |
			BCHP_FIELD_DATA(VFD_0_DCD_RANGE10, MAX_QP,     12   ) |
			BCHP_FIELD_DATA(VFD_0_DCD_RANGE10, MIN_QP,     9   ) |
			BCHP_FIELD_DATA(VFD_0_DCD_RANGE10, THRESHOLD,  3920));

		/* VFD_0_DCD_RANGE11 */
		*pList->pulCurrent++ = (
			BCHP_FIELD_DATA(VFD_0_DCD_RANGE11, BPG_OFFSET, 0xf4) |
			BCHP_FIELD_DATA(VFD_0_DCD_RANGE11, MAX_QP,     14   ) |
			BCHP_FIELD_DATA(VFD_0_DCD_RANGE11, MIN_QP,     12   ) |
			BCHP_FIELD_DATA(VFD_0_DCD_RANGE11, THRESHOLD,  3976));

		/* VFD_0_DCD_RANGE12 */
		*pList->pulCurrent++ = (
			BCHP_FIELD_DATA(VFD_0_DCD_RANGE12, BPG_OFFSET, 0xf4) |
			BCHP_FIELD_DATA(VFD_0_DCD_RANGE12, MAX_QP,     15   ) |
			BCHP_FIELD_DATA(VFD_0_DCD_RANGE12, MIN_QP,     14   ) |
			BCHP_FIELD_DATA(VFD_0_DCD_RANGE12, THRESHOLD,  4032));

		/* VFD_0_DCD_RANGE13 */
		*pList->pulCurrent++ = (
			BCHP_FIELD_DATA(VFD_0_DCD_RANGE13, BPG_OFFSET, 0xf4) |
			BCHP_FIELD_DATA(VFD_0_DCD_RANGE13, MAX_QP,     16   ) |
			BCHP_FIELD_DATA(VFD_0_DCD_RANGE13, MIN_QP,     15   ) |
			BCHP_FIELD_DATA(VFD_0_DCD_RANGE13, THRESHOLD,  4060));

		/* VFD_0_DCD_RANGE14 */
		*pList->pulCurrent++ = (
			BCHP_FIELD_DATA(VFD_0_DCD_RANGE14, BPG_OFFSET, 0xf4) |
			BCHP_FIELD_DATA(VFD_0_DCD_RANGE14, MAX_QP,     17   ) |
			BCHP_FIELD_DATA(VFD_0_DCD_RANGE14, MIN_QP,     17   ));
#endif

	}
#endif

/*------------------------------------------------------------------------
 * {private}
 *
 */
static void BVDC_Feeder_BuildRulGfxSurAddr_isr
	( BVDC_P_Feeder_Handle             hFeeder,
	  BVDC_P_ListInfo                 *pList )
{
	BVDC_P_GfxSurfaceContext  *pGfxSurface;
	BVDC_P_SurfaceInfo   *pCurSur;
	uint32_t  ulSurAddr, ulBottomOffset;
	uint32_t  ulAddrShadowReg0, ulAddrShadowReg1;
	uint32_t  ulRAddrShadowReg0, ulRAddrShadowReg1;

	pGfxSurface = &hFeeder->stGfxSurface;
	pCurSur = &(pGfxSurface->stCurSurInfo);

	/* ---------------------------------------------------------------------
	 * (1) send new surface to address shadow registers.
	 */
	ulSurAddr = pCurSur->ulAddress + pGfxSurface->ulMainByteOffset;
	if (ulSurAddr != pGfxSurface->stSurNode[pGfxSurface->ucNodeIdx].ulAddr)
	{
		BVDC_P_GfxSurface_SetShadowRegs_isr(&hFeeder->stGfxSurface, pCurSur);
	}

	/* ---------------------------------------------------------------------
	 * (2) setup RUL to pick one set of surface address shadow registers.
	 *
	 * After this RUL is built, gfx surface might change with SetSurface, we
	 * want the change showing up as soon as possible. SetSurface will put the
	 * surface address to a shadow register, and here we pre-build the RUL to
	 * copy surface address value from shadow register to GFD.
	 *
	 * In order to activate left and right surface atomically, we use an index
	 * register, and ping-pong buffered left addr and right addr register
	 * pairs.  The index indicates the left/right surface addr in which pair
	 * should be used. The following is the algorithm:
	 *
	 * v_0  <-  index
	 * v_1  <-  ~index
	 *
	 * v_2 = v_0 & left_start_1
	 * v_3 = v_1 & left_start_0
	 * v_2 = v_2 | v_3
	 * v_3 <- ulBottomOffset
	 * BCHP_GFD_0_SRC_START_ADDR = v_2 + v3
	 *
	 * v_2 = v_0 & right_start_0
	 * v_3 = v_1 & right_start_1
	 * v_2 = v_2 | v_3
	 * v_3 <-  ulBottomOffset
	 * BCHP_GFD_0_SRC_START_R_ADDR = v_2 + v3
	 */
	ulBottomOffset = (BAVC_Polarity_eBotField == hFeeder->stPicture.eSrcPolarity)?
		pCurSur->ulPitch : 0;
	if (pGfxSurface->b3dSrc)
	{
		if (BVDC_P_ORIENTATION_IS_3D(pCurSur->stAvcPic.eInOrientation) &&
			BFMT_Orientation_e3D_Right == hFeeder->eOutputOrientation)
		{
			/* sawp right Surface and left surface due to output orientation */
			ulAddrShadowReg0  = BRDC_REGISTER(pGfxSurface->ulRSurAddrReg[0]);
			ulAddrShadowReg1  = BRDC_REGISTER(pGfxSurface->ulRSurAddrReg[1]);
			ulRAddrShadowReg0 = BRDC_REGISTER(pGfxSurface->ulSurAddrReg[0]);
			ulRAddrShadowReg1 = BRDC_REGISTER(pGfxSurface->ulSurAddrReg[1]);
		}
		else
		{
			ulAddrShadowReg0  = BRDC_REGISTER(pGfxSurface->ulSurAddrReg[0]);
			ulAddrShadowReg1  = BRDC_REGISTER(pGfxSurface->ulSurAddrReg[1]);
			ulRAddrShadowReg0 = BRDC_REGISTER(pGfxSurface->ulRSurAddrReg[0]);
			ulRAddrShadowReg1 = BRDC_REGISTER(pGfxSurface->ulRSurAddrReg[1]);
		}

		*pList->pulCurrent++ = BRDC_OP_REG_TO_VAR(BRDC_Variable_0);
		*pList->pulCurrent++ = BRDC_REGISTER(pGfxSurface->ulRegIdxReg);
		*pList->pulCurrent++ = BRDC_OP_VAR_XOR_IMM_TO_VAR(BRDC_Variable_0, BRDC_Variable_1);
		*pList->pulCurrent++ = ~0;

		*pList->pulCurrent++ = BRDC_OP_REG_TO_VAR(BRDC_Variable_2);
		*pList->pulCurrent++ = ulAddrShadowReg1;
		*pList->pulCurrent++ = BRDC_OP_REG_TO_VAR(BRDC_Variable_3);
		*pList->pulCurrent++ = ulAddrShadowReg0;

		*pList->pulCurrent++ = BRDC_OP_VAR_AND_VAR_TO_VAR(BRDC_Variable_0, BRDC_Variable_2, BRDC_Variable_2);
		*pList->pulCurrent++ = BRDC_OP_VAR_AND_VAR_TO_VAR(BRDC_Variable_1, BRDC_Variable_3, BRDC_Variable_3);
		*pList->pulCurrent++ = BRDC_OP_VAR_OR_VAR_TO_VAR (BRDC_Variable_2, BRDC_Variable_3, BRDC_Variable_2);

		*pList->pulCurrent++ = BRDC_OP_IMM_TO_VAR(BRDC_Variable_3);
		*pList->pulCurrent++ = ulBottomOffset;
		*pList->pulCurrent++ = BRDC_OP_VAR_SUM_VAR_TO_VAR(BRDC_Variable_2, BRDC_Variable_3, BRDC_Variable_2);

		*pList->pulCurrent++ = BRDC_OP_VAR_TO_REG(BRDC_Variable_2);
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_MFD_0_PICTURE0_LINE_ADDR_0) + hFeeder->ulRegOffset;

#if BVDC_P_MFD_SUPPORT_3D_VIDEO
		if(BVDC_P_ORIENTATION_IS_3D(pCurSur->stAvcPic.eInOrientation) &&
		   BVDC_P_ORIENTATION_IS_3D(hFeeder->eOutputOrientation))
		{
			*pList->pulCurrent++ = BRDC_OP_REG_TO_VAR(BRDC_Variable_2);
			*pList->pulCurrent++ = ulRAddrShadowReg1;
			*pList->pulCurrent++ = BRDC_OP_REG_TO_VAR(BRDC_Variable_3);
			*pList->pulCurrent++ = ulRAddrShadowReg0;

			*pList->pulCurrent++ = BRDC_OP_VAR_AND_VAR_TO_VAR(BRDC_Variable_0, BRDC_Variable_2, BRDC_Variable_2);
			*pList->pulCurrent++ = BRDC_OP_VAR_AND_VAR_TO_VAR(BRDC_Variable_1, BRDC_Variable_3, BRDC_Variable_3);
			*pList->pulCurrent++ = BRDC_OP_VAR_OR_VAR_TO_VAR (BRDC_Variable_2, BRDC_Variable_3, BRDC_Variable_2);

			*pList->pulCurrent++ = BRDC_OP_IMM_TO_VAR(BRDC_Variable_3);
			*pList->pulCurrent++ = ulBottomOffset;
			*pList->pulCurrent++ = BRDC_OP_VAR_SUM_VAR_TO_VAR(BRDC_Variable_2, BRDC_Variable_3, BRDC_Variable_2);

			*pList->pulCurrent++ = BRDC_OP_VAR_TO_REG(BRDC_Variable_2);
			*pList->pulCurrent++ = BRDC_REGISTER(BCHP_MFD_0_PICTURE0_LINE_ADDR_0_R) + hFeeder->ulRegOffset;
		}
		else
		{
			*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
			*pList->pulCurrent++ = BRDC_REGISTER(BCHP_MFD_0_PICTURE0_LINE_ADDR_0_R) + hFeeder->ulRegOffset;
			*pList->pulCurrent++ = 0;
		}
#endif
	}
	else /* !(pGfxSurface->b3dSrc) */
	{
		*pList->pulCurrent++ = BRDC_OP_REG_TO_VAR(BRDC_Variable_0);
		*pList->pulCurrent++ = BRDC_REGISTER(pGfxSurface->ulSurAddrReg[0]);

		*pList->pulCurrent++ = BRDC_OP_IMM_TO_VAR(BRDC_Variable_1);
		*pList->pulCurrent++ = ulBottomOffset;
		*pList->pulCurrent++ = BRDC_OP_VAR_SUM_VAR_TO_VAR(BRDC_Variable_0, BRDC_Variable_1, BRDC_Variable_0);

		*pList->pulCurrent++ = BRDC_OP_VAR_TO_REG(BRDC_Variable_0);
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_MFD_0_PICTURE0_LINE_ADDR_0) + hFeeder->ulRegOffset;
	}

	/* set RUL to increase VsyncCntr */
	*pList->pulCurrent++ = BRDC_OP_REG_TO_VAR(BRDC_Variable_2);
	*pList->pulCurrent++ = BRDC_REGISTER(pGfxSurface->ulVsyncCntrReg);
	*pList->pulCurrent++ = BRDC_OP_VAR_SUM_IMM_TO_VAR(BRDC_Variable_2, BRDC_Variable_2);
	*pList->pulCurrent++ = 1;
	*pList->pulCurrent++ = BRDC_OP_VAR_TO_REG(BRDC_Variable_2);
	*pList->pulCurrent++ = BRDC_REGISTER(pGfxSurface->ulVsyncCntrReg);
}

/***************************************************************************
 * {private}
 *
 */
static void BVDC_P_Feeder_BuildRulVfd_isr
	( BVDC_P_Feeder_Handle             hFeeder,
	  BVDC_P_ListInfo                 *pList )
{
	BDBG_ASSERT(!BVDC_P_Feeder_IS_MPEG(hFeeder));

#if (BVDC_P_SUPPORT_VIDEO_TESTFEATURE1_CAP)
	if(hFeeder->eId == BVDC_P_FeederId_eVfd0)
	{
		BVDC_P_Feeder_BuildRulVfdDcx_isr(hFeeder, pList);
	}
#endif

	/* Address Offsets are different between MFD and VFD.
	 * Be careful when using BVDC_P_MFD_BLOCK_WRITE_TO_RUL or
	 * BVDC_P_MFD_WRITE_TO_RUL */
	/* VFD_0_TEST_MODE_CNTL */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(
		BCHP_VFD_0_TEST_MODE_CNTL + hFeeder->ulVfd0RegOffset);
	*pList->pulCurrent++ = BVDC_P_MFD_GET_REG_DATA(MFD_0_TEST_MODE_CNTL);

	if (!hFeeder->bGfxSrc)
	{
#if (BVDC_P_SUPPORT_MFD_VER >= BVDC_P_MFD_VER_10)
		BVDC_P_MFD_WRITE_TO_RUL(
			MFD_0_PICTURE0_LINE_ADDR_0_R, pList->pulCurrent);
#endif

		BVDC_P_MFD_BLOCK_WRITE_TO_RUL(
			MFD_0_FEEDER_CNTL, MFD_0_PIC_FEED_CMD, pList->pulCurrent);

#if (!BVDC_P_USE_RDC_TIMESTAMP)
		/* Read and store timestamp */
		*pList->pulCurrent++ = BRDC_OP_REG_TO_VAR(BRDC_Variable_0);
		*pList->pulCurrent++ = BRDC_REGISTER(hFeeder->stTimerReg.status);

		*pList->pulCurrent++ = BRDC_OP_IMM_TO_VAR(BRDC_Variable_1);
		*pList->pulCurrent++ = BCHP_TIMER_TIMER0_STAT_COUNTER_VAL_MASK;

		*pList->pulCurrent++ = BRDC_OP_VAR_AND_VAR_TO_VAR(
			BRDC_Variable_0, BRDC_Variable_1, BRDC_Variable_2);

		*pList->pulCurrent++ = BRDC_OP_VAR_TO_REG(BRDC_Variable_2);
		*pList->pulCurrent++ = BRDC_REGISTER(hFeeder->ulTimestampRegAddr);
#endif

	}

	else /* feed gfx surface */
	{
#if (BVDC_P_SUPPORT_MFD_VER >= BVDC_P_MFD_VER_10)
		/* Split the above BLOCK WRITE to set PICTURE0_LINE_ADDR_0
		 * from shadow registers */
		BVDC_P_MFD_BLOCK_WRITE_TO_RUL(
			MFD_0_FEEDER_CNTL, MFD_0_DISP_VSIZE, pList->pulCurrent);

		BVDC_Feeder_BuildRulGfxSurAddr_isr(hFeeder, pList);

		BVDC_P_MFD_BLOCK_WRITE_TO_RUL(
			MFD_0_DATA_MODE, MFD_0_PIC_FEED_CMD, pList->pulCurrent);

#else
		/* Split the above BLOCK WRITE to set PICTURE0_LINE_ADDR_0
		 * from shadow registers */
		BVDC_P_MFD_BLOCK_WRITE_TO_RUL(
			MFD_0_FEEDER_CNTL, MFD_0_PICTURE0_DISP_VERT_WINDOW, pList->pulCurrent);

		BVDC_Feeder_BuildRulGfxSurAddr_isr(hFeeder, pList);

#if (BVDC_P_SUPPORT_MFD_VER <= BVDC_P_MFD_VER_5)
		BVDC_P_MFD_WRITE_TO_RUL(
			MFD_0_PIC_FEED_CMD, pList->pulCurrent);
#elif (BVDC_P_SUPPORT_MFD_VER == BVDC_P_MFD_VER_7)
		BVDC_P_MFD_BLOCK_WRITE_TO_RUL(
			MFD_0_US_422_TO_444, MFD_0_PIC_FEED_CMD, pList->pulCurrent);
#elif (BVDC_P_SUPPORT_MFD_VER <= BVDC_P_MFD_VER_9)
		BVDC_P_MFD_BLOCK_WRITE_TO_RUL(
			MFD_0_BYTE_ORDER, MFD_0_PIC_FEED_CMD, pList->pulCurrent);
#endif
#endif
	}

	return;
}

/***************************************************************************
 * {private}
 *
 */
void BVDC_P_Feeder_BuildRul_isr
	( const BVDC_P_Feeder_Handle       hFeeder,
	  BVDC_P_ListInfo                 *pList,
	  BVDC_P_State                     eVnetState,
	  BVDC_P_PicComRulInfo            *pPicComRulInfo )
{
	uint32_t  ulRulOpsFlags;

	BDBG_ENTER(BVDC_P_Feeder_BuildRul_isr);
	BDBG_OBJECT_ASSERT(hFeeder, BVDC_FDR);

	/* currently this is only for vnet building / tearing-off*/

	ulRulOpsFlags = BVDC_P_SubRul_GetOps_isr(
		&(hFeeder->SubRul), pPicComRulInfo->eWin, eVnetState, pList->bLastExecuted);

	if ((0 == ulRulOpsFlags) ||
		(ulRulOpsFlags & BVDC_P_RulOp_eReleaseHandle))
		return;
	else if (ulRulOpsFlags & BVDC_P_RulOp_eDisable)
	{
		BVDC_P_Feeder_SetEnable_isr(hFeeder, false);
		BVDC_P_MFD_WRITE_TO_RUL(MFD_0_PIC_FEED_CMD, pList->pulCurrent);
	}

	/* Include a reset at the beginning of feeder bringup. */
	if(hFeeder->bInitial)
	{
		BVDC_P_BUILD_RESET(pList->pulCurrent,
			hFeeder->ulResetRegAddr, hFeeder->ulResetMask);

		if(BVDC_P_Feeder_IS_MPEG(hFeeder))
		{
			BVDC_P_MFD_WRITE_TO_RUL(MFD_0_CRC_CTRL, pList->pulCurrent);
			BVDC_P_MFD_WRITE_TO_RUL(MFD_0_CRC_SEED, pList->pulCurrent);
		}

		hFeeder->bInitial = false;
	}

	/* Add feeder registers to RUL */
	if((BVDC_P_MFD_COMPARE_FIELD_DATA(MFD_0_PIC_FEED_CMD, START_FEED, 1)) &&
	   (ulRulOpsFlags & BVDC_P_RulOp_eEnable))
	{
		if (BVDC_P_Feeder_IS_MPEG(hFeeder))
		{
			BVDC_P_Feeder_BuildRulMfd_isr(hFeeder, pList);
		}
		else
		{
			BVDC_P_Feeder_BuildRulVfd_isr(hFeeder, pList);
		}
	}

	if (ulRulOpsFlags & BVDC_P_RulOp_eDrainVnet)
	{
		BVDC_P_Feeder_BuildRul_DrainVnet_isr(hFeeder, pList);
	}

	BDBG_LEAVE(BVDC_P_Feeder_BuildRul_isr);
	return;
}

/***************************************************************************
 * This function checks if we can save both bandwidth and memory by
 * converting at MFD when src is 3d and display is 2d. For all the other
 * cases, we still convert at VFD.
 *
 */
static BERR_Code BVDC_P_Feeder_GetMpegOutOrientation_isr
	( BVDC_P_Feeder_Handle             hFeeder,
	  const BAVC_MVD_Field            *pFieldData,
	  BFMT_Orientation                *peOutOrientation )
{
	uint32_t   i;
	bool       bMatch = false, bFirstWindow = true;
	BFMT_Orientation  eCmpOrientation = BFMT_Orientation_e2D;
	BFMT_Orientation  eMfdOrientation = BFMT_Orientation_e2D;
	BVDC_Source_Handle hSource = hFeeder->hSource;
	BVDC_Window_Handle hWindow;

	for(i = 0; i < BVDC_P_MAX_WINDOW_COUNT; i++)
	{
		/* SKIP: If it's just created or inactive no need to build ruls. */
		if(!hSource->ahWindow[i] ||
			BVDC_P_STATE_IS_CREATE(hSource->ahWindow[i]) ||
			BVDC_P_STATE_IS_INACTIVE(hSource->ahWindow[i]))
		{
			continue;
		}

		hWindow = hSource->ahWindow[i];
		BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

		/* Get display orientation */
		if(BFMT_IS_3D_MODE(hWindow->hCompositor->stCurInfo.pFmtInfo->eVideoFmt))
			eCmpOrientation = hWindow->hCompositor->stCurInfo.pFmtInfo->eOrientation;
		else
			eCmpOrientation = hWindow->hCompositor->stCurInfo.eOrientation;

		if(bFirstWindow)
		{
			bMatch = true;
			bFirstWindow = false;
			eMfdOrientation = eCmpOrientation;
		}
		else
		{
			if(eMfdOrientation != eCmpOrientation)
			{
				bMatch = false;
			}
		}

	}

	if(bMatch && (eCmpOrientation == BFMT_Orientation_e2D))
	{

		BDBG_MSG(("Optimize case: display is 2d, MFD scan out 2d"));
		*peOutOrientation = eCmpOrientation;
	}
	else if(hSource->stCurInfo.bOrientationOverride)
	{
		*peOutOrientation = hSource->stCurInfo.eOrientation;
	}
	else
	{
		if((pFieldData->eOrientation == BFMT_Orientation_e3D_Left) ||
		   (pFieldData->eOrientation == BFMT_Orientation_e3D_Right) ||
		   (pFieldData->eOrientation == BFMT_Orientation_eLeftRight_Enhanced))
		{
			/* 2 buffer case: 2 separate buffers provided from XVD.
			 * MFD always feeds out as over/under. */
			*peOutOrientation = BFMT_Orientation_e3D_OverUnder;
		}
		else
		{
			/* All other cases: Keep orientation unchanged in MFD */
			*peOutOrientation = pFieldData->eOrientation;
		}
	}

	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
static BERR_Code BVDC_P_Feeder_GetFormatCtrlSettings_isr
	( BVDC_P_Feeder_Handle             hFeeder,
	  const BAVC_MVD_Field            *pFieldData,
	  const BVDC_P_Rect               *pScanoutRect,
	  const BVDC_P_PictureNodePtr      pPicture )
{
	BPXL_Format                 ePxlFormat = BPXL_INVALID;
	BVDC_P_Feeder_ImageFormat   eImageFormat;
	BFMT_Orientation            eInOrientation, eOutOrientation;
	BERR_Code                   err = BERR_SUCCESS;

	if(BVDC_P_Feeder_IS_MPEG(hFeeder))
	{
		/* MFD data comes from pFieldData */
		BDBG_ASSERT(pFieldData);

		/* Get orientatation */
		if((pFieldData->eOrientation == BFMT_Orientation_e3D_LeftRight)||
		   (pFieldData->eOrientation == BFMT_Orientation_e3D_OverUnder) ||
		   (pFieldData->eOrientation == BFMT_Orientation_e3D_Left) ||
		   (pFieldData->eOrientation == BFMT_Orientation_e3D_Right))
		{
			/* always use dual pointer if 3D non-MRE mode */
			eInOrientation = BFMT_Orientation_e3D_Left;
		}
		else
		{
			eInOrientation = pFieldData->eOrientation;
		}

		BVDC_P_Feeder_GetMpegOutOrientation_isr(hFeeder, pFieldData, &eOutOrientation);

		/* Get ulThroughput */
		hFeeder->ulThroughput = pScanoutRect->ulWidth
			* (pScanoutRect->ulHeight  >> (pFieldData->eSourcePolarity != BAVC_Polarity_eFrame))
			* (hFeeder->hSource->ulVertFreq / BFMT_FREQ_FACTOR);
		if(hFeeder->eInputOrientation != BFMT_Orientation_e2D)
			hFeeder->ulThroughput = 2 * hFeeder->ulThroughput;

		ePxlFormat = pFieldData->ePxlFmt;

		if(ePxlFormat == BPXL_INVALID)
		{
			eImageFormat = BVDC_P_Feeder_ImageFormat_eAVC_MPEG;
		}
		else if(BPXL_eX2_Cr10_Y10_Cb10 == ePxlFormat)
		{
			eImageFormat = BVDC_P_Feeder_ImageFormat_eYUV444;
		}
		else
		{
			eImageFormat = BVDC_P_Feeder_ImageFormat_ePacked;
		}
	}
	else
	{
		bool bTestFeatureEnable;

		/* VFD data comes from pPicture */
		BDBG_ASSERT(pPicture);

		/* always use dual pointer if 3D mode */
		eInOrientation = (pPicture->eSrcOrientation == BFMT_Orientation_e2D)
			? BFMT_Orientation_e2D : BFMT_Orientation_e3D_Left;
		eOutOrientation = pPicture->eDispOrientation;

		ePxlFormat = pPicture->ePixelFormat;
		bTestFeatureEnable = (hFeeder->bGfxSrc)
			? false : pPicture->stCapTestFeature1.bEnable;

		/* default for VFD */
#if (!BVDC_P_VFD_SUPPORT_IMAGE_FORMAT)
		/* No VFD_0_FEEDER_CNTL_IMAGE_FORMAT field. Reserved field
		 * must be written with 0 */
		eImageFormat = 0;
#else
		eImageFormat = BVDC_P_Feeder_ImageFormat_ePacked;
#endif

#if (BVDC_P_MFD_SUPPORT_10BIT_444)
		/* Image format */
		if(BVDC_P_CAP_PIXEL_FORMAT_10BIT444 == (BPXL_Format)ePxlFormat)
		{
			eImageFormat = BVDC_P_Feeder_ImageFormat_eYUV444;
		}
		else
		{
			eImageFormat = BVDC_P_Feeder_ImageFormat_ePacked;
		}
#endif

#if (BVDC_P_MFD_SUPPORT_IMAGE_FORMAT_PACKED_NEW)
		/* Use new format to match the byte order with capture */
		if((eImageFormat == BVDC_P_Feeder_ImageFormat_ePacked) &&
		   (BPXL_IS_YCbCr422_FORMAT(ePxlFormat)) &&
		   (!bTestFeatureEnable))
		{
			eImageFormat = BVDC_P_Feeder_ImageFormat_ePacked_new;
		}
#endif
	}

	hFeeder->ePxlFormat         = ePxlFormat;
	hFeeder->eImageFormat       = eImageFormat;
	hFeeder->eInputOrientation  = eInOrientation;
	hFeeder->eOutputOrientation = eOutOrientation;

	return BERR_TRACE(err);
}

#if (BVDC_P_MFD_SUPPORT_3D_VIDEO)
/***************************************************************************
 * Set MFD_0_FEEDER_CNTL.MEM_VIDEO and MFD_0_FEEDER_CNTL.BVB_VIDEO
 */
static BERR_Code BVDC_P_Feeder_SetOrientation_isr
	( BVDC_P_Feeder_Handle             hFeeder )
{
	/* MFD_0_FEEDER_CNTL */
	BVDC_P_MFD_GET_REG_DATA(MFD_0_FEEDER_CNTL) &= ~(
		BCHP_MASK(MFD_0_FEEDER_CNTL, MEM_VIDEO) |
		BCHP_MASK(MFD_0_FEEDER_CNTL, BVB_VIDEO));

	BVDC_P_MFD_GET_REG_DATA(MFD_0_FEEDER_CNTL) |= (
		BCHP_FIELD_DATA(MFD_0_FEEDER_CNTL, MEM_VIDEO,
			s_hwOrientation[hFeeder->eInputOrientation]) |
		BCHP_FIELD_DATA(MFD_0_FEEDER_CNTL, BVB_VIDEO, hFeeder->eOutputOrientation));

	return BERR_SUCCESS;
}
#endif

#if (BVDC_P_MFD_SUPPORT_PACKING_TYPE)
/***************************************************************************
 * Set MFD_0_FEEDER_CNTL.PACKING_TYPE
 *
 * MFD_0_FEEDER_CNTL.PACKING_TYPE is only used when MFD_0_FEEDER_CNTL.IMAGE_FORMAT
 * is PACKED on older chipset. On Chipset PACKED_NEW is support, MFD_0_BYTE_ORDER
 * is used instead.
 */
static BERR_Code BVDC_P_Feeder_SetPackingType_isr
	( BVDC_P_Feeder_Handle             hFeeder,
	  const BAVC_MVD_Field            *pFieldData,
	  const BVDC_P_PictureNodePtr      pPicture )
{
	uint32_t     ulPackingType = 0;
	BERR_Code    err = BERR_SUCCESS;
	BPXL_Format  ePxlFormat = BPXL_eCr8_Y18_Cb8_Y08;

	if(BVDC_P_Feeder_IS_MPEG(hFeeder))
	{
		/* MFD data comes from pFieldData */
		BDBG_ASSERT(pFieldData);
		ePxlFormat = pFieldData->ePxlFmt;
	}
	else
	{
		/* VFD data comes from pPicture */
		BDBG_ASSERT(pPicture);
		ePxlFormat = pPicture->ePixelFormat;
	}

	/* Note:
	 * The following 4 pixel formats:
	 *      BPXL_eY08_Cr8_Y18_Cb8
	 *      BPXL_eCr8_Y08_Cb8_Y18
	 *      BPXL_eCb8_Y08_Cr8_Y18
	 *      BPXL_eY08_Cb8_Y18_Cr8
	 * are not supported by VFD in older chipset, warning sent out in
	 * BVDC_Window_SetPixelFormat, and use opposite endian packing type here.
	 * Video display will have wrong color. However CAP_0_BYTE_ORDER is
	 * set correctly, so the data in capture is correct.
	 * For the newer version of VFD (when PACKED_NEW format is supported), use
	 * VFD_0_BYTE_ORDER to set pixel format
	 */
	ePxlFormat =
		(ePxlFormat == BPXL_eY08_Cr8_Y18_Cb8) ? BPXL_eCb8_Y18_Cr8_Y08 :
		(ePxlFormat == BPXL_eCr8_Y08_Cb8_Y18) ? BPXL_eY18_Cb8_Y08_Cr8 :
		(ePxlFormat == BPXL_eY08_Cb8_Y18_Cr8) ? BPXL_eCr8_Y18_Cb8_Y08 :
		(ePxlFormat == BPXL_eCb8_Y08_Cr8_Y18) ? BPXL_eY18_Cr8_Y08_Cb8 :
		ePxlFormat;

	/* feeder is big endian */
	switch(ePxlFormat)
	{
		case BPXL_eCb8_Y18_Cr8_Y08:
			ulPackingType = BCHP_MFD_0_FEEDER_CNTL_PACKING_TYPE_Y0_V0_Y1_U0;
			break;

		case BPXL_eY18_Cb8_Y08_Cr8:
			ulPackingType = BCHP_MFD_0_FEEDER_CNTL_PACKING_TYPE_V0_Y0_U0_Y1;
			break;

		case BPXL_eY18_Cr8_Y08_Cb8:
			ulPackingType = BCHP_MFD_0_FEEDER_CNTL_PACKING_TYPE_U0_Y0_V0_Y1;
			break;

		case BPXL_eCr8_Y18_Cb8_Y08:
			ulPackingType = BCHP_MFD_0_FEEDER_CNTL_PACKING_TYPE_Y0_U0_Y1_V0;
			break;

		default:
			ulPackingType = BCHP_MFD_0_FEEDER_CNTL_PACKING_TYPE_Y0_U0_Y1_V0;
			break;
	}

	BVDC_P_MFD_GET_REG_DATA(MFD_0_FEEDER_CNTL) &= ~(
		BCHP_MASK(MFD_0_FEEDER_CNTL, PACKING_TYPE));

	BVDC_P_MFD_GET_REG_DATA(MFD_0_FEEDER_CNTL) |= (
		BCHP_FIELD_DATA(MFD_0_FEEDER_CNTL, PACKING_TYPE, ulPackingType));

	return BERR_TRACE(err);
}
#endif

/***************************************************************************
 * Set MFD_0_FEEDER_CNTL.IMAGE_FORMAT
 */
static BERR_Code BVDC_P_Feeder_SetImageFormat_isr
	( BVDC_P_Feeder_Handle             hFeeder )
{

	BVDC_P_MFD_GET_REG_DATA(MFD_0_FEEDER_CNTL) &= ~(
		BCHP_MASK(MFD_0_FEEDER_CNTL, IMAGE_FORMAT) );

	BVDC_P_MFD_GET_REG_DATA(MFD_0_FEEDER_CNTL) |= (
		BCHP_FIELD_DATA(MFD_0_FEEDER_CNTL, IMAGE_FORMAT, hFeeder->eImageFormat) );

	return BERR_SUCCESS;
}

#if (BVDC_P_MFD_SUPPORT_PIXEL_SATURATION_ENABLE)
/***************************************************************************
 * Set MFD_0_FEEDER_CNTL.PIXEL_SATURATION_ENABLE
 */
static BERR_Code BVDC_P_Feeder_SetPixelSaturation_isr
	( BVDC_P_Feeder_Handle             hFeeder )
{

	BVDC_P_MFD_GET_REG_DATA(MFD_0_FEEDER_CNTL) &= ~(
		BCHP_MASK(MFD_0_FEEDER_CNTL, PIXEL_SATURATION_ENABLE));

	BVDC_P_MFD_GET_REG_DATA(MFD_0_FEEDER_CNTL) |= (
		BCHP_FIELD_ENUM(MFD_0_FEEDER_CNTL, PIXEL_SATURATION_ENABLE, OFF));

	return BERR_SUCCESS;
}
#endif

#if (BVDC_P_MFD_SUPPORT_SCB_CLIENT_SEL)
/***************************************************************************
 *  MFD only
 * Set MFD_0_FEEDER_CNTL.SCB_CLIENT_SEL
 */
static BERR_Code BVDC_P_Feeder_SetScbClient_isr
	( BVDC_P_Feeder_Handle             hFeeder )
{
	BDBG_ASSERT(BVDC_P_Feeder_IS_MPEG(hFeeder));

	BVDC_P_MFD_GET_REG_DATA(MFD_0_FEEDER_CNTL) &= ~(
		BCHP_MASK(MFD_0_FEEDER_CNTL, SCB_CLIENT_SEL));

	/* CLIENT_A is designed to handle data rates up to 1080i60
	 * (aka 540x1920@60 = 62208000). Should match RTS settings. */
	if(hFeeder->ulThroughput <= 62208000)
	{
		BVDC_P_MFD_GET_REG_DATA(MFD_0_FEEDER_CNTL) |= (
			BCHP_FIELD_ENUM(MFD_0_FEEDER_CNTL, SCB_CLIENT_SEL, CLIENT_A));
	}
	else
	{
		BVDC_P_MFD_GET_REG_DATA(MFD_0_FEEDER_CNTL) |= (
			BCHP_FIELD_ENUM(MFD_0_FEEDER_CNTL, SCB_CLIENT_SEL, CLIENT_B));
	}

	return BERR_SUCCESS;
}
#endif

#if (BVDC_P_MFD_SUPPORT_SMOOTHING_BUFFER)
/***************************************************************************
 *  MFD only
 * Set MFD_0_FEEDER_CNTL.SMOOTHING_BUFFER
 */
static BERR_Code BVDC_P_Feeder_SetSmoothBuffer_isr
	( BVDC_P_Feeder_Handle             hFeeder )
{
	BDBG_ASSERT(BVDC_P_Feeder_IS_MPEG(hFeeder));

	BVDC_P_MFD_GET_REG_DATA(MFD_0_FEEDER_CNTL) &= ~(
		BCHP_MASK(MFD_0_FEEDER_CNTL, SMOOTHING_BUFFER));

	BVDC_P_MFD_GET_REG_DATA(MFD_0_FEEDER_CNTL) |= (
		BCHP_FIELD_ENUM(MFD_0_FEEDER_CNTL, SMOOTHING_BUFFER, ENABLE));

	return BERR_SUCCESS;
}
#endif

#if (BVDC_P_MFD_SUPPORT_INIT_PHASE)
/***************************************************************************
 *  MFD only
 * Set BOT_INIT_PHASE and TOP_INIT_PHASE
 */
static BERR_Code BVDC_P_Feeder_SetInitPhase_isr
	( BVDC_P_Feeder_Handle             hFeeder,
	  const BAVC_MVD_Field            *pFieldData )
{
	int8_t    cTopInitPhase, cBotInitPhase;
	BAVC_InterpolationMode  eChromaInterpolationMode;

	BDBG_ASSERT(BVDC_P_Feeder_IS_MPEG(hFeeder));

	eChromaInterpolationMode = pFieldData->eChrominanceInterpolationMode;

	/*
	 * TM5:
	 * If BAVC_InterpolationMode_eFrame:
	 *      MFD_FEEDER_CNTL.TOP_INIT_PHASE = 0;
	 *      MFD_FEEDER_CNTL.BOT_INIT_PHASE = 32;
	 * If BAVC_InterpolationMode_eField:
	 *      MFD_FEEDER_CNTL.TOP_INIT_PHASE = 0;
	 *      MFD_FEEDER_CNTL.BOT_INIT_PHASE = -16;

	 * AVC or MPEG1:
	 * If BAVC_InterpolationMode_eFrame:
	 *      MFD_FEEDER_CNTL.TOP_INIT_PHASE = -16;
	 *      MFD_FEEDER_CNTL.BOT_INIT_PHASE = 16;
	 * If BAVC_InterpolationMode_eField:
	 *      MFD_FEEDER_CNTL.TOP_INIT_PHASE = -8;
	 *      MFD_FEEDER_CNTL.BOT_INIT_PHASE = -24;
	 */
	if((pFieldData->eMpegType == BAVC_ChromaLocation_eType2) ||
	   (pFieldData->eMpegType == BAVC_ChromaLocation_eType4))
	{
		if(eChromaInterpolationMode == BAVC_InterpolationMode_eFrame)
		{
			cTopInitPhase = 0;
			cBotInitPhase = 32;
		}
		else
		{
			cTopInitPhase = 0;
			cBotInitPhase = -16;
		}
	}
	else
	{
		if(eChromaInterpolationMode == BAVC_InterpolationMode_eFrame)
		{
			cTopInitPhase = -16;
			cBotInitPhase = 16;
		}
		else
		{
			cTopInitPhase = -8;
			cBotInitPhase = -24;
		}
	}

	BVDC_P_MFD_GET_REG_DATA(MFD_0_FEEDER_CNTL) &= ~(
		BCHP_MASK(MFD_0_FEEDER_CNTL, BOT_INIT_PHASE) |
		BCHP_MASK(MFD_0_FEEDER_CNTL, TOP_INIT_PHASE));

	BVDC_P_MFD_GET_REG_DATA(MFD_0_FEEDER_CNTL) |= (
		BCHP_FIELD_DATA(MFD_0_FEEDER_CNTL, BOT_INIT_PHASE, cBotInitPhase) |
		BCHP_FIELD_DATA(MFD_0_FEEDER_CNTL, TOP_INIT_PHASE, cTopInitPhase));
	return BERR_SUCCESS;
}
#endif

#if (BVDC_P_MFD_SUPPORT_INIT_PHASE)
/***************************************************************************
 *  MFD only
 * Set BOT_SKIP_FIRST_LINE and TOP_SKIP_FIRST_LINE
 */
static BERR_Code BVDC_P_Feeder_SetSkipFirstLine_isr
	( BVDC_P_Feeder_Handle             hFeeder )
{
	BDBG_ASSERT(BVDC_P_Feeder_IS_MPEG(hFeeder));

	/* TODO: Settings ? */
	BVDC_P_MFD_GET_REG_DATA(MFD_0_FEEDER_CNTL) &= ~(
		BCHP_MASK(MFD_0_FEEDER_CNTL, BOT_SKIP_FIRST_LINE) |
		BCHP_MASK(MFD_0_FEEDER_CNTL, TOP_SKIP_FIRST_LINE));
	BVDC_P_MFD_GET_REG_DATA(MFD_0_FEEDER_CNTL) |= (
		BCHP_FIELD_DATA(MFD_0_FEEDER_CNTL, BOT_SKIP_FIRST_LINE, hFeeder->bSkipFirstLine) |
		BCHP_FIELD_DATA(MFD_0_FEEDER_CNTL, TOP_SKIP_FIRST_LINE, hFeeder->bSkipFirstLine));

	return BERR_SUCCESS;
}
#endif

/***************************************************************************
 * Set MFD_0_FEEDER_CNTL
 */
static BERR_Code BVDC_P_Feeder_SetFeederCntl_isr
	( BVDC_P_Feeder_Handle             hFeeder,
	  const BAVC_MVD_Field            *pFieldData,
	  const BVDC_P_PictureNodePtr      pPicture )
{
	BERR_Code     err = BERR_SUCCESS;

#if (!BVDC_P_MFD_SUPPORT_PACKING_TYPE)
	BSTD_UNUSED(pPicture);
#endif

#if ((!BVDC_P_MFD_SUPPORT_PACKING_TYPE) && (!BVDC_P_MFD_SUPPORT_INIT_PHASE))
	BSTD_UNUSED(pFieldData);
#endif

	/* ------------------------------------------
	 * Common settings for both MFD and VFD
	 */
	/* Set IMAGE_FORMAT */
	err = BVDC_P_Feeder_SetImageFormat_isr(hFeeder);

#if (BVDC_P_MFD_SUPPORT_3D_VIDEO)
	/* Set MEM_VIDEO and BVB_VIDEO */
	err = BVDC_P_Feeder_SetOrientation_isr(hFeeder);
#endif

#if (BVDC_P_MFD_SUPPORT_PIXEL_SATURATION_ENABLE)
	/* Set PIXEL_SATURATION_ENABLE */
	err = BVDC_P_Feeder_SetPixelSaturation_isr(hFeeder);
#endif

#if (BVDC_P_MFD_SUPPORT_PACKING_TYPE)
	err = BVDC_P_Feeder_SetPackingType_isr(hFeeder, pFieldData, pPicture);
#endif

	/* ------------------------------------------
	 * MFD settings only starts here
	 */
	if(BVDC_P_Feeder_IS_MPEG(hFeeder))
	{
#if (BVDC_P_MFD_SUPPORT_SCB_CLIENT_SEL)
		/* Set SCB_CLIENT_SEL */
		err = BVDC_P_Feeder_SetScbClient_isr(hFeeder);
#endif

#if (BVDC_P_MFD_SUPPORT_SMOOTHING_BUFFER)
		/* Set SMOOTHING_BUFFER */
		err = BVDC_P_Feeder_SetSmoothBuffer_isr(hFeeder);
#endif

#if (BVDC_P_MFD_SUPPORT_INIT_PHASE)
		/* Set BOT_INIT_PHASE and TOP_INIT_PHASE */
		err = BVDC_P_Feeder_SetInitPhase_isr(hFeeder, pFieldData);
#endif

#if (BVDC_P_MFD_SUPPORT_SKIP_FIRST_LINE)
		/* Set BOT_SKIP_FIRST_LINE and TOP_SKIP_FIRST_LINE */
		err = BVDC_P_Feeder_SetSkipFirstLine_isr(hFeeder);
#endif

	/* Set LINE_REPEAT? */

	}

	/* MFD_0_FEEDER_CNTL */
	BVDC_P_MFD_GET_REG_DATA(MFD_0_FEEDER_CNTL) &= ~(
		BCHP_MASK(MFD_0_FEEDER_CNTL, FIXED_COLOUR_ENABLE));

	BVDC_P_MFD_GET_REG_DATA(MFD_0_FEEDER_CNTL) |= (
		BCHP_FIELD_ENUM(MFD_0_FEEDER_CNTL, FIXED_COLOUR_ENABLE, OFF));

	return BERR_SUCCESS;
}

#if (BVDC_P_MFD_SUPPORT_BYTE_ORDER)
/***************************************************************************
 * Set MFD_0_BYTE_ORDER
 */
static BERR_Code BVDC_P_Feeder_SetByteOrder_isr
	( BVDC_P_Feeder_Handle             hFeeder )
{
	uint32_t      ulByteOrder =0;

	switch((BPXL_Format)hFeeder->ePxlFormat)
	{
	default:
	case BPXL_eY18_Cb8_Y08_Cr8:
		ulByteOrder |= (
			BCHP_FIELD_ENUM(MFD_0_BYTE_ORDER, BYTE_3_SEL, CR) |
			BCHP_FIELD_ENUM(MFD_0_BYTE_ORDER, BYTE_2_SEL, Y0) |
			BCHP_FIELD_ENUM(MFD_0_BYTE_ORDER, BYTE_1_SEL, CB) |
			BCHP_FIELD_ENUM(MFD_0_BYTE_ORDER, BYTE_0_SEL, Y1));
		break;

	case BPXL_eY08_Cb8_Y18_Cr8:
		ulByteOrder |= (
			BCHP_FIELD_ENUM(MFD_0_BYTE_ORDER, BYTE_3_SEL, CR) |
			BCHP_FIELD_ENUM(MFD_0_BYTE_ORDER, BYTE_2_SEL, Y1) |
			BCHP_FIELD_ENUM(MFD_0_BYTE_ORDER, BYTE_1_SEL, CB) |
			BCHP_FIELD_ENUM(MFD_0_BYTE_ORDER, BYTE_0_SEL, Y0));
		break;

	case BPXL_eY18_Cr8_Y08_Cb8:
		ulByteOrder |= (
			BCHP_FIELD_ENUM(MFD_0_BYTE_ORDER, BYTE_3_SEL, CB) |
			BCHP_FIELD_ENUM(MFD_0_BYTE_ORDER, BYTE_2_SEL, Y0) |
			BCHP_FIELD_ENUM(MFD_0_BYTE_ORDER, BYTE_1_SEL, CR) |
			BCHP_FIELD_ENUM(MFD_0_BYTE_ORDER, BYTE_0_SEL, Y1));
		break;

	case BPXL_eY08_Cr8_Y18_Cb8:
		ulByteOrder |= (
			BCHP_FIELD_ENUM(MFD_0_BYTE_ORDER, BYTE_3_SEL, CB) |
			BCHP_FIELD_ENUM(MFD_0_BYTE_ORDER, BYTE_2_SEL, Y1) |
			BCHP_FIELD_ENUM(MFD_0_BYTE_ORDER, BYTE_1_SEL, CR) |
			BCHP_FIELD_ENUM(MFD_0_BYTE_ORDER, BYTE_0_SEL, Y0));
			break;

	case BPXL_eCr8_Y18_Cb8_Y08:
		ulByteOrder |= (
			BCHP_FIELD_ENUM(MFD_0_BYTE_ORDER, BYTE_3_SEL, Y0) |
			BCHP_FIELD_ENUM(MFD_0_BYTE_ORDER, BYTE_2_SEL, CB) |
			BCHP_FIELD_ENUM(MFD_0_BYTE_ORDER, BYTE_1_SEL, Y1) |
			BCHP_FIELD_ENUM(MFD_0_BYTE_ORDER, BYTE_0_SEL, CR));
		break;

	case BPXL_eCr8_Y08_Cb8_Y18:
		ulByteOrder |= (
			BCHP_FIELD_ENUM(MFD_0_BYTE_ORDER, BYTE_3_SEL, Y1) |
			BCHP_FIELD_ENUM(MFD_0_BYTE_ORDER, BYTE_2_SEL, CB) |
			BCHP_FIELD_ENUM(MFD_0_BYTE_ORDER, BYTE_1_SEL, Y0) |
			BCHP_FIELD_ENUM(MFD_0_BYTE_ORDER, BYTE_0_SEL, CR));
			break;

	case BPXL_eCb8_Y18_Cr8_Y08:
		ulByteOrder |= (
			BCHP_FIELD_ENUM(MFD_0_BYTE_ORDER, BYTE_3_SEL, Y0) |
			BCHP_FIELD_ENUM(MFD_0_BYTE_ORDER, BYTE_2_SEL, CR) |
			BCHP_FIELD_ENUM(MFD_0_BYTE_ORDER, BYTE_1_SEL, Y1) |
			BCHP_FIELD_ENUM(MFD_0_BYTE_ORDER, BYTE_0_SEL, CB));
		break;

	case BPXL_eCb8_Y08_Cr8_Y18:
		ulByteOrder |= (
			BCHP_FIELD_ENUM(MFD_0_BYTE_ORDER, BYTE_3_SEL, Y1) |
			BCHP_FIELD_ENUM(MFD_0_BYTE_ORDER, BYTE_2_SEL, CR) |
			BCHP_FIELD_ENUM(MFD_0_BYTE_ORDER, BYTE_1_SEL, Y0) |
			BCHP_FIELD_ENUM(MFD_0_BYTE_ORDER, BYTE_0_SEL, CB));
		break;

	}

	BVDC_P_MFD_GET_REG_DATA(MFD_0_BYTE_ORDER) = ulByteOrder;

	return BERR_SUCCESS;
}
#endif

#if (BVDC_P_MFD_SUPPORT_DATA_MODE)
/***************************************************************************
 * Set MFD_0_DATA_MODE
 */
static BERR_Code BVDC_P_Feeder_SetDataMode_isr
	( BVDC_P_Feeder_Handle             hFeeder )
{
	bool          b8BitMode = true; /* MPEG is always 8 bit */

	if(BVDC_P_FEEDER_PIXEL_FORMAT_IS_10BIT((BPXL_Format)hFeeder->ePxlFormat))
	{
		b8BitMode = false;
	}

	BVDC_P_MFD_GET_REG_DATA(MFD_0_DATA_MODE) &= ~(
		BCHP_MASK(MFD_0_DATA_MODE, PIXEL_WIDTH));

	BVDC_P_MFD_GET_REG_DATA(MFD_0_DATA_MODE) |= (
		BCHP_FIELD_DATA(MFD_0_DATA_MODE, PIXEL_WIDTH, !b8BitMode));

	return BERR_SUCCESS;
}
#endif

#if (BVDC_P_MFD_SUPPORT_10BIT_444)
/***************************************************************************
 * Set MFD_0_US_422_TO_444
 */
static BERR_Code BVDC_P_Feeder_SetUpSampler
	( BVDC_P_Feeder_Handle             hFeeder )
{

	if(BVDC_P_FEEDER_IMAGE_FORMAT_IS_PACKED(hFeeder->eImageFormat) ||
	   !BVDC_P_FEEDER_PIXEL_FORMAT_IS_10BIT((BPXL_Format)hFeeder->ePxlFormat))
	{
		/* 8 bit 422 or 10 bit 422:
		 * Up sample data in memory to 444 */
		hFeeder->stUpSampler.bUnbiasedRound = true;
		hFeeder->stUpSampler.eRingRemoval   = BVDC_RingSuppressionMode_eNormal;
	}

	BVDC_P_MFD_GET_REG_DATA(MFD_0_US_422_TO_444) &= ~(
		BCHP_MASK(MFD_0_US_422_TO_444, RING_SUPPION_MODE)     |
		BCHP_MASK(MFD_0_US_422_TO_444, RING_SUPPION)          |
		BCHP_MASK(MFD_0_US_422_TO_444, UNBIASED_ROUND_ENABLE) |
		BCHP_MASK(MFD_0_US_422_TO_444, FILT_CTRL));

	if((hFeeder->stUpSampler.eFilterType == BVDC_422To444Filter_eTenTaps) ||
	   (hFeeder->stUpSampler.eFilterType == BVDC_422To444Filter_eSixTaps))
	{
		BVDC_P_MFD_GET_REG_DATA(MFD_0_US_422_TO_444) |= (
			BCHP_FIELD_ENUM(MFD_0_US_422_TO_444, RING_SUPPION_MODE, DOUBLE) |
			BCHP_FIELD_ENUM(MFD_0_US_422_TO_444, RING_SUPPION,      ENABLE) |
			BCHP_FIELD_DATA(MFD_0_US_422_TO_444, UNBIASED_ROUND_ENABLE,
				hFeeder->stUpSampler.bUnbiasedRound) |
			BCHP_FIELD_DATA(MFD_0_US_422_TO_444, FILT_CTRL,
				hFeeder->stUpSampler.eFilterType));
	}
	else
	{
		BVDC_P_MFD_GET_REG_DATA(MFD_0_US_422_TO_444) |= (
			BCHP_FIELD_ENUM(MFD_0_US_422_TO_444, RING_SUPPION_MODE, NORMAL)  |
			BCHP_FIELD_ENUM(MFD_0_US_422_TO_444, RING_SUPPION,      DISABLE) |
			BCHP_FIELD_DATA(MFD_0_US_422_TO_444, UNBIASED_ROUND_ENABLE,
				hFeeder->stUpSampler.bUnbiasedRound) |
			BCHP_FIELD_DATA(MFD_0_US_422_TO_444, FILT_CTRL,
				hFeeder->stUpSampler.eFilterType));
	}

	return BERR_SUCCESS;
}
#endif

/***************************************************************************
 *
 */
static BERR_Code BVDC_P_Feeder_SetLacCntlOutput_isr
	( BVDC_P_Feeder_Handle             hFeeder,
	  BAVC_Polarity                    ePolarity)
{
	BVDC_P_MFD_GET_REG_DATA(MFD_0_LAC_CNTL) &= ~(
		BCHP_MASK(MFD_0_LAC_CNTL, OUTPUT_TYPE          ) |
		BCHP_MASK(MFD_0_LAC_CNTL, OUTPUT_FIELD_POLARITY) );

	if(ePolarity != BAVC_Polarity_eFrame)
	{
		BVDC_P_MFD_GET_REG_DATA(MFD_0_LAC_CNTL) |=
			BCHP_FIELD_ENUM(MFD_0_LAC_CNTL, OUTPUT_TYPE, INTERLACED         ) |
			BCHP_FIELD_DATA(MFD_0_LAC_CNTL, OUTPUT_FIELD_POLARITY, ePolarity);
	}
	else
	{
		BVDC_P_MFD_GET_REG_DATA(MFD_0_LAC_CNTL) |=
			BCHP_FIELD_ENUM(MFD_0_LAC_CNTL, OUTPUT_TYPE, PROGRESSIVE   ) |
			BCHP_FIELD_ENUM(MFD_0_LAC_CNTL, OUTPUT_FIELD_POLARITY, TOP );
	}

	return BERR_SUCCESS;
}

/***************************************************************************
 * Set MFD_0_LAC_CNTL
 */
static BERR_Code BVDC_P_Feeder_SetLacCntl_isr
	( BVDC_P_Feeder_Handle             hFeeder,
	  const BAVC_MVD_Field            *pFieldData,
	  BAVC_Polarity                    ePolarity)
{
	BVDC_P_Feeder_ChromaVertPos   eChromaVertPos = BVDC_P_Feeder_ChromaVertPos_eHalfPixelGridBetweenLuma;

	if(BVDC_P_Feeder_IS_MPEG(hFeeder))
	{
		BDBG_ASSERT(pFieldData);
		BDBG_ASSERT(pFieldData->eMpegType <= BAVC_ChromaLocation_eType3);

		if((pFieldData->eMpegType == BAVC_ChromaLocation_eType0) ||
		   (pFieldData->eMpegType == BAVC_ChromaLocation_eType1))
		{
			eChromaVertPos = BVDC_P_Feeder_ChromaVertPos_eHalfPixelGridBetweenLuma;
		}
		else if((pFieldData->eMpegType == BAVC_ChromaLocation_eType2) ||
		        (pFieldData->eMpegType == BAVC_ChromaLocation_eType3))
		{
			eChromaVertPos = BVDC_P_Feeder_ChromaVertPos_eColorcatedWithLuma;
		}

		BVDC_P_MFD_GET_REG_DATA(MFD_0_LAC_CNTL) &= ~(
#if (BVDC_P_MFD_SUPPORT_STRIPE_WIDTH_SEL)
			BCHP_MASK(MFD_0_LAC_CNTL, STRIPE_WIDTH_SEL          ) |
#endif
#if (BVDC_P_MFD_SUPPORT_CHROMA_VERT_POSITION)
			BCHP_MASK(MFD_0_LAC_CNTL, CHROMA_VERT_POSITION      ) |
#endif
			BCHP_MASK(MFD_0_LAC_CNTL, CHROMA_TYPE               ) |
			BCHP_MASK(MFD_0_LAC_CNTL, CHROMA_INTERPOLATION      ));

		BVDC_P_MFD_GET_REG_DATA(MFD_0_LAC_CNTL) = (
#if (BVDC_P_MFD_SUPPORT_STRIPE_WIDTH_SEL)
			BCHP_FIELD_DATA(MFD_0_LAC_CNTL, STRIPE_WIDTH_SEL, pFieldData->eStripeWidth) |
#endif
#if (BVDC_P_MFD_SUPPORT_CHROMA_VERT_POSITION)
			BCHP_FIELD_DATA(MFD_0_LAC_CNTL, CHROMA_VERT_POSITION, eChromaVertPos) |
#endif
			BCHP_FIELD_DATA(MFD_0_LAC_CNTL, CHROMA_TYPE,
				pFieldData->eYCbCrType - BAVC_YCbCrType_e4_2_0) |
			BCHP_FIELD_DATA(MFD_0_LAC_CNTL, CHROMA_INTERPOLATION,
				pFieldData->eChrominanceInterpolationMode     ) );

		if(pFieldData->ePxlFmt == BPXL_INVALID)
		{
			BVDC_P_Feeder_SetLacCntlOutput_isr(hFeeder, ePolarity);
		}
		else
		{
			BVDC_P_Feeder_SetLacCntlOutput_isr(hFeeder, BAVC_Polarity_eFrame);
		}
	}
	else
	{
		BVDC_P_Feeder_SetLacCntlOutput_isr(hFeeder, ePolarity);
	}

	return BERR_SUCCESS;
}


/***************************************************************************
 * MFD only
 * Set MFD_0_CHROMA_SAMPLING_CNTL, MFD_0_CHROMA_REPOSITION_DERING_ENABLE
 */
static BERR_Code BVDC_P_Feeder_SetChromaRepEnable_isr
	( BVDC_P_Feeder_Handle             hFeeder,
	  const BAVC_MVD_Field            *pFieldData )
{
	bool     bChromaRepEnable = false;

	BDBG_ASSERT(BVDC_P_Feeder_IS_MPEG(hFeeder));
	BDBG_ASSERT(pFieldData->eMpegType <= BAVC_ChromaLocation_eType3);

	if((pFieldData->eMpegType == BAVC_ChromaLocation_eType0) ||
	   (pFieldData->eMpegType == BAVC_ChromaLocation_eType2))
	{
		bChromaRepEnable = false;
	}
	else if((pFieldData->eMpegType == BAVC_ChromaLocation_eType1) ||
	        (pFieldData->eMpegType == BAVC_ChromaLocation_eType3))
	{
		bChromaRepEnable = true;
	}

	/* CHROMA_REPOSITION_ENABLE controls horizontal filter enabled, and is used to
	 * reposition the chroma pixel to the luma pixel position in horizontal position.
	 * It's required for MPEG1. In the case of MEPG2, chroma is co-located with luma
	 * after vertical filtering. */
	BVDC_P_MFD_GET_REG_DATA(MFD_0_CHROMA_SAMPLING_CNTL) &= ~(
		BCHP_MASK(MFD_0_CHROMA_SAMPLING_CNTL, CHROMA_REPOSITION_ENABLE) );

	BVDC_P_MFD_GET_REG_DATA(MFD_0_CHROMA_SAMPLING_CNTL) |= (
		BCHP_FIELD_DATA(MFD_0_CHROMA_SAMPLING_CNTL, CHROMA_REPOSITION_ENABLE, bChromaRepEnable));

#if (BVDC_P_MFD_SUPPORT_DERINGING)
	BVDC_P_MFD_GET_REG_DATA(MFD_0_CHROMA_REPOSITION_DERING_ENABLE) &= ~(
		BCHP_MASK(MFD_0_CHROMA_REPOSITION_DERING_ENABLE, DERING_EN));

	BVDC_P_MFD_GET_REG_DATA(MFD_0_CHROMA_REPOSITION_DERING_ENABLE) |=  (
		BCHP_FIELD_DATA(MFD_0_CHROMA_REPOSITION_DERING_ENABLE, DERING_EN, bChromaRepEnable));
#endif

	return BERR_SUCCESS;
}

/***************************************************************************
 * MFD only
 * Set MFD_0_RANGE_EXP_REMAP_CNTL
 */
static BERR_Code BVDC_P_Feeder_SetRangeExpRemap_isr
	( BVDC_P_Feeder_Handle             hFeeder,
	  const BAVC_MVD_Field            *pFieldData )
{
	BDBG_ASSERT(BVDC_P_Feeder_IS_MPEG(hFeeder));

	BVDC_P_MFD_GET_REG_DATA(MFD_0_RANGE_EXP_REMAP_CNTL) = (
		BCHP_FIELD_DATA(MFD_0_RANGE_EXP_REMAP_CNTL, SCALE_Y, pFieldData->ulLumaRangeRemapping) |
		BCHP_FIELD_DATA(MFD_0_RANGE_EXP_REMAP_CNTL, SCALE_C, pFieldData->ulChromaRangeRemapping) );

	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
static BERR_Code BVDC_P_Feeder_SetFormatCtrl_isr
	( BVDC_P_Feeder_Handle             hFeeder,
	  const BAVC_MVD_Field            *pFieldData,
	  const BVDC_P_PictureNodePtr      pPicture,
	  BAVC_Polarity                    ePolarity)
{
	/* Set MFD_0_FEEDER_CNTL */
	BVDC_P_Feeder_SetFeederCntl_isr(hFeeder, pFieldData, pPicture);

#if (BVDC_P_MFD_SUPPORT_BYTE_ORDER)
	/* Set MFD_0_BYTE_ORDER */
	BVDC_P_Feeder_SetByteOrder_isr(hFeeder);
#endif

#if (BVDC_P_MFD_SUPPORT_DATA_MODE)
	/* Set MFD_0_DATA_MODE */
	BVDC_P_Feeder_SetDataMode_isr(hFeeder);
#endif

#if (BVDC_P_MFD_SUPPORT_10BIT_444)
	/* Set MFD_0_US_422_TO_444 */
	BVDC_P_Feeder_SetUpSampler(hFeeder);
#endif

	/* Set MFD_0_LAC_CNTL */
	BVDC_P_Feeder_SetLacCntl_isr(hFeeder, pFieldData, ePolarity);

	if(BVDC_P_Feeder_IS_MPEG(hFeeder))
	{
		BDBG_ASSERT(pFieldData);

		/* Set MFD_0_RANGE_EXP_REMAP_CNTL */
		BVDC_P_Feeder_SetRangeExpRemap_isr(hFeeder, pFieldData);
		/* Set MFD_0_CHROMA_SAMPLING_CNTL, MFD_0_CHROMA_REPOSITION_DERING_ENABLE */
		BVDC_P_Feeder_SetChromaRepEnable_isr(hFeeder, pFieldData);
	}

	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
static BERR_Code BVDC_P_Feeder_SetMpegStride_isr
	( BVDC_P_Feeder_Handle             hFeeder,
	  const BAVC_MVD_Field            *pFieldData )
{
	uint32_t                           ulLumaStride, ulChromaStride;
	BVDC_P_Feeder_StripeWidthConfig    eStripeWidthCfg;

	/* Get stripe width settings */
	eStripeWidthCfg = s_aStripeWidthCfgTbl[pFieldData->eStripeWidth];

	/* Get stride */
	ulLumaStride = ( pFieldData->eSourcePolarity == BAVC_Polarity_eFrame )
		? eStripeWidthCfg.ulStripeWidth : eStripeWidthCfg.ulStripeWidth * 2;

	if(pFieldData->eYCbCrType == BAVC_YCbCrType_e4_2_0)
	{
		ulChromaStride = (pFieldData->eChrominanceInterpolationMode ==
			BAVC_InterpolationMode_eFrame)
			? eStripeWidthCfg.ulStripeWidth : eStripeWidthCfg.ulStripeWidth * 2;
	}
	else
	{
		ulChromaStride = ulLumaStride;
	}

	hFeeder->ulChromaStride = ulChromaStride;
	hFeeder->ulLumaStride = ulLumaStride;
	BDBG_MSG(("Chroma stride = 0x%x", ulChromaStride));
	BDBG_MSG(("Luma   stride = 0x%x", ulLumaStride));

	/* Set stride */
	BVDC_P_MFD_GET_REG_DATA(MFD_0_STRIDE) &= ~(
		BCHP_MASK(MFD_0_STRIDE, AVC_MPEG_CHROMA_LINE_STRIDE) |
		BCHP_MASK(MFD_0_STRIDE, AVC_MPEG_LUMA_LINE_STRIDE  ) );

	BVDC_P_MFD_GET_REG_DATA(MFD_0_STRIDE) |= (
		BCHP_FIELD_DATA(MFD_0_STRIDE, AVC_MPEG_CHROMA_LINE_STRIDE, ulChromaStride) |
		BCHP_FIELD_DATA(MFD_0_STRIDE, AVC_MPEG_LUMA_LINE_STRIDE,   ulLumaStride  ) );

	/* Set NMBY */
	BVDC_P_MFD_GET_REG_DATA(MFD_0_LUMA_NMBY) &= ~BCHP_MASK(MFD_0_LUMA_NMBY, VALUE);
	BVDC_P_MFD_GET_REG_DATA(MFD_0_LUMA_NMBY) |=
		BCHP_FIELD_DATA(MFD_0_LUMA_NMBY, VALUE, pFieldData->ulLuminanceNMBY);

	BVDC_P_MFD_GET_REG_DATA(MFD_0_CHROMA_NMBY) &= ~BCHP_MASK(MFD_0_CHROMA_NMBY, VALUE);
	BVDC_P_MFD_GET_REG_DATA(MFD_0_CHROMA_NMBY) |=
		BCHP_FIELD_DATA(MFD_0_CHROMA_NMBY, VALUE, pFieldData->ulChrominanceNMBY);

	return BERR_SUCCESS;
}


/***************************************************************************
	 * Start address calculation
	 * Note:
	 *   AVD frame buffer is organized by stripes (64 pixels wide, or 4
	 * MacroBlock colomns); different AVD frame buffers might be non-
	 * contiguous; H.264 picture might have stream clip;
	 */
static uint32_t BVDC_P_Feeder_GetMpegDeviceBaseAddr_isr
	( BMEM_Heap_Handle                 hHeap,
	  BVDC_P_Feeder_StripeWidthConfig  eStripeWidthCfg,
	  void                            *pVirtualAddr,
	  uint32_t                         ulNMBY,
	  uint32_t                         ulLeftOffset,
	  uint32_t                         ulVertOffset,
	  bool                             bIsLuma,
	  const BAVC_MVD_Field            *pFieldData )
{
	uint32_t        ulDeviceAddr = 0;
	uint32_t        ulStripeWidth, ulStripeShift;

#if (!BVDC_P_MFD_FEEDER_IS_MVFD)
	BSTD_UNUSED(bIsLuma);
#endif

	ulStripeWidth   = eStripeWidthCfg.ulStripeWidth;
	ulStripeShift   = eStripeWidthCfg.ulShift;

	/* Convert the chroma frame buffer base address to device offset address */
	if(BMEM_Heap_ConvertAddressToOffset_isr(hHeap, pVirtualAddr,
	    &ulDeviceAddr) != BERR_SUCCESS)
	{
		goto done;
	}

	/* to calculate the chroma starting address including the pan/scan/cropping; */
	ulDeviceAddr +=
		/* stripe offset */
		(ulLeftOffset >> ulStripeShift) * ulNMBY * 16 * ulStripeWidth +

		/* vertical offset within a stripe (take care of 422 vs 420 chroma type) */
		ulVertOffset +

		/* horizontal offset within a stripe;
		 *  NOTE: we don't support 4:4:4 chroma; keep it even number; */
		(ulLeftOffset & (ulStripeWidth-1));

#if (BVDC_P_MFD_FEEDER_IS_MVFD)
	/* adjustment for bottom field offset within a stripe on MVFD:
	 * Luma: add stripe width for bottom field
	 * Chroma: add stripe width for bottom field when CHROMA_INTERPOLATION is field
	 */
	if( pFieldData->eSourcePolarity == BAVC_Polarity_eBotField )
	{
		if((pFieldData->eChrominanceInterpolationMode == BAVC_InterpolationMode_eField) ||
		    bIsLuma )
		{
			ulDeviceAddr   += ulStripeWidth;
		}
	}

#else
	/* adjustment for bottom field offset within a stripe */
	if( pFieldData->eSourcePolarity == BAVC_Polarity_eBotField )
	{
		ulDeviceAddr   += ulStripeWidth ;
	}
#endif

done:
	return ulDeviceAddr;
}

/***************************************************************************
	 * Device address calculation
	 */
static void BVDC_P_Feeder_GetMpegDeviceAddr_isr
	( BVDC_P_Feeder_Handle                 hFeeder,
	  BMEM_Heap_Handle                     hHeap,
	  const BAVC_MVD_Field                *pFieldData,
	  const BVDC_P_Rect                   *pScanoutRect,
	  BVDC_P_Feeder_MpegDeviceAddrConfig  *pstDeviceAddr )
{
	uint32_t        ulLumaVertOffset, ulChromaVertOffset;

	uint32_t        ulTopOffset, ulLeftOffset, ulLeftOffset_R;
	uint32_t        ulStripeWidth;
	BAVC_YCbCrType  eYCbCrType = pFieldData->eYCbCrType;
	BVDC_P_Feeder_StripeWidthConfig    eStripeWidthCfg;

	BDBG_ASSERT(pstDeviceAddr);

	eStripeWidthCfg = s_aStripeWidthCfgTbl[pFieldData->eStripeWidth];
	ulStripeWidth   = eStripeWidthCfg.ulStripeWidth;

	/* a stripe is 64 pixels wide
	   NOTE: don't support 4:4:4; */
	ulLeftOffset = (pScanoutRect->lLeft + pFieldData->ulSourceClipLeft) & ~1;
	/* Get the offset of the right buffer */
	ulLeftOffset_R = (pScanoutRect->lLeft_R + pFieldData->ulSourceClipLeft +
		pFieldData->ulSourceHorizontalSize) & ~1;

	if(pFieldData->eYCbCrType == BAVC_YCbCrType_e4_2_0)
	{
		ulTopOffset  = (pScanoutRect->lTop  + pFieldData->ulSourceClipTop ) & ~1;
	}
	else
	{
		ulTopOffset  = pScanoutRect->lTop  + pFieldData->ulSourceClipTop;
	}

	ulLumaVertOffset   = ulTopOffset * ulStripeWidth;
	ulChromaVertOffset = (ulTopOffset * ulStripeWidth / 2) << (eYCbCrType - BAVC_YCbCrType_e4_2_0);

	pstDeviceAddr->ulLumaDeviceAddr = BVDC_P_Feeder_GetMpegDeviceBaseAddr_isr(
		hHeap, eStripeWidthCfg, pFieldData->pLuminanceFrameBufferAddress,
		pFieldData->ulLuminanceNMBY, ulLeftOffset, ulLumaVertOffset,
		true, pFieldData);

	pstDeviceAddr->ulChromaDeviceAddr = BVDC_P_Feeder_GetMpegDeviceBaseAddr_isr(
		hHeap, eStripeWidthCfg, pFieldData->pChrominanceFrameBufferAddress,
		pFieldData->ulChrominanceNMBY, ulLeftOffset, ulChromaVertOffset,
		false, pFieldData);

#if (BVDC_P_MFD_SUPPORT_3D_VIDEO)
	if(pFieldData->eOrientation == BFMT_Orientation_e3D_LeftRight)
	{
		pstDeviceAddr->ulLumaDeviceAddr_R = BVDC_P_Feeder_GetMpegDeviceBaseAddr_isr(
			hHeap, eStripeWidthCfg, pFieldData->pLuminanceFrameBufferAddress,
			pFieldData->ulLuminanceNMBY, ulLeftOffset_R, ulLumaVertOffset,
			true, pFieldData);

		pstDeviceAddr->ulChromaDeviceAddr_R = BVDC_P_Feeder_GetMpegDeviceBaseAddr_isr(
			hHeap, eStripeWidthCfg, pFieldData->pChrominanceFrameBufferAddress,
			pFieldData->ulChrominanceNMBY, ulLeftOffset_R, ulChromaVertOffset,
			false, pFieldData);
	}
	else if(pFieldData->eOrientation == BFMT_Orientation_e3D_OverUnder)
	{
		uint32_t   ulTopOffset_R, ulLumaVertOffset_R, ulChromaVertOffset_R;

		/* Get the offset of the right buffer */
		if(pFieldData->eYCbCrType == BAVC_YCbCrType_e4_2_0)
		{
			ulTopOffset_R  = (pScanoutRect->lTop  + pFieldData->ulSourceClipTop
				+ pFieldData->ulSourceVerticalSize) & ~1;
		}
		else
		{
			ulTopOffset_R  = pScanoutRect->lTop  + pFieldData->ulSourceClipTop
				+ pFieldData->ulSourceVerticalSize;
		}

		ulLumaVertOffset_R = ulTopOffset_R * ulStripeWidth;
		ulChromaVertOffset_R = (ulTopOffset_R * ulStripeWidth / 2) << (eYCbCrType - BAVC_YCbCrType_e4_2_0);

		pstDeviceAddr->ulLumaDeviceAddr_R = BVDC_P_Feeder_GetMpegDeviceBaseAddr_isr(
			hHeap, eStripeWidthCfg, pFieldData->pLuminanceFrameBufferAddress,
			pFieldData->ulLuminanceNMBY, ulLeftOffset, ulLumaVertOffset_R,
			true, pFieldData);

		pstDeviceAddr->ulChromaDeviceAddr_R = BVDC_P_Feeder_GetMpegDeviceBaseAddr_isr(
			hHeap, eStripeWidthCfg, pFieldData->pChrominanceFrameBufferAddress,
			pFieldData->ulChrominanceNMBY, ulLeftOffset, ulChromaVertOffset_R,
			false, pFieldData);

	}
	else if((pFieldData->eOrientation == BFMT_Orientation_e3D_Left) ||
		(pFieldData->eOrientation == BFMT_Orientation_e3D_Right))
	{
		BAVC_MVD_Field   *pEnhancedFieldData;

		pEnhancedFieldData = (BAVC_MVD_Field*)(pFieldData->pEnhanced);
		BDBG_ASSERT(pEnhancedFieldData);

		if(pFieldData->eOrientation == BFMT_Orientation_e3D_Left)
		{
			BDBG_ASSERT(pEnhancedFieldData->eOrientation == BFMT_Orientation_e3D_Right);

			/* pEnhancedFieldData is right buffer */
			pstDeviceAddr->ulLumaDeviceAddr_R = BVDC_P_Feeder_GetMpegDeviceBaseAddr_isr(
				hHeap, eStripeWidthCfg, pEnhancedFieldData->pLuminanceFrameBufferAddress,
				pFieldData->ulLuminanceNMBY, ulLeftOffset, ulLumaVertOffset,
				true, pFieldData);

			pstDeviceAddr->ulChromaDeviceAddr_R = BVDC_P_Feeder_GetMpegDeviceBaseAddr_isr(
				hHeap, eStripeWidthCfg, pEnhancedFieldData->pChrominanceFrameBufferAddress,
				pFieldData->ulChrominanceNMBY, ulLeftOffset, ulChromaVertOffset,
				false, pFieldData);
		}
		else
		{
			BDBG_ASSERT(pEnhancedFieldData->eOrientation == BFMT_Orientation_e3D_Left);

			pstDeviceAddr->ulLumaDeviceAddr_R   = pstDeviceAddr->ulLumaDeviceAddr;
			pstDeviceAddr->ulChromaDeviceAddr_R = pstDeviceAddr->ulChromaDeviceAddr;

			/* pEnhancedFieldData is left buffer */
			pstDeviceAddr->ulLumaDeviceAddr = BVDC_P_Feeder_GetMpegDeviceBaseAddr_isr(
				hHeap, eStripeWidthCfg, pEnhancedFieldData->pLuminanceFrameBufferAddress,
				pFieldData->ulLuminanceNMBY, ulLeftOffset, ulLumaVertOffset,
				true, pFieldData);

			pstDeviceAddr->ulChromaDeviceAddr = BVDC_P_Feeder_GetMpegDeviceBaseAddr_isr(
				hHeap, eStripeWidthCfg, pEnhancedFieldData->pChrominanceFrameBufferAddress,
				pFieldData->ulChrominanceNMBY, ulLeftOffset, ulChromaVertOffset,
				false, pFieldData);
		}
	}
	else if(pFieldData->eOrientation == BFMT_Orientation_eLeftRight_Enhanced)
	{
		BAVC_MVD_Field   *pEnhancedFieldData;

		pEnhancedFieldData = (BAVC_MVD_Field*)(pFieldData->pEnhanced);
		BDBG_ASSERT(pEnhancedFieldData);

		/* Get the offset of the right buffer */
		ulLeftOffset_R = (pScanoutRect->lLeft_R + pFieldData->ulSourceClipLeft +
			pFieldData->ulSourceHorizontalSize/2) & ~1;

		pstDeviceAddr->ulLumaDeviceAddr_R = BVDC_P_Feeder_GetMpegDeviceBaseAddr_isr(
			hHeap, eStripeWidthCfg, pFieldData->pLuminanceFrameBufferAddress,
			pFieldData->ulLuminanceNMBY, ulLeftOffset_R, ulLumaVertOffset,
			true, pFieldData);

		pstDeviceAddr->ulChromaDeviceAddr_R = BVDC_P_Feeder_GetMpegDeviceBaseAddr_isr(
			hHeap, eStripeWidthCfg, pFieldData->pChrominanceFrameBufferAddress,
			pFieldData->ulChrominanceNMBY, ulLeftOffset_R, ulChromaVertOffset,
			false, pFieldData);

#if (BVDC_P_MFD_SUPPORT_3D_VIDEO_MRE)
		pstDeviceAddr->ulLumaDeviceAddr_hp = BVDC_P_Feeder_GetMpegDeviceBaseAddr_isr(
			hHeap, eStripeWidthCfg, pEnhancedFieldData->pLuminanceFrameBufferAddress,
			pFieldData->ulLuminanceNMBY, ulLeftOffset, ulLumaVertOffset,
			true, pFieldData);

		pstDeviceAddr->ulChromaDeviceAddr_hp = BVDC_P_Feeder_GetMpegDeviceBaseAddr_isr(
			hHeap, eStripeWidthCfg, pEnhancedFieldData->pChrominanceFrameBufferAddress,
			pFieldData->ulChrominanceNMBY, ulLeftOffset, ulChromaVertOffset,
			false, pFieldData);

		pstDeviceAddr->ulLumaDeviceAddr_R_hp = BVDC_P_Feeder_GetMpegDeviceBaseAddr_isr(
			hHeap, eStripeWidthCfg, pEnhancedFieldData->pLuminanceFrameBufferAddress,
			pFieldData->ulLuminanceNMBY, ulLeftOffset_R, ulLumaVertOffset,
			true, pFieldData);

		pstDeviceAddr->ulChromaDeviceAddr_R_hp = BVDC_P_Feeder_GetMpegDeviceBaseAddr_isr(
			hHeap, eStripeWidthCfg, pEnhancedFieldData->pChrominanceFrameBufferAddress,
			pFieldData->ulChrominanceNMBY, ulLeftOffset_R, ulChromaVertOffset,
			false, pFieldData);
#endif
	}
	else
	{
		pstDeviceAddr->ulLumaDeviceAddr_R   = pstDeviceAddr->ulLumaDeviceAddr;
		pstDeviceAddr->ulChromaDeviceAddr_R = pstDeviceAddr->ulChromaDeviceAddr;
	}
#endif

	if(hFeeder->bSkipFirstLine)
	{
		pstDeviceAddr->ulLumaDeviceAddr += hFeeder->ulLumaStride;
		pstDeviceAddr->ulChromaDeviceAddr += hFeeder->ulChromaStride;

#if (BVDC_P_MFD_SUPPORT_3D_VIDEO)
		pstDeviceAddr->ulChromaDeviceAddr_R += hFeeder->ulChromaStride;
		pstDeviceAddr->ulLumaDeviceAddr_R += hFeeder->ulLumaStride;

#if (BVDC_P_MFD_SUPPORT_3D_VIDEO_MRE)
		pstDeviceAddr->ulLumaDeviceAddr_hp += hFeeder->ulLumaStride;
		pstDeviceAddr->ulLumaDeviceAddr_R_hp += hFeeder->ulLumaStride;
		pstDeviceAddr->ulChromaDeviceAddr_hp += hFeeder->ulChromaStride;
		pstDeviceAddr->ulChromaDeviceAddr_R_hp += hFeeder->ulChromaStride;
#endif
#endif
	}


	return;
}

/***************************************************************************
 *
 */
static BERR_Code BVDC_P_Feeder_SetMpegAddr_isr
	( BVDC_P_Feeder_Handle             hFeeder,
	  BMEM_Heap_Handle                 hHeap,
	  const BAVC_MVD_Field            *pFieldData,
	  const BVDC_P_Rect               *pScanoutRect)
{
	BVDC_P_Feeder_MpegDeviceAddrConfig   stMpegDeviceAddr;

	BKNI_Memset((void*)&stMpegDeviceAddr, 0x0, sizeof(BVDC_P_Feeder_MpegDeviceAddrConfig));

	BVDC_P_Feeder_GetMpegDeviceAddr_isr(hFeeder, hHeap, pFieldData, pScanoutRect,
		&stMpegDeviceAddr);

	/* Set address */
	/* MFD_0_PICTURE0_LINE_ADDR_0 */
	BVDC_P_MFD_GET_REG_DATA(MFD_0_PICTURE0_LINE_ADDR_0) &= ~(
		BCHP_MASK(MFD_0_PICTURE0_LINE_ADDR_0, AVC_MPEG_LUMA_ADDR) );

	BVDC_P_MFD_GET_REG_DATA(MFD_0_PICTURE0_LINE_ADDR_0) |= (
		BCHP_FIELD_DATA(MFD_0_PICTURE0_LINE_ADDR_0, AVC_MPEG_LUMA_ADDR,
			stMpegDeviceAddr.ulLumaDeviceAddr) );

	/* MFD_0_PICTURE0_LINE_ADDR_1 */
	BVDC_P_MFD_GET_REG_DATA(MFD_0_PICTURE0_LINE_ADDR_1) &= ~(
		BCHP_MASK(MFD_0_PICTURE0_LINE_ADDR_1, AVC_MPEG_CHROMA_ADDR) );

	BVDC_P_MFD_GET_REG_DATA(MFD_0_PICTURE0_LINE_ADDR_1) |= (
		BCHP_FIELD_DATA(MFD_0_PICTURE0_LINE_ADDR_1, AVC_MPEG_CHROMA_ADDR,
			stMpegDeviceAddr.ulChromaDeviceAddr) );

#if (BVDC_P_MFD_SUPPORT_3D_VIDEO)
	/* MFD_0_PICTURE0_LINE_ADDR_0_R */
	BVDC_P_MFD_GET_REG_DATA(MFD_0_PICTURE0_LINE_ADDR_0_R) &= ~(
		BCHP_MASK(MFD_0_PICTURE0_LINE_ADDR_0_R, AVC_MPEG_LUMA_ADDR) );

	BVDC_P_MFD_GET_REG_DATA(MFD_0_PICTURE0_LINE_ADDR_0_R) |= (
		BCHP_FIELD_DATA(MFD_0_PICTURE0_LINE_ADDR_0_R, AVC_MPEG_LUMA_ADDR,
			stMpegDeviceAddr.ulLumaDeviceAddr_R) );

	/* MFD_0_PICTURE0_LINE_ADDR_1_R */
	BVDC_P_MFD_GET_REG_DATA(MFD_0_PICTURE0_LINE_ADDR_1_R) &= ~(
		BCHP_MASK(MFD_0_PICTURE0_LINE_ADDR_1_R, AVC_MPEG_CHROMA_ADDR) );

	BVDC_P_MFD_GET_REG_DATA(MFD_0_PICTURE0_LINE_ADDR_1_R) |= (
		BCHP_FIELD_DATA(MFD_0_PICTURE0_LINE_ADDR_1_R, AVC_MPEG_CHROMA_ADDR,
			stMpegDeviceAddr.ulChromaDeviceAddr_R) );

#if (BVDC_P_MFD_SUPPORT_3D_VIDEO_MRE)
	if(hFeeder->eId == BVDC_P_FeederId_eMfd0)
	{
		/* MFD_0_MRE3D_LINE_ADDR_0_HP */
		BVDC_P_MFD_GET_REG_DATA(MFD_0_MRE3D_LINE_ADDR_0_HP) &= ~(
			BCHP_MASK(MFD_0_MRE3D_LINE_ADDR_0_HP, AVC_MPEG_LUMA_ADDR) );

		BVDC_P_MFD_GET_REG_DATA(MFD_0_MRE3D_LINE_ADDR_0_HP) |= (
			BCHP_FIELD_DATA(MFD_0_MRE3D_LINE_ADDR_0_HP, AVC_MPEG_LUMA_ADDR,
				stMpegDeviceAddr.ulLumaDeviceAddr_hp) );

		/* MFD_0_MRE3D_LINE_ADDR_1_HP */
		BVDC_P_MFD_GET_REG_DATA(MFD_0_MRE3D_LINE_ADDR_1_HP) &= ~(
			BCHP_MASK(MFD_0_MRE3D_LINE_ADDR_1_HP, AVC_MPEG_CHROMA_ADDR) );

		BVDC_P_MFD_GET_REG_DATA(MFD_0_MRE3D_LINE_ADDR_1_HP) |= (
			BCHP_FIELD_DATA(MFD_0_MRE3D_LINE_ADDR_1_HP, AVC_MPEG_CHROMA_ADDR,
				stMpegDeviceAddr.ulChromaDeviceAddr_hp) );

		/* MFD_0_MRE3D_LINE_ADDR_0_R_HP */
		BVDC_P_MFD_GET_REG_DATA(MFD_0_MRE3D_LINE_ADDR_0_R_HP) &= ~(
			BCHP_MASK(MFD_0_MRE3D_LINE_ADDR_0_R_HP, AVC_MPEG_LUMA_ADDR) );

		BVDC_P_MFD_GET_REG_DATA(MFD_0_MRE3D_LINE_ADDR_0_R_HP) |= (
			BCHP_FIELD_DATA(MFD_0_MRE3D_LINE_ADDR_0_R_HP, AVC_MPEG_LUMA_ADDR,
				stMpegDeviceAddr.ulLumaDeviceAddr_R_hp) );

		/* MFD_0_MRE3D_LINE_ADDR_1_R_HP */
		BVDC_P_MFD_GET_REG_DATA(MFD_0_MRE3D_LINE_ADDR_1_R_HP) &= ~(
			BCHP_MASK(MFD_0_MRE3D_LINE_ADDR_1_R_HP, AVC_MPEG_CHROMA_ADDR) );

		BVDC_P_MFD_GET_REG_DATA(MFD_0_MRE3D_LINE_ADDR_1_R_HP) |= (
			BCHP_FIELD_DATA(MFD_0_MRE3D_LINE_ADDR_1_R_HP, AVC_MPEG_CHROMA_ADDR,
				stMpegDeviceAddr.ulChromaDeviceAddr_R_hp) );
	}
#endif
#endif

	return BERR_SUCCESS;
}


/***************************************************************************
 *
 */
static BERR_Code BVDC_P_Feeder_SetSurfaceStrideAddr_isr
	( BVDC_P_Feeder_Handle             hFeeder,
	  const BAVC_MVD_Field            *pFieldData,
	  const BVDC_P_Rect               *pScanoutRect)
{
	uint32_t  ulStride, ulOffsetByteInLine;
	BERR_Code     err = BERR_SUCCESS;

	/* BVDC_P_Source_ValidateMpegData_isr overwrite
	 * pFieldData->ulRowStride to use surface stride */
	/* Gfx surface with field polarity does leave space for the other poalrity */
	ulStride = pFieldData->ulRowStride;

	BPXL_GetBytesPerNPixels(pFieldData->ePxlFmt,
		(pScanoutRect->lLeft & ~1), &ulOffsetByteInLine);
	hFeeder->stGfxSurface.ulMainByteOffset =
		pScanoutRect->lTop * ulStride + ulOffsetByteInLine;

	BVDC_P_MFD_GET_REG_DATA(MFD_0_STRIDE) &= ~(
		BCHP_MASK(MFD_0_STRIDE, PACKED_LINE_STRIDE) );
	BVDC_P_MFD_GET_REG_DATA(MFD_0_STRIDE) |=
		BCHP_FIELD_DATA(MFD_0_STRIDE, PACKED_LINE_STRIDE, ulStride);

	/* Set address: real addr is set to scratch registers */
	BVDC_P_MFD_GET_REG_DATA(MFD_0_PICTURE0_LINE_ADDR_0) &= ~(
		BCHP_MASK(MFD_0_PICTURE0_LINE_ADDR_0, PACKED_LUMA_CHROMA_ADDR) );
#if (BVDC_P_MFD_SUPPORT_3D_VIDEO)
	BVDC_P_MFD_GET_REG_DATA(MFD_0_PICTURE0_LINE_ADDR_0_R) &= ~(
		BCHP_MASK(MFD_0_PICTURE0_LINE_ADDR_0_R, PACKED_LUMA_CHROMA_ADDR) );
#endif

	return err;
}

/***************************************************************************
 * {private}
 *
 * Input:
 *   pFieldData:
 *      Only contains the data from stream.
 *   pScanoutRect:
 *       Points to the actual display rectangle feeder needs to feeds out.
 *       See BVDC_P_Source_GetScanOutRect for detailed information.
 *   pScanOutRect->lLeft:
 *      This is the horizontal pan scan for feeder, a positive number in the format of U32,
 *      where bits 0-3 are pixel pan scan in pixel grid, bits 4-31 are macroblock pan
 *      scan in macroblock grid. Even number of pixels are handled in feeder, others handled
 *      by scaler.
 *  pScanOutRect->lTop:
  *     This is the vertical pan scan for feeder, a positive number in the format of U32,
 *      where bits 0-3 are pixel pan scan in pixel grid, bits 4-31 are macroblock pan scan
 *      in macroblock grid. Down to pixels are handled in feeder, others handled by scaler.
 *  pScanOutRect->ulWidth:
 *      This is the horizontal scan out size, a positive number in units of pixel.
 *  pScanOutRect->ulHeight:
 *      This is the vertical scan out size, a positive number in units of pixel.
 *
 */
BERR_Code BVDC_P_Feeder_SetMpegInfo_isr
	( BVDC_P_Feeder_Handle             hFeeder,
	  const uint32_t                   ulMuteColorYCrCb,
	  const BAVC_MVD_Field            *pFieldData,
	  const BVDC_P_Rect               *pScanoutRect)
{
	BMEM_Heap_Handle           hHeap;

	BDBG_ENTER(BVDC_P_Feeder_SetMpegInfo_isr);
	BDBG_OBJECT_ASSERT(hFeeder, BVDC_FDR);

	/* Check parameter */
	BDBG_ASSERT(BVDC_P_Feeder_IS_MPEG(hFeeder));

	/* Which heap do these frame buffers come from? */
	hHeap = pFieldData->hHeap ? pFieldData->hHeap : hFeeder->hMem;

	/* Chroma interpolation mode can be frame or field when scan
	 * type is interlaced. Chroma interpolation mode is always
	 * frame when scan type is progressive */
	if((!pFieldData->bMute) && (pFieldData->ePxlFmt == BPXL_INVALID) &&
	   (pFieldData->eSourcePolarity == BAVC_Polarity_eFrame) &&
	   (pFieldData->eChrominanceInterpolationMode == BAVC_InterpolationMode_eField))
	{
		BDBG_WRN(("Bad scan type %d and interpolation mode combination %d",
			pFieldData->eSourcePolarity, pFieldData->eChrominanceInterpolationMode));
	}

	if((pFieldData->eStripeWidth >= BVDC_P_FEEDER_STRIPE_WIDTH_CONFIG_TABLE_CNT) &&
	   (pFieldData->ePxlFmt == BPXL_INVALID))
	{
		BDBG_WRN(("Bad eStripeWidth %d", pFieldData->eStripeWidth));
		return BERR_INVALID_PARAMETER;
	}

	/* Common MPEG settings */
	BVDC_P_MFD_GET_REG_DATA(MFD_0_DISP_HSIZE) &= ~(BCHP_MASK(MFD_0_DISP_HSIZE, VALUE));
	BVDC_P_MFD_GET_REG_DATA(MFD_0_DISP_HSIZE) |= (
		BCHP_FIELD_DATA(MFD_0_DISP_HSIZE, VALUE, pScanoutRect->ulWidth) );
	BDBG_MSG(("Scanout HSIZE = 0x%x", pScanoutRect->ulWidth));

	/* don't use feeder's deinterlace feature for packet format */
	if (BPXL_IS_YCbCr422_FORMAT(pFieldData->ePxlFmt) &&
	   (pFieldData->eSourcePolarity != BAVC_Polarity_eFrame))
	{
		BVDC_P_Feeder_SetVertWindow_isr(hFeeder, BAVC_Polarity_eFrame,
			0, pScanoutRect->ulHeight / 2);
	}
	else
	{
		BVDC_P_Feeder_SetVertWindow_isr(hFeeder, pFieldData->eSourcePolarity,
			0, pScanoutRect->ulHeight);
	}

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)
	/* Gather information */
	BVDC_P_Feeder_GetFormatCtrlSettings_isr(hFeeder, pFieldData,
		pScanoutRect, NULL);
#endif

	/* Check if need to do fixed color feed */
	if( pFieldData->bMute )
	{
		BVDC_P_Feeder_SetFixedColor_isr(hFeeder, pFieldData->eSourcePolarity,
			ulMuteColorYCrCb);
	}
	else
	{
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)
#else
		/* Gather information */
		BVDC_P_Feeder_GetFormatCtrlSettings_isr(hFeeder, pFieldData,
			pScanoutRect, NULL);
#endif
		/*
		* Start format change settings
		*/
		BVDC_P_Feeder_SetFormatCtrl_isr(hFeeder, pFieldData, NULL,
			pFieldData->eSourcePolarity);

		/* Set strides and buf start addr */
		if(pFieldData->ePxlFmt == BPXL_INVALID)
		{
			BVDC_P_Feeder_SetMpegStride_isr(hFeeder, pFieldData);
		}
		else
		{
			BVDC_P_Feeder_SetSurfaceStrideAddr_isr(hFeeder, pFieldData,
				pScanoutRect);
		}
		BVDC_P_Feeder_SetMpegAddr_isr(hFeeder, hHeap, pFieldData, pScanoutRect);
	}

#if (BVDC_P_MFD_SUPPORT_CSC)
	BVDC_P_Feeder_SetCsc_isr(hFeeder, pFieldData);
#endif

	/* Enable feeder */
	if( pScanoutRect->ulWidth )
	{
		BVDC_P_Feeder_SetEnable_isr(hFeeder, true);
	}
	else
	{
		BVDC_P_Feeder_SetEnable_isr(hFeeder, false);
	}

	BDBG_LEAVE(BVDC_P_Feeder_SetMpegInfo_isr);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
static BERR_Code BVDC_P_Feeder_SetPlaybackStrideAddr_isr
	( BVDC_P_Feeder_Handle             hFeeder,
	  const BVDC_P_PictureNodePtr      pPicture,
	  bool                             bProgressiveCap,
	  bool                             bEnableAlignWkr,
	  BAVC_Polarity                    ePolarity)
{
	unsigned int   uiStride, uiByteOffset;
	uint32_t  ulDeviceAddr, ulStride;
#if (BVDC_P_MFD_SUPPORT_3D_VIDEO)
	uint32_t  ulDeviceAddr_R = 0;
#endif

	if (hFeeder->bGfxSrc)
	{
		BVDC_P_Rect *pSrcOut = &pPicture->stSrcOut;
		uint32_t  ulBitsPerPixel, ulOffsetByteInLine;

		ulBitsPerPixel = BPXL_BITS_PER_PIXEL(pPicture->ePixelFormat);
		ulOffsetByteInLine = ulBitsPerPixel * pSrcOut->lLeft / 8;
		ulStride = (ePolarity == BAVC_Polarity_eFrame)?
			hFeeder->stGfxSurface.stCurSurInfo.ulPitch :
			hFeeder->stGfxSurface.stCurSurInfo.ulPitch * 2;
		ulDeviceAddr = 0; /* really set from scratch register */
		hFeeder->stGfxSurface.ulMainByteOffset =
			pSrcOut->lTop * ulStride + ulOffsetByteInLine;
	}
	else
	{
		/* Stride is defined by capture width and alignment has to match
		   with CAP pitch as well! */
		BPXL_GetBytesPerNPixels(pPicture->ePixelFormat,
			(pPicture->pVfdIn->ulWidth + 1) & ~0x1, &uiStride);
		ulStride = BVDC_P_ALIGN_UP(uiStride, BVDC_P_PITCH_ALIGN);

		/* CaptureScaler mode analog video needs to handle source clipping here */
		/* we allow different vbi pass-through lines from the same vdec source */
		BPXL_GetBytesPerNPixels(pPicture->ePixelFormat,
			(pPicture->pVfdOut->lLeft & ~1), &uiByteOffset);

		/* Convert address to device offset address */
		ulDeviceAddr = BVDC_P_Buffer_GetDeviceOffset(pPicture);
		ulDeviceAddr += uiByteOffset +
			 ulStride * (bProgressiveCap ? pPicture->pVfdOut->lTop : pPicture->pVfdOut->lTop / 2);

		if(bEnableAlignWkr)
			ulDeviceAddr += 4;

#if (BVDC_P_MFD_SUPPORT_3D_VIDEO)
		/* TODO: optimize ulDeviceAddr and ulDeviceAddr_R */
		if(hFeeder->hWindow && hFeeder->hWindow->eBufAllocMode == BVDC_P_BufHeapAllocMode_eLRSeparate)
		{
			ulDeviceAddr_R = BVDC_P_Buffer_GetDeviceOffset_R(pPicture);
			ulDeviceAddr_R += uiByteOffset +
				 ulStride * (bProgressiveCap ? pPicture->pVfdOut->lTop : pPicture->pVfdOut->lTop / 2);
			if(bEnableAlignWkr)
				ulDeviceAddr_R += 4;
		}
		else if(hFeeder->hWindow && hFeeder->hWindow->eBufAllocMode == BVDC_P_BufHeapAllocMode_eLRCombined)
		{
			uint32_t  ulBufHeapSize;

			BVDC_P_BufferHeap_GetHeapSizeById(
				hFeeder->hWindow->hCompositor->hVdc->hBufferHeap,
				hFeeder->hWindow->eBufferHeapIdRequest, &ulBufHeapSize);

			ulDeviceAddr_R = ulDeviceAddr + ulBufHeapSize / 2;
		}
		else
		{
			/* If source is 2D, set right buffer same as left in case display
			 * is 3D mode */
			ulDeviceAddr_R = ulDeviceAddr;
		}
#endif

		if(ePolarity != BAVC_Polarity_eFrame)
		{
			/* bottom field starts from one frame line later; */
			if(BAVC_Polarity_eBotField == ePolarity)
			{
				/* one frame line stride */
				ulDeviceAddr += ulStride;
#if (BVDC_P_MFD_SUPPORT_3D_VIDEO)
				ulDeviceAddr_R += ulStride;
#endif
			}
			/* Double the stride because we captured frames and scanout as fields. */
			ulStride *= 2;
		}
	}


	BVDC_P_MFD_GET_REG_DATA(MFD_0_STRIDE) &= ~(
		BCHP_MASK(MFD_0_STRIDE, PACKED_LINE_STRIDE) );

	BVDC_P_MFD_GET_REG_DATA(MFD_0_STRIDE) |= (
		BCHP_FIELD_DATA(MFD_0_STRIDE, PACKED_LINE_STRIDE, ulStride) );

	/* Set address */
	BVDC_P_MFD_GET_REG_DATA(MFD_0_PICTURE0_LINE_ADDR_0) &= ~(
		BCHP_MASK(MFD_0_PICTURE0_LINE_ADDR_0, PACKED_LUMA_CHROMA_ADDR) );

#if (BVDC_P_MFD_SUPPORT_3D_VIDEO)
	if((pPicture->eDispOrientation != BFMT_Orientation_e2D) &&
	   (pPicture->e3dSrcBufSel == BVDC_3dSourceBufferSelect_eRightBuffer))
	{
		BVDC_P_MFD_GET_REG_DATA(MFD_0_PICTURE0_LINE_ADDR_0) |= (
			BCHP_FIELD_DATA(MFD_0_PICTURE0_LINE_ADDR_0, PACKED_LUMA_CHROMA_ADDR, ulDeviceAddr_R) );
	}
	else
#endif
	{
		BVDC_P_MFD_GET_REG_DATA(MFD_0_PICTURE0_LINE_ADDR_0) |= (
			BCHP_FIELD_DATA(MFD_0_PICTURE0_LINE_ADDR_0, PACKED_LUMA_CHROMA_ADDR, ulDeviceAddr) );
	}

#if (BVDC_P_MFD_SUPPORT_3D_VIDEO)
	/* Set address */
	BVDC_P_MFD_GET_REG_DATA(MFD_0_PICTURE0_LINE_ADDR_0_R) &= ~(
		BCHP_MASK(MFD_0_PICTURE0_LINE_ADDR_0_R, PACKED_LUMA_CHROMA_ADDR) );

	if((pPicture->eDispOrientation != BFMT_Orientation_e2D) &&
	   (pPicture->e3dSrcBufSel == BVDC_3dSourceBufferSelect_eLeftBuffer))
	{
		BVDC_P_MFD_GET_REG_DATA(MFD_0_PICTURE0_LINE_ADDR_0_R) |= (
			BCHP_FIELD_DATA(MFD_0_PICTURE0_LINE_ADDR_0_R, PACKED_LUMA_CHROMA_ADDR, ulDeviceAddr) );
	}
	else
	{
		BVDC_P_MFD_GET_REG_DATA(MFD_0_PICTURE0_LINE_ADDR_0_R) |= (
			BCHP_FIELD_DATA(MFD_0_PICTURE0_LINE_ADDR_0_R, PACKED_LUMA_CHROMA_ADDR, ulDeviceAddr_R) );
	}
#endif

	return BERR_SUCCESS;
}

#if (BVDC_P_SUPPORT_VIDEO_TESTFEATURE1_CAP)
/***************************************************************************
 * {private}
 *
 */
BERR_Code BVDC_P_Feeder_SetTestFeature1_isr
	( BVDC_P_Feeder_Handle             hFeeder,
	  const BVDC_P_PictureNodePtr      pPicture )
{
#if (BVDC_P_DCX_VFD_RESET_WORKAROUND)
	/* TODO: Remove it later when fixed */
	/* Reset VFD during switch */
	if(hFeeder->stTestFeature1.bEnable != pPicture->stCapTestFeature1.bEnable)
	{
		hFeeder->bInitial = true;
	}
#endif

	/* DCx is not in MFD !!!.
	 * Can not set shadow registers in MFD. Save the settings in
	 * hFeeder->pTestFeature1 */
	hFeeder->stTestFeature1 = pPicture->stCapTestFeature1;

	return BERR_SUCCESS;
}
#endif

/***************************************************************************
 * {private}
 *
 */
void BVDC_P_Feeder_SetPlaybackInfo_isr
	( BVDC_P_Feeder_Handle             hFeeder,
	  const BVDC_P_PictureNodePtr      pPicture,
	  bool                             bFixedColor,
	  const uint32_t                   ulMuteColorYCrCb)
{
	uint32_t                   ulHeight, ulWidth;
	bool                       bProgressiveCap = false;/* capture type */
	BAVC_Polarity              ePolarity;/* VFD scanout type */
	BFMT_VideoFmt              eDispVideoFmt;
	bool                       bTestFeatureEnable, bEnableAlignWkr = false;

	BDBG_ENTER(BVDC_P_Feeder_SetPlaybackInfo_isr);
	BDBG_OBJECT_ASSERT(hFeeder, BVDC_FDR);

	/* Common video feeder settings */
	BVDC_P_MFD_GET_REG_DATA(MFD_0_DISP_HSIZE) &= ~(BCHP_MASK(MFD_0_DISP_HSIZE, VALUE));

	if (hFeeder->bGfxSrc)
	{
		BVDC_P_SurfaceInfo  *pCurSur = &hFeeder->stGfxSurface.stCurSurInfo;

		bTestFeatureEnable = false;
		ulHeight = pCurSur->ulHeight;
		ulWidth = pCurSur->ulWidth;

		ePolarity = hFeeder->stPicture.eSrcPolarity;
		bProgressiveCap = (BAVC_Polarity_eFrame == ePolarity);

		BVDC_P_MFD_GET_REG_DATA(MFD_0_DISP_HSIZE) |= (
			BCHP_FIELD_DATA(MFD_0_DISP_HSIZE, VALUE, ulWidth & ~1));
	}
	else
	{
		bTestFeatureEnable = pPicture->stCapTestFeature1.bEnable;
		ulWidth = pPicture->pVfdOut->ulWidth;

		/* need to determine the scanout size based on the picture polarity of src or display */
		bProgressiveCap =
			((BVDC_P_VNET_USED_SCALER_AT_WRITER(pPicture->stVnetMode)
			 ? pPicture->eDstPolarity : pPicture->eSrcPolarity) == BAVC_Polarity_eFrame);

		eDispVideoFmt = pPicture->hBuffer->hWindow->hCompositor->stCurInfo.pFmtInfo->eVideoFmt;
		/* frame capture -> interlaced playback */
		if ((pPicture->eDstPolarity == BAVC_Polarity_eFrame) &&
				(pPicture->eDisplayPolarity != BAVC_Polarity_eFrame)
#if BFMT_LEGACY_3DTV_SUPPORT
			&& (!VIDEO_FORMAT_IS_CUSTOM_1080P3D(eDispVideoFmt))
#endif
			)
		{
			ePolarity = pPicture->eDisplayPolarity;
			/* progressive capture and interlaced playback might have odd frame height value! */
			ulHeight = pPicture->pVfdOut->ulHeight & ~1;
		}
		else
		{
			ePolarity = BAVC_Polarity_eFrame;
#if BFMT_LEGACY_3DTV_SUPPORT
			ulHeight = (bProgressiveCap &&
				(!VIDEO_FORMAT_IS_CUSTOM_1080P3D(eDispVideoFmt) ||
				 !BVDC_P_VNET_USED_SCALER_AT_WRITER(pPicture->stVnetMode)))
				? pPicture->pVfdOut->ulHeight : pPicture->pVfdOut->ulHeight / 2;
#else
			ulHeight = bProgressiveCap ? pPicture->pVfdOut->ulHeight : pPicture->pVfdOut->ulHeight / 2;
#endif
		}

	/* mpeg source playback vfd always scan out the whole captured progressively */
#if BVDC_P_SUPPORT_BVN_444
		if (!BPXL_IS_YCbCr422_FORMAT(pPicture->ePixelFormat))
		{
			BVDC_P_MFD_GET_REG_DATA(MFD_0_DISP_HSIZE) |= (
					BCHP_FIELD_DATA(MFD_0_DISP_HSIZE, VALUE, ulWidth));
		}
		else
#endif
		{
			ulWidth = (ulWidth + 1) & ~1;
#if (BVDC_P_MVFD_ALIGNMENT_WORKAROUND)
			if(ulWidth % BVDC_P_SCB_BURST_SIZE == 2)
			{
				BDBG_MSG(("Feeder[%d] Enable alignment workaround for MVFD for Width %d",
					hFeeder->eId, ulWidth));
				bEnableAlignWkr = true;
			}
#endif

			BVDC_P_MFD_GET_REG_DATA(MFD_0_DISP_HSIZE) |= (
				BCHP_FIELD_DATA(MFD_0_DISP_HSIZE, VALUE, ulWidth));
		}
	}

	BVDC_P_Feeder_SetVertWindow_isr(hFeeder, ePolarity, 0, ulHeight);

	/* Check if need to do fixed color feed */
	if( bFixedColor )
	{
		BVDC_P_Feeder_SetFixedColor_isr(hFeeder, BAVC_Polarity_eFrame,
			ulMuteColorYCrCb);
	}
	else
	{
		/* Gather information */
		BVDC_P_Feeder_GetFormatCtrlSettings_isr(hFeeder, NULL, NULL, pPicture);

		/*
		* Start format change settings
		*/
		BVDC_P_Feeder_SetFormatCtrl_isr(hFeeder, NULL, pPicture, ePolarity);

		/* Set strides and buf start addr */
		BVDC_P_Feeder_SetPlaybackStrideAddr_isr(hFeeder,
			pPicture, bProgressiveCap, bEnableAlignWkr, ePolarity);
	}

#if (BVDC_P_SUPPORT_VIDEO_TESTFEATURE1_CAP)
	if(hFeeder->eId == BVDC_P_FeederId_eVfd0)
	{
		BVDC_P_Feeder_SetTestFeature1_isr(hFeeder, pPicture);
	}
#endif

	/* Enable feeder */
	if(ulWidth)
	{
		BVDC_P_Feeder_SetEnable_isr(hFeeder, true);
	}
	else
	{
		BVDC_P_Feeder_SetEnable_isr(hFeeder, false);
	}

	BDBG_LEAVE(BVDC_P_Feeder_SetPlaybackInfo_isr);
	return;
}

/***************************************************************************
 *
 */
static void BVDC_P_Feeder_SetFixedColor_isr
	( BVDC_P_Feeder_Handle             hFeeder,
	  BAVC_Polarity                    ePolarity,
	  const uint32_t                   ulMuteColorYCrCb )
{
	uint8_t               ucLuma, ucCb, ucCr;

	/* Fixed color feed. Program every format change.*/
	ucLuma = (uint8_t)BPXL_GET_COMPONENT(BPXL_eA8_Y8_Cb8_Cr8, ulMuteColorYCrCb, 2);
	ucCb   = (uint8_t)BPXL_GET_COMPONENT(BPXL_eA8_Y8_Cb8_Cr8, ulMuteColorYCrCb, 1);
	ucCr   = (uint8_t)BPXL_GET_COMPONENT(BPXL_eA8_Y8_Cb8_Cr8, ulMuteColorYCrCb, 0);

	BVDC_P_MFD_GET_REG_DATA(MFD_0_FEEDER_CNTL) &= ~(
#if (BVDC_P_MFD_SUPPORT_3D_VIDEO)
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)
		BCHP_MASK(MFD_0_FEEDER_CNTL, MEM_VIDEO          ) |
#endif
		BCHP_MASK(MFD_0_FEEDER_CNTL, BVB_VIDEO          ) |
#endif
		BCHP_MASK(MFD_0_FEEDER_CNTL, IMAGE_FORMAT       ) |
		BCHP_MASK(MFD_0_FEEDER_CNTL, FIXED_COLOUR_ENABLE) );

	BVDC_P_MFD_GET_REG_DATA(MFD_0_FEEDER_CNTL) |= (
#if (BVDC_P_MFD_SUPPORT_3D_VIDEO)
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)
		BCHP_FIELD_ENUM(MFD_0_FEEDER_CNTL, MEM_VIDEO, MODE_2D     ) |
		BCHP_FIELD_DATA(MFD_0_FEEDER_CNTL, BVB_VIDEO, hFeeder->eOutputOrientation)|
#else
		BCHP_FIELD_ENUM(MFD_0_FEEDER_CNTL, BVB_VIDEO, MODE_2D     ) |
#endif
#endif
		BCHP_FIELD_ENUM(MFD_0_FEEDER_CNTL, IMAGE_FORMAT, PACKED   ) |
		BCHP_FIELD_ENUM(MFD_0_FEEDER_CNTL, FIXED_COLOUR_ENABLE, ON) );

	BVDC_P_MFD_GET_REG_DATA(MFD_0_FIXED_COLOUR) &= ~(
		BCHP_MASK(MFD_0_FIXED_COLOUR, LUMA) |
		BCHP_MASK(MFD_0_FIXED_COLOUR, CB  ) |
		BCHP_MASK(MFD_0_FIXED_COLOUR, CR  ) );

	BVDC_P_MFD_GET_REG_DATA(MFD_0_FIXED_COLOUR) |= (
		BCHP_FIELD_DATA(MFD_0_FIXED_COLOUR, LUMA, ucLuma) |
		BCHP_FIELD_DATA(MFD_0_FIXED_COLOUR, CB,   ucCb  ) |
		BCHP_FIELD_DATA(MFD_0_FIXED_COLOUR, CR,   ucCr  ) );

	BVDC_P_Feeder_SetLacCntlOutput_isr(hFeeder, ePolarity);

#if BVDC_P_MFD_FIXED_COLOR_WORKAROUND
	/* reset feeder in case transition from memory feed to fixed color stalls. */
	hFeeder->bInitial = true;
#endif
	return;
}

/***************************************************************************
 * {private}
 *
 */
static BERR_Code BVDC_P_Feeder_SetVertWindow_isr
	( BVDC_P_Feeder_Handle             hFeeder,
	  const BAVC_Polarity              eSourcePolarity,
	  const uint32_t                   ulTop,
	  const uint32_t                   ulHeight)
{
	uint32_t   ulVertStart, ulVertEnd;

	switch( eSourcePolarity )
	{
	case BAVC_Polarity_eTopField:
		/* Top field interlaced scan out from 0 to (height - 2) */
		ulVertStart  = ulTop;
		ulVertEnd    = ulTop + ulHeight - 2;
		BDBG_ASSERT((ulVertStart & 1) == (ulVertEnd & 1));
		break;

	case BAVC_Polarity_eBotField:
		/* Bottom field interlaced scan out from 1 to (height - 1) */
		ulVertStart  = ulTop + 1;
		ulVertEnd    = ulTop + ulHeight - 1;
		BDBG_ASSERT((ulVertStart & 1) == (ulVertEnd & 1));
		break;

	case BAVC_Polarity_eFrame:
		/* Progressive scan out from 0 to (height - 1) */
		ulVertStart  = ulTop;
		ulVertEnd    = ulTop + ulHeight - 1;
		break;

	default:
		/* Top field interlaced scan out from 0 to (height - 2) */
		ulVertStart  = ulTop;
		ulVertEnd    = ulTop + ulHeight - 2;
		break;
	}

	BDBG_MSG(("Feeder %d ePolarity = %d", hFeeder->eId, eSourcePolarity));
#if (BVDC_P_SUPPORT_MFD_VER < BVDC_P_MFD_VER_10)
	BDBG_MSG(("Scanout VERT_WIN_START = %d", ulVertStart));
	BDBG_MSG(("Scanout VERT_WIN_END   = %d", ulVertEnd));
	BVDC_P_MFD_GET_REG_DATA(MFD_0_PICTURE0_DISP_VERT_WINDOW) &= ~(
		BCHP_MASK(MFD_0_PICTURE0_DISP_VERT_WINDOW, START) |
		BCHP_MASK(MFD_0_PICTURE0_DISP_VERT_WINDOW, END  ) );

	BVDC_P_MFD_GET_REG_DATA(MFD_0_PICTURE0_DISP_VERT_WINDOW) |= (
		BCHP_FIELD_DATA(MFD_0_PICTURE0_DISP_VERT_WINDOW, START, ulVertStart) |
		BCHP_FIELD_DATA(MFD_0_PICTURE0_DISP_VERT_WINDOW, END,   ulVertEnd  ) );
#else
	BDBG_MSG(("Scanout MFD_0_DISP_VSIZE = %d",
		(BAVC_Polarity_eFrame==eSourcePolarity)
		? (ulHeight - ulTop) : ((ulHeight - ulTop)/2)));
	BVDC_P_MFD_GET_REG_DATA(MFD_0_DISP_VSIZE) &= ~(
		BCHP_MASK(MFD_0_DISP_VSIZE, VALUE));

	BVDC_P_MFD_GET_REG_DATA(MFD_0_DISP_VSIZE) |= (
		BCHP_FIELD_DATA(MFD_0_DISP_VSIZE, VALUE, (BAVC_Polarity_eFrame==eSourcePolarity)
			? (ulHeight - ulTop) : ((ulHeight - ulTop)/2)) );
#endif

	return BERR_SUCCESS;
}


/***************************************************************************
 * {private}
 *
 */
BERR_Code BVDC_P_Feeder_SetEnable_isr
	( BVDC_P_Feeder_Handle             hFeeder,
	  bool                             bEnable )
{
	/* Turn on/off the feeder. */
	BVDC_P_MFD_GET_REG_DATA(MFD_0_PIC_FEED_CMD) &= ~(
		BCHP_MASK(MFD_0_PIC_FEED_CMD, START_FEED) );

	BVDC_P_MFD_GET_REG_DATA(MFD_0_PIC_FEED_CMD) |= (
		BCHP_FIELD_DATA(MFD_0_PIC_FEED_CMD, START_FEED, bEnable) );

	return BERR_SUCCESS;
}

/***************************************************************************
 * {private}
 *
 */
void BVDC_P_Feeder_GetCrc_isr
	( BVDC_P_Feeder_Handle             hFeeder,
	  BREG_Handle                      hReg,
	  BVDC_Source_CallbackData        *pData )
{
	BDBG_OBJECT_ASSERT(hFeeder, BVDC_FDR);
	BDBG_ASSERT(hFeeder->eId < BVDC_P_FeederId_eVfd0);

	/* Fetch the new CRC values */
#if (BVDC_P_MFD_NEED_CRC_WORKAROUND)
	pData->ulLumaCrc   = BREG_Read32_isr(hReg, hFeeder->ulLumaCrcRegAddr);
	pData->ulChromaCrc = BREG_Read32_isr(hReg, hFeeder->ulChromaCrcRegAddr);

	pData->ulLumaCrcR   = BVDC_P_MFD_INVALID_CRC;
	pData->ulChromaCrcR = BVDC_P_MFD_INVALID_CRC;
#else
	pData->ulLumaCrc   = BREG_Read32_isr(hReg, BCHP_MFD_0_LUMA_CRC + hFeeder->ulRegOffset);
	pData->ulChromaCrc = BREG_Read32_isr(hReg, BCHP_MFD_0_CHROMA_CRC + hFeeder->ulRegOffset);

#if (BVDC_P_MFD_SUPPORT_CRC_R)
	pData->ulLumaCrcR   = BREG_Read32_isr(hReg, BCHP_MFD_0_LUMA_CRC_R + hFeeder->ulRegOffset);
	pData->ulChromaCrcR = BREG_Read32_isr(hReg, BCHP_MFD_0_LUMA_CRC_R + hFeeder->ulRegOffset);
#else
	pData->ulLumaCrcR   = BVDC_P_MFD_INVALID_CRC;
	pData->ulChromaCrcR = BVDC_P_MFD_INVALID_CRC;
#endif

#endif
	pData->stDirty.bCrcValue = BVDC_P_DIRTY;

	return;
}

#if (BVDC_P_MFD_SUPPORT_CSC)
/***************************************************************************
 *
 */
static void BVDC_P_Feeder_SetCsc_isr
	( BVDC_P_Feeder_Handle             hFeeder,
	  const BAVC_MVD_Field            *pFieldData )
{
	BVDC_P_CscCoeffs stCscCoeffs;

	BDBG_OBJECT_ASSERT(hFeeder->hSource, BVDC_SRC);

#if (BVDC_P_SUPPORT_CMP_MOSAIC_CSC == 0)
	if(hFeeder->hSource->stCurInfo.bMosaicMode)
	{
		BVDC_P_CscCoeffs *pCscCoeffs;
		/* Correct way is to convert individual input color space from
		 * pFieldData->eMatrixCoefficients to tracked channel color space
		 * defined by hFeeder->hSource->eMatrixCoefficients.
		 * BVDC_P_Compositor_GetCscTable_isr(&stCscCoeffs, false,
		 * pFieldData->eMatrixCoefficients, hFeeder->hSource->eMatrixCoefficients);
		 *
		 * Since we don't have all the matrices yet, just convert all to HD.
		 * hFeeder->hSource->eMatrixCoefficients is hard coded to
		 * BAVC_MatrixCoefficients_eItu_R_BT_709 in
		 * BVDC_P_Window_UpdateSrcAndUserInfo_isr. */
		BVDC_P_Compositor_GetCscTable_isr(&pCscCoeffs, false,
			pFieldData->eMatrixCoefficients, VIDEO_FORMAT_CMP_COLORSPACE(BFMT_VideoFmt_e1080i, false), false);

		stCscCoeffs = *pCscCoeffs;
	}
	else
#endif
	{
		/* Use s_Identity */
		BVDC_P_Csc_GetHdDviTable_isr(&stCscCoeffs, BAVC_CscMode_e709YCbCr);
	}

	/* Turn on CSC. */
	BVDC_P_MFD_GET_REG_DATA(MFD_0_CSC_CNTL) &= ~(
		BCHP_MASK(MFD_0_CSC_CNTL, CSC_ENABLE) );

	BVDC_P_MFD_GET_REG_DATA(MFD_0_CSC_CNTL) |= (
		BCHP_FIELD_ENUM(MFD_0_CSC_CNTL, CSC_ENABLE, ON) );

#if (BVDC_P_MFD_SUPPORT_NEW_CSC)
	/* [ c00, c01 c02 c03 ] */
	BVDC_P_MFD_SET_REG_DATA(MFD_0_CSC_COEFF_C00, \
		BCHP_FIELD_DATA(MFD_0_CSC_COEFF_C00, COEFF_MUL, stCscCoeffs.usY0));
	BVDC_P_MFD_SET_REG_DATA(MFD_0_CSC_COEFF_C01, \
		BCHP_FIELD_DATA(MFD_0_CSC_COEFF_C01, COEFF_MUL, stCscCoeffs.usY1));
	BVDC_P_MFD_SET_REG_DATA(MFD_0_CSC_COEFF_C02, \
		BCHP_FIELD_DATA(MFD_0_CSC_COEFF_C02, COEFF_MUL, stCscCoeffs.usY2));
	BVDC_P_MFD_SET_REG_DATA(MFD_0_CSC_COEFF_C03, \
		BCHP_FIELD_DATA(MFD_0_CSC_COEFF_C03, COEFF_ADD, stCscCoeffs.usYOffset));

	/* [ c10, c11 c12 c13 ] */
	BVDC_P_MFD_SET_REG_DATA(MFD_0_CSC_COEFF_C10, \
		BCHP_FIELD_DATA(MFD_0_CSC_COEFF_C10, COEFF_MUL, stCscCoeffs.usCb0));
	BVDC_P_MFD_SET_REG_DATA(MFD_0_CSC_COEFF_C11, \
		BCHP_FIELD_DATA(MFD_0_CSC_COEFF_C11, COEFF_MUL, stCscCoeffs.usCb1));
	BVDC_P_MFD_SET_REG_DATA(MFD_0_CSC_COEFF_C12, \
		BCHP_FIELD_DATA(MFD_0_CSC_COEFF_C12, COEFF_MUL, stCscCoeffs.usCb2));
	BVDC_P_MFD_SET_REG_DATA(MFD_0_CSC_COEFF_C13, \
		BCHP_FIELD_DATA(MFD_0_CSC_COEFF_C13, COEFF_ADD, stCscCoeffs.usCbOffset));

	/* [ c20, c21 c22 c23 ] */
	BVDC_P_MFD_SET_REG_DATA(MFD_0_CSC_COEFF_C20, \
		BCHP_FIELD_DATA(MFD_0_CSC_COEFF_C20, COEFF_MUL, stCscCoeffs.usCr0));
	BVDC_P_MFD_SET_REG_DATA(MFD_0_CSC_COEFF_C21, \
		BCHP_FIELD_DATA(MFD_0_CSC_COEFF_C21, COEFF_MUL, stCscCoeffs.usCr1));
	BVDC_P_MFD_SET_REG_DATA(MFD_0_CSC_COEFF_C22, \
		BCHP_FIELD_DATA(MFD_0_CSC_COEFF_C22, COEFF_MUL, stCscCoeffs.usCr2));
	BVDC_P_MFD_SET_REG_DATA(MFD_0_CSC_COEFF_C23, \
		BCHP_FIELD_DATA(MFD_0_CSC_COEFF_C23, COEFF_ADD, stCscCoeffs.usCrOffset));

#else
	/* [ c00, c01 c02 c03 ] */
	BVDC_P_MFD_SET_REG_DATA(MFD_0_CSC_COEFF_C01_C00, \
		BCHP_FIELD_DATA(MFD_0_CSC_COEFF_C01_C00, COEFF_C0, stCscCoeffs.usY0) | \
		BCHP_FIELD_DATA(MFD_0_CSC_COEFF_C01_C00, COEFF_C1, stCscCoeffs.usY1));
	BVDC_P_MFD_SET_REG_DATA(MFD_0_CSC_COEFF_C03_C02, \
		BCHP_FIELD_DATA(MFD_0_CSC_COEFF_C03_C02, COEFF_C2, stCscCoeffs.usY2) | \
		BCHP_FIELD_DATA(MFD_0_CSC_COEFF_C03_C02, COEFF_C3, stCscCoeffs.usYOffset));

	/* [ c10, c11 c12 c13 ] */
	BVDC_P_MFD_SET_REG_DATA(MFD_0_CSC_COEFF_C11_C10, \
		BCHP_FIELD_DATA(MFD_0_CSC_COEFF_C11_C10, COEFF_C0, stCscCoeffs.usCb0) | \
		BCHP_FIELD_DATA(MFD_0_CSC_COEFF_C11_C10, COEFF_C1, stCscCoeffs.usCb1));
	BVDC_P_MFD_SET_REG_DATA(MFD_0_CSC_COEFF_C13_C12, \
		BCHP_FIELD_DATA(MFD_0_CSC_COEFF_C13_C12, COEFF_C2, stCscCoeffs.usCb2) | \
		BCHP_FIELD_DATA(MFD_0_CSC_COEFF_C13_C12, COEFF_C3, stCscCoeffs.usCbOffset));

	/* [ c20, c21 c22 c23 ] */
	BVDC_P_MFD_SET_REG_DATA(MFD_0_CSC_COEFF_C21_C20, \
		BCHP_FIELD_DATA(MFD_0_CSC_COEFF_C21_C20, COEFF_C0, stCscCoeffs.usCr0) | \
		BCHP_FIELD_DATA(MFD_0_CSC_COEFF_C21_C20, COEFF_C1, stCscCoeffs.usCr1));
	BVDC_P_MFD_SET_REG_DATA(MFD_0_CSC_COEFF_C23_C22, \
		BCHP_FIELD_DATA(MFD_0_CSC_COEFF_C23_C22, COEFF_C2, stCscCoeffs.usCr2) | \
		BCHP_FIELD_DATA(MFD_0_CSC_COEFF_C23_C22, COEFF_C3, stCscCoeffs.usCrOffset));
#endif

	return;
}

#endif


/*****************************************************************************
 * Functions specially for VFD suuport on Gfx surface
 */

/*------------------------------------------------------------------------
 * {private}
 * BVDC_P_Feeder_ValidateSurAndRects_isr
 *
 * Called for isr set surface validation during RUL build
 */
#define BVDC_P_Feeder_ValidateGfxSurAndRects_isr  \
	    BVDC_P_Feeder_ValidateGfxSurAndRects

/*------------------------------------------------------------------------
 * {private}
 * BVDC_P_Feeder_ValidateGfxSurAndRects
 *
 * It validates the combination of clip, scaler-out and dest rectangles
 * and surface size.
 *
 * It could be called by BVDC_P_Feeder_ValidateChanges in user mode,
 * or during RUL build for isr surface.
 */
BERR_Code BVDC_P_Feeder_ValidateGfxSurAndRects
	( BVDC_P_SurfaceInfo                *pSur,
	  const BVDC_P_ClipRect             *pClipRect,
	  const BVDC_P_Rect                 *pSclOutRect )
{
	uint32_t  ulCntWidth, ulOutWidth, ulCntHeight, ulOutHeight;

	/* Note that the combination of scale-out with dst rect and canvas is checked
	 * by BVDC_P_Window_ValidateChanges.
	 */

	/* check the combination of the surface size with the clip rectange */
	if ( ((pClipRect->ulLeft + pClipRect->ulRight)  >= pSur->ulWidth) ||
		 ((pClipRect->ulTop  + pClipRect->ulBottom) >= pSur->ulHeight) )
	{
		return BERR_TRACE(BVDC_ERR_ILLEGAL_CLIPPING_VALUES);
	}

	/* check the combination of clip rectange and scale-out rectangle */
	ulCntWidth  = (pSur->ulWidth  - (pClipRect->ulLeft + pClipRect->ulRight));
	ulCntHeight = (pSur->ulHeight - (pClipRect->ulTop  + pClipRect->ulBottom));
	ulOutWidth  = pSclOutRect->ulWidth;
	ulOutHeight = pSclOutRect->ulHeight;

	if ( (0 == ulCntWidth  && 0 != ulOutWidth) || /* inifinite scaling! */
		 (0 == ulCntHeight && 0 != ulOutHeight) )
	{
		return BERR_TRACE(BVDC_ERR_ILLEGAL_CLIPPING_VALUES);
	}

	return BERR_SUCCESS;
}

/***************************************************************************
 * Validate the settings in pNewInfo set by user!
 *
 */
BERR_Code BVDC_P_Feeder_ValidateChanges
	( BVDC_P_Feeder_Handle               hFeeder,
	  BVDC_Source_PictureCallback_isr    pfPicCallbackFunc )
{
	BERR_Code eResult = BERR_SUCCESS;
	BVDC_P_SurfaceInfo  *pNewSur;
	const BVDC_P_ClipRect  *pNewClip;
	const BVDC_P_Rect  *pNewDst, *pNewSclOut;

	BDBG_ENTER(BVDC_P_Feeder_ValidateChanges);

	/* whether user set new surface, isr set surface, or current surface
	 * will be used to be validated against configure ? */
	if ( true == hFeeder->stGfxSurface.stNewSurInfo.bChangeSurface )
	{
		pNewSur = &(hFeeder->stGfxSurface.stNewSurInfo);
	}
	else if ( true == hFeeder->stGfxSurface.stIsrSurInfo.bChangeSurface )
	{
		pNewSur = &(hFeeder->stGfxSurface.stIsrSurInfo);
	}
	else /* no new user or isr set to surface, contunue to use cur surface */
	{
		pNewSur = &(hFeeder->stGfxSurface.stCurSurInfo);
	}
	hFeeder->pNewSur = pNewSur;

	/* don't validate if no surface is set yet */
	if (hFeeder->hWindow && pNewSur->ulAddress)
	{
		/* validates the combination of clip, scaler-out and dest rectangles
		 * and surface size. */
		BVDC_P_Window_GetNewRectangles( hFeeder->hWindow, &pNewClip, &pNewSclOut, &pNewDst );
		eResult = BVDC_P_Feeder_ValidateGfxSurAndRects(pNewSur, pNewClip, pNewSclOut);
		if (NULL != pfPicCallbackFunc)
		{
			/* allow pfPicCallbackFunc to provide gfx surface later */
			eResult = BERR_SUCCESS;
		}
	}

	BDBG_LEAVE(BVDC_P_Feeder_ValidateChanges);
	return BERR_TRACE(eResult);
}

/*--------------------------------------------------------------------------
 * {private}
 * BVDC_P_Feeder_ApplyChanges_isr
 *
 * To be called by BVDC_ApplyChanges, to copy "new user state" to "current
 * state", after validation of all VDC modules passed.
 */
BERR_Code BVDC_P_Feeder_ApplyChanges_isr
	( BVDC_P_Feeder_Handle               hFeeder )
{
	BVDC_P_SurfaceInfo  *pNewSur, *pCurSur;

	BDBG_ENTER(BVDC_P_Feeder_ApplyChanges_isr);

	/* validate paramters */
	BDBG_OBJECT_ASSERT(hFeeder, BVDC_FDR);
	BDBG_OBJECT_ASSERT(hFeeder->hSource, BVDC_SRC);

	if (hFeeder->hWindow)
	{
		pCurSur = &(hFeeder->stGfxSurface.stCurSurInfo);
		pNewSur = hFeeder->pNewSur; /* decided by ValidateChanges */

		/* Copy NewSur to curSur if bChangeSurface.
		 * Note: pNewSur could be _isr set Surface,
		 * Note: pNewSur could be NULL before a window is connected to the src */
		if ( NULL != pNewSur && pNewSur->bChangeSurface && pNewSur != pCurSur)
		{
			/* pCurSur->bChangeSurface would stay true so that the surface change
			 * is built into RUL later */
			*pCurSur = *pNewSur;

			/* stGfxSurface.stCurSurInfo.bChangeSurface will be set to true iff
			 * there is format / size / pitch change */
			hFeeder->hSource->bPictureChanged = true;

			/* to avoid future validation if surface no longer changes */
			pNewSur->bChangeSurface  = false;

			/* any previous set IsrSur should no longer be used */
			hFeeder->stGfxSurface.stIsrSurInfo.bChangeSurface = false;
		}

		hFeeder->hSource->bStartFeed = true;
		hFeeder->hSource->bPsfScanout = false;
		hFeeder->hSource->stCurInfo.pFmtInfo =
			hFeeder->hWindow->hCompositor->hDisplay->stCurInfo.pFmtInfo;
	}

	BDBG_LEAVE(BVDC_P_Feeder_ApplyChanges_isr);
	return BERR_SUCCESS;
}

/*************************************************************************
 * {private}
 * BVDC_P_Feeder_AbortChanges
 *
 * Cancel the user settings that either fail to validate or simply
 * because user wish to abort the changes in mid-way.
 *************************************************************************/
void BVDC_P_Feeder_AbortChanges
	( BVDC_P_Feeder_Handle               hFeeder )
{
	BDBG_ENTER(BVDC_P_Feeder_AbortChanges);

	/* validate paramters */
	BDBG_OBJECT_ASSERT(hFeeder, BVDC_FDR);

	/* any user set surface should not be used again */
	hFeeder->stGfxSurface.stNewSurInfo.bChangeSurface = false;

	BDBG_LEAVE(BVDC_P_Feeder_AbortChanges);
	return;
}

/*------------------------------------------------------------------------
 *  {secret}
 *	BVDC_P_Feeder_HandleIsrGfxSurface_isr
 *
 *  No paramter error check is performed. Designed to be called by
 *  BVDC_P_Feeder_BuildRul_isr only
 *
 *  It pulls a new isr surface if callback func is installed; then activates
 *  any new set surace (by Source_SetSurface_isr or this callback func). It
 *  at first validates the combination of this surface and current confugure,
 *  and set up to build RUL for this combination.
 */
void BVDC_P_Feeder_HandleIsrGfxSurface_isr
	( BVDC_P_Feeder_Handle         hFeeder,
  	  BVDC_P_Source_Info *         pCurSrcInfo,
	  BAVC_Polarity                eFieldId )
{
	void  *pvGfxPic;
	const BVDC_P_ClipRect  *pCurClip;
	const BVDC_P_Rect  *pCurDst, *pCurSclOut;
	BVDC_P_SurfaceInfo  *pIsrSur = &(hFeeder->stGfxSurface.stIsrSurInfo);
	BERR_Code  eResult;

	if( NULL != pCurSrcInfo->pfPicCallbackFunc )
	{
		pCurSrcInfo->pfPicCallbackFunc(
			pCurSrcInfo->pvParm1, pCurSrcInfo->iParm2,
			eFieldId, BAVC_SourceState_eActive, &pvGfxPic);
		if ( NULL != pvGfxPic )
		{
			BVDC_P_GfxSurface_SetSurface_isr(&hFeeder->stGfxSurface,
				pIsrSur, (BAVC_Gfx_Picture *)pvGfxPic);

			/* stGfxSurface.stCurSurInfo.bChangeSurface will be set to true iff
			 * there is format / size / pitch change */
			hFeeder->hSource->bPictureChanged =
				hFeeder->stGfxSurface.stCurSurInfo.bChangeSurface;
		}
	}

	/* after previous vsync, pIsrSur->bChangeSurface might be set by
	 * Source_SetSurface_isr, by the above callback function, or not re-set
	 * at all. If it was re-set, and there was no change in format, size, or
	 * pitch, then it has already been sent to HW in GfxSurface_SetSurface_isr,
	 * and pIsrSur->bChangeSurface has been marked as false */
	if (pIsrSur->bChangeSurface && hFeeder->hWindow)
	{
		BVDC_P_Window_GetCurrentRectangles(
			hFeeder->hWindow, &pCurClip, &pCurSclOut, &pCurDst);

		/* validate surface size and rectangles, adjust rectangles and do some
		 * scale related computation. The intermediate values are stored in
		 * CurCfg, and will affect RUL build immediately. */
		eResult = BVDC_P_Feeder_ValidateGfxSurAndRects_isr(
			pIsrSur, pCurClip, pCurSclOut);
		if (BERR_SUCCESS == eResult)
		{
			/* Copy IsrSur to curSur
			 * note: pCurSur->bChangeSurface would stay true so that the surface
			 * change is built into RUL later */
			hFeeder->stGfxSurface.stCurSurInfo = *pIsrSur;

			/* to avoid future validation if isr surface no longer changes */
			pIsrSur->bChangeSurface  = false;
		}
		/* otherwise we stay with original current surface and ignore this
		 * IsrSur until either Cfg or surface changes
		 * note: ValidateSurAndRects will NOT change CurCfg until all error
		 * check are completed */
	}
}

/* End of file. */
