/***************************************************************************
 *     Copyright (c) 2004-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvdc_pep_priv.c $
 * $brcm_Revision: Hydra_Software_Devel/128 $
 * $brcm_Date: 8/6/12 3:09p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vdc/src/common/bvdc_pep_priv.c $
 * 
 * Hydra_Software_Devel/128   8/6/12 3:09p tdo
 * SW7425-3679: sync up TNT programming with the programming guide
 *
 * Hydra_Software_Devel/127   3/30/12 5:05p tdo
 * SW7346-760: Kernel oops during BVDC_P_Pep_BuildRul_isr.  Avoid read
 * HIST stats during vnet reconfig or comp reset
 *
 * Hydra_Software_Devel/126   12/1/11 4:32p yuxiaz
 * SW7425-968, SW7344-95: Merged into mainline.: added independent source
 * clipping of right window in 3D mode.
 *
 * Hydra_Software_Devel/SW7425-968/1   11/3/11 2:28p yuxiaz
 * SW7425-968: Use BVDC_P_Rect in VDC.
 *
 * Hydra_Software_Devel/125   2/16/11 5:14p tdo
 * SW7346-85, SW7422-51 : Software TN2TH Setting
 *
 * Hydra_Software_Devel/124   11/11/10 7:29p albertl
 * SW7125-364: Fixed BVDC_P_CbIsDirty and added assert to check bitfields
 * in dirty bits fit within union integer representation.  Fixed naming
 * of dirty bits.
 *
 * Hydra_Software_Devel/123   6/18/10 8:51p rpan
 * SW7400-2763: Fixed compile errors.
 *
 * Hydra_Software_Devel/122   6/18/10 4:22p rpan
 * SW7400-2763: New BVN resource management.
 *
 * Hydra_Software_Devel/121   5/7/10 7:19p albertl
 * SW7125-364: Changed dirty bits to use union structure to avoid type-pun
 * warnings
 *
 * Hydra_Software_Devel/120   4/19/10 10:18p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic. Move
 * BDBG_OBJECT_ID_DECLARE private header files instead of .c.
 *
 * Hydra_Software_Devel/119   4/7/10 11:33a tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic.  Rename TLA
 *
 * Hydra_Software_Devel/118   4/5/10 4:12p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic
 *
 * Hydra_Software_Devel/117   11/10/09 4:31p tdo
 * SW7400-2574: Adjust TNT based on input and output format
 *
 * Hydra_Software_Devel/116   10/12/09 4:58p tdo
 * SW3548-2536: Improve use of histogram width in Dyn Cont calculations
 *
 * Hydra_Software_Devel/115   9/10/09 2:05p tdo
 * HW7335-382: Change default TAB Luma Peak Coring value
 *
 * Hydra_Software_Devel/114   7/29/09 3:09p tdo
 * PR57181: Adjust coring for TAB
 *
 * Hydra_Software_Devel/113   7/22/09 3:42p tdo
 * PR56975: Correctly set TAB demo mode boundary depending on the location
 * of TAB relative to horizontal and vertical scaler
 *
 * Hydra_Software_Devel/112   3/4/09 4:11p tdo
 * PR51106: Support table base dynamic contrast algorithm.  Change
 * precision of level threshold for histogram data.
 *
 * Hydra_Software_Devel/111   2/19/09 3:25p tdo
 * PR51106: Add Level Stat
 *
 * Hydra_Software_Devel/110   1/16/09 4:20p tdo
 * PR46529, PR51020: Use correct current writer node to store histogram
 * and completely freeze APL when source mute
 *
 * Hydra_Software_Devel/109   1/15/09 1:15p tdo
 * PR46529, PR51020: Implement const delay for histogram for dynamic
 * contrast use, and avoid histogram update when source freezed
 *
 * Hydra_Software_Devel/108   11/12/08 3:13p tdo
 * PR48642: Provide clipping rect for histo luma region
 *
 * Hydra_Software_Devel/107   10/23/08 1:58p tdo
 * PR46529: Implement a consistent delay for the dynamic contrast
 * algorithm
 *
 * Hydra_Software_Devel/106   9/30/08 12:32p jessem
 * PR 46489: Added #include bvdc_tnt_priv.h.
 *
 * Hydra_Software_Devel/105   9/25/08 2:26p tdo
 * PR46529: Implement a consistent delay for histogram data for the
 * dynamic contrast algorithm
 *
 * Hydra_Software_Devel/104   9/17/08 8:44p pntruong
 * PR47013: B0: Initial vdc support.
 *
 * Hydra_Software_Devel/103   9/17/08 1:48p tdo
 * PR46948: Add Backlight Dimming in Dynamic Contrast code
 *
 * Hydra_Software_Devel/102   8/28/08 4:11p tdo
 * PR45896: Add new dynamic back-light callback
 *
 * Hydra_Software_Devel/101   7/30/08 11:35a tdo
 * PR45181: Implement New Dynamic Contrast Algorithm
 *
 ***************************************************************************/
#include "bvdc_compositor_priv.h"
#include "bvdc_source_priv.h"
#include "bvdc_pep_priv.h"
#include "bvdc_tnt_priv.h"
#include "bvdc_hist_priv.h"
#if (BVDC_P_SUPPORT_PEP_VER == BVDC_P_SUPPORT_PEP_VER_4)
#include "bchp_vivid_cmp_0.h"
#endif

BDBG_MODULE(BVDC_PEP);
BDBG_OBJECT_ID(BVDC_PEP);

#if (BVDC_P_SUPPORT_MASK_DITHER)
#include "bchp_mask_0.h"
#endif

#if (BVDC_P_SUPPORT_PEP)

#if (BVDC_P_SUPPORT_PEP_VER == BVDC_P_SUPPORT_PEP_VER_3)
static const uint32_t s_aulBlueStretchCbTbl[] =
{
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   1,   2,   3,   4,   4,   4,   4,
	  4,   5,   6,   7,   8,   8,   8,   8,   8,   9,  10,  11,  12,  13,  14,  15,
	 16,  16,  16,  16,  16,  17,  18,  19,  20,  20,  20,  20,  20,  21,  22,  23,
	 24,  25,  26,  27,  28,  28,  28,  28,  28,  29,  30,  31,  32,  32,  32,  32,
	 32,  33,  34,  35,  36,  37,  38,  39,  40,  40,  40,  40,  40,  41,  42,  43,
	 44,  44,  44,  44,  44,  45,  46,  47,  48,  49,  50,  51,  52,  52,  52,  52,
	 52,  53,  54,  55,  56,  56,  56,  56,  56,  57,  58,  59,  60,  61,  62,  63,
	 64,  64,  64,  64,  64,  65,  66,  67,  68,  68,  68,  68,  68,  69,  70,  71,
	 72,  73,  74,  75,  76,  76,  76,  76,  76,  77,  78,  79,  80,  80,  80,  80,
	 80,  81,  82,  83,  84,  85,  86,  87,  88,  88,  88,  88,  88,  89,  90,  91,
	 92,  92,  92,  92,  92,  93,  94,  95,  96,  97,  98,  99, 100, 100, 100, 100,
	100, 101, 102, 103, 104, 104, 104, 104, 104, 105, 106, 107, 108, 109, 110, 111,
	112, 112, 112, 112, 112, 113, 114, 115, 116, 116, 116, 116, 116, 117, 118, 119,
	120, 119, 118, 117, 116, 116, 116, 116, 116, 115, 114, 113, 112, 111, 110, 109,
	108, 107, 106, 105, 104, 104, 104, 104, 104, 103, 102, 101, 100,  99,  98,  97,
	 96,  95,  94,  93,  92,  92,  92,  92,  92,  91,  90,  89,  88,  87,  86,  85,
	 84,  83,  82,  81,  80,  80,  80,  80,  80,  79,  78,  77,  76,  75,  74,  73,
	 72,  71,  70,  69,  68,  68,  68,  68,  68,  67,  66,  65,  64,  63,  62,  61,
	 60,  59,  58,  57,  56,  56,  56,  56,  56,  55,  54,  53,  52,  51,  50,  49,
	 48,  47,  46,  45,  44,  44,  44,  44,  44,  43,  42,  41,  40,  39,  38,  37,
	 36,  35,  34,  33,  32,  32,  32,  32,  32,  31,  30,  29,  28,  27,  26,  25,
	 24,  23,  22,  21,  20,  20,  20,  20,  20,  19,  18,  17,  16,  15,  14,  13,
	 12,  11,  10,   9,   8,   8,   8,   8,   8,   7,   6,   5,   4,   5,   6,   7,
};

