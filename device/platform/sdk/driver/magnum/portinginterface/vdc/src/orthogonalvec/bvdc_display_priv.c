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
 * $brcm_Revision: Hydra_Software_Devel/188 $
 * $brcm_Date: 8/15/12 6:22p $
 *
 * Module Description:
 *     Private module for setting up the modular Vec
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vdc/src/orthogonalvec/bvdc_display_priv.c $
 * 
 * Hydra_Software_Devel/188   8/15/12 6:22p tdo
 * SW7445-8: Create Appframework emulation environment for 7445 A0
 *
 * Hydra_Software_Devel/187   8/13/12 3:10p yuxiaz
 * SW7425-3626, SW7425-3619: Replace uncache memory access with cache
 * memory access: use the new API names in RDC.
 *
 * Hydra_Software_Devel/186   8/10/12 3:28p pntruong
 * SW7435-276: Updated NTSC/480p to 720x480i/720x480p respectively.
 * Added support 482i/483p with new enums.
 *
 * Hydra_Software_Devel/185   7/24/12 3:02p tdo
 * SW7360-29 : Temporarily turn off DAC auto-detection & calibration until
 * it's fully working for 7360 and 7231
 *
 * Hydra_Software_Devel/184   7/13/12 11:27a tdo
 * SW7563-9: Add support for VDC
 *
 * Hydra_Software_Devel/183   7/10/12 2:34p vanessah
 * SW7425-3364: correct the modular president error
 *
 * Hydra_Software_Devel/182   6/29/12 4:35p vanessah
 * SW7425-3364: remove reference of obsolete ulVertFreqMask
 *
 * Hydra_Software_Devel/181   6/28/12 4:23p darnstein
 * SW7425-3367: fix error in testing for IT_n in use. Affects both DCS and
 * Macrovision.
 *
 * Hydra_Software_Devel/180   6/19/12 4:03p darnstein
 * SW7425-3203: if analog output becomes unused, do not attempt DCS state
 * update. Use same test as Macrovision.
 *
 * Hydra_Software_Devel/179   6/12/12 6:50p darnstein
 * SW7425-3226: fix Coverity errors involving suspicious tests for NULL
 * pointer.
 *
 * Hydra_Software_Devel/178   5/3/12 9:25p vanessah
 * SW7425-2873: transcode path fmt stress test
 *
 * Hydra_Software_Devel/177   3/21/12 1:58p syang
 * SW7425-2699: fix compile error for 7360
 *
 * Hydra_Software_Devel/176   3/21/12 1:39p syang
 * SW7425-2699: put mpaa under the control of vdc resource manager; handle
 * correctly both order of enabling/disabling mpaa and turning on/off
 * hdmi/component output
 *
 * Hydra_Software_Devel/175   3/6/12 6:18p vanessah
 * SW7435-9: change display debug msg
 *
 * Hydra_Software_Devel/174   3/2/12 4:03p pntruong
 * SW7231-584: Added artificial display vsync generation to support dsp
 * transcoding.
 *
 * Hydra_Software_Devel/173   2/23/12 2:29p hongtaoz
 * SW7425-2043: mask STG picture ID change callback if the ID doesn't
 * change;
 *
 * Hydra_Software_Devel/172   2/23/12 10:18a vanessah
 * SW7425-2431: fix custom fmt stress test
 *
 * Hydra_Software_Devel/171   1/23/12 3:31p tdo
 * SW7435-9: Add support for CMP4/5
 *
 * Hydra_Software_Devel/170   1/20/12 6:29p vanessah
 * SW7435-9: add STG2/3 support
 *
 * Hydra_Software_Devel/169   1/19/12 11:31a darnstein
 * SW7125-1124: DCS: control VEC hardware with fewer register writes. Per
 * Nutan and Ali Syed.
 *
 * Hydra_Software_Devel/168   1/17/12 3:36p darnstein
 * SW7125-1124: disable some diagnostics for DCS: RDC scratch variables.
 *
 * Hydra_Software_Devel/167   1/11/12 3:07p darnstein
 * SW7125-1124: merge DCS Hybrid+ to main branch. DCS almost ready for
 * production.
 *
 * Hydra_Software_Devel/SW7125-1124/12   12/21/11 7:31p darnstein
 * SW7125-1124: Fix bug in debugging scratch registers. Implement Nutan's
 * suggestion for DCS access control. Integrate all new microcode just
 * posted today.
 *
 * Hydra_Software_Devel/SW7125-1124/11   12/20/11 6:47p darnstein
 * SW7125-1124: DCS: Simplify logic a bit. Add diagnostics.
 *
 * Hydra_Software_Devel/SW7125-1124/10   12/19/11 4:55p darnstein
 * SW7125-1124: simplify DCS access method. A few bug fixes too.
 *
 * Hydra_Software_Devel/SW7125-1124/9   12/13/11 3:52p darnstein
 * SW7125-1124: merge from main branch.
 *
 * Hydra_Software_Devel/SW7125-1124/8   12/12/11 7:45p darnstein
 * SW7125-1124: DCS Hybrid+ software ready for debugging.
 *
 * Hydra_Software_Devel/SW7125-1124/7   12/9/11 9:43p darnstein
 * SW7125-1124: need to add a dirty bit. Checking in DCS work first.
 *
 * Hydra_Software_Devel/SW7125-1124/6   12/8/11 2:03p darnstein
 * SW7125-1124: fix name of an ISR function.
 *
 * Hydra_Software_Devel/SW7125-1124/5   12/6/11 8:16p darnstein
 * SW7125-1124: Finish DCS code swapping logic.
 *
 * Hydra_Software_Devel/SW7125-1124/4   12/1/11 6:45p darnstein
 * SW7125-1124: change definition of IT_VER to identify early version of
 * IT hardware that has PCL_6, PCL_7, and PCL_8 registers.
 *
 * Hydra_Software_Devel/SW7125-1124/3   12/1/11 4:20p darnstein
 * SW7125-1124: small change for DCS, and merge from main branch.
 *
 * Hydra_Software_Devel/SW7125-1124/1   11/15/11 7:20p darnstein
 * SW7125-1124: need to do a branch merge.
 *
 * Hydra_Software_Devel/165   11/29/11 11:27a vanessah
 * SW7425-1783: add user data support in VDC PI
 *
 * Hydra_Software_Devel/164   10/31/11 4:59p syang
 * SW7425-1429: added cleanup for pCbData->stMask
 *
 * Hydra_Software_Devel/163   10/27/11 4:29p syang
 * SW7425-1429: move per vsync call back from window to display; clean up
 * display callback description/implementation
 *
 * Hydra_Software_Devel/162   10/19/11 1:45p vanessah
 * SW7425-1488: MADR RUL delay for NRT transcode
 *
 * Hydra_Software_Devel/161   10/14/11 2:23p tdo
 * SW7425-1416, SW7358-159: Add feature to control automatic DAC
 * detection.  Default is currently OFF until it's fully functional.
 *
 * Hydra_Software_Devel/160   10/11/11 5:10p tdo
 * SW7358-159: No ouput on composite port for B0.  Properly set DAC
 * registers to support auto detection
 *
 * Hydra_Software_Devel/159   10/11/11 12:18p tdo
 * SW7425-1430: Keep at least one analog chan path per display when analog
 * master and no DACs to drain data.
 *
 * Hydra_Software_Devel/158   8/26/11 5:23p syang
 * SW7425-1170: clean up pixel aspect ratio related code
 *
 * Hydra_Software_Devel/157   8/25/11 3:55p pntruong
 * SW7425-1191: Rollback.  The acquire/release is more efficience to be
 * handle in nexus, where the done event is already known.
 *
 * Hydra_Software_Devel/156   8/25/11 9:22a vanessah
 * SW7425-1191:sg pwr management
 *
 * Hydra_Software_Devel/155   8/24/11 10:29p tdo
 * SW7231-309: Correctly place the HD analog path on the channel pre-
 * allocated even with previously no DACs
 *
 * Hydra_Software_Devel/154   8/23/11 12:15p tdo
 * SW7231-309: Do not need to keep analog resource around when analog
 * master and no DACs and no HDMI slave mode
 *
 * Hydra_Software_Devel/153   8/22/11 12:17p tdo
 * SW7231-309: handling vec resource when analog is in slave mode.
 *
 * Hydra_Software_Devel/152   8/17/11 11:49a tdo
 * SW7425-979: Implement the new trigger
 *
 * Hydra_Software_Devel/151   8/16/11 7:04p vanessah
 * SW7425-922: make 7425 b0 simple_vdc compile
 *
 * Hydra_Software_Devel/150   8/16/11 3:03p vanessah
 * SW7425-1068: MBox information validation
 *
 * Hydra_Software_Devel/149   8/15/11 2:04p vanessah
 * SW7425-1083: get rid of bogus setting
 *
 * Hydra_Software_Devel/148   8/15/11 11:43a vanessah
 * SW7425-1083:enable stg on create display
 *
 * Hydra_Software_Devel/147   8/12/11 5:56p vanessah
 * SW7425-1083: enable STG in master mode
 *
 * Hydra_Software_Devel/146   8/12/11 2:56p vanessah
 * SW7425-1083: enable STG in master mode
 *
 * Hydra_Software_Devel/145   8/5/11 6:17p vanessah
 * SW7425-923: update ViCE2 MBox data structure definition and 2nd STG
 * bring up
 *
 * Hydra_Software_Devel/144   8/4/11 7:21p tdo
 * SW7425-979: Add support for 7425B0 DAC name change
 *
 * Hydra_Software_Devel/143   8/2/11 7:47p vanessah
 * SW7425-465: optimization of delay counter
 *
 * Hydra_Software_Devel/142   7/28/11 2:15p tdo
 * SW7422-194: implement 40nm vDAC scaled voltage swing and auto-detection
 * to reduce power dissipation
 *
 * Hydra_Software_Devel/141   7/22/11 4:56p tdo
 * SW7422-194: implement 40nm vDAC scaled voltage swing and auto-detection
 * to reduce power dissipation
 *
 * Hydra_Software_Devel/140   7/19/11 6:42p vanessah
 * SW7425-923: stg register offset cleanup
 *
 * Hydra_Software_Devel/139   7/18/11 5:33p pntruong
 * SW7231-284, SW7346-295, SW7405-5339: Refactored master framerate
 * tracking, so that it can also handle validation.
 *
 * Hydra_Software_Devel/138   7/18/11 11:26a vanessah
 * SW7425-835: SW7425-923: fix ViCE2 channel id bug + B0 STG
 *
 * Hydra_Software_Devel/137   7/8/11 6:01p albertl
 * SW7408-291: Updated VF filter API to use Dac enums for channel
 * selection.
 *
 * Hydra_Software_Devel/136   7/6/11 6:07p albertl
 * SW7408-291, SW7425-745: Changed VF filter API to also take a selected
 * display output as parameter.  Added selective muting of display
 * outputs using CSC.
 *
 * Hydra_Software_Devel/135   7/6/11 9:59a tdo
 * SW7422-194: Add missing version checking macro
 *
 * Hydra_Software_Devel/134   7/1/11 10:54a tdo
 * SW7422-194: Check back in the implementation for vDAC detection with
 * calibration temporarily disabled for now.
 *
 * Hydra_Software_Devel/133   6/30/11 7:07p vanessah
 * SW7422-194: undo the change temporarily
 *
 * Hydra_Software_Devel/132   6/28/11 11:53a tdo
 * SW7422-194: implement 40nm vDAC scaled voltage swing and auto-detection
 * to reduce power dissipation
 *
 * Hydra_Software_Devel/131   6/28/11 9:16a hongtaoz
 * SW7425-704: remove redundent STG host_arm setting from display vsync
 * isr;
 *
 * Hydra_Software_Devel/130   6/27/11 8:23p pntruong
 * SW7425-785: BVN sent 60Hz frame rate metadata to ViCE2 at start of 480i
 * transcode.  Correctly initialize the default framerate to the main vdc
 * settings.
 *
 * Hydra_Software_Devel/129   6/24/11 3:51p albertl
 * SW7420-1966: Cleaned up and reorganized colorspace enums and indicies
 * in display, vdec, and cmp.
 *
 * Hydra_Software_Devel/128   6/17/11 4:16p vanessah
 * SW7425-686: change display init fmt
 *
 * Hydra_Software_Devel/127   6/17/11 2:33p albertl
 * SW7408-291:  Added interface to set and get VF filter settings.
 *
 * Hydra_Software_Devel/126   6/14/11 4:46p tdo
 * SW7346-237: Fix logic to must select HDSRC for display0 (comp0) and DAC
 * is not SD.
 *
 * Hydra_Software_Devel/125   6/14/11 3:01p tdo
 * SW7346-237: Fix bug in setting bHwBugWorkAround flag to true when
 * display is first created with no DACs.
 *
 * Hydra_Software_Devel/124   6/10/11 5:46p hongtaoz
 * SW7425-704: add NRT transcode support to VDC; added bStallStc flag;
 *
 * Hydra_Software_Devel/123   6/9/11 1:17p tdo
 * SW7405-5339: BVDC_Display_Create doesn't recover from bad parameters.
 * Fix various bugs in error handling cases.
 *
 * Hydra_Software_Devel/122   6/7/11 5:40p tdo
 * SW7346-237: More VEC programming bug fixes
 *
 * Hydra_Software_Devel/121   6/3/11 6:00p tdo
 * SW7346-237: More VEC resource management fine tuning
 *
 * Hydra_Software_Devel/120   6/1/11 12:59p tdo
 * SW7346-237: Using both RGB and CVBS output issue.
 *
 * Hydra_Software_Devel/119   5/27/11 6:38p vanessah
 * SW7425-465: fix for transcoding transition black frame artifact
 *
 * Hydra_Software_Devel/118   4/20/11 3:07p tdo
 * SW7425-365: Remove compiler warnings with new kernel build
 *
 * Hydra_Software_Devel/117   4/19/11 11:59a tdo
 * SW7468-380: Latest VDC code doesn't display hdsd_simul
 *
 * Hydra_Software_Devel/116   3/26/11 12:07a tdo
 * SW7125-248: Enhance analog channel resource management
 *
 * Hydra_Software_Devel/115   3/25/11 2:35p yuxiaz
 * SW7420-1741: Added game mode delay support.
 *
 * Hydra_Software_Devel/114   3/24/11 8:09p tdo
 * SW7420-1638: Assign correct Output CSC for each analog chan
 *
 * Hydra_Software_Devel/113   3/24/11 7:35p tdo
 * SW7340-255: SCART mode requires full VEC hardware reset
 *
 * Hydra_Software_Devel/112   3/24/11 7:29p tdo
 * SW7420-1638: Remove unnecessary checks
 *
 * Hydra_Software_Devel/111   3/21/11 1:42p tdo
 * SW7420-1638: DACs do not get powered down properly with certain call
 * sequences
 *
 * Hydra_Software_Devel/110   3/17/11 6:10p tdo
 * SW7422-391: Incorrect VertRefreshRate return and add support for PAL 60
 * to allow multi rate
 *
 * Hydra_Software_Devel/109   2/28/11 10:15a vanessah
 * SW7422-280: change the default setting
 *
 * Hydra_Software_Devel/108   2/23/11 6:04p vanessah
 * SW7422-280: change default output rate from 60 to 59.94
 *
 * Hydra_Software_Devel/107   1/17/11 1:48p pntruong
 * SW7550-645: Add GRPD support in VEC.
 *
 * Hydra_Software_Devel/106   12/10/10 11:35a vanessah
 * SW7425-50:  STG Display Dynamic Format Switch Issue
 *
 * Hydra_Software_Devel/105   11/29/10 11:22a hongtaoz
 * SW7425-46: corrected STG picture polarity;
 *
 * Hydra_Software_Devel/104   11/18/10 12:32p tdo
 * SW7422-92: Add function to set display orientation.  Add new dirty bit
 * to handle 3D setting
 *
 * Hydra_Software_Devel/103   11/18/10 12:06a hongtaoz
 * SW7422-13, SW7425-29: bring up dtg master mode on/off toggle; make
 * display switch mode more robust; defer DTG reset from disable to
 * enable to avoid timeout; bring up 656out;
 *
 * Hydra_Software_Devel/102   11/15/10 10:27a vanessah
 * SW7550-577:  SCART RGB(YPbPr) and CVBS don't output at the same time.
 *
 * Hydra_Software_Devel/101   11/12/10 12:10p albertl
 * SW7125-364: Fixed BVDC_P_CbIsDirty and added assert to check bitfields
 * in dirty bits fit within union integer representation. Fixed naming of
 * dirty bits.
 *
 * Hydra_Software_Devel/100   11/9/10 3:05p tdo
 * SW7422-12: SW_INIT register doesn't require Atomic access
 *
 * Hydra_Software_Devel/99   10/25/10 11:49a vanessah
 * SW7425-46:  ViCE2 meta data mailbox register
 *
 * Hydra_Software_Devel/98   10/22/10 7:03p vanessah
 * SW7425-46:  ViCE2 meta data mailbox register
 *
 * Hydra_Software_Devel/97   10/2/10 1:32a hongtaoz
 * SW7425-29: adding STG support;
 *
 * Hydra_Software_Devel/96   9/28/10 7:30p tdo
 * SW7550-561: Remove false error message when query for resource
 *
 * Hydra_Software_Devel/95   9/28/10 6:58p tdo
 * SW7420-1096: No RFM output when composite output is enabled
 *
 * Hydra_Software_Devel/94   9/23/10 10:33a vanessah
 * SW7425-29:  STG support for transcoding
 *
 * Hydra_Software_Devel/93   9/16/10 9:51p tdo
 * SW7420-1096: No RFM output when composite output is enabled
 *
 * Hydra_Software_Devel/92   9/13/10 4:57p tdo
 * SW7125-629: Nexus_Display of Phase4.0 can't display to two Composite
 * output ports.
 *
 * Hydra_Software_Devel/91   9/9/10 3:18p rpan
 * SW7550-561: Improved VEC resource management error messages.
 *
 * Hydra_Software_Devel/90   9/9/10 2:51p tdo
 * SW7422-58: Add support for new sw-init method
 *
 * Hydra_Software_Devel/89   8/26/10 5:28p tdo
 * SW7422-57: Add simple vdc support
 *
 * Hydra_Software_Devel/88   8/25/10 4:37p tdo
 * SW7420-978: Add missing init condition
 *
 * Hydra_Software_Devel/87   8/9/10 5:33p tdo
 * SW7125-248: Fix scart output for 7125
 *
 * Hydra_Software_Devel/86   6/18/10 7:44a tdo
 * SW7400-2770: Add multi-frame rate support to SD also.
 *
 * Hydra_Software_Devel/85   6/16/10 6:33p tdo
 * SW7400-2770: Add multi-frame rate support to SD also
 *
 * Hydra_Software_Devel/84   6/7/10 4:46p tdo
 * SW7208-57, SW7125-248: VEC resource management fine tuning
 *
 * Hydra_Software_Devel/83   5/24/10 1:35p rpan
 * SW7208-57: Added VEC modulo count trigger support for DVI master mode.
 *
 * Hydra_Software_Devel/82   5/7/10 7:22p albertl
 * SW7125-364: Changed dirty bits to use union structure to avoid type-pun
 * warnings
 *
 * Hydra_Software_Devel/81   4/19/10 10:27p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic. Move
 * BDBG_OBJECT_ID_DECLARE private header files instead of .c.
 *
 * Hydra_Software_Devel/80   4/7/10 11:40a tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic.  Rename TLA
 *
 * Hydra_Software_Devel/79   4/5/10 4:19p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic
 *
 * Hydra_Software_Devel/78   3/4/10 6:40p tdo
 * SW7420-597: VDCApplyChange Timeout Window0 error.  Remove Dac
 * reprogramming from 704 format change
 *
 * Hydra_Software_Devel/77   2/4/10 4:46p tdo
 * SW7420-568: Reprogram RFM when switching between analog chan 0 and 1
 *
 * Hydra_Software_Devel/76   2/4/10 2:17p tdo
 * SW7420-568: Reprogram RFM if analog channel 1 changes
 *
 * Hydra_Software_Devel/75   1/29/10 8:34p albertl
 * SW7405-3845: Fixed PAL_NC matrices and renamed associated colorspace as
 * YUV_NC.
 *
 * Hydra_Software_Devel/74   1/21/10 3:34p syang
 * SW7550-201: init dac bg and each dac as power down when bvdc is opened;
 * ensure dac bg power down when no dac in the group is used
 *
 * Hydra_Software_Devel/73   1/6/10 3:41p rpan
 * SW7468-64: Consolidate VEC modulo count trigger implementation.
 *
 * Hydra_Software_Devel/72   11/30/09 10:34a rpan
 * SW7468-30: Initial OSCL support.
 *
 * Hydra_Software_Devel/71   11/2/09 4:35p tdo
 * SW7340-71: Flash occurs when Same Format is Set in VDC (toggling 16x9
 * or Auto-Format.)
 *
 * Hydra_Software_Devel/70   10/16/09 5:30p darnstein
 * SW7405-3191: Back out all changes in pixel frequency handling. New
 * design coming soon.
 *
 * Hydra_Software_Devel/69   10/15/09 5:19p darnstein
 * SW7405-3191: Pixel frequency is now defined as both a bitmask, and an
 * enum. The bitmask option is DEPRECATED.
 *
 * Hydra_Software_Devel/68   10/8/09 11:38a tdo
 * SW7340-7: Use #ifdef instead of #if
 *
 * Hydra_Software_Devel/67   10/8/09 11:17a tdo
 * SW7340-7: Cleanup VCXO macro defines
 *
 * Hydra_Software_Devel/66   9/29/09 4:26p tdo
 * SW7125-27: Force CVBS and SVideo Dac to to be Analog Chan 1 for Display
 * 0
 *
 * Hydra_Software_Devel/65   9/24/09 7:25p tdo
 * SW7125-27: Correctly disable Chan1 when VEC resource for Chan1 is
 * released
 *
 * Hydra_Software_Devel/64   9/17/09 6:37p tdo
 * SW7420-347: remove assumption in display code that one vec trigger is
 * next to each other again
 *
 * Hydra_Software_Devel/63   9/17/09 11:15a tdo
 * SW7420-347: Fix display timeout issue
 *
 * Hydra_Software_Devel/62   9/15/09 3:37p tdo
 * SW7420-347: remove assumption in display code that one vec trigger is
 * next to each other
 *
 * Hydra_Software_Devel/61   9/8/09 12:00p rpan
 * SW7420-343: Attempt to fix a coverity FORWARD_NULL pointer complaint.
 *
 * Hydra_Software_Devel/60   8/27/09 4:44p tdo
 * SW7342-27: taking dirty bits into account before clearing apply done
 * event
 *
 * Hydra_Software_Devel/59   8/24/09 3:56p rpan
 * SW7342-27: Make analog channel 1 data structure and programming model
 * consistent with channel 0. Enhance channel 1 handling state machine.
 *
 * Hydra_Software_Devel/58   8/24/09 9:38a tdo
 * SW7342-27: Enhance DAC resource management
 *
 * Hydra_Software_Devel/57   8/21/09 5:29p rpan
 * PR57873: Enhanced analog channel 1 resource management. Take the case
 * that RUL doesn't get executed into consideration.
 *
 * Hydra_Software_Devel/56   8/21/09 2:34p tdo
 * PR57734: Add capability for display to handle DACs re-assignment
 *
 * Hydra_Software_Devel/55   8/13/09 9:49a tdo
 * PR55956: Add support for 7420B0
 *
 * Hydra_Software_Devel/54   8/10/09 3:05p syang
 * PR 55812:  fix NULL hDisplay problem for chips with less than 3 cmps
 *
 * Hydra_Software_Devel/53   6/25/09 12:13p rpan
 * PR56137, PR56138, PR56139, PR56166, PR56167, PR56168: Support for
 * various orthogonal VEC configurations.
 *
 * Hydra_Software_Devel/52   6/18/09 5:56p syang
 * PR 55812: add 7550 support
 *
 * Hydra_Software_Devel/51   6/12/09 4:57p darnstein
 * PR55219: bad merge in previous checkin wiped out Randy's work.
 *
 * Hydra_Software_Devel/50   6/11/09 4:15p darnstein
 * PR55219: fill in some details for 7340 chipset.
 *
 * Hydra_Software_Devel/47   6/1/09 2:13p darnstein
 * PR48884: prevent more crashes when user specifies bad video
 * configuration on VEC. These changes specifically support 7420.
 *
 * Hydra_Software_Devel/46   4/8/09 5:13p darnstein
 * PR51527: when bTiming and bWidthTrim events occur at the same time, get
 * rid of bWidthTrim (redundant).
 *
 * Hydra_Software_Devel/45   3/31/09 3:19p rpan
 * PR53156: Implemented a non-reset version 704<->720 display format
 * switch.
 *
 * Hydra_Software_Devel/44   3/23/09 1:50p darnstein
 * PR51527: change DCS "shadow copy" logic.
 *
 * Hydra_Software_Devel/43   3/17/09 11:52a darnstein
 * PR51527: Late merge of Macrovision software.
 *
 * Hydra_Software_Devel/42   3/16/09 10:55p tdo
 * PR45785, PR45789: Merge from MCVP branch
 *
 * Hydra_Software_Devel/7420_mcvp/4   3/13/09 5:51p darnstein
 * PR51527: Macrovision working now, but it reloads microcode for each
 * change. So more work remains.
 *
 * Hydra_Software_Devel/7420_mcvp/3   3/13/09 4:44p tdo
 * PR53158: Temporarily disable mode switch for 704 source
 *
 * Hydra_Software_Devel/7420_mcvp/2   3/4/09 5:24p tdo
 * PR45785, PR45789: merge from main branch on 3/04/09
 *
 * Hydra_Software_Devel/41   3/4/09 10:59a rpan
 * PR45804: Add BVDC_P_ prefix to display handler table variable name.
 *
 * Hydra_Software_Devel/7420_mcvp/1   3/3/09 7:25p darnstein
 * PR51527: Eliminate unused state variables eVbiPath and eVecPath. The
 * DCS files compile now, although they need a lot of work to acutally
 * function.
 *
 * Hydra_Software_Devel/40   2/13/09 9:39a rpan
 * PR52100: Got the user registered callback mechanism working.
 *
 * Hydra_Software_Devel/39   2/11/09 11:04a tdo
 * PR52001: Check for null pointer to avoid segfault.
 *
 * Hydra_Software_Devel/38   2/10/09 4:47p rpan
 * PR52001: Added 7420 DAC power management.
 *
 * Hydra_Software_Devel/37   2/9/09 3:35p rpan
 * PR45804: Some clean-up work.
 *
 * Hydra_Software_Devel/36   2/6/09 3:55p rpan
 * PR45804: Enabled dither.
 *
 * Hydra_Software_Devel/35   2/6/09 2:52p rpan
 * PR45804: Added HDMI sync-only support.
 *
 * Hydra_Software_Devel/34   2/5/09 3:09p rpan
 * PR45804: Temporarily disable "widthTrim" event handler.
 *
 * Hydra_Software_Devel/33   2/3/09 3:53p rpan
 * PR45804: Added width trimming support.
 *
 * Hydra_Software_Devel/32   1/29/09 4:49p rpan
 * PR45804: Added support for alignment.
 *
 * Hydra_Software_Devel/31   1/26/09 4:57p rpan
 * PR45804:  ITU656 master mode work.
 *
 * Hydra_Software_Devel/30   1/26/09 4:21p rpan
 * PR45804: Initial support for ITU656 output.
 *
 * Hydra_Software_Devel/29   1/21/09 4:01p rpan
 * PR45804: DVI master mode work.
 *
 * Hydra_Software_Devel/28   1/20/09 9:12a rpan
 * PR45804: Initial HDMI support  work.
 *
 * Hydra_Software_Devel/27   1/12/09 4:07p rpan
 * PR45804: Added initial RFM output support.
 *
 * Hydra_Software_Devel/26   1/9/09 2:17p rpan
 * PR45804: Added component path MPAA support.
 *
 * Hydra_Software_Devel/25   1/8/09 2:26p rpan
 * PR50391: Renabled the VEC RUL execution mechanism after fixing a number
 * of RDC related issues.
 *
 * Hydra_Software_Devel/24   12/24/08 10:23a rpan
 * PR50391: Assign triggers according to the IT acquired. We used to hard
 * code that.
 *
 * Hydra_Software_Devel/23   12/22/08 10:33p tdo
 * PR50391: Force Timing dirty when Dac dirty
 *
 * Hydra_Software_Devel/22   12/22/08 10:12p rpan
 * PR45804: Implemented handlers for source stream related events.
 *
 * Hydra_Software_Devel/21   12/22/08 3:19p rpan
 * PR45804: Added support for display callback.
 *
 * Hydra_Software_Devel/20   12/18/08 6:49p rpan
 * PR50391: Changes to make sure format switch RUL can always be executed.
 *
 * Hydra_Software_Devel/19   12/18/08 1:46p rpan
 * PR50391: Add a nop RUL to make RDC sanity check happy.
 *
 * Hydra_Software_Devel/18   12/17/08 9:30p rpan
 * PR50391: Some resource management enhancement.
 *
 * Hydra_Software_Devel/17   12/17/08 7:53p tdo
 * PR50391: Add support for Analog Chan 1
 *
 * Hydra_Software_Devel/16   12/17/08 4:42p rpan
 * PR45804: Remove some debugging changes got accidentally checked in.
 *
 * Hydra_Software_Devel/15   12/17/08 4:38p rpan
 * PR45804: Return valid neg vsync value.
 *
 * Hydra_Software_Devel/14   12/16/08 7:23p tdo
 * PR50391: Basic VDC bringup
 *
 * Hydra_Software_Devel/13   12/15/08 10:23p tdo
 * PR50391: Basic VDC bringup
 *
 * Hydra_Software_Devel/12   12/9/08 10:22p tdo
 * PR45804: Add Dac support
 *
 * Hydra_Software_Devel/11   12/9/08 4:45p rpan
 * PR45804: More orthogonal VEC support.
 *
 * Hydra_Software_Devel/10   12/8/08 5:44p rpan
 * PR45804: Switched to VDC generic resource management lib.
 *
 * Hydra_Software_Devel/9   12/5/08 3:52p rpan
 * PR45804: Cleaned up some tabs in the code.
 *
 * Hydra_Software_Devel/8   12/5/08 2:36p tdo
 * PR45804: Add DACs validation code
 *
 * Hydra_Software_Devel/7   12/4/08 4:46p rpan
 * PR45804: Pass source related info from compositor to VEC in one shot.
 *
 * Hydra_Software_Devel/6   12/4/08 3:46p rpan
 * PR45804: Made changes from source follow the new event driven sheme.
 *
 * Hydra_Software_Devel/5   12/4/08 1:17p rpan
 * PR45804: Display destroy support.
 *
 * Hydra_Software_Devel/4   12/3/08 10:10p rpan
 * PR45804: More orthogonal VEC support.
 *
 * Hydra_Software_Devel/3   12/3/08 4:39p rpan
 * PR45804: Initial effort for the orthogonal VEC support.
 *
 * Hydra_Software_Devel/2   11/24/08 9:37p rpan
 * PR45804: Move VEC reset to display module.
 *
 * Hydra_Software_Devel/1   11/4/08 9:55a tdo
 * PR45804: Implement and Bringup Orthorgonal VEC
 *
 ***************************************************************************/
