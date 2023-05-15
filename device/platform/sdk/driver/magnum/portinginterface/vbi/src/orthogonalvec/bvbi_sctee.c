/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvbi_sctee.c $
 * $brcm_Revision: Hydra_Software_Devel/13 $
 * $brcm_Date: 8/10/12 3:31p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vbi/7420/bvbi_sctee.c $
 * 
 * Hydra_Software_Devel/13   8/10/12 3:31p pntruong
 * SW7435-276: Updated NTSC/480p to 720x480i/720x480p respectively.
 * Added support 482i/483p with new enums.
 *
 * Hydra_Software_Devel/12   4/24/12 12:03p darnstein
 * SW7405-5643: BVBI API change. BVBI_Encode_Create() now includes a
 * settings argument. It is a data structure that includes an option for
 * ARIB style video output.
 *
 * Hydra_Software_Devel/11   2/20/12 2:53p darnstein
 * SW7425-2434: more detail in error messages.
 *
 * Hydra_Software_Devel/10   2/20/12 12:56p darnstein
 * SW7425-2434: when an unsupported video format is entered, the BDBG
 * error message should be informative.
 *
 * Hydra_Software_Devel/9   7/14/11 1:24p darnstein
 * SW7420-1994: add an alias for BCHP_SCTE_0_LCR_DATAi_BANK0_CC_DATA_MASK.
 *
 * Hydra_Software_Devel/8   10/5/10 5:37p darnstein
 * SW7405-4916: avoid ASSERT in initialization. Use error code instead.
 *
 * Hydra_Software_Devel/7   3/25/10 11:54a darnstein
 * SW7125-130: add register name work-around to 7340-B0 and 7342-B0.
 *
 * Hydra_Software_Devel/6   2/9/10 3:33p darnstein
 * SW7125-130: extend register name work-around to 7420-B0.
 *
 * Hydra_Software_Devel/5   1/18/10 4:35p darnstein
 * SW7125-130: add register name work-around to 7420-C0.
 *
 * Hydra_Software_Devel/4   12/1/09 7:34p darnstein
 * SW7125-130: Temporarily, change register names for SCTE encoder core.
 * Only applies to 7125. This will be reversed when the RDB gets
 * straightened out.
 *
 * Hydra_Software_Devel/3   6/24/09 4:58p darnstein
 * PR56290: BVBI now compiles for 7342 chipset.
 *
 * Hydra_Software_Devel/2   12/4/08 6:07p darnstein
 * PR45819: 7420 software will now compile, but not link.
 *
 * Hydra_Software_Devel/1   12/3/08 8:02p darnstein
 * PR45819:
 *
 ***************************************************************************/

#include "bstd.h"           /* standard types */
#include "bdbg.h"           /* Dbglib */
#include "bkni.h"			/* For critical sections */
#include "bvbi.h"           /* VBI processing, this module. */
#include "bvbi_priv.h"      /* VBI internal data structures */
#if (BVBI_P_NUM_SCTEE >= 1)
#include "bchp_scte_0.h"  /* RDB info for primary SCTE encoder core */
#endif
#if (BVBI_P_NUM_SCTEE >= 2)
#include "bchp_scte_1.h"   /* RDB info for secondary SCTE encoder core */
#endif
#if (BVBI_P_NUM_SCTEE >= 3)
#include "bchp_scte_2.h"  /* RDB info for tertiary SCTE encoder core */
#endif
/* Currently, there is no 656/SCTE encoder core. */

/*
 * This is a temporary measure, only until the RDBs get straightened out.
 */
#if ( BCHP_CHIP == 7125)                               || \
    ( BCHP_CHIP == 7420)                               || \
    ((BCHP_CHIP == 7340) && (BCHP_VER >= BCHP_VER_B0)) || \
	((BCHP_CHIP == 7342) && (BCHP_VER >= BCHP_VER_B0))
	#define BCHP_SCTE_0_LCR_CONTROLi_BANK1_ARRAY_BASE \
		    BCHP_SCTE_0_LCR_CONTROL_BANK1i_ARRAY_BASE
	#define BCHP_SCTE_0_LCR_CONTROLi_BANK0_ARRAY_BASE \
			BCHP_SCTE_0_LCR_CONTROL_BANK0i_ARRAY_BASE
	#define BCHP_SCTE_0_LCR_CONTROLi_BANK0_LINE_OFFSET_MASK \
			BCHP_SCTE_0_LCR_CONTROL_BANK0i_LINE_OFFSET_MASK
	#define BCHP_SCTE_0_LCR_CONTROLi_BANK0_VBI_DATA_TYPE_MASK \
			BCHP_SCTE_0_LCR_CONTROL_BANK0i_VBI_DATA_TYPE_MASK
	#define BCHP_SCTE_0_LCR_CONTROLi_BANK0_LINE_OFFSET_SHIFT \
			BCHP_SCTE_0_LCR_CONTROL_BANK0i_LINE_OFFSET_SHIFT
	#define BCHP_SCTE_0_LCR_CONTROLi_BANK0_VBI_DATA_TYPE_PAM_DATA \
			BCHP_SCTE_0_LCR_CONTROL_BANK0i_VBI_DATA_TYPE_PAM_DATA
	#define BCHP_SCTE_0_LCR_CONTROLi_BANK0_VBI_DATA_TYPE_SHIFT \
			BCHP_SCTE_0_LCR_CONTROL_BANK0i_VBI_DATA_TYPE_SHIFT
	#define BCHP_SCTE_0_LCR_CONTROLi_BANK0_VBI_DATA_TYPE_NO_DATA \
			BCHP_SCTE_0_LCR_CONTROL_BANK0i_VBI_DATA_TYPE_NO_DATA
	#define BCHP_SCTE_0_LCR_CONTROLi_BANK0_reserved0_MASK \
			BCHP_SCTE_0_LCR_CONTROL_BANK0i_reserved0_MASK
	#define BCHP_SCTE_0_LCR_CONTROLi_BANK0_CC_SHIFT_DIRECTION_MASK \
			BCHP_SCTE_0_LCR_CONTROL_BANK0i_CC_SHIFT_DIRECTION_MASK
	#define BCHP_SCTE_0_LCR_CONTROLi_BANK0_CC_BYTE_ORDER_MASK \
			BCHP_SCTE_0_LCR_CONTROL_BANK0i_CC_BYTE_ORDER_MASK
	#define BCHP_SCTE_0_LCR_CONTROLi_BANK0_CC_PARITY_TYPE_MASK \
			BCHP_SCTE_0_LCR_CONTROL_BANK0i_CC_PARITY_TYPE_MASK
	#define BCHP_SCTE_0_LCR_CONTROLi_BANK0_CC_PARITY_MASK \
			BCHP_SCTE_0_LCR_CONTROL_BANK0i_CC_PARITY_MASK
	#define BCHP_SCTE_0_LCR_CONTROLi_BANK0_CC_RUN_IN_MASK \
			BCHP_SCTE_0_LCR_CONTROL_BANK0i_CC_RUN_IN_MASK
	#define BCHP_SCTE_0_LCR_CONTROLi_BANK0_CC_NULL_MASK \
			BCHP_SCTE_0_LCR_CONTROL_BANK0i_CC_NULL_MASK
	#define BCHP_SCTE_0_LCR_CONTROLi_BANK0_DELAY_COUNT_MASK \
			BCHP_SCTE_0_LCR_CONTROL_BANK0i_DELAY_COUNT_MASK
	#define BCHP_SCTE_0_LCR_CONTROLi_BANK0_reserved0_SHIFT \
			BCHP_SCTE_0_LCR_CONTROL_BANK0i_reserved0_SHIFT
	#define BCHP_SCTE_0_LCR_CONTROLi_BANK0_CC_SHIFT_DIRECTION_LSB_FIRST \
			BCHP_SCTE_0_LCR_CONTROL_BANK0i_CC_SHIFT_DIRECTION_LSB_FIRST
	#define BCHP_SCTE_0_LCR_CONTROLi_BANK0_CC_SHIFT_DIRECTION_SHIFT \
			BCHP_SCTE_0_LCR_CONTROL_BANK0i_CC_SHIFT_DIRECTION_SHIFT
	#define BCHP_SCTE_0_LCR_CONTROLi_BANK0_CC_BYTE_ORDER_LOW_BYTE_FIRST \
			BCHP_SCTE_0_LCR_CONTROL_BANK0i_CC_BYTE_ORDER_LOW_BYTE_FIRST
	#define BCHP_SCTE_0_LCR_CONTROLi_BANK0_CC_BYTE_ORDER_SHIFT \
			BCHP_SCTE_0_LCR_CONTROL_BANK0i_CC_BYTE_ORDER_SHIFT
	#define BCHP_SCTE_0_LCR_CONTROLi_BANK0_CC_PARITY_TYPE_ODD \
			BCHP_SCTE_0_LCR_CONTROL_BANK0i_CC_PARITY_TYPE_ODD
	#define BCHP_SCTE_0_LCR_CONTROLi_BANK0_CC_PARITY_TYPE_SHIFT \
			BCHP_SCTE_0_LCR_CONTROL_BANK0i_CC_PARITY_TYPE_SHIFT
	#define BCHP_SCTE_0_LCR_CONTROLi_BANK0_CC_PARITY_ENABLE \
			BCHP_SCTE_0_LCR_CONTROL_BANK0i_CC_PARITY_ENABLE
	#define BCHP_SCTE_0_LCR_CONTROLi_BANK0_CC_PARITY_SHIFT \
			BCHP_SCTE_0_LCR_CONTROL_BANK0i_CC_PARITY_SHIFT
	#define BCHP_SCTE_0_LCR_CONTROLi_BANK0_CC_RUN_IN_ENABLE \
			BCHP_SCTE_0_LCR_CONTROL_BANK0i_CC_RUN_IN_ENABLE
	#define BCHP_SCTE_0_LCR_CONTROLi_BANK0_CC_RUN_IN_SHIFT \
			BCHP_SCTE_0_LCR_CONTROL_BANK0i_CC_RUN_IN_SHIFT
	#define BCHP_SCTE_0_LCR_CONTROLi_BANK0_CC_NULL_DISABLE \
			BCHP_SCTE_0_LCR_CONTROL_BANK0i_CC_NULL_DISABLE
	#define BCHP_SCTE_0_LCR_CONTROLi_BANK0_CC_NULL_SHIFT \
			BCHP_SCTE_0_LCR_CONTROL_BANK0i_CC_NULL_SHIFT
	#define BCHP_SCTE_0_LCR_CONTROLi_BANK0_DELAY_COUNT_SHIFT \
			BCHP_SCTE_0_LCR_CONTROL_BANK0i_DELAY_COUNT_SHIFT
	#define BCHP_SCTE_0_LCR_CONTROLi_BANK0_VBI_DATA_TYPE_CC_DATA \
			BCHP_SCTE_0_LCR_CONTROL_BANK0i_VBI_DATA_TYPE_CC_DATA
	#define BCHP_SCTE_0_LCR_DATAi_BANK1_ARRAY_BASE \
			BCHP_SCTE_0_LCR_DATA_BANK1i_ARRAY_BASE
	#define BCHP_SCTE_0_LCR_DATAi_BANK0_ARRAY_BASE \
			BCHP_SCTE_0_LCR_DATA_BANK0i_ARRAY_BASE
	#define BCHP_SCTE_0_LCR_DATAi_BANK0_CC_DATA_SHIFT \
			BCHP_SCTE_0_LCR_DATA_BANK0i_CC_DATA_SHIFT
	#define BCHP_SCTE_0_LCR_DATAi_BANK0_CC_DATA_MASK \
			BCHP_SCTE_0_LCR_DATA_BANK0i_CC_DATA_MASK
	#define BCHP_SCTE_0_LCR_CONTROLi_BANK0_VBI_DATA_TYPE_NRTV_DATA \
			BCHP_SCTE_0_LCR_CONTROL_BANK0i_VBI_DATA_TYPE_NRTV_DATA
