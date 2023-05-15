/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvdc_priv.c $
 * $brcm_Revision: Hydra_Software_Devel/158 $
 * $brcm_Date: 9/11/12 10:26a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vdc/src/common/bvdc_priv.c $
 * 
 * Hydra_Software_Devel/158   9/11/12 10:26a vanessah
 * SW7425-2526: add 3D stereo capability query
 * 
 * Hydra_Software_Devel/157   8/17/12 4:23p albertl
 * SW7425-3397: Fixed coverity issue.
 * 
 * Hydra_Software_Devel/156   8/13/12 3:08p yuxiaz
 * SW7425-3626, SW7425-3619: Replace uncache memory access with cache
 * memory access: use the new API names in RDC.
 * 
 * Hydra_Software_Devel/155   8/9/12 12:07p syang
 * SW7425-2172:  cleanup err msg
 * 
 * Hydra_Software_Devel/154   8/9/12 11:19a syang
 * SW7425-2172: add SetSurface support for MFD gfx feeding
 * 
 * Hydra_Software_Devel/153   6/25/12 1:41p pntruong
 * SW7405-5488: Added support for querying display capabilties of vdc
 * supportable format.
 *
 * Hydra_Software_Devel/152   6/20/12 4:43p pntruong
 * SW7405-5488: Added support for querying display capabilties of vdc
 * supportable format.
 *
 * Hydra_Software_Devel/151   5/30/12 7:38p syang
 * SW7425-2093: extra ApplyChange with 1st SetSurface no longer needed;
 * vfd/cap time stamp related scratch registers now share with vfd's
 * surface manager's; added vfd resource check; more clean up
 *
 * Hydra_Software_Devel/150   5/23/12 6:47p syang
 * SW7425-2093: refactor vfd gfx feeding support for deep cleanup and to
 * use gfxsurface sub-module
 *
 * Hydra_Software_Devel/149   4/12/12 4:12p vanessah
 * SW7425-2854: assign single trigger for NRT mode
 *
 * Hydra_Software_Devel/148   4/5/12 10:58a vanessah
 * SW7552-221: roll back changes
 *
 * Hydra_Software_Devel/147   4/4/12 2:59p vanessah
 * SW7552-221: no subrul win id setting for invalid window
 *
 * Hydra_Software_Devel/146   3/29/12 3:48p pntruong
 * SW7405-4296, SW7425-2526: Added some initial capabilities query at the
 * vdc top level.
 *
 * Hydra_Software_Devel/145   3/1/12 11:46a yuxiaz
 * SW7425-2526, SW7425-1182: Added runtime query capabilities for source
 * in VDC.
 *
 * Hydra_Software_Devel/144   1/11/12 3:06p darnstein
 * SW7125-1124: merge DCS Hybrid+ to main branch. DCS almost ready for
 * production.
 *
 * Hydra_Software_Devel/SW7125-1124/1   12/1/11 6:39p darnstein
 * SW7125-1124: change definition of IT_VER to identify early version of
 * IT hardware that has PCL_6, PCL_7, and PCL_8 registers.
 *
 * Hydra_Software_Devel/143   12/1/11 4:33p yuxiaz
 * SW7425-968, SW7344-95: Merged into mainline.: added independent source
 * clipping of right window in 3D mode.
 *
 * Hydra_Software_Devel/142   11/23/11 4:25p tdo
 * SW7435-9: Add support for CMP4-5, GFD4-5, MFD3, VFD5
 *
 * Hydra_Software_Devel/141   10/14/11 11:25p vanessah
 * SW7425-1027: add gcd calculation for transcoding
 *
 * Hydra_Software_Devel/140   9/15/11 11:29a vanessah
 * SW7425-923: B0 STG NRT mode
 *
 * Hydra_Software_Devel/139   8/31/11 11:19a syang
 * SW7425-1170: pxlAspRatio passed to ViCE2 is corrected to x<<16 | y
 * format
 *
 * Hydra_Software_Devel/138   8/27/11 7:22p hongtaoz
 * SW7425-1132: replaced slip2lock boolean flag with integer counter to
 * fix a timing sensitive hang conditiion when NRT sync-locked window is
 * brought up;
 *
 * Hydra_Software_Devel/137   8/26/11 5:38p syang
 * SW7425-1170: merge from branch SW7572-1170
 *
 * Hydra_Software_Devel/SW7425-1170/2   8/26/11 4:21p syang
 * SW7425-1170: refactor pixel aspect ratio related code
 *
 * Hydra_Software_Devel/SW7425-1170/1   8/23/11 6:18p vanessah
 * SW7425-1170: ASP cleanup
 *
 * Hydra_Software_Devel/136   7/18/11 11:20a vanessah
 * SW7425-835: SW7425-923: fix ViCE2 channel id bug + B0 STG
 *
 * Hydra_Software_Devel/135   7/11/11 2:17p tdo
 * SW7420-1971: Video Pause seen when VEC alignment is going on.  Add flag
 * to keep BVN connected while doing alignment.
 *
 * Hydra_Software_Devel/134   6/10/11 6:26p hongtaoz
 * SW7425-704: fixed compile error for chips without STG;
 *
 * Hydra_Software_Devel/133   6/10/11 5:49p hongtaoz
 * SW7425-704: add NRT mode support to VDC; moved the STG meta data
 * passing from window writer isr to reader isr; added bStallStc flag
 * support;
 *
 * Hydra_Software_Devel/132   3/2/11 11:29a jessem
 * SW7425-135: Fixed incorrect indexing with CMP1 and CMP2 windows.
 *
 * Hydra_Software_Devel/131   11/11/10 7:29p albertl
 * SW7125-364: Fixed BVDC_P_CbIsDirty and added assert to check bitfields
 * in dirty bits fit within union integer representation.  Fixed naming
 * of dirty bits.
 *
 * Hydra_Software_Devel/130   10/25/10 2:57p pntruong
 * SW7420-619: Leave csc dither off until further clarification of usage
 * that does not introduce side-effects.
 *
 * Hydra_Software_Devel/129   10/11/10 12:43p jessem
 * SW7420-173: Added support for VFD as source feature.
 *
 * Hydra_Software_Devel/128   6/23/10 4:59p rpan
 * SW7400-2808: Stop enabling BVN while aligning VECs.
 *
 * Hydra_Software_Devel/127   5/7/10 7:19p albertl
 * SW7125-364: Changed dirty bits to use union structure to avoid type-pun
 * warnings
 *
 * Hydra_Software_Devel/126   4/19/10 10:18p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic. Move
 * BDBG_OBJECT_ID_DECLARE private header files instead of .c.
 *
 * Hydra_Software_Devel/125   4/7/10 11:34a tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic.  Rename TLA
 *
 * Hydra_Software_Devel/124   4/5/10 4:12p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic
 *
 * Hydra_Software_Devel/123   3/17/10 12:04p syang
 * SW7405-4046: set canvas ctrl after sur ctrl; handle gfx win vnet the
 * same as video; ensure that reader and writer vnetState match in sync-
 * locked case; aligned width up to even for stride in feeder and cap;
 * assert mosaic mode doesn't co-exist with dest cut;
 *
 * Hydra_Software_Devel/122   3/16/10 4:34p rpan
 * SW7340-144: Attempt to fix a Coverity warning.
 *
 * Hydra_Software_Devel/121   1/6/10 3:41p rpan
 * SW7468-64: Consolidate VEC modulo count trigger implementation.
 *
 * Hydra_Software_Devel/120   12/21/09 2:45p rpan
 * SW7468-30: 1) When OSCL is enabled for 1080p display, top and bottom
 * RDC slots are used for building RULs.
 * 2) Addressed the video format stored at hCompistor being overwritten
 * issue.
 *
 * Hydra_Software_Devel/119   3/18/09 10:22a rpan
 * PR53104: Always build format switch RUL as long as VEC is switching
 * format. Otherwise, we may get stuck in format swithing if somehow the
 * first TOP field interrupt is missed.
 *
 * Hydra_Software_Devel/118   1/8/09 2:26p rpan
 * PR50391: Renabled the VEC RUL execution mechanism after fixing a number
 * of RDC related issues.
 *
 * Hydra_Software_Devel/117   12/24/08 10:40a rpan
 * PR50391: Back out the change that adds NOP to RUL. This occassionally
 * caused ApplyChange timeout.
 *
 * Hydra_Software_Devel/116   12/24/08 10:28a rpan
 * PR50391: Always add a NOP to RUL during format switch. This helps
 * silent some RDC check messages.
 *
 * Hydra_Software_Devel/115   12/18/08 6:49p rpan
 * PR50391: Changes to make sure format switch RUL can always be executed.
 *
 * Hydra_Software_Devel/114   11/12/08 3:13p tdo
 * PR48642: Provide clipping rect for histo luma region
 *
 * Hydra_Software_Devel/113   8/8/08 3:29p yuxiaz
 * PR45484: Enable Dithering in VDC.
 *
 * Hydra_Software_Devel/112   5/22/08 1:19p pntruong
 * PR42475, PR41898: Rollback pr41898. Need re-revaluation of resource
 * releasing.
 *
 * Hydra_Software_Devel/111   4/18/08 1:43p pntruong
 * PR41898: Need better synchronization of window states.  Synchronize the
 * releases of resource and vnetmode together in writer task.
 *
 * Hydra_Software_Devel/110   3/25/08 3:17p syang
 * PR 40431: add complete assert for critical section protection among src
 * _isr, dsp _isr, and ApplyChanges
 *
 * Hydra_Software_Devel/109   2/29/08 4:19p yuxiaz
 * PR39158: Implement dithering in various BVN components for 3548.
 *
 * Hydra_Software_Devel/108   9/24/07 3:42p hongtaoz
 * PR34955: track and clean up the sync-slipped mpeg PIP source during
 * display format switch;
 *
 * Hydra_Software_Devel/107   2/20/07 2:56p jessem
 * PR25235: Removed the use of rate mask and replaced with actual value of
 * source frame and display refresh rates.
 *
 * Hydra_Software_Devel/106   1/18/07 11:35a hongtaoz
 * PR23260: disable display triggers at destroy for clean shutdown;
 *
 * Hydra_Software_Devel/105   10/25/06 3:45p hongtaoz
 * PR25166: avoid flushing the same cached RUL twice in a vsync isr;
 *
 * Hydra_Software_Devel/104   7/23/06 4:23p hongtaoz
 * PR22437: 1st sync-lock source _isr cleans up display slip RUL; when a
 * mpeg source's last window is disconnected, clean up source slots
 * immediately; when mpeg source is muting, don't perform 50->60
 * conversion to avoid video freeze or flash artifact;
 *
 * Hydra_Software_Devel/103   7/17/06 1:01p hongtaoz
 * PR22713: increased the synlock transfer semaphore count; display _isr
 * cleans up source slots only when the semaphore count is 0;
 *
 * Hydra_Software_Devel/102   7/13/06 10:56a jessem
 * PR 22389: Added BVDC_P_MapSrcFrameRateToVerticalRefreshRate_isr.
 *
 * Hydra_Software_Devel/101   5/26/06 3:42p pntruong
 * PR20642: Refactored handling of hList.
 *
 * Hydra_Software_Devel/99   4/6/06 1:56p pntruong
 * PR20642: Optimized VDC codes (refactoring).
 *
 * Hydra_Software_Devel/98   2/9/06 4:15p pntruong
 * PR19270: HDCP glitches at wrap of Sarnoff 1080i DYNPICTS.TRP stream.
 * Update hdmi rate manager in vertical blanking to avoid glitches.
 *
 * Hydra_Software_Devel/96   1/25/06 10:58p pntruong
 * PR19172: Black-screen with latest VDC on 97398.  The mosaic added 4
 * more slots for capture compositing surface  this pushes the hddvi slot
 * out, and not all slot has track execution.  This causes the update
 * format rul of hddvi not properly execute and cause hddvi not to lock.
 * Fixes by freeing non-used frame slot in vdec/656/hddvi, and free up
 * more rdc vars to be used for track execution.
 *
 * Hydra_Software_Devel/95   10/11/05 11:18a hongtaoz
 * PR15495: back out previous change; single RUL applied to dual slots
 * needs further work to be robust;
 *
 * Hydra_Software_Devel/94   10/10/05 3:23p hongtaoz
 * PR15495: combine T/B RUL in case of critical double RULs;
 *
 * Hydra_Software_Devel/93   9/16/05 2:30p hongtaoz
 * PR16812: when synclock transfers to new cmp, reset semaphore at
 * display_isr also in case mpeg isr stops calling back;
 *
 * Hydra_Software_Devel/92   8/29/05 3:30p hongtaoz
 * PR16812, PR15495: build both T/B RULs for sync-slipped playback side
 * muting RUL and not touch window playback side before the new vnet RUL
 * is executed; restore window's current shutdown state without affecting
 * user setting;
 *
 * Hydra_Software_Devel/91   8/18/05 1:18p pntruong
 * PR15757, PR16391, PR16411, PR12519, PR14791, PR15535, PR15206, PR15778:
 * Improved bandwidth for cropping/scaler/capture/playback.   Unified
 * window shutdown sequence for destroy, reconfigure mad/scaler, and/or
 * reconfigure result of source changes.  And miscellances fixes from
 * above PRs.
 *
 * Hydra_Software_Devel/XVDPhase1/XVDPhase1_merge/1   8/16/05 5:24p hongtaoz
 * PR12519: take in fix from main-line;
 *
 * Hydra_Software_Devel/90   6/14/05 1:35p hongtaoz
 * PR14510, PR15163, PR15743: avoid repeated faked triggers;
 *
 * Hydra_Software_Devel/XVDPhase1/2   5/2/05 11:22p pntruong
 * PR15084: Monitor both triggers when source lost triggers.
 *
 * Hydra_Software_Devel/XVDPhase1/1   5/2/05 4:54p pntruong
 * PR12519, PR13121, PR15048, PR15084, PR15100: Dynamically re-allocate
 * capture in bvn path to reduce memory consumption and bandwith.
 *
 * Hydra_Software_Devel/2   5/1/05 11:40p pntruong
 * Added monitoring of both triggers, since hddvi does not always trigger
 * frame for progressive.
 *
 * Hydra_Software_Devel/1   5/1/05 4:49p pntruong
 * XVDPhase1 branch off.  Temp storage so we can keep track of changes.
 *
 * Hydra_Software_Devel/89   4/21/05 5:51p hongtaoz
 * PR14868: force trigger the correct vdec src slot;
 *
 * Hydra_Software_Devel/88   4/20/05 10:51a pntruong
 * PR14834: Reconnecting linein with MAD enabled will put video output
 * into bad state.
 *
 * Hydra_Software_Devel/87   4/18/05 2:45p yuxiaz
 * PR14808: Force trigger the slot lose trigger based on input source
 * format for hd_dvi. Fixed src/window state machine for HD_DVI.
 *
 * Hydra_Software_Devel/86   4/8/05 3:38p pntruong
 * PR14018, PR14011, PR14450, PR14648:  Vec to generate fake trigger when
 * source is pulled or not configured correctly or any condition that
 * cause source to lose trigger.  This vec's fake trigger enable the
 * applychanges to go thru, and does not result in timeout.
 *
 * Hydra_Software_Devel/85   3/11/05 9:21a pntruong
 * PR14112, PR14046: Fixed hd/sd channel change for progressive mode.
 *
 * Hydra_Software_Devel/84   2/1/05 3:06p pntruong
 * PR14003: Rapid open and close of the same mpeg window will cause feeder
 * error.
 *
 * Hydra_Software_Devel/83   1/31/05 6:52p pntruong
 * PR12790: Noticeable screen flickering on digital to digital channel
 * change.  Also, used _isr version of kni functions in critical
 * sections.
 *
 * Hydra_Software_Devel/82   1/28/05 9:17a pntruong
 * PR13321: Stress test failed (MAIN/PIP swap and format change).
 *
 * Hydra_Software_Devel/81   1/26/05 4:53p pntruong
 * PR13450, PR12854, PR13549, PR13617, PR13618, PR13683, PR13321, PR13646,
 * PR13447, PR13429: Disabled vec triggers when vec reset (e.g. format
 * change) to prevent it from continuosly executing same register update
 * list that cause system locked up and/or causing vec errors.
 *
 * Hydra_Software_Devel/80   12/3/04 4:41p pntruong
 * PR13453: Pip window deletion timeout error with BCM7038B0 Phase 4
 * reference software.
 *
 * Hydra_Software_Devel/79   12/2/04 7:17p pntruong
 * PR12854: Repeated display format changes causes loss of sync and
 * flashing video.
 *
 * Hydra_Software_Devel/78   11/29/04 4:57p pntruong
 * PR13076, PR11749:  No need to keep track of synclock dummy rul.
 *
 * Hydra_Software_Devel/77   11/23/04 8:54p pntruong
 * PR13076, PR11749: Video jitter under heavy system load.  Added RUL
 * execution check to reduce number of programmed registers.
 *
 * Hydra_Software_Devel/76   11/16/04 8:01p pntruong
 * PR13076: Video jitter under heavy system load.  Added some
 * optimizations, additional work needed.
 *
 * Hydra_Software_Devel/75   11/12/04 5:43p pntruong
 * PR13242: Repeated stop/start of live digital decode produces vertical
 * split-screen which moves to the right with each restart.
 *
 * Hydra_Software_Devel/74   11/2/04 5:29p pntruong
 * PR13076: Added cached memory support for rul.
 *
 * Hydra_Software_Devel/73   11/1/04 2:11p yuxiaz
 * PR12790: Add sync slip to sync lock transition to fix graphics
 * flickering on analog to digital channel change.
 *
 * Hydra_Software_Devel/72   10/29/04 3:55p hongtaoz
 * PR13125: added time stamp at the end of display RUL and Vdec RUL to
 * measure RUL execution time;
 *
 * Hydra_Software_Devel/71   10/12/04 3:50p yuxiaz
 * PR12790: Fixed flickering for digital to digital channel change in
 * Brutus.
 *
 * Hydra_Software_Devel/70   10/6/04 3:40p yuxiaz
 * PR12790: Fixed flickering for digital to digital channel change.
 *
 * Hydra_Software_Devel/69   10/6/04 2:27p pntruong
 * PR12854: Repeated display format changes causes loss of sync and
 * flashing video.
 *
 * Hydra_Software_Devel/68   10/5/04 4:27p pntruong
 * PR12854: Increased the vsync delay for build front-end blocks when
 * format change.
 *
 * Hydra_Software_Devel/67   10/5/04 2:08p pntruong
 * PR12907: Timing problem in VDC.
 *
 * Hydra_Software_Devel/66   9/15/04 3:55p yuxiaz
 * PR 12606: Fixed channel change in simul mode.
 *
 * Hydra_Software_Devel/65   9/10/04 4:13p pntruong
 * PR12615: Re-intitial multibuffer when window re-create to remove old
 * states.
 *
 * Hydra_Software_Devel/64   9/9/04 8:00p pntruong
 * PR12615: Re-intitial multibuffer when window re-create to remove old
 * states.
 *
 * Hydra_Software_Devel/63   9/9/04 1:40p pntruong
 * PR11573: Remove 3rd display slot from VDC allocation.
 *
 * Hydra_Software_Devel/62   8/31/04 1:05p pntruong
 * PR11266:  Added the visibility or shutoff gfx feeder check for the
 * sync-slip case.
 *
 * Hydra_Software_Devel/61   8/26/04 6:35p syang
 * PR 11266: added src picture call back func for gfx;  added src state to
 * call back func for XVD sync;
 *
 * Hydra_Software_Devel/60   8/18/04 1:59p pntruong
 * PR12231: Fixed for gfx window as well.
 *
 * Hydra_Software_Devel/59   8/12/04 1:49p pntruong
 * PR12272: TAB is not enabled the first time when bring up analog.
 *
 * Hydra_Software_Devel/58   8/11/04 3:47p pntruong
 * PR12246: Video may be lost on main display when stress HD/SD streams/
 * formats change in 10-20 minutes.
 *
 * Hydra_Software_Devel/57   8/2/04 4:36p pntruong
 * PR11981: Improved read/modify/write sequence. Factored out duplicate
 * codes.  Removed RDC instructions (any instruction that uses IMM with
 * VAR) that sensitive to 256-byte boundary.
 *
 * Hydra_Software_Devel/56   7/30/04 12:45p tdo
 * PR11971: add call to BVDC_P_Pep_BuildRul_isr to build PEP RUL
 *
 * Hydra_Software_Devel/55   7/9/04 5:47p pntruong
 * PR11887: Simplify the computation of rectangles in VDC.
 *
 * Hydra_Software_Devel/54   7/8/04 7:19p albertl
 * PR 7662:  Fixed issues with different compositors overwriting 656 and
 * MISC settings used by other compostiors.  Disabled feeder to Bypass
 * when 656 is inactive.
 *
 * Hydra_Software_Devel/53   7/6/04 8:33a pntruong
 * PR9957: Prevent create/destroy from effecting hardware in VDC.
 *
 * Hydra_Software_Devel/52   6/25/04 3:26p yuxiaz
 * PR 11659: Rename RDC variables reserved for VDC to
 * BRDC_Variable_VDC_num. Move the defination of reserved variables back
 * to RDC.
 *
 * Hydra_Software_Devel/50   6/25/04 9:09a yuxiaz
 * PR 11659: Use reserved RDC variables defined in VDC instead of the ones
 * in RDC. Variables defined in RDC can be used by all the moduals.
 *
 * Hydra_Software_Devel/49   6/22/04 2:10p yuxiaz
 * PR 11612: Fixed mutil-buffering after mode switching. Fixed pitch for
 * capture and playback.
 *
 * Hydra_Software_Devel/48   6/22/04 1:25p hongtaoz
 * PR11549: added MAD support for 720p/1080i format;
 *
 * Hydra_Software_Devel/47   6/17/04 4:37p pntruong
 * PR9957: Prevent create/destroy from effecting hardware in VDC.
 *
 * Hydra_Software_Devel/46   6/16/04 7:38p hongtaoz
 * PR11422: fixed mode switch problem with analog video source; modified
 * multi-buffering algorithm to avoid deadlock.
 *
 * Hydra_Software_Devel/45   6/3/04 9:35p albertl
 * PR 7662: Bringup of 656 output for B0.
 *
 * Hydra_Software_Devel/44   5/24/04 6:59p jasonh
 * PR 11189: Merge down from B0 to main-line
 *
 * Hydra_Software_Devel/Refsw_Devel_7038_B0/3   5/21/04 6:57p hongtaoz
 * PR10944: added initial MAD32 support of 480i->480p;
 *
 * Hydra_Software_Devel/Refsw_Devel_7038_B0/2   5/17/04 7:35p hongtaoz
 * PR10944: added initial MAD32 support;
 *
 * Hydra_Software_Devel/Refsw_Devel_7038_B0/1   4/29/04 7:30p hongtaoz
 * PR10852: simplified the color space conversion process;
 * PR9996: supported multi-standard color space conversion at compositor
 * with color primaries matching;
 * PR8761: fixed part of C++ compiling errors;
 *
 * Hydra_Software_Devel/42   4/27/04 4:42p yuxiaz
 * PR 10699: Added time stamp tracking for multi-buffering debugging.
 *
 * Hydra_Software_Devel/41   4/13/04 5:41p pntruong
 * PR8987: Added support for output format change from interlace to/from
 * progressive format.
 *
 * Hydra_Software_Devel/40   4/7/04 2:14p yuxiaz
 * PR 10404: Combine BRDC_Slot_SetNumEntries_isr and
 * BRDC_Slot_AssignList_isr into BRDC_Slot_SetList_isr so both RUL
 * address and count are set in one atomic function.
 *
 * Hydra_Software_Devel/39   4/1/04 2:25p hongtaoz
 * PR9996: add color space conversion for different video surfaces in
 * compositor.
 *
 * Hydra_Software_Devel/38   3/29/04 3:23p pntruong
 * PR8987: Removed direct register write  timer code as it would need to
 * be in a separate basemodule before we can use it.  Added counter to
 * ignore vec reset in build RUL until vec stable.  This fixed the
 * display output format change to/from 1080i & 480i and 720p & 480p.
 *
 * Hydra_Software_Devel/37   3/23/04 8:42p pntruong
 * PR8987: Added debug capability to output interrupt timing.  Use
 * BVDC_P_USE_TIMER_DEBUGGER to enable or disable timing feature.  This
 * currently use TIMER2 timer watchdog.
 *
 * Hydra_Software_Devel/36   3/22/04 8:13p pntruong
 * PR8987: Added fixed for output format change from I->I, and P->P.
 * Additional work required for I->P and P->I.
 *
 * Hydra_Software_Devel/35   3/16/04 12:37p maivu
 * PR 8987: Update Vec state when mode is switched
 *
 * Hydra_Software_Devel/34   3/12/04 4:23p yuxiaz
 * PR 10032: Move BVDC_P_Vnet_BuildRul_isr to source isr.
 *
 * Hydra_Software_Devel/33   3/11/04 12:44p pntruong
 * PR9865:  Make iTransitionPic per compositor based.
 *
 * Hydra_Software_Devel/32   3/11/04 10:01a hongtaoz
 * PR9865: VDC sync lock determination at video window open/close;
 * 1) a vdc-wise global flag "bSupportSlip2Lock" is added to turn on/off
 * the sync-slip to lock on-the-fly transition; by default it's off; only
 * when all the transition works smoothly, we'll turn on this feature or
 * put it into pDefSettings for application to decide when calling
 * BVDC_Open;
 * 2) when the slip2lock on-the-fly transition is allowed, there will be 2
 * fields of period for transition to happen; during this period, the -
 * to-be-locked display would still build sync-slip RUL to avoid
 * overwriting MFD in case of timing difference between the old sync-
 * locked vec and newly locked vec;
 * 3) also during the transition period, the vnet mode would not be
 * modified to scaler-only mode even if the lock window doesn't require
 * scaling down, so that the transition fields would be played back from
 * the captured buffers;
 *
 * Hydra_Software_Devel/31   3/8/04 6:37p hongtaoz
 * PR9917: display freezes and hang when sync-slipped mpeg video window
 * transitioned to sync-lock on the fly; the reason is that the vnet
 * switch isn't turn off and the sync-locked compositor is still waiting
 * for the closed video picture when the sync-locked window is closed.
 *
 * Hydra_Software_Devel/30   3/3/04 11:57a hongtaoz
 * PR9689: Open/Close/Open sequence for video windows doesn't work because
 * a video window is created with zero window sizes which caused assert,
 * the fix is to use a flag to indicate whether the window setting being
 * applied to determine if RUL builder to accomandate that window into
 * the RUL;
 *
 * Hydra_Software_Devel/29   3/3/04 9:46a maivu
 * PR 8987: Fixed 480p/720p (progressive) mode switching problem.
 *
 * Hydra_Software_Devel/28   2/25/04 5:34p maivu
 * PR 9784, PR 9831: Removed unnecessary initial nop RUL, since we're
 * double buffering the Ruls now.
 *
 * Hydra_Software_Devel/27   2/20/04 9:58a pntruong
 * PR8987: Added code to re-initialized RULs, and reset each window's
 * block.
 *
 * Hydra_Software_Devel/26   2/4/04 1:32p pntruong
 * PR8861: Analyze VDC Rul building for next field.
 *
 * Hydra_Software_Devel/25   2/3/04 11:02a pntruong
 * PR9075: Double buffer RULs in VDC.  Need to keep track of index for
 * each slot.
 *
 * Hydra_Software_Devel/24   2/2/04 4:33p pntruong
 * PR9559: 720p and 480p are broken in VDC and not working on
 * mvd_vdc_test.
 *
 * Hydra_Software_Devel/23   1/23/04 3:48p pntruong
 * PR9459: BVDC_Window_Get_? functions return incorrect state.
 *
 * Hydra_Software_Devel/22   1/21/04 10:30a yuxiaz
 * PR 9076: Change BRDC_Slot_AssignList to _isr for double buffer RUL.
 *
 * Hydra_Software_Devel/21   1/20/04 8:32p pntruong
 * PR9075: Double buffer RULs in VDC.
 *
 * Hydra_Software_Devel/20   1/14/04 4:39p yuxiaz
 * PR 9076: Change isr functions to _isr.
 *
 * Hydra_Software_Devel/19   12/29/03 6:42p pntruong
 * PR 9117: Refactor BINT to no longer use strings to specify interrupt
 * name.
 *
 * Hydra_Software_Devel/18   12/19/03 4:35p pntruong
 * PR9073: Fix interrupt abstraction for MVD to VDC.
 *
 * Hydra_Software_Devel/17   12/10/03 3:54p syang
 * PR 8914: ensure DefaultKeyAlpha and other register inited, fixed
 * prroblem with GFD_0_SRC_START in the case of bottom field
 *
 * Hydra_Software_Devel/16   12/5/03 11:55a maivu
 * PR 8863: always build for current field, instead of next field. Fixes
 * HD problem.
 *
 * Hydra_Software_Devel/15   11/25/03 5:04p pntruong
 * Added GFX build RUL.
 *
 * Hydra_Software_Devel/14   11/24/03 5:02p yuxiaz
 * Build RUL of next filed in ISR for interlace.
 *
 * Hydra_Software_Devel/13   11/7/03 10:02a pntruong
 * Fxied partial RUL execution.  That may result in RDC hang.
 *
 * Hydra_Software_Devel/12   10/30/03 6:21p pntruong
 * Updated window state.
 *
 * Hydra_Software_Devel/11   10/29/03 11:45a pntruong
 * Reformated code.
 *
 * Hydra_Software_Devel/10   10/28/03 2:46p maivu
 * Update the slot entries in hw, after we build the Rul.
 *
 * Hydra_Software_Devel/9   10/24/03 5:12p pntruong
 * Updated RUL building to hanlde sync-lock and sync-slip.
 *
 * Hydra_Software_Devel/8   10/14/03 10:53a pntruong
 * Allocated more RULs to adapt to the way analog fields come in.
 *
 * Hydra_Software_Devel/7   10/7/03 5:36p maivu
 * Added field id to BVDC_P_Vec_BuildRul.
 *
 * Hydra_Software_Devel/6   10/6/03 2:50p yuxiaz
 * Added bvdc_common_priv.h, clean up private include files.
 *
 * Hydra_Software_Devel/6   10/6/03 2:48p yuxiaz
 * Added bvdc_common_priv.h, clean up private include files.
 *
 * Hydra_Software_Devel/5   10/2/03 10:31a pntruong
 * Added backend isr callback.
 *
 * Hydra_Software_Devel/4   9/26/03 3:56p pntruong
 * Removed uneeded functions.
 *
 * Hydra_Software_Devel/3   8/22/03 12:00p pntruong
 * Added create and destroy.
 *
 * Hydra_Software_Devel/2   7/28/03 11:29a pntruong
 * added BDBG_MODULE(BVDC).
 *
 * Hydra_Software_Devel/1   7/17/03 5:55p pntruong
 * Added source.
 *
 ***************************************************************************/
