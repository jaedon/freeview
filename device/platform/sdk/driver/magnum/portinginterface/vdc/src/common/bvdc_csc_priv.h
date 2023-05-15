/***************************************************************************
 *     Copyright (c) 2003-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvdc_csc_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/68 $
 * $brcm_Date: 8/5/11 8:24p $
 *
 * Module Description:
 *   VDEC/HD_DVI (or similiar hw) CSC matrix coefficients.
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vdc/7038/bvdc_csc_priv.h $
 * 
 * Hydra_Software_Devel/68   8/5/11 8:24p albertl
 * SW7425-745: Added blanking to compositor color for all outputs and all
 * colorspaces.
 * 
 * Hydra_Software_Devel/67   6/24/11 2:58p albertl
 * SW7420-1966: Cleaned up and reorganized colorspace enums and indicies
 * in display, vdec, and cmp.
 * 
 * Hydra_Software_Devel/66   2/22/11 2:51p vanessah
 * SW7422-136: Fix CSC error
 * 
 * Hydra_Software_Devel/65   10/11/10 5:25p yuxiaz
 * SW7405-4936: Include bvdc_gfxfeeder_priv.h for BVDC_P_SUPPORT_GFD_VER_3
 * 
 * Hydra_Software_Devel/64   8/26/10 7:40p albertl
 * SW7405-4556, SW7405-4515: Changed RGB attenuation to use inverse of
 * user matrix C.
 * 
 * Hydra_Software_Devel/63   3/11/10 5:35p albertl
 * SW3548-2700: Added float debug messages.  Fixed incorrect value for Mc
 * when doing RGB attenuation with user matrices.
 * 
 * Hydra_Software_Devel/62   3/3/10 5:07p tdo
 * SW7420-556 : Setting Display graphics source/destination blend factor
 * to constant value causes green screen.  Wrong GFD CSC precision.
 *
 * Hydra_Software_Devel/61   1/12/10 8:42p albertl
 * SW3548-2700: Changed RGB attenuation to use DVO matrix as YCbCr->RGB
 * matrix and Matrix C as RGB->YCbCr matrix.
 *
 * Hydra_Software_Devel/60   11/19/09 10:15a pntruong
 * SW7408-13: Initial check in to get 7408 build.
 *
 * Hydra_Software_Devel/59   10/23/09 10:47p albertl
 * SW7340-53: Added clamping to csc values to fit within ranges allowed by
 * hardware bits.
 *
 * Hydra_Software_Devel/58   9/1/09 12:45a pntruong
 * SW7125-13: Added initial 7125 support.
 *
 * Hydra_Software_Devel/57   8/19/09 3:43p pntruong
 * PR55225: Generalized the vec csc min/max range to orthongonal vec.
 *
 * Hydra_Software_Devel/56   8/13/09 1:46p tdo
 * PR55225: Initial VDC/Appframework support for 7342
 *
 * Hydra_Software_Devel/55   6/19/09 6:03p darnstein
 * PR55225: add support for 7342 chipset.
 *
 * Hydra_Software_Devel/54   6/18/09 5:53p syang
 * PR 55812: add 7550 support
 *
 * Hydra_Software_Devel/53   6/11/09 4:04p darnstein
 * PR55219: fill in some details for 7340 chipset.
 *
 * Hydra_Software_Devel/52   5/15/09 2:27p pntruong
 * PR51338: Updated for the rgb input signal.
 *
 * Hydra_Software_Devel/51   4/30/09 3:03p albertl
 * PR54593: Fixed Set and GetDvoMatrix to take and return matrices in
 * YCbCr->RGB and YCbCr->YCbCr format and convert to hardware format
 * internally.
 *
 * Hydra_Software_Devel/50   3/25/09 5:54p pntruong
 * PR52524: Used the index determined by upstream block.
 *
 * Hydra_Software_Devel/49   3/9/09 1:29p pntruong
 * PR52524: Added support for hdmi rgb quantization of full range and
 * limited range.
 *
 * Hydra_Software_Devel/48   2/23/09 5:35p albertl
 * PR51940: Fixed csc macros and table values for non 7420 platforms.
 *
 * Hydra_Software_Devel/47   2/20/09 9:01p albertl
 * PR51940: Changed display matrix min max values to allow conversion for
 * different platforms.  Incorporated interface changes of
 * BVDC_P_Display_GetCscTable_isr from 7420.
 *
 * Hydra_Software_Devel/46   2/3/09 7:15p tdo
 * PR51627: add VDC 7336 PI support
 *
 * Hydra_Software_Devel/45   1/31/09 3:29a albertl
 * PR51397: Changed 656 format to 11.4/4.11 8.2-bit to 8.2-bit.
 *
 * Hydra_Software_Devel/44   10/24/08 2:44a albertl
 * PR48023:  Added BVDC_Display_SetDvoAttenuationRGB and
 * BVDC_Display_GetDvoAttenuationRGB, enabling CSC adjustment on DVO
 * path.  Further cleaned up CSC matrix multiplication code.
 *
 * Hydra_Software_Devel/43   10/23/08 5:45p albertl
 * PR47318, PR47814: Fixed overflow when calculating RGB attenuation
 * offsets.  Rewrote portions of RGB attenuation code and fixed various
 * offset related issues.
 *
 * Hydra_Software_Devel/42   10/9/08 5:27p syang
 * PR 46891: add _isr to the name of func used in _isr context
 *
 * Hydra_Software_Devel/41   10/6/08 6:30p albertl
 * PR45808: Updated fixed point formats for 7420.
 *
 * Hydra_Software_Devel/40   10/2/08 5:53p albertl
 * PR45808: Updated CSC tables and code to use unified 8-bit to 8-bit
 * coefficients and offsets.  Added support for custom panels.
 *
 * Hydra_Software_Devel/39   9/29/08 12:58p pntruong
 * PR47072: Moving 3563 vdc dedicated branch.
 *
 * Hydra_Software_Devel/38   7/23/08 2:06p albertl
 * PR44726:  Removed unnecessary multiplies and shifts from DVI macros.
 * Updated DVIcsc table bit setting to 8 bits.
 *
 * Hydra_Software_Devel/37   7/8/08 10:36a yuxiaz
 * PR44621: Fixed BVDC_P_MAKE_656_CSC for 656 output.
 *
 * Hydra_Software_Devel/36   6/17/08 3:52p albertl
 * PR35135:  Fixed incorrect black offsets causing incorrect contrast
 * adjustments.
 *
 * Hydra_Software_Devel/35   6/16/08 8:50p pntruong
 * PR39321: Updated video bit precision for HD_DVI.
 *
 * Hydra_Software_Devel/34   6/5/08 8:43p pntruong
 * PR41315: Initial secam bringup.
 *
 * Hydra_Software_Devel/33   5/21/08 1:54p albertl
 * PR39163:  Updated CMP csc tables to use 10 bit offset values.
 * Corrected to use proper csc tables in conversions involving ntsc, pal,
 * and smpte 240M.  Moved colorclip from window to compositor and changed
 * BVDC_Window_Set/GetColorClip to BVDC_Compositor_Set/GetColorClip.
 *
 * Hydra_Software_Devel/32   5/15/08 6:44p albertl
 * PR39163:  Fixed BVDC_P_MAKE_DVO_RGB_CSC macro inputs being out of
 * order.
 *
 * Hydra_Software_Devel/31   5/15/08 3:11p albertl
 * PR39163:  Changed all matrices to use YCbCr and RGB output format.
 * Updated non-linear matrices to include scale and regain factors.
 *
 * Hydra_Software_Devel/30   4/16/08 5:33p albertl
 * PR37121:  Fixed CbAlpha nd CrAlpha overflow on 7325 GFD.
 *
 * Hydra_Software_Devel/29   3/13/08 4:11p yuxiaz
 * PR38929: Initial VDC bringup: Added CSC settings for CMP.
 *
 * Hydra_Software_Devel/28   1/13/08 3:28a albertl
 * PR38150: Added 7440 HDMI xvYCC CSC support.
 *
 * Hydra_Software_Devel/27   12/21/07 4:04p albertl
 * PR36588:  Added documentation on converting values between code using
 * different color adjustment ranges.
 *
 * Hydra_Software_Devel/26   12/14/07 11:16a tdo
 * PR36898: Bringup 7335
 *
 * Hydra_Software_Devel/25   12/6/07 7:27p albertl
 * PR37722:  Moved matrix initialization out of isr.  Fixed float to fix
 * conversion macros that allowed fload instructions to creep in.
 *
 * Hydra_Software_Devel/24   11/15/07 5:04p yuxiaz
 * PR35013: CSC in CMP is 8 bit in, 10 bit out on 7325.
 *
 * Hydra_Software_Devel/23   11/11/07 9:57a yuxiaz
 * PR34523: 7325 bring up.
 *
 * Hydra_Software_Devel/22   10/15/07 2:58p albertl
 * PR35135:  Fixed overflow when multiplying offsets that occurs with
 * 3563.
 *
 * Hydra_Software_Devel/21   10/11/07 2:05p albertl
 * PR35135:  Fixed user csc macros and added 10-bit/8-bit video data
 * conversion.
 *
 * Hydra_Software_Devel/20   10/2/07 11:22a pntruong
 * PR35136: Some cleanup of unused code.
 *
 * Hydra_Software_Devel/19   10/1/07 11:23a pntruong
 * PR35460: Fixed precision for hddvi.
 *
 * Hydra_Software_Devel/PR35460/1   9/29/07 12:56p dyzhang
 * PR35460: The HDMI input in RGB colorspace is not correct.
 *
 * Hydra_Software_Devel/18   9/27/07 7:14p albertl
 * PR35135:  Fixed missing shift for Y Alphas.  Removed obsolete macros.
 *
 * Hydra_Software_Devel/17   9/27/07 2:01p yuxiaz
 * PR35013, PR34523: Add new CSC support on 7325.
 *
 * Hydra_Software_Devel/16   9/26/07 8:01p albertl
 * PR35135:  Updated csc calculations to properly handle alpha values.
 *
 * Hydra_Software_Devel/15   9/20/07 3:28p pntruong
 * PR35135: Fixed build errors from csc refactoring.
 *
 * Hydra_Software_Devel/14   9/20/07 1:14a albertl
 * PR35135:  Cleaned up color space matrices and changed them to use same
 * macro system.  Added color space conversion functionality to graphics
 * windows.
 *
 * Hydra_Software_Devel/13   7/23/07 6:25p albertl
 * PR30292:  Updated CSC for 10-bit video for 3563.
 *
 * Hydra_Software_Devel/12   7/2/07 11:42a pntruong
 * PR32673: [VDEC] Add NTSC_J support.
 *
 * Hydra_Software_Devel/11   6/26/07 6:04p pntruong
 * PR30702: Updated C3 precision for 3563.
 *
 * Hydra_Software_Devel/10   6/20/07 3:31p albertl
 * PR30702:  Changed brightness to range from -256 to 256.
 *
 * Hydra_Software_Devel/9   6/18/07 6:30p albertl
 * PR30702:  Expanded hue and saturation ranges, and reworked saturation
 * algorithm to support one slope from 0 to 1, and another for 1 to
 * KaMax.
 *
 * Hydra_Software_Devel/8   6/12/07 11:20a tdo
 * PR30613: Move CSC calculations into bvdc_csc_priv.c
 *
 * Hydra_Software_Devel/7   4/9/07 11:24a pntruong
 * PR28266: Provide API to customize DVO CSC.
 *
 * Hydra_Software_Devel/6   3/29/07 3:24p pntruong
 * PR28824: Optimize manual ACGC adjust algorithm.  Use macro for
 * precision bits.  Use csc to adjust cr/cb gain for acgc.
 *
 * Hydra_Software_Devel/5   3/28/07 6:17p tdo
 * PR28392, PR28282: Implement get color matrix function
 *
 * Hydra_Software_Devel/4   3/28/07 5:29p tdo
 * PR28392: Implement get color matrix function
 *
 * Hydra_Software_Devel/3   3/23/07 11:20a tdo
 * PR 28392: Provide API to customize source CSC
 *
 * Hydra_Software_Devel/2   1/22/07 7:51p pntruong
 * PR22579: Bringup of HD_DVI input (dual core).  Fixed colorspace.
 *
 * Hydra_Software_Devel/1   12/18/06 11:41p pntruong
 * PR22577: Merged back to mainline.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/2   10/18/06 12:15p pntruong
 * PR23222: VDEC - ATSC input support.  Separate out colorspace conversion
 * code.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/1   10/18/06 12:06p pntruong
 * PR23222: VDEC - ATSC input support.  Separate out colorspace conversion
 * code.
 *
 ***************************************************************************/
