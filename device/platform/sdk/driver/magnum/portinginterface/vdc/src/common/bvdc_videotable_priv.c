/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvdc_videotable_priv.c $
 * $brcm_Revision: Hydra_Software_Devel/63 $
 * $brcm_Date: 3/8/12 11:51a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vdc/7038/bvdc_videotable_priv.c $
 * 
 * Hydra_Software_Devel/63   3/8/12 11:51a yuxiaz
 * SW7425-2546: Fixed warning msg.
 * 
 * Hydra_Software_Devel/62   6/24/11 3:49p albertl
 * SW7420-1966: Cleaned up and reorganized colorspace enums and indicies
 * in display, vdec, and cmp.
 * 
 * Hydra_Software_Devel/61   4/20/11 3:09p tdo
 * SW7425-365: Remove compiler warnings with new kernel build
 *
 * Hydra_Software_Devel/60   10/4/10 3:27p yuxiaz
 * SW7405-4849: Fixed Klocwork issue.
 *
 * Hydra_Software_Devel/59   1/12/10 8:43p albertl
 * SW3548-2700: Changed RGB attenuation to use DVO matrix as YCbCr->RGB
 * matrix and Matrix C as RGB->YCbCr matrix.
 *
 * Hydra_Software_Devel/58   10/27/09 10:52p albertl
 * SW7420-30: Changed mosaic matrices to be loaded statically with xvYcc
 * matrix loaded dynamically as necessary.
 *
 * Hydra_Software_Devel/57   1/31/09 3:30a albertl
 * PR45798: Fixed bugs and enabled mosaic mode csc implementation.
 *
 * Hydra_Software_Devel/56   10/23/08 5:51p albertl
 * PR47318, PR47814: Fixed overflow when calculating RGB attenuation
 * offsets.  Rewrote portions of RGB attenuation code and fixed various
 * offset related issues.
 *
 * Hydra_Software_Devel/55   10/2/08 5:53p albertl
 * PR45808: Updated CSC tables and code to use unified 8-bit to 8-bit
 * coefficients and offsets.  Added support for custom panels.
 *
 * Hydra_Software_Devel/54   9/19/08 7:12p albertl
 * PR39163: Added panel csc tables that use source white points for proper
 * CCB functionality.
 *
 * Hydra_Software_Devel/53   8/13/08 6:19p pntruong
 * PR45632: [PQ] CSC matrix coefficients for HD need mods to match demo
 * display.
 *
 * Hydra_Software_Devel/52   5/21/08 1:55p albertl
 * PR39163:  Updated CMP csc tables to use 10 bit offset values.
 * Corrected to use proper csc tables in conversions involving ntsc, pal,
 * and smpte 240M.  Moved colorclip from window to compositor and changed
 * BVDC_Window_Set/GetColorClip to BVDC_Compositor_Set/GetColorClip.
 *
 * Hydra_Software_Devel/51   5/15/08 3:12p albertl
 * PR39163:  Changed all matrices to use YCbCr and RGB output format.
 * Updated non-linear matrices to include scale and regain factors.
 *
 * Hydra_Software_Devel/50   4/24/08 8:46p albertl
 * PR39163:  Added temporary hack to set win csc to identity due to black
 * cmp screen.
 *
 * Hydra_Software_Devel/49   3/31/08 6:32p albertl
 * PR39163:  Updated non-linear xvYCC tables to use correct data.
 *
 * Hydra_Software_Devel/48   3/27/08 5:08p albertl
 * PR39163:  Implemented non-linear xvYCC csc support for 3548.
 *
 * Hydra_Software_Devel/47   1/14/08 8:39p albertl
 * PR38150:  xvYCC conversion tables changed to use post-multiplied
 * inverse xvYCC matrix.
 *
 * Hydra_Software_Devel/46   1/13/08 3:30a albertl
 * PR38150: Added 7440 HDMI xvYCC CSC support.
 *
 * Hydra_Software_Devel/45   10/30/07 7:27p pntruong
 * PR34239: Allow dynamically loading of vec custom timing.
 *
 * Hydra_Software_Devel/44   9/20/07 1:21a albertl
 * PR35135:  Cleaned up color space matrices and changed them to use same
 * macro system.  Added color space conversion functionality to graphics
 * windows.
 *
 * Hydra_Software_Devel/42   12/18/06 11:29p pntruong
 * PR22577: Merged back to mainline.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/1   9/19/06 1:47p pntruong
 * PR23222, PR23225: VDEC - ATSC & PC input support.
 *
 * Hydra_Software_Devel/41   9/14/06 8:40p albertl
 * PR20874:  Added dual had core support for 3563:
 *
 * Hydra_Software_Devel/40   8/18/06 4:27p albertl
 * PR23117:  Added 7440 support.
 *
 * Hydra_Software_Devel/39   8/7/06 5:37p pntruong
 * PR22577: Initial bringup of VDC.
 *
 * Hydra_Software_Devel/38   6/14/06 1:44p syang
 * PR 21689: add support for 7118
 *
 * Hydra_Software_Devel/37   5/31/06 5:07p yuxiaz
 * PR21892: 7401 B0 bring up.
 *
 * Hydra_Software_Devel/36   5/19/06 1:49p pntruong
 * PR21560: Global symbols without BVDC prefix in the BVDC module.  Reduce
 * unnecessary global exported symbols.  Tighten const params.
 *
 * Hydra_Software_Devel/35   4/14/06 4:29p pntruong
 * PR20753: Used idientity matrix for unknown colorspace, and fixed color.
 *
 * Hydra_Software_Devel/34   3/7/06 4:18p syang
 * PR 19670: added 7438 support
 *
 * Hydra_Software_Devel/33   2/15/06 4:07p syang
 * PR 19403: add two missing  space with #elif
 *
 * Hydra_Software_Devel/32   2/15/06 11:47a syang
 * PR 19403: use S3.10 to replace the mis-leading 4.10 in the comment
 *
 * Hydra_Software_Devel/31   2/10/06 6:05p syang
 * PR 19403: corrected color-matching-color-space-conversion matrix
 * problem due to SMPTE 170 RGB to BT709 RGB matrix error.
 *
 * Hydra_Software_Devel/30   2/1/06 5:45p pntruong
 * PR17778: Added colorspace conversion support for 3560B0.
 *
 * Hydra_Software_Devel/29   1/17/06 4:19p hongtaoz
 * PR19082: first compiled for 7400;
 *
 * Hydra_Software_Devel/28   9/7/05 3:24p jasonh
 * PR 16967: Removed unused define.
 *
 * Hydra_Software_Devel/27   4/26/05 9:37a jasonh
 * PR14798: Updated one single remaining difference. Matches spreadsheet
 * version 1.12
 *
 * Hydra_Software_Devel/26   4/25/05 5:42p hongtaoz
 * PR14798: minor update again with two matrices;
 *
 * Hydra_Software_Devel/25   4/25/05 2:50p hongtaoz
 * PR14702: minor update with some matrices to comply with architecture
 * page;
 *
 * Hydra_Software_Devel/24   3/24/05 4:20p hongtaoz
 * PR14521: update matrices for 3560 without truncation compensation;
 *
 * Hydra_Software_Devel/23   3/18/05 6:31p pntruong
 * PR14494: Basic vdc up and running.  Background and passed output format
 * switch test.
 *
 * Hydra_Software_Devel/22   11/18/04 4:20p albertl
 * PR 12496, PR 12497, PR 12498:  Added contrast, brightness, saturation,
 * and hue adjustment functions.
 *
 * Hydra_Software_Devel/21   11/1/04 2:23p hongtaoz
 * PR13124: added an API to enable/disable color primaries matching for
 * color space conversion process;
 *
 * Hydra_Software_Devel/20   10/8/04 2:57p hongtaoz
 * PR12745: use identity matrix for color space conversion between NTSC
 * and PAL since we don't do gamma correction between them;
 *
 * Hydra_Software_Devel/19   10/1/04 7:30p hongtaoz
 * PR12745: re-organized compositor CSC tables;
 *
 * Hydra_Software_Devel/18   8/6/04 2:12p jasonh
 * PR 12208: Removed scale coefficients placed them in bvdc_coeff_priv.c
 *
 * Hydra_Software_Devel/17   7/16/04 6:54p hongtaoz
 * PR11922: added scaling coeff table entry for 240p -> 480i;
 *
 * Hydra_Software_Devel/16   7/6/04 8:35a pntruong
 * PR9957: Prevent create/destroy from effecting hardware in VDC.
 *
 * Hydra_Software_Devel/15   5/24/04 5:11p jasonh
 * PR 11189: Merge down from B0 to main-line
 *
 * Hydra_Software_Devel/Refsw_Devel_7038_B0/1   4/29/04 7:30p hongtaoz
 * PR10852: simplified the color space conversion process;
 * PR9996: supported multi-standard color space conversion at compositor
 * with color primaries matching;
 * PR8761: fixed part of C++ compiling errors;
 *
 * Hydra_Software_Devel/14   4/1/04 2:25p hongtaoz
 * PR9996: add color space conversion for different video surfaces in
 * compositor.
 *
 * Hydra_Software_Devel/13   12/20/03 12:45p pntruong
 * PR9054:  Added 2-tap filter coeffs.
 *
 * Hydra_Software_Devel/12   12/19/03 2:12p pntruong
 * PR9054: Added support to select between different tap mode filtering.
 *
 * Hydra_Software_Devel/11   12/10/03 10:41a pntruong
 * PR 8841: Added addtional down scaled coefficients.
 *
 * Hydra_Software_Devel/10   12/8/03 9:50p pntruong
 * PR 8841:  Fixed progressive output.  Added scale up coefficients.
 *
 * Hydra_Software_Devel/9   11/13/03 6:05p pntruong
 * Fixed 1to1 table.
 *
 * Hydra_Software_Devel/8   11/13/03 1:37p pntruong
 * Added 1to1 coeffs table.
 *
 * Hydra_Software_Devel/7   11/5/03 1:06p pntruong
 * Added ipc resample coeffs and luma/chroma gain/threshold coeffs.
 *
 * Hydra_Software_Devel/6   10/20/03 11:24a pntruong
 * Removed warnings.
 *
 * Hydra_Software_Devel/5   10/8/03 6:03p pntruong
 * Added a 2to1 table.
 *
 * Hydra_Software_Devel/4   9/19/03 4:48p pntruong
 * Removed linux warnings.
 *
 * Hydra_Software_Devel/3   9/12/03 5:22p pntruong
 * Fir coefficient tables are selected base on scaling down ratio.
 *
 * Hydra_Software_Devel/2   9/11/03 6:35p pntruong
 * Added stub coefftables and luma chrom gain lut.
 *
 * Hydra_Software_Devel/1   8/28/03 7:50p pntruong
 * Coefficient tables for scaler and other miscellaneous lookup tables.
 *
 ***************************************************************************/
