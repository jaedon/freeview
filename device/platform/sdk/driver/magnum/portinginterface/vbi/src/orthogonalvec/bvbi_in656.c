/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvbi_in656.c $
 * $brcm_Revision: Hydra_Software_Devel/7 $
 * $brcm_Date: 8/10/12 3:32p $
 *
 * Module Description:
 *
 * This module provides access to the IN656 core for the VBI porting
 * interface (BVBI).  This module is private to BVBI.
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vbi/7420/bvbi_in656.c $
 * 
 * Hydra_Software_Devel/7   8/10/12 3:32p pntruong
 * SW7435-276: Updated NTSC/480p to 720x480i/720x480p respectively.
 * Added support 482i/483p with new enums.
 *
 * Hydra_Software_Devel/6   2/20/12 2:53p darnstein
 * SW7425-2434: more detail in error messages.
 *
 * Hydra_Software_Devel/5   2/20/12 12:57p darnstein
 * SW7425-2434: when an unsupported video format is entered, the BDBG
 * error message should be informative.
 *
 * Hydra_Software_Devel/4   12/21/09 7:00p darnstein
 * SW7550-120: Add support for SECAM variants.
 *
 * Hydra_Software_Devel/3   8/26/09 1:33p darnstein
 * SW7125-17: Integrate recent improvements from 7405 software.
 *
 * Hydra_Software_Devel/2   12/4/08 6:08p darnstein
 * PR45819: 7420 software will now compile, but not link.
 *
 * Hydra_Software_Devel/1   12/4/08 3:00p darnstein
 * PR45819: copy over from 7400 branch.
 *
 * Hydra_Software_Devel/2   12/3/08 7:56p darnstein
 * PR45819: New, more modular form of most BVBI source files.
 *
 * Hydra_Software_Devel/18   4/2/08 7:55p darnstein
 * PR38956: VBI software compiles now.
 *
 * Hydra_Software_Devel/17   10/31/07 3:51p darnstein
 * PR34528: BVBI is ready for testing on 7325.
 *
 * Hydra_Software_Devel/16   4/20/07 3:36p darnstein
 * PR29723: Compilation for 7405 chipset.
 *
 * Hydra_Software_Devel/15   1/2/07 4:20p darnstein
 * PR26872: Mechanically add SECAM to all cases where PAL formats are
 * accepted.
 *
 * Hydra_Software_Devel/14   8/31/06 2:10p darnstein
 * PR23869: clean up the handling of multiple VECs and VDECs.
 *
 * Hydra_Software_Devel/13   8/18/06 6:51p darnstein
 * PR23178: basic compile on 93563 is possible.
 *
 * Hydra_Software_Devel/12   8/15/06 1:08p darnstein
 * PR23178: Include bchp_in656.h when needed.
 *
 * Hydra_Software_Devel/11   4/25/06 1:21p darnstein
 * PR18010: silence compiler warnings for some chipsets. Reported by
 * DLwin.
 *
 * Hydra_Software_Devel/10   9/23/05 2:47p darnstein
 * PR13750: Proper use of BERR_TRACE and BERR_CODEs.
 *
 * Hydra_Software_Devel/9   9/19/05 2:56p darnstein
 * PR17151: Check for chip name where needed. Also, convert to new scheme
 * for testing chip revisions (BCHP_VER).
 *
 * Hydra_Software_Devel/8   7/7/05 3:34p darnstein
 * PR 16008: The default settings struct for BVBI_Open() now allows the
 * user to choose a buffer size for capturing ancillary data packets in
 * incoming ITU-R 656 digital video.
 *
 * Hydra_Software_Devel/7   7/6/05 5:55p darnstein
 * PR 16008: Input of closed caption data in SAA7113 ancillary data
 * packets of ITU-R 656 digital video has been confirmed. SAA7114 input
 * almost certainly needs some debugging though.
 *
 * Hydra_Software_Devel/6   5/18/05 6:04p agin
 * PR14720: B2, C1, C2 compilation support.
 *
 * Hydra_Software_Devel/5   5/18/05 5:47p darnstein
 * PR 11440: Progress towards ITU-R 656 input of VBI data.
 *
 * Hydra_Software_Devel/4   4/13/05 8:38p darnstein
 * PR 14720: Modify #defines to account for 7038-B2 and 7038-C0 chip
 * revisions.  Compiles OK now.  But untested!
 *
 * Hydra_Software_Devel/3   3/11/05 3:49p darnstein
 * PR 14426: use new _0 names for VDEC cores.
 *
 * Hydra_Software_Devel/2   1/5/05 4:28p jasonh
 * PR 13700: Fixed VBI compile issues for 7038 C0.
 *
 * Hydra_Software_Devel/1   7/21/04 2:40p darnstein
 * PR9080: Finish merging ITU-R 656 software to main branch.
 *
 * I656/2   7/21/04 11:34a darnstein
 * Fix up revision history (Clearcase).
 *
 ***************************************************************************/

