/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvbi_cgmsd.c $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 8/10/12 3:30p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vbi/7400/bvbi_cgmsd.c $
 * 
 * Hydra_Software_Devel/2   8/10/12 3:30p pntruong
 * SW7435-276: Updated NTSC/480p to 720x480i/720x480p respectively.
 * Added support 482i/483p with new enums.
 *
 * Hydra_Software_Devel/1   12/3/08 8:00p darnstein
 * PR45819:
 *
 ***************************************************************************/

#include "bstd.h"             /* standard types */
#include "bdbg.h"             /* Dbglib */
#include "bkni.h"			  /* For critical sections */
#include "bvbi.h"             /* VBI processing, this module. */
#include "bvbi_priv.h"        /* VBI internal data structures */
#if (BVBI_P_NUM_VDEC >= 1)
#include "bchp_cgmsd_0.h"     /* RDB info for CGMSD core */
#endif
#if (BVBI_P_NUM_VDEC >= 2)
#include "bchp_cgmsd_1.h"     /* RDB info for CGMSD core */
#endif

BDBG_MODULE(BVBI);


/***************************************************************************
* Forward declarations of static (private) functions
***************************************************************************/


/***************************************************************************
* Implementation of supporting CGMS functions that are not in API
***************************************************************************/

#if (BVBI_P_NUM_VDEC >= 1) /** { **/

void BVBI_P_CGMS_Dec_Init (BREG_Handle hReg, uint32_t ulCoreOffset)
{
	uint32_t ulReg = 0x0;

	BDBG_ENTER(BVBI_P_CGMS_Dec_Init);

	BKNI_EnterCriticalSection();

	BREG_Write32 (hReg, BCHP_CGMSD_0_Reset + ulCoreOffset, ulReg);

	BKNI_LeaveCriticalSection();

	BDBG_LEAVE(BVBI_P_CGMS_Dec_Init);
}

