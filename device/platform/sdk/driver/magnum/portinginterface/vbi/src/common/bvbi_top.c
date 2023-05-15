/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvbi_top.c $
 * $brcm_Revision: Hydra_Software_Devel/5 $
 * $brcm_Date: 8/10/12 3:29p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vbi/7400/bvbi_top.c $
 * 
 * Hydra_Software_Devel/5   8/10/12 3:29p pntruong
 * SW7435-276: Updated NTSC/480p to 720x480i/720x480p respectively.
 * Added support 482i/483p with new enums.
 *
 * Hydra_Software_Devel/4   2/20/12 12:55p darnstein
 * SW7425-2434: when an unsupported video format is entered, the BDBG
 * error message should be informative.
 *
 * Hydra_Software_Devel/3   12/21/09 7:05p darnstein
 * SW7550-120: Add support for SECAM variants.
 *
 * Hydra_Software_Devel/2   12/3/08 7:57p darnstein
 * PR45819: New, more modular form of most BVBI source files.
 *
 * Hydra_Software_Devel/23   6/26/08 7:51p darnstein
 * PR40710: update for 50 Hz HDTV video formats.
 *
 * Hydra_Software_Devel/22   4/2/08 7:55p darnstein
 * PR38956: VBI software compiles now.
 *
 * Hydra_Software_Devel/21   1/2/08 2:28p darnstein
 * PR32758: PAL VBI decoding starts very late in each video field. This is
 * to give the TTD core lots of time to do its job. I might be able to
 * reduce the delay later.
 *
 * Hydra_Software_Devel/20   4/9/07 8:26p darnstein
 * PR23254: Fix programming of top level decoder core for second VDEC.
 *
 * Hydra_Software_Devel/19   1/2/07 4:20p darnstein
 * PR26872: Mechanically add SECAM to all cases where PAL formats are
 * accepted.
 *
 * Hydra_Software_Devel/18   11/20/06 5:53p darnstein
 * PR20428: Fix silly typos in previous checkin.
 *
 * Hydra_Software_Devel/17   11/16/06 4:41p darnstein
 * PR20428: Change the way interrupts for progressive video input is
 * handled. This might have to be revisited.
 *
 * Hydra_Software_Devel/16   11/2/06 5:21p darnstein
 * PR23178: Merge back to main branch.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/1   11/2/06 3:19p darnstein
 * PR23178: adapt to recent changes in the 3563 branch of BVDC.
 *
 * Hydra_Software_Devel/15   8/18/06 6:51p darnstein
 * PR23178: basic compile on 93563 is possible.
 *
 * Hydra_Software_Devel/14   3/9/06 2:02p darnstein
 * PR 20088: streamline the way that existence of EXT_656_TOP chip core is
 * handled.
 *
 * Hydra_Software_Devel/13   9/23/05 2:47p darnstein
 * PR13750: Proper use of BERR_TRACE and BERR_CODEs.
 *
 * Hydra_Software_Devel/12   9/19/05 2:57p darnstein
 * PR17151: Check for chip name where needed. Also, convert to new scheme
 * for testing chip revisions (BCHP_VER).
 *
 * Hydra_Software_Devel/11   4/13/05 8:38p darnstein
 * PR 14720: Modify #defines to account for 7038-B2 and 7038-C0 chip
 * revisions.  Compiles OK now.  But untested!
 *
 * Hydra_Software_Devel/10   3/17/05 6:39p darnstein
 * PR 14426: use new _0 names for VDEC cores.
 *
 * Hydra_Software_Devel/9   3/17/05 6:27p darnstein
 * PR 14426: use new _0 names for VDEC cores.
 *
 * Hydra_Software_Devel/8   3/9/05 3:44p darnstein
 * PR 11440: fix errors involving ITU-R 656 input and output.
 *
 * Hydra_Software_Devel/7   1/5/05 4:27p jasonh
 * PR 13700: Fixed VBI compile issues for 7038 C0.
 *
 * Hydra_Software_Devel/6   7/16/04 7:07p darnstein
 * PR 9080: merge in 656 input and output work. Some testing and debugging
 * remains to be done.
 *
 * Hydra_Software_Devel/I656/1   7/8/04 7:45p darnstein
 * ITU-R 656 decoding of VBI seems to be ready for bring up.  Expect bugs.
 *
 * Hydra_Software_Devel/5   5/24/04 5:08p jasonh
 * PR 11189: Merge down from B0 to main-line
 *
 * Hydra_Software_Devel/Refsw_Devel_7038_B0/1   4/19/04 2:45p darnstein
 * PR 9080: Compilation is possible with 7038 B0. Correct execution is not
 * likely.
 *
 * Hydra_Software_Devel/4   4/2/04 6:42p darnstein
 * PR 9080: Allow NTSC-J video format.
 *
 * Hydra_Software_Devel/3   3/26/04 1:50p darnstein
 * PR 9080: Adjust which line interrupt fires on.  I don't think this is
 * the optimal setting.
 *
 * Hydra_Software_Devel/2   3/18/04 10:41a darnstein
 * PR 9080: first working version.
 *
 * Hydra_Software_Devel/1   12/19/03 5:04p darnstein
 * PR 9080: initial version.
 *
 ***************************************************************************/
