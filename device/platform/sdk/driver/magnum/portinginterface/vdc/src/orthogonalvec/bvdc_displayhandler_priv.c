/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvdc_displayhandler_priv.c $
 * $brcm_Revision: Hydra_Software_Devel/349 $
 * $brcm_Date: 10/16/12 6:20p $
 *
 * Module Description:
 *     Private module for setting up the modular Vec
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vdc/src/orthogonalvec/bvdc_displayhandler_priv.c $
 * 
 * Hydra_Software_Devel/349   10/16/12 6:20p albertl
 * SW7425-2989: Added output colorspace debug messages.
 * 
 * Hydra_Software_Devel/347   10/15/12 4:01p tdo
 * SW7425-977: refactor display format info usage in HDMI path programming
 * for 4kx2k format support
 *
 * Hydra_Software_Devel/346   9/24/12 6:20p syang
 * SW7425-3977: add aspect-ratio handling for 3D video src and display
 *
 * Hydra_Software_Devel/345   9/10/12 6:26p tdo
 * SW7231-837: Remove band gap calibration option
 *
 * Hydra_Software_Devel/344   9/4/12 4:52p darnstein
 * SW7346-909: tables of IT block register values grow by one entry. THe
 * tables that previously covered ADDR_0_3 to PCL_8 now end with
 * STACK_REG_8_9.
 *
 * Hydra_Software_Devel/343   8/31/12 10:29a vanessah
 * SW7425-3862: use the roundoff for VertFreq equality check
 *
 * Hydra_Software_Devel/342   8/26/12 1:13p tdo
 * SW7125-1302: Avoid reseting used resources.
 *
 * Hydra_Software_Devel/341   8/24/12 10:17a tdo
 * SW7552-324: Fix bug in selecting SECAM block when format switching
 * to/from secam format
 *
 * Hydra_Software_Devel/340   8/15/12 6:23p tdo
 * SW7445-8: Create Appframework emulation environment for 7445 A0
 *
 * Hydra_Software_Devel/339   8/10/12 3:29p pntruong
 * SW7435-276: Updated NTSC/480p to 720x480i/720x480p respectively.
 * Added support 482i/483p with new enums.
 *
 * Hydra_Software_Devel/338   8/9/12 4:05p tdo
 * SW7125-1302: Fix compiling error
 *
 * Hydra_Software_Devel/337   8/8/12 6:01p tdo
 * SW7125-1302: Composite output is black & white when Component not
 * enabled
 *
 * Hydra_Software_Devel/336   7/24/12 3:03p tdo
 * SW7360-29 : Temporarily turn off DAC auto-detection & calibration until
 * it's fully working for 7360 and 7231
 *
 * Hydra_Software_Devel/335   7/13/12 11:27a tdo
 * SW7563-9: Add support for VDC
 *
 * Hydra_Software_Devel/334   7/5/12 12:02p tdo
 * SW7231-837:  Add option to turn on/off Dac Gain Override
 *
 * Hydra_Software_Devel/333   6/6/12 8:52a tdo
 * SW7552-284: Change in CABLE_DETECT_SEL register for HD formats
 *
 * Hydra_Software_Devel/332   6/5/12 3:55p tdo
 * SW7435-121: Corrupted video displayed in NTSC J format on Composite
 * output if build brutus binary with MACROVISION_SUPPORT=y
 *
 * Hydra_Software_Devel/331   5/24/12 2:24p vanessah
 * SW7435-199: using display aspect ratio when no window open
 *
 * Hydra_Software_Devel/330   5/21/12 5:40p darnstein
 * HW7552-167: Custom Macrovision logic was not turning off PS/AGC pulses
 * when requested by user.
 *
 * Hydra_Software_Devel/329   5/9/12 5:51p vanessah
 * SW7425-2873: fix non real time fmt change issue
 *
 * Hydra_Software_Devel/328   5/7/12 5:31p albertl
 * SW7358-279: Fixed warnings.
 *
 * Hydra_Software_Devel/327   5/7/12 5:27p albertl
 * SW7358-279: Rewrote code to simpify dac cofiguration and fix issues for
 * all TADC versions.
 *
 * Hydra_Software_Devel/326   5/7/12 10:41a vanessah
 * SW7425-2873:  exclude kick start from fmt switch
 *
 * Hydra_Software_Devel/325   5/3/12 9:26p vanessah
 * SW7425-2873: transcode path fmt stress test
 *
 * Hydra_Software_Devel/324   4/11/12 11:30a tdo
 * SW7425-2865: It3D_isr called with uninitialized data
 *
 * Hydra_Software_Devel/323   3/21/12 1:39p syang
 * SW7425-2699: put mpaa under the control of vdc resource manager; handle
 * correctly both order of enabling/disabling mpaa and turning on/off
 * hdmi/component output
 *
 * Hydra_Software_Devel/322   3/12/12 5:16p tdo
 * SW7425-2603: Update drain pixels delay for SCART output
 *
 * Hydra_Software_Devel/321   3/12/12 4:04p tdo
 * SW7425-2603: Update drain pixels delay for SCART output
 *
 * Hydra_Software_Devel/320   3/8/12 12:04p yuxiaz
 * SW7425-2546: Fixed warning msg.
 *
 * Hydra_Software_Devel/319   3/2/12 4:05p pntruong
 * SW7231-584: Added artificial display vsync generation to support dsp
 * transcoding.
 *
 * Hydra_Software_Devel/318   3/1/12 9:59a hongtaoz
 * SW7425-2474: removed STG teardown for dynamic resolution change to
 * avoid dropped trigger;
 *
 * Hydra_Software_Devel/317   2/23/12 10:19a vanessah
 * SW7425-2431: fix custom fmt stress test
 *
 * Hydra_Software_Devel/316   2/21/12 5:31p albertl
 * SW7344-288: Corrected DAC scale ctrl values for blue and red channels.
 *
 * Hydra_Software_Devel/315   2/17/12 5:53p darnstein
 * SW7125-1234: PAL-M and PAL-N cannot be supported for DCS at this time.
 * This situation was not handled cleanly in software.
 *
 * Hydra_Software_Devel/314   2/13/12 7:04p darnstein
 * SW7358-227: handle chips that do not have register
 * VF_n.NEG_SYNC_AMPLITUDE_EXTN.
 *
 * Hydra_Software_Devel/313   2/2/12 10:46a tdo
 * SW7429-57: Using the right analog channel for RFM
 *
 * Hydra_Software_Devel/312   1/23/12 3:31p tdo
 * SW7435-9: Add support for CMP4/5
 *
 * Hydra_Software_Devel/311   1/13/12 2:51p tdo
 * SW7358-159 , SW7418-55 : Default DAC auto detection ON for
 * 7231/7344/7346Bx
 *
 * Hydra_Software_Devel/310   1/12/12 12:37p darnstein
 * SW7125-1124: Implement two-stage transition between DCS ON and DCS OFF.
 * This works around a hardware issue.
 *
 * Hydra_Software_Devel/309   1/11/12 3:07p darnstein
 * SW7125-1124: merge DCS Hybrid+ to main branch. DCS almost ready for
 * production.
 *
 * Hydra_Software_Devel/SW7125-1124/13   1/6/12 1:32p darnstein
 * SW7125-1124: Experimental toggle of TG_CONFIG.MC_ENABLES is commented
 * out now.
 *
 * Hydra_Software_Devel/SW7125-1124/12   1/4/12 4:34p darnstein
 * SW7125-1124: implement experimental toggle of MC_ENABLES. This improves
 * access to buffered VEC registers.
 *
 * Hydra_Software_Devel/SW7125-1124/11   12/19/11 4:55p darnstein
 * SW7125-1124: simplify DCS access method. A few bug fixes too.
 *
 * Hydra_Software_Devel/SW7125-1124/10   12/14/11 4:46p darnstein
 * SW7125-1124: merge from main branch.
 *
 * Hydra_Software_Devel/SW7125-1124/9   12/13/11 3:52p darnstein
 * SW7125-1124: merge from main branch.
 *
 *
 * Hydra_Software_Devel/SW7125-1124/8   12/12/11 7:45p darnstein
 * SW7125-1124: DCS Hybrid+ software ready for debugging.
 *
 * Hydra_Software_Devel/SW7125-1124/7   12/9/11 9:43p darnstein
 * SW7125-1124: need to add a dirty bit. Checking in DCS work first.
 *
 * Hydra_Software_Devel/SW7125-1124/6   12/6/11 8:18p darnstein
 * SW7125-1124: Finish DCS code swapping logic.
 *
 * Hydra_Software_Devel/SW7125-1124/5   12/1/11 6:46p darnstein
 * SW7125-1124: change definition of IT_VER to identify early version of
 * IT hardware that has PCL_6, PCL_7, and PCL_8 registers.
 *
 * Hydra_Software_Devel/SW7125-1124/4   12/1/11 5:39p darnstein
 * SW7125-1124: checkpoint.
 *
 * Hydra_Software_Devel/SW7125-1124/3   11/21/11 6:17p darnstein
 * SW7125-1124: merge from main branch.
 *
 * Hydra_Software_Devel/SW7125-1124/1   11/10/11 7:06p darnstein
 * SW7125-1124: partial implementation of DCS Hybrid+.
 *
 * Hydra_Software_Devel/305   11/17/11 2:43p pntruong
 * SW7344-239: Fixed 7550b0 build errors.
 *
 * Hydra_Software_Devel/304   11/17/11 10:52a pntruong
 * SW7344-239: Fixed 7429 build error.
 *
 * Hydra_Software_Devel/303   11/16/11 3:08p pntruong
 * SW7344-239: Initialized grpd mux.
 *
 *
 * Hydra_Software_Devel/301   11/9/11 3:07p darnstein
 * SW7425-1658: finish programming VF NEG_SYNC amplitude (extension,
 * VALUE2).
 *
 * Hydra_Software_Devel/300   11/8/11 1:11p vanessah
 * SW7425-1704: factor STG functions out of bvdc_displayhandler_priv.c
 *
 * Hydra_Software_Devel/299   11/8/11 10:43a tdo
 * SW7425-1696: Display VDAC output lost sync when encoder display is
 * opened
 *
 * Hydra_Software_Devel/298   11/4/11 3:50p darnstein
 * SW7425-1658: program the new register VF_n_NEG_SYNC_AMPLITUDE_EXTN.
 *
 * Hydra_Software_Devel/297   11/3/11 3:57p tdo
 * SW7344-135: Video lost in Vesa format on HDMI/Component outputs when
 * switching output formats during a playback.  IT stand-alone is not
 * working for 7344B0/7231B0
 *
 * Hydra_Software_Devel/296   10/19/11 5:04p hongtaoz
 * SW7425-1546: added rate change callback support for STG display;
 *
 * Hydra_Software_Devel/295   10/19/11 4:55p tdo
 * SW7425-1416, SW7358-159:  Re-order DAC auto detection programming to
 * avoid the issue with ADC state machine when the ADC clock is slow.
 *
 * Hydra_Software_Devel/294   10/19/11 1:47p vanessah
 * SW7425-1488: MADR RUL delay for NRT transcode
 *
 * Hydra_Software_Devel/293   10/18/11 1:49p vanessah
 * SW7425-1170: use digital W and H for digital trigger in ASP calculation
 *
 * Hydra_Software_Devel/292   10/14/11 2:23p tdo
 * SW7425-1416, SW7358-159: Add feature to control automatic DAC
 * detection.  Default is currently OFF until it's fully functional.
 *
 * Hydra_Software_Devel/291   10/11/11 5:11p tdo
 * SW7358-159: No ouput on composite port for B0.  Properly set DAC
 * registers to support auto detection
 *
 * Hydra_Software_Devel/290   10/11/11 12:19p tdo
 * SW7425-1475, SW7425-1430: Fix MPAA for HDMI. Keep at least one analog
 * chan path per display when analog master and no DACs to drain data.
 *
 * Hydra_Software_Devel/289   10/5/11 5:36p vanessah
 * SW7425-1417: fix code refactor error
 *
 * Hydra_Software_Devel/288   9/30/11 5:15p darnstein
 * SW7400-2665: merge from JIRA branch.
 *
 * Hydra_Software_Devel/SW7400-2665/2   9/30/11 4:57p darnstein
 * SW7400-2665: merge from main branch.
 *
 * Hydra_Software_Devel/SW7400-2665/1   9/28/11 7:27p darnstein
 * SW7400-2665: Implement double rate 576P analog output for orthogonal
 * VEC chipsets.
 *
 * Hydra_Software_Devel/287   9/29/11 4:51p tdo
 * SW7425-980: Insure HDMI tx is always in slave mode
 *
 * Hydra_Software_Devel/286   9/22/11 5:22p tdo
 * SW7425-1334: B0: (720p/1080i/1080p/etc) 59.94hz output has sawtooth on
 * analog output
 *
 * Hydra_Software_Devel/285   9/15/11 4:08p pntruong
 * SW7425-1291: Removed references to channel id.  Refactored stg control
 * rdb usage instances.
 *
 * Hydra_Software_Devel/284   9/15/11 11:31a vanessah
 * SW7425-923: B0 STG NRT mode
 *
 * Hydra_Software_Devel/283   9/14/11 11:12p vanessah
 * SW7425-923: diasable timer trigger only when ignore picture flag
 *
 * Hydra_Software_Devel/282   9/14/11 6:22p vanessah
 * SW7425-923: add B0 NRT STG bringup
 *
 * Hydra_Software_Devel/281   9/14/11 1:54p vanessah
 * SW7425-923: fix MBox address error
 *
 * Hydra_Software_Devel/280   9/13/11 10:12a vanessah
 * SW7425-923: erase race condition on STG
 *
 * Hydra_Software_Devel/279   9/8/11 10:23p tdo
 * SW7425-922: VDC bringup for B0
 *
 * Hydra_Software_Devel/278   9/8/11 1:56a tdo
 * SW7425-979: Temporarity disable DAC auto-calibration and auto detection
 * feature for B0 bringup
 *
 * Hydra_Software_Devel/277   9/6/11 3:36p tdo
 * SW7425-979: Change init value for DAC setting for 7425B0/7346B0...
 *
 * Hydra_Software_Devel/276   8/31/11 11:49a syang
 * SW7425-1170: pxlAspRatio passed to ViCE2 is corrected to format x<<16 |
 * y
 *
 * Hydra_Software_Devel/274   8/30/11 3:56p syang
 * SW7425-1170: fix display 0 time out; clean up
 *
 * Hydra_Software_Devel/270   8/29/11 5:50p syang
 * SW7425-1170: add a dummy BVDC_P_Display_Apply_AspRatio_Setting
 *
 * Hydra_Software_Devel/269   8/29/11 10:22a tdo
 * SW7231-309: Keep at least one analog path correctly enabled in no DACs
 * case for hdmi slave to work properly.
 *
 * Hydra_Software_Devel/268   8/26/11 5:40p syang
 * SW7425-1170: merge from branch SW7572-1170
 *
 * Hydra_Software_Devel/SW7425-1170/3   8/26/11 4:28p syang
 * SW7425-1170: refactor pixel aspect ratio related code
 *
 * Hydra_Software_Devel/SW7425-1170/2   8/24/11 6:58p vanessah
 * SW7425-1170: ASP code clean
 *
 * Hydra_Software_Devel/SW7425-1170/1   8/23/11 6:21p vanessah
 * SW7425-1170: SAR cleanup
 *
 * Hydra_Software_Devel/267   8/25/11 3:56p pntruong
 * SW7425-1191: Rollback.  The acquire/release is more efficience to be
 * handle in nexus, where the done event is already known.
 *
 * Hydra_Software_Devel/266   8/25/11 9:22a vanessah
 * SW7425-1191:sg pwr management
 *
 * Hydra_Software_Devel/265   8/23/11 9:24p tdo
 * SW7231-309: Teardown/release resource for the correct analog path again
 * in case of missed RUL.
 *
 * Hydra_Software_Devel/264   8/23/11 12:15p tdo
 * SW7231-309: Do not need to keep analog resource around when analog
 * master and no DACs and no HDMI slave mode
 *
 * Hydra_Software_Devel/263   8/22/11 12:18p tdo
 * SW7231-309: handling vec resource when analog is in slave mode.
 *
 * Hydra_Software_Devel/262   8/21/11 5:36p hongtaoz
 * SW7425-592: fixed STG display eSAR x&y setting; corrected meta data SAR
 * X/Y bit fields assignment issue;
 *
 * Hydra_Software_Devel/261   8/18/11 11:36p vanessah
 * SW7425-1027: simplify aspect ratio
 *
 * Hydra_Software_Devel/260   8/16/11 11:01p tdo
 * SW7420-2030: VEC: Incorrect resource (dvi/656/etc) handling between
 * displays
 *
 * Hydra_Software_Devel/259   8/16/11 3:03p vanessah
 * SW7425-1068: MBox information validation
 *
 * Hydra_Software_Devel/258   8/12/11 2:57p vanessah
 * SW7425-835: correct stgid conception.
 *
 * Hydra_Software_Devel/257   8/12/11 12:16p hongtaoz
 * SW7425-835: fixed channel ID as 0 for regular transcode usage;
 *
 * Hydra_Software_Devel/256   8/10/11 3:01p vanessah
 * SW7425-923: add MBox revision id
 *
 * Hydra_Software_Devel/255   8/8/11 6:32p vanessah
 * SW7425-990: add And/or logic in STG trigger generation.
 *
 * Hydra_Software_Devel/254   8/8/11 1:42p vanessah
 * SW7425-448: add ViCE2 power management support
 *
 * Hydra_Software_Devel/253   8/5/11 8:34p albertl
 * SW7425-745: Added blanking to compositor color for all outputs and all
 * colorspaces.
 *
 * Hydra_Software_Devel/252   8/5/11 6:19p vanessah
 * SW7425-923: update ViCE2 MBox data structure definition and 2nd STG
 * bring up
 *
 * Hydra_Software_Devel/251   8/4/11 7:21p tdo
 * SW7425-979: Add support for 7425B0 DAC name change
 *
 * Hydra_Software_Devel/250   8/2/11 7:48p vanessah
 * SW7425-465: optimization of delay counter
 *
 * Hydra_Software_Devel/249   7/29/11 1:24p tdo
 * SW7422-194: Add SDSRC/HDSRC setting for calibration.
 *
 * Hydra_Software_Devel/248   7/28/11 2:15p tdo
 * SW7422-194: implement 40nm vDAC scaled voltage swing and auto-detection
 * to reduce power dissipation
 *
 * Hydra_Software_Devel/247   7/22/11 4:56p tdo
 * SW7422-194: implement 40nm vDAC scaled voltage swing and auto-detection
 * to reduce power dissipation
 *
 * Hydra_Software_Devel/246   7/19/11 6:43p vanessah
 * SW7425-923: stg register offset cleanup
 *
 * Hydra_Software_Devel/245   7/19/11 9:53a vanessah
 * SW7425-923: add B0 STG_1 interrupt
 *
 * Hydra_Software_Devel/244   7/18/11 5:33p pntruong
 * SW7231-284, SW7346-295, SW7405-5339: Refactored master framerate
 * tracking, so that it can also handle validation.
 *
 * Hydra_Software_Devel/243   7/18/11 11:27a vanessah
 * SW7425-835: SW7425-923: fix ViCE2 channel id bug + B0 STG
 *
 * Hydra_Software_Devel/242   7/11/11 2:19p tdo
 * SW7420-1971: Video Pause seen when VEC alignment is going on.  Add flag
 * to keep BVN connected while doing alignment.
 *
 * Hydra_Software_Devel/241   7/8/11 6:01p albertl
 * SW7408-291: Updated VF filter API to use Dac enums for channel
 * selection.
 *
 * Hydra_Software_Devel/240   7/7/11 2:55p vanessah
 * SW7425-787: set ignorepicture flag when no MFD input
 *
 * Hydra_Software_Devel/239   7/6/11 6:07p albertl
 * SW7408-291, SW7425-745: Changed VF filter API to also take a selected
 * display output as parameter.  Added selective muting of display
 * outputs using CSC.
 *
 * Hydra_Software_Devel/238   7/1/11 10:55a tdo
 * SW7422-194: Check back in the implementation for vDAC detection with
 * calibration temporarily disabled for now.
 *
 * Hydra_Software_Devel/237   7/1/11 1:05a hongtaoz
 * SW7425-784: en/disable timer trigger in the same RUL when in timer mode
 * to avoid extra timer triggerfires after switched into EOP mode;
 *
 * Hydra_Software_Devel/236   6/30/11 7:07p vanessah
 * SW7422-194: undo the change temporarily
 *
 * Hydra_Software_Devel/235   6/29/11 6:55p hongtaoz
 * SW7425-704, SW7425-785: only skip RM validation when display master is
 * STG;
 *
 * Hydra_Software_Devel/234   6/28/11 10:53p hongtaoz
 * SW7425-704, SW7425-785: remove STG sw_init on format switch; don't
 * validate STG frame rate against RM table;
 *
 * Hydra_Software_Devel/233   6/28/11 11:54a tdo
 * SW7422-194: implement 40nm vDAC scaled voltage swing and auto-detection
 * to reduce power dissipation
 *
 * Hydra_Software_Devel/231   6/27/11 8:23p pntruong
 * SW7425-785: BVN sent 60Hz frame rate metadata to ViCE2 at start of 480i
 * transcode.  Correctly initialize the default framerate to the main vdc
 * settings.
 *
 * Hydra_Software_Devel/230   6/24/11 7:01p albertl
 * SW7420-1966: Fixed bad merge.
 *
 * Hydra_Software_Devel/229   6/24/11 3:52p albertl
 * SW7420-1966: Cleaned up and reorganized colorspace enums and indicies
 * in display, vdec, and cmp.
 *
 * Hydra_Software_Devel/228   6/24/11 12:53p darnstein
 * SW7405-5081: Install new microcode for ARIB style 480I output.
 *
 * Hydra_Software_Devel/227   6/21/11 1:13p hongtaoz
 * SW7425-704: toggle between timer and eop trigger for NRT mode when
 * ignore flag toggles;
 *
 * Hydra_Software_Devel/226   6/19/11 11:00a vanessah
 * SW7425-686: check in stg change again
 *
 * Hydra_Software_Devel/225   6/17/11 4:13p vanessah
 * SW7425-29: Turn on STG trigger to ViCE always
 *
 * Hydra_Software_Devel/224   6/17/11 3:02p vanessah
 * SW7425-29: Adjust ViCE interrupt according to compositor status
 *
 * Hydra_Software_Devel/223   6/17/11 2:35p albertl
 * SW7408-291:  Added interface to set and get VF filter settings.
 *
 * Hydra_Software_Devel/221   6/10/11 5:47p hongtaoz
 * SW7425-704: add NRT transcode support to VDC; added bStallStc flag;
 *
 * Hydra_Software_Devel/220   6/9/11 1:17p tdo
 * SW7405-5339: BVDC_Display_Create doesn't recover from bad parameters.
 * Fix various bugs in error handling cases.
 *
 * Hydra_Software_Devel/219   6/8/11 2:55p pntruong
 * SW7346-276: Corrected populating the bandgap field.
 *
 * Hydra_Software_Devel/218   6/7/11 5:40p tdo
 * SW7346-237: More VEC programming bug fixes
 *
 * Hydra_Software_Devel/217   6/3/11 6:08p tdo
 * SW7346-237: More VEC resource management fine tuning
 *
 * Hydra_Software_Devel/216   6/3/11 6:01p tdo
 * SW7346-237: More VEC resource management fine tuning
 *
 * Hydra_Software_Devel/215   6/1/11 1:00p tdo
 * SW7346-237: Using both RGB and CVBS output issue.
 *
 * Hydra_Software_Devel/214   5/27/11 6:38p vanessah
 * SW7425-465: fix for transcoding transition black frame artifact
 *
 * Hydra_Software_Devel/213   5/11/11 12:48p tdo
 * SW7425-515: Add HW7425-475 sw work around to not reset SECAM_2
 *
 * Hydra_Software_Devel/212   4/21/11 9:19a vanessah
 * SW7425-46: remove frame rate msg
 *
 * Hydra_Software_Devel/211   4/19/11 11:38a hongtaoz
 * SW7425-148: fixed 480i frame rate setting for STG display;
 *
 * Hydra_Software_Devel/210   3/26/11 12:07a tdo
 * SW7125-248: Enhance analog channel resource management
 *
 * Hydra_Software_Devel/209   3/25/11 2:35p yuxiaz
 * SW7420-1741: Added game mode delay support.
 *
 * Hydra_Software_Devel/208   3/25/11 11:13a albertl
 * SW7550-701: Removed leftover source disconnect code for synconly that
 * should no longer be done.  Was preventing hotplug from working.
 *
 * Hydra_Software_Devel/207   3/24/11 8:10p tdo
 * SW7420-1638: Assign correct Output CSC for each analog chan
 *
 * Hydra_Software_Devel/206   3/24/11 7:30p tdo
 * SW7420-1638: Reprogram Analog Chan 0 and 1 independently when DAC dirty
 *
 * Hydra_Software_Devel/205   3/24/11 2:00p pntruong
 * SW7425-243: Enabled stg's down sample.
 *
 * Hydra_Software_Devel/204   3/24/11 11:26a tdo
 * SW7422-124: Program correct BVB SIZE for 3D formats
 *
 * Hydra_Software_Devel/203   3/21/11 1:42p tdo
 * SW7420-1638: DACs do not get powered down properly with certain call
 * sequences
 *
 * Hydra_Software_Devel/202   3/17/11 6:10p tdo
 * SW7422-391: Incorrect VertRefreshRate return and add support for PAL 60
 * to allow multi rate
 *
 * Hydra_Software_Devel/201   3/7/11 4:32p albertl
 * SW7125-832: Fixed proper setting of bAlighment bit to Dirty when time
 * stamps are invalid.
 *
 * Hydra_Software_Devel/200   2/28/11 11:16a vanessah
 * SW7425-50: SRC frame rate tracking
 *
 * Hydra_Software_Devel/199   2/27/11 8:08p hongtaoz
 * SW7425-50: fixed frame rate tracking;
 *
 * Hydra_Software_Devel/198   2/25/11 5:52p vanessah
 * SW7425-50: frame rate tracking
 *
 * Hydra_Software_Devel/197   2/7/11 5:50p vanessah
 * SW7425-46: STG-Mbox programming
 *
 * Hydra_Software_Devel/196   2/7/11 5:24p vanessah
 * SW7425-46: add MFD mute to the skip picture to start quality comparison
 *
 * Hydra_Software_Devel/195   2/7/11 2:06p darnstein
 * SW7405-5081: Implement ARIB option for NTSC.
 *
 * Hydra_Software_Devel/194   2/3/11 5:33p vanessah
 * SW7425-29:  non-real-time ViCE2 support
 *
 * Hydra_Software_Devel/193   2/1/11 4:21p pntruong
 * SW7125-792: Also update the digital blank color padding.
 *
 * Hydra_Software_Devel/192   2/1/11 4:18p pntruong
 * SW7550-645: Updated with new optimal filter values from bringup
 * scripts.
 *
 * Hydra_Software_Devel/191   1/25/11 3:07p vanessah
 * SW7425-46:
 * PAR info for ViCE2 MBox metadata
 *
 * Hydra_Software_Devel/190   1/20/11 5:00p tdo
 * SW7125-792: Properly handle HDMI sync only
 *
 * Hydra_Software_Devel/188   1/20/11 2:42p yuxiaz
 * SW7422-97: Clean up macros for HDMI Rate manage
 *
 * Hydra_Software_Devel/187   1/17/11 4:55p yuxiaz
 * SW7422-97: Added KP, KI and KA settings on 7422.
 *
 * Hydra_Software_Devel/186   1/17/11 1:49p pntruong
 * SW7550-645: Add GRPD support in VEC.
 *
 * Hydra_Software_Devel/185   1/12/11 10:01a vanessah
 * SW7425-32:  Change IT CFG value for 7425
 *
 * Hydra_Software_Devel/184   1/7/11 5:14p tdo
 * SW7125-792: Properly handle HDMI sync only
 *
 * Hydra_Software_Devel/183   1/3/11 4:07p vanessah
 * SW7425-29:
 * STG fix SAR for CES 2011
 *
 * Hydra_Software_Devel/182   1/3/11 1:27p vanessah
 * SW7425-29:
 * STG fix SAR for CES 2011
 *
 * Hydra_Software_Devel/181   12/21/10 11:04a tdo
 * SW7422-28: Adjust to 3D format size change
 *
 * Hydra_Software_Devel/180   12/15/10 1:37p pntruong
 * SW7231-2: Removed more BCHP_CHIP spot fixes.
 *
 * Hydra_Software_Devel/179   12/15/10 10:06a tdo
 * SW7422-44: Undo previous check in
 *
 * Hydra_Software_Devel/178   12/15/10 9:27a tdo
 * SW7422-44: Set HDMI_FIFO_CTL.MASTER_OR_SLAVE_N for HDMI master mode
 *
 * Hydra_Software_Devel/177   12/14/10 5:33p pntruong
 * SW7231-2: Removed BCHP_CHIP spot fixes.  Programmed bandgap adjustment
 * number from API.  Programmed power management for bandgap power down.
 * Need to add for individual dac.
 *
 * Hydra_Software_Devel/176   12/14/10 2:50p darnstein
 * SW7231-2: apply "dark colors" work around from 7422 to 7344 and 7346.
 *
 * Hydra_Software_Devel/175   12/10/10 11:36a vanessah
 * SW7425-50:  STG Display Dynamic Format Switch Issue
 *
 * Hydra_Software_Devel/174   12/9/10 5:52p tdo
 * SW7422-44: Bring up 3D full resolution formats
 *
 * Hydra_Software_Devel/173   12/7/10 2:24p tdo
 * SW7422-44: Correctly set AS control in 3D mode
 *
 * Hydra_Software_Devel/172   12/3/10 5:30p albertl
 * SW7325-825: Changed GROSS_LUMA_DELAY and FINE_SC_DELAY for SECAM.
 *
 * Hydra_Software_Devel/171   12/1/10 6:12p tdo
 * SW7422-44: Handle 3D changes for display
 *
 * Hydra_Software_Devel/170   11/29/10 11:22a hongtaoz
 * SW7425-46: corrected STG picture polarity;
 *
 * Hydra_Software_Devel/169   11/19/10 6:23p darnstein
 * SW7231-2: add support for 7344, 7346, 7231.
 *
 * Hydra_Software_Devel/168   11/19/10 5:01p hongtaoz
 * SW7425-29: resynced 7422 ViCE RDB;
 *
 * Hydra_Software_Devel/167   11/18/10 4:01p vanessah
 * SW7425-29:  STG support for 7425 & 7422
 *
 * Hydra_Software_Devel/166   11/18/10 3:24p vanessah
 * SW7425-29:  STG support for 7425 & 7422
 *
 * Hydra_Software_Devel/165   11/18/10 12:32p tdo
 * SW7422-92: Add function to set display orientation.  Add new dirty bit
 * to handle 3D setting
 *
 * Hydra_Software_Devel/164   11/18/10 11:14a hongtaoz
 * SW7422-13, SW7425-29: fixed DVI/656 out sw reset sequence;
 *
 * Hydra_Software_Devel/163   11/18/10 10:48a vanessah
 * SW7425-29:  STG back off for 7422 compile
 *
 * Hydra_Software_Devel/162   11/18/10 10:19a pntruong
 * SW7422-13, SW7425-29: Fixed build errors. Removed extra whitespaces.
 *
 * Hydra_Software_Devel/161   11/18/10 9:53a vanessah
 * SW7425-29:  STG Mbox macro update
 *
 * Hydra_Software_Devel/160   11/18/10 12:06a hongtaoz
 * SW7422-13, SW7425-29: bring up dtg master mode on/off toggle; make
 * display switch mode more robust; defer DTG reset from disable to
 * enable to avoid timeout; bring up 656out;
 *
 * Hydra_Software_Devel/159   11/16/10 6:52p tdo
 * SW7422-92: Need display 3D orientation override function for VDC
 *
 * Hydra_Software_Devel/158   11/16/10 5:42p yuxiaz
 * SW7422-97: Program HDMI_TX_PHY_MDIV_LOAD for PLL and rate manager to
 * work correctly.
 *
 * Hydra_Software_Devel/157   11/16/10 4:07p yuxiaz
 * SW7422-97: Updated basic HDMI TX rate manager settings for 40nm chips.
 *
 * Hydra_Software_Devel/156   11/10/10 11:43a tdo
 * SW7422-13: 7422 VEC bringup
 *
 * Hydra_Software_Devel/155   11/8/10 1:29p nilesh
 * SW7425-1: Updated VICE2 Mailbox Register Address to match updated VCE
 * FW
 *
 * Hydra_Software_Devel/154   11/4/10 6:00p vanessah
 * SW7425-29:   updated ViCE2 meta-data order;
 *
 * Hydra_Software_Devel/153   11/3/10 1:43p tdo
 * SW7422-44: Handle 3D changes for display
 *
 * Hydra_Software_Devel/152   10/28/10 5:40p hongtaoz
 * SW7425-29: updated ViCE2 meta-data starting address;
 *
 * Hydra_Software_Devel/151   10/27/10 12:02p vanessah
 * SW7425-46:  STG Aspect ratio
 *
 * Hydra_Software_Devel/150   10/27/10 9:20a vanessah
 * SW7425-46:  Digital resolution instead of the analog one
 *
 * Hydra_Software_Devel/149   10/25/10 7:30p vanessah
 * SW7425-46: SW7425-29:  fixed mailbox meta-data RUL command
 *
 * Hydra_Software_Devel/148   10/25/10 6:09p hongtaoz
 * SW7425-46, SW7425-29: fixed mailbox meta-data RUL command;
 *
 * Hydra_Software_Devel/147   10/22/10 7:04p vanessah
 * SW7425-46:  ViCE2 meta data mailbox register
 *
 * Hydra_Software_Devel/146   10/19/10 6:01p hongtaoz
 * SW7425-29: moved STG_CONTROL programming after FRAME_SIZE to prevent
 * BVB freeze;
 *
 * Hydra_Software_Devel/145   10/11/10 2:29p pntruong
 * SW7550-577: Fixed typo for digital master mode control bit.
 *
 * Hydra_Software_Devel/144   10/4/10 10:26a hongtaoz
 * SW7425-29: fixed coverity defect;
 *
 * Hydra_Software_Devel/143   10/2/10 1:33a hongtaoz
 * SW7425-29: adding STG support;
 *
 * Hydra_Software_Devel/142   9/28/10 11:41p tdo
 * SW7550-561: fix coverity
 *
 * Hydra_Software_Devel/141   9/28/10 7:30p tdo
 * SW7550-561: Remove false error message when query for resource
 *
 * Hydra_Software_Devel/140   9/23/10 10:34a vanessah
 * SW7425-29:  STG support for transcoding
 *
 * Hydra_Software_Devel/139   9/13/10 4:43p yuxiaz
 * SW7358-4: Added initial VDC support for 7358.
 *
 * Hydra_Software_Devel/138   9/9/10 3:19p rpan
 * SW7550-561: Improved VEC resource management error messages.
 *
 * Hydra_Software_Devel/137   8/30/10 4:15p tdo
 * SW7425-11, SW7425-13: Add 7425 support for VDC
 *
 * Hydra_Software_Devel/136   8/26/10 5:29p tdo
 * SW7422-57: Add simple vdc support
 *
 * Hydra_Software_Devel/135   8/24/10 3:53p tdo
 * SW7420-978: BVB_SAV_REMOVE is set to 3 with 704 microcode.  Clean up
 * duplicated logic.
 *
 * Hydra_Software_Devel/134   7/26/10 2:58p rpan
 * SW7420-811: Program register MISC_RFM_0_SYNC_0_CFG.
 *
 * Hydra_Software_Devel/133   7/14/10 8:14p albertl
 * SW7550-474, SW7420-542:  Rewrote some table handling code and corrected
 * VF settings for 54Mhz 480p in RGB.
 *
 * Hydra_Software_Devel/132   7/14/10 4:02p rpan
 * SW7468-282: Fixed how S-Video DACs connect to SDSRC channels.
 *
 * Hydra_Software_Devel/131   6/18/10 4:26p rpan
 * SW7400-2763: New BVN resource management.
 *
 * Hydra_Software_Devel/130   6/16/10 6:34p tdo
 * SW7400-2770: Add multi-frame rate support to SD also
 *
 * Hydra_Software_Devel/129   6/7/10 4:46p tdo
 * SW7208-57, SW7125-248: VEC resource management fine tuning
 *
 * Hydra_Software_Devel/128   5/24/10 1:35p rpan
 * SW7208-57: Added VEC modulo count trigger support for DVI master mode.
 *
 * Hydra_Software_Devel/127   5/17/10 5:15p yuxiaz
 * SW7405-4339:  Inconsistent selection of DECIMATE_RATIO based on video
 * format.
 *
 * Hydra_Software_Devel/126   5/13/10 3:35p rpan
 * SW7420-738: Fixed the VEC alignment start criteria. Take the scenario
 * that the two VEC's phases are exactly opposite into count.
 *
 * Hydra_Software_Devel/125   5/11/10 3:17p darnstein
 * SW7340-174: DVI_DTG_0_DTG_CONFIG.AUTO_RESTART only depends on
 * DVI_DTG_0_DTG_CONFIG.SLAVE_MODE now.
 *
 * Hydra_Software_Devel/124   5/7/10 7:22p albertl
 * SW7125-364: Changed dirty bits to use union structure to avoid type-pun
 * warnings
 *
 * Hydra_Software_Devel/123   4/22/10 4:41p rpan
 * SW7340-168: Error messages improvements.
 *
 * Hydra_Software_Devel/122   4/22/10 1:39p rpan
 * SW7208-6: Set format switch marker only if bTiming dirty bit is on.
 *
 * Hydra_Software_Devel/121   4/19/10 10:28p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic. Move
 * BDBG_OBJECT_ID_DECLARE private header files instead of .c.
 *
 * Hydra_Software_Devel/120   4/16/10 2:36p darnstein
 * SW7420-543: get 4x pixel replication working for 480P.
 *
 * Hydra_Software_Devel/119   4/14/10 10:53p tdo
 * SW7420-673: Reprogram RM when DAC changes
 *
 * Hydra_Software_Devel/118   4/7/10 5:11p darnstein
 * SW7420-543: fix previous check in.
 *
 * Hydra_Software_Devel/117   4/7/10 5:07p darnstein
 * SW7420-543: fix previous check in.
 *
 * Hydra_Software_Devel/116   4/7/10 4:32p darnstein
 * SW7420-543: fix previous check in.
 *
 * Hydra_Software_Devel/115   4/7/10 4:25p darnstein
 * SW7420-543: Analog 480P output is always sampled at 54Mhz.
 *
 * Hydra_Software_Devel/114   4/7/10 11:41a tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic.  Rename TLA
 *
 * Hydra_Software_Devel/113   4/5/10 4:26p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic
 *
 * Hydra_Software_Devel/112   3/30/10 7:46p albertl
 * SW7550-351: Removed handling of BCHP_DVI_DVF_X_DVI_DVF registers now
 * that names are corrected in affected chips.
 *
 * Hydra_Software_Devel/111   3/24/10 4:30p darnstein
 * SW7420-167: work in version Hydra_Software_Devel/109 was incomplete.
 *
 * Hydra_Software_Devel/110   3/22/10 5:24p tdo
 * SW7420-617: DAC Hsync output mode needs to be implemented
 *
 * Hydra_Software_Devel/109   3/19/10 3:41p darnstein
 * SW7420-167: Set DVI-DTG toggle bits to 1 whenever 480P VEC IT microcode
 * is loaded.
 *
 * Hydra_Software_Devel/108   3/5/10 10:14a rpan
 * SW7420-602: Fixed BCHP_VF_0_NEG_SYNC_VALUES programming for VFs other
 * than VF_0.
 *
 * Hydra_Software_Devel/107   2/9/10 2:10p rpan
 * SW7400-2642: Added SECAM output for orthogonal VEC.
 *
 * Hydra_Software_Devel/106   2/4/10 4:46p tdo
 * SW7420-568: Reprogram RFM when switching between analog chan 0 and 1
 *
 * Hydra_Software_Devel/105   1/29/10 8:34p albertl
 * SW7405-3845: Fixed PAL_NC matrices and renamed associated colorspace as
 * YUV_NC.
 *
 * Hydra_Software_Devel/104   1/21/10 3:34p syang
 * SW7550-201: init dac bg and each dac as power down when bvdc is opened;
 * ensure dac bg power down when no dac in the group is used
 *
 * Hydra_Software_Devel/103   1/15/10 2:00p pntruong
 * SW7408-61: Need to program the timebase register for digital path as
 * well when timebase setting is changed.
 *
 * Hydra_Software_Devel/102   1/6/10 3:41p rpan
 * SW7468-64: Consolidate VEC modulo count trigger implementation.
 *
 * Hydra_Software_Devel/101   12/29/09 1:05p syang
 * SW7408-13:  add VEC_TRIGGER MODULO_COUNT setting for 7408
 *
 * Hydra_Software_Devel/100   12/22/09 12:22p syang
 * SW7550-143: add conditional place holder in DisplayEventHndlTbl for
 * coverity check
 *
 * Hydra_Software_Devel/99   12/21/09 1:36p tdo
 * SW7420-514: Remove overrun of static array coverity warning
 *
 * Hydra_Software_Devel/98   12/21/09 1:13p tdo
 * SW7420-514: Remove Coverity warnings in VDC
 *
 * Hydra_Software_Devel/97   12/16/09 3:04p tdo
 * SW7342-126: set DACs to UNUSED when turned off
 *
 * Hydra_Software_Devel/96   12/15/09 1:57p rpan
 * SW7468-30: Brought up 1080p output on 7468.
 *
 * Hydra_Software_Devel/95   12/14/09 1:37p rpan
 * SW7468-30: Fixed 7468 bottom trigger select.
 *
 * Hydra_Software_Devel/94   12/3/09 2:44p rpan
 * SW7342-120: Make sure display eItState is eNotActive or eSwitchMode
 * when building RUL to reset VEC.
 *
 * Hydra_Software_Devel/93   11/30/09 10:35a rpan
 * SW7468-30: Initial OSCL support.
 *
 * Hydra_Software_Devel/92   11/13/09 1:00p darnstein
 * SW7400-2607,SW7405-3132: New analog and digital VEC IT microcode for
 * PAL-M.
 *
 * Hydra_Software_Devel/91   11/11/09 3:57p darnstein
 * SW7405-3132: Set LINE_REMOVE_TOP=1 for PAL-M. This might need to be
 * revisited.
 *
 * Hydra_Software_Devel/90   10/23/09 6:51p darnstein
 * SW7340-45: some logic related to custom Macrovision output was not
 * ported over to the orthogonal VEC software.
 *
 * Hydra_Software_Devel/89   10/13/09 6:03p darnstein
 * SW7405-3004: install 480P ARIB output via BVDC display settings.
 *
 * Hydra_Software_Devel/88   10/1/09 10:59p pntruong
 * SW7420-367: Fixed coverity static overrun warning.
 *
 * Hydra_Software_Devel/87   9/24/09 7:25p tdo
 * SW7125-27: Correctly disable Chan1 when VEC resource for Chan1 is
 * released
 *
 * Hydra_Software_Devel/86   9/9/09 9:24a rpan
 * SW7342-38: Fixed dirty bit handling for big endian environment.
 *
 * Hydra_Software_Devel/85   8/25/09 11:41a rpan
 * SW7420-324: Enhance resource management for DVI and 656. Take the RUL
 * not executed scenario into consideration.
 *
 * Hydra_Software_Devel/84   8/24/09 3:57p rpan
 * SW7342-27: Make analog channel 1 data structure and programming model
 * consistent with channel 0. Enhance channel 1 handling state machine.
 *
 * Hydra_Software_Devel/83   8/24/09 9:38a tdo
 * SW7342-27: Enhance DAC resource management
 *
 * Hydra_Software_Devel/82   8/21/09 5:38p rpan
 * PR57873: Enhanced analog channel 1 resource management. Take the case
 * that RUL doesn't get executed into consideration.
 *
 * Hydra_Software_Devel/81   8/21/09 3:57p tdo
 * PR57734: Remove coverity error
 *
 * Hydra_Software_Devel/80   8/21/09 2:35p tdo
 * PR57734: Add capability for display to handle DACs re-assignment
 *
 * Hydra_Software_Devel/79   8/21/09 10:37a rpan
 * PR57873: Temporarily disable an assertion.
 *
 * Hydra_Software_Devel/78   8/13/09 9:50a tdo
 * PR55956: Add support for 7420B0
 *
 * Hydra_Software_Devel/77   8/4/09 5:06p tdo
 * PR57352: Fix issue of uninitialized variables in non-debug mode
 *
 * Hydra_Software_Devel/76   6/25/09 12:14p rpan
 * PR56137, PR56138, PR56139, PR56166, PR56167, PR56168: Support for
 * various orthogonal VEC configurations.
 *
 * Hydra_Software_Devel/75   6/23/09 2:40p darnstein
 * PR56245: fix a BDBG_MSG statement. Minor.
 *
 * Hydra_Software_Devel/74   6/22/09 3:30p darnstein
 * PR55219: eliminate a compiler warning. Minor.
 *
 * Hydra_Software_Devel/73   6/22/09 3:12p darnstein
 * PR55225: mechanical changes to support 7342 chipset.
 *
 * Hydra_Software_Devel/72   6/19/09 3:59p darnstein
 * PR55219: adapt to varying number of compositors, and to
 * presence/absence of ITU-R 656 digital video output.
 *
 * Hydra_Software_Devel/71   6/16/09 8:54p tdo
 * PR55956: Add VDC support for 7420B0.
 *
 * Hydra_Software_Devel/70   6/12/09 6:11p darnstein
 * PR55997: No-op instruction address for VEC IT microcode is now 253.
 *
 * Hydra_Software_Devel/69   6/9/09 3:41p rpan
 * PR55613, PR55643: 1) Each user display settings now generates dirty
 * bit(s). This simplifies how invalid user settings can be cleaned. 2)
 * Moved HDMI and ITU-656 validation to per display basis. 3) Fixed the
 * ITU-656 not enabled in slave mode issue.
 *
 * Hydra_Software_Devel/68   6/2/09 3:22p darnstein
 * PR48884: More resistance to invalid user inputs.
 *
 * Hydra_Software_Devel/67   6/1/09 2:14p darnstein
 * PR48884: prevent more crashes when user specifies bad video
 * configuration on VEC. These changes specifically support 7420.
 *
 * Hydra_Software_Devel/66   5/28/09 6:04p darnstein
 * PR48884: BVDC_P_GetRmTable_isr() no longer modifies its DisplayInfo
 * argument. Instead, it returns a new agrument with the modified data.
 * There are two consequences:
 * 1. When BVDC_P_GetRmTable_isr() is called, a DisplayInfo may have to be
 * modified by the caller.
 * 2. BVDC_P_HdmiRmTableEx_isr() gets a new argument, the same new
 * argument as BVDC_P_GetRmTable_isr().
 *
 * Hydra_Software_Devel/65   5/1/09 4:56p tdo
 * PR54778: make sure the blank color is correct for RGB and YCbCr format.
 *
 * Hydra_Software_Devel/64   4/30/09 4:33p albertl
 * PR54567: Fixed build error.
 *
 * Hydra_Software_Devel/63   4/30/09 3:13p albertl
 * PR54593: Fixed Set and GetDvoMatrix to take and return matrices in
 * YCbCr->RGB and YCbCr->YCbCr format and convert to hardware format
 * internally.
 *
 * Hydra_Software_Devel/62   4/24/09 3:25p rpan
 * PR54567: Removed the policy of which video formats allow decimation.
 * Let VDC PI caller enforce it.
 *
 * Hydra_Software_Devel/61   4/24/09 12:01a tdo
 * PR53070: Fix segfault in brutus run
 *
 * Hydra_Software_Devel/60   4/21/09 4:07p rpan
 * PR53070: Added more VEC alignment options for memory reduction.
 *
 * Hydra_Software_Devel/59   4/16/09 12:53p rpan
 * PR45807: RM_SKIP_REPEAT_CONTROL.MODE has SKIP and REPEAT swapped.
 *
 * Hydra_Software_Devel/58   4/8/09 1:59p tdo
 * PR53373: Add HDMI deep color support
 *
 * Hydra_Software_Devel/57   4/7/09 9:58a tdo
 * PR53373: Add HDMI deep color support
 *
 * Hydra_Software_Devel/56   3/31/09 3:19p rpan
 * PR53156: Implemented a non-reset version 704<->720 display format
 * switch.
 *
 * Hydra_Software_Devel/55   3/18/09 7:33p darnstein
 * PR51527: DCS logic in place and mostly working.
 *
 * Hydra_Software_Devel/54   3/17/09 11:55a darnstein
 * PR51527: Late merge of Macrovision software.
 *
 * Hydra_Software_Devel/53   3/16/09 10:57p tdo
 * PR45785, PR45789: Merge from MCVP branch
 *
 * Hydra_Software_Devel/7420_mcvp/4   3/13/09 5:52p darnstein
 * PR51527: Macrovision working now, but it reloads microcode for each
 * change. So more work remains.
 *
 * Hydra_Software_Devel/7420_mcvp/3   3/6/09 1:41p pntruong
 * PR52750: Set HDM_RM_FORMAT STABLE_COUNT field.
 *
 * Hydra_Software_Devel/7420_mcvp/2   3/4/09 5:25p tdo
 * PR45785, PR45789: merge from main branch on 3/04/09
 *
 * Hydra_Software_Devel/51   3/4/09 10:59a rpan
 * PR45804: Add BVDC_P_ prefix to display handler table variable name.
 *
 * Hydra_Software_Devel/7420_mcvp/1   3/3/09 7:26p darnstein
 * PR51527: Eliminate unused state variables eVbiPath and eVecPath. The
 * DCS files compile now, although they need a lot of work to acutally
 * function.
 *
 * Hydra_Software_Devel/50   2/17/09 2:33p rpan
 * PR52001: Added 7420 DAC connection state for power management.
 *
 * Hydra_Software_Devel/49   2/13/09 9:39a rpan
 * PR52100: Got the user registered callback mechanism working.
 *
 * Hydra_Software_Devel/48   2/11/09 9:59a rpan
 * PR45804: Added an VEC alignment threshold.
 *
 * Hydra_Software_Devel/47   2/10/09 4:47p rpan
 * PR52001: Added 7420 DAC power management.
 *
 * Hydra_Software_Devel/46   2/9/09 3:35p rpan
 * PR45804: Some clean-up work.
 *
 * Hydra_Software_Devel/45   2/9/09 12:08p rpan
 * PR51599: Updated HDMI PLL settings.
 *
 * Hydra_Software_Devel/44   2/6/09 3:55p rpan
 * PR45804: Enabled dither.
 *
 * Hydra_Software_Devel/43   2/6/09 2:52p rpan
 * PR45804: Added HDMI sync-only support.
 *
 * Hydra_Software_Devel/42   2/5/09 4:42p rpan
 * PR45804: Fixed a HdmiXvYcc iimplementation bug.
 *
 * Hydra_Software_Devel/41   2/5/09 11:12a rpan
 * PR45804: More work for seamless attachment of a slave path.
 *
 * Hydra_Software_Devel/40   2/5/09 8:50a rpan
 * PR45804: Added the initial support for Hdmi XvYcc.
 *
 * Hydra_Software_Devel/39   2/4/09 3:44p rpan
 * PR45804: Added HDMI drop-line support.
 *
 * Hydra_Software_Devel/38   2/3/09 3:53p rpan
 * PR45804: Added width trimming support.
 *
 * Hydra_Software_Devel/37   2/3/09 2:33p rpan
 * PR45804: Set DVI and 656 resource ID to invalid after being released.
 *
 * Hydra_Software_Devel/36   2/3/09 12:03p rpan
 * PR45804: Added DVI decimation support.
 *
 * Hydra_Software_Devel/35   2/3/09 9:38a rpan
 * PR45804: Implemented another scheme of attaching DVI slave path to
 * analog master path, which resets master path.
 *
 * Hydra_Software_Devel/34   1/30/09 3:04p rpan
 * PR45807: Restart VEC alignment process in case timestamp taken is not
 * valid.
 *
 * Hydra_Software_Devel/33   1/29/09 4:49p rpan
 * PR45804: Added support for alignment.
 *
 * Hydra_Software_Devel/32   1/27/09 3:18p rpan
 * PR45804: Set master DVI and ITU656 timing generator to free-run.
 *
 * Hydra_Software_Devel/31   1/26/09 4:21p rpan
 * PR45804: Initial support for ITU656 output.
 *
 * Hydra_Software_Devel/30   1/23/09 5:53p rpan
 * PR45804: Changes for HDMI slave mode to work.
 *
 * Hydra_Software_Devel/29   1/21/09 4:01p rpan
 * PR45804: DVI master mode work.
 *
 * Hydra_Software_Devel/28   1/21/09 10:14a rpan
 * PR45804: More HDMI output work.
 *
 * Hydra_Software_Devel/27   1/20/09 9:12a rpan
 * PR45804: Initial HDMI support  work.
 *
 * Hydra_Software_Devel/26   1/12/09 4:07p rpan
 * PR45804: Added initial RFM output support.
 *
 * Hydra_Software_Devel/25   1/9/09 2:17p rpan
 * PR45804: Added component path MPAA support.
 *
 * Hydra_Software_Devel/24   12/23/08 2:16p rpan
 * PR45804: Implemented force frame drop handlers.
 *
 * Hydra_Software_Devel/23   12/23/08 11:44a rpan
 * PR50391: Moved VF, CSC, SRC and SM programming to DAC event handler.
 *
 * Hydra_Software_Devel/22   12/23/08 10:00a rpan
 * PR45804: Complete handlers for stream source related events.
 *
 * Hydra_Software_Devel/21   12/22/08 10:33p tdo
 * PR50391: Force Timing dirty when Dac dirty
 *
 * Hydra_Software_Devel/20   12/22/08 10:12p rpan
 * PR45804: Implemented handlers for source stream related events.
 *
 * Hydra_Software_Devel/19   12/22/08 3:19p rpan
 * PR45804: Added support for display callback.
 *
 * Hydra_Software_Devel/18   12/19/08 4:35p rpan
 * PR50391: Implemented set timebase support.
 *
 * Hydra_Software_Devel/17   12/19/08 1:59p rpan
 * PR50391: Fixed a cut-n-paste error for display 2 IT source.
 *
 * Hydra_Software_Devel/16   12/19/08 12:11p rpan
 * PR50391: Diable analog chan 1. Also not use Secam_0.
 *
 * Hydra_Software_Devel/15   12/18/08 6:49p rpan
 * PR50391: Changes to make sure format switch RUL can always be executed.
 *
 * Hydra_Software_Devel/14   12/18/08 11:39a rpan
 * PR50391: Always take the cores out of reset state.
 *
 * Hydra_Software_Devel/13   12/17/08 9:31p rpan
 * PR50391: Some resource management enhancement.
 *
 * Hydra_Software_Devel/12   12/17/08 7:53p tdo
 * PR50391: Add support for Analog Chan 1
 *
 * Hydra_Software_Devel/11   12/16/08 7:24p tdo
 * PR50391: Basic VDC bringup
 *
 * Hydra_Software_Devel/10   12/15/08 10:23p tdo
 * PR50391: Basic VDC bringup
 *
 * Hydra_Software_Devel/9   12/9/08 10:23p tdo
 * PR45804: Add Dac support
 *
 * Hydra_Software_Devel/8   12/9/08 4:46p rpan
 * PR45804: More orthogonal VEC support.
 *
 * Hydra_Software_Devel/7   12/9/08 2:27p rpan
 * PR45804: Implemented IT micro-controller enable function.
 *
 * Hydra_Software_Devel/6   12/9/08 11:00a rpan
 * PR50095: Added VF envelop generator programming.
 *
 * Hydra_Software_Devel/5   12/8/08 5:44p rpan
 * PR45804: Switched to VDC generic resource management lib.
 *
 * Hydra_Software_Devel/4   12/5/08 3:52p rpan
 * PR45804: Cleaned up some tabs in the code.
 *
 * Hydra_Software_Devel/3   12/4/08 3:47p rpan
 * PR45804: Made changes from source follow the new event driven sheme.
 *
 * Hydra_Software_Devel/2   12/3/08 10:10p rpan
 * PR45804: More orthogonal VEC support.
 *
 * Hydra_Software_Devel/1   12/3/08 5:03p rpan
 * PR45804: Initial effort for the orthogonal VEC support.
 *
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
#include "bchp.h"
#include "bvdc_compositor_priv.h"
#include "bvdc_displayfmt_priv.h"
#include "bvdc_common_priv.h"
#include "bvdc_display_priv.h"
#include "bchp_common.h"

#if (BVDC_P_SUPPORT_VEC_GRPD)
#include "bchp_grpd_0.h"
#endif

#ifdef BCHP_PWR_SUPPORT
#include "bchp_pwr.h"
#endif
#define BVDC_P_VF_SAV_RPLCT              6
#define BVDC_P_VF_EAV_PRDCT              3
#define BVDC_P_VF_THRESH                 1  /* HW reset value */
#define BVDC_P_VF_ENABLE                 1  /* HW reset value */

#if (BCHP_CHIP == 7422) || ((BCHP_CHIP == 7425) && (BCHP_VER < BCHP_VER_B0))
#define BVDC_P_VEC_HW7425_475_WORK_AROUND  1
#else
#define BVDC_P_VEC_HW7425_475_WORK_AROUND  0
#endif

BDBG_MODULE(BVDC_DISP);


/****************************************************************
 *                   VEC Helper Functions                       *
 ****************************************************************/
#if (BVDC_P_SUPPORT_VEC_GRPD)
#define BVDC_P_DLAY_FILTER_COUNT BVDC_P_REGS_ENTRIES(GRPD_0_GRP00,  GRPD_0_GRP19 )
#define BVDC_P_TRAP_FILTER_COUNT BVDC_P_REGS_ENTRIES(GRPD_0_TRAP00, GRPD_0_TRAP23)

static const struct
{
	uint32_t aulDlay[BVDC_P_DLAY_FILTER_COUNT];
	uint32_t aulTrap[BVDC_P_TRAP_FILTER_COUNT];

} s_aulGrpdFilters[2] =
{
	/* NTSC */
	{
		{
			0x00000180, 0xf8d010e0, 0xe2202920, 0xd6601650, 0x0a80e140,
			0x10c011a0, 0xe1e0fd80, 0x3e904110, 0x1600fbb0, 0xfbc00110,
			0x01a0ffc0, 0xff200000, 0x00800010, 0xffc00010, 0x00500020,
			0xfff0ffe0, 0x00000000, 0x00000000, 0x00000000, 0x00000000
		},
		{
			0xFFF40014, 0xFFF8FFF4, 0x00180000, 0xFFE4001C, 0x0010FFD0,
			0x00140030, 0xFFC0FFF8, 0x0058FFC0, 0xFFC8007C, 0xFFE0FF80,
			0x00900028, 0xFF300074, 0x00A0FEF4, 0x00180134, 0xFEF0FF68,
			0x01C4FF50, 0xFE68021C, 0x0044FD30, 0x01F401F4, 0xFBE800DC,
			0x04B8FAC8, 0xFDD009FC, 0xFA04F254, 0x25F44F14,
		}
	},

	/* PAL */
	{
		{
			0xffc0ffb0, 0x01f0fa80, 0x0b10ee50, 0x1710e7c0, 0x1310f820,
			0xfab00e90, 0xf21003e0, 0x0ba0eeb0, 0x09101480, 0xe9401180,
			0x5a602840, 0xf9e0f530, 0xfcd00050, 0x0130fc60, 0x0560f930,
			0x0680fb20, 0x02a0ff60, 0xff200180, 0xfe700110, 0xff500040,
		},
		{
			0x000c0004, 0xffec0028, 0xffc4003c, 0xffd80000, 0x002cffb8,
			0x0044ffe8, 0xffd80064, 0xff8c004c, 0x000cff90, 0x00b0ff64,
			0x00340064, 0xff1c0100, 0xff64ffd4, 0x0100fe88, 0x013cffb0,
			0xff0c01f4, 0xfde4012c, 0x0098fd98, 0x035cfd4c, 0x006402cc,
			0xfa9405e8, 0xfce0fcf4, 0x0b74ebf0, 0x1a806320,
		}
	},
};
#endif /* (BVDC_P_SUPPORT_VEC_GRPD) */

/* make sure matches with BAVC_MatrixCoefficients */
static const struct
{
	BAVC_MatrixCoefficients eAvcCs;
	char *pcAvcCsStr;

} stAVC_MatrixCoefficient_InfoTbl[] =
{
	{BAVC_MatrixCoefficients_eHdmi_RGB,           "BAVC_MatrixCoefficients_eHdmi_RGB"},
	{BAVC_MatrixCoefficients_eItu_R_BT_709,       "BAVC_MatrixCoefficients_eItu_R_BT_709"},
	{BAVC_MatrixCoefficients_eUnknown,            "BAVC_MatrixCoefficients_eUnknown"},
	{BAVC_MatrixCoefficients_eDvi_Full_Range_RGB, "BAVC_MatrixCoefficients_eDvi_Full_Range_RGB"},
	{BAVC_MatrixCoefficients_eFCC,                "BAVC_MatrixCoefficients_eFCC"},
	{BAVC_MatrixCoefficients_eItu_R_BT_470_2_BG,  "BAVC_MatrixCoefficients_eItu_R_BT_470_2_BG"},
	{BAVC_MatrixCoefficients_eSmpte_170M,         "BAVC_MatrixCoefficients_eSmpte_170M"},
	{BAVC_MatrixCoefficients_eSmpte_240M,         "BAVC_MatrixCoefficients_eSmpte_240M"},
	{BAVC_MatrixCoefficients_eXvYCC_709,          "BAVC_MatrixCoefficients_eXvYCC_709"},
	{BAVC_MatrixCoefficients_eXvYCC_601,          "BAVC_MatrixCoefficients_eXvYCC_601"}
};

/* make sure matches with BVDC_P_Output */
static const struct
{
	BVDC_P_Output eVdcOutput;
	char *pcVdcOutputStr;

} stVDC_P_Output_InfoTbl[] =
{
	{BVDC_P_Output_eYQI,           "BVDC_P_Output_eYQI"},
	{BVDC_P_Output_eYQI_M,         "BVDC_P_Output_eYQI_M"},
	{BVDC_P_Output_eYUV,           "BVDC_P_Output_eYUV"},
	{BVDC_P_Output_eYUV_N,         "BVDC_P_Output_eYUV_N"},
	{BVDC_P_Output_eYUV_NC,        "BVDC_P_Output_eYUV_NC"},
#if BVDC_P_SUPPORT_VEC_SECAM
	{BVDC_P_Output_eYDbDr_LDK,     "BVDC_P_Output_eYDbDr_LDK"},
	{BVDC_P_Output_eYDbDr_BG,      "BVDC_P_Output_eYDbDr_BG"},
	{BVDC_P_Output_eYDbDr_H,       "BVDC_P_Output_eYDbDr_H"},
#endif
	{BVDC_P_Output_eSDYPrPb,       "BVDC_P_Output_eSDYPrPb"},
	{BVDC_P_Output_eRGB,           "BVDC_P_Output_eRGB"},
	{BVDC_P_Output_eHDYPrPb,       "BVDC_P_Output_eHDYPrPb"},
	{BVDC_P_Output_eHsync,         "BVDC_P_Output_eHsync"},
	{BVDC_P_Output_eUnknown,       "BVDC_P_Output_eUnknown"},
	{BVDC_P_Output_eNone,          "BVDC_P_Output_eNone"}
};


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
	BVDC_P_DisplayInfo     *pCurInfo = &hDisplay->stCurInfo;

	/* Find the Dac output in the array */
	for(uiIndex=0; uiIndex < BVDC_P_MAX_DACS; uiIndex++)
	{
		if (pCurInfo->aDacOutput[uiIndex] == eDacOutput)
		{
			return true;
		}
	}
	return false;
}

void BVDC_P_ResetAnalogChanInfo
	( BVDC_P_DisplayAnlgChan          *pstChan )
{
	pstChan->bEnable             = false; /* off */
	pstChan->bTearDown           = false;
	pstChan->eState              = BVDC_P_DisplayResource_eInactive;
	pstChan->ulIt                = BVDC_P_HW_ID_INVALID;
	pstChan->ulVf                = BVDC_P_HW_ID_INVALID;
	pstChan->ulSecam             = BVDC_P_HW_ID_INVALID;
	pstChan->ulSecam_HD          = BVDC_P_HW_ID_INVALID;
	pstChan->ulPrevSecam         = BVDC_P_HW_ID_INVALID;
	pstChan->ulPrevSecam_HD      = BVDC_P_HW_ID_INVALID;
	pstChan->ulSdsrc             = BVDC_P_HW_ID_INVALID;
	pstChan->ulPrevSdsrc         = BVDC_P_HW_ID_INVALID;
	pstChan->ulHdsrc             = BVDC_P_HW_ID_INVALID;
	pstChan->ulPrevHdsrc         = BVDC_P_HW_ID_INVALID;
	pstChan->ulDac_0             = BVDC_P_HW_ID_INVALID;
	pstChan->ulDac_1             = BVDC_P_HW_ID_INVALID;
	pstChan->ulDac_2             = BVDC_P_HW_ID_INVALID;
}

void BVDC_P_FreeAnalogChanResources
	( BVDC_P_Resource_Handle           hResource,
	  BVDC_P_DisplayAnlgChan          *pstChan )
{
	pstChan->ulPrevSdsrc = pstChan->ulSdsrc;
	pstChan->ulPrevHdsrc = pstChan->ulHdsrc;

	if (pstChan->ulVf != BVDC_P_HW_ID_INVALID)
	{
		BVDC_P_Resource_ReleaseHwId_isr(hResource, BVDC_P_ResourceType_eVf, pstChan->ulVf);
		pstChan->ulVf = BVDC_P_HW_ID_INVALID;
	}

	if (pstChan->ulSecam != BVDC_P_HW_ID_INVALID)
	{
		BVDC_P_Resource_ReleaseHwId_isr(hResource, BVDC_P_ResourceType_eSecam, pstChan->ulSecam);
		pstChan->ulSecam = BVDC_P_HW_ID_INVALID;
	}

	if (pstChan->ulSecam_HD != BVDC_P_HW_ID_INVALID)
	{
		BVDC_P_Resource_ReleaseHwId_isr(hResource, BVDC_P_ResourceType_eSecam_HD, pstChan->ulSecam_HD);
		pstChan->ulSecam_HD = BVDC_P_HW_ID_INVALID;
	}

	if (pstChan->ulSdsrc != BVDC_P_HW_ID_INVALID)
	{
		BVDC_P_Resource_ReleaseHwId_isr(hResource, BVDC_P_ResourceType_eSdsrc, pstChan->ulSdsrc);
		pstChan->ulSdsrc = BVDC_P_HW_ID_INVALID;
	}

	if (pstChan->ulHdsrc != BVDC_P_HW_ID_INVALID)
	{
		BVDC_P_Resource_ReleaseHwId_isr(hResource, BVDC_P_ResourceType_eHdsrc, pstChan->ulHdsrc);
		pstChan->ulHdsrc = BVDC_P_HW_ID_INVALID;
	}

	return;
}

void BVDC_P_FreeITResources
	( BVDC_P_Resource_Handle           hResource,
	  BVDC_P_DisplayAnlgChan          *pstChan )
{
	if (pstChan->ulId == 0 && (pstChan->ulIt != BVDC_P_HW_ID_INVALID))
	{
		BVDC_P_Resource_ReleaseHwId_isr(hResource, BVDC_P_ResourceType_eIt, pstChan->ulIt);
		pstChan->ulIt = BVDC_P_HW_ID_INVALID;
	}
	return;
}

BERR_Code BVDC_P_AllocITResources
	( BVDC_P_Resource_Handle           hResource,
	  BVDC_DisplayId                   eDisplayId,
	  BVDC_P_DisplayAnlgChan          *pstChan,
	  uint32_t                         ulIt )
{
	BERR_Code err = BERR_SUCCESS;

	if (ulIt == BVDC_P_HW_ID_INVALID)
	{
		err = BVDC_P_Resource_AcquireHwId_isr(hResource, BVDC_P_ResourceType_eIt, 0, eDisplayId, &pstChan->ulIt, false);

		if (err)
		{
			BDBG_ERR(("No IT available"));
			goto fail;
		}
	}
	else
	{
		pstChan->ulIt = ulIt;
	}
	BDBG_MSG(("\t ulIt = %d", pstChan->ulIt));
	return BERR_SUCCESS;

fail:
	BVDC_P_FreeITResources(hResource, pstChan);
	return BERR_TRACE(err);
}

BERR_Code BVDC_P_AllocAnalogChanResources
	( BVDC_P_Resource_Handle           hResource,
	  BVDC_DisplayId                   eDisplayId,
	  BVDC_P_DisplayAnlgChan          *pstChan,
	  bool                             bHdMust,
	  bool                             bHdRec,
	  bool                             bSecamCap )
{
	BERR_Code err = BERR_SUCCESS;

	BDBG_MSG(("bHdMust = %d, bHdRec = %d, bSecamCap = %d", bHdMust, bHdRec, bSecamCap));

	if(pstChan->ulVf == BVDC_P_HW_ID_INVALID)
	{
		err = BVDC_P_Resource_AcquireHwId_isr(hResource, BVDC_P_ResourceType_eVf, 0, eDisplayId, &pstChan->ulVf, false);

		if (err)
		{
			BDBG_ERR(("No VF available"));
			goto fail;
		}
	}
	BDBG_MSG(("\t ulVf = %d", pstChan->ulVf));

#if 1
	pstChan->ulPrevSecam = pstChan->ulSecam;
	pstChan->ulPrevSecam_HD = pstChan->ulSecam_HD;
	if (bSecamCap)
	{
		if(pstChan->ulSecam_HD != BVDC_P_HW_ID_INVALID)
		{
			BVDC_P_Resource_ReleaseHwId_isr(hResource, BVDC_P_ResourceType_eSecam_HD, pstChan->ulSecam_HD);
			pstChan->ulSecam_HD = BVDC_P_HW_ID_INVALID;
		}
		if(pstChan->ulSecam == BVDC_P_HW_ID_INVALID)
		{
			err = BVDC_P_Resource_AcquireHwId_isr(hResource, BVDC_P_ResourceType_eSecam, 0, eDisplayId, &pstChan->ulSecam, false);
			if (err)
			{
				BDBG_ERR(("No SECAM  available"));
				goto fail;
			}
		}
	}
	else
	{
		if(pstChan->ulSecam != BVDC_P_HW_ID_INVALID)
		{
			BVDC_P_Resource_ReleaseHwId_isr(hResource, BVDC_P_ResourceType_eSecam, pstChan->ulSecam);
			pstChan->ulSecam = BVDC_P_HW_ID_INVALID;
		}
		if(pstChan->ulSecam_HD == BVDC_P_HW_ID_INVALID)
		{
			err = BVDC_P_Resource_AcquireHwId_isr(hResource, BVDC_P_ResourceType_eSecam_HD, 0, eDisplayId, &pstChan->ulSecam_HD, true);
			if (err)
			{
				BDBG_MSG(("No HD SECAM available... trying to acquire SECAM"));
				err = BVDC_P_Resource_AcquireHwId_isr(hResource, BVDC_P_ResourceType_eSecam, 0, eDisplayId, &pstChan->ulSecam, false);
				if (err)
				{
					BDBG_ERR(("No SECAM available"));
					goto fail;
				}
			}
			else
			{
				err = BVDC_P_Resource_AcquireHwId_isr(hResource, BVDC_P_ResourceType_eSecam_HD, 0, eDisplayId, &pstChan->ulSecam_HD, false);
				if(err)
				{
					BDBG_ERR(("No SECAM HD available"));
					goto fail;
				}
			}
		}
	}
	BDBG_MSG(("\t ulSecam = %d", pstChan->ulSecam));
	BDBG_MSG(("\t ulSecam_HD = %d", pstChan->ulSecam_HD));
	BDBG_MSG(("\t ulPrevSecam = %d", pstChan->ulPrevSecam));
	BDBG_MSG(("\t ulPrevSecam_HD = %d", pstChan->ulPrevSecam_HD));
#endif

	pstChan->ulPrevSdsrc = pstChan->ulSdsrc;
	pstChan->ulPrevHdsrc = pstChan->ulHdsrc;
	if (bHdMust)
	{
		if(pstChan->ulSdsrc != BVDC_P_HW_ID_INVALID)
		{
			BVDC_P_Resource_ReleaseHwId_isr(hResource, BVDC_P_ResourceType_eSdsrc, pstChan->ulSdsrc);
			pstChan->ulSdsrc = BVDC_P_HW_ID_INVALID;
		}
		if(pstChan->ulHdsrc == BVDC_P_HW_ID_INVALID)
		{
			err = BVDC_P_Resource_AcquireHwId_isr(hResource, BVDC_P_ResourceType_eHdsrc, 0, eDisplayId, &pstChan->ulHdsrc, false);

			if (err)
			{
				BDBG_ERR(("No HDSRC available"));
				goto fail;
			}
		}
		BDBG_MSG(("\t ulSdsrc = %d", pstChan->ulSdsrc));
		BDBG_MSG(("\t ulHdsrc = %d", pstChan->ulHdsrc));
	}
	else if (bHdRec)
	{
		if(pstChan->ulSdsrc != BVDC_P_HW_ID_INVALID)
		{
			BVDC_P_Resource_ReleaseHwId_isr(hResource, BVDC_P_ResourceType_eSdsrc, pstChan->ulSdsrc);
			pstChan->ulSdsrc = BVDC_P_HW_ID_INVALID;
		}
		if(pstChan->ulHdsrc == BVDC_P_HW_ID_INVALID)
		{
			err = BVDC_P_Resource_AcquireHwId_isr(hResource, BVDC_P_ResourceType_eHdsrc, 0, eDisplayId, &pstChan->ulHdsrc, true);

			if (err)
			{
				BDBG_MSG(("No HDSRC available... trying to acquire SDSRC"));
				err = BVDC_P_Resource_AcquireHwId_isr(hResource, BVDC_P_ResourceType_eSdsrc, 0, eDisplayId, &pstChan->ulSdsrc, false);
				if (err)
				{
					BDBG_ERR(("No SDSRC available"));
					goto fail;
				}
				BDBG_MSG(("\t ulSdsrc = %d", pstChan->ulSdsrc));
				BDBG_MSG(("\t ulHdsrc = %d", pstChan->ulHdsrc));
			}
			else
			{
				err = BVDC_P_Resource_AcquireHwId_isr(hResource, BVDC_P_ResourceType_eHdsrc, 0, eDisplayId, &pstChan->ulHdsrc, false);
				if(err)
				{
					BDBG_ERR(("No HDSRC available"));
					goto fail;
				}
				BDBG_MSG(("\t ulSdsrc = %d", pstChan->ulSdsrc));
				BDBG_MSG(("\t ulHdsrc = %d", pstChan->ulHdsrc));
			}
		}
		else
		{
			BDBG_MSG(("\t ulSdsrc = %d", pstChan->ulSdsrc));
			BDBG_MSG(("\t ulHdsrc = %d", pstChan->ulHdsrc));
		}
	}
	else
	{
		if(pstChan->ulHdsrc != BVDC_P_HW_ID_INVALID)
		{
			BVDC_P_Resource_ReleaseHwId_isr(hResource, BVDC_P_ResourceType_eHdsrc, pstChan->ulHdsrc);
			pstChan->ulHdsrc = BVDC_P_HW_ID_INVALID;
		}
		if(pstChan->ulSdsrc == BVDC_P_HW_ID_INVALID)
		{
			err = BVDC_P_Resource_AcquireHwId_isr(hResource, BVDC_P_ResourceType_eSdsrc, 0, eDisplayId, &pstChan->ulSdsrc, true);

			if (err)
			{
				BDBG_MSG(("No SDSRC available... trying to acquire HDSRC"));
				err = BVDC_P_Resource_AcquireHwId_isr(hResource, BVDC_P_ResourceType_eHdsrc, 0, eDisplayId, &pstChan->ulHdsrc, false);
				if (err)
				{
					BDBG_ERR(("No HDSRC available"));
					goto fail;
				}
				BDBG_MSG(("\t ulSdsrc = %d", pstChan->ulSdsrc));
				BDBG_MSG(("\t ulHdsrc = %d", pstChan->ulHdsrc));
			}
			else
			{
				err = BVDC_P_Resource_AcquireHwId_isr(hResource, BVDC_P_ResourceType_eSdsrc, 0, eDisplayId, &pstChan->ulSdsrc, false);
				if(err)
				{
					BDBG_ERR(("No SDSRC available"));
					goto fail;
				}
				BDBG_MSG(("\t ulSdsrc = %d", pstChan->ulSdsrc));
				BDBG_MSG(("\t ulHdsrc = %d", pstChan->ulHdsrc));
			}
		}
		else
		{
			BDBG_MSG(("\t ulSdsrc = %d", pstChan->ulSdsrc));
			BDBG_MSG(("\t ulHdsrc = %d", pstChan->ulHdsrc));
		}
	}
	BDBG_MSG(("\t ulPrevSdsrc = %d", pstChan->ulPrevSdsrc));
	BDBG_MSG(("\t ulPrevHdsrc = %d", pstChan->ulPrevHdsrc));

	return BERR_SUCCESS;

fail:
	BVDC_P_FreeAnalogChanResources(hResource, pstChan);
	return BERR_TRACE(err);
}


static void BVDC_P_TearDownIT
	( BVDC_P_DisplayAnlgChan             *pstChan,
	  BVDC_P_ListInfo                    *pList )
{
	/* Disable modules in the path. This will drain data in the pipeline. */
	if ((pstChan->ulIt != BVDC_P_HW_ID_INVALID))
	{
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VEC_CFG_IT_0_SOURCE + pstChan->ulIt * 4);
		*pList->pulCurrent++ =
		    BCHP_FIELD_DATA(VEC_CFG_IT_0_SOURCE, SOURCE, BCHP_VEC_CFG_IT_0_SOURCE_SOURCE_DISABLE);
	}

#if BVDC_P_SUPPORT_NEW_SW_INIT
	if ((pstChan->ulIt != BVDC_P_HW_ID_INVALID))
	{
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VEC_CFG_SW_INIT_IT_0 + pstChan->ulIt * 4);
		*pList->pulCurrent++ =
		    BCHP_FIELD_DATA(VEC_CFG_SW_INIT_IT_0, INIT, 1);

		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VEC_CFG_SW_INIT_IT_0 + pstChan->ulIt * 4);
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(VEC_CFG_SW_INIT_IT_0, INIT, 0);
	}
#else
	if ((pstChan->ulIt != BVDC_P_HW_ID_INVALID))
	{
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VEC_CFG_SW_RESET_IT_0 + pstChan->ulIt * 4);
		*pList->pulCurrent++ =
		    BCHP_FIELD_DATA(VEC_CFG_SW_RESET_IT_0, RESET, 1);

		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VEC_CFG_SW_RESET_IT_0 + pstChan->ulIt * 4);
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(VEC_CFG_SW_RESET_IT_0, RESET, 0);
	}
#endif

	return;
}

static void BVDC_P_TearDownAnalogChan
	( BVDC_P_DisplayAnlgChan             *pstChan,
	  BVDC_P_ListInfo                    *pList )
{
	if (pstChan->ulVf != BVDC_P_HW_ID_INVALID)
	{
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VEC_CFG_VF_0_SOURCE + pstChan->ulVf * 4);
		*pList->pulCurrent++ =
		    BCHP_FIELD_DATA(VEC_CFG_VF_0_SOURCE, SOURCE, BCHP_VEC_CFG_VF_0_SOURCE_SOURCE_DISABLE);
	}

	if (pstChan->ulSecam != BVDC_P_HW_ID_INVALID)
	{
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VEC_CFG_SECAM_0_SOURCE + pstChan->ulSecam * 4);
		*pList->pulCurrent++ =
		    BCHP_FIELD_DATA(VEC_CFG_SECAM_0_SOURCE, SOURCE, BCHP_VEC_CFG_SECAM_0_SOURCE_SOURCE_DISABLE);
	}

	if (pstChan->ulSecam_HD != BVDC_P_HW_ID_INVALID)
	{
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VEC_CFG_SECAM_0_SOURCE + (BVDC_P_NUM_SHARED_SECAM + pstChan->ulSecam_HD) * 4);
		*pList->pulCurrent++ =
		    BCHP_FIELD_DATA(VEC_CFG_SECAM_0_SOURCE, SOURCE, BCHP_VEC_CFG_SECAM_0_SOURCE_SOURCE_DISABLE);
	}

	if (pstChan->ulSdsrc != BVDC_P_HW_ID_INVALID)
	{
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VEC_CFG_SDSRC_0_SOURCE + pstChan->ulSdsrc * 4);
		*pList->pulCurrent++ =
		    BCHP_FIELD_DATA(VEC_CFG_SDSRC_0_SOURCE, SOURCE, BCHP_VEC_CFG_SDSRC_0_SOURCE_SOURCE_DISABLE);
	}

#if (BVDC_P_NUM_SHARED_HDSRC > 0)
	if (pstChan->ulHdsrc != BVDC_P_HW_ID_INVALID)
	{
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VEC_CFG_HDSRC_0_SOURCE + pstChan->ulHdsrc * 4);
		*pList->pulCurrent++ =
		    BCHP_FIELD_DATA(VEC_CFG_HDSRC_0_SOURCE, SOURCE, BCHP_VEC_CFG_HDSRC_0_SOURCE_SOURCE_DISABLE);
	}
#endif

	/* Reset and unreset all the modules on the path.
	 * SECAM0 can not be left at reset state. It would
	 * affect other cores.
	 */
#if BVDC_P_SUPPORT_NEW_SW_INIT
	if (pstChan->ulVf != BVDC_P_HW_ID_INVALID)
	{
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VEC_CFG_SW_INIT_VF_0 + pstChan->ulVf * 4);
		*pList->pulCurrent++ =
		    BCHP_FIELD_DATA(VEC_CFG_SW_INIT_VF_0, INIT, 1);

		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VEC_CFG_SW_INIT_VF_0 + pstChan->ulVf * 4);
		*pList->pulCurrent++ =
		    BCHP_FIELD_DATA(VEC_CFG_SW_INIT_VF_0, INIT, 0);
	}

	if (pstChan->ulSecam != BVDC_P_HW_ID_INVALID)
	{
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VEC_CFG_SW_INIT_SECAM_0 + pstChan->ulSecam * 4);
		*pList->pulCurrent++ =
		    BCHP_FIELD_DATA(VEC_CFG_SW_INIT_SECAM_0, INIT, 1);

		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VEC_CFG_SW_INIT_SECAM_0 + pstChan->ulSecam * 4);
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(VEC_CFG_SW_INIT_SECAM_0, INIT, 0);
	}

	if (pstChan->ulSecam_HD != BVDC_P_HW_ID_INVALID
#if BVDC_P_VEC_HW7425_475_WORK_AROUND
		&& pstChan->ulSecam_HD != 1 /* SECAM_2 */
#endif
	    )
	{
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VEC_CFG_SW_INIT_SECAM_0 + (BVDC_P_NUM_SHARED_SECAM + pstChan->ulSecam_HD) * 4);
		*pList->pulCurrent++ =
		    BCHP_FIELD_DATA(VEC_CFG_SW_INIT_SECAM_0, INIT, 1);

		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VEC_CFG_SW_INIT_SECAM_0 + (BVDC_P_NUM_SHARED_SECAM + pstChan->ulSecam_HD) * 4);
		*pList->pulCurrent++ =
		    BCHP_FIELD_DATA(VEC_CFG_SW_INIT_SECAM_0, INIT, 0);
	}

	if (pstChan->ulSdsrc != BVDC_P_HW_ID_INVALID)
	{
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VEC_CFG_SW_INIT_SDSRC_0 + pstChan->ulSdsrc * 4);
		*pList->pulCurrent++ =
		    BCHP_FIELD_DATA(VEC_CFG_SW_INIT_SDSRC_0, INIT, 1);

		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VEC_CFG_SW_INIT_SDSRC_0 + pstChan->ulSdsrc * 4);
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(VEC_CFG_SW_INIT_SDSRC_0, INIT, 0);
	}

#if (BVDC_P_NUM_SHARED_HDSRC > 0)
	if (pstChan->ulHdsrc != BVDC_P_HW_ID_INVALID)
	{
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VEC_CFG_SW_INIT_HDSRC_0 + pstChan->ulHdsrc * 4);
		*pList->pulCurrent++ =
		    BCHP_FIELD_DATA(VEC_CFG_SW_INIT_HDSRC_0, INIT, 1);

		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VEC_CFG_SW_INIT_HDSRC_0 + pstChan->ulHdsrc * 4);
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(VEC_CFG_SW_INIT_HDSRC_0, INIT, 0);
	}
#endif
#else
	if (pstChan->ulVf != BVDC_P_HW_ID_INVALID)
	{
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VEC_CFG_SW_RESET_VF_0 + pstChan->ulVf * 4);
		*pList->pulCurrent++ =
		    BCHP_FIELD_DATA(VEC_CFG_SW_RESET_VF_0, RESET, 1);

		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VEC_CFG_SW_RESET_VF_0 + pstChan->ulVf * 4);
		*pList->pulCurrent++ =
		    BCHP_FIELD_DATA(VEC_CFG_SW_RESET_VF_0, RESET, 0);
	}

	if (pstChan->ulSecam != BVDC_P_HW_ID_INVALID)
	{
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VEC_CFG_SW_RESET_SECAM_0 + pstChan->ulSecam * 4);
		*pList->pulCurrent++ =
		    BCHP_FIELD_DATA(VEC_CFG_SW_RESET_SECAM_0, RESET, 1);

		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VEC_CFG_SW_RESET_SECAM_0 + pstChan->ulSecam * 4);
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(VEC_CFG_SW_RESET_SECAM_0, RESET, 0);
	}

	if (pstChan->ulSecam_HD != BVDC_P_HW_ID_INVALID)
	{
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VEC_CFG_SW_RESET_SECAM_0 + (BVDC_P_NUM_SHARED_SECAM + pstChan->ulSecam_HD) * 4);
		*pList->pulCurrent++ =
		    BCHP_FIELD_DATA(VEC_CFG_SW_RESET_SECAM_0, RESET, 1);

		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VEC_CFG_SW_RESET_SECAM_0 + (BVDC_P_NUM_SHARED_SECAM + pstChan->ulSecam_HD) * 4);
		*pList->pulCurrent++ =
		    BCHP_FIELD_DATA(VEC_CFG_SW_RESET_SECAM_0, RESET, 0);
	}

	if (pstChan->ulSdsrc != BVDC_P_HW_ID_INVALID)
	{
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VEC_CFG_SW_RESET_SDSRC_0 + pstChan->ulSdsrc * 4);
		*pList->pulCurrent++ =
		    BCHP_FIELD_DATA(VEC_CFG_SW_RESET_SDSRC_0, RESET, 1);

		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VEC_CFG_SW_RESET_SDSRC_0 + pstChan->ulSdsrc * 4);
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(VEC_CFG_SW_RESET_SDSRC_0, RESET, 0);
	}

#if (BVDC_P_NUM_SHARED_HDSRC > 0)
	if (pstChan->ulHdsrc != BVDC_P_HW_ID_INVALID)
	{
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VEC_CFG_SW_RESET_HDSRC_0 + pstChan->ulHdsrc * 4);
		*pList->pulCurrent++ =
		    BCHP_FIELD_DATA(VEC_CFG_SW_RESET_HDSRC_0, RESET, 1);

		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VEC_CFG_SW_RESET_HDSRC_0 + pstChan->ulHdsrc * 4);
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(VEC_CFG_SW_RESET_HDSRC_0, RESET, 0);
	}
#endif
#endif

	return;
}

/* return the IT/DECIM/ITU656/DVI source ID */
static uint32_t BVDC_P_GetVecCfgSrc
	( BVDC_Display_Handle              hDisplay )
{
	uint32_t ulSrc = 0;

	switch(hDisplay->hCompositor->eId)
	{
		case BVDC_CompositorId_eCompositor0:
		    ulSrc = BCHP_VEC_CFG_IT_0_SOURCE_SOURCE_S_0;
		    break;

		case BVDC_CompositorId_eCompositor1:
		    ulSrc = BCHP_VEC_CFG_IT_0_SOURCE_SOURCE_S_1;
		    break;

#if (BVDC_P_CMP_2_MAX_VIDEO_WINDOW_COUNT > 0)
		case BVDC_CompositorId_eCompositor2:
		    ulSrc = BCHP_VEC_CFG_IT_0_SOURCE_SOURCE_S_2;
		    break;
#endif

#if (BVDC_P_CMP_3_MAX_VIDEO_WINDOW_COUNT > 0)
		case BVDC_CompositorId_eCompositor3:
			ulSrc = BCHP_VEC_CFG_IT_0_SOURCE_SOURCE_S_3;
			break;
#endif

#if (BVDC_P_CMP_4_MAX_VIDEO_WINDOW_COUNT > 0)
		case BVDC_CompositorId_eCompositor4:
			ulSrc = BCHP_VEC_CFG_IT_0_SOURCE_SOURCE_S_4;
			break;
#endif

#if (BVDC_P_CMP_5_MAX_VIDEO_WINDOW_COUNT > 0)
		case BVDC_CompositorId_eCompositor5:
			ulSrc = BCHP_VEC_CFG_IT_0_SOURCE_SOURCE_S_5;
			break;
#endif

#if (BVDC_P_CMP_6_MAX_VIDEO_WINDOW_COUNT > 0)
		case BVDC_CompositorId_eCompositor6:
			ulSrc = BCHP_VEC_CFG_IT_0_SOURCE_SOURCE_S_6;
			break;
#endif

		default:
			/* Add other source later */
			BDBG_ERR(("unsupport source %d", hDisplay->hCompositor->eId));
			BDBG_ASSERT(0);
			break;
	}

	return ulSrc;
}


static void BVDC_P_SetupIT
	( BVDC_Display_Handle              hDisplay,
	  BVDC_P_DisplayAnlgChan          *pstChan,
	  BVDC_P_ListInfo                 *pList )
{
	uint32_t ulSrc = 0;

	BDBG_OBJECT_ASSERT(hDisplay, BVDC_DSP);
	BDBG_OBJECT_ASSERT(hDisplay->hCompositor, BVDC_CMP);

	/* Connect each module to its source */
	if ((pstChan->ulIt != BVDC_P_HW_ID_INVALID))
	{
		ulSrc = BVDC_P_GetVecCfgSrc(hDisplay);
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VEC_CFG_IT_0_SOURCE + pstChan->ulIt * 4);
		*pList->pulCurrent++ =
		    BCHP_FIELD_DATA(VEC_CFG_IT_0_SOURCE, SOURCE, ulSrc);
	}
	return;
}

static void BVDC_P_SetupAnalogChan
	( BVDC_P_Resource_Handle           hResource,
	  BVDC_P_DisplayAnlgChan          *pstChan,
	  BVDC_P_ListInfo                 *pList )
{
	uint32_t ulSrcSource = 0;

	if (pstChan->ulVf != BVDC_P_HW_ID_INVALID)
	{
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VEC_CFG_VF_0_SOURCE + pstChan->ulVf * 4);
		*pList->pulCurrent++ =
		    BCHP_FIELD_DATA(VEC_CFG_VF_0_SOURCE, SOURCE, pstChan->ulIt);
	}

	if (pstChan->ulSecam != BVDC_P_HW_ID_INVALID)
	{
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VEC_CFG_SECAM_0_SOURCE + pstChan->ulSecam * 4);
		*pList->pulCurrent++ =
		    BCHP_FIELD_DATA(VEC_CFG_SECAM_0_SOURCE, SOURCE, pstChan->ulVf);
		if(pstChan->ulPrevSecam_HD != pstChan->ulSecam_HD &&
		   BVDC_P_Resource_GetHwIdAcquireCntr_isr(hResource, BVDC_P_ResourceType_eSecam_HD, pstChan->ulPrevSecam_HD) == 0)
		{
			BDBG_MSG(("Need to disable Secam_HD %d", pstChan->ulPrevSecam_HD));
			*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
			*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VEC_CFG_SECAM_0_SOURCE + (BVDC_P_NUM_SHARED_SECAM + pstChan->ulPrevSecam_HD) * 4);
			*pList->pulCurrent++ =
			    BCHP_FIELD_DATA(VEC_CFG_SECAM_0_SOURCE, SOURCE, BCHP_VEC_CFG_SECAM_0_SOURCE_SOURCE_DISABLE);
		}
	}

	if (pstChan->ulSecam_HD != BVDC_P_HW_ID_INVALID)
	{
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VEC_CFG_SECAM_0_SOURCE + (BVDC_P_NUM_SHARED_SECAM + pstChan->ulSecam_HD) * 4);
		*pList->pulCurrent++ =
		    BCHP_FIELD_DATA(VEC_CFG_SECAM_0_SOURCE, SOURCE, pstChan->ulVf);
		if(pstChan->ulPrevSecam != pstChan->ulSecam &&
		   BVDC_P_Resource_GetHwIdAcquireCntr_isr(hResource, BVDC_P_ResourceType_eSecam, pstChan->ulPrevSecam) == 0)
		{
			BDBG_MSG(("Need to disable Secam", pstChan->ulPrevSecam));
			*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
			*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VEC_CFG_SECAM_0_SOURCE + pstChan->ulPrevSecam * 4);
			*pList->pulCurrent++ =
			    BCHP_FIELD_DATA(VEC_CFG_SECAM_0_SOURCE, SOURCE, BCHP_VEC_CFG_SECAM_0_SOURCE_SOURCE_DISABLE);
		}
	}

	if(pstChan->ulSecam != BVDC_P_HW_ID_INVALID)
	{
		ulSrcSource = pstChan->ulSecam;
	}
	else if(pstChan->ulSecam_HD != BVDC_P_HW_ID_INVALID)
	{
		ulSrcSource = pstChan->ulSecam_HD;
	}

	if(pstChan->ulSecam_HD != BVDC_P_HW_ID_INVALID)
	{
		ulSrcSource += BVDC_P_NUM_SHARED_SECAM;
	}

	if (pstChan->ulSdsrc != BVDC_P_HW_ID_INVALID)
	{
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VEC_CFG_SDSRC_0_SOURCE + pstChan->ulSdsrc * 4);
		*pList->pulCurrent++ =
		    BCHP_FIELD_DATA(VEC_CFG_SDSRC_0_SOURCE, SOURCE, ulSrcSource);

#if (BVDC_P_NUM_SHARED_HDSRC > 0)
		if(pstChan->ulSdsrc != pstChan->ulPrevSdsrc &&
		   pstChan->ulHdsrc != pstChan->ulPrevHdsrc &&
		   BVDC_P_Resource_GetHwIdAcquireCntr_isr(hResource, BVDC_P_ResourceType_eHdsrc, pstChan->ulPrevHdsrc) == 0)
		{
			BDBG_MSG(("Need to disable prev HDSRC %d", pstChan->ulPrevHdsrc));
			*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
			*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VEC_CFG_HDSRC_0_SOURCE + pstChan->ulPrevHdsrc * 4);
			*pList->pulCurrent++ =
			    BCHP_FIELD_DATA(VEC_CFG_HDSRC_0_SOURCE, SOURCE, BCHP_VEC_CFG_HDSRC_0_SOURCE_SOURCE_DISABLE);
		}
#endif
	}

#if (BVDC_P_NUM_SHARED_HDSRC > 0)
	if (pstChan->ulHdsrc != BVDC_P_HW_ID_INVALID)
	{
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VEC_CFG_HDSRC_0_SOURCE + pstChan->ulHdsrc * 4);
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(VEC_CFG_HDSRC_0_SOURCE, SOURCE, BCHP_VEC_CFG_HDSRC_0_SOURCE_SOURCE_SSP_0 +
			ulSrcSource);
		if(pstChan->ulSdsrc != pstChan->ulPrevSdsrc &&
		   pstChan->ulHdsrc != pstChan->ulPrevHdsrc &&
		   BVDC_P_Resource_GetHwIdAcquireCntr_isr(hResource, BVDC_P_ResourceType_eSdsrc, pstChan->ulPrevSdsrc) == 0)
		{
			BDBG_MSG(("Need to disable Prev SDSRC %d", pstChan->ulPrevSdsrc));
			*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
			*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VEC_CFG_SDSRC_0_SOURCE + pstChan->ulPrevSdsrc * 4);
			*pList->pulCurrent++ =
			    BCHP_FIELD_DATA(VEC_CFG_SDSRC_0_SOURCE, SOURCE, BCHP_VEC_CFG_SDSRC_0_SOURCE_SOURCE_DISABLE);
		}
	}
#endif

	return;
}

BERR_Code BVDC_P_FreeDacResources
	( BVDC_P_Resource_Handle           hResource,
	  BVDC_P_DisplayAnlgChan          *pstChan )
{
	BERR_Code err = BERR_SUCCESS;

	if(BVDC_P_HW_ID_INVALID != pstChan->ulDac_0)
	{
		BDBG_MSG(("\t release Dac %d", pstChan->ulDac_0));
		err = BVDC_P_Resource_ReleaseHwId_isr(hResource, BVDC_P_ResourceType_eDac, pstChan->ulDac_0);
		pstChan->ulDac_0 = BVDC_P_HW_ID_INVALID;
	}
	if(BVDC_P_HW_ID_INVALID != pstChan->ulDac_1)
	{
		BDBG_MSG(("\t release Dac %d", pstChan->ulDac_1));
		err = BVDC_P_Resource_ReleaseHwId_isr(hResource, BVDC_P_ResourceType_eDac, pstChan->ulDac_1);
		pstChan->ulDac_1 = BVDC_P_HW_ID_INVALID;
	}
	if(BVDC_P_HW_ID_INVALID != pstChan->ulDac_2)
	{
		BDBG_MSG(("\t release Dac %d", pstChan->ulDac_2));
		err = BVDC_P_Resource_ReleaseHwId_isr(hResource, BVDC_P_ResourceType_eDac, pstChan->ulDac_2);
		pstChan->ulDac_2 = BVDC_P_HW_ID_INVALID;
	}

	return BERR_TRACE(err);
}

BERR_Code BVDC_P_AllocDacResources
	( BVDC_P_Resource_Handle           hResource,
	  BVDC_P_DisplayAnlgChan          *pstChan,
	  uint32_t                         ulDacMask )
{
	BERR_Code err;

	err = BVDC_P_Resource_AcquireHwId_isr(hResource, BVDC_P_ResourceType_eDac, 0, ulDacMask, &pstChan->ulDac_0, false);
	if (err)
	{
		BDBG_ERR(("No DAC available"));
		goto fail;
	}
	BDBG_MSG(("\t Dac_0 ID: %d", pstChan->ulDac_0));

	err = BVDC_P_Resource_AcquireHwId_isr(hResource, BVDC_P_ResourceType_eDac, 0, ulDacMask, &pstChan->ulDac_1, false);
	if (err)
	{
		BDBG_ERR(("No DAC available"));
		goto fail;
	}
	BDBG_MSG(("\t Dac_1 ID: %d", pstChan->ulDac_1));

	err = BVDC_P_Resource_AcquireHwId_isr(hResource, BVDC_P_ResourceType_eDac, 0, ulDacMask, &pstChan->ulDac_2, false);
	if (err)
	{
		BDBG_ERR(("No DAC available"));
		goto fail;
	}
	BDBG_MSG(("\t Dac_2 ID: %d", pstChan->ulDac_2));

	return BERR_SUCCESS;

fail:

	BVDC_P_FreeDacResources(hResource, pstChan);
	return BERR_TRACE(err);
}


static uint32_t BVDC_P_Display_GetTg
	( BVDC_Display_Handle              hDisplay )
{
	uint32_t masterTg = 0;

	/* Set up master and slave timing generators */
	switch(hDisplay->eMasterTg)
	{
		case BVDC_DisplayTg_ePrimIt:
		case BVDC_DisplayTg_eSecIt:
		case BVDC_DisplayTg_eTertIt:
			/* Analog channel is the TG master */
			BDBG_ASSERT(hDisplay->stAnlgChan_0.ulIt != BVDC_P_HW_ID_INVALID);
			masterTg = hDisplay->stAnlgChan_0.ulIt;
			break;

		case BVDC_DisplayTg_eDviDtg: /* dvi in master mode */
			masterTg = BCHP_MISC_IT_0_MASTER_SEL_SELECT_DVI_DTG_0;
			break;

#if (BVDC_P_SUPPORT_ITU656_OUT != 0)
		case BVDC_DisplayTg_e656Dtg: /* 656 masster mode */
			masterTg = BCHP_MISC_DVI_DTG_0_MASTER_SEL_SELECT_ITU656_DTG_0;
			break;
#endif

		default:
			break;
	}

	return masterTg;
}


static void BVDC_P_Display_SetupAnlgTG_isr
	( BVDC_Display_Handle              hDisplay,
	  BVDC_P_ListInfo                 *pList )
{
	uint32_t masterTg;

	if(hDisplay->bAnlgEnable ||   /* if analog master */
	   (!hDisplay->bAnlgEnable &&  /* or anlog slave with DACs */
	   (hDisplay->stAnlgChan_0.bEnable || hDisplay->stAnlgChan_1.bEnable)))
	{
		masterTg = BVDC_P_Display_GetTg(hDisplay);
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_MISC_IT_0_MASTER_SEL + hDisplay->stAnlgChan_0.ulIt * 4);
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(MISC_IT_0_MASTER_SEL, SELECT, masterTg);
	}

	return;
}

static void BVDC_P_Display_SetupDviTG_isr
	( BVDC_Display_Handle              hDisplay,
	  BVDC_P_ListInfo                 *pList )
{
	uint32_t masterTg;

	if(hDisplay->stDviChan.bEnable || hDisplay->stCurInfo.bEnableHdmi)
	{
		masterTg = BVDC_P_Display_GetTg(hDisplay);
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_MISC_DVI_DTG_0_MASTER_SEL);
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(MISC_DVI_DTG_0_MASTER_SEL, SELECT, masterTg) |
			BCHP_FIELD_DATA(MISC_DVI_DTG_0_MASTER_SEL, FREERUN, (hDisplay->eMasterTg == BVDC_DisplayTg_eDviDtg) ? 1 : 0);
	}

	return;
}

#if (BVDC_P_SUPPORT_ITU656_OUT != 0)
static void BVDC_P_Display_Setup656TG_isr
	( BVDC_Display_Handle              hDisplay,
	  BVDC_P_ListInfo                 *pList )
{
	uint32_t masterTg;

	if (hDisplay->st656Chan.bEnable)
	{
		masterTg = BVDC_P_Display_GetTg(hDisplay);
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_MISC_ITU656_DTG_0_MASTER_SEL);
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(MISC_ITU656_DTG_0_MASTER_SEL, SELECT, masterTg) |
			BCHP_FIELD_DATA(MISC_ITU656_DTG_0_MASTER_SEL, FREERUN, (hDisplay->eMasterTg == BVDC_DisplayTg_e656Dtg) ? 1 : 0);
	}
	return;
}
#endif




static void BVDC_P_Vec_Build_CSC_isr
	( const BVDC_P_DisplayCscMatrix   *pCscMatrix,
	  BVDC_P_ListInfo                 *pList )
{
	*pList->pulCurrent++ =
		BCHP_FIELD_ENUM(CSC_0_CSC_MODE, CLAMP_MODE_C0, MIN_MAX) |
		BCHP_FIELD_ENUM(CSC_0_CSC_MODE, CLAMP_MODE_C1, MIN_MAX) |
		BCHP_FIELD_ENUM(CSC_0_CSC_MODE, CLAMP_MODE_C2, MIN_MAX) |
		BCHP_FIELD_DATA(CSC_0_CSC_MODE, RANGE1, 0x005A) |
		BCHP_FIELD_DATA(CSC_0_CSC_MODE, RANGE2, 0x007F);

	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(CSC_0_CSC_MIN_MAX, MIN, pCscMatrix->ulMin) |
		BCHP_FIELD_DATA(CSC_0_CSC_MIN_MAX, MAX, pCscMatrix->ulMax);

	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(CSC_0_CSC_COEFF_C01_C00, COEFF_C0, pCscMatrix->stCscCoeffs.usY0) |
		BCHP_FIELD_DATA(CSC_0_CSC_COEFF_C01_C00, COEFF_C1, pCscMatrix->stCscCoeffs.usY1);

	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(CSC_0_CSC_COEFF_C03_C02, COEFF_C2, pCscMatrix->stCscCoeffs.usY2) |
		BCHP_FIELD_DATA(CSC_0_CSC_COEFF_C03_C02, COEFF_C3, pCscMatrix->stCscCoeffs.usYOffset);

	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(CSC_0_CSC_COEFF_C11_C10, COEFF_C0, pCscMatrix->stCscCoeffs.usCb0) |
		BCHP_FIELD_DATA(CSC_0_CSC_COEFF_C11_C10, COEFF_C1, pCscMatrix->stCscCoeffs.usCb1);

	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(CSC_0_CSC_COEFF_C13_C12, COEFF_C2, pCscMatrix->stCscCoeffs.usCb2) |
		BCHP_FIELD_DATA(CSC_0_CSC_COEFF_C13_C12, COEFF_C3, pCscMatrix->stCscCoeffs.usCbOffset);

	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(CSC_0_CSC_COEFF_C21_C20, COEFF_C0, pCscMatrix->stCscCoeffs.usCr0) |
		BCHP_FIELD_DATA(CSC_0_CSC_COEFF_C21_C20, COEFF_C1, pCscMatrix->stCscCoeffs.usCr1);

	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(CSC_0_CSC_COEFF_C23_C22, COEFF_C2, pCscMatrix->stCscCoeffs.usCr2) |
		BCHP_FIELD_DATA(CSC_0_CSC_COEFF_C23_C22, COEFF_C3, pCscMatrix->stCscCoeffs.usCrOffset);

	BDBG_MSG(("CSC_isr: "));
	BDBG_MSG(("\t min = 0x%x, max = 0x%x", pCscMatrix->ulMin, pCscMatrix->ulMax));
	BDBG_MSG(("\t 0x%08x 0x%08x 0x%08x 0x%08x",
		pCscMatrix->stCscCoeffs.usY0, pCscMatrix->stCscCoeffs.usY1,
		pCscMatrix->stCscCoeffs.usY2, pCscMatrix->stCscCoeffs.usYOffset));
	BDBG_MSG(("\t 0x%08x 0x%08x 0x%08x 0x%08x",
		pCscMatrix->stCscCoeffs.usCb0, pCscMatrix->stCscCoeffs.usCb1,
		pCscMatrix->stCscCoeffs.usCb2, pCscMatrix->stCscCoeffs.usCbOffset));
	BDBG_MSG(("\t 0x%08x 0x%08x 0x%08x 0x%08x",
		pCscMatrix->stCscCoeffs.usCr0, pCscMatrix->stCscCoeffs.usCr1,
		pCscMatrix->stCscCoeffs.usCr2, pCscMatrix->stCscCoeffs.usCrOffset));

	return;
}

void BVDC_P_Vec_Build_Dither_isr
	( BVDC_P_DitherSetting            *pDither,
	  BVDC_P_ListInfo                 *pList )
{
	/* CSC_0_DITHER_CONTROL */
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(CSC_0_DITHER_CONTROL, MODE,       pDither->ulMode     ) |
		BCHP_FIELD_DATA(CSC_0_DITHER_CONTROL, OFFSET_CH2, pDither->ulCh2Offset) |
		BCHP_FIELD_DATA(CSC_0_DITHER_CONTROL, SCALE_CH2,  pDither->ulCh2Scale ) |
		BCHP_FIELD_DATA(CSC_0_DITHER_CONTROL, OFFSET_CH1, pDither->ulCh1Offset) |
		BCHP_FIELD_DATA(CSC_0_DITHER_CONTROL, SCALE_CH1,  pDither->ulCh1Scale ) |
		BCHP_FIELD_DATA(CSC_0_DITHER_CONTROL, OFFSET_CH0, pDither->ulCh0Offset) |
		BCHP_FIELD_DATA(CSC_0_DITHER_CONTROL, SCALE_CH0,  pDither->ulCh0Scale );

	/* CSC_0_DITHER_LFSR */
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(CSC_0_DITHER_LFSR, T0, pDither->ulLfsrCtrlT0) |
		BCHP_FIELD_DATA(CSC_0_DITHER_LFSR, T1, pDither->ulLfsrCtrlT1) |
		BCHP_FIELD_DATA(CSC_0_DITHER_LFSR, T2, pDither->ulLfsrCtrlT2);

	/* CSC_0_DITHER_LFSR_INIT */
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(CSC_0_DITHER_LFSR_INIT, SEQ,   pDither->ulLfsrSeq  ) |
		BCHP_FIELD_DATA(CSC_0_DITHER_LFSR_INIT, VALUE, pDither->ulLfsrValue);

	return;
}

static void BVDC_P_Vec_Build_SRC_isr
	( uint32_t                         ulSrcControl,
	  BVDC_P_DisplayAnlgChan          *pstChan,
	  BVDC_P_ListInfo                 *pList )
{
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
#if (BVDC_P_NUM_SHARED_HDSRC > 0)
	*pList->pulCurrent++ = (pstChan->ulHdsrc != BVDC_P_HW_ID_INVALID) ?
		BRDC_REGISTER(BCHP_HDSRC_0_HDSRC_CONTROL) :
		BRDC_REGISTER(BCHP_SDSRC_0_SRC_CONTROL + BVDC_P_VEC_SDSRC_SIZE * pstChan->ulSdsrc);
#else
	*pList->pulCurrent++ =
		BRDC_REGISTER(BCHP_SDSRC_0_SRC_CONTROL + BVDC_P_VEC_SDSRC_SIZE * pstChan->ulSdsrc);
#endif
	*pList->pulCurrent++ = ulSrcControl;

#if (BVDC_P_SUPPORT_TDAC_VER >= BVDC_P_SUPPORT_TDAC_VER_9)
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
#if (BVDC_P_NUM_SHARED_HDSRC > 0)
	*pList->pulCurrent++ = (pstChan->ulHdsrc != BVDC_P_HW_ID_INVALID) ?
		BRDC_REGISTER(BCHP_HDSRC_0_HDSRC_ANA_SCL_0_1) :
		BRDC_REGISTER(BCHP_SDSRC_0_SRC_ANA_SCL_0_1 + BVDC_P_VEC_SDSRC_SIZE * pstChan->ulSdsrc);
#else
	*pList->pulCurrent++ =
		BRDC_REGISTER(BCHP_SDSRC_0_SRC_ANA_SCL_0_1 + BVDC_P_VEC_SDSRC_SIZE * pstChan->ulSdsrc);
#endif
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(SDSRC_0_SRC_ANA_SCL_0_1, OFFSET_1, 0x0  ) |
		BCHP_FIELD_DATA(SDSRC_0_SRC_ANA_SCL_0_1, VALUE_1,  0x133) |
		BCHP_FIELD_DATA(SDSRC_0_SRC_ANA_SCL_0_1, OFFSET_0, 0x0  ) |
		BCHP_FIELD_DATA(SDSRC_0_SRC_ANA_SCL_0_1, VALUE_0,  0x180);

	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
#if (BVDC_P_NUM_SHARED_HDSRC > 0)
	*pList->pulCurrent++ = (pstChan->ulHdsrc != BVDC_P_HW_ID_INVALID) ?
		BRDC_REGISTER(BCHP_HDSRC_0_HDSRC_ANA_SCL_2_3) :
		BRDC_REGISTER(BCHP_SDSRC_0_SRC_ANA_SCL_2_3 + BVDC_P_VEC_SDSRC_SIZE * pstChan->ulSdsrc);
#else
	*pList->pulCurrent++ =
		BRDC_REGISTER(BCHP_SDSRC_0_SRC_ANA_SCL_2_3 + BVDC_P_VEC_SDSRC_SIZE * pstChan->ulSdsrc);
#endif
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(SDSRC_0_SRC_ANA_SCL_2_3, OFFSET_3, 0x0  ) |
		BCHP_FIELD_DATA(SDSRC_0_SRC_ANA_SCL_2_3, VALUE_3,  0x100) |
		BCHP_FIELD_DATA(SDSRC_0_SRC_ANA_SCL_2_3, OFFSET_2, 0x0  ) |
		BCHP_FIELD_DATA(SDSRC_0_SRC_ANA_SCL_2_3, VALUE_2,  0x120);
#endif

	return;
}

static void BVDC_P_Vec_Build_SM_isr
	( BFMT_VideoFmt                    eVideoFmt,
	  BVDC_P_Output                    eOutputCS,
	  const uint32_t                  *pTable,
	  BVDC_P_DisplayAnlgChan          *pstChan,
	  BVDC_P_ListInfo                 *pList )
{
	uint32_t ulOffset = pstChan->ulSdsrc * BVDC_P_VEC_SM_SIZE;

	*pList->pulCurrent++ = BRDC_OP_IMMS_TO_REGS(BVDC_P_SM_TABLE_SIZE);
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_SM_0_PG_CNTRL + ulOffset);

	/* SM_PG_CNTRL -> SM_COMP_CNTRL */
	BKNI_Memcpy((void*)pList->pulCurrent, (void*)pTable,
	    BVDC_P_SM_TABLE_SIZE * sizeof(uint32_t));
	pList->pulCurrent += BVDC_P_SM_TABLE_SIZE;

	/* Setup SM_COMP_CONFIG */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_SM_0_COMP_CONFIG + ulOffset);

	if(VIDEO_FORMAT_IS_NTSC(eVideoFmt))
	{
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(SM_0_COMP_CONFIG, U_LINE_SEL,          0) |
			BCHP_FIELD_DATA(SM_0_COMP_CONFIG, COMPOSITE_CLAMP_SEL, 1) |
			BCHP_FIELD_DATA(SM_0_COMP_CONFIG, V_LINE_SEL,          0) |
			BCHP_FIELD_DATA(SM_0_COMP_CONFIG, U_FIXED_LINE,        0) |
			BCHP_FIELD_DATA(SM_0_COMP_CONFIG, COMPOSITE_OFFSET,    0);
	}
	else if(VIDEO_FORMAT_IS_SECAM(eVideoFmt))
	{
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(SM_0_COMP_CONFIG, U_LINE_SEL,          0) |
			BCHP_FIELD_DATA(SM_0_COMP_CONFIG, COMPOSITE_CLAMP_SEL, 1) |
			BCHP_FIELD_DATA(SM_0_COMP_CONFIG, V_LINE_SEL,          1) |
			BCHP_FIELD_DATA(SM_0_COMP_CONFIG, U_FIXED_LINE,        0) |
			BCHP_FIELD_DATA(SM_0_COMP_CONFIG, COMPOSITE_OFFSET, 0xd0);
	}
	else /* PAL */
	{
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(SM_0_COMP_CONFIG, U_LINE_SEL,          0) |
			BCHP_FIELD_DATA(SM_0_COMP_CONFIG, COMPOSITE_CLAMP_SEL, 1) |
			BCHP_FIELD_DATA(SM_0_COMP_CONFIG, V_LINE_SEL,
			((eOutputCS == BVDC_P_Output_eYUV)   ||
			 (eOutputCS == BVDC_P_Output_eYUV_N) ||
			 (eOutputCS == BVDC_P_Output_eYUV_NC))                  ) |
			BCHP_FIELD_DATA(SM_0_COMP_CONFIG, U_FIXED_LINE,        0) |
			BCHP_FIELD_DATA(SM_0_COMP_CONFIG, COMPOSITE_OFFSET, 0);
	}

	return;
}

static void BVDC_P_Vec_Build_SECAM_isr
	( BFMT_VideoFmt                    eVideoFmt,
	  BVDC_P_DisplayAnlgChan          *pstChan,
	  BVDC_P_ListInfo                 *pList )
{
#if BVDC_P_SUPPORT_VEC_SECAM
	uint32_t ulOffset = pstChan->ulSecam * BVDC_P_VEC_SECAM_SIZE;

	if(VIDEO_FORMAT_IS_SECAM(eVideoFmt))
	{
		/* Setup SECAM_FM_CONTROL */
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_SECAM_0_FM_CONTROL + ulOffset);
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(SECAM_0_FM_CONTROL, FINE_LUMA_DELAY,  0) |
			BCHP_FIELD_DATA(SECAM_0_FM_CONTROL, GROSS_LUMA_DELAY, 0x18) |
			BCHP_FIELD_DATA(SECAM_0_FM_CONTROL, FINE_SC_DELAY,    0x3) |
			BCHP_FIELD_DATA(SECAM_0_FM_CONTROL, GROSS_SC_DELAY,   0x8) |
			BCHP_FIELD_ENUM(SECAM_0_FM_CONTROL, ENABLE, ON);

		/* Setup SECAM_FM_FMAMP */
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_SECAM_0_FM_FMAMP + ulOffset);
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(SECAM_0_FM_FMAMP, LOWER_LIMIT,  64) |
			BCHP_FIELD_DATA(SECAM_0_FM_FMAMP, UPPER_LIMIT, 192) |
			BCHP_FIELD_DATA(SECAM_0_FM_FMAMP, SLOPE_ADJUST,  3) |
			BCHP_FIELD_DATA(SECAM_0_FM_FMAMP, SCALE, 92);
	}
	else /* NTSC or PAL */
	{
		/* Setup SECAM_FM_CONTROL */
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_SECAM_0_FM_CONTROL + ulOffset);
		*pList->pulCurrent++ = BCHP_FIELD_ENUM(SECAM_0_FM_CONTROL, ENABLE, OFF);
	}

#else
	BSTD_UNUSED(eVideoFmt);
	BSTD_UNUSED(pstChan);
	BSTD_UNUSED(pList);
#endif

	return;
}

static void BDVC_P_Vec_Build_Grpd_isr
	( const BFMT_VideoInfo            *pFmtInfo,
	  const BVDC_P_DisplayAnlgChan    *pChannel,
	  BVDC_P_ListInfo                 *pList )
{
#if (BVDC_P_SUPPORT_VEC_GRPD)
	/* TODO: Support multiple instances after 7550 to minimized change set.. */
	uint32_t ulOffset = pChannel->ulSdsrc;
	uint32_t ulVidIndex = VIDEO_FORMAT_IS_NTSC(pFmtInfo->eVideoFmt) ? 0 : 1;
	BDBG_ASSERT(0 == ulOffset);

	/* GRPD_0_GRP00 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMMS_TO_REGS(BVDC_P_DLAY_FILTER_COUNT);
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_GRPD_0_GRP00 + ulOffset);
	BKNI_Memcpy(pList->pulCurrent, s_aulGrpdFilters[ulVidIndex].aulDlay,
		sizeof(s_aulGrpdFilters[ulVidIndex].aulDlay));
	pList->pulCurrent += BVDC_P_DLAY_FILTER_COUNT;

	/* BCHP_GRPD_0_TRAP00 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMMS_TO_REGS(BVDC_P_TRAP_FILTER_COUNT);
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_GRPD_0_TRAP00 + ulOffset);
	BKNI_Memcpy(pList->pulCurrent, s_aulGrpdFilters[ulVidIndex].aulTrap,
		sizeof(s_aulGrpdFilters[ulVidIndex].aulTrap));
	pList->pulCurrent += BVDC_P_TRAP_FILTER_COUNT;

	/* GRPD_0_CLIP0 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_GRPD_0_CLIP0 + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(GRPD_0_CLIP0, CLIP0_LO_TH, 0    ) |
		BCHP_FIELD_DATA(GRPD_0_CLIP0, CLIP0_HI_TH, 4095 );

	/* GRPD_0_CLIP1 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_GRPD_0_CLIP1 + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(GRPD_0_CLIP1, CLIP1_LO_TH, 0    ) |
		BCHP_FIELD_DATA(GRPD_0_CLIP1, CLIP1_HI_TH, 8191 );

	/* GRPD_0_CLIPBYP (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_GRPD_0_CLIPBYP + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(GRPD_0_CLIPBYP, BYP_CLIP1, 0 ) |
		BCHP_FIELD_DATA(GRPD_0_CLIPBYP, BYP_CLIP0, 0 );

	/* GRPD_0_VIDEOSCL (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_GRPD_0_VIDEOSCL + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(GRPD_0_VIDEOSCL, VIDEOOUTSCL, 302 ) |
		BCHP_FIELD_DATA(GRPD_0_VIDEOSCL, VIDEOINSCL,  32  );

	/* GRPD_0_AMCTL0 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_GRPD_0_AMCTL0 + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(GRPD_0_AMCTL0, DEPTH,    4096 ) |
		BCHP_FIELD_DATA(GRPD_0_AMCTL0, PREPILOT, 0    );

	/* GRPD_0_AMCTL1 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_GRPD_0_AMCTL1 + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(GRPD_0_AMCTL1, POSTPILOT, 0 ) |
		BCHP_FIELD_DATA(GRPD_0_AMCTL1, PRENEG,    0 );

	/* GRPD_0_DDFSFCW0 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_GRPD_0_DDFSFCW0 + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(GRPD_0_DDFSFCW0, DDFSFCW0, 7954 );

	/* GRPD_0_DDFSFCW1 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_GRPD_0_DDFSFCW1 + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(GRPD_0_DDFSFCW1, DDFSFCW1, 19884 );

	/* GRPD_0_DDFSSCL01 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_GRPD_0_DDFSSCL01 + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(GRPD_0_DDFSSCL01, SCLDDFS0, 2011 ) |
		BCHP_FIELD_DATA(GRPD_0_DDFSSCL01, SCLDDFS1, 1851 );

	/* GRPD_0_VIDEOBYP (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_GRPD_0_VIDEOBYP + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(GRPD_0_VIDEOBYP, TRAPBYP,  0 ) |
		BCHP_FIELD_DATA(GRPD_0_VIDEOBYP, BYP_GDY,  0 ) |
		BCHP_FIELD_DATA(GRPD_0_VIDEOBYP, BYP_QBPH, 0 ) |
		BCHP_FIELD_DATA(GRPD_0_VIDEOBYP, BYP_QB,   0 ) |
		BCHP_FIELD_DATA(GRPD_0_VIDEOBYP, BYP_HB,   0 ) |
		BCHP_FIELD_DATA(GRPD_0_VIDEOBYP, EN_TPIN,  0 ) |
		BCHP_FIELD_DATA(GRPD_0_VIDEOBYP, BYP_FV,   0 );

	/* GRPD_0_VIDEOTONE (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_GRPD_0_VIDEOTONE + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(GRPD_0_VIDEOTONE, DDFS_EN, 0 ) |
		BCHP_FIELD_DATA(GRPD_0_VIDEOTONE, FCW,     0 );

	/* GRPD_0_MODBYP (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_GRPD_0_MODBYP + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(GRPD_0_MODBYP, BYP_FILT, 0 ) |
		BCHP_FIELD_DATA(GRPD_0_MODBYP, SEL_MIX,  0 ) |
		BCHP_FIELD_DATA(GRPD_0_MODBYP, BYP_VID,  0 ) |
		BCHP_FIELD_DATA(GRPD_0_MODBYP, BYP_MIX,  0 ) |
		BCHP_FIELD_DATA(GRPD_0_MODBYP, BYP_SINC, 0 );

	/* GRPD_0_OUTSCL (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_GRPD_0_OUTSCL + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(GRPD_0_OUTSCL, EN,     0   ) |
		BCHP_FIELD_DATA(GRPD_0_OUTSCL, SCLOUT, 128 );

	/* GRPD_0_RESERVED0 (RW) */
	/* GRPD_0_RESERVED1 (RW) */
	/* VEC_CFG_GRPD_0_SOURCE (RW) */
#ifdef BCHP_VEC_CFG_GRPD_0_SOURCE_SOURCE_MASK
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VEC_CFG_GRPD_0_SOURCE + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VEC_CFG_GRPD_0_SOURCE, SOURCE, pChannel->ulSdsrc);
#endif
#else /* BVDC_P_SUPPORT_VEC_GRPD */
	BSTD_UNUSED(pFmtInfo);
	BSTD_UNUSED(pChannel);
	BSTD_UNUSED(pList);
#endif

	return;
}

/*************************************************************************
 *
 *  BVDC_P_Program_CSC_SRC_SM_isr
 *  Adds CSC, SRC and SM blocks to RUL for a display.
 *  Bypass - CSC
 *
 **************************************************************************/
static void BVDC_P_Vec_Build_CSC_SRC_SM_isr
	( BVDC_Display_Handle              hDisplay,
	  BVDC_P_DisplayAnlgChan          *pstChan,
	  BVDC_P_Output                    eOutputCS,
	  BVDC_P_ListInfo                 *pList )
{
	const BVDC_P_DisplayCscMatrix *pCscMatrix = NULL;
	const uint32_t     *pTable = NULL;
	BVDC_P_DisplayInfo *pCurInfo = &hDisplay->stCurInfo;
	uint32_t            ulSrcControl;

	/* Setup Main CSC */
	if( eOutputCS != BVDC_P_Output_eUnknown )
	{
		/* Setup SRC */
		ulSrcControl = BVDC_P_GetSrcControl_isr(pCurInfo);
		BVDC_P_Vec_Build_SRC_isr(ulSrcControl, pstChan, pList);

		/* Setup SM */
		if (pstChan->ulSdsrc != BVDC_P_HW_ID_INVALID)
		{
			pTable = BVDC_P_GetSmTable_isr(pCurInfo, eOutputCS);
			BVDC_P_Vec_Build_SM_isr(pCurInfo->pFmtInfo->eVideoFmt, eOutputCS, pTable, pstChan, pList);
			BVDC_P_Vec_Build_SECAM_isr(pCurInfo->pFmtInfo->eVideoFmt, pstChan, pList);

			/* TODO: Which versioning to use?  SM, SRC, or by itself? */
			/* SRC, has hardwired GRPD into it.  Need to separate if HW
			 * introduces Mux after SRC to bypass or selectable GRPD_x. */
			BDVC_P_Vec_Build_Grpd_isr(pCurInfo->pFmtInfo, pstChan, pList);
		}

		/* Setup CSC for  */
		BVDC_P_Display_GetCscTable_isr(pCurInfo, eOutputCS, &pCscMatrix);

		pstChan->stCscMatrix = *pCscMatrix;

		/* TODO: handle user csc */
		/* Handle CSC mute */
		if (((pCurInfo->abOutputMute[BVDC_DisplayOutput_eComponent]) && BVDC_P_DISP_IS_ANLG_CHAN_CO(pstChan, pCurInfo)) ||
			((pCurInfo->abOutputMute[BVDC_DisplayOutput_eComposite]) && BVDC_P_DISP_IS_ANLG_CHAN_CVBS(pstChan, pCurInfo)) ||
			((pCurInfo->abOutputMute[BVDC_DisplayOutput_eSVideo]) && BVDC_P_DISP_IS_ANLG_CHAN_SVIDEO(pstChan, pCurInfo)))
		{
			const BVDC_P_Compositor_Info *pCmpInfo = &hDisplay->hCompositor->stCurInfo;
			uint8_t ucCh0 = BPXL_GET_COMPONENT(BPXL_eA8_Y8_Cb8_Cr8, pCmpInfo->ulBgColorYCrCb, 2);
			uint8_t ucCh1 = BPXL_GET_COMPONENT(BPXL_eA8_Y8_Cb8_Cr8, pCmpInfo->ulBgColorYCrCb, 1);
			uint8_t ucCh2 = BPXL_GET_COMPONENT(BPXL_eA8_Y8_Cb8_Cr8, pCmpInfo->ulBgColorYCrCb, 0);

			/* Swap ch0 and 1 of input color to match vec csc layout */
			BVDC_P_Csc_ApplyYCbCrColor(&pstChan->stCscMatrix.stCscCoeffs, ucCh1, ucCh0, ucCh2);
		}

		*pList->pulCurrent++ = BRDC_OP_IMMS_TO_REGS(BVDC_P_CSC_TABLE_SIZE);
		/* CSC module pairs with VF */
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_CSC_0_CSC_MODE + BVDC_P_VEC_CSC_SIZE * pstChan->ulVf);

		BVDC_P_Vec_Build_CSC_isr(&pstChan->stCscMatrix, pList);

		*pList->pulCurrent++ = BRDC_OP_IMMS_TO_REGS(BVDC_P_DITHER_TABLE_SIZE);
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_CSC_0_DITHER_CONTROL + BVDC_P_VEC_CSC_SIZE * pstChan->ulVf);
		BVDC_P_Vec_Build_Dither_isr(&hDisplay->stCscDither, pList);
	}

	return;
}


/*************************************************************************
 *  {secret}
 *	BVDC_P_Vec_Build_VF_isr
 *	Builds either: SEC_VF, SEC_VF_CO or PRIM_VF
 *  Required for Videoformat or colorspace change
 **************************************************************************/
static void BVDC_P_Vec_Build_VF_isr
	( BVDC_Display_Handle              hDisplay,
	  BVDC_P_DisplayAnlgChan          *pstChan,
	  BVDC_P_Output                    eOutputCS,
	  BVDC_P_ListInfo                 *pList )
{
	BVDC_P_Display_ShaperSettings stShaperSettings;
	uint32_t             ulNumRegs;
	uint32_t            *pulFmtAdder;
	const uint32_t      *pTable=0;
	BVDC_P_DisplayInfo  *pCurInfo = &hDisplay->stCurInfo;
	uint32_t             ulOffset = pstChan->ulVf * BVDC_P_VEC_VF_SIZE;
	uint32_t             i=0;
	uint32_t             ulVal;
	uint32_t             ulVfMiscRegIdx, ulVfMiscRegValue;
#if !DCS_SUPPORT
	uint32_t             ulValEx;
	uint32_t             ulVfSTRegIdx,   ulVfSTRegValue;
#endif

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
	BVDC_P_GetChFilters_isr(pCurInfo, eOutputCS, &pstChan->apVfFilter[0], &pstChan->apVfFilter[1], &pstChan->apVfFilter[2]);

	/* override with user filters */
	for (i=0; i < BVDC_P_VEC_CH_NUM; i++)
	{
		if (pCurInfo->abUserVfFilterCo[i] && BVDC_P_DISP_IS_ANLG_CHAN_CO(pstChan, pCurInfo))
		{
			pstChan->apVfFilter[i] = pCurInfo->aaulUserVfFilterCo[i];
		}
		else if (pCurInfo->abUserVfFilterCvbs[i] && BVDC_P_DISP_IS_ANLG_CHAN_CVBS(pstChan, pCurInfo))
		{
			pstChan->apVfFilter[i] = pCurInfo->aaulUserVfFilterCvbs[i];
		}
	}

	/* get the correct vf table */
	BVDC_P_GetVfTable_isr(pCurInfo, eOutputCS, &pTable, &stShaperSettings);

	/* prepare for setting vec BVB left cut */
	ulVfMiscRegIdx = (BCHP_VF_0_MISC - BCHP_VF_0_FORMAT_ADDER) / sizeof(uint32_t);
	ulVfMiscRegValue = pTable[ulVfMiscRegIdx] & ~BCHP_MASK(VF_0_MISC, BVB_SAV_REMOVE);
	ulVfMiscRegValue |= BCHP_FIELD_DATA(VF_0_MISC, BVB_SAV_REMOVE, stShaperSettings.ulSavRemove);

#if !DCS_SUPPORT /** { **/
	/* Prepare for settng the _VF_SYNC_TRANS_0 register */
	ulVfSTRegIdx =
		(BCHP_VF_0_SYNC_TRANS_0 - BCHP_VF_0_FORMAT_ADDER) /
			sizeof(uint32_t);
	ulVfSTRegValue =
		BVDC_P_Display_Modify_SYNC_TRANS_0 (pCurInfo, pTable[ulVfSTRegIdx]);
#endif /** } !DCS_SUPPORT **/


	/* PAL line 23 is reserved for WSS vbi data; Cx and beyond can drop the
     * 1st active video line from BVN; */
	if (VIDEO_FORMAT_IS_625_LINES(pCurInfo->pFmtInfo->eVideoFmt))
	{
		ulVfMiscRegValue |=
			BCHP_FIELD_DATA(VF_0_MISC, BVB_LINE_REMOVE_TOP, 1);
	}

	/* Also for PAL-N. */
	else if ((pCurInfo->pFmtInfo->eVideoFmt == BFMT_VideoFmt_ePAL_N) &&
			 ((pCurInfo->eMacrovisionType ==
			 	BVDC_MacrovisionType_eNoProtection) ||
			 (pCurInfo->eMacrovisionType ==
			 	BVDC_MacrovisionType_eTest01) ||
			 (pCurInfo->eMacrovisionType ==
			 	BVDC_MacrovisionType_eTest02)) )
	{
		ulVfMiscRegValue |=
			BCHP_FIELD_DATA(VF_0_MISC, BVB_LINE_REMOVE_TOP, 1);
	}

	ulNumRegs = (BCHP_VF_0_SHAPER - BCHP_VF_0_FORMAT_ADDER)/4+1 ;

	/* Setup VF_FORMAT_ADDER -> VF_BVB_STATUS */
	*pList->pulCurrent++ = BRDC_OP_IMMS_TO_REGS(ulNumRegs);
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VF_0_FORMAT_ADDER + ulOffset);

	/* Save RUL table entry for VF_FORMAT_ADDER. It will be modified later. */
	pulFmtAdder = pList->pulCurrent;

	/* Setup VF_FORMAT_ADDER -> SEC_VF_SYNC_TRANS_1 */
	BKNI_Memcpy((void*)pList->pulCurrent, (void*)(pTable),
		(BVDC_P_VF_TABLE_SIZE) * sizeof(uint32_t));
	*(pList->pulCurrent + ulVfMiscRegIdx) = ulVfMiscRegValue;  /* override */
#if !DCS_SUPPORT
	*(pList->pulCurrent + ulVfSTRegIdx) = ulVfSTRegValue;  /* posible ovride */
#endif
	pList->pulCurrent += BVDC_P_VF_TABLE_SIZE;

	/* Setup Channel VF filters */
	for (i=0; i < BVDC_P_VEC_CH_NUM; i++)
	{
		BKNI_Memcpy((void*)pList->pulCurrent, (void*)pstChan->apVfFilter[i],
			BVDC_P_CHROMA_TABLE_SIZE * sizeof(uint32_t));
		pList->pulCurrent += BVDC_P_CHROMA_TABLE_SIZE;
	}

	/* BVB_STATUS */
	*pList->pulCurrent++ = 0xff;

	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VF_0_SHAPER, SAV_REPLICATE, stShaperSettings.ulSavReplicate) |
		BCHP_FIELD_DATA(VF_0_SHAPER, EAV_PREDICT,   stShaperSettings.ulEavPredict  ) |
		BCHP_FIELD_DATA(VF_0_SHAPER, THRESHOLD,     BVDC_P_VF_THRESH)    |
		BCHP_FIELD_DATA(VF_0_SHAPER, CONTROL,       BVDC_P_VF_ENABLE);

	/* Programming note: ulNumRegs values have been placed in the RUL. The
	 * last BRDC_OP_IMMS_TO_REGS is fulfilled.
	 */

/* Unfortunately, part of NEG_SYNC_AMPLITUDE is in a distant register. */
#if (BVDC_P_SUPPORT_VEC_VF_VER >= 1)
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ =
			BRDC_REGISTER(BCHP_VF_0_NEG_SYNC_AMPLITUDE_EXTN + ulOffset);
		*pList->pulCurrent++ = pTable[BVDC_P_VF_TABLE_SIZE];
#endif

	/* FORMAT_ADDER */
	ulVal = pTable[0];

	/* SD with RGB? */
	if((VIDEO_FORMAT_IS_SD(pCurInfo->pFmtInfo->eVideoFmt)) &&
		(eOutputCS == BVDC_P_Output_eRGB))
	{
		ulVal = BVDC_P_GetFmtAdderValue(pCurInfo);
	}

	/* remove sync on G channel if necessary */
	if(BVDC_P_Display_FindDac(hDisplay, BVDC_DacOutput_eGreen_NoSync) &&
	   (eOutputCS == BVDC_P_Output_eRGB))
	{
		/* remove sync pulses from G channel */
		ulVal &= ~BCHP_MASK(VF_0_FORMAT_ADDER, C0_SYNC);
		BDBG_MSG(("Turn OFF sync on G channel: 0x%x", ulVal));
	}

	if(!pCurInfo->stN0Bits.bPsAgc)
	{
		ulVal &= ~BCHP_MASK(VF_0_FORMAT_ADDER, SECOND_NEGATIVE_SYNC);
	}

#if DCS_SUPPORT
	if (VIDEO_FORMAT_SUPPORTS_DCS (pCurInfo->pFmtInfo->eVideoFmt))
	{
		ulVal &= ~BCHP_MASK       (VF_0_FORMAT_ADDER, C0_POSITIVESYNC);
		ulVal |=  BCHP_FIELD_DATA (VF_0_FORMAT_ADDER, C0_POSITIVESYNC, 1);
	}
#endif

	/* Edit the VF_FORMAT_ADDER RUL entry */
	*pulFmtAdder = ulVal;

#if DCS_SUPPORT /** { **/
#ifndef BVDC_P_SMOOTH_DCS /** { **/
	if (VIDEO_FORMAT_SUPPORTS_DCS (pCurInfo->pFmtInfo->eVideoFmt))
	{
		bool bComposite =
			(BVDC_P_DISP_IS_ANLG_CHAN_CVBS  (pstChan,pCurInfo) ||
			 BVDC_P_DISP_IS_ANLG_CHAN_SVIDEO(pstChan,pCurInfo)     );
		const BVDC_P_DCS_VFvalues* pVfValues =
			BVDC_P_DCS_GetVFvalues_isr (
				pCurInfo->pFmtInfo->eVideoFmt, bComposite, pCurInfo->eDcsMode);
		BVDC_P_DCS_VF_Update_isr (pVfValues, ulOffset, &(pList->pulCurrent));
	}
#endif /** } ! BVDC_P_SMOOTH_DCS **/

#else /** }  ! DCS_SUPPORT { **/

	/* NTSC/PAL could have sync reduction */
	if((VIDEO_FORMAT_IS_SD(pCurInfo->pFmtInfo->eVideoFmt)) &&
		(eOutputCS != BVDC_P_Output_eUnknown))
	{
		/* Note: for RGBHV, need to remove sync signals from G channel; */
		if (BVDC_P_Display_FindDac(hDisplay, BVDC_DacOutput_eHsync) == 0)
		{
			BVDC_P_GetNegSyncValue(pCurInfo, eOutputCS, &ulVal, &ulValEx);
		}
		else
		{
			ulVal   = 0;
			ulValEx = 0;
		}

		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ =
			BRDC_REGISTER(BCHP_VF_0_NEG_SYNC_VALUES + ulOffset);
		*pList->pulCurrent++ = ulVal;
#if (BVDC_P_SUPPORT_VEC_VF_VER >= 1)
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ =
			BRDC_REGISTER(BCHP_VF_0_NEG_SYNC_AMPLITUDE_EXTN + ulOffset);
		*pList->pulCurrent++ = ulValEx;
#endif
	}
	else
#endif /** } !DCS_SUPPORT **/
	if((VIDEO_FORMAT_IS_HD(pCurInfo->pFmtInfo->eVideoFmt)) &&
		BVDC_P_Display_FindDac(hDisplay, BVDC_DacOutput_eHsync))
	{
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VF_0_NEG_SYNC_VALUES + ulOffset);
		*pList->pulCurrent++ = 0;
	}

	/* Set up envelop generator */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VF_0_ENVELOPE_GENERATOR + ulOffset);
	*pList->pulCurrent++ = BVDC_P_GetVfEnvelopGenerator_isr(pCurInfo, eOutputCS);

#if (BVDC_P_SUPPORT_VEC_VF_VER > 1)
	/* Program DRAIN_PIXELS_SCART for CVBS channel when it's in scart mode */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VF_0_DRAIN_PIXELS_SCART + ulOffset);
	if(pCurInfo->bRgb && pCurInfo->bCvbs && BVDC_P_DISP_IS_ANLG_CHAN_CVBS(pstChan, pCurInfo))
	{
		*pList->pulCurrent++ =
			BCHP_FIELD_ENUM(VF_0_DRAIN_PIXELS_SCART, ENABLE, ENABLE) |
			BCHP_FIELD_DATA(VF_0_DRAIN_PIXELS_SCART, NUM_OF_SAMPLES,
				VIDEO_FORMAT_IS_NTSC(pCurInfo->pFmtInfo->eVideoFmt) ? 15 : 14);
	}
	else
	{
		*pList->pulCurrent++ =
			BCHP_FIELD_ENUM(VF_0_DRAIN_PIXELS_SCART, ENABLE, DISABLE) |
			BCHP_FIELD_DATA(VF_0_DRAIN_PIXELS_SCART, NUM_OF_SAMPLES, 0);
	}
#endif
	return;
}

/*************************************************************************
 *  {secret}
 *	BVDC_P_Vec_Build_RM_isr
 **************************************************************************/
static void BVDC_P_Vec_Build_RM_isr
	( BVDC_Display_Handle              hDisplay,
	  BVDC_P_DisplayAnlgChan          *pstChan,
	  BVDC_P_ListInfo                 *pList  )
{
	/* RM pairs with IT */
	uint32_t          ulOffset = pstChan->ulIt * BVDC_P_VEC_RM_SIZE;
	BVDC_P_DisplayInfo *pCurInfo = &hDisplay->stCurInfo;
	const uint32_t   *pRmTable;
	BAVC_VdcDisplay_Info    lRateInfo;
	BAVC_VdcDisplay_Info	stPrevRateInfo;


	BDBG_ENTER(BVDC_P_Vec_Build_RM_isr);

	stPrevRateInfo = pCurInfo->stRateInfo;
	if(BVDC_P_GetRmTable_isr(pCurInfo, pCurInfo->pFmtInfo, &pRmTable, pCurInfo->bFullRate, &lRateInfo) != BERR_SUCCESS)
		BDBG_ASSERT(0);
	pCurInfo->stRateInfo = lRateInfo;
	pCurInfo->ulVertFreq = lRateInfo.ulVertRefreshRate;
	hDisplay->pRmTable = pRmTable;

	/* Notify external modules that Rate Manager has been changed. */
	if((lRateInfo.ulPixelClkRate    != stPrevRateInfo.ulPixelClkRate   ) ||
	   (lRateInfo.ulVertRefreshRate != stPrevRateInfo.ulVertRefreshRate))
	{
		BDBG_MSG(("\tVEC's RM PxlClk = %sMHz, RefRate = %d (1/%dth Hz), MultiRate=%d",
			BVDC_P_GetRmString_isr(pCurInfo, pCurInfo->pFmtInfo),
			pCurInfo->ulVertFreq, BFMT_FREQ_FACTOR, pCurInfo->bMultiRateAllow));
		hDisplay->bRateManagerUpdated = true;
	}

	/* --- Setup RM --- */
	*pList->pulCurrent++ = BRDC_OP_IMMS_TO_REGS(BVDC_P_RM_TABLE_SIZE);
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_RM_0_RATE_RATIO + ulOffset);

	/* Setup RM_RATE_RATIO to RM_INTEGRATOR */
	BKNI_Memcpy((void*)pList->pulCurrent, (void*)pRmTable,
		BVDC_P_RM_TABLE_SIZE * sizeof(uint32_t));
	pList->pulCurrent += BVDC_P_RM_TABLE_SIZE;

	BDBG_LEAVE(BVDC_P_Vec_Build_RM_isr);

	return;
}


/*************************************************************************
 *  {secret}
 *  BVDC_P_Display_Build_ItBvbSize_isr
 *  Builds IT BVB Size register
 **************************************************************************/
static void BVDC_P_Display_Build_ItBvbSize_isr
	( BVDC_Display_Handle              hDisplay,
	  BVDC_P_ListInfo                 *pList,
	  uint32_t                         ulOffset )
{
	BVDC_P_DisplayInfo  *pCurInfo = &hDisplay->stCurInfo;
	uint32_t ulHeight = pCurInfo->ulHeight;

	if(BFMT_IS_3D_MODE(pCurInfo->pFmtInfo->eVideoFmt))
	{
		ulHeight = ulHeight * 2 + pCurInfo->pFmtInfo->ulActiveSpace;
	}

	/* Update IT BVB size */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_IT_0_BVB_SIZE + ulOffset);
	if(pCurInfo->bWidthTrimmed &&
	   VIDEO_FORMAT_IS_NTSC(pCurInfo->pFmtInfo->eVideoFmt) &&
	   (pCurInfo->eMacrovisionType < BVDC_MacrovisionType_eTest01))
	{
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(IT_0_BVB_SIZE, HORIZONTAL,  712) |
			BCHP_FIELD_DATA(IT_0_BVB_SIZE, VERTICAL,    ulHeight);
	}
	else
	{
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(IT_0_BVB_SIZE, HORIZONTAL,  pCurInfo->pFmtInfo->ulWidth) |
			BCHP_FIELD_DATA(IT_0_BVB_SIZE, VERTICAL,    ulHeight);
	}

	return;
}


/*************************************************************************
 *  {secret}
 *  BVDC_P_Vec_Build_ItMc_isr
 **************************************************************************/
static void BVDC_P_Vec_Build_ItMc_isr
	( BVDC_P_ListInfo                 *pList,
	  const uint32_t                  *pRamTbl,
	  uint32_t                         ulOffset )
{
	/* Write IT_MICRO_INSTRUCTIONS 0..255 */
	*pList->pulCurrent++ = BRDC_OP_IMMS_TO_REGS(BVDC_P_RAM_TABLE_SIZE);
	*pList->pulCurrent++ =
		BRDC_REGISTER(BCHP_IT_0_MICRO_INSTRUCTIONi_ARRAY_BASE + ulOffset);

	BKNI_Memcpy((void*)pList->pulCurrent, (void*)pRamTbl,
		BVDC_P_RAM_TABLE_SIZE * sizeof(uint32_t));
	pList->pulCurrent += BVDC_P_RAM_TABLE_SIZE;

	return;
}


#if (BVDC_P_SUPPORT_3D_VIDEO)
/*************************************************************************
 *  {secret}
 *  BVDC_P_Vec_Build_It3D_isr
 **************************************************************************/
static void BVDC_P_Vec_Build_It3D_isr
	( BVDC_Display_Handle              hDisplay,
	  BVDC_P_ListInfo                 *pList )
{
	uint32_t ulLineNum, ulStartLine;
	BVDC_P_DisplayInfo  *pCurInfo = &hDisplay->stCurInfo;
	uint32_t             ulOffset = hDisplay->stAnlgChan_0.ulIt * BVDC_P_VEC_IT_SIZE;

	/* TODO */
	ulStartLine = pCurInfo->pFmtInfo->ulHeight;
	ulLineNum = ulStartLine + pCurInfo->pFmtInfo->ulActiveSpace + 1;

	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_IT_0_AS_CONTROL + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_ENUM(IT_0_AS_CONTROL, AS_INPUT_TYPE, COMBINED   ) |
		BCHP_FIELD_DATA(IT_0_AS_CONTROL, AS_NUM_LINES,  pCurInfo->pFmtInfo->ulActiveSpace) |
		BCHP_FIELD_DATA(IT_0_AS_CONTROL, AS_START_LINE, ulStartLine) |
		(BFMT_IS_3D_MODE(pCurInfo->pFmtInfo->eVideoFmt) ?
		  BCHP_FIELD_DATA(IT_0_AS_CONTROL, AS_ENABLE, 1) :
		  BCHP_FIELD_DATA(IT_0_AS_CONTROL, AS_ENABLE, 0));

	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_IT_0_AS_LINE_NUMBER + ulOffset);
	*pList->pulCurrent++ = BCHP_FIELD_DATA(IT_0_AS_LINE_NUMBER, LINE_NUMBER, ulLineNum);

	return;
}
#endif


/*************************************************************************
 *  {secret}
 *  BVDC_P_Vec_Build_IT_isr
 *  Builds IT and Video_Enc blocks (for mode switch)
 **************************************************************************/
static void BVDC_P_Vec_Build_IT_isr
	( BVDC_Display_Handle              hDisplay,
	  BVDC_P_DisplayAnlgChan          *pstChan,
	  bool                             bLoadMicrocode,
	  BVDC_P_ListInfo                 *pList )
{
	const uint32_t      *pRamTbl = NULL;
	const uint32_t      *pTable  = NULL;
	BVDC_P_DisplayInfo  *pCurInfo = &hDisplay->stCurInfo;
	uint32_t             ulOffset = pstChan->ulIt * BVDC_P_VEC_IT_SIZE;
	uint32_t             ulTopTrigVal, ulBotTrigVal;

	/* Compute Trigger Values.  For pal the Top trigger is in field1.
	   Note: it seems the new PAL microcode corrected the trigger polarity, so we
	   don't need to swap it now; */
	ulTopTrigVal = BVDC_P_TRIGGER_LINE;
	ulBotTrigVal = (pCurInfo->pFmtInfo->bInterlaced)
		? (((pCurInfo->pFmtInfo->ulScanHeight + 1) / 2) + BVDC_P_TRIGGER_LINE) : 0;

#if (BVDC_P_SUPPORT_IT_VER >= 2)
	/* Having IT trigger modulo count set to 2 means the chipset has
	 * a OSCL which produces 1080p output. BVN still runs as 1080i.
	 */
	if (1 != pCurInfo->ulTriggerModuloCnt)
		ulBotTrigVal = BVDC_P_TRIGGER_LINE;
#endif

	/* get correct ItTable, RamTable, and ItConfig */
#if DCS_SUPPORT
	if (VIDEO_FORMAT_SUPPORTS_DCS        (pCurInfo->pFmtInfo->eVideoFmt))
#else
	if (VIDEO_FORMAT_SUPPORTS_MACROVISION(pCurInfo->pFmtInfo->eVideoFmt))
#endif
	{
		pTable     = BVDC_P_GetItTableMv_isr(pCurInfo);
		pRamTbl    = BVDC_P_GetRamTableMv_isr(pCurInfo, hDisplay->bArib480p);
	}
	else
	{
		pTable     = BVDC_P_GetItTable_isr(pCurInfo);
		pRamTbl    = BVDC_P_GetRamTable_isr(pCurInfo, hDisplay->bArib480p);
	}
	BDBG_ASSERT (pTable);
	BDBG_ASSERT (pRamTbl);

	if (bLoadMicrocode)
	{
		BDBG_MSG(("Analog vec microcode - timestamp: 0x%.8x",
			pRamTbl[BVDC_P_RAM_TABLE_TIMESTAMP_IDX]));
		BDBG_MSG(("Analog vec microcode - checksum:  0x%.8x",
			pRamTbl[BVDC_P_RAM_TABLE_CHECKSUM_IDX]));
	}

	/* to support MV mode control byte change */
#if DCS_SUPPORT /** { **/

	if (bLoadMicrocode)
	{
#ifndef BVDC_P_SMOOTH_DCS /** { **/
		bool bDcsOn =
			(pCurInfo->eDcsMode != BVDC_DCS_Mode_eOff      ) &&
			(pCurInfo->eDcsMode != BVDC_DCS_Mode_eUndefined)    ;
#endif /** } ! BVDC_P_SMOOTH_DCS **/

		/* --- Setup IT block --- */
		*pList->pulCurrent++ = BRDC_OP_IMMS_TO_REGS(BVDC_P_IT_TABLE_SIZE);
		*pList->pulCurrent++ =
			BRDC_REGISTER(BCHP_IT_0_ADDR_0_3 + ulOffset);

		/* Setup IT_ADDR_0_3 to IT_STACK_REG_8_9 */
		BKNI_Memcpy((void*)pList->pulCurrent, (void*)pTable,
			BVDC_P_IT_TABLE_SIZE * sizeof(uint32_t));

		/* move pointer after adjustment */
		pList->pulCurrent += BVDC_P_IT_TABLE_SIZE;

#ifndef BVDC_P_SMOOTH_DCS /** { **/
		if (VIDEO_FORMAT_SUPPORTS_DCS (pCurInfo->pFmtInfo->eVideoFmt))
		{
			uint32_t ulItConfig = BVDC_P_GetItConfigMv_isr(pCurInfo);
			BDBG_ASSERT (ulItConfig);
			uint32_t ulItConfigOff =
				ulItConfig & ~BCHP_MASK(IT_0_TG_CONFIG, MC_ENABLES);

			BVDC_P_DCS_IT_ON_OFF_isr (
				ulOffset, pCurInfo->pFmtInfo->eVideoFmt, bDcsOn,
				&(pList->pulCurrent));
			BVDC_P_DCS_IT_Final_ON_OFF_isr (
				ulOffset, pCurInfo->pFmtInfo->eVideoFmt, bDcsOn,
				&(pList->pulCurrent));

			/* Hardware problem
			*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
			*pList->pulCurrent++ =
				BRDC_REGISTER(BCHP_IT_0_TG_CONFIG + ulOffset);
			*pList->pulCurrent++ = ulItConfigOff;
			*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
			*pList->pulCurrent++ =
				BRDC_REGISTER(BCHP_IT_0_TG_CONFIG + ulOffset);
			*pList->pulCurrent++ = ulItConfig;
			*/
		}
#endif /** } ! BVDC_P_SMOOTH_DCS **/
	}

#else /** } DCS_SUPPORT { **/

	if (VIDEO_FORMAT_SUPPORTS_MACROVISION(pCurInfo->pFmtInfo->eVideoFmt))
	{
		uint32_t ulAddr03Offset;
		uint32_t ulAddr46Offset;
		uint32_t ulPcl4Offset;
		uint32_t ulAddr03;
		uint32_t ulAddr46;
		uint32_t ulPcl4;

		/* --- Setup IT block --- */
		*pList->pulCurrent++ = BRDC_OP_IMMS_TO_REGS(BVDC_P_IT_TABLE_SIZE);
		*pList->pulCurrent++ =
			BRDC_REGISTER(BCHP_IT_0_ADDR_0_3 + ulOffset);

		/* Setup IT_ADDR_0_3 to IT_STACK_REG_8_9 */
		BKNI_Memcpy((void*)pList->pulCurrent, (void*)pTable,
			BVDC_P_IT_TABLE_SIZE * sizeof(uint32_t));

		ulAddr03Offset = 0x0;
		ulAddr46Offset =
			(BCHP_IT_0_ADDR_4_6 - BCHP_IT_0_ADDR_0_3)/sizeof(uint32_t);
		ulPcl4Offset =
			(BCHP_IT_0_PCL_4 - BCHP_IT_0_ADDR_0_3)/sizeof(uint32_t);
		ulAddr03 = pTable[ulAddr03Offset];
		ulAddr46 = pTable[ulAddr46Offset];
		ulPcl4   = pTable[ulPcl4Offset];

		/* if toggle N0 control bits for CS(MC4/5) or BP(PCL_4) */
		if(!pCurInfo->stN0Bits.bBp)
		{
			ulPcl4 &= ~(BCHP_MASK(IT_0_PCL_4, PSB_AND_TERM_2));
			ulPcl4 |= BCHP_FIELD_ENUM(IT_0_PCL_4, PSB_AND_TERM_2, ZERO);
			*(pList->pulCurrent + ulPcl4Offset) = ulPcl4;
		}
		if(!pCurInfo->stN0Bits.bPsAgc)
		{
			ulAddr03 &= ~(
				BCHP_MASK(IT_0_ADDR_0_3, MC_2_START_ADDR) |
				BCHP_MASK(IT_0_ADDR_0_3, MC_3_START_ADDR));
			ulAddr03 |= (
				BCHP_FIELD_DATA(IT_0_ADDR_0_3, MC_2_START_ADDR, 0xfd) |
				BCHP_FIELD_DATA(IT_0_ADDR_0_3, MC_3_START_ADDR, 0xfd));

			*(pList->pulCurrent + ulAddr03Offset) = ulAddr03;
		}
		if(!pCurInfo->stN0Bits.bCs && (!(VIDEO_FORMAT_IS_SECAM(pCurInfo->pFmtInfo->eVideoFmt))))
		{
			ulAddr46 &= ~(
				BCHP_MASK(IT_0_ADDR_4_6, MC_4_START_ADDR) |
				BCHP_MASK(IT_0_ADDR_4_6, MC_5_START_ADDR));
			ulAddr46 |= (
				BCHP_FIELD_DATA(IT_0_ADDR_4_6, MC_4_START_ADDR, 0xfd) |
				BCHP_FIELD_DATA(IT_0_ADDR_4_6, MC_5_START_ADDR, 0xfd));

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
			BRDC_REGISTER(BCHP_IT_0_ADDR_0_3 + ulOffset);

		/* Setup IT_ADDR_0_3 to IT_STACK_REG_8_9 */
		BKNI_Memcpy((void*)pList->pulCurrent, (void*)pTable,
			BVDC_P_IT_TABLE_SIZE * sizeof(uint32_t));

		/* move pointer after adjustment */
		pList->pulCurrent += BVDC_P_IT_TABLE_SIZE;
	}

#endif /** } !DCS_SUPPORT **/

	if(hDisplay->bAnlgEnable)
	{
		/* only program this for analog master */
		/* program PCL_0 for separate h/v sync outputs for DTV application. */
		BVDC_P_Vec_Build_SyncPCL_isr(hDisplay, pList);
	}

#if (BVDC_P_SUPPORT_3D_VIDEO)
	BVDC_P_Vec_Build_It3D_isr(hDisplay, pList);
#endif

#if (BVDC_P_SUPPORT_TDAC_VER >= BVDC_P_SUPPORT_TDAC_VER_9)
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_IT_0_CABLE_DETECT_SEL + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(IT_0_CABLE_DETECT_SEL, LINE,
			(VIDEO_FORMAT_IS_NTSC(pCurInfo->pFmtInfo->eVideoFmt)   ? 4 :
			(VIDEO_FORMAT_IS_HD(pCurInfo->pFmtInfo->eVideoFmt) &&
			 !VIDEO_FORMAT_IS_ED(pCurInfo->pFmtInfo->eVideoFmt))   ? 6 :
			(VIDEO_FORMAT_IS_480P(pCurInfo->pFmtInfo->eVideoFmt)) ? 7 : 1)) |
		BCHP_FIELD_ENUM(IT_0_CABLE_DETECT_SEL, SEL,  FLAG0          );
#endif

	if (bLoadMicrocode)
	{
		BVDC_P_Display_Build_ItBvbSize_isr(hDisplay, pList, ulOffset);

		/* Setup Vec triggers */
		/* Must be in the Rul, otherwise the Reset will destroy it */
		*pList->pulCurrent++ = BRDC_OP_IMMS_TO_REGS(2);
		*pList->pulCurrent++ =
			BRDC_REGISTER(BCHP_IT_0_VEC_TRIGGER_0 + ulOffset);

		/* Set 1st trigger */
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(IT_0_VEC_TRIGGER_0, TRIGGER_VALUE, ulTopTrigVal) |
#if (BVDC_P_SUPPORT_IT_VER >= 2)
			BCHP_FIELD_DATA(IT_0_VEC_TRIGGER_0, TRIGGER_SELECT, 0) |
			BCHP_FIELD_DATA(IT_0_VEC_TRIGGER_0, MODULO_COUNT, pCurInfo->ulTriggerModuloCnt) |
#endif
			BCHP_FIELD_DATA(IT_0_VEC_TRIGGER_0, ENABLE, 0);

		/* Set 2nd trigger (for interlaced modes) */
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(IT_0_VEC_TRIGGER_1, TRIGGER_VALUE, ulBotTrigVal) |
#if (BVDC_P_SUPPORT_IT_VER >= 2)
			BCHP_FIELD_DATA(IT_0_VEC_TRIGGER_1, TRIGGER_SELECT, (1 != pCurInfo->ulTriggerModuloCnt) ? 1 : 0) |
			BCHP_FIELD_DATA(IT_0_VEC_TRIGGER_1, MODULO_COUNT, pCurInfo->ulTriggerModuloCnt) |
#endif
			BCHP_FIELD_DATA(IT_0_VEC_TRIGGER_1, ENABLE, 0);

		BVDC_P_Vec_Build_ItMc_isr(pList, pRamTbl, ulOffset);
	}

	return;
}


void BVDC_P_ProgramAnalogChan
	( BVDC_Display_Handle                 hDisplay,
	  BVDC_P_DisplayAnlgChan             *pstChan,
	  bool                                bReloadMicrocode,
	  BVDC_P_ListInfo                    *pList )
{
	/*
	 * Program IT and RM for analog channel 0. Analog channel 1 shares
	 * the same IT and RM with channel 0.
	 *
	 * Note: programming cores like CSC, SRC, SM and VF depends on
	 * the value of output color space, which is computed from DAC settings.
	 * Hence those cores get programmed at BVDC_P_Display_Apply_DAC_Setting().
	 */
	if (pstChan->ulId == 0)
	{
		BVDC_P_Vec_Build_IT_isr(hDisplay, pstChan, bReloadMicrocode, pList);
		BVDC_P_Vec_Build_RM_isr(hDisplay, pstChan, pList);
		BVDC_P_Display_SetupAnlgTG_isr(hDisplay, pList);
	}

	return;
}

void BVDC_P_ResetDviChanInfo
	( BVDC_P_DisplayDviChan           *pstChan )
{
	pstChan->bEnable    = false; /* off */
	pstChan->ulDvi       = BVDC_P_HW_ID_INVALID;

	return;
}

BERR_Code BVDC_P_AllocDviChanResources
	( BVDC_P_Resource_Handle           hResource,
	  BVDC_DisplayId                   eDisplayId,
	  BVDC_P_DisplayDviChan           *pstChan,
	  uint32_t                         ulSrcId )
{
	BERR_Code err = BERR_SUCCESS;
	BSTD_UNUSED(ulSrcId);

	if(pstChan->ulDvi == BVDC_P_HW_ID_INVALID)
	{
		err = BVDC_P_Resource_AcquireHwId_isr(hResource, BVDC_P_ResourceType_eDvi, 0, eDisplayId, &pstChan->ulDvi, false);
		if (err)
		{
			BDBG_ERR(("No DVI block available for display %d. Check hardware capability.", eDisplayId));
		}
		else
		{
			BDBG_MSG(("\t ulDvi = %d", pstChan->ulDvi));
		}
	}
	return BERR_TRACE(err);
}

void BVDC_P_FreeDviChanResources
	( BVDC_P_Resource_Handle           hResource,
	  BVDC_P_DisplayDviChan           *pstChan )
{
	if (pstChan->ulDvi != BVDC_P_HW_ID_INVALID)
	{
		BVDC_P_Resource_ReleaseHwId_isr(hResource, BVDC_P_ResourceType_eDvi, pstChan->ulDvi);
		pstChan->ulDvi = BVDC_P_HW_ID_INVALID;
	}

	return;
}

static void BVDC_P_Vec_Build_Hdmi_isr
	( BVDC_Display_Handle              hDisplay,
	  BVDC_P_ListInfo                 *pList )
{
	uint32_t               ulPixelClkRate;
	const BVDC_P_RateInfo *pRmInfo;
	BAVC_VdcDisplay_Info   lRateInfo;
	const uint32_t        *pRmTable;
	BVDC_P_DisplayInfo    *pCurInfo = &hDisplay->stCurInfo;
#if (BVDC_P_SUPPORT_DVI_65NM)
	uint32_t              ulNdivMod , ulIcpx , ulCn;
#endif

	if(BVDC_P_GetRmTable_isr(pCurInfo, pCurInfo->pFmtInfo, &pRmTable, pCurInfo->bFullRate, &lRateInfo) != BERR_SUCCESS)
		BDBG_ASSERT(0);
	pCurInfo->stRateInfo = lRateInfo;
	pCurInfo->ulVertFreq = lRateInfo.ulVertRefreshRate;

	/* Analog VEC operates at double rate for 480P and 576P. So we have to
	 * convert back to single rate frequency to access the HDMI rate table. */
	ulPixelClkRate = pCurInfo->stRateInfo.ulPixelClkRate;
#if BVDC_P_ORTHOGONAL_VEC
	if (VIDEO_FORMAT_IS_ED(pCurInfo->pFmtInfo->eVideoFmt))
#else
	if ((VIDEO_FORMAT_IS_ED(pCurInfo->pFmtInfo->eVideoFmt) &&
		(BAVC_HDMI_PixelRepetition_e2x != eHdmiPixelRepetition))
#endif
	{
		if (ulPixelClkRate & BFMT_PXL_54MHz)
		{
			ulPixelClkRate &= ~BFMT_PXL_54MHz;
			ulPixelClkRate |=  BFMT_PXL_27MHz;
		}
		if (ulPixelClkRate & BFMT_PXL_54MHz_MUL_1_001)
		{
			ulPixelClkRate &= ~BFMT_PXL_54MHz_MUL_1_001;
			ulPixelClkRate |=  BFMT_PXL_27MHz_MUL_1_001;
		}
	}

	pRmInfo = BVDC_P_HdmiRmTable_isr(
				pCurInfo->pFmtInfo->eVideoFmt,
				ulPixelClkRate,
				pCurInfo->eHdmiColorDepth,
				pCurInfo->eHdmiPixelRepetition);
	BDBG_ASSERT(pRmInfo);
	BDBG_MSG(("tHdmi's RM PxlClk = %sMhz", pRmInfo->pchRate));

	/* HDMI_RM_CONTROL (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HDMI_RM_CONTROL);
	*pList->pulCurrent++ =
		BCHP_FIELD_ENUM(HDMI_RM_CONTROL, RESET,      RESET_OFF   ) |
		BCHP_FIELD_DATA(HDMI_RM_CONTROL, INT_GAIN,    4          ) |
		BCHP_FIELD_DATA(HDMI_RM_CONTROL, DIRECT_GAIN, 4          ) |
		BCHP_FIELD_ENUM(HDMI_RM_CONTROL, DITHER,      DITHER_OFF ) |
		BCHP_FIELD_ENUM(HDMI_RM_CONTROL, FREE_RUN,    TIMEBASE   ) |
		BCHP_FIELD_DATA(HDMI_RM_CONTROL, TIMEBASE,    pCurInfo->eTimeBase);

	/* HDMI_RM_RATE_RATIO (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HDMI_RM_RATE_RATIO);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(HDMI_RM_RATE_RATIO, DENOMINATOR, pRmInfo->ulDenominator);

	/* HDMI_RM_SAMPLE_INC (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HDMI_RM_SAMPLE_INC);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(HDMI_RM_SAMPLE_INC, NUMERATOR,	pRmInfo->ulNumerator) |
		BCHP_FIELD_DATA(HDMI_RM_SAMPLE_INC, SAMPLE_INC, pRmInfo->ulSampleInc);

	/* HDMI_RM_OFFSET (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HDMI_RM_OFFSET);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(HDMI_RM_OFFSET, OFFSET, pRmInfo->ulOffset);

	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HDMI_RM_FORMAT);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(HDMI_RM_FORMAT, SHIFT, pRmInfo->ulShift) |
		BCHP_FIELD_DATA(HDMI_RM_FORMAT, STABLE_COUNT, 10000);

#if (BVDC_P_SUPPORT_DVI_65NM)
	/* HDMI_TX_PHY_HDMI_TX_PHY_PLL_CFG (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HDMI_TX_PHY_HDMI_TX_PHY_PLL_CFG);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(HDMI_TX_PHY_HDMI_TX_PHY_PLL_CFG, PLL_RM_DIV, pRmInfo->ulRmDiv) |
		BCHP_FIELD_DATA(HDMI_TX_PHY_HDMI_TX_PHY_PLL_CFG, PLL_VCO_RANGE, pRmInfo->ulVcoRange) |
		BCHP_FIELD_DATA(HDMI_TX_PHY_HDMI_TX_PHY_PLL_CFG, PLL_PX_DIV, pRmInfo->ulPxDiv) |
		BCHP_FIELD_DATA(HDMI_TX_PHY_HDMI_TX_PHY_PLL_CFG, PLL_FEEDBACK_PRE_DIVIDER, pRmInfo->ulFeedbackPreDiv) |
		BCHP_FIELD_DATA(HDMI_TX_PHY_HDMI_TX_PHY_PLL_CFG, PLL_INPUT_PRE_DIVIDER, pRmInfo->ulInputPreDiv);

	if ((VIDEO_FORMAT_IS_ED(pCurInfo->pFmtInfo->eVideoFmt)) &&
	    ((BAVC_HDMI_PixelRepetition_e2x == pCurInfo->eHdmiPixelRepetition) ||
	     (BAVC_HDMI_PixelRepetition_e4x == pCurInfo->eHdmiPixelRepetition)))
	{
		ulNdivMod = 2;
		ulIcpx = 10;
		ulCn = 0;
	}
	else
	{
		ulNdivMod = 2;
		ulIcpx = 31;
		ulCn = 1;
	}

	/* HDMI_TX_PHY_HDMI_TX_PHY_CTL_1 (RW) */
	BVDC_P_RD_MOD_WR_RUL(pList->pulCurrent,
						 ~(BCHP_MASK(HDMI_TX_PHY_HDMI_TX_PHY_CTL_1, ICPX) |
						   BCHP_MASK(HDMI_TX_PHY_HDMI_TX_PHY_CTL_1, LF_ORDER) |
						   BCHP_MASK(HDMI_TX_PHY_HDMI_TX_PHY_CTL_1, RZ) |
						   BCHP_MASK(HDMI_TX_PHY_HDMI_TX_PHY_CTL_1, CZ) |
				 		   BCHP_MASK(HDMI_TX_PHY_HDMI_TX_PHY_CTL_1, CP) |
						   BCHP_MASK(HDMI_TX_PHY_HDMI_TX_PHY_CTL_1, RN_0) |
						   BCHP_MASK(HDMI_TX_PHY_HDMI_TX_PHY_CTL_1, RN_2_1) |
						   BCHP_MASK(HDMI_TX_PHY_HDMI_TX_PHY_CTL_1, CN)),
						 BCHP_FIELD_DATA(HDMI_TX_PHY_HDMI_TX_PHY_CTL_1, ICPX,
						 	ulIcpx) |
						 BCHP_FIELD_DATA(HDMI_TX_PHY_HDMI_TX_PHY_CTL_1, LF_ORDER, 1) |
						 BCHP_FIELD_DATA(HDMI_TX_PHY_HDMI_TX_PHY_CTL_1, RZ, 1) |
						 BCHP_FIELD_DATA(HDMI_TX_PHY_HDMI_TX_PHY_CTL_1, CZ, 3) |
						 BCHP_FIELD_DATA(HDMI_TX_PHY_HDMI_TX_PHY_CTL_1, CP, 1) |
						 BCHP_FIELD_DATA(HDMI_TX_PHY_HDMI_TX_PHY_CTL_1, RN_0, 1) |
						 BCHP_FIELD_DATA(HDMI_TX_PHY_HDMI_TX_PHY_CTL_1, RN_2_1, 0) |
						 BCHP_FIELD_DATA(HDMI_TX_PHY_HDMI_TX_PHY_CTL_1, CN,
						 	ulCn),
						 BCHP_HDMI_TX_PHY_HDMI_TX_PHY_CTL_1);

	/* HDMI_TX_PHY_HDMI_TX_PHY_CTL_2 (RW) */
	BVDC_P_RD_MOD_WR_RUL(pList->pulCurrent,
						 ~(BCHP_MASK(HDMI_TX_PHY_HDMI_TX_PHY_CTL_2, KVCO_XS) |
						   BCHP_MASK(HDMI_TX_PHY_HDMI_TX_PHY_CTL_2, NDIV_MOD)),
						 BCHP_FIELD_DATA(HDMI_TX_PHY_HDMI_TX_PHY_CTL_2, KVCO_XS, pRmInfo->ulKVcoXs) |
						 BCHP_FIELD_DATA(HDMI_TX_PHY_HDMI_TX_PHY_CTL_2,
						 	NDIV_MOD, ulNdivMod),
						 BCHP_HDMI_TX_PHY_HDMI_TX_PHY_CTL_2);
#else
#if(BVDC_P_SUPPORT_HDMI_RM_VER <= BVDC_P_HDMI_RM_VER_5)
	/* HDMI_TX_PHY_CK_DIV (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HDMI_TX_PHY_CK_DIV);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(HDMI_TX_PHY_CK_DIV, AUX,                    0) |
		BCHP_FIELD_DATA(HDMI_TX_PHY_CK_DIV, EXT,                    0) |
		BCHP_FIELD_DATA(HDMI_TX_PHY_CK_DIV, VCO,  pRmInfo->ulVcoRange) |
		BCHP_FIELD_DATA(HDMI_TX_PHY_CK_DIV, RM,   pRmInfo->ulRmDiv   );

	/* HDMI_TX_PHY_PLL_CFG (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HDMI_TX_PHY_PLL_CFG);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(HDMI_TX_PHY_PLL_CFG, LOCAL_BYPASS_CONTROL,    0) |
		BCHP_FIELD_DATA(HDMI_TX_PHY_PLL_CFG, BYPASS_OVERRIDE_CONTROL, 0) |
		BCHP_FIELD_DATA(HDMI_TX_PHY_PLL_CFG, RNDGEN_START,            0) |
		BCHP_FIELD_DATA(HDMI_TX_PHY_PLL_CFG, PDIV,     pRmInfo->ulPxDiv);

	/* HDMI_TX_PHY_CTL_2 (RW) */
	BVDC_P_RD_MOD_WR_RUL(pList->pulCurrent,
						 ~(BCHP_MASK(HDMI_TX_PHY_CTL_2, KP) |
						   BCHP_MASK(HDMI_TX_PHY_CTL_2, KI) |
						   BCHP_MASK(HDMI_TX_PHY_CTL_2, KA)),
						 BCHP_FIELD_DATA(HDMI_TX_PHY_CTL_2, KP, pRmInfo->ulKP) |
						 BCHP_FIELD_DATA(HDMI_TX_PHY_CTL_2, KI, pRmInfo->ulKI) |
						 BCHP_FIELD_DATA(HDMI_TX_PHY_CTL_2, KA, pRmInfo->ulKA),
						 BCHP_HDMI_TX_PHY_CTL_2);

	/* HDMI_TX_PHY_MDIV_LOAD (RW) */
	BVDC_P_RD_MOD_WR_RUL(pList->pulCurrent,
						 ~(BCHP_MASK(HDMI_TX_PHY_MDIV_LOAD, VCO) |
						   BCHP_MASK(HDMI_TX_PHY_MDIV_LOAD,  RM)),
						 BCHP_FIELD_DATA(HDMI_TX_PHY_MDIV_LOAD, VCO, 1) |
						 BCHP_FIELD_DATA(HDMI_TX_PHY_MDIV_LOAD, RM,  1),
						 BCHP_HDMI_TX_PHY_MDIV_LOAD);

	/* HW7422-770: Instead of wait for 1 vysnc, add few noops */
	BVDC_P_BUILD_NO_OPS(pList->pulCurrent);
	BVDC_P_BUILD_NO_OPS(pList->pulCurrent);
	BVDC_P_BUILD_NO_OPS(pList->pulCurrent);

	BVDC_P_RD_MOD_WR_RUL(pList->pulCurrent,
						 ~(BCHP_MASK(HDMI_TX_PHY_MDIV_LOAD, VCO) |
						   BCHP_MASK(HDMI_TX_PHY_MDIV_LOAD,  RM)),
						 BCHP_FIELD_DATA(HDMI_TX_PHY_MDIV_LOAD, VCO, 0) |
						 BCHP_FIELD_DATA(HDMI_TX_PHY_MDIV_LOAD, RM,  0),
						 BCHP_HDMI_TX_PHY_MDIV_LOAD);
#else
	/* TODO: Need to program new HDMI_TX_PHY block */
	/* HDMI_TX_PHY_CKK_DIV (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HDMI_TX_PHY_CLK_DIV);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(HDMI_TX_PHY_CLK_DIV, AUX,                    0) |
		BCHP_FIELD_DATA(HDMI_TX_PHY_CLK_DIV, VCO,  pRmInfo->ulVcoRange) |
		BCHP_FIELD_DATA(HDMI_TX_PHY_CLK_DIV, RM,   pRmInfo->ulRmDiv   );
#endif
#endif

	hDisplay->bRateManagerUpdated = true;

	return;
}

static void BVDC_P_Vec_Build_DVI_DTG_isr
	( BVDC_Display_Handle              hDisplay,
	  bool                             bReloadMicrocode,
	  BVDC_P_ListInfo                 *pList )
{
	const uint32_t *pDtRam;
	uint32_t ulTopTrigVal, ulBotTrigVal;
	const BVDC_P_DisplayInfo *pCurInfo = &hDisplay->stCurInfo;
	uint32_t ulHeight = pCurInfo->pFmtInfo->ulDigitalHeight>>(pCurInfo->pFmtInfo->bInterlaced);

	if(BFMT_IS_3D_MODE(pCurInfo->pFmtInfo->eVideoFmt))
	{
		ulHeight = ulHeight * 2 + pCurInfo->pFmtInfo->ulActiveSpace;
	}

	/* Dtram[40..7f] is for DVI */
	pDtRam = BVDC_P_GetDtramTable_isr(pCurInfo, pCurInfo->pFmtInfo, hDisplay->bArib480p);
	BDBG_MSG(("Dtram microcode - timestamp: 0x%.8x", pDtRam[BVDC_P_DTRAM_TABLE_TIMESTAMP_IDX]));
	BDBG_MSG(("Dtram microcode - checksum:  0x%.8x", pDtRam[BVDC_P_DTRAM_TABLE_CHECKSUM_IDX]));

	/*-------------------------------*/
	/* NOTE: Toggle Reset first */
	/*-------------------------------*/


	if (bReloadMicrocode)
	{
		/* Load uCode */
		*pList->pulCurrent++ = BRDC_OP_IMMS_TO_REGS(BVDC_P_DTRAM_TABLE_SIZE);
		*pList->pulCurrent++ =
			BRDC_REGISTER(BCHP_DTRAM_0_DMC_INSTRUCTIONi_ARRAY_BASE +
				(BVDC_P_DVI_DTRAM_START_ADDR * sizeof(uint32_t)));
		BKNI_Memcpy((void*)pList->pulCurrent, (void*)pDtRam,
			BVDC_P_DTRAM_TABLE_SIZE * sizeof(uint32_t));
		pList->pulCurrent += BVDC_P_DTRAM_TABLE_SIZE;
	}

	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_DTRAM_0_DTRAM_CONFIG);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(DTRAM_0_DTRAM_CONFIG, ARBITER_LATENCY, 0xb);

	/* DVI_DTG_DTG_BVB (RW) */
	/* DVI_DTG_CCIR_PCL (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_DVI_DTG_0_CCIR_PCL);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(DVI_DTG_0_CCIR_PCL, HACTIVE_ENABLE,  1) | /* nominal */
		BCHP_FIELD_DATA(DVI_DTG_0_CCIR_PCL, HACTIVE_SEL,     0) | /* nominal */
		BCHP_FIELD_DATA(DVI_DTG_0_CCIR_PCL, VACTIVE_ENABLE,  1) | /* nominal */
		BCHP_FIELD_DATA(DVI_DTG_0_CCIR_PCL, VACTIVE_SEL,     0) | /* nominal */
		BCHP_FIELD_DATA(DVI_DTG_0_CCIR_PCL, VBLANK_ENABLE,   1) | /* nominal */
		BCHP_FIELD_DATA(DVI_DTG_0_CCIR_PCL, VBLANK_SEL,      0) | /* nominal */
		BCHP_FIELD_DATA(DVI_DTG_0_CCIR_PCL, ODD_EVEN_ENABLE, 1) | /* nominal */
		BCHP_FIELD_DATA(DVI_DTG_0_CCIR_PCL, ODD_EVEN_SEL,    0);  /* nominal */

	/* DVI_DTG_DVI_PCL (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_DVI_DTG_0_DVI_PCL);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(DVI_DTG_0_DVI_PCL, DIGITAL_HSYNC_ENABLE, 1) | /* nominal */
		BCHP_FIELD_DATA(DVI_DTG_0_DVI_PCL, DIGITAL_HSYNC_SEL,    0) | /* nominal */
		BCHP_FIELD_DATA(DVI_DTG_0_DVI_PCL, DVI_V_ENABLE,         1) | /* nominal */
		BCHP_FIELD_DATA(DVI_DTG_0_DVI_PCL, DVI_V_SEL,            1) | /* nominal */
		BCHP_FIELD_DATA(DVI_DTG_0_DVI_PCL, DVI_DE_ENABLE,        1) | /* nominal */
		BCHP_FIELD_DATA(DVI_DTG_0_DVI_PCL, DVI_DE_SEL,           3);  /* nominal */

	/* DVI_DTG_CNTL_PCL (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_DVI_DTG_0_CNTL_PCL);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(DVI_DTG_0_CNTL_PCL, NEW_LINE_CLR_SEL, 3 );  /* nominal */

	/* DVI_DTG_RAM_ADDR (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_DVI_DTG_0_RAM_ADDR);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(DVI_DTG_0_RAM_ADDR, MC_START_ADDR,
			BVDC_P_DVI_DTRAM_START_ADDR                       );  /* nominal */

	/* DVI_DTG_DTG_BVB_SIZE (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_DVI_DTG_0_DTG_BVB_SIZE);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(DVI_DTG_0_DTG_BVB_SIZE, HORIZONTAL, pCurInfo->pFmtInfo->ulDigitalWidth ) |
		BCHP_FIELD_DATA(DVI_DTG_0_DTG_BVB_SIZE, VERTICAL,   ulHeight);

	/* Set up triggers */
	ulTopTrigVal = BVDC_P_TRIGGER_LINE;
	ulBotTrigVal = (pCurInfo->pFmtInfo->bInterlaced) ?
		(((pCurInfo->pFmtInfo->ulScanHeight + 1) / 2) + BVDC_P_TRIGGER_LINE) : 0;

#if (BVDC_P_SUPPORT_IT_VER >= 2)
	/* Having IT trigger modulo count set to 2 means the chipset has
	 * a OSCL which produces 1080p output. BVN still runs as 1080i.
	 */
	if (1 != pCurInfo->ulTriggerModuloCnt)
		ulBotTrigVal = BVDC_P_TRIGGER_LINE;
#endif

	/* DVI_DTG_DTG_TRIGGER_0 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_DVI_DTG_0_DTG_TRIGGER_0);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(DVI_DTG_0_DTG_TRIGGER_0, TRIGGER_VALUE, ulTopTrigVal) |
#if (BVDC_P_SUPPORT_IT_VER >= 2)
		BCHP_FIELD_DATA(DVI_DTG_0_DTG_TRIGGER_0, TRIGGER_SELECT, 0) |
		BCHP_FIELD_DATA(DVI_DTG_0_DTG_TRIGGER_0, MODULO_COUNT, pCurInfo->ulTriggerModuloCnt) |
#endif
		BCHP_FIELD_DATA(DVI_DTG_0_DTG_TRIGGER_0, ENABLE, 0);

	/* DVI_DTG_DTG_TRIGGER_1 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_DVI_DTG_0_DTG_TRIGGER_1);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(DVI_DTG_0_DTG_TRIGGER_1, TRIGGER_VALUE, ulBotTrigVal) |
#if (BVDC_P_SUPPORT_IT_VER >= 2)
		BCHP_FIELD_DATA(DVI_DTG_0_DTG_TRIGGER_1, TRIGGER_SELECT, (1 != pCurInfo->ulTriggerModuloCnt) ? 1 : 0) |
		BCHP_FIELD_DATA(DVI_DTG_0_DTG_TRIGGER_1, MODULO_COUNT, pCurInfo->ulTriggerModuloCnt) |
#endif
		BCHP_FIELD_DATA(DVI_DTG_0_DTG_TRIGGER_1, ENABLE, 0);

#if (BVDC_P_SUPPORT_3D_VIDEO)
{
	uint32_t ulLineNum, ulStartLine;
	BVDC_P_DisplayInfo  *pCurInfo = &hDisplay->stCurInfo;

	ulStartLine = pCurInfo->pFmtInfo->ulHeight;
	ulLineNum = ulStartLine + pCurInfo->pFmtInfo->ulActiveSpace + 1;

	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_DVI_DTG_0_DTG_AS_CONTROL);
	*pList->pulCurrent++ =
		BCHP_FIELD_ENUM(DVI_DTG_0_DTG_AS_CONTROL, AS_INPUT_TYPE, COMBINED   ) |
		BCHP_FIELD_DATA(DVI_DTG_0_DTG_AS_CONTROL, AS_NUM_LINES,  pCurInfo->pFmtInfo->ulActiveSpace) |
		BCHP_FIELD_DATA(DVI_DTG_0_DTG_AS_CONTROL, AS_START_LINE, ulStartLine) |
		(BFMT_IS_3D_MODE(pCurInfo->pFmtInfo->eVideoFmt) ?
		  BCHP_FIELD_DATA(DVI_DTG_0_DTG_AS_CONTROL, AS_ENABLE, 1) :
		  BCHP_FIELD_DATA(DVI_DTG_0_DTG_AS_CONTROL, AS_ENABLE, 0));

	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_DVI_DTG_0_DTG_AS_LINE_NUMBER);
	*pList->pulCurrent++ = BCHP_FIELD_DATA(DVI_DTG_0_DTG_AS_LINE_NUMBER, LINE_NUMBER, ulLineNum);
}
#endif

	return;
}

static void BVDC_P_Vec_Build_DVI_CSC_isr
	( BVDC_Display_Handle              hDisplay,
	  BVDC_P_ListInfo                 *pList )
{
	const BVDC_P_DisplayCscMatrix *pCscMatrix = NULL;
	const BVDC_P_DisplayInfo *pCurInfo = &hDisplay->stCurInfo;
	uint8_t ucCh0, ucCh1, ucCh2;
	const BVDC_P_Compositor_Info *pCmpInfo = &hDisplay->hCompositor->stCurInfo;

	BDBG_ASSERT(stAVC_MatrixCoefficient_InfoTbl[hDisplay->stCurInfo.eHdmiOutput].eAvcCs == hDisplay->stCurInfo.eHdmiOutput);
	BDBG_MSG(("Display %d Hdmi using %s", hDisplay->eId, stAVC_MatrixCoefficient_InfoTbl[hDisplay->stCurInfo.eHdmiOutput].pcAvcCsStr));

	/* --- Setup DVI CSC --- */
	BVDC_P_Display_GetDviCscTable_isr(pCurInfo, &pCscMatrix);

	/* TODO: fold sync only into generic display output mute functionality */
	if(pCurInfo->bUserCsc)
	{
		BVDC_P_Csc_FromMatrixDvo_isr(&hDisplay->stDvoCscMatrix.stCscCoeffs,
			pCurInfo->pl32_Matrix, pCurInfo->ulUserShift,
			((pCurInfo->eHdmiOutput == BAVC_MatrixCoefficients_eHdmi_RGB) ||
			 (pCurInfo->eHdmiOutput == BAVC_MatrixCoefficients_eDvi_Full_Range_RGB))? true : false);

		hDisplay->stDvoCscMatrix.ulMin = pCscMatrix->ulMin;
		hDisplay->stDvoCscMatrix.ulMax = pCscMatrix->ulMax;
	}
	else
	{
		/* For BVDC_Display_GetDvoColorMatrix() */
		hDisplay->stDvoCscMatrix = *pCscMatrix;
	}

	/* Blank color, use in CSC and DVI_DVF_0_DVF_VALUES */
	ucCh0 = BPXL_GET_COMPONENT(BPXL_eA8_Y8_Cb8_Cr8, pCmpInfo->ulBgColorYCrCb, 2);
	ucCh1 = BPXL_GET_COMPONENT(BPXL_eA8_Y8_Cb8_Cr8, pCmpInfo->ulBgColorYCrCb, 1);
	ucCh2 = BPXL_GET_COMPONENT(BPXL_eA8_Y8_Cb8_Cr8, pCmpInfo->ulBgColorYCrCb, 0);

	if(!pCurInfo->abOutputMute[BVDC_DisplayOutput_eDvo])
	{
		BVDC_P_Csc_DvoApplyAttenuationRGB_isr(hDisplay->stCurInfo.lDvoAttenuationR,
		                                      hDisplay->stCurInfo.lDvoAttenuationG,
		                                      hDisplay->stCurInfo.lDvoAttenuationB,
		                                      hDisplay->stCurInfo.lDvoOffsetR,
		                                      hDisplay->stCurInfo.lDvoOffsetG,
		                                      hDisplay->stCurInfo.lDvoOffsetB,
		                                     &hDisplay->stDvoCscMatrix.stCscCoeffs);
	}
	else
	{
		/* Swap ch0 and 1 of input color to match vec csc layout */
		BVDC_P_Csc_ApplyYCbCrColor(&hDisplay->stDvoCscMatrix.stCscCoeffs, ucCh1, ucCh0, ucCh2);
	}

	if((BAVC_MatrixCoefficients_eHdmi_RGB == pCurInfo->eHdmiOutput) ||
	   (BAVC_MatrixCoefficients_eDvi_Full_Range_RGB == pCurInfo->eHdmiOutput))
	{
		ucCh0 = pCmpInfo->ucGreen;
		ucCh1 = pCmpInfo->ucBlue;
		ucCh2 = pCmpInfo->ucRed;
	}

	/* DVI_CSC_CSC_MODE (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMMS_TO_REGS(BVDC_P_CSC_TABLE_SIZE);
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_DVI_CSC_0_CSC_MODE);
	BVDC_P_Vec_Build_CSC_isr(&hDisplay->stDvoCscMatrix, pList);

	/* Blank padding color */
	/* DVI_DVF_DVF_VALUES (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_DVI_DVF_0_DVF_VALUES);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(DVI_DVF_0_DVF_VALUES, CH2_BLANK, ucCh2 ) |
		BCHP_FIELD_DATA(DVI_DVF_0_DVF_VALUES, CH1_BLANK, ucCh1 ) |
		BCHP_FIELD_DATA(DVI_DVF_0_DVF_VALUES, CH0_BLANK, ucCh0 );

	/* DVI_CSC_DITHER */
	if(hDisplay->stCurInfo.stDvoCfg.b8BitPanel)
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
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_DVI_CSC_0_DITHER_CONTROL);
	BVDC_P_Vec_Build_Dither_isr(&hDisplay->stDviDither, pList);

	return;
}


/*************************************************************************
 *  {secret}
 *
 *  Builds the blank color of DVI or 656 output.  Use the compositor
 *  background color.
 **************************************************************************/
static void BVDC_P_Vec_Build_DVI_DVF_isr
	( BVDC_Display_Handle              hDisplay,
	  BVDC_P_ListInfo                 *pList )
{
	uint32_t upsample2x, downsample2x;

	if (VIDEO_FORMAT_IS_ED(hDisplay->stCurInfo.pFmtInfo->eVideoFmt))
	{
		if ((BAVC_HDMI_PixelRepetition_e2x ==
			hDisplay->stCurInfo.eHdmiPixelRepetition) ||
		    (BAVC_HDMI_PixelRepetition_e4x ==
			hDisplay->stCurInfo.eHdmiPixelRepetition))
		{
			upsample2x   = BCHP_DVI_DVF_0_DVF_CONFIG_UPSAMPLE2X_ON;
			downsample2x = BCHP_DVI_DVF_0_DVF_CONFIG_DOWNSAMPLE2X_OFF;
		}
		else
		{
			upsample2x   = BCHP_DVI_DVF_0_DVF_CONFIG_UPSAMPLE2X_OFF;
			downsample2x = BCHP_DVI_DVF_0_DVF_CONFIG_DOWNSAMPLE2X_ON;
		}
	}
	else
	{
		upsample2x   =
			(VIDEO_FORMAT_IS_SD(hDisplay->stCurInfo.pFmtInfo->eVideoFmt)) ?
			BCHP_DVI_DVF_0_DVF_CONFIG_UPSAMPLE2X_ON :
			BCHP_DVI_DVF_0_DVF_CONFIG_UPSAMPLE2X_OFF ;
		downsample2x = BCHP_DVI_DVF_0_DVF_CONFIG_DOWNSAMPLE2X_OFF;
	}

	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_DVI_DVF_0_DVF_CONFIG);
	*pList->pulCurrent++ = (
		BCHP_FIELD_DATA(DVI_DVF_0_DVF_CONFIG,   UPSAMPLE2X,   upsample2x) |
		BCHP_FIELD_DATA(DVI_DVF_0_DVF_CONFIG, DOWNSAMPLE2X, downsample2x) );

	return;
}

static void BVDC_P_ProgramDviChan
	( BVDC_Display_Handle              hDisplay,
	  BVDC_P_DisplayDviChan           *pstChan,
	  bool                             bReloadMicrocode,
	  BVDC_P_ListInfo                 *pList )
{
	BSTD_UNUSED(pstChan); /* This info will be used when there are multiple DVI cores */

	BVDC_P_Vec_Build_Hdmi_isr(hDisplay, pList);
	BVDC_P_Vec_Build_DVI_DTG_isr(hDisplay, bReloadMicrocode, pList);
	BVDC_P_Vec_Build_DVI_DVF_isr(hDisplay, pList);
	BVDC_P_Vec_Build_DVI_CSC_isr(hDisplay, pList);
	BVDC_P_Display_SetupDviTG_isr(hDisplay, pList);

	return;
}

static void BVDC_P_SetupDviChan
	( BVDC_Display_Handle              hDisplay,
	  BVDC_P_DisplayDviChan           *pstChan,
	  BVDC_P_ListInfo                 *pList )
{
	BSTD_UNUSED(hDisplay);
	BSTD_UNUSED(pstChan); /* For multiple DVI instances in the future */

#if BVDC_P_SUPPORT_NEW_SW_INIT
	/* reset unreset */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VEC_CFG_SW_INIT_DVI_DTG_0);
	*pList->pulCurrent++ = BCHP_FIELD_DATA(VEC_CFG_SW_INIT_DVI_DTG_0, INIT, 1);

	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VEC_CFG_SW_INIT_DVI_CSC_0);
	*pList->pulCurrent++ = BCHP_FIELD_DATA(VEC_CFG_SW_INIT_DVI_CSC_0, INIT, 1);

	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VEC_CFG_SW_INIT_DVI_DVF_0);
	*pList->pulCurrent++ = BCHP_FIELD_DATA(VEC_CFG_SW_INIT_DVI_DVF_0, INIT, 1);

	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VEC_CFG_SW_INIT_DVI_MISC_0);
	*pList->pulCurrent++ = BCHP_FIELD_DATA(VEC_CFG_SW_INIT_DVI_MISC_0, INIT, 1);

	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VEC_CFG_SW_INIT_DVI_DTG_0);
	*pList->pulCurrent++ = BCHP_FIELD_DATA(VEC_CFG_SW_INIT_DVI_DTG_0, INIT, 0);

	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VEC_CFG_SW_INIT_DVI_CSC_0);
	*pList->pulCurrent++ = BCHP_FIELD_DATA(VEC_CFG_SW_INIT_DVI_CSC_0, INIT, 0);

	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VEC_CFG_SW_INIT_DVI_DVF_0);
	*pList->pulCurrent++ = BCHP_FIELD_DATA(VEC_CFG_SW_INIT_DVI_DVF_0, INIT, 0);

	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VEC_CFG_SW_INIT_DVI_MISC_0);
	*pList->pulCurrent++ = BCHP_FIELD_DATA(VEC_CFG_SW_INIT_DVI_MISC_0, INIT, 0);
#else
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VEC_CFG_SW_RESET_DVI_DTG_0);
	*pList->pulCurrent++ = BCHP_FIELD_DATA(VEC_CFG_SW_RESET_DVI_DTG_0, RESET, 1);

	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VEC_CFG_SW_RESET_DVI_CSC_0);
	*pList->pulCurrent++ = BCHP_FIELD_DATA(VEC_CFG_SW_RESET_DVI_CSC_0, RESET, 1);

	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VEC_CFG_SW_RESET_DVI_DVF_0);
	*pList->pulCurrent++ = BCHP_FIELD_DATA(VEC_CFG_SW_RESET_DVI_DVF_0, RESET, 1);

	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VEC_CFG_SW_RESET_DVI_MISC_0);
	*pList->pulCurrent++ = BCHP_FIELD_DATA(VEC_CFG_SW_RESET_DVI_MISC_0, RESET, 1);

	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VEC_CFG_SW_RESET_DVI_DTG_0);
	*pList->pulCurrent++ = BCHP_FIELD_DATA(VEC_CFG_SW_RESET_DVI_DTG_0, RESET, 0);

	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VEC_CFG_SW_RESET_DVI_CSC_0);
	*pList->pulCurrent++ = BCHP_FIELD_DATA(VEC_CFG_SW_RESET_DVI_CSC_0, RESET, 0);

	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VEC_CFG_SW_RESET_DVI_DVF_0);
	*pList->pulCurrent++ = BCHP_FIELD_DATA(VEC_CFG_SW_RESET_DVI_DVF_0, RESET, 0);

	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VEC_CFG_SW_RESET_DVI_MISC_0);
	*pList->pulCurrent++ = BCHP_FIELD_DATA(VEC_CFG_SW_RESET_DVI_MISC_0, RESET, 0);
#endif

	return;
}

static uint32_t BVDC_P_GetDviSrc
	( BVDC_Display_Handle              hDisplay )
{
	uint32_t ulDviSrc = 0;

	if(hDisplay->stCurInfo.aulEnableMpaaDeci[BVDC_MpaaDeciIf_eHdmi])
	{
		ulDviSrc = BCHP_VEC_CFG_DVI_DTG_0_SOURCE_SOURCE_DECIM_0;
	}
	else
	{
		ulDviSrc = BVDC_P_GetVecCfgSrc(hDisplay);
	}

	return ulDviSrc;
}

static void BVDC_P_ConnectDviSrc
	( BVDC_Display_Handle              hDisplay,
	  BVDC_P_DisplayDviChan           *pstChan,
	  BVDC_P_ListInfo                 *pList )
{
	uint32_t ulDviSrc = BVDC_P_GetDviSrc(hDisplay);

	BSTD_UNUSED(pstChan); /* For multiple DVI instances in the future */

	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VEC_CFG_DVI_DTG_0_SOURCE);
	*pList->pulCurrent++ =
			BCHP_FIELD_DATA(VEC_CFG_DVI_DTG_0_SOURCE, SOURCE, ulDviSrc);

	return;
}

static void BVDC_P_TearDownDviChan
	( BVDC_Display_Handle              hDisplay,
	  BVDC_P_DisplayDviChan           *pstChan,
	  BVDC_P_ListInfo                 *pList )
{
	BSTD_UNUSED(hDisplay);
	BSTD_UNUSED(pstChan); /* For multiple DVI instances in the future */

	/* Disable DVI source */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VEC_CFG_DVI_DTG_0_SOURCE);
	*pList->pulCurrent++ =
			BCHP_FIELD_DATA(VEC_CFG_DVI_DTG_0_SOURCE, SOURCE, BCHP_VEC_CFG_DVI_DTG_0_SOURCE_SOURCE_DISABLE);

	return;
}

void BVDC_P_Reset656ChanInfo
	( BVDC_P_Display656Chan           *pstChan )
{
	pstChan->bEnable    = false; /* off */
	pstChan->ul656       = BVDC_P_HW_ID_INVALID;

	return;
}

#if (BVDC_P_SUPPORT_ITU656_OUT != 0)
BERR_Code BVDC_P_Alloc656ChanResources
	( BVDC_P_Resource_Handle           hResource,
	  BVDC_DisplayId                   eDisplayId,
	  BVDC_P_Display656Chan           *pstChan,
	  uint32_t                         ulSrcId )
{
	BERR_Code err;

	BSTD_UNUSED(ulSrcId);

	err = BVDC_P_Resource_AcquireHwId_isr(hResource, BVDC_P_ResourceType_e656, 0, eDisplayId, &pstChan->ul656, false);

	if (err)
	{
		BDBG_ERR(("No 656 available"));
	}

	return BERR_TRACE(err);
}

void BVDC_P_Free656ChanResources
	( BVDC_P_Resource_Handle           hResource,
	  BVDC_P_Display656Chan           *pstChan )
{

	if (pstChan->ul656 != BVDC_P_HW_ID_INVALID)
	{
		BVDC_P_Resource_ReleaseHwId_isr(hResource, BVDC_P_ResourceType_e656, pstChan->ul656);
		pstChan->ul656 = BVDC_P_HW_ID_INVALID;
	}

	return;
}

static void BVDC_P_TearDown656Chan
	( BVDC_Display_Handle              hDisplay,
	  BVDC_P_Display656Chan           *pstChan,
	  BVDC_P_ListInfo                 *pList )
{
	BSTD_UNUSED(hDisplay);
	BSTD_UNUSED(pstChan); /* For multiple 656 instances in the future */

	/* Disable 656 source */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VEC_CFG_ITU656_DTG_0_SOURCE);
	*pList->pulCurrent++ =
			BCHP_FIELD_DATA(VEC_CFG_ITU656_DTG_0_SOURCE, SOURCE, BCHP_VEC_CFG_ITU656_DTG_0_SOURCE_SOURCE_DISABLE);

	return;
}

static void BVDC_P_Setup656Chan
	( BVDC_Display_Handle              hDisplay,
	  BVDC_P_Display656Chan           *pstChan,
	  BVDC_P_ListInfo                 *pList )
{
	BSTD_UNUSED(hDisplay);
	BSTD_UNUSED(pstChan); /* For multiple 656 instances in the future */

	/* reset */
#if BVDC_P_SUPPORT_NEW_SW_INIT
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VEC_CFG_SW_INIT_ITU656_DTG_0);
	*pList->pulCurrent++ = BCHP_FIELD_DATA(VEC_CFG_SW_INIT_ITU656_DTG_0, INIT, 1);

	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VEC_CFG_SW_INIT_ITU656_CSC_0);
	*pList->pulCurrent++ = BCHP_FIELD_DATA(VEC_CFG_SW_INIT_ITU656_CSC_0, INIT, 1);

	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VEC_CFG_SW_INIT_ITU656_DVF_0);
	*pList->pulCurrent++ = BCHP_FIELD_DATA(VEC_CFG_SW_INIT_ITU656_DVF_0, INIT, 1);

	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VEC_CFG_SW_INIT_ITU656_FORMATTER_0);
	*pList->pulCurrent++ = BCHP_FIELD_DATA(VEC_CFG_SW_INIT_ITU656_FORMATTER_0, INIT, 1);

	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VEC_CFG_SW_INIT_ITU656_DTG_0);
	*pList->pulCurrent++ = BCHP_FIELD_DATA(VEC_CFG_SW_INIT_ITU656_DTG_0, INIT, 0);

	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VEC_CFG_SW_INIT_ITU656_CSC_0);
	*pList->pulCurrent++ = BCHP_FIELD_DATA(VEC_CFG_SW_INIT_ITU656_CSC_0, INIT, 0);

	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VEC_CFG_SW_INIT_ITU656_DVF_0);
	*pList->pulCurrent++ = BCHP_FIELD_DATA(VEC_CFG_SW_INIT_ITU656_DVF_0, INIT, 0);

	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VEC_CFG_SW_INIT_ITU656_FORMATTER_0);
	*pList->pulCurrent++ = BCHP_FIELD_DATA(VEC_CFG_SW_INIT_ITU656_FORMATTER_0, INIT, 0);
#else
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VEC_CFG_SW_RESET_ITU656_DTG_0);
	*pList->pulCurrent++ = BCHP_FIELD_DATA(VEC_CFG_SW_RESET_ITU656_DTG_0, RESET, 1);

	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VEC_CFG_SW_RESET_ITU656_CSC_0);
	*pList->pulCurrent++ = BCHP_FIELD_DATA(VEC_CFG_SW_RESET_ITU656_CSC_0, RESET, 1);

	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VEC_CFG_SW_RESET_ITU656_DVF_0);
	*pList->pulCurrent++ = BCHP_FIELD_DATA(VEC_CFG_SW_RESET_ITU656_DVF_0, RESET, 1);

	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VEC_CFG_SW_RESET_ITU656_FORMATTER_0);
	*pList->pulCurrent++ = BCHP_FIELD_DATA(VEC_CFG_SW_RESET_ITU656_FORMATTER_0, RESET, 1);

	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VEC_CFG_SW_RESET_ITU656_DTG_0);
	*pList->pulCurrent++ = BCHP_FIELD_DATA(VEC_CFG_SW_RESET_ITU656_DTG_0, RESET, 0);

	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VEC_CFG_SW_RESET_ITU656_CSC_0);
	*pList->pulCurrent++ = BCHP_FIELD_DATA(VEC_CFG_SW_RESET_ITU656_CSC_0, RESET, 0);

	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VEC_CFG_SW_RESET_ITU656_DVF_0);
	*pList->pulCurrent++ = BCHP_FIELD_DATA(VEC_CFG_SW_RESET_ITU656_DVF_0, RESET, 0);

	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VEC_CFG_SW_RESET_ITU656_FORMATTER_0);
	*pList->pulCurrent++ = BCHP_FIELD_DATA(VEC_CFG_SW_RESET_ITU656_FORMATTER_0, RESET, 0);
#endif

	return;
}

static void BVDC_P_Connect656Src
	( BVDC_Display_Handle              hDisplay,
	  BVDC_P_Display656Chan           *pstChan,
	  BVDC_P_ListInfo                 *pList )
{
	uint32_t ulSrc = 0;

	BSTD_UNUSED(pstChan); /* For multiple 656 instances in the future */

	/* Connect 656 to source */
	ulSrc = BVDC_P_GetVecCfgSrc(hDisplay);
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VEC_CFG_ITU656_DTG_0_SOURCE);
	*pList->pulCurrent++ =
			BCHP_FIELD_DATA(VEC_CFG_ITU656_DTG_0_SOURCE, SOURCE, ulSrc);

	return;
}

static void BVDC_P_Vec_Build_656_DTG_isr
	( BVDC_Display_Handle              hDisplay,
	  BVDC_P_ListInfo                 *pList )
{
	const uint32_t *p656Dtram = NULL;
	uint32_t ulTopTrigVal, ulBotTrigVal;
	const BVDC_P_DisplayInfo *pCurInfo = &hDisplay->stCurInfo;
	uint32_t ulNumRegs;

	/* get correct 656 dtram */
	p656Dtram = BVDC_P_Get656DtramTable_isr(pCurInfo);

	/* Load uCode */
	/* Dtram[0..3f] is reserved for 656 */
	*pList->pulCurrent++ = BRDC_OP_IMMS_TO_REGS(BVDC_P_DTRAM_TABLE_SIZE);
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_DTRAM_0_DMC_INSTRUCTIONi_ARRAY_BASE);
	BKNI_Memcpy((void*)pList->pulCurrent, (void*)p656Dtram,
		BVDC_P_DTRAM_TABLE_SIZE * sizeof(uint32_t));
	pList->pulCurrent += BVDC_P_DTRAM_TABLE_SIZE;

	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_DTRAM_0_DTRAM_CONFIG);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(DTRAM_0_DTRAM_CONFIG, ARBITER_LATENCY, 0xb);

	ulNumRegs = (BCHP_ITU656_DTG_0_DTG_BVB_SIZE - BCHP_ITU656_DTG_0_CCIR_PCL)/4 + 1;

	*pList->pulCurrent++ = BRDC_OP_IMMS_TO_REGS(ulNumRegs);
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_ITU656_DTG_0_CCIR_PCL);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(ITU656_DTG_0_CCIR_PCL, HACTIVE_ENABLE,  1) |
		BCHP_FIELD_DATA(ITU656_DTG_0_CCIR_PCL, HACTIVE_SEL,     0) |
		BCHP_FIELD_DATA(ITU656_DTG_0_CCIR_PCL, VACTIVE_ENABLE,  1) |
		BCHP_FIELD_DATA(ITU656_DTG_0_CCIR_PCL, VACTIVE_SEL,     0) |
		BCHP_FIELD_DATA(ITU656_DTG_0_CCIR_PCL, VBLANK_ENABLE,   1) |
		BCHP_FIELD_DATA(ITU656_DTG_0_CCIR_PCL, VBLANK_SEL,      0) |
		BCHP_FIELD_DATA(ITU656_DTG_0_CCIR_PCL, ODD_EVEN_ENABLE, 1) |
		BCHP_FIELD_DATA(ITU656_DTG_0_CCIR_PCL, ODD_EVEN_SEL,    0);

	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(ITU656_DTG_0_DVI_PCL, DIGITAL_HSYNC_ENABLE, 1) |
		BCHP_FIELD_DATA(ITU656_DTG_0_DVI_PCL, DIGITAL_HSYNC_SEL,    0) |
		BCHP_FIELD_DATA(ITU656_DTG_0_DVI_PCL, DVI_V_ENABLE,     0) |
		BCHP_FIELD_DATA(ITU656_DTG_0_DVI_PCL, DVI_V_SEL,        0) |
		BCHP_FIELD_DATA(ITU656_DTG_0_DVI_PCL, DVI_DE_ENABLE,    0) |
		BCHP_FIELD_DATA(ITU656_DTG_0_DVI_PCL, DVI_DE_SEL,       0);

	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(ITU656_DTG_0_CNTL_PCL, NEW_LINE_CLR_SEL, 3);

	/* DTG_RAM_ADDR = 0 for 656out */
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(ITU656_DTG_0_RAM_ADDR, MC_START_ADDR, 0);

	/* DTG_BVB_SIZE (RW) */
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(ITU656_DTG_0_DTG_BVB_SIZE, HORIZONTAL,pCurInfo->pFmtInfo->ulDigitalWidth) |
		BCHP_FIELD_DATA(ITU656_DTG_0_DTG_BVB_SIZE, VERTICAL,  pCurInfo->pFmtInfo->ulDigitalHeight>>(pCurInfo->pFmtInfo->bInterlaced));

	/* Set up triggers */
	ulTopTrigVal = BVDC_P_TRIGGER_LINE;
	ulBotTrigVal = (pCurInfo->pFmtInfo->bInterlaced) ?
		(((pCurInfo->pFmtInfo->ulScanHeight + 1) / 2) + BVDC_P_TRIGGER_LINE) : 0;
#if (BVDC_P_SUPPORT_IT_VER >= 2)
	/* Having IT trigger modulo count set to 2 means the chipset has
	 * a OSCL which produces 1080p output. BVN still runs as 1080i.
	 */
	if (1 != pCurInfo->ulTriggerModuloCnt)
		ulBotTrigVal = BVDC_P_TRIGGER_LINE;
#endif

	/* DTG_TRIGGER_0 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_ITU656_DTG_0_DTG_TRIGGER_0);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(ITU656_DTG_0_DTG_TRIGGER_0, ENABLE, 0 ) |
#if (BVDC_P_SUPPORT_IT_VER >= 2)
		BCHP_FIELD_DATA(ITU656_DTG_0_DTG_TRIGGER_0, TRIGGER_SELECT, 0) |
		BCHP_FIELD_DATA(ITU656_DTG_0_DTG_TRIGGER_0, MODULO_COUNT, pCurInfo->ulTriggerModuloCnt) |
#endif
		BCHP_FIELD_DATA(ITU656_DTG_0_DTG_TRIGGER_0, TRIGGER_VALUE, ulTopTrigVal );

	/* DVI_DTG_DTG_TRIGGER_1 (RW) */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_ITU656_DTG_0_DTG_TRIGGER_1);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(ITU656_DTG_0_DTG_TRIGGER_1, ENABLE, 0 ) |
#if (BVDC_P_SUPPORT_IT_VER >= 2)
		BCHP_FIELD_DATA(ITU656_DTG_0_DTG_TRIGGER_1, TRIGGER_SELECT, (1 != pCurInfo->ulTriggerModuloCnt) ? 1 : 0) |
		BCHP_FIELD_DATA(ITU656_DTG_0_DTG_TRIGGER_1, MODULO_COUNT, pCurInfo->ulTriggerModuloCnt) |
#endif
		BCHP_FIELD_DATA(ITU656_DTG_0_DTG_TRIGGER_1, TRIGGER_VALUE, ulBotTrigVal );

	return;
}

static void BVDC_P_Vec_Build_656_DVF_isr
	( BVDC_Display_Handle              hDisplay,
	  BVDC_P_ListInfo                 *pList )
{
	uint32_t ulNumRegs = (BCHP_ITU656_DVF_0_DVF_BVB_STATUS - BCHP_ITU656_DVF_0_DVF_CONFIG)/4 + 1;

	BSTD_UNUSED(hDisplay);

	*pList->pulCurrent++ = BRDC_OP_IMMS_TO_REGS(ulNumRegs);
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_ITU656_DVF_0_DVF_CONFIG);

	*pList->pulCurrent++ =
		BCHP_FIELD_ENUM(ITU656_DVF_0_DVF_CONFIG, VBI_PREFERRED, OFF)|
		BCHP_FIELD_ENUM(ITU656_DVF_0_DVF_CONFIG, VBI_ENABLE,    OFF)|
		BCHP_FIELD_ENUM(ITU656_DVF_0_DVF_CONFIG, UPSAMPLE2X,    ON) |
		BCHP_FIELD_DATA(ITU656_DVF_0_DVF_CONFIG, reserved0,	   0) |
		BCHP_FIELD_DATA(ITU656_DVF_0_DVF_CONFIG, reserved_for_eco1, 0);

	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(ITU656_DVF_0_DVF_VALUES, CH0_VBI_OFFSET, 1) |
		BCHP_FIELD_DATA(ITU656_DVF_0_DVF_VALUES, CH2_BLANK,	  128) |
		BCHP_FIELD_DATA(ITU656_DVF_0_DVF_VALUES, CH1_BLANK,	  128) |
		BCHP_FIELD_DATA(ITU656_DVF_0_DVF_VALUES, CH0_BLANK,	  16);

	/* ITU656_DVF_0_DVF_BVB_STATUS */
	*pList->pulCurrent++ = 0xffffffff;

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
	const BVDC_P_DisplayCscMatrix *pCscMatrix = NULL;
	const BVDC_P_DisplayInfo *pCurInfo = &hDisplay->stCurInfo;

	/* --- Setup CSC ---*/
	*pList->pulCurrent++ = BRDC_OP_IMMS_TO_REGS(BVDC_P_CSC_TABLE_SIZE);
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_ITU656_CSC_0_CSC_MODE);

	BVDC_P_Display_Get656CscTable_isr(&hDisplay->stCurInfo,
		hDisplay->hCompositor->bIsBypass, &pCscMatrix);

	/* TODO: handle user csc */
	hDisplay->st656CscMatrix = *pCscMatrix;

	/* Handle CSC mute */
	if (pCurInfo->abOutputMute[BVDC_DisplayOutput_e656])
	{
		uint8_t ucCh0, ucCh1, ucCh2;
		const BVDC_P_Compositor_Info *pCmpInfo = &hDisplay->hCompositor->stCurInfo;

		/* Blank color, use in CSC */
		ucCh0 = BPXL_GET_COMPONENT(BPXL_eA8_Y8_Cb8_Cr8, pCmpInfo->ulBgColorYCrCb, 2);
		ucCh1 = BPXL_GET_COMPONENT(BPXL_eA8_Y8_Cb8_Cr8, pCmpInfo->ulBgColorYCrCb, 1);
		ucCh2 = BPXL_GET_COMPONENT(BPXL_eA8_Y8_Cb8_Cr8, pCmpInfo->ulBgColorYCrCb, 0);

		/* Swap ch0 and 1 of input color to match vec csc layout */
		BVDC_P_Csc_ApplyYCbCrColor(&hDisplay->st656CscMatrix.stCscCoeffs, ucCh1, ucCh0, ucCh2);
	}

	BVDC_P_Vec_Build_CSC_isr(&hDisplay->st656CscMatrix, pList);

	*pList->pulCurrent++ = BRDC_OP_IMMS_TO_REGS(BVDC_P_DITHER_TABLE_SIZE);
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_ITU656_CSC_0_DITHER_CONTROL);
	BVDC_P_Vec_Build_Dither_isr(&hDisplay->st656Dither, pList);

	return;
}

static void BVDC_P_Vec_Build_656_Formatter_isr
	( BVDC_Display_Handle              hDisplay,
	  BVDC_P_ListInfo                 *pList )
{
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_ITU656_0_ITU656_CONFIG);
	if (hDisplay->bIsBypass)
	{
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(ITU656_0_ITU656_CONFIG, FILTER_MODE, 0)|
			BCHP_FIELD_ENUM(ITU656_0_ITU656_CONFIG, INPUT_MODE, BYPASS)|
			BCHP_FIELD_ENUM(ITU656_0_ITU656_CONFIG, DATA_OUT_PATTERN, ZERO)|
			BCHP_FIELD_DATA(ITU656_0_ITU656_CONFIG, reserved0, 0);
	}
	else
	{
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(ITU656_0_ITU656_CONFIG, FILTER_MODE, 0)|
			BCHP_FIELD_ENUM(ITU656_0_ITU656_CONFIG, INPUT_MODE, SHARED)|
			BCHP_FIELD_ENUM(ITU656_0_ITU656_CONFIG, DATA_OUT_PATTERN, ZERO)|
			BCHP_FIELD_DATA(ITU656_0_ITU656_CONFIG, reserved0, 0);
	}

	return;
}

static void BVDC_P_Program656Chan
	( BVDC_Display_Handle              hDisplay,
	  BVDC_P_Display656Chan           *pstChan,
	  BVDC_P_ListInfo                 *pList )
{
	BSTD_UNUSED(pstChan); /* This info will be used when there are multiple 656 cores */

	BVDC_P_Vec_Build_656_DTG_isr(hDisplay, pList);
	BVDC_P_Vec_Build_656_DVF_isr(hDisplay, pList);
	BVDC_P_Vec_Build_656_CSC_isr(hDisplay, pList);
	BVDC_P_Vec_Build_656_Formatter_isr(hDisplay, pList);
	BVDC_P_Display_Setup656TG_isr(hDisplay, pList);
}
#endif /* BVDC_P_SUPPORT_ITU656_OUT != 0 */


void BVDC_P_Display_StartAnlgCtrler
	( BVDC_Display_Handle              hDisplay,
	  BVDC_P_ListInfo                 *pList )
{
	uint32_t             ulItConfig = 0;
	BVDC_P_DisplayInfo  *pCurInfo = &hDisplay->stCurInfo;
	uint32_t             ulOffset;

	if(hDisplay->bAnlgEnable ||   /* if analog master */
	   (!hDisplay->bAnlgEnable &&  /* or anlog slave with DACs */
	   (hDisplay->stAnlgChan_0.bEnable || hDisplay->stAnlgChan_1.bEnable)))
	{
#if DCS_SUPPORT
		if (VIDEO_FORMAT_SUPPORTS_DCS        (pCurInfo->pFmtInfo->eVideoFmt))
#else
		if (VIDEO_FORMAT_SUPPORTS_MACROVISION(pCurInfo->pFmtInfo->eVideoFmt))
#endif
		{
			ulItConfig = BVDC_P_GetItConfigMv_isr(pCurInfo);
		}
		else
		{
			ulItConfig = BVDC_P_GetItConfig_isr(pCurInfo);
		}
		BDBG_ASSERT (ulItConfig);

		if ((hDisplay->eMasterTg == BVDC_DisplayTg_ePrimIt) ||
			(hDisplay->eMasterTg == BVDC_DisplayTg_eSecIt) ||
			(hDisplay->eMasterTg == BVDC_DisplayTg_eTertIt))
		{
			/* Master mode */
			ulItConfig &= ~(BCHP_FIELD_ENUM(IT_0_TG_CONFIG, SLAVE_MODE, ENABLED));
		}
		else
		{
			/* Slave mode */
			ulItConfig |= (BCHP_FIELD_ENUM(IT_0_TG_CONFIG, SLAVE_MODE, ENABLED));
		}

#if (BVDC_P_SUPPORT_IT_VER >= 3)
		if(hDisplay->stCurInfo.ulAnlgChan0Mask == 0 &&
		   hDisplay->stCurInfo.ulAnlgChan1Mask == 0)
		{
			/* HDMI slave no analog (require at least IT) */
			ulItConfig |= (BCHP_FIELD_ENUM(IT_0_TG_CONFIG, STAND_ALONE, ON));
		}
		else
		{
			ulItConfig |= (BCHP_FIELD_ENUM(IT_0_TG_CONFIG, STAND_ALONE, OFF));
		}
#endif

		ulOffset = hDisplay->stAnlgChan_0.ulIt * BVDC_P_VEC_IT_SIZE;
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_IT_0_TG_CONFIG + ulOffset);
		*pList->pulCurrent++ = ulItConfig;
	}
}

void BVDC_P_Display_StartDviCtrler
	( BVDC_Display_Handle              hDisplay,
	  BVDC_P_ListInfo                 *pList )
{
	BVDC_P_DisplayInfo  *pCurInfo = &hDisplay->stCurInfo;
	const BVDC_Display_DvoSettings *pDvoSettings =
		&hDisplay->stCurInfo.stDvoCfg;
	BFMT_VideoFmt eCurVideoFmt = pCurInfo->pFmtInfo->eVideoFmt;
	uint32_t ulIsSlave =
		(hDisplay->eMasterTg == BVDC_DisplayTg_eDviDtg) ? 0 : 1;
	uint32_t ulDviAutoRestart = ulIsSlave;

	if(hDisplay->stDviChan.bEnable || hDisplay->stCurInfo.bEnableHdmi)
	{
		/* HDMI transmitter needs specific values for DVI/DTG toggles. But
		 * custom hardware (display panels) need values provided by user. */
		uint32_t toggles[2];
		if((BFMT_VideoFmt_eCustom0 == eCurVideoFmt) ||
		   (BFMT_VideoFmt_eCustom1 == eCurVideoFmt) ||
		   (BFMT_VideoFmt_eCustom2 == eCurVideoFmt))
		{
		    toggles[0] = pDvoSettings->eHsPolarity;
		    toggles[1] = pDvoSettings->eVsPolarity;
		}
		else
		{
			const uint32_t* pToggles = BVDC_P_GetDviDtgToggles_isr (pCurInfo);
			toggles[0] = pToggles[0];
			toggles[1] = pToggles[1];
		}

		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_DVI_DTG_0_DTG_CONFIG);
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(DVI_DTG_0_DTG_CONFIG, AUTO_RESTART,
			                                               ulDviAutoRestart ) |
			BCHP_FIELD_DATA(DVI_DTG_0_DTG_CONFIG, RESTART_WIN,	         31 ) |
			BCHP_FIELD_DATA(DVI_DTG_0_DTG_CONFIG, TOGGLE_DVI_DE,
			                                      pDvoSettings->eDePolarity ) |
			BCHP_FIELD_DATA(DVI_DTG_0_DTG_CONFIG, TOGGLE_DVI_V,  toggles[1] ) |
			BCHP_FIELD_DATA(DVI_DTG_0_DTG_CONFIG, TOGGLE_DVI_H,  toggles[0] ) |
			BCHP_FIELD_DATA(DVI_DTG_0_DTG_CONFIG, TRIGGER_CNT_CLR_COND,   0 ) |
			BCHP_FIELD_DATA(DVI_DTG_0_DTG_CONFIG, SLAVE_MODE,     ulIsSlave ) |
			BCHP_FIELD_DATA(DVI_DTG_0_DTG_CONFIG, MCS_ENABLE,             1 ) ;
	}
}

#if (BVDC_P_SUPPORT_ITU656_OUT != 0)
void BVDC_P_Display_Start656Ctrler
	( BVDC_Display_Handle              hDisplay,
	  BVDC_P_ListInfo                 *pList )
{
	BSTD_UNUSED(pList);

	if (hDisplay->st656Chan.bEnable)
	{
		uint32_t ulIsSlave;
		ulIsSlave = (hDisplay->eMasterTg == BVDC_DisplayTg_e656Dtg) ? 0 : 1;

		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_ITU656_DTG_0_DTG_CONFIG);
		*pList->pulCurrent++ =
			/* master mode 656 out should turn off AUTO_RESTART */
			BCHP_FIELD_DATA(ITU656_DTG_0_DTG_CONFIG, AUTO_RESTART, ulIsSlave) |
			BCHP_FIELD_DATA(ITU656_DTG_0_DTG_CONFIG, RESTART_WIN, 31) |
			BCHP_FIELD_DATA(ITU656_DTG_0_DTG_CONFIG, MCS_ENABLE, 1	) |
			BCHP_FIELD_DATA(ITU656_DTG_0_DTG_CONFIG, SLAVE_MODE,  ulIsSlave) |
			BCHP_FIELD_DATA(ITU656_DTG_0_DTG_CONFIG, TRIGGER_CNT_CLR_COND, 0) |
			BCHP_FIELD_DATA(ITU656_DTG_0_DTG_CONFIG, TOGGLE_DVI_H, 0) |
			BCHP_FIELD_DATA(ITU656_DTG_0_DTG_CONFIG, TOGGLE_DVI_V, 0) |
			BCHP_FIELD_DATA(ITU656_DTG_0_DTG_CONFIG, TOGGLE_DVI_DE, 0);


	}
}
#endif

void BVDC_P_Display_SetFormatSwitchMarker
	( BVDC_Display_Handle              hDisplay,
	  BVDC_P_ListInfo                 *pList )
{
	/* This RUL sets a marker to indicate that we are switching
	 * format and VEC triggers are disabled. Upon detecting this marker,
	 * BVDC_P_CompositorDisplay_isr() will re-enable the triggers.
	 */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(hDisplay->ulRdcVarAddr);
	*pList->pulCurrent++ = 1;
}


void BVDC_P_Display_StartMicroCtrler
	( BVDC_Display_Handle              hDisplay,
	  BVDC_P_ListInfo                 *pList )
{
	/* MicroControllers of the master timing generator will
	 * be started the last.
	 */
	switch (hDisplay->eMasterTg)
	{
		case BVDC_DisplayTg_ePrimIt:
		case BVDC_DisplayTg_eSecIt:
		case BVDC_DisplayTg_eTertIt:
#if (BVDC_P_SUPPORT_ITU656_OUT != 0)
			BVDC_P_Display_Start656Ctrler(hDisplay, pList);
#endif
			BVDC_P_Display_StartDviCtrler(hDisplay, pList);
			BVDC_P_Display_StartAnlgCtrler(hDisplay, pList);
			break;

		case BVDC_DisplayTg_eDviDtg:
			BVDC_P_Display_StartAnlgCtrler(hDisplay, pList);
#if (BVDC_P_SUPPORT_ITU656_OUT != 0)
			BVDC_P_Display_Start656Ctrler(hDisplay, pList);
#endif
			BVDC_P_Display_StartDviCtrler(hDisplay, pList);
			break;

		case BVDC_DisplayTg_e656Dtg:
			BVDC_P_Display_StartAnlgCtrler(hDisplay, pList);
			BVDC_P_Display_StartDviCtrler(hDisplay, pList);
#if (BVDC_P_SUPPORT_ITU656_OUT != 0)
			BVDC_P_Display_Start656Ctrler(hDisplay, pList);
#endif
			break;

		default:
			break;
	}

	return;
}

static void BVDC_P_Display_SnapshotTimer
	( BVDC_Display_Handle              hDisplay,
	  BVDC_P_ListInfo                 *pList )
{
	*pList->pulCurrent++ = BRDC_OP_REG_TO_VAR(BRDC_Variable_0);
	*pList->pulCurrent++ = BRDC_REGISTER(hDisplay->stTimerReg.status);

	*pList->pulCurrent++ = BRDC_OP_IMM_TO_VAR(BRDC_Variable_1);
	*pList->pulCurrent++ = BCHP_TIMER_TIMER0_STAT_COUNTER_VAL_MASK;

	*pList->pulCurrent++ = BRDC_OP_VAR_AND_VAR_TO_VAR(BRDC_Variable_0, BRDC_Variable_1, BRDC_Variable_2);

	*pList->pulCurrent++ = BRDC_OP_VAR_TO_REG(BRDC_Variable_2);
	*pList->pulCurrent++ = BRDC_REGISTER(hDisplay->ulScratchTsAddr);

	return;
}

	/****************************************************************
	 *                                                              *
	 *                    Display Event Handlers                    *
	 *                                                              *
	 ****************************************************************/

/* Validation helper functions */
static BERR_Code BVDC_P_Display_Validate_AnalogChan
	( BVDC_Display_Handle              hDisplay )
{
	const uint32_t          *pTable;
	BAVC_VdcDisplay_Info     lRateInfo;
	BVDC_P_DisplayInfo      *pNewInfo = &hDisplay->stNewInfo;

	if((BVDC_P_GetRmTable_isr(pNewInfo, pNewInfo->pFmtInfo, &pTable, true, &lRateInfo) != BERR_SUCCESS) ||
	   (BVDC_P_GetRmTable_isr(pNewInfo, pNewInfo->pFmtInfo, &pTable, false, &lRateInfo) != BERR_SUCCESS))
	{
		BDBG_ERR(("Failed to locate Rate Manager Table"));
		return
			BERR_TRACE(BVDC_ERR_FORMAT_NOT_SUPPORT_ANALOG_OUTPUT);
	}

	/* Need this to trap out 240P@60Hz */
#if DCS_SUPPORT
	if (VIDEO_FORMAT_SUPPORTS_DCS        (pNewInfo->pFmtInfo->eVideoFmt))
#else
	if (VIDEO_FORMAT_SUPPORTS_MACROVISION(pNewInfo->pFmtInfo->eVideoFmt))
#endif
	{
		pTable = BVDC_P_GetRamTableMv_isr(pNewInfo, hDisplay->bArib480p);
	}
	else
	{
		pTable = BVDC_P_GetRamTable_isr(pNewInfo, hDisplay->bArib480p);
	}
	if (pTable == NULL)
	{
		BDBG_ERR(("Failed to locate RAM table"));
		return
			BERR_TRACE(BVDC_ERR_FORMAT_NOT_SUPPORT_ANALOG_OUTPUT);
	}

	return BERR_SUCCESS;
}

static BERR_Code BVDC_P_Display_Validate_DviChan
	( BVDC_Display_Handle              hDisplay )
{
	const uint32_t          *pTable;
	uint32_t                 ulPixelClkRate;
	BAVC_VdcDisplay_Info     lRateInfo;
	BVDC_P_DisplayInfo      *pNewInfo = &hDisplay->stNewInfo;

	if (!VIDEO_FORMAT_IS_HDMI(pNewInfo->pFmtInfo->eVideoFmt))
	{
		BDBG_ERR(("Invalid HDMI video format."));
		return BERR_TRACE(BVDC_ERR_INVALID_HDMI_MODE);
	}

	if((BVDC_P_GetRmTable_isr(pNewInfo, pNewInfo->pFmtInfo, &pTable, true, &lRateInfo) != BERR_SUCCESS) ||
	   (BVDC_P_GetRmTable_isr(pNewInfo, pNewInfo->pFmtInfo, &pTable, false, &lRateInfo) != BERR_SUCCESS))
	{
		BDBG_ERR(("Failed to get Rate Manager table"));
		return BERR_TRACE(BVDC_ERR_INVALID_HDMI_MODE);
	}

	/* Analog VEC operates at double rate for 480P and 576P. So we have to
	 * convert back to single rate frequency to access the HDMI rate table. */
	ulPixelClkRate = lRateInfo.ulPixelClkRate;
	if (VIDEO_FORMAT_IS_ED(pNewInfo->pFmtInfo->eVideoFmt))
	{
		if (ulPixelClkRate & BFMT_PXL_54MHz)
		{
			ulPixelClkRate &= ~BFMT_PXL_54MHz;
			ulPixelClkRate |=  BFMT_PXL_27MHz;
		}
		if (ulPixelClkRate & BFMT_PXL_54MHz_MUL_1_001)
		{
			ulPixelClkRate &= ~BFMT_PXL_54MHz_MUL_1_001;
			ulPixelClkRate |=  BFMT_PXL_27MHz_MUL_1_001;
		}
	}

	if (BVDC_P_HdmiRmTable_isr(
		pNewInfo->pFmtInfo->eVideoFmt,
		ulPixelClkRate,
		pNewInfo->eHdmiColorDepth,
		pNewInfo->eHdmiPixelRepetition) == NULL)
	{
		BDBG_ERR(("Failed to allocate HDMI Rate Manager settings for video format %d, pixel clock rate %d, HDMI color depth %d, pixel repetition %d",
			pNewInfo->pFmtInfo->eVideoFmt, ulPixelClkRate, pNewInfo->eHdmiColorDepth, pNewInfo->eHdmiPixelRepetition));
		return BERR_TRACE(BVDC_ERR_INVALID_HDMI_MODE);
	}

	return BERR_SUCCESS;
}

#if (BVDC_P_SUPPORT_ITU656_OUT != 0)
static BERR_Code BVDC_P_Display_Validate_656Chan
	( BVDC_Display_Handle              hDisplay )
{
	BVDC_P_DisplayInfo      *pNewInfo = &hDisplay->stNewInfo;
	const uint32_t          *pTable;
	BAVC_VdcDisplay_Info     lRateInfo;

	if (!VIDEO_FORMAT_IS_525_LINES(pNewInfo->pFmtInfo->eVideoFmt)
		&&
		!VIDEO_FORMAT_IS_625_LINES(pNewInfo->pFmtInfo->eVideoFmt))
	{
		BDBG_ERR(("Invalid video format %d for 656", pNewInfo->pFmtInfo->eVideoFmt));
		return BERR_TRACE(BVDC_ERR_INVALID_656OUT_USE);
	}

	if((BVDC_P_GetRmTable_isr(pNewInfo, pNewInfo->pFmtInfo, &pTable, true, &lRateInfo) != BERR_SUCCESS) ||
	   (BVDC_P_GetRmTable_isr(pNewInfo, pNewInfo->pFmtInfo, &pTable, false, &lRateInfo) != BERR_SUCCESS))
	{
		BDBG_ERR(("Failed to get Rate Manager table"));
		return BERR_TRACE(BVDC_ERR_INVALID_656OUT_MODE);
	}

	return BERR_SUCCESS;
}
#endif

/**************** New analog channel event hanlders **************/
static void BVDC_P_Display_Apply_Chan
	( BVDC_Display_Handle              hDisplay,
	  BVDC_P_DisplayAnlgChan          *pstChan,
	  BVDC_P_ListInfo                 *pList )
{
	bool bHwBugWorkAround = false;

#if BVDC_P_VEC_STANDALONE_BUG_FIXED
	bHwBugWorkAround = false;
#else
	bHwBugWorkAround = hDisplay->bAnlgEnable &&
		(((pstChan->ulId == 0) &&
		((hDisplay->stAnlgChan_1.eState == BVDC_P_DisplayResource_eDestroy) ||
		 (hDisplay->stAnlgChan_1.eState == BVDC_P_DisplayResource_eShuttingdown) ||
		 (hDisplay->stAnlgChan_1.eState == BVDC_P_DisplayResource_eInactive))) ||
		 ((pstChan->ulId == 1) &&
		  (hDisplay->stAnlgChan_0.eState == BVDC_P_DisplayResource_eInactive)));
#endif

	switch (pstChan->eState)
	{
		case BVDC_P_DisplayResource_eCreate:
			BDBG_MSG(("Display %d set up AnlgChan_%d", hDisplay->eId, pstChan->ulId));
			if(pstChan->ulId == 0)
			{
				BVDC_P_SetupIT(hDisplay, pstChan, pList);
			}
			BVDC_P_SetupAnalogChan(hDisplay->hVdc->hResource, pstChan, pList);
			pstChan->eState = BVDC_P_DisplayResource_eActivating;
			break;

		case BVDC_P_DisplayResource_eActivating:
			if (pList->bLastExecuted)
			{
				BDBG_MSG(("Display %d AnlgChan_%d is active", hDisplay->eId, pstChan->ulId));
				pstChan->eState = BVDC_P_DisplayResource_eActive;
				if(pstChan->ulId == 0)
				{
					hDisplay->stCurInfo.stDirty.stBits.bChan0 = BVDC_P_CLEAN;
				}
				else
				{
					hDisplay->stCurInfo.stDirty.stBits.bChan1 = BVDC_P_CLEAN;
				}
			}
			else
			{
				BDBG_MSG(("Display %d set up AnlgChan_%d again because last RUL didn't execute",
					hDisplay->eId, pstChan->ulId));
				if(pstChan->ulId == 0)
				{
					BVDC_P_SetupIT(hDisplay, pstChan, pList);
				}
				BVDC_P_SetupAnalogChan(hDisplay->hVdc->hResource, pstChan, pList);
				}
			break;

		case BVDC_P_DisplayResource_eDestroy:
			if(bHwBugWorkAround)
			{
				BDBG_MSG(("No Dacs - Don't tear down Chan%d", pstChan->ulId));
			}
			else
			{
				BDBG_MSG(("Display %d tear down AnlgChan_%d", hDisplay->eId, pstChan->ulId));
				BVDC_P_TearDownAnalogChan(pstChan, pList);
				pstChan->bEnable = false;
				pstChan->bTearDown = true;
			}
			pstChan->eState = BVDC_P_DisplayResource_eShuttingdown;
			break;

		case BVDC_P_DisplayResource_eShuttingdown:
			/* We can reach here for two reasons:
			 * 1) The channel one tear down RUL has been exectued and we need to free the resources;
			 * 2) The channel one tear down RUL somehow didn't get executed. So we have to buile
			 *    and run the RUL again.
			 */
			if (pList->bLastExecuted)
			{
				if(!pstChan->bTearDown)
				{
					BDBG_MSG(("No Dacs - Keep Chan%d Resources", pstChan->ulId));
					pstChan->eState = BVDC_P_DisplayResource_eResInactive;
				}
				else
				{
					BDBG_MSG(("Display %d free AnlgChan_%d resources", hDisplay->eId, pstChan->ulId));
					/* if not analog master, free IT resource as well */
					if(!hDisplay->bAnlgEnable)
						BVDC_P_FreeITResources(hDisplay->hVdc->hResource, pstChan);
					BVDC_P_FreeAnalogChanResources(hDisplay->hVdc->hResource, pstChan);
					pstChan->eState = BVDC_P_DisplayResource_eInactive;
				}
				pstChan->bTearDown = false;
				BVDC_P_FreeDacResources(hDisplay->hVdc->hResource, pstChan);
				if(pstChan->ulId == 0)
				{
					hDisplay->stCurInfo.stDirty.stBits.bChan0 = BVDC_P_CLEAN;
				}
				else
				{
					hDisplay->stCurInfo.stDirty.stBits.bChan1 = BVDC_P_CLEAN;
				}
			}
			else
			{
				if(pstChan->bTearDown)
				{
					BDBG_MSG(("Display %d tear down AnlgChan_%d because last RUL didn't execute",
						hDisplay->eId, pstChan->ulId));
					BVDC_P_TearDownAnalogChan(pstChan, pList);
				}
			}
			break;

		default:
			/* Do nothing */
			if(pstChan->ulId == 0)
			{
				hDisplay->stCurInfo.stDirty.stBits.bChan0 = BVDC_P_CLEAN;
			}
			else
			{
				hDisplay->stCurInfo.stDirty.stBits.bChan1 = BVDC_P_CLEAN;
			}
			break;
	}

	return;
}

/**************** New analog channel 0 event hanlders **************/
static void BVDC_P_Display_Apply_Chan0
	( BVDC_Display_Handle              hDisplay,
	  BVDC_P_ListInfo                 *pList,
	  BAVC_Polarity                    eFieldPolarity )
{
	BSTD_UNUSED(eFieldPolarity);
	BVDC_P_Display_Apply_Chan(hDisplay, &hDisplay->stAnlgChan_0, pList);
	return;
}

/**************** New analog channel 1 event hanlders **************/
static void BVDC_P_Display_Apply_Chan1
	( BVDC_Display_Handle              hDisplay,
	  BVDC_P_ListInfo                 *pList,
	  BAVC_Polarity                    eFieldPolarity )
{
	BSTD_UNUSED(eFieldPolarity);
	BVDC_P_Display_Apply_Chan(hDisplay, &hDisplay->stAnlgChan_1, pList);
	return;
}

/**************** New video format (output timing) event hanlders **************/
static BERR_Code BVDC_P_Display_Validate_VideoFormat
	( BVDC_Display_Handle              hDisplay )
{
	BERR_Code err;
	BVDC_P_DisplayInfo *pNewInfo;
	BVDC_P_DisplayInfo *pCurInfo;

	/* Currently video format validation is actually divided into two parts.
	 *
	 * 1) New video format vs DAC assignment.
	 *
	 * This is achieved by BVDC_P_Display_ValidateDacSettings() in bvdc_display_priv.c.
	 * The reason is that DACs are considered as resources shared among all the
	 * displays. Any DAC assignment change to one display could possibly affect other
	 * displays. BVDC_P_Display_ValidateChanges() is a place we have access to all
	 * display handles.
	 *
	 *
	 * 2) New video format for a particular display.
	 *
	 * Here we validate items like if we have valid settings such as RM table for this format,
	 * outputs (analog/HDMI/ITU-656) allows this format, etc.
	 */

	pNewInfo = &hDisplay->stNewInfo;
	pCurInfo = &hDisplay->stCurInfo;

	if(hDisplay->bAnlgEnable ||   /* if analog master */
	   (!hDisplay->bAnlgEnable &&  /* or anlog slave with DACs */
	   (hDisplay->stAnlgChan_0.bEnable || hDisplay->stAnlgChan_1.bEnable)))
	{
		if ((err = BVDC_P_Display_Validate_AnalogChan(hDisplay)) !=
			BERR_SUCCESS)
		{
			return BERR_TRACE(err);
		}

		if ((err = BVDC_P_ValidateMacrovision (hDisplay)) != BERR_SUCCESS)
		{
			return BERR_TRACE(err);
		}

		/* validate secam */
		if(VIDEO_FORMAT_IS_SECAM(pCurInfo->pFmtInfo->eVideoFmt) !=
		   VIDEO_FORMAT_IS_SECAM(pNewInfo->pFmtInfo->eVideoFmt))
		{
			if(hDisplay->stAnlgChan_0.bEnable)
			{
				BDBG_MSG(("Realocate Analog resource for chan 0"));
				err = BVDC_P_AllocAnalogChanResources(hDisplay->hVdc->hResource, hDisplay->eId * 2, &hDisplay->stAnlgChan_0,
							(!(hDisplay->stNewInfo.ulAnlgChan0Mask & BVDC_P_Dac_Mask_SD) && hDisplay->bHdCap) ? true : false ,
							(hDisplay->stNewInfo.ulAnlgChan0Mask & BVDC_P_Dac_Mask_HD) ? true : false ,
							((hDisplay->stNewInfo.ulAnlgChan0Mask & BVDC_P_Dac_Mask_SD) && (BVDC_P_NUM_SHARED_SECAM != 0) && (VIDEO_FORMAT_IS_SECAM(pNewInfo->pFmtInfo->eVideoFmt))) ? true : false);
			}
			if(hDisplay->stAnlgChan_1.bEnable)
			{
				BDBG_MSG(("Realocate Analog resource for chan 1"));
				err |= BVDC_P_AllocAnalogChanResources(hDisplay->hVdc->hResource, hDisplay->eId * 2 + 1, &hDisplay->stAnlgChan_1,
							(!(hDisplay->stNewInfo.ulAnlgChan1Mask & BVDC_P_Dac_Mask_SD) && hDisplay->bHdCap) ? true : false ,
							(hDisplay->stNewInfo.ulAnlgChan1Mask & BVDC_P_Dac_Mask_HD) ? true : false ,
							((hDisplay->stNewInfo.ulAnlgChan1Mask & BVDC_P_Dac_Mask_SD) && (BVDC_P_NUM_SHARED_SECAM != 0) && (VIDEO_FORMAT_IS_SECAM(pNewInfo->pFmtInfo->eVideoFmt))) ? true : false);
			}
			if(err != BERR_SUCCESS)
				return BERR_TRACE(err);
		}
	}

	if(hDisplay->stDviChan.bEnable)
	{
		if ((err = BVDC_P_Display_Validate_DviChan(hDisplay)) != BERR_SUCCESS)
			return BERR_TRACE(err);
	}

#if (BVDC_P_SUPPORT_ITU656_OUT != 0)
	if (hDisplay->st656Chan.bEnable)
	{
		if ((err = BVDC_P_Display_Validate_656Chan(hDisplay)) != BERR_SUCCESS)
			return BERR_TRACE(err);
	}
#endif

	return BERR_SUCCESS;
}

static void BVDC_P_Display_Copy_VideoFormatSoft
	( BVDC_Display_Handle              hDisplay )
{
	BVDC_P_DisplayInfo *pCurInfo, *pNewInfo;

	pCurInfo = &hDisplay->stCurInfo;
	pNewInfo = &hDisplay->stNewInfo;

#ifdef BVDC_P_SUPPORT_STG
	{
		/* 6 is the maximum difference between frequency-tracking support freq: 6000/5994, 2400/2397 */
		uint32_t ulNewVertFreq = BVDC_P_ROUND_OFF(pNewInfo->pFmtInfo->ulVertFreq, 6, BFMT_FREQ_FACTOR);
		uint32_t ulCurVertFreq = BVDC_P_ROUND_OFF(pCurInfo->pFmtInfo->ulVertFreq, 6, BFMT_FREQ_FACTOR);

		if((BVDC_P_ItState_eActive == hDisplay->eItState) &&
			BVDC_P_DISPLAY_USED_STG(hDisplay->eMasterTg) &&
			(!pCurInfo->bStgNonRealTime) &&
			((pCurInfo->pFmtInfo->bInterlaced!= pNewInfo->pFmtInfo->bInterlaced) ||
			 (ulNewVertFreq != ulCurVertFreq)))
		{
			hDisplay->eItState = BVDC_P_ItState_eSwitchMode;
			BDBG_MSG(("dsp[%d] STG [%d] change itstate to switch", hDisplay->eId, hDisplay->stStgChan.ulStg));
		}
	}

#endif

	BDBG_MSG(("dsp[%d] Format change %s -> %s", hDisplay->eId,
		pCurInfo->pFmtInfo->pchFormatStr, pNewInfo->pFmtInfo->pchFormatStr));
	pCurInfo->pFmtInfo     = pNewInfo->pFmtInfo ;
	if(BVDC_P_IS_CUSTOMFMT(pNewInfo->pFmtInfo->eVideoFmt))
	{
		BVDC_P_SET_CUSTOMFMT(pCurInfo->pFmtInfo,
			&pCurInfo->stCustomFormatInfo, hDisplay->stNewInfo.pFmtInfo);
	}
#ifdef BVDC_P_SUPPORT_STG
	if(!BVDC_P_DISPLAY_USED_STG(hDisplay->eMasterTg))
#endif
	pCurInfo->ulVertFreq   = pNewInfo->ulVertFreq;
	pCurInfo->ulHeight     = pNewInfo->ulHeight;
	pCurInfo->eOrientation = pNewInfo->eOrientation;

	pCurInfo->eAnlg_0_OutputColorSpace = pNewInfo->eAnlg_0_OutputColorSpace;
	pCurInfo->eAnlg_1_OutputColorSpace = pNewInfo->eAnlg_1_OutputColorSpace;
	pCurInfo->eCmpColorSpace = pNewInfo->eCmpColorSpace;
	pCurInfo->bMultiRateAllow = pNewInfo->bMultiRateAllow;

	/* Copy protection state */
	pCurInfo->eMacrovisionType = pNewInfo->eMacrovisionType;
	pCurInfo->stN0Bits = pNewInfo->stN0Bits;
#if DCS_SUPPORT
	pCurInfo->eDcsMode = pNewInfo->eDcsMode;
#endif

#if (BVDC_P_SUPPORT_IT_VER >= 2)
	pCurInfo->ulTriggerModuloCnt = pNewInfo->ulTriggerModuloCnt;
#endif

	return;
}

static void BVDC_P_Display_Copy_VideoFormat
	( BVDC_Display_Handle              hDisplay )
{
	BVDC_P_Display_Copy_VideoFormatSoft (hDisplay);

	/* We set to Switch Mode here instead of in ISR.
	 * This is just to accommdate the legacy logic in other sub-lib.
	 */
	if ((hDisplay->eItState != BVDC_P_ItState_eNotActive) &&
	    (!BVDC_P_DISPLAY_USED_STG(hDisplay->eMasterTg)))
		hDisplay->eItState = BVDC_P_ItState_eSwitchMode;

	return;
}

static void BVDC_P_Display_Apply_VideoFormat
	( BVDC_Display_Handle              hDisplay,
	  BVDC_P_ListInfo                 *pList,
	  BAVC_Polarity                    eFieldPolarity )
{
	uint32_t    ulSrcId;

	BSTD_UNUSED(eFieldPolarity);

	/* The copy handler BVDC_P_Display_Copy_VideoFormat() sets hDisplay->eItState
	 * to BVDC_P_ItState_eSwitchMode. If BVDC_P_Display_Apply_VideoFormat() is
	 * invoked by normal apply change path, BVDC_P_Display_Copy_VideoFormat() will
	 * get executed hence the state gets set. If BVDC_P_Display_Apply_VideoFormat()
	 * gets invoked because the previous RUL didn't get executed or partially executed
	 * and eItState was changed to BVDC_P_ItState_eActive, we have to force
	 * eItState to BVDC_P_ItState_eSwitchMode so that BVDC_P_CompositorDisplay_isr()
	 * knows it needs to enable VEC trigger again. */
	if ((hDisplay->eItState  != BVDC_P_ItState_eNotActive ) &&
	    (hDisplay->eItState  != BVDC_P_ItState_eSwitchMode) &&
	    (!BVDC_P_DISPLAY_USED_STG(hDisplay->eMasterTg)))
		hDisplay->eItState = BVDC_P_ItState_eSwitchMode;
	ulSrcId = (uint32_t) hDisplay->hCompositor->eId;

	if(hDisplay->bAnlgEnable ||   /* if analog master */
	   (!hDisplay->bAnlgEnable &&  /* or anlog slave with DACs */
	   (hDisplay->stAnlgChan_0.bEnable || hDisplay->stAnlgChan_1.bEnable)))
	{
		/*
		 * 1) Tear down IT
		 */
		BDBG_MSG(("VideoFmt: Tear down display[%d] IT", hDisplay->eId));
		BVDC_P_TearDownIT(&hDisplay->stAnlgChan_0, pList);

		/*
		 * 2) Set up the new path
		 */
		BDBG_MSG(("VideoFmt: Setup display[%d] IT", hDisplay->eId));
		BVDC_P_SetupIT(hDisplay, &hDisplay->stAnlgChan_0, pList);

		/*
		 * 3) Program the modules to the new format
		 *    Note: Format changing involves reprogramming all the settings
		 *          in the current info. We will call each event handler
		 *          and clears all the dirty bits.
		 */
		BVDC_P_ProgramAnalogChan(
			hDisplay, &hDisplay->stAnlgChan_0, true, pList);
	}

	if (hDisplay->stAnlgChan_0.bEnable)
	{
		/*
		 * 1) Tear down previous path
		 */
		BDBG_MSG(("VideoFmt: Tear down display[%d] analog channel 0", hDisplay->eId));
		BVDC_P_TearDownAnalogChan(&hDisplay->stAnlgChan_0, pList);

		/*
		 * 2) Set up the new path
		 */
		BDBG_MSG(("VideoFmt: Setup display[%d] analog channel 0", hDisplay->eId));
		BVDC_P_SetupAnalogChan(hDisplay->hVdc->hResource, &hDisplay->stAnlgChan_0, pList);
	}

	/* Secam_0 shares reset with Secam_3 (pass_thru Secam). So we
	 * don't use Secam_0.
	 */
	if (hDisplay->stAnlgChan_1.bEnable)
	{
		/*
		 * 1) Tear down previous path and free up acquired resources
		 */
		BDBG_MSG(("VideoFmt: Tear down display[%d] analog channel 1 ", hDisplay->eId));
		BVDC_P_TearDownAnalogChan(&hDisplay->stAnlgChan_1, pList);

		/*
		 * 2) Set up the new path
		 */
		BDBG_MSG(("VideoFmt: Setup display[%d] analog channel 1", hDisplay->eId));
		BVDC_P_SetupAnalogChan(hDisplay->hVdc->hResource, &hDisplay->stAnlgChan_1, pList);
	}

	if(hDisplay->stDviChan.bEnable || hDisplay->stCurInfo.bEnableHdmi)
	{
		/*
		 * 1) Tear down previous path and free up acquired resources
		 */
		BVDC_P_TearDownDviChan(hDisplay, &hDisplay->stDviChan, pList);

		/*
		 * 2) Acquire necessary resources and set up the new path
		 */
		BDBG_MSG(("BVDC_P_Display_Apply_VideoFormat Display %d allocates resource for DviChan", hDisplay->eId));
		BVDC_P_SetupDviChan(hDisplay, &hDisplay->stDviChan, pList);
		BVDC_P_ConnectDviSrc(hDisplay, &hDisplay->stDviChan, pList);

		/*
		 * 3) Program the modules to the new format
		 */
		BVDC_P_ProgramDviChan(hDisplay, &hDisplay->stDviChan, true, pList);
	}

#if (BVDC_P_SUPPORT_ITU656_OUT != 0)
	if (hDisplay->st656Chan.bEnable)
	{
		/*
		 * 1) Tear down previous path and free up acquired resources
		 */
		BVDC_P_TearDown656Chan(hDisplay, &hDisplay->st656Chan, pList);

		/*
		 * 2) Acquire necessary resources and set up the new path
		 */
		BDBG_MSG(("BVDC_P_Display_Apply_VideoFormat Display %d allocates resource for 656Chan", hDisplay->eId));
		BVDC_P_Setup656Chan(hDisplay, &hDisplay->st656Chan, pList);
		BVDC_P_Connect656Src(hDisplay, &hDisplay->st656Chan, pList);

		/*
		 * 3) Program the modules to the new format
		 */
		BVDC_P_Program656Chan(hDisplay, &hDisplay->st656Chan, pList);
	}
#endif

#if (BVDC_P_SUPPORT_STG)
	if((hDisplay->stStgChan.bEnable) && (0 == hDisplay->hCompositor->ulDelayCnt))
	{
		/*
		 * 1) Tear down previous path and free up acquired resources;
		 *    Note: STG has no extra resource to free, and the only crossbar could be immediately switched without teardown;
		 *          Removing the teardown is no harm and more importantly, it avoids masking next timer trigger!
		 */

		/*
		 * 2) Acquire necessary resources and set up the new path
		 */
		BDBG_MSG(("BVDC_P_Display_Apply_VideoFormat Display %d allocates resource for STG chan", hDisplay->eId));
		BVDC_P_ConnectStgSrc(hDisplay, pList);

		/*
		 * 3) Program the modules to the new format
		 */
		BVDC_P_ProgramStgChan(hDisplay, pList);
		hDisplay->eStgState = BVDC_P_DisplayResource_eActive;
		hDisplay->stCurInfo.stDirty.stBits.bStgEnable = BVDC_P_CLEAN;
	}
	if(!BVDC_P_DISPLAY_USED_STG(hDisplay->eMasterTg) ||
		(BVDC_P_DISPLAY_USED_STG(hDisplay->eMasterTg) && (BVDC_P_ItState_eSwitchMode == hDisplay->eItState)))
#endif

	BVDC_P_Display_SetFormatSwitchMarker(hDisplay, pList);

#if (BVDC_P_SUPPORT_STG)
	if((hDisplay->stStgChan.bEnable) &&
		(BVDC_P_DISPLAY_USED_STG(hDisplay->eMasterTg)))
	{
		hDisplay->stCurInfo.stDirty.stBits.bTiming = (hDisplay->hCompositor->ulDelayCnt!= 0);
		if(!hDisplay->hCompositor->ulDelayCnt)
		{
			hDisplay->pPrevFmtInfo = hDisplay->stCurInfo.pFmtInfo;
			if(BVDC_P_IS_CUSTOMFMT(hDisplay->stCurInfo.pFmtInfo->eVideoFmt))
			{
				BVDC_P_SET_CUSTOMFMT(hDisplay->pPrevFmtInfo,
					&hDisplay->stPrevCustomFormatInfo, hDisplay->stCurInfo.pFmtInfo);
			}
		}
	}
	else
	{
		hDisplay->stCurInfo.stDirty.stBits.bTiming = BVDC_P_CLEAN;
	}
#else
	hDisplay->stCurInfo.stDirty.stBits.bTiming = BVDC_P_CLEAN;
#endif

#if DCS_SUPPORT
	BVDC_P_DCS_StateFault_isr (hDisplay);
#endif

	return;
}

/************ New video format (output timing) SOFT event hanlders **********/
static BERR_Code BVDC_P_Display_Validate_VideoFormatSoft
	( BVDC_Display_Handle              hDisplay )
{
	BERR_Code err;
	BVDC_P_DisplayInfo *pNewInfo;

	pNewInfo = &hDisplay->stNewInfo;

	if(hDisplay->bAnlgEnable ||   /* if analog master */
	   (!hDisplay->bAnlgEnable &&  /* or anlog slave with DACs */
	   (hDisplay->stAnlgChan_0.bEnable || hDisplay->stAnlgChan_1.bEnable)))
	{
		if ((err = BVDC_P_ValidateMacrovision (hDisplay)) != BERR_SUCCESS)
		{
			return BERR_TRACE (err);
		}
	}

	return BERR_SUCCESS;
}

static void BVDC_P_Display_Apply_VideoFormatSoft
	( BVDC_Display_Handle              hDisplay,
	  BVDC_P_ListInfo                 *pList,
	  BAVC_Polarity                    eFieldPolarity )
{
#if DCS_SUPPORT /** { **/

	BSTD_UNUSED(eFieldPolarity);
	BSTD_UNUSED(pList);

#else /** } !DCS_SUPPORT { **/

	uint32_t    ulSrcId;

	BSTD_UNUSED(eFieldPolarity);

	ulSrcId = (uint32_t) hDisplay->hCompositor->eId;

	/* Program IT_?_TG_CONFIG register */
	BVDC_P_Display_StartAnlgCtrler (hDisplay, pList);

	if(hDisplay->bAnlgEnable ||   /* if analog master */
	   (!hDisplay->bAnlgEnable &&  /* or anlog slave with DACs */
	   (hDisplay->stAnlgChan_0.bEnable || hDisplay->stAnlgChan_1.bEnable)))
	{
		/*
		 * 3) Program the modules to the new format
		 *    Note: Format changing involves reprogramming all the settings
		 *          in the current info. We will call each event handler
		 *          and clears all the dirty bits.
		 */
		BVDC_P_ProgramAnalogChan(
			hDisplay, &hDisplay->stAnlgChan_0, false, pList);
	}

	if(hDisplay->stDviChan.bEnable || hDisplay->stCurInfo.bEnableHdmi)
	{
		/*
		 * 3) Program the modules to the new format
		 */
		BVDC_P_ProgramDviChan(hDisplay, &hDisplay->stDviChan, false, pList);
	}


#if (BVDC_P_SUPPORT_ITU656_OUT != 0)
	if (hDisplay->st656Chan.bEnable)
	{
		/*
		 * 3) Program the modules to the new format
		 */
		BVDC_P_Program656Chan(hDisplay, &hDisplay->st656Chan, pList);
	}
#endif

#if (BVDC_P_SUPPORT_STG)
	if(hDisplay->stStgChan.bEnable)
	{

		/*
		 * 3) Program the modules to the new format
		 */
		BVDC_P_ProgramStgChan(hDisplay, pList);

	}
#endif

#endif /** } !DCS_SUPPORT **/

	hDisplay->stCurInfo.stDirty.stBits.bSoftTiming = BVDC_P_CLEAN;

	return;
}

/**************** Setting time base event hanlders **************/
static void BVDC_P_Display_Copy_TimeBase_Setting
	( BVDC_Display_Handle              hDisplay )
{
	hDisplay->stCurInfo.eTimeBase = hDisplay->stNewInfo.eTimeBase;

	return;
}

static void BVDC_P_Display_Apply_TimeBase_Setting
	( BVDC_Display_Handle              hDisplay,
	  BVDC_P_ListInfo                 *pList,
	  BAVC_Polarity                    eFieldPolarity )
{
	uint32_t          ulOffset;
	BAVC_Timebase     eTimeBase = hDisplay->stCurInfo.eTimeBase;
	BSTD_UNUSED(eFieldPolarity);

	if(hDisplay->bAnlgEnable ||   /* if analog master */
	   (!hDisplay->bAnlgEnable &&  /* or anlog slave with DACs */
	   (hDisplay->stAnlgChan_0.bEnable || hDisplay->stAnlgChan_1.bEnable)))
	{
		/* RM pairs with IT */
		ulOffset = hDisplay->stAnlgChan_0.ulIt * BVDC_P_VEC_RM_SIZE;

		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_RM_0_CONTROL + ulOffset);
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(RM_0_CONTROL, RESET, 0) |
			BCHP_FIELD_DATA(RM_0_CONTROL, INT_GAIN, 4) |
			BCHP_FIELD_DATA(RM_0_CONTROL, DIRECT_GAIN, 1) |
			BCHP_FIELD_DATA(RM_0_CONTROL, DITHER, 0) |
			BCHP_FIELD_DATA(RM_0_CONTROL, FREE_RUN, 0) |
			BCHP_FIELD_DATA(RM_0_CONTROL, TIMEBASE, eTimeBase);
	}

	if(hDisplay->stDviChan.bEnable || hDisplay->stCurInfo.bEnableHdmi)
	{
		BVDC_P_Vec_Build_Hdmi_isr(hDisplay, pList);
	}

	hDisplay->bRateManagerUpdated = true;

	hDisplay->stCurInfo.stDirty.stBits.bTimeBase = BVDC_P_CLEAN;

	return;
}


/***************** Setting DAC event hanlders *********************/
static void BVDC_P_Display_Copy_DAC_Setting
	( BVDC_Display_Handle              hDisplay )
{
	BVDC_P_DisplayInfo *pCurInfo, *pNewInfo;
	int i;

	pCurInfo = &hDisplay->stCurInfo;
	pNewInfo = &hDisplay->stNewInfo;

	for(i = 0; i < BVDC_P_MAX_DACS; i++)
	{
		pCurInfo->aDacOutput[i] = pNewInfo->aDacOutput[i];
	}

	pCurInfo->bCvbs   = pNewInfo->bCvbs;
	pCurInfo->bSvideo = pNewInfo->bSvideo;
	pCurInfo->bHsync  = pNewInfo->bHsync;
	pCurInfo->bRgb    = pNewInfo->bRgb;
	pCurInfo->bYPrPb  = pNewInfo->bYPrPb;
	pCurInfo->ulAnlgChan0Mask = pNewInfo->ulAnlgChan0Mask;
	pCurInfo->ulAnlgChan1Mask = pNewInfo->ulAnlgChan1Mask;
	pCurInfo->eAnlg_0_OutputColorSpace = pNewInfo->eAnlg_0_OutputColorSpace;
	pCurInfo->eAnlg_1_OutputColorSpace = pNewInfo->eAnlg_1_OutputColorSpace;
	pCurInfo->bMultiRateAllow = pNewInfo->bMultiRateAllow;

	return;
}

static void BVDC_P_Display_ProgramDac_isr
	( BVDC_DacOutput                   eDacOutput,
	  uint32_t                         ulDac,
	  uint32_t                         ulDacSrc,
	  BVDC_P_ListInfo                 *pList )
{
#if (BVDC_P_MAX_DACS > 1)
	uint32_t ulOffset = (BCHP_MISC_DAC_1_CFG - BCHP_MISC_DAC_0_CFG) * ulDac;
#else
	uint32_t ulOffset = 0;
#endif
	uint32_t ulDacSel = 0;

	switch(eDacOutput)
	{
		case BVDC_DacOutput_eY:
		case BVDC_DacOutput_eGreen:
		case BVDC_DacOutput_eGreen_NoSync:
		case BVDC_DacOutput_eSVideo_Luma:
			ulDacSel = 0 + ulDacSrc;
			break;
		case BVDC_DacOutput_eComposite:
		case BVDC_DacOutput_ePb:
		case BVDC_DacOutput_eBlue:
			ulDacSel = 1 + ulDacSrc;
			break;
		case BVDC_DacOutput_ePr:
		case BVDC_DacOutput_eRed:
		case BVDC_DacOutput_eSVideo_Chroma:
			ulDacSel = 2 + ulDacSrc;
			break;
		case BVDC_DacOutput_eUnused:
			ulDacSel = BCHP_MISC_DAC_0_CFG_SEL_CONST;
			break;
#ifdef BCHP_MISC_DAC_0_CFG_SEL_GRPD_0_CVBS
		case BVDC_DacOutput_eFilteredCvbs:
			ulDacSel = BCHP_MISC_DAC_0_CFG_SEL_GRPD_0_CVBS;
			break;
#endif
#ifdef BCHP_MISC_DAC_0_CFG_SEL_HDSRC_0_HSYNC
		case BVDC_DacOutput_eHsync:
			ulDacSel = BCHP_MISC_DAC_0_CFG_SEL_HDSRC_0_HSYNC;
			break;
#endif
		default:
			BDBG_WRN(("Disp[%d], eDacOutput=%d not supported"));
			BDBG_ASSERT(0);
			break;
	}

	BDBG_MSG(("\t DAC %d Src %d Chan %d, output %d, ulOffset = %d, power %s",
		ulDac, ulDacSrc, ulDacSel - ulDacSrc, eDacOutput, ulOffset,
		(eDacOutput == BVDC_DacOutput_eUnused)? "off" : "on"));

	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_MISC_DAC_0_CFG + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(MISC_DAC_0_CFG, CONST,        0) |
		BCHP_FIELD_ENUM(MISC_DAC_0_CFG, SINC,        ON) |
		BCHP_FIELD_DATA(MISC_DAC_0_CFG, DELAY,        0) |
		BCHP_FIELD_DATA(MISC_DAC_0_CFG, SEL,   ulDacSel);

#if (BVDC_P_SUPPORT_TDAC_VER >= BVDC_P_SUPPORT_TDAC_VER_11)
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_MISC_DAC_0_CTRL + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(MISC_DAC_0_CTRL, FORMAT,       0 ) | /* optimmal */
		BCHP_FIELD_DATA(MISC_DAC_0_CTRL, RSTB,         1 ) | /* optimmal */
		BCHP_FIELD_DATA(MISC_DAC_0_CTRL, PWRDN,
			(eDacOutput == BVDC_DacOutput_eUnused) ? 1 : 0);
#elif (BVDC_P_SUPPORT_TDAC_VER < BVDC_P_SUPPORT_TDAC_VER_9)
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_MISC_DAC_0_CTRL + ulOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(MISC_DAC_0_CTRL, PWRUP_BAIS,
			(eDacOutput == BVDC_DacOutput_eUnused) ? 0 : 1) |
		BCHP_FIELD_DATA(MISC_DAC_0_CTRL, PWRDN,
			(eDacOutput == BVDC_DacOutput_eUnused) ? 1 : 0);
#endif

	return;
}

static uint32_t BVDC_P_Dislay_FindDacSrc
	( BVDC_Display_Handle              hDisplay,
	  uint32_t                         ulDacMask )
{
	BVDC_P_DisplayAnlgChan *pChan = NULL;
	uint32_t ulDacSrc  = UINT32_C(-1);

	if((hDisplay->stCurInfo.ulAnlgChan0Mask & ulDacMask) == ulDacMask)
	{
		pChan = &hDisplay->stAnlgChan_0;
	}
	else if((hDisplay->stCurInfo.ulAnlgChan1Mask & ulDacMask) == ulDacMask)
	{
		pChan = &hDisplay->stAnlgChan_1;
	}
	else
	{
		return ulDacSrc;
	}

	if(!pChan->bEnable)
	{
		ulDacSrc = UINT32_C(-1);
	}
#if (BVDC_P_NUM_SHARED_HDSRC > 0)
	else if(pChan->ulHdsrc != BVDC_P_HW_ID_INVALID)
	{
		ulDacSrc = BCHP_MISC_DAC_0_CFG_SEL_HDSRC_0_CH0;
	}
#endif
	else
	{
		ulDacSrc = BCHP_MISC_DAC_0_CFG_SEL_SDSRC_0_CH0 + 3 * pChan->ulSdsrc;
	}

	return ulDacSrc;
}

static void BVDC_P_Display_Apply_DAC_Setting
	( BVDC_Display_Handle              hDisplay,
	  BVDC_P_ListInfo                 *pList,
	  BAVC_Polarity                    eFieldPolarity )
{
	uint32_t i, ulCurDac, ulDacSrc, ulDacMask = 0, ulDacGroupMask;
	BVDC_P_Output eOutputCS;
	BVDC_P_DisplayInfo *pCurInfo = &hDisplay->stCurInfo;
	uint32_t *pulDacBGAdj;
	const uint32_t *pulDacGrouping;
	BVDC_DacOutput *aDacOutput;
	bool bPowerupBG, bPowerdownBG, bValidBG;

	BDBG_MSG(("Display%d programming Dac...", hDisplay->eId));

	pulDacBGAdj = hDisplay->hVdc->stSettings.aulDacBandGapAdjust;
	pulDacGrouping = hDisplay->hVdc->aulDacGrouping;
	aDacOutput = hDisplay->hVdc->aDacOutput;

	for(i = 0; i < BVDC_P_MAX_DACS; i++)
	{
		switch(aDacOutput[i])
		{
			case BVDC_DacOutput_eFilteredCvbs:
				ulDacMask = BVDC_P_Dac_Mask_Cvbs;
				break;
			case BVDC_DacOutput_eComposite:
				ulDacMask = BVDC_P_Dac_Mask_Cvbs;
				break;
			case BVDC_DacOutput_eSVideo_Luma:
			case BVDC_DacOutput_eSVideo_Chroma:
				ulDacMask = BVDC_P_Dac_Mask_Svideo;
				break;
			case BVDC_DacOutput_eGreen:
			case BVDC_DacOutput_eGreen_NoSync:
			case BVDC_DacOutput_eRed:
			case BVDC_DacOutput_eBlue:
				ulDacMask = BVDC_P_Dac_Mask_RGB;
				break;
			case BVDC_DacOutput_eY:
			case BVDC_DacOutput_ePr:
			case BVDC_DacOutput_ePb:
				ulDacMask = BVDC_P_Dac_Mask_YPrPb;
				break;
			case BVDC_DacOutput_eUnused:
			default:
				ulDacMask = 0;
				break;
		}

		if (aDacOutput[i] != BVDC_DacOutput_eUnused)
		{
			/* Only attempt to find DAC source for those DACs belong to this display */
			if(hDisplay->hVdc->aDacDisplay[i] == hDisplay->eId)
			{
				ulDacSrc = BVDC_P_Dislay_FindDacSrc(hDisplay, ulDacMask);
			}
			else
				ulDacSrc = UINT32_C(-1);
		}
		else
			ulDacSrc = 0;

		if(ulDacSrc != UINT32_C(-1))
		{
			BVDC_P_Display_ProgramDac_isr(aDacOutput[i], i, ulDacSrc, pList);
		}
	}

	/* Power up bandgap control if any DAC in the group is in use. */
	/* Power down bandgap control if all the DACs in the group are not in use. */
	for(ulDacGroupMask = 1; ulDacGroupMask < (BVDC_P_MAX_DACS + 1); ulDacGroupMask++)
	{
		bPowerupBG = false;
		bPowerdownBG = true;
		bValidBG     = false;
		for (ulCurDac = 0; ulCurDac < BVDC_P_MAX_DACS; ulCurDac++)
		{
			/*BDBG_MSG(("ulDacGroupMask = %d, ulCurDac = %d, pualDacGrouping[%d] = %d, adacOutput[%d] = %d",
				ulDacGroupMask, ulCurDac, ulCurDac, pulDacGrouping[ulCurDac], ulCurDac, aDacOutput[ulCurDac]));*/

			if ((pulDacGrouping[ulCurDac] == ulDacGroupMask) && (aDacOutput[ulCurDac] != BVDC_DacOutput_eUnused))
			{
				bPowerupBG = true;
				bValidBG = true;
				bPowerdownBG = false;
				break;
			}
			else if (pulDacGrouping[ulCurDac] == ulDacGroupMask)
			{
				bValidBG = true;
			}
		}

		if (bPowerdownBG && bValidBG)
		{
			BDBG_MSG(("power down BG %d", ulDacGroupMask - 1));
#if (BVDC_P_SUPPORT_TDAC_VER >= BVDC_P_SUPPORT_TDAC_VER_11)
			*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
			*pList->pulCurrent++ = BRDC_REGISTER(BCHP_MISC_DAC_INST_BIAS_CTRL_0);
			*pList->pulCurrent++ =
				BCHP_FIELD_DATA(MISC_DAC_INST_BIAS_CTRL_0, PWRDN, 1);
#elif (BVDC_P_SUPPORT_TDAC_VER >= BVDC_P_SUPPORT_TDAC_VER_9)
			*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
			*pList->pulCurrent++ = BRDC_REGISTER(BCHP_MISC_DAC_BIAS_CTRL_0);
			*pList->pulCurrent++ =
				BCHP_FIELD_DATA(MISC_DAC_BIAS_CTRL_0, PWRDN, 1);
#else
			*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
			*pList->pulCurrent++ = BRDC_REGISTER(BCHP_MISC_DAC_BG_CTRL_0 + (ulDacGroupMask - 1) * 4);
			*pList->pulCurrent++ =
				BCHP_FIELD_ENUM(MISC_DAC_BG_CTRL_0, PWRDN,        PWRDN     ) |
				BCHP_FIELD_ENUM(MISC_DAC_BG_CTRL_0, PWRDN_REFAMP, POWER_DOWN);
#endif
		}

		if (bPowerupBG)
		{
#if (BVDC_P_SUPPORT_TDAC_VER >= BVDC_P_SUPPORT_TDAC_VER_9)
			uint32_t ulHDDacSyncSrc = 0;
			uint32_t ulSvideoDacSyncSrc = 0;
			uint32_t ulCvbsDacSyncSrc = 0;
			uint32_t aulDacScaleSel[BVDC_P_MAX_DACS];
			uint32_t aulDacSyncSource[BVDC_P_MAX_DACS];
			uint32_t aulDacSyncEn[BVDC_P_MAX_DACS];

			if(aDacOutput[0] == BVDC_DacOutput_eY || aDacOutput[0] == BVDC_DacOutput_eGreen)
				ulHDDacSyncSrc = 0;
#if (BVDC_P_MAX_DACS > 1)
			else if(aDacOutput[1] == BVDC_DacOutput_eY || aDacOutput[1] == BVDC_DacOutput_eGreen)
				ulHDDacSyncSrc = 1;
			else if(aDacOutput[2] == BVDC_DacOutput_eY || aDacOutput[2] == BVDC_DacOutput_eGreen)
				ulHDDacSyncSrc = 2;
			else if(aDacOutput[3] == BVDC_DacOutput_eY || aDacOutput[3] == BVDC_DacOutput_eGreen)
				ulHDDacSyncSrc = 3;
#endif

			if(aDacOutput[0] == BVDC_DacOutput_eSVideo_Luma)
				ulSvideoDacSyncSrc = 0;
#if (BVDC_P_MAX_DACS > 1)
			else if(aDacOutput[1] == BVDC_DacOutput_eSVideo_Luma)
				ulSvideoDacSyncSrc = 1;
			else if(aDacOutput[2] == BVDC_DacOutput_eSVideo_Luma)
				ulSvideoDacSyncSrc = 2;
			else if(aDacOutput[3] == BVDC_DacOutput_eSVideo_Luma)
				ulSvideoDacSyncSrc = 3;
#endif

			if(aDacOutput[0] == BVDC_DacOutput_eComposite)
				ulCvbsDacSyncSrc = 0;
#if (BVDC_P_MAX_DACS > 1)
			else if(aDacOutput[1] == BVDC_DacOutput_eComposite)
				ulCvbsDacSyncSrc = 1;
			else if(aDacOutput[2] == BVDC_DacOutput_eComposite)
				ulCvbsDacSyncSrc = 2;
			else if(aDacOutput[3] == BVDC_DacOutput_eComposite)
				ulCvbsDacSyncSrc = 3;
#endif

			if(aDacOutput[0] == BVDC_DacOutput_eGreen_NoSync)
				ulHDDacSyncSrc = ulCvbsDacSyncSrc;
#if (BVDC_P_MAX_DACS > 1)
			else if(aDacOutput[1] == BVDC_DacOutput_eGreen_NoSync)
				ulHDDacSyncSrc = ulCvbsDacSyncSrc;
			else if(aDacOutput[2] == BVDC_DacOutput_eGreen_NoSync)
				ulHDDacSyncSrc = ulCvbsDacSyncSrc;
			else if(aDacOutput[3] == BVDC_DacOutput_eGreen_NoSync)
				ulHDDacSyncSrc = ulCvbsDacSyncSrc;
#endif

			if(hDisplay->hVdc->bDacDetectionEnable)
			{
				/* Toggling plug in and plug out in BCHP_MISC_DAC_DETECT_EN_0 */
				*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
				*pList->pulCurrent++ = BRDC_REGISTER(BCHP_MISC_DAC_DETECT_EN_0);
				*pList->pulCurrent++ =
					BCHP_FIELD_DATA(MISC_DAC_DETECT_EN_0, USE_STEP_DLY,   0) |
					BCHP_FIELD_DATA(MISC_DAC_DETECT_EN_0, SW_CALIBRATE,   0) |
					BCHP_FIELD_DATA(MISC_DAC_DETECT_EN_0, CALIBRATE,      1) |
					BCHP_FIELD_DATA(MISC_DAC_DETECT_EN_0, PLUGOUT_DETECT, 0) |
					BCHP_FIELD_DATA(MISC_DAC_DETECT_EN_0, PLUGIN_DETECT,  0) |
					BCHP_FIELD_DATA(MISC_DAC_DETECT_EN_0, AUTO_DETECT,    1);
			}

			for (i = 0; i < BVDC_P_MAX_DACS; i++)
			{
				aulDacScaleSel[i] =
					(hDisplay->hVdc->bCalibrated == 0) ? 3 :
					(aDacOutput[i] == BVDC_DacOutput_eComposite) ? 3 :
					(aDacOutput[i] == BVDC_DacOutput_eY &&
					 pCurInfo->eMacrovisionType != BVDC_MacrovisionType_eNoProtection) ? 2 :
					(aDacOutput[i] == BVDC_DacOutput_eY     ||
					 aDacOutput[i] == BVDC_DacOutput_ePb    ||
					 aDacOutput[i] == BVDC_DacOutput_ePr    ||
					 aDacOutput[i] == BVDC_DacOutput_eGreen ||
					 aDacOutput[i] == BVDC_DacOutput_eBlue  ||
					 aDacOutput[i] == BVDC_DacOutput_eRed) ? 1 : 0;

				aulDacSyncSource[i] =
					(aDacOutput[i] == BVDC_DacOutput_ePb   ||
					 aDacOutput[i] == BVDC_DacOutput_ePr   ||
					 aDacOutput[i] == BVDC_DacOutput_eY ||
					 aDacOutput[i] == BVDC_DacOutput_eGreen ||
					 aDacOutput[i] == BVDC_DacOutput_eGreen_NoSync ||
					 aDacOutput[i] == BVDC_DacOutput_eBlue ||
					 aDacOutput[i] == BVDC_DacOutput_eRed) ? ulHDDacSyncSrc :
					(aDacOutput[i] == BVDC_DacOutput_eSVideo_Luma ||
					 aDacOutput[i] == BVDC_DacOutput_eSVideo_Chroma) ? ulSvideoDacSyncSrc : 3;

				aulDacSyncEn[i] =
 					(aDacOutput[i] == BVDC_DacOutput_eY ||
					 aDacOutput[i] == BVDC_DacOutput_eGreen ||
					 aDacOutput[i] == BVDC_DacOutput_eComposite) ? 1 : 0;
			}

#if (BVDC_P_SUPPORT_TDAC_VER == BVDC_P_SUPPORT_TDAC_VER_9) || (BVDC_P_SUPPORT_TDAC_VER == BVDC_P_SUPPORT_TDAC_VER_10)
			/* BCHP_MISC_DAC_BIAS_CTRL_0 */
			*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
			*pList->pulCurrent++ = BRDC_REGISTER(BCHP_MISC_DAC_BIAS_CTRL_0);
			*pList->pulCurrent++ =
				BCHP_FIELD_DATA(MISC_DAC_BIAS_CTRL_0, DAC3_SCALE_LP,  0             ) | /* optimmal */
				BCHP_FIELD_DATA(MISC_DAC_BIAS_CTRL_0, DAC3_SCALE_SEL, aulDacScaleSel[3]) |
				BCHP_FIELD_DATA(MISC_DAC_BIAS_CTRL_0, DAC2_SCALE_LP,  0             ) | /* optimmal */
				BCHP_FIELD_DATA(MISC_DAC_BIAS_CTRL_0, DAC2_SCALE_SEL, aulDacScaleSel[2]) |
				BCHP_FIELD_DATA(MISC_DAC_BIAS_CTRL_0, DAC1_SCALE_LP,  0             ) | /* optimmal */
				BCHP_FIELD_DATA(MISC_DAC_BIAS_CTRL_0, DAC1_SCALE_SEL, aulDacScaleSel[1]) |
				BCHP_FIELD_DATA(MISC_DAC_BIAS_CTRL_0, DAC0_SCALE_LP,  0             ) | /* optimmal */
				BCHP_FIELD_DATA(MISC_DAC_BIAS_CTRL_0, DAC0_SCALE_SEL, aulDacScaleSel[0]) |
				BCHP_FIELD_DATA(MISC_DAC_BIAS_CTRL_0, GAIN_ADJ,       pulDacBGAdj[ulCurDac]) | /* optimmal */
				BCHP_FIELD_DATA(MISC_DAC_BIAS_CTRL_0, GAIN_OVERRIDE,  0             ) |
				BCHP_FIELD_DATA(MISC_DAC_BIAS_CTRL_0, REG_ADJ,        4             ) | /* optimmal */
				BCHP_FIELD_DATA(MISC_DAC_BIAS_CTRL_0, BG_ADJ,         5             ) |
				BCHP_FIELD_DATA(MISC_DAC_BIAS_CTRL_0, PWRDN,          0             );

			/* BCHP_MISC_DAC_CTRL_0 */
			*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
			*pList->pulCurrent++ = BRDC_REGISTER(BCHP_MISC_DAC_CTRL_0);
			*pList->pulCurrent++ =
				BCHP_FIELD_DATA(MISC_DAC_CTRL_0, DAC3_RSTB,         1   ) | /* optimmal */
				BCHP_FIELD_DATA(MISC_DAC_CTRL_0, DAC2_RSTB,         1   ) | /* optimmal */
				BCHP_FIELD_DATA(MISC_DAC_CTRL_0, DAC1_RSTB,         1   ) | /* optimmal */
				BCHP_FIELD_DATA(MISC_DAC_CTRL_0, DAC0_RSTB,         1   ) | /* optimmal */
				BCHP_FIELD_DATA(MISC_DAC_CTRL_0, TBD,               0   ) | /* optimmal */
				BCHP_FIELD_DATA(MISC_DAC_CTRL_0, PRBS_LENGTH,       0   ) | /* optimmal */
				BCHP_FIELD_ENUM(MISC_DAC_CTRL_0, PRBS_MODE,         PRBS) | /* optimmal */
				BCHP_FIELD_ENUM(MISC_DAC_CTRL_0, DISABLE_PRBS,      EN  ) | /* optimmal */
				BCHP_FIELD_ENUM(MISC_DAC_CTRL_0, DISABLE_SCRAMBLER, EN  ) | /* optimmal */
				BCHP_FIELD_DATA(MISC_DAC_CTRL_0, DAC3_FORMAT,       0   ) | /* optimmal */
				BCHP_FIELD_DATA(MISC_DAC_CTRL_0, DAC2_FORMAT,       0   ) | /* optimmal */
				BCHP_FIELD_DATA(MISC_DAC_CTRL_0, DAC1_FORMAT,       0   ) | /* optimmal */
				BCHP_FIELD_DATA(MISC_DAC_CTRL_0, DAC0_FORMAT,       0   ) | /* optimmal */
				BCHP_FIELD_DATA(MISC_DAC_CTRL_0, DAC3_PWRDN,
					(aDacOutput[3] == BVDC_DacOutput_eUnused) ? 1 : 0) |
				BCHP_FIELD_DATA(MISC_DAC_CTRL_0, DAC2_PWRDN,
					(aDacOutput[2] == BVDC_DacOutput_eUnused) ? 1 : 0) |
				BCHP_FIELD_DATA(MISC_DAC_CTRL_0, DAC1_PWRDN,
					(aDacOutput[1] == BVDC_DacOutput_eUnused) ? 1 : 0) |
				BCHP_FIELD_DATA(MISC_DAC_CTRL_0, DAC0_PWRDN,
					(aDacOutput[0] == BVDC_DacOutput_eUnused) ? 1 : 0);

#elif (BVDC_P_SUPPORT_TDAC_VER >= BVDC_P_SUPPORT_TDAC_VER_11)
			/* BCHP_MISC_DAC_INST_BIAS_CTRL_0 */
			*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
			*pList->pulCurrent++ = BRDC_REGISTER(BCHP_MISC_DAC_INST_BIAS_CTRL_0);
			*pList->pulCurrent++ =
				BCHP_FIELD_DATA(MISC_DAC_INST_BIAS_CTRL_0, GAIN_ADJ,       pulDacBGAdj[ulCurDac]) |
				BCHP_FIELD_DATA(MISC_DAC_INST_BIAS_CTRL_0, GAIN_OVERRIDE,  0             ) |
				BCHP_FIELD_DATA(MISC_DAC_INST_BIAS_CTRL_0, REG_ADJ,        4             ) | /* optimmal */
				BCHP_FIELD_DATA(MISC_DAC_INST_BIAS_CTRL_0, BG_ADJ,         5             ) |
				BCHP_FIELD_DATA(MISC_DAC_INST_BIAS_CTRL_0, PWRDN,          0             );

#if (BVDC_P_SUPPORT_TDAC_VER <= BVDC_P_SUPPORT_TDAC_VER_12)
			/* BCHP_MISC_DAC_INST_PRBS_CTRL_0 */
			*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
			*pList->pulCurrent++ = BRDC_REGISTER(BCHP_MISC_DAC_INST_PRBS_CTRL_0);
			*pList->pulCurrent++ =
				BCHP_FIELD_DATA(MISC_DAC_INST_PRBS_CTRL_0, TBD,               0   ) | /* optimmal */
				BCHP_FIELD_DATA(MISC_DAC_INST_PRBS_CTRL_0, PRBS_LENGTH,       0   ) | /* optimmal */
				BCHP_FIELD_ENUM(MISC_DAC_INST_PRBS_CTRL_0, PRBS_MODE,         PRBS) | /* optimmal */
				BCHP_FIELD_ENUM(MISC_DAC_INST_PRBS_CTRL_0, DISABLE_PRBS,      EN  ) | /* optimmal */
				BCHP_FIELD_ENUM(MISC_DAC_INST_PRBS_CTRL_0, DISABLE_SCRAMBLER, EN  );  /* optimmal */
#endif
			for (i = 0; i < BVDC_P_MAX_DACS; i++)
			{
#if (BVDC_P_MAX_DACS > 1)
				uint32_t ulOffset = (BCHP_MISC_DAC_1_SCALE_CTRL - BCHP_MISC_DAC_0_SCALE_CTRL) * i;
#else
				uint32_t ulOffset = 0;
#endif
				/* BCHP_MISC_DAC_x_SCALE_CTRL  */
				*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
				*pList->pulCurrent++ = BRDC_REGISTER(BCHP_MISC_DAC_0_SCALE_CTRL + ulOffset);
				*pList->pulCurrent++ =
					BCHP_FIELD_DATA(MISC_DAC_0_SCALE_CTRL, SCALE_LP,  0             ) | /* optimmal */
					BCHP_FIELD_DATA(MISC_DAC_0_SCALE_CTRL, SCALE_SEL, aulDacScaleSel[i]);
			}
#endif

#if (BVDC_P_MAX_DACS > 1)
			/* if only 1 DAC, use RESET val = DAC0 */
			/* BCHP_MISC_DAC_DETECT_SYNC_CTRL_0 */
			*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
			*pList->pulCurrent++ = BRDC_REGISTER(BCHP_MISC_DAC_DETECT_SYNC_CTRL_0);
			*pList->pulCurrent++ =
				BCHP_FIELD_DATA(MISC_DAC_DETECT_SYNC_CTRL_0, DAC3_SYNC_SOURCE, aulDacSyncSource[3]) |
				BCHP_FIELD_DATA(MISC_DAC_DETECT_SYNC_CTRL_0, DAC2_SYNC_SOURCE, aulDacSyncSource[2]) |
				BCHP_FIELD_DATA(MISC_DAC_DETECT_SYNC_CTRL_0, DAC1_SYNC_SOURCE, aulDacSyncSource[1]) |
				BCHP_FIELD_DATA(MISC_DAC_DETECT_SYNC_CTRL_0, DAC0_SYNC_SOURCE, aulDacSyncSource[0]) |
				BCHP_FIELD_DATA(MISC_DAC_DETECT_SYNC_CTRL_0, DAC3_SYNC_EN, aulDacSyncEn[3]        ) |
				BCHP_FIELD_DATA(MISC_DAC_DETECT_SYNC_CTRL_0, DAC2_SYNC_EN, aulDacSyncEn[2]        ) |
				BCHP_FIELD_DATA(MISC_DAC_DETECT_SYNC_CTRL_0, DAC1_SYNC_EN, aulDacSyncEn[1]        ) |
				BCHP_FIELD_DATA(MISC_DAC_DETECT_SYNC_CTRL_0, DAC0_SYNC_EN, aulDacSyncEn[0]        );
#endif

			if(hDisplay->hVdc->bDacDetectionEnable)
			{
				*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
				*pList->pulCurrent++ = BRDC_REGISTER(BCHP_MISC_DAC_DETECT_EN_0);
				*pList->pulCurrent++ =
					BCHP_FIELD_DATA(MISC_DAC_DETECT_EN_0, USE_STEP_DLY,   0) |
					BCHP_FIELD_DATA(MISC_DAC_DETECT_EN_0, SW_CALIBRATE,   0) |
					BCHP_FIELD_DATA(MISC_DAC_DETECT_EN_0, CALIBRATE,      1) |
					BCHP_FIELD_DATA(MISC_DAC_DETECT_EN_0, PLUGOUT_DETECT, 1) |
					BCHP_FIELD_DATA(MISC_DAC_DETECT_EN_0, PLUGIN_DETECT,  1) |
					BCHP_FIELD_DATA(MISC_DAC_DETECT_EN_0, AUTO_DETECT,    1);
			}
#else
			*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
			*pList->pulCurrent++ = BRDC_REGISTER(BCHP_MISC_DAC_BG_CTRL_0 + (ulDacGroupMask - 1) * 4);
			*pList->pulCurrent++ =
				BCHP_FIELD_ENUM(MISC_DAC_BG_CTRL_0, PWRDN, PWRUP            ) |
				BCHP_FIELD_ENUM(MISC_DAC_BG_CTRL_0, CORE_ADJ, FOUR          ) |
				BCHP_FIELD_ENUM(MISC_DAC_BG_CTRL_0, BANDGAP_BYP, BANDGAP    ) |
				BCHP_FIELD_DATA(MISC_DAC_BG_CTRL_0, IREF_ADJ, pulDacBGAdj[ulCurDac]) |
				BCHP_FIELD_ENUM(MISC_DAC_BG_CTRL_0, PWRDN_REFAMP, POWER_UP  );
#endif
			BDBG_MSG(("power up BG %d", ulDacGroupMask - 1));
		}
		/*BDBG_MSG(("end of mask %d", ulDacGroupMask));*/
	}

	/* Program cores which are driven by output color space. */
	if(hDisplay->bAnlgEnable ||   /* if analog master */
	   (!hDisplay->bAnlgEnable &&  /* or anlog slave with DACs */
	   (hDisplay->stAnlgChan_0.bEnable || hDisplay->stAnlgChan_1.bEnable)))
	{
		BVDC_P_Vec_Build_RM_isr(hDisplay, &hDisplay->stAnlgChan_0, pList);
	}
	if (hDisplay->stAnlgChan_0.bEnable)
	{
		eOutputCS = pCurInfo->eAnlg_0_OutputColorSpace;
		BDBG_ASSERT(stVDC_P_Output_InfoTbl[eOutputCS].eVdcOutput == pCurInfo->eAnlg_0_OutputColorSpace);
		BDBG_MSG(("Display %d Anlg 0 using %s", hDisplay->eId, stVDC_P_Output_InfoTbl[eOutputCS].pcVdcOutputStr));
		BVDC_P_Vec_Build_CSC_SRC_SM_isr(hDisplay, &hDisplay->stAnlgChan_0, eOutputCS, pList);
		BVDC_P_Vec_Build_VF_isr(hDisplay, &hDisplay->stAnlgChan_0, eOutputCS, pList);
	}

	if (hDisplay->stAnlgChan_1.bEnable)
	{
		eOutputCS = pCurInfo->eAnlg_1_OutputColorSpace;
		BDBG_ASSERT(stVDC_P_Output_InfoTbl[eOutputCS].eVdcOutput == pCurInfo->eAnlg_1_OutputColorSpace);
		BDBG_MSG(("Display %d Anlg 1 using %s", hDisplay->eId, stVDC_P_Output_InfoTbl[pCurInfo->eAnlg_1_OutputColorSpace].pcVdcOutputStr));
		BVDC_P_Vec_Build_CSC_SRC_SM_isr(hDisplay, &hDisplay->stAnlgChan_1, eOutputCS, pList);
		BVDC_P_Vec_Build_VF_isr(hDisplay, &hDisplay->stAnlgChan_1, eOutputCS, pList);
	}

	/* Start micro controllers. This has to be after
	 * the VEC pipeline is established.  */
	BVDC_P_Display_StartMicroCtrler(hDisplay, pList);

	hDisplay->stCurInfo.stDirty.stBits.bDacSetting = BVDC_P_CLEAN;

#if DCS_SUPPORT
	BVDC_P_DCS_StateFault_isr (hDisplay);
#endif

	BSTD_UNUSED(eFieldPolarity);

	return;
}


/***************** Setting callback hanlders *********************/
static void BVDC_P_Display_Copy_Callback_Setting
	( BVDC_Display_Handle              hDisplay )
{
	hDisplay->stCurInfo.stCallbackSettings = hDisplay->stNewInfo.stCallbackSettings;

	return;
}

static void BVDC_P_Display_Apply_Callback_Setting
	( BVDC_Display_Handle              hDisplay,
	  BVDC_P_ListInfo                 *pList,
	  BAVC_Polarity                    eFieldPolarity )
{
	BSTD_UNUSED(pList);
	BSTD_UNUSED(eFieldPolarity);

	hDisplay->stCurInfo.stDirty.stBits.bCallback = BVDC_P_CLEAN;

	return;
}

static void BVDC_P_Display_Copy_CallbackFunc_Setting
	( BVDC_Display_Handle              hDisplay )
{
	BVDC_P_DisplayInfo *pCurInfo, *pNewInfo;

	pCurInfo = &hDisplay->stCurInfo;
	pNewInfo = &hDisplay->stNewInfo;

	pCurInfo->pfGenericCallback = pNewInfo->pfGenericCallback;
	pCurInfo->pvGenericParm1    = pNewInfo->pvGenericParm1;
	pCurInfo->iGenericParm2      = pNewInfo->iGenericParm2;

	return;
}

static void BVDC_P_Display_Apply_CallbackFunc_Setting
	( BVDC_Display_Handle              hDisplay,
	  BVDC_P_ListInfo                 *pList,
	  BAVC_Polarity                    eFieldPolarity )
{
	BSTD_UNUSED(pList);
	BSTD_UNUSED(eFieldPolarity);

	hDisplay->stCurInfo.stDirty.stBits.bCallbackFunc = BVDC_P_CLEAN;

	return;
}


/****************** Trimming width event handlers ******************/
static void BVDC_P_Display_Apply_TrimmingWidth_Setting
	( BVDC_Display_Handle              hDisplay,
	  BVDC_P_ListInfo                 *pList,
	  BAVC_Polarity                    eFieldPolarity )
{
	const uint32_t      *pRamTbl = NULL;
	BVDC_P_DisplayInfo  *pCurInfo = &hDisplay->stCurInfo;
	uint32_t             ulItOffset;

	BSTD_UNUSED(pList);
	BSTD_UNUSED(eFieldPolarity);

	if(hDisplay->bAnlgEnable ||   /* if analog master */
	   (!hDisplay->bAnlgEnable &&  /* or anlog slave with DACs */
	   (hDisplay->stAnlgChan_0.bEnable || hDisplay->stAnlgChan_1.bEnable)))
	{

		/* No VEC reset when switching between 704 and 720. Only load
		 * new micro-code and update some register settings. This can
		 * avoid TV resync.
		 */
		ulItOffset  = hDisplay->stAnlgChan_0.ulIt * BVDC_P_VEC_IT_SIZE;
		BVDC_P_Display_Build_ItBvbSize_isr(hDisplay, pList, ulItOffset);

		/* Update VF shaper settings */
		if (hDisplay->stAnlgChan_0.bEnable)
		{
			BVDC_P_Vec_Build_VF_isr(hDisplay, &hDisplay->stAnlgChan_0,
				pCurInfo->eAnlg_0_OutputColorSpace, pList);
		}

		if (hDisplay->stAnlgChan_1.bEnable)
		{
			BVDC_P_Vec_Build_VF_isr(hDisplay, &hDisplay->stAnlgChan_1,
				pCurInfo->eAnlg_1_OutputColorSpace, pList);
		}

		/* Load new micro-code */
		pRamTbl   = BVDC_P_GetRamTable_isr(pCurInfo, hDisplay->bArib480p);
		BDBG_ASSERT (pRamTbl);
		BVDC_P_Vec_Build_ItMc_isr(pList, pRamTbl, ulItOffset);
	}

	hDisplay->stCurInfo.stDirty.stBits.bWidthTrim = BVDC_P_CLEAN;

	return;
}


/****************** Input color space event handlers ******************/
static void BVDC_P_Display_Apply_InputColorSpace_Setting
	( BVDC_Display_Handle              hDisplay,
	  BVDC_P_ListInfo                 *pList,
	  BAVC_Polarity                    eFieldPolarity )
{
	BVDC_P_Output       eOutputCS;

	if (hDisplay->stAnlgChan_0.bEnable)
	{
		eOutputCS = hDisplay->stCurInfo.eAnlg_0_OutputColorSpace;
		BVDC_P_Vec_Build_CSC_SRC_SM_isr(hDisplay, &hDisplay->stAnlgChan_0, eOutputCS, pList);
	}

	if (hDisplay->stAnlgChan_1.bEnable)
	{
		eOutputCS = hDisplay->stCurInfo.eAnlg_1_OutputColorSpace;
		BVDC_P_Vec_Build_CSC_SRC_SM_isr(hDisplay, &hDisplay->stAnlgChan_1, eOutputCS, pList);
	}

	if(hDisplay->stDviChan.bEnable || hDisplay->stCurInfo.bEnableHdmi)
	{
		BVDC_P_Vec_Build_DVI_CSC_isr(hDisplay, pList );
	}

#if (BVDC_P_SUPPORT_ITU656_OUT != 0)
	if (hDisplay->st656Chan.bEnable)
	{
		BVDC_P_Vec_Build_656_CSC_isr(hDisplay, pList );
	}
#endif

	hDisplay->stCurInfo.stDirty.stBits.bInputCS = BVDC_P_CLEAN;

	BSTD_UNUSED(eFieldPolarity);

	return;
}


/*************************************************************************
 *  {secret}
 *  BVDC_P_Vec_Build_240p_isr
 *
 *  Adds Vec settings required for 240p mode updates:
 **************************************************************************/
static void BVDC_P_Vec_Build_240p_isr
	( uint32_t                         ulItOffset,
	  BVDC_P_ListInfo                 *pList,
	  const uint32_t                   ul240pMode )
{
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_IT_0_STACK_reg_0_1 + ulItOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(IT_0_STACK_reg_0_1, REG_0, ul240pMode);

	return;
}


/****************** 240p mode event handlers ******************/
static void BVDC_P_Display_Apply_240pMode_Setting
	( BVDC_Display_Handle              hDisplay,
	  BVDC_P_ListInfo                 *pList,
	  BAVC_Polarity                    eFieldPolarity )
{
	uint32_t ulOffset;
	uint32_t ul240pMode;
	BVDC_P_DisplayInfo *pCurInfo;

	pCurInfo = &hDisplay->stCurInfo;
	ul240pMode = hDisplay->stCurInfo.b240pMode ? 1: 0;

	if(VIDEO_FORMAT_IS_NTSC(pCurInfo->pFmtInfo->eVideoFmt))
	{
		if(hDisplay->bAnlgEnable ||   /* if analog master */
		   (!hDisplay->bAnlgEnable &&  /* or anlog slave with DACs */
		   (hDisplay->stAnlgChan_0.bEnable || hDisplay->stAnlgChan_1.bEnable)))
		{
			{
				BDBG_MSG(("disp[%d] Chan_0 - Build 240p Mode ", hDisplay->eId));
				ulOffset = hDisplay->stAnlgChan_0.ulIt * BVDC_P_VEC_IT_SIZE;
				BVDC_P_Vec_Build_240p_isr(ulOffset, pList, ul240pMode);
			}
		}
	}

	hDisplay->stCurInfo.stDirty.stBits.b240p = BVDC_P_CLEAN;

	BSTD_UNUSED(eFieldPolarity);

	return;
}

/****************** VDEC->VEC phase adjustment event handlers ******************/
static void BVDC_P_Display_Apply_PhaseAdjustement_Setting
	( BVDC_Display_Handle              hDisplay,
	  BVDC_P_ListInfo                 *pList,
	  BAVC_Polarity                    eFieldPolarity )
{

	hDisplay->stCurInfo.stDirty.stBits.bPhaseAdjustment = BVDC_P_CLEAN;

	/* We won't have this event on STB platforms */
	BSTD_UNUSED(pList);
	BSTD_UNUSED(eFieldPolarity);

	return;
}

/****************** Source frame rate change event handlers ******************/
static BERR_Code BVDC_P_Display_Validate_SrcFrameRate_Setting
	( BVDC_Display_Handle              hDisplay )
{
	const uint32_t          *pTable;
	BAVC_VdcDisplay_Info     lRateInfo;
	BVDC_P_DisplayInfo      *pNewInfo = &hDisplay->stNewInfo;

	/* STG doesn't have RM */
	if((!BVDC_P_DISPLAY_USED_STG(hDisplay->eMasterTg)) &&
	   ((BVDC_P_GetRmTable_isr(pNewInfo, pNewInfo->pFmtInfo, &pTable, true, &lRateInfo) != BERR_SUCCESS) ||
		(BVDC_P_GetRmTable_isr(pNewInfo, pNewInfo->pFmtInfo, &pTable, false, &lRateInfo) != BERR_SUCCESS)))
	{
		BDBG_ERR(("Failed to locate Rate Manager Table"));
		return BERR_TRACE(BVDC_ERR_FORMAT_NOT_SUPPORT_ANALOG_OUTPUT);
	}

	return BERR_SUCCESS;
}

static void BVDC_P_Display_Apply_SrcFrameRate_Setting
	( BVDC_Display_Handle              hDisplay,
	  BVDC_P_ListInfo                 *pList,
	  BAVC_Polarity                    eFieldPolarity )
{
	BSTD_UNUSED(eFieldPolarity);

	if(hDisplay->bAnlgEnable ||   /* if analog master */
	   (!hDisplay->bAnlgEnable &&  /* or anlog slave with DACs */
	   (hDisplay->stAnlgChan_0.bEnable || hDisplay->stAnlgChan_1.bEnable)))
	{
		BVDC_P_Vec_Build_RM_isr(hDisplay, &hDisplay->stAnlgChan_0, pList);
	}

	if(hDisplay->stDviChan.bEnable || hDisplay->stCurInfo.bEnableHdmi)
	{
		BVDC_P_Vec_Build_Hdmi_isr(hDisplay, pList);
	}

#if (BVDC_P_SUPPORT_STG != 0)
	if (hDisplay->stStgChan.bEnable)
	{
		BVDC_P_STG_Build_RM_isr(hDisplay, pList);
	}
#endif
	hDisplay->stCurInfo.stDirty.stBits.bSrcFrameRate = BVDC_P_CLEAN;

	return;
}


/**************** Setting force frame drop event hanlders **************/
static void BVDC_P_Display_Copy_FrameDrop_Setting
	( BVDC_Display_Handle              hDisplay )
{
	hDisplay->stCurInfo.bForceFrameDrop = hDisplay->stNewInfo.bForceFrameDrop;

	return;
}

static void BVDC_P_Display_Apply_FrameDrop_Setting
	( BVDC_Display_Handle              hDisplay,
	  BVDC_P_ListInfo                 *pList,
	  BAVC_Polarity                    eFieldPolarity )
{
	BSTD_UNUSED(eFieldPolarity);

	if(hDisplay->bAnlgEnable ||   /* if analog master */
	   (!hDisplay->bAnlgEnable &&  /* or anlog slave with DACs */
	   (hDisplay->stAnlgChan_0.bEnable || hDisplay->stAnlgChan_1.bEnable)))
	{
		BVDC_P_Vec_Build_RM_isr(hDisplay, &hDisplay->stAnlgChan_0, pList);
	}

	hDisplay->stCurInfo.stDirty.stBits.bFrameDrop = BVDC_P_CLEAN;

	return;
}


/**************** Component path MPAA setting event hanlders **************/
static BERR_Code BVDC_P_Display_Validate_CompMpaa_Setting
	( BVDC_Display_Handle              hDisplay )
{
	BERR_Code err;

	if (hDisplay->stNewInfo.aulEnableMpaaDeci[BVDC_MpaaDeciIf_eComponent] &&
		BVDC_P_HW_ID_INVALID == hDisplay->stMpaaComp.ulHwId)
	{
		/* query only */
		err = BVDC_P_Resource_AcquireHwId_isr(hDisplay->hVdc->hResource,
			BVDC_P_ResourceType_eMpaa, 0, hDisplay->eId, &hDisplay->stMpaaComp.ulHwId, true);
		if (BERR_SUCCESS != err)
		{
			return BERR_TRACE(err);
		}
	}

	return BERR_SUCCESS;
}

static void BVDC_P_Display_Copy_CompMpaa_Setting
	( BVDC_Display_Handle              hDisplay )
{
	if (hDisplay->stNewInfo.aulEnableMpaaDeci[BVDC_MpaaDeciIf_eComponent] &&
		(hDisplay->stNewInfo.bRgb || hDisplay->stNewInfo.bYPrPb))
	{
		if (BVDC_P_RESOURCE_ID_AVAIL == hDisplay->stMpaaComp.ulHwId)
		{
			BERR_Code err;

			/* queried, but not acquired yet */
			err = BVDC_P_Resource_AcquireHwId_isr(hDisplay->hVdc->hResource,
				BVDC_P_ResourceType_eMpaa, 0, hDisplay->eId, &hDisplay->stMpaaComp.ulHwId, false);
			if (BERR_SUCCESS != err)
			{
				BDBG_ERR(("Display %d fail to acquire component mpaa!", hDisplay->eId));
				return;
			}
			BDBG_MSG(("acquire component mpaa %d", hDisplay->stMpaaComp.ulHwId));
		}
		/* init enabling and connecting */
		hDisplay->stMpaaComp.eState = BVDC_P_DisplayResource_eCreate;
	}
	else if (BVDC_P_DisplayResource_eActivating == hDisplay->stMpaaComp.eState ||
			 BVDC_P_DisplayResource_eActive     == hDisplay->stMpaaComp.eState)
	{
		/* init shut down process */
		hDisplay->stMpaaComp.eState = BVDC_P_DisplayResource_eDestroy;
	}
	else if (!hDisplay->stNewInfo.aulEnableMpaaDeci[BVDC_MpaaDeciIf_eComponent] &&
			 hDisplay->stCurInfo.aulEnableMpaaDeci[BVDC_MpaaDeciIf_eComponent] &&
			 BVDC_P_DisplayResource_eDestroy      != hDisplay->stMpaaComp.eState &&
			 BVDC_P_DisplayResource_eShuttingdown != hDisplay->stMpaaComp.eState)
	{
		if (BVDC_P_HW_ID_INVALID     != hDisplay->stMpaaComp.ulHwId &&
			BVDC_P_RESOURCE_ID_AVAIL != hDisplay->stMpaaComp.ulHwId)
		{
			BDBG_MSG(("release component mpaa %d", hDisplay->stMpaaComp.ulHwId));
			BVDC_P_Resource_ReleaseHwId_isr(hDisplay->hVdc->hResource,
				BVDC_P_ResourceType_eMpaa, hDisplay->stMpaaComp.ulHwId);
			hDisplay->stMpaaComp.ulHwId = BVDC_P_HW_ID_INVALID;
		}
		hDisplay->stMpaaComp.eState = BVDC_P_DisplayResource_eInactive;
	}

	hDisplay->stCurInfo.aulEnableMpaaDeci[BVDC_MpaaDeciIf_eComponent] =
		hDisplay->stNewInfo.aulEnableMpaaDeci[BVDC_MpaaDeciIf_eComponent];

	return;
}

static void BVDC_P_EnableMpaa
	( BVDC_Display_Handle              hDisplay,
	  BVDC_P_ListInfo                 *pList )
{
	uint32_t	ulSrc = 0;

	/* during format change, we need to recfg even if already enabled */
#if 0
	if (BVDC_P_DisplayResource_eActive == hDisplay->stMpaaHdmi.eState ||
		BVDC_P_DisplayResource_eActive == hDisplay->stMpaaComp.eState)
	{
		/* already enabled */
		return;
	}
#endif

	/* reset DECIM */
#if BVDC_P_SUPPORT_NEW_SW_INIT
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VEC_CFG_SW_INIT_DECIM_0);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VEC_CFG_SW_INIT_DECIM_0, INIT, 1);

	/* Unreset DECIM */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VEC_CFG_SW_INIT_DECIM_0);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VEC_CFG_SW_INIT_DECIM_0, INIT, 0);
#else
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VEC_CFG_SW_RESET_DECIM_0);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VEC_CFG_SW_RESET_DECIM_0, RESET, 1);

	/* Unreset DECIM */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VEC_CFG_SW_RESET_DECIM_0);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VEC_CFG_SW_RESET_DECIM_0, RESET, 0);
#endif

	/* Connect DECIM to compositor */
	ulSrc = BVDC_P_GetVecCfgSrc(hDisplay);
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VEC_CFG_DECIM_0_SOURCE);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VEC_CFG_DECIM_0_SOURCE, SOURCE, ulSrc);

	/* Configure DECIM */
	/* DECIMATE_RATIO: The MPAA standard specifies the resolution to be
	 * not greater than 520000 pixels in a frame */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VIDEO_ENC_DECIM_0_DECIM_CONTROL);
	if((hDisplay->stCurInfo.pFmtInfo->ulWidth *
	    hDisplay->stCurInfo.pFmtInfo->ulHeight)/2 < 520000)
	{
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(VIDEO_ENC_DECIM_0_DECIM_CONTROL, PASSTHROUGH_COUNT, 0) |
			BCHP_FIELD_ENUM(VIDEO_ENC_DECIM_0_DECIM_CONTROL, DECIMATE_RATIO, BY2)  |
			BCHP_FIELD_ENUM(VIDEO_ENC_DECIM_0_DECIM_CONTROL, DECIMATE_SAMPLING_EN, ON) |
			BCHP_FIELD_ENUM(VIDEO_ENC_DECIM_0_DECIM_CONTROL, DECIMATE_FILTER_EN, ON);
	}
	else
	{
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(VIDEO_ENC_DECIM_0_DECIM_CONTROL, PASSTHROUGH_COUNT, 0) |
			BCHP_FIELD_ENUM(VIDEO_ENC_DECIM_0_DECIM_CONTROL, DECIMATE_RATIO, BY4) |
			BCHP_FIELD_ENUM(VIDEO_ENC_DECIM_0_DECIM_CONTROL, DECIMATE_SAMPLING_EN, ON) |
			BCHP_FIELD_ENUM(VIDEO_ENC_DECIM_0_DECIM_CONTROL, DECIMATE_FILTER_EN, ON);
	}

	return;
}

static void BVDC_P_DisableMpaa
	( BVDC_Display_Handle              hDisplay,
	  BVDC_P_ListInfo                 *pList )
{
	if (BVDC_P_DisplayResource_eInactive == hDisplay->stMpaaHdmi.eState &&
		BVDC_P_DisplayResource_eInactive == hDisplay->stMpaaComp.eState)
	{
		/* already disabled */
		return;
	}

	/* Disable DECIM source */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VEC_CFG_DECIM_0_SOURCE);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VEC_CFG_DECIM_0_SOURCE, SOURCE, BCHP_VEC_CFG_DECIM_0_SOURCE_SOURCE_DISABLE);

	/* reset DECIM */
#if BVDC_P_SUPPORT_NEW_SW_INIT
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VEC_CFG_SW_INIT_DECIM_0);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VEC_CFG_SW_INIT_DECIM_0, INIT, 1);

	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VEC_CFG_SW_INIT_DECIM_0);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VEC_CFG_SW_INIT_DECIM_0, INIT, 0);
#else
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VEC_CFG_SW_RESET_DECIM_0);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VEC_CFG_SW_RESET_DECIM_0, RESET, 1);

	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VEC_CFG_SW_RESET_DECIM_0);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VEC_CFG_SW_RESET_DECIM_0, RESET, 0);
#endif

	return;
}

static void BVDC_P_MpaaComp_Setup_isr
	( BVDC_Display_Handle              hDisplay,
	  BVDC_P_ListInfo                 *pList,
	  BVDC_P_DisplayAnlgChan          *pstChan)
{
	/* inside it will avoid re-do */
	BVDC_P_EnableMpaa(hDisplay, pList);

	/* Connect IT to DECIM */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VEC_CFG_IT_0_SOURCE + pstChan->ulIt * 4);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VEC_CFG_IT_0_SOURCE, SOURCE, BCHP_VEC_CFG_IT_0_SOURCE_SOURCE_DECIM_0);
}

static void BVDC_P_MpaaComp_Shutdown_isr
	( BVDC_Display_Handle              hDisplay,
	  BVDC_P_ListInfo                 *pList,
	  BVDC_P_DisplayAnlgChan          *pstChan)
{
	uint32_t	ulSrc = 0;

	/* Connect IT to compositor */
	ulSrc = BVDC_P_GetVecCfgSrc(hDisplay);
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VEC_CFG_IT_0_SOURCE + pstChan->ulIt * 4);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VEC_CFG_IT_0_SOURCE, SOURCE, ulSrc);

	if (!hDisplay->stCurInfo.aulEnableMpaaDeci[BVDC_MpaaDeciIf_eHdmi])
	{
		/* Both component and HDMI paths disable MPAA. So we disable the HW
		 */
		BVDC_P_DisableMpaa(hDisplay, pList);
	}
}

static void BVDC_P_Display_Apply_CompMpaa_Setting
	( BVDC_Display_Handle              hDisplay,
	  BVDC_P_ListInfo                 *pList,
	  BAVC_Polarity                    eFieldPolarity )
{
	BVDC_P_DisplayAnlgChan *pstChan;
	BSTD_UNUSED(eFieldPolarity);

	pstChan = &hDisplay->stAnlgChan_0;

	/* enable/disable info is already coded in eState */
	switch (hDisplay->stMpaaComp.eState)
	{
		/* Enable MPAA */
		case BVDC_P_DisplayResource_eCreate:
			BVDC_P_MpaaComp_Setup_isr(hDisplay, pList, pstChan);
			hDisplay->stMpaaComp.eState = BVDC_P_DisplayResource_eActivating;
			break;
		case BVDC_P_DisplayResource_eActivating:
			if (pList->bLastExecuted)
			{
				hDisplay->stMpaaComp.eState = BVDC_P_DisplayResource_eActive;
				hDisplay->stCurInfo.stDirty.stBits.bMpaaComp = BVDC_P_CLEAN;
			}
			else
			{
				/* Re-build the setup RUL */
				BVDC_P_MpaaComp_Setup_isr(hDisplay, pList, pstChan);
			}
			break;

		/* Disable MPAA */
		case BVDC_P_DisplayResource_eDestroy:
			BVDC_P_MpaaComp_Shutdown_isr(hDisplay, pList, pstChan);
			hDisplay->stMpaaComp.eState = BVDC_P_DisplayResource_eShuttingdown;
			break;

		case BVDC_P_DisplayResource_eShuttingdown:
			if (pList->bLastExecuted)
			{
				if (!hDisplay->stCurInfo.aulEnableMpaaDeci[BVDC_MpaaDeciIf_eComponent])
				{
					BDBG_MSG(("release component mpaa %d", hDisplay->stMpaaComp.ulHwId));
					BVDC_P_Resource_ReleaseHwId_isr(hDisplay->hVdc->hResource,
						BVDC_P_ResourceType_eMpaa, hDisplay->stMpaaComp.ulHwId);
					hDisplay->stMpaaComp.ulHwId = BVDC_P_HW_ID_INVALID;
				}
				hDisplay->stMpaaComp.eState = BVDC_P_DisplayResource_eInactive;
				hDisplay->stCurInfo.stDirty.stBits.bMpaaComp = BVDC_P_CLEAN;
			}
			else
			{
				/* Re-build the shutdown RUL */
				BVDC_P_MpaaComp_Shutdown_isr(hDisplay, pList, pstChan);
			}
			break;

		default:
			hDisplay->stCurInfo.stDirty.stBits.bMpaaComp = BVDC_P_CLEAN;
			break;
	}
}

/**************** HDMI path MPAA setting event hanlders **************/

static BERR_Code BVDC_P_Display_Validate_HdmiMpaa_Setting
	( BVDC_Display_Handle              hDisplay )
{
	BERR_Code err;

	if (hDisplay->stNewInfo.aulEnableMpaaDeci[BVDC_MpaaDeciIf_eHdmi] &&
		BVDC_P_HW_ID_INVALID == hDisplay->stMpaaHdmi.ulHwId)
	{
		/* query only */
		err = BVDC_P_Resource_AcquireHwId_isr(hDisplay->hVdc->hResource,
			BVDC_P_ResourceType_eMpaa, 0, hDisplay->eId, &hDisplay->stMpaaHdmi.ulHwId, true);
		if (BERR_SUCCESS != err)
		{
			return BERR_TRACE(err);
		}
	}

	return BERR_SUCCESS;
}

static void BVDC_P_Display_Copy_HdmiMpaa_Setting
	( BVDC_Display_Handle              hDisplay )
{
	if (hDisplay->stNewInfo.aulEnableMpaaDeci[BVDC_MpaaDeciIf_eHdmi] &&
		hDisplay->stNewInfo.bEnableHdmi)
	{
		if (BVDC_P_RESOURCE_ID_AVAIL == hDisplay->stMpaaHdmi.ulHwId)
		{
			BERR_Code err;

			/* quered, but not acquired yet */
			err = BVDC_P_Resource_AcquireHwId_isr(hDisplay->hVdc->hResource,
				BVDC_P_ResourceType_eMpaa, 0, hDisplay->eId, &hDisplay->stMpaaHdmi.ulHwId, false);
			if (BERR_SUCCESS != err)
			{
				BDBG_ERR(("Display %d fail to acquire hdmi mpaa!", hDisplay->eId));
				return;
			}
			BDBG_MSG(("acquire hdmi mpaa %d", hDisplay->stMpaaHdmi.ulHwId));
		}
		/* init enabling and connect */
		hDisplay->stMpaaHdmi.eState = BVDC_P_DisplayResource_eCreate;
	}
	else if (BVDC_P_DisplayResource_eActivating == hDisplay->stMpaaHdmi.eState ||
			 BVDC_P_DisplayResource_eActive     == hDisplay->stMpaaHdmi.eState)
	{
		/* init shut down process */
		hDisplay->stMpaaHdmi.eState = BVDC_P_DisplayResource_eDestroy;
	}
	else if (!hDisplay->stNewInfo.aulEnableMpaaDeci[BVDC_MpaaDeciIf_eHdmi] &&
			 hDisplay->stCurInfo.aulEnableMpaaDeci[BVDC_MpaaDeciIf_eHdmi] &&
			 BVDC_P_DisplayResource_eDestroy      != hDisplay->stMpaaHdmi.eState &&
			 BVDC_P_DisplayResource_eShuttingdown != hDisplay->stMpaaHdmi.eState)
	{
		if (BVDC_P_HW_ID_INVALID     != hDisplay->stMpaaHdmi.ulHwId &&
			BVDC_P_RESOURCE_ID_AVAIL != hDisplay->stMpaaHdmi.ulHwId)
		{
			BDBG_MSG(("release hdmi mpaa %d", hDisplay->stMpaaHdmi.ulHwId));
			BVDC_P_Resource_ReleaseHwId_isr(hDisplay->hVdc->hResource,
				BVDC_P_ResourceType_eMpaa, hDisplay->stMpaaHdmi.ulHwId);
			hDisplay->stMpaaHdmi.ulHwId = BVDC_P_HW_ID_INVALID;
		}
		hDisplay->stMpaaHdmi.eState = BVDC_P_DisplayResource_eInactive;
	}

	hDisplay->stCurInfo.aulEnableMpaaDeci[BVDC_MpaaDeciIf_eHdmi] =
		hDisplay->stNewInfo.aulEnableMpaaDeci[BVDC_MpaaDeciIf_eHdmi];

	return;
}

static void BVDC_P_MpaaHdmi_Setup_isr
	( BVDC_Display_Handle              hDisplay,
	  BVDC_P_ListInfo                 *pList )
{
	uint32_t ulDviSrc = 0;

	/* inside it will avoid re-do */
	BVDC_P_EnableMpaa(hDisplay, pList);

	/* Connect DVI to DECIM */
	ulDviSrc = BCHP_VEC_CFG_DVI_DTG_0_SOURCE_SOURCE_DECIM_0;
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VEC_CFG_DVI_DTG_0_SOURCE);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VEC_CFG_DVI_DTG_0_SOURCE, SOURCE, ulDviSrc);
}

static void BVDC_P_MpaaHdmi_Shutdown_isr
	( BVDC_Display_Handle              hDisplay,
	  BVDC_P_ListInfo                 *pList )
{
	uint32_t ulDviSrc = 0;

	/* Connect DVI to compositor */
	ulDviSrc = BVDC_P_GetVecCfgSrc(hDisplay);
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VEC_CFG_DVI_DTG_0_SOURCE);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(VEC_CFG_DVI_DTG_0_SOURCE, SOURCE, ulDviSrc);

	if (!hDisplay->stCurInfo.aulEnableMpaaDeci[BVDC_MpaaDeciIf_eComponent])
	{
		/* Both component and hdmi paths disable MPAA. So we disable the HW
		 */
		BVDC_P_DisableMpaa(hDisplay, pList);
	}
}

static void BVDC_P_Display_Apply_HdmiMpaa_Setting
	( BVDC_Display_Handle              hDisplay,
	  BVDC_P_ListInfo                 *pList,
	  BAVC_Polarity                    eFieldPolarity )
{
	BVDC_P_DisplayDviChan *pstChan;
	BSTD_UNUSED(eFieldPolarity);

	pstChan = &hDisplay->stDviChan;

	/* enable/disable info is already coded in eState */
	switch (hDisplay->stMpaaHdmi.eState)
	{
		/* Enable MPAA */
		case BVDC_P_DisplayResource_eCreate:
			BVDC_P_MpaaHdmi_Setup_isr(hDisplay, pList);
			hDisplay->stMpaaHdmi.eState = BVDC_P_DisplayResource_eActivating;
			break;
		case BVDC_P_DisplayResource_eActivating:
			if (pList->bLastExecuted)
			{
				hDisplay->stMpaaHdmi.eState = BVDC_P_DisplayResource_eActive;
				hDisplay->stCurInfo.stDirty.stBits.bMpaaHdmi = BVDC_P_CLEAN;
			}
			else
			{
				/* Re-build the setup RUL */
				BVDC_P_MpaaHdmi_Setup_isr(hDisplay, pList);
			}
			break;

		/* Disable MPAA */
		case BVDC_P_DisplayResource_eDestroy:
			BVDC_P_MpaaHdmi_Shutdown_isr(hDisplay, pList);
			hDisplay->stMpaaHdmi.eState = BVDC_P_DisplayResource_eShuttingdown;
			break;
		case BVDC_P_DisplayResource_eShuttingdown:
			if (pList->bLastExecuted)
			{
				if (!hDisplay->stCurInfo.aulEnableMpaaDeci[BVDC_MpaaDeciIf_eHdmi])
				{
					BDBG_MSG(("release hdmi mpaa %d", hDisplay->stMpaaHdmi.ulHwId));
					BVDC_P_Resource_ReleaseHwId_isr(hDisplay->hVdc->hResource,
						BVDC_P_ResourceType_eMpaa, hDisplay->stMpaaHdmi.ulHwId);
					hDisplay->stMpaaHdmi.ulHwId = BVDC_P_HW_ID_INVALID;
				}
				hDisplay->stMpaaHdmi.eState = BVDC_P_DisplayResource_eInactive;
				hDisplay->stCurInfo.stDirty.stBits.bMpaaHdmi = BVDC_P_CLEAN;
			}
			else
			{
				/* Re-build the shutdown RUL */
				BVDC_P_MpaaHdmi_Shutdown_isr(hDisplay, pList);
			}
			break;
		default:
			hDisplay->stCurInfo.stDirty.stBits.bMpaaHdmi = BVDC_P_CLEAN;
			break;
	}
}

/**************** RFM configuration event hanlders **************/
#if (BVDC_P_SUPPORT_RFM_OUTPUT != 0)

static BERR_Code BVDC_P_Display_Validate_Rfm_Setting
	( BVDC_Display_Handle              hDisplay )
{
	BVDC_P_DisplayInfo *pNewInfo;

	if (!hDisplay->bAnlgEnable)
	{
		BDBG_ERR(("Analog channel of display[%d] is not enabled. No RFM",
			hDisplay->eId));
		return BERR_TRACE(BVDC_ERR_INVALID_RFM_PATH);
	}

	if(hDisplay->stAnlgChan_0.ulSdsrc == BVDC_P_HW_ID_INVALID &&
	   hDisplay->stAnlgChan_1.ulSdsrc == BVDC_P_HW_ID_INVALID)
	{
		BDBG_ERR(("Display[%d] does not acquire SDSRC", hDisplay->eId));
		return BERR_TRACE(BVDC_ERR_INVALID_RFM_PATH);
	}

	pNewInfo = &hDisplay->stNewInfo;

	if(BVDC_RfmOutput_eCVBS == pNewInfo->eRfmOutput)
	{
		if ( VIDEO_FORMAT_IS_HD(pNewInfo->pFmtInfo->eVideoFmt) )
		{
			BDBG_ERR(("Display[%d] video format %d doesn't support RFM CVBS output ",
				hDisplay->eId, pNewInfo->pFmtInfo->eVideoFmt));
			return BERR_TRACE(BVDC_ERR_VIDEOFMT_OUTPUT_MISMATCH);
		}
	}

	return BERR_SUCCESS;
}

static void BVDC_P_Display_Copy_Rfm_Setting
	( BVDC_Display_Handle              hDisplay )
{
	hDisplay->stCurInfo.eRfmOutput = hDisplay->stNewInfo.eRfmOutput;
	hDisplay->stCurInfo.ulRfmConst = hDisplay->stNewInfo.ulRfmConst;

	return;
}

static void BVDC_P_Display_Apply_Rfm_Setting
	( BVDC_Display_Handle              hDisplay,
	  BVDC_P_ListInfo                 *pList,
	  BAVC_Polarity                    eFieldPolarity )
{
	uint32_t ulRfmSel=0, ulRfmSyncSel=0, ulConst=0, ulSdsrcId = BVDC_P_HW_ID_INVALID;

	BSTD_UNUSED(eFieldPolarity);

	if(hDisplay->stCurInfo.eRfmOutput != BVDC_RfmOutput_eCVBS)
	{
		ulRfmSel = BCHP_MISC_RFM_0_CFG_SEL_CONST;
		ulConst = hDisplay->stCurInfo.ulRfmConst;
	}
	else
	{
		ulSdsrcId = (hDisplay->stAnlgChan_0.ulSdsrc != BVDC_P_HW_ID_INVALID) ?
			hDisplay->stAnlgChan_0.ulSdsrc : hDisplay->stAnlgChan_1.ulSdsrc;

		switch(ulSdsrcId)
		{
			case 0:
				ulRfmSel = BCHP_MISC_RFM_0_CFG_SEL_SDSRC_0;
				ulRfmSyncSel = BCHP_MISC_RFM_0_SYNC_0_CFG_SEL_SDSRC_0;
				break;

#if (BVDC_P_NUM_SHARED_SDSRC > 1)
			case 1:
				ulRfmSel = BCHP_MISC_RFM_0_CFG_SEL_SDSRC_1;
				ulRfmSyncSel = BCHP_MISC_RFM_0_SYNC_0_CFG_SEL_SDSRC_1;
				break;

#if (BVDC_P_NUM_SHARED_SDSRC > 2)
			case 2:
				ulRfmSel = BCHP_MISC_RFM_0_CFG_SEL_SDSRC_2;
				ulRfmSyncSel = BCHP_MISC_RFM_0_SYNC_0_CFG_SEL_SDSRC_2;
				break;
#endif
#endif
			default:
				BDBG_ERR(("Display [%d] analog channel acquires no SDSRC sub-block", hDisplay->eId));
				BDBG_ASSERT(0);
		}

		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_MISC_RFM_0_SYNC_0_CFG);
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(MISC_RFM_0_SYNC_0_CFG, SEL, ulRfmSyncSel);
	}

	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_MISC_RFM_0_CFG );
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(MISC_RFM_0_CFG, SEL, ulRfmSel) |
		BCHP_FIELD_DATA(MISC_RFM_0_CFG, CONST, ulConst);

	hDisplay->stCurInfo.stDirty.stBits.bRfm = BVDC_P_CLEAN;

	return;
}
#endif

/**************** HDMI output enable/disable event handlers **************/
static BERR_Code BVDC_P_Display_Validate_Hdmi_Setting
	( BVDC_Display_Handle              hDisplay )
{
	BVDC_P_DisplayInfo *pNewInfo;
	BERR_Code err;
	pNewInfo = &hDisplay->stNewInfo;

	/* By checking "stDviChan.bEnable" in BVDC_P_Display_Validate_VideoFormat()
	 * and checking "pNewInfo->bEnableHdmi" here guarantees this validation won't be done
	 * two times.
	 */
	if (pNewInfo->bEnableHdmi)
	{
		if ((err = BVDC_P_Display_Validate_DviChan(hDisplay)) != BERR_SUCCESS)
			return BERR_TRACE(err);

		if((err = BVDC_P_AllocDviChanResources(
					hDisplay->hVdc->hResource,
					hDisplay->eId, &hDisplay->stDviChan,
					hDisplay->hCompositor->eId)) != BERR_SUCCESS)
			return BERR_TRACE(err);
	}

	return BERR_SUCCESS;
}

/**************** HDMI output CSC matrix event handlers **************/
static void BVDC_P_Display_Copy_HdmiCsc_Setting
	( BVDC_Display_Handle              hDisplay )
{
	int i;

	hDisplay->stCurInfo.eHdmiOutput = hDisplay->stNewInfo.eHdmiOutput;
	hDisplay->stCurInfo.stDvoCfg = hDisplay->stNewInfo.stDvoCfg;

	hDisplay->stCurInfo.lDvoAttenuationR = hDisplay->stNewInfo.lDvoAttenuationR;
	hDisplay->stCurInfo.lDvoAttenuationG = hDisplay->stNewInfo.lDvoAttenuationG;
	hDisplay->stCurInfo.lDvoAttenuationB = hDisplay->stNewInfo.lDvoAttenuationB;
	hDisplay->stCurInfo.lDvoOffsetR = hDisplay->stNewInfo.lDvoOffsetR;
	hDisplay->stCurInfo.lDvoOffsetG = hDisplay->stNewInfo.lDvoOffsetG;
	hDisplay->stCurInfo.lDvoOffsetB = hDisplay->stNewInfo.lDvoOffsetB;

	hDisplay->stCurInfo.bUserCsc = hDisplay->stNewInfo.bUserCsc;
	for (i = 0; i < BVDC_CSC_COEFF_COUNT; i++)
	{
		hDisplay->stCurInfo.pl32_Matrix[i] = hDisplay->stNewInfo.pl32_Matrix[i];
	}

	return;
}

static void BVDC_P_Display_Apply_HdmiCsc_Setting
	( BVDC_Display_Handle              hDisplay,
	  BVDC_P_ListInfo                 *pList,
	  BAVC_Polarity                    eFieldPolarity )

{
	BSTD_UNUSED(eFieldPolarity);

	BVDC_P_Vec_Build_DVI_CSC_isr(hDisplay, pList);

	hDisplay->stCurInfo.stDirty.stBits.bHdmiCsc = BVDC_P_CLEAN;

	return;
}

static void BVDC_P_Display_Copy_Hdmi_Setting
	( BVDC_Display_Handle              hDisplay )
{
	int i;

	hDisplay->stCurInfo.bEnableHdmi = hDisplay->stNewInfo.bEnableHdmi;

	if (hDisplay->stCurInfo.bEnableHdmi)
	{

#if BVDC_P_SUPPORT_SEAMLESS_ATTACH
		/* Reset the DVI slave attachment process state */
		hDisplay->eDviSlaveState = BVDC_P_Slave_eInactive;
#else
		hDisplay->eDviState = BVDC_P_DisplayResource_eCreate;
#endif
	}
	else
	{
		/* Tear down the DVI channel */
		hDisplay->eDviState = BVDC_P_DisplayResource_eDestroy;
	}

	BVDC_P_Display_Copy_HdmiCsc_Setting(hDisplay);

	hDisplay->stCurInfo.bUserCsc = hDisplay->stNewInfo.bUserCsc;
	for (i = 0; i < BVDC_CSC_COEFF_COUNT; i++)
	{
		hDisplay->stCurInfo.pl32_Matrix[i] = hDisplay->stNewInfo.pl32_Matrix[i];
	}

	return;
}

static void BVDC_P_Display_Apply_Hdmi_Setting
	( BVDC_Display_Handle              hDisplay,
	  BVDC_P_ListInfo                 *pList,
	  BAVC_Polarity                    eFieldPolarity )

{
	BVDC_P_DisplayInfo *pCurInfo;
	BVDC_P_DisplayDviChan *pstChan;

	BSTD_UNUSED(eFieldPolarity);

	pCurInfo = &hDisplay->stCurInfo;
	pstChan = &hDisplay->stDviChan;

	if (pCurInfo->bEnableHdmi)
	{
#if BVDC_P_SUPPORT_SEAMLESS_ATTACH
		switch (hDisplay->eDviSlaveState)
		{
			case BVDC_P_Slave_eInactive:
				/* Acquire DVI core */
				if (BVDC_P_AllocDviChanResources(hDisplay->hVdc->hResource,
					  hDisplay->eId, pstChan, hDisplay->hCompositor->eId))
				{
					BDBG_ERR(("No DVI available for display [%]", hDisplay->eId));
					return;
				}

				BVDC_P_SetupDviChan(hDisplay, pstChan, pList);
				BVDC_P_ProgramDviChan(hDisplay, pstChan, pList);

				/* Enable micro-controller */
				BVDC_P_Display_StartDviCtrler(hDisplay, pList);
				hDisplay->eDviSlaveState = BVDC_P_Slave_eEnable;
				break;

			case BVDC_P_Slave_eEnable:
				BVDC_P_ConnectDviSrc(hDisplay, pstChan, pList);
				hDisplay->eDviSlaveState = BVDC_P_Slave_eConnectSrc;
				break;

			case BVDC_P_Slave_eConnectSrc:
			default:
				hDisplay->eDviSlaveState = BVDC_P_Slave_eAttached;
				hDisplay->stCurInfo.stDirty.stBits.bHdmiEnable = BVDC_P_CLEAN;
				break;
		}
#else
		/* When DVI is in slave mode, a format switch has to be done. Even
		 * if the RUL somehow didn't get exeucted, our re-do mechanism will keep
		 * trying. So it is safe to move the state to eActive directly. */
		hDisplay->eDviState = BVDC_P_DisplayResource_eActive;
		hDisplay->stCurInfo.stDirty.stBits.bHdmiEnable = BVDC_P_CLEAN;
#endif
	}
	else
	{
		/* Reset DVI core, disconnect source.
		 * If DVI is the master timing generator, will this automatically
		 * shut down all the slave paths?
		 */
		switch (hDisplay->eDviState)
		{
			case BVDC_P_DisplayResource_eDestroy:
				BVDC_P_TearDownDviChan(hDisplay, pstChan, pList);
				hDisplay->eDviState = BVDC_P_DisplayResource_eShuttingdown;
				break;

			case BVDC_P_DisplayResource_eShuttingdown:
				if (pList->bLastExecuted)
				{
					if(!hDisplay->stDviChan.bEnable)
						BVDC_P_FreeDviChanResources(hDisplay->hVdc->hResource, pstChan);
					hDisplay->eDviState = BVDC_P_DisplayResource_eInactive;
					hDisplay->stCurInfo.stDirty.stBits.bHdmiEnable = BVDC_P_CLEAN;
				}
				else
				{
					/* Re-build the teardown RUL */
					BVDC_P_TearDownDviChan(hDisplay, pstChan, pList);
				}
				break;

			default:
				hDisplay->stCurInfo.stDirty.stBits.bHdmiEnable = BVDC_P_CLEAN;
				break;
		}
	}

	return;
}

/**************** 656 output enable/disable event handlers **************/
#if (BVDC_P_SUPPORT_ITU656_OUT != 0)
static BERR_Code BVDC_P_Display_Validate_656_Setting
	( BVDC_Display_Handle              hDisplay )
{
	BVDC_P_DisplayInfo *pNewInfo;
	BERR_Code err;
	pNewInfo = &hDisplay->stNewInfo;

	/* By checking "st656Chan.bEnable" in BVDC_P_Display_Validate_VideoFormat()
	 * and checking "pNewInfo->bEnable656" here guarantees this validation won't be done
	 * two times.
	 */
	if (pNewInfo->bEnable656)
	{
		if ((err = BVDC_P_Display_Validate_656Chan(hDisplay)) != BERR_SUCCESS)
			return BERR_TRACE(err);

		if ((err = BVDC_P_Alloc656ChanResources(hDisplay->hVdc->hResource,
			  hDisplay->eId, &hDisplay->st656Chan, hDisplay->hCompositor->eId)) != BERR_SUCCESS)
			return BERR_TRACE(err);
	}

#if !BVDC_P_SUPPORT_SEAMLESS_ATTACH
	hDisplay->st656Chan.bEnable = pNewInfo->bEnable656;
#endif

	return BERR_SUCCESS;
}


static void BVDC_P_Display_Copy_656_Setting
	( BVDC_Display_Handle              hDisplay )
{
	hDisplay->stCurInfo.bEnable656 = hDisplay->stNewInfo.bEnable656;

	if (hDisplay->stCurInfo.bEnable656)
	{
#if BVDC_P_SUPPORT_SEAMLESS_ATTACH
		/* Reset the 656 slave attachment process state */
		hDisplay->e656SlaveState = BVDC_P_Slave_eInactive;
#else
		hDisplay->e656State = BVDC_P_DisplayResource_eCreate;
#endif
	}
	else
	{
		/* Tear down the 656 channel */
		hDisplay->e656State = BVDC_P_DisplayResource_eDestroy;
	}
}

static void BVDC_P_Display_Apply_656_Setting
	( BVDC_Display_Handle              hDisplay,
	  BVDC_P_ListInfo                 *pList,
	  BAVC_Polarity                    eFieldPolarity )

{
	BVDC_P_DisplayInfo *pCurInfo;
	BVDC_P_Display656Chan *pstChan;

	BSTD_UNUSED(eFieldPolarity);

	pCurInfo = &hDisplay->stCurInfo;
	pstChan = &hDisplay->st656Chan;

	if (pCurInfo->bEnable656)
	{
#if BVDC_P_SUPPORT_SEAMLESS_ATTACH
		switch (hDisplay->e656SlaveState)
		{
			case BVDC_P_Slave_eInactive:
				/* Acquire 656 core */
				if (BVDC_P_Alloc656ChanResources(hDisplay->hVdc->hResource,
					  hDisplay->eId, pstChan, hDisplay->hCompositor->eId))
				{
					BDBG_ERR(("No 656 available "));
					return;
				}

				hDisplay->st656Chan.bEnable = true;

				BVDC_P_Setup656Chan(hDisplay, pstChan, pList);
				BVDC_P_Program656Chan(hDisplay, pstChan, pList);

				/* Enable micro-controller */
				BVDC_P_Display_Start656Ctrler(hDisplay, pList);

				hDisplay->e656SlaveState = BVDC_P_Slave_eEnable;
				break;

			case BVDC_P_Slave_eEnable:
				BVDC_P_Connect656Src(hDisplay, pstChan, pList);
				hDisplay->e656SlaveState = BVDC_P_Slave_eConnectSrc;
				break;

			case BVDC_P_Slave_eConnectSrc:
			default:
				hDisplay->e656SlaveState = BVDC_P_Slave_eAttached;
				hDisplay->stCurInfo.stDirty.stBits.b656Enable = BVDC_P_CLEAN;
				break;
		}
#else
		/* When 656 is in slave mode, a format switch has to be done. Even
		 * if the RUL somehow didn't get exeucted, our re-do mechanism will keep
		 * trying. So it is safe to move the state to eActive directly.
		 */
		hDisplay->e656State = BVDC_P_DisplayResource_eActive;
		hDisplay->stCurInfo.stDirty.stBits.b656Enable = BVDC_P_CLEAN;
#endif
	}
	else
	{
		/* Reset 656 core, disconnect source.
		 * If 656 is the master timing generator, will this automatically
		 * shut down all the slave paths?
		 */
		switch (hDisplay->e656State)
		{

			case BVDC_P_DisplayResource_eDestroy:
				BVDC_P_TearDown656Chan(hDisplay, pstChan, pList);
				hDisplay->st656Chan.bEnable = false;
				hDisplay->e656State = BVDC_P_DisplayResource_eShuttingdown;
				break;

			case BVDC_P_DisplayResource_eShuttingdown:
				if (pList->bLastExecuted)
				{
					BVDC_P_Free656ChanResources(hDisplay->hVdc->hResource, pstChan);
					hDisplay->e656State = BVDC_P_DisplayResource_eInactive;
					hDisplay->stCurInfo.stDirty.stBits.b656Enable = BVDC_P_CLEAN;
				}
				else
				{
					/* Re-build the teardown RUL */
					BVDC_P_TearDown656Chan(hDisplay, pstChan, pList);
				}
				break;

			default:
				hDisplay->stCurInfo.stDirty.stBits.b656Enable = BVDC_P_CLEAN;
				break;
		}
	}

	return;
}
#endif

/**************** aspect ratio event handlers **************/

static BERR_Code BVDC_P_Display_Validate_AspRatio_Setting
	( BVDC_Display_Handle              hDisplay )
{
	BVDC_P_ClipRect *pAspRatRectClip;
	BVDC_P_DisplayInfo *pNewInfo = &hDisplay->stNewInfo;

	/* aspect ratio canvas clip validation */
	pAspRatRectClip = &(pNewInfo->stAspRatRectClip);
	if ((pAspRatRectClip->ulLeft + pAspRatRectClip->ulRight >= pNewInfo->pFmtInfo->ulWidth) ||
		(pAspRatRectClip->ulTop + pAspRatRectClip->ulBottom >= pNewInfo->pFmtInfo->ulHeight))
	{
		return BERR_TRACE(BVDC_ERR_INVALID_DISP_ASPECT_RATIO_RECT);
	}
	else
	{
		return BERR_SUCCESS;
	}
}

static void BVDC_P_Display_Copy_AspRatio_Setting
	( BVDC_Display_Handle              hDisplay )
{
	bool bUseDigTrg = BVDC_P_DISPLAY_USED_DIGTRIG(hDisplay->eMasterTg);
	hDisplay->stCurInfo.eAspectRatio = hDisplay->stNewInfo.eAspectRatio;
	hDisplay->stCurInfo.uiSampleAspectRatioX = hDisplay->stNewInfo.uiSampleAspectRatioX;
	hDisplay->stCurInfo.uiSampleAspectRatioY = hDisplay->stNewInfo.uiSampleAspectRatioY;
	hDisplay->stCurInfo.stAspRatRectClip = hDisplay->stNewInfo.stAspRatRectClip;

	BVDC_P_CalcuPixelAspectRatio_isr(
		hDisplay->stCurInfo.eAspectRatio,
		hDisplay->stCurInfo.uiSampleAspectRatioX,
		hDisplay->stCurInfo.uiSampleAspectRatioY,
		bUseDigTrg?hDisplay->stCurInfo.pFmtInfo->ulDigitalWidth : hDisplay->stCurInfo.pFmtInfo->ulWidth,
		bUseDigTrg?hDisplay->stCurInfo.pFmtInfo->ulDigitalHeight: hDisplay->stCurInfo.pFmtInfo->ulHeight,
		&hDisplay->stCurInfo.stAspRatRectClip,
		&hDisplay->ulPxlAspRatio,
		&hDisplay->ulPxlAspRatio_x_y);

#if (BVDC_P_SUPPORT_3D_VIDEO)
	if(hDisplay->stCurInfo.eOrientation == BFMT_Orientation_e3D_OverUnder)
	{
		hDisplay->ulPxlAspRatio = hDisplay->ulPxlAspRatio / 2;
	}
	else if(hDisplay->stCurInfo.eOrientation == BFMT_Orientation_e3D_LeftRight)
	{
		hDisplay->ulPxlAspRatio = hDisplay->ulPxlAspRatio * 2;
	}
#endif

	hDisplay->hCompositor->ulStgPxlAspRatio_x_y = hDisplay->ulPxlAspRatio_x_y;
	/* inform window ApplyChanges  */
	hDisplay->hCompositor->bDspAspRatDirty = true;

	return;
}

static void BVDC_P_Display_Apply_AspRatio_Setting
	( BVDC_Display_Handle              hDisplay,
	  BVDC_P_ListInfo                 *pList,
	  BAVC_Polarity                    eFieldPolarity )

{
	BSTD_UNUSED(hDisplay);
	BSTD_UNUSED(pList);
	BSTD_UNUSED(eFieldPolarity);

	/* nothing to do here */
	hDisplay->stCurInfo.stDirty.stBits.bAspRatio = BVDC_P_CLEAN;
	return;
}

/**************** Take a snapshot of timer event handlers **************/
static  BERR_Code BVDC_P_Display_CalculateTimeGap
	( BVDC_Display_Handle              hDisplay,
	  BVDC_P_DisplayContext           *pTargetDisplay,
	  int32_t                         *plDeltaTs )
{
	int32_t lDeltaTs;
	uint32_t ulVsyncInterval =
		BVDC_P_DISPLAY_ONE_VSYNC_INTERVAL(hDisplay->stCurInfo.pFmtInfo->ulVertFreq);
	uint32_t ulTs, ulTargetTs, ulDecelTime=0, ulAccelTime=0, ulAdjustTsCnt = 0;

	ulTs = BREG_Read32_isr(hDisplay->hVdc->hRegister, hDisplay->ulScratchTsAddr);
	ulTargetTs = BREG_Read32_isr(pTargetDisplay->hVdc->hRegister, pTargetDisplay->ulScratchTsAddr);

	/* The delta between the two displays should be within one vsync.
	 * Otherwise the timestamp is invalid.
	 */
	if (((ulTs < ulTargetTs) && ((ulTs + ulVsyncInterval) < ulTargetTs)) ||
		((ulTs > ulTargetTs) && (ulTs > (ulVsyncInterval + ulTargetTs))))
	{
		/* This could be caused by execution of the RUL that samples timestamp
		 * was delayed or timer happened to wraparound between the two samples.
		 *
		 */
		BDBG_MSG(("Invalid timestamp value: display[%d] %d, target display[%d] %d",
					hDisplay->eId, ulTs, pTargetDisplay->eId, ulTargetTs));
		return BERR_TRACE(BVDC_ERR_INVALID_TIMESTAMP);
	}

	/* Adjust time stamp based on the alignment pattern */
	ulAdjustTsCnt = (hDisplay->stCurInfo.ulVertFreq == 2397 && pTargetDisplay->stCurInfo.ulVertFreq==5994) ?
					1 : 0;
	ulTs += ulAdjustTsCnt * ulVsyncInterval;

	if(BVDC_AlignmentDirection_eAuto == hDisplay->stCurInfo.stAlignCfg.eDirection)
	{
		/* interlaced/interlaced case; */
		if(hDisplay->stCurInfo.pFmtInfo->bInterlaced &&
			pTargetDisplay->stCurInfo.pFmtInfo->bInterlaced)
		{
			/* same polarity */
			if (hDisplay->eTimeStampPolarity == pTargetDisplay->eTimeStampPolarity)
			{
				lDeltaTs = (int32_t)ulTs - (int32_t)ulTargetTs;
			}
			else /* opposite polarity */
			{
				/* Acceleration or deceleration will take the same amount of time.
				 * The goal is to match polarity as well after adjustment.
				 */
				lDeltaTs = (int32_t)ulVsyncInterval + (int32_t)ulTs - (int32_t)ulTargetTs;
			}
		}
		/* progressive/interlaced case */
		else
		{
			/* We can either accelaerate or decelerate. The more efficient approach
			 * will be chosen.
			 */
			if (ulTs < ulTargetTs)
			{
				ulDecelTime	= ulTargetTs - ulTs;
				ulAccelTime = (ulVsyncInterval/2 + ulTs - ulTargetTs);
			}
			else
			{
				ulAccelTime	= ulTs - ulTargetTs;
				ulDecelTime = (ulVsyncInterval/2 + ulTargetTs - ulTs);
			}

			lDeltaTs = (ulDecelTime < ulAccelTime) ? ((int32_t)0 - (int32_t) ulDecelTime) : (int32_t)ulAccelTime;
		}
	}
	else /* manual mode */
	{
		/* interlaced/interlaced case; */
		if(hDisplay->stCurInfo.pFmtInfo->bInterlaced &&
			pTargetDisplay->stCurInfo.pFmtInfo->bInterlaced)
		{
			if (ulTs < ulTargetTs)
			{
				/* same polarity */
				if (hDisplay->eTimeStampPolarity == pTargetDisplay->eTimeStampPolarity)
				{
					ulAccelTime = 2 * ulVsyncInterval + ulTs - ulTargetTs;
					ulDecelTime = ulTargetTs - ulTs;
				}
				else /* opposite polarity */
				{
					ulAccelTime = ulVsyncInterval + ulTs - ulTargetTs;
					ulDecelTime = ulVsyncInterval + ulTargetTs - ulTs;
				}
			}
		}
		/* progressive/interlaced case */
		else
		{
			if (ulTs < ulTargetTs)
			{
				ulAccelTime = ulVsyncInterval + ulTs - ulTargetTs;
				ulDecelTime = ulTargetTs - ulTs;
			}
			else
			{
				ulAccelTime = ulTargetTs - ulTs;
				ulDecelTime = ulVsyncInterval + ulTargetTs - ulTs;
			}
		}

		lDeltaTs = (hDisplay->stCurInfo.stAlignCfg.eDirection == BVDC_AlignmentDirection_eFaster) ?
					(int32_t) ulAccelTime : ((int32_t)0 - (int32_t) ulDecelTime);
	}

	*plDeltaTs = lDeltaTs;

	return BERR_SUCCESS;
}

static void BVDC_P_Display_StartAlignAdjustment
	( BVDC_Display_Handle              hDisplay,
	  BVDC_P_ListInfo                 *pList,
	  int32_t                          lDeltaTs )
{
	uint32_t ulCount;
	bool bSkip;

	/* Timebase pulse is 27MHz which is the same as the timer frequency.
	 * We can directly use the time stamp delta value as the repeat-skip
	 * count.
	 */
	ulCount = ABS(lDeltaTs);
	BDBG_MSG(("About to start alignment adjustment, delta %d,  %d us", lDeltaTs, ulCount/27));

	bSkip = (lDeltaTs < 0) ? true : false;

	if(hDisplay->bAnlgEnable)
	{
		/* RM pairs with IT */
		uint32_t ulOffset = hDisplay->stAnlgChan_0.ulIt * BVDC_P_VEC_RM_SIZE;

		/* Change Tracking_Range to 15625 ppm */
		BVDC_P_RD_MOD_WR_RUL(pList->pulCurrent,
			~(BCHP_MASK(RM_0_CONTROL, TRACKING_RANGE)),
			  BCHP_FIELD_DATA(RM_0_CONTROL, TRACKING_RANGE, 1),
			  BCHP_RM_0_CONTROL + ulOffset);

		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_RM_0_SKIP_REPEAT_GAP + ulOffset);
		*pList->pulCurrent++ =
		    BCHP_FIELD_DATA(RM_0_SKIP_REPEAT_GAP, GAP_COUNT, BVDC_P_DISPLAY_SKIP_REPEAT_GAP);

		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_RM_0_SKIP_REPEAT_NUMBER + ulOffset);
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(RM_0_SKIP_REPEAT_NUMBER, COUNT, ulCount);

		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_RM_0_SKIP_REPEAT_CONTROL + ulOffset);
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(RM_0_SKIP_REPEAT_CONTROL, MODE, bSkip ? 1 : 0) |
			BCHP_FIELD_DATA(RM_0_SKIP_REPEAT_CONTROL, ENABLE, 1);
	}

	if(hDisplay->stDviChan.bEnable)
	{
		/* Change Tracking_Range to 15625 ppm */
		BVDC_P_RD_MOD_WR_RUL(pList->pulCurrent,
					 ~(BCHP_MASK(HDMI_RM_CONTROL, TRACKING_RANGE)),
					 BCHP_FIELD_DATA(HDMI_RM_CONTROL, TRACKING_RANGE, 1),
					 BCHP_HDMI_RM_CONTROL);

		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HDMI_RM_SKIP_REPEAT_GAP);
		*pList->pulCurrent++ =
		    BCHP_FIELD_DATA(HDMI_RM_SKIP_REPEAT_GAP, GAP_COUNT, BVDC_P_DISPLAY_SKIP_REPEAT_GAP);

		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HDMI_RM_SKIP_REPEAT_NUMBER);
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(HDMI_RM_SKIP_REPEAT_NUMBER, COUNT, ulCount);

		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HDMI_RM_SKIP_REPEAT_CONTROL);
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(HDMI_RM_SKIP_REPEAT_CONTROL, MODE, bSkip ? 1 : 0) |
			BCHP_FIELD_DATA(HDMI_RM_SKIP_REPEAT_CONTROL, ENABLE, 1);
	}

	return;
}

static void BVDC_P_Display_StopAlignAdjustment
	( BVDC_Display_Handle              hDisplay,
	  BVDC_P_ListInfo                 *pList )
{
	BDBG_MSG(("Alignment is done "));

	if(hDisplay->bAnlgEnable)
	{
		/* RM pairs with IT */
		uint32_t ulOffset = hDisplay->stAnlgChan_0.ulIt * BVDC_P_VEC_RM_SIZE;

		/* Change Tracking_Range to 244 ppm */
		BVDC_P_RD_MOD_WR_RUL(pList->pulCurrent,
					 ~(BCHP_MASK(RM_0_CONTROL, TRACKING_RANGE)),
					 BCHP_FIELD_DATA(RM_0_CONTROL, TRACKING_RANGE, 0),
					 BCHP_RM_0_CONTROL + ulOffset);

		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_RM_0_SKIP_REPEAT_CONTROL + ulOffset);
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(RM_0_SKIP_REPEAT_CONTROL, ENABLE, 0);
	}

	if(hDisplay->stDviChan.bEnable)
	{
		/* Change Tracking_Range to 244 ppm */
		BVDC_P_RD_MOD_WR_RUL(pList->pulCurrent,
					 ~(BCHP_MASK(HDMI_RM_CONTROL, TRACKING_RANGE)),
					 BCHP_FIELD_DATA(HDMI_RM_CONTROL, TRACKING_RANGE, 0),
					 BCHP_HDMI_RM_CONTROL);

		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_HDMI_RM_SKIP_REPEAT_CONTROL);
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(HDMI_RM_SKIP_REPEAT_CONTROL, ENABLE, 0);
	}
}


void BVDC_P_Display_Alignment
	( BVDC_Display_Handle              hDisplay,
	  BVDC_P_ListInfo                 *pList )
{
	int32_t lDeltaTs = 0;
	BVDC_Display_Handle hTargetDisplay;
	uint32_t ulOffset = 0, ulCount = 0;

	switch(hDisplay->eAlignmentState)
	{
		case BVDC_P_Alignment_eInactive:
			break;

		case BVDC_P_Alignment_eWaitTimeStamp:
			if ((hDisplay->eTimeStampState == BVDC_P_TimeStamp_eAvail) &&
				(hDisplay->stCurInfo.hTargetDisplay->eTimeStampState == BVDC_P_TimeStamp_eAvail))
			{
				hTargetDisplay = hDisplay->stCurInfo.hTargetDisplay;
				BDBG_OBJECT_ASSERT(hTargetDisplay, BVDC_DSP);
				if (BVDC_P_Display_CalculateTimeGap(hDisplay, hTargetDisplay, &lDeltaTs))
				{
					/* Invalid timestamp. We have to start over again.
					 * This dirty bit will be picked up at next Vysnc.
					 */
					hDisplay->stCurInfo.stDirty.stBits.bAlignment = BVDC_P_DIRTY;
				}
				else
				{
					BDBG_MSG(("Display[%d] and TargetDisplay[%d] time gap %d, %d us",
						hDisplay->eId, hTargetDisplay->eId, lDeltaTs, ABS(lDeltaTs)/27));

					if (ABS(lDeltaTs) < BVDC_P_DISPLAY_ALIGNMENT_THRESHOLD)
					{
						/* no adjustment needed */
						BDBG_MSG(("Display[%d] and TargetDisplay[%d] time gap %d within the threshold",
									hDisplay->eId, hTargetDisplay->eId, lDeltaTs));
						hDisplay->eAlignmentState = BVDC_P_Alignment_eDone;
					}
					else
					{
						/* Start the adjustment process */
						BVDC_P_Display_StartAlignAdjustment(hDisplay, pList, lDeltaTs);
						hDisplay->eAlignmentState = BVDC_P_Alignment_eActive;
						hDisplay->bAlignAdjusting = true;
					}
				}
			}
			break;

		case BVDC_P_Alignment_eActive:
			/* Poll SKIP_REPEAT_NUM to see if the adjustment finishes.
			 */
			switch (hDisplay->eMasterTg)
			{
				case BVDC_DisplayTg_ePrimIt:
				case BVDC_DisplayTg_eSecIt:
				case BVDC_DisplayTg_eTertIt:
					/* RM pairs with IT */
					ulOffset = hDisplay->stAnlgChan_0.ulIt * BVDC_P_VEC_RM_SIZE;
					ulCount = BREG_Read32_isr(hDisplay->hVdc->hRegister,
						BCHP_RM_0_SKIP_REPEAT_NUMBER + ulOffset) &
						BCHP_RM_0_SKIP_REPEAT_NUMBER_COUNT_MASK;
					break;

				case BVDC_DisplayTg_eDviDtg:
					ulCount = BREG_Read32_isr(hDisplay->hVdc->hRegister,
						BCHP_HDMI_RM_SKIP_REPEAT_NUMBER) &
						BCHP_HDMI_RM_SKIP_REPEAT_NUMBER_COUNT_MASK;
					break;

				default:
					/* This feature only exists on IT and DVI */
					break;
			}

			if (ulCount == 0)
			{
				BVDC_P_Display_StopAlignAdjustment(hDisplay, pList);
				hDisplay->eAlignmentState = BVDC_P_Alignment_eDone;
				hDisplay->bAlignAdjusting = false;
			}
			break;

		case BVDC_P_Alignment_eDone:
			/* A BVDC_P_Alignment_eDone state is created so that
			 * we can add watch-dog later to maintain the alignment
			 * even after the adjustment is done.
			 */
		default:
			break;
	}

	return;
}

static void BVDC_P_Display_Apply_TimeStamp_Setting
	( BVDC_Display_Handle              hDisplay,
	  BVDC_P_ListInfo                 *pList,
	  BAVC_Polarity                    eFieldPolarity )
{
	switch (hDisplay->eTimeStampState )
	{
		case BVDC_P_TimeStamp_eStart:
			BVDC_P_Display_SnapshotTimer(hDisplay, pList);
			hDisplay->eTimeStampPolarity = eFieldPolarity;
			hDisplay->eTimeStampState = BVDC_P_TimeStamp_eRul;
			break;

		case BVDC_P_TimeStamp_eRul:
			hDisplay->eTimeStampState = BVDC_P_TimeStamp_eAvail;
			hDisplay->stCurInfo.stDirty.stBits.bTimeStamp = BVDC_P_CLEAN;
			break;

		case BVDC_P_TimeStamp_eAvail:
		default:
			hDisplay->stCurInfo.stDirty.stBits.bTimeStamp = BVDC_P_CLEAN;
			break;
	}

	return;
}

/**************** Display alignment event handlers **************/
static BERR_Code BVDC_P_Display_Validate_Alignment_Setting
	( BVDC_Display_Handle              hDisplay )
{

	BVDC_Display_Handle hTargetDisplay = hDisplay->stNewInfo.hTargetDisplay;


	if (hTargetDisplay && ((hTargetDisplay->eAlignmentState == BVDC_P_Alignment_eActive) ||
		(hTargetDisplay->eItState != BVDC_P_ItState_eActive)))
	{
		BDBG_ERR(("Display %d is not a valid alignment target: alignment state %d, IT state %d",
			hTargetDisplay->eId, hTargetDisplay->eAlignmentState, hTargetDisplay->eItState));

		return BERR_TRACE(BVDC_ERR_INVALID_TARGET_DISPLAY);
	}

	/* Check alignment chain */
	if(hTargetDisplay && hTargetDisplay->stNewInfo.hTargetDisplay)
	{
		BDBG_ERR(("Chained display alignment is not allowed! Display%d->Display%d->Display%d",
			hDisplay->eId, hTargetDisplay->eId,
			hTargetDisplay->stNewInfo.hTargetDisplay->eId));
		return BERR_TRACE(BVDC_ERR_INVALID_MODE_PATH);
	}

	/* check timebase */
	if(hTargetDisplay && hDisplay->stNewInfo.eTimeBase != hTargetDisplay->stNewInfo.eTimeBase)
	{
		BDBG_ERR(("Bad alignment config: Display%d's timebase(%d) != target display%d's timebase(%d)!",
			hDisplay->eId, hDisplay->stNewInfo.eTimeBase,
			hTargetDisplay->eId, hTargetDisplay->stNewInfo.eTimeBase));
		return BERR_TRACE(BVDC_ERR_INVALID_MODE_PATH);
	}

	/* check default display frame rate */
	if(hTargetDisplay &&
	   ((hDisplay->hVdc->stSettings.eDisplayFrameRate == BAVC_FrameRateCode_e60) ||
	    (hDisplay->hVdc->stSettings.eDisplayFrameRate == BAVC_FrameRateCode_e30) ||
	    (hDisplay->hVdc->stSettings.eDisplayFrameRate == BAVC_FrameRateCode_e24)))
	{
		BDBG_ERR(("Bad VDC default display rate config for VEC locking: %d!",
			hDisplay->hVdc->stSettings.eDisplayFrameRate));
		return BERR_TRACE(BVDC_ERR_INVALID_FRAMERATE_USE);
	}

	/* currently supported vertical frequency pair for alignment */
	if (hTargetDisplay &&
		(!((hDisplay->stNewInfo.ulVertFreq == hTargetDisplay->stNewInfo.ulVertFreq) ||
		 (hDisplay->stNewInfo.ulVertFreq == 2997 && hTargetDisplay->stNewInfo.ulVertFreq == 5994) ||
		 (hDisplay->stNewInfo.ulVertFreq == 2397 && hTargetDisplay->stNewInfo.ulVertFreq == 5994) ||
		 (hDisplay->stNewInfo.ulVertFreq == 2500 && hTargetDisplay->stNewInfo.ulVertFreq == 5000))))
	{
		BDBG_ERR(("Vertical frequency not supported for alignment. Display%d %d, Target display%d %d",
			hDisplay->eId, hDisplay->stNewInfo.ulVertFreq,
			hTargetDisplay->eId, hTargetDisplay->stNewInfo.ulVertFreq));
		return BERR_TRACE(BVDC_ERR_INVALID_FRAMERATE_USE);
	}

	return BERR_SUCCESS;
}

static void BVDC_P_Display_Copy_Alignment_Setting
	( BVDC_Display_Handle              hDisplay )
{

	hDisplay->stCurInfo.hTargetDisplay = hDisplay->stNewInfo.hTargetDisplay;
	hDisplay->stCurInfo.stAlignCfg     = hDisplay->stNewInfo.stAlignCfg;

	return;
}

static void BVDC_P_Display_Apply_Alignment_Setting
	( BVDC_Display_Handle              hDisplay,
	  BVDC_P_ListInfo                 *pList,
	  BAVC_Polarity                    eFieldPolarity )

{
	BVDC_P_DisplayInfo *pCurInfo;

	pCurInfo = &hDisplay->stCurInfo;

	if(pCurInfo->hTargetDisplay)
	{
		/* Set target display bTimeStamp dirty bit so that
		 * it can build RUL to take a snapshot of timer.
		 */
		pCurInfo->hTargetDisplay->eTimeStampState = BVDC_P_TimeStamp_eStart;
		pCurInfo->hTargetDisplay->stCurInfo.stDirty.stBits.bTimeStamp = BVDC_P_DIRTY;

		/*
		 * The display itself builds RUL to take a snapshot of timer clock as well.
		 */
		BVDC_P_Display_SnapshotTimer(hDisplay, pList);
		hDisplay->eTimeStampPolarity = eFieldPolarity;
		hDisplay->eTimeStampState = BVDC_P_TimeStamp_eRul;
		hDisplay->eAlignmentState = BVDC_P_Alignment_eWaitTimeStamp;
		pCurInfo->stDirty.stBits.bTimeStamp = BVDC_P_DIRTY;
	}
	else
	{
		/* Disable alignment process */
		if ((hDisplay->eAlignmentState == BVDC_P_Alignment_eWaitTimeStamp) ||
			(hDisplay->eAlignmentState == BVDC_P_Alignment_eActive))
		{
			BVDC_P_Display_StopAlignAdjustment(hDisplay, pList);
		}

		hDisplay->eAlignmentState = BVDC_P_Alignment_eInactive;
	}

	hDisplay->stCurInfo.stDirty.stBits.bAlignment = BVDC_P_CLEAN;

	return;
}


/**************** HDMI line dropping event handlers **************/
static void BVDC_P_Display_Copy_HdmiDropLines_Setting
	( BVDC_Display_Handle              hDisplay )
{
	int i;

	for (i = 0; i < BFMT_VideoFmt_eMaxCount; i++)
	{
		hDisplay->stCurInfo.aulHdmiDropLines[i] =
			hDisplay->stNewInfo.aulHdmiDropLines[i];
	}

	return;
}

static void BVDC_P_Display_Apply_HdmiDropLines_Setting
	( BVDC_Display_Handle              hDisplay,
	  BVDC_P_ListInfo                 *pList,
	  BAVC_Polarity                    eFieldPolarity )
{
	BSTD_UNUSED(pList);
	BSTD_UNUSED(eFieldPolarity);

	/* The new setting is achieved through a format switch.
	 */
	hDisplay->stCurInfo.stDirty.stBits.bHdmiDroplines = BVDC_P_CLEAN;

	return;
}


/**************** HDMI XvYcc event handlers **************/
static void BVDC_P_Display_Copy_HdmiXvYcc_Setting
	( BVDC_Display_Handle              hDisplay )
{
	hDisplay->stCurInfo.bXvYcc = hDisplay->stNewInfo.bXvYcc;

	return;
}

static void BVDC_P_Display_Apply_HdmiXvYcc_Setting
	( BVDC_Display_Handle              hDisplay,
	  BVDC_P_ListInfo                 *pList,
	  BAVC_Polarity                    eFieldPolarity )
{
	BVDC_P_Output eOutputCS;

	BSTD_UNUSED(eFieldPolarity);

	if (hDisplay->stAnlgChan_0.bEnable)
	{
		/* Reload analog path CSC settings to undo the
		 * XvYcc color space change made at compositor level.
		 */
		eOutputCS = hDisplay->stCurInfo.eAnlg_0_OutputColorSpace;
		BVDC_P_Vec_Build_CSC_SRC_SM_isr(hDisplay, &hDisplay->stAnlgChan_0, eOutputCS, pList);
	}

	if (hDisplay->stAnlgChan_1.bEnable)
	{
		eOutputCS = hDisplay->stCurInfo.eAnlg_1_OutputColorSpace;
		BVDC_P_Vec_Build_CSC_SRC_SM_isr(hDisplay, &hDisplay->stAnlgChan_1, eOutputCS, pList);
	}

	hDisplay->stCurInfo.stDirty.stBits.bHdmiXvYcc = BVDC_P_CLEAN;

	return;
}


/**************** HDMI sync-only event handlers **************/
static BERR_Code BVDC_P_Display_Validate_HdmiSyncOnly_Setting
	( BVDC_Display_Handle              hDisplay )
{
	if(!hDisplay->stNewInfo.bEnableHdmi)
	{
		BDBG_ERR(("Display [%d] doesn't have DVI channel enabled", hDisplay->eId));
		return BERR_TRACE(BVDC_ERR_INVALID_HDMI_PATH);
	}

	return BERR_SUCCESS;
}

static void BVDC_P_Display_Apply_HdmiSyncOnly_Setting
	( BVDC_Display_Handle              hDisplay,
	  BVDC_P_ListInfo                 *pList,
	  BAVC_Polarity                    eFieldPolarity )
{
	BSTD_UNUSED(pList);
	BSTD_UNUSED(eFieldPolarity);

	if(!hDisplay->stCurInfo.bEnableHdmi)
	{
		goto done;
	}

	/* The new setting is achieved through a mute settings.
	 */
done:
	hDisplay->stCurInfo.stDirty.stBits.bHdmiSyncOnly = BVDC_P_CLEAN;

	return;
}

static void BVDC_P_Display_Copy_HdmiDeepColor_Setting
	( BVDC_Display_Handle              hDisplay )
{
	hDisplay->stCurInfo.eHdmiColorDepth = hDisplay->stNewInfo.eHdmiColorDepth;
	hDisplay->stCurInfo.eHdmiPixelRepetition = hDisplay->stNewInfo.eHdmiPixelRepetition;

	return;
}

static void BVDC_P_Display_Apply_HdmiDeepColor_Setting
	( BVDC_Display_Handle              hDisplay,
	  BVDC_P_ListInfo                 *pList,
	  BAVC_Polarity                    eFieldPolarity )
{
	BVDC_P_Vec_Build_Hdmi_isr(hDisplay, pList);
	BSTD_UNUSED(eFieldPolarity);
	hDisplay->stCurInfo.stDirty.stBits.bHdmiRmChanged = BVDC_P_CLEAN;

	return;
}

static void BVDC_P_Display_Copy_3D_Setting
	( BVDC_Display_Handle              hDisplay )
{
	hDisplay->stCurInfo.eOrientation = hDisplay->stNewInfo.eOrientation;
	hDisplay->stCurInfo.e3dSrcBufSel = hDisplay->stNewInfo.e3dSrcBufSel;
}

static void BVDC_P_Display_Apply_3D_Setting
	( BVDC_Display_Handle              hDisplay,
	  BVDC_P_ListInfo                 *pList,
	  BAVC_Polarity                    eFieldPolarity )
{
#if (BVDC_P_SUPPORT_3D_VIDEO)
	if(hDisplay->bAnlgEnable ||   /* if analog master */
	   (!hDisplay->bAnlgEnable &&  /* or anlog slave with DACs */
	   (hDisplay->stAnlgChan_0.bEnable || hDisplay->stAnlgChan_1.bEnable)))
	{
		BVDC_P_Vec_Build_It3D_isr(hDisplay, pList);
	}
#else
	BSTD_UNUSED(pList);
#endif

	BSTD_UNUSED(eFieldPolarity);
	hDisplay->stCurInfo.stDirty.stBits.b3DSetting = BVDC_P_CLEAN;
}

static void BVDC_P_Display_Copy_VfFilter_Setting
	( BVDC_Display_Handle              hDisplay )
{
	int i;

	for (i = 0; i < BVDC_P_VEC_CH_NUM; i++)
	{
		hDisplay->stCurInfo.abUserVfFilterCo[i] = hDisplay->stNewInfo.abUserVfFilterCo[i];
		hDisplay->stCurInfo.abUserVfFilterCvbs[i] = hDisplay->stNewInfo.abUserVfFilterCvbs[i];
		BKNI_Memcpy(hDisplay->stCurInfo.aaulUserVfFilterCo[i], hDisplay->stNewInfo.aaulUserVfFilterCo[i], BVDC_P_CHROMA_TABLE_SIZE * sizeof(uint32_t));
		BKNI_Memcpy(hDisplay->stCurInfo.aaulUserVfFilterCvbs[i], hDisplay->stNewInfo.aaulUserVfFilterCvbs[i], BVDC_P_CHROMA_TABLE_SIZE * sizeof(uint32_t));
	}

	return;
}

static void BVDC_P_Display_Apply_VfFilter_Setting
	( BVDC_Display_Handle              hDisplay,
	  BVDC_P_ListInfo                 *pList,
	  BAVC_Polarity                    eFieldPolarity )
{
	BVDC_P_DisplayInfo  *pCurInfo = &hDisplay->stCurInfo;
	BSTD_UNUSED(eFieldPolarity);

	/* Update VF filter settings */
	if (hDisplay->stAnlgChan_0.bEnable)
	{
		BVDC_P_Vec_Build_VF_isr(hDisplay, &hDisplay->stAnlgChan_0,
			pCurInfo->eAnlg_0_OutputColorSpace, pList);
	}

	if (hDisplay->stAnlgChan_1.bEnable)
	{
		BVDC_P_Vec_Build_VF_isr(hDisplay, &hDisplay->stAnlgChan_1,
			pCurInfo->eAnlg_1_OutputColorSpace, pList);
	}

	hDisplay->stCurInfo.stDirty.stBits.bVfFilter = BVDC_P_CLEAN;

	return;
}


static void BVDC_P_Display_Copy_OutputMute_Setting
	( BVDC_Display_Handle              hDisplay )
{
	BKNI_Memcpy(hDisplay->stCurInfo.abOutputMute, hDisplay->stNewInfo.abOutputMute, sizeof (hDisplay->stCurInfo.abOutputMute));

	return;
}

static void BVDC_P_Display_Apply_OutputMute_Setting
	( BVDC_Display_Handle              hDisplay,
	  BVDC_P_ListInfo                 *pList,
	  BAVC_Polarity                    eFieldPolarity )
{
	BVDC_P_Output       eOutputCS;

	if (hDisplay->stAnlgChan_0.bEnable)
	{
		eOutputCS = hDisplay->stCurInfo.eAnlg_0_OutputColorSpace;
		BVDC_P_Vec_Build_CSC_SRC_SM_isr(hDisplay, &hDisplay->stAnlgChan_0, eOutputCS, pList);
	}

	if (hDisplay->stAnlgChan_1.bEnable)
	{
		eOutputCS = hDisplay->stCurInfo.eAnlg_1_OutputColorSpace;
		BVDC_P_Vec_Build_CSC_SRC_SM_isr(hDisplay, &hDisplay->stAnlgChan_1, eOutputCS, pList);
	}

	if(hDisplay->stCurInfo.bEnableHdmi)
	{
		BVDC_P_Vec_Build_DVI_CSC_isr(hDisplay, pList );
	}

#if (BVDC_P_SUPPORT_ITU656_OUT != 0)
	if (hDisplay->st656Chan.bEnable)
	{
		BVDC_P_Vec_Build_656_CSC_isr(hDisplay, pList );
	}
#endif

	hDisplay->stCurInfo.stDirty.stBits.bOutputMute = BVDC_P_CLEAN;

	BSTD_UNUSED(eFieldPolarity);

	return;
}


static void BVDC_P_Display_Copy_bMiscCtrl
	( BVDC_Display_Handle              hDisplay )
{
	hDisplay->stCurInfo.bArtificialVsync   = hDisplay->stNewInfo.bArtificialVsync;
	hDisplay->stCurInfo.ulArtificialVsyncRegAddr = hDisplay->stNewInfo.ulArtificialVsyncRegAddr;
	hDisplay->stCurInfo.ulArtificialVsyncMask = hDisplay->stNewInfo.ulArtificialVsyncMask;

	return;
}

static void BVDC_P_Display_Apply_bMiscCtrl
	( BVDC_Display_Handle              hDisplay,
	  BVDC_P_ListInfo                 *pList,
	  BAVC_Polarity                    eFieldPolarity )
{
	BSTD_UNUSED(pList);
	BSTD_UNUSED(eFieldPolarity);
	hDisplay->stCurInfo.stDirty.stBits.bMiscCtrl = BVDC_P_CLEAN;
	return;
}


/* This table has the handlers for all the display events.
 *
 * Note, it is critical to keep the position of each hander match with
 * the position of its associated dirty bit. The position of dirty bit
 * will be used to index this table to obtain the handlers.
 *
 */
const BVDC_Display_EventHandler BVDC_P_astDisplayEventHndlTbl[] =
{
	/* New analog channel 0    event                               - index 0 */
	{
		NULL,
		NULL,
		BVDC_P_Display_Apply_Chan0
	},

	/* New analog channel 1    event                               - index 1 */
	{
		NULL,
		NULL,
		BVDC_P_Display_Apply_Chan1
	},

	/* New video timing format event                               - index 2 */
	{
		BVDC_P_Display_Validate_VideoFormat,
		BVDC_P_Display_Copy_VideoFormat,
		BVDC_P_Display_Apply_VideoFormat
	},

	/* New video timing format event, no reload of VEC IT uCode.   - index 3 */
	{
		BVDC_P_Display_Validate_VideoFormatSoft,
		BVDC_P_Display_Copy_VideoFormatSoft,
		BVDC_P_Display_Apply_VideoFormatSoft
	},

	/* Change 3D setting                                           - index 4 */
	{
		/* Changing 3D setting */
		NULL,
		BVDC_P_Display_Copy_3D_Setting,
		BVDC_P_Display_Apply_3D_Setting
	},

	/* Setting DAC event                                           - index 5 */
	{
		NULL, /* Validated together with video format */
		BVDC_P_Display_Copy_DAC_Setting,
		BVDC_P_Display_Apply_DAC_Setting
	},

	/* Setting time base event                                     - index 6 */
	{
		NULL,
		BVDC_P_Display_Copy_TimeBase_Setting,
		BVDC_P_Display_Apply_TimeBase_Setting
	},

	/* New callback mask event                                     - index 7 */
	{
		NULL,
		BVDC_P_Display_Copy_Callback_Setting,
		BVDC_P_Display_Apply_Callback_Setting
	},

	/* New callback function event                                 - index 8 */
	{
		NULL,
		BVDC_P_Display_Copy_CallbackFunc_Setting,
		BVDC_P_Display_Apply_CallbackFunc_Setting
	},

	/* Trimming width evet                                         - index 9 */
	{
		/* Internal event from compositor. So no validation and copy here. Settings
		 * go to current info directly
		 */
		NULL,
		NULL,
		BVDC_P_Display_Apply_TrimmingWidth_Setting
	},

	/* Input color space change event                              - index 10 */
	{
		/* Internal event from compositor. So no validation and copy here. Settings
		 * go to current info directly
		 */
		NULL,
		NULL,
		BVDC_P_Display_Apply_InputColorSpace_Setting
	},

	/* 240p mode event                                             - index 11 */
	{
		/* Internal event from compositor. So no validation and copy here. Settings
		 * go to current info directly
		 */
		NULL,
		NULL,
		BVDC_P_Display_Apply_240pMode_Setting
	},

	/* VDEC -> VEC phase adjustment event                          - index 12 */
	{
		/* Internal event from compositor. So no validation and copy here. Settings
		 * go to current info directly
		 */
		NULL,
		NULL,
		BVDC_P_Display_Apply_PhaseAdjustement_Setting
	},

	/* Source frame rate change event                              - index 13 */
	{
		/* Internal event from compositor. So no validation and copy here. Settings
		 * go to current info directly
		 */
		BVDC_P_Display_Validate_SrcFrameRate_Setting,
		NULL,
		BVDC_P_Display_Apply_SrcFrameRate_Setting
	},

	/* Frame drop event	                                           - index 14 */
	{
		/* 59.94Hz vs. 60Hz and 29.97Hz vs 30Hz
		 */
		NULL,
		BVDC_P_Display_Copy_FrameDrop_Setting,
		BVDC_P_Display_Apply_FrameDrop_Setting
	},

#if (BVDC_P_SUPPORT_RFM_OUTPUT != 0)
	/* RFM configuration event	                                   - index 15 */
	{
		/* Enable/disable RFM ouput.
		 */
		BVDC_P_Display_Validate_Rfm_Setting,
		BVDC_P_Display_Copy_Rfm_Setting,
		BVDC_P_Display_Apply_Rfm_Setting
	},
#endif

	/* Enable/disable HDMI event                                   - index 16 */
	{
		/* Enable/disable HDMI ouput.
		 */
		BVDC_P_Display_Validate_Hdmi_Setting,
		BVDC_P_Display_Copy_Hdmi_Setting,
		BVDC_P_Display_Apply_Hdmi_Setting
	},

	/* HDMI output CSC matrix setting event                        - index 17 */
	{
		/* Set new HDMI output CSC matrix.
		 */
		NULL,
		BVDC_P_Display_Copy_HdmiCsc_Setting,
		BVDC_P_Display_Apply_HdmiCsc_Setting
	},

#if (BVDC_P_SUPPORT_ITU656_OUT != 0)
	/* Enable/disable 656 event                                    - index 18 */
	{
		/* Enable/disable 656 ouput.
		 */
		BVDC_P_Display_Validate_656_Setting,
		BVDC_P_Display_Copy_656_Setting,
		BVDC_P_Display_Apply_656_Setting
	},
#endif

	/* Compenent MPAA setting event                                - index 19 */
	{
		/* Enable/disable MPAA.
		 */
		BVDC_P_Display_Validate_CompMpaa_Setting,
		BVDC_P_Display_Copy_CompMpaa_Setting,
		BVDC_P_Display_Apply_CompMpaa_Setting
	},

	/* HDMI setting event                                          - index 20 */
	{
		/* Enable/disable MPAA.
		 */
		BVDC_P_Display_Validate_HdmiMpaa_Setting,
		BVDC_P_Display_Copy_HdmiMpaa_Setting,
		BVDC_P_Display_Apply_HdmiMpaa_Setting
	},

	/* Snapshot timestamp event                                    - index 21 */
	{
		/* Take a snapshot of temstamp.
		 */
		NULL,
		NULL,
		BVDC_P_Display_Apply_TimeStamp_Setting
	},

	/* display alignment event                                     - index 22 */
	{
		/* Turn on/off alignment.
		 */
		BVDC_P_Display_Validate_Alignment_Setting,
		BVDC_P_Display_Copy_Alignment_Setting,
		BVDC_P_Display_Apply_Alignment_Setting
	},

	/* HDMI line-dropping event                                    - index 23 */
	{
		/* New HDMI line-dropping settings */
		NULL,
		BVDC_P_Display_Copy_HdmiDropLines_Setting,
		BVDC_P_Display_Apply_HdmiDropLines_Setting
	},

	/* HDMI XvYcc event                                            - index 24 */
	{
		/* New HDMI XvYcc settings */
		NULL,
		BVDC_P_Display_Copy_HdmiXvYcc_Setting,
		BVDC_P_Display_Apply_HdmiXvYcc_Setting
	},

	/* HDMI sync-only event                                        - index 25 */
	{
		/* Turn on/off HDMI sync-only feature */
		BVDC_P_Display_Validate_HdmiSyncOnly_Setting,
		NULL,
		BVDC_P_Display_Apply_HdmiSyncOnly_Setting
	},

	/* HDMI deep color event                                       - index 26 */
	{
		/* Changing deep color settings */
		NULL,
		BVDC_P_Display_Copy_HdmiDeepColor_Setting,
		BVDC_P_Display_Apply_HdmiDeepColor_Setting
	},

	/* display aspect ratio event                                  - index 27 */
	{
		/* asp ratio or aspR canvas clip settings */
		BVDC_P_Display_Validate_AspRatio_Setting,
		BVDC_P_Display_Copy_AspRatio_Setting,
		BVDC_P_Display_Apply_AspRatio_Setting
	},

#if (BVDC_P_SUPPORT_STG != 0)
	/* Enable/disable Stg event                                    - index 28 */
	{
		/* Enable/disable Stg ouput.
		 */
		BVDC_P_Display_Validate_Stg_Setting,
		BVDC_P_Display_Copy_Stg_Setting,
		BVDC_P_Display_Apply_Stg_Setting
	},
#endif

	/* Place holder                                                - index 29 */
	{
		NULL,
		BVDC_P_Display_Copy_VfFilter_Setting,
		BVDC_P_Display_Apply_VfFilter_Setting
	},

	/* the following place holders are for coverity check only */

	/* Enable/disable output mute event.                           - index 30 */
	{
		NULL,
		BVDC_P_Display_Copy_OutputMute_Setting,
		BVDC_P_Display_Apply_OutputMute_Setting
	},

	/* Place holder                                                - index 31 */
	{
		NULL,
		BVDC_P_Display_Copy_bMiscCtrl,
		BVDC_P_Display_Apply_bMiscCtrl
	},

	/* to silent coverity defects */
#if (BVDC_P_SUPPORT_RFM_OUTPUT == 0)
	/* Extra Place holder                                          - index 15 */
	{
		/* Empty */
		NULL,
		NULL,
		NULL
	},
#endif

#if (BVDC_P_SUPPORT_ITU656_OUT == 0)
	/* Extra Place holder                                          - index 18 */
	{
		/* Empty */
		NULL,
		NULL,
		NULL
	},
#endif

#if (BVDC_P_SUPPORT_STG == 0)
	/* Extra Place holder                                          - index 28 */
	{
		/* Empty */
		NULL,
		NULL,
		NULL
	},
#endif
};

/* End of File */