#include "bstd.h"
#include "bvdc_scaler_priv.h"
#include "bvdc_window_priv.h"
#include "bvdc_displayfmt_priv.h"
#include "bvdc_csc_priv.h"

BDBG_MODULE(BVDC_CSC);

/***************************************************************************
 * video surface color space conversion matrices Set I
 * Note:
 *  - these csc tables match the color primaries of different standards;
 *  - we do not do non-linear gamma correction;
 *  - the CMP matrix is for [YCbCr] -> [YCbCr];
 *  - PR10417: coeffs are in two's complement fixed point format;
 * -----------------------------------------------------
 * Color Space Standards:
 *  - ITU-R BT.709, i.e. ATSC HD or PAL HD;
 *  - FCC, i.e. NTSC SD 1953 standard, with ITU-R BT.470-2 System M color primaries;
 *  - ITU-R BT.470-2 System B, G, i.e. PAL SD;
 *  - SMPTE 170M, i.e. modern NTSC SD;
 *  - SMPTE 240M (1987 standard), the legacy ATSC HD;
 **************************************************************************/

/* Group 0: xyz -> BT.709 (i.e. HD) */
/* SMPTE 170M (i.e. modern SD NTSC) -> BT.709 (i.e. HD) */
static const BVDC_P_CscCoeffs  s_CMP_NtscSDYCbCr_to_HDYCbCr = BVDC_P_MAKE_CMP_CSC
	(  1.000000, -0.086918, -0.198602,  36.546531,
	   0.000000,  1.008912,  0.109928, -15.211527,
	  -0.000000,  0.057004,  0.942791,   0.026243 );

/* BT.470-2 System B, G (i.e. SD Pal) -> BT.709 (i.e. HD) */
static const BVDC_P_CscCoeffs  s_CMP_PalSDYCbCr_to_HDYCbCr = BVDC_P_MAKE_CMP_CSC
	(  1.000000, -0.115168, -0.189312,  38.973396,
	   0.000000,  1.004499,  0.099809, -13.351437,
	  -0.000000,  0.084468,  1.072557, -20.099241 );

/* SMPTE 240M (i.e. 1987 ATSC HD) -> BT.709 (i.e. HD) */
static const BVDC_P_CscCoeffs  s_CMP_240MHDYCbCr_to_HDYCbCr = BVDC_P_MAKE_CMP_CSC
	(  1.000000,  0.000000, -0.000000,  0.000000,
	  -0.000000,  0.990940, -0.000253,  1.192039,
	  -0.000000,  0.004734,  0.924649,  9.039049 );

/* FCC (i.e. 1953 NTSC) -> BT.709 (i.e. HD) */
static const BVDC_P_CscCoeffs  s_CMP_FccSDYCbCr_to_HDYCbCr = BVDC_P_MAKE_CMP_CSC
	(  1.000000, -0.000000,  0.000000,   0.0000000,
	   0.054988,  1.121448, -0.005106, -15.771562,
	   0.033634, -0.011863,  1.511262, -64.461172 );

/* Group 1: xyz -> SMPTE 170M (i.e. modern SD NTSC) */
/* BT.709 (i.e. HD) -> SMPTE 170M (i.e. modern SD NTSC) */
static const BVDC_P_CscCoeffs  s_CMP_HDYCbCr_to_NtscSDYCbCr = BVDC_P_MAKE_CMP_CSC
	(  1.000000,  0.074740,  0.201939, -35.414914,
	  -0.000000,  0.997740, -0.116335,  15.180202,
	   0.000000, -0.060327,  1.067715,  -0.945678 );

/* BT.470-2 System B, G (i.e. SD Pal) -> SMPTE 170M (i.e. modern SD NTSC) */
static const BVDC_P_CscCoeffs  s_CMP_PalSDYCbCr_to_NtscSDYCbCr = BVDC_P_MAKE_CMP_CSC
	(  1.000000, -0.023033,  0.034738,  -1.498223,
	  -0.000000,  0.992402, -0.025193,   4.197192,
	  -0.000000,  0.029590,  1.139164, -21.600485 );

/* SMPTE 240M (i.e. 1987 ATSC HD) -> SMPTE 170M (i.e. modern SD NTSC) */
static const BVDC_P_CscCoeffs  s_CMP_240MHDYCbCr_to_NtscSDYCbCr = BVDC_P_MAKE_CMP_CSC
	(  1.000000,  0.075019,  0.186703, -33.500488,
	  -0.000000,  0.988150, -0.107822,  15.317986,
	  -0.000000, -0.054726,  0.987276,   8.633535 );

/* FCC (i.e. 1953 NTSC) -> SMPTE 170M (i.e. modern SD NTSC) */
static const BVDC_P_CscCoeffs  s_CMP_FccSDYCbCr_to_NtscSDYCbCr = BVDC_P_MAKE_CMP_CSC
	(  1.010902,  0.081422,  0.304801, -49.610885,
	   0.050951,  1.120293, -0.180907,   6.943399,
	   0.032594, -0.080320,  1.613905, -68.820370 );

/* Group 2: xyz -> BT.470-2 System B, G (i.e. SD Pal) */
/* SMPTE 170M (i.e. modern SD NTSC) -> BT.470-2 System B, G (i.e. SD Pal) */
static const BVDC_P_CscCoeffs  s_CMP_NtscSDYCbCr_to_PalSDYCbCr = BVDC_P_MAKE_CMP_CSC
	(  1.000000,  0.024103, -0.029962,   0.749873,
	   0.000000,  1.006992,  0.022270,  -3.745506,
	   0.000000, -0.026157,  0.877258,  19.058994 );

