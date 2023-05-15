/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvbi_wsse.c $
 * $brcm_Revision: Hydra_Software_Devel/9 $
 * $brcm_Date: 8/10/12 3:32p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vbi/7420/bvbi_wsse.c $
 * 
 * Hydra_Software_Devel/9   8/10/12 3:32p pntruong
 * SW7435-276: Updated NTSC/480p to 720x480i/720x480p respectively.
 * Added support 482i/483p with new enums.
 *
 * Hydra_Software_Devel/8   2/20/12 2:53p darnstein
 * SW7425-2434: more detail in error messages.
 *
 * Hydra_Software_Devel/7   2/20/12 12:56p darnstein
 * SW7425-2434: when an unsupported video format is entered, the BDBG
 * error message should be informative.
 *
 * Hydra_Software_Devel/6   12/21/09 7:03p darnstein
 * SW7550-120: Add support for SECAM variants.
 *
 * Hydra_Software_Devel/5   6/24/09 7:10p darnstein
 * PR53956: 7420-B0 has a new WSE core.
 *
 * Hydra_Software_Devel/4   5/13/09 1:18p darnstein
 * PR53956: 7420-Bx has fixed WSE core.
 *
 * Hydra_Software_Devel/3   3/27/09 7:43p darnstein
 * PR53635: Remove internal ConfigForOthers code. It is obsolete, and it
 * was causing a problem (this PR).
 *
 * Hydra_Software_Devel/2   12/4/08 6:07p darnstein
 * PR45819: 7420 software will now compile, but not link.
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
#if (BVBI_P_NUM_WSE >= 1)
#include "bchp_wse_0.h"  /* RDB info for primary WSSE core */
#endif
#if (BVBI_P_NUM_WSE >= 2)
#include "bchp_wse_1.h"   /* RDB info for secondary WSSE core */
#endif
#if (BVBI_P_NUM_WSE >= 3)
#include "bchp_wse_2.h"  /* RDB info for tertiary WSSE core */
#endif
#if (BVBI_P_NUM_WSE_656 >= 1)
#include "bchp_wse_ancil_0.h"   /* RDB info for ITU-R 656 "bypass" WSSE core */
#endif

BDBG_MODULE(BVBI);


/***************************************************************************
* Forward declarations of static (private) functions
***************************************************************************/
static uint32_t P_GetCoreOffset (bool is656, uint8_t hwCoreIndex);


/***************************************************************************
* Implementation of supporting WSS functions that are not in API
***************************************************************************/

/***************************************************************************
 *
 */
void BVBI_P_WSS_Enc_Init (BREG_Handle hReg, uint8_t hwCoreIndex)
{
	uint32_t ulCoreOffset;
	uint32_t ulDataReg;
	uint32_t ulControlReg;

	BDBG_ENTER(BVBI_P_WSS_Enc_Init);

	BVBI_P_VIE_SoftReset (hReg, false, hwCoreIndex, BVBI_P_SELECT_WSS);

	/* Determine which core to access */
	ulCoreOffset = P_GetCoreOffset (false, hwCoreIndex);
	if (ulCoreOffset == 0xFFFFFFFF)
	{
		/* This should never happen!  This parameter was checked by
		   BVBI_Encode_Create() */
		BDBG_ASSERT (0);
	}

	BKNI_EnterCriticalSection();

	/* Program the control register */
	ulControlReg = BREG_Read32 (hReg, BCHP_WSE_0_control + ulCoreOffset);
	ulControlReg &= ~(
		BCHP_MASK       (WSE_0_control, output_attenuation             ) |
		BCHP_MASK       (WSE_0_control, OUTPUT_ORDER                   ) |
		BCHP_MASK       (WSE_0_control, anci656_enable                 ) |
		BCHP_MASK       (WSE_0_control, invert_data                    ) |
		BCHP_MASK       (WSE_0_control, active_line                    ) |
		BCHP_MASK       (WSE_0_control, enable                         ) );
	ulControlReg |= (
		BCHP_FIELD_DATA (WSE_0_control, output_attenuation,        0x64) |
		BCHP_FIELD_ENUM (WSE_0_control, OUTPUT_ORDER,       Low_Pad_2nd) |
		BCHP_FIELD_DATA (WSE_0_control, anci656_enable,               1) |
		BCHP_FIELD_ENUM (WSE_0_control, invert_data,                Off) |
		BCHP_FIELD_DATA (WSE_0_control, active_line,                 22) |
		BCHP_FIELD_DATA (WSE_0_control, enable,                       0) );
	BREG_Write32 (hReg, BCHP_WSE_0_control + ulCoreOffset, ulControlReg);

	/* Clear burst lock status */
	ulDataReg = BREG_Read32 (hReg, BCHP_WSE_0_wss_data + ulCoreOffset);
	ulDataReg &= ~BCHP_MASK       (WSE_0_wss_data, wss_data     );
	ulDataReg |=  BCHP_FIELD_DATA (WSE_0_wss_data, wss_data, 0x0);
	BREG_Write32 (hReg, BCHP_WSE_0_wss_data + ulCoreOffset, ulDataReg);

	BKNI_LeaveCriticalSection();

	BDBG_LEAVE(BVBI_P_WSS_Enc_Init);
}