#endif

BDBG_MODULE(BVBI);

/***************************************************************************
* Private data
***************************************************************************/

/*
 * Programming note: this table must be in sync with the enum
 * BVBI_CSC.
 */
/* These settings work for CC and SCTE modes. Not for monochrome mode. */
static const uint16_t P_csc_coeff_c00[4] = {0x146b, 0x12ec, 0x12d6, 0x1456};
static const uint16_t P_csc_coeff_c03[4] = {0xfd73, 0x0042, 0xfd75, 0xfd77};
static const uint16_t P_csc_coeff_c11[4] = {0xf6aa, 0xf75a, 0x10f8, 0x13e4};
static const uint16_t P_csc_coeff_c12[4] = {0x1441, 0x12c4, 0x0000, 0xfffe};
static const uint16_t P_csc_coeff_c13[4] = {0xf515, 0xf5e2, 0xef08, 0xec20};
static const uint16_t P_csc_coeff_c21[4] = {0x0e2d, 0x0d23, 0x0000, 0x0003};
static const uint16_t P_csc_coeff_c22[4] = {0x0d23, 0x0c2c, 0x17f1, 0x13e3};
static const uint16_t P_csc_coeff_c23[4] = {0xe4b0, 0xe6b1, 0xe80e, 0xec18};

/***************************************************************************
* Forward declarations of static (private) functions
***************************************************************************/

#if (BVBI_P_NUM_SCTEE > 0)  /** { **/
static uint32_t P_GetCoreOffset (uint8_t hwCoreIndex);
static void BVBI_P_ProgramNull (
	BREG_Handle hReg, uint32_t coreOffset,
	uint32_t ulWritePointer, uint32_t value);
static bool BVBI_P_HasComponentOnly (uint8_t hwCoreIndex);
#endif /** } BVBI_P_NUM_SCTEE **/

/***************************************************************************
* Implementation of supporting SCTE functions that are not in API
***************************************************************************/

/***************************************************************************
 *
 */
void BVBI_P_SCTE_Enc_Init (BREG_Handle hReg, uint8_t hwCoreIndex)
{
#if (BVBI_P_NUM_SCTEE > 0)  /** { **/

	BDBG_ENTER(BVBI_P_SCTE_Enc_Init);

	BVBI_P_VIE_SoftReset (hReg, false, hwCoreIndex, BVBI_P_SELECT_SCTE);

	BDBG_LEAVE(BVBI_P_SCTE_Enc_Init);

#else /** } BVBI_P_NUM_SCTEE { **/

	BSTD_UNUSED (hReg);
	BSTD_UNUSED (hwCoreIndex);

#endif /** } BVBI_P_NUM_SCTEE **/
}

/***************************************************************************
 *
 */
