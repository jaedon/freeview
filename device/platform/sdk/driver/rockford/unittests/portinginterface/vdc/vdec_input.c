/***************************************************************************
 *     Copyright (c) 2004-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: vdec_input.c $
 * $brcm_Revision: Hydra_Software_Devel/14 $
 * $brcm_Date: 3/23/10 3:44p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/portinginterface/vdc/vdec_input.c $
 * 
 * Hydra_Software_Devel/14   3/23/10 3:44p pntruong
 * SW3548-2846: Added the ifdemod case input for ease of testing with
 * unittest.
 *
 * Hydra_Software_Devel/13   10/17/08 5:59p pntruong
 * PR47013: Fixed connector swapped.
 *
 * Hydra_Software_Devel/12   9/20/08 10:34a pntruong
 * PR34711: Added example of external ifd.
 *
 * Hydra_Software_Devel/11   6/17/08 7:20p pntruong
 * PR40922: Some more tuning for SCART input.
 *
 * Hydra_Software_Devel/10   6/12/08 10:19p pntruong
 * PR34711: Updated mapping for PC input.
 *
 * Hydra_Software_Devel/9   6/12/08 2:19p pntruong
 * PR34711: Updated video input mapping for 93549.
 *
 * Hydra_Software_Devel/8   6/11/08 9:15p pntruong
 * PR34711: Updated video input mapping for 93556.
 *
 * Hydra_Software_Devel/7   6/11/08 1:21p tdo
 * PR34711: Adjust adc input table for 3556 and 3548
 *
 * Hydra_Software_Devel/6   6/11/08 10:20a tdo
 * PR34711: Adjust adc input table for 3556 and 3548
 *
 * Hydra_Software_Devel/5   6/5/08 8:43a pntruong
 * PR34711: Fixed pin for svideo.
 *
 * Hydra_Software_Devel/4   5/19/08 2:27p pntruong
 * PR34711: Corrected enumeration indexing, and added sanity check for
 * table.
 *
 * Hydra_Software_Devel/3   5/16/08 3:08p pntruong
 * PR34711: Updated to include new inputs.
 *
 * Hydra_Software_Devel/2   10/16/07 2:47p rpan
 * PR34857: First version after merging the latest xvd_vdc_test and
 * mvd_vdc_test as of Oct. 16, 2007, 12pm.
 *
 * Hydra_Software_Devel/1   9/27/07 4:13p rpan
 * PR34857: Copied over as is from mvd_vdc_test.
 *
 *
 ***************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bstd.h"
#include "bdbg.h"
#include "bkni.h"

#include "vdec_input.h"

BDBG_MODULE(VDEC_INPUT);


#if (BCHP_CHIP==3556)
/* Shorthand to make entry */
#define BTST_P_MAKE_INPUT(adc, ch0, ch1, ch2, ch3, ch0_type, ch1_type, ch2_type, ch3_type) \
{                                                                           \
	BTST_VdecInput_e##adc,                                                  \
	(ch0), (ch1), (ch2), (ch3),                                                   \
	BVDC_AdcInput_e##ch0_type,                                              \
	BVDC_AdcInput_e##ch1_type,                                              \
	BVDC_AdcInput_e##ch2_type,                                              \
	BVDC_AdcInput_e##ch3_type,                                              \
	#adc                                                                    \
}

/* Lookup table that is board specific. */
typedef struct
{
	BTST_VdecInput            eInput;
	uint32_t                  ulCh0;   /* Y/Luma/Cvbs/Ifd */
	uint32_t                  ulCh1;   /* Pr/Chroma */
	uint32_t                  ulCh2;   /* Pb */
	uint32_t                  ulCh3;   /* Pb */
	BVDC_AdcInput             eCh0;    /* Colorspace/sync of each channel */
	BVDC_AdcInput             eCh1;
	BVDC_AdcInput             eCh2;
	BVDC_AdcInput             eCh3;
	const char               *pchName;

} BTST_P_InputEntry;

