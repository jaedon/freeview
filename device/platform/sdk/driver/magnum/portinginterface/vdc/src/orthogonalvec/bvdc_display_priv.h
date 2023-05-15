/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvdc_display_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/134 $
 * $brcm_Date: 9/4/12 4:52p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vdc/src/orthogonalvec/bvdc_display_priv.h $
 * 
 * Hydra_Software_Devel/134   9/4/12 4:52p darnstein
 * SW7346-909: tables of IT block register values grow by one entry. THe
 * tables that previously covered ADDR_0_3 to PCL_8 now end with
 * STACK_REG_8_9.
 * 
 * Hydra_Software_Devel/133   8/24/12 10:17a tdo
 * SW7552-324: Fix bug in selecting SECAM block when format switching
 * to/from secam format
 *
 * Hydra_Software_Devel/132   8/15/12 6:23p tdo
 * SW7445-8: Create Appframework emulation environment for 7445 A0
 *
 * Hydra_Software_Devel/131   8/9/12 11:41a syang
 * SW7425-2172: improve def of  BVDC_P_DISPLAY_ONE_VSYNC_INTERVA
 *
 * Hydra_Software_Devel/130   8/8/12 6:01p tdo
 * SW7125-1302: Composite output is black & white when Component not
 * enabled
 *
 * Hydra_Software_Devel/129   7/27/12 3:45p vanessah
 * SW7425-3150: adding Bypass Video Processing support
 *
 * Hydra_Software_Devel/128   7/17/12 7:04p darnstein
 * SW7425-3474: DCS: try much harder to guarantee that previous RUL update
 * was successful.
 *
 * Hydra_Software_Devel/127   7/16/12 7:25p darnstein
 * SW7425-3474: Fix two DCS problems: 1. Soft on and Soft off procedures
 * need access control. 2. Previous state updates need to be carefully
 * tested for completion (RULs).
 *
 * Hydra_Software_Devel/126   7/13/12 11:48a tdo
 * SW7563-9: Fix compiling error
 *
 * Hydra_Software_Devel/125   7/13/12 11:27a tdo
 * SW7563-9: Add support for VDC
 *
 * Hydra_Software_Devel/124   7/3/12 7:26p albertl
 * SW7125-1297: Changed Svideo Chroma VF adjustments to adjust Ch1 and Ch2
 * and Composite adjustments to affect all channels.
 *
 * Hydra_Software_Devel/123   6/29/12 12:28p darnstein
 * HW7552-167: Support the VAMP on/off bit in CPC array for custom
 * Macrovision.
 *
 * Hydra_Software_Devel/122   4/12/12 4:09p vanessah
 * SW7425-2831: add NRT mode definition
 *
 * Hydra_Software_Devel/121   3/21/12 1:39p syang
 * SW7425-2699: put mpaa under the control of vdc resource manager; handle
 * correctly both order of enabling/disabling mpaa and turning on/off
 * hdmi/component output
 *
 * Hydra_Software_Devel/120   3/2/12 4:04p pntruong
 * SW7231-584: Added artificial display vsync generation to support dsp
 * transcoding.
 *
 * Hydra_Software_Devel/119   2/23/12 11:08a vanessah
 * SW7425-2431: change memcpy for code cleaness
 *
 * Hydra_Software_Devel/118   2/23/12 10:19a vanessah
 * SW7425-2431: fix custom fmt stress test
 *
 * Hydra_Software_Devel/117   1/26/12 12:06p vanessah
 * SW7435-9: move STG version definition to bvdc_displaystg_priv.c
 *
 * Hydra_Software_Devel/116   1/20/12 6:30p vanessah
 * SW7435-9: add STG2/3 support
 *
 * Hydra_Software_Devel/115   1/17/12 3:36p darnstein
 * SW7125-1124: disable some diagnostics for DCS: RDC scratch variables.
 *
 * Hydra_Software_Devel/114   1/11/12 3:07p darnstein
 * SW7125-1124: merge DCS Hybrid+ to main branch. DCS almost ready for
 * production.
 *
 * Hydra_Software_Devel/SW7125-1124/12   12/20/11 6:47p darnstein
 * SW7125-1124: DCS: Simplify logic a bit. Add diagnostics.
 *
 * Hydra_Software_Devel/SW7125-1124/11   12/19/11 4:55p darnstein
 * SW7125-1124: simplify DCS access method. A few bug fixes too.
 *
 * Hydra_Software_Devel/SW7125-1124/10   12/14/11 4:45p darnstein
 * SW7125-1124: merge from main branch.
 *
 * Hydra_Software_Devel/SW7125-1124/9   12/13/11 6:22p darnstein
 * SW7125-1124: DCS LOCK ON function added to API.
 *
 *
 * Hydra_Software_Devel/SW7125-1124/8   12/13/11 3:52p darnstein
 * SW7125-1124: merge from main branch.
 *
 * Hydra_Software_Devel/SW7125-1124/7   12/12/11 7:45p darnstein
 * SW7125-1124: DCS Hybrid+ software ready for debugging.
 *
 * Hydra_Software_Devel/SW7125-1124/6   12/9/11 9:43p darnstein
 * SW7125-1124: need to add a dirty bit. Checking in DCS work first.
 *
 * Hydra_Software_Devel/SW7125-1124/5   12/6/11 8:17p darnstein
 * SW7125-1124: Finish DCS code swapping logic.
 *
 * Hydra_Software_Devel/SW7125-1124/4   12/1/11 6:46p darnstein
 * SW7125-1124: change definition of IT_VER to identify early version of
 * IT hardware that has PCL_6, PCL_7, and PCL_8 registers.
 *
 * Hydra_Software_Devel/SW7125-1124/3   12/1/11 5:38p darnstein
 * SW7125-1124: remove unneeded functions.
 *
 * Hydra_Software_Devel/SW7125-1124/1   11/10/11 7:03p darnstein
 * SW7125-1124: partial implementation of DCS Hybrid+.
 *
 * Hydra_Software_Devel/110   11/9/11 3:07p darnstein
 * SW7425-1658: finish programming VF NEG_SYNC amplitude (extension,
 * VALUE2).
 *
 * Hydra_Software_Devel/109   11/8/11 1:11p vanessah
 * SW7425-1704: factor STG functions out of bvdc_displayhandler_priv.c
 *
 * Hydra_Software_Devel/108   11/4/11 3:49p darnstein
 * SW7425-1658: program the new register VF_n_NEG_SYNC_AMPLITUDE_EXTN.
 *
 * Hydra_Software_Devel/107   11/3/11 3:57p tdo
 * SW7344-135: Video lost in Vesa format on HDMI/Component outputs when
 * switching output formats during a playback.  IT stand-alone is not
 * working for 7344B0/7231B0
 *
 * Hydra_Software_Devel/106   10/19/11 1:45p vanessah
 * SW7425-1488: MADR RUL delay for NRT transcode
 *
 * Hydra_Software_Devel/105   9/8/11 10:23p tdo
 * SW7425-922: VDC bringup for B0
 *
 * Hydra_Software_Devel/104   8/31/11 11:48a syang
 * SW7425-1170: pxlAspRatio passed to ViCE2 is corrected to format x<<16 |
 * y
 *
 * Hydra_Software_Devel/103   8/26/11 5:40p syang
 * SW7425-1170: merge from branch SW7572-1170
 *
 * Hydra_Software_Devel/SW7425-1170/2   8/26/11 4:28p syang
 * SW7425-1170: refactor all pixel aspect ratio related code
 *
 * Hydra_Software_Devel/SW7425-1170/1   8/24/11 6:57p vanessah
 * SW7425-1170: ASP code clean
 *
 * Hydra_Software_Devel/102   8/25/11 3:56p pntruong
 * SW7425-1191: Rollback.  The acquire/release is more efficience to be
 * handle in nexus, where the done event is already known.
 *
 * Hydra_Software_Devel/100   8/23/11 9:24p tdo
 * SW7231-309: Teardown/release resource for the correct analog path again
 * in case of missed RUL.
 *
 * Hydra_Software_Devel/99   8/22/11 12:17p tdo
 * SW7231-309: handling vec resource when analog is in slave mode.
 *
 * Hydra_Software_Devel/98   8/17/11 11:49a tdo
 * SW7425-979: Implement the new trigger
 *
 * Hydra_Software_Devel/97   8/5/11 8:34p albertl
 * SW7425-745: Added blanking to compositor color for all outputs and all
 * colorspaces.
 *
 * Hydra_Software_Devel/96   8/5/11 6:18p vanessah
 * SW7425-923: update ViCE2 MBox data structure definition and 2nd STG
 * bring up
 *
 * Hydra_Software_Devel/95   8/2/11 7:47p vanessah
 * SW7425-465: optimization of delay counter
 *
 * Hydra_Software_Devel/94   7/19/11 6:43p vanessah
 * SW7425-923: stg register offset cleanup
 *
 * Hydra_Software_Devel/93   7/18/11 5:33p pntruong
 * SW7231-284, SW7346-295, SW7405-5339: Refactored master framerate
 * tracking, so that it can also handle validation.
 *
 * Hydra_Software_Devel/92   7/18/11 11:26a vanessah
 * SW7425-835: SW7425-923: fix ViCE2 channel id bug + B0 STG
 *
 * Hydra_Software_Devel/91   7/11/11 2:18p tdo
 * SW7420-1971: Video Pause seen when VEC alignment is going on.  Add flag
 * to keep BVN connected while doing alignment.
 *
 * Hydra_Software_Devel/90   7/8/11 6:01p albertl
 * SW7408-291: Updated VF filter API to use Dac enums for channel
 * selection.
 *
 * Hydra_Software_Devel/89   7/6/11 6:07p albertl
 * SW7408-291, SW7425-745: Changed VF filter API to also take a selected
 * display output as parameter.  Added selective muting of display
 * outputs using CSC.
 *
 * Hydra_Software_Devel/88   6/24/11 3:51p albertl
 * SW7420-1966: Cleaned up and reorganized colorspace enums and indicies
 * in display, vdec, and cmp.
 *
 * Hydra_Software_Devel/87   6/17/11 2:33p albertl
 * SW7408-291:  Added interface to set and get VF filter settings.
 *
 * Hydra_Software_Devel/86   6/3/11 6:01p tdo
 * SW7346-237: More VEC resource management fine tuning
 *
 * Hydra_Software_Devel/85   6/1/11 1:00p tdo
 * SW7346-237: Using both RGB and CVBS output issue.
 *
 * Hydra_Software_Devel/84   5/27/11 6:38p vanessah
 * SW7425-465: fix for transcoding transition black frame artifact
 *
 * Hydra_Software_Devel/83   5/24/11 7:58p albertl
 * SW7425-169: Converted 10-bit and 8-bit filters to compact hex format
 * and updated to use latest recommended filter coefficients.
 *
 * Hydra_Software_Devel/82   3/26/11 12:07a tdo
 * SW7125-248: Enhance analog channel resource management
 *
 * Hydra_Software_Devel/81   3/25/11 2:35p yuxiaz
 * SW7420-1741: Added game mode delay support.
 *
 * Hydra_Software_Devel/80   3/21/11 1:42p tdo
 * SW7420-1638: DACs do not get powered down properly with certain call
 * sequences
 *
 * Hydra_Software_Devel/79   1/25/11 5:56p vanessah
 * SW7425-46: PAR info for VicE2 MBox metadata
 *
 * Hydra_Software_Devel/78   1/20/11 3:25p yuxiaz
 * SW7422-97: Rename macros for HDMI Rate manage
 *
 * Hydra_Software_Devel/77   1/20/11 2:42p yuxiaz
 * SW7422-97: Clean up macros for HDMI Rate manage
 *
 * Hydra_Software_Devel/76   1/17/11 4:55p yuxiaz
 * SW7422-97: Added KP, KI and KA settings on 7422.
 *
 * Hydra_Software_Devel/75   1/7/11 5:13p tdo
 * SW7125-792: Properly handle HDMI sync only
 *
 * Hydra_Software_Devel/74   11/29/10 11:22a hongtaoz
 * SW7425-46: corrected STG picture polarity;
 *
 * Hydra_Software_Devel/73   11/18/10 12:32p tdo
 * SW7422-92: Add function to set display orientation.  Add new dirty bit
 * to handle 3D setting
 *
 * Hydra_Software_Devel/72   11/16/10 6:52p tdo
 * SW7422-92: Need display 3D orientation override function for VDC
 *
 * Hydra_Software_Devel/71   11/16/10 4:07p yuxiaz
 * SW7422-97: Updated basic HDMI TX rate manager settings for 40nm chips.
 *
 * Hydra_Software_Devel/70   10/22/10 7:04p vanessah
 * SW7425-46:  ViCE2 meta data mailbox register
 *
 * Hydra_Software_Devel/69   10/2/10 1:32a hongtaoz
 * SW7425-29: adding STG support;
 *
 * Hydra_Software_Devel/68   10/1/10 11:15a hongtaoz
 * SW7425-29: added explicit enable argument to the STG set API;
 *
 * Hydra_Software_Devel/67   9/23/10 10:34a vanessah
 * SW7425-29:  STG support for transcoding
 *
 * Hydra_Software_Devel/66   8/24/10 3:53p tdo
 * SW7420-978: BVB_SAV_REMOVE is set to 3 with 704 microcode.  Clean up
 * duplicated logic.
 *
 * Hydra_Software_Devel/65   6/16/10 6:34p tdo
 * SW7400-2770: Add multi-frame rate support to SD also
 *
 * Hydra_Software_Devel/64   6/7/10 4:46p tdo
 * SW7208-57, SW7125-248: VEC resource management fine tuning
 *
 * Hydra_Software_Devel/63   5/10/10 3:48p albertl
 * SW7125-364: Fixed big endian compile error.
 *
 * Hydra_Software_Devel/62   5/7/10 7:22p albertl
 * SW7125-364: Changed dirty bits to use union structure to avoid type-pun
 * warnings
 *
 * Hydra_Software_Devel/61   4/7/10 11:41a tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic.  Rename TLA
 *
 * Hydra_Software_Devel/60   4/5/10 4:19p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic
 *
 * Hydra_Software_Devel/59   2/9/10 2:30p rpan
 * SW7400-2642: Fixed a compile error for chips with more than one SECAM.
 *
 * Hydra_Software_Devel/58   2/9/10 2:09p rpan
 * SW7400-2642: Added SECAM output for orthogonal VEC.
 *
 * Hydra_Software_Devel/57   1/29/10 8:34p albertl
 * SW7405-3845: Fixed PAL_NC matrices and renamed associated colorspace as
 * YUV_NC.
 *
 * Hydra_Software_Devel/56   1/6/10 3:41p rpan
 * SW7468-64: Consolidate VEC modulo count trigger implementation.
 *
 * Hydra_Software_Devel/55   11/30/09 10:34a rpan
 * SW7468-30: Initial OSCL support.
 *
 * Hydra_Software_Devel/54   11/2/09 4:35p tdo
 * SW7340-71: Flash occurs when Same Format is Set in VDC (toggling 16x9
 * or Auto-Format.)
 *
 * Hydra_Software_Devel/53   10/23/09 6:50p darnstein
 * SW7405-3132,SW7405-3133,HW7413-238,SW7340-45: Updates for PAL-M, PAL-N,
 * and Macrovision. Fix small error in NTSC/Macrovision_TEST0?.
 *
 * Hydra_Software_Devel/52   10/16/09 5:30p darnstein
 * SW7405-3191: Back out all changes in pixel frequency handling. New
 * design coming soon.
 *
 * Hydra_Software_Devel/51   10/15/09 5:20p darnstein
 * SW7405-3191: Pixel frequency is now defined as both a bitmask, and an
 * enum. The bitmask option is DEPRECATED.
 *
 * Hydra_Software_Devel/50   10/13/09 6:03p darnstein
 * SW7405-3004: install 480P ARIB output via BVDC display settings.
 *
 * Hydra_Software_Devel/49   8/25/09 11:40a rpan
 * SW7420-324: Enhance resource management for DVI and 656. Take the RUL
 * not executed scenario into consideration.
 *
 * Hydra_Software_Devel/48   8/24/09 3:56p rpan
 * SW7342-27: Make analog channel 1 data structure and programming model
 * consistent with channel 0. Enhance channel 1 handling state machine.
 *
 * Hydra_Software_Devel/47   8/21/09 5:30p rpan
 * PR57873: Enhanced analog channel 1 resource management. Take the case
 * that RUL doesn't get executed into consideration.
 *
 * Hydra_Software_Devel/46   8/21/09 2:35p tdo
 * PR57734: Add capability for display to handle DACs re-assignment
 *
 * Hydra_Software_Devel/45   6/25/09 12:13p rpan
 * PR56137, PR56138, PR56139, PR56166, PR56167, PR56168: Support for
 * various orthogonal VEC configurations.
 *
 * Hydra_Software_Devel/44   6/22/09 3:12p darnstein
 * PR55225: mechanical changes to support 7342 chipset.
 *
 * Hydra_Software_Devel/43   6/19/09 7:46p darnstein
 * PR55225: previous check-in messed up 7550 compile.
 *
 * Hydra_Software_Devel/42   6/19/09 6:56p darnstein
 * PR55225: previous checkin was slightly broken.
 *
 * Hydra_Software_Devel/41   6/19/09 6:27p darnstein
 * PR55225: make some adjustments for 7342.
 *
 * Hydra_Software_Devel/40   6/18/09 5:55p syang
 * PR 55812: add 7550 support
 *
 * Hydra_Software_Devel/39   6/11/09 4:07p darnstein
 * PR55219: fill in some details for 7340 chipset.
 *
 * Hydra_Software_Devel/38   4/21/09 4:07p rpan
 * PR53070: Added more VEC alignment options for memory reduction.
 *
 * Hydra_Software_Devel/37   4/7/09 9:57a tdo
 * PR53373: Add HDMI deep color support
 *
 * Hydra_Software_Devel/36   3/31/09 3:29p hongtaoz
 * PR53037, PR53687: define display alignment threshold in usecs;
 *
 * Hydra_Software_Devel/35   3/31/09 3:19p rpan
 * PR53156: Implemented a non-reset version 704<->720 display format
 * switch.
 *
 * Hydra_Software_Devel/34   3/31/09 1:25p hongtaoz
 * PR53037: fixed compile error for 7420; 7420 VEC alignment is not ready
 * yet;
 *
 * Hydra_Software_Devel/33   3/18/09 7:33p darnstein
 * PR51527: DCS logic in place and mostly working.
 *
 * Hydra_Software_Devel/32   3/17/09 11:53a darnstein
 * PR51527: Late merge of Macrovision software.
 *
 * Hydra_Software_Devel/31   3/16/09 10:56p tdo
 * PR45785, PR45789: Merge from MCVP branch
 *
 * Hydra_Software_Devel/7420_mcvp/4   3/5/09 9:13p darnstein
 * PR51527: Eliminate unused state variables eVbiPath and eVecPath. The
 * DCS files compile now, although they need a lot of work to acutally
 * function.
 *
 * Hydra_Software_Devel/7420_mcvp/3   3/4/09 5:52p tdo
 * PR51527: Fix compiling error
 *
 * Hydra_Software_Devel/7420_mcvp/2   3/4/09 5:24p tdo
 * PR45785, PR45789: merge from main branch on 3/04/09
 *
 * Hydra_Software_Devel/30   3/4/09 10:59a rpan
 * PR45804: Add BVDC_P_ prefix to display handler table variable name.
 *
 * Hydra_Software_Devel/7420_mcvp/1   3/3/09 7:25p darnstein
 * PR51527: Eliminate unused state variables eVbiPath and eVecPath. The
 * DCS files compile now, although they need a lot of work to acutally
 * function.
 *
 * Hydra_Software_Devel/29   2/11/09 9:59a rpan
 * PR45804: Added an VEC alignment threshold.
 *
 * Hydra_Software_Devel/28   2/10/09 4:47p rpan
 * PR52001: Added 7420 DAC power management.
 *
 * Hydra_Software_Devel/27   2/9/09 3:35p rpan
 * PR45804: Some clean-up work.
 *
 * Hydra_Software_Devel/26   2/9/09 12:08p rpan
 * PR51599: Updated HDMI PLL settings.
 *
 * Hydra_Software_Devel/25   2/6/09 2:52p rpan
 * PR45804: Added HDMI sync-only support.
 *
 * Hydra_Software_Devel/24   2/5/09 11:12a rpan
 * PR45804: More work for seamless attachment of a slave path.
 *
 * Hydra_Software_Devel/23   2/5/09 8:50a rpan
 * PR45804: Added the initial support for Hdmi XvYcc.
 *
 * Hydra_Software_Devel/22   2/4/09 3:43p rpan
 * PR45804: Added HDMI drop-line support.
 *
 * Hydra_Software_Devel/21   2/3/09 9:38a rpan
 * PR45804: Implemented another scheme of attaching DVI slave path to
 * analog master path, which resets master path.
 *
 * Hydra_Software_Devel/20   1/30/09 3:04p rpan
 * PR45807: Restart VEC alignment process in case timestamp taken is not
 * valid.
 *
 * Hydra_Software_Devel/19   1/29/09 4:49p rpan
 * PR45804: Added support for alignment.
 *
 * Hydra_Software_Devel/18   1/26/09 4:21p rpan
 * PR45804: Initial support for ITU656 output.
 *
 * Hydra_Software_Devel/17   1/20/09 9:12a rpan
 * PR45804: Initial HDMI support  work.
 *
 * Hydra_Software_Devel/16   1/12/09 4:07p rpan
 * PR45804: Added initial RFM output support.
 *
 * Hydra_Software_Devel/15   1/9/09 2:17p rpan
 * PR45804: Added component path MPAA support.
 *
 * Hydra_Software_Devel/14   12/23/08 2:15p rpan
 * PR45804: Implemented force frame drop handlers.
 *
 * Hydra_Software_Devel/13   12/22/08 10:12p rpan
 * PR45804: Implemented handlers for source stream related events.
 *
 * Hydra_Software_Devel/12   12/22/08 3:19p rpan
 * PR45804: Added support for display callback.
 *
 * Hydra_Software_Devel/11   12/17/08 9:31p rpan
 * PR50391: Some resource management enhancement.
 *
 * Hydra_Software_Devel/10   12/16/08 7:23p tdo
 * PR50391: Basic VDC bringup
 *
 * Hydra_Software_Devel/9   12/9/08 10:22p tdo
 * PR45804: Add Dac support
 *
 * Hydra_Software_Devel/8   12/9/08 4:46p rpan
 * PR45804: More orthogonal VEC support.
 *
 * Hydra_Software_Devel/7   12/8/08 5:44p rpan
 * PR45804: Switched to VDC generic resource management lib.
 *
 * Hydra_Software_Devel/6   12/5/08 3:52p rpan
 * PR45804: Cleaned up some tabs in the code.
 *
 * Hydra_Software_Devel/5   12/4/08 4:46p rpan
 * PR45804: Pass source related info from compositor to VEC in one shot.
 *
 * Hydra_Software_Devel/4   12/4/08 3:46p rpan
 * PR45804: Made changes from source follow the new event driven sheme.
 *
 * Hydra_Software_Devel/3   12/3/08 10:10p rpan
 * PR45804: More orthogonal VEC support.
 *
 * Hydra_Software_Devel/2   12/3/08 4:40p rpan
 * PR45804: Initial effort for the orthogonal VEC support.
 *
 * Hydra_Software_Devel/1   11/25/08 10:26a rpan
 * PR45804: Create 7420 specific display module.
 *
 ***************************************************************************/