#include "bstd.h"			/* standard types */
#include "bdbg.h"			/* Dbglib */
#include "bvbi.h"			/* VBI processing, this module. */
#include "bvbi_priv.h"      /* VBI internal data structures */

#if (BVBI_P_NUM_IN656 >= 1) /** { **/

#include "bchp_in656_0.h"   /* RDB info for IN656 registers */
#if (BVBI_P_NUM_IN656 >= 2)
#include "bchp_in656_1.h"   /* RDB info for IN656 registers */
#endif

BDBG_MODULE(BVBI);


/***************************************************************************
* Forward declarations of static (private) functions
***************************************************************************/
static void BVBI_P_IN656_Dec_Init (BREG_Handle hReg, uint32_t ulCoreOffset);


/***************************************************************************
* Implementation of "BVBI_" API functions
***************************************************************************/


/***************************************************************************
* Implementation of supporting VBI_DEC functions that are not in API
***************************************************************************/


BERR_Code BVBI_P_IN656_Init( BVBI_P_Handle *pVbi )
{
	uint32_t ulCoreOffset;

	BDBG_ENTER(BVBI_P_IN656_Init);

	/* Initialize IN656 core */
	ulCoreOffset = 0x0;
	BVBI_P_IN656_Dec_Init (pVbi->hReg, ulCoreOffset);
#if (BVBI_P_NUM_IN656 >= 2)
	ulCoreOffset = BCHP_IN656_1_REV_ID - BCHP_IN656_0_REV_ID;
	BVBI_P_IN656_Dec_Init (pVbi->hReg, ulCoreOffset);
#endif

	BDBG_LEAVE(BVBI_P_IN656_Init);
	return BERR_SUCCESS;
}


