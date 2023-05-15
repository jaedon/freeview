/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvbi_ccd.c $
 * $brcm_Revision: Hydra_Software_Devel/3 $
 * $brcm_Date: 8/10/12 3:30p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vbi/7400/bvbi_ccd.c $
 * 
 * Hydra_Software_Devel/3   8/10/12 3:30p pntruong
 * SW7435-276: Updated NTSC/480p to 720x480i/720x480p respectively.
 * Added support 482i/483p with new enums.
 *
 * Hydra_Software_Devel/2   12/21/09 7:04p darnstein
 * SW7550-120: Add support for SECAM variants.
 *
 * Hydra_Software_Devel/1   12/3/08 8:00p darnstein
 * PR45819:
 *
 ***************************************************************************/

#include "bstd.h"           /* standard types */
#include "bdbg.h"           /* Dbglib */
#include "bkni.h"			/* For critical sections */
#include "bvbi.h"           /* VBI processing, this module. */
#include "bvbi_priv.h"      /* VBI internal data structures */

#if (BVBI_P_NUM_VDEC >= 1)
#include "bchp_ccd_0.h"     /* RDB info for CCD core */
#if (BVBI_P_NUM_VDEC >= 2)
#include "bchp_ccd_1.h"     /* RDB info for CCD core */
#endif
#endif

BDBG_MODULE(BVBI);

/***************************************************************************
* Forward declarations of static (private) functions
***************************************************************************/

#if (BVBI_P_NUM_VDEC >= 1) /** { **/
static void BVBI_P_CC_Dec_Get_Word0 (
	BREG_Handle hReg, uint32_t ulWord0_Base,
	uint8_t ucRegNum, uint16_t *pusWord0 );
static void BVBI_P_CC_Dec_Get_Word1 (
	BREG_Handle hReg, uint32_t ulWord0_Base,
	uint8_t ucRegNum, uint16_t *pusWord1 );
static void BVBI_P_CC_Dec_Init (BREG_Handle hReg, uint32_t ulCoreOffset);
static BERR_Code BVBI_P_CC_Validate_Word ( uint16_t usWord );
#endif /** } (BVBI_P_NUM_VDEC >= 1) **/


/***************************************************************************
* Some private extensions to the RDB
***************************************************************************/
#define BCHP_CCD_0_Top_STATUS_ACKNOWLEDGE_DATA    0
#define BCHP_CCD_0_Bottom_STATUS_ACKNOWLEDGE_DATA 0


/***************************************************************************
* Implementation supporting closed caption functions that are not in API
***************************************************************************/

#if (BVBI_P_NUM_VDEC >= 1) /** { **/
/***************************************************************************
 *
 */
void BVBI_P_CC_Dec_Init (BREG_Handle hReg, uint32_t ulCoreOffset)
{
	uint32_t ulReg = 0x0;

	BDBG_ENTER(BVBI_P_CC_Dec_Init);

	BKNI_EnterCriticalSection();

	BREG_Write32 (hReg, BCHP_CCD_0_Reset + ulCoreOffset, ulReg);

	BKNI_LeaveCriticalSection();

	BDBG_LEAVE(BVBI_P_CC_Dec_Init);
}

