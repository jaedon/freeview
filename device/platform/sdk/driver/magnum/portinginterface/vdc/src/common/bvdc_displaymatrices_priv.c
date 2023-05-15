/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvdc_displaymatrices_priv.c $
 * $brcm_Revision: Hydra_Software_Devel/67 $
 * $brcm_Date: 7/18/12 6:35p $
 *
 * Module Description:
 *   Contains tables for Display settings.
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vdc/src/common/bvdc_displaymatrices_priv.c $
 * 
 * Hydra_Software_Devel/67   7/18/12 6:35p albertl
 * SW7358-357: Updated s_SDYCbCr_to_RGB matrix.
 * 
 * Hydra_Software_Devel/66   12/2/11 1:14p albertl
 * SW7425-1873: Updated YPbPr conversion matrices to hand calculated for
 * accuracy.
 * 
 * Hydra_Software_Devel/65   6/24/11 3:48p albertl
 * SW7420-1966: Cleaned up and reorganized colorspace enums and indicies
 * in display, vdec, and cmp.
 * 
 * Hydra_Software_Devel/64   6/16/10 6:43p albertl
 * CR3556-60: Updated all matrices to match current 8-bit to 8-bit VEC
 * scripts where available.
 * 
 * Hydra_Software_Devel/63   6/11/10 6:47p albertl
 * CR3556-60: Updated SDYCbCr to YUV matrix with old script values until
 * more accurate new scripts are available.
 * 
 * Hydra_Software_Devel/62   2/10/10 1:51p albertl
 * SW7405-3899: Removied unnecessary input colorspace dimension from
 * matrices.
 * 
 * Hydra_Software_Devel/61   1/29/10 8:31p albertl
 * SW7405-3845: Fixed PAL_NC matrices and renamed associated colorspace as
 * YUV_NC.
 * 
 * Hydra_Software_Devel/60   1/27/10 6:05p albertl
 * SW7405-3684: Changed DVI RGB limited range clipping to 1-254.
 * 
 * Hydra_Software_Devel/59   10/16/09 5:21p darnstein
 * SW7405-3191: Back out all changes in pixel frequency handling. New
 * design coming soon.
 * 
 * Hydra_Software_Devel/58   10/15/09 4:53p darnstein
 * SW7405-3191: Pixel frequency is now defined as both a bitmask, and an
 * enum. The bitmask option is DEPRECATED.
 * 
 * Hydra_Software_Devel/57   5/7/09 5:22p albertl
 * PR47736: Changed PAL luma back to standard levels.
 * 
 * Hydra_Software_Devel/56   5/5/09 7:31p albertl
 * PR47736: Updated PAL M csc to correct values.
 * 
 * Hydra_Software_Devel/55   3/4/09 8:14p albertl
 * PR52003: Fixed NTSC_J VEC table.  Set display xvycc to default to
 * false.
 * 
 * Hydra_Software_Devel/54   2/23/09 5:36p albertl
 * PR51940: Fixed csc macros and table values for non 7420 platforms.
 * 
 * Hydra_Software_Devel/52   2/20/09 9:02p albertl
 * PR51940: Changed display matrix min max values to allow conversion for
 * different platforms.  Incorporated interface changes of
 * BVDC_P_Display_GetCscTable_isr from 7420.
 * 
 * Hydra_Software_Devel/51   1/22/09 2:51p albertl
 * PR51169: Fixed issue where BVDC_P_Display_GetCscTable_isr returns an
 * invalid table when display format and input colorspace changes but
 * applychanges is not applied and output colorspace is not properly
 * selected.
 * 
 * Hydra_Software_Devel/50   12/18/08 3:55p albertl
 * PR47736:  Increased chroma/luma gain in SD YCbCr to YUV PAL table.
 * 
 * Hydra_Software_Devel/49   10/23/08 9:39p tdo
 * PR46484:Initial VDC bringup for 7420
 *
 * Hydra_Software_Devel/48   10/2/08 5:53p albertl
 * PR45808: Updated CSC tables and code to use unified 8-bit to 8-bit
 * coefficients and offsets.  Added support for custom panels.
 *
 * Hydra_Software_Devel/47   8/13/08 6:18p pntruong
 * PR45632: [PQ] CSC matrix coefficients for HD need mods to match demo
 * display.
 *
 * Hydra_Software_Devel/46   7/10/08 9:14a yuxiaz
 * PR44652: KLOCWORK: bvdc_displaymatrices_priv.c
 *
 * Hydra_Software_Devel/45   6/16/08 8:09p yuxiaz
 * PR43405: NTSC_J signal has noise
 *
 * Hydra_Software_Devel/44   5/16/08 5:00p darnstein
 * PR39163: fix a problem indexing into arrays (one SECAM becomes three).
 *
 * Hydra_Software_Devel/43   5/15/08 4:58p yuxiaz
 * PR42714: Coverity Defect ID:9041 OVERRUN_STATIC
 * bvdc_displaymatrices_priv.c
 *
 * Hydra_Software_Devel/42   5/15/08 3:12p albertl
 * PR39163:  Changed all matrices to use YCbCr and RGB output format.
 * Updated non-linear matrices to include scale and regain factors.
 *
 * Hydra_Software_Devel/41   3/27/08 5:07p albertl
 * PR39163:  Implemented non-linear xvYCC csc support for 3548.
 *
 * Hydra_Software_Devel/40   3/13/08 9:14a yuxiaz
 * PR40432: Fixed KLOCWORK errors.
 *
 * Hydra_Software_Devel/39   1/14/08 8:40p albertl
 * PR38150:  Fixed incorrect xvYCC table comment.
 *
 * Hydra_Software_Devel/38   1/13/08 3:29a albertl
 * PR38150: Added 7440 HDMI xvYCC CSC support.
 *
 * Hydra_Software_Devel/37   11/16/07 3:45p rpan
 * PR36949: Updated the NTSC-J CSC coefficient settings to get rid of
 * noise.
 *
 * Hydra_Software_Devel/36   10/10/07 9:02p pntruong
 * PR35896: DVO output setup for PAL/SECAM SD.
 *
 * Hydra_Software_Devel/35   10/8/07 2:38p pntruong
 * PR34855: Initial VDC bringup.
 *
 * Hydra_Software_Devel/34   8/30/07 4:25p darnstein
 * PR33711: declare static tables to be const. This is required by
 * Broadcom's programming standards.
 *
 * Hydra_Software_Devel/33   11/15/06 7:06p hongtaoz
 * PR25668: add vdc support for 7403;
 *
 * Hydra_Software_Devel/32   9/27/06 11:46a hongtaoz
 * PR24494: updated SD outputs color matrices;
 *
 * Hydra_Software_Devel/31   9/21/06 12:05p hongtaoz
 * PR23402: updated PAL_NC's YDrDb color matrix;
 *
 * Hydra_Software_Devel/30   8/18/06 4:23p albertl
 * PR23117:  Added 7440 support.
 *
 * Hydra_Software_Devel/29   8/7/06 5:29p pntruong
 * PR22577: Initial bringup of VDC.
 *
 * Hydra_Software_Devel/28   7/25/06 3:53p hongtaoz
 * PR22246: fixed YUV matrices;
 *
 * Hydra_Software_Devel/27   6/30/06 5:45p hongtaoz
 * PR22081, PR20395, PR22062, PR22246: added SECAM Macrovision support;
 * fixed PAL_M dvi microcode; fixed PAL_M sub-carrier frequency setting;
 * PAL_NC should use the same microcode as normal PAL; fixed SECAM and
 * various PAL's csc matrices according to BT.470-6 spec; changed SRC
 * filter setting to beneifit higher freq response for HD output on
 * PRIM_VEC; fixed PAL sync tip setting for non-Macrovision build;
 *
 * Hydra_Software_Devel/26   6/23/06 6:33p hongtaoz
 * PR22246: some VEC csc, sync, offset values are incorrect;
 *
 * Hydra_Software_Devel/25   6/14/06 3:52p syang
 * PR 21689: redo adding support for 7118
 *
 * Hydra_Software_Devel/23   6/12/06 11:11a hongtaoz
 * PR22081: bring up SECAM; added SECAM color matrix;
 *
 * Hydra_Software_Devel/22   5/19/06 1:47p pntruong
 * PR21560: Global symbols without BVDC prefix in the BVDC module.  Reduce
 * unnecessary global exported symbols.  Tighten const params.
 *
 * Hydra_Software_Devel/21   5/9/06 1:35p tdo
 * PR 21170: Add support for 7401 B0
 *
 * Hydra_Software_Devel/20   4/6/06 10:57a hongtaoz
 * PR19265: 7400 dvi output is in 10-bit range;
 *
 * Hydra_Software_Devel/19   3/7/06 4:12p syang
 * PR 19670: added 7438 support
 *
 * Hydra_Software_Devel/18   1/31/06 5:21p pntruong
 * PR17778: Take in changes for new hddvi.
 *
 * Hydra_Software_Devel/14   1/17/06 4:18p hongtaoz
 * PR19082: first compiled for 7400;
 *
 * Hydra_Software_Devel/13   10/25/05 6:08p hongtaoz
 * PR17756: updated 656 bypass matrices;
 *
 * Hydra_Software_Devel/12   9/30/05 11:03a hongtaoz
 * PR17368: updated VEC matrices for RGB outputs;
 *
 * Hydra_Software_Devel/11   8/18/05 1:15p pntruong
 * PR15757, PR16391, PR16411, PR12519, PR14791, PR15535, PR15206, PR15778:
 * Improved bandwidth for cropping/scaler/capture/playback.   Unified
 * window shutdown sequence for destroy, reconfigure mad/scaler, and/or
 * reconfigure result of source changes.  And miscellances fixes from
 * above PRs.
 *
 * Hydra_Software_Devel/XVDPhase1/XVDPhase1_merge/1   8/15/05 12:06p hongtaoz
 * PR15888:  added bypass display's 656 CSC matrices;
 *
 * Hydra_Software_Devel/10   8/14/05 1:16p hongtaoz
 * PR15888: added bypass display's 656 CSC matrices;
 *
 * Hydra_Software_Devel/9   6/28/05 11:02a jasonh
 * PR 16012: Adding support for 7401
 *
 * Hydra_Software_Devel/8   5/3/05 2:50p hongtaoz
 * PR14976: added full range RGB coloremitry outputs support for DVI
 * monitor; fixed DVI response to the source dynamic color space change;
 *
 * Hydra_Software_Devel/7   3/24/05 4:19p hongtaoz
 * PR14521: update matrices for 3560 without truncation compensation;
 *
 * Hydra_Software_Devel/6   3/18/05 6:30p pntruong
 * PR14494: Basic vdc up and running.  Background and passed output format
 * switch test.
 *
 * Hydra_Software_Devel/5   3/17/05 6:36p pntruong
 * PR14494: Add preliminary software support to 3560 A0.
 *
 * Hydra_Software_Devel/4   12/29/04 2:40p hongtaoz
 * PR13177: added RGBHV output support;
 *
 * Hydra_Software_Devel/3   12/20/04 7:20p hongtaoz
 * PR13206: fixed color swap problem for PAL's YUV (CVBS/SVideo) output;
 * changed RGB outputs csc minimum values to be zero;
 *
 * Hydra_Software_Devel/2   12/10/04 7:09p hongtaoz
 * PR13206: set the correct CSC_MIN_MAX value for DVI RGB output;
 *
 * Hydra_Software_Devel/1   12/8/04 4:44p hongtaoz
 * PR13206: added file bvdc_displaymatrices_priv.c to contain only vec
 * color matrices;
 *
 ***************************************************************************/

