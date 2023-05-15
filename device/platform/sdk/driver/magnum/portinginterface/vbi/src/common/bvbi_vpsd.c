/***************************************************************************
 *     Copyright (c) 2003-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvbi_vpsd.c $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 12/21/09 7:07p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vbi/7400/bvbi_vpsd.c $
 * 
 * Hydra_Software_Devel/2   12/21/09 7:07p darnstein
 * SW7550-120: Add support for SECAM variants.
 * 
 * Hydra_Software_Devel/1   12/3/08 8:03p darnstein
 * PR45819:
 * 
 ***************************************************************************/

#include "bstd.h"           /* standard types */
#include "bdbg.h"           /* Dbglib */
#include "bkni.h"			/* For critical sections */
#include "bvbi.h"           /* VBI processing, this module. */
#include "bvbi_priv.h"      /* VBI internal data structures */
#if defined(BVBI_P_HAS_VPSD)
#include "bchp_vpsd_0.h"
#if (BVBI_P_NUM_VDEC >= 2)
#include "bchp_vpsd_1.h"     
#endif
#endif

BDBG_MODULE(BVBI);


/***************************************************************************
* Forward declarations of static (private) functions
***************************************************************************/


/***************************************************************************
* Implementation of supporting VPS functions that are not in API
***************************************************************************/

#if defined(BVBI_P_HAS_VPSD) /** { **/

/***************************************************************************
 *
 */
void BVBI_P_VPS_Dec_Init (BREG_Handle hReg, uint32_t ulCoreOffset)
{
	uint32_t ulControlReg;

	BDBG_ENTER(BVBI_P_VPS_Dec_Init);

	/* No reset here. */

	BKNI_EnterCriticalSection();

	/* Program the control register (all default values) */
	ulControlReg = (
		BCHP_FIELD_DATA (VPSD_0_CONTROL,    BIPHASE_MIN_01,        1) |
		BCHP_FIELD_DATA (VPSD_0_CONTROL,  STATUS_OVERWRITE,        0) |
		BCHP_FIELD_DATA (VPSD_0_CONTROL,    DATA_OVERWRITE,        0) |
		BCHP_FIELD_DATA (VPSD_0_CONTROL,      CORR_WIN_DIS,        0) |
		BCHP_FIELD_DATA (VPSD_0_CONTROL, RUNIN_DET_WIN_DIS,        0) |
		BCHP_FIELD_DATA (VPSD_0_CONTROL,   VPS_DECODE_TYPE,        0) |
		BCHP_FIELD_ENUM (VPSD_0_CONTROL,            DECODE, DISABLED) );
	BREG_Write32 (hReg, BCHP_VPSD_0_CONTROL + ulCoreOffset, ulControlReg);

	/* Program the first config register */
	ulControlReg = (
		BCHP_FIELD_DATA (VPSD_0_CONFIG_0,    VPS_FIELD,  0) |
		BCHP_FIELD_DATA (VPSD_0_CONFIG_0, VPS_LINE_CNT, 16) );
	BREG_Write32 (hReg, BCHP_VPSD_0_CONFIG_0 + ulCoreOffset, ulControlReg);

	/* Program the second config register */
	ulControlReg = (
		BCHP_FIELD_DATA (VPSD_0_CONFIG_1, CORR_PEAK_START_TIME, 0x76) |
		BCHP_FIELD_DATA (VPSD_0_CONFIG_1,    RUN_IN_START_TIME, 0x10) );
	BREG_Write32 (hReg, BCHP_VPSD_0_CONFIG_1 + ulCoreOffset, ulControlReg);

	/* Program the threshold register */
	ulControlReg = (
		 BCHP_FIELD_DATA (VPSD_0_THRESHOLD,      CORR_THRESHOLD, 0x02c) |
		 BCHP_FIELD_DATA (VPSD_0_THRESHOLD, AVG_LEVEL_THRESHOLD, 0x150) );
	BREG_Write32 (hReg, BCHP_VPSD_0_THRESHOLD + ulCoreOffset, ulControlReg);

	BKNI_LeaveCriticalSection();

	BDBG_LEAVE(BVBI_P_VPS_Dec_Init);
}