#ifndef BVDC_CSC_PRIV_H__
#define BVDC_CSC_PRIV_H__

#include "bmth_fix.h"
#include "bmth_fix_matrix.h"
#include "bvdc.h"
#include "bvdc_common_priv.h"
#include "bvdc_gfxfeeder_priv.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BVDC_P_FIX_MAX_BITS                 (31)

/* Default FIX point shift */
#define BVDC_P_FIX_POINT_SHIFT              (16)

/* CSC Precision notation SI.F or S4.11, etc */
#if (BCHP_CHIP == 7400) || (BCHP_CHIP == 7405)
#define BVDC_P_CSC_CMP_CX_I_BITS             (4)
#define BVDC_P_CSC_CMP_CX_F_BITS            (11)
#define BVDC_P_CSC_CMP_CO_I_BITS            (11)
#define BVDC_P_CSC_CMP_CO_F_BITS             (4)
#define BVDC_P_CSC_CMP_CO_VID_BITS           (8)

#define BVDC_P_CSC_VDEC_CX_I_BITS            (4)
#define BVDC_P_CSC_VDEC_CX_F_BITS           (11)
#define BVDC_P_CSC_VDEC_CO_I_BITS            (9)
#define BVDC_P_CSC_VDEC_CO_F_BITS            (6)
#define BVDC_P_CSC_VDEC_CO_VID_BITS          (8)