#include "bchp.h"
#include "bvdc_display_priv.h"
#include "bvdc_displayfmt_priv.h"
#include "bvdc_csc_priv.h"


/**********************************************************
 * static tables
 **********************************************************/

/************* the followings are the original settings *****************/
/* SDYCrCb_to_SDRGB_480i */
static const BVDC_P_DisplayCscMatrix s_SDYCbCr_to_RGB = BVDC_P_MAKE_VEC_CSC_MATRIX
(
	0,
	4095,
	BVDC_P_MAKE_VEC_RGB_CSC
	(  0.6256, -0.0016,  0.8543, -119.3583,
	   0.6256, -0.2087, -0.4378,   72.4649,
	   0.6256,  1.0834, -0.0038, -148.6812 )
);

/* calculated by hand since scripts introduce roundoff errors */
static const BVDC_P_DisplayCscMatrix s_SDYCbCr_to_SDYPbPr = BVDC_P_MAKE_VEC_CSC_MATRIX
(
	-4096,
	4095,
	BVDC_P_MAKE_VEC_CSC
	(  0.6267,  0.0000,  0.0000,  -10.0269,
	   0.0000,  0.6127,  0.0000,  -78.4286,
	   0.0000,  0.0000,  0.6127,  -78.4286 )
);

/* vec_csc_CbYCr2dviGBR */
static const BVDC_P_DisplayCscMatrix s_SDYCbCr_to_RGB_DVI = BVDC_P_MAKE_VEC_CSC_MATRIX
(
	16,   /* clamped for (1-254) */
	4064,
	BVDC_P_MAKE_DVO_RGB_CSC
	(  1.000,  0.000,  1.371, -175.488,
	   1.000, -0.336, -0.698,  132.352,
	   1.000,  1.732,  0.000, -221.696 )
);

