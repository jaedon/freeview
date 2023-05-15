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
 * $brcm_Revision: Hydra_Software_Devel/217 $
 * $brcm_Date: 8/15/12 6:20p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vdc/src/modularvec/bvdc_display_priv.h $
 * 
 * Hydra_Software_Devel/217   8/15/12 6:20p tdo
 * SW7445-8: Create Appframework emulation environment for 7445 A0
 *
 * Hydra_Software_Devel/216   7/27/12 3:44p vanessah
 * SW7425-3150: adding Bypass Video Processing support
 *
 * Hydra_Software_Devel/215   12/13/11 5:11p yuxiaz
 * SW7425-1875: Added support for source buffer selection in 3d mode.
 *
 * Hydra_Software_Devel/214   8/28/11 5:00p yuxiaz
 * SW7425-1170: Fixed build on 7405
 *
 * Hydra_Software_Devel/213   7/18/11 5:31p pntruong
 * SW7231-284, SW7346-295, SW7405-5339: Refactored master framerate
 * tracking, so that it can also handle validation.
 *
 * Hydra_Software_Devel/212   7/18/11 11:20a vanessah
 * SW7425-835: SW7425-923: fix ViCE2 channel id bug + B0 STG
 *
 * Hydra_Software_Devel/211   7/11/11 2:17p tdo
 * SW7420-1971: Video Pause seen when VEC alignment is going on.  Add flag
 * to keep BVN connected while doing alignment.
 *
 * Hydra_Software_Devel/210   7/8/11 6:01p albertl
 * SW7408-291: Updated VF filter API to use Dac enums for channel
 * selection.
 *
 * Hydra_Software_Devel/209   6/24/11 2:58p albertl
 * SW7420-1966: Cleaned up and reorganized colorspace enums and indicies
 * in display, vdec, and cmp.
 *
 * Hydra_Software_Devel/208   6/22/11 4:17p albertl
 * SW7408-291: Fixed missing vf filter fields.
 *
 * Hydra_Software_Devel/207   6/17/11 2:32p albertl
 * SW7408-291:  Added interface to set and get VF filter settings.
 *
 * Hydra_Software_Devel/206   5/24/11 7:30p albertl
 * SW7425-169: Converted 10-bit and 8-bit filters to compact hex format
 * and updated to use latest recommended filter coefficients.
 *
 * Hydra_Software_Devel/205   1/20/11 3:19p yuxiaz
 * SW7422-97: Clean up macros for HDMI Rate manage
 *
 * Hydra_Software_Devel/204   1/20/11 2:56p yuxiaz
 * SW7422-97: Clean up macros for HDMI Rate manage
 *
 * Hydra_Software_Devel/203   6/16/10 6:33p tdo
 * SW7400-2770: Add multi-frame rate support to SD also
 *
 * Hydra_Software_Devel/202   5/10/10 3:33p albertl
 * SW7125-364: Fixed compile error for big endian.
 *
 * Hydra_Software_Devel/201   5/7/10 7:09p albertl
 * SW7125-364: Changed dirty bits to use union structure to avoid type-pun
 * warnings.
 *
 * Hydra_Software_Devel/200   4/9/10 3:20p tdo
 * SW7420-673: add multi-frame rate support to SD
 *
 * Hydra_Software_Devel/199   4/7/10 11:28a tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic.  Rename TLA
 *
 * Hydra_Software_Devel/198   4/5/10 4:04p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic
 *
 * Hydra_Software_Devel/197   2/23/10 3:09p darnstein
 * SW7400-2620: add support for double rate 480P.
 *
 * Hydra_Software_Devel/196   1/29/10 8:38p albertl
 * SW7405-3845: Fixed PAL_NC matrices and renamed associated colorspace as
 * YUV_NC.
 *
 * Hydra_Software_Devel/195   1/21/10 7:25p darnstein
 * SW7325-389: change symbol from BVBI_P_WSE_VER3 to BVDC_P_WSE_VER3.
 *
 * Hydra_Software_Devel/194   1/11/10 1:17p rpan
 * SW7400-2620: Undo HDMI pixel replication. The 7400/7405 HDMI core
 * doesn't support this feature.
 *
 * Hydra_Software_Devel/193   12/11/09 3:41p rpan
 * SW7400-2620: Added HDMI pixel replication for 480p display. This is for
 * chips such as 7400 and 7405 only.
 *
 * Hydra_Software_Devel/192   10/16/09 5:20p darnstein
 * SW7405-3191: Back out all changes in pixel frequency handling. New
 * design coming soon.
 *
 * Hydra_Software_Devel/191   10/15/09 4:48p darnstein
 * SW7405-3191: Pixel frequency is now defined as both a bitmask, and an
 * enum. The bitmask option is DEPRECATED.
 *
 * Hydra_Software_Devel/190   10/13/09 5:47p darnstein
 * SW7405-3004: install 480P ARIB output via BVDC display settings.
 *
 * Hydra_Software_Devel/189   6/16/09 3:32p pntruong
 * PR56071: Initialize dac to off in vdc open.
 *
 * Hydra_Software_Devel/188   6/2/09 3:21p darnstein
 * PR48884: More resistance to invalid user inputs.
 *
 * Hydra_Software_Devel/187   5/4/09 6:00p pntruong
 * PR53954: B2: DVP LVDS spread spectrum feature causes a DC shift in
 * frequency and an irregular spreading.  Compiled out for b2.
 *
 * Hydra_Software_Devel/186   4/2/09 6:09p darnstein
 * PR51325: Workaround for WSE bug: toggle VF_MISC.VBI_ENABLE every video
 * field.
 *
 * Hydra_Software_Devel/185   4/1/09 2:58p pntruong
 * PR53584,PR46645: Need to delay kick start when lvds/dvpo reprogram.
 *
 * Hydra_Software_Devel/184   3/31/09 3:30p hongtaoz
 * PR53087, PR53687: defined display alignment threshold in usecs;
 *
 * Hydra_Software_Devel/183   3/30/09 2:21p hongtaoz
 * PR53037, PR53038: Implement the VEC Locking/Alignments For Memory
 * Reduction; Implement Double-Capture-Buffer Algorithm Under VEC Locking
 * Scheme
 *
 * Hydra_Software_Devel/182   3/26/09 2:34p yuxiaz
 * PR52179: Video Rate Change callback invoked continously
 *
 * Hydra_Software_Devel/181   11/24/08 5:02p rpan
 * PR45804: Move VEC reset to display module.
 *
 * Hydra_Software_Devel/180   11/17/08 4:11p darnstein
 * PR45819: Update scratch register trickery to support ITU-R 656 output,
 * in addition to analog output.
 *
 * Hydra_Software_Devel/179   10/27/08 11:55p tdo
 * PR46484: Clean up VEC code
 *
 * Hydra_Software_Devel/178   10/24/08 2:45a albertl
 * PR48023:  Added BVDC_Display_SetDvoAttenuationRGB and
 * BVDC_Display_GetDvoAttenuationRGB, enabling CSC adjustment on DVO
 * path.  Further cleaned up CSC matrix multiplication code.
 *
 * Hydra_Software_Devel/177   10/17/08 2:19p tdo
 * PR47744: Fix compiling error for chips not supporting DVI CCB
 *
 * Hydra_Software_Devel/176   10/17/08 2:12p tdo
 * PR47744: Provide smooth updating of CCB table
 *
 * Hydra_Software_Devel/175   10/9/08 5:28p syang
 * PR 46891: add _isr to the name of func used in _isr context
 *
 * Hydra_Software_Devel/174   9/29/08 9:20p pntruong
 * PR45196: Fixed triggering of reloading ucode for custom timing.
 *
 * Hydra_Software_Devel/173   9/24/08 7:50p darnstein
 * PR46949: DCS ON/OFF no longer "flashes." This is the 97400 check in.
 *
 * Hydra_Software_Devel/172   9/10/08 7:50p pntruong
 * PR46546, PR46584: Dynamic power management of video DAC blocks.
 *
 * Hydra_Software_Devel/171   8/22/08 10:31a rpan
 * PR45979: Remove VBI passthru support.
 *
 * Hydra_Software_Devel/170   8/8/08 3:28p yuxiaz
 * PR45484: Enable Dithering in VDC.
 *
 * Hydra_Software_Devel/169   6/11/08 1:40p yuxiaz
 * PR39158: More dither settings in VEC.
 *
 * Hydra_Software_Devel/168   5/27/08 7:05p hongtaoz
 * PR42248, PR42494: added display alignment API;
 *
 * Hydra_Software_Devel/167   5/16/08 2:17p rpan
 * PR42109: More SECAM variants support.
 *
 * Hydra_Software_Devel/166   5/16/08 1:59p rpan
 * PR42109: SECAM variants support.
 *
 * Hydra_Software_Devel/165   4/24/08 2:45p hongtaoz
 * PR32130: optimized RM adjustment for game mode support;
 *
 * Hydra_Software_Devel/164   3/17/08 4:01p darnstein
 * PR25971: Declare a function used for DCS.
 *
 * Hydra_Software_Devel/163   3/10/08 9:32p darnstein
 * PR25971: first try at DCS for 7400
 *
 * Hydra_Software_Devel/162   2/19/08 10:39a rpan
 * PR39421: Added DVI color converter block support to bcm3548.
 *
 * Hydra_Software_Devel/161   2/11/08 8:44p pntruong
 * PR39177: [VEC] Need new display api to specify 60.00Hz or 59.94Hz
 * (framedrop).
 *
 * Hydra_Software_Devel/160   1/13/08 3:29a albertl
 * PR38150: Added 7440 HDMI xvYCC CSC support.
 *
 * Hydra_Software_Devel/159   11/2/07 7:34p albertl
 * PR36701:  Added MUD functionality for 7440 C0.
 *
 * Hydra_Software_Devel/158   10/31/07 3:07p pntruong
 * PR28459: Need heap added to VDC's display or compositor.
 *
 * Hydra_Software_Devel/157   10/30/07 7:14p pntruong
 * PR34239: Allow dynamically loading of vec custom timing.
 *
 * Hydra_Software_Devel/156   10/8/07 2:38p pntruong
 * PR34855: Initial VDC bringup.
 *
 * Hydra_Software_Devel/155   8/15/07 8:40p pntruong
 * PR28892: Fixed window surface field id.  It should be base on the
 * current display field id.
 *
 * Hydra_Software_Devel/154   6/28/07 7:25p hongtaoz
 * PR32130, PR29189: added buffer game mode delay control; fixed picture
 * timestamp update bug when isr reorder occurs; invalid timestamp/drift
 * delay should disappear now; lipsync delay reflects the correct game
 * mode effect;
 *
 * Hydra_Software_Devel/VDC_Game_Mode/1   6/26/07 2:22p hongtaoz
 * PR32130: added window game mode buffer delay control;
 *
 * Hydra_Software_Devel/153   4/9/07 11:25a pntruong
 * PR28266: Provide API to customize DVO CSC.
 *
 * Hydra_Software_Devel/152   3/27/07 5:36p pntruong
 * PR28836: API to control H/V-Sync polarity control of DVO independently.
 *
 * Hydra_Software_Devel/151   3/26/07 11:02p tdo
 * PR 28402: Provide API to load customized CCB table for whitebalance
 *
 * Hydra_Software_Devel/150   2/15/07 4:03p pntruong
 * PR25235: Need to use actual pixel frequency and vertical frequency
 * fields in BFMT instead of the MASK.
 *
 * Hydra_Software_Devel/149   2/6/07 3:22p tdo
 * PR 27576: Move CAB table and DVI_CCB table program out of critical
 * section
 *
 * Hydra_Software_Devel/148   1/18/07 11:35a hongtaoz
 * PR23260: disable display triggers at destroy for clean shutdown;
 *
 * Hydra_Software_Devel/147   12/21/06 7:12p hongtaoz
 * PR25668, PR22768: added API access of RDC scratch registers;
 *
 * Hydra_Software_Devel/146   12/19/06 3:56p hongtaoz
 * PR25668: added 7403 support for VDC; further consolidate chip specific
 * ifdefs to bvdc_common_priv.h;
 *
 * Hydra_Software_Devel/145   12/18/06 11:19p pntruong
 * PR22577: Merged back to mainline.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/11   12/13/06 5:22p hongtaoz
 * PR26039: updated 480i/1080i/720p analog microcode for the new shaper
 * settings;
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/10   11/16/06 1:57p hongtaoz
 * PR25668: merge in vdc support for 7403;
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/9   11/15/06 5:07p albertl
 * PR24587:  Added downsample source for 7440.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/8   11/6/06 11:27a pntruong
 * PR25113: Show blue screen on HDMI without affecting component output
 * when HDCP fails.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/7   11/3/06 4:14p pntruong
 * PR25113: Show blue screen on HDMI without affecting component output
 * when HDCP fails.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/6   11/1/06 2:26p tdo
 * PR 24457:  Add color temp to white balance support for DVO output
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/5   10/23/06 2:33p tdo
 * PR 24457: Add white balance support
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/4   10/19/06 6:59p hongtaoz
 * PR23260: add dvo master mode support; add support for cmp/display
 * connection at create time; restructured display code to be scalable
 * for 7400B support;
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/3   10/9/06 4:28p maivu
 * PR 23173: Added BVDC_Display_SetCallbackSettings/GetCallbackSettings
 * support
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/2   9/29/06 2:17p tdo
 * PR 24608:  Add 240p mode support
 *
 * Hydra_Software_Devel/140   8/29/06 11:00a hongtaoz
 * PR23246: add new QDAC support for 3563; centralize some chip-specific
 * display macroes into bvdc_common_priv.h;
 *
 * Hydra_Software_Devel/139   8/18/06 4:22p albertl
 * PR23117:  Added 7440 support.
 *
 * Hydra_Software_Devel/138   8/7/06 5:29p pntruong
 * PR22577: Initial bringup of VDC.
 *
 * Hydra_Software_Devel/137   6/30/06 5:44p hongtaoz
 * PR22081, PR20395, PR22062, PR22246: added SECAM Macrovision support;
 * fixed PAL_M dvi microcode; fixed PAL_M sub-carrier frequency setting;
 * PAL_NC should use the same microcode as normal PAL; fixed SECAM and
 * various PAL's csc matrices according to BT.470-6 spec; changed SRC
 * filter setting to beneifit higher freq response for HD output on
 * PRIM_VEC; fixed PAL sync tip setting for non-Macrovision build;
 *
 * Hydra_Software_Devel/136   6/23/06 6:33p hongtaoz
 * PR22246: some VEC csc, sync, offset values are incorrect;
 *
 * Hydra_Software_Devel/135   6/14/06 1:37p syang
 * PR 21689: add support for 7118
 *
 * Hydra_Software_Devel/134   6/12/06 11:10a hongtaoz
 * PR22081: bring up SECAM; added SECAM color matrix;
 *
 * Hydra_Software_Devel/133   5/26/06 3:38p pntruong
 * PR20642: Refactored handling of hList.
 *
 * Hydra_Software_Devel/132   5/19/06 1:46p pntruong
 * PR21560: Global symbols without BVDC prefix in the BVDC module.  Reduce
 * unnecessary global exported symbols.  Tighten const params.
 *
 * Hydra_Software_Devel/131   5/9/06 1:34p tdo
 * PR 21170: Add support for 7401 B0
 *
 * Hydra_Software_Devel/130   4/4/06 1:48p hongtaoz
 * PR20403: add triple displays for 7400;
 *
 * Hydra_Software_Devel/129   3/7/06 3:59p syang
 * PR 19670: added 7438 support
 *
 * Hydra_Software_Devel/128   1/31/06 5:21p pntruong
 * PR17778: Take in changes for new hddvi.
 *
 * Hydra_Software_Devel/124   1/17/06 4:18p hongtaoz
 * PR19082: first compiled for 7400;
 *
 * Hydra_Software_Devel/123   11/29/05 4:57p hongtaoz
 * PR16298, PR16781, PR17804, PR18025, PR18232: implemented 704-sample
 * NTSC analog output; added two more Macrovision control modes; added
 * RGB SCART mode (G channel w/o sync) DAC config for Macrovision
 * validation test;
 *
 * Hydra_Software_Devel/122   10/11/05 5:48p albertl
 * PR17544:  Debug messages now display digital and analog microcode
 * timestamp and checksums when they're loaded.
 *
 * Hydra_Software_Devel/121   9/21/05 6:08p albertl
 * PR16459:  Display format code restructured, moved to
 * bvdc_displayfmt_priv.c and bvdc_displayfmt.h, and changed to be  table
 * driven  to allow ease of adding new formats.
 *
 * Hydra_Software_Devel/120   9/12/05 2:23p hongtaoz
 * PR16822, PR15749: updated PAL microcode; fixed Macrovision
 * certification control process;
 *
 * Hydra_Software_Devel/119   8/18/05 1:14p pntruong
 * PR15757, PR16391, PR16411, PR12519, PR14791, PR15535, PR15206, PR15778:
 * Improved bandwidth for cropping/scaler/capture/playback.   Unified
 * window shutdown sequence for destroy, reconfigure mad/scaler, and/or
 * reconfigure result of source changes.  And miscellances fixes from
 * above PRs.
 *
 * Hydra_Software_Devel/XVDPhase1/XVDPhase1_merge/1   8/15/05 12:06p hongtaoz
 * PR15749, PR15888:  added Macrovision support for new test process, i.e.
 * toggling of certain CPC control bits; added bypass display's 656 CSC
 * matrices;
 *
 * Hydra_Software_Devel/118   8/14/05 1:15p hongtaoz
 * PR15749, PR15888: added Macrovision support for new test process, i.e.
 * toggling of certain CPC control bits; added bypass display's 656 CSC
 * matrices;
 *
 * Hydra_Software_Devel/117   8/4/05 10:45a syang
 * PR 16536: refine some "#if (CHIP=xxxx)"  coding for 7401
 *
 * Hydra_Software_Devel/116   8/1/05 4:05p hongtaoz
 * PR16142, PR16394: added 656in analog out vbi pass-thru support for NTSC
 * format; fixed simul mode vdec vbi pass-thru; updated vec NTSC
 * microcode(080105); software workaround 656in hibernation mode
 * artifact;
 *
 * Hydra_Software_Devel/115   7/18/05 6:12p hongtaoz
 * PR15630, PR16218: sychronize VBI encoder control registers programming
 * through scratch registers between BVDC and BVBI;
 *
 * Hydra_Software_Devel/114   6/21/05 7:46p albertl
 * PR15385:  Fixed HDMI display anomalies that occur during format
 * switches.
 *
 * Hydra_Software_Devel/113   5/3/05 2:50p hongtaoz
 * PR14976: added full range RGB coloremitry outputs support for DVI
 * monitor; fixed DVI response to the source dynamic color space change;
 *
 * Hydra_Software_Devel/112   3/30/05 10:42a syang
 * PR 14537: added BVB_SAV_REMOVE setting for all chips, and added
 * SAV_REPLICATE setting for 7018 C and later chips.
 *
 * Hydra_Software_Devel/111   3/18/05 6:30p pntruong
 * PR14494: Basic vdc up and running.  Background and passed output format
 * switch test.
 *
 * Hydra_Software_Devel/110   3/17/05 6:36p pntruong
 * PR14494: Add preliminary software support to 3560 A0.
 *
 * Hydra_Software_Devel/109   3/4/05 2:07p albertl
 * PR13274:  Added BVDC_Display_SetHdmiDropLines and
 * BVDC_Display_GetHdmiDropLines for setting the number of compositor
 * lines to drop for hdmi output.
 *
 * Hydra_Software_Devel/108   2/9/05 2:54p albertl
 * PR12560: Added BFMT_VideoFmt_eDVI_1280x768p,
 * BFMT_VideoFmt_eDVI_1280x720p_50Hz, BFMT_VideoFmt_eDVI_1280x720p, and
 * BFMT_VideoFmt_eDVI_1280x720p_ReducedBlank formats.  Renamed format
 * tables for readability and pixel rate defines for clarity.
 *
 * Hydra_Software_Devel/107   1/31/05 3:13p albertl
 * PR7664: Updated phase offset value.
 *
 * Hydra_Software_Devel/106   1/31/05 3:01p albertl
 * PR7664:  Added definitions for phase values.
 *
 * Hydra_Software_Devel/105   1/26/05 4:52p pntruong
 * PR13450, PR12854, PR13549, PR13617, PR13618, PR13683, PR13321, PR13646,
 * PR13447, PR13429: Disabled vec triggers when vec reset (e.g. format
 * change) to prevent it from continuosly executing same register update
 * list that cause system locked up and/or causing vec errors.
 *
 ***************************************************************************/
