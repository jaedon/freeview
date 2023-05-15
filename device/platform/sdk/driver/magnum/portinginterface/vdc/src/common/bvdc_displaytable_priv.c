/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvdc_displaytable_priv.c $
 * $brcm_Revision: Hydra_Software_Devel/139 $
 * $brcm_Date: 7/13/12 11:24a $
 *
 * Module Description:
 *   Contains tables for Display settings.
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vdc/src/common/bvdc_displaytable_priv.c $
 * 
 * Hydra_Software_Devel/139   7/13/12 11:24a tdo
 * SW7563-9: Add support for VDC
 *
 * Hydra_Software_Devel/138   6/11/09 4:02p darnstein
 * PR55219: fill in some details for 7340 chipset.
 *
 * Hydra_Software_Devel/137   3/9/09 5:58p pntruong
 * PR52991: No CVBS out on DAC (AH20) from NTSC Demod.  Added ability to
 * output ifd to dac.
 *
 * Hydra_Software_Devel/136   10/23/08 9:39p tdo
 * PR46484:Initial VDC bringup for 7420
 *
 * Hydra_Software_Devel/135   9/16/08 10:17p pntruong
 * PR46494: B0: Remove 3rd output dac.
 *
 * Hydra_Software_Devel/134   4/2/08 11:41a rpan
 * PR39159: Implemented the VEC/DAC side support for SD video pass-
 * through.
 *
 * Hydra_Software_Devel/133   2/13/08 12:24p rpan
 * PR39421: Got 3548 display files to compile.
 *
 * Hydra_Software_Devel/132   9/27/06 4:03p syang
 * PR 24241: add component_only path for 7118's prim vec
 *
 * Hydra_Software_Devel/131   8/29/06 11:01a hongtaoz
 * PR23246: add new QDAC support for 3563; centralize some chip-specific
 * display macroes into bvdc_common_priv.h;
 *
 * Hydra_Software_Devel/130   6/14/06 1:39p syang
 * PR 21689: add support for 7118
 *
 * Hydra_Software_Devel/129   5/19/06 1:47p pntruong
 * PR21560: Global symbols without BVDC prefix in the BVDC module.  Reduce
 * unnecessary global exported symbols.  Tighten const params.
 *
 * Hydra_Software_Devel/128   4/4/06 1:51p hongtaoz
 * PR20403: add triple displays support for 7400;
 *
 * Hydra_Software_Devel/127   3/7/06 4:12p syang
 * PR 19670: added 7438 support
 *
 * Hydra_Software_Devel/126   2/2/06 12:12p hongtaoz
 * PR19082: bring up 7400;
 *
 * Hydra_Software_Devel/125   1/17/06 4:18p hongtaoz
 * PR19082: first compiled for 7400;
 *
 * Hydra_Software_Devel/124   11/29/05 4:57p hongtaoz
 * PR16298, PR16781, PR17804, PR18025, PR18232: implemented 704-sample
 * NTSC analog output; added two more Macrovision control modes; added
 * RGB SCART mode (G channel w/o sync) DAC config for Macrovision
 * validation test;
 *
 * Hydra_Software_Devel/123   9/21/05 7:39p albertl
 * PR16459:  Display format code restructured, moved to
 * bvdc_displayfmt_priv.c and bvdc_displayfmt.h, and changed to be  table
 * driven  to allow ease of adding new formats.
 *
 * Hydra_Software_Devel/122   9/14/05 4:51p hongtaoz
 * PR15749: support analog hsync output for 3560 and 7401;
 *
 * Hydra_Software_Devel/121   9/12/05 2:23p hongtaoz
 * PR16822, PR15749: updated PAL microcode; fixed Macrovision
 * certification control process;
 *
 * Hydra_Software_Devel/120   8/25/05 5:04p albertl
 * PR16407:  Integrated new 1080i 50Hz microcode.
 *
 * Hydra_Software_Devel/119   8/4/05 3:22p syang
 * PR 16563: added new field vf_misc_vf_enable setting for 7401
 *
 * Hydra_Software_Devel/118   8/4/05 10:44a syang
 * PR 16536: refine some "#if (CHIP=xxxx)"  coding for 7401
 *
 * Hydra_Software_Devel/117   8/1/05 4:06p hongtaoz
 * PR16142, PR16394: added 656in analog out vbi pass-thru support for NTSC
 * format; fixed simul mode vdec vbi pass-thru; updated vec NTSC
 * microcode(080105); software workaround 656in hibernation mode
 * artifact;
 *
 * Hydra_Software_Devel/116   6/17/05 4:11p hongtaoz
 * PR15034: put back the infinite loop at the end of PAL microcode;
 *
 * Hydra_Software_Devel/115   5/25/05 2:44p albertl
 * PR15034:  Updated PAL vec microcode and added microvision microcode.
 *
 * Hydra_Software_Devel/114   5/13/05 5:25p hongtaoz
 * PR14950: updated 640x480p dtram microcode to fix Cx timing, but video
 * is still got shifted towards bottom left; back out PAL microcode since
 * it broke Macrovision and dvi output;
 *
 * Hydra_Software_Devel/113   5/9/05 2:34p albertl
 * PR15034:  Updated PAL and NTSC 656 microcode.  Updated PAL vec
 * microcode.
 *
 * Hydra_Software_Devel/112   4/29/05 10:42a hongtaoz
 * PR15037: AGC process always starts from minimum amplitude when change
 * MV types;
 *
 * Hydra_Software_Devel/111   4/27/05 5:56p darnstein
 * PR 14772: Include NTSC/656 output microcode translated by Hong Tao.
 *
 * Hydra_Software_Devel/110   4/25/05 6:30p albertl
 * PR 14772:  Incorporated new 656 NTSC microcode.
 *
 * Hydra_Software_Devel/109   4/25/05 6:25p albertl
 * PR14042:  Updated 480p line drop DTRAM microcode.
 *
 * Hydra_Software_Devel/108   4/19/05 12:15p syang
 * PR 14712: put setting of BVB_SAV_REMOVE and SAV_REPLICATE back (cutting
 * 4 from left for 1080i on B1 hits a known vec buf bug, changed to 3) .
 *
 * Hydra_Software_Devel/107   4/8/05 5:19p syang
 * PR 14712:  shaper in VEC now replicates even number, and cut even
 * number from left (saw odd number cause 1080i tearing in middle of
 * line)
 *
 * Hydra_Software_Devel/106   4/1/05 10:18a hongtaoz
 * PR14608: updated 1080i microcodes timestamp to sync with CVS;
 *
 * Hydra_Software_Devel/105   3/31/05 6:39p syang
 * PR 14537: updated BVB_SAV_REMOVE setting for C0 chip
 *
 * Hydra_Software_Devel/104   3/30/05 10:41a syang
 * PR 14537: added BVB_SAV_REMOVE setting for all chips, and added
 * SAV_REPLICATE setting for 7018 C and later chips.
 *
 * Hydra_Software_Devel/103   3/23/05 3:42p hongtaoz
 * PR14336: updated 480i DVI and analog microcode;
 *
 * Hydra_Software_Devel/102   3/17/05 6:36p pntruong
 * PR14494: Add preliminary software support to 3560 A0.
 *
 * Hydra_Software_Devel/101   3/4/05 2:07p albertl
 * PR13274:  Added BVDC_Display_SetHdmiDropLines and
 * BVDC_Display_GetHdmiDropLines for setting the number of compositor
 * lines to drop for hdmi output.
 *
 * Hydra_Software_Devel/100   3/2/05 11:15a hongtaoz
 * PR14042: updated 480p dvi DTRAM microcode and enabled dvi_dtg
 * auto_restart feature to align digital vs analog video;
 *
 * Hydra_Software_Devel/99   2/10/05 12:26p albertl
 * PR14083:  Updated PAL 656 microcode.
 *
 ***************************************************************************/