/* SDYCrCb_to_SDRGB_DVI */
static const BVDC_P_DisplayCscMatrix s_SDYCbCr_to_RGB_Full_Range_DVI = BVDC_P_MAKE_VEC_CSC_MATRIX
(
	0,
	4095,
	BVDC_P_MAKE_DVO_RGB_CSC
	(  1.1644,  0.0029,  1.5901, -222.1632,
	   1.1644, -0.3885, -0.8149,  134.8800,
	   1.1644,  2.0165,  0.0071, -276.7424 )
);

/* HDYCrCb_to_HDRGB */
static const BVDC_P_DisplayCscMatrix s_HDYCbCr_to_RGB = BVDC_P_MAKE_VEC_CSC_MATRIX
(
	0,
	4095,
	BVDC_P_MAKE_VEC_RGB_CSC
	(  0.6267,  0.0000,  0.9649, -133.5367,
	   0.6267, -0.1148, -0.2868,   41.3785,
	   0.6267,  1.1370,  0.0000, -155.5594 )
);

/* HDYCrCb_to_HDYPrPb_1080i720p */
#define s_HDYCbCr_to_HDYPbPr s_SDYCbCr_to_SDYPbPr

/* HDYCrCb_to_HDRGB_DVI */
static const BVDC_P_DisplayCscMatrix s_HDYCbCr_to_RGB_DVI = BVDC_P_MAKE_VEC_CSC_MATRIX
(
	16,   /* clamped for (1-254) */
	4064,
	BVDC_P_MAKE_DVO_RGB_CSC
	(  1.0000,  0.0000,  1.5396, -197.0749,
	   1.0000, -0.1831, -0.4577,   82.0247,
	   1.0000,  1.8142,  0.0000, -232.2150 )
);

