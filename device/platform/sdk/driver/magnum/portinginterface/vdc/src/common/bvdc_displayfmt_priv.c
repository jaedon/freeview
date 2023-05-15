/***************************************************************************
 *
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvdc_displayfmt_priv.c $
 * $brcm_Revision: Hydra_Software_Devel/330 $
 * $brcm_Date: 10/15/12 4:01p $
 *
 * Module Description:
 *   Contains tables for Display settings.
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vdc/src/common/bvdc_displayfmt_priv.c $
 * 
 * Hydra_Software_Devel/330   10/15/12 4:01p tdo
 * SW7425-977: refactor display format info usage in HDMI path programming
 * for 4kx2k format support
 *
 * Hydra_Software_Devel/329   10/12/12 11:38a darnstein
 * SW7552-320: integrate new microcode for format 1080I@50Hz. Checksum
 * 0x343eff3c.
 * 
 * Hydra_Software_Devel/328   10/11/12 3:24p albertl
 * SW7425-4019: Turned C1 and C2 offsets back on with offset of 0x10 to
 * eliminate video artifacts.
 * 
 * Hydra_Software_Devel/327   9/4/12 4:51p darnstein
 * SW7346-909: tables of IT block register values grow by one entry. THe
 * tables that previously covered ADDR_0_3 to PCL_8 now end with
 * STACK_REG_8_9.
 *
 * Hydra_Software_Devel/326   8/21/12 4:46p darnstein
 * SW7435-276: Consistently support DCS for the new video format
 * BFMT_VideoFmt_e720x482_NTSC_J.
 *
 * Hydra_Software_Devel/325   8/15/12 6:22p tdo
 * SW7445-8: Create Appframework emulation environment for 7445 A0
 *
 * Hydra_Software_Devel/324   8/10/12 3:27p pntruong
 * SW7435-276: Updated NTSC/480p to 720x480i/720x480p respectively.
 * Added support 482i/483p with new enums.
 *
 * Hydra_Software_Devel/323   7/5/12 12:32p tdo
 * SW7425-975: Need new microcode for high pixel rate formats
 *
 * Hydra_Software_Devel/322   7/5/12 11:25a vanessah
 * SW7405-5488: solve the custome format segment fault error
 *
 * Hydra_Software_Devel/321   6/27/12 3:59p darnstein
 * SW7335-1422: Install updated analog VEC IT microcode for 480P, 27MHz
 * sampling, no ARIB.
 *
 * Hydra_Software_Devel/320   6/25/12 1:42p pntruong
 * SW7405-5488: Added support for querying display capabilties of vdc
 * supportable format.
 *
 * Hydra_Software_Devel/319   6/8/12 4:02p albertl
 * SW7358-323: Turned off C1 and C2 offsets for RGB formats.
 *
 * Hydra_Software_Devel/318   4/17/12 7:06p darnstein
 * SW7405-3004: Integrate new ARIB 480P microcode from Nutan. Fix up
 * microcode selection logic.
 *
 * Hydra_Software_Devel/317   2/20/12 1:13p darnstein
 * SW7125-1234: previous check-in broke compilation for non-orthogonal VEC
 * chipsets.
 *
 * Hydra_Software_Devel/316   2/17/12 5:53p darnstein
 * SW7125-1234: PAL-M and PAL-N cannot be supported for DCS at this time.
 * This situation was not handled cleanly in software.
 *
 * Hydra_Software_Devel/315   2/13/12 4:20p darnstein
 * SW7358-227: still not programming VF_n.NEG_SYNC_AMPLITUDE_EXTN
 * correctly.
 *
 * Hydra_Software_Devel/314   1/31/12 10:41a darnstein
 * SW7125-1124: DCS: adjust INIT_PHASE for NTSC subcarrier.
 *
 * Hydra_Software_Devel/313   12/1/11 3:10p darnstein
 * SW7346-579: correct outright errors in setting negative sync extension
 * register bitfield in VF_n blocks.
 *
 * Hydra_Software_Devel/312   11/10/11 6:46p darnstein
 * SW7344-101: Program IT registers PCL_5 through PCL_8.
 *
 * Hydra_Software_Devel/311   11/9/11 3:06p darnstein
 * SW7425-1658: finish programming VF NEG_SYNC amplitude (extension,
 * VALUE2).
 *
 * Hydra_Software_Devel/310   11/4/11 6:11p albertl
 * SW7425-1677: Changed YIQ and YUV to use 1.3 Mhz filter for chroma.
 *
 * Hydra_Software_Devel/309   10/7/11 4:09p yuxiaz
 * SW7425-974: Added new high pixel rate formats.
 *
 * Hydra_Software_Devel/308   9/30/11 5:14p darnstein
 * SW7400-2665: merge from JIRA branch.
 *
 * Hydra_Software_Devel/SW7400-2665/2   9/30/11 2:13p albertl
 * SW7400-2665:  Fixed VF settings for 576p 54Mhz.
 *
 * Hydra_Software_Devel/SW7400-2665/1   9/28/11 7:25p darnstein
 * SW7400-2665: Implement double rate 576P analog output for orthogonal
 * VEC chipsets.
 *
 * Hydra_Software_Devel/307   8/24/11 10:54a tdo
 * SW7425-610: Add RM table for VESA 1280x1024 format
 *
 * Hydra_Software_Devel/306   7/18/11 5:32p pntruong
 * SW7231-284, SW7346-295, SW7405-5339: Refactored master framerate
 * tracking, so that it can also handle validation.
 *
 * Hydra_Software_Devel/305   7/18/11 5:03p albertl
 * SW7552-44: Fixed 576p filter to use 1024 sum of taps.
 *
 * Hydra_Software_Devel/304   7/18/11 4:17p albertl
 * SW7425-169: Fixed filter typo.
 *
 * Hydra_Software_Devel/303   6/24/11 7:09p albertl
 * SW7405-5081: Restore bad merge.
 *
 * Hydra_Software_Devel/301   6/24/11 12:53p darnstein
 * SW7405-5081: Install new microcode for ARIB style 480I output.
 *
 * Hydra_Software_Devel/300   6/9/11 1:15p tdo
 * SW7405-5339: Correct the order of some DVI FMT in format data table to
 * match the definition in bfmt.h
 *
 * Hydra_Software_Devel/299   6/3/11 3:41p darnstein
 * SW7325-848: replaced three analog VEC IT microcode arrays for 480P
 * output.
 *
 * Hydra_Software_Devel/298   6/2/11 10:22a tdo
 * SW7422-124: Add MC to support 1080p 3D formats for HDMI slave mode
 *
 * Hydra_Software_Devel/297   5/24/11 7:31p albertl
 * SW7425-169: Converted 10-bit and 8-bit filters to compact hex format
 * and updated to use latest recommended filter coefficients.
 *
 * Hydra_Software_Devel/296   4/15/11 8:08p albertl
 * SW7425-169: Updated HD chroma filters to 10-bit.
 *
 * Hydra_Software_Devel/295   3/24/11 1:14p tdo
 * SW7422-124: New microcde for 3D formats supporting HDMI slave and
 * master mode
 *
 * Hydra_Software_Devel/294   3/21/11 11:42a tdo
 * SW7422-391: Force ED to use Drop Rate table
 *
 * Hydra_Software_Devel/293   3/17/11 6:10p tdo
 * SW7422-391: Incorrect VertRefreshRate return and add support for PAL 60
 * to allow multi rate
 *
 * Hydra_Software_Devel/292   2/23/11 5:58p vanessah
 * SW7422-280: change Rate manuager number according to
 * http://www.sj.broadcom.com/projects/rdb/snapshot/rdb/bcm7340_b0/curren
 * t/webs/RM_0.html#RM_0_SAMPLE_INC
 *
 * Hydra_Software_Devel/291   2/22/11 5:07p darnstein
 * SW3548-3134: Comment only. Need support in BFMT.
 *
 * Hydra_Software_Devel/290   2/7/11 2:03p darnstein
 * SW7405-5081: Implement ARIB option to NTSC.
 *
 * Hydra_Software_Devel/289   1/20/11 2:41p yuxiaz
 * SW7422-97: Clean up macros for HDMI Rate manage
 *
 * Hydra_Software_Devel/288   1/17/11 4:56p yuxiaz
 * SW7422-97: Added KP, KI and KA settings on 7422.
 *
 * Hydra_Software_Devel/287   12/21/10 8:17a tdo
 * SW7422-124: update new microcode for 3D fmts
 *
 * Hydra_Software_Devel/286   12/14/10 4:36p tdo
 * SW7422-124: Fix GetRmString function to return the correct pxl clk rate
 * string
 *
 * Hydra_Software_Devel/285   12/9/10 2:16p pntruong
 * SW35230-2250: Merged to mainline.
 *
 * Hydra_Software_Devel/284   12/8/10 12:23p tdo
 * SW7422-124: Add new microcode for 3D fmts
 *
 * Hydra_Software_Devel/283   12/1/10 6:10p tdo
 * SW7422-44: Handle 3D changes for display
 *
 * Hydra_Software_Devel/282   11/24/10 1:36p yuxiaz
 * SW7422-97: Added HDMI TX rate manager settings for VESA formats on 40nm
 * chips.
 *
 * Hydra_Software_Devel/281   11/17/10 5:19p yuxiaz
 * SW7422-97: More HDMI TX rate manager settings for 40nm chips.
 *
 * Hydra_Software_Devel/280   11/16/10 4:07p yuxiaz
 * SW7422-97: Updated basic HDMI TX rate manager settings for 40nm chips.
 *
 * Hydra_Software_Devel/279   10/20/10 3:25p darnstein
 * SW7340-193: add a final comma to an array. This is to help with an
 * automatic verification script.
 *
 * Hydra_Software_Devel/278   10/15/10 7:01p albertl
 * SW7420-823: Updated 6.75Mhz and 1.35Mhz filters.  Changed YQI to use
 * 1.35Mhz for both chroma channels.
 *
 * Hydra_Software_Devel/277   10/14/10 5:12p darnstein
 * SW7340-193: integrate many VEC IT microcode arrays.
 *
 * Hydra_Software_Devel/275   9/21/10 2:27p albertl
 * SW7420-823: Fixed wrong value for 6.75Mhz filter.
 *
 * Hydra_Software_Devel/274   9/20/10 4:44p albertl
 * SW7420-823: Updated 6.75 Mhz filter.
 *
 * Hydra_Software_Devel/273   9/14/10 3:43p yuxiaz
 * SW7422-28: Use new 3D formats
 *
 * Hydra_Software_Devel/272   8/27/10 6:57p albertl
 * SW7420-885: Updated sync transition settings to improve sync variation.
 *
 * Hydra_Software_Devel/271   7/19/10 6:09p albertl
 * SW7400-2620: Fixed 480p and 576p 27Mhz sync trans settings.
 *
 * Hydra_Software_Devel/270   7/15/10 3:20p albertl
 * SW7550-474, SW7420-542: Fixed build.
 *
 * Hydra_Software_Devel/269   7/15/10 3:01p albertl
 * SW7550-474, SW7420-542: Fixed warnings and build errors.
 *
 * Hydra_Software_Devel/268   7/14/10 8:14p albertl
 * SW7550-474, SW7420-542:  Rewrote some table handling code and corrected
 * VF settings for 54Mhz 480p in RGB.
 *
 * Hydra_Software_Devel/267   7/13/10 11:28a yuxiaz
 * SW3548-2987: Added new 1080p@30hz support for 3DTV.
 *
 * Hydra_Software_Devel/266   7/6/10 4:07p darnstein
 * SW7550-189: implement rules cited in JIRA for values to DVI_DTG toggle
 * bits.
 *
 * Hydra_Software_Devel/265   6/28/10 2:27p darnstein
 * SW7335-696: Work-around for 7335 video shift issue.
 *
 * Hydra_Software_Devel/264   6/24/10 3:44p darnstein
 * SW7325-389: New microcode for SECAM/WSE fix needs new microcontroller
 * start addresses.
 *
 * Hydra_Software_Devel/263   6/24/10 3:07p yuxiaz
 * SW7405-4549, SW7420-543:  No Audio or video on HDMI outputs for 480p60.
 *
 * Hydra_Software_Devel/262   6/23/10 5:58p albertl
 * SW7420-823: Updated low bandwidth filters to match old filters and
 * converted to SUM 1024 format.
 *
 * Hydra_Software_Devel/261   6/23/10 4:26p darnstein
 * SW7325-389: extend software work-around for WSE problem to SECAM video
 * output.
 *
 * Hydra_Software_Devel/259   6/18/10 7:44a tdo
 * SW7400-2770: Add multi-frame rate support to SD also
 *
 * Hydra_Software_Devel/258   6/16/10 6:33p tdo
 * SW7400-2770: Add multi-frame rate support to SD also
 *
 * Hydra_Software_Devel/257   5/21/10 4:09p darnstein
 * SW7125-326: integrate latest DTRAM microcode for 480I. Two arrays.
 *
 * Hydra_Software_Devel/256   5/19/10 4:54p darnstein
 * SW7125-326: restore changes from version Hydra_Software_Devel/253
 *
 * Hydra_Software_Devel/255   5/19/10 4:27p darnstein
 * SW7125-326: install new DTRAM microcode for 576I and 576P video
 * formats.
 *
 * Hydra_Software_Devel/254   5/19/10 11:35a pntruong
 * SW7400-2769: To keep ntsc(cvbs/svideo) as 59.94hz and 480i(component)
 * can be 59.94hz or 60.00hz.
 *
 * Hydra_Software_Devel/253   5/18/10 5:30p darnstein
 * SW7125-326: install new DTRAM microcode for 720P formats (except for
 * 60Hz refresh rate). Update choice of DVI_DTG "toggles" bits per Nutan.
 *
 * Hydra_Software_Devel/252   5/11/10 3:19p darnstein
 * SW7340-174: DVI_DTG_0_DTG_CONFIG.AUTO_RESTART only depends on
 * DVI_DTG_0_DTG_CONFIG.SLAVE_MODE now.
 *
 * Hydra_Software_Devel/251   4/21/10 6:59p darnstein
 * SW7420-695: fix a few more DVI video formats. HDMI rate manager
 * settings were wrong.
 *
 * Hydra_Software_Devel/250   4/21/10 11:00a darnstein
 * SW7420-695: Previous check-in only fixed 59.94 Hz refresh rate. This
 * check-in also fixes 60.00 Hz refresh rate.
 *
 * Hydra_Software_Devel/249   4/20/10 5:47p darnstein
 * SW7420-695: fix HDMI rate manager table entry for this single case:
 * BFMT_VideoFmt_eDVI_800x600p, HDMI 24 bits per pixel.
 *
 * Hydra_Software_Devel/248   4/16/10 2:37p darnstein
 * SW7420-543: get 4x pixel replication working for 480P.
 *
 * Hydra_Software_Devel/247   4/12/10 6:37p hongtaoz
 * SW3548-2680, SW3548-2681: fixed 720p3D 60 and 50 Hz VEC dvi microcodes
 * VSYNC timing issue;
 *
 * Hydra_Software_Devel/246   4/12/10 10:51a tdo
 * SW7420-673: Fix compiling error
 *
 * Hydra_Software_Devel/245   4/12/10 10:49a tdo
 * SW7420-673: add multi-frame rate support to SD
 *
 * Hydra_Software_Devel/244   4/9/10 3:21p tdo
 * SW7420-673: add multi-frame rate support to SD
 *
 * Hydra_Software_Devel/243   4/7/10 4:29p darnstein
 * SW7420-543: fix previous check in.
 *
 * Hydra_Software_Devel/242   4/7/10 4:01p darnstein
 * SW7420-543: Analog 480P output is always sampled at 54Mhz.
 *
 * Hydra_Software_Devel/241   4/5/10 5:54p darnstein
 * SW7400-2620: 480P double rate needs different HDMI setting for 7325.
 *
 * Hydra_Software_Devel/240   3/30/10 3:33p darnstein
 * SW7405-3752: For SD video formats, always enable all VEC IT
 * microcontrollers. The start addresses of unused microcontrollers is
 * set to 253, which is a loop-self instruction in every SD VEC IT
 * microcode.
 *
 * Hydra_Software_Devel/239   3/10/10 3:05p darnstein
 * SW7400-2620: logic for this JIRA should not apply to orthogonal VEC
 * chipsets. These chipsets need a different solution.
 *
 * Hydra_Software_Devel/238   3/1/10 3:51p darnstein
 * SW7400-2620,SW7400-2700: Integrate Nutan's new microcode for 480P. This
 * fixes the 480P "drop lines" problem. Also, DVI-DTG toggle settings are
 * now associated with individual microcode arrays. A new function
 * BVDC_P_GetDviDtgToggles_isr() manages this. When all microcode arrays
 * use the same settings, this function can be removed again.
 *
 * Hydra_Software_Devel/237   2/23/10 5:43p darnstein
 * SW7400-2620: fix the MC_START address for idle microcontrollers. This
 * is mostly cosmetic.
 *
 * Hydra_Software_Devel/236   2/23/10 5:10p darnstein
 * SW7400-2620: 54 MHz for 480P not yet availiable for orthogonal VEC
 * chipsets.
 *
 * Hydra_Software_Devel/235   2/23/10 4:06p darnstein
 * SW7400-2620: remove stray comment.
 *
 * Hydra_Software_Devel/234   2/23/10 3:49p pntruong
 * SW3548-2764: Allows 2x oversample 480p/576p for hdmi input.
 *
 * Hydra_Software_Devel/233   2/23/10 3:06p darnstein
 * SW7400-2620: add support for double rate 480P.
 *
 * Hydra_Software_Devel/232   2/22/10 5:51p darnstein
 * SW7420-539: integrate new 1080I analog microcode. Fixes the CGMS bad
 * line number problem.
 *
 * Hydra_Software_Devel/231   2/5/10 4:14p albertl
 * SW7405-3845: Fixed envelope generator settings for PAL_Nc and chips
 * with non-orthogonal VECs.
 *
 * Hydra_Software_Devel/230   2/2/10 11:07a pntruong
 * SW3548-2678, SW3548-2680, SW3548-2681: Filled missing data for new
 * format.
 *
 * Hydra_Software_Devel/229   2/1/10 6:36p hongtaoz
 * SW3548-2678, SW3548-2680, SW3548-2681: added HDMI1.4 3D formats support
 * for 3548; added multi-buffer low delay mode support for 1to2 and 2to5
 * rate gap cases; reduced capture buffer allocation for interlaced
 * pulldown case;
 *
 * Hydra_Software_Devel/BCM3D/5   1/29/10 6:56p hongtaoz
 * SW3548-2680, SW3548-2681: removed 1470p custom formats since they are
 * the same as 720p3D formats; added macro to wrap the 3548 specific
 * legacy 3DTV format; reduced window buffer allocation needed for
 * pulldown case; disable hddvi DE if invalid HVstart;
 *
 * Hydra_Software_Devel/BCM3D/4   1/15/10 6:34p hongtaoz
 * SW3548-2680, SW3548-2681: added custom formats to support HDMI 1.4 3D
 * 720p50/60 input passed-thru to external panel processor; reanmed 2160i
 * to custom DVI format;
 *
 * Hydra_Software_Devel/BCM3D/3   1/15/10 4:52p hongtaoz
 * SW3548-2680, SW3548-2681: added HDMI1.4 3D format 720p50 input support;
 *
 * Hydra_Software_Devel/BCM3D/2   1/14/10 5:18p hongtaoz
 * SW3548-2678, SW3548-2680, SW3548-2681: fixed build error due to the
 * changes on other mainline VDC code
 *
 * Hydra_Software_Devel/228   1/29/10 8:30p albertl
 * SW7405-3845: Fixed PAL_NC matrices and renamed associated colorspace as
 * YUV_NC.
 *
 * Hydra_Software_Devel/227   1/21/10 7:25p darnstein
 * SW7325-389: change symbol from BVBI_P_WSE_VER3 to BVDC_P_WSE_VER3.
 *
 * Hydra_Software_Devel/226   1/21/10 3:05p darnstein
 * SW7342-135,SW7420-539: my last check-in broke 1080I@50Hz output.
 *
 * Hydra_Software_Devel/225   1/20/10 4:19p darnstein
 * SW7420-539: for 1080I output, change the VEC IT microcontroller start
 * addresses, per Nutan Raj.
 *
 * Hydra_Software_Devel/224   1/13/10 4:25p darnstein
 * SW7335-636,SW7325-675: integrate PAL microcode and VEC IT start
 * addresses from Nutan. This applies only to 7325-B0 and 7335. #ifdefs
 * are used for this.
 *
 * Hydra_Software_Devel/223   1/11/10 1:17p rpan
 * SW7400-2620: Undo HDMI pixel replication. The 7400/7405 HDMI core
 * doesn't support this feature.
 *
 * Hydra_Software_Devel/BCM3D/1   12/17/09 6:08p hongtaoz
 * SW3548-2680, SW3548-2681: added HDMI1.4 3D format detection and
 * handling for 11-bit BVN;
 *
 * Hydra_Software_Devel/222   12/15/09 2:16p pntruong
 * SW3556-933: Display no signal when input signal is 1440x480@60Hz.
 *
 * Hydra_Software_Devel/221   12/11/09 3:41p rpan
 * SW7400-2620: Added HDMI pixel replication for 480p display. This is for
 * chips such as 7400 and 7405 only.
 *
 * Hydra_Software_Devel/220   12/7/09 7:10p albertl
 * SW7420-197: Fixed 480p VF settings.
 *
 * Hydra_Software_Devel/219   12/7/09 3:40p darnstein
 * SW7420-53: A "dump" function needed to be adapted to orthogonal VEC
 * chipsets. Error discovered by Coverity.
 *
 * Hydra_Software_Devel/218   12/7/09 1:04p albertl
 * SW7420-197: Updated orthogonal vecs to use same VF filter tables for
 * CVBS/S-video as component following Sam's recommendation.
 *
 * Hydra_Software_Devel/217   12/5/09 3:48a albertl
 * SW7420-197: Updated VF filters and tables for 7420 and new orthogonal
 * vecs.
 *
 * Hydra_Software_Devel/216   11/13/09 1:02p darnstein
 * SW7400-2607,SW7405-3132: New analog and digital VEC IT microcode for
 * PAL-M.
 *
 * Hydra_Software_Devel/215   10/30/09 5:21p darnstein
 * SW7405-3132: Latest PAL-M VEC IT microcode from Nutan. The "breezeway"
 * timing is correct now (per Sam He).
 *
 * Hydra_Software_Devel/214   10/22/09 5:04p darnstein
 * SW7405-3132,SW7405-3133,HW7413-238: fix breakage introduced by previous
 * check-in.
 *
 * Hydra_Software_Devel/213   10/22/09 4:35p darnstein
 * SW7405-3132,SW7405-3133,HW7413-238: Updates for PAL-M, PAL-N, and
 * Macrovision. Fix small error in NTSC/Macrovision_TEST0?. This check-in
 * breaks the build, but is needed for record keeping. Next check-in will
 * fix the build.
 *
 * Hydra_Software_Devel/212   10/16/09 5:21p darnstein
 * SW7405-3191: Back out all changes in pixel frequency handling. New
 * design coming soon.
 *
 * Hydra_Software_Devel/211   10/15/09 4:55p darnstein
 * SW7405-3191: Pixel frequency is now defined as both a bitmask, and an
 * enum. The bitmask option is DEPRECATED.
 *
 * Hydra_Software_Devel/210   10/14/09 1:19p darnstein
 * SW7405-3004: 480p/ARIB now has Macrovision capability.
 *
 * Hydra_Software_Devel/209   10/13/09 5:48p darnstein
 * SW7405-3004: install 480P ARIB output via BVDC display settings.
 *
 * Hydra_Software_Devel/208   10/7/09 1:11p darnstein
 * HW7413-238: Integrate new analog VEC IT microcode arrays for PAL-N. All
 * forms of Macrovision for PAL-N are thus updated.
 *
 * Hydra_Software_Devel/207   10/1/09 7:47p darnstein
 * SW7405-2612: add support for three new VESA display formats. Some work
 * involving HDMI output still remains.
 *
 * Hydra_Software_Devel/206   10/1/09 12:11p darnstein
 * SW7405-3099: install new analog and DVI microcode for 720P output.
 *
 * Hydra_Software_Devel/205   7/21/09 4:23p rpan
 * PR56037: Initial effort for analog VESA outputs.
 *
 * Hydra_Software_Devel/204   7/1/09 3:28p darnstein
 * PR55497: check in new microcode from Nutan, for 1080I@50Hz analog.
 * Checksum 0x48e1ecca.
 *
 * Hydra_Software_Devel/203   6/30/09 2:52p darnstein
 * PR55497: use Nutan's latest microcode for 1080I@50Hz: (0x00062209,
 * 0x5f433417).
 *
 * Hydra_Software_Devel/202   6/24/09 5:57p tdo
 * PR56334: Add all the VESA output format support for digitial output for
 * 7420.
 *
 * Hydra_Software_Devel/201   6/22/09 4:44p yuxiaz
 * PR55895, PR56025: Updated HDMI rate manager settings.
 *
 * Hydra_Software_Devel/200   6/16/09 12:11p darnstein
 * PR44592,PR54152,PR55140: I installed the wrong PAL/656 VEC IT microcode
 * array.
 *
 * Hydra_Software_Devel/199   6/16/09 11:38a yuxiaz
 * PR55497: Set SAV_REPLICATE to 2.
 *
 * Hydra_Software_Devel/198   6/15/09 2:05p yuxiaz
 * PR55895: Updated HDMI rate manager settings, added VESA modes support.
 *
 * Hydra_Software_Devel/197   6/12/09 3:42p yuxiaz
 * PR55497: First column and last few columns of analogue outputs are
 * replicated from adjacent columns.
 *
 * Hydra_Software_Devel/196   6/4/09 12:08p darnstein
 * PR44592,PR54152,PR55140: update analog and digital VEC IT microcode for
 * PAL-G and similar formats. Update corresponding microcontroller start
 * addresses too.
 *
 * Hydra_Software_Devel/195   6/2/09 3:21p darnstein
 * PR48884: More resistance to invalid user inputs.
 *
 * Hydra_Software_Devel/194   6/1/09 2:13p darnstein
 * PR48884: prevent more crashes when user specifies bad video
 * configuration on VEC. These changes specifically support 7420.
 *
 * Hydra_Software_Devel/193   5/28/09 5:57p darnstein
 * PR48884: BVDC_P_GetRmTable_isr() no longer modifies its DisplayInfo
 * argument. Instead, it returns a new agrument with the modified data.
 * There are two consequences:
 * 1. When BVDC_P_GetRmTable_isr() is called, a DisplayInfo may have to be
 * modified by the caller.
 * 2. BVDC_P_HdmiRmTableEx_isr() gets a new argument, the same new
 * argument as BVDC_P_GetRmTable_isr().
 *
 * Hydra_Software_Devel/192   5/27/09 6:11p darnstein
 * PR48884: Fix problem that I introduced yesterday.
 *
 * Hydra_Software_Devel/191   5/27/09 12:27a pntruong
 * PR48884: Temporary backout for getting by build error and code review.
 *
 * Hydra_Software_Devel/189   5/15/09 3:08p darnstein
 * PR55163: updated analog and digital VEC IT microcode for 720P@50Hz
 *
 * Hydra_Software_Devel/188   5/6/09 3:30p rpan
 * PR53106: Added uCode for 720p25/30Hz.
 *
 * Hydra_Software_Devel/187   5/4/09 4:22p rpan
 * PR53106: Added new format 720p25/30Hz.
 *
 * Hydra_Software_Devel/186   4/29/09 6:24p pntruong
 * PR50031: Updated dvo rate manager for 27.0 * 1.001Mhz.
 *
 * Hydra_Software_Devel/185   4/28/09 8:24p darnstein
 * PR54550: apply Maulshri's updated microcode and register settings for
 * PAL-M output.
 *
 * Hydra_Software_Devel/183   4/24/09 3:24p rpan
 * PR54567: Removed the policy of which video formats allow decimation.
 * Let VDC PI caller enforce it.
 *
 * Hydra_Software_Devel/182   4/23/09 3:32p darnstein
 * PR51325: update SM.PG_CNTRL.INIT_PHASE to 0x102, per Nutan Raj.
 *
 * Hydra_Software_Devel/181   4/23/09 2:59p rpan
 * PR54437: Merged 7420 version bvdc_displayfmt_priv.c/h back to mainline.
 * Updated digitial 480p@60hz, 576p@50hz, and 720p@50hz ucode with the
 * updated checksum and date.
 *
 * Hydra_Software_Devel/180   4/16/09 4:55p rpan
 * PR54255: Updated 1080p30 uCode to address the HDMI audio drop issue.
 *
 * Hydra_Software_Devel/179   4/6/09 11:37a pntruong
 * PR50031: Updated digitial 480p@60hz, 576p@50hz, and 720p@50hz ucode to
 * be able to run in master mode.
 *
 * Hydra_Software_Devel/178   4/2/09 6:11p darnstein
 * PR51325: Workaround for WSE bug: toggle VF_MISC.VBI_ENABLE every video
 * field.
 *
 * Hydra_Software_Devel/177   3/31/09 8:51p pntruong
 * PR51930: Corrected rate print out, and remove error assumption that vec
 * rate must be present when doing dvo master output.
 *
 * Hydra_Software_Devel/176   3/31/09 5:34p pntruong
 * PR51930: Added support for multiple rates for 23.93/24.00, 29.97/30.00.
 *
 * Hydra_Software_Devel/175   3/31/09 4:38p darnstein
 * PR52749: port over microcode and registry settings from 7420, for 1080P
 * output.
 *
 * Hydra_Software_Devel/174   3/18/09 6:04p pntruong
 * PR53234, PR53395: Added support for wvga, wsvga, sxga.
 *
 * Hydra_Software_Devel/173   3/5/09 10:34a yuxiaz
 * PR50031: Added LVDS output support for 576p/720p/1080i/1080p@50Hz and
 * 480p@60Hz.
 *
 * Hydra_Software_Devel/172   3/4/09 11:19a pntruong
 * PR52568: Customer not needed the custom 1340x725p@60hz.
 *
 * Hydra_Software_Devel/171   3/2/09 3:05p pntruong
 * PR52568: Added support for custom format 1340x725p@60hz.
 *
 * Hydra_Software_Devel/170   2/11/09 10:12a pntruong
 * PR51930: Used BFMT_VideoInfo to check for multirates capability.
 *
 * Hydra_Software_Devel/169   2/10/09 5:08p pntruong
 * PR51930: Updated dynamics custom format to be capable of dual rates.
 *
 * Hydra_Software_Devel/168   2/10/09 11:16a yuxiaz
 * PR51614: VDC global symbol naming convention violations
 *
 * Hydra_Software_Devel/167   1/28/09 1:48p darnstein
 * PR50904: install new analog and digital VEC IT microcode for video
 * format 1125I@50Hz. Per Nutan Raj.
 *
 * Hydra_Software_Devel/166   1/8/09 5:52p darnstein
 * PR50904: install new microcode for 1080I@50Hz. Checksum 517547.
 *
 * Hydra_Software_Devel/165   1/6/09 5:25p darnstein
 * PR45737: For 1080P outupt at 50Hz, use the VEC IT microcode developed
 * for 1080P at 25 Hz. According to Nutan Raj, this is the way to go.
 *
 * Hydra_Software_Devel/164   12/2/08 4:29p yuxiaz
 * PR49581: Updated settings for BFMT_VideoFmt_eCUSTOM_1366x768p
 *
 * Hydra_Software_Devel/163   11/26/08 4:52p yuxiaz
 * PR49581: Added 1366 x 768 60Hz support over DVI / HDMI
 *
 * Hydra_Software_Devel/162   11/10/08 3:08p tdo
 * PR46484: fix error due to merge
 *
 * Hydra_Software_Devel/161   11/10/08 3:03p tdo
 * PR46484: Fix VF_FORMAT_ADDER settings
 *
 * Hydra_Software_Devel/160   11/7/08 5:34p darnstein
 * PR46484: Add functions to print out contents of some important register
 * value tables. Must be activated with #define BVDC_P_DISPLAY_DUMP.
 *
 * Hydra_Software_Devel/159   11/7/08 1:40p pntruong
 * PR45239, PR48758: Added 1080i/1080p/720p@ 60hz LVDS output support.
 * Updated respective digital ucode to be able to run in master mode.
 *
 * Hydra_Software_Devel/158   11/7/08 11:02a darnstein
 * PR46484: fix errors in new macro usage, more.
 *
 * Hydra_Software_Devel/157   11/5/08 8:00p pntruong
 * PR42222, PR48033, PR48613: Updated 576p@50hz ucode that fixes the
 * jagged edges.
 *
 * Hydra_Software_Devel/156   11/5/08 5:11p pntruong
 * PR45239: Added temporary support for DVI master mode 1080p/720p/1080i.
 *
 * Hydra_Software_Devel/155   11/5/08 4:20p darnstein
 * PR46847: more PAL VEC IT microcode from Bangalore.
 *
 * Hydra_Software_Devel/154   11/5/08 10:58a tdo
 * PR46484: Fix RM settings
 *
 * Hydra_Software_Devel/153   10/29/08 12:02p tdo
 * PR46484: Fix SM setting
 *
 * Hydra_Software_Devel/152   10/28/08 7:35p darnstein
 * PR46847: combine VEC IT microcode for PAL Macrovision type1, type2,
 * type3.
 *
 * Hydra_Software_Devel/151   10/23/08 9:53p tdo
 * PR46484: Fix compiling error
 *
 * Hydra_Software_Devel/150   10/23/08 9:39p tdo
 * PR46484:Initial VDC bringup for 7420
 *
 * Hydra_Software_Devel/149   10/21/08 10:00a rpan
 * PR48159: Fixed an index bug when used to access RM table.
 *
 * Hydra_Software_Devel/148   9/24/08 7:51p darnstein
 * PR46949: DCS ON/OFF no longer "flashes." This is the 97400 check in.
 *
 * Hydra_Software_Devel/147   9/20/08 10:56a pntruong
 * PR47149: Relax const to allow app to dynamically load of custom ucode.
 *
 * Hydra_Software_Devel/146   8/8/08 2:25p darnstein
 * PR44592: Complete integration of new VEC IT microcode.
 *
 * Hydra_Software_Devel/145   8/6/08 4:45p pntruong
 * PR45219:  Added new RGB formats.
 *
 * Hydra_Software_Devel/144   7/31/08 5:33p darnstein
 * PR45142: VBI_PREFERRED bit is now ON for PAL and NTSC. New VEC IT
 * microcode requires this for WSS, and for AMOL II high speed.
 *
 * Hydra_Software_Devel/143   7/11/08 3:47p darnstein
 * PR44592: Properly implement PAL microcode. Roll back PAL-M analog
 * microcode. It was broken. Roll back PAL digital microcode. It seems to
 * break PAL-N HDMI output.
 *
 * Hydra_Software_Devel/142   7/10/08 8:13p darnstein
 * PR44592: Partial completion of microcode update.
 * Analog updates:
 * NTSC
 * PAL-M
 * 1080P_30Hz
 * 480P
 * 576P
 * Digital updates:
 * 576I
 * 1080I
 * 720P (checksum and timestamp only)
 * 720P_50Hz (checksum and timestamp only)
 *
 * Hydra_Software_Devel/141   6/23/08 3:44p rpan
 * PR43939: Enabled 1080p@29.97.
 *
 * Hydra_Software_Devel/140   6/9/08 1:57p rpan
 * PR39529: Back out FMT_VideoFmt_e1080p_48Hz change. Will use
 * FMT_Video_eCustom to test 48Hz panel.
 *
 * Hydra_Software_Devel/139   6/9/08 10:47a rpan
 * PR39529: Added new display format FMT_VideoFmt_e1080p_48Hz. uCode is
 * empty right now.
 *
 * Hydra_Software_Devel/138   5/22/08 1:00p rpan
 * PR42109: Turn BOTTLES_ENABLE off for all SECAM variants.
 *
 * Hydra_Software_Devel/137   5/16/08 2:00p rpan
 * PR42109: SECAM variants support.
 *
 * Hydra_Software_Devel/136   5/1/08 2:38p rpan
 * PR42109: Initial work for SECAM variants.
 *
 * Hydra_Software_Devel/135   3/24/08 9:00p darnstein
 * PR25971: Add bit that enables DCS out over 576P video.
 *
 * Hydra_Software_Devel/134   3/20/08 10:31a rpan
 * PR40144: Increased video formatter adder offset value for SD-YPbPr.
 *
 * Hydra_Software_Devel/133   3/13/08 4:53p pntruong
 * PR40280: Fixed bad placement of new enum.
 *
 * Hydra_Software_Devel/132   3/12/08 10:39a pntruong
 * PR40280: Added missing entry for pal-60.
 *
 * Hydra_Software_Devel/131   3/11/08 10:45p pntruong
 * PR40280: Added support for vdec PAL-60.
 *
 * Hydra_Software_Devel/130   3/10/08 9:36p darnstein
 * PR25971: first try at DCS for 7400
 *
 * Hydra_Software_Devel/129   2/29/08 2:26p rpan
 * PR39776: Reverted change for PR39325.
 *
 * Hydra_Software_Devel/128   2/22/08 5:12p darnstein
 * PR39783: Use VEC timing microcode for SECAM that includes Macrovision
 * algorithms.
 *
 * Hydra_Software_Devel/127   2/13/08 3:10p pntruong
 * PR39421: Removed unused code to improve readability.
 *
 * Hydra_Software_Devel/126   2/13/08 11:58a darnstein
 * PR39526: updateVEC  microcode for PAL (one array).
 *
 * Hydra_Software_Devel/125   2/11/08 8:44p pntruong
 * PR39177: [VEC] Need new display api to specify 60.00Hz or 59.94Hz
 * (framedrop).
 *
 * Hydra_Software_Devel/124   2/6/08 8:01p darnstein
 * PR28452: another try at integrating Nutan's 576P VEC microcode. One
 * analog array in.
 *
 * Hydra_Software_Devel/123   2/4/08 4:11p rpan
 * PR39325: Uses all-pass band-limiting filters for SD-YPrPb.
 *
 * Hydra_Software_Devel/122   2/1/08 12:04p darnstein
 * PR28452: second try at integrating the digital microcode for 576P. At
 * least  everything compiles now.
 *
 * Hydra_Software_Devel/121   1/31/08 5:42p albertl
 * PR36701:  Removed MUD rate manager hack.
 *
 * Hydra_Software_Devel/120   1/31/08 2:20p darnstein
 * PR28452: integrate VEC microcode from
 * vec_625i_625pMV_dvis_656b_bvb_input.bss, for 576P analog and digital
 * output. Two VEC microcode arrays changed.
 *
 * Hydra_Software_Devel/119   1/25/08 9:10p albertl
 * PR36701:  Fixed secam microcode accidentally overwritten in merge.
 *
 * Hydra_Software_Devel/118   12/22/07 3:39a albertl
 * PR36701:  Integrated and rewrote andover mud code.  Fixed mud
 * transitional issues.
 *
 * Hydra_Software_Devel/117   12/18/07 3:48p darnstein
 * PR34586: Integrate new microcode from Nutan, for SECAM output. The
 * start signal to VBI encoder cores occurs earlier. This allows teletext
 * to meet timing requirements.
 *
 * Hydra_Software_Devel/116   12/3/07 2:02p yuxiaz
 * PR37802, PR37704: Added HDMI support on 7325.
 *
 * Hydra_Software_Devel/115   12/3/07 11:43a pntruong
 * PR37790: [PC]: Remove two reduntant formats:
 * BFMT_VideoFmt_eDVI_1920x1080p_60Hz_Cea; and
 * BFMT_VideoFmt_eDVI_1920x1080i_60Hz.
 *
 * Hydra_Software_Devel/114   11/29/07 3:14p rpan
 * PR36066: Updated Video Formatter register settings.
 *
 * Hydra_Software_Devel/113   11/27/07 9:54a rpan
 * PR36751, PR36752: Updated the VEC settings to address the PAL output
 * voltage and rise/fall time issues.
 *
 * Hydra_Software_Devel/112   11/1/07 10:02p pntruong
 * PR34952: updated settings for 1080p@25Hz.
 *
 * Hydra_Software_Devel/111   10/30/07 8:25p pntruong
 * PR34239: Fixed build errors, and use cur info instead.
 *
 * Hydra_Software_Devel/110   10/30/07 7:14p pntruong
 * PR34239: Allow dynamically loading of vec custom timing.
 *
 * Hydra_Software_Devel/109   10/15/07 3:15p pntruong
 * PR35137: Add NTSC443 support for cvbs/Svideo.
 *
 * Hydra_Software_Devel/108   10/11/07 6:27p syang
 * PR 35068, PR 35069, PR35070: coverity ( OVERRUN_STATIC) fix
 *
 * Hydra_Software_Devel/107   10/5/07 5:44p darnstein
 * PR35195: these are the same changes I made on the dedicated 7401
 * branch. Three new Macrovision types for PAL.
 *
 * Hydra_Software_Devel/105   10/4/07 3:10p syang
 * PR 34773: added  s_aulItTable_720p_50hz basing on
 * vobs\BBSScripts\Bcm7400\B0\BringUp\VEC\50hz_vec_s1720p_s0_720p_s2720p_
 * dvis_656p_bvb_input.bss
 *
 * Hydra_Software_Devel/104   9/21/07 2:15p darnstein
 * PR35195: the NTSC and NTSC_704 work is ready for test.
 *
 * Hydra_Software_Devel/103   8/30/07 4:25p darnstein
 * PR33711: declare static tables to be const. This is required by
 * Broadcom's programming standards.
 *
 * Hydra_Software_Devel/102   8/30/07 3:23p albertl
 * PR27204:  Added 7440 specific ntsc macrovision microcode.
 *
 * Hydra_Software_Devel/101   8/27/07 5:23p albertl
 * PR26039:  Updated PCL settings for 720p to fix missing positive sync.
 *
 * Hydra_Software_Devel/100   8/17/07 9:59a pntruong
 * PR34059: Need to fix FMT name for 1024x768i_43Hz.
 *
 * Hydra_Software_Devel/PR34059/1   8/17/07 11:40a gorgon
 * PR34059: Need to fix FMT name for 1024x768i_43Hz
 *
 * Hydra_Software_Devel/99   7/19/07 4:33p pntruong
 * PR33138: [VDEC][PC]:Support PC 1920x1080i_60Hz.
 *
 * Hydra_Software_Devel/Gorgon_20070719/1   7/19/07 3:39p gorgon
 * PR33138:[VDEC][PC]:Support PC 1920x1080i_60Hz.
 *
 * Hydra_Software_Devel/98   7/11/07 12:09p pntruong
 * PR32296, PR32913, PR32919: Added support for 1080p@60Hz_Cea,
 * 1366x768@60Hz.
 *
 * Hydra_Software_Devel/PR32296/2   7/11/07 9:34p gorgon
 * PR32919:[PC]: Support format 1366x768@60.
 *
 * Hydra_Software_Devel/PR32296/1   7/11/07 7:26p gorgon
 * PR32913:[PC]:Add a new format 1920x1080p@60Hz_Cea.
 *
 * Hydra_Software_Devel/97   6/28/07 7:25p hongtaoz
 * PR32130, PR29189: added buffer game mode delay control; fixed picture
 * timestamp update bug when isr reorder occurs; invalid timestamp/drift
 * delay should disappear now; lipsync delay reflects the correct game
 * mode effect;
 *
 * Hydra_Software_Devel/96   6/19/07 4:56p darnstein
 * PR30013: use new microcode and register settings from Vivek and Nutan.
 *
 * Hydra_Software_Devel/95   6/12/07 4:33p pntruong
 * PR32076, PR32078, PR32081: Updated pc-input pll settings.
 *
 * Hydra_Software_Devel/94   6/5/07 8:41p pntruong
 * PR31619: Add two new format: 1280x768p/60Hz reduced blank and
 * 1400x1050p/60Hz reduced blank.
 *
 * Hydra_Software_Devel/93   5/18/07 2:50p yuxiaz
 * PR28810: add format 832x624 support. Merge from PR28810 branch.
 *
 * Hydra_Software_Devel/PR28810/1   5/16/07 11:27a honglang
 * PR28810: add format 832x624 support
 *
 * Hydra_Software_Devel/92   3/7/07 10:55a maivu
 * PR 27609: Add DVI support for 7400B0
 *
 * Hydra_Software_Devel/91   2/28/07 10:52a hongtaoz
 * PR23188: use the same 1080p hdmi RM settings as custom0 format;
 *
 * Hydra_Software_Devel/90   2/15/07 4:04p pntruong
 * PR25235: Need to use actual pixel frequency and vertical frequency
 * fields in BFMT instead of the MASK.
 *
 * Hydra_Software_Devel/89   2/14/07 4:59p darnstein
 * PR27204: Update the first two arrays. These are the NTSC720 and NTSC704
 * arrays, respectively. The update is quite unusual. In the first array,
 * I replaced entries 07 and 09 from the corresponding entries in
 * Bangalore file Cx/vec_ntscMV3_ntscMV3_nodvi_656b_bvb_input.bss. In the
 * second array, I replaced entries 09 and 09 from the corresponding
 * entries in Bangalore file Cx/vec_ntsc_bvb_input_704.bss. In both
 * arrays, I supplied timestamps and checksums that I simply made up. I
 * hope that this is the last time this sort of editing has to be done.
 *
 * Hydra_Software_Devel/88   2/12/07 1:49p pntruong
 * PR27821:  Remove unused lagacy and work-around code (7038) from
 * mainline.
 *
 * Hydra_Software_Devel/87   2/6/07 11:59a hongtaoz
 * PR26039: roll back the 480i/1080i/720p microcodes and shaper settings
 * to keep spec compliant;
 *
 * Hydra_Software_Devel/86   1/23/07 4:06p pntruong
 * PR23225: VDEC - PC input.  Updated values for FD detect for PC mode.
 *
 * Hydra_Software_Devel/85   1/16/07 4:19p hongtaoz
 * PR23260, PR23280, PR23204: added 59.94 and 50Hz 1080p panel custom
 * format support;
 *
 * Hydra_Software_Devel/84   1/11/07 6:17p hongtaoz
 * PR23188, PR23196: 3563 uses new dvpo RM;
 *
 * Hydra_Software_Devel/83   12/22/06 11:56a syang
 * PR 22569:  ensure a SD buffer could handle a anr frame
 *
 * Hydra_Software_Devel/82   12/20/06 4:53p maivu
 * PR 25963: Added initial support for 7400B0
 *
 * Hydra_Software_Devel/81   12/18/06 11:37p pntruong
 * PR22577: Merged back to mainline.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/9   12/13/06 5:22p hongtaoz
 * PR26039: updated 480i/720p/1080i analog microcodes for new shaper
 * settings;
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/8   10/26/06 5:19p hongtaoz
 * PR24995: merge to 3563 branch;
 *
 * Hydra_Software_Devel/77   10/26/06 5:02p hongtaoz
 * PR24995: 720/704 transition issue;
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/7   10/23/06 4:30p hongtaoz
 * PR23280: add custom format support for dvo master mode;
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/6   10/9/06 2:39p hongtaoz
 * PR23196, PR24727: add DVO RM support for 3563; add 1600x1200p (162MHz)
 * pixel clock RM table;
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/5   10/4/06 6:56p hongtaoz
 * PR23204, PR23279, PR23280, PR24727: add user-defined custom formats;
 * removed CUSTOM 1600x1200p format (it should be VESA standard);
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/4   10/4/06 3:49p jessem
 * PR 24727: Added VESA 1600 x 1200p tables.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/3   9/29/06 2:18p tdo
 * PR 24608:  Add 240p mode support
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/2   9/29/06 8:30a pntruong
 * PR24540, PR20875, PR11739, PR24376, PR18329: Take in changes from
 * mainline.
 *
 * Hydra_Software_Devel/75   9/28/06 2:38p hongtaoz
 * PR11739: use -6dbSRC filter and proper band-limiting filter for HD
 * output when HD SRC  filter is improved after7038Cx;
 *
 * Hydra_Software_Devel/74   9/25/06 4:36p jessem
 * PR 24376: Added 1080p 25Hz support.
 *
 * Hydra_Software_Devel/73   9/18/06 5:04p jessem
 * PR 24224: Swapped BFMT_PXL_27MHz and BFMT_PXL_27MHz_MUL_1_001 entries
 * in s_aRmTable_Tbl.
 *
 * Hydra_Software_Devel/72   9/14/06 6:52p pntruong
 * PR23222, PR23225: Support VDEC ATSC & PC formats.  Added fields to
 * internal format look up table to make distinction about standard
 * definition (SD), extended definition (ED), and high definition (HD) as
 * many fields in vdec are base on these fields.
 *
 * Hydra_Software_Devel/70   9/6/06 4:19p hongtaoz
 * PR22568, PR23188: added 1080p60 format for 3563;
 *
 * Hydra_Software_Devel/69   8/29/06 10:58a hongtaoz
 * PR23032, PR23246: updated microcode for NTSC to support Gemstar; add
 * new QDAC support for 3563; centralize some chip-specific display
 * macroes into bvdc_common_priv.h;
 *
 * Hydra_Software_Devel/68   8/23/06 5:33p pntruong
 * PR23836: Please fix compiler warnings with: VDC, RDC, and FMT.
 * Clearcase ate previous check in.
 *
 * Hydra_Software_Devel/65   8/16/06 3:27p pntruong
 * PR23522: Add supporting hdmi pc input 800x600@56Hz and 800x600@85Hz
 *
 * Hydra_Software_Devel/64   7/18/06 5:57p hongtaoz
 * PR22062: added separate PAL_M microcode to comply with BT.470-6
 * Figure.5b burst timing spec;
 *
 * Hydra_Software_Devel/63   7/17/06 7:16p hongtaoz
 * PR22081: updated SECAM microcode to fix the dvi output; remove one line
 * at top, so no need to invert triggers;
 *
 * Hydra_Software_Devel/62   7/11/06 7:00p hongtaoz
 * PR22062: fixed PAL_M tg_config setting; updated PAL_N microcode; PAL_NC
 * both u/v burst amplitude have to be negative;
 *
 * Hydra_Software_Devel/61   7/5/06 6:38p pntruong
 * PR20913: Need support for smaller formats to avoid clipping issues.
 * Made 240p and 288p a custom format for 7411 because of non-repeat
 * pixel support.
 *
 * Hydra_Software_Devel/60   6/30/06 5:46p hongtaoz
 * PR22081, PR20395, PR22062, PR22246: added SECAM Macrovision support;
 * fixed PAL_M dvi microcode; fixed PAL_M sub-carrier frequency setting;
 * PAL_NC should use the same microcode as normal PAL; fixed SECAM and
 * various PAL's csc matrices according to BT.470-6 spec; changed SRC
 * filter setting to beneifit higher freq response for HD output on
 * PRIM_VEC; fixed PAL sync tip setting for non-Macrovision build;
 *
 * Hydra_Software_Devel/59   6/23/06 6:33p hongtaoz
 * PR22246: some VEC csc, sync, offset values are incorrect;
 *
 * Hydra_Software_Devel/58   6/19/06 6:14p hongtaoz
 * PR22081: add SECAM support for dvi output;
 *
 * Hydra_Software_Devel/57   6/19/06 3:35p hongtaoz
 * PR21955, PR22081: s/w workaround for 7401B0 SEC_VEC format switch;
 * SECAM microcode seems field inversion;
 *
 * Hydra_Software_Devel/56   6/19/06 10:15a pntruong
 * PR20913: Need support for smaller formats to avoid clipping issues.
 * Added 240p60Hz and 288p50Hz hddvi input support.
 *
 * Hydra_Software_Devel/55   6/12/06 11:10a hongtaoz
 * PR22081: bring up SECAM; added SECAM color matrix;
 *
 * Hydra_Software_Devel/54   6/9/06 8:45a jessem
 * PR 19995: Added 720p 23.976/24Hz support.
 *
 * Hydra_Software_Devel/53   6/7/06 7:16p hongtaoz
 * PR21955, PR21541: 7401B0 SEC_VEC_SM needs to adjust delay to bringup
 * CVBS output on sec_vec; initial bringup SECAM, no color yet;
 *
 * Hydra_Software_Devel/52   6/5/06 4:16p jessem
 * PR 19995: Added 1080p 24Hz support.
 *
 * Hydra_Software_Devel/51   5/19/06 1:46p pntruong
 * PR21560: Global symbols without BVDC prefix in the BVDC module.  Reduce
 * unnecessary global exported symbols.  Tighten const params.
 *
 * Hydra_Software_Devel/50   5/15/06 2:26p hongtaoz
 * PR18360: updated PAL analog microcode to fix h-timing;
 *
 * Hydra_Software_Devel/49   5/10/06 2:58p jessem
 * PR 21447: Added ulVertRateTime to BVDC_P_FormatInfo. Added a table of
 * VSYNC time in usecs as a counterpart to VSYNC rate in Hz. This is
 * needed to determine if time between VSYNC isrs exceeds 1 field or
 * frame time.
 *
 * Hydra_Software_Devel/48   4/28/06 12:26p hongtaoz
 * PR20126: updated 7038Bx PAL microcodes to comply with vbi encode timing
 * spec;
 *
 * Hydra_Software_Devel/47   4/19/06 1:55p hongtaoz
 * PR20938: redefined pixel frequency bit masks; adjusted some VESA
 * formats' pixel frequency to comply with DVT Format Support document,
 * i.e. we only support 50/59.94/60 Hz trackable frame rate for display
 * formats;
 *
 * Hydra_Software_Devel/46   4/14/06 5:09p hongtaoz
 * PR20720: enable VBI_PREFFERED bit for AMOL encoder feature (only for
 * NTSC );
 *
 * Hydra_Software_Devel/45   4/12/06 8:30p hongtaoz
 * PR20761: changed HDMI rate manager settings for 7400 and 3560B;
 *
 * Hydra_Software_Devel/44   4/7/06 10:33a yuxiaz
 * PR20598: Add support to break larger buffer into smaller buffers in
 * VDC.
 *
 * Hydra_Software_Devel/43   4/6/06 1:46p pntruong
 * PR20644: VDC to comply to BVN display usage doc from arch group.  MAD
 * does not support on PIP or PIG, prevent SD capture from direct capture
 * of HD source, and fixed live feed to scale-capture to avoid
 * transitions artifacts.
 *
 * Hydra_Software_Devel/42   3/27/06 1:56p pntruong
 * PR20239: Added support for 1920x1080P@24Hz, 1920x1080P@30Hz formats on
 * 3560B0 on HD_DVI input.
 *
 * Hydra_Software_Devel/41   3/24/06 1:32p hongtaoz
 * PR19812, PR20359, PR19204: updated NTSC 704-sample microcode; updated
 * PAL sync tip value and sync_trans_1 setting;
 *
 * Hydra_Software_Devel/40   3/22/06 7:39p hongtaoz
 * PR19812, PR20359: updated NTSC microcode again;
 *
 * Hydra_Software_Devel/39   3/20/06 3:02p hongtaoz
 * PR19223, PR18360: updated PAL microcode;
 *
 * Hydra_Software_Devel/38   3/10/06 4:29p hongtaoz
 * PR19727, PR19812: updated 704-sample NTSC microcode;
 *
 * Hydra_Software_Devel/37   3/7/06 4:11p syang
 * PR 19670: added 7438 support
 *
 * Hydra_Software_Devel/36   3/7/06 3:37p hongtaoz
 * PR19900: updated 480p dvi microcode;
 *
 * Hydra_Software_Devel/35   3/7/06 2:05p hongtaoz
 * PR19727, PR19812: updated NTSC microcode to improve the Macrovision
 * color burst start timing;
 *
 * Hydra_Software_Devel/34   2/25/06 11:01p rgreen
 * PR14950: Update 800x600 microcode to remove partial line
 *
 * Hydra_Software_Devel/33   2/21/06 4:26p yuxiaz
 * PR19258: Need to increase memory for 1366x768 case. Added
 * eBufferFormat_HD in BVDC_Settings.
 *
 * Hydra_Software_Devel/32   2/15/06 1:19p jessem
 * PR 17554: Added 1080p 30Hz entries to s_ulVfLeftCut and
 * BVDC_P_aFormatInfoTable tables.
 *
 * Hydra_Software_Devel/31   2/15/06 11:32a jessem
 * PR 17554: Added support for 1080p 30Hz.
 *
 * Hydra_Software_Devel/30   2/10/06 5:05p albertl
 * PR19119:  Integrated new 576p VEC microcode.
 *
 * Hydra_Software_Devel/29   2/9/06 4:14p pntruong
 * PR19270: HDCP glitches at wrap of Sarnoff 1080i DYNPICTS.TRP stream.
 * Update hdmi rate manager in vertical blanking to avoid glitches.
 *
 * Hydra_Software_Devel/27   2/2/06 12:13p hongtaoz
 * PR19082: bring up 7400;
 *
 * Hydra_Software_Devel/26   1/31/06 5:21p pntruong
 * PR17778: Take in changes for new hddvi.
 *
 * Hydra_Software_Devel/22   1/11/06 5:02p albertl
 * PR18803:  Updated PAL Phase settings.
 *
 * Hydra_Software_Devel/21   1/3/06 3:36p hongtaoz
 * PR17332: restore YQI/YUV outputs chroma band-limiting filter settings;
 *
 * Hydra_Software_Devel/20   12/20/05 7:10p rgreen
 * PR18020: Update microcode for 1080i AutoRestart. Remove software
 * workaround for 1080i/60;
 *
 * Hydra_Software_Devel/19   11/29/05 4:57p hongtaoz
 * PR16298, PR16781, PR17804, PR18025, PR18232: implemented 704-sample
 * NTSC analog output; added two more Macrovision control modes; added
 * RGB SCART mode (G channel w/o sync) DAC config for Macrovision
 * validation test;
 *
 * Hydra_Software_Devel/18   11/9/05 7:16p hongtaoz
 * PR17997, PR14950: updated VESA 640x480p microcodes, and enabled auto
 * restart; note dvi output still has a little left shift;
 *
 * Hydra_Software_Devel/17   11/9/05 2:21p hongtaoz
 * PR17779: updated NTSC analog microcode;
 *
 * Hydra_Software_Devel/16   11/7/05 5:35p albertl
 * PR15628:  Updated 1080i vec microcode.
 *
 * Hydra_Software_Devel/15   11/4/05 10:18a hongtaoz
 * PR17432: put back the original MV routines for 576p;
 *
 * Hydra_Software_Devel/14   11/2/05 1:30p hongtaoz
 * PR17332: updated band limiting filter settings for SD formats;
 *
 * Hydra_Software_Devel/13   10/31/05 4:11p hongtaoz
 * PR17330, PR17331: updated PAL sub-carrier modulator settings;
 *
 * Hydra_Software_Devel/12   10/27/05 7:16p albertl
 * PR17432:  Updated 576p analog and DVI microcode.  Removed
 * DVI_DTG_DTG_CONFIG workaround from Cx series.
 *
 * Hydra_Software_Devel/10   10/19/05 4:46p albertl
 * PR17432:  Enabled auto restart for 576p, changed workaround so that it
 * does not disable auto restart for 576p.
 *
 * Hydra_Software_Devel/9   10/18/05 3:58p albertl
 * PR17432:  Updated 576p Dtram microcode.
 *
 * Hydra_Software_Devel/7   9/30/05 4:24p albertl
 * PR17320:  Reenabled autorestart for the formats that need them,
 * accidentally removed in code restructuring.
 *
 * Hydra_Software_Devel/6   9/28/05 6:41p hongtaoz
 * PR17196: split PAL microcode for hydra Bx and Cx beyond; 7038Bx does
 * not support WSS vbi data insertion at line 23 for PAL format;
 *
 * Hydra_Software_Devel/5   9/26/05 7:39p albertl
 * PR15034:  Updated 656 NTSC microcode, now works with AUTO_RESTART on
 * and SLAVE_MODE on.
 *
 * Hydra_Software_Devel/4   9/22/05 4:46p albertl
 * PR16459:  Integrated new 1366 microcode.
 *
 * Hydra_Software_Devel/3   9/22/05 4:15p albertl
 * PR16601:  Integrated 480i Drop 1 DTRAM microcode.
 *
 * Hydra_Software_Devel/2   9/22/05 11:02a pntruong
 * PR16459: Fixed macrovision build errors.
 *
 * Hydra_Software_Devel/1   9/21/05 7:51p albertl
 * PR16459:  Initial Revision.  Display format code restructured, moved to
 * bvdc_displayfmt_priv.c and bvdc_displayfmt.h, and changed to be  table
 * driven  to allow ease of adding new formats.
 *
 ***************************************************************************/

#include "bvdc_displayfmt_priv.h"
#include "bchp_misc.h"

BDBG_MODULE(BVDC_DISP);

/****************************************************************
 *  Tables
 ****************************************************************/
/* NTSC it_microinstructions */
/* From vec_ntsc_macrovision_type_3.bss */
static const uint32_t s_aulRamBVBInput_NTSC[BVDC_P_RAM_TABLE_SIZE] =
{
	0x0020400c, /* 0 */
	0x00a24000, /* 1 */
	0x00288059, /* 2 */
	0x0028001b, /* 3 */
	0x0024000d, /* 4 */
	0x0020703f, /* 5 */
	0x00205040, /* 6 */
	0x00204014, /* 7 */
	0x00214044, /* 8 */
	0x00284021, /* 9 */
	0x0028c00f, /* 10 */
	0x0028c1d2, /* 11 */
	0x00288075, /* 12 */
	0x002c800b, /* 13 */
	0x0028e040, /* 14 */
	0x0028c284, /* 15 */
	0x00801000, /* 16 */
	0x00000000, /* 17 */
	0x00000000, /* 18 */
	0x00000000, /* 19 */
	0x00460003, /* 20 */
	0x00ad8003, /* 21 */
	0x00462001, /* 22 */
	0x00442005, /* 23 */
	0x00441006, /* 24 */
	0x00442006, /* 25 */
	0x0063700b, /* 26 */
	0x0063c001, /* 27 */
	0x00214018, /* 28 */
	0x0041c1e2, /* 29 */
	0x0021c018, /* 30 */
	0x00414001, /* 31 */
	0x0082d001, /* 32 */
	0x0020200f, /* 33 */
	0x00242010, /* 34 */
	0x00442006, /* 35 */
	0x004c1006, /* 36 */
	0x004c2006, /* 37 */
	0x004c4001, /* 38 */
	0x0064100b, /* 39 */
	0x00646001, /* 40 */
	0x0049c1e0, /* 41 */
	0x0049c001, /* 42 */
	0x004bc001, /* 43 */
	0x00815000, /* 44 */
	0x00444002, /* 45 */
	0x00442005, /* 46 */
	0x004c1006, /* 47 */
	0x004c2006, /* 48 */
	0x0064100b, /* 49 */
	0x00646001, /* 50 */
	0x0049c1e0, /* 51 */
	0x0049c001, /* 52 */
	0x004bc001, /* 53 */
	0x00815000, /* 54 */
	0x0083a002, /* 55 */
	0x00454001, /* 56 */
	0x00554001, /* 57 */
	0x0041c001, /* 58 */
	0x0051c001, /* 59 */
	0x0083f002, /* 60 */
	0x00414001, /* 61 */
	0x00514001, /* 62 */
	0x0041c001, /* 63 */
	0x0051c001, /* 64 */
	0x00844001, /* 65 */
	0x004d4001, /* 66 */
	0x005d4001, /* 67 */
	0x0049c001, /* 68 */
	0x0059c001, /* 69 */
	0x00849001, /* 70 */
	0x00494001, /* 71 */
	0x00594001, /* 72 */
	0x0049c001, /* 73 */
	0x0059c001, /* 74 */
	0x00000000, /* 75 */
	0x00000000, /* 76 */
	0x00000000, /* 77 */
	0x00000000, /* 78 */
	0x00000000, /* 79 */
	0x00200012, /* 80 */
	0x00a00018, /* 81 */
	0x0085c004, /* 82 */
	0x00400801, /* 83 */
	0x0020005a, /* 84 */
	0x00657008, /* 85 */
	0x00400801, /* 86 */
	0x00859008, /* 87 */
	0x003000db, /* 88 */
	0x0020103d, /* 89 */
	0x00208052, /* 90 */
	0x0030004c, /* 91 */
	0x00400801, /* 92 */
	0x0020005a, /* 93 */
	0x00660008, /* 94 */
	0x00400801, /* 95 */
	0x00862010, /* 96 */
	0x003000db, /* 97 */
	0x0020103d, /* 98 */
	0x00208052, /* 99 */
	0x0030004c, /* 100 */
	0x00000000, /* 101 */
	0x00000000, /* 102 */
	0x00000000, /* 103 */
	0x00000000, /* 104 */
	0x00400806, /* 105 */
	0x00a00064, /* 106 */
	0x0067b00f, /* 107 */
	0x00400801, /* 108 */
	0x00c08020, /* 109 */
	0x00c08020, /* 110 */
	0x00c01040, /* 111 */
	0x00c01040, /* 112 */
	0x004008f7, /* 113 */
	0x00a00064, /* 114 */
	0x0067b00f, /* 115 */
	0x00400801, /* 116 */
	0x00c08020, /* 117 */
	0x00c08020, /* 118 */
	0x00c01040, /* 119 */
	0x00c01040, /* 120 */
	0x004008f0, /* 121 */
	0x00869000, /* 122 */
	0x00400801, /* 123 */
	0x00c08020, /* 124 */
	0x00d01040, /* 125 */
	0x00000000, /* 126 */
	0x00400017, /* 127 */
	0x0068c001, /* 128 */
	0x00400011, /* 129 */
	0x0068c001, /* 130 */
	0x0068b009, /* 131 */
	0x00400019, /* 132 */
	0x00400022, /* 133 */
	0x0068c001, /* 134 */
	0x00400011, /* 135 */
	0x0068c001, /* 136 */
	0x0068b009, /* 137 */
	0x0040000f, /* 138 */
	0x00400011, /* 139 */
	0x00a00080, /* 140 */
	0x00504004, /* 141 */
	0x00000000, /* 142 */
	0x00400001, /* 143 */
	0x0020006d, /* 144 */
	0x00c03080, /* 145 */
	0x0020300b, /* 146 */
	0x0020c02c, /* 147 */
	0x0020402a, /* 148 */
	0x00400001, /* 149 */
	0x0088f000, /* 150 */
	0x00000000, /* 151 */
	0x00000000, /* 152 */
	0x00200035, /* 153 */
	0x00202036, /* 154 */
	0x00200018, /* 155 */
	0x006a6002, /* 156 */
	0x00400003, /* 157 */
	0x004000f5, /* 158 */
	0x0040000a, /* 159 */
	0x006aa002, /* 160 */
	0x006a6003, /* 161 */
	0x00400003, /* 162 */
	0x004000f4, /* 163 */
	0x0040000a, /* 164 */
	0x006aa002, /* 165 */
	0x00400001, /* 166 */
	0x0020003f, /* 167 */
	0x00202036, /* 168 */
	0x00300018, /* 169 */
	0x00400001, /* 170 */
	0x0020007f, /* 171 */
	0x00202036, /* 172 */
	0x00300018, /* 173 */
	0x00000000, /* 174 */
	0x00200035, /* 175 */
	0x00202036, /* 176 */
	0x00200018, /* 177 */
	0x006bc002, /* 178 */
	0x00400003, /* 179 */
	0x004000f5, /* 180 */
	0x0040000b, /* 181 */
	0x006c0001, /* 182 */
	0x006bc003, /* 183 */
	0x00400003, /* 184 */
	0x004000f4, /* 185 */
	0x0040000b, /* 186 */
	0x006c0001, /* 187 */
	0x00400001, /* 188 */
	0x0020003f, /* 189 */
	0x00202036, /* 190 */
	0x00300018, /* 191 */
	0x00400001, /* 192 */
	0x0020007f, /* 193 */
	0x00202036, /* 194 */
	0x00300018, /* 195 */
	0x00000000, /* 196 */
	0x00000000, /* 197 */
	0x00000000, /* 198 */
	0x00000000, /* 199 */
	0x0040001d, /* 200 */
	0x006d5001, /* 201 */
	0x0040000f, /* 202 */
	0x006d5001, /* 203 */
	0x006d400b, /* 204 */
	0x0040001b, /* 205 */
	0x00400026, /* 206 */
	0x006d5001, /* 207 */
	0x0040000f, /* 208 */
	0x006d5001, /* 209 */
	0x006d400b, /* 210 */
	0x00400013, /* 211 */
	0x0040000f, /* 212 */
	0x00a00080, /* 213 */
	0x00504002, /* 214 */
	0x00400001, /* 215 */
	0x0020006d, /* 216 */
	0x00c03080, /* 217 */
	0x0020300b, /* 218 */
	0x0020c02c, /* 219 */
	0x0020402a, /* 220 */
	0x00400001, /* 221 */
	0x008d7000, /* 222 */
	0x00000000, /* 223 */
	0x00000000, /* 224 */
	0x00200035, /* 225 */
	0x00202036, /* 226 */
	0x00200018, /* 227 */
	0x006ee002, /* 228 */
	0x00400003, /* 229 */
	0x004000f5, /* 230 */
	0x00400009, /* 231 */
	0x006f2003, /* 232 */
	0x006ee003, /* 233 */
	0x00400003, /* 234 */
	0x004000f4, /* 235 */
	0x00400009, /* 236 */
	0x006f2003, /* 237 */
	0x00400001, /* 238 */
	0x0020003f, /* 239 */
	0x00202036, /* 240 */
	0x00300018, /* 241 */
	0x00400001, /* 242 */
	0x0020007f, /* 243 */
	0x00202036, /* 244 */
	0x00300018, /* 245 */
	0x00000000, /* 246 */
	0x00000000, /* 247 */
	0x00000000, /* 248 */
	0x00000000, /* 249 */
	0x00000000, /* 250 */
	0x00000000, /* 251 */
	0x00000000, /* 252 */
	0x008fd000, /* 253 */
	0x00083007, /* 254 */
	0x1cea2c9d, /* 255 */
};

/* From vec_ntsc_704_macrovision_type_3.bss */
static const uint32_t s_aulRamBVBInput_NTSC_704[BVDC_P_RAM_TABLE_SIZE] =
{
	0x0020400c, /* 0 */
	0x00a24000, /* 1 */
	0x00288052, /* 2 */
	0x0028001e, /* 3 */
	0x00240010, /* 4 */
	0x00207040, /* 5 */
	0x00205040, /* 6 */
	0x00204010, /* 7 */
	0x00214044, /* 8 */
	0x0028402c, /* 9 */
	0x0028c00f, /* 10 */
	0x0028c1cb, /* 11 */
	0x00288075, /* 12 */
	0x002c800b, /* 13 */
	0x0028e040, /* 14 */
	0x0028c284, /* 15 */
	0x00801000, /* 16 */
	0x00000000, /* 17 */
	0x00000000, /* 18 */
	0x00000000, /* 19 */
	0x00460003, /* 20 */
	0x00ad8003, /* 21 */
	0x00462001, /* 22 */
	0x00442005, /* 23 */
	0x00441006, /* 24 */
	0x00442006, /* 25 */
	0x0063700b, /* 26 */
	0x0063c001, /* 27 */
	0x00214018, /* 28 */
	0x0041c1e2, /* 29 */
	0x0021c018, /* 30 */
	0x00414001, /* 31 */
	0x0082d001, /* 32 */
	0x0020200f, /* 33 */
	0x00242010, /* 34 */
	0x00442006, /* 35 */
	0x004c1006, /* 36 */
	0x004c2006, /* 37 */
	0x004c4001, /* 38 */
	0x0064100b, /* 39 */
	0x00646001, /* 40 */
	0x0049c1e0, /* 41 */
	0x0049c001, /* 42 */
	0x004bc001, /* 43 */
	0x00815000, /* 44 */
	0x00444002, /* 45 */
	0x00442005, /* 46 */
	0x004c1006, /* 47 */
	0x004c2006, /* 48 */
	0x0064100b, /* 49 */
	0x00646001, /* 50 */
	0x0049c1e0, /* 51 */
	0x0049c001, /* 52 */
	0x004bc001, /* 53 */
	0x00815000, /* 54 */
	0x0083a002, /* 55 */
	0x00454001, /* 56 */
	0x00554001, /* 57 */
	0x0041c001, /* 58 */
	0x0051c001, /* 59 */
	0x0083f002, /* 60 */
	0x00414001, /* 61 */
	0x00514001, /* 62 */
	0x0041c001, /* 63 */
	0x0051c001, /* 64 */
	0x00844001, /* 65 */
	0x004d4001, /* 66 */
	0x005d4001, /* 67 */
	0x0049c001, /* 68 */
	0x0059c001, /* 69 */
	0x00849001, /* 70 */
	0x00494001, /* 71 */
	0x00594001, /* 72 */
	0x0049c001, /* 73 */
	0x0059c001, /* 74 */
	0x00000000, /* 75 */
	0x00000000, /* 76 */
	0x00000000, /* 77 */
	0x00000000, /* 78 */
	0x00000000, /* 79 */
	0x00200012, /* 80 */
	0x00a00018, /* 81 */
	0x0085c004, /* 82 */
	0x00400801, /* 83 */
	0x0020005a, /* 84 */
	0x00657008, /* 85 */
	0x00400801, /* 86 */
	0x00859008, /* 87 */
	0x003000db, /* 88 */
	0x0020103d, /* 89 */
	0x00208052, /* 90 */
	0x0030004c, /* 91 */
	0x00400801, /* 92 */
	0x0020005a, /* 93 */
	0x00660008, /* 94 */
	0x00400801, /* 95 */
	0x00862010, /* 96 */
	0x003000db, /* 97 */
	0x0020103d, /* 98 */
	0x00208052, /* 99 */
	0x0030004c, /* 100 */
	0x00000000, /* 101 */
	0x00000000, /* 102 */
	0x00000000, /* 103 */
	0x00000000, /* 104 */
	0x00400806, /* 105 */
	0x00a00064, /* 106 */
	0x0067b00f, /* 107 */
	0x00400801, /* 108 */
	0x00c08020, /* 109 */
	0x00c08020, /* 110 */
	0x00c01040, /* 111 */
	0x00c01040, /* 112 */
	0x004008f7, /* 113 */
	0x00a00064, /* 114 */
	0x0067b00f, /* 115 */
	0x00400801, /* 116 */
	0x00c08020, /* 117 */
	0x00c08020, /* 118 */
	0x00c01040, /* 119 */
	0x00c01040, /* 120 */
	0x004008f0, /* 121 */
	0x00869000, /* 122 */
	0x00400801, /* 123 */
	0x00c08020, /* 124 */
	0x00d01040, /* 125 */
	0x00000000, /* 126 */
	0x00400017, /* 127 */
	0x0068c001, /* 128 */
	0x00400011, /* 129 */
	0x0068c001, /* 130 */
	0x0068b009, /* 131 */
	0x00400019, /* 132 */
	0x00400022, /* 133 */
	0x0068c001, /* 134 */
	0x00400011, /* 135 */
	0x0068c001, /* 136 */
	0x0068b009, /* 137 */
	0x0040000f, /* 138 */
	0x00400011, /* 139 */
	0x00a00080, /* 140 */
	0x00504004, /* 141 */
	0x00000000, /* 142 */
	0x00400001, /* 143 */
	0x00200069, /* 144 */
	0x00c03080, /* 145 */
	0x0020300b, /* 146 */
	0x0020c02c, /* 147 */
	0x0020402a, /* 148 */
	0x00400001, /* 149 */
	0x0088f000, /* 150 */
	0x00000000, /* 151 */
	0x00000000, /* 152 */
	0x00200035, /* 153 */
	0x00202036, /* 154 */
	0x00200018, /* 155 */
	0x006a6002, /* 156 */
	0x00400003, /* 157 */
	0x004000f5, /* 158 */
	0x0040000a, /* 159 */
	0x006aa002, /* 160 */
	0x006a6003, /* 161 */
	0x00400003, /* 162 */
	0x004000f4, /* 163 */
	0x0040000a, /* 164 */
	0x006aa002, /* 165 */
	0x00400001, /* 166 */
	0x0020003f, /* 167 */
	0x00202036, /* 168 */
	0x00300018, /* 169 */
	0x00400001, /* 170 */
	0x0020007f, /* 171 */
	0x00202036, /* 172 */
	0x00300018, /* 173 */
	0x00000000, /* 174 */
	0x00200035, /* 175 */
	0x00202036, /* 176 */
	0x00200018, /* 177 */
	0x006bc002, /* 178 */
	0x00400003, /* 179 */
	0x004000f5, /* 180 */
	0x0040000b, /* 181 */
	0x006c0001, /* 182 */
	0x006bc003, /* 183 */
	0x00400003, /* 184 */
	0x004000f4, /* 185 */
	0x0040000b, /* 186 */
	0x006c0001, /* 187 */
	0x00400001, /* 188 */
	0x0020003f, /* 189 */
	0x00202036, /* 190 */
	0x00300018, /* 191 */
	0x00400001, /* 192 */
	0x0020007f, /* 193 */
	0x00202036, /* 194 */
	0x00300018, /* 195 */
	0x00000000, /* 196 */
	0x00000000, /* 197 */
	0x00000000, /* 198 */
	0x00000000, /* 199 */
	0x0040001d, /* 200 */
	0x006d5001, /* 201 */
	0x0040000f, /* 202 */
	0x006d5001, /* 203 */
	0x006d400b, /* 204 */
	0x0040001b, /* 205 */
	0x00400026, /* 206 */
	0x006d5001, /* 207 */
	0x0040000f, /* 208 */
	0x006d5001, /* 209 */
	0x006d400b, /* 210 */
	0x00400013, /* 211 */
	0x0040000f, /* 212 */
	0x00a00080, /* 213 */
	0x00504002, /* 214 */
	0x00400001, /* 215 */
	0x00200069, /* 216 */
	0x00c03080, /* 217 */
	0x0020300b, /* 218 */
	0x0020c02c, /* 219 */
	0x0020402a, /* 220 */
	0x00400001, /* 221 */
	0x008d7000, /* 222 */
	0x00000000, /* 223 */
	0x00000000, /* 224 */
	0x00200035, /* 225 */
	0x00202036, /* 226 */
	0x00200018, /* 227 */
	0x006ee002, /* 228 */
	0x00400003, /* 229 */
	0x004000f5, /* 230 */
	0x00400009, /* 231 */
	0x006f2003, /* 232 */
	0x006ee003, /* 233 */
	0x00400003, /* 234 */
	0x004000f4, /* 235 */
	0x00400009, /* 236 */
	0x006f2003, /* 237 */
	0x00400001, /* 238 */
	0x0020003f, /* 239 */
	0x00202036, /* 240 */
	0x00300018, /* 241 */
	0x00400001, /* 242 */
	0x0020007f, /* 243 */
	0x00202036, /* 244 */
	0x00300018, /* 245 */
	0x00000000, /* 246 */
	0x00000000, /* 247 */
	0x00000000, /* 248 */
	0x00000000, /* 249 */
	0x00000000, /* 250 */
	0x00000000, /* 251 */
	0x00000000, /* 252 */
	0x008fd000, /* 253 */
	0x00011907, /* 254 */
	0xf7582519, /* 255 */
};

/* NTSC ARIB microcode */
/* From vec_ntsc_drop1_macrovision_type_3.bss */
static const uint32_t s_aulRamBVBInput_NTSCARIB[BVDC_P_RAM_TABLE_SIZE] =
{
	0x0020400c, /* 0 */
	0x00a24000, /* 1 */
	0x00288059, /* 2 */
	0x0028001b, /* 3 */
	0x0024000d, /* 4 */
	0x0020703f, /* 5 */
	0x00205040, /* 6 */
	0x00204014, /* 7 */
	0x00214044, /* 8 */
	0x00284021, /* 9 */
	0x0028c00f, /* 10 */
	0x0028c1d2, /* 11 */
	0x00288075, /* 12 */
	0x002c800b, /* 13 */
	0x0028e040, /* 14 */
	0x0028c284, /* 15 */
	0x00801000, /* 16 */
	0x00000000, /* 17 */
	0x00000000, /* 18 */
	0x00000000, /* 19 */
	0x00440003, /* 20 */
	0x00ad8003, /* 21 */
	0x00442001, /* 22 */
	0x00462002, /* 23 */
	0x00442003, /* 24 */
	0x00441006, /* 25 */
	0x00442006, /* 26 */
	0x0063900b, /* 27 */
	0x0063e001, /* 28 */
	0x00214018, /* 29 */
	0x00414002, /* 30 */
	0x0041c1e0, /* 31 */
	0x0021c018, /* 32 */
	0x00414001, /* 33 */
	0x0082f001, /* 34 */
	0x0020200f, /* 35 */
	0x00242010, /* 36 */
	0x00442006, /* 37 */
	0x004c1006, /* 38 */
	0x004c2006, /* 39 */
	0x004c4001, /* 40 */
	0x0064300b, /* 41 */
	0x00648001, /* 42 */
	0x00494002, /* 43 */
	0x0049c1de, /* 44 */
	0x0049c002, /* 45 */
	0x00815000, /* 46 */
	0x00444002, /* 47 */
	0x00442005, /* 48 */
	0x004c1006, /* 49 */
	0x004c2006, /* 50 */
	0x0064300b, /* 51 */
	0x00648001, /* 52 */
	0x00494002, /* 53 */
	0x0049c1de, /* 54 */
	0x0049c002, /* 55 */
	0x00815000, /* 56 */
	0x0083c002, /* 57 */
	0x00454001, /* 58 */
	0x00554001, /* 59 */
	0x0041c001, /* 60 */
	0x0051c001, /* 61 */
	0x00841002, /* 62 */
	0x00414001, /* 63 */
	0x00514001, /* 64 */
	0x0041c001, /* 65 */
	0x0051c001, /* 66 */
	0x00846001, /* 67 */
	0x004d4001, /* 68 */
	0x005d4001, /* 69 */
	0x0049c001, /* 70 */
	0x0059c001, /* 71 */
	0x0084b001, /* 72 */
	0x00494001, /* 73 */
	0x00594001, /* 74 */
	0x0049c001, /* 75 */
	0x0059c001, /* 76 */
	0x00000000, /* 77 */
	0x00000000, /* 78 */
	0x00000000, /* 79 */
	0x00200012, /* 80 */
	0x00a00018, /* 81 */
	0x0085c004, /* 82 */
	0x00400801, /* 83 */
	0x0020005a, /* 84 */
	0x00657008, /* 85 */
	0x00400801, /* 86 */
	0x00859008, /* 87 */
	0x003000db, /* 88 */
	0x0020103d, /* 89 */
	0x00208052, /* 90 */
	0x0030004c, /* 91 */
	0x00400801, /* 92 */
	0x0020005a, /* 93 */
	0x00660008, /* 94 */
	0x00400801, /* 95 */
	0x00862010, /* 96 */
	0x003000db, /* 97 */
	0x0020103d, /* 98 */
	0x00208052, /* 99 */
	0x0030004c, /* 100 */
	0x00000000, /* 101 */
	0x00000000, /* 102 */
	0x00000000, /* 103 */
	0x00000000, /* 104 */
	0x00400806, /* 105 */
	0x00a00064, /* 106 */
	0x0067b00f, /* 107 */
	0x00400801, /* 108 */
	0x00c08020, /* 109 */
	0x00c08020, /* 110 */
	0x00c01040, /* 111 */
	0x00c01040, /* 112 */
	0x004008f7, /* 113 */
	0x00a00064, /* 114 */
	0x0067b00f, /* 115 */
	0x00400801, /* 116 */
	0x00c08020, /* 117 */
	0x00c08020, /* 118 */
	0x00c01040, /* 119 */
	0x00c01040, /* 120 */
	0x004008f0, /* 121 */
	0x00869000, /* 122 */
	0x00400801, /* 123 */
	0x00c08020, /* 124 */
	0x00d01040, /* 125 */
	0x00000000, /* 126 */
	0x00400017, /* 127 */
	0x0068c001, /* 128 */
	0x00400011, /* 129 */
	0x0068c001, /* 130 */
	0x0068b009, /* 131 */
	0x00400019, /* 132 */
	0x00400022, /* 133 */
	0x0068c001, /* 134 */
	0x00400011, /* 135 */
	0x0068c001, /* 136 */
	0x0068b009, /* 137 */
	0x0040000f, /* 138 */
	0x00400011, /* 139 */
	0x00a00080, /* 140 */
	0x00504004, /* 141 */
	0x00000000, /* 142 */
	0x00400001, /* 143 */
	0x0020006d, /* 144 */
	0x00c03080, /* 145 */
	0x0020300b, /* 146 */
	0x0020c02c, /* 147 */
	0x0020402a, /* 148 */
	0x00400001, /* 149 */
	0x0088f000, /* 150 */
	0x00000000, /* 151 */
	0x00000000, /* 152 */
	0x00200035, /* 153 */
	0x00202036, /* 154 */
	0x00200018, /* 155 */
	0x006a6002, /* 156 */
	0x00400003, /* 157 */
	0x004000f5, /* 158 */
	0x0040000a, /* 159 */
	0x006aa002, /* 160 */
	0x006a6003, /* 161 */
	0x00400003, /* 162 */
	0x004000f4, /* 163 */
	0x0040000a, /* 164 */
	0x006aa002, /* 165 */
	0x00400001, /* 166 */
	0x0020003f, /* 167 */
	0x00202036, /* 168 */
	0x00300018, /* 169 */
	0x00400001, /* 170 */
	0x0020007f, /* 171 */
	0x00202036, /* 172 */
	0x00300018, /* 173 */
	0x00000000, /* 174 */
	0x00200035, /* 175 */
	0x00202036, /* 176 */
	0x00200018, /* 177 */
	0x006bc002, /* 178 */
	0x00400003, /* 179 */
	0x004000f5, /* 180 */
	0x0040000b, /* 181 */
	0x006c0001, /* 182 */
	0x006bc003, /* 183 */
	0x00400003, /* 184 */
	0x004000f4, /* 185 */
	0x0040000b, /* 186 */
	0x006c0001, /* 187 */
	0x00400001, /* 188 */
	0x0020003f, /* 189 */
	0x00202036, /* 190 */
	0x00300018, /* 191 */
	0x00400001, /* 192 */
	0x0020007f, /* 193 */
	0x00202036, /* 194 */
	0x00300018, /* 195 */
	0x00000000, /* 196 */
	0x00000000, /* 197 */
	0x00000000, /* 198 */
	0x00000000, /* 199 */
	0x0040001d, /* 200 */
	0x006d5001, /* 201 */
	0x0040000f, /* 202 */
	0x006d5001, /* 203 */
	0x006d400b, /* 204 */
	0x0040001b, /* 205 */
	0x00400026, /* 206 */
	0x006d5001, /* 207 */
	0x0040000f, /* 208 */
	0x006d5001, /* 209 */
	0x006d400b, /* 210 */
	0x00400013, /* 211 */
	0x0040000f, /* 212 */
	0x00a00080, /* 213 */
	0x00504002, /* 214 */
	0x00400001, /* 215 */
	0x0020006d, /* 216 */
	0x00c03080, /* 217 */
	0x0020300b, /* 218 */
	0x0020c02c, /* 219 */
	0x0020402a, /* 220 */
	0x00400001, /* 221 */
	0x008d7000, /* 222 */
	0x00000000, /* 223 */
	0x00000000, /* 224 */
	0x00200035, /* 225 */
	0x00202036, /* 226 */
	0x00200018, /* 227 */
	0x006ee002, /* 228 */
	0x00400003, /* 229 */
	0x004000f5, /* 230 */
	0x00400009, /* 231 */
	0x006f2003, /* 232 */
	0x006ee003, /* 233 */
	0x00400003, /* 234 */
	0x004000f4, /* 235 */
	0x00400009, /* 236 */
	0x006f2003, /* 237 */
	0x00400001, /* 238 */
	0x0020003f, /* 239 */
	0x00202036, /* 240 */
	0x00300018, /* 241 */
	0x00400001, /* 242 */
	0x0020007f, /* 243 */
	0x00202036, /* 244 */
	0x00300018, /* 245 */
	0x00000000, /* 246 */
	0x00000000, /* 247 */
	0x00000000, /* 248 */
	0x00000000, /* 249 */
	0x00000000, /* 250 */
	0x00000000, /* 251 */
	0x00000000, /* 252 */
	0x008fd000, /* 253 */
	0x00062311, /* 254 */
	0x9a568b77, /* 255 */
};

/* PAL it_micro_instruction */
/* From vec_pal_macrovision_type_3.bss */
static const uint32_t s_aulRamBVBInput_PAL[BVDC_P_RAM_TABLE_SIZE] =
#ifdef BVDC_P_WSE_VER3
{
	0x0020400c, /* 0 */
	0x00a24000, /* 1 */
	0x00208058, /* 2 */
	0x00240028, /* 3 */
	0x00207040, /* 4 */
	0x00205040, /* 5 */
	0x00204018, /* 6 */
	0x0021403e, /* 7 */
	0x0020402e, /* 8 */
	0x00812080, /* 9 */
	0x0020400d, /* 10 */
	0x0020c00f, /* 11 */
	0x0020c1b5, /* 12 */
	0x00208075, /* 13 */
	0x0024800b, /* 14 */
	0x0020e040, /* 15 */
	0x0020c28a, /* 16 */
	0x00801000, /* 17 */
	0x0028400d, /* 18 */
	0x0028c00f, /* 19 */
	0x0028c1b5, /* 20 */
	0x00288075, /* 21 */
	0x002c800b, /* 22 */
	0x0028e040, /* 23 */
	0x0028c295, /* 24 */
	0x00a24000, /* 25 */
	0x0028804d, /* 26 */
	0x00803000, /* 27 */
	0x00000000, /* 28 */
	0x00000000, /* 29 */
	0x00462003, /* 30 */
	0x00ad8000, /* 31 */
	0x00461001, /* 32 */
	0x00441004, /* 33 */
	0x00442003, /* 34 */
	0x00a00080, /* 35 */
	0x00442002, /* 36 */
	0x00444002, /* 37 */
	0x00454002, /* 38 */
	0x00a00080, /* 39 */
	0x00454020, /* 40 */
	0x00254018, /* 41 */
	0x0041c23c, /* 42 */
	0x0040c002, /* 43 */
	0x0020c018, /* 44 */
	0x004c2005, /* 45 */
	0x004c1005, /* 46 */
	0x004c2004, /* 47 */
	0x00a00080, /* 48 */
	0x004c2001, /* 49 */
	0x004c4001, /* 50 */
	0x004d4002, /* 51 */
	0x00a00080, /* 52 */
	0x004d4020, /* 53 */
	0x002d4018, /* 54 */
	0x0049c23c, /* 55 */
	0x0048c002, /* 56 */
	0x00404001, /* 57 */
	0x00204018, /* 58 */
	0x00442004, /* 59 */
	0x00462001, /* 60 */
	0x00a98000, /* 61 */
	0x00461001, /* 62 */
	0x00441004, /* 63 */
	0x00442003, /* 64 */
	0x00a00080, /* 65 */
	0x00442002, /* 66 */
	0x00454004, /* 67 */
	0x00a00080, /* 68 */
	0x00454020, /* 69 */
	0x00254018, /* 70 */
	0x0041c23c, /* 71 */
	0x0041c002, /* 72 */
	0x0021c018, /* 73 */
	0x004c2005, /* 74 */
	0x004c1005, /* 75 */
	0x004c2004, /* 76 */
	0x00a00080, /* 77 */
	0x004c2001, /* 78 */
	0x004c4003, /* 79 */
	0x00a00080, /* 80 */
	0x004d4020, /* 81 */
	0x002d4018, /* 82 */
	0x0049c23e, /* 83 */
	0x00404001, /* 84 */
	0x00204018, /* 85 */
	0x00442004, /* 86 */
	0x00462001, /* 87 */
	0x0081f000, /* 88 */
	0x00000000, /* 89 */
	0x00000000, /* 90 */
	0x00000000, /* 91 */
	0x00000000, /* 92 */
	0x00000000, /* 93 */
	0x00000000, /* 94 */
	0x00000000, /* 95 */
	0x00000000, /* 96 */
	0x00000000, /* 97 */
	0x00000000, /* 98 */
	0x00000000, /* 99 */
	0x00000000, /* 100 */
	0x00000000, /* 101 */
	0x00000000, /* 102 */
	0x00000000, /* 103 */
	0x00000000, /* 104 */
	0x00000000, /* 105 */
	0x00000000, /* 106 */
	0x00000000, /* 107 */
	0x00000000, /* 108 */
	0x00000000, /* 109 */
	0x00000000, /* 110 */
	0x00000000, /* 111 */
	0x00200012, /* 112 */
	0x00a00006, /* 113 */
	0x0087f001, /* 114 */
	0x00400001, /* 115 */
	0x00200101, /* 116 */
	0x0067a008, /* 117 */
	0x00000000, /* 118 */
	0x00000000, /* 119 */
	0x0020000b, /* 120 */
	0x00400801, /* 121 */
	0x0087c002, /* 122 */
	0x0030007a, /* 123 */
	0x00200019, /* 124 */
	0x0020102d, /* 125 */
	0x00308034, /* 126 */
	0x00400001, /* 127 */
	0x002000c5, /* 128 */
	0x00686008, /* 129 */
	0x00000000, /* 130 */
	0x00000000, /* 131 */
	0x0020000b, /* 132 */
	0x00400801, /* 133 */
	0x00888004, /* 134 */
	0x0030009a, /* 135 */
	0x00200035, /* 136 */
	0x00201031, /* 137 */
	0x00308034, /* 138 */
	0x00000000, /* 139 */
	0x00400806, /* 140 */
	0x00a00019, /* 141 */
	0x0069e00f, /* 142 */
	0x00400801, /* 143 */
	0x00c08008, /* 144 */
	0x00c08008, /* 145 */
	0x00c01010, /* 146 */
	0x00c01010, /* 147 */
	0x00400928, /* 148 */
	0x00a00019, /* 149 */
	0x0069e00f, /* 150 */
	0x00400801, /* 151 */
	0x00c08008, /* 152 */
	0x00c08008, /* 153 */
	0x00c01010, /* 154 */
	0x00c01010, /* 155 */
	0x00400923, /* 156 */
	0x0088c000, /* 157 */
	0x00400801, /* 158 */
	0x00c08008, /* 159 */
	0x00d01010, /* 160 */
	0x00000000, /* 161 */
	0x00400003, /* 162 */
	0x00400126, /* 163 */
	0x0040000b, /* 164 */
	0x006bb002, /* 165 */
	0x006b7002, /* 166 */
	0x00400003, /* 167 */
	0x00400127, /* 168 */
	0x0040000b, /* 169 */
	0x006bb002, /* 170 */
	0x006b7002, /* 171 */
	0x00400003, /* 172 */
	0x00400126, /* 173 */
	0x0040000b, /* 174 */
	0x006bb002, /* 175 */
	0x006b7002, /* 176 */
	0x00400003, /* 177 */
	0x00400127, /* 178 */
	0x0040000b, /* 179 */
	0x006bb002, /* 180 */
	0x006b7002, /* 181 */
	0x008a2000, /* 182 */
	0x00400001, /* 183 */
	0x0020003f, /* 184 */
	0x00202036, /* 185 */
	0x00300018, /* 186 */
	0x00400001, /* 187 */
	0x0020007f, /* 188 */
	0x00202036, /* 189 */
	0x00300018, /* 190 */
	0x00000000, /* 191 */
	0x00000000, /* 192 */
	0x00400003, /* 193 */
	0x00400126, /* 194 */
	0x00400009, /* 195 */
	0x006da004, /* 196 */
	0x006d6002, /* 197 */
	0x00400003, /* 198 */
	0x00400127, /* 199 */
	0x00400009, /* 200 */
	0x006da004, /* 201 */
	0x006d6002, /* 202 */
	0x00400003, /* 203 */
	0x00400126, /* 204 */
	0x00400009, /* 205 */
	0x006da004, /* 206 */
	0x006d6002, /* 207 */
	0x00400003, /* 208 */
	0x00400127, /* 209 */
	0x00400009, /* 210 */
	0x006da004, /* 211 */
	0x006d6002, /* 212 */
	0x008c1000, /* 213 */
	0x00400001, /* 214 */
	0x0020003f, /* 215 */
	0x00202036, /* 216 */
	0x00300018, /* 217 */
	0x00400001, /* 218 */
	0x0020007f, /* 219 */
	0x00202036, /* 220 */
	0x00300018, /* 221 */
	0x00000000, /* 222 */
	0x00400003, /* 223 */
	0x00400126, /* 224 */
	0x0040000a, /* 225 */
	0x006f8003, /* 226 */
	0x006f4002, /* 227 */
	0x00400003, /* 228 */
	0x00400127, /* 229 */
	0x0040000a, /* 230 */
	0x006f8003, /* 231 */
	0x006f4002, /* 232 */
	0x00400003, /* 233 */
	0x00400126, /* 234 */
	0x0040000a, /* 235 */
	0x006f8003, /* 236 */
	0x006f4002, /* 237 */
	0x00400003, /* 238 */
	0x00400127, /* 239 */
	0x0040000a, /* 240 */
	0x006f8003, /* 241 */
	0x006f4002, /* 242 */
	0x008df000, /* 243 */
	0x00400001, /* 244 */
	0x0020003f, /* 245 */
	0x00202036, /* 246 */
	0x00300018, /* 247 */
	0x00400001, /* 248 */
	0x0020007f, /* 249 */
	0x00202036, /* 250 */
	0x00300018, /* 251 */
	0x00000000, /* 252 */
	0x008fd000, /* 253 */
	0x00011310, /* 254 */
	0x5ded102e, /* 255 */
};
#else
{
	0x0020400c, /* 0 */
	0x00a24000, /* 1 */
	0x00288058, /* 2 */
	0x00240028, /* 3 */
	0x00207040, /* 4 */
	0x00205040, /* 5 */
	0x00204018, /* 6 */
	0x0021403e, /* 7 */
	0x00204039, /* 8 */
	0x0028400d, /* 9 */
	0x0028c00f, /* 10 */
	0x0028c1b5, /* 11 */
	0x00288075, /* 12 */
	0x002c800b, /* 13 */
	0x0028e040, /* 14 */
	0x0028c28a, /* 15 */
	0x00801000, /* 16 */
	0x00000000, /* 17 */
	0x00000000, /* 18 */
	0x00000000, /* 19 */
	0x00462003, /* 20 */
	0x00ad8000, /* 21 */
	0x00461001, /* 22 */
	0x00441004, /* 23 */
	0x00442005, /* 24 */
	0x00444002, /* 25 */
	0x00454022, /* 26 */
	0x0041c23c, /* 27 */
	0x0040c002, /* 28 */
	0x004c2005, /* 29 */
	0x004c1005, /* 30 */
	0x004c2005, /* 31 */
	0x004c4001, /* 32 */
	0x004d4022, /* 33 */
	0x0049c23c, /* 34 */
	0x0048c002, /* 35 */
	0x00404001, /* 36 */
	0x00442004, /* 37 */
	0x00462001, /* 38 */
	0x00a98000, /* 39 */
	0x00461001, /* 40 */
	0x00441004, /* 41 */
	0x00442005, /* 42 */
	0x00454024, /* 43 */
	0x0041c23c, /* 44 */
	0x0041c002, /* 45 */
	0x004c2005, /* 46 */
	0x004c1005, /* 47 */
	0x004c2005, /* 48 */
	0x004c4003, /* 49 */
	0x004d4020, /* 50 */
	0x002d4018, /* 51 */
	0x0049c23e, /* 52 */
	0x00404001, /* 53 */
	0x00442004, /* 54 */
	0x00462001, /* 55 */
	0x00815000, /* 56 */
	0x00000000, /* 57 */
	0x00000000, /* 58 */
	0x00000000, /* 59 */
	0x00000000, /* 60 */
	0x00000000, /* 61 */
	0x00000000, /* 62 */
	0x00000000, /* 63 */
	0x00000000, /* 64 */
	0x00000000, /* 65 */
	0x00000000, /* 66 */
	0x00000000, /* 67 */
	0x00000000, /* 68 */
	0x00000000, /* 69 */
	0x00000000, /* 70 */
	0x00000000, /* 71 */
	0x00000000, /* 72 */
	0x00000000, /* 73 */
	0x00000000, /* 74 */
	0x00000000, /* 75 */
	0x00000000, /* 76 */
	0x00000000, /* 77 */
	0x00000000, /* 78 */
	0x00000000, /* 79 */
	0x00200012, /* 80 */
	0x00a00006, /* 81 */
	0x0085f001, /* 82 */
	0x00400001, /* 83 */
	0x00200101, /* 84 */
	0x0065a008, /* 85 */
	0x00000000, /* 86 */
	0x00000000, /* 87 */
	0x0020000b, /* 88 */
	0x00400801, /* 89 */
	0x0085c002, /* 90 */
	0x0030007a, /* 91 */
	0x00200019, /* 92 */
	0x0020102d, /* 93 */
	0x00308034, /* 94 */
	0x00400001, /* 95 */
	0x002000c5, /* 96 */
	0x00666008, /* 97 */
	0x00000000, /* 98 */
	0x00000000, /* 99 */
	0x0020000b, /* 100 */
	0x00400801, /* 101 */
	0x00868004, /* 102 */
	0x0030009a, /* 103 */
	0x00200035, /* 104 */
	0x00201031, /* 105 */
	0x00308034, /* 106 */
	0x00000000, /* 107 */
	0x00000000, /* 108 */
	0x00000000, /* 109 */
	0x00400806, /* 110 */
	0x00a00019, /* 111 */
	0x0068000f, /* 112 */
	0x00400801, /* 113 */
	0x00c08008, /* 114 */
	0x00c08008, /* 115 */
	0x00c01010, /* 116 */
	0x00c01010, /* 117 */
	0x00400928, /* 118 */
	0x00a00019, /* 119 */
	0x0068000f, /* 120 */
	0x00400801, /* 121 */
	0x00c08008, /* 122 */
	0x00c08008, /* 123 */
	0x00c01010, /* 124 */
	0x00c01010, /* 125 */
	0x00400923, /* 126 */
	0x0086e000, /* 127 */
	0x00400801, /* 128 */
	0x00c08008, /* 129 */
	0x00d01010, /* 130 */
	0x00000000, /* 131 */
	0x00000000, /* 132 */
	0x00000000, /* 133 */
	0x00000000, /* 134 */
	0x00400003, /* 135 */
	0x00400126, /* 136 */
	0x0040000b, /* 137 */
	0x006a0002, /* 138 */
	0x0069c002, /* 139 */
	0x00400003, /* 140 */
	0x00400127, /* 141 */
	0x0040000b, /* 142 */
	0x006a0002, /* 143 */
	0x0069c002, /* 144 */
	0x00400003, /* 145 */
	0x00400126, /* 146 */
	0x0040000b, /* 147 */
	0x006a0002, /* 148 */
	0x0069c002, /* 149 */
	0x00400003, /* 150 */
	0x00400127, /* 151 */
	0x0040000b, /* 152 */
	0x006a0002, /* 153 */
	0x0069c002, /* 154 */
	0x00887000, /* 155 */
	0x00400001, /* 156 */
	0x0020003f, /* 157 */
	0x00202036, /* 158 */
	0x00300018, /* 159 */
	0x00400001, /* 160 */
	0x0020007f, /* 161 */
	0x00202036, /* 162 */
	0x00300018, /* 163 */
	0x00000000, /* 164 */
	0x00000000, /* 165 */
	0x00000000, /* 166 */
	0x00000000, /* 167 */
	0x00000000, /* 168 */
	0x00000000, /* 169 */
	0x00400003, /* 170 */
	0x00400126, /* 171 */
	0x00400009, /* 172 */
	0x006c3004, /* 173 */
	0x006bf002, /* 174 */
	0x00400003, /* 175 */
	0x00400127, /* 176 */
	0x00400009, /* 177 */
	0x006c3004, /* 178 */
	0x006bf002, /* 179 */
	0x00400003, /* 180 */
	0x00400126, /* 181 */
	0x00400009, /* 182 */
	0x006c3004, /* 183 */
	0x006bf002, /* 184 */
	0x00400003, /* 185 */
	0x00400127, /* 186 */
	0x00400009, /* 187 */
	0x006c3004, /* 188 */
	0x006bf002, /* 189 */
	0x008aa000, /* 190 */
	0x00400001, /* 191 */
	0x0020003f, /* 192 */
	0x00202036, /* 193 */
	0x00300018, /* 194 */
	0x00400001, /* 195 */
	0x0020007f, /* 196 */
	0x00202036, /* 197 */
	0x00300018, /* 198 */
	0x00000000, /* 199 */
	0x00400003, /* 200 */
	0x00400126, /* 201 */
	0x0040000a, /* 202 */
	0x006e1003, /* 203 */
	0x006dd002, /* 204 */
	0x00400003, /* 205 */
	0x00400127, /* 206 */
	0x0040000a, /* 207 */
	0x006e1003, /* 208 */
	0x006dd002, /* 209 */
	0x00400003, /* 210 */
	0x00400126, /* 211 */
	0x0040000a, /* 212 */
	0x006e1003, /* 213 */
	0x006dd002, /* 214 */
	0x00400003, /* 215 */
	0x00400127, /* 216 */
	0x0040000a, /* 217 */
	0x006e1003, /* 218 */
	0x006dd002, /* 219 */
	0x008c8000, /* 220 */
	0x00400001, /* 221 */
	0x0020003f, /* 222 */
	0x00202036, /* 223 */
	0x00300018, /* 224 */
	0x00400001, /* 225 */
	0x0020007f, /* 226 */
	0x00202036, /* 227 */
	0x00300018, /* 228 */
	0x00000000, /* 229 */
	0x00000000, /* 230 */
	0x00000000, /* 231 */
	0x00000000, /* 232 */
	0x00000000, /* 233 */
	0x00000000, /* 234 */
	0x00000000, /* 235 */
	0x00000000, /* 236 */
	0x00000000, /* 237 */
	0x00000000, /* 238 */
	0x00000000, /* 239 */
	0x00000000, /* 240 */
	0x00000000, /* 241 */
	0x00000000, /* 242 */
	0x00000000, /* 243 */
	0x00000000, /* 244 */
	0x00000000, /* 245 */
	0x00000000, /* 246 */
	0x00000000, /* 247 */
	0x00000000, /* 248 */
	0x00000000, /* 249 */
	0x00000000, /* 250 */
	0x00000000, /* 251 */
	0x00000000, /* 252 */
	0x008fd000, /* 253 */
	0x00060110, /* 254 */
	0xb8006632, /* 255 */
};
#endif

/* PAL_N it_micro_instruction is unique per ITU-BT.470-6 */
/* From vec_paln_macrovision_type_3.bss */
static const uint32_t s_aulRamBVBInput_PALN[BVDC_P_RAM_TABLE_SIZE] =
{
	0x0020400c, /* 0 */
	0x00a24000, /* 1 */
	0x00288059, /* 2 */
	0x0028001b, /* 3 */
	0x00240018, /* 4 */
	0x00207040, /* 5 */
	0x00205040, /* 6 */
	0x00204014, /* 7 */
	0x00214044, /* 8 */
	0x00204021, /* 9 */
	0x0020c00f, /* 10 */
	0x0028c1d2, /* 11 */
	0x00288075, /* 12 */
	0x002c800b, /* 13 */
	0x0028e040, /* 14 */
	0x0028c284, /* 15 */
	0x00801000, /* 16 */
	0x00000000, /* 17 */
	0x00000000, /* 18 */
	0x00000000, /* 19 */
	0x00462001, /* 20 */
	0x00ad8000, /* 21 */
	0x00461001, /* 22 */
	0x00441004, /* 23 */
	0x00442005, /* 24 */
	0x00444002, /* 25 */
	0x00454022, /* 26 */
	0x00254018, /* 27 */
	0x0041c23c, /* 28 */
	0x0040c002, /* 29 */
	0x0020c018, /* 30 */
	0x004c2005, /* 31 */
	0x004c1005, /* 32 */
	0x004c2005, /* 33 */
	0x004c4001, /* 34 */
	0x004d4022, /* 35 */
	0x002d4018, /* 36 */
	0x0049c23c, /* 37 */
	0x0048c002, /* 38 */
	0x00404001, /* 39 */
	0x00442004, /* 40 */
	0x00462001, /* 41 */
	0x00a98000, /* 42 */
	0x00461001, /* 43 */
	0x00441004, /* 44 */
	0x00442005, /* 45 */
	0x00454024, /* 46 */
	0x00254018, /* 47 */
	0x0041c23c, /* 48 */
	0x0041c002, /* 49 */
	0x0021c018, /* 50 */
	0x004c2005, /* 51 */
	0x004c1005, /* 52 */
	0x004c2005, /* 53 */
	0x004c4003, /* 54 */
	0x004d4020, /* 55 */
	0x002d4018, /* 56 */
	0x0049c23e, /* 57 */
	0x00404001, /* 58 */
	0x00442004, /* 59 */
	0x00462001, /* 60 */
	0x00815000, /* 61 */
	0x00000000, /* 62 */
	0x00000000, /* 63 */
	0x00000000, /* 64 */
	0x00000000, /* 65 */
	0x00000000, /* 66 */
	0x00000000, /* 67 */
	0x00000000, /* 68 */
	0x00000000, /* 69 */
	0x00000000, /* 70 */
	0x00000000, /* 71 */
	0x00000000, /* 72 */
	0x00000000, /* 73 */
	0x00000000, /* 74 */
	0x00000000, /* 75 */
	0x00000000, /* 76 */
	0x00000000, /* 77 */
	0x00000000, /* 78 */
	0x00000000, /* 79 */
	0x00200012, /* 80 */
	0x00a00006, /* 81 */
	0x0085f001, /* 82 */
	0x00400001, /* 83 */
	0x00200101, /* 84 */
	0x0065a008, /* 85 */
	0x00000000, /* 86 */
	0x00000000, /* 87 */
	0x0020000b, /* 88 */
	0x00400801, /* 89 */
	0x0085c002, /* 90 */
	0x0030007a, /* 91 */
	0x00200019, /* 92 */
	0x0020102d, /* 93 */
	0x00308034, /* 94 */
	0x00400001, /* 95 */
	0x002000c5, /* 96 */
	0x00666008, /* 97 */
	0x00000000, /* 98 */
	0x00000000, /* 99 */
	0x0020000b, /* 100 */
	0x00400801, /* 101 */
	0x00868004, /* 102 */
	0x0030009a, /* 103 */
	0x00200035, /* 104 */
	0x00201031, /* 105 */
	0x00308034, /* 106 */
	0x00000000, /* 107 */
	0x00000000, /* 108 */
	0x00000000, /* 109 */
	0x00400806, /* 110 */
	0x00a00019, /* 111 */
	0x0068000f, /* 112 */
	0x00400801, /* 113 */
	0x00c08008, /* 114 */
	0x00c08008, /* 115 */
	0x00c01010, /* 116 */
	0x00c01010, /* 117 */
	0x00400928, /* 118 */
	0x00a00019, /* 119 */
	0x0068000f, /* 120 */
	0x00400801, /* 121 */
	0x00c08008, /* 122 */
	0x00c08008, /* 123 */
	0x00c01010, /* 124 */
	0x00c01010, /* 125 */
	0x00400923, /* 126 */
	0x0086e000, /* 127 */
	0x00400801, /* 128 */
	0x00c08008, /* 129 */
	0x00d01010, /* 130 */
	0x00000000, /* 131 */
	0x00000000, /* 132 */
	0x00000000, /* 133 */
	0x00000000, /* 134 */
	0x00000000, /* 135 */
	0x00000000, /* 136 */
	0x00000000, /* 137 */
	0x00000000, /* 138 */
	0x00000000, /* 139 */
	0x00000000, /* 140 */
	0x00000000, /* 141 */
	0x00000000, /* 142 */
	0x00000000, /* 143 */
	0x00000000, /* 144 */
	0x00000000, /* 145 */
	0x00000000, /* 146 */
	0x00000000, /* 147 */
	0x00000000, /* 148 */
	0x00000000, /* 149 */
	0x00000000, /* 150 */
	0x00000000, /* 151 */
	0x00000000, /* 152 */
	0x00000000, /* 153 */
	0x00000000, /* 154 */
	0x00000000, /* 155 */
	0x00000000, /* 156 */
	0x00000000, /* 157 */
	0x00000000, /* 158 */
	0x00000000, /* 159 */
	0x00000000, /* 160 */
	0x00000000, /* 161 */
	0x00000000, /* 162 */
	0x00000000, /* 163 */
	0x00000000, /* 164 */
	0x00000000, /* 165 */
	0x00000000, /* 166 */
	0x00000000, /* 167 */
	0x00000000, /* 168 */
	0x00000000, /* 169 */
	0x00000000, /* 170 */
	0x00000000, /* 171 */
	0x00000000, /* 172 */
	0x00000000, /* 173 */
	0x00000000, /* 174 */
	0x00000000, /* 175 */
	0x00000000, /* 176 */
	0x00000000, /* 177 */
	0x00000000, /* 178 */
	0x00000000, /* 179 */
	0x00000000, /* 180 */
	0x00000000, /* 181 */
	0x00000000, /* 182 */
	0x00000000, /* 183 */
	0x00000000, /* 184 */
	0x00000000, /* 185 */
	0x00000000, /* 186 */
	0x00000000, /* 187 */
	0x00000000, /* 188 */
	0x00000000, /* 189 */
	0x00000000, /* 190 */
	0x00000000, /* 191 */
	0x00000000, /* 192 */
	0x00000000, /* 193 */
	0x00000000, /* 194 */
	0x00000000, /* 195 */
	0x00000000, /* 196 */
	0x00000000, /* 197 */
	0x00000000, /* 198 */
	0x00000000, /* 199 */
	0x00400003, /* 200 */
	0x00400126, /* 201 */
	0x0040000a, /* 202 */
	0x006e1003, /* 203 */
	0x006dd002, /* 204 */
	0x00400003, /* 205 */
	0x00400127, /* 206 */
	0x0040000a, /* 207 */
	0x006e1003, /* 208 */
	0x006dd002, /* 209 */
	0x00400003, /* 210 */
	0x00400126, /* 211 */
	0x0040000a, /* 212 */
	0x006e1003, /* 213 */
	0x006dd002, /* 214 */
	0x00400003, /* 215 */
	0x00400127, /* 216 */
	0x0040000a, /* 217 */
	0x006e1003, /* 218 */
	0x006dd002, /* 219 */
	0x008c8000, /* 220 */
	0x00400001, /* 221 */
	0x0020003f, /* 222 */
	0x00202036, /* 223 */
	0x00300018, /* 224 */
	0x00400001, /* 225 */
	0x0020007f, /* 226 */
	0x00202036, /* 227 */
	0x00300018, /* 228 */
	0x00000000, /* 229 */
	0x00000000, /* 230 */
	0x00000000, /* 231 */
	0x00000000, /* 232 */
	0x00000000, /* 233 */
	0x00000000, /* 234 */
	0x00000000, /* 235 */
	0x00000000, /* 236 */
	0x00000000, /* 237 */
	0x00000000, /* 238 */
	0x00000000, /* 239 */
	0x00000000, /* 240 */
	0x00000000, /* 241 */
	0x00000000, /* 242 */
	0x00000000, /* 243 */
	0x00000000, /* 244 */
	0x00000000, /* 245 */
	0x00000000, /* 246 */
	0x00000000, /* 247 */
	0x00000000, /* 248 */
	0x00000000, /* 249 */
	0x00000000, /* 250 */
	0x00000000, /* 251 */
	0x00000000, /* 252 */
	0x008fd000, /* 253 */
	0x00060110, /* 254 */
	0xa099befb, /* 255 */
};

/* PAL_M it_micro_instruction is unique per ITU-BT.470-6 */
/* From vec_palm_macrovision_type_3.bss */
static const uint32_t s_aulRamBVBInput_PALM[BVDC_P_RAM_TABLE_SIZE] =
{
	0x0020400c, /* 0 */
	0x00a24000, /* 1 */
	0x00288059, /* 2 */
	0x0028001b, /* 3 */
	0x0024000c, /* 4 */
	0x00207040, /* 5 */
	0x00205040, /* 6 */
	0x0020400f, /* 7 */
	0x00214044, /* 8 */
	0x00204026, /* 9 */
	0x0020c00f, /* 10 */
	0x0028c1d2, /* 11 */
	0x00288075, /* 12 */
	0x002c800b, /* 13 */
	0x0028e040, /* 14 */
	0x0028c284, /* 15 */
	0x00801000, /* 16 */
	0x00000000, /* 17 */
	0x00000000, /* 18 */
	0x00000000, /* 19 */
	0x004e2001, /* 20 */
	0x00ad8000, /* 21 */
	0x00461001, /* 22 */
	0x00441005, /* 23 */
	0x00442006, /* 24 */
	0x00444004, /* 25 */
	0x00454014, /* 26 */
	0x00214018, /* 27 */
	0x0041c1e2, /* 28 */
	0x0021c018, /* 29 */
	0x00404001, /* 30 */
	0x0020200f, /* 31 */
	0x00242010, /* 32 */
	0x00442006, /* 33 */
	0x004c1006, /* 34 */
	0x004c2005, /* 35 */
	0x004c4004, /* 36 */
	0x004d4016, /* 37 */
	0x0049c1e0, /* 38 */
	0x0029c018, /* 39 */
	0x004cc002, /* 40 */
	0x004c2005, /* 41 */
	0x004e2001, /* 42 */
	0x00ad8000, /* 43 */
	0x00461001, /* 44 */
	0x00441005, /* 45 */
	0x00442006, /* 46 */
	0x00444002, /* 47 */
	0x00454016, /* 48 */
	0x00214018, /* 49 */
	0x0041c1e0, /* 50 */
	0x0021c018, /* 51 */
	0x0040c002, /* 52 */
	0x00404001, /* 53 */
	0x0020200f, /* 54 */
	0x00242010, /* 55 */
	0x00442006, /* 56 */
	0x004c1006, /* 57 */
	0x004c2005, /* 58 */
	0x004c4002, /* 59 */
	0x004d4018, /* 60 */
	0x0049c1e2, /* 61 */
	0x0029c018, /* 62 */
	0x004c2005, /* 63 */
	0x004e2001, /* 64 */
	0x00815000, /* 65 */
	0x00000000, /* 66 */
	0x00000000, /* 67 */
	0x00000000, /* 68 */
	0x00000000, /* 69 */
	0x00000000, /* 70 */
	0x00000000, /* 71 */
	0x00000000, /* 72 */
	0x00000000, /* 73 */
	0x00000000, /* 74 */
	0x00000000, /* 75 */
	0x00000000, /* 76 */
	0x00000000, /* 77 */
	0x00000000, /* 78 */
	0x00000000, /* 79 */
	0x00200012, /* 80 */
	0x00a00006, /* 81 */
	0x0085c001, /* 82 */
	0x00400801, /* 83 */
	0x0020005a, /* 84 */
	0x00657008, /* 85 */
	0x00400801, /* 86 */
	0x00859002, /* 87 */
	0x003000db, /* 88 */
	0x0020103d, /* 89 */
	0x00208052, /* 90 */
	0x0030004c, /* 91 */
	0x00400801, /* 92 */
	0x0020005a, /* 93 */
	0x00660008, /* 94 */
	0x00400801, /* 95 */
	0x00862004, /* 96 */
	0x003000db, /* 97 */
	0x0020103d, /* 98 */
	0x00208052, /* 99 */
	0x0030004c, /* 100 */
	0x00000000, /* 101 */
	0x00000000, /* 102 */
	0x00000000, /* 103 */
	0x00000000, /* 104 */
	0x00400803, /* 105 */
	0x00a00019, /* 106 */
	0x0067b00f, /* 107 */
	0x00400801, /* 108 */
	0x00c08008, /* 109 */
	0x00c08008, /* 110 */
	0x00c01010, /* 111 */
	0x00c01010, /* 112 */
	0x004008f7, /* 113 */
	0x00a00019, /* 114 */
	0x0067b00f, /* 115 */
	0x00400801, /* 116 */
	0x00c08008, /* 117 */
	0x00c08008, /* 118 */
	0x00c01010, /* 119 */
	0x00c01010, /* 120 */
	0x004008f3, /* 121 */
	0x00869000, /* 122 */
	0x00400801, /* 123 */
	0x00c08008, /* 124 */
	0x00d01010, /* 125 */
	0x00000000, /* 126 */
	0x00000000, /* 127 */
	0x00000000, /* 128 */
	0x00000000, /* 129 */
	0x00000000, /* 130 */
	0x00000000, /* 131 */
	0x00000000, /* 132 */
	0x00000000, /* 133 */
	0x00000000, /* 134 */
	0x00000000, /* 135 */
	0x00000000, /* 136 */
	0x00000000, /* 137 */
	0x00000000, /* 138 */
	0x00000000, /* 139 */
	0x00000000, /* 140 */
	0x00000000, /* 141 */
	0x00000000, /* 142 */
	0x00000000, /* 143 */
	0x00000000, /* 144 */
	0x00000000, /* 145 */
	0x00000000, /* 146 */
	0x00000000, /* 147 */
	0x00000000, /* 148 */
	0x00000000, /* 149 */
	0x00000000, /* 150 */
	0x00000000, /* 151 */
	0x00000000, /* 152 */
	0x004000fe, /* 153 */
	0x00400003, /* 154 */
	0x006a6002, /* 155 */
	0x006a2003, /* 156 */
	0x004000fe, /* 157 */
	0x00400003, /* 158 */
	0x006a6002, /* 159 */
	0x006a2003, /* 160 */
	0x00899000, /* 161 */
	0x00400001, /* 162 */
	0x0020003f, /* 163 */
	0x00202036, /* 164 */
	0x00300018, /* 165 */
	0x00400001, /* 166 */
	0x0020007f, /* 167 */
	0x00202036, /* 168 */
	0x00300018, /* 169 */
	0x00000000, /* 170 */
	0x00000000, /* 171 */
	0x00000000, /* 172 */
	0x00000000, /* 173 */
	0x00000000, /* 174 */
	0x004000fe, /* 175 */
	0x00400004, /* 176 */
	0x006bc001, /* 177 */
	0x006b8003, /* 178 */
	0x004000fe, /* 179 */
	0x00400004, /* 180 */
	0x006bc001, /* 181 */
	0x006b8003, /* 182 */
	0x008af000, /* 183 */
	0x00400001, /* 184 */
	0x0020003f, /* 185 */
	0x00202036, /* 186 */
	0x00300018, /* 187 */
	0x00400001, /* 188 */
	0x0020007f, /* 189 */
	0x00202036, /* 190 */
	0x00300018, /* 191 */
	0x00000000, /* 192 */
	0x00000000, /* 193 */
	0x00000000, /* 194 */
	0x00000000, /* 195 */
	0x00000000, /* 196 */
	0x00000000, /* 197 */
	0x00000000, /* 198 */
	0x00000000, /* 199 */
	0x00000000, /* 200 */
	0x00000000, /* 201 */
	0x00000000, /* 202 */
	0x00000000, /* 203 */
	0x00000000, /* 204 */
	0x00000000, /* 205 */
	0x00000000, /* 206 */
	0x00000000, /* 207 */
	0x00000000, /* 208 */
	0x00000000, /* 209 */
	0x00000000, /* 210 */
	0x00000000, /* 211 */
	0x00000000, /* 212 */
	0x00000000, /* 213 */
	0x00000000, /* 214 */
	0x00000000, /* 215 */
	0x00000000, /* 216 */
	0x00000000, /* 217 */
	0x00000000, /* 218 */
	0x00000000, /* 219 */
	0x00000000, /* 220 */
	0x00000000, /* 221 */
	0x00000000, /* 222 */
	0x00000000, /* 223 */
	0x00000000, /* 224 */
	0x004000fe, /* 225 */
	0x00400002, /* 226 */
	0x006ee003, /* 227 */
	0x006ea003, /* 228 */
	0x004000fe, /* 229 */
	0x00400002, /* 230 */
	0x006ee003, /* 231 */
	0x006ea003, /* 232 */
	0x008e1000, /* 233 */
	0x00400001, /* 234 */
	0x0020003f, /* 235 */
	0x00202036, /* 236 */
	0x00300018, /* 237 */
	0x00400001, /* 238 */
	0x0020007f, /* 239 */
	0x00202036, /* 240 */
	0x00300018, /* 241 */
	0x00000000, /* 242 */
	0x00000000, /* 243 */
	0x00000000, /* 244 */
	0x00000000, /* 245 */
	0x00000000, /* 246 */
	0x00000000, /* 247 */
	0x00000000, /* 248 */
	0x00000000, /* 249 */
	0x00000000, /* 250 */
	0x00000000, /* 251 */
	0x00000000, /* 252 */
	0x008fd000, /* 253 */
	0x00111109, /* 254 */
	0x8847d8c8, /* 255 */
};

/* SECAM it_micro_instruction */
/* From vec_secam_macrovision.bss */
static const uint32_t s_aulRamBVBInput_SECAM[BVDC_P_RAM_TABLE_SIZE] =
#ifdef BVDC_P_WSE_VER3
{
	0x0020400c, /* 0 */
	0x00a04000, /* 1 */
	0x00218055, /* 2 */
	0x0020001f, /* 3 */
	0x0024000c, /* 4 */
	0x00207040, /* 5 */
	0x00205040, /* 6 */
	0x0020401a, /* 7 */
	0x00214055, /* 8 */
	0x00812080, /* 9 */
	0x00214022, /* 10 */
	0x0021c00b, /* 11 */
	0x0021c1b9, /* 12 */
	0x00218075, /* 13 */
	0x0025800b, /* 14 */
	0x0021e040, /* 15 */
	0x0021c28a, /* 16 */
	0x00801000, /* 17 */
	0x00294022, /* 18 */
	0x0029c00b, /* 19 */
	0x0029c1b9, /* 20 */
	0x00298075, /* 21 */
	0x002d800b, /* 22 */
	0x0029e040, /* 23 */
	0x0029c295, /* 24 */
	0x00a04000, /* 25 */
	0x00298055, /* 26 */
	0x00280014, /* 27 */
	0x00804000, /* 28 */
	0x00000000, /* 29 */
	0x00462003, /* 30 */
	0x00a98000, /* 31 */
	0x00461001, /* 32 */
	0x00441004, /* 33 */
	0x00442003, /* 34 */
	0x00a00080, /* 35 */
	0x00442002, /* 36 */
	0x00444002, /* 37 */
	0x00454002, /* 38 */
	0x00a00080, /* 39 */
	0x00454010, /* 40 */
	0x00444010, /* 41 */
	0x0040c23e, /* 42 */
	0x00442005, /* 43 */
	0x004c1005, /* 44 */
	0x004c2004, /* 45 */
	0x00a00080, /* 46 */
	0x004c2001, /* 47 */
	0x004c4003, /* 48 */
	0x002d400b, /* 49 */
	0x00a00080, /* 50 */
	0x004d4012, /* 51 */
	0x004c400e, /* 52 */
	0x0048c23e, /* 53 */
	0x0048c001, /* 54 */
	0x00442004, /* 55 */
	0x00462001, /* 56 */
	0x0081f000, /* 57 */
	0x00000000, /* 58 */
	0x00000000, /* 59 */
	0x00400003, /* 60 */
	0x00409002, /* 61 */
	0x00400002, /* 62 */
	0x0083d000, /* 63 */
	0x00000000, /* 64 */
	0x00000000, /* 65 */
	0x00000000, /* 66 */
	0x00000000, /* 67 */
	0x00000000, /* 68 */
	0x00000000, /* 69 */
	0x00000000, /* 70 */
	0x00000000, /* 71 */
	0x00000000, /* 72 */
	0x00000000, /* 73 */
	0x00000000, /* 74 */
	0x00000000, /* 75 */
	0x00000000, /* 76 */
	0x00000000, /* 77 */
	0x00000000, /* 78 */
	0x00000000, /* 79 */
	0x00200012, /* 80 */
	0x00a00006, /* 81 */
	0x0085f001, /* 82 */
	0x00400001, /* 83 */
	0x00200101, /* 84 */
	0x0065a008, /* 85 */
	0x00000000, /* 86 */
	0x00000000, /* 87 */
	0x0020000b, /* 88 */
	0x00400801, /* 89 */
	0x0085c002, /* 90 */
	0x0030007a, /* 91 */
	0x00200019, /* 92 */
	0x0020102d, /* 93 */
	0x00308034, /* 94 */
	0x00400001, /* 95 */
	0x002000c5, /* 96 */
	0x00666008, /* 97 */
	0x00000000, /* 98 */
	0x00000000, /* 99 */
	0x0020000b, /* 100 */
	0x00400801, /* 101 */
	0x00868004, /* 102 */
	0x0030009a, /* 103 */
	0x00200035, /* 104 */
	0x00201031, /* 105 */
	0x00308034, /* 106 */
	0x00000000, /* 107 */
	0x00000000, /* 108 */
	0x00000000, /* 109 */
	0x00400806, /* 110 */
	0x00a00019, /* 111 */
	0x0068000f, /* 112 */
	0x00400801, /* 113 */
	0x00c08008, /* 114 */
	0x00c08008, /* 115 */
	0x00c01010, /* 116 */
	0x00c01010, /* 117 */
	0x00400928, /* 118 */
	0x00a00019, /* 119 */
	0x0068000f, /* 120 */
	0x00400801, /* 121 */
	0x00c08008, /* 122 */
	0x00c08008, /* 123 */
	0x00c01010, /* 124 */
	0x00c01010, /* 125 */
	0x00400923, /* 126 */
	0x0086e000, /* 127 */
	0x00400801, /* 128 */
	0x00c08008, /* 129 */
	0x00d01010, /* 130 */
	0x00000000, /* 131 */
	0x00400002, /* 132 */
	0x00400126, /* 133 */
	0x00400009, /* 134 */
	0x00693004, /* 135 */
	0x0068f002, /* 136 */
	0x00400003, /* 137 */
	0x00400127, /* 138 */
	0x00400009, /* 139 */
	0x00693004, /* 140 */
	0x0068f002, /* 141 */
	0x00884000, /* 142 */
	0x00400001, /* 143 */
	0x0020003f, /* 144 */
	0x00204036, /* 145 */
	0x00300018, /* 146 */
	0x00400001, /* 147 */
	0x0020007f, /* 148 */
	0x00204036, /* 149 */
	0x00300018, /* 150 */
	0x00000000, /* 151 */
	0x00000000, /* 152 */
	0x00000000, /* 153 */
	0x00000000, /* 154 */
	0x00000000, /* 155 */
	0x00000000, /* 156 */
	0x00000000, /* 157 */
	0x00000000, /* 158 */
	0x00000000, /* 159 */
	0x00000000, /* 160 */
	0x00000000, /* 161 */
	0x00000000, /* 162 */
	0x00000000, /* 163 */
	0x00000000, /* 164 */
	0x00000000, /* 165 */
	0x00000000, /* 166 */
	0x00000000, /* 167 */
	0x00000000, /* 168 */
	0x00000000, /* 169 */
	0x00000000, /* 170 */
	0x00000000, /* 171 */
	0x00000000, /* 172 */
	0x00000000, /* 173 */
	0x00000000, /* 174 */
	0x00000000, /* 175 */
	0x00000000, /* 176 */
	0x00000000, /* 177 */
	0x00000000, /* 178 */
	0x00000000, /* 179 */
	0x00000000, /* 180 */
	0x00000000, /* 181 */
	0x00000000, /* 182 */
	0x00000000, /* 183 */
	0x00000000, /* 184 */
	0x00000000, /* 185 */
	0x00000000, /* 186 */
	0x00000000, /* 187 */
	0x00000000, /* 188 */
	0x00000000, /* 189 */
	0x00000000, /* 190 */
	0x00000000, /* 191 */
	0x00000000, /* 192 */
	0x00000000, /* 193 */
	0x00000000, /* 194 */
	0x00000000, /* 195 */
	0x00000000, /* 196 */
	0x00000000, /* 197 */
	0x00000000, /* 198 */
	0x00000000, /* 199 */
	0x00000000, /* 200 */
	0x00000000, /* 201 */
	0x00000000, /* 202 */
	0x00000000, /* 203 */
	0x00000000, /* 204 */
	0x00000000, /* 205 */
	0x00000000, /* 206 */
	0x00000000, /* 207 */
	0x00000000, /* 208 */
	0x00000000, /* 209 */
	0x00000000, /* 210 */
	0x00000000, /* 211 */
	0x00000000, /* 212 */
	0x00000000, /* 213 */
	0x00000000, /* 214 */
	0x00000000, /* 215 */
	0x00000000, /* 216 */
	0x00000000, /* 217 */
	0x00000000, /* 218 */
	0x00000000, /* 219 */
	0x00000000, /* 220 */
	0x00000000, /* 221 */
	0x00000000, /* 222 */
	0x00000000, /* 223 */
	0x00000000, /* 224 */
	0x00000000, /* 225 */
	0x00000000, /* 226 */
	0x00000000, /* 227 */
	0x00000000, /* 228 */
	0x00000000, /* 229 */
	0x00000000, /* 230 */
	0x00000000, /* 231 */
	0x00000000, /* 232 */
	0x00000000, /* 233 */
	0x00000000, /* 234 */
	0x00000000, /* 235 */
	0x00000000, /* 236 */
	0x00000000, /* 237 */
	0x00000000, /* 238 */
	0x00000000, /* 239 */
	0x00000000, /* 240 */
	0x00000000, /* 241 */
	0x00000000, /* 242 */
	0x00000000, /* 243 */
	0x00000000, /* 244 */
	0x00000000, /* 245 */
	0x00000000, /* 246 */
	0x00000000, /* 247 */
	0x00000000, /* 248 */
	0x00000000, /* 249 */
	0x00000000, /* 250 */
	0x00000000, /* 251 */
	0x00000000, /* 252 */
	0x008fd000, /* 253 */
	0x00062310, /* 254 */
	0x3521f20b, /* 255 */
};
#else
{
	0x0020400c, /* 0 */
	0x00a04000, /* 1 */
	0x00298055, /* 2 */
	0x0028001f, /* 3 */
	0x0024000c, /* 4 */
	0x00207040, /* 5 */
	0x00205040, /* 6 */
	0x0020401a, /* 7 */
	0x00214060, /* 8 */
	0x00294022, /* 9 */
	0x0029c00b, /* 10 */
	0x0029c1b9, /* 11 */
	0x00298075, /* 12 */
	0x002d800b, /* 13 */
	0x0029e040, /* 14 */
	0x0029c28a, /* 15 */
	0x00801000, /* 16 */
	0x00000000, /* 17 */
	0x00000000, /* 18 */
	0x00000000, /* 19 */
	0x00462003, /* 20 */
	0x00a98000, /* 21 */
	0x00461001, /* 22 */
	0x00441004, /* 23 */
	0x00442005, /* 24 */
	0x00444002, /* 25 */
	0x00454012, /* 26 */
	0x00444010, /* 27 */
	0x0040c23e, /* 28 */
	0x00442005, /* 29 */
	0x004c1005, /* 30 */
	0x004c2004, /* 31 */
	0x004c2001, /* 32 */
	0x004c4003, /* 33 */
	0x004d4012, /* 34 */
	0x004c400e, /* 35 */
	0x0048c23e, /* 36 */
	0x0048c001, /* 37 */
	0x00442004, /* 38 */
	0x00462001, /* 39 */
	0x00815000, /* 40 */
	0x00000000, /* 41 */
	0x00000000, /* 42 */
	0x00000000, /* 43 */
	0x00000000, /* 44 */
	0x00000000, /* 45 */
	0x00000000, /* 46 */
	0x00000000, /* 47 */
	0x00000000, /* 48 */
	0x00000000, /* 49 */
	0x00400003, /* 50 */
	0x00409002, /* 51 */
	0x00400002, /* 52 */
	0x00833000, /* 53 */
	0x00000000, /* 54 */
	0x00000000, /* 55 */
	0x00000000, /* 56 */
	0x00000000, /* 57 */
	0x00000000, /* 58 */
	0x00000000, /* 59 */
	0x00000000, /* 60 */
	0x00000000, /* 61 */
	0x00000000, /* 62 */
	0x00000000, /* 63 */
	0x00000000, /* 64 */
	0x00000000, /* 65 */
	0x00000000, /* 66 */
	0x00000000, /* 67 */
	0x00000000, /* 68 */
	0x00000000, /* 69 */
	0x00000000, /* 70 */
	0x00000000, /* 71 */
	0x00000000, /* 72 */
	0x00000000, /* 73 */
	0x00000000, /* 74 */
	0x00000000, /* 75 */
	0x00000000, /* 76 */
	0x00000000, /* 77 */
	0x00000000, /* 78 */
	0x00000000, /* 79 */
	0x00200012, /* 80 */
	0x00a00006, /* 81 */
	0x0085f001, /* 82 */
	0x00400001, /* 83 */
	0x00200101, /* 84 */
	0x0065a008, /* 85 */
	0x00000000, /* 86 */
	0x00000000, /* 87 */
	0x0020000b, /* 88 */
	0x00400801, /* 89 */
	0x0085c002, /* 90 */
	0x0030007a, /* 91 */
	0x00200019, /* 92 */
	0x0020102d, /* 93 */
	0x00308034, /* 94 */
	0x00400001, /* 95 */
	0x002000c5, /* 96 */
	0x00666008, /* 97 */
	0x00000000, /* 98 */
	0x00000000, /* 99 */
	0x0020000b, /* 100 */
	0x00400801, /* 101 */
	0x00868004, /* 102 */
	0x0030009a, /* 103 */
	0x00200035, /* 104 */
	0x00201031, /* 105 */
	0x00308034, /* 106 */
	0x00000000, /* 107 */
	0x00000000, /* 108 */
	0x00000000, /* 109 */
	0x00400806, /* 110 */
	0x00a00019, /* 111 */
	0x0068000f, /* 112 */
	0x00400801, /* 113 */
	0x00c08008, /* 114 */
	0x00c08008, /* 115 */
	0x00c01010, /* 116 */
	0x00c01010, /* 117 */
	0x00400928, /* 118 */
	0x00a00019, /* 119 */
	0x0068000f, /* 120 */
	0x00400801, /* 121 */
	0x00c08008, /* 122 */
	0x00c08008, /* 123 */
	0x00c01010, /* 124 */
	0x00c01010, /* 125 */
	0x00400923, /* 126 */
	0x0086e000, /* 127 */
	0x00400801, /* 128 */
	0x00c08008, /* 129 */
	0x00d01010, /* 130 */
	0x00000000, /* 131 */
	0x00400002, /* 132 */
	0x00400126, /* 133 */
	0x00400009, /* 134 */
	0x00693004, /* 135 */
	0x0068f002, /* 136 */
	0x00400003, /* 137 */
	0x00400127, /* 138 */
	0x00400009, /* 139 */
	0x00693004, /* 140 */
	0x0068f002, /* 141 */
	0x00884000, /* 142 */
	0x00400001, /* 143 */
	0x0020003f, /* 144 */
	0x00204036, /* 145 */
	0x00300018, /* 146 */
	0x00400001, /* 147 */
	0x0020007f, /* 148 */
	0x00204036, /* 149 */
	0x00300018, /* 150 */
	0x00000000, /* 151 */
	0x00000000, /* 152 */
	0x00000000, /* 153 */
	0x00000000, /* 154 */
	0x00000000, /* 155 */
	0x00000000, /* 156 */
	0x00000000, /* 157 */
	0x00000000, /* 158 */
	0x00000000, /* 159 */
	0x00000000, /* 160 */
	0x00000000, /* 161 */
	0x00000000, /* 162 */
	0x00000000, /* 163 */
	0x00000000, /* 164 */
	0x00000000, /* 165 */
	0x00000000, /* 166 */
	0x00000000, /* 167 */
	0x00000000, /* 168 */
	0x00000000, /* 169 */
	0x00000000, /* 170 */
	0x00000000, /* 171 */
	0x00000000, /* 172 */
	0x00000000, /* 173 */
	0x00000000, /* 174 */
	0x00000000, /* 175 */
	0x00000000, /* 176 */
	0x00000000, /* 177 */
	0x00000000, /* 178 */
	0x00000000, /* 179 */
	0x00000000, /* 180 */
	0x00000000, /* 181 */
	0x00000000, /* 182 */
	0x00000000, /* 183 */
	0x00000000, /* 184 */
	0x00000000, /* 185 */
	0x00000000, /* 186 */
	0x00000000, /* 187 */
	0x00000000, /* 188 */
	0x00000000, /* 189 */
	0x00000000, /* 190 */
	0x00000000, /* 191 */
	0x00000000, /* 192 */
	0x00000000, /* 193 */
	0x00000000, /* 194 */
	0x00000000, /* 195 */
	0x00000000, /* 196 */
	0x00000000, /* 197 */
	0x00000000, /* 198 */
	0x00000000, /* 199 */
	0x00000000, /* 200 */
	0x00000000, /* 201 */
	0x00000000, /* 202 */
	0x00000000, /* 203 */
	0x00000000, /* 204 */
	0x00000000, /* 205 */
	0x00000000, /* 206 */
	0x00000000, /* 207 */
	0x00000000, /* 208 */
	0x00000000, /* 209 */
	0x00000000, /* 210 */
	0x00000000, /* 211 */
	0x00000000, /* 212 */
	0x00000000, /* 213 */
	0x00000000, /* 214 */
	0x00000000, /* 215 */
	0x00000000, /* 216 */
	0x00000000, /* 217 */
	0x00000000, /* 218 */
	0x00000000, /* 219 */
	0x00000000, /* 220 */
	0x00000000, /* 221 */
	0x00000000, /* 222 */
	0x00000000, /* 223 */
	0x00000000, /* 224 */
	0x00000000, /* 225 */
	0x00000000, /* 226 */
	0x00000000, /* 227 */
	0x00000000, /* 228 */
	0x00000000, /* 229 */
	0x00000000, /* 230 */
	0x00000000, /* 231 */
	0x00000000, /* 232 */
	0x00000000, /* 233 */
	0x00000000, /* 234 */
	0x00000000, /* 235 */
	0x00000000, /* 236 */
	0x00000000, /* 237 */
	0x00000000, /* 238 */
	0x00000000, /* 239 */
	0x00000000, /* 240 */
	0x00000000, /* 241 */
	0x00000000, /* 242 */
	0x00000000, /* 243 */
	0x00000000, /* 244 */
	0x00000000, /* 245 */
	0x00000000, /* 246 */
	0x00000000, /* 247 */
	0x00000000, /* 248 */
	0x00000000, /* 249 */
	0x00000000, /* 250 */
	0x00000000, /* 251 */
	0x00000000, /* 252 */
	0x008fd000, /* 253 */
	0x00060110, /* 254 */
	0x7a14c858, /* 255 */
};
#endif

/* vec_1080i_bvb_input.bss */
static const uint32_t s_aulRamBVBInput_1080i[BVDC_P_RAM_TABLE_SIZE] =
{
	0x00a10000, /* 0 */
	0x0021702c, /* 1 */
	0x0024002c, /* 2 */
	0x00200058, /* 3 */
	0x0028403c, /* 4 */
	0x0028c334, /* 5 */
	0x0028802c, /* 6 */
	0x0029e02c, /* 7 */
	0x0028802c, /* 8 */
	0x00288058, /* 9 */
	0x0028c36e, /* 10 */
	0x00200018, /* 11 */
	0x00800000, /* 12 */
	0x00000000, /* 13 */
	0x00000000, /* 14 */
	0x00000000, /* 15 */
	0x00000000, /* 16 */
	0x00000000, /* 17 */
	0x00000000, /* 18 */
	0x00000000, /* 19 */
	0x00470003, /* 20 */
	0x00a80001, /* 21 */
	0x00471001, /* 22 */
	0x00451009, /* 23 */
	0x00452002, /* 24 */
	0x0044401c, /* 25 */
	0x0040c438, /* 26 */
	0x00444004, /* 27 */
	0x00452001, /* 28 */
	0x00825001, /* 29 */
	0x004d100a, /* 30 */
	0x004d2001, /* 31 */
	0x004c401e, /* 32 */
	0x0048c438, /* 33 */
	0x004c4003, /* 34 */
	0x004e4001, /* 35 */
	0x00815000, /* 36 */
	0x004d100a, /* 37 */
	0x004d2002, /* 38 */
	0x004c401c, /* 39 */
	0x0048c438, /* 40 */
	0x004c4004, /* 41 */
	0x004e4001, /* 42 */
	0x00000000, /* 43 */
	0x00000000, /* 44 */
	0x00000000, /* 45 */
	0x00000000, /* 46 */
	0x00000000, /* 47 */
	0x00000000, /* 48 */
	0x00000000, /* 49 */
	0x00400001, /* 50 */
	0x00202052, /* 51 */
	0x00200427, /* 52 */
	0x0020102c, /* 53 */
	0x0020000b, /* 54 */
	0x00832000, /* 55 */
	0x00000000, /* 56 */
	0x00000000, /* 57 */
	0x00000000, /* 58 */
	0x00000000, /* 59 */
	0x00000000, /* 60 */
	0x00000000, /* 61 */
	0x00000000, /* 62 */
	0x00000000, /* 63 */
	0x00000000, /* 64 */
	0x00000000, /* 65 */
	0x00000000, /* 66 */
	0x00000000, /* 67 */
	0x00000000, /* 68 */
	0x00000000, /* 69 */
	0x00000000, /* 70 */
	0x00000000, /* 71 */
	0x00000000, /* 72 */
	0x00000000, /* 73 */
	0x00000000, /* 74 */
	0x00000000, /* 75 */
	0x00000000, /* 76 */
	0x00000000, /* 77 */
	0x00000000, /* 78 */
	0x00000000, /* 79 */
	0x00000000, /* 80 */
	0x00000000, /* 81 */
	0x00000000, /* 82 */
	0x00000000, /* 83 */
	0x00000000, /* 84 */
	0x00000000, /* 85 */
	0x00000000, /* 86 */
	0x00000000, /* 87 */
	0x00000000, /* 88 */
	0x00000000, /* 89 */
	0x00000000, /* 90 */
	0x00000000, /* 91 */
	0x00000000, /* 92 */
	0x00000000, /* 93 */
	0x00000000, /* 94 */
	0x00000000, /* 95 */
	0x00000000, /* 96 */
	0x00000000, /* 97 */
	0x00000000, /* 98 */
	0x00000000, /* 99 */
	0x00000000, /* 100 */
	0x00000000, /* 101 */
	0x00000000, /* 102 */
	0x00000000, /* 103 */
	0x00000000, /* 104 */
	0x00000000, /* 105 */
	0x00000000, /* 106 */
	0x00000000, /* 107 */
	0x00000000, /* 108 */
	0x00000000, /* 109 */
	0x00000000, /* 110 */
	0x00000000, /* 111 */
	0x00000000, /* 112 */
	0x00000000, /* 113 */
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
	0x00000000, /* 126 */
	0x00000000, /* 127 */
	0x00000000, /* 128 */
	0x00000000, /* 129 */
	0x00000000, /* 130 */
	0x00000000, /* 131 */
	0x00000000, /* 132 */
	0x00000000, /* 133 */
	0x00000000, /* 134 */
	0x00000000, /* 135 */
	0x00000000, /* 136 */
	0x00000000, /* 137 */
	0x00000000, /* 138 */
	0x00000000, /* 139 */
	0x00000000, /* 140 */
	0x00000000, /* 141 */
	0x00000000, /* 142 */
	0x00000000, /* 143 */
	0x00000000, /* 144 */
	0x00000000, /* 145 */
	0x00000000, /* 146 */
	0x00000000, /* 147 */
	0x00000000, /* 148 */
	0x00000000, /* 149 */
	0x00000000, /* 150 */
	0x00000000, /* 151 */
	0x00000000, /* 152 */
	0x00000000, /* 153 */
	0x00000000, /* 154 */
	0x00000000, /* 155 */
	0x00000000, /* 156 */
	0x00000000, /* 157 */
	0x00000000, /* 158 */
	0x00000000, /* 159 */
	0x00000000, /* 160 */
	0x00000000, /* 161 */
	0x00000000, /* 162 */
	0x00000000, /* 163 */
	0x00000000, /* 164 */
	0x00000000, /* 165 */
	0x00000000, /* 166 */
	0x00000000, /* 167 */
	0x00000000, /* 168 */
	0x00000000, /* 169 */
	0x00000000, /* 170 */
	0x00000000, /* 171 */
	0x00000000, /* 172 */
	0x00000000, /* 173 */
	0x00000000, /* 174 */
	0x00000000, /* 175 */
	0x00000000, /* 176 */
	0x00000000, /* 177 */
	0x00000000, /* 178 */
	0x00000000, /* 179 */
	0x00000000, /* 180 */
	0x00000000, /* 181 */
	0x00000000, /* 182 */
	0x00000000, /* 183 */
	0x00000000, /* 184 */
	0x00000000, /* 185 */
	0x00000000, /* 186 */
	0x00000000, /* 187 */
	0x00000000, /* 188 */
	0x00000000, /* 189 */
	0x00000000, /* 190 */
	0x00000000, /* 191 */
	0x00000000, /* 192 */
	0x00000000, /* 193 */
	0x00000000, /* 194 */
	0x00000000, /* 195 */
	0x00000000, /* 196 */
	0x00000000, /* 197 */
	0x00000000, /* 198 */
	0x00000000, /* 199 */
	0x00000000, /* 200 */
	0x00000000, /* 201 */
	0x00000000, /* 202 */
	0x00000000, /* 203 */
	0x00000000, /* 204 */
	0x00000000, /* 205 */
	0x00000000, /* 206 */
	0x00000000, /* 207 */
	0x00000000, /* 208 */
	0x00000000, /* 209 */
	0x00000000, /* 210 */
	0x00000000, /* 211 */
	0x00000000, /* 212 */
	0x00000000, /* 213 */
	0x00000000, /* 214 */
	0x00000000, /* 215 */
	0x00000000, /* 216 */
	0x00000000, /* 217 */
	0x00000000, /* 218 */
	0x00000000, /* 219 */
	0x00000000, /* 220 */
	0x00000000, /* 221 */
	0x00000000, /* 222 */
	0x00000000, /* 223 */
	0x00000000, /* 224 */
	0x00000000, /* 225 */
	0x00000000, /* 226 */
	0x00000000, /* 227 */
	0x00000000, /* 228 */
	0x00000000, /* 229 */
	0x00000000, /* 230 */
	0x00000000, /* 231 */
	0x00000000, /* 232 */
	0x00000000, /* 233 */
	0x00000000, /* 234 */
	0x00000000, /* 235 */
	0x00000000, /* 236 */
	0x00000000, /* 237 */
	0x00000000, /* 238 */
	0x00000000, /* 239 */
	0x00000000, /* 240 */
	0x00000000, /* 241 */
	0x00000000, /* 242 */
	0x00000000, /* 243 */
	0x00000000, /* 244 */
	0x00000000, /* 245 */
	0x00000000, /* 246 */
	0x00000000, /* 247 */
	0x00000000, /* 248 */
	0x00000000, /* 249 */
	0x00000000, /* 250 */
	0x00000000, /* 251 */
	0x00000000, /* 252 */
	0x008fd000, /* 253 */
	0x00020810, /* 254 */
	0x270e0f91, /* 255 */
};

/* vec_1080p24hz_bvb_input.bss */
static const uint32_t s_aulRamBVBInput_1080p_24hz[BVDC_P_RAM_TABLE_SIZE] =
{
	0x00a30000, /* 0 */
	0x0020702c, /* 1 */
	0x0024002c, /* 2 */
	0x00200058, /* 3 */
	0x00204010, /* 4 */
	0x0028402c, /* 5 */
	0x0028c023, /* 6 */
	0x0028c75d, /* 7 */
	0x00200226, /* 8 */
	0x00210016, /* 9 */
	0x00800000, /* 10 */
	0x00000000, /* 11 */
	0x00000000, /* 12 */
	0x00000000, /* 13 */
	0x00000000, /* 14 */
	0x00000000, /* 15 */
	0x00000000, /* 16 */
	0x00000000, /* 17 */
	0x00000000, /* 18 */
	0x00000000, /* 19 */
	0x00a80000, /* 20 */
	0x00471001, /* 21 */
	0x00451004, /* 22 */
	0x00444024, /* 23 */
	0x0040c438, /* 24 */
	0x00444004, /* 25 */
	0x00814000, /* 26 */
	0x00000000, /* 27 */
	0x00000000, /* 28 */
	0x00000000, /* 29 */
	0x00000000, /* 30 */
	0x00000000, /* 31 */
	0x00000000, /* 32 */
	0x00000000, /* 33 */
	0x00000000, /* 34 */
	0x00000000, /* 35 */
	0x00000000, /* 36 */
	0x00000000, /* 37 */
	0x00000000, /* 38 */
	0x00000000, /* 39 */
	0x00000000, /* 40 */
	0x00000000, /* 41 */
	0x00000000, /* 42 */
	0x00000000, /* 43 */
	0x00000000, /* 44 */
	0x00000000, /* 45 */
	0x00000000, /* 46 */
	0x00000000, /* 47 */
	0x00000000, /* 48 */
	0x00000000, /* 49 */
	0x00400001, /* 50 */
	0x00202052, /* 51 */
	0x00200538, /* 52 */
	0x0020102c, /* 53 */
	0x0020000b, /* 54 */
	0x00832000, /* 55 */
	0x00000000, /* 56 */
	0x00000000, /* 57 */
	0x00000000, /* 58 */
	0x00000000, /* 59 */
	0x00000000, /* 60 */
	0x00000000, /* 61 */
	0x00000000, /* 62 */
	0x00000000, /* 63 */
	0x00000000, /* 64 */
	0x00000000, /* 65 */
	0x00000000, /* 66 */
	0x00000000, /* 67 */
	0x00000000, /* 68 */
	0x00000000, /* 69 */
	0x00000000, /* 70 */
	0x00000000, /* 71 */
	0x00000000, /* 72 */
	0x00000000, /* 73 */
	0x00000000, /* 74 */
	0x00000000, /* 75 */
	0x00000000, /* 76 */
	0x00000000, /* 77 */
	0x00000000, /* 78 */
	0x00000000, /* 79 */
	0x00000000, /* 80 */
	0x00000000, /* 81 */
	0x00000000, /* 82 */
	0x00000000, /* 83 */
	0x00000000, /* 84 */
	0x00000000, /* 85 */
	0x00000000, /* 86 */
	0x00000000, /* 87 */
	0x00000000, /* 88 */
	0x00000000, /* 89 */
	0x00000000, /* 90 */
	0x00000000, /* 91 */
	0x00000000, /* 92 */
	0x00000000, /* 93 */
	0x00000000, /* 94 */
	0x00000000, /* 95 */
	0x00000000, /* 96 */
	0x00000000, /* 97 */
	0x00000000, /* 98 */
	0x00000000, /* 99 */
	0x00000000, /* 100 */
	0x00000000, /* 101 */
	0x00000000, /* 102 */
	0x00000000, /* 103 */
	0x00000000, /* 104 */
	0x00000000, /* 105 */
	0x00000000, /* 106 */
	0x00000000, /* 107 */
	0x00000000, /* 108 */
	0x00000000, /* 109 */
	0x00000000, /* 110 */
	0x00000000, /* 111 */
	0x00000000, /* 112 */
	0x00000000, /* 113 */
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
	0x00000000, /* 126 */
	0x00000000, /* 127 */
	0x00000000, /* 128 */
	0x00000000, /* 129 */
	0x00000000, /* 130 */
	0x00000000, /* 131 */
	0x00000000, /* 132 */
	0x00000000, /* 133 */
	0x00000000, /* 134 */
	0x00000000, /* 135 */
	0x00000000, /* 136 */
	0x00000000, /* 137 */
	0x00000000, /* 138 */
	0x00000000, /* 139 */
	0x00000000, /* 140 */
	0x00000000, /* 141 */
	0x00000000, /* 142 */
	0x00000000, /* 143 */
	0x00000000, /* 144 */
	0x00000000, /* 145 */
	0x00000000, /* 146 */
	0x00000000, /* 147 */
	0x00000000, /* 148 */
	0x00000000, /* 149 */
	0x00000000, /* 150 */
	0x00000000, /* 151 */
	0x00000000, /* 152 */
	0x00000000, /* 153 */
	0x00000000, /* 154 */
	0x00000000, /* 155 */
	0x00000000, /* 156 */
	0x00000000, /* 157 */
	0x00000000, /* 158 */
	0x00000000, /* 159 */
	0x00000000, /* 160 */
	0x00000000, /* 161 */
	0x00000000, /* 162 */
	0x00000000, /* 163 */
	0x00000000, /* 164 */
	0x00000000, /* 165 */
	0x00000000, /* 166 */
	0x00000000, /* 167 */
	0x00000000, /* 168 */
	0x00000000, /* 169 */
	0x00000000, /* 170 */
	0x00000000, /* 171 */
	0x00000000, /* 172 */
	0x00000000, /* 173 */
	0x00000000, /* 174 */
	0x00000000, /* 175 */
	0x00000000, /* 176 */
	0x00000000, /* 177 */
	0x00000000, /* 178 */
	0x00000000, /* 179 */
	0x00000000, /* 180 */
	0x00000000, /* 181 */
	0x00000000, /* 182 */
	0x00000000, /* 183 */
	0x00000000, /* 184 */
	0x00000000, /* 185 */
	0x00000000, /* 186 */
	0x00000000, /* 187 */
	0x00000000, /* 188 */
	0x00000000, /* 189 */
	0x00000000, /* 190 */
	0x00000000, /* 191 */
	0x00000000, /* 192 */
	0x00000000, /* 193 */
	0x00000000, /* 194 */
	0x00000000, /* 195 */
	0x00000000, /* 196 */
	0x00000000, /* 197 */
	0x00000000, /* 198 */
	0x00000000, /* 199 */
	0x00000000, /* 200 */
	0x00000000, /* 201 */
	0x00000000, /* 202 */
	0x00000000, /* 203 */
	0x00000000, /* 204 */
	0x00000000, /* 205 */
	0x00000000, /* 206 */
	0x00000000, /* 207 */
	0x00000000, /* 208 */
	0x00000000, /* 209 */
	0x00000000, /* 210 */
	0x00000000, /* 211 */
	0x00000000, /* 212 */
	0x00000000, /* 213 */
	0x00000000, /* 214 */
	0x00000000, /* 215 */
	0x00000000, /* 216 */
	0x00000000, /* 217 */
	0x00000000, /* 218 */
	0x00000000, /* 219 */
	0x00000000, /* 220 */
	0x00000000, /* 221 */
	0x00000000, /* 222 */
	0x00000000, /* 223 */
	0x00000000, /* 224 */
	0x00000000, /* 225 */
	0x00000000, /* 226 */
	0x00000000, /* 227 */
	0x00000000, /* 228 */
	0x00000000, /* 229 */
	0x00000000, /* 230 */
	0x00000000, /* 231 */
	0x00000000, /* 232 */
	0x00000000, /* 233 */
	0x00000000, /* 234 */
	0x00000000, /* 235 */
	0x00000000, /* 236 */
	0x00000000, /* 237 */
	0x00000000, /* 238 */
	0x00000000, /* 239 */
	0x00000000, /* 240 */
	0x00000000, /* 241 */
	0x00000000, /* 242 */
	0x00000000, /* 243 */
	0x00000000, /* 244 */
	0x00000000, /* 245 */
	0x00000000, /* 246 */
	0x00000000, /* 247 */
	0x00000000, /* 248 */
	0x00000000, /* 249 */
	0x00000000, /* 250 */
	0x00000000, /* 251 */
	0x00000000, /* 252 */
	0x008fd000, /* 253 */
	0x00052510, /* 254 */
	0x7c33892e, /* 255 */
};

/* vec_1080p24hz_bvb_input.bss */
static const uint32_t s_aulRamBVBInput_1080p_24hz_3D[BVDC_P_RAM_TABLE_SIZE] =
{
	0x00a24000, /* 0 */
	0x0024027e, /* 1 */
	0x0020102c, /* 2 */
	0x00200094, /* 3 */
	0x0020876a, /* 4 */
	0x00800000, /* 5 */
	0x00000000, /* 6 */
	0x00000000, /* 7 */
	0x00000000, /* 8 */
	0x00000000, /* 9 */
	0x00000000, /* 10 */
	0x00000000, /* 11 */
	0x00000000, /* 12 */
	0x00000000, /* 13 */
	0x00000000, /* 14 */
	0x00000000, /* 15 */
	0x00000000, /* 16 */
	0x00000000, /* 17 */
	0x00000000, /* 18 */
	0x00000000, /* 19 */
	0x00a80000, /* 20 */
	0x00421001, /* 21 */
	0x00401004, /* 22 */
	0x00400024, /* 23 */
	0x00408438, /* 24 */
	0x00408465, /* 25 */
	0x00400004, /* 26 */
	0x00a80000, /* 27 */
	0x00815000, /* 28 */
	0x00000000, /* 29 */
	0x00000000, /* 30 */
	0x00000000, /* 31 */
	0x00000000, /* 32 */
	0x00000000, /* 33 */
	0x00000000, /* 34 */
	0x00000000, /* 35 */
	0x00000000, /* 36 */
	0x00000000, /* 37 */
	0x00000000, /* 38 */
	0x00000000, /* 39 */
	0x00000000, /* 40 */
	0x00000000, /* 41 */
	0x00000000, /* 42 */
	0x00000000, /* 43 */
	0x00000000, /* 44 */
	0x00000000, /* 45 */
	0x00000000, /* 46 */
	0x00000000, /* 47 */
	0x00000000, /* 48 */
	0x00000000, /* 49 */
	0x00000000, /* 50 */
	0x00000000, /* 51 */
	0x00000000, /* 52 */
	0x00000000, /* 53 */
	0x00000000, /* 54 */
	0x00000000, /* 55 */
	0x00000000, /* 56 */
	0x00000000, /* 57 */
	0x00000000, /* 58 */
	0x00000000, /* 59 */
	0x00000000, /* 60 */
	0x00000000, /* 61 */
	0x00000000, /* 62 */
	0x00000000, /* 63 */
	0x00000000, /* 64 */
	0x00000000, /* 65 */
	0x00000000, /* 66 */
	0x00000000, /* 67 */
	0x00000000, /* 68 */
	0x00000000, /* 69 */
	0x00000000, /* 70 */
	0x00000000, /* 71 */
	0x00000000, /* 72 */
	0x00000000, /* 73 */
	0x00000000, /* 74 */
	0x00000000, /* 75 */
	0x00000000, /* 76 */
	0x00000000, /* 77 */
	0x00000000, /* 78 */
	0x00000000, /* 79 */
	0x00000000, /* 80 */
	0x00000000, /* 81 */
	0x00000000, /* 82 */
	0x00000000, /* 83 */
	0x00000000, /* 84 */
	0x00000000, /* 85 */
	0x00000000, /* 86 */
	0x00000000, /* 87 */
	0x00000000, /* 88 */
	0x00000000, /* 89 */
	0x00000000, /* 90 */
	0x00000000, /* 91 */
	0x00000000, /* 92 */
	0x00000000, /* 93 */
	0x00000000, /* 94 */
	0x00000000, /* 95 */
	0x00000000, /* 96 */
	0x00000000, /* 97 */
	0x00000000, /* 98 */
	0x00000000, /* 99 */
	0x00000000, /* 100 */
	0x00000000, /* 101 */
	0x00000000, /* 102 */
	0x00000000, /* 103 */
	0x00000000, /* 104 */
	0x00000000, /* 105 */
	0x00000000, /* 106 */
	0x00000000, /* 107 */
	0x00000000, /* 108 */
	0x00000000, /* 109 */
	0x00000000, /* 110 */
	0x00000000, /* 111 */
	0x00000000, /* 112 */
	0x00000000, /* 113 */
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
	0x00000000, /* 126 */
	0x00000000, /* 127 */
	0x00000000, /* 128 */
	0x00000000, /* 129 */
	0x00000000, /* 130 */
	0x00000000, /* 131 */
	0x00000000, /* 132 */
	0x00000000, /* 133 */
	0x00000000, /* 134 */
	0x00000000, /* 135 */
	0x00000000, /* 136 */
	0x00000000, /* 137 */
	0x00000000, /* 138 */
	0x00000000, /* 139 */
	0x00000000, /* 140 */
	0x00000000, /* 141 */
	0x00000000, /* 142 */
	0x00000000, /* 143 */
	0x00000000, /* 144 */
	0x00000000, /* 145 */
	0x00000000, /* 146 */
	0x00000000, /* 147 */
	0x00000000, /* 148 */
	0x00000000, /* 149 */
	0x00000000, /* 150 */
	0x00000000, /* 151 */
	0x00000000, /* 152 */
	0x00000000, /* 153 */
	0x00000000, /* 154 */
	0x00000000, /* 155 */
	0x00000000, /* 156 */
	0x00000000, /* 157 */
	0x00000000, /* 158 */
	0x00000000, /* 159 */
	0x00000000, /* 160 */
	0x00000000, /* 161 */
	0x00000000, /* 162 */
	0x00000000, /* 163 */
	0x00000000, /* 164 */
	0x00000000, /* 165 */
	0x00000000, /* 166 */
	0x00000000, /* 167 */
	0x00000000, /* 168 */
	0x00000000, /* 169 */
	0x00000000, /* 170 */
	0x00000000, /* 171 */
	0x00000000, /* 172 */
	0x00000000, /* 173 */
	0x00000000, /* 174 */
	0x00000000, /* 175 */
	0x00000000, /* 176 */
	0x00000000, /* 177 */
	0x00000000, /* 178 */
	0x00000000, /* 179 */
	0x00000000, /* 180 */
	0x00000000, /* 181 */
	0x00000000, /* 182 */
	0x00000000, /* 183 */
	0x00000000, /* 184 */
	0x00000000, /* 185 */
	0x00000000, /* 186 */
	0x00000000, /* 187 */
	0x00000000, /* 188 */
	0x00000000, /* 189 */
	0x00000000, /* 190 */
	0x00000000, /* 191 */
	0x00000000, /* 192 */
	0x00000000, /* 193 */
	0x00000000, /* 194 */
	0x00000000, /* 195 */
	0x00000000, /* 196 */
	0x00000000, /* 197 */
	0x00000000, /* 198 */
	0x00000000, /* 199 */
	0x00000000, /* 200 */
	0x00000000, /* 201 */
	0x00000000, /* 202 */
	0x00000000, /* 203 */
	0x00000000, /* 204 */
	0x00000000, /* 205 */
	0x00000000, /* 206 */
	0x00000000, /* 207 */
	0x00000000, /* 208 */
	0x00000000, /* 209 */
	0x00000000, /* 210 */
	0x00000000, /* 211 */
	0x00000000, /* 212 */
	0x00000000, /* 213 */
	0x00000000, /* 214 */
	0x00000000, /* 215 */
	0x00000000, /* 216 */
	0x00000000, /* 217 */
	0x00000000, /* 218 */
	0x00000000, /* 219 */
	0x00000000, /* 220 */
	0x00000000, /* 221 */
	0x00000000, /* 222 */
	0x00000000, /* 223 */
	0x00000000, /* 224 */
	0x00000000, /* 225 */
	0x00000000, /* 226 */
	0x00000000, /* 227 */
	0x00000000, /* 228 */
	0x00000000, /* 229 */
	0x00000000, /* 230 */
	0x00000000, /* 231 */
	0x00000000, /* 232 */
	0x00000000, /* 233 */
	0x00000000, /* 234 */
	0x00000000, /* 235 */
	0x00000000, /* 236 */
	0x00000000, /* 237 */
	0x00000000, /* 238 */
	0x00000000, /* 239 */
	0x00000000, /* 240 */
	0x00000000, /* 241 */
	0x00000000, /* 242 */
	0x00000000, /* 243 */
	0x00000000, /* 244 */
	0x00000000, /* 245 */
	0x00000000, /* 246 */
	0x00000000, /* 247 */
	0x00000000, /* 248 */
	0x00000000, /* 249 */
	0x00000000, /* 250 */
	0x00000000, /* 251 */
	0x00000000, /* 252 */
	0x008fd000, /* 253 */
	0x00051711, /* 254 */
	0x67511cb3, /* 255 */
};

/* vec_1080p25hz_bvb_input.bss */
static const uint32_t s_aulRamBVBInput_1080p_25hz[BVDC_P_RAM_TABLE_SIZE] =
{
	0x00a30000, /* 0 */
	0x0020702c, /* 1 */
	0x0024002c, /* 2 */
	0x00200058, /* 3 */
	0x00204010, /* 4 */
	0x0028402c, /* 5 */
	0x0028c023, /* 6 */
	0x0028c75d, /* 7 */
	0x002001b8, /* 8 */
	0x00210016, /* 9 */
	0x00800000, /* 10 */
	0x00000000, /* 11 */
	0x00000000, /* 12 */
	0x00000000, /* 13 */
	0x00000000, /* 14 */
	0x00000000, /* 15 */
	0x00000000, /* 16 */
	0x00000000, /* 17 */
	0x00000000, /* 18 */
	0x00000000, /* 19 */
	0x00a80000, /* 20 */
	0x00471001, /* 21 */
	0x00451004, /* 22 */
	0x00444024, /* 23 */
	0x0040c438, /* 24 */
	0x00444004, /* 25 */
	0x00814000, /* 26 */
	0x00000000, /* 27 */
	0x00000000, /* 28 */
	0x00000000, /* 29 */
	0x00000000, /* 30 */
	0x00000000, /* 31 */
	0x00000000, /* 32 */
	0x00000000, /* 33 */
	0x00000000, /* 34 */
	0x00000000, /* 35 */
	0x00000000, /* 36 */
	0x00000000, /* 37 */
	0x00000000, /* 38 */
	0x00000000, /* 39 */
	0x00000000, /* 40 */
	0x00000000, /* 41 */
	0x00000000, /* 42 */
	0x00000000, /* 43 */
	0x00000000, /* 44 */
	0x00000000, /* 45 */
	0x00000000, /* 46 */
	0x00000000, /* 47 */
	0x00000000, /* 48 */
	0x00000000, /* 49 */
	0x00400001, /* 50 */
	0x00202052, /* 51 */
	0x00200501, /* 52 */
	0x0020102c, /* 53 */
	0x0020000b, /* 54 */
	0x00832000, /* 55 */
	0x00000000, /* 56 */
	0x00000000, /* 57 */
	0x00000000, /* 58 */
	0x00000000, /* 59 */
	0x00000000, /* 60 */
	0x00000000, /* 61 */
	0x00000000, /* 62 */
	0x00000000, /* 63 */
	0x00000000, /* 64 */
	0x00000000, /* 65 */
	0x00000000, /* 66 */
	0x00000000, /* 67 */
	0x00000000, /* 68 */
	0x00000000, /* 69 */
	0x00000000, /* 70 */
	0x00000000, /* 71 */
	0x00000000, /* 72 */
	0x00000000, /* 73 */
	0x00000000, /* 74 */
	0x00000000, /* 75 */
	0x00000000, /* 76 */
	0x00000000, /* 77 */
	0x00000000, /* 78 */
	0x00000000, /* 79 */
	0x00000000, /* 80 */
	0x00000000, /* 81 */
	0x00000000, /* 82 */
	0x00000000, /* 83 */
	0x00000000, /* 84 */
	0x00000000, /* 85 */
	0x00000000, /* 86 */
	0x00000000, /* 87 */
	0x00000000, /* 88 */
	0x00000000, /* 89 */
	0x00000000, /* 90 */
	0x00000000, /* 91 */
	0x00000000, /* 92 */
	0x00000000, /* 93 */
	0x00000000, /* 94 */
	0x00000000, /* 95 */
	0x00000000, /* 96 */
	0x00000000, /* 97 */
	0x00000000, /* 98 */
	0x00000000, /* 99 */
	0x00000000, /* 100 */
	0x00000000, /* 101 */
	0x00000000, /* 102 */
	0x00000000, /* 103 */
	0x00000000, /* 104 */
	0x00000000, /* 105 */
	0x00000000, /* 106 */
	0x00000000, /* 107 */
	0x00000000, /* 108 */
	0x00000000, /* 109 */
	0x00000000, /* 110 */
	0x00000000, /* 111 */
	0x00000000, /* 112 */
	0x00000000, /* 113 */
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
	0x00000000, /* 126 */
	0x00000000, /* 127 */
	0x00000000, /* 128 */
	0x00000000, /* 129 */
	0x00000000, /* 130 */
	0x00000000, /* 131 */
	0x00000000, /* 132 */
	0x00000000, /* 133 */
	0x00000000, /* 134 */
	0x00000000, /* 135 */
	0x00000000, /* 136 */
	0x00000000, /* 137 */
	0x00000000, /* 138 */
	0x00000000, /* 139 */
	0x00000000, /* 140 */
	0x00000000, /* 141 */
	0x00000000, /* 142 */
	0x00000000, /* 143 */
	0x00000000, /* 144 */
	0x00000000, /* 145 */
	0x00000000, /* 146 */
	0x00000000, /* 147 */
	0x00000000, /* 148 */
	0x00000000, /* 149 */
	0x00000000, /* 150 */
	0x00000000, /* 151 */
	0x00000000, /* 152 */
	0x00000000, /* 153 */
	0x00000000, /* 154 */
	0x00000000, /* 155 */
	0x00000000, /* 156 */
	0x00000000, /* 157 */
	0x00000000, /* 158 */
	0x00000000, /* 159 */
	0x00000000, /* 160 */
	0x00000000, /* 161 */
	0x00000000, /* 162 */
	0x00000000, /* 163 */
	0x00000000, /* 164 */
	0x00000000, /* 165 */
	0x00000000, /* 166 */
	0x00000000, /* 167 */
	0x00000000, /* 168 */
	0x00000000, /* 169 */
	0x00000000, /* 170 */
	0x00000000, /* 171 */
	0x00000000, /* 172 */
	0x00000000, /* 173 */
	0x00000000, /* 174 */
	0x00000000, /* 175 */
	0x00000000, /* 176 */
	0x00000000, /* 177 */
	0x00000000, /* 178 */
	0x00000000, /* 179 */
	0x00000000, /* 180 */
	0x00000000, /* 181 */
	0x00000000, /* 182 */
	0x00000000, /* 183 */
	0x00000000, /* 184 */
	0x00000000, /* 185 */
	0x00000000, /* 186 */
	0x00000000, /* 187 */
	0x00000000, /* 188 */
	0x00000000, /* 189 */
	0x00000000, /* 190 */
	0x00000000, /* 191 */
	0x00000000, /* 192 */
	0x00000000, /* 193 */
	0x00000000, /* 194 */
	0x00000000, /* 195 */
	0x00000000, /* 196 */
	0x00000000, /* 197 */
	0x00000000, /* 198 */
	0x00000000, /* 199 */
	0x00000000, /* 200 */
	0x00000000, /* 201 */
	0x00000000, /* 202 */
	0x00000000, /* 203 */
	0x00000000, /* 204 */
	0x00000000, /* 205 */
	0x00000000, /* 206 */
	0x00000000, /* 207 */
	0x00000000, /* 208 */
	0x00000000, /* 209 */
	0x00000000, /* 210 */
	0x00000000, /* 211 */
	0x00000000, /* 212 */
	0x00000000, /* 213 */
	0x00000000, /* 214 */
	0x00000000, /* 215 */
	0x00000000, /* 216 */
	0x00000000, /* 217 */
	0x00000000, /* 218 */
	0x00000000, /* 219 */
	0x00000000, /* 220 */
	0x00000000, /* 221 */
	0x00000000, /* 222 */
	0x00000000, /* 223 */
	0x00000000, /* 224 */
	0x00000000, /* 225 */
	0x00000000, /* 226 */
	0x00000000, /* 227 */
	0x00000000, /* 228 */
	0x00000000, /* 229 */
	0x00000000, /* 230 */
	0x00000000, /* 231 */
	0x00000000, /* 232 */
	0x00000000, /* 233 */
	0x00000000, /* 234 */
	0x00000000, /* 235 */
	0x00000000, /* 236 */
	0x00000000, /* 237 */
	0x00000000, /* 238 */
	0x00000000, /* 239 */
	0x00000000, /* 240 */
	0x00000000, /* 241 */
	0x00000000, /* 242 */
	0x00000000, /* 243 */
	0x00000000, /* 244 */
	0x00000000, /* 245 */
	0x00000000, /* 246 */
	0x00000000, /* 247 */
	0x00000000, /* 248 */
	0x00000000, /* 249 */
	0x00000000, /* 250 */
	0x00000000, /* 251 */
	0x00000000, /* 252 */
	0x008fd000, /* 253 */
	0x00052510, /* 254 */
	0x4f1ce5f6, /* 255 */
};

/* vec_1080p30hz_bvb_input.bss */
static const uint32_t s_aulRamBVBInput_1080p_30hz[BVDC_P_RAM_TABLE_SIZE] =
{
	0x00a30000, /* 0 */
	0x0020702c, /* 1 */
	0x0024002c, /* 2 */
	0x00200058, /* 3 */
	0x00204010, /* 4 */
	0x0028402c, /* 5 */
	0x0028c023, /* 6 */
	0x0028c75d, /* 7 */
	0x00210016, /* 8 */
	0x00800000, /* 9 */
	0x00000000, /* 10 */
	0x00000000, /* 11 */
	0x00000000, /* 12 */
	0x00000000, /* 13 */
	0x00000000, /* 14 */
	0x00000000, /* 15 */
	0x00000000, /* 16 */
	0x00000000, /* 17 */
	0x00000000, /* 18 */
	0x00000000, /* 19 */
	0x00470001, /* 20 */
	0x00a80000, /* 21 */
	0x00471001, /* 22 */
	0x00451004, /* 23 */
	0x00444024, /* 24 */
	0x0040c438, /* 25 */
	0x00444003, /* 26 */
	0x00444001, /* 27 */
	0x00815000, /* 28 */
	0x00000000, /* 29 */
	0x00000000, /* 30 */
	0x00000000, /* 31 */
	0x00000000, /* 32 */
	0x00000000, /* 33 */
	0x00000000, /* 34 */
	0x00000000, /* 35 */
	0x00000000, /* 36 */
	0x00000000, /* 37 */
	0x00000000, /* 38 */
	0x00000000, /* 39 */
	0x00000000, /* 40 */
	0x00000000, /* 41 */
	0x00000000, /* 42 */
	0x00000000, /* 43 */
	0x00000000, /* 44 */
	0x00000000, /* 45 */
	0x00000000, /* 46 */
	0x00000000, /* 47 */
	0x00000000, /* 48 */
	0x00000000, /* 49 */
	0x00400001, /* 50 */
	0x00202052, /* 51 */
	0x00200425, /* 52 */
	0x0020102c, /* 53 */
	0x0020000b, /* 54 */
	0x00832000, /* 55 */
	0x00000000, /* 56 */
	0x00000000, /* 57 */
	0x00000000, /* 58 */
	0x00000000, /* 59 */
	0x00000000, /* 60 */
	0x00000000, /* 61 */
	0x00000000, /* 62 */
	0x00000000, /* 63 */
	0x00000000, /* 64 */
	0x00000000, /* 65 */
	0x00000000, /* 66 */
	0x00000000, /* 67 */
	0x00000000, /* 68 */
	0x00000000, /* 69 */
	0x00000000, /* 70 */
	0x00000000, /* 71 */
	0x00000000, /* 72 */
	0x00000000, /* 73 */
	0x00000000, /* 74 */
	0x00000000, /* 75 */
	0x00000000, /* 76 */
	0x00000000, /* 77 */
	0x00000000, /* 78 */
	0x00000000, /* 79 */
	0x00000000, /* 80 */
	0x00000000, /* 81 */
	0x00000000, /* 82 */
	0x00000000, /* 83 */
	0x00000000, /* 84 */
	0x00000000, /* 85 */
	0x00000000, /* 86 */
	0x00000000, /* 87 */
	0x00000000, /* 88 */
	0x00000000, /* 89 */
	0x00000000, /* 90 */
	0x00000000, /* 91 */
	0x00000000, /* 92 */
	0x00000000, /* 93 */
	0x00000000, /* 94 */
	0x00000000, /* 95 */
	0x00000000, /* 96 */
	0x00000000, /* 97 */
	0x00000000, /* 98 */
	0x00000000, /* 99 */
	0x00000000, /* 100 */
	0x00000000, /* 101 */
	0x00000000, /* 102 */
	0x00000000, /* 103 */
	0x00000000, /* 104 */
	0x00000000, /* 105 */
	0x00000000, /* 106 */
	0x00000000, /* 107 */
	0x00000000, /* 108 */
	0x00000000, /* 109 */
	0x00000000, /* 110 */
	0x00000000, /* 111 */
	0x00000000, /* 112 */
	0x00000000, /* 113 */
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
	0x00000000, /* 126 */
	0x00000000, /* 127 */
	0x00000000, /* 128 */
	0x00000000, /* 129 */
	0x00000000, /* 130 */
	0x00000000, /* 131 */
	0x00000000, /* 132 */
	0x00000000, /* 133 */
	0x00000000, /* 134 */
	0x00000000, /* 135 */
	0x00000000, /* 136 */
	0x00000000, /* 137 */
	0x00000000, /* 138 */
	0x00000000, /* 139 */
	0x00000000, /* 140 */
	0x00000000, /* 141 */
	0x00000000, /* 142 */
	0x00000000, /* 143 */
	0x00000000, /* 144 */
	0x00000000, /* 145 */
	0x00000000, /* 146 */
	0x00000000, /* 147 */
	0x00000000, /* 148 */
	0x00000000, /* 149 */
	0x00000000, /* 150 */
	0x00000000, /* 151 */
	0x00000000, /* 152 */
	0x00000000, /* 153 */
	0x00000000, /* 154 */
	0x00000000, /* 155 */
	0x00000000, /* 156 */
	0x00000000, /* 157 */
	0x00000000, /* 158 */
	0x00000000, /* 159 */
	0x00000000, /* 160 */
	0x00000000, /* 161 */
	0x00000000, /* 162 */
	0x00000000, /* 163 */
	0x00000000, /* 164 */
	0x00000000, /* 165 */
	0x00000000, /* 166 */
	0x00000000, /* 167 */
	0x00000000, /* 168 */
	0x00000000, /* 169 */
	0x00000000, /* 170 */
	0x00000000, /* 171 */
	0x00000000, /* 172 */
	0x00000000, /* 173 */
	0x00000000, /* 174 */
	0x00000000, /* 175 */
	0x00000000, /* 176 */
	0x00000000, /* 177 */
	0x00000000, /* 178 */
	0x00000000, /* 179 */
	0x00000000, /* 180 */
	0x00000000, /* 181 */
	0x00000000, /* 182 */
	0x00000000, /* 183 */
	0x00000000, /* 184 */
	0x00000000, /* 185 */
	0x00000000, /* 186 */
	0x00000000, /* 187 */
	0x00000000, /* 188 */
	0x00000000, /* 189 */
	0x00000000, /* 190 */
	0x00000000, /* 191 */
	0x00000000, /* 192 */
	0x00000000, /* 193 */
	0x00000000, /* 194 */
	0x00000000, /* 195 */
	0x00000000, /* 196 */
	0x00000000, /* 197 */
	0x00000000, /* 198 */
	0x00000000, /* 199 */
	0x00000000, /* 200 */
	0x00000000, /* 201 */
	0x00000000, /* 202 */
	0x00000000, /* 203 */
	0x00000000, /* 204 */
	0x00000000, /* 205 */
	0x00000000, /* 206 */
	0x00000000, /* 207 */
	0x00000000, /* 208 */
	0x00000000, /* 209 */
	0x00000000, /* 210 */
	0x00000000, /* 211 */
	0x00000000, /* 212 */
	0x00000000, /* 213 */
	0x00000000, /* 214 */
	0x00000000, /* 215 */
	0x00000000, /* 216 */
	0x00000000, /* 217 */
	0x00000000, /* 218 */
	0x00000000, /* 219 */
	0x00000000, /* 220 */
	0x00000000, /* 221 */
	0x00000000, /* 222 */
	0x00000000, /* 223 */
	0x00000000, /* 224 */
	0x00000000, /* 225 */
	0x00000000, /* 226 */
	0x00000000, /* 227 */
	0x00000000, /* 228 */
	0x00000000, /* 229 */
	0x00000000, /* 230 */
	0x00000000, /* 231 */
	0x00000000, /* 232 */
	0x00000000, /* 233 */
	0x00000000, /* 234 */
	0x00000000, /* 235 */
	0x00000000, /* 236 */
	0x00000000, /* 237 */
	0x00000000, /* 238 */
	0x00000000, /* 239 */
	0x00000000, /* 240 */
	0x00000000, /* 241 */
	0x00000000, /* 242 */
	0x00000000, /* 243 */
	0x00000000, /* 244 */
	0x00000000, /* 245 */
	0x00000000, /* 246 */
	0x00000000, /* 247 */
	0x00000000, /* 248 */
	0x00000000, /* 249 */
	0x00000000, /* 250 */
	0x00000000, /* 251 */
	0x00000000, /* 252 */
	0x008fd000, /* 253 */
	0x00052510, /* 254 */
	0x61389946, /* 255 */
};

/* vec_1080p30hz_bvb_input.bss */
static const uint32_t s_aulRamBVBInput_1080p_30hz_3D[BVDC_P_RAM_TABLE_SIZE] =
{
	0x00a24000, /* 0 */
	0x00240058, /* 1 */
	0x0020102c, /* 2 */
	0x00200094, /* 3 */
	0x0020876a, /* 4 */
	0x00800000, /* 5 */
	0x00000000, /* 6 */
	0x00000000, /* 7 */
	0x00000000, /* 8 */
	0x00000000, /* 9 */
	0x00000000, /* 10 */
	0x00000000, /* 11 */
	0x00000000, /* 12 */
	0x00000000, /* 13 */
	0x00000000, /* 14 */
	0x00000000, /* 15 */
	0x00000000, /* 16 */
	0x00000000, /* 17 */
	0x00000000, /* 18 */
	0x00000000, /* 19 */
	0x00a80000, /* 20 */
	0x00421001, /* 21 */
	0x00401004, /* 22 */
	0x00400024, /* 23 */
	0x00408438, /* 24 */
	0x00408465, /* 25 */
	0x00400004, /* 26 */
	0x00a80000, /* 27 */
	0x00815000, /* 28 */
	0x00000000, /* 29 */
	0x00000000, /* 30 */
	0x00000000, /* 31 */
	0x00000000, /* 32 */
	0x00000000, /* 33 */
	0x00000000, /* 34 */
	0x00000000, /* 35 */
	0x00000000, /* 36 */
	0x00000000, /* 37 */
	0x00000000, /* 38 */
	0x00000000, /* 39 */
	0x00000000, /* 40 */
	0x00000000, /* 41 */
	0x00000000, /* 42 */
	0x00000000, /* 43 */
	0x00000000, /* 44 */
	0x00000000, /* 45 */
	0x00000000, /* 46 */
	0x00000000, /* 47 */
	0x00000000, /* 48 */
	0x00000000, /* 49 */
	0x00000000, /* 50 */
	0x00000000, /* 51 */
	0x00000000, /* 52 */
	0x00000000, /* 53 */
	0x00000000, /* 54 */
	0x00000000, /* 55 */
	0x00000000, /* 56 */
	0x00000000, /* 57 */
	0x00000000, /* 58 */
	0x00000000, /* 59 */
	0x00000000, /* 60 */
	0x00000000, /* 61 */
	0x00000000, /* 62 */
	0x00000000, /* 63 */
	0x00000000, /* 64 */
	0x00000000, /* 65 */
	0x00000000, /* 66 */
	0x00000000, /* 67 */
	0x00000000, /* 68 */
	0x00000000, /* 69 */
	0x00000000, /* 70 */
	0x00000000, /* 71 */
	0x00000000, /* 72 */
	0x00000000, /* 73 */
	0x00000000, /* 74 */
	0x00000000, /* 75 */
	0x00000000, /* 76 */
	0x00000000, /* 77 */
	0x00000000, /* 78 */
	0x00000000, /* 79 */
	0x00000000, /* 80 */
	0x00000000, /* 81 */
	0x00000000, /* 82 */
	0x00000000, /* 83 */
	0x00000000, /* 84 */
	0x00000000, /* 85 */
	0x00000000, /* 86 */
	0x00000000, /* 87 */
	0x00000000, /* 88 */
	0x00000000, /* 89 */
	0x00000000, /* 90 */
	0x00000000, /* 91 */
	0x00000000, /* 92 */
	0x00000000, /* 93 */
	0x00000000, /* 94 */
	0x00000000, /* 95 */
	0x00000000, /* 96 */
	0x00000000, /* 97 */
	0x00000000, /* 98 */
	0x00000000, /* 99 */
	0x00000000, /* 100 */
	0x00000000, /* 101 */
	0x00000000, /* 102 */
	0x00000000, /* 103 */
	0x00000000, /* 104 */
	0x00000000, /* 105 */
	0x00000000, /* 106 */
	0x00000000, /* 107 */
	0x00000000, /* 108 */
	0x00000000, /* 109 */
	0x00000000, /* 110 */
	0x00000000, /* 111 */
	0x00000000, /* 112 */
	0x00000000, /* 113 */
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
	0x00000000, /* 126 */
	0x00000000, /* 127 */
	0x00000000, /* 128 */
	0x00000000, /* 129 */
	0x00000000, /* 130 */
	0x00000000, /* 131 */
	0x00000000, /* 132 */
	0x00000000, /* 133 */
	0x00000000, /* 134 */
	0x00000000, /* 135 */
	0x00000000, /* 136 */
	0x00000000, /* 137 */
	0x00000000, /* 138 */
	0x00000000, /* 139 */
	0x00000000, /* 140 */
	0x00000000, /* 141 */
	0x00000000, /* 142 */
	0x00000000, /* 143 */
	0x00000000, /* 144 */
	0x00000000, /* 145 */
	0x00000000, /* 146 */
	0x00000000, /* 147 */
	0x00000000, /* 148 */
	0x00000000, /* 149 */
	0x00000000, /* 150 */
	0x00000000, /* 151 */
	0x00000000, /* 152 */
	0x00000000, /* 153 */
	0x00000000, /* 154 */
	0x00000000, /* 155 */
	0x00000000, /* 156 */
	0x00000000, /* 157 */
	0x00000000, /* 158 */
	0x00000000, /* 159 */
	0x00000000, /* 160 */
	0x00000000, /* 161 */
	0x00000000, /* 162 */
	0x00000000, /* 163 */
	0x00000000, /* 164 */
	0x00000000, /* 165 */
	0x00000000, /* 166 */
	0x00000000, /* 167 */
	0x00000000, /* 168 */
	0x00000000, /* 169 */
	0x00000000, /* 170 */
	0x00000000, /* 171 */
	0x00000000, /* 172 */
	0x00000000, /* 173 */
	0x00000000, /* 174 */
	0x00000000, /* 175 */
	0x00000000, /* 176 */
	0x00000000, /* 177 */
	0x00000000, /* 178 */
	0x00000000, /* 179 */
	0x00000000, /* 180 */
	0x00000000, /* 181 */
	0x00000000, /* 182 */
	0x00000000, /* 183 */
	0x00000000, /* 184 */
	0x00000000, /* 185 */
	0x00000000, /* 186 */
	0x00000000, /* 187 */
	0x00000000, /* 188 */
	0x00000000, /* 189 */
	0x00000000, /* 190 */
	0x00000000, /* 191 */
	0x00000000, /* 192 */
	0x00000000, /* 193 */
	0x00000000, /* 194 */
	0x00000000, /* 195 */
	0x00000000, /* 196 */
	0x00000000, /* 197 */
	0x00000000, /* 198 */
	0x00000000, /* 199 */
	0x00000000, /* 200 */
	0x00000000, /* 201 */
	0x00000000, /* 202 */
	0x00000000, /* 203 */
	0x00000000, /* 204 */
	0x00000000, /* 205 */
	0x00000000, /* 206 */
	0x00000000, /* 207 */
	0x00000000, /* 208 */
	0x00000000, /* 209 */
	0x00000000, /* 210 */
	0x00000000, /* 211 */
	0x00000000, /* 212 */
	0x00000000, /* 213 */
	0x00000000, /* 214 */
	0x00000000, /* 215 */
	0x00000000, /* 216 */
	0x00000000, /* 217 */
	0x00000000, /* 218 */
	0x00000000, /* 219 */
	0x00000000, /* 220 */
	0x00000000, /* 221 */
	0x00000000, /* 222 */
	0x00000000, /* 223 */
	0x00000000, /* 224 */
	0x00000000, /* 225 */
	0x00000000, /* 226 */
	0x00000000, /* 227 */
	0x00000000, /* 228 */
	0x00000000, /* 229 */
	0x00000000, /* 230 */
	0x00000000, /* 231 */
	0x00000000, /* 232 */
	0x00000000, /* 233 */
	0x00000000, /* 234 */
	0x00000000, /* 235 */
	0x00000000, /* 236 */
	0x00000000, /* 237 */
	0x00000000, /* 238 */
	0x00000000, /* 239 */
	0x00000000, /* 240 */
	0x00000000, /* 241 */
	0x00000000, /* 242 */
	0x00000000, /* 243 */
	0x00000000, /* 244 */
	0x00000000, /* 245 */
	0x00000000, /* 246 */
	0x00000000, /* 247 */
	0x00000000, /* 248 */
	0x00000000, /* 249 */
	0x00000000, /* 250 */
	0x00000000, /* 251 */
	0x00000000, /* 252 */
	0x008fd000, /* 253 */
	0x00051711, /* 254 */
	0x7550fb85, /* 255 */
};

/* ANA_MCODE_1920x2205p3D_30Hz.bss */
static const uint32_t s_aulRamBVBInput_1080p_60hz_3DOU[BVDC_P_RAM_TABLE_SIZE] =
{
	0x00a24000, /* 0 */
	0x00240058, /* 1 */
	0x0020102c, /* 2 */
	0x00200094, /* 3 */
	0x0020876a, /* 4 */
	0x00800000, /* 5 */
	0x00000000, /* 6 */
	0x00000000, /* 7 */
	0x00000000, /* 8 */
	0x00000000, /* 9 */
	0x00000000, /* 10 */
	0x00000000, /* 11 */
	0x00000000, /* 12 */
	0x00000000, /* 13 */
	0x00000000, /* 14 */
	0x00000000, /* 15 */
	0x00000000, /* 16 */
	0x00000000, /* 17 */
	0x00000000, /* 18 */
	0x00000000, /* 19 */
	0x00a80000, /* 20 */
	0x00421001, /* 21 */
	0x00401004, /* 22 */
	0x00400024, /* 23 */
	0x0040889d, /* 24 */
	0x00400004, /* 25 */
	0x00a80000, /* 26 */
	0x00815000, /* 27 */
	0x00000000, /* 28 */
	0x00000000, /* 29 */
	0x00000000, /* 30 */
	0x00000000, /* 31 */
	0x00000000, /* 32 */
	0x00000000, /* 33 */
	0x00000000, /* 34 */
	0x00000000, /* 35 */
	0x00000000, /* 36 */
	0x00000000, /* 37 */
	0x00000000, /* 38 */
	0x00000000, /* 39 */
	0x00000000, /* 40 */
	0x00000000, /* 41 */
	0x00000000, /* 42 */
	0x00000000, /* 43 */
	0x00000000, /* 44 */
	0x00000000, /* 45 */
	0x00000000, /* 46 */
	0x00000000, /* 47 */
	0x00000000, /* 48 */
	0x00000000, /* 49 */
	0x00000000, /* 50 */
	0x00000000, /* 51 */
	0x00000000, /* 52 */
	0x00000000, /* 53 */
	0x00000000, /* 54 */
	0x00000000, /* 55 */
	0x00000000, /* 56 */
	0x00000000, /* 57 */
	0x00000000, /* 58 */
	0x00000000, /* 59 */
	0x00000000, /* 60 */
	0x00000000, /* 61 */
	0x00000000, /* 62 */
	0x00000000, /* 63 */
	0x00000000, /* 64 */
	0x00000000, /* 65 */
	0x00000000, /* 66 */
	0x00000000, /* 67 */
	0x00000000, /* 68 */
	0x00000000, /* 69 */
	0x00000000, /* 70 */
	0x00000000, /* 71 */
	0x00000000, /* 72 */
	0x00000000, /* 73 */
	0x00000000, /* 74 */
	0x00000000, /* 75 */
	0x00000000, /* 76 */
	0x00000000, /* 77 */
	0x00000000, /* 78 */
	0x00000000, /* 79 */
	0x00000000, /* 80 */
	0x00000000, /* 81 */
	0x00000000, /* 82 */
	0x00000000, /* 83 */
	0x00000000, /* 84 */
	0x00000000, /* 85 */
	0x00000000, /* 86 */
	0x00000000, /* 87 */
	0x00000000, /* 88 */
	0x00000000, /* 89 */
	0x00000000, /* 90 */
	0x00000000, /* 91 */
	0x00000000, /* 92 */
	0x00000000, /* 93 */
	0x00000000, /* 94 */
	0x00000000, /* 95 */
	0x00000000, /* 96 */
	0x00000000, /* 97 */
	0x00000000, /* 98 */
	0x00000000, /* 99 */
	0x00000000, /* 100 */
	0x00000000, /* 101 */
	0x00000000, /* 102 */
	0x00000000, /* 103 */
	0x00000000, /* 104 */
	0x00000000, /* 105 */
	0x00000000, /* 106 */
	0x00000000, /* 107 */
	0x00000000, /* 108 */
	0x00000000, /* 109 */
	0x00000000, /* 110 */
	0x00000000, /* 111 */
	0x00000000, /* 112 */
	0x00000000, /* 113 */
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
	0x00000000, /* 126 */
	0x00000000, /* 127 */
	0x00000000, /* 128 */
	0x00000000, /* 129 */
	0x00000000, /* 130 */
	0x00000000, /* 131 */
	0x00000000, /* 132 */
	0x00000000, /* 133 */
	0x00000000, /* 134 */
	0x00000000, /* 135 */
	0x00000000, /* 136 */
	0x00000000, /* 137 */
	0x00000000, /* 138 */
	0x00000000, /* 139 */
	0x00000000, /* 140 */
	0x00000000, /* 141 */
	0x00000000, /* 142 */
	0x00000000, /* 143 */
	0x00000000, /* 144 */
	0x00000000, /* 145 */
	0x00000000, /* 146 */
	0x00000000, /* 147 */
	0x00000000, /* 148 */
	0x00000000, /* 149 */
	0x00000000, /* 150 */
	0x00000000, /* 151 */
	0x00000000, /* 152 */
	0x00000000, /* 153 */
	0x00000000, /* 154 */
	0x00000000, /* 155 */
	0x00000000, /* 156 */
	0x00000000, /* 157 */
	0x00000000, /* 158 */
	0x00000000, /* 159 */
	0x00000000, /* 160 */
	0x00000000, /* 161 */
	0x00000000, /* 162 */
	0x00000000, /* 163 */
	0x00000000, /* 164 */
	0x00000000, /* 165 */
	0x00000000, /* 166 */
	0x00000000, /* 167 */
	0x00000000, /* 168 */
	0x00000000, /* 169 */
	0x00000000, /* 170 */
	0x00000000, /* 171 */
	0x00000000, /* 172 */
	0x00000000, /* 173 */
	0x00000000, /* 174 */
	0x00000000, /* 175 */
	0x00000000, /* 176 */
	0x00000000, /* 177 */
	0x00000000, /* 178 */
	0x00000000, /* 179 */
	0x00000000, /* 180 */
	0x00000000, /* 181 */
	0x00000000, /* 182 */
	0x00000000, /* 183 */
	0x00000000, /* 184 */
	0x00000000, /* 185 */
	0x00000000, /* 186 */
	0x00000000, /* 187 */
	0x00000000, /* 188 */
	0x00000000, /* 189 */
	0x00000000, /* 190 */
	0x00000000, /* 191 */
	0x00000000, /* 192 */
	0x00000000, /* 193 */
	0x00000000, /* 194 */
	0x00000000, /* 195 */
	0x00000000, /* 196 */
	0x00000000, /* 197 */
	0x00000000, /* 198 */
	0x00000000, /* 199 */
	0x00000000, /* 200 */
	0x00000000, /* 201 */
	0x00000000, /* 202 */
	0x00000000, /* 203 */
	0x00000000, /* 204 */
	0x00000000, /* 205 */
	0x00000000, /* 206 */
	0x00000000, /* 207 */
	0x00000000, /* 208 */
	0x00000000, /* 209 */
	0x00000000, /* 210 */
	0x00000000, /* 211 */
	0x00000000, /* 212 */
	0x00000000, /* 213 */
	0x00000000, /* 214 */
	0x00000000, /* 215 */
	0x00000000, /* 216 */
	0x00000000, /* 217 */
	0x00000000, /* 218 */
	0x00000000, /* 219 */
	0x00000000, /* 220 */
	0x00000000, /* 221 */
	0x00000000, /* 222 */
	0x00000000, /* 223 */
	0x00000000, /* 224 */
	0x00000000, /* 225 */
	0x00000000, /* 226 */
	0x00000000, /* 227 */
	0x00000000, /* 228 */
	0x00000000, /* 229 */
	0x00000000, /* 230 */
	0x00000000, /* 231 */
	0x00000000, /* 232 */
	0x00000000, /* 233 */
	0x00000000, /* 234 */
	0x00000000, /* 235 */
	0x00000000, /* 236 */
	0x00000000, /* 237 */
	0x00000000, /* 238 */
	0x00000000, /* 239 */
	0x00000000, /* 240 */
	0x00000000, /* 241 */
	0x00000000, /* 242 */
	0x00000000, /* 243 */
	0x00000000, /* 244 */
	0x00000000, /* 245 */
	0x00000000, /* 246 */
	0x00000000, /* 247 */
	0x00000000, /* 248 */
	0x00000000, /* 249 */
	0x00000000, /* 250 */
	0x00000000, /* 251 */
	0x00000000, /* 252 */
	0x008fd000, /* 253 */
	0x00091211, /* 254 */
	0x00c5cc13, /* 255 */
};

/* ANA_MCODE_3840x1080p3D_FR_60Hz.bss */
static const uint32_t s_aulRamBVBInput_1080p_60hz_3DLR[BVDC_P_RAM_TABLE_SIZE] =
{
	0x00a24000, /* 0 */
	0x002400b0, /* 1 */
	0x00201058, /* 2 */
	0x00200128, /* 3 */
	0x00208eea, /* 4 */
	0x00800000, /* 5 */
	0x00000000, /* 6 */
	0x00000000, /* 7 */
	0x00000000, /* 8 */
	0x00000000, /* 9 */
	0x00000000, /* 10 */
	0x00000000, /* 11 */
	0x00000000, /* 12 */
	0x00000000, /* 13 */
	0x00000000, /* 14 */
	0x00000000, /* 15 */
	0x00000000, /* 16 */
	0x00000000, /* 17 */
	0x00000000, /* 18 */
	0x00000000, /* 19 */
	0x00a80000, /* 20 */
	0x00421001, /* 21 */
	0x00401004, /* 22 */
	0x00400024, /* 23 */
	0x00408438, /* 24 */
	0x00400004, /* 25 */
	0x00a80000, /* 26 */
	0x00815000, /* 27 */
	0x00000000, /* 28 */
	0x00000000, /* 29 */
	0x00000000, /* 30 */
	0x00000000, /* 31 */
	0x00000000, /* 32 */
	0x00000000, /* 33 */
	0x00000000, /* 34 */
	0x00000000, /* 35 */
	0x00000000, /* 36 */
	0x00000000, /* 37 */
	0x00000000, /* 38 */
	0x00000000, /* 39 */
	0x00000000, /* 40 */
	0x00000000, /* 41 */
	0x00000000, /* 42 */
	0x00000000, /* 43 */
	0x00000000, /* 44 */
	0x00000000, /* 45 */
	0x00000000, /* 46 */
	0x00000000, /* 47 */
	0x00000000, /* 48 */
	0x00000000, /* 49 */
	0x00000000, /* 50 */
	0x00000000, /* 51 */
	0x00000000, /* 52 */
	0x00000000, /* 53 */
	0x00000000, /* 54 */
	0x00000000, /* 55 */
	0x00000000, /* 56 */
	0x00000000, /* 57 */
	0x00000000, /* 58 */
	0x00000000, /* 59 */
	0x00000000, /* 60 */
	0x00000000, /* 61 */
	0x00000000, /* 62 */
	0x00000000, /* 63 */
	0x00000000, /* 64 */
	0x00000000, /* 65 */
	0x00000000, /* 66 */
	0x00000000, /* 67 */
	0x00000000, /* 68 */
	0x00000000, /* 69 */
	0x00000000, /* 70 */
	0x00000000, /* 71 */
	0x00000000, /* 72 */
	0x00000000, /* 73 */
	0x00000000, /* 74 */
	0x00000000, /* 75 */
	0x00000000, /* 76 */
	0x00000000, /* 77 */
	0x00000000, /* 78 */
	0x00000000, /* 79 */
	0x00000000, /* 80 */
	0x00000000, /* 81 */
	0x00000000, /* 82 */
	0x00000000, /* 83 */
	0x00000000, /* 84 */
	0x00000000, /* 85 */
	0x00000000, /* 86 */
	0x00000000, /* 87 */
	0x00000000, /* 88 */
	0x00000000, /* 89 */
	0x00000000, /* 90 */
	0x00000000, /* 91 */
	0x00000000, /* 92 */
	0x00000000, /* 93 */
	0x00000000, /* 94 */
	0x00000000, /* 95 */
	0x00000000, /* 96 */
	0x00000000, /* 97 */
	0x00000000, /* 98 */
	0x00000000, /* 99 */
	0x00000000, /* 100 */
	0x00000000, /* 101 */
	0x00000000, /* 102 */
	0x00000000, /* 103 */
	0x00000000, /* 104 */
	0x00000000, /* 105 */
	0x00000000, /* 106 */
	0x00000000, /* 107 */
	0x00000000, /* 108 */
	0x00000000, /* 109 */
	0x00000000, /* 110 */
	0x00000000, /* 111 */
	0x00000000, /* 112 */
	0x00000000, /* 113 */
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
	0x00000000, /* 126 */
	0x00000000, /* 127 */
	0x00000000, /* 128 */
	0x00000000, /* 129 */
	0x00000000, /* 130 */
	0x00000000, /* 131 */
	0x00000000, /* 132 */
	0x00000000, /* 133 */
	0x00000000, /* 134 */
	0x00000000, /* 135 */
	0x00000000, /* 136 */
	0x00000000, /* 137 */
	0x00000000, /* 138 */
	0x00000000, /* 139 */
	0x00000000, /* 140 */
	0x00000000, /* 141 */
	0x00000000, /* 142 */
	0x00000000, /* 143 */
	0x00000000, /* 144 */
	0x00000000, /* 145 */
	0x00000000, /* 146 */
	0x00000000, /* 147 */
	0x00000000, /* 148 */
	0x00000000, /* 149 */
	0x00000000, /* 150 */
	0x00000000, /* 151 */
	0x00000000, /* 152 */
	0x00000000, /* 153 */
	0x00000000, /* 154 */
	0x00000000, /* 155 */
	0x00000000, /* 156 */
	0x00000000, /* 157 */
	0x00000000, /* 158 */
	0x00000000, /* 159 */
	0x00000000, /* 160 */
	0x00000000, /* 161 */
	0x00000000, /* 162 */
	0x00000000, /* 163 */
	0x00000000, /* 164 */
	0x00000000, /* 165 */
	0x00000000, /* 166 */
	0x00000000, /* 167 */
	0x00000000, /* 168 */
	0x00000000, /* 169 */
	0x00000000, /* 170 */
	0x00000000, /* 171 */
	0x00000000, /* 172 */
	0x00000000, /* 173 */
	0x00000000, /* 174 */
	0x00000000, /* 175 */
	0x00000000, /* 176 */
	0x00000000, /* 177 */
	0x00000000, /* 178 */
	0x00000000, /* 179 */
	0x00000000, /* 180 */
	0x00000000, /* 181 */
	0x00000000, /* 182 */
	0x00000000, /* 183 */
	0x00000000, /* 184 */
	0x00000000, /* 185 */
	0x00000000, /* 186 */
	0x00000000, /* 187 */
	0x00000000, /* 188 */
	0x00000000, /* 189 */
	0x00000000, /* 190 */
	0x00000000, /* 191 */
	0x00000000, /* 192 */
	0x00000000, /* 193 */
	0x00000000, /* 194 */
	0x00000000, /* 195 */
	0x00000000, /* 196 */
	0x00000000, /* 197 */
	0x00000000, /* 198 */
	0x00000000, /* 199 */
	0x00000000, /* 200 */
	0x00000000, /* 201 */
	0x00000000, /* 202 */
	0x00000000, /* 203 */
	0x00000000, /* 204 */
	0x00000000, /* 205 */
	0x00000000, /* 206 */
	0x00000000, /* 207 */
	0x00000000, /* 208 */
	0x00000000, /* 209 */
	0x00000000, /* 210 */
	0x00000000, /* 211 */
	0x00000000, /* 212 */
	0x00000000, /* 213 */
	0x00000000, /* 214 */
	0x00000000, /* 215 */
	0x00000000, /* 216 */
	0x00000000, /* 217 */
	0x00000000, /* 218 */
	0x00000000, /* 219 */
	0x00000000, /* 220 */
	0x00000000, /* 221 */
	0x00000000, /* 222 */
	0x00000000, /* 223 */
	0x00000000, /* 224 */
	0x00000000, /* 225 */
	0x00000000, /* 226 */
	0x00000000, /* 227 */
	0x00000000, /* 228 */
	0x00000000, /* 229 */
	0x00000000, /* 230 */
	0x00000000, /* 231 */
	0x00000000, /* 232 */
	0x00000000, /* 233 */
	0x00000000, /* 234 */
	0x00000000, /* 235 */
	0x00000000, /* 236 */
	0x00000000, /* 237 */
	0x00000000, /* 238 */
	0x00000000, /* 239 */
	0x00000000, /* 240 */
	0x00000000, /* 241 */
	0x00000000, /* 242 */
	0x00000000, /* 243 */
	0x00000000, /* 244 */
	0x00000000, /* 245 */
	0x00000000, /* 246 */
	0x00000000, /* 247 */
	0x00000000, /* 248 */
	0x00000000, /* 249 */
	0x00000000, /* 250 */
	0x00000000, /* 251 */
	0x00000000, /* 252 */
	0x008fd000, /* 253 */
	0x00091211, /* 254 */
	0x00c5d046, /* 255 */
};

/* From vec_720p_bvb_input.bss */
static const uint32_t s_aulRamBVBInput_720p[BVDC_P_RAM_TABLE_SIZE] =
{
	0x00a10000, /* 0 */
	0x00207028, /* 1 */
	0x00220028, /* 2 */
	0x002000b4, /* 3 */
	0x00280028, /* 4 */
	0x0028c01f, /* 5 */
	0x0028c4e1, /* 6 */
	0x0020001a, /* 7 */
	0x00210016, /* 8 */
	0x00800000, /* 9 */
	0x00000000, /* 10 */
	0x00000000, /* 11 */
	0x00000000, /* 12 */
	0x00000000, /* 13 */
	0x00000000, /* 14 */
	0x00000000, /* 15 */
	0x00000000, /* 16 */
	0x00000000, /* 17 */
	0x00000000, /* 18 */
	0x00000000, /* 19 */
	0x00aa0000, /* 20 */
	0x00261062, /* 21 */
	0x0027100b, /* 22 */
	0x00471001, /* 23 */
	0x002d100b, /* 24 */
	0x00451004, /* 25 */
	0x00252078, /* 26 */
	0x00442001, /* 27 */
	0x00442013, /* 28 */
	0x0040c2d0, /* 29 */
	0x00442005, /* 30 */
	0x00814000, /* 31 */
	0x00000000, /* 32 */
	0x00000000, /* 33 */
	0x00000000, /* 34 */
	0x00000000, /* 35 */
	0x00000000, /* 36 */
	0x00000000, /* 37 */
	0x00000000, /* 38 */
	0x00000000, /* 39 */
	0x00000000, /* 40 */
	0x00000000, /* 41 */
	0x00000000, /* 42 */
	0x00000000, /* 43 */
	0x00000000, /* 44 */
	0x00000000, /* 45 */
	0x00000000, /* 46 */
	0x00000000, /* 47 */
	0x00000000, /* 48 */
	0x00000000, /* 49 */
	0x00400001, /* 50 */
	0x00202027, /* 51 */
	0x0020301d, /* 52 */
	0x00832000, /* 53 */
	0x00000000, /* 54 */
	0x00000000, /* 55 */
	0x00000000, /* 56 */
	0x00000000, /* 57 */
	0x00000000, /* 58 */
	0x00000000, /* 59 */
	0x00000000, /* 60 */
	0x00000000, /* 61 */
	0x00000000, /* 62 */
	0x00000000, /* 63 */
	0x00000000, /* 64 */
	0x00000000, /* 65 */
	0x00000000, /* 66 */
	0x00000000, /* 67 */
	0x00000000, /* 68 */
	0x00000000, /* 69 */
	0x00000000, /* 70 */
	0x00000000, /* 71 */
	0x00000000, /* 72 */
	0x00000000, /* 73 */
	0x00000000, /* 74 */
	0x00000000, /* 75 */
	0x00000000, /* 76 */
	0x00000000, /* 77 */
	0x00000000, /* 78 */
	0x00000000, /* 79 */
	0x00000000, /* 80 */
	0x00000000, /* 81 */
	0x00000000, /* 82 */
	0x00000000, /* 83 */
	0x00000000, /* 84 */
	0x00000000, /* 85 */
	0x00000000, /* 86 */
	0x00000000, /* 87 */
	0x00000000, /* 88 */
	0x00000000, /* 89 */
	0x00000000, /* 90 */
	0x00000000, /* 91 */
	0x00000000, /* 92 */
	0x00000000, /* 93 */
	0x00000000, /* 94 */
	0x00000000, /* 95 */
	0x00000000, /* 96 */
	0x00000000, /* 97 */
	0x00000000, /* 98 */
	0x00000000, /* 99 */
	0x00000000, /* 100 */
	0x00000000, /* 101 */
	0x00000000, /* 102 */
	0x00000000, /* 103 */
	0x00000000, /* 104 */
	0x00000000, /* 105 */
	0x00000000, /* 106 */
	0x00000000, /* 107 */
	0x00000000, /* 108 */
	0x00000000, /* 109 */
	0x00000000, /* 110 */
	0x00000000, /* 111 */
	0x00000000, /* 112 */
	0x00000000, /* 113 */
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
	0x00000000, /* 126 */
	0x00000000, /* 127 */
	0x00000000, /* 128 */
	0x00000000, /* 129 */
	0x00000000, /* 130 */
	0x00000000, /* 131 */
	0x00000000, /* 132 */
	0x00000000, /* 133 */
	0x00000000, /* 134 */
	0x00000000, /* 135 */
	0x00000000, /* 136 */
	0x00000000, /* 137 */
	0x00000000, /* 138 */
	0x00000000, /* 139 */
	0x00000000, /* 140 */
	0x00000000, /* 141 */
	0x00000000, /* 142 */
	0x00000000, /* 143 */
	0x00000000, /* 144 */
	0x00000000, /* 145 */
	0x00000000, /* 146 */
	0x00000000, /* 147 */
	0x00000000, /* 148 */
	0x00000000, /* 149 */
	0x00000000, /* 150 */
	0x00000000, /* 151 */
	0x00000000, /* 152 */
	0x00000000, /* 153 */
	0x00000000, /* 154 */
	0x00000000, /* 155 */
	0x00000000, /* 156 */
	0x00000000, /* 157 */
	0x00000000, /* 158 */
	0x00000000, /* 159 */
	0x00000000, /* 160 */
	0x00000000, /* 161 */
	0x00000000, /* 162 */
	0x00000000, /* 163 */
	0x00000000, /* 164 */
	0x00000000, /* 165 */
	0x00000000, /* 166 */
	0x00000000, /* 167 */
	0x00000000, /* 168 */
	0x00000000, /* 169 */
	0x00000000, /* 170 */
	0x00000000, /* 171 */
	0x00000000, /* 172 */
	0x00000000, /* 173 */
	0x00000000, /* 174 */
	0x00000000, /* 175 */
	0x00000000, /* 176 */
	0x00000000, /* 177 */
	0x00000000, /* 178 */
	0x00000000, /* 179 */
	0x00000000, /* 180 */
	0x00000000, /* 181 */
	0x00000000, /* 182 */
	0x00000000, /* 183 */
	0x00000000, /* 184 */
	0x00000000, /* 185 */
	0x00000000, /* 186 */
	0x00000000, /* 187 */
	0x00000000, /* 188 */
	0x00000000, /* 189 */
	0x00000000, /* 190 */
	0x00000000, /* 191 */
	0x00000000, /* 192 */
	0x00000000, /* 193 */
	0x00000000, /* 194 */
	0x00000000, /* 195 */
	0x00000000, /* 196 */
	0x00000000, /* 197 */
	0x00000000, /* 198 */
	0x00000000, /* 199 */
	0x00000000, /* 200 */
	0x00000000, /* 201 */
	0x00000000, /* 202 */
	0x00000000, /* 203 */
	0x00000000, /* 204 */
	0x00000000, /* 205 */
	0x00000000, /* 206 */
	0x00000000, /* 207 */
	0x00000000, /* 208 */
	0x00000000, /* 209 */
	0x00000000, /* 210 */
	0x00000000, /* 211 */
	0x00000000, /* 212 */
	0x00000000, /* 213 */
	0x00000000, /* 214 */
	0x00000000, /* 215 */
	0x00000000, /* 216 */
	0x00000000, /* 217 */
	0x00000000, /* 218 */
	0x00000000, /* 219 */
	0x00000000, /* 220 */
	0x00000000, /* 221 */
	0x00000000, /* 222 */
	0x00000000, /* 223 */
	0x00000000, /* 224 */
	0x00000000, /* 225 */
	0x00000000, /* 226 */
	0x00000000, /* 227 */
	0x00000000, /* 228 */
	0x00000000, /* 229 */
	0x00000000, /* 230 */
	0x00000000, /* 231 */
	0x00000000, /* 232 */
	0x00000000, /* 233 */
	0x00000000, /* 234 */
	0x00000000, /* 235 */
	0x00000000, /* 236 */
	0x00000000, /* 237 */
	0x00000000, /* 238 */
	0x00000000, /* 239 */
	0x00000000, /* 240 */
	0x00000000, /* 241 */
	0x00000000, /* 242 */
	0x00000000, /* 243 */
	0x00000000, /* 244 */
	0x00000000, /* 245 */
	0x00000000, /* 246 */
	0x00000000, /* 247 */
	0x00000000, /* 248 */
	0x00000000, /* 249 */
	0x00000000, /* 250 */
	0x00000000, /* 251 */
	0x00000000, /* 252 */
	0x008fd000, /* 253 */
	0x00022610, /* 254 */
	0x2bf40f67, /* 255 */
};

/* From vec_720p_bvb_input.bss */
static const uint32_t s_aulRamBVBInput_720p_3D[BVDC_P_RAM_TABLE_SIZE] =
{
	0x00a24000, /* 0 */
	0x0024006e, /* 1 */
	0x00201028, /* 2 */
	0x002000dc, /* 3 */
	0x002084ea, /* 4 */
	0x00800000, /* 5 */
	0x00000000, /* 6 */
	0x00000000, /* 7 */
	0x00000000, /* 8 */
	0x00000000, /* 9 */
	0x00000000, /* 10 */
	0x00000000, /* 11 */
	0x00000000, /* 12 */
	0x00000000, /* 13 */
	0x00000000, /* 14 */
	0x00000000, /* 15 */
	0x00000000, /* 16 */
	0x00000000, /* 17 */
	0x00000000, /* 18 */
	0x00000000, /* 19 */
	0x00a80000, /* 20 */
	0x00421001, /* 21 */
	0x00401004, /* 22 */
	0x00400014, /* 23 */
	0x004085be, /* 24 */
	0x00400005, /* 25 */
	0x00a80000, /* 26 */
	0x00815000, /* 27 */
	0x00000000, /* 28 */
	0x00000000, /* 29 */
	0x00000000, /* 30 */
	0x00000000, /* 31 */
	0x00000000, /* 32 */
	0x00000000, /* 33 */
	0x00000000, /* 34 */
	0x00000000, /* 35 */
	0x00000000, /* 36 */
	0x00000000, /* 37 */
	0x00000000, /* 38 */
	0x00000000, /* 39 */
	0x00000000, /* 40 */
	0x00000000, /* 41 */
	0x00000000, /* 42 */
	0x00000000, /* 43 */
	0x00000000, /* 44 */
	0x00000000, /* 45 */
	0x00000000, /* 46 */
	0x00000000, /* 47 */
	0x00000000, /* 48 */
	0x00000000, /* 49 */
	0x00000000, /* 50 */
	0x00000000, /* 51 */
	0x00000000, /* 52 */
	0x00000000, /* 53 */
	0x00000000, /* 54 */
	0x00000000, /* 55 */
	0x00000000, /* 56 */
	0x00000000, /* 57 */
	0x00000000, /* 58 */
	0x00000000, /* 59 */
	0x00000000, /* 60 */
	0x00000000, /* 61 */
	0x00000000, /* 62 */
	0x00000000, /* 63 */
	0x00000000, /* 64 */
	0x00000000, /* 65 */
	0x00000000, /* 66 */
	0x00000000, /* 67 */
	0x00000000, /* 68 */
	0x00000000, /* 69 */
	0x00000000, /* 70 */
	0x00000000, /* 71 */
	0x00000000, /* 72 */
	0x00000000, /* 73 */
	0x00000000, /* 74 */
	0x00000000, /* 75 */
	0x00000000, /* 76 */
	0x00000000, /* 77 */
	0x00000000, /* 78 */
	0x00000000, /* 79 */
	0x00000000, /* 80 */
	0x00000000, /* 81 */
	0x00000000, /* 82 */
	0x00000000, /* 83 */
	0x00000000, /* 84 */
	0x00000000, /* 85 */
	0x00000000, /* 86 */
	0x00000000, /* 87 */
	0x00000000, /* 88 */
	0x00000000, /* 89 */
	0x00000000, /* 90 */
	0x00000000, /* 91 */
	0x00000000, /* 92 */
	0x00000000, /* 93 */
	0x00000000, /* 94 */
	0x00000000, /* 95 */
	0x00000000, /* 96 */
	0x00000000, /* 97 */
	0x00000000, /* 98 */
	0x00000000, /* 99 */
	0x00000000, /* 100 */
	0x00000000, /* 101 */
	0x00000000, /* 102 */
	0x00000000, /* 103 */
	0x00000000, /* 104 */
	0x00000000, /* 105 */
	0x00000000, /* 106 */
	0x00000000, /* 107 */
	0x00000000, /* 108 */
	0x00000000, /* 109 */
	0x00000000, /* 110 */
	0x00000000, /* 111 */
	0x00000000, /* 112 */
	0x00000000, /* 113 */
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
	0x00000000, /* 126 */
	0x00000000, /* 127 */
	0x00000000, /* 128 */
	0x00000000, /* 129 */
	0x00000000, /* 130 */
	0x00000000, /* 131 */
	0x00000000, /* 132 */
	0x00000000, /* 133 */
	0x00000000, /* 134 */
	0x00000000, /* 135 */
	0x00000000, /* 136 */
	0x00000000, /* 137 */
	0x00000000, /* 138 */
	0x00000000, /* 139 */
	0x00000000, /* 140 */
	0x00000000, /* 141 */
	0x00000000, /* 142 */
	0x00000000, /* 143 */
	0x00000000, /* 144 */
	0x00000000, /* 145 */
	0x00000000, /* 146 */
	0x00000000, /* 147 */
	0x00000000, /* 148 */
	0x00000000, /* 149 */
	0x00000000, /* 150 */
	0x00000000, /* 151 */
	0x00000000, /* 152 */
	0x00000000, /* 153 */
	0x00000000, /* 154 */
	0x00000000, /* 155 */
	0x00000000, /* 156 */
	0x00000000, /* 157 */
	0x00000000, /* 158 */
	0x00000000, /* 159 */
	0x00000000, /* 160 */
	0x00000000, /* 161 */
	0x00000000, /* 162 */
	0x00000000, /* 163 */
	0x00000000, /* 164 */
	0x00000000, /* 165 */
	0x00000000, /* 166 */
	0x00000000, /* 167 */
	0x00000000, /* 168 */
	0x00000000, /* 169 */
	0x00000000, /* 170 */
	0x00000000, /* 171 */
	0x00000000, /* 172 */
	0x00000000, /* 173 */
	0x00000000, /* 174 */
	0x00000000, /* 175 */
	0x00000000, /* 176 */
	0x00000000, /* 177 */
	0x00000000, /* 178 */
	0x00000000, /* 179 */
	0x00000000, /* 180 */
	0x00000000, /* 181 */
	0x00000000, /* 182 */
	0x00000000, /* 183 */
	0x00000000, /* 184 */
	0x00000000, /* 185 */
	0x00000000, /* 186 */
	0x00000000, /* 187 */
	0x00000000, /* 188 */
	0x00000000, /* 189 */
	0x00000000, /* 190 */
	0x00000000, /* 191 */
	0x00000000, /* 192 */
	0x00000000, /* 193 */
	0x00000000, /* 194 */
	0x00000000, /* 195 */
	0x00000000, /* 196 */
	0x00000000, /* 197 */
	0x00000000, /* 198 */
	0x00000000, /* 199 */
	0x00000000, /* 200 */
	0x00000000, /* 201 */
	0x00000000, /* 202 */
	0x00000000, /* 203 */
	0x00000000, /* 204 */
	0x00000000, /* 205 */
	0x00000000, /* 206 */
	0x00000000, /* 207 */
	0x00000000, /* 208 */
	0x00000000, /* 209 */
	0x00000000, /* 210 */
	0x00000000, /* 211 */
	0x00000000, /* 212 */
	0x00000000, /* 213 */
	0x00000000, /* 214 */
	0x00000000, /* 215 */
	0x00000000, /* 216 */
	0x00000000, /* 217 */
	0x00000000, /* 218 */
	0x00000000, /* 219 */
	0x00000000, /* 220 */
	0x00000000, /* 221 */
	0x00000000, /* 222 */
	0x00000000, /* 223 */
	0x00000000, /* 224 */
	0x00000000, /* 225 */
	0x00000000, /* 226 */
	0x00000000, /* 227 */
	0x00000000, /* 228 */
	0x00000000, /* 229 */
	0x00000000, /* 230 */
	0x00000000, /* 231 */
	0x00000000, /* 232 */
	0x00000000, /* 233 */
	0x00000000, /* 234 */
	0x00000000, /* 235 */
	0x00000000, /* 236 */
	0x00000000, /* 237 */
	0x00000000, /* 238 */
	0x00000000, /* 239 */
	0x00000000, /* 240 */
	0x00000000, /* 241 */
	0x00000000, /* 242 */
	0x00000000, /* 243 */
	0x00000000, /* 244 */
	0x00000000, /* 245 */
	0x00000000, /* 246 */
	0x00000000, /* 247 */
	0x00000000, /* 248 */
	0x00000000, /* 249 */
	0x00000000, /* 250 */
	0x00000000, /* 251 */
	0x00000000, /* 252 */
	0x008fd000, /* 253 */
	0x00012611, /* 254 */
	0x8e89f95c, /* 255 */
};

/* From vec_1250i_bvb_input.bss */
static const uint32_t s_aulRamBVBInput_1250i_50hz[] =
{
	0x00a10000, /* 0 */
	0x00213042, /* 1 */
	0x00240042, /* 2 */
	0x002000e8, /* 3 */
	0x0028000b, /* 4 */
	0x0028c252, /* 5 */
	0x002880c3, /* 6 */
	0x00298018, /* 7 */
	0x0029a042, /* 8 */
	0x00288411, /* 9 */
	0x0020003b, /* 10 */
	0x00800000, /* 11 */
	0x00000000, /* 12 */
	0x00000000, /* 13 */
	0x00000000, /* 14 */
	0x00000000, /* 15 */
	0x00000000, /* 16 */
	0x00000000, /* 17 */
	0x00000000, /* 18 */
	0x00000000, /* 19 */
	0x00470001, /* 20 */
	0x00a80000, /* 21 */
	0x004c5001, /* 22 */
	0x004e0001, /* 23 */
	0x00464001, /* 24 */
	0x00444001, /* 25 */
	0x0044409e, /* 26 */
	0x0040c438, /* 27 */
	0x00444008, /* 28 */
	0x00445001, /* 29 */
	0x00452001, /* 30 */
	0x004c40a0, /* 31 */
	0x0048c438, /* 32 */
	0x004c4008, /* 33 */
	0x00815000, /* 34 */
	0x00000000, /* 35 */
	0x00000000, /* 36 */
	0x00000000, /* 37 */
	0x00000000, /* 38 */
	0x00000000, /* 39 */
	0x00000000, /* 40 */
	0x00000000, /* 41 */
	0x00000000, /* 42 */
	0x00000000, /* 43 */
	0x00000000, /* 44 */
	0x00000000, /* 45 */
	0x00000000, /* 46 */
	0x00000000, /* 47 */
	0x00000000, /* 48 */
	0x00000000, /* 49 */
	0x00400001, /* 50 */
	0x00200041, /* 51 */
	0x00202041, /* 52 */
	0x00200463, /* 53 */
	0x00201042, /* 54 */
	0x0020000b, /* 55 */
	0x00832000, /* 56 */
	0x00000000, /* 57 */
	0x00000000, /* 58 */
	0x00000000, /* 59 */
	0x00000000, /* 60 */
	0x00000000, /* 61 */
	0x00000000, /* 62 */
	0x00000000, /* 63 */
	0x00000000, /* 64 */
	0x00000000, /* 65 */
	0x00000000, /* 66 */
	0x00000000, /* 67 */
	0x00000000, /* 68 */
	0x00000000, /* 69 */
	0x00000000, /* 70 */
	0x00000000, /* 71 */
	0x00000000, /* 72 */
	0x00000000, /* 73 */
	0x00000000, /* 74 */
	0x00000000, /* 75 */
	0x00000000, /* 76 */
	0x00000000, /* 77 */
	0x00000000, /* 78 */
	0x00000000, /* 79 */
	0x00000000, /* 80 */
	0x00000000, /* 81 */
	0x00000000, /* 82 */
	0x00000000, /* 83 */
	0x00000000, /* 84 */
	0x00000000, /* 85 */
	0x00000000, /* 86 */
	0x00000000, /* 87 */
	0x00000000, /* 88 */
	0x00000000, /* 89 */
	0x00000000, /* 90 */
	0x00000000, /* 91 */
	0x00000000, /* 92 */
	0x00000000, /* 93 */
	0x00000000, /* 94 */
	0x00000000, /* 95 */
	0x00000000, /* 96 */
	0x00000000, /* 97 */
	0x00000000, /* 98 */
	0x00000000, /* 99 */
	0x00000000, /* 100 */
	0x00000000, /* 101 */
	0x00000000, /* 102 */
	0x00000000, /* 103 */
	0x00000000, /* 104 */
	0x00000000, /* 105 */
	0x00000000, /* 106 */
	0x00000000, /* 107 */
	0x00000000, /* 108 */
	0x00000000, /* 109 */
	0x00000000, /* 110 */
	0x00000000, /* 111 */
	0x00000000, /* 112 */
	0x00000000, /* 113 */
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
	0x00000000, /* 126 */
	0x00000000, /* 127 */
	0x00000000, /* 128 */
	0x00000000, /* 129 */
	0x00000000, /* 130 */
	0x00000000, /* 131 */
	0x00000000, /* 132 */
	0x00000000, /* 133 */
	0x00000000, /* 134 */
	0x00000000, /* 135 */
	0x00000000, /* 136 */
	0x00000000, /* 137 */
	0x00000000, /* 138 */
	0x00000000, /* 139 */
	0x00000000, /* 140 */
	0x00000000, /* 141 */
	0x00000000, /* 142 */
	0x00000000, /* 143 */
	0x00000000, /* 144 */
	0x00000000, /* 145 */
	0x00000000, /* 146 */
	0x00000000, /* 147 */
	0x00000000, /* 148 */
	0x00000000, /* 149 */
	0x00000000, /* 150 */
	0x00000000, /* 151 */
	0x00000000, /* 152 */
	0x00000000, /* 153 */
	0x00000000, /* 154 */
	0x00000000, /* 155 */
	0x00000000, /* 156 */
	0x00000000, /* 157 */
	0x00000000, /* 158 */
	0x00000000, /* 159 */
	0x00000000, /* 160 */
	0x00000000, /* 161 */
	0x00000000, /* 162 */
	0x00000000, /* 163 */
	0x00000000, /* 164 */
	0x00000000, /* 165 */
	0x00000000, /* 166 */
	0x00000000, /* 167 */
	0x00000000, /* 168 */
	0x00000000, /* 169 */
	0x00000000, /* 170 */
	0x00000000, /* 171 */
	0x00000000, /* 172 */
	0x00000000, /* 173 */
	0x00000000, /* 174 */
	0x00000000, /* 175 */
	0x00000000, /* 176 */
	0x00000000, /* 177 */
	0x00000000, /* 178 */
	0x00000000, /* 179 */
	0x00000000, /* 180 */
	0x00000000, /* 181 */
	0x00000000, /* 182 */
	0x00000000, /* 183 */
	0x00000000, /* 184 */
	0x00000000, /* 185 */
	0x00000000, /* 186 */
	0x00000000, /* 187 */
	0x00000000, /* 188 */
	0x00000000, /* 189 */
	0x00000000, /* 190 */
	0x00000000, /* 191 */
	0x00000000, /* 192 */
	0x00000000, /* 193 */
	0x00000000, /* 194 */
	0x00000000, /* 195 */
	0x00000000, /* 196 */
	0x00000000, /* 197 */
	0x00000000, /* 198 */
	0x00000000, /* 199 */
	0x00000000, /* 200 */
	0x00000000, /* 201 */
	0x00000000, /* 202 */
	0x00000000, /* 203 */
	0x00000000, /* 204 */
	0x00000000, /* 205 */
	0x00000000, /* 206 */
	0x00000000, /* 207 */
	0x00000000, /* 208 */
	0x00000000, /* 209 */
	0x00000000, /* 210 */
	0x00000000, /* 211 */
	0x00000000, /* 212 */
	0x00000000, /* 213 */
	0x00000000, /* 214 */
	0x00000000, /* 215 */
	0x00000000, /* 216 */
	0x00000000, /* 217 */
	0x00000000, /* 218 */
	0x00000000, /* 219 */
	0x00000000, /* 220 */
	0x00000000, /* 221 */
	0x00000000, /* 222 */
	0x00000000, /* 223 */
	0x00000000, /* 224 */
	0x00000000, /* 225 */
	0x00000000, /* 226 */
	0x00000000, /* 227 */
	0x00000000, /* 228 */
	0x00000000, /* 229 */
	0x00000000, /* 230 */
	0x00000000, /* 231 */
	0x00000000, /* 232 */
	0x00000000, /* 233 */
	0x00000000, /* 234 */
	0x00000000, /* 235 */
	0x00000000, /* 236 */
	0x00000000, /* 237 */
	0x00000000, /* 238 */
	0x00000000, /* 239 */
	0x00000000, /* 240 */
	0x00000000, /* 241 */
	0x00000000, /* 242 */
	0x00000000, /* 243 */
	0x00000000, /* 244 */
	0x00000000, /* 245 */
	0x00000000, /* 246 */
	0x00000000, /* 247 */
	0x00000000, /* 248 */
	0x00000000, /* 249 */
	0x00000000, /* 250 */
	0x00000000, /* 251 */
	0x00000000, /* 252 */
	0x008fd000, /* 253 */
	0x00012210, /* 254 */
	0x0f960ac9, /* 255 */
};

/* vec_1125i50hz_bvb_input.bss */
static const uint32_t s_aulRamBVBInput_1125i_50hz[] =
{
	0x00a10000, /* 0 */
	0x0021702c, /* 1 */
	0x0024002c, /* 2 */
	0x00200058, /* 3 */
	0x0028403c, /* 4 */
	0x0028c334, /* 5 */
	0x00288108, /* 6 */
	0x0029e02c, /* 7 */
	0x00288084, /* 8 */
	0x0028c294, /* 9 */
	0x002840dc, /* 10 */
	0x002000f2, /* 11 */
	0x00800000, /* 12 */
	0x00000000, /* 13 */
	0x00000000, /* 14 */
	0x00000000, /* 15 */
	0x00000000, /* 16 */
	0x00000000, /* 17 */
	0x00000000, /* 18 */
	0x00000000, /* 19 */
	0x00450001, /* 20 */
	0x00a80000, /* 21 */
	0x004c4002, /* 22 */
	0x00471001, /* 23 */
	0x00451009, /* 24 */
	0x00452002, /* 25 */
	0x0044401c, /* 26 */
	0x0040c438, /* 27 */
	0x00444004, /* 28 */
	0x00452001, /* 29 */
	0x004d100a, /* 30 */
	0x004d2001, /* 31 */
	0x004c401e, /* 32 */
	0x0048c438, /* 33 */
	0x004c4002, /* 34 */
	0x00a80000, /* 35 */
	0x00816000, /* 36 */
	0x00000000, /* 37 */
	0x00000000, /* 38 */
	0x00000000, /* 39 */
	0x00000000, /* 40 */
	0x00000000, /* 41 */
	0x00000000, /* 42 */
	0x00000000, /* 43 */
	0x00000000, /* 44 */
	0x00000000, /* 45 */
	0x00000000, /* 46 */
	0x00000000, /* 47 */
	0x00000000, /* 48 */
	0x00000000, /* 49 */
	0x00400001, /* 50 */
	0x00200027, /* 51 */
	0x0020202b, /* 52 */
	0x00200501, /* 53 */
	0x0020102c, /* 54 */
	0x0020000b, /* 55 */
	0x00832000, /* 56 */
	0x00000000, /* 57 */
	0x00000000, /* 58 */
	0x00000000, /* 59 */
	0x00000000, /* 60 */
	0x00000000, /* 61 */
	0x00000000, /* 62 */
	0x00000000, /* 63 */
	0x00000000, /* 64 */
	0x00000000, /* 65 */
	0x00000000, /* 66 */
	0x00000000, /* 67 */
	0x00000000, /* 68 */
	0x00000000, /* 69 */
	0x00000000, /* 70 */
	0x00000000, /* 71 */
	0x00000000, /* 72 */
	0x00000000, /* 73 */
	0x00000000, /* 74 */
	0x00000000, /* 75 */
	0x00000000, /* 76 */
	0x00000000, /* 77 */
	0x00000000, /* 78 */
	0x00000000, /* 79 */
	0x00000000, /* 80 */
	0x00000000, /* 81 */
	0x00000000, /* 82 */
	0x00000000, /* 83 */
	0x00000000, /* 84 */
	0x00000000, /* 85 */
	0x00000000, /* 86 */
	0x00000000, /* 87 */
	0x00000000, /* 88 */
	0x00000000, /* 89 */
	0x00000000, /* 90 */
	0x00000000, /* 91 */
	0x00000000, /* 92 */
	0x00000000, /* 93 */
	0x00000000, /* 94 */
	0x00000000, /* 95 */
	0x00000000, /* 96 */
	0x00000000, /* 97 */
	0x00000000, /* 98 */
	0x00000000, /* 99 */
	0x00000000, /* 100 */
	0x00000000, /* 101 */
	0x00000000, /* 102 */
	0x00000000, /* 103 */
	0x00000000, /* 104 */
	0x00000000, /* 105 */
	0x00000000, /* 106 */
	0x00000000, /* 107 */
	0x00000000, /* 108 */
	0x00000000, /* 109 */
	0x00000000, /* 110 */
	0x00000000, /* 111 */
	0x00000000, /* 112 */
	0x00000000, /* 113 */
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
	0x00000000, /* 126 */
	0x00000000, /* 127 */
	0x00000000, /* 128 */
	0x00000000, /* 129 */
	0x00000000, /* 130 */
	0x00000000, /* 131 */
	0x00000000, /* 132 */
	0x00000000, /* 133 */
	0x00000000, /* 134 */
	0x00000000, /* 135 */
	0x00000000, /* 136 */
	0x00000000, /* 137 */
	0x00000000, /* 138 */
	0x00000000, /* 139 */
	0x00000000, /* 140 */
	0x00000000, /* 141 */
	0x00000000, /* 142 */
	0x00000000, /* 143 */
	0x00000000, /* 144 */
	0x00000000, /* 145 */
	0x00000000, /* 146 */
	0x00000000, /* 147 */
	0x00000000, /* 148 */
	0x00000000, /* 149 */
	0x00000000, /* 150 */
	0x00000000, /* 151 */
	0x00000000, /* 152 */
	0x00000000, /* 153 */
	0x00000000, /* 154 */
	0x00000000, /* 155 */
	0x00000000, /* 156 */
	0x00000000, /* 157 */
	0x00000000, /* 158 */
	0x00000000, /* 159 */
	0x00000000, /* 160 */
	0x00000000, /* 161 */
	0x00000000, /* 162 */
	0x00000000, /* 163 */
	0x00000000, /* 164 */
	0x00000000, /* 165 */
	0x00000000, /* 166 */
	0x00000000, /* 167 */
	0x00000000, /* 168 */
	0x00000000, /* 169 */
	0x00000000, /* 170 */
	0x00000000, /* 171 */
	0x00000000, /* 172 */
	0x00000000, /* 173 */
	0x00000000, /* 174 */
	0x00000000, /* 175 */
	0x00000000, /* 176 */
	0x00000000, /* 177 */
	0x00000000, /* 178 */
	0x00000000, /* 179 */
	0x00000000, /* 180 */
	0x00000000, /* 181 */
	0x00000000, /* 182 */
	0x00000000, /* 183 */
	0x00000000, /* 184 */
	0x00000000, /* 185 */
	0x00000000, /* 186 */
	0x00000000, /* 187 */
	0x00000000, /* 188 */
	0x00000000, /* 189 */
	0x00000000, /* 190 */
	0x00000000, /* 191 */
	0x00000000, /* 192 */
	0x00000000, /* 193 */
	0x00000000, /* 194 */
	0x00000000, /* 195 */
	0x00000000, /* 196 */
	0x00000000, /* 197 */
	0x00000000, /* 198 */
	0x00000000, /* 199 */
	0x00000000, /* 200 */
	0x00000000, /* 201 */
	0x00000000, /* 202 */
	0x00000000, /* 203 */
	0x00000000, /* 204 */
	0x00000000, /* 205 */
	0x00000000, /* 206 */
	0x00000000, /* 207 */
	0x00000000, /* 208 */
	0x00000000, /* 209 */
	0x00000000, /* 210 */
	0x00000000, /* 211 */
	0x00000000, /* 212 */
	0x00000000, /* 213 */
	0x00000000, /* 214 */
	0x00000000, /* 215 */
	0x00000000, /* 216 */
	0x00000000, /* 217 */
	0x00000000, /* 218 */
	0x00000000, /* 219 */
	0x00000000, /* 220 */
	0x00000000, /* 221 */
	0x00000000, /* 222 */
	0x00000000, /* 223 */
	0x00000000, /* 224 */
	0x00000000, /* 225 */
	0x00000000, /* 226 */
	0x00000000, /* 227 */
	0x00000000, /* 228 */
	0x00000000, /* 229 */
	0x00000000, /* 230 */
	0x00000000, /* 231 */
	0x00000000, /* 232 */
	0x00000000, /* 233 */
	0x00000000, /* 234 */
	0x00000000, /* 235 */
	0x00000000, /* 236 */
	0x00000000, /* 237 */
	0x00000000, /* 238 */
	0x00000000, /* 239 */
	0x00000000, /* 240 */
	0x00000000, /* 241 */
	0x00000000, /* 242 */
	0x00000000, /* 243 */
	0x00000000, /* 244 */
	0x00000000, /* 245 */
	0x00000000, /* 246 */
	0x00000000, /* 247 */
	0x00000000, /* 248 */
	0x00000000, /* 249 */
	0x00000000, /* 250 */
	0x00000000, /* 251 */
	0x00000000, /* 252 */
	0x008fd000, /* 253 */
	0x00091712, /* 254 */
	0x343eff3c, /* 255 */
};

/* IT_Microcode for 720p 24Hz */
static const uint32_t s_aulRamBVBInput_720p_24hz[] =
{
	0x00a30000, /* 0 */
	0x00207028, /* 1 */
	0x00220028, /* 2 */
	0x002000b4, /* 3 */
	0x00280028, /* 4 */
	0x0028c01f, /* 5 */
	0x0028c4e1, /* 6 */
	0x002109db, /* 7 */
	0x00800000, /* 8 */
	0x00000000, /* 9 */
	0x00000000, /* 10 */
	0x00000000, /* 11 */
	0x00000000, /* 12 */
	0x00000000, /* 13 */
	0x00000000, /* 14 */
	0x00000000, /* 15 */
	0x00000000, /* 16 */
	0x00000000, /* 17 */
	0x00000000, /* 18 */
	0x00000000, /* 19 */
	0x00a80000, /* 20 */
	0x00461001, /* 21 */
	0x002c100b, /* 22 */
	0x00441004, /* 23 */
	0x00442014, /* 24 */
	0x0040c2d0, /* 25 */
	0x00442005, /* 26 */
	0x00814000, /* 27 */
	0x00000000, /* 28 */
	0x00000000, /* 29 */
	0x00400001, /* 30 */
	0x00200027, /* 31 */
	0x0020301d, /* 32 */
	0x0081e000, /* 33 */
	0x00000000, /* 34 */
	0x00000000, /* 35 */
	0x00000000, /* 36 */
	0x00000000, /* 37 */
	0x00000000, /* 38 */
	0x00000000, /* 39 */
	0x00000000, /* 40 */
	0x00000000, /* 41 */
	0x00000000, /* 42 */
	0x00000000, /* 43 */
	0x00000000, /* 44 */
	0x00000000, /* 45 */
	0x00000000, /* 46 */
	0x00000000, /* 47 */
	0x00000000, /* 48 */
	0x00000000, /* 49 */
	0x00000000, /* 50 */
	0x00000000, /* 51 */
	0x00000000, /* 52 */
	0x00000000, /* 53 */
	0x00000000, /* 54 */
	0x00000000, /* 55 */
	0x00000000, /* 56 */
	0x00000000, /* 57 */
	0x00000000, /* 58 */
	0x00000000, /* 59 */
	0x00000000, /* 60 */
	0x00000000, /* 61 */
	0x00000000, /* 62 */
	0x00000000, /* 63 */
	0x00000000, /* 64 */
	0x00000000, /* 65 */
	0x00000000, /* 66 */
	0x00000000, /* 67 */
	0x00000000, /* 68 */
	0x00000000, /* 69 */
	0x00000000, /* 70 */
	0x00000000, /* 71 */
	0x00000000, /* 72 */
	0x00000000, /* 73 */
	0x00000000, /* 74 */
	0x00000000, /* 75 */
	0x00000000, /* 76 */
	0x00000000, /* 77 */
	0x00000000, /* 78 */
	0x00000000, /* 79 */
	0x00000000, /* 80 */
	0x00000000, /* 81 */
	0x00000000, /* 82 */
	0x00000000, /* 83 */
	0x00000000, /* 84 */
	0x00000000, /* 85 */
	0x00000000, /* 86 */
	0x00000000, /* 87 */
	0x00000000, /* 88 */
	0x00000000, /* 89 */
	0x00000000, /* 90 */
	0x00000000, /* 91 */
	0x00000000, /* 92 */
	0x00000000, /* 93 */
	0x00000000, /* 94 */
	0x00000000, /* 95 */
	0x00000000, /* 96 */
	0x00000000, /* 97 */
	0x00000000, /* 98 */
	0x00000000, /* 99 */
	0x00000000, /* 100 */
	0x00000000, /* 101 */
	0x00000000, /* 102 */
	0x00000000, /* 103 */
	0x00000000, /* 104 */
	0x00000000, /* 105 */
	0x00000000, /* 106 */
	0x00000000, /* 107 */
	0x00000000, /* 108 */
	0x00000000, /* 109 */
	0x00000000, /* 110 */
	0x00000000, /* 111 */
	0x00000000, /* 112 */
	0x00000000, /* 113 */
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
	0x00000000, /* 126 */
	0x00000000, /* 127 */
	0x00000000, /* 128 */
	0x00000000, /* 129 */
	0x00000000, /* 130 */
	0x00000000, /* 131 */
	0x00000000, /* 132 */
	0x00000000, /* 133 */
	0x00000000, /* 134 */
	0x00000000, /* 135 */
	0x00000000, /* 136 */
	0x00000000, /* 137 */
	0x00000000, /* 138 */
	0x00000000, /* 139 */
	0x00000000, /* 140 */
	0x00000000, /* 141 */
	0x00000000, /* 142 */
	0x00000000, /* 143 */
	0x00000000, /* 144 */
	0x00000000, /* 145 */
	0x00000000, /* 146 */
	0x00000000, /* 147 */
	0x00000000, /* 148 */
	0x00000000, /* 149 */
	0x00000000, /* 150 */
	0x00000000, /* 151 */
	0x00000000, /* 152 */
	0x00000000, /* 153 */
	0x00000000, /* 154 */
	0x00000000, /* 155 */
	0x00000000, /* 156 */
	0x00000000, /* 157 */
	0x00000000, /* 158 */
	0x00000000, /* 159 */
	0x00000000, /* 160 */
	0x00000000, /* 161 */
	0x00000000, /* 162 */
	0x00000000, /* 163 */
	0x00000000, /* 164 */
	0x00000000, /* 165 */
	0x00000000, /* 166 */
	0x00000000, /* 167 */
	0x00000000, /* 168 */
	0x00000000, /* 169 */
	0x00000000, /* 170 */
	0x00000000, /* 171 */
	0x00000000, /* 172 */
	0x00000000, /* 173 */
	0x00000000, /* 174 */
	0x00000000, /* 175 */
	0x00000000, /* 176 */
	0x00000000, /* 177 */
	0x00000000, /* 178 */
	0x00000000, /* 179 */
	0x00000000, /* 180 */
	0x00000000, /* 181 */
	0x00000000, /* 182 */
	0x00000000, /* 183 */
	0x00000000, /* 184 */
	0x00000000, /* 185 */
	0x00000000, /* 186 */
	0x00000000, /* 187 */
	0x00000000, /* 188 */
	0x00000000, /* 189 */
	0x00000000, /* 190 */
	0x00000000, /* 191 */
	0x00000000, /* 192 */
	0x00000000, /* 193 */
	0x00000000, /* 194 */
	0x00000000, /* 195 */
	0x00000000, /* 196 */
	0x00000000, /* 197 */
	0x00000000, /* 198 */
	0x00000000, /* 199 */
	0x00000000, /* 200 */
	0x00000000, /* 201 */
	0x00000000, /* 202 */
	0x00000000, /* 203 */
	0x00000000, /* 204 */
	0x00000000, /* 205 */
	0x00000000, /* 206 */
	0x00000000, /* 207 */
	0x00000000, /* 208 */
	0x00000000, /* 209 */
	0x00000000, /* 210 */
	0x00000000, /* 211 */
	0x00000000, /* 212 */
	0x00000000, /* 213 */
	0x00000000, /* 214 */
	0x00000000, /* 215 */
	0x00000000, /* 216 */
	0x00000000, /* 217 */
	0x00000000, /* 218 */
	0x00000000, /* 219 */
	0x00000000, /* 220 */
	0x00000000, /* 221 */
	0x00000000, /* 222 */
	0x00000000, /* 223 */
	0x00000000, /* 224 */
	0x00000000, /* 225 */
	0x00000000, /* 226 */
	0x00000000, /* 227 */
	0x00000000, /* 228 */
	0x00000000, /* 229 */
	0x00000000, /* 230 */
	0x00000000, /* 231 */
	0x00000000, /* 232 */
	0x00000000, /* 233 */
	0x00000000, /* 234 */
	0x00000000, /* 235 */
	0x00000000, /* 236 */
	0x00000000, /* 237 */
	0x00000000, /* 238 */
	0x00000000, /* 239 */
	0x00000000, /* 240 */
	0x00000000, /* 241 */
	0x00000000, /* 242 */
	0x00000000, /* 243 */
	0x00000000, /* 244 */
	0x00000000, /* 245 */
	0x00000000, /* 246 */
	0x00000000, /* 247 */
	0x00000000, /* 248 */
	0x00000000, /* 249 */
	0x00000000, /* 250 */
	0x00000000, /* 251 */
	0x00000000, /* 252 */
	0x00000000, /* 253 */
	0x00041906, /* 254 */
	0x730f7f2f, /* 255 */
};


/* IT_Microcode for 720p 25Hz */
/* From vec_720p25hz_bvb_input.bss */
static const uint32_t s_aulRamBVBInput_720p_25hz[] =
{
	0x00a30000, /* 0 */
	0x00207028, /* 1 */
	0x00220028, /* 2 */
	0x002000b4, /* 3 */
	0x00280028, /* 4 */
	0x0028c01f, /* 5 */
	0x0028c4e1, /* 6 */
	0x00200920, /* 7 */
	0x00210016, /* 8 */
	0x00800000, /* 9 */
	0x00000000, /* 10 */
	0x00000000, /* 11 */
	0x00000000, /* 12 */
	0x00000000, /* 13 */
	0x00000000, /* 14 */
	0x00000000, /* 15 */
	0x00000000, /* 16 */
	0x00000000, /* 17 */
	0x00000000, /* 18 */
	0x00000000, /* 19 */
	0x00a80000, /* 20 */
	0x00461001, /* 21 */
	0x002c100b, /* 22 */
	0x00441004, /* 23 */
	0x00442014, /* 24 */
	0x0040c2d0, /* 25 */
	0x00442005, /* 26 */
	0x00814000, /* 27 */
	0x00000000, /* 28 */
	0x00000000, /* 29 */
	0x00000000, /* 30 */
	0x00000000, /* 31 */
	0x00000000, /* 32 */
	0x00000000, /* 33 */
	0x00000000, /* 34 */
	0x00000000, /* 35 */
	0x00000000, /* 36 */
	0x00000000, /* 37 */
	0x00000000, /* 38 */
	0x00000000, /* 39 */
	0x00000000, /* 40 */
	0x00000000, /* 41 */
	0x00000000, /* 42 */
	0x00000000, /* 43 */
	0x00000000, /* 44 */
	0x00000000, /* 45 */
	0x00000000, /* 46 */
	0x00000000, /* 47 */
	0x00000000, /* 48 */
	0x00000000, /* 49 */
	0x00400001, /* 50 */
	0x00202027, /* 51 */
	0x0020301d, /* 52 */
	0x00832000, /* 53 */
	0x00000000, /* 54 */
	0x00000000, /* 55 */
	0x00000000, /* 56 */
	0x00000000, /* 57 */
	0x00000000, /* 58 */
	0x00000000, /* 59 */
	0x00000000, /* 60 */
	0x00000000, /* 61 */
	0x00000000, /* 62 */
	0x00000000, /* 63 */
	0x00000000, /* 64 */
	0x00000000, /* 65 */
	0x00000000, /* 66 */
	0x00000000, /* 67 */
	0x00000000, /* 68 */
	0x00000000, /* 69 */
	0x00000000, /* 70 */
	0x00000000, /* 71 */
	0x00000000, /* 72 */
	0x00000000, /* 73 */
	0x00000000, /* 74 */
	0x00000000, /* 75 */
	0x00000000, /* 76 */
	0x00000000, /* 77 */
	0x00000000, /* 78 */
	0x00000000, /* 79 */
	0x00000000, /* 80 */
	0x00000000, /* 81 */
	0x00000000, /* 82 */
	0x00000000, /* 83 */
	0x00000000, /* 84 */
	0x00000000, /* 85 */
	0x00000000, /* 86 */
	0x00000000, /* 87 */
	0x00000000, /* 88 */
	0x00000000, /* 89 */
	0x00000000, /* 90 */
	0x00000000, /* 91 */
	0x00000000, /* 92 */
	0x00000000, /* 93 */
	0x00000000, /* 94 */
	0x00000000, /* 95 */
	0x00000000, /* 96 */
	0x00000000, /* 97 */
	0x00000000, /* 98 */
	0x00000000, /* 99 */
	0x00000000, /* 100 */
	0x00000000, /* 101 */
	0x00000000, /* 102 */
	0x00000000, /* 103 */
	0x00000000, /* 104 */
	0x00000000, /* 105 */
	0x00000000, /* 106 */
	0x00000000, /* 107 */
	0x00000000, /* 108 */
	0x00000000, /* 109 */
	0x00000000, /* 110 */
	0x00000000, /* 111 */
	0x00000000, /* 112 */
	0x00000000, /* 113 */
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
	0x00000000, /* 126 */
	0x00000000, /* 127 */
	0x00000000, /* 128 */
	0x00000000, /* 129 */
	0x00000000, /* 130 */
	0x00000000, /* 131 */
	0x00000000, /* 132 */
	0x00000000, /* 133 */
	0x00000000, /* 134 */
	0x00000000, /* 135 */
	0x00000000, /* 136 */
	0x00000000, /* 137 */
	0x00000000, /* 138 */
	0x00000000, /* 139 */
	0x00000000, /* 140 */
	0x00000000, /* 141 */
	0x00000000, /* 142 */
	0x00000000, /* 143 */
	0x00000000, /* 144 */
	0x00000000, /* 145 */
	0x00000000, /* 146 */
	0x00000000, /* 147 */
	0x00000000, /* 148 */
	0x00000000, /* 149 */
	0x00000000, /* 150 */
	0x00000000, /* 151 */
	0x00000000, /* 152 */
	0x00000000, /* 153 */
	0x00000000, /* 154 */
	0x00000000, /* 155 */
	0x00000000, /* 156 */
	0x00000000, /* 157 */
	0x00000000, /* 158 */
	0x00000000, /* 159 */
	0x00000000, /* 160 */
	0x00000000, /* 161 */
	0x00000000, /* 162 */
	0x00000000, /* 163 */
	0x00000000, /* 164 */
	0x00000000, /* 165 */
	0x00000000, /* 166 */
	0x00000000, /* 167 */
	0x00000000, /* 168 */
	0x00000000, /* 169 */
	0x00000000, /* 170 */
	0x00000000, /* 171 */
	0x00000000, /* 172 */
	0x00000000, /* 173 */
	0x00000000, /* 174 */
	0x00000000, /* 175 */
	0x00000000, /* 176 */
	0x00000000, /* 177 */
	0x00000000, /* 178 */
	0x00000000, /* 179 */
	0x00000000, /* 180 */
	0x00000000, /* 181 */
	0x00000000, /* 182 */
	0x00000000, /* 183 */
	0x00000000, /* 184 */
	0x00000000, /* 185 */
	0x00000000, /* 186 */
	0x00000000, /* 187 */
	0x00000000, /* 188 */
	0x00000000, /* 189 */
	0x00000000, /* 190 */
	0x00000000, /* 191 */
	0x00000000, /* 192 */
	0x00000000, /* 193 */
	0x00000000, /* 194 */
	0x00000000, /* 195 */
	0x00000000, /* 196 */
	0x00000000, /* 197 */
	0x00000000, /* 198 */
	0x00000000, /* 199 */
	0x00000000, /* 200 */
	0x00000000, /* 201 */
	0x00000000, /* 202 */
	0x00000000, /* 203 */
	0x00000000, /* 204 */
	0x00000000, /* 205 */
	0x00000000, /* 206 */
	0x00000000, /* 207 */
	0x00000000, /* 208 */
	0x00000000, /* 209 */
	0x00000000, /* 210 */
	0x00000000, /* 211 */
	0x00000000, /* 212 */
	0x00000000, /* 213 */
	0x00000000, /* 214 */
	0x00000000, /* 215 */
	0x00000000, /* 216 */
	0x00000000, /* 217 */
	0x00000000, /* 218 */
	0x00000000, /* 219 */
	0x00000000, /* 220 */
	0x00000000, /* 221 */
	0x00000000, /* 222 */
	0x00000000, /* 223 */
	0x00000000, /* 224 */
	0x00000000, /* 225 */
	0x00000000, /* 226 */
	0x00000000, /* 227 */
	0x00000000, /* 228 */
	0x00000000, /* 229 */
	0x00000000, /* 230 */
	0x00000000, /* 231 */
	0x00000000, /* 232 */
	0x00000000, /* 233 */
	0x00000000, /* 234 */
	0x00000000, /* 235 */
	0x00000000, /* 236 */
	0x00000000, /* 237 */
	0x00000000, /* 238 */
	0x00000000, /* 239 */
	0x00000000, /* 240 */
	0x00000000, /* 241 */
	0x00000000, /* 242 */
	0x00000000, /* 243 */
	0x00000000, /* 244 */
	0x00000000, /* 245 */
	0x00000000, /* 246 */
	0x00000000, /* 247 */
	0x00000000, /* 248 */
	0x00000000, /* 249 */
	0x00000000, /* 250 */
	0x00000000, /* 251 */
	0x00000000, /* 252 */
	0x008fd000, /* 253 */
	0x00020410, /* 254 */
	0xa366ed1f, /* 255 */
};

/* IT_Microcode for 720p 30Hz */
/* From vec_720p30hz_bvb_input.bss */
static const uint32_t s_aulRamBVBInput_720p_30hz[] =
{
	0x00a30000, /* 0 */
	0x00207028, /* 1 */
	0x00220028, /* 2 */
	0x002000b4, /* 3 */
	0x00280028, /* 4 */
	0x0028c01f, /* 5 */
	0x0028c4e1, /* 6 */
	0x0020068c, /* 7 */
	0x00210016, /* 8 */
	0x00800000, /* 9 */
	0x00000000, /* 10 */
	0x00000000, /* 11 */
	0x00000000, /* 12 */
	0x00000000, /* 13 */
	0x00000000, /* 14 */
	0x00000000, /* 15 */
	0x00000000, /* 16 */
	0x00000000, /* 17 */
	0x00000000, /* 18 */
	0x00000000, /* 19 */
	0x00aa0000, /* 20 */
	0x00261062, /* 21 */
	0x0027100b, /* 22 */
	0x00471001, /* 23 */
	0x002d100b, /* 24 */
	0x00451004, /* 25 */
	0x00252078, /* 26 */
	0x00442001, /* 27 */
	0x00442013, /* 28 */
	0x0040c2d0, /* 29 */
	0x00442005, /* 30 */
	0x00814000, /* 31 */
	0x00000000, /* 32 */
	0x00000000, /* 33 */
	0x00000000, /* 34 */
	0x00000000, /* 35 */
	0x00000000, /* 36 */
	0x00000000, /* 37 */
	0x00000000, /* 38 */
	0x00000000, /* 39 */
	0x00000000, /* 40 */
	0x00000000, /* 41 */
	0x00000000, /* 42 */
	0x00000000, /* 43 */
	0x00000000, /* 44 */
	0x00000000, /* 45 */
	0x00000000, /* 46 */
	0x00000000, /* 47 */
	0x00000000, /* 48 */
	0x00000000, /* 49 */
	0x00400001, /* 50 */
	0x00202027, /* 51 */
	0x0020301d, /* 52 */
	0x00832000, /* 53 */
	0x00000000, /* 54 */
	0x00000000, /* 55 */
	0x00000000, /* 56 */
	0x00000000, /* 57 */
	0x00000000, /* 58 */
	0x00000000, /* 59 */
	0x00000000, /* 60 */
	0x00000000, /* 61 */
	0x00000000, /* 62 */
	0x00000000, /* 63 */
	0x00000000, /* 64 */
	0x00000000, /* 65 */
	0x00000000, /* 66 */
	0x00000000, /* 67 */
	0x00000000, /* 68 */
	0x00000000, /* 69 */
	0x00000000, /* 70 */
	0x00000000, /* 71 */
	0x00000000, /* 72 */
	0x00000000, /* 73 */
	0x00000000, /* 74 */
	0x00000000, /* 75 */
	0x00000000, /* 76 */
	0x00000000, /* 77 */
	0x00000000, /* 78 */
	0x00000000, /* 79 */
	0x00000000, /* 80 */
	0x00000000, /* 81 */
	0x00000000, /* 82 */
	0x00000000, /* 83 */
	0x00000000, /* 84 */
	0x00000000, /* 85 */
	0x00000000, /* 86 */
	0x00000000, /* 87 */
	0x00000000, /* 88 */
	0x00000000, /* 89 */
	0x00000000, /* 90 */
	0x00000000, /* 91 */
	0x00000000, /* 92 */
	0x00000000, /* 93 */
	0x00000000, /* 94 */
	0x00000000, /* 95 */
	0x00000000, /* 96 */
	0x00000000, /* 97 */
	0x00000000, /* 98 */
	0x00000000, /* 99 */
	0x00000000, /* 100 */
	0x00000000, /* 101 */
	0x00000000, /* 102 */
	0x00000000, /* 103 */
	0x00000000, /* 104 */
	0x00000000, /* 105 */
	0x00000000, /* 106 */
	0x00000000, /* 107 */
	0x00000000, /* 108 */
	0x00000000, /* 109 */
	0x00000000, /* 110 */
	0x00000000, /* 111 */
	0x00000000, /* 112 */
	0x00000000, /* 113 */
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
	0x00000000, /* 126 */
	0x00000000, /* 127 */
	0x00000000, /* 128 */
	0x00000000, /* 129 */
	0x00000000, /* 130 */
	0x00000000, /* 131 */
	0x00000000, /* 132 */
	0x00000000, /* 133 */
	0x00000000, /* 134 */
	0x00000000, /* 135 */
	0x00000000, /* 136 */
	0x00000000, /* 137 */
	0x00000000, /* 138 */
	0x00000000, /* 139 */
	0x00000000, /* 140 */
	0x00000000, /* 141 */
	0x00000000, /* 142 */
	0x00000000, /* 143 */
	0x00000000, /* 144 */
	0x00000000, /* 145 */
	0x00000000, /* 146 */
	0x00000000, /* 147 */
	0x00000000, /* 148 */
	0x00000000, /* 149 */
	0x00000000, /* 150 */
	0x00000000, /* 151 */
	0x00000000, /* 152 */
	0x00000000, /* 153 */
	0x00000000, /* 154 */
	0x00000000, /* 155 */
	0x00000000, /* 156 */
	0x00000000, /* 157 */
	0x00000000, /* 158 */
	0x00000000, /* 159 */
	0x00000000, /* 160 */
	0x00000000, /* 161 */
	0x00000000, /* 162 */
	0x00000000, /* 163 */
	0x00000000, /* 164 */
	0x00000000, /* 165 */
	0x00000000, /* 166 */
	0x00000000, /* 167 */
	0x00000000, /* 168 */
	0x00000000, /* 169 */
	0x00000000, /* 170 */
	0x00000000, /* 171 */
	0x00000000, /* 172 */
	0x00000000, /* 173 */
	0x00000000, /* 174 */
	0x00000000, /* 175 */
	0x00000000, /* 176 */
	0x00000000, /* 177 */
	0x00000000, /* 178 */
	0x00000000, /* 179 */
	0x00000000, /* 180 */
	0x00000000, /* 181 */
	0x00000000, /* 182 */
	0x00000000, /* 183 */
	0x00000000, /* 184 */
	0x00000000, /* 185 */
	0x00000000, /* 186 */
	0x00000000, /* 187 */
	0x00000000, /* 188 */
	0x00000000, /* 189 */
	0x00000000, /* 190 */
	0x00000000, /* 191 */
	0x00000000, /* 192 */
	0x00000000, /* 193 */
	0x00000000, /* 194 */
	0x00000000, /* 195 */
	0x00000000, /* 196 */
	0x00000000, /* 197 */
	0x00000000, /* 198 */
	0x00000000, /* 199 */
	0x00000000, /* 200 */
	0x00000000, /* 201 */
	0x00000000, /* 202 */
	0x00000000, /* 203 */
	0x00000000, /* 204 */
	0x00000000, /* 205 */
	0x00000000, /* 206 */
	0x00000000, /* 207 */
	0x00000000, /* 208 */
	0x00000000, /* 209 */
	0x00000000, /* 210 */
	0x00000000, /* 211 */
	0x00000000, /* 212 */
	0x00000000, /* 213 */
	0x00000000, /* 214 */
	0x00000000, /* 215 */
	0x00000000, /* 216 */
	0x00000000, /* 217 */
	0x00000000, /* 218 */
	0x00000000, /* 219 */
	0x00000000, /* 220 */
	0x00000000, /* 221 */
	0x00000000, /* 222 */
	0x00000000, /* 223 */
	0x00000000, /* 224 */
	0x00000000, /* 225 */
	0x00000000, /* 226 */
	0x00000000, /* 227 */
	0x00000000, /* 228 */
	0x00000000, /* 229 */
	0x00000000, /* 230 */
	0x00000000, /* 231 */
	0x00000000, /* 232 */
	0x00000000, /* 233 */
	0x00000000, /* 234 */
	0x00000000, /* 235 */
	0x00000000, /* 236 */
	0x00000000, /* 237 */
	0x00000000, /* 238 */
	0x00000000, /* 239 */
	0x00000000, /* 240 */
	0x00000000, /* 241 */
	0x00000000, /* 242 */
	0x00000000, /* 243 */
	0x00000000, /* 244 */
	0x00000000, /* 245 */
	0x00000000, /* 246 */
	0x00000000, /* 247 */
	0x00000000, /* 248 */
	0x00000000, /* 249 */
	0x00000000, /* 250 */
	0x00000000, /* 251 */
	0x00000000, /* 252 */
	0x008fd000, /* 253 */
	0x00020410, /* 254 */
	0x7b0393bf, /* 255 */
};

/* From vec_720p50hz_bvb_input.bss */
static const uint32_t s_aulRamBVBInput_720p_50hz[] =
{
	0x00a30000, /* 0 */
	0x00207028, /* 1 */
	0x00220028, /* 2 */
	0x002000b4, /* 3 */
	0x00280028, /* 4 */
	0x0028c01f, /* 5 */
	0x0028c4e1, /* 6 */
	0x00200164, /* 7 */
	0x00210016, /* 8 */
	0x00800000, /* 9 */
	0x00000000, /* 10 */
	0x00000000, /* 11 */
	0x00000000, /* 12 */
	0x00000000, /* 13 */
	0x00000000, /* 14 */
	0x00000000, /* 15 */
	0x00000000, /* 16 */
	0x00000000, /* 17 */
	0x00000000, /* 18 */
	0x00000000, /* 19 */
	0x00a80000, /* 20 */
	0x00461001, /* 21 */
	0x002c100b, /* 22 */
	0x00441004, /* 23 */
	0x00442014, /* 24 */
	0x0040c2d0, /* 25 */
	0x00442005, /* 26 */
	0x00814000, /* 27 */
	0x00000000, /* 28 */
	0x00000000, /* 29 */
	0x00000000, /* 30 */
	0x00000000, /* 31 */
	0x00000000, /* 32 */
	0x00000000, /* 33 */
	0x00000000, /* 34 */
	0x00000000, /* 35 */
	0x00000000, /* 36 */
	0x00000000, /* 37 */
	0x00000000, /* 38 */
	0x00000000, /* 39 */
	0x00000000, /* 40 */
	0x00000000, /* 41 */
	0x00000000, /* 42 */
	0x00000000, /* 43 */
	0x00000000, /* 44 */
	0x00000000, /* 45 */
	0x00000000, /* 46 */
	0x00000000, /* 47 */
	0x00000000, /* 48 */
	0x00000000, /* 49 */
	0x00400001, /* 50 */
	0x00202027, /* 51 */
	0x0020301d, /* 52 */
	0x00832000, /* 53 */
	0x00000000, /* 54 */
	0x00000000, /* 55 */
	0x00000000, /* 56 */
	0x00000000, /* 57 */
	0x00000000, /* 58 */
	0x00000000, /* 59 */
	0x00000000, /* 60 */
	0x00000000, /* 61 */
	0x00000000, /* 62 */
	0x00000000, /* 63 */
	0x00000000, /* 64 */
	0x00000000, /* 65 */
	0x00000000, /* 66 */
	0x00000000, /* 67 */
	0x00000000, /* 68 */
	0x00000000, /* 69 */
	0x00000000, /* 70 */
	0x00000000, /* 71 */
	0x00000000, /* 72 */
	0x00000000, /* 73 */
	0x00000000, /* 74 */
	0x00000000, /* 75 */
	0x00000000, /* 76 */
	0x00000000, /* 77 */
	0x00000000, /* 78 */
	0x00000000, /* 79 */
	0x00000000, /* 80 */
	0x00000000, /* 81 */
	0x00000000, /* 82 */
	0x00000000, /* 83 */
	0x00000000, /* 84 */
	0x00000000, /* 85 */
	0x00000000, /* 86 */
	0x00000000, /* 87 */
	0x00000000, /* 88 */
	0x00000000, /* 89 */
	0x00000000, /* 90 */
	0x00000000, /* 91 */
	0x00000000, /* 92 */
	0x00000000, /* 93 */
	0x00000000, /* 94 */
	0x00000000, /* 95 */
	0x00000000, /* 96 */
	0x00000000, /* 97 */
	0x00000000, /* 98 */
	0x00000000, /* 99 */
	0x00000000, /* 100 */
	0x00000000, /* 101 */
	0x00000000, /* 102 */
	0x00000000, /* 103 */
	0x00000000, /* 104 */
	0x00000000, /* 105 */
	0x00000000, /* 106 */
	0x00000000, /* 107 */
	0x00000000, /* 108 */
	0x00000000, /* 109 */
	0x00000000, /* 110 */
	0x00000000, /* 111 */
	0x00000000, /* 112 */
	0x00000000, /* 113 */
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
	0x00000000, /* 126 */
	0x00000000, /* 127 */
	0x00000000, /* 128 */
	0x00000000, /* 129 */
	0x00000000, /* 130 */
	0x00000000, /* 131 */
	0x00000000, /* 132 */
	0x00000000, /* 133 */
	0x00000000, /* 134 */
	0x00000000, /* 135 */
	0x00000000, /* 136 */
	0x00000000, /* 137 */
	0x00000000, /* 138 */
	0x00000000, /* 139 */
	0x00000000, /* 140 */
	0x00000000, /* 141 */
	0x00000000, /* 142 */
	0x00000000, /* 143 */
	0x00000000, /* 144 */
	0x00000000, /* 145 */
	0x00000000, /* 146 */
	0x00000000, /* 147 */
	0x00000000, /* 148 */
	0x00000000, /* 149 */
	0x00000000, /* 150 */
	0x00000000, /* 151 */
	0x00000000, /* 152 */
	0x00000000, /* 153 */
	0x00000000, /* 154 */
	0x00000000, /* 155 */
	0x00000000, /* 156 */
	0x00000000, /* 157 */
	0x00000000, /* 158 */
	0x00000000, /* 159 */
	0x00000000, /* 160 */
	0x00000000, /* 161 */
	0x00000000, /* 162 */
	0x00000000, /* 163 */
	0x00000000, /* 164 */
	0x00000000, /* 165 */
	0x00000000, /* 166 */
	0x00000000, /* 167 */
	0x00000000, /* 168 */
	0x00000000, /* 169 */
	0x00000000, /* 170 */
	0x00000000, /* 171 */
	0x00000000, /* 172 */
	0x00000000, /* 173 */
	0x00000000, /* 174 */
	0x00000000, /* 175 */
	0x00000000, /* 176 */
	0x00000000, /* 177 */
	0x00000000, /* 178 */
	0x00000000, /* 179 */
	0x00000000, /* 180 */
	0x00000000, /* 181 */
	0x00000000, /* 182 */
	0x00000000, /* 183 */
	0x00000000, /* 184 */
	0x00000000, /* 185 */
	0x00000000, /* 186 */
	0x00000000, /* 187 */
	0x00000000, /* 188 */
	0x00000000, /* 189 */
	0x00000000, /* 190 */
	0x00000000, /* 191 */
	0x00000000, /* 192 */
	0x00000000, /* 193 */
	0x00000000, /* 194 */
	0x00000000, /* 195 */
	0x00000000, /* 196 */
	0x00000000, /* 197 */
	0x00000000, /* 198 */
	0x00000000, /* 199 */
	0x00000000, /* 200 */
	0x00000000, /* 201 */
	0x00000000, /* 202 */
	0x00000000, /* 203 */
	0x00000000, /* 204 */
	0x00000000, /* 205 */
	0x00000000, /* 206 */
	0x00000000, /* 207 */
	0x00000000, /* 208 */
	0x00000000, /* 209 */
	0x00000000, /* 210 */
	0x00000000, /* 211 */
	0x00000000, /* 212 */
	0x00000000, /* 213 */
	0x00000000, /* 214 */
	0x00000000, /* 215 */
	0x00000000, /* 216 */
	0x00000000, /* 217 */
	0x00000000, /* 218 */
	0x00000000, /* 219 */
	0x00000000, /* 220 */
	0x00000000, /* 221 */
	0x00000000, /* 222 */
	0x00000000, /* 223 */
	0x00000000, /* 224 */
	0x00000000, /* 225 */
	0x00000000, /* 226 */
	0x00000000, /* 227 */
	0x00000000, /* 228 */
	0x00000000, /* 229 */
	0x00000000, /* 230 */
	0x00000000, /* 231 */
	0x00000000, /* 232 */
	0x00000000, /* 233 */
	0x00000000, /* 234 */
	0x00000000, /* 235 */
	0x00000000, /* 236 */
	0x00000000, /* 237 */
	0x00000000, /* 238 */
	0x00000000, /* 239 */
	0x00000000, /* 240 */
	0x00000000, /* 241 */
	0x00000000, /* 242 */
	0x00000000, /* 243 */
	0x00000000, /* 244 */
	0x00000000, /* 245 */
	0x00000000, /* 246 */
	0x00000000, /* 247 */
	0x00000000, /* 248 */
	0x00000000, /* 249 */
	0x00000000, /* 250 */
	0x00000000, /* 251 */
	0x00000000, /* 252 */
	0x008fd000, /* 253 */
	0x00020410, /* 254 */
	0xf1144893, /* 255 */
};

/* From vec_720p50hz_bvb_input.bss */
static const uint32_t s_aulRamBVBInput_720p_50hz_3D[] =
{
	0x00a24000, /* 0 */
	0x002401b8, /* 1 */
	0x00201028, /* 2 */
	0x002000dc, /* 3 */
	0x002084ea, /* 4 */
	0x00800000, /* 5 */
	0x00000000, /* 6 */
	0x00000000, /* 7 */
	0x00000000, /* 8 */
	0x00000000, /* 9 */
	0x00000000, /* 10 */
	0x00000000, /* 11 */
	0x00000000, /* 12 */
	0x00000000, /* 13 */
	0x00000000, /* 14 */
	0x00000000, /* 15 */
	0x00000000, /* 16 */
	0x00000000, /* 17 */
	0x00000000, /* 18 */
	0x00000000, /* 19 */
	0x00a80000, /* 20 */
	0x00421001, /* 21 */
	0x00401004, /* 22 */
	0x00400014, /* 23 */
	0x004085be, /* 24 */
	0x00400005, /* 25 */
	0x00a80000, /* 26 */
	0x00815000, /* 27 */
	0x00000000, /* 28 */
	0x00000000, /* 29 */
	0x00000000, /* 30 */
	0x00000000, /* 31 */
	0x00000000, /* 32 */
	0x00000000, /* 33 */
	0x00000000, /* 34 */
	0x00000000, /* 35 */
	0x00000000, /* 36 */
	0x00000000, /* 37 */
	0x00000000, /* 38 */
	0x00000000, /* 39 */
	0x00000000, /* 40 */
	0x00000000, /* 41 */
	0x00000000, /* 42 */
	0x00000000, /* 43 */
	0x00000000, /* 44 */
	0x00000000, /* 45 */
	0x00000000, /* 46 */
	0x00000000, /* 47 */
	0x00000000, /* 48 */
	0x00000000, /* 49 */
	0x00000000, /* 50 */
	0x00000000, /* 51 */
	0x00000000, /* 52 */
	0x00000000, /* 53 */
	0x00000000, /* 54 */
	0x00000000, /* 55 */
	0x00000000, /* 56 */
	0x00000000, /* 57 */
	0x00000000, /* 58 */
	0x00000000, /* 59 */
	0x00000000, /* 60 */
	0x00000000, /* 61 */
	0x00000000, /* 62 */
	0x00000000, /* 63 */
	0x00000000, /* 64 */
	0x00000000, /* 65 */
	0x00000000, /* 66 */
	0x00000000, /* 67 */
	0x00000000, /* 68 */
	0x00000000, /* 69 */
	0x00000000, /* 70 */
	0x00000000, /* 71 */
	0x00000000, /* 72 */
	0x00000000, /* 73 */
	0x00000000, /* 74 */
	0x00000000, /* 75 */
	0x00000000, /* 76 */
	0x00000000, /* 77 */
	0x00000000, /* 78 */
	0x00000000, /* 79 */
	0x00000000, /* 80 */
	0x00000000, /* 81 */
	0x00000000, /* 82 */
	0x00000000, /* 83 */
	0x00000000, /* 84 */
	0x00000000, /* 85 */
	0x00000000, /* 86 */
	0x00000000, /* 87 */
	0x00000000, /* 88 */
	0x00000000, /* 89 */
	0x00000000, /* 90 */
	0x00000000, /* 91 */
	0x00000000, /* 92 */
	0x00000000, /* 93 */
	0x00000000, /* 94 */
	0x00000000, /* 95 */
	0x00000000, /* 96 */
	0x00000000, /* 97 */
	0x00000000, /* 98 */
	0x00000000, /* 99 */
	0x00000000, /* 100 */
	0x00000000, /* 101 */
	0x00000000, /* 102 */
	0x00000000, /* 103 */
	0x00000000, /* 104 */
	0x00000000, /* 105 */
	0x00000000, /* 106 */
	0x00000000, /* 107 */
	0x00000000, /* 108 */
	0x00000000, /* 109 */
	0x00000000, /* 110 */
	0x00000000, /* 111 */
	0x00000000, /* 112 */
	0x00000000, /* 113 */
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
	0x00000000, /* 126 */
	0x00000000, /* 127 */
	0x00000000, /* 128 */
	0x00000000, /* 129 */
	0x00000000, /* 130 */
	0x00000000, /* 131 */
	0x00000000, /* 132 */
	0x00000000, /* 133 */
	0x00000000, /* 134 */
	0x00000000, /* 135 */
	0x00000000, /* 136 */
	0x00000000, /* 137 */
	0x00000000, /* 138 */
	0x00000000, /* 139 */
	0x00000000, /* 140 */
	0x00000000, /* 141 */
	0x00000000, /* 142 */
	0x00000000, /* 143 */
	0x00000000, /* 144 */
	0x00000000, /* 145 */
	0x00000000, /* 146 */
	0x00000000, /* 147 */
	0x00000000, /* 148 */
	0x00000000, /* 149 */
	0x00000000, /* 150 */
	0x00000000, /* 151 */
	0x00000000, /* 152 */
	0x00000000, /* 153 */
	0x00000000, /* 154 */
	0x00000000, /* 155 */
	0x00000000, /* 156 */
	0x00000000, /* 157 */
	0x00000000, /* 158 */
	0x00000000, /* 159 */
	0x00000000, /* 160 */
	0x00000000, /* 161 */
	0x00000000, /* 162 */
	0x00000000, /* 163 */
	0x00000000, /* 164 */
	0x00000000, /* 165 */
	0x00000000, /* 166 */
	0x00000000, /* 167 */
	0x00000000, /* 168 */
	0x00000000, /* 169 */
	0x00000000, /* 170 */
	0x00000000, /* 171 */
	0x00000000, /* 172 */
	0x00000000, /* 173 */
	0x00000000, /* 174 */
	0x00000000, /* 175 */
	0x00000000, /* 176 */
	0x00000000, /* 177 */
	0x00000000, /* 178 */
	0x00000000, /* 179 */
	0x00000000, /* 180 */
	0x00000000, /* 181 */
	0x00000000, /* 182 */
	0x00000000, /* 183 */
	0x00000000, /* 184 */
	0x00000000, /* 185 */
	0x00000000, /* 186 */
	0x00000000, /* 187 */
	0x00000000, /* 188 */
	0x00000000, /* 189 */
	0x00000000, /* 190 */
	0x00000000, /* 191 */
	0x00000000, /* 192 */
	0x00000000, /* 193 */
	0x00000000, /* 194 */
	0x00000000, /* 195 */
	0x00000000, /* 196 */
	0x00000000, /* 197 */
	0x00000000, /* 198 */
	0x00000000, /* 199 */
	0x00000000, /* 200 */
	0x00000000, /* 201 */
	0x00000000, /* 202 */
	0x00000000, /* 203 */
	0x00000000, /* 204 */
	0x00000000, /* 205 */
	0x00000000, /* 206 */
	0x00000000, /* 207 */
	0x00000000, /* 208 */
	0x00000000, /* 209 */
	0x00000000, /* 210 */
	0x00000000, /* 211 */
	0x00000000, /* 212 */
	0x00000000, /* 213 */
	0x00000000, /* 214 */
	0x00000000, /* 215 */
	0x00000000, /* 216 */
	0x00000000, /* 217 */
	0x00000000, /* 218 */
	0x00000000, /* 219 */
	0x00000000, /* 220 */
	0x00000000, /* 221 */
	0x00000000, /* 222 */
	0x00000000, /* 223 */
	0x00000000, /* 224 */
	0x00000000, /* 225 */
	0x00000000, /* 226 */
	0x00000000, /* 227 */
	0x00000000, /* 228 */
	0x00000000, /* 229 */
	0x00000000, /* 230 */
	0x00000000, /* 231 */
	0x00000000, /* 232 */
	0x00000000, /* 233 */
	0x00000000, /* 234 */
	0x00000000, /* 235 */
	0x00000000, /* 236 */
	0x00000000, /* 237 */
	0x00000000, /* 238 */
	0x00000000, /* 239 */
	0x00000000, /* 240 */
	0x00000000, /* 241 */
	0x00000000, /* 242 */
	0x00000000, /* 243 */
	0x00000000, /* 244 */
	0x00000000, /* 245 */
	0x00000000, /* 246 */
	0x00000000, /* 247 */
	0x00000000, /* 248 */
	0x00000000, /* 249 */
	0x00000000, /* 250 */
	0x00000000, /* 251 */
	0x00000000, /* 252 */
	0x008fd000, /* 253 */
	0x00122710, /* 254 */
	0x6546d8a9, /* 255 */
};

/* From vec_480p_macrovision_type_1.bss */
static const uint32_t s_aulRamBVBInput_480p[BVDC_P_RAM_TABLE_SIZE] =
{
	0x00a04000, /* 0 */
	0x00240010, /* 1 */
	0x0020703f, /* 2 */
	0x00204030, /* 3 */
	0x00a20000, /* 4 */
	0x0028c2a1, /* 5 */
	0x00288019, /* 6 */
	0x00a04000, /* 7 */
	0x00801000, /* 8 */
	0x00000000, /* 9 */
	0x00000000, /* 10 */
	0x00000000, /* 11 */
	0x00000000, /* 12 */
	0x00000000, /* 13 */
	0x00000000, /* 14 */
	0x00000000, /* 15 */
	0x00000000, /* 16 */
	0x00000000, /* 17 */
	0x00000000, /* 18 */
	0x00000000, /* 19 */
	0x0022c016, /* 20 */
	0x00a88000, /* 21 */
	0x0022c00d, /* 22 */
	0x004e2001, /* 23 */
	0x00242018, /* 24 */
	0x00442005, /* 25 */
	0x0024000f, /* 26 */
	0x00441006, /* 27 */
	0x0024100f, /* 28 */
	0x0044201e, /* 29 */
	0x0040c1e3, /* 30 */
	0x0022c00b, /* 31 */
	0x00815000, /* 32 */
	0x00000000, /* 33 */
	0x00000000, /* 34 */
	0x00000000, /* 35 */
	0x00000000, /* 36 */
	0x00000000, /* 37 */
	0x00000000, /* 38 */
	0x00000000, /* 39 */
	0x00400801, /* 40 */
	0x00200039, /* 41 */
	0x0020101e, /* 42 */
	0x00208028, /* 43 */
	0x00200012, /* 44 */
	0x0020101e, /* 45 */
	0x00208028, /* 46 */
	0x00200012, /* 47 */
	0x0020101e, /* 48 */
	0x00208028, /* 49 */
	0x00200012, /* 50 */
	0x0020101e, /* 51 */
	0x00208028, /* 52 */
	0x00200012, /* 53 */
	0x00828000, /* 54 */
	0x00000000, /* 55 */
	0x00000000, /* 56 */
	0x00000000, /* 57 */
	0x00000000, /* 58 */
	0x00000000, /* 59 */
	0x00000000, /* 60 */
	0x00000000, /* 61 */
	0x00000000, /* 62 */
	0x00000000, /* 63 */
	0x0040000c, /* 64 */
	0x00644008, /* 65 */
	0x004001f8, /* 66 */
	0x00840000, /* 67 */
	0x0020900b, /* 68 */
	0x00509001, /* 69 */
	0x00000000, /* 70 */
	0x00000000, /* 71 */
	0x00000000, /* 72 */
	0x00000000, /* 73 */
	0x00000000, /* 74 */
	0x00000000, /* 75 */
	0x00000000, /* 76 */
	0x00000000, /* 77 */
	0x00000000, /* 78 */
	0x00000000, /* 79 */
	0x00000000, /* 80 */
	0x00000000, /* 81 */
	0x00000000, /* 82 */
	0x00000000, /* 83 */
	0x00000000, /* 84 */
	0x00000000, /* 85 */
	0x00000000, /* 86 */
	0x00000000, /* 87 */
	0x00000000, /* 88 */
	0x00000000, /* 89 */
	0x00000000, /* 90 */
	0x00000000, /* 91 */
	0x00000000, /* 92 */
	0x00000000, /* 93 */
	0x00000000, /* 94 */
	0x00000000, /* 95 */
	0x00000000, /* 96 */
	0x00000000, /* 97 */
	0x00000000, /* 98 */
	0x00000000, /* 99 */
	0x00000000, /* 100 */
	0x00000000, /* 101 */
	0x00000000, /* 102 */
	0x00000000, /* 103 */
	0x00000000, /* 104 */
	0x00000000, /* 105 */
	0x00000000, /* 106 */
	0x00000000, /* 107 */
	0x00000000, /* 108 */
	0x00000000, /* 109 */
	0x00000000, /* 110 */
	0x00000000, /* 111 */
	0x00000000, /* 112 */
	0x00000000, /* 113 */
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
	0x00000000, /* 126 */
	0x00000000, /* 127 */
	0x00000000, /* 128 */
	0x00000000, /* 129 */
	0x00000000, /* 130 */
	0x00000000, /* 131 */
	0x00000000, /* 132 */
	0x00000000, /* 133 */
	0x00000000, /* 134 */
	0x00000000, /* 135 */
	0x00000000, /* 136 */
	0x00000000, /* 137 */
	0x00000000, /* 138 */
	0x00000000, /* 139 */
	0x00000000, /* 140 */
	0x00000000, /* 141 */
	0x00000000, /* 142 */
	0x00000000, /* 143 */
	0x00000000, /* 144 */
	0x00000000, /* 145 */
	0x00000000, /* 146 */
	0x00000000, /* 147 */
	0x00000000, /* 148 */
	0x00000000, /* 149 */
	0x00000000, /* 150 */
	0x00000000, /* 151 */
	0x00000000, /* 152 */
	0x00000000, /* 153 */
	0x00000000, /* 154 */
	0x00000000, /* 155 */
	0x00000000, /* 156 */
	0x00000000, /* 157 */
	0x00000000, /* 158 */
	0x00000000, /* 159 */
	0x00000000, /* 160 */
	0x00000000, /* 161 */
	0x00000000, /* 162 */
	0x00000000, /* 163 */
	0x00000000, /* 164 */
	0x00000000, /* 165 */
	0x00000000, /* 166 */
	0x00000000, /* 167 */
	0x00000000, /* 168 */
	0x00000000, /* 169 */
	0x00000000, /* 170 */
	0x00000000, /* 171 */
	0x00000000, /* 172 */
	0x00000000, /* 173 */
	0x00000000, /* 174 */
	0x00000000, /* 175 */
	0x00000000, /* 176 */
	0x00000000, /* 177 */
	0x00000000, /* 178 */
	0x00000000, /* 179 */
	0x00000000, /* 180 */
	0x00000000, /* 181 */
	0x00000000, /* 182 */
	0x00000000, /* 183 */
	0x00000000, /* 184 */
	0x00000000, /* 185 */
	0x00000000, /* 186 */
	0x00000000, /* 187 */
	0x00000000, /* 188 */
	0x00000000, /* 189 */
	0x00000000, /* 190 */
	0x00000000, /* 191 */
	0x00000000, /* 192 */
	0x00000000, /* 193 */
	0x00000000, /* 194 */
	0x00000000, /* 195 */
	0x00000000, /* 196 */
	0x00000000, /* 197 */
	0x00000000, /* 198 */
	0x00000000, /* 199 */
	0x00000000, /* 200 */
	0x00000000, /* 201 */
	0x00000000, /* 202 */
	0x00000000, /* 203 */
	0x00000000, /* 204 */
	0x00000000, /* 205 */
	0x00000000, /* 206 */
	0x00000000, /* 207 */
	0x00000000, /* 208 */
	0x00000000, /* 209 */
	0x00000000, /* 210 */
	0x00000000, /* 211 */
	0x00000000, /* 212 */
	0x00000000, /* 213 */
	0x00000000, /* 214 */
	0x00000000, /* 215 */
	0x00000000, /* 216 */
	0x00000000, /* 217 */
	0x00000000, /* 218 */
	0x00000000, /* 219 */
	0x00000000, /* 220 */
	0x00000000, /* 221 */
	0x00000000, /* 222 */
	0x00000000, /* 223 */
	0x00000000, /* 224 */
	0x00000000, /* 225 */
	0x00000000, /* 226 */
	0x00000000, /* 227 */
	0x00000000, /* 228 */
	0x00000000, /* 229 */
	0x00000000, /* 230 */
	0x00000000, /* 231 */
	0x00000000, /* 232 */
	0x00000000, /* 233 */
	0x00000000, /* 234 */
	0x00000000, /* 235 */
	0x00000000, /* 236 */
	0x00000000, /* 237 */
	0x00000000, /* 238 */
	0x00000000, /* 239 */
	0x00000000, /* 240 */
	0x00000000, /* 241 */
	0x00000000, /* 242 */
	0x00000000, /* 243 */
	0x00000000, /* 244 */
	0x00000000, /* 245 */
	0x00000000, /* 246 */
	0x00000000, /* 247 */
	0x00000000, /* 248 */
	0x00000000, /* 249 */
	0x00000000, /* 250 */
	0x00000000, /* 251 */
	0x00000000, /* 252 */
	0x008fd000, /* 253 */
	0x00062512, /* 254 */
	0xb5b8601c, /* 255 */
};

/* From vec_480p_rm54_macrovision_type_1.bss */
static const uint32_t s_aulRamBVBInput_480p_54MHz[BVDC_P_RAM_TABLE_SIZE] =
{
	0x00a04000, /* 0 */
	0x00240020, /* 1 */
	0x0020707e, /* 2 */
	0x0020406b, /* 3 */
	0x00a20000, /* 4 */
	0x0028c542, /* 5 */
	0x00288048, /* 6 */
	0x00a04000, /* 7 */
	0x00801000, /* 8 */
	0x00000000, /* 9 */
	0x00000000, /* 10 */
	0x00000000, /* 11 */
	0x00000000, /* 12 */
	0x00000000, /* 13 */
	0x00000000, /* 14 */
	0x00000000, /* 15 */
	0x00000000, /* 16 */
	0x00000000, /* 17 */
	0x00000000, /* 18 */
	0x00000000, /* 19 */
	0x0022c016, /* 20 */
	0x00a88000, /* 21 */
	0x0022c00d, /* 22 */
	0x004e2001, /* 23 */
	0x00242018, /* 24 */
	0x00442005, /* 25 */
	0x0024001f, /* 26 */
	0x00441006, /* 27 */
	0x0024101f, /* 28 */
	0x0044201e, /* 29 */
	0x0040c1e3, /* 30 */
	0x0022c00b, /* 31 */
	0x00815000, /* 32 */
	0x00000000, /* 33 */
	0x00000000, /* 34 */
	0x00000000, /* 35 */
	0x00000000, /* 36 */
	0x00000000, /* 37 */
	0x00000000, /* 38 */
	0x00000000, /* 39 */
	0x00400801, /* 40 */
	0x00200072, /* 41 */
	0x0020103c, /* 42 */
	0x00208050, /* 43 */
	0x00200024, /* 44 */
	0x0020103c, /* 45 */
	0x00208050, /* 46 */
	0x00200024, /* 47 */
	0x0020103c, /* 48 */
	0x00208050, /* 49 */
	0x00200024, /* 50 */
	0x0020103c, /* 51 */
	0x00208050, /* 52 */
	0x00200024, /* 53 */
	0x00828000, /* 54 */
	0x00000000, /* 55 */
	0x00000000, /* 56 */
	0x00000000, /* 57 */
	0x00000000, /* 58 */
	0x00000000, /* 59 */
	0x00000000, /* 60 */
	0x00000000, /* 61 */
	0x00000000, /* 62 */
	0x00000000, /* 63 */
	0x0040000c, /* 64 */
	0x00644008, /* 65 */
	0x004001f8, /* 66 */
	0x00840000, /* 67 */
	0x0020900b, /* 68 */
	0x00509001, /* 69 */
	0x00000000, /* 70 */
	0x00000000, /* 71 */
	0x00000000, /* 72 */
	0x00000000, /* 73 */
	0x00000000, /* 74 */
	0x00000000, /* 75 */
	0x00000000, /* 76 */
	0x00000000, /* 77 */
	0x00000000, /* 78 */
	0x00000000, /* 79 */
	0x00000000, /* 80 */
	0x00000000, /* 81 */
	0x00000000, /* 82 */
	0x00000000, /* 83 */
	0x00000000, /* 84 */
	0x00000000, /* 85 */
	0x00000000, /* 86 */
	0x00000000, /* 87 */
	0x00000000, /* 88 */
	0x00000000, /* 89 */
	0x00000000, /* 90 */
	0x00000000, /* 91 */
	0x00000000, /* 92 */
	0x00000000, /* 93 */
	0x00000000, /* 94 */
	0x00000000, /* 95 */
	0x00000000, /* 96 */
	0x00000000, /* 97 */
	0x00000000, /* 98 */
	0x00000000, /* 99 */
	0x00000000, /* 100 */
	0x00000000, /* 101 */
	0x00000000, /* 102 */
	0x00000000, /* 103 */
	0x00000000, /* 104 */
	0x00000000, /* 105 */
	0x00000000, /* 106 */
	0x00000000, /* 107 */
	0x00000000, /* 108 */
	0x00000000, /* 109 */
	0x00000000, /* 110 */
	0x00000000, /* 111 */
	0x00000000, /* 112 */
	0x00000000, /* 113 */
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
	0x00000000, /* 126 */
	0x00000000, /* 127 */
	0x00000000, /* 128 */
	0x00000000, /* 129 */
	0x00000000, /* 130 */
	0x00000000, /* 131 */
	0x00000000, /* 132 */
	0x00000000, /* 133 */
	0x00000000, /* 134 */
	0x00000000, /* 135 */
	0x00000000, /* 136 */
	0x00000000, /* 137 */
	0x00000000, /* 138 */
	0x00000000, /* 139 */
	0x00000000, /* 140 */
	0x00000000, /* 141 */
	0x00000000, /* 142 */
	0x00000000, /* 143 */
	0x00000000, /* 144 */
	0x00000000, /* 145 */
	0x00000000, /* 146 */
	0x00000000, /* 147 */
	0x00000000, /* 148 */
	0x00000000, /* 149 */
	0x00000000, /* 150 */
	0x00000000, /* 151 */
	0x00000000, /* 152 */
	0x00000000, /* 153 */
	0x00000000, /* 154 */
	0x00000000, /* 155 */
	0x00000000, /* 156 */
	0x00000000, /* 157 */
	0x00000000, /* 158 */
	0x00000000, /* 159 */
	0x00000000, /* 160 */
	0x00000000, /* 161 */
	0x00000000, /* 162 */
	0x00000000, /* 163 */
	0x00000000, /* 164 */
	0x00000000, /* 165 */
	0x00000000, /* 166 */
	0x00000000, /* 167 */
	0x00000000, /* 168 */
	0x00000000, /* 169 */
	0x00000000, /* 170 */
	0x00000000, /* 171 */
	0x00000000, /* 172 */
	0x00000000, /* 173 */
	0x00000000, /* 174 */
	0x00000000, /* 175 */
	0x00000000, /* 176 */
	0x00000000, /* 177 */
	0x00000000, /* 178 */
	0x00000000, /* 179 */
	0x00000000, /* 180 */
	0x00000000, /* 181 */
	0x00000000, /* 182 */
	0x00000000, /* 183 */
	0x00000000, /* 184 */
	0x00000000, /* 185 */
	0x00000000, /* 186 */
	0x00000000, /* 187 */
	0x00000000, /* 188 */
	0x00000000, /* 189 */
	0x00000000, /* 190 */
	0x00000000, /* 191 */
	0x00000000, /* 192 */
	0x00000000, /* 193 */
	0x00000000, /* 194 */
	0x00000000, /* 195 */
	0x00000000, /* 196 */
	0x00000000, /* 197 */
	0x00000000, /* 198 */
	0x00000000, /* 199 */
	0x00000000, /* 200 */
	0x00000000, /* 201 */
	0x00000000, /* 202 */
	0x00000000, /* 203 */
	0x00000000, /* 204 */
	0x00000000, /* 205 */
	0x00000000, /* 206 */
	0x00000000, /* 207 */
	0x00000000, /* 208 */
	0x00000000, /* 209 */
	0x00000000, /* 210 */
	0x00000000, /* 211 */
	0x00000000, /* 212 */
	0x00000000, /* 213 */
	0x00000000, /* 214 */
	0x00000000, /* 215 */
	0x00000000, /* 216 */
	0x00000000, /* 217 */
	0x00000000, /* 218 */
	0x00000000, /* 219 */
	0x00000000, /* 220 */
	0x00000000, /* 221 */
	0x00000000, /* 222 */
	0x00000000, /* 223 */
	0x00000000, /* 224 */
	0x00000000, /* 225 */
	0x00000000, /* 226 */
	0x00000000, /* 227 */
	0x00000000, /* 228 */
	0x00000000, /* 229 */
	0x00000000, /* 230 */
	0x00000000, /* 231 */
	0x00000000, /* 232 */
	0x00000000, /* 233 */
	0x00000000, /* 234 */
	0x00000000, /* 235 */
	0x00000000, /* 236 */
	0x00000000, /* 237 */
	0x00000000, /* 238 */
	0x00000000, /* 239 */
	0x00000000, /* 240 */
	0x00000000, /* 241 */
	0x00000000, /* 242 */
	0x00000000, /* 243 */
	0x00000000, /* 244 */
	0x00000000, /* 245 */
	0x00000000, /* 246 */
	0x00000000, /* 247 */
	0x00000000, /* 248 */
	0x00000000, /* 249 */
	0x00000000, /* 250 */
	0x00000000, /* 251 */
	0x00000000, /* 252 */
	0x008fd000, /* 253 */
	0x00020310, /* 254 */
	0x48b8bff4, /* 255 */
};

/* From vec_480pARIB_bvb_input.bss */
static const uint32_t s_aulRamBVBInput_480pARIB[BVDC_P_RAM_TABLE_SIZE] =
{
	0x00a04000, /* 0 */
	0x0024000f, /* 1 */
	0x00207040, /* 2 */
	0x00204030, /* 3 */
	0x00a20000, /* 4 */
	0x0028c2a1, /* 5 */
	0x00288019, /* 6 */
	0x00a04000, /* 7 */
	0x00801000, /* 8 */
	0x00000000, /* 9 */
	0x00000000, /* 10 */
	0x00000000, /* 11 */
	0x00000000, /* 12 */
	0x00000000, /* 13 */
	0x00000000, /* 14 */
	0x00000000, /* 15 */
	0x00000000, /* 16 */
	0x00000000, /* 17 */
	0x00000000, /* 18 */
	0x00000000, /* 19 */
	0x0020c016, /* 20 */
	0x0020c00d, /* 21 */
	0x00442001, /* 22 */
	0x00222021, /* 23 */
	0x00a88000, /* 24 */
	0x00462001, /* 25 */
	0x00242018, /* 26 */
	0x00442005, /* 27 */
	0x0024000e, /* 28 */
	0x00441006, /* 29 */
	0x0024100e, /* 30 */
	0x00442020, /* 31 */
	0x0040c1e0, /* 32 */
	0x0020c00b, /* 33 */
	0x00815000, /* 34 */
	0x00000000, /* 35 */
	0x00000000, /* 36 */
	0x00000000, /* 37 */
	0x00000000, /* 38 */
	0x00000000, /* 39 */
	0x00400801, /* 40 */
	0x00200038, /* 41 */
	0x0020101e, /* 42 */
	0x00208028, /* 43 */
	0x00200012, /* 44 */
	0x0020101e, /* 45 */
	0x00208028, /* 46 */
	0x00200012, /* 47 */
	0x0020101e, /* 48 */
	0x00208028, /* 49 */
	0x00200012, /* 50 */
	0x0020101e, /* 51 */
	0x00208028, /* 52 */
	0x00200012, /* 53 */
	0x00828000, /* 54 */
	0x00000000, /* 55 */
	0x00000000, /* 56 */
	0x00000000, /* 57 */
	0x00000000, /* 58 */
	0x00000000, /* 59 */
	0x00000000, /* 60 */
	0x00000000, /* 61 */
	0x00000000, /* 62 */
	0x00000000, /* 63 */
	0x0040000c, /* 64 */
	0x00644008, /* 65 */
	0x004001f8, /* 66 */
	0x00840000, /* 67 */
	0x0020900b, /* 68 */
	0x00509001, /* 69 */
	0x00000000, /* 70 */
	0x00000000, /* 71 */
	0x00000000, /* 72 */
	0x00000000, /* 73 */
	0x00000000, /* 74 */
	0x00000000, /* 75 */
	0x00000000, /* 76 */
	0x00000000, /* 77 */
	0x00000000, /* 78 */
	0x00000000, /* 79 */
	0x00000000, /* 80 */
	0x00000000, /* 81 */
	0x00000000, /* 82 */
	0x00000000, /* 83 */
	0x00000000, /* 84 */
	0x00000000, /* 85 */
	0x00000000, /* 86 */
	0x00000000, /* 87 */
	0x00000000, /* 88 */
	0x00000000, /* 89 */
	0x00000000, /* 90 */
	0x00000000, /* 91 */
	0x00000000, /* 92 */
	0x00000000, /* 93 */
	0x00000000, /* 94 */
	0x00000000, /* 95 */
	0x00000000, /* 96 */
	0x00000000, /* 97 */
	0x00000000, /* 98 */
	0x00000000, /* 99 */
	0x00000000, /* 100 */
	0x00000000, /* 101 */
	0x00000000, /* 102 */
	0x00000000, /* 103 */
	0x00000000, /* 104 */
	0x00000000, /* 105 */
	0x00000000, /* 106 */
	0x00000000, /* 107 */
	0x00000000, /* 108 */
	0x00000000, /* 109 */
	0x00000000, /* 110 */
	0x00000000, /* 111 */
	0x00000000, /* 112 */
	0x00000000, /* 113 */
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
	0x00000000, /* 126 */
	0x00000000, /* 127 */
	0x00000000, /* 128 */
	0x00000000, /* 129 */
	0x00000000, /* 130 */
	0x00000000, /* 131 */
	0x00000000, /* 132 */
	0x00000000, /* 133 */
	0x00000000, /* 134 */
	0x00000000, /* 135 */
	0x00000000, /* 136 */
	0x00000000, /* 137 */
	0x00000000, /* 138 */
	0x00000000, /* 139 */
	0x00000000, /* 140 */
	0x00000000, /* 141 */
	0x00000000, /* 142 */
	0x00000000, /* 143 */
	0x00000000, /* 144 */
	0x00000000, /* 145 */
	0x00000000, /* 146 */
	0x00000000, /* 147 */
	0x00000000, /* 148 */
	0x00000000, /* 149 */
	0x00000000, /* 150 */
	0x00000000, /* 151 */
	0x00000000, /* 152 */
	0x00000000, /* 153 */
	0x00000000, /* 154 */
	0x00000000, /* 155 */
	0x00000000, /* 156 */
	0x00000000, /* 157 */
	0x00000000, /* 158 */
	0x00000000, /* 159 */
	0x00000000, /* 160 */
	0x00000000, /* 161 */
	0x00000000, /* 162 */
	0x00000000, /* 163 */
	0x00000000, /* 164 */
	0x00000000, /* 165 */
	0x00000000, /* 166 */
	0x00000000, /* 167 */
	0x00000000, /* 168 */
	0x00000000, /* 169 */
	0x00000000, /* 170 */
	0x00000000, /* 171 */
	0x00000000, /* 172 */
	0x00000000, /* 173 */
	0x00000000, /* 174 */
	0x00000000, /* 175 */
	0x00000000, /* 176 */
	0x00000000, /* 177 */
	0x00000000, /* 178 */
	0x00000000, /* 179 */
	0x00000000, /* 180 */
	0x00000000, /* 181 */
	0x00000000, /* 182 */
	0x00000000, /* 183 */
	0x00000000, /* 184 */
	0x00000000, /* 185 */
	0x00000000, /* 186 */
	0x00000000, /* 187 */
	0x00000000, /* 188 */
	0x00000000, /* 189 */
	0x00000000, /* 190 */
	0x00000000, /* 191 */
	0x00000000, /* 192 */
	0x00000000, /* 193 */
	0x00000000, /* 194 */
	0x00000000, /* 195 */
	0x00000000, /* 196 */
	0x00000000, /* 197 */
	0x00000000, /* 198 */
	0x00000000, /* 199 */
	0x00000000, /* 200 */
	0x00000000, /* 201 */
	0x00000000, /* 202 */
	0x00000000, /* 203 */
	0x00000000, /* 204 */
	0x00000000, /* 205 */
	0x00000000, /* 206 */
	0x00000000, /* 207 */
	0x00000000, /* 208 */
	0x00000000, /* 209 */
	0x00000000, /* 210 */
	0x00000000, /* 211 */
	0x00000000, /* 212 */
	0x00000000, /* 213 */
	0x00000000, /* 214 */
	0x00000000, /* 215 */
	0x00000000, /* 216 */
	0x00000000, /* 217 */
	0x00000000, /* 218 */
	0x00000000, /* 219 */
	0x00000000, /* 220 */
	0x00000000, /* 221 */
	0x00000000, /* 222 */
	0x00000000, /* 223 */
	0x00000000, /* 224 */
	0x00000000, /* 225 */
	0x00000000, /* 226 */
	0x00000000, /* 227 */
	0x00000000, /* 228 */
	0x00000000, /* 229 */
	0x00000000, /* 230 */
	0x00000000, /* 231 */
	0x00000000, /* 232 */
	0x00000000, /* 233 */
	0x00000000, /* 234 */
	0x00000000, /* 235 */
	0x00000000, /* 236 */
	0x00000000, /* 237 */
	0x00000000, /* 238 */
	0x00000000, /* 239 */
	0x00000000, /* 240 */
	0x00000000, /* 241 */
	0x00000000, /* 242 */
	0x00000000, /* 243 */
	0x00000000, /* 244 */
	0x00000000, /* 245 */
	0x00000000, /* 246 */
	0x00000000, /* 247 */
	0x00000000, /* 248 */
	0x00000000, /* 249 */
	0x00000000, /* 250 */
	0x00000000, /* 251 */
	0x00000000, /* 252 */
	0x008fd000, /* 253 */
	0x00101409, /* 254 */
	0x260a5638, /* 255 */
};

/* From vec_480pARIB_rm54_macrovision_type_1.bss */
static const uint32_t s_aulRamBVBInput_480pARIB_54MHz[BVDC_P_RAM_TABLE_SIZE] =
{
	0x00a04000, /* 0 */
	0x00240020, /* 1 */
	0x0020707e, /* 2 */
	0x0020406b, /* 3 */
	0x00a20000, /* 4 */
	0x0028c542, /* 5 */
	0x00288048, /* 6 */
	0x00a04000, /* 7 */
	0x00801000, /* 8 */
	0x00000000, /* 9 */
	0x00000000, /* 10 */
	0x00000000, /* 11 */
	0x00000000, /* 12 */
	0x00000000, /* 13 */
	0x00000000, /* 14 */
	0x00000000, /* 15 */
	0x00000000, /* 16 */
	0x00000000, /* 17 */
	0x00000000, /* 18 */
	0x00000000, /* 19 */
	0x0020c016, /* 20 */
	0x0020c00d, /* 21 */
	0x00442001, /* 22 */
	0x00222021, /* 23 */
	0x00a88000, /* 24 */
	0x00462001, /* 25 */
	0x00242018, /* 26 */
	0x00442005, /* 27 */
	0x0024001f, /* 28 */
	0x00441006, /* 29 */
	0x0024101f, /* 30 */
	0x00442020, /* 31 */
	0x0040c1e0, /* 32 */
	0x0020c00b, /* 33 */
	0x00815000, /* 34 */
	0x00000000, /* 35 */
	0x00000000, /* 36 */
	0x00000000, /* 37 */
	0x00000000, /* 38 */
	0x00000000, /* 39 */
	0x00400801, /* 40 */
	0x00200072, /* 41 */
	0x0020103c, /* 42 */
	0x00208050, /* 43 */
	0x00200024, /* 44 */
	0x0020103c, /* 45 */
	0x00208050, /* 46 */
	0x00200024, /* 47 */
	0x0020103c, /* 48 */
	0x00208050, /* 49 */
	0x00200024, /* 50 */
	0x0020103c, /* 51 */
	0x00208050, /* 52 */
	0x00200024, /* 53 */
	0x00828000, /* 54 */
	0x00000000, /* 55 */
	0x00000000, /* 56 */
	0x00000000, /* 57 */
	0x00000000, /* 58 */
	0x00000000, /* 59 */
	0x00000000, /* 60 */
	0x00000000, /* 61 */
	0x00000000, /* 62 */
	0x00000000, /* 63 */
	0x0040000c, /* 64 */
	0x00644008, /* 65 */
	0x004001f8, /* 66 */
	0x00840000, /* 67 */
	0x0020900b, /* 68 */
	0x00509001, /* 69 */
	0x00000000, /* 70 */
	0x00000000, /* 71 */
	0x00000000, /* 72 */
	0x00000000, /* 73 */
	0x00000000, /* 74 */
	0x00000000, /* 75 */
	0x00000000, /* 76 */
	0x00000000, /* 77 */
	0x00000000, /* 78 */
	0x00000000, /* 79 */
	0x00000000, /* 80 */
	0x00000000, /* 81 */
	0x00000000, /* 82 */
	0x00000000, /* 83 */
	0x00000000, /* 84 */
	0x00000000, /* 85 */
	0x00000000, /* 86 */
	0x00000000, /* 87 */
	0x00000000, /* 88 */
	0x00000000, /* 89 */
	0x00000000, /* 90 */
	0x00000000, /* 91 */
	0x00000000, /* 92 */
	0x00000000, /* 93 */
	0x00000000, /* 94 */
	0x00000000, /* 95 */
	0x00000000, /* 96 */
	0x00000000, /* 97 */
	0x00000000, /* 98 */
	0x00000000, /* 99 */
	0x00000000, /* 100 */
	0x00000000, /* 101 */
	0x00000000, /* 102 */
	0x00000000, /* 103 */
	0x00000000, /* 104 */
	0x00000000, /* 105 */
	0x00000000, /* 106 */
	0x00000000, /* 107 */
	0x00000000, /* 108 */
	0x00000000, /* 109 */
	0x00000000, /* 110 */
	0x00000000, /* 111 */
	0x00000000, /* 112 */
	0x00000000, /* 113 */
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
	0x00000000, /* 126 */
	0x00000000, /* 127 */
	0x00000000, /* 128 */
	0x00000000, /* 129 */
	0x00000000, /* 130 */
	0x00000000, /* 131 */
	0x00000000, /* 132 */
	0x00000000, /* 133 */
	0x00000000, /* 134 */
	0x00000000, /* 135 */
	0x00000000, /* 136 */
	0x00000000, /* 137 */
	0x00000000, /* 138 */
	0x00000000, /* 139 */
	0x00000000, /* 140 */
	0x00000000, /* 141 */
	0x00000000, /* 142 */
	0x00000000, /* 143 */
	0x00000000, /* 144 */
	0x00000000, /* 145 */
	0x00000000, /* 146 */
	0x00000000, /* 147 */
	0x00000000, /* 148 */
	0x00000000, /* 149 */
	0x00000000, /* 150 */
	0x00000000, /* 151 */
	0x00000000, /* 152 */
	0x00000000, /* 153 */
	0x00000000, /* 154 */
	0x00000000, /* 155 */
	0x00000000, /* 156 */
	0x00000000, /* 157 */
	0x00000000, /* 158 */
	0x00000000, /* 159 */
	0x00000000, /* 160 */
	0x00000000, /* 161 */
	0x00000000, /* 162 */
	0x00000000, /* 163 */
	0x00000000, /* 164 */
	0x00000000, /* 165 */
	0x00000000, /* 166 */
	0x00000000, /* 167 */
	0x00000000, /* 168 */
	0x00000000, /* 169 */
	0x00000000, /* 170 */
	0x00000000, /* 171 */
	0x00000000, /* 172 */
	0x00000000, /* 173 */
	0x00000000, /* 174 */
	0x00000000, /* 175 */
	0x00000000, /* 176 */
	0x00000000, /* 177 */
	0x00000000, /* 178 */
	0x00000000, /* 179 */
	0x00000000, /* 180 */
	0x00000000, /* 181 */
	0x00000000, /* 182 */
	0x00000000, /* 183 */
	0x00000000, /* 184 */
	0x00000000, /* 185 */
	0x00000000, /* 186 */
	0x00000000, /* 187 */
	0x00000000, /* 188 */
	0x00000000, /* 189 */
	0x00000000, /* 190 */
	0x00000000, /* 191 */
	0x00000000, /* 192 */
	0x00000000, /* 193 */
	0x00000000, /* 194 */
	0x00000000, /* 195 */
	0x00000000, /* 196 */
	0x00000000, /* 197 */
	0x00000000, /* 198 */
	0x00000000, /* 199 */
	0x00000000, /* 200 */
	0x00000000, /* 201 */
	0x00000000, /* 202 */
	0x00000000, /* 203 */
	0x00000000, /* 204 */
	0x00000000, /* 205 */
	0x00000000, /* 206 */
	0x00000000, /* 207 */
	0x00000000, /* 208 */
	0x00000000, /* 209 */
	0x00000000, /* 210 */
	0x00000000, /* 211 */
	0x00000000, /* 212 */
	0x00000000, /* 213 */
	0x00000000, /* 214 */
	0x00000000, /* 215 */
	0x00000000, /* 216 */
	0x00000000, /* 217 */
	0x00000000, /* 218 */
	0x00000000, /* 219 */
	0x00000000, /* 220 */
	0x00000000, /* 221 */
	0x00000000, /* 222 */
	0x00000000, /* 223 */
	0x00000000, /* 224 */
	0x00000000, /* 225 */
	0x00000000, /* 226 */
	0x00000000, /* 227 */
	0x00000000, /* 228 */
	0x00000000, /* 229 */
	0x00000000, /* 230 */
	0x00000000, /* 231 */
	0x00000000, /* 232 */
	0x00000000, /* 233 */
	0x00000000, /* 234 */
	0x00000000, /* 235 */
	0x00000000, /* 236 */
	0x00000000, /* 237 */
	0x00000000, /* 238 */
	0x00000000, /* 239 */
	0x00000000, /* 240 */
	0x00000000, /* 241 */
	0x00000000, /* 242 */
	0x00000000, /* 243 */
	0x00000000, /* 244 */
	0x00000000, /* 245 */
	0x00000000, /* 246 */
	0x00000000, /* 247 */
	0x00000000, /* 248 */
	0x00000000, /* 249 */
	0x00000000, /* 250 */
	0x00000000, /* 251 */
	0x00000000, /* 252 */
	0x008fd000, /* 253 */
	0x00041212, /* 254 */
	0x85d2cca3, /* 255 */
};

/* From vec_625p_macrovision_type_1.bss */
static const uint32_t s_aulRamBVBInput_576p[BVDC_P_RAM_TABLE_SIZE] =
{
	0x00a24000, /* 0 */
	0x0020701f, /* 1 */
	0x0020404d, /* 2 */
	0x0028c293, /* 3 */
	0x0028802b, /* 4 */
	0x00240015, /* 5 */
	0x0020700b, /* 6 */
	0x00800000, /* 7 */
	0x00000000, /* 8 */
	0x00000000, /* 9 */
	0x00000000, /* 10 */
	0x00000000, /* 11 */
	0x00000000, /* 12 */
	0x00000000, /* 13 */
	0x00000000, /* 14 */
	0x00000000, /* 15 */
	0x00000000, /* 16 */
	0x00000000, /* 17 */
	0x00000000, /* 18 */
	0x00000000, /* 19 */
	0x00ac8000, /* 20 */
	0x00461001, /* 21 */
	0x00441004, /* 22 */
	0x00241018, /* 23 */
	0x00442027, /* 24 */
	0x0040c240, /* 25 */
	0x00442005, /* 26 */
	0x00262018, /* 27 */
	0x00814000, /* 28 */
	0x00000000, /* 29 */
	0x00000000, /* 30 */
	0x00000000, /* 31 */
	0x00000000, /* 32 */
	0x00000000, /* 33 */
	0x00000000, /* 34 */
	0x00000000, /* 35 */
	0x00000000, /* 36 */
	0x00000000, /* 37 */
	0x00000000, /* 38 */
	0x00000000, /* 39 */
	0x00000000, /* 40 */
	0x00000000, /* 41 */
	0x00400801, /* 42 */
	0x00200038, /* 43 */
	0x0020101e, /* 44 */
	0x00208028, /* 45 */
	0x00200012, /* 46 */
	0x0020101e, /* 47 */
	0x00208028, /* 48 */
	0x00200012, /* 49 */
	0x0020101e, /* 50 */
	0x00208028, /* 51 */
	0x00200012, /* 52 */
	0x0020101e, /* 53 */
	0x00208028, /* 54 */
	0x00200012, /* 55 */
	0x0082a000, /* 56 */
	0x00000000, /* 57 */
	0x00000000, /* 58 */
	0x00000000, /* 59 */
	0x00000000, /* 60 */
	0x00000000, /* 61 */
	0x00000000, /* 62 */
	0x00000000, /* 63 */
	0x00400005, /* 64 */
	0x00644008, /* 65 */
	0x00400264, /* 66 */
	0x00840000, /* 67 */
	0x0020900b, /* 68 */
	0x00509001, /* 69 */
	0x00000000, /* 70 */
	0x00000000, /* 71 */
	0x00000000, /* 72 */
	0x00000000, /* 73 */
	0x00000000, /* 74 */
	0x00000000, /* 75 */
	0x00000000, /* 76 */
	0x00000000, /* 77 */
	0x00000000, /* 78 */
	0x00000000, /* 79 */
	0x00000000, /* 80 */
	0x00000000, /* 81 */
	0x00000000, /* 82 */
	0x00000000, /* 83 */
	0x00000000, /* 84 */
	0x00000000, /* 85 */
	0x00000000, /* 86 */
	0x00000000, /* 87 */
	0x00000000, /* 88 */
	0x00000000, /* 89 */
	0x00000000, /* 90 */
	0x00000000, /* 91 */
	0x00000000, /* 92 */
	0x00000000, /* 93 */
	0x00000000, /* 94 */
	0x00000000, /* 95 */
	0x00000000, /* 96 */
	0x00000000, /* 97 */
	0x00000000, /* 98 */
	0x00000000, /* 99 */
	0x00000000, /* 100 */
	0x00000000, /* 101 */
	0x00000000, /* 102 */
	0x00000000, /* 103 */
	0x00000000, /* 104 */
	0x00000000, /* 105 */
	0x00000000, /* 106 */
	0x00000000, /* 107 */
	0x00000000, /* 108 */
	0x00000000, /* 109 */
	0x00000000, /* 110 */
	0x00000000, /* 111 */
	0x00000000, /* 112 */
	0x00000000, /* 113 */
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
	0x00000000, /* 126 */
	0x00000000, /* 127 */
	0x00000000, /* 128 */
	0x00000000, /* 129 */
	0x00000000, /* 130 */
	0x00000000, /* 131 */
	0x00000000, /* 132 */
	0x00000000, /* 133 */
	0x00000000, /* 134 */
	0x00000000, /* 135 */
	0x00000000, /* 136 */
	0x00000000, /* 137 */
	0x00000000, /* 138 */
	0x00000000, /* 139 */
	0x00000000, /* 140 */
	0x00000000, /* 141 */
	0x00000000, /* 142 */
	0x00000000, /* 143 */
	0x00000000, /* 144 */
	0x00000000, /* 145 */
	0x00000000, /* 146 */
	0x00000000, /* 147 */
	0x00000000, /* 148 */
	0x00000000, /* 149 */
	0x00000000, /* 150 */
	0x00000000, /* 151 */
	0x00000000, /* 152 */
	0x00000000, /* 153 */
	0x00000000, /* 154 */
	0x00000000, /* 155 */
	0x00000000, /* 156 */
	0x00000000, /* 157 */
	0x00000000, /* 158 */
	0x00000000, /* 159 */
	0x00000000, /* 160 */
	0x00000000, /* 161 */
	0x00000000, /* 162 */
	0x00000000, /* 163 */
	0x00000000, /* 164 */
	0x00000000, /* 165 */
	0x00000000, /* 166 */
	0x00000000, /* 167 */
	0x00000000, /* 168 */
	0x00000000, /* 169 */
	0x00000000, /* 170 */
	0x00000000, /* 171 */
	0x00000000, /* 172 */
	0x00000000, /* 173 */
	0x00000000, /* 174 */
	0x00000000, /* 175 */
	0x00000000, /* 176 */
	0x00000000, /* 177 */
	0x00000000, /* 178 */
	0x00000000, /* 179 */
	0x00000000, /* 180 */
	0x00000000, /* 181 */
	0x00000000, /* 182 */
	0x00000000, /* 183 */
	0x00000000, /* 184 */
	0x00000000, /* 185 */
	0x00000000, /* 186 */
	0x00000000, /* 187 */
	0x00000000, /* 188 */
	0x00000000, /* 189 */
	0x00000000, /* 190 */
	0x00000000, /* 191 */
	0x00000000, /* 192 */
	0x00000000, /* 193 */
	0x00000000, /* 194 */
	0x00000000, /* 195 */
	0x00000000, /* 196 */
	0x00000000, /* 197 */
	0x00000000, /* 198 */
	0x00000000, /* 199 */
	0x00000000, /* 200 */
	0x00000000, /* 201 */
	0x00000000, /* 202 */
	0x00000000, /* 203 */
	0x00000000, /* 204 */
	0x00000000, /* 205 */
	0x00000000, /* 206 */
	0x00000000, /* 207 */
	0x00000000, /* 208 */
	0x00000000, /* 209 */
	0x00000000, /* 210 */
	0x00000000, /* 211 */
	0x00000000, /* 212 */
	0x00000000, /* 213 */
	0x00000000, /* 214 */
	0x00000000, /* 215 */
	0x00000000, /* 216 */
	0x00000000, /* 217 */
	0x00000000, /* 218 */
	0x00000000, /* 219 */
	0x00000000, /* 220 */
	0x00000000, /* 221 */
	0x00000000, /* 222 */
	0x00000000, /* 223 */
	0x00000000, /* 224 */
	0x00000000, /* 225 */
	0x00000000, /* 226 */
	0x00000000, /* 227 */
	0x00000000, /* 228 */
	0x00000000, /* 229 */
	0x00000000, /* 230 */
	0x00000000, /* 231 */
	0x00000000, /* 232 */
	0x00000000, /* 233 */
	0x00000000, /* 234 */
	0x00000000, /* 235 */
	0x00000000, /* 236 */
	0x00000000, /* 237 */
	0x00000000, /* 238 */
	0x00000000, /* 239 */
	0x00000000, /* 240 */
	0x00000000, /* 241 */
	0x00000000, /* 242 */
	0x00000000, /* 243 */
	0x00000000, /* 244 */
	0x00000000, /* 245 */
	0x00000000, /* 246 */
	0x00000000, /* 247 */
	0x00000000, /* 248 */
	0x00000000, /* 249 */
	0x00000000, /* 250 */
	0x00000000, /* 251 */
	0x00000000, /* 252 */
	0x008fd000, /* 253 */
	0x00122407, /* 254 */
	0x28b3ea1b, /* 255 */
};

/* From vec_625p_rm54_macrovision_type_1.bss */
static const uint32_t s_aulRamBVBInput_576p_54MHz[BVDC_P_RAM_TABLE_SIZE] =
{
	0x00a24000, /* 0 */
	0x0020705f, /* 1 */
	0x0020409a, /* 2 */
	0x0028c526, /* 3 */
	0x00288056, /* 4 */
	0x0024002a, /* 5 */
	0x0020700b, /* 6 */
	0x00800000, /* 7 */
	0x00000000, /* 8 */
	0x00000000, /* 9 */
	0x00000000, /* 10 */
	0x00000000, /* 11 */
	0x00000000, /* 12 */
	0x00000000, /* 13 */
	0x00000000, /* 14 */
	0x00000000, /* 15 */
	0x00000000, /* 16 */
	0x00000000, /* 17 */
	0x00000000, /* 18 */
	0x00000000, /* 19 */
	0x00ac8000, /* 20 */
	0x00461001, /* 21 */
	0x00441004, /* 22 */
	0x00241018, /* 23 */
	0x00442027, /* 24 */
	0x0040c240, /* 25 */
	0x00442005, /* 26 */
	0x00262018, /* 27 */
	0x00814000, /* 28 */
	0x00000000, /* 29 */
	0x00000000, /* 30 */
	0x00000000, /* 31 */
	0x00000000, /* 32 */
	0x00000000, /* 33 */
	0x00000000, /* 34 */
	0x00000000, /* 35 */
	0x00000000, /* 36 */
	0x00000000, /* 37 */
	0x00000000, /* 38 */
	0x00000000, /* 39 */
	0x00000000, /* 40 */
	0x00000000, /* 41 */
	0x00400801, /* 42 */
	0x00200070, /* 43 */
	0x0020103c, /* 44 */
	0x00208050, /* 45 */
	0x00200024, /* 46 */
	0x0020103c, /* 47 */
	0x00208050, /* 48 */
	0x00200024, /* 49 */
	0x0020103c, /* 50 */
	0x00208050, /* 51 */
	0x00200024, /* 52 */
	0x0020103c, /* 53 */
	0x00208050, /* 54 */
	0x00200024, /* 55 */
	0x0082a000, /* 56 */
	0x00000000, /* 57 */
	0x00000000, /* 58 */
	0x00000000, /* 59 */
	0x00000000, /* 60 */
	0x00000000, /* 61 */
	0x00000000, /* 62 */
	0x00000000, /* 63 */
	0x00400005, /* 64 */
	0x00644008, /* 65 */
	0x00400264, /* 66 */
	0x00840000, /* 67 */
	0x0020900b, /* 68 */
	0x00509001, /* 69 */
	0x00000000, /* 70 */
	0x00000000, /* 71 */
	0x00000000, /* 72 */
	0x00000000, /* 73 */
	0x00000000, /* 74 */
	0x00000000, /* 75 */
	0x00000000, /* 76 */
	0x00000000, /* 77 */
	0x00000000, /* 78 */
	0x00000000, /* 79 */
	0x00000000, /* 80 */
	0x00000000, /* 81 */
	0x00000000, /* 82 */
	0x00000000, /* 83 */
	0x00000000, /* 84 */
	0x00000000, /* 85 */
	0x00000000, /* 86 */
	0x00000000, /* 87 */
	0x00000000, /* 88 */
	0x00000000, /* 89 */
	0x00000000, /* 90 */
	0x00000000, /* 91 */
	0x00000000, /* 92 */
	0x00000000, /* 93 */
	0x00000000, /* 94 */
	0x00000000, /* 95 */
	0x00000000, /* 96 */
	0x00000000, /* 97 */
	0x00000000, /* 98 */
	0x00000000, /* 99 */
	0x00000000, /* 100 */
	0x00000000, /* 101 */
	0x00000000, /* 102 */
	0x00000000, /* 103 */
	0x00000000, /* 104 */
	0x00000000, /* 105 */
	0x00000000, /* 106 */
	0x00000000, /* 107 */
	0x00000000, /* 108 */
	0x00000000, /* 109 */
	0x00000000, /* 110 */
	0x00000000, /* 111 */
	0x00000000, /* 112 */
	0x00000000, /* 113 */
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
	0x00000000, /* 126 */
	0x00000000, /* 127 */
	0x00000000, /* 128 */
	0x00000000, /* 129 */
	0x00000000, /* 130 */
	0x00000000, /* 131 */
	0x00000000, /* 132 */
	0x00000000, /* 133 */
	0x00000000, /* 134 */
	0x00000000, /* 135 */
	0x00000000, /* 136 */
	0x00000000, /* 137 */
	0x00000000, /* 138 */
	0x00000000, /* 139 */
	0x00000000, /* 140 */
	0x00000000, /* 141 */
	0x00000000, /* 142 */
	0x00000000, /* 143 */
	0x00000000, /* 144 */
	0x00000000, /* 145 */
	0x00000000, /* 146 */
	0x00000000, /* 147 */
	0x00000000, /* 148 */
	0x00000000, /* 149 */
	0x00000000, /* 150 */
	0x00000000, /* 151 */
	0x00000000, /* 152 */
	0x00000000, /* 153 */
	0x00000000, /* 154 */
	0x00000000, /* 155 */
	0x00000000, /* 156 */
	0x00000000, /* 157 */
	0x00000000, /* 158 */
	0x00000000, /* 159 */
	0x00000000, /* 160 */
	0x00000000, /* 161 */
	0x00000000, /* 162 */
	0x00000000, /* 163 */
	0x00000000, /* 164 */
	0x00000000, /* 165 */
	0x00000000, /* 166 */
	0x00000000, /* 167 */
	0x00000000, /* 168 */
	0x00000000, /* 169 */
	0x00000000, /* 170 */
	0x00000000, /* 171 */
	0x00000000, /* 172 */
	0x00000000, /* 173 */
	0x00000000, /* 174 */
	0x00000000, /* 175 */
	0x00000000, /* 176 */
	0x00000000, /* 177 */
	0x00000000, /* 178 */
	0x00000000, /* 179 */
	0x00000000, /* 180 */
	0x00000000, /* 181 */
	0x00000000, /* 182 */
	0x00000000, /* 183 */
	0x00000000, /* 184 */
	0x00000000, /* 185 */
	0x00000000, /* 186 */
	0x00000000, /* 187 */
	0x00000000, /* 188 */
	0x00000000, /* 189 */
	0x00000000, /* 190 */
	0x00000000, /* 191 */
	0x00000000, /* 192 */
	0x00000000, /* 193 */
	0x00000000, /* 194 */
	0x00000000, /* 195 */
	0x00000000, /* 196 */
	0x00000000, /* 197 */
	0x00000000, /* 198 */
	0x00000000, /* 199 */
	0x00000000, /* 200 */
	0x00000000, /* 201 */
	0x00000000, /* 202 */
	0x00000000, /* 203 */
	0x00000000, /* 204 */
	0x00000000, /* 205 */
	0x00000000, /* 206 */
	0x00000000, /* 207 */
	0x00000000, /* 208 */
	0x00000000, /* 209 */
	0x00000000, /* 210 */
	0x00000000, /* 211 */
	0x00000000, /* 212 */
	0x00000000, /* 213 */
	0x00000000, /* 214 */
	0x00000000, /* 215 */
	0x00000000, /* 216 */
	0x00000000, /* 217 */
	0x00000000, /* 218 */
	0x00000000, /* 219 */
	0x00000000, /* 220 */
	0x00000000, /* 221 */
	0x00000000, /* 222 */
	0x00000000, /* 223 */
	0x00000000, /* 224 */
	0x00000000, /* 225 */
	0x00000000, /* 226 */
	0x00000000, /* 227 */
	0x00000000, /* 228 */
	0x00000000, /* 229 */
	0x00000000, /* 230 */
	0x00000000, /* 231 */
	0x00000000, /* 232 */
	0x00000000, /* 233 */
	0x00000000, /* 234 */
	0x00000000, /* 235 */
	0x00000000, /* 236 */
	0x00000000, /* 237 */
	0x00000000, /* 238 */
	0x00000000, /* 239 */
	0x00000000, /* 240 */
	0x00000000, /* 241 */
	0x00000000, /* 242 */
	0x00000000, /* 243 */
	0x00000000, /* 244 */
	0x00000000, /* 245 */
	0x00000000, /* 246 */
	0x00000000, /* 247 */
	0x00000000, /* 248 */
	0x00000000, /* 249 */
	0x00000000, /* 250 */
	0x00000000, /* 251 */
	0x00000000, /* 252 */
	0x008fd000, /* 253 */
	0x00051608, /* 254 */
	0x13fb9035, /* 255 */
};

/* vec_vesa_1600x1200_bvb_input.bss */
static const uint32_t s_aulRamBVBInput_1600x1200p_60Hz[BVDC_P_RAM_TABLE_SIZE] =
{
	0x00a24000, /* 0 */
	0x00200040, /* 1 */
	0x002010c0, /* 2 */
	0x0024012b, /* 3 */
	0x00208624, /* 4 */
	0x0020800b, /* 5 */
	0x00800000, /* 6 */
	0x00000000, /* 7 */
	0x00000000, /* 8 */
	0x00000000, /* 9 */
	0x00000000, /* 10 */
	0x00000000, /* 11 */
	0x00000000, /* 12 */
	0x00000000, /* 13 */
	0x00000000, /* 14 */
	0x00000000, /* 15 */
	0x00000000, /* 16 */
	0x00000000, /* 17 */
	0x00000000, /* 18 */
	0x00000000, /* 19 */
	0x00a40000, /* 20 */
	0x00a80000, /* 21 */
	0x00420001, /* 22 */
	0x0040001b, /* 23 */
	0x004084b0, /* 24 */
	0x00400001, /* 25 */
	0x00815000, /* 26 */
	0x00000000, /* 27 */
	0x00000000, /* 28 */
	0x00000000, /* 29 */
	0x00000000, /* 30 */
	0x00000000, /* 31 */
	0x00000000, /* 32 */
	0x00000000, /* 33 */
	0x00000000, /* 34 */
	0x00000000, /* 35 */
	0x00000000, /* 36 */
	0x00000000, /* 37 */
	0x00000000, /* 38 */
	0x00000000, /* 39 */
	0x00000000, /* 40 */
	0x00000000, /* 41 */
	0x00000000, /* 42 */
	0x00000000, /* 43 */
	0x00000000, /* 44 */
	0x00000000, /* 45 */
	0x00000000, /* 46 */
	0x00000000, /* 47 */
	0x00000000, /* 48 */
	0x00000000, /* 49 */
	0x00000000, /* 50 */
	0x00000000, /* 51 */
	0x00000000, /* 52 */
	0x00000000, /* 53 */
	0x00000000, /* 54 */
	0x00000000, /* 55 */
	0x00000000, /* 56 */
	0x00000000, /* 57 */
	0x00000000, /* 58 */
	0x00000000, /* 59 */
	0x00000000, /* 60 */
	0x00000000, /* 61 */
	0x00000000, /* 62 */
	0x00000000, /* 63 */
	0x00000000, /* 64 */
	0x00000000, /* 65 */
	0x00000000, /* 66 */
	0x00000000, /* 67 */
	0x00000000, /* 68 */
	0x00000000, /* 69 */
	0x00000000, /* 70 */
	0x00000000, /* 71 */
	0x00000000, /* 72 */
	0x00000000, /* 73 */
	0x00000000, /* 74 */
	0x00000000, /* 75 */
	0x00000000, /* 76 */
	0x00000000, /* 77 */
	0x00000000, /* 78 */
	0x00000000, /* 79 */
	0x00000000, /* 80 */
	0x00000000, /* 81 */
	0x00000000, /* 82 */
	0x00000000, /* 83 */
	0x00000000, /* 84 */
	0x00000000, /* 85 */
	0x00000000, /* 86 */
	0x00000000, /* 87 */
	0x00000000, /* 88 */
	0x00000000, /* 89 */
	0x00000000, /* 90 */
	0x00000000, /* 91 */
	0x00000000, /* 92 */
	0x00000000, /* 93 */
	0x00000000, /* 94 */
	0x00000000, /* 95 */
	0x00000000, /* 96 */
	0x00000000, /* 97 */
	0x00000000, /* 98 */
	0x00000000, /* 99 */
	0x00000000, /* 100 */
	0x00000000, /* 101 */
	0x00000000, /* 102 */
	0x00000000, /* 103 */
	0x00000000, /* 104 */
	0x00000000, /* 105 */
	0x00000000, /* 106 */
	0x00000000, /* 107 */
	0x00000000, /* 108 */
	0x00000000, /* 109 */
	0x00000000, /* 110 */
	0x00000000, /* 111 */
	0x00000000, /* 112 */
	0x00000000, /* 113 */
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
	0x00000000, /* 126 */
	0x00000000, /* 127 */
	0x00000000, /* 128 */
	0x00000000, /* 129 */
	0x00000000, /* 130 */
	0x00000000, /* 131 */
	0x00000000, /* 132 */
	0x00000000, /* 133 */
	0x00000000, /* 134 */
	0x00000000, /* 135 */
	0x00000000, /* 136 */
	0x00000000, /* 137 */
	0x00000000, /* 138 */
	0x00000000, /* 139 */
	0x00000000, /* 140 */
	0x00000000, /* 141 */
	0x00000000, /* 142 */
	0x00000000, /* 143 */
	0x00000000, /* 144 */
	0x00000000, /* 145 */
	0x00000000, /* 146 */
	0x00000000, /* 147 */
	0x00000000, /* 148 */
	0x00000000, /* 149 */
	0x00000000, /* 150 */
	0x00000000, /* 151 */
	0x00000000, /* 152 */
	0x00000000, /* 153 */
	0x00000000, /* 154 */
	0x00000000, /* 155 */
	0x00000000, /* 156 */
	0x00000000, /* 157 */
	0x00000000, /* 158 */
	0x00000000, /* 159 */
	0x00000000, /* 160 */
	0x00000000, /* 161 */
	0x00000000, /* 162 */
	0x00000000, /* 163 */
	0x00000000, /* 164 */
	0x00000000, /* 165 */
	0x00000000, /* 166 */
	0x00000000, /* 167 */
	0x00000000, /* 168 */
	0x00000000, /* 169 */
	0x00000000, /* 170 */
	0x00000000, /* 171 */
	0x00000000, /* 172 */
	0x00000000, /* 173 */
	0x00000000, /* 174 */
	0x00000000, /* 175 */
	0x00000000, /* 176 */
	0x00000000, /* 177 */
	0x00000000, /* 178 */
	0x00000000, /* 179 */
	0x00000000, /* 180 */
	0x00000000, /* 181 */
	0x00000000, /* 182 */
	0x00000000, /* 183 */
	0x00000000, /* 184 */
	0x00000000, /* 185 */
	0x00000000, /* 186 */
	0x00000000, /* 187 */
	0x00000000, /* 188 */
	0x00000000, /* 189 */
	0x00000000, /* 190 */
	0x00000000, /* 191 */
	0x00000000, /* 192 */
	0x00000000, /* 193 */
	0x00000000, /* 194 */
	0x00000000, /* 195 */
	0x00000000, /* 196 */
	0x00000000, /* 197 */
	0x00000000, /* 198 */
	0x00000000, /* 199 */
	0x00000000, /* 200 */
	0x00000000, /* 201 */
	0x00000000, /* 202 */
	0x00000000, /* 203 */
	0x00000000, /* 204 */
	0x00000000, /* 205 */
	0x00000000, /* 206 */
	0x00000000, /* 207 */
	0x00000000, /* 208 */
	0x00000000, /* 209 */
	0x00000000, /* 210 */
	0x00000000, /* 211 */
	0x00000000, /* 212 */
	0x00000000, /* 213 */
	0x00000000, /* 214 */
	0x00000000, /* 215 */
	0x00000000, /* 216 */
	0x00000000, /* 217 */
	0x00000000, /* 218 */
	0x00000000, /* 219 */
	0x00000000, /* 220 */
	0x00000000, /* 221 */
	0x00000000, /* 222 */
	0x00000000, /* 223 */
	0x00000000, /* 224 */
	0x00000000, /* 225 */
	0x00000000, /* 226 */
	0x00000000, /* 227 */
	0x00000000, /* 228 */
	0x00000000, /* 229 */
	0x00000000, /* 230 */
	0x00000000, /* 231 */
	0x00000000, /* 232 */
	0x00000000, /* 233 */
	0x00000000, /* 234 */
	0x00000000, /* 235 */
	0x00000000, /* 236 */
	0x00000000, /* 237 */
	0x00000000, /* 238 */
	0x00000000, /* 239 */
	0x00000000, /* 240 */
	0x00000000, /* 241 */
	0x00000000, /* 242 */
	0x00000000, /* 243 */
	0x00000000, /* 244 */
	0x00000000, /* 245 */
	0x00000000, /* 246 */
	0x00000000, /* 247 */
	0x00000000, /* 248 */
	0x00000000, /* 249 */
	0x00000000, /* 250 */
	0x00000000, /* 251 */
	0x00000000, /* 252 */
	0x00000000, /* 253 */
	0x00092706, /* 254 */
	0xeec773f0, /* 255 */
};

/* ANA_MCODE_1360x768pVESA_60Hz.bss */
static const uint32_t
	s_aulRamBVBInput_DVI_1360x768p_60Hz[BVDC_P_RAM_TABLE_SIZE] =
{
	0x00a24000, /* 0 */
	0x00240040, /* 1 */
	0x00201070, /* 2 */
	0x002000f7, /* 3 */
	0x00208543, /* 4 */
	0x00800000, /* 5 */
	0x00000000, /* 6 */
	0x00000000, /* 7 */
	0x00000000, /* 8 */
	0x00000000, /* 9 */
	0x00000000, /* 10 */
	0x00000000, /* 11 */
	0x00000000, /* 12 */
	0x00000000, /* 13 */
	0x00000000, /* 14 */
	0x00000000, /* 15 */
	0x00000000, /* 16 */
	0x00000000, /* 17 */
	0x00000000, /* 18 */
	0x00000000, /* 19 */
	0x00a80000, /* 20 */
	0x00421001, /* 21 */
	0x00401005, /* 22 */
	0x00400012, /* 23 */
	0x00408300, /* 24 */
	0x00a80000, /* 25 */
	0x00400003, /* 26 */
	0x00815000, /* 27 */
	0x00000000, /* 28' */
	0x00000000, /* 29' */
	0x00000000, /* 30' */
	0x00000000, /* 31' */
	0x00000000, /* 32' */
	0x00000000, /* 33' */
	0x00000000, /* 34' */
	0x00000000, /* 35' */
	0x00000000, /* 36' */
	0x00000000, /* 37' */
	0x00000000, /* 38' */
	0x00000000, /* 39' */
	0x00000000, /* 40' */
	0x00000000, /* 41' */
	0x00000000, /* 42' */
	0x00000000, /* 43' */
	0x00000000, /* 44' */
	0x00000000, /* 45' */
	0x00000000, /* 46' */
	0x00000000, /* 47' */
	0x00000000, /* 48' */
	0x00000000, /* 49' */
	0x00000000, /* 50' */
	0x00000000, /* 51' */
	0x00000000, /* 52' */
	0x00000000, /* 53' */
	0x00000000, /* 54' */
	0x00000000, /* 55' */
	0x00000000, /* 56' */
	0x00000000, /* 57' */
	0x00000000, /* 58' */
	0x00000000, /* 59' */
	0x00000000, /* 60' */
	0x00000000, /* 61' */
	0x00000000, /* 62' */
	0x00000000, /* 63' */
	0x00000000, /* 64' */
	0x00000000, /* 65' */
	0x00000000, /* 66' */
	0x00000000, /* 67' */
	0x00000000, /* 68' */
	0x00000000, /* 69' */
	0x00000000, /* 70' */
	0x00000000, /* 71' */
	0x00000000, /* 72' */
	0x00000000, /* 73' */
	0x00000000, /* 74' */
	0x00000000, /* 75' */
	0x00000000, /* 76' */
	0x00000000, /* 77' */
	0x00000000, /* 78' */
	0x00000000, /* 79' */
	0x00000000, /* 80' */
	0x00000000, /* 81' */
	0x00000000, /* 82' */
	0x00000000, /* 83' */
	0x00000000, /* 84' */
	0x00000000, /* 85' */
	0x00000000, /* 86' */
	0x00000000, /* 87' */
	0x00000000, /* 88' */
	0x00000000, /* 89' */
	0x00000000, /* 90' */
	0x00000000, /* 91' */
	0x00000000, /* 92' */
	0x00000000, /* 93' */
	0x00000000, /* 94' */
	0x00000000, /* 95' */
	0x00000000, /* 96' */
	0x00000000, /* 97' */
	0x00000000, /* 98' */
	0x00000000, /* 99' */
	0x00000000, /* 100' */
	0x00000000, /* 101' */
	0x00000000, /* 102' */
	0x00000000, /* 103' */
	0x00000000, /* 104' */
	0x00000000, /* 105' */
	0x00000000, /* 106' */
	0x00000000, /* 107' */
	0x00000000, /* 108' */
	0x00000000, /* 109' */
	0x00000000, /* 110' */
	0x00000000, /* 111' */
	0x00000000, /* 112' */
	0x00000000, /* 113' */
	0x00000000, /* 114' */
	0x00000000, /* 115' */
	0x00000000, /* 116' */
	0x00000000, /* 117' */
	0x00000000, /* 118' */
	0x00000000, /* 119' */
	0x00000000, /* 120' */
	0x00000000, /* 121' */
	0x00000000, /* 122' */
	0x00000000, /* 123' */
	0x00000000, /* 124' */
	0x00000000, /* 125' */
	0x00000000, /* 126' */
	0x00000000, /* 127' */
	0x00000000, /* 128' */
	0x00000000, /* 129' */
	0x00000000, /* 130' */
	0x00000000, /* 131' */
	0x00000000, /* 132' */
	0x00000000, /* 133' */
	0x00000000, /* 134' */
	0x00000000, /* 135' */
	0x00000000, /* 136' */
	0x00000000, /* 137' */
	0x00000000, /* 138' */
	0x00000000, /* 139' */
	0x00000000, /* 140' */
	0x00000000, /* 141' */
	0x00000000, /* 142' */
	0x00000000, /* 143' */
	0x00000000, /* 144' */
	0x00000000, /* 145' */
	0x00000000, /* 146' */
	0x00000000, /* 147' */
	0x00000000, /* 148' */
	0x00000000, /* 149' */
	0x00000000, /* 150' */
	0x00000000, /* 151' */
	0x00000000, /* 152' */
	0x00000000, /* 153' */
	0x00000000, /* 154' */
	0x00000000, /* 155' */
	0x00000000, /* 156' */
	0x00000000, /* 157' */
	0x00000000, /* 158' */
	0x00000000, /* 159' */
	0x00000000, /* 160' */
	0x00000000, /* 161' */
	0x00000000, /* 162' */
	0x00000000, /* 163' */
	0x00000000, /* 164' */
	0x00000000, /* 165' */
	0x00000000, /* 166' */
	0x00000000, /* 167' */
	0x00000000, /* 168' */
	0x00000000, /* 169' */
	0x00000000, /* 170' */
	0x00000000, /* 171' */
	0x00000000, /* 172' */
	0x00000000, /* 173' */
	0x00000000, /* 174' */
	0x00000000, /* 175' */
	0x00000000, /* 176' */
	0x00000000, /* 177' */
	0x00000000, /* 178' */
	0x00000000, /* 179' */
	0x00000000, /* 180' */
	0x00000000, /* 181' */
	0x00000000, /* 182' */
	0x00000000, /* 183' */
	0x00000000, /* 184' */
	0x00000000, /* 185' */
	0x00000000, /* 186' */
	0x00000000, /* 187' */
	0x00000000, /* 188' */
	0x00000000, /* 189' */
	0x00000000, /* 190' */
	0x00000000, /* 191' */
	0x00000000, /* 192' */
	0x00000000, /* 193' */
	0x00000000, /* 194' */
	0x00000000, /* 195' */
	0x00000000, /* 196' */
	0x00000000, /* 197' */
	0x00000000, /* 198' */
	0x00000000, /* 199' */
	0x00000000, /* 200' */
	0x00000000, /* 201' */
	0x00000000, /* 202' */
	0x00000000, /* 203' */
	0x00000000, /* 204' */
	0x00000000, /* 205' */
	0x00000000, /* 206' */
	0x00000000, /* 207' */
	0x00000000, /* 208' */
	0x00000000, /* 209' */
	0x00000000, /* 210' */
	0x00000000, /* 211' */
	0x00000000, /* 212' */
	0x00000000, /* 213' */
	0x00000000, /* 214' */
	0x00000000, /* 215' */
	0x00000000, /* 216' */
	0x00000000, /* 217' */
	0x00000000, /* 218' */
	0x00000000, /* 219' */
	0x00000000, /* 220' */
	0x00000000, /* 221' */
	0x00000000, /* 222' */
	0x00000000, /* 223' */
	0x00000000, /* 224' */
	0x00000000, /* 225' */
	0x00000000, /* 226' */
	0x00000000, /* 227' */
	0x00000000, /* 228' */
	0x00000000, /* 229' */
	0x00000000, /* 230' */
	0x00000000, /* 231' */
	0x00000000, /* 232' */
	0x00000000, /* 233' */
	0x00000000, /* 234' */
	0x00000000, /* 235' */
	0x00000000, /* 236' */
	0x00000000, /* 237' */
	0x00000000, /* 238' */
	0x00000000, /* 239' */
	0x00000000, /* 240' */
	0x00000000, /* 241' */
	0x00000000, /* 242' */
	0x00000000, /* 243' */
	0x00000000, /* 244' */
	0x00000000, /* 245' */
	0x00000000, /* 246' */
	0x00000000, /* 247' */
	0x00000000, /* 248' */
	0x00000000, /* 249' */
	0x00000000, /* 250' */
	0x00000000, /* 251' */
	0x00000000, /* 252' */
	0x008fd000, /* 253 */
	0x00090109, /* 254 */
	0x00c5c4cc, /* 255 */
};

/* ANA_MCODE_3840x2160pDCI_24Hz.bss */
static const uint32_t s_aulRamBVBInput_3840x2160p_24Hz[BVDC_P_RAM_TABLE_SIZE] =
{
	0x00a24000, /* 0 */
	0x002404fc, /* 1 */
	0x00201058, /* 2 */
	0x0020011f, /* 3 */
	0x00208ef3, /* 4 */
	0x00800000, /* 5 */
	0x00000000, /* 6 */
	0x00000000, /* 7 */
	0x00000000, /* 8 */
	0x00000000, /* 9 */
	0x00000000, /* 10 */
	0x00000000, /* 11 */
	0x00000000, /* 12 */
	0x00000000, /* 13 */
	0x00000000, /* 14 */
	0x00000000, /* 15 */
	0x00000000, /* 16 */
	0x00000000, /* 17 */
	0x00000000, /* 18 */
	0x00000000, /* 19 */
	0x00a80000, /* 20 */
	0x00421001, /* 21 */
	0x00401009, /* 22 */
	0x00400048, /* 23 */
	0x00408870, /* 24 */
	0x00400008, /* 25 */
	0x00a80000, /* 26 */
	0x00815000, /* 27 */
	0x00000000, /* 28 */
	0x00000000, /* 29 */
	0x00000000, /* 30 */
	0x00000000, /* 31 */
	0x00000000, /* 32 */
	0x00000000, /* 33 */
	0x00000000, /* 34 */
	0x00000000, /* 35 */
	0x00000000, /* 36 */
	0x00000000, /* 37 */
	0x00000000, /* 38 */
	0x00000000, /* 39 */
	0x00000000, /* 40 */
	0x00000000, /* 41 */
	0x00000000, /* 42 */
	0x00000000, /* 43 */
	0x00000000, /* 44 */
	0x00000000, /* 45 */
	0x00000000, /* 46 */
	0x00000000, /* 47 */
	0x00000000, /* 48 */
	0x00000000, /* 49 */
	0x00000000, /* 50 */
	0x00000000, /* 51 */
	0x00000000, /* 52 */
	0x00000000, /* 53 */
	0x00000000, /* 54 */
	0x00000000, /* 55 */
	0x00000000, /* 56 */
	0x00000000, /* 57 */
	0x00000000, /* 58 */
	0x00000000, /* 59 */
	0x00000000, /* 60 */
	0x00000000, /* 61 */
	0x00000000, /* 62 */
	0x00000000, /* 63 */
	0x00000000, /* 64 */
	0x00000000, /* 65 */
	0x00000000, /* 66 */
	0x00000000, /* 67 */
	0x00000000, /* 68 */
	0x00000000, /* 69 */
	0x00000000, /* 70 */
	0x00000000, /* 71 */
	0x00000000, /* 72 */
	0x00000000, /* 73 */
	0x00000000, /* 74 */
	0x00000000, /* 75 */
	0x00000000, /* 76 */
	0x00000000, /* 77 */
	0x00000000, /* 78 */
	0x00000000, /* 79 */
	0x00000000, /* 80 */
	0x00000000, /* 81 */
	0x00000000, /* 82 */
	0x00000000, /* 83 */
	0x00000000, /* 84 */
	0x00000000, /* 85 */
	0x00000000, /* 86 */
	0x00000000, /* 87 */
	0x00000000, /* 88 */
	0x00000000, /* 89 */
	0x00000000, /* 90 */
	0x00000000, /* 91 */
	0x00000000, /* 92 */
	0x00000000, /* 93 */
	0x00000000, /* 94 */
	0x00000000, /* 95 */
	0x00000000, /* 96 */
	0x00000000, /* 97 */
	0x00000000, /* 98 */
	0x00000000, /* 99 */
	0x00000000, /* 100 */
	0x00000000, /* 101 */
	0x00000000, /* 102 */
	0x00000000, /* 103 */
	0x00000000, /* 104 */
	0x00000000, /* 105 */
	0x00000000, /* 106 */
	0x00000000, /* 107 */
	0x00000000, /* 108 */
	0x00000000, /* 109 */
	0x00000000, /* 110 */
	0x00000000, /* 111 */
	0x00000000, /* 112 */
	0x00000000, /* 113 */
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
	0x00000000, /* 126 */
	0x00000000, /* 127 */
	0x00000000, /* 128 */
	0x00000000, /* 129 */
	0x00000000, /* 130 */
	0x00000000, /* 131 */
	0x00000000, /* 132 */
	0x00000000, /* 133 */
	0x00000000, /* 134 */
	0x00000000, /* 135 */
	0x00000000, /* 136 */
	0x00000000, /* 137 */
	0x00000000, /* 138 */
	0x00000000, /* 141' */
	0x00000000, /* 142' */
	0x00000000, /* 143' */
	0x00000000, /* 144' */
	0x00000000, /* 145' */
	0x00000000, /* 146' */
	0x00000000, /* 147' */
	0x00000000, /* 148' */
	0x00000000, /* 149' */
	0x00000000, /* 150' */
	0x00000000, /* 151' */
	0x00000000, /* 152' */
	0x00000000, /* 153' */
	0x00000000, /* 154' */
	0x00000000, /* 155' */
	0x00000000, /* 156' */
	0x00000000, /* 157' */
	0x00000000, /* 158' */
	0x00000000, /* 159' */
	0x00000000, /* 160' */
	0x00000000, /* 161' */
	0x00000000, /* 162' */
	0x00000000, /* 163' */
	0x00000000, /* 164' */
	0x00000000, /* 165' */
	0x00000000, /* 166' */
	0x00000000, /* 167' */
	0x00000000, /* 168' */
	0x00000000, /* 169' */
	0x00000000, /* 170' */
	0x00000000, /* 171' */
	0x00000000, /* 172' */
	0x00000000, /* 173' */
	0x00000000, /* 174' */
	0x00000000, /* 175' */
	0x00000000, /* 176' */
	0x00000000, /* 177' */
	0x00000000, /* 178' */
	0x00000000, /* 179' */
	0x00000000, /* 180' */
	0x00000000, /* 181' */
	0x00000000, /* 182' */
	0x00000000, /* 183' */
	0x00000000, /* 184' */
	0x00000000, /* 185' */
	0x00000000, /* 186' */
	0x00000000, /* 187' */
	0x00000000, /* 188' */
	0x00000000, /* 189' */
	0x00000000, /* 190' */
	0x00000000, /* 191' */
	0x00000000, /* 192' */
	0x00000000, /* 193' */
	0x00000000, /* 194' */
	0x00000000, /* 195' */
	0x00000000, /* 196' */
	0x00000000, /* 197' */
	0x00000000, /* 198' */
	0x00000000, /* 199' */
	0x00000000, /* 200' */
	0x00000000, /* 201' */
	0x00000000, /* 202' */
	0x00000000, /* 203' */
	0x00000000, /* 204' */
	0x00000000, /* 205' */
	0x00000000, /* 206' */
	0x00000000, /* 207' */
	0x00000000, /* 208' */
	0x00000000, /* 209' */
	0x00000000, /* 210' */
	0x00000000, /* 211' */
	0x00000000, /* 212' */
	0x00000000, /* 213' */
	0x00000000, /* 214' */
	0x00000000, /* 215' */
	0x00000000, /* 216' */
	0x00000000, /* 217' */
	0x00000000, /* 218' */
	0x00000000, /* 219' */
	0x00000000, /* 220' */
	0x00000000, /* 221' */
	0x00000000, /* 222' */
	0x00000000, /* 223' */
	0x00000000, /* 224' */
	0x00000000, /* 225' */
	0x00000000, /* 226' */
	0x00000000, /* 227' */
	0x00000000, /* 228' */
	0x00000000, /* 229' */
	0x00000000, /* 230' */
	0x00000000, /* 231' */
	0x00000000, /* 232' */
	0x00000000, /* 233' */
	0x00000000, /* 234' */
	0x00000000, /* 235' */
	0x00000000, /* 236' */
	0x00000000, /* 237' */
	0x00000000, /* 238' */
	0x00000000, /* 239' */
	0x00000000, /* 240' */
	0x00000000, /* 241' */
	0x00000000, /* 242' */
	0x00000000, /* 243' */
	0x00000000, /* 244' */
	0x00000000, /* 245' */
	0x00000000, /* 246' */
	0x00000000, /* 247' */
	0x00000000, /* 248' */
	0x00000000, /* 249' */
	0x00000000, /* 250' */
	0x00000000, /* 249 */
	0x00000000, /* 250 */
	0x00000000, /* 251 */
	0x00000000, /* 252 */
	0x008fd000, /* 253 */
	0x00091211, /* 254 */
	0x00c5d8f7  /* 255 */
};

/* vec_vesa_1366x768_bvb_input.bss */
static const uint32_t s_aulRamBVBInput_CUSTOM_1366x768p[BVDC_P_RAM_TABLE_SIZE] =
{
	0x00a24000, /* 0 */
	0x00200018, /* 1 */
	0x00201018, /* 2 */
	0x00200011, /* 3 */
	0x0020853e, /* 4 */
	0x0024800b, /* 5 */
	0x00800000, /* 6 */
	0x00000000, /* 7 */
	0x00000000, /* 8 */
	0x00000000, /* 9 */
	0x00000000, /* 10 */
	0x00000000, /* 11 */
	0x00000000, /* 12 */
	0x00000000, /* 13 */
	0x00000000, /* 14 */
	0x00000000, /* 15 */
	0x00000000, /* 16 */
	0x00000000, /* 17 */
	0x00000000, /* 18 */
	0x00000000, /* 19 */
	0x00a40000, /* 20 */
	0x00a80000, /* 21 */
	0x00420001, /* 22 */
	0x0040000d, /* 23 */
	0x00408300, /* 24 */
	0x00815000, /* 25 */
	0x00000000, /* 26 */
	0x00000000, /* 27 */
	0x00000000, /* 28 */
	0x00000000, /* 29 */
	0x00000000, /* 30 */
	0x00000000, /* 31 */
	0x00000000, /* 32 */
	0x00000000, /* 33 */
	0x00000000, /* 34 */
	0x00000000, /* 35 */
	0x00000000, /* 36 */
	0x00000000, /* 37 */
	0x00000000, /* 38 */
	0x00000000, /* 39 */
	0x00000000, /* 40 */
	0x00000000, /* 41 */
	0x00000000, /* 42 */
	0x00000000, /* 43 */
	0x00000000, /* 44 */
	0x00000000, /* 45 */
	0x00000000, /* 46 */
	0x00000000, /* 47 */
	0x00000000, /* 48 */
	0x00000000, /* 49 */
	0x00000000, /* 50 */
	0x00000000, /* 51 */
	0x00000000, /* 52 */
	0x00000000, /* 53 */
	0x00000000, /* 54 */
	0x00000000, /* 55 */
	0x00000000, /* 56 */
	0x00000000, /* 57 */
	0x00000000, /* 58 */
	0x00000000, /* 59 */
	0x00000000, /* 60 */
	0x00000000, /* 61 */
	0x00000000, /* 62 */
	0x00000000, /* 63 */
	0x00000000, /* 64 */
	0x00000000, /* 65 */
	0x00000000, /* 66 */
	0x00000000, /* 67 */
	0x00000000, /* 68 */
	0x00000000, /* 69 */
	0x00000000, /* 70 */
	0x00000000, /* 71 */
	0x00000000, /* 72 */
	0x00000000, /* 73 */
	0x00000000, /* 74 */
	0x00000000, /* 75 */
	0x00000000, /* 76 */
	0x00000000, /* 77 */
	0x00000000, /* 78 */
	0x00000000, /* 79 */
	0x00000000, /* 80 */
	0x00000000, /* 81 */
	0x00000000, /* 82 */
	0x00000000, /* 83 */
	0x00000000, /* 84 */
	0x00000000, /* 85 */
	0x00000000, /* 86 */
	0x00000000, /* 87 */
	0x00000000, /* 88 */
	0x00000000, /* 89 */
	0x00000000, /* 90 */
	0x00000000, /* 91 */
	0x00000000, /* 92 */
	0x00000000, /* 93 */
	0x00000000, /* 94 */
	0x00000000, /* 95 */
	0x00000000, /* 96 */
	0x00000000, /* 97 */
	0x00000000, /* 98 */
	0x00000000, /* 99 */
	0x00000000, /* 100 */
	0x00000000, /* 101 */
	0x00000000, /* 102 */
	0x00000000, /* 103 */
	0x00000000, /* 104 */
	0x00000000, /* 105 */
	0x00000000, /* 106 */
	0x00000000, /* 107 */
	0x00000000, /* 108 */
	0x00000000, /* 109 */
	0x00000000, /* 110 */
	0x00000000, /* 111 */
	0x00000000, /* 112 */
	0x00000000, /* 113 */
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
	0x00000000, /* 126 */
	0x00000000, /* 127 */
	0x00000000, /* 128 */
	0x00000000, /* 129 */
	0x00000000, /* 130 */
	0x00000000, /* 131 */
	0x00000000, /* 132 */
	0x00000000, /* 133 */
	0x00000000, /* 134 */
	0x00000000, /* 135 */
	0x00000000, /* 136 */
	0x00000000, /* 137 */
	0x00000000, /* 138 */
	0x00000000, /* 141' */
	0x00000000, /* 142' */
	0x00000000, /* 143' */
	0x00000000, /* 144' */
	0x00000000, /* 145' */
	0x00000000, /* 146' */
	0x00000000, /* 147' */
	0x00000000, /* 148' */
	0x00000000, /* 149' */
	0x00000000, /* 150' */
	0x00000000, /* 151' */
	0x00000000, /* 152' */
	0x00000000, /* 153' */
	0x00000000, /* 154' */
	0x00000000, /* 155' */
	0x00000000, /* 156' */
	0x00000000, /* 157' */
	0x00000000, /* 158' */
	0x00000000, /* 159' */
	0x00000000, /* 160' */
	0x00000000, /* 161' */
	0x00000000, /* 162' */
	0x00000000, /* 163' */
	0x00000000, /* 164' */
	0x00000000, /* 165' */
	0x00000000, /* 166' */
	0x00000000, /* 167' */
	0x00000000, /* 168' */
	0x00000000, /* 169' */
	0x00000000, /* 170' */
	0x00000000, /* 171' */
	0x00000000, /* 172' */
	0x00000000, /* 173' */
	0x00000000, /* 174' */
	0x00000000, /* 175' */
	0x00000000, /* 176' */
	0x00000000, /* 177' */
	0x00000000, /* 178' */
	0x00000000, /* 179' */
	0x00000000, /* 180' */
	0x00000000, /* 181' */
	0x00000000, /* 182' */
	0x00000000, /* 183' */
	0x00000000, /* 184' */
	0x00000000, /* 185' */
	0x00000000, /* 186' */
	0x00000000, /* 187' */
	0x00000000, /* 188' */
	0x00000000, /* 189' */
	0x00000000, /* 190' */
	0x00000000, /* 191' */
	0x00000000, /* 192' */
	0x00000000, /* 193' */
	0x00000000, /* 194' */
	0x00000000, /* 195' */
	0x00000000, /* 196' */
	0x00000000, /* 197' */
	0x00000000, /* 198' */
	0x00000000, /* 199' */
	0x00000000, /* 200' */
	0x00000000, /* 201' */
	0x00000000, /* 202' */
	0x00000000, /* 203' */
	0x00000000, /* 204' */
	0x00000000, /* 205' */
	0x00000000, /* 206' */
	0x00000000, /* 207' */
	0x00000000, /* 208' */
	0x00000000, /* 209' */
	0x00000000, /* 210' */
	0x00000000, /* 211' */
	0x00000000, /* 212' */
	0x00000000, /* 213' */
	0x00000000, /* 214' */
	0x00000000, /* 215' */
	0x00000000, /* 216' */
	0x00000000, /* 217' */
	0x00000000, /* 218' */
	0x00000000, /* 219' */
	0x00000000, /* 220' */
	0x00000000, /* 221' */
	0x00000000, /* 222' */
	0x00000000, /* 223' */
	0x00000000, /* 224' */
	0x00000000, /* 225' */
	0x00000000, /* 226' */
	0x00000000, /* 227' */
	0x00000000, /* 228' */
	0x00000000, /* 229' */
	0x00000000, /* 230' */
	0x00000000, /* 231' */
	0x00000000, /* 232' */
	0x00000000, /* 233' */
	0x00000000, /* 234' */
	0x00000000, /* 235' */
	0x00000000, /* 236' */
	0x00000000, /* 237' */
	0x00000000, /* 238' */
	0x00000000, /* 239' */
	0x00000000, /* 240' */
	0x00000000, /* 241' */
	0x00000000, /* 242' */
	0x00000000, /* 243' */
	0x00000000, /* 244' */
	0x00000000, /* 245' */
	0x00000000, /* 246' */
	0x00000000, /* 247' */
	0x00000000, /* 248' */
	0x00000000, /* 249' */
	0x00000000, /* 250' */
	0x00000000, /* 249 */
	0x00000000, /* 250 */
	0x00000000, /* 251 */
	0x00000000, /* 252 */
	0x00000000, /* 253 */
	0x00000000, /* 254 */
	0x444b66d7  /* 255 */
};

/* IT_Microcode for 640x480p DVI safe mode */
static const uint32_t s_aulRamBVBInput_DVI_640x480p[BVDC_P_RAM_TABLE_SIZE] =
{
	0x00a24000, /* 0 */
	0x00240010, /* 1 */
	0x00201060, /* 2 */
	0x00200027, /* 3 */
	0x00208273, /* 4 */
	0x00800000, /* 5 */
	0x00000000, /* 6 */
	0x00000000, /* 7 */
	0x00000000, /* 8 */
	0x00000000, /* 9 */
	0x00000000, /* 10 */
	0x00000000, /* 11 */
	0x00000000, /* 12 */
	0x00000000, /* 13 */
	0x00000000, /* 14 */
	0x00000000, /* 15 */
	0x00000000, /* 16 */
	0x00000000, /* 17 */
	0x00000000, /* 18 */
	0x00000000, /* 19 */
	0x00a80000, /* 20 */
	0x00421001, /* 21 */
	0x00401001, /* 22 */
	0x00400021, /* 23 */
	0x004081e0, /* 24 */
	0x00a80000, /* 25 */
	0x0040000a, /* 26 */
	0x00815000, /* 27 */
	0x00000000, /* 28' */
	0x00000000, /* 29' */
	0x00000000, /* 30' */
	0x00000000, /* 31' */
	0x00000000, /* 34 */
	0x00000000, /* 35 */
	0x00000000, /* 36 */
	0x00000000, /* 37 */
	0x00000000, /* 38 */
	0x00000000, /* 39 */
	0x00000000, /* 40 */
	0x00000000, /* 41 */
	0x00000000, /* 42 */
	0x00000000, /* 43 */
	0x00000000, /* 44 */
	0x00000000, /* 45 */
	0x00000000, /* 46 */
	0x00000000, /* 47 */
	0x00000000, /* 48 */
	0x00000000, /* 49 */
	0x00000000, /* 50 */
	0x00000000, /* 51 */
	0x00000000, /* 52 */
	0x00000000, /* 53 */
	0x00000000, /* 54 */
	0x00000000, /* 55 */
	0x00000000, /* 56 */
	0x00000000, /* 57 */
	0x00000000, /* 58 */
	0x00000000, /* 59 */
	0x00000000, /* 60 */
	0x00000000, /* 61 */
	0x00000000, /* 62 */
	0x00000000, /* 63 */
	0x00000000, /* 64 */
	0x00000000, /* 65 */
	0x00000000, /* 66 */
	0x00000000, /* 67 */
	0x00000000, /* 68 */
	0x00000000, /* 69 */
	0x00000000, /* 70 */
	0x00000000, /* 71 */
	0x00000000, /* 72 */
	0x00000000, /* 73 */
	0x00000000, /* 74 */
	0x00000000, /* 75 */
	0x00000000, /* 76 */
	0x00000000, /* 77 */
	0x00000000, /* 78 */
	0x00000000, /* 79 */
	0x00000000, /* 80 */
	0x00000000, /* 81 */
	0x00000000, /* 82 */
	0x00000000, /* 83 */
	0x00000000, /* 84 */
	0x00000000, /* 85 */
	0x00000000, /* 86 */
	0x00000000, /* 87 */
	0x00000000, /* 88 */
	0x00000000, /* 89 */
	0x00000000, /* 90 */
	0x00000000, /* 91 */
	0x00000000, /* 92 */
	0x00000000, /* 93 */
	0x00000000, /* 94 */
	0x00000000, /* 95 */
	0x00000000, /* 96 */
	0x00000000, /* 97 */
	0x00000000, /* 98 */
	0x00000000, /* 99 */
	0x00000000, /* 100 */
	0x00000000, /* 101 */
	0x00000000, /* 102 */
	0x00000000, /* 103 */
	0x00000000, /* 104 */
	0x00000000, /* 105 */
	0x00000000, /* 106 */
	0x00000000, /* 107 */
	0x00000000, /* 108 */
	0x00000000, /* 109 */
	0x00000000, /* 110 */
	0x00000000, /* 111 */
	0x00000000, /* 112 */
	0x00000000, /* 113 */
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
	0x00000000, /* 126 */
	0x00000000, /* 127 */
	0x00000000, /* 128 */
	0x00000000, /* 129 */
	0x00000000, /* 130 */
	0x00000000, /* 131 */
	0x00000000, /* 132 */
	0x00000000, /* 133 */
	0x00000000, /* 134 */
	0x00000000, /* 135 */
	0x00000000, /* 136 */
	0x00000000, /* 137 */
	0x00000000, /* 138 */
	0x00000000, /* 139 */
	0x00000000, /* 140 */
	0x00000000, /* 141 */
	0x00000000, /* 142 */
	0x00000000, /* 143 */
	0x00000000, /* 144 */
	0x00000000, /* 145 */
	0x00000000, /* 146 */
	0x00000000, /* 147 */
	0x00000000, /* 148 */
	0x00000000, /* 149 */
	0x00000000, /* 150 */
	0x00000000, /* 151 */
	0x00000000, /* 152 */
	0x00000000, /* 153 */
	0x00000000, /* 154 */
	0x00000000, /* 155 */
	0x00000000, /* 156 */
	0x00000000, /* 157 */
	0x00000000, /* 158 */
	0x00000000, /* 159 */
	0x00000000, /* 160 */
	0x00000000, /* 161 */
	0x00000000, /* 162 */
	0x00000000, /* 163 */
	0x00000000, /* 164 */
	0x00000000, /* 165 */
	0x00000000, /* 166 */
	0x00000000, /* 167 */
	0x00000000, /* 168 */
	0x00000000, /* 169 */
	0x00000000, /* 170 */
	0x00000000, /* 171 */
	0x00000000, /* 172 */
	0x00000000, /* 173 */
	0x00000000, /* 174 */
	0x00000000, /* 175 */
	0x00000000, /* 176 */
	0x00000000, /* 177 */
	0x00000000, /* 178 */
	0x00000000, /* 179 */
	0x00000000, /* 180 */
	0x00000000, /* 181 */
	0x00000000, /* 182 */
	0x00000000, /* 183 */
	0x00000000, /* 184 */
	0x00000000, /* 185 */
	0x00000000, /* 186 */
	0x00000000, /* 187 */
	0x00000000, /* 188 */
	0x00000000, /* 189 */
	0x00000000, /* 190 */
	0x00000000, /* 191 */
	0x00000000, /* 192 */
	0x00000000, /* 193 */
	0x00000000, /* 194 */
	0x00000000, /* 195 */
	0x00000000, /* 196 */
	0x00000000, /* 197 */
	0x00000000, /* 198 */
	0x00000000, /* 199 */
	0x00000000, /* 200 */
	0x00000000, /* 201 */
	0x00000000, /* 202 */
	0x00000000, /* 203 */
	0x00000000, /* 204 */
	0x00000000, /* 205 */
	0x00000000, /* 206 */
	0x00000000, /* 207 */
	0x00000000, /* 208 */
	0x00000000, /* 209 */
	0x00000000, /* 210 */
	0x00000000, /* 211 */
	0x00000000, /* 212 */
	0x00000000, /* 213 */
	0x00000000, /* 214 */
	0x00000000, /* 215 */
	0x00000000, /* 216 */
	0x00000000, /* 217 */
	0x00000000, /* 218 */
	0x00000000, /* 219 */
	0x00000000, /* 220 */
	0x00000000, /* 221 */
	0x00000000, /* 222 */
	0x00000000, /* 223 */
	0x00000000, /* 224 */
	0x00000000, /* 225 */
	0x00000000, /* 226 */
	0x00000000, /* 227 */
	0x00000000, /* 228 */
	0x00000000, /* 229 */
	0x00000000, /* 230 */
	0x00000000, /* 231 */
	0x00000000, /* 232 */
	0x00000000, /* 233 */
	0x00000000, /* 234 */
	0x00000000, /* 235 */
	0x00000000, /* 236 */
	0x00000000, /* 237 */
	0x00000000, /* 238 */
	0x00000000, /* 239 */
	0x00000000, /* 240 */
	0x00000000, /* 241 */
	0x00000000, /* 242 */
	0x00000000, /* 243 */
	0x00000000, /* 244 */
	0x00000000, /* 245 */
	0x00000000, /* 246 */
	0x00000000, /* 247 */
	0x00000000, /* 248 */
	0x00000000, /* 249 */
	0x00000000, /* 250 */
	0x00000000, /* 249' */
	0x00000000, /* 250' */
	0x00000000, /* 251' */
	0x00000000, /* 252' */
	0x008fd000, /* 253 */
	0x00090109, /* 254 */
	0x00c5bfde, /* 255 */
};

/* IT_Microcode for 640x480p_CVT DVI mode */
static const uint32_t s_aulRamBVBInput_DVI_640x480p_CVT[BVDC_P_RAM_TABLE_SIZE] =
{
	0x00a24000, /* 0 */
	0x00200010, /* 1 */
	0x00201040, /* 2 */
	0x00200047, /* 3 */
	0x00208268, /* 4 */
	0x0024800b, /* 5 */
	0x00800000, /* 6 */
	0x00000000, /* 7 */
	0x00000000, /* 8 */
	0x00000000, /* 9 */
	0x00000000, /* 10 */
	0x00000000, /* 11 */
	0x00000000, /* 12 */
	0x00000000, /* 13 */
	0x00000000, /* 14 */
	0x00000000, /* 15 */
	0x00000000, /* 16 */
	0x00000000, /* 17 */
	0x00000000, /* 18 */
	0x00000000, /* 19 */
	0x00a80000, /* 20 */
	0x00421001, /* 21 */
	0x00401003, /* 22 */
	0x0040000d, /* 23 */
	0x004081e0, /* 24 */
	0x00400003, /* 25 */
	0x00814000, /* 26 */
	0x00000000, /* 27 */
	0x00000000, /* 28 */
	0x00000000, /* 29 */
	0x00000000, /* 30 */
	0x00000000, /* 31 */
	0x00000000, /* 32 */
	0x00000000, /* 34' */
	0x00000000, /* 35' */
	0x00000000, /* 36' */
	0x00000000, /* 37' */
	0x00000000, /* 38' */
	0x00000000, /* 39' */
	0x00000000, /* 40' */
	0x00000000, /* 41' */
	0x00000000, /* 42' */
	0x00000000, /* 43' */
	0x00000000, /* 44' */
	0x00000000, /* 45' */
	0x00000000, /* 46' */
	0x00000000, /* 47' */
	0x00000000, /* 48' */
	0x00000000, /* 49' */
	0x00000000, /* 50' */
	0x00000000, /* 51' */
	0x00000000, /* 52' */
	0x00000000, /* 53' */
	0x00000000, /* 54' */
	0x00000000, /* 55' */
	0x00000000, /* 56' */
	0x00000000, /* 57' */
	0x00000000, /* 58' */
	0x00000000, /* 59' */
	0x00000000, /* 60' */
	0x00000000, /* 61' */
	0x00000000, /* 62' */
	0x00000000, /* 63' */
	0x00000000, /* 64' */
	0x00000000, /* 65' */
	0x00000000, /* 66' */
	0x00000000, /* 67' */
	0x00000000, /* 68' */
	0x00000000, /* 69' */
	0x00000000, /* 70' */
	0x00000000, /* 71' */
	0x00000000, /* 72' */
	0x00000000, /* 73' */
	0x00000000, /* 74' */
	0x00000000, /* 75' */
	0x00000000, /* 76' */
	0x00000000, /* 77' */
	0x00000000, /* 78' */
	0x00000000, /* 79' */
	0x00000000, /* 80' */
	0x00000000, /* 81' */
	0x00000000, /* 82' */
	0x00000000, /* 83' */
	0x00000000, /* 84' */
	0x00000000, /* 85' */
	0x00000000, /* 86' */
	0x00000000, /* 87' */
	0x00000000, /* 88' */
	0x00000000, /* 89' */
	0x00000000, /* 90' */
	0x00000000, /* 91' */
	0x00000000, /* 92' */
	0x00000000, /* 93' */
	0x00000000, /* 94' */
	0x00000000, /* 95' */
	0x00000000, /* 96' */
	0x00000000, /* 97' */
	0x00000000, /* 98' */
	0x00000000, /* 99' */
	0x00000000, /* 100' */
	0x00000000, /* 101' */
	0x00000000, /* 102' */
	0x00000000, /* 103' */
	0x00000000, /* 104' */
	0x00000000, /* 105' */
	0x00000000, /* 106' */
	0x00000000, /* 107' */
	0x00000000, /* 108' */
	0x00000000, /* 109' */
	0x00000000, /* 110' */
	0x00000000, /* 111' */
	0x00000000, /* 112' */
	0x00000000, /* 113' */
	0x00000000, /* 114' */
	0x00000000, /* 115' */
	0x00000000, /* 116' */
	0x00000000, /* 117' */
	0x00000000, /* 118' */
	0x00000000, /* 119' */
	0x00000000, /* 120' */
	0x00000000, /* 121' */
	0x00000000, /* 122' */
	0x00000000, /* 123' */
	0x00000000, /* 124' */
	0x00000000, /* 125' */
	0x00000000, /* 126' */
	0x00000000, /* 127' */
	0x00000000, /* 128' */
	0x00000000, /* 129' */
	0x00000000, /* 130' */
	0x00000000, /* 131' */
	0x00000000, /* 132' */
	0x00000000, /* 133' */
	0x00000000, /* 134' */
	0x00000000, /* 135' */
	0x00000000, /* 136' */
	0x00000000, /* 137' */
	0x00000000, /* 138' */
	0x00000000, /* 139' */
	0x00000000, /* 140' */
	0x00000000, /* 141' */
	0x00000000, /* 142' */
	0x00000000, /* 143' */
	0x00000000, /* 144' */
	0x00000000, /* 145' */
	0x00000000, /* 146' */
	0x00000000, /* 147' */
	0x00000000, /* 148' */
	0x00000000, /* 149' */
	0x00000000, /* 150' */
	0x00000000, /* 151' */
	0x00000000, /* 152' */
	0x00000000, /* 153' */
	0x00000000, /* 154' */
	0x00000000, /* 155' */
	0x00000000, /* 156' */
	0x00000000, /* 157' */
	0x00000000, /* 158' */
	0x00000000, /* 159' */
	0x00000000, /* 160' */
	0x00000000, /* 161' */
	0x00000000, /* 162' */
	0x00000000, /* 163' */
	0x00000000, /* 164' */
	0x00000000, /* 165' */
	0x00000000, /* 166' */
	0x00000000, /* 167' */
	0x00000000, /* 168' */
	0x00000000, /* 169' */
	0x00000000, /* 170' */
	0x00000000, /* 171' */
	0x00000000, /* 172' */
	0x00000000, /* 173' */
	0x00000000, /* 174' */
	0x00000000, /* 175' */
	0x00000000, /* 176' */
	0x00000000, /* 177' */
	0x00000000, /* 178' */
	0x00000000, /* 179' */
	0x00000000, /* 180' */
	0x00000000, /* 181' */
	0x00000000, /* 182' */
	0x00000000, /* 183' */
	0x00000000, /* 184' */
	0x00000000, /* 185' */
	0x00000000, /* 186' */
	0x00000000, /* 187' */
	0x00000000, /* 188' */
	0x00000000, /* 189' */
	0x00000000, /* 190' */
	0x00000000, /* 191' */
	0x00000000, /* 192' */
	0x00000000, /* 193' */
	0x00000000, /* 194' */
	0x00000000, /* 195' */
	0x00000000, /* 196' */
	0x00000000, /* 197' */
	0x00000000, /* 198' */
	0x00000000, /* 199' */
	0x00000000, /* 200' */
	0x00000000, /* 201' */
	0x00000000, /* 202' */
	0x00000000, /* 203' */
	0x00000000, /* 204' */
	0x00000000, /* 205' */
	0x00000000, /* 206' */
	0x00000000, /* 207' */
	0x00000000, /* 208' */
	0x00000000, /* 209' */
	0x00000000, /* 210' */
	0x00000000, /* 211' */
	0x00000000, /* 212' */
	0x00000000, /* 213' */
	0x00000000, /* 214' */
	0x00000000, /* 215' */
	0x00000000, /* 216' */
	0x00000000, /* 217' */
	0x00000000, /* 218' */
	0x00000000, /* 219' */
	0x00000000, /* 220' */
	0x00000000, /* 221' */
	0x00000000, /* 222' */
	0x00000000, /* 223' */
	0x00000000, /* 224' */
	0x00000000, /* 225' */
	0x00000000, /* 226' */
	0x00000000, /* 227' */
	0x00000000, /* 228' */
	0x00000000, /* 229' */
	0x00000000, /* 230' */
	0x00000000, /* 231' */
	0x00000000, /* 232' */
	0x00000000, /* 233' */
	0x00000000, /* 234' */
	0x00000000, /* 235' */
	0x00000000, /* 236' */
	0x00000000, /* 237' */
	0x00000000, /* 238' */
	0x00000000, /* 239' */
	0x00000000, /* 240' */
	0x00000000, /* 241' */
	0x00000000, /* 242' */
	0x00000000, /* 243' */
	0x00000000, /* 244' */
	0x00000000, /* 245' */
	0x00000000, /* 246' */
	0x00000000, /* 246 */
	0x00000000, /* 247 */
	0x00000000, /* 248 */
	0x00000000, /* 249 */
	0x00000000, /* 250 */
	0x00000000, /* 251 */
	0x00000000, /* 252 */
	0x008fd000, /* 253 */
	0x00070009, /* 254 */
	0x003e2fc5, /* 255 */
};


/* IT_Microcode for 800x600p DVI mode */
static const uint32_t s_aulRamBVBInput_DVI_800x600p[BVDC_P_RAM_TABLE_SIZE] =
{
	0x00a24000, /* 0 */
	0x00240028, /* 1 */
	0x00201080, /* 2 */
	0x0020004f, /* 3 */
	0x00208313, /* 4 */
	0x00800000, /* 5 */
	0x00000000, /* 6 */
	0x00000000, /* 7 */
	0x00000000, /* 8 */
	0x00000000, /* 9 */
	0x00000000, /* 10 */
	0x00000000, /* 11 */
	0x00000000, /* 12 */
	0x00000000, /* 13 */
	0x00000000, /* 14 */
	0x00000000, /* 15 */
	0x00000000, /* 16 */
	0x00000000, /* 17 */
	0x00000000, /* 18 */
	0x00000000, /* 19 */
	0x00a80000, /* 20 */
	0x00421001, /* 21 */
	0x00401003, /* 22 */
	0x00400017, /* 23 */
	0x00408258, /* 24 */
	0x00a80000, /* 25 */
	0x00400001, /* 26 */
	0x00815000, /* 27 */
	0x00000000, /* 28' */
	0x00000000, /* 29' */
	0x00000000, /* 30' */
	0x00000000, /* 31' */
	0x00000000, /* 32' */
	0x00000000, /* 33' */
	0x00000000, /* 34' */
	0x00000000, /* 35' */
	0x00000000, /* 36' */
	0x00000000, /* 37' */
	0x00000000, /* 38' */
	0x00000000, /* 39' */
	0x00000000, /* 40' */
	0x00000000, /* 41' */
	0x00000000, /* 42' */
	0x00000000, /* 43' */
	0x00000000, /* 44' */
	0x00000000, /* 45' */
	0x00000000, /* 46' */
	0x00000000, /* 47' */
	0x00000000, /* 48' */
	0x00000000, /* 49' */
	0x00000000, /* 50' */
	0x00000000, /* 51' */
	0x00000000, /* 52' */
	0x00000000, /* 53' */
	0x00000000, /* 54' */
	0x00000000, /* 55' */
	0x00000000, /* 56' */
	0x00000000, /* 57' */
	0x00000000, /* 58' */
	0x00000000, /* 59' */
	0x00000000, /* 60' */
	0x00000000, /* 61' */
	0x00000000, /* 62' */
	0x00000000, /* 63' */
	0x00000000, /* 64' */
	0x00000000, /* 65' */
	0x00000000, /* 66' */
	0x00000000, /* 67' */
	0x00000000, /* 68' */
	0x00000000, /* 69' */
	0x00000000, /* 70' */
	0x00000000, /* 71' */
	0x00000000, /* 72' */
	0x00000000, /* 73' */
	0x00000000, /* 74' */
	0x00000000, /* 75' */
	0x00000000, /* 76' */
	0x00000000, /* 77' */
	0x00000000, /* 78' */
	0x00000000, /* 79' */
	0x00000000, /* 80' */
	0x00000000, /* 81' */
	0x00000000, /* 82' */
	0x00000000, /* 83' */
	0x00000000, /* 84' */
	0x00000000, /* 85' */
	0x00000000, /* 86' */
	0x00000000, /* 87' */
	0x00000000, /* 88' */
	0x00000000, /* 89' */
	0x00000000, /* 90' */
	0x00000000, /* 91' */
	0x00000000, /* 92' */
	0x00000000, /* 93' */
	0x00000000, /* 94' */
	0x00000000, /* 95' */
	0x00000000, /* 96' */
	0x00000000, /* 97' */
	0x00000000, /* 98' */
	0x00000000, /* 99' */
	0x00000000, /* 100' */
	0x00000000, /* 101' */
	0x00000000, /* 102' */
	0x00000000, /* 103' */
	0x00000000, /* 104' */
	0x00000000, /* 105' */
	0x00000000, /* 106' */
	0x00000000, /* 107' */
	0x00000000, /* 108' */
	0x00000000, /* 109' */
	0x00000000, /* 110' */
	0x00000000, /* 111' */
	0x00000000, /* 112' */
	0x00000000, /* 113' */
	0x00000000, /* 114' */
	0x00000000, /* 115' */
	0x00000000, /* 116' */
	0x00000000, /* 117' */
	0x00000000, /* 118' */
	0x00000000, /* 119' */
	0x00000000, /* 120' */
	0x00000000, /* 121' */
	0x00000000, /* 122' */
	0x00000000, /* 123' */
	0x00000000, /* 124' */
	0x00000000, /* 125' */
	0x00000000, /* 126' */
	0x00000000, /* 127' */
	0x00000000, /* 128' */
	0x00000000, /* 129' */
	0x00000000, /* 130' */
	0x00000000, /* 131' */
	0x00000000, /* 132' */
	0x00000000, /* 133' */
	0x00000000, /* 134' */
	0x00000000, /* 135' */
	0x00000000, /* 136' */
	0x00000000, /* 137' */
	0x00000000, /* 138' */
	0x00000000, /* 139' */
	0x00000000, /* 140' */
	0x00000000, /* 141' */
	0x00000000, /* 142' */
	0x00000000, /* 143' */
	0x00000000, /* 144' */
	0x00000000, /* 145' */
	0x00000000, /* 146' */
	0x00000000, /* 147' */
	0x00000000, /* 148' */
	0x00000000, /* 149' */
	0x00000000, /* 150' */
	0x00000000, /* 151' */
	0x00000000, /* 152' */
	0x00000000, /* 153' */
	0x00000000, /* 154' */
	0x00000000, /* 155' */
	0x00000000, /* 156' */
	0x00000000, /* 157' */
	0x00000000, /* 158' */
	0x00000000, /* 159' */
	0x00000000, /* 160' */
	0x00000000, /* 161' */
	0x00000000, /* 162' */
	0x00000000, /* 163' */
	0x00000000, /* 164' */
	0x00000000, /* 165' */
	0x00000000, /* 166' */
	0x00000000, /* 167' */
	0x00000000, /* 168' */
	0x00000000, /* 169' */
	0x00000000, /* 170' */
	0x00000000, /* 171' */
	0x00000000, /* 172' */
	0x00000000, /* 173' */
	0x00000000, /* 174' */
	0x00000000, /* 175' */
	0x00000000, /* 176' */
	0x00000000, /* 177' */
	0x00000000, /* 178' */
	0x00000000, /* 179' */
	0x00000000, /* 180' */
	0x00000000, /* 181' */
	0x00000000, /* 182' */
	0x00000000, /* 183' */
	0x00000000, /* 184' */
	0x00000000, /* 185' */
	0x00000000, /* 186' */
	0x00000000, /* 187' */
	0x00000000, /* 188' */
	0x00000000, /* 189' */
	0x00000000, /* 190' */
	0x00000000, /* 191' */
	0x00000000, /* 192' */
	0x00000000, /* 193' */
	0x00000000, /* 194' */
	0x00000000, /* 195' */
	0x00000000, /* 196' */
	0x00000000, /* 197' */
	0x00000000, /* 198' */
	0x00000000, /* 199' */
	0x00000000, /* 200' */
	0x00000000, /* 201' */
	0x00000000, /* 202' */
	0x00000000, /* 203' */
	0x00000000, /* 204' */
	0x00000000, /* 205' */
	0x00000000, /* 206' */
	0x00000000, /* 207' */
	0x00000000, /* 208' */
	0x00000000, /* 209' */
	0x00000000, /* 210' */
	0x00000000, /* 211' */
	0x00000000, /* 212' */
	0x00000000, /* 213' */
	0x00000000, /* 214' */
	0x00000000, /* 215' */
	0x00000000, /* 216' */
	0x00000000, /* 217' */
	0x00000000, /* 218' */
	0x00000000, /* 219' */
	0x00000000, /* 220' */
	0x00000000, /* 221' */
	0x00000000, /* 222' */
	0x00000000, /* 223' */
	0x00000000, /* 224' */
	0x00000000, /* 225' */
	0x00000000, /* 226' */
	0x00000000, /* 227' */
	0x00000000, /* 228' */
	0x00000000, /* 229' */
	0x00000000, /* 230' */
	0x00000000, /* 231' */
	0x00000000, /* 232' */
	0x00000000, /* 233' */
	0x00000000, /* 234' */
	0x00000000, /* 235' */
	0x00000000, /* 236' */
	0x00000000, /* 237' */
	0x00000000, /* 238' */
	0x00000000, /* 239' */
	0x00000000, /* 240' */
	0x00000000, /* 241' */
	0x00000000, /* 242' */
	0x00000000, /* 243' */
	0x00000000, /* 244' */
	0x00000000, /* 245' */
	0x00000000, /* 246' */
	0x00000000, /* 247' */
	0x00000000, /* 248' */
	0x00000000, /* 249' */
	0x00000000, /* 250' */
	0x00000000, /* 251' */
	0x00000000, /* 252' */
	0x008fd000, /* 253 */
	0x00090109, /* 254 */
	0x00c5c145, /* 255 */
};

/* IT_Microcode for 1024x768p DVI mode */
static const uint32_t s_aulRamBVBInput_DVI_1024x768p[BVDC_P_RAM_TABLE_SIZE] =
{
	0x00a24000, /* 0 */
	0x00240018, /* 1 */
	0x00201088, /* 2 */
	0x00200097, /* 3 */
	0x002083f3, /* 4 */
	0x00800000, /* 5 */
	0x00000000, /* 6 */
	0x00000000, /* 7 */
	0x00000000, /* 8 */
	0x00000000, /* 9 */
	0x00000000, /* 10 */
	0x00000000, /* 12' */
	0x00000000, /* 13' */
	0x00000000, /* 13 */
	0x00000000, /* 14 */
	0x00000000, /* 15 */
	0x00000000, /* 16 */
	0x00000000, /* 17 */
	0x00000000, /* 18 */
	0x00000000, /* 19 */
	0x00a80000, /* 20 */
	0x00421001, /* 21 */
	0x00401005, /* 22 */
	0x0040001d, /* 23 */
	0x00408300, /* 24 */
	0x00a80000, /* 25 */
	0x00400003, /* 26 */
	0x00815000, /* 27 */
	0x00000000, /* 28' */
	0x00000000, /* 29' */
	0x00000000, /* 30' */
	0x00000000, /* 31' */
	0x00000000, /* 32' */
	0x00000000, /* 31 */
	0x00000000, /* 32 */
	0x00000000, /* 33 */
	0x00000000, /* 34 */
	0x00000000, /* 35 */
	0x00000000, /* 36 */
	0x00000000, /* 37 */
	0x00000000, /* 38 */
	0x00000000, /* 39 */
	0x00000000, /* 40 */
	0x00000000, /* 41 */
	0x00000000, /* 42 */
	0x00000000, /* 43 */
	0x00000000, /* 44 */
	0x00000000, /* 45 */
	0x00000000, /* 46 */
	0x00000000, /* 47 */
	0x00000000, /* 48 */
	0x00000000, /* 49 */
	0x00000000, /* 50 */
	0x00000000, /* 51 */
	0x00000000, /* 52 */
	0x00000000, /* 53 */
	0x00000000, /* 54 */
	0x00000000, /* 55 */
	0x00000000, /* 56 */
	0x00000000, /* 57 */
	0x00000000, /* 58 */
	0x00000000, /* 59 */
	0x00000000, /* 60 */
	0x00000000, /* 61 */
	0x00000000, /* 62 */
	0x00000000, /* 63 */
	0x00000000, /* 64 */
	0x00000000, /* 65 */
	0x00000000, /* 66 */
	0x00000000, /* 67 */
	0x00000000, /* 68 */
	0x00000000, /* 69 */
	0x00000000, /* 70 */
	0x00000000, /* 71 */
	0x00000000, /* 72 */
	0x00000000, /* 73 */
	0x00000000, /* 74 */
	0x00000000, /* 75 */
	0x00000000, /* 76 */
	0x00000000, /* 77 */
	0x00000000, /* 78 */
	0x00000000, /* 79 */
	0x00000000, /* 80 */
	0x00000000, /* 81 */
	0x00000000, /* 82 */
	0x00000000, /* 83 */
	0x00000000, /* 84 */
	0x00000000, /* 85 */
	0x00000000, /* 86 */
	0x00000000, /* 87 */
	0x00000000, /* 88 */
	0x00000000, /* 89 */
	0x00000000, /* 90 */
	0x00000000, /* 91 */
	0x00000000, /* 92 */
	0x00000000, /* 93 */
	0x00000000, /* 94 */
	0x00000000, /* 95 */
	0x00000000, /* 96 */
	0x00000000, /* 97 */
	0x00000000, /* 98 */
	0x00000000, /* 99 */
	0x00000000, /* 100 */
	0x00000000, /* 101 */
	0x00000000, /* 102 */
	0x00000000, /* 103 */
	0x00000000, /* 104 */
	0x00000000, /* 105 */
	0x00000000, /* 106 */
	0x00000000, /* 107 */
	0x00000000, /* 108 */
	0x00000000, /* 109 */
	0x00000000, /* 110 */
	0x00000000, /* 111 */
	0x00000000, /* 112 */
	0x00000000, /* 113 */
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
	0x00000000, /* 126 */
	0x00000000, /* 127 */
	0x00000000, /* 128 */
	0x00000000, /* 129 */
	0x00000000, /* 130 */
	0x00000000, /* 131 */
	0x00000000, /* 132 */
	0x00000000, /* 133 */
	0x00000000, /* 134 */
	0x00000000, /* 135 */
	0x00000000, /* 136 */
	0x00000000, /* 137 */
	0x00000000, /* 138 */
	0x00000000, /* 139 */
	0x00000000, /* 140 */
	0x00000000, /* 141 */
	0x00000000, /* 142 */
	0x00000000, /* 143 */
	0x00000000, /* 144 */
	0x00000000, /* 145 */
	0x00000000, /* 146 */
	0x00000000, /* 147 */
	0x00000000, /* 148 */
	0x00000000, /* 149 */
	0x00000000, /* 150 */
	0x00000000, /* 151 */
	0x00000000, /* 152 */
	0x00000000, /* 153 */
	0x00000000, /* 154 */
	0x00000000, /* 155 */
	0x00000000, /* 156 */
	0x00000000, /* 157 */
	0x00000000, /* 158 */
	0x00000000, /* 159 */
	0x00000000, /* 160 */
	0x00000000, /* 161 */
	0x00000000, /* 162 */
	0x00000000, /* 163 */
	0x00000000, /* 164 */
	0x00000000, /* 165 */
	0x00000000, /* 166 */
	0x00000000, /* 167 */
	0x00000000, /* 168 */
	0x00000000, /* 169 */
	0x00000000, /* 170 */
	0x00000000, /* 171 */
	0x00000000, /* 172 */
	0x00000000, /* 173 */
	0x00000000, /* 174 */
	0x00000000, /* 175 */
	0x00000000, /* 176 */
	0x00000000, /* 177 */
	0x00000000, /* 178 */
	0x00000000, /* 179 */
	0x00000000, /* 180 */
	0x00000000, /* 181 */
	0x00000000, /* 182 */
	0x00000000, /* 183 */
	0x00000000, /* 184 */
	0x00000000, /* 185 */
	0x00000000, /* 186 */
	0x00000000, /* 187 */
	0x00000000, /* 188 */
	0x00000000, /* 189 */
	0x00000000, /* 190 */
	0x00000000, /* 191 */
	0x00000000, /* 192 */
	0x00000000, /* 193 */
	0x00000000, /* 194 */
	0x00000000, /* 195 */
	0x00000000, /* 196 */
	0x00000000, /* 197 */
	0x00000000, /* 198 */
	0x00000000, /* 199 */
	0x00000000, /* 200 */
	0x00000000, /* 201 */
	0x00000000, /* 202 */
	0x00000000, /* 203 */
	0x00000000, /* 204 */
	0x00000000, /* 205 */
	0x00000000, /* 206 */
	0x00000000, /* 207 */
	0x00000000, /* 208 */
	0x00000000, /* 209 */
	0x00000000, /* 210 */
	0x00000000, /* 211 */
	0x00000000, /* 212 */
	0x00000000, /* 213 */
	0x00000000, /* 214 */
	0x00000000, /* 215 */
	0x00000000, /* 216 */
	0x00000000, /* 217 */
	0x00000000, /* 218 */
	0x00000000, /* 219 */
	0x00000000, /* 220 */
	0x00000000, /* 221 */
	0x00000000, /* 222 */
	0x00000000, /* 223 */
	0x00000000, /* 224 */
	0x00000000, /* 225 */
	0x00000000, /* 226 */
	0x00000000, /* 227 */
	0x00000000, /* 228 */
	0x00000000, /* 229 */
	0x00000000, /* 230 */
	0x00000000, /* 231 */
	0x00000000, /* 232 */
	0x00000000, /* 233 */
	0x00000000, /* 234 */
	0x00000000, /* 235 */
	0x00000000, /* 236 */
	0x00000000, /* 237 */
	0x00000000, /* 238 */
	0x00000000, /* 239 */
	0x00000000, /* 240 */
	0x00000000, /* 241 */
	0x00000000, /* 242 */
	0x00000000, /* 243 */
	0x00000000, /* 244 */
	0x00000000, /* 247' */
	0x00000000, /* 248' */
	0x00000000, /* 249' */
	0x00000000, /* 250' */
	0x00000000, /* 251' */
	0x00000000, /* 252' */
	0x008fd000, /* 253 */
	0x00090109, /* 254 */
	0x00c5c317, /* 255 */
};

/* IT_Microcode for 1280x768p DVI */
static const uint32_t s_aulRamBVBInput_DVI_1280x768p[BVDC_P_RAM_TABLE_SIZE] =
{
	0x00a24000, /* 0 */
	0x00200040, /* 1 */
	0x00201080, /* 2 */
	0x002000b7, /* 3 */
	0x002084e8, /* 4 */
	0x0024800b, /* 5 */
	0x00800000, /* 6 */
	0x00000000, /* 7' */
	0x00000000, /* 8' */
	0x00000000, /* 9' */
	0x00000000, /* 10' */
	0x00000000, /* 11' */
	0x00000000, /* 12' */
	0x00000000, /* 13' */
	0x00000000, /* 14' */
	0x00000000, /* 15' */
	0x00000000, /* 16' */
	0x00000000, /* 17' */
	0x00000000, /* 18' */
	0x00000000, /* 19' */
	0x00a40000, /* 20 */
	0x00a80000, /* 21 */
	0x00420001, /* 22 */
	0x0040001d, /* 23 */
	0x00408300, /* 24 */
	0x00815000, /* 25 */
	0x00000000, /* 26 */
	0x00000000, /* 27 */
	0x00000000, /* 28 */
	0x00000000, /* 29 */
	0x00000000, /* 30 */
	0x00000000, /* 31 */
	0x00000000, /* 32 */
	0x00000000, /* 33 */
	0x00000000, /* 34 */
	0x00000000, /* 35 */
	0x00000000, /* 36 */
	0x00000000, /* 37 */
	0x00000000, /* 38 */
	0x00000000, /* 39 */
	0x00000000, /* 40 */
	0x00000000, /* 41 */
	0x00000000, /* 42 */
	0x00000000, /* 43 */
	0x00000000, /* 44 */
	0x00000000, /* 45 */
	0x00000000, /* 46 */
	0x00000000, /* 47 */
	0x00000000, /* 48 */
	0x00000000, /* 49 */
	0x00000000, /* 50 */
	0x00000000, /* 51 */
	0x00000000, /* 52 */
	0x00000000, /* 53 */
	0x00000000, /* 54 */
	0x00000000, /* 55 */
	0x00000000, /* 56 */
	0x00000000, /* 57 */
	0x00000000, /* 58 */
	0x00000000, /* 59 */
	0x00000000, /* 60 */
	0x00000000, /* 61 */
	0x00000000, /* 62 */
	0x00000000, /* 63 */
	0x00000000, /* 64 */
	0x00000000, /* 65 */
	0x00000000, /* 66 */
	0x00000000, /* 67 */
	0x00000000, /* 68 */
	0x00000000, /* 69 */
	0x00000000, /* 70 */
	0x00000000, /* 71 */
	0x00000000, /* 72 */
	0x00000000, /* 73 */
	0x00000000, /* 74 */
	0x00000000, /* 75 */
	0x00000000, /* 76 */
	0x00000000, /* 77 */
	0x00000000, /* 78 */
	0x00000000, /* 79 */
	0x00000000, /* 80 */
	0x00000000, /* 81 */
	0x00000000, /* 82 */
	0x00000000, /* 83 */
	0x00000000, /* 84 */
	0x00000000, /* 85 */
	0x00000000, /* 86 */
	0x00000000, /* 87 */
	0x00000000, /* 88 */
	0x00000000, /* 89 */
	0x00000000, /* 90 */
	0x00000000, /* 91 */
	0x00000000, /* 92 */
	0x00000000, /* 93 */
	0x00000000, /* 94 */
	0x00000000, /* 95 */
	0x00000000, /* 96 */
	0x00000000, /* 97 */
	0x00000000, /* 98 */
	0x00000000, /* 99 */
	0x00000000, /* 100 */
	0x00000000, /* 101 */
	0x00000000, /* 102 */
	0x00000000, /* 103 */
	0x00000000, /* 104 */
	0x00000000, /* 105 */
	0x00000000, /* 106 */
	0x00000000, /* 107 */
	0x00000000, /* 108 */
	0x00000000, /* 109 */
	0x00000000, /* 110 */
	0x00000000, /* 111 */
	0x00000000, /* 112 */
	0x00000000, /* 113 */
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
	0x00000000, /* 126 */
	0x00000000, /* 127 */
	0x00000000, /* 128 */
	0x00000000, /* 129 */
	0x00000000, /* 130 */
	0x00000000, /* 131 */
	0x00000000, /* 132 */
	0x00000000, /* 133 */
	0x00000000, /* 134 */
	0x00000000, /* 135 */
	0x00000000, /* 136 */
	0x00000000, /* 137 */
	0x00000000, /* 138 */
	0x00000000, /* 139 */
	0x00000000, /* 140 */
	0x00000000, /* 141 */
	0x00000000, /* 142 */
	0x00000000, /* 143 */
	0x00000000, /* 144 */
	0x00000000, /* 145 */
	0x00000000, /* 146 */
	0x00000000, /* 147 */
	0x00000000, /* 148 */
	0x00000000, /* 149 */
	0x00000000, /* 150 */
	0x00000000, /* 151 */
	0x00000000, /* 152 */
	0x00000000, /* 153 */
	0x00000000, /* 154 */
	0x00000000, /* 155 */
	0x00000000, /* 156 */
	0x00000000, /* 157 */
	0x00000000, /* 158 */
	0x00000000, /* 159 */
	0x00000000, /* 160 */
	0x00000000, /* 161 */
	0x00000000, /* 162 */
	0x00000000, /* 163 */
	0x00000000, /* 164 */
	0x00000000, /* 165 */
	0x00000000, /* 166 */
	0x00000000, /* 167 */
	0x00000000, /* 168 */
	0x00000000, /* 169 */
	0x00000000, /* 170 */
	0x00000000, /* 171 */
	0x00000000, /* 172 */
	0x00000000, /* 173 */
	0x00000000, /* 174 */
	0x00000000, /* 175 */
	0x00000000, /* 176 */
	0x00000000, /* 177 */
	0x00000000, /* 178 */
	0x00000000, /* 179 */
	0x00000000, /* 180 */
	0x00000000, /* 181 */
	0x00000000, /* 182 */
	0x00000000, /* 183 */
	0x00000000, /* 184 */
	0x00000000, /* 185 */
	0x00000000, /* 186 */
	0x00000000, /* 187 */
	0x00000000, /* 188 */
	0x00000000, /* 189 */
	0x00000000, /* 190 */
	0x00000000, /* 191 */
	0x00000000, /* 192 */
	0x00000000, /* 193 */
	0x00000000, /* 194 */
	0x00000000, /* 195 */
	0x00000000, /* 196 */
	0x00000000, /* 197 */
	0x00000000, /* 198 */
	0x00000000, /* 199 */
	0x00000000, /* 200 */
	0x00000000, /* 201 */
	0x00000000, /* 202 */
	0x00000000, /* 203 */
	0x00000000, /* 204 */
	0x00000000, /* 205 */
	0x00000000, /* 206 */
	0x00000000, /* 207 */
	0x00000000, /* 208 */
	0x00000000, /* 209 */
	0x00000000, /* 210 */
	0x00000000, /* 211 */
	0x00000000, /* 212 */
	0x00000000, /* 213 */
	0x00000000, /* 214 */
	0x00000000, /* 215 */
	0x00000000, /* 216 */
	0x00000000, /* 217 */
	0x00000000, /* 218 */
	0x00000000, /* 219 */
	0x00000000, /* 220 */
	0x00000000, /* 221 */
	0x00000000, /* 222 */
	0x00000000, /* 223 */
	0x00000000, /* 224 */
	0x00000000, /* 225 */
	0x00000000, /* 226 */
	0x00000000, /* 227 */
	0x00000000, /* 228 */
	0x00000000, /* 229 */
	0x00000000, /* 230 */
	0x00000000, /* 231 */
	0x00000000, /* 232 */
	0x00000000, /* 233 */
	0x00000000, /* 234 */
	0x00000000, /* 235 */
	0x00000000, /* 236 */
	0x00000000, /* 237 */
	0x00000000, /* 238 */
	0x00000000, /* 239 */
	0x00000000, /* 240 */
	0x00000000, /* 241 */
	0x00000000, /* 242 */
	0x00000000, /* 243 */
	0x00000000, /* 244 */
	0x00000000, /* 245 */
	0x00000000, /* 246 */
	0x00000000, /* 247 */
	0x00000000, /* 248 */
	0x00000000, /* 249 */
	0x00000000, /* 250 */
	0x00000000, /* 251 */
	0x00000000, /* 252 */
	0x00000000, /* 253 */
	0x00090704, /* 254 */
	0x401e5bda  /* 255 */
};

/* IT_Microcode for 1280x720p 50Hz DVI */
static const uint32_t s_aulRamBVBInput_DVI_1280x720p_50Hz[BVDC_P_RAM_TABLE_SIZE] =
{
	0x00a24000, /* 0 */
	0x00200030, /* 1 */
	0x00201080, /* 2 */
	0x002000a7, /* 3 */
	0x002084e8, /* 4 */
	0x0024800b, /* 5 */
	0x00800000, /* 6 */
	0x00000000, /* 7' */
	0x00000000, /* 8' */
	0x00000000, /* 9' */
	0x00000000, /* 10' */
	0x00000000, /* 11' */
	0x00000000, /* 11 */
	0x00000000, /* 12 */
	0x00000000, /* 14' */
	0x00000000, /* 15' */
	0x00000000, /* 16' */
	0x00000000, /* 17' */
	0x00000000, /* 18' */
	0x00000000, /* 19' */
	0x00a40000, /* 20 */
	0x00a80000, /* 21 */
	0x00420001, /* 22 */
	0x00400014, /* 23 */
	0x004082d0, /* 24 */
	0x00815000, /* 25 */
	0x00000000, /* 26 */
	0x00000000, /* 27 */
	0x00000000, /* 28 */
	0x00000000, /* 29 */
	0x00000000, /* 30 */
	0x00000000, /* 33' */
	0x00000000, /* 34' */
	0x00000000, /* 35' */
	0x00000000, /* 36' */
	0x00000000, /* 37' */
	0x00000000, /* 38' */
	0x00000000, /* 39' */
	0x00000000, /* 40' */
	0x00000000, /* 41' */
	0x00000000, /* 42' */
	0x00000000, /* 43' */
	0x00000000, /* 44' */
	0x00000000, /* 45' */
	0x00000000, /* 46' */
	0x00000000, /* 47' */
	0x00000000, /* 48' */
	0x00000000, /* 49' */
	0x00000000, /* 50' */
	0x00000000, /* 51' */
	0x00000000, /* 52' */
	0x00000000, /* 53' */
	0x00000000, /* 54' */
	0x00000000, /* 55' */
	0x00000000, /* 56' */
	0x00000000, /* 57' */
	0x00000000, /* 58' */
	0x00000000, /* 59' */
	0x00000000, /* 60' */
	0x00000000, /* 61' */
	0x00000000, /* 62' */
	0x00000000, /* 63' */
	0x00000000, /* 64' */
	0x00000000, /* 65' */
	0x00000000, /* 66' */
	0x00000000, /* 67' */
	0x00000000, /* 68' */
	0x00000000, /* 69' */
	0x00000000, /* 70' */
	0x00000000, /* 71' */
	0x00000000, /* 72' */
	0x00000000, /* 73' */
	0x00000000, /* 74' */
	0x00000000, /* 75' */
	0x00000000, /* 76' */
	0x00000000, /* 77' */
	0x00000000, /* 78' */
	0x00000000, /* 79' */
	0x00000000, /* 80' */
	0x00000000, /* 81' */
	0x00000000, /* 82' */
	0x00000000, /* 83' */
	0x00000000, /* 84' */
	0x00000000, /* 85' */
	0x00000000, /* 86' */
	0x00000000, /* 87' */
	0x00000000, /* 88' */
	0x00000000, /* 89' */
	0x00000000, /* 90' */
	0x00000000, /* 91' */
	0x00000000, /* 92' */
	0x00000000, /* 93' */
	0x00000000, /* 94' */
	0x00000000, /* 95' */
	0x00000000, /* 96' */
	0x00000000, /* 97' */
	0x00000000, /* 98' */
	0x00000000, /* 99' */
	0x00000000, /* 100' */
	0x00000000, /* 101' */
	0x00000000, /* 102' */
	0x00000000, /* 103' */
	0x00000000, /* 104' */
	0x00000000, /* 105' */
	0x00000000, /* 106' */
	0x00000000, /* 107' */
	0x00000000, /* 108' */
	0x00000000, /* 109' */
	0x00000000, /* 110' */
	0x00000000, /* 111' */
	0x00000000, /* 112' */
	0x00000000, /* 113' */
	0x00000000, /* 114' */
	0x00000000, /* 115' */
	0x00000000, /* 116' */
	0x00000000, /* 117' */
	0x00000000, /* 118' */
	0x00000000, /* 119' */
	0x00000000, /* 120' */
	0x00000000, /* 121' */
	0x00000000, /* 122' */
	0x00000000, /* 123' */
	0x00000000, /* 124' */
	0x00000000, /* 125' */
	0x00000000, /* 126' */
	0x00000000, /* 127' */
	0x00000000, /* 128' */
	0x00000000, /* 129' */
	0x00000000, /* 130' */
	0x00000000, /* 131' */
	0x00000000, /* 132' */
	0x00000000, /* 133' */
	0x00000000, /* 134' */
	0x00000000, /* 135' */
	0x00000000, /* 136' */
	0x00000000, /* 137' */
	0x00000000, /* 138' */
	0x00000000, /* 139' */
	0x00000000, /* 140' */
	0x00000000, /* 141' */
	0x00000000, /* 142' */
	0x00000000, /* 143' */
	0x00000000, /* 144' */
	0x00000000, /* 145' */
	0x00000000, /* 146' */
	0x00000000, /* 147' */
	0x00000000, /* 148' */
	0x00000000, /* 149' */
	0x00000000, /* 150' */
	0x00000000, /* 151' */
	0x00000000, /* 152' */
	0x00000000, /* 153' */
	0x00000000, /* 154' */
	0x00000000, /* 155' */
	0x00000000, /* 156' */
	0x00000000, /* 157' */
	0x00000000, /* 158' */
	0x00000000, /* 159' */
	0x00000000, /* 160' */
	0x00000000, /* 161' */
	0x00000000, /* 162' */
	0x00000000, /* 163' */
	0x00000000, /* 164' */
	0x00000000, /* 165' */
	0x00000000, /* 166' */
	0x00000000, /* 167' */
	0x00000000, /* 168' */
	0x00000000, /* 169' */
	0x00000000, /* 170' */
	0x00000000, /* 171' */
	0x00000000, /* 172' */
	0x00000000, /* 173' */
	0x00000000, /* 174' */
	0x00000000, /* 175' */
	0x00000000, /* 176' */
	0x00000000, /* 177' */
	0x00000000, /* 178' */
	0x00000000, /* 179' */
	0x00000000, /* 180' */
	0x00000000, /* 181' */
	0x00000000, /* 182' */
	0x00000000, /* 183' */
	0x00000000, /* 184' */
	0x00000000, /* 185' */
	0x00000000, /* 186' */
	0x00000000, /* 187' */
	0x00000000, /* 188' */
	0x00000000, /* 189' */
	0x00000000, /* 190' */
	0x00000000, /* 191' */
	0x00000000, /* 192' */
	0x00000000, /* 193' */
	0x00000000, /* 194' */
	0x00000000, /* 195' */
	0x00000000, /* 196' */
	0x00000000, /* 197' */
	0x00000000, /* 198' */
	0x00000000, /* 199' */
	0x00000000, /* 200' */
	0x00000000, /* 201' */
	0x00000000, /* 202' */
	0x00000000, /* 203' */
	0x00000000, /* 204' */
	0x00000000, /* 205' */
	0x00000000, /* 206' */
	0x00000000, /* 207' */
	0x00000000, /* 208' */
	0x00000000, /* 209' */
	0x00000000, /* 210' */
	0x00000000, /* 211' */
	0x00000000, /* 212' */
	0x00000000, /* 213' */
	0x00000000, /* 214' */
	0x00000000, /* 215' */
	0x00000000, /* 216' */
	0x00000000, /* 217' */
	0x00000000, /* 218' */
	0x00000000, /* 219' */
	0x00000000, /* 220' */
	0x00000000, /* 221' */
	0x00000000, /* 222' */
	0x00000000, /* 223' */
	0x00000000, /* 224' */
	0x00000000, /* 225' */
	0x00000000, /* 226' */
	0x00000000, /* 227' */
	0x00000000, /* 228' */
	0x00000000, /* 229' */
	0x00000000, /* 230' */
	0x00000000, /* 231' */
	0x00000000, /* 232' */
	0x00000000, /* 233' */
	0x00000000, /* 234' */
	0x00000000, /* 235' */
	0x00000000, /* 236' */
	0x00000000, /* 237' */
	0x00000000, /* 238' */
	0x00000000, /* 239' */
	0x00000000, /* 240' */
	0x00000000, /* 241' */
	0x00000000, /* 242' */
	0x00000000, /* 243' */
	0x00000000, /* 244' */
	0x00000000, /* 245' */
	0x00000000, /* 246' */
	0x00000000, /* 245 */
	0x00000000, /* 246 */
	0x00000000, /* 247 */
	0x00000000, /* 248 */
	0x00000000, /* 249 */
	0x00000000, /* 250 */
	0x00000000, /* 251 */
	0x00000000, /* 252 */
	0x00000000, /* 253 */
	0x00122904, /* 254 */
	0xf409075f  /* 255 */
};

/* IT_Microcode for 1280x720p DVI */
static const uint32_t s_aulRamBVBInput_DVI_1280x720p[BVDC_P_RAM_TABLE_SIZE] =
{
	0x00a24000, /* 0 */
	0x00240040, /* 1 */
	0x00201080, /* 2 */
	0x002000b7, /* 3 */
	0x002084f3, /* 4 */
	0x00800000, /* 5 */
	0x00000000, /* 6 */
	0x00000000, /* 7 */
	0x00000000, /* 8 */
	0x00000000, /* 9 */
	0x00000000, /* 10 */
	0x00000000, /* 12' */
	0x00000000, /* 13' */
	0x00000000, /* 13 */
	0x00000000, /* 14 */
	0x00000000, /* 15 */
	0x00000000, /* 16 */
	0x00000000, /* 17 */
	0x00000000, /* 18 */
	0x00000000, /* 19 */
	0x00a80000, /* 20 */
	0x00421001, /* 21 */
	0x00401004, /* 22 */
	0x00400014, /* 23 */
	0x004082d0, /* 24 */
	0x00a80000, /* 25 */
	0x00400003, /* 26 */
	0x00815000, /* 27 */
	0x00000000, /* 28' */
	0x00000000, /* 29' */
	0x00000000, /* 30' */
	0x00000000, /* 31' */
	0x00000000, /* 32' */
	0x00000000, /* 31 */
	0x00000000, /* 32 */
	0x00000000, /* 33 */
	0x00000000, /* 34 */
	0x00000000, /* 35 */
	0x00000000, /* 36 */
	0x00000000, /* 37 */
	0x00000000, /* 38 */
	0x00000000, /* 39 */
	0x00000000, /* 40 */
	0x00000000, /* 41 */
	0x00000000, /* 42 */
	0x00000000, /* 43 */
	0x00000000, /* 44 */
	0x00000000, /* 45 */
	0x00000000, /* 46 */
	0x00000000, /* 47 */
	0x00000000, /* 48 */
	0x00000000, /* 49 */
	0x00000000, /* 50 */
	0x00000000, /* 51 */
	0x00000000, /* 52 */
	0x00000000, /* 53 */
	0x00000000, /* 54 */
	0x00000000, /* 55 */
	0x00000000, /* 56 */
	0x00000000, /* 57 */
	0x00000000, /* 58 */
	0x00000000, /* 59 */
	0x00000000, /* 60 */
	0x00000000, /* 61 */
	0x00000000, /* 62 */
	0x00000000, /* 63 */
	0x00000000, /* 64 */
	0x00000000, /* 65 */
	0x00000000, /* 66 */
	0x00000000, /* 67 */
	0x00000000, /* 68 */
	0x00000000, /* 69 */
	0x00000000, /* 70 */
	0x00000000, /* 71 */
	0x00000000, /* 72 */
	0x00000000, /* 73 */
	0x00000000, /* 74 */
	0x00000000, /* 75 */
	0x00000000, /* 76 */
	0x00000000, /* 77 */
	0x00000000, /* 78 */
	0x00000000, /* 79 */
	0x00000000, /* 80 */
	0x00000000, /* 81 */
	0x00000000, /* 82 */
	0x00000000, /* 83 */
	0x00000000, /* 84 */
	0x00000000, /* 85 */
	0x00000000, /* 86 */
	0x00000000, /* 87 */
	0x00000000, /* 88 */
	0x00000000, /* 89 */
	0x00000000, /* 90 */
	0x00000000, /* 91 */
	0x00000000, /* 92 */
	0x00000000, /* 93 */
	0x00000000, /* 94 */
	0x00000000, /* 95 */
	0x00000000, /* 96 */
	0x00000000, /* 97 */
	0x00000000, /* 98 */
	0x00000000, /* 99 */
	0x00000000, /* 100 */
	0x00000000, /* 101 */
	0x00000000, /* 102 */
	0x00000000, /* 103 */
	0x00000000, /* 104 */
	0x00000000, /* 105 */
	0x00000000, /* 106 */
	0x00000000, /* 107 */
	0x00000000, /* 108 */
	0x00000000, /* 109 */
	0x00000000, /* 110 */
	0x00000000, /* 111 */
	0x00000000, /* 112 */
	0x00000000, /* 113 */
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
	0x00000000, /* 126 */
	0x00000000, /* 127 */
	0x00000000, /* 128 */
	0x00000000, /* 129 */
	0x00000000, /* 130 */
	0x00000000, /* 131 */
	0x00000000, /* 132 */
	0x00000000, /* 133 */
	0x00000000, /* 134 */
	0x00000000, /* 135 */
	0x00000000, /* 136 */
	0x00000000, /* 137 */
	0x00000000, /* 138 */
	0x00000000, /* 139 */
	0x00000000, /* 140 */
	0x00000000, /* 141 */
	0x00000000, /* 142 */
	0x00000000, /* 143 */
	0x00000000, /* 144 */
	0x00000000, /* 145 */
	0x00000000, /* 146 */
	0x00000000, /* 147 */
	0x00000000, /* 148 */
	0x00000000, /* 149 */
	0x00000000, /* 150 */
	0x00000000, /* 151 */
	0x00000000, /* 152 */
	0x00000000, /* 153 */
	0x00000000, /* 154 */
	0x00000000, /* 155 */
	0x00000000, /* 156 */
	0x00000000, /* 157 */
	0x00000000, /* 158 */
	0x00000000, /* 159 */
	0x00000000, /* 160 */
	0x00000000, /* 161 */
	0x00000000, /* 162 */
	0x00000000, /* 163 */
	0x00000000, /* 164 */
	0x00000000, /* 165 */
	0x00000000, /* 166 */
	0x00000000, /* 167 */
	0x00000000, /* 168 */
	0x00000000, /* 169 */
	0x00000000, /* 170 */
	0x00000000, /* 171 */
	0x00000000, /* 172 */
	0x00000000, /* 173 */
	0x00000000, /* 174 */
	0x00000000, /* 175 */
	0x00000000, /* 176 */
	0x00000000, /* 177 */
	0x00000000, /* 178 */
	0x00000000, /* 179 */
	0x00000000, /* 180 */
	0x00000000, /* 181 */
	0x00000000, /* 182 */
	0x00000000, /* 183 */
	0x00000000, /* 184 */
	0x00000000, /* 185 */
	0x00000000, /* 186 */
	0x00000000, /* 187 */
	0x00000000, /* 188 */
	0x00000000, /* 189 */
	0x00000000, /* 190 */
	0x00000000, /* 191 */
	0x00000000, /* 192 */
	0x00000000, /* 193 */
	0x00000000, /* 194 */
	0x00000000, /* 195 */
	0x00000000, /* 196 */
	0x00000000, /* 197 */
	0x00000000, /* 198 */
	0x00000000, /* 199 */
	0x00000000, /* 200 */
	0x00000000, /* 201 */
	0x00000000, /* 202 */
	0x00000000, /* 203 */
	0x00000000, /* 204 */
	0x00000000, /* 205 */
	0x00000000, /* 206 */
	0x00000000, /* 207 */
	0x00000000, /* 208 */
	0x00000000, /* 209 */
	0x00000000, /* 210 */
	0x00000000, /* 211 */
	0x00000000, /* 212 */
	0x00000000, /* 213 */
	0x00000000, /* 214 */
	0x00000000, /* 215 */
	0x00000000, /* 216 */
	0x00000000, /* 217 */
	0x00000000, /* 218 */
	0x00000000, /* 219 */
	0x00000000, /* 220 */
	0x00000000, /* 221 */
	0x00000000, /* 222 */
	0x00000000, /* 223 */
	0x00000000, /* 224 */
	0x00000000, /* 225 */
	0x00000000, /* 226 */
	0x00000000, /* 227 */
	0x00000000, /* 228 */
	0x00000000, /* 229 */
	0x00000000, /* 230 */
	0x00000000, /* 231 */
	0x00000000, /* 232 */
	0x00000000, /* 233 */
	0x00000000, /* 234 */
	0x00000000, /* 235 */
	0x00000000, /* 236 */
	0x00000000, /* 237 */
	0x00000000, /* 238 */
	0x00000000, /* 239 */
	0x00000000, /* 240 */
	0x00000000, /* 241 */
	0x00000000, /* 242 */
	0x00000000, /* 243 */
	0x00000000, /* 244 */
	0x00000000, /* 247' */
	0x00000000, /* 248' */
	0x00000000, /* 249' */
	0x00000000, /* 250' */
	0x00000000, /* 251' */
	0x00000000, /* 252' */
	0x008fd000, /* 253 */
	0x00090109, /* 254 */
	0x00c5c41d, /* 255 */
};

/* IT_Microcode for 1280x720p Reduced Blanking DVI */
static const uint32_t s_aulRamBVBInput_DVI_1280x720p_Red[BVDC_P_RAM_TABLE_SIZE] =
{
	0x00a24000, /* 0 */
	0x00200030, /* 1 */
	0x00201020, /* 2 */
	0x00200047, /* 3 */
	0x002084e8, /* 4 */
	0x0024800b, /* 5 */
	0x00800000, /* 6 */
	0x00000000, /* 7' */
	0x00000000, /* 8' */
	0x00000000, /* 9' */
	0x00000000, /* 10' */
	0x00000000, /* 11' */
	0x00000000, /* 11 */
	0x00000000, /* 12 */
	0x00000000, /* 14' */
	0x00000000, /* 15' */
	0x00000000, /* 16' */
	0x00000000, /* 17' */
	0x00000000, /* 18' */
	0x00000000, /* 19' */
	0x00a40000, /* 20 */
	0x00a80000, /* 21 */
	0x00420001, /* 22 */
	0x00400014, /* 23 */
	0x004082d0, /* 24 */
	0x00815000, /* 25 */
	0x00000000, /* 26 */
	0x00000000, /* 27 */
	0x00000000, /* 28 */
	0x00000000, /* 29 */
	0x00000000, /* 30 */
	0x00000000, /* 33' */
	0x00000000, /* 34' */
	0x00000000, /* 35' */
	0x00000000, /* 36' */
	0x00000000, /* 37' */
	0x00000000, /* 38' */
	0x00000000, /* 39' */
	0x00000000, /* 40' */
	0x00000000, /* 41' */
	0x00000000, /* 42' */
	0x00000000, /* 43' */
	0x00000000, /* 44' */
	0x00000000, /* 45' */
	0x00000000, /* 46' */
	0x00000000, /* 47' */
	0x00000000, /* 48' */
	0x00000000, /* 49' */
	0x00000000, /* 50' */
	0x00000000, /* 51' */
	0x00000000, /* 52' */
	0x00000000, /* 53' */
	0x00000000, /* 54' */
	0x00000000, /* 55' */
	0x00000000, /* 56' */
	0x00000000, /* 57' */
	0x00000000, /* 58' */
	0x00000000, /* 59' */
	0x00000000, /* 60' */
	0x00000000, /* 61' */
	0x00000000, /* 62' */
	0x00000000, /* 63' */
	0x00000000, /* 64' */
	0x00000000, /* 65' */
	0x00000000, /* 66' */
	0x00000000, /* 67' */
	0x00000000, /* 68' */
	0x00000000, /* 69' */
	0x00000000, /* 70' */
	0x00000000, /* 71' */
	0x00000000, /* 72' */
	0x00000000, /* 73' */
	0x00000000, /* 74' */
	0x00000000, /* 75' */
	0x00000000, /* 76' */
	0x00000000, /* 77' */
	0x00000000, /* 78' */
	0x00000000, /* 79' */
	0x00000000, /* 80' */
	0x00000000, /* 81' */
	0x00000000, /* 82' */
	0x00000000, /* 83' */
	0x00000000, /* 84' */
	0x00000000, /* 85' */
	0x00000000, /* 86' */
	0x00000000, /* 87' */
	0x00000000, /* 88' */
	0x00000000, /* 89' */
	0x00000000, /* 90' */
	0x00000000, /* 91' */
	0x00000000, /* 92' */
	0x00000000, /* 93' */
	0x00000000, /* 94' */
	0x00000000, /* 95' */
	0x00000000, /* 96' */
	0x00000000, /* 97' */
	0x00000000, /* 98' */
	0x00000000, /* 99' */
	0x00000000, /* 100' */
	0x00000000, /* 101' */
	0x00000000, /* 102' */
	0x00000000, /* 103' */
	0x00000000, /* 104' */
	0x00000000, /* 105' */
	0x00000000, /* 106' */
	0x00000000, /* 107' */
	0x00000000, /* 108' */
	0x00000000, /* 109' */
	0x00000000, /* 110' */
	0x00000000, /* 111' */
	0x00000000, /* 112' */
	0x00000000, /* 113' */
	0x00000000, /* 114' */
	0x00000000, /* 115' */
	0x00000000, /* 116' */
	0x00000000, /* 117' */
	0x00000000, /* 118' */
	0x00000000, /* 119' */
	0x00000000, /* 120' */
	0x00000000, /* 121' */
	0x00000000, /* 122' */
	0x00000000, /* 123' */
	0x00000000, /* 124' */
	0x00000000, /* 125' */
	0x00000000, /* 126' */
	0x00000000, /* 127' */
	0x00000000, /* 128' */
	0x00000000, /* 129' */
	0x00000000, /* 130' */
	0x00000000, /* 131' */
	0x00000000, /* 132' */
	0x00000000, /* 133' */
	0x00000000, /* 134' */
	0x00000000, /* 135' */
	0x00000000, /* 136' */
	0x00000000, /* 137' */
	0x00000000, /* 138' */
	0x00000000, /* 139' */
	0x00000000, /* 140' */
	0x00000000, /* 141' */
	0x00000000, /* 142' */
	0x00000000, /* 143' */
	0x00000000, /* 144' */
	0x00000000, /* 145' */
	0x00000000, /* 146' */
	0x00000000, /* 147' */
	0x00000000, /* 148' */
	0x00000000, /* 149' */
	0x00000000, /* 150' */
	0x00000000, /* 151' */
	0x00000000, /* 152' */
	0x00000000, /* 153' */
	0x00000000, /* 154' */
	0x00000000, /* 155' */
	0x00000000, /* 156' */
	0x00000000, /* 157' */
	0x00000000, /* 158' */
	0x00000000, /* 159' */
	0x00000000, /* 160' */
	0x00000000, /* 161' */
	0x00000000, /* 162' */
	0x00000000, /* 163' */
	0x00000000, /* 164' */
	0x00000000, /* 165' */
	0x00000000, /* 166' */
	0x00000000, /* 167' */
	0x00000000, /* 168' */
	0x00000000, /* 169' */
	0x00000000, /* 170' */
	0x00000000, /* 171' */
	0x00000000, /* 172' */
	0x00000000, /* 173' */
	0x00000000, /* 174' */
	0x00000000, /* 175' */
	0x00000000, /* 176' */
	0x00000000, /* 177' */
	0x00000000, /* 178' */
	0x00000000, /* 179' */
	0x00000000, /* 180' */
	0x00000000, /* 181' */
	0x00000000, /* 182' */
	0x00000000, /* 183' */
	0x00000000, /* 184' */
	0x00000000, /* 185' */
	0x00000000, /* 186' */
	0x00000000, /* 187' */
	0x00000000, /* 188' */
	0x00000000, /* 189' */
	0x00000000, /* 190' */
	0x00000000, /* 191' */
	0x00000000, /* 192' */
	0x00000000, /* 193' */
	0x00000000, /* 194' */
	0x00000000, /* 195' */
	0x00000000, /* 196' */
	0x00000000, /* 197' */
	0x00000000, /* 198' */
	0x00000000, /* 199' */
	0x00000000, /* 200' */
	0x00000000, /* 201' */
	0x00000000, /* 202' */
	0x00000000, /* 203' */
	0x00000000, /* 204' */
	0x00000000, /* 205' */
	0x00000000, /* 206' */
	0x00000000, /* 207' */
	0x00000000, /* 208' */
	0x00000000, /* 209' */
	0x00000000, /* 210' */
	0x00000000, /* 211' */
	0x00000000, /* 212' */
	0x00000000, /* 213' */
	0x00000000, /* 214' */
	0x00000000, /* 215' */
	0x00000000, /* 216' */
	0x00000000, /* 217' */
	0x00000000, /* 218' */
	0x00000000, /* 219' */
	0x00000000, /* 220' */
	0x00000000, /* 221' */
	0x00000000, /* 222' */
	0x00000000, /* 223' */
	0x00000000, /* 224' */
	0x00000000, /* 225' */
	0x00000000, /* 226' */
	0x00000000, /* 227' */
	0x00000000, /* 228' */
	0x00000000, /* 229' */
	0x00000000, /* 230' */
	0x00000000, /* 231' */
	0x00000000, /* 232' */
	0x00000000, /* 233' */
	0x00000000, /* 234' */
	0x00000000, /* 235' */
	0x00000000, /* 236' */
	0x00000000, /* 237' */
	0x00000000, /* 238' */
	0x00000000, /* 239' */
	0x00000000, /* 240' */
	0x00000000, /* 241' */
	0x00000000, /* 242' */
	0x00000000, /* 243' */
	0x00000000, /* 244' */
	0x00000000, /* 245' */
	0x00000000, /* 246' */
	0x00000000, /* 245 */
	0x00000000, /* 246 */
	0x00000000, /* 247 */
	0x00000000, /* 248 */
	0x00000000, /* 249 */
	0x00000000, /* 250 */
	0x00000000, /* 251 */
	0x00000000, /* 252 */
	0x00000000, /* 253 */
	0x00010105, /* 254 */
	0x77d35cc6 /* 255 */
};

/* IT_Microcode for 1280x800p_60Hz DVI mode */
static const uint32_t s_aulRamBVBInput_DVI_1280x800p_60Hz[BVDC_P_RAM_TABLE_SIZE] =
{
	0x00a24000, /* 0 */
	0x00240048, /* 1 */
	0x00201080, /* 2 */
	0x002000bf, /* 3 */
	0x002084f3, /* 4 */
	0x00800000, /* 5 */
	0x00000000, /* 6 */
	0x00000000, /* 7 */
	0x00000000, /* 8 */
	0x00000000, /* 9 */
	0x00000000, /* 10 */
	0x00000000, /* 11 */
	0x00000000, /* 12 */
	0x00000000, /* 13 */
	0x00000000, /* 14 */
	0x00000000, /* 15 */
	0x00000000, /* 16 */
	0x00000000, /* 17 */
	0x00000000, /* 18 */
	0x00000000, /* 19 */
	0x00a80000, /* 20 */
	0x00421001, /* 21 */
	0x00401005, /* 22 */
	0x00400016, /* 23 */
	0x00408320, /* 24 */
	0x00a80000, /* 25 */
	0x00400003, /* 26 */
	0x00815000, /* 27 */
	0x00000000, /* 28' */
	0x00000000, /* 29' */
	0x00000000, /* 30' */
	0x00000000, /* 31' */
	0x00000000, /* 32' */
	0x00000000, /* 33' */
	0x00000000, /* 34' */
	0x00000000, /* 35' */
	0x00000000, /* 36' */
	0x00000000, /* 37' */
	0x00000000, /* 38' */
	0x00000000, /* 39' */
	0x00000000, /* 40' */
	0x00000000, /* 41' */
	0x00000000, /* 42' */
	0x00000000, /* 43' */
	0x00000000, /* 44' */
	0x00000000, /* 45' */
	0x00000000, /* 46' */
	0x00000000, /* 47' */
	0x00000000, /* 48' */
	0x00000000, /* 49' */
	0x00000000, /* 50' */
	0x00000000, /* 51' */
	0x00000000, /* 52' */
	0x00000000, /* 53' */
	0x00000000, /* 54' */
	0x00000000, /* 55' */
	0x00000000, /* 56' */
	0x00000000, /* 57' */
	0x00000000, /* 58' */
	0x00000000, /* 59' */
	0x00000000, /* 60' */
	0x00000000, /* 61' */
	0x00000000, /* 62' */
	0x00000000, /* 63' */
	0x00000000, /* 64' */
	0x00000000, /* 65' */
	0x00000000, /* 66' */
	0x00000000, /* 67' */
	0x00000000, /* 68' */
	0x00000000, /* 69' */
	0x00000000, /* 70' */
	0x00000000, /* 71' */
	0x00000000, /* 72' */
	0x00000000, /* 73' */
	0x00000000, /* 74' */
	0x00000000, /* 75' */
	0x00000000, /* 76' */
	0x00000000, /* 77' */
	0x00000000, /* 78' */
	0x00000000, /* 79' */
	0x00000000, /* 80' */
	0x00000000, /* 81' */
	0x00000000, /* 82' */
	0x00000000, /* 83' */
	0x00000000, /* 84' */
	0x00000000, /* 85' */
	0x00000000, /* 86' */
	0x00000000, /* 87' */
	0x00000000, /* 88' */
	0x00000000, /* 89' */
	0x00000000, /* 90' */
	0x00000000, /* 91' */
	0x00000000, /* 92' */
	0x00000000, /* 93' */
	0x00000000, /* 94' */
	0x00000000, /* 95' */
	0x00000000, /* 96' */
	0x00000000, /* 97' */
	0x00000000, /* 98' */
	0x00000000, /* 99' */
	0x00000000, /* 100' */
	0x00000000, /* 101' */
	0x00000000, /* 102' */
	0x00000000, /* 103' */
	0x00000000, /* 104' */
	0x00000000, /* 105' */
	0x00000000, /* 106' */
	0x00000000, /* 107' */
	0x00000000, /* 108' */
	0x00000000, /* 109' */
	0x00000000, /* 110' */
	0x00000000, /* 111' */
	0x00000000, /* 112' */
	0x00000000, /* 113' */
	0x00000000, /* 114' */
	0x00000000, /* 115' */
	0x00000000, /* 116' */
	0x00000000, /* 117' */
	0x00000000, /* 118' */
	0x00000000, /* 119' */
	0x00000000, /* 120' */
	0x00000000, /* 121' */
	0x00000000, /* 122' */
	0x00000000, /* 123' */
	0x00000000, /* 124' */
	0x00000000, /* 125' */
	0x00000000, /* 126' */
	0x00000000, /* 127' */
	0x00000000, /* 128' */
	0x00000000, /* 129' */
	0x00000000, /* 130' */
	0x00000000, /* 131' */
	0x00000000, /* 132' */
	0x00000000, /* 133' */
	0x00000000, /* 134' */
	0x00000000, /* 135' */
	0x00000000, /* 136' */
	0x00000000, /* 137' */
	0x00000000, /* 138' */
	0x00000000, /* 139' */
	0x00000000, /* 140' */
	0x00000000, /* 141' */
	0x00000000, /* 142' */
	0x00000000, /* 143' */
	0x00000000, /* 144' */
	0x00000000, /* 145' */
	0x00000000, /* 146' */
	0x00000000, /* 147' */
	0x00000000, /* 148' */
	0x00000000, /* 149' */
	0x00000000, /* 150' */
	0x00000000, /* 151' */
	0x00000000, /* 152' */
	0x00000000, /* 153' */
	0x00000000, /* 154' */
	0x00000000, /* 155' */
	0x00000000, /* 156' */
	0x00000000, /* 157' */
	0x00000000, /* 158' */
	0x00000000, /* 159' */
	0x00000000, /* 160' */
	0x00000000, /* 161' */
	0x00000000, /* 162' */
	0x00000000, /* 163' */
	0x00000000, /* 164' */
	0x00000000, /* 165' */
	0x00000000, /* 166' */
	0x00000000, /* 167' */
	0x00000000, /* 168' */
	0x00000000, /* 169' */
	0x00000000, /* 170' */
	0x00000000, /* 171' */
	0x00000000, /* 172' */
	0x00000000, /* 173' */
	0x00000000, /* 174' */
	0x00000000, /* 175' */
	0x00000000, /* 176' */
	0x00000000, /* 177' */
	0x00000000, /* 178' */
	0x00000000, /* 179' */
	0x00000000, /* 180' */
	0x00000000, /* 181' */
	0x00000000, /* 182' */
	0x00000000, /* 183' */
	0x00000000, /* 184' */
	0x00000000, /* 185' */
	0x00000000, /* 186' */
	0x00000000, /* 187' */
	0x00000000, /* 188' */
	0x00000000, /* 189' */
	0x00000000, /* 190' */
	0x00000000, /* 191' */
	0x00000000, /* 192' */
	0x00000000, /* 193' */
	0x00000000, /* 194' */
	0x00000000, /* 195' */
	0x00000000, /* 196' */
	0x00000000, /* 197' */
	0x00000000, /* 198' */
	0x00000000, /* 199' */
	0x00000000, /* 200' */
	0x00000000, /* 201' */
	0x00000000, /* 202' */
	0x00000000, /* 203' */
	0x00000000, /* 204' */
	0x00000000, /* 205' */
	0x00000000, /* 206' */
	0x00000000, /* 207' */
	0x00000000, /* 208' */
	0x00000000, /* 209' */
	0x00000000, /* 210' */
	0x00000000, /* 211' */
	0x00000000, /* 212' */
	0x00000000, /* 213' */
	0x00000000, /* 214' */
	0x00000000, /* 215' */
	0x00000000, /* 216' */
	0x00000000, /* 217' */
	0x00000000, /* 218' */
	0x00000000, /* 219' */
	0x00000000, /* 220' */
	0x00000000, /* 221' */
	0x00000000, /* 222' */
	0x00000000, /* 223' */
	0x00000000, /* 224' */
	0x00000000, /* 225' */
	0x00000000, /* 226' */
	0x00000000, /* 227' */
	0x00000000, /* 228' */
	0x00000000, /* 229' */
	0x00000000, /* 230' */
	0x00000000, /* 231' */
	0x00000000, /* 232' */
	0x00000000, /* 233' */
	0x00000000, /* 234' */
	0x00000000, /* 235' */
	0x00000000, /* 236' */
	0x00000000, /* 237' */
	0x00000000, /* 238' */
	0x00000000, /* 239' */
	0x00000000, /* 240' */
	0x00000000, /* 241' */
	0x00000000, /* 242' */
	0x00000000, /* 243' */
	0x00000000, /* 244' */
	0x00000000, /* 245' */
	0x00000000, /* 246' */
	0x00000000, /* 247' */
	0x00000000, /* 248' */
	0x00000000, /* 249' */
	0x00000000, /* 250' */
	0x00000000, /* 251' */
	0x00000000, /* 252' */
	0x008fd000, /* 253 */
	0x00090109, /* 254 */
	0x00c5c480, /* 255 */
};

/* IT_Microcode for VESA 1280x1024p 60Hz DVI mode */
static const uint32_t
	s_aulRamBVBInput_DVI_1280x1024p_60Hz[BVDC_P_RAM_TABLE_SIZE] =
{
	0x00a24000, /* 0 */
	0x00240030, /* 1 */
	0x00201070, /* 2 */
	0x002000ef, /* 3 */
	0x002084f3, /* 4 */
	0x00800000, /* 5 */
	0x00000000, /* 6 */
	0x00000000, /* 7 */
	0x00000000, /* 8 */
	0x00000000, /* 9 */
	0x00000000, /* 10 */
	0x00000000, /* 11 */
	0x00000000, /* 12 */
	0x00000000, /* 13 */
	0x00000000, /* 14 */
	0x00000000, /* 15 */
	0x00000000, /* 16 */
	0x00000000, /* 17 */
	0x00000000, /* 18 */
	0x00000000, /* 19 */
	0x00a80000, /* 20 */
	0x00421001, /* 21 */
	0x00401002, /* 22 */
	0x00400026, /* 23 */
	0x00408400, /* 24 */
	0x00a80000, /* 25 */
	0x00400001, /* 26 */
	0x00815000, /* 27 */
	0x00000000, /* 28' */
	0x00000000, /* 29' */
	0x00000000, /* 30' */
	0x00000000, /* 31' */
	0x00000000, /* 32' */
	0x00000000, /* 33' */
	0x00000000, /* 35 */
	0x00000000, /* 36 */
	0x00000000, /* 37 */
	0x00000000, /* 38 */
	0x00000000, /* 39 */
	0x00000000, /* 40 */
	0x00000000, /* 41 */
	0x00000000, /* 42 */
	0x00000000, /* 43 */
	0x00000000, /* 44 */
	0x00000000, /* 45 */
	0x00000000, /* 46 */
	0x00000000, /* 47 */
	0x00000000, /* 48 */
	0x00000000, /* 49 */
	0x00000000, /* 50 */
	0x00000000, /* 51 */
	0x00000000, /* 52 */
	0x00000000, /* 53 */
	0x00000000, /* 54 */
	0x00000000, /* 55 */
	0x00000000, /* 56 */
	0x00000000, /* 57 */
	0x00000000, /* 58 */
	0x00000000, /* 59 */
	0x00000000, /* 60 */
	0x00000000, /* 61 */
	0x00000000, /* 62 */
	0x00000000, /* 63 */
	0x00000000, /* 64 */
	0x00000000, /* 65 */
	0x00000000, /* 66 */
	0x00000000, /* 67 */
	0x00000000, /* 68 */
	0x00000000, /* 69 */
	0x00000000, /* 70 */
	0x00000000, /* 71 */
	0x00000000, /* 72 */
	0x00000000, /* 73 */
	0x00000000, /* 74 */
	0x00000000, /* 75 */
	0x00000000, /* 76 */
	0x00000000, /* 77 */
	0x00000000, /* 78 */
	0x00000000, /* 79 */
	0x00000000, /* 80 */
	0x00000000, /* 81 */
	0x00000000, /* 82 */
	0x00000000, /* 83 */
	0x00000000, /* 84 */
	0x00000000, /* 85 */
	0x00000000, /* 86 */
	0x00000000, /* 87 */
	0x00000000, /* 88 */
	0x00000000, /* 89 */
	0x00000000, /* 90 */
	0x00000000, /* 91 */
	0x00000000, /* 92 */
	0x00000000, /* 93 */
	0x00000000, /* 94 */
	0x00000000, /* 95 */
	0x00000000, /* 96 */
	0x00000000, /* 97 */
	0x00000000, /* 98 */
	0x00000000, /* 99 */
	0x00000000, /* 100 */
	0x00000000, /* 101 */
	0x00000000, /* 102 */
	0x00000000, /* 103 */
	0x00000000, /* 104 */
	0x00000000, /* 105 */
	0x00000000, /* 106 */
	0x00000000, /* 107 */
	0x00000000, /* 108 */
	0x00000000, /* 109 */
	0x00000000, /* 110 */
	0x00000000, /* 111 */
	0x00000000, /* 112 */
	0x00000000, /* 113 */
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
	0x00000000, /* 126 */
	0x00000000, /* 127 */
	0x00000000, /* 128 */
	0x00000000, /* 129 */
	0x00000000, /* 130 */
	0x00000000, /* 131 */
	0x00000000, /* 132 */
	0x00000000, /* 133 */
	0x00000000, /* 134 */
	0x00000000, /* 135 */
	0x00000000, /* 136 */
	0x00000000, /* 137 */
	0x00000000, /* 138 */
	0x00000000, /* 139 */
	0x00000000, /* 140 */
	0x00000000, /* 141 */
	0x00000000, /* 142 */
	0x00000000, /* 143 */
	0x00000000, /* 144 */
	0x00000000, /* 145 */
	0x00000000, /* 146 */
	0x00000000, /* 147 */
	0x00000000, /* 148 */
	0x00000000, /* 149 */
	0x00000000, /* 150 */
	0x00000000, /* 151 */
	0x00000000, /* 152 */
	0x00000000, /* 153 */
	0x00000000, /* 154 */
	0x00000000, /* 155 */
	0x00000000, /* 156 */
	0x00000000, /* 157 */
	0x00000000, /* 158 */
	0x00000000, /* 159 */
	0x00000000, /* 160 */
	0x00000000, /* 161 */
	0x00000000, /* 162 */
	0x00000000, /* 163 */
	0x00000000, /* 164 */
	0x00000000, /* 165 */
	0x00000000, /* 166 */
	0x00000000, /* 167 */
	0x00000000, /* 168 */
	0x00000000, /* 169 */
	0x00000000, /* 170 */
	0x00000000, /* 171 */
	0x00000000, /* 172 */
	0x00000000, /* 173 */
	0x00000000, /* 174 */
	0x00000000, /* 175 */
	0x00000000, /* 176 */
	0x00000000, /* 177 */
	0x00000000, /* 178 */
	0x00000000, /* 179 */
	0x00000000, /* 180 */
	0x00000000, /* 181 */
	0x00000000, /* 182 */
	0x00000000, /* 183 */
	0x00000000, /* 184 */
	0x00000000, /* 185 */
	0x00000000, /* 186 */
	0x00000000, /* 187 */
	0x00000000, /* 188 */
	0x00000000, /* 189 */
	0x00000000, /* 190 */
	0x00000000, /* 191 */
	0x00000000, /* 192 */
	0x00000000, /* 193 */
	0x00000000, /* 194 */
	0x00000000, /* 195 */
	0x00000000, /* 196 */
	0x00000000, /* 197 */
	0x00000000, /* 198 */
	0x00000000, /* 199 */
	0x00000000, /* 200 */
	0x00000000, /* 201 */
	0x00000000, /* 202 */
	0x00000000, /* 203 */
	0x00000000, /* 204 */
	0x00000000, /* 205 */
	0x00000000, /* 206 */
	0x00000000, /* 207 */
	0x00000000, /* 208 */
	0x00000000, /* 209 */
	0x00000000, /* 210 */
	0x00000000, /* 211 */
	0x00000000, /* 212 */
	0x00000000, /* 213 */
	0x00000000, /* 214 */
	0x00000000, /* 215 */
	0x00000000, /* 216 */
	0x00000000, /* 217 */
	0x00000000, /* 218 */
	0x00000000, /* 219 */
	0x00000000, /* 220 */
	0x00000000, /* 221 */
	0x00000000, /* 222 */
	0x00000000, /* 223 */
	0x00000000, /* 224 */
	0x00000000, /* 225 */
	0x00000000, /* 226 */
	0x00000000, /* 227 */
	0x00000000, /* 228 */
	0x00000000, /* 229 */
	0x00000000, /* 230 */
	0x00000000, /* 231 */
	0x00000000, /* 232 */
	0x00000000, /* 233 */
	0x00000000, /* 234 */
	0x00000000, /* 235 */
	0x00000000, /* 236 */
	0x00000000, /* 237 */
	0x00000000, /* 238 */
	0x00000000, /* 239 */
	0x00000000, /* 240 */
	0x00000000, /* 241 */
	0x00000000, /* 242 */
	0x00000000, /* 243 */
	0x00000000, /* 244 */
	0x00000000, /* 245 */
	0x00000000, /* 246 */
	0x00000000, /* 246' */
	0x00000000, /* 247' */
	0x00000000, /* 248' */
	0x00000000, /* 249' */
	0x00000000, /* 250' */
	0x00000000, /* 251' */
	0x00000000, /* 252' */
	0x008fd000, /* 253 */
	0x00090109, /* 254 */
	0x00c5c573, /* 255 */
};

/* IT_Microcode for 1366x768p_60Hz DVI mode */
static const uint32_t s_aulRamBVBInput_DVI_1366x768p_60Hz[BVDC_P_RAM_TABLE_SIZE] =
{
	0x00a24000, /* 0 */
	0x00200046, /* 1 */
	0x0020108f, /* 2 */
	0x002000cc, /* 3 */
	0x0020853e, /* 4 */
	0x0024800b, /* 5 */
	0x00800000, /* 6 */
	0x00000000, /* 7 */
	0x00000000, /* 8 */
	0x00000000, /* 9 */
	0x00000000, /* 10 */
	0x00000000, /* 11 */
	0x00000000, /* 12 */
	0x00000000, /* 13 */
	0x00000000, /* 14 */
	0x00000000, /* 15 */
	0x00000000, /* 16 */
	0x00000000, /* 17 */
	0x00000000, /* 18 */
	0x00000000, /* 19 */
	0x00a80000, /* 20 */
	0x00421001, /* 21 */
	0x00401002, /* 22 */
	0x00400018, /* 23 */
	0x00408300, /* 24 */
	0x00400003, /* 25 */
	0x00814000, /* 26 */
	0x00000000, /* 27 */
	0x00000000, /* 28 */
	0x00000000, /* 29 */
	0x00000000, /* 30 */
	0x00000000, /* 31 */
	0x00000000, /* 32 */
	0x00000000, /* 34' */
	0x00000000, /* 35' */
	0x00000000, /* 36' */
	0x00000000, /* 37' */
	0x00000000, /* 38' */
	0x00000000, /* 39' */
	0x00000000, /* 40' */
	0x00000000, /* 41' */
	0x00000000, /* 42' */
	0x00000000, /* 43' */
	0x00000000, /* 44' */
	0x00000000, /* 45' */
	0x00000000, /* 46' */
	0x00000000, /* 47' */
	0x00000000, /* 48' */
	0x00000000, /* 49' */
	0x00000000, /* 50' */
	0x00000000, /* 51' */
	0x00000000, /* 52' */
	0x00000000, /* 53' */
	0x00000000, /* 54' */
	0x00000000, /* 55' */
	0x00000000, /* 56' */
	0x00000000, /* 57' */
	0x00000000, /* 58' */
	0x00000000, /* 59' */
	0x00000000, /* 60' */
	0x00000000, /* 61' */
	0x00000000, /* 62' */
	0x00000000, /* 63' */
	0x00000000, /* 64' */
	0x00000000, /* 65' */
	0x00000000, /* 66' */
	0x00000000, /* 67' */
	0x00000000, /* 68' */
	0x00000000, /* 69' */
	0x00000000, /* 70' */
	0x00000000, /* 71' */
	0x00000000, /* 72' */
	0x00000000, /* 73' */
	0x00000000, /* 74' */
	0x00000000, /* 75' */
	0x00000000, /* 76' */
	0x00000000, /* 77' */
	0x00000000, /* 78' */
	0x00000000, /* 79' */
	0x00000000, /* 80' */
	0x00000000, /* 81' */
	0x00000000, /* 82' */
	0x00000000, /* 83' */
	0x00000000, /* 84' */
	0x00000000, /* 85' */
	0x00000000, /* 86' */
	0x00000000, /* 87' */
	0x00000000, /* 88' */
	0x00000000, /* 89' */
	0x00000000, /* 90' */
	0x00000000, /* 91' */
	0x00000000, /* 92' */
	0x00000000, /* 93' */
	0x00000000, /* 94' */
	0x00000000, /* 95' */
	0x00000000, /* 96' */
	0x00000000, /* 97' */
	0x00000000, /* 98' */
	0x00000000, /* 99' */
	0x00000000, /* 100' */
	0x00000000, /* 101' */
	0x00000000, /* 102' */
	0x00000000, /* 103' */
	0x00000000, /* 104' */
	0x00000000, /* 105' */
	0x00000000, /* 106' */
	0x00000000, /* 107' */
	0x00000000, /* 108' */
	0x00000000, /* 109' */
	0x00000000, /* 110' */
	0x00000000, /* 111' */
	0x00000000, /* 112' */
	0x00000000, /* 113' */
	0x00000000, /* 114' */
	0x00000000, /* 115' */
	0x00000000, /* 116' */
	0x00000000, /* 117' */
	0x00000000, /* 118' */
	0x00000000, /* 119' */
	0x00000000, /* 120' */
	0x00000000, /* 121' */
	0x00000000, /* 122' */
	0x00000000, /* 123' */
	0x00000000, /* 124' */
	0x00000000, /* 125' */
	0x00000000, /* 126' */
	0x00000000, /* 127' */
	0x00000000, /* 128' */
	0x00000000, /* 129' */
	0x00000000, /* 130' */
	0x00000000, /* 131' */
	0x00000000, /* 132' */
	0x00000000, /* 133' */
	0x00000000, /* 134' */
	0x00000000, /* 135' */
	0x00000000, /* 136' */
	0x00000000, /* 137' */
	0x00000000, /* 138' */
	0x00000000, /* 139' */
	0x00000000, /* 140' */
	0x00000000, /* 141' */
	0x00000000, /* 142' */
	0x00000000, /* 143' */
	0x00000000, /* 144' */
	0x00000000, /* 145' */
	0x00000000, /* 146' */
	0x00000000, /* 147' */
	0x00000000, /* 148' */
	0x00000000, /* 149' */
	0x00000000, /* 150' */
	0x00000000, /* 151' */
	0x00000000, /* 152' */
	0x00000000, /* 153' */
	0x00000000, /* 154' */
	0x00000000, /* 155' */
	0x00000000, /* 156' */
	0x00000000, /* 157' */
	0x00000000, /* 158' */
	0x00000000, /* 159' */
	0x00000000, /* 160' */
	0x00000000, /* 161' */
	0x00000000, /* 162' */
	0x00000000, /* 163' */
	0x00000000, /* 164' */
	0x00000000, /* 165' */
	0x00000000, /* 166' */
	0x00000000, /* 167' */
	0x00000000, /* 168' */
	0x00000000, /* 169' */
	0x00000000, /* 170' */
	0x00000000, /* 171' */
	0x00000000, /* 172' */
	0x00000000, /* 173' */
	0x00000000, /* 174' */
	0x00000000, /* 175' */
	0x00000000, /* 176' */
	0x00000000, /* 177' */
	0x00000000, /* 178' */
	0x00000000, /* 179' */
	0x00000000, /* 180' */
	0x00000000, /* 181' */
	0x00000000, /* 182' */
	0x00000000, /* 183' */
	0x00000000, /* 184' */
	0x00000000, /* 185' */
	0x00000000, /* 186' */
	0x00000000, /* 187' */
	0x00000000, /* 188' */
	0x00000000, /* 189' */
	0x00000000, /* 190' */
	0x00000000, /* 191' */
	0x00000000, /* 192' */
	0x00000000, /* 193' */
	0x00000000, /* 194' */
	0x00000000, /* 195' */
	0x00000000, /* 196' */
	0x00000000, /* 197' */
	0x00000000, /* 198' */
	0x00000000, /* 199' */
	0x00000000, /* 200' */
	0x00000000, /* 201' */
	0x00000000, /* 202' */
	0x00000000, /* 203' */
	0x00000000, /* 204' */
	0x00000000, /* 205' */
	0x00000000, /* 206' */
	0x00000000, /* 207' */
	0x00000000, /* 208' */
	0x00000000, /* 209' */
	0x00000000, /* 210' */
	0x00000000, /* 211' */
	0x00000000, /* 212' */
	0x00000000, /* 213' */
	0x00000000, /* 214' */
	0x00000000, /* 215' */
	0x00000000, /* 216' */
	0x00000000, /* 217' */
	0x00000000, /* 218' */
	0x00000000, /* 219' */
	0x00000000, /* 220' */
	0x00000000, /* 221' */
	0x00000000, /* 222' */
	0x00000000, /* 223' */
	0x00000000, /* 224' */
	0x00000000, /* 225' */
	0x00000000, /* 226' */
	0x00000000, /* 227' */
	0x00000000, /* 228' */
	0x00000000, /* 229' */
	0x00000000, /* 230' */
	0x00000000, /* 231' */
	0x00000000, /* 232' */
	0x00000000, /* 233' */
	0x00000000, /* 234' */
	0x00000000, /* 235' */
	0x00000000, /* 236' */
	0x00000000, /* 237' */
	0x00000000, /* 238' */
	0x00000000, /* 239' */
	0x00000000, /* 240' */
	0x00000000, /* 241' */
	0x00000000, /* 242' */
	0x00000000, /* 243' */
	0x00000000, /* 244' */
	0x00000000, /* 245' */
	0x00000000, /* 246' */
	0x00000000, /* 246 */
	0x00000000, /* 247 */
	0x00000000, /* 248 */
	0x00000000, /* 249 */
	0x00000000, /* 250 */
	0x00000000, /* 251 */
	0x00000000, /* 252 */
	0x008fd000, /* 253 */
	0x00070909, /* 254 */
	0x003e34cf, /* 255 */
};

/* IT_Microcode for 1440x900p_60Hz DVI mode */
static const uint32_t s_aulRamBVBInput_DVI_1440x900p_60Hz[BVDC_P_RAM_TABLE_SIZE] =
{
	0x00a24000, /* 0 */
	0x00240050, /* 1 */
	0x00201098, /* 2 */
	0x002000df, /* 3 */
	0x00208593, /* 4 */
	0x00800000, /* 5 */
	0x00000000, /* 6 */
	0x00000000, /* 7 */
	0x00000000, /* 8 */
	0x00000000, /* 9 */
	0x00000000, /* 10 */
	0x00000000, /* 11 */
	0x00000000, /* 12 */
	0x00000000, /* 13 */
	0x00000000, /* 14 */
	0x00000000, /* 15 */
	0x00000000, /* 16 */
	0x00000000, /* 17 */
	0x00000000, /* 18 */
	0x00000000, /* 19 */
	0x00a80000, /* 20 */
	0x00421001, /* 21 */
	0x00401005, /* 22 */
	0x00400019, /* 23 */
	0x00408384, /* 24 */
	0x00a80000, /* 25 */
	0x00400003, /* 26 */
	0x00815000, /* 27 */
	0x00000000, /* 28' */
	0x00000000, /* 29' */
	0x00000000, /* 30' */
	0x00000000, /* 31' */
	0x00000000, /* 32' */
	0x00000000, /* 33' */
	0x00000000, /* 34' */
	0x00000000, /* 35' */
	0x00000000, /* 36' */
	0x00000000, /* 37' */
	0x00000000, /* 38' */
	0x00000000, /* 39' */
	0x00000000, /* 40' */
	0x00000000, /* 41' */
	0x00000000, /* 42' */
	0x00000000, /* 43' */
	0x00000000, /* 44' */
	0x00000000, /* 45' */
	0x00000000, /* 46' */
	0x00000000, /* 47' */
	0x00000000, /* 48' */
	0x00000000, /* 49' */
	0x00000000, /* 50' */
	0x00000000, /* 51' */
	0x00000000, /* 52' */
	0x00000000, /* 53' */
	0x00000000, /* 54' */
	0x00000000, /* 55' */
	0x00000000, /* 56' */
	0x00000000, /* 57' */
	0x00000000, /* 58' */
	0x00000000, /* 59' */
	0x00000000, /* 60' */
	0x00000000, /* 61' */
	0x00000000, /* 62' */
	0x00000000, /* 63' */
	0x00000000, /* 64' */
	0x00000000, /* 65' */
	0x00000000, /* 66' */
	0x00000000, /* 67' */
	0x00000000, /* 68' */
	0x00000000, /* 69' */
	0x00000000, /* 70' */
	0x00000000, /* 71' */
	0x00000000, /* 72' */
	0x00000000, /* 73' */
	0x00000000, /* 74' */
	0x00000000, /* 75' */
	0x00000000, /* 76' */
	0x00000000, /* 77' */
	0x00000000, /* 78' */
	0x00000000, /* 79' */
	0x00000000, /* 80' */
	0x00000000, /* 81' */
	0x00000000, /* 82' */
	0x00000000, /* 83' */
	0x00000000, /* 84' */
	0x00000000, /* 85' */
	0x00000000, /* 86' */
	0x00000000, /* 87' */
	0x00000000, /* 88' */
	0x00000000, /* 89' */
	0x00000000, /* 90' */
	0x00000000, /* 91' */
	0x00000000, /* 92' */
	0x00000000, /* 93' */
	0x00000000, /* 94' */
	0x00000000, /* 95' */
	0x00000000, /* 96' */
	0x00000000, /* 97' */
	0x00000000, /* 98' */
	0x00000000, /* 99' */
	0x00000000, /* 100' */
	0x00000000, /* 101' */
	0x00000000, /* 102' */
	0x00000000, /* 103' */
	0x00000000, /* 104' */
	0x00000000, /* 105' */
	0x00000000, /* 106' */
	0x00000000, /* 107' */
	0x00000000, /* 108' */
	0x00000000, /* 109' */
	0x00000000, /* 110' */
	0x00000000, /* 111' */
	0x00000000, /* 112' */
	0x00000000, /* 113' */
	0x00000000, /* 114' */
	0x00000000, /* 115' */
	0x00000000, /* 116' */
	0x00000000, /* 117' */
	0x00000000, /* 118' */
	0x00000000, /* 119' */
	0x00000000, /* 120' */
	0x00000000, /* 121' */
	0x00000000, /* 122' */
	0x00000000, /* 123' */
	0x00000000, /* 124' */
	0x00000000, /* 125' */
	0x00000000, /* 126' */
	0x00000000, /* 127' */
	0x00000000, /* 128' */
	0x00000000, /* 129' */
	0x00000000, /* 130' */
	0x00000000, /* 131' */
	0x00000000, /* 132' */
	0x00000000, /* 133' */
	0x00000000, /* 134' */
	0x00000000, /* 135' */
	0x00000000, /* 136' */
	0x00000000, /* 137' */
	0x00000000, /* 138' */
	0x00000000, /* 139' */
	0x00000000, /* 140' */
	0x00000000, /* 141' */
	0x00000000, /* 142' */
	0x00000000, /* 143' */
	0x00000000, /* 144' */
	0x00000000, /* 145' */
	0x00000000, /* 146' */
	0x00000000, /* 147' */
	0x00000000, /* 148' */
	0x00000000, /* 149' */
	0x00000000, /* 150' */
	0x00000000, /* 151' */
	0x00000000, /* 152' */
	0x00000000, /* 153' */
	0x00000000, /* 154' */
	0x00000000, /* 155' */
	0x00000000, /* 156' */
	0x00000000, /* 157' */
	0x00000000, /* 158' */
	0x00000000, /* 159' */
	0x00000000, /* 160' */
	0x00000000, /* 161' */
	0x00000000, /* 162' */
	0x00000000, /* 163' */
	0x00000000, /* 164' */
	0x00000000, /* 165' */
	0x00000000, /* 166' */
	0x00000000, /* 167' */
	0x00000000, /* 168' */
	0x00000000, /* 169' */
	0x00000000, /* 170' */
	0x00000000, /* 171' */
	0x00000000, /* 172' */
	0x00000000, /* 173' */
	0x00000000, /* 174' */
	0x00000000, /* 175' */
	0x00000000, /* 176' */
	0x00000000, /* 177' */
	0x00000000, /* 178' */
	0x00000000, /* 179' */
	0x00000000, /* 180' */
	0x00000000, /* 181' */
	0x00000000, /* 182' */
	0x00000000, /* 183' */
	0x00000000, /* 184' */
	0x00000000, /* 185' */
	0x00000000, /* 186' */
	0x00000000, /* 187' */
	0x00000000, /* 188' */
	0x00000000, /* 189' */
	0x00000000, /* 190' */
	0x00000000, /* 191' */
	0x00000000, /* 192' */
	0x00000000, /* 193' */
	0x00000000, /* 194' */
	0x00000000, /* 195' */
	0x00000000, /* 196' */
	0x00000000, /* 197' */
	0x00000000, /* 198' */
	0x00000000, /* 199' */
	0x00000000, /* 200' */
	0x00000000, /* 201' */
	0x00000000, /* 202' */
	0x00000000, /* 203' */
	0x00000000, /* 204' */
	0x00000000, /* 205' */
	0x00000000, /* 206' */
	0x00000000, /* 207' */
	0x00000000, /* 208' */
	0x00000000, /* 209' */
	0x00000000, /* 210' */
	0x00000000, /* 211' */
	0x00000000, /* 212' */
	0x00000000, /* 213' */
	0x00000000, /* 214' */
	0x00000000, /* 215' */
	0x00000000, /* 216' */
	0x00000000, /* 217' */
	0x00000000, /* 218' */
	0x00000000, /* 219' */
	0x00000000, /* 220' */
	0x00000000, /* 221' */
	0x00000000, /* 222' */
	0x00000000, /* 223' */
	0x00000000, /* 224' */
	0x00000000, /* 225' */
	0x00000000, /* 226' */
	0x00000000, /* 227' */
	0x00000000, /* 228' */
	0x00000000, /* 229' */
	0x00000000, /* 230' */
	0x00000000, /* 231' */
	0x00000000, /* 232' */
	0x00000000, /* 233' */
	0x00000000, /* 234' */
	0x00000000, /* 235' */
	0x00000000, /* 236' */
	0x00000000, /* 237' */
	0x00000000, /* 238' */
	0x00000000, /* 239' */
	0x00000000, /* 240' */
	0x00000000, /* 241' */
	0x00000000, /* 242' */
	0x00000000, /* 243' */
	0x00000000, /* 244' */
	0x00000000, /* 245' */
	0x00000000, /* 246' */
	0x00000000, /* 247' */
	0x00000000, /* 248' */
	0x00000000, /* 249' */
	0x00000000, /* 250' */
	0x00000000, /* 251' */
	0x00000000, /* 252' */
	0x008fd000, /* 253 */
	0x00090109, /* 254 */
	0x00c5c5c7, /* 255 */
};


/* Dtram(0..3f) values for NTSC 656 output */
static const uint32_t s_aulDtRamBVBInput_656_NTSC[BVDC_P_DTRAM_TABLE_SIZE] =
{
	0x0060c001, /* 0 */
	0x00610002, /* 1 */
	0x0061c010, /* 2 */
	0x00620002, /* 3 */
	0x006240f0, /* 4 */
	0x00620002, /* 5 */
	0x0061c002, /* 6 */
	0x00610011, /* 7 */
	0x00614002, /* 8 */
	0x006280f0, /* 9 */
	0x00618001, /* 10 */
	0x00800000, /* 11 */
	0x0028c026, /* 12 */
	0x0029c07c, /* 13 */
	0x0020c072, /* 14 */
	0x0030d595, /* 15 */
	0x0020c026, /* 16 */
	0x0021c07c, /* 17 */
	0x0020c072, /* 18 */
	0x0030d595, /* 19 */
	0x00208026, /* 20 */
	0x0021807c, /* 21 */
	0x00208072, /* 22 */
	0x00309595, /* 23 */
	0x00208026, /* 24 */
	0x0021807c, /* 25 */
	0x00208072, /* 26 */
	0x0030958a, /* 27 */
	0x00204026, /* 28 */
	0x0021407c, /* 29 */
	0x00204072, /* 30 */
	0x00305595, /* 31 */
	0x00200026, /* 32 */
	0x0021007c, /* 33 */
	0x00200072, /* 34 */
	0x00301595, /* 35 */
	0x00202026, /* 36 */
	0x0021207c, /* 37 */
	0x00202072, /* 38 */
	0x00303595, /* 39 */
	0x0020a026, /* 40 */
	0x0021a07c, /* 41 */
	0x0020a072, /* 42 */
	0x0030b595, /* 43 */
	0x00000000, /* 44 */
	0x00000000, /* 45 */
	0x00000000, /* 46 */
	0x00000000, /* 47 */
	0x00000000, /* 48 */
	0x00000000, /* 49 */
	0x00000000, /* 50 */
	0x00000000, /* 51 */
	0x00000000, /* 52 */
	0x00000000, /* 53 */
	0x00000000, /* 54 */
	0x00000000, /* 55 */
	0x00000000, /* 56 */
	0x00000000, /* 57 */
	0x00000000, /* 58 */
	0x00000000, /* 59 */
	0x00000000, /* 60 */
	0x00000000, /* 61 */
	0x00091905, /* 62 */
	0xb4d00d76, /* 63 */
};

/* Dtram(0..3f) values for PAL(BB1D1GH) 656 output */
/* From vec_dit_pal_656.bss */
static const uint32_t s_aulDtRamBVBInput_656_PAL[BVDC_P_DTRAM_TABLE_SIZE] =
{
	0x0020400f, /* 0 */
	0x0060f001, /* 1 */
	0x00613014, /* 2 */
	0x0061b001, /* 3 */
	0x0061f11f, /* 4 */
	0x00623001, /* 5 */
	0x00613001, /* 6 */
	0x00617001, /* 7 */
	0x0062f016, /* 8 */
	0x00633001, /* 9 */
	0x0062711f, /* a */
	0x0062b001, /* b */
	0x0062f001, /* c */
	0x00637001, /* d */
	0x00800000, /* e */
	0x0029407c, /* f */
	0x0020408a, /* 10 */
	0x002055a0, /* 11 */
	0x0030400f, /* 12 */
	0x0021407c, /* 13 */
	0x0020408a, /* 14 */
	0x002055a0, /* 15 */
	0x0030400f, /* 16 */
	0x0021407c, /* 17 */
	0x0020408a, /* 18 */
	0x002055a0, /* 19 */
	0x0030c00f, /* 1a */
	0x0021407c, /* 1b */
	0x0020408a, /* 1c */
	0x002055a0, /* 1d */
	0x0030200f, /* 1e */
	0x0021207c, /* 1f */
	0x0020208a, /* 20 */
	0x002035a0, /* 21 */
	0x0030200f, /* 22 */
	0x0021207c, /* 23 */
	0x0020208a, /* 24 */
	0x002035a0, /* 25 */
	0x0030400f, /* 26 */
	0x0021a07c, /* 27 */
	0x0020a08a, /* 28 */
	0x0020b5a0, /* 29 */
	0x0030a00f, /* 2a */
	0x0021a07c, /* 2b */
	0x0020a08a, /* 2c */
	0x0020b5a0, /* 2d */
	0x0030c00f, /* 2e */
	0x0021c07c, /* 2f */
	0x0020c08a, /* 30 */
	0x0020d5a0, /* 31 */
	0x0030c00f, /* 32 */
	0x0021c07c, /* 33 */
	0x0020c08a, /* 34 */
	0x0020d5a0, /* 35 */
	0x0030a00f, /* 36 */
	0x0021c07c, /* 37 */
	0x0020c08a, /* 38 */
	0x0030d595, /* 39 */
	0x00000000, /* 3a */
	0x00000000, /* 3b */
	0x00000000, /* 3c */
	0x00000000, /* 3d */
	0x00101107, /* 3e */
	0x0718d8bc, /* 3f */
};

/* Dtram values for Custom 1600x1200p */
static const uint32_t s_aulDtRamBVBInput_1600x1200p_60Hz[BVDC_P_DTRAM_TABLE_SIZE] =
{
	0x00647001, /* 64 */
	0x0064b002, /* 65 */
	0x0064f001, /* 66 */
	0x0065302d, /* 67 */
	0x0065b4b0, /* 68 */
	0x00657001, /* 69 */
	0x00840000, /* 70 */
	0x00204040, /* 71 */
	0x002d40c0, /* 72 */
	0x00244130, /* 73 */
	0x00345635, /* 74 */
	0x00244040, /* 75 */
	0x002540c0, /* 76 */
	0x00244130, /* 77 */
	0x00345635, /* 78 */
	0x00244040, /* 79 */
	0x002140c0, /* 80 */
	0x00204130, /* 81 */
	0x00305635, /* 82 */
	0x00204040, /* 83 */
	0x002140c0, /* 84 */
	0x00204130, /* 85 */
	0x00305635, /* 86 */
	0x00204040, /* 87 */
	0x002140c0, /* 88 */
	0x00204130, /* 89 */
	0x0030562a, /* 90 */
	0x00202040, /* 91 */
	0x002120c0, /* 92 */
	0x00202130, /* 93 */
	0x00323635, /* 94 */
	0x00202040, /* 95 */
	0x002120c0, /* 96 */
	0x00202130, /* 97 */
	0x0032362a, /* 98 */
	0x00000000, /* 99 */
	0x00000000, /* 100 */
	0x00000000, /* 101 */
	0x00000000, /* 102 */
	0x00000000, /* 103 */
	0x00000000, /* 104 */
	0x00000000, /* 105 */
	0x00000000, /* 106 */
	0x00000000, /* 107 */
	0x00000000, /* 108 */
	0x00000000, /* 109 */
	0x00000000, /* 110 */
	0x00000000, /* 111 */
	0x00000000, /* 112 */
	0x00000000, /* 113 */
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
	0x00092106, /* 126 */
	0x5e9229ba, /* 127 */
};

/* IT_Microcode for 1360x768p_60Hz DVI mode */
static const uint32_t
	s_aulDtRamBVBInput_DVI_1360x768p[BVDC_P_DTRAM_TABLE_SIZE] =
{
	0x00662001, /* 40 */
	0x0064e005, /* 41 */
	0x00652001, /* 42 */
	0x00656011, /* 43 */
	0x0065a300, /* 44 */
	0x00656002, /* 45 */
	0x00666001, /* 46 */
	0x00840000, /* 47 */
	0x00000000, /* 48 */
	0x00000000, /* 49 */
	0x00200040, /* 4a */
	0x00250070, /* 4b */
	0x00240100, /* 4c */
	0x00341545, /* 4d */
	0x00240040, /* 4e */
	0x00250070, /* 4f */
	0x00240100, /* 50 */
	0x00341545, /* 51 */
	0x00240040, /* 52 */
	0x00210070, /* 53 */
	0x00200100, /* 54 */
	0x00301545, /* 55 */
	0x00200040, /* 56 */
	0x00210070, /* 57 */
	0x00200100, /* 58 */
	0x00301545, /* 59 */
	0x00202040, /* 5a */
	0x00212070, /* 5b */
	0x00202100, /* 5c */
	0x00323545, /* 5d */
	0x00200040, /* 5e */
	0x00290070, /* 5f */
	0x00200100, /* 60 */
	0x00301545, /* 61 */
	0x00200040, /* 62 */
	0x002d0070, /* 63 */
	0x00240100, /* 64 */
	0x00341545, /* 65 */
	0x00200040, /* 66 */
	0x00210070, /* 67 */
	0x00200100, /* 68 */
	0x0030153a, /* 69 */
	0x00000000, /* 106' */
	0x00000000, /* 107' */
	0x00000000, /* 108' */
	0x00000000, /* 109' */
	0x00000000, /* 110' */
	0x00000000, /* 111' */
	0x00000000, /* 112' */
	0x00000000, /* 113' */
	0x00000000, /* 114' */
	0x00000000, /* 115' */
	0x00000000, /* 116' */
	0x00000000, /* 117' */
	0x00000000, /* 118' */
	0x00000000, /* 119' */
	0x00000000, /* 120' */
	0x00000000, /* 121' */
	0x00000000, /* 122' */
	0x00000000, /* 123' */
	0x00000000, /* 124' */
	0x008fd000, /* 7d */
	0x00090109, /* 7e */
	0x00281046, /* 7f */
};

/* Dtram values for 3840x2160p 24Hz */
/* DVI_MCODE_3840x2160pDCI_24Hz.bss */
static const uint32_t s_aulDtRamBVBInput_3840x2160p_24Hz[BVDC_P_DTRAM_TABLE_SIZE] =
{
	0x00673001, /* 40 */
	0x0065f009, /* 41 */
	0x00663001, /* 42 */
	0x00667047, /* 43 */
	0x0066b870, /* 44 */
	0x00667007, /* 45 */
	0x00677001, /* 46 */
	0x00840000, /* 47 */
	0x0066f001, /* 48 */
	0x00667000, /* 49 */
	0x0065b001, /* 4a */
	0x0065f009, /* 4b */
	0x00663001, /* 4c */
	0x00667047, /* 4d */
	0x0066b870, /* 4e */
	0x00677001, /* 4f */
	0x00848000, /* 50 */
	0x0066f001, /* 51 */
	0x00667000, /* 52 */
	0x0065b001, /* 53 */
	0x0065f009, /* 54 */
	0x00663001, /* 55 */
	0x00667047, /* 56 */
	0x0066b870, /* 57 */
	0x00667007, /* 58 */
	0x00677001, /* 59 */
	0x00851000, /* 5a */
	0x002004fc, /* 5b */
	0x00250058, /* 5c */
	0x00240128, /* 5d */
	0x00341ef5, /* 5e */
	0x002404fc, /* 5f */
	0x00250058, /* 60 */
	0x00240128, /* 61 */
	0x00341ef5, /* 62 */
	0x002404fc, /* 63 */
	0x00210058, /* 64 */
	0x00200128, /* 65 */
	0x00301ef5, /* 66 */
	0x002004fc, /* 67 */
	0x00210058, /* 68 */
	0x00200128, /* 69 */
	0x00301ef5, /* 6a */
	0x002024fc, /* 6b */
	0x00212058, /* 6c */
	0x00202128, /* 6d */
	0x00323ef5, /* 6e */
	0x002004fc, /* 6f */
	0x00290058, /* 70 */
	0x00200128, /* 71 */
	0x00301ef5, /* 72 */
	0x002004fc, /* 73 */
	0x002d0058, /* 74 */
	0x00240128, /* 75 */
	0x00341ef5, /* 76 */
	0x002004fc, /* 77 */
	0x00210058, /* 78 */
	0x00200128, /* 79 */
	0x00301eea, /* 7a */
	0xfeff0b08, /* 7b */
	0x00000001, /* 7c */
	0x008fd000, /* 7d */
	0x00091211, /* 7e */
	0x001d80f2, /* 7f */
};


/* Dtram values for 3840x2160p 24Hz */
/* DVI_MCODE_4096x2160pSMPTE_24Hz.bss */
static const uint32_t s_aulDtRamBVBInput_4096x2160p_24Hz[BVDC_P_DTRAM_TABLE_SIZE] =
{
	0x00673001, /* 40 */
	0x0065f009, /* 41 */
	0x00663001, /* 42 */
	0x00667047, /* 43 */
	0x0066b870, /* 44 */
	0x00667007, /* 45 */
	0x00677001, /* 46 */
	0x00840000, /* 47 */
	0x0066f001, /* 48 */
	0x00667000, /* 49 */
	0x0065b001, /* 4a */
	0x0065f009, /* 4b */
	0x00663001, /* 4c */
	0x00667047, /* 4d */
	0x0066b870, /* 4e */
	0x00677001, /* 4f */
	0x00848000, /* 50 */
	0x0066f001, /* 51 */
	0x00667000, /* 52 */
	0x0065b001, /* 53 */
	0x0065f009, /* 54 */
	0x00663001, /* 55 */
	0x00667047, /* 56 */
	0x0066b870, /* 57 */
	0x00667007, /* 58 */
	0x00677001, /* 59 */
	0x00851000, /* 5a */
	0x002003fc, /* 5b */
	0x00250058, /* 5c */
	0x00240128, /* 5d */
	0x00341ff5, /* 5e */
	0x002403fc, /* 5f */
	0x00250058, /* 60 */
	0x00240128, /* 61 */
	0x00341ff5, /* 62 */
	0x002403fc, /* 63 */
	0x00210058, /* 64 */
	0x00200128, /* 65 */
	0x00301ff5, /* 66 */
	0x002003fc, /* 67 */
	0x00210058, /* 68 */
	0x00200128, /* 69 */
	0x00301ff5, /* 6a */
	0x002023fc, /* 6b */
	0x00212058, /* 6c */
	0x00202128, /* 6d */
	0x00323ff5, /* 6e */
	0x002003fc, /* 6f */
	0x00290058, /* 70 */
	0x00200128, /* 71 */
	0x00301ff5, /* 72 */
	0x002003fc, /* 73 */
	0x002d0058, /* 74 */
	0x00240128, /* 75 */
	0x00341ff5, /* 76 */
	0x002003fc, /* 77 */
	0x00210058, /* 78 */
	0x00200128, /* 79 */
	0x00301fea, /* 7a */
	0xfeff0b08, /* 7b */
	0x00000001, /* 7c */
	0x008fd000, /* 7d */
	0x00031312, /* 7e */
	0x001d80f2, /* 7f */
};

/* Dtram values for Custom 1366x768p */
static const uint32_t s_aulDtRamBVBInput_CUSTOM_1366x768p[BVDC_P_DTRAM_TABLE_SIZE] =
{
	0x00647001, /* 40 */
	0x0064a004, /* 41 */
	0x0064c007, /* 42 */
	0x0064e300, /* 43 */
	0x0064c001, /* 44 */
	0x00652001, /* 45 */
	0x00840000, /* 46 */
	0x002d4017, /* 47 */
	0x00284010, /* 48 */
	0x00314585, /* 49 */
	0x00204010, /* 4a */
	0x00314585, /* 4b */
	0x00244010, /* 4c */
	0x00354585, /* 4d */
	0x00242010, /* 4e */
	0x00252023, /* 4f */
	0x00273556, /* 50 */
	0x0035200c, /* 51 */
	0x00244010, /* 52 */
	0x00354563, /* 53 */
	0x00000000, /* 54 */
	0x00000000, /* 55 */
	0x00000000, /* 56 */
	0x00000000, /* 57 */
	0x00000000, /* 58 */
	0x00000000, /* 59 */
	0x00000000, /* 5a */
	0x00000000, /* 5b */
	0x00000000, /* 5c */
	0x00000000, /* 5d */
	0x00000000, /* 5e */
	0x00000000, /* 5f */
	0x00000000, /* 60 */
	0x00000000, /* 61 */
	0x00000000, /* 62 */
	0x00000000, /* 63 */
	0x00000000, /* 64 */
	0x00000000, /* 65 */
	0x00000000, /* 66 */
	0x00000000, /* 67 */
	0x00000000, /* 68 */
	0x00000000, /* 69 */
	0x00000000, /* 6a */
	0x00000000, /* 6b */
	0x00000000, /* 6c */
	0x00000000, /* 6d */
	0x00000000, /* 6e */
	0x00000000, /* 6f */
	0x00000000, /* 70 */
	0x00000000, /* 71 */
	0x00000000, /* 72 */
	0x00000000, /* 73 */
	0x00000000, /* 74 */
	0x00000000, /* 75 */
	0x00000000, /* 76 */
	0x00000000, /* 77 */
	0x00000000, /* 78 */
	0x00000000, /* 79 */
	0x00000000, /* 7a */
	0x00000000, /* 7b */
	0x00000000, /* 7c */
	0x00000000, /* 7d */
	0x00022105, /* 7e */
	0x43a03a13	/* 7f */
};

/* Dtram values for 1440x900 DVI */
static const uint32_t
	s_aulDtRamBVBInput_DVI_1440x900p[BVDC_P_DTRAM_TABLE_SIZE] =
{
	0x00662001, /* 40 */
	0x0064e005, /* 41 */
	0x00652001, /* 42 */
	0x00656018, /* 43 */
	0x0065a384, /* 44 */
	0x00656002, /* 45 */
	0x00666001, /* 46 */
	0x00840000, /* 47 */
	0x00000000, /* 48 */
	0x00000000, /* 49 */
	0x00200050, /* 4a */
	0x00250098, /* 4b */
	0x002400e8, /* 4c */
	0x00341595, /* 4d */
	0x00240050, /* 4e */
	0x00250098, /* 4f */
	0x002400e8, /* 50 */
	0x00341595, /* 51 */
	0x00240050, /* 52 */
	0x00210098, /* 53 */
	0x002000e8, /* 54 */
	0x00301595, /* 55 */
	0x00200050, /* 56 */
	0x00210098, /* 57 */
	0x002000e8, /* 58 */
	0x00301595, /* 59 */
	0x00202050, /* 5a */
	0x00212098, /* 5b */
	0x002020e8, /* 5c */
	0x00323595, /* 5d */
	0x00200050, /* 5e */
	0x00290098, /* 5f */
	0x002000e8, /* 60 */
	0x00301595, /* 61 */
	0x00200050, /* 62 */
	0x002d0098, /* 63 */
	0x002400e8, /* 64 */
	0x00341595, /* 65 */
	0x00200050, /* 66 */
	0x00210098, /* 67 */
	0x002000e8, /* 68 */
	0x0030158a, /* 69 */
	0x00000000, /* 106' */
	0x00000000, /* 107' */
	0x00000000, /* 108' */
	0x00000000, /* 109' */
	0x00000000, /* 110' */
	0x00000000, /* 111' */
	0x00000000, /* 112' */
	0x00000000, /* 113' */
	0x00000000, /* 114' */
	0x00000000, /* 115' */
	0x00000000, /* 116' */
	0x00000000, /* 117' */
	0x00000000, /* 118' */
	0x00000000, /* 119' */
	0x00000000, /* 120' */
	0x00000000, /* 121' */
	0x00000000, /* 122' */
	0x00000000, /* 123' */
	0x00000000, /* 124' */
	0x008fd000, /* 7d */
	0x00090109, /* 7e */
	0x00281451, /* 7f */
};

/* Dtram values for 640x480p DVI */
static const uint32_t s_aulDtRamBVBInput_DVI_640x480p[BVDC_P_DTRAM_TABLE_SIZE] =
{
	0x00662001, /* 40 */
	0x0064e001, /* 41 */
	0x00652001, /* 42 */
	0x00656020, /* 43 */
	0x0065a1e0, /* 44 */
	0x00656009, /* 45 */
	0x00666001, /* 46 */
	0x00840000, /* 47 */
	0x00000000, /* 48 */
	0x00000000, /* 49 */
	0x00200010, /* 4a */
	0x00250060, /* 4b */
	0x00240030, /* 4c */
	0x00341275, /* 4d */
	0x00240010, /* 4e */
	0x00250060, /* 4f */
	0x00240030, /* 50 */
	0x00341275, /* 51 */
	0x00240010, /* 52 */
	0x00210060, /* 53 */
	0x00200030, /* 54 */
	0x00301275, /* 55 */
	0x00200010, /* 56 */
	0x00210060, /* 57 */
	0x00200030, /* 58 */
	0x00301275, /* 59 */
	0x00202010, /* 5a */
	0x00212060, /* 5b */
	0x00202030, /* 5c */
	0x00323275, /* 5d */
	0x00200010, /* 5e */
	0x00290060, /* 5f */
	0x00200030, /* 60 */
	0x00301275, /* 61 */
	0x00200010, /* 62 */
	0x002d0060, /* 63 */
	0x00240030, /* 64 */
	0x00341275, /* 65 */
	0x00200010, /* 66 */
	0x00210060, /* 67 */
	0x00200030, /* 68 */
	0x0030126a, /* 69 */
	0x00000000, /* 106' */
	0x00000000, /* 107' */
	0x00000000, /* 108' */
	0x00000000, /* 109' */
	0x00000000, /* 110' */
	0x00000000, /* 111' */
	0x00000000, /* 112' */
	0x00000000, /* 113' */
	0x00000000, /* 114' */
	0x00000000, /* 115' */
	0x00000000, /* 116' */
	0x00000000, /* 117' */
	0x00000000, /* 118' */
	0x00000000, /* 119' */
	0x00000000, /* 120' */
	0x00000000, /* 121' */
	0x00000000, /* 122' */
	0x00000000, /* 123' */
	0x00000000, /* 124' */
	0x008fd000, /* 7d */
	0x00090109, /* 7e */
	0x0027f038, /* 7f */
};

/* Dtram values for 800x600p DVI */
static const uint32_t s_aulDtRamBVBInput_DVI_800x600p[BVDC_P_DTRAM_TABLE_SIZE] =
{
	0x00662001, /* 40 */
	0x0064e003, /* 41 */
	0x00652001, /* 42 */
	0x00656016, /* 43 */
	0x0065a258, /* 44 */
	0x00656000, /* 45 */
	0x00666001, /* 46 */
	0x00840000, /* 47 */
	0x00000000, /* 48 */
	0x00000000, /* 49 */
	0x00200028, /* 4a */
	0x00250080, /* 4b */
	0x00240058, /* 4c */
	0x00341315, /* 4d */
	0x00240028, /* 4e */
	0x00250080, /* 4f */
	0x00240058, /* 50 */
	0x00341315, /* 51 */
	0x00240028, /* 52 */
	0x00210080, /* 53 */
	0x00200058, /* 54 */
	0x00301315, /* 55 */
	0x00200028, /* 56 */
	0x00210080, /* 57 */
	0x00200058, /* 58 */
	0x00301315, /* 59 */
	0x00202028, /* 5a */
	0x00212080, /* 5b */
	0x00202058, /* 5c */
	0x00323315, /* 5d */
	0x00200028, /* 5e */
	0x00290080, /* 5f */
	0x00200058, /* 60 */
	0x00301315, /* 61 */
	0x00200028, /* 62 */
	0x002d0080, /* 63 */
	0x00240058, /* 64 */
	0x00341315, /* 65 */
	0x00200028, /* 66 */
	0x00210080, /* 67 */
	0x00200058, /* 68 */
	0x0030130a, /* 69 */
	0x00000000, /* 106' */
	0x00000000, /* 107' */
	0x00000000, /* 108' */
	0x00000000, /* 109' */
	0x00000000, /* 110' */
	0x00000000, /* 111' */
	0x00000000, /* 112' */
	0x00000000, /* 113' */
	0x00000000, /* 114' */
	0x00000000, /* 115' */
	0x00000000, /* 116' */
	0x00000000, /* 117' */
	0x00000000, /* 118' */
	0x00000000, /* 119' */
	0x00000000, /* 120' */
	0x00000000, /* 121' */
	0x00000000, /* 122' */
	0x00000000, /* 123' */
	0x00000000, /* 124' */
	0x008fd000, /* 7d */
	0x00090109, /* 7e */
	0x0027f89f, /* 7f */
};

/* Dtram values for 1024x768p DVI */
static const uint32_t s_aulDtRamBVBInput_DVI_1024x768p[BVDC_P_DTRAM_TABLE_SIZE] =
{
	0x00662001, /* 40 */
	0x0064e005, /* 41 */
	0x00652001, /* 42 */
	0x0065601c, /* 43 */
	0x0065a300, /* 44 */
	0x00656002, /* 45 */
	0x00666001, /* 46 */
	0x00840000, /* 47 */
	0x00000000, /* 48 */
	0x00000000, /* 49 */
	0x00200018, /* 4a */
	0x00250088, /* 4b */
	0x002400a0, /* 4c */
	0x003413f5, /* 4d */
	0x00240018, /* 4e */
	0x00250088, /* 4f */
	0x002400a0, /* 50 */
	0x003413f5, /* 51 */
	0x00240018, /* 52 */
	0x00210088, /* 53 */
	0x002000a0, /* 54 */
	0x003013f5, /* 55 */
	0x00200018, /* 56 */
	0x00210088, /* 57 */
	0x002000a0, /* 58 */
	0x003013f5, /* 59 */
	0x00202018, /* 5a */
	0x00212088, /* 5b */
	0x002020a0, /* 5c */
	0x003233f5, /* 5d */
	0x00200018, /* 5e */
	0x00290088, /* 5f */
	0x002000a0, /* 60 */
	0x003013f5, /* 61 */
	0x00200018, /* 62 */
	0x002d0088, /* 63 */
	0x002400a0, /* 64 */
	0x003413f5, /* 65 */
	0x00200018, /* 66 */
	0x00210088, /* 67 */
	0x002000a0, /* 68 */
	0x003013ea, /* 69 */
	0x00000000, /* 106' */
	0x00000000, /* 107' */
	0x00000000, /* 108' */
	0x00000000, /* 109' */
	0x00000000, /* 110' */
	0x00000000, /* 111' */
	0x00000000, /* 112' */
	0x00000000, /* 113' */
	0x00000000, /* 114' */
	0x00000000, /* 115' */
	0x00000000, /* 116' */
	0x00000000, /* 117' */
	0x00000000, /* 118' */
	0x00000000, /* 119' */
	0x00000000, /* 120' */
	0x00000000, /* 121' */
	0x00000000, /* 122' */
	0x00000000, /* 123' */
	0x00000000, /* 124' */
	0x008fd000, /* 7d */
	0x00090109, /* 7e */
	0x00280251, /* 7f */
};

/* Dtram values for 1280x768p DVI */
static const uint32_t s_aulDtRamBVBInput_DVI_1280x768p[BVDC_P_DTRAM_TABLE_SIZE] =
{
	0x00651001, /* 64 */
	0x00659002, /* 65 */
	0x0020500b, /* 66 */
	0x00204040, /* 67 */
	0x00254080, /* 68 */
	0x002440c0, /* 69 */
	0x002454f5, /* 70 */
	0x00655006, /* 71 */
	0x0024500b, /* 72 */
	0x00244040, /* 73 */
	0x00214080, /* 74 */
	0x002040c0, /* 75 */
	0x002054f5, /* 76 */
	0x00659013, /* 77 */
	0x0065d2ff, /* 78 */
	0x00661001, /* 79 */
	0x00840000, /* 80 */
	0x00204040, /* 81 */
	0x00294080, /* 82 */
	0x002040c0, /* 83 */
	0x003054f5, /* 84 */
	0x00244040, /* 85 */
	0x00254080, /* 86 */
	0x002440c0, /* 87 */
	0x003454f5, /* 88 */
	0x00204040, /* 89 */
	0x00214080, /* 90 */
	0x002040c0, /* 91 */
	0x003054f5, /* 92 */
	0x00202040, /* 93 */
	0x00212080, /* 94 */
	0x002020c0, /* 95 */
	0x003234f5, /* 96 */
	0x00202040, /* 97 */
	0x00212080, /* 98 */
	0x002020c0, /* 99 */
	0x003234ea, /* 100 */
	0x00000000, /* 101 */
	0x00000000, /* 102 */
	0x00000000, /* 103 */
	0x00000000, /* 104 */
	0x00000000, /* 105 */
	0x00000000, /* 106 */
	0x00000000, /* 107 */
	0x00000000, /* 108 */
	0x00000000, /* 109 */
	0x00000000, /* 110 */
	0x00000000, /* 111 */
	0x00000000, /* 112 */
	0x00000000, /* 113 */
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
	0x00090704, /* 126 */
	0x798f0798  /* 127 */
};

/* Dtram values for 1280x720p 50Hz DVI */
static const uint32_t s_aulDtRamBVBInput_DVI_1280x720p_50Hz[BVDC_P_DTRAM_TABLE_SIZE] =
{
	0x00650001, /* 64 */
	0x0020500b, /* 65 */
	0x00204030, /* 66 */
	0x00254080, /* 67 */
	0x002440b0, /* 68 */
	0x002454f5, /* 69 */
	0x00654004, /* 70 */
	0x0024500b, /* 71 */
	0x00244030, /* 72 */
	0x00214080, /* 73 */
	0x002040b0, /* 74 */
	0x002054f5, /* 75 */
	0x0065800e, /* 76 */
	0x0065c2cf, /* 77 */
	0x00660001, /* 78 */
	0x00840000, /* 79 */
	0x00204030, /* 80 */
	0x00294080, /* 81 */
	0x002040b0, /* 82 */
	0x003054f5, /* 83 */
	0x00244030, /* 84 */
	0x00254080, /* 85 */
	0x002440b0, /* 86 */
	0x003454f5, /* 87 */
	0x00204030, /* 88 */
	0x00214080, /* 89 */
	0x002040b0, /* 90 */
	0x003054f5, /* 91 */
	0x00202030, /* 92 */
	0x00212080, /* 93 */
	0x002020b0, /* 94 */
	0x003234f5, /* 95 */
	0x00202030, /* 96 */
	0x00212080, /* 97 */
	0x002020b0, /* 98 */
	0x003234ea, /* 99 */
	0x00000000, /* 100 */
	0x00000000, /* 101 */
	0x00000000, /* 102 */
	0x00000000, /* 103 */
	0x00000000, /* 104 */
	0x00000000, /* 105 */
	0x00000000, /* 106 */
	0x00000000, /* 107 */
	0x00000000, /* 108 */
	0x00000000, /* 109 */
	0x00000000, /* 110 */
	0x00000000, /* 111 */
	0x00000000, /* 112 */
	0x00000000, /* 113 */
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
	0x00010405, /* 126 */
	0xde7e9668  /* 127 */
};

/* Dtram values for 1280x720p DVI */
static const uint32_t s_aulDtRamBVBInput_DVI_1280x720p[BVDC_P_DTRAM_TABLE_SIZE] =
{
	0x00662001, /* 40 */
	0x0064e004, /* 41 */
	0x00652001, /* 42 */
	0x00656013, /* 43 */
	0x0065a2d0, /* 44 */
	0x00656002, /* 45 */
	0x00666001, /* 46 */
	0x00840000, /* 47 */
	0x00000000, /* 48 */
	0x00000000, /* 49 */
	0x00200040, /* 4a */
	0x00250080, /* 4b */
	0x002400c0, /* 4c */
	0x003414f5, /* 4d */
	0x00240040, /* 4e */
	0x00250080, /* 4f */
	0x002400c0, /* 50 */
	0x003414f5, /* 51 */
	0x00240040, /* 52 */
	0x00210080, /* 53 */
	0x002000c0, /* 54 */
	0x003014f5, /* 55 */
	0x00200040, /* 56 */
	0x00210080, /* 57 */
	0x002000c0, /* 58 */
	0x003014f5, /* 59 */
	0x00202040, /* 5a */
	0x00212080, /* 5b */
	0x002020c0, /* 5c */
	0x003234f5, /* 5d */
	0x00200040, /* 5e */
	0x00290080, /* 5f */
	0x002000c0, /* 60 */
	0x003014f5, /* 61 */
	0x00200040, /* 62 */
	0x002d0080, /* 63 */
	0x002400c0, /* 64 */
	0x003414f5, /* 65 */
	0x00200040, /* 66 */
	0x00210080, /* 67 */
	0x002000c0, /* 68 */
	0x003014ea, /* 69 */
	0x00000000, /* 106' */
	0x00000000, /* 107' */
	0x00000000, /* 108' */
	0x00000000, /* 109' */
	0x00000000, /* 110' */
	0x00000000, /* 111' */
	0x00000000, /* 112' */
	0x00000000, /* 113' */
	0x00000000, /* 114' */
	0x00000000, /* 115' */
	0x00000000, /* 116' */
	0x00000000, /* 117' */
	0x00000000, /* 118' */
	0x00000000, /* 119' */
	0x00000000, /* 120' */
	0x00000000, /* 121' */
	0x00000000, /* 122' */
	0x00000000, /* 123' */
	0x00000000, /* 124' */
	0x008fd000, /* 7d */
	0x00090109, /* 7e */
	0x00280c17, /* 7f */
};

/* Dtram values for 1280x720p Reduced Blanking DVI */
static const uint32_t s_aulDtRamBVBInput_DVI_1280x720p_Red[BVDC_P_DTRAM_TABLE_SIZE] =
{
	0x00651001, /* 64 */
	0x00659001, /* 65 */
	0x0020500b, /* 66 */
	0x00204030, /* 67 */
	0x00254020, /* 68 */
	0x00244050, /* 69 */
	0x002454f5, /* 70 */
	0x00655004, /* 71 */
	0x0024500b, /* 72 */
	0x00244030, /* 73 */
	0x00214020, /* 74 */
	0x00204050, /* 75 */
	0x002054f5, /* 76 */
	0x0065900d, /* 77 */
	0x0065d2cf, /* 78 */
	0x00661001, /* 79 */
	0x00840000, /* 80 */
	0x00204030, /* 81 */
	0x00294020, /* 82 */
	0x00204050, /* 83 */
	0x003054f5, /* 84 */
	0x00244030, /* 85 */
	0x00254020, /* 86 */
	0x00244050, /* 87 */
	0x003454f5, /* 88 */
	0x00204030, /* 89 */
	0x00214020, /* 90 */
	0x00204050, /* 91 */
	0x003054f5, /* 92 */
	0x00202030, /* 93 */
	0x00212020, /* 94 */
	0x00202050, /* 95 */
	0x003234f5, /* 96 */
	0x00202030, /* 97 */
	0x00212020, /* 98 */
	0x00202050, /* 99 */
	0x003234ea, /* 100 */
	0x00000000, /* 101 */
	0x00000000, /* 102 */
	0x00000000, /* 103 */
	0x00000000, /* 104 */
	0x00000000, /* 105 */
	0x00000000, /* 106 */
	0x00000000, /* 107 */
	0x00000000, /* 108 */
	0x00000000, /* 109 */
	0x00000000, /* 110 */
	0x00000000, /* 111 */
	0x00000000, /* 112 */
	0x00000000, /* 113 */
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
	0x00010605, /* 126 */
	0xbf67cc37  /* 127 */
};


/* Dtram values for 1280x800p DVI */
static const uint32_t s_aulDtRamBVBInput_DVI_1280x800p[BVDC_P_DTRAM_TABLE_SIZE] =
{
	0x00662001, /* 40 */
	0x0064e005, /* 41 */
	0x00652001, /* 42 */
	0x00656015, /* 43 */
	0x0065a320, /* 44 */
	0x00656002, /* 45 */
	0x00666001, /* 46 */
	0x00840000, /* 47 */
	0x00000000, /* 48 */
	0x00000000, /* 49 */
	0x00200048, /* 4a */
	0x00250080, /* 4b */
	0x002400c8, /* 4c */
	0x003414f5, /* 4d */
	0x00240048, /* 4e */
	0x00250080, /* 4f */
	0x002400c8, /* 50 */
	0x003414f5, /* 51 */
	0x00240048, /* 52 */
	0x00210080, /* 53 */
	0x002000c8, /* 54 */
	0x003014f5, /* 55 */
	0x00200048, /* 56 */
	0x00210080, /* 57 */
	0x002000c8, /* 58 */
	0x003014f5, /* 59 */
	0x00202048, /* 5a */
	0x00212080, /* 5b */
	0x002020c8, /* 5c */
	0x003234f5, /* 5d */
	0x00200048, /* 5e */
	0x00290080, /* 5f */
	0x002000c8, /* 60 */
	0x003014f5, /* 61 */
	0x00200048, /* 62 */
	0x002d0080, /* 63 */
	0x002400c8, /* 64 */
	0x003414f5, /* 65 */
	0x00200048, /* 66 */
	0x00210080, /* 67 */
	0x002000c8, /* 68 */
	0x003014ea, /* 69 */
	0x00000000, /* 106' */
	0x00000000, /* 107' */
	0x00000000, /* 108' */
	0x00000000, /* 109' */
	0x00000000, /* 110' */
	0x00000000, /* 111' */
	0x00000000, /* 112' */
	0x00000000, /* 113' */
	0x00000000, /* 114' */
	0x00000000, /* 115' */
	0x00000000, /* 116' */
	0x00000000, /* 117' */
	0x00000000, /* 118' */
	0x00000000, /* 119' */
	0x00000000, /* 120' */
	0x00000000, /* 121' */
	0x00000000, /* 122' */
	0x00000000, /* 123' */
	0x00000000, /* 124' */
	0x008fd000, /* 7d */
	0x00090109, /* 7e */
	0x00280cea, /* 7f */
};


/* Dtram values for 1280x1024p DVI */
static const uint32_t s_aulDtRamBVBInput_DVI_1280x1024p[BVDC_P_DTRAM_TABLE_SIZE] =
{
	0x00662001, /* 40 */
	0x0064e002, /* 41 */
	0x00652001, /* 42 */
	0x00656025, /* 43 */
	0x0065a400, /* 44 */
	0x00656000, /* 45 */
	0x00666001, /* 46 */
	0x00840000, /* 47 */
	0x00000000, /* 48 */
	0x00000000, /* 49 */
	0x00200030, /* 4a */
	0x00250070, /* 4b */
	0x002400f8, /* 4c */
	0x003414f5, /* 4d */
	0x00240030, /* 4e */
	0x00250070, /* 4f */
	0x002400f8, /* 50 */
	0x003414f5, /* 51 */
	0x00240030, /* 52 */
	0x00210070, /* 53 */
	0x002000f8, /* 54 */
	0x003014f5, /* 55 */
	0x00200030, /* 56 */
	0x00210070, /* 57 */
	0x002000f8, /* 58 */
	0x003014f5, /* 59 */
	0x00202030, /* 5a */
	0x00212070, /* 5b */
	0x002020f8, /* 5c */
	0x003234f5, /* 5d */
	0x00200030, /* 5e */
	0x00290070, /* 5f */
	0x002000f8, /* 60 */
	0x003014f5, /* 61 */
	0x00200030, /* 62 */
	0x002d0070, /* 63 */
	0x002400f8, /* 64 */
	0x003414f5, /* 65 */
	0x00200030, /* 66 */
	0x00210070, /* 67 */
	0x002000f8, /* 68 */
	0x003014ea, /* 69 */
	0x00000000, /* 106' */
	0x00000000, /* 107' */
	0x00000000, /* 108' */
	0x00000000, /* 109' */
	0x00000000, /* 110' */
	0x00000000, /* 111' */
	0x00000000, /* 112' */
	0x00000000, /* 113' */
	0x00000000, /* 114' */
	0x00000000, /* 115' */
	0x00000000, /* 116' */
	0x00000000, /* 117' */
	0x00000000, /* 118' */
	0x00000000, /* 119' */
	0x00000000, /* 120' */
	0x00000000, /* 121' */
	0x00000000, /* 122' */
	0x00000000, /* 123' */
	0x00000000, /* 124' */
	0x008fd000, /* 7d */
	0x00090109, /* 7e */
	0x00280e15, /* 7f */
};

/* Dtram values for 480i DVI, drop 0 lines from compositor */
/* From vec_dit_mc.bss */
static const uint32_t
s_aulDtRamBVBInput_DVI_480i_Drop0[BVDC_P_DTRAM_TABLE_SIZE] =
{
	0x00000000, /* 40 */
	0x0064f001, /* 41 */
	0x00653002, /* 42 */
	0x0066b003, /* 43 */
	0x0066300f, /* 44 */
	0x0066d0f0, /* 45 */
	0x00667002, /* 46 */
	0x00663002, /* 47 */
	0x0065d001, /* 48 */
	0x0065b002, /* 49 */
	0x00660001, /* 4a */
	0x0065300f, /* 4b */
	0x006710f0, /* 4c */
	0x00657001, /* 4d */
	0x00841000, /* 4e */
	0x0029807c, /* 4f */
	0x00288072, /* 50 */
	0x002095a0, /* 51 */
	0x0030801b, /* 52 */
	0x0021807c, /* 53 */
	0x00208072, /* 54 */
	0x002095a0, /* 55 */
	0x0030801b, /* 56 */
	0x0021807c, /* 57 */
	0x00208072, /* 58 */
	0x002095a0, /* 59 */
	0x00308010, /* 5a */
	0x0025807c, /* 5b */
	0x0034862d, /* 5c */
	0x0021807c, /* 5d */
	0x002082de, /* 5e */
	0x0034834f, /* 5f */
	0x0025807c, /* 60 */
	0x002482de, /* 61 */
	0x0030834f, /* 62 */
	0x0021007c, /* 63 */
	0x00200072, /* 64 */
	0x002015a0, /* 65 */
	0x0030001b, /* 66 */
	0x0021007c, /* 67 */
	0x00200072, /* 68 */
	0x002015a0, /* 69 */
	0x0030001b, /* 6a */
	0x0025007c, /* 6b */
	0x0034062d, /* 6c */
	0x0021207c, /* 6d */
	0x00202072, /* 6e */
	0x002235a0, /* 6f */
	0x0030201b, /* 70 */
	0x0021a07c, /* 71 */
	0x0020a072, /* 72 */
	0x0022b5a0, /* 73 */
	0x0030a01b, /* 74 */
	0x00000000, /* 75 */
	0x00000000, /* 76 */
	0x00000000, /* 77 */
	0x00000000, /* 78 */
	0x00000000, /* 79 */
	0x00000000, /* 7a */
	0x00000000, /* 7b */
	0x00000000, /* 7c */
	0x00000000, /* 7d */
	0x00062209, /* 7e */
	0xa0d975d1, /* 7f */
};

/* Dtram values for 480i DVI, drop 1 line from compositor */
/* From vec_dit_ntscj_dvi.bss */
static const uint32_t
s_aulDtRamBVBInput_DVI_480i_Drop1[BVDC_P_DTRAM_TABLE_SIZE] =
{
	0x00000000, /* 40 */
	0x00650001, /* 41 */
	0x00654003, /* 42 */
	0x00664003, /* 43 */
	0x0066000e, /* 44 */
	0x00666001, /* 45 */
	0x0066a0f0, /* 46 */
	0x00660004, /* 47 */
	0x0065a001, /* 48 */
	0x00658002, /* 49 */
	0x0065d001, /* 4a */
	0x0065400e, /* 4b */
	0x00672001, /* 4c */
	0x006760ef, /* 4d */
	0x0066e001, /* 4e */
	0x00841000, /* 4f */
	0x0029807c, /* 50 */
	0x00208072, /* 51 */
	0x002095a0, /* 52 */
	0x0030801b, /* 53 */
	0x0021807c, /* 54 */
	0x00208072, /* 55 */
	0x002095a0, /* 56 */
	0x0030801b, /* 57 */
	0x0025807c, /* 58 */
	0x0034862d, /* 59 */
	0x0021807c, /* 5a */
	0x002082de, /* 5b */
	0x0034834f, /* 5c */
	0x0025807c, /* 5d */
	0x002482de, /* 5e */
	0x0030834f, /* 5f */
	0x0021007c, /* 60 */
	0x00200072, /* 61 */
	0x002015a0, /* 62 */
	0x0030001b, /* 63 */
	0x0025007c, /* 64 */
	0x0034062d, /* 65 */
	0x0021207c, /* 66 */
	0x00202072, /* 67 */
	0x002035a0, /* 68 */
	0x0030201b, /* 69 */
	0x0021207c, /* 6a */
	0x00202072, /* 6b */
	0x002235a0, /* 6c */
	0x0030201b, /* 6d */
	0x0021a07c, /* 6e */
	0x0020a072, /* 6f */
	0x0022b5a0, /* 70 */
	0x0030a010, /* 71 */
	0x0021a07c, /* 72 */
	0x0020a072, /* 73 */
	0x0020b5a0, /* 74 */
	0x0030a01b, /* 75 */
	0x0021a07c, /* 76 */
	0x0020a072, /* 77 */
	0x0022b5a0, /* 78 */
	0x0030a01b, /* 79 */
	0x00000000, /* 7a */
	0x00000000, /* 7b */
	0x00000000, /* 7c */
	0x00000000, /* 7d */
	0x00052110, /* 7e */
	0x69f17823, /* 7f */
};

/* Dtram(0..3f) values for 576i(BB1D1GH) DVI */
/* From vec_dit_pal_mc.bss */
static const uint32_t s_aulDtRamBVBInput_DVI_576i[BVDC_P_DTRAM_TABLE_SIZE] =
{
	0x002d007e, /* 40 */
	0x00667001, /* 41 */
	0x00665002, /* 42 */
	0x00661013, /* 43 */
	0x0066a120, /* 44 */
	0x00661002, /* 45 */
	0x00659001, /* 46 */
	0x00657002, /* 47 */
	0x0065d001, /* 48 */
	0x0064e013, /* 49 */
	0x0066e120, /* 4a */
	0x0064e001, /* 4b */
	0x00652001, /* 4c */
	0x00841000, /* 4d */
	0x0021807e, /* 4e */
	0x0020808a, /* 4f */
	0x002095a0, /* 50 */
	0x0030800d, /* 51 */
	0x0021807e, /* 52 */
	0x0020808a, /* 53 */
	0x002095a0, /* 54 */
	0x00208018, /* 55 */
	0x003d0068, /* 56 */
	0x0025807e, /* 57 */
	0x00348637, /* 58 */
	0x0021807e, /* 59 */
	0x002082e2, /* 5a */
	0x00248348, /* 5b */
	0x0034800d, /* 5c */
	0x0025807e, /* 5d */
	0x002482e2, /* 5e */
	0x00208348, /* 5f */
	0x0030800d, /* 60 */
	0x0021007e, /* 61 */
	0x0020008a, /* 62 */
	0x002015a0, /* 63 */
	0x0030000d, /* 64 */
	0x0025007e, /* 65 */
	0x00340637, /* 66 */
	0x002c008a, /* 67 */
	0x002415a0, /* 68 */
	0x0034000d, /* 69 */
	0x0021207e, /* 6a */
	0x0020208a, /* 6b */
	0x002235a0, /* 6c */
	0x0030200d, /* 6d */
	0x0021a07e, /* 6e */
	0x0020a08a, /* 6f */
	0x0022b5a0, /* 70 */
	0x0030a00d, /* 71 */
	0x00000000, /* 72 */
	0x00000000, /* 73 */
	0x00000000, /* 74 */
	0x00000000, /* 75 */
	0x00000000, /* 76 */
	0x00000000, /* 77 */
	0x00000000, /* 78 */
	0x00000000, /* 79 */
	0x00000000, /* 7a */
	0x00000000, /* 7b */
	0x00000000, /* 7c */
	0x00000000, /* 7d */
	0x00062409, /* 7e */
	0xc91050e3, /* 7f */
};

/* Dtram(0..3f) values for 576i(BB1D1GH) DVI, OLD MICROCODE */
static const uint32_t s_aulDtRamBVBInput_DVI_576i_OLD[BVDC_P_DTRAM_TABLE_SIZE] =
{
	0x0028407e, /* 40 */
	0x00667001, /* 41 */
	0x00665002, /* 42 */
	0x00661013, /* 43 */
	0x0066a120, /* 44 */
	0x00661002, /* 45 */
	0x00659001, /* 46 */
	0x00657002, /* 47 */
	0x0065d001, /* 48 */
	0x0064e013, /* 49 */
	0x0066e120, /* 4a */
	0x0064e001, /* 4b */
	0x00652001, /* 4c */
	0x00841000, /* 4d */
	0x0024c07e, /* 4e */
	0x0025c08a, /* 4f */
	0x0025d5a0, /* 50 */
	0x0035c00d, /* 51 */
	0x0024c07e, /* 52 */
	0x0025c08a, /* 53 */
	0x0025d5a0, /* 54 */
	0x0025c018, /* 55 */
	0x00384068, /* 56 */
	0x0020c07e, /* 57 */
	0x0031c637, /* 58 */
	0x0024c07e, /* 59 */
	0x0025c2e2, /* 5a */
	0x0021c348, /* 5b */
	0x0031c00d, /* 5c */
	0x0020c07e, /* 5d */
	0x0021c2e2, /* 5e */
	0x0025c348, /* 5f */
	0x0035c00d, /* 60 */
	0x0024407e, /* 61 */
	0x0025408a, /* 62 */
	0x002555a0, /* 63 */
	0x0035400d, /* 64 */
	0x0020407e, /* 65 */
	0x00314637, /* 66 */
	0x0021408a, /* 67 */
	0x002155a0, /* 68 */
	0x0031400d, /* 69 */
	0x0024207e, /* 6a */
	0x0025208a, /* 6b */
	0x002735a0, /* 6c */
	0x0035200d, /* 6d */
	0x0024a07e, /* 6e */
	0x0025a08a, /* 6f */
	0x0027b5a0, /* 70 */
	0x0035a00d, /* 71 */
	0x00000000, /* 72 */
	0x00000000, /* 73 */
	0x00000000, /* 74 */
	0x00000000, /* 75 */
	0x00000000, /* 76 */
	0x00000000, /* 77 */
	0x00000000, /* 78 */
	0x00000000, /* 79 */
	0x00000000, /* 7a */
	0x00000000, /* 7b */
	0x00000000, /* 7c */
	0x00000000, /* 7d */
	0x00102605, /* 7e */
	0xf20a30c2, /* 7f */
};

/* Dtram(0..3f) values for PAL-M DVI */
/* From vec_dit_palm_mc.bss */
static const uint32_t s_aulDtRamBVBInput_DVI_PALM[BVDC_P_DTRAM_TABLE_SIZE] =
{
	0x0064d001, /* 40 */
	0x00651002, /* 41 */
	0x0065500f, /* 42 */
	0x006590f0, /* 43 */
	0x00655004, /* 44 */
	0x00661001, /* 45 */
	0x00664002, /* 46 */
	0x00668001, /* 47 */
	0x0065d00f, /* 48 */
	0x0066b0f0, /* 49 */
	0x0065d003, /* 4a */
	0x0066f001, /* 4b */
	0x00840000, /* 4c */
	0x002d007c, /* 4d */
	0x002c0072, /* 4e */
	0x002415a0, /* 4f */
	0x0034001b, /* 50 */
	0x0025007c, /* 51 */
	0x00240072, /* 52 */
	0x002415a0, /* 53 */
	0x0034001b, /* 54 */
	0x0021007c, /* 55 */
	0x00200072, /* 56 */
	0x002015a0, /* 57 */
	0x0030001b, /* 58 */
	0x0021207c, /* 59 */
	0x00202072, /* 5a */
	0x002235a0, /* 5b */
	0x0030201b, /* 5c */
	0x0021807c, /* 5d */
	0x00208072, /* 5e */
	0x002095a0, /* 5f */
	0x0030801b, /* 60 */
	0x0021807c, /* 61 */
	0x002082de, /* 62 */
	0x0034834f, /* 63 */
	0x0025807c, /* 64 */
	0x00248072, /* 65 */
	0x002495a0, /* 66 */
	0x0034801b, /* 67 */
	0x0025807c, /* 68 */
	0x002482de, /* 69 */
	0x0030834f, /* 6a */
	0x0021a07c, /* 6b */
	0x0020a072, /* 6c */
	0x0022b5a0, /* 6d */
	0x0030a01b, /* 6e */
	0x0021807c, /* 6f */
	0x00208072, /* 70 */
	0x002095a0, /* 71 */
	0x00308010, /* 72 */
	0x00000000, /* 73 */
	0x00000000, /* 74 */
	0x00000000, /* 75 */
	0x00000000, /* 76 */
	0x00000000, /* 77 */
	0x00000000, /* 78 */
	0x00000000, /* 79 */
	0x00000000, /* 7a */
	0x00000000, /* 7b */
	0x00000000, /* 7c */
	0x00000000, /* 7d */
	0x00111109, /* 7e */
	0x6e65eaf5, /* 7f */
};

/* Dtram values for 1080i DVI */
static const uint32_t s_aulDtRamBVBInput_DVI_1080i[BVDC_P_DTRAM_TABLE_SIZE] =
{
	0x0028402c, /* 64 */
	0x002d402c, /* 65 */
	0x00244835, /* 66 */
	0x00667004, /* 67 */
	0x00664001, /* 68 */
	0x0065f00e, /* 69 */
	0x0066a21c, /* 70 */
	0x0065f002, /* 71 */
	0x00657001, /* 72 */
	0x00654004, /* 73 */
	0x0065b001, /* 74 */
	0x0064f00f, /* 75 */
	0x0066f21c, /* 76 */
	0x0064f002, /* 77 */
	0x00840000, /* 78 */
	0x0020c02c, /* 79 */
	0x0021c02c, /* 80 */
	0x0020c094, /* 81 */
	0x0020d780, /* 82 */
	0x0030c021, /* 83 */
	0x0024c02c, /* 84 */
	0x0025c02c, /* 85 */
	0x0034c835, /* 86 */
	0x0020c02c, /* 87 */
	0x0021c02c, /* 88 */
	0x0020c420, /* 89 */
	0x0034c415, /* 90 */
	0x0024c02c, /* 91 */
	0x0025c02c, /* 92 */
	0x0024c420, /* 93 */
	0x0030c415, /* 94 */
	0x0020402c, /* 95 */
	0x0021402c, /* 96 */
	0x00204094, /* 97 */
	0x00205780, /* 98 */
	0x00304021, /* 99 */
	0x0024402c, /* 100 */
	0x0021402c, /* 101 */
	0x00304835, /* 102 */
	0x0024402c, /* 103 */
	0x0025402c, /* 104 */
	0x00344835, /* 105 */
	0x0020202c, /* 106 */
	0x0021202c, /* 107 */
	0x00202094, /* 108 */
	0x00223780, /* 109 */
	0x00302021, /* 110 */
	0x0020a02c, /* 111 */
	0x0021a02c, /* 112 */
	0x0020a094, /* 113 */
	0x0022b780, /* 114 */
	0x0030a021, /* 115 */
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
	0x00110708, /* 126 */
	0x44470f2e, /* 127 */
};

/* Dtram values for 2160i_48Hz DVI */
/* vec_dit_2160i48hz_dvi.bss */
static const uint32_t s_aulDtRamBVBInput_DVI_2160i_48hz[BVDC_P_RAM_TABLE_SIZE] =
{
	0x00204020, /* 64 */
	0x0024402c, /* 65 */
	0x0021402c, /* 66 */
	0x00204a5b, /* 67 */
	0x0065a005, /* 68 */
	0x00650023, /* 69 */
	0x0065d438, /* 70 */
	0x00650004, /* 71 */
	0x00655029, /* 72 */
	0x0066214e, /* 73 */
	0x00667001, /* 74 */
	0x006620f9, /* 75 */
	0x00667001, /* 76 */
	0x006621ef, /* 77 */
	0x00655004, /* 78 */
	0x00841000, /* 79 */
	0x0020402c, /* 80 */
	0x0021402c, /* 81 */
	0x00204094, /* 82 */
	0x00205780, /* 83 */
	0x00304247, /* 84 */
	0x0024402c, /* 85 */
	0x0025402c, /* 86 */
	0x00244094, /* 87 */
	0x00245780, /* 88 */
	0x00344247, /* 89 */
	0x0020402c, /* 90 */
	0x0021402c, /* 91 */
	0x00304a5b, /* 92 */
	0x0020202c, /* 93 */
	0x0021202c, /* 94 */
	0x00202094, /* 95 */
	0x00223780, /* 96 */
	0x00302247, /* 97 */
	0x0024202c, /* 98 */
	0x0025202c, /* 99 */
	0x00242094, /* 100 */
	0x00263780, /* 101 */
	0x00342247, /* 102 */
	0x002c202c, /* 103 */
	0x002d202c, /* 104 */
	0x00242094, /* 105 */
	0x00263780, /* 106 */
	0x00342247, /* 107 */
	0x00000000, /* 108 */
	0x00000000, /* 109 */
	0x00000000, /* 110 */
	0x00000000, /* 111 */
	0x00000000, /* 112 */
	0x00000000, /* 113 */
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
	0x00120209, /* 126 */
	0x5c344fa8, /* 127 */
};

/* Dtram values for 2160i_60Hz DVI */
/* vec_dit_2160i48hz_dvi.bss */
static const uint32_t s_aulDtRamBVBInput_DVI_2160i_60hz[BVDC_P_RAM_TABLE_SIZE] =
{
	0x00204020, /* 64 */
	0x0020402c, /* 65 */
	0x0025402c, /* 66 */
	0x00244835, /* 67 */
	0x0065b004, /* 68 */
	0x00658001, /* 69 */
	0x00653023, /* 70 */
	0x0065e438, /* 71 */
	0x00653004, /* 72 */
	0x0065b005, /* 73 */
	0x00658001, /* 74 */
	0x00653023, /* 75 */
	0x0065e14e, /* 76 */
	0x00663001, /* 77 */
	0x0065e0f9, /* 78 */
	0x00663001, /* 79 */
	0x0065e1ef, /* 80 */
	0x00653004, /* 81 */
	0x00841000, /* 82 */
	0x0020402c, /* 83 */
	0x0021402c, /* 84 */
	0x00204094, /* 85 */
	0x00205780, /* 86 */
	0x00304021, /* 87 */
	0x0024402c, /* 88 */
	0x0021402c, /* 89 */
	0x00304835, /* 90 */
	0x0024402c, /* 91 */
	0x0025402c, /* 92 */
	0x00344835, /* 93 */
	0x0020202c, /* 94 */
	0x0021202c, /* 95 */
	0x00202094, /* 96 */
	0x00223780, /* 97 */
	0x00302021, /* 98 */
	0x0028202c, /* 99 */
	0x0029202c, /* 100 */
	0x00202094, /* 101 */
	0x00223780, /* 102 */
	0x00302021, /* 103 */
	0x00000000, /* 104 */
	0x00000000, /* 105 */
	0x00000000, /* 106 */
	0x00000000, /* 107 */
	0x00000000, /* 108 */
	0x00000000, /* 109 */
	0x00000000, /* 110 */
	0x00000000, /* 111 */
	0x00000000, /* 112 */
	0x00000000, /* 113 */
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
	0x00070910, /* 126 */
	0x2dbd490c, /* 127 */
};

/* Dtram values for 3D 720p_60Hz DVI */
static const uint32_t s_aulDtRamBVBInput_DVI_3D_720p_60hz[BVDC_P_RAM_TABLE_SIZE] =
{
	0x00673001, /* 64 */
	0x0065f004, /* 65 */
	0x00663001, /* 66 */
	0x00667013, /* 67 */
	0x0066b5be, /* 68 */
	0x00667004, /* 69 */
	0x00677001, /* 70 */
	0x00840000, /* 71 */
	0x0066f001, /* 72 */
	0x00667000, /* 73 */
	0x0065b001, /* 74 */
	0x0065f004, /* 75 */
	0x00663001, /* 76 */
	0x00667013, /* 77 */
	0x0066b5be, /* 78 */
	0x00677001, /* 79 */
	0x00848000, /* 80 */
	0x0066f001, /* 81 */
	0x00667000, /* 82 */
	0x0065b001, /* 83 */
	0x0065f004, /* 84 */
	0x00663001, /* 85 */
	0x00667013, /* 86 */
	0x0066b5be, /* 87 */
	0x00667004, /* 88 */
	0x00677001, /* 89 */
	0x00851000, /* 90 */
	0x0020006e, /* 91 */
	0x00250028, /* 92 */
	0x002400dc, /* 93 */
	0x003414f5, /* 94 */
	0x0024006e, /* 95 */
	0x00250028, /* 96 */
	0x002400dc, /* 97 */
	0x003414f5, /* 98 */
	0x0024006e, /* 99 */
	0x00210028, /* 100 */
	0x002000dc, /* 101 */
	0x003014f5, /* 102 */
	0x0020006e, /* 103 */
	0x00210028, /* 104 */
	0x002000dc, /* 105 */
	0x003014f5, /* 106 */
	0x0020206e, /* 107 */
	0x00212028, /* 108 */
	0x002020dc, /* 109 */
	0x003234f5, /* 110 */
	0x0020006e, /* 111 */
	0x00290028, /* 112 */
	0x002000dc, /* 113 */
	0x003014f5, /* 114 */
	0x0020006e, /* 115 */
	0x002d0028, /* 116 */
	0x002400dc, /* 117 */
	0x003414f5, /* 118 */
	0x0020006e, /* 119 */
	0x00210028, /* 120 */
	0x002000dc, /* 121 */
	0x003014ea, /* 122 */
	0xfeff0a48, /* 123 */
	0x00000001, /* 124 */
	0x008fd000, /* 125 */
	0x00012611, /* 126 */
	0x001cffdb, /* 127 */
};

/* Dtram values for 3D 720p_50Hz DVI */
static const uint32_t s_aulDtRamBVBInput_DVI_3D_720p_50hz[BVDC_P_RAM_TABLE_SIZE] =
{
	0x00673001, /* 64 */
	0x0065f004, /* 65 */
	0x00663001, /* 66 */
	0x00667013, /* 67 */
	0x0066b5be, /* 68 */
	0x00667004, /* 69 */
	0x00677001, /* 70 */
	0x00840000, /* 71 */
	0x0066f001, /* 72 */
	0x00667000, /* 73 */
	0x0065b001, /* 74 */
	0x0065f004, /* 75 */
	0x00663001, /* 76 */
	0x00667013, /* 77 */
	0x0066b5be, /* 78 */
	0x00677001, /* 79 */
	0x00848000, /* 80 */
	0x0066f001, /* 81 */
	0x00667000, /* 82 */
	0x0065b001, /* 83 */
	0x0065f004, /* 84 */
	0x00663001, /* 85 */
	0x00667013, /* 86 */
	0x0066b5be, /* 87 */
	0x00667004, /* 88 */
	0x00677001, /* 89 */
	0x00851000, /* 90 */
	0x002001b8, /* 91 */
	0x00250028, /* 92 */
	0x002400dc, /* 93 */
	0x003414f5, /* 94 */
	0x002401b8, /* 95 */
	0x00250028, /* 96 */
	0x002400dc, /* 97 */
	0x003414f5, /* 98 */
	0x002401b8, /* 99 */
	0x00210028, /* 100 */
	0x002000dc, /* 101 */
	0x003014f5, /* 102 */
	0x002001b8, /* 103 */
	0x00210028, /* 104 */
	0x002000dc, /* 105 */
	0x003014f5, /* 106 */
	0x002021b8, /* 107 */
	0x00212028, /* 108 */
	0x002020dc, /* 109 */
	0x003234f5, /* 110 */
	0x002001b8, /* 111 */
	0x00290028, /* 112 */
	0x002000dc, /* 113 */
	0x003014f5, /* 114 */
	0x002001b8, /* 115 */
	0x002d0028, /* 116 */
	0x002400dc, /* 117 */
	0x003414f5, /* 118 */
	0x002001b8, /* 119 */
	0x00210028, /* 120 */
	0x002000dc, /* 121 */
	0x003014ea, /* 122 */
	0xfeff0a48, /* 123 */
	0x00000001, /* 124 */
	0x008fd000, /* 125 */
	0x00122710, /* 126 */
	0x001d0a2b, /* 127 */
};

/* Dtram values for 3D 1080p 24Hz DVI */
/* vec_dit_2205p24hz3D_dvi.bss */
static const uint32_t s_aulDtRamBVBInput_DVI_3D_1080p_24hz[BVDC_P_DTRAM_TABLE_SIZE] =
{
	0x00673001, /* 40 */
	0x0065f004, /* 41 */
	0x00663001, /* 42 */
	0x00667023, /* 43 */
	0x0066b89d, /* 44 */
	0x00667003, /* 45 */
	0x00677001, /* 46 */
	0x00840000, /* 47 */
	0x0066f001, /* 48 */
	0x00667000, /* 49 */
	0x0065b001, /* 4a */
	0x0065f004, /* 4b */
	0x00663001, /* 4c */
	0x00667023, /* 4d */
	0x0066b89d, /* 4e */
	0x00677001, /* 4f */
	0x00848000, /* 50 */
	0x0066f001, /* 51 */
	0x00667000, /* 52 */
	0x0065b001, /* 53 */
	0x0065f004, /* 54 */
	0x00663001, /* 55 */
	0x00667023, /* 56 */
	0x0066b89d, /* 57 */
	0x00667003, /* 58 */
	0x00677001, /* 59 */
	0x00851000, /* 5a */
	0x0020027e, /* 5b */
	0x0025002c, /* 5c */
	0x00240094, /* 5d */
	0x00341775, /* 5e */
	0x0024027e, /* 5f */
	0x0025002c, /* 60 */
	0x00240094, /* 61 */
	0x00341775, /* 62 */
	0x0024027e, /* 63 */
	0x0021002c, /* 64 */
	0x00200094, /* 65 */
	0x00301775, /* 66 */
	0x0020027e, /* 67 */
	0x0021002c, /* 68 */
	0x00200094, /* 69 */
	0x00301775, /* 6a */
	0x0020227e, /* 6b */
	0x0021202c, /* 6c */
	0x00202094, /* 6d */
	0x00323775, /* 6e */
	0x0020027e, /* 6f */
	0x0029002c, /* 70 */
	0x00200094, /* 71 */
	0x00301775, /* 72 */
	0x0020027e, /* 73 */
	0x002d002c, /* 74 */
	0x00240094, /* 75 */
	0x00341775, /* 76 */
	0x0020027e, /* 77 */
	0x0021002c, /* 78 */
	0x00200094, /* 79 */
	0x0030176a, /* 7a */
	0xfeff0a48, /* 7b */
	0x00000001, /* 7c */
	0x008fd000, /* 7d */
	0x00122710, /* 7e */
	0x001d2b06, /* 7f */
};

/* Dtram values for 3D 1080p 30Hz DVI */
/* vec_dit_2205p30hz3D_dvi.bss */
static const uint32_t s_aulDtRamBVBInput_DVI_3D_1080p_30hz[BVDC_P_DTRAM_TABLE_SIZE] =
{
	0x00673001, /* 40 */
	0x0065f004, /* 41 */
	0x00663001, /* 42 */
	0x00667023, /* 43 */
	0x0066b89d, /* 44 */
	0x00667003, /* 45 */
	0x00677001, /* 46 */
	0x00840000, /* 47 */
	0x0066f001, /* 48 */
	0x00667000, /* 49 */
	0x0065b001, /* 4a */
	0x0065f004, /* 4b */
	0x00663001, /* 4c */
	0x00667023, /* 4d */
	0x0066b89d, /* 4e */
	0x00677001, /* 4f */
	0x00848000, /* 50 */
	0x0066f001, /* 51 */
	0x00667000, /* 52 */
	0x0065b001, /* 53 */
	0x0065f004, /* 54 */
	0x00663001, /* 55 */
	0x00667023, /* 56 */
	0x0066b89d, /* 57 */
	0x00667003, /* 58 */
	0x00677001, /* 59 */
	0x00851000, /* 5a */
	0x00200058, /* 5b */
	0x0025002c, /* 5c */
	0x00240094, /* 5d */
	0x00341775, /* 5e */
	0x00240058, /* 5f */
	0x0025002c, /* 60 */
	0x00240094, /* 61 */
	0x00341775, /* 62 */
	0x00240058, /* 63 */
	0x0021002c, /* 64 */
	0x00200094, /* 65 */
	0x00301775, /* 66 */
	0x00200058, /* 67 */
	0x0021002c, /* 68 */
	0x00200094, /* 69 */
	0x00301775, /* 6a */
	0x00202058, /* 6b */
	0x0021202c, /* 6c */
	0x00202094, /* 6d */
	0x00323775, /* 6e */
	0x00200058, /* 6f */
	0x0029002c, /* 70 */
	0x00200094, /* 71 */
	0x00301775, /* 72 */
	0x00200058, /* 73 */
	0x002d002c, /* 74 */
	0x00240094, /* 75 */
	0x00341775, /* 76 */
	0x00200058, /* 77 */
	0x0021002c, /* 78 */
	0x00200094, /* 79 */
	0x0030176a, /* 7a */
	0xfeff0a48, /* 7b */
	0x00000001, /* 7c */
	0x008fd000, /* 7d */
	0x00122710, /* 7e */
	0x001d19d6, /* 7f */
};

/* Dtram values for 3D 1080p 60Hz OU DVI */
/* DVI_MCODE_1920x2205p3D_30Hz.bss */
static const uint32_t s_aulDtRamBVBInput_DVI_3DOU_1080p_60hz[BVDC_P_DTRAM_TABLE_SIZE] =
{
	0x00673001, /* 40 */
	0x0065f004, /* 41 */
	0x00663001, /* 42 */
	0x00667023, /* 43 */
	0x0066b89d, /* 44 */
	0x00667003, /* 45 */
	0x00677001, /* 46 */
	0x00840000, /* 47 */
	0x0066f001, /* 48 */
	0x00667000, /* 49 */
	0x0065b001, /* 4a */
	0x0065f004, /* 4b */
	0x00663001, /* 4c */
	0x00667023, /* 4d */
	0x0066b89d, /* 4e */
	0x00677001, /* 4f */
	0x00848000, /* 50 */
	0x0066f001, /* 51 */
	0x00667000, /* 52 */
	0x0065b001, /* 53 */
	0x0065f004, /* 54 */
	0x00663001, /* 55 */
	0x00667023, /* 56 */
	0x0066b89d, /* 57 */
	0x00667003, /* 58 */
	0x00677001, /* 59 */
	0x00851000, /* 5a */
	0x00200058, /* 5b */
	0x0025002c, /* 5c */
	0x00240094, /* 5d */
	0x00341775, /* 5e */
	0x00240058, /* 5f */
	0x0025002c, /* 60 */
	0x00240094, /* 61 */
	0x00341775, /* 62 */
	0x00240058, /* 63 */
	0x0021002c, /* 64 */
	0x00200094, /* 65 */
	0x00301775, /* 66 */
	0x00200058, /* 67 */
	0x0021002c, /* 68 */
	0x00200094, /* 69 */
	0x00301775, /* 6a */
	0x00202058, /* 6b */
	0x0021202c, /* 6c */
	0x00202094, /* 6d */
	0x00323775, /* 6e */
	0x00200058, /* 6f */
	0x0029002c, /* 70 */
	0x00200094, /* 71 */
	0x00301775, /* 72 */
	0x00200058, /* 73 */
	0x002d002c, /* 74 */
	0x00240094, /* 75 */
	0x00341775, /* 76 */
	0x00200058, /* 77 */
	0x0021002c, /* 78 */
	0x00200094, /* 79 */
	0x0030176a, /* 7a */
	0xfeff0b08, /* 7b */
	0x00000001, /* 7c */
	0x008fd000, /* 7d */
	0x00091211, /* 7e */
	0x001d19d6, /* 7f */
};

/* Dtram values for 3D 1080p 60Hz LR DVI */
/* DVI_MCODE_3840x1080p3D_FR_60Hz.bss */
static const uint32_t s_aulDtRamBVBInput_DVI_3DLR_1080p_60hz[BVDC_P_DTRAM_TABLE_SIZE] =
{
	0x00673001, /* 40 */
	0x0065f004, /* 41 */
	0x00663001, /* 42 */
	0x00667023, /* 43 */
	0x0066b438, /* 44 */
	0x00667003, /* 45 */
	0x00677001, /* 46 */
	0x00840000, /* 47 */
	0x0066f001, /* 48 */
	0x00667000, /* 49 */
	0x0065b001, /* 4a */
	0x0065f004, /* 4b */
	0x00663001, /* 4c */
	0x00667023, /* 4d */
	0x0066b438, /* 4e */
	0x00677001, /* 4f */
	0x00848000, /* 50 */
	0x0066f001, /* 51 */
	0x00667000, /* 52 */
	0x0065b001, /* 53 */
	0x0065f004, /* 54 */
	0x00663001, /* 55 */
	0x00667023, /* 56 */
	0x0066b438, /* 57 */
	0x00667003, /* 58 */
	0x00677001, /* 59 */
	0x00851000, /* 5a */
	0x002000b0, /* 5b */
	0x00250058, /* 5c */
	0x00240128, /* 5d */
	0x00341ef5, /* 5e */
	0x002400b0, /* 5f */
	0x00250058, /* 60 */
	0x00240128, /* 61 */
	0x00341ef5, /* 62 */
	0x002400b0, /* 63 */
	0x00210058, /* 64 */
	0x00200128, /* 65 */
	0x00301ef5, /* 66 */
	0x002000b0, /* 67 */
	0x00210058, /* 68 */
	0x00200128, /* 69 */
	0x00301ef5, /* 6a */
	0x002020b0, /* 6b */
	0x00212058, /* 6c */
	0x00202128, /* 6d */
	0x00323ef5, /* 6e */
	0x002000b0, /* 6f */
	0x00290058, /* 70 */
	0x00200128, /* 71 */
	0x00301ef5, /* 72 */
	0x002000b0, /* 73 */
	0x002d0058, /* 74 */
	0x00240128, /* 75 */
	0x00341ef5, /* 76 */
	0x002000b0, /* 77 */
	0x00210058, /* 78 */
	0x00200128, /* 79 */
	0x00301eea, /* 7a */
	0xfeff0b08, /* 7b */
	0x00000001, /* 7c */
	0x008fd000, /* 7d */
	0x00091211, /* 7e */
	0x001d5167, /* 7f */
};

/* Dtram values for 1080p 24Hz DVI */
/* vec_dit_1080p24hz_dvi.bss */
static const uint32_t s_aulDtRamBVBInput_DVI_1080p_24hz[BVDC_P_DTRAM_TABLE_SIZE] =
{
	0x00284020, /* 40 */
	0x0028402c, /* 41 */
	0x002d402c, /* 42 */
	0x00244a5b, /* 43 */
	0x00652004, /* 44 */
	0x0064f001, /* 45 */
	0x0064a023, /* 46 */
	0x00655438, /* 47 */
	0x0064a004, /* 48 */
	0x00841000, /* 49 */
	0x0020402c, /* 4a */
	0x0021402c, /* 4b */
	0x00204094, /* 4c */
	0x00205780, /* 4d */
	0x00304247, /* 4e */
	0x0024402c, /* 4f */
	0x0021402c, /* 50 */
	0x00304a5b, /* 51 */
	0x0024402c, /* 52 */
	0x0025402c, /* 53 */
	0x00344a5b, /* 54 */
	0x0020202c, /* 55 */
	0x0021202c, /* 56 */
	0x00202094, /* 57 */
	0x00223780, /* 58 */
	0x00302247, /* 59 */
	0x00000000, /* 5a */
	0x00000000, /* 5b */
	0x00000000, /* 5c */
	0x00000000, /* 5d */
	0x00000000, /* 5e */
	0x00000000, /* 5f */
	0x00000000, /* 60 */
	0x00000000, /* 61 */
	0x00000000, /* 62 */
	0x00000000, /* 63 */
	0x00000000, /* 64 */
	0x00000000, /* 65 */
	0x00000000, /* 66 */
	0x00000000, /* 67 */
	0x00000000, /* 68 */
	0x00000000, /* 69 */
	0x00000000, /* 6a */
	0x00000000, /* 6b */
	0x00000000, /* 6c */
	0x00000000, /* 6d */
	0x00000000, /* 6e */
	0x00000000, /* 6f */
	0x00000000, /* 70 */
	0x00000000, /* 71 */
	0x00000000, /* 72 */
	0x00000000, /* 73 */
	0x00000000, /* 74 */
	0x00000000, /* 75 */
	0x00000000, /* 76 */
	0x00000000, /* 77 */
	0x00000000, /* 78 */
	0x00000000, /* 79 */
	0x00000000, /* 7a */
	0x00000000, /* 7b */
	0x00000000, /* 7c */
	0x00000000, /* 7d */
	0x00051106, /* 7e */
	0xb8fbc268, /* 7f */
};

/* Dtram values for 1080p 25Hz DVI */
/* vec_dit_1080p25hz_dvi.bss */
static const uint32_t s_aulDtRamBVBInput_DVI_1080p_25hz[BVDC_P_DTRAM_TABLE_SIZE] =
{
	0x00284010, /* 40 */
	0x0028402c, /* 41 */
	0x002d402c, /* 42 */
	0x002449ed, /* 43 */
	0x00652004, /* 44 */
	0x0064f001, /* 45 */
	0x0064a023, /* 46 */
	0x00655438, /* 47 */
	0x0064a004, /* 48 */
	0x00841000, /* 49 */
	0x0020402c, /* 4a */
	0x0021402c, /* 4b */
	0x00204094, /* 4c */
	0x00205780, /* 4d */
	0x003041d9, /* 4e */
	0x0024402c, /* 4f */
	0x0021402c, /* 50 */
	0x003049ed, /* 51 */
	0x0024402c, /* 52 */
	0x0025402c, /* 53 */
	0x003449ed, /* 54 */
	0x0020202c, /* 55 */
	0x0021202c, /* 56 */
	0x00202094, /* 57 */
	0x00223780, /* 58 */
	0x003021d9, /* 59 */
	0x00000000, /* 5a */
	0x00000000, /* 5b */
	0x00000000, /* 5c */
	0x00000000, /* 5d */
	0x00000000, /* 5e */
	0x00000000, /* 5f */
	0x00000000, /* 60 */
	0x00000000, /* 61 */
	0x00000000, /* 62 */
	0x00000000, /* 63 */
	0x00000000, /* 64 */
	0x00000000, /* 65 */
	0x00000000, /* 66 */
	0x00000000, /* 67 */
	0x00000000, /* 68 */
	0x00000000, /* 69 */
	0x00000000, /* 6a */
	0x00000000, /* 6b */
	0x00000000, /* 6c */
	0x00000000, /* 6d */
	0x00000000, /* 6e */
	0x00000000, /* 6f */
	0x00000000, /* 70 */
	0x00000000, /* 71 */
	0x00000000, /* 72 */
	0x00000000, /* 73 */
	0x00000000, /* 74 */
	0x00000000, /* 75 */
	0x00000000, /* 76 */
	0x00000000, /* 77 */
	0x00000000, /* 78 */
	0x00000000, /* 79 */
	0x00000000, /* 7a */
	0x00000000, /* 7b */
	0x00000000, /* 7c */
	0x00000000, /* 7d */
	0x00092406, /* 7e */
	0x64ad4064, /* 7f */
};

/* Dtram values for 1080p 30Hz DVI */
/* vec_dit_1080p30hz_dvi.bss */
static const uint32_t s_aulDtRamBVBInput_DVI_1080p_30hz[BVDC_P_DTRAM_TABLE_SIZE] =
{
	0x00284020, /* 40 */
	0x0028402c, /* 41 */
	0x002d402c, /* 42 */
	0x00244835, /* 43 */
	0x00652004, /* 44 */
	0x0064f001, /* 45 */
	0x0064a023, /* 46 */
	0x00655438, /* 47 */
	0x0064a004, /* 48 */
	0x00841000, /* 49 */
	0x0020402c, /* 4a */
	0x0021402c, /* 4b */
	0x00204094, /* 4c */
	0x00205780, /* 4d */
	0x00304021, /* 4e */
	0x0024402c, /* 4f */
	0x0021402c, /* 50 */
	0x00304835, /* 51 */
	0x0024402c, /* 52 */
	0x0025402c, /* 53 */
	0x00344835, /* 54 */
	0x0020202c, /* 55 */
	0x0021202c, /* 56 */
	0x00202094, /* 57 */
	0x00223780, /* 58 */
	0x00302021, /* 59 */
	0x00000000, /* 5a */
	0x00000000, /* 5b */
	0x00000000, /* 5c */
	0x00000000, /* 5d */
	0x00000000, /* 5e */
	0x00000000, /* 5f */
	0x00000000, /* 60 */
	0x00000000, /* 61 */
	0x00000000, /* 62 */
	0x00000000, /* 63 */
	0x00000000, /* 64 */
	0x00000000, /* 65 */
	0x00000000, /* 66 */
	0x00000000, /* 67 */
	0x00000000, /* 68 */
	0x00000000, /* 69 */
	0x00000000, /* 6a */
	0x00000000, /* 6b */
	0x00000000, /* 6c */
	0x00000000, /* 6d */
	0x00000000, /* 6e */
	0x00000000, /* 6f */
	0x00000000, /* 70 */
	0x00000000, /* 71 */
	0x00000000, /* 72 */
	0x00000000, /* 73 */
	0x00000000, /* 74 */
	0x00000000, /* 75 */
	0x00000000, /* 76 */
	0x00000000, /* 77 */
	0x00000000, /* 78 */
	0x00000000, /* 79 */
	0x00000000, /* 7a */
	0x00000000, /* 7b */
	0x00000000, /* 7c */
	0x00000000, /* 7d */
	0x00020706, /* 7e */
	0x2a211d50, /* 7f */
};

/* Dtram values for 1125i 50Hz DVI */
static const uint32_t s_aulDtRamBVBInput_DVI_1125i_50hz[BVDC_P_DTRAM_TABLE_SIZE] =
{
	0x0020c021, /* 40 */
	0x0021c02c, /* 41 */
	0x0020c094, /* 42 */
	0x0020d780, /* 43 */
	0x0020c1e4, /* 44 */
	0x0028402c, /* 45 */
	0x002d402c, /* 46 */
	0x002449ed, /* 47 */
	0x00671004, /* 48 */
	0x0066e001, /* 49 */
	0x0066900e, /* 4a */
	0x0067421c, /* 4b */
	0x00669002, /* 4c */
	0x00661001, /* 4d */
	0x0065e004, /* 4e */
	0x00665001, /* 4f */
	0x0065400f, /* 50 */
	0x0067921c, /* 51 */
	0x00659001, /* 52 */
	0x00840000, /* 53 */
	0x0020c02c, /* 54 */
	0x0021c02c, /* 55 */
	0x0020c094, /* 56 */
	0x0020d780, /* 57 */
	0x0030c1d9, /* 58 */
	0x0020c02c, /* 59 */
	0x0021c02c, /* 5a */
	0x0020c094, /* 5b */
	0x0020d780, /* 5c */
	0x0030c1e4, /* 5d */
	0x0024c02c, /* 5e */
	0x0025c02c, /* 5f */
	0x0034c9ed, /* 60 */
	0x0020c02c, /* 61 */
	0x0021c02c, /* 62 */
	0x0020c4fc, /* 63 */
	0x0034c4f1, /* 64 */
	0x0024c02c, /* 65 */
	0x0025c02c, /* 66 */
	0x0024c4fc, /* 67 */
	0x0030c4f1, /* 68 */
	0x0020402c, /* 69 */
	0x0021402c, /* 6a */
	0x00204094, /* 6b */
	0x00205780, /* 6c */
	0x003041d9, /* 6d */
	0x0024402c, /* 6e */
	0x0021402c, /* 6f */
	0x003049ed, /* 70 */
	0x0024402c, /* 71 */
	0x0025402c, /* 72 */
	0x003449ed, /* 73 */
	0x0020202c, /* 74 */
	0x0021202c, /* 75 */
	0x00202094, /* 76 */
	0x00223780, /* 77 */
	0x003021d9, /* 78 */
	0x0020a02c, /* 79 */
	0x0021a02c, /* 7a */
	0x0020a094, /* 7b */
	0x0022b780, /* 7c */
	0x0030a1d9, /* 7d */
	0x00012809, /* 7e */
	0xb1a1ad8e, /* 7f */
};

/* Dtram values for 720p DVI */
/* From vec_dit_720p_dvi.bss */
static const uint32_t s_aulDtRamBVBInput_DVI_720p[BVDC_P_DTRAM_TABLE_SIZE] =
{
	0x00648001, /* 40 */
	0x0064d004, /* 41 */
	0x00657001, /* 42 */
	0x00652013, /* 43 */
	0x006612d0, /* 44 */
	0x00652004, /* 45 */
	0x0065c001, /* 46 */
	0x00840000, /* 47 */
	0x0020401d, /* 48 */
	0x002d0028, /* 49 */
	0x002c00dc, /* 4a */
	0x00241500, /* 4b */
	0x0034003b, /* 4c */
	0x00244028, /* 4d */
	0x00250028, /* 4e */
	0x002400dc, /* 4f */
	0x00241500, /* 50 */
	0x0034003b, /* 51 */
	0x00204028, /* 52 */
	0x00210028, /* 53 */
	0x002000dc, /* 54 */
	0x00201500, /* 55 */
	0x0030003b, /* 56 */
	0x00244028, /* 57 */
	0x00210028, /* 58 */
	0x002000dc, /* 59 */
	0x00201500, /* 5a */
	0x0030003b, /* 5b */
	0x00204028, /* 5c */
	0x00210028, /* 5d */
	0x002000dc, /* 5e */
	0x00201500, /* 5f */
	0x0030003b, /* 60 */
	0x00206028, /* 61 */
	0x00212028, /* 62 */
	0x002020dc, /* 63 */
	0x00223500, /* 64 */
	0x0030203b, /* 65 */
	0x00000000, /* 66 */
	0x00000000, /* 67 */
	0x00000000, /* 68 */
	0x00000000, /* 69 */
	0x00000000, /* 6a */
	0x00000000, /* 6b */
	0x00000000, /* 6c */
	0x00000000, /* 6d */
	0x00000000, /* 6e */
	0x00000000, /* 6f */
	0x00000000, /* 70 */
	0x00000000, /* 71 */
	0x00000000, /* 72 */
	0x00000000, /* 73 */
	0x00000000, /* 74 */
	0x00000000, /* 75 */
	0x00000000, /* 76 */
	0x00000000, /* 77 */
	0x00000000, /* 78 */
	0x00000000, /* 79 */
	0x00000000, /* 7a */
	0x00000000, /* 7b */
	0x00000000, /* 7c */
	0x00000000, /* 7d */
	0x00061209, /* 7e */
	0x4b542fce, /* 7f */
};

/* Dtram values for 720p 24Hz DVI */
/* From vec_dit_720p24hz_dvi.bss */
static const uint32_t s_aulDtRamBVBInput_DVI_720p_24hz[BVDC_P_DTRAM_TABLE_SIZE] =
{
	0x00648001, /* 40 */
	0x0064d004, /* 41 */
	0x00657001, /* 42 */
	0x00652013, /* 43 */
	0x006612d0, /* 44 */
	0x00652004, /* 45 */
	0x0065c001, /* 46 */
	0x00840000, /* 47 */
	0x0020401d, /* 48 */
	0x002d0028, /* 49 */
	0x002c00dc, /* 4a */
	0x00241500, /* 4b */
	0x003409e6, /* 4c */
	0x00244028, /* 4d */
	0x00250028, /* 4e */
	0x002400dc, /* 4f */
	0x00241500, /* 50 */
	0x003409e6, /* 51 */
	0x00204028, /* 52 */
	0x00210028, /* 53 */
	0x002000dc, /* 54 */
	0x00201500, /* 55 */
	0x003009e6, /* 56 */
	0x00244028, /* 57 */
	0x00210028, /* 58 */
	0x002000dc, /* 59 */
	0x00201500, /* 5a */
	0x003009e6, /* 5b */
	0x00204028, /* 5c */
	0x00210028, /* 5d */
	0x002000dc, /* 5e */
	0x00201500, /* 5f */
	0x003009e6, /* 60 */
	0x00206028, /* 61 */
	0x00212028, /* 62 */
	0x002020dc, /* 63 */
	0x00223500, /* 64 */
	0x003029e6, /* 65 */
	0x00000000, /* 66 */
	0x00000000, /* 67 */
	0x00000000, /* 68 */
	0x00000000, /* 69 */
	0x00000000, /* 6a */
	0x00000000, /* 6b */
	0x00000000, /* 6c */
	0x00000000, /* 6d */
	0x00000000, /* 6e */
	0x00000000, /* 6f */
	0x00000000, /* 70 */
	0x00000000, /* 71 */
	0x00000000, /* 72 */
	0x00000000, /* 73 */
	0x00000000, /* 74 */
	0x00000000, /* 75 */
	0x00000000, /* 76 */
	0x00000000, /* 77 */
	0x00000000, /* 78 */
	0x00000000, /* 79 */
	0x00000000, /* 7a */
	0x00000000, /* 7b */
	0x00000000, /* 7c */
	0x00000000, /* 7d */
	0x00061609, /* 7e */
	0xe17105b0, /* 7f */
};

/* Dtram values for 720p 25Hz DVI */
/* From vec_dit_720p25hz_dvi.bss */
static const uint32_t s_aulDtRamBVBInput_DVI_720p_25hz[BVDC_P_DTRAM_TABLE_SIZE] =
{
	0x00648001, /* 40 */
	0x0064d004, /* 41 */
	0x00657001, /* 42 */
	0x00652013, /* 43 */
	0x006612d0, /* 44 */
	0x00652004, /* 45 */
	0x0065c001, /* 46 */
	0x00840000, /* 47 */
	0x0020401d, /* 48 */
	0x002d0028, /* 49 */
	0x002c00dc, /* 4a */
	0x00241500, /* 4b */
	0x00340941, /* 4c */
	0x00244028, /* 4d */
	0x00250028, /* 4e */
	0x002400dc, /* 4f */
	0x00241500, /* 50 */
	0x00340941, /* 51 */
	0x00204028, /* 52 */
	0x00210028, /* 53 */
	0x002000dc, /* 54 */
	0x00201500, /* 55 */
	0x00300941, /* 56 */
	0x00244028, /* 57 */
	0x00210028, /* 58 */
	0x002000dc, /* 59 */
	0x00201500, /* 5a */
	0x00300941, /* 5b */
	0x00204028, /* 5c */
	0x00210028, /* 5d */
	0x002000dc, /* 5e */
	0x00201500, /* 5f */
	0x00300941, /* 60 */
	0x00206028, /* 61 */
	0x00212028, /* 62 */
	0x002020dc, /* 63 */
	0x00223500, /* 64 */
	0x00302941, /* 65 */
	0x00000000, /* 66 */
	0x00000000, /* 67 */
	0x00000000, /* 68 */
	0x00000000, /* 69 */
	0x00000000, /* 6a */
	0x00000000, /* 6b */
	0x00000000, /* 6c */
	0x00000000, /* 6d */
	0x00000000, /* 6e */
	0x00000000, /* 6f */
	0x00000000, /* 70 */
	0x00000000, /* 71 */
	0x00000000, /* 72 */
	0x00000000, /* 73 */
	0x00000000, /* 74 */
	0x00000000, /* 75 */
	0x00000000, /* 76 */
	0x00000000, /* 77 */
	0x00000000, /* 78 */
	0x00000000, /* 79 */
	0x00000000, /* 7a */
	0x00000000, /* 7b */
	0x00000000, /* 7c */
	0x00000000, /* 7d */
	0x00061609, /* 7e */
	0xc89cbf75, /* 7f */
};

/* Dtram values for 720p 30Hz DVI */
/* From vec_dit_720p30hz_dvi.bss */
static const uint32_t s_aulDtRamBVBInput_DVI_720p_30hz[BVDC_P_DTRAM_TABLE_SIZE] =
{
	0x00648001, /* 40 */
	0x0064d004, /* 41 */
	0x00657001, /* 42 */
	0x00652013, /* 43 */
	0x006612d0, /* 44 */
	0x00652004, /* 45 */
	0x0065c001, /* 46 */
	0x00840000, /* 47 */
	0x0020401d, /* 48 */
	0x002d0028, /* 49 */
	0x002c00dc, /* 4a */
	0x00241500, /* 4b */
	0x003406ad, /* 4c */
	0x00244028, /* 4d */
	0x00250028, /* 4e */
	0x002400dc, /* 4f */
	0x00241500, /* 50 */
	0x003406ad, /* 51 */
	0x00204028, /* 52 */
	0x00210028, /* 53 */
	0x002000dc, /* 54 */
	0x00201500, /* 55 */
	0x003006ad, /* 56 */
	0x00244028, /* 57 */
	0x00210028, /* 58 */
	0x002000dc, /* 59 */
	0x00201500, /* 5a */
	0x003006ad, /* 5b */
	0x00204028, /* 5c */
	0x00210028, /* 5d */
	0x002000dc, /* 5e */
	0x00201500, /* 5f */
	0x003006ad, /* 60 */
	0x00206028, /* 61 */
	0x00212028, /* 62 */
	0x002020dc, /* 63 */
	0x00223500, /* 64 */
	0x003026ad, /* 65 */
	0x00000000, /* 66 */
	0x00000000, /* 67 */
	0x00000000, /* 68 */
	0x00000000, /* 69 */
	0x00000000, /* 6a */
	0x00000000, /* 6b */
	0x00000000, /* 6c */
	0x00000000, /* 6d */
	0x00000000, /* 6e */
	0x00000000, /* 6f */
	0x00000000, /* 70 */
	0x00000000, /* 71 */
	0x00000000, /* 72 */
	0x00000000, /* 73 */
	0x00000000, /* 74 */
	0x00000000, /* 75 */
	0x00000000, /* 76 */
	0x00000000, /* 77 */
	0x00000000, /* 78 */
	0x00000000, /* 79 */
	0x00000000, /* 7a */
	0x00000000, /* 7b */
	0x00000000, /* 7c */
	0x00000000, /* 7d */
	0x00061609, /* 7e */
	0xcd075201, /* 7f */
};


/* Dtram values for 720p 50Hz DVI */
/* From vec_dit_720p50hz_dvi.bss */
static const uint32_t s_aulDtRamBVBInput_DVI_720p_50hz[BVDC_P_DTRAM_TABLE_SIZE] =
{
	0x00648001, /* 40 */
	0x0064d004, /* 41 */
	0x00657001, /* 42 */
	0x00652013, /* 43 */
	0x006612d0, /* 44 */
	0x00652004, /* 45 */
	0x0065c001, /* 46 */
	0x00840000, /* 47 */
	0x0020401d, /* 48 */
	0x002d0028, /* 49 */
	0x002c00dc, /* 4a */
	0x00241500, /* 4b */
	0x00340185, /* 4c */
	0x00244028, /* 4d */
	0x00250028, /* 4e */
	0x002400dc, /* 4f */
	0x00241500, /* 50 */
	0x00340185, /* 51 */
	0x00204028, /* 52 */
	0x00210028, /* 53 */
	0x002000dc, /* 54 */
	0x00201500, /* 55 */
	0x00300185, /* 56 */
	0x00244028, /* 57 */
	0x00210028, /* 58 */
	0x002000dc, /* 59 */
	0x00201500, /* 5a */
	0x00300185, /* 5b */
	0x00204028, /* 5c */
	0x00210028, /* 5d */
	0x002000dc, /* 5e */
	0x00201500, /* 5f */
	0x00300185, /* 60 */
	0x00206028, /* 61 */
	0x00212028, /* 62 */
	0x002020dc, /* 63 */
	0x00223500, /* 64 */
	0x00302185, /* 65 */
	0x00000000, /* 66 */
	0x00000000, /* 67 */
	0x00000000, /* 68 */
	0x00000000, /* 69 */
	0x00000000, /* 6a */
	0x00000000, /* 6b */
	0x00000000, /* 6c */
	0x00000000, /* 6d */
	0x00000000, /* 6e */
	0x00000000, /* 6f */
	0x00000000, /* 70 */
	0x00000000, /* 71 */
	0x00000000, /* 72 */
	0x00000000, /* 73 */
	0x00000000, /* 74 */
	0x00000000, /* 75 */
	0x00000000, /* 76 */
	0x00000000, /* 77 */
	0x00000000, /* 78 */
	0x00000000, /* 79 */
	0x00000000, /* 7a */
	0x00000000, /* 7b */
	0x00000000, /* 7c */
	0x00000000, /* 7d */
	0x00061209, /* 7e */
	0x7b899c3b, /* 7f */
};

/* Dtram values for 480p DVI, drop 0 lines from compositor */
/* From vec_dit_480p_dvi.bss */
static const uint32_t
s_aulDtRamBVBInput_DVI_480p_Drop0[BVDC_P_DTRAM_TABLE_SIZE] =
{
	0x0029003e, /* 40 */
	0x00280031, /* 41 */
	0x0065c005, /* 42 */
	0x00650001, /* 43 */
	0x00654005, /* 44 */
	0x00658001, /* 45 */
	0x0065c01d, /* 46 */
	0x00660001, /* 47 */
	0x006641df, /* 48 */
	0x00668001, /* 49 */
	0x0065c002, /* 4a */
	0x0064d001, /* 4b */
	0x00841000, /* 4c */
	0x002012d0, /* 4d */
	0x00200010, /* 4e */
	0x00390033, /* 4f */
	0x002012d0, /* 50 */
	0x00200010, /* 51 */
	0x0025003e, /* 52 */
	0x00340031, /* 53 */
	0x002412d0, /* 54 */
	0x00240010, /* 55 */
	0x0025003e, /* 56 */
	0x00340031, /* 57 */
	0x002412d0, /* 58 */
	0x00240010, /* 59 */
	0x0021003e, /* 5a */
	0x00300031, /* 5b */
	0x002012d0, /* 5c */
	0x00200010, /* 5d */
	0x0021003e, /* 5e */
	0x00300031, /* 5f */
	0x002012d0, /* 60 */
	0x00200010, /* 61 */
	0x0021003e, /* 62 */
	0x00302031, /* 63 */
	0x002232d0, /* 64 */
	0x00202010, /* 65 */
	0x0021203e, /* 66 */
	0x00302031, /* 67 */
	0x002232d0, /* 68 */
	0x00202010, /* 69 */
	0x0021003e, /* 6a */
	0x00300031, /* 6b */
	0x00000000, /* 6c */
	0x00000000, /* 6d */
	0x00000000, /* 6e */
	0x00000000, /* 6f */
	0x00000000, /* 70 */
	0x00000000, /* 71 */
	0x00000000, /* 72 */
	0x00000000, /* 73 */
	0x00000000, /* 74 */
	0x00000000, /* 75 */
	0x00000000, /* 76 */
	0x00000000, /* 77 */
	0x00000000, /* 78 */
	0x00000000, /* 79 */
	0x00000000, /* 7a */
	0x00000000, /* 7b */
	0x00000000, /* 7c */
	0x00000000, /* 7d */
	0x00071409, /* 7e */
	0xf4596f59, /* 7f */
};

/* Dtram values for 480p DVI, drop 1 line from compositor */
/* From vec_dit_480p_dvi_drop1.bss */
static const uint32_t
s_aulDtRamBVBInput_DVI_480p_Drop1[BVDC_P_DTRAM_TABLE_SIZE] =
{
	0x0029003e, /* 40 */
	0x00280031, /* 41 */
	0x0065d006, /* 42 */
	0x00651001, /* 43 */
	0x00655005, /* 44 */
	0x00659001, /* 45 */
	0x0065d01c, /* 46 */
	0x00661001, /* 47 */
	0x0066d001, /* 48 */
	0x006651df, /* 49 */
	0x00669001, /* 4a */
	0x0065d001, /* 4b */
	0x0064e001, /* 4c */
	0x00841000, /* 4d */
	0x002012d0, /* 4e */
	0x00200010, /* 4f */
	0x00390033, /* 50 */
	0x002012d0, /* 51 */
	0x00200010, /* 52 */
	0x0025003e, /* 53 */
	0x00340031, /* 54 */
	0x002412d0, /* 55 */
	0x00240010, /* 56 */
	0x0025003e, /* 57 */
	0x00340031, /* 58 */
	0x002412d0, /* 59 */
	0x00240010, /* 5a */
	0x0021003e, /* 5b */
	0x00300031, /* 5c */
	0x002012d0, /* 5d */
	0x00200010, /* 5e */
	0x0021003e, /* 5f */
	0x00300031, /* 60 */
	0x002012d0, /* 61 */
	0x00200010, /* 62 */
	0x0021003e, /* 63 */
	0x00302031, /* 64 */
	0x002232d0, /* 65 */
	0x00202010, /* 66 */
	0x0021203e, /* 67 */
	0x00302031, /* 68 */
	0x002232d0, /* 69 */
	0x00202010, /* 6a */
	0x0021003e, /* 6b */
	0x00300031, /* 6c */
	0x002032d0, /* 6d */
	0x00202010, /* 6e */
	0x0021203e, /* 6f */
	0x00302031, /* 70 */
	0x00000000, /* 71 */
	0x00000000, /* 72 */
	0x00000000, /* 73 */
	0x00000000, /* 74 */
	0x00000000, /* 75 */
	0x00000000, /* 76 */
	0x00000000, /* 77 */
	0x00000000, /* 78 */
	0x00000000, /* 79 */
	0x00000000, /* 7a */
	0x00000000, /* 7b */
	0x00000000, /* 7c */
	0x00000000, /* 7d */
	0x00012910, /* 7e */
	0xffa84dd1, /* 7f */
};

/* Dtram values for 480p DVI, drop 2 lines from compositor */
/* From vec_dit_480p_dvi_drop2.bss */
static const uint32_t
s_aulDtRamBVBInput_DVI_480p_Drop2[BVDC_P_DTRAM_TABLE_SIZE] =
{
	0x0029003e, /* 40 */
	0x00280031, /* 41 */
	0x0065c007, /* 42 */
	0x00650001, /* 43 */
	0x00654005, /* 44 */
	0x00658001, /* 45 */
	0x0065c01b, /* 46 */
	0x00660001, /* 47 */
	0x0066c002, /* 48 */
	0x006641df, /* 49 */
	0x00668001, /* 4a */
	0x0064d001, /* 4b */
	0x00841000, /* 4c */
	0x002012d0, /* 4d */
	0x00200010, /* 4e */
	0x00390033, /* 4f */
	0x002012d0, /* 50 */
	0x00200010, /* 51 */
	0x0025003e, /* 52 */
	0x00340031, /* 53 */
	0x002412d0, /* 54 */
	0x00240010, /* 55 */
	0x0025003e, /* 56 */
	0x00340031, /* 57 */
	0x002412d0, /* 58 */
	0x00240010, /* 59 */
	0x0021003e, /* 5a */
	0x00300031, /* 5b */
	0x002012d0, /* 5c */
	0x00200010, /* 5d */
	0x0021003e, /* 5e */
	0x00300031, /* 5f */
	0x002012d0, /* 60 */
	0x00200010, /* 61 */
	0x0021003e, /* 62 */
	0x00302031, /* 63 */
	0x002232d0, /* 64 */
	0x00202010, /* 65 */
	0x0021203e, /* 66 */
	0x00302031, /* 67 */
	0x002232d0, /* 68 */
	0x00202010, /* 69 */
	0x0021003e, /* 6a */
	0x00300031, /* 6b */
	0x002032d0, /* 6c */
	0x00202010, /* 6d */
	0x0021203e, /* 6e */
	0x00302031, /* 6f */
	0x00000000, /* 70 */
	0x00000000, /* 71 */
	0x00000000, /* 72 */
	0x00000000, /* 73 */
	0x00000000, /* 74 */
	0x00000000, /* 75 */
	0x00000000, /* 76 */
	0x00000000, /* 77 */
	0x00000000, /* 78 */
	0x00000000, /* 79 */
	0x00000000, /* 7a */
	0x00000000, /* 7b */
	0x00000000, /* 7c */
	0x00000000, /* 7d */
	0x00012910, /* 7e */
	0x19a6f781, /* 7f */
};

/* Dtram values for 480p DVI, drop 3 lines from compositor */
/* From vec_dit_480p_dvi_drop3.bss */
static const uint32_t
s_aulDtRamBVBInput_DVI_480p_Drop3[BVDC_P_DTRAM_TABLE_SIZE] =
{
	0x0029003e, /* 40 */
	0x00280031, /* 41 */
	0x00658008, /* 42 */
	0x0064c001, /* 43 */
	0x00650005, /* 44 */
	0x00654001, /* 45 */
	0x0065801a, /* 46 */
	0x0065c001, /* 47 */
	0x00667003, /* 48 */
	0x006601df, /* 49 */
	0x00664001, /* 4a */
	0x00841000, /* 4b */
	0x002012d0, /* 4c */
	0x00200010, /* 4d */
	0x0025003e, /* 4e */
	0x00340031, /* 4f */
	0x002412d0, /* 50 */
	0x00240010, /* 51 */
	0x0025003e, /* 52 */
	0x00340031, /* 53 */
	0x002412d0, /* 54 */
	0x00240010, /* 55 */
	0x0021003e, /* 56 */
	0x00300031, /* 57 */
	0x002012d0, /* 58 */
	0x00200010, /* 59 */
	0x0021003e, /* 5a */
	0x00300031, /* 5b */
	0x002012d0, /* 5c */
	0x00200010, /* 5d */
	0x0021003e, /* 5e */
	0x00302031, /* 5f */
	0x002232d0, /* 60 */
	0x00202010, /* 61 */
	0x0021203e, /* 62 */
	0x00302031, /* 63 */
	0x002232d0, /* 64 */
	0x00202010, /* 65 */
	0x00390033, /* 66 */
	0x002032d0, /* 67 */
	0x00202010, /* 68 */
	0x0021203e, /* 69 */
	0x00302031, /* 6a */
	0x00000000, /* 6b */
	0x00000000, /* 6c */
	0x00000000, /* 6d */
	0x00000000, /* 6e */
	0x00000000, /* 6f */
	0x00000000, /* 70 */
	0x00000000, /* 71 */
	0x00000000, /* 72 */
	0x00000000, /* 73 */
	0x00000000, /* 74 */
	0x00000000, /* 75 */
	0x00000000, /* 76 */
	0x00000000, /* 77 */
	0x00000000, /* 78 */
	0x00000000, /* 79 */
	0x00000000, /* 7a */
	0x00000000, /* 7b */
	0x00000000, /* 7c */
	0x00000000, /* 7d */
	0x00012910, /* 7e */
	0x6a71adc6, /* 7f */
};

/* Dtram values for 480p DVI, drop 0 lines from compositor */
/* From vec_dit_480p_rm54_dvi.bss */
static const uint32_t
s_aulDtRamBVBInput_DVI_480p_Drop0_54MHz[BVDC_P_DTRAM_TABLE_SIZE] =
{
	0x0029007c, /* 40 */
	0x0028006d, /* 41 */
	0x0065c005, /* 42 */
	0x00650001, /* 43 */
	0x00654005, /* 44 */
	0x00658001, /* 45 */
	0x0065c01d, /* 46 */
	0x00660001, /* 47 */
	0x006641df, /* 48 */
	0x00668001, /* 49 */
	0x0065c002, /* 4a */
	0x0064d001, /* 4b */
	0x00841000, /* 4c */
	0x002015a0, /* 4d */
	0x00200020, /* 4e */
	0x00390071, /* 4f */
	0x002015a0, /* 50 */
	0x00200020, /* 51 */
	0x0025007c, /* 52 */
	0x0034006d, /* 53 */
	0x002415a0, /* 54 */
	0x00240020, /* 55 */
	0x0025007c, /* 56 */
	0x0034006d, /* 57 */
	0x002415a0, /* 58 */
	0x00240020, /* 59 */
	0x0021007c, /* 5a */
	0x0030006d, /* 5b */
	0x002015a0, /* 5c */
	0x00200020, /* 5d */
	0x0021007c, /* 5e */
	0x0030006d, /* 5f */
	0x002015a0, /* 60 */
	0x00200020, /* 61 */
	0x0021007c, /* 62 */
	0x0030206d, /* 63 */
	0x002235a0, /* 64 */
	0x00202020, /* 65 */
	0x0021207c, /* 66 */
	0x0030206d, /* 67 */
	0x002235a0, /* 68 */
	0x00202020, /* 69 */
	0x0021007c, /* 6a */
	0x0030006d, /* 6b */
	0x00000000, /* 6c */
	0x00000000, /* 6d */
	0x00000000, /* 6e */
	0x00000000, /* 6f */
	0x00000000, /* 70 */
	0x00000000, /* 71 */
	0x00000000, /* 72 */
	0x00000000, /* 73 */
	0x00000000, /* 74 */
	0x00000000, /* 75 */
	0x00000000, /* 76 */
	0x00000000, /* 77 */
	0x00000000, /* 78 */
	0x00000000, /* 79 */
	0x00000000, /* 7a */
	0x00000000, /* 7b */
	0x00000000, /* 7c */
	0x00000000, /* 7d */
	0x00120309, /* 7e */
	0x6b5192b1, /* 7f */
};

/* Dtram values for 480p DVI, drop 1 line from compositor */
/* From vec_dit_480p_rm54_dvi_drop1.bss */
static const uint32_t
s_aulDtRamBVBInput_DVI_480p_Drop1_54MHz[BVDC_P_DTRAM_TABLE_SIZE] =
{
	0x0029007c, /* 40 */
	0x0028006d, /* 41 */
	0x0065d006, /* 42 */
	0x00651001, /* 43 */
	0x00655005, /* 44 */
	0x00659001, /* 45 */
	0x0065d01c, /* 46 */
	0x00661001, /* 47 */
	0x0066d001, /* 48 */
	0x006651df, /* 49 */
	0x00669001, /* 4a */
	0x0065d001, /* 4b */
	0x0064e001, /* 4c */
	0x00841000, /* 4d */
	0x002015a0, /* 4e */
	0x00200020, /* 4f */
	0x00390071, /* 50 */
	0x002015a0, /* 51 */
	0x00200020, /* 52 */
	0x0025007c, /* 53 */
	0x0034006d, /* 54 */
	0x002415a0, /* 55 */
	0x00240020, /* 56 */
	0x0025007c, /* 57 */
	0x0034006d, /* 58 */
	0x002415a0, /* 59 */
	0x00240020, /* 5a */
	0x0021007c, /* 5b */
	0x0030006d, /* 5c */
	0x002015a0, /* 5d */
	0x00200020, /* 5e */
	0x0021007c, /* 5f */
	0x0030006d, /* 60 */
	0x002015a0, /* 61 */
	0x00200020, /* 62 */
	0x0021007c, /* 63 */
	0x0030206d, /* 64 */
	0x002235a0, /* 65 */
	0x00202020, /* 66 */
	0x0021207c, /* 67 */
	0x0030206d, /* 68 */
	0x002235a0, /* 69 */
	0x00202020, /* 6a */
	0x0021007c, /* 6b */
	0x0030006d, /* 6c */
	0x002035a0, /* 6d */
	0x00202020, /* 6e */
	0x0021207c, /* 6f */
	0x0030206d, /* 70 */
	0x00000000, /* 71 */
	0x00000000, /* 72 */
	0x00000000, /* 73 */
	0x00000000, /* 74 */
	0x00000000, /* 75 */
	0x00000000, /* 76 */
	0x00000000, /* 77 */
	0x00000000, /* 78 */
	0x00000000, /* 79 */
	0x00000000, /* 7a */
	0x00000000, /* 7b */
	0x00000000, /* 7c */
	0x00000000, /* 7d */
	0x00020110, /* 7e */
	0xed945b0e, /* 7f */
};

/* Dtram values for 480p DVI, drop 2 lines from compositor */
/* From vec_dit_480p_rm54_dvi_drop2.bss */
static const uint32_t
s_aulDtRamBVBInput_DVI_480p_Drop2_54MHz[BVDC_P_DTRAM_TABLE_SIZE] =
{
	0x0029007c, /* 40 */
	0x0028006d, /* 41 */
	0x0065c007, /* 42 */
	0x00650001, /* 43 */
	0x00654005, /* 44 */
	0x00658001, /* 45 */
	0x0065c01b, /* 46 */
	0x00660001, /* 47 */
	0x0066c002, /* 48 */
	0x006641df, /* 49 */
	0x00668001, /* 4a */
	0x0064d001, /* 4b */
	0x00841000, /* 4c */
	0x002015a0, /* 4d */
	0x00200020, /* 4e */
	0x00390071, /* 4f */
	0x002015a0, /* 50 */
	0x00200020, /* 51 */
	0x0025007c, /* 52 */
	0x0034006d, /* 53 */
	0x002415a0, /* 54 */
	0x00240020, /* 55 */
	0x0025007c, /* 56 */
	0x0034006d, /* 57 */
	0x002415a0, /* 58 */
	0x00240020, /* 59 */
	0x0021007c, /* 5a */
	0x0030006d, /* 5b */
	0x002015a0, /* 5c */
	0x00200020, /* 5d */
	0x0021007c, /* 5e */
	0x0030006d, /* 5f */
	0x002015a0, /* 60 */
	0x00200020, /* 61 */
	0x0021007c, /* 62 */
	0x0030206d, /* 63 */
	0x002235a0, /* 64 */
	0x00202020, /* 65 */
	0x0021207c, /* 66 */
	0x0030206d, /* 67 */
	0x002235a0, /* 68 */
	0x00202020, /* 69 */
	0x0021007c, /* 6a */
	0x0030006d, /* 6b */
	0x002035a0, /* 6c */
	0x00202020, /* 6d */
	0x0021207c, /* 6e */
	0x0030206d, /* 6f */
	0x00000000, /* 70 */
	0x00000000, /* 71 */
	0x00000000, /* 72 */
	0x00000000, /* 73 */
	0x00000000, /* 74 */
	0x00000000, /* 75 */
	0x00000000, /* 76 */
	0x00000000, /* 77 */
	0x00000000, /* 78 */
	0x00000000, /* 79 */
	0x00000000, /* 7a */
	0x00000000, /* 7b */
	0x00000000, /* 7c */
	0x00000000, /* 7d */
	0x00020110, /* 7e */
	0xb21a6020, /* 7f */
};

/* Dtram values for 480p DVI, drop 3 lines from compositor */
/* From vec_dit_480p_rm54_dvi_drop3.bss */
static const uint32_t
s_aulDtRamBVBInput_DVI_480p_Drop3_54MHz[BVDC_P_DTRAM_TABLE_SIZE] =
{
	0x0029007c, /* 40 */
	0x0028006d, /* 41 */
	0x00658008, /* 42 */
	0x0064c001, /* 43 */
	0x00650005, /* 44 */
	0x00654001, /* 45 */
	0x0065801a, /* 46 */
	0x0065c001, /* 47 */
	0x00667003, /* 48 */
	0x006601df, /* 49 */
	0x00664001, /* 4a */
	0x00841000, /* 4b */
	0x002015a0, /* 4c */
	0x00200020, /* 4d */
	0x0025007c, /* 4e */
	0x0034006d, /* 4f */
	0x002415a0, /* 50 */
	0x00240020, /* 51 */
	0x0025007c, /* 52 */
	0x0034006d, /* 53 */
	0x002415a0, /* 54 */
	0x00240020, /* 55 */
	0x0021007c, /* 56 */
	0x0030006d, /* 57 */
	0x002015a0, /* 58 */
	0x00200020, /* 59 */
	0x0021007c, /* 5a */
	0x0030006d, /* 5b */
	0x002015a0, /* 5c */
	0x00200020, /* 5d */
	0x0021007c, /* 5e */
	0x0030206d, /* 5f */
	0x002235a0, /* 60 */
	0x00202020, /* 61 */
	0x0021207c, /* 62 */
	0x0030206d, /* 63 */
	0x002235a0, /* 64 */
	0x00202020, /* 65 */
	0x00390071, /* 66 */
	0x002035a0, /* 67 */
	0x00202020, /* 68 */
	0x0021207c, /* 69 */
	0x0030206d, /* 6a */
	0x00000000, /* 6b */
	0x00000000, /* 6c */
	0x00000000, /* 6d */
	0x00000000, /* 6e */
	0x00000000, /* 6f */
	0x00000000, /* 70 */
	0x00000000, /* 71 */
	0x00000000, /* 72 */
	0x00000000, /* 73 */
	0x00000000, /* 74 */
	0x00000000, /* 75 */
	0x00000000, /* 76 */
	0x00000000, /* 77 */
	0x00000000, /* 78 */
	0x00000000, /* 79 */
	0x00000000, /* 7a */
	0x00000000, /* 7b */
	0x00000000, /* 7c */
	0x00000000, /* 7d */
	0x00020110, /* 7e */
	0xee6a15de, /* 7f */
};

/* Dtram values for 576p_50Hz DVI */
/* From vec_dit_625p_dvi.bss */
static const uint32_t s_aulDtRamBVBInput_DVI_576p[BVDC_P_DTRAM_TABLE_SIZE] =
{
	0x00649001, /* 40 */
	0x0064e003, /* 41 */
	0x00653001, /* 42 */
	0x00658027, /* 43 */
	0x0065d001, /* 44 */
	0x0066223f, /* 45 */
	0x00658004, /* 46 */
	0x00667001, /* 47 */
	0x00840000, /* 48 */
	0x002d0014, /* 49 */
	0x002c0044, /* 4a */
	0x002412d0, /* 4b */
	0x0024000c, /* 4c */
	0x0035000b, /* 4d */
	0x0025002a, /* 4e */
	0x00240044, /* 4f */
	0x002412d0, /* 50 */
	0x0024000c, /* 51 */
	0x0035000b, /* 52 */
	0x0025002a, /* 53 */
	0x00240044, /* 54 */
	0x002412d0, /* 55 */
	0x0024000c, /* 56 */
	0x0031000b, /* 57 */
	0x0021002a, /* 58 */
	0x00200044, /* 59 */
	0x002012d0, /* 5a */
	0x0020000c, /* 5b */
	0x0031400b, /* 5c */
	0x0021002a, /* 5d */
	0x00202044, /* 5e */
	0x002232d0, /* 5f */
	0x0020200c, /* 60 */
	0x0031200b, /* 61 */
	0x0021202a, /* 62 */
	0x00202044, /* 63 */
	0x002232d0, /* 64 */
	0x0020200c, /* 65 */
	0x0031200b, /* 66 */
	0x0021002a, /* 67 */
	0x00200044, /* 68 */
	0x002012d0, /* 69 */
	0x0020000c, /* 6a */
	0x003d0016, /* 6b */
	0x00000000, /* 6c */
	0x00000000, /* 6d */
	0x00000000, /* 6e */
	0x00000000, /* 6f */
	0x00000000, /* 70 */
	0x00000000, /* 71 */
	0x00000000, /* 72 */
	0x00000000, /* 73 */
	0x00000000, /* 74 */
	0x00000000, /* 75 */
	0x00000000, /* 76 */
	0x00000000, /* 77 */
	0x00000000, /* 78 */
	0x00000000, /* 79 */
	0x00000000, /* 7a */
	0x00000000, /* 7b */
	0x00000000, /* 7c */
	0x00000000, /* 7d */
	0x00082509, /* 7e */
	0x020e0d76, /* 7f */
};

/* Dtram values for 576p_50Hz DVI @ 54 MHz pixel sampling rate */
/* From vec_dit_625p_rm54_dvi.bss */
static const uint32_t
s_aulDtRamBVBInput_DVI_576p_54MHz[BVDC_P_DTRAM_TABLE_SIZE] =
{
	0x00649001, /* 40 */
	0x0064e003, /* 41 */
	0x00653001, /* 42 */
	0x00658027, /* 43 */
	0x0065d001, /* 44 */
	0x0066223f, /* 45 */
	0x00658004, /* 46 */
	0x00667001, /* 47 */
	0x00840000, /* 48 */
	0x0028405f, /* 49 */
	0x00214088, /* 4a */
	0x002145a0, /* 4b */
	0x00214018, /* 4c */
	0x0030400b, /* 4d */
	0x0020406a, /* 4e */
	0x00214088, /* 4f */
	0x002145a0, /* 50 */
	0x00214018, /* 51 */
	0x0030400b, /* 52 */
	0x0020406a, /* 53 */
	0x00214088, /* 54 */
	0x002145a0, /* 55 */
	0x00214018, /* 56 */
	0x0034400b, /* 57 */
	0x0024406a, /* 58 */
	0x00254088, /* 59 */
	0x002545a0, /* 5a */
	0x00254018, /* 5b */
	0x0034400b, /* 5c */
	0x0024006a, /* 5d */
	0x00252088, /* 5e */
	0x002735a0, /* 5f */
	0x00252018, /* 60 */
	0x0034200b, /* 61 */
	0x0024206a, /* 62 */
	0x00252088, /* 63 */
	0x002735a0, /* 64 */
	0x00252018, /* 65 */
	0x0034200b, /* 66 */
	0x0024406a, /* 67 */
	0x00254088, /* 68 */
	0x002545a0, /* 69 */
	0x00254018, /* 6a */
	0x0038400b, /* 6b */
	0x00000000, /* 6c */
	0x00000000, /* 6d */
	0x00000000, /* 6e */
	0x00000000, /* 6f */
	0x00000000, /* 70 */
	0x00000000, /* 71 */
	0x00000000, /* 72 */
	0x00000000, /* 73 */
	0x00000000, /* 74 */
	0x00000000, /* 75 */
	0x00000000, /* 76 */
	0x00000000, /* 77 */
	0x00000000, /* 78 */
	0x00000000, /* 79 */
	0x00000000, /* 7a */
	0x00000000, /* 7b */
	0x00000000, /* 7c */
	0x00000000, /* 7d */
	0x00010808, /* 7e */
	0x0958d564, /* 7f */
};

static const uint32_t* const
	s_aulDtRamBVBInput_DVI_480i_DropTbl[BVDC_P_480i_DROP_TABLE_SIZE] =
{
	s_aulDtRamBVBInput_DVI_480i_Drop0,
	s_aulDtRamBVBInput_DVI_480i_Drop1,
};

static const uint32_t* const
	s_aulDtRamBVBInput_DVI_480p_DropTbl[BVDC_P_480p_DROP_TABLE_SIZE] =
{
	s_aulDtRamBVBInput_DVI_480p_Drop0,
	s_aulDtRamBVBInput_DVI_480p_Drop1,
	s_aulDtRamBVBInput_DVI_480p_Drop2,
	s_aulDtRamBVBInput_DVI_480p_Drop3
};

static const uint32_t* const
	s_aulDtRamBVBInput_DVI_480p_54MHz_DropTbl[BVDC_P_480p_DROP_TABLE_SIZE] =
{
	s_aulDtRamBVBInput_DVI_480p_Drop0_54MHz,
	s_aulDtRamBVBInput_DVI_480p_Drop1_54MHz,
	s_aulDtRamBVBInput_DVI_480p_Drop2_54MHz,
	s_aulDtRamBVBInput_DVI_480p_Drop3_54MHz
};


/* NTSC Composite/SVideo VF_FORMAT_ADDER -> VF_SYNC_TRANS1 */
static const uint32_t s_aulVfTable_YQI[BVDC_P_VF_TABLE_SIZE+1] =
{
	BVDC_P_MAKE_VF_FORMAT_ADDER(1, 0, 0, 1, 0, 0, 1, 0, 1, 1, 1, 0, 0),
#if BVDC_P_ORTHOGONAL_VEC
	BVDC_P_MAKE_VF_MISC(SUM_1024, ON, 0, ON, ON),
#else
	BVDC_P_MAKE_VF_MISC(SUM_256, ON, 0, ON, ON),
#endif
	BVDC_P_MAKE_VF_SYNC(
		BVDC_P_NEG_SYNC_AMPLITUDE_VALUE(BVDC_P_DAC_OUTPUT_NTSC_SYNC_LEVEL),
		BVDC_P_NEG_SYNC_AMPLITUDE_VALUE(BVDC_P_DAC_OUTPUT_NTSC_SYNC_LEVEL),
		BVDC_P_NEG_SYNC_TIP_VALUE,
		BVDC_P_POS_SYNC_AMPLITUDE_VALUE(BVDC_P_DAC_OUTPUT_NTSC_PEAK_WHITE_LEVEL),
		0x0),
	BVDC_P_MAKE_VF_SYNC_TRANS(0,  64, 26,  8,  0),
	BVDC_P_MAKE_VF_SYNC_TRANS(1, 100, 72, 46, 20),
	BVDC_P_MAKE_VF_SYNC_EXTN(
		0,
		0,
		BVDC_P_NEG_SYNC_AMPLITUDE_VALUE(BVDC_P_DAC_OUTPUT_NTSC_SYNC_LEVEL))
};

/* PAL Composite/SVideo VF_FORMAT_ADDER -> VF_SYNC_TRANS1 */
static const uint32_t s_aulVfTable_YUV[BVDC_P_VF_TABLE_SIZE+1] =
{
	BVDC_P_MAKE_VF_FORMAT_ADDER(1, 0, 0, 1, 0, 0, 1, 0, 1, 1, 1, 0, 0),
#if BVDC_P_ORTHOGONAL_VEC
	BVDC_P_MAKE_VF_MISC(SUM_1024, ON, 0, ON, ON),
#else
	BVDC_P_MAKE_VF_MISC(SUM_256, ON, 0, ON, ON),
#endif
	BVDC_P_MAKE_VF_SYNC(
		BVDC_P_NEG_SYNC_AMPLITUDE_VALUE(BVDC_P_DAC_OUTPUT_SYNC_LEVEL),
		BVDC_P_NEG_SYNC_AMPLITUDE_VALUE(BVDC_P_DAC_OUTPUT_SYNC_LEVEL),
		BVDC_P_NEG_SYNC_TIP_VALUE,
		BVDC_P_POS_SYNC_AMPLITUDE_VALUE(BVDC_P_DAC_OUTPUT_PEAK_WHITE_LEVEL),
		0x0),
	BVDC_P_MAKE_VF_SYNC_TRANS(0, 0x58, 0x2A, 0x12, 0x05),
	BVDC_P_MAKE_VF_SYNC_TRANS(1, 0x44, 0x33, 0x10, 5),
	BVDC_P_MAKE_VF_SYNC_EXTN(
		0, 0, BVDC_P_NEG_SYNC_AMPLITUDE_VALUE(BVDC_P_DAC_OUTPUT_SYNC_LEVEL))
};

/* SD YPbPr VF_FORMAT_ADDER -> VF_SYNC_TRANS1 */
static const uint32_t s_aulVfTable_SD_YPbPr[BVDC_P_VF_TABLE_SIZE+1] =
{
#if BVDC_P_ORTHOGONAL_VEC
	BVDC_P_MAKE_VF_FORMAT_ADDER(1, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 512),
	BVDC_P_MAKE_VF_MISC(SUM_1024, ON, 0, ON, ON),
#else
	BVDC_P_MAKE_VF_FORMAT_ADDER(1, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 364),
	BVDC_P_MAKE_VF_MISC(SUM_256, ON, 0, ON, ON),
#endif
	BVDC_P_MAKE_VF_SYNC(
		BVDC_P_NEG_SYNC_AMPLITUDE_VALUE(BVDC_P_DAC_OUTPUT_SYNC_LEVEL),
		BVDC_P_NEG_SYNC_AMPLITUDE_VALUE(BVDC_P_DAC_OUTPUT_SYNC_LEVEL),
		BVDC_P_NEG_SYNC_TIP_VALUE,
		BVDC_P_POS_SYNC_AMPLITUDE_VALUE(BVDC_P_DAC_OUTPUT_PEAK_WHITE_LEVEL),
		0x0),
	BVDC_P_MAKE_VF_SYNC_TRANS(0, 64, 23, 0, 0),
	BVDC_P_MAKE_VF_SYNC_TRANS(1, 64, 23, 0, 0),
	BVDC_P_MAKE_VF_SYNC_EXTN(
		0, 0, BVDC_P_NEG_SYNC_AMPLITUDE_VALUE(BVDC_P_DAC_OUTPUT_SYNC_LEVEL))
};

/* SD YPbPr VF_FORMAT_ADDER -> VF_SYNC_TRANS1 */
static const uint32_t s_aulVfTable_SD_YPbPr_50Hz[BVDC_P_VF_TABLE_SIZE+1] =
{
	BVDC_P_MAKE_VF_FORMAT_ADDER(1, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 330),
#if BVDC_P_ORTHOGONAL_VEC
	BVDC_P_MAKE_VF_MISC(SUM_1024, ON, 0, ON, ON),
#else
	BVDC_P_MAKE_VF_MISC(SUM_256, ON, 0, ON, ON),
#endif
	BVDC_P_MAKE_VF_SYNC(
		BVDC_P_NEG_SYNC_AMPLITUDE_VALUE(BVDC_P_DAC_OUTPUT_SYNC_LEVEL),
		BVDC_P_NEG_SYNC_AMPLITUDE_VALUE(BVDC_P_DAC_OUTPUT_SYNC_LEVEL),
		BVDC_P_NEG_SYNC_TIP_VALUE,
		BVDC_P_POS_SYNC_AMPLITUDE_VALUE(BVDC_P_DAC_OUTPUT_PEAK_WHITE_LEVEL),
		0x0),
	BVDC_P_MAKE_VF_SYNC_TRANS(0, 84, 42, 21, 0),
	BVDC_P_MAKE_VF_SYNC_TRANS(1, 100, 72, 46, 20),
	BVDC_P_MAKE_VF_SYNC_EXTN(
		0, 0, BVDC_P_NEG_SYNC_AMPLITUDE_VALUE(BVDC_P_DAC_OUTPUT_SYNC_LEVEL))
};

/* SD RGB VF_FORMAT_ADDER -> VF_SYNC_TRANS1 */
static const uint32_t s_aulVfTable_SD_RGB[BVDC_P_VF_TABLE_SIZE+1] =
{
#if defined(HUMAX_PLATFORM_BASE)
	BVDC_P_MAKE_VF_FORMAT_ADDER(0, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0xFB),
#else
	BVDC_P_MAKE_VF_FORMAT_ADDER(0, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0x10),
#endif
#if BVDC_P_ORTHOGONAL_VEC
	BVDC_P_MAKE_VF_MISC(SUM_1024, ON, 0, ON, ON),
#else
	BVDC_P_MAKE_VF_MISC(SUM_256, ON, 0, ON, ON),
#endif
	BVDC_P_MAKE_VF_SYNC(
		BVDC_P_NEG_SYNC_AMPLITUDE_VALUE(BVDC_P_DAC_OUTPUT_SYNC_LEVEL),
		BVDC_P_NEG_SYNC_AMPLITUDE_VALUE(BVDC_P_DAC_OUTPUT_SYNC_LEVEL),
		BVDC_P_NEG_SYNC_TIP_VALUE,
		BVDC_P_POS_SYNC_AMPLITUDE_VALUE(BVDC_P_DAC_OUTPUT_PEAK_WHITE_LEVEL),
		0x0),
	BVDC_P_MAKE_VF_SYNC_TRANS(0, 70, 28, 7, 0),
	BVDC_P_MAKE_VF_SYNC_TRANS(1, 100, 72, 46, 20),
	BVDC_P_MAKE_VF_SYNC_EXTN(
		0, 0, BVDC_P_NEG_SYNC_AMPLITUDE_VALUE(BVDC_P_DAC_OUTPUT_SYNC_LEVEL))
};

/* SD RGB VF_FORMAT_ADDER -> VF_SYNC_TRANS1 */
static const uint32_t s_aulVfTable_SD_RGB_50Hz[BVDC_P_VF_TABLE_SIZE+1] =
{
#if defined(HUMAX_PLATFORM_BASE)
	BVDC_P_MAKE_VF_FORMAT_ADDER(0, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0xFB),
#else
	BVDC_P_MAKE_VF_FORMAT_ADDER(0, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0x10),
#endif
#if BVDC_P_ORTHOGONAL_VEC
	BVDC_P_MAKE_VF_MISC(SUM_1024, ON, 0, ON, ON),
#else
	BVDC_P_MAKE_VF_MISC(SUM_256, ON, 0, ON, ON),
#endif
	BVDC_P_MAKE_VF_SYNC(
		BVDC_P_NEG_SYNC_AMPLITUDE_VALUE(BVDC_P_DAC_OUTPUT_SYNC_LEVEL),
		BVDC_P_NEG_SYNC_AMPLITUDE_VALUE(BVDC_P_DAC_OUTPUT_SYNC_LEVEL),
		BVDC_P_NEG_SYNC_TIP_VALUE,
		BVDC_P_POS_SYNC_AMPLITUDE_VALUE(BVDC_P_DAC_OUTPUT_PEAK_WHITE_LEVEL),
		0x0),
	BVDC_P_MAKE_VF_SYNC_TRANS(0, 84, 42, 21, 0),
	BVDC_P_MAKE_VF_SYNC_TRANS(1, 100, 72, 46, 20),
	BVDC_P_MAKE_VF_SYNC_EXTN(
		0, 0, BVDC_P_NEG_SYNC_AMPLITUDE_VALUE(BVDC_P_DAC_OUTPUT_SYNC_LEVEL))
};

/* SD Hsync-only VF_FORMAT_ADDER -> VF_SYNC_TRANS1, used with RGB outputs */
static const uint32_t s_aulVfTable_SD_Hsync[BVDC_P_VF_TABLE_SIZE+1] =
{
	BVDC_P_MAKE_VF_FORMAT_ADDER(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0),
#if BVDC_P_ORTHOGONAL_VEC
	BVDC_P_MAKE_VF_MISC(SUM_1024, ON, 0, ON, OFF),
#else
	BVDC_P_MAKE_VF_MISC(SUM_256, ON, 0, ON, OFF),
#endif
	BVDC_P_MAKE_VF_SYNC(
		BVDC_P_NEG_SYNC_AMPLITUDE_VALUE(BVDC_P_DAC_OUTPUT_SYNC_LEVEL),
		BVDC_P_NEG_SYNC_AMPLITUDE_VALUE(BVDC_P_DAC_OUTPUT_SYNC_LEVEL),
		BVDC_P_NEG_SYNC_TIP_VALUE,
		BVDC_P_POS_SYNC_AMPLITUDE_VALUE(BVDC_P_DAC_OUTPUT_PEAK_WHITE_LEVEL),
		0x0),
	BVDC_P_MAKE_VF_SYNC_TRANS(0, 70, 28, 7, 0),
	BVDC_P_MAKE_VF_SYNC_TRANS(1, 100, 72, 46, 20),
	BVDC_P_MAKE_VF_SYNC_EXTN(
		0, 0, BVDC_P_NEG_SYNC_AMPLITUDE_VALUE(BVDC_P_DAC_OUTPUT_SYNC_LEVEL))
};

/* VF_FORMAT_ADDER -> VF_SYNC_TRANS1 */
static const uint32_t s_aulVfTable_1080i_720p_YPbPr[BVDC_P_VF_TABLE_SIZE+1] =
{
	BVDC_P_MAKE_VF_FORMAT_ADDER(0, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 330),
#if BVDC_P_ORTHOGONAL_VEC
	BVDC_P_MAKE_VF_MISC(SUM_1024, OFF, 0, OFF, ON),
#else
	BVDC_P_MAKE_VF_MISC(SUM_512, OFF, 0, OFF, ON),
#endif
	BVDC_P_MAKE_VF_SYNC(
		BVDC_P_NEG_SYNC_AMPLITUDE_VALUE(BVDC_P_DAC_OUTPUT_SYNC_LEVEL),
		BVDC_P_NEG_SYNC_AMPLITUDE_VALUE(BVDC_P_DAC_OUTPUT_SYNC_LEVEL),
		BVDC_P_NEG_SYNC_TIP_VALUE, 0x0eb, 0x0),
	BVDC_P_MAKE_VF_SYNC_TRANS(0, 70, 28, 7, 0),
	BVDC_P_MAKE_VF_SYNC_TRANS(1, 70, 28, 7, 0),
	BVDC_P_MAKE_VF_SYNC_EXTN(
		0, 0, BVDC_P_NEG_SYNC_AMPLITUDE_VALUE(BVDC_P_DAC_OUTPUT_SYNC_LEVEL))
};

/* VF_FORMAT_ADDER -> VF_SYNC_TRANS1 */
static const uint32_t s_aulVfTable_1080i_720p_RGB[BVDC_P_VF_TABLE_SIZE+1] =
{
#if defined(HUMAX_PLATFORM_BASE)
	BVDC_P_MAKE_VF_FORMAT_ADDER(0, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0xFB),
#else
	BVDC_P_MAKE_VF_FORMAT_ADDER(0, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0x10),
#endif
#if BVDC_P_ORTHOGONAL_VEC
	BVDC_P_MAKE_VF_MISC(SUM_1024, OFF, 0, OFF, ON),
#else
	BVDC_P_MAKE_VF_MISC(SUM_512, OFF, 0, OFF, ON),
#endif
	BVDC_P_MAKE_VF_SYNC(
		BVDC_P_NEG_SYNC_AMPLITUDE_VALUE(BVDC_P_DAC_OUTPUT_SYNC_LEVEL),
		BVDC_P_NEG_SYNC_AMPLITUDE_VALUE(BVDC_P_DAC_OUTPUT_SYNC_LEVEL),
		BVDC_P_NEG_SYNC_TIP_VALUE, 0x0eb, 0x0),
	BVDC_P_MAKE_VF_SYNC_TRANS(0, 70, 28, 7, 0),
	BVDC_P_MAKE_VF_SYNC_TRANS(1, 70, 28, 7, 0),
	BVDC_P_MAKE_VF_SYNC_EXTN(
		0, 0, BVDC_P_NEG_SYNC_AMPLITUDE_VALUE(BVDC_P_DAC_OUTPUT_SYNC_LEVEL))
};

/* HD Hsync-only VF_FORMAT_ADDER -> VF_SYNC_TRANS1 */
static const uint32_t s_aulVfTable_HD_Hsync[BVDC_P_VF_TABLE_SIZE+1] =
{
	BVDC_P_MAKE_VF_FORMAT_ADDER(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0),
#if BVDC_P_ORTHOGONAL_VEC
	BVDC_P_MAKE_VF_MISC(SUM_1024, OFF, 0, OFF, OFF),
#else
	BVDC_P_MAKE_VF_MISC(SUM_512, OFF, 0, OFF, OFF),
#endif
	BVDC_P_MAKE_VF_SYNC(
		BVDC_P_NEG_SYNC_AMPLITUDE_VALUE(BVDC_P_DAC_OUTPUT_SYNC_LEVEL),
		BVDC_P_NEG_SYNC_AMPLITUDE_VALUE(BVDC_P_DAC_OUTPUT_SYNC_LEVEL),
		BVDC_P_NEG_SYNC_TIP_VALUE, 0x0eb, 0x0),
	BVDC_P_MAKE_VF_SYNC_TRANS(0, 70, 28, 7, 0),
	BVDC_P_MAKE_VF_SYNC_TRANS(1, 70, 28, 7, 0),
	BVDC_P_MAKE_VF_SYNC_EXTN(
		0, 0, BVDC_P_NEG_SYNC_AMPLITUDE_VALUE(BVDC_P_DAC_OUTPUT_SYNC_LEVEL))
};

/* VF_FORMAT_ADDER -> VF_SYNC_TRANS1 */
static const uint32_t s_aulVfTable_480p_YPbPr[BVDC_P_VF_TABLE_SIZE+1] =
{
#if BVDC_P_ORTHOGONAL_VEC
	BVDC_P_MAKE_VF_FORMAT_ADDER(1, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 512),
#else
	BVDC_P_MAKE_VF_FORMAT_ADDER(1, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 330),
#endif
	BVDC_P_MAKE_VF_MISC(SUM_512, OFF, 0, OFF, ON),
	BVDC_P_MAKE_VF_SYNC(
		BVDC_P_NEG_SYNC_AMPLITUDE_VALUE(BVDC_P_DAC_OUTPUT_SYNC_LEVEL),
		BVDC_P_NEG_SYNC_AMPLITUDE_VALUE(BVDC_P_DAC_OUTPUT_SYNC_LEVEL),
		BVDC_P_NEG_SYNC_TIP_VALUE, 0x0eb, 0x0),
	BVDC_P_MAKE_VF_SYNC_TRANS(0, 23, 3, 0, 0),
	BVDC_P_MAKE_VF_SYNC_TRANS(1, 23, 3, 0, 0),
	BVDC_P_MAKE_VF_SYNC_EXTN(
		0, 0, BVDC_P_NEG_SYNC_AMPLITUDE_VALUE(BVDC_P_DAC_OUTPUT_SYNC_LEVEL))
};

/* VF_FORMAT_ADDER -> VF_SYNC_TRANS1 */
static const uint32_t s_aulVfTable_576p_YPbPr[BVDC_P_VF_TABLE_SIZE+1] =
{
#if BVDC_P_ORTHOGONAL_VEC
	BVDC_P_MAKE_VF_FORMAT_ADDER(1, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 512),
	BVDC_P_MAKE_VF_MISC(SUM_1024, OFF, 0, OFF, ON),
#else
	BVDC_P_MAKE_VF_FORMAT_ADDER(1, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 330),
	BVDC_P_MAKE_VF_MISC(SUM_512, OFF, 0, OFF, ON),
#endif
	BVDC_P_MAKE_VF_SYNC(
		BVDC_P_NEG_SYNC_AMPLITUDE_VALUE(BVDC_P_DAC_OUTPUT_SYNC_LEVEL),
		BVDC_P_NEG_SYNC_AMPLITUDE_VALUE(BVDC_P_DAC_OUTPUT_SYNC_LEVEL),
		BVDC_P_NEG_SYNC_TIP_VALUE, 0x0eb, 0x0),
	BVDC_P_MAKE_VF_SYNC_TRANS(0, 51, 3, 0, 0),
	BVDC_P_MAKE_VF_SYNC_TRANS(1, 51, 3, 0, 0),
	BVDC_P_MAKE_VF_SYNC_EXTN(
		0, 0, BVDC_P_NEG_SYNC_AMPLITUDE_VALUE(BVDC_P_DAC_OUTPUT_SYNC_LEVEL))
};

/* VF_FORMAT_ADDER -> VF_SYNC_TRANS1 */
static const uint32_t s_aulVfTable_576p_54MHz_YPbPr[BVDC_P_VF_TABLE_SIZE+1] =
{
	BVDC_P_MAKE_VF_FORMAT_ADDER(1, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 330),
#if BVDC_P_ORTHOGONAL_VEC
	BVDC_P_MAKE_VF_MISC(SUM_1024, ON, 0, OFF, ON),
#else
	BVDC_P_MAKE_VF_MISC(SUM_256, ON, 0, OFF, ON),
#endif
	BVDC_P_MAKE_VF_SYNC(
		BVDC_P_NEG_SYNC_AMPLITUDE_VALUE(BVDC_P_DAC_OUTPUT_SYNC_LEVEL),
		BVDC_P_NEG_SYNC_AMPLITUDE_VALUE(BVDC_P_DAC_OUTPUT_SYNC_LEVEL),
		BVDC_P_NEG_SYNC_TIP_VALUE,
		BVDC_P_POS_SYNC_AMPLITUDE_VALUE(BVDC_P_DAC_OUTPUT_PEAK_WHITE_LEVEL),
		0x0),
	BVDC_P_MAKE_VF_SYNC_TRANS(0, 84, 42, 21, 0),
	BVDC_P_MAKE_VF_SYNC_TRANS(1, 100, 72, 46, 20),
	BVDC_P_MAKE_VF_SYNC_EXTN(
		0, 0, BVDC_P_NEG_SYNC_AMPLITUDE_VALUE(BVDC_P_DAC_OUTPUT_SYNC_LEVEL))
};

/* VF_FORMAT_ADDER -> VF_SYNC_TRANS1 */
static const uint32_t s_aulVfTable_480p_RGB[BVDC_P_VF_TABLE_SIZE+1] =
{
#if defined(HUMAX_PLATFORM_BASE)
	BVDC_P_MAKE_VF_FORMAT_ADDER(0, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0xFB),
#else
	BVDC_P_MAKE_VF_FORMAT_ADDER(0, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0x10),
#endif
	BVDC_P_MAKE_VF_MISC(SUM_512, OFF, 0, OFF, ON),
	BVDC_P_MAKE_VF_SYNC(
		BVDC_P_NEG_SYNC_AMPLITUDE_VALUE(BVDC_P_DAC_OUTPUT_SYNC_LEVEL),
		BVDC_P_NEG_SYNC_AMPLITUDE_VALUE(BVDC_P_DAC_OUTPUT_SYNC_LEVEL),
		BVDC_P_NEG_SYNC_TIP_VALUE, 0x0eb, 0x0),
	BVDC_P_MAKE_VF_SYNC_TRANS(0, 70, 28, 7, 0),
	BVDC_P_MAKE_VF_SYNC_TRANS(1, 100, 72, 46, 20),
	BVDC_P_MAKE_VF_SYNC_EXTN(
		0, 0, BVDC_P_NEG_SYNC_AMPLITUDE_VALUE(BVDC_P_DAC_OUTPUT_SYNC_LEVEL))
};

/* VF_FORMAT_ADDER -> VF_SYNC_TRANS1 */
static const uint32_t s_aulVfTable_480p_54MHz_YPbPr[BVDC_P_VF_TABLE_SIZE+1] =
{
#if BVDC_P_ORTHOGONAL_VEC
	BVDC_P_MAKE_VF_FORMAT_ADDER(1, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 512),
	BVDC_P_MAKE_VF_MISC(SUM_1024, ON, 0, ON, ON),
#else
	BVDC_P_MAKE_VF_FORMAT_ADDER(1, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 364),
	BVDC_P_MAKE_VF_MISC(SUM_256, ON, 0, ON, ON),
#endif
	BVDC_P_MAKE_VF_SYNC(
		BVDC_P_NEG_SYNC_AMPLITUDE_VALUE(BVDC_P_DAC_OUTPUT_SYNC_LEVEL),
		BVDC_P_NEG_SYNC_AMPLITUDE_VALUE(BVDC_P_DAC_OUTPUT_SYNC_LEVEL),
		BVDC_P_NEG_SYNC_TIP_VALUE,
		BVDC_P_POS_SYNC_AMPLITUDE_VALUE(BVDC_P_DAC_OUTPUT_PEAK_WHITE_LEVEL),
		0x0),
	BVDC_P_MAKE_VF_SYNC_TRANS(0, 64, 23, 0, 0),
	BVDC_P_MAKE_VF_SYNC_TRANS(1, 64, 23, 0, 0),
	BVDC_P_MAKE_VF_SYNC_EXTN(
		0, 0, BVDC_P_NEG_SYNC_AMPLITUDE_VALUE(BVDC_P_DAC_OUTPUT_SYNC_LEVEL))
};


/* VF_FORMAT_ADDER -> VF_SYNC_TRANS1 */
static const uint32_t s_aulVfTable_480p_54MHz_RGB[BVDC_P_VF_TABLE_SIZE+1] =
{
#if defined(HUMAX_PLATFORM_BASE)
	BVDC_P_MAKE_VF_FORMAT_ADDER(0, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0xFB),
#else
	BVDC_P_MAKE_VF_FORMAT_ADDER(0, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0x10),
#endif
#if BVDC_P_ORTHOGONAL_VEC
	BVDC_P_MAKE_VF_MISC(SUM_1024, ON, 0, ON, ON),
#else
	BVDC_P_MAKE_VF_MISC(SUM_256, ON, 0, ON, ON),
#endif
	BVDC_P_MAKE_VF_SYNC(
		BVDC_P_NEG_SYNC_AMPLITUDE_VALUE(BVDC_P_DAC_OUTPUT_SYNC_LEVEL),
		BVDC_P_NEG_SYNC_AMPLITUDE_VALUE(BVDC_P_DAC_OUTPUT_SYNC_LEVEL),
		BVDC_P_NEG_SYNC_TIP_VALUE,
		BVDC_P_POS_SYNC_AMPLITUDE_VALUE(BVDC_P_DAC_OUTPUT_PEAK_WHITE_LEVEL),
		0x0),
	BVDC_P_MAKE_VF_SYNC_TRANS(0, 70, 28, 7, 0),
	BVDC_P_MAKE_VF_SYNC_TRANS(1, 100, 72, 46, 20),
	BVDC_P_MAKE_VF_SYNC_EXTN(
		0, 0, BVDC_P_NEG_SYNC_AMPLITUDE_VALUE(BVDC_P_DAC_OUTPUT_SYNC_LEVEL))
};


/* VF_FORMAT_ADDER -> VF_SYNC_TRANS1 */
static const uint32_t s_aulVfTable_VESA_RGB[BVDC_P_VF_TABLE_SIZE+1] =
{
	BVDC_P_MAKE_VF_FORMAT_ADDER(0, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0x10 /*old value 286*/),
	BVDC_P_MAKE_VF_MISC(SUM_256, OFF, 0, ON, ON),
	BVDC_P_MAKE_VF_SYNC(0xF0, 0xF0, 0x10, 0x230, 0x2B1),
	BVDC_P_MAKE_VF_SYNC_TRANS(0, 0x40, 0x1A, 0x08, 0x00),
	BVDC_P_MAKE_VF_SYNC_TRANS(1, 0x64, 0x48, 0x2E, 0x14),
	BVDC_P_MAKE_VF_SYNC_EXTN(0, 0, 0xF0)
};


/* BFMT_VideoFmt_e -> number of active pixels cut from left of each line */
/* the following numbers must match the #define of BVDC_P_VF_SAV_RPLCT. That is 5 for now.
 */
static const BVDC_P_Display_ShaperSettings s_astShaperSettings[] =
{
	{3, 6, 3}, /*BFMT_VideoFmt_eNTSC = 0                  : 480i, NTSC-M for North America */
	{3, 6, 3}, /*BFMT_VideoFmt_eNTSC_J                    : 480i (Japan) */
	{3, 6, 3}, /*BFMT_VideoFmt_eNTSC_443                  : 480i, NTSC443*/
	{9, 2, 0}, /*BFMT_VideoFmt_ePAL_B                     : Australia */
	{9, 2, 0}, /*BFMT_VideoFmt_ePAL_B1                    : Hungary */
	{9, 2, 0}, /*BFMT_VideoFmt_ePAL_D                     : China */
	{9, 2, 0}, /*BFMT_VideoFmt_ePAL_D1                    : Poland */
	{9, 2, 0}, /*BFMT_VideoFmt_ePAL_G                     : Europe */
	{9, 2, 0}, /*BFMT_VideoFmt_ePAL_H                     : Europe */
	{9, 2, 0}, /*BFMT_VideoFmt_ePAL_K                     : Europe */
	{9, 2, 0}, /*BFMT_VideoFmt_ePAL_I                     : U.K. */
	{3, 2, 0}, /*BFMT_VideoFmt_ePAL_M                     : 525-lines (Brazil) */
	{9, 2, 0}, /*BFMT_VideoFmt_ePAL_N                     : Jamaica, Uruguay */
	{9, 2, 0}, /*BFMT_VideoFmt_ePAL_NC                    : N combination (Argentina) */
	{3, 6, 3}, /*BFMT_VideoFmt_ePAL_60                    : 480i, PAL_60*/
	{9, 6, 3}, /*BFMT_VideoFmt_eSECAM_L                   : France */
	{9, 6, 3}, /*BFMT_VideoFmt_eSECAM_B                   : Middle East */
	{9, 6, 3}, /*BFMT_VideoFmt_eSECAM_G                   : Middle East */
	{9, 6, 3}, /*BFMT_VideoFmt_eSECAM_D                   : Eastern Europe */
	{9, 6, 3}, /*BFMT_VideoFmt_eSECAM_K                   : Eastern Europe */
	{9, 6, 3}, /*BFMT_VideoFmt_eSECAM_H                   : Line SECAM */
	{0, 6, 3}, /*BFMT_VideoFmt_e1080i                     : HD 1080i */
	{3, 6, 3}, /*BFMT_VideoFmt_e1080p                     : HD 1080p 60Hz, 1125 line, SMPTE 274M */
	{0, 6, 3}, /*BFMT_VideoFmt_e720p                      : HD 720p */
	{0, 6, 3}, /*BFMT_VideoFmt_e720p_60Hz_3DOU_AS         : HD 720p 3D */
	{0, 6, 3}, /*BFMT_VideoFmt_e720p_50Hz_3DOU_AS         : HD 720p 3D */
	{0, 6, 3}, /*BFMT_VideoFmt_e720p_30Hz_3DOU_AS         : HD 720p 3D */
	{0, 6, 3}, /*BFMT_VideoFmt_e720p_24Hz_3DOU_AS         : HD 720p 3D */
	{0, 6, 3}, /*BFMT_VideoFmt_e480p                      : HD 480p */
	{0, 2, 0}, /*BFMT_VideoFmt_e1080i_50Hz                : HD 1080i 50Hz, 1125 line, SMPTE 274M */
	{3, 6, 3}, /*BFMT_VideoFmt_e3D_1080p_24Hz             : HD 1080p 24Hz, 2205 line, SMPTE 274M */
	{3, 6, 3}, /*BFMT_VideoFmt_e3D_1080p_30Hz             : HD 1080p 24Hz, 2205 line, SMPTE 274M */
	/* TODO: Verify the settings for new formats */
	{3, 6, 3}, /*BFMT_VideoFmt_e1080p_60Hz_3DOU_AS        : HD 1080p 60Hz, 2200 line */
	{3, 6, 3}, /*BFMT_VideoFmt_e1080p_60Hz_3DLR           : HD 1080p 60Hz, 4400 line */
	{3, 6, 3}, /*BFMT_VideoFmt_e1080p_24Hz                : HD 1080p 24Hz, 1125 line, SMPTE 274M */
	{3, 6, 3}, /*BFMT_VideoFmt_e1080p_25Hz                : HD 1080p 25Hz, 1125 line, SMPTE 274M */
	{0, 6, 3}, /*BFMT_VideoFmt_e1080p_30Hz                : HD 1080p 30Hz, 1125 line, SMPTE 274M */
	{3, 6, 3}, /*BFMT_VideoFmt_e1080p_50Hz                : HD 1080p 50Hz */
	{0, 6, 3}, /*BFMT_VideoFmt_e1250i_50Hz                : HD 1250i 50Hz, another 1080i_50hz standard SMPTE 295M */
	{0, 6, 3}, /*BFMT_VideoFmt_e720p_24Hz                 : HD 720p 23.976/24Hz */
	{0, 6, 3}, /*BFMT_VideoFmt_e720p_25Hz                 : HD 720p 25Hz */
	{0, 6, 3}, /*BFMT_VideoFmt_e720p_30Hz                 : HD 720p 30Hz */
	{0, 2, 0}, /*BFMT_VideoFmt_e720p_50Hz                 : HD 720p 50Hz (Australia) */
	{9, 2, 0}, /*BFMT_VideoFmt_e576p_50Hz                 : HD 576p 50Hz (Australia) */
	{0, 6, 3}, /*BFMT_VideoFmt_e240p_60Hz                 : 240p 60Hz */
	{9, 6, 3}, /*BFMT_VideoFmt_e288p_50Hz                 : 288p 50Hz */
	{0, 6, 3}, /*BFMT_VideoFmt_e1440x480p_60Hz            : CEA861B */
	{0, 6, 3}, /*BFMT_VideoFmt_e1440x576p_50Hz            : CEA861B */
	/* TODO: Verify the settings for new formats */
	{0, 6, 3}, /*BFMT_VideoFmt_e3840x2160p_24Hz            : 3840x2160 24Hz  */
	{0, 6, 3}, /*BFMT_VideoFmt_e4096x2160p_24Hz            : 4096x2160 24Hz  */
#if BFMT_LEGACY_3DTV_SUPPORT
	{0, 6, 3}, /*BFMT_VideoFmt_eCUSTOM1920x2160i_48Hz     : LVDS output for legacy 3DTV support */
	{0, 6, 3}, /*BFMT_VideoFmt_eCUSTOM1920x2160i_60Hz     : LVDS output for legacy 3DTV support */
#endif
	{0, 6, 3}, /*BFMT_VideoFmt_eCUSTOM_1440x240p_60Hz     : Custom 240p 60Hz */
	{0, 6, 3}, /*BFMT_VideoFmt_eCUSTOM_1440x288p_50Hz     : Custom 288p 50Hz */
	{0, 6, 3}, /*BFMT_VideoFmt_eCUSTOM_1366x768p          : Custom 1366x768 mode */
	{0, 6, 3}, /*BFMT_VideoFmt_eCUSTOM_1366x768p_50       : Custom 1366x768 mode */
	{0, 9, 0}, /*BFMT_VideoFmt_eDVI_640x480p              : DVI Safe mode for computer monitors */
	{0, 9, 0}, /*BFMT_VideoFmt_eDVI_640x480p_CVT          : DVI VESA mode for computer monitors */
	{0, 9, 0}, /*BFMT_VideoFmt_eDVI_800x600p              : DVI VESA mode for computer monitors */
	{0, 9, 0}, /*BFMT_VideoFmt_eDVI_1024x768p             : DVI VESA mode for computer monitors */
	{0, 6, 3}, /*BFMT_VideoFmt_eDVI_1280x768p             : DVI VESA mode for computer monitors */
	{0, 6, 3}, /*BFMT_VideoFmt_eDVI_1280x720p_50Hz        : DVI VESA mode for computer monitors */
	{0, 9, 0}, /*BFMT_VideoFmt_eDVI_1280x720p             : DVI VESA mode for computer monitors */
	{0, 6, 3}, /*BFMT_VideoFmt_eDVI_1280x720p_Red         : DVI VESA mode for computer monitors */

	{0, 6, 3}, /*BFMT_VideoFmt_eDVI_640x350p_60Hz         : DVI VESA mode for computer input */
	{0, 6, 3}, /*BFMT_VideoFmt_eDVI_640x350p_70Hz         : DVI VESA mode for computer input */
	{0, 6, 3}, /*BFMT_VideoFmt_eDVI_640x350p_72Hz         : DVI VESA mode for computer input */
	{0, 6, 3}, /*BFMT_VideoFmt_eDVI_640x350p_75Hz         : DVI VESA mode for computer input */
	{0, 6, 3}, /*BFMT_VideoFmt_eDVI_640x350p_85Hz         : DVI VESA mode for computer input */

	{0, 6, 3}, /*BFMT_VideoFmt_eDVI_640x400p_60Hz         : DVI VESA mode for computer input */
	{0, 6, 3}, /*BFMT_VideoFmt_eDVI_640x400p_70Hz         : DVI VESA mode for computer input */
	{0, 6, 3}, /*BFMT_VideoFmt_eDVI_640x400p_72Hz         : DVI VESA mode for computer input */
	{0, 6, 3}, /*BFMT_VideoFmt_eDVI_640x400p_75Hz         : DVI VESA mode for computer input */
	{0, 6, 3}, /*BFMT_VideoFmt_eDVI_640x400p_85Hz         : DVI VESA mode for computer input */

	{0, 0, 0}, /*BFMT_VideoFmt_eDVI_640x480p_66Hz         : DVI VESA mode for computer input */
	{0, 6, 3}, /*BFMT_VideoFmt_eDVI_640x480p_70Hz         : DVI VESA mode for computer input */
	{0, 6, 3}, /*BFMT_VideoFmt_eDVI_640x480p_72Hz         : DVI VESA mode for computer input */
	{0, 6, 3}, /*BFMT_VideoFmt_eDVI_640x480p_75Hz         : DVI VESA mode for computer input */
	{0, 6, 3}, /*BFMT_VideoFmt_eDVI_640x480p_85Hz         : DVI VESA mode for computer input */

	{0, 6, 3}, /*BFMT_VideoFmt_eDVI_720x400p_60Hz         : DVI VESA mode for computer input */
	{0, 6, 3}, /*BFMT_VideoFmt_eDVI_720x400p_70Hz         : DVI VESA mode for computer input */
	{0, 6, 3}, /*BFMT_VideoFmt_eDVI_720x400p_72Hz         : DVI VESA mode for computer input */
	{0, 6, 3}, /*BFMT_VideoFmt_eDVI_720x400p_75Hz         : DVI VESA mode for computer input */
	{0, 6, 3}, /*BFMT_VideoFmt_eDVI_720x400p_85Hz         : DVI VESA mode for computer input */

	{0, 6, 3}, /*BFMT_VideoFmt_eDVI_800x600p_56Hz         : DVI VESA mode for computer input */
	{0, 0, 0}, /*BFMT_VideoFmt_eDVI_800x600p_59Hz_Red     : DVI VESA mode for computer input */
	{0, 6, 3}, /*BFMT_VideoFmt_eDVI_800x600p_70Hz         : DVI VESA mode for computer input */
	{0, 6, 3}, /*BFMT_VideoFmt_eDVI_800x600p_72Hz         : DVI VESA mode for computer input */
	{0, 6, 3}, /*BFMT_VideoFmt_eDVI_800x600p_75Hz         : DVI VESA mode for computer input */
	{0, 6, 3}, /*BFMT_VideoFmt_eDVI_800x600p_85Hz         : DVI VESA mode for computer input */

	{0, 0, 0}, /*BFMT_VideoFmt_eDVI_1024x768p_66Hz        : DVI VESA mode for computer input */
	{0, 6, 3}, /*BFMT_VideoFmt_eDVI_1024x768p_70Hz        : DVI VESA mode for computer input */
	{0, 6, 3}, /*BFMT_VideoFmt_eDVI_1024x768p_72Hz        : DVI VESA mode for computer input */
	{0, 6, 3}, /*BFMT_VideoFmt_eDVI_1024x768p_75Hz        : DVI VESA mode for computer input */
	{0, 6, 3}, /*BFMT_VideoFmt_eDVI_1024x768p_85Hz        : DVI VESA mode for computer input */

	{0, 6, 3}, /*BFMT_VideoFmt_eDVI_1280x720p_70Hz        : DVI VESA mode for computer input */
	{0, 6, 3}, /*BFMT_VideoFmt_eDVI_1280x720p_72Hz        : DVI VESA mode for computer input */
	{0, 6, 3}, /*BFMT_VideoFmt_eDVI_1280x720p_72Hz        : DVI VESA mode for computer input */
	{0, 6, 3}, /*BFMT_VideoFmt_eDVI_1280x720p_75Hz        : DVI VESA mode for computer input */
	{0, 6, 3}, /*BFMT_VideoFmt_eDVI_1280x720p_85Hz        : DVI VESA mode for computer input */

	/* New DVI or PC vdec input support */
	{0, 0, 0}, /* BFMT_VideoFmt_eDVI_1024x768i_43Hz,      : DVI VESA mode for computer monitors */
	{0, 0, 0}, /* BFMT_VideoFmt_eDVI_1152x864p_75Hz,      : DVI VESA mode for computer monitors */
	{0, 0, 0}, /* BFMT_VideoFmt_eDVI_1280x768p_75Hz,      : DVI VESA mode for computer monitors */
	{0, 0, 0}, /* BFMT_VideoFmt_eDVI_1280x768p_85Hz,      : DVI VESA mode for computer monitors */
	{0, 9, 0}, /* BFMT_VideoFmt_eDVI_1280x800p_60Hz,   : DVI VESA mode for computer monitors */
	{0, 0, 0}, /* BFMT_VideoFmt_eDVI_1280x960p_60Hz,      : DVI VESA mode for computer monitors */
	{0, 0, 0}, /* BFMT_VideoFmt_eDVI_1280x960p_85Hz,      : DVI VESA mode for computer monitors */
	{0, 9, 0}, /* BFMT_VideoFmt_eDVI_1280x1024p_60Hz,     : DVI VESA mode for computer monitors */
	{0, 0, 0}, /* BFMT_VideoFmt_eDVI_1280x1024p_69Hz,     : DVI VESA mode for computer monitors */
	{0, 0, 0}, /* BFMT_VideoFmt_eDVI_1280x1024p_75Hz,     : DVI VESA mode for computer monitors */
	{0, 0, 0}, /* BFMT_VideoFmt_eDVI_1280x1024p_85Hz,     : DVI VESA mode for computer monitors */
	{0, 0, 0}, /* BFMT_VideoFmt_eDVI_832x624p_75Hz,       : DVI VESA mode for computer monitors */
	{0, 9, 0}, /* BFMT_VideoFmt_eDVI_1360x768p_60Hz,      : DVI VESA mode for computer monitors */
	{0, 9, 9}, /* BFMT_VideoFmt_eDVI_1366x768p_60Hz,      : DVI VESA mode for computer monitors */
	{0, 0, 0}, /* BFMT_VideoFmt_eDVI_1400x1050p_60Hz,     : DVI VESA mode for computer monitors */
	{0, 0, 0}, /* BFMT_VideoFmt_eDVI_1400x1050p_60Hz_Red, : DVI VESA mode for computer monitors */
	{0, 0, 0}, /* BFMT_VideoFmt_eDVI_1400x1050p_75Hz,     : DVI VESA mode for computer monitors */
	{0, 0, 0}, /* BFMT_VideoFmt_eDVI_1600x1200p_60Hz,     : DVI VESA mode for computer monitors */
	{0, 0, 0}, /* BFMT_VideoFmt_eDVI_1920x1080p_60Hz_Red, : DVI VESA mode for computer monitors */
	{0, 0, 0}, /* BFMT_VideoFmt_eDVI_848x480p_60Hz,       : DVI VESA mode for computer monitors */
	{0, 0, 0}, /* BFMT_VideoFmt_eDVI_1064x600p_60Hz,      : DVI VESA mode for computer monitors */
	{0, 9, 0}, /* BFMT_VideoFmt_eDVI_1440x900p_60Hz,   : DVI VESA mode for computer monitors */

	/* SW7435-276: New format enums for 482/483 */
	{3, 6, 3}, /*BFMT_VideoFmt_e720x482_NTSC = 0          : 480i, NTSC-M for North America */
	{3, 6, 3}, /*BFMT_VideoFmt_e720x482_NTSC_J            : 480i (Japan) */
	{0, 6, 3}, /*BFMT_VideoFmt_e720x483p                  : HD 483p */

	{0, 0, 0}, /* BFMT_VideoFmt_eCustome0,                : custom format for DVO master 59.94/60Hz */
	{0, 0, 0}, /* BFMT_VideoFmt_eCustome1,                : custom format for DVO master 50Hz */
	{0, 0, 0}, /* BFMT_VideoFmt_eCustome2,                : custom format for DVO */
	/* Must be last */
	{0, 4, 5}  /*BFMT_VideoFmt_eMaxCount                  :  Counter. Do not use! */
};

#if BVDC_P_ORTHOGONAL_VEC
/* All-pass */
static const uint32_t s_aulChFilterTbl_AllPass[BVDC_P_CHROMA_TABLE_SIZE] =
{
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00A00000,
};

/* Cr 0.6 MHz/13.5 MHz */
static const uint32_t s_aulChFilterTbl_Cr_06Mhz[BVDC_P_CHROMA_TABLE_SIZE] =
{
	0x000000FD, 0x00000A1A, 0x00000AEE, 0x00000FC4, 0x0000A58D,
	0x0000A09B, 0x0000AA1c, 0x0000AE67, 0x0000AF09, 0x0000AFA0,
};

/* Cr 1.3 Mhz/13.5 Mhz */
static const uint32_t s_aulChFilterTbl_Cr_13Mhz[BVDC_P_CHROMA_TABLE_SIZE] =
{
	0x0000000E, 0x000000A5, 0x00000040, 0x00000564, 0x00005F28,
	0x000017F9, 0x0000823D, 0x00087562, 0x000A139B, 0x000A0AFF,
};

/* Cr 3.375 Mhz/13.5 Mhz */
static const uint32_t s_aulChFilterTbl_Cr_3375Mhz[BVDC_P_CHROMA_TABLE_SIZE] =
{
	0X000000AC, 0x0000006F, 0x00000569, 0x000001E5, 0x00008741,
	0x00000759, 0x0004DD7D, 0x00009066, 0x000ADB23, 0x000F05FF,
};

/* Y 4.2 Mhz/13.5Mhz */
static const uint32_t s_aulChFilterTbl_Y_42Mhz[BVDC_P_CHROMA_TABLE_SIZE] =
{
	0X00000000, 0X00000000, 0X00000000, 0X00000003, 0X000005BE,
	0X00004C2C, 0x00000747, 0X0005FF95, 0X0008EF0E, 0X000FFF05,
};

/* Y 5.0 Mhz/13.5Mhz */
static const uint32_t s_aulChFilterTbl_Y_50Mhz[BVDC_P_CHROMA_TABLE_SIZE] =
{
	0x00000001, 0x0000000F, 0x000005BF, 0x000008A1, 0x00004ED0,
	0x000003F4, 0x0000A438, 0x00049D08, 0x0008C8FF, 0x00A5F5AF,
};

/* Y 5.5 Mhz/13.5Mhz */
static const uint32_t s_aulChFilterTbl_Y_55Mhz[BVDC_P_CHROMA_TABLE_SIZE] =
{
	0x00000048, 0x00000089, 0x000004EE, 0x00000187, 0x000003D9,
	0x00005E76, 0x0000A99C, 0x00049BBD, 0x0008D023, 0x00A5A55F,
};

/* Y 6.0 Mhz/13.5Mhz */
static const uint32_t s_aulChFilterTbl_Y_60Mhz[BVDC_P_CHROMA_TABLE_SIZE] =
{
	0x000000EF, 0x00000415, 0x00000C65, 0x00004C44, 0x000085B7,
	0x000046CD, 0x00009F0C, 0x0004ACCA, 0x0009AB77, 0x00A5F5AF,
};

/* Y 6.75 Mhz/13.5Mhz */
static const uint32_t s_aulChFilterTbl_Y_675Mhz[BVDC_P_CHROMA_TABLE_SIZE] =
{
	0x0000008F, 0x00000499, 0x000008B5, 0x00004D3F, 0x0000846D,
	0x00004068, 0x00008D1C, 0x0004ADAA, 0x0009B9A7, 0x00A5F505,
};

/* Cr 6.75 Mhz/27Mhz */
static const uint32_t s_aulChFilterTbl_Cr_675Mhz[BVDC_P_CHROMA_TABLE_SIZE] =
{
	0x00000031, 0X00000000, 0x00001333, 0X00000000, 0x00002103,
	0X00000000, 0x00013313, 0X00000000, 0x00022001, 0x00030000,
};

/* Cr 16.56 Mhz/74.25 Mhz */
static const uint32_t s_aulChFilterTbl_Cr_1856Mhz[BVDC_P_CHROMA_TABLE_SIZE] =
{
	0x00000031, 0X00000000, 0x00001333, 0X00000000, 0x00002103,
	0X00000000, 0x00013313, 0X00000000, 0x00022001, 0x00030000,
};

/* Cr 12 Mhz/54 Mhz */
static const uint32_t s_aulChFilterTbl_Y_12Mhz[BVDC_P_CHROMA_TABLE_SIZE] =
{
	0x0000000C, 0X0000004A, 0x0000009B, 0X000004E9, 0x000000DD,
	0X0000043C, 0x000009A8, 0X00004E6E, 0x00009A27, 0x000A5F0A,
};

/* Cr SD */
static const uint32_t s_aulChFilterTbl_Cr_SD[BVDC_P_CHROMA_TABLE_SIZE] =
{
	0x00000085, 0x000001F0, 0x00000409, 0x0000173E, 0x00008656,
	0x00002940, 0x0004D8B3, 0x00013B0A, 0x000ADEA5, 0x000F0FFA,
};

/* Cr SECAM */
static const uint32_t s_aulChFilterTbl_Cr_SECAM[BVDC_P_CHROMA_TABLE_SIZE] =
{
	0x000000EF, 0X00000415, 0x00000C65, 0X00004C44, 0x000085B7,
	0X000046CD, 0x00009F0C, 0X0004ACCA, 0x0009AB77, 0x00A5F5AF,
};


#else
/* All-pass */
static const uint32_t s_aulChFilterTbl_AllPass[BVDC_P_CHROMA_TABLE_SIZE] =
{
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00030000,
};

/* Cr 0.6 MHz/13.5 MHz */
static const uint32_t s_aulChFilterTbl_Cr_06Mhz[BVDC_P_CHROMA_TABLE_SIZE] =
{
	0x000E6A1F, 0x00A590FC, 0x00AA9A09, 0x0AF00AE7, 0x00000AFF,
};

/* Cr 1.3 Mhz/13.5 Mhz */
static const uint32_t s_aulChFilterTbl_Cr_13Mhz[BVDC_P_CHROMA_TABLE_SIZE] =
{
	0x000040A8, 0x005F2056, 0x0082317F, 0xA1388756, 0x0000A0AF,
};

/* Cr 3.375 Mhz/13.5 Mhz */ /* Changed by rpan */
static const uint32_t s_aulChFilterTbl_Cr_3375Mhz[BVDC_P_CHROMA_TABLE_SIZE] =
{
/*	0x0005003C, 0x000FC01F, 0x04DD34EF, 0xAD3F0903, 0x0000F05F,*/
	0x000060A0, 0x000C1055, 0x04DB82D2, 0xE5481347, 0x0000FAAA,
};

/* Y 5.0 Mhz/13.5Mhz */
static const uint32_t s_aulChFilterTbl_Y_50Mhz[BVDC_P_CHROMA_TABLE_SIZE] =
{
	0x0005F0A0, 0x004ED08A, 0x00A4312F, 0x8C8F49D0, 0x000A505A,
};

/* Y 6.0 Mhz/13.5Mhz */
static const uint32_t s_aulChFilterTbl_Y_60Mhz[BVDC_P_CHROMA_TABLE_SIZE] =
{
	0x000A05F3, 0x008604D0, 0x008CC45F, 0x9BC348BC, 0x000A5AF0,
};

/* Cr 6.75 Mhz/27Mhz */
static const uint32_t s_aulChFilterTbl_Cr_675Mhz[BVDC_P_CHROMA_TABLE_SIZE] =
{
	0x00132003, 0X00203000, 0x01220000, 0X33010000, 0x00020000,
};

/* Y 6.75 Mhz/13.5Mhz */ /* Changed by rpan */
static const uint32_t s_aulChFilterTbl_Y_675Mhz[BVDC_P_CHROMA_TABLE_SIZE] =
{
/*	0x00086058, 0x000CA045, 0x0089C40E, 0x9B934E54, 0x000A5F55,*/
	0x000854C8, 0x000D0047, 0x009A24A7, 0x9A354E24, 0x000A5FA5,
};

#endif

/* SRC Control Filter Modes */
static const uint32_t s_ulSrcControl =
	BVDC_P_SRC_FIELD_ENUM(SRC_CONTROL, HSYNC,    Minus_6dB) |
	BVDC_P_SRC_FIELD_ENUM(SRC_CONTROL, H_CLAMP, UNSIGNED) |
	BVDC_P_SRC_FIELD_ENUM(SRC_CONTROL, CH12_CLAMP,  SIGNED) |
	BVDC_P_SRC_FIELD_ENUM(SRC_CONTROL, CH0_CLAMP, UNSIGNED) |
	BVDC_P_SRC_FIELD_ENUM(SRC_CONTROL, CH1_2,    Minus_6dB) |
	BVDC_P_SRC_FIELD_ENUM(SRC_CONTROL, CH0,   Minus_11dB);



static const uint32_t s_ulItConfig_NTSC             = BVDC_P_MAKE_IT_TG(1, 0x7f,  0); /* IT_TG_CONFIG for NTSC, NTSC_J */
static const uint32_t s_ulItConfig_PALM             = BVDC_P_MAKE_IT_TG(3, 0x7f,  0); /* IT_TG_CONFIG for PAL_M */
static const uint32_t s_ulItConfig_PAL              = BVDC_P_MAKE_IT_TG(3, 0x7f,  0); /* IT_TG_CONFIG for PAL, PAL_N, PAL_NC */
static const uint32_t s_ulItConfig_SECAM            = BVDC_P_MAKE_IT_TG(0, 0x7f,  1); /* IT_TG_CONFIG for SECAM */
static const uint32_t s_ulItConfig_1080p            = BVDC_P_MAKE_IT_TG(1, 0x7f,  0); /*IT_TG_CONFIG for 1080p */
static const uint32_t s_ulItConfig_1080i            = BVDC_P_MAKE_IT_TG(1, 0x7,  0); /* IT_TG_CONFIG for 1080i, 1080i_50Hz */
static const uint32_t s_ulItConfig_720p             = BVDC_P_MAKE_IT_TG(1, 0x7,  0); /* IT_TG_CONFIG for 720p, 720p_50Hz */
static const uint32_t s_ulItConfig_3D               = BVDC_P_MAKE_IT_TG(4, 0x3,  0); /* IT_TG_CONFIG for 720p, 720p_50Hz */
static const uint32_t s_ulItConfig_480p             = BVDC_P_MAKE_IT_TG(1, 0x3,  0); /* IT_TG_CONFIG for 480p, 576p_50Hz, 640x480p */
static const uint32_t s_ulItConfig_CUSTOM_1366x768p = BVDC_P_MAKE_IT_TG(1, 0x3,  0); /* IT_TG_CONFIG for 1600x1200, 1366x768, 576p_59.94Hz, LCD */

/* HDMI Rate Manager */
static const BVDC_P_RateInfo s_HdmiRm[] =
{
#if (BVDC_P_SUPPORT_HDMI_RM_VER <= BVDC_P_HDMI_RM_VER_1)
	/* ulPixelClkRate;
	   ulMDiv;
	   ulNDiv; (Offset: 9.22 format)
	   ulRDiv;
	   ulSampleInc;
	   ulNumerator;
	   ulDenominator;
	   ulVcoRange;
	   ulLinkDivCtrl;
	   ulP2; ( pll feedback pre-divider)
	   pchRate;
	*/
	{BFMT_PXL_25_2MHz,             5, 148,        210, 3,    87,   400,      1,      1, 1, "25.20"},     /* 25.200000 MHz */
	{BFMT_PXL_25_2MHz_DIV_1_001,   5, 148,        210, 3,     3,    14,      1,      1, 1, "25.17"},     /* 25.174825 MHz */

	{BFMT_PXL_27MHz,               4, 0xE000000,  112, 4,   0,     1,        0,      0, 1, "27.00" },     /* 480p @ 60Hz  */
	{BFMT_PXL_27MHz_MUL_1_001,     4, 0x0E039581, 112, 3,   997, 1001,       0,      0, 1, "27.02" },     /* 480p @ 60Hz  */

	{BFMT_PXL_74_25MHz,            2, 0x13400000, 112, 2,    10,    11,      1,      1, 1, "74.25"},     /* 74.250000 MHz */
	{BFMT_PXL_74_25MHz_DIV_1_001,  2, 0x133B13B0, 112, 2,   114,   125,      1,      1, 1, "74.17"},     /* 74.175824 MHz */

	{BFMT_PXL_148_5MHz,            1, 0x13400000, 112, 2,    10,    11,      1,      0, 1, "148.5"},     /* 148.50000 MHz */
	{BFMT_PXL_148_5MHz_DIV_1_001,  1, 0x133B13B0, 112, 2,   114,   125,      1,      0, 1, "148.3"},     /* 148.35000 MHz */

	{BFMT_PXL_162MHz,              1, 190,        210, 2,     1,     2,      1,      1, 1, "162.0"},       /* 162.00000 MHz */

	{BFMT_PXL_56_304MHz,           2,  98,        210, 3,   933,  1564,      0,      1, 1, "56.30"},       /* 56.304000 MHz 1440x782 @ 50Hz */
	{BFMT_PXL_67_565MHz_DIV_1_001, 2, 157,        210, 3,     3, 25024,      1,      1, 1, "67.49"},       /* 67.497303 MHz 1440x782 @ 59.94Hz*/
	{BFMT_PXL_67_565MHz,           2, 154,        210, 2,  3119,  3128,      1,      1, 1, "67.56"},       /* 67.564800 MHz 1440x782 @ 60Hz */

	/* 60/50 Hz VESA formats */
	{BFMT_PXL_39_79MHz,            3, 109,        210, 3,  5427, 13816,      1,      1, 1, "39.79"},       /* 39.790080 MHz */
	{BFMT_PXL_65MHz,               2, 144,        210, 3,  5217, 45136,      1,      1, 1, "65.00"},       /* 64.995840 MHz */
	{BFMT_PXL_65_286MHz,           2, 145,        210, 3,    41,   403,      1,      1, 1, "65.28"},       /* 65.286000 MHz */
	{BFMT_PXL_60_4656MHz,          2, 107,        210, 3,  2931,  8398,      1,      1, 1, "60.46"},       /* 60.465600 MHz */
	{BFMT_PXL_64_022MHz,           2, 146,        210, 3,   161,   988,      1,      1, 1, "64.02"},       /* 64.022400 MHz */

	/* 59.94 Hz VESA formats */
	{BFMT_PXL_39_79MHz_DIV_1_001,  3, 109,        210, 3,  3981, 10048,      1,      1, 1, "39.79/1.001"}, /* 39.750329 MHz */
	{BFMT_PXL_65MHz_DIV_1_001,     2, 144,        210, 3,   471,  3968,      1,      1, 1, "65.00/1.001"}, /* 64.930909 MHz */
	{BFMT_PXL_65_286MHz_DIV_1_001, 2, 145,        210, 3,    13,   124,      1,      1, 1, "65.28/1.001"}, /* 65.220779 MHz */
	{BFMT_PXL_64_022MHz_DIV_1_001, 2, 146,        210, 3,   101,   608,      1,      1, 1, "64.02/1.001"}, /* 63.958441 MHz */

#ifdef LATER
	/* Used by certain LCD panels */
	{BFMT_PXL_72_427MHz,           2, 0x12c70524, 112, 2, 14822, 15089,      1,      0, 2,  "72427"     }, /* 72427200 */
#endif /* LATER */

#elif (BVDC_P_SUPPORT_HDMI_RM_VER == BVDC_P_HDMI_RM_VER_2)
	/* Mask                         Denom   Num Sample Offset shift rmdiv vco pxDiv Feed prediv */
	{BFMT_PXL_25_2MHz,                  1,     0,  3, 0x5d55555,  1,  70,  0,   5,    1,   1, "25.20" }, /* 25.200000 MHz */
	{BFMT_PXL_25_2MHz_DIV_1_001,    10000,  9601,  2, 0x5d3d76c,  1,  69,  0,   5,    1,   1, "25.17" }, /* 25.174825 MHz */

	{BFMT_PXL_27MHz,                    1,     0,  3, 0x6400000,  1,  75,  0,   5,    1,   1, "27.00" }, /* 27.000000 MHz  */
	{BFMT_PXL_27MHz_MUL_1_001,       1001,   998,  2, 0x6419999,  1,  75,  0,   5,    1,   1, "27.02" }, /* 27.027000 MHz  */

	{BFMT_PXL_67_565MHz,             3128,  3119,  2, 0xc83126e,  2,  75,  0,   2,    1,   1, "67.56" }, /* 67.564800 MHz 1440x782 @ 60Hz */
	{BFMT_PXL_67_565MHz_DIV_1_001,  12512, 12013,  2, 0xc7fdf43,  2,  74,  0,   2,    1,   1, "67.49" }, /* 67.497303 MHz 1440x782 @ 59.94Hz*/

	{BFMT_PXL_74_25MHz,                55,    54,  2, 0x6e00000,  1,  82,  0,   2,    1,   1, "74.25" }, /* 74.250000 MHz */
	{BFMT_PXL_74_25MHz_DIV_1_001,     1250,  1231,  2, 0x6de3de3,  1,  82,  0,   2,    1,   1, "74.17" }, /* 74.175824 MHz */

	{BFMT_PXL_148_5MHz,                 11,     9,  3, 0xdc00000,  2,  105, 0,   1,    1,   2, "148.5" }, /* 148.50000 MHz */
	{BFMT_PXL_148_5MHz_DIV_1_001,     1250,  1231,  2, 0xdbc7bc7,  2,  82,  0,   1,    1,   2, "148.35"}, /* 148351648 MHz */

	/* 60/50 Hz VESA formats */
	{BFMT_PXL_39_79MHz,                628,   619,  2, 0x586c226,  1,  66,  0,   3,    1,   1, "39.79" }, /* 39.790080 MHz */
	{BFMT_PXL_65MHz,                   325,   322,  2, 0x604bda1,  1,  72,  0,   2,    1,   1, "65.00" }, /* 65.000000 MHz */
	{BFMT_PXL_65_286MHz,               403,   394,  2, 0x60b851e,  1,  72,  0,   2,    1,   1, "65.28" }, /* 65.286000 MHz */
	{BFMT_PXL_64_022MHz,              2964,  2947,  2, 0x5ed9168,  1,  71,  0,   2,    1,   1, "64.02" }, /* 64.022400 MHz */
	{BFMT_PXL_83_5MHz,  		       835, 	814,  2, 0x3dda12f,  0,  46,  0,   1,    1,   1, "83500" }, /* 83.500000 MHz */
	{BFMT_PXL_85_5MHz,                  95,    92,  2, 0x3f55555,  0,  47,  0,   1,    1,   1, "85500" }, /* 85.500000 MHz */

	/* 59.94 Hz VESA formats */
	{BFMT_PXL_39_79MHz_DIV_1_001,     5024,  4967,  2, 0x5855855,  1,  66,  0,   3,    1,   1, "39.70/1.001" }, /* 39.750329 MHz */
	{BFMT_PXL_65MHz_DIV_1_001,         496,   493,  2, 0x6031a60,  1,  72,  0,   2,    1,   1, "65.00/1.001" }, /* 64.930909 MHz */
	{BFMT_PXL_65_286MHz_DIV_1_001,     155,   152,  2, 0x609f959,  1,  72,  0,   2,    1,   1, "65.28/1.001" }, /* 65.220779 MHz */
	{BFMT_PXL_64_022MHz_DIV_1_001,    1824,  1819,  2, 0x5ec0d4c,  1,  71,  0,   2,    1,   1, "64.02/1.001" }, /* 63.958441 MHz */

	/* For (480P or 576P), 54 MHz pixel rate. */
	{BFMT_PXL_54MHz,                     1,     0,  3, 0xa000000,  2,  60,  0,   2,    1,   2, "54.00"       }, /* 54.00 MHz  */
	{BFMT_PXL_54MHz_MUL_1_001,        1001,   998,  2, 0xa028f5c,  2,  60,  0,   2,    1,   2, "54.00/1.001" }, /* 54.054 MHz */
	/* Mask                          Denom    Num Sample Offset shift rmdiv vco pxDiv Feed prediv */

#elif (BVDC_P_SUPPORT_HDMI_RM_VER == BVDC_P_HDMI_RM_VER_3)
	/* Mask                      Denom   Num Sample  Offset shift rmdiv vco pxDiv Feed prediv */
	{BFMT_PXL_25_2MHz,               1,    0,  3, 0xbaaaaaa,  2,  70,    0,   5,   1,    1, "25.20" }, /* 25.200000 MHz */
	{BFMT_PXL_25_2MHz_DIV_1_001, 10000, 9601,  2, 0xba7aed8,  2,  69,    0,   5,   1,    1, "25.20" }, /* 25.174825 MHz */

	{BFMT_PXL_27MHz,                 1,    0,  3, 0xc800000,  2,  75,    0,   5,    1,   1, "27.00" }, /* 27.000000 MHz */
	{BFMT_PXL_27MHz_MUL_1_001,    1001,  998,  2, 0xc833333,  2,  75,    0,   5,    1,   1, "27.02" }, /* 27.027000 MHz */

	{BFMT_PXL_74_25MHz,             55,   54,  2, 0xdc00000,  2,   82,   0,   2,    1,   1, "74.25" }, /* 74.250000 MHz */
	{BFMT_PXL_74_25MHz_DIV_1_001, 1250, 1231,  2, 0xdbc7bc7,  2,   82,   0,   2,    1,   1, "74.17" }, /* 74.175824 MHz */

	{BFMT_PXL_148_5MHz,           11,     9,  3, 0xdc00000,  2,  105,    0,   1,    1,   1, "148.5"}, /* 148.50000 MHz */

	/* 60/50 Hz VESA formats */
	{BFMT_PXL_39_79MHz,              628,   619,  2, 0xb0d844d,  2,  66,  0,   3,    1,   1, "39.79" }, /* 39.790080 MHz */
	{BFMT_PXL_65MHz,                 325,   322,  2, 0xc097b42,  2,  72,  0,   2,    1,   1, "65.00" }, /* 65.000000 MHz */
	{BFMT_PXL_65_286MHz,             403,   394,  2, 0xc170a3d,  2,  72,  0,   2,    1,   1, "65.28" }, /* 65.286000 MHz */
	{BFMT_PXL_64_022MHz,            2964,  2947,  2, 0xbdb22d0,  2,  71,  0,   2,    1,   1, "64.02" }, /* 64.022400 MHz */

	/* 59.94 Hz VESA formats */
	{BFMT_PXL_39_79MHz_DIV_1_001,   5024,  4967,  2, 0xb0ab0ab,  2,  66,  0,   3,    1,   1, "39.79/1.001" }, /* 39.750329 MHz */
	{BFMT_PXL_65MHz_DIV_1_001,       496,   493,  2, 0xc0634c0,  2,  72,  0,   2,    1,   1, "65.00/1.001" }, /* 64.930909 MHz */
	{BFMT_PXL_65_286MHz_DIV_1_001,   155,   152,  2, 0xc13f2b3,  2,  72,  0,   2,    1,   1, "65.28/1.001" }, /* 65.220779 MHz */
	{BFMT_PXL_64_022MHz_DIV_1_001,  1824,  1819,  2, 0xbd81a98,  2,  71,  0,   2,    1,   1, "64.02/1.001" }, /* 63.958441 MHz */

	/* For (480P or 576P), 54 MHz pixel rate. */
	{BFMT_PXL_54MHz,                   1,     0,  3, 0xa000000,  2,  60,  0,   2,    1,   1, "54.00"       }, /* 54.00 MHz  */
	{BFMT_PXL_54MHz_MUL_1_001,      1001,   998,  2, 0xa028f5c,  2,  60,  0,   2,    1,   1, "54.00/1.001" }, /* 54.054 MHz */

#elif (BVDC_P_SUPPORT_HDMI_RM_VER == BVDC_P_HDMI_RM_VER_4)
	/* Mask                        ColorDepth                     PixelRep                         Denom    Num Sample  Offset shift rmdiv vco pxDiv Feed prediv KVcoXS pchRate */
	{BFMT_PXL_25_2MHz,             BAVC_HDMI_BitsPerPixel_e24bit, BAVC_HDMI_PixelRepetition_eNone,     1,     0,  3, 0xbaaaaaa,  2,  70,    0,   5,    1,   2,    4,    "25200" }, /* 25200000 */
	{BFMT_PXL_25_2MHz,             BAVC_HDMI_BitsPerPixel_e30bit, BAVC_HDMI_PixelRepetition_eNone,     1,     0,  3, 0xbaaaaaa,  2,  70,    0,   4,    1,   2,    4,    "31500" }, /* 31500000 */
	{BFMT_PXL_25_2MHz,             BAVC_HDMI_BitsPerPixel_e36bit, BAVC_HDMI_PixelRepetition_eNone,     1,     0,  3, 0x2a00000,  0,  63,    0,   3,    1,   2,    4,    "37800" }, /* 37800000 */

	{BFMT_PXL_25_2MHz_DIV_1_001,   BAVC_HDMI_BitsPerPixel_e24bit, BAVC_HDMI_PixelRepetition_eNone, 10000,  9601,  2, 0xba7aed8,  2,  69,    0,   5,    1,   2,    4,    "25174" }, /* 25174825.1748252 */
	{BFMT_PXL_25_2MHz_DIV_1_001,   BAVC_HDMI_BitsPerPixel_e30bit, BAVC_HDMI_PixelRepetition_eNone, 10000,  9601,  2, 0x2e9ebb6,  0,  69,    0,   4,    1,   2,    4,    "31469" }, /* 31468531.4685315 */
	{BFMT_PXL_25_2MHz_DIV_1_001,   BAVC_HDMI_BitsPerPixel_e36bit, BAVC_HDMI_PixelRepetition_eNone,  1500,  1433,  2, 0x29f5423,  0,  62,    0,   3,    1,   2,    4,    "37762" }, /* 37762237.7622378 */

	{BFMT_PXL_27MHz,               BAVC_HDMI_BitsPerPixel_e24bit, BAVC_HDMI_PixelRepetition_eNone,     1,     0,  3, 0xc800000,  2,  75,    0,   5,    1,   2,    4,    "27000" }, /* 27000000 */
	{BFMT_PXL_27MHz,               BAVC_HDMI_BitsPerPixel_e30bit, BAVC_HDMI_PixelRepetition_eNone,     1,     0,  3, 0xc800000,  2,  75,    0,   4,    1,   2,    4,    "33750" }, /* 33750000 */
	{BFMT_PXL_27MHz,               BAVC_HDMI_BitsPerPixel_e36bit, BAVC_HDMI_PixelRepetition_eNone,    45,    44,  2, 0xb400000,  2,  67,    0,   3,    1,   2,    4,    "40500" }, /* 40500000 */

	{BFMT_PXL_27MHz,               BAVC_HDMI_BitsPerPixel_e24bit, BAVC_HDMI_PixelRepetition_e2x,       1,     0,  3, 0xa000000,  2,  60,    0,   2,    1,   2,    4,    "54000" }, /* 54000000 */
	{BFMT_PXL_27MHz,               BAVC_HDMI_BitsPerPixel_e30bit, BAVC_HDMI_PixelRepetition_e2x,       1,     0,  3, 0xc800000,  2,  75,    0,   2,    1,   2,    4,    "67500" }, /* 67500000 */
	{BFMT_PXL_27MHz,               BAVC_HDMI_BitsPerPixel_e36bit, BAVC_HDMI_PixelRepetition_e2x,       1,     0,  3, 0x7800000,  1,  45,    0,   1,    1,   2,    4,    "81000" }, /* 81000000 */

	{BFMT_PXL_27MHz,               BAVC_HDMI_BitsPerPixel_e24bit, BAVC_HDMI_PixelRepetition_e4x,       1,     0,  3, 0xa000000,  2,  60,    0,   1,    1,   2,    4,   "108000" }, /* 108000000 */
	{BFMT_PXL_27MHz,               BAVC_HDMI_BitsPerPixel_e30bit, BAVC_HDMI_PixelRepetition_e4x,       1,     0,  3, 0xc800000,  2,  75,    0,   1,    1,   2,    4,   "135000" }, /* 135000000 */
	{BFMT_PXL_27MHz,               BAVC_HDMI_BitsPerPixel_e36bit, BAVC_HDMI_PixelRepetition_e4x,       1,     0,  3, 0xf000000,  2,  90,    1,   1,    1,   2,    7,   "162000" }, /* 162000000 */

	{BFMT_PXL_27MHz_MUL_1_001,     BAVC_HDMI_BitsPerPixel_e24bit, BAVC_HDMI_PixelRepetition_eNone,  1001,   998,  2, 0xc833333,  2,  75,    0,   5,    1,   2,    4,    "27027" }, /* 27027000 */
	{BFMT_PXL_27MHz_MUL_1_001,     BAVC_HDMI_BitsPerPixel_e30bit, BAVC_HDMI_PixelRepetition_eNone, 12500, 12037,  2, 0xc7ccd9e,  2,  74,    0,   4,    1,   2,    4,    "33783" }, /* 33716283.7162837 */
	{BFMT_PXL_27MHz_MUL_1_001,     BAVC_HDMI_BitsPerPixel_e36bit, BAVC_HDMI_PixelRepetition_eNone, 22500, 22067,  2, 0xb3d1f74,  2,  67,    0,   3,    1,   2,    4,    "40459" }, /* 40459540.4595405 */
	{BFMT_PXL_27MHz_MUL_1_001,     BAVC_HDMI_BitsPerPixel_e24bit, BAVC_HDMI_PixelRepetition_e2x,    1001,   998,  2, 0xa028f5c,  2,  60,    0,   2,    1,   2,    4,    "54054" }, /* 54.054 MHz */
	{BFMT_PXL_27MHz_MUL_1_001,     BAVC_HDMI_BitsPerPixel_e30bit, BAVC_HDMI_PixelRepetition_e2x,    1001,   998,  2, 0xc833333,  2,  75,    0,   2,    1,   2,    4,    "67567" }, /* 67567500 */
	{BFMT_PXL_27MHz_MUL_1_001,     BAVC_HDMI_BitsPerPixel_e36bit, BAVC_HDMI_PixelRepetition_e2x,    1001,   998,  2, 0x781eb85,  1,  45,    0,   1,    1,   2,    4,    "81081" }, /* 81081000 */
	{BFMT_PXL_27MHz_MUL_1_001,     BAVC_HDMI_BitsPerPixel_e24bit, BAVC_HDMI_PixelRepetition_e4x,    1001,   998,  2, 0xa028f5c,  2,  60,    0,   1,    1,   2,    4,   "108108" }, /* 108108000 */
	{BFMT_PXL_27MHz_MUL_1_001,     BAVC_HDMI_BitsPerPixel_e30bit, BAVC_HDMI_PixelRepetition_e4x,    1001,   998,  2, 0xc833333,  2,  75,    0,   1,    1,   2,    4,   "135135" }, /* 135135000 */
	{BFMT_PXL_27MHz_MUL_1_001,     BAVC_HDMI_BitsPerPixel_e36bit, BAVC_HDMI_PixelRepetition_e4x,    1001,   998,  2, 0xf03d70a,  2,  90,    1,   1,    1,   2,    7,   "162162" }, /* 162162000 */
	{BFMT_PXL_67_565MHz,           BAVC_HDMI_BitsPerPixel_e24bit, BAVC_HDMI_PixelRepetition_eNone,  3128,  3119,  2, 0xc83126e,  2,  75,    0,   2,    1,   2,    4,    "67564" }, /* 67564800 */
	{BFMT_PXL_67_565MHz,           BAVC_HDMI_BitsPerPixel_e30bit, BAVC_HDMI_PixelRepetition_eNone, 13513, 12718,  2, 0x1f47b42,  0,  46,    0,   1,    1,   2,    4,    "84456" }, /* 84456250 */
	{BFMT_PXL_67_565MHz,           BAVC_HDMI_BitsPerPixel_e36bit, BAVC_HDMI_PixelRepetition_eNone, 13513, 13294,  2, 0x25893e9,  0,  56,    0,   1,    1,   2,    4,   "101347" }, /* 101347500 */

	{BFMT_PXL_67_565MHz_DIV_1_001, BAVC_HDMI_BitsPerPixel_e24bit, BAVC_HDMI_PixelRepetition_eNone, 24999, 24002,  2, 0xc7fdf3b,  2,  74,    0,   2,    1,   2,    4,    "67497" }, /* 67497300 */

	{BFMT_PXL_74_25MHz,            BAVC_HDMI_BitsPerPixel_e24bit, BAVC_HDMI_PixelRepetition_eNone,    55,    54,  2, 0x6e00000,  1,  82,    0,   2,    1,   1,    4,    "74.25" }, /* 74.250000 MHz */
	{BFMT_PXL_74_25MHz,            BAVC_HDMI_BitsPerPixel_e30bit, BAVC_HDMI_PixelRepetition_eNone,   275,   266,  2, 0x8980000,  2,  51,    0,   1,    1,   2,    4,    "92812" }, /* 92812500 */
	{BFMT_PXL_74_25MHz,            BAVC_HDMI_BitsPerPixel_e36bit, BAVC_HDMI_PixelRepetition_eNone,   165,   158,  2, 0xa500000,  2,  61,    0,   1,    1,   2,    4,   "111375" }, /* 111375000 */

	{BFMT_PXL_74_25MHz_DIV_1_001,  BAVC_HDMI_BitsPerPixel_e24bit, BAVC_HDMI_PixelRepetition_eNone,  1250,  1231,  2, 0x6de3de3,  1,  82,    0,   2,    1,   1,    4,    "74.17" }, /* 74.175824 MHz */
	{BFMT_PXL_74_25MHz_DIV_1_001,  BAVC_HDMI_BitsPerPixel_e30bit, BAVC_HDMI_PixelRepetition_eNone,  3125,  3032,  2, 0x895cd5c,  2,  51,    0,   1,    1,   2,    4,    "92719" }, /* 92719780.2197802 */
	{BFMT_PXL_74_25MHz_DIV_1_001,  BAVC_HDMI_BitsPerPixel_e36bit, BAVC_HDMI_PixelRepetition_eNone,  1875,  1801,  2, 0xa4d5cd5,  2,  61,    0,   1,    1,   2,    4,   "111263" }, /* 111263736.263736 */

	{BFMT_PXL_148_5MHz,            BAVC_HDMI_BitsPerPixel_e24bit, BAVC_HDMI_PixelRepetition_eNone,    55,    54,  2, 0xdc00000,  2,  82,    0,   1,    1,   2,    4,   "148500" }, /* 148500000 */
	{BFMT_PXL_148_5MHz,            BAVC_HDMI_BitsPerPixel_e30bit, BAVC_HDMI_PixelRepetition_eNone,   275,   274,  2, 0x11300000, 3, 103,    1,   1,    1,   2,    7,   "185625" }, /* 185625000 */
	{BFMT_PXL_148_5MHz,            BAVC_HDMI_BitsPerPixel_e36bit, BAVC_HDMI_PixelRepetition_eNone,    55,    54,  2, 0x14a00000, 3, 123,    1,   1,    1,   2,    7,   "222750" }, /* 222750000 */

	{BFMT_PXL_148_5MHz_DIV_1_001,  BAVC_HDMI_BitsPerPixel_e24bit, BAVC_HDMI_PixelRepetition_eNone,  1250,  1231,  2, 0xdbc7bc7,  2,  82,    0,   1,    1,   2,    4,   "148351" }, /* 148351648.351648 */
	{BFMT_PXL_148_5MHz_DIV_1_001,  BAVC_HDMI_BitsPerPixel_e30bit, BAVC_HDMI_PixelRepetition_eNone,  3125,  3123,  2, 0x112b9ab9, 3, 103,    1,   1,    1,   2,    7,   "185439" }, /* 185439560.43956 */
	{BFMT_PXL_148_5MHz_DIV_1_001,  BAVC_HDMI_BitsPerPixel_e36bit, BAVC_HDMI_PixelRepetition_eNone,  1250,  1231,  2, 0x149ab9ab, 3, 123,    1,   1,    1,   2,    7,   "222527" }, /* 222527472.527473 */

	/* 60/50 Hz VESA formats */
	{BFMT_PXL_39_79MHz,            BAVC_HDMI_BitsPerPixel_e24bit, BAVC_HDMI_PixelRepetition_eNone,   628,   619,  2, 0x586c226,  1,  66,    0,   3,    1,   1,    4,    "39.79" }, /* 39.790080 MHz */
	{BFMT_PXL_39_79MHz,            BAVC_HDMI_BitsPerPixel_e30bit, BAVC_HDMI_PixelRepetition_eNone,  1727,  1671,  2, 0x3743958,  0,  82,    0,   3,    1,   2,    4,    "49737" }, /* 49737600 */
	{BFMT_PXL_39_79MHz,            BAVC_HDMI_BitsPerPixel_e36bit, BAVC_HDMI_PixelRepetition_eNone,   628,   619,  2, 0x2c36113,  0,  66,    0,   2,    1,   2,    4,    "59685" }, /* 59685120 */

	{BFMT_PXL_65MHz,               BAVC_HDMI_BitsPerPixel_e24bit, BAVC_HDMI_PixelRepetition_eNone,   325,   322,  2, 0x604bda1,  1,  72,    0,   2,    1,   1,    4,    "65.00" }, /* 65.000000 MHz */
	{BFMT_PXL_65MHz,               BAVC_HDMI_BitsPerPixel_e30bit, BAVC_HDMI_PixelRepetition_eNone,   325,   322,  2, 0x1e17b42,  0,  45,    0,   1,    1,   2,    4,    "81250" }, /* 81250000 */
	{BFMT_PXL_65MHz,               BAVC_HDMI_BitsPerPixel_e36bit, BAVC_HDMI_PixelRepetition_eNone,   325,   322,  2, 0x241c71c,  0,  54,    0,   1,    1,   2,    4,    "97500" }, /* 97500000 */

	{BFMT_PXL_65_286MHz,           BAVC_HDMI_BitsPerPixel_e24bit, BAVC_HDMI_PixelRepetition_eNone,   403,   394,  2, 0x60b851e,  1,  72,    0,   2,    1,   1,    4,    "65.28" }, /* 65.286000 MHz */
	{BFMT_PXL_65_286MHz,           BAVC_HDMI_BitsPerPixel_e30bit, BAVC_HDMI_PixelRepetition_eNone,   403,   394,  2, 0x1e39999,  0,  45,    0,   1,    1,   2,    4,    "81607" }, /* 81607500 */
	{BFMT_PXL_65_286MHz,           BAVC_HDMI_BitsPerPixel_e36bit, BAVC_HDMI_PixelRepetition_eNone,   403,   394,  2, 0x24451eb,  0,  54,    0,   1,    1,   2,    4,    "97929" }, /* 97929000 */

	{BFMT_PXL_64_022MHz,           BAVC_HDMI_BitsPerPixel_e24bit, BAVC_HDMI_PixelRepetition_eNone,   2964, 2947,  2, 0x2f6c8b4,  0,  71,    0,   2,    1,   2,    4,    "64022" }, /* 64022400 */
	{BFMT_PXL_64_022MHz,           BAVC_HDMI_BitsPerPixel_e30bit, BAVC_HDMI_PixelRepetition_eNone,  32011,31018,  2, 0x1da3cae,  0,  44,    0,   1,    1,   2,    4,    "80027" }, /* 80027500 */
	{BFMT_PXL_64_022MHz,           BAVC_HDMI_BitsPerPixel_e36bit, BAVC_HDMI_PixelRepetition_eNone,  32011,31378,  2, 0x239159e,  0,  53,    0,   1,    1,   2,    4,    "96033" }, /* 96033000 */

	/* 59.94 Hz VESA formats */
	{BFMT_PXL_39_79MHz_DIV_1_001,  BAVC_HDMI_BitsPerPixel_e24bit, BAVC_HDMI_PixelRepetition_eNone,  5024,  4967,  2, 0x5855855,  1,  66,    0,   3,    1,   1,    4,    "39.70/1.001" }, /* 39.750329 MHz */
	{BFMT_PXL_39_79MHz_DIV_1_001,  BAVC_HDMI_BitsPerPixel_e30bit, BAVC_HDMI_PixelRepetition_eNone,  1256,  1219,  2, 0x6e6ae6a,  1,  82,    0,   3,    1,   1,    4,    "49737" }, /* 49687912.0879121 */
	{BFMT_PXL_39_79MHz_DIV_1_001,  BAVC_HDMI_BitsPerPixel_e36bit, BAVC_HDMI_PixelRepetition_eNone,  5024,  4967,  2, 0x5855855,  1,  66,    0,   2,    1,   1,    4,    "59685" }, /* 59625494.5054945 */

	{BFMT_PXL_65MHz_DIV_1_001,     BAVC_HDMI_BitsPerPixel_e24bit, BAVC_HDMI_PixelRepetition_eNone,   496,   493,  2, 0x6031a60,  1,  72,    0,   2,    1,   1,    4, "65.00/1.001" }, /* 64.930909 MHz */
	{BFMT_PXL_65MHz_DIV_1_001,     BAVC_HDMI_BitsPerPixel_e30bit, BAVC_HDMI_PixelRepetition_eNone,  6250,  6211,  2, 0x3c2003f,  0,  45,    0,   1,    1,   1,    4,    "81250" }, /* 81168831.1688312 */
	{BFMT_PXL_65MHz_DIV_1_001,     BAVC_HDMI_BitsPerPixel_e36bit, BAVC_HDMI_PixelRepetition_eNone,  6250,  6211,  2, 0x48266b2,  1,  54,    0,   1,    1,   1,    4,    "97500" }, /* 97402597.4025974 */

	{BFMT_PXL_65_286MHz_DIV_1_001, BAVC_HDMI_BitsPerPixel_e24bit, BAVC_HDMI_PixelRepetition_eNone,   155,   152,  2, 0x609f959,  1,  72,    0,   2,    1,   1,    4, "65.28/1.001" }, /* 65.220779 MHz */
	{BFMT_PXL_65_286MHz_DIV_1_001, BAVC_HDMI_BitsPerPixel_e30bit, BAVC_HDMI_PixelRepetition_eNone,   155,   152,  2, 0x3c63bd8,  0,  45,    0,   1,    1,   1,    4,    "81607" }, /* 81525974.025974 */
	{BFMT_PXL_65_286MHz_DIV_1_001, BAVC_HDMI_BitsPerPixel_e36bit, BAVC_HDMI_PixelRepetition_eNone,   155,   152,  2, 0x4877b03,  1,  54,    0,   1,    1,   1,    4,    "97929" }, /* 97831168.8311688 */

	{BFMT_PXL_64_022MHz_DIV_1_001, BAVC_HDMI_BitsPerPixel_e24bit, BAVC_HDMI_PixelRepetition_eNone,  1824,  1819,  2, 0x2f606a6,  0,  71,    0,   2,    1,   2,    4,    "64022" }, /* 63958441.5584416 */
	{BFMT_PXL_64_022MHz_DIV_1_001, BAVC_HDMI_BitsPerPixel_e36bit, BAVC_HDMI_PixelRepetition_eNone, 22865, 22481,  2, 0x4710827,  1,  53,    0,   1,    1,   1,    4,    "96033" }, /* 95937062.9370629 */

#elif (BVDC_P_SUPPORT_HDMI_RM_VER == BVDC_P_HDMI_RM_VER_5 || BVDC_P_SUPPORT_HDMI_RM_VER == BVDC_P_HDMI_RM_VER_6)

	/* Mask                        ColorDepth                     PixelRep                         Denom    Num Sample  Offset shift rmdiv vco pxDiv KP KI KA pchRate */
	{BFMT_PXL_25_2MHz,             BAVC_HDMI_BitsPerPixel_e24bit, BAVC_HDMI_PixelRepetition_eNone,    56,    31,  1, 0x0E000000, 2,   87,  12,   1,  8, 3, 3, "25200" }, /* 25200000 */
	{BFMT_PXL_25_2MHz,             BAVC_HDMI_BitsPerPixel_e30bit, BAVC_HDMI_PixelRepetition_eNone,    25,    14,  1, 0x0E955555, 2,   91,  10,   1,  8, 3, 3, "31500" }, /* 31500000 */
	{BFMT_PXL_25_2MHz,             BAVC_HDMI_BitsPerPixel_e36bit, BAVC_HDMI_PixelRepetition_eNone,    56,    31,  1, 0x0E000000, 2,   87,   8,   1,  8, 3, 3, "37800" }, /* 37800000 */

	{BFMT_PXL_25_2MHz_DIV_1_001,   BAVC_HDMI_BitsPerPixel_e24bit, BAVC_HDMI_PixelRepetition_eNone,  4000,  2149,  1, 0x0DFC6B6A, 2,   86,  12,   1,  8, 3, 3, "25174" }, /* 25174825.1748252 */
	{BFMT_PXL_25_2MHz_DIV_1_001,   BAVC_HDMI_BitsPerPixel_e30bit, BAVC_HDMI_PixelRepetition_eNone,  2500,  1361,  1, 0x0E919A8E, 2,   90,  10,   1,  8, 3, 3, "31469" }, /* 31468531.4685315 */
	{BFMT_PXL_25_2MHz_DIV_1_001,   BAVC_HDMI_BitsPerPixel_e36bit, BAVC_HDMI_PixelRepetition_eNone,  4000,  2149,  1, 0x0DFC6B6A, 2,   86,   8,   1,  8, 3, 3, "37762" }, /* 37762237.7622378 */

	{BFMT_PXL_27MHz,               BAVC_HDMI_BitsPerPixel_e24bit, BAVC_HDMI_PixelRepetition_eNone,    55,    28,  1, 0x0DC00000, 2,   83,  11,   1,  8, 3, 3, "27000" }, /* 27000000 */
	{BFMT_PXL_27MHz,               BAVC_HDMI_BitsPerPixel_e30bit, BAVC_HDMI_PixelRepetition_eNone,    75,    41,  1, 0x0E100000, 2,   87,   9,   1,  8, 3, 3, "33750" }, /* 33750000 */
	{BFMT_PXL_27MHz,               BAVC_HDMI_BitsPerPixel_e36bit, BAVC_HDMI_PixelRepetition_eNone,    21,    11,  1, 0x0D200000, 2,   80,   7,   1,  8, 3, 3, "40500" }, /* 40500000 */

	{BFMT_PXL_27MHz,               BAVC_HDMI_BitsPerPixel_e24bit, BAVC_HDMI_PixelRepetition_e2x,      20,    11,  1, 0x0F000000, 2,   93,   6,   1, 10, 4, 3, "54000" }, /* 54000000 */
	{BFMT_PXL_27MHz,               BAVC_HDMI_BitsPerPixel_e30bit, BAVC_HDMI_PixelRepetition_e2x,      50,    27,  1, 0x0C800000, 2,   77,   4,   1,  8, 3, 3, "67500" }, /* 67500000 */
	{BFMT_PXL_27MHz,               BAVC_HDMI_BitsPerPixel_e36bit, BAVC_HDMI_PixelRepetition_e2x,      20,    11,  1, 0x0F000000, 2,   93,   4,   1, 10, 4, 3, "81000" }, /* 81000000 */

	{BFMT_PXL_27MHz,               BAVC_HDMI_BitsPerPixel_e24bit, BAVC_HDMI_PixelRepetition_e4x,      20,    11,  1, 0x0F000000, 2,   93,   3,   1, 10, 4, 3, "108000" }, /* 108000000 */
	{BFMT_PXL_27MHz,               BAVC_HDMI_BitsPerPixel_e30bit, BAVC_HDMI_PixelRepetition_e4x,      50,    27,  1, 0x0C800000, 2,   77,   2,   1,  8, 3, 3, "135000" }, /* 135000000 */
	{BFMT_PXL_27MHz,               BAVC_HDMI_BitsPerPixel_e36bit, BAVC_HDMI_PixelRepetition_e4x,      20,    11,  1, 0x0F000000, 2,   93,   2,   1, 10, 4, 3, "162000" }, /* 162000000 */

	{BFMT_PXL_27MHz_MUL_1_001,     BAVC_HDMI_BitsPerPixel_e24bit, BAVC_HDMI_PixelRepetition_eNone,  1573,   827,  1, 0x0DC3851F, 2,   84,  11,   1,  8, 3, 3,  "27027" }, /* 27027000 */
	{BFMT_PXL_27MHz_MUL_1_001,     BAVC_HDMI_BitsPerPixel_e30bit, BAVC_HDMI_PixelRepetition_eNone,  9009,  4751,  1, 0x0E13999A, 2,   86,   9,   1,  8, 3, 3,  "33783" }, /* 33716283.7162837 */
	{BFMT_PXL_27MHz_MUL_1_001,     BAVC_HDMI_BitsPerPixel_e36bit, BAVC_HDMI_PixelRepetition_eNone,  7007,  3793,  1, 0x0D235C29, 2,   81,   7,   1,  8, 3, 3,  "40459" }, /* 40459540.4595405 */

	{BFMT_PXL_27MHz_MUL_1_001,     BAVC_HDMI_BitsPerPixel_e24bit, BAVC_HDMI_PixelRepetition_e2x,     33,     17,  1, 0x0F03D70A, 2,   91,   6,   1, 10, 4, 3,  "54054" }, /* 54.054 MHz */
	{BFMT_PXL_27MHz_MUL_1_001,     BAVC_HDMI_BitsPerPixel_e30bit, BAVC_HDMI_PixelRepetition_e2x,    1001,   599,  1, 0x0C833333, 2,   80,   4,   1,  8, 3, 3,  "67567" }, /* 67567500 */
	{BFMT_PXL_27MHz_MUL_1_001,     BAVC_HDMI_BitsPerPixel_e36bit, BAVC_HDMI_PixelRepetition_e2x,     33,     17,  1, 0x0F03D70A, 2,   91,   4,   1, 10, 4, 3,  "81081" }, /* 81081000 */

	{BFMT_PXL_27MHz_MUL_1_001,     BAVC_HDMI_BitsPerPixel_e24bit, BAVC_HDMI_PixelRepetition_e4x,      33,    17,  1, 0x0F03D70A, 2,   91,   3,   1, 10, 4, 3, "108108" }, /* 108108000 */
	{BFMT_PXL_27MHz_MUL_1_001,     BAVC_HDMI_BitsPerPixel_e30bit, BAVC_HDMI_PixelRepetition_e4x,    1001,   599,  1, 0x0C833333, 2,   80,   2,   1,  8, 3, 3, "135135" }, /* 135135000 */
	{BFMT_PXL_27MHz_MUL_1_001,     BAVC_HDMI_BitsPerPixel_e36bit, BAVC_HDMI_PixelRepetition_e4x,    33,    17,    1, 0x0F03D70A, 2,   91,   2,   1, 10, 4, 3, "162162" }, /* 162162000 */

	{BFMT_PXL_67_565MHz,           BAVC_HDMI_BitsPerPixel_e24bit, BAVC_HDMI_PixelRepetition_eNone,  6256,  3369,  1, 0x0C83126F, 2,   77,   4,   1,  8, 3, 3,  "67564" }, /* 67564800 */
	{BFMT_PXL_67_565MHz,           BAVC_HDMI_BitsPerPixel_e30bit, BAVC_HDMI_PixelRepetition_eNone,  1564,   861,  1, 0x0FA3D70A, 2,   97,   4,   1, 10, 4, 3,  "84456" }, /* 84456250 */
	{BFMT_PXL_67_565MHz,           BAVC_HDMI_BitsPerPixel_e36bit, BAVC_HDMI_PixelRepetition_eNone,  3519,  1856,  1, 0x0E1374BC, 2,   86,   3,   1,  8, 3, 3, "101347" }, /* 101347500 */

	{BFMT_PXL_67_565MHz_DIV_1_001, BAVC_HDMI_BitsPerPixel_e24bit, BAVC_HDMI_PixelRepetition_eNone, 25024, 14015,  1, 0x0C7FDF44, 2,   78,   4,   1,  8, 3, 3,  "67497" }, /* 67497300 */
	{BFMT_PXL_67_565MHz_DIV_1_001, BAVC_HDMI_BitsPerPixel_e30bit, BAVC_HDMI_PixelRepetition_eNone, 12512,  6507,  1, 0x0F9FD715, 2,   95,   4,   1, 10, 4, 3,  "84371" }, /* 84371250 */
	{BFMT_PXL_67_565MHz_DIV_1_001, BAVC_HDMI_BitsPerPixel_e36bit, BAVC_HDMI_PixelRepetition_eNone, 18768, 10261,  1, 0x0E0FDB2C, 2,   87,   3,   1,  8, 3, 3, "101245" }, /* 101245500 */

	{BFMT_PXL_74_25MHz,            BAVC_HDMI_BitsPerPixel_e24bit, BAVC_HDMI_PixelRepetition_eNone,    55,    28,  1, 0x0DC00000,  2, 83,    4,    1,  8, 3, 3,  "74.25" }, /* 74.250000 MHz */
	{BFMT_PXL_74_25MHz,            BAVC_HDMI_BitsPerPixel_e30bit, BAVC_HDMI_PixelRepetition_eNone,   165,    91,  1, 0x0CE40000,  2, 80,    3,    1,  8, 3, 3,  "92812" }, /* 92812500 */
	{BFMT_PXL_74_25MHz,            BAVC_HDMI_BitsPerPixel_e36bit, BAVC_HDMI_PixelRepetition_eNone,   165,    91,  1, 0x0F780000,  2, 96,    3,    1, 10, 4, 3, "111375" }, /* 111375000 */

	{BFMT_PXL_74_25MHz_DIV_1_001,  BAVC_HDMI_BitsPerPixel_e24bit, BAVC_HDMI_PixelRepetition_eNone,  1000,   547,  1, 0x0DBC7BC8,  2,  85,   4,    1,  8, 3, 3,  "74.17" }, /* 74.175824 MHz */
	{BFMT_PXL_74_25MHz_DIV_1_001,  BAVC_HDMI_BitsPerPixel_e30bit, BAVC_HDMI_PixelRepetition_eNone,  9375,  5003,  1, 0x0CE0B40B,  2,  79,   3,    1,  8, 3, 3,  "92719" }, /* 92719780.2197802 */
	{BFMT_PXL_74_25MHz_DIV_1_001,  BAVC_HDMI_BitsPerPixel_e36bit, BAVC_HDMI_PixelRepetition_eNone,  1125,   604,  1, 0x0F740B41,  2,  95,   3,    1, 10, 4, 3, "111263" }, /* 111263736.263736 */

	{BFMT_PXL_148_5MHz,            BAVC_HDMI_BitsPerPixel_e24bit, BAVC_HDMI_PixelRepetition_eNone,    55,    28,  1, 0x0DC00000, 2,   83,   2,    1,  8, 3, 3,  "148500" }, /* 148500000 */
	{BFMT_PXL_148_5MHz,            BAVC_HDMI_BitsPerPixel_e30bit, BAVC_HDMI_PixelRepetition_eNone,   275,   149,  1, 0x11300000, 3,   106,  2,    1, 10, 4, 3,  "185625" }, /* 185625000 */
	{BFMT_PXL_148_5MHz,            BAVC_HDMI_BitsPerPixel_e36bit, BAVC_HDMI_PixelRepetition_eNone,   165,    91,  1, 0x0A500000, 2,   64,   1,    1,  8, 3, 3,  "222750" }, /* 222750000 */

	{BFMT_PXL_148_5MHz_DIV_1_001,  BAVC_HDMI_BitsPerPixel_e24bit, BAVC_HDMI_PixelRepetition_eNone,  1000,   547,  1, 0x0DBC7BC8, 2,   85,   2,    1,  8, 3, 3,  "148351" }, /* 148351648.351648 */
	{BFMT_PXL_148_5MHz_DIV_1_001,  BAVC_HDMI_BitsPerPixel_e30bit, BAVC_HDMI_PixelRepetition_eNone,  3125,  1698,  1, 0x112B9ABA, 3,  106,   2,    1, 10, 4, 3,  "185439" }, /* 185439560.43956 */
	{BFMT_PXL_148_5MHz_DIV_1_001,  BAVC_HDMI_BitsPerPixel_e36bit, BAVC_HDMI_PixelRepetition_eNone,  1875,  1037,  1, 0x0A4D5CD6, 2,   64,   1,    1,  8, 3, 3,  "222527" }, /* 222527472.527473 */

	/* 60/50 Hz VESA formats */
	{BFMT_PXL_39_79MHz,            BAVC_HDMI_BitsPerPixel_e24bit, BAVC_HDMI_PixelRepetition_eNone,  3979,  2096,  1, 0x0EBCAE76, 2,   90,   8,    1,  8, 3, 3,   "39790" }, /* 39790080 */
	{BFMT_PXL_39_79MHz,            BAVC_HDMI_BitsPerPixel_e30bit, BAVC_HDMI_PixelRepetition_eNone,  3979,  2141,  1, 0x0DD0E38E, 2,   85,   6,    1,  8, 3, 3,   "49737" }, /* 49737600 */
	{BFMT_PXL_39_79MHz,            BAVC_HDMI_BitsPerPixel_e36bit, BAVC_HDMI_PixelRepetition_eNone,  3979,  2141,  1, 0x0DD0E38E, 2,   85,   5,    1,  8, 3, 3,   "59685" }, /* 59685120 */

	{BFMT_PXL_65MHz,               BAVC_HDMI_BitsPerPixel_e24bit, BAVC_HDMI_PixelRepetition_eNone,  1625,   886,  1, 0x0F0BDA13, 2,   93,   5,    1, 10, 4, 3,   "65000" }, /* 65.000000 MHz */
	{BFMT_PXL_65MHz,               BAVC_HDMI_BitsPerPixel_e30bit, BAVC_HDMI_PixelRepetition_eNone,  1625,   886,  1, 0x0F0BDA13, 2,   93,   4,    1, 10, 4, 3,   "81250" }, /* 81250000 */
	{BFMT_PXL_65MHz,               BAVC_HDMI_BitsPerPixel_e36bit, BAVC_HDMI_PixelRepetition_eNone,   325,   173,  1, 0x0D8AAAAB, 2,   83,   3,    1,  8, 3, 3,   "97500" }, /* 97500000 */

	{BFMT_PXL_65_286MHz,           BAVC_HDMI_BitsPerPixel_e24bit, BAVC_HDMI_PixelRepetition_eNone,    13,     7,  1, 0x0F1CCCCD, 2,   93,   5,    1, 10, 4, 3,   "65286" }, /* 65.286000 MHz */
	{BFMT_PXL_65_286MHz,           BAVC_HDMI_BitsPerPixel_e30bit, BAVC_HDMI_PixelRepetition_eNone,    13,     7,  1, 0x0F1CCCCD, 2,   93,   4,    1, 10, 4, 3,   "81607" }, /* 81607500 */
	{BFMT_PXL_65_286MHz,           BAVC_HDMI_BitsPerPixel_e36bit, BAVC_HDMI_PixelRepetition_eNone,  3627,  1973,  1, 0x0D99EB85, 2,   84,   3,    1,  8, 3, 3,   "97929" }, /* 97929000 */

	{BFMT_PXL_64_022MHz,           BAVC_HDMI_BitsPerPixel_e24bit, BAVC_HDMI_PixelRepetition_eNone,   114,    61,  1, 0x0ED1EB85, 2,   91,   5,    1, 10, 4, 3,   "64022" }, /* 64022400 */
	{BFMT_PXL_64_022MHz,           BAVC_HDMI_BitsPerPixel_e30bit, BAVC_HDMI_PixelRepetition_eNone,   114,    61,  1, 0x0ED1EB85, 2,   91,   4,    1, 10, 4, 3,   "80027" }, /* 80027500 */
	{BFMT_PXL_64_022MHz,           BAVC_HDMI_BitsPerPixel_e36bit, BAVC_HDMI_PixelRepetition_eNone,  6669,  3581,  1, 0x0D56872B, 2,   82,   3,    1,  8, 3, 3,   "96033" }, /* 96033000 */

	/* 59.94 Hz VESA formats */
	{BFMT_PXL_39_79MHz_DIV_1_001,  BAVC_HDMI_BitsPerPixel_e24bit, BAVC_HDMI_PixelRepetition_eNone,   9200,  8627, 2, 0x0EB8E99E, 2,  173,   8,    1, 8,  3, 3,   "39750" }, /* 39750329 */
	{BFMT_PXL_39_79MHz_DIV_1_001,  BAVC_HDMI_BitsPerPixel_e30bit, BAVC_HDMI_PixelRepetition_eNone,  21625, 14411, 1, 0x0DCD5B04, 2,   92,   6,    1, 8,  3, 3,   "49688" }, /* 49687912.0879121 */
	{BFMT_PXL_39_79MHz_DIV_1_001,  BAVC_HDMI_BitsPerPixel_e36bit, BAVC_HDMI_PixelRepetition_eNone,  21625, 14411, 1, 0x0DCD5B04, 2,   92,   5,    1, 8,  3, 3,   "59625" }, /* 59625494.5054945 */


	{BFMT_PXL_65MHz_DIV_1_001,     BAVC_HDMI_BitsPerPixel_e24bit, BAVC_HDMI_PixelRepetition_eNone, 31250, 16567,  1, 0x0F0800FC,  2,  92,   5,    1, 10, 4, 3,   "64931" }, /* 64930909 */
	{BFMT_PXL_65MHz_DIV_1_001,     BAVC_HDMI_BitsPerPixel_e30bit, BAVC_HDMI_PixelRepetition_eNone, 31250, 16567,  1, 0x0F0800FC,  2,  92,   4,    1, 10, 4, 3,   "81169" }, /* 81168831.1688312 */
	{BFMT_PXL_65MHz_DIV_1_001,     BAVC_HDMI_BitsPerPixel_e36bit, BAVC_HDMI_PixelRepetition_eNone, 12500,  6673,  1, 0x0D873416,  2,  83,   3,    1,  8, 3, 3,   "97403" }, /* 97402597.4025974 */

	{BFMT_PXL_65_286MHz_DIV_1_001, BAVC_HDMI_BitsPerPixel_e24bit, BAVC_HDMI_PixelRepetition_eNone,    50,    27,  1, 0x0F18EF60,  2,  93,   5,    1, 10, 4, 3,   "65221" }, /* 65220779 */
	{BFMT_PXL_65_286MHz_DIV_1_001, BAVC_HDMI_BitsPerPixel_e30bit, BAVC_HDMI_PixelRepetition_eNone,    50,    27,  1, 0x0F18EF60,  2,  93,   4,    1, 10, 4, 3,   "81526" }, /* 81525974.025974 */
	{BFMT_PXL_65_286MHz_DIV_1_001, BAVC_HDMI_BitsPerPixel_e36bit, BAVC_HDMI_PixelRepetition_eNone,  4185,  2591,  1, 0x0D96710A,  2,  88,   3,    1,  8, 3, 3,   "97831" }, /* 97831168.8311688 */

	{BFMT_PXL_64_022MHz_DIV_1_001, BAVC_HDMI_BitsPerPixel_e24bit, BAVC_HDMI_PixelRepetition_eNone,  4560,  2447,  1, 0x0ECE213F,  2,  91,   5,    1,  8, 3, 3,   "63958" }, /* 63958441.5584416 */
	{BFMT_PXL_64_022MHz_DIV_1_001, BAVC_HDMI_BitsPerPixel_e30bit, BAVC_HDMI_PixelRepetition_eNone,  4560,  2447,  1, 0x0ECE213F,  2,  91,   4,    1,  8, 3, 3,   "79947" }, /* 79947500 */
	{BFMT_PXL_64_022MHz_DIV_1_001, BAVC_HDMI_BitsPerPixel_e36bit, BAVC_HDMI_PixelRepetition_eNone,  2052,  1105,  1, 0x0D531DEC,  2,  82,   3,    1,  8, 3, 3,   "95937" }, /* 95937062.9370629 */

	{BFMT_PXL_108MHz,              BAVC_HDMI_BitsPerPixel_e24bit, BAVC_HDMI_PixelRepetition_eNone,    20,    11,  1, 0x0F000000,  2,  93,   3,    1, 10, 4, 3, "108.000"}, /* Pixel Clock = 108000000.000000, TMDS Clock = 108000000.000000, Color Depth = 24, fRM = 17.419, fVCO = (3240.00000000000, 3240.00000000000, 3240.00000000000), ABS(Offset Error) = 0 */
	{BFMT_PXL_108MHz,              BAVC_HDMI_BitsPerPixel_e30bit, BAVC_HDMI_PixelRepetition_eNone,    50,    27,  1, 0x0C800000,  2,  77,   2,    1,  8, 3, 3, "135.000"}, /* Pixel Clock = 108000000.000000, TMDS Clock = 135000000.000000, Color Depth = 30, fRM = 17.532, fVCO = (2700.00000000000, 2700.00000000000, 2700.00000000000), ABS(Offset Error) = 0 */
	{BFMT_PXL_108MHz,              BAVC_HDMI_BitsPerPixel_e36bit, BAVC_HDMI_PixelRepetition_eNone,    20,    11,  1, 0x0F000000,  2,  93,   2,    1, 10, 4, 3, "162.000"}, /* Pixel Clock = 108000000.000000, TMDS Clock = 162000000.000000, Color Depth = 36, fRM = 17.419, fVCO = (3240.00000000000, 3240.00000000000, 3240.00000000000), ABS(Offset Error) = 0 */

	{BFMT_PXL_108MHz_DIV_1_001,    BAVC_HDMI_BitsPerPixel_e24bit, BAVC_HDMI_PixelRepetition_eNone, 15000,  8023,  1, 0x0EFC29F1,  2,  92,   3,    1, 10, 4, 3, "107.892"}, /* Pixel Clock = 107892107.892108, TMDS Clock = 107892107.892108, Color Depth = 24, fRM = 17.591, fVCO = (3236.76323676324, 3236.76323676324, 3236.76323461533), ABS(Offset Error) = 0.166833162307739 */
	{BFMT_PXL_108MHz_DIV_1_001,    BAVC_HDMI_BitsPerPixel_e30bit, BAVC_HDMI_PixelRepetition_eNone, 25000, 14039,  1, 0x0C7CCD9E,  2,  78,   2,    1,  8, 3, 3, "134.865"}, /* Pixel Clock = 107892107.892108, TMDS Clock = 134865134.865135, Color Depth = 30, fRM = 17.290, fVCO = (2697.30269730270, 2697.30269730270, 2697.30269336700), ABS(Offset Error) = 0.305694341659546 */
	{BFMT_PXL_108MHz_DIV_1_001,    BAVC_HDMI_BitsPerPixel_e36bit, BAVC_HDMI_PixelRepetition_eNone, 15000,  8023,  1, 0x0EFC29F1,  2,  92,   2,    1, 10, 4, 3, "161.838"}, /* Pixel Clock = 107892107.892108, TMDS Clock = 161838161.838162, Color Depth = 36, fRM = 17.591, fVCO = (3236.76323676324, 3236.76323676324, 3236.76323461533), ABS(Offset Error) = 0.166833162307739 */


#else
	/* Mask                        M   P    N     RDiv  Sample   Num   Denom   OFFSET   BHDM_PixelClock */
	{BFMT_PXL_25_2MHz,             0,  5,   28,   210,     7,     1,      2,      0, "25.20"}, /* 25.200000 MHz */
	{BFMT_PXL_25_2MHz_DIV_1_001,   0,  5,   28,   210,     7,   203,    400,  15860, "25.17"}, /* 25.174825 MHz */

	{BFMT_PXL_27MHz,               0,  2,   15,   105,     7,     0,      1,      0, "27.00"}, /* 27.000000 MHz */
	{BFMT_PXL_27MHz_MUL_1_001,     0,  2,   15,   105,     6,   142,    143,    523, "27.02"}, /* 27.027000 MHz */

	{BFMT_PXL_74_25MHz,            1,  1,   55,    70,     2,     6,     11,      0, "74.25"}, /* 74.250000 MHz */
	{BFMT_PXL_74_25MHz_DIV_1_001,  1,  1,   55,    70,     2,   137,    250,  15860, "74.17"}, /* 74.175824 MHz */

	{BFMT_PXL_56_304MHz,           3,  1,   83,    70,     3,   279,    782,   2597, "56.30"}, /* 56.304000 MHz 1440x782 @ 50Hz */
	{BFMT_PXL_67_565MHz_DIV_1_001, 0,  1,   25,    70,     2, 10011,  12512,  16364, "67.49"}, /* 67.497303 MHz 1440x782 @ 59.94Hz*/
	{BFMT_PXL_67_565MHz,           0,  1,   25,    70,     2,  1247,   1564,    502, "67.56"}, /* 67.564800 MHz 1440x782 @ 60Hz */

	/* 60/50 Hz VESA formats */
	{BFMT_PXL_39_79MHz,            1,  3,   59,   140,     4, 10361,  13816,  15924, "39.79"}, /* 39.790080 MHz */
	{BFMT_PXL_65MHz,               0,  1,   24,    70,     2,  2927,   3224,   1579, "65.00"}, /* 64.995840 MHz */
	{BFMT_PXL_65_286MHz,           3,  1,   97,    70,     2,  1082,   1209,  14867, "65.28"}, /* 65.286000 MHz */
	{BFMT_PXL_60_4656MHz,          2,  1,   67,    70,     3,   528,   4199,   1435, "60.47"}, /* 60.465600 MHz */
	{BFMT_PXL_64_022MHz,           3,  1,   95,    70,     2,  1411,   1482,  15544, "64.02"},  /* 64.022400 MHz */

	/* 59.94 Hz VESA formats */
	{BFMT_PXL_39_79MHz_DIV_1_001,  1,  3,   59,   140,     4,  7583,  10048,  15400, "39.79/1.001"}, /* 39.750329 MHz */
	{BFMT_PXL_65MHz_DIV_1_001,     0,  1,   24,    70,     2,  1807,   1984,   1057, "65.00/1.001"}, /* 64.930909 MHz */
	{BFMT_PXL_65_286MHz_DIV_1_001, 3,  1,   97,    70,     2,   167,    186,  14341, "65.28/1.001"}, /* 65.220779 MHz */
	{BFMT_PXL_64_022MHz_DIV_1_001, 2,  1,   71,    70,     2,   871,    912,    479, "64.02/1.001"}  /* 63.958441 MHz */
#endif
};

/* Number of entries in the above table! */
#define BVDC_P_RM_TABLE_ENTRIES \
	(sizeof(s_HdmiRm) / sizeof(BVDC_P_RateInfo))

#define BVDC_P_MAKE_RM(denom, num, sample, phase, integrator)  \
	BVDC_P_RM_FIELD_DATA(RATE_RATIO, DENOMINATOR,     denom),  \
	BVDC_P_RM_FIELD_DATA(SAMPLE_INC, NUMERATOR,         num) | \
	BVDC_P_RM_FIELD_DATA(SAMPLE_INC, reserved0,           0) | \
	BVDC_P_RM_FIELD_DATA(SAMPLE_INC, SAMPLE_INC,     sample),  \
	BVDC_P_RM_FIELD_DATA(PHASE_INC,  reserved0,           0) | \
	BVDC_P_RM_FIELD_DATA(PHASE_INC,  PHASE_INC,       phase),  \
	BVDC_P_RM_FIELD_DATA(INTEGRATOR, INTEGRATOR, integrator)

static const uint32_t s_aulRmTable_25_2[BVDC_P_RM_TABLE_SIZE]             = { BVDC_P_MAKE_RM(   14,     1,    1, 0x1DDDDE, 0) }; /* 25.2MHz for Vesa 640x480p */
static const uint32_t s_aulRmTable_25_2_Div_1001[BVDC_P_RM_TABLE_SIZE]    = { BVDC_P_MAKE_RM(  400,    29,    1, 0x1DD63A, 0) }; /* 25.2/1.001 Mhz for Vesa 640x480p */
static const uint32_t s_aulRmTable_27[BVDC_P_RM_TABLE_SIZE]               = { BVDC_P_MAKE_RM(    4,     0,    1, 0x200000, 0) }; /* 27Mhz for NTSC, NTSC_J, PAL, PAL_M, PAL_N, PAL_NC, 480p, 576p_50Hz */
static const uint32_t s_aulRmTable_27_Mul_1001[BVDC_P_RM_TABLE_SIZE]      = { BVDC_P_MAKE_RM( 1001,  1000,    0, 0x200831, 0) }; /* 27*1.001 MHz for 480p @60Hz */
static const uint32_t s_aulRmTable_39_79[BVDC_P_RM_TABLE_SIZE]            = { BVDC_P_MAKE_RM(13816,  9375,    0, 0x2F289B, 0) }; /* 39.790080 Mhz for Vesa 00x600p @60Hz refresh rate */
static const uint32_t s_aulRmTable_39_79_Div_1001[BVDC_P_RM_TABLE_SIZE]   = { BVDC_P_MAKE_RM(10048,  6825,    0, 0x2F1C8B, 0) }; /* 39.790080/1.001 Mhz for Vesa 800x600p @59.94Hz refresh rate */
static const uint32_t s_aulRmTable_60_4656[BVDC_P_RM_TABLE_SIZE]          = { BVDC_P_MAKE_RM( 4199,  1875,    0,  0x47A9B, 0) }; /* 60.4656 Mhz for Vesa 1280x720p @50Hz refresh rate */
static const uint32_t s_aulRmTable_64_0224[BVDC_P_RM_TABLE_SIZE]          = { BVDC_P_MAKE_RM( 1482,   625,    0, 0x4BE0DF, 0) }; /* 64.0224 Mhz for Vesa 1280x720p Reduced Blanking @60Hz refresh rate */
static const uint32_t s_aulRmTable_64_0224_Div_1001[BVDC_P_RM_TABLE_SIZE] = { BVDC_P_MAKE_RM(  912,   385,    0, 0x4BCD77, 0) }; /* 64.0224/1.001 Mhz for Vesa 1280x720p Reduced Blanking @59.94Hz refresh rate */
static const uint32_t s_aulRmTable_65[BVDC_P_RM_TABLE_SIZE]               = { BVDC_P_MAKE_RM(22568,  9375,    0, 0x4D0838, 0) }; /* 64.995840 Mhz for Vesa 1024x768p @ 60Hz */
static const uint32_t s_aulRmTable_65_Div_1001[BVDC_P_RM_TABLE_SIZE]      = { BVDC_P_MAKE_RM(1984,    825,    0, 0x4CF485, 0) }; /* 64.995840/1.001 Mhz for Vesa 1024x768p @ 59.94Hz */
static const uint32_t s_aulRmTable_65_286[BVDC_P_RM_TABLE_SIZE]           = { BVDC_P_MAKE_RM(22568,  9375,    0, 0x4D0838, 0) }; /* 65.286 Mhz for Vesa 1280x768 @60Hz refresh rate */
static const uint32_t s_aulRmTable_65_286_Div_1001[BVDC_P_RM_TABLE_SIZE]  = { BVDC_P_MAKE_RM(  586,    77,    0, 0x4D4C78, 0) }; /* 65.286/1.001 Mhz for Vesa 1280x768 @59.94Hz refresh rate */
static const uint32_t s_aulRmTable_74_25[BVDC_P_RM_TABLE_SIZE]            = { BVDC_P_MAKE_RM(   11,     4,    0, 0x580000, 0x0a02530) }; /* 74.25Mhz (1080i, 720p) */
static const uint32_t s_aulRmTable_74_25_Div_1001[BVDC_P_RM_TABLE_SIZE]   = { BVDC_P_MAKE_RM(  250,    91,    0, 0x57e97f, 0x229e910) }; /* 74.25/1.001 Mhz sample rate, 720p_5994Hz or 1080i_2997Hz */
static const uint32_t s_aulRmTable_74_48[BVDC_P_RM_TABLE_SIZE]            = { BVDC_P_MAKE_RM(77584, 28125,    0, 0x5845FA, 0) }; /* 74.480640 Mhz for vesa 1280x720p @60Hz refresh rate */
static const uint32_t s_aulRmTable_74_48_Div_1001[BVDC_P_RM_TABLE_SIZE]   = { BVDC_P_MAKE_RM(47744, 17325,    0, 0x582F67, 0) }; /* 74.406234 Mhz for vesa 1280x720p @59.94Hz refresh rate */
static const uint32_t s_aulRmTable_148_5[BVDC_P_RM_TABLE_SIZE]            = { BVDC_P_MAKE_RM(   11,     2,    0, 0xB00000, 0) }; /* 148.5 Mhz for 1080p @60Hz refresh rate */
static const uint32_t s_aulRmTable_148_5_Div_1001[BVDC_P_RM_TABLE_SIZE]   = { BVDC_P_MAKE_RM( 5500,  1001,    0, 0xAFD2FD, 0) }; /* 148.5 Mhz for 1080p @59.94Hz refresh rate */
static const uint32_t s_aulRmTable_162[BVDC_P_RM_TABLE_SIZE]              = { BVDC_P_MAKE_RM(    6,     1,    0, 0xc00000, 0) }; /* 162 Mhz for 1600x1200p @60Hz refresh rate */
static const uint32_t s_aulRmTable_67_565[BVDC_P_RM_TABLE_SIZE]           = { BVDC_P_MAKE_RM( 1564,   625,    0, 0x5013A9, 0xCD870) }; /* 67.565 Mhz sample rate, 1366x768_60Hz */
static const uint32_t s_aulRmTable_67_565_Div_1001[BVDC_P_RM_TABLE_SIZE]  = { BVDC_P_MAKE_RM(12512,  5005,    0, 0x4FFF2E, 0xCD870) }; /* 67.565/1.001 Mhz sample rate, 1366x768_5994Hz */
static const uint32_t s_aulRmTable_56_304[BVDC_P_RM_TABLE_SIZE]           = { BVDC_P_MAKE_RM(  782,   375,    0, 0x42BB0C, 0xCD870) }; /* 56.304 Mhz sample rate, 1366x768_50Hz */

static const uint32_t s_aulRmTable_23_75[BVDC_P_RM_TABLE_SIZE]            = { BVDC_P_MAKE_RM( 0x5F,   0xD,  0x1, 0x1C25ED, 0) }; /* 23.75 Mhz sample rate, VESA 640x480p_CVT @60Hz */
static const uint32_t s_aulRmTable_23_75_Div_1001[BVDC_P_RM_TABLE_SIZE]   = { BVDC_P_MAKE_RM( 0x5F,   0xD,  0x1, 0x1C25ED, 0) }; /* 23.75/1.001 Mhz sample rate, VESA 640x480p_CVT @60Hz */
static const uint32_t s_aulRmTable_83_5[BVDC_P_RM_TABLE_SIZE]             = { BVDC_P_MAKE_RM( 0xA7,  0x36,  0x0, 0x62F684, 0) }; /* 83.5 Mhz sample rate, VESA 1280x800_60Hz */
static const uint32_t s_aulRmTable_83_5_Div_1001[BVDC_P_RM_TABLE_SIZE]    = { BVDC_P_MAKE_RM( 0xA7,  0x36,  0x0, 0x62F684, 0) }; /* 83.5/1.001 Mhz sample rate, VESA 1280x800_60Hz */
static const uint32_t s_aulRmTable_108[BVDC_P_RM_TABLE_SIZE]              = { BVDC_P_MAKE_RM(  0x4,   0x1,  0x0, 0x800000, 0) }; /* 108 Mhz sample rate, VESA 1280x1024_60Hz */
static const uint32_t s_aulRmTable_108_Div_1001[BVDC_P_RM_TABLE_SIZE]     = { BVDC_P_MAKE_RM(  0x4,   0x1,  0x0, 0x800000, 0) }; /* 108/1.001 Mhz sample rate, VESA 1280x1024_60Hz */
static const uint32_t s_aulRmTable_85_5[BVDC_P_RM_TABLE_SIZE]             = { BVDC_P_MAKE_RM( 0x13,   0x6,  0x0, 0x655555, 0) }; /* 85.5 Mhz sample rate, VESA 1366x768_60Hz */
static const uint32_t s_aulRmTable_85_5_Div_1001[BVDC_P_RM_TABLE_SIZE]    = { BVDC_P_MAKE_RM( 0x13,   0x6,  0x0, 0x655555, 0) }; /* 85.5/1.001 Mhz sample rate, VESA 1366x768_60Hz */
static const uint32_t s_aulRmTable_106_5[BVDC_P_RM_TABLE_SIZE]            = { BVDC_P_MAKE_RM( 0x47,  0x12,  0x0, 0x7E38E3, 0) }; /* 106.5 Mhz sample rate, VESA 1440x900_60Hz */
static const uint32_t s_aulRmTable_106_5_Div_1001[BVDC_P_RM_TABLE_SIZE]   = { BVDC_P_MAKE_RM( 0x47,  0x12,  0x0, 0x7E38E3, 0) }; /* 106.5/1.001 Mhz sample rate, VESA 1440x900_60Hz */
static const uint32_t s_aulRmTable_54[BVDC_P_RM_TABLE_SIZE]               = { BVDC_P_MAKE_RM(    2,     1,    0, 0x400000, 0) }; /* 54Mhz for (480P or 576P) at double rate */
static const uint32_t s_aulRmTable_54_Mul_1001[BVDC_P_RM_TABLE_SIZE]               = { BVDC_P_MAKE_RM(0x3e9, 0x1f4,    0, 0x401062, 0) }; /* 54Mhz for 480P at double rate */

/* NTSC values for IT_ADDR_0_3 => IT_PCL_8 */
const uint32_t BVDC_P_aulItTable_NTSC[BVDC_P_IT_TABLE_SIZE] =
{
	BVDC_P_MAKE_IT_ADDR(0xfd, 0xfd, 0x14, 0, 0xfd, 0xfd, 0xfd),
	BVDC_P_MAKE_IT_STACK(0, 0, 0, 0, 0, 0, 0, 0),
	BVDC_P_MAKE_IT_EVENT(0, 0, 0, 0, 0, 6),
	BVDC_P_MAKE_IT_PCL_0(3, ZERO, ZERO, LINE_A, LINE_B, LINE_C, 0, 3, 0, 2),
	BVDC_P_MAKE_IT_PCL_1(LINE_C, ZERO, LINE_A),
	BVDC_P_MAKE_IT_PCL_2_3(ENABLED, 5, 5, DISABLED, ENABLED, 4, 5, 0),
	BVDC_P_MAKE_IT_PCL_4(ZERO, ZERO, 0, 3, 0, 3, ZERO, ZERO, ZERO, 0, 0, 3, 0),
#if (BVDC_P_SUPPORT_IT_VER >= 1)
	0x0,
	0x0,
	0x0,
	0x0,
	0x0,
#endif
};

/* PAL values for IT_ADDR_0_3 => IT_PCL_8 */
const uint32_t BVDC_P_aulItTable_PAL[BVDC_P_IT_TABLE_SIZE] =
{
#ifdef BVDC_P_WSE_VER3
	BVDC_P_MAKE_IT_ADDR(0xfd, 0xfd,  30,   0, 0xfd, 0xfd, 0xfd),
	BVDC_P_MAKE_IT_STACK(0, 0, 0, 0, 0, 0, 0, 0),
#else
	BVDC_P_MAKE_IT_ADDR(0xfd, 0xfd, 0x14, 0, 0xfd, 0xfd, 0xfd),
	BVDC_P_MAKE_IT_STACK(0, 0, 0, 0, 0, 0, 0xFFFF, 0),
#endif
	BVDC_P_MAKE_IT_EVENT(0, 0, 0, 0, 0, 6),
	BVDC_P_MAKE_IT_PCL_0(3, ZERO, ZERO, LINE_A, LINE_B, LINE_C, 0, 3, 0, 2),
	BVDC_P_MAKE_IT_PCL_1(LINE_C, ZERO, LINE_A),
	BVDC_P_MAKE_IT_PCL_2_3(ENABLED, 5, 5, DISABLED, ENABLED, 4, 5, 0),
	BVDC_P_MAKE_IT_PCL_4(ZERO, ZERO, 0, 3, 0, 3, ZERO, ZERO, ZERO, 0, 0, 3, 0),
#if (BVDC_P_SUPPORT_IT_VER >= 1)
	0x0,
	0x0,
	0x0,
	0x0,
	0x0,
#endif
};

/* SECAM values for IT_ADDR_0_3 => IT_PCL_8
 */
const uint32_t BVDC_P_aulItTable_SECAM[BVDC_P_IT_TABLE_SIZE] =
{
#ifdef BVDC_P_WSE_VER3
	BVDC_P_MAKE_IT_ADDR(253, 253, 30, 0, 60, 253, 253),
#else
	BVDC_P_MAKE_IT_ADDR(253, 253, 20, 0, 50, 253, 253),
#endif
	BVDC_P_MAKE_IT_STACK(0, 0, 0, 0, 0, 0, 0, 0),
	BVDC_P_MAKE_IT_EVENT(6, 0, 0, 0, 0, 6),
	BVDC_P_MAKE_IT_PCL_0(3, ZERO, LINE_D, LINE_A, LINE_B, LINE_C, 0, 3, 0, 2),
	BVDC_P_MAKE_IT_PCL_1(LINE_C, ZERO, ONE),
	BVDC_P_MAKE_IT_PCL_2_3(ENABLED, 5, 5, DISABLED, ENABLED, 6, 7, 0),
	BVDC_P_MAKE_IT_PCL_4(ZERO, LINE_A, 0, 3, 0, 3, ZERO, ZERO, ZERO, 0, 0, 3, 0),
#if (BVDC_P_SUPPORT_IT_VER >= 1)
	0x0,
	0x0,
	0x0,
	0x0,
	0x0,
#endif
};

/* IT_ADDR_0_3 => IT_PCL_8 for 1080p */
static const uint32_t s_aulItTable_1080p[BVDC_P_IT_TABLE_SIZE] =
{
	BVDC_P_MAKE_IT_ADDR(0xfd, 0x32, 0x14, 0, 0xfd, 0xfd, 0xfd),
	BVDC_P_MAKE_IT_STACK(0, 0, 0, 0, 0, 0, 0, 0),
	BVDC_P_MAKE_IT_EVENT(0, 0, 0, 0, 0, 4),
	BVDC_P_MAKE_IT_PCL_0(3, ZERO, LINE_D, LINE_A, LINE_B, LINE_C, 0, 3, 0, 2),
	BVDC_P_MAKE_IT_PCL_1(LINE_C, ZERO, ZERO),

	BVDC_P_IT_FIELD_DATA(PCL_2, reserved0 ,                        0) |
	BVDC_P_IT_FIELD_DATA(PCL_2, U_FLIP_MUX_0_SELECT ,              0) |
	BVDC_P_IT_FIELD_DATA(PCL_2, U_FLIP_MUX_A_SELECT ,              1) |
	BVDC_P_IT_FIELD_ENUM(PCL_2, U_FLIP_ENABLE,               ENABLED) |
	BVDC_P_IT_FIELD_DATA(PCL_2, V_FLIP_MUX_0_SELECT ,              2) |
	BVDC_P_IT_FIELD_DATA(PCL_2, V_FLIP_MUX_A_SELECT ,              0) |
	BVDC_P_IT_FIELD_ENUM(PCL_2, V_FLIP_ENABLE,              DISABLED) |
	BVDC_P_IT_FIELD_DATA(PCL_2, SEC_NEG_SYNC_MUX_0_SELECT,         3) |
	BVDC_P_IT_FIELD_DATA(PCL_2, SEC_NEG_SYNC_MUX_A_SELECT,         0) |
	BVDC_P_IT_FIELD_ENUM(PCL_2, SEC_NEG_SYNC_ENABLE,        DISABLED) |
	BVDC_P_IT_FIELD_DATA(PCL_2, EXT_HSYNC_MUX_SELECT,              0) |
	BVDC_P_IT_FIELD_ENUM(PCL_2, EXT_HSYNC_ENABLE,                 ON) |
	BVDC_P_IT_FIELD_DATA(PCL_2, EXT_VSYNC_MUX_SELECT,              0) |
	BVDC_P_IT_FIELD_ENUM(PCL_2, EXT_VSYNC_ENABLE,                 ON) ,

	BVDC_P_IT_FIELD_DATA(PCL_3, reserved0,                      0)    |
	BVDC_P_IT_FIELD_ENUM(PCL_3, LINE_COUNT_CLEAR_ENABLE,       ON)    |
	BVDC_P_IT_FIELD_DATA(PCL_3, LINE_COUNT_CLEAR_SELECT,        1)    |
	BVDC_P_IT_FIELD_ENUM(PCL_3, NEW_LINE_ENABLE,               ON)    |
	BVDC_P_IT_FIELD_DATA(PCL_3, NEW_LINE_MUX_SELECT,            5)    |
	BVDC_P_IT_FIELD_ENUM(PCL_3, V_ACTIVE_ENABLE,               ON)    |
	BVDC_P_IT_FIELD_DATA(PCL_3, V_ACTIVE_MUX_SELECT,            1)    |
	BVDC_P_IT_FIELD_ENUM(PCL_3, H_ACTIVE_ENABLE,               ON)    |
	BVDC_P_IT_FIELD_DATA(PCL_3, H_ACTIVE_MUX_SELECT,            1)    |
	BVDC_P_IT_FIELD_ENUM(PCL_3, ODD_EVEN_ENABLE,               ON)    |
	BVDC_P_IT_FIELD_DATA(PCL_3, ODD_EVEN_MUX_SELECT,            3)    |
	BVDC_P_IT_FIELD_ENUM(PCL_3, VSYNC_ENABLE,                  ON)    |
	BVDC_P_IT_FIELD_DATA(PCL_3, VSYNC_MUX_SELECT,               0)    |
	BVDC_P_IT_FIELD_ENUM(PCL_3, VBLANK_ENABLE,                 ON)    |
	BVDC_P_IT_FIELD_DATA(PCL_3, VBLANK_MUX_SELECT,              3)    ,

	BVDC_P_IT_FIELD_DATA(PCL_4, reserved0,                0) |
	BVDC_P_IT_FIELD_DATA(PCL_4, PSB_MUX_1_SELECT,         0) |
	BVDC_P_IT_FIELD_DATA(PCL_4, PSB_MUX_2_SELECT,         3) |
	BVDC_P_IT_FIELD_DATA(PCL_4, PSB_MUX_B_SELECT,         0) |
	BVDC_P_IT_FIELD_DATA(PCL_4, PSB_MUX_C_SELECT,         0) |
	BVDC_P_IT_FIELD_ENUM(PCL_4, PSB_AND_TERM_0,         ONE) |
	BVDC_P_IT_FIELD_ENUM(PCL_4, PSB_AND_TERM_1,        ZERO) |
	BVDC_P_IT_FIELD_ENUM(PCL_4, PSB_AND_TERM_2,        ZERO) |
	BVDC_P_IT_FIELD_DATA(PCL_4, PSA_MUX_0_SELECT,         3) |
	BVDC_P_IT_FIELD_DATA(PCL_4, PSA_MUX_1_SELECT,         0) |
	BVDC_P_IT_FIELD_DATA(PCL_4, PSA_MUX_A_SELECT,         3) |
	BVDC_P_IT_FIELD_DATA(PCL_4, PSA_MUX_B_SELECT,         0) |
	BVDC_P_IT_FIELD_ENUM(PCL_4, PSA_AND_TERM_0,      LINE_A) |
	BVDC_P_IT_FIELD_ENUM(PCL_4, PSA_AND_TERM_1,        ZERO) |
	BVDC_P_IT_FIELD_DATA(PCL_4, reserved1,                0),
#if (BVDC_P_SUPPORT_IT_VER >= 1)
	0x0,
	0x0,
	0x0,
	0x0,
	0x0,
#endif
};

/* IT_ADDR_0_3 => IT_PCL_8 */
static const uint32_t s_aulItTable_1080i[BVDC_P_IT_TABLE_SIZE] =
{
	BVDC_P_MAKE_IT_ADDR(0xfd, 0x32, 0x14, 0, 0xfd, 0xfd, 0xfd),
	BVDC_P_MAKE_IT_STACK(0, 0, 0, 0, 0, 0, 0, 0),
	BVDC_P_MAKE_IT_EVENT(0, 0, 0, 0, 0, 4),
	BVDC_P_MAKE_IT_PCL_0(3, ZERO, LINE_D, LINE_A, LINE_B, LINE_C, 0, 3, 0, 2),
	BVDC_P_MAKE_IT_PCL_1(LINE_C, ZERO, ZERO),
	BVDC_P_MAKE_IT_PCL_2_3(ENABLED, 5, 5, DISABLED, ENABLED, 4, 5, 5),
	BVDC_P_MAKE_IT_PCL_4(ZERO, LINE_A, 0, 3, 0, 3, ZERO, LINE_B, ONE, 0, 1, 3, 2),
#if (BVDC_P_SUPPORT_IT_VER >= 1)
	0x0,
	0x0,
	0x0,
	0x0,
	0x0,
#endif
};

/* IT_ADDR_0_3 => IT_PCL_8 */
/* Same as s_aulItTable_1080i except for TRIGGER_VALUE. */
static const uint32_t s_aulItTable_1125i_50hz[BVDC_P_IT_TABLE_SIZE] =
{
	BVDC_P_MAKE_IT_ADDR(0xfd, 0x32, 0x14, 0, 0xfd, 0xfd, 0xfd),
	BVDC_P_MAKE_IT_STACK(0, 0, 0, 0, 0, 0, 0, 0),
	BVDC_P_MAKE_IT_EVENT(0, 0, 0, 0, 0, 4),
	BVDC_P_MAKE_IT_PCL_0(3, ZERO, LINE_D, LINE_A, LINE_B, LINE_C, 0, 3, 0, 2),
	BVDC_P_MAKE_IT_PCL_1(LINE_C, ZERO, ZERO),
	BVDC_P_MAKE_IT_PCL_2_3(ENABLED, 5, 5, DISABLED, ENABLED, 4, 5, 5),
	BVDC_P_MAKE_IT_PCL_4(ZERO, LINE_A, 0, 3, 0, 3, ZERO, LINE_B, ONE, 0, 1, 3, 2),
#if (BVDC_P_SUPPORT_IT_VER >= 1)
	0x0,
	0x0,
	0x0,
	0x0,
	0x0,
#endif
};

static const uint32_t s_aulItTable_720p[BVDC_P_IT_TABLE_SIZE] =
{
	BVDC_P_MAKE_IT_ADDR(0xfd, 0x32, 0x14, 0, 0xfd, 0xfd, 0xfd),
	BVDC_P_MAKE_IT_STACK(0, 0, 0, 0, 0, 0, 0, 0),
	BVDC_P_MAKE_IT_EVENT(0, 0, 0, 0, 0, 4),
	BVDC_P_MAKE_IT_PCL_0(3, ZERO, ZERO, LINE_A, LINE_B, LINE_C, 0, 3, 0, 3),
	BVDC_P_MAKE_IT_PCL_1(LINE_C, ZERO, ZERO),
	BVDC_P_MAKE_IT_PCL_2_3(ENABLED, 5, 5, DISABLED, ENABLED, 4, 5, 5),
	BVDC_P_MAKE_IT_PCL_4(ONE, LINE_A, 0, 3, 0, 3, ZERO, ONE, ZERO, 0, 0, 3, 2),
#if (BVDC_P_SUPPORT_IT_VER >= 1)
	0x0,
	0x0,
	0x0,
	0x0,
	0x0,
#endif
};

static const uint32_t  s_aulItTable_720p_24hz[BVDC_P_IT_TABLE_SIZE] =
{
	BVDC_P_MAKE_IT_ADDR(0xfd, 0x32, 0x14, 0, 0xfd, 0xfd, 0xfd),
	BVDC_P_MAKE_IT_STACK(0, 0, 0, 0, 0, 0, 0, 0),
	BVDC_P_MAKE_IT_EVENT(0, 0, 0, 0, 0, 4),
	BVDC_P_MAKE_IT_PCL_0(0, ZERO, LINE_D, LINE_A, LINE_B, LINE_C, 0, 3, 0, 3),
	BVDC_P_MAKE_IT_PCL_1(LINE_C, ZERO, ZERO),
	BVDC_P_MAKE_IT_PCL_2_3(ENABLED, 5, 5, DISABLED, ENABLED, 4, 5, 5),
	BVDC_P_MAKE_IT_PCL_4(ZERO, LINE_A, 0, 3, 0, 3, ZERO, ONE, ZERO, 0, 0, 3, 2),
#if (BVDC_P_SUPPORT_IT_VER >= 1)
	0x0,
	0x0,
	0x0,
	0x0,
	0x0,
#endif
};

static const uint32_t  s_aulItTable_720p_30hz[BVDC_P_IT_TABLE_SIZE] =
{
	BVDC_P_MAKE_IT_ADDR(0xfd, 0x32, 0x14, 0, 0xfd, 0xfd, 0xfd),
	BVDC_P_MAKE_IT_STACK(0, 0, 0, 0, 0, 0, 0, 0),
	BVDC_P_MAKE_IT_EVENT(0, 0, 0, 0, 0, 4),
	BVDC_P_MAKE_IT_PCL_0(0, ZERO, LINE_D, LINE_A, LINE_B, LINE_C, 0, 3, 0, 3),
	BVDC_P_MAKE_IT_PCL_1(LINE_C, ZERO, ZERO),
	BVDC_P_MAKE_IT_PCL_2_3(ENABLED, 5, 5, DISABLED, ENABLED, 4, 5, 5),
	BVDC_P_MAKE_IT_PCL_4(ZERO, LINE_A, 0, 3, 0, 3, ZERO, ONE, ZERO, 0, 0, 3, 2),
#if (BVDC_P_SUPPORT_IT_VER >= 1)
	0x0,
	0x0,
	0x0,
	0x0,
	0x0,
#endif
};

static const uint32_t s_aulItTable_720p_50hz[BVDC_P_IT_TABLE_SIZE] =
{
	BVDC_P_MAKE_IT_ADDR(0xfd, 0x32, 0x14, 0, 0xfd, 0xfd, 0xfd),
	BVDC_P_MAKE_IT_STACK(0, 0, 0, 0, 0, 0, 0, 0),
	BVDC_P_MAKE_IT_EVENT(0, 0, 0, 0, 0, 4),
	BVDC_P_MAKE_IT_PCL_0(3, ZERO, LINE_D, LINE_A, LINE_B, LINE_C, 0, 3, 0, 3),
	BVDC_P_MAKE_IT_PCL_1(LINE_C, ZERO, ZERO),
	BVDC_P_MAKE_IT_PCL_2_3(ENABLED, 5, 5, DISABLED, ENABLED, 4, 5, 5),
	BVDC_P_MAKE_IT_PCL_4(ZERO, LINE_A, 0, 3, 0, 3, ZERO, ONE, ZERO, 0, 0, 3, 2),
#if (BVDC_P_SUPPORT_IT_VER >= 1)
	0x0,
	0x0,
	0x0,
	0x0,
	0x0,
#endif
};

static const uint32_t s_aulItTable_3D[BVDC_P_IT_TABLE_SIZE] =
{
	BVDC_P_MAKE_IT_ADDR(0xfd, 0xfd, 0x14, 0, 0xfd, 0xfd, 0xfd),
	BVDC_P_MAKE_IT_STACK(0, 0, 0, 0, 0, 0, 0, 0),
	BVDC_P_MAKE_IT_EVENT(0, 0, 0, 0, 0, 6),
	BVDC_P_MAKE_IT_PCL_0(3, ZERO, LINE_D, LINE_A, LINE_B, LINE_C, 0, 3, 0, 3),
	BVDC_P_MAKE_IT_PCL_1(LINE_C, ZERO, LINE_A),
	BVDC_P_MAKE_IT_PCL_2_3(ENABLED, 5, 5, DISABLED, ENABLED, 4, 5, 0),
	BVDC_P_MAKE_IT_PCL_4(ZERO, LINE_A, 0, 3, 0, 3, ZERO, ONE, ZERO, 0, 0, 3, 2),
#if (BVDC_P_SUPPORT_IT_VER >= 1)
	0x0,
	0x0,
	0x0,
	0x0,
	0x0,
#endif
};

static const uint32_t s_aulItTable_480p[BVDC_P_IT_TABLE_SIZE] =
{
	BVDC_P_MAKE_IT_ADDR(0xfd, 0xfd, 0x14, 0, 0xfd, 0xfd, 0xfd),
	BVDC_P_MAKE_IT_STACK(0, 0, 0, 0, 0, 0, 0, 0),
	BVDC_P_MAKE_IT_EVENT(0, 0, 0, 0, 0, 6),
	BVDC_P_MAKE_IT_PCL_0(3, ZERO, ZERO, LINE_A, LINE_B, LINE_C, 0, 3, 0, 2),
	BVDC_P_MAKE_IT_PCL_1(LINE_C, ZERO, ZERO),
	BVDC_P_MAKE_IT_PCL_2_3(ENABLED, 5, 5, DISABLED, ENABLED, 4, 5, 0),
	BVDC_P_MAKE_IT_PCL_4(ZERO, LINE_A, 0, 3, 0, 3, ZERO, ZERO, ZERO, 0, 0, 3, 0),
#if (BVDC_P_SUPPORT_IT_VER >= 1)
	0x0,
	0x0,
	0x0,
	0x0,
	0x0,
#endif
};

static const uint32_t s_aulItTable_1280x720p[BVDC_P_IT_TABLE_SIZE] =
{
	BVDC_P_MAKE_IT_ADDR(0, 0, 0x14, 0, 0, 0, 0),
	BVDC_P_MAKE_IT_STACK(0, 0, 0, 0, 0, 0, 0, 0),
	BVDC_P_MAKE_IT_EVENT(0, 0, 0, 0, 0, 6),
	BVDC_P_MAKE_IT_PCL_0(0, ZERO, ZERO, LINE_A, LINE_B, LINE_C, 0, 3, 0, 2),
	BVDC_P_MAKE_IT_PCL_1(LINE_C, ZERO, LINE_A),
	BVDC_P_MAKE_IT_PCL_2_3(DISABLED, 0, 0, DISABLED, ENABLED, 4, 5, 0),
	BVDC_P_MAKE_IT_PCL_4(ZERO, LINE_A, 0, 3, 0, 3, ZERO, ZERO, ZERO, 0, 0, 3, 0),
#if (BVDC_P_SUPPORT_IT_VER >= 1)
	0x0,
	0x0,
	0x0,
	0x0,
	0x0,
#endif
};

/* Custom 1600x1200p */
static const uint32_t s_aulItTable_1600x1200p_60Hz[BVDC_P_IT_TABLE_SIZE] =
{
	BVDC_P_MAKE_IT_ADDR(0, 0, 0x14, 0, 0, 0, 0),
	BVDC_P_MAKE_IT_STACK(0, 0, 0, 0, 0, 0, 0, 0),
	BVDC_P_MAKE_IT_EVENT(0, 0, 0, 0, 0, 6),
	BVDC_P_MAKE_IT_PCL_0(0, ZERO, LINE_D, LINE_A, LINE_B, LINE_C, 0, 3, 0, 2),
	BVDC_P_MAKE_IT_PCL_1(LINE_C, ZERO, LINE_A),
	BVDC_P_MAKE_IT_PCL_2_3(ENABLED, 5, 5, DISABLED, ENABLED, 5, 2, 0),
	BVDC_P_MAKE_IT_PCL_4(ZERO, LINE_A, 0, 3, 0, 3, ZERO, ZERO, ZERO, 0, 0, 3, 0),
#if (BVDC_P_SUPPORT_IT_VER >= 1)
	0x0,
	0x0,
	0x0,
	0x0,
	0x0,
#endif
};

/* Custom 1366x768p */
static const uint32_t s_aulItTable_CUSTOM_1366x768p[BVDC_P_IT_TABLE_SIZE] =
{
	BVDC_P_MAKE_IT_ADDR(0, 0, 0x14, 0, 0, 0, 0),
	BVDC_P_MAKE_IT_STACK(0, 0, 0, 0, 0, 0, 0, 0),
	BVDC_P_MAKE_IT_EVENT(0, 0, 0, 0, 0, 6),
	BVDC_P_MAKE_IT_PCL_0(0, ZERO, LINE_D, LINE_A, LINE_B, LINE_C, 0, 3, 0, 2),
	BVDC_P_MAKE_IT_PCL_1(LINE_C, ZERO, LINE_A),
	BVDC_P_MAKE_IT_PCL_2_3(ENABLED, 5, 5, DISABLED, ENABLED, 4, 5, 0),
	BVDC_P_MAKE_IT_PCL_4(ZERO, LINE_A, 0, 3, 0, 3, ZERO, ZERO, ZERO, 0, 0, 3, 0),
#if (BVDC_P_SUPPORT_IT_VER >= 1)
	0x0,
	0x0,
	0x0,
	0x0,
	0x0,
#endif
};

/* Analog VESA */
static const uint32_t s_aulItTable_VESA[BVDC_P_IT_TABLE_SIZE] =
{
	BVDC_P_MAKE_IT_ADDR(0, 0, 0x14, 0, 0, 0, 0),
	BVDC_P_MAKE_IT_STACK(0, 0, 0, 0, 0, 0, 0, 0),
	BVDC_P_MAKE_IT_EVENT(0, 0, 0, 0, 0, 6),
	BVDC_P_MAKE_IT_PCL_0(0, ZERO, LINE_D, LINE_A, LINE_B, LINE_C, 0, 3, 0, 2),
	BVDC_P_MAKE_IT_PCL_1(LINE_C, ZERO, ZERO),
	BVDC_P_MAKE_IT_PCL_2_3(ENABLED, 5, 5, DISABLED, ENABLED, 4, 5, 0),
	BVDC_P_MAKE_IT_PCL_4(ZERO, LINE_A, 0, 3, 0, 3, ZERO, ZERO, ZERO, 0, 0, 3, 0),
#if (BVDC_P_SUPPORT_IT_VER >= 1)
	0x0,
	0x0,
	0x0,
	0x0,
	0x0,
#endif
};

#if BVDC_P_ORTHOGONAL_VEC /** { **/
#define BVDC_P_MAKE_SM(pixel_frac_en, init_phase, phase_offset, u_coring_en, \
	v_coring_en, fre0, chroma_offset, comp_out_sel, combine_chroma,          \
	sin_coef_en, cos_coef_en)                                                \
	BVDC_P_SM_FIELD_ENUM(PG_CNTRL, PIXEL_FRAC_ENABLE,      pixel_frac_en) |  \
	BVDC_P_SM_FIELD_DATA(PG_CNTRL, INIT_PHASE,                init_phase) |  \
	BVDC_P_SM_FIELD_DATA(PG_CNTRL, ACTIVE_PHASE_OFFSET,     phase_offset),   \
	BVDC_P_SM_FIELD_DATA(PG_CONFIG, U_CORING_ENABLE,         u_coring_en) |  \
	BVDC_P_SM_FIELD_DATA(PG_CONFIG, V_CORING_ENABLE,         v_coring_en),   \
	BVDC_P_SM_FIELD_DATA(SC_FREQ_0, FRE0,                           fre0),   \
	BVDC_P_SM_FIELD_DATA(COMP_CNTRL, CHROMA_OFFSET,        chroma_offset) |  \
	BVDC_P_SM_FIELD_ENUM(COMP_CNTRL, COMP_OUT_SEL,          comp_out_sel) |  \
	BVDC_P_SM_FIELD_ENUM(COMP_CNTRL, COMBINE_CHROMA,      combine_chroma) |  \
	BVDC_P_SM_FIELD_ENUM(COMP_CNTRL, SIN_COEF_EN,            sin_coef_en) |  \
	BVDC_P_SM_FIELD_ENUM(COMP_CNTRL, COS_COEF_EN,            cos_coef_en)

/* SM values for NTSC SVideo/CVBS */
static const uint32_t s_aulSmTable_YQI[BVDC_P_SM_TABLE_SIZE] =
{
#if DCS_SUPPORT
	BVDC_P_MAKE_SM(
		ON, 0x86,  0x005e, 1, 1, 0x21f07c1f, 0x400, COMPOSITE, ON, USE_SIN,
		USE_COS),
#else
	BVDC_P_MAKE_SM(
		ON, 0x3f0, 0x005e, 1, 1, 0x21f07c1f, 0x400, COMPOSITE, ON, USE_SIN,
		USE_COS),
#endif
};

/* PAL_M SVideo/CVBS */
static const uint32_t s_aulSmTable_PALM_YUV[BVDC_P_SM_TABLE_SIZE] =
{
	BVDC_P_MAKE_SM(ON, 0x250, 0x0, 1, 1, 0x21E6EFA4, 0x400, COMPOSITE, ON, USE_SIN, USE_COS),
};

/* PAL_NC SVideo/CVBS */
static const uint32_t s_aulSmTable_PALNC_YUV[BVDC_P_SM_TABLE_SIZE] =
{
	BVDC_P_MAKE_SM(ON, 0x3A4, 0x0, 1, 1, 0x21F69447, 0x400, COMPOSITE, ON, USE_SIN, USE_COS),
};

/* PAL_N SVideo/CVBS */
static const uint32_t s_aulSmTable_PALN_YUV[BVDC_P_SM_TABLE_SIZE] =
{
	BVDC_P_MAKE_SM(
		ON, 0x3A4, 0x0, 1, 1, 0x2A098ACB, 0x400, COMPOSITE, ON, USE_SIN,
		USE_COS),
};

/* All other PAL SVideo/CVBS */
static const uint32_t s_aulSmTable_PAL_YUV[BVDC_P_SM_TABLE_SIZE] =
{
#if DCS_SUPPORT
	BVDC_P_MAKE_SM(
		ON, 0x1D,  0x0, 1, 1, 0x2A098ACB, 0x400, COMPOSITE, ON, USE_SIN,
		USE_COS),
#else
	BVDC_P_MAKE_SM(
		ON, 0x3A4, 0x0, 1, 1, 0x2A098ACB, 0x400, COMPOSITE, ON, USE_SIN,
		USE_COS),
#endif
};

/* SECAM SVideo/CVBS */
static const uint32_t s_aulSmTable_SECAM[BVDC_P_SM_TABLE_SIZE] =
{
	BVDC_P_MAKE_SM(ON, 0, 0x0, 0, 0, 0x284BDA12, 0x400, COMPOSITE, ON, USE_SIN, USE_COS),
};

/* SM values for Component output */
static const uint32_t s_aulSmTable_Component[BVDC_P_SM_TABLE_SIZE] =
{
	BVDC_P_MAKE_SM(OFF, 0, 0x0, 0, 0, 0, 0, COMPONENT, OFF, USE_ONE, USE_ONE),
};

#else /** } { not orthogonal VEC **/
#define BVDC_P_MAKE_SM(amp_cntl, amp_clamp, u_burst, v_burst, u_en, v_en,      \
	init_amp, pixel_frac_en, init_phase, phase_offset, u_coring_en,            \
	v_coring_en, fre0, chroma_offset, comp_out_sel, combine_chroma,            \
	sin_coef_en, cos_coef_en)                                                  \
	BVDC_P_SM_FIELD_ENUM(ENVELOPE_GENERATOR, AMP_CONTROL,       amp_cntl) | \
	BVDC_P_SM_FIELD_DATA(ENVELOPE_GENERATOR, AMP_CLAMP,        amp_clamp) | \
	BVDC_P_SM_FIELD_ENUM(ENVELOPE_GENERATOR, U_BURST_AMP_SEL,    u_burst) | \
	BVDC_P_SM_FIELD_ENUM(ENVELOPE_GENERATOR, V_BURST_AMP_SEL,    v_burst) | \
	BVDC_P_SM_FIELD_ENUM(ENVELOPE_GENERATOR, U_ENABLE,              u_en) | \
	BVDC_P_SM_FIELD_ENUM(ENVELOPE_GENERATOR, V_ENABLE,              v_en) | \
	BVDC_P_SM_FIELD_DATA(ENVELOPE_GENERATOR, INIT_AMP,          init_amp),  \
	BVDC_P_SM_FIELD_ENUM(PG_CNTRL, PIXEL_FRAC_ENABLE,      pixel_frac_en) | \
	BVDC_P_SM_FIELD_DATA(PG_CNTRL, INIT_PHASE,                init_phase) | \
	BVDC_P_SM_FIELD_DATA(PG_CNTRL, ACTIVE_PHASE_OFFSET,     phase_offset),  \
	BVDC_P_SM_FIELD_DATA(PG_CONFIG, U_CORING_ENABLE,         u_coring_en) | \
	BVDC_P_SM_FIELD_DATA(PG_CONFIG, V_CORING_ENABLE,         v_coring_en),  \
	BVDC_P_SM_FIELD_DATA(SC_FREQ_0, FRE0,                           fre0),  \
	BVDC_P_SM_FIELD_DATA(COMP_CNTRL, CHROMA_OFFSET,        chroma_offset) | \
	BVDC_P_SM_FIELD_ENUM(COMP_CNTRL, COMP_OUT_SEL,          comp_out_sel) | \
	BVDC_P_SM_FIELD_ENUM(COMP_CNTRL, COMBINE_CHROMA,      combine_chroma) | \
	BVDC_P_SM_FIELD_ENUM(COMP_CNTRL, SIN_COEF_EN,            sin_coef_en) | \
	BVDC_P_SM_FIELD_ENUM(COMP_CNTRL, COS_COEF_EN,            cos_coef_en)

/* SM values for NTSC SVideo/CVBS */
static const uint32_t s_aulSmTable_YQI[BVDC_P_SM_TABLE_SIZE] =
{
	BVDC_P_MAKE_SM(CONSTANT, 0, NEGATIVE, NEGATIVE, ON, OFF, 0x70, ON, 0x3f0, 0x005e, 1, 1, 0x21f07c1f, 0x400, COMPOSITE, ON, USE_SIN, USE_COS),
};

/* PAL, PAL_N SVideo/CVBS */
static const uint32_t s_aulSmTable_PAL_YUV[BVDC_P_SM_TABLE_SIZE] =
{
	BVDC_P_MAKE_SM(CONSTANT, 0, NEGATIVE, POSITIVE, ON, ON, 0x54, ON, 0x102, 0x0, 1, 1, 0x2A098ACB, 0x400, COMPOSITE, ON, USE_SIN, USE_COS),
};
#define s_aulSmTable_PALN_YUV s_aulSmTable_PAL_YUV

/* PAL_NC SVideo/CVBS */
static const uint32_t s_aulSmTable_PALNC_YUV[BVDC_P_SM_TABLE_SIZE] =
{
	BVDC_P_MAKE_SM(CONSTANT, 0, NEGATIVE, POSITIVE, ON, ON, 0x54, ON, 0x3A4, 0x0, 1, 1, 0x21F69447, 0x400, COMPOSITE, ON, USE_SIN, USE_COS),
};

/* PAL_M SVideo/CVBS */
static const uint32_t s_aulSmTable_PALM_YUV[BVDC_P_SM_TABLE_SIZE] =
{
	BVDC_P_MAKE_SM(CONSTANT, 0, NEGATIVE, POSITIVE, ON, ON, 0x58, ON, 0x250, 0x0, 1, 1, 0x21E6EFA4, 0x400, COMPOSITE, ON, USE_SIN, USE_COS),
};

/* SECAM SVideo/CVBS */
static const uint32_t s_aulSmTable_SECAM[BVDC_P_SM_TABLE_SIZE] =
{
	BVDC_P_MAKE_SM(INC_BY_ONE, 424, NEGATIVE, NEGATIVE, ON, OFF, 0x78, ON, 0, 0x0, 0, 0, 0x284BDA12, 0x400, COMPOSITE, ON, USE_SIN, USE_COS),
};

/* SM values for Component output */
static const uint32_t s_aulSmTable_Component[BVDC_P_SM_TABLE_SIZE] =
{
	BVDC_P_MAKE_SM(CONSTANT, 0, NEGATIVE, NEGATIVE, OFF, OFF, 0, OFF, 0, 0x0, 0, 0, 0, 0, COMPONENT, OFF, USE_ONE, USE_ONE),
};

#endif /** } orthogonal VEC **/

/****************************************************************
 *  Format Tables
 ****************************************************************/
const BVDC_P_FormatInfo BVDC_P_aFormatInfoTable[] =
{
    /* Format                                HD     ED     SD     VESA   Prog   MacVin DCS    HDMI   Use
                                                                                                     DropTbl */
	{BFMT_VideoFmt_eNTSC,                    false, false, true,  false, false, true,  true,  true,  true },
	{BFMT_VideoFmt_eNTSC_J,                  false, false, true,  false, false, true,  true,  true,  true },
	{BFMT_VideoFmt_eNTSC_443,                false, false, true,  false, false, false, false, true,  true },
	{BFMT_VideoFmt_ePAL_B,                   false, false, true,  false, false, true,  true,  true,  false},
	{BFMT_VideoFmt_ePAL_B1,                  false, false, true,  false, false, true,  true,  true,  false},
	{BFMT_VideoFmt_ePAL_D,                   false, false, true,  false, false, true,  true,  true,  false},
	{BFMT_VideoFmt_ePAL_D1,                  false, false, true,  false, false, true,  true,  true,  false},
	{BFMT_VideoFmt_ePAL_G,                   false, false, true,  false, false, true,  true,  true,  false},
	{BFMT_VideoFmt_ePAL_H,                   false, false, true,  false, false, true,  true,  true,  false},
	{BFMT_VideoFmt_ePAL_K,                   false, false, true,  false, false, true,  true,  true,  false},
	{BFMT_VideoFmt_ePAL_I,                   false, false, true,  false, false, true,  true,  true,  false},
	{BFMT_VideoFmt_ePAL_M,                   false, false, true,  false, false, true,  false, true,  false},
	{BFMT_VideoFmt_ePAL_N,                   false, false, true,  false, false, true,  false, true,  false},
	{BFMT_VideoFmt_ePAL_NC,                  false, false, true,  false, false, true,  true,  true,  false},
	{BFMT_VideoFmt_ePAL_60,                  false, false, true,  false, false, false, false, true,  true,},
	{BFMT_VideoFmt_eSECAM_L,                 false, false, true,  false, false, true,  false, true,  false},
	{BFMT_VideoFmt_eSECAM_B,                 false, false, true,  false, false, true,  false, true,  false},
	{BFMT_VideoFmt_eSECAM_G,                 false, false, true,  false, false, true,  false, true,  false},
	{BFMT_VideoFmt_eSECAM_D,                 false, false, true,  false, false, true,  false, true,  false},
	{BFMT_VideoFmt_eSECAM_K,                 false, false, true,  false, false, true,  false, true,  false},
	{BFMT_VideoFmt_eSECAM_H,                 false, false, true,  false, false, true,  false, true,  false},
	{BFMT_VideoFmt_e1080i,                   true,  false, false, false, false, false, false, true,  false},
	{BFMT_VideoFmt_e1080p,                   true,  false, false, false, true,  false, false, true,  false},
	{BFMT_VideoFmt_e720p,                    true,  false, false, false, true,  false, false, true,  false},
	{BFMT_VideoFmt_e720p_60Hz_3DOU_AS,       true,  false, false, false, true,  false, false, true,  false},
	{BFMT_VideoFmt_e720p_50Hz_3DOU_AS,       true,  false, false, false, true,  false, false, true,  false},
	{BFMT_VideoFmt_e720p_30Hz_3DOU_AS,       true,  false, false, false, true,  false, false, true,  false},
	{BFMT_VideoFmt_e720p_24Hz_3DOU_AS,       true,  false, false, false, true,  false, false, true,  false},
	{BFMT_VideoFmt_e480p,                    false, true,  false, false, true,  true,  true,  true,  true },
	{BFMT_VideoFmt_e1080i_50Hz,              true,  false, false, false, false, false, false, true,  false},
	{BFMT_VideoFmt_e1080p_24Hz_3DOU_AS,      true,  false, false, false, true,  false, false, true,  false},
	{BFMT_VideoFmt_e1080p_30Hz_3DOU_AS,      true,  false, false, false, true,  false, false, true,  false},
	{BFMT_VideoFmt_e1080p_60Hz_3DOU_AS,      true,  false, false, false, true,  false, false, true,  false},
	{BFMT_VideoFmt_e1080p_60Hz_3DLR,         true,  false, false, false, true,  false, false, true,  false},

	{BFMT_VideoFmt_e1080p_24Hz,              true,  false, false, false, true,  false, false, true,  false},
	{BFMT_VideoFmt_e1080p_25Hz,              true,  false, false, false, true,  false, false, true,  false},
	{BFMT_VideoFmt_e1080p_30Hz,              true,  false, false, false, true,  false, false, true,  false},
	{BFMT_VideoFmt_e1080p_50Hz,              true,  false, false, false, true,  false, false, true,  false},
	{BFMT_VideoFmt_e1250i_50Hz,              true,  false, false, false, false, false, false, false, false},
	{BFMT_VideoFmt_e720p_24Hz,               true,  false, false, false, true,  false, false, true,  false},
	{BFMT_VideoFmt_e720p_25Hz,               true,  false, false, false, true,  false, false, true,  false},
	{BFMT_VideoFmt_e720p_30Hz,               true,  false, false, false, true,  false, false, true,  false},
	{BFMT_VideoFmt_e720p_50Hz,               true,  false, false, false, true,  false, false, true,  false},
	{BFMT_VideoFmt_e576p_50Hz,               false, true,  false, false, true,  true,  true,  true,  false},
	{BFMT_VideoFmt_e240p_60Hz,               false, false, true,  false, true,  false, false, false, false},
	{BFMT_VideoFmt_e288p_50Hz,               false, false, true,  false, true,  false, false, false, false},
	{BFMT_VideoFmt_e1440x480p_60Hz,          false, false, true,  false, true,  false, false, false, false},
	{BFMT_VideoFmt_e1440x576p_50Hz,          false, false, true,  false, true,  false, false, false, false},
	{BFMT_VideoFmt_e3840x2160p_24Hz,         true,  false, false, false, true,  false, false, true,  false},
	{BFMT_VideoFmt_e4096x2160p_24Hz,         true,  false, false, false, true,  false, false, true,  false},
#if BFMT_LEGACY_3DTV_SUPPORT
	{BFMT_VideoFmt_eCUSTOM1920x2160i_48Hz,   true,  false, false, false, false, false, false, true,  false},
	{BFMT_VideoFmt_eCUSTOM1920x2160i_60Hz,   true,  false, false, false, false, false, false, true,  false},
#endif
	{BFMT_VideoFmt_eCUSTOM_1440x240p_60Hz,   false, false, true,  false, true,  false, false, false, false},
	{BFMT_VideoFmt_eCUSTOM_1440x288p_50Hz,   false, false, true,  false, true,  false, false, false, false},
	{BFMT_VideoFmt_eCUSTOM_1366x768p,        true,  false, false, false, true,  false, false, true,  false},
	{BFMT_VideoFmt_eCUSTOM_1366x768p_50Hz,   true,  false, false, false, true,  false, false, true,  false},
	{BFMT_VideoFmt_eDVI_640x480p,            true,  false, false, true,  true,  false, false, true,  false},
	{BFMT_VideoFmt_eDVI_640x480p_CVT,        true,  false, false, true,  true,  false, false, true,  false},

	{BFMT_VideoFmt_eDVI_800x600p,            true,  false, false, true,  true,  false, false, true,  false},
	{BFMT_VideoFmt_eDVI_1024x768p,           true,  false, false, true,  true,  false, false, true,  false},
	{BFMT_VideoFmt_eDVI_1280x768p,           true,  false, false, true,  true,  false, false, true,  false},
	{BFMT_VideoFmt_eDVI_1280x768p_Red,       true,  false, false, true,  true,  false, false, true,  false},
	{BFMT_VideoFmt_eDVI_1280x720p_50Hz,      true,  false, false, true,  true,  false, false, true,  false},
	{BFMT_VideoFmt_eDVI_1280x720p,           true,  false, false, true,  true,  false, false, true,  false},
	{BFMT_VideoFmt_eDVI_1280x720p_Red,       true,  false, false, true,  true,  false, false, true,  false},

	/* PC input format! */
	{BFMT_VideoFmt_eDVI_640x350p_60Hz,       true,  false, false, true,  true,  false, false, false, false},
	{BFMT_VideoFmt_eDVI_640x350p_70Hz,       true,  false, false, true,  true,  false, false, false, false},
	{BFMT_VideoFmt_eDVI_640x350p_72Hz,       true,  false, false, true,  true,  false, false, false, false},
	{BFMT_VideoFmt_eDVI_640x350p_75Hz,       true,  false, false, true,  true,  false, false, false, false},
	{BFMT_VideoFmt_eDVI_640x350p_85Hz,       true,  false, false, true,  true,  false, false, false, false},

	{BFMT_VideoFmt_eDVI_640x400p_60Hz,       true,  false, false, true,  true,  false, false, false, false},
	{BFMT_VideoFmt_eDVI_640x400p_70Hz,       true,  false, false, true,  true,  false, false, false, false},
	{BFMT_VideoFmt_eDVI_640x400p_72Hz,       true,  false, false, true,  true,  false, false, false, false},
	{BFMT_VideoFmt_eDVI_640x400p_75Hz,       true,  false, false, true,  true,  false, false, false, false},
	{BFMT_VideoFmt_eDVI_640x400p_85Hz,       true,  false, false, true,  true,  false, false, false, false},

	{BFMT_VideoFmt_eDVI_640x480p_66Hz,       true,  false, false, true,  true,  false, false, false, false},
	{BFMT_VideoFmt_eDVI_640x480p_70Hz,       true,  false, false, true,  true,  false, false, false, false},
	{BFMT_VideoFmt_eDVI_640x480p_72Hz,       true,  false, false, true,  true,  false, false, false, false},
	{BFMT_VideoFmt_eDVI_640x480p_75Hz,       true,  false, false, true,  true,  false, false, false, false},
	{BFMT_VideoFmt_eDVI_640x480p_85Hz,       true,  false, false, true,  true,  false, false, false, false},

	{BFMT_VideoFmt_eDVI_720x400p_60Hz,       true,  false, false, true,  true,  false, false, false, false},
	{BFMT_VideoFmt_eDVI_720x400p_70Hz,       true,  false, false, true,  true,  false, false, false, false},
	{BFMT_VideoFmt_eDVI_720x400p_72Hz,       true,  false, false, true,  true,  false, false, false, false},
	{BFMT_VideoFmt_eDVI_720x400p_75Hz,       true,  false, false, true,  true,  false, false, false, false},
	{BFMT_VideoFmt_eDVI_720x400p_85Hz,       true,  false, false, true,  true,  false, false, false, false},

	{BFMT_VideoFmt_eDVI_800x600p_56Hz,       true,  false, false, true,  true,  false, false, false, false},
	{BFMT_VideoFmt_eDVI_800x600p_59Hz_Red,   true,  false, false, true,  true,  false, false, false, false},
	{BFMT_VideoFmt_eDVI_800x600p_70Hz,       true,  false, false, true,  true,  false, false, false, false},
	{BFMT_VideoFmt_eDVI_800x600p_72Hz,       true,  false, false, true,  true,  false, false, false, false},
	{BFMT_VideoFmt_eDVI_800x600p_75Hz,       true,  false, false, true,  true,  false, false, false, false},
	{BFMT_VideoFmt_eDVI_800x600p_85Hz,       true,  false, false, true,  true,  false, false, false, false},

	{BFMT_VideoFmt_eDVI_1024x768p_66Hz,      true,  false, false, true,  true,  false, false, false, false},
	{BFMT_VideoFmt_eDVI_1024x768p_70Hz,      true,  false, false, true,  true,  false, false, false, false},
	{BFMT_VideoFmt_eDVI_1024x768p_72Hz,      true,  false, false, true,  true,  false, false, false, false},
	{BFMT_VideoFmt_eDVI_1024x768p_75Hz,      true,  false, false, true,  true,  false, false, false, false},
	{BFMT_VideoFmt_eDVI_1024x768p_85Hz,      true,  false, false, true,  true,  false, false, false, false},

	{BFMT_VideoFmt_eDVI_1280x720p_70Hz,      true,  false, false, true,  true,  false, false, false, false},
	{BFMT_VideoFmt_eDVI_1280x720p_72Hz,      true,  false, false, true,  true,  false, false, false, false},
	{BFMT_VideoFmt_eDVI_1280x720p_75Hz,      true,  false, false, true,  true,  false, false, false, false},
	{BFMT_VideoFmt_eDVI_1280x720p_85Hz,      true,  false, false, true,  true,  false, false, false, false},

	/* New DVI or PC vdec input support */
	{BFMT_VideoFmt_eDVI_1024x768i_87Hz,      true,  false, false, true,  true,  false, false, false, false},
	{BFMT_VideoFmt_eDVI_1152x864p_75Hz,      true,  false, false, true,  true,  false, false, false, false},
	{BFMT_VideoFmt_eDVI_1280x768p_75Hz,      true,  false, false, true,  true,  false, false, false, false},
	{BFMT_VideoFmt_eDVI_1280x768p_85Hz,      true,  false, false, true,  true,  false, false, false, false},
	{BFMT_VideoFmt_eDVI_1280x800p_60Hz,      true,  false, false, true,  true,  false, false, true,  false},
	{BFMT_VideoFmt_eDVI_1280x960p_60Hz,      true,  false, false, true,  true,  false, false, false, false},
	{BFMT_VideoFmt_eDVI_1280x960p_85Hz,      true,  false, false, true,  true,  false, false, false, false},
	{BFMT_VideoFmt_eDVI_1280x1024p_60Hz,     true,  false, false, true,  true,  false, false, true, false},
	{BFMT_VideoFmt_eDVI_1280x1024p_69Hz,     true,  false, false, true,  true,  false, false, false, false},
	{BFMT_VideoFmt_eDVI_1280x1024p_75Hz,     true,  false, false, true,  true,  false, false, false, false},
	{BFMT_VideoFmt_eDVI_1280x1024p_85Hz,     true,  false, false, true,  true,  false, false, false, false},
	{BFMT_VideoFmt_eDVI_832x624p_75Hz,       true,  false, false, true,  true,  false, false, false, false},
	{BFMT_VideoFmt_eDVI_1360x768p_60Hz,      true,  false, false, true,  true,  false, false, true,  false},
	{BFMT_VideoFmt_eDVI_1366x768p_60Hz,      true,  false, false, true,  true,  false, false, false, false},
	{BFMT_VideoFmt_eDVI_1400x1050p_60Hz_Red, true,  false, false, true,  true,  false, false, false, false},
	{BFMT_VideoFmt_eDVI_1400x1050p_60Hz,     true,  false, false, true,  true,  false, false, false, false},
	{BFMT_VideoFmt_eDVI_1400x1050p_75Hz,     true,  false, false, true,  true,  false, false, false, false},
	{BFMT_VideoFmt_eDVI_1600x1200p_60Hz,     true,  false, false, true,  true,  false, false, false, false},
	{BFMT_VideoFmt_eDVI_1920x1080p_60Hz_Red, true,  false, false, true,  true,  false, false, false, false},
	{BFMT_VideoFmt_eDVI_848x480p_60Hz,       true,  false, false, true,  true,  false, false, false, false},
	{BFMT_VideoFmt_eDVI_1064x600p_60Hz,      true,  false, false, true,  true,  false, false, false, false},
	{BFMT_VideoFmt_eDVI_1440x900p_60Hz,      true,  false, false, true,  true,  false, false, true,  false},

	/* SW7435-276: New format enums for 482/483 */
	{BFMT_VideoFmt_e720x482_NTSC,            false, false, true,  false, false, true,  true,  true,  true },
	{BFMT_VideoFmt_e720x482_NTSC_J,          false, false, true,  false, false, false, true,  true,  true },
	{BFMT_VideoFmt_e720x483p,                false, true,  false, false, true,  true,  true,  true,  true },

	/* custom formats */
	{BFMT_VideoFmt_eCustom0,                 true,  false, false, false, true,  false, false, true,  false},
	{BFMT_VideoFmt_eCustom1,                 true,  false, false, false, true,  false, false, true,  false},
	{BFMT_VideoFmt_eCustom2,                 true,  false, false, false, true,  false, false, true,  false},

	/* Must be last */
	{BFMT_VideoFmt_eMaxCount,                false, false, false, false, false, false, false, false, false},
};


static const BVDC_P_FormatData s_aFormatDataTable[] =
{
	/* Format                                   Analog Microcode                          Digital Microcode                     Digital Microcode DropTbl           ItTable                     ItConfig           */
	{BFMT_VideoFmt_eNTSC,                       s_aulRamBVBInput_NTSC,                    NULL,                                 s_aulDtRamBVBInput_DVI_480i_DropTbl,BVDC_P_aulItTable_NTSC,        &s_ulItConfig_NTSC},
	{BFMT_VideoFmt_eNTSC_J,                     s_aulRamBVBInput_NTSC,                    NULL,                                 s_aulDtRamBVBInput_DVI_480i_DropTbl,BVDC_P_aulItTable_NTSC,        &s_ulItConfig_NTSC},
	{BFMT_VideoFmt_eNTSC_443,                   s_aulRamBVBInput_NTSC,                    NULL,                                 s_aulDtRamBVBInput_DVI_480i_DropTbl,BVDC_P_aulItTable_NTSC,        &s_ulItConfig_NTSC},
	{BFMT_VideoFmt_ePAL_B,                      s_aulRamBVBInput_PAL,                     s_aulDtRamBVBInput_DVI_576i,          NULL,                               BVDC_P_aulItTable_PAL,        &s_ulItConfig_PAL},
	{BFMT_VideoFmt_ePAL_B1,                     s_aulRamBVBInput_PAL,                     s_aulDtRamBVBInput_DVI_576i,          NULL,                               BVDC_P_aulItTable_PAL,        &s_ulItConfig_PAL},
	{BFMT_VideoFmt_ePAL_D,                      s_aulRamBVBInput_PAL,                     s_aulDtRamBVBInput_DVI_576i,          NULL,                               BVDC_P_aulItTable_PAL,        &s_ulItConfig_PAL},
	{BFMT_VideoFmt_ePAL_D1,                     s_aulRamBVBInput_PAL,                     s_aulDtRamBVBInput_DVI_576i,          NULL,                               BVDC_P_aulItTable_PAL,        &s_ulItConfig_PAL},
	{BFMT_VideoFmt_ePAL_G,                      s_aulRamBVBInput_PAL,                     s_aulDtRamBVBInput_DVI_576i,          NULL,                               BVDC_P_aulItTable_PAL,        &s_ulItConfig_PAL},
	{BFMT_VideoFmt_ePAL_H,                      s_aulRamBVBInput_PAL,                     s_aulDtRamBVBInput_DVI_576i,          NULL,                               BVDC_P_aulItTable_PAL,        &s_ulItConfig_PAL},
	{BFMT_VideoFmt_ePAL_K,                      s_aulRamBVBInput_PAL,                     s_aulDtRamBVBInput_DVI_576i,          NULL,                               BVDC_P_aulItTable_PAL,        &s_ulItConfig_PAL},
	{BFMT_VideoFmt_ePAL_I,                      s_aulRamBVBInput_PAL,                     s_aulDtRamBVBInput_DVI_576i,          NULL,                               BVDC_P_aulItTable_PAL,        &s_ulItConfig_PAL},
	{BFMT_VideoFmt_ePAL_M,                      s_aulRamBVBInput_PALM,                    s_aulDtRamBVBInput_DVI_PALM,          NULL,                               BVDC_P_aulItTable_NTSC,       &s_ulItConfig_PALM},
	{BFMT_VideoFmt_ePAL_N,                      s_aulRamBVBInput_PALN,                    s_aulDtRamBVBInput_DVI_576i_OLD,      NULL,                               BVDC_P_aulItTable_NTSC,       &s_ulItConfig_PAL},
	{BFMT_VideoFmt_ePAL_NC,                     s_aulRamBVBInput_PAL,                     s_aulDtRamBVBInput_DVI_576i,          NULL,                               BVDC_P_aulItTable_PAL,        &s_ulItConfig_PAL},
	{BFMT_VideoFmt_ePAL_60,                     s_aulRamBVBInput_NTSC,                    NULL,                                 s_aulDtRamBVBInput_DVI_480i_DropTbl,BVDC_P_aulItTable_NTSC,        &s_ulItConfig_NTSC},
	{BFMT_VideoFmt_eSECAM_L,                    s_aulRamBVBInput_SECAM,                   s_aulDtRamBVBInput_DVI_576i,          NULL,                               BVDC_P_aulItTable_SECAM,      &s_ulItConfig_SECAM},
	{BFMT_VideoFmt_eSECAM_B,                    s_aulRamBVBInput_SECAM,                   s_aulDtRamBVBInput_DVI_576i,          NULL,                               BVDC_P_aulItTable_SECAM,      &s_ulItConfig_SECAM},
	{BFMT_VideoFmt_eSECAM_G,                    s_aulRamBVBInput_SECAM,                   s_aulDtRamBVBInput_DVI_576i,          NULL,                               BVDC_P_aulItTable_SECAM,      &s_ulItConfig_SECAM},
	{BFMT_VideoFmt_eSECAM_D,                    s_aulRamBVBInput_SECAM,                   s_aulDtRamBVBInput_DVI_576i,          NULL,                               BVDC_P_aulItTable_SECAM,      &s_ulItConfig_SECAM},
	{BFMT_VideoFmt_eSECAM_K,                    s_aulRamBVBInput_SECAM,                   s_aulDtRamBVBInput_DVI_576i,          NULL,                               BVDC_P_aulItTable_SECAM,      &s_ulItConfig_SECAM},
	{BFMT_VideoFmt_eSECAM_H,                    s_aulRamBVBInput_SECAM,                   s_aulDtRamBVBInput_DVI_576i,          NULL,                               BVDC_P_aulItTable_SECAM,      &s_ulItConfig_SECAM},
	{BFMT_VideoFmt_e1080i,                      s_aulRamBVBInput_1080i,                   s_aulDtRamBVBInput_DVI_1080i,         NULL,                               s_aulItTable_1080i,            &s_ulItConfig_1080i},
	{BFMT_VideoFmt_e1080p,                      s_aulRamBVBInput_1080p_30hz,              s_aulDtRamBVBInput_DVI_1080p_30hz,    NULL,                               s_aulItTable_1080p,            &s_ulItConfig_1080p},
	{BFMT_VideoFmt_e720p,                       s_aulRamBVBInput_720p,                    s_aulDtRamBVBInput_DVI_720p,          NULL,                               s_aulItTable_720p,             &s_ulItConfig_720p},
	{BFMT_VideoFmt_e720p_60Hz_3DOU_AS,          s_aulRamBVBInput_720p_3D,                 s_aulDtRamBVBInput_DVI_3D_720p_60hz,  NULL,                               s_aulItTable_3D,               &s_ulItConfig_3D},
	{BFMT_VideoFmt_e720p_50Hz_3DOU_AS,          s_aulRamBVBInput_720p_50hz_3D,            s_aulDtRamBVBInput_DVI_3D_720p_50hz,  NULL,                               s_aulItTable_3D,               &s_ulItConfig_3D},
	{BFMT_VideoFmt_e720p_30Hz_3DOU_AS,          s_aulRamBVBInput_720p,                    s_aulDtRamBVBInput_DVI_3D_720p_60hz,  NULL,                               s_aulItTable_720p,             &s_ulItConfig_720p},
	{BFMT_VideoFmt_e720p_24Hz_3DOU_AS,          s_aulRamBVBInput_720p,                    s_aulDtRamBVBInput_DVI_3D_720p_60hz,  NULL,                               s_aulItTable_720p,             &s_ulItConfig_720p},
	{BFMT_VideoFmt_e480p,                       s_aulRamBVBInput_480p,                    NULL,                                 s_aulDtRamBVBInput_DVI_480p_DropTbl,s_aulItTable_480p,             &s_ulItConfig_480p},
	{BFMT_VideoFmt_e1080i_50Hz,                 s_aulRamBVBInput_1125i_50hz,              s_aulDtRamBVBInput_DVI_1125i_50hz,    NULL,                               s_aulItTable_1125i_50hz,       &s_ulItConfig_1080i},
	{BFMT_VideoFmt_e1080p_24Hz_3DOU_AS,         s_aulRamBVBInput_1080p_24hz_3D,           s_aulDtRamBVBInput_DVI_3D_1080p_24hz, NULL,                               s_aulItTable_3D,               &s_ulItConfig_3D},
	{BFMT_VideoFmt_e1080p_30Hz_3DOU_AS,         s_aulRamBVBInput_1080p_30hz_3D,           s_aulDtRamBVBInput_DVI_3D_1080p_30hz, NULL,                               s_aulItTable_3D,               &s_ulItConfig_3D},
	{BFMT_VideoFmt_e1080p_60Hz_3DOU_AS,         s_aulRamBVBInput_1080p_60hz_3DOU,         s_aulDtRamBVBInput_DVI_3DOU_1080p_60hz, NULL,                             s_aulItTable_3D,               &s_ulItConfig_3D},
	{BFMT_VideoFmt_e1080p_60Hz_3DLR,            s_aulRamBVBInput_1080p_60hz_3DLR,         s_aulDtRamBVBInput_DVI_3DLR_1080p_60hz, NULL,                             s_aulItTable_3D,               &s_ulItConfig_3D},
	{BFMT_VideoFmt_e1080p_24Hz,                 s_aulRamBVBInput_1080p_24hz,              s_aulDtRamBVBInput_DVI_1080p_24hz,    NULL,                               s_aulItTable_1080p,            &s_ulItConfig_1080p},
	{BFMT_VideoFmt_e1080p_25Hz,                 s_aulRamBVBInput_1080p_25hz,              s_aulDtRamBVBInput_DVI_1080p_25hz,    NULL,                               s_aulItTable_1080p,            &s_ulItConfig_1080p},
	{BFMT_VideoFmt_e1080p_30Hz,                 s_aulRamBVBInput_1080p_30hz,              s_aulDtRamBVBInput_DVI_1080p_30hz,    NULL,                               s_aulItTable_1080p,            &s_ulItConfig_1080p},
	{BFMT_VideoFmt_e1080p_50Hz,                 s_aulRamBVBInput_1080p_25hz,              s_aulDtRamBVBInput_DVI_1080p_25hz,    NULL,                               s_aulItTable_1080p,            &s_ulItConfig_1080p},
	{BFMT_VideoFmt_e1250i_50Hz,                 s_aulRamBVBInput_1250i_50hz,              NULL,                                 NULL,                               s_aulItTable_1080i,            &s_ulItConfig_1080i},
	{BFMT_VideoFmt_e720p_24Hz,                  s_aulRamBVBInput_720p_24hz,               s_aulDtRamBVBInput_DVI_720p_24hz,     NULL,                               s_aulItTable_720p_24hz,        &s_ulItConfig_720p},
	{BFMT_VideoFmt_e720p_25Hz,                  s_aulRamBVBInput_720p_25hz,               s_aulDtRamBVBInput_DVI_720p_25hz,     NULL,                               s_aulItTable_720p_24hz,        &s_ulItConfig_720p},
	{BFMT_VideoFmt_e720p_30Hz,                  s_aulRamBVBInput_720p_30hz,               s_aulDtRamBVBInput_DVI_720p_30hz,     NULL,                               s_aulItTable_720p_30hz,        &s_ulItConfig_720p},
	{BFMT_VideoFmt_e720p_50Hz,                  s_aulRamBVBInput_720p_50hz,               s_aulDtRamBVBInput_DVI_720p_50hz,     NULL,                               s_aulItTable_720p_50hz,        &s_ulItConfig_720p},
	{BFMT_VideoFmt_e576p_50Hz,                  s_aulRamBVBInput_576p,                    s_aulDtRamBVBInput_DVI_576p,          NULL,                               s_aulItTable_480p,             &s_ulItConfig_480p},
	{BFMT_VideoFmt_e240p_60Hz,                  NULL,                                     NULL,                                 NULL,                               NULL,                          NULL},
	{BFMT_VideoFmt_e288p_50Hz,                  NULL,                                     NULL,                                 NULL,                               NULL,                          NULL},
	{BFMT_VideoFmt_e1440x480p_60Hz,             NULL,                                     NULL,                                 NULL,                               NULL,                          NULL},
	{BFMT_VideoFmt_e1440x576p_50Hz,             NULL,                                     NULL,                                 NULL,                               NULL,                          NULL},
	{BFMT_VideoFmt_e3840x2160p_24Hz,            s_aulRamBVBInput_3840x2160p_24Hz,         s_aulDtRamBVBInput_3840x2160p_24Hz,   NULL,                               s_aulItTable_1080p,            &s_ulItConfig_1080p},
	{BFMT_VideoFmt_e4096x2160p_24Hz,            NULL,                                     s_aulDtRamBVBInput_4096x2160p_24Hz,   NULL,                               s_aulItTable_1080p,            &s_ulItConfig_1080p},
#if BFMT_LEGACY_3DTV_SUPPORT
	{BFMT_VideoFmt_eCUSTOM1920x2160i_48Hz,      NULL,                                     s_aulDtRamBVBInput_DVI_2160i_48hz,    NULL,                               s_aulItTable_1080p,            &s_ulItConfig_1080p},
	{BFMT_VideoFmt_eCUSTOM1920x2160i_60Hz,      NULL,                                     s_aulDtRamBVBInput_DVI_2160i_60hz,    NULL,                               s_aulItTable_1080p,            &s_ulItConfig_1080p},
#endif
	{BFMT_VideoFmt_eCUSTOM_1440x240p_60Hz,      NULL,                                     NULL,                                 NULL,                               NULL,                          NULL},
	{BFMT_VideoFmt_eCUSTOM_1440x288p_50Hz,      NULL,                                     NULL,                                 NULL,                               NULL,                          NULL},
	{BFMT_VideoFmt_eCUSTOM_1366x768p,           s_aulRamBVBInput_CUSTOM_1366x768p,        s_aulDtRamBVBInput_CUSTOM_1366x768p,  NULL,                               s_aulItTable_CUSTOM_1366x768p, &s_ulItConfig_CUSTOM_1366x768p},
	{BFMT_VideoFmt_eCUSTOM_1366x768p_50Hz,      s_aulRamBVBInput_CUSTOM_1366x768p,        s_aulDtRamBVBInput_CUSTOM_1366x768p,  NULL,                               s_aulItTable_CUSTOM_1366x768p, &s_ulItConfig_CUSTOM_1366x768p},
	{BFMT_VideoFmt_eDVI_640x480p,               s_aulRamBVBInput_DVI_640x480p,            s_aulDtRamBVBInput_DVI_640x480p,      NULL,                               s_aulItTable_480p,             &s_ulItConfig_480p},
	{BFMT_VideoFmt_eDVI_640x480p_CVT,           s_aulRamBVBInput_DVI_640x480p_CVT,        NULL,                                 NULL,                               s_aulItTable_VESA,             &s_ulItConfig_480p},
	{BFMT_VideoFmt_eDVI_800x600p,               s_aulRamBVBInput_DVI_800x600p,            s_aulDtRamBVBInput_DVI_800x600p,      NULL,                               s_aulItTable_480p,             &s_ulItConfig_480p},
	{BFMT_VideoFmt_eDVI_1024x768p,              s_aulRamBVBInput_DVI_1024x768p,           s_aulDtRamBVBInput_DVI_1024x768p,     NULL,                               s_aulItTable_480p,             &s_ulItConfig_480p},
	{BFMT_VideoFmt_eDVI_1280x768p,              s_aulRamBVBInput_DVI_1280x768p,           s_aulDtRamBVBInput_DVI_1280x768p,     NULL,                               s_aulItTable_1280x720p,        &s_ulItConfig_480p},
	{BFMT_VideoFmt_eDVI_1280x768p_Red,          NULL,                                     NULL,                                 NULL,                               NULL,                          NULL},
	{BFMT_VideoFmt_eDVI_1280x720p_50Hz,         s_aulRamBVBInput_DVI_1280x720p_50Hz,      s_aulDtRamBVBInput_DVI_1280x720p_50Hz,NULL,                               s_aulItTable_1280x720p,        &s_ulItConfig_480p},
	{BFMT_VideoFmt_eDVI_1280x720p,              s_aulRamBVBInput_DVI_1280x720p,           s_aulDtRamBVBInput_DVI_1280x720p,     NULL,                               s_aulItTable_1280x720p,        &s_ulItConfig_480p},
	{BFMT_VideoFmt_eDVI_1280x720p_Red,          s_aulRamBVBInput_DVI_1280x720p_Red,       s_aulDtRamBVBInput_DVI_1280x720p_Red, NULL,                               s_aulItTable_1280x720p,        &s_ulItConfig_480p},

	/* PC input format! */
	{BFMT_VideoFmt_eDVI_640x350p_60Hz,          NULL,                                     NULL,                                 NULL,                               NULL,                       NULL},
	{BFMT_VideoFmt_eDVI_640x350p_70Hz,          NULL,                                     NULL,                                 NULL,                               NULL,                       NULL},
	{BFMT_VideoFmt_eDVI_640x350p_72Hz,          NULL,                                     NULL,                                 NULL,                               NULL,                       NULL},
	{BFMT_VideoFmt_eDVI_640x350p_75Hz,          NULL,                                     NULL,                                 NULL,                               NULL,                       NULL},
	{BFMT_VideoFmt_eDVI_640x350p_85Hz,          NULL,                                     NULL,                                 NULL,                               NULL,                       NULL},

	{BFMT_VideoFmt_eDVI_640x400p_60Hz,          NULL,                                     NULL,                                 NULL,                               NULL,                       NULL},
	{BFMT_VideoFmt_eDVI_640x400p_70Hz,          NULL,                                     NULL,                                 NULL,                               NULL,                       NULL},
	{BFMT_VideoFmt_eDVI_640x400p_72Hz,          NULL,                                     NULL,                                 NULL,                               NULL,                       NULL},
	{BFMT_VideoFmt_eDVI_640x400p_75Hz,          NULL,                                     NULL,                                 NULL,                               NULL,                       NULL},
	{BFMT_VideoFmt_eDVI_640x400p_85Hz,          NULL,                                     NULL,                                 NULL,                               NULL,                       NULL},

	{BFMT_VideoFmt_eDVI_640x480p_66Hz,          NULL,                                     NULL,                                 NULL,                               NULL,                       NULL},
	{BFMT_VideoFmt_eDVI_640x480p_70Hz,          NULL,                                     NULL,                                 NULL,                               NULL,                       NULL},
	{BFMT_VideoFmt_eDVI_640x480p_72Hz,          NULL,                                     NULL,                                 NULL,                               NULL,                       NULL},
	{BFMT_VideoFmt_eDVI_640x480p_75Hz,          NULL,                                     NULL,                                 NULL,                               NULL,                       NULL},
	{BFMT_VideoFmt_eDVI_640x480p_85Hz,          NULL,                                     NULL,                                 NULL,                               NULL,                       NULL},

	{BFMT_VideoFmt_eDVI_720x400p_60Hz,          NULL,                                     NULL,                                 NULL,                               NULL,                       NULL},
	{BFMT_VideoFmt_eDVI_720x400p_70Hz,          NULL,                                     NULL,                                 NULL,                               NULL,                       NULL},
	{BFMT_VideoFmt_eDVI_720x400p_72Hz,          NULL,                                     NULL,                                 NULL,                               NULL,                       NULL},
	{BFMT_VideoFmt_eDVI_720x400p_75Hz,          NULL,                                     NULL,                                 NULL,                               NULL,                       NULL},
	{BFMT_VideoFmt_eDVI_720x400p_85Hz,          NULL,                                     NULL,                                 NULL,                               NULL,                       NULL},

	{BFMT_VideoFmt_eDVI_800x600p_56Hz,          NULL,                                     NULL,                                 NULL,                               NULL,                       NULL},
	{BFMT_VideoFmt_eDVI_800x600p_59Hz_Red,      NULL,                                     NULL,                                 NULL,                               NULL,                       NULL},
	{BFMT_VideoFmt_eDVI_800x600p_70Hz,          NULL,                                     NULL,                                 NULL,                               NULL,                       NULL},
	{BFMT_VideoFmt_eDVI_800x600p_72Hz,          NULL,                                     NULL,                                 NULL,                               NULL,                       NULL},
	{BFMT_VideoFmt_eDVI_800x600p_75Hz,          NULL,                                     NULL,                                 NULL,                               NULL,                       NULL},
	{BFMT_VideoFmt_eDVI_800x600p_85Hz,          NULL,                                     NULL,                                 NULL,                               NULL,                       NULL},


	{BFMT_VideoFmt_eDVI_1024x768p_66Hz,         NULL,                                     NULL,                                 NULL,                               NULL,                       NULL},
	{BFMT_VideoFmt_eDVI_1024x768p_70Hz,         NULL,                                     NULL,                                 NULL,                               NULL,                       NULL},
	{BFMT_VideoFmt_eDVI_1024x768p_72Hz,         NULL,                                     NULL,                                 NULL,                               NULL,                       NULL},
	{BFMT_VideoFmt_eDVI_1024x768p_75Hz,         NULL,                                     NULL,                                 NULL,                               NULL,                       NULL},
	{BFMT_VideoFmt_eDVI_1024x768p_85Hz,         NULL,                                     NULL,                                 NULL,                               NULL,                       NULL},

	{BFMT_VideoFmt_eDVI_1280x720p_70Hz,         NULL,                                     NULL,                                 NULL,                               NULL,                       NULL},
	{BFMT_VideoFmt_eDVI_1280x720p_72Hz,         NULL,                                     NULL,                                 NULL,                               NULL,                       NULL},
	{BFMT_VideoFmt_eDVI_1280x720p_75Hz,         NULL,                                     NULL,                                 NULL,                               NULL,                       NULL},
	{BFMT_VideoFmt_eDVI_1280x720p_85Hz,         NULL,                                     NULL,                                 NULL,                               NULL,                       NULL},

	/* New DVI or PC vdec input support */
	{BFMT_VideoFmt_eDVI_1024x768i_87Hz,         NULL,                                     NULL,                                 NULL,                               NULL,                          NULL},
	{BFMT_VideoFmt_eDVI_1152x864p_75Hz,         NULL,                                     NULL,                                 NULL,                               NULL,                          NULL},
	{BFMT_VideoFmt_eDVI_1280x768p_75Hz,         NULL,                                     NULL,                                 NULL,                               NULL,                          NULL},
	{BFMT_VideoFmt_eDVI_1280x768p_85Hz,         NULL,                                     NULL,                                 NULL,                               NULL,                          NULL},
	{BFMT_VideoFmt_eDVI_1280x800p_60Hz,         s_aulRamBVBInput_DVI_1280x800p_60Hz,      s_aulDtRamBVBInput_DVI_1280x800p,     NULL,                               s_aulItTable_VESA,             &s_ulItConfig_480p},
	{BFMT_VideoFmt_eDVI_1280x960p_60Hz,         NULL,                                     NULL,                                 NULL,                               NULL,                          NULL},
	{BFMT_VideoFmt_eDVI_1280x960p_85Hz,         NULL,                                     NULL,                                 NULL,                               NULL,                          NULL},
	{BFMT_VideoFmt_eDVI_1280x1024p_60Hz,        s_aulRamBVBInput_DVI_1280x1024p_60Hz,     s_aulDtRamBVBInput_DVI_1280x1024p,    NULL,                               s_aulItTable_VESA,             &s_ulItConfig_480p},
	{BFMT_VideoFmt_eDVI_1280x1024p_69Hz,        NULL,                                     NULL,                                 NULL,                               NULL,                          NULL},
	{BFMT_VideoFmt_eDVI_1280x1024p_75Hz,        NULL,                                     NULL,                                 NULL,                               NULL,                          NULL},
	{BFMT_VideoFmt_eDVI_1280x1024p_85Hz,        NULL,                                     NULL,                                 NULL,                               NULL,                          NULL},
	{BFMT_VideoFmt_eDVI_832x624p_75Hz,          NULL,                                     NULL,                                 NULL,                               NULL,                          NULL},
	{BFMT_VideoFmt_eDVI_1360x768p_60Hz,         s_aulRamBVBInput_DVI_1360x768p_60Hz,      s_aulDtRamBVBInput_DVI_1360x768p,     NULL,                               s_aulItTable_VESA,             &s_ulItConfig_480p},
	{BFMT_VideoFmt_eDVI_1366x768p_60Hz,         s_aulRamBVBInput_DVI_1366x768p_60Hz,      NULL,                                 NULL,                               s_aulItTable_VESA,             &s_ulItConfig_480p},
	{BFMT_VideoFmt_eDVI_1400x1050p_60Hz,        NULL,                                     NULL,                                 NULL,                               NULL,                          NULL},
	{BFMT_VideoFmt_eDVI_1400x1050p_60Hz_Red,    NULL,                                     NULL,                                 NULL,                               NULL,                          NULL},
	{BFMT_VideoFmt_eDVI_1400x1050p_75Hz,        NULL,                                     NULL,                                 NULL,                               NULL,                          NULL},
	{BFMT_VideoFmt_eDVI_1600x1200p_60Hz,        s_aulRamBVBInput_1600x1200p_60Hz,         s_aulDtRamBVBInput_1600x1200p_60Hz,   NULL,                               s_aulItTable_1600x1200p_60Hz,  &s_ulItConfig_CUSTOM_1366x768p},
	{BFMT_VideoFmt_eDVI_1920x1080p_60Hz_Red,    NULL,                                     NULL,                                 NULL,                               NULL,                          NULL},
	{BFMT_VideoFmt_eDVI_848x480p_60Hz,          NULL,                                     NULL,                                 NULL,                               NULL,                          NULL},
	{BFMT_VideoFmt_eDVI_1064x600p_60Hz,         NULL,                                     NULL,                                 NULL,                               NULL,                          NULL},
	{BFMT_VideoFmt_eDVI_1440x900p_60Hz,         s_aulRamBVBInput_DVI_1440x900p_60Hz,      s_aulDtRamBVBInput_DVI_1440x900p,     NULL,                               s_aulItTable_VESA,             &s_ulItConfig_480p},

	/* SW7435-276: New format enums for 482/483 */
	{BFMT_VideoFmt_e720x482_NTSC,               s_aulRamBVBInput_NTSC,                    NULL,                                 s_aulDtRamBVBInput_DVI_480i_DropTbl,BVDC_P_aulItTable_NTSC,        &s_ulItConfig_NTSC},
	{BFMT_VideoFmt_e720x482_NTSC_J,             s_aulRamBVBInput_NTSC,                    NULL,                                 s_aulDtRamBVBInput_DVI_480i_DropTbl,BVDC_P_aulItTable_NTSC,        &s_ulItConfig_NTSC},
	{BFMT_VideoFmt_e720x483p,                   s_aulRamBVBInput_480p,                    NULL,                                 s_aulDtRamBVBInput_DVI_480p_DropTbl,s_aulItTable_480p,             &s_ulItConfig_480p},

	/* custom formats */
	{BFMT_VideoFmt_eCustom0,                    s_aulRamBVBInput_CUSTOM_1366x768p,        s_aulDtRamBVBInput_CUSTOM_1366x768p,  NULL,                               s_aulItTable_CUSTOM_1366x768p, &s_ulItConfig_CUSTOM_1366x768p},
	{BFMT_VideoFmt_eCustom1,                    s_aulRamBVBInput_CUSTOM_1366x768p,        s_aulDtRamBVBInput_CUSTOM_1366x768p,  NULL,                               s_aulItTable_CUSTOM_1366x768p, &s_ulItConfig_CUSTOM_1366x768p},
	{BFMT_VideoFmt_eCustom2,                    s_aulRamBVBInput_CUSTOM_1366x768p,        s_aulDtRamBVBInput_CUSTOM_1366x768p,  NULL,                               s_aulItTable_CUSTOM_1366x768p, &s_ulItConfig_CUSTOM_1366x768p},

	/* Must be last */
	{BFMT_VideoFmt_eMaxCount,                   NULL,                                     NULL,                                 NULL,                               NULL,                       NULL},
};

/****************************************************************
 *  When adding additional formats, make sure each format is
 *  properly configured for all colorspaces it supports in the
 *  tables below.  Please also ensure that SD and HD specific
 *  configuration tables also configure them properly.
 *
 *  Each table below has an entry with BFMT_VideoFmt_eMaxCount
 *  as the format.  This entry specifies the default
 *  configuration for formats that aren't treated as a special
 *  case. Special case formats with non-default configurations
 *  are specified by adding an entry for them above the
 *  BFMT_VideoFmt_eMaxCount entry.
 *
 *  The BFMT_VideoFmt_eMaxCount format entry should always be the
 *  last in a table, and is used by search loops to determine
 *  when to terminate.
 ****************************************************************/

/****************************************************************
 *  Sm Tables
 ****************************************************************/
/* YQI SmTables */
static const BVDC_P_SmTableInfo s_aulSmTable_Tbl_YQI[] =
{
	{BFMT_VideoFmt_eMaxCount,           s_aulSmTable_YQI}
};

/* YQI_M SmTables */
static const BVDC_P_SmTableInfo s_aulSmTable_Tbl_YQI_M[] =
{
	{BFMT_VideoFmt_eMaxCount,           s_aulSmTable_YQI}
};

/* YUV SmTables */
static const BVDC_P_SmTableInfo s_aulSmTable_Tbl_YUV[] =
{
	{BFMT_VideoFmt_ePAL_N,              s_aulSmTable_PALN_YUV},
	{BFMT_VideoFmt_ePAL_NC,             s_aulSmTable_PALNC_YUV},
	{BFMT_VideoFmt_eSECAM_L,            s_aulSmTable_SECAM},
	{BFMT_VideoFmt_eSECAM_B,            s_aulSmTable_SECAM},
	{BFMT_VideoFmt_eSECAM_G,            s_aulSmTable_SECAM},
	{BFMT_VideoFmt_eSECAM_D,            s_aulSmTable_SECAM},
	{BFMT_VideoFmt_eSECAM_K,            s_aulSmTable_SECAM},
	{BFMT_VideoFmt_eSECAM_H,            s_aulSmTable_SECAM},
	{BFMT_VideoFmt_eMaxCount,           s_aulSmTable_PAL_YUV}
};

/* YUV_N SmTables */
static const BVDC_P_SmTableInfo s_aulSmTable_Tbl_YUV_N[] =
{
	{BFMT_VideoFmt_ePAL_M,              s_aulSmTable_PALM_YUV},
	{BFMT_VideoFmt_ePAL_N,              s_aulSmTable_PALN_YUV},
	{BFMT_VideoFmt_eMaxCount,           s_aulSmTable_PAL_YUV}
};

/* RGB SmTables */
static const BVDC_P_SmTableInfo s_aulSmTable_Tbl_RGB[] =
{
	{BFMT_VideoFmt_eMaxCount,           s_aulSmTable_Component}
};

/* SDYPrPb SmTables */
static const BVDC_P_SmTableInfo s_aulSmTable_Tbl_SDYPrPb[] =
{
	{BFMT_VideoFmt_eMaxCount,           s_aulSmTable_Component}
};

/* HDYPrPb SmTables */
static const BVDC_P_SmTableInfo s_aulSmTable_Tbl_HDYPrPb[] =
{
	{BFMT_VideoFmt_eMaxCount,           s_aulSmTable_Component}
};

/* Hsync SmTables */
static const BVDC_P_SmTableInfo s_aulSmTable_Tbl_Hsync[] =
{
	{BFMT_VideoFmt_eMaxCount,           s_aulSmTable_Component}
};

/* Unknown SmTables */
static const BVDC_P_SmTableInfo s_aulSmTable_Tbl_Unknown[] =
{
	{BFMT_VideoFmt_eMaxCount,           s_aulSmTable_Component}
};

/****************************************************************
 *  Channel Filter Tables
 ****************************************************************/
static const BVDC_P_FilterTableInfo s_aulFilterTable_Tbl[] =
{
	/* OutputFilter                ChFilter_Ch0               ChFilter_Ch1                 ChFilter_Ch2 */
#if BVDC_P_ORTHOGONAL_VEC
	{BVDC_P_OutputFilter_eHD,      s_aulChFilterTbl_AllPass,  s_aulChFilterTbl_Cr_675Mhz,  s_aulChFilterTbl_Cr_675Mhz},
	{BVDC_P_OutputFilter_eED,      s_aulChFilterTbl_AllPass,  s_aulChFilterTbl_AllPass,    s_aulChFilterTbl_AllPass},
	{BVDC_P_OutputFilter_eSDYPrPb, s_aulChFilterTbl_Y_60Mhz,  s_aulChFilterTbl_Cr_SD,      s_aulChFilterTbl_Cr_SD},
	{BVDC_P_OutputFilter_eYQI,     s_aulChFilterTbl_Y_60Mhz,  s_aulChFilterTbl_Cr_13Mhz,   s_aulChFilterTbl_Cr_13Mhz},
	{BVDC_P_OutputFilter_eYUV,     s_aulChFilterTbl_Y_60Mhz,  s_aulChFilterTbl_Cr_13Mhz,   s_aulChFilterTbl_Cr_13Mhz},
	{BVDC_P_OutputFilter_eSECAM,   s_aulChFilterTbl_Y_12Mhz,  s_aulChFilterTbl_Cr_SECAM,   s_aulChFilterTbl_Cr_SECAM},
	{BVDC_P_OutputFilter_eRGB,     s_aulChFilterTbl_Y_675Mhz, s_aulChFilterTbl_Y_675Mhz,   s_aulChFilterTbl_Y_675Mhz},
	{BVDC_P_OutputFilter_eHsync,   s_aulChFilterTbl_Y_675Mhz, s_aulChFilterTbl_Y_675Mhz,   s_aulChFilterTbl_Y_675Mhz},
	{BVDC_P_OutputFilter_eUnknown, s_aulChFilterTbl_Y_60Mhz,  s_aulChFilterTbl_Cr_SD,      s_aulChFilterTbl_Cr_SD},
#else
	{BVDC_P_OutputFilter_eHD,      s_aulChFilterTbl_AllPass,  s_aulChFilterTbl_AllPass,    s_aulChFilterTbl_AllPass},
	{BVDC_P_OutputFilter_eED,      s_aulChFilterTbl_AllPass,  s_aulChFilterTbl_AllPass,    s_aulChFilterTbl_AllPass},
	{BVDC_P_OutputFilter_eSDYPrPb, s_aulChFilterTbl_Y_675Mhz, s_aulChFilterTbl_Cr_3375Mhz, s_aulChFilterTbl_Cr_3375Mhz},
	{BVDC_P_OutputFilter_eYQI,     s_aulChFilterTbl_Y_60Mhz,  s_aulChFilterTbl_Cr_13Mhz,   s_aulChFilterTbl_Cr_13Mhz},
	{BVDC_P_OutputFilter_eYUV,     s_aulChFilterTbl_Y_60Mhz,  s_aulChFilterTbl_Cr_13Mhz,   s_aulChFilterTbl_Cr_13Mhz},
	{BVDC_P_OutputFilter_eSECAM,   s_aulChFilterTbl_Y_50Mhz,  s_aulChFilterTbl_Cr_13Mhz,   s_aulChFilterTbl_Cr_13Mhz},
	{BVDC_P_OutputFilter_eRGB,     s_aulChFilterTbl_Y_675Mhz, s_aulChFilterTbl_Y_675Mhz,   s_aulChFilterTbl_Y_675Mhz},
	{BVDC_P_OutputFilter_eHsync,   s_aulChFilterTbl_Y_675Mhz, s_aulChFilterTbl_Y_675Mhz,   s_aulChFilterTbl_Y_675Mhz},
	{BVDC_P_OutputFilter_eUnknown, s_aulChFilterTbl_Y_60Mhz,  s_aulChFilterTbl_Cr_3375Mhz, s_aulChFilterTbl_Cr_3375Mhz},
#endif
};

/****************************************************************
 *  Vf Tables
 ****************************************************************/
/* Hd Hsync VfTables */
static const BVDC_P_VfTableInfo s_aulHdVfTable_Tbl_Hsync[] =
{
	{BFMT_VideoFmt_eCUSTOM_1366x768p,      s_aulVfTable_1080i_720p_RGB},
	{BFMT_VideoFmt_eCUSTOM_1366x768p_50Hz, s_aulVfTable_1080i_720p_RGB},
	{BFMT_VideoFmt_eMaxCount,              s_aulVfTable_1080i_720p_YPbPr}
};

/* Hd RGB VfTables */
static const BVDC_P_VfTableInfo s_aulHdVfTable_Tbl_RGB[] =
{
	{BFMT_VideoFmt_e480p,                  s_aulVfTable_480p_RGB},
	{BFMT_VideoFmt_e720x483p,              s_aulVfTable_480p_RGB},
	{BFMT_VideoFmt_e576p_50Hz,             s_aulVfTable_480p_RGB},
	{BFMT_VideoFmt_eDVI_640x480p,          s_aulVfTable_VESA_RGB},
	{BFMT_VideoFmt_eDVI_640x480p_CVT,      s_aulVfTable_VESA_RGB},
	{BFMT_VideoFmt_eDVI_800x600p,          s_aulVfTable_VESA_RGB},
	{BFMT_VideoFmt_eDVI_1024x768p,         s_aulVfTable_VESA_RGB},
	{BFMT_VideoFmt_eDVI_1280x720p,         s_aulVfTable_VESA_RGB},
	{BFMT_VideoFmt_eDVI_1280x800p_60Hz,    s_aulVfTable_VESA_RGB},
	{BFMT_VideoFmt_eDVI_1280x1024p_60Hz,   s_aulVfTable_VESA_RGB},
	{BFMT_VideoFmt_eDVI_1360x768p_60Hz,    s_aulVfTable_VESA_RGB},
	{BFMT_VideoFmt_eDVI_1366x768p_60Hz,    s_aulVfTable_VESA_RGB},
	{BFMT_VideoFmt_eDVI_1440x900p_60Hz,    s_aulVfTable_VESA_RGB},
	{BFMT_VideoFmt_eMaxCount,              s_aulVfTable_1080i_720p_RGB}
};

/* Same, but for double rate pixels. */
static const BVDC_P_VfTableInfo s_aulHdVfTable_Tbl_54MHz_RGB[] =
{
	{BFMT_VideoFmt_e480p,                  s_aulVfTable_480p_54MHz_RGB},
	{BFMT_VideoFmt_e720x483p,              s_aulVfTable_480p_54MHz_RGB},
	{BFMT_VideoFmt_e576p_50Hz,             s_aulVfTable_480p_54MHz_RGB},
	{BFMT_VideoFmt_eMaxCount,              s_aulVfTable_1080i_720p_RGB}
};

/* Hd HDYPrPb VfTables */
static const BVDC_P_VfTableInfo s_aulHdVfTable_Tbl_HDYPrPb[] =
{
	{BFMT_VideoFmt_e480p,                  s_aulVfTable_480p_YPbPr},
	{BFMT_VideoFmt_e720x483p,              s_aulVfTable_480p_YPbPr},
	{BFMT_VideoFmt_e576p_50Hz,             s_aulVfTable_576p_YPbPr},
	{BFMT_VideoFmt_eCUSTOM_1366x768p,      s_aulVfTable_1080i_720p_RGB},
	{BFMT_VideoFmt_eCUSTOM_1366x768p_50Hz, s_aulVfTable_1080i_720p_RGB},
	{BFMT_VideoFmt_eMaxCount,              s_aulVfTable_1080i_720p_YPbPr}
};

/* Same, but for double rate pixels. */
static const BVDC_P_VfTableInfo s_aulHdVfTable_Tbl_54MHz_HDYPrPb[] =
{
	{BFMT_VideoFmt_e480p,                  s_aulVfTable_480p_54MHz_YPbPr},
	{BFMT_VideoFmt_e720x483p,              s_aulVfTable_480p_54MHz_YPbPr},
	{BFMT_VideoFmt_e576p_50Hz,             s_aulVfTable_576p_54MHz_YPbPr},
	{BFMT_VideoFmt_eMaxCount,              s_aulVfTable_1080i_720p_YPbPr}
};

/* Unknown VfTables */
static const BVDC_P_VfTableInfo s_aulHdVfTable_Tbl_Unknown[] =
{
	{BFMT_VideoFmt_e480p,                  s_aulVfTable_480p_YPbPr},
	{BFMT_VideoFmt_e720x483p,              s_aulVfTable_480p_YPbPr},
	{BFMT_VideoFmt_e576p_50Hz,             s_aulVfTable_576p_YPbPr},
	{BFMT_VideoFmt_eCUSTOM_1366x768p,      s_aulVfTable_1080i_720p_RGB},
	{BFMT_VideoFmt_eCUSTOM_1366x768p_50Hz, s_aulVfTable_1080i_720p_RGB},
	{BFMT_VideoFmt_eMaxCount,              s_aulVfTable_1080i_720p_YPbPr}
};

/* Same, for 480P / 54 MHz pixel rate */
static const BVDC_P_VfTableInfo s_aulHdVfTable_Tbl_54MHz_Unknown[] =
{
	{BFMT_VideoFmt_e480p,                  s_aulVfTable_480p_54MHz_YPbPr},
	{BFMT_VideoFmt_e720x483p,              s_aulVfTable_480p_54MHz_YPbPr},
	{BFMT_VideoFmt_e576p_50Hz,             s_aulVfTable_576p_54MHz_YPbPr},
	{BFMT_VideoFmt_eMaxCount,              s_aulVfTable_1080i_720p_YPbPr}
};

/* Sd YQI VfTables */
static const BVDC_P_VfTableInfo s_aulSdVfTable_Tbl_YQI[] =
{
	{BFMT_VideoFmt_eMaxCount, s_aulVfTable_YQI}
};

/* Sd YQI_M VfTables */
static const BVDC_P_VfTableInfo s_aulSdVfTable_Tbl_YQI_M[] =
{
	{BFMT_VideoFmt_eMaxCount, s_aulVfTable_YQI}
};

/* Sd YUV VfTables */
static const BVDC_P_VfTableInfo s_aulSdVfTable_Tbl_YUV[] =
{
	{BFMT_VideoFmt_eMaxCount, s_aulVfTable_YUV}
};

/* Sd YUVN VfTables */
static const BVDC_P_VfTableInfo s_aulSdVfTable_Tbl_YUV_N[] =
{
	{BFMT_VideoFmt_eMaxCount, s_aulVfTable_YQI}
};

/* Sd RGB VfTables */
static const BVDC_P_VfTableInfo s_aulSdVfTable_Tbl_RGB[] =
{
	{BFMT_VideoFmt_eNTSC,           s_aulVfTable_SD_RGB},
	{BFMT_VideoFmt_eNTSC_J,         s_aulVfTable_SD_RGB},
	{BFMT_VideoFmt_e720x482_NTSC,   s_aulVfTable_SD_RGB},
	{BFMT_VideoFmt_e720x482_NTSC_J, s_aulVfTable_SD_RGB},
	{BFMT_VideoFmt_eNTSC_443,       s_aulVfTable_SD_RGB},
	{BFMT_VideoFmt_ePAL_60,         s_aulVfTable_SD_RGB},
	{BFMT_VideoFmt_ePAL_M,          s_aulVfTable_SD_RGB},
	{BFMT_VideoFmt_eMaxCount,       s_aulVfTable_SD_RGB_50Hz}
};

/* Sd Hsync VfTables */
static const BVDC_P_VfTableInfo s_aulSdVfTable_Tbl_Hsync[] =
{
	{BFMT_VideoFmt_eMaxCount, s_aulVfTable_SD_Hsync}
};

/* Sd YPrPb VfTables */
static const BVDC_P_VfTableInfo s_aulSdVfTable_Tbl_SDYPrPb[] =
{
	{BFMT_VideoFmt_eNTSC,           s_aulVfTable_SD_YPbPr},
	{BFMT_VideoFmt_eNTSC_J,         s_aulVfTable_SD_YPbPr},
	{BFMT_VideoFmt_e720x482_NTSC,   s_aulVfTable_SD_YPbPr},
	{BFMT_VideoFmt_e720x482_NTSC_J, s_aulVfTable_SD_YPbPr},
	{BFMT_VideoFmt_eNTSC_443,       s_aulVfTable_SD_YPbPr},
	{BFMT_VideoFmt_ePAL_60,         s_aulVfTable_SD_YPbPr},
	{BFMT_VideoFmt_ePAL_M,          s_aulVfTable_SD_YPbPr},
	{BFMT_VideoFmt_eMaxCount,       s_aulVfTable_SD_YPbPr_50Hz}
};

/* Sd Unknown VfTables */
static const BVDC_P_VfTableInfo s_aulSdVfTable_Tbl_Unknown[] =
{
	{BFMT_VideoFmt_eMaxCount, s_aulVfTable_SD_YPbPr}
};



#if BVDC_P_ORTHOGONAL_VEC
#define BVDC_P_MAKE_ENVELOPGENERATOR(amp_cntl, amp_clamp, u_burst, v_burst,    \
	u_en, v_en, init_amp)                                                      \
	BVDC_P_VF_FIELD_ENUM(ENVELOPE_GENERATOR, AMP_CONTROL,       amp_cntl) | \
	BVDC_P_VF_FIELD_DATA(ENVELOPE_GENERATOR, AMP_CLAMP,        amp_clamp) | \
	BVDC_P_VF_FIELD_ENUM(ENVELOPE_GENERATOR, U_BURST_AMP_SEL,    u_burst) | \
	BVDC_P_VF_FIELD_ENUM(ENVELOPE_GENERATOR, V_BURST_AMP_SEL,    v_burst) | \
	BVDC_P_VF_FIELD_ENUM(ENVELOPE_GENERATOR, U_ENABLE,              u_en) | \
	BVDC_P_VF_FIELD_ENUM(ENVELOPE_GENERATOR, V_ENABLE,              v_en) | \
	BVDC_P_VF_FIELD_DATA(ENVELOPE_GENERATOR, INIT_AMP,          init_amp)

#define BVDC_EG_YQI_SETTING       BVDC_P_MAKE_ENVELOPGENERATOR(CONSTANT,     0, NEGATIVE, NEGATIVE,  ON, OFF, 0x70)
#define BVDC_EG_PAL_YUV_SETTING   BVDC_P_MAKE_ENVELOPGENERATOR(CONSTANT,     0, NEGATIVE, POSITIVE,  ON,  ON, 0x54)
#define BVDC_EG_PALNC_YUV_SETTING BVDC_P_MAKE_ENVELOPGENERATOR(CONSTANT,     0, NEGATIVE, POSITIVE,  ON,  ON, 0x54)
#define BVDC_EG_PALM_YUV_SETTING  BVDC_P_MAKE_ENVELOPGENERATOR(CONSTANT,     0, NEGATIVE, POSITIVE,  ON,  ON, 0x58)
#define BVDC_EG_SECAM_SETTING     BVDC_P_MAKE_ENVELOPGENERATOR(INC_BY_ONE, 424, NEGATIVE, NEGATIVE,  ON, OFF, 0x78)
#define BVDC_EG_COMPONET_SETTING  BVDC_P_MAKE_ENVELOPGENERATOR(CONSTANT,     0, NEGATIVE, NEGATIVE, OFF, OFF,    0)


/****************************************************************
 *  When adding additional formats, make sure each format is
 *  properly configured for all colorspaces it supports in the
 *  tables below.
 *
 *  Each table below has an entry with BFMT_VideoFmt_eMaxCount
 *  as the format.  This entry specifies the default
 *  configuration for formats that aren't treated as a special
 *  case. Special case formats with non-default configurations
 *  are specified by adding an entry for them above the
 *  BFMT_VideoFmt_eMaxCount entry.
 *
 *  The BFMT_VideoFmt_eMaxCount format entry should always be the
 *  last in a table, and is used by search loops to determine
 *  when to terminate.
 ****************************************************************/

/****************************************************************
 *  Envelop Generator Control Setting Tables
 ****************************************************************/
static const BVDC_P_EnvelopGeneratorSetting s_aulEG_Tbl_YQI[] =
{
	{BFMT_VideoFmt_eMaxCount,           BVDC_EG_YQI_SETTING}
};

static const BVDC_P_EnvelopGeneratorSetting s_aulEG_Tbl_YQI_M[] =
{
	{BFMT_VideoFmt_eMaxCount,           BVDC_EG_YQI_SETTING}
};

static const BVDC_P_EnvelopGeneratorSetting s_aulEG_Tbl_YUV[] =
{
	{BFMT_VideoFmt_ePAL_NC,             BVDC_EG_PALNC_YUV_SETTING},
	{BFMT_VideoFmt_eSECAM_L,            BVDC_EG_SECAM_SETTING},
	{BFMT_VideoFmt_eSECAM_B,            BVDC_EG_SECAM_SETTING},
	{BFMT_VideoFmt_eSECAM_G,            BVDC_EG_SECAM_SETTING},
	{BFMT_VideoFmt_eSECAM_D,            BVDC_EG_SECAM_SETTING},
	{BFMT_VideoFmt_eSECAM_K,            BVDC_EG_SECAM_SETTING},
	{BFMT_VideoFmt_eSECAM_H,            BVDC_EG_SECAM_SETTING},
	{BFMT_VideoFmt_eMaxCount,           BVDC_EG_PAL_YUV_SETTING}
};

static const BVDC_P_EnvelopGeneratorSetting s_aulEG_Tbl_YUV_N[] =
{
	{BFMT_VideoFmt_ePAL_M,              BVDC_EG_PALM_YUV_SETTING},
	{BFMT_VideoFmt_eMaxCount,           BVDC_EG_PAL_YUV_SETTING}
};

static const BVDC_P_EnvelopGeneratorSetting s_aulEG_Tbl_RGB[] =
{
	{BFMT_VideoFmt_eMaxCount,           BVDC_EG_COMPONET_SETTING}
};

static const BVDC_P_EnvelopGeneratorSetting s_aulEG_Tbl_SDYPrPb[] =
{
	{BFMT_VideoFmt_eMaxCount,           BVDC_EG_COMPONET_SETTING}
};

static const BVDC_P_EnvelopGeneratorSetting s_aulEG_Tbl_HDYPrPb[] =
{
	{BFMT_VideoFmt_eMaxCount,           BVDC_EG_COMPONET_SETTING}
};

static const BVDC_P_EnvelopGeneratorSetting s_aulEG_Tbl_Hsync[] =
{
	{BFMT_VideoFmt_eMaxCount,           BVDC_EG_COMPONET_SETTING}
};

static const BVDC_P_EnvelopGeneratorSetting s_aulEG_Tbl_Unknown[] =
{
	{BFMT_VideoFmt_eMaxCount,           BVDC_EG_COMPONET_SETTING}
};

/* VF Envelop Generator Control settings */
static const BVDC_P_EnvelopGeneratorInfo s_aEnvelopGeneratorTable[] =
{
	/* Output Color Space            Envelope Generator Control Setting */
	{BVDC_P_Output_eYQI,             s_aulEG_Tbl_YQI                      },
	{BVDC_P_Output_eYQI_M,           s_aulEG_Tbl_YQI_M                    },
	{BVDC_P_Output_eYUV,             s_aulEG_Tbl_YUV                      },
	{BVDC_P_Output_eYUV_N,           s_aulEG_Tbl_YUV_N                    },
	{BVDC_P_Output_eYUV_NC,          s_aulEG_Tbl_YUV                      },
#if BVDC_P_SUPPORT_VEC_SECAM
	{BVDC_P_Output_eYDbDr_LDK,       s_aulEG_Tbl_YUV                      },
	{BVDC_P_Output_eYDbDr_BG,        s_aulEG_Tbl_YUV                      },
	{BVDC_P_Output_eYDbDr_H,         s_aulEG_Tbl_YUV                      },
#endif
	/* special handling for 480p and 576p, see above */
	{BVDC_P_Output_eSDYPrPb,         s_aulEG_Tbl_SDYPrPb                  },
	{BVDC_P_Output_eRGB,             s_aulEG_Tbl_RGB                      },
	{BVDC_P_Output_eHDYPrPb,         s_aulEG_Tbl_HDYPrPb                  },
	{BVDC_P_Output_eHsync,           s_aulEG_Tbl_Hsync                    },
	{BVDC_P_Output_eUnknown,         s_aulEG_Tbl_Unknown                  }
};
#endif


/****************************************************************
 *  Rate Manager Tables
 ****************************************************************/
static const BVDC_P_RmTableInfo s_aRmFullRate_Tbl[] =
{
	{BFMT_PXL_25_2MHz,         s_aulRmTable_25_2,         "25.20"},
	{BFMT_PXL_27MHz_MUL_1_001, s_aulRmTable_27_Mul_1001,  "27.00*1.001"},
	{BFMT_PXL_74_25MHz,        s_aulRmTable_74_25,        "74.25"},
	{BFMT_PXL_148_5MHz,        s_aulRmTable_148_5,        "148.5"},
	{BFMT_PXL_56_304MHz,       s_aulRmTable_56_304,       "56.304"},
	{BFMT_PXL_67_565MHz,       s_aulRmTable_67_565,       "67.565"},
	{BFMT_PXL_39_79MHz,        s_aulRmTable_39_79,        "39.79"},
	{BFMT_PXL_65MHz,           s_aulRmTable_65,           "64.99584"},
	{BFMT_PXL_65_286MHz,       s_aulRmTable_65_286,       "65.286"},
	{BFMT_PXL_60_4656MHz,      s_aulRmTable_60_4656,      "60.4656"},
	{BFMT_PXL_64_022MHz,       s_aulRmTable_64_0224,      "64.0224"},
	{BFMT_PXL_162MHz,          s_aulRmTable_162,          "162"},
	{BFMT_PXL_23_75MHz,        s_aulRmTable_23_75,        "23.75"},
	{BFMT_PXL_83_5MHz,         s_aulRmTable_83_5,         "83.5"},
	{BFMT_PXL_108MHz,          s_aulRmTable_108,          "108"},
	{BFMT_PXL_85_5MHz,         s_aulRmTable_85_5,         "85.5"},
	{BFMT_PXL_106_5MHz,        s_aulRmTable_106_5,        "106.5"},
	{BFMT_PXL_54MHz_MUL_1_001, s_aulRmTable_54_Mul_1001,  "54.00*1.001"}
};
#define BVDC_P_FULLRATE_TBL_SIZE (sizeof(s_aRmFullRate_Tbl) / sizeof(BVDC_P_RmTableInfo))

static const BVDC_P_RmTableInfo s_aRmDropRate_Tbl[] =
{
	{BFMT_PXL_25_2MHz_DIV_1_001,   s_aulRmTable_25_2_Div_1001,   "25.20/1.001"},
	{BFMT_PXL_27MHz,               s_aulRmTable_27,              "27.00"},
	{BFMT_PXL_74_25MHz_DIV_1_001,  s_aulRmTable_74_25_Div_1001,  "74.25/1.001"},
	{BFMT_PXL_148_5MHz_DIV_1_001,  s_aulRmTable_148_5_Div_1001,  "148.5/1.001"},
	{BFMT_PXL_67_565MHz_DIV_1_001, s_aulRmTable_67_565_Div_1001, "67.565/1.001"},
	{BFMT_PXL_39_79MHz_DIV_1_001,  s_aulRmTable_39_79_Div_1001,  "39.79/1.001"},
	{BFMT_PXL_65MHz_DIV_1_001,     s_aulRmTable_65_Div_1001,     "64.99584/1.001"},
	{BFMT_PXL_65_286MHz_DIV_1_001, s_aulRmTable_65_286_Div_1001, "65.286/1.001"},
	{BFMT_PXL_64_022MHz_DIV_1_001, s_aulRmTable_64_0224_Div_1001,"64.0224/1.001"},
	{BFMT_PXL_23_75MHz_DIV_1_001,  s_aulRmTable_23_75_Div_1001,  "23.75/1.001"},
	{BFMT_PXL_83_5MHz_DIV_1_001,   s_aulRmTable_83_5_Div_1001,   "83.5/1.001"},
	{BFMT_PXL_108MHz_DIV_1_001,    s_aulRmTable_108_Div_1001,    "108/1.001"},
	{BFMT_PXL_85_5MHz_DIV_1_001,   s_aulRmTable_85_5_Div_1001,   "85.5/1.001"},
	{BFMT_PXL_106_5MHz_DIV_1_001,  s_aulRmTable_106_5_Div_1001,  "106.5/1.001"},
	{BFMT_PXL_54MHz,               s_aulRmTable_54,              "54.00"},
};
#define BVDC_P_DROPRATE_TBL_SIZE (sizeof(s_aRmDropRate_Tbl) / sizeof(BVDC_P_RmTableInfo))

/****************************************************************
 *  ColorSpace Data Table
 *  This table bears an entry for every colorspace.  Each
 *  entry specifies the config tables needed to support that
 *  colorspace.
 *
 *  Some colorspaces are used by both HD and SD display formats,
 *  and their entries include both HD and SD configuration tables
 *  accordingly.
 *
 *  Configuration tables are loaded by selecting the correct
 *  table entry according to whichever colorspace the display is
 *  configured for, and then selecting the correct configuration
 *  tables from that entry, depending on whether the display
 *  format is HD or SD.
 *
 *  BVDC_P_Output_eSDYPrPb is a special case.  While it is
 *  technically an SD-only colorspace, the HD formats 480p and
 *  576p both use it for loading CSC tables elsewhere, but still
 *  need to be configured as HD here, and thus, HD configuration
 *  tables exist in the entry in order to comply with the
 *  convention used above.
 *
 *  This table should not need to be modified when
 *  adding formats.
 *
 *  Note:
 *    1. NTSC uses YQI settings; NTSC_J uses YQI_M settings;
 *    2. PAL_M has the same CSC matrix as PAL_N, both with 7.5IRE
 *       pedestal on luma, but the same chroma as PAL/BDGHI;
 *    3. PAL_M has sub-carrier freq at 3.575611.49 MHz; PAL_N has the
 *       same FSC as PAL/BDGHI at 4.433618.75 MHz;
 *    4. PAL_NC has the same CSC matrix as PAL; but PAL_NC
 *       has sub-carrier frequency at 3.58205625 MHz;
 ****************************************************************/
static const BVDC_P_ColorSpaceData s_aColorSpaceDataTable[] =
{
	/* Format                  Sm Table Tbl              SD Filter Table Tbl               HD Filter Table Tbl     SD Vf Table Tbl             HD Vf Table Tbl */
	{BVDC_P_Output_eYQI,       s_aulSmTable_Tbl_YQI,     BVDC_P_OutputFilter_eYQI,       BVDC_P_OutputFilter_eNone,    s_aulSdVfTable_Tbl_YQI,     NULL},
	{BVDC_P_Output_eYQI_M,     s_aulSmTable_Tbl_YQI_M,   BVDC_P_OutputFilter_eYQI,       BVDC_P_OutputFilter_eNone,    s_aulSdVfTable_Tbl_YQI_M,   NULL},
	{BVDC_P_Output_eYUV,       s_aulSmTable_Tbl_YUV,     BVDC_P_OutputFilter_eYUV,       BVDC_P_OutputFilter_eNone,    s_aulSdVfTable_Tbl_YUV,     NULL},
	{BVDC_P_Output_eYUV_N,     s_aulSmTable_Tbl_YUV_N,   BVDC_P_OutputFilter_eYUV,       BVDC_P_OutputFilter_eNone,    s_aulSdVfTable_Tbl_YUV_N,   NULL},
	{BVDC_P_Output_eYUV_NC,    s_aulSmTable_Tbl_YUV,     BVDC_P_OutputFilter_eYUV,       BVDC_P_OutputFilter_eNone,    s_aulSdVfTable_Tbl_YUV,     NULL},
#if BVDC_P_SUPPORT_VEC_SECAM
#if !BVDC_P_ORTHOGONAL_VEC
	{BVDC_P_Output_eYDbDr_LDK, s_aulSmTable_Tbl_YUV,     BVDC_P_OutputFilter_eYUV,       BVDC_P_OutputFilter_eNone,    s_aulSdVfTable_Tbl_YUV,     NULL},
#else
	{BVDC_P_Output_eYDbDr_LDK, s_aulSmTable_Tbl_YUV,     BVDC_P_OutputFilter_eSECAM,     BVDC_P_OutputFilter_eNone,    s_aulSdVfTable_Tbl_YUV,     NULL},
#endif
	{BVDC_P_Output_eYDbDr_BG,  s_aulSmTable_Tbl_YUV,  BVDC_P_OutputFilter_eSECAM,     BVDC_P_OutputFilter_eNone,    s_aulSdVfTable_Tbl_YUV,     NULL},
	{BVDC_P_Output_eYDbDr_H,   s_aulSmTable_Tbl_YUV,  BVDC_P_OutputFilter_eSECAM,     BVDC_P_OutputFilter_eNone,    s_aulSdVfTable_Tbl_YUV,     NULL},
#endif
	/* special handling for 480p and 576p, see above */
	{BVDC_P_Output_eSDYPrPb,   s_aulSmTable_Tbl_SDYPrPb, BVDC_P_OutputFilter_eSDYPrPb,   BVDC_P_OutputFilter_eED,      s_aulSdVfTable_Tbl_SDYPrPb, s_aulHdVfTable_Tbl_HDYPrPb},
	{BVDC_P_Output_eRGB,       s_aulSmTable_Tbl_RGB,     BVDC_P_OutputFilter_eRGB,       BVDC_P_OutputFilter_eHD,      s_aulSdVfTable_Tbl_RGB,     s_aulHdVfTable_Tbl_RGB},
	{BVDC_P_Output_eHDYPrPb,   s_aulSmTable_Tbl_HDYPrPb, BVDC_P_OutputFilter_eNone,      BVDC_P_OutputFilter_eHD,      NULL,                       s_aulHdVfTable_Tbl_HDYPrPb},
	{BVDC_P_Output_eHsync,     s_aulSmTable_Tbl_Hsync,   BVDC_P_OutputFilter_eHsync,     BVDC_P_OutputFilter_eHD,      s_aulSdVfTable_Tbl_Hsync,   s_aulHdVfTable_Tbl_Hsync},
	{BVDC_P_Output_eUnknown,   s_aulSmTable_Tbl_Unknown, BVDC_P_OutputFilter_eUnknown,   BVDC_P_OutputFilter_eHD,      s_aulSdVfTable_Tbl_Unknown, s_aulHdVfTable_Tbl_Unknown}
};

/* special handling for 480p and 576p at double pixel rate (HDMI) */
static const BVDC_P_ColorSpaceData s_aColorSpaceDataTable_54MHz[] =
{
	/* Format                  Sm Table Tbl              SD Filter Table Tbl             HD Filter Table Tbl           SD Vf Table Tbl             HD Vf Table Tbl */
	{BVDC_P_Output_eSDYPrPb,   s_aulSmTable_Tbl_SDYPrPb, BVDC_P_OutputFilter_eNone,      BVDC_P_OutputFilter_eSDYPrPb, NULL,                       s_aulHdVfTable_Tbl_54MHz_HDYPrPb},
	{BVDC_P_Output_eRGB,       s_aulSmTable_Tbl_RGB,     BVDC_P_OutputFilter_eNone,      BVDC_P_OutputFilter_eRGB,     NULL,                       s_aulHdVfTable_Tbl_54MHz_RGB},
	{BVDC_P_Output_eUnknown,   s_aulSmTable_Tbl_Unknown, BVDC_P_OutputFilter_eNone,      BVDC_P_OutputFilter_eUnknown, NULL,                       s_aulHdVfTable_Tbl_54MHz_Unknown}
};

/* For table size sanity check */
#define BVDC_P_DISPLAY_LEFTCUT_COUNT \
	(sizeof(s_astShaperSettings) / sizeof(BVDC_P_Display_ShaperSettings))

#define BVDC_P_DISPLAY_FMTINFO_COUNT \
	(sizeof(BVDC_P_aFormatInfoTable) / sizeof(BVDC_P_FormatInfo))

#define BVDC_P_DISPLAY_FMTDATA_COUNT \
	(sizeof(s_aFormatDataTable) / sizeof(BVDC_P_FormatData))

/* Pick color space data with colorspace search */
const BVDC_P_ColorSpaceData* BVDC_P_GetColorSpaceData(const BVDC_P_ColorSpaceData *paColorSpaceDataTable, BVDC_P_Output eOutputColorSpace)
{
	const BVDC_P_ColorSpaceData *paSpace = &paColorSpaceDataTable[0];
	while ((paSpace->eOutputColorSpace != eOutputColorSpace) &&
	       (paSpace->eOutputColorSpace != BVDC_P_Output_eUnknown))
	{
		++paSpace;
	}
	return paSpace;
}

/*************************************************************************
 *  {secret}
 *	Returns pointer to appropriate RamTable for display modes.
 **************************************************************************/
const uint32_t* BVDC_P_GetRamTable_isr
(
	const BVDC_P_DisplayInfo     *pDispInfo,
	bool bArib480p
)
{
	const uint32_t *pTable = NULL;

	/* check if table entry matches our format */
	BDBG_ASSERT(s_aFormatDataTable[pDispInfo->pFmtInfo->eVideoFmt].eVideoFmt ==
		pDispInfo->pFmtInfo->eVideoFmt);

#if BVDC_P_ORTHOGONAL_VEC
	/* Analog VEC, (480P or 576P) will always be double rate.
	 * Improves frequency response. */
	if(VIDEO_FORMAT_IS_480P(pDispInfo->pFmtInfo->eVideoFmt))
	{
		pTable =
			(bArib480p ?
				s_aulRamBVBInput_480pARIB_54MHz : s_aulRamBVBInput_480p_54MHz);
	}
	else if (BFMT_VideoFmt_e576p_50Hz      == pDispInfo->pFmtInfo->eVideoFmt )
	{
		pTable = s_aulRamBVBInput_576p_54MHz;
	}
#else
	/* Analog VEC, (480P or 576P) may be either single or double rate */
	if ((VIDEO_FORMAT_IS_480P(pDispInfo->pFmtInfo->eVideoFmt)) &&
	    (BAVC_HDMI_PixelRepetition_e2x == pDispInfo->eHdmiPixelRepetition))
	{
		pTable =
			(bArib480p ?
				s_aulRamBVBInput_480pARIB_54MHz : s_aulRamBVBInput_480p_54MHz);
	}
	else if(VIDEO_FORMAT_IS_480P(pDispInfo->pFmtInfo->eVideoFmt))
	{
		pTable =
			(bArib480p ?
				s_aulRamBVBInput_480pARIB : s_aulRamBVBInput_480p);
	}
	else if ((BFMT_VideoFmt_e576p_50Hz      == pDispInfo->pFmtInfo->eVideoFmt ) &&
			 (BAVC_HDMI_PixelRepetition_e2x == pDispInfo->eHdmiPixelRepetition))
	{
		pTable = s_aulRamBVBInput_576p_54MHz;
	}
#endif
	else if (
		pDispInfo->bWidthTrimmed &&
		VIDEO_FORMAT_IS_NTSC(pDispInfo->pFmtInfo->eVideoFmt))
	{
		BDBG_MSG(("NTSC width = 704 samples microcode"));
		pTable = s_aulRamBVBInput_NTSC_704;
	}
	else if(VIDEO_FORMAT_IS_NTSC(pDispInfo->pFmtInfo->eVideoFmt) &&
			 (true == bArib480p))
	{
		pTable = s_aulRamBVBInput_NTSCARIB;
	}
	else
	{
		/*
		 * Programming note: the following table supplies only 27 MHz
		 * microcodes for 480P and 576P. The table does not supply any ARIB
		 * microcodes. The table does not supply any "width trimmed"
		 * microcodes.
		 */
		pTable =
			s_aFormatDataTable[pDispInfo->pFmtInfo->eVideoFmt].pRamBVBInput;
	}

	return pTable;
}


/*************************************************************************
 *  {secret}
 *	Returns pointer to appropriate ItTable for display modes.
 **************************************************************************/
const uint32_t* BVDC_P_GetItTable_isr
(
	const BVDC_P_DisplayInfo     *pDispInfo
)
{
	const uint32_t *pTable = NULL;

	/* check if table entry matches our format */
	BDBG_ASSERT(s_aFormatDataTable[pDispInfo->pFmtInfo->eVideoFmt].eVideoFmt ==
		pDispInfo->pFmtInfo->eVideoFmt);

	pTable = s_aFormatDataTable[pDispInfo->pFmtInfo->eVideoFmt].pItTable;

	return pTable;
}


/*************************************************************************
 *  {secret}
 *	Returns pointer to appropriate ItConfig for display modes.
 **************************************************************************/
uint32_t BVDC_P_GetItConfig_isr
(
	const BVDC_P_DisplayInfo     *pDispInfo
)
{
	uint32_t ulItConfig=0;

	/* check if table entry matches our format */
	BDBG_ASSERT(s_aFormatDataTable[pDispInfo->pFmtInfo->eVideoFmt].eVideoFmt ==
		pDispInfo->pFmtInfo->eVideoFmt);

	/* Table size sanity check!  Just in case someone added new format in fmt,
	 * but forgot to add the new into these table. */
#if (BDBG_DEBUG_BUILD)
	if((BVDC_P_DISPLAY_LEFTCUT_COUNT != (BFMT_VideoFmt_eMaxCount+1)) ||
	   (BVDC_P_DISPLAY_FMTINFO_COUNT != (BFMT_VideoFmt_eMaxCount+1)) ||
	   (BVDC_P_DISPLAY_FMTDATA_COUNT != (BFMT_VideoFmt_eMaxCount+1)))
	{
		BDBG_ERR(("BVDC_P_DISPLAY_LEFTCUT_COUNT = %d", BVDC_P_DISPLAY_LEFTCUT_COUNT));
		BDBG_ERR(("BVDC_P_DISPLAY_FMTINFO_COUNT = %d", BVDC_P_DISPLAY_FMTINFO_COUNT));
		BDBG_ERR(("BVDC_P_DISPLAY_FMTDATA_COUNT = %d", BVDC_P_DISPLAY_FMTDATA_COUNT));
		BDBG_ASSERT(false);
	}
#endif

#if (BVDC_P_TEST_ORDER_CORRECTNESS)
	{
		BFMT_VideoFmt eFormat;
		for(eFormat = 0; eFormat < BFMT_VideoFmt_eMaxCount; eFormat++)
		{
			BDBG_ASSERT(s_aFormatDataTable[eFormat].eVideoFmt == eFormat);
			BDBG_ASSERT(BVDC_P_aFormatInfoTable[eFormat].eVideoFmt == eFormat);
		}
	}
#endif

	ulItConfig =
		*(s_aFormatDataTable[pDispInfo->pFmtInfo->eVideoFmt].pulItConfig);

	return ulItConfig;
}


/*************************************************************************
 *  {secret}
 *	Returns pointer to appropriate DtramTable for display modes.
 **************************************************************************/
const uint32_t* BVDC_P_GetDtramTable_isr
(
	const BVDC_P_DisplayInfo     *pDispInfo,
	const BFMT_VideoInfo         *pFmtInfo,
	bool                          bArib480p
)
{
	const uint32_t *pTable = NULL;
	uint32_t ulHdmiDropLines =
		(pFmtInfo->bInterlaced)                                 ?
		(pDispInfo->aulHdmiDropLines[pFmtInfo->eVideoFmt] /
			BVDC_P_FIELD_PER_FRAME)                                        :
		(pDispInfo->aulHdmiDropLines[pFmtInfo->eVideoFmt]);

	/* check if table entry matches our format */
	BDBG_ASSERT(s_aFormatDataTable[pFmtInfo->eVideoFmt].eVideoFmt ==
		pFmtInfo->eVideoFmt);

	/* Analog VEC, 480P may be either single or double rate */
	if((VIDEO_FORMAT_IS_480P(pFmtInfo->eVideoFmt)) &&
	   (bArib480p))
	{
		if ((BAVC_HDMI_PixelRepetition_e2x == pDispInfo->eHdmiPixelRepetition)||
			(BAVC_HDMI_PixelRepetition_e4x == pDispInfo->eHdmiPixelRepetition))
		{
			pTable = s_aulDtRamBVBInput_DVI_480p_Drop1_54MHz;
		}
		else
		{
			pTable = s_aulDtRamBVBInput_DVI_480p_Drop1;
		}
	}
	else if(VIDEO_FORMAT_IS_480P(pFmtInfo->eVideoFmt))
	{
		if ((BAVC_HDMI_PixelRepetition_e2x == pDispInfo->eHdmiPixelRepetition)||
			(BAVC_HDMI_PixelRepetition_e4x == pDispInfo->eHdmiPixelRepetition))
		{
			const uint32_t * const * apDropTbl =
				s_aulDtRamBVBInput_DVI_480p_54MHz_DropTbl;
			pTable = apDropTbl[ulHdmiDropLines];
		}
		else
		{
			const uint32_t * const * apDropTbl =
				s_aulDtRamBVBInput_DVI_480p_DropTbl;
			pTable = apDropTbl[ulHdmiDropLines];
		}
	}
	else if ((BFMT_VideoFmt_eCustom0 == pFmtInfo->eVideoFmt) ||
	         (BFMT_VideoFmt_eCustom1 == pFmtInfo->eVideoFmt) ||
	         (BFMT_VideoFmt_eCustom2 == pFmtInfo->eVideoFmt))
	{
		pTable = pFmtInfo->pCustomInfo->pDvoMicrocodeTbl;
	}
	/* Analog VEC, 576P may be either single or double rate */
	else if ((BFMT_VideoFmt_e576p_50Hz == pFmtInfo->eVideoFmt ) &&
			 ((BAVC_HDMI_PixelRepetition_e2x ==
			 	pDispInfo->eHdmiPixelRepetition) ||
			  (BAVC_HDMI_PixelRepetition_e4x ==
			 	pDispInfo->eHdmiPixelRepetition)) )
	{
		pTable = s_aulDtRamBVBInput_DVI_576p_54MHz;
	}
	else if (BVDC_P_aFormatInfoTable[pFmtInfo->eVideoFmt].bUseDropTbl)
	{
		const uint32_t * const * apDropTbl =
			s_aFormatDataTable[pFmtInfo->eVideoFmt].apDtRamBVBInput_DropTbl;
		pTable = apDropTbl[ulHdmiDropLines];
	}
	else
	{
		pTable = s_aFormatDataTable[pFmtInfo->eVideoFmt].pDtRamBVBInput;
	}

	BDBG_ASSERT(pTable);

	return pTable;
}


/*************************************************************************
 *  {secret}
 *	Returns pointer to appropriate 656Dtram for display modes.
 **************************************************************************/
const uint32_t* BVDC_P_Get656DtramTable_isr
(
	const BVDC_P_DisplayInfo     *pDispInfo
)
{
	const uint32_t *pTable = NULL;

	/* 656 only supports 480i and PAL */
	if (VIDEO_FORMAT_IS_525_LINES(pDispInfo->pFmtInfo->eVideoFmt))
	{
		pTable = &(s_aulDtRamBVBInput_656_NTSC[0]);
	}
	else if (VIDEO_FORMAT_IS_625_LINES(pDispInfo->pFmtInfo->eVideoFmt))
	{
		pTable = &(s_aulDtRamBVBInput_656_PAL[0]);
	}

	return pTable;
}


/*************************************************************************
 *  {secret}
 *	Returns pointer to appropriate SmTable for display modes.
 **************************************************************************/
const uint32_t* BVDC_P_GetSmTable_isr
(
	const BVDC_P_DisplayInfo     *pDispInfo,
	BVDC_P_Output			      eOutputCS
)
{
	const BVDC_P_SmTableInfo *pSmTable_Tbl = NULL;
	const uint32_t           *pTable = NULL;
	uint32_t                  ulCurSmTable = 0;

	/* to make coverity check silent */
	if (eOutputCS > BVDC_P_Output_eUnknown)
	{
		BDBG_ASSERT(eOutputCS <= BVDC_P_Output_eUnknown);
		return NULL;
	}

	pSmTable_Tbl = s_aColorSpaceDataTable[eOutputCS].pSmTable_Tbl;

	BDBG_ASSERT(pSmTable_Tbl);

	while(true)
	{
		if((pSmTable_Tbl[ulCurSmTable].eVideoFmt == pDispInfo->pFmtInfo->eVideoFmt) ||
		   (pSmTable_Tbl[ulCurSmTable].eVideoFmt == BFMT_VideoFmt_eMaxCount))
		{
			pTable = pSmTable_Tbl[ulCurSmTable].pSmTable;
			break;
		}

		ulCurSmTable++;
	}

	BDBG_ASSERT(pTable);

	return pTable;
}


/*************************************************************************
 *  {secret}
 *	Returns pointers to appropriate channel filters for display modes.
 **************************************************************************/
BERR_Code BVDC_P_GetChFilters_isr
(
	const BVDC_P_DisplayInfo *pDispInfo,
	BVDC_P_Output             eOutputColorSpace,
	const uint32_t          **ppChFilter_Ch0,
	const uint32_t          **ppChFilter_Ch1,
	const uint32_t          **ppChFilter_Ch2
)
{
	BVDC_P_OutputFilter           eOutputFilter;
	static const BVDC_P_ColorSpaceData *paSpace;

	/* to make coverity check silent */
	if (eOutputColorSpace > BVDC_P_Output_eUnknown)
	{
		BDBG_ASSERT(eOutputColorSpace <= BVDC_P_Output_eUnknown);
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

#if BVDC_P_ORTHOGONAL_VEC
	/* Analog VEC, (480P or 576P) will always be double rate.
	 * Improves frequency response. */
	if (VIDEO_FORMAT_IS_ED(pDispInfo->pFmtInfo->eVideoFmt))
#else
	/* Analog VEC, 480P may be either single or double rate */
	if ((VIDEO_FORMAT_IS_ED(pDispInfo->pFmtInfo->eVideoFmt)) &&
		(BAVC_HDMI_PixelRepetition_e2x == pDispInfo->eHdmiPixelRepetition))
#endif
	{
		paSpace =
			BVDC_P_GetColorSpaceData(
				s_aColorSpaceDataTable_54MHz, eOutputColorSpace);
	}
	else
	{
		paSpace =
			BVDC_P_GetColorSpaceData(
				s_aColorSpaceDataTable, eOutputColorSpace);
	}

	if (VIDEO_FORMAT_IS_HD(pDispInfo->pFmtInfo->eVideoFmt))
	{
		eOutputFilter = paSpace->eHdOutputFilter;
	}
	else
	{
		eOutputFilter = paSpace->eSdOutputFilter;
	}

	BDBG_ASSERT(
		s_aulFilterTable_Tbl[eOutputFilter].eOutputFilter == eOutputFilter);

	*ppChFilter_Ch0 = s_aulFilterTable_Tbl[eOutputFilter].pChFilter_Ch0;
	*ppChFilter_Ch1 = s_aulFilterTable_Tbl[eOutputFilter].pChFilter_Ch1;
	*ppChFilter_Ch2 = s_aulFilterTable_Tbl[eOutputFilter].pChFilter_Ch2;

	BDBG_ASSERT(*ppChFilter_Ch0);
	BDBG_ASSERT(*ppChFilter_Ch1);
	BDBG_ASSERT(*ppChFilter_Ch2);

	return BERR_SUCCESS;
}


/*************************************************************************
 *  {secret}
 *	Returns appropriate SrcControl for display modes.
 **************************************************************************/
uint32_t BVDC_P_GetSrcControl_isr
(
	const BVDC_P_DisplayInfo     *pDispInfo
)
{
	BSTD_UNUSED(pDispInfo);
	return s_ulSrcControl;
}


/*************************************************************************
 *  {secret}
 *	Returns pointer to appropriate VfTable for display modes.
 **************************************************************************/
void BVDC_P_GetVfTable_isr
(
	const BVDC_P_DisplayInfo *pDispInfo,
	BVDC_P_Output             eOutputColorSpace,
	const uint32_t          **ppTable,
	BVDC_P_Display_ShaperSettings *pstShaperSettings
)
{
	const BVDC_P_VfTableInfo *pVfTable_Tbl = NULL;
	uint32_t                  ulCurVfTable = 0;
	static const BVDC_P_ColorSpaceData *paSpace;

	/* to make coverity check silent */
	if (eOutputColorSpace > BVDC_P_Output_eUnknown)
	{
		BDBG_ASSERT(eOutputColorSpace <= BVDC_P_Output_eUnknown);
		return;
	}

#if BVDC_P_ORTHOGONAL_VEC
	/* Analog VEC, (480P or 576P) will always be double rate. Improves
	 * frequency response. */
	if (VIDEO_FORMAT_IS_ED(pDispInfo->pFmtInfo->eVideoFmt))
#else
	/* Analog VEC, 480P may be either single or double rate */
	if ((VIDEO_FORMAT_IS_ED(pDispInfo->pFmtInfo->eVideoFmt)) &&
	    (BAVC_HDMI_PixelRepetition_e2x == pDispInfo->eHdmiPixelRepetition))
#endif
	{
		paSpace =
			BVDC_P_GetColorSpaceData(
				s_aColorSpaceDataTable_54MHz, eOutputColorSpace);
	}
	else
	{
		paSpace =
			BVDC_P_GetColorSpaceData(
				s_aColorSpaceDataTable, eOutputColorSpace);
	}

	if(VIDEO_FORMAT_IS_HD(pDispInfo->pFmtInfo->eVideoFmt))
	{
		pVfTable_Tbl = paSpace->pHdVfTable_Tbl;
	}
	else
	{
		pVfTable_Tbl = paSpace->pSdVfTable_Tbl;
	}
	BDBG_ASSERT(pVfTable_Tbl);

	/* Extract vf table */
	if(ppTable)
	{
		while(true)
		{
			if((pVfTable_Tbl[ulCurVfTable].eVideoFmt ==
					pDispInfo->pFmtInfo->eVideoFmt) ||
			   (pVfTable_Tbl[ulCurVfTable].eVideoFmt ==
			   		BFMT_VideoFmt_eMaxCount))
			{
				*ppTable = pVfTable_Tbl[ulCurVfTable].pVfTable;
				break;
			}
			ulCurVfTable++;
		}
	}

	/* Extract vf left cut as well */
	if(pstShaperSettings)
	{
		*pstShaperSettings = s_astShaperSettings[pDispInfo->pFmtInfo->eVideoFmt];
		if(pDispInfo->bWidthTrimmed && VIDEO_FORMAT_IS_NTSC(pDispInfo->pFmtInfo->eVideoFmt) &&
			(pDispInfo->eMacrovisionType < BVDC_MacrovisionType_eTest01))
		{
			BDBG_MSG(("NTSC width = 704 samples, left cut 8 pixels"));
			pstShaperSettings->ulSavRemove = 8;
		}
	}

	return;
}

#if BVDC_P_ORTHOGONAL_VEC
/*************************************************************************
 *  {secret}
 *	Returns VF envelop generator control settings
 **************************************************************************/
uint32_t BVDC_P_GetVfEnvelopGenerator_isr
(
	const BVDC_P_DisplayInfo *pDispInfo,
	BVDC_P_Output			  eOutputCS
)
{
	const BVDC_P_EnvelopGeneratorSetting *pEG_Tbl = NULL;
	uint32_t    ulEGSetting;
	uint32_t    ulEG_idx = 0;

	/* to make coverity check silent */
	if (eOutputCS > BVDC_P_Output_eUnknown)
	{
		BDBG_ASSERT(eOutputCS <= BVDC_P_Output_eUnknown);
		return 0;
	}

	pEG_Tbl = s_aEnvelopGeneratorTable[eOutputCS].pEG_Tbl;

	BDBG_ASSERT(pEG_Tbl);

	while(true)
	{
		if((pEG_Tbl[ulEG_idx].eVideoFmt == pDispInfo->pFmtInfo->eVideoFmt) ||
		   (pEG_Tbl[ulEG_idx].eVideoFmt == BFMT_VideoFmt_eMaxCount))
		{
			ulEGSetting = pEG_Tbl[ulEG_idx].ulSetting;
			break;
		}

		ulEG_idx++;
	}

	return ulEGSetting;
}
#endif

/*************************************************************************
 *  {secret}
 *	Finds appropriate RmTable and frequency info for display modes.
 **************************************************************************/
static void BVDC_P_SelectRmTable_isr
(
	const BVDC_P_DisplayInfo  *pDispInfo,
	const BFMT_VideoInfo      *pFmtInfo,
	const BVDC_P_RmTableInfo **pRmTable_Tbl,
	bool                       bFullRate,
	uint32_t                  *pulRmTable_TblSize,
	uint32_t                  *pulVertRefreshRate
)
{
	if(pDispInfo->bMultiRateAllow)
	{
		if(( bFullRate) &&
		   (!pDispInfo->bForceFrameDrop))
		{
			*pRmTable_Tbl = s_aRmFullRate_Tbl;
			*pulRmTable_TblSize = BVDC_P_FULLRATE_TBL_SIZE;
			*pulVertRefreshRate = pFmtInfo->ulVertFreq;
		}
		else
		{
			*pRmTable_Tbl = s_aRmDropRate_Tbl;
			*pulRmTable_TblSize = BVDC_P_DROPRATE_TBL_SIZE;
			*pulVertRefreshRate = (pFmtInfo->ulVertFreq * 1000) / 1001;
		}
	}
	else
	{
		/* Single vertical refresh rate. */
		if(!VIDEO_FORMAT_IS_SD(pFmtInfo->eVideoFmt) &&
		   !VIDEO_FORMAT_IS_ED(pFmtInfo->eVideoFmt))
		{
			*pRmTable_Tbl = s_aRmFullRate_Tbl;
			*pulRmTable_TblSize = BVDC_P_FULLRATE_TBL_SIZE;
			*pulVertRefreshRate = pFmtInfo->ulVertFreq;
		}
		else
		{
			*pRmTable_Tbl = s_aRmDropRate_Tbl;
			*pulRmTable_TblSize = BVDC_P_DROPRATE_TBL_SIZE;
			if(VIDEO_FORMAT_IS_525_LINES(pFmtInfo->eVideoFmt))
			{
				*pulVertRefreshRate = (pFmtInfo->ulVertFreq * 1000) / 1001;
			}
			else
			{
				*pulVertRefreshRate = pFmtInfo->ulVertFreq;
			}
		}
	}
	return;
}

BERR_Code BVDC_P_GetRmTable_isr
(
	const BVDC_P_DisplayInfo *pDispInfo,
	const BFMT_VideoInfo     *pFmtInfo,
	const uint32_t          **ppTable,
	bool                      bFullRate,
	BAVC_VdcDisplay_Info     *pRateInfo
)
{
	uint32_t                  ulPxlFreqMask;
	const BVDC_P_RmTableInfo *pRmTable_Tbl=NULL;
	const uint32_t           *pTable = NULL;
	BAVC_VdcDisplay_Info      lRateInfo;
	BERR_Code                 eErr = BVDC_ERR_UNSUPPORTED_PIXEL_RATE;
	uint32_t                  ulRmTable_TblSize = 0;
	uint32_t                  ulCurRmTable = 0;

	/* Select pxl freq.  Need a way to notify HDMI that our pixel rate has
	 * changed.  Handle by callback */
	BVDC_P_SelectRmTable_isr(pDispInfo, pFmtInfo, &pRmTable_Tbl, bFullRate,
		&ulRmTable_TblSize, &lRateInfo.ulVertRefreshRate);
	ulPxlFreqMask = pFmtInfo->ulPxlFreqMask;

#if BVDC_P_ORTHOGONAL_VEC
	/* For (480P or 576P), operate the analog VEC at double rate.
	 * Improves frequency response. */
	if ((BFMT_VideoFmt_e480p      == pFmtInfo->eVideoFmt) ||
	    (BFMT_VideoFmt_e576p_50Hz == pFmtInfo->eVideoFmt))
#else
	/* This is only to support (480P or 576P) with 54 MHz pixel rate */
	if (((BFMT_VideoFmt_e480p      == pFmtInfo->eVideoFmt) ||
	     (BFMT_VideoFmt_e576p_50Hz == pFmtInfo->eVideoFmt))
	&&
	   (BAVC_HDMI_PixelRepetition_e2x == pDispInfo->eHdmiPixelRepetition))
#endif
	{
		if (ulPxlFreqMask & BFMT_PXL_27MHz)
		{
			ulPxlFreqMask &= ~BFMT_PXL_27MHz;
			ulPxlFreqMask |=  BFMT_PXL_54MHz;
		}
		if (ulPxlFreqMask & BFMT_PXL_27MHz_MUL_1_001)
		{
			ulPxlFreqMask &= ~BFMT_PXL_27MHz_MUL_1_001;
			ulPxlFreqMask |=  BFMT_PXL_54MHz_MUL_1_001;
		}
	}


	/* find and get correct rmtable and pixel clock rate */
	for (ulCurRmTable = 0; ulCurRmTable < ulRmTable_TblSize; ulCurRmTable++)
	{
		if (pRmTable_Tbl[ulCurRmTable].ulPixelClkRate & ulPxlFreqMask)
		{
			pTable = pRmTable_Tbl[ulCurRmTable].pRmTable;
			lRateInfo.ulPixelClkRate =
				pRmTable_Tbl[ulCurRmTable].ulPixelClkRate;
			break;
		}
	}

	if (pTable)
	{
		*ppTable = pTable;
		*pRateInfo = lRateInfo;
		eErr = BERR_SUCCESS;
	}

	return eErr;
}

/*************************************************************************
 *  {secret}
 *	Returns pointer to appropriate RmString name for display modes.
 **************************************************************************/
const char* BVDC_P_GetRmString_isr
(
	const BVDC_P_DisplayInfo     *pDispInfo,
	const BFMT_VideoInfo         *pFmtInfo
)
{
	const BVDC_P_RmTableInfo *pRmTable_Tbl=NULL;
	uint32_t                  ulRmTable_TblSize = 0;
	uint32_t                  ulCurRmTable = 0;
	uint32_t                  ulVertRefreshRate = 0;

	/* Select pxl freq.  Need a way to notify HDMI that our pixel rate has
	 * changed.  Handle by callback */

	/* find rm and return its text string */
	BVDC_P_SelectRmTable_isr(pDispInfo, pFmtInfo, &pRmTable_Tbl, pDispInfo->bFullRate,
		&ulRmTable_TblSize, &ulVertRefreshRate);

	/* find and get correct rmtable and pixel clock rate */
	for (ulCurRmTable = 0; ulCurRmTable < ulRmTable_TblSize; ulCurRmTable++)
	{
		if (pRmTable_Tbl[ulCurRmTable].ulPixelClkRate == pDispInfo->stRateInfo.ulPixelClkRate)
		{
			return pRmTable_Tbl[ulCurRmTable].pString;
		}
	}

	return "Unknown-";
}


/*************************************************************************
 *
 *
 */
#if BVDC_P_ORTHOGONAL_VEC
/* HDMI color depth support */
const BVDC_P_RateInfo* BVDC_P_HdmiRmTable_isr
(
	BFMT_VideoFmt             eVideoFmt,
	uint32_t                  ulPixelClkRate,
	BAVC_HDMI_BitsPerPixel    eHdmiColorDepth,
	BAVC_HDMI_PixelRepetition eHdmiPixelRepetition
)
{
	uint32_t i;
	BAVC_HDMI_PixelRepetition ePxlRep = eHdmiPixelRepetition;

	/* for 480i/576i Pixel Repetition 2x is inherent from VEC so use the
	 * same RM as BAVC_HDMI_PixelRepetition_eNone */
	if(VIDEO_FORMAT_IS_NTSC(eVideoFmt) || VIDEO_FORMAT_IS_PAL(eVideoFmt))
	{
		if(eHdmiPixelRepetition == BAVC_HDMI_PixelRepetition_e2x)
		{
			BDBG_MSG(("VideoFmt %d PxlRep e2x => use PxlRep eNone", eVideoFmt));
			ePxlRep = BAVC_HDMI_PixelRepetition_eNone;
		}
	}
	else if((eVideoFmt != BFMT_VideoFmt_e480p) &&
			(eVideoFmt != BFMT_VideoFmt_e576p_50Hz))
	{
		if(eHdmiPixelRepetition != BAVC_HDMI_PixelRepetition_eNone)
		{
			BDBG_MSG(("VideoFmt %d PxlRep %d => use PxlRep eNone", eVideoFmt, eHdmiPixelRepetition));
			ePxlRep = BAVC_HDMI_PixelRepetition_eNone;
		}
	}

	for(i = 0; i < BVDC_P_RM_TABLE_ENTRIES; i++)
	{
		if((s_HdmiRm[i].ulPixelClkRate       ==  ulPixelClkRate) &&
		   (s_HdmiRm[i].eHdmiColorDepth      == eHdmiColorDepth) &&
		   (s_HdmiRm[i].eHdmiPixelRepetition ==         ePxlRep)   )
		{
			BDBG_MSG(("PxlClkRate 0x%x, HDMI Color Depth %d, Pxl Repetition %d => %sMHz",
				ulPixelClkRate, eHdmiColorDepth, eHdmiPixelRepetition,
				s_HdmiRm[i].pchRate));
			return &s_HdmiRm[i];
		}
	}

	BDBG_ERR(("PxlClkRate 0x%x, HDMI Color Depth %d, Pxl Repetition %d unsupported",
		ulPixelClkRate, eHdmiColorDepth, eHdmiPixelRepetition));
	return NULL;
}

#else
const BVDC_P_RateInfo* BVDC_P_HdmiRmTable_isr
(
	BFMT_VideoFmt             eVideoFmt,
	uint32_t                  ulPixelClkRate,
	BAVC_HDMI_BitsPerPixel    eHdmiColorDepth,
	BAVC_HDMI_PixelRepetition eHdmiPixelRepetition
)
{
	uint32_t i;

	BSTD_UNUSED(eVideoFmt);
	BSTD_UNUSED(eHdmiColorDepth);
	BSTD_UNUSED(eHdmiPixelRepetition);

	for(i = 0; i < BVDC_P_RM_TABLE_ENTRIES; i++)
	{
		if(s_HdmiRm[i].ulPixelClkRate == ulPixelClkRate)
		{
			return &s_HdmiRm[i];
		}
	}
	return NULL;
}
#endif

/* H, then V */
const uint32_t* BVDC_P_GetDviDtgToggles_isr
(
	const BVDC_P_DisplayInfo *pDispInfo
)
{
	static const uint32_t s_zeroes[2] = {0, 0};
	static const uint32_t s_ones[2] = {1, 1};
	static const uint32_t s_mixed[2] = {1, 0};

	const uint32_t* retval;
	BFMT_VideoFmt eFmt = pDispInfo->pFmtInfo->eVideoFmt;

	/* A rule for NTSC, PAL, and SECAM */
	if (VIDEO_FORMAT_IS_525_LINES(eFmt) || VIDEO_FORMAT_IS_625_LINES(eFmt) ||
		(BFMT_VideoFmt_e480p == eFmt) || (BFMT_VideoFmt_e576p_50Hz == eFmt))
	{
		retval = s_ones;
	}
	else
	{
		/* Individual rules for some VESA formats */
		switch (eFmt)
		{
		case BFMT_VideoFmt_eDVI_1024x768p:
		case BFMT_VideoFmt_eDVI_640x480p_CVT:
		case BFMT_VideoFmt_eDVI_640x480p:
			retval = s_ones;
			break;
		case BFMT_VideoFmt_eDVI_1280x720p:
		case BFMT_VideoFmt_eDVI_1280x720p_Red:
		case BFMT_VideoFmt_eDVI_1440x900p_60Hz:
			retval = s_mixed;
			break;
		/* Everything else */
		default:
			retval = s_zeroes;
			break;
		}
	}

	return retval;
}

/*************************************************************************
 *
 *
 */
const BVDC_P_FormatInfo* BVDC_P_GetFormatInfoPtr_isr
(
	const BFMT_VideoInfo    *pFmtInfo
)
{
	BDBG_ASSERT(pFmtInfo->eVideoFmt < BFMT_VideoFmt_eMaxCount);
	return &BVDC_P_aFormatInfoTable[pFmtInfo->eVideoFmt];
}


/***************************************************************************
 *
 * Utility function called by BVDC_Display_GetCapabilities
 */
bool  BVDC_P_IsVidfmtSupported
	( BFMT_VideoFmt                    eVideoFmt)
{
	BDBG_ASSERT(eVideoFmt < BFMT_VideoFmt_eMaxCount);

#if !BVDC_P_SUPPORT_VEC_SECAM
	if(VIDEO_FORMAT_IS_SECAM(eVideoFmt))
	{
		return false;
	}
#endif

#if !BVDC_P_SUPPORT_1080p_60HZ
	if((eVideoFmt == BFMT_VideoFmt_e1080p) ||
	   (eVideoFmt == BFMT_VideoFmt_e1080p_50Hz))
	{
		return false;
	}
#endif

	/* No ucodes */
	if((!s_aFormatDataTable[eVideoFmt].pRamBVBInput) &&
	   (!s_aFormatDataTable[eVideoFmt].pDtRamBVBInput))
	{
		return false;
	}

	return true;

}


/*************************************************************************
 *
 *
 */
#define BVDC_P_DISPLAY_DUMP
#ifdef BVDC_P_DISPLAY_DUMP /** { **/

void BVDC_P_Display_Dump_aulVfTable (const char* name, const uint32_t* table)
{
	BKNI_Printf ("//%s %d\n", name, BVDC_P_VF_TABLE_SIZE+1);

	BKNI_Printf ("//%s %08x\n", "FORMAT_ADDER",       table[0]);
	BKNI_Printf ("//%s %08x\n", "VF_MISC",            table[1]);
	BKNI_Printf ("//%s %08x\n", "VF_NEG_SYNC_VALUES", table[2]);
	BKNI_Printf ("//%s %08x\n", "VF_POS_SYNC_VALUES", table[3]);
	BKNI_Printf ("//%s %08x\n", "VF_SYNC_TRANS_0",    table[4]);
	BKNI_Printf ("//%s %08x\n", "VF_SYNC_TRANS_1",    table[5]);
	BKNI_Printf ("//%s %08x\n", "VF_NEG_SYNC_EXTN",   table[6]);
}

void BVDC_Display_DumpAll_aulVfTable (void)
{
	BVDC_P_Display_Dump_aulVfTable (
		"s_aulVfTable_YQI", s_aulVfTable_YQI);
	BVDC_P_Display_Dump_aulVfTable (
		"s_aulVfTable_YUV", s_aulVfTable_YUV);
	BVDC_P_Display_Dump_aulVfTable (
		"s_aulVfTable_SD_YPbPr", s_aulVfTable_SD_YPbPr);
	BVDC_P_Display_Dump_aulVfTable (
		"s_aulVfTable_SD_YPbPr_50Hz", s_aulVfTable_SD_YPbPr_50Hz);
	BVDC_P_Display_Dump_aulVfTable (
		"s_aulVfTable_SD_RGB", s_aulVfTable_SD_RGB);
	BVDC_P_Display_Dump_aulVfTable (
		"s_aulVfTable_SD_RGB_50Hz", s_aulVfTable_SD_RGB_50Hz);
	BVDC_P_Display_Dump_aulVfTable (
		"s_aulVfTable_SD_Hsync", s_aulVfTable_SD_Hsync);
	BVDC_P_Display_Dump_aulVfTable (
		"s_aulVfTable_1080i_720p_YPbPr", s_aulVfTable_1080i_720p_YPbPr);
	BVDC_P_Display_Dump_aulVfTable (
		"s_aulVfTable_1080i_720p_RGB", s_aulVfTable_1080i_720p_RGB);
	BVDC_P_Display_Dump_aulVfTable (
		"s_aulVfTable_HD_Hsync", s_aulVfTable_HD_Hsync);
	BVDC_P_Display_Dump_aulVfTable (
		"s_aulVfTable_480p_YPbPr", s_aulVfTable_480p_YPbPr);
	BVDC_P_Display_Dump_aulVfTable (
		"s_aulVfTable_576p_YPbPr", s_aulVfTable_576p_YPbPr);
	BVDC_P_Display_Dump_aulVfTable (
		"s_aulVfTable_576p_54MHz_YPbPr", s_aulVfTable_576p_54MHz_YPbPr);
	BVDC_P_Display_Dump_aulVfTable (
		"s_aulVfTable_480p_RGB", s_aulVfTable_480p_RGB);
	BVDC_P_Display_Dump_aulVfTable (
		"s_aulVfTable_480p_54Mhz_YPbPr", s_aulVfTable_480p_54MHz_YPbPr);
	BVDC_P_Display_Dump_aulVfTable (
		"s_aulVfTable_480p_54MHz_RGB", s_aulVfTable_480p_54MHz_RGB);
}

void BVDC_P_Display_Dump_aulChFilterTbl (
	const char* name, const uint32_t* table)
{
	BKNI_Printf ("//%s %d\n", name, BVDC_P_CHROMA_TABLE_SIZE);

	BKNI_Printf ("//%s %08x\n", "PRIM_VF_CH2_TAP1_3", table[0]);
	BKNI_Printf ("//%s %08x\n", "PRIM_VF_CH2_TAP4_5", table[1]);
	BKNI_Printf ("//%s %08x\n", "PRIM_VF_CH2_TAP6_7", table[2]);
	BKNI_Printf ("//%s %08x\n", "PRIM_VF_CH2_TAP8_9", table[3]);
	BKNI_Printf ("//%s %08x\n", "PRIM_VF_CH2_TAP10",  table[4]);
}

void BVDC_Display_DumpAll_aulChFilterTbl (void)
{
	BVDC_P_Display_Dump_aulChFilterTbl (
		"s_aulChFilterTbl_AllPass", s_aulChFilterTbl_AllPass);
	BVDC_P_Display_Dump_aulChFilterTbl (
		"s_aulChFilterTbl_Cr_06Mhz", s_aulChFilterTbl_Cr_06Mhz);
	BVDC_P_Display_Dump_aulChFilterTbl (
		"s_aulChFilterTbl_Cr_13Mhz", s_aulChFilterTbl_Cr_13Mhz);
	BVDC_P_Display_Dump_aulChFilterTbl (
		"s_aulChFilterTbl_Cr_3375Mhz", s_aulChFilterTbl_Cr_3375Mhz);
	BVDC_P_Display_Dump_aulChFilterTbl (
		"s_aulChFilterTbl_Y_50Mhz", s_aulChFilterTbl_Y_50Mhz);
	BVDC_P_Display_Dump_aulChFilterTbl (
		"s_aulChFilterTbl_Y_60Mhz", s_aulChFilterTbl_Y_60Mhz);
	BVDC_P_Display_Dump_aulChFilterTbl (
		"s_aulChFilterTbl_Y_675Mhz", s_aulChFilterTbl_Y_675Mhz);
	BVDC_P_Display_Dump_aulChFilterTbl (
		"s_aulChFilterTbl_Cr_675Mhz", s_aulChFilterTbl_Cr_675Mhz);
#if BVDC_P_ORTHOGONAL_VEC
	BVDC_P_Display_Dump_aulChFilterTbl (
		"s_aulChFilterTbl_Y_42Mhz", s_aulChFilterTbl_Y_42Mhz);
	BVDC_P_Display_Dump_aulChFilterTbl (
		"s_aulChFilterTbl_Y_55Mhz", s_aulChFilterTbl_Y_55Mhz);
	BVDC_P_Display_Dump_aulChFilterTbl (
		"s_aulChFilterTbl_Cr_1856Mhz", s_aulChFilterTbl_Cr_1856Mhz);
	BVDC_P_Display_Dump_aulChFilterTbl (
		"s_aulChFilterTbl_Y_12Mhz", s_aulChFilterTbl_Y_12Mhz);
	BVDC_P_Display_Dump_aulChFilterTbl (
		"s_aulChFilterTbl_Cr_SD", s_aulChFilterTbl_Cr_SD);
	BVDC_P_Display_Dump_aulChFilterTbl (
		"s_aulChFilterTbl_Cr_SECAM", s_aulChFilterTbl_Cr_SECAM);
#endif
}

void BVDC_P_Display_Dump_aulRmTable (const char* name, const uint32_t* table)
{
	BKNI_Printf ("//%s %d\n", name, BVDC_P_RM_TABLE_SIZE);

	BKNI_Printf ("//%s %08x\n", "PRIM_RM_RATE_RATIO", table[0]);
	BKNI_Printf ("//%s %08x\n", "PRIM_RM_PHASE_INC",  table[1]);
	BKNI_Printf ("//%s %08x\n", "PRIM_RM_INTEGRATOR", table[2]);
}

void BVDC_Display_DumpAll_aulRmTable (void)
{
	BVDC_P_Display_Dump_aulRmTable ("s_aulRmTable_25_2", s_aulRmTable_25_2);
	BVDC_P_Display_Dump_aulRmTable (
		"s_aulRmTable_25_2_Div_1001", s_aulRmTable_25_2_Div_1001);
	BVDC_P_Display_Dump_aulRmTable ("s_aulRmTable_27", s_aulRmTable_27);
	BVDC_P_Display_Dump_aulRmTable (
		"s_aulRmTable_27_Mul_1001", s_aulRmTable_27_Mul_1001);
	BVDC_P_Display_Dump_aulRmTable ("s_aulRmTable_39_79", s_aulRmTable_39_79);
	BVDC_P_Display_Dump_aulRmTable (
		"s_aulRmTable_39_79_Div_1001", s_aulRmTable_39_79_Div_1001);
	BVDC_P_Display_Dump_aulRmTable (
		"s_aulRmTable_60_4656", s_aulRmTable_60_4656);
	BVDC_P_Display_Dump_aulRmTable (
		"s_aulRmTable_64_0224", s_aulRmTable_64_0224);
	BVDC_P_Display_Dump_aulRmTable (
		"s_aulRmTable_64_0224_Div_1001", s_aulRmTable_64_0224_Div_1001);
	BVDC_P_Display_Dump_aulRmTable ("s_aulRmTable_65", s_aulRmTable_65);
	BVDC_P_Display_Dump_aulRmTable (
		"s_aulRmTable_65_Div_1001", s_aulRmTable_65_Div_1001);
	BVDC_P_Display_Dump_aulRmTable ("s_aulRmTable_65_286", s_aulRmTable_65_286);
	BVDC_P_Display_Dump_aulRmTable (
		"s_aulRmTable_65_286_Div_1001", s_aulRmTable_65_286_Div_1001);
	BVDC_P_Display_Dump_aulRmTable ("s_aulRmTable_74_25", s_aulRmTable_74_25);
	BVDC_P_Display_Dump_aulRmTable (
		"s_aulRmTable_74_25_Div_1001", s_aulRmTable_74_25_Div_1001);
	BVDC_P_Display_Dump_aulRmTable ("s_aulRmTable_74_48", s_aulRmTable_74_48);
	BVDC_P_Display_Dump_aulRmTable (
		"s_aulRmTable_74_48_Div_1001", s_aulRmTable_74_48_Div_1001);
	BVDC_P_Display_Dump_aulRmTable ("s_aulRmTable_148_5", s_aulRmTable_148_5);
	BVDC_P_Display_Dump_aulRmTable (
		"s_aulRmTable_148_5_Div_1001", s_aulRmTable_148_5_Div_1001);
	BVDC_P_Display_Dump_aulRmTable ("s_aulRmTable_162", s_aulRmTable_162);
	BVDC_P_Display_Dump_aulRmTable ("s_aulRmTable_67_565", s_aulRmTable_67_565);
	BVDC_P_Display_Dump_aulRmTable (
		"s_aulRmTable_67_565_Div_1001", s_aulRmTable_67_565_Div_1001);
	BVDC_P_Display_Dump_aulRmTable ("s_aulRmTable_56_304", s_aulRmTable_56_304);
}

void BVDC_P_Display_Dump_aulItTable (const char* name, const uint32_t* table)
{
	BKNI_Printf ("//%s %d\n", name, BVDC_P_IT_TABLE_SIZE);

	BKNI_Printf ("//%s %08x\n", "PRIM_IT_ADDR_0_3",        table[0]);
	BKNI_Printf ("//%s %08x\n", "PRIM_IT_ADDR_4_6",        table[1]);
	BKNI_Printf ("//%s %08x\n", "PRIM_IT_STACK_reg_0_1",   table[2]);
	BKNI_Printf ("//%s %08x\n", "PRIM_IT_STACK_reg_2_3",   table[3]);
	BKNI_Printf ("//%s %08x\n", "PRIM_IT_STACK_reg_4_5",   table[4]);
	BKNI_Printf ("//%s %08x\n", "PRIM_IT_STACK_reg_6_7",   table[5]);
	BKNI_Printf ("//%s %08x\n", "PRIM_IT_EVENT_SELECTION", table[6]);
	BKNI_Printf ("//%s %08x\n", "PRIM_IT_PCL_0",           table[7]);
	BKNI_Printf ("//%s %08x\n", "PRIM_IT_PCL_1",           table[8]);
	BKNI_Printf ("//%s %08x\n", "PRIM_IT_PCL_2",           table[9]);
	BKNI_Printf ("//%s %08x\n", "PRIM_IT_PCL_3",           table[10]);
	BKNI_Printf ("//%s %08x\n", "PRIM_IT_PCL_4",           table[11]);
#if (BVDC_P_SUPPORT_IT_VER >= 1)
	BKNI_Printf ("//%s %08x\n", "PRIM_IT_PCL_5",           table[12]);
	BKNI_Printf ("//%s %08x\n", "PRIM_IT_PCL_6",           table[13]);
	BKNI_Printf ("//%s %08x\n", "PRIM_IT_PCL_7",           table[14]);
	BKNI_Printf ("//%s %08x\n", "PRIM_IT_PCL_8",           table[15]);
	BKNI_Printf ("//%s %08x\n", "PRIM_IT_STACK_reg_8_9",   table[16]);
#endif
}

void BVDC_Display_DumpAll_aulItTable (void)
{
	BVDC_P_Display_Dump_aulItTable ("s_aulItTable_1080p", s_aulItTable_1080p);
	BVDC_P_Display_Dump_aulItTable ("s_aulItTable_1080i", s_aulItTable_1080i);
	BVDC_P_Display_Dump_aulItTable ("s_aulItTable_720p", s_aulItTable_720p);
	BVDC_P_Display_Dump_aulItTable (
		"s_aulItTable_720p_24hz", s_aulItTable_720p_24hz);
	BVDC_P_Display_Dump_aulItTable (
		"s_aulItTable_720p_50hz", s_aulItTable_720p_50hz);
	BVDC_P_Display_Dump_aulItTable ("s_aulItTable_480p", s_aulItTable_480p);
	BVDC_P_Display_Dump_aulItTable (
		"s_aulItTable_1280x720p", s_aulItTable_1280x720p);
	BVDC_P_Display_Dump_aulItTable (
		"s_aulItTable_1600x1200p_60Hz", s_aulItTable_1600x1200p_60Hz);
	BVDC_P_Display_Dump_aulItTable (
		"s_aulItTable_CUSTOM_1366x768p", s_aulItTable_CUSTOM_1366x768p);
}

void BVDC_P_Display_Dump_ulItConfig (const char* name, uint32_t value)
{
	BKNI_Printf ("//%s %d\n", name, 1);
	BKNI_Printf ("//%s %08x\n", "PRIM_IT_TG_CONFIG", value);
}

void BVDC_Display_DumpAll_ulItConfig (void)
{
	BVDC_P_Display_Dump_ulItConfig ("s_ulItConfig_NTSC", s_ulItConfig_NTSC);
	BVDC_P_Display_Dump_ulItConfig ("s_ulItConfig_PALM", s_ulItConfig_PALM);
	BVDC_P_Display_Dump_ulItConfig ("s_ulItConfig_PAL", s_ulItConfig_PAL);
	BVDC_P_Display_Dump_ulItConfig ("s_ulItConfig_SECAM", s_ulItConfig_SECAM);
	BVDC_P_Display_Dump_ulItConfig ("s_ulItConfig_1080p", s_ulItConfig_1080p);
	BVDC_P_Display_Dump_ulItConfig ("s_ulItConfig_1080i", s_ulItConfig_1080i);
	BVDC_P_Display_Dump_ulItConfig ("s_ulItConfig_720p", s_ulItConfig_720p);
	BVDC_P_Display_Dump_ulItConfig ("s_ulItConfig_480p", s_ulItConfig_480p);
	BVDC_P_Display_Dump_ulItConfig (
		"s_ulItConfig_CUSTOM_1366x768p", s_ulItConfig_CUSTOM_1366x768p);
}

void BVDC_P_Display_Dump_aulSmTable (const char* name, const uint32_t* table)
{
	BKNI_Printf ("//%s %d\n", name, BVDC_P_SM_TABLE_SIZE);

#if !BVDC_P_ORTHOGONAL_VEC
	BKNI_Printf ("//%s %08x\n", "PRIM_SM_ENVELOPE_GENERATOR", *table++);
#endif
	BKNI_Printf ("//%s %08x\n", "PRIM_SM_PG_CNTRL",           *table++);
	BKNI_Printf ("//%s %08x\n", "PRIM_SM_PG_CONFIG",          *table++);
	BKNI_Printf ("//%s %08x\n", "PRIM_SM_SC_FREQ_0",          *table++);
	BKNI_Printf ("//%s %08x\n", "PRIM_SM_COMP_CNTRL",         *table++);
}

void BVDC_Display_DumpAll_aulSmTable (void)
{
	BVDC_P_Display_Dump_aulSmTable ("s_aulSmTable_YQI", s_aulSmTable_YQI);
	BVDC_P_Display_Dump_aulSmTable (
		"s_aulSmTable_PAL_YUV", s_aulSmTable_PAL_YUV);
	BVDC_P_Display_Dump_aulSmTable (
		"s_aulSmTable_PALN_YUV", s_aulSmTable_PALN_YUV);
	BVDC_P_Display_Dump_aulSmTable (
		"s_aulSmTable_PALNC_YUV", s_aulSmTable_PALNC_YUV);
	BVDC_P_Display_Dump_aulSmTable (
		"s_aulSmTable_PALM_YUV", s_aulSmTable_PALM_YUV);
	BVDC_P_Display_Dump_aulSmTable ("s_aulSmTable_SECAM", s_aulSmTable_SECAM);
	BVDC_P_Display_Dump_aulSmTable (
		"s_aulSmTable_Component", s_aulSmTable_Component);
}

void BVDC_Display_DumpTables (void)
{
	BVDC_Display_DumpAll_aulVfTable();
	BVDC_Display_DumpAll_aulChFilterTbl();
	BVDC_Display_DumpAll_aulRmTable();
	BVDC_Display_DumpAll_aulItTable();
	BVDC_Display_DumpAll_aulSmTable();
}
#endif /** } BVDC_P_DISPLAY_DUMP **/

/* End of File */