#include "bchp_misc.h"
#include "bvdc_display_priv.h"

#if BVDC_P_ORTHOGONAL_VEC

#if (BVDC_P_MAX_DACS > 6)
#define BVDC_P_MAKE_DAC_SEL(channel)        { 0, 0, 0, 0, 0, 0, 0 }
#elif (BVDC_P_MAX_DACS > 5)
#define BVDC_P_MAKE_DAC_SEL(channel)        { 0, 0, 0, 0, 0, 0 }
#elif (BVDC_P_MAX_DACS > 4)
#define BVDC_P_MAKE_DAC_SEL(channel)        { 0, 0, 0, 0, 0 }
#elif (BVDC_P_MAX_DACS > 3)
#define BVDC_P_MAKE_DAC_SEL(channel)        { 0, 0, 0, 0 }
#elif (BVDC_P_MAX_DACS > 2)
#define BVDC_P_MAKE_DAC_SEL(channel)        { 0, 0, 0 }
#elif (BVDC_P_MAX_DACS > 1)
#define BVDC_P_MAKE_DAC_SEL(channel)        { 0, 0 }
#else /* 1 DAC */
#define BVDC_P_MAKE_DAC_SEL(channel)        { 0 }
#endif

#else

#if (BVDC_P_MAX_DACS > 6)
#define BVDC_P_MAKE_DAC_SEL(channel)                              \
{                                                                 \
	BCHP_FIELD_ENUM(MISC_OUT_MUX, DAC0_SEL, channel),             \
	BCHP_FIELD_ENUM(MISC_OUT_MUX, DAC1_SEL, channel),             \
	BCHP_FIELD_ENUM(MISC_OUT_MUX, DAC2_SEL, channel),             \
	BCHP_FIELD_ENUM(MISC_OUT_MUX, DAC3_SEL, channel),             \
	BCHP_FIELD_ENUM(MISC_OUT_MUX, DAC4_SEL, channel),             \
	BCHP_FIELD_ENUM(MISC_OUT_MUX, DAC5_SEL, channel),             \
	BCHP_FIELD_ENUM(MISC_OUT_MUX, DAC6_SEL, channel),             \
}
#elif (BVDC_P_MAX_DACS > 5)
#define BVDC_P_MAKE_DAC_SEL(channel)                              \
{                                                                 \
	BCHP_FIELD_ENUM(MISC_OUT_MUX, DAC0_SEL, channel),             \
	BCHP_FIELD_ENUM(MISC_OUT_MUX, DAC1_SEL, channel),             \
	BCHP_FIELD_ENUM(MISC_OUT_MUX, DAC2_SEL, channel),             \
	BCHP_FIELD_ENUM(MISC_OUT_MUX, DAC3_SEL, channel),             \
	BCHP_FIELD_ENUM(MISC_OUT_MUX, DAC4_SEL, channel),             \
	BCHP_FIELD_ENUM(MISC_OUT_MUX, DAC5_SEL, channel),             \
}
#elif (BVDC_P_MAX_DACS > 4)
#define BVDC_P_MAKE_DAC_SEL(channel)                              \
{                                                                 \
	BCHP_FIELD_ENUM(MISC_OUT_MUX, DAC0_SEL, channel),             \
	BCHP_FIELD_ENUM(MISC_OUT_MUX, DAC1_SEL, channel),             \
	BCHP_FIELD_ENUM(MISC_OUT_MUX, DAC2_SEL, channel),             \
	BCHP_FIELD_ENUM(MISC_OUT_MUX, DAC3_SEL, channel),             \
	BCHP_FIELD_ENUM(MISC_OUT_MUX, DAC4_SEL, channel),             \
}
#elif (BVDC_P_MAX_DACS > 3)
#define BVDC_P_MAKE_DAC_SEL(channel)                              \
{                                                                 \
	BCHP_FIELD_ENUM(MISC_OUT_MUX, DAC0_SEL, channel),             \
	BCHP_FIELD_ENUM(MISC_OUT_MUX, DAC1_SEL, channel),             \
	BCHP_FIELD_ENUM(MISC_OUT_MUX, DAC2_SEL, channel),             \
	BCHP_FIELD_ENUM(MISC_OUT_MUX, DAC3_SEL, channel),             \
}
#elif (BVDC_P_MAX_DACS > 2)
#define BVDC_P_MAKE_DAC_SEL(channel)                              \
{                                                                 \
	BCHP_FIELD_ENUM(MISC_OUT_MUX, DAC0_SEL, channel),             \
	BCHP_FIELD_ENUM(MISC_OUT_MUX, DAC1_SEL, channel),             \
	BCHP_FIELD_ENUM(MISC_OUT_MUX, DAC2_SEL, channel),             \
}
#else /* 2 DACs */
#define BVDC_P_MAKE_DAC_SEL(channel)                              \
{                                                                 \
	BCHP_FIELD_ENUM(MISC_OUT_MUX, DAC0_SEL, channel),             \
	BCHP_FIELD_ENUM(MISC_OUT_MUX, DAC1_SEL, channel),             \
}
#endif
#endif /* orthogonal VEC */

