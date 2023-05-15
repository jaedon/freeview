/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvbi_mcce.c $
 * $brcm_Revision: Hydra_Software_Devel/18 $
 * $brcm_Date: 8/10/12 3:31p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vbi/7420/bvbi_mcce.c $
 * 
 * Hydra_Software_Devel/18   8/10/12 3:31p pntruong
 * SW7435-276: Updated NTSC/480p to 720x480i/720x480p respectively.
 * Added support 482i/483p with new enums.
 *
 * Hydra_Software_Devel/17   4/24/12 12:03p darnstein
 * SW7405-5643: BVBI API change. BVBI_Encode_Create() now includes a
 * settings argument. It is a data structure that includes an option for
 * ARIB style video output.
 *
 * Hydra_Software_Devel/16   2/20/12 2:53p darnstein
 * SW7425-2434: more detail in error messages.
 *
 * Hydra_Software_Devel/15   2/20/12 12:55p darnstein
 * SW7425-2434: when an unsupported video format is entered, the BDBG
 * error message should be informative.
 *
 * Hydra_Software_Devel/14   3/2/11 6:06p darnstein
 * SW7422-261: getting PAL-M multi-line closed caption right for all
 * chipsets .
 *
 * Hydra_Software_Devel/12   3/2/11 1:48p darnstein
 * SW7422-261: line numbers for PAL-M multi-line closed caption were still
 * wrong.
 *
 * Hydra_Software_Devel/11   2/25/11 2:01p darnstein
 * SW7422-261: change line number, gain, and horizontal offset for
 * waveform in PAL-M video.
 *
 * Hydra_Software_Devel/10   2/14/11 2:37p darnstein
 * SW7422-261: Handle PAL-M the same way that NTSC is handled.
 *
 * Hydra_Software_Devel/9   10/28/10 6:50p darnstein
 * SW7422-46: fix typos in previous check-in.
 *
 * Hydra_Software_Devel/8   10/28/10 6:14p darnstein
 * SW7422-46: Use new SCTE features in CCE hardware core, where available.
 *
 * Hydra_Software_Devel/7   10/1/10 2:38p darnstein
 * SW7422-46: Adapt to 7422 and 7425 chipsets.
 *
 * Hydra_Software_Devel/4   12/21/09 7:00p darnstein
 * SW7550-120: Add support for SECAM variants.
 *
 * Hydra_Software_Devel/3   12/2/09 5:52p darnstein
 * SW7401-4348: put in missing break statement in switch block.
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


/***************************************************************************
* Forward declarations of static (private) functions
***************************************************************************/
static uint32_t P_GetCoreOffset (bool is656, uint8_t hwCoreIndex);


/***************************************************************************
* Implementation supporting closed caption functions that are not in API
***************************************************************************/