#include "bstd.h"
#include "bdbg.h"
#include "bfmt.h"

/* Note: Tricky here!  bavc.h needs bchp_gfd_x.h defininitions.
 * The check here is to see if chips has more than one gfx feeder. */
#include "bchp_gfd_0.h"
#include "bchp_gfd_1.h"

#include "bavc.h"
#include "bvdc_compositor_priv.h"
#include "bvdc_displayfmt_priv.h"
#include "bvdc_common_priv.h"

#include "bchp_sun_top_ctrl.h"
#include "bchp_misc.h"
#include "bchp_timer.h"


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

#define HW7425_807_IS_FIXED 0

/***************** RM clock adjustment macroes *************/
#define BVDC_P_DISPLAY_RM_PHASE_INC_REG_OFFSET \
	((BCHP_RM_0_PHASE_INC - BCHP_RM_0_RATE_RATIO) / sizeof(uint32_t))


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
    BDBG_MSG(("Display %d allocates ulRdcVarAddr register 0x%x", eId, pDisplay->ulRdcVarAddr));
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

#if (BVDC_P_STG_RUL_DELAY_WORKAROUND)
	/* Dummy register for RUL delay due to MADR BW issue */
	pDisplay->ulScratchDummyAddr = BRDC_AllocScratchReg(pVdc->hRdc);
	if(pDisplay->ulScratchDummyAddr == 0)
	{
		BDBG_ERR(("Not enough scratch registers for display Timestamp!"));
		BRDC_FreeScratchReg(pDisplay->hVdc->hRdc, pDisplay->ulRdcVarAddr);
		BRDC_FreeScratchReg(pDisplay->hVdc->hRdc, pDisplay->ulScratchTsAddr);
		BKNI_Free((void*)pDisplay);
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}
#endif
	pDisplay->hTimer            = pVdc->hTimer;
	BTMR_GetTimerRegisters(pDisplay->hTimer, &pDisplay->stTimerReg);

	pDisplay->eId               = eId;
	pDisplay->hVdc              = (BVDC_Handle)pVdc;
	pDisplay->bIsBypass         = pVdc->pFeatures->bCmpBIsBypass &&
		(BVDC_DisplayId_eDisplay2 == eId);

	pDisplay->aulMpaaDeciIfPortMask[BVDC_MpaaDeciIf_eHdmi]      = BVDC_Hdmi_0;
	pDisplay->aulMpaaDeciIfPortMask[BVDC_MpaaDeciIf_eComponent] = BVDC_Cmpnt_0;
	pDisplay->stMpaaComp.eState = BVDC_P_DisplayResource_eInactive;
	pDisplay->stMpaaComp.ulHwId = BVDC_P_HW_ID_INVALID;
	pDisplay->stMpaaHdmi.eState = BVDC_P_DisplayResource_eInactive;
	pDisplay->stMpaaHdmi.ulHwId = BVDC_P_HW_ID_INVALID;