#define BVDC_P_CSC_HDDVI_CX_I_BITS           (4)  /* HD-DVI, MFD */
#define BVDC_P_CSC_HDDVI_CX_F_BITS          (11)
#define BVDC_P_CSC_HDDVI_CO_I_BITS          (11)
#define BVDC_P_CSC_HDDVI_CO_F_BITS           (4)
#define BVDC_P_CSC_HDDVI_CO_VID_BITS         (8)

#define BVDC_P_CSC_GFD_CX_I_BITS             (3)
#define BVDC_P_CSC_GFD_CX_F_BITS            (10)
#define BVDC_P_CSC_GFD_CO_I_BITS             (9)
#define BVDC_P_CSC_GFD_CO_F_BITS             (4)
#define BVDC_P_CSC_GFD_CO_VID_BITS           (8)

#define BVDC_P_CSC_VEC_CX_I_BITS             (2)
#define BVDC_P_CSC_VEC_CX_F_BITS            (13)
#define BVDC_P_CSC_VEC_CO_I_BITS             (9)
#define BVDC_P_CSC_VEC_CO_F_BITS             (6)
#define BVDC_P_CSC_VEC_CO_VID_BITS           (8)

#define BVDC_P_CSC_656_CX_I_BITS             (4)
#define BVDC_P_CSC_656_CX_F_BITS            (11)
#define BVDC_P_CSC_656_CO_I_BITS            (11)
#define BVDC_P_CSC_656_CO_F_BITS             (4)
#define BVDC_P_CSC_656_CO_VID_BITS           (8)

#define BVDC_P_CSC_DVO_CX_I_BITS             (2)
#define BVDC_P_CSC_DVO_CX_F_BITS            (13)
#define BVDC_P_CSC_DVO_CO_I_BITS             (9)
#define BVDC_P_CSC_DVO_CO_F_BITS             (6)
#define BVDC_P_CSC_DVO_CO_VID_BITS           (8)

#else /* 3548 3556 7325 7335 7336 7340 7342 7550 7420 7125 */

#if (BVDC_P_SUPPORT_GFD_VER >= BVDC_P_SUPPORT_GFD_VER_3)
#define BVDC_P_CSC_GFD_CX_I_BITS             (2)
#define BVDC_P_CSC_GFD_CX_F_BITS            (13)
#define BVDC_P_CSC_GFD_CO_I_BITS             (9)
#define BVDC_P_CSC_GFD_CO_F_BITS             (6)
#define BVDC_P_CSC_GFD_CO_VID_BITS           (8)
#else
#define BVDC_P_CSC_GFD_CX_I_BITS             (2)
#define BVDC_P_CSC_GFD_CX_F_BITS            (13)
#define BVDC_P_CSC_GFD_CO_I_BITS            (11)
#define BVDC_P_CSC_GFD_CO_F_BITS             (4)
#define BVDC_P_CSC_GFD_CO_VID_BITS           (8)
#endif

#define BVDC_P_CSC_CMP_CX_I_BITS             (2)
#define BVDC_P_CSC_CMP_CX_F_BITS            (13)
#define BVDC_P_CSC_CMP_CO_I_BITS             (9)
#define BVDC_P_CSC_CMP_CO_F_BITS             (6)
#define BVDC_P_CSC_CMP_CO_VID_BITS           (8)

#define BVDC_P_CSC_VDEC_CX_I_BITS            (2)
#define BVDC_P_CSC_VDEC_CX_F_BITS           (13)
#define BVDC_P_CSC_VDEC_CO_I_BITS            (9)
#define BVDC_P_CSC_VDEC_CO_F_BITS            (6)
#define BVDC_P_CSC_VDEC_CO_VID_BITS          (8)

#define BVDC_P_CSC_HDDVI_CX_I_BITS           (2)  /* HD-DVI, MFD */
#define BVDC_P_CSC_HDDVI_CX_F_BITS          (13)
#define BVDC_P_CSC_HDDVI_CO_I_BITS           (9)
#define BVDC_P_CSC_HDDVI_CO_F_BITS           (6)
#define BVDC_P_CSC_HDDVI_CO_VID_BITS         (8)

#define BVDC_P_CSC_VEC_CX_I_BITS             (2)
#define BVDC_P_CSC_VEC_CX_F_BITS            (13)
#define BVDC_P_CSC_VEC_CO_I_BITS             (9)
#define BVDC_P_CSC_VEC_CO_F_BITS             (6)
#define BVDC_P_CSC_VEC_CO_VID_BITS           (8)

#define BVDC_P_CSC_656_CX_I_BITS             (4)
#define BVDC_P_CSC_656_CX_F_BITS            (11)
#define BVDC_P_CSC_656_CO_I_BITS            (11)
#define BVDC_P_CSC_656_CO_F_BITS             (4)
#define BVDC_P_CSC_656_CO_VID_BITS           (8)

#define BVDC_P_CSC_DVO_CX_I_BITS             (2)
#define BVDC_P_CSC_DVO_CX_F_BITS            (13)
#define BVDC_P_CSC_DVO_CO_I_BITS             (9)
#define BVDC_P_CSC_DVO_CO_F_BITS             (6)
#define BVDC_P_CSC_DVO_CO_VID_BITS           (8)
#endif

/* video bit format of static tables for each module */
#define BVDC_P_CSC_CMP_CO_VID_TBL_BITS       (8)
#define BVDC_P_CSC_VDEC_CO_VID_TBL_BITS      (8)
#define BVDC_P_CSC_HDDVI_CO_VID_TBL_BITS     (8)
#define BVDC_P_CSC_GFD_CO_VID_TBL_BITS       (8)
#define BVDC_P_CSC_656_CO_VID_TBL_BITS       (8)
#define BVDC_P_CSC_DVO_CO_VID_TBL_BITS       (8)
#define BVDC_P_CSC_VEC_CO_VID_TBL_BITS       (8)
#define BVDC_P_CSC_VEC_RANGE_VID_TBL_BITS   (12)

/* vec csc range bits */
#if (BVDC_P_ORTHOGONAL_VEC)
#define BVDC_P_CSC_VEC_RANGE_BITS           (12)
#else
#define BVDC_P_CSC_VEC_RANGE_BITS           (10)
#endif

/* ranges are specified in signed 12 bit dither values */
#define BVDC_P_CSC_VEC_RANGE_SHIFT (BVDC_P_CSC_VEC_RANGE_VID_TBL_BITS - BVDC_P_CSC_VEC_RANGE_BITS)
#define BVDC_P_CSC_VEC_RANGE_MASK  (0xFFFFFFFF >> (BVDC_P_FIX_MAX_BITS - BVDC_P_CSC_VEC_RANGE_VID_TBL_BITS))


/* Color Space Conversion Matrix */
#define BVDC_P_MAKE_CSC_CX(cx, i_bits, f_bits)                                           \
	(uint16_t)BMTH_FIX_SIGNED_FTOFIX(cx, i_bits, f_bits)

#define BVDC_P_MAKE_CSC_CO(co, i_bits, f_bits, co_vid_bits, co_vid_tbl_bits)             \
	(uint16_t)BMTH_FIX_SIGNED_FTOFIX((co_vid_tbl_bits > co_vid_bits) ?                   \
									 ((co) / (1 << (co_vid_tbl_bits - co_vid_bits))) :   \
									 ((co) * (1 << (co_vid_bits - co_vid_tbl_bits))),    \
									 i_bits, f_bits)

