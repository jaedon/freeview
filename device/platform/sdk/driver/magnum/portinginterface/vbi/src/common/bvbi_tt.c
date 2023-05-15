/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvbi_tt.c $
 * $brcm_Revision: Hydra_Software_Devel/8 $
 * $brcm_Date: 8/10/12 3:29p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vbi/7400/bvbi_tt.c $
 * 
 * Hydra_Software_Devel/8   8/10/12 3:29p pntruong
 * SW7435-276: Updated NTSC/480p to 720x480i/720x480p respectively.
 * Added support 482i/483p with new enums.
 *
 * Hydra_Software_Devel/7   8/2/12 1:18p darnstein
 * SW7125-1311: with regards to teletext, handle PAL-M in same way as NTSC
 * and NTSC-J.
 *
 * Hydra_Software_Devel/6   8/1/12 2:47p darnstein
 * SW7425-3623: DRAM writes to DRAM for teletext now access cached memory.
 *
 * Hydra_Software_Devel/5   12/21/09 7:06p darnstein
 * SW7550-120: Add support for SECAM variants.
 *
 * Hydra_Software_Devel/4   6/24/09 5:39p darnstein
 * PR56342: BVBI compiles for 7550 chipset now.
 *
 * Hydra_Software_Devel/3   1/20/09 12:12p darnstein
 * PR51074: For the problem with 15 lines or more teletext (output), apply
 * the same software workaround that was used on 7401-C0. Note that this
 * workaround itself causes problems for some chipsets, such as 3563-C0.
 * So the workaround is controlled by an #ifdef.
 *
 * Hydra_Software_Devel/2   12/3/08 7:57p darnstein
 * PR45819: New, more modular form of most BVBI source files.
 *
 * Hydra_Software_Devel/30   7/30/08 8:39p darnstein
 * PR43926,PR43413: Workaround for TTD addressing error in hardware.
 * Decodes at most one line of teletext per video field. Will not
 * corrrupt memory if input signal actually contains more teletext.
 *
 * Hydra_Software_Devel/29   7/30/08 5:41p darnstein
 * PR43926: A basic solution to the memory addressing problem is in place.
 * It can decode one line of teletext. If more teletext is present, then
 * memory corruption will occur. This danger will be eliminated next.
 *
 * Hydra_Software_Devel/28   4/25/08 2:36p darnstein
 * PR37064: Do not do byte-swapping on line masks. It was wrong.
 *
 * Hydra_Software_Devel/27   4/8/08 6:14p darnstein
 * PR37064: This is the same 15-lines-of-teletext problem that was solved
 * (worked-around, actually) on the dedicated 7401 ClearCase branch.
 *
 * Hydra_Software_Devel/26   1/21/08 3:29p darnstein
 * PR38858: fix up test for insufficient memory (to store teletext data).
 *
 * Hydra_Software_Devel/25   7/26/07 2:43p darnstein
 * PR33057: when allocating space for teletext data, initialize its line
 * mask to "empty" state. This will prevent display of random teletext in
 * Settop API applications.
 *
 * Hydra_Software_Devel/24   1/17/07 5:32p darnstein
 * PR26464: correctly handle teletext output to multiple VECs
 *
 * Hydra_Software_Devel/23   1/2/07 4:21p darnstein
 * PR26872: Mechanically add SECAM to all cases where PAL formats are
 * accepted.
 *
 * Hydra_Software_Devel/22   12/14/06 7:18p darnstein
 * PR25990: Can compile for BCM97400-B0 now.
 *
 * Hydra_Software_Devel/21   8/31/06 2:09p darnstein
 * PR23869: clean up the handling of multiple VECs and VDECs.
 *
 * Hydra_Software_Devel/20   7/24/06 4:45p darnstein
 * PR22720: Implement the software workaround: duplicate the first framing
 * code byte.
 *
 * Hydra_Software_Devel/19   1/4/06 12:57p rjlewis
 * PR18341: fixed typo.
 *
 * Hydra_Software_Devel/18   12/21/05 5:15p darnstein
 * PR18341: Swap bytes of the teletext "line mask" DRAM entry on little-
 * endian systems.
 *
 * Hydra_Software_Devel/17   9/23/05 2:47p darnstein
 * PR13750: Proper use of BERR_TRACE and BERR_CODEs.
 *
 * Hydra_Software_Devel/16   8/22/05 8:11p darnstein
 * PR16057: To support many different chips, use private #defines that
 * specify number of VECs, VDECs, and (separately) pass-through VECs.
 *
 * Hydra_Software_Devel/15   3/17/05 7:35p darnstein
 * PR 14472: Eliminate references to secondary VEC if chip is 3560.
 *
 * Hydra_Software_Devel/I656/1   6/28/04 1:10p darnstein
 * 656 output is ready for testing.
 *
 * Hydra_Software_Devel/13   4/6/04 4:13p darnstein
 * PR 9080: fix a stupid memory leak.
 *
 * Hydra_Software_Devel/12   4/2/04 6:42p darnstein
 * PR 9080: Allow NTSC-J video format.
 *
 * Hydra_Software_Devel/11   3/26/04 1:46p darnstein
 * PR 9080: commented out some debug code.
 *
 * Hydra_Software_Devel/10   3/12/04 5:52p darnstein
 * PR 9080: Teletext for NTSC is working, with a kludge for an A0 hardware
 * flaw.  There is a lot of debug code that will be removed later.
 *
 * Hydra_Software_Devel/9   3/4/04 4:28p darnstein
 * PR 9080: Improve allocation of teletext private data.  Partial
 * implementation of teletext.
 *
 * Hydra_Software_Devel/8   2/6/04 11:49a darnstein
 * PR 9080: better handle VBI decoding errors.
 *
 * Hydra_Software_Devel/7   2/3/04 4:42p darnstein
 * PR 9080: fix logic errors in handling decoding errors. A few other
 * minor fixes.
 *
 * Hydra_Software_Devel/6   10/17/03 6:04p darnstein
 * Define a user-friendly data structure for holding teletext data.
 *
 * Hydra_Software_Devel/5   10/16/03 1:21p darnstein
 * Fix usage of BDBG_ENTER(), BDBG_LEAVE().
 *
 * Hydra_Software_Devel/4   10/9/03 3:57p darnstein
 * Change name of several API functions to indicate that they may be
 * called from an ISR.
 *
 * Hydra_Software_Devel/3   9/25/03 4:48p darnstein
 * BVBI module is mostly complete. The only things that I know are missing
 * are the critical sections, 656 support, macrovision support, and LOTS
 * OF TESTING.
 *
 * Hydra_Software_Devel/2   9/9/03 7:53p darnstein
 * I think all API functions have been implemented, though some with
 * stubs.
 *
 * Hydra_Software_Devel/1   9/9/03 4:00p darnstein
 * Teletext specific code to support VBI processing.
 *
 ***************************************************************************/