static const uint32_t s_aulBlueStretchCrTbl[] =
{
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0, 255, 254, 253, 252, 252, 252, 252, 252, 252, 252, 252, 252, 252, 252, 252,
	252, 252, 252, 252, 252, 252, 252, 252, 252, 251, 250, 249, 248, 248, 248, 248,
	248, 248, 248, 248, 248, 248, 248, 248, 248, 248, 248, 248, 248, 247, 246, 245,
	244, 244, 244, 244, 244, 244, 244, 244, 244, 244, 244, 244, 244, 244, 244, 244,
	244, 244, 244, 244, 244, 243, 242, 241, 240, 240, 240, 240, 240, 240, 240, 240,
	240, 240, 240, 240, 240, 240, 240, 240, 240, 240, 240, 240, 240, 240, 240, 240,
	240, 240, 240, 240, 240, 240, 240, 240, 240, 240, 240, 240, 240, 240, 240, 240,
	240, 240, 240, 240, 240, 240, 240, 240, 240, 240, 240, 240, 240, 240, 240, 240,
	240, 240, 240, 240, 240, 240, 240, 240, 240, 240, 240, 240, 240, 240, 240, 240,
	240, 240, 240, 240, 240, 240, 240, 240, 240, 240, 240, 240, 240, 240, 240, 240,
	240, 240, 240, 240, 240, 240, 240, 240, 240, 240, 240, 240, 240, 240, 240, 240,
	240, 240, 240, 240, 240, 240, 240, 240, 240, 240, 240, 240, 240, 240, 240, 240,
	240, 240, 240, 240, 240, 240, 240, 240, 240, 240, 240, 240, 240, 240, 240, 240,
	240, 240, 240, 240, 240, 240, 240, 240, 240, 240, 240, 240, 240, 240, 240, 240,
	240, 240, 240, 240, 240, 240, 240, 240, 240, 240, 240, 240, 240, 240, 240, 240,
	240, 240, 240, 240, 240, 240, 240, 240, 240, 240, 240, 240, 240, 240, 240, 240,
	240, 240, 240, 240, 240, 240, 240, 240, 240, 240, 240, 240, 240, 240, 240, 240,
	240, 240, 240, 240, 240, 240, 240, 240, 240, 240, 240, 240, 240, 240, 240, 240,
	240, 241, 242, 243, 244, 244, 244, 244, 244, 244, 244, 244, 244, 244, 244, 244,
	244, 245, 246, 247, 248, 248, 248, 248, 248, 248, 248, 248, 248, 248, 248, 248,
	248, 248, 248, 248, 248, 249, 250, 251, 252, 252, 252, 252, 252, 252, 252, 252,
	252, 252, 252, 252, 252, 253, 254, 255,   0,   0,   0,   0,   0,   1,   2,   3,
};
#endif


/*************************************************************************
 *  {secret}
 *	BVDC_P_Cab_BuildRul_isr
 *	Builds CAB block
 **************************************************************************/
static void BVDC_P_Cab_BuildRul_isr
	( const BVDC_Window_Handle     hWindow,
	  BVDC_P_Pep_Handle            hPep,
	  BVDC_P_ListInfo             *pList )
{
	const BVDC_P_Window_Info    *pCurInfo;
	bool                         bCabEnable = false;
	bool                         bDemoModeEnable = false;

	BDBG_ENTER(BVDC_P_Cab_BuildRul_isr);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	BDBG_OBJECT_ASSERT(hPep, BVDC_PEP);

	/* Get current pointer to RUL */
	pCurInfo = &hWindow->stCurInfo;

	/* Only enable CAB if there is CAB table available */
	/* right now only 10 bits CAB tables are supported */
	bCabEnable = ((pCurInfo->ulFleshtone    != 0) ||
	              (pCurInfo->ulBlueBoost  != 0) ||
	              (pCurInfo->ulGreenBoost != 0) ||
	              (BVDC_P_PEP_CMS_IS_ENABLE(&pCurInfo->stSatGain, &pCurInfo->stHueGain)) ||
	              (pCurInfo->bUserCabEnable)) ? true : false;
	bDemoModeEnable = (((pCurInfo->ulFleshtone    != 0 ||
	                     pCurInfo->ulBlueBoost  != 0 ||
	                     pCurInfo->ulGreenBoost != 0) &&
	                    (pCurInfo->stSplitScreenSetting.eAutoFlesh != BVDC_SplitScreenMode_eDisable)) ||
	                   (BVDC_P_PEP_CMS_IS_ENABLE(&pCurInfo->stSatGain, &pCurInfo->stHueGain) &&
	                    pCurInfo->stSplitScreenSetting.eCms != BVDC_SplitScreenMode_eDisable));
	BDBG_MSG(("User CAB table = %s, CAB_ENABLE = %s, DEMO_MODE = %s",
		(pCurInfo->bUserCabEnable == true) ? "true" : "false",
		(bCabEnable == true) ? "true" : "false",
		(bDemoModeEnable == true) ? "true" : "false"));

	/* Demo mode setting for Auto Flesh, Blue Boost and Green Boost */
	/* has to be the same, since they are applied to the same CAB table */
	BDBG_ASSERT(pCurInfo->stSplitScreenSetting.eAutoFlesh == pCurInfo->stSplitScreenSetting.eGreenBoost);
	BDBG_ASSERT(pCurInfo->stSplitScreenSetting.eAutoFlesh == pCurInfo->stSplitScreenSetting.eBlueBoost);

#if (BVDC_P_SUPPORT_PEP_VER <= BVDC_P_SUPPORT_PEP_VER_3)
	/* Build RUL */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_PEP_CMP_0_V0_CAB_CTRL);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(PEP_CMP_0_V0_CAB_CTRL, CAB_DEMO_ENABLE, bDemoModeEnable) |
		BCHP_FIELD_DATA(PEP_CMP_0_V0_CAB_CTRL, CAB_ENABLE, bCabEnable) |
		BCHP_FIELD_DATA(PEP_CMP_0_V0_CAB_CTRL, LUMA_OFFSET_ENABLE, 0);

	/* Setting up the demo mode register */
	if(bDemoModeEnable)
	{
		uint32_t ulBoundary = hWindow->stAdjDstRect.ulWidth / 2;
		uint32_t ulDemoSide;

		if(BVDC_P_PEP_CMS_IS_ENABLE(&pCurInfo->stSatGain, &pCurInfo->stHueGain) &&
		   pCurInfo->stSplitScreenSetting.eCms != BVDC_SplitScreenMode_eDisable)
		{
			ulDemoSide = (pCurInfo->stSplitScreenSetting.eCms == BVDC_SplitScreenMode_eLeft) ?
			                   BCHP_PEP_CMP_0_V0_CAB_DEMO_SETTING_DEMO_L_R_LEFT :
			                   BCHP_PEP_CMP_0_V0_CAB_DEMO_SETTING_DEMO_L_R_RIGHT;
		}
		else
		{
			ulDemoSide = (pCurInfo->stSplitScreenSetting.eAutoFlesh == BVDC_SplitScreenMode_eLeft) ?
			                   BCHP_PEP_CMP_0_V0_CAB_DEMO_SETTING_DEMO_L_R_LEFT :
			                   BCHP_PEP_CMP_0_V0_CAB_DEMO_SETTING_DEMO_L_R_RIGHT;
		}

		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_PEP_CMP_0_V0_CAB_DEMO_SETTING);
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(PEP_CMP_0_V0_CAB_DEMO_SETTING, DEMO_L_R, ulDemoSide) |
			BCHP_FIELD_DATA(PEP_CMP_0_V0_CAB_DEMO_SETTING, DEMO_BOUNDARY, ulBoundary) ;

		BDBG_MSG(("CAB Demo Mode: L_R = %s, BOUNDARY = %d",
		          ((ulDemoSide == BCHP_PEP_CMP_0_V0_CAB_DEMO_SETTING_DEMO_L_R_LEFT) ? "L" : "R"),
		          ulBoundary));
	}

	if(bCabEnable)
	{
		/* Set flag to load user CAB table to the HW when the slot is avaiable */
		hPep->bLoadCabTable = true;
		/* Force CAB table to be loaded in same vsync cycle with CAB enable */
		hPep->bProcessCab   = true;
	}