#define BVDC_P_FR_USR_MATRIX(cx, usr_shift, i_bits, f_bits)            \
	(uint16_t)BMTH_FIX_SIGNED_CONVERT(cx, BVDC_P_FIX_MAX_BITS - usr_shift, usr_shift, i_bits, f_bits)

#define BVDC_P_TO_USR_MATRIX(cx, i_bits, f_bits, usr_shift)            \
	BMTH_FIX_SIGNED_CONVERT(cx, i_bits, f_bits, BVDC_P_FIX_MAX_BITS - usr_shift, usr_shift)

/* An entry */
#define BVDC_P_MAKE_CSC(Y0, Y1, Y2, YAlpha, YOffset,                \
	                    Cb0, Cb1, Cb2, CbAlpha, CbOffset,           \
	                    Cr0, Cr1, Cr2, CrAlpha, CrOffset,           \
						cx_i_bits, cx_f_bits, co_i_bits, co_f_bits, \
                        co_vid_bits, co_vid_tbl_bits)               \
{                                                                   \
	BVDC_P_MAKE_CSC_CX(Y0,       cx_i_bits, cx_f_bits),             \
	BVDC_P_MAKE_CSC_CX(Y1,       cx_i_bits, cx_f_bits),             \
	BVDC_P_MAKE_CSC_CX(Y2,       cx_i_bits, cx_f_bits),             \
	BVDC_P_MAKE_CSC_CX(YAlpha,   cx_i_bits, cx_f_bits),             \
	BVDC_P_MAKE_CSC_CO(YOffset,  co_i_bits, co_f_bits,              \
                       co_vid_bits, co_vid_tbl_bits),               \
                                                                    \
	BVDC_P_MAKE_CSC_CX(Cb0,      cx_i_bits, cx_f_bits),             \
	BVDC_P_MAKE_CSC_CX(Cb1,      cx_i_bits, cx_f_bits),             \
	BVDC_P_MAKE_CSC_CX(Cb2,      cx_i_bits, cx_f_bits),             \
	BVDC_P_MAKE_CSC_CX(CbAlpha,  cx_i_bits, cx_f_bits),             \
	BVDC_P_MAKE_CSC_CO(CbOffset, co_i_bits, co_f_bits,              \
                       co_vid_bits, co_vid_tbl_bits),               \
                                                                    \
	BVDC_P_MAKE_CSC_CX(Cr0,      cx_i_bits, cx_f_bits),             \
	BVDC_P_MAKE_CSC_CX(Cr1,      cx_i_bits, cx_f_bits),             \
	BVDC_P_MAKE_CSC_CX(Cr2,      cx_i_bits, cx_f_bits),             \
	BVDC_P_MAKE_CSC_CX(CrAlpha,  cx_i_bits, cx_f_bits),             \
	BVDC_P_MAKE_CSC_CO(CrOffset, co_i_bits, co_f_bits,              \
                       co_vid_bits, co_vid_tbl_bits),               \
                                                                    \
	cx_i_bits,                                                      \
	cx_f_bits,                                                      \
	co_i_bits,                                                      \
	co_f_bits,                                                      \
	co_vid_bits                                                     \
}

/* CMP's Csc */
#define BVDC_P_MAKE_CMP_CSC(Y0, Y1, Y2, YOffset,                    \
	                        Cb0, Cb1, Cb2, CbOffset,                \
	                        Cr0, Cr1, Cr2, CrOffset)                \
	BVDC_P_MAKE_CSC(                                                \
	Y0, Y1, Y2, 0, YOffset,                                         \
	Cb0, Cb1, Cb2, 0, CbOffset,                                     \
	Cr0, Cr1, Cr2, 0, CrOffset,                                     \
	BVDC_P_CSC_CMP_CX_I_BITS,                                       \
	BVDC_P_CSC_CMP_CX_F_BITS,                                       \
	BVDC_P_CSC_CMP_CO_I_BITS,                                       \
	BVDC_P_CSC_CMP_CO_F_BITS,                                       \
	BVDC_P_CSC_CMP_CO_VID_BITS,                                     \
	BVDC_P_CSC_CMP_CO_VID_TBL_BITS)

/* VDEC's Csc */
#define BVDC_P_MAKE_VDEC_CSC(Y0, Y1, Y2, YOffset,                   \
	                         Cb0, Cb1, Cb2, CbOffset,               \
	                         Cr0, Cr1, Cr2, CrOffset)               \
	BVDC_P_MAKE_CSC(                                                \
	Y0, Y1, Y2, 0, YOffset,                                         \
	Cb0, Cb1, Cb2, 0, CbOffset,                                     \
	Cr0, Cr1, Cr2, 0, CrOffset,                                     \
	BVDC_P_CSC_VDEC_CX_I_BITS,                                      \
	BVDC_P_CSC_VDEC_CX_F_BITS,                                      \
	BVDC_P_CSC_VDEC_CO_I_BITS,                                      \
	BVDC_P_CSC_VDEC_CO_F_BITS,                                      \
	BVDC_P_CSC_VDEC_CO_VID_BITS,                                    \
	BVDC_P_CSC_VDEC_CO_VID_TBL_BITS)

/* HD-DVI's Csc */
#define BVDC_P_MAKE_HDDVI_CSC(Y0, Y1, Y2, YOffset,                  \
	                          Cb0, Cb1, Cb2, CbOffset,              \
	                          Cr0, Cr1, Cr2, CrOffset)              \
	BVDC_P_MAKE_CSC(                                                \
	Y0, Y1, Y2, 0, YOffset,                                         \
	Cb0, Cb1, Cb2, 0, CbOffset,                                     \
	Cr0, Cr1, Cr2, 0, CrOffset,                                     \
	BVDC_P_CSC_HDDVI_CX_I_BITS,                                     \
	BVDC_P_CSC_HDDVI_CX_F_BITS,                                     \
	BVDC_P_CSC_HDDVI_CO_I_BITS,                                     \
	BVDC_P_CSC_HDDVI_CO_F_BITS,                                     \
	BVDC_P_CSC_HDDVI_CO_VID_BITS,                                   \
	BVDC_P_CSC_HDDVI_CO_VID_TBL_BITS)

/* GFD's Csc */
#define BVDC_P_MAKE_GFD_CSC(Y0, Y1, Y2, YAlpha, YOffset,                        \
	                        Cb0, Cb1, Cb2, CbAlpha, CbOffset,                   \
	                        Cr0, Cr1, Cr2, CrAlpha, CrOffset)                   \
	BVDC_P_MAKE_CSC(                                                            \
	Y0, Y1, Y2, (YAlpha / (1 << BVDC_P_CSC_GFD_CO_VID_BITS)), YOffset,          \
	Cb0, Cb1, Cb2, (CbAlpha / (1 << BVDC_P_CSC_GFD_CO_VID_BITS)), CbOffset,     \
	Cr0, Cr1, Cr2, (CrAlpha / (1 << BVDC_P_CSC_GFD_CO_VID_BITS)), CrOffset,     \
	BVDC_P_CSC_GFD_CX_I_BITS,                                                   \
	BVDC_P_CSC_GFD_CX_F_BITS,                                                   \
	BVDC_P_CSC_GFD_CO_I_BITS,                                                   \
	BVDC_P_CSC_GFD_CO_F_BITS,                                                   \
	BVDC_P_CSC_GFD_CO_VID_BITS,                                                 \
	BVDC_P_CSC_GFD_CO_VID_TBL_BITS)

