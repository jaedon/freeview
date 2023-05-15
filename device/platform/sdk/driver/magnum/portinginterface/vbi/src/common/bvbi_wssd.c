/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvbi_wssd.c $
 * $brcm_Revision: Hydra_Software_Devel/3 $
 * $brcm_Date: 8/10/12 3:30p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vbi/7400/bvbi_wssd.c $
 * 
 * Hydra_Software_Devel/3   8/10/12 3:30p pntruong
 * SW7435-276: Updated NTSC/480p to 720x480i/720x480p respectively.
 * Added support 482i/483p with new enums.
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
#include "bvbi.h"           /* VBI processing, this module. */
#include "bkni.h"			/* For critical sections */
#include "bvbi_priv.h"      /* VBI internal data structures */
#if (BVBI_P_NUM_VDEC >= 1)
#include "bchp_wssd_0.h"    /* RDB info for WSSD core */
#endif
#if (BVBI_P_NUM_VDEC >= 2)
#include "bchp_wssd_1.h"    /* RDB info for WSSD core */
#endif

BDBG_MODULE(BVBI);


/***************************************************************************
* Forward declarations of static (private) functions
***************************************************************************/

#define BCHP_WSSD_0_Status_DATA_OVERRUN_WSSD_CLEAR_STATUS 0x1
#define BCHP_WSSD_0_Status_DATA_LOCK_WSSD_CLEAR_STATUS    0x1
#define BCHP_WSSD_0_Status_PARITY_ERROR_WSSD_CLEAR_STATUS 0x1
#define BCHP_WSSD_0_Status_DIFFERENCE_WSSD_CLEAR_STATUS   0x1


/***************************************************************************
* Implementation of supporting WSS functions that are not in API
***************************************************************************/

#if (BVBI_P_NUM_VDEC >= 1) /** { **/

/***************************************************************************
 *
 */
void BVBI_P_WSS_Dec_Init (BREG_Handle hReg, uint32_t ulCoreOffset)
{
	uint32_t ulResetReg;
	uint32_t ulControlReg;
	uint32_t ulStatusReg;
	uint32_t ulThresholdReg;
	uint32_t ulThresholdVal;
	uint32_t ulSyncLevelVal;

	BDBG_ENTER(BVBI_P_WSS_Dec_Init);

#if (BCHP_CHIP == 3548)
	/* Brad Grossman
	ulThresholdVal = 0x230;
	ulSyncLevelVal = 0x79;
	*/
	/* David Arnstein */
	ulThresholdVal = 0x180;
	ulSyncLevelVal = 0x7D;
#elif (BCHP_CHIP == 3556) && (BCHP_VER == BCHP_VER_A0)
	/* Same as above case */
	ulThresholdVal = 0x180;
	ulSyncLevelVal = 0x7D;
#elif (BCHP_CHIP == 3556) && (BCHP_VER >= BCHP_VER_B0)
	/* This may need tuning */
	ulThresholdVal = 0x180;
	ulSyncLevelVal = 0x7B;
#else
	ulThresholdVal = 0x150;
	ulSyncLevelVal = 0x7e;
#endif

	BKNI_EnterCriticalSection();

	/* Start by resetting the whole core */
	ulResetReg = 0x0;
	BREG_Write32 (hReg, BCHP_WSSD_0_Reset + ulCoreOffset, ulResetReg);

	/* Program the control register */
	ulControlReg = BREG_Read32 (hReg, BCHP_WSSD_0_Control + ulCoreOffset);
	ulControlReg &= ~(
		BCHP_MASK       (WSSD_0_Control, DISABLE_WINDOW               ) |
		BCHP_MASK       (WSSD_0_Control, SYNC_LEVEL                   ) |
		BCHP_MASK       (WSSD_0_Control, WSS_FIELD                    ) |
		BCHP_MASK       (WSSD_0_Control, LINE_OFFSET                  ) |
		BCHP_MASK       (WSSD_0_Control, HSYNC_DELAY                  ) |
		BCHP_MASK       (WSSD_0_Control, ENABLE_WSS_DECODE            ) );
	ulControlReg |= (
		BCHP_FIELD_ENUM (WSSD_0_Control, DISABLE_WINDOW, ENABLE_WINDOW) |
		BCHP_FIELD_DATA (WSSD_0_Control, SYNC_LEVEL,    ulSyncLevelVal) |
		BCHP_FIELD_DATA (WSSD_0_Control, WSS_FIELD,                  0) |
		BCHP_FIELD_DATA (WSSD_0_Control, LINE_OFFSET,             0x17) |
		BCHP_FIELD_DATA (WSSD_0_Control, HSYNC_DELAY,            0x040) |
		BCHP_FIELD_ENUM (WSSD_0_Control, ENABLE_WSS_DECODE,   DISABLED) );
	BREG_Write32 (hReg, BCHP_WSSD_0_Control + ulCoreOffset, ulControlReg);

	/* Clear the status register */
	ulStatusReg = 0x0;
	ulStatusReg |= (
		BCHP_FIELD_ENUM (WSSD_0_Status, DATA_OVERRUN, WSSD_CLEAR_STATUS) |
		BCHP_FIELD_ENUM (WSSD_0_Status, DATA_LOCK,    WSSD_CLEAR_STATUS) |
		BCHP_FIELD_ENUM (WSSD_0_Status, PARITY_ERROR, WSSD_CLEAR_STATUS) |
		BCHP_FIELD_ENUM (WSSD_0_Status, DIFFERENCE,   WSSD_CLEAR_STATUS) );
	BREG_Write32 (hReg, BCHP_WSSD_0_Status + ulCoreOffset, ulStatusReg);

	/* Program threshold values */
	ulThresholdReg = BREG_Read32 (hReg, BCHP_WSSD_0_Threshold + ulCoreOffset);
	ulThresholdReg &= ~(
		BCHP_MASK       (WSSD_0_Threshold, ADAPT_THRESHOLD       ) |
		BCHP_MASK       (WSSD_0_Threshold, THRESHOLD             ) );
	ulThresholdReg |= (
#ifdef BVBI_P_WSSD_VER2
		BCHP_FIELD_ENUM (WSSD_0_Threshold, ADAPT_THRESHOLD,    ADAPT) |
#else
		BCHP_FIELD_ENUM (WSSD_0_Threshold, ADAPT_THRESHOLD,     HOLD) |
#endif
		BCHP_FIELD_DATA (WSSD_0_Threshold, THRESHOLD, ulThresholdVal) );
	BREG_Write32 (hReg, BCHP_WSSD_0_Threshold + ulCoreOffset, ulThresholdReg);

	BKNI_LeaveCriticalSection();

	BDBG_LEAVE(BVBI_P_WSS_Dec_Init);
}