/* vec_csc_hdCbYCr2dviGBR */
static const BVDC_P_DisplayCscMatrix s_HDYCbCr_to_RGB_Full_Range_DVI = BVDC_P_MAKE_VEC_CSC_MATRIX
(
	0,
	4095,
	BVDC_P_MAKE_DVO_RGB_CSC
	(  1.164,  0.000,  1.793, -248.128,
	   1.164, -0.213, -0.534,   76.992,
	   1.164,  2.115,  0.000, -289.344 )
);

/* For custom panel, can be replaced as necessary */
static const BVDC_P_DisplayCscMatrix s_CustomYCbCr_to_RGB_Full_Range_DVI = BVDC_P_MAKE_VEC_CSC_MATRIX
(
	0,
	4095,
	BVDC_P_MAKE_DVO_RGB_CSC
	(  1.168950,  0.000000,  1.727430, -239.814253,
	   1.168950, -0.183493, -0.605934,   82.343430,
	   1.168950,  2.149871,  0.000000, -293.886738 )
);

/* For SECAM Composite/SVideo (SDYCrCb_to_secamYDrDb) */
static const BVDC_P_DisplayCscMatrix s_SDYCbCr_to_YDbDr = BVDC_P_MAKE_VEC_CSC_MATRIX
(
	-4096,
	4095,
	BVDC_P_MAKE_VEC_CSC
	(  0.6267,  0.0000,  0.0000,  -10.0274,
	   0.0000,  0.8238,  0.0000, -103.1917,
	   0.0000,  0.0000, -1.0040,  128.4742 )
);

/* For PAL, PAL_NC Composite/SVideo (SDYCrCb_to_YUV) */
static const BVDC_P_DisplayCscMatrix s_SDYCbCr_to_YUV = BVDC_P_MAKE_VEC_CSC_MATRIX
(
	-4096,
	4095,
	BVDC_P_MAKE_VEC_CSC
	( 0.6267, 0.0000, 0.0000, -10.0274,
	  0.0000, 0.5320, 0.0000, -68.0895,
	  0.0000, 0.0000, 0.7503, -96.0247 )
);

/* For PAL_N/M Composite/SVideo (SDYCrCb_to_nYUV)
   The color space conversion matrix for pal_n is a little
   different from other pal modes, there is a 7.5IRE pedestal
 */
static const BVDC_P_DisplayCscMatrix s_SDYCbCr_to_YUV_N = BVDC_P_MAKE_VEC_CSC_MATRIX
(
	-4096,
	4095,
	BVDC_P_MAKE_VEC_CSC
	(  0.5886,  0.0000,  0.0000,   1.0388,
	   0.0000,  0.5426,  0.0000, -69.4537,
	   0.0000,  0.0000,  0.7652, -97.9487 )
);