#ifndef BVDC_DISPLAY_PRIV_H__
#define BVDC_DISPLAY_PRIV_H__

#include "bvdc.h"
#include "bvdc_priv.h"
#if DCS_SUPPORT
#include "bvdc_dcs_priv.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include "bchp_it_0.h"
#include "bchp_vf_0.h"
#include "bchp_sm_0.h"
#include "bchp_csc_0.h"
#include "bchp_rm_0.h"
#include "bchp_sdsrc_0.h"
#if (BVDC_P_NUM_SHARED_HDSRC > 0)
#include "bchp_hdsrc_0.h"
#endif
#if (BVDC_P_SUPPORT_VEC_SECAM)
#include "bchp_secam_0.h"
#if (BVDC_P_NUM_SHARED_SECAM > 1)
#include "bchp_secam_1.h"
#endif
#endif
#if (BVDC_P_NUM_SHARED_SDSRC > 1)
#include "bchp_sdsrc_1.h"
#endif
#if (BVDC_P_NUM_SHARED_IT > 1)
#include "bchp_it_1.h"
#endif
#if (BVDC_P_NUM_SHARED_VF > 1)
#include "bchp_vf_1.h"
#include "bchp_csc_1.h"
#include "bchp_rm_1.h"
#endif

#if (BVDC_P_NUM_SHARED_SM > 1)
#include "bchp_sm_1.h"
#endif
#include "bchp_vec_cfg.h"
#include "bchp_misc.h"
#include "bchp_hdmi.h"
#include "bchp_hdmi_rm.h"
#include "bchp_hdmi_tx_phy.h"
#include "bchp_dvi_dtg_0.h"
#include "bchp_dvi_dvf_0.h"
#include "bchp_dvi_csc_0.h"
#include "bchp_dtram_0.h"
#include "bchp_video_enc_decim_0.h"
#if (BVDC_P_SUPPORT_VBI_ENC_656)
#include "bchp_itu656_0.h"
#include "bchp_itu656_csc_0.h"
#include "bchp_itu656_dtg_0.h"
#include "bchp_itu656_dvf_0.h"
#endif /* 656 */
#include "bchp_timer.h"