#else
	BSTD_UNUSED(hPep);
	BSTD_UNUSED(pList);
#endif

	BDBG_LEAVE(BVDC_P_Cab_BuildRul_isr);
	return;
}


/*************************************************************************
 *  {secret}
 *	BVDC_P_Lab_BuildRul_isr
 *	Builds LAB block
 **************************************************************************/
static void BVDC_P_Lab_BuildRul_isr
	( const BVDC_Window_Handle     hWindow,
	  BVDC_P_Pep_Handle            hPep,
	  BVDC_P_ListInfo             *pList )
{
	const BVDC_P_Window_Info    *pCurInfo;
	uint32_t ulTpOrMask;
	uint32_t ulTpAndMask;

	BDBG_ENTER(BVDC_P_Lab_BuildRul_isr);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	BDBG_OBJECT_ASSERT(hPep, BVDC_PEP);

	/* Get current pointer to RUL */
	pCurInfo = &hWindow->stCurInfo;

	/* Setting up the demo mode register */
	if(pCurInfo->stSplitScreenSetting.eContrastStretch != BVDC_SplitScreenMode_eDisable)
	{
#if (BVDC_P_SUPPORT_PEP_VER == BVDC_P_SUPPORT_PEP_VER_4)
		/* Assume that CM3D will set this register if its demo mode is enable */
		if(pCurInfo->stSplitScreenSetting.eCm3d == BVDC_SplitScreenMode_eDisable)
		{
			*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
			*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VIVID_CMP_0_DEMO_SETTING);
			*pList->pulCurrent++ =
				BCHP_FIELD_DATA(VIVID_CMP_0_DEMO_SETTING, DEMO_L_R,
				               (pCurInfo->stSplitScreenSetting.eContrastStretch == BVDC_SplitScreenMode_eLeft) ?
				                BCHP_VIVID_CMP_0_DEMO_SETTING_DEMO_L_R_LEFT :
				                BCHP_VIVID_CMP_0_DEMO_SETTING_DEMO_L_R_RIGHT) |
				BCHP_FIELD_DATA(VIVID_CMP_0_DEMO_SETTING, DEMO_BOUNDARY,
				                hWindow->stAdjDstRect.ulWidth / 2) ;
		}
#else
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_PEP_CMP_0_V0_LAB_DEMO_SETTING);
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(PEP_CMP_0_V0_LAB_DEMO_SETTING, DEMO_L_R,
			               (pCurInfo->stSplitScreenSetting.eContrastStretch == BVDC_SplitScreenMode_eLeft) ?
			                BCHP_PEP_CMP_0_V0_LAB_DEMO_SETTING_DEMO_L_R_LEFT :
			                BCHP_PEP_CMP_0_V0_LAB_DEMO_SETTING_DEMO_L_R_RIGHT) |
			BCHP_FIELD_DATA(PEP_CMP_0_V0_LAB_DEMO_SETTING, DEMO_BOUNDARY,
			                hWindow->stAdjDstRect.ulWidth / 2) ;
#endif

		BDBG_MSG(("LAB Demo Mode: L_R = %s, BOUNDARY = %d",
		         (pCurInfo->stSplitScreenSetting.eContrastStretch == BVDC_SplitScreenMode_eLeft) ? "L" : "R",
		          hWindow->stAdjDstRect.ulWidth / 2));
	}

#if (BVDC_P_SUPPORT_PEP_VER == BVDC_P_SUPPORT_PEP_VER_3)
	BDBG_MSG(("blue stretch slope = %d, offset = %d",
		pCurInfo->stBlueStretch.ulBlueStretchSlope,
		pCurInfo->stBlueStretch.ulBlueStretchOffset));
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_PEP_CMP_0_V0_BLUE_STRETCH_PARAM);
	*pList->pulCurrent++ =
			BCHP_FIELD_DATA(PEP_CMP_0_V0_BLUE_STRETCH_PARAM, BLUE_STRETCH_SLOPE,
				pCurInfo->stBlueStretch.ulBlueStretchSlope) |
			BCHP_FIELD_DATA(PEP_CMP_0_V0_BLUE_STRETCH_PARAM, BLUE_STRETCH_OFFSET,
				pCurInfo->stBlueStretch.ulBlueStretchOffset) ;
#endif

#if (BVDC_P_SUPPORT_PEP_VER == BVDC_P_SUPPORT_PEP_VER_4)
	/* Build up AND/OR mask for RUL read/modify/write */
	hPep->bLabCtrlPending = (pCurInfo->bContrastStretch && !hPep->bLabTableValid) ? true : false;

	if(!hPep->bLabCtrlPending)
	{
		ulTpAndMask =
			BCHP_MASK(VIVID_CMP_0_BYPASS_CTRL, BYPASS_VIVID        ) |
			BCHP_MASK(VIVID_CMP_0_BYPASS_CTRL, BYPASS_LAB_SAT_GAIN ) |
			BCHP_MASK(VIVID_CMP_0_BYPASS_CTRL, BYPASS_LAB_Y        );
		ulTpOrMask =
			(!pCurInfo->bContrastStretch &&
			 !pCurInfo->bCm3dRegionEnable &&
			 !pCurInfo->bCm3dOverlapRegionEnable) ?
			BCHP_FIELD_ENUM(VIVID_CMP_0_BYPASS_CTRL, BYPASS_VIVID , Bypass) :
			BCHP_FIELD_ENUM(VIVID_CMP_0_BYPASS_CTRL, BYPASS_VIVID , Normal);
		if(pCurInfo->bContrastStretch && hPep->bLabTableValid)
		{
			ulTpOrMask |=
				BCHP_FIELD_ENUM(VIVID_CMP_0_BYPASS_CTRL, BYPASS_LAB_SAT_GAIN, Normal) |
				BCHP_FIELD_ENUM(VIVID_CMP_0_BYPASS_CTRL, BYPASS_LAB_Y,        Normal);
		}
		else
		{
			ulTpOrMask |=
				BCHP_FIELD_ENUM(VIVID_CMP_0_BYPASS_CTRL, BYPASS_LAB_SAT_GAIN, Bypass) |
				BCHP_FIELD_ENUM(VIVID_CMP_0_BYPASS_CTRL, BYPASS_LAB_Y,        Bypass);
		}
		BVDC_P_RD_MOD_WR_RUL(pList->pulCurrent, ~ulTpAndMask, ulTpOrMask,
			BCHP_VIVID_CMP_0_BYPASS_CTRL);

		ulTpAndMask =
			BCHP_MASK(VIVID_CMP_0_DEMO_CTRL, DEMO_ENA_LAB_SAT_GAIN ) |
			BCHP_MASK(VIVID_CMP_0_DEMO_CTRL, DEMO_ENA_LAB_Y        );
		if(pCurInfo->stSplitScreenSetting.eContrastStretch == BVDC_SplitScreenMode_eDisable)
		{
			ulTpOrMask =
				BCHP_FIELD_ENUM(VIVID_CMP_0_DEMO_CTRL, DEMO_ENA_LAB_SAT_GAIN, Normal) |
				BCHP_FIELD_ENUM(VIVID_CMP_0_DEMO_CTRL, DEMO_ENA_LAB_Y,        Normal);
		}
		else
		{
			ulTpOrMask =
				BCHP_FIELD_ENUM(VIVID_CMP_0_DEMO_CTRL, DEMO_ENA_LAB_SAT_GAIN, Demo) |
				BCHP_FIELD_ENUM(VIVID_CMP_0_DEMO_CTRL, DEMO_ENA_LAB_Y,        Demo);
		}
		BVDC_P_RD_MOD_WR_RUL(pList->pulCurrent, ~ulTpAndMask, ulTpOrMask,
			BCHP_VIVID_CMP_0_DEMO_CTRL);
	}