/* For NTSC Composite/SVideo (SDYCrCb_to_ntscYIQ)
   Color space convert CbYCr to YIQ + 7.5 IRE pedestal on Y
*/
static const BVDC_P_DisplayCscMatrix s_SDYCbCr_to_YIQ = BVDC_P_MAKE_VEC_CSC_MATRIX
(
	-4096,
	4095,
	BVDC_P_MAKE_VEC_YIQ_CSC
	(  0.5913,  0.0000,  0.0000,    1.0388,
	   0.0000, -0.2766,  0.5998,  -41.3690,
	   0.0000,  0.4200,  0.3891, -103.5615 )
);

/* For NTSC_J Composite/SVideo (SDYCrCb_to_mYIQ)*/
static const BVDC_P_DisplayCscMatrix s_SDYCbCr_to_YIQ_M = BVDC_P_MAKE_VEC_CSC_MATRIX
(
	-4096,
	4095,
	BVDC_P_MAKE_VEC_YIQ_CSC
	(  0.6393,  0.0000,  0.0000,  -10.2283,
	   0.0000, -0.2990,  0.6484,  -44.7232,
	   0.0000,  0.4541,  0.4206, -111.9584 )
);

/* For 656 output. */
static const BVDC_P_DisplayCscMatrix s_HDYCbCr_to_SDYCbCr_656 = BVDC_P_MAKE_VEC_CSC_MATRIX
(
	-4096,
	4095,
	BVDC_P_MAKE_656_CSC
	(  1.000102,  0.099579,  0.191999, -37.323650,
	   0.001012,  0.989664, -0.110798,  15.488960,
	  -0.000073, -0.071500,  0.983585,  11.254214 )
);

static const BVDC_P_DisplayCscMatrix s_HD240MYCbCr_to_SDYCbCr_656 = BVDC_P_MAKE_VEC_CSC_MATRIX
(
	-4096,
	4095,
	BVDC_P_MAKE_656_CSC
	(  1.000000,  0.073380,  0.187140, -33.346601,
	  -0.000000,  0.988035, -0.108776,  15.454889,
	   0.000000, -0.053362,  0.987620,   8.414941 )
);

static const BVDC_P_DisplayCscMatrix s_Identity_656 = BVDC_P_MAKE_VEC_CSC_MATRIX
(
	-4096,
	4095,
	BVDC_P_MAKE_656_CSC
	(  1.0000,  0.0000,  0.0000,  0.0000,
	   0.0000,  1.0000,  0.0000,  0.0000,
	   0.0000,  0.0000,  1.0000,  0.0000 )
);

/* vec_csc_std_init */
static const BVDC_P_DisplayCscMatrix s_Identity_DVI = BVDC_P_MAKE_VEC_CSC_MATRIX
(
	-4096,
	4095,
	BVDC_P_MAKE_DVO_CSC
	(  1.0000,  0.0000,  0.0000,  0.0000,
	   0.0000,  1.0000,  0.0000,  0.0000,
	   0.0000,  0.0000,  1.0000,  0.0000 )
);

/* RGB + Hsync matrix */
static const BVDC_P_DisplayCscMatrix s_HsyncMatrix = BVDC_P_MAKE_VEC_CSC_MATRIX
(
	-4096,
	4095,
	BVDC_P_MAKE_VEC_CSC
	(  0.0000,  0.0000,  0.0000,   0.0000,
	   0.0000,  0.0000,  0.0000,  16.0000,
	   0.0000,  0.0000,  0.0000,  16.0000 )
);

/* XvYCC to no XvYCC matrices for analog output.

Calculated by post-multiplying SDYCbCr_to_XXX matrix by the inverse
of s_CMP_NtscSDYCbCr_to_XvYCCSDYCbCr which strips XvYCC from the source */

/* SDYCrCb_to_SDRGB_480i */
static const BVDC_P_DisplayCscMatrix s_XvYCC_SDYCbCr_to_RGB = BVDC_P_MAKE_VEC_CSC_MATRIX
(
	0,
	4095,
	BVDC_P_MAKE_VEC_RGB_CSC
	(  0.6267, -0.0008,  0.9360, -129.9406,
	   0.6267, -0.2350, -0.4715,   80.1133,
	   0.6267,  1.0778,  0.0047, -147.8734 )
);