#if (BVBI_P_NUM_VDEC >= 1) /** { **/
BERR_Code BVBI_P_CC_Dec_Program (
	BREG_Handle hReg,
	BAVC_SourceId eSource,
	bool bActive,
	BFMT_VideoFmt eVideoFormat)
{
	uint32_t H_Control;
	uint32_t H_Status;
	uint32_t H_Top;
	uint32_t H_Bottom;
	uint32_t H_Lock;
	uint32_t ulControlReg;
	uint32_t ulStatusReg;
	uint32_t ulTopReg;
	uint32_t ulLockReg;
	uint32_t ulBottomReg;
	uint16_t usTopActiveLine;
	uint16_t usBottomActiveLine;
	uint32_t ulCoreOffset;
	int index;

	BDBG_ENTER(BVBI_P_CC_Dec_Program);

	/* Figure out which decoder core to use */
	switch (eSource)
	{
	case BAVC_SourceId_eVdec0:
		H_Control = BCHP_CCD_0_Control;
		H_Status = BCHP_CCD_0_Status;
		H_Top = BCHP_CCD_0_Top;
		H_Bottom = BCHP_CCD_0_Bottom;
		H_Lock = BCHP_CCD_0_Lock;
		ulCoreOffset = 0x0;
		index = 0;
		break;
#if (BVBI_P_NUM_VDEC >= 2)
	case BAVC_SourceId_eVdec1:
		H_Control = BCHP_CCD_1_Control;
		H_Status = BCHP_CCD_1_Status;
		H_Top = BCHP_CCD_1_Top;
		H_Bottom = BCHP_CCD_1_Bottom;
		H_Lock = BCHP_CCD_1_Lock;
		ulCoreOffset = BCHP_CCD_1_RevID - BCHP_CCD_0_RevID;
		index = 1;
		break;
#endif
	default:
		/* This should never happen!  This parameter was checked by
		   BVBI_Decode_Create() */
		BDBG_LEAVE(BVBI_P_CC_Dec_Program);
		return BERR_TRACE(BERR_INVALID_PARAMETER);
		break;
	}

	/* If user wants to turn off closed caption processing, just reset the
	   entire core. */
	if (!bActive)
	{
		BVBI_P_CC_Dec_Init (hReg, ulCoreOffset);
		BDBG_LEAVE(BVBI_P_CC_Dec_Program);
		return BERR_SUCCESS;
	}

	BKNI_EnterCriticalSection();

	/* get and set fields in the CCD control register */
	ulControlReg = BREG_Read32 (hReg, H_Control);
	ulControlReg &= ~(
		BCHP_MASK       (CCD_0_Control, BYTE_ORDER                            ) |
		BCHP_MASK       (CCD_0_Control, SHIFT_DIRECTION                       ) |
		BCHP_MASK       (CCD_0_Control, REGISTER_USE_MODE                     ) |
		BCHP_MASK       (CCD_0_Control, ENABLE_CLOSED_CAPTION                 ) );
	ulControlReg |= (
		BCHP_FIELD_ENUM (CCD_0_Control, BYTE_ORDER,            LOW_BYTE_FIRST ) |
		BCHP_FIELD_ENUM (CCD_0_Control, SHIFT_DIRECTION,       LSB2MSB        ) |
		BCHP_FIELD_ENUM (CCD_0_Control, REGISTER_USE_MODE,     SPLIT          ) |
		BCHP_FIELD_ENUM (CCD_0_Control, ENABLE_CLOSED_CAPTION, ENABLED        ) );

	/* get and set fields in the CCD top register */
	ulTopReg = BREG_Read32 (hReg,  H_Top);
	ulTopReg &= ~(
		BCHP_MASK       (CCD_0_Top, STATUS          ) );
	ulTopReg |= (
		BCHP_FIELD_DATA (CCD_0_Top, STATUS,         0 ) );

	/* get and set fields in the CCD bottom register */
	ulBottomReg = BREG_Read32 (hReg,  H_Bottom);
	ulBottomReg &= ~(
		BCHP_MASK       (CCD_0_Bottom, STATUS          ) );
	ulBottomReg |= (
		BCHP_FIELD_DATA (CCD_0_Bottom, STATUS,         0 ) );

	/* Select video format */
	switch (eVideoFormat)
	{
	case BFMT_VideoFmt_eNTSC:
	case BFMT_VideoFmt_eNTSC_J:
	case BFMT_VideoFmt_e720x482_NTSC:
	case BFMT_VideoFmt_e720x482_NTSC_J:
		/* NTSC specific settings */

		usTopActiveLine    =  21;
		usBottomActiveLine = 284;

		ulControlReg &= ~(
			BCHP_MASK       (CCD_0_Control, SIGNAL_THRESHOLD      ) |
			BCHP_MASK       (CCD_0_Control, PHASE_SYNC_DELAY      ) |
			BCHP_MASK       (CCD_0_Control, VIDEO_FORMAT          ) );
		ulControlReg |= (
			BCHP_FIELD_DATA (CCD_0_Control, SIGNAL_THRESHOLD, 0x43) |
			BCHP_FIELD_DATA (CCD_0_Control, PHASE_SYNC_DELAY, 0x37) |
			BCHP_FIELD_ENUM (CCD_0_Control, VIDEO_FORMAT,     NTSC) );

		ulTopReg &= ~(
			BCHP_MASK       (CCD_0_Top, VSYNC_OFFSET    ) |
			BCHP_MASK       (CCD_0_Top, ACTIVE_LINES    ) );
		ulTopReg |= (
			BCHP_FIELD_DATA (CCD_0_Top, VSYNC_OFFSET, 0xF ) |
			BCHP_FIELD_DATA (CCD_0_Top, ACTIVE_LINES,
				(((uint16_t)1) << (usTopActiveLine - 10 - 1))));

		ulBottomReg &= ~(
			BCHP_MASK       (CCD_0_Bottom, VSYNC_OFFSET    ) |
			BCHP_MASK       (CCD_0_Bottom, ACTIVE_LINES    ) );
		ulBottomReg |= (
			BCHP_FIELD_DATA (CCD_0_Bottom, VSYNC_OFFSET, 0xF ) |
			BCHP_FIELD_DATA (CCD_0_Bottom, ACTIVE_LINES,
                (((uint16_t)1) << (usBottomActiveLine - 273 - 1))));

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
        /* 576I settings */

		usTopActiveLine    =  22;
		usBottomActiveLine = 335;

		ulControlReg &= ~(
			BCHP_MASK       (CCD_0_Control, SIGNAL_THRESHOLD        ) |
			BCHP_MASK       (CCD_0_Control, PHASE_SYNC_DELAY        ) |
			BCHP_MASK       (CCD_0_Control, VIDEO_FORMAT            ) );
		ulControlReg |= (
			BCHP_FIELD_DATA (CCD_0_Control, SIGNAL_THRESHOLD, 0x43 ) |
			BCHP_FIELD_DATA (CCD_0_Control, PHASE_SYNC_DELAY, 0x37 ) |
			BCHP_FIELD_ENUM (CCD_0_Control, VIDEO_FORMAT,      PAL ) );

		ulTopReg &= ~(
			BCHP_MASK       (CCD_0_Top, VSYNC_OFFSET    ) |
			BCHP_MASK       (CCD_0_Top, ACTIVE_LINES    ) );
		ulTopReg |= (
			BCHP_FIELD_DATA (CCD_0_Top, VSYNC_OFFSET, 0xF ) |
			BCHP_FIELD_DATA (CCD_0_Top, ACTIVE_LINES,
				(((uint16_t)1) << (usTopActiveLine - 11 - 1))));

		ulBottomReg &= ~(
			BCHP_MASK       (CCD_0_Bottom, VSYNC_OFFSET    ) |
			BCHP_MASK       (CCD_0_Bottom, ACTIVE_LINES    ) );
		ulBottomReg |= (
			BCHP_FIELD_DATA (CCD_0_Bottom, VSYNC_OFFSET, 0xF ) |
			BCHP_FIELD_DATA (CCD_0_Bottom, ACTIVE_LINES,
                (((uint16_t)1) << (usBottomActiveLine - 324 - 1))));

		break;

	case BFMT_VideoFmt_ePAL_M:
		/* PAL-M specific settings */
		usTopActiveLine    =  18;
		usBottomActiveLine = 281;

		ulControlReg &= ~(
			BCHP_MASK       (CCD_0_Control, VIDEO_FORMAT          ) |
			BCHP_MASK       (CCD_0_Control, SIGNAL_THRESHOLD      ) );
		ulControlReg |= (
			BCHP_FIELD_ENUM (CCD_0_Control, VIDEO_FORMAT,     NTSC) |
			BCHP_FIELD_DATA (CCD_0_Control, SIGNAL_THRESHOLD, 0x43) );

		ulTopReg &= ~(
			BCHP_MASK       (CCD_0_Top, VSYNC_OFFSET    ) |
			BCHP_MASK       (CCD_0_Top, ACTIVE_LINES    ) );
		ulTopReg |= (
			BCHP_FIELD_DATA (CCD_0_Top, VSYNC_OFFSET, 0x0 ) |
			BCHP_FIELD_DATA (CCD_0_Top, ACTIVE_LINES,
				(((uint16_t)1) << (usTopActiveLine - 10 - 1))));

		ulBottomReg &= ~(
			BCHP_MASK       (CCD_0_Bottom, VSYNC_OFFSET    ) |
			BCHP_MASK       (CCD_0_Bottom, ACTIVE_LINES    ) );
		ulBottomReg |= (
			BCHP_FIELD_DATA (CCD_0_Bottom, VSYNC_OFFSET, 0x0 ) |
			BCHP_FIELD_DATA (CCD_0_Bottom, ACTIVE_LINES,
                (((uint16_t)1) << (usBottomActiveLine - 273 - 1))));

		break;

	default:
		BKNI_LeaveCriticalSection();
		BDBG_LEAVE(BVBI_P_CC_Dec_Program);
		return BERR_TRACE (BERR_INVALID_PARAMETER);
		break;
	}

	/* write the three registers with updated values */
    BREG_Write32 ( hReg, H_Control, ulControlReg );
    BREG_Write32 ( hReg, H_Top,     ulTopReg     );
    BREG_Write32 ( hReg, H_Bottom,  ulBottomReg  );

	/* Clear the status bits */
	ulStatusReg = (
		BCHP_FIELD_DATA (CCD_0_Status, DATA_OVERRUN,  1 ) |
		BCHP_FIELD_DATA (CCD_0_Status, BF_PARITY_ERR, 1 ) |
		BCHP_FIELD_DATA (CCD_0_Status, TF_PARITY_ERR, 1 ) );
    BREG_Write32 ( hReg, H_Status, ulStatusReg );

	/* Program the lock register */
    ulLockReg = BREG_Read32 ( hReg, H_Lock );
	ulLockReg &= ~(
		BCHP_MASK       ( CCD_0_Lock, DELAY     ) |
		BCHP_MASK       ( CCD_0_Lock, STATUS    ) );
	ulLockReg |= (
		BCHP_FIELD_DATA ( CCD_0_Lock, DELAY,  3 ) |
		BCHP_FIELD_DATA ( CCD_0_Lock, STATUS, 0 ) );
    BREG_Write32 ( hReg, H_Lock, ulLockReg);

	BKNI_LeaveCriticalSection();
	BDBG_LEAVE(BVBI_P_CC_Dec_Program);
	return BERR_SUCCESS;
}