#include "bstd.h"                 /* standard types */
#include "bkni.h"                 /* memcpy calls */
#include "bvdc.h"                 /* Video display */
#include "bdbg.h"
#include "bavc.h"
#include "bvdc_priv.h"
#include "bvdc_common_priv.h"
#include "bvdc_compositor_priv.h"
#include "bvdc_display_priv.h"
#include "bvdc_source_priv.h"
#include "bvdc_feeder_priv.h"
#include "bvdc_656in_priv.h"

BDBG_MODULE(BVDC_PRIV);

/***************************************************************************
 * Add an NO-OP into RUL.
 *
 */
void BVDC_P_BuildNoOpsRul_isr
	( BRDC_List_Handle                 hList )
{
	uint32_t ulCurrentEntries;
	uint32_t ulNewEntries;
	uint32_t *pulCurrent;
	uint32_t *pulStart;

	/* Save the current number of entries, will update number for this list
	 * at the end. */
	BRDC_List_GetNumEntries_isr(hList, &ulCurrentEntries);

	/* get pointer to list entries */
	pulStart = pulCurrent =
		BRDC_List_GetStartAddress_isr(hList) + ulCurrentEntries;

	/* Valid start address */
	BDBG_ASSERT(pulStart);
	BVDC_P_BUILD_NO_OPS(pulCurrent);

	/* Update entries count */
	ulNewEntries = (uint32_t)(pulCurrent - pulStart);

	BRDC_List_SetNumEntries_isr(hList, ulCurrentEntries + ulNewEntries);
	return;
}