#else
	/* Defer LAB enable if CAB table is being processed this vsync */
	hPep->bLabCtrlPending = ((pCurInfo->bContrastStretch || pCurInfo->bUserLabLuma) &&
	                          hPep->bProcessCab) ? true : false;

	if(!hPep->bLabCtrlPending)
	{
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_PEP_CMP_0_V0_LAB_CTRL);
		*pList->pulCurrent++ =
#if (BVDC_P_SUPPORT_PEP_VER == BVDC_P_SUPPORT_PEP_VER_3)
			((pCurInfo->bBlueStretch) ?
			  BCHP_FIELD_ENUM(PEP_CMP_0_V0_LAB_CTRL, LAB_BLUE_STRETCH_ENABLE, ENABLE) :
			  BCHP_FIELD_ENUM(PEP_CMP_0_V0_LAB_CTRL, LAB_BLUE_STRETCH_ENABLE, DISABLE)) |
#endif
			((pCurInfo->stSplitScreenSetting.eContrastStretch == BVDC_SplitScreenMode_eDisable) ?
			  BCHP_FIELD_DATA(PEP_CMP_0_V0_LAB_CTRL, LAB_DEMO_ENABLE, 0) :
			  BCHP_FIELD_DATA(PEP_CMP_0_V0_LAB_CTRL, LAB_DEMO_ENABLE, 1)) |
			((pCurInfo->bContrastStretch || pCurInfo->bBlueStretch ||
			  pCurInfo->bUserLabLuma || pCurInfo->bUserLabCbCr) ?
			  BCHP_FIELD_DATA(PEP_CMP_0_V0_LAB_CTRL, ENABLE, 1):
			  BCHP_FIELD_DATA(PEP_CMP_0_V0_LAB_CTRL, ENABLE, 0));

		if(pCurInfo->bUserLabLuma || pCurInfo->bUserLabCbCr ||
		   (pCurInfo->bBlueStretch && !pCurInfo->bUserLabCbCr))
		{
			/* Set flag to load user LAB table to the HW when the slot is avaiable */
			hPep->bLoadLabTable = true;
			/* Set flag to delay CAB table loaded one vsync so that the first LAB */
			/* table can be loaded in one vsync with LAB enable */
			hPep->bProcessCab   = false;
		}

		BDBG_MSG(("Contrast %s, Blue %s, Usr luma %s, Usr CbCr %s, load tbl %s",
			(pCurInfo->bContrastStretch) ? "enabled" : "disabled",
			(pCurInfo->bBlueStretch) ? "enabled" : "disabled",
			(pCurInfo->bUserLabLuma) ? "enabled" : "disabled",
			(pCurInfo->bUserLabCbCr) ? "enabled" : "disabled",
			(hPep->bLoadLabTable) ? "enabled" : "disabled"));
	}
#endif

	BSTD_UNUSED(ulTpOrMask);
	BSTD_UNUSED(ulTpAndMask);
	BDBG_LEAVE(BVDC_P_Lab_BuildRul_isr);
	return;
}

#endif /* (BVDC_P_SUPPORT_PEP) */


#if(BVDC_P_SUPPORT_HIST)
#if(BVDC_P_SUPPORT_HIST_VER == BVDC_P_SUPPORT_HIST_VER_2)
static void BVDC_P_Pep_ReadHistoData_isr
	( const BVDC_Window_Handle     hWindow,
	  BVDC_P_Pep_Handle            hPep,
	  BVDC_P_HistContext          *pHist )
{
	uint32_t id;
	BVDC_P_PictureNode       *pPicture;
	BVDC_P_SourceContext     *pSource;

	BDBG_ENTER(BVDC_P_Pep_ReadHistoData_isr);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	pSource = hWindow->stCurInfo.hSource;
	BDBG_OBJECT_ASSERT(pSource, BVDC_SRC);
	BDBG_OBJECT_ASSERT(hPep, BVDC_PEP);

	/* Get current pointer to RUL */
	pPicture = hWindow->pCurReaderNode;

	if(hWindow->stCurInfo.bHistAtSrc)
	{
		/* If source mute, reader freezed, no need to update histogram data */
		if(pSource->stCurInfo.eMuteMode == BVDC_MuteMode_eDisable)
		{
			/* Get the actual histogram data size from picture node */
			hPep->ulHistSize = pPicture->ulCurHistSize;

			for(id = 0; id < hPep->ulHistSize; id++)
			{
				/* Keeping the last 3 histo counts used by contrast stretch algorithm */
				hPep->aulLastLastBin[id] = hPep->aulLastBin[id];
				hPep->aulLastBin[id] = hPep->stHistoData.aulHistogram[id];
			}

			hPep->stHistoData = pPicture->stCurHistData;
		}
		else
		{
			pHist->stFreezedHistData = hPep->stHistoData;
		}
	}
	else
	{
		/* Get the actual histogram data size from HIST block */
		hPep->ulHistSize = pHist->ulHistSize;

		for(id = 0; id < hPep->ulHistSize; id++)
		{
			/* Keeping the last 3 histo counts used by contrast stretch algorithm */
			hPep->aulLastLastBin[id] = hPep->aulLastBin[id];
			hPep->aulLastBin[id] = hPep->stHistoData.aulHistogram[id];
		}

		hPep->stHistoData = pHist->stHistData;
	}

	BDBG_LEAVE(BVDC_P_Pep_ReadHistoData_isr);
}
#elif(BVDC_P_SUPPORT_HIST_VER == BVDC_P_SUPPORT_HIST_VER_1)
/*************************************************************************
 *  {secret}
 *	BVDC_P_Histo_BuildRul_isr
 *	Builds Histogram block
 **************************************************************************/
static void BVDC_P_Histo_BuildRul_isr
	( const BVDC_Window_Handle     hWindow,
	  bool                         bInterlaced,
	  BVDC_P_ListInfo             *pList )
{
	const BVDC_P_Window_Info    *pCurInfo;
	BVDC_P_Rect                  stRect;

	BDBG_ENTER(BVDC_P_Histo_BuildRul_isr);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	/* Get current pointer to RUL */
	pCurInfo = &hWindow->stCurInfo;

	BVDC_P_CalculateRect_isr(&pCurInfo->stLumaRect.stRegion,
	                         pCurInfo->stDstRect.ulWidth,
	                         pCurInfo->stDstRect.ulHeight,
	                         bInterlaced, &stRect);
	BDBG_MSG(("Dst Rect: %d x %d => Hist Rec: %d x %d",
		pCurInfo->stDstRect.ulWidth, pCurInfo->stDstRect.ulHeight,
		stRect.ulWidth, stRect.ulHeight));

	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_PEP_CMP_0_V0_HISTO_WIN_OFFSET);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(PEP_CMP_0_V0_HISTO_WIN_OFFSET, X_OFFSET, stRect.lLeft) |
		BCHP_FIELD_DATA(PEP_CMP_0_V0_HISTO_WIN_OFFSET, Y_OFFSET, stRect.lTop);

	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_PEP_CMP_0_V0_HISTO_WIN_SIZE);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(PEP_CMP_0_V0_HISTO_WIN_SIZE, HSIZE, stRect.ulWidth) |
		BCHP_FIELD_DATA(PEP_CMP_0_V0_HISTO_WIN_SIZE, VSIZE, stRect.ulHeight);

	BDBG_LEAVE(BVDC_P_Histo_BuildRul_isr);
	return;
}

/*************************************************************************
 *  {secret}
 *	BVDC_P_Histo_UpdateHistoData_isr
 *	Reading the histogram data every vysnc, called from Window Reader isr
 **************************************************************************/
void BVDC_P_Histo_UpdateHistoData_isr
	( BVDC_P_Pep_Handle            hPep )
{
	uint32_t id;
	uint32_t ulReg;

	BDBG_ENTER(BVDC_P_Histo_UpdateHistoData_isr);
	BDBG_OBJECT_ASSERT(hPep, BVDC_PEP);

	for(id = 0; id < hPep->ulHistSize; id++)
	{
		hPep->stTmpHistoData.aulHistogram[id] = BREG_Read32(hPep->hReg,
		    BCHP_PEP_CMP_0_V0_HISTO_DATA_COUNT_i_ARRAY_BASE + (id * 4));
	}

	ulReg = BREG_Read32(hPep->hReg, BCHP_PEP_CMP_0_V0_HISTO_MIN_MAX);
	hPep->stTmpHistoData.ulMin = BCHP_GET_FIELD_DATA(ulReg, PEP_CMP_0_V0_HISTO_MIN_MAX, MIN);
	hPep->stTmpHistoData.ulMax = BCHP_GET_FIELD_DATA(ulReg, PEP_CMP_0_V0_HISTO_MIN_MAX, MAX);

	BDBG_LEAVE(BVDC_P_Histo_UpdateHistoData_isr);
}