/* BT.709 (i.e. HD) -> BT.470-2 System B, G (i.e. SD Pal) */
static const BVDC_P_CscCoeffs  s_CMP_HDYCbCr_to_PalSDYCbCr = BVDC_P_MAKE_CMP_CSC
	(  1.000000,  0.100597,  0.167144, -34.270816,
	  -0.000000,  1.003373, -0.093371,  11.519780,
	   0.000000, -0.079020,  0.939705,  17.832324 );

/* SMPTE 240M (i.e. 1987 ATSC HD) -> BT.470-2 System B, G (i.e. SD Pal) */
static const BVDC_P_CscCoeffs  s_CMP_240MHDYCbCr_to_PalSDYCbCr = BVDC_P_MAKE_CMP_CSC
	(  1.000000,  0.100476,  0.154524, -32.640077,
	  -0.000000,  0.993840, -0.086589,  11.871852,
	   0.000000, -0.073855,  0.868917,  26.232166 );

/* FCC (i.e. 1953 NTSC) -> BT.470-2 System B, G (i.e. SD Pal) */
static const BVDC_P_CscCoeffs  s_CMP_FccSDYCbCr_to_PalSDYCbCr = BVDC_P_MAKE_CMP_CSC
	(  1.011153,  0.110831,  0.252085, -46.631688,
	   0.052033,  1.126338, -0.146231,   1.713843,
	   0.027261, -0.099765,  1.420544, -41.495877 );

/* BT.709 (i.e. HD) -> to xvYCC BT.601 (i.e. SD) */
static const BVDC_P_CscCoeffs  s_CMP_HDYCbCr_to_XvYCCSDYCbCr = BVDC_P_MAKE_CMP_CSC
	(  1.000000,  0.100178,  0.191707, -37.361314,
	   0.000000,  0.989849, -0.110711,  15.470362,
	  -0.000000, -0.073079,  0.983251,  11.497916 );

/* FCC (i.e. 1953 NTSC) -> to xvYCC BT.601 (i.e. SD) */
static const BVDC_P_CscCoeffs  s_CMP_FccSDYCbCr_to_XvYCCSDYCbCr = BVDC_P_MAKE_CMP_CSC
	(  1.011956,  0.110070,  0.289208, -51.298935,
	   0.050706,  1.111377, -0.172368,   6.995471,
	   0.029052, -0.093619,  1.486324, -50.731057 );

/* BT.470-2 System B, G (i.e. SD Pal) -> xvYCC BT.601 (i.e. SD) */
static const BVDC_P_CscCoeffs  s_CMP_PalSDYCbCr_to_XvYCCSDYCbCr = BVDC_P_MAKE_CMP_CSC
	(  1.000000,  0.001655,  0.026303, -3.578607,
	   0.000000,  0.984950, -0.019948,  4.479666,
	  -0.000000,  0.009646,  1.047299, -7.288983 );

/* SMPTE 170M (i.e. modern SD NTSC)) -> xvYCC BT.601 (i.e. SD) */
static const BVDC_P_CscCoeffs  s_CMP_NtscSDYCbCr_to_XvYCCSDYCbCr = BVDC_P_MAKE_CMP_CSC
	(  1.000000,  0.025081, -0.006850,  -2.333618,
	   0.000000,  0.992359,  0.004435,   0.410342,
	  -0.000000, -0.017681,  0.918967,  12.635360 );

/* SMPTE 240M (i.e. 1987 ATSC HD) -> xvYCC BT.601 (i.e. SD) */
static const BVDC_P_CscCoeffs  s_CMP_240MHDYCbCr_to_XvYCCSDYCbCr = BVDC_P_MAKE_CMP_CSC
	(  1.000000,  0.100178,  0.177236, -35.509050,
	  -0.000000,  0.980357, -0.102619,  15.649576,
	  -0.000000, -0.067762,  0.909181,  20.298460 );

/* xvYCC BT.601 (i.e. SD) -> to BT.709 (i.e. HD) */
static const BVDC_P_CscCoeffs  s_CMP_XvYCCSDYCbCr_to_HDYCbCr = BVDC_P_MAKE_CMP_CSC
	(  1.000000, -0.116569, -0.208098,  41.557373,
	   0.000000,  1.018724,  0.114705, -17.078894,
	  -0.000000,  0.075715,  1.025559, -12.963136 );

/* xvYCC BT.601 (i.e. SD) -> SMPTE 170M (i.e. modern SD NTSC)) */
static const BVDC_P_CscCoeffs  s_CMP_XvYCCSDYCbCr_to_NtscSDYCbCr = BVDC_P_MAKE_CMP_CSC
	(  1.000000, -0.025140,  0.007575,   2.248217,
	  -0.000000,  1.007613, -0.004863,  -0.352023,
	   0.000000,  0.019386,  1.088085, -13.756298 );

/* xvYCC BT.601 (i.e. SD) -> BT.470-2 System B, G (i.e. SD Pal) */
static const BVDC_P_CscCoeffs  s_CMP_XvYCCSDYCbCr_to_PalSDYCbCr = BVDC_P_MAKE_CMP_CSC
	(  1.000000, -0.001434, -0.025143,  3.401765,
	   0.000000,  1.015090,  0.019334, -4.406337,
	   0.000000, -0.009349,  0.954659,  7.000373 );

/* identity matrix */
static const BVDC_P_CscCoeffs s_CMP_Identity_YCbCr_to_YCbCr = BVDC_P_MAKE_CMP_CSC
	(  1.000000,  0.000000,  0.000000,  0.000000,
	   0.000000,  1.000000,  0.000000,  0.000000,
	   0.000000,  0.000000,  1.000000,  0.000000 );


/***************************************************************************
 * video surface color space conversion matrices Set II
 * Note:
 *  - these csc matrices DON'T MATCH the color primaries of different standards;
 *  - we do not do non-linear gamma correction;
 *  - the CMP matrix is for [YCbCr] -> [YCbCr];
 *  - PR10417: coeffs are in two's complement fixed point format;
 * -----------------------------------------------------
 * Color Space Standards:
 *  - ITU-R BT.709, i.e. ATSC HD or PAL HD;
 *  - FCC, i.e. NTSC SD 1953 standard, with ITU-R BT.470-2 System M color primaries;
 *  - ITU-R BT.470-2 System B, G, i.e. PAL SD;
 *  - SMPTE 170M, i.e. modern NTSC SD;
 *  - SMPTE 240M (1987 standard), the legacy ATSC HD;
 **************************************************************************/

/* Group 0: xyz -> BT.709 (i.e. HD) */
/* SMPTE 170M (i.e. modern SD NTSC) -> BT.709 (i.e. HD) */
static const BVDC_P_CscCoeffs  s_CMP_NtscSDYCbCr_to_HDYCbCr_II = BVDC_P_MAKE_CMP_CSC
	(  1.000000, -0.116569, -0.208098,  41.557373,
	   0.000000,  1.018724,  0.114705, -17.078894,
	   0.000000,  0.075715,  1.025559, -12.963136 );

/* BT.470-2 System B, G (i.e. SD Pal) -> BT.709 (i.e. HD),
   the same as  SMPTE 170M (i.e. modern SD NTSC) -> BT.709 (i.e. HD) */

/* SMPTE 240M (i.e. 1987 ATSC HD) -> BT.709 (i.e. HD) */
static const BVDC_P_CscCoeffs  s_CMP_240MHDYCbCr_to_HDYCbCr_II = BVDC_P_MAKE_CMP_CSC
	(  1.000000, -0.028780, -0.006178,  4.474660,
	  -0.000000,  1.000374,  0.003405, -0.483788,
	  -0.000000,  0.018694,  1.004346, -2.949129 );

/* FCC (i.e. 1953 NTSC) -> BT.709 (i.e. HD),
   the same as  SMPTE 170M (i.e. modern SD NTSC) -> BT.709 (i.e. HD) */

/* Group 1: xyz -> SMPTE 170M (i.e. modern SD NTSC) */
/* BT.709 (i.e. HD) -> SMPTE 170M (i.e. modern SD NTSC) */
static const BVDC_P_CscCoeffs  s_CMP_HDYCbCr_to_NtscSDYCbCr_II = BVDC_P_MAKE_CMP_CSC
	(  1.000000,  0.100178,  0.191707, -37.361314,
	   0.000000,  0.989849, -0.110711,  15.470362,
	   0.000000, -0.073079,  0.983251,  11.497916 );