#if (BVBI_P_NUM_WSE_656 >= 1)
/***************************************************************************
 *
 */
void BVBI_P_WSS_656_Enc_Init (BREG_Handle hReg, uint8_t hwCoreIndex)
{
	uint32_t ulDataReg;
	uint32_t ulControlReg;

	/* This is just a little sloppy */
	BSTD_UNUSED (hwCoreIndex);

	BDBG_ENTER(BVBI_P_WSS_656_Enc_Init);

	BKNI_EnterCriticalSection();

	/* Start by doing a reset */

	BVBI_P_VIE_SoftReset (hReg, true, hwCoreIndex, BVBI_P_SELECT_WSS);

	/* Program the control register */
	ulControlReg = BREG_Read32 (hReg, BCHP_WSE_ANCIL_0_control);
	ulControlReg &= ~(
		BCHP_MASK       (WSE_ANCIL_0_control, OUTPUT_ORDER                   ) |
		BCHP_MASK       (WSE_ANCIL_0_control, anci656_enable                 ) |
		BCHP_MASK       (WSE_ANCIL_0_control, active_line                    ) );
	ulControlReg |= (
		BCHP_FIELD_ENUM (WSE_ANCIL_0_control, OUTPUT_ORDER,       Low_Pad_2nd) |
		BCHP_FIELD_DATA (WSE_ANCIL_0_control, anci656_enable,               1) |
		BCHP_FIELD_DATA (WSE_ANCIL_0_control, active_line,                 22) );
	BREG_Write32 (hReg, BCHP_WSE_ANCIL_0_control, ulControlReg);

	/* Clear burst lock status */
	ulDataReg = BREG_Read32 (hReg, BCHP_WSE_ANCIL_0_wss_data);
	ulDataReg &= ~BCHP_MASK       (WSE_ANCIL_0_wss_data, wss_data     );
	ulDataReg |=  BCHP_FIELD_DATA (WSE_ANCIL_0_wss_data, wss_data, 0x0);
	BREG_Write32 (hReg, BCHP_WSE_ANCIL_0_wss_data, ulDataReg);

	BKNI_LeaveCriticalSection();

	BDBG_LEAVE(BVBI_P_WSS_656_Enc_Init);
}
#endif /** } (BVBI_P_NUM_WSE_656 >= 1) **/