#include "bstd.h"			/* standard types */
#include "bdbg.h"			/* Dbglib */
#include "bvbi.h"			/* VBI processing, this module. */
#include "bkni.h"			/* For critical sections */
#include "bvbi_priv.h"		/* VBI internal data structures */

#if (BVBI_P_NUM_VDEC >= 1) /** { **/

#include "bchp_vd_top_0.h"	/* RDB info for VD_TOP_0 registers */
#ifdef BVBI_P_HAS_FE_BE
	#include "bchp_vdec_be_0.h"
	#if (BVBI_P_NUM_VDEC >= 2)
		#include "bchp_vdec_be_1.h"
	#endif
#endif
#ifdef BVBI_P_HAS_EXT_656
	#include "bchp_ext_656_top_0.h"
#endif

BDBG_MODULE(BVBI);


/***************************************************************************
* Forward declarations of static (private) functions
***************************************************************************/


/***************************************************************************
* Implementation of "BVBI_" API functions
***************************************************************************/


/***************************************************************************
* Implementation of supporting VD_TOP functions that are not in API
***************************************************************************/


BERR_Code BVBI_P_VDTOP_Dec_Program (
	BREG_Handle hReg,
	BAVC_SourceId eSource,
	BFMT_VideoFmt eVideoFormat)
{
/*
	Programming note: the implementation here assumes that the bitfield layout
	within registers is the same for all VD_TOP cores in the chip.

	If a chip is built that has multiple VD_TOP decoder cores that are not
	identical, then this routine will have to be redesigned.
*/
	uint32_t ulOffset;
	uint32_t ulReg;
	uint32_t topLine;
	uint32_t botLine;

	BDBG_ENTER(BVBI_P_VDTOP_Dec_Program);

	/* Figure out which decoder core to use */
	switch (eSource)
	{
	case BAVC_SourceId_eVdec0:
		ulOffset = 0;
		break;
#if (BVBI_P_NUM_VDEC > 1)
	case BAVC_SourceId_eVdec1:
#ifdef BVBI_P_HAS_FE_BE
		ulOffset = BCHP_VDEC_BE_1_BE_REV_ID - BCHP_VDEC_BE_0_BE_REV_ID;
#else
		ulOffset = BCHP_VD_TOP_1_VD_REV_ID - BCHP_VD_TOP_0_VD_REV_ID;
#endif
		break;
#endif
	default:
		/* This should never happen!  This parameter was checked by
		   BVBI_Decode_Create() */
		BDBG_LEAVE(BVBI_P_VDTOP_Dec_Program);
		return BERR_TRACE(BERR_INVALID_PARAMETER);
		break;
	}

	/* Prepare to program the interrupt control register */
#ifdef BVBI_P_HAS_FE_BE
	ulReg =
		BREG_Read32 ( hReg, BCHP_VDEC_BE_0_VDEC_INTERRUPT + ulOffset );
#else
	ulReg =
		BREG_Read32 ( hReg, BCHP_VD_TOP_0_vd_vdec_interrupt + ulOffset );
#endif

	/* Select video format */
	switch (eVideoFormat)
	{
	case BFMT_VideoFmt_eNTSC:
	case BFMT_VideoFmt_eNTSC_J:
	case BFMT_VideoFmt_e720x482_NTSC:
	case BFMT_VideoFmt_e720x482_NTSC_J:
		topLine = 30;
		botLine = 30;
		break;

	case BFMT_VideoFmt_e1080i:
	case BFMT_VideoFmt_e1080i_50Hz:
		topLine = 23;
		botLine = 23;
		break;

	case BFMT_VideoFmt_e720p:
	case BFMT_VideoFmt_e720p_50Hz:
		topLine = 30;
		botLine = 30;
		break;

	case BFMT_VideoFmt_e480p:
	case BFMT_VideoFmt_e720x483p:
		topLine = 45;
		botLine = 45;
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
		topLine = 100;
		botLine = 100;
		break;

	default:
		BDBG_LEAVE(BVBI_P_VDTOP_Dec_Program);
		BDBG_ERR(("video format %d not supported", eVideoFormat));
		return BERR_TRACE (BERR_NOT_SUPPORTED);
		break;
	}

	/* Finish programming the interrupt control register */
#ifdef BVBI_P_HAS_FE_BE /** { **/
	ulReg &= ~(
		 BCHP_MASK      (VDEC_BE_0_VDEC_INTERRUPT, FIELD_1         ) |
		 BCHP_MASK      (VDEC_BE_0_VDEC_INTERRUPT, FIELD_0         )
	);
	ulReg |= (
		 BCHP_FIELD_DATA(VDEC_BE_0_VDEC_INTERRUPT, FIELD_1, botLine) |
		 BCHP_FIELD_DATA(VDEC_BE_0_VDEC_INTERRUPT, FIELD_0, topLine)
	);
	BREG_Write32 (hReg, BCHP_VDEC_BE_0_VDEC_INTERRUPT + ulOffset, ulReg);
#else /** } BVBI_P_HAS_FE_BE { **/
	ulReg &= ~(
		 BCHP_MASK      (VD_TOP_0_vd_vdec_interrupt, field_1         ) |
		 BCHP_MASK      (VD_TOP_0_vd_vdec_interrupt, field_0         )
	);
	ulReg |= (
		 BCHP_FIELD_DATA(VD_TOP_0_vd_vdec_interrupt, field_0, topLine) |
		 BCHP_FIELD_DATA(VD_TOP_0_vd_vdec_interrupt, field_1, botLine)
	);
	BREG_Write32 (hReg, BCHP_VD_TOP_0_vd_vdec_interrupt + ulOffset, ulReg);
#endif /** } BVBI_P_HAS_FE_BE **/

	BDBG_LEAVE(BVBI_P_VDTOP_Dec_Program);
	return BERR_SUCCESS;
}