/* VEC's Csc */
/* converts inputs from YCbCr to CbYCr format */
#define BVDC_P_MAKE_VEC_CSC(Y0, Y1, Y2, YOffset,                    \
	                        Cb0, Cb1, Cb2, CbOffset,                \
	                        Cr0, Cr1, Cr2, CrOffset)                \
	BVDC_P_MAKE_CSC(                                                \
	Y1, Y0, Y2, 0, YOffset,                                         \
	Cb1, Cb0, Cb2, 0, CbOffset,                                     \
	Cr1, Cr0, Cr2, 0, CrOffset,                                     \
	BVDC_P_CSC_VEC_CX_I_BITS,                                       \
	BVDC_P_CSC_VEC_CX_F_BITS,                                       \
	BVDC_P_CSC_VEC_CO_I_BITS,                                       \
	BVDC_P_CSC_VEC_CO_F_BITS,                                       \
	BVDC_P_CSC_VEC_CO_VID_BITS,                                     \
	BVDC_P_CSC_VEC_CO_VID_TBL_BITS)

/* VEC's Csc for RGB */
/* converts YCbCr->RGB matrix to CbYCr->GBR format */
#define BVDC_P_MAKE_VEC_RGB_CSC(R0, R1, R2, ROffset,                \
	                            G0, G1, G2, GOffset,                \
	                            B0, B1, B2, BOffset)                \
	BVDC_P_MAKE_VEC_CSC(                                            \
	G0, G1, G2, GOffset,                                            \
	B0, B1, B2, BOffset,                                            \
	R0, R1, R2, ROffset)                                            \

/* VEC's Csc for YIQ */
/* converts YCbCr->YIQ matrix to CbYCr->YQI format */
#define BVDC_P_MAKE_VEC_YIQ_CSC(Y0, Y1, Y2, YOffset,                \
	                            I0, I1, I2, IOffset,                \
	                            Q0, Q1, Q2, QOffset)                \
	BVDC_P_MAKE_VEC_CSC(                                            \
	Y0, Y1, Y2, YOffset,                                            \
	Q0, Q1, Q2, QOffset,                                            \
	I0, I1, I2, IOffset)                                            \

/* 656's Csc */
/* converts YCbCr->YCbCr matrix to CbYCr->YCrCb format */
#define BVDC_P_MAKE_656_CSC(Y0, Y1, Y2, YOffset,                    \
	                        Cb0, Cb1, Cb2, CbOffset,                \
	                        Cr0, Cr1, Cr2, CrOffset)                \
	BVDC_P_MAKE_CSC(                                                \
	Y1, Y0, Y2, 0, YOffset,                                         \
	Cr1, Cr0, Cr2, 0, CrOffset,                                     \
	Cb1, Cb0, Cb2, 0, CbOffset,                                     \
	BVDC_P_CSC_656_CX_I_BITS,                                       \
	BVDC_P_CSC_656_CX_F_BITS,                                       \
	BVDC_P_CSC_656_CO_I_BITS,                                       \
	BVDC_P_CSC_656_CO_F_BITS,                                       \
	BVDC_P_CSC_656_CO_VID_BITS,                                     \
	BVDC_P_CSC_656_CO_VID_TBL_BITS)

/* DVO's Csc */
/* converts inputs from YCbCr to CbYCr format */
#define BVDC_P_MAKE_DVO_CSC(Y0, Y1, Y2, YOffset,                    \
	                        Cb0, Cb1, Cb2, CbOffset,                \
	                        Cr0, Cr1, Cr2, CrOffset)                \
	BVDC_P_MAKE_CSC(                                                \
	Y1, Y0, Y2, 0, YOffset,                                         \
	Cb1, Cb0, Cb2, 0, CbOffset,                                     \
	Cr1, Cr0, Cr2, 0, CrOffset,                                     \
	BVDC_P_CSC_DVO_CX_I_BITS,                                       \
	BVDC_P_CSC_DVO_CX_F_BITS,                                       \
	BVDC_P_CSC_DVO_CO_I_BITS,                                       \
	BVDC_P_CSC_DVO_CO_F_BITS,                                       \
	BVDC_P_CSC_DVO_CO_VID_BITS,                                     \
	BVDC_P_CSC_DVO_CO_VID_TBL_BITS)

/* converts YCbCr->RGB matrix to CbYCr->GBR format */
#define BVDC_P_MAKE_DVO_RGB_CSC(R0, R1, R2, ROffset,                \
	                            G0, G1, G2, GOffset,                \
	                            B0, B1, B2, BOffset)                \
	BVDC_P_MAKE_DVO_CSC(                                            \
	G0, G1, G2, GOffset,                                            \
	B0, B1, B2, BOffset,                                            \
	R0, R1, R2, ROffset)                                            \

/* ColorTemp calculation Csc */
#define BVDC_P_MAKE_CLRTEMP_CSC(Y0, Y1, Y2, YOffset,                \
	                            Cb0, Cb1, Cb2, CbOffset,            \
	                            Cr0, Cr1, Cr2, CrOffset,            \
					        	cx_i_bits, cx_f_bits)               \
{                                                                   \
	{                                                               \
		BMTH_FIX_SIGNED_FTOFIX(Y0,       cx_i_bits, cx_f_bits),     \
		BMTH_FIX_SIGNED_FTOFIX(Y1,       cx_i_bits, cx_f_bits),     \
		BMTH_FIX_SIGNED_FTOFIX(Y2,       cx_i_bits, cx_f_bits),     \
		BMTH_FIX_SIGNED_FTOFIX(YOffset,  cx_i_bits, cx_f_bits)      \
	},                                                              \
	{                                                               \
		BMTH_FIX_SIGNED_FTOFIX(Cb0,      cx_i_bits, cx_f_bits),     \
		BMTH_FIX_SIGNED_FTOFIX(Cb1,      cx_i_bits, cx_f_bits),     \
		BMTH_FIX_SIGNED_FTOFIX(Cb2,      cx_i_bits, cx_f_bits),     \
		BMTH_FIX_SIGNED_FTOFIX(CbOffset, cx_i_bits, cx_f_bits),     \
	},                                                              \
	{                                                               \
		BMTH_FIX_SIGNED_FTOFIX(Cr0,      cx_i_bits, cx_f_bits),     \
		BMTH_FIX_SIGNED_FTOFIX(Cr1,      cx_i_bits, cx_f_bits),     \
		BMTH_FIX_SIGNED_FTOFIX(Cr2,      cx_i_bits, cx_f_bits),     \
		BMTH_FIX_SIGNED_FTOFIX(CrOffset, cx_i_bits, cx_f_bits),     \
	}                                                               \
}