/***************************************************************************
 *
 */
BERR_Code BVBI_P_VPS_Dec_Program (
	BREG_Handle hReg,
	BAVC_SourceId eSource,
	bool bActive,
	BFMT_VideoFmt eVideoFormat)
{
/*
	Programming note: the implementation here assumes that the bitfield layout
	within registers is the same for all VPS decoder cores in the chip.  

	If a chip is built that has multiple VPS decoder cores that are not
	identical, then this routine will have to be redesigned.
*/
	uint32_t ulControlReg;
	uint32_t ulStatusReg;
	uint32_t ulOffset;

	BDBG_ENTER(BVBI_P_VPS_Dec_Program);

	/* Complain if video format is not supported */
	switch (eVideoFormat)
	{
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
#if (BVBI_P_NUM_VDEC > 1)
	case BAVC_SourceId_eVdec1:
		ulOffset = BCHP_VPSD_1_CONTROL - BCHP_VPSD_0_CONTROL;
		break;
#endif
	default:
		/* This should never happen!  This parameter was checked by
		   BVBI_Decode_Create() */
		BDBG_LEAVE(BVBI_P_VPS_Dec_Program);
		return BERR_TRACE(BERR_INVALID_PARAMETER);
		break;
	}

	BKNI_EnterCriticalSection();

	/* Program the control register */
	ulControlReg = BREG_Read32 (hReg, BCHP_VPSD_0_CONTROL + ulOffset);
	ulControlReg &= ~BCHP_MASK (VPSD_0_CONTROL, DECODE);
	if (bActive)
	{
		/* Will enable decoding */
		ulControlReg |= BCHP_FIELD_ENUM (VPSD_0_CONTROL, DECODE, ENABLED);

		/* Clear the status register */
		ulStatusReg = (
			BCHP_FIELD_DATA (VPSD_0_STATUS, START_CODE_II_DET, 1) |
			BCHP_FIELD_DATA (VPSD_0_STATUS,     PEAK_CORR_DET, 1) |
			BCHP_FIELD_DATA (VPSD_0_STATUS,    SIGNAL_AVG_DET, 1) |
			BCHP_FIELD_DATA (VPSD_0_STATUS,        RUN_IN_DET, 1) |
			BCHP_FIELD_DATA (VPSD_0_STATUS,      DATA_OVERRUN, 1) |
			BCHP_FIELD_DATA (VPSD_0_STATUS,     BIPHASE_ERROR, 1) |
			BCHP_FIELD_DATA (VPSD_0_STATUS,    VPS_DATA_VALID, 1) );
		BREG_Write32 (hReg, BCHP_VPSD_0_STATUS + ulOffset, ulStatusReg);
	}
	else
	{
		/* Will disable decoding */
		ulControlReg |= (
			BCHP_FIELD_ENUM (VPSD_0_CONTROL, DECODE, DISABLED) );
	}

	/* Finish programming the control register */
	BREG_Write32 (hReg, BCHP_VPSD_0_CONTROL + ulOffset, ulControlReg);

	BKNI_LeaveCriticalSection();

	BDBG_LEAVE(BVBI_P_VPS_Dec_Program);
	return BERR_SUCCESS;
}


/***************************************************************************
 *
 */