BERR_Code BVBI_P_VDTOP_Dec_Reset (
	BREG_Handle hReg,
	BAVC_SourceId eSource,
	uint32_t whichStandard)
{
/*
	Programming note: the implementation here assumes that the bitfield layout
	within registers is the same for all VD_TOP cores in the chip.

	If a chip is built that has multiple VD_TOP decoder cores that are not
	identical, then this routine will have to be redesigned.
*/
	uint32_t ulOffset;
	uint32_t ulReg = 0x0;

	BDBG_ENTER(BVBI_P_VDTOP_Dec_Reset);

	/* Figure out which decoder core to use */
	switch (eSource)
	{
	case BAVC_SourceId_eVdec0:
		ulOffset = 0;
		break;
#if (BVBI_P_NUM_VDEC > 1)
	case BAVC_SourceId_eVdec1:
		ulOffset = BCHP_VD_TOP_0_VBI1_RESET - BCHP_VD_TOP_0_VBI0_RESET;
		break;
#endif
	default:
		/* This should never happen!  This parameter was checked by
		   BVBI_Decode_Create() */
		BDBG_LEAVE(BVBI_P_VDTOP_Dec_Reset);
		return BERR_TRACE(BERR_INVALID_PARAMETER);
		break;
	}

	/* Prepare to reset cores of user's choosing */
	if ((whichStandard & BVBI_P_SELECT_CC) != 0x0)
		ulReg |= BCHP_VD_TOP_0_VBI0_RESET_CCD_MASK;
	if ((whichStandard & BVBI_P_SELECT_CGMSA) != 0x0)
		ulReg |= BCHP_VD_TOP_0_VBI0_RESET_CGMSA_MASK;
	if ((whichStandard & BVBI_P_SELECT_CGMSB) != 0x0)
		ulReg |= BCHP_VD_TOP_0_VBI0_RESET_CGMSA_MASK;
	if ((whichStandard & BVBI_P_SELECT_TT) != 0x0)
		ulReg |= BCHP_VD_TOP_0_VBI0_RESET_TTX_MASK;
	if ((whichStandard & BVBI_P_SELECT_WSS) != 0x0)
		ulReg |= BCHP_VD_TOP_0_VBI0_RESET_WSS_MASK;
	if ((whichStandard & BVBI_P_SELECT_GS) != 0x0)
		ulReg |= BCHP_VD_TOP_0_VBI0_RESET_GSD_MASK;

	/* Write the register twice to effect reset */
    BREG_Write32 (hReg, BCHP_VD_TOP_0_VBI0_RESET + ulOffset, ulReg);
    BREG_Write32 (hReg, BCHP_VD_TOP_0_VBI0_RESET + ulOffset, 0x00000000);

	BDBG_LEAVE(BVBI_P_VDTOP_Dec_Reset);
	return BERR_SUCCESS;
}