/***************************************************************************
 *
 */
BERR_Code BVBI_P_WSS_Dec_Program (
	BREG_Handle hReg,
	BAVC_SourceId eSource,
	bool bActive,
	BFMT_VideoFmt eVideoFormat,
	uint16_t wssVline576i)
{
	uint32_t ulControlReg;
	uint32_t ulStatusReg;
	uint32_t ulOffset;
	uint32_t line_offset;

	BDBG_ENTER(BVBI_P_WSS_Dec_Program);

	/* Complain if video format is not supported */
	switch (eVideoFormat)
	{
	/* NTSC case is just for testing */
	case BFMT_VideoFmt_eNTSC:
	case BFMT_VideoFmt_eNTSC_J:
	case BFMT_VideoFmt_e720x482_NTSC:
	case BFMT_VideoFmt_e720x482_NTSC_J:
		line_offset = 0;
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
#if (BVDC_P_3DCOMB_TTD_CONFLICT)
		line_offset = wssVline576i - 1;
#else
		line_offset = wssVline576i;
#endif
		break;

    case BFMT_VideoFmt_ePAL_M:
		/* TODO: verify this! */
#if (BVDC_P_3DCOMB_TTD_CONFLICT)
		line_offset = wssVline576i - 1;
#else
		line_offset = wssVline576i;
#endif
		break;

	default:
		line_offset = 0;
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
		ulOffset = BCHP_WSSD_1_Reset - BCHP_WSSD_0_Reset;
		break;
#endif
	default:
		/* This should never happen!  This parameter was checked by
		   BVBI_Decode_Create() */
		BDBG_LEAVE(BVBI_P_WSS_Dec_Program);
		return BERR_TRACE(BERR_INVALID_PARAMETER);
		break;
	}

	BKNI_EnterCriticalSection();

	/* Program the control register */
	ulControlReg = BREG_Read32 (hReg, BCHP_WSSD_0_Control + ulOffset);
	ulControlReg &= ~(
		BCHP_MASK       (WSSD_0_Control, LINE_OFFSET                  ) |
		BCHP_MASK       (WSSD_0_Control, ENABLE_WSS_DECODE            ) );
	if (bActive)
	{
		/* Will enable decoding */
		ulControlReg |= (
			BCHP_FIELD_DATA (WSSD_0_Control, LINE_OFFSET,      line_offset) |
			BCHP_FIELD_ENUM (WSSD_0_Control, ENABLE_WSS_DECODE,    ENABLED) );

		/* Clear the status register */
		ulStatusReg = 0x0;
		ulStatusReg |= (
			BCHP_FIELD_ENUM (WSSD_0_Status, DATA_OVERRUN, WSSD_CLEAR_STATUS) |
			BCHP_FIELD_ENUM (WSSD_0_Status, DATA_LOCK,    WSSD_CLEAR_STATUS) |
			BCHP_FIELD_ENUM (WSSD_0_Status, PARITY_ERROR, WSSD_CLEAR_STATUS) |
			BCHP_FIELD_ENUM (WSSD_0_Status, DIFFERENCE,   WSSD_CLEAR_STATUS) );
		BREG_Write32 (hReg, BCHP_WSSD_0_Status + ulOffset, ulStatusReg);
	}
	else
	{
		/* Will disable decoding */
		ulControlReg |= (
			BCHP_FIELD_ENUM (WSSD_0_Control, ENABLE_WSS_DECODE,   DISABLED) );
	}

	/* Finish programming the control register */
	BREG_Write32 (hReg, BCHP_WSSD_0_Control + ulOffset, ulControlReg);

	BKNI_LeaveCriticalSection();

	BDBG_LEAVE(BVBI_P_WSS_Dec_Program);
	return BERR_SUCCESS;
}


