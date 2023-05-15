/***************************************************************************
 *     Copyright (c) 2003-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvdc_vdecfmt_priv.c $
 * $brcm_Revision: Hydra_Software_Devel/96 $
 * $brcm_Date: 4/19/10 10:24p $
 *
 * Module Description:
 *
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vdc/7038/bvdc_vdecfmt_priv.c $
 * 
 * Hydra_Software_Devel/96   4/19/10 10:24p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic. Move
 * BDBG_OBJECT_ID_DECLARE private header files instead of .c.
 *
 * Hydra_Software_Devel/95   4/7/10 11:36a tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic.  Rename TLA
 *
 * Hydra_Software_Devel/94   4/5/10 4:16p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic
 *
 * Hydra_Software_Devel/93   2/1/10 2:18p pntruong
 * SW3556-961: Updated mv settings from hw recommendation.  May need a
 * further fine turned from mv characterization.
 *
 * Hydra_Software_Devel/92   1/8/10 4:22p pntruong
 * SW3556-962: Add addition software (using VDEC_CD_0_SECAM_ACGC_STATUS)
 * to detect secam unlock.  Also additional fine tune on settings.
 *
 * Hydra_Software_Devel/91   12/15/09 6:29p pntruong
 * SW3556-928, SW3556-929: Fine tuned mv detections.
 *
 * Hydra_Software_Devel/90   12/8/09 3:45p pntruong
 * SW3556-916:  Additional fine tuned SECAM settings based suggestion from
 * HW.
 *
 * Hydra_Software_Devel/89   11/6/09 1:34p rpan
 * SW3556-841: Turn on MATCH_REQ_LOCK for RF_PAL and CVBS_SECAM.
 *
 * Hydra_Software_Devel/88   11/3/09 7:23p pntruong
 * SW3556-841: Improved noisy signal handling for secam.  Use pal's noisy
 * setting until further tuning needed.
 *
 * Hydra_Software_Devel/87   4/21/09 7:23p pntruong
 * PR54382: Every tuner change color is different on RF.
 *
 * Hydra_Software_Devel/86   3/10/09 11:05a pntruong
 * PR52653: Merged from pr branch.
 *
 * Hydra_Software_Devel/PR52653/2   3/10/09 7:13p xkliu
 * PR52653:[customer] Color error and shaking was happen when channel
 * changing in the analog RF.
 * 1. fix a checkin mistake from PR52730
 * 2. only apply FREQ_AVG=enable into RF, exclude cvbs
 *
 * Hydra_Software_Devel/85   3/4/09 7:22p pntruong
 * PR52730 : add fmt table for RF PAL. Change
 * VDEC_FE_0_RAW_HSYNC_CONTROL.INT_AREA from 1 to 0; change
 * VDEC_FE_0_SYNC_DETECT.SIGNAL_TO_MONITOR from AUTO to FILTA
 *
 * Hydra_Software_Devel/PR52730/1   3/4/09 10:24a chengli
 * PR52730 : add fmt table for RF PAL. Change
 * VDEC_FE_0_RAW_HSYNC_CONTROL.INT_AREA from 1 to 0; change
 * VDEC_FE_0_SYNC_DETECT.SIGNAL_TO_MONITOR from AUTO to FILTA
 *
 * Hydra_Software_Devel/84   3/2/09 4:32p pntruong
 * PR52653: Color error and shaking was happen when channel changing in
 * the analog RF.
 *
 * Hydra_Software_Devel/83   2/23/09 11:14p pntruong
 * PR52421:[customer] A Specific RF NTSC Video moved horizontally at the
 * moment. INT_AREA =1->0.
 *
 * Hydra_Software_Devel/PR52421/1   2/24/09 2:42p xkliu
 * PR52421:[customer] A Specific RF NTSC Video moved horizontally at the
 * moment. INT_AREA =1->0.
 *
 * Hydra_Software_Devel/82   2/17/09 11:28p pntruong
 * PR52201:[customer] In the bottom side, Text box was shaken. It's a
 * field analog stream problem.
 *
 * Hydra_Software_Devel/PR52201/1   2/18/09 11:19a xkliu
 * PR52201:[customer] In the bottom side, Text box was shaken. It's a
 * field analog stream problem.
 *
 * Hydra_Software_Devel/81   1/16/09 3:54p pntruong
 * PR50097: Toggle SLICE_LPF_HPLL_LOCK_CTL to improved detection of full
 * white pattern.
 *
 * Hydra_Software_Devel/80   1/13/09 9:28p pntruong
 * PR50973: Horizontal Jitter problem of Analog TV at Multiburst pattern
 * area (Color Standard signal pattern).
 *
 * Hydra_Software_Devel/79   12/1/08 4:44p pntruong
 * PR48892: Use fine tuned value for bandlimit filter setting.
 * PR49257: PR49304: 1) component input 1080p/23.97 and 1080p/24 locked
 * slower than other 1080p formats. 2) It takes long time before HPLL get
 * locked for Component inuput, 1080p@60Hz, full white pattern.
 * PR49192: Remove the workaround for PR40810.
 * PR49561: [VDEC]: Vertical black bar on the white noise channel
 * PR48639: [customer]1line on/off pattern is bluring on component 1080p
 * signals.  Change the bandlimit filter for 1080P component.
 *
 * Hydra_Software_Devel/PR48639/1   12/1/08 3:38p mlei
 * PR49304: It takes long time before HPLL get locked for Component
 * inuput, 1080p@60Hz, full white pattern.
 *
 * Hydra_Software_Devel/78   11/5/08 6:24p pntruong
 * PR48678: [VDEC] Component 480i HPLL lock unstable sometimes.
 *
 * Hydra_Software_Devel/PR47952/2   11/5/08 5:58p mlei
 * PR48678: [VDEC] Component 480i HPLL lock unstable sometimes
 *
 * Hydra_Software_Devel/77   11/4/08 11:58a pntruong
 * PR48576 : There is a merge error due to PR46796 in table
 * s_aVdecFmtTbl_YPrPb_NTSC.
 *
 * Hydra_Software_Devel/PR48576/1   10/31/08 5:29p chengli
 * PR48576 : There is a merge error due to PR46796 in table
 * s_aVdecFmtTbl_YPrPb_NTSC
 *
 * Hydra_Software_Devel/76   10/23/08 8:54a pntruong
 * PR48162, PR48266: Restored the setting for 1080p AGC_PATH_SEP settings.
 *
 * Hydra_Software_Devel/75   10/22/08 8:37p pntruong
 * PR48192: Enable full sample rate and enable 256 phase.  216M sample
 * rate in FE
 * PR45566: fix hang up issue in PAL, PAL-Nc Legacy Save mode.
 * PR48196: B0: Fine tuning Y/C delay in 3549B0.
 * PR43517: Fine tune secam auto detection on 3549B0.
 *
 * Hydra_Software_Devel/PR47952/1   10/22/08 12:32p mlei
 * PR48194: VDEC: SVideo Chroma Burst Window
 *
 * Hydra_Software_Devel/74   10/21/08 8:18p pntruong
 * PR48162: VDEC: FE AGC overflow when tip is at negative maximum.
 *
 * Hydra_Software_Devel/73   9/22/08 3:59p pntruong
 * PR46796: Fine tuning HPLL setting for CVBS, Svideo and analog RF.
 *
 * Hydra_Software_Devel/72   9/17/08 10:20a pntruong
 * PR47013: Initial b0 build.
 *
 * Hydra_Software_Devel/71   7/23/08 4:57p pntruong
 * PR44484, PR44483: Fine tune settings for component 1080p locking.
 *
 * Hydra_Software_Devel/70   7/14/08 6:44p pntruong
 * PR37240: VDEC: Need to match Filt A / Filt B group delays (including
 * fractional).
 *
 * Hydra_Software_Devel/69   7/14/08 5:44p pntruong
 * PR44802: Optimize SECAM settings for VCR
 *
 * Hydra_Software_Devel/68   7/10/08 11:10a pntruong
 * PR44039: Correct line offset according to spec.
 *
 * Hydra_Software_Devel/67   7/9/08 11:05a pntruong
 * PR44237, PR44389: 1) set MASK_EQ_SYNC to 1 to avoid VCR false detection
 * issue. 2) disable CHROMA_ALIASING_3D to keep consistent with 3563.
 *
 * Hydra_Software_Devel/66   7/8/08 3:34p pntruong
 * PR44484, PR44483: Fine tune settings for component 720p locking.
 *
 * Hydra_Software_Devel/65   6/22/08 10:45p pntruong
 * PR43562:  [VDEC]1080p setting can locked 480i input in S/W.
 *
 * Hydra_Software_Devel/64   6/19/08 3:29p pntruong
 * PR43811: Fine tuned RF analog video quality.
 *
 * Hydra_Software_Devel/63   6/12/08 10:14p pntruong
 * PR43458: Fine tune to fix rainbow issues.
 *
 * Hydra_Software_Devel/62   6/9/08 4:18p pntruong
 * PR34806, PR40963: Fine tuned secam and 3dcomb settings.
 *
 * Hydra_Software_Devel/61   6/7/08 4:49p pntruong
 * PR41315: Additional secam tunings.
 *
 * Hydra_Software_Devel/60   6/6/08 7:29p pntruong
 * PR40963: Intial 3DComb bringup.
 *
 * Hydra_Software_Devel/59   6/5/08 8:43p pntruong
 * PR41315: Initial secam bringup.
 *
 * Hydra_Software_Devel/58   6/5/08 3:07p pntruong
 * PR34711: Initial 3548 vdec bringup and added more default for new bits.
 *
 * Hydra_Software_Devel/57   5/29/08 5:02p pntruong
 * PR42033: PAL video quality is very bad in weak signal.
 *
 * Hydra_Software_Devel/56   5/12/08 8:57p darnstein
 * PR42416: Add a missing register setting for SECAM.
 *
 * Hydra_Software_Devel/55   5/12/08 8:37p pntruong
 * PR42416: Isolated for 625 lines formats, and also include vbi start.
 *
 * Hydra_Software_Devel/54   5/12/08 2:56p darnstein
 * PR42416: Implement Grossman's work-around for the conflict between VDEC
 * frame comb filter and teletext decoder (TTD).
 *
 * Hydra_Software_Devel/53   4/23/08 4:04p pntruong
 * PR40213, PR41315, PR41529: Added initial support SECAM and pc auto
 * raster.
 *
 * Hydra_Software_Devel/52   4/21/08 11:15p pntruong
 * PR34711: Prepared to add new features into vdec.  Removed vdec1
 * support.
 *
 * Hydra_Software_Devel/51   3/31/08 9:10a pntruong
 * PR38874: Apply the DISABLE_SLICE_TILT=1 workaround for all.
 *
 * Hydra_Software_Devel/50   3/11/08 10:46p pntruong
 * PR40280: Added support for vdec PAL-60.
 *
 * Hydra_Software_Devel/49   3/5/08 4:50p pntruong
 * PR38872: Updated the CONSISTEN_ERROR_TH=16 for svideo per system engr
 * request.
 *
 * Hydra_Software_Devel/48   2/5/08 1:38p pntruong
 * PR38872: Need to include noisy case as well.
 *
 * Hydra_Software_Devel/47   2/5/08 1:21p pntruong
 * PR38872: Sync is bent at top of the CVBS signal.(special device).
 *
 * Hydra_Software_Devel/46   1/30/08 10:05a pntruong
 * PR38874: More fine tune rf settings from field test.
 * DISABLE_SLICE_TILT=1 (disable compensation logic).
 *
 * Hydra_Software_Devel/pr38874/1   1/30/08 6:12p xkliu
 * PR38874: NTSC Video vertical shaking on specific RF channel.
 * DISABLE_SLICE_TILT=1.
 *
 * Hydra_Software_Devel/45   12/12/07 9:32a pntruong
 * PR38038: There is video jitter in noisy analog RF signals without color
 * burst.
 *
 * Hydra_Software_Devel/44   12/6/07 11:27p pntruong
 * PR36462: PR36785: Apply new setting for DCR and SLICE_MACRO_VISION
 * PR36462:Video color of Pal-M VCR input is greenish
 *
 * Hydra_Software_Devel/43   11/29/07 1:49p pntruong
 * PR36216: [PC]:Support component input formats with PC input.  Tighten
 * const.
 *
 * Hydra_Software_Devel/42   11/5/07 8:00p pntruong
 * PR36462:Video color of Pal-M VCR input is greenish.
 *
 * Hydra_Software_Devel/41   10/15/07 3:17p pntruong
 * PR35137: Add NTSC443 support for cvbs/Svideo.
 *
 * Hydra_Software_Devel/40   10/15/07 10:11a pntruong
 * PR34465: Need different setting in VDEC for auto scan mode to improve
 * the HPLL locking.
 *
 * Hydra_Software_Devel/PR34465/1   10/11/07 10:07p dyzhang
 * PR34465: Need different setting in VDEC for auto scan mode to improve
 * the HPLL locking.
 *
 * Hydra_Software_Devel/39   9/4/07 1:52p pntruong
 * PR34428, PR30691, PR31623, PR33981, PR33545, PR32402: Merged fixes from
 * field tests with customers.
 *
 * Hydra_Software_Devel/PR34428/4   9/4/07 6:50p mlei
 * PR31623: [VDEC] auto detection of PAL-M/PAL/PAL-Nc/NTSC for VCR input
 *
 * Hydra_Software_Devel/PR34428/3   9/3/07 7:24p mlei
 * PR33545:[VDEC] Optimize HPLL programming to improve noise handling.
 *
 * Hydra_Software_Devel/PR34428/2   9/3/07 4:44p mlei
 * PR30691:[VDEC]some VCR tape(landscaping shows) can not detect some VCR
 * headswitch
 *
 * Hydra_Software_Devel/PR34428/1   9/3/07 4:15p mlei
 * PR34428:Auto format detection for RF input
 *
 * Hydra_Software_Devel/38   8/24/07 8:22p pntruong
 * PR32896: More adjustment to copy protection settings.
 *
 * Hydra_Software_Devel/37   8/16/07 6:17p tdo
 * PR32896, PR34007: Add 50Hz support and progressive MV detection
 *
 * Hydra_Software_Devel/36   8/14/07 9:45a pntruong
 * PR33545: Fixed bending issue, and hshift.
 *
 * Hydra_Software_Devel/35   8/2/07 5:28p tdo
 * PR32896:  [VDEC] Need Macrovision signal detection software
 *
 * Hydra_Software_Devel/34   8/1/07 1:21p pntruong
 * PR33011, PR33011, PR31623: [VDEC] auto detection of PAL-M/PAL/PAL-
 * Nc/NTSC for VCR input.
 *
 * Hydra_Software_Devel/33   7/26/07 6:48p pntruong
 * PR33545: [VDEC] Optimize HPLL programming to improve noise handling for
 * cvbs.
 *
 * Hydra_Software_Devel/32   7/9/07 3:49p pntruong
 * PR32775: Hstart user setting is overwritten in VDEC.
 *
 * Hydra_Software_Devel/31   7/2/07 11:43a pntruong
 * PR32673: [VDEC] Add NTSC_J support.
 *
 * Hydra_Software_Devel/30   6/19/07 2:41p pntruong
 * PR32260: PR32260: 1: delay 80 Vsyncs to detect VCR; 2: for PAL format,
 * once  HPLL/ADC/Format status change, clear subcarrier .
 *
 * Hydra_Software_Devel/29   5/25/07 5:57p pntruong
 * PR30691: [VDEC]some VCR tape(landscaping shows) can not detect some VCR
 * headswitch.
 *
 * Hydra_Software_Devel/28   5/24/07 4:21p pntruong
 * PR31575: H-jitter when Play VCR.
 *
 * Hydra_Software_Devel/27   5/16/07 11:16a pntruong
 * PR30361: Provide API to control DC restore.
 *
 * Hydra_Software_Devel/26   5/10/07 8:37p pntruong
 * PR30643, PR30642: [VDEC]component 1080p@30Hz, Video shift to right side
 * a little.  [VDEC]component 1080i, video flash on red raster pattern.
 *
 * Hydra_Software_Devel/25   5/4/07 3:50p pntruong
 * PR30373, PR29999: Improve noise and vcr input handling.
 *
 * Hydra_Software_Devel/PR29983/3   5/4/07 12:10p mlei
 * PR30373:change some registers to fix vcr false detection for PAL and
 * PAL-Nc
 *
 * Hydra_Software_Devel/24   5/3/07 7:38p pntruong
 * PR29983: Improved noise signal handling.
 *
 * Hydra_Software_Devel/PR29983/2   5/3/07 4:41p dyzhang
 * PR29983: video shake horizontally in noise condition
 *
 * Hydra_Software_Devel/PR29983/1   5/3/07 3:59p dyzhang
 * PR29983: video shake horizontally in noise condition
 *
 * Hydra_Software_Devel/23   5/1/07 9:49p tdo
 * PR29659: C0: Support 1080p @ 60 through component input
 *
 * Hydra_Software_Devel/22   4/24/07 7:58p pntruong
 * PR29669: C0: VDEC: Vsync lock status doesn't work for 720p with raw
 * vsync mode.
 *
 * Hydra_Software_Devel/21   4/24/07 5:50p pntruong
 * PR29663: C0: VDEC FE: Potential error for DCR blank level estimation
 * average region.
 *
 * Hydra_Software_Devel/20   4/11/07 7:39p pntruong
 * PR29483: PC input. Add two supported formats:720x400@70Hz and
 * 640x350@70Hz.  Also not include the dummy adc/edsafe regs in rul.
 *
 * Hydra_Software_Devel/19   4/10/07 7:36p tdo
 * PR26033: Add FE table for PAL_M
 *
 * Hydra_Software_Devel/18   4/4/07 6:40p tdo
 * PR29208: Add support for component 1080p@23/24//29/30Hz input
 *
 * Hydra_Software_Devel/17   4/3/07 6:24p pntruong
 * PR28773: Use RDC read for VDEC_FE_0_FE_FIELD_SIZE to get consistence
 * values.  Use median averaging to remove toggling.  Update statics
 * settings to better match 3560bx.
 *
 * Hydra_Software_Devel/16   3/23/07 6:46p pntruong
 * PR28773: Optimize settings for ShibaSoku FH testing.
 *
 * Hydra_Software_Devel/15   3/21/07 5:43p pntruong
 * PR28884: Used VDEC_FE_0_DCR_CONTROL_CH0.REFERENCE_LEVEL when HPLL
 * unlocked and macrovision-free signal for vbi lines.  This is to
 * removed the black line with noise signal.
 *
 * Hydra_Software_Devel/14   3/17/07 5:25p pntruong
 * PR28773, PR23226: Restored SIGNAL_TO_MONITOR to AUTO.  Fixed VCR
 * settings to work with standard tape.  Need additional improvements for
 * bad vcr tape.
 *
 * Hydra_Software_Devel/13   3/3/07 7:45p pntruong
 * PR28359: VDEC: Video quality improvements for
 * IF/CVBS/YPrPb/480i/480p/720p/1080i.
 *
 * Hydra_Software_Devel/12   3/3/07 12:50p pntruong
 * PR23424: Correct digital gain reference settings.  Refactored code, so
 * that vdec fmt table is higher precedence.  This will ease the adding
 * of new format/input modes.
 *
 * Hydra_Software_Devel/11   3/2/07 5:40p pntruong
 * PR28359: VDEC: Video quality improvements for CVBS NTSC.
 *
 * Hydra_Software_Devel/10   2/27/07 2:03p pntruong
 * PR23419: Change AWC normal level value.
 *
 * Hydra_Software_Devel/9   2/17/07 9:37a pntruong
 * PR23482: Fine tuned framerate detection, handle case for pc input.
 *
 * Hydra_Software_Devel/8   2/16/07 6:44p pntruong
 * PR26033: Correct color for pal input.
 *
 * Hydra_Software_Devel/7   2/13/07 12:58p pntruong
 * PR26033: Initial value to lock PAL-BDGHIN.
 *
 * Hydra_Software_Devel/6   2/9/07 2:52p pntruong
 * PR27557:  Support new vnet mode for all active chipsets except for 7440
 * and 7403.
 *
 * Hydra_Software_Devel/5   1/21/07 2:31p pntruong
 * PR23225: VDEC - PC input.  Fine tune the other parameters for pc
 * formats.
 *
 * Hydra_Software_Devel/4   1/18/07 9:04p pntruong
 * PR23225: VDEC - PC input.  Added more PC input formats, and tablelized
 * pc format specifics settings.
 *
 * Hydra_Software_Devel/3   1/17/07 7:56p pntruong
 * PR23225: VDEC - PC input.  Updated some pll settings, and allow pc
 * format to use the same fe settings.
 *
 * Hydra_Software_Devel/2   1/17/07 3:45p pntruong
 * PR23225: VDEC - PC input.  Added initial PC format support for
 * 640x480p@60.
 *
 * Hydra_Software_Devel/1   1/15/07 8:37p pntruong
 * PR23222: VDEC - ATSC input support.
 *
 ***************************************************************************/
#include "bstd.h"
#include "bkni.h"
#include "bdbg.h"
#include "bvdc_priv.h"
#include "bvdc_vdec_priv.h"
#include "bvdc_displayfmt_priv.h"

#if (BVDC_P_SUPPORT_VDEC)
#include "bchp_vdec_fe_0.h"

BDBG_MODULE(BVDC_VDEC);

/* PR18220: INT_MIN is a reserved name of POSIX, so we have to undine this
 * macro first */
#ifdef INT_MIN
#undef INT_MIN
#endif

#if (BVDC_P_SUPPORT_VDEC_VER < BVDC_P_VDEC_VER_4)
#define BCHP_VDEC_FE_0_FIELD_SYNC_ID_UPDATE_WINDOW_MASK  BCHP_VDEC_FE_0_FIELD_SYNC_ID_UPDATE_WIDOW_MASK
#define BCHP_VDEC_FE_0_FIELD_SYNC_ID_UPDATE_WINDOW_SHIFT BCHP_VDEC_FE_0_FIELD_SYNC_ID_UPDATE_WIDOW_SHIFT
#endif

/***************************************************************************
 * NOTE: These are take directly from characterized BBS scripts,
 * care must be taken when modifying these values.
 *
 * Currently VDEC_x_FE is so format dependence that it's messy to wrap in
 * any kind of logic.  It's there much quicker just tablelized the FE portion
 * of VDEC.
 ***************************************************************************/
static const BVDC_P_AddrDataPair s_aVdecFmtTbl_PcIn[] =
{
	/* VDEC_FE_0_FE_REV_ID (RO) */
	{
		BCHP_VDEC_FE_0_HSTART2,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_DATA(VDEC_FE_0_HSTART2, FILTER_COEF_SEL, 0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSTART2, WINDOW_SW_SELECT,    0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSTART2, WINDOW_MODE,    0 ) |
#endif
		BCHP_FIELD_DATA(VDEC_FE_0_HSTART2, STORE, 215 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSTART2, CS,    134 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSTART2, CB,    126 )
	},

	{
		BCHP_VDEC_FE_0_NB_CONTROL,
		BCHP_FIELD_DATA(VDEC_FE_0_NB_CONTROL, NOISE_LENGTH, 6    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NB_CONTROL, NOISE_START,  1024 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NB_CONTROL, BLANK_LENGTH, 6    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NB_CONTROL, BLANK_START,  511  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NB_CONTROL, LINE,         8    )
	},

	{
		BCHP_VDEC_FE_0_AGC_REGION,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_5)
		BCHP_FIELD_ENUM(VDEC_FE_0_AGC_REGION, AGC_PATH_SEP,     SEPARATE ) |
		BCHP_FIELD_DATA(VDEC_FE_0_AGC_REGION, AGC_GAIN_LIMIT_VDEC,     0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_AGC_REGION, AGC_GAIN_LIMIT_SYNC_SEP, 0 ) |
#elif (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_DATA(VDEC_FE_0_AGC_REGION, AGC_PATH_SEP,            0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_AGC_REGION, AGC_GAIN_LIMIT_VDEC,     0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_AGC_REGION, AGC_GAIN_LIMIT_SYNC_SEP, 0 ) |
#endif
		BCHP_FIELD_DATA(VDEC_FE_0_AGC_REGION, END,   127 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_AGC_REGION, START, 6   )
	},

	{
		BCHP_VDEC_FE_0_HPLL_MISC,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_MISC, IFD_PATH_SEL,        VDEC ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_MISC, NOISE_MEASURE_SEL,   VDEC ) |
#endif
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_MISC, SLICE_LPF_HPLL_LOCK_CTL, ENABLE ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_MISC, DISABLE_SLICE_TILT,  1  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_MISC, VSYNC_OFFSET,        0  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_MISC, MATCH_FILT_TH_COEFF, 12 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_MISC, JUMP_END_LINE,       0  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_MISC, JUMP_START_LINE,     1  )
	},

	{
		BCHP_VDEC_FE_0_RAW_VSYNC_CONTROL,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_VSYNC_CONTROL, MAX_HEAD_SWITCH_HYSTERESIS_TH, 5 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_VSYNC_CONTROL, MIN_HEAD_SWITCH_HYSTERESIS_TH, 3 ) |
#endif
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_VSYNC_CONTROL, VPLL_LINE_UPDATE, 1  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_VSYNC_CONTROL, CLEAN_LINE_COUNT, 40 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_VSYNC_CONTROL, BLOCK_LINE_COUNT, 25 )
	},

	{
		BCHP_VDEC_FE_0_SYNC_DETECT,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, DCR_SEL,           VDEC      ) |
#else
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, HEAD_SWITCH_SEL,   FOUR_HEAD ) |
#endif
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, TIP_FILT_SEL,      FILTA     ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, TIP_SELECT_MID,    0         ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, SLICE_CTL_DISABLE, 0         ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, MASK_EQ_SYNC,      0         ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, SIGNAL_TO_MONITOR, FILTB     ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, SLICE_MODE,        AUTO      ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, SLICE_DELAY,       7         ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, SLICE_AVG,         AVG8      ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, P_COUNT,           0         ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, H_COUNT,           0         ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, V_COUNT,           0         )
	},

	{
		BCHP_VDEC_FE_0_SYNC_DETECT2,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, HEAD_SWITCH_HISTORY_MASK, 0 ) |
#else
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, MATCH_FILTER_TH,      0 ) |
#endif
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, MATCH_REQ_LOCK,       0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, VCR_MODE,             0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, FREQ_OFFSET,          0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, CONSISTEN_ERROR_TH,   0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, HEAD_SWITCH_ERROR_TH, 0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, MATCH_FILTER_MODE,    0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, RAW_VSYNC_MODE,       0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, HEAD_SWITCH_THRESH,   6 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, LINE_OFFSET,          1 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, SLICE_FILT,           2 )
	},

	{
		BCHP_VDEC_FE_0_SYNC_BLANK,
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_BLANK, F1_END_LINE,   6  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_BLANK, F1_START_LINE, 63 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_BLANK, F0_END_LINE,   6  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_BLANK, F0_START_LINE, 63 )
	},

	{
		BCHP_VDEC_FE_0_RAW_HSYNC_CONTROL,
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, MATCH_OVERRIDE_MIN, 0  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, SAMPLE_TYPE_CTL,    0  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, MATCH_MIN,          32 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, INT_MIN,            45 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, INT_AREA,           0  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, MATCH_DELTA_COEFF,  12 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, DELTA_COEFF,        12 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, INT_TH_COEFF,       10 )
	},

	{
		BCHP_VDEC_FE_0_HSYNC_PLL1,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL1, FRO_FREQ,     FIXED   ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL1, AVG_EN,       UPDATED ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL1, FREQ_AVG,     ENABLE  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL1, FILTA_OFFSET, 0       ) |
#endif
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL1, BIG_SMALL_MODE, AUTO ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL1, DIRECT_GAIN,    4    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL1, SMALL_TERM_CNT, 0    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL1, BIG_TERM_CNT,   1    )
	},

	{
		BCHP_VDEC_FE_0_SYNC_ACCUM_CONTROL,
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_ACCUM_CONTROL, HSYNC_CLAMP_VALUE, 63 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_ACCUM_CONTROL, VSYNC_CLAMP_VALUE, 31 )
	},

	{
		BCHP_VDEC_FE_0_HSYNC_PLL,
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL, BY_PASS_LPF,  0   ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL, ACCUM_THRESH, OFF ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL, LOCK_THRESH,  3   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL, LOCK_WINDOW,  15  )
	},

	{
		BCHP_VDEC_FE_0_HSYNC_PLL_FILTER,
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL_FILTER, NOISE_3, 48 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL_FILTER, NOISE_2, 48 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL_FILTER, NOISE_1, 48 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL_FILTER, NOISE_0, 48 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL_FILTER, UNLOCK,  48 )
	},

	{
		BCHP_VDEC_FE_0_FRO_CONTROL,
		BCHP_FIELD_DATA(VDEC_FE_0_FRO_CONTROL, AVG_ERR_FACTOR, 2               ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_FRO_CONTROL, AUTO_COEFF,     ONE_AND_QUARTER ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FRO_CONTROL, FIELD_ID_TH,    20              ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FRO_CONTROL, CUT_OFF_TH,     31              ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FRO_CONTROL, ERR_TH,         20              ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FRO_CONTROL, FRO_MODE,       0               )
	},

	{
		BCHP_VDEC_FE_0_FIELD_SYNC,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_ENUM(VDEC_FE_0_FIELD_SYNC, FID_INVERT, NO_INVERT               ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_FIELD_SYNC, PC_INTERLACED_FID,  ENABLE          ) |
#endif
		BCHP_FIELD_DATA(VDEC_FE_0_FIELD_SYNC, ID_UPDATE_WINDOW, 0                 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FIELD_SYNC, MARKING_THRESH,  0                  ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_FIELD_SYNC, MARKING_WINDOW,  PLUS_MINUS_64      ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FIELD_SYNC, FIELD_SCOPE,     2                  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FIELD_SYNC, MODE_THRESHOLD,  2                  ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_FIELD_SYNC, TOP_ID,          SYNC_ON_FIRST_HALF ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_FIELD_SYNC, TYPE,            BY_POSITION        )
	},

	{
		BCHP_VDEC_FE_0_SYNC_SLAVE_HRANGE,
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_HRANGE, SYNC_END,   50   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_HRANGE, SYNC_START, 1998 )
	},

	{
		BCHP_VDEC_FE_0_SYNC_SLAVE_LEVEL,
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_LEVEL, BLANK_LENGTH, 6    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_LEVEL, BLANK_START,  160  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_LEVEL, TIP_LENGTH,   5    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_LEVEL, TIP_START,    1990 )
	},

	{
		BCHP_VDEC_FE_0_SYNC_SLAVE_DIFF,
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_DIFF, MAX,        7  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_DIFF, LINE_RANGE, 9  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_DIFF, LINE_START, 10 )
	},

	{
		BCHP_VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN1,
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN1, FILT_SEL_NOISE_0, FILTB ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN1, FILT_SEL_NOISE_1, FILTB ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN1, NOISE_0,          63    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN1, NOISE_1,          63    )
	},

	{
		BCHP_VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN2,
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN2, FILT_SEL_NOISE_2, FILTA ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN2, FILT_SEL_NOISE_3, FILTA ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN2, NOISE_2,          63    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN2, NOISE_3,          63    )
	},

	{
		BCHP_VDEC_FE_0_BIG_SMALL_TH1,
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH1, SMALL_TO_BIG_TH_NOISE_0, 20 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH1, BIG_TO_SMALL_TH_NOISE_0, 5  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH1, SMALL_TO_BIG_TH_NOISE_1, 20 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH1, BIG_TO_SMALL_TH_NOISE_1, 5  )
	},

	{
		BCHP_VDEC_FE_0_BIG_SMALL_TH2,
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH2, SMALL_TO_BIG_TH_NOISE_2, 20 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH2, BIG_TO_SMALL_TH_NOISE_2, 5  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH2, SMALL_TO_BIG_TH_NOISE_3, 20 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH2, BIG_TO_SMALL_TH_NOISE_3, 5  )
	},

	{
		BCHP_VDEC_FE_0_HSYNC_ACC_GAIN1,
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN1, SMALL_NOISE_0, 74 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN1, BIG_NOISE_0,   21 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN1, SMALL_NOISE_1, 74 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN1, BIG_NOISE_1,   21 )
	},

	{
		BCHP_VDEC_FE_0_HSYNC_ACC_GAIN2,
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN2, SMALL_NOISE_2, 74 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN2, BIG_NOISE_2,   21 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN2, SMALL_NOISE_3, 74 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN2, BIG_NOISE_3,   21 )
	},

	{
		BCHP_VDEC_FE_0_CLEAN_VSYNC_CNT,
		BCHP_FIELD_DATA(VDEC_FE_0_CLEAN_VSYNC_CNT, NOISE_0, 36 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_CLEAN_VSYNC_CNT, NOISE_1, 36 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_CLEAN_VSYNC_CNT, NOISE_2, 36 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_CLEAN_VSYNC_CNT, NOISE_3, 36 )
	},

	{
		BCHP_VDEC_FE_0_NOISE_BIN_TH,
		BCHP_FIELD_DATA(VDEC_FE_0_NOISE_BIN_TH, HYSTERESIS, 5   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NOISE_BIN_TH, TH_2,       149 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NOISE_BIN_TH, TH_1,       75  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NOISE_BIN_TH, TH_0,       30  )
	},

	/* VDEC_FE_0_FE_STATUS0 (RO) */
	/* VDEC_FE_0_FE_STATUS (RO) */
	/* VDEC_FE_0_FE_FIELD_SIZE (RO) */
	/* VDEC_FE_0_FE_SAMPLE_COUNT (RO) */
	/* VDEC_FE_0_FE_SYNC_SLICE (RO) */
	/* VDEC_FE_0_FE_BLANK_LEVELS (RO) */
	/* VDEC_FE_0_FE_TIP_LEVELS (RO) */
	/* VDEC_FE_0_FE_AGC_LEVELS (RO) */

	/* Last Entry */
	{
		UINT32_C(-1),
		UINT32_C(-1)
	}
};