uint32_t BVBI_P_CC_Decode_Data_isr (
	BREG_Handle hReg,
    BAVC_SourceId eSource,
	BAVC_Polarity polarity,
	uint16_t *pusData)
{
/*
	Programming note: the implementation here assumes that the bitfield layout
	within registers is the same for all CC decoder cores in the chip.

	If a chip is built that has multiple CC decoder cores that are not
	identical, then this routine will have to be redesigned.
*/
	uint32_t ulReg;
	uint32_t H_Top;
	uint32_t H_Bottom;
	uint32_t H_Status;
	uint32_t H_Data0;
	uint16_t usStatus;
	uint16_t usWord0;
	uint32_t ulErrInfo = 0;
	uint32_t status_bit1 = 0x01;
	uint32_t status_bit2 = 0x40;

	BDBG_ENTER(BVBI_P_CC_Decode_Data_isr);

	switch (eSource)
	{
	case BAVC_SourceId_eVdec0:
		H_Top    = BCHP_CCD_0_Top;
		H_Bottom = BCHP_CCD_0_Bottom;
		H_Status = BCHP_CCD_0_Status;
		H_Data0  = BCHP_CCD_0_Data0;
		break;
#if (BVBI_P_NUM_VDEC > 1)
	case BAVC_SourceId_eVdec1:
		H_Top    = BCHP_CCD_1_Top;
		H_Bottom = BCHP_CCD_1_Bottom;
		H_Status = BCHP_CCD_1_Status;
		H_Data0  = BCHP_CCD_1_Data0;
		break;
#endif
	default:
		/* This should never happen!  This parameter was checked by
		   BVBI_Decode_Create() */
		BDBG_LEAVE(BVBI_P_CC_Decode_Data_isr);
		return (-1);
		break;
	}

	/* If top field */
	if (polarity == BAVC_Polarity_eTopField)
	{
		/* Pull status info out of the hardware */
		ulReg = BREG_Read32 (hReg, H_Top);
		usStatus = (uint16_t)BCHP_GET_FIELD_DATA (ulReg, CCD_0_Top, STATUS);

		/* check to see if we found the top line */
		if(usStatus & status_bit1)
		{
			/* read the data 0 register */
			BVBI_P_CC_Dec_Get_Word0 (hReg, H_Data0, 0, &usWord0);

			/* provide the decoded data to higher level software */
			*pusData = usWord0;

			/* Inform of parity error */
			if ( BVBI_P_CC_Validate_Word ( usWord0 ) != BERR_SUCCESS)
			{
				ulErrInfo |= BVBI_LINE_ERROR_CC_PARITY;
			}

			/* Write Closed Caption Decoder Top Register To Acknowledge Data */
			ulReg &= ~BCHP_MASK       (CCD_0_Top, STATUS                  );
			/* The following should work, but it does not.
			ulReg |=  BCHP_FIELD_DATA (CCD_0_Top, STATUS, ~usStatus       );
			*/
			BREG_Write32 (hReg, H_Top, ulReg);
		}
		else
		{
			/* Failed to find top line, so complain */
			ulErrInfo |= BVBI_LINE_ERROR_CC_NODATA;
		}
	}
	else /* Bottom field */
	{
		/* Pull status info out of the hardware */
		ulReg = BREG_Read32 (hReg, H_Bottom);
		usStatus = (uint16_t)BCHP_GET_FIELD_DATA (ulReg, CCD_0_Bottom, STATUS);

		/* check to see if we found the bottom line */
		if (usStatus & status_bit2)
		{
			/**
			 * read the data 3 register
			 * This is split mode (see hardware doc for more information
			 * on split/share mode).
			 */
			BVBI_P_CC_Dec_Get_Word0 (hReg, H_Data0, 3, &usWord0);

			/* provide the decoded data to higher level software */
			*pusData = usWord0;

			/* Inform of parity error */
			if ( BVBI_P_CC_Validate_Word ( usWord0 ) != BERR_SUCCESS)
			{
				ulErrInfo |= BVBI_LINE_ERROR_CC_PARITY;
			}

			/* Write Closed Caption Decoder Bottom Register
			to Acknowledge Data */
			ulReg &= ~BCHP_MASK       (CCD_0_Bottom, STATUS                  );
			/* The following should work, but it does not.
			ulReg |=  BCHP_FIELD_DATA (CCD_0_Bottom, STATUS, ~usStatus       );
			*/
			BREG_Write32 (hReg, H_Bottom, ulReg);
		}
		else
		{
			/* Failed to find bottom line, so complain */
			ulErrInfo |= BVBI_LINE_ERROR_CC_NODATA;
		}
	}

	/* Check for data overrun */
	ulReg = BREG_Read32 ( hReg, H_Status );
	if (BCHP_GET_FIELD_DATA (ulReg, CCD_0_Status, DATA_OVERRUN) != 0)
	{
		ulErrInfo |= BVBI_LINE_ERROR_CC_OVERRUN;
	}

	/* Clear status bits */
	ulReg &= ~(
		BCHP_MASK       ( CCD_0_Status, DATA_OVERRUN     ) |
		BCHP_MASK       ( CCD_0_Status, BF_PARITY_ERR    ) |
		BCHP_MASK       ( CCD_0_Status, TF_PARITY_ERR    ) );
	ulReg |= (
		BCHP_FIELD_DATA ( CCD_0_Status, DATA_OVERRUN,  1 ) |
		BCHP_FIELD_DATA ( CCD_0_Status, BF_PARITY_ERR, 1 ) |
		BCHP_FIELD_DATA ( CCD_0_Status, TF_PARITY_ERR, 1 ) );
    BREG_Write32 ( hReg, H_Status, ulReg) ;

	BDBG_LEAVE(BVBI_P_CC_Decode_Data_isr);
	return ulErrInfo;
}
#endif /** } (BVBI_P_NUM_VDEC >= 1) **/