/* BT.470-2 System B, G (i.e. SD Pal) -> SMPTE 170M (i.e. modern SD NTSC),
   the same as  SMPTE 170M (i.e. modern SD NTSC) -> SMPTE 170M (i.e. modern SD NTSC) */

/* SMPTE 240M (i.e. 1987 ATSC HD) -> SMPTE 170M (i.e. modern SD NTSC) */
static const BVDC_P_CscCoeffs  s_CMP_240MHDYCbCr_to_NtscSDYCbCr_II = BVDC_P_MAKE_CMP_CSC
	(  1.000000,  0.075019,  0.186703, -33.500488,
	  -0.000000,  0.988150, -0.107822,  15.317986,
	  -0.000000, -0.054726,  0.987276,   8.633535 );

/* FCC (i.e. 1953 NTSC) -> SMPTE 170M (i.e. modern SD NTSC),
   the same as  SMPTE 170M (i.e. modern SD NTSC) -> SMPTE 170M (i.e. modern SD NTSC) */

/* Group 2: xyz -> BT.470-2 System B, G (i.e. SD Pal) */
/* xyz -> BT.709 (i.e. HD),
   the same as xyz -> BT.470-2 System B, G (i.e. SD Pal) */


/***************************************************************************
 * video surface color space conversion non-linear matrices Set A
 * Note:
 *  - the CMP matrix A is for non-linear [YCbCr] -> [Rgb];
 *  - PR10417: coeffs are in two's complement fixed point format;
 * -----------------------------------------------------
 * Color Space Standards:
 *  - ITU-R BT.709, i.e. ATSC HD or PAL HD;
 *  - FCC, i.e. NTSC SD 1953 standard, with ITU-R BT.470-2 System M color primaries;
 *  - ITU-R BT.470-2 System B, G, i.e. PAL SD;
 *  - SMPTE 170M, i.e. modern NTSC SD;
 *  - SMPTE 240M (1987 standard), the legacy ATSC HD;
 **************************************************************************/

/* BT.709 (i.e. HD) YCbCr -> RGB Primaries */
static const BVDC_P_CscCoeffs s_CMP_HDYCbCr_to_RGBPrim = BVDC_P_MAKE_CMP_CSC
	(  1.168950,  0.000000,  1.799682, -249.062527,
	   1.168950, -0.214073, -0.535094,   77.190243,
	   1.168950,  2.120703,  0.000000, -290.153216 );

/* FCC (i.e. 1953 NTSC) -> RGB Primaries,
   the same as  SMPTE 170M (i.e. modern SD NTSC) */

/* BT.470-2 System B, G (i.e. SD Pal) -> RGB Primaries,
   the same as  SMPTE 170M (i.e. modern SD NTSC) */

/* SMPTE 170M (i.e. modern SD NTSC) YCbCr -> RGB Primaries */
static const BVDC_P_CscCoeffs s_CMP_NtscSDYCbCr_to_RGBPrim = BVDC_P_MAKE_CMP_CSC
	(  1.168950,  0.000000,  1.602425, -223.813572,
	   1.168950, -0.394860, -0.816582,  136.361359,
	   1.168950,  2.024147, -0.000000, -277.794001 );

/* SMPTE 240M (i.e. 1987 ATSC HD) -> RGB Primaries */
static const BVDC_P_CscCoeffs s_CMP_240MHDYCbCr_to_RGBPrim = BVDC_P_MAKE_CMP_CSC
	(  1.168950,  0.000000,  1.800283, -249.139370,
	   1.168950, -0.257799, -0.545371,   84.102524,
	   1.168950,  2.087854,  0.000000, -285.948536 );


/***************************************************************************
 * video surface color space conversion non-linear matrices Set B
 * Note:
 *  - these csc tables match the color primaries of different standards;
 *  - the CMP matrix B is for linear [Rgb] -> [Rgb];
 *  - PR10417: coeffs are in two's complement fixed point format;
 * -----------------------------------------------------
 * Color Space Standards:
 *  - ITU-R BT.709, i.e. ATSC HD or PAL HD;
 *  - FCC, i.e. NTSC SD 1953 standard, with ITU-R BT.470-2 System M color primaries;
 *  - ITU-R BT.470-2 System B, G, i.e. PAL SD;
 *  - SMPTE 170M, i.e. modern NTSC SD;
 *  - SMPTE 240M (1987 standard), the legacy ATSC HD;
 *  - Custom Panel, using source white points for proper functionality of CCB;
 **************************************************************************/

/* Group 0: xyz -> BT.709 (i.e. HD) */
/* SMPTE 170M (i.e. modern SD NTSC) -> BT.709 (i.e. HD) */
static const BVDC_P_CscCoeffs s_CMP_BT601RGBPrim_to_BT709RGBPrim = BVDC_P_MAKE_CMP_CSC
	(  0.939542,  0.050181,  0.010277,  0.000000,
	   0.017772,  0.965793,  0.016435,  0.000000,
	  -0.001622, -0.004370,  1.005991,  0.000000 );

/* BT.470-2 System B, G (i.e. SD Pal) -> BT.709 (i.e. HD) */
static const BVDC_P_CscCoeffs s_CMP_PalSDRGBPrim_to_BT709RGBPrim = BVDC_P_MAKE_CMP_CSC
	(  1.044043, -0.044043, -0.000000,  0.000000,
	  -0.000000,  1.000000,  0.000000,  0.000000,
	   0.000000,  0.011793,  0.988207,  0.000000 );

/* FCC (i.e. 1953 NTSC) -> BT.709 (i.e. HD) */
static const BVDC_P_CscCoeffs s_CMP_FccSDRGBPrim_to_BT709RGBPrim = BVDC_P_MAKE_CMP_CSC
	(  1.507481, -0.372488, -0.083211,  0.000000,
	  -0.027470,  0.935064,  0.066940,  0.000000,
	  -0.027213, -0.040141,  1.167113,  0.000000 );

/* Group 1: xyz -> SMPTE 170M (i.e. modern SD NTSC) */
/* BT.709 (i.e. HD) -> SMPTE 170M (i.e. modern SD NTSC) */
static const BVDC_P_CscCoeffs s_CMP_BT709RGBPrim_to_BT601RGBPrim = BVDC_P_MAKE_CMP_CSC
	(  1.065379, -0.055401, -0.009978,  0.000000,
	  -0.019633,  1.036363, -0.016731,  0.000000,
	   0.001632,  0.004412,  0.993956,  0.000000 );

/* BT.470-2 System B, G (i.e. SD Pal) -> SMPTE 170M (i.e. modern SD NTSC) */
static const BVDC_P_CscCoeffs s_CMP_PalSDRGBPrim_to_BT601RGBPrim = BVDC_P_MAKE_CMP_CSC
	(  1.112302, -0.102441, -0.009860,  0.000000,
	  -0.020497,  1.037030, -0.016533,  0.000000,
	   0.001704,  0.016063,  0.982233,  0.000000 );

/* FCC (i.e. 1953 NTSC) -> SMPTE 170M (i.e. modern SD NTSC) */
static const BVDC_P_CscCoeffs s_CMP_FccSDRGBPrim_to_BT601RGBPrim = BVDC_P_MAKE_CMP_CSC
	(  1.607832, -0.448244, -0.104005,  0.000000,
	  -0.057609,  0.977050,  0.051481,  0.000000,
	  -0.024709, -0.036381,  1.160218,  0.000000 );

/* Group 2: xyz -> BT.470-2 System B, G (i.e. SD Pal) */
/* BT.709 (i.e. HD) -> BT.470-2 System B, G (i.e. SD Pal) */
static const BVDC_P_CscCoeffs s_CMP_BT709RGBPrim_to_PalSDRGBPrim = BVDC_P_MAKE_CMP_CSC
	(  0.957815,  0.042185,  0.000000,  0.000000,
	   0.000000,  1.000000,  0.000000,  0.000000,
	   0.000000, -0.011934,  1.011934,  0.000000 );

