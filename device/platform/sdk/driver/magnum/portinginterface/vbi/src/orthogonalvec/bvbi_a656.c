
/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvbi_a656.c $
 * $brcm_Revision: Hydra_Software_Devel/8 $
 * $brcm_Date: 8/10/12 3:32p $
 *
 * Module Description:
 *
 * This module provides access to the ANCI_656 cores for the VBI porting
 * interface (BVBI).  This module is private to BVBI.
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vbi/7420/bvbi_a656.c $
 * 
 * Hydra_Software_Devel/8   8/10/12 3:32p pntruong
 * SW7435-276: Updated NTSC/480p to 720x480i/720x480p respectively.
 * Added support 482i/483p with new enums.
 *
 * Hydra_Software_Devel/7   2/20/12 2:53p darnstein
 * SW7425-2434: more detail in error messages.
 *
 * Hydra_Software_Devel/6   2/20/12 12:56p darnstein
 * SW7425-2434: when an unsupported video format is entered, the BDBG
 * error message should be informative.
 *
 * Hydra_Software_Devel/5   10/1/10 2:35p darnstein
 * SW7422-46: Adapt to 7422 and 7425 chipsets.
 *
 * Hydra_Software_Devel/4   12/21/09 6:58p darnstein
 * SW7550-120: Add support for SECAM variants.
 *
 * Hydra_Software_Devel/3   12/5/08 11:14a darnstein
 * PR45819: register names were all wrong.
 *
 * Hydra_Software_Devel/2   12/4/08 6:07p darnstein
 * PR45819: 7420 software will now compile, but not link.
 *
 * Hydra_Software_Devel/1   12/3/08 8:56p darnstein
 * PR45819: copied over from 7400 area.
 *
 * Hydra_Software_Devel/2   12/3/08 7:56p darnstein
 * PR45819: New, more modular form of most BVBI source files.
 *
 * Hydra_Software_Devel/12   10/10/07 4:00p darnstein
 * PR35440: handle ancillary VEC and passthrough VEC in the same fashion.
 * Since there is already a #define macro for modular VEC, this seems
 * more graceful than before.
 *
 * Hydra_Software_Devel/11   2/26/07 2:28p darnstein
 * PR25990: Some minor changes were required for compiling on 7400-B0.
 *
 * Hydra_Software_Devel/10   1/2/07 4:20p darnstein
 * PR26872: Mechanically add SECAM to all cases where PAL formats are
 * accepted.
 *
 * Hydra_Software_Devel/9   12/14/06 7:19p darnstein
 * PR25990: Can compile for BCM97400-B0 now.
 *
 * Hydra_Software_Devel/8   7/20/06 2:22p darnstein
 * PR21688: Use the new hardware soft reset scheme for later model chips.
 *
 * Hydra_Software_Devel/7   4/12/06 1:56p darnstein
 * PR18010: Add ANCI656_PRIM_ANCI656_CNTRL setting to the PR18010
 * backwards compatibility option.
 *
 * Hydra_Software_Devel/6   9/23/05 2:47p darnstein
 * PR13750: Proper use of BERR_TRACE and BERR_CODEs.
 *
 * Hydra_Software_Devel/5   8/22/05 8:11p darnstein
 * PR16057: To support many different chips, use private #defines that
 * specify number of VECs, VDECs, and (separately) pass-through VECs.
 *
 * Hydra_Software_Devel/4   8/8/05 3:07p darnstein
 * PR16057: Avoid non-existent cores on BCM97401 chip.
 *
 * Hydra_Software_Devel/3   3/17/05 6:05p darnstein
 * PR 14472: Eliminate references to secondary VEC if chip is 3560.
 *
 * Hydra_Software_Devel/2   3/14/05 4:16p darnstein
 * PR 14382: Code DCount value according to D. Perrier, for SMPTE 291M
 * format.
 *
 * Hydra_Software_Devel/1   7/21/04 2:39p darnstein
 * PR9080: Finish merging ITU-R 656 software to main branch.
 *
 * I656/2   7/15/04 5:53p darnstein
 * Allow choice of sdid field in ancillary data packets written out.  Try
 * to get dcount field right too.
 *
 * I656/1   6/25/04 3:24p darnstein
 * Initial version.
 *
 ***************************************************************************/