static const BVDC_P_AddrDataPair s_aVdecFmtTbl_YPrPb_1080p[] =
{
	{
		BCHP_VDEC_FE_0_HSTART2,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_DATA(VDEC_FE_0_HSTART2, FILTER_COEF_SEL, 0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSTART2, WINDOW_SW_SELECT,    0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSTART2, WINDOW_MODE,    0 ) |
#endif
		BCHP_FIELD_DATA(VDEC_FE_0_HSTART2, STORE, 215 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSTART2, CS,    134 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSTART2, CB,    126 )
	},

	{
		BCHP_VDEC_FE_0_NB_CONTROL,
		BCHP_FIELD_DATA(VDEC_FE_0_NB_CONTROL, NOISE_LENGTH, 6    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NB_CONTROL, NOISE_START,  1024 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NB_CONTROL, BLANK_LENGTH, 6    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NB_CONTROL, BLANK_START,  511  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NB_CONTROL, LINE,         8    )
	},

	{
		BCHP_VDEC_FE_0_AGC_REGION,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_5)
		BCHP_FIELD_ENUM(VDEC_FE_0_AGC_REGION, AGC_PATH_SEP,     SEPARATE ) |
		BCHP_FIELD_DATA(VDEC_FE_0_AGC_REGION, AGC_GAIN_LIMIT_VDEC,     4 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_AGC_REGION, AGC_GAIN_LIMIT_SYNC_SEP, 2 ) |
#elif (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_DATA(VDEC_FE_0_AGC_REGION, AGC_PATH_SEP,            1 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_AGC_REGION, AGC_GAIN_LIMIT_VDEC,     4 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_AGC_REGION, AGC_GAIN_LIMIT_SYNC_SEP, 2 ) |
#endif
		BCHP_FIELD_DATA(VDEC_FE_0_AGC_REGION, END,   127                 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_AGC_REGION, START, 6                   )
	},

	{
		BCHP_VDEC_FE_0_HPLL_MISC,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_MISC, IFD_PATH_SEL,        VDEC ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_MISC, NOISE_MEASURE_SEL,   VDEC ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_MISC, SLICE_LPF_HPLL_LOCK_CTL, DISABLE ) |
#else
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_MISC, SLICE_LPF_HPLL_LOCK_CTL, ENABLE ) |
#endif
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_MISC, DISABLE_SLICE_TILT,  1   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_MISC, VSYNC_OFFSET,        343 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_MISC, MATCH_FILT_TH_COEFF, 12  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_MISC, JUMP_END_LINE,       0   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_MISC, JUMP_START_LINE,     1   )
	},

	{
		BCHP_VDEC_FE_0_RAW_VSYNC_CONTROL,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_VSYNC_CONTROL, MAX_HEAD_SWITCH_HYSTERESIS_TH, 5 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_VSYNC_CONTROL, MIN_HEAD_SWITCH_HYSTERESIS_TH, 3 ) |
#endif
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_VSYNC_CONTROL, VPLL_LINE_UPDATE, 1  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_VSYNC_CONTROL, CLEAN_LINE_COUNT, 40 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_VSYNC_CONTROL, BLOCK_LINE_COUNT, 25 )
	},

	{
		BCHP_VDEC_FE_0_SYNC_DETECT,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, DCR_SEL,           VDEC      ) |
#else
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, HEAD_SWITCH_SEL,   FOUR_HEAD ) |
#endif
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, TIP_FILT_SEL,      FILTA     ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, TIP_SELECT_MID,    0         ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, SLICE_CTL_DISABLE, 0         ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, MASK_EQ_SYNC,      0         ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, SIGNAL_TO_MONITOR, FILTA     ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, SLICE_MODE,        AUTO      ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, SLICE_DELAY,       2         ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, SLICE_AVG,         AVG8      ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, P_COUNT,           12        ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, H_COUNT,           24        ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, V_COUNT,           120       )
	},

	{
		BCHP_VDEC_FE_0_SYNC_DETECT2,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, HEAD_SWITCH_HISTORY_MASK, 0 ) |
#else
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, MATCH_FILTER_TH,      0 ) |
#endif
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, MATCH_REQ_LOCK,       0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, VCR_MODE,             0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, FREQ_OFFSET,          0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, CONSISTEN_ERROR_TH,   0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, HEAD_SWITCH_ERROR_TH, 0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, MATCH_FILTER_MODE,    0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, RAW_VSYNC_MODE,       0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, HEAD_SWITCH_THRESH,   6 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, LINE_OFFSET,          1 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, SLICE_FILT,           2 )
	},

	{
		BCHP_VDEC_FE_0_SYNC_BLANK,
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_BLANK, F1_END_LINE,   6  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_BLANK, F1_START_LINE, 63 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_BLANK, F0_END_LINE,   6  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_BLANK, F0_START_LINE, 63 )
	},

	{
		BCHP_VDEC_FE_0_RAW_HSYNC_CONTROL,
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, MATCH_OVERRIDE_MIN, 0  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, SAMPLE_TYPE_CTL,    0  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, MATCH_MIN,          32 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, INT_MIN,            45 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, INT_AREA,           0  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, MATCH_DELTA_COEFF,  12 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, DELTA_COEFF,        12 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, INT_TH_COEFF,       10 )
	},

	{
		BCHP_VDEC_FE_0_HSYNC_PLL1,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL1, FRO_FREQ,     FIXED   ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL1, AVG_EN,       UPDATED ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL1, FREQ_AVG,     ENABLE  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL1, FILTA_OFFSET, 0       ) |
#endif
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL1, BIG_SMALL_MODE, AUTO ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL1, DIRECT_GAIN,    4    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL1, SMALL_TERM_CNT, 0    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL1, BIG_TERM_CNT,   1    )
	},

	{
		BCHP_VDEC_FE_0_SYNC_ACCUM_CONTROL,
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_ACCUM_CONTROL, HSYNC_CLAMP_VALUE, 63 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_ACCUM_CONTROL, VSYNC_CLAMP_VALUE, 31 )
	},

	{
		BCHP_VDEC_FE_0_HSYNC_PLL,
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL, BY_PASS_LPF,  0   ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL, ACCUM_THRESH, OFF ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL, LOCK_THRESH,  3   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL, LOCK_WINDOW,  15  )
	},

	{
		BCHP_VDEC_FE_0_HSYNC_PLL_FILTER,
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL_FILTER, NOISE_3, 48 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL_FILTER, NOISE_2, 48 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL_FILTER, NOISE_1, 48 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL_FILTER, NOISE_0, 48 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL_FILTER, UNLOCK,  48 )
	},

	{
		BCHP_VDEC_FE_0_FRO_CONTROL,
		BCHP_FIELD_DATA(VDEC_FE_0_FRO_CONTROL, AVG_ERR_FACTOR, 2               ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_FRO_CONTROL, AUTO_COEFF,     ONE_AND_QUARTER ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FRO_CONTROL, FIELD_ID_TH,    20              ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FRO_CONTROL, CUT_OFF_TH,     31              ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FRO_CONTROL, ERR_TH,         20              ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FRO_CONTROL, FRO_MODE,       0               )
	},

	{
		BCHP_VDEC_FE_0_FIELD_SYNC,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_ENUM(VDEC_FE_0_FIELD_SYNC, FID_INVERT, NO_INVERT               ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_FIELD_SYNC, PC_INTERLACED_FID,  ENABLE          ) |
#endif
		BCHP_FIELD_DATA(VDEC_FE_0_FIELD_SYNC, ID_UPDATE_WINDOW, 0                  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FIELD_SYNC, MARKING_THRESH,  0                   ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_FIELD_SYNC, MARKING_WINDOW,  PLUS_MINUS_64       ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FIELD_SYNC, FIELD_SCOPE,     3                   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FIELD_SYNC, MODE_THRESHOLD,  2                   ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_FIELD_SYNC, TOP_ID,          SYNC_ON_SECOND_HALF ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_FIELD_SYNC, TYPE,            BY_POSITION         )
	},

	{
		BCHP_VDEC_FE_0_SYNC_SLAVE_HRANGE,
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_HRANGE, SYNC_END,   50   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_HRANGE, SYNC_START, 1998 )
	},

	{
		BCHP_VDEC_FE_0_SYNC_SLAVE_LEVEL,
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_LEVEL, BLANK_LENGTH, 6    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_LEVEL, BLANK_START,  160  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_LEVEL, TIP_LENGTH,   5    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_LEVEL, TIP_START,    1948 )
	},

	{
		BCHP_VDEC_FE_0_SYNC_SLAVE_DIFF,
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_DIFF, MAX,        7  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_DIFF, LINE_RANGE, 9  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_DIFF, LINE_START, 10 )
	},

	{
		BCHP_VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN1,
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN1, FILT_SEL_NOISE_0, FILTB ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN1, FILT_SEL_NOISE_1, FILTB ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN1, NOISE_0,          63    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN1, NOISE_1,          63    )
	},

	{
		BCHP_VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN2,
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN2, FILT_SEL_NOISE_2, FILTA ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN2, FILT_SEL_NOISE_3, FILTA ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN2, NOISE_2,          63    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN2, NOISE_3,          63    )
	},

	{
		BCHP_VDEC_FE_0_BIG_SMALL_TH1,
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH1, SMALL_TO_BIG_TH_NOISE_0, 20 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH1, BIG_TO_SMALL_TH_NOISE_0, 5  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH1, SMALL_TO_BIG_TH_NOISE_1, 20 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH1, BIG_TO_SMALL_TH_NOISE_1, 5  )
	},

	{
		BCHP_VDEC_FE_0_BIG_SMALL_TH2,
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH2, SMALL_TO_BIG_TH_NOISE_2, 20 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH2, BIG_TO_SMALL_TH_NOISE_2, 5  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH2, SMALL_TO_BIG_TH_NOISE_3, 20 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH2, BIG_TO_SMALL_TH_NOISE_3, 5  )
	},

	{
		BCHP_VDEC_FE_0_HSYNC_ACC_GAIN1,
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN1, SMALL_NOISE_0, 74 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN1, BIG_NOISE_0,   21 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN1, SMALL_NOISE_1, 74 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN1, BIG_NOISE_1,   21 )
	},

	{
		BCHP_VDEC_FE_0_HSYNC_ACC_GAIN2,
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN2, SMALL_NOISE_2, 74 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN2, BIG_NOISE_2,   21 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN2, SMALL_NOISE_3, 74 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN2, BIG_NOISE_3,   21 )
	},

	{
		BCHP_VDEC_FE_0_CLEAN_VSYNC_CNT,
		BCHP_FIELD_DATA(VDEC_FE_0_CLEAN_VSYNC_CNT, NOISE_0, 36 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_CLEAN_VSYNC_CNT, NOISE_1, 36 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_CLEAN_VSYNC_CNT, NOISE_2, 36 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_CLEAN_VSYNC_CNT, NOISE_3, 36 )
	},

	{
		BCHP_VDEC_FE_0_NOISE_BIN_TH,
		BCHP_FIELD_DATA(VDEC_FE_0_NOISE_BIN_TH, HYSTERESIS, 5   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NOISE_BIN_TH, TH_2,       149 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NOISE_BIN_TH, TH_1,       75  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NOISE_BIN_TH, TH_0,       30  )
	},

	/* VDEC_FE_0_FE_STATUS0 (RO) */
	/* VDEC_FE_0_FE_STATUS (RO) */
	/* VDEC_FE_0_FE_FIELD_SIZE (RO) */
	/* VDEC_FE_0_FE_SAMPLE_COUNT (RO) */
	/* VDEC_FE_0_FE_SYNC_SLICE (RO) */
	/* VDEC_FE_0_FE_BLANK_LEVELS (RO) */
	/* VDEC_FE_0_FE_TIP_LEVELS (RO) */
	/* VDEC_FE_0_FE_AGC_LEVELS (RO) */

	/* Last Entry */
	{
		UINT32_C(-1),
		UINT32_C(-1)
	}
};


static const BVDC_P_AddrDataPair s_aVdecFmtTbl_YPrPb_1080i[] =
{
	/* VDEC_FE_0_FE_REV_ID (RO) */
	{
		BCHP_VDEC_FE_0_HSTART2,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_DATA(VDEC_FE_0_HSTART2, FILTER_COEF_SEL, 0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSTART2, WINDOW_SW_SELECT,    0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSTART2, WINDOW_MODE,    0 ) |
#endif
		BCHP_FIELD_DATA(VDEC_FE_0_HSTART2, STORE, 215 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSTART2, CS,    134 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSTART2, CB,    126 )
	},

	{
		BCHP_VDEC_FE_0_NB_CONTROL,
		BCHP_FIELD_DATA(VDEC_FE_0_NB_CONTROL, NOISE_LENGTH, 6    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NB_CONTROL, NOISE_START,  1024 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NB_CONTROL, BLANK_LENGTH, 6    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NB_CONTROL, BLANK_START,  511  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NB_CONTROL, LINE,         8    )
	},

	{
		BCHP_VDEC_FE_0_AGC_REGION,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_5)
		BCHP_FIELD_ENUM(VDEC_FE_0_AGC_REGION, AGC_PATH_SEP,     SEPARATE ) |
		BCHP_FIELD_DATA(VDEC_FE_0_AGC_REGION, AGC_GAIN_LIMIT_VDEC,     0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_AGC_REGION, AGC_GAIN_LIMIT_SYNC_SEP, 0 ) |
#elif (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_DATA(VDEC_FE_0_AGC_REGION, AGC_PATH_SEP,            1 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_AGC_REGION, AGC_GAIN_LIMIT_VDEC,     4 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_AGC_REGION, AGC_GAIN_LIMIT_SYNC_SEP, 4 ) |
#endif
		BCHP_FIELD_DATA(VDEC_FE_0_AGC_REGION, END,   127                 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_AGC_REGION, START, 6                   )
	},

	{
		BCHP_VDEC_FE_0_HPLL_MISC,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_MISC, IFD_PATH_SEL,        VDEC ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_MISC, NOISE_MEASURE_SEL,   VDEC ) |
#endif
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_MISC, SLICE_LPF_HPLL_LOCK_CTL, ENABLE ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_MISC, DISABLE_SLICE_TILT,  1   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_MISC, VSYNC_OFFSET,        343 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_MISC, MATCH_FILT_TH_COEFF, 12  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_MISC, JUMP_END_LINE,       0   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_MISC, JUMP_START_LINE,     1   )
	},

	{
		BCHP_VDEC_FE_0_RAW_VSYNC_CONTROL,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_VSYNC_CONTROL, MAX_HEAD_SWITCH_HYSTERESIS_TH, 5 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_VSYNC_CONTROL, MIN_HEAD_SWITCH_HYSTERESIS_TH, 3 ) |
#endif
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_VSYNC_CONTROL, VPLL_LINE_UPDATE, 1  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_VSYNC_CONTROL, CLEAN_LINE_COUNT, 36 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_VSYNC_CONTROL, BLOCK_LINE_COUNT, 25 )
	},

	{
		BCHP_VDEC_FE_0_SYNC_DETECT,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, DCR_SEL,           VDEC      ) |
#else
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, HEAD_SWITCH_SEL,   FOUR_HEAD ) |
#endif
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, TIP_FILT_SEL,      FILTA     ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, TIP_SELECT_MID,    0         ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, SLICE_CTL_DISABLE, 0         ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, MASK_EQ_SYNC,      0         ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, SIGNAL_TO_MONITOR, FILTA     ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, SLICE_MODE,        AUTO      ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, SLICE_DELAY,       2         ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, SLICE_AVG,         AVG8      ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, P_COUNT,           12        ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, H_COUNT,           24        ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, V_COUNT,           120       )
	},

	{
		BCHP_VDEC_FE_0_SYNC_DETECT2,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, HEAD_SWITCH_HISTORY_MASK, 0 ) |
#else
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, MATCH_FILTER_TH,      0 ) |
#endif
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, MATCH_REQ_LOCK,       0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, VCR_MODE,             0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, FREQ_OFFSET,          0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, CONSISTEN_ERROR_TH,   0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, HEAD_SWITCH_ERROR_TH, 0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, MATCH_FILTER_MODE,    0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, RAW_VSYNC_MODE,       0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, HEAD_SWITCH_THRESH,   6 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, LINE_OFFSET,          1 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, SLICE_FILT,           2 )
	},

	{
		BCHP_VDEC_FE_0_SYNC_BLANK,
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_BLANK, F1_END_LINE,   6  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_BLANK, F1_START_LINE, 63 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_BLANK, F0_END_LINE,   6  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_BLANK, F0_START_LINE, 63 )
	},

	{
		BCHP_VDEC_FE_0_RAW_HSYNC_CONTROL,
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, MATCH_OVERRIDE_MIN, 0  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, SAMPLE_TYPE_CTL,    0  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, MATCH_MIN,          32 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, INT_MIN,            45 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, INT_AREA,           0  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, MATCH_DELTA_COEFF,  12 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, DELTA_COEFF,        12 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, INT_TH_COEFF,       10 )
	},

	{
		BCHP_VDEC_FE_0_HSYNC_PLL1,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL1, FRO_FREQ,     FIXED   ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL1, AVG_EN,       UPDATED ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL1, FREQ_AVG,     ENABLE  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL1, FILTA_OFFSET, 0       ) |
#endif
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL1, BIG_SMALL_MODE, AUTO ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL1, DIRECT_GAIN,    4    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL1, SMALL_TERM_CNT, 0    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL1, BIG_TERM_CNT,   1    )
	},

	{
		BCHP_VDEC_FE_0_SYNC_ACCUM_CONTROL,
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_ACCUM_CONTROL, HSYNC_CLAMP_VALUE, 63 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_ACCUM_CONTROL, VSYNC_CLAMP_VALUE, 31 )
	},

	{
		BCHP_VDEC_FE_0_HSYNC_PLL,
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL, BY_PASS_LPF,  0   ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL, ACCUM_THRESH, OFF ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL, LOCK_THRESH,  3   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL, LOCK_WINDOW,  15  )
	},

	{
		BCHP_VDEC_FE_0_HSYNC_PLL_FILTER,
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL_FILTER, NOISE_3, 48 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL_FILTER, NOISE_2, 48 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL_FILTER, NOISE_1, 48 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL_FILTER, NOISE_0, 48 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL_FILTER, UNLOCK,  48 )
	},

	{
		BCHP_VDEC_FE_0_FRO_CONTROL,
		BCHP_FIELD_DATA(VDEC_FE_0_FRO_CONTROL, AVG_ERR_FACTOR, 2               ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_FRO_CONTROL, AUTO_COEFF,     ONE_AND_QUARTER ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FRO_CONTROL, FIELD_ID_TH,    20              ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FRO_CONTROL, CUT_OFF_TH,     31              ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FRO_CONTROL, ERR_TH,         20              ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FRO_CONTROL, FRO_MODE,       0               )
	},

	{
		BCHP_VDEC_FE_0_FIELD_SYNC,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_ENUM(VDEC_FE_0_FIELD_SYNC, FID_INVERT, NO_INVERT               ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_FIELD_SYNC, PC_INTERLACED_FID,  ENABLE          ) |
#endif
		BCHP_FIELD_DATA(VDEC_FE_0_FIELD_SYNC, ID_UPDATE_WINDOW, 0                   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FIELD_SYNC, MARKING_THRESH,  0                   ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_FIELD_SYNC, MARKING_WINDOW,  PLUS_MINUS_64       ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FIELD_SYNC, FIELD_SCOPE,     3                   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FIELD_SYNC, MODE_THRESHOLD,  2                   ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_FIELD_SYNC, TOP_ID,          SYNC_ON_SECOND_HALF ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_FIELD_SYNC, TYPE,            BY_POSITION         )
	},

	{
		BCHP_VDEC_FE_0_SYNC_SLAVE_HRANGE,
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_HRANGE, SYNC_END,   50   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_HRANGE, SYNC_START, 1998 )
	},

	{
		BCHP_VDEC_FE_0_SYNC_SLAVE_LEVEL,
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_LEVEL, BLANK_LENGTH, 6    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_LEVEL, BLANK_START,  160  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_LEVEL, TIP_LENGTH,   5    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_LEVEL, TIP_START,    1948 )
	},

	{
		BCHP_VDEC_FE_0_SYNC_SLAVE_DIFF,
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_DIFF, MAX,        7  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_DIFF, LINE_RANGE, 9  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_DIFF, LINE_START, 10 )
	},

	{
		BCHP_VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN1,
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN1, FILT_SEL_NOISE_0, FILTB ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN1, FILT_SEL_NOISE_1, FILTB ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN1, NOISE_0,          63    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN1, NOISE_1,          63    )
	},

	{
		BCHP_VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN2,
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN2, FILT_SEL_NOISE_2, FILTA ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN2, FILT_SEL_NOISE_3, FILTA ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN2, NOISE_2,          63    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN2, NOISE_3,          63    )
	},

	{
		BCHP_VDEC_FE_0_BIG_SMALL_TH1,
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH1, SMALL_TO_BIG_TH_NOISE_0, 20 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH1, BIG_TO_SMALL_TH_NOISE_0, 5  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH1, SMALL_TO_BIG_TH_NOISE_1, 20 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH1, BIG_TO_SMALL_TH_NOISE_1, 5  )
	},

	{
		BCHP_VDEC_FE_0_BIG_SMALL_TH2,
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH2, SMALL_TO_BIG_TH_NOISE_2, 20 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH2, BIG_TO_SMALL_TH_NOISE_2, 5  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH2, SMALL_TO_BIG_TH_NOISE_3, 20 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH2, BIG_TO_SMALL_TH_NOISE_3, 5  )
	},

	{
		BCHP_VDEC_FE_0_HSYNC_ACC_GAIN1,
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN1, SMALL_NOISE_0, 74 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN1, BIG_NOISE_0,   21 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN1, SMALL_NOISE_1, 74 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN1, BIG_NOISE_1,   21 )
	},

	{
		BCHP_VDEC_FE_0_HSYNC_ACC_GAIN2,
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN2, SMALL_NOISE_2, 74 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN2, BIG_NOISE_2,   21 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN2, SMALL_NOISE_3, 74 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN2, BIG_NOISE_3,   21 )
	},

	{
		BCHP_VDEC_FE_0_CLEAN_VSYNC_CNT,
		BCHP_FIELD_DATA(VDEC_FE_0_CLEAN_VSYNC_CNT, NOISE_0, 36 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_CLEAN_VSYNC_CNT, NOISE_1, 36 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_CLEAN_VSYNC_CNT, NOISE_2, 36 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_CLEAN_VSYNC_CNT, NOISE_3, 36 )
	},

	{
		BCHP_VDEC_FE_0_NOISE_BIN_TH,
		BCHP_FIELD_DATA(VDEC_FE_0_NOISE_BIN_TH, HYSTERESIS, 5   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NOISE_BIN_TH, TH_2,       149 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NOISE_BIN_TH, TH_1,       75  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NOISE_BIN_TH, TH_0,       30  )
	},

	/* VDEC_FE_0_FE_STATUS0 (RO) */
	/* VDEC_FE_0_FE_STATUS (RO) */
	/* VDEC_FE_0_FE_FIELD_SIZE (RO) */
	/* VDEC_FE_0_FE_SAMPLE_COUNT (RO) */
	/* VDEC_FE_0_FE_SYNC_SLICE (RO) */
	/* VDEC_FE_0_FE_BLANK_LEVELS (RO) */
	/* VDEC_FE_0_FE_TIP_LEVELS (RO) */
	/* VDEC_FE_0_FE_AGC_LEVELS (RO) */

	/* Last Entry */
	{
		UINT32_C(-1),
		UINT32_C(-1)
	}
};


static const BVDC_P_AddrDataPair s_aVdecFmtTbl_YPrPb_720p[]  =
{
	/* VDEC_FE_0_FE_REV_ID (RO) */
	{
		BCHP_VDEC_FE_0_HSTART2,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_DATA(VDEC_FE_0_HSTART2, FILTER_COEF_SEL, 0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSTART2, WINDOW_SW_SELECT,    0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSTART2, WINDOW_MODE,    0 ) |
#endif
		BCHP_FIELD_DATA(VDEC_FE_0_HSTART2, STORE, 215 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSTART2, CS,    134 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSTART2, CB,    126 )
	},

	{
		BCHP_VDEC_FE_0_NB_CONTROL,
		BCHP_FIELD_DATA(VDEC_FE_0_NB_CONTROL, NOISE_LENGTH, 6    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NB_CONTROL, NOISE_START,  1024 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NB_CONTROL, BLANK_LENGTH, 6    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NB_CONTROL, BLANK_START,  511  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NB_CONTROL, LINE,         8    )
	},

	{
		BCHP_VDEC_FE_0_AGC_REGION,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_5)
		BCHP_FIELD_ENUM(VDEC_FE_0_AGC_REGION, AGC_PATH_SEP,     SEPARATE ) |
		BCHP_FIELD_DATA(VDEC_FE_0_AGC_REGION, AGC_GAIN_LIMIT_VDEC,     0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_AGC_REGION, AGC_GAIN_LIMIT_SYNC_SEP, 0 ) |
#elif (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_ENUM(VDEC_FE_0_AGC_REGION, AGC_PATH_SEP,     SEPARATE ) |
		BCHP_FIELD_DATA(VDEC_FE_0_AGC_REGION, AGC_GAIN_LIMIT_VDEC,     4 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_AGC_REGION, AGC_GAIN_LIMIT_SYNC_SEP, 2 ) |
#endif
		BCHP_FIELD_DATA(VDEC_FE_0_AGC_REGION, END,   127                 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_AGC_REGION, START, 6                   )
	},

	{
		BCHP_VDEC_FE_0_HPLL_MISC,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_MISC, IFD_PATH_SEL,        VDEC ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_MISC, NOISE_MEASURE_SEL,   VDEC ) |
#endif
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_MISC, SLICE_LPF_HPLL_LOCK_CTL, ENABLE ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_MISC, DISABLE_SLICE_TILT,  1   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_MISC, VSYNC_OFFSET,        436 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_MISC, MATCH_FILT_TH_COEFF, 12  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_MISC, JUMP_END_LINE,       0   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_MISC, JUMP_START_LINE,     1   )
	},

	{
		BCHP_VDEC_FE_0_RAW_VSYNC_CONTROL,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_VSYNC_CONTROL, MAX_HEAD_SWITCH_HYSTERESIS_TH, 5 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_VSYNC_CONTROL, MIN_HEAD_SWITCH_HYSTERESIS_TH, 3 ) |
#endif
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_VSYNC_CONTROL, VPLL_LINE_UPDATE, 1  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_VSYNC_CONTROL, CLEAN_LINE_COUNT, 36 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_VSYNC_CONTROL, BLOCK_LINE_COUNT, 25 )
	},

	{
		BCHP_VDEC_FE_0_SYNC_DETECT,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, DCR_SEL,           VDEC      ) |
#else
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, HEAD_SWITCH_SEL,   FOUR_HEAD ) |
#endif
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, TIP_FILT_SEL,      FILTA     ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, TIP_SELECT_MID,    0         ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, SLICE_CTL_DISABLE, 0         ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, MASK_EQ_SYNC,      0         ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, SIGNAL_TO_MONITOR, FILTA     ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, SLICE_MODE,        AUTO      ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, SLICE_DELAY,       7         ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, SLICE_AVG,         AVG8      ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, P_COUNT,           11        ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, H_COUNT,           22        ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, V_COUNT,           168       )
	},

	{
		BCHP_VDEC_FE_0_SYNC_DETECT2,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, HEAD_SWITCH_HISTORY_MASK, 0 ) |
#else
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, MATCH_FILTER_TH,      0 ) |
#endif
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, MATCH_REQ_LOCK,       0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, VCR_MODE,             0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, FREQ_OFFSET,          0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, CONSISTEN_ERROR_TH,   0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, HEAD_SWITCH_ERROR_TH, 0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, MATCH_FILTER_MODE,    0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, RAW_VSYNC_MODE,       0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, HEAD_SWITCH_THRESH,   6 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, LINE_OFFSET,          1 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, SLICE_FILT,           2 )
	},

	{
		BCHP_VDEC_FE_0_SYNC_BLANK,
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_BLANK, F1_END_LINE,   6  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_BLANK, F1_START_LINE, 63 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_BLANK, F0_END_LINE,   6  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_BLANK, F0_START_LINE, 63 )
	},

	{
		BCHP_VDEC_FE_0_RAW_HSYNC_CONTROL,
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, MATCH_OVERRIDE_MIN, 0  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, SAMPLE_TYPE_CTL,    0  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, MATCH_MIN,          32 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, INT_MIN,            45 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, INT_AREA,           0  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, MATCH_DELTA_COEFF,  12 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, DELTA_COEFF,        12 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, INT_TH_COEFF,       10 )
	},

	{
		BCHP_VDEC_FE_0_HSYNC_PLL1,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL1, FRO_FREQ,     FIXED   ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL1, AVG_EN,       UPDATED ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL1, FREQ_AVG,     ENABLE  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL1, FILTA_OFFSET, 0       ) |