/*************************************************************************
 *  {secret}
 *	BVDC_P_Pep_ReadHistoData_isr
 *	Reading the histogram data
 **************************************************************************/
static void BVDC_P_Pep_ReadHistoData_isr
	( const BVDC_Window_Handle     hWindow,
	  BVDC_P_Pep_Handle            hPep,
	  const BVDC_P_HistContext    *pHist )
{
	uint32_t id;

	BSTD_UNUSED(pHist);
	BSTD_UNUSED(hWindow);
	BDBG_ENTER(BVDC_P_Pep_ReadHistoData_isr);
	BDBG_OBJECT_ASSERT(hPep, BVDC_PEP);

	for(id = 0; id < hPep->ulHistSize; id++)
	{
		/* Keeping the last 3 histo counts used by contrast stretch algorithm */
		hPep->aulLastLastBin[id] = hPep->aulLastBin[id];
		hPep->aulLastBin[id] = hPep->stHistoData.aulHistogram[id];
		hPep->stHistoData.aulHistogram[id] = hPep->stTmpHistoData.aulHistogram[id];
	}

	hPep->stHistoData.ulMin = hPep->stTmpHistoData.ulMin;
	hPep->stHistoData.ulMax = hPep->stTmpHistoData.ulMax;

	BDBG_LEAVE(BVDC_P_Pep_ReadHistoData_isr);
}

#endif /* BVDC_P_SUPPORT_HIST_VER */
#endif /* BVDC_P_SUPPORT_HIST */


/***************************************************************************
 * {private}
 *
 */
BERR_Code BVDC_P_Pep_Create
	( BVDC_P_Pep_Handle            *phPep,
	  const BVDC_P_WindowId         eWinId,
	  const BREG_Handle             hReg )
{
	BVDC_P_PepContext *pPep;

	BDBG_ENTER(BVDC_P_Pep_Create);

	BDBG_ASSERT(phPep);

	/* (1) Alloc the context. */
	pPep = (BVDC_P_PepContext*)
		(BKNI_Malloc(sizeof(BVDC_P_PepContext)));
	if(!pPep)
	{
		return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
	}

	/* Clear out the context and set defaults. */
	BKNI_Memset((void*)pPep, 0x0, sizeof(BVDC_P_PepContext));
	BDBG_OBJECT_SET(pPep, BVDC_PEP);

	pPep->eId              = eWinId;
	pPep->hReg             = hReg;

	/* All done. now return the new fresh context to user. */
	*phPep = (BVDC_P_Pep_Handle)pPep;

	BDBG_LEAVE(BVDC_P_Pep_Create);
	return BERR_SUCCESS;
}


/***************************************************************************
 * {private}
 *
 */
void BVDC_P_Pep_Destroy
	( BVDC_P_Pep_Handle             hPep )
{
	BDBG_ENTER(BVDC_P_Pep_Destroy);

	BDBG_OBJECT_ASSERT(hPep, BVDC_PEP);

	BDBG_OBJECT_DESTROY(hPep, BVDC_PEP);
	/* [1] Free the context. */
	BKNI_Free((void*)hPep);

	BDBG_LEAVE(BVDC_P_Pep_Destroy);
	return;
}


/***************************************************************************
 * {private}
 *
 */
void BVDC_P_Pep_Init
	( const BVDC_P_Pep_Handle       hPep )
{
	BDBG_ENTER(BVDC_P_Pep_Init);
	BDBG_OBJECT_ASSERT(hPep, BVDC_PEP);

	hPep->bInitial = true;

	/* Clear out the context and set defaults. */
	BKNI_Memset((void*)hPep->aulLabTable,      0x0, sizeof(hPep->aulLabTable));
	BKNI_Memset((void*)hPep->aulLastBin,       0x0, sizeof(hPep->aulLastBin));
	BKNI_Memset((void*)hPep->aulLastLastBin,   0x0, sizeof(hPep->aulLastLastBin));
	BKNI_Memset((void*)hPep->lFixEstLuma,      0x0, sizeof(hPep->lFixEstLuma));
	BKNI_Memset((void*)hPep->lFixHist_out,     0x0, sizeof(hPep->lFixHist_out));
	BKNI_Memset((void*)&hPep->stHistoData,     0x0, sizeof(BVDC_LumaStatus));
	BKNI_Memset((void*)&hPep->stTmpHistoData,  0x0, sizeof(BVDC_LumaStatus));
	BKNI_Memset((void*)&hPep->ulAvgLevelStats, 0x0, sizeof(hPep->ulAvgLevelStats));

	hPep->bLoadCabTable  = false;
	hPep->bLoadLabTable  = false;
	hPep->bProcessCab    = true;
	hPep->bProcessHist   = false;
	hPep->bLabTableValid = false;
	hPep->bLabCtrlPending= false;
	hPep->lFixLastMin    = 0;
	hPep->lFixLastMax    = 0;
	hPep->lFixLastMid    = 0;
	hPep->lFixBrtCur     = 0;
	hPep->lFixBrtLast    = 0;

	hPep->ulHistSize       = BVDC_P_HISTO_TABLE_SIZE;
	hPep->ulLumaChromaGain = 0x3f;

	hPep->ulAvgAPL       = 0;
	hPep->ulFiltAPL      = 0;
	hPep->lLastGain      = 0;
	hPep->stCallbackData.ulShift = 8;
	hPep->stCallbackData.iScalingFactor = 0;

	BDBG_LEAVE(BVDC_P_Pep_Init);
	return;
}


/*************************************************************************
 *  {secret}
 *	BVDC_P_Dither_BuildRul_isr
 *	Builds MASK (Dithering) block
 **************************************************************************/
static void BVDC_P_Dither_BuildRul_isr
	( const BVDC_Window_Handle     hWindow,
	  BVDC_P_ListInfo             *pList )
{
#if (BVDC_P_SUPPORT_MASK_DITHER)
	const BVDC_DitherSettings *pDither;
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	pDither = &hWindow->stCurInfo.stDither;

	/* Update the dithering registers */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_MASK_0_CONTROL);
	*pList->pulCurrent++ =
		BCHP_FIELD_ENUM(MASK_0_CONTROL, TEXTURE_ENABLE, DISABLE ) |
		BCHP_FIELD_DATA(MASK_0_CONTROL, REDUCE_SMOOTH,
			pDither->bReduceSmooth                              ) |
		BCHP_FIELD_DATA(MASK_0_CONTROL, SMOOTH_ENABLE,
			pDither->bSmoothEnable                              );

	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_MASK_0_SMOOTH_LIMIT);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(MASK_0_SMOOTH_LIMIT, VALUE,
			pDither->ulSmoothLimit                              );

	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_MASK_0_TEXTURE_FREQ);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(MASK_0_TEXTURE_FREQ, HORIZ_FREQ, 8 ) |
		BCHP_FIELD_DATA(MASK_0_TEXTURE_FREQ, VERT_FREQ,  8 );

	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_MASK_0_RANDOM_PATTERN);
	*pList->pulCurrent++ =
		BCHP_FIELD_ENUM(MASK_0_RANDOM_PATTERN, RNG_MODE, RUN  ) |
		BCHP_FIELD_DATA(MASK_0_RANDOM_PATTERN, RNG_SEED, 4369 );

	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_MASK_0_SCALE_0_1_2_3);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(MASK_0_SCALE_0_1_2_3, MULT_0,  0 ) |
		BCHP_FIELD_DATA(MASK_0_SCALE_0_1_2_3, SHIFT_0, 4 ) |
		BCHP_FIELD_DATA(MASK_0_SCALE_0_1_2_3, MULT_1,  0 ) |
		BCHP_FIELD_DATA(MASK_0_SCALE_0_1_2_3, SHIFT_1, 4 ) |
		BCHP_FIELD_DATA(MASK_0_SCALE_0_1_2_3, MULT_2,  0 ) |
		BCHP_FIELD_DATA(MASK_0_SCALE_0_1_2_3, SHIFT_2, 4 ) |
		BCHP_FIELD_DATA(MASK_0_SCALE_0_1_2_3, MULT_3,  0 ) |
		BCHP_FIELD_DATA(MASK_0_SCALE_0_1_2_3, SHIFT_3, 4 );

	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_MASK_0_SCALE_4_5_6_7);
	*pList->pulCurrent++ =
		BCHP_FIELD_DATA(MASK_0_SCALE_4_5_6_7, MULT_4,  0 ) |
		BCHP_FIELD_DATA(MASK_0_SCALE_4_5_6_7, SHIFT_4, 4 ) |
		BCHP_FIELD_DATA(MASK_0_SCALE_4_5_6_7, MULT_5,  0 ) |
		BCHP_FIELD_DATA(MASK_0_SCALE_4_5_6_7, SHIFT_5, 4 ) |
		BCHP_FIELD_DATA(MASK_0_SCALE_4_5_6_7, MULT_6,  0 ) |
		BCHP_FIELD_DATA(MASK_0_SCALE_4_5_6_7, SHIFT_6, 4 ) |
		BCHP_FIELD_DATA(MASK_0_SCALE_4_5_6_7, MULT_7,  0 ) |
		BCHP_FIELD_DATA(MASK_0_SCALE_4_5_6_7, SHIFT_7, 4 );