/***************************************************************************
 * Get the currently list pointer and store in pList!  Including the
 * last executed status.
 *
 */
void BVDC_P_ReadListInfo_isr
	( BVDC_P_ListInfo                 *pList,
	  BRDC_List_Handle                 hList )
{
	uint32_t ulNumEntries;

	/* Read list info once!  This prevent calling into RDC multiple times. */
	                           BRDC_List_GetNumEntries_isr(hList, &ulNumEntries);
	pList->bLastExecuted     = BRDC_List_GetLastExecStatus_isr(hList);
	pList->pulStart          = BRDC_List_GetStartAddress_isr(hList);
	pList->pulCurrent        = pList->pulStart + ulNumEntries;
	return;
}


/***************************************************************************
 * Update the number of entries from pList to hList!
 *
 */
void BVDC_P_WriteListInfo_isr
	( const BVDC_P_ListInfo           *pList,
	  BRDC_List_Handle                 hList )
{
	BERR_Code eRdcErrCode;
	uint32_t ulNumEntries;

	/* Update the entries again. */
	ulNumEntries = (uint32_t)(pList->pulCurrent - pList->pulStart);
	eRdcErrCode = BRDC_List_SetNumEntries_isr(hList, ulNumEntries);

	/* Got to make sure we allocated enough RUL mem!  Something that must be
	 * done in BVDC_Open(), and instrumented with tests runs! */
	BDBG_ASSERT(BERR_SUCCESS == eRdcErrCode);
	return;
}