#endif
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL1, BIG_SMALL_MODE, AUTO ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL1, DIRECT_GAIN,    4    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL1, SMALL_TERM_CNT, 0    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL1, BIG_TERM_CNT,   1    )
	},

	{
		BCHP_VDEC_FE_0_SYNC_ACCUM_CONTROL,
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_ACCUM_CONTROL, HSYNC_CLAMP_VALUE, 63 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_ACCUM_CONTROL, VSYNC_CLAMP_VALUE, 31 )
	},

	{
		BCHP_VDEC_FE_0_HSYNC_PLL,
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL, BY_PASS_LPF,  0   ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL, ACCUM_THRESH, OFF ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL, LOCK_THRESH,  3   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL, LOCK_WINDOW,  15  )
	},

	{
		BCHP_VDEC_FE_0_HSYNC_PLL_FILTER,
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL_FILTER, NOISE_3, 48 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL_FILTER, NOISE_2, 48 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL_FILTER, NOISE_1, 48 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL_FILTER, NOISE_0, 48 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL_FILTER, UNLOCK,  48 )
	},

	{
		BCHP_VDEC_FE_0_FRO_CONTROL,
		BCHP_FIELD_DATA(VDEC_FE_0_FRO_CONTROL, AVG_ERR_FACTOR, 1   ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_FRO_CONTROL, AUTO_COEFF,     ONE ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FRO_CONTROL, FIELD_ID_TH,    20  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FRO_CONTROL, CUT_OFF_TH,     31  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FRO_CONTROL, ERR_TH,         20  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FRO_CONTROL, FRO_MODE,       0   )
	},

	{
		BCHP_VDEC_FE_0_FIELD_SYNC,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_ENUM(VDEC_FE_0_FIELD_SYNC, FID_INVERT, NO_INVERT               ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_FIELD_SYNC, PC_INTERLACED_FID,  ENABLE          ) |
#endif
		BCHP_FIELD_DATA(VDEC_FE_0_FIELD_SYNC, ID_UPDATE_WINDOW, 0                  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FIELD_SYNC, MARKING_THRESH,  0                  ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_FIELD_SYNC, MARKING_WINDOW,  PLUS_MINUS_64      ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FIELD_SYNC, FIELD_SCOPE,     2                  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FIELD_SYNC, MODE_THRESHOLD,  2                  ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_FIELD_SYNC, TOP_ID,          SYNC_ON_FIRST_HALF ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_FIELD_SYNC, TYPE,            BY_POSITION        )
	},

	{
		BCHP_VDEC_FE_0_SYNC_SLAVE_HRANGE,
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_HRANGE, SYNC_END,   50   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_HRANGE, SYNC_START, 1998 )
	},

	{
		BCHP_VDEC_FE_0_SYNC_SLAVE_LEVEL,
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_LEVEL, BLANK_LENGTH, 6    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_LEVEL, BLANK_START,  160  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_LEVEL, TIP_LENGTH,   5    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_LEVEL, TIP_START,    1990 )
	},

	{
		BCHP_VDEC_FE_0_SYNC_SLAVE_DIFF,
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_DIFF, MAX,        7  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_DIFF, LINE_RANGE, 9  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_DIFF, LINE_START, 10 )
	},

	{
		BCHP_VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN1,
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN1, FILT_SEL_NOISE_0, FILTB ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN1, FILT_SEL_NOISE_1, FILTB ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN1, NOISE_0,          63    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN1, NOISE_1,          63    )
	},

	{
		BCHP_VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN2,
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN2, FILT_SEL_NOISE_2, FILTA ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN2, FILT_SEL_NOISE_3, FILTA ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN2, NOISE_2,          63    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN2, NOISE_3,          63    )
	},

	{
		BCHP_VDEC_FE_0_BIG_SMALL_TH1,
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH1, SMALL_TO_BIG_TH_NOISE_0, 20 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH1, BIG_TO_SMALL_TH_NOISE_0, 5  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH1, SMALL_TO_BIG_TH_NOISE_1, 20 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH1, BIG_TO_SMALL_TH_NOISE_1, 5  )
	},

	{
		BCHP_VDEC_FE_0_BIG_SMALL_TH2,
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH2, SMALL_TO_BIG_TH_NOISE_2, 20 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH2, BIG_TO_SMALL_TH_NOISE_2, 5  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH2, SMALL_TO_BIG_TH_NOISE_3, 20 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH2, BIG_TO_SMALL_TH_NOISE_3, 5  )
	},

	{
		BCHP_VDEC_FE_0_HSYNC_ACC_GAIN1,
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN1, SMALL_NOISE_0, 74 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN1, BIG_NOISE_0,   21 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN1, SMALL_NOISE_1, 74 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN1, BIG_NOISE_1,   21 )
	},

	{
		BCHP_VDEC_FE_0_HSYNC_ACC_GAIN2,
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN2, SMALL_NOISE_2, 74 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN2, BIG_NOISE_2,   21 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN2, SMALL_NOISE_3, 74 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN2, BIG_NOISE_3,   21 )
	},

	{
		BCHP_VDEC_FE_0_CLEAN_VSYNC_CNT,
		BCHP_FIELD_DATA(VDEC_FE_0_CLEAN_VSYNC_CNT, NOISE_0, 36 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_CLEAN_VSYNC_CNT, NOISE_1, 36 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_CLEAN_VSYNC_CNT, NOISE_2, 36 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_CLEAN_VSYNC_CNT, NOISE_3, 36 )
	},

	{
		BCHP_VDEC_FE_0_NOISE_BIN_TH,
		BCHP_FIELD_DATA(VDEC_FE_0_NOISE_BIN_TH, HYSTERESIS, 5   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NOISE_BIN_TH, TH_2,       149 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NOISE_BIN_TH, TH_1,       75  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NOISE_BIN_TH, TH_0,       30  )
	},

	/* VDEC_FE_0_FE_STATUS0 (RO) */
	/* VDEC_FE_0_FE_STATUS (RO) */
	/* VDEC_FE_0_FE_FIELD_SIZE (RO) */
	/* VDEC_FE_0_FE_SAMPLE_COUNT (RO) */
	/* VDEC_FE_0_FE_SYNC_SLICE (RO) */
	/* VDEC_FE_0_FE_BLANK_LEVELS (RO) */
	/* VDEC_FE_0_FE_TIP_LEVELS (RO) */
	/* VDEC_FE_0_FE_AGC_LEVELS (RO) */

	/* Last Entry */
	{
		UINT32_C(-1),
		UINT32_C(-1)
	}
};

static const BVDC_P_AddrDataPair s_aVdecFmtTbl_YPrPb_480p[]  =
{
	/* VDEC_FE_0_FE_REV_ID (RO) */
	{
		BCHP_VDEC_FE_0_HSTART2,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_DATA(VDEC_FE_0_HSTART2, FILTER_COEF_SEL, 0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSTART2, WINDOW_SW_SELECT,    0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSTART2, WINDOW_MODE,    0 ) |
#endif
		BCHP_FIELD_DATA(VDEC_FE_0_HSTART2, STORE, 215 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSTART2, CS,    134 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSTART2, CB,    126 )
	},

	{
		BCHP_VDEC_FE_0_NB_CONTROL,
		BCHP_FIELD_DATA(VDEC_FE_0_NB_CONTROL, NOISE_LENGTH, 7    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NB_CONTROL, NOISE_START,  1024 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NB_CONTROL, BLANK_LENGTH, 7    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NB_CONTROL, BLANK_START,  0    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NB_CONTROL, LINE,         8    )
	},

	{
		BCHP_VDEC_FE_0_AGC_REGION,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_5)
		BCHP_FIELD_ENUM(VDEC_FE_0_AGC_REGION, AGC_PATH_SEP,     SEPARATE ) |
		BCHP_FIELD_DATA(VDEC_FE_0_AGC_REGION, AGC_GAIN_LIMIT_VDEC,     0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_AGC_REGION, AGC_GAIN_LIMIT_SYNC_SEP, 0 ) |
#elif (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_ENUM(VDEC_FE_0_AGC_REGION, AGC_PATH_SEP,     SEPARATE ) |
		BCHP_FIELD_DATA(VDEC_FE_0_AGC_REGION, AGC_GAIN_LIMIT_VDEC,     4 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_AGC_REGION, AGC_GAIN_LIMIT_SYNC_SEP, 4 ) |
#endif
		BCHP_FIELD_DATA(VDEC_FE_0_AGC_REGION, END,   125                 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_AGC_REGION, START, 7                   )
	},

	{
		BCHP_VDEC_FE_0_HPLL_MISC,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_MISC, IFD_PATH_SEL,        VDEC ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_MISC, NOISE_MEASURE_SEL,   VDEC ) |
#endif
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_MISC, SLICE_LPF_HPLL_LOCK_CTL, ENABLE ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_MISC, DISABLE_SLICE_TILT,  1  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_MISC, VSYNC_OFFSET,        0  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_MISC, MATCH_FILT_TH_COEFF, 12 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_MISC, JUMP_END_LINE,       18 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_MISC, JUMP_START_LINE,     10 )
	},

	{
		BCHP_VDEC_FE_0_RAW_VSYNC_CONTROL,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_VSYNC_CONTROL, MAX_HEAD_SWITCH_HYSTERESIS_TH, 5 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_VSYNC_CONTROL, MIN_HEAD_SWITCH_HYSTERESIS_TH, 3 ) |
#endif
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_VSYNC_CONTROL, VPLL_LINE_UPDATE, 1  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_VSYNC_CONTROL, CLEAN_LINE_COUNT, 36 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_VSYNC_CONTROL, BLOCK_LINE_COUNT, 25 )
	},

	{
		BCHP_VDEC_FE_0_SYNC_DETECT,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, DCR_SEL,           VDEC      ) |
#else
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, HEAD_SWITCH_SEL,   FOUR_HEAD ) |
#endif
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, TIP_FILT_SEL,      FILTA     ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, TIP_SELECT_MID,    0         ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, SLICE_CTL_DISABLE, 0         ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, MASK_EQ_SYNC,      0         ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, SIGNAL_TO_MONITOR, AUTO      ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, SLICE_MODE,        AUTO      ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, SLICE_DELAY,       2         ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, SLICE_AVG,         AVG16     ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, P_COUNT,           20        ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, H_COUNT,           34        ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, V_COUNT,           144       )
	},

	{
		BCHP_VDEC_FE_0_SYNC_DETECT2,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, HEAD_SWITCH_HISTORY_MASK, 0 ) |
#else
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, MATCH_FILTER_TH,      0 ) |
#endif
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, MATCH_REQ_LOCK,       0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, VCR_MODE,             0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, FREQ_OFFSET,          0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, CONSISTEN_ERROR_TH,   0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, HEAD_SWITCH_ERROR_TH, 0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, MATCH_FILTER_MODE,    0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, RAW_VSYNC_MODE,       0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, HEAD_SWITCH_THRESH,   6 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, LINE_OFFSET,          7 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, SLICE_FILT,           2 )
	},

	{
		BCHP_VDEC_FE_0_SYNC_BLANK,
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_BLANK, F1_END_LINE,   7  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_BLANK, F1_START_LINE, 61 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_BLANK, F0_END_LINE,   7  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_BLANK, F0_START_LINE, 61 )
	},

	{
		BCHP_VDEC_FE_0_RAW_HSYNC_CONTROL,
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, MATCH_OVERRIDE_MIN, 1  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, SAMPLE_TYPE_CTL,    0  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, MATCH_MIN,          32 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, INT_MIN,            45 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, INT_AREA,           1  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, MATCH_DELTA_COEFF,  12 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, DELTA_COEFF,        12 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, INT_TH_COEFF,       10 )
	},

	{
		BCHP_VDEC_FE_0_HSYNC_PLL1,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL1, FRO_FREQ,     FIXED   ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL1, AVG_EN,       UPDATED ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL1, FREQ_AVG,     ENABLE  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL1, FILTA_OFFSET, 0       ) |
#endif
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL1, BIG_SMALL_MODE, AUTO ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL1, DIRECT_GAIN,    4    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL1, SMALL_TERM_CNT, 0    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL1, BIG_TERM_CNT,   1    )
	},

	{
		BCHP_VDEC_FE_0_SYNC_ACCUM_CONTROL,
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_ACCUM_CONTROL, HSYNC_CLAMP_VALUE, 63 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_ACCUM_CONTROL, VSYNC_CLAMP_VALUE, 31 )
	},

	{
		BCHP_VDEC_FE_0_HSYNC_PLL,
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL, BY_PASS_LPF,  0   ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL, ACCUM_THRESH, OFF ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL, LOCK_THRESH,  3   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL, LOCK_WINDOW,  15  )
	},

	{
		BCHP_VDEC_FE_0_HSYNC_PLL_FILTER,
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL_FILTER, NOISE_3, 48 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL_FILTER, NOISE_2, 48 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL_FILTER, NOISE_1, 48 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL_FILTER, NOISE_0, 48 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL_FILTER, UNLOCK,  48 )
	},

	{
		BCHP_VDEC_FE_0_FRO_CONTROL,
		BCHP_FIELD_DATA(VDEC_FE_0_FRO_CONTROL, AVG_ERR_FACTOR, 2               ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_FRO_CONTROL, AUTO_COEFF,     ONE_AND_QUARTER ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FRO_CONTROL, FIELD_ID_TH,    20              ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FRO_CONTROL, CUT_OFF_TH,     31              ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FRO_CONTROL, ERR_TH,         20              ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FRO_CONTROL, FRO_MODE,       0               )
	},

	{
		BCHP_VDEC_FE_0_FIELD_SYNC,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_ENUM(VDEC_FE_0_FIELD_SYNC, FID_INVERT, NO_INVERT               ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_FIELD_SYNC, PC_INTERLACED_FID,  ENABLE          ) |
#endif
		BCHP_FIELD_DATA(VDEC_FE_0_FIELD_SYNC, ID_UPDATE_WINDOW, 0                  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FIELD_SYNC, MARKING_THRESH,  0                  ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_FIELD_SYNC, MARKING_WINDOW,  PLUS_MINUS_64      ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FIELD_SYNC, FIELD_SCOPE,     2                  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FIELD_SYNC, MODE_THRESHOLD,  2                  ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_FIELD_SYNC, TOP_ID,          SYNC_ON_FIRST_HALF ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_FIELD_SYNC, TYPE,            BY_POSITION        )
	},

	{
		BCHP_VDEC_FE_0_SYNC_SLAVE_HRANGE,
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_HRANGE, SYNC_END,   50   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_HRANGE, SYNC_START, 1998 )
	},

	{
		BCHP_VDEC_FE_0_SYNC_SLAVE_LEVEL,
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_LEVEL, BLANK_LENGTH, 6   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_LEVEL, BLANK_START,  160 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_LEVEL, TIP_LENGTH,   5   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_LEVEL, TIP_START,    40  )
	},

	{
		BCHP_VDEC_FE_0_SYNC_SLAVE_DIFF,
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_DIFF, MAX,        7  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_DIFF, LINE_RANGE, 9  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_DIFF, LINE_START, 10 )
	},

	{
		BCHP_VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN1,
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN1, FILT_SEL_NOISE_0, FILTB ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN1, FILT_SEL_NOISE_1, FILTB ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN1, NOISE_0,          63    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN1, NOISE_1,          63    )
	},

	{
		BCHP_VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN2,
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN2, FILT_SEL_NOISE_2, FILTA ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN2, FILT_SEL_NOISE_3, FILTA ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN2, NOISE_2,          63    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN2, NOISE_3,          63    )
	},

	{
		BCHP_VDEC_FE_0_BIG_SMALL_TH1,
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH1, SMALL_TO_BIG_TH_NOISE_0, 20 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH1, BIG_TO_SMALL_TH_NOISE_0, 5  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH1, SMALL_TO_BIG_TH_NOISE_1, 20 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH1, BIG_TO_SMALL_TH_NOISE_1, 5  )
	},

	{
		BCHP_VDEC_FE_0_BIG_SMALL_TH2,
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH2, SMALL_TO_BIG_TH_NOISE_2, 20 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH2, BIG_TO_SMALL_TH_NOISE_2, 5  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH2, SMALL_TO_BIG_TH_NOISE_3, 20 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH2, BIG_TO_SMALL_TH_NOISE_3, 5  )
	},

	{
		BCHP_VDEC_FE_0_HSYNC_ACC_GAIN1,
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN1, SMALL_NOISE_0, 74 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN1, BIG_NOISE_0,   21 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN1, SMALL_NOISE_1, 74 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN1, BIG_NOISE_1,   21 )
	},

	{
		BCHP_VDEC_FE_0_HSYNC_ACC_GAIN2,
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN2, SMALL_NOISE_2, 74 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN2, BIG_NOISE_2,   21 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN2, SMALL_NOISE_3, 74 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN2, BIG_NOISE_3,   21 )
	},

	{
		BCHP_VDEC_FE_0_CLEAN_VSYNC_CNT,
		BCHP_FIELD_DATA(VDEC_FE_0_CLEAN_VSYNC_CNT, NOISE_0, 36 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_CLEAN_VSYNC_CNT, NOISE_1, 36 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_CLEAN_VSYNC_CNT, NOISE_2, 36 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_CLEAN_VSYNC_CNT, NOISE_3, 36 )
	},

	{
		BCHP_VDEC_FE_0_NOISE_BIN_TH,
		BCHP_FIELD_DATA(VDEC_FE_0_NOISE_BIN_TH, HYSTERESIS, 5   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NOISE_BIN_TH, TH_2,       149 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NOISE_BIN_TH, TH_1,       75  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NOISE_BIN_TH, TH_0,       30  )
	},

	/* VDEC_FE_0_FE_STATUS0 (RO) */
	/* VDEC_FE_0_FE_STATUS (RO) */
	/* VDEC_FE_0_FE_FIELD_SIZE (RO) */
	/* VDEC_FE_0_FE_SAMPLE_COUNT (RO) */
	/* VDEC_FE_0_FE_SYNC_SLICE (RO) */
	/* VDEC_FE_0_FE_BLANK_LEVELS (RO) */
	/* VDEC_FE_0_FE_TIP_LEVELS (RO) */
	/* VDEC_FE_0_FE_AGC_LEVELS (RO) */

	/* Last Entry */
	{
		UINT32_C(-1),
		UINT32_C(-1)
	}
};

static const BVDC_P_AddrDataPair s_aVdecFmtTbl_YPrPb_NTSC[]  =
{
	/* VDEC_FE_0_FE_REV_ID (RO) */
	{
		BCHP_VDEC_FE_0_HSTART2,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_DATA(VDEC_FE_0_HSTART2, FILTER_COEF_SEL, 0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSTART2, WINDOW_SW_SELECT,    0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSTART2, WINDOW_MODE,    0 ) |
#endif
		BCHP_FIELD_DATA(VDEC_FE_0_HSTART2, STORE, 215 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSTART2, CS,    134 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSTART2, CB,    126 )
	},

	{
		BCHP_VDEC_FE_0_NB_CONTROL,
		BCHP_FIELD_DATA(VDEC_FE_0_NB_CONTROL, NOISE_LENGTH, 6   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NB_CONTROL, NOISE_START,  958 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NB_CONTROL, BLANK_LENGTH, 6   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NB_CONTROL, BLANK_START,  96  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NB_CONTROL, LINE,         8   )
	},

	{
		BCHP_VDEC_FE_0_AGC_REGION,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_5)
		BCHP_FIELD_ENUM(VDEC_FE_0_AGC_REGION, AGC_PATH_SEP,     SEPARATE ) |
		BCHP_FIELD_DATA(VDEC_FE_0_AGC_REGION, AGC_GAIN_LIMIT_VDEC,     0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_AGC_REGION, AGC_GAIN_LIMIT_SYNC_SEP, 0 ) |
#elif (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_ENUM(VDEC_FE_0_AGC_REGION, AGC_PATH_SEP,     SEPARATE ) |
		BCHP_FIELD_DATA(VDEC_FE_0_AGC_REGION, AGC_GAIN_LIMIT_VDEC,     4 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_AGC_REGION, AGC_GAIN_LIMIT_SYNC_SEP, 4 ) |
#endif
		BCHP_FIELD_DATA(VDEC_FE_0_AGC_REGION, END,   124                 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_AGC_REGION, START, 12                  )
	},

	{
		BCHP_VDEC_FE_0_HPLL_MISC,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_MISC, IFD_PATH_SEL,        VDEC ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_MISC, NOISE_MEASURE_SEL,   VDEC ) |
#endif
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_MISC, SLICE_LPF_HPLL_LOCK_CTL, ENABLE ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_MISC, DISABLE_SLICE_TILT,  1  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_MISC, VSYNC_OFFSET,        0  ) |
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_MISC, MATCH_FILT_TH_COEFF, 10 ) |
#else
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_MISC, MATCH_FILT_TH_COEFF, 12 ) |
#endif
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_MISC, JUMP_END_LINE,       18 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_MISC, JUMP_START_LINE,     10 )
	},

	{
		BCHP_VDEC_FE_0_RAW_VSYNC_CONTROL,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_VSYNC_CONTROL, MAX_HEAD_SWITCH_HYSTERESIS_TH, 5 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_VSYNC_CONTROL, MIN_HEAD_SWITCH_HYSTERESIS_TH, 3 ) |
#endif
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_VSYNC_CONTROL, VPLL_LINE_UPDATE, 1  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_VSYNC_CONTROL, CLEAN_LINE_COUNT, 34 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_VSYNC_CONTROL, BLOCK_LINE_COUNT, 25 )
	},

	{
		BCHP_VDEC_FE_0_SYNC_DETECT,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, DCR_SEL,           VDEC      ) |
#else
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, HEAD_SWITCH_SEL,   FOUR_HEAD ) |
#endif
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, TIP_FILT_SEL,      FILTA     ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, TIP_SELECT_MID,    0         ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, SLICE_CTL_DISABLE, 0         ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, MASK_EQ_SYNC,      0         ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, SIGNAL_TO_MONITOR, AUTO      ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, SLICE_MODE,        AUTO      ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, SLICE_DELAY,       3         ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, SLICE_AVG,         AVG16     ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, P_COUNT,           20        ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, H_COUNT,           48        ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, V_COUNT,           68        )
	},

	{
		BCHP_VDEC_FE_0_SYNC_DETECT2,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, HEAD_SWITCH_HISTORY_MASK, 0 ) |
#else
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, MATCH_FILTER_TH,      0 ) |
#endif
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, MATCH_REQ_LOCK,       0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, VCR_MODE,             0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, FREQ_OFFSET,          0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, CONSISTEN_ERROR_TH,   0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, HEAD_SWITCH_ERROR_TH, 0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, MATCH_FILTER_MODE,    1 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, RAW_VSYNC_MODE,       0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, HEAD_SWITCH_THRESH,   6 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, LINE_OFFSET,          4 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, SLICE_FILT,           4 )
	},

	{
		BCHP_VDEC_FE_0_SYNC_BLANK,
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_BLANK, F1_END_LINE,   6  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_BLANK, F1_START_LINE, 61 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_BLANK, F0_END_LINE,   6  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_BLANK, F0_START_LINE, 61 )
	},

	{
		BCHP_VDEC_FE_0_RAW_HSYNC_CONTROL,
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, MATCH_OVERRIDE_MIN, 1  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, SAMPLE_TYPE_CTL,    0  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, MATCH_MIN,          32 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, INT_MIN,            45 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, INT_AREA,           1  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, MATCH_DELTA_COEFF,  12 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, DELTA_COEFF,        12 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, INT_TH_COEFF,       10 )
	},

	{
		BCHP_VDEC_FE_0_HSYNC_PLL1,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL1, FRO_FREQ,     FIXED   ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL1, AVG_EN,       UPDATED ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL1, FREQ_AVG,     ENABLE  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL1, FILTA_OFFSET, 0       ) |
#endif
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL1, BIG_SMALL_MODE, AUTO ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL1, DIRECT_GAIN,    4    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL1, SMALL_TERM_CNT, 0    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL1, BIG_TERM_CNT,   1    )
	},

	{
		BCHP_VDEC_FE_0_SYNC_ACCUM_CONTROL,
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_ACCUM_CONTROL, HSYNC_CLAMP_VALUE, 63 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_ACCUM_CONTROL, VSYNC_CLAMP_VALUE, 31 )
	},

	{
		BCHP_VDEC_FE_0_HSYNC_PLL,
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL, BY_PASS_LPF,  0   ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL, ACCUM_THRESH, OFF ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL, LOCK_THRESH,  3   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL, LOCK_WINDOW,  15  )
	},

	{
		BCHP_VDEC_FE_0_HSYNC_PLL_FILTER,
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL_FILTER, NOISE_3, 48 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL_FILTER, NOISE_2, 48 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL_FILTER, NOISE_1, 48 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL_FILTER, NOISE_0, 48 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL_FILTER, UNLOCK,  48 )
	},

	{
		BCHP_VDEC_FE_0_FRO_CONTROL,
		BCHP_FIELD_DATA(VDEC_FE_0_FRO_CONTROL, AVG_ERR_FACTOR, 2               ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_FRO_CONTROL, AUTO_COEFF,     ONE_AND_QUARTER ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FRO_CONTROL, FIELD_ID_TH,    20              ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FRO_CONTROL, CUT_OFF_TH,     31              ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FRO_CONTROL, ERR_TH,         20              ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FRO_CONTROL, FRO_MODE,       0               )
	},

	{
		BCHP_VDEC_FE_0_FIELD_SYNC,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_ENUM(VDEC_FE_0_FIELD_SYNC, FID_INVERT, NO_INVERT               ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_FIELD_SYNC, PC_INTERLACED_FID,  ENABLE          ) |
#endif
		BCHP_FIELD_DATA(VDEC_FE_0_FIELD_SYNC, ID_UPDATE_WINDOW, 0                   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FIELD_SYNC, MARKING_THRESH,  0                   ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_FIELD_SYNC, MARKING_WINDOW,  PLUS_MINUS_64       ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FIELD_SYNC, FIELD_SCOPE,     3                   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FIELD_SYNC, MODE_THRESHOLD,  2                   ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_FIELD_SYNC, TOP_ID,          SYNC_ON_SECOND_HALF ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_FIELD_SYNC, TYPE,            BY_POSITION         )
	},

	{
		BCHP_VDEC_FE_0_SYNC_SLAVE_HRANGE,
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_HRANGE, SYNC_END,   50   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_HRANGE, SYNC_START, 1998 )
	},

	{
		BCHP_VDEC_FE_0_SYNC_SLAVE_LEVEL,
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_LEVEL, BLANK_LENGTH, 6   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_LEVEL, BLANK_START,  160 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_LEVEL, TIP_LENGTH,   5   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_LEVEL, TIP_START,    40  )
	},

	{
		BCHP_VDEC_FE_0_SYNC_SLAVE_DIFF,
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_DIFF, MAX,        7  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_DIFF, LINE_RANGE, 8  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_DIFF, LINE_START, 10 )
	},

	{
		BCHP_VDEC_FE_0_CLEAN_VSYNC_CNT,
		BCHP_FIELD_DATA(VDEC_FE_0_CLEAN_VSYNC_CNT, NOISE_0, 36 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_CLEAN_VSYNC_CNT, NOISE_1, 36 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_CLEAN_VSYNC_CNT, NOISE_2, 36 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_CLEAN_VSYNC_CNT, NOISE_3, 36 )
	},

	{
		BCHP_VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN1,
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN1, FILT_SEL_NOISE_0, FILTB ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN1, FILT_SEL_NOISE_1, FILTB ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN1, NOISE_0,          63    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN1, NOISE_1,          63    )
	},

	{
		BCHP_VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN2,
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN2, FILT_SEL_NOISE_2, FILTA ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN2, FILT_SEL_NOISE_3, FILTA ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN2, NOISE_2,          63    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN2, NOISE_3,          63    )
	},

	{
		BCHP_VDEC_FE_0_BIG_SMALL_TH1,
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH1, SMALL_TO_BIG_TH_NOISE_0, 20 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH1, BIG_TO_SMALL_TH_NOISE_0, 5  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH1, SMALL_TO_BIG_TH_NOISE_1, 20 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH1, BIG_TO_SMALL_TH_NOISE_1, 5  )
	},

	{
		BCHP_VDEC_FE_0_BIG_SMALL_TH2,
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH2, SMALL_TO_BIG_TH_NOISE_2, 20 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH2, BIG_TO_SMALL_TH_NOISE_2, 5  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH2, SMALL_TO_BIG_TH_NOISE_3, 20 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH2, BIG_TO_SMALL_TH_NOISE_3, 5  )
	},

	{
		BCHP_VDEC_FE_0_HSYNC_ACC_GAIN1,
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN1, SMALL_NOISE_0, 74 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN1, BIG_NOISE_0,   21 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN1, SMALL_NOISE_1, 74 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN1, BIG_NOISE_1,   21 )
	},

	{
		BCHP_VDEC_FE_0_HSYNC_ACC_GAIN2,
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN2, SMALL_NOISE_2, 74 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN2, BIG_NOISE_2,   21 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN2, SMALL_NOISE_3, 74 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN2, BIG_NOISE_3,   21 )
	},

	{
		BCHP_VDEC_FE_0_CLEAN_VSYNC_CNT,
		BCHP_FIELD_DATA(VDEC_FE_0_CLEAN_VSYNC_CNT, NOISE_0, 36 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_CLEAN_VSYNC_CNT, NOISE_1, 36 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_CLEAN_VSYNC_CNT, NOISE_2, 36 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_CLEAN_VSYNC_CNT, NOISE_3, 36 )
	},

	{
		BCHP_VDEC_FE_0_NOISE_BIN_TH,
		BCHP_FIELD_DATA(VDEC_FE_0_NOISE_BIN_TH, HYSTERESIS, 5   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NOISE_BIN_TH, TH_2,       149 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NOISE_BIN_TH, TH_1,       75  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NOISE_BIN_TH, TH_0,       30  )
	},

	/* VDEC_FE_0_FE_STATUS0 (RO) */
	/* VDEC_FE_0_FE_STATUS (RO) */
	/* VDEC_FE_0_FE_FIELD_SIZE (RO) */
	/* VDEC_FE_0_FE_SAMPLE_COUNT (RO) */
	/* VDEC_FE_0_FE_SYNC_SLICE (RO) */
	/* VDEC_FE_0_FE_BLANK_LEVELS (RO) */
	/* VDEC_FE_0_FE_TIP_LEVELS (RO) */
	/* VDEC_FE_0_FE_AGC_LEVELS (RO) */

	/* Last Entry */
	{
		UINT32_C(-1),
		UINT32_C(-1)
	}
};

static const BVDC_P_AddrDataPair s_aVdecFmtTbl_Svideo_NTSC[] =
{
	/* VDEC_FE_0_FE_REV_ID (RO) */
	{
		BCHP_VDEC_FE_0_HSTART2,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_DATA(VDEC_FE_0_HSTART2, FILTER_COEF_SEL, 0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSTART2, WINDOW_SW_SELECT,    2 ) |
#if 	(BVDC_P_SUPPORT_VDEC_VER == BVDC_P_VDEC_VER_4)
		BCHP_FIELD_DATA(VDEC_FE_0_HSTART2, WINDOW_MODE,    0 ) |
#else
		BCHP_FIELD_DATA(VDEC_FE_0_HSTART2, WINDOW_MODE,    1 ) |
#endif
#endif
		BCHP_FIELD_DATA(VDEC_FE_0_HSTART2, STORE, 215 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSTART2, CS,    134 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSTART2, CB,    126 )
	},

	{
		BCHP_VDEC_FE_0_NB_CONTROL,
		BCHP_FIELD_DATA(VDEC_FE_0_NB_CONTROL, NOISE_LENGTH, 6   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NB_CONTROL, NOISE_START,  958 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NB_CONTROL, BLANK_LENGTH, 6   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NB_CONTROL, BLANK_START,  96  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NB_CONTROL, LINE,         8   )
	},

	{
		BCHP_VDEC_FE_0_AGC_REGION,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_5)
		BCHP_FIELD_ENUM(VDEC_FE_0_AGC_REGION, AGC_PATH_SEP,     SEPARATE ) |
		BCHP_FIELD_DATA(VDEC_FE_0_AGC_REGION, AGC_GAIN_LIMIT_VDEC,     0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_AGC_REGION, AGC_GAIN_LIMIT_SYNC_SEP, 0 ) |
#elif (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_ENUM(VDEC_FE_0_AGC_REGION, AGC_PATH_SEP,     SEPARATE ) |
		BCHP_FIELD_DATA(VDEC_FE_0_AGC_REGION, AGC_GAIN_LIMIT_VDEC,     4 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_AGC_REGION, AGC_GAIN_LIMIT_SYNC_SEP, 4 ) |
#endif
		BCHP_FIELD_DATA(VDEC_FE_0_AGC_REGION, END,   124                 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_AGC_REGION, START, 12                  )
	},

	{
		BCHP_VDEC_FE_0_HPLL_MISC,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_MISC, IFD_PATH_SEL,        VDEC ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_MISC, NOISE_MEASURE_SEL,   VDEC ) |
#endif
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_MISC, SLICE_LPF_HPLL_LOCK_CTL, ENABLE ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_MISC, DISABLE_SLICE_TILT,  1  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_MISC, VSYNC_OFFSET,        0  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_MISC, MATCH_FILT_TH_COEFF, 12 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_MISC, JUMP_END_LINE,       18 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_MISC, JUMP_START_LINE,     10 )
	},

	{
		BCHP_VDEC_FE_0_RAW_VSYNC_CONTROL,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_VSYNC_CONTROL, MAX_HEAD_SWITCH_HYSTERESIS_TH, 5 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_VSYNC_CONTROL, MIN_HEAD_SWITCH_HYSTERESIS_TH, 3 ) |
#endif
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_VSYNC_CONTROL, VPLL_LINE_UPDATE, 1  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_VSYNC_CONTROL, CLEAN_LINE_COUNT, 34 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_VSYNC_CONTROL, BLOCK_LINE_COUNT, 25 )
	},

	{
		BCHP_VDEC_FE_0_SYNC_DETECT,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, DCR_SEL,           SYNC_SEP      ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, MASK_EQ_SYNC,      1         ) |  /*PR44237: different setting with 3563*/