BERR_Code BVBI_P_MCC_Enc_Program (
	BREG_Handle hReg,
	bool is656,
	uint8_t hwCoreIndex,
	bool bActive,
	BFMT_VideoFmt eVideoFormat,
	bool bArib480p)
{
/*
	Programming note: the implementation here assumes that the bitfield layout
	within registers is the same for all CC encoder cores in the chip.

	If a chip is built that has multiple CC encoder cores that are not
	identical, then this routine will have to be redesigned.
*/
	uint32_t ulCoreOffset;
	uint32_t ulControlReg;
	uint32_t ulGainDelayReg = 0;
	uint32_t ulGain = 0;
	uint32_t ulDelayCount = 0;
#if defined(BVBI_P_CCE_VER2)
	uint32_t ulScteBaseReg = 0;
	uint32_t ulLineTop = 0;
	uint32_t ulLineBot = 0;
#endif

	BDBG_ENTER(BVBI_P_MCC_Enc_Program);

#if !defined(BVBI_P_CCE_VER2)
	BSTD_UNUSED (bArib480p);
#endif

	/* Figure out which encoder core to use */
	ulCoreOffset = P_GetCoreOffset (is656, hwCoreIndex);
	if (ulCoreOffset == 0xFFFFFFFF)
	{
		/* This should never happen!  This parameter was checked by
		   BVBI_Encode_Create() */
		BDBG_LEAVE(BVBI_P_MCC_Enc_Program);
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
		BDBG_LEAVE(BVBI_P_MCC_Enc_Program);
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
		if (!is656)
		{
			ulGain = 0x47;
			ulDelayCount = 0x3A;
		}

		/* PAL_M always seems to have an issue */
		if (eVideoFormat == BFMT_VideoFmt_ePAL_M)
		{
			ulGain = 0x4A;
			ulDelayCount = 0xD;
		}
#if defined(BVBI_P_CCE_VER2)
		else
		{
			ulLineTop = 8;
			ulLineBot = 271;
			if (bArib480p)
			{
				--ulLineTop;
				--ulLineBot;
			}
		}
#endif
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
		/* set the format specific gain delay values, if any. */
		if (!is656)
		{
			ulGain = 0x46;
			ulDelayCount = 0x01;
		}
#if defined(BVBI_P_CCE_VER2)
		/* SCTE line origin */
		ulLineTop = 5;
		ulLineBot = 318;
#endif
		break;

	default:
		BKNI_LeaveCriticalSection();
		BDBG_LEAVE(BVBI_P_MCC_Enc_Program);
		BDBG_ERR(("BVBI_MCCE: video format %d not supported", eVideoFormat));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
		break;
	}

	BKNI_EnterCriticalSection();

	/* write the gain delay register, if any. */
	if (!is656)
	{
		ulGainDelayReg = BREG_Read32 (
			hReg, BCHP_CCE_0_Gain_Delay + ulCoreOffset );
		ulGainDelayReg &= ~(
			BCHP_MASK       ( CCE_0_Gain_Delay, GAIN                     ) |
			BCHP_MASK       ( CCE_0_Gain_Delay, DELAY_COUNT              )   );
		ulGainDelayReg |= (
			BCHP_FIELD_DATA ( CCE_0_Gain_Delay, GAIN,              ulGain) |
			BCHP_FIELD_DATA ( CCE_0_Gain_Delay, DELAY_COUNT, ulDelayCount)   );
		BREG_Write32 (
			hReg, BCHP_CCE_0_Gain_Delay + ulCoreOffset, ulGainDelayReg );
	}

#if defined(BVBI_P_CCE_VER2)
	/* Write the SCTE base lines register */
	ulScteBaseReg = BREG_Read32 (
		hReg, BCHP_CCE_0_SCTE_Base_Lines + ulCoreOffset );
	ulScteBaseReg &= ~(
		BCHP_MASK       ( CCE_0_SCTE_Base_Lines, LINE_TOP           ) |
		BCHP_MASK       ( CCE_0_SCTE_Base_Lines, LINE_BOT           )   );
	ulScteBaseReg |= (
		BCHP_FIELD_DATA ( CCE_0_SCTE_Base_Lines, LINE_TOP, ulLineTop) |
		BCHP_FIELD_DATA ( CCE_0_SCTE_Base_Lines, LINE_BOT, ulLineBot)   );
	BREG_Write32 (
		hReg, BCHP_CCE_0_SCTE_Base_Lines +ulCoreOffset, ulScteBaseReg );
#endif

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
		BCHP_FIELD_ENUM( CCE_0_Control, SCTE_MODE,            SCTE_ON      )|
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
	    (eVideoFormat == BFMT_VideoFmt_ePAL_M )  )
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

	BDBG_LEAVE(BVBI_P_MCC_Enc_Program);
	return BERR_SUCCESS;
}