#include "bstd.h"			/* standard types */
#include "bdbg.h"			/* Dbglib */
#include "bvbi.h"			/* VBI processing, this module. */
#include "bvbi_priv.h"		/* VBI internal data structures */

#if (BVBI_P_NUM_ANCI656_656 > 0) /** { **/

#include "bchp_anci656_ancil_0.h"	/* RDB info for ANCI_656_656 registers */
#if (BVBI_P_NUM_ANCI656_656 >= 2)
#include "bchp_anci656_ancil_1.h"
#endif

BDBG_MODULE(BVBI);

/***************************************************************************
* Forward declarations of static (private) functions
***************************************************************************/
static void BVBI_P_A656_Enc_Init (BREG_Handle hReg);
static uint32_t P_GetCoreOffset (uint8_t hwCoreIndex);


/***************************************************************************
* Implementation of "BVBI_" API functions
***************************************************************************/


/***************************************************************************
* Implementation of supporting VBI_ENC functions that are not in API
***************************************************************************/


BERR_Code BVBI_P_A656_Init( BVBI_P_Handle *pVbi )
{
	BDBG_ENTER(BVBI_P_A656_Init);

	/* Initialize ANCI656 cores */
	BVBI_P_A656_Enc_Init (pVbi->hReg);

	BDBG_LEAVE(BVBI_P_A656_Init);
	return BERR_SUCCESS;
}