#else
	BSTD_UNUSED(hWindow);
	BSTD_UNUSED(pList);
#endif

	return;
}


/*************************************************************************
 *  {secret}
 *	BVDC_P_Pep_BuildVsyncRul_isr
 *	Builds LAB contrast stretch every vsync
 **************************************************************************/
static void BVDC_P_Pep_BuildVsyncRul_isr
	( const BVDC_Window_Handle     hWindow,
	  BVDC_P_Pep_Handle            hPep,
	  BVDC_P_ListInfo             *pList )
{
	const BVDC_P_Window_Info *pCurInfo;

	BDBG_ENTER(BVDC_P_Pep_BuildVsyncRul_isr);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	BDBG_OBJECT_ASSERT(hPep, BVDC_PEP);

	/* Get current pointer to RUL */
	pCurInfo = &hWindow->stCurInfo;

	BVDC_P_Tnt_BuildVysncRul_isr(hWindow, pList);


#if(BVDC_P_SUPPORT_HIST)
#if(BVDC_P_SUPPORT_HIST_VER <= BVDC_P_SUPPORT_HIST_VER_1)
	/* Process HIST data here */
	/* Old PEP arch, histogram data is read every frame */
	if(hPep->bProcessHist == true)
	{
		/* Build RUL to reset histogram every other field and collect  */
		/* the histogram the alternate field */
		BVDC_P_Pep_ReadHistoData_isr(hWindow, hPep, NULL);

		/* Reset histogram and min_max */
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_PEP_CMP_0_V0_HISTO_RESET);
		*pList->pulCurrent++ = 1;

		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_PEP_CMP_0_V0_MIN_MAX_RESET);
		*pList->pulCurrent++ = 1;

		/* Enable HISTO_CTRL and MIN_MAX_CTRL every other vsync since */
		/* these registers are cleared automatically */
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_PEP_CMP_0_V0_HISTO_CTRL);
		*pList->pulCurrent++ = BCHP_FIELD_ENUM(PEP_CMP_0_V0_HISTO_CTRL, HISTO_CTRL, SINGLE_PICTURE);

		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_PEP_CMP_0_V0_MIN_MAX_CTRL);
		*pList->pulCurrent++ = BCHP_FIELD_ENUM(PEP_CMP_0_V0_MIN_MAX_CTRL, MIN_MAX_CTRL, SINGLE_PICTURE);

		hPep->bProcessHist = false;
	}
	else
	{
		hPep->bProcessHist = true;
	}
#elif (BVDC_P_SUPPORT_HIST_VER == BVDC_P_SUPPORT_HIST_VER_2)
	/* Update pep histogram data every vsync with new HIST block if dynamic
	   contrast is enabled */
	if(pCurInfo->bContrastStretch && hWindow->stCurResource.hHist)
	{
		BDBG_OBJECT_ASSERT(hWindow->stCurResource.hHist, BVDC_HST);
		BVDC_P_Pep_ReadHistoData_isr(hWindow, hPep, hWindow->stCurResource.hHist);
	}
#endif /* BVDC_P_SUPPORT_HIST_VER */
#endif /* BVDC_P_SUPPORT_HIST */

#if(BVDC_P_SUPPORT_PEP)
#if(BVDC_P_SUPPORT_PEP_VER == BVDC_P_SUPPORT_PEP_VER_4)
	/* Build LAB table every vsync if dynamic contrast is enabled */
	if(pCurInfo->bContrastStretch)
	{
		hWindow->sFinalBrightness = pCurInfo->sBrightness;
		hWindow->sFinalContrast   = pCurInfo->sContrast;
		hWindow->sFinalSaturation = pCurInfo->sSaturation;
		BVDC_P_Pep_DynamicContrast_isr(&pCurInfo->stContrastStretch,
		                                hPep,
		                               &hWindow->sFinalSaturation,
		                               &hWindow->sFinalBrightness,
		                               &hWindow->sFinalContrast,
		                               &hPep->aulLabTable[0]);

		*pList->pulCurrent++ = BRDC_OP_IMMS_TO_REGS(BVDC_P_LAB_TABLE_SIZE);
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VIVID_CMP_0_LAB_LUT_DATA_i_ARRAY_BASE);
		BKNI_Memcpy((void*)pList->pulCurrent, (void*)&hPep->aulLabTable[0], BVDC_P_LAB_TABLE_SIZE * sizeof(uint32_t));
		pList->pulCurrent += BVDC_P_LAB_TABLE_SIZE;
	}