#else
/* Shorthand to make entry */
#define BTST_P_MAKE_INPUT(adc, ch0, ch1, ch2, ch0_type, ch1_type, ch2_type) \
{                                                                           \
	BTST_VdecInput_e##adc,                                                  \
	(ch0), (ch1), (ch2),                                                    \
	BVDC_AdcInput_e##ch0_type,                                              \
	BVDC_AdcInput_e##ch1_type,                                              \
	BVDC_AdcInput_e##ch2_type,                                              \
	#adc                                                                    \
}

/* Lookup table that is board specific. */
typedef struct
{
	BTST_VdecInput            eInput;
	uint32_t                  ulCh0;   /* Y/Luma/Cvbs/Ifd */
	uint32_t                  ulCh1;   /* Pr/Chroma */
	uint32_t                  ulCh2;   /* Pb */
	BVDC_AdcInput             eCh0;    /* Colorspace/sync of each channel */
	BVDC_AdcInput             eCh1;
	BVDC_AdcInput             eCh2;
	const char               *pchName;

} BTST_P_InputEntry;

#endif

/* 93563 Chip/Board specific;  Need to look at schematics */
static BTST_P_InputEntry s_aInput[] =
{
#if (BCHP_CHIP==3548)
	BTST_P_MAKE_INPUT(Ifd_0,   BVDC_Ibafe_0, 0, 0, IfDemod, Unused, Unused),
	BTST_P_MAKE_INPUT(Ifd_1,   BVDC_Adc_17,  0, 0, IfDemod, Unused, Unused),
	BTST_P_MAKE_INPUT(Cvbs_0,  BVDC_Adc_15,  0, 0, Composite, Unused, Unused),
	BTST_P_MAKE_INPUT(Cvbs_1,  BVDC_Adc_16,  0, 0, Composite, Unused, Unused),
	BTST_P_MAKE_INPUT(Cvbs_2,  BVDC_Adc_17,  0, 0, IfDemod, Unused, Unused),
	BTST_P_MAKE_INPUT(Cvbs_3,  BVDC_Adc_17,  0, 0, Unused,    Unused, Unused),
	BTST_P_MAKE_INPUT(Svid_0,  BVDC_Adc_9,   BVDC_Adc_10, 0, Luma, Chroma, Unused),
	BTST_P_MAKE_INPUT(Svid_1,  BVDC_Adc_11,  BVDC_Adc_12, 0, Luma, Chroma, Unused),
	BTST_P_MAKE_INPUT(Svid_2,  BVDC_Adc_13,  BVDC_Adc_14, 0, Luma, Chroma, Unused),
#if (BCHP_VER == BCHP_VER_A0)
	BTST_P_MAKE_INPUT(YPrPb_0, BVDC_Adc_6,   BVDC_Adc_7, BVDC_Adc_8, Y, Pb, Pr),
#else
	BTST_P_MAKE_INPUT(YPrPb_0, BVDC_Adc_6,   BVDC_Adc_7, BVDC_Adc_8, Y, Pr, Pb),
#endif
	BTST_P_MAKE_INPUT(YPrPb_1, BVDC_Adc_0,   BVDC_Adc_1, BVDC_Adc_2, Y, Pr, Pb),
	BTST_P_MAKE_INPUT(YPrPb_2, BVDC_Adc_3,   BVDC_Adc_4, BVDC_Adc_5, Y, Pr, Pb),
	BTST_P_MAKE_INPUT(Rgb_0,   BVDC_Adc_6,   BVDC_Adc_7, BVDC_Adc_8, Green, Red, Blue),
	BTST_P_MAKE_INPUT(Rgb_1,   BVDC_Adc_0,   BVDC_Adc_1, BVDC_Adc_2, Green, Red, Blue),
	BTST_P_MAKE_INPUT(Rgb_2,   BVDC_Adc_3,   BVDC_Adc_4, BVDC_Adc_5, Green, Red, Blue),
	BTST_P_MAKE_INPUT(PcIn_0,  BVDC_Adc_19,  BVDC_Adc_20, BVDC_Adc_21, PcRed, PcGreen, PcBlue),
#elif (BCHP_CHIP==3556)
	BTST_P_MAKE_INPUT(Ifd_0,   BVDC_Ibafe_0, 0, 0, 0, IfDemod,   Unused, Unused, Unused),
	BTST_P_MAKE_INPUT(Ifd_1,   BVDC_Ibafe_0, 0, 0, 0, IfDemod,   Unused, Unused, Unused),
	BTST_P_MAKE_INPUT(Cvbs_0,  BVDC_Adc_15,  0, 0, 0, Composite, Unused, Unused, Unused),
	BTST_P_MAKE_INPUT(Cvbs_1,  BVDC_Adc_17,  0, 0, 0, Composite, Unused, Unused, Unused),
	BTST_P_MAKE_INPUT(Svid_0,  BVDC_Adc_9,   BVDC_Adc_10, 0, 0, Luma, Chroma, Unused, Unused),
	BTST_P_MAKE_INPUT(Svid_1,  BVDC_Adc_13,  BVDC_Adc_14, 0, 0, Luma, Chroma, Unused, Unused),
	BTST_P_MAKE_INPUT(YPrPb_0, BVDC_Adc_0,   BVDC_Adc_1, BVDC_Adc_2, 0, Y, Pr, Pb, Unused),
	BTST_P_MAKE_INPUT(YPrPb_1, BVDC_Adc_6,   BVDC_Adc_7, BVDC_Adc_8, 0, Y, Pr, Pb, Unused),
	BTST_P_MAKE_INPUT(Rgb_0,   BVDC_Adc_0,   BVDC_Adc_1, BVDC_Adc_2, 0, Green, Red, Blue, Unused),
	BTST_P_MAKE_INPUT(Rgb_1,   BVDC_Adc_6,   BVDC_Adc_7, BVDC_Adc_8, 0, Green, Red, Blue, Unused),
	BTST_P_MAKE_INPUT(PcIn_0,  BVDC_Adc_19,  BVDC_Adc_20, BVDC_Adc_21, 0, PcRed, PcGreen, PcBlue, Unused),
	/*scart input*/
	BTST_P_MAKE_INPUT(Scart, BVDC_Adc_18,  BVDC_Adc_3, BVDC_Adc_4, BVDC_Adc_5, ScartComposite, ScartRed, ScartGreen, ScartBlue),
	BTST_P_MAKE_INPUT(Cvbs_2, BVDC_Adc_16,  0, 0, 0, ScartComposite,    Unused, Unused, Unused),
	BTST_P_MAKE_INPUT(Cvbs_3,  BVDC_Adc_18,  0, 0, 0, ScartComposite,    Unused, Unused, Unused),
	BTST_P_MAKE_INPUT(Svid_2,  BVDC_Adc_16,  BVDC_Adc_12, 0, 0, ScartLuma, ScartChroma, Unused, Unused),
	BTST_P_MAKE_INPUT(YPrPb_2, BVDC_Adc_3,   BVDC_Adc_4, BVDC_Adc_5, 0, Pr, Y, Pb, Unused),
	BTST_P_MAKE_INPUT(Rgb_2,   BVDC_Adc_3,   BVDC_Adc_4, BVDC_Adc_5, 0, ScartRed, ScartGreen, ScartBlue, Unused),
#else
	BTST_P_MAKE_INPUT(Ifd_0,   BVDC_Ibafe_0, 0, 0, IfDemod,   Unused, Unused),
	BTST_P_MAKE_INPUT(Ifd_1,   BVDC_Adc_16,  0, 0, IfDemod,   Unused, Unused),
	BTST_P_MAKE_INPUT(Cvbs_0,  BVDC_Adc_12,  0, 0, Composite, Unused, Unused),
	BTST_P_MAKE_INPUT(Cvbs_1,  BVDC_Adc_13,  0, 0, Composite, Unused, Unused),
	BTST_P_MAKE_INPUT(Cvbs_2,  BVDC_Adc_14,  0, 0, Composite, Unused, Unused),
	BTST_P_MAKE_INPUT(Cvbs_3,  BVDC_Adc_15,  0, 0, Composite, Unused, Unused),
	BTST_P_MAKE_INPUT(Svid_0,  BVDC_Adc_6,   BVDC_Adc_7,  0, Luma, Chroma, Unused),
	BTST_P_MAKE_INPUT(Svid_1,  BVDC_Adc_8,   BVDC_Adc_9,  0, Luma, Chroma, Unused),
	BTST_P_MAKE_INPUT(Svid_2,  BVDC_Adc_10,  BVDC_Adc_11, 0, Luma, Chroma, Unused),
	BTST_P_MAKE_INPUT(YPrPb_0, BVDC_Adc_0,   BVDC_Adc_1, BVDC_Adc_2, Y, Pr, Pb),
	BTST_P_MAKE_INPUT(YPrPb_1, BVDC_Adc_3,   BVDC_Adc_4, BVDC_Adc_5, Y, Pr, Pb),
	BTST_P_MAKE_INPUT(YPrPb_2, BVDC_Adc_3,   BVDC_Adc_4, BVDC_Adc_5, Y, Pr, Pb),
	BTST_P_MAKE_INPUT(Rgb_0,   BVDC_Adc_0,   BVDC_Adc_1, BVDC_Adc_2, Green, Red, Blue),
	BTST_P_MAKE_INPUT(Rgb_1,   BVDC_Adc_3,   BVDC_Adc_4, BVDC_Adc_5, Green, Red, Blue),
	BTST_P_MAKE_INPUT(Rgb_2,   BVDC_Adc_3,   BVDC_Adc_4, BVDC_Adc_5, Green, Red, Blue),
	BTST_P_MAKE_INPUT(PcIn_0,  BVDC_Adc_17,  BVDC_Adc_18, BVDC_Adc_19, PcRed, PcGreen, PcBlue),
#endif
};