BERR_Code BVBI_P_IN656_Dec_Program (
	BREG_Handle hReg,
	BMEM_Handle hMem,
	BAVC_SourceId eSource,
	bool bActive,
	BVBI_656Fmt anci656Fmt,
	BVBI_P_SMPTE291Moptions* pMoptions,
	BFMT_VideoFmt eVideoFormat,
	uint8_t* topData,
	uint8_t* botData)
{
/*
	Programming note: the implementation here assumes that the bitfield layout
	within registers is the same for all IN656 cores in the chip.

	If a chip is built that has multiple IN656 decoder cores that are not
	identical, then this routine will have to be redesigned.
*/
	uint32_t offset;
	uint32_t ulOffset;
	uint32_t ulReg;
	bool     isPal;
	BERR_Code eErr;

	BDBG_ENTER(BVBI_P_IN656_Dec_Program);

	/* Figure out which decoder core to use */
	switch (eSource)
	{
	case BAVC_SourceId_e656In0:
		ulOffset = 0;
		break;
#if (BVBI_P_NUM_VDEC > 1)
	case BAVC_SourceId_e656In1:
		ulOffset = BCHP_IN656_1_REV_ID - BCHP_IN656_0_REV_ID;
		break;
#endif
	default:
		/* This should never happen!  This parameter was checked by
		   BVBI_Decode_Create() */
		BDBG_LEAVE(BVBI_P_IN656_Dec_Program);
		return BERR_TRACE (BERR_INVALID_PARAMETER);
		break;
	}

	/* Determine whether PAL or NTSC */
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
		BDBG_LEAVE(BVBI_P_IN656_Dec_Program);
		BDBG_ERR(("BVBI_IN656: video format %d not supported", eVideoFormat));
		return BERR_TRACE (BERR_INVALID_PARAMETER);
		break;
	}

	/* Program the encapsulation method */
	ulReg = BREG_Read32 (hReg,  BCHP_IN656_0_STRM_CNTRL + ulOffset);
	ulReg &= ~(
		BCHP_MASK (IN656_0_STRM_CNTRL, SMPTE_COUNT_TYPE) |
		BCHP_MASK (IN656_0_STRM_CNTRL,          TEST) |
		BCHP_MASK (IN656_0_STRM_CNTRL,       MODE_FP) |
		BCHP_MASK (IN656_0_STRM_CNTRL,     VCNT_LAST) |
		BCHP_MASK (IN656_0_STRM_CNTRL,        VBLANK) |
		BCHP_MASK (IN656_0_STRM_CNTRL,          TYPE) |
		BCHP_MASK (IN656_0_STRM_CNTRL, ANCILLARY_WIN) |
		BCHP_MASK (IN656_0_STRM_CNTRL, ANCILLARY_PKT) );
	switch (anci656Fmt)
	{
	case BVBI_656Fmt_SAA7113:
		ulReg |=  BCHP_FIELD_ENUM (IN656_0_STRM_CNTRL, TYPE, SAA7113);
		break;
	case BVBI_656Fmt_SAA7114A:
		ulReg |=  BCHP_FIELD_ENUM (IN656_0_STRM_CNTRL, TYPE, SAA7114A);
		break;
	case BVBI_656Fmt_SAA7114B:
		ulReg |=  BCHP_FIELD_ENUM (IN656_0_STRM_CNTRL, TYPE, SAA7114B);
		break;
	case BVBI_656Fmt_SAA7114C:
		ulReg |=  BCHP_FIELD_ENUM (IN656_0_STRM_CNTRL, TYPE, SAA7114C);
		break;
	case BVBI_656Fmt_SAA7115:
		ulReg |=  BCHP_FIELD_ENUM (IN656_0_STRM_CNTRL, TYPE, SAA7115);
		break;
	case BVBI_656Fmt_SMPTE291:
		ulReg |=  BCHP_FIELD_ENUM (IN656_0_STRM_CNTRL, TYPE, SMPTE291);
    	if (pMoptions->bBrokenDataCount)
		{
			ulReg |=
				BCHP_FIELD_ENUM (IN656_0_STRM_CNTRL, SMPTE_COUNT_TYPE, BYTE);
		}
		else
		{
			ulReg |=
				BCHP_FIELD_ENUM (IN656_0_STRM_CNTRL, SMPTE_COUNT_TYPE, DWORD);
		}
		break;
	default:
		BDBG_LEAVE(BVBI_P_IN656_Dec_Program);
		return BERR_TRACE (BERR_INVALID_PARAMETER);
		break;
	}

	/* Put in a reasonable value for other attributes */
	/* TODO: use enums when RDB and headers are fixed. */
	ulReg |=  BCHP_FIELD_ENUM (IN656_0_STRM_CNTRL, TEST, BUS_0);
	ulReg |=  BCHP_FIELD_ENUM (IN656_0_STRM_CNTRL, MODE_FP, AUTO);
	ulReg |=  BCHP_FIELD_DATA (IN656_0_STRM_CNTRL, VCNT_LAST, 0x22);
	ulReg |=  BCHP_FIELD_DATA (IN656_0_STRM_CNTRL, VBLANK, 0);
	ulReg |=  BCHP_FIELD_ENUM (IN656_0_STRM_CNTRL, DATA_MSB_POLARITY, ONE);
	ulReg |=  BCHP_FIELD_ENUM (IN656_0_STRM_CNTRL, ANCILLARY_WIN, ENABLE);

	/* Turn on or off */
	/* TODO: use enums when RDB and headers are fixed. */
	ulReg &= ~BCHP_MASK (IN656_0_STRM_CNTRL, ANCILLARY_PKT);
	if (bActive)
		ulReg |=  BCHP_FIELD_DATA (IN656_0_STRM_CNTRL, ANCILLARY_PKT, 1);
	else
		ulReg |=  BCHP_FIELD_DATA (IN656_0_STRM_CNTRL, ANCILLARY_PKT, 0);

	/* Done with one register */
    BREG_Write32 (hReg, BCHP_IN656_0_STRM_CNTRL + ulOffset, ulReg);

	/* Program the window for ancillary packet acceptance */
	ulReg = BREG_Read32 (hReg, BCHP_IN656_0_STRM_WIN + ulOffset);
	ulReg &= ~(
		BCHP_MASK      (IN656_0_STRM_WIN, END   ) |
		BCHP_MASK      (IN656_0_STRM_WIN, START ) );
	ulReg |= BCHP_FIELD_DATA (IN656_0_STRM_WIN, START, 0x16A);
	ulReg |= BCHP_FIELD_DATA (IN656_0_STRM_WIN, END, 0x1AD);
	/*
	if (isPal)
	{
		ulReg |= BCHP_FIELD_DATA (IN656_0_STRM_WIN, END, 432);
	}
	*/
    BREG_Write32 (hReg, BCHP_IN656_0_STRM_WIN + ulOffset, ulReg);

	/* Tell the hardware where to put ancillary data packets that it finds */
	eErr = BERR_TRACE (BMEM_ConvertAddressToOffset (hMem, topData, &offset));
	if (eErr != BERR_SUCCESS)
	{
		BDBG_LEAVE(BVBI_P_IN656_Dec_Program);
		return eErr;
	}
	BREG_Write32 (hReg, BCHP_IN656_0_FLD_0_PTR + ulOffset, offset);
	eErr = BERR_TRACE (BMEM_ConvertAddressToOffset (hMem, botData, &offset));
	if (eErr != BERR_SUCCESS)
	{
		BDBG_LEAVE(BVBI_P_IN656_Dec_Program);
		return eErr;
	}
	BREG_Write32 ( hReg, BCHP_IN656_0_FLD_1_PTR + ulOffset, offset);

	BDBG_LEAVE(BVBI_P_IN656_Dec_Program);
	return BERR_SUCCESS;
}