BERR_Code BVBI_P_VDTOP_656_Dec_Program (
	BREG_Handle hReg,
	BAVC_SourceId eSource,
	BFMT_VideoFmt eVideoFormat)
{
/*
	Programming note: the implementation here assumes that the bitfield layout
	within registers is the same for all VD_TOP cores in the chip.

	If a chip is built that has multiple VBI_VD_TOP decoder cores that are not
	identical, then this routine will have to be redesigned.
*/
	uint32_t ulOffset;
	uint32_t ulReg;
	uint32_t topLine;
	uint32_t botLine;

	BDBG_ENTER(BVBI_P_VDTOP_656_Dec_Program);

	/* Figure out which decoder core to use */
	switch (eSource)
	{
	case BAVC_SourceId_e656In0:
		ulOffset = 0;
		break;
#if (BVBI_P_NUM_VDEC > 1)
	case BAVC_SourceId_eVdec1:
#ifdef BVBI_P_HAS_FE_BE
		ulOffset = BCHP_VDEC_BE_1_BE_REV_ID - BCHP_VDEC_BE_0_BE_REV_ID;
#else
		ulOffset = BCHP_VD_TOP_1_VD_REV_ID - BCHP_VD_TOP_0_VD_REV_ID;
#endif
		break;
#endif
	default:
		/* This should never happen!  This parameter was checked by
		   BVBI_Decode_Create() */
		BDBG_LEAVE(BVBI_P_VDTOP_656_Dec_Program);
		return BERR_TRACE(BERR_INVALID_PARAMETER);
		break;
	}

	/* Prepare to program the interrupt control register */
	ulReg =
#ifdef BVBI_P_HAS_EXT_656
		BREG_Read32 ( hReg, BCHP_EXT_656_TOP_0_ext_656_interrupt + ulOffset );
#else
		BREG_Read32 ( hReg, BCHP_VD_TOP_0_vd_656_interrupt + ulOffset );
#endif

	/* Select video format */
	switch (eVideoFormat)
	{
	case BFMT_VideoFmt_eNTSC:
	case BFMT_VideoFmt_eNTSC_J:
	case BFMT_VideoFmt_e720x482_NTSC:
	case BFMT_VideoFmt_e720x482_NTSC_J:
		/* NTSC specific settings */
		topLine = 34;
		botLine = 34;
		break;

	case BFMT_VideoFmt_e1080i:
	case BFMT_VideoFmt_e1080i_50Hz:
		topLine = 27;
		botLine = 27;
		break;

	case BFMT_VideoFmt_e720p:
	case BFMT_VideoFmt_e720p_50Hz:
		topLine = 0;
		botLine = 34;
		break;

	case BFMT_VideoFmt_e480p:
	case BFMT_VideoFmt_e720x483p:
		topLine = 0;
		botLine = 49;
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
		topLine = 100;
		botLine = 100;
		break;

	default:
		BDBG_LEAVE(BVBI_P_VDTOP_656_Dec_Program);
		BDBG_ERR(("video format %d not supported", eVideoFormat));
		return BERR_TRACE (BERR_NOT_SUPPORTED);
		break;
	}

	/* Finish programming the interrupt control register */
#ifdef BVBI_P_HAS_EXT_656
	ulReg &= ~(
		 BCHP_MASK      (EXT_656_TOP_0_ext_656_interrupt, field_1         ) |
		 BCHP_MASK      (EXT_656_TOP_0_ext_656_interrupt, field_0         )
	);
	ulReg |= (
		 BCHP_FIELD_DATA(EXT_656_TOP_0_ext_656_interrupt, field_1, botLine) |
		 BCHP_FIELD_DATA(EXT_656_TOP_0_ext_656_interrupt, field_0, topLine)
	);
    BREG_Write32 (hReg, BCHP_EXT_656_TOP_0_ext_656_interrupt + ulOffset, ulReg);
#else
	ulReg &= ~(
		 BCHP_MASK      (VD_TOP_0_vd_656_interrupt, field_1         ) |
		 BCHP_MASK      (VD_TOP_0_vd_656_interrupt, field_0         )
	);
	ulReg |= (
		 BCHP_FIELD_DATA(VD_TOP_0_vd_656_interrupt, field_1, botLine) |
		 BCHP_FIELD_DATA(VD_TOP_0_vd_656_interrupt, field_0, topLine)
	);
    BREG_Write32 (hReg, BCHP_VD_TOP_0_vd_656_interrupt + ulOffset, ulReg);
#endif


	BDBG_LEAVE(BVBI_P_VDTOP_656_Dec_Program);
	return BERR_SUCCESS;
}

/***************************************************************************
* Static (private) functions
***************************************************************************/

#endif /** } (BVBI_P_NUM_VDEC >= 1) **/

/* End of file */