BERR_Code BVBI_P_A656_Enc_Program (
	BREG_Handle hReg,
	uint8_t hwCoreIndex,
	BVBI_P_Encode_656_Options* h656options,
	bool bPR18010_bad_line_number,
	BFMT_VideoFmt eVideoFormat)
{
/*
	Programming note: the implementation here assumes that the bitfield layout
	within registers is the same for all ANCI656 cores in the chip.

	If a chip is built that has multiple ANCI656 encoder cores that are not
	identical, then this routine will have to be redesigned.
*/
	uint32_t ulCoreOffset;
	uint32_t ulReg;
	uint8_t sdid;
	uint8_t dcountCC;
	uint8_t dcountTT;
	uint8_t dcountWSS;
	bool    isPal;

	BDBG_ENTER(BVBI_P_A656_Enc_Program);

	/* Figure out which encoder core to use */
	ulCoreOffset = P_GetCoreOffset (hwCoreIndex);
	if (ulCoreOffset == 0xFFFFFFFF)
	{
		/* This should never happen!  This parameter was checked by
		   BVBI_Encode_Create() */
		BDBG_LEAVE(BVBI_P_A656_Enc_Program);
		return BERR_TRACE (BERR_INVALID_PARAMETER);
	}

	/* Determine whether PAL or NTSC.  The hardware can only handle straight
	   PAL or NTSC, so some of the video standards are "don't cares." */
	/* Select video format */
	switch (eVideoFormat)
	{
	case BFMT_VideoFmt_eNTSC:
	case BFMT_VideoFmt_eNTSC_J:
	case BFMT_VideoFmt_e720x482_NTSC:
	case BFMT_VideoFmt_e720x482_NTSC_J:
	case BFMT_VideoFmt_e1080i:
	case BFMT_VideoFmt_e720p:
	case BFMT_VideoFmt_e480p:
	case BFMT_VideoFmt_e720x483p:
		isPal = false;
		break;

	case BFMT_VideoFmt_ePAL_B:
	case BFMT_VideoFmt_ePAL_B1:
	case BFMT_VideoFmt_ePAL_D:
	case BFMT_VideoFmt_ePAL_D1:
	case BFMT_VideoFmt_ePAL_G:
	case BFMT_VideoFmt_ePAL_H:
	case BFMT_VideoFmt_ePAL_K:
	case BFMT_VideoFmt_ePAL_I:
	case BFMT_VideoFmt_ePAL_M:
	case BFMT_VideoFmt_ePAL_N:
	case BFMT_VideoFmt_ePAL_NC:
	case BFMT_VideoFmt_eSECAM_L:
	case BFMT_VideoFmt_eSECAM_B:
	case BFMT_VideoFmt_eSECAM_G:
	case BFMT_VideoFmt_eSECAM_D:
	case BFMT_VideoFmt_eSECAM_K:
	case BFMT_VideoFmt_eSECAM_H:
	case BFMT_VideoFmt_e1080i_50Hz:
	case BFMT_VideoFmt_e720p_50Hz:
	case BFMT_VideoFmt_e576p_50Hz:
		isPal = true;
		break;

	default:
		BDBG_LEAVE(BVBI_P_A656_Enc_Program);
		BDBG_ERR(("BVBI_A656: video format %d not supported", eVideoFormat));
		return BERR_TRACE (BERR_NOT_SUPPORTED);
		break;
	}

	/* Figure out the data ID and length fields for ancillary data packets */
	/* TODO: some of this is likely wrong, especially SAA7113. */
	switch (h656options->e656Format)
	{
	case BVBI_656Fmt_SAA7113:
	case BVBI_656Fmt_Modified_SAA7113:
		dcountCC = (isPal ? 1 : 5);
		dcountTT = (isPal ? 8 : 12);
		dcountWSS = 3;
		break;
		break;
	case BVBI_656Fmt_SMPTE291:
		dcountCC = 1;
		dcountTT = (isPal ? 21 : 17);
		dcountWSS = 2;
		break;
	default:
		BDBG_LEAVE(BVBI_P_A656_Enc_Program);
		BDBG_ERR(("BVBI_A656: ITU-R 656 encoding format %d not supported",
			h656options->e656Format));
		return BERR_TRACE (BERR_INVALID_PARAMETER);
		break;
	}
	sdid = h656options->sdid;

	/* Program the "type A" register for closed caption */
	if (h656options->e656Format != BVBI_656Fmt_SMPTE291)
		dcountCC = BVBI_P_p656_SetEEbits (dcountCC);
	ulReg = (
		BCHP_FIELD_DATA(ANCI656_ANCIL_0_TYPE_A_VBI_HEADER,        SDID,     sdid) |
		BCHP_FIELD_DATA(ANCI656_ANCIL_0_TYPE_A_VBI_HEADER, DWORD_COUNT, dcountCC)
	);
	if (isPal)
	{
		ulReg |=
			BCHP_FIELD_ENUM(ANCI656_ANCIL_0_TYPE_A_VBI_HEADER, TYPE,
				EURO_CLOSED_CAPTION);
	}
	else
	{
		ulReg |=
			BCHP_FIELD_ENUM(ANCI656_ANCIL_0_TYPE_A_VBI_HEADER, TYPE,
				US_CLOSED_CAPTION);
	}
    BREG_Write32 (
		hReg, BCHP_ANCI656_ANCIL_0_TYPE_A_VBI_HEADER + ulCoreOffset, ulReg);

	/* Program the "type B" register for teletext */
	if (h656options->e656Format != BVBI_656Fmt_SMPTE291)
		dcountTT = BVBI_P_p656_SetEEbits (dcountTT);
	ulReg = (
		BCHP_FIELD_DATA(ANCI656_ANCIL_0_TYPE_B_VBI_HEADER,        SDID,     sdid) |
		BCHP_FIELD_DATA(ANCI656_ANCIL_0_TYPE_B_VBI_HEADER, DWORD_COUNT, dcountTT)
	);
	if (isPal)
	{
		ulReg |=
			BCHP_FIELD_ENUM(ANCI656_ANCIL_0_TYPE_B_VBI_HEADER, TYPE,
				TELETEXT);
	}
	else
	{
		ulReg |=
			BCHP_FIELD_ENUM(ANCI656_ANCIL_0_TYPE_B_VBI_HEADER, TYPE,
				US_NABTS);
	}
    BREG_Write32 (
		hReg, BCHP_ANCI656_ANCIL_0_TYPE_B_VBI_HEADER + ulCoreOffset, ulReg);

	/* Program the "type C" register for WSS */
	if (h656options->e656Format != BVBI_656Fmt_SMPTE291)
		dcountWSS = BVBI_P_p656_SetEEbits (dcountWSS);
	ulReg = (
		BCHP_FIELD_DATA(ANCI656_ANCIL_0_TYPE_C_VBI_HEADER,        SDID,      sdid)|
		BCHP_FIELD_DATA(ANCI656_ANCIL_0_TYPE_C_VBI_HEADER, DWORD_COUNT, dcountWSS)
	);
	ulReg |=
		BCHP_FIELD_ENUM(ANCI656_ANCIL_0_TYPE_C_VBI_HEADER, TYPE,
			WIDE_SCREEN);
    BREG_Write32 (
		hReg, BCHP_ANCI656_ANCIL_0_TYPE_C_VBI_HEADER + ulCoreOffset, ulReg);

	/* Program the ITU-R 656 data encoding format */
	ulReg =
		BREG_Read32 ( hReg,  BCHP_ANCI656_ANCIL_0_ANCI656_CNTRL + ulCoreOffset );
	ulReg &=
		 ~BCHP_MASK      (ANCI656_ANCIL_0_ANCI656_CNTRL, FORMAT         );
	switch (h656options->e656Format)
	{
	case BVBI_656Fmt_SAA7113:
		ulReg |=
			BCHP_FIELD_ENUM(ANCI656_ANCIL_0_ANCI656_CNTRL, FORMAT, SAA7113H);
		break;
	case BVBI_656Fmt_Modified_SAA7113:
		ulReg |=
			BCHP_FIELD_ENUM(ANCI656_ANCIL_0_ANCI656_CNTRL, FORMAT,
				MODIFIED_SAA7113H);
		break;
	case BVBI_656Fmt_SMPTE291:
		ulReg |=
			BCHP_FIELD_ENUM(ANCI656_ANCIL_0_ANCI656_CNTRL, FORMAT, SMPTE291M);
		break;
	default:
		BDBG_LEAVE(BVBI_P_A656_Enc_Program);
		BDBG_ERR(("BVBI_A656: ITU-R 656 encoding format %d not supported",
			h656options->e656Format));
		return BERR_TRACE (BERR_INVALID_PARAMETER);
		break;
	}

	/* Take care of the PR18010 backwards compatibility issue */
	if (bPR18010_bad_line_number)
	{
		ulReg |= (
			BCHP_FIELD_ENUM(ANCI656_ANCIL_0_ANCI656_CNTRL, V_BIT_SEL, REG) |
			BCHP_FIELD_ENUM(ANCI656_ANCIL_0_ANCI656_CNTRL, F_BIT_SEL, REG)
		);
	}

    BREG_Write32 (hReg, BCHP_ANCI656_ANCIL_0_ANCI656_CNTRL + ulCoreOffset, ulReg);

	BDBG_LEAVE(BVBI_P_A656_Enc_Program);
	return BERR_SUCCESS;
}