BERR_Code BVBI_P_IN656_Decode_Data_isr (
	BREG_Handle hReg,
    BAVC_SourceId eSource,
	BAVC_Polarity polarity,
	bool* bDataFound)
{
/*
	Programming note: the implementation here assumes that the bitfield layout
	within registers is the same for all IN656 cores in the chip.

	If a chip is built that has multiple IN656 decoder cores that are not
	identical, then this routine will have to be redesigned.
*/
	uint32_t ulOffset;
	uint32_t ulErrors;
	uint32_t ulWriteComplete;
	uint32_t ulWriteCompleteMask;
	uint32_t ulReg;
	BERR_Code eErr;

	BDBG_ENTER(BVBI_P_IN656_Decode_Data_isr);

	/* Figure out which decoder core to use */
	switch (eSource)
	{
	case BAVC_SourceId_e656In0:
		ulOffset = 0;
		break;
#if (BVBI_P_NUM_VDEC > 1)
	case BAVC_SourceId_e656In1:
		ulOffset = BCHP_IN656_1_REV_ID - BCHP_IN656_0_REV_ID;
		break;
#endif
	default:
		/* This should never happen!  This parameter was checked by
		   BVBI_Decode_Create() */
		*bDataFound = false;
		BDBG_LEAVE(BVBI_P_IN656_Decode_Data_isr);
		return BERR_TRACE (BERR_INVALID_PARAMETER);
		break;
	}

	/* Choose top vs bottom field */
	switch (polarity)
	{
	case BAVC_Polarity_eTopField:
		ulWriteCompleteMask = BCHP_MASK (IN656_0_WRITE_COMPLETE, F0_COMPLETE);
		break;
	case BAVC_Polarity_eBotField:
		ulWriteCompleteMask = BCHP_MASK (IN656_0_WRITE_COMPLETE, F1_COMPLETE);
		break;
	default:
		*bDataFound = false;
		BDBG_LEAVE(BVBI_P_IN656_Decode_Data_isr);
		return BERR_TRACE (BERR_INVALID_PARAMETER);
		break;
	}

	/* Check and clear error status */
	ulReg = BREG_Read32 (hReg, BCHP_IN656_0_ERROR_STATUS + ulOffset);
	ulErrors = ulReg & (
		BCHP_MASK (IN656_0_ERROR_STATUS, FIFO_FULL    ) |
		BCHP_MASK (IN656_0_ERROR_STATUS, BAD_AP_PREFIX) |
		BCHP_MASK (IN656_0_ERROR_STATUS, BAD_SAV      ) );
	BREG_Write32 (hReg, BCHP_IN656_0_ERROR_STATUS + ulOffset, ulErrors);

	/* Check and clear capture status */
	ulReg = BREG_Read32 (hReg, BCHP_IN656_0_WRITE_COMPLETE + ulOffset);
	ulWriteComplete = ulReg & ulWriteCompleteMask;
	BREG_Write32 (
		hReg, BCHP_IN656_0_WRITE_COMPLETE + ulOffset, ulWriteComplete);
	*bDataFound =  ((ulErrors == 0) && (ulWriteComplete != 0));

	BDBG_LEAVE(BVBI_P_IN656_Decode_Data_isr);
	eErr =  (ulErrors == 0) ? BERR_SUCCESS : BVBI_ERR_FIELD_BADDATA;

	return eErr;
}