/***************************************************************************
 *
 */
void BVDC_P_Dither_Init
	( BVDC_P_DitherSetting            *pDitherSetting,
	  uint32_t                         ulLfsrCtrlT0,
	  uint32_t                         ulLfsrCtrlT1,
	  uint32_t                         ulLfsrCtrlT2,
	  uint32_t                         ulLfsrValue )
{
	if(pDitherSetting)
	{
		pDitherSetting->ulLfsrSeq    = 0;  /* once */
		pDitherSetting->ulLfsrValue  = ulLfsrValue;
		pDitherSetting->ulLfsrCtrlT0 = ulLfsrCtrlT0;
		pDitherSetting->ulLfsrCtrlT1 = ulLfsrCtrlT1;
		pDitherSetting->ulLfsrCtrlT2 = ulLfsrCtrlT2;

		pDitherSetting->ulMode       = 0;  /* 0=rounding, 1=TRUNCATE, 2=Dither */
		pDitherSetting->ulCh0Offset  = 0;
		pDitherSetting->ulCh1Offset  = 0;
		pDitherSetting->ulCh2Offset  = 0;
		pDitherSetting->ulCh0Scale   = 1;
		pDitherSetting->ulCh1Scale   = 1;
		pDitherSetting->ulCh2Scale   = 1;
	}

	return;
}