#ifndef BVDC_DISPLAY_PRIV_H__
#define BVDC_DISPLAY_PRIV_H__

#include "bvdc.h"
#include "bvdc_priv.h"

#ifdef __cplusplus
extern "C" {
#endif

#if (BCHP_CHIP != 7420)
#include "bchp_prim_it.h"
#include "bchp_prim_vf.h"
#include "bchp_prim_sm.h"
#include "bchp_prim_csc.h"
#include "bchp_prim_rm.h"
#include "bchp_prim_src.h"

#if BVDC_P_SUPPORT_SEC_VEC
#include "bchp_sec_it.h"
#include "bchp_sec_vf.h"
#include "bchp_sec_sm.h"
#include "bchp_sec_csc.h"
#include "bchp_sec_rm.h"
#include "bchp_sec_src.h"
#endif

#if BVDC_P_SUPPORT_TER_VEC
#include "bchp_tert_it.h"
#include "bchp_tert_vf.h"
#include "bchp_tert_sm.h"
#include "bchp_tert_csc.h"
#include "bchp_tert_rm.h"
#include "bchp_tert_src.h"
#endif
#else
#include "bchp_it_0.h"
#include "bchp_vf_0.h"
#include "bchp_sm_0.h"
#include "bchp_csc_0.h"
#include "bchp_rm_0.h"
#include "bchp_sdsrc_0.h"
#include "bchp_secam_0.h"

#if BVDC_P_SUPPORT_SEC_VEC
#include "bchp_it_1.h"
#include "bchp_vf_1.h"
#include "bchp_sm_1.h"
#include "bchp_csc_1.h"
#include "bchp_rm_1.h"
#include "bchp_sdsrc_1.h"
#include "bchp_secam_1.h"
#endif

#if BVDC_P_SUPPORT_TER_VEC
#include "bchp_it_2.h"
#include "bchp_vf_2.h"
#include "bchp_sm_2.h"
#include "bchp_csc_2.h"
#include "bchp_rm_2.h"
#include "bchp_sdsrc_2.h"
#include "bchp_secam_2.h"
#endif
#endif /* 7420 */

#if BVDC_P_VEC_SUPPORT_DVI_COLOR_CNVT
#include "bchp_dvi_ccb.h"
#endif

/****************************************************************
 *  Defines
 ****************************************************************/
/* ---------------------------------------------
 * HDMI_RM revision
 * --------------------------------------------- */
/* 3548 Ax, B0, B1, 3556 Ax, B0, B1
 *  DVPO support
 */
#define BVDC_P_HDMI_RM_VER_0                 (0)

/* 3548 B2 and above, 3556 B2 and above
 *  DVPO support  with fixed spread spectrum
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

#define BVDC_P_MAX_VEC_RUL_ENTRIES          665
#define BVDC_P_RAM_TABLE_SIZE               256
#define BVDC_P_DTRAM_TABLE_SIZE            (128/2)
#define BVDC_P_RAM_TABLE_TIMESTAMP_IDX     (BVDC_P_RAM_TABLE_SIZE - 2)
#define BVDC_P_RAM_TABLE_CHECKSUM_IDX      (BVDC_P_RAM_TABLE_SIZE - 1)
#define BVDC_P_DTRAM_TABLE_TIMESTAMP_IDX   (BVDC_P_DTRAM_TABLE_SIZE - 2)
#define BVDC_P_DTRAM_TABLE_CHECKSUM_IDX    (BVDC_P_DTRAM_TABLE_SIZE - 1)
#if BVDC_P_VEC_SUPPORT_DVI_COLOR_CNVT
#define BVDC_P_CCB_TABLE_SIZE              (BCHP_DVI_CCB_CCB_ELEMENT_i_ARRAY_END + 1)
#else
#define BVDC_P_CCB_TABLE_SIZE              (1)
#endif

#if (BCHP_CHIP != 7420)
#define BVDC_P_CSC_TABLE_SIZE              (uint32_t)((BCHP_PRIM_CSC_CSC_COEFF_C23_C22 - BCHP_PRIM_CSC_CSC_MODE)/4+1)
#define BVDC_P_DITHER_TABLE_SIZE           (uint32_t)((BCHP_PRIM_CSC_DITHER_LFSR_INIT - BCHP_PRIM_CSC_DITHER_CONTROL)/4+1)

#define BVDC_P_VF_TABLE_SIZE              (uint32_t)((BCHP_PRIM_VF_SYNC_TRANS_1 - BCHP_PRIM_VF_FORMAT_ADDER)/4+1)
#define BVDC_P_CHROMA_TABLE_SIZE          (uint32_t)((BCHP_PRIM_VF_CH0_TAP10 - BCHP_PRIM_VF_CH0_TAP1_3)/4+1)
#define BVDC_P_RM_TABLE_SIZE              (uint32_t)((BCHP_PRIM_RM_INTEGRATOR - BCHP_PRIM_RM_RATE_RATIO)/4+1)
#define BVDC_P_IT_TABLE_SIZE              (uint32_t)((BCHP_PRIM_IT_PCL_5 - BCHP_PRIM_IT_ADDR_0_3)/4+1)
#define BVDC_P_SM_TABLE_SIZE              (uint32_t)((BCHP_PRIM_SM_COMP_CNTRL - BCHP_PRIM_SM_ENVELOPE_GENERATOR)/4+1)
#else
#define BVDC_P_CSC_TABLE_SIZE              (uint32_t)((BCHP_CSC_0_CSC_COEFF_C23_C22 - BCHP_CSC_0_CSC_MODE)/4+1)
#define BVDC_P_DITHER_TABLE_SIZE           (uint32_t)((BCHP_CSC_0_DITHER_LFSR_INIT - BCHP_CSC_0_DITHER_CONTROL)/4+1)

#define BVDC_P_VF_TABLE_SIZE              (uint32_t)((BCHP_VF_0_SYNC_TRANS_1 - BCHP_VF_0_FORMAT_ADDER)/4+1)
#define BVDC_P_CHROMA_TABLE_SIZE          (uint32_t)((BCHP_VF_0_CH0_TAP10 - BCHP_VF_0_CH0_TAP1)/4+1)
#define BVDC_P_RM_TABLE_SIZE              (uint32_t)((BCHP_RM_0_INTEGRATOR - BCHP_RM_0_RATE_RATIO)/4+1)
#define BVDC_P_IT_TABLE_SIZE              (uint32_t)((BCHP_IT_0_PCL_5 - BCHP_IT_0_ADDR_0_3)/4+1)
#define BVDC_P_SM_TABLE_SIZE              (uint32_t)((BCHP_SM_0_COMP_CNTRL - BCHP_SM_0_PG_CNTRL)/4+1)
#endif /* 7420 */

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

#define BVDC_P_DISP_IS_COMPONENT(eColorSpace) ((eColorSpace == BVDC_P_Output_eSDYPrPb) || \
											   (eColorSpace == BVDC_P_Output_eHDYPrPb) || \
											   (eColorSpace == BVDC_P_Output_eRGB) ||     \
											   (eColorSpace == BVDC_P_Output_eHsync))