BERR_Code BVBI_P_WSS_Enc_Program (
	BREG_Handle hReg,
	bool is656,
	uint8_t hwCoreIndex,
	bool bActive,
	bool bPR18010_bad_line_number,
	BFMT_VideoFmt eVideoFormat)
{
/*
	Programming note: the implementation here assumes that the bitfield layout
	within registers is the same for all WSS encoder cores in the chip.

	If a chip is built that has multiple WSS encoder cores that are not
	identical, then this routine will have to be redesigned.
*/
	BERR_Code eErr;
	uint32_t ulCoreOffset;
	uint32_t ulControlReg;
	uint32_t start_delay;
	uint32_t ulActiveLine;
#if defined(BVBI_P_WSE_VER3) || defined(BVBI_P_WSE_VER4) || \
	defined(BVBI_P_WSE_VER5)
	uint32_t iDecimate = BCHP_WSE_0_control_DECIMATOR_EN_DISABLE;
#endif
#if (BVBI_P_NUM_WSE_656 >= 1)
#else
	BSTD_UNUSED (bPR18010_bad_line_number);
#endif

	BDBG_ENTER(BVBI_P_WSS_Enc_Program);

	/* Figure out which encoder core to use */
	ulCoreOffset = P_GetCoreOffset (is656, hwCoreIndex);
	if (ulCoreOffset == 0xFFFFFFFF)
	{
		/* This should never happen!  This parameter was checked by
		   BVBI_Encode_Create() */
		BDBG_LEAVE(BVBI_P_WSE_Enc_Program);
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* Complain if video format is not supported */
	switch (eVideoFormat)
	{
	/* NTSC case is just for testing */
	case BFMT_VideoFmt_eNTSC:
	case BFMT_VideoFmt_eNTSC_J:
	case BFMT_VideoFmt_e720x482_NTSC:
	case BFMT_VideoFmt_e720x482_NTSC_J:
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
		start_delay = 0xE;
		ulActiveLine = 22;
		break;
	case BFMT_VideoFmt_e576p_50Hz:
		/* Assumes use of special 54 MHz VEC IT microcode */
		start_delay = 0x7;
		ulActiveLine = 42;
#if defined(BVBI_P_WSE_VER3) || defined(BVBI_P_WSE_VER4) || \
	defined(BVBI_P_WSE_VER5)
		iDecimate = BCHP_WSE_0_control_DECIMATOR_EN_ENABLE;
#endif
		break;
	case BFMT_VideoFmt_eSECAM_L:
	case BFMT_VideoFmt_eSECAM_B:
	case BFMT_VideoFmt_eSECAM_G:
	case BFMT_VideoFmt_eSECAM_D:
	case BFMT_VideoFmt_eSECAM_K:
	case BFMT_VideoFmt_eSECAM_H:
		start_delay = 0x18;
		ulActiveLine = 22;
		break;

	default:
		start_delay = 0x0;
		ulActiveLine = 0;
		if (bActive)
		{
			BDBG_ERR(("BVBI_WSSE: video format %d not supported",
				eVideoFormat));
			return BERR_TRACE (BVBI_ERR_VFMT_CONFLICT);
		}
	}
	if (is656)
	{
#if (BVBI_P_NUM_WSE_656 >= 1)
		/* PR18010 workaround, with exception. */
		if (!bPR18010_bad_line_number)
		{
			ulActiveLine -= 1;
		}
#endif
	}

	/* Program the active line in the control register */
	ulControlReg = BREG_Read32 (hReg, BCHP_WSE_0_control + ulCoreOffset);
	if (is656)
	{
#if (BVBI_P_NUM_WSE_656 >= 1)
		ulControlReg &=
			~BCHP_MASK       (WSE_ANCIL_0_control, active_line              );
		ulControlReg |=
			 BCHP_FIELD_DATA (WSE_ANCIL_0_control, active_line, ulActiveLine);
#ifdef BVBI_P_HAS_WSE_PARITY
#if defined(BVBI_P_WSE_VER1)
		ulControlReg &=
			~BCHP_MASK       (WSE_ANCIL_0_control, AUTO_PARITY_TYP_656      );
		ulControlReg |=
			 BCHP_FIELD_ENUM (WSE_ANCIL_0_control, AUTO_PARITY_TYP_656,  ODD);
#endif
		ulControlReg &=
			~BCHP_MASK       (WSE_ANCIL_0_control, AUTO_PARITY_EN_656       );
		ulControlReg |=
			 BCHP_FIELD_DATA (WSE_ANCIL_0_control, AUTO_PARITY_EN_656,     1);
#endif
#endif
	}
	else
	{
		ulControlReg &=
			~BCHP_MASK       (WSE_0_control, active_line              );
		ulControlReg |=
			 BCHP_FIELD_DATA (WSE_0_control, active_line, ulActiveLine);
		ulControlReg &=
			~BCHP_MASK       (WSE_0_control, start_delay              );
		ulControlReg |=
			 BCHP_FIELD_DATA (WSE_0_control, start_delay,  start_delay);
#ifdef BVBI_P_HAS_WSE_PARITY
		ulControlReg &=
			~BCHP_MASK       (WSE_0_control, AUTO_PARITY_TYP          );
		ulControlReg |=
			 BCHP_FIELD_ENUM (WSE_0_control, AUTO_PARITY_TYP,      ODD);
#if defined(BVBI_P_WSE_VER1)
		ulControlReg &=
			~BCHP_MASK       (WSE_0_control, AUTO_PARITY_TYP_656       );
		ulControlReg |=
			 BCHP_FIELD_ENUM (WSE_0_control, AUTO_PARITY_TYP_656,  ODD);
#endif
		ulControlReg &=
			~BCHP_MASK       (WSE_0_control, AUTO_PARITY_EN           );
		ulControlReg |=
			 BCHP_FIELD_DATA (WSE_0_control, AUTO_PARITY_EN ,        1);
		ulControlReg &=
			~BCHP_MASK       (WSE_0_control, AUTO_PARITY_EN_656       );
		ulControlReg |=
			 BCHP_FIELD_DATA (WSE_0_control, AUTO_PARITY_EN_656,     1);
#endif
#if defined(BVBI_P_WSE_VER3) || defined(BVBI_P_WSE_VER4) || \
	defined(BVBI_P_WSE_VER5)
		ulControlReg &=
			~BCHP_MASK       (WSE_0_control, DECIMATOR_EN             );
		ulControlReg |=
			 BCHP_FIELD_DATA (WSE_0_control, DECIMATOR_EN,   iDecimate);
#endif
	}
	BREG_Write32 (hReg, BCHP_WSE_0_control + ulCoreOffset, ulControlReg);

	eErr = BERR_TRACE (BVBI_P_WSS_Encode_Enable_isr (
		hReg, is656, hwCoreIndex, BFMT_VideoFmt_ePAL_G, bActive));
	if (eErr != BERR_SUCCESS)
		goto done;

done:
	BDBG_LEAVE(BVBI_P_WSS_Enc_Program);
	return eErr;
}

uint32_t BVBI_P_WSS_Encode_Data_isr (
	BREG_Handle hReg,
	bool is656,
	uint8_t hwCoreIndex,
	BAVC_Polarity polarity,
	uint16_t usData)
{
	uint32_t ulCoreOffset;
	uint32_t ulDataReg;
	uint32_t ulErrInfo = 0;

	BDBG_ENTER(BVBI_P_WSS_Encode_Data_isr);

	/* WSS is for top field only */
	if ((polarity != BAVC_Polarity_eTopField) &&
	    (polarity != BAVC_Polarity_eFrame))
	{
		return BVBI_LINE_ERROR_PARITY_CONFLICT;
	}

	/* Figure out which encoder core to use */
	ulCoreOffset = P_GetCoreOffset (is656, hwCoreIndex);
	if (ulCoreOffset == 0xFFFFFFFF)
	{
		/* This should never happen!  This parameter was checked by
		   BVBI_Encode_Create() */
		BDBG_LEAVE(BVBI_P_WSS_Encode_Data_isr);
		return (uint32_t)(-1);
	}

	/* Apply the WSS parity bit */
#ifndef BVBI_P_HAS_WSE_PARITY
	usData = BVBI_P_AddWSSparity (usData);
#endif

	/* Write new register value */
	ulDataReg = BREG_Read32 (hReg, BCHP_WSE_0_wss_data + ulCoreOffset);
	ulDataReg &=
		~BCHP_MASK       (WSE_0_wss_data, wss_data                  );
	ulDataReg |=
		 BCHP_FIELD_DATA (WSE_0_wss_data, wss_data, (uint32_t)usData);
	BREG_Write32 (hReg, BCHP_WSE_0_wss_data + ulCoreOffset, ulDataReg);

	BDBG_LEAVE(BVBI_P_WSS_Encode_Data_isr);
	return ulErrInfo;
}

BERR_Code BVBI_P_WSS_Encode_Enable_isr (
	BREG_Handle hReg,
	bool is656,
	uint8_t hwCoreIndex,
	BFMT_VideoFmt eVideoFormat,
	bool bActive)
{
/*
	Programming note: the implementation here assumes that the bitfield layout
	within registers is the same for all WSS encoder cores in the chip.

	If a chip is built that has multiple WSS encoder cores that are not
	identical, then this routine will have to be redesigned.
*/
	uint32_t ulCoreOffset;
	uint32_t ulControlReg;

	/* TODO: handle progressive video */
	BSTD_UNUSED (eVideoFormat);

	BDBG_ENTER(BVBI_P_WSS_Encode_Enable_isr);

	/* Figure out which encoder core to use */
	ulCoreOffset = P_GetCoreOffset (is656, hwCoreIndex);
	if (ulCoreOffset == 0xFFFFFFFF)
	{
		/* This should never happen!  This parameter was checked by
		   BVBI_Encode_Create() */
		BDBG_LEAVE(BVBI_P_WSS_Encode_Enable_isr);
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

#if (BVBI_P_NUM_WSE_656 >= 1)
	if (!is656)
#endif
	{
		ulControlReg = BREG_Read32 (hReg, BCHP_WSE_0_control + ulCoreOffset);
		ulControlReg &= ~BCHP_MASK       (WSE_0_control, enable   );
		if (bActive)
		{
			ulControlReg |=  BCHP_FIELD_DATA (WSE_0_control, enable, 1);
		}
		else
		{
			ulControlReg |=  BCHP_FIELD_DATA (WSE_0_control, enable, 0);
		}
		BREG_Write32 (hReg, BCHP_WSE_0_control + ulCoreOffset, ulControlReg);
	}

	BDBG_LEAVE(BVBI_P_WSS_Encode_Enable_isr);
	return BERR_SUCCESS;
}


/***************************************************************************
* Static (private) functions
***************************************************************************/

/***************************************************************************
 *
 */
static uint32_t P_GetCoreOffset (bool is656, uint8_t hwCoreIndex)
{
	uint32_t ulCoreOffset = 0xFFFFFFFF;

	if (is656)
	{
#if (BVBI_P_NUM_WSE_656 >= 1)
		ulCoreOffset = (BCHP_WSE_ANCIL_0_control - BCHP_WSE_0_control);
#endif
	}
	else
	{
		switch (hwCoreIndex)
		{
#if (BVBI_P_NUM_WSE >= 1)
		case 0:
			ulCoreOffset = 0;
			break;
#endif
#if (BVBI_P_NUM_WSE >= 2)
		case 1:
			ulCoreOffset = (BCHP_WSE_1_control - BCHP_WSE_0_control);
			break;
#endif
#if (BVBI_P_NUM_WSE >= 3)
		case 2:
			ulCoreOffset = (BCHP_WSE_2_control - BCHP_WSE_0_control);
			break;
#endif
		default:
			break;
		}
	}

	return ulCoreOffset;
}