/***************************************************************************
 *
 */
void BTST_PrintVdecInputMenu
	( void )
{
	uint32_t i;

	BKNI_Printf("VDEC Input selection!\n");
	for(i = 0; i < sizeof(s_aInput)/sizeof(BTST_P_InputEntry); i++)
	{
		BKNI_Printf("\t(%d) %s\n", i, s_aInput[i].pchName);
	}

	return;
}


/***************************************************************************
 *
 */
void BTST_SelectVdecInput
	( BVDC_Source_Handle      hVidSrc,
	  BTST_VdecInput          eVdecInput )
{
	if(!hVidSrc)
	{
		BDBG_WRN(("Passing uninitialized pointer!"));
		goto done;
	}

	if(s_aInput[eVdecInput].eCh0 == BVDC_AdcInput_eUnused &&
	   s_aInput[eVdecInput].eCh1 == BVDC_AdcInput_eUnused &&
	   s_aInput[eVdecInput].eCh2 == BVDC_AdcInput_eUnused)
	{
		BKNI_Printf("adc %d is unused\n", eVdecInput);
		return;
	}

	/* Set all to unused first */
	BVDC_Source_SetAdcConfiguration(hVidSrc,
		BVDC_Adc_0  | BVDC_Adc_5  | BVDC_Adc_10 | BVDC_Adc_15 |
		BVDC_Adc_1  | BVDC_Adc_6  | BVDC_Adc_11 | BVDC_Adc_16 |
		BVDC_Adc_2  | BVDC_Adc_7  | BVDC_Adc_12 | BVDC_Adc_17 |
		BVDC_Adc_3  | BVDC_Adc_8  | BVDC_Adc_13 | BVDC_Adc_18 |
		BVDC_Adc_4  | BVDC_Adc_9  | BVDC_Adc_14 | BVDC_Adc_19 |
		BVDC_Adc_20 | BVDC_Adc_21 |
		BVDC_Ibafe_0, BVDC_AdcInput_eUnused);

	BDBG_ASSERT(s_aInput[eVdecInput].eInput == eVdecInput);

	/* The 3 channels */
	BVDC_Source_SetAdcConfiguration(hVidSrc,
		s_aInput[eVdecInput].ulCh0, s_aInput[eVdecInput].eCh0);
	BVDC_Source_SetAdcConfiguration(hVidSrc,
		s_aInput[eVdecInput].ulCh1, s_aInput[eVdecInput].eCh1);
	BVDC_Source_SetAdcConfiguration(hVidSrc,
		s_aInput[eVdecInput].ulCh2, s_aInput[eVdecInput].eCh2);
#if (BCHP_CHIP==3556)
	BVDC_Source_SetAdcConfiguration(hVidSrc,
		s_aInput[eVdecInput].ulCh3, s_aInput[eVdecInput].eCh3);
#endif

done:
	return;
}

/* end of file */