/* SMPTE 170M (i.e. modern SD NTSC) -> BT.470-2 System B, G (i.e. SD Pal) */
static const BVDC_P_CscCoeffs s_CMP_BT601RGBPrim_to_PalSDRGBPrim = BVDC_P_MAKE_CMP_CSC
	(  0.900657,  0.088807,  0.010536,  0.000000,
	   0.017772,  0.965793,  0.016435,  0.000000,
	  -0.001853, -0.015948,  1.017801,  0.000000 );

/* FCC (i.e. 1953 NTSC) -> BT.470-2 System B, G (i.e. SD Pal) */
static const BVDC_P_CscCoeffs s_CMP_FccSDRGBPrim_to_PalSDRGBPrim = BVDC_P_MAKE_CMP_CSC
	(  1.442728, -0.317329, -0.076877,  0.000000,
	  -0.027470,  0.935064,  0.066940,  0.000000,
	  -0.027210, -0.051780,  1.180242,  0.000000 );

/* Group 3: xyz -> Custom Panel, using source white points.
   Can be replaced with panel specific matrices.  */
/* BT.709 (i.e. HD) -> Custom Panel, using BT.709 white points */
static const BVDC_P_CscCoeffs s_CMP_BT709RGBPrim_to_CustomRGBPrim = BVDC_P_MAKE_CMP_CSC
	(  0.850089,  0.145323,  0.004589,  0.000000,
	   0.007660,  0.973440,  0.018900,  0.000000,
	  -0.005524,  0.031098,  0.974426,  0.000000 );

/* SMPTE 170M (i.e. modern SD NTSC) -> Custom Panel, using SMPTE 170M white points */
static const BVDC_P_CscCoeffs s_CMP_BT601RGBPrim_to_CustomRGBPrim = BVDC_P_MAKE_CMP_CSC
	(  0.801269,  0.182990,  0.015740,  0.000000,
	   0.024466,  0.940444,  0.035090,  0.000000,
	  -0.006217,  0.025499,  0.980719,  0.000000 );

/* BT.470-2 System B, G (i.e. SD Pal) -> Custom Panel, using BT.470-2 white points */
static const BVDC_P_CscCoeffs s_CMP_PalSDRGBPrim_to_CustomRGBPrim = BVDC_P_MAKE_CMP_CSC
	(  0.887529,  0.107936,  0.004534,  0.000000,
	   0.007997,  0.973326,  0.018677,  0.000000,
	  -0.005767,  0.042833,  0.962934,  0.000000 );

/* FCC (i.e. 1953 NTSC) -> Custom Panel, using FCC white points */
static const BVDC_P_CscCoeffs s_CMP_FccSDRGBPrim_to_CustomRGBPrim = BVDC_P_MAKE_CMP_CSC
	(  1.227330, -0.173857, -0.053473,  0.000000,
	  -0.016069,  0.927493,  0.088576,  0.000000,
	  -0.032567, -0.007279,  1.039847,  0.000000 );

/* identity matrix */
#define s_CMP_Identity_RGBPrim_to_RGBPrim s_CMP_Identity_YCbCr_to_YCbCr


/***************************************************************************
 * video surface color space conversion non-linear matrices Set C
 * Note:
 *  - the CMP matrix C is for non-linear [Rgb] -> [YCbCr];
 *  - PR10417: coeffs are in two's complement fixed point format;
 * -----------------------------------------------------
 * Color Space Standards:
 *  - ITU-R BT.709, i.e. ATSC HD or PAL HD;
 *  - ITU-R BT.470-2 System B, G, i.e. PAL SD;
 *  - SMPTE 170M, i.e. modern NTSC SD;
 *  - Custom Panel, with source white points for proper functionality of CCB;
 **************************************************************************/

/* RGB Primaries -> BT.709 (i.e. HD) YCbCr */
static const BVDC_P_CscCoeffs s_CMP_RGBPrim_to_HDYCbCr = BVDC_P_MAKE_CMP_CSC
	(  0.181906,  0.611804,  0.061758,   16.000000,
	  -0.100268, -0.337232,  0.437500,  128.000000,
	   0.437500, -0.397386, -0.040114,  128.000000 );

/* RGB Primaries -> SMPTE 170M (i.e. modern SD NTSC) YCbCr */
static const BVDC_P_CscCoeffs s_CMP_RGBPrim_to_NtscSDYCbCr = BVDC_P_MAKE_CMP_CSC
	(  0.255733,  0.501839,  0.097896,   16.000000,
	  -0.147686, -0.289814,  0.437500,  128.000000,
	   0.437500, -0.366086, -0.071414,  128.000000 );

/* RGB Primaries -> BT.470-2 System B, G (i.e. SD Pal) YCbCr,
   the same as  SMPTE 170M (i.e. modern SD NTSC) */

/* MC matrices for Custom Panels, using source white points,
   can be replaced with panel specific matrices. */
/* RGB Primaries -> Custom Panel YCbCr, using BT.709 white points */
static const BVDC_P_CscCoeffs s_CMP_RGBPrim_to_CustomYCbCr_HDWp = BVDC_P_MAKE_CMP_CSC
	(  0.208948,  0.595680,  0.050842,   16.000000,
	  -0.113611, -0.323889,  0.437500,  128.000000,
	   0.437500, -0.403096, -0.034404,  128.000000 );

/* RGB Primaries -> Custom Panel YCbCr, using SMPTE 170M (i.e. modern SD NTSC) white points */
static const BVDC_P_CscCoeffs s_CMP_RGBPrim_to_CustomYCbCr_NtscWp = BVDC_P_MAKE_CMP_CSC
	(  0.217468,  0.582272,  0.055729,   16.000000,
	  -0.118966, -0.318534,  0.437500,  128.000000,
	   0.437500, -0.399285, -0.038215,  128.000000 );


const BVDC_P_MatrixCoefficients s_aCMP_MosaicCscType_To_ClrSpace_MapTbl[] =
{
	BVDC_P_MatrixCoefficients_eItu_R_BT_709,       /* BVDC_P_MosaicCscType_eItu_R_BT_709,      */
	BVDC_P_MatrixCoefficients_eFcc,                /* BVDC_P_MosaicCscType_eFCC,               */
	BVDC_P_MatrixCoefficients_eItu_R_BT_470_2_BG,  /* BVDC_P_MosaicCscType_eItu_R_BT_470_2_BG, */
	BVDC_P_MatrixCoefficients_eSmpte_170M,         /* BVDC_P_MosaicCscType_eSmpte_170M,        */
	BVDC_P_MatrixCoefficients_eSmpte_240M,         /* BVDC_P_MosaicCscType_eSmpte_240M,        */
	BVDC_P_MatrixCoefficients_eXvYcc_601           /* BVDC_P_MosaicCscType_eXvYCC_601,         */
};


const BVDC_P_CmpColorSpace s_aCMP_ClrSpace_To_MosaicCscType_MapTbl[] =
{
	BVDC_P_CmpColorSpace_eInvalid,                 /* BVDC_P_MatrixCoefficients_eHdmi_RGB           */
	BVDC_P_CmpColorSpace_eHdYCrCb,                 /* BVDC_P_MatrixCoefficients_eItu_R_BT_709       */
	BVDC_P_CmpColorSpace_eInvalid,                 /* BVDC_P_MatrixCoefficients_eUnknown            */
	BVDC_P_CmpColorSpace_eInvalid,                 /* BVDC_P_MatrixCoefficients_eDvi_Full_Range_RGB */
	BVDC_P_CmpColorSpace_eFcc,                     /* BVDC_P_MatrixCoefficients_eFcc                */
	BVDC_P_CmpColorSpace_ePalSdYCrCb,              /* BVDC_P_MatrixCoefficients_eItu_R_BT_470_2_BG  */
	BVDC_P_CmpColorSpace_eNtscSdYCrCb,             /* BVDC_P_MatrixCoefficients_eSmpte_170M         */
	BVDC_P_CmpColorSpace_eSmpte_240M,              /* BVDC_P_MatrixCoefficients_eSmpte_240M         */
	BVDC_P_CmpColorSpace_eHdYCrCb,                 /* BVDC_P_MatrixCoefficients_eXvYcc_709          */
	BVDC_P_CmpColorSpace_eXvYcc_601                /* BVDC_P_MatrixCoefficients_eXvYcc_601          */
};