#define BVDC_P_DISP_IS_VALID_DISPOUTPUT_AND_DAC(eDispOutput, eDacOutput)                                                         \
	(((eDisplayOutput == BVDC_DisplayOutput_eComponent) &&                                                                       \
	  ((eDacOutput == BVDC_DacOutput_eY) || (eDacOutput == BVDC_DacOutput_ePr) || (eDacOutput == BVDC_DacOutput_ePb) ||          \
	   (eDacOutput == BVDC_DacOutput_eRed) || (eDacOutput == BVDC_DacOutput_eGreen) || (eDacOutput == BVDC_DacOutput_eBlue) ||   \
	   (eDacOutput == BVDC_DacOutput_eGreen_NoSync))) ? true :                                                                   \
	    (((eDisplayOutput == BVDC_DisplayOutput_eComposite) && (eDacOutput == BVDC_DacOutput_eComposite)) ? true :               \
	     ((eDisplayOutput == BVDC_DisplayOutput_eSVideo) &&                                                                      \
		  ((eDacOutput == BVDC_DacOutput_eSVideo_Luma) || (eDacOutput == BVDC_DacOutput_eSVideo_Chroma)) ? true : false)))

#define BVDC_P_DISP_INVALID_VF_CH (-1)
#define BVDC_P_DISP_GET_VF_CH_FROM_DAC(eDacOutput) \
	(((eDacOutput == BVDC_DacOutput_eComposite) || (eDacOutput == BVDC_DacOutput_eSVideo_Luma) || (eDacOutput == BVDC_DacOutput_eGreen) ||  \
	  (eDacOutput == BVDC_DacOutput_eGreen_NoSync) || (eDacOutput == BVDC_DacOutput_eY)) ? 0 :                                              \
	  (((eDacOutput == BVDC_DacOutput_eSVideo_Chroma) || (eDacOutput == BVDC_DacOutput_eRed) || (eDacOutput == BVDC_DacOutput_ePr)) ? 1 :   \
	    ((eDacOutput == BVDC_DacOutput_eBlue) || (eDacOutput == BVDC_DacOutput_ePb)) ? 2 : BVDC_P_DISP_INVALID_VF_CH))