#else
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, HEAD_SWITCH_SEL,   FOUR_HEAD ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, MASK_EQ_SYNC,      0         ) |
#endif
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, TIP_FILT_SEL,      FILTA     ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, TIP_SELECT_MID,    0         ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, SLICE_CTL_DISABLE, 0         ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, SIGNAL_TO_MONITOR, AUTO      ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, SLICE_MODE,        AUTO      ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, SLICE_DELAY,       3         ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, SLICE_AVG,         AVG16     ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, P_COUNT,           20        ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, H_COUNT,           48        ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, V_COUNT,           68        )
	},

	{
		BCHP_VDEC_FE_0_SYNC_DETECT2,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, HEAD_SWITCH_HISTORY_MASK, 1 ) |
#else
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, MATCH_FILTER_TH,      1 ) |
#endif
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, MATCH_REQ_LOCK,       0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, VCR_MODE,             1 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, FREQ_OFFSET,          0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, CONSISTEN_ERROR_TH,   16) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, HEAD_SWITCH_ERROR_TH, 0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, MATCH_FILTER_MODE,    1 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, RAW_VSYNC_MODE,       0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, HEAD_SWITCH_THRESH,   6 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, LINE_OFFSET,          4 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, SLICE_FILT,           4 )
	},

	{
		BCHP_VDEC_FE_0_SYNC_BLANK,
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_BLANK, F1_END_LINE,   6  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_BLANK, F1_START_LINE, 61 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_BLANK, F0_END_LINE,   6  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_BLANK, F0_START_LINE, 61 )
	},

	{
		BCHP_VDEC_FE_0_RAW_HSYNC_CONTROL,
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, MATCH_OVERRIDE_MIN, 1  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, SAMPLE_TYPE_CTL,    0  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, MATCH_MIN,          32 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, INT_MIN,            45 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, INT_AREA,           1  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, MATCH_DELTA_COEFF,  12 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, DELTA_COEFF,        8  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, INT_TH_COEFF,       10 )
	},

	{
		BCHP_VDEC_FE_0_HSYNC_PLL1,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL1, FRO_FREQ,     FIXED  ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL1, AVG_EN,       FROZEN ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL1, FREQ_AVG,     ENABLE ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL1, FILTA_OFFSET, 21     ) |
#endif
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL1, BIG_SMALL_MODE, AUTO ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL1, DIRECT_GAIN,    4    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL1, SMALL_TERM_CNT, 0    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL1, BIG_TERM_CNT,   1    )
	},

	{
		BCHP_VDEC_FE_0_SYNC_ACCUM_CONTROL,
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_ACCUM_CONTROL, HSYNC_CLAMP_VALUE, 63 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_ACCUM_CONTROL, VSYNC_CLAMP_VALUE, 31 )
	},

	{
		BCHP_VDEC_FE_0_FRO_CONTROL,
		BCHP_FIELD_DATA(VDEC_FE_0_FRO_CONTROL, AVG_ERR_FACTOR, 2               ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_FRO_CONTROL, AUTO_COEFF,     ONE_AND_QUARTER ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FRO_CONTROL, FIELD_ID_TH,    20              ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FRO_CONTROL, CUT_OFF_TH,     31              ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FRO_CONTROL, ERR_TH,         20              ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FRO_CONTROL, FRO_MODE,       0               )
	},

	{
		BCHP_VDEC_FE_0_FIELD_SYNC,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_ENUM(VDEC_FE_0_FIELD_SYNC, FID_INVERT, NO_INVERT               ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_FIELD_SYNC, PC_INTERLACED_FID,  ENABLE          ) |
#endif
		BCHP_FIELD_DATA(VDEC_FE_0_FIELD_SYNC, ID_UPDATE_WINDOW, 0                   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FIELD_SYNC, MARKING_THRESH,  0                   ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_FIELD_SYNC, MARKING_WINDOW,  PLUS_MINUS_64       ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FIELD_SYNC, FIELD_SCOPE,     3                   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FIELD_SYNC, MODE_THRESHOLD,  2                   ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_FIELD_SYNC, TOP_ID,          SYNC_ON_SECOND_HALF ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_FIELD_SYNC, TYPE,            BY_POSITION         )
	},

	{
		BCHP_VDEC_FE_0_SYNC_SLAVE_HRANGE,
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_HRANGE, SYNC_END,   50   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_HRANGE, SYNC_START, 1998 )
	},

	{
		BCHP_VDEC_FE_0_SYNC_SLAVE_LEVEL,
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_LEVEL, BLANK_LENGTH, 6   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_LEVEL, BLANK_START,  160 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_LEVEL, TIP_LENGTH,   5   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_LEVEL, TIP_START,    40  )
	},

	{
		BCHP_VDEC_FE_0_SYNC_SLAVE_DIFF,
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_DIFF, MAX,        7  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_DIFF, LINE_RANGE, 8  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_DIFF, LINE_START, 10 )
	},

	{
		BCHP_VDEC_FE_0_CLEAN_VSYNC_CNT,
		BCHP_FIELD_DATA(VDEC_FE_0_CLEAN_VSYNC_CNT, NOISE_0, 36 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_CLEAN_VSYNC_CNT, NOISE_1, 36 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_CLEAN_VSYNC_CNT, NOISE_2, 36 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_CLEAN_VSYNC_CNT, NOISE_3, 36 )
	},

	/* VDEC_FE_0_FE_STATUS0 (RO) */
	/* VDEC_FE_0_FE_STATUS (RO) */
	/* VDEC_FE_0_FE_FIELD_SIZE (RO) */
	/* VDEC_FE_0_FE_SAMPLE_COUNT (RO) */
	/* VDEC_FE_0_FE_SYNC_SLICE (RO) */
	/* VDEC_FE_0_FE_BLANK_LEVELS (RO) */
	/* VDEC_FE_0_FE_TIP_LEVELS (RO) */
	/* VDEC_FE_0_FE_AGC_LEVELS (RO) */

	/* Last Entry */
	{
		UINT32_C(-1),
		UINT32_C(-1)
	}
};

static const BVDC_P_AddrDataPair s_aVdecFmtTbl_Svideo_PAL[] =
{
	/* VDEC_FE_0_FE_REV_ID (RO) */
	{
		BCHP_VDEC_FE_0_HSTART2,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_ENUM(VDEC_FE_0_HSTART2, FILTER_COEF_SEL, CENTER_FREQ_443361875) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSTART2, WINDOW_SW_SELECT,    2 ) |
#if 	(BVDC_P_SUPPORT_VDEC_VER == BVDC_P_VDEC_VER_4)
		BCHP_FIELD_DATA(VDEC_FE_0_HSTART2, WINDOW_MODE,    0 ) |
#else
		BCHP_FIELD_DATA(VDEC_FE_0_HSTART2, WINDOW_MODE,    1 ) |
#endif
#endif
		BCHP_FIELD_DATA(VDEC_FE_0_HSTART2, STORE, 215 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSTART2, CS,    134 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSTART2, CB,    126 )
	},

	{
		BCHP_VDEC_FE_0_NB_CONTROL,
		BCHP_FIELD_DATA(VDEC_FE_0_NB_CONTROL, NOISE_LENGTH, 6   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NB_CONTROL, NOISE_START,  958 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NB_CONTROL, BLANK_LENGTH, 6   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NB_CONTROL, BLANK_START,  96  ) |
#if (BVDC_P_3DCOMB_TTD_CONFLICT)
		BCHP_FIELD_DATA(VDEC_FE_0_NB_CONTROL, LINE,         4   )
#else
		BCHP_FIELD_DATA(VDEC_FE_0_NB_CONTROL, LINE,         5   )
#endif
	},

	{
		BCHP_VDEC_FE_0_AGC_REGION,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_5)
		BCHP_FIELD_ENUM(VDEC_FE_0_AGC_REGION, AGC_PATH_SEP,     SEPARATE ) |
		BCHP_FIELD_DATA(VDEC_FE_0_AGC_REGION, AGC_GAIN_LIMIT_VDEC,     0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_AGC_REGION, AGC_GAIN_LIMIT_SYNC_SEP, 0 ) |
#elif (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_ENUM(VDEC_FE_0_AGC_REGION, AGC_PATH_SEP,     SEPARATE ) |
		BCHP_FIELD_DATA(VDEC_FE_0_AGC_REGION, AGC_GAIN_LIMIT_VDEC,     4 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_AGC_REGION, AGC_GAIN_LIMIT_SYNC_SEP, 4 ) |
#endif
		BCHP_FIELD_DATA(VDEC_FE_0_AGC_REGION, END,   124                 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_AGC_REGION, START, 12                  )
	},

	{
		BCHP_VDEC_FE_0_HPLL_MISC,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_MISC, IFD_PATH_SEL,        VDEC ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_MISC, NOISE_MEASURE_SEL,   VDEC ) |
#endif
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_MISC, SLICE_LPF_HPLL_LOCK_CTL, ENABLE ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_MISC, DISABLE_SLICE_TILT,  1  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_MISC, VSYNC_OFFSET,        0  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_MISC, MATCH_FILT_TH_COEFF, 12 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_MISC, JUMP_END_LINE,       18 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_MISC, JUMP_START_LINE,     10 )
	},

	{
		BCHP_VDEC_FE_0_RAW_VSYNC_CONTROL,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_VSYNC_CONTROL, MAX_HEAD_SWITCH_HYSTERESIS_TH, 11 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_VSYNC_CONTROL, MIN_HEAD_SWITCH_HYSTERESIS_TH, 4 ) |
#endif
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_VSYNC_CONTROL, VPLL_LINE_UPDATE, 1  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_VSYNC_CONTROL, CLEAN_LINE_COUNT, 34 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_VSYNC_CONTROL, BLOCK_LINE_COUNT, 25 )
	},

	{
		BCHP_VDEC_FE_0_SYNC_DETECT,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, DCR_SEL,           VDEC      ) |
#else
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, HEAD_SWITCH_SEL,   FOUR_HEAD ) |
#endif
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, TIP_FILT_SEL,      FILTA     ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, TIP_SELECT_MID,    0         ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, SLICE_CTL_DISABLE, 0         ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, MASK_EQ_SYNC,      1         ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, SIGNAL_TO_MONITOR, AUTO      ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, SLICE_MODE,        AUTO      ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, SLICE_DELAY,       3         ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, SLICE_AVG,         AVG16     ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, P_COUNT,           12        ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, H_COUNT,           48        ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, V_COUNT,           68        )
	},

	{
		BCHP_VDEC_FE_0_SYNC_DETECT2,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, HEAD_SWITCH_HISTORY_MASK, 1 ) |
#else
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, MATCH_FILTER_TH,      0 ) |
#endif
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, MATCH_REQ_LOCK,       0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, VCR_MODE,             1 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, FREQ_OFFSET,          0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, CONSISTEN_ERROR_TH,   16 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, HEAD_SWITCH_ERROR_TH, 20 ) |
#if (BVDC_MACROVISION && BVDC_P_VDEC_PR32896_WORK_AROUND)
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, MATCH_FILTER_MODE,    0 ) |
#else
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, MATCH_FILTER_MODE,    1 ) |
#endif
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, RAW_VSYNC_MODE,       0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, HEAD_SWITCH_THRESH,   9  ) |
#if (BVDC_P_3DCOMB_TTD_CONFLICT)
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, LINE_OFFSET,          0 ) |
#else
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, LINE_OFFSET,          1 ) |
#endif
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, SLICE_FILT,           4 )
	},

	{
		BCHP_VDEC_FE_0_SYNC_BLANK,
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_BLANK, F1_END_LINE,   6  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_BLANK, F1_START_LINE, 61 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_BLANK, F0_END_LINE,   6  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_BLANK, F0_START_LINE, 61 )
	},

	{
		BCHP_VDEC_FE_0_RAW_HSYNC_CONTROL,
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, MATCH_OVERRIDE_MIN, 1  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, SAMPLE_TYPE_CTL,    0  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, MATCH_MIN,          32 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, INT_MIN,            45 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, INT_AREA,           1  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, MATCH_DELTA_COEFF,  12 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, DELTA_COEFF,        12 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, INT_TH_COEFF,       10 )
	},

	{
		BCHP_VDEC_FE_0_HSYNC_PLL1,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL1, FRO_FREQ,     FIXED  ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL1, AVG_EN,       UPDATED) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL1, FREQ_AVG,     DISABLE) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL1, FILTA_OFFSET, 21     ) |
#endif
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL1, BIG_SMALL_MODE, AUTO ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL1, DIRECT_GAIN,    4    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL1, SMALL_TERM_CNT, 0    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL1, BIG_TERM_CNT,   1    )
	},

	{
		BCHP_VDEC_FE_0_SYNC_ACCUM_CONTROL,
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_ACCUM_CONTROL, HSYNC_CLAMP_VALUE, 63 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_ACCUM_CONTROL, VSYNC_CLAMP_VALUE, 31 )
	},

	{
		BCHP_VDEC_FE_0_FRO_CONTROL,
		BCHP_FIELD_DATA(VDEC_FE_0_FRO_CONTROL, AVG_ERR_FACTOR, 2               ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_FRO_CONTROL, AUTO_COEFF,     ONE_AND_QUARTER ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FRO_CONTROL, FIELD_ID_TH,    20              ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FRO_CONTROL, CUT_OFF_TH,     31              ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FRO_CONTROL, ERR_TH,         20              ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FRO_CONTROL, FRO_MODE,       1               )
	},

	{
		BCHP_VDEC_FE_0_FIELD_SYNC,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_ENUM(VDEC_FE_0_FIELD_SYNC, FID_INVERT, NO_INVERT               ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_FIELD_SYNC, PC_INTERLACED_FID,  ENABLE          ) |
#endif
		BCHP_FIELD_DATA(VDEC_FE_0_FIELD_SYNC, ID_UPDATE_WINDOW, 0                   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FIELD_SYNC, MARKING_THRESH,  0                   ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_FIELD_SYNC, MARKING_WINDOW,  PLUS_MINUS_64       ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FIELD_SYNC, FIELD_SCOPE,     3                   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FIELD_SYNC, MODE_THRESHOLD,  2                   ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_FIELD_SYNC, TOP_ID,          SYNC_ON_SECOND_HALF ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_FIELD_SYNC, TYPE,            BY_POSITION         )
	},

	{
		BCHP_VDEC_FE_0_SYNC_SLAVE_HRANGE,
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_HRANGE, SYNC_END,   50   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_HRANGE, SYNC_START, 1998 )
	},

	{
		BCHP_VDEC_FE_0_SYNC_SLAVE_LEVEL,
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_LEVEL, BLANK_LENGTH, 6   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_LEVEL, BLANK_START,  160 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_LEVEL, TIP_LENGTH,   5   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_LEVEL, TIP_START,    40  )
	},

	{
		BCHP_VDEC_FE_0_SYNC_SLAVE_DIFF,
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_DIFF, MAX,        7  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_DIFF, LINE_RANGE, 8  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_DIFF, LINE_START, 10 )
	},

	{
		BCHP_VDEC_FE_0_CLEAN_VSYNC_CNT,
		BCHP_FIELD_DATA(VDEC_FE_0_CLEAN_VSYNC_CNT, NOISE_0, 36 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_CLEAN_VSYNC_CNT, NOISE_1, 36 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_CLEAN_VSYNC_CNT, NOISE_2, 36 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_CLEAN_VSYNC_CNT, NOISE_3, 36 )
	},

	/* VDEC_FE_0_FE_STATUS0 (RO) */
	/* VDEC_FE_0_FE_STATUS (RO) */
	/* VDEC_FE_0_FE_FIELD_SIZE (RO) */
	/* VDEC_FE_0_FE_SAMPLE_COUNT (RO) */
	/* VDEC_FE_0_FE_SYNC_SLICE (RO) */
	/* VDEC_FE_0_FE_BLANK_LEVELS (RO) */
	/* VDEC_FE_0_FE_TIP_LEVELS (RO) */
	/* VDEC_FE_0_FE_AGC_LEVELS (RO) */

	/* Last Entry */
	{
		UINT32_C(-1),
		UINT32_C(-1)
	}
};

static const BVDC_P_AddrDataPair s_aVdecFmtTbl_Svideo_PALM[] =
{
	{
		BCHP_VDEC_FE_0_HSTART2,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_DATA(VDEC_FE_0_HSTART2, FILTER_COEF_SEL, 0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSTART2, WINDOW_SW_SELECT,    2 ) |
#if 	(BVDC_P_SUPPORT_VDEC_VER == BVDC_P_VDEC_VER_4)
		BCHP_FIELD_DATA(VDEC_FE_0_HSTART2, WINDOW_MODE,    0 ) |
#else
		BCHP_FIELD_DATA(VDEC_FE_0_HSTART2, WINDOW_MODE,    1 ) |
#endif
#endif
		BCHP_FIELD_DATA(VDEC_FE_0_HSTART2, STORE, 215 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSTART2, CS,    134 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSTART2, CB,    126 )
	},

	{
		BCHP_VDEC_FE_0_NB_CONTROL,
		BCHP_FIELD_DATA(VDEC_FE_0_NB_CONTROL, NOISE_LENGTH, 6   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NB_CONTROL, NOISE_START,  1120 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NB_CONTROL, BLANK_LENGTH, 6   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NB_CONTROL, BLANK_START,  96  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NB_CONTROL, LINE,         9    )
	},

	{
		BCHP_VDEC_FE_0_AGC_REGION,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_5)
		BCHP_FIELD_ENUM(VDEC_FE_0_AGC_REGION, AGC_PATH_SEP,     SEPARATE ) |
		BCHP_FIELD_DATA(VDEC_FE_0_AGC_REGION, AGC_GAIN_LIMIT_VDEC,     0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_AGC_REGION, AGC_GAIN_LIMIT_SYNC_SEP, 0 ) |
#elif (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_ENUM(VDEC_FE_0_AGC_REGION, AGC_PATH_SEP,     SEPARATE ) |
		BCHP_FIELD_DATA(VDEC_FE_0_AGC_REGION, AGC_GAIN_LIMIT_VDEC,     4 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_AGC_REGION, AGC_GAIN_LIMIT_SYNC_SEP, 4 ) |
#endif
		BCHP_FIELD_DATA(VDEC_FE_0_AGC_REGION, END,   124                 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_AGC_REGION, START, 12                  )
	},

	{
		BCHP_VDEC_FE_0_HPLL_MISC,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_MISC, IFD_PATH_SEL,        VDEC ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_MISC, NOISE_MEASURE_SEL,   VDEC ) |
#endif
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_MISC, SLICE_LPF_HPLL_LOCK_CTL, DISABLE ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_MISC, DISABLE_SLICE_TILT,  1  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_MISC, VSYNC_OFFSET,        0  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_MISC, MATCH_FILT_TH_COEFF, 12 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_MISC, JUMP_END_LINE,       18 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_MISC, JUMP_START_LINE,     10 )
	},

	{
		BCHP_VDEC_FE_0_RAW_VSYNC_CONTROL,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_VSYNC_CONTROL, MAX_HEAD_SWITCH_HYSTERESIS_TH, 11 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_VSYNC_CONTROL, MIN_HEAD_SWITCH_HYSTERESIS_TH, 4 ) |
#endif
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_VSYNC_CONTROL, VPLL_LINE_UPDATE, 1  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_VSYNC_CONTROL, CLEAN_LINE_COUNT, 36 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_VSYNC_CONTROL, BLOCK_LINE_COUNT, 25 )
	},

	{
		BCHP_VDEC_FE_0_SYNC_DETECT,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, DCR_SEL,           VDEC      ) |
#else
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, HEAD_SWITCH_SEL,   TWO_HEAD) |
#endif
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, TIP_FILT_SEL,      FILTA     ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, TIP_SELECT_MID,    0         ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, SLICE_CTL_DISABLE, 0         ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, MASK_EQ_SYNC,      1         ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, SIGNAL_TO_MONITOR, AUTO      ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, SLICE_MODE,        AUTO      ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, SLICE_DELAY,       3         ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, SLICE_AVG,         AVG16     ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, P_COUNT,           24        ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, H_COUNT,           48        ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, V_COUNT,           68        )
	},

	{
		BCHP_VDEC_FE_0_SYNC_DETECT2,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, HEAD_SWITCH_HISTORY_MASK, 0 ) |
#else
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, MATCH_FILTER_TH,      0 ) |
#endif
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, MATCH_REQ_LOCK,       1  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, VCR_MODE,             1  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, FREQ_OFFSET,          32 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, CONSISTEN_ERROR_TH,   16 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, HEAD_SWITCH_ERROR_TH, 20 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, MATCH_FILTER_MODE,    1  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, RAW_VSYNC_MODE,       1  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, HEAD_SWITCH_THRESH,   9  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, LINE_OFFSET,          4  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, SLICE_FILT,           4  )
	},

	{
		BCHP_VDEC_FE_0_SYNC_BLANK,
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_BLANK, F1_END_LINE,   7  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_BLANK, F1_START_LINE, 61 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_BLANK, F0_END_LINE,   7  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_BLANK, F0_START_LINE, 61 )
	},

	{
		BCHP_VDEC_FE_0_RAW_HSYNC_CONTROL,
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, MATCH_OVERRIDE_MIN, 1  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, SAMPLE_TYPE_CTL,    0  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, MATCH_MIN,          32 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, INT_MIN,            45 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, INT_AREA,           1  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, MATCH_DELTA_COEFF,  8 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, DELTA_COEFF,        12 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, INT_TH_COEFF,       10 )
	},

	{
		BCHP_VDEC_FE_0_HSYNC_PLL1,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL1, FRO_FREQ,    FIXED   ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL1, AVG_EN,      UPDATED ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL1, FREQ_AVG,    DISABLE ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL1, FILTA_OFFSET, 21     ) |
#endif
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL1, BIG_SMALL_MODE, AUTO ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL1, DIRECT_GAIN,    4    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL1, SMALL_TERM_CNT, 0    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL1, BIG_TERM_CNT,   1    )
	},

	{
		BCHP_VDEC_FE_0_SYNC_ACCUM_CONTROL,
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_ACCUM_CONTROL, HSYNC_CLAMP_VALUE, 63 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_ACCUM_CONTROL, VSYNC_CLAMP_VALUE, 31 )
	},

	{
		BCHP_VDEC_FE_0_FRO_CONTROL,
		BCHP_FIELD_DATA(VDEC_FE_0_FRO_CONTROL, AVG_ERR_FACTOR, 2               ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_FRO_CONTROL, AUTO_COEFF,     ONE             ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FRO_CONTROL, FIELD_ID_TH,    20              ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FRO_CONTROL, CUT_OFF_TH,     31              ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FRO_CONTROL, ERR_TH,         20              ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FRO_CONTROL, FRO_MODE,       1               )
	},

	{
		BCHP_VDEC_FE_0_FIELD_SYNC,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_ENUM(VDEC_FE_0_FIELD_SYNC, FID_INVERT, NO_INVERT               ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_FIELD_SYNC, PC_INTERLACED_FID,  ENABLE          ) |
#endif
		BCHP_FIELD_DATA(VDEC_FE_0_FIELD_SYNC, ID_UPDATE_WINDOW, 0                   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FIELD_SYNC, MARKING_THRESH,  0                   ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_FIELD_SYNC, MARKING_WINDOW,  PLUS_MINUS_64       ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FIELD_SYNC, FIELD_SCOPE,     3                   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FIELD_SYNC, MODE_THRESHOLD,  2                   ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_FIELD_SYNC, TOP_ID,          SYNC_ON_SECOND_HALF ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_FIELD_SYNC, TYPE,            BY_POSITION         )
	},

	{
		BCHP_VDEC_FE_0_SYNC_SLAVE_HRANGE,
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_HRANGE, SYNC_END,   50   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_HRANGE, SYNC_START, 1998 )
	},

	{
		BCHP_VDEC_FE_0_SYNC_SLAVE_LEVEL,
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_LEVEL, BLANK_LENGTH, 6   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_LEVEL, BLANK_START,  160 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_LEVEL, TIP_LENGTH,   5   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_LEVEL, TIP_START,    40  )
	},

	{
		BCHP_VDEC_FE_0_SYNC_SLAVE_DIFF,
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_DIFF, MAX,        7  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_DIFF, LINE_RANGE, 8  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_DIFF, LINE_START, 10 )
	},

	{
		BCHP_VDEC_FE_0_CLEAN_VSYNC_CNT,
		BCHP_FIELD_DATA(VDEC_FE_0_CLEAN_VSYNC_CNT, NOISE_0, 36 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_CLEAN_VSYNC_CNT, NOISE_1, 36 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_CLEAN_VSYNC_CNT, NOISE_2, 36 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_CLEAN_VSYNC_CNT, NOISE_3, 36 )
	},

	/* VDEC_FE_0_FE_STATUS0 (RO) */
	/* VDEC_FE_0_FE_STATUS (RO) */
	/* VDEC_FE_0_FE_FIELD_SIZE (RO) */
	/* VDEC_FE_0_FE_SAMPLE_COUNT (RO) */
	/* VDEC_FE_0_FE_SYNC_SLICE (RO) */
	/* VDEC_FE_0_FE_BLANK_LEVELS (RO) */
	/* VDEC_FE_0_FE_TIP_LEVELS (RO) */
	/* VDEC_FE_0_FE_AGC_LEVELS (RO) */

	/* Last Entry */
	{
		UINT32_C(-1),
		UINT32_C(-1)
	}
};

static const BVDC_P_AddrDataPair s_aVdecFmtTbl_Rf_NTSC[] =
{
	/* VDEC_FE_0_FE_REV_ID (RO) */
	{
		BCHP_VDEC_FE_0_HSTART2,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_ENUM(VDEC_FE_0_HSTART2, FILTER_COEF_SEL,  CENTER_FREQ_3579545 ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HSTART2, WINDOW_SW_SELECT, C_WINDOW            ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HSTART2, WINDOW_MODE,      MANUAL              ) |  /* toggle to -> AUTO after locked */
#endif
		BCHP_FIELD_DATA(VDEC_FE_0_HSTART2, STORE, 215 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSTART2, CS,    134 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSTART2, CB,    126 )
	},

	{
		BCHP_VDEC_FE_0_NB_CONTROL,
		BCHP_FIELD_DATA(VDEC_FE_0_NB_CONTROL, NOISE_LENGTH, 6    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NB_CONTROL, NOISE_START,  1120 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NB_CONTROL, BLANK_LENGTH, 6    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NB_CONTROL, BLANK_START,  96   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NB_CONTROL, LINE,         9    )
	},

	{
		BCHP_VDEC_FE_0_AGC_REGION,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_5)
		BCHP_FIELD_ENUM(VDEC_FE_0_AGC_REGION, AGC_PATH_SEP,     SEPARATE ) |
		BCHP_FIELD_DATA(VDEC_FE_0_AGC_REGION, AGC_GAIN_LIMIT_VDEC,     0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_AGC_REGION, AGC_GAIN_LIMIT_SYNC_SEP, 0 ) |
#elif (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_ENUM(VDEC_FE_0_AGC_REGION, AGC_PATH_SEP,     SEPARATE ) |
		BCHP_FIELD_DATA(VDEC_FE_0_AGC_REGION, AGC_GAIN_LIMIT_VDEC,     4 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_AGC_REGION, AGC_GAIN_LIMIT_SYNC_SEP, 4 ) |
#endif
		BCHP_FIELD_DATA(VDEC_FE_0_AGC_REGION, END,   124                 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_AGC_REGION, START, 12                  )
	},

	{
		BCHP_VDEC_FE_0_HPLL_MISC,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_MISC, IFD_PATH_SEL,        VDEC ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_MISC, NOISE_MEASURE_SEL,   VDEC ) |
#endif
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_MISC, SLICE_LPF_HPLL_LOCK_CTL, ENABLE ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_MISC, DISABLE_SLICE_TILT,  1  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_MISC, VSYNC_OFFSET,        0  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_MISC, MATCH_FILT_TH_COEFF, 12 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_MISC, JUMP_END_LINE,       31 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_MISC, JUMP_START_LINE,     10 )
	},

	{
		BCHP_VDEC_FE_0_RAW_VSYNC_CONTROL,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_VSYNC_CONTROL, MAX_HEAD_SWITCH_HYSTERESIS_TH, 5 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_VSYNC_CONTROL, MIN_HEAD_SWITCH_HYSTERESIS_TH, 3 ) |
#endif
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_VSYNC_CONTROL, VPLL_LINE_UPDATE, 1  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_VSYNC_CONTROL, CLEAN_LINE_COUNT, 34 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_VSYNC_CONTROL, BLOCK_LINE_COUNT, 25 )
	},

	{
		BCHP_VDEC_FE_0_SYNC_DETECT,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, DCR_SEL,           SYNC_SEP      ) |
#else
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, HEAD_SWITCH_SEL,   TWO_HEAD) |
#endif
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, TIP_FILT_SEL,      FILTA    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, TIP_SELECT_MID,    0        ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, SLICE_CTL_DISABLE, 0        ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, MASK_EQ_SYNC,      1        ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, SIGNAL_TO_MONITOR, FILTA    ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, SLICE_MODE,        AUTO     ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, SLICE_DELAY,       3        ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, SLICE_AVG,         AVG16    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, P_COUNT,           20       ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, H_COUNT,           48       ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, V_COUNT,           68       )
	},

	{
		BCHP_VDEC_FE_0_SYNC_DETECT2,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, HEAD_SWITCH_HISTORY_MASK, 1 ) |
#else
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, MATCH_FILTER_TH,      0 ) |
#endif
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, MATCH_REQ_LOCK,       1  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, VCR_MODE,             1  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, FREQ_OFFSET,          32 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, CONSISTEN_ERROR_TH,   10 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, HEAD_SWITCH_ERROR_TH, 20 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, MATCH_FILTER_MODE,    1  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, RAW_VSYNC_MODE,       1  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, HEAD_SWITCH_THRESH,   9  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, LINE_OFFSET,          4  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, SLICE_FILT,           4  )
	},

	{
		BCHP_VDEC_FE_0_SYNC_BLANK,
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_BLANK, F1_END_LINE,   7  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_BLANK, F1_START_LINE, 61 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_BLANK, F0_END_LINE,   7  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_BLANK, F0_START_LINE, 61 )
	},

	{
		BCHP_VDEC_FE_0_RAW_HSYNC_CONTROL,
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, MATCH_OVERRIDE_MIN, 1  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, SAMPLE_TYPE_CTL,    0  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, MATCH_MIN,          32 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, INT_MIN,            45 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, INT_AREA,           0  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, MATCH_DELTA_COEFF,  12 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, DELTA_COEFF,        8  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, INT_TH_COEFF,       10 )
	},

	{
		BCHP_VDEC_FE_0_HSYNC_PLL1,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL1, FRO_FREQ,     FIXED  ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL1, AVG_EN,       UPDATED) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL1, FREQ_AVG,     ENABLE ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL1, FILTA_OFFSET, 21     ) |
#endif
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL1, BIG_SMALL_MODE, AUTO ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL1, DIRECT_GAIN,    4    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL1, SMALL_TERM_CNT, 0    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL1, BIG_TERM_CNT,   1    )
	},

	{
		BCHP_VDEC_FE_0_SYNC_ACCUM_CONTROL,
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_ACCUM_CONTROL, HSYNC_CLAMP_VALUE, 63 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_ACCUM_CONTROL, VSYNC_CLAMP_VALUE, 31 )
	},

	{
		BCHP_VDEC_FE_0_FRO_CONTROL,
		BCHP_FIELD_DATA(VDEC_FE_0_FRO_CONTROL, AVG_ERR_FACTOR, 2               ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_FRO_CONTROL, AUTO_COEFF,     ONE_AND_QUARTER ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FRO_CONTROL, FIELD_ID_TH,    20              ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FRO_CONTROL, CUT_OFF_TH,     31              ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FRO_CONTROL, ERR_TH,         20              ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FRO_CONTROL, FRO_MODE,       0               )
	},

	{
		BCHP_VDEC_FE_0_FIELD_SYNC,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_ENUM(VDEC_FE_0_FIELD_SYNC, FID_INVERT, NO_INVERT               ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_FIELD_SYNC, PC_INTERLACED_FID,  ENABLE          ) |
#endif
		BCHP_FIELD_DATA(VDEC_FE_0_FIELD_SYNC, ID_UPDATE_WINDOW, 0                   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FIELD_SYNC, MARKING_THRESH,  0                   ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_FIELD_SYNC, MARKING_WINDOW,  PLUS_MINUS_64       ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FIELD_SYNC, FIELD_SCOPE,     3                   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FIELD_SYNC, MODE_THRESHOLD,  2                   ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_FIELD_SYNC, TOP_ID,          SYNC_ON_SECOND_HALF ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_FIELD_SYNC, TYPE,            BY_POSITION         )
	},

	{
		BCHP_VDEC_FE_0_SYNC_SLAVE_HRANGE,
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_HRANGE, SYNC_END,   50   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_HRANGE, SYNC_START, 1998 )
	},

	{
		BCHP_VDEC_FE_0_SYNC_SLAVE_LEVEL,
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_LEVEL, BLANK_LENGTH, 6   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_LEVEL, BLANK_START,  160 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_LEVEL, TIP_LENGTH,   5   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_LEVEL, TIP_START,    40  )
	},

	{
		BCHP_VDEC_FE_0_SYNC_SLAVE_DIFF,
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_DIFF, MAX,        7  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_DIFF, LINE_RANGE, 8  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_DIFF, LINE_START, 10 )
	},

	{
		BCHP_VDEC_FE_0_CLEAN_VSYNC_CNT,
		BCHP_FIELD_DATA(VDEC_FE_0_CLEAN_VSYNC_CNT, NOISE_0, 0x7f ) |
		BCHP_FIELD_DATA(VDEC_FE_0_CLEAN_VSYNC_CNT, NOISE_1, 0x7f ) |
		BCHP_FIELD_DATA(VDEC_FE_0_CLEAN_VSYNC_CNT, NOISE_2, 0x7f ) |
		BCHP_FIELD_DATA(VDEC_FE_0_CLEAN_VSYNC_CNT, NOISE_3, 0x7f )
	},

	/* VDEC_FE_0_FE_STATUS0 (RO) */
	/* VDEC_FE_0_FE_STATUS (RO) */
	/* VDEC_FE_0_FE_FIELD_SIZE (RO) */
	/* VDEC_FE_0_FE_SAMPLE_COUNT (RO) */
	/* VDEC_FE_0_FE_SYNC_SLICE (RO) */
	/* VDEC_FE_0_FE_BLANK_LEVELS (RO) */
	/* VDEC_FE_0_FE_TIP_LEVELS (RO) */
	/* VDEC_FE_0_FE_AGC_LEVELS (RO) */

	/* Last Entry */
	{
		UINT32_C(-1),
		UINT32_C(-1)
	}
};