#if DCS_SUPPORT /** { **/
#if 0
	BDBG_ASSERT (pDisplay->ulDcsScratch[0] = BRDC_AllocScratchReg(pVdc->hRdc));
	BDBG_ASSERT (pDisplay->ulDcsScratch[1] = BRDC_AllocScratchReg(pVdc->hRdc));
	BDBG_ASSERT (pDisplay->ulDcsScratch[2] = BRDC_AllocScratchReg(pVdc->hRdc));
	BDBG_ASSERT (pDisplay->ulDcsScratch[3] = BRDC_AllocScratchReg(pVdc->hRdc));
	printf ("\nDCS scratch registers for IT_%d: %08x %08x %08x %08x\n\n",
		pDisplay->eId,
		pDisplay->ulDcsScratch[0],
		pDisplay->ulDcsScratch[1],
		pDisplay->ulDcsScratch[2],
		pDisplay->ulDcsScratch[3]);
#endif
#endif /** } DCS_SUPPORT **/

	/* (2) create a AppliedDone event. */
	BKNI_CreateEvent(&pDisplay->hAppliedDoneEvent);

	/* (3) Save hDisplay in hVdc */
	pVdc->ahDisplay[pDisplay->eId] = (BVDC_Display_Handle)pDisplay;

	*phDisplay = (BVDC_Display_Handle)pDisplay;

	/* assert assumption for the shared usage of BVDC_P_GetVecCfgSrc() */
	BDBG_ASSERT(BCHP_VEC_CFG_IT_0_SOURCE_SOURCE_S_0 == BCHP_VEC_CFG_DVI_DTG_0_SOURCE_SOURCE_S_0);
#ifdef BCHP_VEC_CFG_ITU656_DTG_0_SOURCE_SOURCE_S_0
	BDBG_ASSERT(BCHP_VEC_CFG_IT_0_SOURCE_SOURCE_S_0 == BCHP_VEC_CFG_ITU656_DTG_0_SOURCE_SOURCE_S_0);
#endif
	BDBG_ASSERT(BCHP_VEC_CFG_IT_0_SOURCE_SOURCE_S_0 == BCHP_VEC_CFG_DECIM_0_SOURCE_SOURCE_S_0);
	BDBG_ASSERT(BCHP_VEC_CFG_IT_0_SOURCE_SOURCE_S_1 == BCHP_VEC_CFG_DVI_DTG_0_SOURCE_SOURCE_S_1);
#ifdef BCHP_VEC_CFG_ITU656_DTG_0_SOURCE_SOURCE_S_1
	BDBG_ASSERT(BCHP_VEC_CFG_IT_0_SOURCE_SOURCE_S_1 == BCHP_VEC_CFG_ITU656_DTG_0_SOURCE_SOURCE_S_1);
#endif
	BDBG_ASSERT(BCHP_VEC_CFG_IT_0_SOURCE_SOURCE_S_1 == BCHP_VEC_CFG_DECIM_0_SOURCE_SOURCE_S_1);
#if (BVDC_P_CMP_2_MAX_VIDEO_WINDOW_COUNT > 0)
	BDBG_ASSERT(BCHP_VEC_CFG_IT_0_SOURCE_SOURCE_S_2 == BCHP_VEC_CFG_DVI_DTG_0_SOURCE_SOURCE_S_2);
	BDBG_ASSERT(BCHP_VEC_CFG_IT_0_SOURCE_SOURCE_S_2 == BCHP_VEC_CFG_ITU656_DTG_0_SOURCE_SOURCE_S_2);
	BDBG_ASSERT(BCHP_VEC_CFG_IT_0_SOURCE_SOURCE_S_2 == BCHP_VEC_CFG_DECIM_0_SOURCE_SOURCE_S_2);
#endif
#if (BVDC_P_CMP_3_MAX_VIDEO_WINDOW_COUNT > 0)
	BDBG_ASSERT(BCHP_VEC_CFG_IT_0_SOURCE_SOURCE_S_3 == BCHP_VEC_CFG_DVI_DTG_0_SOURCE_SOURCE_S_3);
	BDBG_ASSERT(BCHP_VEC_CFG_IT_0_SOURCE_SOURCE_S_3 == BCHP_VEC_CFG_ITU656_DTG_0_SOURCE_SOURCE_S_3);
	BDBG_ASSERT(BCHP_VEC_CFG_IT_0_SOURCE_SOURCE_S_3 == BCHP_VEC_CFG_DECIM_0_SOURCE_SOURCE_S_3);
#endif
#if (BVDC_P_CMP_4_MAX_VIDEO_WINDOW_COUNT > 0)
	BDBG_ASSERT(BCHP_VEC_CFG_IT_0_SOURCE_SOURCE_S_4 == BCHP_VEC_CFG_DVI_DTG_0_SOURCE_SOURCE_S_4);
	BDBG_ASSERT(BCHP_VEC_CFG_IT_0_SOURCE_SOURCE_S_4 == BCHP_VEC_CFG_ITU656_DTG_0_SOURCE_SOURCE_S_4);
	BDBG_ASSERT(BCHP_VEC_CFG_IT_0_SOURCE_SOURCE_S_4 == BCHP_VEC_CFG_DECIM_0_SOURCE_SOURCE_S_4);
#endif
#if (BVDC_P_CMP_5_MAX_VIDEO_WINDOW_COUNT > 0)
	BDBG_ASSERT(BCHP_VEC_CFG_IT_0_SOURCE_SOURCE_S_5 == BCHP_VEC_CFG_DVI_DTG_0_SOURCE_SOURCE_S_5);
	BDBG_ASSERT(BCHP_VEC_CFG_IT_0_SOURCE_SOURCE_S_5 == BCHP_VEC_CFG_ITU656_DTG_0_SOURCE_SOURCE_S_5);
	BDBG_ASSERT(BCHP_VEC_CFG_IT_0_SOURCE_SOURCE_S_5 == BCHP_VEC_CFG_DECIM_0_SOURCE_SOURCE_S_5);
#endif

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
	BVDC_P_Display_DirtyBits *pNewDirty;
	BVDC_P_CscCoeffs stIdentity = BVDC_P_MAKE_DVO_CSC
		(1.0000, 0.0000, 0.0000, 0.0000,
		 0.0000, 1.0000, 0.0000, 0.0000,
		 0.0000, 0.0000, 1.0000, 0.0000);
	uint32_t  ulLfsrCtrlT0, ulLfsrCtrlT1, ulLfsrCtrlT2, ulLfsrValue;
	BFMT_VideoInfo *pCustomFmtInfo = NULL;

	BDBG_ENTER(BVDC_P_Display_Init);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);

	BKNI_Memset((void*)&hDisplay->stNewInfo, 0x0, sizeof(BVDC_P_DisplayInfo));
	BKNI_Memset((void*)&hDisplay->stCurInfo, 0x0, sizeof(BVDC_P_DisplayInfo));

	pNewDirty = &(hDisplay->stNewInfo.stDirty);
	BDBG_ASSERT(sizeof(pNewDirty->stBits) <= sizeof(pNewDirty->aulInts));

	hDisplay->stNewInfo.bErrorLastSetting = false;

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
	hDisplay->stNewInfo.stDirty.stBits.bTimeBase = BVDC_P_DIRTY;

	/* Default Dacs to unused */
	for(i=0; i < BVDC_P_MAX_DACS; i++)
	{
		hDisplay->stNewInfo.aDacOutput[i] = BVDC_DacOutput_eUnused;
	}
	hDisplay->stNewInfo.stDirty.stBits.bDacSetting = BVDC_P_DIRTY;

#if	(BVDC_P_SUPPORT_IT_VER >= 2)
	hDisplay->stNewInfo.ulTriggerModuloCnt = 1;
#endif

	/* Init video format */
	hDisplay->stNewInfo.pFmtInfo   = BFMT_GetVideoFormatInfoPtr(hDisplay->hVdc->stSettings.eVideoFormat);
	hDisplay->stNewInfo.ulVertFreq = hDisplay->stNewInfo.pFmtInfo->ulVertFreq;
	hDisplay->stNewInfo.ulHeight   = (hDisplay->stNewInfo.pFmtInfo->bInterlaced) ?
		hDisplay->stNewInfo.pFmtInfo->ulHeight / BVDC_P_FIELD_PER_FRAME:
		hDisplay->stNewInfo.pFmtInfo->ulHeight;

	/*Init custom format */
	pCustomFmtInfo = &hDisplay->stNewInfo.stCustomFormatInfo;
	pCustomFmtInfo->eVideoFmt = BFMT_VideoFmt_eCustom2;
	pCustomFmtInfo->ulDigitalWidth  = pCustomFmtInfo->ulScanWidth  = pCustomFmtInfo->ulWidth = 352;
	pCustomFmtInfo->ulDigitalHeight = pCustomFmtInfo->ulScanHeight = pCustomFmtInfo->ulWidth = 288;
	pCustomFmtInfo->ulTopActive = pCustomFmtInfo->ulActiveSpace
		= pCustomFmtInfo->ulTopMaxVbiPassThru = pCustomFmtInfo->ulBotMaxVbiPassThru = 0;
	pCustomFmtInfo->ulVertFreq     = 5000;
	pCustomFmtInfo->ulPxlFreqMask  = BFMT_PXL_27MHz;
	pCustomFmtInfo->bInterlaced    = false,
	pCustomFmtInfo->eAspectRatio   = BFMT_AspectRatio_e4_3,
	pCustomFmtInfo->eOrientation   = BFMT_Orientation_e2D,
	pCustomFmtInfo->ulPxlFreq      = 2700,
	pCustomFmtInfo->pchFormatStr   ="BFMT_VideoFmt_eCustom2";
	pCustomFmtInfo->pCustomInfo    = NULL;
	/* Init display aspect ratio */
	hDisplay->stNewInfo.eAspectRatio = hDisplay->stNewInfo.pFmtInfo->eAspectRatio;
	hDisplay->stNewInfo.stDirty.stBits.bAspRatio = BVDC_P_DIRTY;

	/* Initialize output color space */
	hDisplay->stNewInfo.eAnlg_0_OutputColorSpace = BVDC_P_Output_eUnknown;
	hDisplay->stNewInfo.eAnlg_1_OutputColorSpace = BVDC_P_Output_eUnknown;

	/* Initialize analog dac masks */
	hDisplay->stNewInfo.ulAnlgChan0Mask = 0;
	hDisplay->stNewInfo.ulAnlgChan0Mask = 0;

	/* Initialize output mute flags */
	BKNI_Memset(hDisplay->stNewInfo.abOutputMute, false, sizeof(hDisplay->stNewInfo.abOutputMute));

	/* set default display input color space.
	 * NOTE: primary and secondary displays will always take HD color space input
	 * from compositors; while the bypass display input color space could be SD
	 * or HD depends on the VDEC source format. */
	hDisplay->stNewInfo.eCmpColorSpace = BVDC_P_CmpColorSpace_eHdYCrCb;

	/* Current display rate info, update at least once at initialization */
	hDisplay->stNewInfo.stRateInfo.ulPixelClkRate    = 0;
	hDisplay->stNewInfo.stRateInfo.ulVertRefreshRate = 0;
	hDisplay->stNewInfo.bFullRate =
		BVDC_P_IS_FULL_FRAMRATE(hDisplay->hVdc->stSettings.eDisplayFrameRate);

	/* the bFormat dirty bit covers the above info */
	hDisplay->stNewInfo.stDirty.stBits.bTiming = BVDC_P_DIRTY;

	/* Initialize Rfm output */
	hDisplay->stNewInfo.eRfmOutput = BVDC_RfmOutput_eUnused;

	/* Set Hdmi default */
	hDisplay->stNewInfo.eHdmiOutput = BAVC_MatrixCoefficients_eUnknown;
	hDisplay->stNewInfo.bEnableHdmi = false;

	/* Default Dvo CSC */
	hDisplay->stDvoCscMatrix.ulMin       = 0x0000;
	hDisplay->stDvoCscMatrix.ulMax       = 0x0fff;
	hDisplay->stDvoCscMatrix.stCscCoeffs = stIdentity;
	hDisplay->stNewInfo.bUserCsc         = false;

	/* Dvo CSC adjustment values */
	hDisplay->stNewInfo.lDvoAttenuationR = BMTH_FIX_SIGNED_ITOFIX(1, BVDC_P_CSC_DVO_CX_I_BITS, BVDC_P_CSC_DVO_CX_F_BITS);
	hDisplay->stNewInfo.lDvoAttenuationG = BMTH_FIX_SIGNED_ITOFIX(1, BVDC_P_CSC_DVO_CX_I_BITS, BVDC_P_CSC_DVO_CX_F_BITS);
	hDisplay->stNewInfo.lDvoAttenuationB = BMTH_FIX_SIGNED_ITOFIX(1, BVDC_P_CSC_DVO_CX_I_BITS, BVDC_P_CSC_DVO_CX_F_BITS);
	hDisplay->stNewInfo.lDvoOffsetR      = 0;
	hDisplay->stNewInfo.lDvoOffsetG      = 0;
	hDisplay->stNewInfo.lDvoOffsetB      = 0;

	/* Initialize analog channel ids */
	hDisplay->stAnlgChan_0.ulId          = 0;
	hDisplay->stAnlgChan_1.ulId          = 1;

	/* Initialize STG reg offset   */
	hDisplay->ulStgRegOffset             = 0;

#if BVDC_P_SUPPORT_STG
	hDisplay->stNewInfo.bEnableStg = hDisplay->stStgChan.bEnable;
#if (BVDC_P_SUPPORT_STG > 1)
	hDisplay->ulStgRegOffset = (hDisplay->eMasterTg - BVDC_DisplayTg_eStg0) *
		(BCHP_VIDEO_ENC_STG_1_REG_START - BCHP_VIDEO_ENC_STG_0_REG_START);
#else
	hDisplay->ulStgRegOffset = 0;
#endif
#endif
	/* VF filters */
	BKNI_Memset(hDisplay->stNewInfo.abUserVfFilterCo, false, sizeof(hDisplay->stNewInfo.abUserVfFilterCo));
	BKNI_Memset(hDisplay->stNewInfo.abUserVfFilterCvbs, false, sizeof(hDisplay->stNewInfo.abUserVfFilterCvbs));
	BKNI_Memset(hDisplay->stNewInfo.aaulUserVfFilterCo, 0, sizeof(hDisplay->stNewInfo.aaulUserVfFilterCo));
	BKNI_Memset(hDisplay->stNewInfo.aaulUserVfFilterCvbs, 0, sizeof(hDisplay->stNewInfo.aaulUserVfFilterCvbs));
	BKNI_Memset(hDisplay->stAnlgChan_0.apVfFilter, 0, sizeof(hDisplay->stAnlgChan_0.apVfFilter));
	BKNI_Memset(hDisplay->stAnlgChan_1.apVfFilter, 0, sizeof(hDisplay->stAnlgChan_1.apVfFilter));

	/* Set 656 default */
	hDisplay->stNewInfo.bEnable656 = false;

	/* Callback */
	hDisplay->stNewInfo.pfGenericCallback = NULL;
	hDisplay->stNewInfo.pvGenericParm1    = NULL;
	hDisplay->stNewInfo.iGenericParm2     = 0;
	hDisplay->stNewInfo.stDirty.stBits.bCallback = BVDC_P_DIRTY;

	hDisplay->bSetEventPending  = false;
	BKNI_ResetEvent(hDisplay->hAppliedDoneEvent);

	/* Vec is not alive yet */
	hDisplay->eItState = BVDC_P_ItState_eNotActive;
	hDisplay->eState   = BVDC_P_State_eInactive;

	/* Game mode off */
	hDisplay->hWinGameMode = NULL;
	hDisplay->pRmTable     = NULL;
	hDisplay->bRmAdjusted = false;

	/* alignment off */
	hDisplay->eTimeStampPolarity   = BAVC_Polarity_eTopField;
	hDisplay->stNewInfo.hTargetDisplay = NULL;

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

	/* Same as new */
	hDisplay->stCurInfo = hDisplay->stNewInfo;

	hDisplay->pPrevFmtInfo = BFMT_GetVideoFormatInfoPtr(hDisplay->hVdc->stSettings.eVideoFormat);
	BKNI_Memcpy((uint8_t*)&hDisplay->stPrevCustomFormatInfo,
		(uint8_t*)&hDisplay->stCurInfo.stCustomFormatInfo, sizeof(BFMT_VideoInfo));
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

	/* At this point application should have disabled all the
	 * callbacks &slots */

	/* [3] Remove display handle from main VDC handle */
	hDisplay->hVdc->ahDisplay[hDisplay->eId] = NULL;

	/* [2] Destroy event */
	BKNI_DestroyEvent(hDisplay->hAppliedDoneEvent);

	/* [1] Release RDC scratch register before release context */
	BRDC_FreeScratchReg(hDisplay->hVdc->hRdc, hDisplay->ulScratchTsAddr);
	BRDC_FreeScratchReg(hDisplay->hVdc->hRdc, hDisplay->ulRdcVarAddr);
#if (BVDC_P_STG_RUL_DELAY_WORKAROUND)
	BRDC_FreeScratchReg(hDisplay->hVdc->hRdc, hDisplay->ulScratchDummyAddr);
#endif