/* ColorTemp calculation Linear Model */
#define BVDC_P_MAKE_CLRTEMP_LMODEL(R0, R1,                          \
	                               G0, G1,                          \
	                               B0, B1,                          \
	                               cx_i_bits, cx_f_bits)            \
{                                                                   \
	{                                                               \
		BMTH_FIX_SIGNED_FTOFIX(R0, cx_i_bits, cx_f_bits),           \
		BMTH_FIX_SIGNED_FTOFIX(R1, cx_i_bits, cx_f_bits)            \
	},                                                              \
	{                                                               \
		BMTH_FIX_SIGNED_FTOFIX(G0, cx_i_bits, cx_f_bits),           \
		BMTH_FIX_SIGNED_FTOFIX(G1, cx_i_bits, cx_f_bits)            \
	},                                                              \
	{                                                               \
		BMTH_FIX_SIGNED_FTOFIX(B0, cx_i_bits, cx_f_bits),           \
		BMTH_FIX_SIGNED_FTOFIX(B1, cx_i_bits, cx_f_bits)            \
	}                                                               \
}

/* macro to build BVDC_P_DisplayCscMatrix */
#define BVDC_P_MAKE_VEC_CSC_MATRIX(min, max, csc_coeff)             \
{                                                                   \
	(min & BVDC_P_CSC_VEC_RANGE_MASK) >> BVDC_P_CSC_VEC_RANGE_SHIFT,\
	(max & BVDC_P_CSC_VEC_RANGE_MASK) >> BVDC_P_CSC_VEC_RANGE_SHIFT,\
	csc_coeff                                                       \
}

#define BVDC_P_CSC_VIDEO_DATA_BITS      (pCscCoeffs->usCoVideoBits)

/* Color adjustment values */
#define BVDC_P_CONTRAST_VAL_MIN INT16_MIN
#define BVDC_P_CONTRAST_VAL_MAX INT16_MAX
#define BVDC_P_BRIGHTNESS_VAL_MIN INT16_MIN
#define BVDC_P_BRIGHTNESS_VAL_MAX INT16_MAX
#define BVDC_P_SATURATION_VAL_MIN INT16_MIN
#define BVDC_P_SATURATION_VAL_MAX INT16_MAX
#define BVDC_P_HUE_VAL_MIN INT16_MIN
#define BVDC_P_HUE_VAL_MAX INT16_MAX
#define BVDC_P_COLORTEMP_VAL_MIN  INT16_MIN
#define BVDC_P_COLORTEMP_VAL_MAX  INT16_MAX

#define BVDC_P_BRIGHTNESS_MAX      (1 << BVDC_P_CSC_VIDEO_DATA_BITS) /* 1024 on 3563 CMP, 256 on others */
#define BVDC_P_LUMA_BLACK_OFFSET   (64  >> (10 - BVDC_P_CSC_VIDEO_DATA_BITS))
#define BVDC_P_CHROMA_BLACK_OFFSET (512 >> (10 - BVDC_P_CSC_VIDEO_DATA_BITS))

/* Maximum, Minimum, and Center Values of Color Temperature in 100s of Kelvin */
#define BVDC_P_COLORTEMP_KELVIN_MAX    150
#define BVDC_P_COLORTEMP_KELVIN_MIN     15
#define BVDC_P_COLORTEMP_KELVIN_CENTER  65

/* Color adjustment range coefficients */
/* These values have been changed several times to comply with different
 * requests.  The following describes how to convert contrast, saturation,
 * hue, and brightness values between code with different minimums and
 * maximum values.
 *
 * Saturation and Contrast
 *
 * for values < 0:  new value = old value(1 - old min/1 - new min)
 * for values > 0:  new value = old value(old max - 1/new max - 1)
 *
 * for saturation,  min = (1 - BVDC_P_SATURATION_FIX_KA_RANGE) or 0 if it does not exist.
 *                  max = (1 + BVDC_P_SATURATION_FIX_KA_RANGE) or old BVDC_P_SATURATION_FIX_KA_MAX.
 *
 * for contrast,    min = BVDC_P_CONTRAST_FIX_K_MIN
 *                  max = BVDC_P_CONTRAST_FIX_K_MAX
 *
 *
 * Hue and Brightness
 *
 * new value = old value(old max/new max)
 *
 * for hue,         max = BVDC_P_HUE_FIX_KH_MAX
 *
 * for brightness,  max = BVDC_P_BRIGHTNESS_K_MAX
 */
#define BVDC_P_CONTRAST_FIX_K_MIN    0x0 /* 0.0 */
#define BVDC_P_CONTRAST_FIX_K_MAX    BMTH_FIX_SIGNED_ITOFIX(4, BVDC_P_FIX_INT_BITS, BVDC_P_FIX_FRACTION_BITS) /* 4.0 */
#define BVDC_P_SATURATION_FIX_KA_MAX BMTH_FIX_SIGNED_ITOFIX(4, BVDC_P_FIX_INT_BITS, BVDC_P_FIX_FRACTION_BITS) /* 4.0 */
#define BVDC_P_HUE_FIX_KH_MAX        BVDC_P_FIX_PI /* 180 degrees */
#define BVDC_P_BRIGHTNESS_K_MAX      BVDC_P_BRIGHTNESS_MAX /* 1024 on 3563 CMP, 256 on others */

#define BVDC_P_FIX_PI                BMTH_FIX_SIGNED_GET_PI(BVDC_P_FIX_INT_BITS, BVDC_P_FIX_FRACTION_BITS) /* pi */

#define BVDC_P_FIX_INT_BITS         (BVDC_P_FIX_MAX_BITS - BVDC_P_FIX_FRACTION_BITS)
#define BVDC_P_FIX_FRACTION_BITS    BVDC_P_CX_FRACTION_BITS

#define BVDC_P_CX_INT_BITS          (pCscCoeffs->usCxIntBits)
#define BVDC_P_CX_FRACTION_BITS     (pCscCoeffs->usCxFractBits)
#define BVDC_P_CX_MASK              (0xFFFFFFFF >> (BVDC_P_FIX_MAX_BITS - (BVDC_P_CX_INT_BITS + BVDC_P_CX_FRACTION_BITS)))
#define BVDC_P_CX_SIGN_MASK         (1 << (BVDC_P_CX_INT_BITS + BVDC_P_CX_FRACTION_BITS))

#define BVDC_P_CO_INT_BITS          (pCscCoeffs->usCoIntBits)
#define BVDC_P_CO_FRACTION_BITS     (pCscCoeffs->usCoFractBits)
#define BVDC_P_CO_MASK              (0xFFFFFFFF >> (BVDC_P_FIX_MAX_BITS - (BVDC_P_CO_INT_BITS + BVDC_P_CO_FRACTION_BITS)))
#define BVDC_P_CO_SIGN_MASK         (1 << (BVDC_P_CO_INT_BITS + BVDC_P_CO_FRACTION_BITS))

/* fixed point conversion macros */

/* fixed to integer */
#define BVDC_P_CSC_FIXTOI(x) \
	(BMTH_FIX_SIGNED_FIXTOI(x, BVDC_P_FIX_INT_BITS, BVDC_P_FIX_FRACTION_BITS))