uint32_t BVBI_P_MCC_Encode_Data_isr (
	BREG_Handle hReg,
	bool is656,
	uint8_t hwCoreIndex,
	BFMT_VideoFmt eVideoFormat,
	bool bArib480p,
	BAVC_Polarity polarity,
	bool bPR18010_bad_line_number,
	BVBI_MCCData* pMCCData)
{
	int iLine;
	unsigned int numSet;
	uint8_t *datum;
	uint8_t  ucRegNum;
	uint16_t usWord;
	uint32_t ulCoreOffset;
	uint32_t ulDataReg;
	uint32_t ulDataRegAddr;
	uint32_t ulLinesRegAddr;
	uint32_t ulLinesReg;
	uint32_t lineMask;
	uint32_t ulErrInfo = 0;

	BDBG_ENTER(BVBI_P_MCC_Encode_Data_isr);

#if (BVBI_P_NUM_CCE_656 >= 1)
#else
	BSTD_UNUSED (bPR18010_bad_line_number);
#endif
#if defined(BVBI_P_CCE_VER2)
	BSTD_UNUSED (bArib480p);
#endif

	/* Get register offset */
	ulCoreOffset = P_GetCoreOffset (is656, hwCoreIndex);
	if (ulCoreOffset == 0xFFFFFFFF)
	{
		/* This should never happen!  This parameter was checked by
		   BVBI_Encode_Create() */
		BDBG_LEAVE(BVBI_P_MCC_Encode_Data_isr);
		return (uint32_t)(-1);
	}

	/* Choose first data register */
	ucRegNum =  (uint8_t)(polarity == BAVC_Polarity_eTopField) ? 0 : 3;
	ulDataRegAddr =
		BCHP_CCE_0_Data0 + ulCoreOffset + (sizeof(uint32_t) * ucRegNum);

	/* Prepare to index into user data */
	datum = pMCCData->ucData;
	numSet = 0;
	lineMask = pMCCData->uhLineMask;
	/* PAL-M always seems to cause problems */
	if (eVideoFormat == BFMT_VideoFmt_ePAL_M)
		lineMask >>= 3;

/* Old CCE core does not have the line base register bitfield */
#if !defined(BVBI_P_CCE_VER2)
	if (bArib480p)
	{
		if ((eVideoFormat == BFMT_VideoFmt_eNTSC)   ||
		    (eVideoFormat == BFMT_VideoFmt_eNTSC_J) ||
		    (eVideoFormat == BFMT_VideoFmt_e720x482_NTSC) ||
		    (eVideoFormat == BFMT_VideoFmt_e720x482_NTSC_J))
		{
			lineMask >>= 1;
		}
	}
#endif

	/* Loop over possible video lines */
	for (iLine = 0 ; iLine < 16 ; ++iLine)
	{
		/* If current video line is selected */
		if (lineMask & 0x1)
		{
			/* Get existing data register value */
			ulDataReg = BREG_Read32 (hReg, ulDataRegAddr);

			/* Encode VBI data back into register value */
			usWord  = *datum++;
			usWord |= ((uint16_t)(*datum++) << 8);
			/* usWord = BVBI_P_SetCCParityBits_isr (usWord); */
			if (numSet & 0x1)
			{
				ulDataReg &= ~BCHP_MASK       (CCE_0_Data0, WORD1        );
				ulDataReg |=  BCHP_FIELD_DATA (CCE_0_Data0, WORD1, usWord);
			}
			else
			{
				ulDataReg &= ~BCHP_MASK       (CCE_0_Data0, WORD0        );
				ulDataReg |=  BCHP_FIELD_DATA (CCE_0_Data0, WORD0, usWord);
			}

			/* Write the data register back */
			BREG_Write32 (hReg, ulDataRegAddr, ulDataReg);

			/* Advance counters */
			if (numSet & 0x1)
			{
				++ucRegNum;
				ulDataRegAddr =
					BCHP_CCE_0_Data0 + ulCoreOffset +
						(sizeof(uint32_t) * ucRegNum);
			}
			++numSet;

#if !defined(BVBI_P_CCE_VER2)
			/* Sanity check */
			if (numSet > 6)
				break;
#endif
		}

		/* Advance */
		lineMask >>= 1;
	}

	/* Adjust line mask for hardware oddities */
	lineMask = pMCCData->uhLineMask;
	/* PAL-M always seems to cause problems */
	if (eVideoFormat == BFMT_VideoFmt_ePAL_M)
		lineMask >>= 3;
#if (BVBI_P_NUM_CCE_656 >= 1)
	if (is656)
	{
		if (bPR18010_bad_line_number)
			lineMask >>= 3;
		else
			lineMask >>= 1;
	}
#endif
#if !defined(BVBI_P_CCE_VER2)
	if (bArib480p)
	{
		if ((eVideoFormat == BFMT_VideoFmt_eNTSC)   ||
		    (eVideoFormat == BFMT_VideoFmt_eNTSC_J) ||
		    (eVideoFormat == BFMT_VideoFmt_e720x482_NTSC) ||
		    (eVideoFormat == BFMT_VideoFmt_e720x482_NTSC_J))
		{
			lineMask >>= 1;
		}
	}
#endif

	/* Finally, set active lines register. */
#if defined(BVBI_P_CCE_VER2) /** { **/
	switch (polarity)
	{
	case BAVC_Polarity_eTopField:
		ulLinesRegAddr = BCHP_CCE_0_Active_Lines + ulCoreOffset;
		ulLinesReg =
			BREG_Read32 ( hReg,  ulLinesRegAddr );
		ulLinesReg &= ~BCHP_MASK       (
			CCE_0_Active_Lines, SCTE_ON_SCTE_TOP_ACTIVE);
		ulLinesReg |=  BCHP_FIELD_DATA (
			CCE_0_Active_Lines, SCTE_ON_SCTE_TOP_ACTIVE, lineMask );
			break;
	case BAVC_Polarity_eBotField:
		ulLinesRegAddr = BCHP_CCE_0_Active_Lines_1 + ulCoreOffset;
		ulLinesReg =
			BREG_Read32 ( hReg,  ulLinesRegAddr );
		ulLinesReg &= ~BCHP_MASK       (
			CCE_0_Active_Lines_1, SCTE_BOT_ACTIVE);
		ulLinesReg |=  BCHP_FIELD_DATA (
			CCE_0_Active_Lines_1, SCTE_BOT_ACTIVE, lineMask );
			break;
		default:
			BDBG_LEAVE(BVBI_P_MCC_Encode_Data_isr);
			return (uint32_t)(-1);
			break;
	}
	BREG_Write32 ( hReg, ulLinesRegAddr, ulLinesReg );
#else /** } !BVBI_P_CCE_VER2 { **/
	ulLinesRegAddr = BCHP_CCE_0_Active_Lines + ulCoreOffset;
	ulLinesReg =
		BREG_Read32 ( hReg,  ulLinesRegAddr );
	switch (polarity)
	{
	case BAVC_Polarity_eTopField:
		ulLinesReg &= ~BCHP_MASK       ( CCE_0_Active_Lines, TOP_ACTIVE);
		ulLinesReg |=  BCHP_FIELD_DATA ( CCE_0_Active_Lines, TOP_ACTIVE,
										 lineMask );
			break;
	case BAVC_Polarity_eBotField:
		ulLinesReg &= ~BCHP_MASK       ( CCE_0_Active_Lines, BOT_ACTIVE);
		ulLinesReg |=  BCHP_FIELD_DATA ( CCE_0_Active_Lines, BOT_ACTIVE,
										 lineMask );
			break;
		default:
			BDBG_LEAVE(BVBI_P_MCC_Encode_Data_isr);
			return (uint32_t)(-1);
			break;
	}
	BREG_Write32 ( hReg, ulLinesRegAddr, ulLinesReg );
#endif /** } BVBI_P_CCE_VER2 **/

	BDBG_LEAVE(BVBI_P_MCC_Encode_Data_isr);
	return ulErrInfo;
}

/***************************************************************************
 *
 */
BERR_Code BVBI_P_MCC_Encode_Enable_isr (
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

	BDBG_ENTER(BVBI_P_MCC_Encode_Enable_isr);

	/* Figure out which encoder core to use */
	ulCoreOffset = P_GetCoreOffset (is656, hwCoreIndex);
	if (ulCoreOffset == 0xFFFFFFFF)
	{
		/* This should never happen!  This parameter was checked by
		   BVBI_Encode_Create() */
		BDBG_LEAVE(BVBI_P_MCC_Encode_Enable_isr);
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

	BDBG_LEAVE(BVBI_P_MCC_Encode_Enable_isr);
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
#if (BVBI_P_NUM_CCE_656 >= 1)
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