#include "bvdc_resource_priv.h"

#if (BVDC_P_SUPPORT_STG)
#include "bchp_video_enc_stg_0.h"
#endif

#define BVDC_P_SHOW_VEC_MSG               0
#if (BVDC_P_SHOW_VEC_MSG==1)
#define BVDC_P_VEC_MSG    BDBG_ERR
#else
#define BVDC_P_VEC_MSG(a)
#endif

#if (BVDC_P_SUPPORT_IT_VER >= 3)
#define BVDC_P_VEC_STANDALONE_BUG_FIXED        1
#else
#define BVDC_P_VEC_STANDALONE_BUG_FIXED        0
#endif

/****************************************************************
 *  Defines
 ****************************************************************/
/* Original Orthgonal VEC starting from 7420 to 7425Ax */
#define BVDC_P_ORTHOGONAL_VEC_VER_0          (0)

/* 7425 B0 - new vec trigger */
#define BVDC_P_ORTHOGONAL_VEC_VER_1          (1)


/* ---------------------------------------------
 * HDMI_RM revision
 * --------------------------------------------- */
/* 3548 Ax, B0, B1, 3556 Ax, B0, B1
 *  DVPO support
 */
#define BVDC_P_HDMI_RM_VER_0                 (0)

/* 3548 B2 and above, 3556 B2 and above
 *  DVPO support with fixed spread spectrum
 */
#define BVDC_P_HDMI_RM_VER_1                 (1)

/* 7400, 7405, 7335, 7336
 *  DVI support: 65NM 54MHz
 */
#define BVDC_P_HDMI_RM_VER_2                 (2)

/* 7325
 *  DVI support: 65NM 27MHz
 */
#define BVDC_P_HDMI_RM_VER_3                 (3)

/* 7340, 7342, 7550, 7420, 7125, 7468, 7408
 *  DVI support: 65NM 27MHz
 */
#define BVDC_P_HDMI_RM_VER_4                 (4)

/* 7422, 7425, 7358, 7552, 7231, 7346, 7344
 *  DVI support: 40NM 54MHz
 */
#define BVDC_P_HDMI_RM_VER_5                 (5)

/* 7445
 *  DVI support: 40NM 54MHz
 */
#define BVDC_P_HDMI_RM_VER_6                 (6)

#define BVDC_P_SUPPORT_DVPO                   \
	(BVDC_P_SUPPORT_HDMI_RM_VER <= BVDC_P_HDMI_RM_VER_1)

#define BVDC_P_SUPPORT_DVI_65NM               \
	((BVDC_P_SUPPORT_HDMI_RM_VER == BVDC_P_HDMI_RM_VER_2) || \
	 (BVDC_P_SUPPORT_HDMI_RM_VER == BVDC_P_HDMI_RM_VER_3) || \
	 (BVDC_P_SUPPORT_HDMI_RM_VER == BVDC_P_HDMI_RM_VER_4))

#define BVDC_P_SUPPORT_DVI_40NM                   \
	((BVDC_P_SUPPORT_HDMI_RM_VER == BVDC_P_HDMI_RM_VER_5) || \
	 (BVDC_P_SUPPORT_HDMI_RM_VER == BVDC_P_HDMI_RM_VER_6))

#define BVDC_P_MAX_VEC_RUL_ENTRIES        665
#define BVDC_P_RAM_TABLE_SIZE             256
#define BVDC_P_DTRAM_TABLE_SIZE           (128/2)
#define BVDC_P_RAM_TABLE_TIMESTAMP_IDX    (BVDC_P_RAM_TABLE_SIZE - 2)
#define BVDC_P_RAM_TABLE_CHECKSUM_IDX     (BVDC_P_RAM_TABLE_SIZE - 1)
#define BVDC_P_DTRAM_TABLE_TIMESTAMP_IDX  (BVDC_P_DTRAM_TABLE_SIZE - 2)
#define BVDC_P_DTRAM_TABLE_CHECKSUM_IDX   (BVDC_P_DTRAM_TABLE_SIZE - 1)
#define BVDC_P_CCB_TABLE_SIZE             (1)

#define BVDC_P_CSC_TABLE_SIZE             (uint32_t)((BCHP_CSC_0_CSC_COEFF_C23_C22 - BCHP_CSC_0_CSC_MODE)/4+1)
#define BVDC_P_DITHER_TABLE_SIZE          (uint32_t)((BCHP_CSC_0_DITHER_LFSR_INIT - BCHP_CSC_0_DITHER_CONTROL)/4+1)

/* Programming note: tables will actually be one larger than the following. */
#define BVDC_P_VF_TABLE_SIZE              (uint32_t)((BCHP_VF_0_SYNC_TRANS_1 - BCHP_VF_0_FORMAT_ADDER)/4+1)