/* SDYCrCb_to_SDYPrPb_480i */
static const BVDC_P_DisplayCscMatrix s_XvYCC_SDYCbCr_to_SDYPbPr = BVDC_P_MAKE_VEC_CSC_MATRIX
(
	-4096,
	4095,
	BVDC_P_MAKE_VEC_CSC
	(  0.6267, -0.0158,  0.0047,  -8.5615,
	   0.0000,  0.6176,  0.0025, -78.6586,
	   0.0000,  0.0115,  0.6667, -86.8115 )
);

/* For SECAM Composite/SVideo (SDYCrCb_to_secamYDrDb) */
static const BVDC_P_DisplayCscMatrix s_XvYCC_SDYCbCr_to_YDbDr = BVDC_P_MAKE_VEC_CSC_MATRIX
(
	-4096,
	4095,
	BVDC_P_MAKE_VEC_CSC
	(  0.6267, -0.0158,  0.0047,   -8.6184,
	   0.0000,  0.8301, -0.0040, -103.4817,
	   0.0000, -0.0195, -1.0924,  142.2855 )
);

/* For PAL, PAL_NC Composite/SVideo (SDYCrCb_to_YUV) */
static const BVDC_P_DisplayCscMatrix s_XvYCC_SDYCbCr_to_YUV = BVDC_P_MAKE_VEC_CSC_MATRIX
(
	-4096,
	4095,
	BVDC_P_MAKE_VEC_CSC
	(  0.6267, -0.0158,  0.0047,   -8.6184,
	   0.0000,  0.5361, -0.0026,  -68.2768,
	   0.0000,  0.0145,  0.8164, -106.3461 )
);

/* For PAL_N/M Composite/SVideo (SDYCrCb_to_nYUV)
   The color space conversion matrix for pal_n is a little
   different from other pal modes, there is a 7.5IRE pedestal
 */
static const BVDC_P_DisplayCscMatrix s_XvYCC_SDYCbCr_to_YUV_N = BVDC_P_MAKE_VEC_CSC_MATRIX
(
	-4096,
	4095,
	BVDC_P_MAKE_VEC_CSC
	(  0.5886, -0.0148,  0.0045,    2.3621,
	   0.0000,  0.5467, -0.0026,  -69.6447,
	   0.0000,  0.0148,  0.8326, -108.4750 )
);

/* For NTSC Composite/SVideo (SDYCrCb_to_ntscYIQ)
   Color space convert CbYCr to YIQ + 7.5 IRE pedestal on Y
*/
static const BVDC_P_DisplayCscMatrix s_XvYCC_SDYCbCr_to_YIQ = BVDC_P_MAKE_VEC_CSC_MATRIX
(
	-4096,
	4095,
	BVDC_P_MAKE_VEC_YIQ_CSC
	(  0.5913, -0.0149,  0.0045,    2.3682,
	   0.0000, -0.2671,  0.6540,  -49.5227,
	   0.0000,  0.4307,  0.4213, -109.0619 )
);

/* For NTSC_J Composite/SVideo (SDYCrCb_to_mYIQ)*/
static const BVDC_P_DisplayCscMatrix s_XvYCC_SDYCbCr_to_YIQ_M = BVDC_P_MAKE_VEC_CSC_MATRIX
(
	-4096,
	4095,
	BVDC_P_MAKE_VEC_YIQ_CSC
	(  0.6393, -0.0161,  0.0048,   -8.7910,
	   0.0000, -0.2887,  0.7070,  -53.5375,
	   0.0000,  0.4657,  0.4554, -117.9042 )
);

static const BVDC_P_DisplayCscMatrix s_XvYCC_SDYCbCr_to_SDYCbCr_656 = BVDC_P_MAKE_VEC_CSC_MATRIX
(
	-4096,
	4095,
	BVDC_P_MAKE_656_CSC
	(  1.0000, -0.0251,  0.0076,   2.2482,
	   0.0000,  1.0076, -0.0049,  -0.3520,
	   0.0000,  0.0194,  1.0881, -13.7563 )
);

/****************************************************************
 *  Global Tables
 *  3560 might not need to compensate CMP truncation error;
 ****************************************************************/