#if DCS_SUPPORT
#if 0
	BRDC_FreeScratchReg(hDisplay->hVdc->hRdc, hDisplay->ulDcsScratch[0]);
	BRDC_FreeScratchReg(hDisplay->hVdc->hRdc, hDisplay->ulDcsScratch[1]);
	BRDC_FreeScratchReg(hDisplay->hVdc->hRdc, hDisplay->ulDcsScratch[2]);
#endif
#endif

	BDBG_OBJECT_DESTROY(hDisplay, BVDC_DSP);
	/* [0] Release context in system memory */
	BKNI_Free((void*)hDisplay);

	BDBG_LEAVE(BVDC_P_Display_Destroy);
	return;
}


/*************************************************************************
 *
 */
void BVDC_P_ResetVec
	( BVDC_P_Context                  *pVdc )
{
#if 0
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

	BDBG_ENTER(BVDC_P_ResetVec);

	/* prepare for software reset */
	BKNI_EnterCriticalSection();


#if 0 /* ToDo: Add 7420 VBI reset handling */
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
#endif

	/* Save VCXO_RM settings before reset VEC core */
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

#if BVDC_P_SUPPORT_NEW_SW_INIT
	BREG_Write32(pVdc->hRegister, BCHP_SUN_TOP_CTRL_SW_INIT_0_SET,
	      BCHP_FIELD_DATA( SUN_TOP_CTRL_SW_INIT_0_SET, vec_sw_init, 1 ));
	BREG_Write32(pVdc->hRegister, BCHP_SUN_TOP_CTRL_SW_INIT_0_CLEAR,
	      BCHP_FIELD_DATA( SUN_TOP_CTRL_SW_INIT_0_CLEAR, vec_sw_init, 1 ));
#else
	/* Software Reset entire VEC block!  This will reset RM  */
	BREG_AtomicUpdate32_isr(pVdc->hRegister, BCHP_SUN_TOP_CTRL_SW_RESET, 0,
		BCHP_FIELD_DATA( SUN_TOP_CTRL_SW_RESET, vec_sw_reset, 1 ));

	BREG_AtomicUpdate32_isr(pVdc->hRegister, BCHP_SUN_TOP_CTRL_SW_RESET,
		BCHP_MASK( SUN_TOP_CTRL_SW_RESET, vec_sw_reset), 0);
#endif


#if 0 /* ToDo: Add 7420 VBI reset handling */

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
#endif

	/* Restore VCXO_RM settings */
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

	/* init VEC MISC block registers */
	BVDC_P_Vec_Init_Misc_isr(pVdc);

	BKNI_LeaveCriticalSection();

	BDBG_LEAVE(BVDC_P_ResetVec);
	return ;
}


/*************************************************************************
 *
 */
void BVDC_P_Vec_Init_Misc_isr
	( BVDC_P_Context           *pVdc )
{
	uint32_t ulReg;
	uint32_t ulOffset;
	int i;

	BDBG_ENTER(BVDC_P_Vec_Init_Misc_isr);

#if BVDC_P_SUPPORT_NEW_SW_INIT
	ulReg = BCHP_FIELD_DATA(VEC_CFG_SW_INIT_VEC_MISC, INIT, 1);
	BREG_Write32(pVdc->hRegister, BCHP_VEC_CFG_SW_INIT_VEC_MISC, ulReg);

	ulReg = BCHP_FIELD_DATA(VEC_CFG_SW_INIT_VEC_MISC, INIT, 0);
	BREG_Write32(pVdc->hRegister, BCHP_VEC_CFG_SW_INIT_VEC_MISC, ulReg);
#if (BVDC_P_SUPPORT_TDAC_VER >= BVDC_P_SUPPORT_TDAC_VER_11)
	ulReg = BCHP_FIELD_DATA(VEC_CFG_SW_INIT_CABLE_DETECT_0, INIT, 1);
	BREG_Write32(pVdc->hRegister, BCHP_VEC_CFG_SW_INIT_CABLE_DETECT_0, ulReg);

	ulReg = BCHP_FIELD_DATA(VEC_CFG_SW_INIT_CABLE_DETECT_0, INIT, 0);
	BREG_Write32(pVdc->hRegister, BCHP_VEC_CFG_SW_INIT_CABLE_DETECT_0, ulReg);
#endif
#else
	ulReg = BCHP_FIELD_DATA(VEC_CFG_SW_RESET_VEC_MISC, RESET, 1);
	BREG_Write32(pVdc->hRegister, BCHP_VEC_CFG_SW_RESET_VEC_MISC, ulReg);

	ulReg = BCHP_FIELD_DATA(VEC_CFG_SW_RESET_VEC_MISC, RESET, 0);
	BREG_Write32(pVdc->hRegister, BCHP_VEC_CFG_SW_RESET_VEC_MISC, ulReg);
#endif

#ifdef BCHP_MISC_DAC_BG_CTRL_0
	/* power off band gap*/
	ulReg =
		BCHP_FIELD_ENUM(MISC_DAC_BG_CTRL_0, PWRDN,        PWRDN     ) |
		BCHP_FIELD_ENUM(MISC_DAC_BG_CTRL_0, CORE_ADJ,     FOUR      ) |
		BCHP_FIELD_ENUM(MISC_DAC_BG_CTRL_0, BANDGAP_BYP,  BANDGAP   ) |
		BCHP_FIELD_ENUM(MISC_DAC_BG_CTRL_0, IREF_ADJ,     TWENTY_SIX) |
		BCHP_FIELD_ENUM(MISC_DAC_BG_CTRL_0, PWRDN_REFAMP, POWER_DOWN  );
	BREG_Write32(pVdc->hRegister, BCHP_MISC_DAC_BG_CTRL_0, ulReg);
#endif

#ifdef BCHP_MISC_DAC_BG_CTRL_1
	ulReg =
		BCHP_FIELD_ENUM(MISC_DAC_BG_CTRL_1, PWRDN,        PWRDN     ) |
		BCHP_FIELD_ENUM(MISC_DAC_BG_CTRL_1, CORE_ADJ,     FOUR      ) |
		BCHP_FIELD_ENUM(MISC_DAC_BG_CTRL_1, BANDGAP_BYP,  BANDGAP   ) |
		BCHP_FIELD_ENUM(MISC_DAC_BG_CTRL_1, IREF_ADJ,     TWENTY_SIX) |
		BCHP_FIELD_ENUM(MISC_DAC_BG_CTRL_1, PWRDN_REFAMP, POWER_DOWN  );
	BREG_Write32(pVdc->hRegister, BCHP_MISC_DAC_BG_CTRL_1, ulReg);
#endif

	/* power down each dac */
	for(i = 0; i < BVDC_P_MAX_DACS; i++)
	{
#ifdef BCHP_MISC_DAC_1_CFG
		ulOffset = (BCHP_MISC_DAC_1_CFG - BCHP_MISC_DAC_0_CFG) * i;
#else
		ulOffset= 0;
#endif
		ulReg =
			BCHP_FIELD_DATA(MISC_DAC_0_CFG, CONST,        0) |
			BCHP_FIELD_ENUM(MISC_DAC_0_CFG, SINC,       OFF) |
			BCHP_FIELD_DATA(MISC_DAC_0_CFG, DELAY,        0) |
			BCHP_FIELD_ENUM(MISC_DAC_0_CFG, SEL,      CONST);
		BREG_Write32(pVdc->hRegister, BCHP_MISC_DAC_0_CFG + ulOffset, ulReg);

#if (BVDC_P_SUPPORT_TDAC_VER >= BVDC_P_SUPPORT_TDAC_VER_11)
#ifdef BCHP_MISC_DAC_1_CTRL
		ulOffset = (BCHP_MISC_DAC_1_CTRL - BCHP_MISC_DAC_0_CTRL) * i;
		ulReg =
			BCHP_FIELD_DATA(MISC_DAC_0_CTRL, PWRDN,       0);

		BREG_Write32(pVdc->hRegister, BCHP_MISC_DAC_0_CTRL + ulOffset, ulReg);
#endif
#else
#ifdef BCHP_MISC_DAC_1_CTRL
		ulOffset = (BCHP_MISC_DAC_1_CTRL - BCHP_MISC_DAC_0_CTRL) * i;
		ulReg =
			BCHP_FIELD_ENUM(MISC_DAC_0_CTRL, PWRUP_BAIS, PWRDN) |
			BCHP_FIELD_ENUM(MISC_DAC_0_CTRL, PWRDN,      PWRDN);

		BREG_Write32(pVdc->hRegister, BCHP_MISC_DAC_0_CTRL + ulOffset, ulReg);
#endif
#endif
	}

#if (BVDC_P_SUPPORT_TDAC_VER >= BVDC_P_SUPPORT_TDAC_VER_9)
	ulReg =
		BCHP_FIELD_DATA(MISC_DAC_CAL_CTRL_0, MAX_TARGET_DELTA, 0x2) |
		BCHP_FIELD_DATA(MISC_DAC_CAL_CTRL_0, ADC_MAX_VAL,      0x3FF) |
		BCHP_FIELD_DATA(MISC_DAC_CAL_CTRL_0, COUNT,            0x1) |
		BCHP_FIELD_DATA(MISC_DAC_CAL_CTRL_0, STEP_DLY,         0x0) |
		BCHP_FIELD_DATA(MISC_DAC_CAL_CTRL_0, TARGET_VAL,       0x2D8);

	BREG_Write32(pVdc->hRegister, BCHP_MISC_DAC_CAL_CTRL_0, ulReg);

	ulReg =
		BCHP_FIELD_DATA(MISC_DAC_DETECT_CTRL_0, PLUGIN_CNT,        0x2) |
		BCHP_FIELD_DATA(MISC_DAC_DETECT_CTRL_0, PLUGOUT_CNT,       0x2) |
		BCHP_FIELD_DATA(MISC_DAC_DETECT_CTRL_0, PLUGOUT_THRESHOLD, 0x21C) |
		BCHP_FIELD_DATA(MISC_DAC_DETECT_CTRL_0, STEP_DLY,          0x2) |
		BCHP_FIELD_DATA(MISC_DAC_DETECT_CTRL_0, PLUGIN_THRESHOLD,  0xAA);

	BREG_Write32(pVdc->hRegister, BCHP_MISC_DAC_DETECT_CTRL_0, ulReg);
#endif

	BDBG_LEAVE(BVDC_P_Vec_Init_Misc_isr);
	return;
}

/*************************************************************************
 *
 */
void BVDC_P_Vec_Update_OutMuxes_isr
	( BVDC_P_Context           *pVdc )
{
	BSTD_UNUSED(pVdc);
	return ;
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
	uint32_t ulIt;
	uint32_t ulItOffset;

	BDBG_ENTER(BVDC_P_Display_EnableTriggers_isr);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);
	switch(hDisplay->eMasterTg)
	{
	case BVDC_DisplayTg_ePrimIt:
	case BVDC_DisplayTg_eSecIt:
	case BVDC_DisplayTg_eTertIt:
		ulIt = hDisplay->stAnlgChan_0.ulIt;
		BDBG_ASSERT(ulIt != BVDC_P_HW_ID_INVALID);
		ulItOffset = ulIt * BVDC_P_VEC_IT_SIZE;

		/* Re-enable triggers. */
		ulTrigger0 = BREG_Read32_isr(hDisplay->hVdc->hRegister,
			BCHP_IT_0_VEC_TRIGGER_0 + ulItOffset);
		ulTrigger0 &= ~BCHP_IT_0_VEC_TRIGGER_0_ENABLE_MASK;
		if(bEnable)
		{
			ulTrigger0 |= BCHP_IT_0_VEC_TRIGGER_0_ENABLE_MASK;
		}
		BREG_Write32(hDisplay->hVdc->hRegister,
			BCHP_IT_0_VEC_TRIGGER_0 + ulItOffset, ulTrigger0);

		ulTrigger1 = BREG_Read32_isr(hDisplay->hVdc->hRegister,
			BCHP_IT_0_VEC_TRIGGER_1 + ulItOffset);
		ulTrigger1 &= ~BCHP_IT_0_VEC_TRIGGER_1_ENABLE_MASK;
		if(bEnable)
		{
#if	(BVDC_P_SUPPORT_IT_VER >= 2)
			ulTrigger1 |= (hDisplay->stCurInfo.pFmtInfo->bInterlaced || (1 != hDisplay->stCurInfo.ulTriggerModuloCnt))
				? BCHP_IT_0_VEC_TRIGGER_1_ENABLE_MASK : 0;
#else
			ulTrigger1 |= (hDisplay->stCurInfo.pFmtInfo->bInterlaced)
				? BCHP_IT_0_VEC_TRIGGER_1_ENABLE_MASK : 0;
#endif
		}
		BREG_Write32(hDisplay->hVdc->hRegister,
			BCHP_IT_0_VEC_TRIGGER_1 + ulItOffset, ulTrigger1);
		break;

	case BVDC_DisplayTg_eDviDtg: /* dvo in master mode */
		/* Re-enable triggers. */
		ulTrigger0 = BREG_Read32_isr(hDisplay->hVdc->hRegister,
			BCHP_DVI_DTG_0_DTG_TRIGGER_0);
		ulTrigger0 &= ~BCHP_DVI_DTG_0_DTG_TRIGGER_0_ENABLE_MASK;
		if(bEnable)
		{
			ulTrigger0 |= BCHP_DVI_DTG_0_DTG_TRIGGER_0_ENABLE_MASK;
		}
		BREG_Write32(hDisplay->hVdc->hRegister,
			BCHP_DVI_DTG_0_DTG_TRIGGER_0, ulTrigger0);

		ulTrigger1 = BREG_Read32_isr(hDisplay->hVdc->hRegister,
			BCHP_DVI_DTG_0_DTG_TRIGGER_1);
		ulTrigger1 &= ~BCHP_DVI_DTG_0_DTG_TRIGGER_1_ENABLE_MASK;
		if(bEnable)
		{
#if	(BVDC_P_SUPPORT_IT_VER >= 2)
			ulTrigger1 |= (hDisplay->stCurInfo.pFmtInfo->bInterlaced || (1 != hDisplay->stCurInfo.ulTriggerModuloCnt))
				? BCHP_DVI_DTG_0_DTG_TRIGGER_1_ENABLE_MASK : 0;
#else
			ulTrigger1 |= (hDisplay->stCurInfo.pFmtInfo->bInterlaced)
				? BCHP_DVI_DTG_0_DTG_TRIGGER_1_ENABLE_MASK : 0;
#endif
		}
		BREG_Write32(hDisplay->hVdc->hRegister,
			BCHP_DVI_DTG_0_DTG_TRIGGER_1, ulTrigger1);
		break;

#if (BVDC_P_SUPPORT_STG)
	case BVDC_DisplayTg_eStg0: /*STG master mode*/
	case BVDC_DisplayTg_eStg1: /*STG master mode*/
	case BVDC_DisplayTg_eStg2: /*STG master mode*/
	case BVDC_DisplayTg_eStg3: /*STG master mode*/

		BVDC_P_Display_EnableSTGTriggers_isr(hDisplay, bEnable);
#endif
		break;
#if BVDC_P_SUPPORT_ITU656_OUT
	case BVDC_DisplayTg_e656Dtg: /* 656 master mode */
		/* Re-enable triggers. */
		ulTrigger0 = BREG_Read32_isr(hDisplay->hVdc->hRegister,
			BCHP_ITU656_DTG_0_DTG_TRIGGER_0);
		ulTrigger0 &= ~BCHP_ITU656_DTG_0_DTG_TRIGGER_0_ENABLE_MASK;
		if(bEnable)
		{
			ulTrigger0 |= BCHP_ITU656_DTG_0_DTG_TRIGGER_0_ENABLE_MASK;
		}
		BREG_Write32(hDisplay->hVdc->hRegister,
			BCHP_ITU656_DTG_0_DTG_TRIGGER_0, ulTrigger0);

		ulTrigger1 = BREG_Read32_isr(hDisplay->hVdc->hRegister,
			BCHP_ITU656_DTG_0_DTG_TRIGGER_1);
		ulTrigger1 &= ~BCHP_ITU656_DTG_0_DTG_TRIGGER_1_ENABLE_MASK;
		if(bEnable)
		{
			ulTrigger1 |= (hDisplay->stCurInfo.pFmtInfo->bInterlaced)
				? BCHP_ITU656_DTG_0_DTG_TRIGGER_1_ENABLE_MASK : 0;
		}
		BREG_Write32(hDisplay->hVdc->hRegister,
			BCHP_ITU656_DTG_0_DTG_TRIGGER_1, ulTrigger1);
		break;
#endif

		default:
			BDBG_ERR((" Invalid timing generator master %d, display id %d",
				hDisplay->eMasterTg, hDisplay->eId));
			BDBG_ASSERT(0);
			break;
	}
	BDBG_LEAVE(BVDC_P_Display_EnableTriggers_isr);
	return;
}