#define BVDC_P_CHROMA_TABLE_SIZE          (uint32_t)((BCHP_VF_0_CH0_TAP10 - BCHP_VF_0_CH0_TAP1)/4+1)
#define BVDC_P_RM_TABLE_SIZE              (uint32_t)((BCHP_RM_0_INTEGRATOR - BCHP_RM_0_RATE_RATIO)/4+1)
#if (BVDC_P_SUPPORT_IT_VER >= 1)
#define BVDC_P_IT_TABLE_SIZE              (uint32_t)((BCHP_IT_0_STACK_reg_8_9 - BCHP_IT_0_ADDR_0_3)/4+1)
#else
#define BVDC_P_IT_TABLE_SIZE              (uint32_t)((BCHP_IT_0_PCL_5 - BCHP_IT_0_ADDR_0_3)/4+1)
#endif
#define BVDC_P_SM_TABLE_SIZE              (uint32_t)((BCHP_SM_0_COMP_CNTRL - BCHP_SM_0_PG_CNTRL)/4+1)

/* Vec phase adjustment values */
#define BVDC_P_PHASE_OFFSET                0x1d8
#define BVDC_P_THRESHOLD1                  2
#define BVDC_P_THRESHOLD2                  0x15
#define BVDC_P_PHASE_180                   0x200
#define BVDC_P_PHASE_90                    0x100
#define BVDC_P_PHASE_45                    0x080
#define BVDC_P_PHASE_135                   0x180
#define BVDC_P_PHASE_225                   0x280
#define BVDC_P_PHASE_270                   0x300
#define BVDC_P_PHASE_315                   0x380

/* NOTE:
   DAC design spec linearly maps DAC code value between 16 ~ 800 onto
   output voltage level 0 ~ 1000 mV;
   so we have scaling factor for DAC code as:
         (800 - 16) / 1000 = 0.784/mV
   where, value 16 maps sync tip voltage level, and 800 maps to 1000 mV
   peak to peak (picture peak white to sync tip) voltage level;   */
#define BVDC_P_DAC_CODE_MAX_VALUE               (800)
#define BVDC_P_DAC_CODE_MIN_VALUE               (16)
#define BVDC_P_DAC_OUTPUT_RANGE                 (1000) /* 1000 mV */

/* To convert mv into DAC code value relative to sync tip level;
   Note: here 'mv' is relative to sync tip level! */
#define BVDC_P_DAC_CODE_VALUE(mv) \
	((mv) * (BVDC_P_DAC_CODE_MAX_VALUE - BVDC_P_DAC_CODE_MIN_VALUE) / \
	 BVDC_P_DAC_OUTPUT_RANGE + BVDC_P_DAC_CODE_MIN_VALUE)

/* Care needs to be taken for NTSC CVBS/Svideo outputs vs 480i YPrPb/RGB
   or outputs of other formats:
   NTSC CVBS/Svideo outputs have 714mV/286mV Picture/Sync ratio, while
   all other cases have 700mV/300mV Picture/Sync ratio, i.e. picture
   white is at 700mV and sync tip at -300mv, all relative to blank level. */

/* NTSC CVBS/Svideo:
	286mV blank level relative to sync tip; */
#define BVDC_P_DAC_OUTPUT_NTSC_SYNC_LEVEL      (286)
#define BVDC_P_DAC_OUTPUT_NTSC_PEAK_WHITE_LEVEL \
	(BVDC_P_DAC_OUTPUT_RANGE - BVDC_P_DAC_OUTPUT_NTSC_SYNC_LEVEL)

/* All other outputs, including 480i YPrPb/RGB and/or CVBS/Svideo/YPrPb/RGB
   for other formats:
   	300mV blank level relative to sync tip; */
#define BVDC_P_DAC_OUTPUT_SYNC_LEVEL           (300)
#define BVDC_P_DAC_OUTPUT_PEAK_WHITE_LEVEL \
	(BVDC_P_DAC_OUTPUT_RANGE - BVDC_P_DAC_OUTPUT_SYNC_LEVEL)

/* To convert mv into DAC code value relative to blank level;
   Note: here 'mv' is relative to the blank level! */
#define BVDC_P_POS_SYNC_AMPLITUDE_VALUE(mv) \
	((mv) * (BVDC_P_DAC_CODE_MAX_VALUE - BVDC_P_DAC_CODE_MIN_VALUE) \
	/ BVDC_P_DAC_OUTPUT_RANGE)

/* To convert mv into DAC code value relative to sync tip level */
#define BVDC_P_NEG_SYNC_AMPLITUDE_VALUE(mv) BVDC_P_DAC_CODE_VALUE(mv)

#define BVDC_P_NEG_SYNC_TIP_VALUE           BVDC_P_DAC_CODE_MIN_VALUE

/* Check for macro vision type */
#define BVDC_P_MACROVISION_ON_RGB(type) \
	((type) > BVDC_MacrovisionType_eCustomized)

#define BVDC_P_MACROVISION_WITH_2LINES_CS(type) \
	(((type) == BVDC_MacrovisionType_eAgc2Lines) || \
	 ((type) == BVDC_MacrovisionType_eAgc2Lines_Rgb))

#define BVDC_P_MACROVISION_WITH_4LINES_CS(type) \
	(((type) == BVDC_MacrovisionType_eAgc4Lines) || \
	 ((type) == BVDC_MacrovisionType_eAgc4Lines_Rgb))

/* Where to trigger after vsync (line 1) */
#define BVDC_P_TRIGGER_LINE               (3)

/* Bx software workaround, number of vsyncs to wait before enabling
 * DVI input. */
#define BVDC_P_DVIINPUT_WAIT_VSYNCS       (1)

/* DVI DTRam LOCATION! */
#define BVDC_P_DVI_DTRAM_START_ADDR       (0x40)

#define BVDC_P_DISP_GET_TOP_TRIGGER(pDisplay) \
	((pDisplay)->eTopTrigger)

#define BVDC_P_DISP_GET_BOT_TRIGGER(pDisplay) \
	((pDisplay)->eBotTrigger)

/* Analog channel types */
#define BVDC_P_DISP_IS_ANLG_CHAN_CO(pChan, pDispInfo) \
	(((BVDC_P_Dac_Mask_YPrPb & (((pChan)->ulId == 0)? (pDispInfo)->ulAnlgChan0Mask : (pDispInfo)->ulAnlgChan1Mask)) == BVDC_P_Dac_Mask_YPrPb) || \
	 ((BVDC_P_Dac_Mask_RGB & (((pChan)->ulId == 0)? (pDispInfo)->ulAnlgChan0Mask : (pDispInfo)->ulAnlgChan1Mask)) == BVDC_P_Dac_Mask_RGB))

#define BVDC_P_DISP_IS_ANLG_CHAN_CVBS(pChan, pDispInfo) \
	((BVDC_P_Dac_Mask_Cvbs & (((pChan)->ulId == 0)? (pDispInfo)->ulAnlgChan0Mask : (pDispInfo)->ulAnlgChan1Mask)) == BVDC_P_Dac_Mask_Cvbs)

#define BVDC_P_DISP_IS_ANLG_CHAN_SVIDEO(pChan, pDispInfo) \
	((BVDC_P_Dac_Mask_Svideo & (((pChan)->ulId == 0)? (pDispInfo)->ulAnlgChan0Mask : (pDispInfo)->ulAnlgChan1Mask)) == BVDC_P_Dac_Mask_Svideo)

#define BVDC_P_DISP_IS_VALID_DISPOUTPUT_AND_DAC(eDispOutput, eDacOutput)                                                               \
	(((eDisplayOutput == BVDC_DisplayOutput_eComponent) &&                                                                       \
	  ((eDacOutput == BVDC_DacOutput_eY) || (eDacOutput == BVDC_DacOutput_ePr) || (eDacOutput == BVDC_DacOutput_ePb) ||          \
	   (eDacOutput == BVDC_DacOutput_eRed) || (eDacOutput == BVDC_DacOutput_eGreen) || (eDacOutput == BVDC_DacOutput_eBlue) ||   \
	   (eDacOutput == BVDC_DacOutput_eGreen_NoSync))) ? true :                                                                   \
	    (((eDisplayOutput == BVDC_DisplayOutput_eComposite) && (eDacOutput == BVDC_DacOutput_eComposite)) ? true :               \
	     ((eDisplayOutput == BVDC_DisplayOutput_eSVideo) &&                                                                      \
		  ((eDacOutput == BVDC_DacOutput_eSVideo_Luma) || (eDacOutput == BVDC_DacOutput_eSVideo_Chroma)) ? true : false)))

#define BVDC_P_DISP_INVALID_VF_CH (-1)

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


/* IT core register size */
#if (BVDC_P_NUM_SHARED_IT > 1)
#define BVDC_P_VEC_IT_SIZE      (BCHP_IT_1_IT_REV_ID - BCHP_IT_0_IT_REV_ID)
#else
#define BVDC_P_VEC_IT_SIZE   0
#endif

#if (BVDC_P_NUM_SHARED_SDSRC > 1)
/* SDSRC core register size */
#define BVDC_P_VEC_SDSRC_SIZE   (BCHP_SDSRC_1_SRC_REV_ID - BCHP_SDSRC_0_SRC_REV_ID)
#else
#define BVDC_P_VEC_SDSRC_SIZE   0
#endif

#if (BVDC_P_NUM_SHARED_SECAM > 1)
/* SECAM core register size */
#define BVDC_P_VEC_SECAM_SIZE   (BCHP_SECAM_1_SECAM_REV_ID - BCHP_SECAM_0_SECAM_REV_ID)
#else
#define BVDC_P_VEC_SECAM_SIZE   0
#endif

/* VF core register size */
#if (BVDC_P_NUM_SHARED_VF > 1)
#define BVDC_P_VEC_VF_SIZE      (BCHP_VF_1_VF_REV_ID - BCHP_VF_0_VF_REV_ID)
#define BVDC_P_VEC_CSC_SIZE     (BCHP_CSC_1_CSC_REV_ID - BCHP_CSC_0_CSC_REV_ID)
#define BVDC_P_VEC_RM_SIZE      (BCHP_RM_1_CONTROL - BCHP_RM_0_CONTROL)
#else
#define BVDC_P_VEC_VF_SIZE      0
#define BVDC_P_VEC_CSC_SIZE     0
#define BVDC_P_VEC_RM_SIZE      0
#endif

#if (BVDC_P_NUM_SHARED_SM > 1)
#define BVDC_P_VEC_SM_SIZE      (BCHP_SM_1_SM_REV_ID - BCHP_SM_0_SM_REV_ID)
#else
#define BVDC_P_VEC_SM_SIZE      0
#endif