static const BVDC_P_AddrDataPair s_aVdecFmtTbl_Cvbs_NTSC[] =
{
	/* VDEC_FE_0_FE_REV_ID (RO) */
	{
		BCHP_VDEC_FE_0_HSTART2,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_ENUM(VDEC_FE_0_HSTART2, FILTER_COEF_SEL,  CENTER_FREQ_3579545 ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HSTART2, WINDOW_SW_SELECT, C_WINDOW            ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HSTART2, WINDOW_MODE,      MANUAL              ) |  /* toggle to -> AUTO after locked */
#endif
		BCHP_FIELD_DATA(VDEC_FE_0_HSTART2, STORE, 215 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSTART2, CS,    134 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSTART2, CB,    126 )
	},

	{
		BCHP_VDEC_FE_0_NB_CONTROL,
		BCHP_FIELD_DATA(VDEC_FE_0_NB_CONTROL, NOISE_LENGTH, 6    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NB_CONTROL, NOISE_START,  1120 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NB_CONTROL, BLANK_LENGTH, 6    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NB_CONTROL, BLANK_START,  96   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NB_CONTROL, LINE,         9    )
	},

	{
		BCHP_VDEC_FE_0_AGC_REGION,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_5)
		BCHP_FIELD_ENUM(VDEC_FE_0_AGC_REGION, AGC_PATH_SEP,     SEPARATE ) |
		BCHP_FIELD_DATA(VDEC_FE_0_AGC_REGION, AGC_GAIN_LIMIT_VDEC,     0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_AGC_REGION, AGC_GAIN_LIMIT_SYNC_SEP, 0 ) |
#elif (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_ENUM(VDEC_FE_0_AGC_REGION, AGC_PATH_SEP,     SEPARATE ) |
		BCHP_FIELD_DATA(VDEC_FE_0_AGC_REGION, AGC_GAIN_LIMIT_VDEC,     4 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_AGC_REGION, AGC_GAIN_LIMIT_SYNC_SEP, 4 ) |
#endif
		BCHP_FIELD_DATA(VDEC_FE_0_AGC_REGION, END,   124                 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_AGC_REGION, START, 12                  )
	},

	{
		BCHP_VDEC_FE_0_HPLL_MISC,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_MISC, IFD_PATH_SEL,        VDEC ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_MISC, NOISE_MEASURE_SEL,   VDEC ) |
#endif
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_MISC, SLICE_LPF_HPLL_LOCK_CTL, ENABLE ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_MISC, DISABLE_SLICE_TILT,  1  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_MISC, VSYNC_OFFSET,        0  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_MISC, MATCH_FILT_TH_COEFF, 12 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_MISC, JUMP_END_LINE,       18 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_MISC, JUMP_START_LINE,     10 )
	},

	{
		BCHP_VDEC_FE_0_RAW_VSYNC_CONTROL,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_VSYNC_CONTROL, MAX_HEAD_SWITCH_HYSTERESIS_TH, 5 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_VSYNC_CONTROL, MIN_HEAD_SWITCH_HYSTERESIS_TH, 3 ) |
#endif
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_VSYNC_CONTROL, VPLL_LINE_UPDATE, 1  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_VSYNC_CONTROL, CLEAN_LINE_COUNT, 36 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_VSYNC_CONTROL, BLOCK_LINE_COUNT, 25 )
	},

	{
		BCHP_VDEC_FE_0_SYNC_DETECT,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, DCR_SEL,           SYNC_SEP      ) |
#else
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, HEAD_SWITCH_SEL,   TWO_HEAD) |
#endif
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, TIP_FILT_SEL,      FILTA    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, TIP_SELECT_MID,    0        ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, SLICE_CTL_DISABLE, 0        ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, MASK_EQ_SYNC,      1        ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, SIGNAL_TO_MONITOR, AUTO     ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, SLICE_MODE,        AUTO     ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, SLICE_DELAY,       3        ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, SLICE_AVG,         AVG16    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, P_COUNT,           20       ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, H_COUNT,           48       ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, V_COUNT,           68       )
	},

	{
		BCHP_VDEC_FE_0_SYNC_DETECT2,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, HEAD_SWITCH_HISTORY_MASK, 1 ) |
#else
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, MATCH_FILTER_TH,      0 ) |
#endif
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, MATCH_REQ_LOCK,       1  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, VCR_MODE,             1  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, FREQ_OFFSET,          32 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, CONSISTEN_ERROR_TH,   16 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, HEAD_SWITCH_ERROR_TH, 20 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, MATCH_FILTER_MODE,    1  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, RAW_VSYNC_MODE,       1  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, HEAD_SWITCH_THRESH,   9  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, LINE_OFFSET,          4  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, SLICE_FILT,           4  )
	},

	{
		BCHP_VDEC_FE_0_SYNC_BLANK,
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_BLANK, F1_END_LINE,   7  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_BLANK, F1_START_LINE, 61 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_BLANK, F0_END_LINE,   7  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_BLANK, F0_START_LINE, 61 )
	},

	{
		BCHP_VDEC_FE_0_RAW_HSYNC_CONTROL,
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, MATCH_OVERRIDE_MIN, 1  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, SAMPLE_TYPE_CTL,    0  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, MATCH_MIN,          32 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, INT_MIN,            45 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, INT_AREA,           1  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, MATCH_DELTA_COEFF,  12 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, DELTA_COEFF,        8  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, INT_TH_COEFF,       10 )
	},

	{
		BCHP_VDEC_FE_0_HSYNC_PLL1,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL1, FRO_FREQ,     FIXED  ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL1, AVG_EN,       UPDATED) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL1, FREQ_AVG,     ENABLE ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL1, FILTA_OFFSET, 21     ) |
#endif
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL1, BIG_SMALL_MODE, AUTO ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL1, DIRECT_GAIN,    4    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL1, SMALL_TERM_CNT, 0    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL1, BIG_TERM_CNT,   1    )
	},

	{
		BCHP_VDEC_FE_0_SYNC_ACCUM_CONTROL,
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_ACCUM_CONTROL, HSYNC_CLAMP_VALUE, 63 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_ACCUM_CONTROL, VSYNC_CLAMP_VALUE, 31 )
	},

	{
		BCHP_VDEC_FE_0_FRO_CONTROL,
		BCHP_FIELD_DATA(VDEC_FE_0_FRO_CONTROL, AVG_ERR_FACTOR, 2               ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_FRO_CONTROL, AUTO_COEFF,     ONE_AND_QUARTER ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FRO_CONTROL, FIELD_ID_TH,    20              ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FRO_CONTROL, CUT_OFF_TH,     31              ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FRO_CONTROL, ERR_TH,         20              ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FRO_CONTROL, FRO_MODE,       0               )
	},

	{
		BCHP_VDEC_FE_0_FIELD_SYNC,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_ENUM(VDEC_FE_0_FIELD_SYNC, FID_INVERT, NO_INVERT               ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_FIELD_SYNC, PC_INTERLACED_FID,  ENABLE          ) |
#endif
		BCHP_FIELD_DATA(VDEC_FE_0_FIELD_SYNC, ID_UPDATE_WINDOW, 0                   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FIELD_SYNC, MARKING_THRESH,  0                   ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_FIELD_SYNC, MARKING_WINDOW,  PLUS_MINUS_64       ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FIELD_SYNC, FIELD_SCOPE,     3                   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FIELD_SYNC, MODE_THRESHOLD,  2                   ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_FIELD_SYNC, TOP_ID,          SYNC_ON_SECOND_HALF ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_FIELD_SYNC, TYPE,            BY_POSITION         )
	},

	{
		BCHP_VDEC_FE_0_SYNC_SLAVE_HRANGE,
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_HRANGE, SYNC_END,   50   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_HRANGE, SYNC_START, 1998 )
	},

	{
		BCHP_VDEC_FE_0_SYNC_SLAVE_LEVEL,
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_LEVEL, BLANK_LENGTH, 6   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_LEVEL, BLANK_START,  160 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_LEVEL, TIP_LENGTH,   5   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_LEVEL, TIP_START,    40  )
	},

	{
		BCHP_VDEC_FE_0_SYNC_SLAVE_DIFF,
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_DIFF, MAX,        7  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_DIFF, LINE_RANGE, 8  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_DIFF, LINE_START, 10 )
	},

	{
		BCHP_VDEC_FE_0_CLEAN_VSYNC_CNT,
		BCHP_FIELD_DATA(VDEC_FE_0_CLEAN_VSYNC_CNT, NOISE_0, 0x7f ) |
		BCHP_FIELD_DATA(VDEC_FE_0_CLEAN_VSYNC_CNT, NOISE_1, 0x7f ) |
		BCHP_FIELD_DATA(VDEC_FE_0_CLEAN_VSYNC_CNT, NOISE_2, 0x7f ) |
		BCHP_FIELD_DATA(VDEC_FE_0_CLEAN_VSYNC_CNT, NOISE_3, 0x7f )
	},

	/* VDEC_FE_0_FE_STATUS0 (RO) */
	/* VDEC_FE_0_FE_STATUS (RO) */
	/* VDEC_FE_0_FE_FIELD_SIZE (RO) */
	/* VDEC_FE_0_FE_SAMPLE_COUNT (RO) */
	/* VDEC_FE_0_FE_SYNC_SLICE (RO) */
	/* VDEC_FE_0_FE_BLANK_LEVELS (RO) */
	/* VDEC_FE_0_FE_TIP_LEVELS (RO) */
	/* VDEC_FE_0_FE_AGC_LEVELS (RO) */

	/* Last Entry */
	{
		UINT32_C(-1),
		UINT32_C(-1)
	}
};

static const BVDC_P_AddrDataPair s_aVdecFmtTbl_Rf_PAL[] =
{
	/* VDEC_FE_0_FE_REV_ID (RO) */
	{
		BCHP_VDEC_FE_0_HSTART2,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_ENUM(VDEC_FE_0_HSTART2, FILTER_COEF_SEL, CENTER_FREQ_443361875) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSTART2, WINDOW_SW_SELECT,    2 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSTART2, WINDOW_MODE,    1 ) |
#endif
		BCHP_FIELD_DATA(VDEC_FE_0_HSTART2, STORE, 215 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSTART2, CS,    134 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSTART2, CB,    126 )
	},

	{
		BCHP_VDEC_FE_0_NB_CONTROL,
		BCHP_FIELD_DATA(VDEC_FE_0_NB_CONTROL, NOISE_LENGTH, 6   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NB_CONTROL, NOISE_START,  958 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NB_CONTROL, BLANK_LENGTH, 6   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NB_CONTROL, BLANK_START,  96  ) |
#if (BVDC_P_3DCOMB_TTD_CONFLICT)
		BCHP_FIELD_DATA(VDEC_FE_0_NB_CONTROL, LINE,         4   )
#else
		BCHP_FIELD_DATA(VDEC_FE_0_NB_CONTROL, LINE,         5   )
#endif
	},

	{
		BCHP_VDEC_FE_0_AGC_REGION,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_5)
		BCHP_FIELD_ENUM(VDEC_FE_0_AGC_REGION, AGC_PATH_SEP,     SEPARATE ) |
		BCHP_FIELD_DATA(VDEC_FE_0_AGC_REGION, AGC_GAIN_LIMIT_VDEC,     0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_AGC_REGION, AGC_GAIN_LIMIT_SYNC_SEP, 0 ) |
#elif (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_ENUM(VDEC_FE_0_AGC_REGION, AGC_PATH_SEP,     SEPARATE ) |
		BCHP_FIELD_DATA(VDEC_FE_0_AGC_REGION, AGC_GAIN_LIMIT_VDEC,     4 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_AGC_REGION, AGC_GAIN_LIMIT_SYNC_SEP, 4 ) |
#endif
		BCHP_FIELD_DATA(VDEC_FE_0_AGC_REGION, END,   124                 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_AGC_REGION, START, 12                  )
	},

	{
		BCHP_VDEC_FE_0_HPLL_MISC,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_MISC, IFD_PATH_SEL,        VDEC ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_MISC, NOISE_MEASURE_SEL,   VDEC ) |
#endif
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_MISC, SLICE_LPF_HPLL_LOCK_CTL, ENABLE ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_MISC, DISABLE_SLICE_TILT,  1  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_MISC, VSYNC_OFFSET,        0  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_MISC, MATCH_FILT_TH_COEFF, 12 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_MISC, JUMP_END_LINE,       18 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_MISC, JUMP_START_LINE,     10 )
	},

	{
		BCHP_VDEC_FE_0_RAW_VSYNC_CONTROL,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_VSYNC_CONTROL, MAX_HEAD_SWITCH_HYSTERESIS_TH, 11 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_VSYNC_CONTROL, MIN_HEAD_SWITCH_HYSTERESIS_TH, 4 ) |
#endif
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_VSYNC_CONTROL, VPLL_LINE_UPDATE, 1  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_VSYNC_CONTROL, CLEAN_LINE_COUNT, 34 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_VSYNC_CONTROL, BLOCK_LINE_COUNT, 25 )
	},

	{
		BCHP_VDEC_FE_0_SYNC_DETECT,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, DCR_SEL,           VDEC      ) |
#else
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, HEAD_SWITCH_SEL,   FOUR_HEAD ) |
#endif
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, TIP_FILT_SEL,      FILTA     ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, TIP_SELECT_MID,    0         ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, SLICE_CTL_DISABLE, 0         ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, MASK_EQ_SYNC,      1         ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, SIGNAL_TO_MONITOR, FILTA     ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, SLICE_MODE,        AUTO      ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, SLICE_DELAY,       3         ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, SLICE_AVG,         AVG16     ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, P_COUNT,           12        ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, H_COUNT,           48        ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, V_COUNT,           68        )
	},

	{
		BCHP_VDEC_FE_0_SYNC_DETECT2,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, HEAD_SWITCH_HISTORY_MASK, 0 ) |
#else
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, MATCH_FILTER_TH,      0 ) |
#endif
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, MATCH_REQ_LOCK,       1 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, VCR_MODE,             1 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, FREQ_OFFSET,          0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, CONSISTEN_ERROR_TH,   16 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, HEAD_SWITCH_ERROR_TH, 20 ) |
#if (BVDC_MACROVISION && BVDC_P_VDEC_PR32896_WORK_AROUND)
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, MATCH_FILTER_MODE,    0 ) |
#else
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, MATCH_FILTER_MODE,    1 ) |
#endif
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, RAW_VSYNC_MODE,       0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, HEAD_SWITCH_THRESH,   9  ) |
#if (BVDC_P_3DCOMB_TTD_CONFLICT)
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, LINE_OFFSET,          0 ) |
#else
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, LINE_OFFSET,          1 ) |
#endif
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, SLICE_FILT,           4 )
	},

	{
		BCHP_VDEC_FE_0_SYNC_BLANK,
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_BLANK, F1_END_LINE,   6  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_BLANK, F1_START_LINE, 61 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_BLANK, F0_END_LINE,   6  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_BLANK, F0_START_LINE, 61 )
	},

	{
		BCHP_VDEC_FE_0_RAW_HSYNC_CONTROL,
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, MATCH_OVERRIDE_MIN, 1  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, SAMPLE_TYPE_CTL,    0  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, MATCH_MIN,          32 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, INT_MIN,            45 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, INT_AREA,           0  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, MATCH_DELTA_COEFF,  12 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, DELTA_COEFF,        8  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, INT_TH_COEFF,       10 )
	},

	{
		BCHP_VDEC_FE_0_HSYNC_PLL1,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL1, FRO_FREQ,     FIXED  ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL1, AVG_EN,       UPDATED) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL1, FREQ_AVG,     ENABLE) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL1, FILTA_OFFSET, 21     ) |
#endif
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL1, BIG_SMALL_MODE, AUTO ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL1, DIRECT_GAIN,    4    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL1, SMALL_TERM_CNT, 0    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL1, BIG_TERM_CNT,   1    )
	},

	{
		BCHP_VDEC_FE_0_SYNC_ACCUM_CONTROL,
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_ACCUM_CONTROL, HSYNC_CLAMP_VALUE, 63 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_ACCUM_CONTROL, VSYNC_CLAMP_VALUE, 31 )
	},

	{
		BCHP_VDEC_FE_0_FRO_CONTROL,
		BCHP_FIELD_DATA(VDEC_FE_0_FRO_CONTROL, AVG_ERR_FACTOR, 2               ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_FRO_CONTROL, AUTO_COEFF,     ONE_AND_QUARTER ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FRO_CONTROL, FIELD_ID_TH,    20              ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FRO_CONTROL, CUT_OFF_TH,     31              ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FRO_CONTROL, ERR_TH,         20              ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FRO_CONTROL, FRO_MODE,       1               )
	},

	{
		BCHP_VDEC_FE_0_FIELD_SYNC,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_ENUM(VDEC_FE_0_FIELD_SYNC, FID_INVERT, NO_INVERT               ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_FIELD_SYNC, PC_INTERLACED_FID,  ENABLE          ) |
#endif
		BCHP_FIELD_DATA(VDEC_FE_0_FIELD_SYNC, ID_UPDATE_WINDOW, 0                   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FIELD_SYNC, MARKING_THRESH,  0                   ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_FIELD_SYNC, MARKING_WINDOW,  PLUS_MINUS_64       ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FIELD_SYNC, FIELD_SCOPE,     3                   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FIELD_SYNC, MODE_THRESHOLD,  2                   ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_FIELD_SYNC, TOP_ID,          SYNC_ON_SECOND_HALF ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_FIELD_SYNC, TYPE,            BY_POSITION         )
	},

	{
		BCHP_VDEC_FE_0_SYNC_SLAVE_HRANGE,
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_HRANGE, SYNC_END,   50   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_HRANGE, SYNC_START, 1998 )
	},

	{
		BCHP_VDEC_FE_0_SYNC_SLAVE_LEVEL,
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_LEVEL, BLANK_LENGTH, 6   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_LEVEL, BLANK_START,  160 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_LEVEL, TIP_LENGTH,   5   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_LEVEL, TIP_START,    40  )
	},

	{
		BCHP_VDEC_FE_0_SYNC_SLAVE_DIFF,
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_DIFF, MAX,        7  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_DIFF, LINE_RANGE, 8  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_DIFF, LINE_START, 10 )
	},

	{
		BCHP_VDEC_FE_0_CLEAN_VSYNC_CNT,
		BCHP_FIELD_DATA(VDEC_FE_0_CLEAN_VSYNC_CNT, NOISE_0, 36 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_CLEAN_VSYNC_CNT, NOISE_1, 36 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_CLEAN_VSYNC_CNT, NOISE_2, 36 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_CLEAN_VSYNC_CNT, NOISE_3, 36 )
	},

	/* VDEC_FE_0_FE_STATUS0 (RO) */
	/* VDEC_FE_0_FE_STATUS (RO) */
	/* VDEC_FE_0_FE_FIELD_SIZE (RO) */
	/* VDEC_FE_0_FE_SAMPLE_COUNT (RO) */
	/* VDEC_FE_0_FE_SYNC_SLICE (RO) */
	/* VDEC_FE_0_FE_BLANK_LEVELS (RO) */
	/* VDEC_FE_0_FE_TIP_LEVELS (RO) */
	/* VDEC_FE_0_FE_AGC_LEVELS (RO) */

	/* Last Entry */
	{
		UINT32_C(-1),
		UINT32_C(-1)
	}
};

static const BVDC_P_AddrDataPair s_aVdecFmtTbl_Cvbs_PAL[] =
{
	/* VDEC_FE_0_FE_REV_ID (RO) */
	{
		BCHP_VDEC_FE_0_HSTART2,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_ENUM(VDEC_FE_0_HSTART2, FILTER_COEF_SEL, CENTER_FREQ_443361875) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSTART2, WINDOW_SW_SELECT,    2 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSTART2, WINDOW_MODE,    1 ) |
#endif
		BCHP_FIELD_DATA(VDEC_FE_0_HSTART2, STORE, 215 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSTART2, CS,    134 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSTART2, CB,    126 )
	},

	{
		BCHP_VDEC_FE_0_NB_CONTROL,
		BCHP_FIELD_DATA(VDEC_FE_0_NB_CONTROL, NOISE_LENGTH, 6   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NB_CONTROL, NOISE_START,  958 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NB_CONTROL, BLANK_LENGTH, 6   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NB_CONTROL, BLANK_START,  96  ) |
#if (BVDC_P_3DCOMB_TTD_CONFLICT)
		BCHP_FIELD_DATA(VDEC_FE_0_NB_CONTROL, LINE,         4   )
#else
		BCHP_FIELD_DATA(VDEC_FE_0_NB_CONTROL, LINE,         5   )
#endif
	},

	{
		BCHP_VDEC_FE_0_AGC_REGION,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_5)
		BCHP_FIELD_ENUM(VDEC_FE_0_AGC_REGION, AGC_PATH_SEP,     SEPARATE ) |
		BCHP_FIELD_DATA(VDEC_FE_0_AGC_REGION, AGC_GAIN_LIMIT_VDEC,     0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_AGC_REGION, AGC_GAIN_LIMIT_SYNC_SEP, 0 ) |
#elif (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_ENUM(VDEC_FE_0_AGC_REGION, AGC_PATH_SEP,     SEPARATE ) |
		BCHP_FIELD_DATA(VDEC_FE_0_AGC_REGION, AGC_GAIN_LIMIT_VDEC,     4 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_AGC_REGION, AGC_GAIN_LIMIT_SYNC_SEP, 4 ) |
#endif
		BCHP_FIELD_DATA(VDEC_FE_0_AGC_REGION, END,   124                 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_AGC_REGION, START, 12                  )
	},

	{
		BCHP_VDEC_FE_0_HPLL_MISC,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_MISC, IFD_PATH_SEL,        VDEC ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_MISC, NOISE_MEASURE_SEL,   VDEC ) |
#endif
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_MISC, SLICE_LPF_HPLL_LOCK_CTL, ENABLE ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_MISC, DISABLE_SLICE_TILT,  1  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_MISC, VSYNC_OFFSET,        0  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_MISC, MATCH_FILT_TH_COEFF, 12 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_MISC, JUMP_END_LINE,       18 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_MISC, JUMP_START_LINE,     10 )
	},

	{
		BCHP_VDEC_FE_0_RAW_VSYNC_CONTROL,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_VSYNC_CONTROL, MAX_HEAD_SWITCH_HYSTERESIS_TH, 11 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_VSYNC_CONTROL, MIN_HEAD_SWITCH_HYSTERESIS_TH, 4 ) |
#endif
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_VSYNC_CONTROL, VPLL_LINE_UPDATE, 1  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_VSYNC_CONTROL, CLEAN_LINE_COUNT, 34 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_VSYNC_CONTROL, BLOCK_LINE_COUNT, 25 )
	},

	{
		BCHP_VDEC_FE_0_SYNC_DETECT,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, DCR_SEL,           VDEC      ) |
#else
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, HEAD_SWITCH_SEL,   FOUR_HEAD ) |
#endif
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, TIP_FILT_SEL,      FILTA     ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, TIP_SELECT_MID,    0         ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, SLICE_CTL_DISABLE, 0         ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, MASK_EQ_SYNC,      1         ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, SIGNAL_TO_MONITOR, AUTO      ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, SLICE_MODE,        AUTO      ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, SLICE_DELAY,       3         ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, SLICE_AVG,         AVG16     ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, P_COUNT,           12        ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, H_COUNT,           48        ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, V_COUNT,           68        )
	},

	{
		BCHP_VDEC_FE_0_SYNC_DETECT2,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, HEAD_SWITCH_HISTORY_MASK, 0 ) |
#else
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, MATCH_FILTER_TH,      0 ) |
#endif
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, MATCH_REQ_LOCK,       0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, VCR_MODE,             1 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, FREQ_OFFSET,          0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, CONSISTEN_ERROR_TH,   16 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, HEAD_SWITCH_ERROR_TH, 20 ) |
#if (BVDC_MACROVISION && BVDC_P_VDEC_PR32896_WORK_AROUND)
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, MATCH_FILTER_MODE,    0 ) |
#else
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, MATCH_FILTER_MODE,    1 ) |
#endif
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, RAW_VSYNC_MODE,       0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, HEAD_SWITCH_THRESH,   9 ) |
#if (BVDC_P_3DCOMB_TTD_CONFLICT)
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, LINE_OFFSET,          0 ) |
#else
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, LINE_OFFSET,          1 ) |
#endif
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, SLICE_FILT,           4 )
	},

	{
		BCHP_VDEC_FE_0_SYNC_BLANK,
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_BLANK, F1_END_LINE,   6  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_BLANK, F1_START_LINE, 61 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_BLANK, F0_END_LINE,   6  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_BLANK, F0_START_LINE, 61 )
	},

	{
		BCHP_VDEC_FE_0_RAW_HSYNC_CONTROL,
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, MATCH_OVERRIDE_MIN, 1  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, SAMPLE_TYPE_CTL,    0  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, MATCH_MIN,          32 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, INT_MIN,            45 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, INT_AREA,           1  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, MATCH_DELTA_COEFF,  12 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, DELTA_COEFF,        8 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, INT_TH_COEFF,       10 )
	},

	{
		BCHP_VDEC_FE_0_HSYNC_PLL1,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL1, FRO_FREQ,     FIXED  ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL1, AVG_EN,       UPDATED) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL1, FREQ_AVG,     DISABLE) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL1, FILTA_OFFSET, 21     ) |
#endif
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL1, BIG_SMALL_MODE, AUTO ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL1, DIRECT_GAIN,    4    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL1, SMALL_TERM_CNT, 0    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL1, BIG_TERM_CNT,   1    )
	},

	{
		BCHP_VDEC_FE_0_SYNC_ACCUM_CONTROL,
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_ACCUM_CONTROL, HSYNC_CLAMP_VALUE, 63 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_ACCUM_CONTROL, VSYNC_CLAMP_VALUE, 31 )
	},

	{
		BCHP_VDEC_FE_0_FRO_CONTROL,
		BCHP_FIELD_DATA(VDEC_FE_0_FRO_CONTROL, AVG_ERR_FACTOR, 2               ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_FRO_CONTROL, AUTO_COEFF,     ONE_AND_QUARTER ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FRO_CONTROL, FIELD_ID_TH,    20              ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FRO_CONTROL, CUT_OFF_TH,     31              ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FRO_CONTROL, ERR_TH,         20              ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FRO_CONTROL, FRO_MODE,       1               )
	},

	{
		BCHP_VDEC_FE_0_FIELD_SYNC,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_ENUM(VDEC_FE_0_FIELD_SYNC, FID_INVERT, NO_INVERT               ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_FIELD_SYNC, PC_INTERLACED_FID,  ENABLE          ) |
#endif
		BCHP_FIELD_DATA(VDEC_FE_0_FIELD_SYNC, ID_UPDATE_WINDOW, 0                   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FIELD_SYNC, MARKING_THRESH,  0                   ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_FIELD_SYNC, MARKING_WINDOW,  PLUS_MINUS_64       ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FIELD_SYNC, FIELD_SCOPE,     3                   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FIELD_SYNC, MODE_THRESHOLD,  2                   ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_FIELD_SYNC, TOP_ID,          SYNC_ON_SECOND_HALF ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_FIELD_SYNC, TYPE,            BY_POSITION         )
	},

	{
		BCHP_VDEC_FE_0_SYNC_SLAVE_HRANGE,
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_HRANGE, SYNC_END,   50   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_HRANGE, SYNC_START, 1998 )
	},

	{
		BCHP_VDEC_FE_0_SYNC_SLAVE_LEVEL,
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_LEVEL, BLANK_LENGTH, 6   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_LEVEL, BLANK_START,  160 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_LEVEL, TIP_LENGTH,   5   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_LEVEL, TIP_START,    40  )
	},

	{
		BCHP_VDEC_FE_0_SYNC_SLAVE_DIFF,
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_DIFF, MAX,        7  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_DIFF, LINE_RANGE, 8  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_DIFF, LINE_START, 10 )
	},

	{
		BCHP_VDEC_FE_0_CLEAN_VSYNC_CNT,
		BCHP_FIELD_DATA(VDEC_FE_0_CLEAN_VSYNC_CNT, NOISE_0, 36 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_CLEAN_VSYNC_CNT, NOISE_1, 36 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_CLEAN_VSYNC_CNT, NOISE_2, 36 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_CLEAN_VSYNC_CNT, NOISE_3, 36 )
	},

	/* VDEC_FE_0_FE_STATUS0 (RO) */
	/* VDEC_FE_0_FE_STATUS (RO) */
	/* VDEC_FE_0_FE_FIELD_SIZE (RO) */
	/* VDEC_FE_0_FE_SAMPLE_COUNT (RO) */
	/* VDEC_FE_0_FE_SYNC_SLICE (RO) */
	/* VDEC_FE_0_FE_BLANK_LEVELS (RO) */
	/* VDEC_FE_0_FE_TIP_LEVELS (RO) */
	/* VDEC_FE_0_FE_AGC_LEVELS (RO) */

	/* Last Entry */
	{
		UINT32_C(-1),
		UINT32_C(-1)
	}
};

static const BVDC_P_AddrDataPair s_aVdecFmtTbl_Cvbs_PAL_Nc[] =
{
	/* VDEC_FE_0_FE_REV_ID (RO) */
	{
		BCHP_VDEC_FE_0_HSTART2,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_DATA(VDEC_FE_0_HSTART2, FILTER_COEF_SEL, 1) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSTART2, WINDOW_SW_SELECT,    2 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSTART2, WINDOW_MODE,    1 ) |
#endif
		BCHP_FIELD_DATA(VDEC_FE_0_HSTART2, STORE, 215 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSTART2, CS,    134 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSTART2, CB,    126 )
	},

	{
		BCHP_VDEC_FE_0_NB_CONTROL,
		BCHP_FIELD_DATA(VDEC_FE_0_NB_CONTROL, NOISE_LENGTH, 6   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NB_CONTROL, NOISE_START,  958 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NB_CONTROL, BLANK_LENGTH, 6   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NB_CONTROL, BLANK_START,  96  ) |
#if (BVDC_P_3DCOMB_TTD_CONFLICT)
		BCHP_FIELD_DATA(VDEC_FE_0_NB_CONTROL, LINE,         4   )
#else
		BCHP_FIELD_DATA(VDEC_FE_0_NB_CONTROL, LINE,         5   )
#endif
	},

	{
		BCHP_VDEC_FE_0_AGC_REGION,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_5)
		BCHP_FIELD_ENUM(VDEC_FE_0_AGC_REGION, AGC_PATH_SEP,     SEPARATE ) |
		BCHP_FIELD_DATA(VDEC_FE_0_AGC_REGION, AGC_GAIN_LIMIT_VDEC,     0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_AGC_REGION, AGC_GAIN_LIMIT_SYNC_SEP, 0 ) |