void BVDC_P_Display_GetAnlgChanByOutput_isr
	( BVDC_Display_Handle             hDisplay,
	  BVDC_P_DisplayInfo             *pDispInfo,
	  BVDC_DisplayOutput              eDisplayOutput,
	  BVDC_P_DisplayAnlgChan        **pstChan
	)
{
	*pstChan = NULL;

	if ((BVDC_P_DISP_IS_ANLG_CHAN_CO(&hDisplay->stAnlgChan_0, pDispInfo) && (eDisplayOutput == BVDC_DisplayOutput_eComponent)) ||
		(BVDC_P_DISP_IS_ANLG_CHAN_CVBS(&hDisplay->stAnlgChan_0, pDispInfo) && (eDisplayOutput == BVDC_DisplayOutput_eComposite)) ||
		(BVDC_P_DISP_IS_ANLG_CHAN_SVIDEO(&hDisplay->stAnlgChan_0, pDispInfo) && (eDisplayOutput == BVDC_DisplayOutput_eSVideo)))
	{
		*pstChan = &hDisplay->stAnlgChan_0;
	}
	else if ((BVDC_P_DISP_IS_ANLG_CHAN_CO(&hDisplay->stAnlgChan_1, pDispInfo) && (eDisplayOutput == BVDC_DisplayOutput_eComponent)) ||
		     (BVDC_P_DISP_IS_ANLG_CHAN_CVBS(&hDisplay->stAnlgChan_1, pDispInfo) && (eDisplayOutput == BVDC_DisplayOutput_eComposite)) ||
		     (BVDC_P_DISP_IS_ANLG_CHAN_SVIDEO(&hDisplay->stAnlgChan_1, pDispInfo) && (eDisplayOutput == BVDC_DisplayOutput_eSVideo)))
	{
		*pstChan = &hDisplay->stAnlgChan_1;
	}

	return;
}


static void BVDC_P_Display_AssignCSOutput
	( BVDC_P_DisplayInfo              *pNewInfo,
	  uint32_t                         ulChanMask,
	  BVDC_P_Output                   *peOutputCS )
{
	if((ulChanMask & BVDC_P_Dac_Mask_Cvbs) || (ulChanMask & BVDC_P_Dac_Mask_Svideo))
	{
		if(VIDEO_FORMAT_IS_NTSC_M(pNewInfo->pFmtInfo->eVideoFmt))
		{
			*peOutputCS = BVDC_P_Output_eYQI;
		}
		else if(VIDEO_FORMAT_IS_NTSC_J(pNewInfo->pFmtInfo->eVideoFmt))
		{
			*peOutputCS = BVDC_P_Output_eYQI_M;
		}
#if BVDC_P_SUPPORT_VEC_SECAM
		else if(VIDEO_FORMAT_IS_SECAM(pNewInfo->pFmtInfo->eVideoFmt))
		{
			if ((pNewInfo->pFmtInfo->eVideoFmt == BFMT_VideoFmt_eSECAM_L) ||
				(pNewInfo->pFmtInfo->eVideoFmt == BFMT_VideoFmt_eSECAM_D) ||
				(pNewInfo->pFmtInfo->eVideoFmt == BFMT_VideoFmt_eSECAM_K))
			{
				*peOutputCS = BVDC_P_Output_eYDbDr_LDK;
			}
			else if((pNewInfo->pFmtInfo->eVideoFmt == BFMT_VideoFmt_eSECAM_B) ||
					(pNewInfo->pFmtInfo->eVideoFmt == BFMT_VideoFmt_eSECAM_G))
			{
				*peOutputCS = BVDC_P_Output_eYDbDr_BG;
			}
			else /* BFMT_VideoFmt_eSECAM_H */
			{
				*peOutputCS = BVDC_P_Output_eYDbDr_H;
			}
		}
#endif
		else if(pNewInfo->pFmtInfo->eVideoFmt == BFMT_VideoFmt_ePAL_NC)
		{
			*peOutputCS = BVDC_P_Output_eYUV_NC;
		}
		else if((pNewInfo->pFmtInfo->eVideoFmt == BFMT_VideoFmt_ePAL_N) ||
				(pNewInfo->pFmtInfo->eVideoFmt == BFMT_VideoFmt_ePAL_M))
		{
			*peOutputCS = BVDC_P_Output_eYUV_N;
		}
		else /* PAL */
		{
			*peOutputCS = BVDC_P_Output_eYUV;
		}
	}

	if(pNewInfo->bHsync)
	{
		*peOutputCS = BVDC_P_Output_eHsync;
	}

	if(ulChanMask & BVDC_P_Dac_Mask_RGB)
	{
		*peOutputCS = BVDC_P_Output_eRGB;
	}

	if(ulChanMask & BVDC_P_Dac_Mask_YPrPb)
	{
		if (VIDEO_FORMAT_27Mhz(pNewInfo->pFmtInfo->ulPxlFreqMask))
		{
			*peOutputCS = BVDC_P_Output_eSDYPrPb;
		}
		else
		{
			*peOutputCS = BVDC_P_Output_eHDYPrPb;
		}
	}
}