/* Number of channels */
#define BVDC_P_VEC_CH_NUM       3


/****************************************************************
 *  Macros
 ****************************************************************/

/***************** RM clock adjustment macroes *************/
#define BVDC_P_TIMER_FREQ                                    (27000000) /* 27 MHz */

/* alignment threshold in usecs */
#define BVDC_P_USEC_ALIGNMENT_THRESHOLD     (200)

/* in 27 MHz clock cycles */
#define BVDC_P_DISPLAY_ONE_VSYNC_INTERVAL(vrate)    ( \
	((uint32_t)BVDC_P_TIMER_FREQ * (uint32_t)BFMT_FREQ_FACTOR) / (vrate) )
#define BVDC_P_DISPLAY_ALIGNMENT_THRESHOLD    ( \
		BVDC_P_TIMER_FREQ / 1000000 * BVDC_P_USEC_ALIGNMENT_THRESHOLD) /* 200 usecs */

/* one vsync in usecs */
#define BVDC_P_USEC_ONE_VSYNC_INTERVAL(vrate)    ( \
	(1000000 * BFMT_FREQ_FACTOR / (vrate)))

/* Digital trigger */
#define BVDC_P_DISPLAY_USED_DIGTRIG(eMasterTg)    \
		((eMasterTg == BVDC_DisplayTg_eStg0)   || \
		 (eMasterTg == BVDC_DisplayTg_eStg1)   || \
		 (eMasterTg == BVDC_DisplayTg_eStg2)   || \
		 (eMasterTg == BVDC_DisplayTg_eStg3)   || \
		 (eMasterTg == BVDC_DisplayTg_eDviDtg) || \
		 (eMasterTg == BVDC_DisplayTg_e656Dtg))

/* STG trigger transcoding path*/
#define BVDC_P_DISPLAY_USED_STG(eMasterTg)    \
		((eMasterTg == BVDC_DisplayTg_eStg0)   || \
		 (eMasterTg == BVDC_DisplayTg_eStg1)   || \
		 (eMasterTg == BVDC_DisplayTg_eStg2)   || \
		 (eMasterTg == BVDC_DisplayTg_eStg3))

/* STG non realtime */
#define BVDC_P_DISPLAY_NRT_STG(hDisplay)                 \
		(BVDC_P_DISPLAY_USED_STG(hDisplay->eMasterTg) && \
		(hDisplay->stCurInfo.bStgNonRealTime))
/* BAVC_FrameRateCode is full rate (non 1/1001 drop).  */
#define BVDC_P_IS_FULL_FRAMRATE(eFrameRate)    \
		((BAVC_FrameRateCode_e24 == eFrameRate) || \
		 (BAVC_FrameRateCode_e25 == eFrameRate) || \
		 (BAVC_FrameRateCode_e30 == eFrameRate) || \
		 (BAVC_FrameRateCode_e50 == eFrameRate) || \
		 (BAVC_FrameRateCode_e60 == eFrameRate))

/* Customer Fmt */
#define BVDC_P_IS_CUSTOMFMT(eVideoFmt)        \
	((eVideoFmt == BFMT_VideoFmt_eCustom0) || \
	(eVideoFmt == BFMT_VideoFmt_eCustom1)  || \
	(eVideoFmt == BFMT_VideoFmt_eCustom2))
/* set customer format content */
#define BVDC_P_SET_CUSTOMFMT(pNewFmtInfo, pstCustomFmtInfo, pNewCustomFmtInfo) \
	pNewFmtInfo = (const BFMT_VideoInfo *) (pstCustomFmtInfo);                 \
	*pstCustomFmtInfo = *pNewCustomFmtInfo;


#ifndef ABS
#define ABS(a) (((a) < 0) ? -(a) : (a))
#endif

/* This value needs to be further tuned.
 *
 * There are conflict interest here. Setting this gap to smaller value
 * will enable faster phase adjustment. However this may cause TV resync,
 * especially for composite output. Setting this gap to bigger value
 * will help the resync issue, however the adjustment process will take
 * longer time to finish.
 *
 * Maybe we should extend our API to allow user control this value.
 */
#define BVDC_P_DISPLAY_SKIP_REPEAT_GAP 70

/***************************************************************************
 *  Private Enums
 ***************************************************************************/
/* Contains VEC path selected by VDC */
typedef enum
{
	BVDC_P_eVecPrimary = 0,
	BVDC_P_eVecSecondary,
	BVDC_P_eVecTertiary,
	BVDC_P_eVecBypass0
} BVDC_P_VecPath;

/* DAC Output */
typedef enum
{
	BVDC_P_Output_eYQI=0,        /* SVideo and/or CVBS for NTSC */
	BVDC_P_Output_eYQI_M,        /* SVideo and/or CVBS for NTSC_J */
	BVDC_P_Output_eYUV,          /* SVideo and/or CVBS for Pal */
	BVDC_P_Output_eYUV_N,        /* SVideo and/or CVBS for Pal_N/M */
	BVDC_P_Output_eYUV_NC,       /* SVideo and/or CVBS for PAL_NC */
#if BVDC_P_SUPPORT_VEC_SECAM
	BVDC_P_Output_eYDbDr_LDK,    /* SVideo and/or CVBS for SECAM_L/D/K */
	BVDC_P_Output_eYDbDr_BG,     /* SVideo and/or CVBS for SECAM_B/G */
	BVDC_P_Output_eYDbDr_H,      /* SVideo and/or CVBS for SECAM_H */
#endif
	/* Folks, would you please put all of the component and RGB formats below
	 * this line. Thanks.
	 */
	BVDC_P_Output_eSDYPrPb,      /* SYPrPb, CYPrPb, YPrPb */
	BVDC_P_Output_eRGB,          /* SRGB, CRGB, SCRGB, RGB */
	BVDC_P_Output_eHDYPrPb,      /* HDYPrPb */
	BVDC_P_Output_eHsync,        /* HSYNC */
	BVDC_P_Output_eUnknown,
	BVDC_P_Output_eNone,
	BVDC_P_Output_eMax
} BVDC_P_Output;

/* Output Filter Types */
typedef enum
{
	BVDC_P_OutputFilter_eHD=0,
	BVDC_P_OutputFilter_eED,
	BVDC_P_OutputFilter_eSDYPrPb,
	BVDC_P_OutputFilter_eYQI,
	BVDC_P_OutputFilter_eYUV,
	BVDC_P_OutputFilter_eSECAM,
	BVDC_P_OutputFilter_eRGB,
	BVDC_P_OutputFilter_eHsync,
	BVDC_P_OutputFilter_eUnknown,
	BVDC_P_OutputFilter_eNone,
	BVDC_P_OutputFilter_eMax
} BVDC_P_OutputFilter;

typedef enum
{
	BVDC_P_ItState_eActive = 0, /* Active, no change necessary */
	BVDC_P_ItState_eNotActive,  /* Initial state. Vecs are not running */
	BVDC_P_ItState_eSwitchMode, /* Mode switch request */
	BVDC_P_ItState_eNewMode     /* Vec switched to new mode. */
} BVDC_P_ItState;

typedef enum
{
	BVDC_P_Dac_Mask_Cvbs   = 0x001,
	BVDC_P_Dac_Mask_Luma   = 0x002,
	BVDC_P_Dac_Mask_Chroma = 0x004,
	BVDC_P_Dac_Mask_Y      = 0x008,
	BVDC_P_Dac_Mask_Pr     = 0x010,
	BVDC_P_Dac_Mask_Pb     = 0x020,
	BVDC_P_Dac_Mask_R      = 0x040,
	BVDC_P_Dac_Mask_G      = 0x080,
	BVDC_P_Dac_Mask_B      = 0x100
} BVDC_P_DAC_Mask;

#define BVDC_P_Dac_Mask_Svideo (BVDC_P_Dac_Mask_Luma | BVDC_P_Dac_Mask_Chroma)
#define BVDC_P_Dac_Mask_YPrPb  (BVDC_P_Dac_Mask_Y | BVDC_P_Dac_Mask_Pr | BVDC_P_Dac_Mask_Pb)
#define BVDC_P_Dac_Mask_RGB    (BVDC_P_Dac_Mask_R | BVDC_P_Dac_Mask_G | BVDC_P_Dac_Mask_B)
#define BVDC_P_Dac_Mask_SD     (BVDC_P_Dac_Mask_Svideo | BVDC_P_Dac_Mask_Cvbs)
#define BVDC_P_Dac_Mask_HD     (BVDC_P_Dac_Mask_YPrPb | BVDC_P_Dac_Mask_RGB)

typedef enum
{
	BVDC_P_TimeStamp_eStart = 0, /* Initial state */
	BVDC_P_TimeStamp_eRul,       /* build RUL for snap shotting time stamp */
	BVDC_P_TimeStamp_eAvail      /* Time stamp is ready for use */
} BVDC_P_TimeStampState;


typedef enum
{
	BVDC_P_Alignment_eInactive = 0,
	BVDC_P_Alignment_eWaitTimeStamp,
	BVDC_P_Alignment_eActive,
	BVDC_P_Alignment_eDone
} BVDC_P_AlignmentState;

typedef enum
{
	BVDC_P_Slave_eInactive = 0,
	BVDC_P_Slave_eEnable,
	BVDC_P_Slave_eConnectSrc,
	BVDC_P_Slave_eAttached
} BVDC_P_SlaveState;

typedef enum
{
	BVDC_P_DisplayResource_eInactive = 0,
	BVDC_P_DisplayResource_eResInactive,
	BVDC_P_DisplayResource_eCreate,
	BVDC_P_DisplayResource_eActivating,
	BVDC_P_DisplayResource_eActive,
	BVDC_P_DisplayResource_eDestroy,
	BVDC_P_DisplayResource_eShuttingdown
} BVDC_P_DisplayResourceState;


/***************************************************************************
 * Display Context
 ***************************************************************************/
typedef struct
{
	const uint32_t   *pulCCBTbl;
	const char       *pchTblName;
} BVDC_P_FormatCCBTbl;

