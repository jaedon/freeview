/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvbi_cce.c $
 * $brcm_Revision: Hydra_Software_Devel/15 $
 * $brcm_Date: 8/10/12 3:31p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vbi/7420/bvbi_cce.c $
 * 
 * Hydra_Software_Devel/15   8/10/12 3:31p pntruong
 * SW7435-276: Updated NTSC/480p to 720x480i/720x480p respectively.
 * Added support 482i/483p with new enums.
 *
 * Hydra_Software_Devel/14   4/24/12 12:03p darnstein
 * SW7405-5643: BVBI API change. BVBI_Encode_Create() now includes a
 * settings argument. It is a data structure that includes an option for
 * ARIB style video output.
 *
 * Hydra_Software_Devel/13   2/20/12 2:53p darnstein
 * SW7425-2434: more detail in error messages.
 *
 * Hydra_Software_Devel/12   2/20/12 12:55p darnstein
 * SW7425-2434: when an unsupported video format is entered, the BDBG
 * error message should be informative.
 *
 * Hydra_Software_Devel/11   10/28/10 6:14p darnstein
 * SW7422-46: Use new SCTE features in CCE hardware core, where available.
 *
 * Hydra_Software_Devel/10   10/1/10 2:35p darnstein
 * SW7422-46: Adapt to 7422 and 7425 chipsets.
 *
 * Hydra_Software_Devel/7   1/14/10 5:41p darnstein
 * SW7400-2659: Lin Ma's register settings for PAL-M closed caption are
 * all in place.
 *
 * Hydra_Software_Devel/6   1/13/10 10:41p darnstein
 * SW7400-2659: for PAL-M, move closed caption up (earlier) by three video
 * lines. This might have to be refined later.
 *
 * Hydra_Software_Devel/5   12/21/09 6:59p darnstein
 * SW7550-120: Add support for SECAM variants.
 *
 * Hydra_Software_Devel/4   4/23/09 5:02p darnstein
 * PR54546: for closed caption output, treat PAL-M just like NTSC.
 *
 * Hydra_Software_Devel/3   3/27/09 7:43p darnstein
 * PR53635: Remove internal ConfigForOthers code. It is obsolete, and it
 * was causing a problem (this PR).
 *
 * Hydra_Software_Devel/2   12/4/08 6:06p darnstein
 * PR45819: 7420 software will now compile, but not link.
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

#if (BVBI_P_NUM_CCE >= 1)
#include "bchp_cce_0.h"  /* RDB info for primary CCE core */
#endif
#if (BVBI_P_NUM_CCE >= 2)
#include "bchp_cce_1.h"   /* RDB info for secondary CCE core */
#endif
#if (BVBI_P_NUM_CCE >= 3)
#include "bchp_cce_2.h"  /* RDB info for tertiary CCE core */
#endif
#if (BVBI_P_NUM_CCE_656 >= 1)
#include "bchp_cce_ancil_0.h" /* RDB info for ITU-R 656 passthrough CCE core */
#endif

BDBG_MODULE(BVBI);

/* Some special aliases for SCTE features */
#if defined(BVBI_P_CCE_VER2)
	#define BCHP_CCE_0_Active_Lines_TOP_ACTIVE_MASK \
		BCHP_CCE_0_Active_Lines_default_TOP_ACTIVE_MASK
	#define BCHP_CCE_0_Active_Lines_TOP_ACTIVE_SHIFT \
		BCHP_CCE_0_Active_Lines_default_TOP_ACTIVE_SHIFT
	#define BCHP_CCE_0_Active_Lines_BOT_ACTIVE_MASK \
		BCHP_CCE_0_Active_Lines_default_BOT_ACTIVE_MASK
	#define BCHP_CCE_0_Active_Lines_BOT_ACTIVE_SHIFT \
		BCHP_CCE_0_Active_Lines_default_BOT_ACTIVE_SHIFT
#endif


/***************************************************************************
* Forward declarations of static (private) functions
***************************************************************************/
static uint8_t BVBI_P_SetOddParityBit( uint8_t uchByte );
static uint32_t P_GetCoreOffset (bool is656, uint8_t hwCoreIndex);


