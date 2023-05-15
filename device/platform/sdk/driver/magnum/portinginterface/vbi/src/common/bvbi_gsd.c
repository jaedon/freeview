/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvbi_gsd.c $
 * $brcm_Revision: Hydra_Software_Devel/3 $
 * $brcm_Date: 8/10/12 3:30p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vbi/7400/bvbi_gsd.c $
 * 
 * Hydra_Software_Devel/3   8/10/12 3:30p pntruong
 * SW7435-276: Updated NTSC/480p to 720x480i/720x480p respectively.
 * Added support 482i/483p with new enums.
 *
 * Hydra_Software_Devel/2   11/18/09 3:54p darnstein
 * SW7468-24: Gemstar options now placed in dedicated data structure.
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


BDBG_MODULE(BVBI);


/***************************************************************************
* Forward declarations of static (private) functions
***************************************************************************/

#ifdef BVBI_P_HAS_GSD
static bool P_IsOddParity (uint32_t input);
#endif


/***************************************************************************
* Implementation of supporting GS functions that are not in API
***************************************************************************/

#ifdef BVBI_P_HAS_GSD /** { **/

void BVBI_P_GS_Dec_Init (BREG_Handle hReg, uint32_t ulCoreOffset)
{
	uint32_t ulResetReg;

	BDBG_ENTER(BVBI_P_GS_Dec_Init);


	/* Start by resetting the whole core */
	ulResetReg = 0x1;
	/* This BVBI_P_GS_Dec_Init can not close enable gemstar decoder now.
	Because Gemstar decoder always enable. see pr:
	M_BME3560B/20600: Gemstar Decoder: Gemstar decoder always enabled
	*/
	BKNI_EnterCriticalSection();
	BREG_Write32 (hReg, BCHP_GSD_0_RESET + ulCoreOffset, ulResetReg);
	BKNI_LeaveCriticalSection();
	BDBG_LEAVE(BVBI_P_GS_Dec_Init);
}