/***************************************************************************
* Static (private) functions
***************************************************************************/

/***************************************************************************
 *
 */
static void BVBI_P_IN656_Dec_Init (BREG_Handle hReg, uint32_t ulCoreOffset)
{
	uint32_t ulReg;

	BDBG_ENTER(BVBI_P_IN656_Dec_Init);

	/* Reset the core */
	ulReg = 0;
#if (BCHP_CHIP == 7405) || (BCHP_CHIP == 7325) || (BCHP_CHIP == 7125)
	/* TODO: hit the reset register when it gets into RDB. */
#else
	/* TODO: apply new reset scheme if necessary. */
    BREG_Write32 (hReg, BCHP_IN656_0_RESET + ulCoreOffset, ulReg);
#endif

	/* Disable the action of the core */
	/* TODO: use enums when RDB and headers are fixed. */
    ulReg = BREG_Read32 (hReg, BCHP_IN656_0_STRM_CNTRL + ulCoreOffset);
	ulReg &= ~BCHP_MASK       (IN656_0_STRM_CNTRL, ANCILLARY_PKT);
	ulReg |=  BCHP_FIELD_DATA (IN656_0_STRM_CNTRL, ANCILLARY_PKT, 0);
    BREG_Write32 (hReg, BCHP_IN656_0_STRM_CNTRL + ulCoreOffset, ulReg);

	BDBG_LEAVE(BVBI_P_IN656_Dec_Init);
}

#endif /** } **/

/* End of file */