#else
	/* We are alternating the service slot for CAB and LAB table every other */
	/* field to avoid overloading the RUL, so at the max each Vsync, there   */
	/* will be 1024 entries from ther LAB or CAB table loaded */
	if(hPep->bProcessCab == true)
	{
		hPep->bProcessCab = false;
		if(hPep->bLoadCabTable)
		{
			BDBG_MSG(("Build RUL to load CAB table"));
			*pList->pulCurrent++ = BRDC_OP_IMMS_TO_REGS(BVDC_P_CAB_TABLE_SIZE);
			*pList->pulCurrent++ = BRDC_REGISTER(BCHP_PEP_CMP_0_V0_CAB_LUT_DATA_i_ARRAY_BASE);
			BKNI_Memcpy((void*)pList->pulCurrent, (void*)&pCurInfo->aulCabTable[0], BVDC_P_CAB_TABLE_SIZE * sizeof(uint32_t));
			pList->pulCurrent += BVDC_P_CAB_TABLE_SIZE;
			hPep->bLoadCabTable = false;
		}
	}
	else
	{
		uint32_t id = 0;

		if(pCurInfo->bContrastStretch || hPep->bLoadLabTable)
		{
			*pList->pulCurrent++ = BRDC_OP_IMMS_TO_REGS(BVDC_P_LAB_TABLE_SIZE);
			*pList->pulCurrent++ = BRDC_REGISTER(BCHP_PEP_CMP_0_V0_LAB_LUT_DATA_i_ARRAY_BASE);
			if(pCurInfo->bContrastStretch == true)
			{
				/* compute LAB table if contrast stretch is enabled */
				hWindow->sFinalBrightness = pCurInfo->sBrightness;
				hWindow->sFinalContrast   = pCurInfo->sContrast;
				hWindow->sFinalSaturation = pCurInfo->sSaturation;
				BVDC_P_Pep_DynamicContrast_isr(&pCurInfo->stContrastStretch,
				                               hPep,
				                               &hWindow->sFinalSaturation,
				                               &hWindow->sFinalBrightness,
				                               &hWindow->sFinalContrast,
				                               &hPep->aulLabTable[0]);

				if(pCurInfo->bUserLabCbCr || pCurInfo->bBlueStretch)
				{
					for(id = 0; id < BVDC_P_LAB_TABLE_SIZE; id++)
					{
						hPep->aulLabTable[id] &= ~(
							BCHP_MASK(PEP_CMP_0_V0_LAB_LUT_DATA_i, CB_OFFSET) |
							BCHP_MASK(PEP_CMP_0_V0_LAB_LUT_DATA_i, CR_OFFSET));
						if(pCurInfo->bUserLabCbCr)
						{
							/* Merge with user Cb and Cr tables */
							hPep->aulLabTable[id] |= (
								BCHP_FIELD_DATA(PEP_CMP_0_V0_LAB_LUT_DATA_i, CB_OFFSET, pCurInfo->aulLabCbTbl[id]) |
								BCHP_FIELD_DATA(PEP_CMP_0_V0_LAB_LUT_DATA_i, CR_OFFSET, pCurInfo->aulLabCrTbl[id]));
						}
#if (BVDC_P_SUPPORT_PEP_VER == BVDC_P_SUPPORT_PEP_VER_3)
						else
						{
							/* Merge with internal Cb and Cr tables for blue stretch */
							hPep->aulLabTable[id] |= (
								BCHP_FIELD_DATA(PEP_CMP_0_V0_LAB_LUT_DATA_i, CB_OFFSET, s_aulBlueStretchCbTbl[id]) |
								BCHP_FIELD_DATA(PEP_CMP_0_V0_LAB_LUT_DATA_i, CR_OFFSET, s_aulBlueStretchCrTbl[id]));
						}
#endif
					}
				}
			} else if(hPep->bLoadLabTable)
			{
				BDBG_MSG(("Build RUL to load LAB table"));

				for(id = 0; id < BVDC_P_LAB_TABLE_SIZE; id++)
				{
					if(pCurInfo->bUserLabLuma)
					{
						/* Merge with user luma table */
						hPep->aulLabTable[id] &= ~(
							BCHP_MASK(PEP_CMP_0_V0_LAB_LUT_DATA_i, LUMA_DATA));
						hPep->aulLabTable[id] = (
							BCHP_FIELD_DATA(PEP_CMP_0_V0_LAB_LUT_DATA_i, LUMA_DATA, pCurInfo->aulLabLumaTbl[id]));
					}
					else if(pCurInfo->bBlueStretch)
					{
						/* use identity for luma table */
						hPep->aulLabTable[id] &= ~(
							BCHP_MASK(PEP_CMP_0_V0_LAB_LUT_DATA_i, LUMA_DATA));
						hPep->aulLabTable[id] = (
							BCHP_FIELD_DATA(PEP_CMP_0_V0_LAB_LUT_DATA_i, LUMA_DATA, id));
					}
					if(pCurInfo->bUserLabCbCr)
					{
						/* Merge with user Cb and Cr tables */
						hPep->aulLabTable[id] &= ~(
							BCHP_MASK(PEP_CMP_0_V0_LAB_LUT_DATA_i, CB_OFFSET) |
							BCHP_MASK(PEP_CMP_0_V0_LAB_LUT_DATA_i, CR_OFFSET));
						hPep->aulLabTable[id] |= (
							BCHP_FIELD_DATA(PEP_CMP_0_V0_LAB_LUT_DATA_i, CB_OFFSET, pCurInfo->aulLabCbTbl[id]) |
							BCHP_FIELD_DATA(PEP_CMP_0_V0_LAB_LUT_DATA_i, CR_OFFSET, pCurInfo->aulLabCrTbl[id]));
					}
#if (BVDC_P_SUPPORT_PEP_VER == BVDC_P_SUPPORT_PEP_VER_3)
					else if(pCurInfo->bBlueStretch)
					{
						/* Merge with internal Cb and Cr tables for blue stretch */
						hPep->aulLabTable[id] &= ~(
							BCHP_MASK(PEP_CMP_0_V0_LAB_LUT_DATA_i, CB_OFFSET) |
							BCHP_MASK(PEP_CMP_0_V0_LAB_LUT_DATA_i, CR_OFFSET));
						hPep->aulLabTable[id] |= (
							BCHP_FIELD_DATA(PEP_CMP_0_V0_LAB_LUT_DATA_i, CB_OFFSET, s_aulBlueStretchCbTbl[id]) |
							BCHP_FIELD_DATA(PEP_CMP_0_V0_LAB_LUT_DATA_i, CR_OFFSET, s_aulBlueStretchCrTbl[id]));
					}
#endif
				}
				hPep->bLoadLabTable = false;
			}

			BKNI_Memcpy((void*)pList->pulCurrent, (void*)&hPep->aulLabTable[0], BVDC_P_LAB_TABLE_SIZE * sizeof(uint32_t));
			pList->pulCurrent += BVDC_P_LAB_TABLE_SIZE;
		}

		hPep->bProcessCab = true;
	}

#endif /* BVDC_P_SUPPORT_PEP_VER */
#else
	BSTD_UNUSED(hPep);
	BSTD_UNUSED(pList);
#endif /* BVDC_P_SUPPORT_PEP */

	BDBG_LEAVE(BVDC_P_Pep_BuildVsyncRul_isr);
	return;
}

/*************************************************************************
 *  {secret}
 *	BVDC_P_Pep_BuildRul_isr
 *	Builds PEP block
 **************************************************************************/
void BVDC_P_Pep_BuildRul_isr
	( const BVDC_Window_Handle     hWindow,
	  BVDC_P_ListInfo             *pList,
	  bool                         bInitial )
{
	BVDC_P_Window_DirtyBits       *pCurDirty;

	BDBG_ENTER(BVDC_P_Pep_BuildRul_isr);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	pCurDirty = &hWindow->stCurInfo.stDirty;

	/* Only C0_V0 has PEP */
	BDBG_ASSERT(BVDC_P_WindowId_eComp0_V0 == hWindow->eId);

	BDBG_OBJECT_ASSERT(hWindow->stCurResource.hPep, BVDC_PEP);
	bInitial |= hWindow->stCurResource.hPep->bInitial;

#if (BVDC_P_SUPPORT_HIST && BVDC_P_SUPPORT_HIST_VER <= BVDC_P_SUPPORT_HIST_VER_1)
	if(pCurDirty->stBits.bHistoRect || bInitial)
	{
		BVDC_P_Histo_BuildRul_isr(hWindow, hWindow->hCompositor->stCurInfo.pFmtInfo->bInterlaced, pList);
		pCurDirty->stBits.bHistoRect = BVDC_P_CLEAN;
	}
#endif

#if (BVDC_P_SUPPORT_PEP)
	if(pCurDirty->stBits.bCabAdjust || bInitial)
	{
		BVDC_P_Cab_BuildRul_isr(hWindow, hWindow->stCurResource.hPep, pList);
		pCurDirty->stBits.bCabAdjust = BVDC_P_CLEAN;
	}

	if(pCurDirty->stBits.bLabAdjust || bInitial || hWindow->stCurResource.hPep->bLabCtrlPending)
	{
		BVDC_P_Lab_BuildRul_isr(hWindow, hWindow->stCurResource.hPep, pList);
		pCurDirty->stBits.bLabAdjust = BVDC_P_CLEAN;
	}

#else
	/* never dirty since hardware doesn't exist */
	pCurDirty->stBits.bLabAdjust = BVDC_P_CLEAN;
	pCurDirty->stBits.bCabAdjust = BVDC_P_CLEAN;
#endif

	if(pCurDirty->stBits.bTntAdjust|| bInitial)
	{
		if(bInitial)
		{
			BVDC_P_Tnt_BuildInit_isr(hWindow, pList);
		}
		BVDC_P_Tnt_BuildRul_isr(hWindow, pList);
		pCurDirty->stBits.bTntAdjust = BVDC_P_CLEAN;
	}

	if(pCurDirty->stBits.bDitAdjust || bInitial)
	{
		BVDC_P_Dither_BuildRul_isr(hWindow, pList);
		pCurDirty->stBits.bDitAdjust = BVDC_P_CLEAN;
	}

	BVDC_P_Pep_BuildVsyncRul_isr(hWindow, hWindow->stCurResource.hPep, pList);
	hWindow->stCurResource.hPep->bInitial = false;

	BDBG_LEAVE(BVDC_P_Pep_BuildRul_isr);
	return;
}