#elif (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_ENUM(VDEC_FE_0_AGC_REGION, AGC_PATH_SEP,     SEPARATE ) |
		BCHP_FIELD_DATA(VDEC_FE_0_AGC_REGION, AGC_GAIN_LIMIT_VDEC,     4 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_AGC_REGION, AGC_GAIN_LIMIT_SYNC_SEP, 4 ) |
#endif
		BCHP_FIELD_DATA(VDEC_FE_0_AGC_REGION, END,   124                 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_AGC_REGION, START, 12                  )
	},

	{
		BCHP_VDEC_FE_0_HPLL_MISC,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_MISC, IFD_PATH_SEL,        VDEC ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_MISC, NOISE_MEASURE_SEL,   VDEC ) |
#endif
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_MISC, SLICE_LPF_HPLL_LOCK_CTL, ENABLE ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_MISC, DISABLE_SLICE_TILT,  1  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_MISC, VSYNC_OFFSET,        0  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_MISC, MATCH_FILT_TH_COEFF, 12 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_MISC, JUMP_END_LINE,       18 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_MISC, JUMP_START_LINE,     10 )
	},

	{
		BCHP_VDEC_FE_0_RAW_VSYNC_CONTROL,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_VSYNC_CONTROL, MAX_HEAD_SWITCH_HYSTERESIS_TH, 11 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_VSYNC_CONTROL, MIN_HEAD_SWITCH_HYSTERESIS_TH, 4 ) |
#endif
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_VSYNC_CONTROL, VPLL_LINE_UPDATE, 1  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_VSYNC_CONTROL, CLEAN_LINE_COUNT, 34 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_VSYNC_CONTROL, BLOCK_LINE_COUNT, 25 )
	},

	{
		BCHP_VDEC_FE_0_SYNC_DETECT,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, DCR_SEL,           VDEC      ) |
#else
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, HEAD_SWITCH_SEL,   FOUR_HEAD ) |
#endif
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, TIP_FILT_SEL,      FILTA     ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, TIP_SELECT_MID,    0         ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, SLICE_CTL_DISABLE, 0         ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, MASK_EQ_SYNC,      1         ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, SIGNAL_TO_MONITOR, AUTO      ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, SLICE_MODE,        AUTO      ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, SLICE_DELAY,       3         ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, SLICE_AVG,         AVG16     ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, P_COUNT,           12        ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, H_COUNT,           48        ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, V_COUNT,           68        )
	},

	{
		BCHP_VDEC_FE_0_SYNC_DETECT2,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, HEAD_SWITCH_HISTORY_MASK, 0 ) |
#else
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, MATCH_FILTER_TH,      0 ) |
#endif
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, MATCH_REQ_LOCK,       0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, VCR_MODE,             1 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, FREQ_OFFSET,          0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, CONSISTEN_ERROR_TH,   16 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, HEAD_SWITCH_ERROR_TH, 20 ) |
#if (BVDC_MACROVISION && BVDC_P_VDEC_PR32896_WORK_AROUND)
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, MATCH_FILTER_MODE,    0 ) |
#else
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, MATCH_FILTER_MODE,    1 ) |
#endif
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, RAW_VSYNC_MODE,       0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, HEAD_SWITCH_THRESH,   9 ) |
#if (BVDC_P_3DCOMB_TTD_CONFLICT)
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, LINE_OFFSET,          0 ) |
#else
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, LINE_OFFSET,          1 ) |
#endif
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, SLICE_FILT,           4 )
	},

	{
		BCHP_VDEC_FE_0_SYNC_BLANK,
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_BLANK, F1_END_LINE,   6  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_BLANK, F1_START_LINE, 61 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_BLANK, F0_END_LINE,   6  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_BLANK, F0_START_LINE, 61 )
	},

	{
		BCHP_VDEC_FE_0_RAW_HSYNC_CONTROL,
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, MATCH_OVERRIDE_MIN, 1  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, SAMPLE_TYPE_CTL,    0  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, MATCH_MIN,          32 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, INT_MIN,            45 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, INT_AREA,           1  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, MATCH_DELTA_COEFF,  12 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, DELTA_COEFF,        12 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, INT_TH_COEFF,       10 )
	},

	{
		BCHP_VDEC_FE_0_HSYNC_PLL1,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL1, FRO_FREQ,     FIXED  ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL1, AVG_EN,       UPDATED) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL1, FREQ_AVG,     DISABLE) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL1, FILTA_OFFSET, 21     ) |
#endif
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL1, BIG_SMALL_MODE, AUTO ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL1, DIRECT_GAIN,    4    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL1, SMALL_TERM_CNT, 0    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL1, BIG_TERM_CNT,   1    )
	},

	{
		BCHP_VDEC_FE_0_SYNC_ACCUM_CONTROL,
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_ACCUM_CONTROL, HSYNC_CLAMP_VALUE, 63 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_ACCUM_CONTROL, VSYNC_CLAMP_VALUE, 31 )
	},

	{
		BCHP_VDEC_FE_0_FRO_CONTROL,
		BCHP_FIELD_DATA(VDEC_FE_0_FRO_CONTROL, AVG_ERR_FACTOR, 2               ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_FRO_CONTROL, AUTO_COEFF,     ONE_AND_QUARTER ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FRO_CONTROL, FIELD_ID_TH,    20              ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FRO_CONTROL, CUT_OFF_TH,     31              ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FRO_CONTROL, ERR_TH,         20              ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FRO_CONTROL, FRO_MODE,       1               )
	},

	{
		BCHP_VDEC_FE_0_FIELD_SYNC,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_ENUM(VDEC_FE_0_FIELD_SYNC, FID_INVERT, NO_INVERT               ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_FIELD_SYNC, PC_INTERLACED_FID,  ENABLE          ) |
#endif
		BCHP_FIELD_DATA(VDEC_FE_0_FIELD_SYNC, ID_UPDATE_WINDOW, 0                   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FIELD_SYNC, MARKING_THRESH,  0                   ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_FIELD_SYNC, MARKING_WINDOW,  PLUS_MINUS_64       ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FIELD_SYNC, FIELD_SCOPE,     3                   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FIELD_SYNC, MODE_THRESHOLD,  2                   ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_FIELD_SYNC, TOP_ID,          SYNC_ON_SECOND_HALF ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_FIELD_SYNC, TYPE,            BY_POSITION         )
	},

	{
		BCHP_VDEC_FE_0_SYNC_SLAVE_HRANGE,
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_HRANGE, SYNC_END,   50   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_HRANGE, SYNC_START, 1998 )
	},

	{
		BCHP_VDEC_FE_0_SYNC_SLAVE_LEVEL,
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_LEVEL, BLANK_LENGTH, 6   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_LEVEL, BLANK_START,  160 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_LEVEL, TIP_LENGTH,   5   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_LEVEL, TIP_START,    40  )
	},

	{
		BCHP_VDEC_FE_0_SYNC_SLAVE_DIFF,
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_DIFF, MAX,        7  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_DIFF, LINE_RANGE, 8  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_DIFF, LINE_START, 10 )
	},

	{
		BCHP_VDEC_FE_0_CLEAN_VSYNC_CNT,
		BCHP_FIELD_DATA(VDEC_FE_0_CLEAN_VSYNC_CNT, NOISE_0, 36 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_CLEAN_VSYNC_CNT, NOISE_1, 36 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_CLEAN_VSYNC_CNT, NOISE_2, 36 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_CLEAN_VSYNC_CNT, NOISE_3, 36 )
	},

	/* VDEC_FE_0_FE_STATUS0 (RO) */
	/* VDEC_FE_0_FE_STATUS (RO) */
	/* VDEC_FE_0_FE_FIELD_SIZE (RO) */
	/* VDEC_FE_0_FE_SAMPLE_COUNT (RO) */
	/* VDEC_FE_0_FE_SYNC_SLICE (RO) */
	/* VDEC_FE_0_FE_BLANK_LEVELS (RO) */
	/* VDEC_FE_0_FE_TIP_LEVELS (RO) */
	/* VDEC_FE_0_FE_AGC_LEVELS (RO) */

	/* Last Entry */
	{
		UINT32_C(-1),
		UINT32_C(-1)
	}
};

static const BVDC_P_AddrDataPair s_aVdecFmtTbl_Cvbs_SECAM[] =
{
	/* VDEC_FE_0_FE_REV_ID (RO) */
	{
		BCHP_VDEC_FE_0_HSTART2,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_ENUM(VDEC_FE_0_HSTART2, FILTER_COEF_SEL, CENTER_FREQ_443361875 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSTART2, WINDOW_SW_SELECT, 2 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSTART2, WINDOW_MODE,      1 ) |
#endif
		BCHP_FIELD_DATA(VDEC_FE_0_HSTART2, STORE, 215 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSTART2, CS,    134 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSTART2, CB,    126 )
	},

	{
		BCHP_VDEC_FE_0_NB_CONTROL,
		BCHP_FIELD_DATA(VDEC_FE_0_NB_CONTROL, NOISE_LENGTH, 6   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NB_CONTROL, NOISE_START,  958 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NB_CONTROL, BLANK_LENGTH, 6   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NB_CONTROL, BLANK_START,  96  ) |
#if (BVDC_P_3DCOMB_TTD_CONFLICT)
		BCHP_FIELD_DATA(VDEC_FE_0_NB_CONTROL, LINE,         4   )
#else
		BCHP_FIELD_DATA(VDEC_FE_0_NB_CONTROL, LINE,         5   )
#endif
	},

	{
		BCHP_VDEC_FE_0_AGC_REGION,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_5)
		BCHP_FIELD_ENUM(VDEC_FE_0_AGC_REGION, AGC_PATH_SEP,     SEPARATE ) |
		BCHP_FIELD_DATA(VDEC_FE_0_AGC_REGION, AGC_GAIN_LIMIT_VDEC,     0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_AGC_REGION, AGC_GAIN_LIMIT_SYNC_SEP, 0 ) |
#elif (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_ENUM(VDEC_FE_0_AGC_REGION, AGC_PATH_SEP,     SEPARATE ) |
		BCHP_FIELD_DATA(VDEC_FE_0_AGC_REGION, AGC_GAIN_LIMIT_VDEC,     4 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_AGC_REGION, AGC_GAIN_LIMIT_SYNC_SEP, 4 ) |
#endif
		BCHP_FIELD_DATA(VDEC_FE_0_AGC_REGION, END,   124                 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_AGC_REGION, START, 12                  )
	},

	{
		BCHP_VDEC_FE_0_HPLL_MISC,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_MISC, IFD_PATH_SEL,        VDEC ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_MISC, NOISE_MEASURE_SEL,   VDEC ) |
#endif
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_MISC, SLICE_LPF_HPLL_LOCK_CTL, ENABLE ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_MISC, DISABLE_SLICE_TILT,  1  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_MISC, VSYNC_OFFSET,        0  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_MISC, MATCH_FILT_TH_COEFF, 12 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_MISC, JUMP_END_LINE,       18 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_MISC, JUMP_START_LINE,     10 )
	},

	{
		BCHP_VDEC_FE_0_RAW_VSYNC_CONTROL,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_VSYNC_CONTROL, MAX_HEAD_SWITCH_HYSTERESIS_TH, 11 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_VSYNC_CONTROL, MIN_HEAD_SWITCH_HYSTERESIS_TH, 4 ) |
#endif
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_VSYNC_CONTROL, VPLL_LINE_UPDATE, 1  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_VSYNC_CONTROL, CLEAN_LINE_COUNT, 34 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_VSYNC_CONTROL, BLOCK_LINE_COUNT, 25 )
	},

	{
		BCHP_VDEC_FE_0_SYNC_DETECT,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, DCR_SEL,           VDEC      ) |
#else
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, HEAD_SWITCH_SEL,   FOUR_HEAD ) |
#endif
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, TIP_FILT_SEL,      FILTA     ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, TIP_SELECT_MID,    0         ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, SLICE_CTL_DISABLE, 0         ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, MASK_EQ_SYNC,      1         ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, SIGNAL_TO_MONITOR, AUTO      ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, SLICE_MODE,        AUTO      ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, SLICE_DELAY,       3         ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, SLICE_AVG,         AVG16     ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, P_COUNT,           12        ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, H_COUNT,           48        ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, V_COUNT,           68        )
	},

	{
		BCHP_VDEC_FE_0_SYNC_DETECT2,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, HEAD_SWITCH_HISTORY_MASK, 0 ) |
#else
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, MATCH_FILTER_TH,      0 ) |
#endif
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, MATCH_REQ_LOCK,       0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, VCR_MODE,             1 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, FREQ_OFFSET,          0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, CONSISTEN_ERROR_TH,   16) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, HEAD_SWITCH_ERROR_TH, 20) |
#if (BVDC_MACROVISION && BVDC_P_VDEC_PR32896_WORK_AROUND)
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, MATCH_FILTER_MODE,    0 ) |
#else
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, MATCH_FILTER_MODE,    1 ) |
#endif
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, RAW_VSYNC_MODE,       0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, HEAD_SWITCH_THRESH,   9 ) |
#if (BVDC_P_3DCOMB_TTD_CONFLICT)
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, LINE_OFFSET,          0 ) |
#else
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, LINE_OFFSET,          1 ) |
#endif
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, SLICE_FILT,           4 )
	},

	{
		BCHP_VDEC_FE_0_SYNC_BLANK,
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_BLANK, F1_END_LINE,   6  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_BLANK, F1_START_LINE, 61 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_BLANK, F0_END_LINE,   6  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_BLANK, F0_START_LINE, 61 )
	},

	{
		BCHP_VDEC_FE_0_RAW_HSYNC_CONTROL,
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, MATCH_OVERRIDE_MIN, 1  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, SAMPLE_TYPE_CTL,    0  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, MATCH_MIN,          32 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, INT_MIN,            45 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, INT_AREA,           1  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, MATCH_DELTA_COEFF,  12 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, DELTA_COEFF,        8  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, INT_TH_COEFF,       10 )
	},

	{
		BCHP_VDEC_FE_0_HSYNC_PLL1,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL1, FRO_FREQ,     FIXED  ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL1, AVG_EN,       UPDATED) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL1, FREQ_AVG,     DISABLE) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL1, FILTA_OFFSET, 21     ) |
#endif
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL1, BIG_SMALL_MODE, AUTO ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL1, DIRECT_GAIN,    4    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL1, SMALL_TERM_CNT, 0    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL1, BIG_TERM_CNT,   1    )
	},

	{
		BCHP_VDEC_FE_0_SYNC_ACCUM_CONTROL,
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_ACCUM_CONTROL, HSYNC_CLAMP_VALUE, 63 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_ACCUM_CONTROL, VSYNC_CLAMP_VALUE, 31 )
	},

	{
		BCHP_VDEC_FE_0_FRO_CONTROL,
		BCHP_FIELD_DATA(VDEC_FE_0_FRO_CONTROL, AVG_ERR_FACTOR, 2               ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_FRO_CONTROL, AUTO_COEFF,     ONE_AND_QUARTER ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FRO_CONTROL, FIELD_ID_TH,    20              ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FRO_CONTROL, CUT_OFF_TH,     31              ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FRO_CONTROL, ERR_TH,         20              ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FRO_CONTROL, FRO_MODE,       1               )
	},

	{
		BCHP_VDEC_FE_0_FIELD_SYNC,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_ENUM(VDEC_FE_0_FIELD_SYNC, FID_INVERT, NO_INVERT               ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_FIELD_SYNC, PC_INTERLACED_FID,  ENABLE          ) |
#endif
		BCHP_FIELD_DATA(VDEC_FE_0_FIELD_SYNC, ID_UPDATE_WINDOW, 0                   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FIELD_SYNC, MARKING_THRESH,  0                   ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_FIELD_SYNC, MARKING_WINDOW,  PLUS_MINUS_64       ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FIELD_SYNC, FIELD_SCOPE,     3                   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FIELD_SYNC, MODE_THRESHOLD,  2                   ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_FIELD_SYNC, TOP_ID,          SYNC_ON_SECOND_HALF ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_FIELD_SYNC, TYPE,            BY_POSITION         )
	},

	{
		BCHP_VDEC_FE_0_SYNC_SLAVE_HRANGE,
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_HRANGE, SYNC_END,   50   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_HRANGE, SYNC_START, 1998 )
	},

	{
		BCHP_VDEC_FE_0_SYNC_SLAVE_LEVEL,
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_LEVEL, BLANK_LENGTH, 6   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_LEVEL, BLANK_START,  160 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_LEVEL, TIP_LENGTH,   5   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_LEVEL, TIP_START,    40  )
	},

	{
		BCHP_VDEC_FE_0_SYNC_SLAVE_DIFF,
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_DIFF, MAX,        7  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_DIFF, LINE_RANGE, 8  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_DIFF, LINE_START, 10 )
	},

	{
		BCHP_VDEC_FE_0_CLEAN_VSYNC_CNT,
		BCHP_FIELD_DATA(VDEC_FE_0_CLEAN_VSYNC_CNT, NOISE_0, 36 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_CLEAN_VSYNC_CNT, NOISE_1, 36 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_CLEAN_VSYNC_CNT, NOISE_2, 36 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_CLEAN_VSYNC_CNT, NOISE_3, 36 )
	},

	/* VDEC_FE_0_FE_STATUS0 (RO) */
	/* VDEC_FE_0_FE_STATUS (RO) */
	/* VDEC_FE_0_FE_FIELD_SIZE (RO) */
	/* VDEC_FE_0_FE_SAMPLE_COUNT (RO) */
	/* VDEC_FE_0_FE_SYNC_SLICE (RO) */
	/* VDEC_FE_0_FE_BLANK_LEVELS (RO) */
	/* VDEC_FE_0_FE_TIP_LEVELS (RO) */
	/* VDEC_FE_0_FE_AGC_LEVELS (RO) */

	/* Last Entry */
	{
		UINT32_C(-1),
		UINT32_C(-1)
	}
};

static const BVDC_P_AddrDataPair s_aVdecFmtTbl_Cvbs_PALM[] =
{
	{
		BCHP_VDEC_FE_0_HSTART2,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_DATA(VDEC_FE_0_HSTART2, FILTER_COEF_SEL, 0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSTART2, WINDOW_SW_SELECT,    2 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSTART2, WINDOW_MODE,    1 ) |
#endif
		BCHP_FIELD_DATA(VDEC_FE_0_HSTART2, STORE, 215 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSTART2, CS,    134 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSTART2, CB,    126 )
	},

	{
		BCHP_VDEC_FE_0_NB_CONTROL,
		BCHP_FIELD_DATA(VDEC_FE_0_NB_CONTROL, NOISE_LENGTH, 6   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NB_CONTROL, NOISE_START,  1120 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NB_CONTROL, BLANK_LENGTH, 6   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NB_CONTROL, BLANK_START,  96  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NB_CONTROL, LINE,         9    )
	},

	{
		BCHP_VDEC_FE_0_AGC_REGION,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_5)
		BCHP_FIELD_ENUM(VDEC_FE_0_AGC_REGION, AGC_PATH_SEP,     SEPARATE ) |
		BCHP_FIELD_DATA(VDEC_FE_0_AGC_REGION, AGC_GAIN_LIMIT_VDEC,     0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_AGC_REGION, AGC_GAIN_LIMIT_SYNC_SEP, 0 ) |
#elif (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_ENUM(VDEC_FE_0_AGC_REGION, AGC_PATH_SEP,     SEPARATE ) |
		BCHP_FIELD_DATA(VDEC_FE_0_AGC_REGION, AGC_GAIN_LIMIT_VDEC,     4 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_AGC_REGION, AGC_GAIN_LIMIT_SYNC_SEP, 4 ) |
#endif
		BCHP_FIELD_DATA(VDEC_FE_0_AGC_REGION, END,   124                 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_AGC_REGION, START, 12                  )
	},

	{
		BCHP_VDEC_FE_0_HPLL_MISC,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_MISC, IFD_PATH_SEL,        VDEC ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_MISC, NOISE_MEASURE_SEL,   VDEC ) |
#endif
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_MISC, SLICE_LPF_HPLL_LOCK_CTL, DISABLE ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_MISC, DISABLE_SLICE_TILT,  1  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_MISC, VSYNC_OFFSET,        0  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_MISC, MATCH_FILT_TH_COEFF, 12 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_MISC, JUMP_END_LINE,       18 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_MISC, JUMP_START_LINE,     10 )
	},

	{
		BCHP_VDEC_FE_0_RAW_VSYNC_CONTROL,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_VSYNC_CONTROL, MAX_HEAD_SWITCH_HYSTERESIS_TH, 5 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_VSYNC_CONTROL, MIN_HEAD_SWITCH_HYSTERESIS_TH, 3 ) |
#endif
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_VSYNC_CONTROL, VPLL_LINE_UPDATE, 1  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_VSYNC_CONTROL, CLEAN_LINE_COUNT, 36 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_VSYNC_CONTROL, BLOCK_LINE_COUNT, 25 )
	},

	{
		BCHP_VDEC_FE_0_SYNC_DETECT,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, DCR_SEL,           VDEC      ) |
#else
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, HEAD_SWITCH_SEL,   TWO_HEAD) |
#endif
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, TIP_FILT_SEL,      FILTA     ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, TIP_SELECT_MID,    0         ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, SLICE_CTL_DISABLE, 0         ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, MASK_EQ_SYNC,      1         ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, SIGNAL_TO_MONITOR, AUTO      ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, SLICE_MODE,        AUTO      ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, SLICE_DELAY,       3         ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, SLICE_AVG,         AVG16     ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, P_COUNT,           24        ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, H_COUNT,           48        ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, V_COUNT,           68        )
	},

	{
		BCHP_VDEC_FE_0_SYNC_DETECT2,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, HEAD_SWITCH_HISTORY_MASK, 0 ) |
#else
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, MATCH_FILTER_TH,      0 ) |
#endif
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, MATCH_REQ_LOCK,       1  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, VCR_MODE,             1  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, FREQ_OFFSET,          32 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, CONSISTEN_ERROR_TH,   16 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, HEAD_SWITCH_ERROR_TH, 20 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, MATCH_FILTER_MODE,    1  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, RAW_VSYNC_MODE,       1  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, HEAD_SWITCH_THRESH,   9  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, LINE_OFFSET,          4  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, SLICE_FILT,           4  )
	},

	{
		BCHP_VDEC_FE_0_SYNC_BLANK,
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_BLANK, F1_END_LINE,   7  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_BLANK, F1_START_LINE, 61 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_BLANK, F0_END_LINE,   7  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_BLANK, F0_START_LINE, 61 )
	},

	{
		BCHP_VDEC_FE_0_RAW_HSYNC_CONTROL,
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, MATCH_OVERRIDE_MIN, 1  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, SAMPLE_TYPE_CTL,    0  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, MATCH_MIN,          32 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, INT_MIN,            45 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, INT_AREA,           1  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, MATCH_DELTA_COEFF,  8 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, DELTA_COEFF,        12 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, INT_TH_COEFF,       10 )
	},

	{
		BCHP_VDEC_FE_0_HSYNC_PLL1,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL1, FRO_FREQ,    FIXED   ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL1, AVG_EN,      UPDATED ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL1, FREQ_AVG,    ENABLE  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL1, FILTA_OFFSET, 21     ) |
#endif
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL1, BIG_SMALL_MODE, AUTO ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL1, DIRECT_GAIN,    4    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL1, SMALL_TERM_CNT, 0    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL1, BIG_TERM_CNT,   1    )
	},

	{
		BCHP_VDEC_FE_0_SYNC_ACCUM_CONTROL,
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_ACCUM_CONTROL, HSYNC_CLAMP_VALUE, 63 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_ACCUM_CONTROL, VSYNC_CLAMP_VALUE, 31 )
	},

	{
		BCHP_VDEC_FE_0_FRO_CONTROL,
		BCHP_FIELD_DATA(VDEC_FE_0_FRO_CONTROL, AVG_ERR_FACTOR, 2               ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_FRO_CONTROL, AUTO_COEFF,     ONE             ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FRO_CONTROL, FIELD_ID_TH,    20              ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FRO_CONTROL, CUT_OFF_TH,     31              ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FRO_CONTROL, ERR_TH,         20              ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FRO_CONTROL, FRO_MODE,       1               )
	},

	{
		BCHP_VDEC_FE_0_FIELD_SYNC,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_ENUM(VDEC_FE_0_FIELD_SYNC, FID_INVERT, NO_INVERT               ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_FIELD_SYNC, PC_INTERLACED_FID,  ENABLE          ) |
#endif
		BCHP_FIELD_DATA(VDEC_FE_0_FIELD_SYNC, ID_UPDATE_WINDOW, 0                   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FIELD_SYNC, MARKING_THRESH,  0                   ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_FIELD_SYNC, MARKING_WINDOW,  PLUS_MINUS_64       ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FIELD_SYNC, FIELD_SCOPE,     3                   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FIELD_SYNC, MODE_THRESHOLD,  2                   ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_FIELD_SYNC, TOP_ID,          SYNC_ON_SECOND_HALF ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_FIELD_SYNC, TYPE,            BY_POSITION         )
	},

	{
		BCHP_VDEC_FE_0_SYNC_SLAVE_HRANGE,
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_HRANGE, SYNC_END,   50   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_HRANGE, SYNC_START, 1998 )
	},

	{
		BCHP_VDEC_FE_0_SYNC_SLAVE_LEVEL,
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_LEVEL, BLANK_LENGTH, 6   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_LEVEL, BLANK_START,  160 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_LEVEL, TIP_LENGTH,   5   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_LEVEL, TIP_START,    40  )
	},

	{
		BCHP_VDEC_FE_0_SYNC_SLAVE_DIFF,
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_DIFF, MAX,        7  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_DIFF, LINE_RANGE, 8  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_DIFF, LINE_START, 10 )
	},

	{
		BCHP_VDEC_FE_0_CLEAN_VSYNC_CNT,
		BCHP_FIELD_DATA(VDEC_FE_0_CLEAN_VSYNC_CNT, NOISE_0, 36 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_CLEAN_VSYNC_CNT, NOISE_1, 36 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_CLEAN_VSYNC_CNT, NOISE_2, 36 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_CLEAN_VSYNC_CNT, NOISE_3, 36 )
	},

	/* VDEC_FE_0_FE_STATUS0 (RO) */
	/* VDEC_FE_0_FE_STATUS (RO) */
	/* VDEC_FE_0_FE_FIELD_SIZE (RO) */
	/* VDEC_FE_0_FE_SAMPLE_COUNT (RO) */
	/* VDEC_FE_0_FE_SYNC_SLICE (RO) */
	/* VDEC_FE_0_FE_BLANK_LEVELS (RO) */
	/* VDEC_FE_0_FE_TIP_LEVELS (RO) */
	/* VDEC_FE_0_FE_AGC_LEVELS (RO) */

	/* Last Entry */
	{
		UINT32_C(-1),
		UINT32_C(-1)
	}
};

static const BVDC_P_AddrDataPair s_aVdecFmtTbl_YPrPb_576p[] =
{
	/* VDEC_FE_0_FE_REV_ID (RO) */
	{
		BCHP_VDEC_FE_0_HSTART2,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_DATA(VDEC_FE_0_HSTART2, FILTER_COEF_SEL, 0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSTART2, WINDOW_SW_SELECT,    0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSTART2, WINDOW_MODE,    0 ) |
#endif
		BCHP_FIELD_DATA(VDEC_FE_0_HSTART2, STORE, 215 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSTART2, CS,    134 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSTART2, CB,    126 )
	},

	{
		BCHP_VDEC_FE_0_NB_CONTROL,
		BCHP_FIELD_DATA(VDEC_FE_0_NB_CONTROL, NOISE_LENGTH, 6   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NB_CONTROL, NOISE_START,  958 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NB_CONTROL, BLANK_LENGTH, 6   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NB_CONTROL, BLANK_START,  96  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NB_CONTROL, LINE,         5   )
	},

	{
		BCHP_VDEC_FE_0_AGC_REGION,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_5)
		BCHP_FIELD_ENUM(VDEC_FE_0_AGC_REGION, AGC_PATH_SEP,     SEPARATE ) |
		BCHP_FIELD_DATA(VDEC_FE_0_AGC_REGION, AGC_GAIN_LIMIT_VDEC,     0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_AGC_REGION, AGC_GAIN_LIMIT_SYNC_SEP, 0 ) |
#elif (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_ENUM(VDEC_FE_0_AGC_REGION, AGC_PATH_SEP,     SEPARATE ) |
		BCHP_FIELD_DATA(VDEC_FE_0_AGC_REGION, AGC_GAIN_LIMIT_VDEC,     4 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_AGC_REGION, AGC_GAIN_LIMIT_SYNC_SEP, 4 ) |
#endif
		BCHP_FIELD_DATA(VDEC_FE_0_AGC_REGION, END,   124                 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_AGC_REGION, START, 12                  )
	},

	{
		BCHP_VDEC_FE_0_HPLL_MISC,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_MISC, IFD_PATH_SEL,        VDEC ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_MISC, NOISE_MEASURE_SEL,   VDEC ) |
#endif
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_MISC, SLICE_LPF_HPLL_LOCK_CTL, ENABLE ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_MISC, DISABLE_SLICE_TILT,  1  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_MISC, VSYNC_OFFSET,        0  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_MISC, MATCH_FILT_TH_COEFF, 12 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_MISC, JUMP_END_LINE,       18 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_MISC, JUMP_START_LINE,     10 )
	},

	{
		BCHP_VDEC_FE_0_RAW_VSYNC_CONTROL,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_VSYNC_CONTROL, MAX_HEAD_SWITCH_HYSTERESIS_TH, 11 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_VSYNC_CONTROL, MIN_HEAD_SWITCH_HYSTERESIS_TH, 4 ) |
#endif
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_VSYNC_CONTROL, VPLL_LINE_UPDATE, 1  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_VSYNC_CONTROL, CLEAN_LINE_COUNT, 34 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_VSYNC_CONTROL, BLOCK_LINE_COUNT, 25 )
	},

	{
		BCHP_VDEC_FE_0_SYNC_DETECT,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, DCR_SEL,           VDEC      ) |
#else
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, HEAD_SWITCH_SEL,   FOUR_HEAD ) |
#endif
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, TIP_FILT_SEL,      FILTA     ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, TIP_SELECT_MID,    0         ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, SLICE_CTL_DISABLE, 0         ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, MASK_EQ_SYNC,      1         ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, SIGNAL_TO_MONITOR, AUTO      ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, SLICE_MODE,        AUTO      ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, SLICE_DELAY,       3         ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, SLICE_AVG,         AVG16     ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, P_COUNT,           12        ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, H_COUNT,           48        ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, V_COUNT,           144       )
	},

	{
		BCHP_VDEC_FE_0_SYNC_DETECT2,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, HEAD_SWITCH_HISTORY_MASK, 0 ) |
#else
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, MATCH_FILTER_TH,      0 ) |
#endif
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, MATCH_REQ_LOCK,       0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, VCR_MODE,             0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, FREQ_OFFSET,          0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, CONSISTEN_ERROR_TH,   16 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, HEAD_SWITCH_ERROR_TH, 20 ) |
#if (BVDC_MACROVISION && BVDC_P_VDEC_PR32896_WORK_AROUND)
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, MATCH_FILTER_MODE,    0 ) |
#else
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, MATCH_FILTER_MODE,    1 ) |
#endif
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, RAW_VSYNC_MODE,       0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, HEAD_SWITCH_THRESH,   9  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, LINE_OFFSET,          1 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, SLICE_FILT,           4 )
	},

	{
		BCHP_VDEC_FE_0_SYNC_BLANK,
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_BLANK, F1_END_LINE,   7  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_BLANK, F1_START_LINE, 61 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_BLANK, F0_END_LINE,   7  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_BLANK, F0_START_LINE, 61 )
	},

	{
		BCHP_VDEC_FE_0_RAW_HSYNC_CONTROL,
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, MATCH_OVERRIDE_MIN, 1  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, SAMPLE_TYPE_CTL,    0  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, MATCH_MIN,          32 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, INT_MIN,            45 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, INT_AREA,           1  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, MATCH_DELTA_COEFF,  12 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, DELTA_COEFF,        12 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, INT_TH_COEFF,       10 )
	},

	{
		BCHP_VDEC_FE_0_HSYNC_PLL1,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL1, FRO_FREQ,     FIXED  ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL1, AVG_EN,       UPDATED) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL1, FREQ_AVG,     DISABLE) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL1, FILTA_OFFSET, 0      ) |