BERR_Code BVBI_P_CGMS_Dec_Program (
	BREG_Handle hReg,
	BAVC_SourceId eSource,
	bool bActive,
	BFMT_VideoFmt eVideoFormat)
{
	uint32_t H_Top_Format;
	uint32_t H_Top_Control;
	uint32_t H_Bot_Format;
	uint32_t H_Bot_Control;
	uint32_t H_Status;
	uint32_t ulTop_FormatReg;
	uint32_t ulTop_ControlReg;
	uint32_t ulBot_FormatReg;
	uint32_t ulBot_ControlReg;
	uint32_t ulStatusReg;
	uint32_t idle_width, num_samples, hsync_delay;
	int top_line;
	int bot_line;
	BERR_Code eErr = BERR_SUCCESS;

	BDBG_ENTER(BVBI_P_CGMS_Dec_Program);

	/* Figure out which decoder core to use */
	switch (eSource)
	{
	case BAVC_SourceId_eVdec0:
		H_Top_Format  = BCHP_CGMSD_0_Top_Format;
		H_Top_Control = BCHP_CGMSD_0_Top_Control;
		H_Bot_Format  = BCHP_CGMSD_0_Bot_Format;
		H_Bot_Control = BCHP_CGMSD_0_Bot_Control;
		H_Status      = BCHP_CGMSD_0_Status;
		break;
#if (BVBI_P_NUM_VDEC > 1)
	case BAVC_SourceId_eVdec1:
		H_Top_Format  = BCHP_CGMSD_1_Top_Format;
		H_Top_Control = BCHP_CGMSD_1_Top_Control;
		H_Bot_Format  = BCHP_CGMSD_1_Bot_Format;
		H_Bot_Control = BCHP_CGMSD_1_Bot_Control;
		H_Status      = BCHP_CGMSD_1_Status;
		break;
#endif
	default:
		/* This should never happen!  This parameter was checked by
		   BVBI_Decode_Create() */
		BDBG_LEAVE(BVBI_P_CGMS_Dec_Program);
		return BERR_TRACE(BERR_INVALID_PARAMETER);
		break;
	}

	BKNI_EnterCriticalSection();

	/* get the control registers' values */
    ulTop_ControlReg = BREG_Read32 (hReg, H_Top_Control);
    ulBot_ControlReg = BREG_Read32 (hReg, H_Bot_Control);

	/* If decoding is to be enabled */
	if (bActive)
	{
		/* Select video line */
		switch (eVideoFormat)
		{
		case BFMT_VideoFmt_eNTSC:
		case BFMT_VideoFmt_eNTSC_J:
		case BFMT_VideoFmt_e720x482_NTSC:
		case BFMT_VideoFmt_e720x482_NTSC_J:
			top_line = 20;
			bot_line = 20;
			idle_width = 304;
			num_samples = 22;
			hsync_delay = 0x49;
			break;

			/* TODO: The idle_width, hsync_delay, and num_samples settings are
			 * ----  not known for HTDV formats!
			 */

#ifdef BVBI_P_HAS_COMPON_INPUT
		case BFMT_VideoFmt_e1080i:
			top_line = 19;
			bot_line = 19;
			idle_width = 392;
			num_samples = 28;
			hsync_delay = 0x89;
			break;

		case BFMT_VideoFmt_e1080i_50Hz:
			top_line = 19;
			bot_line = 19;
			idle_width = 392;
			num_samples = 28;
			hsync_delay = 0x89;
			break;

		case BFMT_VideoFmt_e720p:
			/* Something is wrong with VDEC front end? */
			top_line = 23;
			bot_line = 0;
			idle_width = 296;
			num_samples = 21;
			hsync_delay = 0x8;
			break;

		case BFMT_VideoFmt_e720p_50Hz:
			/* Something is wrong with VDEC front end? */
			top_line = 23;
			bot_line = 0;
			idle_width = 296;
			num_samples = 21;
			hsync_delay = 0x8;
			break;

		case BFMT_VideoFmt_e480p:
		case BFMT_VideoFmt_e720x483p:
			/* Something is wrong with VDEC front end? */
			/* top_line = 41; */
			top_line = 40;
			bot_line = 0;
			idle_width = 264;
			num_samples = 19;
			/* This is optimal for 3563-D0. Other chipsets? */
			hsync_delay = 0x51;
			break;
#endif

		default:
			eErr = BERR_TRACE (BVBI_ERR_VFMT_CONFLICT);
			goto done;
		}

		/* Top control register programming */
		ulTop_ControlReg &= ~(
			BCHP_MASK       ( CGMSD_0_Top_Control, THRESHOLD                 ) |
			BCHP_MASK       ( CGMSD_0_Top_Control, THRESHOLD_TECHNIQUE       ) |
			BCHP_MASK       ( CGMSD_0_Top_Control, BLANK_LEVEL               ) |
			BCHP_MASK       ( CGMSD_0_Top_Control, INERTIA                   ) |
			BCHP_MASK       ( CGMSD_0_Top_Control, RAW_COUNT                 ) |
			BCHP_MASK       ( CGMSD_0_Top_Control, VBI_LINE                  ) |
			BCHP_MASK       ( CGMSD_0_Top_Control, BIT_ORDER                 ) |
			BCHP_MASK       ( CGMSD_0_Top_Control, ENABLE                    ) );
		ulTop_ControlReg |= (
			BCHP_FIELD_DATA ( CGMSD_0_Top_Control, THRESHOLD,              7 ) |
			BCHP_FIELD_ENUM ( CGMSD_0_Top_Control, THRESHOLD_TECHNIQUE, AUTO ) |
			BCHP_FIELD_DATA ( CGMSD_0_Top_Control, BLANK_LEVEL,         0x17 ) |
			BCHP_FIELD_ENUM ( CGMSD_0_Top_Control, INERTIA,       NOISE_FREE ) |
			BCHP_FIELD_DATA ( CGMSD_0_Top_Control, RAW_COUNT,             13 ) |
			BCHP_FIELD_DATA ( CGMSD_0_Top_Control, VBI_LINE,        top_line ) |
			BCHP_FIELD_ENUM ( CGMSD_0_Top_Control, BIT_ORDER,      LSB_FIRST ) |
			BCHP_FIELD_ENUM ( CGMSD_0_Top_Control, ENABLE,             CGMSA ) );

		/* Bottom control register programming */
		ulBot_ControlReg &= ~(
			BCHP_MASK       ( CGMSD_0_Bot_Control, THRESHOLD                 ) |
			BCHP_MASK       ( CGMSD_0_Bot_Control, THRESHOLD_TECHNIQUE       ) |
			BCHP_MASK       ( CGMSD_0_Bot_Control, BLANK_LEVEL               ) |
			BCHP_MASK       ( CGMSD_0_Bot_Control, INERTIA                   ) |
			BCHP_MASK       ( CGMSD_0_Bot_Control, RAW_COUNT                 ) |
			BCHP_MASK       ( CGMSD_0_Bot_Control, VBI_LINE                  ) |
			BCHP_MASK       ( CGMSD_0_Bot_Control, BIT_ORDER                 ) |
			BCHP_MASK       ( CGMSD_0_Bot_Control, ENABLE                    ) );
		ulBot_ControlReg |= (
			BCHP_FIELD_DATA ( CGMSD_0_Bot_Control, THRESHOLD,              7 ) |
			BCHP_FIELD_ENUM ( CGMSD_0_Bot_Control, THRESHOLD_TECHNIQUE, AUTO ) |
			BCHP_FIELD_DATA ( CGMSD_0_Bot_Control, BLANK_LEVEL,         0x17 ) |
			BCHP_FIELD_ENUM ( CGMSD_0_Bot_Control, INERTIA,       NOISE_FREE ) |
			BCHP_FIELD_DATA ( CGMSD_0_Bot_Control, RAW_COUNT,             13 ) |
			BCHP_FIELD_DATA ( CGMSD_0_Bot_Control, VBI_LINE,        bot_line ) |
			BCHP_FIELD_ENUM ( CGMSD_0_Bot_Control, BIT_ORDER,      LSB_FIRST ) |
			BCHP_FIELD_ENUM ( CGMSD_0_Bot_Control, ENABLE,             CGMSA ) );

		/* Program and write top format register value */
		ulTop_FormatReg = BREG_Read32 (hReg, H_Top_Format);
		ulTop_FormatReg &= ~(
			BCHP_MASK       ( CGMSD_0_Top_Format, IDLE_WIDTH                ) |
			BCHP_MASK       ( CGMSD_0_Top_Format, NUM_SAMPLES               ) |
			BCHP_MASK       ( CGMSD_0_Top_Format, HSYNC_DELAY               ) |
			BCHP_MASK       ( CGMSD_0_Top_Format, HSYNC_EDGE                ) );
		ulTop_FormatReg |= (
			BCHP_FIELD_DATA ( CGMSD_0_Top_Format, IDLE_WIDTH,    idle_width ) |
			BCHP_FIELD_DATA ( CGMSD_0_Top_Format, NUM_SAMPLES,  num_samples ) |
			BCHP_FIELD_DATA ( CGMSD_0_Top_Format, HSYNC_DELAY,  hsync_delay ) |
			BCHP_FIELD_ENUM ( CGMSD_0_Top_Format, HSYNC_EDGE,  FALLING_EDGE ) );
		BREG_Write32 ( hReg, H_Top_Format, ulTop_FormatReg );

		/* Program and write bottom format register value */
		ulBot_FormatReg = BREG_Read32 (hReg, H_Bot_Format);
		ulBot_FormatReg &= ~(
			BCHP_MASK       ( CGMSD_0_Bot_Format, IDLE_WIDTH                ) |
			BCHP_MASK       ( CGMSD_0_Bot_Format, NUM_SAMPLES               ) |
			BCHP_MASK       ( CGMSD_0_Bot_Format, HSYNC_DELAY               ) |
			BCHP_MASK       ( CGMSD_0_Bot_Format, HSYNC_EDGE                ) );
		ulBot_FormatReg |= (
			BCHP_FIELD_DATA ( CGMSD_0_Bot_Format, IDLE_WIDTH,    idle_width ) |
			BCHP_FIELD_DATA ( CGMSD_0_Bot_Format, NUM_SAMPLES,  num_samples ) |
			BCHP_FIELD_DATA ( CGMSD_0_Bot_Format, HSYNC_DELAY,  hsync_delay ) |
			BCHP_FIELD_ENUM ( CGMSD_0_Bot_Format, HSYNC_EDGE,  FALLING_EDGE ) );
		BREG_Write32 ( hReg, H_Bot_Format, ulBot_FormatReg );

	}
	else /* Decoding is not to be enabled */
	{
		/* Disable decoding in top field */
		ulTop_ControlReg &=
			~BCHP_MASK       ( CGMSD_0_Top_Control, ENABLE           );
		ulTop_ControlReg |=
			 BCHP_FIELD_ENUM ( CGMSD_0_Top_Control, ENABLE, DISABLED );

		/* Disable decoding in bottom field */
		ulBot_ControlReg &=
			~BCHP_MASK       ( CGMSD_0_Bot_Control, ENABLE           );
		ulBot_ControlReg |=
			 BCHP_FIELD_ENUM ( CGMSD_0_Bot_Control, ENABLE, DISABLED );
	}

	/* Write top field control register */
	BREG_Write32 ( hReg, H_Top_Control, ulTop_ControlReg );

	/* Write bottom field control register */
	BREG_Write32 ( hReg, H_Bot_Control, ulBot_ControlReg );

	/* Clear status bits */
	ulStatusReg = BREG_Read32 ( hReg, H_Status );
	ulStatusReg &= ~(
		BCHP_MASK       ( CGMSD_0_Status, DATA_OVERRUN      ) |
		BCHP_MASK       ( CGMSD_0_Status, BOT_CRC           ) |
		BCHP_MASK       ( CGMSD_0_Status, TOP_CRC           ) |
		BCHP_MASK       ( CGMSD_0_Status, BOT_CHANGE        ) |
		BCHP_MASK       ( CGMSD_0_Status, TOP_CHANGE        ) |
		BCHP_MASK       ( CGMSD_0_Status, BOT_DATA          ) |
		BCHP_MASK       ( CGMSD_0_Status, TOP_DATA          ) );
	ulStatusReg |= (
		BCHP_FIELD_DATA ( CGMSD_0_Status, DATA_OVERRUN, 0x1 ) |
		BCHP_FIELD_DATA ( CGMSD_0_Status, BOT_CRC,      0x1 ) |
		BCHP_FIELD_DATA ( CGMSD_0_Status, TOP_CRC,      0x1 ) |
		BCHP_FIELD_DATA ( CGMSD_0_Status, BOT_CHANGE,   0x1 ) |
		BCHP_FIELD_DATA ( CGMSD_0_Status, TOP_CHANGE,   0x1 ) |
		BCHP_FIELD_DATA ( CGMSD_0_Status, BOT_DATA,     0x1 ) |
		BCHP_FIELD_DATA ( CGMSD_0_Status, TOP_DATA,     0x1 ) );
	BREG_Write32 ( hReg, H_Status, ulStatusReg );

done:
	BKNI_LeaveCriticalSection();

	BDBG_LEAVE(BVBI_P_CGMS_Dec_Program);
	return eErr;
}