uint32_t BVBI_P_VPS_Decode_Data_isr ( 
	BREG_Handle hReg, 
    BAVC_SourceId eSource,
	BAVC_Polarity polarity,
	BVBI_VPSData *pVPSData)
{
/*
	Programming note: the implementation here assumes that the bitfield layout
	within registers is the same for all VPS decoder cores in the chip.  

	If a chip is built that has multiple VPS decoder cores that are not
	identical, then this routine will have to be redesigned.
*/
	uint32_t ulOffset;
	uint32_t ulStatusReg;
	uint8_t ucBiphaseBit;
	uint8_t ucOverrunBit;
	uint8_t ucValidBit;
	uint32_t ulDataReg;
	uint32_t ulErrInfo = 0;

	BDBG_ENTER(BVBI_P_VPS_Decode_Data_isr);

	/* Figure out which decoder core to use */
	switch (eSource)
	{
	case BAVC_SourceId_eVdec0:
		ulOffset = 0x0;
		break;
#if (BVBI_P_NUM_VDEC > 1)
	case BAVC_SourceId_eVdec1:
		ulOffset = BCHP_VPSD_1_CONTROL - BCHP_VPSD_0_CONTROL;
		break;
#endif
	default:
		/* This should never happen!  This parameter was checked by
		   BVBI_Decode_Create() */
		ulOffset = 0x0;
		BDBG_LEAVE(BVBI_P_VPS_Decode_Data_isr);
		return (-1);
		break;
	}

	/* VPS is for top field only */
	if (polarity != BAVC_Polarity_eTopField)
	{
		return BVBI_LINE_ERROR_PARITY_CONFLICT | BVBI_LINE_ERROR_VPS_NODATA;
	}

	/* Verify that field handle data pointer is valid (allocated) */
	if (pVPSData == 0x0)
	{
		ulErrInfo |= 
			(BVBI_LINE_ERROR_FLDH_CONFLICT | BVBI_LINE_ERROR_VPS_NODATA);
		BDBG_LEAVE(BVBI_P_VPS_Decode_Data_isr);
		return ulErrInfo;
	}


	/* Pull status info out of the hardware */
	ulStatusReg = BREG_Read32 ( hReg, BCHP_VPSD_0_STATUS + ulOffset );
	ucBiphaseBit  = 
		(uint8_t)BCHP_GET_FIELD_DATA (
			ulStatusReg, VPSD_0_STATUS,	BIPHASE_ERROR);
	ucOverrunBit = 
		(uint8_t)BCHP_GET_FIELD_DATA (
			ulStatusReg, VPSD_0_STATUS, DATA_OVERRUN);
	ucValidBit = 
		(uint8_t)BCHP_GET_FIELD_DATA (
			ulStatusReg, VPSD_0_STATUS, VPS_DATA_VALID);

	/* Got data? */
	if (ucValidBit == 1)
	{
		/* read the data */
		ulDataReg = BREG_Read32 (hReg, BCHP_VPSD_0_DATA3_0 + ulOffset);
		pVPSData->ucByte05 = (uint8_t)((ulDataReg & 0x00FF0000) >> 16);
		ulDataReg = BREG_Read32 (hReg, BCHP_VPSD_0_DATA11_8 + ulOffset);
		pVPSData->ucByte11 = (uint8_t)(ulDataReg & 0x000000FF);
		ulDataReg >>= 8;
		pVPSData->ucByte12 = (uint8_t)(ulDataReg & 0x000000FF);
		ulDataReg >>= 8;
		pVPSData->ucByte13 = (uint8_t)(ulDataReg & 0x000000FF);
		ulDataReg >>= 8;
		pVPSData->ucByte14 = (uint8_t)(ulDataReg & 0x000000FF);
		ulDataReg = BREG_Read32 (hReg, BCHP_VPSD_0_DATA12 + ulOffset);
		pVPSData->ucByte15 = (uint8_t)(ulDataReg & 0x000000FF);
	}
	else /* No data */
	{
		ulErrInfo |= BVBI_LINE_ERROR_VPS_NODATA;
	}

	/* Report other error conditions */
	if (ucOverrunBit == 1)
	{
		ulErrInfo |= BVBI_LINE_ERROR_VPS_OVERRUN;
	}
	if (ucBiphaseBit == 1)
	{
		ulErrInfo |= BVBI_LINE_ERROR_VPS_BIPHASE;
	}

	/* Clear status bits in hardware */
	BREG_Write32 ( hReg, BCHP_VPSD_0_STATUS + ulOffset, ulStatusReg );

	BDBG_LEAVE(BVBI_P_VPS_Decode_Data_isr);
	return ulErrInfo;
}

#endif /** } BVBI_P_HAS_VPSD **/


/***************************************************************************
* Static (private) functions
***************************************************************************/