#endif
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL1, BIG_SMALL_MODE, AUTO ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL1, DIRECT_GAIN,    4    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL1, SMALL_TERM_CNT, 0    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL1, BIG_TERM_CNT,   1    )
	},

	{
		BCHP_VDEC_FE_0_SYNC_ACCUM_CONTROL,
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_ACCUM_CONTROL, HSYNC_CLAMP_VALUE, 63 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_ACCUM_CONTROL, VSYNC_CLAMP_VALUE, 31 )
	},

	{
		BCHP_VDEC_FE_0_HSYNC_PLL,
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL, BY_PASS_LPF,  0   ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL, ACCUM_THRESH, OFF ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL, LOCK_THRESH,  3   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL, LOCK_WINDOW,  15  )
	},

	{
		BCHP_VDEC_FE_0_HSYNC_PLL_FILTER,
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL_FILTER, NOISE_3, 48 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL_FILTER, NOISE_2, 48 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL_FILTER, NOISE_1, 48 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL_FILTER, NOISE_0, 48 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL_FILTER, UNLOCK,  48 )
	},

	{
		BCHP_VDEC_FE_0_FRO_CONTROL,
		BCHP_FIELD_DATA(VDEC_FE_0_FRO_CONTROL, AVG_ERR_FACTOR, 2               ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_FRO_CONTROL, AUTO_COEFF,     ONE_AND_QUARTER ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FRO_CONTROL, FIELD_ID_TH,    20              ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FRO_CONTROL, CUT_OFF_TH,     31              ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FRO_CONTROL, ERR_TH,         20              ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FRO_CONTROL, FRO_MODE,       1               )
	},

	{
		BCHP_VDEC_FE_0_FIELD_SYNC,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_ENUM(VDEC_FE_0_FIELD_SYNC, FID_INVERT, NO_INVERT               ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_FIELD_SYNC, PC_INTERLACED_FID,  ENABLE          ) |
#endif
		BCHP_FIELD_DATA(VDEC_FE_0_FIELD_SYNC, ID_UPDATE_WINDOW, 0                   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FIELD_SYNC, MARKING_THRESH,  0                   ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_FIELD_SYNC, MARKING_WINDOW,  PLUS_MINUS_64       ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FIELD_SYNC, FIELD_SCOPE,     3                   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FIELD_SYNC, MODE_THRESHOLD,  2                   ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_FIELD_SYNC, TOP_ID,          SYNC_ON_SECOND_HALF ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_FIELD_SYNC, TYPE,            BY_POSITION         )
	},

	{
		BCHP_VDEC_FE_0_SYNC_SLAVE_HRANGE,
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_HRANGE, SYNC_END,   50   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_HRANGE, SYNC_START, 1998 )
	},

	{
		BCHP_VDEC_FE_0_SYNC_SLAVE_LEVEL,
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_LEVEL, BLANK_LENGTH, 6   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_LEVEL, BLANK_START,  160 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_LEVEL, TIP_LENGTH,   5   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_LEVEL, TIP_START,    40  )
	},

	{
		BCHP_VDEC_FE_0_SYNC_SLAVE_DIFF,
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_DIFF, MAX,        7  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_DIFF, LINE_RANGE, 9  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_SLAVE_DIFF, LINE_START, 10 )
	},

	{
		BCHP_VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN1,
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN1, FILT_SEL_NOISE_0, FILTB ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN1, FILT_SEL_NOISE_1, FILTB ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN1, NOISE_0,          63    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN1, NOISE_1,          63    )
	},

	{
		BCHP_VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN2,
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN2, FILT_SEL_NOISE_2, FILTA ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN2, FILT_SEL_NOISE_3, FILTA ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN2, NOISE_2,          63    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN2, NOISE_3,          63    )
	},

	{
		BCHP_VDEC_FE_0_BIG_SMALL_TH1,
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH1, SMALL_TO_BIG_TH_NOISE_0, 20 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH1, BIG_TO_SMALL_TH_NOISE_0, 5  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH1, SMALL_TO_BIG_TH_NOISE_1, 20 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH1, BIG_TO_SMALL_TH_NOISE_1, 5  )
	},

	{
		BCHP_VDEC_FE_0_BIG_SMALL_TH2,
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH2, SMALL_TO_BIG_TH_NOISE_2, 20 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH2, BIG_TO_SMALL_TH_NOISE_2, 5  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH2, SMALL_TO_BIG_TH_NOISE_3, 20 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH2, BIG_TO_SMALL_TH_NOISE_3, 5  )
	},

	{
		BCHP_VDEC_FE_0_HSYNC_ACC_GAIN1,
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN1, SMALL_NOISE_0, 74 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN1, BIG_NOISE_0,   21 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN1, SMALL_NOISE_1, 74 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN1, BIG_NOISE_1,   21 )
	},

	{
		BCHP_VDEC_FE_0_HSYNC_ACC_GAIN2,
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN2, SMALL_NOISE_2, 74 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN2, BIG_NOISE_2,   21 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN2, SMALL_NOISE_3, 74 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN2, BIG_NOISE_3,   21 )
	},

	{
		BCHP_VDEC_FE_0_CLEAN_VSYNC_CNT,
		BCHP_FIELD_DATA(VDEC_FE_0_CLEAN_VSYNC_CNT, NOISE_0, 36 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_CLEAN_VSYNC_CNT, NOISE_1, 36 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_CLEAN_VSYNC_CNT, NOISE_2, 36 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_CLEAN_VSYNC_CNT, NOISE_3, 36 )
	},

	{
		BCHP_VDEC_FE_0_NOISE_BIN_TH,
		BCHP_FIELD_DATA(VDEC_FE_0_NOISE_BIN_TH, HYSTERESIS, 5   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NOISE_BIN_TH, TH_2,       149 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NOISE_BIN_TH, TH_1,       75  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NOISE_BIN_TH, TH_0,       30  )
	},

	/* VDEC_FE_0_FE_STATUS0 (RO) */
	/* VDEC_FE_0_FE_STATUS (RO) */
	/* VDEC_FE_0_FE_FIELD_SIZE (RO) */
	/* VDEC_FE_0_FE_SAMPLE_COUNT (RO) */
	/* VDEC_FE_0_FE_SYNC_SLICE (RO) */
	/* VDEC_FE_0_FE_BLANK_LEVELS (RO) */
	/* VDEC_FE_0_FE_TIP_LEVELS (RO) */
	/* VDEC_FE_0_FE_AGC_LEVELS (RO) */

	/* Last Entry */
	{
		UINT32_C(-1),
		UINT32_C(-1)
	}
};


/****************************************************************************
 * VCR table!  Not to many of them!  If a lot jsut move it to a function!
 * If other functionality start writing to this it's better just to move
 * out as well.
 * TODO: Optimizations needed.
 ****************************************************************************/
static const BVDC_P_AddrDataPair s_aVdecFmtTbl_Cvbs_NTSC_Vcr[] =
{
	{
		BCHP_VDEC_FE_0_HSYNC_PLL1,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL1, FRO_FREQ,     FIXED  ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL1, AVG_EN,       UPDATED) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL1, FREQ_AVG,     ENABLE ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL1, FILTA_OFFSET, 21     ) |
#endif
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL1, BIG_SMALL_MODE, SMALL) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL1, DIRECT_GAIN,    4    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL1, SMALL_TERM_CNT, 0    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL1, BIG_TERM_CNT,   1    )
	},

	{
		BCHP_VDEC_FE_0_SYNC_DETECT2,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, HEAD_SWITCH_HISTORY_MASK, 0 ) |
#else
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, MATCH_FILTER_TH,      0 ) |
#endif
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, MATCH_REQ_LOCK,       1  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, VCR_MODE,             1  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, FREQ_OFFSET,          32 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, CONSISTEN_ERROR_TH,   16 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, HEAD_SWITCH_ERROR_TH, 20 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, MATCH_FILTER_MODE,    1  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, RAW_VSYNC_MODE,       1  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, HEAD_SWITCH_THRESH,   9  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, LINE_OFFSET,          4  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, SLICE_FILT,           4  )
	},

	{
		BCHP_VDEC_FE_0_FRO_CONTROL,
		BCHP_FIELD_DATA(VDEC_FE_0_FRO_CONTROL, AVG_ERR_FACTOR, 2               ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_FRO_CONTROL, AUTO_COEFF,     ONE_AND_QUARTER ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FRO_CONTROL, FIELD_ID_TH,    20              ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FRO_CONTROL, CUT_OFF_TH,     31              ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FRO_CONTROL, ERR_TH,         20              ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FRO_CONTROL, FRO_MODE,       2   )
	},

	{
		BCHP_VDEC_FE_0_CLEAN_VSYNC_CNT,
		BCHP_FIELD_DATA(VDEC_FE_0_CLEAN_VSYNC_CNT, NOISE_0, 0x24 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_CLEAN_VSYNC_CNT, NOISE_1, 0x24 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_CLEAN_VSYNC_CNT, NOISE_2, 0x24 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_CLEAN_VSYNC_CNT, NOISE_3, 0x24 )
	},

	/* Last Entry */
	{
		UINT32_C(-1),
		UINT32_C(-1)
	}
};

/*PAL-G*/
static const BVDC_P_AddrDataPair s_aVdecFmtTbl_Cvbs_PAL_Vcr[] =
{
	{
		BCHP_VDEC_FE_0_HSYNC_PLL1,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL1, FRO_FREQ,     FIXED  ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL1, AVG_EN,       UPDATED) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL1, FREQ_AVG,     ENABLE ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL1, FILTA_OFFSET, 21     ) |
#endif
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL1, BIG_SMALL_MODE, SMALL) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL1, DIRECT_GAIN,    4    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL1, SMALL_TERM_CNT, 0    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL1, BIG_TERM_CNT,   1    )
	},

	{
		BCHP_VDEC_FE_0_SYNC_DETECT2,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, HEAD_SWITCH_HISTORY_MASK, 0 ) |
#else
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, MATCH_FILTER_TH,      0 ) |
#endif
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, MATCH_REQ_LOCK,       0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, VCR_MODE,             1 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, FREQ_OFFSET,          0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, CONSISTEN_ERROR_TH,   16 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, HEAD_SWITCH_ERROR_TH, 20 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, MATCH_FILTER_MODE,    0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, RAW_VSYNC_MODE,       0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, HEAD_SWITCH_THRESH,   9  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, LINE_OFFSET,          1 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, SLICE_FILT,           4 )
	},

	{
		BCHP_VDEC_FE_0_FRO_CONTROL,
		BCHP_FIELD_DATA(VDEC_FE_0_FRO_CONTROL, AVG_ERR_FACTOR, 2               ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_FRO_CONTROL, AUTO_COEFF,     ONE_AND_QUARTER ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FRO_CONTROL, FIELD_ID_TH,    20              ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FRO_CONTROL, CUT_OFF_TH,     31              ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FRO_CONTROL, ERR_TH,         20              ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FRO_CONTROL, FRO_MODE,       1               )
	},

	{
		BCHP_VDEC_FE_0_CLEAN_VSYNC_CNT,
		BCHP_FIELD_DATA(VDEC_FE_0_CLEAN_VSYNC_CNT, NOISE_0, 0x24 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_CLEAN_VSYNC_CNT, NOISE_1, 0x24 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_CLEAN_VSYNC_CNT, NOISE_2, 0x24 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_CLEAN_VSYNC_CNT, NOISE_3, 0x24 )
	},

	/* Last Entry */
	{
		UINT32_C(-1),
		UINT32_C(-1)
	}
};

/*PAL_Nc*/
static const BVDC_P_AddrDataPair s_aVdecFmtTbl_Cvbs_PAL_Nc_Vcr[] =
{
	{
		BCHP_VDEC_FE_0_HSYNC_PLL1,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL1, FRO_FREQ,     FIXED  ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL1, AVG_EN,       UPDATED) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL1, FREQ_AVG,     ENABLE ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL1, FILTA_OFFSET, 21     ) |
#endif
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL1, BIG_SMALL_MODE, SMALL) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL1, DIRECT_GAIN,    4    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL1, SMALL_TERM_CNT, 0    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL1, BIG_TERM_CNT,   1    )
	},

	{
		BCHP_VDEC_FE_0_SYNC_DETECT2,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, HEAD_SWITCH_HISTORY_MASK, 0 ) |
#else
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, MATCH_FILTER_TH,      0 ) |
#endif
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, MATCH_REQ_LOCK,       0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, VCR_MODE,             1 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, FREQ_OFFSET,          0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, CONSISTEN_ERROR_TH,   16 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, HEAD_SWITCH_ERROR_TH, 20 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, MATCH_FILTER_MODE,    0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, RAW_VSYNC_MODE,       0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, HEAD_SWITCH_THRESH,   9  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, LINE_OFFSET,          1 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, SLICE_FILT,           4 )
	},

	{
		BCHP_VDEC_FE_0_FRO_CONTROL,
		BCHP_FIELD_DATA(VDEC_FE_0_FRO_CONTROL, AVG_ERR_FACTOR, 2               ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_FRO_CONTROL, AUTO_COEFF,     ONE_AND_QUARTER ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FRO_CONTROL, FIELD_ID_TH,    20              ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FRO_CONTROL, CUT_OFF_TH,     31              ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FRO_CONTROL, ERR_TH,         20              ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FRO_CONTROL, FRO_MODE,       1               )
	},

	{
		BCHP_VDEC_FE_0_CLEAN_VSYNC_CNT,
		BCHP_FIELD_DATA(VDEC_FE_0_CLEAN_VSYNC_CNT, NOISE_0, 0x24 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_CLEAN_VSYNC_CNT, NOISE_1, 0x24 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_CLEAN_VSYNC_CNT, NOISE_2, 0x24 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_CLEAN_VSYNC_CNT, NOISE_3, 0x24 )
	},

	/* Last Entry */
	{
		UINT32_C(-1),
		UINT32_C(-1)
	}
};

/*PAL_M*/
static const BVDC_P_AddrDataPair s_aVdecFmtTbl_Cvbs_PAL_M_Vcr[] =
{
	{
		BCHP_VDEC_FE_0_HSYNC_PLL1,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL1, FRO_FREQ,     FIXED  ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL1, AVG_EN,       UPDATED) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL1, FREQ_AVG,     ENABLE ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL1, FILTA_OFFSET, 21     ) |
#endif
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL1, BIG_SMALL_MODE, SMALL) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL1, DIRECT_GAIN,    4    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL1, SMALL_TERM_CNT, 0    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL1, BIG_TERM_CNT,   1    )
	},

	{
		BCHP_VDEC_FE_0_SYNC_DETECT2,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, HEAD_SWITCH_HISTORY_MASK, 0 ) |
#else
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, MATCH_FILTER_TH,      0 ) |
#endif
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, MATCH_REQ_LOCK,       0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, VCR_MODE,             1 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, FREQ_OFFSET,          0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, CONSISTEN_ERROR_TH,   16 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, HEAD_SWITCH_ERROR_TH, 20 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, MATCH_FILTER_MODE,    1 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, RAW_VSYNC_MODE,       0 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, HEAD_SWITCH_THRESH,   9  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, LINE_OFFSET,          4 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, SLICE_FILT,           4 )
	},

	{
		BCHP_VDEC_FE_0_FRO_CONTROL,
		BCHP_FIELD_DATA(VDEC_FE_0_FRO_CONTROL, AVG_ERR_FACTOR, 2               ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_FRO_CONTROL, AUTO_COEFF,     ONE_AND_QUARTER ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FRO_CONTROL, FIELD_ID_TH,    20              ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FRO_CONTROL, CUT_OFF_TH,     31              ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FRO_CONTROL, ERR_TH,         20              ) |
		BCHP_FIELD_DATA(VDEC_FE_0_FRO_CONTROL, FRO_MODE,       1               )
	},

	{
		BCHP_VDEC_FE_0_CLEAN_VSYNC_CNT,
		BCHP_FIELD_DATA(VDEC_FE_0_CLEAN_VSYNC_CNT, NOISE_0, 0x24 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_CLEAN_VSYNC_CNT, NOISE_1, 0x24 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_CLEAN_VSYNC_CNT, NOISE_2, 0x24 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_CLEAN_VSYNC_CNT, NOISE_3, 0x24 )
	},

	/* Last Entry */
	{
		UINT32_C(-1),
		UINT32_C(-1)
	}
};

/****************************************************************************
 * Noisy table for NTSC!
 * TODO: Optimizations needed.
 ****************************************************************************/
static const BVDC_P_AddrDataPair s_aVdecFmtTbl_Cvbs_NTSC_Noisy[] =
{
	{
		BCHP_VDEC_FE_0_SYNC_DETECT,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, DCR_SEL,           SYNC_SEP     ) |
#else
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, HEAD_SWITCH_SEL,   TWO_HEAD ) |
#endif
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, TIP_FILT_SEL,      FILTA    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, TIP_SELECT_MID,    0        ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, SLICE_CTL_DISABLE, 0        ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, MASK_EQ_SYNC,      1        ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, SIGNAL_TO_MONITOR, FILTA    ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, SLICE_MODE,        SLOW_ONLY) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, SLICE_DELAY,       3        ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, SLICE_AVG,         AVG16    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, P_COUNT,           24       ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, H_COUNT,           8        ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, V_COUNT,           24       )
	},

	{
		BCHP_VDEC_FE_0_SYNC_DETECT2,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, HEAD_SWITCH_HISTORY_MASK, 1 ) |
#else
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, MATCH_FILTER_TH,      0 ) |
#endif
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, MATCH_REQ_LOCK,       1  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, VCR_MODE,             1  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, FREQ_OFFSET,          32 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, CONSISTEN_ERROR_TH,   10 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, HEAD_SWITCH_ERROR_TH, 20 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, MATCH_FILTER_MODE,    0  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, RAW_VSYNC_MODE,       1  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, HEAD_SWITCH_THRESH,   9  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, LINE_OFFSET,          4  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, SLICE_FILT,           4  )
	},

	{
		BCHP_VDEC_FE_0_RAW_HSYNC_CONTROL,
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, MATCH_OVERRIDE_MIN, 1  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, SAMPLE_TYPE_CTL,    0  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, MATCH_MIN,          0  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, INT_MIN,            45 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, INT_AREA,           0  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, MATCH_DELTA_COEFF,  12 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, DELTA_COEFF,        8  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, INT_TH_COEFF,       14 )
	},

	{
		BCHP_VDEC_FE_0_HSYNC_PLL1,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL1, FRO_FREQ,     FIXED  ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL1, AVG_EN,       UPDATED) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL1, FREQ_AVG,     ENABLE ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL1, FILTA_OFFSET, 21     ) |
#endif
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL1, BIG_SMALL_MODE, SMALL) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL1, DIRECT_GAIN,    4    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL1, SMALL_TERM_CNT, 0    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL1, BIG_TERM_CNT,   1    )
	},

	/* Last Entry */
	{
		UINT32_C(-1),
		UINT32_C(-1)
	}
};

/****************************************************************************
 * Noisy table for PAL!
 * TODO: Optimizations needed.
 ****************************************************************************/
static const BVDC_P_AddrDataPair s_aVdecFmtTbl_Cvbs_PAL_Noisy[] =
{
	{
		BCHP_VDEC_FE_0_SYNC_DETECT,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, DCR_SEL,           VDEC     ) |
#else
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, HEAD_SWITCH_SEL,   TWO_HEAD ) |
#endif
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, TIP_FILT_SEL,      FILTA    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, TIP_SELECT_MID,    0        ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, SLICE_CTL_DISABLE, 0        ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, MASK_EQ_SYNC,      1        ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, SIGNAL_TO_MONITOR, FILTA    ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, SLICE_MODE,        AUTO) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, SLICE_DELAY,       3        ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, SLICE_AVG,         AVG16    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, P_COUNT,           24       ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, H_COUNT,           8        ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, V_COUNT,           24       )
	},

	{
		BCHP_VDEC_FE_0_SYNC_DETECT2,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, HEAD_SWITCH_HISTORY_MASK, 0 ) |
#else
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, MATCH_FILTER_TH,      0 ) |
#endif
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, MATCH_REQ_LOCK,       1  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, VCR_MODE,             1  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, FREQ_OFFSET,          32 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, CONSISTEN_ERROR_TH,   10 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, HEAD_SWITCH_ERROR_TH, 20 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, MATCH_FILTER_MODE,    0  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, RAW_VSYNC_MODE,       1  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, HEAD_SWITCH_THRESH,   9  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, LINE_OFFSET,          1  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, SLICE_FILT,           4  )
	},

	{
		BCHP_VDEC_FE_0_RAW_HSYNC_CONTROL,
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, MATCH_OVERRIDE_MIN, 1  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, SAMPLE_TYPE_CTL,    0  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, MATCH_MIN,          0  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, INT_MIN,            45 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, INT_AREA,           0  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, MATCH_DELTA_COEFF,  12 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, DELTA_COEFF,        8  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, INT_TH_COEFF,       14 )
	},

	{
		BCHP_VDEC_FE_0_HSYNC_PLL1,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL1, FRO_FREQ,     FIXED  ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL1, AVG_EN,       UPDATED) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL1, FREQ_AVG,     DISABLE) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL1, FILTA_OFFSET, 21     ) |
#endif
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL1, BIG_SMALL_MODE, SMALL) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL1, DIRECT_GAIN,    4    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL1, SMALL_TERM_CNT, 0    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL1, BIG_TERM_CNT,   1    )
	},

	/* Last Entry */
	{
		UINT32_C(-1),
		UINT32_C(-1)
	}
};

/****************************************************************************
 * Noisy table for PAL-M!
 * TODO: Optimizations needed.
 ****************************************************************************/
static const BVDC_P_AddrDataPair s_aVdecFmtTbl_Cvbs_PALM_Noisy[] =
{
	{
		BCHP_VDEC_FE_0_SYNC_DETECT,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, DCR_SEL,           VDEC     ) |
#else
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, HEAD_SWITCH_SEL,   TWO_HEAD ) |
#endif
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, TIP_FILT_SEL,      FILTA    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, TIP_SELECT_MID,    0        ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, SLICE_CTL_DISABLE, 0        ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, MASK_EQ_SYNC,      1        ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, SIGNAL_TO_MONITOR, FILTA    ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, SLICE_MODE,        AUTO) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, SLICE_DELAY,       3        ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_SYNC_DETECT, SLICE_AVG,         AVG16    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, P_COUNT,           24       ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, H_COUNT,           8        ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT, V_COUNT,           24       )
	},

	{
		BCHP_VDEC_FE_0_SYNC_DETECT2,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, HEAD_SWITCH_HISTORY_MASK, 0 ) |
#else
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, MATCH_FILTER_TH,      0 ) |
#endif
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, MATCH_REQ_LOCK,       1  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, VCR_MODE,             1  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, FREQ_OFFSET,          32 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, CONSISTEN_ERROR_TH,   10 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, HEAD_SWITCH_ERROR_TH, 20 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, MATCH_FILTER_MODE,    0  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, RAW_VSYNC_MODE,       1  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, HEAD_SWITCH_THRESH,   9  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, LINE_OFFSET,          4  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_SYNC_DETECT2, SLICE_FILT,           4  )
	},

	{
		BCHP_VDEC_FE_0_RAW_HSYNC_CONTROL,
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, MATCH_OVERRIDE_MIN, 1  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, SAMPLE_TYPE_CTL,    0  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, MATCH_MIN,          0  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, INT_MIN,            45 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, INT_AREA,           0  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, MATCH_DELTA_COEFF,  12 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, DELTA_COEFF,        8  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_RAW_HSYNC_CONTROL, INT_TH_COEFF,       14 )
	},

	{
		BCHP_VDEC_FE_0_HSYNC_PLL1,
#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL1, FRO_FREQ,     FIXED  ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL1, AVG_EN,       UPDATED) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL1, FREQ_AVG,     ENABLE ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL1, FILTA_OFFSET, 21     ) |
#endif
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL1, BIG_SMALL_MODE, SMALL) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL1, DIRECT_GAIN,    4    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL1, SMALL_TERM_CNT, 0    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL1, BIG_TERM_CNT,   1    )
	},

	/* Last Entry */
	{
		UINT32_C(-1),
		UINT32_C(-1)
	}
};

/****************************************************************************
 * PLL table for different noise level and input.
 ****************************************************************************/
/* RF, Normal */
static const BVDC_P_AddrDataPair s_aVdecFmtTbl_Rf_NTSC_Pll[] =
{
	{
		BCHP_VDEC_FE_0_HSYNC_PLL,
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL, BY_PASS_LPF,  0   ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL, ACCUM_THRESH, OFF ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL, LOCK_THRESH,  3   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL, LOCK_WINDOW,  15  )
	},

	{
		BCHP_VDEC_FE_0_HSYNC_PLL_FILTER,
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL_FILTER, NOISE_3, 8    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL_FILTER, NOISE_2, 8    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL_FILTER, NOISE_1, 8    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL_FILTER, NOISE_0, 8    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL_FILTER, UNLOCK,  0x20 )
	},

	{
		BCHP_VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN1,
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN1, FILT_SEL_NOISE_0, FILTB ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN1, FILT_SEL_NOISE_1, FILTA ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN1, NOISE_0,          96    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN1, NOISE_1,          47    )
	},

	{
		BCHP_VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN2,
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN2, FILT_SEL_NOISE_2, FILTA ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN2, FILT_SEL_NOISE_3, FILTA ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN2, NOISE_2,          47    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN2, NOISE_3,          47    )
	},

	{
		BCHP_VDEC_FE_0_BIG_SMALL_TH1,
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH1, SMALL_TO_BIG_TH_NOISE_0, 80  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH1, BIG_TO_SMALL_TH_NOISE_0, 20  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH1, SMALL_TO_BIG_TH_NOISE_1, 120 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH1, BIG_TO_SMALL_TH_NOISE_1, 40  )
	},

	{
		BCHP_VDEC_FE_0_BIG_SMALL_TH2,
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH2, SMALL_TO_BIG_TH_NOISE_2, 120 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH2, BIG_TO_SMALL_TH_NOISE_2, 10  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH2, SMALL_TO_BIG_TH_NOISE_3, 120 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH2, BIG_TO_SMALL_TH_NOISE_3, 10  )
	},

	{
		BCHP_VDEC_FE_0_HSYNC_ACC_GAIN1,
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN1, SMALL_NOISE_0, 74 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN1, BIG_NOISE_0,   21 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN1, SMALL_NOISE_1, 74 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN1, BIG_NOISE_1,   21 )
	},

	{
		BCHP_VDEC_FE_0_HSYNC_ACC_GAIN2,
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN2, SMALL_NOISE_2, 74 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN2, BIG_NOISE_2,   21 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN2, SMALL_NOISE_3, 74 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN2, BIG_NOISE_3,   21 )
	},

	{
		BCHP_VDEC_FE_0_NOISE_BIN_TH,
		BCHP_FIELD_DATA(VDEC_FE_0_NOISE_BIN_TH, HYSTERESIS, 5   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NOISE_BIN_TH, TH_2,       149 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NOISE_BIN_TH, TH_1,       75  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NOISE_BIN_TH, TH_0,       20  )
	},

	/* Last Entry */
	{
		UINT32_C(-1),
		UINT32_C(-1)
	}
};

/* RF, Vcr */
static const BVDC_P_AddrDataPair s_aVdecFmtTbl_Rf_NTSC_Vcr_Pll[] =
{
	{
		BCHP_VDEC_FE_0_HSYNC_PLL,
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL, BY_PASS_LPF,  0   ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL, ACCUM_THRESH, OFF ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL, LOCK_THRESH,  3   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL, LOCK_WINDOW,  15  )
	},

	{
		BCHP_VDEC_FE_0_HSYNC_PLL_FILTER,
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL_FILTER, NOISE_3, 8 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL_FILTER, NOISE_2, 8 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL_FILTER, NOISE_1, 8 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL_FILTER, NOISE_0, 8 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL_FILTER, UNLOCK,  0 )
	},

	{
		BCHP_VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN1,
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN1, FILT_SEL_NOISE_0, FILTB ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN1, FILT_SEL_NOISE_1, FILTB ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN1, NOISE_0,          416   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN1, NOISE_1,          416   )
	},

	{
		BCHP_VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN2,
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN2, FILT_SEL_NOISE_2, FILTA ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN2, FILT_SEL_NOISE_3, FILTA ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN2, NOISE_2,          416   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN2, NOISE_3,          416   )
	},

	{
		BCHP_VDEC_FE_0_BIG_SMALL_TH1,
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH1, SMALL_TO_BIG_TH_NOISE_0, 32 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH1, BIG_TO_SMALL_TH_NOISE_0, 5  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH1, SMALL_TO_BIG_TH_NOISE_1, 32 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH1, BIG_TO_SMALL_TH_NOISE_1, 5  )
	},

	{
		BCHP_VDEC_FE_0_BIG_SMALL_TH2,
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH2, SMALL_TO_BIG_TH_NOISE_2, 32 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH2, BIG_TO_SMALL_TH_NOISE_2, 5  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH2, SMALL_TO_BIG_TH_NOISE_3, 32 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH2, BIG_TO_SMALL_TH_NOISE_3, 5  )
	},

	{
		BCHP_VDEC_FE_0_HSYNC_ACC_GAIN1,
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN1, SMALL_NOISE_0, 127 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN1, BIG_NOISE_0,   0   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN1, SMALL_NOISE_1, 119 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN1, BIG_NOISE_1,   0   )
	},

	{
		BCHP_VDEC_FE_0_HSYNC_ACC_GAIN2,
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN2, SMALL_NOISE_2, 119 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN2, BIG_NOISE_2,   0   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN2, SMALL_NOISE_3, 119 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN2, BIG_NOISE_3,   0   )
	},

	{
		BCHP_VDEC_FE_0_NOISE_BIN_TH,
		BCHP_FIELD_DATA(VDEC_FE_0_NOISE_BIN_TH, HYSTERESIS, 5   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NOISE_BIN_TH, TH_2,       149 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NOISE_BIN_TH, TH_1,       75  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NOISE_BIN_TH, TH_0,       20  )
	},

	/* Last Entry */
	{
		UINT32_C(-1),
		UINT32_C(-1)
	}
};

/* RF, Noisy */
static const BVDC_P_AddrDataPair s_aVdecFmtTbl_Rf_NTSC_Noisy_Pll[] =
{
	{
		BCHP_VDEC_FE_0_HSYNC_PLL,
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL, BY_PASS_LPF,  0   ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL, ACCUM_THRESH, OFF ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL, LOCK_THRESH,  3   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL, LOCK_WINDOW,  15  )
	},

	{
		BCHP_VDEC_FE_0_HSYNC_PLL_FILTER,
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL_FILTER, NOISE_3, 0x8 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL_FILTER, NOISE_2, 0x8 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL_FILTER, NOISE_1, 0x8 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL_FILTER, NOISE_0, 0x8 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL_FILTER, UNLOCK,  0x20)
	},

	{
		BCHP_VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN1,
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN1, FILT_SEL_NOISE_0, FILTA ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN1, FILT_SEL_NOISE_1, FILTA ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN1, NOISE_0,          96    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN1, NOISE_1,          47    )
	},

	{
		BCHP_VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN2,
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN2, FILT_SEL_NOISE_2, FILTA ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN2, FILT_SEL_NOISE_3, FILTA ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN2, NOISE_2,          47    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN2, NOISE_3,          47    )
	},

	{
		BCHP_VDEC_FE_0_BIG_SMALL_TH1,
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH1, SMALL_TO_BIG_TH_NOISE_0, 80  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH1, BIG_TO_SMALL_TH_NOISE_0, 20  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH1, SMALL_TO_BIG_TH_NOISE_1, 120 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH1, BIG_TO_SMALL_TH_NOISE_1, 40  )
	},

	{
		BCHP_VDEC_FE_0_BIG_SMALL_TH2,
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH2, SMALL_TO_BIG_TH_NOISE_2, 120 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH2, BIG_TO_SMALL_TH_NOISE_2, 10  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH2, SMALL_TO_BIG_TH_NOISE_3, 120 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH2, BIG_TO_SMALL_TH_NOISE_3, 10  )
	},

	{
		BCHP_VDEC_FE_0_HSYNC_ACC_GAIN1,
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN1, SMALL_NOISE_0, 74 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN1, BIG_NOISE_0,   21 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN1, SMALL_NOISE_1, 127) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN1, BIG_NOISE_1,   21 )
	},

	{
		BCHP_VDEC_FE_0_HSYNC_ACC_GAIN2,
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN2, SMALL_NOISE_2, 127) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN2, BIG_NOISE_2,   21 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN2, SMALL_NOISE_3, 127) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN2, BIG_NOISE_3,   21 )
	},

	{
		BCHP_VDEC_FE_0_NOISE_BIN_TH,
		BCHP_FIELD_DATA(VDEC_FE_0_NOISE_BIN_TH, HYSTERESIS, 5   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NOISE_BIN_TH, TH_2,       149 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NOISE_BIN_TH, TH_1,       75  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NOISE_BIN_TH, TH_0,       20  )
	},

	/* Last Entry */
	{
		UINT32_C(-1),
		UINT32_C(-1)
	}
};