/* BAVC_FrameRateCode is full rate (non 1/1001 drop).  */
#define BVDC_P_IS_FULL_FRAMRATE(eFrameRate)    \
		((BAVC_FrameRateCode_e24 == eFrameRate) || \
		 (BAVC_FrameRateCode_e25 == eFrameRate) || \
		 (BAVC_FrameRateCode_e30 == eFrameRate) || \
		 (BAVC_FrameRateCode_e50 == eFrameRate) || \
		 (BAVC_FrameRateCode_e60 == eFrameRate))

/* Work around a hardware bug in a specific WSE core.
 * This value should be identical to BVBI_P_WSE_VER3 in
 * the other porting interface module. */
#if (BCHP_CHIP == 7325) && (BCHP_VER == BCHP_VER_B0)
	#define BVDC_P_WSE_VER3 1
#endif
#if (BCHP_CHIP == 7335)  || (BCHP_CHIP == 7336)
	#define BVDC_P_WSE_VER3 1
#endif
#if (BCHP_CHIP == 7601)
	#define BVDC_P_WSE_VER3 1
#endif

/* Number of channels */
#define BVDC_P_VEC_CH_NUM       3


/****************************************************************
 *  Macros
 ****************************************************************/
#define BVDC_P_TIMER_FREQ                                    (27000000) /* 27 MHz */