typedef struct
{
	uint32_t                    bPsAgc            : 1; /* Pseudo-Sync/AGC      */
	uint32_t                    bBp               : 1; /* Back Porch           */
	uint32_t                    bCs               : 1; /* Color-Stripes        */
	uint32_t                    bCycl             : 1; /* AGC Cyclic variation */
	uint32_t                    bHamp             : 1; /* H-sync amaplitude reduction outside VBI */
	uint32_t                    bVamp             : 1; /* V-sync amaplitude reduction outside VBI */
	uint32_t                    bRgb              : 1; /* RGB on/off */
} BVDC_P_MacrovisionCtrlBits;

typedef union
{
	/* some events such as format change which will cause core reset and restart
	 * need to be handled first.
	 */
	struct
	{
		uint32_t                bChan0            : 1; /* setup/destroy analog chan 0 */
		uint32_t                bChan1            : 1; /* setup/destroy analog chan 1 */
		uint32_t                bTiming           : 1; /* new output timing format */
		uint32_t                bSoftTiming       : 1; /* Like bTiming, but no need to reload VEC IT microcode */
		uint32_t                b3DSetting        : 1; /* new 3D setting */

		uint32_t                bDacSetting       : 1; /* new DAC settings */
		uint32_t                bTimeBase         : 1; /* new time base setting */
		uint32_t                bCallback         : 1; /* new callback mask settings */
		uint32_t                bCallbackFunc     : 1; /* new callback function */
		uint32_t                bWidthTrim        : 1; /* 704-sample vs. 720-sample */

		uint32_t                bInputCS          : 1; /* input color space */
		uint32_t                b240p             : 1; /* 240p format */
		uint32_t                bPhaseAdjustment  : 1; /* VDEC->VEC phase adjustment */
		uint32_t                bSrcFrameRate     : 1; /* Source frame rate */
		uint32_t                bFrameDrop        : 1; /* rate change 59.94Hz vs 60.00Hz */

#if (BVDC_P_SUPPORT_RFM_OUTPUT != 0)
		uint32_t                bRfm              : 1; /* new configuration */
#endif
		uint32_t                bHdmiEnable       : 1; /* enable/disable HDMI output */
		uint32_t                bHdmiCsc          : 1; /* new HDMI output CSC matrix settings */
#if (BVDC_P_SUPPORT_ITU656_OUT != 0)
		uint32_t                b656Enable        : 1; /* enable/disable ITU656 output */
#endif
		uint32_t                bMpaaComp         : 1; /* new component path MPAA settings  */

		uint32_t                bMpaaHdmi         : 1; /* new HDMI path MPAA settings */
		uint32_t                bTimeStamp        : 1; /* Take a time stamp for alignment */
		uint32_t                bAlignment        : 1; /* start/stop alignment */
		uint32_t                bHdmiDroplines    : 1; /* new drop-line setting for HDMI */
		uint32_t                bHdmiXvYcc        : 1; /* new HDMI XvYcc setting */

		uint32_t                bHdmiSyncOnly     : 1; /* Turn on/off HDMI sync only feature */
		uint32_t                bHdmiRmChanged    : 1; /* HDMI RM setting changed */
		uint32_t                bAspRatio         : 1; /* aspect ratio might changed */
#if (BVDC_P_SUPPORT_STG != 0)
		uint32_t                bStgEnable        : 1; /* Stg enable/disable */
#endif
		uint32_t                bVfFilter         : 1; /* user VF filters */

		uint32_t                bOutputMute       : 1; /* output Mute */
		uint32_t                bMiscCtrl         : 1; /* Combined various dirty bits does not trigger together. */
	} stBits;

	uint32_t aulInts[BVDC_P_DIRTY_INT_ARRAY_SIZE];
} BVDC_P_Display_DirtyBits;

typedef BERR_Code (* BVDC_Display_ValidateSetting)(BVDC_Display_Handle);
typedef void (* BVDC_Display_CopySetting)(BVDC_Display_Handle);
typedef void (* BVDC_Display_ApplySetting)(BVDC_Display_Handle, BVDC_P_ListInfo *, BAVC_Polarity);

typedef struct
{
	BVDC_Display_ValidateSetting validateSettingHandler;
	BVDC_Display_CopySetting     copySettingHandler;
	BVDC_Display_ApplySetting    applySettingHandler;
} BVDC_Display_EventHandler;

typedef struct
{
	uint32_t                    ulSavRemove;
	uint32_t                    ulSavReplicate;
	uint32_t                    ulEavPredict;
} BVDC_P_Display_ShaperSettings;

typedef struct
{
	uint32_t                    ulMin;
	uint32_t                    ulMax;
	BVDC_P_CscCoeffs            stCscCoeffs;
} BVDC_P_DisplayCscMatrix;

#if (BVDC_P_SUPPORT_DVI_40NM)
typedef struct BVDC_P_RateInfo
{
	/* Use for searching a matching one! */
	uint32_t                    ulPixelClkRate;
	BAVC_HDMI_BitsPerPixel      eHdmiColorDepth;      /* 24, 30, 36, or 48 (future) */
	BAVC_HDMI_PixelRepetition   eHdmiPixelRepetition; /* 2x or 4x */
	uint32_t                    ulDenominator;
	uint32_t                    ulNumerator;
	uint32_t                    ulSampleInc;
	uint32_t                    ulOffset;
	uint32_t                    ulShift;
	uint32_t                    ulRmDiv;
	uint32_t                    ulVcoRange;
	uint32_t                    ulPxDiv;
	uint32_t                    ulKP;
	uint32_t                    ulKI;
	uint32_t                    ulKA;
	const char                 *pchRate;
} BVDC_P_RateInfo;

#else
typedef struct BVDC_P_RateInfo
{
	/* Use for searching a matching one! */
	uint32_t                    ulPixelClkRate;
	BAVC_HDMI_BitsPerPixel      eHdmiColorDepth;      /* 24, 30, 36, or 48 (future) */
	BAVC_HDMI_PixelRepetition   eHdmiPixelRepetition; /* 2x or 4x */
	uint32_t                    ulDenominator;
	uint32_t                    ulNumerator;
	uint32_t                    ulSampleInc;
	uint32_t                    ulOffset;
	uint32_t                    ulShift;
	uint32_t                    ulRmDiv;
	uint32_t                    ulVcoRange;
	uint32_t                    ulPxDiv;
	uint32_t                    ulFeedbackPreDiv;
	uint32_t                    ulInputPreDiv;
	uint32_t                    ulKVcoXs;
	const char                 *pchRate;
} BVDC_P_RateInfo;

#endif

typedef struct
{
	bool                        bWidthTrimmed;
	BVDC_PhaseAdjustMode        ePhaseAdjustMode;
	uint32_t                    ulVdecPhase;
	bool                        bFullRate;
	BVDC_P_CmpColorSpace        eCmpColorSpace;
	bool                        b240pMode;

} BVDC_P_Display_SrcInfo;

/*
 * Analog channel state and resources acquired
 */
typedef struct
{
	uint32_t                    ulId;
	bool                        bEnable;    /* on or off */
	bool                        bTearDown;  /* tearing down flag */
	BVDC_P_DisplayResourceState eState;     /* Analog channel state */
	uint32_t                    ulIt;
	uint32_t                    ulVf;
	uint32_t                    ulSecam;
	uint32_t                    ulSecam_HD;
	uint32_t                    ulPrevSecam;
	uint32_t                    ulPrevSecam_HD;
	uint32_t                    ulSdsrc;
	uint32_t                    ulHdsrc;
	uint32_t                    ulPrevSdsrc;
	uint32_t                    ulPrevHdsrc;
	uint32_t                    ulDac_0;
	uint32_t                    ulDac_1;
	uint32_t                    ulDac_2;

	BVDC_P_DisplayCscMatrix     stCscMatrix;

	/* VF filters */
	const uint32_t             *apVfFilter[BVDC_P_VEC_CH_NUM];
} BVDC_P_DisplayAnlgChan;