static BERR_Code BVDC_P_Display_ValidateDacSettings
	( BVDC_Display_Handle              ahDisplay[] )
{
	BERR_Code                 err = BERR_SUCCESS;
	int i, j;
	BVDC_Handle               hVdc=NULL;
	BVDC_Display_Handle       hDisplay = NULL;
	BVDC_P_DisplayInfo       *pNewInfo=NULL;
	BVDC_P_DisplayInfo       *pCurInfo=NULL;
	BVDC_DacOutput            aNewUsedDac[BVDC_P_MAX_DACS];
	uint32_t                  ulDacMask;
	uint32_t                  ulHdDacCnt = 0;
	uint32_t                  aulCurAnalogChan[BVDC_P_MAX_DISPLAY_COUNT * 2];
	uint32_t                  aulNewAnalogChan[BVDC_P_MAX_DISPLAY_COUNT * 2];

	BDBG_ENTER(BVDC_P_Display_ValidateDacSettings);

	/* Initialized */
	for(i = 0; i < BVDC_P_MAX_DACS; i++)
	{
		aNewUsedDac[i] = BVDC_DacOutput_eUnused;
	}
	for(i = 0; i < BVDC_P_MAX_DISPLAY_COUNT * 2; i++)
	{
		aulCurAnalogChan[i] = aulNewAnalogChan[i] = 0;
	}

	for(i = 0; i < BVDC_P_MAX_DISPLAY_COUNT; i++)
	{
		/* Only validate the create or active onces. */
		if(BVDC_P_STATE_IS_ACTIVE(ahDisplay[i]) ||
		   BVDC_P_STATE_IS_CREATE(ahDisplay[i]))
		{
			hDisplay = ahDisplay[i];
			BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);
			hVdc = hDisplay->hVdc;
			BDBG_OBJECT_ASSERT(hVdc, BVDC_VDC);
			pNewInfo = &hDisplay->stNewInfo;
			pCurInfo = &hDisplay->stCurInfo;

			ulDacMask = 0;
			pNewInfo->bCvbs   = false;
			pNewInfo->bSvideo = false;
			pNewInfo->bHsync  = false;
			pNewInfo->bRgb    = false;
			pNewInfo->bYPrPb  = false;
			pNewInfo->bMultiRateAllow = true;

			/* if RFM is enable, treat it as CVBS */
			if(pNewInfo->eRfmOutput != BVDC_RfmOutput_eUnused)
			{
				pNewInfo->bCvbs = true;
			}

			for(j = 0; j < BVDC_P_MAX_DACS; j++)
			{
				/* Checking for duplicated use of DACs accross all displays */
				if(pNewInfo->aDacOutput[j] != BVDC_DacOutput_eUnused)
				{
					if(aNewUsedDac[j] != BVDC_DacOutput_eUnused)
					{
						BDBG_ERR(("Dac[%d] already used by Display[%d].",
							j, hDisplay->eId));
						return BERR_TRACE(BVDC_ERR_INVALID_DAC_SETTINGS);
					}
					else
					{
						aNewUsedDac[j] = pNewInfo->aDacOutput[j];
					}

					switch(pNewInfo->aDacOutput[j])
					{
						case BVDC_DacOutput_eFilteredCvbs:
						case BVDC_DacOutput_eComposite:
						case BVDC_DacOutput_eVdec0:
							ulDacMask |= BVDC_P_Dac_Mask_Cvbs;
							pNewInfo->bCvbs = true;
							break;
						case BVDC_DacOutput_eSVideo_Luma:
							ulDacMask |= BVDC_P_Dac_Mask_Luma;
							pNewInfo->bSvideo = true;
							break;
						case BVDC_DacOutput_eSVideo_Chroma:
							ulDacMask |= BVDC_P_Dac_Mask_Chroma;
							pNewInfo->bSvideo = true;
							break;
						case BVDC_DacOutput_eRed:
							ulDacMask |= BVDC_P_Dac_Mask_R;
							pNewInfo->bRgb = true;
							break;
						case BVDC_DacOutput_eGreen:
						case BVDC_DacOutput_eGreen_NoSync:
							ulDacMask |= BVDC_P_Dac_Mask_G;
							pNewInfo->bRgb = true;
							break;
						case BVDC_DacOutput_eBlue:
							ulDacMask |= BVDC_P_Dac_Mask_B;
							pNewInfo->bRgb = true;
							break;
						case BVDC_DacOutput_eY:
							ulDacMask |= BVDC_P_Dac_Mask_Y;
							pNewInfo->bYPrPb = true;
							break;
						case BVDC_DacOutput_ePr:
							ulDacMask |= BVDC_P_Dac_Mask_Pr;
							pNewInfo->bYPrPb = true;
							break;
						case BVDC_DacOutput_ePb:
							ulDacMask |= BVDC_P_Dac_Mask_Pb;
							pNewInfo->bYPrPb = true;
							break;
						case BVDC_DacOutput_eHsync:
							pNewInfo->bHsync = true;
							break;
						default:
							break;
							return BERR_TRACE(BVDC_ERR_INVALID_DAC_SETTINGS);
					} /* end of switch */
				}
			} /* end of MAX_DACS looping */

			/* Checking for valid DAC grouping */
			if(pNewInfo->bYPrPb && (ulDacMask & BVDC_P_Dac_Mask_YPrPb) != BVDC_P_Dac_Mask_YPrPb)
			{
				BDBG_ERR(("ulDacMask = 0x%x, BVDC_P_Dac_Mask_YPrPb = 0x%x", ulDacMask, BVDC_P_Dac_Mask_YPrPb));
				return BERR_TRACE(BVDC_ERR_INVALID_DAC_SETTINGS);
			}
			if(pNewInfo->bRgb && (ulDacMask & BVDC_P_Dac_Mask_RGB) != BVDC_P_Dac_Mask_RGB)
			{
				BDBG_ERR(("ulDacMask = 0x%x, BVDC_P_Dac_Mask_RGB = 0x%x", ulDacMask, BVDC_P_Dac_Mask_RGB));
				return BERR_TRACE(BVDC_ERR_INVALID_DAC_SETTINGS);
			}
			if(pNewInfo->bSvideo && (ulDacMask & BVDC_P_Dac_Mask_Svideo) != BVDC_P_Dac_Mask_Svideo)
			{
				BDBG_ERR(("ulDacMask = 0x%x, BVDC_P_Dac_Mask_Svideo = 0x%x", ulDacMask, BVDC_P_Dac_Mask_Svideo));
				return BERR_TRACE(BVDC_ERR_INVALID_DAC_SETTINGS);
			}

			/* Checking for valid display fmt with DAC setting */
			if((pNewInfo->bCvbs || pNewInfo->bSvideo) &&
			   (VIDEO_FORMAT_IS_HD(pNewInfo->pFmtInfo->eVideoFmt)))
			{
				return BERR_TRACE(BVDC_ERR_VIDEOFMT_OUTPUT_MISMATCH);
			}

			/* Update HD Dac count */
			if(pNewInfo->bYPrPb && VIDEO_FORMAT_IS_HD(pNewInfo->pFmtInfo->eVideoFmt))
			{
				ulHdDacCnt++;
			}
			if(pNewInfo->bRgb && VIDEO_FORMAT_IS_HD(pNewInfo->pFmtInfo->eVideoFmt))
			{
				ulHdDacCnt++;
			}

			/*******/
			aulCurAnalogChan[i * 2] = pCurInfo->ulAnlgChan0Mask;
			aulCurAnalogChan[i * 2 + 1] = pCurInfo->ulAnlgChan1Mask;

			if(pNewInfo->bYPrPb && pNewInfo->bRgb)
			{
				/* if both YPrPb and Rgb, use both analog channels */
				aulNewAnalogChan[i * 2] |= BVDC_P_Dac_Mask_YPrPb;
				aulNewAnalogChan[i * 2 + 1] |= BVDC_P_Dac_Mask_RGB;
			}
			else if((pNewInfo->bYPrPb || pNewInfo->bRgb) &&
					!pNewInfo->bCvbs && !pNewInfo->bSvideo)
			{
				/* only component => if previously allocated on chan 1,
				   then use same channel */
				if((aulCurAnalogChan[i * 2 + 1] & BVDC_P_Dac_Mask_HD) ||
				   (hDisplay->stAnlgChan_1.eState == BVDC_P_DisplayResource_eResInactive))
				{
					aulNewAnalogChan[i * 2 + 1] |= (pNewInfo->bYPrPb) ? BVDC_P_Dac_Mask_YPrPb : 0;
					aulNewAnalogChan[i * 2 + 1] |= (pNewInfo->bRgb) ? BVDC_P_Dac_Mask_RGB : 0;
				}
				else
				{
					aulNewAnalogChan[i * 2] |= (pNewInfo->bYPrPb) ? BVDC_P_Dac_Mask_YPrPb : 0;
					aulNewAnalogChan[i * 2] |= (pNewInfo->bRgb) ? BVDC_P_Dac_Mask_RGB : 0;
				}
			}
			else if(pNewInfo->bYPrPb || pNewInfo->bRgb)
			{
				/* Both component and composite/svideo */
#if BVDC_P_VEC_STANDALONE_BUG_FIXED
				if((aulCurAnalogChan[i * 2 + 1] & BVDC_P_Dac_Mask_HD) ||
				   (aulCurAnalogChan[i * 2] & BVDC_P_Dac_Mask_SD))
#else
				/* either chan_0 or chan_1 is still around even with no DACs */
				/* need to match up the HD used one */
				if(hDisplay->stAnlgChan_1.ulHdsrc != BVDC_P_HW_ID_INVALID)
#endif
				{
					/* if previously allocated HD on chan 1 or SD on chan 0  */
					/* then use same channel */
					aulNewAnalogChan[i * 2 + 1] |= (pNewInfo->bYPrPb) ? BVDC_P_Dac_Mask_YPrPb : 0;
					aulNewAnalogChan[i * 2 + 1] |= (pNewInfo->bRgb) ? BVDC_P_Dac_Mask_RGB : 0;
					aulNewAnalogChan[i * 2] |= (pNewInfo->bCvbs) ? BVDC_P_Dac_Mask_Cvbs : 0;
					aulNewAnalogChan[i * 2] |= (pNewInfo->bSvideo) ? BVDC_P_Dac_Mask_Svideo : 0;
				}
				else
				{
					aulNewAnalogChan[i * 2] |= (pNewInfo->bYPrPb) ? BVDC_P_Dac_Mask_YPrPb : 0;
					aulNewAnalogChan[i * 2] |= (pNewInfo->bRgb) ? BVDC_P_Dac_Mask_RGB : 0;
					aulNewAnalogChan[i * 2 + 1] |= (pNewInfo->bCvbs) ? BVDC_P_Dac_Mask_Cvbs : 0;
					aulNewAnalogChan[i * 2 + 1] |= (pNewInfo->bSvideo) ? BVDC_P_Dac_Mask_Svideo : 0;
				}
			}
			else
			{
				/* Only composite or svideo */
				if((hDisplay->stAnlgChan_1.eState == BVDC_P_DisplayResource_eResInactive) ||
				   (aulCurAnalogChan[i * 2 + 1] & BVDC_P_Dac_Mask_SD))
				{
					aulNewAnalogChan[i * 2 + 1] |= (pNewInfo->bCvbs) ? BVDC_P_Dac_Mask_Cvbs : 0;
					aulNewAnalogChan[i * 2 + 1] |= (pNewInfo->bSvideo) ? BVDC_P_Dac_Mask_Svideo : 0;
				}
				else
				{
					aulNewAnalogChan[i * 2] |= (pNewInfo->bCvbs) ? BVDC_P_Dac_Mask_Cvbs : 0;
					aulNewAnalogChan[i * 2] |= (pNewInfo->bSvideo) ? BVDC_P_Dac_Mask_Svideo : 0;
				}
			}

			pNewInfo->ulAnlgChan0Mask = aulNewAnalogChan[i * 2];
			pNewInfo->ulAnlgChan1Mask = aulNewAnalogChan[i * 2 + 1];

			/* Initialize colorspace */
			pNewInfo->eAnlg_0_OutputColorSpace = BVDC_P_Output_eUnknown;
			pNewInfo->eAnlg_1_OutputColorSpace = BVDC_P_Output_eUnknown;

			BVDC_P_Display_AssignCSOutput(pNewInfo, pNewInfo->ulAnlgChan0Mask, &pNewInfo->eAnlg_0_OutputColorSpace);
			BVDC_P_Display_AssignCSOutput(pNewInfo, pNewInfo->ulAnlgChan1Mask, &pNewInfo->eAnlg_1_OutputColorSpace);

			/* evaluate bMultiRateAllow flag */
			/* To keep NTSC(cvbs/svideo) as 59.94hz and 480i can be 60.00hz or 59.94hz */
			/* Assumptions multiple of 24/30hz are capable of multirate capable display. */
			pNewInfo->bMultiRateAllow =
				(0 == (pNewInfo->pFmtInfo->ulVertFreq % (24 * BFMT_FREQ_FACTOR))) ||
				(0 == (pNewInfo->pFmtInfo->ulVertFreq % (15 * BFMT_FREQ_FACTOR)));
			pNewInfo->bMultiRateAllow &= !(pNewInfo->bCvbs || pNewInfo->bSvideo);
		}	  /* end of active display	 */
	}		  /* end of DISPLAY looping  */

	/* Checking against HW limitation */
	if(ulHdDacCnt > BVDC_P_SUPPORT_HD_DAC)
	{
		return BERR_TRACE(BVDC_ERR_RESOURCE_NOT_AVAILABLE);
	}

	if(hVdc == NULL)
	{
		BDBG_LEAVE(BVDC_P_Display_ValidateDacSettings);
		return BERR_SUCCESS;
	}

	/* Handling DAC resource */
	BKNI_EnterCriticalSection();

	/* release cur first */
	for(i = 0; i < BVDC_P_MAX_DISPLAY_COUNT * 2; i++)
	{
		if(aulNewAnalogChan[i] != aulCurAnalogChan[i])
		{
			if(aulCurAnalogChan[i] != 0 && aulNewAnalogChan[i] == 0)
			{
				hDisplay = ahDisplay[i >> 1];
				BDBG_MSG(("D%d Release cur Dac resource for Chan%d: Cur = 0x%x, New = 0x%x",
					hDisplay->eId, (i & 0x1) ? 1 : 0,
					aulCurAnalogChan[i], aulNewAnalogChan[i]));
				if((i & 0x1) == 0)
				{
					hDisplay->stAnlgChan_0.eState = BVDC_P_DisplayResource_eDestroy;
					hDisplay->stNewInfo.stDirty.stBits.bChan0 = BVDC_P_DIRTY;
				}
				else
				{
						hDisplay->stAnlgChan_1.eState = BVDC_P_DisplayResource_eDestroy;
						hDisplay->stNewInfo.stDirty.stBits.bChan1 = BVDC_P_DIRTY;
					}
				}
			}
		}

	/* acquire new */
	for(i = 0; i < BVDC_P_MAX_DISPLAY_COUNT * 2; i++)
	{
		BVDC_P_DisplayAnlgChan *pstChan;
		uint32_t ulMask;
		bool bHwBugWorkAround = false;
		hDisplay = ahDisplay[i >> 1];

#if BVDC_P_VEC_STANDALONE_BUG_FIXED
		bHwBugWorkAround = false;
#else
		if(BVDC_P_STATE_IS_ACTIVE(hDisplay) ||
		   BVDC_P_STATE_IS_CREATE(hDisplay))
		{
			bHwBugWorkAround = (hDisplay->bAnlgEnable && /* analog master */
			                    /* and no DACs */
			                    (hDisplay->stNewInfo.ulAnlgChan0Mask == 0) &&
			                    (hDisplay->stNewInfo.ulAnlgChan1Mask == 0) &&
			                    (hDisplay->stAnlgChan_0.eState == BVDC_P_DisplayResource_eInactive) &&
			                    (hDisplay->stAnlgChan_1.eState == BVDC_P_DisplayResource_eInactive));
		}
#endif

		if(((aulNewAnalogChan[i] != aulCurAnalogChan[i]) &&
			(((aulNewAnalogChan[i] & BVDC_P_Dac_Mask_SD) && !(aulCurAnalogChan[i] & BVDC_P_Dac_Mask_SD)) ||
			 ((aulNewAnalogChan[i] & BVDC_P_Dac_Mask_HD) && !(aulCurAnalogChan[i] & BVDC_P_Dac_Mask_HD))))
			|| bHwBugWorkAround)
		{
			BDBG_MSG(("i=%d, bWA=%d, aulCurAnalogChan = 0x%x, aulNewAnalogChan = 0x%x",
				i, bHwBugWorkAround, aulCurAnalogChan[i], aulNewAnalogChan[i]));
			if((aulNewAnalogChan[i] != 0) || bHwBugWorkAround)
			{
				if((i & 0x1) == 0)
				{
					/* Chan 0 */
					pstChan = &hDisplay->stAnlgChan_0;
					ulMask = hDisplay->stNewInfo.ulAnlgChan0Mask;
				}
				else
				{
					/* Chan 1 */
					pstChan = &hDisplay->stAnlgChan_1;
					ulMask = hDisplay->stNewInfo.ulAnlgChan1Mask;
				}

				BDBG_MSG(("D%d Acquire new resource for Chan_%d", hDisplay->eId, (i & 0x1)));
				err = BVDC_P_AllocITResources(hVdc->hResource, hDisplay->eId * 2 + (i & 0x1), pstChan,
							hDisplay->stAnlgChan_0.ulIt);
				err |= BVDC_P_AllocAnalogChanResources(hVdc->hResource, hDisplay->eId * 2 + (i & 0x1), pstChan,
							(!(ulMask & BVDC_P_Dac_Mask_SD) && hDisplay->bHdCap) ? true : false ,
							(ulMask & BVDC_P_Dac_Mask_HD) ? true : false ,
							((ulMask & BVDC_P_Dac_Mask_SD) && (BVDC_P_NUM_SHARED_SECAM != 0) && (VIDEO_FORMAT_IS_SECAM(pNewInfo->pFmtInfo->eVideoFmt))) ? true : false);

				if(err)
				{
					BKNI_LeaveCriticalSection();
					BDBG_ERR(("Failed to add %s%s%soutput to display %d%s. Short of VEC hardware block. Check hardware capability.",
						     (ulMask & BVDC_P_Dac_Mask_HD) ? "component " : "",
						     (ulMask & BVDC_P_Dac_Mask_Cvbs) ? "composite " : "",
						     (ulMask & BVDC_P_Dac_Mask_Svideo) ? "S-Video " : "",
						     hDisplay->eId,
						     ((ulMask & BVDC_P_Dac_Mask_SD) && (BVDC_P_NUM_SHARED_SECAM != 0) && (VIDEO_FORMAT_IS_SECAM(pNewInfo->pFmtInfo->eVideoFmt))) ?
						     ", and being SECAM capable" : ""));
					BDBG_LEAVE(BVDC_P_Display_ValidateDacSettings);
					return BERR_TRACE(err);
				}

				/* Sanity check - CVBS won't work with HDSRC */
				if((ulMask & BVDC_P_Dac_Mask_SD) && (pstChan->ulHdsrc != BVDC_P_HW_ID_INVALID))
				{
					/* if not analog master, free IT resource */
					if(!hDisplay->bAnlgEnable)
						BVDC_P_FreeITResources(hVdc->hResource, pstChan);
					BVDC_P_FreeAnalogChanResources(hVdc->hResource, pstChan);
					BKNI_LeaveCriticalSection();
					BDBG_ERR(("Fail to assign HDSRC to %s%soutput for display %d",
						     (ulMask & BVDC_P_Dac_Mask_Cvbs) ? "composite " : "",
						     (ulMask & BVDC_P_Dac_Mask_Svideo) ? "S-Video " : "",
						     hDisplay->eId));
					BDBG_LEAVE(BVDC_P_Display_ValidateDacSettings);
					return BERR_TRACE(BVDC_ERR_RESOURCE_NOT_AVAILABLE);
				}

				pstChan->bEnable = true;
				pstChan->eState = BVDC_P_DisplayResource_eCreate;
				if((i & 0x1) == 0)
					hDisplay->stNewInfo.stDirty.stBits.bChan0 = BVDC_P_DIRTY;
				else
					hDisplay->stNewInfo.stDirty.stBits.bChan1 = BVDC_P_DIRTY;

#if !HW7425_807_IS_FIXED
				if((hDisplay->stAnlgChan_0.eState == BVDC_P_DisplayResource_eCreate && hDisplay->stNewInfo.ulAnlgChan1Mask != 0) ||
				   (hDisplay->stAnlgChan_1.eState == BVDC_P_DisplayResource_eCreate && hDisplay->stNewInfo.ulAnlgChan0Mask != 0))
				{
					BDBG_MSG(("Need to reset both Chan0 & Chan1"));
					hDisplay->stNewInfo.stDirty.stBits.bTiming = BVDC_P_DIRTY;
				}
#endif

				if(bHwBugWorkAround)
				{
					BDBG_MSG(("All DACs OFF"));
				}
				else
				{
					if(aulNewAnalogChan[i] != 0 && aulCurAnalogChan[i] == 0)
					{
						BDBG_MSG(("Display %d Acquire new Dac resource for AnlgChan_%d", hDisplay->eId, (i & 0x1)));
						err = BVDC_P_AllocDacResources(hVdc->hResource, pstChan, aulNewAnalogChan[i]);
						if(err)
						{
							BKNI_LeaveCriticalSection();
							BDBG_ERR(("Failed to allocate DAC for display %d", hDisplay->eId));
							BDBG_LEAVE(BVDC_P_Display_ValidateDacSettings);
							return BERR_TRACE(err);
						}
					}
				}

#if (BVDC_P_SUPPORT_RFM_OUTPUT != 0)
				if(hDisplay->stCurInfo.eRfmOutput != BVDC_RfmOutput_eUnused)
				{
					hDisplay->stNewInfo.stDirty.stBits.bRfm = BVDC_P_DIRTY;
				}
#endif
			}
		}
	}
	BKNI_LeaveCriticalSection();

	/* Now aggregate the DAC assignment from all the display handle to a central place */
	BKNI_EnterCriticalSection();
	for (i = 0; i < BVDC_P_MAX_DACS; i++ )
	{
		hVdc->aDacOutput[i] = BVDC_DacOutput_eUnused;
		hVdc->aDacDisplay[i] = UINT32_C(-1);

		for(j = 0; j < BVDC_P_MAX_DISPLAY_COUNT; j++)
		{
			if((BVDC_P_STATE_IS_ACTIVE(ahDisplay[j]) || BVDC_P_STATE_IS_CREATE(ahDisplay[j]))
				&& (ahDisplay[j]->stNewInfo.aDacOutput[i] != BVDC_DacOutput_eUnused))
			{
				hVdc->aDacOutput[i] = ahDisplay[j]->stNewInfo.aDacOutput[i];
				hVdc->aDacDisplay[i] = (uint32_t)ahDisplay[j]->eId;
			}
		}
	}
	BKNI_LeaveCriticalSection();


	BDBG_LEAVE(BVDC_P_Display_ValidateDacSettings);
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
	BERR_Code                 err = BERR_SUCCESS;
	BVDC_Display_Handle       hDisplay;
	BVDC_Display_ValidateSetting validateSettingHandler;
	uint32_t i, j;

	BDBG_ENTER(BVDC_P_Display_ValidateChanges);

	for (i=0; i < BVDC_P_MAX_DISPLAY_COUNT; i++)
	{
		if(BVDC_P_STATE_IS_ACTIVE(ahDisplay[i]) ||
		   BVDC_P_STATE_IS_CREATE(ahDisplay[i]))
		{
			ahDisplay[i]->stNewInfo.bErrorLastSetting = true;
		}
	}

	if ((err = BVDC_P_Display_ValidateDacSettings(ahDisplay)))
	{
		BDBG_ERR(("Invalid DAC settings, error 0x%x", err));
		return BERR_TRACE(err);
	}

	for (i=0; i < BVDC_P_MAX_DISPLAY_COUNT; i++)
	{
		hDisplay = ahDisplay[i];

		if(BVDC_P_STATE_IS_ACTIVE(hDisplay) ||
		   BVDC_P_STATE_IS_CREATE(hDisplay))
		{
			/*
			 * Edit out redundant dirty bits state. It would be nice to move
			 * this logic somewhere else, but I don't know where it should go.
			 */
			BVDC_P_DisplayInfo* pNewInfo = &(hDisplay->stNewInfo);
			if (pNewInfo->stDirty.stBits.bTiming == BVDC_P_DIRTY)
			{
				pNewInfo->stDirty.stBits.bSoftTiming = BVDC_P_CLEAN;
				pNewInfo->stDirty.stBits.bWidthTrim  = BVDC_P_CLEAN;
			}
#if DCS_SUPPORT
			if (pNewInfo->stDirty.stBits.bDacSetting == BVDC_P_DIRTY)
			{
				pNewInfo->stDirty.stBits.bSoftTiming = BVDC_P_CLEAN;
			}
#endif

			if (BVDC_P_IS_DIRTY(&(pNewInfo->stDirty)))
			{
				for (j=0; j < BVDC_P_NUM_DIRTY_BITS(&(pNewInfo->stDirty)); j++)
				{
					if (BVDC_P_DISPLAY_IS_BIT_DIRTY(&(pNewInfo->stDirty), j))
					{
						/* Get validation handler from display event handling table */
						validateSettingHandler = BVDC_P_astDisplayEventHndlTbl[j].validateSettingHandler;
						if (validateSettingHandler)
						{
							if ((err = validateSettingHandler(hDisplay)))
							{
								BDBG_ERR(("Invalid setting %d on display %d, err %d", j, i, err));
								return BERR_TRACE(err);
							}
						}
					}
				}
			}
		}
	}

	for (i=0; i < BVDC_P_MAX_DISPLAY_COUNT; i++)
	{
		if(BVDC_P_STATE_IS_ACTIVE(ahDisplay[i]) ||
		   BVDC_P_STATE_IS_CREATE(ahDisplay[i]))
		{
			ahDisplay[i]->stNewInfo.bErrorLastSetting = false;
		}
	}

	BDBG_LEAVE(BVDC_P_Display_ValidateChanges);
	return err;
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
	BVDC_Display_CopySetting copySettingHandler;

	BDBG_ENTER(BVDC_P_Display_ApplyChanges_isr);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);
	BDBG_OBJECT_ASSERT(hDisplay->hCompositor, BVDC_CMP);

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
	}
	else if(BVDC_P_STATE_IS_DESTROY(hDisplay))
	{
		BDBG_MSG(("Display%d de-activated.", hDisplay->eId));

		/* disable triggers to complete shutdown display callbacks. */
		BVDC_P_Display_EnableTriggers_isr(hDisplay, false);
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

		/* Free all the resources
		 * Note: The cores may still be up but triggers are
		 *       disabled. Next create will reset and restart
		 *       them.
		 *
		 *       If we want to shut the cores down, then RUL version
		 *       tear-down can not be used. This is because the RDC
		 *       slot has been disabled thus RUL won't get executed.
		 *       If we defer the RDC slot disable, a complicated scheme
		 *       may be needed here because there won't be ISR any more
		 *       since triggers are disabled.
		 *
		 *       One way to solve this dilemma is to create host write
		 *       version tear-down. For now we leave them as active since
		 *       the power consumption is small.
		 */
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)
		if ((hDisplay->stAnlgChan_0.ulIt != BVDC_P_HW_ID_INVALID))
		{
			BREG_Write32(hDisplay->hVdc->hRegister,
				BCHP_VEC_CFG_IT_0_SOURCE + hDisplay->stAnlgChan_0.ulIt * 4, BCHP_VEC_CFG_IT_0_SOURCE_SOURCE_DISABLE);
		}
		BVDC_P_FreeITResources(hDisplay->hCompositor->hVdc->hResource, &hDisplay->stAnlgChan_0);
		BVDC_P_FreeAnalogChanResources(hDisplay->hCompositor->hVdc->hResource, &hDisplay->stAnlgChan_0);
		BVDC_P_FreeAnalogChanResources(hDisplay->hCompositor->hVdc->hResource, &hDisplay->stAnlgChan_1);
#else
		if (hDisplay->bAnlgEnable)
		{
			BVDC_P_FreeITResources(hDisplay->hCompositor->hVdc->hResource, &hDisplay->stAnlgChan_0);
			BVDC_P_FreeAnalogChanResources(hDisplay->hCompositor->hVdc->hResource, &hDisplay->stAnlgChan_0);
			BVDC_P_FreeAnalogChanResources(hDisplay->hCompositor->hVdc->hResource, &hDisplay->stAnlgChan_1);
		}
#endif

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)
		if ((hDisplay->stDviChan.ulDvi != BVDC_P_HW_ID_INVALID))
		{
			BREG_Write32(hDisplay->hVdc->hRegister,
				BCHP_VEC_CFG_DVI_DTG_0_SOURCE + hDisplay->stDviChan.ulDvi * 4, BCHP_VEC_CFG_DVI_DTG_0_SOURCE_SOURCE_DISABLE);
		}
		BVDC_P_FreeDviChanResources(hDisplay->hCompositor->hVdc->hResource, &hDisplay->stDviChan);
#else
		if (hDisplay->stDviChan.bEnable)
		{
			BVDC_P_FreeDviChanResources(hDisplay->hCompositor->hVdc->hResource, &hDisplay->stDviChan);
		}
#endif

#if BVDC_P_SUPPORT_ITU656_OUT
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)
		if ((hDisplay->st656Chan.ul656 != BVDC_P_HW_ID_INVALID))
		{
			BREG_Write32(hDisplay->hVdc->hRegister,
				BCHP_VEC_CFG_ITU656_DTG_0_SOURCE, BCHP_VEC_CFG_ITU656_DTG_0_SOURCE_SOURCE_DISABLE);
		}
		BVDC_P_Free656ChanResources(hDisplay->hCompositor->hVdc->hResource, &hDisplay->st656Chan);
