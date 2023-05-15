/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvdc_display_priv.c $
 * $brcm_Revision: Hydra_Software_Devel/231 $
 * $brcm_Date: 10/16/12 5:22p $
 *
 * Module Description:
 *     Private module for setting up the modular Vec
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vdc/src/modularvec/bvdc_display_priv.c $
 * 
 * Hydra_Software_Devel/231   10/16/12 5:22p tdo
 * SW7425-977: refactor display format info usage in HDMI path programming
 * for 4kx2k format support
 *
 * Hydra_Software_Devel/230   8/13/12 3:10p yuxiaz
 * SW7425-3626, SW7425-3619: Replace uncache memory access with cache
 * memory access: use the new API names in RDC.
 *
 * Hydra_Software_Devel/229   8/10/12 3:28p pntruong
 * SW7435-276: Updated NTSC/480p to 720x480i/720x480p respectively.
 * Added support 482i/483p with new enums.
 *
 * Hydra_Software_Devel/228   3/22/12 1:01p tdo
 * SW7335-1404: Turning off unused Video DACs on LHR26.  Properly init all
 * DAC related registers
 *
 * Hydra_Software_Devel/227   2/16/12 1:56p darnstein
 * SW7358-227:  Work on this JIRA left breakage in 7405, 7400, 7468, and
 * 7408.
 *
 * Hydra_Software_Devel/226   11/10/11 1:46p tdo
 * SW7400-3064: Aspect ratio incorrect (narrow) on 7400
 *
 * Hydra_Software_Devel/SW7400-2665/1   9/29/11 3:39p darnstein
 * SW7400-2665: Implement double rate 576P analog output for chipsets that
 * do not feature the orthogonal VEC design.
 *
 * Hydra_Software_Devel/224   8/30/11 11:09a syang
 * SW7425-1170: init bAspRatio during Display_Init
 *
 * Hydra_Software_Devel/221   8/29/11 4:23p syang
 * SW7425-1170: refactor pixel aspect ratio related code
 *
 * Hydra_Software_Devel/220   8/18/11 3:16p yuxiaz
 * SW3548-3166: Fixed display apply times out.
 *
 * Hydra_Software_Devel/219   8/5/11 8:25p albertl
 * SW7425-745: Added blanking to compositor color for all outputs and all
 * colorspaces.
 *
 * Hydra_Software_Devel/218   7/28/11 3:47p tdo
 * SW7405-5339: BVDC_Display_Create doesn't recover from bad parameters.
 * Fix display timeout issue with HDMI master mode.
 *
 * Hydra_Software_Devel/217   7/18/11 5:32p pntruong
 * SW7231-284, SW7346-295, SW7405-5339: Refactored master framerate
 * tracking, so that it can also handle validation.
 *
 * Hydra_Software_Devel/216   7/11/11 2:18p tdo
 * SW7420-1971: Video Pause seen when VEC alignment is going on.  Add flag
 * to keep BVN connected while doing alignment.
 *
 * Hydra_Software_Devel/215   7/8/11 6:01p albertl
 * SW7408-291: Updated VF filter API to use Dac enums for channel
 * selection.
 *
 * Hydra_Software_Devel/214   6/24/11 3:50p albertl
 * SW7420-1966: Cleaned up and reorganized colorspace enums and indicies
 * in display, vdec, and cmp.
 *
 * Hydra_Software_Devel/212   6/17/11 4:18p vanessah
 * SW7425-686: change display init fmt
 *
 * Hydra_Software_Devel/211   6/17/11 2:32p albertl
 * SW7408-291:  Added interface to set and get VF filter settings.
 *
 * Hydra_Software_Devel/210   6/2/11 11:56a vanessah
 * SW7422-391: fix compile warning for 3548/3556
 *
 * Hydra_Software_Devel/209   3/17/11 6:10p tdo
 * SW7422-391: Incorrect VertRefreshRate return and add support for PAL 60
 * to allow multi rate
 *
 * Hydra_Software_Devel/208   2/7/11 2:05p darnstein
 * SW7405-5081: Implement ARIB option for NTSC.
 *
 * Hydra_Software_Devel/207   1/20/11 2:56p yuxiaz
 * SW7422-97: Clean up macros for HDMI Rate manage
 *
 * Hydra_Software_Devel/206   12/16/10 11:40p pntruong
 * SW3548-2987: Added 1080p@30hz 3DOU_AS hdmi input support.
 *
 * Hydra_Software_Devel/205   12/9/10 1:32p pntruong
 * SW7335-828, SW7405-4669: Fixed frame rate tracking for vec bypass mode.
 *
 * Hydra_Software_Devel/204   12/3/10 5:29p albertl
 * SW7325-825: Changed GROSS_LUMA_DELAY and FINE_SC_DELAY for SECAM.
 *
 * Hydra_Software_Devel/203   8/31/10 5:46p tdo
 * SW7335-828, SW7405-4669: composite output lacks color until video
 * decode starts
 *
 * Hydra_Software_Devel/202   8/31/10 4:47p tdo
 * SW7335-828, SW7405-4669: composite output lacks color until video
 * decode starts
 *
 * Hydra_Software_Devel/200   7/13/10 11:28a yuxiaz
 * SW3548-2987: Added new 1080p@30hz support for 3DTV.
 *
 * Hydra_Software_Devel/199   6/28/10 2:27p darnstein
 * SW7335-696: Work-around for 7335 video shift issue.
 *
 * Hydra_Software_Devel/198   6/24/10 3:07p yuxiaz
 * SW7405-4549, SW7420-543:  No Audio or video on HDMI outputs for 480p60.
 *
 * Hydra_Software_Devel/197   6/23/10 4:26p darnstein
 * SW7325-389: extend software work-around for WSE problem to SECAM video
 * output.
 *
 * Hydra_Software_Devel/196   6/18/10 7:44a tdo
 * SW7400-2770: Add multi-frame rate support to SD also
 *
 * Hydra_Software_Devel/195   6/16/10 6:33p tdo
 * SW7400-2770: Add multi-frame rate support to SD also
 *
 * Hydra_Software_Devel/194   5/19/10 11:35a pntruong
 * SW7400-2769: To keep ntsc(cvbs/svideo) as 59.94hz and 480i(component)
 * can be 59.94hz or 60.00hz.
 *
 * Hydra_Software_Devel/193   5/17/10 4:32p yuxiaz
 * SW7405-4339:  Inconsistent selection of DECIMATE_RATIO based on video
 * format
 *
 * Hydra_Software_Devel/192   5/11/10 3:18p darnstein
 * SW7340-174: DVI_DTG_0_DTG_CONFIG.AUTO_RESTART only depends on
 * DVI_DTG_0_DTG_CONFIG.SLAVE_MODE now.
 *
 * Hydra_Software_Devel/191   5/7/10 7:21p albertl
 * SW7125-364: Changed dirty bits to use union structure to avoid type-pun
 * warnings
 *
 * Hydra_Software_Devel/190   4/19/10 10:26p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic. Move
 * BDBG_OBJECT_ID_DECLARE private header files instead of .c.
 *
 * Hydra_Software_Devel/189   4/7/10 11:50a tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic. Fix typo
 *
 * Hydra_Software_Devel/188   4/7/10 11:39a tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic.  Rename TLA
 *
 * Hydra_Software_Devel/187   4/5/10 5:25p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic.  Remove warning.
 *
 * Hydra_Software_Devel/186   4/5/10 5:23p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic
 *
 * Hydra_Software_Devel/185   4/5/10 4:19p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic
 *
 * Hydra_Software_Devel/184   3/24/10 3:53p pntruong
 * SW7405-4106: CVBS video output is black for 2 or 3 seconds when USB
 * flash format almost finished.
 *
 * Hydra_Software_Devel/183   3/18/10 11:39a yuxiaz
 * SW7405-3954: Use RDC reserved registers to coordinate programming of
 * VBI encoder control registers between BVBI and BVDC.
 *
 * Hydra_Software_Devel/182   3/1/10 3:51p darnstein
 * SW7400-2620,SW7400-2700: Integrate Nutan's new microcode for 480P. This
 * fixes the 480P "drop lines" problem. Also, DVI-DTG toggle settings are
 * now associated with individual microcode arrays. A new function
 * BVDC_P_GetDviDtgToggles_isr() manages this. When all microcode arrays
 * use the same settings, this function can be removed again.
 *
 * Hydra_Software_Devel/181   2/24/10 6:45p darnstein
 * SW7400-2620: Do not program DVI_DTG.DTG_CONFIG.TOGGLE_DVI_H or
 * DVI_DTG.DTG_CONFIG.TOGGLE_DVI_V for this JIRA. These bitfields are
 * controlled by a public API, so the customer will have to program these
 * bits through the API.
 *
 * Hydra_Software_Devel/180   2/23/10 4:02p darnstein
 * SW7400-2620: previous check-in declared some local variables that don't
 * get used unless HDMI hardware is present.
 *
 * Hydra_Software_Devel/179   2/23/10 3:00p darnstein
 * SW7400-2620: add support for double rate 480P.
 *
 * Hydra_Software_Devel/178   2/19/10 10:33a rpan
 * SW3556-935: Re-arranged spread spectrum 'ulRange' calculation to avoid
 * overflow.
 *
 * Hydra_Software_Devel/177   2/1/10 6:44p hongtaoz
 * SW3548-2678, SW3548-2680, SW3548-2681: added HDMI1.4 3D formats support
 * for 3548; added multi-buffer low delay mode support for 1to2 and 2to5
 * rate gap cases; reduced capture buffer allocation for interlaced
 * pulldown case;
 *
 * Hydra_Software_Devel/BCM3D/4   1/29/10 7:01p hongtaoz
 * SW3548-2680, SW3548-2681: removed 1470p custom formats since they are
 * the same as 720p3D formats; added macro to wrap the 3548 specific
 * legacy 3DTV format;
 *
 * Hydra_Software_Devel/BCM3D/3   1/15/10 6:37p hongtaoz
 * SW3548-2680, SW3548-2681: added custom formats to support HDMI 1.4 3D
 * 720p50/60 input passed-thru to external panel processor; reanmed 2160i
 * to custom DVI format;
 *
 * Hydra_Software_Devel/BCM3D/2   1/14/10 5:22p hongtaoz
 * SW3548-2678, SW3548-2680, SW3548-2681: fixed build error due to the
 * changes on other mainline VDC code;
 *
 * Hydra_Software_Devel/176   1/29/10 8:49p albertl
 * SW7405-3845: Fixed PAL_NC matrices and renamed associated colorspace as
 * YUV_NC.
 *
 * Hydra_Software_Devel/175   1/21/10 7:25p darnstein
 * SW7325-389: change symbol from BVBI_P_WSE_VER3 to BVDC_P_WSE_VER3.
 *
 * Hydra_Software_Devel/174   1/20/10 9:27p pntruong
 * SW3548-2689: Additional fix to spread specturm fix.
 *
 * Hydra_Software_Devel/173   1/11/10 1:17p rpan
 * SW7400-2620: Undo HDMI pixel replication. The 7400/7405 HDMI core
 * doesn't support this feature.
 *
 * Hydra_Software_Devel/172   12/18/09 9:22a rpan
 * SW3556-935: Fixed a compile error for chips not supporting DVO.
 *
 * Hydra_Software_Devel/171   12/18/09 8:59a rpan
 * SW3556-935: New interface for setting spread spectrum.
 *
 * Hydra_Software_Devel/BCM3D/1   12/17/09 6:16p hongtaoz
 * SW3548-2680, SW3548-2681: changed trigger location for special 2160i48
 * VEC format to support 3D video for 11-bit BVN;
 *
 * Hydra_Software_Devel/170   12/11/09 3:42p rpan
 * SW7400-2620: Added HDMI pixel replication for 480p display. This is for
 * chips such as 7400 and 7405 only.
 *
 * Hydra_Software_Devel/169   12/4/09 10:35a pntruong
 * SW3548-2637: Allows additional time for vfd to drain.
 *
 * Hydra_Software_Devel/168   11/13/09 1:05p darnstein
 * SW7400-2607,SW7405-3132: New analog and digital VEC IT microcode for
 * PAL-M.
 *
 * Hydra_Software_Devel/167   11/11/09 3:57p darnstein
 * SW7405-3132: Set LINE_REMOVE_TOP=1 for PAL-M. This might need to be
 * revisited.
 *
 * Hydra_Software_Devel/166   10/16/09 5:22p darnstein
 * SW7405-3191: Back out all changes in pixel frequency handling. New
 * design coming soon.
 *
 * Hydra_Software_Devel/165   10/15/09 4:59p darnstein
 * SW7405-3191: Pixel frequency is now defined as both a bitmask, and an
 * enum. The bitmask option is DEPRECATED.
 *
 * Hydra_Software_Devel/164   10/13/09 5:48p darnstein
 * SW7405-3004: install 480P ARIB output via BVDC display settings.
 *
 * Hydra_Software_Devel/163   10/8/09 11:38a tdo
 * SW7340-7: Use #ifdef instead of #if
 *
 * Hydra_Software_Devel/162   10/8/09 11:17a tdo
 * SW7340-7: Cleanup VCXO macro defines
 *
 * Hydra_Software_Devel/161   9/17/09 10:28a yuxiaz
 * SW7405-3040: Remove kick start PLL on STB.
 *
 * Hydra_Software_Devel/160   7/23/09 4:15p yuxiaz
 * PR52179: Video Rate Change callback invoked continously.
 *
 * Hydra_Software_Devel/159   7/21/09 4:23p rpan
 * PR56037: Initial effort for analog VESA outputs.
 *
 * Hydra_Software_Devel/158   7/9/09 4:32p rpan
 * PR56609: Added HDMI spread spectrum control capabilty.
 *
 * Hydra_Software_Devel/157   6/22/09 6:34p pntruong
 * PR56245, PR48884: Need to response to source rate change when enabled
 * frame tracking.
 *
 * Hydra_Software_Devel/156   6/16/09 3:32p pntruong
 * PR56071: Initialize dac to off in vdc open.
 *
 * Hydra_Software_Devel/155   6/12/09 6:07p darnstein
 * PR55997: No-op instruction address for VEC IT microcode is now 253.
 *
 * Hydra_Software_Devel/154   6/2/09 3:22p darnstein
 * PR48884: More resistance to invalid user inputs.
 *
 * Hydra_Software_Devel/153   5/28/09 6:03p darnstein
 * PR48884: BVDC_P_GetRmTable_isr() no longer modifies its DisplayInfo
 * argument. Instead, it returns a new agrument with the modified data.
 * There are two consequences:
 * 1. When BVDC_P_GetRmTable_isr() is called, a DisplayInfo may have to be
 * modified by the caller.
 * 2. BVDC_P_HdmiRmTableEx_isr() gets a new argument, the same new
 * argument as BVDC_P_GetRmTable_isr().
 *
 * Hydra_Software_Devel/152   5/27/09 6:11p darnstein
 * PR48884: Fix problem that I introduced yesterday.
 *
 * Hydra_Software_Devel/151   5/27/09 4:31p rpan
 * PR55480: ITU-656 master mode support.
 *
 * Hydra_Software_Devel/150   5/27/09 12:27a pntruong
 * PR48884: Temporary backout for getting by build error and code review.
 *
 * Hydra_Software_Devel/148   5/15/09 5:22p rpan
 * PR55212: Fixed an issue that DECIM didn't connect to compositor if MPAA
 * is only enabled on HDMI path.
 *
 * Hydra_Software_Devel/147   5/4/09 6:00p pntruong
 * PR53954: B2: DVP LVDS spread spectrum feature causes a DC shift in
 * frequency and an irregular spreading.  Compiled out for b2.
 *
 * Hydra_Software_Devel/146   5/1/09 11:18a pntruong
 * PR54771: Function BVDC_Display_SetHdmiSyncOnly() dosn't work.  Got left
 * out of the original port from 7401 to 7400.
 *
 * Hydra_Software_Devel/145   4/30/09 3:10p albertl
 * PR54593: Fixed Set and GetDvoMatrix to take and return matrices in
 * YCbCr->RGB and YCbCr->YCbCr format and convert to hardware format
 * internally.
 *
 * Hydra_Software_Devel/144   4/30/09 2:57p rpan
 * PR54725: Fixed the user registered RM change callback not invoked
 * issue.
 *
 * Hydra_Software_Devel/143   4/24/09 3:25p rpan
 * PR54567: Removed the policy of which video formats allow decimation.
 * Let VDC PI caller enforce it.
 *
 * Hydra_Software_Devel/142   4/23/09 3:00p rpan
 * PR54437: Merged 7420 version bvdc_displayfmt_priv.c/h back to mainline.
 * Updated digitial 480p@60hz, 576p@50hz, and 720p@50hz ucode with the
 * updated checksum and date.
 *
 * Hydra_Software_Devel/141   4/17/09 11:57a pntruong
 * PR54064: Refactored common code for defered callback til bvn shutdown.
 * Improved readability and extensibility of dirty bits.
 *
 * Hydra_Software_Devel/140   4/10/09 1:29p yuxiaz
 * PR50031, PR54053: Program LINKDIV_CTRL based on LVDS mode.
 *
 * Hydra_Software_Devel/139   4/6/09 4:53p hongtaoz
 * PR53037, PR53038, PR53687, PR53688: increaed display alignment
 * measurement range to cover the alignment threshold;
 *
 * Hydra_Software_Devel/138   4/6/09 4:32p syang
 * PR  53484 , PR 52037: cleanup (rect, scl-ratio, ..) rounding code and
 * AdjustRect code ; add 1 line delay to TV chip "early trigger"; change
 * BVDC_P_DST_CUT_TOP_THRESH to 4 for 3548;
 *
 * Hydra_Software_Devel/136   4/2/09 8:01p hongtaoz
 * PR53037, PR53038, PR53687, PR53688: display alignment state is cleared
 * after RM is resumed;
 *
 * Hydra_Software_Devel/135   4/2/09 6:06p darnstein
 * PR51325: clean up after faulty merge by ClearCase (previous check-in).
 *
 * Hydra_Software_Devel/134   4/2/09 5:57p darnstein
 * PR51325: Workaround for WSE bug: toggle VF_MISC.VBI_ENABLE every video
 * field.
 *
 * Hydra_Software_Devel/133   4/2/09 1:50p hongtaoz
 * PR53037, PR53038, PR53687, PR53688: clear alignment flag to stop mute
 * when user disables alignment;
 *
 * Hydra_Software_Devel/132   4/1/09 2:58p pntruong
 * PR53584,PR46645: Need to delay kick start when lvds/dvpo reprogram.
 *
 * Hydra_Software_Devel/131   3/31/09 8:51p pntruong
 * PR51930: Corrected rate print out, and remove error assumption that vec
 * rate must be present when doing dvo master output.
 *
 * Hydra_Software_Devel/130   3/31/09 5:34p pntruong
 * PR51930: Added support for multiple rates for 23.93/24.00, 29.97/30.00.
 *
 * Hydra_Software_Devel/129   3/30/09 2:55p hongtaoz
 * PR53037, PR53038: To Implement the VEC Locking/Alignments For Memory
 * Reduction; To Implement Double-Capture-Buffer Algorithm Under VEC
 * Locking Scheme;
 *
 * Hydra_Software_Devel/128   3/26/09 2:34p yuxiaz
 * PR52179: Video Rate Change callback invoked continously
 *
 * Hydra_Software_Devel/127   3/25/09 4:43p pntruong
 * PR35584: Needed areset/dreset after toggle spreadspectrum.
 *
 * Hydra_Software_Devel/126   3/25/09 3:55p pntruong
 * PR35584: LVDS latch error noise and vertical sync lost with digital RF
 * stream.
 *
 * Hydra_Software_Devel/125   3/16/09 10:53p tdo
 * PR45785, PR45789: Merge from MCVP branch
 *
 * Hydra_Software_Devel/7420_mcvp/7   3/13/09 11:15a tdo
 * PR45785, PR45789: merge from main branch on 3/13/09
 *
 * Hydra_Software_Devel/124   3/11/09 7:45p pntruong
 * PR51930, PR52072, PR52364: Added 0.08% offset compensation for all
 * rates.
 *
 * Hydra_Software_Devel/7420_mcvp/6   3/10/09 9:09a tdo
 * PR45785, PR45789: merge from main branch on 3/10/09
 *
 * Hydra_Software_Devel/123   3/9/09 5:59p pntruong
 * PR52991: No CVBS out on DAC (AH20) from NTSC Demod.  Added ability to
 * output ifd to dac.
 *
 * Hydra_Software_Devel/7420_mcvp/5   3/9/09 2:50p tdo
 * PR45785, PR45789: merge from main branch on 3/09/09
 *
 * Hydra_Software_Devel/122   3/7/09 6:34p pntruong
 * PR51930: Moved spread spectrum to the rest of configuration instead of
 * offset adjust.
 *
 * Hydra_Software_Devel/121   3/6/09 7:45p pntruong
 * PR51930: Need to follow special protocol to enable spread spectrum.
 *
 * Hydra_Software_Devel/7420_mcvp/4   3/5/09 1:44p tdo
 * PR45785, PR45789: merge from main branch on 3/05/09
 *
 * Hydra_Software_Devel/120   3/4/09 8:14p albertl
 * PR52003: Fixed NTSC_J VEC table.  Set display xvycc to default to
 * false.
 *
 * Hydra_Software_Devel/7420_mcvp/3   3/4/09 5:24p tdo
 * PR45785, PR45789: merge from main branch on 3/04/09
 *
 * Hydra_Software_Devel/119   3/3/09 8:38p pntruong
 * PR51930: Moved spread spectrum function to vdc.  Refactored duplicate
 * pll build rul.
 *
 * Hydra_Software_Devel/7420_mcvp/2   3/2/09 4:23p tdo
 * PR 45785, PR 45789: merge from main branch on 3/02/09
 *
 * Hydra_Software_Devel/118   2/27/09 7:47p pntruong
 * PR52588: Video will flash or mute when toggle Half Field (config 2/6).
 * Aggressive adjustment of rm will run into rts issue.  Reduced the
 * speed to ease rts.
 *
 * Hydra_Software_Devel/7420_mcvp/1   2/25/09 12:49p tdo
 * PR 45785, PR 45789: merge from main branch on 2/25/09
 *
 * Hydra_Software_Devel/117   2/23/09 11:22a pntruong
 * PR51940: Temporary backout last check ins.  CSC min/max are incorrectly
 * constructed.
 *
 * Hydra_Software_Devel/116   2/20/09 2:45p pntruong
 * PR52072: Allow adjustment of dvo rate manager offset.
 *
 * Hydra_Software_Devel/115   2/18/09 9:59a yuxiaz
 * PR52179: Video Rate Change callback invoked continously
 *
 * Hydra_Software_Devel/114   2/11/09 10:13a pntruong
 * PR51930: Used BFMT_VideoInfo to check for multirates capability.
 *
 * Hydra_Software_Devel/113   2/4/09 10:57a rpan
 * PR51727: Included DAC bandgap AUX bit to the power management control.
 *
 * Hydra_Software_Devel/112   2/2/09 2:29p rpan
 * PR51727: Changes for DAC power management to work on 7405 and 7325.
 *
 * Hydra_Software_Devel/111   2/2/09 12:08p rpan
 * PR51727: Added power management for all versions of TDAC.
 *
 * Hydra_Software_Devel/110   1/22/09 2:54p albertl
 * PR51169: Fixed issue where BVDC_P_Display_GetCscTable_isr returns an
 * invalid table when display format and input colorspace changes but
 * applychanges is not applied and output colorspace is not properly
 * selected.
 *
 * Hydra_Software_Devel/109   1/15/09 6:20p hongtaoz
 * PR50621: repositioned trigger locations to as early as possible for DTV
 * display timing to have enough head room for the longest RULs;
 *
 * Hydra_Software_Devel/108   11/26/08 4:02p yuxiaz
 * PR47495: Enable DVI_CSC Dithering for 8 bit Outputs
 *
 * Hydra_Software_Devel/107   11/25/08 5:09p tdo
 * PR49528: fix compiling error
 *
 * Hydra_Software_Devel/106   11/25/08 4:24p darnstein
 * PR49528: restore old logic for 3548 and 3556, for initializing some
 * path properties in the case pDisplay->eId == BVDC_DisplayId_eDisplay2.
 *
 * Hydra_Software_Devel/105   11/24/08 5:02p rpan
 * PR45804: Move VEC reset to display module.
 *
 * Hydra_Software_Devel/104   11/17/08 5:02p darnstein
 * PR45819: fix error in previous check-in (include file name).
 *
 * Hydra_Software_Devel/103   11/17/08 4:12p darnstein
 * PR45819: Update scratch register trickery to support ITU-R 656 output,
 * in addition to analog output.
 *
 * Hydra_Software_Devel/102   11/12/08 5:26p darnstein
 * PR40806: provide function BVDC_Display_GetItUcodeInfo() to return info
 * about VEC IT microcode.
 *
 * Hydra_Software_Devel/101   11/10/08 2:01p pntruong
 * PR48757: [PQ] Need N2L and L2N in compositor CSC enabled by default.
 *
 * Hydra_Software_Devel/100   11/7/08 1:40p pntruong
 * PR45239, PR48758: Added 1080i/1080p/720p@ 60hz LVDS output support.
 * Updated respective digital ucode to be able to run in master mode.
 *
 * Hydra_Software_Devel/99   11/5/08 5:12p pntruong
 * PR45239: Added temporary support for DVI master mode 1080p/720p/1080i.
 *
 * Hydra_Software_Devel/98   10/28/08 7:49p darnstein
 * PR46847: ClearCase merge duplicated a line in previous check-in.
 *
 * Hydra_Software_Devel/97   10/28/08 7:33p darnstein
 * PR46847: Combined microcode for PAL Macrovsion type1, type2, and type
 * 3.
 *
 * Hydra_Software_Devel/96   10/28/08 12:01a tdo
 * PR46484: Fix compiling error
 *
 * Hydra_Software_Devel/95   10/27/08 11:56p tdo
 * PR46484: Clean up VEC code
 *
 * Hydra_Software_Devel/94   10/24/08 1:17p albertl
 * PR48023: Fixed missing initialization value for  dvo rgb attenuation
 * causing purple dvo output.
 *
 * Hydra_Software_Devel/93   10/24/08 10:45a pntruong
 * PR48023: Temporary bypassed rgb attenuation adjustment for further
 * investigation of operation.
 *
 * Hydra_Software_Devel/92   10/24/08 2:46a albertl
 * PR48023:  Added BVDC_Display_SetDvoAttenuationRGB and
 * BVDC_Display_GetDvoAttenuationRGB, enabling CSC adjustment on DVO
 * path.  Further cleaned up CSC matrix multiplication code.
 *
 * Hydra_Software_Devel/91   10/22/08 6:12p pntruong
 * PR47816: Make sure the dvo configuration flags are used in PI.
 *
 * Hydra_Software_Devel/90   10/18/08 3:37p pntruong
 * PR48025: Account for bypass display with dac enabled.
 *
 * Hydra_Software_Devel/89   10/17/08 2:13p tdo
 * PR47744: Provide smooth updating of CCB table
 *
 * Hydra_Software_Devel/88   10/9/08 5:28p syang
 * PR 46891: add _isr to the name of fuc that used in _isr context
 *
 * Hydra_Software_Devel/87   9/29/08 8:31p darnstein
 * PR46949: fix a typo. Affected chipsets that do not support component
 * only output.
 *
 * Hydra_Software_Devel/86   9/24/08 7:52p darnstein
 * PR46949: DCS ON/OFF no longer "flashes." This is the 97400 check in.
 *
 * Hydra_Software_Devel/85   9/20/08 10:42a pntruong
 * PR28001: Fixed warnings.
 *
 * Hydra_Software_Devel/84   9/20/08 9:49a pntruong
 * PR28001: Updated ndiv_mode to match settings from bringup scripts.
 *
 * Hydra_Software_Devel/83   9/18/08 3:09p rpan
 * PR46912: Update MPAA decimation registers only if there is a setting
 * change.
 *
 * Hydra_Software_Devel/82   9/16/08 10:18p pntruong
 * PR46494: B0: Remove 3rd output dac.
 *
 * Hydra_Software_Devel/81   9/10/08 7:51p pntruong
 * PR46546, PR46584: Dynamic power management of video DAC blocks.
 *
 * Hydra_Software_Devel/80   8/22/08 10:29a rpan
 * PR45979: Remove VBI passthru support.
 *
 * Hydra_Software_Devel/79   8/21/08 4:29p hongtaoz
 * PR45409: removed the GISB error caused by incorrect VEC crossbar
 * programming; this may need to be revisited for orthorgonal VEC;
 *
 * Hydra_Software_Devel/78   8/8/08 3:31p yuxiaz
 * PR45484: Enable Dithering in VDC.
 *
 * Hydra_Software_Devel/77   7/31/08 5:42p darnstein
 * PR45142: VBI_PREFERRED bit is now ON for PAL and NTSC. New VEC IT
 * microcode requires this for WSS, and for AMOL II high speed.
 *
 * Hydra_Software_Devel/76   7/31/08 4:50p darnstein
 * PR44592: complicated adjustment of VF_MISC.BVB_LINE_REMOVE_TOP.
 *
 * Hydra_Software_Devel/75   7/30/08 3:01p darnstein
 * PR44592: put in another temporary fix until microcodes can get updated.
 *
 * Hydra_Software_Devel/74   7/24/08 6:41p albertl
 * PR39163:  Added bXvYcc flag check to  BVDC_P_Display_ApplyChanges.
 *
 * Hydra_Software_Devel/73   7/23/08 7:17p tdo
 * PR43508, PR43509: Fix compiling errors
 *
 * Hydra_Software_Devel/72   7/23/08 7:01p tdo
 * PR43508, PR43509: Mapping individual bandgap adjustment for each DAC
 *
 * Hydra_Software_Devel/71   7/17/08 3:51p darnstein
 * PR44592: Restore the VF_MISC.BVB_LINE_REMOVE_TOP setting, but only for
 * DCS.
 *
 * Hydra_Software_Devel/70   7/11/08 3:36p darnstein
 * PR44592: Don't do BVB_LINE_REMOVE_TOP in 625 line special case any
 * more. Advised by Maulshree Tamhankar.
 *
 * Hydra_Software_Devel/69   7/10/08 10:46p pntruong
 * PR44677: Fine tune DVO pll.
 *
 * Hydra_Software_Devel/68   7/9/08 5:02p tdo
 * PR41932: Add support for VDC 7335B0 changes.
 *
 * Hydra_Software_Devel/67   7/1/08 6:07p hongtaoz
 * PR32130: enabled DVO clock adjustment game mode control for 3548;
 *
 * Hydra_Software_Devel/66   7/1/08 1:43p rpan
 * PR44375: Updated LVDS PLL settings for 3548/3556.
 *
 * Hydra_Software_Devel/65   6/27/08 11:21a rpan
 * PR44206: Clear the "bRateManagerUpdated" flag only if user has
 * registered a callback and the callback has been invoked.
 *
 * Hydra_Software_Devel/64   6/25/08 7:10p pntruong
 * PR42486: Allowed vdec->vec passthru dacs bypass display too.
 *
 * Hydra_Software_Devel/63   6/23/08 8:12p pntruong
 * PR34239:  Allow dynamically loading of vec custom timing.
 *
 * Hydra_Software_Devel/62   6/16/08 2:28p darnstein
 * PR43509: Fix stupid error in previous check-in.
 *
 * Hydra_Software_Devel/61   6/16/08 2:15p darnstein
 * PR43509: Provide adjustments to DAC bandgap for 3548 and 3556.
 *
 * Hydra_Software_Devel/60   6/16/08 1:27p darnstein
 * PR43509: Provide separate adjustments to TDAC and QDAC bandgap
 * settings.
 *
 * Hydra_Software_Devel/59   6/11/08 1:40p yuxiaz
 * PR39158: More dither settings in VEC.
 *
 * Hydra_Software_Devel/58   6/9/08 9:47p tdo
 * PR43462: Support CCB for 3549/3556
 *
 * Hydra_Software_Devel/57   6/7/08 1:06p rpan
 * PR39159: Return DAC setting for pass-through.
 *
 * Hydra_Software_Devel/56   6/7/08 12:10p darnstein
 * PR43423: PAL/Macrovision: IT state wasn't getting updated.
 *
 * Hydra_Software_Devel/55   6/4/08 1:44p rpan
 * PR39421: Added LVDS PLL settings for 3548/3556.
 *
 * Hydra_Software_Devel/54   6/4/08 9:38a maivu
 * PR 43313: Program correct new misc registers to fix component output
 *
 * Hydra_Software_Devel/53   5/30/08 10:44a rpan
 * PR42619: Disable VEC trigger when destroying a display handle.
 *
 * Hydra_Software_Devel/52   5/27/08 7:07p hongtaoz
 * PR42248, PR42494: added display alignment API;
 *
 * Hydra_Software_Devel/51   5/16/08 2:17p rpan
 * PR42109: More SECAM variants support.
 *
 * Hydra_Software_Devel/50   5/2/08 4:02p rpan
 * PR39421: Ported 3548 DVPO support from 3563.
 *
 * Hydra_Software_Devel/49   5/1/08 2:39p rpan
 * PR42109: Initial work for SECAM variants.
 *
 * Hydra_Software_Devel/48   4/24/08 2:47p hongtaoz
 * PR32130: merged RM clock adjustment feature for game mode support from
 * 3563 to 7400/3548;
 *
 * Hydra_Software_Devel/47   3/17/08 3:58p darnstein
 * PR25971: A VF core register setting for DCS/7400 was missing.
 *
 * Hydra_Software_Devel/46   3/13/08 9:16a yuxiaz
 * PR40495: Fixed KLOCWORK errors.
 *
 * Hydra_Software_Devel/45   3/10/08 8:35p darnstein
 * PR25971: first try at DCS for 7400
 *
 * Hydra_Software_Devel/44   2/13/08 3:23p rpan
 * PR39421: Fixed compile warnings.
 *
 * Hydra_Software_Devel/43   2/13/08 12:24p rpan
 * PR39421: Got 3548 display files to compile.
 *
 * Hydra_Software_Devel/42   2/11/08 9:06p pntruong
 * PR39177: [VEC] Need new display api to specify framedrop at runtime.
 *
 * Hydra_Software_Devel/41   2/5/08 2:00p rpan
 * PR38990: Enforced PI to check only bit 0 of aulEnableMpaaDeci for MPAA
 * decimate.
 *
 * Hydra_Software_Devel/40   1/24/08 6:12p tdo
 * PR38150: Undo some of the changes made from previous check in to bring
 * up the DAC output again for 7335
 *
 * Hydra_Software_Devel/39   1/13/08 4:24a albertl
 * PR38150: Updated 7325 to incorporate CSC interface changes.
 *
 * Hydra_Software_Devel/37   11/20/07 11:08p tdo
 * PR36889: Remove strictly alias warnings
 *
 * Hydra_Software_Devel/36   11/19/07 3:48p tdo
 * PR36898: Add VDC PI support for 7335
 *
 * Hydra_Software_Devel/35   10/30/07 8:27p pntruong
 * PR34239: Fixed build errors, and use cur info instead.
 *
 * Hydra_Software_Devel/34   10/30/07 7:51p pntruong
 * PR34239: Allow dynamically loading of vec custom timing.
 *
 * Hydra_Software_Devel/33   10/19/07 5:01p rpan
 * PR36026: Relaxed restriction between RFM and DACs. Enabling RFM does
 * not necessarily have to drive a CVBS DAC.
 *
 * Hydra_Software_Devel/32   10/12/07 9:23a pntruong
 * PR35135:  Fixed user csc macros and added 10-bit/8-bit video data
 * conversion.
 *
 * Hydra_Software_Devel/31   10/8/07 2:46p pntruong
 * PR34855: Initial VDC bringup.
 *
 * Hydra_Software_Devel/30   10/2/07 12:48p pntruong
 * PR35136: Some cleanup of unused code.
 *
 * Hydra_Software_Devel/29   10/2/07 9:52a yuxiaz
 * PR34523: All the BVB_CONFIG registers are removed from VEC on 7325.
 *
 * Hydra_Software_Devel/28   9/20/07 5:07p yuxiaz
 * PR35135: Fixed build errors on 7405 from csc refactoring.
 *
 * Hydra_Software_Devel/27   9/14/07 10:33a yuxiaz
 * PR34803: Consolidate TDAC settings for 7405 A0 and A1
 *
 * Hydra_Software_Devel/26   9/11/07 4:37p yuxiaz
 * PR34803: Added 7405 A1 support.
 *
 * Hydra_Software_Devel/25   9/10/07 4:55p maivu
 * PR 34624: Fixed error with previous check-in, to match with changes in
 * RDB.
 *
 * Hydra_Software_Devel/24   9/6/07 11:17a maivu
 * PR 34624: Added 7400D0 support
 *
 * Hydra_Software_Devel/23   6/26/07 1:22p maivu
 * PR 32452: Removed incorrect interpretation of max Dac outputs on
 * primary/tert Vecs. Primary Vec supports 3 separate outputs, which are
 * fed to an output cross-bar. Any path can control all Dacs.
 *
 * Hydra_Software_Devel/22   5/10/07 4:25p yuxiaz
 * PR27644: Add TDAC settings for 7405. Clean up TDAC and QDAC flags.
 *
 * Hydra_Software_Devel/21   5/7/07 11:57a yuxiaz
 * PR27644: Added 7405 support for 656 out.
 *
 * Hydra_Software_Devel/20   5/4/07 4:18p yuxiaz
 * PR28786: Fix 656 out switching from master to slave mode.
 *
 * Hydra_Software_Devel/19   4/24/07 11:46a yuxiaz
 * PR27644: Added 7405 support.
 *
 * Hydra_Software_Devel/18   4/23/07 10:04a maivu
 * PR 29537: Need to select proper 656 source.
 *
 * Hydra_Software_Devel/17   4/18/07 11:55a maivu
 * PR 29768: Set passthr count for decim source to zero for 7400B0, since
 * there is no vdec source. Confirmed MPPA works.
 *
 * Hydra_Software_Devel/16   4/13/07 11:29a maivu
 * PR 29768: Added MPAA support for 7400B0
 *
 * Hydra_Software_Devel/15   4/9/07 12:06p pntruong
 * PR28266: Provide API to customize DVO CSC.
 *
 * Hydra_Software_Devel/14   3/26/07 11:04p tdo
 * PR 28402: Provide API to load customized CCB table for whitebalance
 *
 * Hydra_Software_Devel/13   3/12/07 4:00p maivu
 * PR 28597: No need to disable dvi, soft_reset will disable dvi src
 * select. Effects modular vec design only.
 *
 * Hydra_Software_Devel/12   3/7/07 5:22p maivu
 * PR 27609: Connect proper DVI source. Moved HDMI_TX_PHY settings to HDM
 * module.
 *
 * Hydra_Software_Devel/11   3/7/07 10:58a maivu
 * PR 27609: Add DVI support for 7400B0
 *
 * Hydra_Software_Devel/10   2/28/07 12:59p maivu
 * PR 26051: Fixed Svideo & composite outputs for 7400B0
 *
 * Hydra_Software_Devel/9   2/26/07 10:02a maivu
 * PR 26051: Fixed error with 7400A0 build from previous check-in
 *
 * Hydra_Software_Devel/8   2/25/07 2:29p maivu
 * PR 26051: Proper settings for new modular Vec.
 *
 * Hydra_Software_Devel/7   2/14/07 10:12a maivu
 * PR 26053: Added CRC support for 7400B0
 *
 * Hydra_Software_Devel/6   2/7/07 1:22p maivu
 * PR 26051: Complete support for 7400B0.
 *
 * Hydra_Software_Devel/5   1/22/07 5:27p tdo
 * PR 24608:  Only toggle 240p display as source changed to 240p input
 * when display format is NTSC
 *
 * Hydra_Software_Devel/4   1/22/07 5:24p hongtaoz
 * PR23260: fixed typo;
 *
 * Hydra_Software_Devel/3   1/19/07 6:19p hongtaoz
 * PR23260: disable trigger when destroy display;
 *
 * Hydra_Software_Devel/2   1/17/07 5:06p maivu
 * PR 26051: Initial support for modular vec. Incomplete, waiting for
 * scripts.
 *
 * Hydra_Software_Devel/1   1/16/07 3:38p maivu
 * PR25963: Ported from Hydra_software_Devel/332.
 *
 ***************************************************************************/
#include "bstd.h"
#include "bdbg.h"
#include "bfmt.h"

/* Note: Tricky here!  bavc.h needs bchp_rdc.h defininitions */
#include "bchp_rdc.h"

#include "bavc.h"
#include "bvdc_compositor_priv.h"
#include "bvdc_displayfmt_priv.h"
#include "bvdc_common_priv.h"

#include "bchp_misc.h"
#include "bchp_video_enc.h"
#include "bchp_vbi_enc_prim.h"
#include "bchp_video_enc_decim.h"
#include "bchp_timer.h"


/* Reset VEC */
#include "bchp_sun_top_ctrl.h"

#if BVDC_P_SUPPORT_ITU656_OUT
#include "bchp_csc.h"
#include "bchp_dtg.h"
#include "bchp_dvf.h"
#include "bchp_itu656.h"
#endif

#if BVDC_P_SUPPORT_DVI_OUT
#include "bchp_dvi_dtg.h"
#include "bchp_dvi_csc.h"
#include "bchp_dvi_dvf.h"
#endif

#if (BVDC_P_SUPPORT_ITU656_OUT || BVDC_P_SUPPORT_DVI_OUT)
#include "bchp_dtram.h"
#endif

#if (BVDC_P_SUPPORT_VBI_ENC_656)
	#if (BCHP_CHIP == 3548) || (BCHP_CHIP == 3556)
		#include "bchp_vbi_enc_656.h"
	#else
		#include "bchp_vbi_enc_656_ancil.h"
	#endif
#endif

#if BVDC_P_SUPPORT_SEC_VEC
#include "bchp_sec_csc_co.h"
#include "bchp_sec_src_co.h"
#include "bchp_sec_vf_co.h"
#include "bchp_vbi_enc_sec.h"
#endif

#if BVDC_P_SUPPORT_TER_VEC
#include "bchp_vbi_enc_tert.h"
#endif

#if BVDC_P_SUPPORT_PRM_VEC_CMPN_ONLY
#include "bchp_prim_src_co.h"
#include "bchp_prim_csc_co.h"
#include "bchp_prim_vf_co.h"
#endif

#if BVDC_P_SUPPORT_DVI_OUT
#if BVDC_P_SUPPORT_HDMI_OUT /* 7038/740x etc */
#include "bchp_hdmi_rm.h"
#elif BVDC_P_SUPPORT_DVPO /* 3563 */
#include "bchp_dvpo_rm_0.h"
#include "bchp_dvpo_0.h"
#include "bchp_lvds_phy_0.h"
#else /* 3560 */
#include "bchp_sparrow_rm.h"
#endif
#endif

#if BVDC_P_SUPPORT_DVI_65NM
#include "bchp_hdmi_tx_phy.h"
#endif

#if BVDC_P_SUPPORT_DOWNSAMPLE
#include "bvdc_downsample_priv.h"
#endif

/* registers for component-only output path.
 * note: now assume only one of prm and sec support component-only output
 *       if this is not true, maybe we should use something like
 *       BVDC_P_VEC_VF_CO_OFFSET and BVDC_P_VEC_CSC_CO_OFFSET VF and VF_CO
 *       are identical duplicate of HW .
 */
#if BVDC_P_SUPPORT_PRM_VEC_CMPN_ONLY
#define BVDC_P_VEC_VF_MISC                 BCHP_PRIM_VF_MISC
#define BVDC_P_VEC_VF_CO_MISC              BCHP_PRIM_VF_CO_MISC
#define BVDC_P_VEC_VF_CO_FORMAT_ADDER      BCHP_PRIM_VF_CO_FORMAT_ADDER
#define BVDC_P_VEC_VF_CO_VF_RESET          BCHP_PRIM_VF_CO_VF_RESET
#define BVDC_P_VEC_VF_CO_NEG_SYNC_VALUES   BCHP_PRIM_VF_CO_NEG_SYNC_VALUES
#define BVDC_P_VEC_VF_CO_POS_SYNC_VALUES   BCHP_PRIM_VF_CO_POS_SYNC_VALUES
#define BVDC_P_VEC_VF_CO_FORMAT_ADDER      BCHP_PRIM_VF_CO_FORMAT_ADDER
#define BVDC_P_VEC_CSC_CO_CSC_MODE         BCHP_PRIM_CSC_CO_CSC_MODE
#define BVDC_P_VEC_CSC_CO_CSC_RESET        BCHP_PRIM_CSC_CO_CSC_RESET
#define BVDC_P_VEC_SRC_CO_SRC_CONTROL      BCHP_PRIM_SRC_CO_SRC_CONTROL
#define BVDC_P_VEC_PATH_SUPPORT_CO(path)   (path == BVDC_P_eVecPrimary)
#elif BVDC_P_SUPPORT_SEC_VEC_CMPN_ONLY
#define BVDC_P_VEC_VF_MISC                 BCHP_SEC_VF_MISC
#define BVDC_P_VEC_VF_CO_MISC              BCHP_SEC_VF_CO_MISC
#define BVDC_P_VEC_VF_CO_FORMAT_ADDER      BCHP_SEC_VF_CO_FORMAT_ADDER
#define BVDC_P_VEC_VF_CO_VF_RESET          BCHP_SEC_VF_CO_VF_RESET
#define BVDC_P_VEC_VF_CO_NEG_SYNC_VALUES   BCHP_SEC_VF_CO_NEG_SYNC_VALUES
#define BVDC_P_VEC_VF_CO_POS_SYNC_VALUES   BCHP_SEC_VF_CO_POS_SYNC_VALUES
#define BVDC_P_VEC_VF_CO_FORMAT_ADDER      BCHP_SEC_VF_CO_FORMAT_ADDER
#define BVDC_P_VEC_CSC_CO_CSC_MODE         BCHP_SEC_CSC_CO_CSC_MODE
#define BVDC_P_VEC_CSC_CO_CSC_RESET        BCHP_SEC_CSC_CO_CSC_RESET
#define BVDC_P_VEC_SRC_CO_SRC_CONTROL      BCHP_SEC_SRC_CO_SRC_CONTROL
#define BVDC_P_VEC_PATH_SUPPORT_CO(path)   (path == BVDC_P_eVecSecondary)
#else
#define BVDC_P_VEC_PATH_SUPPORT_CO(path)   (false)
#endif

/* PR14712: */
#define BVDC_P_PR14712_FIXED             1

#ifdef BVDC_P_PR14712_FIXED
#define BVDC_P_VF_SAV_RPLCT              6
#define BVDC_P_VF_EAV_PRDCT              3
#else
#define BVDC_P_VF_SAV_RPLCT              9
#define BVDC_P_VF_EAV_PRDCT              0
#endif
#define BVDC_P_VF_THRESH                 1  /* HW reset value */
#define BVDC_P_VF_ENABLE                 1  /* HW reset value */

/***************** RM clock adjustment macroes *************/
#define BVDC_P_DISPLAY_RM_PHASE_INC_REG_OFFSET \
	((BCHP_PRIM_RM_PHASE_INC - BCHP_PRIM_RM_RATE_RATIO) / sizeof(uint32_t))

/* Dither settings for DISP */
#define BVDC_P_DITHER_DISP_CSC_LFSR_VALUE            (0xFFC01)
#define BVDC_P_DITHER_DISP_CSC_LFSR_CTRL_T0          (0x5)
#define BVDC_P_DITHER_DISP_CSC_LFSR_CTRL_T1          (0x3)
#define BVDC_P_DITHER_DISP_CSC_LFSR_CTRL_T2          (0x5)
/* Dither settings for DVI, 656 */
#define BVDC_P_DITHER_DISP_DVI_LFSR_VALUE            (0xEF83D)
#define BVDC_P_DITHER_DISP_DVI_LFSR_CTRL_T0          (0x0)
#define BVDC_P_DITHER_DISP_DVI_LFSR_CTRL_T1          (0x1)
#define BVDC_P_DITHER_DISP_DVI_LFSR_CTRL_T2          (0x6)

#define BVDC_P_DITHER_DISP_DVI_SCALE_10BIT           (0x1)
#define BVDC_P_DITHER_DISP_DVI_OFFSET_10BIT          (0x0)
#define BVDC_P_DITHER_DISP_DVI_SCALE_8BIT            (0x4)
#define BVDC_P_DITHER_DISP_DVI_OFFSET_8BIT           (0x1D)

/* Special settings for 656 dither */
#define BVDC_P_DITHER_DISP_CSC_SCALE_CH0             (0x1)
#define BVDC_P_DITHER_DISP_CSC_SCALE_CH1             (0x5)
#define BVDC_P_DITHER_DISP_CSC_SCALE_CH2             (0x5)

/* VCXO_RM in the following chipsets need to be reserved since vec core
   reset would reset VCXO_RM also! */
#ifdef BCHP_VCXO_0_RM_REG_START
#include "bchp_vcxo_0_rm.h"
#define BVDC_P_VCXO_RM_REG_COUNT \
	((BCHP_VCXO_0_RM_REG_END - BCHP_VCXO_0_RM_REG_START) / sizeof(uint32_t) + 1)
#endif
#ifdef BCHP_VCXO_1_RM_REG_START
#include "bchp_vcxo_1_rm.h"
#endif
#ifdef BCHP_VCXO_2_RM_REG_START
#include "bchp_vcxo_2_rm.h"
#endif

BDBG_MODULE(BVDC_DISP);
BDBG_OBJECT_ID(BVDC_DSP);

/* Forward declarations */
static const BVDC_P_RateInfo* BVDC_P_HdmiRmTableEx_isr
(
	const BFMT_VideoInfo                *pFmtInfo,
	BAVC_HDMI_PixelRepetition eHdmiPixelRepetition,
	const BAVC_VdcDisplay_Info          *pRateInfo
);


/*************************************************************************
 * BVDC_P_Display_Create
 *
 *************************************************************************/
BERR_Code BVDC_P_Display_Create
(
	BVDC_P_Context                  *pVdc,
	BVDC_Display_Handle             *phDisplay,
	BVDC_DisplayId                   eId
)
{
	BVDC_P_DisplayContext *pDisplay;

	BDBG_ENTER(BVDC_P_Display_Create);

	/* (1) Allocate display context */
	pDisplay = (BVDC_P_DisplayContext*)
		(BKNI_Malloc(sizeof(BVDC_P_DisplayContext)));
	if(!pDisplay)
	{
		return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
	}

	/* Initialize to zero */
	BKNI_Memset((void*)pDisplay, 0x0, sizeof(BVDC_P_DisplayContext));
	BDBG_OBJECT_SET(pDisplay, BVDC_DSP);

	/* Initialize non-changing states.  These are not to be changed by runtime. */
	pDisplay->ulRdcVarAddr      = BRDC_AllocScratchReg(pVdc->hRdc);
	if(pDisplay->ulRdcVarAddr == 0)
	{
		BDBG_ERR(("Not enough scratch registers for display	format switch!"));
		BKNI_Free((void*)pDisplay);
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	pDisplay->ulScratchTsAddr = BRDC_AllocScratchReg(pVdc->hRdc);
	if(pDisplay->ulScratchTsAddr == 0)
	{
		BDBG_ERR(("Not enough scratch registers for display Timestamp!"));
		BRDC_FreeScratchReg(pDisplay->hVdc->hRdc, pDisplay->ulRdcVarAddr);
		BKNI_Free((void*)pDisplay);
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	pDisplay->eId               = eId;
	pDisplay->hVdc              = (BVDC_Handle)pVdc;
	pDisplay->hTimer          = pVdc->hTimer;
	BTMR_GetTimerRegisters(pDisplay->hTimer, &pDisplay->stTimerReg);
	pDisplay->bIsBypass         = pVdc->pFeatures->bCmpBIsBypass &&
		(BVDC_DisplayId_eDisplay2 == eId);
	pDisplay->aulMpaaDeciIfPortMask[BVDC_MpaaDeciIf_eHdmi]      = BVDC_Hdmi_0;
	pDisplay->aulMpaaDeciIfPortMask[BVDC_MpaaDeciIf_eComponent] = BVDC_Cmpnt_0;

	/* CompB => Tertiary/Bypass, Comp0 => Secondary, Comp1 => Primary */
	if(pDisplay->eId == BVDC_DisplayId_eDisplay2)
	{
#if BVDC_P_SUPPORT_TER_VEC
		pDisplay->eVecPath    = BVDC_P_eVecTertiary;
		pDisplay->eVbiPath    = BAVC_VbiPath_eVec2;
		pDisplay->ulScratchVbiEncControl = BAVC_VBI_ENC_2_CTRL_SCRATCH;
		pDisplay->ulVbiEncOffset =
			(int32_t)(BCHP_VBI_ENC_TERT_Control - BCHP_VBI_ENC_PRIM_Control);
		pDisplay->lItOffset  =
			(int32_t)(BCHP_TERT_IT_IT_REV_ID    - BCHP_PRIM_IT_IT_REV_ID);
		pDisplay->lVfOffset  =
			(int32_t)(BCHP_TERT_VF_VF_REV_ID    - BCHP_PRIM_VF_VF_REV_ID);
		pDisplay->lSmOffset  =
			(int32_t)(BCHP_TERT_SM_SM_REV_ID    - BCHP_PRIM_SM_SM_REV_ID);
		pDisplay->lCscOffset =
			(int32_t)(BCHP_TERT_CSC_CSC_REV_ID  - BCHP_PRIM_CSC_CSC_REV_ID);
		pDisplay->lSrcOffset =
			(int32_t)(BCHP_TERT_SRC_SRC_REV_ID  - BCHP_PRIM_SRC_SRC_REV_ID);
		pDisplay->lRmOffset  =
			(int32_t)(BCHP_TERT_RM_CONTROL      - BCHP_PRIM_RM_CONTROL);
/* TODO: replace the following #elif statement with something more graceful. */
#elif (BCHP_CHIP == 3548) || (BCHP_CHIP == 3556)
		pDisplay->eVecPath    = BVDC_P_eVecBypass0;
		pDisplay->eVbiPath    = BAVC_VbiPath_eBypass0;
		pDisplay->ulScratchVbiEncControl = BAVC_VBI_ENC_BP_CTRL_SCRATCH;
		pDisplay->ulVbiEncOffset =
			(int32_t)(BCHP_VBI_ENC_656_Ancil_Control -
				BCHP_VBI_ENC_PRIM_Control);
#endif
	}

#if (BVDC_P_SUPPORT_SEC_VEC)
	else if(pDisplay->eId == BVDC_DisplayId_eDisplay1)
	{
		pDisplay->eVecPath = BVDC_P_eVecSecondary;
		/* BCHP_SEC_* is lower address */
		pDisplay->eVbiPath    = BAVC_VbiPath_eVec1;

		pDisplay->lItOffset  =
			(int32_t)(BCHP_SEC_IT_IT_REV_ID    - BCHP_PRIM_IT_IT_REV_ID);
		pDisplay->lVfOffset  =
			(int32_t)(BCHP_SEC_VF_VF_REV_ID    - BCHP_PRIM_VF_VF_REV_ID);
		pDisplay->lSmOffset  =
			(int32_t)(BCHP_SEC_SM_SM_REV_ID    - BCHP_PRIM_SM_SM_REV_ID);
		pDisplay->lCscOffset =
			(int32_t)(BCHP_SEC_CSC_CSC_REV_ID  - BCHP_PRIM_CSC_CSC_REV_ID);
		pDisplay->lSrcOffset =
			(int32_t)(BCHP_SEC_SRC_SRC_REV_ID  - BCHP_PRIM_SRC_SRC_REV_ID);
		pDisplay->lRmOffset  =
			(int32_t)(BCHP_SEC_RM_CONTROL      - BCHP_PRIM_RM_CONTROL);

		/* BCHP_VBI_ENC_PRIM_Control is lower address */
		pDisplay->ulScratchVbiEncControl = BAVC_VBI_ENC_1_CTRL_SCRATCH;
		pDisplay->ulVbiEncOffset =
			(int32_t)(BCHP_VBI_ENC_SEC_Control - BCHP_VBI_ENC_PRIM_Control);
	}
#endif
	else if(pDisplay->eId == BVDC_DisplayId_eDisplay0)
	{
		pDisplay->eVecPath    = BVDC_P_eVecPrimary;
		pDisplay->eVbiPath    = BAVC_VbiPath_eVec0;
		pDisplay->lItOffset  = 0;
		pDisplay->lVfOffset  = 0;
		pDisplay->lSmOffset  = 0;
		pDisplay->lCscOffset = 0;
		pDisplay->lSrcOffset = 0;
		pDisplay->lRmOffset  = 0;

		pDisplay->ulScratchVbiEncControl = BAVC_VBI_ENC_0_CTRL_SCRATCH;
		pDisplay->ulVbiEncOffset = 0;
	}

#if BVDC_P_SUPPORT_VBI_ENC_656
		pDisplay->ulScratchVbiEncControl_656 = BAVC_VBI_ENC_BP_CTRL_SCRATCH;
		pDisplay->ulVbiEncOffset_656 = 0;	/* Only one VBI/656 path */
#endif

	/* (2) create a AppliedDone event. */
	BKNI_CreateEvent(&pDisplay->hAppliedDoneEvent);

	/* (3) Save hDisplay in hVdc */
	pVdc->ahDisplay[pDisplay->eId] = (BVDC_Display_Handle)pDisplay;

	/* TODO: assign later
	pCompositor->hDisplay = (BVDC_Display_Handle)pDisplay;*/
	*phDisplay = (BVDC_Display_Handle)pDisplay;

	BDBG_LEAVE(BVDC_P_Display_Create);
	return BERR_SUCCESS;
}


/***************************************************************************
 * {private}
 *
 */
void BVDC_P_Display_Init
	( BVDC_Display_Handle              hDisplay )
{
	uint32_t i;
	uint32_t ulGammaTbl;
	uint32_t ulColorTempTbl;
	BVDC_P_CscCoeffs stIdentity = BVDC_P_MAKE_DVO_CSC
		(1.0000, 0.0000, 0.0000, 0.0000,
		 0.0000, 1.0000, 0.0000, 0.0000,
		 0.0000, 0.0000, 1.0000, 0.0000);
	uint32_t  ulLfsrCtrlT0, ulLfsrCtrlT1, ulLfsrCtrlT2, ulLfsrValue;

	BDBG_ENTER(BVDC_P_Display_Init);

	BKNI_Memset((void*)&hDisplay->stNewInfo, 0x0, sizeof(BVDC_P_DisplayInfo));
	BKNI_Memset((void*)&hDisplay->stCurInfo, 0x0, sizeof(BVDC_P_DisplayInfo));

	/* set default timebase:
	 * main display      (Comp0, HD)  -> timebase 0;
	 * secondary display (Comp1, SD)  -> timebase 1;
	 * Note:
	 *  bypass video path is intended for analog PVR use, and its 656 output
	 *  is free run could lock to any timebase programmed by upper level software;
	 *  the baseline clock configuration would use timebase 2 for analog PVR. */
	hDisplay->stNewInfo.eTimeBase =
		(hDisplay->eId == BVDC_DisplayId_eDisplay1)
		? BAVC_Timebase_e1 : BAVC_Timebase_e0;

	/* Default Dacs to unused */
	for(i=0; i < BVDC_P_MAX_DACS; i++)
	{
		hDisplay->stNewInfo.aDacOutput[i] = BVDC_DacOutput_eUnused;
	}

	/* Default CSC */
	hDisplay->stDvoCscMatrix.ulMin       = 0x0400;
	hDisplay->stDvoCscMatrix.ulMax       = 0x03ff;
	hDisplay->stDvoCscMatrix.stCscCoeffs = stIdentity;

	/* Default to off. */
	hDisplay->stNewInfo.bEnableHdmi = false;
	hDisplay->stNewInfo.bEnable656  = false;
	hDisplay->stNewInfo.aulEnableMpaaDeci[BVDC_MpaaDeciIf_eHdmi] = 0;
	hDisplay->stNewInfo.aulEnableMpaaDeci[BVDC_MpaaDeciIf_eComponent] = 0;

	/* Init format */
	hDisplay->stNewInfo.pFmtInfo   = BFMT_GetVideoFormatInfoPtr(hDisplay->hVdc->stSettings.eVideoFormat);
	hDisplay->stNewInfo.ulVertFreq = hDisplay->stNewInfo.pFmtInfo->ulVertFreq;
	hDisplay->stNewInfo.eAspectRatio = hDisplay->stNewInfo.pFmtInfo->eAspectRatio;
	hDisplay->stNewInfo.stDirty.stBits.bAspRatio = BVDC_P_DIRTY;

	/* Set Hdmi default */
	hDisplay->stNewInfo.eHdmiOutput = BAVC_MatrixCoefficients_eUnknown;

	/* Initialize Dac output color space */
	hDisplay->stNewInfo.eOutputColorSpace = BVDC_P_Output_eUnknown;

	/* Initialize Rfm output */
	hDisplay->stNewInfo.eRfmOutput = BVDC_RfmOutput_eUnused;
	hDisplay->stCurInfo.eRfmOutput = BVDC_RfmOutput_eUnused;

	/* set default display input color space.
	 * NOTE: primary and secondary displays will always take HD color space input
	 * from compositors; while the bypass display input color space could be SD
	 * or HD depends on the VDEC source format. */
	hDisplay->stNewInfo.eCmpColorSpace = BVDC_P_CmpColorSpace_eHdYCrCb;
	/* Rate change callback */
	hDisplay->stNewInfo.pfGenericCallback = NULL;
	hDisplay->stNewInfo.pvGenericParm1    = NULL;
	hDisplay->stNewInfo.iGenericParm2     = 0;

	/* Current display rate info, update at least once at initialization */
	hDisplay->stNewInfo.stRateInfo.ulPixelClkRate    = 0;
	hDisplay->stNewInfo.stRateInfo.ulVertRefreshRate = 0;

	/* White balance */
	BVDC_P_Display_GetMaxTable(&ulGammaTbl, &ulColorTempTbl);
	hDisplay->stNewInfo.bCCEnable          = false;
	hDisplay->stNewInfo.bUserCCTable       = false;
	hDisplay->stNewInfo.ulGammaTableId     = ulGammaTbl;
	hDisplay->stNewInfo.ulColorTempId      = ulColorTempTbl;

	/* Dvo CSC adjustment values */
	hDisplay->stNewInfo.lDvoAttenuationR = BMTH_FIX_SIGNED_ITOFIX(1, BVDC_P_CSC_DVO_CX_I_BITS, BVDC_P_CSC_DVO_CX_F_BITS);
	hDisplay->stNewInfo.lDvoAttenuationG = BMTH_FIX_SIGNED_ITOFIX(1, BVDC_P_CSC_DVO_CX_I_BITS, BVDC_P_CSC_DVO_CX_F_BITS);
	hDisplay->stNewInfo.lDvoAttenuationB = BMTH_FIX_SIGNED_ITOFIX(1, BVDC_P_CSC_DVO_CX_I_BITS, BVDC_P_CSC_DVO_CX_F_BITS);
	hDisplay->stNewInfo.lDvoOffsetR      = 0;
	hDisplay->stNewInfo.lDvoOffsetG      = 0;
	hDisplay->stNewInfo.lDvoOffsetB      = 0;

	/* Rate manager info */
	hDisplay->stNewInfo.pulAnalogRateTable = NULL;
	/* hDisplay->stNewInfo.aAnalogRateInfo = {0, 0}; */

	/* VF filters */
	BKNI_Memset(hDisplay->stNewInfo.abUserVfFilterCo, false, sizeof(hDisplay->stNewInfo.abUserVfFilterCo));
	BKNI_Memset(hDisplay->stNewInfo.abUserVfFilterCvbs, false, sizeof(hDisplay->stNewInfo.abUserVfFilterCvbs));
	BKNI_Memset(hDisplay->stNewInfo.aaulUserVfFilterCo, 0, sizeof(hDisplay->stNewInfo.aaulUserVfFilterCo));
	BKNI_Memset(hDisplay->stNewInfo.aaulUserVfFilterCvbs, 0, sizeof(hDisplay->stNewInfo.aaulUserVfFilterCvbs));
	BKNI_Memset(hDisplay->apVfFilterCo, 0, sizeof(hDisplay->apVfFilterCo));
	BKNI_Memset(hDisplay->apVfFilterCvbs, 0, sizeof(hDisplay->apVfFilterCvbs));

	hDisplay->stNewInfo.pHdmiRateInfo = NULL;

	/* Dvo/Lvds spread spectrum */
	hDisplay->stNewInfo.stDvoCfg.stSpreadSpectrum.bEnable = false;
	hDisplay->stNewInfo.stDvoCfg.stSpreadSpectrum.ulFrequency = 23077;
	hDisplay->stNewInfo.stDvoCfg.stSpreadSpectrum.ulDelta = 10;

	/* misc/xvYcc parameters */
	hDisplay->stNewInfo.bXvYcc    = false;

	/* Other changable during runtime. */
	hDisplay->bUserAppliedChanges = true;

	/* Vec is not alive yet */
	hDisplay->eItState = BVDC_P_ItState_eNotActive;
	hDisplay->eState   = BVDC_P_State_eInactive;

	/* Vec Phase not being adjusted at startup */
	hDisplay->bVecPhaseInAdjust = false;
	hDisplay->bSetEventPending  = false;
	hDisplay->bRateManagerUpdatedPending = false;
	hDisplay->bRateManagerUpdated = false;

	/* Game mode off */
	hDisplay->hWinGameMode = NULL;
	hDisplay->pDvoRmInfo   = NULL;
	hDisplay->pRmTable     = NULL;
	hDisplay->bRmAdjusted = false;

	/* alignment off */
	hDisplay->ulCurrentTs     = 0;
	hDisplay->eNextPolarity   = BAVC_Polarity_eTopField;
	hDisplay->bAlignAdjusting = false;
	hDisplay->ulTsSampleCount = 0;
	hDisplay->ulTsSamplePeriod = 0;


	/* Dither init */
	ulLfsrCtrlT0 = BVDC_P_DITHER_DISP_CSC_LFSR_CTRL_T0;
	ulLfsrCtrlT1 = BVDC_P_DITHER_DISP_CSC_LFSR_CTRL_T1;
	ulLfsrCtrlT2 = BVDC_P_DITHER_DISP_CSC_LFSR_CTRL_T2;
	ulLfsrValue  = BVDC_P_DITHER_DISP_CSC_LFSR_VALUE;

	/* PRIM_CSC_DITHER */
	BVDC_P_Dither_Init(&hDisplay->stCscDither,
		ulLfsrCtrlT0, ulLfsrCtrlT1, ulLfsrCtrlT2, ulLfsrValue);

	/* DVI_CSC_DITHER */
	ulLfsrCtrlT0 = BVDC_P_DITHER_DISP_DVI_LFSR_CTRL_T0;
	ulLfsrCtrlT1 = BVDC_P_DITHER_DISP_DVI_LFSR_CTRL_T1;
	ulLfsrCtrlT2 = BVDC_P_DITHER_DISP_DVI_LFSR_CTRL_T2;
	ulLfsrValue  = BVDC_P_DITHER_DISP_DVI_LFSR_VALUE;
	BVDC_P_Dither_Init(&hDisplay->stDviDither,
		ulLfsrCtrlT0, ulLfsrCtrlT1, ulLfsrCtrlT2, ulLfsrValue);

	/* CSC_DITHER */
	BVDC_P_Dither_Init(&hDisplay->st656Dither,
		ulLfsrCtrlT0, ulLfsrCtrlT1, ulLfsrCtrlT2, ulLfsrValue);
	/* Special settings for 656. Default is always 1 */
	hDisplay->st656Dither.ulCh0Scale = BVDC_P_DITHER_DISP_CSC_SCALE_CH0;
	hDisplay->st656Dither.ulCh1Scale = BVDC_P_DITHER_DISP_CSC_SCALE_CH1;
	hDisplay->st656Dither.ulCh2Scale = BVDC_P_DITHER_DISP_CSC_SCALE_CH2;

	/* Reset done events */
	BKNI_ResetEvent(hDisplay->hAppliedDoneEvent);

	/* Same as new */
	hDisplay->stCurInfo = hDisplay->stNewInfo;

#if BVDC_P_SUPPORT_656_MASTER_MODE
	/* 656 in master mode */
	if(BVDC_DisplayTg_e656Dtg == hDisplay->eMasterTg)
	{
		/* The enabling 656 output logic requires this flag
		 * has different value in "stNewInfo" and "stCurInfo"
		 * so that 656 master mode can be jump started.
		 */
		hDisplay->stNewInfo.bEnable656 = true;
	}
#endif

#if BVDC_P_SUPPORT_DVO_MASTER_MODE
	/* DVI in master mode */
	if(BVDC_DisplayTg_eDviDtg == hDisplay->eMasterTg)
	{
		/* The enabling DVI output logic requires this flag
		 * has different value in "stNewInfo" and "stCurInfo"
		 * so that DVI master mode can be jump started.
		 */
		hDisplay->stNewInfo.bEnableHdmi = true;
	}
#endif


	BDBG_LEAVE(BVDC_P_Display_Init);
	return;
}


/***************************************************************************
 * {private}
 *
 */
void BVDC_P_Display_Destroy
	( BVDC_Display_Handle              hDisplay )
{
	BDBG_ENTER(BVDC_P_Display_Destroy);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);
	BDBG_OBJECT_ASSERT(hDisplay->hVdc, BVDC_VDC);

	/* At this point application should have disable all the
	 * callbacks &slots */

	/* [3] Remove display handle from main VDC handle */
	hDisplay->hVdc->ahDisplay[hDisplay->eId] = NULL;

	/* [2] Destroy event */
	BKNI_DestroyEvent(hDisplay->hAppliedDoneEvent);

	/* [1] Release RDC scratch register before release context */
	BRDC_FreeScratchReg(hDisplay->hVdc->hRdc, hDisplay->ulScratchTsAddr);
	BRDC_FreeScratchReg(hDisplay->hVdc->hRdc, hDisplay->ulRdcVarAddr);

	BDBG_OBJECT_DESTROY(hDisplay, BVDC_DSP);
	/* [0] Release context in system memory */
	BKNI_Free((void*)hDisplay);

	BDBG_LEAVE(BVDC_P_Display_Destroy);
	return;
}


/*************************************************************************
 *  {secret}
 *  BVDC_P_Display_GetDacSetting
 *
 *  Validate combinations that are supported. RGB, SDYPrPb, HDYPrPb, SVideo
 *  Mark the combinations found, so we don't repeat the search. Pass the
 *  updated array back. Return the output associated with the Dac settings
 *  in peOutput(ex: YQI, RGB, etc) for internal use.
 **************************************************************************/
static void BVDC_P_Display_GetDacSetting
	( BVDC_Display_Handle              hDisplay,
	  uint32_t                         uiIndex,
	  BVDC_P_Output                   *peOutput )
{
	BVDC_P_DisplayInfo *pNewInfo;

	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);
	pNewInfo = &hDisplay->stNewInfo;

	switch(pNewInfo->aDacOutput[uiIndex])
	{
		case BVDC_DacOutput_eComposite:
		case BVDC_DacOutput_eVdec0:
			*peOutput = BVDC_P_Output_eYQI;
			break;

		case BVDC_DacOutput_eSVideo_Luma:
		case BVDC_DacOutput_eSVideo_Chroma:
			if( BVDC_P_Display_FindDac(hDisplay, BVDC_DacOutput_eSVideo_Luma) &&
				 BVDC_P_Display_FindDac(hDisplay, BVDC_DacOutput_eSVideo_Chroma))
			{
				*peOutput = BVDC_P_Output_eYQI;
			}
			break;

		case BVDC_DacOutput_eRed:
		case BVDC_DacOutput_eGreen:
		case BVDC_DacOutput_eGreen_NoSync:
		case BVDC_DacOutput_eBlue:
			if( BVDC_P_Display_FindDac(hDisplay, BVDC_DacOutput_eRed) &&
				(BVDC_P_Display_FindDac(hDisplay, BVDC_DacOutput_eGreen) ||
				 BVDC_P_Display_FindDac(hDisplay, BVDC_DacOutput_eGreen_NoSync)) &&
				 BVDC_P_Display_FindDac(hDisplay, BVDC_DacOutput_eBlue) )
			{
				*peOutput = BVDC_P_Output_eRGB;
			}
			break;

		case BVDC_DacOutput_eY:
		case BVDC_DacOutput_ePr:
		case BVDC_DacOutput_ePb:
			if ( BVDC_P_Display_FindDac(hDisplay, BVDC_DacOutput_eY) &&
				 BVDC_P_Display_FindDac(hDisplay, BVDC_DacOutput_ePr) &&
				 BVDC_P_Display_FindDac(hDisplay, BVDC_DacOutput_ePb))
			{
				/* BT.601 (480i,Pal,480p) */
				if (VIDEO_FORMAT_27Mhz(pNewInfo->pFmtInfo->ulPxlFreqMask))
				{
					*peOutput = BVDC_P_Output_eSDYPrPb;
				}
				else
				{	/* BT.709 (1080i,720p) */
					*peOutput = BVDC_P_Output_eHDYPrPb;
				}
			}
			break;

		case BVDC_DacOutput_eHsync:
			*peOutput = BVDC_P_Output_eHsync;
			break;

		default:
			*peOutput = BVDC_P_Output_eUnknown;
			break;
	};

	return;
}

/*************************************************************************
 *
 */
static void BVDC_P_Vec_Init_Misc_isr
	( BVDC_P_Context           *pVdc )
{
	uint32_t ulReg;

#if (BVDC_P_SUPPORT_TDAC_VER == BVDC_P_SUPPORT_TDAC_VER_0)
	uint32_t ulQdacAdj = pVdc->stSettings.aulDacBandGapAdjust[0];
#elif (BVDC_P_SUPPORT_TDAC_VER == BVDC_P_SUPPORT_TDAC_VER_2)
	/* TODO: Need to confirm if TDAC = DAC[0-2] and QDAC = DAC[3-6] or
	   TDAC = DAC[4-6] and QDAC = DAC[0-3] */
	uint32_t ulTdacAdj = pVdc->stSettings.aulDacBandGapAdjust[0];
	uint32_t ulQdacAdj = pVdc->stSettings.aulDacBandGapAdjust[3];
#elif (BVDC_P_SUPPORT_TDAC_VER == BVDC_P_SUPPORT_TDAC_VER_5)
	uint32_t ulTdacAdj = pVdc->stSettings.aulDacBandGapAdjust[0];
#elif ((BVDC_P_SUPPORT_TDAC_VER == BVDC_P_SUPPORT_TDAC_VER_1) || \
       (BVDC_P_SUPPORT_TDAC_VER == BVDC_P_SUPPORT_TDAC_VER_3) || \
       (BVDC_P_SUPPORT_TDAC_VER == BVDC_P_SUPPORT_TDAC_VER_4))
	uint32_t ulTdac0Adj = pVdc->stSettings.aulDacBandGapAdjust[0];
	uint32_t ulTdac1Adj = pVdc->stSettings.aulDacBandGapAdjust[3];
#endif

	BDBG_ENTER(BVDC_P_Vec_Init_Misc_isr);

	/* MISC_OUT_CTRL: Default for Vec misc regs */
	pVdc->aulMiscRegs[BVDC_P_DISP_GET_REG_IDX(MISC_OUT_CTRL)] &= ~(
#if (BVDC_P_MAX_DACS > 6)
		BCHP_MASK(MISC_OUT_CTRL, DAC6_SINC) |
#endif
#if (BVDC_P_MAX_DACS > 5)
		BCHP_MASK(MISC_OUT_CTRL, DAC5_SINC) |
#endif
#if (BVDC_P_MAX_DACS > 4)
		BCHP_MASK(MISC_OUT_CTRL, DAC4_SINC) |
#endif
#if (BVDC_P_MAX_DACS > 3)
		BCHP_MASK(MISC_OUT_CTRL, DAC3_SINC) |
#endif
#if (BVDC_P_MAX_DACS > 2)
		BCHP_MASK(MISC_OUT_CTRL, DAC2_SINC) |
#endif
		BCHP_MASK(MISC_OUT_CTRL, DAC1_SINC) |
		BCHP_MASK(MISC_OUT_CTRL, DAC0_SINC));

	pVdc->aulMiscRegs[BVDC_P_DISP_GET_REG_IDX(MISC_OUT_CTRL)] |=  (
#if (BVDC_P_MAX_DACS > 6)
		BCHP_FIELD_ENUM(MISC_OUT_CTRL, DAC6_SINC ,ON) |
#endif
#if (BVDC_P_MAX_DACS > 5)
		BCHP_FIELD_ENUM(MISC_OUT_CTRL, DAC5_SINC ,ON) |
#endif
#if (BVDC_P_MAX_DACS > 4)
		BCHP_FIELD_ENUM(MISC_OUT_CTRL, DAC4_SINC ,ON) |
#endif
#if (BVDC_P_MAX_DACS > 3)
		BCHP_FIELD_ENUM(MISC_OUT_CTRL, DAC3_SINC ,ON) |
#endif
#if (BVDC_P_MAX_DACS > 2)
		BCHP_FIELD_ENUM(MISC_OUT_CTRL, DAC2_SINC ,ON) |
#endif
		BCHP_FIELD_ENUM(MISC_OUT_CTRL, DAC1_SINC ,ON) |
		BCHP_FIELD_ENUM(MISC_OUT_CTRL, DAC0_SINC ,ON));

#if BVDC_P_SUPPORT_RFM_OUTPUT
	/* MISC_OUT_MUX: Default for Vec misc regs */
	pVdc->aulMiscRegs[BVDC_P_DISP_GET_REG_IDX(MISC_OUT_MUX)] &= ~(
		BCHP_MASK(MISC_OUT_MUX, RFM_SEL   ) );

	pVdc->aulMiscRegs[BVDC_P_DISP_GET_REG_IDX(MISC_OUT_MUX)] |=  (
		BCHP_FIELD_ENUM(MISC_OUT_MUX, RFM_SEL  , PRIM ) );
#endif

#if BCHP_MISC_SYNC_DELAY_REG
	/* MISC_SYNC_DELAY_REG: default for 7400 ext_sync */
	pVdc->aulMiscRegs[BVDC_P_DISP_GET_REG_IDX(MISC_SYNC_DELAY_REG)] &= ~(
		BCHP_MASK(MISC_SYNC_DELAY_REG, EXT_SYNC0 ) |
		BCHP_MASK(MISC_SYNC_DELAY_REG, EXT_SYNC1 ) );

	pVdc->aulMiscRegs[BVDC_P_DISP_GET_REG_IDX(MISC_SYNC_DELAY_REG)] |=  (
		BCHP_FIELD_ENUM(MISC_SYNC_DELAY_REG, EXT_SYNC0 , SEC ) |
		BCHP_FIELD_ENUM(MISC_SYNC_DELAY_REG, EXT_SYNC1 , PRIM) );

	BREG_Write32(pVdc->hRegister, BCHP_MISC_SYNC_DELAY_REG,
		pVdc->aulMiscRegs[BVDC_P_DISP_GET_REG_IDX(MISC_SYNC_DELAY_REG)]);
#endif

	/* --- Setup Video_Enc --- */
	/* Disable source select */

	ulReg = BCHP_FIELD_ENUM(VIDEO_ENC_DECIM_SRC_SEL, SOURCE, DISABLE);
	BREG_Write32(pVdc->hRegister, BCHP_VIDEO_ENC_DECIM_SRC_SEL, ulReg);

	ulReg = BCHP_FIELD_ENUM(VIDEO_ENC_PRIM_SRC_SEL, SOURCE, DISABLE);
	BREG_Write32(pVdc->hRegister, BCHP_VIDEO_ENC_PRIM_SRC_SEL, ulReg);

#if BVDC_P_SUPPORT_SEC_VEC
	ulReg = BCHP_FIELD_ENUM(VIDEO_ENC_SEC_SRC_SEL, SOURCE, DISABLE);
	BREG_Write32(pVdc->hRegister, BCHP_VIDEO_ENC_SEC_SRC_SEL, ulReg);
#endif

#if BVDC_P_SUPPORT_TER_VEC
	ulReg = BCHP_FIELD_ENUM(VIDEO_ENC_TERT_SRC_SEL, SOURCE, DISABLE);
	BREG_Write32(pVdc->hRegister, BCHP_VIDEO_ENC_TERT_SRC_SEL, ulReg);
#endif

/* T-DAC, and Q-DAC Settings */
#if (BVDC_P_SUPPORT_TDAC_VER == BVDC_P_SUPPORT_TDAC_VER_1)
	/* MISC_TDAC0_CTRL_REG: Default for Vec misc regs */
	pVdc->aulMiscRegs[BVDC_P_DISP_GET_REG_IDX(MISC_TDAC0_CTRL_REG)] &= ~(
		BCHP_MASK(MISC_TDAC0_CTRL_REG, CLK_EDGE) |
		BCHP_MASK(MISC_TDAC0_CTRL_REG, BG_PTATADJ) |
		BCHP_MASK(MISC_TDAC0_CTRL_REG, BG_CTATADJ));

	pVdc->aulMiscRegs[BVDC_P_DISP_GET_REG_IDX(MISC_TDAC0_CTRL_REG)] |= (
		BCHP_FIELD_ENUM(MISC_TDAC0_CTRL_REG, CLK_EDGE, FALL) |
		BCHP_FIELD_DATA(MISC_TDAC0_CTRL_REG, BG_PTATADJ, ulTdac0Adj) |
		BCHP_FIELD_DATA(MISC_TDAC0_CTRL_REG, BG_CTATADJ, ulTdac0Adj));

	/* MISC_TDAC1_CTRL_REG: Default for Vec misc regs */
	pVdc->aulMiscRegs[BVDC_P_DISP_GET_REG_IDX(MISC_TDAC1_CTRL_REG)]&= ~(
		BCHP_MASK(MISC_TDAC0_CTRL_REG, CLK_EDGE) |
		BCHP_MASK(MISC_TDAC0_CTRL_REG, BG_PTATADJ) |
		BCHP_MASK(MISC_TDAC0_CTRL_REG, BG_CTATADJ));

	pVdc->aulMiscRegs[BVDC_P_DISP_GET_REG_IDX(MISC_TDAC1_CTRL_REG)] |= (
		BCHP_FIELD_ENUM(MISC_TDAC0_CTRL_REG, CLK_EDGE, FALL) |
		BCHP_FIELD_DATA(MISC_TDAC0_CTRL_REG, BG_PTATADJ, ulTdac1Adj) |
		BCHP_FIELD_DATA(MISC_TDAC0_CTRL_REG, BG_CTATADJ, ulTdac1Adj));

	BREG_Write32(pVdc->hRegister, BCHP_MISC_TDAC0_CTRL_REG,
		pVdc->aulMiscRegs[BVDC_P_DISP_GET_REG_IDX(MISC_TDAC0_CTRL_REG)]);
	BREG_Write32(pVdc->hRegister, BCHP_MISC_TDAC1_CTRL_REG,
		pVdc->aulMiscRegs[BVDC_P_DISP_GET_REG_IDX(MISC_TDAC1_CTRL_REG)]);

#elif (BVDC_P_SUPPORT_TDAC_VER == BVDC_P_SUPPORT_TDAC_VER_2)
	/* MISC_TDAC_BG_CTRL_REG: power up band gap */
	/* Unused bit14 must be set */
	pVdc->aulMiscRegs[BVDC_P_DISP_GET_REG_IDX(MISC_TDAC_BG_CTRL_REG)] &= ~(
		BCHP_MASK(MISC_TDAC_BG_CTRL_REG, UNUSED)  |
		BCHP_MASK(MISC_TDAC_BG_CTRL_REG, PWRDN)  |
		BCHP_MASK(MISC_TDAC_BG_CTRL_REG, CORE_ADJ)  |
		BCHP_MASK(MISC_TDAC_BG_CTRL_REG, BANDGAP_BYP)  |
		BCHP_MASK(MISC_TDAC_BG_CTRL_REG, IREF_ADJ)  |
		BCHP_MASK(MISC_TDAC_BG_CTRL_REG, PWRDN_REFAMP) );

	pVdc->aulMiscRegs[BVDC_P_DISP_GET_REG_IDX(MISC_TDAC_BG_CTRL_REG)] |= (
		BCHP_FIELD_DATA(MISC_TDAC_BG_CTRL_REG, UNUSED, 0) |
		BCHP_FIELD_ENUM(MISC_TDAC_BG_CTRL_REG, PWRDN, PWRDN) |
		BCHP_FIELD_ENUM(MISC_TDAC_BG_CTRL_REG, CORE_ADJ, FOUR) |
		BCHP_FIELD_ENUM(MISC_TDAC_BG_CTRL_REG, BANDGAP_BYP, BANDGAP) |
		BCHP_FIELD_DATA(MISC_TDAC_BG_CTRL_REG, IREF_ADJ,  ulTdacAdj) |
		BCHP_FIELD_ENUM(MISC_TDAC_BG_CTRL_REG, PWRDN_REFAMP, POWER_DOWN));

	BREG_Write32(pVdc->hRegister, BCHP_MISC_TDAC_BG_CTRL_REG,
		pVdc->aulMiscRegs[BVDC_P_DISP_GET_REG_IDX(MISC_TDAC_BG_CTRL_REG)]);

	pVdc->aulMiscRegs[BVDC_P_DISP_GET_REG_IDX(MISC_TDAC1_CTRL_REG)] &= ~(
		BCHP_MASK(MISC_TDAC1_CTRL_REG, PWRUP_BAIS)  |
		BCHP_MASK(MISC_TDAC1_CTRL_REG, PWRDN)  |
		BCHP_MASK(MISC_TDAC1_CTRL_REG, TCOBB)  |
		BCHP_MASK(MISC_TDAC1_CTRL_REG, DCWORD)  |
		BCHP_MASK(MISC_TDAC1_CTRL_REG, DCENBL));

	pVdc->aulMiscRegs[BVDC_P_DISP_GET_REG_IDX(MISC_TDAC1_CTRL_REG)] |= (
		BCHP_FIELD_ENUM(MISC_TDAC1_CTRL_REG, PWRUP_BAIS, PWRDN)  |
		BCHP_FIELD_ENUM(MISC_TDAC1_CTRL_REG, PWRDN, PWRDN)  |
		BCHP_FIELD_ENUM(MISC_TDAC1_CTRL_REG, TCOBB, OFFSET)  |
		BCHP_FIELD_DATA(MISC_TDAC1_CTRL_REG, DCWORD, 0)  |
		BCHP_FIELD_ENUM(MISC_TDAC1_CTRL_REG, DCENBL, NORM));

	BREG_Write32(pVdc->hRegister, BCHP_MISC_TDAC1_CTRL_REG,
		pVdc->aulMiscRegs[BVDC_P_DISP_GET_REG_IDX(MISC_TDAC1_CTRL_REG)]);

	pVdc->aulMiscRegs[BVDC_P_DISP_GET_REG_IDX(MISC_TDAC2_CTRL_REG)] =
		pVdc->aulMiscRegs[BVDC_P_DISP_GET_REG_IDX(MISC_TDAC1_CTRL_REG)];

	BREG_Write32(pVdc->hRegister, BCHP_MISC_TDAC2_CTRL_REG,
		pVdc->aulMiscRegs[BVDC_P_DISP_GET_REG_IDX(MISC_TDAC2_CTRL_REG)]);

	pVdc->aulMiscRegs[BVDC_P_DISP_GET_REG_IDX(MISC_TDAC3_CTRL_REG)] =
		pVdc->aulMiscRegs[BVDC_P_DISP_GET_REG_IDX(MISC_TDAC1_CTRL_REG)];

	BREG_Write32(pVdc->hRegister, BCHP_MISC_TDAC3_CTRL_REG,
		pVdc->aulMiscRegs[BVDC_P_DISP_GET_REG_IDX(MISC_TDAC3_CTRL_REG)]);

#elif (BVDC_P_SUPPORT_TDAC_VER == BVDC_P_SUPPORT_TDAC_VER_3)
	/* MISC_TDAC[0-1]_BG_CTRL_REG: power up band gap */
	/* Unused bit14 must be set */
	pVdc->aulMiscRegs[BVDC_P_DISP_GET_REG_IDX(MISC_TDAC0_BG_CTRL_REG)] &= ~(
		BCHP_MASK(MISC_TDAC0_BG_CTRL_REG, UNUSED      ) |
		BCHP_MASK(MISC_TDAC0_BG_CTRL_REG, PWRDN       ) |
		BCHP_MASK(MISC_TDAC0_BG_CTRL_REG, CORE_ADJ    ) |
		BCHP_MASK(MISC_TDAC0_BG_CTRL_REG, BANDGAP_BYP ) |
		BCHP_MASK(MISC_TDAC0_BG_CTRL_REG, IREF_ADJ    ) |
		BCHP_MASK(MISC_TDAC0_BG_CTRL_REG, PWRDN_REFAMP));

	pVdc->aulMiscRegs[BVDC_P_DISP_GET_REG_IDX(MISC_TDAC0_BG_CTRL_REG)] |= (
		BCHP_FIELD_DATA(MISC_TDAC0_BG_CTRL_REG, UNUSED,       0         ) |
		BCHP_FIELD_ENUM(MISC_TDAC0_BG_CTRL_REG, PWRDN,        PWRDN     ) |
		BCHP_FIELD_ENUM(MISC_TDAC0_BG_CTRL_REG, CORE_ADJ,     FOUR      ) |
		BCHP_FIELD_ENUM(MISC_TDAC0_BG_CTRL_REG, BANDGAP_BYP,  BANDGAP   ) |
		BCHP_FIELD_DATA(MISC_TDAC0_BG_CTRL_REG, IREF_ADJ,     ulTdac0Adj) |
		BCHP_FIELD_ENUM(MISC_TDAC0_BG_CTRL_REG, PWRDN_REFAMP, POWER_DOWN));

	pVdc->aulMiscRegs[BVDC_P_DISP_GET_REG_IDX(MISC_TDAC1_BG_CTRL_REG)] &= ~(
		BCHP_MASK(MISC_TDAC1_BG_CTRL_REG, UNUSED      ) |
		BCHP_MASK(MISC_TDAC1_BG_CTRL_REG, PWRDN       ) |
		BCHP_MASK(MISC_TDAC1_BG_CTRL_REG, CORE_ADJ    ) |
		BCHP_MASK(MISC_TDAC1_BG_CTRL_REG, BANDGAP_BYP ) |
		BCHP_MASK(MISC_TDAC1_BG_CTRL_REG, IREF_ADJ    ) |
		BCHP_MASK(MISC_TDAC1_BG_CTRL_REG, PWRDN_REFAMP));

	pVdc->aulMiscRegs[BVDC_P_DISP_GET_REG_IDX(MISC_TDAC1_BG_CTRL_REG)] |= (
		BCHP_FIELD_DATA(MISC_TDAC1_BG_CTRL_REG, UNUSED,       0         ) |
		BCHP_FIELD_ENUM(MISC_TDAC1_BG_CTRL_REG, PWRDN,        PWRDN     ) |
		BCHP_FIELD_ENUM(MISC_TDAC1_BG_CTRL_REG, CORE_ADJ,     FOUR      ) |
		BCHP_FIELD_ENUM(MISC_TDAC1_BG_CTRL_REG, BANDGAP_BYP,  BANDGAP   ) |
		BCHP_FIELD_DATA(MISC_TDAC1_BG_CTRL_REG, IREF_ADJ,     ulTdac1Adj) |
		BCHP_FIELD_ENUM(MISC_TDAC1_BG_CTRL_REG, PWRDN_REFAMP, POWER_DOWN));

	pVdc->aulMiscRegs[BVDC_P_DISP_GET_REG_IDX(MISC_TDAC01_CTRL_REG)] &= ~(
		BCHP_MASK(MISC_TDAC01_CTRL_REG, PWRUP_BAIS) |
		BCHP_MASK(MISC_TDAC01_CTRL_REG, PWRDN     ) |
		BCHP_MASK(MISC_TDAC01_CTRL_REG, TCOBB     ) |
		BCHP_MASK(MISC_TDAC01_CTRL_REG, DCWORD    ) |
		BCHP_MASK(MISC_TDAC01_CTRL_REG, DCENBL    ));

	pVdc->aulMiscRegs[BVDC_P_DISP_GET_REG_IDX(MISC_TDAC01_CTRL_REG)] |= (
		BCHP_FIELD_ENUM(MISC_TDAC01_CTRL_REG, PWRUP_BAIS, PWRDN ) |
		BCHP_FIELD_ENUM(MISC_TDAC01_CTRL_REG, PWRDN,      PWRDN ) |
		BCHP_FIELD_ENUM(MISC_TDAC01_CTRL_REG, TCOBB,      OFFSET) |
		BCHP_FIELD_DATA(MISC_TDAC01_CTRL_REG, DCWORD,     0     ) |
		BCHP_FIELD_ENUM(MISC_TDAC01_CTRL_REG, DCENBL,     NORM  ));

	pVdc->aulMiscRegs[BVDC_P_DISP_GET_REG_IDX(MISC_TDAC02_CTRL_REG)] =
		pVdc->aulMiscRegs[BVDC_P_DISP_GET_REG_IDX(MISC_TDAC01_CTRL_REG)];
	pVdc->aulMiscRegs[BVDC_P_DISP_GET_REG_IDX(MISC_TDAC03_CTRL_REG)] =
		pVdc->aulMiscRegs[BVDC_P_DISP_GET_REG_IDX(MISC_TDAC01_CTRL_REG)];
	pVdc->aulMiscRegs[BVDC_P_DISP_GET_REG_IDX(MISC_TDAC11_CTRL_REG)] =
		pVdc->aulMiscRegs[BVDC_P_DISP_GET_REG_IDX(MISC_TDAC01_CTRL_REG)];
	pVdc->aulMiscRegs[BVDC_P_DISP_GET_REG_IDX(MISC_TDAC12_CTRL_REG)] =
		pVdc->aulMiscRegs[BVDC_P_DISP_GET_REG_IDX(MISC_TDAC01_CTRL_REG)];
	pVdc->aulMiscRegs[BVDC_P_DISP_GET_REG_IDX(MISC_TDAC13_CTRL_REG)] =
		pVdc->aulMiscRegs[BVDC_P_DISP_GET_REG_IDX(MISC_TDAC01_CTRL_REG)];

	BREG_Write32(pVdc->hRegister, BCHP_MISC_TDAC0_BG_CTRL_REG,
		pVdc->aulMiscRegs[BVDC_P_DISP_GET_REG_IDX(MISC_TDAC0_BG_CTRL_REG)]);
	BREG_Write32(pVdc->hRegister, BCHP_MISC_TDAC1_BG_CTRL_REG,
		pVdc->aulMiscRegs[BVDC_P_DISP_GET_REG_IDX(MISC_TDAC1_BG_CTRL_REG)]);

	BREG_Write32(pVdc->hRegister, BCHP_MISC_TDAC01_CTRL_REG,
		pVdc->aulMiscRegs[BVDC_P_DISP_GET_REG_IDX(MISC_TDAC01_CTRL_REG)]);

	BREG_Write32(pVdc->hRegister, BCHP_MISC_TDAC02_CTRL_REG,
		pVdc->aulMiscRegs[BVDC_P_DISP_GET_REG_IDX(MISC_TDAC02_CTRL_REG)]);

	BREG_Write32(pVdc->hRegister, BCHP_MISC_TDAC03_CTRL_REG,
		pVdc->aulMiscRegs[BVDC_P_DISP_GET_REG_IDX(MISC_TDAC03_CTRL_REG)]);

	BREG_Write32(pVdc->hRegister, BCHP_MISC_TDAC11_CTRL_REG,
		pVdc->aulMiscRegs[BVDC_P_DISP_GET_REG_IDX(MISC_TDAC11_CTRL_REG)]);

	BREG_Write32(pVdc->hRegister, BCHP_MISC_TDAC12_CTRL_REG,
		pVdc->aulMiscRegs[BVDC_P_DISP_GET_REG_IDX(MISC_TDAC12_CTRL_REG)]);

	BREG_Write32(pVdc->hRegister, BCHP_MISC_TDAC13_CTRL_REG,
		pVdc->aulMiscRegs[BVDC_P_DISP_GET_REG_IDX(MISC_TDAC13_CTRL_REG)]);

#elif (BVDC_P_SUPPORT_TDAC_VER == BVDC_P_SUPPORT_TDAC_VER_4)
	/* MISC_TDAC[0-1]_BG_CTRL_REG: power up band gap */
	/* Unused bit14 must be set */
	pVdc->aulMiscRegs[BVDC_P_DISP_GET_REG_IDX(MISC_TDAC0_BG_CTRL_REG)] &= ~(
		BCHP_MASK(MISC_TDAC0_BG_CTRL_REG, UNUSED      ) |
		BCHP_MASK(MISC_TDAC0_BG_CTRL_REG, PWRDN_CORE  ) |
		BCHP_MASK(MISC_TDAC0_BG_CTRL_REG, CORE_ADJ    ) |
		BCHP_MASK(MISC_TDAC0_BG_CTRL_REG, AUX_ADJ     ) |
		BCHP_MASK(MISC_TDAC0_BG_CTRL_REG, DBG_EN_AUX  ) |
		BCHP_MASK(MISC_TDAC0_BG_CTRL_REG, PWRDN_AUX   ) |
		BCHP_MASK(MISC_TDAC0_BG_CTRL_REG, IREF_ADJ    ) |
		BCHP_MASK(MISC_TDAC0_BG_CTRL_REG, PWRDN_REFAMP));

	pVdc->aulMiscRegs[BVDC_P_DISP_GET_REG_IDX(MISC_TDAC0_BG_CTRL_REG)] |= (
		BCHP_FIELD_DATA(MISC_TDAC0_BG_CTRL_REG, UNUSED,       0         ) |
		BCHP_FIELD_ENUM(MISC_TDAC0_BG_CTRL_REG, PWRDN_CORE,   PWRDN     ) |
		BCHP_FIELD_ENUM(MISC_TDAC0_BG_CTRL_REG, CORE_ADJ,     ZERO      ) |
		BCHP_FIELD_ENUM(MISC_TDAC0_BG_CTRL_REG, AUX_ADJ,      TWO       ) |
		BCHP_FIELD_ENUM(MISC_TDAC0_BG_CTRL_REG, DBG_EN_AUX,   ZERO      ) |
		BCHP_FIELD_ENUM(MISC_TDAC0_BG_CTRL_REG, PWRDN_AUX,    POWER_DOWN) |
		BCHP_FIELD_DATA(MISC_TDAC0_BG_CTRL_REG, IREF_ADJ,     ulTdac0Adj) |
		BCHP_FIELD_ENUM(MISC_TDAC0_BG_CTRL_REG, PWRDN_REFAMP, POWER_DOWN));

	pVdc->aulMiscRegs[BVDC_P_DISP_GET_REG_IDX(MISC_TDAC1_BG_CTRL_REG)] &= ~(
		BCHP_MASK(MISC_TDAC1_BG_CTRL_REG, UNUSED      ) |
		BCHP_MASK(MISC_TDAC1_BG_CTRL_REG, PWRDN_CORE  ) |
		BCHP_MASK(MISC_TDAC1_BG_CTRL_REG, CORE_ADJ    ) |
		BCHP_MASK(MISC_TDAC1_BG_CTRL_REG, AUX_ADJ     ) |
		BCHP_MASK(MISC_TDAC1_BG_CTRL_REG, DBG_EN_AUX  ) |
		BCHP_MASK(MISC_TDAC1_BG_CTRL_REG, PWRDN_AUX   ) |
		BCHP_MASK(MISC_TDAC1_BG_CTRL_REG, IREF_ADJ    ) |
		BCHP_MASK(MISC_TDAC1_BG_CTRL_REG, PWRDN_REFAMP));

	pVdc->aulMiscRegs[BVDC_P_DISP_GET_REG_IDX(MISC_TDAC1_BG_CTRL_REG)] |= (
		BCHP_FIELD_DATA(MISC_TDAC1_BG_CTRL_REG, UNUSED,       0         ) |
		BCHP_FIELD_ENUM(MISC_TDAC1_BG_CTRL_REG, PWRDN_CORE,   PWRDN     ) |
		BCHP_FIELD_ENUM(MISC_TDAC1_BG_CTRL_REG, CORE_ADJ,     ZERO      ) |
		BCHP_FIELD_ENUM(MISC_TDAC1_BG_CTRL_REG, AUX_ADJ,      TWO       ) |
		BCHP_FIELD_ENUM(MISC_TDAC1_BG_CTRL_REG, DBG_EN_AUX,   ZERO      ) |
		BCHP_FIELD_ENUM(MISC_TDAC1_BG_CTRL_REG, PWRDN_AUX,    POWER_DOWN) |
		BCHP_FIELD_DATA(MISC_TDAC1_BG_CTRL_REG, IREF_ADJ,     ulTdac1Adj) |
		BCHP_FIELD_ENUM(MISC_TDAC1_BG_CTRL_REG, PWRDN_REFAMP, POWER_DOWN));

	pVdc->aulMiscRegs[BVDC_P_DISP_GET_REG_IDX(MISC_TDAC01_CTRL_REG)] &= ~(
		BCHP_MASK(MISC_TDAC01_CTRL_REG, UNUSED) |
		BCHP_MASK(MISC_TDAC01_CTRL_REG, PWRDN_CORE) |
		BCHP_MASK(MISC_TDAC01_CTRL_REG, CLK_INV   ) |
		BCHP_MASK(MISC_TDAC01_CTRL_REG, TC_OBB    ) |
		BCHP_MASK(MISC_TDAC01_CTRL_REG, DC_DATA   ) |
		BCHP_MASK(MISC_TDAC01_CTRL_REG, DCENBL    ));

	pVdc->aulMiscRegs[BVDC_P_DISP_GET_REG_IDX(MISC_TDAC01_CTRL_REG)] |= (
		BCHP_FIELD_DATA(MISC_TDAC0_BG_CTRL_REG, UNUSED,   1     ) |
		BCHP_FIELD_ENUM(MISC_TDAC01_CTRL_REG, PWRDN_CORE, PWRDN ) |
		BCHP_FIELD_ENUM(MISC_TDAC01_CTRL_REG, CLK_INV,    NORM  ) |
		BCHP_FIELD_ENUM(MISC_TDAC01_CTRL_REG, TC_OBB,     OFFSET) |
		BCHP_FIELD_DATA(MISC_TDAC01_CTRL_REG, DC_DATA,    0     ) |
		BCHP_FIELD_ENUM(MISC_TDAC01_CTRL_REG, DCENBL,     NORM  ));

	pVdc->aulMiscRegs[BVDC_P_DISP_GET_REG_IDX(MISC_TDAC02_CTRL_REG)] =
		pVdc->aulMiscRegs[BVDC_P_DISP_GET_REG_IDX(MISC_TDAC01_CTRL_REG)];
	pVdc->aulMiscRegs[BVDC_P_DISP_GET_REG_IDX(MISC_TDAC03_CTRL_REG)] =
		pVdc->aulMiscRegs[BVDC_P_DISP_GET_REG_IDX(MISC_TDAC01_CTRL_REG)];
	pVdc->aulMiscRegs[BVDC_P_DISP_GET_REG_IDX(MISC_TDAC11_CTRL_REG)] =
		pVdc->aulMiscRegs[BVDC_P_DISP_GET_REG_IDX(MISC_TDAC01_CTRL_REG)];
	pVdc->aulMiscRegs[BVDC_P_DISP_GET_REG_IDX(MISC_TDAC12_CTRL_REG)] =
		pVdc->aulMiscRegs[BVDC_P_DISP_GET_REG_IDX(MISC_TDAC01_CTRL_REG)];
	pVdc->aulMiscRegs[BVDC_P_DISP_GET_REG_IDX(MISC_TDAC13_CTRL_REG)] =
		pVdc->aulMiscRegs[BVDC_P_DISP_GET_REG_IDX(MISC_TDAC01_CTRL_REG)];

	BREG_Write32(pVdc->hRegister, BCHP_MISC_TDAC0_BG_CTRL_REG,
		pVdc->aulMiscRegs[BVDC_P_DISP_GET_REG_IDX(MISC_TDAC0_BG_CTRL_REG)]);
	BREG_Write32(pVdc->hRegister, BCHP_MISC_TDAC1_BG_CTRL_REG,
		pVdc->aulMiscRegs[BVDC_P_DISP_GET_REG_IDX(MISC_TDAC1_BG_CTRL_REG)]);

	BREG_Write32(pVdc->hRegister, BCHP_MISC_TDAC01_CTRL_REG,
		pVdc->aulMiscRegs[BVDC_P_DISP_GET_REG_IDX(MISC_TDAC01_CTRL_REG)]);

	BREG_Write32(pVdc->hRegister, BCHP_MISC_TDAC02_CTRL_REG,
		pVdc->aulMiscRegs[BVDC_P_DISP_GET_REG_IDX(MISC_TDAC02_CTRL_REG)]);

	BREG_Write32(pVdc->hRegister, BCHP_MISC_TDAC03_CTRL_REG,
		pVdc->aulMiscRegs[BVDC_P_DISP_GET_REG_IDX(MISC_TDAC03_CTRL_REG)]);

	BREG_Write32(pVdc->hRegister, BCHP_MISC_TDAC11_CTRL_REG,
		pVdc->aulMiscRegs[BVDC_P_DISP_GET_REG_IDX(MISC_TDAC11_CTRL_REG)]);

	BREG_Write32(pVdc->hRegister, BCHP_MISC_TDAC12_CTRL_REG,
		pVdc->aulMiscRegs[BVDC_P_DISP_GET_REG_IDX(MISC_TDAC12_CTRL_REG)]);

	BREG_Write32(pVdc->hRegister, BCHP_MISC_TDAC13_CTRL_REG,
		pVdc->aulMiscRegs[BVDC_P_DISP_GET_REG_IDX(MISC_TDAC13_CTRL_REG)]);

#elif (BVDC_P_SUPPORT_TDAC_VER == BVDC_P_SUPPORT_TDAC_VER_5)
	pVdc->aulMiscRegs[BVDC_P_DISP_GET_REG_IDX(MISC_DAC1_CTRL_REG)] &= ~(
		BCHP_MASK(MISC_DAC1_CTRL_REG, DCENBL)  |
		BCHP_MASK(MISC_DAC1_CTRL_REG, DC_DATA)  |
		BCHP_MASK(MISC_DAC1_CTRL_REG, TC_OBB)  |
		BCHP_MASK(MISC_DAC1_CTRL_REG, CLK_INV)  |
		BCHP_MASK(MISC_DAC1_CTRL_REG, PWRDN_CORE)  |
		BCHP_MASK(MISC_DAC1_CTRL_REG, PWRDN_BIAS));

	pVdc->aulMiscRegs[BVDC_P_DISP_GET_REG_IDX(MISC_DAC1_CTRL_REG)] |= (
		BCHP_FIELD_ENUM(MISC_DAC1_CTRL_REG, DCENBL, NORM)  |
		BCHP_FIELD_DATA(MISC_DAC1_CTRL_REG, DC_DATA, 0)  |
		BCHP_FIELD_ENUM(MISC_DAC1_CTRL_REG, TC_OBB, OFFSET)  |
		BCHP_FIELD_ENUM(MISC_DAC1_CTRL_REG, CLK_INV, NORM)  |
		BCHP_FIELD_ENUM(MISC_DAC1_CTRL_REG, PWRDN_CORE, PWRDN)  |
		BCHP_FIELD_ENUM(MISC_DAC1_CTRL_REG, PWRDN_BIAS, PWRDN));

	BREG_Write32(pVdc->hRegister, BCHP_MISC_DAC1_CTRL_REG,
		pVdc->aulMiscRegs[BVDC_P_DISP_GET_REG_IDX(MISC_DAC1_CTRL_REG)]);

	pVdc->aulMiscRegs[BVDC_P_DISP_GET_REG_IDX(MISC_DAC2_CTRL_REG)] =
		pVdc->aulMiscRegs[BVDC_P_DISP_GET_REG_IDX(MISC_DAC1_CTRL_REG)];

	BREG_Write32(pVdc->hRegister, BCHP_MISC_DAC2_CTRL_REG,
		pVdc->aulMiscRegs[BVDC_P_DISP_GET_REG_IDX(MISC_DAC2_CTRL_REG)]);

#if (BVDC_P_MAX_DACS > 2)
	pVdc->aulMiscRegs[BVDC_P_DISP_GET_REG_IDX(MISC_DAC3_CTRL_REG)] =
		pVdc->aulMiscRegs[BVDC_P_DISP_GET_REG_IDX(MISC_DAC1_CTRL_REG)];
	BREG_Write32(pVdc->hRegister, BCHP_MISC_DAC3_CTRL_REG,
		pVdc->aulMiscRegs[BVDC_P_DISP_GET_REG_IDX(MISC_DAC3_CTRL_REG)]);
#endif

	/* MISC_BG_CTRL_REG: power up band gap */
	pVdc->aulMiscRegs[BVDC_P_DISP_GET_REG_IDX(MISC_DAC_BG_CTRL_REG)] =
		BREG_Read32(pVdc->hRegister, BCHP_MISC_DAC_BG_CTRL_REG) & ~(
		BCHP_MASK(MISC_DAC_BG_CTRL_REG, PWRDN_REFAMP) |
		BCHP_MASK(MISC_DAC_BG_CTRL_REG, IREF_ADJ)  |
		BCHP_MASK(MISC_DAC_BG_CTRL_REG, BANDGAP_BYP)  |
		BCHP_MASK(MISC_DAC_BG_CTRL_REG, CORE_ADJ)  |
		BCHP_MASK(MISC_DAC_BG_CTRL_REG, PWRDN_CORE) );

	pVdc->aulMiscRegs[BVDC_P_DISP_GET_REG_IDX(MISC_DAC_BG_CTRL_REG)] |= (
		BCHP_FIELD_ENUM(MISC_DAC_BG_CTRL_REG, PWRDN_REFAMP, POWER_DOWN) |
		BCHP_FIELD_DATA(MISC_DAC_BG_CTRL_REG, IREF_ADJ, ulTdacAdj) |
		BCHP_FIELD_ENUM(MISC_DAC_BG_CTRL_REG, BANDGAP_BYP, BYP) |
		BCHP_FIELD_ENUM(MISC_DAC_BG_CTRL_REG, CORE_ADJ, FOUR) |
		BCHP_FIELD_ENUM(MISC_DAC_BG_CTRL_REG, PWRDN_CORE, PWRDN));

	BREG_Write32(pVdc->hRegister, BCHP_MISC_DAC_BG_CTRL_REG,
		pVdc->aulMiscRegs[BVDC_P_DISP_GET_REG_IDX(MISC_DAC_BG_CTRL_REG)]);
#endif /* BVDC_P_SUPPORT_TDAC_VER_5 */

#if (BVDC_P_SUPPORT_QDAC_VER == BVDC_P_SUPPORT_QDAC_VER_1)
	/* MISC_QDAC_BG_CTRL_REG: power up band gap */
	pVdc->aulMiscRegs[BVDC_P_DISP_GET_REG_IDX(MISC_QDAC_BG_CTRL_REG)] =
		BREG_Read32(pVdc->hRegister, BCHP_MISC_QDAC_BG_CTRL_REG) & ~(
		BCHP_MASK(MISC_QDAC_BG_CTRL_REG, PWRDN) |
		BCHP_MASK(MISC_QDAC_BG_CTRL_REG, CORE_ADJ)  |
		BCHP_MASK(MISC_QDAC_BG_CTRL_REG, BANDGAP_BYP)  |
		BCHP_MASK(MISC_QDAC_BG_CTRL_REG, IREF_ADJ)  |
		BCHP_MASK(MISC_QDAC_BG_CTRL_REG, PWRDN_REFAMP) );

	pVdc->aulMiscRegs[BVDC_P_DISP_GET_REG_IDX(MISC_QDAC_BG_CTRL_REG)] |= (
		BCHP_FIELD_ENUM(MISC_QDAC_BG_CTRL_REG, PWRDN, PWRUP) |
		BCHP_FIELD_ENUM(MISC_QDAC_BG_CTRL_REG, CORE_ADJ, FOUR) |
		BCHP_FIELD_ENUM(MISC_QDAC_BG_CTRL_REG, BANDGAP_BYP, BANDGAP) |
		BCHP_FIELD_DATA(MISC_QDAC_BG_CTRL_REG, IREF_ADJ, ulQdacAdj ) |
		BCHP_FIELD_ENUM(MISC_QDAC_BG_CTRL_REG, PWRDN_REFAMP, POWER_DOWN));

	BREG_Write32(pVdc->hRegister, BCHP_MISC_QDAC_BG_CTRL_REG,
		pVdc->aulMiscRegs[BVDC_P_DISP_GET_REG_IDX(MISC_QDAC_BG_CTRL_REG)]);

	pVdc->aulMiscRegs[BVDC_P_DISP_GET_REG_IDX(MISC_QDAC1_CTRL_REG)] &= ~(
		BCHP_MASK(MISC_QDAC1_CTRL_REG, PWRUP_BAIS)  |
		BCHP_MASK(MISC_QDAC1_CTRL_REG, PWRDN)  |
		BCHP_MASK(MISC_QDAC1_CTRL_REG, CLKINV)  |
		BCHP_MASK(MISC_QDAC1_CTRL_REG, TCOBB)  |
		BCHP_MASK(MISC_QDAC1_CTRL_REG, DCWORD)  |
		BCHP_MASK(MISC_QDAC1_CTRL_REG, DCENBL));

	pVdc->aulMiscRegs[BVDC_P_DISP_GET_REG_IDX(MISC_QDAC1_CTRL_REG)] |= (
		BCHP_FIELD_ENUM(MISC_QDAC1_CTRL_REG, PWRUP_BAIS, PWRDN)  |
		BCHP_FIELD_ENUM(MISC_QDAC1_CTRL_REG, PWRDN, PWRDN)  |
		BCHP_FIELD_DATA(MISC_QDAC1_CTRL_REG, CLKINV, 0)  |
		BCHP_FIELD_ENUM(MISC_QDAC1_CTRL_REG, TCOBB, OFFSET)  |
		BCHP_FIELD_DATA(MISC_QDAC1_CTRL_REG, DCWORD, 0)  |
		BCHP_FIELD_ENUM(MISC_QDAC1_CTRL_REG, DCENBL, NORM));

	BREG_Write32(pVdc->hRegister, BCHP_MISC_QDAC1_CTRL_REG,
		pVdc->aulMiscRegs[BVDC_P_DISP_GET_REG_IDX(MISC_QDAC1_CTRL_REG)]);

	pVdc->aulMiscRegs[BVDC_P_DISP_GET_REG_IDX(MISC_QDAC2_CTRL_REG)] =
		pVdc->aulMiscRegs[BVDC_P_DISP_GET_REG_IDX(MISC_QDAC1_CTRL_REG)];

	BREG_Write32(pVdc->hRegister, BCHP_MISC_QDAC2_CTRL_REG,
		pVdc->aulMiscRegs[BVDC_P_DISP_GET_REG_IDX(MISC_QDAC2_CTRL_REG)]);

	pVdc->aulMiscRegs[BVDC_P_DISP_GET_REG_IDX(MISC_QDAC3_CTRL_REG)] =
		pVdc->aulMiscRegs[BVDC_P_DISP_GET_REG_IDX(MISC_QDAC1_CTRL_REG)];

	BREG_Write32(pVdc->hRegister, BCHP_MISC_QDAC3_CTRL_REG,
		pVdc->aulMiscRegs[BVDC_P_DISP_GET_REG_IDX(MISC_QDAC3_CTRL_REG)]);

	pVdc->aulMiscRegs[BVDC_P_DISP_GET_REG_IDX(MISC_QDAC4_CTRL_REG)] =
		pVdc->aulMiscRegs[BVDC_P_DISP_GET_REG_IDX(MISC_QDAC1_CTRL_REG)];

	BREG_Write32(pVdc->hRegister, BCHP_MISC_QDAC4_CTRL_REG,
		pVdc->aulMiscRegs[BVDC_P_DISP_GET_REG_IDX(MISC_QDAC4_CTRL_REG)]);
#endif

	BDBG_LEAVE(BVDC_P_Vec_Init_Misc_isr);
	return ;
}

/*************************************************************************
 *
 */
void BVDC_P_Vec_Update_OutMuxes_isr
	( BVDC_Handle           hVdc )
{
	uint32_t i, j;
	BVDC_Display_Handle hDisplay;
	bool bRfmSet = false;

	BDBG_OBJECT_ASSERT(hVdc, BVDC_VDC);

	/* 1. Clear shared registers; */
#if BVDC_P_SUPPORT_RFM_OUTPUT
	BVDC_P_VDC_GET_MISC_REG_DATA(MISC_OUT_MUX) &= ~(
		BCHP_MASK(MISC_OUT_MUX, RFM_SEL ));
	BVDC_P_VDC_GET_MISC_REG_DATA(MISC_OUT_CTRL) &= ~(
		BCHP_MASK(MISC_OUT_CTRL, RF_CONST));
#endif
	BVDC_P_VDC_GET_MISC_REG_DATA(MISC_OUT_MUX) &= ~(
#if (BVDC_P_MAX_DACS > 6)
		BCHP_MASK(MISC_OUT_MUX, DAC6_SEL ) |
#endif
#if (BVDC_P_MAX_DACS > 5)
		BCHP_MASK(MISC_OUT_MUX, DAC5_SEL ) |
#endif
#if (BVDC_P_MAX_DACS > 4)
		BCHP_MASK(MISC_OUT_MUX, DAC4_SEL ) |
#endif
#if (BVDC_P_MAX_DACS > 3)
		BCHP_MASK(MISC_OUT_MUX, DAC3_SEL ) |
#endif
#if (BVDC_P_MAX_DACS > 2)
		BCHP_MASK(MISC_OUT_MUX, DAC2_SEL ) |
#endif
		BCHP_MASK(MISC_OUT_MUX, DAC1_SEL ) |
		BCHP_MASK(MISC_OUT_MUX, DAC0_SEL ) );

	/* PR46546, PR46584: Power down unused DACs, and if all unused do it
	 * for bandgap too. */
#if (BVDC_P_SUPPORT_TDAC_VER == BVDC_P_SUPPORT_TDAC_VER_5)
	BVDC_P_VDC_GET_MISC_REG_DATA(MISC_DAC1_CTRL_REG) &= ~(
		BCHP_MASK(MISC_DAC1_CTRL_REG, PWRDN_BIAS)|
		BCHP_MASK(MISC_DAC1_CTRL_REG, PWRDN_CORE));
	BVDC_P_VDC_GET_MISC_REG_DATA(MISC_DAC1_CTRL_REG) |=
		BCHP_FIELD_ENUM(MISC_DAC1_CTRL_REG, PWRDN_BIAS, PWRDN)|
		BCHP_FIELD_ENUM(MISC_DAC1_CTRL_REG, PWRDN_CORE, PWRDN);

	BVDC_P_VDC_GET_MISC_REG_DATA(MISC_DAC2_CTRL_REG) &= ~(
		BCHP_MASK(MISC_DAC2_CTRL_REG, PWRDN_BIAS)|
		BCHP_MASK(MISC_DAC2_CTRL_REG, PWRDN_CORE));
	BVDC_P_VDC_GET_MISC_REG_DATA(MISC_DAC2_CTRL_REG) |=
		BCHP_FIELD_ENUM(MISC_DAC2_CTRL_REG, PWRDN_BIAS, PWRDN)|
		BCHP_FIELD_ENUM(MISC_DAC2_CTRL_REG, PWRDN_CORE, PWRDN);

#if (BVDC_P_MAX_DACS > 2)
	BVDC_P_VDC_GET_MISC_REG_DATA(MISC_DAC3_CTRL_REG) &= ~(
		BCHP_MASK(MISC_DAC3_CTRL_REG, PWRDN_BIAS)|
		BCHP_MASK(MISC_DAC3_CTRL_REG, PWRDN_CORE));
	BVDC_P_VDC_GET_MISC_REG_DATA(MISC_DAC3_CTRL_REG) |=
		BCHP_FIELD_ENUM(MISC_DAC3_CTRL_REG, PWRDN_BIAS, PWRDN)|
		BCHP_FIELD_ENUM(MISC_DAC3_CTRL_REG, PWRDN_CORE, PWRDN);
#endif

	BVDC_P_VDC_GET_MISC_REG_DATA(MISC_DAC_BG_CTRL_REG) &= ~(
		BCHP_MASK(MISC_DAC_BG_CTRL_REG, PWRDN_CORE)|
		BCHP_MASK(MISC_DAC_BG_CTRL_REG, PWRDN_REFAMP));
	BVDC_P_VDC_GET_MISC_REG_DATA(MISC_DAC_BG_CTRL_REG) |=
		BCHP_FIELD_ENUM(MISC_DAC_BG_CTRL_REG, PWRDN_CORE, PWRDN)|
		BCHP_FIELD_ENUM(MISC_DAC_BG_CTRL_REG, PWRDN_REFAMP, POWER_DOWN);

#elif (BVDC_P_SUPPORT_TDAC_VER == BVDC_P_SUPPORT_TDAC_VER_4)
	BVDC_P_VDC_GET_MISC_REG_DATA(MISC_TDAC01_CTRL_REG) &= ~(
		BCHP_MASK(MISC_TDAC01_CTRL_REG, PWRDN_CORE));
	BVDC_P_VDC_GET_MISC_REG_DATA(MISC_TDAC01_CTRL_REG) |=
		BCHP_FIELD_ENUM(MISC_TDAC01_CTRL_REG, PWRDN_CORE, PWRDN);

	BVDC_P_VDC_GET_MISC_REG_DATA(MISC_TDAC02_CTRL_REG) &= ~(
		BCHP_MASK(MISC_TDAC02_CTRL_REG, PWRDN_CORE));
	BVDC_P_VDC_GET_MISC_REG_DATA(MISC_TDAC02_CTRL_REG) |=
		BCHP_FIELD_ENUM(MISC_TDAC02_CTRL_REG, PWRDN_CORE, PWRDN);

	BVDC_P_VDC_GET_MISC_REG_DATA(MISC_TDAC03_CTRL_REG) &= ~(
		BCHP_MASK(MISC_TDAC03_CTRL_REG, PWRDN_CORE));
	BVDC_P_VDC_GET_MISC_REG_DATA(MISC_TDAC03_CTRL_REG) |=
		BCHP_FIELD_ENUM(MISC_TDAC03_CTRL_REG, PWRDN_CORE, PWRDN);

	BVDC_P_VDC_GET_MISC_REG_DATA(MISC_TDAC0_BG_CTRL_REG) &= ~(
		BCHP_MASK(MISC_TDAC0_BG_CTRL_REG, PWRDN_CORE)|
		BCHP_MASK(MISC_TDAC0_BG_CTRL_REG, PWRDN_AUX)|
		BCHP_MASK(MISC_TDAC0_BG_CTRL_REG, PWRDN_REFAMP));
	BVDC_P_VDC_GET_MISC_REG_DATA(MISC_TDAC0_BG_CTRL_REG) |=
		BCHP_FIELD_ENUM(MISC_TDAC0_BG_CTRL_REG, PWRDN_CORE, PWRDN)|
		BCHP_FIELD_ENUM(MISC_TDAC0_BG_CTRL_REG, PWRDN_AUX, POWER_DOWN)|
		BCHP_FIELD_ENUM(MISC_TDAC0_BG_CTRL_REG, PWRDN_REFAMP, POWER_DOWN);

	BVDC_P_VDC_GET_MISC_REG_DATA(MISC_TDAC11_CTRL_REG) &= ~(
		BCHP_MASK(MISC_TDAC11_CTRL_REG, PWRDN_CORE));
	BVDC_P_VDC_GET_MISC_REG_DATA(MISC_TDAC11_CTRL_REG) |=
		BCHP_FIELD_ENUM(MISC_TDAC11_CTRL_REG, PWRDN_CORE, PWRDN);

	BVDC_P_VDC_GET_MISC_REG_DATA(MISC_TDAC12_CTRL_REG) &= ~(
		BCHP_MASK(MISC_TDAC12_CTRL_REG, PWRDN_CORE));
	BVDC_P_VDC_GET_MISC_REG_DATA(MISC_TDAC12_CTRL_REG) |=
		BCHP_FIELD_ENUM(MISC_TDAC12_CTRL_REG, PWRDN_CORE, PWRDN);

	BVDC_P_VDC_GET_MISC_REG_DATA(MISC_TDAC13_CTRL_REG) &= ~(
		BCHP_MASK(MISC_TDAC13_CTRL_REG, PWRDN_CORE));
	BVDC_P_VDC_GET_MISC_REG_DATA(MISC_TDAC13_CTRL_REG) |=
		BCHP_FIELD_ENUM(MISC_TDAC13_CTRL_REG, PWRDN_CORE, PWRDN);

	BVDC_P_VDC_GET_MISC_REG_DATA(MISC_TDAC1_BG_CTRL_REG) &= ~(
		BCHP_MASK(MISC_TDAC1_BG_CTRL_REG, PWRDN_CORE)|
		BCHP_MASK(MISC_TDAC1_BG_CTRL_REG, PWRDN_AUX)|
		BCHP_MASK(MISC_TDAC1_BG_CTRL_REG, PWRDN_REFAMP));
	BVDC_P_VDC_GET_MISC_REG_DATA(MISC_TDAC1_BG_CTRL_REG) |=
		BCHP_FIELD_ENUM(MISC_TDAC1_BG_CTRL_REG, PWRDN_CORE, PWRDN)|
		BCHP_FIELD_ENUM(MISC_TDAC1_BG_CTRL_REG, PWRDN_AUX, POWER_DOWN)|
		BCHP_FIELD_ENUM(MISC_TDAC1_BG_CTRL_REG, PWRDN_REFAMP, POWER_DOWN);

#elif (BVDC_P_SUPPORT_TDAC_VER == BVDC_P_SUPPORT_TDAC_VER_3)
	BVDC_P_VDC_GET_MISC_REG_DATA(MISC_TDAC01_CTRL_REG) &= ~(
		BCHP_MASK(MISC_TDAC01_CTRL_REG, PWRUP_BAIS)|
		BCHP_MASK(MISC_TDAC01_CTRL_REG, PWRDN));
	BVDC_P_VDC_GET_MISC_REG_DATA(MISC_TDAC01_CTRL_REG) |=
		BCHP_FIELD_ENUM(MISC_TDAC01_CTRL_REG, PWRUP_BAIS, PWRDN)|
		BCHP_FIELD_ENUM(MISC_TDAC01_CTRL_REG, PWRDN, PWRDN);

	BVDC_P_VDC_GET_MISC_REG_DATA(MISC_TDAC02_CTRL_REG) &= ~(
		BCHP_MASK(MISC_TDAC02_CTRL_REG, PWRUP_BAIS)|
		BCHP_MASK(MISC_TDAC02_CTRL_REG, PWRDN));
	BVDC_P_VDC_GET_MISC_REG_DATA(MISC_TDAC02_CTRL_REG) |=
		BCHP_FIELD_ENUM(MISC_TDAC02_CTRL_REG, PWRUP_BAIS, PWRDN)|
		BCHP_FIELD_ENUM(MISC_TDAC02_CTRL_REG, PWRDN, PWRDN);

	BVDC_P_VDC_GET_MISC_REG_DATA(MISC_TDAC03_CTRL_REG) &= ~(
		BCHP_MASK(MISC_TDAC03_CTRL_REG, PWRUP_BAIS)|
		BCHP_MASK(MISC_TDAC03_CTRL_REG, PWRDN));
	BVDC_P_VDC_GET_MISC_REG_DATA(MISC_TDAC03_CTRL_REG) |=
		BCHP_FIELD_ENUM(MISC_TDAC03_CTRL_REG, PWRUP_BAIS, PWRDN)|
		BCHP_FIELD_ENUM(MISC_TDAC03_CTRL_REG, PWRDN, PWRDN);

	BVDC_P_VDC_GET_MISC_REG_DATA(MISC_TDAC0_BG_CTRL_REG) &= ~(
		BCHP_MASK(MISC_TDAC0_BG_CTRL_REG, PWRDN)|
		BCHP_MASK(MISC_TDAC0_BG_CTRL_REG, PWRDN_REFAMP));
	BVDC_P_VDC_GET_MISC_REG_DATA(MISC_TDAC0_BG_CTRL_REG) |=
		BCHP_FIELD_ENUM(MISC_TDAC0_BG_CTRL_REG, PWRDN, PWRDN)|
		BCHP_FIELD_ENUM(MISC_TDAC0_BG_CTRL_REG, PWRDN_REFAMP, POWER_DOWN);

	BVDC_P_VDC_GET_MISC_REG_DATA(MISC_TDAC11_CTRL_REG) &= ~(
		BCHP_MASK(MISC_TDAC11_CTRL_REG, PWRUP_BAIS)|
		BCHP_MASK(MISC_TDAC11_CTRL_REG, PWRDN));
	BVDC_P_VDC_GET_MISC_REG_DATA(MISC_TDAC11_CTRL_REG) |=
		BCHP_FIELD_ENUM(MISC_TDAC11_CTRL_REG, PWRUP_BAIS, PWRDN)|
		BCHP_FIELD_ENUM(MISC_TDAC11_CTRL_REG, PWRDN, PWRDN);

	BVDC_P_VDC_GET_MISC_REG_DATA(MISC_TDAC12_CTRL_REG) &= ~(
		BCHP_MASK(MISC_TDAC12_CTRL_REG, PWRUP_BAIS)|
		BCHP_MASK(MISC_TDAC12_CTRL_REG, PWRDN));
	BVDC_P_VDC_GET_MISC_REG_DATA(MISC_TDAC12_CTRL_REG) |=
		BCHP_FIELD_ENUM(MISC_TDAC12_CTRL_REG, PWRUP_BAIS, PWRDN)|
		BCHP_FIELD_ENUM(MISC_TDAC12_CTRL_REG, PWRDN, PWRDN);

	BVDC_P_VDC_GET_MISC_REG_DATA(MISC_TDAC13_CTRL_REG) &= ~(
		BCHP_MASK(MISC_TDAC13_CTRL_REG, PWRUP_BAIS)|
		BCHP_MASK(MISC_TDAC13_CTRL_REG, PWRDN));
	BVDC_P_VDC_GET_MISC_REG_DATA(MISC_TDAC13_CTRL_REG) |=
		BCHP_FIELD_ENUM(MISC_TDAC13_CTRL_REG, PWRUP_BAIS, PWRDN)|
		BCHP_FIELD_ENUM(MISC_TDAC13_CTRL_REG, PWRDN, PWRDN);

	BVDC_P_VDC_GET_MISC_REG_DATA(MISC_TDAC1_BG_CTRL_REG) &= ~(
		BCHP_MASK(MISC_TDAC1_BG_CTRL_REG, PWRDN)|
		BCHP_MASK(MISC_TDAC1_BG_CTRL_REG, PWRDN_REFAMP));
	BVDC_P_VDC_GET_MISC_REG_DATA(MISC_TDAC1_BG_CTRL_REG) |=
		BCHP_FIELD_ENUM(MISC_TDAC1_BG_CTRL_REG, PWRDN, PWRDN)|
		BCHP_FIELD_ENUM(MISC_TDAC1_BG_CTRL_REG, PWRDN_REFAMP, POWER_DOWN);

#elif (BVDC_P_SUPPORT_TDAC_VER == BVDC_P_SUPPORT_TDAC_VER_2)
	BVDC_P_VDC_GET_MISC_REG_DATA(MISC_TDAC1_CTRL_REG) &= ~(
		BCHP_MASK(MISC_TDAC1_CTRL_REG, PWRUP_BAIS)|
		BCHP_MASK(MISC_TDAC1_CTRL_REG, PWRDN));
	BVDC_P_VDC_GET_MISC_REG_DATA(MISC_TDAC1_CTRL_REG) |=
		BCHP_FIELD_ENUM(MISC_TDAC1_CTRL_REG, PWRUP_BAIS, PWRDN)|
		BCHP_FIELD_ENUM(MISC_TDAC1_CTRL_REG, PWRDN, PWRDN);

	BVDC_P_VDC_GET_MISC_REG_DATA(MISC_TDAC2_CTRL_REG) &= ~(
		BCHP_MASK(MISC_TDAC2_CTRL_REG, PWRUP_BAIS)|
		BCHP_MASK(MISC_TDAC2_CTRL_REG, PWRDN));
	BVDC_P_VDC_GET_MISC_REG_DATA(MISC_TDAC2_CTRL_REG) |=
		BCHP_FIELD_ENUM(MISC_TDAC2_CTRL_REG, PWRUP_BAIS, PWRDN)|
		BCHP_FIELD_ENUM(MISC_TDAC2_CTRL_REG, PWRDN, PWRDN);

	BVDC_P_VDC_GET_MISC_REG_DATA(MISC_TDAC3_CTRL_REG) &= ~(
		BCHP_MASK(MISC_TDAC3_CTRL_REG, PWRUP_BAIS)|
		BCHP_MASK(MISC_TDAC3_CTRL_REG, PWRDN));
	BVDC_P_VDC_GET_MISC_REG_DATA(MISC_TDAC3_CTRL_REG) |=
		BCHP_FIELD_ENUM(MISC_TDAC3_CTRL_REG, PWRUP_BAIS, PWRDN)|
		BCHP_FIELD_ENUM(MISC_TDAC3_CTRL_REG, PWRDN, PWRDN);

	BVDC_P_VDC_GET_MISC_REG_DATA(MISC_TDAC_BG_CTRL_REG) &= ~(
		BCHP_MASK(MISC_TDAC_BG_CTRL_REG, PWRDN)|
		BCHP_MASK(MISC_TDAC_BG_CTRL_REG, PWRDN_REFAMP));
	BVDC_P_VDC_GET_MISC_REG_DATA(MISC_TDAC_BG_CTRL_REG) |=
		BCHP_FIELD_ENUM(MISC_TDAC_BG_CTRL_REG, PWRDN, PWRDN)|
		BCHP_FIELD_ENUM(MISC_TDAC_BG_CTRL_REG, PWRDN_REFAMP, POWER_DOWN);
#endif /* BVDC_P_SUPPORT_TDAC_VER_5 */

#if (BVDC_P_SUPPORT_QDAC_VER == BVDC_P_SUPPORT_QDAC_VER_1)
	BVDC_P_VDC_GET_MISC_REG_DATA(MISC_QDAC1_CTRL_REG) &= ~(
		BCHP_MASK(MISC_QDAC1_CTRL_REG, PWRUP_BAIS)|
		BCHP_MASK(MISC_QDAC1_CTRL_REG, PWRDN));
	BVDC_P_VDC_GET_MISC_REG_DATA(MISC_QDAC1_CTRL_REG) |=
		BCHP_FIELD_ENUM(MISC_QDAC1_CTRL_REG, PWRUP_BAIS, PWRDN)|
		BCHP_FIELD_ENUM(MISC_QDAC1_CTRL_REG, PWRDN, PWRDN);

	BVDC_P_VDC_GET_MISC_REG_DATA(MISC_QDAC2_CTRL_REG) &= ~(
		BCHP_MASK(MISC_QDAC2_CTRL_REG, PWRUP_BAIS)|
		BCHP_MASK(MISC_QDAC2_CTRL_REG, PWRDN));
	BVDC_P_VDC_GET_MISC_REG_DATA(MISC_QDAC2_CTRL_REG) |=
		BCHP_FIELD_ENUM(MISC_QDAC2_CTRL_REG, PWRUP_BAIS, PWRDN)|
		BCHP_FIELD_ENUM(MISC_QDAC2_CTRL_REG, PWRDN, PWRDN);

	BVDC_P_VDC_GET_MISC_REG_DATA(MISC_QDAC3_CTRL_REG) &= ~(
		BCHP_MASK(MISC_QDAC3_CTRL_REG, PWRUP_BAIS)|
		BCHP_MASK(MISC_QDAC3_CTRL_REG, PWRDN));
	BVDC_P_VDC_GET_MISC_REG_DATA(MISC_QDAC3_CTRL_REG) |=
		BCHP_FIELD_ENUM(MISC_QDAC3_CTRL_REG, PWRUP_BAIS, PWRDN)|
		BCHP_FIELD_ENUM(MISC_QDAC3_CTRL_REG, PWRDN, PWRDN);

	BVDC_P_VDC_GET_MISC_REG_DATA(MISC_QDAC4_CTRL_REG) &= ~(
		BCHP_MASK(MISC_QDAC4_CTRL_REG, PWRUP_BAIS)|
		BCHP_MASK(MISC_QDAC4_CTRL_REG, PWRDN));
	BVDC_P_VDC_GET_MISC_REG_DATA(MISC_QDAC4_CTRL_REG) |=
		BCHP_FIELD_ENUM(MISC_QDAC4_CTRL_REG, PWRUP_BAIS, PWRDN)|
		BCHP_FIELD_ENUM(MISC_QDAC4_CTRL_REG, PWRDN, PWRDN);

	BVDC_P_VDC_GET_MISC_REG_DATA(MISC_QDAC_BG_CTRL_REG) &= ~(
		BCHP_MASK(MISC_QDAC_BG_CTRL_REG, PWRDN)|
		BCHP_MASK(MISC_TDAC_BG_CTRL_REG, PWRDN_REFAMP));
	BVDC_P_VDC_GET_MISC_REG_DATA(MISC_QDAC_BG_CTRL_REG) |=
		BCHP_FIELD_ENUM(MISC_QDAC_BG_CTRL_REG, PWRDN, PWRDN)|
		BCHP_FIELD_ENUM(MISC_QDAC_BG_CTRL_REG, PWRDN_REFAMP, POWER_DOWN);

#endif

	/* 2. Set shared registers; */
	for(j = 0; j < BVDC_P_MAX_COMPOSITOR_COUNT; j++)
	{
		hDisplay = hVdc->ahDisplay[j];
		if(BVDC_P_STATE_IS_ACTIVE(hVdc->ahDisplay[j]) ||
		   BVDC_P_STATE_IS_CREATE(hVdc->ahDisplay[j]) ||
		   BVDC_P_STATE_IS_DESTROY(hVdc->ahDisplay[j]))
		{
			const BVDC_P_DisplayInfo *pNewInfo = &hDisplay->stNewInfo;

#if BVDC_P_SUPPORT_RFM_OUTPUT
			if(!bRfmSet)
			{
				/* MISC_OUT_MUX: RF output */
				if((BVDC_P_eVecPrimary   == pNewInfo->eRfmVecpath) ||
				   (BVDC_P_eVecSecondary == pNewInfo->eRfmVecpath) ||
				   (BVDC_P_eVecTertiary  == pNewInfo->eRfmVecpath))
				{
					/*BDBG_MSG(("RF is %s on Display[%d].",
						(BVDC_RfmOutput_eUnused != pNewInfo->eRfmOutput)
						? "Enable" : "Disable", hDisplay->eId));*/

					if(BVDC_RfmOutput_eCVBS == pNewInfo->eRfmOutput)
					{
	#if BVDC_P_SUPPORT_SEC_VEC
						BVDC_P_DISP_GET_REG_DATA(MISC_OUT_MUX) |=  (
							((BVDC_P_eVecPrimary == pNewInfo->eRfmVecpath) ?
							BCHP_FIELD_ENUM(MISC_OUT_MUX, RFM_SEL, PRIM ):
		#if BVDC_P_SUPPORT_TER_VEC
							((BVDC_P_eVecTertiary == pNewInfo->eRfmVecpath) ?
							BCHP_FIELD_ENUM(MISC_OUT_MUX, RFM_SEL, TERT ):
							BCHP_FIELD_ENUM(MISC_OUT_MUX, RFM_SEL, SEC  ))));
		#else
							BCHP_FIELD_ENUM(MISC_OUT_MUX, RFM_SEL, SEC  )));
		#endif /* BVDC_P_SUPPORT_TER_VEC */
	#else
						BVDC_P_DISP_GET_REG_DATA(MISC_OUT_MUX) |=
							BCHP_FIELD_ENUM(MISC_OUT_MUX, RFM_SEL, PRIM );
	#endif /* BVDC_P_SUPPORT_SEC_VEC */
					}
					else if(BVDC_RfmOutput_eConstant == pNewInfo->eRfmOutput)
					{
						BVDC_P_DISP_GET_REG_DATA(MISC_OUT_CTRL) |= (
							BCHP_FIELD_DATA(MISC_OUT_CTRL, RF_CONST, pNewInfo->ulRfmConst));
					}
				}

				/* only set it once; */
				if(BVDC_RfmOutput_eUnused != pNewInfo->eRfmOutput)
				{
					bRfmSet = true;
				}
			}
#endif /* BVDC_P_SUPPORT_RFM_OUTPUT */

			/* MISC_OUT_MUX: Set the Dac outputs */
			for(i = 0; i < BVDC_P_MAX_DACS; i++)
			{
				if((BVDC_P_eVecPrimary == hDisplay->eVecPath) &&
				   (BVDC_DacOutput_eUnused != pNewInfo->aDacOutput[i]))
				{
					/*BDBG_MSG(("Display[%d] setting DAC[%d] = 0x%08x", hDisplay->eId,
						i, BVDC_P_aaulPrimaryDacTable[pNewInfo->aDacOutput[i]][i])); */
					BVDC_P_DISP_GET_REG_DATA(MISC_OUT_MUX) |=
						BVDC_P_aaulPrimaryDacTable[pNewInfo->aDacOutput[i]][i];
				}
#if BVDC_P_SUPPORT_SEC_VEC
				else if((BVDC_P_eVecSecondary == hDisplay->eVecPath) &&
				        (BVDC_DacOutput_eUnused != pNewInfo->aDacOutput[i]))
				{
					/*BDBG_MSG(("Display[%d] setting DAC[%d] = 0x%08x", hDisplay->eId,
						i, BVDC_P_aaulSecondaryDacTable[pNewInfo->aDacOutput[i]][i])); */
					BVDC_P_DISP_GET_REG_DATA(MISC_OUT_MUX) |=
						BVDC_P_aaulSecondaryDacTable[pNewInfo->aDacOutput[i]][i];
				}
#endif
#if BVDC_P_SUPPORT_TER_VEC
				else if((BVDC_P_eVecTertiary == hDisplay->eVecPath) &&
				        (BVDC_DacOutput_eUnused != pNewInfo->aDacOutput[i]))
				{
					BVDC_P_DISP_GET_REG_DATA(MISC_OUT_MUX) |=
						BVDC_P_aaulTertiaryDacTable[pNewInfo->aDacOutput[i]][i];
				}
#endif
#if BVDC_P_SUPPORT_VDEC_PASSTHROUGH
				else if((BVDC_P_eVecBypass0 == hDisplay->eVecPath) &&
				        ((BVDC_DacOutput_eVdec0 == pNewInfo->aDacOutput[i]) ||
				         (BVDC_DacOutput_eIfdm0 == pNewInfo->aDacOutput[i]) ||
				         (BVDC_DacOutput_eIfdm1 == pNewInfo->aDacOutput[i])))
				{
					/* BDBG_ERR(("Display[%d] setting DAC[%d] = 0x%08x", hDisplay->eId,
						i, BVDC_P_aaulPrimaryDacTable[pNewInfo->aDacOutput[i]][i])); */
					BVDC_P_DISP_GET_REG_DATA(MISC_OUT_MUX) |=
						BVDC_P_aaulPrimaryDacTable[pNewInfo->aDacOutput[i]][i];
				}
#endif
			}

#if (BVDC_P_SUPPORT_TDAC_VER == BVDC_P_SUPPORT_TDAC_VER_5)
			if(pNewInfo->abEnableDac[0])
			{
				BVDC_P_VDC_GET_MISC_REG_DATA(MISC_DAC1_CTRL_REG) &= ~(
					BCHP_MASK(MISC_DAC1_CTRL_REG, PWRDN_BIAS)|
					BCHP_MASK(MISC_DAC1_CTRL_REG, PWRDN_CORE));
				BVDC_P_VDC_GET_MISC_REG_DATA(MISC_DAC1_CTRL_REG) |=
					BCHP_FIELD_ENUM(MISC_DAC1_CTRL_REG, PWRDN_BIAS, PWRUP)|
					BCHP_FIELD_ENUM(MISC_DAC1_CTRL_REG, PWRDN_CORE, PWRUP);
			}
			if(pNewInfo->abEnableDac[1])
			{
				BVDC_P_VDC_GET_MISC_REG_DATA(MISC_DAC2_CTRL_REG) &= ~(
					BCHP_MASK(MISC_DAC2_CTRL_REG, PWRDN_BIAS)|
					BCHP_MASK(MISC_DAC2_CTRL_REG, PWRDN_CORE));
				BVDC_P_VDC_GET_MISC_REG_DATA(MISC_DAC2_CTRL_REG) |=
					BCHP_FIELD_ENUM(MISC_DAC2_CTRL_REG, PWRDN_BIAS, PWRUP)|
					BCHP_FIELD_ENUM(MISC_DAC2_CTRL_REG, PWRDN_CORE, PWRUP);
			}
#if (BVDC_P_MAX_DACS > 2)
			if(pNewInfo->abEnableDac[2])
			{
				BVDC_P_VDC_GET_MISC_REG_DATA(MISC_DAC3_CTRL_REG) &= ~(
					BCHP_MASK(MISC_DAC3_CTRL_REG, PWRDN_BIAS)|
					BCHP_MASK(MISC_DAC3_CTRL_REG, PWRDN_CORE));
				BVDC_P_VDC_GET_MISC_REG_DATA(MISC_DAC3_CTRL_REG) |=
					BCHP_FIELD_ENUM(MISC_DAC3_CTRL_REG, PWRDN_BIAS, PWRUP)|
					BCHP_FIELD_ENUM(MISC_DAC3_CTRL_REG, PWRDN_CORE, PWRUP);
			}
#endif
			/* At least one will turn on bandgap power!*/
			if(   (pNewInfo->abEnableDac[0])
			   || (pNewInfo->abEnableDac[1])
#if (BVDC_P_MAX_DACS > 2)
			   || (pNewInfo->abEnableDac[2])
#endif
			  )
			{
				BVDC_P_VDC_GET_MISC_REG_DATA(MISC_DAC_BG_CTRL_REG) &= ~(
					BCHP_MASK(MISC_DAC_BG_CTRL_REG, PWRDN_CORE)|
					BCHP_MASK(MISC_DAC_BG_CTRL_REG, PWRDN_REFAMP));
				BVDC_P_VDC_GET_MISC_REG_DATA(MISC_DAC_BG_CTRL_REG) |=
					BCHP_FIELD_ENUM(MISC_DAC_BG_CTRL_REG, PWRDN_CORE, PWRUP)|
					BCHP_FIELD_ENUM(MISC_DAC_BG_CTRL_REG, PWRDN_REFAMP, POWER_UP);
			}
#elif (BVDC_P_SUPPORT_TDAC_VER == BVDC_P_SUPPORT_TDAC_VER_4)
			if(pNewInfo->abEnableDac[0])
			{
				BVDC_P_VDC_GET_MISC_REG_DATA(MISC_TDAC01_CTRL_REG) &= ~(
					BCHP_MASK(MISC_TDAC01_CTRL_REG, UNUSED) |
					BCHP_MASK(MISC_TDAC01_CTRL_REG, PWRDN_CORE));
				BVDC_P_VDC_GET_MISC_REG_DATA(MISC_TDAC01_CTRL_REG) |=
					BCHP_FIELD_DATA(MISC_TDAC01_CTRL_REG, UNUSED, 1) |
					BCHP_FIELD_ENUM(MISC_TDAC01_CTRL_REG, PWRDN_CORE, PWRUP);
			}
			if(pNewInfo->abEnableDac[1])
			{
				BVDC_P_VDC_GET_MISC_REG_DATA(MISC_TDAC02_CTRL_REG) &= ~(
					BCHP_MASK(MISC_TDAC02_CTRL_REG, UNUSED) |
					BCHP_MASK(MISC_TDAC02_CTRL_REG, PWRDN_CORE));
				BVDC_P_VDC_GET_MISC_REG_DATA(MISC_TDAC02_CTRL_REG) |=
					BCHP_FIELD_DATA(MISC_TDAC02_CTRL_REG, UNUSED, 1) |
					BCHP_FIELD_ENUM(MISC_TDAC02_CTRL_REG, PWRDN_CORE, PWRUP);
			}
			if(pNewInfo->abEnableDac[2])
			{
				BVDC_P_VDC_GET_MISC_REG_DATA(MISC_TDAC03_CTRL_REG) &= ~(
					BCHP_MASK(MISC_TDAC03_CTRL_REG, UNUSED) |
					BCHP_MASK(MISC_TDAC03_CTRL_REG, PWRDN_CORE));
				BVDC_P_VDC_GET_MISC_REG_DATA(MISC_TDAC03_CTRL_REG) |=
					BCHP_FIELD_DATA(MISC_TDAC03_CTRL_REG, UNUSED, 1) |
					BCHP_FIELD_ENUM(MISC_TDAC03_CTRL_REG, PWRDN_CORE, PWRUP);
			}
			/* At least one will turn on bandgap power!*/
			if(   (pNewInfo->abEnableDac[0])
			   || (pNewInfo->abEnableDac[1])
			   || (pNewInfo->abEnableDac[2])
			  )
			{
				BVDC_P_VDC_GET_MISC_REG_DATA(MISC_TDAC0_BG_CTRL_REG) &= ~(
					BCHP_MASK(MISC_TDAC0_BG_CTRL_REG, PWRDN_CORE)|
					BCHP_MASK(MISC_TDAC0_BG_CTRL_REG, PWRDN_AUX)|
					BCHP_MASK(MISC_TDAC0_BG_CTRL_REG, PWRDN_REFAMP));
				BVDC_P_VDC_GET_MISC_REG_DATA(MISC_TDAC0_BG_CTRL_REG) |=
					BCHP_FIELD_ENUM(MISC_TDAC0_BG_CTRL_REG, PWRDN_CORE, PWRUP)|
					BCHP_FIELD_ENUM(MISC_TDAC0_BG_CTRL_REG, PWRDN_AUX, POWER_UP)|
					BCHP_FIELD_ENUM(MISC_TDAC0_BG_CTRL_REG, PWRDN_REFAMP, POWER_UP);
			}

			if(pNewInfo->abEnableDac[3])
			{
				BVDC_P_VDC_GET_MISC_REG_DATA(MISC_TDAC11_CTRL_REG) &= ~(
					BCHP_MASK(MISC_TDAC11_CTRL_REG, UNUSED) |
					BCHP_MASK(MISC_TDAC11_CTRL_REG, PWRDN_CORE));
				BVDC_P_VDC_GET_MISC_REG_DATA(MISC_TDAC11_CTRL_REG) |=
					BCHP_FIELD_DATA(MISC_TDAC11_CTRL_REG, UNUSED, 1) |
					BCHP_FIELD_ENUM(MISC_TDAC11_CTRL_REG, PWRDN_CORE, PWRUP);
			}
			if(pNewInfo->abEnableDac[4])
			{
				BVDC_P_VDC_GET_MISC_REG_DATA(MISC_TDAC12_CTRL_REG) &= ~(
					BCHP_MASK(MISC_TDAC12_CTRL_REG, UNUSED) |
					BCHP_MASK(MISC_TDAC12_CTRL_REG, PWRDN_CORE));
				BVDC_P_VDC_GET_MISC_REG_DATA(MISC_TDAC12_CTRL_REG) |=
					BCHP_FIELD_DATA(MISC_TDAC12_CTRL_REG, UNUSED, 1) |
					BCHP_FIELD_ENUM(MISC_TDAC12_CTRL_REG, PWRDN_CORE, PWRUP);
			}
			if(pNewInfo->abEnableDac[5])
			{
				BVDC_P_VDC_GET_MISC_REG_DATA(MISC_TDAC13_CTRL_REG) &= ~(
					BCHP_MASK(MISC_TDAC13_CTRL_REG, UNUSED) |
					BCHP_MASK(MISC_TDAC13_CTRL_REG, PWRDN_CORE));
				BVDC_P_VDC_GET_MISC_REG_DATA(MISC_TDAC13_CTRL_REG) |=
					BCHP_FIELD_DATA(MISC_TDAC13_CTRL_REG, UNUSED, 1) |
					BCHP_FIELD_ENUM(MISC_TDAC13_CTRL_REG, PWRDN_CORE, PWRUP);
			}
			/* At least one will turn on bandgap power!*/
			if(   (pNewInfo->abEnableDac[3])
			   || (pNewInfo->abEnableDac[4])
			   || (pNewInfo->abEnableDac[5])
			  )
			{
				BVDC_P_VDC_GET_MISC_REG_DATA(MISC_TDAC1_BG_CTRL_REG) &= ~(
					BCHP_MASK(MISC_TDAC1_BG_CTRL_REG, PWRDN_CORE)|
					BCHP_MASK(MISC_TDAC1_BG_CTRL_REG, PWRDN_AUX)|
					BCHP_MASK(MISC_TDAC1_BG_CTRL_REG, PWRDN_REFAMP));
				BVDC_P_VDC_GET_MISC_REG_DATA(MISC_TDAC1_BG_CTRL_REG) |=
					BCHP_FIELD_ENUM(MISC_TDAC1_BG_CTRL_REG, PWRDN_CORE, PWRUP)|
					BCHP_FIELD_ENUM(MISC_TDAC1_BG_CTRL_REG, PWRDN_AUX, POWER_UP)|
					BCHP_FIELD_ENUM(MISC_TDAC1_BG_CTRL_REG, PWRDN_REFAMP, POWER_UP);
			}

#elif (BVDC_P_SUPPORT_TDAC_VER == BVDC_P_SUPPORT_TDAC_VER_3)
			if(pNewInfo->abEnableDac[0])
			{
				BVDC_P_VDC_GET_MISC_REG_DATA(MISC_TDAC01_CTRL_REG) &= ~(
					BCHP_MASK(MISC_TDAC01_CTRL_REG, UNUSED) |
					BCHP_MASK(MISC_TDAC01_CTRL_REG, PWRUP_BAIS)|
					BCHP_MASK(MISC_TDAC01_CTRL_REG, PWRDN));
				BVDC_P_VDC_GET_MISC_REG_DATA(MISC_TDAC01_CTRL_REG) |=
					BCHP_FIELD_DATA(MISC_TDAC01_CTRL_REG, UNUSED, 1)|
					BCHP_FIELD_ENUM(MISC_TDAC01_CTRL_REG, PWRUP_BAIS, PWRUP)|
					BCHP_FIELD_ENUM(MISC_TDAC01_CTRL_REG, PWRDN, PWRUP);
			}
			if(pNewInfo->abEnableDac[1])
			{
				BVDC_P_VDC_GET_MISC_REG_DATA(MISC_TDAC02_CTRL_REG) &= ~(
					BCHP_MASK(MISC_TDAC02_CTRL_REG, UNUSED) |
					BCHP_MASK(MISC_TDAC02_CTRL_REG, PWRUP_BAIS)|
					BCHP_MASK(MISC_TDAC02_CTRL_REG, PWRDN));
				BVDC_P_VDC_GET_MISC_REG_DATA(MISC_TDAC02_CTRL_REG) |=
					BCHP_FIELD_DATA(MISC_TDAC02_CTRL_REG, UNUSED, 1)|
					BCHP_FIELD_ENUM(MISC_TDAC02_CTRL_REG, PWRUP_BAIS, PWRUP)|
					BCHP_FIELD_ENUM(MISC_TDAC02_CTRL_REG, PWRDN, PWRUP);
			}
			if(pNewInfo->abEnableDac[2])
			{
				BVDC_P_VDC_GET_MISC_REG_DATA(MISC_TDAC03_CTRL_REG) &= ~(
					BCHP_MASK(MISC_TDAC03_CTRL_REG, UNUSED) |
					BCHP_MASK(MISC_TDAC03_CTRL_REG, PWRUP_BAIS)|
					BCHP_MASK(MISC_TDAC03_CTRL_REG, PWRDN));
				BVDC_P_VDC_GET_MISC_REG_DATA(MISC_TDAC03_CTRL_REG) |=
					BCHP_FIELD_DATA(MISC_TDAC03_CTRL_REG, UNUSED, 1)|
					BCHP_FIELD_ENUM(MISC_TDAC03_CTRL_REG, PWRUP_BAIS, PWRUP)|
					BCHP_FIELD_ENUM(MISC_TDAC03_CTRL_REG, PWRDN, PWRUP);
			}
			/* At least one will turn on bandgap power!*/
			if(   (pNewInfo->abEnableDac[0])
			   || (pNewInfo->abEnableDac[1])
			   || (pNewInfo->abEnableDac[2])
			  )
			{
				BVDC_P_VDC_GET_MISC_REG_DATA(MISC_TDAC0_BG_CTRL_REG) &= ~(
					BCHP_MASK(MISC_TDAC0_BG_CTRL_REG, PWRDN)|
					BCHP_MASK(MISC_TDAC0_BG_CTRL_REG, PWRDN_REFAMP));
				BVDC_P_VDC_GET_MISC_REG_DATA(MISC_TDAC0_BG_CTRL_REG) |=
					BCHP_FIELD_ENUM(MISC_TDAC0_BG_CTRL_REG, PWRDN, PWRUP)|
					BCHP_FIELD_ENUM(MISC_TDAC0_BG_CTRL_REG, PWRDN_REFAMP, POWER_UP);
			}

			if(pNewInfo->abEnableDac[3])
			{
				BVDC_P_VDC_GET_MISC_REG_DATA(MISC_TDAC11_CTRL_REG) &= ~(
					BCHP_MASK(MISC_TDAC11_CTRL_REG, UNUSED) |
					BCHP_MASK(MISC_TDAC11_CTRL_REG, PWRUP_BAIS)|
					BCHP_MASK(MISC_TDAC11_CTRL_REG, PWRDN));
				BVDC_P_VDC_GET_MISC_REG_DATA(MISC_TDAC11_CTRL_REG) |=
					BCHP_FIELD_DATA(MISC_TDAC11_CTRL_REG, UNUSED, 1)|
					BCHP_FIELD_ENUM(MISC_TDAC11_CTRL_REG, PWRUP_BAIS, PWRUP)|
					BCHP_FIELD_ENUM(MISC_TDAC11_CTRL_REG, PWRDN, PWRUP);
			}
			if(pNewInfo->abEnableDac[4])
			{
				BVDC_P_VDC_GET_MISC_REG_DATA(MISC_TDAC12_CTRL_REG) &= ~(
					BCHP_MASK(MISC_TDAC12_CTRL_REG, UNUSED) |
					BCHP_MASK(MISC_TDAC12_CTRL_REG, PWRUP_BAIS)|
					BCHP_MASK(MISC_TDAC12_CTRL_REG, PWRDN));
				BVDC_P_VDC_GET_MISC_REG_DATA(MISC_TDAC12_CTRL_REG) |=
					BCHP_FIELD_DATA(MISC_TDAC12_CTRL_REG, UNUSED, 1)|
					BCHP_FIELD_ENUM(MISC_TDAC12_CTRL_REG, PWRUP_BAIS, PWRUP)|
					BCHP_FIELD_ENUM(MISC_TDAC12_CTRL_REG, PWRDN, PWRUP);
			}
			if(pNewInfo->abEnableDac[5])
			{
				BVDC_P_VDC_GET_MISC_REG_DATA(MISC_TDAC13_CTRL_REG) &= ~(
					BCHP_MASK(MISC_TDAC13_CTRL_REG, UNUSED) |
					BCHP_MASK(MISC_TDAC13_CTRL_REG, PWRUP_BAIS)|
					BCHP_MASK(MISC_TDAC13_CTRL_REG, PWRDN));
				BVDC_P_VDC_GET_MISC_REG_DATA(MISC_TDAC13_CTRL_REG) |=
					BCHP_FIELD_DATA(MISC_TDAC13_CTRL_REG, UNUSED, 1)|
					BCHP_FIELD_ENUM(MISC_TDAC13_CTRL_REG, PWRUP_BAIS, PWRUP)|
					BCHP_FIELD_ENUM(MISC_TDAC13_CTRL_REG, PWRDN, PWRUP);
			}
			/* At least one will turn on bandgap power!*/
			if(   (pNewInfo->abEnableDac[3])
			   || (pNewInfo->abEnableDac[4])
			   || (pNewInfo->abEnableDac[5])
			  )
			{
				BVDC_P_VDC_GET_MISC_REG_DATA(MISC_TDAC1_BG_CTRL_REG) &= ~(
					BCHP_MASK(MISC_TDAC1_BG_CTRL_REG, PWRDN)|
					BCHP_MASK(MISC_TDAC1_BG_CTRL_REG, PWRDN_REFAMP));
				BVDC_P_VDC_GET_MISC_REG_DATA(MISC_TDAC1_BG_CTRL_REG) |=
					BCHP_FIELD_ENUM(MISC_TDAC1_BG_CTRL_REG, PWRDN, PWRUP)|
					BCHP_FIELD_ENUM(MISC_TDAC1_BG_CTRL_REG, PWRDN_REFAMP, POWER_UP);
			}

#elif (BVDC_P_SUPPORT_TDAC_VER == BVDC_P_SUPPORT_TDAC_VER_2)
			if(pNewInfo->abEnableDac[0])
			{
				BVDC_P_VDC_GET_MISC_REG_DATA(MISC_TDAC1_CTRL_REG) &= ~(
					BCHP_MASK(MISC_TDAC1_CTRL_REG, PWRUP_BAIS)|
					BCHP_MASK(MISC_TDAC1_CTRL_REG, PWRDN));
				BVDC_P_VDC_GET_MISC_REG_DATA(MISC_TDAC1_CTRL_REG) |=
					BCHP_FIELD_ENUM(MISC_TDAC1_CTRL_REG, PWRUP_BAIS, PWRUP)|
					BCHP_FIELD_ENUM(MISC_TDAC1_CTRL_REG, PWRDN, PWRUP);
			}
			if(pNewInfo->abEnableDac[1])
			{
				BVDC_P_VDC_GET_MISC_REG_DATA(MISC_TDAC2_CTRL_REG) &= ~(
					BCHP_MASK(MISC_TDAC2_CTRL_REG, PWRUP_BAIS)|
					BCHP_MASK(MISC_TDAC2_CTRL_REG, PWRDN));
				BVDC_P_VDC_GET_MISC_REG_DATA(MISC_TDAC2_CTRL_REG) |=
					BCHP_FIELD_ENUM(MISC_TDAC2_CTRL_REG, PWRUP_BAIS, PWRUP)|
					BCHP_FIELD_ENUM(MISC_TDAC2_CTRL_REG, PWRDN, PWRUP);
			}
			if(pNewInfo->abEnableDac[2])
			{
				BVDC_P_VDC_GET_MISC_REG_DATA(MISC_TDAC3_CTRL_REG) &= ~(
					BCHP_MASK(MISC_TDAC3_CTRL_REG, PWRUP_BAIS)|
					BCHP_MASK(MISC_TDAC3_CTRL_REG, PWRDN));
				BVDC_P_VDC_GET_MISC_REG_DATA(MISC_TDAC3_CTRL_REG) |=
					BCHP_FIELD_ENUM(MISC_TDAC3_CTRL_REG, PWRUP_BAIS, PWRUP)|
					BCHP_FIELD_ENUM(MISC_TDAC3_CTRL_REG, PWRDN, PWRUP);
			}
			/* At least one will turn on bandgap power!*/
			if(   (pNewInfo->abEnableDac[0])
			   || (pNewInfo->abEnableDac[1])
			   || (pNewInfo->abEnableDac[2])
			  )
			{
				BVDC_P_VDC_GET_MISC_REG_DATA(MISC_TDAC_BG_CTRL_REG) &= ~(
					BCHP_MASK(MISC_TDAC_BG_CTRL_REG, PWRDN)|
					BCHP_MASK(MISC_TDAC_BG_CTRL_REG, PWRDN_REFAMP));
				BVDC_P_VDC_GET_MISC_REG_DATA(MISC_TDAC_BG_CTRL_REG) |=
					BCHP_FIELD_ENUM(MISC_TDAC_BG_CTRL_REG, PWRDN, PWRUP)|
					BCHP_FIELD_ENUM(MISC_TDAC_BG_CTRL_REG, PWRDN_REFAMP, POWER_UP);
			}
#endif

#if (BVDC_P_SUPPORT_QDAC_VER == BVDC_P_SUPPORT_QDAC_VER_1)
			/* 7400B0 MISC_QDAC[3-6]_CTRL_REG */
			if(pNewInfo->abEnableDac[3])
			{
				BVDC_P_DISP_GET_REG_DATA(MISC_QDAC1_CTRL_REG) &= ~(
					BCHP_MASK(MISC_QDAC1_CTRL_REG, PWRUP_BAIS) |
					BCHP_MASK(MISC_QDAC1_CTRL_REG, PWRDN));

				BVDC_P_DISP_GET_REG_DATA(MISC_QDAC1_CTRL_REG) |= (
					BCHP_FIELD_ENUM(MISC_QDAC1_CTRL_REG, PWRUP_BAIS, PWRUP) |
					BCHP_FIELD_ENUM(MISC_QDAC1_CTRL_REG, PWRDN, PWRUP));
			}
			if(pNewInfo->abEnableDac[4])
			{
				BVDC_P_DISP_GET_REG_DATA(MISC_QDAC2_CTRL_REG) &= ~(
					BCHP_MASK(MISC_QDAC1_CTRL_REG, PWRUP_BAIS) |
					BCHP_MASK(MISC_QDAC1_CTRL_REG, PWRDN));

				BVDC_P_DISP_GET_REG_DATA(MISC_QDAC2_CTRL_REG) |= (
					BCHP_FIELD_ENUM(MISC_QDAC1_CTRL_REG, PWRUP_BAIS, PWRUP) |
					BCHP_FIELD_ENUM(MISC_QDAC2_CTRL_REG, PWRDN, PWRUP));
			}
			if(pNewInfo->abEnableDac[5])
			{
				BVDC_P_DISP_GET_REG_DATA(MISC_QDAC3_CTRL_REG) &= ~(
					BCHP_MASK(MISC_QDAC1_CTRL_REG, PWRUP_BAIS) |
					BCHP_MASK(MISC_QDAC1_CTRL_REG, PWRDN));

				BVDC_P_DISP_GET_REG_DATA(MISC_QDAC3_CTRL_REG) |= (
					BCHP_FIELD_ENUM(MISC_QDAC1_CTRL_REG, PWRUP_BAIS, PWRUP) |
					BCHP_FIELD_ENUM(MISC_QDAC3_CTRL_REG, PWRDN, PWRUP));
			}
			if(pNewInfo->abEnableDac[6])
			{
				BVDC_P_DISP_GET_REG_DATA(MISC_QDAC4_CTRL_REG) &= ~(
					BCHP_MASK(MISC_QDAC1_CTRL_REG, PWRUP_BAIS) |
					BCHP_MASK(MISC_QDAC1_CTRL_REG, PWRDN));

				BVDC_P_DISP_GET_REG_DATA(MISC_QDAC4_CTRL_REG) |= (
					BCHP_FIELD_ENUM(MISC_QDAC1_CTRL_REG, PWRUP_BAIS, PWRUP) |
					BCHP_FIELD_ENUM(MISC_QDAC4_CTRL_REG, PWRDN, PWRUP));
			}

			/* At least one will turn on bandgap power!*/
			if(   (pNewInfo->abEnableDac[3])
			   || (pNewInfo->abEnableDac[4])
			   || (pNewInfo->abEnableDac[5])
			   || (pNewInfo->abEnableDac[6])
			  )
			{
				BVDC_P_VDC_GET_MISC_REG_DATA(MISC_QDAC_BG_CTRL_REG) &= ~(
					BCHP_MASK(MISC_QDAC_BG_CTRL_REG, PWRDN)|
					BCHP_MASK(MISC_QDAC_BG_CTRL_REG, PWRDN_REFAMP));
				BVDC_P_VDC_GET_MISC_REG_DATA(MISC_QDAC_BG_CTRL_REG) |=
					BCHP_FIELD_ENUM(MISC_QDAC_BG_CTRL_REG, PWRDN, PWRUP)|
					BCHP_FIELD_ENUM(MISC_QDAC_BG_CTRL_REG, PWRDN_REFAMP, POWER_UP);
			}
#endif /* DAC enables */

		}
	}

	BSTD_UNUSED(bRfmSet);

	return ;
}


/*************************************************************************
 *  {secret}
 *	BVDC_P_Display_Validate
 *	Validate settings for Primary or Secondary display
 **************************************************************************/
static BERR_Code BVDC_P_Display_Validate
	( BVDC_Display_Handle           hDisplay )
{
	uint32_t                uiIndex;
	BVDC_P_Output           eOutput = BVDC_P_Output_eUnknown;
	BVDC_P_DisplayInfo      *pNewInfo, *pCurInfo;
	BFMT_VideoFmt           eNewVideoFmt;
	BERR_Code               eErr = BERR_SUCCESS;
	bool                    bYqi=false;
	bool                    bRgb=false;
	bool                    bSdYprpb=false;
	bool                    bHdYprpb=false;
	bool                    bHsync=false;

	BDBG_ENTER(BVDC_P_Display_Validate);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);

	pNewInfo = &hDisplay->stNewInfo;
	pCurInfo = &hDisplay->stCurInfo;

	/* check alignment */
	if(pNewInfo->hTargetDisplay)
	{
		/* Check alignment chain */
		if(pNewInfo->hTargetDisplay->stNewInfo.hTargetDisplay)
		{
			BDBG_ERR(("Display alignment chain is invalid!Display%d->Display%d->Display%d",
				hDisplay->eId, pNewInfo->hTargetDisplay->eId,
				pNewInfo->hTargetDisplay->stNewInfo.hTargetDisplay->eId));
			return BERR_TRACE(BVDC_ERR_INVALID_MODE_PATH);
		}

		/* check timebase */
		if(pNewInfo->eTimeBase != pNewInfo->hTargetDisplay->stNewInfo.eTimeBase)
		{
			BDBG_ERR(("Bad alignment config: Display%d's timebase(%d) != target display%d's timebase(%d)!",
				hDisplay->eId, pNewInfo->eTimeBase,
				pNewInfo->hTargetDisplay->eId, pNewInfo->hTargetDisplay->stNewInfo.eTimeBase));
			return BERR_TRACE(BVDC_ERR_INVALID_MODE_PATH);
		}

		/* check default display frame rate */
		if((hDisplay->hVdc->stSettings.eDisplayFrameRate == BAVC_FrameRateCode_e60) ||
		   (hDisplay->hVdc->stSettings.eDisplayFrameRate == BAVC_FrameRateCode_e30) ||
		   (hDisplay->hVdc->stSettings.eDisplayFrameRate == BAVC_FrameRateCode_e24))
		{
			BDBG_ERR(("Bad VDC default display rate config for VEC locking: %d!",
				hDisplay->hVdc->stSettings.eDisplayFrameRate));
		}
	}

	eNewVideoFmt = pNewInfo->pFmtInfo->eVideoFmt;

	/* Check for valid HDMI Video format */
	if ((pNewInfo->bEnableHdmi) &&
		(!VIDEO_FORMAT_IS_HDMI(eNewVideoFmt)))
	{
		BDBG_ERR(("Invalid HDMI video format."));
		return BERR_TRACE(BVDC_ERR_INVALID_HDMI_MODE);
	}

	/* Check for valid 656out support */
	if ((pNewInfo->bEnable656) &&
		(VIDEO_FORMAT_IS_HD(eNewVideoFmt)))
	{
		return BERR_TRACE(BVDC_ERR_INVALID_656OUT_MODE);
	}

	/* Check for valid Dac combination */
	for(uiIndex=0; uiIndex < BVDC_P_MAX_DACS; uiIndex++)
	{
		if (pNewInfo->aDacOutput[uiIndex] != BVDC_DacOutput_eUnused)
		{	/* Search for valid Dac combinations */
			BVDC_P_Display_GetDacSetting((BVDC_Display_Handle)hDisplay, uiIndex, &eOutput);
			switch(eOutput)
			{
			case BVDC_P_Output_eYQI:
				if ( VIDEO_FORMAT_IS_HD(eNewVideoFmt) )
				{
					return BERR_TRACE(BVDC_ERR_VIDEOFMT_OUTPUT_MISMATCH);
				}
				bYqi = true;
				break;

			case BVDC_P_Output_eRGB:
				bRgb = true;
				break;

			case BVDC_P_Output_eSDYPrPb:
				bSdYprpb = true;
				break;

			case BVDC_P_Output_eHDYPrPb:
				bHdYprpb = true;
				break;

			case BVDC_P_Output_eHsync:
				bHsync = true;
				break;

			case BVDC_P_Output_eUnknown:
			default:
				return BERR_TRACE(BVDC_ERR_INVALID_DAC_SETTINGS);
			};
		}
	}

	/* If Rfm is enabled from CVBS output, composite must be enabled at the same time. */
	if(BVDC_RfmOutput_eCVBS == pNewInfo->eRfmOutput)
	{
		if ( VIDEO_FORMAT_IS_HD(eNewVideoFmt) )
		{
			return BERR_TRACE(BVDC_ERR_VIDEOFMT_OUTPUT_MISMATCH);
		}
		bYqi = true;
	}

	/* Invalid combinations */
	if ((bYqi && bHdYprpb) ||
		(bHsync && (bYqi || (! BVDC_P_VEC_PATH_SUPPORT_CO(hDisplay->eVecPath) &&
		(hDisplay->eVecPath == BVDC_P_eVecPrimary && !BVDC_P_SUPPORT_4_DACS_PRIM_VEC)))))
	{
		return BERR_TRACE(BVDC_ERR_INVALID_DAC_SETTINGS);
	}

	/* Reset num dacs used */
	pNewInfo->uiNumDacsOn = 0;

	/* How many Dacs are we using ? */
	for(uiIndex=0; uiIndex < BVDC_P_MAX_DACS; uiIndex++)
	{
		if (pNewInfo->aDacOutput[uiIndex] != BVDC_DacOutput_eUnused)
		{
			pNewInfo->uiNumDacsOn++;
		}
	}

	/* Initialize colorspace */
	pNewInfo->eOutputColorSpace   = BVDC_P_Output_eUnknown;
	pNewInfo->eCoOutputColorSpace = BVDC_P_Output_eUnknown;

	/* Update output color space for internal use */
	if (bYqi)
	{
		if(VIDEO_FORMAT_IS_NTSC_M(eNewVideoFmt))
		{
			pNewInfo->eOutputColorSpace = BVDC_P_Output_eYQI;
		}
		else if(VIDEO_FORMAT_IS_NTSC_J(eNewVideoFmt))
		{
			pNewInfo->eOutputColorSpace = BVDC_P_Output_eYQI_M;
		}
#if BVDC_P_SUPPORT_VEC_SECAM
		else if(VIDEO_FORMAT_IS_SECAM(eNewVideoFmt))
		{
			if ((eNewVideoFmt == BFMT_VideoFmt_eSECAM_L) ||
				(eNewVideoFmt == BFMT_VideoFmt_eSECAM_D) ||
				(eNewVideoFmt == BFMT_VideoFmt_eSECAM_K))
				pNewInfo->eOutputColorSpace = BVDC_P_Output_eYDbDr_LDK;
			else if ((eNewVideoFmt == BFMT_VideoFmt_eSECAM_B) ||
					 (eNewVideoFmt == BFMT_VideoFmt_eSECAM_G))
				pNewInfo->eOutputColorSpace = BVDC_P_Output_eYDbDr_BG;
			else /* BFMT_VideoFmt_eSECAM_H */
				pNewInfo->eOutputColorSpace = BVDC_P_Output_eYDbDr_H;
		}
#endif
		else if(eNewVideoFmt == BFMT_VideoFmt_ePAL_NC)
		{
			pNewInfo->eOutputColorSpace = BVDC_P_Output_eYUV_NC;
		}
		else /* (VIDEO_FORMAT_IS_PAL(eNewVideoFmt)) */
		{
			pNewInfo->eOutputColorSpace =
				((eNewVideoFmt == BFMT_VideoFmt_ePAL_M) ||
				 (eNewVideoFmt == BFMT_VideoFmt_ePAL_N)) ?
				BVDC_P_Output_eYUV_N : BVDC_P_Output_eYUV;
		}
	}

	if (bHsync)
	{
		pNewInfo->eOutputColorSpace = BVDC_P_Output_eHsync;
	}

	if (bRgb)
	{
		if (BVDC_P_VEC_PATH_SUPPORT_CO(hDisplay->eVecPath))
		{
			pNewInfo->eCoOutputColorSpace = BVDC_P_Output_eRGB;
		}
		else
		{
			pNewInfo->eOutputColorSpace = BVDC_P_Output_eRGB;
		}
	}
	else if (bSdYprpb)
	{
		if (BVDC_P_VEC_PATH_SUPPORT_CO(hDisplay->eVecPath))
		{
			pNewInfo->eCoOutputColorSpace = BVDC_P_Output_eSDYPrPb;
		}
		else
		{
			pNewInfo->eOutputColorSpace = BVDC_P_Output_eSDYPrPb;
		}
	}
	else if (bHdYprpb)
	{
		if (BVDC_P_VEC_PATH_SUPPORT_CO(hDisplay->eVecPath))
		{
			pNewInfo->eCoOutputColorSpace = BVDC_P_Output_eHDYPrPb;
		}
		else
		{
			pNewInfo->eOutputColorSpace = BVDC_P_Output_eHDYPrPb;
		}
	}

	if( (eErr = BVDC_P_ValidateMacrovision(hDisplay)) != BERR_SUCCESS )
	{
		return BERR_TRACE(eErr);
	}

	/* Do not change DVI-DTG toggle for DVI-DTG toggle for custom formats. */
	 if((BFMT_VideoFmt_eCustom0 != eNewVideoFmt) &&
	    (BFMT_VideoFmt_eCustom1 != eNewVideoFmt) &&
	    (BFMT_VideoFmt_eCustom2 != eNewVideoFmt))
	{
		const uint32_t* toggles = BVDC_P_GetDviDtgToggles_isr (pNewInfo);
		pNewInfo->stDvoCfg.eHsPolarity = toggles[0];
		pNewInfo->stDvoCfg.eVsPolarity = toggles[1];
	}

	/* evaluate bMultiRateAllow flag */
	/* To keep NTSC(cvbs/svideo) as 59.94hz and 480i can be 60.00hz or 59.94hz */
	/* Assumptions multiple of 24/30hz are capable of multirate capable display. */
	pNewInfo->bMultiRateAllow =
		(0 == (pNewInfo->pFmtInfo->ulVertFreq % (24 * BFMT_FREQ_FACTOR))) ||
		(0 == (pNewInfo->pFmtInfo->ulVertFreq % (30 * BFMT_FREQ_FACTOR)));
	pNewInfo->bMultiRateAllow &= !bYqi;

	if(pNewInfo->bMultiRateAllow != pCurInfo->bMultiRateAllow)
	{
		pNewInfo->stDirty.stBits.bRateChange = BVDC_P_DIRTY;
	}

	BDBG_LEAVE(BVDC_P_Display_Validate);
	return BERR_SUCCESS;
}

/*************************************************************************
 *  {secret}
 * BVDC_P_Display_ValidateChanges
 *
 * Validates user's new settings for all displays
 **************************************************************************/
BERR_Code BVDC_P_Display_ValidateChanges
	( BVDC_Display_Handle              ahDisplay[] )
{
	BERR_Code                 eStatus = BERR_SUCCESS;
	BVDC_Display_Handle       hDisplay=NULL;
	BVDC_P_DisplayInfo       *pNewPriInfo=NULL;
	BVDC_P_DisplayInfo       *pNewSecInfo=NULL;
	BVDC_P_DisplayInfo       *pNewTerInfo=NULL;
	BVDC_P_DisplayInfo       *pNewBpsInfo=NULL;
	BVDC_P_DisplayInfo       *pNewInfo=NULL;
	uint32_t                  uiIndex=0;
	uint32_t                  i;
	uint32_t                  uiNumRfmOut=0;
	uint32_t                  uiNumHdmiOut=0;
	uint32_t                  uiNum656Out=0;
	uint32_t                  ulNumDacOut[BVDC_P_MAX_DACS] = {0,};
	BVDC_P_VecPath            e656path = BVDC_P_eVecBypass0;
	BVDC_P_VecPath            eHdmipath = BVDC_P_eVecBypass0;
	BVDC_P_VecPath            eRfmpath = BVDC_P_eVecBypass0;
	uint32_t                  ulRfmConstant = 0;

	BDBG_ENTER(BVDC_P_Display_ValidateChanges);

	for (i=0; i < BVDC_P_MAX_DISPLAY_COUNT; i++)
	{
		/* Only validate the create or active onces. */
		if(BVDC_P_STATE_IS_ACTIVE(ahDisplay[i]) ||
		   BVDC_P_STATE_IS_CREATE(ahDisplay[i]))
		{
			hDisplay = ahDisplay[i];
			pNewInfo = &hDisplay->stNewInfo;

			/* Can't turn on/off sync only if hdmi is disabled. */
			pNewInfo->bSyncOnly &= pNewInfo->bEnableHdmi;

			if (hDisplay->eVecPath == BVDC_P_eVecBypass0)
			{
				pNewBpsInfo = pNewInfo;
			}
			else if (hDisplay->eVecPath == BVDC_P_eVecTertiary)
			{
				pNewTerInfo = pNewInfo;
			}
			else if (hDisplay->eVecPath == BVDC_P_eVecPrimary)
			{
				pNewPriInfo = pNewInfo;
			}
			else
			{
				pNewSecInfo = pNewInfo;
			}

			eStatus = BVDC_P_Display_Validate(hDisplay);
			if (eStatus != BERR_SUCCESS)
			{
				return BERR_TRACE(eStatus);
			}

			/* Look up analog rate manager info */
			if ((hDisplay->eVecPath != BVDC_P_eVecBypass0) ||
			    (pNewInfo->bEnableHdmi))
			{
				const uint32_t          *pTable;
				BAVC_VdcDisplay_Info     lRateInfo;
				eStatus = BVDC_P_GetRmTable_isr (pNewInfo, pNewInfo->pFmtInfo, &pTable, pNewInfo->bFullRate, &lRateInfo);

				if (eStatus != BERR_SUCCESS)
				{
					if (pNewInfo->bEnableHdmi)
					{
						BDBG_ERR(("disp[%d] - Digital pixel frequency for %s not yet supportted.",
							hDisplay->eId, pNewInfo->pFmtInfo->pchFormatStr));
						return BERR_TRACE(BVDC_ERR_INVALID_HDMI_MODE);
					}
					else
					{
						BDBG_ERR(("disp[%d] - Analog pixel frequency for %s not yet supportted.",
							hDisplay->eId, pNewInfo->pFmtInfo->pchFormatStr));
						return BERR_TRACE(BVDC_ERR_FORMAT_NOT_SUPPORT_ANALOG_OUTPUT);
					}
				}

				pNewInfo->pulAnalogRateTable = pTable;
				pNewInfo->aAnalogRateInfo = lRateInfo;

				if (pNewInfo->bEnableHdmi)
				{
					const BVDC_P_RateInfo *pHdmiRateInfo;
					pHdmiRateInfo = BVDC_P_HdmiRmTableEx_isr(
						pNewInfo->pFmtInfo, pNewInfo->eHdmiPixelRepetition,
						&lRateInfo);
					if (pHdmiRateInfo == NULL)
					{
						BDBG_ERR(("disp[%d] - Digital pixel frequency for %s not yet supportted.",
							hDisplay->eId, pNewInfo->pFmtInfo->pchFormatStr));
						return BERR_TRACE(BVDC_ERR_INVALID_HDMI_MODE);
					}
					pNewInfo->pHdmiRateInfo = pHdmiRateInfo;
				}
			}

			/* Will always need IT table entry */
			{
				const uint32_t *pTable;
				if(VIDEO_FORMAT_SUPPORTS_DCS(pNewInfo->pFmtInfo->eVideoFmt) ||
				   VIDEO_FORMAT_SUPPORTS_MACROVISION(pNewInfo->pFmtInfo->eVideoFmt))
				{
					pTable = BVDC_P_GetItTableMv_isr(pNewInfo);
				}
				else
				{
					pTable = BVDC_P_GetItTable_isr(pNewInfo);
				}

				if (pTable == NULL)
				{
					return BERR_TRACE (BERR_INVALID_PARAMETER);
				}
			}

			/* count how many displays enable a DAC? */
			for (uiIndex=0; uiIndex < BVDC_P_MAX_DACS; uiIndex++)
			{
				if(pNewInfo->aDacOutput[uiIndex] != BVDC_DacOutput_eUnused)
					ulNumDacOut[uiIndex]++;
			}

			/* 656output only supports NTSC and NTSC_J */
			if (pNewInfo->bEnable656)
			{
				if (!VIDEO_FORMAT_IS_525_LINES(pNewInfo->pFmtInfo->eVideoFmt)
					&&
					!VIDEO_FORMAT_IS_625_LINES(pNewInfo->pFmtInfo->eVideoFmt))
				{
					return BERR_TRACE(BVDC_ERR_INVALID_656OUT_USE);
				}
				e656path = hDisplay->eVecPath;
				uiNum656Out++;
			}

			/* Hdmi output path */
			if (pNewInfo->bEnableHdmi)
			{
				eHdmipath = hDisplay->eVecPath;
				uiNumHdmiOut++;
			}

			/* Rfm output path */
			if(pNewInfo->eRfmOutput != BVDC_RfmOutput_eUnused)
			{
				eRfmpath = hDisplay->eVecPath;
				ulRfmConstant = pNewInfo->ulRfmConst;
				uiNumRfmOut++;
			}
		}
	}

	/* Things to watch for:
	 *   - e656Vecpath
	 *   - eHdmiVecpath
	 *   - eRfmVecpath
	 *   - aDacOutput
	 *   - abEnableDac
	 *
	 * Are shared resources between the display contexts.  After
	 * validation they all should be the same.  Only one context (or its isr)
	 * should be building the RUL for settting these. */

	/* Only one Rfm Output active at a time. */
	if(uiNumRfmOut > 1)
	{
		return BERR_TRACE(BVDC_ERR_RFMOUT_MORE_THAN_ONE);
	}

	/* Only one 656/Hdmi active at a time.
	 * Need to save this since Misc bits are shared! */
	if(uiNum656Out > 1)
	{
		return BERR_TRACE(BVDC_ERR_656OUT_MORE_THAN_ONE);
	}

	if(uiNumHdmiOut > 1)
	{
		return BERR_TRACE(BVDC_ERR_HDMIOUT_MORE_THAN_ONE);
	}

	/* Are there any display conflicts with DAC settings? */
	for (uiIndex=0; uiIndex < BVDC_P_MAX_DACS; uiIndex++)
	{
		/* If multiple displays want to use a Dac, return error */
		if(ulNumDacOut[uiIndex] > 1)
			return BERR_TRACE(BVDC_ERR_INVALID_DAC_SETTINGS);
	}

	if (pNewBpsInfo)
	{
		pNewBpsInfo->e656Vecpath  = e656path;
		pNewBpsInfo->eHdmiVecpath = eHdmipath;
		pNewBpsInfo->eRfmVecpath  = eRfmpath;
		pNewBpsInfo->ulRfmConst   = ulRfmConstant;
	}
	if (pNewPriInfo)
	{
		pNewPriInfo->e656Vecpath  = e656path;
		pNewPriInfo->eHdmiVecpath = eHdmipath;
		pNewPriInfo->eRfmVecpath  = eRfmpath;
		pNewPriInfo->ulRfmConst   = ulRfmConstant;
	}
	if (pNewSecInfo)
	{
		pNewSecInfo->e656Vecpath  = e656path;
		pNewSecInfo->eHdmiVecpath = eHdmipath;
		pNewSecInfo->eRfmVecpath  = eRfmpath;
		pNewSecInfo->ulRfmConst   = ulRfmConstant;
	}
	if (pNewTerInfo)
	{
		pNewTerInfo->e656Vecpath  = e656path;
		pNewTerInfo->eHdmiVecpath = eHdmipath;
		pNewTerInfo->eRfmVecpath  = eRfmpath;
		pNewTerInfo->ulRfmConst   = ulRfmConstant;
	}

	/* Set DAC enable flags */
	for (uiIndex=0; uiIndex < BVDC_P_MAX_DACS; uiIndex++)
	{
		/* unused means disable */
		if (pNewPriInfo)
		{
			pNewPriInfo->abEnableDac[uiIndex] = ulNumDacOut[uiIndex] > 0;
		}
		if (pNewSecInfo)
		{
			pNewSecInfo->abEnableDac[uiIndex] = ulNumDacOut[uiIndex] > 0;
		}
		if (pNewTerInfo)
		{
			pNewTerInfo->abEnableDac[uiIndex] = ulNumDacOut[uiIndex] > 0;
		}
		if (pNewBpsInfo)
		{
			pNewBpsInfo->abEnableDac[uiIndex] = ulNumDacOut[uiIndex] > 0;
		}
	}

	BDBG_LEAVE(BVDC_P_Display_ValidateChanges);
	return eStatus;
}

/*************************************************************************
 *  {secret}
 *  BVDC_P_Display_ApplyChanges_isr
 *
 *  New requests have been validated. New user settings will be applied
 *  at next Display isr.
 **************************************************************************/
void BVDC_P_Display_ApplyChanges_isr
	( BVDC_Display_Handle              hDisplay )
{
	uint32_t i;
	uint32_t ulReg;
	BVDC_P_DisplayInfo    *pCurInfo;
	BVDC_P_DisplayInfo    *pNewInfo;

	BDBG_ENTER(BVDC_P_Display_ApplyChanges_isr);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);
	BDBG_OBJECT_ASSERT(hDisplay->hCompositor, BVDC_CMP);

	/* To reduce the amount of typing */
	pNewInfo = &hDisplay->stNewInfo;
	pCurInfo = &hDisplay->stCurInfo;

	/* State transition for display/compositor. */
	if(BVDC_P_STATE_IS_CREATE(hDisplay))
	{
		BDBG_MSG(("Display%d activated.", hDisplay->eId));
		hDisplay->eState = BVDC_P_State_eActive;
		/* Re-enable callback in apply. */
		for(i = 0; i < BVDC_P_CMP_MAX_SLOT_COUNT; i++)
		{
			BINT_ClearCallback_isr(hDisplay->hCompositor->ahCallback[i]);
			BINT_EnableCallback_isr(hDisplay->hCompositor->ahCallback[i]);
		}

		/* Assign Trigger to slot.  Effectively enable slots. */
		BVDC_P_Compositor_AssignTrigger_isr(hDisplay->hCompositor,
			hDisplay->eTopTrigger, hDisplay->eBotTrigger);

		/* set up top level vec muxing */
		if(!hDisplay->bIsBypass)
		/* slect which cmp goes to which display */
		{
			ulReg =
				BCHP_FIELD_ENUM(VIDEO_ENC_PRIM_SRC_SEL, SOURCE, S0_SOURCE+hDisplay->hCompositor->eId);

			BREG_Write32(hDisplay->hVdc->hRegister,
				BCHP_VIDEO_ENC_PRIM_SRC_SEL + hDisplay->eId * sizeof(uint32_t), ulReg);
		}
	}
	else if(BVDC_P_STATE_IS_DESTROY(hDisplay))
	{
		BDBG_MSG(("Display%d de-activated.", hDisplay->eId));

		/* disable triggers to complete shutdown display callbacks. */
		BVDC_P_Display_EnableTriggers_isr(hDisplay, false);

		/* Alignment slave is destroyed */
		if(pCurInfo->hTargetDisplay &&
		   pCurInfo->hTargetDisplay->ulAlignSlaves)
		{
			pCurInfo->hTargetDisplay->ulAlignSlaves--;
		}

		hDisplay->eState = BVDC_P_State_eInactive;
		for(i = 0; i < BVDC_P_CMP_MAX_SLOT_COUNT; i++)
		{
			BRDC_Slot_Disable_isr(hDisplay->hCompositor->ahSlot[i]);
		}

		for(i = 0; i < BVDC_P_CMP_MAX_SLOT_COUNT; i++)
		{
			BINT_DisableCallback_isr(hDisplay->hCompositor->ahCallback[i]);
			BINT_ClearCallback_isr(hDisplay->hCompositor->ahCallback[i]);
		}
		if(!hDisplay->bIsBypass)
		/* slect which cmp goes to which display */
		{
			ulReg = BCHP_FIELD_ENUM(VIDEO_ENC_PRIM_SRC_SEL, SOURCE, DISABLE);

			BREG_Write32(hDisplay->hVdc->hRegister,
				BCHP_VIDEO_ENC_PRIM_SRC_SEL + hDisplay->eId * sizeof(uint32_t), ulReg);
		}
	}

#if BVDC_P_SUPPORT_DOWNSAMPLE
	/* Inform the DS source using this display's compositor of format changes */
	if(pNewInfo->stDirty.stBits.bTiming)
	{
		if (BVDC_P_CMP_GET_DOWNSAMPLE(hDisplay->hCompositor))
		{
			BVDC_P_DownSample_SetVideoFormat(BVDC_P_CMP_GET_DOWNSAMPLE(hDisplay->hCompositor),
				pNewInfo->pFmtInfo);
			BDBG_MSG(("Display %d signals DS of format change", hDisplay->eId));
		}
	}
#endif

	/**
	 * Reset IT state to not active for bypass, in order for apply
	 * changes to restart ISR.
	 */
	if((hDisplay->bIsBypass) &&
	   ((pNewInfo->bEnable656  && !pCurInfo->bEnable656) ||
	    (pNewInfo->bEnableHdmi && !pCurInfo->bEnableHdmi)))
	{
		hDisplay->eItState = BVDC_P_ItState_eNotActive;
		BDBG_MSG(("Display %d resets state to kick start", hDisplay->eId));
	}

	/* Requires reset/program IT block:
	 * 1) Switch format
	 * 2) DVI was off, now it is on (sync up DVI/Vec)
	 *    If the DVI block is started up after the "Master/Slave
	 *    Synchronization" signal is sent from Primary DTG, the two
	 *    paths  will be out of sync and the output will probably stall.
	 * 3) 656 is turned on after Vec is already active
	 *    Setting ITU656_PATH0_BVB_SEL also requires a syncronization
	 *    signal from the Primary DTG.
	 * 4) When run into/out of Macrovision Test01/02 configuration.
	 */
	if (
		(hDisplay->eItState != BVDC_P_ItState_eNotActive) &&
			((pNewInfo->stDirty.stBits.bTiming) ||
			(pNewInfo->eMacrovisionType >= BVDC_MacrovisionType_eTest01) ||
			(pCurInfo->eMacrovisionType >= BVDC_MacrovisionType_eTest01) ||
			(hDisplay->iDcsChange == 2) ||
			(!pCurInfo->bEnableHdmi && pNewInfo->bEnableHdmi) ||
			(!pCurInfo->bEnable656 && pNewInfo->bEnable656) ||
			(pCurInfo->aulHdmiDropLines[pNewInfo->pFmtInfo->eVideoFmt] !=
				pNewInfo->aulHdmiDropLines[pNewInfo->pFmtInfo->eVideoFmt])))
	{
		BDBG_MSG(("[ApplyChanges], %s", pNewInfo->pFmtInfo->pchFormatStr));
		hDisplay->eItState = BVDC_P_ItState_eSwitchMode;
	}

	/* Are the new settings different from the current ones */
	if((BVDC_P_IS_DIRTY(&(pNewInfo->stDirty))) ||
	   (pCurInfo->pfGenericCallback != pNewInfo->pfGenericCallback) ||
	   (pCurInfo->pvGenericParm1 != pNewInfo->pvGenericParm1) ||
	   (pCurInfo->bEnable656 != pNewInfo->bEnable656) ||
	   (pCurInfo->bEnableHdmi != pNewInfo->bEnableHdmi) ||
	   (pCurInfo->eHdmiOutput != pNewInfo->eHdmiOutput) ||
	   (pCurInfo->aulEnableMpaaDeci[BVDC_MpaaDeciIf_eHdmi] !=
	    pNewInfo->aulEnableMpaaDeci[BVDC_MpaaDeciIf_eHdmi]) ||
	   (pCurInfo->aulEnableMpaaDeci[BVDC_MpaaDeciIf_eComponent] !=
	    pNewInfo->aulEnableMpaaDeci[BVDC_MpaaDeciIf_eComponent]) ||
	   (pCurInfo->eRfmOutput != pNewInfo->eRfmOutput) ||
	   (pCurInfo->ulRfmConst != pNewInfo->ulRfmConst) ||
	   (pCurInfo->bFullRate != pNewInfo->bFullRate) ||
	   (pCurInfo->eTimeBase != pNewInfo->eTimeBase) ||
	   (pCurInfo->bCCEnable != pNewInfo->bCCEnable) ||
	   (pCurInfo->bSyncOnly != pNewInfo->bSyncOnly) ||
	   (pCurInfo->bXvYcc != pNewInfo->bXvYcc) ||
	   (hDisplay->bMvChange) ||
	   (hDisplay->iDcsChange != 0) ||
	   (hDisplay->eItState == BVDC_P_ItState_eNotActive) ||
	   (hDisplay->hCompositor->stCurInfo.ulBgColorYCrCb !=
	    hDisplay->hCompositor->stNewInfo.ulBgColorYCrCb) ||
	   (pCurInfo->aulHdmiDropLines[pNewInfo->pFmtInfo->eVideoFmt] !=
	    pNewInfo->aulHdmiDropLines[pNewInfo->pFmtInfo->eVideoFmt]))
	{
		/* alignment */
		if(!pCurInfo->hTargetDisplay)
		{
			if(pNewInfo->hTargetDisplay)
			{
				pNewInfo->hTargetDisplay->ulAlignSlaves++;
				BDBG_MSG(("Display %d will be aligned to display %d with %d slaves.",
					hDisplay->eId, pNewInfo->hTargetDisplay->eId, pNewInfo->hTargetDisplay->ulAlignSlaves));
			}
		}
		else
		{
			if(!pNewInfo->hTargetDisplay) /* disables alignment */
			{
				BDBG_MSG(("Display%d to disable alignment.", hDisplay->eId));
				if(pCurInfo->hTargetDisplay->ulAlignSlaves)
				{
					pCurInfo->hTargetDisplay->ulAlignSlaves--;
				}
			}
		}

		/* re-calculate display pixel aspect ratio */
		if (pNewInfo->stDirty.stBits.bAspRatio)
		{
			uint32_t  ulPxlAspRatio_y_x;

			pCurInfo->eAspectRatio = pNewInfo->eAspectRatio;
			pCurInfo->uiSampleAspectRatioX = pNewInfo->uiSampleAspectRatioX;
			pCurInfo->uiSampleAspectRatioY = pNewInfo->uiSampleAspectRatioY;
			pCurInfo->stAspRatRectClip = pNewInfo->stAspRatRectClip;

			BVDC_P_CalcuPixelAspectRatio_isr(
				pCurInfo->eAspectRatio,
				pCurInfo->uiSampleAspectRatioX,
				pCurInfo->uiSampleAspectRatioY,
				pNewInfo->pFmtInfo->ulWidth,
				pNewInfo->pFmtInfo->ulHeight,
				&pCurInfo->stAspRatRectClip,
				&hDisplay->ulPxlAspRatio,
				&ulPxlAspRatio_y_x);

			/* inform window ApplyChanges  */
			hDisplay->hCompositor->bDspAspRatDirty = true;
		}

		/* Copying the new info to the current info.  Must be careful here
		 * of not globble current dirty bits set by source, but rather ORed
		 * them together. */
		BVDC_P_OR_ALL_DIRTY(&pNewInfo->stDirty, &pCurInfo->stDirty);

		/* Our new now becomes our current(hw-applied) */
		pCurInfo->stDirty = pNewInfo->stDirty;

		/* Clear dirty bits since it's already OR'ed into current.  Notes
		 * the it might not apply until next vysnc, so we're defering
		 * setting the event until next vsync. */
		BVDC_P_CLEAN_ALL_DIRTY(&pNewInfo->stDirty);

		/* Isr will set event to notify apply done. */
		BKNI_ResetEvent(hDisplay->hAppliedDoneEvent);
		hDisplay->bSetEventPending = true;
		hDisplay->bUserAppliedChanges = true;
	}

	/* No RUL has been executed yet, we've no triggers activated.  Must
	 * force the initial top RUL here! */
	if((BVDC_P_ItState_eNotActive == hDisplay->eItState) &&
	   ((!hDisplay->bIsBypass) ||
	    (pNewInfo->bEnable656) ||
	    (pNewInfo->bEnableHdmi)))
	{
		/* Start List */
		BVDC_P_ListInfo stList;

		/* Build Vec Top RUL and force it to execute immediately. */
		BRDC_List_Handle hList = BVDC_P_CMP_GET_LIST(hDisplay->hCompositor,
			BAVC_Polarity_eTopField);
		BRDC_Slot_Handle hSlot = BVDC_P_CMP_GET_SLOT(hDisplay->hCompositor,
			BAVC_Polarity_eTopField);

		/* Remove the first NOP in the Rul for 7038-B0 */
		BRDC_List_SetNumEntries_isr(hList, 0);
		BVDC_P_ReadListInfo_isr(&stList, hList);
		BVDC_P_Vec_BuildRul_isr(hDisplay, &stList, BAVC_Polarity_eTopField);
		BVDC_P_WriteListInfo_isr(&stList, hList);
		BRDC_Slot_SetList_isr(hSlot, hList);
		BRDC_Slot_Execute_isr(hSlot);
	}

	if(hDisplay->bIsBypass && (!pNewInfo->bEnable656) && (!pNewInfo->bEnableHdmi))
	{
		BKNI_SetEvent_isr(hDisplay->hAppliedDoneEvent);
	}

	BDBG_LEAVE(BVDC_P_Display_ApplyChanges_isr);
	return;
}


/***************************************************************************
 * {private}
 *
 */
void BVDC_P_Display_AbortChanges
	( BVDC_Display_Handle              hDisplay )
{
	BDBG_ENTER(BVDC_P_Display_AbortChanges);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);

	/* Cancel the setting user set to the new state. */
	hDisplay->stNewInfo = hDisplay->stCurInfo;

	BDBG_LEAVE(BVDC_P_Display_AbortChanges);
	return;
}

/*************************************************************************
 *  {secret}
 * BVDC_P_Display_FindDac
 *  Return true if found, false otherwise.
 **************************************************************************/
bool BVDC_P_Display_FindDac
	( BVDC_Display_Handle              hDisplay,
	  BVDC_DacOutput                   eDacOutput )
{
	uint32_t               uiIndex;
	BVDC_P_DisplayInfo     *pNewInfo;

	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);
	pNewInfo = &hDisplay->stNewInfo;

	/* Find the Dac output in the array */
	for(uiIndex=0; uiIndex < BVDC_P_MAX_DACS; uiIndex++)
	{
		if (pNewInfo->aDacOutput[uiIndex] == eDacOutput)
		{
			return true;
		}
	}
	return false;
}


/*************************************************************************
 *  {secret}
 * BVDC_P_Display_EnableTriggers_isr
 *  Re-enable trigger after vec reset.
 **************************************************************************/
void BVDC_P_Display_EnableTriggers_isr
	( BVDC_Display_Handle              hDisplay,
	  bool                             bEnable )
{
	uint32_t ulTrigger0;
	uint32_t ulTrigger1;

	BDBG_ENTER(BVDC_P_Display_EnableTriggers_isr);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);
	switch(hDisplay->eMasterTg)
	{
	case BVDC_DisplayTg_ePrimIt:
	case BVDC_DisplayTg_eSecIt:
	case BVDC_DisplayTg_eTertIt:
		/* Re-enable triggers. */
		ulTrigger0 = BREG_Read32_isr(hDisplay->hVdc->hRegister,
			BCHP_PRIM_IT_VEC_TRIGGER_0 + hDisplay->lItOffset);
		ulTrigger0 &= ~BCHP_PRIM_IT_VEC_TRIGGER_0_ENABLE_MASK;
		if(bEnable)
		{
			ulTrigger0 |= BCHP_PRIM_IT_VEC_TRIGGER_0_ENABLE_MASK;
		}
		BREG_Write32(hDisplay->hVdc->hRegister,
			BCHP_PRIM_IT_VEC_TRIGGER_0 + hDisplay->lItOffset, ulTrigger0);

		ulTrigger1 = BREG_Read32_isr(hDisplay->hVdc->hRegister,
			BCHP_PRIM_IT_VEC_TRIGGER_1 + hDisplay->lItOffset);
		ulTrigger1 &= ~BCHP_PRIM_IT_VEC_TRIGGER_1_ENABLE_MASK;
		if(bEnable)
		{
			ulTrigger1 |= (hDisplay->stCurInfo.pFmtInfo->bInterlaced)
				? BCHP_PRIM_IT_VEC_TRIGGER_1_ENABLE_MASK : 0;
		}
		BREG_Write32(hDisplay->hVdc->hRegister,
			BCHP_PRIM_IT_VEC_TRIGGER_1 + hDisplay->lItOffset, ulTrigger1);
		break;

#if BVDC_P_SUPPORT_656_MASTER_MODE
	case BVDC_DisplayTg_e656Dtg: /* 656out in master mode */
		/* Re-enable triggers. */
		ulTrigger0 = BREG_Read32_isr(hDisplay->hVdc->hRegister,
			BCHP_DTG_DTG_TRIGGER_0);
		ulTrigger0 &= ~BCHP_DVI_DTG_DTG_TRIGGER_0_ENABLE_MASK;
		if(bEnable)
		{
			ulTrigger0 |= BCHP_DVI_DTG_DTG_TRIGGER_0_ENABLE_MASK;
		}
		BREG_Write32(hDisplay->hVdc->hRegister,
			BCHP_DTG_DTG_TRIGGER_0, ulTrigger0);

		ulTrigger1 = BREG_Read32_isr(hDisplay->hVdc->hRegister,
			BCHP_DTG_DTG_TRIGGER_1);
		ulTrigger1 &= ~BCHP_DVI_DTG_DTG_TRIGGER_1_ENABLE_MASK;
		if(bEnable)
		{
			ulTrigger1 |= (hDisplay->stCurInfo.pFmtInfo->bInterlaced)
				? BCHP_DVI_DTG_DTG_TRIGGER_1_ENABLE_MASK : 0;
		}
		BREG_Write32(hDisplay->hVdc->hRegister,
			BCHP_DTG_DTG_TRIGGER_1, ulTrigger1);
		break;
#endif

#if BVDC_P_SUPPORT_DVO_MASTER_MODE
	case BVDC_DisplayTg_eDviDtg: /* dvo in master mode */
		/* Re-enable triggers. */
		ulTrigger0 = BREG_Read32_isr(hDisplay->hVdc->hRegister,
			BCHP_DVI_DTG_DTG_TRIGGER_0);
		ulTrigger0 &= ~BCHP_DVI_DTG_DTG_TRIGGER_0_ENABLE_MASK;
		if(bEnable)
		{
			ulTrigger0 |= BCHP_DVI_DTG_DTG_TRIGGER_0_ENABLE_MASK;
		}
		BREG_Write32(hDisplay->hVdc->hRegister,
			BCHP_DVI_DTG_DTG_TRIGGER_0, ulTrigger0);

		ulTrigger1 = BREG_Read32_isr(hDisplay->hVdc->hRegister,
			BCHP_DVI_DTG_DTG_TRIGGER_1);
		ulTrigger1 &= ~BCHP_DVI_DTG_DTG_TRIGGER_1_ENABLE_MASK;
		if(bEnable)
		{
			ulTrigger1 |= (hDisplay->stCurInfo.pFmtInfo->bInterlaced)
				? BCHP_DVI_DTG_DTG_TRIGGER_1_ENABLE_MASK : 0;
		}
		BREG_Write32(hDisplay->hVdc->hRegister,
			BCHP_DVI_DTG_DTG_TRIGGER_1, ulTrigger1);
		break;
#endif
	default: break;
	}
	BDBG_LEAVE(BVDC_P_Display_EnableTriggers_isr);

	return;
}

/*************************************************************************
 *  {secret}
 *  BVDC_P_Display_ResumeRmPhaseInc_isr
 **************************************************************************/
static void BVDC_P_Display_ResumeRmPhaseInc_isr
	( BVDC_Display_Handle              hDisplay,
	  BVDC_P_ListInfo                 *pList )
{
	/* PRIM_RM_PHASE_INC (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_PRIM_RM_PHASE_INC + hDisplay->lRmOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(PRIM_RM_PHASE_INC, PHASE_INC,
			hDisplay->pRmTable[BVDC_P_DISPLAY_RM_PHASE_INC_REG_OFFSET]);
}


/*************************************************************************
 *  {secret}
 *  BVDC_P_Vec_BuildVecPhase_isr
 *  Build the Rul setting the Vec Phase.
 **************************************************************************/
static void BVDC_P_Vec_BuildVecPhase_isr
	( BVDC_Display_Handle              hDisplay,
	  BVDC_P_ListInfo                 *pList,
	  BVDC_PhaseAdjustMode             ePhaseAdjustMode,
	  uint32_t                         ulVdecPhase )
{
	uint32_t ulAdjustedVdecPhase=0;
	uint32_t ulReg=0;
	uint32_t ulVecSchph=0;                     /* VEC_SCHPH */
	uint32_t ulVecPhase=0;
	int32_t  iDeltaPh=0;
	int32_t  iAdjustValue=0;                   /* move up or down x degrees */

	BDBG_ENTER(BVDC_P_Vec_BuildVecPhase_isr);

	if (ePhaseAdjustMode == BVDC_PhaseAdjustMode_eDisable)
	{
		/* don't need to make any vec adjustments. */
		return;
	}

	switch(ePhaseAdjustMode)
	{
		case BVDC_PhaseAdjustMode_eTrack:
			/* use actual vdec phase offset */
			ulAdjustedVdecPhase = ulVdecPhase;
			break;

		case BVDC_PhaseAdjustMode_eClamp:
			if ((ulVdecPhase > BVDC_P_PHASE_270) || (ulVdecPhase < BVDC_P_PHASE_90))
			{
				ulAdjustedVdecPhase = 0;
			}
			else
			{
				ulAdjustedVdecPhase = BVDC_P_PHASE_180;
			}
			break;

		case BVDC_PhaseAdjustMode_eTrackAndClamp:
			if ((ulVdecPhase > BVDC_P_PHASE_315) || (ulVdecPhase < BVDC_P_PHASE_45))
			{
				ulAdjustedVdecPhase = 0;
			}

			if ((ulVdecPhase > BVDC_P_PHASE_135) && (ulVdecPhase < BVDC_P_PHASE_225))
			{
				ulAdjustedVdecPhase = BVDC_P_PHASE_180;
			}
			break;

		default:
			/* Invalid Phase Adjust Mode */
			BDBG_ASSERT(false);
	}

	/* Get VEC subcarrier phase */
	ulReg = BREG_Read32_isr(hDisplay->hVdc->hRegister,
		BCHP_PRIM_SM_PG_CNTRL + hDisplay->lSmOffset);
	ulVecSchph = BVDC_P_GET_FIELD(ulReg, PRIM_SM_PG_CNTRL, INIT_PHASE);

	ulReg = BREG_Read32_isr(hDisplay->hVdc->hRegister,
		BCHP_PRIM_IT_VEC_CTRL_STAT + hDisplay->lItOffset);

	/* Current phase is 0x1fF for odd frames, 0x3ff for even frames */
	if ((BVDC_P_GET_FIELD(ulReg, PRIM_IT_VEC_CTRL_STAT, FIELD_ID)) >> 1)
	{
		ulVecPhase = 0x3ff;
	}
	else
	{
		ulVecPhase = 0x1ff;
	}

	/* Calculate phase, care about 10-bits only */
	iDeltaPh = ((ulAdjustedVdecPhase - ulVecPhase - BVDC_P_PHASE_OFFSET) & 0x3ff) - ulVecSchph;

	/* Move up or down x degrees depending on the quadrant location */
	if(iDeltaPh < -BVDC_P_PHASE_180)
	{
		iAdjustValue = 2;
	}
	else if(iDeltaPh > BVDC_P_PHASE_180)
	{
		iAdjustValue = -2;
	}
	else
	{
		iAdjustValue = (iDeltaPh < 0) ? -2 : 2;
	}

	/* Get the magnitude */
	if (iDeltaPh < 0)
	{
		iDeltaPh = -iDeltaPh;
	}

	/* Start adjusting if we pass the higher threshold,
	   continue till we reach the lower threshold. */
	if (iDeltaPh < BVDC_P_THRESHOLD1)
	{
		iAdjustValue = 0;
		hDisplay->bVecPhaseInAdjust = false;
	}
	else if (!(hDisplay->bVecPhaseInAdjust) && (iDeltaPh < BVDC_P_THRESHOLD2))
	{
		iAdjustValue = 0;
	}
	else if (iDeltaPh > BVDC_P_THRESHOLD2)
	{
		hDisplay->bVecPhaseInAdjust = true;
	}

	/* Program the new VEC subcarrier phase */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_PRIM_SM_PG_CNTRL + hDisplay->lSmOffset);

	if((hDisplay->stCurInfo.eOutputColorSpace == BVDC_P_Output_eYQI) ||
		((hDisplay->stCurInfo.eOutputColorSpace == BVDC_P_Output_eYQI_M) &&
		 (VIDEO_FORMAT_IS_NTSC_J(hDisplay->stCurInfo.pFmtInfo->eVideoFmt))))
	{
		*pList->pulCurrent++ =
			BCHP_FIELD_ENUM(PRIM_SM_PG_CNTRL, PIXEL_FRAC_ENABLE, ON) |
			BCHP_FIELD_DATA(PRIM_SM_PG_CNTRL, ACTIVE_PHASE_OFFSET, 0x005e) |
			BCHP_FIELD_DATA(PRIM_SM_PG_CNTRL, INIT_PHASE, (ulVecSchph + iAdjustValue)&0x3ff);
	}
	else
	{
		*pList->pulCurrent++ =
			BCHP_FIELD_ENUM(PRIM_SM_PG_CNTRL, PIXEL_FRAC_ENABLE, ON) |
			BCHP_FIELD_DATA(PRIM_SM_PG_CNTRL, ACTIVE_PHASE_OFFSET, 0x0) |
			BCHP_FIELD_DATA(PRIM_SM_PG_CNTRL, INIT_PHASE, (ulVecSchph + iAdjustValue)&0x3ff);
	}

	BDBG_LEAVE(BVDC_P_Vec_BuildVecPhase_isr);
	return;
}

#if (BVDC_P_SUPPORT_VEC_DITHER)
/*************************************************************************
 *  {secret}
 *  BVDC_P_Vec_Build_Dither_isr
 *  Builds CSC Dither settings RUL for a display.
 **************************************************************************/
void BVDC_P_Vec_Build_Dither_isr
	( BVDC_Display_Handle              hDisplay,
	  BVDC_P_DitherSetting            *pDither,
	  BVDC_P_ListInfo                 *pList )
{
	BSTD_UNUSED(hDisplay);

	/* PRIM_CSC_DITHER_CONTROL */
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(PRIM_CSC_DITHER_CONTROL, MODE,       pDither->ulMode     ) |
		BCHP_FIELD_DATA(PRIM_CSC_DITHER_CONTROL, OFFSET_CH2, pDither->ulCh2Offset) |
		BCHP_FIELD_DATA(PRIM_CSC_DITHER_CONTROL, SCALE_CH2,  pDither->ulCh2Scale ) |
		BCHP_FIELD_DATA(PRIM_CSC_DITHER_CONTROL, OFFSET_CH1, pDither->ulCh1Offset) |
		BCHP_FIELD_DATA(PRIM_CSC_DITHER_CONTROL, SCALE_CH1,  pDither->ulCh1Scale ) |
		BCHP_FIELD_DATA(PRIM_CSC_DITHER_CONTROL, OFFSET_CH0, pDither->ulCh0Offset) |
		BCHP_FIELD_DATA(PRIM_CSC_DITHER_CONTROL, SCALE_CH0,  pDither->ulCh0Scale );

	/* PRIM_CSC_DITHER_LFSR */
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(PRIM_CSC_DITHER_LFSR, T0, pDither->ulLfsrCtrlT0) |
		BCHP_FIELD_DATA(PRIM_CSC_DITHER_LFSR, T1, pDither->ulLfsrCtrlT1) |
		BCHP_FIELD_DATA(PRIM_CSC_DITHER_LFSR, T2, pDither->ulLfsrCtrlT2);

	/* PRIM_CSC_DITHER_LFSR_INIT */
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(PRIM_CSC_DITHER_LFSR_INIT, SEQ,   pDither->ulLfsrSeq  ) |
		BCHP_FIELD_DATA(PRIM_CSC_DITHER_LFSR_INIT, VALUE, pDither->ulLfsrValue);

}
#endif


/*************************************************************************
 *  {secret}
 *  BVDC_P_Vec_Build_CSC_isr
 *  Builds CSC settings RUL for a display.
 **************************************************************************/
void BVDC_P_Vec_Build_CSC_isr
	( const BVDC_P_DisplayCscMatrix   *pCscMatrix,
	  BVDC_P_ListInfo                 *pList )
{
	BDBG_ASSERT(pCscMatrix);

	*pList->pulCurrent++ =
		BCHP_FIELD_ENUM(PRIM_CSC_CSC_MODE, CLAMP_MODE_C0, MIN_MAX) |
		BCHP_FIELD_ENUM(PRIM_CSC_CSC_MODE, CLAMP_MODE_C1, MIN_MAX) |
		BCHP_FIELD_ENUM(PRIM_CSC_CSC_MODE, CLAMP_MODE_C2, MIN_MAX) |
		BCHP_FIELD_DATA(PRIM_CSC_CSC_MODE, RANGE1, 0x005A) |
		BCHP_FIELD_DATA(PRIM_CSC_CSC_MODE, RANGE2, 0x007F);

	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(PRIM_CSC_CSC_MIN_MAX, MIN, pCscMatrix->ulMin) |
		BCHP_FIELD_DATA(PRIM_CSC_CSC_MIN_MAX, MAX, pCscMatrix->ulMax);

	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(PRIM_CSC_CSC_COEFF_C01_C00, COEFF_C0, pCscMatrix->stCscCoeffs.usY0) |
		BCHP_FIELD_DATA(PRIM_CSC_CSC_COEFF_C01_C00, COEFF_C1, pCscMatrix->stCscCoeffs.usY1);

	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(PRIM_CSC_CSC_COEFF_C03_C02, COEFF_C2, pCscMatrix->stCscCoeffs.usY2) |
		BCHP_FIELD_DATA(PRIM_CSC_CSC_COEFF_C03_C02, COEFF_C3, pCscMatrix->stCscCoeffs.usYOffset);

	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(PRIM_CSC_CSC_COEFF_C11_C10, COEFF_C0, pCscMatrix->stCscCoeffs.usCb0) |
		BCHP_FIELD_DATA(PRIM_CSC_CSC_COEFF_C11_C10, COEFF_C1, pCscMatrix->stCscCoeffs.usCb1);

	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(PRIM_CSC_CSC_COEFF_C13_C12, COEFF_C2, pCscMatrix->stCscCoeffs.usCb2) |
		BCHP_FIELD_DATA(PRIM_CSC_CSC_COEFF_C13_C12, COEFF_C3, pCscMatrix->stCscCoeffs.usCbOffset);

	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(PRIM_CSC_CSC_COEFF_C21_C20, COEFF_C0, pCscMatrix->stCscCoeffs.usCr0) |
		BCHP_FIELD_DATA(PRIM_CSC_CSC_COEFF_C21_C20, COEFF_C1, pCscMatrix->stCscCoeffs.usCr1);

	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(PRIM_CSC_CSC_COEFF_C23_C22, COEFF_C2, pCscMatrix->stCscCoeffs.usCr2) |
		BCHP_FIELD_DATA(PRIM_CSC_CSC_COEFF_C23_C22, COEFF_C3, pCscMatrix->stCscCoeffs.usCrOffset);
}


#if BVDC_P_SUPPORT_DVI_OUT

/*************************************************************************
 *  {secret}
 *	BVDC_P_Vec_Build_DVI_isr
 *  Builds DTRAM, DVI_DTG, DVI_DVF
 **************************************************************************/
static void BVDC_P_Vec_Build_DVI_isr
	( BVDC_Display_Handle              hDisplay,
	  BVDC_P_ListInfo                 *pList,
	  uint32_t                         ulHeight,
	  uint32_t                         ulTrig0Val,
	  uint32_t                         ulTrig1Val )
{
	const uint32_t *pDtRam;
#if (!BVDC_P_VEC_HAS_2_DIFFERENT_DVF)
	uint32_t upsample2x;
#endif
	const BVDC_P_DisplayInfo *pNewInfo = &hDisplay->stNewInfo;
	const BVDC_Display_DvoSettings *pDvoCfg = &pNewInfo->stDvoCfg;
	uint32_t ulIsSlave =
		(hDisplay->eMasterTg == BVDC_DisplayTg_e656Dtg) ? 0 : 1;
	uint32_t ulDviAutoRestart = ulIsSlave;

	/* Dtram[40..7f] is for DVI */
	pDtRam = BVDC_P_GetDtramTable_isr(pNewInfo, pNewInfo->pFmtInfo, hDisplay->bArib480p);
	BDBG_MSG(("Dtram microcode - timestamp: 0x%.8x", pDtRam[BVDC_P_DTRAM_TABLE_TIMESTAMP_IDX]));
	BDBG_MSG(("Dtram microcode - checksum:  0x%.8x", pDtRam[BVDC_P_DTRAM_TABLE_CHECKSUM_IDX]));

	/*-------------------------------*/
	/* NOTE: Reset Must happen first */
	/*-------------------------------*/
	/* VIDEO_ENC_SOFT_RESET_DVI */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VIDEO_ENC_SOFT_RESET_DVI);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VIDEO_ENC_SOFT_RESET_DVI_DVI_DVF, RESET, 1) |
		BCHP_FIELD_DATA(VIDEO_ENC_SOFT_RESET_DVI_DVI_DTG, RESET, 1);

	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VIDEO_ENC_SOFT_RESET_DVI);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VIDEO_ENC_SOFT_RESET_DVI_DVI_DVF, RESET, 0) |
		BCHP_FIELD_DATA(VIDEO_ENC_SOFT_RESET_DVI_DVI_DTG, RESET, 0 );

	/* Load here! */
	*pList->pulCurrent++ = BRDC_OP_IMMS_TO_REGS(BVDC_P_DTRAM_TABLE_SIZE);
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_DTRAM_DMC_INSTRUCTIONi_ARRAY_BASE +
		(BVDC_P_DVI_DTRAM_START_ADDR * sizeof(uint32_t)));
	BKNI_Memcpy((void*)pList->pulCurrent, (void*)pDtRam,
		BVDC_P_DTRAM_TABLE_SIZE * sizeof(uint32_t));
	pList->pulCurrent += BVDC_P_DTRAM_TABLE_SIZE;

	/* DVI_DTG_DTG_BVB (RW) */
	/* DVI_DTG_CCIR_PCL (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_DVI_DTG_CCIR_PCL);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(DVI_DTG_CCIR_PCL, HACTIVE_ENABLE,  1) | /* nominal */
		BCHP_FIELD_DATA(DVI_DTG_CCIR_PCL, HACTIVE_SEL,     0) | /* nominal */
		BCHP_FIELD_DATA(DVI_DTG_CCIR_PCL, VACTIVE_ENABLE,  1) | /* nominal */
		BCHP_FIELD_DATA(DVI_DTG_CCIR_PCL, VACTIVE_SEL,     0) | /* nominal */
		BCHP_FIELD_DATA(DVI_DTG_CCIR_PCL, VBLANK_ENABLE,   1) | /* nominal */
		BCHP_FIELD_DATA(DVI_DTG_CCIR_PCL, VBLANK_SEL,      0) | /* nominal */
		BCHP_FIELD_DATA(DVI_DTG_CCIR_PCL, ODD_EVEN_ENABLE, 1) | /* nominal */
		BCHP_FIELD_DATA(DVI_DTG_CCIR_PCL, ODD_EVEN_SEL,    0);  /* nominal */

	/* DVI_DTG_DVI_PCL (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_DVI_DTG_DVI_PCL);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(DVI_DTG_DVI_PCL, DIGITAL_HSYNC_ENABLE, 1) | /* nominal */
		BCHP_FIELD_DATA(DVI_DTG_DVI_PCL, DIGITAL_HSYNC_SEL,    0) | /* nominal */
		BCHP_FIELD_DATA(DVI_DTG_DVI_PCL, DVI_V_ENABLE,         1) | /* nominal */
		BCHP_FIELD_DATA(DVI_DTG_DVI_PCL, DVI_V_SEL,            1) | /* nominal */
		BCHP_FIELD_DATA(DVI_DTG_DVI_PCL, DVI_DE_ENABLE,        1) | /* nominal */
		BCHP_FIELD_DATA(DVI_DTG_DVI_PCL, DVI_DE_SEL,           3);  /* nominal */

	/* DVI_DTG_CNTL_PCL (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_DVI_DTG_CNTL_PCL);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(DVI_DTG_CNTL_PCL, NEW_LINE_CLR_SEL, 3 );  /* nominal */

	/* DVI_DTG_RAM_ADDR (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_DVI_DTG_RAM_ADDR);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(DVI_DTG_RAM_ADDR, MC_START_ADDR,
			BVDC_P_DVI_DTRAM_START_ADDR                       );  /* nominal */

	/* DVI_DTG_DTG_BVB_SIZE (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_DVI_DTG_DTG_BVB_SIZE);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(DVI_DTG_DTG_BVB_SIZE, HORIZONTAL, pNewInfo->pFmtInfo->ulWidth ) |
		BCHP_FIELD_DATA(DVI_DTG_DTG_BVB_SIZE, VERTICAL, ulHeight                      );

	/* DVI_DTG_DTG_BVB_RSTATUS (RO) */
	/* DVI_DTG_DTG_BVB_CSTATUS (WO) */
	/* DVI_DTG_DTG_CTRL_STAT (RO) */
	/* DVI_DTG_DTG_LCNTR (RO) */
	/* DVI_DVF_DVF_REV_ID (RO) */
	/* DVI_DVF_DVF_CONFIG (RW) */
#if (!BVDC_P_VEC_HAS_2_DIFFERENT_DVF)
	if ((VIDEO_FORMAT_IS_ED(pNewInfo->pFmtInfo->eVideoFmt)) &&
		(BAVC_HDMI_PixelRepetition_e2x == pNewInfo->eHdmiPixelRepetition))
	{
		upsample2x = BCHP_DVI_DVF_DVF_CONFIG_UPSAMPLE2X_ON;
	}
	else
	{
		upsample2x = (VIDEO_FORMAT_IS_SD(pNewInfo->pFmtInfo->eVideoFmt)) ?
			BCHP_DVI_DVF_DVF_CONFIG_UPSAMPLE2X_ON :
			BCHP_DVI_DVF_DVF_CONFIG_UPSAMPLE2X_OFF ;
	}
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_DVI_DVF_DVF_CONFIG);
	*pList->pulCurrent++ =
		BCHP_FIELD_ENUM(DVI_DVF_DVF_CONFIG, VBI_PREFERRED, OFF ) |
		BCHP_FIELD_ENUM(DVI_DVF_DVF_CONFIG, VBI_ENABLE,    OFF ) |
		BCHP_FIELD_DATA(DVI_DVF_DVF_CONFIG, UPSAMPLE2X,  upsample2x  );
#endif

#if (BVDC_P_SUPPORT_DVO_MASTER_MODE)

	/* DVI_DTG_DTG_TRIGGER_0 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_DVI_DTG_DTG_TRIGGER_0);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(DVI_DTG_DTG_TRIGGER_0, ENABLE,        0 ) |
		BCHP_FIELD_DATA(DVI_DTG_DTG_TRIGGER_0, TRIGGER_VALUE, ulTrig0Val );

	/* DVI_DTG_DTG_TRIGGER_1 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_DVI_DTG_DTG_TRIGGER_1);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(DVI_DTG_DTG_TRIGGER_1, ENABLE,        0 ) |
		BCHP_FIELD_DATA(DVI_DTG_DTG_TRIGGER_1, TRIGGER_VALUE, ulTrig1Val );
#else
	BSTD_UNUSED(ulTrig0Val);
	BSTD_UNUSED(ulTrig1Val);
#endif

	/*-------------------------------*/
	/* NOTE: HW requires Conig last  */
	/*-------------------------------*/
	/* DVI_DTG_DTG_CONFIG (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_DVI_DTG_DTG_CONFIG);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(DVI_DTG_DTG_CONFIG, AUTO_RESTART, ulDviAutoRestart     ) |
		BCHP_FIELD_DATA(DVI_DTG_DTG_CONFIG, RESTART_WIN,   31                  ) |
		BCHP_FIELD_DATA(DVI_DTG_DTG_CONFIG, TOGGLE_DVI_DE, pDvoCfg->eDePolarity) |
		BCHP_FIELD_DATA(DVI_DTG_DTG_CONFIG, TOGGLE_DVI_V,  pDvoCfg->eVsPolarity) |
		BCHP_FIELD_DATA(DVI_DTG_DTG_CONFIG, TOGGLE_DVI_H,  pDvoCfg->eHsPolarity) |
		BCHP_FIELD_DATA(DVI_DTG_DTG_CONFIG, TRIGGER_CNT_CLR_COND, 0            ) |
		BCHP_FIELD_DATA(DVI_DTG_DTG_CONFIG, SLAVE_MODE,           ulIsSlave    ) |
		BCHP_FIELD_DATA(DVI_DTG_DTG_CONFIG, MCS_ENABLE, 1                      );

#if (BVDC_P_SUPPORT_DVO_MASTER_MODE)
	/* NOTE: Set ulRdcVarAddr first time through, and allow switching
	 * of Vec State to Active by ulRdcVarAddr handlers.  TG reset transitions. */
	if(BVDC_DisplayTg_eDviDtg == hDisplay->eMasterTg)
	{
		/* Reset Marker! */
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(hDisplay->ulRdcVarAddr);
		*pList->pulCurrent++ = 1;

		/* KLUDGE: This sent some sort of signal to HW, and we must need
		 * it for it to work properly. */
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_DTRAM_DTRAM_CONFIG);
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(DTRAM_DTRAM_CONFIG, ARBITER_LATENCY, 0xb);
	}
#endif

	return;
}


/***************************************************************************
 *
 */
static void BVDC_P_Vec_Build_DVI_CSC_isr
	( BVDC_Display_Handle              hDisplay,
	  BVDC_P_ListInfo                 *pList )
{
	const BVDC_P_DisplayCscMatrix *pCscMatrix = NULL;
	const BVDC_P_DisplayInfo *pNewInfo = &hDisplay->stNewInfo;

	/* Reset DVI_CSC */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VIDEO_ENC_SOFT_RESET_DVI);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VIDEO_ENC_SOFT_RESET_DVI_DVI_CSC, RESET, 1);

	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VIDEO_ENC_SOFT_RESET_DVI);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VIDEO_ENC_SOFT_RESET_DVI_DVI_CSC, RESET, 0 );

	/* --- Setup DVI CSC --- */
	BVDC_P_Display_GetDviCscTable_isr(pNewInfo, &pCscMatrix);

	if(pNewInfo->bUserCsc)
	{
		BVDC_P_Csc_FromMatrixDvo_isr(&hDisplay->stDvoCscMatrix.stCscCoeffs,
			pNewInfo->pl32_Matrix, pNewInfo->ulUserShift,
			((pNewInfo->eHdmiOutput == BAVC_MatrixCoefficients_eHdmi_RGB) ||
			 (pNewInfo->eHdmiOutput == BAVC_MatrixCoefficients_eDvi_Full_Range_RGB))? true : false);

		hDisplay->stDvoCscMatrix.ulMin = pCscMatrix->ulMin;
		hDisplay->stDvoCscMatrix.ulMax = pCscMatrix->ulMax;
	}
	else
	{
		/* For BVDC_Display_GetDvoColorMatrix() */
		hDisplay->stDvoCscMatrix = *pCscMatrix;
	}

	if(!pNewInfo->abOutputMute[BVDC_DisplayOutput_eDvo])
	{
		BVDC_P_Csc_DvoApplyAttenuationRGB_isr(hDisplay->stNewInfo.lDvoAttenuationR,
		                                      hDisplay->stNewInfo.lDvoAttenuationG,
		                                      hDisplay->stNewInfo.lDvoAttenuationB,
		                                      hDisplay->stNewInfo.lDvoOffsetR,
		                                      hDisplay->stNewInfo.lDvoOffsetG,
		                                      hDisplay->stNewInfo.lDvoOffsetB,
		                                     &hDisplay->stDvoCscMatrix.stCscCoeffs);
	}
	else
	{
		uint8_t ucCh0, ucCh1, ucCh2;
		const BVDC_P_Compositor_Info *pCmpInfo = &hDisplay->hCompositor->stCurInfo;

		ucCh0 = BPXL_GET_COMPONENT(BPXL_eA8_Y8_Cb8_Cr8, pCmpInfo->ulBgColorYCrCb, 2);
		ucCh1 = BPXL_GET_COMPONENT(BPXL_eA8_Y8_Cb8_Cr8, pCmpInfo->ulBgColorYCrCb, 1);
		ucCh2 = BPXL_GET_COMPONENT(BPXL_eA8_Y8_Cb8_Cr8, pCmpInfo->ulBgColorYCrCb, 0);

		/* Swap ch0 and 1 of input color to match vec csc layout */
		BVDC_P_Csc_ApplyYCbCrColor(&hDisplay->stDvoCscMatrix.stCscCoeffs, ucCh1, ucCh0, ucCh2);
	}

	/* DVI_CSC_CSC_MODE (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMMS_TO_REGS(BVDC_P_CSC_TABLE_SIZE);
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_DVI_CSC_CSC_MODE);

	/* Setup CSC for Dac outputs */
	BVDC_P_Vec_Build_CSC_isr(&hDisplay->stDvoCscMatrix, pList);

#if (BVDC_P_SUPPORT_VEC_DITHER)
	/* DVI_CSC_DITHER */
	if(hDisplay->stNewInfo.stDvoCfg.b8BitPanel)
	{
		hDisplay->stDviDither.ulCh0Offset = BVDC_P_DITHER_DISP_DVI_OFFSET_8BIT;
		hDisplay->stDviDither.ulCh1Offset = BVDC_P_DITHER_DISP_DVI_OFFSET_8BIT;
		hDisplay->stDviDither.ulCh2Offset = BVDC_P_DITHER_DISP_DVI_OFFSET_8BIT;
		hDisplay->stDviDither.ulCh0Scale  = BVDC_P_DITHER_DISP_DVI_SCALE_8BIT;
		hDisplay->stDviDither.ulCh1Scale  = BVDC_P_DITHER_DISP_DVI_SCALE_8BIT;
		hDisplay->stDviDither.ulCh2Scale  = BVDC_P_DITHER_DISP_DVI_SCALE_8BIT;
	}
	else
	{
		hDisplay->stDviDither.ulCh0Offset = BVDC_P_DITHER_DISP_DVI_OFFSET_10BIT;
		hDisplay->stDviDither.ulCh1Offset = BVDC_P_DITHER_DISP_DVI_OFFSET_10BIT;
		hDisplay->stDviDither.ulCh2Offset = BVDC_P_DITHER_DISP_DVI_OFFSET_10BIT;
		hDisplay->stDviDither.ulCh0Scale  = BVDC_P_DITHER_DISP_DVI_SCALE_10BIT;
		hDisplay->stDviDither.ulCh1Scale  = BVDC_P_DITHER_DISP_DVI_SCALE_10BIT;
		hDisplay->stDviDither.ulCh2Scale  = BVDC_P_DITHER_DISP_DVI_SCALE_10BIT;
	}

	*pList->pulCurrent++ = BRDC_OP_IMMS_TO_REGS(BVDC_P_DITHER_TABLE_SIZE);
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_DVI_CSC_DITHER_CONTROL);
	BVDC_P_Vec_Build_Dither_isr(hDisplay, &hDisplay->stDviDither, pList);
#endif

	return;
}


/*************************************************************************
 *  {secret}
 *
 *  Builds the blank color of DVI or 656 output.  Use the compositor
 *  background color.
 **************************************************************************/
static void BVDC_P_Vec_Build_DVF_Color_isr
	( BVDC_Display_Handle              hDisplay,
	  BVDC_P_ListInfo                 *pList,
	  uint32_t                         ulDvfRegAddr )
{
	uint8_t ucCh0, ucCh1, ucCh2;
	const BVDC_P_Compositor_Info *pCmpInfo = &hDisplay->hCompositor->stCurInfo;

	if((BAVC_MatrixCoefficients_eHdmi_RGB == hDisplay->stCurInfo.eHdmiOutput) ||
	   (BAVC_MatrixCoefficients_eDvi_Full_Range_RGB == hDisplay->stCurInfo.eHdmiOutput))
	{
		ucCh0 = pCmpInfo->ucGreen;
		ucCh1 = pCmpInfo->ucBlue;
		ucCh2 = pCmpInfo->ucRed;
	}
	else
	{
		ucCh0 = BPXL_GET_COMPONENT(BPXL_eA8_Y8_Cb8_Cr8, pCmpInfo->ulBgColorYCrCb, 2);
		ucCh1 = BPXL_GET_COMPONENT(BPXL_eA8_Y8_Cb8_Cr8, pCmpInfo->ulBgColorYCrCb, 1);
		ucCh2 = BPXL_GET_COMPONENT(BPXL_eA8_Y8_Cb8_Cr8, pCmpInfo->ulBgColorYCrCb, 0);
	}

	/* DVI_DVF_DVF_VALUES (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(ulDvfRegAddr);
	*pList->pulCurrent++ =
#if (!BVDC_P_VEC_HAS_2_DIFFERENT_DVF)
		BCHP_FIELD_DATA(DVI_DVF_DVF_VALUES, CH0_VBI_OFFSET, 0x1  ) |
		BCHP_FIELD_DATA(DVI_DVF_DVF_VALUES, CH2_BLANK, ucCh2     ) |
		BCHP_FIELD_DATA(DVI_DVF_DVF_VALUES, CH1_BLANK, ucCh1     ) |
		BCHP_FIELD_DATA(DVI_DVF_DVF_VALUES, CH0_BLANK, ucCh0     );
#else
		BCHP_FIELD_DATA(DVI_DVF_DVI_DVF_VALUES, CH2_BLANK, ucCh2 ) |
		BCHP_FIELD_DATA(DVI_DVF_DVI_DVF_VALUES, CH1_BLANK, ucCh1 ) |
		BCHP_FIELD_DATA(DVI_DVF_DVI_DVF_VALUES, CH0_BLANK, ucCh0 );
#endif

	return;
}
#endif

#if BVDC_P_SUPPORT_ITU656_OUT
/*************************************************************************
 *  {secret}
 *	BVDC_P_Vec_Build_656_isr
 *  Build DTRAM, DTG, DVF, ITU656
 **************************************************************************/
static void BVDC_P_Vec_Build_656_isr
	( BVDC_Display_Handle              hDisplay,
	  BVDC_P_ListInfo                 *pList,
	  uint32_t                         ulHeight,
	  uint32_t                         ulTrig0Val,
	  uint32_t                         ulTrig1Val )
{
	uint32_t ulNumRegs;
	const uint32_t *pTable=NULL;
	BVDC_P_DisplayInfo *pNewInfo = &hDisplay->stNewInfo;
	bool bDisplayRestart = (hDisplay->eItState == BVDC_P_ItState_eSwitchMode) ||
	   (hDisplay->eItState == BVDC_P_ItState_eNotActive);
	bool bIsMaster = (hDisplay->eMasterTg == BVDC_DisplayTg_e656Dtg);
#if !BVDC_P_SUPPORT_656_MASTER_MODE
	BSTD_UNUSED(ulTrig0Val);
	BSTD_UNUSED(ulTrig1Val);
#endif

	/* Since the vec IT reseted we want to reset the DTG as well. */
	if(bDisplayRestart)
	{
		BDBG_MSG(("Reset 656 DTG block."));
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VIDEO_ENC_SOFT_RESET_ITU656);
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(VIDEO_ENC_SOFT_RESET_ITU656_ITU656_DTG, RESET, 1) |
			BCHP_FIELD_DATA(VIDEO_ENC_SOFT_RESET_ITU656_ITU656_CSC, RESET, 1) |
			BCHP_FIELD_DATA(VIDEO_ENC_SOFT_RESET_ITU656_ITU656_DVF, RESET, 1) |
			BCHP_FIELD_DATA(VIDEO_ENC_SOFT_RESET_ITU656_ITU656_FORMATTER, RESET, 1);

		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VIDEO_ENC_SOFT_RESET_ITU656);
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(VIDEO_ENC_SOFT_RESET_ITU656_ITU656_DTG, RESET, 0) |
			BCHP_FIELD_DATA(VIDEO_ENC_SOFT_RESET_ITU656_ITU656_CSC, RESET, 0) |
			BCHP_FIELD_DATA(VIDEO_ENC_SOFT_RESET_ITU656_ITU656_DVF, RESET, 0) |
			BCHP_FIELD_DATA(VIDEO_ENC_SOFT_RESET_ITU656_ITU656_FORMATTER, RESET, 0);
	}

	/* Setup VIDEO_ENC */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VIDEO_ENC_ITU656_SRC_SEL);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VIDEO_ENC_ITU656_SRC_SEL, SOURCE, hDisplay->hCompositor->eId);

	/* --- Setup DTRAM --- */
	/* Dtram[0..3f] is reserved for 656 */
	*pList->pulCurrent++ = BRDC_OP_IMMS_TO_REGS(BVDC_P_DTRAM_TABLE_SIZE);
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_DTRAM_DMC_INSTRUCTIONi_ARRAY_BASE);

	/* get correct 656 dtram */
	pTable = BVDC_P_Get656DtramTable_isr(pNewInfo);

	BKNI_Memcpy((void*)pList->pulCurrent, (void*)pTable,
		BVDC_P_DTRAM_TABLE_SIZE * sizeof(uint32_t));
	pList->pulCurrent += BVDC_P_DTRAM_TABLE_SIZE;

#if BVDC_P_SUPPORT_656_MASTER_MODE /* bypass display */
	/* --- Setup 656 triggers for master mode --- */
	if (bDisplayRestart && bIsMaster)
	{
		*pList->pulCurrent++ = BRDC_OP_IMMS_TO_REGS(2);
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_DTG_DTG_TRIGGER_0);
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(DVI_DTG_DTG_TRIGGER_0, TRIGGER_VALUE, ulTrig0Val) |
			BCHP_FIELD_DATA(DVI_DTG_DTG_TRIGGER_0, ENABLE, 0);
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(DVI_DTG_DTG_TRIGGER_1, TRIGGER_VALUE, ulTrig1Val) |
			BCHP_FIELD_DATA(DVI_DTG_DTG_TRIGGER_1, ENABLE, 0);
	}
#endif

	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_DTG_DTG_CONFIG);
	*pList->pulCurrent++ =
		/* master mode 656out should turn off AUTO_RESTART */
		BCHP_FIELD_DATA(DVI_DTG_DTG_CONFIG, AUTO_RESTART,!bIsMaster) |
		BCHP_FIELD_DATA(DVI_DTG_DTG_CONFIG, RESTART_WIN, 31) |
		BCHP_FIELD_DATA(DVI_DTG_DTG_CONFIG, MCS_ENABLE, 1  ) |
		BCHP_FIELD_DATA(DVI_DTG_DTG_CONFIG, SLAVE_MODE,  !bIsMaster) |
		BCHP_FIELD_DATA(DVI_DTG_DTG_CONFIG, TRIGGER_CNT_CLR_COND, 0) |
		BCHP_FIELD_DATA(DVI_DTG_DTG_CONFIG, TOGGLE_DVI_H, 0) |
		BCHP_FIELD_DATA(DVI_DTG_DTG_CONFIG, TOGGLE_DVI_V, 0) |
		BCHP_FIELD_DATA(DVI_DTG_DTG_CONFIG, TOGGLE_DVI_DE, 0);

	/* PR28786: Need to set DTG_DTG_BVB.BVB_DATA to DISP, and
	 * DTG_DTG_CONFIG.AUTO_RESTART to ON for 656 ouput to work
	 * correctly when switching from master to slave mode */
#if (BVDC_P_SUPPORT_MODULAR_VEC == 1)
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_DVF_DVF_BVB_CONFIG);
	*pList->pulCurrent++ =
		BCHP_FIELD_ENUM(PRIM_VF_BVB_CONFIG, PSYNC,     START_TOP) |
		BCHP_FIELD_ENUM(PRIM_VF_BVB_CONFIG, BVB_DATA , DISP);
#endif

	ulNumRegs = (BCHP_DTG_DTG_BVB_SIZE - BCHP_DTG_CCIR_PCL)/4 + 1;

	*pList->pulCurrent++ = BRDC_OP_IMMS_TO_REGS(ulNumRegs);
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_DTG_CCIR_PCL);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(DVI_DTG_CCIR_PCL, HACTIVE_ENABLE,  1) |
		BCHP_FIELD_DATA(DVI_DTG_CCIR_PCL, HACTIVE_SEL,     0) |
		BCHP_FIELD_DATA(DVI_DTG_CCIR_PCL, VACTIVE_ENABLE,  1) |
		BCHP_FIELD_DATA(DVI_DTG_CCIR_PCL, VACTIVE_SEL,     0) |
		BCHP_FIELD_DATA(DVI_DTG_CCIR_PCL, VBLANK_ENABLE,   1) |
		BCHP_FIELD_DATA(DVI_DTG_CCIR_PCL, VBLANK_SEL,      0) |
		BCHP_FIELD_DATA(DVI_DTG_CCIR_PCL, ODD_EVEN_ENABLE, 1) |
		BCHP_FIELD_DATA(DVI_DTG_CCIR_PCL, ODD_EVEN_SEL,    0);

	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(DVI_DTG_DVI_PCL, DIGITAL_HSYNC_ENABLE, 1) |
		BCHP_FIELD_DATA(DVI_DTG_DVI_PCL, DIGITAL_HSYNC_SEL,    0) |
		BCHP_FIELD_DATA(DVI_DTG_DVI_PCL, DVI_V_ENABLE,     0) |
		BCHP_FIELD_DATA(DVI_DTG_DVI_PCL, DVI_V_SEL,        0) |
		BCHP_FIELD_DATA(DVI_DTG_DVI_PCL, DVI_DE_ENABLE,    0) |
		BCHP_FIELD_DATA(DVI_DTG_DVI_PCL, DVI_DE_SEL,       0);

	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(DVI_DTG_CNTL_PCL, NEW_LINE_CLR_SEL, 3);

	/* DTG_RAM_ADDR = 0 for 656out */
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(DVI_DTG_RAM_ADDR, MC_START_ADDR, 0);

	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(DVI_DTG_DTG_BVB_SIZE, HORIZONTAL,pNewInfo->pFmtInfo->ulWidth) |
		BCHP_FIELD_DATA(DVI_DTG_DTG_BVB_SIZE, VERTICAL, ulHeight);

	/* --- Setup DVF block --- */
	ulNumRegs = (BCHP_DVF_DVF_BVB_STATUS - BCHP_DVF_DVF_CONFIG)/4 + 1;

	*pList->pulCurrent++ = BRDC_OP_IMMS_TO_REGS(ulNumRegs);
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_DVF_DVF_CONFIG);

#if (!BVDC_P_VEC_HAS_2_DIFFERENT_DVF)
	*pList->pulCurrent++ =
		BCHP_FIELD_ENUM(DVI_DVF_DVF_CONFIG, VBI_PREFERRED, OFF)|
		BCHP_FIELD_ENUM(DVI_DVF_DVF_CONFIG, VBI_ENABLE,    OFF)|
		BCHP_FIELD_ENUM(DVI_DVF_DVF_CONFIG, UPSAMPLE2X,    ON) |
		BCHP_FIELD_DATA(DVI_DVF_DVF_CONFIG, reserved0,     0) |
		BCHP_FIELD_DATA(DVI_DVF_DVF_CONFIG, reserved_for_eco1, 0);

	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(DVI_DVF_DVF_VALUES, CH0_VBI_OFFSET, 1) |
		BCHP_FIELD_DATA(DVI_DVF_DVF_VALUES, CH2_BLANK,    128) |
		BCHP_FIELD_DATA(DVI_DVF_DVF_VALUES, CH1_BLANK,    128) |
		BCHP_FIELD_DATA(DVI_DVF_DVF_VALUES, CH0_BLANK,    16);
#else
	*pList->pulCurrent++ =
		BCHP_FIELD_ENUM(DVI_DVF_DVI_DVF_CONFIG, UPSAMPLE2X,    ON) |
		BCHP_FIELD_DATA(DVI_DVF_DVI_DVF_CONFIG, reserved0,	   0) |
		BCHP_FIELD_DATA(DVI_DVF_DVI_DVF_CONFIG, reserved_for_eco1, 0);

	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(DVI_DVF_DVI_DVF_VALUES, CH2_BLANK,	  128) |
		BCHP_FIELD_DATA(DVI_DVF_DVI_DVF_VALUES, CH1_BLANK,	  128) |
		BCHP_FIELD_DATA(DVI_DVF_DVI_DVF_VALUES, CH0_BLANK,	  16);
#endif

	/* bvb_status */
	*pList->pulCurrent++ = 0xffffffff;

	/*--- Setup ITU_656 ---*/
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_ITU656_ITU656_CONFIG);
	if (hDisplay->bIsBypass)
	{
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(ITU656_ITU656_CONFIG, FILTER_MODE, 0)|
			BCHP_FIELD_ENUM(ITU656_ITU656_CONFIG, INPUT_MODE, BYPASS)|
			BCHP_FIELD_ENUM(ITU656_ITU656_CONFIG, DATA_OUT_PATTERN, ZERO)|
			BCHP_FIELD_DATA(ITU656_ITU656_CONFIG, reserved0, 0);
	}
	else
	{
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(ITU656_ITU656_CONFIG, FILTER_MODE, 0)|
			BCHP_FIELD_ENUM(ITU656_ITU656_CONFIG, INPUT_MODE, SHARED)|
			BCHP_FIELD_ENUM(ITU656_ITU656_CONFIG, DATA_OUT_PATTERN, ZERO)|
			BCHP_FIELD_DATA(ITU656_ITU656_CONFIG, reserved0, 0);
	}

	if (bDisplayRestart && bIsMaster)
	{
		/* Set ulRdcVarAddr first time through, and allow switching
		 * of Vec State to Active by ulRdcVarAddr handlers */
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(hDisplay->ulRdcVarAddr);
		*pList->pulCurrent++ = 1;

		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_DTRAM_DTRAM_CONFIG);
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(DTRAM_DTRAM_CONFIG, ARBITER_LATENCY, 0xb);
	}

	return;
}

/*************************************************************************
 *  {secret}
 *	BVDC_P_Vec_Build_656_CSC_isr
 *  Build CSC
 **************************************************************************/
static void BVDC_P_Vec_Build_656_CSC_isr
	( BVDC_Display_Handle              hDisplay,
	  BVDC_P_ListInfo                 *pList )
{
	BVDC_P_DisplayInfo  *pNewInfo = &hDisplay->stNewInfo;
	const BVDC_P_DisplayCscMatrix *pCscMatrix = NULL;
	BVDC_P_DisplayCscMatrix stCscMatrix;

	/* --- Setup CSC ---*/
	*pList->pulCurrent++ = BRDC_OP_IMMS_TO_REGS(BVDC_P_CSC_TABLE_SIZE);
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_CSC_CSC_MODE);

	BVDC_P_Display_Get656CscTable_isr(&hDisplay->stNewInfo,
		hDisplay->hCompositor->bIsBypass, &pCscMatrix);

	/* Setup CSC for Dac outputs */
	BVDC_P_Vec_Build_CSC_isr(pCscMatrix, pList);

	stCscMatrix = *pCscMatrix;

	/* Handle CSC mute */
	if (pNewInfo->abOutputMute[BVDC_DisplayOutput_e656])
	{
		uint8_t ucCh0, ucCh1, ucCh2;
		const BVDC_P_Compositor_Info *pCmpInfo = &hDisplay->hCompositor->stCurInfo;

		/* Blank color, use in CSC */
		ucCh0 = BPXL_GET_COMPONENT(BPXL_eA8_Y8_Cb8_Cr8, pCmpInfo->ulBgColorYCrCb, 2);
		ucCh1 = BPXL_GET_COMPONENT(BPXL_eA8_Y8_Cb8_Cr8, pCmpInfo->ulBgColorYCrCb, 1);
		ucCh2 = BPXL_GET_COMPONENT(BPXL_eA8_Y8_Cb8_Cr8, pCmpInfo->ulBgColorYCrCb, 0);

		/* Swap ch0 and 1 of input color to match vec csc layout */
		BVDC_P_Csc_ApplyYCbCrColor(&stCscMatrix.stCscCoeffs, ucCh1, ucCh0, ucCh2);
	}

	/* Setup CSC for Dac outputs */
	BVDC_P_Vec_Build_CSC_isr(&stCscMatrix, pList);

#if (BVDC_P_SUPPORT_VEC_DITHER)
	*pList->pulCurrent++ = BRDC_OP_IMMS_TO_REGS(BVDC_P_DITHER_TABLE_SIZE);
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_CSC_DITHER_CONTROL);
	BVDC_P_Vec_Build_Dither_isr(hDisplay, &hDisplay->st656Dither, pList);
#endif

	return;
}
#endif


#if (BVDC_P_SUPPORT_DVPO)
/*************************************************************************
 *  {secret}
 *	BVDC_P_Vec_BuildPllCfg_isr
 **************************************************************************/
static void BVDC_P_Vec_BuildPllCfg_isr
	( BVDC_P_ListInfo                 *pList,
	  BREG_Handle                      hReg,
	  const BVDC_Display_DvoSettings  *pDvoCfg,
	  const BVDC_P_RateInfo           *pRmInfo,
	  uint32_t                         ulRDiv,
	  uint32_t                         ulOffset )
{
	uint32_t ulPreDiv = 2;
	uint32_t ulReg, ulLvdsMode;

	/* Spread spectrum adjustment:
	 * When enable spread spectrum the OFFSET must be compenstate.  By
	 * 0.08% off the current offset.  Need to also make sure
	 * XPT_DPCR0_LOOP_CTRL.TIME_REF is correct setup for the correct source
	 * to have input/output locking. */
	if(pDvoCfg->stSpreadSpectrum.bEnable)
	{
		ulPreDiv  = 1;
		ulOffset  = (ulOffset / 2);
#if (BVDC_P_SUPPORT_HDMI_RM_VER < BVDC_P_HDMI_RM_VER_1)  /* pre-B2 */
		ulOffset += (ulOffset * 8)/10000;
#endif
	}

	/* Check LVDS mode
	 * Dual link:
	 *      DVPO_0_MISC_CONTROL_2.LVDS_MODE = 1
	 *      LVDS_PHY_0_LVDS_PLL_CFG. LINKDIV_CTRL = 0
	 * else
	 *      DVPO_0_MISC_CONTROL_2.LVDS_MODE = 0
	 *      LVDS_PHY_0_LVDS_PLL_CFG. LINKDIV_CTRL = 1
	 */
	ulReg  = BREG_Read32_isr(hReg, BCHP_DVPO_0_MISC_CONTROL_2);
	ulLvdsMode = BVDC_P_GET_FIELD(ulReg, DVPO_0_MISC_CONTROL_2, LVDS_MODE);

	/* LVDS_PHY_0_LVDS_PLL_CFG (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_LVDS_PHY_0_LVDS_PLL_CFG);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(LVDS_PHY_0_LVDS_PLL_CFG, LINKDIV_CTRL,            (ulLvdsMode != 1)      ) |
		BCHP_FIELD_DATA(LVDS_PHY_0_LVDS_PLL_CFG, PLL_R_DIV,                ulRDiv                ) |
		BCHP_FIELD_DATA(LVDS_PHY_0_LVDS_PLL_CFG, PLL_VCO_RANGE,            pRmInfo->ulVcoRange   ) |
		BCHP_FIELD_DATA(LVDS_PHY_0_LVDS_PLL_CFG, PLL_M_DIV,                pRmInfo->ulMDiv       ) |
		BCHP_FIELD_DATA(LVDS_PHY_0_LVDS_PLL_CFG, PLL_FEEDBACK_PRE_DIVIDER, pRmInfo->ulP2         ) |
		BCHP_FIELD_DATA(LVDS_PHY_0_LVDS_PLL_CFG, PLL_INPUT_PRE_DIVIDER,    ulPreDiv              );

	/* DVPO_RM_0_OFFSET (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_DVPO_RM_0_OFFSET);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(DVPO_RM_0_OFFSET, OFFSET_ONLY, 0   ) |
		BCHP_FIELD_DATA(DVPO_RM_0_OFFSET, OFFSET, ulOffset);

	return;
}
#endif


/*************************************************************************
 *  {secret}
 *	BVDC_P_Vec_Build_RM_isr
 **************************************************************************/
static void BVDC_P_Vec_Build_RM_isr
	( BVDC_Display_Handle              hDisplay,
	  const uint32_t                  *pTable,
	  BVDC_P_ListInfo                 *pList )
{
	BDBG_ENTER(BVDC_P_Vec_Build_RM_isr);

	/* if switching mode, we must reset IT block before anything else to avoid deadlock */
	if((BVDC_P_ItState_eSwitchMode == hDisplay->eItState) ||
	   (BVDC_P_ItState_eNotActive  == hDisplay->eItState))
	{
		unsigned ulResetOffset = hDisplay->eId * sizeof(uint32_t);

		/* Reset IT */
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VIDEO_ENC_SOFT_RESET_PRIM + ulResetOffset);
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(VIDEO_ENC_SOFT_RESET_PRIM_PRIM_IT, RESET, 1) |
			BCHP_FIELD_DATA(VIDEO_ENC_SOFT_RESET_PRIM_PRIM_SM, RESET, 1) |
			BCHP_FIELD_DATA(VIDEO_ENC_SOFT_RESET_PRIM_PRIM_VF, RESET, 1) |
			BCHP_FIELD_DATA(VIDEO_ENC_SOFT_RESET_PRIM_PRIM_CSC, RESET, 1);

		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VIDEO_ENC_SOFT_RESET_PRIM + ulResetOffset);
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(VIDEO_ENC_SOFT_RESET_PRIM_PRIM_IT, RESET, 0) |
			BCHP_FIELD_DATA(VIDEO_ENC_SOFT_RESET_PRIM_PRIM_SM, RESET, 0) |
			BCHP_FIELD_DATA(VIDEO_ENC_SOFT_RESET_PRIM_PRIM_VF, RESET, 0) |
			BCHP_FIELD_DATA(VIDEO_ENC_SOFT_RESET_PRIM_PRIM_CSC, RESET, 0);

#if BVDC_P_SUPPORT_COMPONENT_ONLY
		if(BVDC_P_VEC_PATH_SUPPORT_CO(hDisplay->eVecPath))
		{
			*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
			*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VIDEO_ENC_SOFT_RESET_SEC);
			*pList->pulCurrent++ =
				BCHP_FIELD_DATA(VIDEO_ENC_SOFT_RESET_SEC_SEC_CO_VF, RESET, 1) |
				BCHP_FIELD_DATA(VIDEO_ENC_SOFT_RESET_SEC_SEC_CO_CSC, RESET, 1)|
				BCHP_FIELD_DATA(VIDEO_ENC_SOFT_RESET_SEC_SEC_CO_SRC, RESET, 1);

			*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
			*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VIDEO_ENC_SOFT_RESET_SEC);
			*pList->pulCurrent++ =
				BCHP_FIELD_DATA(VIDEO_ENC_SOFT_RESET_SEC_SEC_CO_VF, RESET, 0) |
				BCHP_FIELD_DATA(VIDEO_ENC_SOFT_RESET_SEC_SEC_CO_CSC, RESET, 0)|
				BCHP_FIELD_DATA(VIDEO_ENC_SOFT_RESET_SEC_SEC_CO_SRC, RESET, 0);
		}
#endif
	}

	/* --- Setup RM --- */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_PRIM_RM_CONTROL + hDisplay->lRmOffset);

	/* timebases by default:
	 * - primary Vec  (SD display) -> timebase 1;
	 * - secondary Vec(HD display) -> timebase 0; */
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(PRIM_RM_CONTROL, RESET, 0) |
		BCHP_FIELD_DATA(PRIM_RM_CONTROL, INT_GAIN, 4) |
		BCHP_FIELD_DATA(PRIM_RM_CONTROL, DIRECT_GAIN, 1) |
		BCHP_FIELD_DATA(PRIM_RM_CONTROL, DITHER, 0) |
		BCHP_FIELD_DATA(PRIM_RM_CONTROL, FREE_RUN, 0) |
		BCHP_FIELD_DATA(PRIM_RM_CONTROL, TIMEBASE,
			hDisplay->stNewInfo.eTimeBase);

	*pList->pulCurrent++ = BRDC_OP_IMMS_TO_REGS(BVDC_P_RM_TABLE_SIZE);
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_PRIM_RM_RATE_RATIO + hDisplay->lRmOffset);

	/* Setup RM_RATE_RATIO to RM_INTEGRATOR */
	BKNI_Memcpy((void*)pList->pulCurrent, (void*)pTable,
		BVDC_P_RM_TABLE_SIZE * sizeof(uint32_t));
	pList->pulCurrent += BVDC_P_RM_TABLE_SIZE;

	BDBG_LEAVE(BVDC_P_Vec_Build_RM_isr);
	return;
}

#if BVDC_P_SUPPORT_DVI_OUT
/*************************************************************************
 *  {secret}
 *	BVDC_P_Vec_Build_HdmiRM_isr
 **************************************************************************/
static void BVDC_P_Vec_Build_HdmiRM_isr
	( BVDC_Display_Handle              hDisplay,
	  BVDC_P_ListInfo                 *pList )
{
#if ((!BVDC_P_SUPPORT_DVI_65NM) && (BVDC_P_SUPPORT_HDMI_OUT)) || (BVDC_P_SUPPORT_DVPO)
	uint32_t ulVcxoCtrl;
	bool bDisplayRestart =
		(hDisplay->eItState == BVDC_P_ItState_eNotActive) ||
		(hDisplay->eItState == BVDC_P_ItState_eSwitchMode);
#endif

#if ((BVDC_P_SUPPORT_DVI_65NM) && (BVDC_P_SUPPORT_HDMI_OUT)) || (BVDC_P_SUPPORT_DVPO)
	uint32_t ulSSReg;
	const BVDC_Display_DvoSettings *pDvoCfg;
#endif

	const BVDC_P_RateInfo *pRmInfo;
	const BVDC_P_DisplayInfo *pNewInfo = &hDisplay->stNewInfo;
	uint32_t ulScale, ulRange, ulHold, ulStep, ulFCW;
#if BVDC_P_SUPPORT_DVI_65NM
	uint32_t ulNdivMod, ulIcpx, ulCn;
#endif

	BDBG_ENTER(BVDC_P_Vec_Build_HdmiRM_isr);

	hDisplay->pDvoRmInfo = pRmInfo = BVDC_P_HdmiRmTableEx_isr(
		pNewInfo->pFmtInfo, pNewInfo->eHdmiPixelRepetition,
		&pNewInfo->stRateInfo);

	/* Spread sprectrum settings */
	ulHold  = 3; /* fixed value */
	ulScale = 7; /* fixed value */

	/* ulRange = 0.001 * FCW * (2**(-10-ulScale)) * ulDelta
	 * FCW = DVPO_RM_0_OFFSET
	 */
#if BVDC_P_SUPPORT_DVI_65NM
	ulFCW = hDisplay->pDvoRmInfo->ulOffset;
#else
	ulFCW = hDisplay->pDvoRmInfo->ulNDiv;
#endif

	/* ulRange computation is arranged in the following way so that
	 * there won't be overflow and we can still get accurate result
	 * while no floating point used.
	 *
	 */
	ulRange = ((ulFCW / 1310720) * pNewInfo->stDvoCfg.stSpreadSpectrum.ulDelta) / 100;

	/* ulStep = 1 / ( (1/ulFrequency) * 108000000 / ulRange / 512 / 4)
	 */
	ulStep = pNewInfo->stDvoCfg.stSpreadSpectrum.ulFrequency * ulRange / 52734;

#if BVDC_P_SUPPORT_HDMI_OUT
	/* HDMI_RM_CONTROL (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HDMI_RM_CONTROL);
	*pList->pulCurrent++ =
		BCHP_FIELD_ENUM(HDMI_RM_CONTROL, RESET,       RESET_OFF  ) |
		BCHP_FIELD_DATA(HDMI_RM_CONTROL, INT_GAIN,    4          ) |
		BCHP_FIELD_DATA(HDMI_RM_CONTROL, DIRECT_GAIN, 4          ) |
		BCHP_FIELD_ENUM(HDMI_RM_CONTROL, DITHER,      DITHER_OFF ) |
		BCHP_FIELD_ENUM(HDMI_RM_CONTROL, FREE_RUN,    TIMEBASE   ) |
		BCHP_FIELD_DATA(HDMI_RM_CONTROL, TIMEBASE,    pNewInfo->eTimeBase);

	/* HDMI_RM_RATE_RATIO (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HDMI_RM_RATE_RATIO);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(HDMI_RM_RATE_RATIO, MAX_ERR,     0 ) |
		BCHP_FIELD_DATA(HDMI_RM_RATE_RATIO, MIN_ERR,     0 ) |
		BCHP_FIELD_DATA(HDMI_RM_RATE_RATIO, DENOMINATOR, pRmInfo->ulDenominator);

	/* HDMI_RM_SAMPLE_INC (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HDMI_RM_SAMPLE_INC);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(HDMI_RM_SAMPLE_INC, NUMERATOR,  pRmInfo->ulNumerator) |
		BCHP_FIELD_DATA(HDMI_RM_SAMPLE_INC, SAMPLE_INC, pRmInfo->ulSampleInc);

	/* HDMI_RM_PHASE_INC (RW) */
	/* Don't care!
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HDMI_RM_PHASE_INC);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(HDMI_RM_PHASE_INC, PHASE_INC, 0 ); */

	/* HDMI_RM_INTEGRATOR (RW) */
	/* Don't care!
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HDMI_RM_INTEGRATOR);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(HDMI_RM_INTEGRATOR, INTEGRATOR, 4291432974); */

#if BVDC_P_SUPPORT_DVI_65NM
	pDvoCfg = &pNewInfo->stDvoCfg;

	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HDMI_RM_OFFSET);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(HDMI_RM_OFFSET, OFFSET, pRmInfo->ulOffset);

	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HDMI_RM_FORMAT);
	*pList->pulCurrent++ = (
		BCHP_FIELD_DATA(HDMI_RM_FORMAT, SHIFT,        pRmInfo->ulShift) |
		BCHP_FIELD_DATA(HDMI_RM_FORMAT, STABLE_COUNT,            10000) );

	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HDMI_TX_PHY_HDMI_TX_PHY_PLL_CFG);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(HDMI_TX_PHY_HDMI_TX_PHY_PLL_CFG, PLL_RM_DIV, pRmInfo->ulRmDiv) |
		BCHP_FIELD_DATA(HDMI_TX_PHY_HDMI_TX_PHY_PLL_CFG, PLL_VCO_RANGE, pRmInfo->ulVcoRange) |
		BCHP_FIELD_DATA(HDMI_TX_PHY_HDMI_TX_PHY_PLL_CFG, PLL_PX_DIV, pRmInfo->ulPxDiv) |
		BCHP_FIELD_DATA(HDMI_TX_PHY_HDMI_TX_PHY_PLL_CFG, PLL_FEEDBACK_PRE_DIVIDER, pRmInfo->ulFeedbackPreDiv) |
		BCHP_FIELD_DATA(HDMI_TX_PHY_HDMI_TX_PHY_PLL_CFG, PLL_INPUT_PRE_DIVIDER, pRmInfo->ulInputPreDiv);

	/* LVDS_PHY_0_SPREAD_SPECTRUM (RW) */
	ulSSReg =
		BCHP_FIELD_DATA(HDMI_TX_PHY_SPREAD_SPECTRUM, LOAD,   0      ) |
		BCHP_FIELD_DATA(HDMI_TX_PHY_SPREAD_SPECTRUM, ENABLE, 0      ) |
		BCHP_FIELD_DATA(HDMI_TX_PHY_SPREAD_SPECTRUM, SCALE,  ulScale) |
		BCHP_FIELD_DATA(HDMI_TX_PHY_SPREAD_SPECTRUM, RANGE,  ulRange) |
		BCHP_FIELD_DATA(HDMI_TX_PHY_SPREAD_SPECTRUM, HOLD,   ulHold ) |
		BCHP_FIELD_DATA(HDMI_TX_PHY_SPREAD_SPECTRUM, STEP,   ulStep );

	/****************************************************************/
	/* Require specific protocol to follow in order to turn on/off. */
	/****************************************************************/
	/* (1) Disable spread spectrum and put in new changes. */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HDMI_TX_PHY_SPREAD_SPECTRUM);
	*pList->pulCurrent++ = ulSSReg;

	/* (2) Clock in the new changes, LOAD: 0 -> 1 */
	ulSSReg |= (
		BCHP_FIELD_DATA(HDMI_TX_PHY_SPREAD_SPECTRUM, LOAD, 1));
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HDMI_TX_PHY_SPREAD_SPECTRUM);
	*pList->pulCurrent++ = ulSSReg;

	/* (3) Clock in the new changes, LOAD: 1 -> 0 */
	ulSSReg &= ~(
		BCHP_MASK(HDMI_TX_PHY_SPREAD_SPECTRUM, LOAD));
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HDMI_TX_PHY_SPREAD_SPECTRUM);
	*pList->pulCurrent++ = ulSSReg;

	/* (4) reenable the Spread Spectrum after making the changes */
	ulSSReg |= (
		BCHP_FIELD_DATA(HDMI_TX_PHY_SPREAD_SPECTRUM, ENABLE, pDvoCfg->stSpreadSpectrum.bEnable));
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HDMI_TX_PHY_SPREAD_SPECTRUM);
	*pList->pulCurrent++ = ulSSReg;

	if ((VIDEO_FORMAT_IS_ED(pNewInfo->pFmtInfo->eVideoFmt)) &&
	    (BAVC_HDMI_PixelRepetition_e2x == pNewInfo->eHdmiPixelRepetition))
	{
		ulNdivMod = 2;
		ulIcpx = 10;
		ulCn = 0;
	}
	else
	{
		ulNdivMod = 4;
		ulIcpx = 3;
		ulCn = 1;
	}

	/* HDMI_TX_PHY_HDMI_TX_PHY_CTL_1 (RW) */
	BVDC_P_RD_MOD_WR_RUL(pList->pulCurrent,
						 ~(BCHP_MASK(HDMI_TX_PHY_HDMI_TX_PHY_CTL_1, ICPX) |
						   BCHP_MASK(HDMI_TX_PHY_HDMI_TX_PHY_CTL_1, LF_ORDER) |
						   BCHP_MASK(HDMI_TX_PHY_HDMI_TX_PHY_CTL_1, RZ) |
						   BCHP_MASK(HDMI_TX_PHY_HDMI_TX_PHY_CTL_1, CZ) |
						   BCHP_MASK(HDMI_TX_PHY_HDMI_TX_PHY_CTL_1, CP) |
						   BCHP_MASK(HDMI_TX_PHY_HDMI_TX_PHY_CTL_1, RN) |
						   BCHP_MASK(HDMI_TX_PHY_HDMI_TX_PHY_CTL_1, CN)),
						 BCHP_FIELD_DATA(HDMI_TX_PHY_HDMI_TX_PHY_CTL_1, ICPX,
						 	ulIcpx) |
						 BCHP_FIELD_DATA(HDMI_TX_PHY_HDMI_TX_PHY_CTL_1, LF_ORDER, 1) |
						 BCHP_FIELD_DATA(HDMI_TX_PHY_HDMI_TX_PHY_CTL_1, RZ, 1) |
						 BCHP_FIELD_DATA(HDMI_TX_PHY_HDMI_TX_PHY_CTL_1, CZ, 1) |
						 BCHP_FIELD_DATA(HDMI_TX_PHY_HDMI_TX_PHY_CTL_1, CP, 0) |
						 BCHP_FIELD_DATA(HDMI_TX_PHY_HDMI_TX_PHY_CTL_1, RN, 0) |
						 BCHP_FIELD_DATA(HDMI_TX_PHY_HDMI_TX_PHY_CTL_1, CN,
						 	ulCn),
						 BCHP_HDMI_TX_PHY_HDMI_TX_PHY_CTL_1);

	/* HDMI_TX_PHY_HDMI_TX_PHY_CTL_2 (RW) */
	BVDC_P_RD_MOD_WR_RUL(pList->pulCurrent,
						 ~(BCHP_MASK(HDMI_TX_PHY_HDMI_TX_PHY_CTL_2, KVCO_XS) |
						   BCHP_MASK(HDMI_TX_PHY_HDMI_TX_PHY_CTL_2, NDIV_MOD)),
						 BCHP_FIELD_DATA(HDMI_TX_PHY_HDMI_TX_PHY_CTL_2, KVCO_XS, 4) |
						 BCHP_FIELD_DATA(HDMI_TX_PHY_HDMI_TX_PHY_CTL_2, NDIV_MOD,
						 	ulNdivMod),
						 BCHP_HDMI_TX_PHY_HDMI_TX_PHY_CTL_2);

	/* Do not need Kick start PLL on STB */
	hDisplay->ulKickStartDelay = (0); /* vsync until kick start */

#else

	/* HDMI_RM_FORMAT (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HDMI_RM_FORMAT);
	*pList->pulCurrent++ = (
		BCHP_FIELD_DATA(HDMI_RM_FORMAT, STABLE_COUNT,               10000) |
		BCHP_FIELD_ENUM(HDMI_RM_FORMAT, OFFSET_ONLY,             DISABLE ) |
		BCHP_FIELD_DATA(HDMI_RM_FORMAT, RDIV_CTRL,        pRmInfo->ulRDiv) |
		BCHP_FIELD_DATA(HDMI_RM_FORMAT, OP_OFFSET,    pRmInfo->ulOpOffset) |
		BCHP_FIELD_DATA(HDMI_RM_FORMAT, OP_DITHER,                      0) );

	/* HDMI_RM_VCXO_CTRL (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HDMI_RM_VCXO_CTRL);
	*pList->pulCurrent++ = ulVcxoCtrl =
		BCHP_FIELD_DATA(HDMI_RM_VCXO_CTRL, LOCK,        1       ) |
		BCHP_FIELD_DATA(HDMI_RM_VCXO_CTRL, RSEN,        15      ) |
		BCHP_FIELD_DATA(HDMI_RM_VCXO_CTRL, SPARE,       0       ) |
		BCHP_FIELD_DATA(HDMI_RM_VCXO_CTRL, CLK_PATTERN, 0       ) |
		BCHP_FIELD_DATA(HDMI_RM_VCXO_CTRL, INT_RESETB,  1       ) |
		BCHP_FIELD_DATA(HDMI_RM_VCXO_CTRL, DIV_RST,     0       ) |
		BCHP_FIELD_DATA(HDMI_RM_VCXO_CTRL, FILRSTB,     1       ) |
		BCHP_FIELD_DATA(HDMI_RM_VCXO_CTRL, CLK_EDGE,    0       ) |
		BCHP_FIELD_DATA(HDMI_RM_VCXO_CTRL, PLL_PWRDN,   0       ) |
		BCHP_FIELD_DATA(HDMI_RM_VCXO_CTRL, BYPASS,      0       ) |
		BCHP_FIELD_DATA(HDMI_RM_VCXO_CTRL, BIT_CTRL,    1       ) |
		BCHP_FIELD_DATA(HDMI_RM_VCXO_CTRL, MDIV,        pRmInfo->ulMDiv) |
		BCHP_FIELD_DATA(HDMI_RM_VCXO_CTRL, NDIV,        pRmInfo->ulNDiv) |
		BCHP_FIELD_DATA(HDMI_RM_VCXO_CTRL, PDIV,        pRmInfo->ulPDiv) |
		BCHP_FIELD_ENUM(HDMI_RM_VCXO_CTRL, NEGCW,       NEGATE  ) |
		BCHP_FIELD_ENUM(HDMI_RM_VCXO_CTRL, TCOMP,       CONVERT );

	/* Kick start PLL!  This is taken from hdmi code, hw requires this in
	 * order for hdmi to start up correctly! */
	if(bDisplayRestart)
	{
		ulVcxoCtrl &= ~(BCHP_MASK(HDMI_RM_VCXO_CTRL, FILRSTB));
		ulVcxoCtrl |=  (BCHP_FIELD_DATA(HDMI_RM_VCXO_CTRL, FILRSTB,    0));
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HDMI_RM_VCXO_CTRL);
		*pList->pulCurrent++ = ulVcxoCtrl;

		ulVcxoCtrl &= ~(BCHP_MASK(HDMI_RM_VCXO_CTRL, DIV_RST));
		ulVcxoCtrl |=  (BCHP_FIELD_DATA(HDMI_RM_VCXO_CTRL, DIV_RST,    0));
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HDMI_RM_VCXO_CTRL);
		*pList->pulCurrent++ = ulVcxoCtrl;

		ulVcxoCtrl &= ~(BCHP_MASK(HDMI_RM_VCXO_CTRL, DIV_RST));
		ulVcxoCtrl |=  (BCHP_FIELD_DATA(HDMI_RM_VCXO_CTRL, DIV_RST,    1));
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HDMI_RM_VCXO_CTRL);
		*pList->pulCurrent++ = ulVcxoCtrl;

		ulVcxoCtrl &= ~(BCHP_MASK(HDMI_RM_VCXO_CTRL, INT_RESETB));
		ulVcxoCtrl |=  (BCHP_FIELD_DATA(HDMI_RM_VCXO_CTRL, INT_RESETB, 1));
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HDMI_RM_VCXO_CTRL);
		*pList->pulCurrent++ = ulVcxoCtrl;

		ulVcxoCtrl &= ~(BCHP_MASK(HDMI_RM_VCXO_CTRL, INT_RESETB));
		ulVcxoCtrl |=  (BCHP_FIELD_DATA(HDMI_RM_VCXO_CTRL, INT_RESETB, 0));
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HDMI_RM_VCXO_CTRL);
		*pList->pulCurrent++ = ulVcxoCtrl;

		ulVcxoCtrl &= ~(BCHP_MASK(HDMI_RM_VCXO_CTRL, INT_RESETB));
		ulVcxoCtrl |=  (BCHP_FIELD_DATA(HDMI_RM_VCXO_CTRL, INT_RESETB, 1));
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HDMI_RM_VCXO_CTRL);
		*pList->pulCurrent++ = ulVcxoCtrl;

		ulVcxoCtrl &= ~(BCHP_MASK(HDMI_RM_VCXO_CTRL, DIV_RST));
		ulVcxoCtrl |=  (BCHP_FIELD_DATA(HDMI_RM_VCXO_CTRL, DIV_RST,    0));
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HDMI_RM_VCXO_CTRL);
		*pList->pulCurrent++ = ulVcxoCtrl;

		ulVcxoCtrl &= ~(BCHP_MASK(HDMI_RM_VCXO_CTRL, FILRSTB));
		ulVcxoCtrl |=  (BCHP_FIELD_DATA(HDMI_RM_VCXO_CTRL, FILRSTB,    1));
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HDMI_RM_VCXO_CTRL);
		*pList->pulCurrent++ = ulVcxoCtrl;
	}
#endif /* modularvec */

#elif BVDC_P_SUPPORT_DVPO

	BSTD_UNUSED(ulVcxoCtrl);
	BSTD_UNUSED(bDisplayRestart);

	pDvoCfg = &pNewInfo->stDvoCfg;

	/* DVPO_RM_0_CONTROL (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_DVPO_RM_0_CONTROL);
	*pList->pulCurrent++ =
		BCHP_FIELD_ENUM(DVPO_RM_0_CONTROL, RESET,       RESET_OFF  ) |
		BCHP_FIELD_DATA(DVPO_RM_0_CONTROL, INT_GAIN,    4          ) |
		BCHP_FIELD_DATA(DVPO_RM_0_CONTROL, DIRECT_GAIN, 4          ) |
		BCHP_FIELD_ENUM(DVPO_RM_0_CONTROL, DITHER,      DITHER_OFF ) |
		BCHP_FIELD_ENUM(DVPO_RM_0_CONTROL, FREE_RUN,    TIMEBASE   ) |
		BCHP_FIELD_DATA(DVPO_RM_0_CONTROL, TIMEBASE,    pNewInfo->eTimeBase);

	/* DVPO_RM_0_RATE_RATIO (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_DVPO_RM_0_RATE_RATIO);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(DVPO_RM_0_RATE_RATIO, MAX_ERR,     0 ) |
		BCHP_FIELD_DATA(DVPO_RM_0_RATE_RATIO, MIN_ERR,     0 ) |
		BCHP_FIELD_DATA(DVPO_RM_0_RATE_RATIO, DENOMINATOR, pRmInfo->ulDenominator);

	/* DVPO_RM_0_SAMPLE_INC (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_DVPO_RM_0_SAMPLE_INC);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(DVPO_RM_0_SAMPLE_INC, NUMERATOR,  pRmInfo->ulNumerator) |
		BCHP_FIELD_DATA(DVPO_RM_0_SAMPLE_INC, SAMPLE_INC, pRmInfo->ulSampleInc);

	/* DVPO_RM_0_PHASE_INC (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_DVPO_RM_0_PHASE_INC);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(DVPO_RM_0_PHASE_INC, PHASE_INC, 0);

	/* DVPO_RM_0_FORMAT (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_DVPO_RM_0_FORMAT);
	*pList->pulCurrent++ = (
		BCHP_FIELD_DATA(DVPO_RM_0_FORMAT, SHIFT,        3    ) |
		BCHP_FIELD_DATA(DVPO_RM_0_FORMAT, STABLE_COUNT, 10000) );

	/* DVPO_RM_0_OFFSET (RW) */
	BVDC_P_Vec_BuildPllCfg_isr(pList, hDisplay->hVdc->hRegister,
		&pNewInfo->stDvoCfg, pRmInfo, pRmInfo->ulRDiv, pRmInfo->ulNDiv);

	/* LVDS_PHY_0_LVDS_PLL_CTL_1 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_LVDS_PHY_0_LVDS_PLL_CTL_1);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(LVDS_PHY_0_LVDS_PLL_CTL_1, test_buffer,         0 ) |
		BCHP_FIELD_DATA(LVDS_PHY_0_LVDS_PLL_CTL_1, kvco_xs,             5 ) |
		BCHP_FIELD_DATA(LVDS_PHY_0_LVDS_PLL_CTL_1, kvco_xf,             0 ) |
		BCHP_FIELD_DATA(LVDS_PHY_0_LVDS_PLL_CTL_1, lpf_bw,              0 ) |
		BCHP_FIELD_DATA(LVDS_PHY_0_LVDS_PLL_CTL_1, LF_order,            0 ) |
		BCHP_FIELD_DATA(LVDS_PHY_0_LVDS_PLL_CTL_1, Cn,                  0 ) |
		BCHP_FIELD_DATA(LVDS_PHY_0_LVDS_PLL_CTL_1, Rn,                  0 ) |
		BCHP_FIELD_DATA(LVDS_PHY_0_LVDS_PLL_CTL_1, Cp,                  1 ) |
		BCHP_FIELD_DATA(LVDS_PHY_0_LVDS_PLL_CTL_1, Cz,                  1 ) |
		BCHP_FIELD_DATA(LVDS_PHY_0_LVDS_PLL_CTL_1, Rz,                  1 ) |
		BCHP_FIELD_DATA(LVDS_PHY_0_LVDS_PLL_CTL_1, lcpx,                31) |
		BCHP_FIELD_DATA(LVDS_PHY_0_LVDS_PLL_CTL_1, freq_det_dis,        0 ) |
		BCHP_FIELD_DATA(LVDS_PHY_0_LVDS_PLL_CTL_1, AVDD1p2,             1 ) |
		BCHP_FIELD_DATA(LVDS_PHY_0_LVDS_PLL_CTL_1, pll_ctrl_1_reserved, 0 );

	/* LVDS_PHY_0_LVDS_PLL_CTL_2 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_LVDS_PHY_0_LVDS_PLL_CTL_2);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(LVDS_PHY_0_LVDS_PLL_CTL_2, ndiv_mode,  2) |
		BCHP_FIELD_DATA(LVDS_PHY_0_LVDS_PLL_CTL_2, txvcom_sel, 3) |
		BCHP_FIELD_DATA(LVDS_PHY_0_LVDS_PLL_CTL_2, txbg_adjust,4) |
		BCHP_FIELD_DATA(LVDS_PHY_0_LVDS_PLL_CTL_2, ptap_adj,   2) |
		BCHP_FIELD_DATA(LVDS_PHY_0_LVDS_PLL_CTL_2, ctap_adj,   2) |
		BCHP_FIELD_DATA(LVDS_PHY_0_LVDS_PLL_CTL_2, LDO_vout,   2) |
		BCHP_FIELD_DATA(LVDS_PHY_0_LVDS_PLL_CTL_2, biasin_en,  1);

	/* LVDS_PHY_0_SPREAD_SPECTRUM (RW) */
	ulSSReg =
		BCHP_FIELD_DATA(LVDS_PHY_0_SPREAD_SPECTRUM, LOAD,   0      ) |
		BCHP_FIELD_DATA(LVDS_PHY_0_SPREAD_SPECTRUM, ENABLE, 0      ) |
		BCHP_FIELD_DATA(LVDS_PHY_0_SPREAD_SPECTRUM, SCALE,  ulScale) |
		BCHP_FIELD_DATA(LVDS_PHY_0_SPREAD_SPECTRUM, RANGE,  ulRange) |
		BCHP_FIELD_DATA(LVDS_PHY_0_SPREAD_SPECTRUM, HOLD,   ulHold ) |
		BCHP_FIELD_DATA(LVDS_PHY_0_SPREAD_SPECTRUM, STEP,   ulStep );

	/****************************************************************/
	/* Require specific protocol to follow in order to turn on/off. */
	/****************************************************************/
	/* (1) Disable spread spectrum and put in new changes. */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_LVDS_PHY_0_SPREAD_SPECTRUM);
	*pList->pulCurrent++ = ulSSReg;

	/* (2) Clock in the new changes, LOAD: 1 -> 0 */
	ulSSReg |= (
		BCHP_FIELD_DATA(LVDS_PHY_0_SPREAD_SPECTRUM, LOAD, 1));
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_LVDS_PHY_0_SPREAD_SPECTRUM);
	*pList->pulCurrent++ = ulSSReg;

	/* (3) Clock in the new changes, LOAD: 1 -> 0 */
	ulSSReg &= ~(
		BCHP_MASK(LVDS_PHY_0_SPREAD_SPECTRUM, LOAD));
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_LVDS_PHY_0_SPREAD_SPECTRUM);
	*pList->pulCurrent++ = ulSSReg;

	/* (4) reenable the Spread Spectrum after making the changes */
	ulSSReg |= (
		BCHP_FIELD_DATA(LVDS_PHY_0_SPREAD_SPECTRUM, ENABLE, pDvoCfg->stSpreadSpectrum.bEnable));
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_LVDS_PHY_0_SPREAD_SPECTRUM);
	*pList->pulCurrent++ = ulSSReg;

	/* Kick start PLL!  This is taken from hdmi code, hw requires this in
	 * order for hdmi to start up correctly!  This is needed after we
	 * enable sprectrum. */
	hDisplay->ulKickStartDelay = (2); /* vsync until kick start */

#else
	BSTD_UNUSED(pList);
#endif

	BDBG_LEAVE(BVDC_P_Vec_Build_HdmiRM_isr);
	return;
}
#else
#define BVDC_P_Vec_Build_HdmiRM_isr(hDisplay, pList)          BDBG_ASSERT(0)
#endif

/*************************************************************************
 *  {secret}
 *	BVDC_P_Vec_Build_IT_isr
 *	Builds IT and Video_Enc blocks (for mode switch)
 **************************************************************************/
static void BVDC_P_Vec_Build_IT_isr
	( BVDC_Display_Handle              hDisplay,
	  BVDC_P_ListInfo                 *pList,
	  uint32_t                         ulHeight,
	  uint32_t                         ulTrig0Val,
	  uint32_t                         ulTrig1Val )
{
	const uint32_t      *pRamTbl = NULL;
	const uint32_t      *pTable  = NULL;
	uint32_t             ulItConfig = 0;
	BVDC_P_DisplayInfo  *pNewInfo = &hDisplay->stNewInfo;
	uint32_t             ulVecOffset = hDisplay->lItOffset;

	/* get correct ItTable, RamTable, and ItConfig */
	if((VIDEO_FORMAT_SUPPORTS_MACROVISION(pNewInfo->pFmtInfo->eVideoFmt)) ||
	   (VIDEO_FORMAT_SUPPORTS_DCS        (pNewInfo->pFmtInfo->eVideoFmt))   )
	{
		pTable     = BVDC_P_GetItTableMv_isr(pNewInfo);
		pRamTbl    = BVDC_P_GetRamTableMv_isr(
						hDisplay->eVecPath, pNewInfo, hDisplay->bArib480p);
		ulItConfig = BVDC_P_GetItConfigMv_isr(pNewInfo);
	}
	else
	{
		pTable     = BVDC_P_GetItTable_isr(pNewInfo);
		pRamTbl    = BVDC_P_GetRamTable_isr(pNewInfo, hDisplay->bArib480p);
		ulItConfig = BVDC_P_GetItConfig_isr(pNewInfo);
	}
	BDBG_ASSERT (pTable);

	BDBG_MSG(("Analog vec microcode - timestamp: 0x%.8x", pRamTbl[BVDC_P_RAM_TABLE_TIMESTAMP_IDX]));
	BDBG_MSG(("Analog vec microcode - checksum:  0x%.8x", pRamTbl[BVDC_P_RAM_TABLE_CHECKSUM_IDX]));

	/* to support MV mode control byte change */
	if (VIDEO_FORMAT_SUPPORTS_MACROVISION(pNewInfo->pFmtInfo->eVideoFmt) &&
	    (BVDC_P_Get_Prot_Alt() != BVDC_P_PROT_ALT_DCS ))
	{
		uint32_t ulAddr46Offset;
		uint32_t ulPcl4Offset;
		uint32_t ulAddr46;
		uint32_t ulPcl4;

		/* --- Setup IT block --- */
		*pList->pulCurrent++ = BRDC_OP_IMMS_TO_REGS(BVDC_P_IT_TABLE_SIZE);
		*pList->pulCurrent++ =
			BRDC_REGISTER(BCHP_PRIM_IT_ADDR_0_3 + ulVecOffset);

		/* Setup IT_ADDR_0_3 to IT_PCL_5 */
		BKNI_Memcpy((void*)pList->pulCurrent, (void*)pTable,
			BVDC_P_IT_TABLE_SIZE * sizeof(uint32_t));

		ulAddr46Offset =
			(BCHP_PRIM_IT_ADDR_4_6 - BCHP_PRIM_IT_ADDR_0_3)/sizeof(uint32_t);
		ulPcl4Offset =
			(BCHP_PRIM_IT_PCL_4 - BCHP_PRIM_IT_ADDR_0_3)/sizeof(uint32_t);
		ulAddr46 = pTable[ulAddr46Offset];
		ulPcl4   = pTable[ulPcl4Offset];

		/* if toggle N0 control bits for CS(MC4/5) or BP(PCL_4) */
		if(!pNewInfo->stN0Bits.bBp)
		{
			ulPcl4 &= ~(BCHP_MASK(PRIM_IT_PCL_4, PSB_AND_TERM_2));
			ulPcl4 |= BCHP_FIELD_ENUM(PRIM_IT_PCL_4, PSB_AND_TERM_2, ZERO);
			*(pList->pulCurrent + ulPcl4Offset) = ulPcl4;
		}
		if(!pNewInfo->stN0Bits.bCs && (!(VIDEO_FORMAT_IS_SECAM(pNewInfo->pFmtInfo->eVideoFmt))))
		{
			ulAddr46 &= ~(
				BCHP_MASK(PRIM_IT_ADDR_4_6, MC_4_START_ADDR) |
				BCHP_MASK(PRIM_IT_ADDR_4_6, MC_5_START_ADDR));
			ulAddr46 |= (
				BCHP_FIELD_DATA(PRIM_IT_ADDR_4_6, MC_4_START_ADDR, 0xfd) |
				BCHP_FIELD_DATA(PRIM_IT_ADDR_4_6, MC_5_START_ADDR, 0xfd));

			*(pList->pulCurrent + ulAddr46Offset) = ulAddr46;
		}

		/* move pointer after adjustment */
		pList->pulCurrent += BVDC_P_IT_TABLE_SIZE;
	}
	else
	{
		/* --- Setup IT block --- */
		*pList->pulCurrent++ = BRDC_OP_IMMS_TO_REGS(BVDC_P_IT_TABLE_SIZE);
		*pList->pulCurrent++ =
			BRDC_REGISTER(BCHP_PRIM_IT_ADDR_0_3 + ulVecOffset);

		/* Setup IT_ADDR_0_3 to IT_PCL_5 */
		BKNI_Memcpy((void*)pList->pulCurrent, (void*)pTable,
			BVDC_P_IT_TABLE_SIZE * sizeof(uint32_t));

		/* move pointer after adjustment */
		pList->pulCurrent += BVDC_P_IT_TABLE_SIZE;
	}

	/* Setup SEC_IT_BVB_SIZE */
	/* TODO: Does DCS require work here? */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_PRIM_IT_BVB_SIZE + ulVecOffset);
	if(pNewInfo->bWidthTrimmed && VIDEO_FORMAT_IS_NTSC(pNewInfo->pFmtInfo->eVideoFmt) &&
	   (pNewInfo->eMacrovisionType < BVDC_MacrovisionType_eTest01))
	{
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(PRIM_IT_BVB_SIZE, HORIZONTAL,712) |
			BCHP_FIELD_DATA(PRIM_IT_BVB_SIZE, VERTICAL, ulHeight);
	}
	else
	{
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(PRIM_IT_BVB_SIZE, HORIZONTAL,pNewInfo->pFmtInfo->ulWidth) |
			BCHP_FIELD_DATA(PRIM_IT_BVB_SIZE, VERTICAL, ulHeight);
	}

	/* Setup Vec triggers */
	/* Must be in the Rul, otherwise the Reset will destroy it */
	*pList->pulCurrent++ = BRDC_OP_IMMS_TO_REGS(2);
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_PRIM_IT_VEC_TRIGGER_0 + ulVecOffset);

	/* Set 1st trigger */
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(PRIM_IT_VEC_TRIGGER_0, TRIGGER_VALUE, ulTrig0Val) |
		BCHP_FIELD_DATA(PRIM_IT_VEC_TRIGGER_0, ENABLE, 0);

	/* Set 2nd trigger (for interlaced modes) */
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(PRIM_IT_VEC_TRIGGER_1, TRIGGER_VALUE, ulTrig1Val) |
		BCHP_FIELD_DATA(PRIM_IT_VEC_TRIGGER_1, ENABLE, 0);

	/* Write IT_MICRO_INSTRUCTIONS 0..127 */
	*pList->pulCurrent++ = BRDC_OP_IMMS_TO_REGS(BVDC_P_RAM_TABLE_SIZE);
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_PRIM_IT_MICRO_INSTRUCTIONi_ARRAY_BASE + ulVecOffset);

	BKNI_Memcpy((void*)pList->pulCurrent, (void*)pRamTbl,
		BVDC_P_RAM_TABLE_SIZE * sizeof(uint32_t));
	pList->pulCurrent += BVDC_P_RAM_TABLE_SIZE;
	hDisplay->ulCurShadowDCSKeyLow = hDisplay->stNewInfo.ulDCSKeyLow;

#if (BVDC_P_SUPPORT_HDMI_OUT || BVDC_P_SUPPORT_ITU656_OUT)
	/* Setup DTRAM_CONFIG. */
	if ((pNewInfo->bEnable656) || (pNewInfo->bEnableHdmi))
	{
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_DTRAM_DTRAM_CONFIG);
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(DTRAM_DTRAM_CONFIG, ARBITER_LATENCY, 0xb);
	}
#endif

	/* This RUL contains the reset which produce the extra interrupts
	 * tell isr to ignore these and response to trigger executed rdc.
	 * In a way this also give the vec a chance to become stablize before
	 * starting the bvn front-end modules. */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(hDisplay->ulRdcVarAddr);
	*pList->pulCurrent++ = 1;

	/* MUST be LAST one during initialization!!! */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_PRIM_IT_TG_CONFIG + ulVecOffset);
	*pList->pulCurrent++ = ulItConfig;

	return;
}

/*************************************************************************
 *  {secret}
 *  BVDC_P_Vec_Build_CSC_SRC_SM_isr
 *  Adds CSC and SRC blocks to RUL for a display.
 *  Bypass - CSC
 *  Secondary - SEC_CSC(SVideo/composite),
 *              SEC_CSC_CO(component)
 *              SEC_SRC, SM
 **************************************************************************/
static void BVDC_P_Vec_Build_CSC_SRC_SM_isr
	( BVDC_Display_Handle              hDisplay,
	  BVDC_P_ListInfo                 *pList )
{
	const BVDC_P_DisplayCscMatrix *pCscMatrix = NULL;
	const uint32_t          *pTable = NULL;
	BVDC_P_DisplayInfo      *pNewInfo =
	                         &hDisplay->stNewInfo;
	uint32_t                 ulSrcControl = BVDC_P_GetSrcControl_isr(pNewInfo);
	BVDC_P_DisplayCscMatrix  stCscMatrix;

	const BVDC_P_Compositor_Info *pCmpInfo = &hDisplay->hCompositor->stNewInfo;
	uint8_t ucCh0 = BPXL_GET_COMPONENT(BPXL_eA8_Y8_Cb8_Cr8, pCmpInfo->ulBgColorYCrCb, 2);
	uint8_t ucCh1 = BPXL_GET_COMPONENT(BPXL_eA8_Y8_Cb8_Cr8, pCmpInfo->ulBgColorYCrCb, 1);
	uint8_t ucCh2 = BPXL_GET_COMPONENT(BPXL_eA8_Y8_Cb8_Cr8, pCmpInfo->ulBgColorYCrCb, 0);

	/* Setup Main CSC */
	if( pNewInfo->eOutputColorSpace != BVDC_P_Output_eUnknown )
	{
		*pList->pulCurrent++ = BRDC_OP_IMMS_TO_REGS(BVDC_P_CSC_TABLE_SIZE);
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_PRIM_CSC_CSC_MODE + hDisplay->lCscOffset);

		/* Get the color matrix table for CSC output */
		BVDC_P_Display_GetCscTable_isr(pNewInfo, pNewInfo->eOutputColorSpace, &pCscMatrix);

		stCscMatrix = *pCscMatrix;

		/* TODO: handle user csc */
		/* Handle CSC mute */
		if (((pNewInfo->abOutputMute[BVDC_DisplayOutput_eComponent]) &&
			 (pNewInfo->eCoOutputColorSpace != BVDC_P_Output_eUnknown) &&
			 (pNewInfo->eCoOutputColorSpace != BVDC_P_Output_eHsync)) ||
			(pNewInfo->abOutputMute[BVDC_DisplayOutput_eComposite]) ||
			(pNewInfo->abOutputMute[BVDC_DisplayOutput_eSVideo]))
		{
			/* Swap ch0 and 1 of input color to match vec csc layout */
			BVDC_P_Csc_ApplyYCbCrColor(&stCscMatrix.stCscCoeffs, ucCh1, ucCh0, ucCh2);
		}

		/* Setup CSC for Dac outputs */
		BVDC_P_Vec_Build_CSC_isr(&stCscMatrix, pList);

#if (BVDC_P_SUPPORT_VEC_DITHER)
	*pList->pulCurrent++ = BRDC_OP_IMMS_TO_REGS(BVDC_P_DITHER_TABLE_SIZE);
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_PRIM_CSC_DITHER_CONTROL + hDisplay->lCscOffset);
	BVDC_P_Vec_Build_Dither_isr(hDisplay, &hDisplay->stCscDither, pList);
#endif

		/* Setup SRC */
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_PRIM_SRC_SRC_CONTROL + hDisplay->lSrcOffset);
		*pList->pulCurrent++ = ulSrcControl;
	}

#if (BVDC_P_SUPPORT_COMPONENT_ONLY)
	/* Setup Component-only CSC */
	if((pNewInfo->eCoOutputColorSpace != BVDC_P_Output_eUnknown) &&
	   (pNewInfo->eCoOutputColorSpace != BVDC_P_Output_eHsync))
	{
		*pList->pulCurrent++ = BRDC_OP_IMMS_TO_REGS(BVDC_P_CSC_TABLE_SIZE);
		*pList->pulCurrent++ = BRDC_REGISTER(BVDC_P_VEC_CSC_CO_CSC_MODE);

		/* Get the color matrix table for CSC output */
		BVDC_P_Display_GetCscTable_isr(pNewInfo, pNewInfo->eCoOutputColorSpace, &pCscMatrix);

		stCscMatrix = *pCscMatrix;

		/* TODO: handle user csc */
		/* Handle CSC mute */
		if (pNewInfo->abOutputMute[BVDC_DisplayOutput_eComponent])
		{
			/* Swap ch0 and 1 of input color to match vec csc layout */
			BVDC_P_Csc_ApplyYCbCrColor(&stCscMatrix.stCscCoeffs, ucCh1, ucCh0, ucCh2);
		}

		/* Setup CSC for Dac outputs */
		BVDC_P_Vec_Build_CSC_isr(&stCscMatrix, pList);

		/* Setup SRC_CO */
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BVDC_P_VEC_SRC_CO_SRC_CONTROL);
		*pList->pulCurrent++ = ulSrcControl;
	}
#endif

	/* --- Setup SM --- */
	*pList->pulCurrent++ = BRDC_OP_IMMS_TO_REGS(BVDC_P_SM_TABLE_SIZE);
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_PRIM_SM_ENVELOPE_GENERATOR + hDisplay->lSmOffset);

	/* get the correct sm table */
	pTable = BVDC_P_GetSmTable_isr(pNewInfo, pNewInfo->eOutputColorSpace);

	/* SM_ENVELOPE_GENERATOR -> SM_COMP_CNTRL */
	BKNI_Memcpy((void*)pList->pulCurrent, (void*)pTable,
		BVDC_P_SM_TABLE_SIZE * sizeof(uint32_t));
	pList->pulCurrent += BVDC_P_SM_TABLE_SIZE;

	/* Setup SM_COMP_CONFIG */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_PRIM_SM_COMP_CONFIG + hDisplay->lSmOffset);

	if(VIDEO_FORMAT_IS_NTSC(pNewInfo->pFmtInfo->eVideoFmt))
	{
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(PRIM_SM_COMP_CONFIG, U_LINE_SEL, 0) |
			BCHP_FIELD_DATA(PRIM_SM_COMP_CONFIG, COMPOSITE_CLAMP_SEL, 1) |
			BCHP_FIELD_DATA(PRIM_SM_COMP_CONFIG, V_LINE_SEL, 0) |
			BCHP_FIELD_DATA(PRIM_SM_COMP_CONFIG, U_FIXED_LINE, 0) |
			BCHP_FIELD_DATA(PRIM_SM_COMP_CONFIG, COMPOSITE_OFFSET, 0);
	}
#if BVDC_P_SUPPORT_VEC_SECAM
	else if(VIDEO_FORMAT_IS_SECAM(pNewInfo->pFmtInfo->eVideoFmt))
	{
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(PRIM_SM_COMP_CONFIG, U_LINE_SEL, 0)          |
			BCHP_FIELD_DATA(PRIM_SM_COMP_CONFIG, COMPOSITE_CLAMP_SEL, 1) |
 			BCHP_FIELD_DATA(PRIM_SM_COMP_CONFIG, V_LINE_SEL,          1) |
			BCHP_FIELD_DATA(PRIM_SM_COMP_CONFIG, U_FIXED_LINE, 0)        |
			BCHP_FIELD_DATA(PRIM_SM_COMP_CONFIG, COMPOSITE_OFFSET, 0xd0);
	}
#endif
	else /* PAL */
	{
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(PRIM_SM_COMP_CONFIG, U_LINE_SEL, 0)          |
			BCHP_FIELD_DATA(PRIM_SM_COMP_CONFIG, COMPOSITE_CLAMP_SEL, 1) |
			BCHP_FIELD_DATA(PRIM_SM_COMP_CONFIG, V_LINE_SEL,
			((pNewInfo->eOutputColorSpace == BVDC_P_Output_eYUV)   ||
			 (pNewInfo->eOutputColorSpace == BVDC_P_Output_eYUV_N) ||
			 (pNewInfo->eOutputColorSpace == BVDC_P_Output_eYUV_NC)))     |
			BCHP_FIELD_DATA(PRIM_SM_COMP_CONFIG, U_FIXED_LINE, 0)        |
			BCHP_FIELD_DATA(PRIM_SM_COMP_CONFIG, COMPOSITE_OFFSET, 0);
	}

#if BVDC_P_SUPPORT_VEC_SECAM
	/* Setup SM_FM_CONTROL */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_PRIM_SM_FM_CONTROL + hDisplay->lSmOffset);
	if(VIDEO_FORMAT_IS_SECAM(pNewInfo->pFmtInfo->eVideoFmt))
	{
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(PRIM_SM_FM_CONTROL, FINE_LUMA_DELAY,  0) |
			BCHP_FIELD_DATA(PRIM_SM_FM_CONTROL, GROSS_LUMA_DELAY, 0x18) |
			BCHP_FIELD_DATA(PRIM_SM_FM_CONTROL, FINE_SC_DELAY,    0x3) |
			BCHP_FIELD_DATA(PRIM_SM_FM_CONTROL, GROSS_SC_DELAY,   0x8) |
			BCHP_FIELD_ENUM(PRIM_SM_FM_CONTROL, ENABLE, ON);

		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_PRIM_SM_FM_FMAMP + hDisplay->lSmOffset);
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(PRIM_SM_FM_FMAMP, LOWER_LIMIT,  64) |
			BCHP_FIELD_DATA(PRIM_SM_FM_FMAMP, UPPER_LIMIT, 192) |
			BCHP_FIELD_DATA(PRIM_SM_FM_FMAMP, SLOPE_ADJUST,  3) |
			BCHP_FIELD_DATA(PRIM_SM_FM_FMAMP, SCALE, 92);
	}
	else
	{
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(PRIM_SM_FM_CONTROL, GROSS_LUMA_DELAY, 0x18) |
			BCHP_FIELD_DATA(PRIM_SM_FM_CONTROL, GROSS_SC_DELAY,   0x18);
	}
#endif
	return;
}


/*************************************************************************
 *  {secret}
 *	BVDC_P_Vec_Build_VF_MISC_isr
 *	Builds either: SEC_VF_MISC, SEC_VF_CO_MISC or PRIM_VF_MISC register
 *  Required for Videoformat or colorspace change. Also required EVERY
 *  VSYNC, if a certain hardware bug exists.
 **************************************************************************/
static uint32_t BVDC_P_Vec_Get_VF_MISC_isr
	( BVDC_P_DisplayInfo                  *pNewInfo,
	  bool                                bArib480p,
	  const uint32_t                      *pVfValuesTable,
	  const BVDC_P_Display_ShaperSettings *pShaperSettings
)
{
	uint32_t             ulVfMiscRegIdx, ulVfMiscRegValue;

	ulVfMiscRegIdx =
		(BCHP_PRIM_VF_MISC - BCHP_PRIM_VF_FORMAT_ADDER) / sizeof(uint32_t);
	ulVfMiscRegValue =
		pVfValuesTable[ulVfMiscRegIdx] &
			~BCHP_MASK(PRIM_VF_MISC, BVB_SAV_REMOVE);
	ulVfMiscRegValue |=
		BCHP_FIELD_DATA(
			PRIM_VF_MISC, BVB_SAV_REMOVE, pShaperSettings->ulSavRemove);

	/* PAL line 23 is reserved for WSS vbi data; Cx and beyond can drop the
     * 1st active video line from BVN; */
	if (VIDEO_FORMAT_IS_625_LINES(pNewInfo->pFmtInfo->eVideoFmt))
	{
		ulVfMiscRegValue |=
			BCHP_FIELD_DATA(PRIM_VF_MISC, BVB_LINE_REMOVE_TOP, 1);
	}

	/* Also for PAL-N. */
	else if ((pNewInfo->pFmtInfo->eVideoFmt == BFMT_VideoFmt_ePAL_N) &&
			 ((pNewInfo->eMacrovisionType ==
			 	BVDC_MacrovisionType_eNoProtection) ||
			 (pNewInfo->eMacrovisionType ==
			 	BVDC_MacrovisionType_eTest01) ||
			 (pNewInfo->eMacrovisionType ==
			 	BVDC_MacrovisionType_eTest02)) )
	{
		ulVfMiscRegValue |=
			BCHP_FIELD_DATA(PRIM_VF_MISC, BVB_LINE_REMOVE_TOP, 1);
	}
	/* Also special case for ARIB NTSC */
	else if(VIDEO_FORMAT_IS_NTSC(pNewInfo->pFmtInfo->eVideoFmt) &&
		    (true  == bArib480p) &&
            (false == pNewInfo->bWidthTrimmed))
	{
		ulVfMiscRegValue |= (
			BCHP_FIELD_DATA(PRIM_VF_MISC, BVB_LINE_REMOVE_TOP   , 1) |
			BCHP_FIELD_DATA(PRIM_VF_MISC, BVB_LINE_REMOVE_BOTTOM, 1) );
	}

	return ulVfMiscRegValue;
}


/*************************************************************************
 *  {secret}
 *	BVDC_P_Vec_Build_VF_isr
 *	Builds either: SEC_VF, SEC_VF_CO or PRIM_VF
 *  Required for Videoformat or colorspace change
 **************************************************************************/
static void BVDC_P_Vec_Build_VF_isr
	( BVDC_Display_Handle              hDisplay,
	  BVDC_P_ListInfo                 *pList,
	  uint32_t                         ulFormatAdder,
	  BVDC_P_Output                    eOutputColorSpace )
{
	BVDC_P_Display_ShaperSettings stShaperSettings;
	uint32_t             ulNumRegs;
	uint32_t             ulVfMiscRegIdx, ulVfMiscRegValue;
	uint32_t             ulVfSTRegIdx,   ulVfSTRegValue;
	uint32_t            *pulFmtAdder;
	uint32_t            *pulPosSync;
	uint32_t             ulVal;
	const uint32_t     **apVfFilter;
	const uint32_t      *pTable=0;
	BVDC_P_DisplayInfo  *pNewInfo = &hDisplay->stNewInfo;
	uint32_t             i=0;

	/* Channel band-limiting filter
	                    Ch0    Ch1    Ch2
	YQI (NTSC,NTSC_J)   6.0    0.6    1.3
	YUV (M,N,NC)        4.2    1.3    1.3
	YUV (B,B1,D1,G,H)   5.0    1.3    1.3
	YUV (I)             5.5    1.3    1.3
	YUV (D,K,K1)        6.0    1.3    1.3
	RGB                 6.75   6.75   6.75
	YPrPb               6.75   3.375  3.375 */

	/* get the correct channel filters */
	for (i=0; i < BVDC_P_VEC_CH_NUM; i++)
	{
		if(BVDC_P_DISP_IS_COMPONENT(eOutputColorSpace))
		{
			BVDC_P_GetChFilters_isr(pNewInfo, eOutputColorSpace, &hDisplay->apVfFilterCo[0], &hDisplay->apVfFilterCo[1], &hDisplay->apVfFilterCo[2]);
			if(pNewInfo->abUserVfFilterCo[i])
			{
				hDisplay->apVfFilterCo[i] = (const uint32_t *)pNewInfo->aaulUserVfFilterCo[i];
			}
			apVfFilter = hDisplay->apVfFilterCo;
		}
		else
		{
			BVDC_P_GetChFilters_isr(pNewInfo, eOutputColorSpace, &hDisplay->apVfFilterCvbs[0], &hDisplay->apVfFilterCvbs[1], &hDisplay->apVfFilterCvbs[2]);
			if(pNewInfo->abUserVfFilterCvbs[i])
			{
				hDisplay->apVfFilterCvbs[i] = (const uint32_t *)pNewInfo->aaulUserVfFilterCvbs[i];
			}
			apVfFilter = hDisplay->apVfFilterCvbs;
		}
	}

	/* get the correct vf table */
	BVDC_P_GetVfTable_isr(pNewInfo, eOutputColorSpace, &pTable, &stShaperSettings);

	/* prepare for setting vec BVB left cut */
#if !BVDC_P_PR14712_FIXED
	stShaperSettings.ulSavRemove = 0;
#endif

	/* Prepare for setting *_VF_MISC and SEC_VF_CO_MISC, if it exists. */
	ulVfMiscRegIdx =
		(BCHP_PRIM_VF_MISC - BCHP_PRIM_VF_FORMAT_ADDER) / sizeof(uint32_t);
	ulVfMiscRegValue =
		BVDC_P_Vec_Get_VF_MISC_isr (
		    pNewInfo, hDisplay->bArib480p, pTable, &stShaperSettings);

	/* Prepare for settng the _VF_SYNC_TRANS_0 register */
	ulVfSTRegIdx =
		(BCHP_PRIM_VF_SYNC_TRANS_0 - BCHP_PRIM_VF_FORMAT_ADDER) /
			sizeof(uint32_t);
	ulVfSTRegValue =
		BVDC_P_Display_Modify_SYNC_TRANS_0 (pNewInfo, pTable[ulVfSTRegIdx]);

#if BVDC_P_SUPPORT_COMPONENT_ONLY
	/* When using Secondary Vec, must make sure Vec consumes
	 * pixels at the same rate for both SEC_VF and SEC_VF_CO */
	if (BVDC_P_VEC_PATH_SUPPORT_CO(hDisplay->eVecPath))
	{
		uint32_t ulOtherMiscReg =
			(ulFormatAdder == BVDC_P_VEC_VF_CO_FORMAT_ADDER) ?
				BVDC_P_VEC_VF_MISC : BVDC_P_VEC_VF_CO_MISC;

		if((ulOtherMiscReg == BVDC_P_VEC_VF_CO_MISC) ||
		   (pNewInfo->eOutputColorSpace != BVDC_P_Output_eHsync))
		{
			*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
			*pList->pulCurrent++ = BRDC_REGISTER(ulOtherMiscReg);
			*pList->pulCurrent++ = ulVfMiscRegValue;
		}
	}
#endif

	ulNumRegs = (BCHP_PRIM_VF_SHAPER - BCHP_PRIM_VF_FORMAT_ADDER)/4+1 ;

	/* Setup VF_FORMAT_ADDER -> VF_BVB_STATUS */
	*pList->pulCurrent++ = BRDC_OP_IMMS_TO_REGS(ulNumRegs);
	*pList->pulCurrent++ = BRDC_REGISTER(ulFormatAdder);

	/* Save RUL table entry for VF_FORMAT_ADDER. It will be modified later. */
	pulFmtAdder = pList->pulCurrent;

	/* Save RUL table entry for VF_POS_SYNC_VALUES. It will be modified
	 * later. */
	pulPosSync =
		pList->pulCurrent +
			(BCHP_PRIM_VF_POS_SYNC_VALUES - BCHP_PRIM_VF_FORMAT_ADDER)/4;

	/* Setup VF_FORMAT_ADDER -> SEC_VF_SYNC_TRANS_1 */
	BKNI_Memcpy((void*)pList->pulCurrent, (void*)(pTable),
		(BVDC_P_VF_TABLE_SIZE) * sizeof(uint32_t));
	*(pList->pulCurrent + ulVfMiscRegIdx) = ulVfMiscRegValue;     /* override */
	*(pList->pulCurrent + ulVfSTRegIdx) = ulVfSTRegValue;   /* posible ovride */
	pList->pulCurrent += BVDC_P_VF_TABLE_SIZE;

	/* Setup Channel VF filters */
	for (i=0; i < BVDC_P_VEC_CH_NUM; i++)
	{
		BKNI_Memcpy((void*)pList->pulCurrent, apVfFilter[i],
			BVDC_P_CHROMA_TABLE_SIZE * sizeof(uint32_t));
		pList->pulCurrent += BVDC_P_CHROMA_TABLE_SIZE;
	}

	/* BVB_STATUS */
	*pList->pulCurrent++ = 0xff;

	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(PRIM_VF_SHAPER, SAV_REPLICATE, stShaperSettings.ulSavReplicate) |
		BCHP_FIELD_DATA(PRIM_VF_SHAPER, EAV_PREDICT,   stShaperSettings.ulEavPredict) |
		BCHP_FIELD_DATA(PRIM_VF_SHAPER, THRESHOLD,     BVDC_P_VF_THRESH)    |
		BCHP_FIELD_DATA(PRIM_VF_SHAPER, CONTROL,       BVDC_P_VF_ENABLE);

		/* Programming note: ulNumRegs values have been placed in the RUL. The
		 * last BRDC_OP_IMMS_TO_REGS is fulfilled.
		 */

	/* FORMAT_ADDER */
	ulVal = pTable[0];

	/* SD with RGB? */
	if((VIDEO_FORMAT_IS_SD(pNewInfo->pFmtInfo->eVideoFmt)) &&
		(eOutputColorSpace == BVDC_P_Output_eRGB))
	{
		ulVal = BVDC_P_GetFmtAdderValue(pNewInfo);
	}

	/* remove sync on G channel if necessary */
	if(BVDC_P_Display_FindDac(hDisplay, BVDC_DacOutput_eGreen_NoSync) &&
	   (eOutputColorSpace == BVDC_P_Output_eRGB))
	{
		/* remove sync pulses from G channel */
		ulVal &= ~BCHP_MASK(PRIM_VF_FORMAT_ADDER, C0_SYNC);
		BDBG_MSG(("Turn OFF sync on G channel: 0x%x", ulVal));
	}

	if(!pNewInfo->stN0Bits.bPsAgc)
	{
		ulVal &= ~BCHP_MASK(PRIM_VF_FORMAT_ADDER, SECOND_NEGATIVE_SYNC);
	}

#if DCS_SUPPORT
	if (hDisplay->ulNewShadowDCSKeyLow != 0x0)
	{
		ulVal &= ~BCHP_MASK       (PRIM_VF_FORMAT_ADDER, C0_POSITIVESYNC);
		ulVal |=  BCHP_FIELD_DATA (PRIM_VF_FORMAT_ADDER, C0_POSITIVESYNC, 1);
	}
#endif

	/* Edit the VF_FORMAT_ADDER RUL entry */
	*pulFmtAdder = ulVal;

	/* NTSC/PAL could have sync reduction */
	if((VIDEO_FORMAT_IS_SD(pNewInfo->pFmtInfo->eVideoFmt)) &&
		(eOutputColorSpace != BVDC_P_Output_eUnknown))
	{
		/* Note: for RGBHV, need to remove sync signals from G channel; */
		ulVal = (
#if (BVDC_P_SUPPORT_COMPONENT_ONLY)
			(eOutputColorSpace == pNewInfo->eCoOutputColorSpace) &&
#endif
			BVDC_P_Display_FindDac(hDisplay, BVDC_DacOutput_eHsync)) ?
			0 : BVDC_P_GetNegSyncValue(pNewInfo, eOutputColorSpace);

		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
#if BVDC_P_SUPPORT_COMPONENT_ONLY
		*pList->pulCurrent++ = (eOutputColorSpace == pNewInfo->eCoOutputColorSpace)
			? BRDC_REGISTER(BVDC_P_VEC_VF_CO_NEG_SYNC_VALUES)
			: BRDC_REGISTER(BCHP_PRIM_VF_NEG_SYNC_VALUES + hDisplay->lVfOffset);
#else
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_PRIM_VF_NEG_SYNC_VALUES);
#endif
		*pList->pulCurrent++ = ulVal;
	}
	else if((VIDEO_FORMAT_IS_HD(pNewInfo->pFmtInfo->eVideoFmt)) &&
		BVDC_P_Display_FindDac(hDisplay, BVDC_DacOutput_eHsync))
	{
#if BVDC_P_SUPPORT_COMPONENT_ONLY
		if(eOutputColorSpace == pNewInfo->eCoOutputColorSpace)
		{
			*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
			*pList->pulCurrent++ = BRDC_REGISTER(BVDC_P_VEC_VF_CO_NEG_SYNC_VALUES);
			*pList->pulCurrent++ = 0;
		}
#else
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_PRIM_VF_NEG_SYNC_VALUES);
		*pList->pulCurrent++ = 0;
#endif
	}

#if DCS_SUPPORT /** { **/
	/* A new VF table was loaded, so need to override
	 * Pos. sync. values for DCS. */
	if (hDisplay->ulNewShadowDCSKeyLow != 0x0)
	{
		uint32_t ulVal;
		BVDC_P_DisplayInfo *pDispInfo = &hDisplay->stNewInfo;
#if (BVDC_P_SUPPORT_COMPONENT_ONLY) /** { **/
		if (ulFormatAdder == BVDC_P_VEC_VF_CO_FORMAT_ADDER)
		{
			ulVal =
				BVDC_P_GetCoPosSyncValueDcs(
					pDispInfo->pFmtInfo->eVideoFmt,
					pDispInfo->eCoOutputColorSpace,
					hDisplay->ulNewShadowDCSKeyLow);
		}
		else
		{
			ulVal =
				BVDC_P_GetPosSyncValueDcs(
					pDispInfo->pFmtInfo->eVideoFmt,
					pDispInfo->eCoOutputColorSpace,
					hDisplay->ulNewShadowDCSKeyLow);
		}
#else /** } BVDC_P_SUPPORT_COMPONENT_ONLY { **/
		ulVal =
			BVDC_P_GetPosSyncValueDcs(
				pDispInfo->pFmtInfo->eVideoFmt,
				pDispInfo->eCoOutputColorSpace,
				hDisplay->ulNewShadowDCSKeyLow);
#endif /** } BVDC_P_SUPPORT_COMPONENT_ONLY **/

		/*
		 * Programming note: not a mistake. Each of the above three function
		 * calls needs the same value pDispInfo->eCoOutputColorSpace.
		 */

		/* Update VEC path chosen above */
		*pulPosSync = ulVal;
	}
#endif /** } DCS_SUPPORT **/

	return;
}

 /*************************************************************************
 *  {secret}
 *  BVDC_P_Vec_Build_MISC_isr
 *
 *  Adds Vec MISC and VIDEO_ENC block to RUL
 **************************************************************************/
static void BVDC_P_Vec_Build_MISC_isr
	( BVDC_Display_Handle              hDisplay,
	  BVDC_P_ListInfo                 *pList )
{
	BVDC_P_DisplayInfo  *pCurInfo = &hDisplay->stCurInfo;
	BVDC_P_DisplayInfo  *pNewInfo = &hDisplay->stNewInfo;

	/* Add In/Out Muxes update to RUL. */
	if ((pNewInfo->aulEnableMpaaDeci[BVDC_MpaaDeciIf_eComponent] !=
	     pCurInfo->aulEnableMpaaDeci[BVDC_MpaaDeciIf_eComponent]) ||
		(pNewInfo->aulEnableMpaaDeci[BVDC_MpaaDeciIf_eHdmi] !=
		 pCurInfo->aulEnableMpaaDeci[BVDC_MpaaDeciIf_eHdmi]))
	{

		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ =
			BRDC_REGISTER(BCHP_VIDEO_ENC_PRIM_SRC_SEL + hDisplay->eId * sizeof(uint32_t));

		if (pNewInfo->aulEnableMpaaDeci[BVDC_MpaaDeciIf_eComponent])
		{
			*pList->pulCurrent++ = BCHP_FIELD_ENUM(VIDEO_ENC_PRIM_SRC_SEL, SOURCE, DECIM_SOURCE);
		}
		else
		{
			*pList->pulCurrent++ =
				BCHP_FIELD_DATA(VIDEO_ENC_PRIM_SRC_SEL, SOURCE, hDisplay->hCompositor->eId);
		}

		/* Enable mpaa if either component or hdmi mpaa is selected */
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VIDEO_ENC_DECIM_DECIM_CONTROL);

		if (pNewInfo->aulEnableMpaaDeci[BVDC_MpaaDeciIf_eComponent] ||
		    pNewInfo->aulEnableMpaaDeci[BVDC_MpaaDeciIf_eHdmi])
		{
			/* TODO: For vdec source, passthr_count is equal to num_vbi
			         No vdec in 7400B0 */
			/* DECIMATE_RATIO: The MPAA standard specifies the resolution to be
			 * not greater than 520000 pixels in a frame */
			if((pNewInfo->pFmtInfo->ulWidth * pNewInfo->pFmtInfo->ulHeight)/2 < 520000)
			{
				*pList->pulCurrent++ =
					BCHP_FIELD_DATA(VIDEO_ENC_DECIM_DECIM_CONTROL, PASSTHROUGH_COUNT, 0) |
					BCHP_FIELD_ENUM(VIDEO_ENC_DECIM_DECIM_CONTROL, DECIMATE_RATIO, BY2)  |
					BCHP_FIELD_ENUM(VIDEO_ENC_DECIM_DECIM_CONTROL, DECIMATE_SAMPLING_EN, ON) |
					BCHP_FIELD_ENUM(VIDEO_ENC_DECIM_DECIM_CONTROL, DECIMATE_FILTER_EN, ON);
			}
			else
			{
				*pList->pulCurrent++ =
					BCHP_FIELD_DATA(VIDEO_ENC_DECIM_DECIM_CONTROL, PASSTHROUGH_COUNT, 0) |
					BCHP_FIELD_ENUM(VIDEO_ENC_DECIM_DECIM_CONTROL, DECIMATE_RATIO, BY4)  |
					BCHP_FIELD_ENUM(VIDEO_ENC_DECIM_DECIM_CONTROL, DECIMATE_SAMPLING_EN, ON) |
					BCHP_FIELD_ENUM(VIDEO_ENC_DECIM_DECIM_CONTROL, DECIMATE_FILTER_EN, ON);
			}

			/* Decim controls Mpaa for all Vec outputs */
			*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
			*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VIDEO_ENC_DECIM_SRC_SEL);
			*pList->pulCurrent++ =
				BCHP_FIELD_DATA(VIDEO_ENC_DECIM_SRC_SEL, SOURCE, hDisplay->hCompositor->eId);
		}
		else
		{
			*pList->pulCurrent++ =
				BCHP_FIELD_ENUM(VIDEO_ENC_DECIM_DECIM_CONTROL, DECIMATE_SAMPLING_EN, OFF) |
				BCHP_FIELD_ENUM(VIDEO_ENC_DECIM_DECIM_CONTROL, DECIMATE_FILTER_EN, OFF);

			/* Decim controls Mpaa for all Vec outputs */
			*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
			*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VIDEO_ENC_DECIM_SRC_SEL);
			*pList->pulCurrent++ = BCHP_FIELD_ENUM(VIDEO_ENC_DECIM_SRC_SEL, SOURCE, DISABLE);
		}
	}

#if BVDC_P_SUPPORT_ITU656_OUT
	/* MISC_ITU656_IN_MUX: 656 output */
	if(pNewInfo->bEnable656 != pCurInfo->bEnable656)
	{
		BDBG_MSG(("ITUR-656 is %s on Display[%d].",
			pNewInfo->bEnable656 ? "Enable" : "Disable", hDisplay->eId));

		/* MISC_ITU656_IN_MUX: Default for Vec misc regs */
		BVDC_P_DISP_GET_REG_DATA(MISC_ITU656_MASTER_SEL) &= ~(
			BCHP_MASK(MISC_ITU656_MASTER_SEL, FREERUN     ) |
			BCHP_MASK(MISC_ITU656_MASTER_SEL, SELECT     ));

		if((BVDC_P_eVecPrimary   == pNewInfo->e656Vecpath) ||
		   (BVDC_P_eVecSecondary == pNewInfo->e656Vecpath) ||
		   (BVDC_P_eVecTertiary  == pNewInfo->e656Vecpath))
		{
			BVDC_P_DISP_GET_REG_DATA(MISC_ITU656_MASTER_SEL) |=  (
				BCHP_FIELD_DATA(MISC_ITU656_MASTER_SEL, FREERUN, 0) |
				BCHP_FIELD_DATA(MISC_ITU656_MASTER_SEL, SELECT, pNewInfo->e656Vecpath));
		}
		BVDC_P_DISP_WRITE_TO_RUL(MISC_ITU656_MASTER_SEL, pList->pulCurrent);

		if (!pNewInfo->bEnable656)
		{
			/* Decim controls Mpaa for all Vec outputs */
			*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
			*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VIDEO_ENC_ITU656_SRC_SEL);
			*pList->pulCurrent++ = BCHP_FIELD_ENUM(VIDEO_ENC_ITU656_SRC_SEL, SOURCE, DISABLE);
		}
	}
#endif

#if BVDC_P_SUPPORT_DVI_OUT
	/* MISC_DVI_IN_MUX: Hdmi or Dvi output */
	if((pNewInfo->bSyncOnly != pCurInfo->bSyncOnly) ||
	   (pNewInfo->bEnableHdmi != pCurInfo->bEnableHdmi) ||
	   (pNewInfo->aulEnableMpaaDeci[BVDC_MpaaDeciIf_eHdmi] !=
	    pCurInfo->aulEnableMpaaDeci[BVDC_MpaaDeciIf_eHdmi]))
	{
		BDBG_MSG(("HDMI is %s on Display[%d].",
			pNewInfo->bEnableHdmi ? "Enable" : "Disable", hDisplay->eId));

		/* MISC_DVI_IN_MUX: Default for Vec misc regs */
		BVDC_P_DISP_GET_REG_DATA(MISC_DVI_MASTER_SEL) &= ~(
			BCHP_MASK(MISC_DVI_MASTER_SEL, FREERUN     ) |
			BCHP_MASK(MISC_DVI_MASTER_SEL, SELECT     ));

		BVDC_P_DISP_GET_REG_DATA(MISC_DVI_MASTER_SEL) |=  (
			BCHP_FIELD_DATA(MISC_DVI_MASTER_SEL, SELECT, (pNewInfo->eHdmiVecpath)) |
			BCHP_FIELD_DATA(MISC_DVI_MASTER_SEL, FREERUN,
				(BVDC_DisplayTg_eDviDtg == hDisplay->eMasterTg)));

		BVDC_P_DISP_WRITE_TO_RUL(MISC_DVI_MASTER_SEL, pList->pulCurrent);

		/* Select DVI source */
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VIDEO_ENC_DVI_SRC_SEL);

		if((!pNewInfo->bEnableHdmi) ||
		   ( pNewInfo->bSyncOnly))
		{
			*pList->pulCurrent++ =
				BCHP_FIELD_ENUM(VIDEO_ENC_DVI_SRC_SEL, SOURCE, DISABLE);
		}
		else
		{
			if (pNewInfo->aulEnableMpaaDeci[BVDC_MpaaDeciIf_eHdmi])
			{
				*pList->pulCurrent++ =
					BCHP_FIELD_ENUM(VIDEO_ENC_DVI_SRC_SEL, SOURCE, DECIM_SOURCE);
			}
			else
			{
				*pList->pulCurrent++ =
					BCHP_FIELD_DATA(VIDEO_ENC_DVI_SRC_SEL, SOURCE, hDisplay->hCompositor->eId);
			}
		}
	}
#endif

	BVDC_P_DISP_WRITE_TO_RUL(MISC_OUT_MUX,        pList->pulCurrent);
	BVDC_P_DISP_WRITE_TO_RUL(MISC_OUT_CTRL,       pList->pulCurrent);
#if (BVDC_P_SUPPORT_TDAC_VER == BVDC_P_SUPPORT_TDAC_VER_5)
	BVDC_P_DISP_WRITE_TO_RUL(MISC_DAC1_CTRL_REG,  pList->pulCurrent);
	BVDC_P_DISP_WRITE_TO_RUL(MISC_DAC2_CTRL_REG,  pList->pulCurrent);
#if (BVDC_P_MAX_DACS > 2)
	BVDC_P_DISP_WRITE_TO_RUL(MISC_DAC3_CTRL_REG,  pList->pulCurrent);
#endif
	BVDC_P_DISP_WRITE_TO_RUL(MISC_DAC_BG_CTRL_REG,pList->pulCurrent);
#elif (BVDC_P_SUPPORT_TDAC_VER >= BVDC_P_SUPPORT_TDAC_VER_3)
	BVDC_P_DISP_WRITE_TO_RUL(MISC_TDAC01_CTRL_REG,  pList->pulCurrent);
	BVDC_P_DISP_WRITE_TO_RUL(MISC_TDAC02_CTRL_REG,  pList->pulCurrent);
	BVDC_P_DISP_WRITE_TO_RUL(MISC_TDAC03_CTRL_REG,  pList->pulCurrent);
	BVDC_P_DISP_WRITE_TO_RUL(MISC_TDAC0_BG_CTRL_REG,pList->pulCurrent);
	BVDC_P_DISP_WRITE_TO_RUL(MISC_TDAC11_CTRL_REG,  pList->pulCurrent);
	BVDC_P_DISP_WRITE_TO_RUL(MISC_TDAC12_CTRL_REG,  pList->pulCurrent);
	BVDC_P_DISP_WRITE_TO_RUL(MISC_TDAC13_CTRL_REG,  pList->pulCurrent);
	BVDC_P_DISP_WRITE_TO_RUL(MISC_TDAC1_BG_CTRL_REG,pList->pulCurrent);
#elif (BVDC_P_SUPPORT_TDAC_VER == BVDC_P_SUPPORT_TDAC_VER_2)
	BVDC_P_DISP_WRITE_TO_RUL(MISC_TDAC1_CTRL_REG,	pList->pulCurrent);
	BVDC_P_DISP_WRITE_TO_RUL(MISC_TDAC2_CTRL_REG,	pList->pulCurrent);
	BVDC_P_DISP_WRITE_TO_RUL(MISC_TDAC3_CTRL_REG,	pList->pulCurrent);
	BVDC_P_DISP_WRITE_TO_RUL(MISC_TDAC_BG_CTRL_REG,pList->pulCurrent);
#endif

#if (BVDC_P_SUPPORT_QDAC_VER == BVDC_P_SUPPORT_QDAC_VER_1)
	BVDC_P_DISP_WRITE_TO_RUL(MISC_QDAC1_CTRL_REG, pList->pulCurrent);
	BVDC_P_DISP_WRITE_TO_RUL(MISC_QDAC2_CTRL_REG, pList->pulCurrent);
	BVDC_P_DISP_WRITE_TO_RUL(MISC_QDAC3_CTRL_REG, pList->pulCurrent);
	BVDC_P_DISP_WRITE_TO_RUL(MISC_QDAC4_CTRL_REG, pList->pulCurrent);
	BVDC_P_DISP_WRITE_TO_RUL(MISC_QDAC_BG_CTRL_REG,pList->pulCurrent);
#endif

	return ;
}


#if (BVDC_P_SUPPORT_MODULAR_VEC == 1)
/*************************************************************************
 *  {secret}
 *  BVDC_P_Vec_BuildPSync_isr
 *
 * Read the PSYNC from IT's status and program to PSYNC.
 **************************************************************************/
static void BVDC_P_Vec_BuildPSync_isr
	( BVDC_Display_Handle              hDisplay,
	  BVDC_P_ListInfo                 *pList,
	  uint32_t                         ulVfRegAddr )
{
	/* PSYNC needs to match with the internal MC field polarity.  For the case of
	 * PAL it's crucial because depend on TOP/BOT it will removed the extra line
	 * for WSS, but since interrupt lacency can cause the RUL to programm the
	 * PSYNC with old polarity it will cause the VEC to incorrect removed the
	 * line and resulted in stall.
	 *
	 * Solution here is for RDC to read the polarity status and program it on
	 * the fly.  So even with T/B rul the PSYNC will get programmed correctly. */

	/* Extract out current field id: x {0, 1} */
	*pList->pulCurrent++ = BRDC_OP_REG_TO_VAR(BRDC_Variable_0);
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_PRIM_IT_VEC_CTRL_STAT + hDisplay->lItOffset);

	*pList->pulCurrent++ = BRDC_OP_VAR_AND_IMM_TO_VAR(BRDC_Variable_0, BRDC_Variable_1);
	*pList->pulCurrent++ = BCHP_PRIM_IT_VEC_CTRL_STAT_COMP_ODD_EVEN_MASK;

	*pList->pulCurrent++ = BRDC_OP_VAR_ROR_TO_VAR(
		BRDC_Variable_1, BCHP_PRIM_IT_VEC_CTRL_STAT_COMP_ODD_EVEN_SHIFT, BRDC_Variable_2);

	/* psync = ( x << 1) | ( x ^ 1 ) */
	/* BRDC_Variable_3 = BRDC_Variable_2 ror 31; */
	*pList->pulCurrent++ = BRDC_OP_VAR_ROR_TO_VAR(
		BRDC_Variable_2, 31, BRDC_Variable_3);

	/* BRDC_Variable_1 = BRDC_Variable_2 xor 1 */
	*pList->pulCurrent++ = BRDC_OP_VAR_XOR_IMM_TO_VAR(
		BRDC_Variable_2, BRDC_Variable_1);
	*pList->pulCurrent++ = 1;

	/* BRDC_Variable_2 = BRDC_Variable_3 or BRDC_Variable_1 */
	*pList->pulCurrent++ = BRDC_OP_VAR_OR_VAR_TO_VAR(
		BRDC_Variable_3, BRDC_Variable_1, BRDC_Variable_2);

	*pList->pulCurrent++ = BRDC_OP_VAR_TO_REG(BRDC_Variable_2);
	*pList->pulCurrent++ = BRDC_REGISTER(ulVfRegAddr);

	return;
}
#endif

/*************************************************************************
 *  {secret}
 *	BVDC_P_Vec_BuildVsync_isr
 *
 *	Adds Vec settings required for Vsync updates:
 *	VF_AGC_VALUES (for macrovision)
 *	IT_BVB_CONFIG[PSYNC] or DTG_BVB[PSYNC]
 **************************************************************************/
static void BVDC_P_Vec_BuildVsync_isr
	( BVDC_Display_Handle              hDisplay,
	  BVDC_P_ListInfo                 *pList,
	  BAVC_Polarity                    eFieldPolarity )
{
	uint32_t             ulPicSync;
	uint32_t             ulVbiCtrl;
	BVDC_P_DisplayInfo  *pCurInfo = &hDisplay->stCurInfo;
	BVDC_P_DisplayInfo  *pNewInfo = &hDisplay->stNewInfo;

	/* Compute picture sync */
#if (BVDC_P_SUPPORT_MODULAR_VEC == 1)
	if(BAVC_Polarity_eTopField == eFieldPolarity)
	{
		ulPicSync =
			BCHP_FIELD_ENUM(PRIM_VF_BVB_CONFIG, PSYNC, START_TOP) |
			BCHP_FIELD_ENUM(PRIM_VF_BVB_CONFIG, BVB_DATA , DISP);
	}
	else if(BAVC_Polarity_eBotField == eFieldPolarity)
	{
		ulPicSync =
			BCHP_FIELD_ENUM(PRIM_VF_BVB_CONFIG, PSYNC, START_BOT) |
			BCHP_FIELD_ENUM(PRIM_VF_BVB_CONFIG, BVB_DATA , DISP);
	}
	else
	{
		ulPicSync =
			BCHP_FIELD_ENUM(PRIM_VF_BVB_CONFIG, PSYNC, START_PROG) |
			BCHP_FIELD_ENUM(PRIM_VF_BVB_CONFIG, BVB_DATA , DISP);
	}
#else
	ulPicSync = 0;
#endif

	/* If it has automode then we'd use the automod instead. */
#ifdef BCHP_PRIM_VF_BVB_CONFIG_PSYNC_SEL_AUTO
	ulPicSync |=
		BCHP_FIELD_ENUM(PRIM_VF_BVB_CONFIG, PSYNC_SEL, AUTO);
#endif

	/* program vbi enc control */
	ulVbiCtrl =
		BREG_Read32_isr(
			hDisplay->hVdc->hRegister, hDisplay->ulScratchVbiEncControl);
	if(hDisplay->ulPrevVbiEncControlValue != ulVbiCtrl)
	{
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ =
			BRDC_REGISTER(BCHP_VBI_ENC_PRIM_Control + hDisplay->ulVbiEncOffset);
		*pList->pulCurrent++ = ulVbiCtrl;
		hDisplay->ulPrevVbiEncControlValue = ulVbiCtrl;
	}
#if BVDC_P_SUPPORT_VBI_ENC_656
	ulVbiCtrl =
		BREG_Read32_isr(
			hDisplay->hVdc->hRegister, hDisplay->ulScratchVbiEncControl_656);
	if(hDisplay->ulPrevVbiEncControlValue_656 != ulVbiCtrl)
	{
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ =
			BRDC_REGISTER(BCHP_VBI_ENC_656_Ancil_Control +
			hDisplay->ulVbiEncOffset_656);
		*pList->pulCurrent++ = ulVbiCtrl;
		hDisplay->ulPrevVbiEncControlValue_656 = ulVbiCtrl;
	}
#endif

#if BVDC_P_SUPPORT_ITU656_OUT
	if(pCurInfo->bEnable656)
	{
		/* Setup DTG_DTG_BVB (656 master mode) */
#if (BVDC_P_SUPPORT_MODULAR_VEC == 1)
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_DVF_DVF_BVB_CONFIG);
		*pList->pulCurrent++ = ulPicSync;
#endif  /* modular vec */
	}
#endif

#if BVDC_P_SUPPORT_DVI_OUT
	if(pCurInfo->bEnableHdmi)
	{
	#if BVDC_P_SUPPORT_DVPO /* 3548 */
		uint32_t ulNDiv;
		const BVDC_P_RateInfo *pRmInfo = hDisplay->pDvoRmInfo;
	#endif

		/* Setup DVI_DTG_DTG_BVB (dvo master mode) */
#if (BVDC_P_SUPPORT_MODULAR_VEC == 1)
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_DVI_DVF_DVF_BVB_CONFIG);
		*pList->pulCurrent++ = ulPicSync;
#endif  /* modular vec */

	#if BVDC_P_SUPPORT_DVPO /* clock adjustment */
		/* adjust DVO RM clock if:
		   1. one video window turns on game mode delay control and
		   2. the window is not reconfig;
		 */
		if(hDisplay->hWinGameMode && pRmInfo &&
		   BVDC_P_STATE_IS_ACTIVE(hDisplay->hWinGameMode) &&
		   hDisplay->hWinGameMode->stCurInfo.stGameDelaySetting.bEnable &&
		   BVDC_P_IS_CLEAN(&hDisplay->hWinGameMode->stCurInfo.stDirty))
		{
			#define BVDC_P_GAMEM_MODE_DELAY_STEP         (5000)
			#define BVDC_P_GAMEM_MODE_DVO_ADJ_MAX_STEP      (1)
			/* set rate */
			if(hDisplay->hWinGameMode->bAdjGameModeClock)
			{
				int32_t lAdjCnt;

				if(hDisplay->hWinGameMode->bFastAdjClock)
				{
					lAdjCnt = hDisplay->hWinGameMode->lCurGameModeLag / BVDC_P_GAMEM_MODE_DELAY_STEP +
						((hDisplay->hWinGameMode->lCurGameModeLag > 0) ? 1 : -1);
					lAdjCnt = (lAdjCnt > BVDC_P_GAMEM_MODE_DVO_ADJ_MAX_STEP)?
						BVDC_P_GAMEM_MODE_DVO_ADJ_MAX_STEP : lAdjCnt;
					BDBG_ASSERT(pRmInfo->ulRDiv + lAdjCnt > 0);
				}
				else
				{
					lAdjCnt = (hDisplay->hWinGameMode->lCurGameModeLag > 0)? 1 : -1;
				}

				/* Note: adjust Ndiv will be coarse since it changes VCO clock */
				if(lAdjCnt > 1 || lAdjCnt < -1 ||
					hDisplay->hWinGameMode->bCoarseAdjClock)
				{
					ulNDiv  = (uint32_t)(pRmInfo->ulRDiv + lAdjCnt) * (pRmInfo->ulNDiv / pRmInfo->ulRDiv);
				}
				else
				{
					ulNDiv = pRmInfo->ulNDiv;
				}

				BVDC_P_Vec_BuildPllCfg_isr(pList, hDisplay->hVdc->hRegister,
					&pCurInfo->stDvoCfg, pRmInfo,
					pRmInfo->ulRDiv + lAdjCnt, ulNDiv);

				hDisplay->bRmAdjusted = true;
				BDBG_MSG(("Game mode delay: %d[usec]; to accelarate DVO RM? %d;",
					hDisplay->hWinGameMode->ulCurBufDelay, lAdjCnt));
			}
			else if(!hDisplay->hWinGameMode->bCoarseAdjClock && hDisplay->bRmAdjusted)/* restore */
			{
				BVDC_P_Vec_BuildPllCfg_isr(pList, hDisplay->hVdc->hRegister,
					&pCurInfo->stDvoCfg, pRmInfo,
					pRmInfo->ulRDiv, pRmInfo->ulNDiv);
				hDisplay->bRmAdjusted = false;
			}
		}
		else if(hDisplay->bRmAdjusted)/* restore */
		{
			BVDC_P_Vec_BuildPllCfg_isr(pList, hDisplay->hVdc->hRegister,
				&pCurInfo->stDvoCfg, pRmInfo,
				pRmInfo->ulRDiv, pRmInfo->ulNDiv);
			hDisplay->bRmAdjusted = false;
		}

		/* PR35122, PR53584: Add a LVDS DRESET on LVDS PLL / Rate Manager changes. */
		if(hDisplay->ulKickStartDelay)
		{
			hDisplay->ulKickStartDelay--;
			if(!hDisplay->ulKickStartDelay)
			{
				/* LVDS_PHY_0_LVDS_PLL_RESET_CTL (RW) */
				*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG() ;
				*pList->pulCurrent++ = BRDC_REGISTER(BCHP_LVDS_PHY_0_LVDS_RESET_CTL) ;
				*pList->pulCurrent++ =
					BCHP_FIELD_DATA(LVDS_PHY_0_LVDS_RESET_CTL, LVDS_ARESET, 1) ;

				*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG() ;
				*pList->pulCurrent++ = BRDC_REGISTER(BCHP_LVDS_PHY_0_LVDS_RESET_CTL) ;
				*pList->pulCurrent++ =
					BCHP_FIELD_DATA(LVDS_PHY_0_LVDS_RESET_CTL, LVDS_DRESET, 1) ;

				*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG() ;
				*pList->pulCurrent++ = BRDC_REGISTER(BCHP_LVDS_PHY_0_LVDS_RESET_CTL) ;
				*pList->pulCurrent++ =
					BCHP_FIELD_DATA(LVDS_PHY_0_LVDS_RESET_CTL, LVDS_DRESET, 0) ;

				*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG() ;
				*pList->pulCurrent++ = BRDC_REGISTER(BCHP_LVDS_PHY_0_LVDS_RESET_CTL) ;
				*pList->pulCurrent++ =
					BCHP_FIELD_DATA(LVDS_PHY_0_LVDS_RESET_CTL, LVDS_ARESET, 0) ;
			}
		}
	#endif
	}
#endif

	/* TODO: support slave analog + master digital tg */
	if (!hDisplay->bIsBypass) /* Primary or Secondary path */
	{
		bool bChangeMV = false;

		/* to clear VBI_ENC_XXX_Pass_Through.COUNT */
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VBI_ENC_PRIM_Pass_Through + hDisplay->ulVbiEncOffset);
		*pList->pulCurrent++ = 0;

		/* set up game mode VEC RM */
		if(hDisplay->hWinGameMode && hDisplay->pRmTable &&
		   BVDC_P_STATE_IS_ACTIVE(hDisplay->hWinGameMode) &&
		   hDisplay->hWinGameMode->stCurInfo.stGameDelaySetting.bEnable &&
		   BVDC_P_IS_CLEAN(&hDisplay->hWinGameMode->stCurInfo.stDirty))
		{
			/* set rate */
			if(hDisplay->hWinGameMode->bAdjGameModeClock)
			{
				int32_t lAdj = (int32_t)hDisplay->pRmTable[BVDC_P_DISPLAY_RM_PHASE_INC_REG_OFFSET] /
					(1000 * ((hDisplay->hWinGameMode->lCurGameModeLag>0)? 1 : -1));

				/* PRIM_RM_PHASE_INC (RW) */
				*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
				*pList->pulCurrent++ = BRDC_REGISTER(BCHP_PRIM_RM_PHASE_INC + hDisplay->lRmOffset);
				*pList->pulCurrent++ =
					BCHP_FIELD_DATA(PRIM_RM_PHASE_INC, PHASE_INC,
						hDisplay->pRmTable[BVDC_P_DISPLAY_RM_PHASE_INC_REG_OFFSET] + lAdj);

				hDisplay->bRmAdjusted = true;
				BDBG_MSG(("Game mode delay: %d[usec]; to accelarate VEC%d RM? %d;",
					hDisplay->hWinGameMode->ulCurBufDelay, hDisplay->eVecPath, lAdj));
			}
			else if(hDisplay->bRmAdjusted) /* restore */
			{
				BVDC_P_Display_ResumeRmPhaseInc_isr(hDisplay, pList);
				hDisplay->bRmAdjusted = false;
			}
		}
		else if(hDisplay->bRmAdjusted) /* restore */
		{
			BVDC_P_Display_ResumeRmPhaseInc_isr(hDisplay, pList);
			hDisplay->bRmAdjusted = false;
		}

#if (BVDC_P_SUPPORT_MODULAR_VEC == 1)
		/* Setup IT_BVB_CONFIG */
		if((BAVC_Polarity_eFrame != eFieldPolarity) &&
		   (VIDEO_FORMAT_IS_625_LINES(pCurInfo->pFmtInfo->eVideoFmt)))
		{
			BVDC_P_Vec_BuildPSync_isr(hDisplay, pList, BCHP_PRIM_VF_BVB_CONFIG + hDisplay->lVfOffset);
		}
		else
		{
			*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
			*pList->pulCurrent++ = BRDC_REGISTER(BCHP_PRIM_VF_BVB_CONFIG + hDisplay->lVfOffset);
			*pList->pulCurrent++ = ulPicSync;
		}
#endif

#if BVDC_P_SUPPORT_COMPONENT_ONLY
#if (BVDC_P_SUPPORT_MODULAR_VEC == 1)
		if (BVDC_P_VEC_PATH_SUPPORT_CO(hDisplay->eVecPath))
		{
			if((BAVC_Polarity_eFrame != eFieldPolarity) &&
			   (VIDEO_FORMAT_IS_625_LINES(pCurInfo->pFmtInfo->eVideoFmt)))
			{
				BVDC_P_Vec_BuildPSync_isr(hDisplay, pList, BCHP_SEC_VF_CO_BVB_CONFIG);
			}
			else
			{
				*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
				*pList->pulCurrent++ = BRDC_REGISTER(BCHP_SEC_VF_CO_BVB_CONFIG);
				*pList->pulCurrent++ = ulPicSync;
			}
		}
#endif
#endif

		/* Update VF_POS_SYNC_VALUES for every frame for AGC cycling.
		 * Note: progressive format will not have Bottom Field interrupt. */
		if(pCurInfo->eMacrovisionType != BVDC_MacrovisionType_eNoProtection)
		{
			uint32_t ulVal = BVDC_P_GetPosSyncValue(hDisplay, &pList->pulCurrent,
				hDisplay->eVecPath);

			*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
			*pList->pulCurrent++ =
				BRDC_REGISTER(BCHP_PRIM_VF_POS_SYNC_VALUES + hDisplay->lVfOffset);
			*pList->pulCurrent++ = ulVal;

#if (BVDC_P_SUPPORT_COMPONENT_ONLY)
			/* don't forget SEC_VF_CO! */
			if(pCurInfo->eCoOutputColorSpace != BVDC_P_Output_eUnknown)
			{
				*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
				*pList->pulCurrent++ = BRDC_REGISTER(BVDC_P_VEC_VF_CO_POS_SYNC_VALUES);
				*pList->pulCurrent++ = ulVal;
			}
#endif
		}

		/* Determine if new IT register settings are required */
		if(hDisplay->bMvChange &&
		   (pCurInfo->pFmtInfo->eVideoFmt == pNewInfo->pFmtInfo->eVideoFmt) &&
		   (hDisplay->eItState != BVDC_P_ItState_eSwitchMode) &&
		    hDisplay->bUserAppliedChanges)
		{
			bChangeMV = true;
		}
		else if (hDisplay->iDcsChange == 1)
		{
			bChangeMV = true;
		}

		/* Update some IT registers if MV type changed */
		if (bChangeMV)
		{
			BDBG_MSG(("Disp %d field %d to change mv type: %d -> %d",
				hDisplay->eId, eFieldPolarity,
				pCurInfo->eMacrovisionType, pNewInfo->eMacrovisionType));
			BVDC_P_ChangeMvType(hDisplay, &pList->pulCurrent);
			hDisplay->bMvChange = false;
		}
		hDisplay->iDcsChange = 0;

		/* reload 480i microcode to handle 704-sample vs. 720-sample analog output;
		 * TODO: add PAL trim support; */
		if(pCurInfo->stDirty.stBits.bWidthTrim && VIDEO_FORMAT_IS_NTSC(pCurInfo->pFmtInfo->eVideoFmt) &&
		   (pCurInfo->eMacrovisionType < BVDC_MacrovisionType_eTest01))
		{
			BVDC_P_Display_ShaperSettings stShaper;
			uint32_t        ulVfMiscRegValue;
			const uint32_t *pRamTbl =
				BVDC_P_GetRamTableMv_isr(
					hDisplay->eVecPath, pCurInfo, hDisplay->bArib480p);

			/* Write IT_MICRO_INSTRUCTIONS 0..255 on-the-fly */
			*pList->pulCurrent++ = BRDC_OP_IMMS_TO_REGS(BVDC_P_RAM_TABLE_SIZE);
			*pList->pulCurrent++ = BRDC_REGISTER(BCHP_PRIM_IT_MICRO_INSTRUCTIONi_ARRAY_BASE + hDisplay->lItOffset);

			BKNI_Memcpy((void*)pList->pulCurrent, (void*)pRamTbl,
				BVDC_P_RAM_TABLE_SIZE * sizeof(uint32_t));
			pList->pulCurrent += BVDC_P_RAM_TABLE_SIZE;
			hDisplay->ulCurShadowDCSKeyLow = hDisplay->stNewInfo.ulDCSKeyLow;

			/* adjust BVB_SIZE */
			*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
			*pList->pulCurrent++ = BRDC_REGISTER(BCHP_PRIM_IT_BVB_SIZE + hDisplay->lItOffset);
			*pList->pulCurrent++ =
				BCHP_FIELD_DATA(PRIM_IT_BVB_SIZE, HORIZONTAL, pCurInfo->bWidthTrimmed? 712 : 720) |
				BCHP_FIELD_DATA(PRIM_IT_BVB_SIZE, VERTICAL, pCurInfo->pFmtInfo->ulHeight / BVDC_P_FIELD_PER_FRAME);

			/* get the correct vf left cut */
			BVDC_P_GetVfTable_isr(pCurInfo, BVDC_P_Output_eUnknown, NULL, &stShaper);

			/* set the BVB_SAV_REMOVE to left cut */
#if BVDC_P_PR14712_FIXED
			if(pCurInfo->bWidthTrimmed)
			{
				stShaper.ulSavRemove = 8;
			}
#else
			stShaper.ulSavRemove = 0;
#endif
			ulVfMiscRegValue =
				BCHP_FIELD_DATA(PRIM_VF_MISC, reserved0,      0) |
				BCHP_FIELD_ENUM(PRIM_VF_MISC, VF_ENABLE,      ON) |
				BCHP_FIELD_DATA(PRIM_VF_MISC, SUM_OF_TAPS,    0) |
				BCHP_FIELD_ENUM(PRIM_VF_MISC, UPSAMPLE2X,     ON) |
				BCHP_FIELD_DATA(PRIM_VF_MISC, BVB_SAV_REMOVE, stShaper.ulSavRemove) |
				BCHP_FIELD_DATA(PRIM_VF_MISC, VBI_PREFERRED,  1) |
#ifdef BVDC_P_WSE_VER3
				BCHP_FIELD_ENUM(PRIM_VF_MISC, VBI_ENABLE,    OFF) |
#else
				BCHP_FIELD_ENUM(PRIM_VF_MISC, VBI_ENABLE,     ON) |
#endif
				BCHP_FIELD_ENUM(PRIM_VF_MISC, C2_RAMP,        SYNC_TRANS_1) |
				BCHP_FIELD_ENUM(PRIM_VF_MISC, C1_RAMP,        SYNC_TRANS_1) |
				BCHP_FIELD_ENUM(PRIM_VF_MISC, C0_RAMP,        SYNC_TRANS_0);

		    if ((true  == hDisplay->bArib480p) &&
			    (false == pCurInfo->bWidthTrimmed))
			{
				ulVfMiscRegValue |= (
					BCHP_FIELD_DATA(PRIM_VF_MISC, BVB_LINE_REMOVE_TOP   , 1) |
					BCHP_FIELD_DATA(PRIM_VF_MISC, BVB_LINE_REMOVE_BOTTOM, 1) );
			}

			*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
			*pList->pulCurrent++ = BRDC_REGISTER(BCHP_PRIM_VF_MISC + hDisplay->lVfOffset);
			*pList->pulCurrent++ = ulVfMiscRegValue;

#if BVDC_P_SUPPORT_COMPONENT_ONLY
			/* When using Secondary Vec, must make sure Vec consumes
			 * pixels at the same rate for both SEC_VF and SEC_VF_CO */
			if (BVDC_P_VEC_PATH_SUPPORT_CO(hDisplay->eVecPath))
			{
				*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
				*pList->pulCurrent++ = BRDC_REGISTER(BVDC_P_VEC_VF_CO_MISC);
				*pList->pulCurrent++ = ulVfMiscRegValue;
			}
#endif
			/* clear the duirty bit */
			pCurInfo->stDirty.stBits.bWidthTrim = BVDC_P_CLEAN;
		}
	}

#ifdef BVDC_P_WSE_VER3 /** { **/
	/* Work-around for a hardware bug */
	{
		uint32_t       ulVfMiscRegIdx;
		uint32_t       ulVfMiscRegValue;
		const uint32_t *pTable;
		BVDC_P_Display_ShaperSettings stShaperSettings;

		BVDC_P_GetVfTable_isr(
			pCurInfo, pCurInfo->eOutputColorSpace, &pTable, &stShaperSettings);
		ulVfMiscRegIdx =
			(BCHP_PRIM_VF_MISC - BCHP_PRIM_VF_FORMAT_ADDER) / sizeof(uint32_t);
		ulVfMiscRegValue =
			BVDC_P_Vec_Get_VF_MISC_isr (
				pCurInfo, hDisplay->bArib480p, pTable, &stShaperSettings);
		ulVfMiscRegValue &= ~BCHP_FIELD_ENUM(PRIM_VF_MISC, VBI_ENABLE, ON);
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ =
			BRDC_REGISTER(BCHP_PRIM_VF_MISC + hDisplay->lVfOffset);
		*pList->pulCurrent++ = ulVfMiscRegValue;
		ulVfMiscRegValue |= BCHP_FIELD_ENUM(PRIM_VF_MISC, VBI_ENABLE, ON);
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ =
			BRDC_REGISTER(BCHP_PRIM_VF_MISC + hDisplay->lVfOffset);
		*pList->pulCurrent++ = ulVfMiscRegValue;
#if BVDC_P_SUPPORT_COMPONENT_ONLY /** { **/
		/* When using Secondary Vec, must make sure Vec consumes
		 * pixels at the same rate for both SEC_VF and SEC_VF_CO */
		if ((BVDC_P_VEC_PATH_SUPPORT_CO(hDisplay->eVecPath)) &&
		    (pCurInfo->eCoOutputColorSpace != BVDC_P_Output_eRGB))
		{
			ulVfMiscRegValue &= ~BCHP_FIELD_ENUM(PRIM_VF_MISC, VBI_ENABLE, ON);
			*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
			*pList->pulCurrent++ = BRDC_REGISTER(BVDC_P_VEC_VF_CO_MISC);
			*pList->pulCurrent++ = ulVfMiscRegValue;
			ulVfMiscRegValue |= BCHP_FIELD_ENUM(PRIM_VF_MISC, VBI_ENABLE, ON);
			*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
			*pList->pulCurrent++ = BRDC_REGISTER(BVDC_P_VEC_VF_CO_MISC);
			*pList->pulCurrent++ = ulVfMiscRegValue;
		}
#endif /** } BVDC_P_SUPPORT_COMPONENT_ONLY **/
		if (VIDEO_FORMAT_IS_625_LINES(pCurInfo->pFmtInfo->eVideoFmt))
		{
			pTable = BVDC_P_GetItTableMv_isr(pCurInfo);
			ulVfMiscRegValue = pTable[5];
			ulVfMiscRegValue &=
				~BCHP_MASK       (PRIM_IT_STACK_reg_6_7, REG_7        );
			ulVfMiscRegValue |=
				 BCHP_FIELD_DATA (PRIM_IT_STACK_reg_6_7, REG_7, 0xFFFF);
			*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
			*pList->pulCurrent++ =
				BRDC_REGISTER(BCHP_PRIM_IT_STACK_reg_6_7 + hDisplay->lItOffset);
				*pList->pulCurrent++ = ulVfMiscRegValue;
		}
	}
#endif /** } BVDC_P_WSE_VER3 **/

	/* Display callback installed? */
	if (pCurInfo->pfGenericCallback)
	{
		BVDC_Display_CallbackData *pCbData = &hDisplay->stCallbackData;

		/* Notify external modules of rate manager updated. */
		if((hDisplay->bRateManagerUpdated) &&
		   (hDisplay->stCallbackSettings.stMask.bRateChange))
		{
			pCbData->stMask.bRateChange = 1;
			pCbData->sDisplayInfo = pCurInfo->stRateInfo;
			pCurInfo->pfGenericCallback(pCurInfo->pvGenericParm1,
				pCurInfo->iGenericParm2, (void*)pCbData);
			/* Clear rate change flag */
			hDisplay->bRateManagerUpdated = false;
		}

#if BVDC_P_SUPPORT_CMP_CRC
		if (hDisplay->stCallbackSettings.stMask.bCrc)
		{
			uint32_t ulReg;

			/* Get new CRC YCrCb values */
			ulReg = BREG_Read32_isr(hDisplay->hVdc->hRegister,
			                        BCHP_CMP_0_CRC_Y_STATUS + hDisplay->hCompositor->ulRegOffset);
			pCbData->ulCrcLuma = BVDC_P_GET_FIELD(ulReg, CMP_0_CRC_Y_STATUS, VALUE);

			ulReg = BREG_Read32_isr(hDisplay->hVdc->hRegister,
			                        BCHP_CMP_0_CRC_CR_STATUS + hDisplay->hCompositor->ulRegOffset);
			pCbData->ulCrcCr = BVDC_P_GET_FIELD(ulReg, CMP_0_CRC_CR_STATUS, VALUE);

			ulReg = BREG_Read32_isr(hDisplay->hVdc->hRegister,
			                        BCHP_CMP_0_CRC_CB_STATUS + hDisplay->hCompositor->ulRegOffset);
			pCbData->ulCrcCb= BVDC_P_GET_FIELD(ulReg, CMP_0_CRC_CB_STATUS, VALUE);

			pCbData->eId = hDisplay->eId;
			pCbData->stMask.bCrc = 1;

			/* Callback application with the above data */
			pCurInfo->pfGenericCallback(pCurInfo->pvGenericParm1,
			                            eFieldPolarity, (void*)pCbData);
		}
#endif
		/* Clear mask bits */
		pCbData->stMask.bRateChange = 0;
		pCbData->stMask.bCrc = 0;
	}

	/* Set event now. */
	if((hDisplay->bSetEventPending) &&
	   (BVDC_P_ItState_eActive == hDisplay->eItState))
	{
		BKNI_SetEvent_isr(hDisplay->hAppliedDoneEvent);
		hDisplay->bSetEventPending = false;
	}

	return;
}

/*************************************************************************
 *  {secret}
 *  BVDC_P_Vec_Build_240p_isr
 *
 *  Adds Vec settings required for 240p mode updates:
 **************************************************************************/
static void BVDC_P_Vec_Build_240p_isr
	( BVDC_Display_Handle              hDisplay,
	  BVDC_P_ListInfo                 *pList,
	  const uint32_t                   ul240pMode )
{
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_PRIM_IT_STACK_reg_0_1 + hDisplay->lItOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(PRIM_IT_STACK_reg_0_1, REG_0, ul240pMode);

	return;
}

/*************************************************************************
 *  {secret}
 *  BVDC_P_Display_UpdateTimestamp_isr
 *  Update delta between two displays timestamps; delta>0 needs to speedup; otherwise slowdown;
 **************************************************************************/
static int32_t BVDC_P_Display_UpdateTimestamp_isr
	( BVDC_Display_Handle              hDisplay,
	  BVDC_P_DisplayContext           *pTargetDisplay,
	  BAVC_Polarity                    eFieldPolarity )
{
	int32_t lDeltaTs;
	uint32_t ulTargetTs = pTargetDisplay->ulCurrentTs;
	uint32_t ulVsyncInterval =
		BVDC_P_DISPLAY_ONE_VSYNC_INTERVAL(pTargetDisplay->stCurInfo.pFmtInfo->ulVertFreq);
	uint32_t ulMyVsyncInterval =
		BVDC_P_DISPLAY_ONE_VSYNC_INTERVAL(hDisplay->stCurInfo.pFmtInfo->ulVertFreq);

	/* 0) update the display timestamps & cadence */

	/*  note: my Ts may be triggered (N-1) myVsyncs earlier depending on my alignment
	    period N number! So to get my current Ts, need to add (N-1)* myVsyncIntervals; */
	hDisplay->ulCurrentTs   = BREG_Read32_isr(hDisplay->hVdc->hRegister, hDisplay->ulScratchTsAddr)
		+ (hDisplay->ulTsSamplePeriod - 1) * ulMyVsyncInterval;
	hDisplay->eNextPolarity = eFieldPolarity;

	/* adjust only when delta is within one Vsync interval;
	 * TODO: we assumed timer is 30-bit range, so the addition won't wraparound;
	 *       but if timer is 32-bit, need to handle wraparound case! */
	if((hDisplay->ulCurrentTs + BVDC_P_DISPLAY_ALIGNMENT_THRESHOLD >  ulTargetTs) &&
	   (hDisplay->ulCurrentTs <  ulTargetTs + ulVsyncInterval + BVDC_P_DISPLAY_ALIGNMENT_THRESHOLD))
	{
		/* 1) compute delta between this display and the target display in auto mode; */
		if(BVDC_AlignmentDirection_eAuto == hDisplay->stCurInfo.stAlignCfg.eDirection)
		{
			/* 1.1) interlaced/interlaced case; */
			if(hDisplay->stCurInfo.pFmtInfo->bInterlaced &&
			    pTargetDisplay->stCurInfo.pFmtInfo->bInterlaced)
			{
				/* same polarity */
				if (hDisplay->eNextPolarity == pTargetDisplay->eNextPolarity)
				{
					/* positive delta: speedup; */
					lDeltaTs = hDisplay->ulCurrentTs - ulTargetTs;
				}
				else /* opposite polarity */
				{
					/* negative delta: slow down; */
					lDeltaTs = (int32_t)(hDisplay->ulCurrentTs - ulTargetTs) - (int32_t)ulVsyncInterval;
				}
			}
			/* 1.2) progressive/interlaced case */
			else
			{
				/* accelerate or decelerate whichever converge quicker; */
				if(hDisplay->ulCurrentTs > ulTargetTs + ulVsyncInterval/2)
				{
					/* negative delta: slow down; */
					lDeltaTs =(int32_t)(hDisplay->ulCurrentTs - ulTargetTs) - (int32_t)ulVsyncInterval;
				}
				else
				{
					/* positive delta: speed up; */
					lDeltaTs = (int32_t)(hDisplay->ulCurrentTs - ulTargetTs);
				}
			}
		}
		else /* 2) manual mode */
		{
			lDeltaTs = (hDisplay->stCurInfo.stAlignCfg.eDirection == BVDC_AlignmentDirection_eFaster) ?
				(int32_t)(hDisplay->ulCurrentTs - ulTargetTs) :
				((int32_t)(hDisplay->ulCurrentTs - ulTargetTs) - (int32_t)ulVsyncInterval);
		}

		/* align behind the target */
		lDeltaTs -= BVDC_P_DISPLAY_ALIGNMENT_OFFSET;
	}
	else
	{
		lDeltaTs = 0; /* ignore isr dis-ordering; */
	}

	/*BDBG_MSG(("hDisplay->ulCurrentTs=%d, ulTargetTs=%d", hDisplay->ulCurrentTs,ulTargetTs));*/
	return lDeltaTs;
}

/*************************************************************************
 *  {secret}
 *  BVDC_P_Display_Alignment_isr
 **************************************************************************/
static void BVDC_P_Display_Alignment_isr
	( BVDC_Display_Handle              hDisplay,
	  BVDC_P_DisplayContext           *pTargetDisplay,
	  BVDC_P_ListInfo                 *pList,
	  BAVC_Polarity                    eFieldPolarity )
{
	int32_t lDeltaTs = BVDC_P_Display_UpdateTimestamp_isr(hDisplay, pTargetDisplay, eFieldPolarity);

	/* 1) build RUL to snapshot display timestamp; */
	*pList->pulCurrent++ = BRDC_OP_REG_TO_VAR(BRDC_Variable_0);
	*pList->pulCurrent++ = BRDC_REGISTER(hDisplay->stTimerReg.status);

	*pList->pulCurrent++ = BRDC_OP_IMM_TO_VAR(BRDC_Variable_1);
	*pList->pulCurrent++ = BCHP_TIMER_TIMER0_STAT_COUNTER_VAL_MASK;

	*pList->pulCurrent++ = BRDC_OP_VAR_AND_VAR_TO_VAR(BRDC_Variable_0, BRDC_Variable_1, BRDC_Variable_2);

	*pList->pulCurrent++ = BRDC_OP_VAR_TO_REG(BRDC_Variable_2);
	*pList->pulCurrent++ = BRDC_REGISTER(hDisplay->ulScratchTsAddr);

	/* 2) adjust RM clock for alignment; */
	if(!hDisplay->bIsBypass) /* analog path */
	{
		/* set rate: TODO: when to stop? */
		if ((lDeltaTs > BVDC_P_DISPLAY_ALIGNMENT_THRESHOLD) ||
		    (lDeltaTs < -(int32_t)BVDC_P_DISPLAY_ALIGNMENT_THRESHOLD))
		{
			#define BVDC_P_DISPLAY_ALIGNMENT_STEP_SCALE       (6)  /* 1/6 delta step size */
			/* variable step size */
			int32_t lAdj = (int32_t)(hDisplay->pRmTable[BVDC_P_DISPLAY_RM_PHASE_INC_REG_OFFSET] /
				BVDC_P_DISPLAY_ONE_VSYNC_INTERVAL(hDisplay->stCurInfo.pFmtInfo->ulVertFreq)) * lDeltaTs /
				BVDC_P_DISPLAY_ALIGNMENT_STEP_SCALE;

			/* PRIM_RM_PHASE_INC (RW) */
			*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
			*pList->pulCurrent++ = BRDC_REGISTER(BCHP_PRIM_RM_PHASE_INC + hDisplay->lRmOffset);
			*pList->pulCurrent++ =
				BCHP_FIELD_DATA(PRIM_RM_PHASE_INC, PHASE_INC,
					((int32_t)hDisplay->pRmTable[BVDC_P_DISPLAY_RM_PHASE_INC_REG_OFFSET] + lAdj));
			BDBG_MSG(("Display align delta: %d[cycles]; to accelarate VEC%d RM? %d;",
				lDeltaTs, hDisplay->eVecPath, lAdj));
			hDisplay->bAlignAdjusting = true;
		}
		else if(hDisplay->bAlignAdjusting)
		{
			BVDC_P_Display_ResumeRmPhaseInc_isr(hDisplay, pList);
			hDisplay->bAlignAdjusting = false;
		}
	}

}

/* Due to alignment of MFD, or multiple windows sharing a MPEG src, MAD might
 * have to process extra lines after CMP completed the field. If RDC trigers
 * right after it, MAD has little time to complete the job, and cmd error will
 * occur.  Therefore, we need to delay the RDC triger a little bit. However,
 * for the panel we use in vdc team, the biggest BVDC_P_TRIGGER_DELAY could 3.
 * Number bigger than that leads to (ulTopTrigVal > pFmtInfo->ulScanHeight)
 * and the triger line is forced to 1. */
#define BVDC_P_EARLY_TRIGGER_DELAY               (2)

/*************************************************************************
 *  {secret}
 *  BVDC_P_Vec_BuildRul
 *  Build the necessary Vec blocks.
 **************************************************************************/
void BVDC_P_Vec_BuildRul_isr
	( BVDC_Display_Handle              hDisplay,
	  BVDC_P_ListInfo                 *pList,
	  BAVC_Polarity                    eFieldPolarity )
{
	BVDC_P_Display_DirtyBits *pCurDirty;
	uint32_t             ulTopTrigVal, ulBotTrigVal;
	uint32_t             ulVdecPhase;
	BVDC_PhaseAdjustMode ePhaseAdjustMode;
	BVDC_P_DisplayInfo  *pCurInfo = &hDisplay->stCurInfo;
	BVDC_P_DisplayInfo  *pNewInfo = &hDisplay->stNewInfo;
	uint32_t             ulHeight;
	bool                 b240pMode;
	bool                 bDisplayRestart =
		(hDisplay->eItState == BVDC_P_ItState_eNotActive) ||
		(hDisplay->eItState == BVDC_P_ItState_eSwitchMode);

	/* TODO: Building RUL should be off the current context.  New context
	 * should be use for validations, and once it's passed validation it will
	 * be used.  We'd like to narrow down of accessing the NEW context in
	 * isr context. */

	/* Display dirty bits. */
	pCurDirty = &pCurInfo->stDirty;

	/* Compute number of lines per field/frame base on format! */
	ulHeight = (pNewInfo->pFmtInfo->bInterlaced) ?
		pNewInfo->pFmtInfo->ulHeight / BVDC_P_FIELD_PER_FRAME:
		pNewInfo->pFmtInfo->ulHeight;

	/* Alignment:
	    1) neither displays are in format switch;
	    2) both displays are in the mutual-lockable vertical refresh rate;
	    3) we only support slower slave aligned with faster master now;
	  */
	if(!bDisplayRestart &&
	    hDisplay->pRmTable &&
	    BVDC_P_IS_CLEAN(pCurDirty) &&
	    pCurInfo->hTargetDisplay &&
	    BVDC_P_IS_CLEAN(&pCurInfo->hTargetDisplay->stCurInfo.stDirty) &&
	    (pCurInfo->hTargetDisplay->eItState == BVDC_P_ItState_eActive) &&
	    ((pCurInfo->ulVertFreq == pCurInfo->hTargetDisplay->stCurInfo.ulVertFreq) ||
	     (pCurInfo->ulVertFreq == 2997 && pCurInfo->hTargetDisplay->stCurInfo.ulVertFreq==5994) ||
	     (pCurInfo->ulVertFreq == 2397 && pCurInfo->hTargetDisplay->stCurInfo.ulVertFreq==5994) ||
	     (pCurInfo->ulVertFreq == 2500 && pCurInfo->hTargetDisplay->stCurInfo.ulVertFreq==5000)))
	{
		/* slave->master vsync TS sampling periods for display alignment:
			72->60: sample once every 6 vsyncs; not required yet;
			60->50: sample once every 6 vsyncs; not required yet;
			50->60: sample once every 5 vsyncs; not required yet;
			60->24: sample once every 5 vsyncs; not required yet;
			48->60: sample once every 4 vsyncs; not required yet;
			24->60: sample once every 2 vsyncs;
			60->30: sample once every 2 vsyncs; not required yet;
			50->25: sample once every 2 vsyncs; not required yet;
			30->60: sample once every 1 vsyncs;
			25->50: sample once every 1 vsyncs;
			50->50: sample once every 1 vsyncs;
			60->60: sample once every 1 vsyncs;
			24->24: sample once every 1 vsyncs; not required;
			30->30: sample once every 1 vsyncs; not required;
		 */
		if(pCurInfo->ulVertFreq == 2397 && pCurInfo->hTargetDisplay->stCurInfo.ulVertFreq==5994)
		{
			hDisplay->ulTsSamplePeriod = 2;
		}
		else
		{
			hDisplay->ulTsSamplePeriod = 1;
		}

		/* counting down TS sampling vsyncs */
		if(hDisplay->ulTsSampleCount == 0)
		{
			hDisplay->ulTsSampleCount = hDisplay->ulTsSamplePeriod-1;
			BVDC_P_Display_Alignment_isr(hDisplay, (BVDC_P_DisplayContext*)pCurInfo->hTargetDisplay, pList, eFieldPolarity);
		}
		else
		{
			hDisplay->ulTsSampleCount--;
		}
	}
	else if(hDisplay->ulAlignSlaves) /* update timestamp for target display */
	{
		hDisplay->eNextPolarity = eFieldPolarity;

		hDisplay->ulCurrentTs   = BREG_Read32_isr(hDisplay->hVdc->hRegister, hDisplay->ulScratchTsAddr);

		/* 1) build RUL to snapshot display timestamp; */
		*pList->pulCurrent++ = BRDC_OP_REG_TO_VAR(BRDC_Variable_0);
		*pList->pulCurrent++ = BRDC_REGISTER(hDisplay->stTimerReg.status);

		*pList->pulCurrent++ = BRDC_OP_IMM_TO_VAR(BRDC_Variable_1);
		*pList->pulCurrent++ = BCHP_TIMER_TIMER0_STAT_COUNTER_VAL_MASK;

		*pList->pulCurrent++ = BRDC_OP_VAR_AND_VAR_TO_VAR(BRDC_Variable_0, BRDC_Variable_1, BRDC_Variable_2);

		*pList->pulCurrent++ = BRDC_OP_VAR_TO_REG(BRDC_Variable_2);
		*pList->pulCurrent++ = BRDC_REGISTER(hDisplay->ulScratchTsAddr);
	}
	else if(!hDisplay->bIsBypass && hDisplay->pRmTable && hDisplay->bAlignAdjusting)
	{
		BVDC_P_Display_ResumeRmPhaseInc_isr(hDisplay, pList);
		hDisplay->bAlignAdjusting = false;
	}

	/* Users can switch streams, without notifying us. Need to get the
	 * input color space & frame rate here, Adjusm. */
	BVDC_P_Compositor_GetOutputInfo_isr(hDisplay->hCompositor, &pNewInfo->bFullRate,
		&ePhaseAdjustMode, &ulVdecPhase, &b240pMode);
	BVDC_P_Vec_BuildVecPhase_isr(hDisplay, pList, ePhaseAdjustMode, ulVdecPhase);

	/* Compute Display Trigger Values. Rules of thumb are:
	   1) Trigger must locate in the VBI region to avoid partial picture update and BVN stall;
	   2) Leave as much headroom as possible for RUL to complete execution within VBI
	      region and for video data to pre-fill the BVN pipeline;
	   3) Top trigger fires a top RUL to program BVN for display of a top field or a progressive
	      frame, so top trigger should locate BEFORE the start of a progressive frame or the
	      start of a top field picture;
	   4) Bottom trigger fires a bottom RUL to program BVN for display of a bottom field, so
	      bottom trigger should locate BEFORE the start of an interlaced bottom field picture;
	   Note: DTV custom formats reposition the triggers to the end of active video region
	            to leave more room for long RULs to execute;
	            Settop products need more thorough tests to make sure all display formats
	            ulTopActive values to match microcode implementation; */
#if BVDC_P_SUPPORT_EARLY_TRIGGER
	/* DTV projects come here: position trigger as early as possible; */
	/* for progressive format, this value is the 1st line after the active frame; */
#if BFMT_LEGACY_3DTV_SUPPORT
	if(VIDEO_FORMAT_IS_CUSTOM_1080P3D(pNewInfo->pFmtInfo->eVideoFmt))
	{
		ulTopTrigVal = 500;
		ulBotTrigVal = 1625;
	}
	else
#endif
	{
		ulTopTrigVal = BVDC_P_EARLY_TRIGGER_DELAY + pNewInfo->pFmtInfo->ulTopActive +
			(pNewInfo->pFmtInfo->ulHeight >> pNewInfo->pFmtInfo->bInterlaced);

		/* to avoid trigger value overbound; */
		if(ulTopTrigVal > pNewInfo->pFmtInfo->ulScanHeight)
		{
			ulTopTrigVal = 1;/* line counter starts from 1; */
		}

		/* For interlaced format, end of frame means end of bottom picture; */
		if(pNewInfo->pFmtInfo->bInterlaced)
		{
			/* bottom trigger is after end of top picture; */
			ulBotTrigVal = ulTopTrigVal;
			/* top trigger is after the end of bottom picture; */
			ulTopTrigVal = ((pNewInfo->pFmtInfo->ulScanHeight + 1) / 2) + ulBotTrigVal;

			/* avoid overbound as well; */
			if(ulTopTrigVal > pNewInfo->pFmtInfo->ulScanHeight)
				ulTopTrigVal = 1;
		}
		/* For progressive format, bottom trigger is disabled; */
		else
		{
			ulBotTrigVal = 0;
		}
	}
#else
	ulTopTrigVal = BVDC_P_TRIGGER_LINE;
	ulBotTrigVal = (pNewInfo->pFmtInfo->bInterlaced)
		? (((pNewInfo->pFmtInfo->ulScanHeight + 1) / 2) + BVDC_P_TRIGGER_LINE) : 0;
#endif

	/* Setup RM block:
	 * (1) must be !bypass, or master timing generator.
	 * AND
	 * (a) eFramerate changes
	 * (b) eVideoFmt, eTimebase, RateChange changes; on applychanges()
	 * (c) bDisplayRestart
	 * (d) missed rul */

	/* Notify external modules that Rate Manager has been changed. */
	if(hDisplay->bRateManagerUpdatedPending && pList->bLastExecuted)
	{
		hDisplay->bRateManagerUpdatedPending = false;
		hDisplay->bRateManagerUpdated = true;
	}

	if(((!hDisplay->bIsBypass) ||
	    (BVDC_DisplayTg_eDviDtg == hDisplay->eMasterTg)) && /* (1) */
	   ((pCurInfo->bFullRate != pNewInfo->bFullRate) ||   /* (a) */
	   (((pCurInfo->stDirty.stBits.bTiming) ||
	     (pCurInfo->stDirty.stBits.bRateChange) ||
	     (pCurInfo->eTimeBase != pNewInfo->eTimeBase)) &&
	    (hDisplay->bUserAppliedChanges)) ||                 /* (b) */
	   (bDisplayRestart) ||                                 /* (c) */
	   (!pList->bLastExecuted)))                            /* (d) */
	{
		/* get the correct rm table and update RateInfo */
		BVDC_P_GetRmTable_isr (pNewInfo, pNewInfo->pFmtInfo, &hDisplay->pRmTable, pNewInfo->bFullRate, &pNewInfo->stRateInfo);
		pNewInfo->ulVertFreq = pNewInfo->stRateInfo.ulVertRefreshRate;

		if((pCurInfo->stRateInfo.ulPixelClkRate !=
		    pNewInfo->stRateInfo.ulPixelClkRate) ||
		   (pCurInfo->stRateInfo.ulVertRefreshRate !=
		    pNewInfo->stRateInfo.ulVertRefreshRate) ||
		   (pCurInfo->eTimeBase != pNewInfo->eTimeBase))
		{
			pCurInfo->stRateInfo = pNewInfo->stRateInfo;
			hDisplay->bRateManagerUpdatedPending = true;
		}

		/* Note: when switching mode, the IT block needs to be reset
		 * before anything else! */
		if(!hDisplay->bIsBypass)
		{
			BVDC_P_Vec_Build_RM_isr(hDisplay, hDisplay->pRmTable, pList);
			BDBG_MSG(("disp[%d] - Build BVDC_P_Vec_Build_RM_isr", hDisplay->eId));
			BDBG_MSG(("\tVEC's RM PxlClk = %sMHz, RefRate = %d (1/%dth Hz), MultiRate=%d",
				BVDC_P_GetRmString_isr(pNewInfo, pNewInfo->pFmtInfo), pNewInfo->ulVertFreq, BFMT_FREQ_FACTOR,
				pNewInfo->bMultiRateAllow));
		}

		/* Build HDMI Rate manager as well. */
		if(pNewInfo->bEnableHdmi)
		{
			if((bDisplayRestart) ||
			   (hDisplay->bRateManagerUpdatedPending))
			{
				BVDC_P_Vec_Build_HdmiRM_isr(hDisplay, pList);
				BDBG_MSG(("disp[%d] - Build BVDC_P_Vec_Build_HdmiRM_isr", hDisplay->eId));
				BDBG_MSG(("\tHDMI/LVDS's RM PxlClk = %sMHz, RefRate = %d (1/%dth Hz)",
					hDisplay->pDvoRmInfo->pchRate, pNewInfo->ulVertFreq, BFMT_FREQ_FACTOR));
			}
		}

		/* Update frame rate! */
		pCurInfo->bFullRate = pNewInfo->bFullRate;
	}

	/* Setup CSC, SRC, SM blocks.
	 * Note, when source dynamically changes color space, vec needs to load different
	 * csc matrix to compensate the compositor csc truncation errors;
	 * ASSUME: non-bypass display's input color space doesn't change. */
	if (((bDisplayRestart) ||
	     ((pCurInfo->eCmpColorSpace != pNewInfo->eCmpColorSpace) &&
		  /* PR51169: fix for when both input colorspace and format changed but dac not yet
		     updated through applychanges() */
		  (pCurInfo->pFmtInfo->eVideoFmt == pNewInfo->pFmtInfo->eVideoFmt)) ||
		 (pCurInfo->stDirty.stBits.bOutputMute) ||
	     (hDisplay->bUserAppliedChanges)) &&
	    (!hDisplay->bIsBypass))
	{
		BDBG_MSG(("disp[%d] - Build BVDC_P_Vec_Build_CSC_SRC_SM_isr", hDisplay->eId));
		BVDC_P_Vec_Build_CSC_SRC_SM_isr(hDisplay, pList);
	}

	if((hDisplay->bUserAppliedChanges) || bDisplayRestart)
	{
		/* Build MISC block */
		BDBG_MSG(("disp[%d] - Build BVDC_P_Vec_Build_MISC_isr", hDisplay->eId));
		BVDC_P_Vec_Build_MISC_isr(hDisplay, pList);
	}

#if BVDC_P_SUPPORT_ITU656_OUT
	if(pNewInfo->bEnable656)
	{
		/* re-program 656 DTG/DVF blocks if format switch */
		if((hDisplay->bUserAppliedChanges) || bDisplayRestart)
		{
			BDBG_MSG(("disp[%d] - Build BVDC_P_Vec_Build_656_isr", hDisplay->eId));
			BVDC_P_Vec_Build_656_isr(hDisplay, pList, ulHeight,
				ulTopTrigVal, ulBotTrigVal);
		}

		/* reload 656 csc matrix if source dynamic change, or format switch; */
		if((pCurInfo->eCmpColorSpace != pNewInfo->eCmpColorSpace) ||
		   (pCurInfo->stDirty.stBits.bOutputMute) ||
		   bDisplayRestart)
		{
			BDBG_MSG(("disp[%d] - Build BVDC_P_Vec_Build_656_CSC_isr", hDisplay->eId));
			BVDC_P_Vec_Build_656_CSC_isr(hDisplay, pList);
		}
	}
#endif

#if BVDC_P_SUPPORT_DVI_OUT
	if(pNewInfo->bEnableHdmi)
	{
		/* reload HDMI/DVO csc matrix if source dynamic change, user change or
		 * format switch;  Or custom matrix. */
		if((bDisplayRestart) ||
		   (pCurInfo->stDirty.stBits.bCscAdjust) ||
		   (pCurInfo->stDirty.stBits.bOutputMute) ||
		   (pCurInfo->eCmpColorSpace != pNewInfo->eCmpColorSpace))
		{
			BDBG_MSG(("disp[%d] - Build BVDC_P_Vec_Build_DVI_CSC_isr", hDisplay->eId));
			BVDC_P_Vec_Build_DVI_CSC_isr(hDisplay, pList);
		}
	}
#endif

	/* save the dynamic changed new input color space after all the CSC matrices are updated; */
	pCurInfo->eCmpColorSpace = pNewInfo->eCmpColorSpace;

	/* Do we need to build the other blocks?
	 * 1) ApplyChanges
	 * 2) Initial state, Vec is not active */
 	if(hDisplay->bUserAppliedChanges || bDisplayRestart)
	{
#if BVDC_P_SUPPORT_DVI_OUT
		/* Setup DVI blocks */
		if(pNewInfo->bEnableHdmi)
		{
			if(bDisplayRestart)
			{
				BDBG_MSG(("disp[%d] - Build BVDC_P_Vec_Build_DVI_isr", hDisplay->eId));
				BVDC_P_Vec_Build_DVI_isr(hDisplay, pList, ulHeight, ulTopTrigVal, ulBotTrigVal);
			}
			BDBG_MSG(("disp[%d] - Build BVDC_P_Vec_Build_DVF_Color_isr", hDisplay->eId));

#if (!BVDC_P_VEC_HAS_2_DIFFERENT_DVF)
			BVDC_P_Vec_Build_DVF_Color_isr(hDisplay, pList, BCHP_DVI_DVF_DVF_VALUES);
#else
			BVDC_P_Vec_Build_DVF_Color_isr(hDisplay, pList, BCHP_DVI_DVF_DVI_DVF_VALUES);
#endif
			if(pCurDirty->stBits.bCcbAdjust)
			{
				BVDC_P_Vec_BuildCCBRul_isr(hDisplay, pList);
			}
		}
#endif
		if(hDisplay->bIsBypass)
		{
			goto BVDC_P_Vec_BuildRul_isr_UpdateDisplayState;
		}

		/* Setup VF */
		BDBG_MSG(("disp[%d] - Build BVDC_P_Vec_Build_VF_isr", hDisplay->eId));
		BVDC_P_Vec_Build_VF_isr(hDisplay, pList,
			BCHP_PRIM_VF_FORMAT_ADDER + hDisplay->lVfOffset,
			pNewInfo->eOutputColorSpace);

#if BVDC_P_SUPPORT_COMPONENT_ONLY
		/* Setup SEC_VF_CO */
		if (BVDC_P_VEC_PATH_SUPPORT_CO(hDisplay->eVecPath))
		{
			BDBG_MSG(("disp[%d] - Build BVDC_P_Vec_Build_VF_CO_isr", hDisplay->eId));
			BVDC_P_Vec_Build_VF_isr(hDisplay, pList,
			BVDC_P_VEC_VF_CO_FORMAT_ADDER,
				pNewInfo->eCoOutputColorSpace);
		}
#endif

		/* MUST be last!! */
		/* Switch modes if mode changes or for the first time */
		if(bDisplayRestart)
		{
			/* Reset/program IT block. Required for new format switch */
			BDBG_MSG(("disp[%d] - Build BVDC_P_Vec_Build_IT_isr", hDisplay->eId));
			BVDC_P_Vec_Build_IT_isr(hDisplay, pList, ulHeight,
				ulTopTrigVal, ulBotTrigVal);
		}
		else
		{
			/* Necessary settings for every Vsync to avoid flash */
			BVDC_P_Vec_BuildVsync_isr(hDisplay, pList, eFieldPolarity);
		}

		/* program PCL_0 for separate h/v sync outputs for DTV application. */
		BVDC_P_Vec_Build_SyncPCL_isr(hDisplay, pList);

BVDC_P_Vec_BuildRul_isr_UpdateDisplayState:
		/* We're done with the new changes, reset our flag */
		if(hDisplay->bUserAppliedChanges)
		{
			/* Our new now becomes our current(hw-applied) */
			hDisplay->stCurInfo = hDisplay->stNewInfo;
			hDisplay->bUserAppliedChanges = false;
		}
	}
	else
	{
		/* Necessary settings for every Vsync */
		BVDC_P_Vec_BuildVsync_isr(hDisplay, pList, eFieldPolarity);

		/* Clear dirty bits when done building RUL!
		 * TODO: eventually move all the build rul into this condition. */
		if(BVDC_P_IS_DIRTY(pCurDirty))
		{
			BDBG_MSG(("pCurDirty  = 0x%08x", (*pCurDirty).aulInts[0]));
			BDBG_MSG(("bCscAdjust = %d, bUserCsc = %d",
				pCurDirty->stBits.bCscAdjust, pCurInfo->bUserCsc));
			BDBG_MSG(("bRateChange = %d, bForceFrameDrop = %d",
				pCurDirty->stBits.bRateChange, pCurInfo->bForceFrameDrop));
			BDBG_MSG(("bWidthTrim = %d, bWidthTrimmed = %d",
				pCurDirty->stBits.bWidthTrim, pCurInfo->bWidthTrimmed));
			BVDC_P_CLEAN_ALL_DIRTY(pCurDirty);
		}
	}

	if(b240pMode != hDisplay->b240pMode)
	{
		if(VIDEO_FORMAT_IS_NTSC(pCurInfo->pFmtInfo->eVideoFmt))
		{
			uint32_t ul240pMode = (b240pMode == true) ? 1 : 0;
			BDBG_MSG(("disp[%d] - Build BVDC_P_Vec_Build_240p_isr", hDisplay->eId));
			BVDC_P_Vec_Build_240p_isr(hDisplay, pList, ul240pMode);
		}
		hDisplay->b240pMode = b240pMode;
	}

	return;
}

/***************************************************************************
 *
 * BVDC_Display_GetItUcodeInfo
 * Return info about VEC IT microcode, from actual hardware registers.
 ***************************************************************************/
BERR_Code BVDC_Display_GetItUcodeInfo
	( BVDC_Display_Handle hDisplay,
	  BVDC_Display_ItUcodeInfo* pInfo )
{
	BREG_Handle hReg;
	uint32_t iOffset;

	BDBG_ENTER (BVDC_Display_GetItUcodeInfo);

	if (!hDisplay)
	{
		return BERR_TRACE (BERR_INVALID_PARAMETER);
	}

	iOffset = hDisplay->lItOffset;
	hReg = hDisplay->hVdc->hRegister;

	pInfo->ulAnalogTimestamp =
		BREG_Read32 (
			hReg,
			BCHP_PRIM_IT_MICRO_INSTRUCTIONi_ARRAY_BASE +
				(sizeof(uint32_t)*BVDC_P_RAM_TABLE_TIMESTAMP_IDX) +
				iOffset);
	pInfo->ulAnalogChecksum =
		BREG_Read32 (
			hReg,
			BCHP_PRIM_IT_MICRO_INSTRUCTIONi_ARRAY_BASE +
				(sizeof(uint32_t)*BVDC_P_RAM_TABLE_CHECKSUM_IDX) +
				iOffset);
	pInfo->ulI656Timestamp =
		BREG_Read32 (
			hReg,
			BCHP_DTRAM_DMC_INSTRUCTIONi_ARRAY_BASE +
				(sizeof(uint32_t)*BVDC_P_DTRAM_TABLE_TIMESTAMP_IDX));
	pInfo->ulI656Checksum =
		BREG_Read32 (
			hReg,
			BCHP_DTRAM_DMC_INSTRUCTIONi_ARRAY_BASE +
				(sizeof(uint32_t)*BVDC_P_DTRAM_TABLE_CHECKSUM_IDX));
	pInfo->ulDviTimestamp =
		BREG_Read32 (
			hReg,
			BCHP_DTRAM_DMC_INSTRUCTIONi_ARRAY_BASE +
				(sizeof(uint32_t)*BVDC_P_DTRAM_TABLE_SIZE) +
				(sizeof(uint32_t)*BVDC_P_DTRAM_TABLE_TIMESTAMP_IDX));
	pInfo->ulDviChecksum =
		BREG_Read32 (
			hReg,
			BCHP_DTRAM_DMC_INSTRUCTIONi_ARRAY_BASE +
			(sizeof(uint32_t)*BVDC_P_DTRAM_TABLE_SIZE) +
			(sizeof(uint32_t)*BVDC_P_DTRAM_TABLE_CHECKSUM_IDX));

	BDBG_LEAVE (BVDC_Display_GetItUcodeInfo);
	return BERR_SUCCESS;
}

void BVDC_P_ResetVec
	( BVDC_P_Context                  *pVdc )
{
	uint32_t ulVbiPrimReg;
#if BVDC_P_SUPPORT_SEC_VEC
	uint32_t ulVbiSecReg;
#endif
#if BVDC_P_SUPPORT_TER_VEC
	uint32_t ulVbiTerReg;
#endif
#if (BVDC_P_SUPPORT_VBI_ENC_656)
	uint32_t ulVbiAncilReg;
#endif

#ifdef BCHP_VCXO_0_RM_REG_START
	uint32_t i;
	uint32_t ulVcxoRm0[BVDC_P_VCXO_RM_REG_COUNT];
#endif
#ifdef BCHP_VCXO_1_RM_REG_START
	uint32_t ulVcxoRm1[BVDC_P_VCXO_RM_REG_COUNT];
#endif
#ifdef BCHP_VCXO_2_RM_REG_START
	uint32_t ulVcxoRm2[BVDC_P_VCXO_RM_REG_COUNT];
#endif

	/* prepare for software reset */
	BKNI_EnterCriticalSection();
	/* before reset, get regsiters partially owned by VBI module */
	ulVbiPrimReg  = BREG_Read32(pVdc->hRegister, BCHP_VBI_ENC_PRIM_Control);
#if (BVDC_P_SUPPORT_VBI_ENC_656)
	ulVbiAncilReg = BREG_Read32(pVdc->hRegister, BCHP_VBI_ENC_656_Ancil_Control);
#endif

#if BVDC_P_SUPPORT_SEC_VEC
	ulVbiSecReg   = BREG_Read32(pVdc->hRegister, BCHP_VBI_ENC_SEC_Control);
#endif

#if BVDC_P_SUPPORT_TER_VEC
	ulVbiTerReg   = BREG_Read32(pVdc->hRegister, BCHP_VBI_ENC_TERT_Control);
#endif

	/* save VCXO_RM settings for 740x since it's moved to vec core; */
#ifdef BCHP_VCXO_0_RM_REG_START
	for(i = 0; i < BVDC_P_VCXO_RM_REG_COUNT; i++)
	{
		ulVcxoRm0[i] = BREG_Read32(pVdc->hRegister, BCHP_VCXO_0_RM_REG_START +
			i * sizeof(uint32_t));
	}
#endif

#ifdef BCHP_VCXO_1_RM_REG_START
	for(i = 0; i < BVDC_P_VCXO_RM_REG_COUNT; i++)
	{
		ulVcxoRm1[i] = BREG_Read32(pVdc->hRegister, BCHP_VCXO_1_RM_REG_START +
			i * sizeof(uint32_t));
	}
#endif

#ifdef BCHP_VCXO_2_RM_REG_START
	for(i = 0; i < BVDC_P_VCXO_RM_REG_COUNT; i++)
	{
		ulVcxoRm2[i] = BREG_Read32(pVdc->hRegister, BCHP_VCXO_2_RM_REG_START +
			i * sizeof(uint32_t));
	}
#endif

	/* Software Reset entire VEC block!  This will reset RM  */
	BREG_AtomicUpdate32_isr(pVdc->hRegister, BCHP_SUN_TOP_CTRL_SW_RESET, 0,
		BCHP_FIELD_DATA( SUN_TOP_CTRL_SW_RESET, vec_sw_reset, 1 ));

	BREG_AtomicUpdate32_isr(pVdc->hRegister, BCHP_SUN_TOP_CTRL_SW_RESET,
		BCHP_MASK( SUN_TOP_CTRL_SW_RESET, vec_sw_reset), 0);

	/* PR 9338:
	   The VBI module owns most of the fields from these registers.
		All fields owned by VBI should be restored and the pass_through field
		of PRIM/SEC vbi encoders are set, while 656 vbi encoder's pass_through
		count is cleared. */
	ulVbiPrimReg  |= BCHP_VBI_ENC_PRIM_Control_ENABLE_PASS_THROUGH_MASK;
	BREG_Write32(pVdc->hRegister, BCHP_VBI_ENC_PRIM_Control, ulVbiPrimReg);
	BREG_Write32(pVdc->hRegister, BAVC_VBI_ENC_0_CTRL_SCRATCH, ulVbiPrimReg);
#if (BVDC_P_SUPPORT_VBI_ENC_656)
	ulVbiAncilReg &= ~BCHP_VBI_ENC_656_Ancil_Control_PASS_THROUGH_COUNT_MASK;
	BREG_Write32(pVdc->hRegister, BCHP_VBI_ENC_656_Ancil_Control, ulVbiAncilReg);
	BREG_Write32(pVdc->hRegister, BAVC_VBI_ENC_BP_CTRL_SCRATCH, ulVbiAncilReg);
#endif

#if (BVDC_P_SUPPORT_TER_VEC)
	ulVbiTerReg |= BCHP_VBI_ENC_PRIM_Control_ENABLE_PASS_THROUGH_MASK;
	BREG_Write32(pVdc->hRegister, BCHP_VBI_ENC_TERT_Control, ulVbiTerReg);
	BREG_Write32(pVdc->hRegister, BAVC_VBI_ENC_2_CTRL_SCRATCH, ulVbiTerReg);
#endif

#if (BVDC_P_SUPPORT_SEC_VEC)
	ulVbiSecReg   |= BCHP_VBI_ENC_PRIM_Control_ENABLE_PASS_THROUGH_MASK;
	BREG_Write32(pVdc->hRegister, BCHP_VBI_ENC_SEC_Control, ulVbiSecReg);
	BREG_Write32(pVdc->hRegister, BAVC_VBI_ENC_1_CTRL_SCRATCH, ulVbiSecReg);
#endif

	/* restore VCXO_RM settings for 740x since it's moved to vec core; */
#ifdef BCHP_VCXO_0_RM_REG_START
	for(i = 0; i < BVDC_P_VCXO_RM_REG_COUNT; i++)
	{
		BREG_Write32(pVdc->hRegister, BCHP_VCXO_0_RM_REG_START +
			i * sizeof(uint32_t), ulVcxoRm0[i]);
	}
#endif

#ifdef BCHP_VCXO_1_RM_REG_START
	for(i = 0; i < BVDC_P_VCXO_RM_REG_COUNT; i++)
	{
		BREG_Write32(pVdc->hRegister, BCHP_VCXO_1_RM_REG_START +
			i * sizeof(uint32_t), ulVcxoRm1[i]);
	}
#endif

#ifdef BCHP_VCXO_2_RM_REG_START
	for(i = 0; i < BVDC_P_VCXO_RM_REG_COUNT; i++)
	{
		BREG_Write32(pVdc->hRegister, BCHP_VCXO_2_RM_REG_START +
			i * sizeof(uint32_t), ulVcxoRm2[i]);
	}
#endif
	BKNI_LeaveCriticalSection();

	/* init VEC MISC block registers */
	BVDC_P_Vec_Init_Misc_isr(pVdc);

	return;
}

/*************************************************************************
 *  {secret}
 *	BVDC_P_HdmiRmTableEx_isr
 **************************************************************************/
static const BVDC_P_RateInfo* BVDC_P_HdmiRmTableEx_isr
(
	const BFMT_VideoInfo                *pFmtInfo,
	BAVC_HDMI_PixelRepetition eHdmiPixelRepetition,
	const BAVC_VdcDisplay_Info          *pRateInfo
)
{
	const BVDC_P_RateInfo *pRmInfo;

	BDBG_ENTER(BVDC_P_HdmiRmTableEx_isr);

	/* DVO master mode custom formats */
	if((BFMT_VideoFmt_eCustom0 == pFmtInfo->eVideoFmt) ||
	   (BFMT_VideoFmt_eCustom1 == pFmtInfo->eVideoFmt) ||
	   (BFMT_VideoFmt_eCustom2 == pFmtInfo->eVideoFmt))
	{
		/* If rate table is not present it i's a single rate format.
		 * Assumptions multiple of 24/30hz are capable of multirate capable
		 * display. */
		bool bFullRate = (
			(0 == (pRateInfo->ulVertRefreshRate % (24 * BFMT_FREQ_FACTOR))) ||
			(0 == (pRateInfo->ulVertRefreshRate % (30 * BFMT_FREQ_FACTOR))))
			? true : false;

		/* User must also supply both rates */
		bFullRate &= (
			(pFmtInfo->pCustomInfo->pDvoRmTbl0) &&
			(pFmtInfo->pCustomInfo->pDvoRmTbl1));

		/* Use the detected source rate when applicable. */
		pRmInfo = (bFullRate)
			? pFmtInfo->pCustomInfo->pDvoRmTbl0
			: pFmtInfo->pCustomInfo->pDvoRmTbl1;
	}
	else
	{
		/* Lookup the correct table! */
		pRmInfo =
			BVDC_P_HdmiRmTable_isr(pFmtInfo->eVideoFmt,
				pRateInfo->ulPixelClkRate, 0, eHdmiPixelRepetition);
	}

	BDBG_LEAVE(BVDC_P_HdmiRmTableEx_isr);
	return pRmInfo;
}

/* End of File */