typedef struct
{
	BVDC_P_CmpColorSpace        eCmpColorSpace;
	BVDC_P_Output               eAnlg_0_OutputColorSpace;
	BVDC_P_Output               eAnlg_1_OutputColorSpace;
	uint32_t                    ulAnlgChan0Mask;
	uint32_t                    ulAnlgChan1Mask;

	BVDC_DacOutput              aDacOutput[BVDC_P_MAX_DACS];
	BAVC_MatrixCoefficients     eHdmiOutput;
	const BFMT_VideoInfo       *pFmtInfo;
	BFMT_VideoInfo              stCustomFormatInfo;
	uint32_t                    ulCheckSum;
	uint32_t                    ulVertFreq;
	uint32_t                    ulHeight;
	uint32_t                    ulVdecPhase;
	BVDC_PhaseAdjustMode        ePhaseAdjustMode;

	/* 240p Mode */
	bool                        b240pMode;

	/* User Dvo's CSC */
	bool                        bUserCsc;
	uint32_t                    ulUserShift;
	int32_t                     pl32_Matrix[BVDC_CSC_COEFF_COUNT];

	/* Misc shares bits for 656/Hdmi/Rfm */
	BVDC_P_VecPath              e656Vecpath;        /* 656 path output */
	BVDC_P_VecPath              eHdmiVecpath;       /* Prim/Sec, Bypass=off both */
	BVDC_P_VecPath              eRfmVecpath;        /* Prim/Sec, Bypass=off both */

	bool                        bFullRate;
	BAVC_Timebase               eTimeBase;          /* timebase for this display */
	BAVC_HDMI_BitsPerPixel      eHdmiColorDepth;      /* 24, 30, 36, or 48 (future) */
	BAVC_HDMI_PixelRepetition   eHdmiPixelRepetition; /* 2x or 4x */

	uint32_t                    uiNumDacsOn;        /* #Dacs enabled for this display */

	/* RFM */
	BVDC_RfmOutput              eRfmOutput;
	uint32_t                    ulRfmConst;         /* constant value to RF port */

	bool                        bEnableHdmi;
	bool                        bXvYcc;             /* hdmi XvYcc output */
	bool                        bEnable656;

	uint32_t                    aulHdmiDropLines[BFMT_VideoFmt_eMaxCount];

	/* MPAA decimation bit field */
	uint32_t                    aulEnableMpaaDeci[BVDC_MpaaDeciIf_eUnused];

	/* Generic callback */
	BVDC_CallbackFunc_isr       pfGenericCallback;
	void                       *pvGenericParm1;
	int                         iGenericParm2;
	BAVC_VdcDisplay_Info        stRateInfo;
	BVDC_Display_CallbackSettings   stCallbackSettings;

	/*
	 * Copy protection state
	 */

	/* MV N0 control bits */
	BVDC_P_MacrovisionCtrlBits  stN0Bits;
	/* Macrovision type */
	BVDC_MacrovisionType        eMacrovisionType;

#if DCS_SUPPORT
	/* DCS type */
	BVDC_DCS_Mode               eDcsMode;
#endif

	/* 704 vs. 720 samples */
	bool                        bWidthTrimmed;

	/* Force vec to run at 59.94Hz, 29.97Hz, etc. */
	bool                        bForceFrameDrop;

	/* PR28836: DVO H/V/De sync polarity. */
	BVDC_Display_DvoSettings    stDvoCfg;

	/* Display alignment settings */
	BVDC_Display_Handle             hTargetDisplay;
	BVDC_Display_AlignmentSettings  stAlignCfg;

	/* color adjustment attributes */
	int32_t                     lDvoAttenuationR;
	int32_t                     lDvoAttenuationG;
	int32_t                     lDvoAttenuationB;
	int32_t                     lDvoOffsetR;
	int32_t                     lDvoOffsetG;
	int32_t                     lDvoOffsetB;

	bool                        bCvbs;
	bool                        bSvideo;
	bool                        bHsync;
	bool                        bRgb;
	bool                        bYPrPb;
	bool                        abOutputMute[BVDC_DisplayOutput_e656 + 1];

	bool                        bMultiRateAllow;

	bool                        bErrorLastSetting;

	/* display aspect ratio */
	BFMT_AspectRatio            eAspectRatio;
	uint32_t                    uiSampleAspectRatioX;
	uint16_t                    uiSampleAspectRatioY;
	BVDC_P_ClipRect             stAspRatRectClip;

	/* STG mode RealTime/ NonRealTime*/
	bool                        bEnableStg;
	bool                        bStgNonRealTime;
#if (BVDC_P_SUPPORT_IT_VER >= 2)
	uint32_t                    ulTriggerModuloCnt;
#endif
	bool                        bBypassVideoProcess;

	BFMT_Orientation            eOrientation;
	BVDC_3dSourceBufferSelect   e3dSrcBufSel;

	/* user VF filters */
	bool                        abUserVfFilterCo[BVDC_P_VEC_CH_NUM];
	bool                        abUserVfFilterCvbs[BVDC_P_VEC_CH_NUM];
	uint32_t                    aaulUserVfFilterCo[BVDC_P_VEC_CH_NUM][BVDC_P_CHROMA_TABLE_SIZE];
	uint32_t                    aaulUserVfFilterCvbs[BVDC_P_VEC_CH_NUM][BVDC_P_CHROMA_TABLE_SIZE];

	/* Artificial vsync */
	bool                        bArtificialVsync;
	uint32_t                    ulArtificialVsyncRegAddr;
	uint32_t                    ulArtificialVsyncMask;

	/* dirty bits */
	BVDC_P_Display_DirtyBits    stDirty;
} BVDC_P_DisplayInfo;

/*
 * DVI channel state and resources acquired
 */
typedef struct
{
	bool              bEnable;          /* on or off */
	uint32_t          ulDvi;
} BVDC_P_DisplayDviChan;

/*
 * 656 channel state and resources acquired
 */
typedef struct
{
	bool              bEnable;          /* on or off */
	uint32_t          ul656;
} BVDC_P_Display656Chan;

/*
* STG channel state and resources acquired
*/
typedef struct
{
	bool                      bEnable;          /*on or off */
	bool                      bStgNonRealTime;  /* real time mode or not*/
	uint32_t                  ulStg;
	uint32_t                  ulMBoxAddr;       /* Mailbox physical address for ViCE2*/
} BVDC_P_DisplayStgChan;

/*
 * MPAA state and resources acquired
 */
typedef struct
{
	BVDC_P_DisplayResourceState eState;  /* MPAA state */
	uint32_t                    ulHwId;
} BVDC_P_DisplayMpaa;

typedef struct BVDC_P_DisplayContext
{
	BDBG_OBJECT(BVDC_DSP)

	BVDC_P_DisplayInfo          stNewInfo;           /* new(to-be apply) display info */
	BVDC_P_DisplayInfo          stCurInfo;           /* current(hw-applied) display info */
	BVDC_P_Display_DirtyBits    stPrevDirty;         /* Previous dirty bits. In case
	                                                  * last RUL didn't get executed, we will
	                                                  * try again.
	                                                  */

	BVDC_DisplayId              eId;                 /* might be different from cmp id */
	BVDC_P_State                eState;              /* Context state. */
	BVDC_Handle                 hVdc;                /* From which main VDC handle */
	BVDC_P_ItState              eItState;            /* Current Vec state */
	BVDC_Compositor_Handle      hCompositor;         /* Conntected to compositor */
	uint32_t                    ulRdcVarAddr;        /* Temp RDC var addr use for format change. */
	uint32_t                    ulDisplayCnt;        /* count number of displays */
	uint32_t                    ulStgRegOffset;      /* Stg instance offset */

	bool                        bHdCap;              /* is HD capable */
	bool                        bSecamCap;           /* is Secam capable */

	/* Display channels */
	BVDC_P_DisplayDviChan       stDviChan;           /* DVI channel */
#if BVDC_P_SUPPORT_SEAMLESS_ATTACH
	BVDC_P_SlaveState           eDviSlaveState;      /* State used when attaching DVI slave to master path */
#endif
	BVDC_P_DisplayResourceState eDviState;           /* DVI channel state */
	BVDC_P_Display656Chan       st656Chan;           /* 656 channel */
#if BVDC_P_SUPPORT_SEAMLESS_ATTACH
	BVDC_P_SlaveState           e656SlaveState;      /* State used when attaching ITU656 slave to master path */
#endif
	BVDC_P_DisplayResourceState e656State;           /* 656 channel state */

#if BVDC_P_SUPPORT_SEAMLESS_ATTACH
	BVDC_P_SlaveState           eStgSlaveState;      /* State used when attaching ITU656 slave to master path */
#endif
	BVDC_P_DisplayResourceState eStgState;           /* 656 channel state */
	BVDC_P_DisplayStgChan       stStgChan;           /* STG channel */
	BVDC_P_DisplayAnlgChan      stAnlgChan_0;        /* Analog channel 0 */
	BVDC_P_DisplayAnlgChan      stAnlgChan_1;        /* Analog channel 1 */
	bool                        bAnlgEnable;         /* Ananlog master */
	BVDC_P_DisplayResourceState eRfmState;           /* Rfm channel state */

	/* Master timing generator among all the channels.
	 * Other channels should slave to it.
	 */
	BVDC_DisplayTg              eMasterTg;
	BRDC_Trigger                eTopTrigger;
	BRDC_Trigger                eBotTrigger;

	/* DVO, and Main CSC */
	BVDC_P_DisplayCscMatrix     stDvoCscMatrix;
	BVDC_P_DisplayCscMatrix     st656CscMatrix;

	/* Event to nofify that changes has been applied to hardware. */
	BKNI_EventHandle            hAppliedDoneEvent;
	bool                        bSetEventPending;

	bool                        bRateManagerUpdated;

	const BFMT_VideoInfo       *pPrevFmtInfo;
	BFMT_VideoInfo              stPrevCustomFormatInfo;
	/* MPAA decimation supported interface port mask */
	uint32_t                    aulMpaaDeciIfPortMask[BVDC_MpaaDeciIf_eUnused];
	BVDC_P_DisplayMpaa          stMpaaHdmi; /* mpaa state in hdmi chan */
	BVDC_P_DisplayMpaa          stMpaaComp; /* state in component chan */

	/* Is this a bypass display? which means no VEC analog output. */
	bool                        bIsBypass;

	/* Internal VDC or App handed down. */
	BVDC_Heap_Handle            hHeap;

	/* Specific flavor of 480P output */
	bool bArib480p;

	/* Game mode tracking window */
	BVDC_Window_Handle          hWinGameMode;
	const uint32_t             *pRmTable;  /* VEC RM */
	bool                        bRmAdjusted;

	/* alignment Timestamps */
	BTMR_TimerHandle            hTimer;
	BTMR_TimerRegisters         stTimerReg;
	uint32_t                    ulScratchTsAddr;
#if (BVDC_P_STG_RUL_DELAY_WORKAROUND)
	uint32_t                    ulScratchDummyAddr;
#endif
	BAVC_Polarity               eTimeStampPolarity;
	BVDC_P_TimeStampState       eTimeStampState;
	BVDC_P_AlignmentState       eAlignmentState;
	uint32_t                    ulAlignSlaves;
	bool                        bAlignAdjusting;

	/* Dither */
	BVDC_P_DitherSetting        stCscDither;
	BVDC_P_DitherSetting        stDviDither;
	BVDC_P_DitherSetting        st656Dither;
	BVDC_Display_CallbackData   stCallbackData;

	/* display pixel aspect ratio */
	uintAR_t                    ulPxlAspRatio;     /* PxlAspR_int.PxlAspR_frac */
	uint32_t                    ulPxlAspRatio_x_y; /* PxlAspR_x<<16 | PxlAspR_y */

#if DCS_SUPPORT
	struct BVDC_P_DCS_UpdateInfo stDcsConfirmed;
	struct BVDC_P_DCS_UpdateInfo stDcsRequested;
	bool                         bDcsNeedsLock;
	uint32_t					 ulDcsSequence;
#if 0
	/* Debug code */
	uint32_t                     ulDcsScratch[4];
#endif
#endif

} BVDC_P_DisplayContext;

/***************************************************************************
 * Private tables
 ***************************************************************************/
/* Always available */
extern const uint32_t   BVDC_P_aaulPrimaryDacTable[][BVDC_P_MAX_DACS];

#if BVDC_P_SUPPORT_SEC_VEC
extern const uint32_t   BVDC_P_aaulSecondaryDacTable[][BVDC_P_MAX_DACS];
#endif

#if BVDC_P_SUPPORT_TER_VEC
extern const uint32_t   BVDC_P_aaulTertiaryDacTable[][BVDC_P_MAX_DACS];
#endif

extern const BVDC_Display_EventHandler  BVDC_P_astDisplayEventHndlTbl[];