/* one vsync in usecs */
#define BVDC_P_USEC_ONE_VSYNC_INTERVAL(vrate)    ( \
	(1000000 * BFMT_FREQ_FACTOR / (vrate)))

/* alignment threshold in usecs */
#define BVDC_P_USEC_ALIGNMENT_THRESHOLD     (200)

/* one vsync in 27 MHz clock cycless */
#define BVDC_P_DISPLAY_ONE_VSYNC_INTERVAL(vrate)    ( \
	(BVDC_P_TIMER_FREQ / (vrate)) * BFMT_FREQ_FACTOR )
#define BVDC_P_DISPLAY_ALIGNMENT_THRESHOLD    ( \
	BVDC_P_TIMER_FREQ / 1000000 * BVDC_P_USEC_ALIGNMENT_THRESHOLD) /* 200 usecs */
#define BVDC_P_DISPLAY_ALIGNMENT_OFFSET    (0*BVDC_P_DISPLAY_ALIGNMENT_THRESHOLD)

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
	uint32_t                    bRgb              : 1; /* RGB on/off */
} BVDC_P_MacrovisionCtrlBits;

typedef union
{
	struct
	{
		uint32_t                    bWidthTrim        : 1; /* 704-sample vs. 720-sample */
		uint32_t                    bCscAdjust        : 1; /* Colorspace change by user / src */
		uint32_t                    bTiming           : 1; /* New output timing format */
		uint32_t                    bRateChange       : 1; /* New rate change 59.94Hz vs 60.00Hz */
		uint32_t                    bAlignChange      : 1; /* new alignment settings */
		uint32_t                    bDacSetting       : 1; /* new dac settings */
		uint32_t                    bCcbAdjust        : 1;
		uint32_t                    bVfFilter         : 1; /* user VF filters */
		uint32_t                    bOutputMute       : 1; /* output Mute */
		uint32_t                    bAspRatio         : 1; /* aspect ratio might changed */
	} stBits;

	uint32_t aulInts[BVDC_P_DIRTY_INT_ARRAY_SIZE];
} BVDC_P_Display_DirtyBits;

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