/***************************************************************************
 * BVDC_P_CompositorDisplay_isr
 *
 * This get call at every display vsync trigger a slot w/ done execution.
 * We're then building the RUL for the next field/frame.
 *
 * pvCompositorHandle contains hCompositor
 * (iParam2) = BAVC_Polarity_eTopField /BAVC_Polarity_eBotField;
 */
void BVDC_P_CompositorDisplay_isr
	( void                            *pvCompositorHandle,
	  int                              iParam2 )
{
	uint32_t i;
	BRDC_Slot_Handle          hSlot;
	BRDC_List_Handle          hList;
	BVDC_P_ListInfo           stList;
	BVDC_Compositor_Handle    hCompositor = (BVDC_Compositor_Handle)pvCompositorHandle;
	BAVC_Polarity             eNextFieldId;

	BDBG_ENTER(BVDC_P_CompositorDisplay_isr);

	/* Get The compositor handle from isr callback */
	eNextFieldId = BVDC_P_NEXT_POLARITY((BAVC_Polarity)(iParam2));

	/* Build Display RUL & Compositor (playback modules) */
	BDBG_OBJECT_ASSERT(hCompositor, BVDC_CMP);
	BDBG_OBJECT_ASSERT(hCompositor->hDisplay, BVDC_DSP);
	BDBG_OBJECT_ASSERT(hCompositor->hVdc, BVDC_VDC);

	/* Display only have T/B slot. */
	BDBG_ASSERT(BAVC_Polarity_eFrame != (BAVC_Polarity)iParam2);

	/* Basically this guaranteed that interrupt will not run dried. */
	BDBG_ASSERT(BVDC_P_STATE_IS_INACTIVE(hCompositor)==false);

	/* Make sure the BKNI enter/leave critical section works. */
	BDBG_ASSERT(0 == hCompositor->hVdc->ulInsideCs);
	hCompositor->hVdc->ulInsideCs++;

	if((BVDC_P_ItState_eNotActive  == hCompositor->hDisplay->eItState) ||
	   (BVDC_P_ItState_eSwitchMode == hCompositor->hDisplay->eItState))
	{
		/* Detected the force execution, or mode switch reset of vec. */
		uint32_t ulVecResetDetected = BREG_Read32(hCompositor->hVdc->hRegister,
			hCompositor->hDisplay->ulRdcVarAddr);

		/* If ulVecResetDetected it means that the last RDC executed a VEC's
		 * reset.  At this point we want to change the state of the vec. */
		if(ulVecResetDetected)
		{
			BAVC_Polarity ePolarity;
			/* Acknowledge reset. */
			BREG_Write32(hCompositor->hVdc->hRegister, hCompositor->hDisplay->ulRdcVarAddr, 0);
			BDBG_MSG(("Display[%d]'s state: %s (%s => %d)", hCompositor->eId,
				(BVDC_P_ItState_eNotActive == hCompositor->hDisplay->eItState) ? "eNotActive" :
				(BVDC_P_ItState_eSwitchMode == hCompositor->hDisplay->eItState) ? "eSwitchMode" : "eActive",
				(BAVC_Polarity_eTopField==iParam2)?"T":
				(BAVC_Polarity_eBotField==iParam2)?"B":"F", ulVecResetDetected));
			if((BVDC_P_ItState_eSwitchMode == hCompositor->hDisplay->eItState) ||
			   (BVDC_P_ItState_eNotActive  == hCompositor->hDisplay->eItState))
			{
				hCompositor->hDisplay->eItState = BVDC_P_ItState_eActive;
			}

			/* Clean up Top/Bot display slot/list */
			for(ePolarity =  BAVC_Polarity_eTopField;
			    ePolarity <= BAVC_Polarity_eBotField; ePolarity++)
			{
				BVDC_P_CMP_NEXT_RUL(hCompositor, ePolarity);
				hSlot = BVDC_P_CMP_GET_SLOT(hCompositor, ePolarity);
				hList = BVDC_P_CMP_GET_LIST(hCompositor, ePolarity);
				BRDC_List_SetNumEntries_isr(hList, 0);
				BVDC_P_BuildNoOpsRul_isr(hList);
#if BVDC_P_ORTHOGONAL_VEC
				/* Disable execution tracking. This caused hwTrackCount and
				 * software TrackCount mismatch because only one RUL actually
				 * gets executed.
				 * This is done for orthogonal VEC for now. It may make sense
				 * to apply this for all the VECs.
				 */
				BRDC_Slot_UpdateLastRulStatus_isr(hSlot, hList, false);

#endif
				BRDC_Slot_SetList_isr(hSlot, hList);
#if BVDC_P_ORTHOGONAL_VEC
				/* Re-enable execution tracking.
				 */
				BRDC_Slot_UpdateLastRulStatus_isr(hSlot, hList, true);

#endif
			}

			/* Clean up Top/Bot/Frame slot/list of sync-lock source. */
			if(hCompositor->hSyncLockSrc)
			{
				BVDC_P_Source_CleanupSlots_isr(hCompositor->hSyncLockSrc);

				/* clean up mpeg PIP source slots as well if it's not locked */
				if(hCompositor->hForceTrigPipSrc)
				{
					BVDC_P_Source_CleanupSlots_isr(hCompositor->hForceTrigPipSrc);
				}
			}
			/* Re-enable triggers. */
			BVDC_P_Display_EnableTriggers_isr(hCompositor->hDisplay, true);
			goto BVDC_P_CompositorDisplay_isr_Done;
		}
	}

	/* Check if we're doing frame.  If we're doing frame we're use a topfield
	 * slot to trigger the frame slot in source isr for sycn lock. */
	if((BAVC_Polarity_eBotField != iParam2) &&
	   (!hCompositor->hDisplay->stCurInfo.pFmtInfo->bInterlaced)
#if	(BVDC_P_SUPPORT_IT_VER >= 2)
	   && (2 != hCompositor->hDisplay->stCurInfo.ulTriggerModuloCnt)
#endif
	)
	{
		eNextFieldId = BAVC_Polarity_eFrame;
		BVDC_P_CMP_NEXT_RUL(hCompositor, BAVC_Polarity_eTopField);
		hSlot = BVDC_P_CMP_GET_SLOT(hCompositor, BAVC_Polarity_eTopField);
		hList = BVDC_P_CMP_GET_LIST(hCompositor, BAVC_Polarity_eTopField);
	}
	else
	{
		/* eNextField can only be either eTopField or eBotField once
		 * we get here. The following line fixes a Coverity warning.
		 */
		eNextFieldId = (BAVC_Polarity_eBotField == eNextFieldId) ?
					BAVC_Polarity_eBotField : BAVC_Polarity_eTopField;

		/* Get the approriate slot/list for building RUL. */
		BVDC_P_CMP_NEXT_RUL(hCompositor, eNextFieldId);
		hSlot = BVDC_P_CMP_GET_SLOT(hCompositor, eNextFieldId);
		hList = BVDC_P_CMP_GET_LIST(hCompositor, eNextFieldId);
	}

	/* Reset the RUL entry count and build RUL for backend! */
#if BVDC_P_ORTHOGONAL_VEC
	/* Always tracking execution will simplify a lot of
	 * cases where we check the bLastExecuted flag. It's not
	 * to bad since the size of compositor side RUL for sync-locked
	 * source path is short anyway.
	 *
	 */
	BRDC_Slot_UpdateLastRulStatus_isr(hSlot, hList, true);
#else
	BRDC_Slot_UpdateLastRulStatus_isr(hSlot, hList, !hCompositor->hSyncLockSrc);
#endif
	BRDC_List_SetNumEntries_isr(hList, 0);
	BVDC_P_ReadListInfo_isr(&stList, hList);

	if((BVDC_P_ItState_eSwitchMode == hCompositor->hDisplay->eItState) ||
		(true == hCompositor->hDisplay->bAlignAdjusting  && !hCompositor->hDisplay->stCurInfo.stAlignCfg.bKeepBvnConnected))
	{
		/* When switching modes or doing VEC alignment, build the Vec only.
		 * Do not enable the front-end blocks yet. When doing VEC alignment VEC
		 * triggers can be fired earlier than normal cases. This may cause
		 * BVN unable to finish processing the picture and result in BVN hang.
		 */
		if(BVDC_P_ItState_eSwitchMode == hCompositor->hDisplay->eItState)
		{
			BDBG_MSG(("Vec mode switch[%s]: %s to %s",
				(BAVC_Polarity_eTopField == eNextFieldId) ? "T" :
				(BAVC_Polarity_eBotField == eNextFieldId) ? "B" : "F",
				hCompositor->hDisplay->stCurInfo.pFmtInfo->pchFormatStr,
				hCompositor->hDisplay->stNewInfo.pFmtInfo->pchFormatStr));
		}

		/* Reset VEC as long as it is in switch mode */
		BVDC_P_Vec_BuildRul_isr(hCompositor->hDisplay, &stList, eNextFieldId);
	}
	else
	{
		for(i = 0; i < BVDC_P_MAX_WINDOW_COUNT; i++)
		{
			if(hCompositor->ahWindow[i])
			{
				BVDC_Source_Handle hSource;

				/* Window detect window destroy done and set event. */
				BVDC_P_Window_UpdateState_isr(hCompositor->ahWindow[i]);

				/* invoke BVDC_P_Source_VfdGfxDataReady_isr for VFD that inputs gfx
				 * surface and works as an initial source of the video window, to
				 * perform the rectangle adjustment, vnet decision, picture node
				 * setup, and misc stuff that normally done in the source interrupt
				 * _isr, such as MpegDataReady_isr and AnalogDataReady_isr.
				 * note: in this case there is no source interrupt, and one vfd can
				 * not be shared by more than one window. */
				hSource = hCompositor->ahWindow[i]->stCurInfo.hSource;
				if (hSource && hSource->hVfdFeeder &&
					BVDC_P_STATE_IS_ACTIVE(hSource))
				{
					BVDC_P_Source_VfdGfxDataReady_isr(
						hSource, hCompositor->ahWindow[i], &stList, iParam2);
				}
			}
		}

		/* A disconnected window causes an mfd source to be orphan and needs a new
		 * compositor to drive it.  Since mfd's slots could be driven by t/b/f,
		 * we need to make it complete the last execution before transferring
		 * the force trigger (driving the mfd job) to this compositor.  The
		 * mfd _isr will release the semaphore (ulTransferLock == 0) to signal it's
		 * done cleaning up. */
		if(hCompositor->hSrcToBeLocked)
		{
			BDBG_MSG(("cmp[%d] trying to lock new orphan mfd[%d]",
				hCompositor->eId, hCompositor->hSrcToBeLocked->eId));
			if(!hCompositor->hSrcToBeLocked->ulTransferLock)
			{
				BVDC_P_Source_FindLockWindow_isr(hCompositor->hSrcToBeLocked, true);
			}
			else
			{
				/* This is a backup to prevent infinite wait for lock transfer in case
				 * the critical mpeg callback is missing during the synclock transfer process; */
				BDBG_MSG(("Clean up mfd[%d] slots before transfer to new cmp %d",
					hCompositor->hSrcToBeLocked->eId, hCompositor->hSrcToBeLocked->ulTransferLock, hCompositor->eId));
				if(--hCompositor->hSrcToBeLocked->ulTransferLock == 0)
				{
					/* clean up source slots to get rid of leftover source RULs for old
					   source/windows config; */
					BVDC_P_Source_CleanupSlots_isr(hCompositor->hSrcToBeLocked);
				}
			}
		}

		/* If Compositor/Display has at least one sync-lock source it will be the
		 * source isr that build the RUL.  This return true if it has sync-lock
		 * source.  In that case it will just build the RUL to force trigger the
		 * source slot. */
		if(hCompositor->hSyncLockSrc)
		{
			/* this is to prevent gfx flash for slip2lock transitioned display */
			if(hCompositor->ulSlip2Lock)
			{
				BDBG_MSG(("cmp[%d] build slip parts for mfd[%d]",
					hCompositor->eId, hCompositor->hSyncLockSrc->eId));
				BVDC_P_Compositor_BuildSyncSlipRul_isr(hCompositor, &stList, eNextFieldId, true);
			}
			BVDC_P_Compositor_BuildSyncLockRul_isr(hCompositor, &stList, eNextFieldId);
		}
		else
		{
			BVDC_P_Compositor_BuildSyncSlipRul_isr(hCompositor, &stList, eNextFieldId, true);
		}

		/* Poll for unplug sources. */
		for(i = 0; i < BVDC_P_MAX_SOURCE_COUNT; i++)
		{
			/* Is this the compositor going to handle polling? */
			if(hCompositor != hCompositor->hVdc->hCmpCheckSource)
			{
				break;
			}

			/* This compositor will check for source that lost triggers or
			 * otherwise no longer function. */
			if((BVDC_P_STATE_IS_ACTIVE(hCompositor->hVdc->ahSource[i])) &&
			   (hCompositor->hVdc->ahSource[i]->hTrigger0Cb) &&
			   (hCompositor->hVdc->ahSource[i]->hTrigger1Cb))
			{
				BVDC_Source_Handle hSource = hCompositor->hVdc->ahSource[i];
				BDBG_OBJECT_ASSERT(hSource, BVDC_SRC);
				/* does the source still have control? */
				if((BVDC_P_TriggerCtrl_eSource == hSource->eTrigCtrl) &&
				   (hSource->ulVecTrigger))
				{
					hSource->ulVecTrigger--;
					if(!hSource->ulVecTrigger)
					{
						uint32_t i;
						/* Remove the triggers from the slots, because VEC is
						 * going generate artificial triggers. */
						for(i = 0; i < hSource->ulSlotUsed; i++)
						{
							BRDC_Slot_ExecuteOnTrigger_isr(hSource->ahSlot[i],
								BRDC_Trigger_UNKNOWN, true);
						}

						/* start the faked field trigger from display; */
						hSource->eNextFieldFake = hSource->eNextFieldIntP;

						/* Make sure the slot are clean, especially if the last
						 * RUL is the one that cause the source to lose triggers. */
						BVDC_P_Source_CleanupSlots_isr(hSource);

						/* Turn trigger _isr callback to detect when trigger
						 * cames back. */
						hSource->eTrigCtrl    = BVDC_P_TriggerCtrl_eDisplay;
						hSource->ulVecTrigger = BVDC_P_TRIGGER_LOST_THRESHOLD;
						BINT_ClearCallback_isr(hSource->hTrigger0Cb);
						BINT_ClearCallback_isr(hSource->hTrigger1Cb);
						BINT_EnableCallback_isr(hSource->hTrigger0Cb);
						BINT_EnableCallback_isr(hSource->hTrigger1Cb);
						BDBG_WRN(("(D) Display[%d] acquires control of source[%d]'s slots",
							hCompositor->hDisplay->eId, hSource->eId));
					}
				}
				else if(BVDC_P_TriggerCtrl_eXfering == hSource->eTrigCtrl)
				{
					hSource->eTrigCtrl = BVDC_P_TriggerCtrl_eSource;
				}
				else if(BVDC_P_TriggerCtrl_eDisplay == hSource->eTrigCtrl)
				{
					/* Force trigger the current source's format fieldid. */
					BVDC_P_BUILD_IMM_EXEC_OPS(stList.pulCurrent,
						hSource->aulImmTriggerAddr[hSource->eNextFieldFake]);
					/* advance the faked field trigger from display; */
					hSource->eNextFieldFake = BVDC_P_NEXT_POLARITY(hSource->eNextFieldFake);
				}
			}
		}
	}

	/* Updated lists count */
	BVDC_P_WriteListInfo_isr(&stList, hList);

#if BVDC_P_ORTHOGONAL_VEC
	if((BAVC_Polarity_eFrame == eNextFieldId) ||
		(BVDC_P_DISPLAY_NRT_STG(hCompositor->hDisplay) && (hCompositor->hSyncLockSrc)))
#else
	if(BAVC_Polarity_eFrame == eNextFieldId)
#endif
	{
		BRDC_Slot_SetList_isr(hSlot, hList);
	}
	else
	{
		BRDC_Slot_Handle hOtherSlot = BVDC_P_CMP_GET_SLOT(hCompositor,
			BVDC_P_NEXT_POLARITY(eNextFieldId));
		/* Note: to flush the cached RUL only once, call the Dual function
		   instead of two individual slot functions; */
		BRDC_Slot_SetListDual_isr(hSlot, hOtherSlot, hList);
	}

#if BVDC_P_SUPPORT_STG
	/* NRT STG host arm if this display isr builds the STG RUL. */
	/* only host arm for sync-slipped display, or the first time when slip transitioned to lock;
	 the reason not to host arm the 2nd time during transition is to avoid NRT trigger firing early
	 such that when source isr replaces sync-locked display slots with dummy RUL and before
	 the source slot is installed with any meaningful RUL the NRT trigger might fire dummy nop
	 RUL at both display and source slots which might stop the NRT trigger forever! */
	if(BVDC_P_DISPLAY_USED_STG(hCompositor->hDisplay->eMasterTg) &&
	   hCompositor->hDisplay->stCurInfo.bStgNonRealTime &&
	   (!hCompositor->hSyncLockSrc || (hCompositor->ulSlip2Lock==1)))
	{
		if(hCompositor->ulSlip2Lock==1) hCompositor->ulSlip2Lock++;
		BREG_Write32(hCompositor->hDisplay->hVdc->hRegister,
			BCHP_VIDEO_ENC_STG_0_HOST_ARM + hCompositor->hDisplay->ulStgRegOffset, 1);
	}
#endif

BVDC_P_CompositorDisplay_isr_Done:
	hCompositor->hVdc->ulInsideCs--;
	BDBG_LEAVE(BVDC_P_CompositorDisplay_isr);
	return;
}