BERR_Code BVBI_P_SCTE_Enc_Program (
	BREG_Handle hReg,
	bool is656,
	uint8_t hwCoreIndex,
	bool bActive,
	BFMT_VideoFmt eVideoFormat,
	bool bArib480p,
	BVBI_SCTE_Type scteType,
	BVBI_CSC csc,
	BVBI_CSC coCsc)
{
/*
	Programming note: the implementation here assumes that the bitfield layout
	within registers is the same for all SCTE encoder cores in the chip.

	If a chip is built that has multiple SCTE encoder cores that are not
	identical, then this routine will have to be redesigned.
*/
#if (BVBI_P_NUM_SCTEE > 0)  /** { **/

	uint32_t ulCoreOffset;
	uint32_t ulSctee_controlReg;
	uint32_t ulLineAdj;

	BDBG_ENTER(BVBI_P_SCTE_Enc_Program);

	/* Figure out which encoder core to use */
	ulCoreOffset = P_GetCoreOffset (hwCoreIndex);
	if (ulCoreOffset == 0xFFFFFFFF)
	{
		/* This should never happen!  This parameter was checked by
		   BVBI_Encode_Create() */
		BDBG_LEAVE(BVBI_P_SCTE_Enc_Program);
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* Sanity check */
	if (is656)
	{
		if (bActive)
		{
			/* No bypass encoder for SCTE */
			BDBG_LEAVE(BVBI_P_SCTE_Enc_Program);
			return BERR_TRACE(BERR_INVALID_PARAMETER);
		}
		else
		{
			/* Do nothing */
			return BERR_SUCCESS;
		}
	}

	/* Complain if video format is not supported */
	switch (eVideoFormat)
	{
	case BFMT_VideoFmt_eNTSC:
	case BFMT_VideoFmt_eNTSC_J:
	case BFMT_VideoFmt_e720x482_NTSC:
	case BFMT_VideoFmt_e720x482_NTSC_J:
		break;

	default:
		if (bActive)
		{
			BDBG_ERR(("BVBI_SCTEE: video format %d not supported",
				eVideoFormat));
			return BERR_TRACE (BVBI_ERR_VFMT_CONFLICT);
		}
	}

	/* This makes use of scteType safe */
	BDBG_ASSERT (
		BVBI_SCTE_Type_NONE   == BCHP_SCTE_0_CONFIG_VBI_MODE_DISABLE    );
	BDBG_ASSERT (
		BVBI_SCTE_Type_CCONLY == BCHP_SCTE_0_CONFIG_VBI_MODE_CC         );
	BDBG_ASSERT (
		BVBI_SCTE_Type_CCNRTV == BCHP_SCTE_0_CONFIG_VBI_MODE_SCTE20     );
	BDBG_ASSERT (
		BVBI_SCTE_Type_CCPAM  == BCHP_SCTE_0_CONFIG_VBI_MODE_SCTE21     );
	BDBG_ASSERT (
		BVBI_SCTE_Type_CCMONO == BCHP_SCTE_0_CONFIG_VBI_MODE_MONO_CHROME);
	BDBG_ASSERT (
		BVBI_SCTE_Type_LAST == BCHP_SCTE_0_CONFIG_VBI_MODE_MONO_CHROME+1);

	/* If user wants to turn off closed caption processing, just use the
	   enable bit. */
	if (!bActive)
	{
		ulSctee_controlReg =
			BREG_Read32 ( hReg, BCHP_SCTE_0_CONFIG + ulCoreOffset );
		ulSctee_controlReg &=
			~BCHP_MASK      (SCTE_0_CONFIG, VBI_MODE         );
		ulSctee_controlReg |=
			BCHP_FIELD_ENUM (SCTE_0_CONFIG, VBI_MODE, DISABLE);
		BREG_Write32 (
			hReg, BCHP_SCTE_0_CONFIG + ulCoreOffset, ulSctee_controlReg );
		BDBG_LEAVE(BVBI_P_SCTE_Enc_Program);
		return BERR_SUCCESS;
	}

	/* LCR enable registers: let everything through. */
	BREG_Write32 (hReg, BCHP_SCTE_0_LCR_ENABLE_BANK0 + ulCoreOffset,
		BCHP_FIELD_DATA (SCTE_0_LCR_ENABLE_BANK0, MASK, 0xFFFFFFFF) );
	BREG_Write32 (hReg, BCHP_SCTE_0_LCR_ENABLE_BANK1 + ulCoreOffset,
		BCHP_FIELD_DATA (SCTE_0_LCR_ENABLE_BANK0, MASK, 0xFFFFFFFF) );
	BREG_Write32 (hReg, BCHP_SCTE_0_LCR_ENABLE_BANK2 + ulCoreOffset,
		BCHP_FIELD_DATA (SCTE_0_LCR_ENABLE_BANK0, MASK, 0xFFFFFFFF) );
	BREG_Write32 (hReg, BCHP_SCTE_0_LCR_ENABLE_BANK3 + ulCoreOffset,
		BCHP_FIELD_DATA (SCTE_0_LCR_ENABLE_BANK0, MASK, 0xFFFFFFFF) );

	ulSctee_controlReg = 0;
	ulSctee_controlReg |= (
		BCHP_FIELD_DATA (SCTE_0_CONFIG, INC_VALUE,       16) |
		BCHP_FIELD_DATA (SCTE_0_CONFIG, MOD_VALUE,      429) |
		BCHP_FIELD_DATA (SCTE_0_CONFIG, VBI_MODE , scteType) );
	BREG_Write32 (
		hReg, BCHP_SCTE_0_CONFIG + ulCoreOffset, ulSctee_controlReg);

	ulSctee_controlReg = 0;
	ulSctee_controlReg |= (
		BCHP_FIELD_DATA (SCTE_0_PADDING_BLANK,  Y_VALUE,  16) |
		BCHP_FIELD_DATA (SCTE_0_PADDING_BLANK, Cb_VALUE, 128) |
		BCHP_FIELD_DATA (SCTE_0_PADDING_BLANK, Cr_VALUE, 128) );
	BREG_Write32 (
		hReg, BCHP_SCTE_0_PADDING_BLANK + ulCoreOffset, ulSctee_controlReg);

	ulSctee_controlReg = 0;
	ulSctee_controlReg |= (
		BCHP_FIELD_DATA (SCTE_0_CC_GAIN_OFFSET, OFFSET,    0) |
		BCHP_FIELD_DATA (SCTE_0_CC_GAIN_OFFSET, GAIN  , 2048) );
	BREG_Write32 (
		hReg, BCHP_SCTE_0_CC_GAIN_OFFSET + ulCoreOffset, ulSctee_controlReg);

	ulSctee_controlReg = 0;
	ulSctee_controlReg |= (
		BCHP_FIELD_DATA (SCTE_0_PAM_ADD_GAIN_OFFSET, OFFSET,   0) |
		BCHP_FIELD_DATA (SCTE_0_PAM_ADD_GAIN_OFFSET, GAIN  ,   0) );
	BREG_Write32 (
		hReg, BCHP_SCTE_0_PAM_ADD_GAIN_OFFSET + ulCoreOffset,
		ulSctee_controlReg);

	ulLineAdj = (bArib480p ? 1 : 0);

	ulSctee_controlReg = 0;
	ulSctee_controlReg |= (
		BCHP_FIELD_DATA (SCTE_0_BANK_BASE_BANK0,   BANK_SKIP,   0) |
		BCHP_FIELD_DATA (SCTE_0_BANK_BASE_BANK0, LINE_NUMBER,
			10 - ulLineAdj) );
	BREG_Write32 (
		hReg, BCHP_SCTE_0_BANK_BASE_BANK0 + ulCoreOffset,
		ulSctee_controlReg);

	ulSctee_controlReg = 0;
	ulSctee_controlReg |= (
		BCHP_FIELD_DATA (SCTE_0_BANK_BASE_BANK1,   BANK_SKIP,   0) |
		BCHP_FIELD_DATA (SCTE_0_BANK_BASE_BANK1, LINE_NUMBER,
			263 + 10 - ulLineAdj) );
	BREG_Write32 (
		hReg, BCHP_SCTE_0_BANK_BASE_BANK1 + ulCoreOffset,
		ulSctee_controlReg);

	ulSctee_controlReg = 0;
	ulSctee_controlReg |= (
		BCHP_FIELD_DATA (SCTE_0_BANK_BASE_BANK2,   BANK_SKIP,   0) |
		BCHP_FIELD_DATA (SCTE_0_BANK_BASE_BANK2, LINE_NUMBER,
			10 - ulLineAdj) );
	BREG_Write32 (
		hReg, BCHP_SCTE_0_BANK_BASE_BANK2 + ulCoreOffset,
		ulSctee_controlReg);

	ulSctee_controlReg = 0;
	ulSctee_controlReg |= (
		BCHP_FIELD_DATA (SCTE_0_BANK_BASE_BANK3,   BANK_SKIP,      0) |
		BCHP_FIELD_DATA (SCTE_0_BANK_BASE_BANK3, LINE_NUMBER,
			263 + 10 - ulLineAdj) );
	BREG_Write32 (
		hReg, BCHP_SCTE_0_BANK_BASE_BANK3 + ulCoreOffset,
		ulSctee_controlReg);

	/* Write start registers: start them at the beginning. */
	ulSctee_controlReg = 0;
	ulSctee_controlReg |= (
		BCHP_FIELD_DATA (
			SCTE_0_NRTV_WRITE_PTR_BANK0,   PTR_WR_ENABLE, 1) |
		BCHP_FIELD_DATA (
			SCTE_0_NRTV_WRITE_PTR_BANK0, Cb_Cr_WRITE_PTR, 0) |
		BCHP_FIELD_DATA (
			SCTE_0_NRTV_WRITE_PTR_BANK0,     Y_WRITE_PTR, 0) );
	BREG_Write32 (
		hReg, BCHP_SCTE_0_NRTV_WRITE_PTR_BANK0 + ulCoreOffset,
		ulSctee_controlReg);
	BREG_Write32 (
		hReg, BCHP_SCTE_0_NRTV_WRITE_PTR_BANK1 + ulCoreOffset,
		ulSctee_controlReg);
	BREG_Write32 (
		hReg, BCHP_SCTE_0_NRTV_WRITE_PTR_BANK2 + ulCoreOffset,
		ulSctee_controlReg);
	BREG_Write32 (
		hReg, BCHP_SCTE_0_NRTV_WRITE_PTR_BANK3 + ulCoreOffset,
		ulSctee_controlReg);

	ulSctee_controlReg = 0;
	ulSctee_controlReg |= (
		BCHP_FIELD_DATA (SCTE_0_CSC_COEFF_C03_C00, COEFF_C0,
			P_csc_coeff_c00[csc]) |
		BCHP_FIELD_DATA (SCTE_0_CSC_COEFF_C03_C00, COEFF_C3,
			P_csc_coeff_c03[csc]) );
	BREG_Write32 (
		hReg, BCHP_SCTE_0_CSC_COEFF_C03_C00 + ulCoreOffset,
		ulSctee_controlReg);

	ulSctee_controlReg = 0;
	ulSctee_controlReg |= (
		BCHP_FIELD_DATA (SCTE_0_CSC_COEFF_C12_C11, COEFF_C1,
			P_csc_coeff_c11[csc]) |
		BCHP_FIELD_DATA (SCTE_0_CSC_COEFF_C12_C11, COEFF_C2,
			P_csc_coeff_c12[csc]) );
	BREG_Write32 (
		hReg, BCHP_SCTE_0_CSC_COEFF_C12_C11 + ulCoreOffset,
		ulSctee_controlReg);

	ulSctee_controlReg = 0;
	ulSctee_controlReg |=
		BCHP_FIELD_DATA (SCTE_0_CSC_COEFF_C13, COEFF_C3,
			P_csc_coeff_c13[csc]) ;
	BREG_Write32 (
		hReg, BCHP_SCTE_0_CSC_COEFF_C13 + ulCoreOffset,
		ulSctee_controlReg);

	ulSctee_controlReg = 0;
	ulSctee_controlReg |= (
		BCHP_FIELD_DATA (SCTE_0_CSC_COEFF_C22_C21, COEFF_C1,
			P_csc_coeff_c21[csc]) |
		BCHP_FIELD_DATA (SCTE_0_CSC_COEFF_C22_C21, COEFF_C2,
			P_csc_coeff_c22[csc]) );
	BREG_Write32 (
		hReg, BCHP_SCTE_0_CSC_COEFF_C22_C21 + ulCoreOffset,
		ulSctee_controlReg);

	ulSctee_controlReg = 0;
	ulSctee_controlReg |=
		BCHP_FIELD_DATA (SCTE_0_CSC_COEFF_C23, COEFF_C3,
			P_csc_coeff_c23[csc]) ;
	BREG_Write32 (
		hReg, BCHP_SCTE_0_CSC_COEFF_C23 + ulCoreOffset,
		ulSctee_controlReg);

#ifdef BVBI_P_HAS_SCTEE_CO /** { **/
	if ((BVBI_P_HasComponentOnly (hwCoreIndex)) &&
		(coCsc != BVBI_CSC_NONE         )   )
	{
		ulSctee_controlReg = 0;
		ulSctee_controlReg |= (
			BCHP_FIELD_DATA (SCTE_0_CSC_CO_COEFF_C03_C00, COEFF_C0,
				P_csc_coeff_c00[coCsc]) |
			BCHP_FIELD_DATA (SCTE_0_CSC_CO_COEFF_C03_C00, COEFF_C3,
				P_csc_coeff_c03[coCsc]) );
		BREG_Write32 (
			hReg, BCHP_SCTE_0_CSC_CO_COEFF_C03_C00 + ulCoreOffset,
			ulSctee_controlReg);

		ulSctee_controlReg = 0;
		ulSctee_controlReg |= (
			BCHP_FIELD_DATA (SCTE_0_CSC_CO_COEFF_C12_C11, COEFF_C1,
				P_csc_coeff_c11[coCsc]) |
			BCHP_FIELD_DATA (SCTE_0_CSC_CO_COEFF_C12_C11, COEFF_C2,
				P_csc_coeff_c12[coCsc]) );
		BREG_Write32 (
			hReg, BCHP_SCTE_0_CSC_CO_COEFF_C12_C11 + ulCoreOffset,
			ulSctee_controlReg);

		ulSctee_controlReg = 0;
		ulSctee_controlReg |=
			BCHP_FIELD_DATA (SCTE_0_CSC_CO_COEFF_C13, COEFF_C3,
				P_csc_coeff_c13[coCsc]) ;
		BREG_Write32 (
			hReg, BCHP_SCTE_0_CSC_CO_COEFF_C13 + ulCoreOffset,
			ulSctee_controlReg);

		ulSctee_controlReg = 0;
		ulSctee_controlReg |= (
			BCHP_FIELD_DATA (SCTE_0_CSC_CO_COEFF_C22_C21, COEFF_C1,
				P_csc_coeff_c21[coCsc]) |
			BCHP_FIELD_DATA (SCTE_0_CSC_CO_COEFF_C22_C21, COEFF_C2,
				P_csc_coeff_c22[coCsc]) );
		BREG_Write32 (
			hReg, BCHP_SCTE_0_CSC_CO_COEFF_C22_C21 + ulCoreOffset,
			ulSctee_controlReg);

		ulSctee_controlReg = 0;
		ulSctee_controlReg |=
			BCHP_FIELD_DATA (SCTE_0_CSC_CO_COEFF_C23, COEFF_C3,
				P_csc_coeff_c23[coCsc]) ;
		BREG_Write32 (
			hReg, BCHP_SCTE_0_CSC_CO_COEFF_C23 + ulCoreOffset,
			ulSctee_controlReg);
	}
#endif /** }  BVBI_P_HAS_SCTEE_CO **/

	ulSctee_controlReg = 0;
	ulSctee_controlReg |= (
		BCHP_FIELD_DATA (SCTE_0_SHAPER_BLANK_VALUE,   LUMA_BLANK,
			0x00) |
		BCHP_FIELD_DATA (SCTE_0_SHAPER_BLANK_VALUE, CHROMA_BLANK,
			0x80) );
	BREG_Write32 (
		hReg, BCHP_SCTE_0_SHAPER_BLANK_VALUE + ulCoreOffset,
		ulSctee_controlReg);
	if ((BVBI_P_HasComponentOnly (hwCoreIndex)) &&
		(coCsc != BVBI_CSC_NONE         )   )
	{
		ulSctee_controlReg = 0;
		ulSctee_controlReg |= (
			BCHP_FIELD_DATA (SCTE_0_SHAPER_CO_BLANK_VALUE,   LUMA_BLANK,
				0x00) |
			BCHP_FIELD_DATA (SCTE_0_SHAPER_CO_BLANK_VALUE, CHROMA_BLANK,
				0x80) );
		BREG_Write32 (
			hReg, BCHP_SCTE_0_SHAPER_CO_BLANK_VALUE + ulCoreOffset,
			ulSctee_controlReg);
	}

/*
 * TODO: registers SCTE_0_CSC_MODE, SCTE_0_CSC_MIN_MAX. Default values
 * suffice.
 */

	switch (scteType)
	{
	case BVBI_SCTE_Type_CCONLY:

	ulSctee_controlReg = 0;
		ulSctee_controlReg |= (
			BCHP_FIELD_DATA (SCTE_0_SHAPER_LUMA_COEFF_0_2, COEFF_2, 0) |
			BCHP_FIELD_DATA (SCTE_0_SHAPER_LUMA_COEFF_0_2, COEFF_1, 0) |
			BCHP_FIELD_DATA (SCTE_0_SHAPER_LUMA_COEFF_0_2, COEFF_0, 0) );
		BREG_Write32 (
			hReg, BCHP_SCTE_0_SHAPER_LUMA_COEFF_0_2 + ulCoreOffset,
			ulSctee_controlReg);

		ulSctee_controlReg = 0;
		ulSctee_controlReg |= (
			BCHP_FIELD_DATA (SCTE_0_SHAPER_LUMA_COEFF_3_5, COEFF_5, 0) |
			BCHP_FIELD_DATA (SCTE_0_SHAPER_LUMA_COEFF_3_5, COEFF_4, 0) |
			BCHP_FIELD_DATA (SCTE_0_SHAPER_LUMA_COEFF_3_5, COEFF_3, 0) );
		BREG_Write32 (
			hReg, BCHP_SCTE_0_SHAPER_LUMA_COEFF_3_5 + ulCoreOffset,
			ulSctee_controlReg);

		ulSctee_controlReg = 0;
		ulSctee_controlReg |= (
			BCHP_FIELD_DATA (SCTE_0_SHAPER_LUMA_COEFF_6_8, COEFF_8, 0) |
			BCHP_FIELD_DATA (SCTE_0_SHAPER_LUMA_COEFF_6_8, COEFF_7, 0) |
			BCHP_FIELD_DATA (SCTE_0_SHAPER_LUMA_COEFF_6_8, COEFF_6, 0) );
		BREG_Write32 (
			hReg, BCHP_SCTE_0_SHAPER_LUMA_COEFF_6_8 + ulCoreOffset,
			ulSctee_controlReg);

		ulSctee_controlReg = 0;
		ulSctee_controlReg |= (
			BCHP_FIELD_DATA (SCTE_0_SHAPER_CHROMA_COEFF_0_2, COEFF_2, 0) |
			BCHP_FIELD_DATA (SCTE_0_SHAPER_CHROMA_COEFF_0_2, COEFF_1, 0) |
			BCHP_FIELD_DATA (SCTE_0_SHAPER_CHROMA_COEFF_0_2, COEFF_0, 0) );
		BREG_Write32 (
			hReg, BCHP_SCTE_0_SHAPER_CHROMA_COEFF_0_2 + ulCoreOffset,
			ulSctee_controlReg);

		ulSctee_controlReg = 0;
		ulSctee_controlReg |= (
			BCHP_FIELD_DATA (SCTE_0_SHAPER_CHROMA_COEFF_3_5, COEFF_5, 0) |
			BCHP_FIELD_DATA (SCTE_0_SHAPER_CHROMA_COEFF_3_5, COEFF_4, 0) |
			BCHP_FIELD_DATA (SCTE_0_SHAPER_CHROMA_COEFF_3_5, COEFF_3, 0) );
		BREG_Write32 (
			hReg, BCHP_SCTE_0_SHAPER_CHROMA_COEFF_3_5 + ulCoreOffset,
			ulSctee_controlReg);

		ulSctee_controlReg = 0;
		ulSctee_controlReg |= (
			BCHP_FIELD_DATA (SCTE_0_SHAPER_CHROMA_COEFF_6_8, COEFF_8, 0) |
			BCHP_FIELD_DATA (SCTE_0_SHAPER_CHROMA_COEFF_6_8, COEFF_7, 0) |
			BCHP_FIELD_DATA (SCTE_0_SHAPER_CHROMA_COEFF_6_8, COEFF_6, 0) );
		BREG_Write32 (
			hReg, BCHP_SCTE_0_SHAPER_CHROMA_COEFF_6_8 + ulCoreOffset,
			ulSctee_controlReg);

		ulSctee_controlReg = 0;
		ulSctee_controlReg |= (
			BCHP_FIELD_DATA (SCTE_0_SHAPER_CONTROL,       REPL_COUNT,   0) |
			BCHP_FIELD_DATA (SCTE_0_SHAPER_CONTROL, SHAPE_OUT_OFFSET,   0) |
			BCHP_FIELD_DATA (SCTE_0_SHAPER_CONTROL,  SHAPE_IN_OFFSET,   0) |
			BCHP_FIELD_DATA (SCTE_0_SHAPER_CONTROL,        CC_ENABLE,   1) );
		BREG_Write32 (
			hReg, BCHP_SCTE_0_SHAPER_CONTROL + ulCoreOffset,
			ulSctee_controlReg);

		break;

	/* This is SCTE 20 mode */
	case BVBI_SCTE_Type_CCNRTV:

		ulSctee_controlReg = 0;
		ulSctee_controlReg |= (
			BCHP_FIELD_DATA (SCTE_0_SHAPER_LUMA_COEFF_0_2, COEFF_0, 0x10) |
			BCHP_FIELD_DATA (SCTE_0_SHAPER_LUMA_COEFF_0_2, COEFF_1, 0x30) |
			BCHP_FIELD_DATA (SCTE_0_SHAPER_LUMA_COEFF_0_2, COEFF_2, 0x50) );
		BREG_Write32 (
			hReg, BCHP_SCTE_0_SHAPER_LUMA_COEFF_0_2 + ulCoreOffset,
			ulSctee_controlReg);

		ulSctee_controlReg = 0;
		ulSctee_controlReg |= (
			BCHP_FIELD_DATA (SCTE_0_SHAPER_LUMA_COEFF_3_5, COEFF_3, 0x70) |
			BCHP_FIELD_DATA (SCTE_0_SHAPER_LUMA_COEFF_3_5, COEFF_4, 0x90) |
			BCHP_FIELD_DATA (SCTE_0_SHAPER_LUMA_COEFF_3_5, COEFF_5, 0xc0) );
		BREG_Write32 (
			hReg, BCHP_SCTE_0_SHAPER_LUMA_COEFF_3_5 + ulCoreOffset,
			ulSctee_controlReg);

		ulSctee_controlReg = 0;
		ulSctee_controlReg |= (
			BCHP_FIELD_DATA (SCTE_0_SHAPER_LUMA_COEFF_6_8, COEFF_6, 0xd0) |
			BCHP_FIELD_DATA (SCTE_0_SHAPER_LUMA_COEFF_6_8, COEFF_7, 0xe0) |
			BCHP_FIELD_DATA (SCTE_0_SHAPER_LUMA_COEFF_6_8, COEFF_8, 0xf0) );
		BREG_Write32 (
			hReg, BCHP_SCTE_0_SHAPER_LUMA_COEFF_6_8 + ulCoreOffset,
			ulSctee_controlReg);

		ulSctee_controlReg = 0;
		ulSctee_controlReg |= (
			BCHP_FIELD_DATA (SCTE_0_SHAPER_CHROMA_COEFF_0_2, COEFF_0, 0x10) |
			BCHP_FIELD_DATA (SCTE_0_SHAPER_CHROMA_COEFF_0_2, COEFF_1, 0x30) |
			BCHP_FIELD_DATA (SCTE_0_SHAPER_CHROMA_COEFF_0_2, COEFF_2, 0x50) );
		BREG_Write32 (
			hReg, BCHP_SCTE_0_SHAPER_CHROMA_COEFF_0_2 + ulCoreOffset,
			ulSctee_controlReg);

		ulSctee_controlReg = 0;
		ulSctee_controlReg |= (
			BCHP_FIELD_DATA (SCTE_0_SHAPER_CHROMA_COEFF_3_5, COEFF_3, 0x70) |
			BCHP_FIELD_DATA (SCTE_0_SHAPER_CHROMA_COEFF_3_5, COEFF_4, 0x90) |
			BCHP_FIELD_DATA (SCTE_0_SHAPER_CHROMA_COEFF_3_5, COEFF_5, 0xc0) );
		BREG_Write32 (
			hReg, BCHP_SCTE_0_SHAPER_CHROMA_COEFF_3_5 + ulCoreOffset,
			ulSctee_controlReg);

		ulSctee_controlReg = 0;
		ulSctee_controlReg |= (
			BCHP_FIELD_DATA (SCTE_0_SHAPER_CHROMA_COEFF_6_8, COEFF_6, 0xd0) |
			BCHP_FIELD_DATA (SCTE_0_SHAPER_CHROMA_COEFF_6_8, COEFF_7, 0xe0) |
			BCHP_FIELD_DATA (SCTE_0_SHAPER_CHROMA_COEFF_6_8, COEFF_8, 0xf0) );
		BREG_Write32 (
			hReg, BCHP_SCTE_0_SHAPER_CHROMA_COEFF_6_8 + ulCoreOffset,
			ulSctee_controlReg);

		ulSctee_controlReg = 0;
		ulSctee_controlReg |= (
			BCHP_FIELD_DATA (SCTE_0_SHAPER_CONTROL,       REPL_COUNT,   0) |
			BCHP_FIELD_DATA (SCTE_0_SHAPER_CONTROL, SHAPE_OUT_OFFSET,   0) |
			BCHP_FIELD_DATA (SCTE_0_SHAPER_CONTROL,  SHAPE_IN_OFFSET,   0) |
			BCHP_FIELD_DATA (SCTE_0_SHAPER_CONTROL,      NRTV_ENABLE,   1) );
		BREG_Write32 (
			hReg, BCHP_SCTE_0_SHAPER_CONTROL + ulCoreOffset,
			ulSctee_controlReg);

		ulSctee_controlReg = 0;
		ulSctee_controlReg |= (
			BCHP_FIELD_DATA (SCTE_0_SHAPER_BLANK_VALUE,   LUMA_BLANK,
				0x00) |
			BCHP_FIELD_DATA (SCTE_0_SHAPER_BLANK_VALUE, CHROMA_BLANK,
				0x80) );
		BREG_Write32 (
			hReg, BCHP_SCTE_0_SHAPER_BLANK_VALUE + ulCoreOffset,
			ulSctee_controlReg);

	/* This is SCTE 21 mode */
	case BVBI_SCTE_Type_CCPAM:

		ulSctee_controlReg = 0;
		ulSctee_controlReg |= (
			BCHP_FIELD_DATA (SCTE_0_SHAPER_LUMA_COEFF_0_2, COEFF_0, 0x10) |
			BCHP_FIELD_DATA (SCTE_0_SHAPER_LUMA_COEFF_0_2, COEFF_1, 0x30) |
			BCHP_FIELD_DATA (SCTE_0_SHAPER_LUMA_COEFF_0_2, COEFF_2, 0x50) );
		BREG_Write32 (
			hReg, BCHP_SCTE_0_SHAPER_LUMA_COEFF_0_2 + ulCoreOffset,
			ulSctee_controlReg);

		ulSctee_controlReg = 0;
		ulSctee_controlReg |= (
			BCHP_FIELD_DATA (SCTE_0_SHAPER_LUMA_COEFF_3_5, COEFF_3, 0x70) |
			BCHP_FIELD_DATA (SCTE_0_SHAPER_LUMA_COEFF_3_5, COEFF_4, 0x90) |
			BCHP_FIELD_DATA (SCTE_0_SHAPER_LUMA_COEFF_3_5, COEFF_5, 0xc0) );
		BREG_Write32 (
			hReg, BCHP_SCTE_0_SHAPER_LUMA_COEFF_3_5 + ulCoreOffset,
			ulSctee_controlReg);

		ulSctee_controlReg = 0;
		ulSctee_controlReg |= (
			BCHP_FIELD_DATA (SCTE_0_SHAPER_LUMA_COEFF_6_8, COEFF_6, 0xd0) |
			BCHP_FIELD_DATA (SCTE_0_SHAPER_LUMA_COEFF_6_8, COEFF_7, 0xe0) |
			BCHP_FIELD_DATA (SCTE_0_SHAPER_LUMA_COEFF_6_8, COEFF_8, 0xf0) );
		BREG_Write32 (
			hReg, BCHP_SCTE_0_SHAPER_LUMA_COEFF_6_8 + ulCoreOffset,
			ulSctee_controlReg);

		ulSctee_controlReg = 0;
		ulSctee_controlReg |= (
			BCHP_FIELD_DATA (SCTE_0_SHAPER_CHROMA_COEFF_0_2, COEFF_0, 0x10) |
			BCHP_FIELD_DATA (SCTE_0_SHAPER_CHROMA_COEFF_0_2, COEFF_1, 0x30) |
			BCHP_FIELD_DATA (SCTE_0_SHAPER_CHROMA_COEFF_0_2, COEFF_2, 0x50) );
		BREG_Write32 (
			hReg, BCHP_SCTE_0_SHAPER_CHROMA_COEFF_0_2 + ulCoreOffset,
			ulSctee_controlReg);

		ulSctee_controlReg = 0;
		ulSctee_controlReg |= (
			BCHP_FIELD_DATA (SCTE_0_SHAPER_CHROMA_COEFF_3_5, COEFF_3, 0x70) |
			BCHP_FIELD_DATA (SCTE_0_SHAPER_CHROMA_COEFF_3_5, COEFF_4, 0x90) |
			BCHP_FIELD_DATA (SCTE_0_SHAPER_CHROMA_COEFF_3_5, COEFF_5, 0xc0) );
		BREG_Write32 (
			hReg, BCHP_SCTE_0_SHAPER_CHROMA_COEFF_3_5 + ulCoreOffset,
			ulSctee_controlReg);

		ulSctee_controlReg = 0;
		ulSctee_controlReg |= (
			BCHP_FIELD_DATA (SCTE_0_SHAPER_CHROMA_COEFF_6_8, COEFF_6, 0xd0) |
			BCHP_FIELD_DATA (SCTE_0_SHAPER_CHROMA_COEFF_6_8, COEFF_7, 0xe0) |
			BCHP_FIELD_DATA (SCTE_0_SHAPER_CHROMA_COEFF_6_8, COEFF_8, 0xf0) );
		BREG_Write32 (
			hReg, BCHP_SCTE_0_SHAPER_CHROMA_COEFF_6_8 + ulCoreOffset,
			ulSctee_controlReg);

		ulSctee_controlReg = 0;
		ulSctee_controlReg |= (
			BCHP_FIELD_DATA (SCTE_0_SHAPER_CONTROL,       REPL_COUNT,   0) |
			BCHP_FIELD_DATA (SCTE_0_SHAPER_CONTROL, SHAPE_OUT_OFFSET,   0) |
			BCHP_FIELD_DATA (SCTE_0_SHAPER_CONTROL,  SHAPE_IN_OFFSET,   0) |
			BCHP_FIELD_DATA (SCTE_0_SHAPER_CONTROL,       PAM_ENABLE,   1) );
		BREG_Write32 (
			hReg, BCHP_SCTE_0_SHAPER_CONTROL + ulCoreOffset,
			ulSctee_controlReg);

		ulSctee_controlReg = 0;
		ulSctee_controlReg |= (
			BCHP_FIELD_DATA (SCTE_0_SHAPER_BLANK_VALUE,   LUMA_BLANK,
				0x00) |
			BCHP_FIELD_DATA (SCTE_0_SHAPER_BLANK_VALUE, CHROMA_BLANK,
				0x80) );
		BREG_Write32 (
			hReg, BCHP_SCTE_0_SHAPER_BLANK_VALUE + ulCoreOffset,
			ulSctee_controlReg);

		break;

	case BVBI_SCTE_Type_CCMONO:

		ulSctee_controlReg = 0;
		ulSctee_controlReg |= (
			BCHP_FIELD_DATA (SCTE_0_SHAPER_LUMA_COEFF_0_2, COEFF_0, 0x10) |
			BCHP_FIELD_DATA (SCTE_0_SHAPER_LUMA_COEFF_0_2, COEFF_1, 0x30) |
			BCHP_FIELD_DATA (SCTE_0_SHAPER_LUMA_COEFF_0_2, COEFF_2, 0x50) );
		BREG_Write32 (
			hReg, BCHP_SCTE_0_SHAPER_LUMA_COEFF_0_2 + ulCoreOffset,
			ulSctee_controlReg);

		ulSctee_controlReg = 0;
		ulSctee_controlReg |= (
			BCHP_FIELD_DATA (SCTE_0_SHAPER_LUMA_COEFF_3_5, COEFF_3, 0x70) |
			BCHP_FIELD_DATA (SCTE_0_SHAPER_LUMA_COEFF_3_5, COEFF_4, 0x90) |
			BCHP_FIELD_DATA (SCTE_0_SHAPER_LUMA_COEFF_3_5, COEFF_5, 0xc0) );
		BREG_Write32 (
			hReg, BCHP_SCTE_0_SHAPER_LUMA_COEFF_3_5 + ulCoreOffset,
			ulSctee_controlReg);

		ulSctee_controlReg = 0;
		ulSctee_controlReg |= (
			BCHP_FIELD_DATA (SCTE_0_SHAPER_LUMA_COEFF_6_8, COEFF_6, 0xd0) |
			BCHP_FIELD_DATA (SCTE_0_SHAPER_LUMA_COEFF_6_8, COEFF_7, 0xe0) |
			BCHP_FIELD_DATA (SCTE_0_SHAPER_LUMA_COEFF_6_8, COEFF_8, 0xf0) );
		BREG_Write32 (
			hReg, BCHP_SCTE_0_SHAPER_LUMA_COEFF_6_8 + ulCoreOffset,
			ulSctee_controlReg);

		ulSctee_controlReg = 0;
		ulSctee_controlReg |= (
			BCHP_FIELD_DATA (SCTE_0_SHAPER_CHROMA_COEFF_0_2, COEFF_0, 0x10) |
			BCHP_FIELD_DATA (SCTE_0_SHAPER_CHROMA_COEFF_0_2, COEFF_1, 0x30) |
			BCHP_FIELD_DATA (SCTE_0_SHAPER_CHROMA_COEFF_0_2, COEFF_2, 0x50) );
		BREG_Write32 (
			hReg, BCHP_SCTE_0_SHAPER_CHROMA_COEFF_0_2 + ulCoreOffset,
			ulSctee_controlReg);

		ulSctee_controlReg = 0;
		ulSctee_controlReg |= (
			BCHP_FIELD_DATA (SCTE_0_SHAPER_CHROMA_COEFF_3_5, COEFF_3, 0x70) |
			BCHP_FIELD_DATA (SCTE_0_SHAPER_CHROMA_COEFF_3_5, COEFF_4, 0x90) |
			BCHP_FIELD_DATA (SCTE_0_SHAPER_CHROMA_COEFF_3_5, COEFF_5, 0xc0) );
		BREG_Write32 (
			hReg, BCHP_SCTE_0_SHAPER_CHROMA_COEFF_3_5 + ulCoreOffset,
			ulSctee_controlReg);

		ulSctee_controlReg = 0;
		ulSctee_controlReg |= (
			BCHP_FIELD_DATA (SCTE_0_SHAPER_CHROMA_COEFF_6_8, COEFF_6, 0xd0) |
			BCHP_FIELD_DATA (SCTE_0_SHAPER_CHROMA_COEFF_6_8, COEFF_7, 0xe0) |
			BCHP_FIELD_DATA (SCTE_0_SHAPER_CHROMA_COEFF_6_8, COEFF_8, 0xf0) );
		BREG_Write32 (
			hReg, BCHP_SCTE_0_SHAPER_CHROMA_COEFF_6_8 + ulCoreOffset,
			ulSctee_controlReg);

		ulSctee_controlReg = 0;
		ulSctee_controlReg |= (
			BCHP_FIELD_DATA (SCTE_0_SHAPER_CONTROL,       REPL_COUNT,   0) |
			BCHP_FIELD_DATA (SCTE_0_SHAPER_CONTROL, SHAPE_OUT_OFFSET,   0) |
			BCHP_FIELD_DATA (SCTE_0_SHAPER_CONTROL,  SHAPE_IN_OFFSET,   0) |
			BCHP_FIELD_DATA (SCTE_0_SHAPER_CONTROL,       PAM_ENABLE,   1) );
		BREG_Write32 (
			hReg, BCHP_SCTE_0_SHAPER_CONTROL + ulCoreOffset,
			ulSctee_controlReg);

		ulSctee_controlReg = 0;
		ulSctee_controlReg |= (
			BCHP_FIELD_DATA (SCTE_0_SHAPER_BLANK_VALUE,   LUMA_BLANK,
				0x00) |
			BCHP_FIELD_DATA (SCTE_0_SHAPER_BLANK_VALUE, CHROMA_BLANK,
				0x80) );
		BREG_Write32 (
			hReg, BCHP_SCTE_0_SHAPER_BLANK_VALUE + ulCoreOffset,
			ulSctee_controlReg);

		/*
		 * Override CSC settings for monochrome data!
		 */
		ulSctee_controlReg = 0;
		ulSctee_controlReg |= (
			BCHP_FIELD_DATA (SCTE_0_CSC_COEFF_C03_C00, COEFF_C0, 0x800) |
			BCHP_FIELD_DATA (SCTE_0_CSC_COEFF_C03_C00, COEFF_C3, 0x000) );
		BREG_Write32 (
			hReg, BCHP_SCTE_0_CSC_COEFF_C03_C00 + ulCoreOffset,
			ulSctee_controlReg);

		ulSctee_controlReg = 0;
		ulSctee_controlReg |= (
			BCHP_FIELD_DATA (SCTE_0_CSC_COEFF_C12_C11, COEFF_C1, 0x800) |
			BCHP_FIELD_DATA (SCTE_0_CSC_COEFF_C12_C11, COEFF_C2, 0x000) );
		BREG_Write32 (
			hReg, BCHP_SCTE_0_CSC_COEFF_C12_C11 + ulCoreOffset,
			ulSctee_controlReg);

		ulSctee_controlReg = 0;
		ulSctee_controlReg |=
			BCHP_FIELD_DATA (SCTE_0_CSC_COEFF_C13, COEFF_C3, 0x000) ;
		BREG_Write32 (
			hReg, BCHP_SCTE_0_CSC_COEFF_C13 + ulCoreOffset,
			ulSctee_controlReg);

		ulSctee_controlReg = 0;
		ulSctee_controlReg |= (
			BCHP_FIELD_DATA (SCTE_0_CSC_COEFF_C22_C21, COEFF_C1, 0x000) |
			BCHP_FIELD_DATA (SCTE_0_CSC_COEFF_C22_C21, COEFF_C2, 0x800) );
		BREG_Write32 (
			hReg, BCHP_SCTE_0_CSC_COEFF_C22_C21 + ulCoreOffset,
			ulSctee_controlReg);

		ulSctee_controlReg = 0;
		ulSctee_controlReg |=
			BCHP_FIELD_DATA (SCTE_0_CSC_COEFF_C23, COEFF_C3, 0x000) ;
		BREG_Write32 (
			hReg, BCHP_SCTE_0_CSC_COEFF_C23 + ulCoreOffset,
			ulSctee_controlReg);

		break;

		default:
			BDBG_LEAVE(BVBI_P_SCTE_Enc_Program);
			return BERR_TRACE(BERR_INVALID_PARAMETER);
			break;
	}

	/*
	 * TODO: configure SCTE_0_NRTV_SAMPLE_ORDER register. Default values
	 * suffice.
	 */

	BDBG_LEAVE(BVBI_P_SCTE_Enc_Program);
	return BERR_SUCCESS;

#else /** } ! BVBI_P_NUM_SCTEE { **/

	BSTD_UNUSED (hReg);
	BSTD_UNUSED (is656);
	BSTD_UNUSED (hwCoreIndex);
	BSTD_UNUSED (bActive);
	BSTD_UNUSED (eVideoFormat);
	BSTD_UNUSED (bArib480p);
	BSTD_UNUSED (scteType);
	BSTD_UNUSED (csc);
	BSTD_UNUSED (coCsc);

	return BERR_TRACE (BVBI_ERR_HW_UNSUPPORTED);

#endif /** } BVBI_P_NUM_SCTEE **/
}

/***************************************************************************
 *
 */
uint32_t BVBI_P_SCTE_Encode_Data_isr (
	BREG_Handle hReg,
	BMEM_Handle hMem,
	bool is656,
	uint8_t hwCoreIndex,
	BFMT_VideoFmt eVideoFormat,
	BAVC_Polarity polarity,
	BVBI_SCTE_Type scteType,
	BVBI_P_SCTE_Data* pData,
	BVBI_LineBuilder_Handle hTopScteNrtv[2],
	BVBI_LineBuilder_Handle hBotScteNrtv[2],
	BVBI_LineBuilder_Handle hTopScteMono[2],
	BVBI_LineBuilder_Handle hBotScteMono[2],
	uint8_t** pSctePamData
)
{
#if (BVBI_P_NUM_SCTEE > 0)  /** { **/

	BVBI_LineBuilder_Handle *hScteNrtv;
	BVBI_LineBuilder_Handle *hScteMono;
	BERR_Code eErr;
	uint32_t hardware_offset;
	uint32_t ulCoreOffset;
	uint32_t ulRegVal;
	uint32_t ulRegAddr;
	uint32_t ulReadPointer;
	uint32_t ulWritePointer;
	uint32_t sourceIndex;
	uint32_t bankIndex;
	uint32_t regBankOffset;
	uint32_t regLineOffset;
	uint32_t dataValue;
	int      lineIndex;
	int iBulk;
	int nrtv_sequence_number[2] = {0, 0};
	uint8_t*    bulkNrtvData[2] = {0, 0};
	int line_number[2];

	/* Debug code
	uint32_t dread_pointer[2];
	uint32_t dwrite_pointer[2];
	*/

	BDBG_ENTER(BVBI_P_SCTE_Encode_Data_isr);

	/* Size check for field data */
	if (!pData)
	{
		BDBG_LEAVE(BVBI_P_SCTE_Encode_Data_isr);
		return (BVBI_LINE_ERROR_FLDH_CONFLICT);
	}

	/* Figure out which encoder core to use */
	ulCoreOffset = P_GetCoreOffset (hwCoreIndex);
	if (is656 || (ulCoreOffset == 0xFFFFFFFF))
	{
		/* This should never happen!  This parameter was checked by
		   BVBI_Encode_Create() */
		ulCoreOffset = 0;
		BDBG_ASSERT (ulCoreOffset);
	}

	/* Complain if video format is not supported */
	switch (eVideoFormat)
	{
	case BFMT_VideoFmt_eNTSC:
	case BFMT_VideoFmt_eNTSC_J:
	case BFMT_VideoFmt_e720x482_NTSC:
	case BFMT_VideoFmt_e720x482_NTSC_J:
		break;

	default:
		/* Should not happen */
		BDBG_ERR(("BVBI_SCTEE: video format %d not supported", eVideoFormat));
		BDBG_ASSERT (0);
		break;
	}

	/* Verify SCTE type */
	/* TODO: handle monochrome data */
	switch (scteType)
	{
	case BVBI_SCTE_Type_CCONLY:
	case BVBI_SCTE_Type_CCNRTV:
	case BVBI_SCTE_Type_CCPAM:
	/* case BVBI_SCTE_Type_CCMONO: */
		break;
	default:
		BDBG_LEAVE(BVBI_P_SCTE_Encode_Data_isr);
		return BERR_SUCCESS;
		break;
	}

	/* Clear linecount underflow conditions */
	ulRegVal = BREG_Read32 (hReg, BCHP_SCTE_0_FIFO_STATUS + ulCoreOffset);
	ulRegVal |= (
		BCHP_FIELD_DATA (SCTE_0_FIFO_STATUS, UNDERFLOW_0, 1) |
		BCHP_FIELD_DATA (SCTE_0_FIFO_STATUS, UNDERFLOW_1, 1) );
	BREG_Write32 (hReg, BCHP_SCTE_0_FIFO_STATUS + ulCoreOffset, ulRegVal);

	/* TODO: clear other status bits? */

	/* Get FIFO pointers */
	ulRegVal = BREG_Read32 (hReg, BCHP_SCTE_0_BANK_CONTROL + ulCoreOffset);
	ulWritePointer =
		BCHP_GET_FIELD_DATA (ulRegVal, SCTE_0_BANK_CONTROL, WRITE_PTR);
	bankIndex = ulWritePointer & 0x00000003;
	ulRegVal = BREG_Read32 (hReg, BCHP_SCTE_0_FIFO_STATUS + ulCoreOffset);
	ulReadPointer =
		BCHP_GET_FIELD_DATA (ulRegVal, SCTE_0_FIFO_STATUS, READ_PTR);

	/* Debug code
	dread_pointer[0]  = ulReadPointer;
	dwrite_pointer[0] = ulWritePointer;
	*/

	/* Check for FIFO full */
	if (((ulReadPointer & 0x3) == bankIndex     ) &&
	    (ulReadPointer         != ulWritePointer)    )
	{
		/* Debug code
		printf ("\n   ***   SCTE FIFO full!!!   ***\n\n");
		*/

		return BVBI_LINE_ERROR_SCTE_OVERRUN;
	}

	/* Choose bulk data stores based on field polarity */
	if (polarity == BAVC_Polarity_eTopField)
	{
		hScteNrtv = hTopScteNrtv;
		hScteMono = hTopScteMono;
	}
	else
	{
		hScteNrtv = hBotScteNrtv;
		hScteMono = hBotScteMono;
	}

	/* Handle field misalignment */
	if (
		((bankIndex == 0) || (bankIndex == 2)) &&
		(polarity != BAVC_Polarity_eTopField)
	)
	{
		BVBI_P_ProgramNull (hReg, ulCoreOffset, bankIndex, 1);
		++ulWritePointer;
	}
	else if (
		((bankIndex == 1) || (bankIndex == 3)) &&
		(polarity != BAVC_Polarity_eBotField)
	)
	{
		BVBI_P_ProgramNull (hReg, ulCoreOffset, bankIndex, 1);
		++ulWritePointer;
	}
	else
	{
		BVBI_P_ProgramNull (hReg, ulCoreOffset, bankIndex, 0);
	}

	/* Set DMA length to zero. This default will be overridden if PAM, NRTV, or
	 * monochrome data is present.
	 */
	regBankOffset =
		BCHP_SCTE_0_CH0_DMA_LENGTH_BANK1 -
		BCHP_SCTE_0_CH0_DMA_LENGTH_BANK0;
	ulRegAddr = BCHP_SCTE_0_CH0_DMA_LENGTH_BANK0 + ulCoreOffset +
		(regBankOffset * bankIndex);
	ulRegVal = 0;
	ulRegVal |= (
		BCHP_FIELD_DATA (
			SCTE_0_CH0_DMA_LENGTH_BANK0, FIELD_NUM,       1) |
		BCHP_FIELD_DATA (
			SCTE_0_CH0_DMA_LENGTH_BANK0,     RESET,       0) |
		BCHP_FIELD_DATA (
			SCTE_0_CH0_DMA_LENGTH_BANK0,    LENGTH,       0) );
	BREG_Write32 (hReg, ulRegAddr, ulRegVal);

	regBankOffset =
		BCHP_SCTE_0_CH1_DMA_LENGTH_BANK1 -
		BCHP_SCTE_0_CH1_DMA_LENGTH_BANK0;
	ulRegAddr = BCHP_SCTE_0_CH1_DMA_LENGTH_BANK0 + ulCoreOffset +
		(regBankOffset * bankIndex);
	ulRegVal = 0;
	ulRegVal |= (
		BCHP_FIELD_DATA (
			SCTE_0_CH1_DMA_LENGTH_BANK0,     RESET,       0) |
		BCHP_FIELD_DATA (
			SCTE_0_CH1_DMA_LENGTH_BANK0,    LENGTH,       0) );
	BREG_Write32 (hReg, ulRegAddr, ulRegVal);

	/* Program the line control registers to some sensible defaults */
	for (lineIndex = 0 ; lineIndex < BVBI_SCTE_MAX_ITEMS ; ++lineIndex)
	{
		regBankOffset =
			BCHP_SCTE_0_LCR_CONTROLi_BANK1_ARRAY_BASE -
				BCHP_SCTE_0_LCR_CONTROLi_BANK0_ARRAY_BASE;
		regLineOffset = 4;
		ulRegAddr =
			BCHP_SCTE_0_LCR_CONTROLi_BANK0_ARRAY_BASE + ulCoreOffset +
			(regBankOffset * bankIndex) + (regLineOffset * lineIndex);
		ulRegVal = BREG_Read32 (hReg, ulRegAddr);
		ulRegVal &= ~(
			BCHP_MASK (SCTE_0_LCR_CONTROLi_BANK0, LINE_OFFSET  ) |
			BCHP_MASK (SCTE_0_LCR_CONTROLi_BANK0, VBI_DATA_TYPE)   );
		ulRegVal |=
			BCHP_FIELD_DATA (
				SCTE_0_LCR_CONTROLi_BANK0, LINE_OFFSET, lineIndex);
		if (scteType == BVBI_SCTE_Type_CCPAM)
		{
			ulRegVal |=
				BCHP_FIELD_ENUM (
					SCTE_0_LCR_CONTROLi_BANK0, VBI_DATA_TYPE, PAM_DATA);
		}
		else
		{
			ulRegVal |=
				BCHP_FIELD_ENUM (
					SCTE_0_LCR_CONTROLi_BANK0, VBI_DATA_TYPE, NO_DATA);
		}
		BREG_Write32 (hReg, ulRegAddr, ulRegVal);
	}

	/* Debug code: research a particular problem.
	for (lineIndex = 4 ; lineIndex < BVBI_SCTE_MAX_ITEMS ; ++lineIndex)
	{
		if ((lineIndex == 8) || (lineIndex == 11))
			continue;
		regBankOffset =
			BCHP_SCTE_0_LCR_CONTROLi_BANK1_ARRAY_BASE -
				BCHP_SCTE_0_LCR_CONTROLi_BANK0_ARRAY_BASE;
		regLineOffset = 4;
		ulRegAddr =
			BCHP_SCTE_0_LCR_CONTROLi_BANK0_ARRAY_BASE + ulCoreOffset +
			(regBankOffset * bankIndex) + (regLineOffset * lineIndex);
		ulRegVal = BREG_Read32 (hReg, ulRegAddr);
		ulRegVal &= ~(
		    BCHP_MASK       (SCTE_0_LCR_CONTROLi_BANK0, VBI_DATA_TYPE) |
			BCHP_MASK       (SCTE_0_LCR_CONTROLi_BANK0, DELAY_COUNT  ) );
		ulRegVal |= (
			BCHP_FIELD_ENUM (SCTE_0_LCR_CONTROLi_BANK0, VBI_DATA_TYPE,
				NRTV_DATA) |
			BCHP_FIELD_DATA (SCTE_0_LCR_CONTROLi_BANK0, DELAY_COUNT  ,
				     0x30) );
		BREG_Write32 (hReg, ulRegAddr, ulRegVal);
	}
	*/

	/* Commit the line-oriented data to hardware */
	for (sourceIndex = 0 ; sourceIndex < pData->line_count ; ++sourceIndex)
	{
		BVBI_P_SCTE_Line_Data* lineData = &(pData->pLine[sourceIndex]);
		lineIndex = lineData->line_number - 10;
		if (lineData->eType == BVBI_SCTE_Type_CCONLY)
		{
			if ((scteType == BVBI_SCTE_Type_CCONLY) ||
			    (scteType == BVBI_SCTE_Type_CCNRTV) ||
			    (scteType == BVBI_SCTE_Type_CCPAM ) ||
			    (scteType == BVBI_SCTE_Type_CCMONO)   )
			{
				/*
				 * Process one line of CC data
				 */
				BVBI_P_SCTE_CC_Line_Data* pScteCcData = &(lineData->d.CC);

				/* Debug code
				printf ("    Programming LCR bank %d, line %d\n",
					bankIndex, lineIndex);
				*/

				/* Locate and program the LCR control register */
				regBankOffset =
					BCHP_SCTE_0_LCR_CONTROLi_BANK1_ARRAY_BASE -
						BCHP_SCTE_0_LCR_CONTROLi_BANK0_ARRAY_BASE;
				regLineOffset = 4;
				ulRegAddr =
					BCHP_SCTE_0_LCR_CONTROLi_BANK0_ARRAY_BASE +
						ulCoreOffset +
						(regBankOffset * bankIndex) +
						(regLineOffset * lineIndex);
				ulRegVal = BREG_Read32 (hReg, ulRegAddr);
				ulRegVal &= ~(
					BCHP_MASK       (
						SCTE_0_LCR_CONTROLi_BANK0, reserved0         ) |
					BCHP_MASK       (
						SCTE_0_LCR_CONTROLi_BANK0, CC_SHIFT_DIRECTION) |
					BCHP_MASK       (
						SCTE_0_LCR_CONTROLi_BANK0, CC_BYTE_ORDER     ) |
					BCHP_MASK       (
						SCTE_0_LCR_CONTROLi_BANK0, CC_PARITY_TYPE    ) |
					BCHP_MASK       (
						SCTE_0_LCR_CONTROLi_BANK0, CC_PARITY         ) |
					BCHP_MASK       (
						SCTE_0_LCR_CONTROLi_BANK0, CC_RUN_IN         ) |
					BCHP_MASK       (
						SCTE_0_LCR_CONTROLi_BANK0, CC_NULL           ) |
					BCHP_MASK       (
						SCTE_0_LCR_CONTROLi_BANK0, DELAY_COUNT       ) |
					BCHP_MASK       (
						SCTE_0_LCR_CONTROLi_BANK0, VBI_DATA_TYPE     ) );
				ulRegVal |= (
					BCHP_FIELD_DATA (
						SCTE_0_LCR_CONTROLi_BANK0, reserved0         ,
							0x0) |
					BCHP_FIELD_ENUM (
						SCTE_0_LCR_CONTROLi_BANK0, CC_SHIFT_DIRECTION,
							LSB_FIRST) |
					BCHP_FIELD_ENUM (
						SCTE_0_LCR_CONTROLi_BANK0, CC_BYTE_ORDER     ,
							LOW_BYTE_FIRST) |
					BCHP_FIELD_ENUM (
						SCTE_0_LCR_CONTROLi_BANK0, CC_PARITY_TYPE    ,
							ODD) |
					BCHP_FIELD_ENUM (
						SCTE_0_LCR_CONTROLi_BANK0, CC_PARITY         ,
							ENABLE) |
					BCHP_FIELD_ENUM (
						SCTE_0_LCR_CONTROLi_BANK0, CC_RUN_IN         ,
							ENABLE) |
					BCHP_FIELD_ENUM (
						SCTE_0_LCR_CONTROLi_BANK0, CC_NULL           ,
							DISABLE) |
					BCHP_FIELD_DATA (
						SCTE_0_LCR_CONTROLi_BANK0, DELAY_COUNT       ,
						       0x24) |
					BCHP_FIELD_ENUM (
						SCTE_0_LCR_CONTROLi_BANK0, VBI_DATA_TYPE     ,
							CC_DATA) );
				BREG_Write32 (hReg, ulRegAddr, ulRegVal);

				/* Locate and program the LCR Data register */
				regBankOffset =
					BCHP_SCTE_0_LCR_DATAi_BANK1_ARRAY_BASE -
						BCHP_SCTE_0_LCR_DATAi_BANK0_ARRAY_BASE;
				regLineOffset = 4;
				ulRegAddr =
					BCHP_SCTE_0_LCR_DATAi_BANK0_ARRAY_BASE + ulCoreOffset +
						(regBankOffset * bankIndex) +
						(regLineOffset * lineIndex);
				dataValue =
					(pScteCcData->cc_data_2 << 8) | pScteCcData->cc_data_1;
				ulRegVal = 0x0;
				ulRegVal |=
					BCHP_FIELD_DATA (SCTE_0_LCR_DATAi_BANK0, CC_DATA,
						dataValue);
				BREG_Write32 (hReg, ulRegAddr, ulRegVal);
			}
		}
		else if (lineData->eType == BVBI_SCTE_Type_CCNRTV)
		{
			if (scteType == BVBI_SCTE_Type_CCNRTV)
			{
				/*
				 * Process one section of one line of NRTV data
				 */
				BVBI_P_SCTE_NTRV_Line_Data* pScteNrtvData = &(lineData->d.NRTV);

				/* Accumulate section */
				for (iBulk = 0 ; iBulk < 2 ; ++iBulk)
				{
					eErr = BVBI_LineBuilder_Put (
						hScteNrtv[iBulk],
						pData->nrtv_data[iBulk],
						32,
						32 * (pScteNrtvData->segment_number - 1),
						pScteNrtvData->sequence_number,
						lineData->line_number);
					if (eErr != BERR_SUCCESS)
						return BVBI_LINE_ERROR_SCTE_NODATA;
				}

				/* Point to the assembled NRTV data, if any */
				for (iBulk = 0 ; iBulk < 2 ; ++iBulk)
				{
					eErr = BVBI_LineBuilder_Get (
						hScteNrtv[iBulk], &bulkNrtvData[iBulk],
						&nrtv_sequence_number[iBulk], &line_number[iBulk]);
					if (eErr != BERR_SUCCESS)
						return BVBI_LINE_ERROR_SCTE_NODATA;
				}

				/* A few sanity checks */
				if (nrtv_sequence_number[0] != nrtv_sequence_number[1])
					return BVBI_LINE_ERROR_SCTE_NODATA;
				if (line_number[0] != line_number[1])
					return BVBI_LINE_ERROR_SCTE_NODATA;

				/* Debug code
				printf (
					"Programming LCR bank %d, line %d, seg. no. %d "
					"seq. no. (%d %d)\n",
					bankIndex, lineIndex, pScteNrtvData->segment_number,
					nrtv_sequence_number[0], nrtv_sequence_number[1]);
				*/

				/* Locate and program the LCR control register */
				regBankOffset =
					BCHP_SCTE_0_LCR_CONTROLi_BANK1_ARRAY_BASE -
						BCHP_SCTE_0_LCR_CONTROLi_BANK0_ARRAY_BASE;
				regLineOffset = 4;
				ulRegAddr =
					BCHP_SCTE_0_LCR_CONTROLi_BANK0_ARRAY_BASE +
						ulCoreOffset +
						(regBankOffset * bankIndex) +
						(regLineOffset * lineIndex);
				ulRegVal = BREG_Read32 (hReg, ulRegAddr);
				ulRegVal &= ~(
					BCHP_MASK       (
						SCTE_0_LCR_CONTROLi_BANK0, reserved0         ) |
					BCHP_MASK       (
						SCTE_0_LCR_CONTROLi_BANK0, DELAY_COUNT       ) |
					BCHP_MASK       (
						SCTE_0_LCR_CONTROLi_BANK0, VBI_DATA_TYPE     ) );
				ulRegVal |= (
					BCHP_FIELD_DATA (
						SCTE_0_LCR_CONTROLi_BANK0, reserved0         ,
							0x0) |
					BCHP_FIELD_DATA (
						SCTE_0_LCR_CONTROLi_BANK0, DELAY_COUNT       ,
						   0x30) );
				if (nrtv_sequence_number[0] == 0)
				{
					ulRegVal |=
						BCHP_FIELD_ENUM (
							SCTE_0_LCR_CONTROLi_BANK0, VBI_DATA_TYPE     ,
							NO_DATA);
				}
				else
				{
					ulRegVal |=
						BCHP_FIELD_ENUM (
							SCTE_0_LCR_CONTROLi_BANK0, VBI_DATA_TYPE     ,
							NRTV_DATA);
				}
				BREG_Write32 (hReg, ulRegAddr, ulRegVal);
			}
		}
	}

	/* Transfer persistent NRTV data, if any. */
	if ((scteType == BVBI_SCTE_Type_CCNRTV) && (nrtv_sequence_number[0] != 0))
	{

		/* Write start registers: start them at the beginning. */
		regBankOffset =
			BCHP_SCTE_0_NRTV_WRITE_PTR_BANK1 -
			BCHP_SCTE_0_NRTV_WRITE_PTR_BANK0;
		ulRegAddr =
			BCHP_SCTE_0_NRTV_WRITE_PTR_BANK0 + ulCoreOffset +
				(regBankOffset * bankIndex);
		ulRegVal = 0;
		ulRegVal |= (
			BCHP_FIELD_DATA (
				SCTE_0_NRTV_WRITE_PTR_BANK0,   PTR_WR_ENABLE, 0) |
			BCHP_FIELD_DATA (
				SCTE_0_NRTV_WRITE_PTR_BANK0, Cb_Cr_WRITE_PTR, 0) |
			BCHP_FIELD_DATA (
				SCTE_0_NRTV_WRITE_PTR_BANK0,     Y_WRITE_PTR, 0) );
		BREG_Write32 (hReg, ulRegAddr, ulRegVal);

		/* Program DMA addresses */
		regBankOffset =
			BCHP_SCTE_0_CH0_DMA_ADDR_BANK1 -
			BCHP_SCTE_0_CH0_DMA_ADDR_BANK0;
		ulRegAddr =
			BCHP_SCTE_0_CH0_DMA_ADDR_BANK0 + ulCoreOffset +
				(regBankOffset * bankIndex);
		eErr = BERR_TRACE (BMEM_ConvertAddressToOffset (
			hMem, bulkNrtvData[0], &hardware_offset));
		BDBG_ASSERT (eErr == BERR_SUCCESS);
		/* Debug code
		printf ("Programmed bank %d DMA CH0 address %08x "
			"(read pointer was %d)\n",
			bankIndex, hardware_offset, ulReadPointer);
		printf ("CH0:\n");
		for (ulRegVal = 0 ; ulRegVal < 704 ; ++ulRegVal)
			printf ("%02x ", bulkNrtvData[0][ulRegVal]);
		printf ("\n");
		*/
		ulRegVal = 0;
		ulRegVal |=
			BCHP_FIELD_DATA (SCTE_0_CH0_DMA_ADDR_BANK0, START_ADDR,
			hardware_offset);
		BREG_Write32 (hReg, ulRegAddr, ulRegVal);

		ulRegAddr =
			BCHP_SCTE_0_CH1_DMA_ADDR_BANK0 + ulCoreOffset +
				(regBankOffset * bankIndex);
		eErr = BERR_TRACE (BMEM_ConvertAddressToOffset (
			hMem, bulkNrtvData[1], &hardware_offset));
		BDBG_ASSERT (eErr == BERR_SUCCESS);
		/* Debug code
		printf ("Programmed bank %d DMA CH1 address %08x "
		"(read pointer was %d)\n",
			bankIndex, hardware_offset, ulReadPointer);
		printf ("CH1:\n");
		for (ulRegVal = 0 ; ulRegVal < 704 ; ++ulRegVal)
			printf ("%02x ", bulkNrtvData[1][ulRegVal]);
		printf ("\n");
		*/
		ulRegVal = 0;
		ulRegVal |=
			BCHP_FIELD_DATA (SCTE_0_CH1_DMA_ADDR_BANK0, START_ADDR,
			hardware_offset);
		BREG_Write32 (hReg, ulRegAddr, ulRegVal);

		/* Debug code
		printf ("    Programming DMA addresses %08x %08x\n",
			bulkNrtvData[0], bulkNrtvData[1]);
		*/

		/* Program DMA length, etc. */
		regBankOffset =
			BCHP_SCTE_0_CH0_DMA_LENGTH_BANK1 -
			BCHP_SCTE_0_CH0_DMA_LENGTH_BANK0;
		ulRegAddr = BCHP_SCTE_0_CH0_DMA_LENGTH_BANK0 + ulCoreOffset +
			(regBankOffset * bankIndex);
		ulRegVal = 0;
		ulRegVal |= (
			BCHP_FIELD_DATA (
				SCTE_0_CH0_DMA_LENGTH_BANK0, FIELD_NUM,       1) |
			BCHP_FIELD_DATA (
				SCTE_0_CH0_DMA_LENGTH_BANK0,     RESET,       1) |
			BCHP_FIELD_DATA (
				SCTE_0_CH0_DMA_LENGTH_BANK0,    LENGTH,     720) );
		BREG_Write32 (hReg, ulRegAddr, ulRegVal);

		regBankOffset =
			BCHP_SCTE_0_CH1_DMA_LENGTH_BANK1 -
			BCHP_SCTE_0_CH1_DMA_LENGTH_BANK0;
		ulRegAddr = BCHP_SCTE_0_CH1_DMA_LENGTH_BANK0 + ulCoreOffset +
			(regBankOffset * bankIndex);
		ulRegVal = 0;
		ulRegVal |= (
			BCHP_FIELD_DATA (
				SCTE_0_CH1_DMA_LENGTH_BANK0,     RESET,       1) |
			BCHP_FIELD_DATA (
				SCTE_0_CH1_DMA_LENGTH_BANK0,    LENGTH,     720) );
		BREG_Write32 (hReg, ulRegAddr, ulRegVal);
	}

	/* Program PAM registers */
	if ((pData->pam_data_count != 0) && (scteType == BVBI_SCTE_Type_CCPAM))
	{
		uint8_t* sourceArray;
		int dma_length;
		int field_index = pData->field_number;
		int reset = (bankIndex == 0) ? 1 : 0;
		BERR_Code eErr = BERR_SUCCESS;

		/* Swap PAM bulk data between field handle and VEC encoder handle */
		P_SWAP (*pSctePamData, pData->pam_data, sourceArray);
		dma_length = pData->pam_data_count;

		regBankOffset =
			BCHP_SCTE_0_CH0_DMA_ADDR_BANK1 -
			BCHP_SCTE_0_CH0_DMA_ADDR_BANK0;
		ulRegAddr =
			BCHP_SCTE_0_CH0_DMA_ADDR_BANK0 + ulCoreOffset +
				(regBankOffset * bankIndex);
		eErr = BERR_TRACE (BMEM_ConvertAddressToOffset (
			hMem, *pSctePamData, &hardware_offset));
		BDBG_ASSERT (eErr == BERR_SUCCESS);
		ulRegVal = 0;
		ulRegVal |=
			BCHP_FIELD_DATA (SCTE_0_CH0_DMA_ADDR_BANK0, START_ADDR,
			hardware_offset);
		BREG_Write32 (hReg, ulRegAddr, ulRegVal);

		regBankOffset =
			BCHP_SCTE_0_CH0_DMA_LENGTH_BANK1 -
			BCHP_SCTE_0_CH0_DMA_LENGTH_BANK0;
		ulRegAddr = BCHP_SCTE_0_CH0_DMA_LENGTH_BANK0 + ulCoreOffset +
			(regBankOffset * bankIndex);
		ulRegVal = 0;
		ulRegVal |= (
			BCHP_FIELD_DATA (
				SCTE_0_CH0_DMA_LENGTH_BANK0, FIELD_NUM, field_index) |
			BCHP_FIELD_DATA (
				SCTE_0_CH0_DMA_LENGTH_BANK0,     RESET,       reset) |
			BCHP_FIELD_DATA (
				SCTE_0_CH0_DMA_LENGTH_BANK0,    LENGTH,  dma_length) );
		BREG_Write32 (hReg, ulRegAddr, ulRegVal);

		/* Debug code
		printf ("PAM length: register %08x, value %08x\n", ulRegAddr, ulRegVal);
		*/
	}

	/* TODO: monochrome data. */

	/* Program the write pointer into hardware */
	++ulWritePointer;
	ulWritePointer &= 0x7;
	ulRegVal =
		BCHP_FIELD_DATA (SCTE_0_BANK_CONTROL, WRITE_PTR, ulWritePointer);
	BREG_Write32 (hReg, BCHP_SCTE_0_BANK_CONTROL + ulCoreOffset, ulRegVal);

	/* Debug code
	ulRegVal = BREG_Read32 (hReg, BCHP_SCTE_0_BANK_CONTROL + ulCoreOffset);
	ulWritePointer =
		BCHP_GET_FIELD_DATA (ulRegVal, SCTE_0_BANK_CONTROL, WRITE_PTR);
	dwrite_pointer[1] = ulWritePointer;
	ulRegVal = BREG_Read32 (hReg, BCHP_SCTE_0_FIFO_STATUS + ulCoreOffset);
	dread_pointer[1] =
		BCHP_GET_FIELD_DATA (ulRegVal, SCTE_0_FIFO_STATUS, READ_PTR);
	printf ("Programming complete: R/W pointers now %d/%d\n",
		dread_pointer[1], dwrite_pointer[1]);
	printf (
		"Field %c:  R/W (%d/%d) --> (%d/%d)\n",
		((polarity == BAVC_Polarity_eTopField) ? 'T' : 'B'),
		dread_pointer[0], dwrite_pointer[0],
		dread_pointer[1], dwrite_pointer[1]);
	*/

	BDBG_LEAVE(BVBI_P_SCTE_Encode_Data_isr);
	return 0x0;

#else /** } ! BVBI_P_NUM_SCTEE { **/

	BSTD_UNUSED (hMem);
	BSTD_UNUSED (hReg);
	BSTD_UNUSED (is656);
	BSTD_UNUSED (hwCoreIndex);
	BSTD_UNUSED (eVideoFormat);
	BSTD_UNUSED (polarity);
	BSTD_UNUSED (scteType);
	BSTD_UNUSED (pData);
	BSTD_UNUSED (pSctePamData);
	BSTD_UNUSED (hTopScteNrtv);
	BSTD_UNUSED (hBotScteNrtv);
	BSTD_UNUSED (hTopScteMono);
	BSTD_UNUSED (hBotScteMono);

	return (-1);

#endif /** } BVBI_P_NUM_SCTEE **/
}

/***************************************************************************
 *
 */
BERR_Code BVBI_P_SCTE_Encode_Enable_isr (
	BREG_Handle hReg,
	bool is656,
	uint8_t hwCoreIndex,
	BFMT_VideoFmt eVideoFormat,
	BVBI_SCTE_Type scteType,
	bool bEnable)
{
#if (BVBI_P_NUM_SCTEE > 0)  /** { **/

	uint32_t ulCoreOffset;
	uint32_t ulSctee_controlReg;

	BSTD_UNUSED (eVideoFormat);

	BDBG_ENTER(BVBI_P_SCTE_Encode_Enable_isr);

	/* Figure out which encoder core to use */
	ulCoreOffset = P_GetCoreOffset (hwCoreIndex);
	if (is656 || (ulCoreOffset == 0xFFFFFFFF))
	{
		/* This should never happen!  This parameter was checked by
		   BVBI_Encode_Create() */
		BDBG_LEAVE(BVBI_P_SCTE_Encode_Enable_isr);
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	ulSctee_controlReg =
		BREG_Read32 (hReg, BCHP_SCTE_0_CONFIG + ulCoreOffset);
	ulSctee_controlReg &= ~(
		BCHP_MASK       (SCTE_0_CONFIG, VBI_MODE) );
	if (bEnable)
	{
		ulSctee_controlReg |= (
			BCHP_FIELD_DATA (SCTE_0_CONFIG, VBI_MODE, scteType) );
	}
	else
	{
		ulSctee_controlReg |= (
			BCHP_FIELD_ENUM (SCTE_0_CONFIG, VBI_MODE, DISABLE) );
	}
	BREG_Write32 (
		hReg, BCHP_SCTE_0_CONFIG + ulCoreOffset, ulSctee_controlReg);

	BDBG_LEAVE(BVBI_P_SCTE_Encode_Enable_isr);
	return BERR_SUCCESS;

#else /** } ! BVBI_P_NUM_SCTEE { **/

	BSTD_UNUSED (hReg);
	BSTD_UNUSED (is656);
	BSTD_UNUSED (hwCoreIndex);
	BSTD_UNUSED (eVideoFormat);
	BSTD_UNUSED (scteType);
	BSTD_UNUSED (bEnable);

	return BERR_TRACE (BVBI_ERR_HW_UNSUPPORTED);

#endif /** } BVBI_P_NUM_SCTEE **/
}


/***************************************************************************
* Static (private) functions
***************************************************************************/


/***************************************************************************
 *
 */
#if (BVBI_P_NUM_SCTEE > 0)  /** { **/
static void BVBI_P_ProgramNull (
	BREG_Handle hReg, uint32_t coreOffset,
	uint32_t ulWritePointer, uint32_t value)
{
	uint32_t ulRegAddr;
	uint32_t ulRegVal = BCHP_FIELD_DATA (SCTE_0_CC_NULL_BANK0, VALUE, value);

	switch (ulWritePointer & 0x3)
	{
	case 0:
		ulRegAddr = BCHP_SCTE_0_CC_NULL_BANK0;
		break;
	case 1:
		ulRegAddr = BCHP_SCTE_0_CC_NULL_BANK1;
		break;
	case 2:
		ulRegAddr = BCHP_SCTE_0_CC_NULL_BANK2;
		break;
	case 3:
		ulRegAddr = BCHP_SCTE_0_CC_NULL_BANK3;
		break;
	default:
		/* Should never happen! Programming error! */
		BDBG_ASSERT (false);
		ulRegAddr = BCHP_SCTE_0_CC_NULL_BANK0;
		break;
	}
	ulRegAddr += coreOffset;

	BREG_Write32 (hReg, ulRegAddr, ulRegVal);
}
#endif /** } BVBI_P_NUM_SCTEE **/

#if (BVBI_P_NUM_SCTEE > 0)  /** { **/
/***************************************************************************
 *
 */
static bool BVBI_P_HasComponentOnly (uint8_t hwCoreIndex)
{
	bool retval;

	switch (hwCoreIndex)
	{
	case 0:
#if (BCHP_CHIP == 7400)
		retval = false;
#elif (BCHP_CHIP == 7405)
		retval = false;
#else
		retval = false;
#endif
		break;

#if (BVBI_P_NUM_SCTEE >= 2)
	case 1:
#if (BCHP_CHIP == 7400)
		retval = true;
#else
		retval = false;
#endif
		break;
#endif
#if (BVBI_P_NUM_SCTEE >= 2)
	case BAVC_VbiPath_eVec2:
#if (BCHP_CHIP == 7400)
		retval = false;
#else
		retval = false;
#endif
		break;
#endif
	default:
		/* This should never happen!  This parameter was checked by
		   BVBI_Encode_Create() */
		BDBG_ASSERT(0);
		retval = false;
		break;
	}

	return retval;
}
#endif /** } BVBI_P_NUM_SCTEE **/

#if (BVBI_P_NUM_SCTEE > 0)  /** { **/
/***************************************************************************
 *
 */
static uint32_t P_GetCoreOffset (uint8_t hwCoreIndex)
{
	uint32_t ulCoreOffset = 0xFFFFFFFF;

	switch (hwCoreIndex)
	{
#if (BVBI_P_NUM_SCTEE >= 1)
	case 0:
		ulCoreOffset = 0;
		break;
#endif
#if (BVBI_P_NUM_SCTEE >= 2)
	case 1:
		ulCoreOffset = (BCHP_SCTE_1_REVID - BCHP_SCTE_0_REVID);
		break;
#endif
#if (BVBI_P_NUM_SCTEE >= 3)
	case 2:
		ulCoreOffset = (BCHP_SCTE_2_REVID - BCHP_SCTE_0_REVID);
		break;
#endif
	default:
		break;
	}

	return ulCoreOffset;
}
#endif /** } BVBI_P_NUM_SCTEE **/