BERR_Code BVBI_P_GS_Dec_Program (
	BREG_Handle hReg,
	BAVC_SourceId eSource,
	bool bActive,
	BFMT_VideoFmt eVideoFormat,
	BVBI_GSOptions* gsOptions)
{
	uint32_t ulControlReg;
	uint32_t ulTopControlReg;
	uint32_t ulBottomControlReg;
	uint32_t ulTopStatusReg;
	uint32_t ulBottomStatusReg;
	uint32_t ulOffset;

	BDBG_ENTER(BVBI_P_GS_Dec_Program);

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
			return BERR_TRACE (BVBI_ERR_VFMT_CONFLICT);
	}

	/* Figure out which decoder core to use */
	switch (eSource)
	{
	case BAVC_SourceId_eVdec0:
		ulOffset = 0;
		break;
	default:
		/* This should never happen!  This parameter was checked by
		   BVBI_Decode_Create() */
		BDBG_LEAVE(BVBI_P_GS_Dec_Program);
		return BERR_TRACE(BERR_INVALID_PARAMETER);
		break;
	}
	/* If user wants to turn off Gemstar, just reset the entire core. */
	if (!bActive)
	{
		/* This BVBI_P_GS_Dec_Init can not close enable gemstar decoder now.
		Because Gemstar decoder always enable. see pr:
		M_BME3560B/20600: Gemstar Decoder: Gemstar decoder always enabled
		*/
		BVBI_P_GS_Dec_Init (hReg, ulOffset);
		BDBG_LEAVE(BVBI_P_GS_Dec_Program);
		return BERR_SUCCESS;
	}
	BKNI_EnterCriticalSection();
	/* get and set fields in the GSD general control register */
	ulControlReg = BREG_Read32(hReg, BCHP_GSD_0_CONTROL + ulOffset );
	ulControlReg &= ~(
		BCHP_MASK      (GSD_0_CONTROL, DELAY) |
		BCHP_MASK      (GSD_0_CONTROL, CORREL_THRESH) |
		BCHP_MASK      (GSD_0_CONTROL, CORREL_TIME_OUT) |
		BCHP_MASK      (GSD_0_CONTROL, PARITY) |
		BCHP_MASK      (GSD_0_CONTROL, BYTE_ORDER) );
	ulControlReg |= (
		BCHP_FIELD_DATA(GSD_0_CONTROL, DELAY, 0x63) |
		BCHP_FIELD_DATA(GSD_0_CONTROL, CORREL_THRESH, 0x35) |
		BCHP_FIELD_DATA(GSD_0_CONTROL, CORREL_TIME_OUT, 0x80) |
		BCHP_FIELD_ENUM(GSD_0_CONTROL, PARITY, ODD) |
		BCHP_FIELD_ENUM(GSD_0_CONTROL, BYTE_ORDER, LOW_BYTE_FIRST) );
	/* get and set fields in the GSD top control register */
	ulTopControlReg = BREG_Read32(hReg, BCHP_GSD_0_FIELD0_CONTROL + ulOffset );
	ulTopControlReg &= ~(
		BCHP_MASK      ( GSD_0_FIELD0_CONTROL, STORE) |
		BCHP_MASK      ( GSD_0_FIELD0_CONTROL, START_LINE) |
		BCHP_MASK      ( GSD_0_FIELD0_CONTROL, SIGNAL_THRESHOLD) |
		BCHP_MASK      ( GSD_0_FIELD0_CONTROL, ENABLE) );
	ulTopControlReg |= (
		BCHP_FIELD_DATA( GSD_0_FIELD0_CONTROL, STORE,
			                                   gsOptions->linemask_top) |
		BCHP_FIELD_DATA( GSD_0_FIELD0_CONTROL, START_LINE,
			                                   gsOptions->baseline_top) |
		BCHP_FIELD_DATA( GSD_0_FIELD0_CONTROL, SIGNAL_THRESHOLD, 0x154) |
		BCHP_FIELD_ENUM( GSD_0_FIELD0_CONTROL, ENABLE, ON) );
	/* get and set fields in the GSD bottom control register */
	ulBottomControlReg =
		BREG_Read32(hReg, BCHP_GSD_0_FIELD1_CONTROL + ulOffset );
	ulBottomControlReg &= ~(
		BCHP_MASK      ( GSD_0_FIELD1_CONTROL, STORE) |
		BCHP_MASK      ( GSD_0_FIELD1_CONTROL, START_LINE) |
		BCHP_MASK      ( GSD_0_FIELD1_CONTROL, SIGNAL_THRESHOLD) |
		BCHP_MASK      ( GSD_0_FIELD1_CONTROL, ENABLE) );
	ulBottomControlReg |= (
		BCHP_FIELD_DATA( GSD_0_FIELD1_CONTROL, STORE,
			                           gsOptions->linemask_bot) |
		BCHP_FIELD_DATA( GSD_0_FIELD1_CONTROL, START_LINE,
			                         gsOptions->baseline_bot - 256 - 7) |
		BCHP_FIELD_DATA( GSD_0_FIELD1_CONTROL, SIGNAL_THRESHOLD, 0x154) |
		BCHP_FIELD_ENUM( GSD_0_FIELD1_CONTROL, ENABLE, ON) );
	/* write the three registers with updated values */
   	BREG_Write32 ( hReg, BCHP_GSD_0_CONTROL + ulOffset, ulControlReg );
	BREG_Write32 (
		hReg, BCHP_GSD_0_FIELD0_CONTROL + ulOffset,     ulTopControlReg  );
	BREG_Write32 (
		hReg, BCHP_GSD_0_FIELD1_CONTROL + ulOffset,  ulBottomControlReg  );
	/* Clear the top field status bits */
	ulTopStatusReg =
	BREG_Read32 ( hReg, BCHP_GSD_0_FIELD0_STATUS + ulOffset );
	BREG_Write32 (
		hReg, BCHP_GSD_0_FIELD0_STATUS + ulOffset, ulTopStatusReg );
	/* Clear the bottom field status bits */
	ulBottomStatusReg =
		BREG_Read32 ( hReg, BCHP_GSD_0_FIELD1_STATUS + ulOffset );
	BREG_Write32 (
		hReg, BCHP_GSD_0_FIELD1_STATUS + ulOffset, ulBottomStatusReg );
	BKNI_LeaveCriticalSection();
	BDBG_LEAVE(BVBI_P_GS_Dec_Program);
	return BERR_SUCCESS;
}