/* Digital trigger */
#define BVDC_P_DISPLAY_USED_DIGTRIG(eMasterTg)    \
		((eMasterTg == BVDC_DisplayTg_eStg0)   || \
		 (eMasterTg == BVDC_DisplayTg_eStg1)   || \
		 (eMasterTg == BVDC_DisplayTg_eDviDtg) || \
		 (eMasterTg == BVDC_DisplayTg_e656Dtg))

/* STG trigger transcoding path*/
#define BVDC_P_DISPLAY_USED_STG(eMasterTg)    \
		((eMasterTg == BVDC_DisplayTg_eStg0)   || \
		 (eMasterTg == BVDC_DisplayTg_eStg1))
#if BVDC_P_SUPPORT_DVPO
typedef struct BVDC_P_RateInfo
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
	uint32_t                    ulP2; /* pll feedback pre-divider */
	const char                 *pchRate;
} BVDC_P_RateInfo;

#elif BVDC_P_SUPPORT_DVI_65NM
typedef struct BVDC_P_RateInfo
{
	/* Use for searching a matching one! */
	uint32_t                    ulPixelClkRate;
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
	const char                 *pchRate;
} BVDC_P_RateInfo;
#else
typedef struct BVDC_P_RateInfo
{
	/* Use for searching a matching one! */
	uint32_t                    ulPixelClkRate;
	uint32_t                    ulMDiv;
	uint32_t                    ulPDiv;
	uint32_t                    ulNDiv;
	uint32_t                    ulRDiv;
	uint32_t                    ulSampleInc;
	uint32_t                    ulNumerator;
	uint32_t                    ulDenominator;
	uint32_t                    ulOpOffset;
	const char                 *pchRate;
} BVDC_P_RateInfo;
#endif