#else
		if (hDisplay->st656Chan.bEnable)
		{
			BVDC_P_Free656ChanResources(hDisplay->hCompositor->hVdc->hResource, &hDisplay->st656Chan);
		}
#endif
#endif

#if BVDC_P_SUPPORT_STG
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)
		if ((hDisplay->stStgChan.ulStg != BVDC_P_HW_ID_INVALID))
		{
			uint32_t  ulRegOffset = 0;
			if(BVDC_P_DISPLAY_USED_STG(hDisplay->eMasterTg))
				ulRegOffset = (hDisplay->eMasterTg - BVDC_DisplayTg_eStg0) * sizeof(uint32_t);
			else /* Slave mode */
				ulRegOffset = hDisplay->stStgChan.ulStg * sizeof(uint32_t);
			BREG_Write32(hDisplay->hVdc->hRegister,
				BCHP_VEC_CFG_STG_0_SOURCE + ulRegOffset, BCHP_VEC_CFG_STG_0_SOURCE_SOURCE_DISABLE);
		}
		BVDC_P_FreeStgChanResources(hDisplay->hCompositor->hVdc->hResource, &hDisplay->stStgChan);
#else
		if (hDisplay->stStgChan.bEnable)
		{
			BVDC_P_FreeStgChanResources(hDisplay->hCompositor->hVdc->hResource, &hDisplay->stStgChan);
		}
#endif
#endif

		/* Clear dirty bits */
		BVDC_P_CLEAN_ALL_DIRTY(&(hDisplay->stNewInfo.stDirty));
		BVDC_P_CLEAN_ALL_DIRTY(&(hDisplay->stCurInfo.stDirty));
		BVDC_P_CLEAN_ALL_DIRTY(&(hDisplay->stPrevDirty));

		return;
	}

	/**
	 * Reset IT state to not active for display with masterTg turned on, in order for apply
	 * changes to restart ISR.
	 */
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)
	if(((hDisplay->st656Chan.bEnable) &&
	    (hDisplay->stNewInfo.bEnable656  && !hDisplay->stCurInfo.bEnable656)) ||
	   ((hDisplay->stStgChan.bEnable) &&
	    (hDisplay->stNewInfo.bEnableStg  && !hDisplay->stCurInfo.bEnableStg)) ||
	   ((hDisplay->stDviChan.bEnable) &&
	    (hDisplay->stNewInfo.bEnableHdmi && !hDisplay->stCurInfo.bEnableHdmi)))
	{
		hDisplay->eItState = BVDC_P_ItState_eNotActive;
		BDBG_MSG(("Display %d resets state to kick start", hDisplay->eId));
	}
#else
	if(((hDisplay->eMasterTg == BVDC_DisplayTg_e656Dtg) &&
	    (hDisplay->stNewInfo.bEnable656  && !hDisplay->stCurInfo.bEnable656)) ||
	   ((BVDC_P_DISPLAY_USED_STG(hDisplay->eMasterTg)) &&
	    (hDisplay->stNewInfo.bEnableStg  && !hDisplay->stCurInfo.bEnableStg)) ||
	   ((hDisplay->eMasterTg == BVDC_DisplayTg_eDviDtg) &&
	    (hDisplay->stNewInfo.bEnableHdmi && !hDisplay->stCurInfo.bEnableHdmi)))
	{
		hDisplay->eItState = BVDC_P_ItState_eNotActive;
		BDBG_MSG(("Display %d resets state to kick start", hDisplay->eId));
	}
#endif

	/* Copy all the new settings to CurInfo so that they can be applied
	 * to HW in BVDC_P_Vec_BuildRul_isr(). */
	if (BVDC_P_IS_DIRTY(&(hDisplay->stNewInfo.stDirty)))
	{
		for (i=0; i < BVDC_P_NUM_DIRTY_BITS(&(hDisplay->stNewInfo.stDirty)); i++)
		{
			if (BVDC_P_DISPLAY_IS_BIT_DIRTY(&(hDisplay->stNewInfo.stDirty), i))
			{
				/* Get copy handler from display event handling table */
				copySettingHandler = BVDC_P_astDisplayEventHndlTbl[i].copySettingHandler;
				if (copySettingHandler)
				{
					copySettingHandler(hDisplay);
				}

				/* Clear the dirty bit in new info */
				BVDC_P_DISPLAY_CLEAR_DIRTY_BIT(&(hDisplay->stNewInfo.stDirty), i);

				/* Set the dirty bit in current info */
				BVDC_P_DISPLAY_SET_DIRTY_BIT(&(hDisplay->stCurInfo.stDirty), i);
			}
		}

		/* Mark user changes pending. Wait for next vsync ISR to build RUL and
		 * clear the flags.
		 */
		BKNI_ResetEvent(hDisplay->hAppliedDoneEvent);
		hDisplay->bSetEventPending = true;
	}

	/* No RUL has been executed yet, we've no triggers activated.  Must
	 * force the initial top RUL here! */
	if((BVDC_P_ItState_eNotActive == hDisplay->eItState) &&
	   (!hDisplay->bIsBypass))
	{
		/* Start List */
		BVDC_P_ListInfo stList;

		/* Build Vec Top RUL and force it to execute immediately. */
		BRDC_List_Handle hList = BVDC_P_CMP_GET_LIST(hDisplay->hCompositor,
			BAVC_Polarity_eTopField);
		BRDC_Slot_Handle hSlot = BVDC_P_CMP_GET_SLOT(hDisplay->hCompositor,
			BAVC_Polarity_eTopField);

		BRDC_List_SetNumEntries_isr(hList, 0);
		BVDC_P_ReadListInfo_isr(&stList, hList);
		if(BVDC_P_DISPLAY_USED_STG(hDisplay->eMasterTg))
		{
			/* skip the intermediate states */
			hDisplay->eItState = BVDC_P_ItState_eActive;
			BVDC_P_Compositor_BuildSyncSlipRul_isr(hDisplay->hCompositor, &stList, BAVC_Polarity_eTopField, true);
		}
		else
		{
			BVDC_P_Vec_BuildRul_isr(hDisplay, &stList, BAVC_Polarity_eTopField);
		}
		BVDC_P_WriteListInfo_isr(&stList, hList);

		/* Assign Trigger to slot.	Effectively enable slots. */
#if (BVDC_P_ORTHOGONAL_VEC_VER < BVDC_P_ORTHOGONAL_VEC_VER_1)
		switch(hDisplay->eMasterTg)
		{
			case BVDC_DisplayTg_eDviDtg:
				hDisplay->eTopTrigger = BRDC_Trigger_eDvoTrig0;
				hDisplay->eBotTrigger = BRDC_Trigger_eDvoTrig1;
				break;

			case BVDC_DisplayTg_e656Dtg:
				hDisplay->eTopTrigger = BRDC_Trigger_eDtgTrig0;
				hDisplay->eBotTrigger = BRDC_Trigger_eDtgTrig1;
				break;

			case BVDC_DisplayTg_ePrimIt:
			case BVDC_DisplayTg_eSecIt:
			case BVDC_DisplayTg_eTertIt:
				switch(hDisplay->stAnlgChan_0.ulIt)
				{
					case 0:
						hDisplay->eTopTrigger = BRDC_Trigger_eVec0Trig0;
						hDisplay->eBotTrigger = BRDC_Trigger_eVec0Trig1;
						break;
					case 1:
						hDisplay->eTopTrigger = BRDC_Trigger_eVec1Trig0;
						hDisplay->eBotTrigger = BRDC_Trigger_eVec1Trig1;
						break;
					case 2:
						hDisplay->eTopTrigger = BRDC_Trigger_eVec2Trig0;
						hDisplay->eBotTrigger = BRDC_Trigger_eVec2Trig1;
						break;
					default:
						break;
				}
				break;

			case BVDC_DisplayTg_eStg0:
				hDisplay->eTopTrigger = BRDC_Trigger_eStg0Trig0;
				hDisplay->eBotTrigger = BRDC_Trigger_eStg0Trig1;
				break;
#if (BVDC_P_SUPPORT_STG > 1)
			case BVDC_DisplayTg_eStg1:
				break;
#endif
			default:
				break;
		}
#else
		switch(hDisplay->hCompositor->eId)
		{
			case BVDC_CompositorId_eCompositor0:
				hDisplay->eTopTrigger = BRDC_Trigger_eCmp_0Trig0;
				hDisplay->eBotTrigger = BRDC_Trigger_eCmp_0Trig1;
				break;

			case BVDC_CompositorId_eCompositor1:
				hDisplay->eTopTrigger = BRDC_Trigger_eCmp_1Trig0;
				hDisplay->eBotTrigger = BRDC_Trigger_eCmp_1Trig1;
				break;

			case BVDC_CompositorId_eCompositor2:
				hDisplay->eTopTrigger = BRDC_Trigger_eCmp_2Trig0;
				hDisplay->eBotTrigger = BRDC_Trigger_eCmp_2Trig1;
				break;

			case BVDC_CompositorId_eCompositor3:
				hDisplay->eTopTrigger = BRDC_Trigger_eCmp_3Trig0;
				hDisplay->eBotTrigger = BRDC_Trigger_eCmp_3Trig1;
				break;

			case BVDC_CompositorId_eCompositor4:
				hDisplay->eTopTrigger = BRDC_Trigger_eCmp_4Trig0;
				hDisplay->eBotTrigger = BRDC_Trigger_eCmp_4Trig1;
				break;

			case BVDC_CompositorId_eCompositor5:
				hDisplay->eTopTrigger = BRDC_Trigger_eCmp_5Trig0;
				hDisplay->eBotTrigger = BRDC_Trigger_eCmp_5Trig1;
				break;

			case BVDC_CompositorId_eCompositor6:
				hDisplay->eTopTrigger = BRDC_Trigger_eCmp_6Trig0;
				hDisplay->eBotTrigger = BRDC_Trigger_eCmp_6Trig1;
				break;

			default:
				break;
		}
#endif

		BVDC_P_Compositor_AssignTrigger_isr(hDisplay->hCompositor,
			hDisplay->eTopTrigger, hDisplay->eBotTrigger);

		BRDC_Slot_SetList_isr(hSlot, hList);
		BRDC_Slot_Execute_isr(hSlot);
	}

	BDBG_LEAVE(BVDC_P_Display_ApplyChanges_isr);
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
 *  BVDC_P_Vec_BuildGameMode_isr
 **************************************************************************/
static void BVDC_P_Vec_BuildGameMode_isr
	( BVDC_Display_Handle              hDisplay,
	  BVDC_P_ListInfo                 *pList )
{
	uint32_t    ulRmOffset;

	ulRmOffset = hDisplay->stAnlgChan_0.ulIt * BVDC_P_VEC_RM_SIZE;

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
			*pList->pulCurrent++ = BRDC_REGISTER(BCHP_RM_0_PHASE_INC + ulRmOffset);
			*pList->pulCurrent++ =
				BCHP_FIELD_DATA(RM_0_PHASE_INC, PHASE_INC,
				hDisplay->pRmTable[BVDC_P_DISPLAY_RM_PHASE_INC_REG_OFFSET] + lAdj);

			hDisplay->bRmAdjusted = true;
			BDBG_MSG(("Game mode delay: %d[usec]; to accelarate VEC%d RM? %d;",
				hDisplay->hWinGameMode->ulCurBufDelay, hDisplay->stAnlgChan_0.ulIt, lAdj));
		}
		else if(hDisplay->bRmAdjusted) /* restore */
		{
			/* PRIM_RM_PHASE_INC (RW) */
			*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
			*pList->pulCurrent++ = BRDC_REGISTER(BCHP_RM_0_PHASE_INC + ulRmOffset);
			*pList->pulCurrent++ = BCHP_FIELD_DATA(RM_0_PHASE_INC, PHASE_INC,
				hDisplay->pRmTable[BVDC_P_DISPLAY_RM_PHASE_INC_REG_OFFSET]);
			hDisplay->bRmAdjusted = false;
		}
	}
	else if(hDisplay->bRmAdjusted) /* restore */
	{
		/* PRIM_RM_PHASE_INC (RW) */
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_RM_0_PHASE_INC + ulRmOffset);
		*pList->pulCurrent++ = BCHP_FIELD_DATA(RM_0_PHASE_INC, PHASE_INC,
			hDisplay->pRmTable[BVDC_P_DISPLAY_RM_PHASE_INC_REG_OFFSET]);
		hDisplay->bRmAdjusted = false;
	}

}

/*************************************************************************
 *  {secret}
 *	BVDC_P_Vec_BuildVsync_isr
 *
 *	Adds Vec settings required for Vsync updates:
 *	VF_AGC_VALUES (for macrovision)
 *	IT_BVB_CONFIG[PSYNC] or DTG_BVB[PSYNC]
 **************************************************************************/
void BVDC_P_Vec_BuildVsync_isr
	( BVDC_Display_Handle              hDisplay,
	  BVDC_P_ListInfo                 *pList,
	  BAVC_Polarity                    eFieldPolarity )
{
	BVDC_P_DisplayInfo *pCurInfo;

	BDBG_ENTER(BVDC_P_Vec_BuildVsync_isr);
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);

	pCurInfo = &hDisplay->stCurInfo;

	/* Primary or Secondary path */
	if (!hDisplay->bIsBypass)
	{
		BVDC_P_Vec_BuildGameMode_isr(hDisplay, pList);
	}

#if DCS_SUPPORT /** { **/

	/* Execute DCS no-glitch transition, if any. */
	if ((hDisplay->stAnlgChan_0.bEnable) || (hDisplay->stAnlgChan_1.bEnable))
	{
		BVDC_P_DCS_StateUpdate_isr (hDisplay, eFieldPolarity, pList);
	}

#else /** } !DCS_SUPPORT { **/

	/* Update VF_POS_SYNC_VALUES for every frame for AGC cycling.
	 * Note: progressive format will not have Bottom Field interrupt. */
	if (((hDisplay->stAnlgChan_0.bEnable) || (hDisplay->stAnlgChan_1.bEnable))
		&&
	   pCurInfo->eMacrovisionType != BVDC_MacrovisionType_eNoProtection)
	{
		uint32_t ulVal = BVDC_P_GetPosSyncValue(hDisplay, &pList->pulCurrent);
		if (hDisplay->stAnlgChan_0.bEnable)
		{
			uint32_t       ulVf = hDisplay->stAnlgChan_0.ulVf;
			uint32_t ulVfOffset = BVDC_P_VEC_VF_SIZE * ulVf;
			BDBG_ASSERT(ulVf != BVDC_P_HW_ID_INVALID);

			*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
			*pList->pulCurrent++ =
				BRDC_REGISTER(BCHP_VF_0_POS_SYNC_VALUES + ulVfOffset);
			*pList->pulCurrent++ = ulVal;
		}
		if (hDisplay->stAnlgChan_1.bEnable)
		{
			uint32_t ulVf       = hDisplay->stAnlgChan_1.ulVf;
			uint32_t ulVfOffset = BVDC_P_VEC_VF_SIZE * ulVf;
			BDBG_ASSERT(ulVf != BVDC_P_HW_ID_INVALID);

			*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
			*pList->pulCurrent++ =
				BRDC_REGISTER(BCHP_VF_0_POS_SYNC_VALUES + ulVfOffset);
			*pList->pulCurrent++ = ulVal;
		}
	}
	else
#endif /* } DCS_SUPPORT **/
	{
		/* Build a nop RUL to make RDC sanity check happy.  */
		*pList->pulCurrent++ = BRDC_OP_NOP();
	}

	/* Write the ariticial L2 interrupt bit */
	if(hDisplay->stCurInfo.bArtificialVsync)
	{
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(hDisplay->stCurInfo.ulArtificialVsyncRegAddr);
		*pList->pulCurrent++ = hDisplay->stNewInfo.ulArtificialVsyncMask;
	}

	/* Display callback installed? */
	if (pCurInfo->pfGenericCallback)
	{
		BVDC_Display_CallbackData *pCbData = &hDisplay->stCallbackData;

		pCbData->eId = hDisplay->eId;
		pCbData->ePolarity = BVDC_P_NEXT_POLARITY(eFieldPolarity);

		/* Notify external modules of rate manager updated. */
		if((hDisplay->bRateManagerUpdated) &&
		   (pCurInfo->stCallbackSettings.stMask.bRateChange))
		{
			pCbData->stMask.bRateChange = 1;
			pCbData->sDisplayInfo = pCurInfo->stRateInfo;
			/* Clear rate change flag */
			hDisplay->bRateManagerUpdated = false;
		}
		else
		{
			pCbData->stMask.bRateChange = 0;
		}

		/* Notify external modules of vsync cadence */
		if((BVDC_P_ItState_eActive == hDisplay->eItState) &&
		   (pCurInfo->stCallbackSettings.stMask.bPerVsync))
		{
			pCbData->stMask.bPerVsync = 1;
		}
		else
		{
			pCbData->stMask.bPerVsync = 0;
		}

#if BVDC_P_SUPPORT_CMP_CRC
		if (pCurInfo->stCallbackSettings.stMask.bCrc)
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

			pCbData->stMask.bCrc = 1;
		}
		else
		{
			pCbData->stMask.bCrc = 0;
		}