/* SDYCbCr CSC table, based on BVDC_P_Output */
static const BVDC_P_DisplayCscMatrix* const s_apSDYCbCr_MatrixTbl[BVDC_P_Output_eMax] =
{
	&s_SDYCbCr_to_YIQ,       /* SVideo, Composite, SC (for NTSC) */
	&s_SDYCbCr_to_YIQ_M,     /* SVideo, Composite, SC (for NTSC_J) */
	&s_SDYCbCr_to_YUV,       /* SVideo, Composite, SC (for generic Pal) */
	&s_SDYCbCr_to_YUV_N,     /* SVideo, Composite, SC (for Pal_N/M) */
	&s_SDYCbCr_to_YUV,       /* SVideo, Composite, SC (for PAL_NC) */
#if BVDC_P_SUPPORT_VEC_SECAM
	&s_SDYCbCr_to_YDbDr,     /* SVideo, Composite, SC (for SECAM) */
	&s_SDYCbCr_to_YDbDr,     /* SVideo, Composite, SC (for SECAM) */
	&s_SDYCbCr_to_YDbDr,     /* SVideo, Composite, SC (for SECAM) */
#endif
	&s_SDYCbCr_to_SDYPbPr,   /* SYPbPr, CYPbPr, SCYPbPr, SDYPbPr */
	&s_SDYCbCr_to_RGB,       /* RGBSRGB, CRGB, SCRGB, RGB */
};

#if 1
/* xvYCC SDYCbCr CSC table, based on BVDC_P_Output */
static const BVDC_P_DisplayCscMatrix* const s_apXvYCC_SDYCbCr_MatrixTbl[BVDC_P_Output_eMax] =
{
	&s_XvYCC_SDYCbCr_to_YIQ,       /* SVideo, Composite, SC (for NTSC) */
	&s_XvYCC_SDYCbCr_to_YIQ_M,     /* SVideo, Composite, SC (for NTSC_J) */
	&s_XvYCC_SDYCbCr_to_YUV,       /* SVideo, Composite, SC (for generic Pal) */
	&s_XvYCC_SDYCbCr_to_YUV_N,     /* SVideo, Composite, SC (for Pal_N/M) */
	&s_XvYCC_SDYCbCr_to_YUV,       /* SVideo, Composite, SC (for PAL_NC) */
#if BVDC_P_SUPPORT_VEC_SECAM
	&s_XvYCC_SDYCbCr_to_YDbDr,     /* SVideo, Composite, SC (for SECAM) */
	&s_XvYCC_SDYCbCr_to_YDbDr,     /* SVideo, Composite, SC (for SECAM) */
	&s_XvYCC_SDYCbCr_to_YDbDr,     /* SVideo, Composite, SC (for SECAM) */
#endif
	&s_XvYCC_SDYCbCr_to_SDYPbPr,   /* SYPbPr, CYPbPr, SCYPbPr, SDYPbPr */
	&s_XvYCC_SDYCbCr_to_RGB,       /* RGBSRGB, CRGB, SCRGB, RGB */
};
#endif

static const BVDC_P_DisplayCscMatrix* const s_apHDYCbCr_MatrixTbl[] =
{
	&s_HDYCbCr_to_RGB,       /* RGBSRGB, CRGB, SCRGB, RGB */
	&s_HDYCbCr_to_HDYPbPr,   /* HDYPbPr */
};

/* SDYCbCr DVI CSC table, based on BVDC_HdmiOutput */
static const BVDC_P_DisplayCscMatrix* const s_apSDYCbCr_DVI_MatrixTbl[] =
{
	&s_SDYCbCr_to_RGB_DVI,
	&s_Identity_DVI,
	&s_SDYCbCr_to_RGB_Full_Range_DVI
};

/* HDYCbCr DVI CSC table, based on BVDC_HdmiOutput */
static const BVDC_P_DisplayCscMatrix* const s_apHDYCbCr_DVI_MatrixTbl[] =
{
	&s_HDYCbCr_to_RGB_DVI,
	&s_Identity_DVI,
	&s_HDYCbCr_to_RGB_Full_Range_DVI
};

/* SDYCbCr 656 CSC table */
static const BVDC_P_DisplayCscMatrix* const s_apSDYCbCr_656_Bypass_MatrixTbl[] =
{
	&s_HDYCbCr_to_SDYCbCr_656,            /* from HD video source */
	&s_Identity_656,                      /* from NTSC SD video source */
	&s_Identity_656,                      /* from PAL SD video source */
	NULL,
	&s_Identity_656,                      /* from FCC 1953 video source */
	&s_HD240MYCbCr_to_SDYCbCr_656         /* from SMPTE 240M HD video source */
};


/***************************************************************************
 *
 */