/****************************** Set One *********************************/
/* the compositor color matrices table WITH Color Primaries Matching */
static const BVDC_P_CscCoeffs *const s_aaCMP_YCbCr_MatrixTbl[][4] =
{
	/* BVDC_P_MatrixCoefficients_eHdmi_RGB = 0 */
	{
		NULL,
	},

	/* BVDC_P_MatrixCoefficients_eItu_R_BT_709 = 1, */
	{
		&s_CMP_Identity_YCbCr_to_YCbCr,    /* HD -> HD, XvYCC_HD */
		&s_CMP_HDYCbCr_to_NtscSDYCbCr,     /* HD -> NTSC */
		&s_CMP_HDYCbCr_to_PalSDYCbCr,      /* HD -> PAL */
		&s_CMP_HDYCbCr_to_XvYCCSDYCbCr     /* HD -> XvYCC_SD */
	},

	/* BVDC_P_MatrixCoefficients_eUnknown = 2 */
	{
		&s_CMP_Identity_YCbCr_to_YCbCr,    /* Identity */
		&s_CMP_Identity_YCbCr_to_YCbCr,    /* Identity */
		&s_CMP_Identity_YCbCr_to_YCbCr,    /* Identity */
		&s_CMP_Identity_YCbCr_to_YCbCr     /* Identity */
	},

	/* forbidden 3 */
	{
		NULL,
	},

	/* BVDC_P_MatrixCoefficients_eFCC = 4 */
	{
		&s_CMP_FccSDYCbCr_to_HDYCbCr,       /* FCC -> HD, XvYCC_HD */
		&s_CMP_FccSDYCbCr_to_NtscSDYCbCr,   /* FCC -> NTSC */
		&s_CMP_FccSDYCbCr_to_PalSDYCbCr,    /* FCC -> PAL */
		&s_CMP_FccSDYCbCr_to_XvYCCSDYCbCr   /* FCC -> XvYCC_SD */
	},

	/* BVDC_P_MatrixCoefficients_eItu_R_BT_470_2_BG = 5 */
	{
		&s_CMP_PalSDYCbCr_to_HDYCbCr,       /* PAL -> HD, XvYCC_HD */
		&s_CMP_PalSDYCbCr_to_NtscSDYCbCr,   /* PAL -> NTSC */
		&s_CMP_Identity_YCbCr_to_YCbCr,     /* PAL -> PAL */
		&s_CMP_PalSDYCbCr_to_XvYCCSDYCbCr   /* PAL -> XvYCC_SD */
	},

	/* BVDC_P_MatrixCoefficients_eSmpte_170M = 6 */
	{
		&s_CMP_NtscSDYCbCr_to_HDYCbCr,      /* NTSC -> HD, XvYCC_HD */
		&s_CMP_Identity_YCbCr_to_YCbCr,     /* NTSC -> NTSC */
		&s_CMP_NtscSDYCbCr_to_PalSDYCbCr,   /* NTSC -> PAL */
		&s_CMP_NtscSDYCbCr_to_XvYCCSDYCbCr  /* NTSC -> XvYCC_SD */
	},

	/* BVDC_P_MatrixCoefficients_eSmpte_240M = 7 */
	{
		&s_CMP_240MHDYCbCr_to_HDYCbCr,      /* 240M -> HD, XvYCC_HD */
		&s_CMP_240MHDYCbCr_to_NtscSDYCbCr,  /* 240M -> NTSC */
		&s_CMP_240MHDYCbCr_to_PalSDYCbCr,   /* 240M -> PAL */
		&s_CMP_240MHDYCbCr_to_XvYCCSDYCbCr  /* 240M -> XvYCC_SD */
	},

	/* BVDC_P_MatrixCoefficients_eXvYCC_709 = 8 */
	{
		&s_CMP_Identity_YCbCr_to_YCbCr,    /* HD -> HD, XvYCC_HD */
		&s_CMP_HDYCbCr_to_NtscSDYCbCr,     /* HD -> NTSC */
		&s_CMP_HDYCbCr_to_PalSDYCbCr,      /* HD -> PAL */
		&s_CMP_HDYCbCr_to_XvYCCSDYCbCr     /* HD -> XvYCC_SD */
	},

	/* BVDC_P_MatrixCoefficients_eXvYCC_601 = 9 */
	{
		&s_CMP_XvYCCSDYCbCr_to_HDYCbCr,     /* XvYCC_SD -> HD, XvYCC_HD */
		&s_CMP_XvYCCSDYCbCr_to_NtscSDYCbCr, /* XvYCC_SD -> NTSC */
		&s_CMP_XvYCCSDYCbCr_to_PalSDYCbCr,  /* XvYCC_SD -> PAL */
		&s_CMP_Identity_YCbCr_to_YCbCr      /* XvYCC_SD -> XvYCC_SD */
	}
};


/****************************** Set Two *********************************/
/* the compositor color matrices table WITHOUT Color Primaries Matching */
static const BVDC_P_CscCoeffs * const s_aaCMP_YCbCr_MatrixTbl_II[][4] =
{
	/* BVDC_P_MatrixCoefficients_eHdmi_RGB = 0 */
	{
		NULL,
	},

	/* BVDC_P_MatrixCoefficients_eItu_R_BT_709 = 1 */
	{
		&s_CMP_Identity_YCbCr_to_YCbCr,     /* HD -> HD */
		&s_CMP_HDYCbCr_to_NtscSDYCbCr_II,   /* HD -> NTSC */
		&s_CMP_HDYCbCr_to_NtscSDYCbCr_II,   /* HD -> PAL */
		&s_CMP_HDYCbCr_to_NtscSDYCbCr_II,   /* HD -> xvYCC_SD */
	},

	/* BVDC_P_MatrixCoefficients_eUnknown = 2 */
	{
		&s_CMP_Identity_YCbCr_to_YCbCr,     /* Identity */
		&s_CMP_Identity_YCbCr_to_YCbCr,     /* Identity */
		&s_CMP_Identity_YCbCr_to_YCbCr,     /* Identity */
		&s_CMP_Identity_YCbCr_to_YCbCr,     /* Identity */
	},

	/* forbidden 3 */
	{
		NULL,
	},

	/* BVDC_P_MatrixCoefficients_eFCC = 4 */
	{
		&s_CMP_NtscSDYCbCr_to_HDYCbCr_II,    /* FCC -> HD */
		&s_CMP_Identity_YCbCr_to_YCbCr,      /* FCC -> NTSC */
		&s_CMP_Identity_YCbCr_to_YCbCr,      /* FCC -> PAL */
		&s_CMP_Identity_YCbCr_to_YCbCr,      /* FCC -> xvYCC_SD */
	},

	/* BVDC_P_MatrixCoefficients_eItu_R_BT_470_2_BG = 5 */
	{
		&s_CMP_NtscSDYCbCr_to_HDYCbCr_II,    /* PAL -> HD */
		&s_CMP_Identity_YCbCr_to_YCbCr,      /* PAL -> NTSC */
		&s_CMP_Identity_YCbCr_to_YCbCr,      /* PAL -> PAL */
		&s_CMP_Identity_YCbCr_to_YCbCr,      /* PAL -> xvYCC_SD */
	},

	/* BVDC_P_MatrixCoefficients_eSmpte_170M = 6 */
	{
		&s_CMP_NtscSDYCbCr_to_HDYCbCr_II,    /* NTSC -> HD */
		&s_CMP_Identity_YCbCr_to_YCbCr,      /* NTSC -> NTSC */
		&s_CMP_Identity_YCbCr_to_YCbCr,      /* NTSC -> PAL */
		&s_CMP_Identity_YCbCr_to_YCbCr,      /* NTSC -> xvYCC_SD */
	},

	/* BVDC_P_MatrixCoefficients_eSmpte_240M = 7 */
	{
		&s_CMP_240MHDYCbCr_to_HDYCbCr_II,    /* 240M -> HD */
		&s_CMP_240MHDYCbCr_to_NtscSDYCbCr_II,/* 240M -> NTSC */
		&s_CMP_240MHDYCbCr_to_NtscSDYCbCr_II,/* 240M -> PAL */
		&s_CMP_240MHDYCbCr_to_NtscSDYCbCr_II,/* 240M -> xvYCC_SD */
	},

	/* forbidden 8 */
	{
		NULL,
	},

	/* forbidden 9 */
	{
		NULL,
	}
};