uint32_t BVBI_P_GS_Decode_Data_isr (
					BREG_Handle hReg,
					BAVC_SourceId eSource,
					BFMT_VideoFmt eVideoFormat,
					BAVC_Polarity polarity,
					BVBI_GSData* pGSData)
{
/*
	Programming note: the implementation here assumes that the bitfield layout
	within registers is the same for all Gemstar decoder cores in the chip.

	If a chip is built that has multiple Gemstar decoder cores that are not
	identical, then this routine will have to be redesigned.
*/
	uint32_t ulOffset;
	uint32_t ulFieldStatusReg;
	uint8_t  ucFieldParityErrorFlag;
	uint16_t usRawFieldDataDetectedFlag;
	uint16_t usFieldDataDetectedFlag;
	uint8_t  ucFieldDataOverRunFlag;
	uint32_t ulFieldControlReg;
	uint16_t usFieldStore;
	uint8_t  ucFieldStartLine;
	uint8_t  i;
	uint8_t  j;
	uint32_t ulErrInfo = 0;
	uint32_t ulRegAddr = 0;


	BDBG_ENTER(BVBI_P_GS_Decode_Data_isr);

	/* Figure out which decoder core to use */
	switch (eSource)
	{
	case BAVC_SourceId_eVdec0:
		ulOffset = 0;
		break;
	default:
		/* This should never happen!  This parameter was checked by
		   BVBI_Decode_Create() */
		BDBG_LEAVE(BVBI_P_GS_Decode_data_isr);
		return BERR_TRACE(BERR_INVALID_PARAMETER);
		break;
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
		return BERR_TRACE (BVBI_ERR_VFMT_CONFLICT);
	}
	/* If top field */
	if (polarity == BAVC_Polarity_eTopField)
	{
		/* Retrieve line mask programming */
		ulFieldControlReg =
			BREG_Read32 (hReg, BCHP_GSD_0_FIELD0_CONTROL + ulOffset);
		usFieldStore = BCHP_GET_FIELD_DATA (
			ulFieldControlReg, GSD_0_FIELD0_CONTROL, STORE);
		ucFieldStartLine = BCHP_GET_FIELD_DATA (
			ulFieldControlReg, GSD_0_FIELD0_CONTROL, START_LINE);

		/* Pull Top field status info out of the hardware */
		ulFieldStatusReg =
			BREG_Read32 (hReg, BCHP_GSD_0_FIELD0_STATUS + ulOffset);
		ucFieldDataOverRunFlag =
			BCHP_GET_FIELD_DATA(
				ulFieldStatusReg, GSD_0_FIELD0_STATUS, DATA_OVERRUN);
		usRawFieldDataDetectedFlag =
			BCHP_GET_FIELD_DATA (
				ulFieldStatusReg, GSD_0_FIELD0_STATUS, DATA_DETECTED);
		ucFieldParityErrorFlag =
			BCHP_GET_FIELD_DATA (
				ulFieldStatusReg, GSD_0_FIELD0_STATUS, PARITY_ERROR);

		/* Fill in user data with above info */
		usFieldDataDetectedFlag = usRawFieldDataDetectedFlag & usFieldStore;
		pGSData->ulDataLines =
			(uint32_t)usFieldDataDetectedFlag << ucFieldStartLine;
		ulRegAddr = BCHP_GSD_0_FIELD0_DATA0 + ulOffset;
		for (i = 0; i < 5; ++i)
		{
			pGSData->ulData[i] = BREG_Read32(hReg, ulRegAddr);
			ulRegAddr += 4;
		}

		/* Clear the top field status bits */
		BREG_Write32 (
			hReg, BCHP_GSD_0_FIELD0_STATUS + ulOffset , ulFieldStatusReg );
	}
	else /* bottom field */
	{
		/* Retrieve line mask programming */
		ulFieldControlReg =
			BREG_Read32 (hReg, BCHP_GSD_0_FIELD1_CONTROL + ulOffset);
		usFieldStore = BCHP_GET_FIELD_DATA (
			ulFieldControlReg, GSD_0_FIELD1_CONTROL, STORE);
		ucFieldStartLine = BCHP_GET_FIELD_DATA (
			ulFieldControlReg, GSD_0_FIELD1_CONTROL, START_LINE);

		/* Pull Bottom field status info out of the hardware */
		ulFieldStatusReg =
			BREG_Read32( hReg, BCHP_GSD_0_FIELD1_STATUS + ulOffset);
		ucFieldDataOverRunFlag =
			BCHP_GET_FIELD_DATA(
				ulFieldStatusReg, GSD_0_FIELD1_STATUS, DATA_OVERRUN);
		usRawFieldDataDetectedFlag =
			BCHP_GET_FIELD_DATA (
				ulFieldStatusReg, GSD_0_FIELD1_STATUS, DATA_DETECTED);
		ucFieldParityErrorFlag =
			BCHP_GET_FIELD_DATA (
				ulFieldStatusReg, GSD_0_FIELD0_STATUS, PARITY_ERROR);

		/* Fill in user data with above info */
		usFieldDataDetectedFlag = usRawFieldDataDetectedFlag & usFieldStore;
		pGSData->ulDataLines =
			(uint32_t)usFieldDataDetectedFlag << ucFieldStartLine;
		ulRegAddr = BCHP_GSD_0_FIELD1_DATA0 + ulOffset;
		for (i = 0; i < 5; ++i)
		{
			pGSData->ulData[i] = BREG_Read32(hReg, ulRegAddr);
			ulRegAddr += 4;
		}

		/* Clear the Bottom field status bits */
		BREG_Write32 (
			hReg, BCHP_GSD_0_FIELD1_STATUS + ulOffset, ulFieldStatusReg );
	}

	/* Work around a hardware flaw. */
	/* Compute parity "by hand." */
	j = 0;
	pGSData->ulErrorLines = 0x0;
	for (i = 0 ; i < 32 ; ++i)
	{
		if (pGSData->ulDataLines & (1 << i))
		{
			if (!P_IsOddParity (pGSData->ulData[j]))
			{
				pGSData->ulErrorLines |= (1 << j);
			}
			++j;
		}
	}

	/* Report "no data" condition as an error */
	if (pGSData->ulDataLines == 0)
	{
		ulErrInfo |= BVBI_LINE_ERROR_GEMSTAR_NODATA;
	}

	/* Report other error conditions */
	if ( pGSData->ulErrorLines != 0x0)
	{
		ulErrInfo |= BVBI_LINE_ERROR_GEMSTAR_PARITY;
	}
	if ( ucFieldDataOverRunFlag ==
		BCHP_GSD_0_FIELD0_STATUS_DATA_OVERRUN_ERROR)
	{
		ulErrInfo |= BVBI_LINE_ERROR_GEMSTAR_OVERRUN;
	}

	BDBG_LEAVE(BVBI_P_GS_Decode_Data_isr);
	return ulErrInfo;
}
#endif /** } **/


/***************************************************************************
* Static (private) functions
***************************************************************************/

#ifdef BVBI_P_HAS_GSD /** { **/

static bool P_IsOddParity (uint32_t input)
{
	static const uint8_t parity[256] = {
		0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
		1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
		1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
		0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
		1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
		0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
		0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
		1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
		1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
		0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
		0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
		1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
		0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
		1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
		1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
		0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0
	};

	bool
	retval =           (parity[input & 0xFF] != 0);
	input >>= 8;
	retval = retval && (parity[input & 0xFF] != 0);
	input >>= 8;
	retval = retval && (parity[input & 0xFF] != 0);
	input >>= 8;
	retval = retval && (parity[input & 0xFF] != 0);

	return retval;
}

#endif /**  } BVBI_P_HAS_GSD **/