/* CVBS, Normal */
static const BVDC_P_AddrDataPair s_aVdecFmtTbl_Cvbs_NTSC_Pll[] =
{
	{
		BCHP_VDEC_FE_0_HSYNC_PLL,
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL, BY_PASS_LPF,  0   ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL, ACCUM_THRESH, OFF ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL, LOCK_THRESH,  3   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL, LOCK_WINDOW,  15  )
	},

	{
		BCHP_VDEC_FE_0_HSYNC_PLL_FILTER,
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL_FILTER, NOISE_3, 8    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL_FILTER, NOISE_2, 8    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL_FILTER, NOISE_1, 8    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL_FILTER, NOISE_0, 8    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL_FILTER, UNLOCK,  0x20 )
	},

	{
		BCHP_VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN1,
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN1, FILT_SEL_NOISE_0, FILTB ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN1, FILT_SEL_NOISE_1, FILTA ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN1, NOISE_0,          416   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN1, NOISE_1,          47    )
	},

	{
		BCHP_VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN2,
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN2, FILT_SEL_NOISE_2, FILTA ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN2, FILT_SEL_NOISE_3, FILTA ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN2, NOISE_2,          47    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN2, NOISE_3,          47    )
	},

	{
		BCHP_VDEC_FE_0_BIG_SMALL_TH1,
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH1, SMALL_TO_BIG_TH_NOISE_0, 80  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH1, BIG_TO_SMALL_TH_NOISE_0, 20  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH1, SMALL_TO_BIG_TH_NOISE_1, 120 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH1, BIG_TO_SMALL_TH_NOISE_1, 40  )
	},

	{
		BCHP_VDEC_FE_0_BIG_SMALL_TH2,
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH2, SMALL_TO_BIG_TH_NOISE_2, 120 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH2, BIG_TO_SMALL_TH_NOISE_2, 10  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH2, SMALL_TO_BIG_TH_NOISE_3, 120 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH2, BIG_TO_SMALL_TH_NOISE_3, 10  )
	},

	{
		BCHP_VDEC_FE_0_HSYNC_ACC_GAIN1,
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN1, SMALL_NOISE_0, 74 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN1, BIG_NOISE_0,   21 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN1, SMALL_NOISE_1, 74 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN1, BIG_NOISE_1,   21 )
	},

	{
		BCHP_VDEC_FE_0_HSYNC_ACC_GAIN2,
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN2, SMALL_NOISE_2, 74 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN2, BIG_NOISE_2,   21 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN2, SMALL_NOISE_3, 74 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN2, BIG_NOISE_3,   21 )
	},

	{
		BCHP_VDEC_FE_0_NOISE_BIN_TH,
		BCHP_FIELD_DATA(VDEC_FE_0_NOISE_BIN_TH, HYSTERESIS, 5   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NOISE_BIN_TH, TH_2,       149 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NOISE_BIN_TH, TH_1,       75  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NOISE_BIN_TH, TH_0,       30  )
	},

	/* Last Entry */
	{
		UINT32_C(-1),
		UINT32_C(-1)
	}
};

/* CVBS, Vcr */
static const BVDC_P_AddrDataPair s_aVdecFmtTbl_Cvbs_NTSC_Vcr_Pll[] =
{
	{
		BCHP_VDEC_FE_0_HSYNC_PLL,
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL, BY_PASS_LPF,  0   ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL, ACCUM_THRESH, OFF ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL, LOCK_THRESH,  3   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL, LOCK_WINDOW,  15  )
	},

	{
		BCHP_VDEC_FE_0_HSYNC_PLL_FILTER,
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL_FILTER, NOISE_3, 8 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL_FILTER, NOISE_2, 8 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL_FILTER, NOISE_1, 8 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL_FILTER, NOISE_0, 8 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL_FILTER, UNLOCK,  0 )
	},

	{
		BCHP_VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN1,
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN1, FILT_SEL_NOISE_0, FILTB ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN1, FILT_SEL_NOISE_1, FILTB ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN1, NOISE_0,          416   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN1, NOISE_1,          416   )
	},

	{
		BCHP_VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN2,
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN2, FILT_SEL_NOISE_2, FILTA ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN2, FILT_SEL_NOISE_3, FILTA ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN2, NOISE_2,          416   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN2, NOISE_3,          416   )
	},

	{
		BCHP_VDEC_FE_0_BIG_SMALL_TH1,
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH1, SMALL_TO_BIG_TH_NOISE_0, 32 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH1, BIG_TO_SMALL_TH_NOISE_0, 5  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH1, SMALL_TO_BIG_TH_NOISE_1, 32 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH1, BIG_TO_SMALL_TH_NOISE_1, 5  )
	},

	{
		BCHP_VDEC_FE_0_BIG_SMALL_TH2,
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH2, SMALL_TO_BIG_TH_NOISE_2, 32 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH2, BIG_TO_SMALL_TH_NOISE_2, 5  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH2, SMALL_TO_BIG_TH_NOISE_3, 32 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH2, BIG_TO_SMALL_TH_NOISE_3, 5  )
	},

    {

		BCHP_VDEC_FE_0_HSYNC_ACC_GAIN1,
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN1, SMALL_NOISE_0, 127 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN1, BIG_NOISE_0,   0   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN1, SMALL_NOISE_1, 119 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN1, BIG_NOISE_1,   0   )
	},

    {
		BCHP_VDEC_FE_0_HSYNC_ACC_GAIN2,
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN2, SMALL_NOISE_2, 119 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN2, BIG_NOISE_2,   0   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN2, SMALL_NOISE_3, 119 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN2, BIG_NOISE_3,   0   )
	},

	{
		BCHP_VDEC_FE_0_NOISE_BIN_TH,
		BCHP_FIELD_DATA(VDEC_FE_0_NOISE_BIN_TH, HYSTERESIS, 5   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NOISE_BIN_TH, TH_2,       149 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NOISE_BIN_TH, TH_1,       75  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NOISE_BIN_TH, TH_0,       30  )
	},

	/* Last Entry */
	{
		UINT32_C(-1),
		UINT32_C(-1)
	}
};


/* CVBS, Noisy */
static const BVDC_P_AddrDataPair s_aVdecFmtTbl_Cvbs_NTSC_Noisy_Pll[] =
{
	{
		BCHP_VDEC_FE_0_HSYNC_PLL,
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL, BY_PASS_LPF,  0   ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL, ACCUM_THRESH, OFF ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL, LOCK_THRESH,  3   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL, LOCK_WINDOW,  15  )
	},

	{
		BCHP_VDEC_FE_0_HSYNC_PLL_FILTER,
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL_FILTER, NOISE_3, 0x8 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL_FILTER, NOISE_2, 0x8 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL_FILTER, NOISE_1, 0x8 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL_FILTER, NOISE_0, 0x8 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL_FILTER, UNLOCK,  0x20)
	},

	{
		BCHP_VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN1,
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN1, FILT_SEL_NOISE_0, FILTB ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN1, FILT_SEL_NOISE_1, FILTA ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN1, NOISE_0,          416   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN1, NOISE_1,          47    )
	},

	{
		BCHP_VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN2,
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN2, FILT_SEL_NOISE_2, FILTA ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN2, FILT_SEL_NOISE_3, FILTA ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN2, NOISE_2,          47    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN2, NOISE_3,          47    )
	},

	{
		BCHP_VDEC_FE_0_BIG_SMALL_TH1,
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH1, SMALL_TO_BIG_TH_NOISE_0, 80  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH1, BIG_TO_SMALL_TH_NOISE_0, 20  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH1, SMALL_TO_BIG_TH_NOISE_1, 120 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH1, BIG_TO_SMALL_TH_NOISE_1, 40  )
	},

	{
		BCHP_VDEC_FE_0_BIG_SMALL_TH2,
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH2, SMALL_TO_BIG_TH_NOISE_2, 120 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH2, BIG_TO_SMALL_TH_NOISE_2, 10  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH2, SMALL_TO_BIG_TH_NOISE_3, 120 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH2, BIG_TO_SMALL_TH_NOISE_3, 10  )
	},

	{
		BCHP_VDEC_FE_0_HSYNC_ACC_GAIN1,
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN1, SMALL_NOISE_0, 74 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN1, BIG_NOISE_0,   21 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN1, SMALL_NOISE_1, 127) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN1, BIG_NOISE_1,   21 )
	},

	{
		BCHP_VDEC_FE_0_HSYNC_ACC_GAIN2,
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN2, SMALL_NOISE_2, 127) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN2, BIG_NOISE_2,   21 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN2, SMALL_NOISE_3, 127) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN2, BIG_NOISE_3,   21 )
	},

	{
		BCHP_VDEC_FE_0_NOISE_BIN_TH,
		BCHP_FIELD_DATA(VDEC_FE_0_NOISE_BIN_TH, HYSTERESIS, 5   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NOISE_BIN_TH, TH_2,       149 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NOISE_BIN_TH, TH_1,       75  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NOISE_BIN_TH, TH_0,       30  )
	},

	/* Last Entry */
	{
		UINT32_C(-1),
		UINT32_C(-1)
	}
};


/* Svideo, Normal */
static const BVDC_P_AddrDataPair s_aVdecFmtTbl_Svideo_NTSC_Pll[] =
{
	{
		BCHP_VDEC_FE_0_HSYNC_PLL,
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL, BY_PASS_LPF,  0   ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL, ACCUM_THRESH, OFF ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL, LOCK_THRESH,  3   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL, LOCK_WINDOW,  15  )
	},

	{
		BCHP_VDEC_FE_0_HSYNC_PLL_FILTER,
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL_FILTER, NOISE_3, 48 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL_FILTER, NOISE_2, 48 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL_FILTER, NOISE_1, 48 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL_FILTER, NOISE_0, 48 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL_FILTER, UNLOCK,  48 )
	},

	{
		BCHP_VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN1,
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN1, FILT_SEL_NOISE_0, FILTB ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN1, FILT_SEL_NOISE_1, FILTB ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN1, NOISE_0,          63    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN1, NOISE_1,          63    )
	},

	{
		BCHP_VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN2,
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN2, FILT_SEL_NOISE_2, FILTA ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN2, FILT_SEL_NOISE_3, FILTA ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN2, NOISE_2,          63    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN2, NOISE_3,          63    )
	},

	{
		BCHP_VDEC_FE_0_BIG_SMALL_TH1,
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH1, SMALL_TO_BIG_TH_NOISE_0, 20 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH1, BIG_TO_SMALL_TH_NOISE_0, 5  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH1, SMALL_TO_BIG_TH_NOISE_1, 20 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH1, BIG_TO_SMALL_TH_NOISE_1, 5  )
	},

	{
		BCHP_VDEC_FE_0_BIG_SMALL_TH2,
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH2, SMALL_TO_BIG_TH_NOISE_2, 20 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH2, BIG_TO_SMALL_TH_NOISE_2, 5  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH2, SMALL_TO_BIG_TH_NOISE_3, 20 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH2, BIG_TO_SMALL_TH_NOISE_3, 5  )
	},

	{
		BCHP_VDEC_FE_0_HSYNC_ACC_GAIN1,
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN1, SMALL_NOISE_0, 74 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN1, BIG_NOISE_0,   21 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN1, SMALL_NOISE_1, 74 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN1, BIG_NOISE_1,   21 )
	},

	{
		BCHP_VDEC_FE_0_HSYNC_ACC_GAIN2,
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN2, SMALL_NOISE_2, 74 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN2, BIG_NOISE_2,   21 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN2, SMALL_NOISE_3, 74 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN2, BIG_NOISE_3,   21 )
	},

	{
		BCHP_VDEC_FE_0_NOISE_BIN_TH,
		BCHP_FIELD_DATA(VDEC_FE_0_NOISE_BIN_TH, HYSTERESIS, 5   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NOISE_BIN_TH, TH_2,       149 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NOISE_BIN_TH, TH_1,       75  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NOISE_BIN_TH, TH_0,       30  )
	},

	/* Last Entry */
	{
		UINT32_C(-1),
		UINT32_C(-1)
	}
};



/* Svideo, Vcr */
static const BVDC_P_AddrDataPair s_aVdecFmtTbl_Svideo_NTSC_Vcr_Pll[] =
{
	{
		BCHP_VDEC_FE_0_HSYNC_PLL,
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL, BY_PASS_LPF,  0   ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL, ACCUM_THRESH, OFF ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL, LOCK_THRESH,  3   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL, LOCK_WINDOW,  15  )
	},

	{
		BCHP_VDEC_FE_0_HSYNC_PLL_FILTER,
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL_FILTER, NOISE_3, 8 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL_FILTER, NOISE_2, 8 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL_FILTER, NOISE_1, 8 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL_FILTER, NOISE_0, 8 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL_FILTER, UNLOCK,  0 )
	},

	{
		BCHP_VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN1,
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN1, FILT_SEL_NOISE_0, FILTB ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN1, FILT_SEL_NOISE_1, FILTB ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN1, NOISE_0,          416   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN1, NOISE_1,          416   )
	},

	{
		BCHP_VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN2,
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN2, FILT_SEL_NOISE_2, FILTA ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN2, FILT_SEL_NOISE_3, FILTA ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN2, NOISE_2,          416   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN2, NOISE_3,          416   )
	},

	{
		BCHP_VDEC_FE_0_BIG_SMALL_TH1,
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH1, SMALL_TO_BIG_TH_NOISE_0, 32 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH1, BIG_TO_SMALL_TH_NOISE_0, 5  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH1, SMALL_TO_BIG_TH_NOISE_1, 32 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH1, BIG_TO_SMALL_TH_NOISE_1, 5  )
	},

	{
		BCHP_VDEC_FE_0_BIG_SMALL_TH2,
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH2, SMALL_TO_BIG_TH_NOISE_2, 32 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH2, BIG_TO_SMALL_TH_NOISE_2, 5  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH2, SMALL_TO_BIG_TH_NOISE_3, 32 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH2, BIG_TO_SMALL_TH_NOISE_3, 5  )
	},

	{
		BCHP_VDEC_FE_0_HSYNC_ACC_GAIN1,
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN1, SMALL_NOISE_0, 127 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN1, BIG_NOISE_0,   0   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN1, SMALL_NOISE_1, 119 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN1, BIG_NOISE_1,   0   )
	},

	{
		BCHP_VDEC_FE_0_HSYNC_ACC_GAIN2,
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN2, SMALL_NOISE_2, 119 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN2, BIG_NOISE_2,   0   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN2, SMALL_NOISE_3, 119 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN2, BIG_NOISE_3,   0   )
	},

	{
		BCHP_VDEC_FE_0_NOISE_BIN_TH,
		BCHP_FIELD_DATA(VDEC_FE_0_NOISE_BIN_TH, HYSTERESIS, 5   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NOISE_BIN_TH, TH_2,       149 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NOISE_BIN_TH, TH_1,       75  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NOISE_BIN_TH, TH_0,       30  )
	},

	/* Last Entry */
	{
		UINT32_C(-1),
		UINT32_C(-1)
	}
};


/* Svideo, Noisy */
static const BVDC_P_AddrDataPair s_aVdecFmtTbl_Svideo_NTSC_Noisy_Pll[] =
{
	{
		BCHP_VDEC_FE_0_HSYNC_PLL,
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL, BY_PASS_LPF,  0   ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HSYNC_PLL, ACCUM_THRESH, OFF ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL, LOCK_THRESH,  3   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL, LOCK_WINDOW,  15  )
	},

	{
		BCHP_VDEC_FE_0_HSYNC_PLL_FILTER,
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL_FILTER, NOISE_3, 0x8 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL_FILTER, NOISE_2, 0x8 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL_FILTER, NOISE_1, 0x8 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL_FILTER, NOISE_0, 0x8 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_PLL_FILTER, UNLOCK,  0x20)
	},

	{
		BCHP_VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN1,
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN1, FILT_SEL_NOISE_0, FILTB ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN1, FILT_SEL_NOISE_1, FILTB ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN1, NOISE_0,          63    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN1, NOISE_1,          63    )
	},

	{
		BCHP_VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN2,
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN2, FILT_SEL_NOISE_2, FILTA ) |
		BCHP_FIELD_ENUM(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN2, FILT_SEL_NOISE_3, FILTA ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN2, NOISE_2,          63    ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HPLL_ONE_OVER_LPF_GAIN2, NOISE_3,          63    )
	},

	{
		BCHP_VDEC_FE_0_BIG_SMALL_TH1,
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH1, SMALL_TO_BIG_TH_NOISE_0, 20 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH1, BIG_TO_SMALL_TH_NOISE_0, 5  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH1, SMALL_TO_BIG_TH_NOISE_1, 20 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH1, BIG_TO_SMALL_TH_NOISE_1, 5  )
	},

	{
		BCHP_VDEC_FE_0_BIG_SMALL_TH2,
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH2, SMALL_TO_BIG_TH_NOISE_2, 20 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH2, BIG_TO_SMALL_TH_NOISE_2, 5  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH2, SMALL_TO_BIG_TH_NOISE_3, 20 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_BIG_SMALL_TH2, BIG_TO_SMALL_TH_NOISE_3, 5  )
	},

	{
		BCHP_VDEC_FE_0_HSYNC_ACC_GAIN1,
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN1, SMALL_NOISE_0, 74 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN1, BIG_NOISE_0,   21 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN1, SMALL_NOISE_1, 127) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN1, BIG_NOISE_1,   21 )
	},

	{
		BCHP_VDEC_FE_0_HSYNC_ACC_GAIN2,
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN2, SMALL_NOISE_2, 127) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN2, BIG_NOISE_2,   21 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN2, SMALL_NOISE_3, 127) |
		BCHP_FIELD_DATA(VDEC_FE_0_HSYNC_ACC_GAIN2, BIG_NOISE_3,   21 )
	},

	{
		BCHP_VDEC_FE_0_NOISE_BIN_TH,
		BCHP_FIELD_DATA(VDEC_FE_0_NOISE_BIN_TH, HYSTERESIS, 5   ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NOISE_BIN_TH, TH_2,       149 ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NOISE_BIN_TH, TH_1,       75  ) |
		BCHP_FIELD_DATA(VDEC_FE_0_NOISE_BIN_TH, TH_0,       30  )
	},

	/* Last Entry */
	{
		UINT32_C(-1),
		UINT32_C(-1)
	}
};

/****************************************************************************/
/* Making an entry for FMT of vdec                                          */
/****************************************************************************/
#define BVDC_P_MAKE_FMT(fmt, type, table)                                   \
{                                                                           \
	BFMT_VideoFmt_e##fmt,                                                   \
	BVDC_P_VdecInput_e##type,                                               \
	(table)                                                                 \
}

#define BVDC_P_MAKE_FMT_ENTRY(index, name, pointer)                         \
{                                                                           \
	index,                                                                  \
	name,                                                                   \
	pointer                                                                 \
}

/* Master table for looking up VDEC fmt settings base on format; and
 * input type. */
static const BVDC_P_VdecFmtTbl s_aVdecFmtTbl[] =
{
	/* PC formats */
	BVDC_P_MAKE_FMT(DVI_640x480p, PcIn,   s_aVdecFmtTbl_PcIn       ),

	/* ATSC formats */
	BVDC_P_MAKE_FMT(NTSC,         Ifd,    s_aVdecFmtTbl_Rf_NTSC    ),
	BVDC_P_MAKE_FMT(NTSC,         Cvbs,   s_aVdecFmtTbl_Cvbs_NTSC  ),
	BVDC_P_MAKE_FMT(NTSC,         Svideo, s_aVdecFmtTbl_Svideo_NTSC),
	BVDC_P_MAKE_FMT(NTSC,         YPrPb,  s_aVdecFmtTbl_YPrPb_NTSC ),

	BVDC_P_MAKE_FMT(NTSC_J,       Ifd,    s_aVdecFmtTbl_Rf_NTSC    ),
	BVDC_P_MAKE_FMT(NTSC_J,       Cvbs,   s_aVdecFmtTbl_Cvbs_NTSC  ),
	BVDC_P_MAKE_FMT(NTSC_J,       Svideo, s_aVdecFmtTbl_Svideo_NTSC),
	BVDC_P_MAKE_FMT(NTSC_J,       YPrPb,  s_aVdecFmtTbl_YPrPb_NTSC ),

	BVDC_P_MAKE_FMT(NTSC_443,     Ifd,    s_aVdecFmtTbl_Rf_NTSC    ),
	BVDC_P_MAKE_FMT(NTSC_443,     Cvbs,   s_aVdecFmtTbl_Cvbs_NTSC  ),
	BVDC_P_MAKE_FMT(NTSC_443,     Svideo, s_aVdecFmtTbl_Svideo_NTSC),
	BVDC_P_MAKE_FMT(NTSC_443,     YPrPb,  s_aVdecFmtTbl_YPrPb_NTSC ),

	BVDC_P_MAKE_FMT(PAL_60,       Ifd,    s_aVdecFmtTbl_Cvbs_PALM  ),
	BVDC_P_MAKE_FMT(PAL_60,       Cvbs,   s_aVdecFmtTbl_Cvbs_PALM  ),
	BVDC_P_MAKE_FMT(PAL_60,       Svideo, s_aVdecFmtTbl_Cvbs_PALM  ),
	BVDC_P_MAKE_FMT(PAL_60,       YPrPb,  s_aVdecFmtTbl_Cvbs_PALM  ),

	BVDC_P_MAKE_FMT(PAL_G,        Ifd,    s_aVdecFmtTbl_Rf_PAL     ),
	BVDC_P_MAKE_FMT(PAL_G,        Cvbs,   s_aVdecFmtTbl_Cvbs_PAL   ),
	BVDC_P_MAKE_FMT(PAL_G,        Svideo, s_aVdecFmtTbl_Svideo_PAL ),
	BVDC_P_MAKE_FMT(PAL_G,        YPrPb,  s_aVdecFmtTbl_Cvbs_PAL   ),

	BVDC_P_MAKE_FMT(PAL_M,        Ifd,    s_aVdecFmtTbl_Cvbs_PALM  ),
	BVDC_P_MAKE_FMT(PAL_M,        Cvbs,   s_aVdecFmtTbl_Cvbs_PALM  ),
	BVDC_P_MAKE_FMT(PAL_M,        Svideo, s_aVdecFmtTbl_Svideo_PALM),
	BVDC_P_MAKE_FMT(PAL_M,        YPrPb,  s_aVdecFmtTbl_Cvbs_PALM  ),

	BVDC_P_MAKE_FMT(PAL_NC,       Ifd,    s_aVdecFmtTbl_Cvbs_PAL_Nc),
	BVDC_P_MAKE_FMT(PAL_NC,       Cvbs,   s_aVdecFmtTbl_Cvbs_PAL_Nc),
	BVDC_P_MAKE_FMT(PAL_NC,       Svideo, s_aVdecFmtTbl_Svideo_PAL ),
	BVDC_P_MAKE_FMT(PAL_NC,       YPrPb,  s_aVdecFmtTbl_Cvbs_PAL_Nc),

	BVDC_P_MAKE_FMT(SECAM,        Ifd,    s_aVdecFmtTbl_Cvbs_SECAM ),
	BVDC_P_MAKE_FMT(SECAM,        Cvbs,   s_aVdecFmtTbl_Cvbs_SECAM ),
	BVDC_P_MAKE_FMT(SECAM,        Svideo, s_aVdecFmtTbl_Cvbs_SECAM ),
	BVDC_P_MAKE_FMT(SECAM,        YPrPb,  s_aVdecFmtTbl_Cvbs_SECAM ),

	BVDC_P_MAKE_FMT(480p,         YPrPb,  s_aVdecFmtTbl_YPrPb_480p ),
	BVDC_P_MAKE_FMT(576p_50Hz,    YPrPb,  s_aVdecFmtTbl_YPrPb_576p ),
	BVDC_P_MAKE_FMT(720p,         YPrPb,  s_aVdecFmtTbl_YPrPb_720p ),
	BVDC_P_MAKE_FMT(720p_50Hz,    YPrPb,  s_aVdecFmtTbl_YPrPb_720p ),
	BVDC_P_MAKE_FMT(1080i,        YPrPb,  s_aVdecFmtTbl_YPrPb_1080i),
	BVDC_P_MAKE_FMT(1080i_50Hz,   YPrPb,  s_aVdecFmtTbl_YPrPb_1080i),
	BVDC_P_MAKE_FMT(1080p,        YPrPb,  s_aVdecFmtTbl_YPrPb_1080p),
	BVDC_P_MAKE_FMT(1080p_24Hz,   YPrPb,  s_aVdecFmtTbl_YPrPb_1080p),
	BVDC_P_MAKE_FMT(1080p_25Hz,   YPrPb,  s_aVdecFmtTbl_YPrPb_1080p),
	BVDC_P_MAKE_FMT(1080p_30Hz,   YPrPb,  s_aVdecFmtTbl_YPrPb_1080p),
	BVDC_P_MAKE_FMT(1080p_50Hz,   YPrPb,  s_aVdecFmtTbl_YPrPb_1080p),

	/* Must be last. */
	BVDC_P_MAKE_FMT(MaxCount, Unknown, NULL)
};

/* Master table for looking up VDEC fmt settings based on noise level; and
 * input type. */
static const BVDC_P_VdecFmtTbl s_aVdecFmtTbl_Pll[][BVDC_P_Vdec_NoiseLevel_eUnknown] =
{
	/* normal */
	{
		BVDC_P_MAKE_FMT(MaxCount,   Ifd,      s_aVdecFmtTbl_Rf_NTSC_Pll           ),
		BVDC_P_MAKE_FMT(MaxCount,   Cvbs,     s_aVdecFmtTbl_Cvbs_NTSC_Pll         ),
		BVDC_P_MAKE_FMT(MaxCount,   Svideo,   s_aVdecFmtTbl_Svideo_NTSC_Pll       ),
	},
	/* Vcr */
	{
		BVDC_P_MAKE_FMT(MaxCount,   Ifd,      s_aVdecFmtTbl_Rf_NTSC_Vcr_Pll       ),
		BVDC_P_MAKE_FMT(MaxCount,   Cvbs,     s_aVdecFmtTbl_Cvbs_NTSC_Vcr_Pll     ),
		BVDC_P_MAKE_FMT(MaxCount,   Svideo,   s_aVdecFmtTbl_Svideo_NTSC_Vcr_Pll   ),
	},
	/* Noisy */
	{
		BVDC_P_MAKE_FMT(MaxCount,   Ifd,      s_aVdecFmtTbl_Rf_NTSC_Noisy_Pll     ),
		BVDC_P_MAKE_FMT(MaxCount,   Cvbs,     s_aVdecFmtTbl_Cvbs_NTSC_Noisy_Pll   ),
		BVDC_P_MAKE_FMT(MaxCount,   Svideo,   s_aVdecFmtTbl_Svideo_NTSC_Noisy_Pll )
	}
};

static BVDC_P_VdecFmtTbl_Entry s_aVdecFmtTbl_Entry[] =
{
	BVDC_P_MAKE_FMT_ENTRY(0,    "Normal",       NULL),  /* setting for normal mode. */
	BVDC_P_MAKE_FMT_ENTRY(1,    "VCR or Noise", NULL),  /* setting for VCR or Noisy mode. */
	BVDC_P_MAKE_FMT_ENTRY(2,    "PLL",          NULL),  /* setting for 9 PLL registers. */

	/* Must be last. */
	BVDC_P_MAKE_FMT_ENTRY(UINT32_C(-1), "End",  NULL)   /* end entry. */
};


/***************************************************************************
 *
 */
void BVDC_P_Vdec_GetFormatTable_isr
	( const BVDC_P_Vdec_Handle         hVdec,
	  const BFMT_VideoInfo            *pFmtInfo,
	  BVDC_P_VdecInput                 eInputType,
	  const BVDC_P_VdecFmtTbl_Entry  **ppFmtEntry )
{

	const BVDC_P_AddrDataPair *pADPair;
	const BVDC_P_AddrDataPair *pADPairVcr;
	const BVDC_P_AddrDataPair *pADPairPll;
	BVDC_P_Vdec_NoiseLevel eNoiseLevel;

	uint32_t i = 0;

	BDBG_OBJECT_ASSERT(hVdec, BVDC_DEC);

	/* Initialized! */
	pADPair = pADPairVcr = pADPairPll = NULL;
	eNoiseLevel = BVDC_P_Vdec_NoiseLevel_eNormal;

	/* Look for FT table. */
	/* 1: Normal table*/
	while(BFMT_VideoFmt_eMaxCount != s_aVdecFmtTbl[i].eFmt)
	{
		/* PC Input uses the same one for all format! */
		if(((s_aVdecFmtTbl[i].eInputType == eInputType) &&
		    (s_aVdecFmtTbl[i].eFmt == pFmtInfo->eVideoFmt)) ||
		   ((s_aVdecFmtTbl[i].eInputType == eInputType) &&
		    (BVDC_P_VdecInput_ePcIn == eInputType)))
		{
			if(s_aVdecFmtTbl[i].pFmtTbl)
			{
				pADPair = s_aVdecFmtTbl[i].pFmtTbl;
				break;
			}
			else
			{
				BDBG_WRN(("%s not yet supported by vdec.", pFmtInfo->pchFormatStr));
			}
		}
		i++;
	}

	/* 2: VCR, Noise table*/
	if(hVdec->bVcrHeadSwitch)
	{
		/* VCR table */
		if(VIDEO_FORMAT_IS_NTSC(pFmtInfo->eVideoFmt))
		{
			pADPairVcr = s_aVdecFmtTbl_Cvbs_NTSC_Vcr;
		}
		else if((BFMT_VideoFmt_ePAL_G == pFmtInfo->eVideoFmt) ||
		        (VIDEO_FORMAT_IS_SECAM(pFmtInfo->eVideoFmt)))
		{
			pADPairVcr = s_aVdecFmtTbl_Cvbs_PAL_Vcr;
		}
		else if(BFMT_VideoFmt_ePAL_NC == pFmtInfo->eVideoFmt)
		{
			pADPairVcr = s_aVdecFmtTbl_Cvbs_PAL_Nc_Vcr;
		}
		else
		{
			pADPairVcr = s_aVdecFmtTbl_Cvbs_PAL_M_Vcr;
		}
		eNoiseLevel = BVDC_P_Vdec_NoiseLevel_eVcr;
	}
	else if(hVdec->bNoisy)
	{
		/* Noisy table */
		if(VIDEO_FORMAT_IS_NTSC(pFmtInfo->eVideoFmt))
		{
			pADPairVcr = s_aVdecFmtTbl_Cvbs_NTSC_Noisy;
		}
		else if(pFmtInfo->eVideoFmt == BFMT_VideoFmt_ePAL_M)
		{
			pADPairVcr = s_aVdecFmtTbl_Cvbs_PALM_Noisy;
		}
		else if((VIDEO_FORMAT_IS_PAL(pFmtInfo->eVideoFmt)) ||
		        (VIDEO_FORMAT_IS_SECAM(pFmtInfo->eVideoFmt)))
		{
			pADPairVcr = s_aVdecFmtTbl_Cvbs_PAL_Noisy;
		}
		eNoiseLevel = BVDC_P_Vdec_NoiseLevel_eNoisy;
	}

	/* 3: HPLL table*/
	if( eInputType > BVDC_P_VdecInput_eSvideo ||  eNoiseLevel > BVDC_P_Vdec_NoiseLevel_eNoisy)
	{
		pADPairPll = NULL;
	}
	else
	{
		pADPairPll = s_aVdecFmtTbl_Pll[eNoiseLevel][eInputType].pFmtTbl;
	}

	s_aVdecFmtTbl_Entry[0].pFmtTbl = pADPair;
	s_aVdecFmtTbl_Entry[1].pFmtTbl = pADPairVcr;
	s_aVdecFmtTbl_Entry[2].pFmtTbl = pADPairPll;

	*ppFmtEntry = s_aVdecFmtTbl_Entry;
	return;
}
#endif /* BVDC_P_SUPPORT_VDEC */

/* End of file */