/****************************** Non-linear Set *********************************/
/* the compositor RGB to YCbCr color matrices A table (YCbCr->RGB) */
static const BVDC_P_CscCoeffs *const s_aCMP_YCbCr_to_RGBPrim_MatrixTbl[] =
{
	NULL,                             /* BVDC_P_MatrixCoefficients_eHdmi_RGB = 0 */
	&s_CMP_HDYCbCr_to_RGBPrim,        /* BVDC_P_MatrixCoefficients_eItu_R_BT_709 = 1, */
	&s_CMP_HDYCbCr_to_RGBPrim,        /* BVDC_P_MatrixCoefficients_eUnknown = 2 */
	NULL,                             /* forbidden 3 */
	&s_CMP_NtscSDYCbCr_to_RGBPrim,    /* BVDC_P_MatrixCoefficients_eFCC = 4 */
	&s_CMP_NtscSDYCbCr_to_RGBPrim,    /* BVDC_P_MatrixCoefficients_eItu_R_BT_470_2_BG = 5 */
	&s_CMP_NtscSDYCbCr_to_RGBPrim,    /* BVDC_P_MatrixCoefficients_eSmpte_170M = 6 */
	&s_CMP_240MHDYCbCr_to_RGBPrim,    /* BVDC_P_MatrixCoefficients_eSmpte_240M = 7 */
	&s_CMP_HDYCbCr_to_RGBPrim,        /* BVDC_P_MatrixCoefficients_eXvYCC_709 = 8, */
	&s_CMP_NtscSDYCbCr_to_RGBPrim     /* BVDC_P_MatrixCoefficients_eXvYCC_601 = 9 */
};

/* the compositor RGB color matrices B table with Color Primaries Matching */
/* with non-linear matrices, RGB primary matching is the only place where XvYCC */
/* is handled differently from non-XvYCC, and only with XvYCC BT601, which uses */
/* BT709 primaries with BT609 luma coefficients that govern YCbCr<->RGB */
/* conversions (Matrix A and C) */
static const BVDC_P_CscCoeffs *const s_aaCMP_RGBPrim_MatrixTbl[][4] =
{
	/* BVDC_P_MatrixCoefficients_eHdmi_RGB = 0 */
	{
		NULL,
	},

	/* BVDC_P_MatrixCoefficients_eItu_R_BT_709 = 1, */
	{
		&s_CMP_Identity_RGBPrim_to_RGBPrim,    /* HD -> HD, XvYCC_HD */
		&s_CMP_BT709RGBPrim_to_BT601RGBPrim,   /* HD -> NTSC */
		&s_CMP_BT709RGBPrim_to_PalSDRGBPrim,   /* HD -> PAL */
		&s_CMP_Identity_RGBPrim_to_RGBPrim     /* HD -> XvYCC_SD */
	},

	/* BVDC_P_MatrixCoefficients_eUnknown = 2 */
	{
		&s_CMP_Identity_RGBPrim_to_RGBPrim,    /* Identity */
		&s_CMP_Identity_RGBPrim_to_RGBPrim,    /* Identity */
		&s_CMP_Identity_RGBPrim_to_RGBPrim,    /* Identity */
		&s_CMP_Identity_RGBPrim_to_RGBPrim     /* Identity */
	},

	/* forbidden 3 */
	{
		NULL,
	},

	/* BVDC_P_MatrixCoefficients_eFCC = 4 */
	{
		&s_CMP_FccSDRGBPrim_to_BT709RGBPrim,   /* FCC -> HD, XvYCC_HD */
		&s_CMP_FccSDRGBPrim_to_BT601RGBPrim,   /* FCC -> NTSC */
		&s_CMP_FccSDRGBPrim_to_PalSDRGBPrim,   /* FCC -> PAL */
		&s_CMP_FccSDRGBPrim_to_BT709RGBPrim    /* FCC -> XvYCC_SD */
	},

	/* BVDC_P_MatrixCoefficients_eItu_R_BT_470_2_BG = 5 */
	{
		&s_CMP_PalSDRGBPrim_to_BT709RGBPrim,   /* PAL -> HD, XvYCC_HD */
		&s_CMP_PalSDRGBPrim_to_BT601RGBPrim,   /* PAL -> NTSC */
		&s_CMP_Identity_RGBPrim_to_RGBPrim,    /* PAL -> PAL */
		&s_CMP_PalSDRGBPrim_to_BT709RGBPrim    /* PAL -> XvYCC_SD */
	},

	/* BVDC_P_MatrixCoefficients_eSmpte_170M = 6 */
	{
		&s_CMP_BT601RGBPrim_to_BT709RGBPrim,   /* NTSC -> HD, XvYCC_HD */
		&s_CMP_Identity_RGBPrim_to_RGBPrim,    /* NTSC -> NTSC */
		&s_CMP_BT601RGBPrim_to_PalSDRGBPrim,   /* NTSC -> PAL */
		&s_CMP_BT601RGBPrim_to_BT709RGBPrim    /* NTSC -> XvYCC_SD */
	},

	/* BVDC_P_MatrixCoefficients_eSmpte_240M = 7 */
	{
		&s_CMP_BT601RGBPrim_to_BT709RGBPrim,   /* 240M -> HD, XvYCC_HD */
		&s_CMP_Identity_RGBPrim_to_RGBPrim,    /* 240M -> NTSC */
		&s_CMP_BT601RGBPrim_to_PalSDRGBPrim,   /* 240M -> PAL */
		&s_CMP_BT601RGBPrim_to_BT709RGBPrim    /* 240M -> XvYCC_SD */
	},

	/* BVDC_P_MatrixCoefficients_eXvYCC_709 = 8 */
	{
		&s_CMP_Identity_RGBPrim_to_RGBPrim,    /* HD -> HD, XvYCC_HD */
		&s_CMP_BT709RGBPrim_to_BT601RGBPrim,   /* HD -> NTSC */
		&s_CMP_BT709RGBPrim_to_PalSDRGBPrim,   /* HD -> PAL */
		&s_CMP_Identity_RGBPrim_to_RGBPrim     /* HD -> XvYCC_SD */
	},

	/* BVDC_P_MatrixCoefficients_eXvYCC_601 = 9 */
	{
		&s_CMP_Identity_RGBPrim_to_RGBPrim,    /* XvYCC_SD -> HD, XvYCC_HD */
		&s_CMP_BT709RGBPrim_to_BT601RGBPrim,   /* XvYCC_SD -> NTSC */
		&s_CMP_BT709RGBPrim_to_PalSDRGBPrim,   /* XvYCC_SD -> PAL */
		&s_CMP_Identity_RGBPrim_to_RGBPrim     /* XvYCC_SD -> XvYCC_SD */
	}
};

/* the compositor RGB color matrices B table with Color Primaries Matching */
/* for custom panels, indexed by source white points */
static const BVDC_P_CscCoeffs *const s_aCMP_RGBPrim_Custom_MatrixTbl[] =
{
	NULL,                                   /* BVDC_P_MatrixCoefficients_eHdmi_RGB = 0 */
	&s_CMP_BT709RGBPrim_to_CustomRGBPrim,   /* BVDC_P_MatrixCoefficients_eItu_R_BT_709 = 1 */
	&s_CMP_BT709RGBPrim_to_CustomRGBPrim,   /* BVDC_P_MatrixCoefficients_eUnknown = 2 */
	NULL,                                   /* forbidden 3 */
	&s_CMP_FccSDRGBPrim_to_CustomRGBPrim,   /* BVDC_P_MatrixCoefficients_eFCC = 4 */
	&s_CMP_PalSDRGBPrim_to_CustomRGBPrim,   /* BVDC_P_MatrixCoefficients_eItu_R_BT_470_2_BG = 5 */
	&s_CMP_BT601RGBPrim_to_CustomRGBPrim,   /* BVDC_P_MatrixCoefficients_eSmpte_170M = 6 */
	&s_CMP_BT601RGBPrim_to_CustomRGBPrim,   /* BVDC_P_MatrixCoefficients_eSmpte_240M = 7 */
	&s_CMP_BT709RGBPrim_to_CustomRGBPrim,   /* BVDC_P_MatrixCoefficients_eXvYCC_709 = 8, */
	&s_CMP_BT709RGBPrim_to_CustomRGBPrim    /* BVDC_P_MatrixCoefficients_eXvYCC_601 = 9 */
};