#include "bstd.h"                /* standard types */
#include "bdbg.h"                /* Dbglib */
#include "bvbi.h"                /* VBI processing, this module. */
#include "bvbi_priv.h"           /* VBI internal data structures */

BDBG_MODULE(BVBI);


/***************************************************************************
* Forward declarations of static (private) functions
***************************************************************************/


/***************************************************************************
* Implementation of "BVBI_" API functions
***************************************************************************/


/***************************************************************************
 *
 */
BERR_Code BVBI_Field_GetTTData_isr (
	BVBI_Field_Handle fieldHandle,
	int                    nLines,
	BVBI_TT_Line *        ttLines
)
{
	uint8_t *pData;
	uint32_t lineMask;
	int      iLine;
	int      iChar;
	BVBI_P_Field_Handle* pVbi_Fld;
	BVBI_P_TTData* pTTData;
	BERR_Code eErr = BERR_SUCCESS;

	BDBG_ENTER(BVBI_Field_GetTTData_isr);

	/* check parameters */
	BVBI_P_GET_FIELD_CONTEXT(fieldHandle, pVbi_Fld);
	if((!pVbi_Fld) || (!ttLines))
	{
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* Verify that data is present on this field handle */
	if (!(pVbi_Fld->ulWhichPresent & BVBI_P_SELECT_TT))
		return (BVBI_ERR_FIELD_NODATA);
	else if (pVbi_Fld->ulErrInfo & BVBI_LINE_ERROR_TELETEXT_NOENCODE)
		eErr = (BVBI_ERR_FIELD_BADDATA);

    /* Pull out the mask of valid lines */
	pTTData = BVBI_P_LCOP_GET_isr (pVbi_Fld, TTDataO);
	lineMask = pTTData->lineMask;
	lineMask <<= (pTTData->firstLine);

#ifdef BVBI_P_TTE_WA15
	/* Abort if user's array is too small */
	if (pTTData->firstLine >= nLines)
	{
		BDBG_ERR(("User array too small\n"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* Loop over initial unused lines */
	for (iLine = 0 ; iLine < pTTData->firstLine ; ++iLine)
	{
		ttLines->ucFramingCode = BVBI_TT_INVALID_FRAMING_CODE;
		++ttLines;
	}
#endif

	/* Loop over teletext data lines in the field handle */
	pData = pTTData->pucData + 4;
	for (iLine = 0 ; iLine < pTTData->ucLines ; ++iLine)
	{
		/* If data line is valid */
		if ((lineMask >> iLine) & 0x1)
		{
			/* Abort if user's array is too small */
#ifdef BVBI_P_TTE_WA15
			if (pTTData->firstLine + iLine >= nLines)
#else
			if (iLine >= nLines)
#endif
			{
				BDBG_ERR(("User array too small\n"));
				return BERR_TRACE(BERR_INVALID_PARAMETER);
			}

			ttLines->ucFramingCode = *pData++;
			for (iChar = 0 ; iChar < pTTData->ucLineSize - 1 ; ++iChar)
			{
				ttLines->aucData[iChar] = *pData++;
			}
			for ( ; iChar < BVBI_TT_MAX_LINESIZE ; ++iChar)
			{
				ttLines->aucData[iChar] = 0x0;
			}
		}
		else /* data line is invalid */
		{
#ifdef BVBI_P_TTE_WA15
			/* Info for user */
			if ( pTTData->firstLine + iLine < nLines)
			ttLines->ucFramingCode = BVBI_TT_INVALID_FRAMING_CODE;

			/* Skip over wasted space */
			pData += pTTData->ucLineSize;
#else
			ttLines->ucFramingCode = BVBI_TT_INVALID_FRAMING_CODE;
#endif
		}

		/* Next element of user's array */
		++ttLines;
	}

	/* Indicate the rest of the lines in the user's data array is empty */
#ifdef BVBI_P_TTE_WA15
	for ( ; pTTData->firstLine + iLine < nLines ; ++iLine)
#else
	for ( ; iLine < nLines ; ++iLine)
#endif
	{
		ttLines->ucFramingCode = BVBI_TT_INVALID_FRAMING_CODE;
		++ttLines;
	}

	BDBG_LEAVE(BVBI_Field_GetTTData_isr);
	return eErr;
}


/***************************************************************************
 *
 */
BERR_Code BVBI_Field_SetTTData_isr (
	BVBI_Field_Handle fieldHandle,
	BFMT_VideoFmt    eVideoFormat,
	int                    nLines,
	BVBI_TT_Line *        ttLines
)
{
	uint32_t lineMask;
	uint8_t* pData;
	void*    cached_ptr;
	uint8_t  lineWidth;
	int      iLine;
	int      iChar;
	BVBI_P_Field_Handle* pVbi_Fld;
	BVBI_P_TTData* pTTData;
	uint16_t bytes_used;
	BMEM_Handle hMem;
	BERR_Code eErr;
#ifdef BVBI_P_TTXADR_WAROUND
	bool firstLine = true;
#endif

	BDBG_ENTER(BVBI_Field_SetTTData_isr);

	/* check parameters */
	BVBI_P_GET_FIELD_CONTEXT(fieldHandle, pVbi_Fld);
	if((!pVbi_Fld) || (!ttLines))
	{
		BDBG_ERR(("Invalid parameter\n"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	hMem = pVbi_Fld->pVbi->hMem;

	switch (eVideoFormat)
	{
	case BFMT_VideoFmt_eNTSC:
	case BFMT_VideoFmt_eNTSC_J:
	case BFMT_VideoFmt_e720x482_NTSC:
	case BFMT_VideoFmt_e720x482_NTSC_J:
	case BFMT_VideoFmt_ePAL_M:
		lineWidth = 34;
		break;
	case BFMT_VideoFmt_ePAL_B:
	case BFMT_VideoFmt_ePAL_B1:
	case BFMT_VideoFmt_ePAL_D:
	case BFMT_VideoFmt_ePAL_D1:
	case BFMT_VideoFmt_ePAL_G:
	case BFMT_VideoFmt_ePAL_H:
	case BFMT_VideoFmt_ePAL_K:
	case BFMT_VideoFmt_ePAL_I:
	case BFMT_VideoFmt_ePAL_N:
	case BFMT_VideoFmt_ePAL_NC:
	case BFMT_VideoFmt_eSECAM_L:
	case BFMT_VideoFmt_eSECAM_B:
	case BFMT_VideoFmt_eSECAM_G:
	case BFMT_VideoFmt_eSECAM_D:
	case BFMT_VideoFmt_eSECAM_K:
	case BFMT_VideoFmt_eSECAM_H:
		lineWidth = 43;
		break;
	default:
		BDBG_LEAVE(BVBI_Field_SetTTData_isr);
		return BERR_TRACE (BERR_INVALID_PARAMETER);
		break;
	}

	/* Mark TT data in field handle "invalid" until it is complete */
	pVbi_Fld->ulWhichPresent &= ~BVBI_P_SELECT_TT;

	/* Initialize line mask to "empty" */
	lineMask = 0x0;

	/* Initialize pointer into field handle data */
	BVBI_P_LCOP_WRITE_isr (
		pVbi_Fld, TTDataO, &pVbi_Fld->pVbi->ttFreelist, clink);
	pTTData = BVBI_P_LCOP_GET_isr (pVbi_Fld, TTDataO);
	eErr = BERR_TRACE (
		BMEM_ConvertAddressToCached (hMem, pTTData->pucData, &cached_ptr));
	if (eErr != BERR_SUCCESS)
	{
		BDBG_ERR(("Cache memory failure"));
		return eErr;
	}
	pData = (uint8_t*)cached_ptr;
	pData += 4;

	/* Check that the field handle is big enough to start.
	   More checking will follow */
	bytes_used = 4;
	if (pTTData->ucDataSize < bytes_used)
	{
		BDBG_ERR(("Field handle cannot accomodate teletext data\n"));
		return BERR_TRACE(BVBI_ERR_FLDH_CONFLICT);
	}

	/* Loop over lines of user's TT data */
	pTTData->firstLine = 0xFF;
	for (iLine = 0 ; iLine < nLines ; ++iLine)
	{
		/* If there is data on this line in user space */
		if (ttLines->ucFramingCode != BVBI_TT_INVALID_FRAMING_CODE)
		{
			/* Verify that there is enough room in field handle */
			bytes_used += lineWidth;
#ifdef BVBI_P_TTXADR_WAROUND
			if (firstLine)
			{
				++bytes_used;
			}
#endif
			if (bytes_used > pTTData->ucDataSize)
			{
				BDBG_ERR(("Field handle cannot accomodate teletext data\n"));
				return BERR_TRACE(BVBI_ERR_FLDH_CONFLICT);
			}

			if (pTTData->firstLine == 0xFF)
				pTTData->firstLine = iLine;

			/* Copy one line of data */
			*pData++ = ttLines->ucFramingCode;
#ifdef BVBI_P_TTXADR_WAROUND
			if (firstLine)
			{
				/* Write framing code again! */
				*pData++ = ttLines->ucFramingCode;
				firstLine = false;
			}
#endif
			for (iChar = 0 ; iChar < lineWidth - 1 ; ++iChar)
			{
				*pData++ = ttLines->aucData[iChar];
			}

			/* Update line mask */
			lineMask |= (0x1 << iLine);

			/* Update line count in private data */
			pTTData->ucLines = (uint8_t)(iLine + 1);
		}
#ifdef BVBI_P_TTE_WA15
		else /* No data on this line in user space */
		{
			/* Create wasted space in the field handle! */
			if (pTTData->firstLine != 0xFF)
			{
				bytes_used += lineWidth;
				pData += lineWidth;
			}

		} /* If there is data on this line in user space */
#endif

		/* Go to next line of user's data */
		++ttLines;
	}

	/* Write the line mask and line width to the start of the data */
	if (pTTData->firstLine == 0xFF)
#ifdef BVBI_P_TTE_WA15
		pTTData->firstLine = 0;
#else
		lineMask = 0x0;
#endif
	else
		lineMask = lineMask >> pTTData->firstLine;
	pTTData->lineMask = lineMask;
	pData = (uint8_t*)cached_ptr;
	*(uint32_t*)(pData) = 0xFFFFFFFF;
	BMEM_FlushCache (hMem, cached_ptr, bytes_used);
	pTTData->ucLineSize = lineWidth;

	/* Indicate valid data is present */
	pVbi_Fld->ulWhichPresent |= BVBI_P_SELECT_TT;

	BDBG_LEAVE(BVBI_Field_SetTTData_isr);
	return BERR_SUCCESS;
}


/***************************************************************************
* Implementation of supporting teletext functions that are not in API
***************************************************************************/


BERR_Code BVBI_P_TT_Init( BVBI_P_Handle *pVbi )
{
#if (BVBI_P_NUM_TTE == 0) && (BVBI_P_NUM_TTE_656 == 0) /** { **/
	BSTD_UNUSED (pVbi);
#else /** } { **/
	uint8_t hwIndex;
#if (BVBI_P_NUM_VDEC >= 1)
	uint32_t ulCoreOffset;
#endif

	BDBG_ENTER(BVBI_P_TT_Init);

	/* Initialize TT decoder */
#if (BVBI_P_NUM_VDEC >= 1)
	ulCoreOffset = 0x0;
	BVBI_P_TT_Dec_Init (pVbi->hReg, ulCoreOffset);
#endif
#if (BVBI_P_NUM_VDEC >= 2)
	ulCoreOffset = BCHP_TTD_1_reset - BCHP_TTD_0_reset;
	BVBI_P_TT_Dec_Init (pVbi->hReg, ulCoreOffset);
#endif

	/* Initialize TT encoders */
#if (BVBI_P_NUM_TTE > 0)
	for (hwIndex = 0 ; hwIndex < BVBI_P_NUM_TTE ; ++hwIndex)
		BVBI_P_TT_Enc_Init (pVbi->hReg, false, hwIndex);
#endif
#if (BVBI_P_NUM_TTE_656 > 0)
	for (hwIndex = 0 ; hwIndex < BVBI_P_NUM_TTE_656 ; ++hwIndex)
		BVBI_P_TT_Enc_Init (pVbi->hReg, true, hwIndex);
#endif

	BDBG_LEAVE(BVBI_P_TT_Init);
#endif /** } **/
	return BERR_SUCCESS;
}


uint32_t BVBI_P_TT_Size_Storage(uint32_t ulMaxLines, uint32_t ulMaxLineSize)
/*
	Sizes teletext data "slab."  Includes space for framing code.  Does not
	include space for alignment.
*/
{
	return 4  +  ulMaxLines * ulMaxLineSize;
}

/***************************************************************************
 *
 */
BERR_Code BVBI_P_TTData_Alloc (
	BMEM_Handle hMem, uint8_t ucMaxLines, uint8_t ucLineSize,
	BVBI_P_TTData* pTTData)
{
	/* Sanity check */
	if (!hMem || !pTTData)
	{
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* Deallocate data if necessary */
	if (pTTData->pucData != 0)
	{
		BMEM_Free (hMem, pTTData->pucData);
		pTTData->ucDataSize =   0;
		pTTData->ucLines    =   0;
		pTTData->ucLineSize =   0;
		pTTData->pucData    = 0x0;
	}

	/* If user wants to hold any teletext data */
	if (ucMaxLines != 0)
	{
		uint32_t dataSize = BVBI_P_TT_Size_Storage (ucMaxLines, ucLineSize);
#ifdef BVBI_P_TTXADR_WAROUND
		++dataSize;
#endif
		pTTData->pucData =
			(uint8_t*)(BMEM_AllocAligned (
				hMem,
				dataSize,
				8,
				0));
		if (!pTTData->pucData)
		{
			return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
		}

		/* Zero out the line mask */
		*(uint32_t*)(pTTData->pucData) = 0x0;

		/* Complete the self-description of the teletext data */
		pTTData->ucDataSize =   dataSize;
		pTTData->ucLines    =          0;
		pTTData->ucLineSize =          0;

		/* Debug code */
		/*
		{
		uint8_t* start = pTTData->pucData;
		uint8_t* end = start + dataSize;
		while (start < end)
		{
			*(uint32_t*)start = 0xDEADBEEF;
			start += sizeof(uint32_t);
		}
		}
		*/
	}

	return BERR_SUCCESS;
}

/***************************************************************************
* Static (private) functions
***************************************************************************/


/* End of file */