uint32_t BVBI_P_CGMS_Decode_Data_isr (
	BREG_Handle hReg,
    BAVC_SourceId eSource,
	BAVC_Polarity polarity,
	uint32_t *pulData)
{
/*
	Programming note: the implementation here assumes that the bitfield layout
	within registers is the same for all CGMS decoder cores in the chip.

	If a chip is built that has multiple CGMS decoder cores that are not
	identical, then this routine will have to be redesigned.
*/
	uint32_t H_Top_Data;
	uint32_t H_Bot_Data;
	uint32_t H_Status;
	uint32_t ulStatusReg;
	uint32_t ulStatusReg2;
	uint32_t ulDataReg = 0;
	uint32_t ulErrInfo = 0;

	BDBG_ENTER(BVBI_P_CGMS_Decode_Data_isr);
	/* Figure out which decoder core to use */
	switch (eSource)
	{
	case BAVC_SourceId_eVdec0:
		H_Top_Data = BCHP_CGMSD_0_Top_Data;
		H_Bot_Data = BCHP_CGMSD_0_Bot_Data;
		H_Status   = BCHP_CGMSD_0_Status;
		break;
#if (BVBI_P_NUM_VDEC > 1)
	case BAVC_SourceId_eVdec1:
		H_Top_Data = BCHP_CGMSD_1_Top_Data;
		H_Bot_Data = BCHP_CGMSD_1_Bot_Data;
		H_Status   = BCHP_CGMSD_1_Status;
		break;
#endif
	default:
		/* This should never happen!  This parameter was checked by
		   BVBI_Decode_Create() */
		BDBG_LEAVE(BVBI_P_CGMS_Decode_Data_isr);
		return (-1);
		break;
	}

	/* Pull status info out of the hardware */
	ulStatusReg = BREG_Read32 ( hReg, H_Status );

	/*
	Programming note:
	Because of PR 10115 on 7038 A0/B0 hardware, I can't be very selective about
	how I clear hardware bits.  I have to clear all conditions as they occur.

	This scheme for clearing status bits requires that TOP field CGMS data be
	processed by this function immediately after the TOP field VBI lines are
	received in hardware.  Fortunately, this is the alternative that most
	users would prefer.
	*/

	/* If top field */
	if ((polarity == BAVC_Polarity_eTopField) ||
	    (polarity == BAVC_Polarity_eFrame   )   )
	{
		/* check to see if we found data in top field */
		if ( BCHP_GET_FIELD_DATA (ulStatusReg, CGMSD_0_Status, TOP_DATA) )
		{
			/* read the data */
			ulDataReg = BREG_Read32 ( hReg, H_Top_Data );

			/* provide the decoded data to higher level software */
			*pulData = ulDataReg;
		}
		else
		{
			/* Failed to find top data in top field, so complain */
			ulErrInfo |= BVBI_LINE_ERROR_CGMS_NODATA;
		}

		/* Inform of CRC error */
		if ( BCHP_GET_FIELD_DATA (ulStatusReg, CGMSD_0_Status, TOP_CRC) != 0 )
		{
			ulErrInfo |= BVBI_LINE_ERROR_CGMS_CRC;
		}

		ulStatusReg2 = ulStatusReg & (
			BCHP_MASK (CGMSD_0_Status, DATA_OVERRUN) |
			BCHP_MASK (CGMSD_0_Status,      TOP_CRC) |
			BCHP_MASK (CGMSD_0_Status,   TOP_CHANGE) |
			BCHP_MASK (CGMSD_0_Status,     TOP_DATA) );
	}
	else /* Bottom field */
	{
		/* check to see if we found data in bottom field */
		if ( BCHP_GET_FIELD_DATA (ulStatusReg, CGMSD_0_Status, BOT_DATA) )
		{
			/* read the data */
			ulDataReg = BREG_Read32 ( hReg, H_Bot_Data );

			/* provide the decoded data to higher level software */
			*pulData = ulDataReg;
		}
		else
		{
			/* Failed to find data in bottom field, so complain */
			ulErrInfo |= BVBI_LINE_ERROR_CGMS_NODATA;
		}

		/* Inform of CRC error */
		if ( BCHP_GET_FIELD_DATA (ulStatusReg, CGMSD_0_Status, BOT_CRC) != 0 )
		{
			ulErrInfo |= BVBI_LINE_ERROR_CGMS_CRC;
		}
		ulStatusReg2 = ulStatusReg & (
			BCHP_MASK (CGMSD_0_Status, DATA_OVERRUN) |
			BCHP_MASK (CGMSD_0_Status,      BOT_CRC) |
			BCHP_MASK (CGMSD_0_Status,   BOT_CHANGE) |
			BCHP_MASK (CGMSD_0_Status,     BOT_DATA) );
	}

	/* Inform of data overrun error */
	if ( BCHP_GET_FIELD_DATA (
		ulStatusReg, CGMSD_0_Status, DATA_OVERRUN) != 0 )
	{
		ulErrInfo |= BVBI_LINE_ERROR_CGMS_OVERRUN;
	}

	/* Finally, clear status in hardware. */
	BREG_Write32 ( hReg, H_Status, ulStatusReg2 );

	BDBG_LEAVE(BVBI_P_CGMS_Decode_Data_isr);
	return ulErrInfo;
}

#endif /** } (BVBI_P_NUM_VDEC >= 1) **/


/***************************************************************************
* Static (private) functions
***************************************************************************/