/***************************************************************************
 * This function convert from percentage clip rect (left, right, top, bottom)
 * and width and height to the actual rect
 *     offset_x = width * left
 *     offset_y = height * top
 *     size_x   = width - (width * left + width * right)
 *     size_y   = height - (height * top + height * bottom)
 */
void BVDC_P_CalculateRect_isr
	( const BVDC_ClipRect             *pClipRect,
	  uint32_t                         ulWidth,
	  uint32_t                         ulHeight,
	  bool                             bInterlaced,
	  BVDC_P_Rect                     *pRect )
{
	pRect->lLeft    = ulWidth * pClipRect->ulLeft / BVDC_P_CLIPRECT_PERCENT;
	pRect->lTop     = (ulHeight >> bInterlaced) * pClipRect->ulTop / BVDC_P_CLIPRECT_PERCENT;
	pRect->ulWidth  = ulWidth -(pRect->lLeft +
		ulWidth * pClipRect->ulRight / BVDC_P_CLIPRECT_PERCENT);
	pRect->ulHeight = (ulHeight >> bInterlaced) -
		(pRect->lTop + (ulHeight >> bInterlaced) * pClipRect->ulBottom / BVDC_P_CLIPRECT_PERCENT);
}

/***************************************************************************
 * This function checks if a callback's dirty bits are dirty.
 */