/***************************************************************************
* Static (private) functions
***************************************************************************/

/***************************************************************************
 *
 */
static void BVBI_P_A656_Enc_Init (BREG_Handle hReg)
{
	BDBG_ENTER(BVBI_P_A656_Enc_Init);

	/* Just reset the core(s).  Probably not necessary. */
	BVBI_P_VIE_AncilSoftReset (hReg, 0);
#if (BVBI_P_NUM_ANCI656_656 >= 2)
	BVBI_P_VIE_AncilSoftReset (hReg, 1);
#endif

	BDBG_LEAVE(BVBI_P_A656_Enc_Init);
}

/***************************************************************************
 *
 */
static uint32_t P_GetCoreOffset (uint8_t hwCoreIndex)
{
	uint32_t ulCoreOffset = 0xFFFFFFFF;

	if (hwCoreIndex == 0)
		ulCoreOffset = 0x0;
#if (BVBI_P_NUM_ANCI656_656 >= 2)
	if (hwCoreIndex == 0)
		ulCoreOffset =
			BCHP_ANCI656_ANCIL_1_ANCI656_REV_ID -
			BCHP_ANCI656_ANCIL_0_ANCI656_REV_ID;
#endif

	return ulCoreOffset;
}

#endif /** } (BVBI_P_NUM_ANCI656_ANCIL_0 > 0) **/

/* End of file */