/***************************************************************************
 * Display private functions
 ***************************************************************************/
BERR_Code BVDC_P_Display_Create
	( BVDC_P_Context                  *pVdc,
	  BVDC_Display_Handle             *phDisplay,
	  BVDC_DisplayId                   eId);

void BVDC_P_Display_Destroy
	( BVDC_Display_Handle              hDisplay );

void BVDC_P_Display_Init
	( BVDC_Display_Handle              hDisplay );

void BVDC_P_Vec_BuildRul_isr
	( BVDC_Display_Handle              hDisplay,
	  BVDC_P_ListInfo                 *pList,
	  BAVC_Polarity                    eFieldPolarity );

BERR_Code BVDC_P_Display_ValidateChanges
	( BVDC_Display_Handle              ahDisplay[] );

void BVDC_P_Display_ApplyChanges_isr
	( BVDC_Display_Handle              hDisplay);

void BVDC_P_Display_AbortChanges
	( BVDC_Display_Handle              hDisplay);

bool BVDC_P_Display_FindDac
	( BVDC_Display_Handle              hDisplay,
	  BVDC_DacOutput                   eDacOutput);

/* Helper functions. */
#if (BSTD_CPU_ENDIAN == BSTD_ENDIAN_BIG)
#define BVDC_P_DISPLAY_DIRTY_MASK_SHIFT(idx) (1 << (BVDC_P_DIRTY_INT_ARRAY_ELEMENT_SIZE - 1 - (idx % BVDC_P_DIRTY_INT_ARRAY_ELEMENT_SIZE)))
#else
#define BVDC_P_DISPLAY_DIRTY_MASK_SHIFT(idx) (1 << idx % BVDC_P_DIRTY_INT_ARRAY_ELEMENT_SIZE)
#endif

#define BVDC_P_DISPLAY_IS_BIT_DIRTY(pDirty, iIdx)    ((*(pDirty)).aulInts[iIdx/BVDC_P_DIRTY_INT_ARRAY_ELEMENT_SIZE] & BVDC_P_DISPLAY_DIRTY_MASK_SHIFT(iIdx))
#define BVDC_P_DISPLAY_CLEAR_DIRTY_BIT(pDirty, iIdx) ((*(pDirty)).aulInts[iIdx/BVDC_P_DIRTY_INT_ARRAY_ELEMENT_SIZE] &= ~(BVDC_P_DISPLAY_DIRTY_MASK_SHIFT(iIdx)))
#define BVDC_P_DISPLAY_SET_DIRTY_BIT(pDirty, iIdx)   ((*(pDirty)).aulInts[iIdx/BVDC_P_DIRTY_INT_ARRAY_ELEMENT_SIZE] |= BVDC_P_DISPLAY_DIRTY_MASK_SHIFT(iIdx))

void BVDC_P_ResetAnalogChanInfo
	(BVDC_P_DisplayAnlgChan *pstChan);

BERR_Code BVDC_P_AllocITResources
	( BVDC_P_Resource_Handle           hResource,
	  BVDC_DisplayId                   eDisplayId,
	  BVDC_P_DisplayAnlgChan          *pstChan,
	  uint32_t                         ulIt );

BERR_Code BVDC_P_AllocAnalogChanResources
	( BVDC_P_Resource_Handle           hResource,
	  BVDC_DisplayId                   eDisplayId,
	  BVDC_P_DisplayAnlgChan          *pstChan,
	  bool                             bHdMust,
	  bool                             bHdRec,
	  bool                             bSecamCap );

void BVDC_P_FreeITResources
	(BVDC_P_Resource_Handle            hResource,
	 BVDC_P_DisplayAnlgChan           *pstChan );

void BVDC_P_FreeAnalogChanResources
	(BVDC_P_Resource_Handle            hResource,
	 BVDC_P_DisplayAnlgChan           *pstChan);

void BVDC_P_Reset656ChanInfo
	(BVDC_P_Display656Chan *pstChan);

#if BVDC_P_SUPPORT_ITU656_OUT
BERR_Code BVDC_P_Alloc656ChanResources
	(BVDC_P_Resource_Handle hResource,
	 BVDC_DisplayId eDisplayId,
	 BVDC_P_Display656Chan *pstChan,
	 uint32_t eSrcId);

void BVDC_P_Free656ChanResources
	(BVDC_P_Resource_Handle hResource,
	 BVDC_P_Display656Chan *pstChan);
#endif

void BVDC_P_ResetDviChanInfo
	(BVDC_P_DisplayDviChan *pstChan);

BERR_Code BVDC_P_AllocDviChanResources
	(BVDC_P_Resource_Handle hResource,
	 BVDC_DisplayId eDisplayId,
	 BVDC_P_DisplayDviChan *pstChan,
	 uint32_t eSrcId);

void BVDC_P_FreeDviChanResources
	(BVDC_P_Resource_Handle hResource,
	 BVDC_P_DisplayDviChan *pstChan);

BERR_Code BVDC_P_AllocDacResources
	( BVDC_P_Resource_Handle     hResource,
	  BVDC_P_DisplayAnlgChan    *pstChan,
	  uint32_t                   ulDacId );

BERR_Code BVDC_P_FreeDacResources
	( BVDC_P_Resource_Handle     hResource,
	  BVDC_P_DisplayAnlgChan    *pstChan);

void  BVDC_P_Display_SetSourceInfo
	( BVDC_Display_Handle  hDisplay,
	  const BVDC_P_Display_SrcInfo *pSrcInfo );

uint32_t BVDC_P_GetPosSyncValue
	( BVDC_P_DisplayContext     *pDisplay,
	  uint32_t                 **ppulRul );

#if (BVDC_P_SUPPORT_STG)
BERR_Code BVDC_P_AllocStgChanResources
	( BVDC_P_Resource_Handle		   hResource,
	  BVDC_DisplayId				   eDisplayId,
	  BVDC_P_DisplayStgChan 		   *pstStgChan);
void BVDC_P_FreeStgChanResources
	(BVDC_P_Resource_Handle hResource,
	 BVDC_P_DisplayStgChan *pstChan);
void BVDC_P_ResetStgChanInfo
	(BVDC_P_DisplayStgChan			  *pstStgChan);
void BVDC_P_Display_EnableSTGTriggers_isr
	( BVDC_Display_Handle              hDisplay,
	bool                             bEnable );

BERR_Code BVDC_P_Display_Validate_Stg_Setting
	( BVDC_Display_Handle              hDisplay );

void BVDC_P_Display_Copy_Stg_Setting
	( BVDC_Display_Handle              hDisplay );

void BVDC_P_Display_Apply_Stg_Setting
	( BVDC_Display_Handle              hDisplay,
	  BVDC_P_ListInfo                 *pList,
	  BAVC_Polarity                    eFieldPolarity );

void BVDC_P_ProgrameStgMBox_isr
	( BVDC_Display_Handle              hDisplay,
	  BVDC_P_ListInfo                 *pList,
	  BAVC_Polarity                    eFieldPolarity );

void BVDC_P_ProgramStgChan
	( BVDC_Display_Handle              hDisplay,
	  BVDC_P_ListInfo                 *pList );

void BVDC_P_STG_Build_RM_isr
	( BVDC_Display_Handle              hDisplay,
	  BVDC_P_ListInfo                 *pList  );

void BVDC_P_SetupStg_isr
	  (uint32_t                       ulRegOffset,
	   BVDC_P_ListInfo               *pList );

void BVDC_P_TearDownStgChan
	  (BVDC_Display_Handle            hDisplay,
	   BVDC_P_ListInfo                *pList );

void BVDC_P_ConnectStgSrc
	(BVDC_Display_Handle             hDisplay,
	 BVDC_P_ListInfo                *pList );
#endif
#if (BVDC_P_STG_RUL_DELAY_WORKAROUND)
void BVDC_P_STG_DelayRUL_isr
	( BVDC_Display_Handle              hDisplay,
	  BVDC_P_ListInfo                 *pList);
#endif

/* Not used in orthogonal VEC */
#if (0) /** { **/
BERR_Code BVDC_P_ChangeMvType
	( BVDC_P_DisplayContext           *pDisplay,
	  uint32_t                       **ppulRul );
#endif /** } 0 **/

uint32_t BVDC_P_Display_Modify_SYNC_TRANS_0 (
	BVDC_P_DisplayInfo               *pNewInfo,
	uint32_t                          ulVfSTRegVal );

void BVDC_P_GetNegSyncValue
	( BVDC_P_DisplayInfo              *pDispInfo,
	  BVDC_P_Output                    eOutputColorSpace,
	  uint32_t*                        ulRegVal,
	  uint32_t*                        ulRegValEx);

uint32_t BVDC_P_GetFmtAdderValue
	( BVDC_P_DisplayInfo              *pDispInfo );

BERR_Code BVDC_P_ValidateMacrovision
	( BVDC_P_DisplayContext           *pDisplay );

void BVDC_P_Vec_Build_SyncPCL_isr
	( BVDC_P_DisplayContext           *pDisplay,
	  BVDC_P_ListInfo                 *pList );

void BVDC_P_Vec_Init_Misc_isr
	( BVDC_P_Context                  *pVdc );

void BVDC_P_Vec_Update_OutMuxes_isr
	( BVDC_P_Context                  *pVdc );

void BVDC_P_Vec_BuildVsync_isr
	( BVDC_Display_Handle              hDisplay,
	  BVDC_P_ListInfo                 *pList,
	  BAVC_Polarity                    eFieldPolarity );

void BVDC_P_Display_EnableTriggers_isr
	( BVDC_Display_Handle              hDisplay,
	  bool                             bEnable );

void BVDC_P_ResetVec
	( BVDC_P_Context                  *pVdc );

void BVDC_P_Display_Alignment
	( BVDC_Display_Handle              hDisplay,
	  BVDC_P_ListInfo                 *pList);

void BVDC_P_Display_GetAnlgChanByOutput_isr
	( BVDC_Display_Handle             hDisplay,
	  BVDC_P_DisplayInfo             *pDispInfo,
	  BVDC_DisplayOutput              eDisplayOutput,
	  BVDC_P_DisplayAnlgChan        **pstChan );

#ifdef __cplusplus
}
#endif

#endif /* #ifndef BVDC_DISPLAY_PRIV_H__ */
/* End of file. */