/* integer to fixed */
#define BVDC_P_CSC_ITOFIX(x) \
	(BMTH_FIX_SIGNED_ITOFIX(x, BVDC_P_FIX_INT_BITS, BVDC_P_FIX_FRACTION_BITS))

/* fixed to float */
/* Useful for debugging.  Comment out uses of these macros when checking in due to lack of
 * float support on some platforms. */
#define BVDC_P_CSC_CXTOF(x) \
	(((int32_t)((BVDC_P_CX_SIGN_MASK & x) ? -((BVDC_P_CX_MASK & ~x) + 1) : x) / (float)(1 << BVDC_P_CX_FRACTION_BITS)))

#define BVDC_P_CSC_COTOF(x) \
	(((int32_t)((BVDC_P_CO_SIGN_MASK & x) ? -((BVDC_P_CO_MASK & ~x) + 1) : x) / (float)(1 << BVDC_P_CO_FRACTION_BITS)))



/* fixed point operation multiply */
#define BVDC_P_CSC_FIX_MUL(x, y) \
	(BMTH_FIX_SIGNED_MUL(x, y, BVDC_P_FIX_INT_BITS, BVDC_P_FIX_FRACTION_BITS, \
                               BVDC_P_FIX_INT_BITS, BVDC_P_FIX_FRACTION_BITS, \
                               BVDC_P_FIX_INT_BITS, BVDC_P_FIX_FRACTION_BITS))

/* alphas are stored as fractionals and need to be converted to integers
 * for calculations.  Instead of converting to integers by shifting
 * BVDC_P_CSC_VIDEO_DATA_BITS up here, we perform multiplication as if
 * alpha had BVDC_P_CSC_VIDEO_DATA_BITS less fractional bits, which is
 * equivalent and avoids overflow.
 *
 * we also reduce non-alpha offsets to their native fixed-point precision
 * before multiplying to avoid overflow.
 */
#define BVDC_P_CSC_FIX_MUL_OFFSET(x, y, matrix) \
	(BVDC_P_CSC_USE_ALPHA(matrix) ? \
	 BMTH_FIX_SIGNED_MUL(x, y >> BVDC_P_CSC_VIDEO_DATA_BITS, \
	                     BVDC_P_FIX_INT_BITS, BVDC_P_FIX_FRACTION_BITS,   \
                         BVDC_P_FIX_INT_BITS + BVDC_P_CSC_VIDEO_DATA_BITS, BVDC_P_FIX_FRACTION_BITS - BVDC_P_CSC_VIDEO_DATA_BITS,   \
						 BVDC_P_FIX_INT_BITS, BVDC_P_FIX_FRACTION_BITS) : \
	 BMTH_FIX_SIGNED_MUL(x, BVDC_P_CSC_FIXTOCO32(y), \
	                     BVDC_P_FIX_INT_BITS, BVDC_P_FIX_FRACTION_BITS,   \
                         BVDC_P_FIX_MAX_BITS - BVDC_P_CO_FRACTION_BITS, BVDC_P_CO_FRACTION_BITS,   \
						 BVDC_P_FIX_INT_BITS, BVDC_P_FIX_FRACTION_BITS))

/* clamp internal fixed point value to fit within values supported by hardware bits */
#define BVDC_P_CSC_FIX_CLAMPTOCX(x) \
	(((int32_t)x >= (1 << (BVDC_P_CX_INT_BITS + BVDC_P_FIX_FRACTION_BITS)))? \
		((1 << (BVDC_P_CX_INT_BITS + BVDC_P_FIX_FRACTION_BITS)) - 1) : \
	 (((int32_t)x <= ~(1 << (BVDC_P_CX_INT_BITS + BVDC_P_FIX_FRACTION_BITS)))? \
		~((1 << (BVDC_P_CX_INT_BITS + BVDC_P_FIX_FRACTION_BITS)) - 1) : \
	  (int32_t)x))

#define BVDC_P_CSC_FIX_CLAMPTOCO(x) \
	(((int32_t)x >= (1 << (BVDC_P_CO_INT_BITS + BVDC_P_FIX_FRACTION_BITS)))? \
		((1 << (BVDC_P_CO_INT_BITS + BVDC_P_FIX_FRACTION_BITS)) - 1) : \
	 (((int32_t)x <= ~(1 << (BVDC_P_CO_INT_BITS + BVDC_P_FIX_FRACTION_BITS)))? \
		~((1 << (BVDC_P_CO_INT_BITS + BVDC_P_FIX_FRACTION_BITS)) - 1) : \
	  (int32_t)x))

/* fixed point operation divide*/
#define BVDC_P_CSC_FIX_DIV(x, y) \
	(BMTH_FIX_SIGNED_DIV(x, y, BVDC_P_FIX_INT_BITS, BVDC_P_FIX_FRACTION_BITS, \
                               BVDC_P_FIX_INT_BITS, BVDC_P_FIX_FRACTION_BITS, \
                               BVDC_P_FIX_INT_BITS, BVDC_P_FIX_FRACTION_BITS))

/* convert CX coeffs to common fixed notation */
#define BVDC_P_CSC_CXTOFIX(x) \
	(BMTH_FIX_SIGNED_CONVERT(x, BVDC_P_CX_INT_BITS, BVDC_P_CX_FRACTION_BITS, \
                                BVDC_P_FIX_INT_BITS, BVDC_P_FIX_FRACTION_BITS))

/* convert common fixed notation to CX coeffs */
#define BVDC_P_CSC_FIXTOCX(x) \
	(BMTH_FIX_SIGNED_CONVERT(BVDC_P_CSC_FIX_CLAMPTOCX(x), BVDC_P_FIX_INT_BITS, BVDC_P_FIX_FRACTION_BITS, \
                                BVDC_P_CX_INT_BITS, BVDC_P_CX_FRACTION_BITS))

/* convert CO offsets to common fixed notation */
#define BVDC_P_CSC_COTOFIX(x) \
	(BMTH_FIX_SIGNED_CONVERT(x, BVDC_P_CO_INT_BITS, BVDC_P_CO_FRACTION_BITS, \
                                BVDC_P_FIX_INT_BITS, BVDC_P_FIX_FRACTION_BITS))

/* convert common fixed notation to CO offsets */
#define BVDC_P_CSC_FIXTOCO(x) \
	(BMTH_FIX_SIGNED_CONVERT(BVDC_P_CSC_FIX_CLAMPTOCO(x), BVDC_P_FIX_INT_BITS, BVDC_P_FIX_FRACTION_BITS, \
                                BVDC_P_CO_INT_BITS, BVDC_P_CO_FRACTION_BITS))

#define BVDC_P_CSC_FIXTOCO32(x) \
	(BMTH_FIX_SIGNED_CONVERT(x, BVDC_P_FIX_INT_BITS, BVDC_P_FIX_FRACTION_BITS, \
                                BVDC_P_FIX_MAX_BITS - BVDC_P_CO_FRACTION_BITS, BVDC_P_CO_FRACTION_BITS))

/* convert CO offsets to integer representation */
#define BVDC_P_CSC_COTOI(x) \
	(BMTH_FIXTOI(x, BVDC_P_CO_INT_BITS, BVDC_P_CO_FRACTION_BITS))