/* the compositor RGB to YCbCr color matrices C table (RGB->YCbCr) */
/* XvYCC output is handled like non-XvYCC output, and conversions are */
/* only to HD and SD */
static const BVDC_P_CscCoeffs *const s_aCMP_RGBPrim_to_YCbCr_MatrixTbl[] =
{
	&s_CMP_RGBPrim_to_HDYCbCr,        /* HD, XvYCC_HD */
	&s_CMP_RGBPrim_to_NtscSDYCbCr,    /* NTSC */
	&s_CMP_RGBPrim_to_NtscSDYCbCr,    /* PAL */
	&s_CMP_RGBPrim_to_NtscSDYCbCr     /* XvYCC_SD */
};

/* the compositor RGB to YCbCr color matrices C table */
/* for custom panels, indexed by source white points */
static const BVDC_P_CscCoeffs *const s_aCMP_RGBPrim_to_YCbCr_Custom_MatrixTbl[] =
{
	NULL,                                  /* BVDC_P_MatrixCoefficients_eHdmi_RGB = 0 */
	&s_CMP_RGBPrim_to_CustomYCbCr_HDWp,    /* BVDC_P_MatrixCoefficients_eItu_R_BT_709 = 1 */
	&s_CMP_RGBPrim_to_CustomYCbCr_HDWp,    /* BVDC_P_MatrixCoefficients_eUnknown = 2 */
	NULL,                                  /* forbidden 3 */
	&s_CMP_RGBPrim_to_CustomYCbCr_NtscWp,  /* BVDC_P_MatrixCoefficients_eFCC = 4 */
	&s_CMP_RGBPrim_to_CustomYCbCr_NtscWp,  /* BVDC_P_MatrixCoefficients_eItu_R_BT_470_2_BG = 5 */
	&s_CMP_RGBPrim_to_CustomYCbCr_NtscWp,  /* BVDC_P_MatrixCoefficients_eSmpte_170M = 6 */
	&s_CMP_RGBPrim_to_CustomYCbCr_HDWp,    /* BVDC_P_MatrixCoefficients_eSmpte_240M = 7 */
	&s_CMP_RGBPrim_to_CustomYCbCr_HDWp,    /* BVDC_P_MatrixCoefficients_eXvYCC_709 = 8 */
	&s_CMP_RGBPrim_to_CustomYCbCr_NtscWp   /* BVDC_P_MatrixCoefficients_eXvYCC_601 = 9 */
};

/***************************************************************************
 * Return the desired color space conversion for CSC in compositor.
 *
 */
void BVDC_P_Compositor_GetCscTable_isr
	( const BVDC_P_CscCoeffs         **ppCscCoeffsTable,
	  bool                             bCscRgbMatching,
	  BAVC_MatrixCoefficients          eInputColorSpace,
	  BVDC_P_CmpColorSpace             eOutputColorSpace,
	  bool                             bInputXvYcc )
{
	BVDC_P_MatrixCoefficients eVdcInputColorSpace = (BVDC_P_MatrixCoefficients) eInputColorSpace;

	/* convert to internal xvYCC color space */
	if(bInputXvYcc && bCscRgbMatching)
	{
		if ((BAVC_MatrixCoefficients)eVdcInputColorSpace == BAVC_MatrixCoefficients_eItu_R_BT_709)
		{
			eVdcInputColorSpace = BVDC_P_MatrixCoefficients_eXvYcc_709;
		}
		else if((BAVC_MatrixCoefficients)eVdcInputColorSpace == BAVC_MatrixCoefficients_eSmpte_170M)
		{
			eVdcInputColorSpace = BVDC_P_MatrixCoefficients_eXvYcc_601;
		}
		else
		{
			BDBG_WRN(("BAVC_TransferCharacteristics_eIec_61966_2_4 with non xvYCC capable colorspace"));
		}
	}

	/* Ouptut debug msgs */
	BDBG_MSG(("bInputXvYcc = %d, bCscRgbMatching = %d", bInputXvYcc, bCscRgbMatching));
	BDBG_MSG(("eVdcInputColorSpace       = %d", eVdcInputColorSpace));
	BDBG_MSG(("eOutputColorSpace         = %d", eOutputColorSpace));

	/* copy color conversion table for modification */
	if(bCscRgbMatching)
	{
		*ppCscCoeffsTable = s_aaCMP_YCbCr_MatrixTbl[eVdcInputColorSpace][eOutputColorSpace];
	}
	else
	{
		*ppCscCoeffsTable = s_aaCMP_YCbCr_MatrixTbl_II[eVdcInputColorSpace][eOutputColorSpace];
	}

	BDBG_ASSERT(*ppCscCoeffsTable);

	return;
}


/***************************************************************************
 * Return the desired color space conversion for CSC in compositor.
 *
 */
void BVDC_P_Compositor_GetCscTablesNonLinear_isr
	( const BVDC_P_CscCoeffs         **ppCscCoeffsTableA,
	  const BVDC_P_CscCoeffs         **ppCscCoeffsTableB,
	  const BVDC_P_CscCoeffs         **ppCscCoeffsTableC,
	  BAVC_MatrixCoefficients          eInputColorSpace,
	  BVDC_P_CmpColorSpace             eOutputColorSpace,
	  bool                             bInputXvYcc )
{
	BVDC_P_MatrixCoefficients eVdcInputColorSpace = (BVDC_P_MatrixCoefficients) eInputColorSpace;
	BVDC_P_MatrixCoefficients eVdcInputPrimColorSpace = eVdcInputColorSpace;

	/* convert to internal xvYCC color space */
	if(bInputXvYcc)
	{
		if ((BAVC_MatrixCoefficients)eVdcInputColorSpace == BAVC_MatrixCoefficients_eItu_R_BT_709)
		{
			eVdcInputPrimColorSpace = BVDC_P_MatrixCoefficients_eXvYcc_709;
		}
		else if((BAVC_MatrixCoefficients)eVdcInputColorSpace == BAVC_MatrixCoefficients_eSmpte_170M)
		{
			eVdcInputPrimColorSpace = BVDC_P_MatrixCoefficients_eXvYcc_601;
		}
		else
		{
			BDBG_WRN(("BAVC_TransferCharacteristics_eIec_61966_2_4 with non xvYCC capable colorspace"));
		}
	}

	/* Ouptut debug msgs */
	BDBG_MSG(("bInputXvYcc = %d", bInputXvYcc));
	BDBG_MSG(("eVdcInputColorSpace       = %d", eVdcInputColorSpace));
	BDBG_MSG(("eVdcInputPrimColorSpace   = %d", eVdcInputPrimColorSpace));
	BDBG_MSG(("eOutputColorSpace         = %d", eOutputColorSpace));

	/* secondary demo csc does not use Matrix A */
	if(ppCscCoeffsTableA)
	{
		*ppCscCoeffsTableA = s_aCMP_YCbCr_to_RGBPrim_MatrixTbl[eVdcInputColorSpace];
		BDBG_ASSERT(*ppCscCoeffsTableA);
	}


	if(eOutputColorSpace == BVDC_P_CmpColorSpace_eCustom)
	{
		/* use DTV panel specific cscs that are calculated with source white points */
		if(ppCscCoeffsTableB)
			*ppCscCoeffsTableB = s_aCMP_RGBPrim_Custom_MatrixTbl[eVdcInputPrimColorSpace];
		if(ppCscCoeffsTableC)
			*ppCscCoeffsTableC = s_aCMP_RGBPrim_to_YCbCr_Custom_MatrixTbl[eVdcInputColorSpace];
	}
	else
	{
		if(ppCscCoeffsTableB)
			*ppCscCoeffsTableB = s_aaCMP_RGBPrim_MatrixTbl[eVdcInputPrimColorSpace][eOutputColorSpace];
		if(ppCscCoeffsTableC)
			*ppCscCoeffsTableC = s_aCMP_RGBPrim_to_YCbCr_MatrixTbl[eOutputColorSpace];
	}

	if (ppCscCoeffsTableB)
		BDBG_ASSERT(*ppCscCoeffsTableB);

	if (ppCscCoeffsTableC)
		BDBG_ASSERT(*ppCscCoeffsTableC);

	return;
}

/* End of file */