bool BVDC_P_CbIsDirty
	(void                           *pDirty,
	 uint32_t                        ulSize )
{
	bool bIsDirty = false;
	static const uint8_t aulZero[BVDC_P_DIRTY_INT_ARRAY_SIZE * sizeof(uint32_t)]={0};

	BDBG_ASSERT(ulSize <= sizeof(aulZero));

	if (ulSize <= sizeof(aulZero))
	{
		bIsDirty = BKNI_Memcmp(pDirty, aulZero, ulSize)? true : false;
	}

	return bIsDirty;
}

/***************************************************************************
 *
 * Utility function called by BVDC_P_Window_AspectRatioCorrection_isr and
 * BVDC_P_Window_CalcuUserDisplaySize_isr, and BVDC_P_Display_CalPixelAspectRatio_isr
 * to calculate the U4.16 fixed point format aspect ratio of a PIXEL
 *
 * note: pixel asp ratio range is well bounded ( <16, i.e. 4 int bits ), so
 * calcu it first, and also it could have more frac bits than the asp ratio
 * of a sub-rect (that is not well bounded).
 */
void BVDC_P_CalcuPixelAspectRatio_isr(
	BFMT_AspectRatio				 eFullAspectRatio,	   /* full asp ratio enum */
	uint32_t						 ulSampleAspectRatioX, /* width of one sampled src pixel */
	uint32_t						 ulSampleAspectRatioY, /* height of one sampled src pixel */
	uint32_t						 ulFullWidth,		   /* full asp ratio width */
	uint32_t						 ulFullHeight,		   /* full asp ratio height */
	const BVDC_P_ClipRect * 		 pAspRatCnvsClip,	   /* asp rat cnvs clip */
	uintAR_t *						 pulPxlAspRatio,       /* PxlAspR_int.PxlAspR_frac */
	uint32_t *                       pulPxlAspRatio_x_y    /* PxlAspR_x<<16 | PxlAspR_y */
)
{
	uint32_t  ulAspRatCnvsWidth, ulAspRatCnvsHeight;
	uintAR_t  ulPixAspRatio = 0;
	uint16_t  uiPixAspR_x=0, uiPixAspR_y=0;
#if BVDC_P_SUPPORT_STG
	uint32_t b=0, a=0, m=0, i=0;
#endif
	BDBG_ASSERT((NULL != pulPxlAspRatio) && (NULL != pulPxlAspRatio_x_y));
	BDBG_ASSERT(NULL != pAspRatCnvsClip);

	ulAspRatCnvsWidth  = ulFullWidth  - (pAspRatCnvsClip->ulLeft + pAspRatCnvsClip->ulRight);
	ulAspRatCnvsHeight = ulFullHeight - (pAspRatCnvsClip->ulTop  + pAspRatCnvsClip->ulBottom);

	/* Set default value for unknown aspect ratio. */
	if(BVDC_P_IS_UNKNOWN_ASPR(eFullAspectRatio, ulSampleAspectRatioX, ulSampleAspectRatioY))
	{
		uint32_t ulHVRatio = (ulFullWidth * 100) / ulFullHeight;
		eFullAspectRatio = BVDC_P_EQ_DELTA(ulHVRatio, 130, 25)
			? BFMT_AspectRatio_e4_3 : BFMT_AspectRatio_eSquarePxl;
	}

	/* Pay attention to overflow, assuming ulAspRatCnvsHeight could be as big as 1080 */
	switch (eFullAspectRatio)
	{
	case BFMT_AspectRatio_eSquarePxl:
		BDBG_MSG(("BFMT_AspectRatio_eSquarePxl"));
		uiPixAspR_x = 1;
		uiPixAspR_y = 1;
		break;
	case BFMT_AspectRatio_e4_3:
		BDBG_MSG(("BFMT_AspectRatio_e4_3"));
		uiPixAspR_x = ulAspRatCnvsHeight * 4;
		uiPixAspR_y = ulAspRatCnvsWidth * 3;
		break;
	case BFMT_AspectRatio_e16_9:
		BDBG_MSG(("BFMT_AspectRatio_e16_9"));
		uiPixAspR_x = ulAspRatCnvsHeight * 16;
		uiPixAspR_y = ulAspRatCnvsWidth * 9;
		break;
	case BFMT_AspectRatio_e221_1:
		BDBG_MSG(("BFMT_AspectRatio_e221_1"));
		uiPixAspR_x = (ulAspRatCnvsHeight * 221) >> 3;
		uiPixAspR_y = (ulAspRatCnvsWidth  * 100) >> 3;
		ulPixAspRatio = ((((uintAR_t)ulAspRatCnvsHeight << BVDC_P_ASPR_FRAC_BITS_NUM) * 2) / (ulAspRatCnvsWidth) +
						 (((uintAR_t)ulAspRatCnvsHeight << BVDC_P_ASPR_FRAC_BITS_NUM) * 21) / (100 * ulAspRatCnvsWidth));
		break;
	case BFMT_AspectRatio_e15_9:
		BDBG_MSG(("BFMT_AspectRatio_e15_9"));
		uiPixAspR_x = ulAspRatCnvsHeight * 15;
		uiPixAspR_y = ulAspRatCnvsWidth * 9;
		break;
	case BFMT_AspectRatio_eSAR:
		BDBG_MSG(("BFMT_AspectRatio_eSAR: %d, %d", ulSampleAspectRatioX, ulSampleAspectRatioY));
		uiPixAspR_x = ulSampleAspectRatioX;
		uiPixAspR_y = ulSampleAspectRatioY;
		break;
	default:
		uiPixAspR_x = 1;
		uiPixAspR_y = 1;
		BDBG_ERR(("Bad asp ratio enum %d", eFullAspectRatio));
		break;
	}

	if(uiPixAspR_y == uiPixAspR_x)
	{
		uiPixAspR_y = uiPixAspR_x = 1;
	}
#if BVDC_P_SUPPORT_STG
	/* Euclidean gcd algorithm */
	else
	{
		a = uiPixAspR_y > uiPixAspR_x ? uiPixAspR_y:uiPixAspR_x;
		b = uiPixAspR_y > uiPixAspR_x ? uiPixAspR_x:uiPixAspR_y;

		while (b  && (i<10)) { m = a % b; a = b; b = m; i++;}

		if (i<10) {
			uiPixAspR_y/=a;
			uiPixAspR_x/=a;
		}
	}
#endif

	if (BFMT_AspectRatio_e221_1 != eFullAspectRatio)
	{
		ulPixAspRatio = ((uintAR_t)uiPixAspR_x << BVDC_P_ASPR_FRAC_BITS_NUM) / (uiPixAspR_y);
	}

	*pulPxlAspRatio = ulPixAspRatio;
	*pulPxlAspRatio_x_y = ((uint32_t)uiPixAspR_x <<16) | uiPixAspR_y;

	BDBG_MSG(("pxlAspR: %8x %8x", pulPxlAspRatio, pulPxlAspRatio_x_y));
}