#endif

#if BVDC_P_SUPPORT_STG
		if (pCurInfo->stCallbackSettings.stMask.bStgPictureId &&
		   (hDisplay->hCompositor->ulPicId != pCbData->ulStgPictureId))
		{
			pCbData->ulDecodePictureId = hDisplay->hCompositor->ulDecodePictureId;
			pCbData->ulStgPictureId    = hDisplay->hCompositor->ulPicId;
			pCbData->sDisplayInfo      = pCurInfo->stRateInfo;
			pCbData->stMask.bStgPictureId = 1;
		}
		else
		{
			pCbData->stMask.bStgPictureId = 0;
		}
#endif
		if (BVDC_P_CB_IS_DIRTY(&pCbData->stMask))
		{
			pCurInfo->pfGenericCallback(pCurInfo->pvGenericParm1,
				pCurInfo->iGenericParm2, (void*)pCbData);
		}
	}

#if (BVDC_P_SUPPORT_TDAC_VER >= BVDC_P_SUPPORT_TDAC_VER_9)
	/* Read DAC detection status register */
	if(hDisplay->hVdc->bDacDetectionEnable)
	{
		uint32_t ulReg;
		bool bCalibrated;

		ulReg = BREG_Read32_isr(hDisplay->hVdc->hRegister, BCHP_MISC_DAC_CABLE_STATUS_0);

		bCalibrated = BVDC_P_GET_FIELD(ulReg, MISC_DAC_CABLE_STATUS_0, CALIBRATED);
		if(bCalibrated != hDisplay->hVdc->bCalibrated)
		{
			BDBG_MSG(("Calibrate = %d", bCalibrated));
			hDisplay->hVdc->bCalibrated = bCalibrated;
			hDisplay->stCurInfo.stDirty.stBits.bDacSetting = BVDC_P_DIRTY;
		}
	}
#endif

	/* If the dirty bit(s) is from user applied changes,
	 * Set the event to unblock the thread waiting for the
	 * "hApppliedDoneEvent".
	 */
	if(hDisplay->bSetEventPending && !BVDC_P_IS_DIRTY(&(hDisplay->stCurInfo.stDirty)))
	{
		BDBG_MSG(("BuildVsync Set AppliedDoneEvent"));
		BKNI_SetEvent_isr(hDisplay->hAppliedDoneEvent);
		hDisplay->bSetEventPending = false;
	}
	BDBG_LEAVE(BVDC_P_Vec_BuildVsync_isr);
}

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
	uint32_t i;
	BVDC_Display_ApplySetting    applySettingHandler;
    bool bBuildFormatRul =
		(BVDC_P_ItState_eNotActive  == hDisplay->eItState) ||
		(BVDC_P_ItState_eSwitchMode == hDisplay->eItState);
	BDBG_ENTER(BVDC_P_Vec_BuildRul_isr);

	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);

#if DCS_SUPPORT
	/* Experimental */
	BVDC_P_DCS_Check_isr (hDisplay, eFieldPolarity, 0, pList);
#endif


	/* if bTiming is dirty, IT state _must_ be either switch or inactive mode;
	 * bDacSetting dirty bit seems to force setting TG_CONFIG to start TG;
	 * if IT state is active already, clear previous bTiming dirty bit to
	 * prevent source isr building VEC format RUL which may lock up display
	 * state machine!
	 */
	if(bBuildFormatRul)
	{   /* enforce format RUL to be built */
		hDisplay->stCurInfo.stDirty.stBits.bTiming = BVDC_P_DIRTY;
		hDisplay->stCurInfo.stDirty.stBits.bDacSetting = BVDC_P_DIRTY;
	}
	else if(!hDisplay->stStgChan.bEnable)
	{   /* Clear previous bTiming dirty bit here.
	     * The reasoning is:
	     *   Display state can only be set to eActive by display_isr after it assured
	     * the VEC format RUL being executed by checking the display format switch
	     * marker. There is no sense to rely on stored bTiming dirty bit here. Plus
	     * the stored previous bTiming dirty bit could mislead VEC to unnecessary format
	     * switch RUL and possible lockup.
	     */
		hDisplay->stPrevDirty.stBits.bTiming = BVDC_P_CLEAN;
	}

	/* We store a copy of last current dirty bits and clear it only if
	* the RUL for the new settngs get executed. Otherwise, we will
	 * re-build the RUL to apply the new settings again.
	 */
	if (BVDC_P_IS_DIRTY(&hDisplay->stPrevDirty))
	{
		if(!pList->bLastExecuted)
		{
			BVDC_P_OR_ALL_DIRTY(&(hDisplay->stCurInfo.stDirty), &(hDisplay->stPrevDirty));
		}
		BVDC_P_CLEAN_ALL_DIRTY(&(hDisplay->stPrevDirty));
	}


	if (BVDC_P_IS_DIRTY(&(hDisplay->stCurInfo.stDirty)))
	{
		BVDC_P_VEC_MSG(("Display%d: CurDirty = 0x%x", hDisplay->eId, hDisplay->stCurInfo.stDirty));
		BVDC_P_VEC_MSG(("\t bChan0            = %d", hDisplay->stCurInfo.stDirty.stBits.bChan0));
		BVDC_P_VEC_MSG(("\t bChan1            = %d", hDisplay->stCurInfo.stDirty.stBits.bChan1));
		BVDC_P_VEC_MSG(("\t bTiming           = %d", hDisplay->stCurInfo.stDirty.stBits.bTiming));
		BVDC_P_VEC_MSG(("\t bSoftTiming       = %d", hDisplay->stCurInfo.stDirty.stBits.bSoftTiming));
		BVDC_P_VEC_MSG(("\t b3DSetting        = %d", hDisplay->stCurInfo.stDirty.stBits.b3DSetting));
		BVDC_P_VEC_MSG(("\t bDacSetting       = %d", hDisplay->stCurInfo.stDirty.stBits.bDacSetting));
		BVDC_P_VEC_MSG(("\t bTimeBase         = %d", hDisplay->stCurInfo.stDirty.stBits.bTimeBase));
		BVDC_P_VEC_MSG(("\t bCallback         = %d", hDisplay->stCurInfo.stDirty.stBits.bCallback));
		BVDC_P_VEC_MSG(("\t bCallbackFunc     = %d", hDisplay->stCurInfo.stDirty.stBits.bCallbackFunc));
		BVDC_P_VEC_MSG(("\t bWidthTrim        = %d", hDisplay->stCurInfo.stDirty.stBits.bWidthTrim));
		BVDC_P_VEC_MSG(("\t bInputCS          = %d", hDisplay->stCurInfo.stDirty.stBits.bInputCS));
		BVDC_P_VEC_MSG(("\t b240p             = %d", hDisplay->stCurInfo.stDirty.stBits.b240p));
		BVDC_P_VEC_MSG(("\t bPhaseAdjustment  = %d", hDisplay->stCurInfo.stDirty.stBits.bPhaseAdjustment));
		BVDC_P_VEC_MSG(("\t bSrcFrameRate     = %d", hDisplay->stCurInfo.stDirty.stBits.bSrcFrameRate));
		BVDC_P_VEC_MSG(("\t bFrameDrop        = %d", hDisplay->stCurInfo.stDirty.stBits.bFrameDrop));
#if (BVDC_P_SUPPORT_RFM_OUTPUT != 0)
		BVDC_P_VEC_MSG(("\t bRfm              = %d", hDisplay->stCurInfo.stDirty.stBits.bRfm));
#endif
		BVDC_P_VEC_MSG(("\t bHdmiEnable       = %d", hDisplay->stCurInfo.stDirty.stBits.bHdmiEnable));
		BVDC_P_VEC_MSG(("\t bHdmiCsc          = %d", hDisplay->stCurInfo.stDirty.stBits.bHdmiCsc));
#if (BVDC_P_SUPPORT_ITU656_OUT != 0)
		BVDC_P_VEC_MSG(("\t b656Enable        = %d", hDisplay->stCurInfo.stDirty.stBits.b656Enable));
#endif
		BVDC_P_VEC_MSG(("\t bMpaaComp         = %d", hDisplay->stCurInfo.stDirty.stBits.bMpaaComp));
		BVDC_P_VEC_MSG(("\t bMpaaHdmi         = %d", hDisplay->stCurInfo.stDirty.stBits.bMpaaHdmi));

		BVDC_P_VEC_MSG(("\t bTimeStamp        = %d", hDisplay->stCurInfo.stDirty.stBits.bTimeStamp));
		BVDC_P_VEC_MSG(("\t bAlignment        = %d", hDisplay->stCurInfo.stDirty.stBits.bAlignment));
		BVDC_P_VEC_MSG(("\t bHdmiDroplines    = %d", hDisplay->stCurInfo.stDirty.stBits.bHdmiDroplines));
		BVDC_P_VEC_MSG(("\t bHdmiXvYcc        = %d", hDisplay->stCurInfo.stDirty.stBits.bHdmiXvYcc));
		BVDC_P_VEC_MSG(("\t bHdmiSyncOnly     = %d", hDisplay->stCurInfo.stDirty.stBits.bHdmiSyncOnly));
		BVDC_P_VEC_MSG(("\t bHdmiRmChanged    = %d", hDisplay->stCurInfo.stDirty.stBits.bHdmiRmChanged));
		BVDC_P_VEC_MSG(("\t bAspRatio         = %d", hDisplay->stCurInfo.stDirty.stBits.bAspRatio));

#if (BVDC_P_SUPPORT_STG != 0)
		BVDC_P_VEC_MSG(("\t bStg              = %d", hDisplay->stCurInfo.stDirty.stBits.bStgEnable));
#endif
		BVDC_P_VEC_MSG(("\t bVfFilters        = %d", hDisplay->stCurInfo.stDirty.stBits.bVfFilter));
		BVDC_P_VEC_MSG(("\t bOutputMute       = %d", hDisplay->stCurInfo.stDirty.stBits.bOutputMute));
		BVDC_P_VEC_MSG(("\t bMiscCtrl         = %d", hDisplay->stCurInfo.stDirty.stBits.bMiscCtrl));

		/* Store a copy of current dirty bits */
		hDisplay->stPrevDirty = hDisplay->stCurInfo.stDirty;

		for (i=0; i < BVDC_P_NUM_DIRTY_BITS(&(hDisplay->stNewInfo.stDirty)); i++)
		{
			if (BVDC_P_DISPLAY_IS_BIT_DIRTY(&(hDisplay->stCurInfo.stDirty), i))
			{
				/* Get copy handler from display event handling table */
				applySettingHandler = BVDC_P_astDisplayEventHndlTbl[i].applySettingHandler;

				/* Each event should at least have a apply setting handler */
				BDBG_ASSERT(applySettingHandler);

				/* Each handler needs to clear its own dirty bit in stCurInfo.stDirty.
				 * This is because some settings may take more than one vsync to finish.
				 * So the handler for such an event will have a state machine and clears
				 * the dirty bit only if all the necessary steps are completed.
				 */
				applySettingHandler(hDisplay, pList, eFieldPolarity);
			}
		}
	}

#if (BVDC_P_SUPPORT_STG)
	if(hDisplay->stStgChan.bEnable && !bBuildFormatRul)
	{
		BVDC_P_ProgrameStgMBox_isr(hDisplay, pList, eFieldPolarity);
	}
#endif
	if (BVDC_P_ItState_eActive == hDisplay->eItState)
	{

		BVDC_P_Vec_BuildVsync_isr(hDisplay, pList, eFieldPolarity);

		/* Alignment */
		BVDC_P_Display_Alignment(hDisplay, pList);
	}

#if DCS_SUPPORT
	/* Experimental */
	BVDC_P_DCS_Check_isr (hDisplay, eFieldPolarity, 1, pList);
#endif

	BDBG_LEAVE(BVDC_P_Vec_BuildRul_isr);
}


/* API for compositor to pass in source picture related info
 */
void  BVDC_P_Display_SetSourceInfo
	( BVDC_Display_Handle              hDisplay,
	  const BVDC_P_Display_SrcInfo    *pSrcInfo )
{
	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);
	if (hDisplay->stCurInfo.bWidthTrimmed != pSrcInfo->bWidthTrimmed)
	{
		if ((VIDEO_FORMAT_IS_NTSC(hDisplay->stCurInfo.pFmtInfo->eVideoFmt) &&
			(hDisplay->stCurInfo.eMacrovisionType < BVDC_MacrovisionType_eTest01) &&
			 pSrcInfo->bWidthTrimmed && (!hDisplay->stCurInfo.bWidthTrimmed)) ||
			 (hDisplay->stCurInfo.bWidthTrimmed && (!pSrcInfo->bWidthTrimmed)))
		{
			/* Enable/Disble width trimmed mode.
			 */
			hDisplay->stCurInfo.bWidthTrimmed = pSrcInfo->bWidthTrimmed;
			hDisplay->stCurInfo.stDirty.stBits.bWidthTrim = BVDC_P_DIRTY;
			hDisplay->stCurInfo.stDirty.stBits.bTimeBase = BVDC_P_DIRTY;
		}
	}

	if (hDisplay->stCurInfo.eCmpColorSpace != pSrcInfo->eCmpColorSpace)
	{
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)
		hDisplay->stNewInfo.eCmpColorSpace = pSrcInfo->eCmpColorSpace;
#endif
		hDisplay->stCurInfo.eCmpColorSpace = pSrcInfo->eCmpColorSpace;
		hDisplay->stCurInfo.stDirty.stBits.bInputCS = BVDC_P_DIRTY;
	}

	if (hDisplay->stCurInfo.b240pMode != pSrcInfo->b240pMode)
	{
		hDisplay->stCurInfo.b240pMode = pSrcInfo->b240pMode;
		hDisplay->stCurInfo.stDirty.stBits.b240p = BVDC_P_DIRTY;
	}

	if (pSrcInfo->ePhaseAdjustMode != BVDC_PhaseAdjustMode_eDisable)
	{
		hDisplay->stCurInfo.ePhaseAdjustMode = pSrcInfo->ePhaseAdjustMode;
		hDisplay->stCurInfo.ulVdecPhase = pSrcInfo->ulVdecPhase;
		hDisplay->stCurInfo.stDirty.stBits.bPhaseAdjustment = BVDC_P_DIRTY;
	}

	if (hDisplay->stCurInfo.bFullRate!= pSrcInfo->bFullRate)
	{
		hDisplay->stCurInfo.bFullRate = pSrcInfo->bFullRate;
		hDisplay->stCurInfo.stDirty.stBits.bSrcFrameRate = BVDC_P_DIRTY;
	}
}


BERR_Code BVDC_Display_GetItUcodeInfo
	( BVDC_Display_Handle              hDisplay,
	  BVDC_Display_ItUcodeInfo        *pInfo )
{
	BREG_Handle hReg;
	uint32_t iOffset = 0; /* TODO: need to find offset for display */

	BDBG_ENTER(BVDC_Display_GetItUcodeInfo);

	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);

	hReg = hDisplay->hVdc->hRegister;

	pInfo->ulAnalogTimestamp =
		BREG_Read32 (
			hReg,
			BCHP_IT_0_MICRO_INSTRUCTIONi_ARRAY_BASE +
				(sizeof(uint32_t)*BVDC_P_RAM_TABLE_TIMESTAMP_IDX) +
				iOffset);
	pInfo->ulAnalogChecksum =
		BREG_Read32 (
			hReg,
			BCHP_IT_0_MICRO_INSTRUCTIONi_ARRAY_BASE +
				(sizeof(uint32_t)*BVDC_P_RAM_TABLE_CHECKSUM_IDX) +
				iOffset);
	pInfo->ulI656Timestamp =
		BREG_Read32 (
			hReg,
			BCHP_DTRAM_0_DMC_INSTRUCTIONi_ARRAY_BASE +
				(sizeof(uint32_t)*BVDC_P_DTRAM_TABLE_TIMESTAMP_IDX));
	pInfo->ulI656Checksum =
		BREG_Read32 (
			hReg,
			BCHP_DTRAM_0_DMC_INSTRUCTIONi_ARRAY_BASE +
				(sizeof(uint32_t)*BVDC_P_DTRAM_TABLE_CHECKSUM_IDX));
	pInfo->ulDviTimestamp =
		BREG_Read32 (
			hReg,
			BCHP_DTRAM_0_DMC_INSTRUCTIONi_ARRAY_BASE +
				(sizeof(uint32_t)*BVDC_P_DTRAM_TABLE_SIZE) +
				(sizeof(uint32_t)*BVDC_P_DTRAM_TABLE_TIMESTAMP_IDX));
	pInfo->ulDviChecksum =
		BREG_Read32 (
			hReg,
			BCHP_DTRAM_0_DMC_INSTRUCTIONi_ARRAY_BASE +
			(sizeof(uint32_t)*BVDC_P_DTRAM_TABLE_SIZE) +
			(sizeof(uint32_t)*BVDC_P_DTRAM_TABLE_CHECKSUM_IDX));

	BDBG_LEAVE(BVDC_Display_GetItUcodeInfo);
	return BERR_SUCCESS;
}

/* End of File */