/***************************************************************************
 *
 */
uint32_t BVBI_P_WSS_Decode_Data_isr (
	BREG_Handle hReg,
    BAVC_SourceId eSource,
	BAVC_Polarity polarity,
	uint16_t *pusData)
{
/*
	Programming note: the implementation here assumes that the bitfield layout
	within registers is the same for all WSS decoder cores in the chip.

	If a chip is built that has multiple WSS decoder cores that are not
	identical, then this routine will have to be redesigned.
*/
	uint32_t H_Status;
	uint32_t ulStatusReg;
	uint32_t ulErrInfo = 0;
	uint16_t usData;
	uint8_t ucLockFlag;
	uint8_t ucParityFlag;
	uint8_t ucOverrunFlag;

	BDBG_ENTER(BVBI_P_WSS_Decode_Data_isr);

	/* Figure out which decoder core to use */
	switch (eSource)
	{
	case BAVC_SourceId_eVdec0:
		H_Status   = BCHP_WSSD_0_Status;
		break;
#if (BVBI_P_NUM_VDEC > 1)
	case BAVC_SourceId_eVdec1:
		H_Status   = BCHP_WSSD_1_Status;
		break;
#endif
	default:
		/* This should never happen!  This parameter was checked by
		   BVBI_Decode_Create() */
		BDBG_LEAVE(BVBI_P_WSS_Decode_Data_isr);
		return (-1);
		break;
	}

	/* WSS is for top field only */
	if (polarity != BAVC_Polarity_eTopField)
	{
		return BVBI_LINE_ERROR_PARITY_CONFLICT | BVBI_LINE_ERROR_WSS_NODATA;
	}


	/* Pull status info out of the hardware */
	ulStatusReg = BREG_Read32 ( hReg, H_Status );
	ucLockFlag    =
		(uint8_t)BCHP_GET_FIELD_DATA (ulStatusReg, WSSD_0_Status, DATA_LOCK   );
	ucParityFlag  =
		(uint8_t)BCHP_GET_FIELD_DATA (ulStatusReg, WSSD_0_Status, PARITY_ERROR);
	ucOverrunFlag =
		(uint8_t)BCHP_GET_FIELD_DATA (ulStatusReg, WSSD_0_Status, DATA_OVERRUN);

	/* Got data? */
	if (ucLockFlag == BCHP_WSSD_0_Status_DATA_LOCK_DATA_FOUND)
	{
		/* read the data */
		usData =
			(uint16_t)BCHP_GET_FIELD_DATA(ulStatusReg, WSSD_0_Status, WSS_DATA);

		/* provide the decoded data to higher level software */
		*pusData = usData;
	}
	else /* No data */
	{
		ulErrInfo |= BVBI_LINE_ERROR_WSS_NODATA;
	}

	/* Report other error conditions */
	if (ucParityFlag == BCHP_WSSD_0_Status_PARITY_ERROR_ERROR)
	{
		ulErrInfo |= BVBI_LINE_ERROR_WSS_PARITY;
	}
	if (ucOverrunFlag == BCHP_WSSD_0_Status_DATA_OVERRUN_ERROR)
	{
		ulErrInfo |= BVBI_LINE_ERROR_WSS_OVERRUN;
	}

	/* Clear status bits in hardware */
	ulStatusReg = (
		BCHP_MASK (WSSD_0_Status, PARITY_ERROR) |
		BCHP_MASK (WSSD_0_Status, DATA_OVERRUN) |
		BCHP_MASK (WSSD_0_Status, DATA_LOCK   ) |
		BCHP_MASK (WSSD_0_Status, DIFFERENCE  )   );
	BREG_Write32 ( hReg, H_Status, ulStatusReg );

	BDBG_LEAVE(BVBI_P_WSS_Decode_Data_isr);
	return ulErrInfo;
}

#endif /** } (BVBI_P_NUM_VDEC >= 1) **/


/***************************************************************************
* Static (private) functions
***************************************************************************/