typedef struct
{
	BVDC_P_CmpColorSpace        eCmpColorSpace;
	BVDC_P_Output               eOutputColorSpace;
	BVDC_P_Output               eCoOutputColorSpace;      /* Component-only color space */
	BVDC_DacOutput              aDacOutput[BVDC_P_MAX_DACS];
	BAVC_MatrixCoefficients     eHdmiOutput;
	BVDC_MacrovisionType        eMacrovisionType;
	uint32_t                    ulDCSKeyLow;
	uint32_t                    ulDCSKeyHigh;
	BVDC_RfmOutput              eRfmOutput;
	const BFMT_VideoInfo       *pFmtInfo;
	uint32_t                    ulCheckSum;
	uint32_t                    ulVertFreq;

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
	BAVC_HDMI_PixelRepetition   eHdmiPixelRepetition; /* 2x or 4x */

	uint32_t                    uiNumDacsOn;        /* #Dacs enabled for this display */
	uint32_t                    ulRfmConst;         /* constant value to RF port */

	bool                        abEnableDac[BVDC_P_MAX_DACS];
	bool                        bSyncOnly;          /* hdmi output sync only no data */
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

	/* MV N0 control bits */
	BVDC_P_MacrovisionCtrlBits  stN0Bits;

	/* 704 vs. 720 samples */
	bool                        bWidthTrimmed;

	/* Force vec to run at 59.94Hz, 29.97Hz, etc. */
	bool                        bForceFrameDrop;

	/* Color Correction parameters */
	bool                        bCCEnable;
	bool                        bUserCCTable;
	uint32_t                    ulGammaTableId;
	uint32_t                    ulColorTempId;
	uint32_t                    aulUserCCTable[BVDC_P_CCB_TABLE_SIZE];

	/* PR28836: DVO H/V/De sync polarity. */
	BVDC_Display_DvoSettings    stDvoCfg;

	/* Display alignment settings */
	BVDC_Display_Handle              hTargetDisplay;
	BVDC_Display_AlignmentSettings  stAlignCfg;

	/* color adjustment attributes */
	int32_t                     lDvoAttenuationR;
	int32_t                     lDvoAttenuationG;
	int32_t                     lDvoAttenuationB;
	int32_t                     lDvoOffsetR;
	int32_t                     lDvoOffsetG;
	int32_t                     lDvoOffsetB;

	bool                        abOutputMute[BVDC_DisplayOutput_e656 + 1];

	bool                        bMultiRateAllow;

	BVDC_3dSourceBufferSelect   e3dSrcBufSel;

	/* Rate manager attributes */
	const uint32_t             *pulAnalogRateTable;
	BAVC_VdcDisplay_Info        aAnalogRateInfo;
	const BVDC_P_RateInfo      *pHdmiRateInfo;

	/* display aspect ratio */
	BFMT_AspectRatio            eAspectRatio;
	uint32_t                    uiSampleAspectRatioX;
	uint16_t                    uiSampleAspectRatioY;
	BVDC_P_ClipRect             stAspRatRectClip;

	bool                        bBypassVideoProcess;
	/* user VF filters */
	bool                        abUserVfFilterCo[BVDC_P_VEC_CH_NUM];
	bool                        abUserVfFilterCvbs[BVDC_P_VEC_CH_NUM];
	uint32_t                    aaulUserVfFilterCo[BVDC_P_VEC_CH_NUM][BVDC_P_CHROMA_TABLE_SIZE];
	uint32_t                    aaulUserVfFilterCvbs[BVDC_P_VEC_CH_NUM][BVDC_P_CHROMA_TABLE_SIZE];

	/* dirty bits */
	BVDC_P_Display_DirtyBits    stDirty;
} BVDC_P_DisplayInfo;