/*************************************************************************
 *  {secret}
 *	BVDC_P_Pep_GetLumaStatus
 *	Get Luma status from PEP block
 **************************************************************************/
void BVDC_P_Pep_GetLumaStatus
	( const BVDC_Window_Handle     hWindow,
	  BVDC_LumaStatus             *pLumaStatus )
{
	BDBG_ENTER(BVDC_P_Pep_GetLumaStatus);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	BDBG_OBJECT_ASSERT(hWindow->stCurResource.hPep, BVDC_PEP);

	BKNI_EnterCriticalSection();
	*pLumaStatus = hWindow->stCurResource.hPep->stHistoData;
	BKNI_LeaveCriticalSection();

	BDBG_LEAVE(BVDC_P_Pep_GetLumaStatus);
	return;
}


/*************************************************************************
 *  {secret}
 *	BVDC_P_Tab_BuildRul_isr
 *	Builds TAB block
 **************************************************************************/
void BVDC_P_Tab_BuildRul_isr
	( const BVDC_Window_Handle     hWindow,
	  BVDC_P_ListInfo             *pList )
{
	BVDC_P_Window_Info    *pCurInfo;

	BDBG_ENTER(BVDC_P_Tab_BuildRul_isr);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	/* Only C0_V0 has PEP */
	BDBG_ASSERT(BVDC_P_WindowId_eComp0_V0 == hWindow->eId);

	/* Get current pointer to RUL */
	pCurInfo = &hWindow->stCurInfo;

#if (BVDC_P_SUPPORT_TAB)
	if(pCurInfo->bSharpnessEnable)
	{
		BDBG_MSG(("TAB Sharpness = %d => Peak Setting = %d, Peak Scale = %d",
		           pCurInfo->sSharpness, pCurInfo->ulSharpnessPeakSetting,
		           pCurInfo->ulSharpnessPeakScale));

		/* Build RUL */
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_TAB_0_CONFIGURATION_REG);
		*pList->pulCurrent++ =
			((pCurInfo->stSplitScreenSetting.eSharpness == BVDC_SplitScreenMode_eDisable) ?
			BCHP_FIELD_ENUM(TAB_0_CONFIGURATION_REG, DEMO_ENABLE, DISABLE) :
			BCHP_FIELD_ENUM(TAB_0_CONFIGURATION_REG, DEMO_ENABLE, ENABLE)) |
			BCHP_FIELD_ENUM(TAB_0_CONFIGURATION_REG, TAB_ENABLE, NORMAL) |
			BCHP_FIELD_ENUM(TAB_0_CONFIGURATION_REG, LUMA_MEDIAN, ONE_TAP_LUMA_MEDIAN) |
			BCHP_FIELD_ENUM(TAB_0_CONFIGURATION_REG, CHROMA_MEDIAN, THREE_TAP_CHROMA_MEDIAN) |
			BCHP_FIELD_ENUM(TAB_0_CONFIGURATION_REG, DISABLE_CHROMA_LUMA, DISABLE) |
			BCHP_FIELD_DATA(TAB_0_CONFIGURATION_REG, CHROMA_LUMA_SETTING, 0x0) |
			BCHP_FIELD_ENUM(TAB_0_CONFIGURATION_REG, DISABLE_CHROMA_CHROMA, DISABLE) |
			BCHP_FIELD_DATA(TAB_0_CONFIGURATION_REG, CHROMA_CHROMA_SETTING, 0x0);

		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_TAB_0_LUMA_PEAK_CORRECTION_REG);
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(TAB_0_LUMA_PEAK_CORRECTION_REG, OLD_CORE_FUNCTION, 0)    |
			BCHP_FIELD_DATA(TAB_0_LUMA_PEAK_CORRECTION_REG, OVERSHOOT,         0xFF) |
			BCHP_FIELD_DATA(TAB_0_LUMA_PEAK_CORRECTION_REG, PEAKCORE,          0x10) |
			BCHP_FIELD_DATA(TAB_0_LUMA_PEAK_CORRECTION_REG, PEAK_SCALE,        pCurInfo->ulSharpnessPeakScale) |
			BCHP_FIELD_DATA(TAB_0_LUMA_PEAK_CORRECTION_REG, PEAK_SETTING,      pCurInfo->ulSharpnessPeakSetting);

		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_TAB_0_LUMA_EDGE_ENHANCEMENT_REG);
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(TAB_0_LUMA_EDGE_ENHANCEMENT_REG, EDGECORE, 0x3) |
			BCHP_FIELD_DATA(TAB_0_LUMA_EDGE_ENHANCEMENT_REG, EDGEMAX, 0x14) |
			BCHP_FIELD_DATA(TAB_0_LUMA_EDGE_ENHANCEMENT_REG, EDGE_SCALE, 0x6) |
			BCHP_FIELD_DATA(TAB_0_LUMA_EDGE_ENHANCEMENT_REG, EDGE_SETTING, 0x0);

		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_TAB_0_CHROMA_PEAKING_REG);
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(TAB_0_CHROMA_PEAKING_REG, SATCORE, 0x0) |
			BCHP_FIELD_DATA(TAB_0_CHROMA_PEAKING_REG, SATMAX, 0x14) |
			BCHP_FIELD_ENUM(TAB_0_CHROMA_PEAKING_REG, SAT_SETTING, DISABLE) |
			BCHP_FIELD_DATA(TAB_0_CHROMA_PEAKING_REG, SATSCALE, 0x6);

		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_TAB_0_CHROMA_MODULATED_FILTER_REG);
		*pList->pulCurrent++ = 0x55150501;

		/* Setting up the demo mode register */
		if(pCurInfo->stSplitScreenSetting.eSharpness != BVDC_SplitScreenMode_eDisable)
		{
			uint32_t ulBoundary = (hWindow->stSrcCnt.ulWidth < hWindow->stAdjSclOut.ulWidth) ?
			                      (hWindow->stSrcCnt.ulWidth / 2) :
			                      (hWindow->stAdjSclOut.ulWidth / 2);
			uint32_t ulDemoSide = (pCurInfo->stSplitScreenSetting.eSharpness == BVDC_SplitScreenMode_eLeft) ?
			                       BCHP_TAB_0_DEMO_SETTING_REG_DEMO_L_R_LEFT :
			                       BCHP_TAB_0_DEMO_SETTING_REG_DEMO_L_R_RIGHT;

			*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
			*pList->pulCurrent++ = BRDC_REGISTER(BCHP_TAB_0_DEMO_SETTING_REG);
			*pList->pulCurrent++ =
				BCHP_FIELD_DATA(TAB_0_DEMO_SETTING_REG, DEMO_L_R, ulDemoSide) |
				BCHP_FIELD_DATA(TAB_0_DEMO_SETTING_REG, DEMO_BOUNDARY, ulBoundary);

			BDBG_MSG(("TAB Demo Mode: L_R = %s, BOUNDARY = %d",
			          ((ulDemoSide == BCHP_TAB_0_DEMO_SETTING_REG_DEMO_L_R_LEFT) ? "L" : "R"),
			          ulBoundary));
		}
	}
	else
	{
		/* Bypass TAB */
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_TAB_0_CONFIGURATION_REG);
		*pList->pulCurrent++ = (
			BCHP_FIELD_ENUM(TAB_0_CONFIGURATION_REG, DEMO_ENABLE, DISABLE) |
			BCHP_FIELD_ENUM(TAB_0_CONFIGURATION_REG, TAB_ENABLE, BYPASS_IN_OUT) |
			BCHP_FIELD_ENUM(TAB_0_CONFIGURATION_REG, DISABLE_CHROMA_LUMA, DISABLE) |
			BCHP_FIELD_ENUM(TAB_0_CONFIGURATION_REG, DISABLE_CHROMA_CHROMA, DISABLE));

		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_TAB_0_CHROMA_PEAKING_REG);
		*pList->pulCurrent++ = (
			BCHP_FIELD_ENUM(TAB_0_CHROMA_PEAKING_REG, SAT_SETTING, DISABLE));
	}

#else
	BSTD_UNUSED(pList);
#endif /* (BVDC_P_SUPPORT_TAB) */

	pCurInfo->stDirty.stBits.bTabAdjust = false;

	BDBG_LEAVE(BVDC_P_Tab_BuildRul_isr);
	return;
}

/* End of File */