/* Select if needed to use the prim_co or prim */
#if BVDC_P_SUPPORT_PRM_VEC_CMPN_ONLY
#define BVDC_P_MAKE_DAC_SEL_COMP(channel)                         \
	BVDC_P_MAKE_DAC_SEL(PRIM_CO_##channel)
#else
#define BVDC_P_MAKE_DAC_SEL_COMP(channel)                         \
	BVDC_P_MAKE_DAC_SEL(PRIM_##channel)
#endif

/****************************************************************
 *  Tables
 ****************************************************************/
#if (BVDC_P_SUPPORT_TER_VEC)
const uint32_t BVDC_P_aaulTertiaryDacTable[][BVDC_P_MAX_DACS] =
{
	BVDC_P_MAKE_DAC_SEL(TERT_CH0), /* BVDC_DacOutput_eSVideo_Luma */
	BVDC_P_MAKE_DAC_SEL(TERT_CH2), /* BVDC_DacOutput_eSVideo_Chroma */
	BVDC_P_MAKE_DAC_SEL(TERT_CH1), /* BVDC_DacOutput_eComposite */
	BVDC_P_MAKE_DAC_SEL(TERT_CH2), /* BVDC_DacOutput_eRed */
	BVDC_P_MAKE_DAC_SEL(TERT_CH0), /* BVDC_DacOutput_eGreen */
	BVDC_P_MAKE_DAC_SEL(TERT_CH1), /* BVDC_DacOutput_eBlue */
	BVDC_P_MAKE_DAC_SEL(TERT_CH0), /* BVDC_DacOutput_eY */
	BVDC_P_MAKE_DAC_SEL(TERT_CH2), /* BVDC_DacOutput_ePr */
	BVDC_P_MAKE_DAC_SEL(TERT_CH1), /* BVDC_DacOutput_ePb */
	BVDC_P_MAKE_DAC_SEL(TERT_CH0), /* BVDC_DacOutput_eHsync */
	BVDC_P_MAKE_DAC_SEL(TERT_CH0), /* BVDC_DacOutput_eGreen_NoSync */
};
#endif

#if (BVDC_P_SUPPORT_SEC_VEC)
const uint32_t BVDC_P_aaulSecondaryDacTable[][BVDC_P_MAX_DACS] =
{
	BVDC_P_MAKE_DAC_SEL(SEC_CH0),    /* BVDC_DacOutput_eSVideo_Luma */
	BVDC_P_MAKE_DAC_SEL(SEC_CH2),    /* BVDC_DacOutput_eSVideo_Chroma */
	BVDC_P_MAKE_DAC_SEL(SEC_CH1),    /* BVDC_DacOutput_eComposite */
	BVDC_P_MAKE_DAC_SEL(SEC_CO_CH2), /* BVDC_DacOutput_eRed */
	BVDC_P_MAKE_DAC_SEL(SEC_CO_CH0), /* BVDC_DacOutput_eGreen */
	BVDC_P_MAKE_DAC_SEL(SEC_CO_CH1), /* BVDC_DacOutput_eBlue */
	BVDC_P_MAKE_DAC_SEL(SEC_CO_CH0), /* BVDC_DacOutput_eY */
	BVDC_P_MAKE_DAC_SEL(SEC_CO_CH2), /* BVDC_DacOutput_ePr */
	BVDC_P_MAKE_DAC_SEL(SEC_CO_CH1), /* BVDC_DacOutput_ePb */
	BVDC_P_MAKE_DAC_SEL(SEC_HSYNC),  /* BVDC_DacOutput_eHsync */
	BVDC_P_MAKE_DAC_SEL(SEC_CO_CH0), /* BVDC_DacOutput_eGreen_NoSync */
};
#endif

/* Dac settings for Primary Vec path */
const uint32_t BVDC_P_aaulPrimaryDacTable[][BVDC_P_MAX_DACS] =
{
	BVDC_P_MAKE_DAC_SEL(PRIM_CH0),  /* BVDC_DacOutput_eSVideo_Luma */
	BVDC_P_MAKE_DAC_SEL(PRIM_CH2),  /* BVDC_DacOutput_eSVideo_Chroma */
	BVDC_P_MAKE_DAC_SEL(PRIM_CH1),  /* BVDC_DacOutput_eComposite */
	BVDC_P_MAKE_DAC_SEL_COMP(CH2),  /* BVDC_DacOutput_eRed */
	BVDC_P_MAKE_DAC_SEL_COMP(CH0),  /* BVDC_DacOutput_eGreen */
	BVDC_P_MAKE_DAC_SEL_COMP(CH1),  /* BVDC_DacOutput_eBlue */
	BVDC_P_MAKE_DAC_SEL_COMP(CH0),  /* BVDC_DacOutput_eY */
	BVDC_P_MAKE_DAC_SEL_COMP(CH2),  /* BVDC_DacOutput_ePr */
	BVDC_P_MAKE_DAC_SEL_COMP(CH1),  /* BVDC_DacOutput_ePb */
	BVDC_P_MAKE_DAC_SEL(PRIM_CH0),  /* BVDC_DacOutput_eHsync */
	BVDC_P_MAKE_DAC_SEL(PRIM_CH0),  /* BVDC_DacOutput_eGreen_NoSync */
#if (BVDC_P_SUPPORT_VDEC_PASSTHROUGH)
	BVDC_P_MAKE_DAC_SEL(VDEC_DATA), /* BVDC_DacOutput_eVdec0 */
	BVDC_P_MAKE_DAC_SEL(IFDM0),     /* BVDC_DacOutput_eIfdm0 */
	BVDC_P_MAKE_DAC_SEL(IFDM1),     /* BVDC_DacOutput_eIfdm1 */
#endif
};

/* End of File */