typedef struct BVDC_P_DisplayContext
{
	BDBG_OBJECT(BVDC_DSP)

	BVDC_P_DisplayInfo          stNewInfo;           /* new(to-be apply) display info */
	BVDC_P_DisplayInfo          stCurInfo;           /* current(hw-applied) display info */

	BVDC_DisplayId              eId;                 /* might be different from cmp id */
	BVDC_P_State                eState;              /* Context state. */
	BVDC_Handle                 hVdc;                /* From which main VDC handle */
	BVDC_P_ItState              eItState;            /* Current Vec state */
	BVDC_Compositor_Handle      hCompositor;         /* Conntected to compositor */
	bool                        bUserAppliedChanges; /* New changes are ready */
	uint32_t                    ulRdcVarAddr;        /* Temp RDC var addr use for format change. */
	uint32_t                    ulDisplayCnt;        /* count number of displays */

	/* DVO, and Main CSC */
	BVDC_P_DisplayCscMatrix     stDvoCscMatrix;

	/* VF filters */
	const uint32_t             *apVfFilterCvbs[BVDC_P_VEC_CH_NUM];
	const uint32_t             *apVfFilterCo[BVDC_P_VEC_CH_NUM];

	/* Display path */
	BVDC_P_VecPath              eVecPath;
	BAVC_VbiPath                eVbiPath;
	BRDC_Trigger                eTopTrigger;
	BRDC_Trigger                eBotTrigger;
	BVDC_DisplayTg              eMasterTg;

	/* into the other vec with similar cores */
	int32_t                     lItOffset;
	int32_t                     lVfOffset;
	int32_t                     lSmOffset;
	int32_t                     lCscOffset;
	int32_t                     lSrcOffset;
	int32_t                     lRmOffset;

	/* Event to nofify that changes has been applied to hardware. */
	BKNI_EventHandle            hAppliedDoneEvent;
	bool                        bSetEventPending;
	bool                        bRateManagerUpdatedPending;
	bool                        bRateManagerUpdated;

	/* vec phase adjustment */
	bool                        bVecPhaseInAdjust;

	/* MPAA decimation supported interface port mask */
	uint32_t                    aulMpaaDeciIfPortMask[BVDC_MpaaDeciIf_eUnused];

	/* used for delaying dvi input for a number of vsyncs as part of Bx software
	 * dvi reset workaround */
	uint32_t                    ulDviElapsedVsyncs;

	/* secret scratch register that holds the VBI encoder control setting */
	uint32_t                    ulVbiEncOffset;
	uint32_t                    ulScratchVbiEncControl;
	uint32_t                    ulPrevVbiEncControlValue;

	/* Same, but for ancillary 656 path */
	uint32_t                    ulVbiEncOffset_656;
	uint32_t                    ulScratchVbiEncControl_656;
	uint32_t                    ulPrevVbiEncControlValue_656;

	/* MV type change flag */
	bool                        bMvChange;
	BVDC_MacrovisionType        ePrevMvType;

	/* DCS state change indicator */
	/* 0 for no change */
	/* 1 for minor change (just enable/disable some VEC IT microcontrollers) */
	/* 2 for major change (new VEC microcode and register settings */
	int                         iDcsChange;
	/* Needed to maintain the above */
	uint32_t                    ulCurShadowDCSKeyLow;
	uint32_t                    ulNewShadowDCSKeyLow;
	/* TODO: I am about 90% convinced that I can collapse the above two
	 * variables into one. Need to examine program logic carefully first.
	 */

	/* Is this a bypass display? which means no VEC analog output. */
	bool                        bIsBypass;

	/* Internal VDC or App handed down. */
	BVDC_Heap_Handle            hHeap;

	/* 240p Mode */
	bool                        b240pMode;

	/* Game mode tracking window */
	BVDC_Window_Handle          hWinGameMode;
	const uint32_t                     *pRmTable;  /* VEC RM */
	const BVDC_P_RateInfo      *pDvoRmInfo;/* DVO RM */
	bool                                    bRmAdjusted;

	/* Mud */
	BVDC_P_Mud_Handle           hMud;

	/* alignment Timestamps */
	BTMR_TimerHandle            hTimer;
	BTMR_TimerRegisters         stTimerReg;
	uint32_t                    ulScratchTsAddr;
	uint32_t                    ulCurrentTs;
	BAVC_Polarity               eNextPolarity;
	uint32_t                    ulAlignSlaves;
	bool                        bAlignAdjusting;
	uint32_t                    ulTsSampleCount;
	uint32_t                    ulTsSamplePeriod;

	/* Dither */
	BVDC_P_DitherSetting        stCscDither;
	BVDC_P_DitherSetting        stDviDither;
	BVDC_P_DitherSetting        st656Dither;

	BVDC_Display_CallbackSettings   stCallbackSettings;
	BVDC_Display_CallbackData       stCallbackData;

	/* Count down for kick start (in vsync unit) */
	uint32_t                    ulKickStartDelay;

	/* display pixel aspect ratio */
	uintAR_t                    ulPxlAspRatio;     /* PxlAspR_int.PxlAspR_frac */

	/* Specific flavor of 480P output */
	bool bArib480p;

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

uint32_t BVDC_P_GetPosSyncValue
	( BVDC_P_DisplayContext           *pDisplay,
	  uint32_t                       **ppulRul,
	  BVDC_P_VecPath                   eVecPath );
uint32_t BVDC_P_GetCoPosSyncValue
	( BVDC_P_DisplayContext           *pDisplay,
	  uint32_t                       **ppulRul,
	  BVDC_P_VecPath                   eVecPath );
#if DCS_SUPPORT
uint32_t BVDC_P_GetPosSyncValueDcs
(
	BFMT_VideoFmt eVideoFmt,
	BVDC_P_Output eCoOutputColorSpace,
	uint32_t ulDCSKeyLow
);
uint32_t BVDC_P_GetCoPosSyncValueDcs
(
	BFMT_VideoFmt eVideoFmt,
	BVDC_P_Output eCoOutputColorSpace,
	uint32_t ulDCSKeyLow
);
#endif

BERR_Code BVDC_P_ChangeMvType
	( BVDC_P_DisplayContext           *pDisplay,
	  uint32_t                       **ppulRul );

typedef enum {
	BVDC_P_PROT_Alt_None = 0,
	BVDC_P_PROT_ALT_MV,
	BVDC_P_PROT_ALT_DCS
} BVDC_P_Prot_Alt;
BVDC_P_Prot_Alt BVDC_P_Get_Prot_Alt (void);

uint32_t BVDC_P_Display_Modify_SYNC_TRANS_0 (
	BVDC_P_DisplayInfo               *pNewInfo,
	uint32_t                         ulVfSTRegVal);

uint32_t BVDC_P_GetNegSyncValue
	( BVDC_P_DisplayInfo              *pDispInfo,
	  BVDC_P_Output                    eOutputColorSpace );

uint32_t BVDC_P_GetFmtAdderValue
	( BVDC_P_DisplayInfo              *pDispInfo );

BERR_Code BVDC_P_ValidateMacrovision
	( BVDC_P_DisplayContext           *pDisplay );

void BVDC_P_Vec_Build_SyncPCL_isr
	( BVDC_P_DisplayContext           *pDisplay,
	  BVDC_P_ListInfo                 *pList );

void BVDC_P_Vec_Update_OutMuxes_isr
	( BVDC_P_Context                  *pVdc );

void BVDC_P_Display_EnableTriggers_isr
	( BVDC_P_DisplayContext           *pDisplay,
	  bool                             bEnable );

const BVDC_P_FormatCCBTbl* BVDC_P_Display_GetCCBTable
	( const uint32_t                   ulGammaId,
	  const uint32_t                   ulColorTempId );

void BVDC_P_Display_GetMaxTable
	( uint32_t                        *pulGammaTable,
	  uint32_t                        *pulColorTempTable );

void BVDC_P_Vec_BuildCCBRul_isr
	( BVDC_Display_Handle              hDisplay,
	  BVDC_P_ListInfo                 *pList);

void BVDC_P_ResetVec
	( BVDC_P_Context                  *pVdc );


#ifdef __cplusplus
}
#endif

#endif /* #ifndef BVDC_DISPLAY_PRIV_H__ */
/* End of file. */