void BVDC_P_Display_GetCscTable_isr
	( const BVDC_P_DisplayInfo        *pDispInfo,
	  BVDC_P_Output                    eOutputColorSpace,
	  const BVDC_P_DisplayCscMatrix  **ppCscMatrix )
{

	/* Component only CSC */
	/* Get proper color space conversion table.
	 * Note: assume non-bypass displays only have HD color space intput! */
	if(BVDC_P_Output_eHsync == eOutputColorSpace)
	{
		/* RGB + Hsync + Vsync */
		*ppCscMatrix = &s_HsyncMatrix;
	}
	else if(VIDEO_FORMAT_27Mhz(pDispInfo->pFmtInfo->ulPxlFreqMask))
	{
		BDBG_ASSERT(eOutputColorSpace <= BVDC_P_Output_eRGB);

		/* NTSCs, PALs, 480p and 576p;
		   TODO: differentiate NTSC SD and PAL SD color space. */
		*ppCscMatrix = (pDispInfo->bXvYcc) ?
			s_apXvYCC_SDYCbCr_MatrixTbl[eOutputColorSpace] :
			s_apSDYCbCr_MatrixTbl[eOutputColorSpace];
	}
	else
	{
		/* HD output */
		uint32_t   ulIndex = (eOutputColorSpace == BVDC_P_Output_eHDYPrPb) ? 1 : 0;
		if(pDispInfo->eCmpColorSpace <= BVDC_P_CmpColorSpace_eSmpte_240M)
		{
			*ppCscMatrix = s_apHDYCbCr_MatrixTbl[ulIndex];
		}
		else
		{
			BDBG_ASSERT(pDispInfo->eCmpColorSpace <= BVDC_P_CmpColorSpace_eSmpte_240M);
		}
	}

	return;
}

/***************************************************************************
 * This is to convert the color space from CMP -> DVO.  Since the CMP's CSC
 * already convert color space of specific input -> YCbCr ; the DVO only needs
 * to convert it further to:
 *   (1) RGB
 *   (2) RGB Full Range
 *   (3) bypass (identity matrix)
 */
void BVDC_P_Display_GetDviCscTable_isr
	( const BVDC_P_DisplayInfo        *pDispInfo,
	  const BVDC_P_DisplayCscMatrix  **ppCscMatrix )
{
	/* TODO: sort out PAL output CSC */
	switch(pDispInfo->eHdmiOutput)
	{
	case BAVC_MatrixCoefficients_eHdmi_RGB:
		*ppCscMatrix = (pDispInfo->pFmtInfo->ulWidth <= BFMT_NTSC_WIDTH) ?
			s_apSDYCbCr_DVI_MatrixTbl[0] :
			s_apHDYCbCr_DVI_MatrixTbl[0];
		break;

	case BAVC_MatrixCoefficients_eDvi_Full_Range_RGB:
		if((pDispInfo->pFmtInfo->eVideoFmt == BFMT_VideoFmt_eCustom0) ||
		   (pDispInfo->pFmtInfo->eVideoFmt == BFMT_VideoFmt_eCustom1) ||
		   (pDispInfo->pFmtInfo->eVideoFmt == BFMT_VideoFmt_eCustom2))
		{
			*ppCscMatrix = &s_CustomYCbCr_to_RGB_Full_Range_DVI;
		}
		else
		{
			*ppCscMatrix = (pDispInfo->pFmtInfo->ulWidth <= BFMT_NTSC_WIDTH) ?
				s_apSDYCbCr_DVI_MatrixTbl[2] :
				s_apHDYCbCr_DVI_MatrixTbl[2];
		}
		break;

	default: /* YCbCr */
		*ppCscMatrix = (pDispInfo->pFmtInfo->ulWidth <= BFMT_NTSC_WIDTH)  ?
			s_apSDYCbCr_DVI_MatrixTbl[1] :
			s_apHDYCbCr_DVI_MatrixTbl[1];
	}

	return;
}


/***************************************************************************
 *
 */
void BVDC_P_Display_Get656CscTable_isr
	( const BVDC_P_DisplayInfo        *pDispInfo,
	  bool                             bBypass,
	  const BVDC_P_DisplayCscMatrix  **ppCscMatrix )
{
	/* Note the new color space conversion in compositor would always
	 * output SD to 656 output. */
	if(pDispInfo->eCmpColorSpace <= BVDC_P_CmpColorSpace_eSmpte_240M)
	{
		*ppCscMatrix = (bBypass) ?
			s_apSDYCbCr_656_Bypass_MatrixTbl[pDispInfo->eCmpColorSpace] :
			((pDispInfo->bXvYcc) ? &s_XvYCC_SDYCbCr_to_SDYCbCr_656 :
			                       &s_Identity_656);
	}
	else
	{
		BDBG_ASSERT(pDispInfo->eCmpColorSpace <= BVDC_P_CmpColorSpace_eSmpte_240M);
	}
	return;
}

/* End of file */