/***************************************************************************
* Implementation supporting closed caption functions that are not in API
***************************************************************************/


/***************************************************************************
 *
 */
void BVBI_P_CC_Enc_Init (BREG_Handle hReg, bool is656, uint8_t hwCoreIndex)
{
	BDBG_ENTER(BVBI_P_CC_Enc_Init);

	BVBI_P_VIE_SoftReset (hReg, is656, hwCoreIndex, BVBI_P_SELECT_CC);

	BDBG_LEAVE(BVBI_P_CC_Enc_Init);
}

/***************************************************************************
 *
 */
uint16_t BVBI_P_SetCCParityBits_isr( uint16_t uchData )
{
	union {
		uint16_t d16;
		uint8_t  d8[2];
	} convenience;

	BDBG_ENTER(BVBI_P_SetCCParityBits_isr);

	convenience.d16 = uchData;
	convenience.d8[0] = BVBI_P_SetOddParityBit (convenience.d8[0]);
	convenience.d8[1] = BVBI_P_SetOddParityBit (convenience.d8[1]);

	BDBG_LEAVE(BVBI_P_SetCCParityBits_isr);
	return convenience.d16;
}

/***************************************************************************
 *
 */
BERR_Code BVBI_P_CC_Enc_Program (
	BREG_Handle hReg,
	bool is656,
	uint8_t hwCoreIndex,
	bool bActive,
	bool bPR18010_bad_line_number,
	BFMT_VideoFmt eVideoFormat,
	bool bArib480p)
{
/*
	Programming note: the implementation here assumes that the bitfield layout
	within registers is the same for all CC encoder cores in the chip.

	If a chip is built that has multiple CC encoder cores that are not
	identical, then this routine will have to be redesigned.
*/
	uint32_t ulControlReg;
	uint32_t ulLinesReg;
	uint32_t ulGain;
	uint32_t ulLineMask;
	uint32_t ulDelayCount;
	uint32_t ulGainDelayReg = 0;
	uint32_t ulCoreOffset;

	BDBG_ENTER(BVBI_P_CC_Enc_Program);

	/* Determine which core to access */
	ulCoreOffset = P_GetCoreOffset (is656, hwCoreIndex);
	if (ulCoreOffset == 0xFFFFFFFF)
	{
		/* This should never happen!  This parameter was checked by
		   BVBI_Encode_Create() */
		BDBG_LEAVE(BVBI_P_CC_Enc_Program);
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* If user wants to turn off closed caption processing, just use the
	   enable bit. */
	if (!bActive)
	{
		ulControlReg =
			BREG_Read32 ( hReg, BCHP_CCE_0_Control + ulCoreOffset );
		ulControlReg &=
			~BCHP_MASK      (CCE_0_Control, ENABLE_CLOSED_CAPTION          );
		ulControlReg |=
			BCHP_FIELD_ENUM (CCE_0_Control, ENABLE_CLOSED_CAPTION, DISABLED);
		BREG_Write32 (
			hReg, BCHP_CCE_0_Control + ulCoreOffset, ulControlReg );
		BDBG_LEAVE(BVBI_P_CC_Enc_Program);
		return BERR_SUCCESS;
	}

	/* Select video format */
	switch (eVideoFormat)
	{
	case BFMT_VideoFmt_eNTSC:
	case BFMT_VideoFmt_eNTSC_J:
	case BFMT_VideoFmt_e720x482_NTSC:
	case BFMT_VideoFmt_e720x482_NTSC_J:
	case BFMT_VideoFmt_ePAL_M:
		/* NTSC specific settings */

		ulGain = 0x47;
		ulDelayCount = 0x3A;
		if (is656)
		{
			ulLineMask = (bPR18010_bad_line_number ? 0x0400 : 0x1000);
		}
		else
		{
			ulLineMask = 0x2000;
		}
		if (eVideoFormat == BFMT_VideoFmt_ePAL_M)
		{
			ulGain = 0x4A;
			ulDelayCount = 0xD;
			ulLineMask >>= 3;
		}
		else
		{
			if (bArib480p)
				ulLineMask >>= 1;
		}
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

		ulGain = 0x46;
		ulDelayCount = 0x01;
		if (is656)
		{
			ulLineMask = (bPR18010_bad_line_number ? 0x4000 : 0x2000);
		}
		else
		{
			ulLineMask = 0x4000;
		}
#if (BVDC_P_3DCOMB_TTD_CONFLICT)
		ulLineMask >>= 1;
#endif

		break;
	default:
		BDBG_LEAVE(BVBI_P_CC_Enc_Program);
		BDBG_ERR(("BVBI_CCE: video format %d not supported", eVideoFormat));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
		break;
	}

	BKNI_EnterCriticalSection();

	/* set the format specific gain delay values, if any. */
	if (!is656)
	{
		ulGainDelayReg = BREG_Read32 (
			hReg, BCHP_CCE_0_Gain_Delay + ulCoreOffset );
		ulGainDelayReg &= ~(
			BCHP_MASK       ( CCE_0_Gain_Delay, GAIN             ) |
			BCHP_MASK       ( CCE_0_Gain_Delay, DELAY_COUNT      )   );
		ulGainDelayReg |= (
			BCHP_FIELD_DATA ( CCE_0_Gain_Delay, GAIN,      ulGain) |
			BCHP_FIELD_DATA ( CCE_0_Gain_Delay, DELAY_COUNT,
			                                            ulDelayCount)   );
		BREG_Write32 (
			hReg, BCHP_CCE_0_Gain_Delay + ulCoreOffset, ulGainDelayReg );
	}

	/* get the active lines register */
	/* Activate lines 22 and 335 for NTSC */
	ulLinesReg =
		BREG_Read32 ( hReg,  BCHP_CCE_0_Active_Lines + ulCoreOffset );
	ulLinesReg &= ~(
		BCHP_MASK       ( CCE_0_Active_Lines, TOP_ACTIVE             ) |
		BCHP_MASK       ( CCE_0_Active_Lines, BOT_ACTIVE             ) );
	ulLinesReg |= (
		BCHP_FIELD_DATA ( CCE_0_Active_Lines, TOP_ACTIVE, ulLineMask ) |
		BCHP_FIELD_DATA ( CCE_0_Active_Lines, BOT_ACTIVE, ulLineMask ) );
	BREG_Write32 (
		hReg, BCHP_CCE_0_Active_Lines + ulCoreOffset, ulLinesReg );

	/* program the control register with non-format specific values */
    ulControlReg = BREG_Read32 ( hReg, BCHP_CCE_0_Control + ulCoreOffset );
	ulControlReg &= ~(
		BCHP_MASK      (CCE_0_Control, reserved0                           )|
		BCHP_MASK      (CCE_0_Control, reserved_for_eco1                   )|
#if defined(BVBI_P_CCE_VER2)
		BCHP_MASK      (CCE_0_Control, SCTE_MODE                           )|
#endif
		/* set parity bits so we provide the parity */
		BCHP_MASK      (CCE_0_Control, TOP_FLD_PARITY                      )|
		BCHP_MASK      (CCE_0_Control, BOT_FLD_PARITY                      )|
		BCHP_MASK      (CCE_0_Control, TOP_FLD_STAT                        )|
		BCHP_MASK      (CCE_0_Control, BOT_FLD_STAT                        )|
		BCHP_MASK      (CCE_0_Control, NULL_CHARACTER                      )|
		BCHP_MASK      (CCE_0_Control, BYTEIF_ENDIAN_ORDER                 )|
		BCHP_MASK      (CCE_0_Control, BYTE_SWAP                           )|
		BCHP_MASK      (CCE_0_Control, SHIFT_DIRECTION                     )|
		BCHP_MASK      (CCE_0_Control, reserved2                           )|
		BCHP_MASK      (CCE_0_Control, REGISTER_USE_MODE                   )|
		BCHP_MASK      (CCE_0_Control, ENABLE_CLOSED_CAPTION               )
	);
	ulControlReg |= (
		BCHP_FIELD_DATA(CCE_0_Control, reserved0,             0            )|
		BCHP_FIELD_DATA(CCE_0_Control, reserved_for_eco1,     0            )|
#if defined(BVBI_P_CCE_VER2)
		BCHP_FIELD_ENUM( CCE_0_Control, SCTE_MODE,            SCTE_OFF     )|
#endif
		/* set parity bits so we provide the parity */
		BCHP_FIELD_ENUM(CCE_0_Control, TOP_FLD_PARITY,        AUTOMATIC    )|
		BCHP_FIELD_ENUM(CCE_0_Control, BOT_FLD_PARITY,        AUTOMATIC    )|
		BCHP_FIELD_DATA(CCE_0_Control, TOP_FLD_STAT,          0            )|
		BCHP_FIELD_DATA(CCE_0_Control, BOT_FLD_STAT,          0            )|
		BCHP_FIELD_DATA(CCE_0_Control, NULL_CHARACTER,        0x80         )|
		BCHP_FIELD_ENUM(CCE_0_Control, BYTEIF_ENDIAN_ORDER,   MAINTAIN     )|
		BCHP_FIELD_ENUM(CCE_0_Control, BYTE_SWAP,             LITTLE_ENDIAN)|
		BCHP_FIELD_ENUM(CCE_0_Control, SHIFT_DIRECTION,       LSB2MSB      )|
		BCHP_FIELD_DATA(CCE_0_Control, reserved2,             0            )|
		BCHP_FIELD_ENUM(CCE_0_Control, REGISTER_USE_MODE,     SPLIT        )|
		BCHP_FIELD_ENUM(CCE_0_Control, ENABLE_CLOSED_CAPTION, ENABLED      )
	);

	/* Format register for PAL vs NTSC */
	ulControlReg &= ~BCHP_MASK ( CCE_0_Control, VIDEO_FORMAT      );
	if ((eVideoFormat == BFMT_VideoFmt_eNTSC  ) ||
	    (eVideoFormat == BFMT_VideoFmt_eNTSC_J) ||
	    (eVideoFormat == BFMT_VideoFmt_e720x482_NTSC) ||
	    (eVideoFormat == BFMT_VideoFmt_e720x482_NTSC_J) ||
	    (eVideoFormat == BFMT_VideoFmt_ePAL_M )   )
	{
		ulControlReg |= BCHP_FIELD_ENUM(CCE_0_Control, VIDEO_FORMAT, NTSC);
	}
	else /* eVideoFormat == BFMT_VideoFmt_ePAL_whatever */
	{
		ulControlReg |= BCHP_FIELD_ENUM(CCE_0_Control, VIDEO_FORMAT, PAL );
	}

	/* Write the finished control register value, finally. */
	BREG_Write32 ( hReg, BCHP_CCE_0_Control + ulCoreOffset, ulControlReg );

	BKNI_LeaveCriticalSection();

	BDBG_LEAVE(BVBI_P_CC_Enc_Program);
	return BERR_SUCCESS;
}

uint32_t BVBI_P_CC_Encode_Data_isr (
	BREG_Handle hReg,
    bool is656,
	uint8_t hwCoreIndex,
	BAVC_Polarity polarity,
	uint16_t usData)
{
	uint8_t  ucRegNum;
	uint16_t usWord0;
	uint32_t ulCoreOffset;
	uint32_t ulReg;
	uint32_t ulRegAddr;
	uint32_t ulErrInfo = 0;

	BDBG_ENTER(BVBI_P_CC_Encode_Data_isr);

	/* Get register offset */
	ulCoreOffset = P_GetCoreOffset (is656, hwCoreIndex);
	if (ulCoreOffset == 0xFFFFFFFF)
	{
		/* This should never happen!  This parameter was checked by
		   BVBI_Encode_Create() */
		BDBG_LEAVE(BVBI_P_CC_Encode_Data_isr);
		return (uint32_t)(-1);
	}

	/* Choose register number */
	ucRegNum =  (uint8_t)(polarity == BAVC_Polarity_eTopField) ? 0 : 3;
	ulRegAddr =
		BCHP_CCE_0_Data0 + ulCoreOffset + (sizeof(uint32_t) * ucRegNum);

	/* Get existing register value */
	ulReg =
		BREG_Read32 (hReg, ulRegAddr);

	/* Encode VBI data back into register value */
	/* usWord0 = BVBI_P_SetCCParityBits_isr (usData); */
	usWord0 = usData;
	ulReg &= ~BCHP_MASK       (CCE_0_Data0, WORD0         );
	ulReg |=  BCHP_FIELD_DATA (CCE_0_Data0, WORD0, usWord0);

	/* Write new register value */
	BREG_Write32 (hReg, ulRegAddr, ulReg);

	BDBG_LEAVE(BVBI_P_CC_Encode_Data_isr);
	return ulErrInfo;
}


/***************************************************************************
 *
 */
BERR_Code BVBI_P_CC_Encode_Enable_isr (
	BREG_Handle hReg,
	bool is656,
	uint8_t hwCoreIndex,
	BFMT_VideoFmt eVideoFormat,
	bool bEnable)
{
	uint32_t ulCoreOffset;
	uint32_t ulControlReg;

	/* TODO: handle progressive video */
	BSTD_UNUSED (eVideoFormat);

	BDBG_ENTER(BVBI_P_CC_Encode_Enable_isr);

	/* Figure out which encoder core to use */
	ulCoreOffset = P_GetCoreOffset (is656, hwCoreIndex);
	if (ulCoreOffset == 0xFFFFFFFF)
	{
		/* This should never happen!  This parameter was checked by
		   BVBI_Encode_Create() */
		BDBG_LEAVE(BVBI_P_CC_Encode_Enable_isr);
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

    ulControlReg = BREG_Read32 ( hReg, BCHP_CCE_0_Control + ulCoreOffset );
	ulControlReg &=
		~BCHP_MASK (CCE_0_Control, ENABLE_CLOSED_CAPTION);
	if (bEnable)
	{
		ulControlReg |=
			BCHP_FIELD_ENUM (CCE_0_Control, ENABLE_CLOSED_CAPTION, ENABLED);
	}
	else
	{
		ulControlReg |=
			BCHP_FIELD_ENUM (CCE_0_Control, ENABLE_CLOSED_CAPTION, DISABLED);
	}
	BREG_Write32 ( hReg, BCHP_CCE_0_Control + ulCoreOffset, ulControlReg );

	BDBG_LEAVE(BVBI_P_CC_Encode_Enable_isr);
	return BERR_SUCCESS;
}


/***************************************************************************
* Static (private) functions
***************************************************************************/

/***************************************************************************
 *
 */
static uint8_t BVBI_P_SetOddParityBit( uint8_t uchByte )
{
    uint8_t uchOriginalByte = uchByte;
    uint8_t uchParity       = 0;
    uint8_t i               = 0;

    /* traverse through all 7 bits to count the number of 1's */
    for (i = 0; i < 7; ++i)
    {
        /* sum current bit and shift */
        uchParity = (uint8_t)(uchParity + (uchByte & 0x1));
        uchByte >>= 1;
    }

    /* return parity with original bits */
    return  (uint8_t)(((uchParity & 0x1) ? 0x00 : 0x80) |
        ((uint8_t)(uchOriginalByte & 0x7F)));
}

/***************************************************************************
 *
 */
static uint32_t P_GetCoreOffset (bool is656, uint8_t hwCoreIndex)
{
	uint32_t ulCoreOffset = 0xFFFFFFFF;

	if (is656)
	{
#if (BVBI_P_NUM_CCE_656_0 >= 1)
		ulCoreOffset = (BCHP_CCE_ANCIL_0_Ancil_RevID - BCHP_CCE_0_RevID);
#endif
	}
	else
	{
		switch (hwCoreIndex)
		{
#if (BVBI_P_NUM_CCE >= 1)
		case 0:
			ulCoreOffset = 0;
			break;
#endif
#if (BVBI_P_NUM_CCE >= 2)
		case 1:
			ulCoreOffset = (BCHP_CCE_1_RevID - BCHP_CCE_0_RevID);
			break;
#endif
#if (BVBI_P_NUM_CCE >= 3)
		case 2:
			ulCoreOffset = (BCHP_CCE_2_RevID - BCHP_CCE_0_RevID);
			break;
#endif
		default:
			break;
		}
	}

	return ulCoreOffset;
}