/***************************************************************************
* Static (private) functions
***************************************************************************/

/***************************************************************************
 *
 */
static void BVBI_P_CC_Dec_Get_Word0 (
	BREG_Handle hReg, uint32_t ulWord0_Base,
	uint8_t ucRegNum, uint16_t *pusWord0 )
{
/*
	Programming note: the implementation here assumes that the
	bitfield layout within registers is the same as the 7038 A0
	CCD_0_NP core.

	If a chip is built that has a different CC decoder core, then
	this routine will have to be redesigned.

	Note that this routine does NOT have critical sections, but it accesses
	registers.
*/
	uint32_t ulReg = 0;

	BDBG_ENTER(BVBI_P_CC_Dec_Get_Word0);

    /* get data from register */
	ulReg = BREG_Read32 (hReg,
		ulWord0_Base + ((BCHP_CCD_0_Data1 - BCHP_CCD_0_Data0) * ucRegNum));

    /* store data */
    *pusWord0 = (uint16_t)BCHP_GET_FIELD_DATA (ulReg, CCD_0_Data0, WORD0);

	BDBG_LEAVE(BVBI_P_CC_Dec_Get_Word0);
}

/***************************************************************************
 *
 */
static void BVBI_P_CC_Dec_Get_Word1 (
	BREG_Handle hReg, uint32_t ulWord1_Base,
	uint8_t ucRegNum, uint16_t *pusWord1 )
{
/*
	Programming note: the implementation here assumes that the
	bitfield layout within registers is the same as the 7038 A0
	CCD_0_NP core.

	If a chip is built that has a different CC decoder core, then
	this routine will have to be redesigned.

	Note that this routine does NOT have critical sections, but it accesses
	registers.
*/
	uint32_t ulReg = 0;

	BDBG_ENTER(BVBI_P_CC_Dec_Get_Word1);

    /* get data from register */
	ulReg = BREG_Read32 (hReg,
		ulWord1_Base + ((BCHP_CCD_0_Data1 - BCHP_CCD_0_Data0) * ucRegNum));

    /* store data */
    *pusWord1 = (uint16_t)BCHP_GET_FIELD_DATA (ulReg, CCD_0_Data0, WORD1);

	BDBG_LEAVE(BVBI_P_CC_Dec_Get_Word1);
}

/***************************************************************************
 * Breaks up a 16-bit word into two 8-bit bytes and checks the parity bit
 * for odd parity.  If the parity of either byte is incorrect it
 * returns BERR_UNKNOWN.  Otherwise it returns BERR_SUCCESS.
 * and returns the reassembled 16-bit word.
 */
static BERR_Code BVBI_P_CC_Validate_Word ( uint16_t usWord )
{
	BERR_Code eErr     = BERR_SUCCESS;
    uint8_t ulByte0   = 0;
    uint8_t ulByte1   = 0;

    /* not NULL? (this speeds things up?) */
    if (usWord != 0x8080)
    {
        /* disassemble the word */
        ulByte0 = (uint8_t)(usWord & 0x00FF);
        ulByte1 = (uint8_t)(usWord >> 8);

        /* check if the two bytes have the proper parity */
        if ((ulByte0 != BVBI_P_SetOddParityBit (ulByte0)) ||
            (ulByte1 != BVBI_P_SetOddParityBit (ulByte1)))
        {
			/* This is the parity error */
			eErr = (BERR_UNKNOWN);
        }
	}

	/* Return error state */
	return eErr;
}

#endif /** } (BVBI_P_NUM_VDEC >= 1) **/