/* sin, with linear interpolation */
#define BVDC_P_CSC_FIX_SIN(x) \
	(BMTH_FIX_SIGNED_SIN(x, BVDC_P_FIX_INT_BITS, BVDC_P_FIX_FRACTION_BITS, \
	                        BVDC_P_FIX_INT_BITS, BVDC_P_FIX_FRACTION_BITS))

/* cos, with linear interpolation */
#define BVDC_P_CSC_FIX_COS(x) \
	(BMTH_FIX_SIGNED_COS(x, BVDC_P_FIX_INT_BITS, BVDC_P_FIX_FRACTION_BITS, \
	                        BVDC_P_FIX_INT_BITS, BVDC_P_FIX_FRACTION_BITS))

/* Convert csc matrix object to 4x4 matrix of fixed point values */
#define BVDC_P_CSC_MAKE4X4(matrix4x4, cscmatrix)                 \
	matrix4x4[0][0] = BVDC_P_CSC_CXTOFIX(cscmatrix->usY0);     \
	matrix4x4[0][1] = BVDC_P_CSC_CXTOFIX(cscmatrix->usY1);     \
	matrix4x4[0][2] = BVDC_P_CSC_CXTOFIX(cscmatrix->usY2);     \
	matrix4x4[0][3] = BVDC_P_CSC_YOTOFIX(cscmatrix);           \
					                                             \
	matrix4x4[1][0] = BVDC_P_CSC_CXTOFIX(cscmatrix->usCb0);    \
	matrix4x4[1][1] = BVDC_P_CSC_CXTOFIX(cscmatrix->usCb1);    \
	matrix4x4[1][2] = BVDC_P_CSC_CXTOFIX(cscmatrix->usCb2);    \
	matrix4x4[1][3] = BVDC_P_CSC_CBOTOFIX(cscmatrix);          \
					                                             \
	matrix4x4[2][0] = BVDC_P_CSC_CXTOFIX(cscmatrix->usCr0);    \
	matrix4x4[2][1] = BVDC_P_CSC_CXTOFIX(cscmatrix->usCr1);    \
	matrix4x4[2][2] = BVDC_P_CSC_CXTOFIX(cscmatrix->usCr2);    \
	matrix4x4[2][3] = BVDC_P_CSC_CROTOFIX(cscmatrix);          \
						                                         \
	matrix4x4[3][0] = 0;                                         \
	matrix4x4[3][1] = 0;                                         \
	matrix4x4[3][2] = 0;                                         \
	matrix4x4[3][3] = BVDC_P_CSC_ITOFIX(1)

#define BVDC_P_CSC_MAKE4X4_MTH(matrix4x4_mth, cscmatrix)         \
	matrix4x4_mth.ulSize = 4;                                    \
	matrix4x4_mth.ulFractBits = cscmatrix->usCxFractBits;        \
	BVDC_P_CSC_MAKE4X4(matrix4x4_mth.data, cscmatrix)       

/***************************************************************************
 * Return the desired color space coverstion for CSC in VDEC.
 *
 */
void BVDC_P_Csc_GetVdecTable_isr
	( BVDC_P_CscCoeffs                *pCsc,
	  BVDC_P_VdecColorSpace            eInColorSpace,
	  BVDC_P_CmpColorSpace             eOuColorSpace,
	  const BFMT_VideoInfo            *pFmtInfo,
	  bool                             bFullRange );

void BVDC_P_Csc_GetHdDviTable_isr
	( BVDC_P_CscCoeffs                *pCsc,
	  BAVC_CscMode                     eCscMode );

void BVDC_P_Csc_ToMatrix_isr
	( int32_t                          pl32_Matrix[BVDC_CSC_COEFF_COUNT],
	  const BVDC_P_CscCoeffs          *pCsc,
	  uint32_t                         ulShift );

void BVDC_P_Csc_FromMatrix_isr
	( BVDC_P_CscCoeffs                *pCsc,
	  const int32_t                    pl32_Matrix[BVDC_CSC_COEFF_COUNT],
	  uint32_t                         ulShift );

void BVDC_P_Csc_ToMatrixDvo_isr
	( int32_t                          pl32_Matrix[BVDC_CSC_COEFF_COUNT],
	  const BVDC_P_CscCoeffs          *pCsc,
	  uint32_t                         ulShift,
	  bool                             bRgb );

void BVDC_P_Csc_FromMatrixDvo_isr
	( BVDC_P_CscCoeffs                *pCsc,
	  const int32_t                    pl32_Matrix[BVDC_CSC_COEFF_COUNT],
	  uint32_t                         ulShift,
	  bool                             bRgb );

void BVDC_P_Csc_ApplyContrast_isr
	( int16_t                          sContrast,
	  BVDC_P_CscCoeffs                *pCscCoeffs );

void BVDC_P_Csc_ApplySaturationAndHue_isr
	( int16_t                          sSaturation,
	  int16_t                          sHue,
	  BVDC_P_CscCoeffs                *pCscCoeffs );

void BVDC_P_Csc_ApplyBrightness_isr
	( int16_t                          sBrightness,
	  BVDC_P_CscCoeffs                *pCscCoeffs );

void BVDC_P_Csc_ApplyAttenuationRGB_isr
	( int32_t                          lAttenuationR,
	  int32_t                          lAttenuationG,
	  int32_t                          lAttenuationB,
	  int32_t                          lOffsetR,
	  int32_t                          lOffsetG,
	  int32_t                          lOffsetB,
	  BVDC_P_CscCoeffs                *pCscCoeffs,
	  const BVDC_P_CscCoeffs          *pYCbCrToRGB,
	  const BVDC_P_CscCoeffs          *pRGBToYCbCr,
	  bool                             bUserCsc,
	  bool                             bUserCscC);

void BVDC_P_Csc_DvoApplyAttenuationRGB_isr
	( int32_t                          lAttenuationR,
	  int32_t                          lAttenuationG,
	  int32_t                          lAttenuationB,
	  int32_t                          lOffsetR,
	  int32_t                          lOffsetG,
	  int32_t                          lOffsetB,
	  BVDC_P_CscCoeffs                *pCscCoeffs );

BERR_Code BVDC_P_Csc_ColorTempToAttenuationRGB
	( int16_t                          sColorTemp,
	  int32_t                         *plAttenuationR,
	  int32_t                         *plAttenuationG,
	  int32_t                         *plAttenuationB,
	  BVDC_P_CscCoeffs                *pCscCoeffs );

void BVDC_P_Csc_MatrixInverse
	( BVDC_P_CscCoeffs                *pCscCoeffs,
	  BVDC_P_CscCoeffs                *pRetInvCscCoeffs );

void BVDC_P_Csc_ApplyYCbCrColor
	( BVDC_P_CscCoeffs                *pCscCoeffs,
	  uint32_t                         ulColor0,
	  uint32_t                         ulColor1,
	  uint32_t                         ulColor2 );
void BVDC_P_Csc_Print_isr
	( const BVDC_P_CscCoeffs          *pCsc );

#ifdef __cplusplus
}
#endif

#endif /* #ifndef BVDC_CSC_PRIV_H__ */
/* End of file. */