/***************************************************************************
 *
 */
BERR_Code BVDC_GetCapabilities
	( BVDC_Handle                      hVdc,
	  BVDC_Capabilities               *pCapabilities )
{
	uint32_t i;
	BDBG_ENTER(BVDC_GetCapabilities);

	BDBG_OBJECT_ASSERT(hVdc, BVDC_VDC);

	if(pCapabilities)
	{
		BKNI_Memset(pCapabilities, 0x0, sizeof(BVDC_Capabilities));

		for(i = 0; i < BVDC_P_MAX_COMPOSITOR_COUNT; i++)
		{
			if(hVdc->pFeatures->abAvailCmp[i])
			{
				pCapabilities->ulNumCmp++;
			}
		}

		pCapabilities->ulNumMad = BVDC_P_SUPPORT_MAD + BVDC_P_SUPPORT_MCVP;
		pCapabilities->ulNumDnr = BVDC_P_SUPPORT_DNR;
		pCapabilities->ulNumPep = BVDC_P_SUPPORT_PEP;
		pCapabilities->ulNumTab = BVDC_P_SUPPORT_TAB + BVDC_P_SUPPORT_TNT;
		pCapabilities->ulNumDac = BVDC_P_MAX_DACS;
		pCapabilities->ulNumRfm = BVDC_P_SUPPORT_RFM_OUTPUT;
		pCapabilities->ulNumStg = BVDC_P_SUPPORT_STG;

		pCapabilities->ulNum656Input   = BVDC_P_NUM_656IN_SUPPORT;
		pCapabilities->ulNum656Output  = BVDC_P_SUPPORT_ITU656_OUT;

		pCapabilities->ulNumHdmiInput  = BVDC_P_SUPPORT_HDDVI;
		pCapabilities->ulNumHdmiOutput = BVDC_P_SUPPORT_DVI_OUT;
		
		pCapabilities->b3DSupport        = BVDC_P_SUPPORT_3D_VIDEO;
	}

	BDBG_LEAVE(BVDC_GetCapabilities);
	return BERR_SUCCESS;
}


/***************************************************************************
 *
 * Utility function called by BVDC_Source_GetCapabilities
 */
bool  BVDC_P_IsPxlfmtSupported
	(BPXL_Format                       ePxlFmt)
{

	if(!BPXL_IS_YCbCr422_FORMAT(ePxlFmt)
#if (BVDC_P_SUPPORT_BVN_10BIT_444)
	 && !BPXL_IS_YCbCr422_10BIT_FORMAT(ePxlFmt)
	 && !BPXL_IS_YCbCr444_10BIT_FORMAT(ePxlFmt)
#endif
	)
	{
		return false;
	}

#if (BVDC_P_MFD_SUPPORT_BYTE_ORDER)
	/* Can support all formats with MFD_SUPPORT_BYTE_ORDER */
	return true;

#else
	/* Old chips: only limited formats are supported */
#if (BSTD_CPU_ENDIAN == BSTD_ENDIAN_LITTLE)
	if ((ePxlFmt == BPXL_eY08_Cr8_Y18_Cb8) ||
		(ePxlFmt == BPXL_eCr8_Y08_Cb8_Y18) ||
		(ePxlFmt == BPXL_eY08_Cb8_Y18_Cr8) ||
		(ePxlFmt == BPXL_eCb8_Y08_Cr8_Y18))
#else
	if ((ePxlFmt == BPXL_eCb8_Y18_Cr8_Y08) ||
		(ePxlFmt == BPXL_eY18_Cb8_Y08_Cr8) ||
		(ePxlFmt == BPXL_eCr8_Y18_Cb8_Y08) ||
		(ePxlFmt == BPXL_eY18_Cr8_Y08_Cb8))
#endif
	{
		return false;
	}
	else
	{
		return true;
	}
#endif
}


/* End of file. */
