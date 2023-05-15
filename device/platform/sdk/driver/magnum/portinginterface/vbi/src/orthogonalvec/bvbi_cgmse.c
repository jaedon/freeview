/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvbi_cgmse.c $
 * $brcm_Revision: Hydra_Software_Devel/21 $
 * $brcm_Date: 8/10/12 3:31p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vbi/7420/bvbi_cgmse.c $
 * 
 * Hydra_Software_Devel/21   8/10/12 3:31p pntruong
 * SW7435-276: Updated NTSC/480p to 720x480i/720x480p respectively.
 * Added support 482i/483p with new enums.
 *
 * Hydra_Software_Devel/20   4/24/12 12:03p darnstein
 * SW7405-5643: BVBI API change. BVBI_Encode_Create() now includes a
 * settings argument. It is a data structure that includes an option for
 * ARIB style video output.
 *
 * Hydra_Software_Devel/19   2/20/12 2:53p darnstein
 * SW7425-2434: more detail in error messages.
 *
 * Hydra_Software_Devel/18   2/20/12 12:55p darnstein
 * SW7425-2434: when an unsupported video format is entered, the BDBG
 * error message should be informative.
 *
 * Hydra_Software_Devel/17   3/24/11 4:08p darnstein
 * SW7420-1693: For 480P video with VEC double rate (54 MHz sampling) CGMS
 * rise times need to change.
 *
 * Hydra_Software_Devel/16   3/22/11 12:57p darnstein
 * SW7420-1693: CGMS for 480P video needs to take "VEC double rate
 * sampling" into account.
 *
 * Hydra_Software_Devel/15   1/20/11 4:37p darnstein
 * SW7335-1139: CGMS-B header was bit-backwards.
 *
 * Hydra_Software_Devel/14   3/10/10 5:04p darnstein
 * SW7420-598: implement Lin's settings for CGMS-A and CGMS-B. Only
 * applies to chips with orthogonal VEC.
 *
 * Hydra_Software_Devel/13   3/3/10 8:07p darnstein
 * SW7420-598: Adjust #ifdefs that select capabilities CGMS-B, CGMS-Bd.
 *
 * Hydra_Software_Devel/12   3/3/10 2:45p darnstein
 * SW7420-598: Implement Lin Ma's recommendations for tuning. This is a
 * port of a similar check-in made to the 7400 branch.
 *
 * Hydra_Software_Devel/10   2/22/10 5:50p darnstein
 * SW7420-539: back out the line number adjustment for 1080I CGMS. New VEC
 * IT microcode makes this unnecessary.
 *
 * Hydra_Software_Devel/9   2/22/10 5:22p darnstein
 * SW7335-666: port over changes from 7335 work. Tuning for 480P, etc.
 *
 * Hydra_Software_Devel/8   1/18/10 5:32p darnstein
 * SW7420-548: New register settings from Lin Ma for 480P, CGMS-Bd.
 *
 * Hydra_Software_Devel/8   1/18/10 5:30p darnstein
 * SW7420-548: New register settings from Lin Ma for 480P, CGMS-Bd.
 *
 * Hydra_Software_Devel/6   11/11/09 6:39p darnstein
 * SW3556-848: newer chipsets have BIT_ORDER option for CGMS-A encoding.
 *
 * Hydra_Software_Devel/5   3/27/09 7:43p darnstein
 * PR53635: Remove internal ConfigForOthers code. It is obsolete, and it
 * was causing a problem (this PR).
 *
 * Hydra_Software_Devel/4   2/9/09 8:25p darnstein
 * PR45819: no choice of CRC method in 7420-Ax? Might have to revisit
 * this.
 *
 * Hydra_Software_Devel/3   2/9/09 7:58p darnstein
 * PR45819: minor update for 7420 and CGMSAE core: version numbers.
 *
 * Hydra_Software_Devel/2   12/4/08 6:06p darnstein
 * PR45819: 7420 software will now compile, but not link.
 *
 * Hydra_Software_Devel/1   12/3/08 8:01p darnstein
 * PR45819:
 *
 ***************************************************************************/

#include "bstd.h"             /* standard types */
#include "bdbg.h"             /* Dbglib */
#include "bkni.h"			  /* For critical sections */
#include "bvbi.h"             /* VBI processing, this module. */
#include "bvbi_priv.h"        /* VBI internal data structures */
#include "bavc_hdmi.h"

#if (BVBI_P_NUM_CGMSAE >= 1)
#include "bchp_cgmsae_0.h" /* RDB info for primary CGMSE core */
#endif
#if (BVBI_P_NUM_CGMSAE >= 2)
#include "bchp_cgmsae_1.h"  /* RDB info for secondary CGMSE core */
#endif
#if (BVBI_P_NUM_CGMSAE >= 3)
#include "bchp_cgmsae_2.h"  /* RDB info for tertiary CGMSE core */
#endif

BDBG_MODULE(BVBI);

/***************************************************************************
* Forward declarations of static (private) functions
***************************************************************************/

static uint32_t P_GetCoreOffset (bool is656, uint8_t hwCoreIndex);
#ifdef P_CGMS_SOFTWARE_CRC
static uint32_t P_CalculateCRC (uint32_t  ulData);
#endif


/***************************************************************************
* Implementation of supporting CGMS functions that are not in API
***************************************************************************/

void BVBI_P_CGMS_Enc_Init (BREG_Handle hReg, bool is656, uint8_t hwCoreIndex)
{
	BDBG_ENTER(BVBI_P_CGMS_Enc_Init);

	BVBI_P_VIE_SoftReset (hReg, is656, hwCoreIndex, BVBI_P_SELECT_CGMSA);

	BDBG_LEAVE(BVBI_P_CGMS_Enc_Init);
}

BERR_Code BVBI_P_CGMSA_Enc_Program (
	BREG_Handle hReg,
	bool is656,
	uint8_t hwCoreIndex,
	bool bActive,
	BFMT_VideoFmt eVideoFormat,
	bool bArib480p,
	BAVC_HDMI_PixelRepetition ePixRep)
{
/*
	Programming note: the implementation here assumes that the bitfield layout
	within registers is the same for all CGMS encoder cores in the chip.

	If a chip is built that has multiple CGMS encoder cores that are not
	identical, then this routine will have to be redesigned.
*/
	uint32_t ulCoreOffset;
	uint32_t ulTop_FormatReg;
	uint32_t ulBot_FormatReg;
	uint32_t ulTop_ControlReg;
	uint32_t ulBot_ControlReg;

	uint32_t rise_time;
	uint32_t gain;
	uint32_t pulse_width;
	uint32_t init_delay;
	uint32_t top_line;
	uint32_t bot_line;
	uint32_t line_start;
	uint32_t enable_top;
	uint32_t enable_bot;

	BERR_Code eErr = BERR_SUCCESS;

	BDBG_ENTER(BVBI_P_CGMSA_Enc_Program);
	BSTD_UNUSED (ePixRep);

	/* Figure out which encoder core to use */
	ulCoreOffset = P_GetCoreOffset (is656, hwCoreIndex);
	if (ulCoreOffset == 0xFFFFFFFF)
	{
		/* This should never happen!  This parameter was checked by
		   BVBI_Encode_Create() */
		BDBG_LEAVE(BVBI_P_CGMSA_Enc_Program);
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* Determine some tuning parameters according to video standard */
	switch (eVideoFormat)
	{
	case BFMT_VideoFmt_eNTSC:
	case BFMT_VideoFmt_eNTSC_J:
	case BFMT_VideoFmt_e720x482_NTSC:
	case BFMT_VideoFmt_e720x482_NTSC_J:
		rise_time   = BCHP_CGMSAE_0_Top_Control_RISE_TIME_MED;
		gain        = 0x62;
		pulse_width = 0x1E3;
		init_delay  = 0x55;
		top_line    = 20;
		bot_line    = 283 - 256;
		if (bArib480p)
		{
			--top_line;
			--bot_line;
		}
		line_start  = BCHP_CGMSAE_0_Bot_Control_VBI_START_LINE256;
#ifdef P_CGMS_SOFTWARE_CRC
		enable_top = BCHP_CGMSAE_0_Bot_Control_ENABLE_RAW_DATA;
		enable_bot = BCHP_CGMSAE_0_Bot_Control_ENABLE_RAW_DATA;
#else
		enable_top = BCHP_CGMSAE_0_Bot_Control_ENABLE_CGMSA;
		enable_bot = BCHP_CGMSAE_0_Bot_Control_ENABLE_CGMSA;
#endif
		break;

	case BFMT_VideoFmt_e1080i:
	case BFMT_VideoFmt_e1080i_50Hz:
		rise_time   = BCHP_CGMSAE_0_Top_Control_RISE_TIME_FAST;
		gain        = 0x67;
		pulse_width = 0x268;
		init_delay  = 0xAF;
		top_line    = 19;
		bot_line    = 582 - 544;
		line_start  = BCHP_CGMSAE_0_Bot_Control_VBI_START_LINE544;
#ifdef P_CGMS_SOFTWARE_CRC
		enable_top = BCHP_CGMSAE_0_Bot_Control_ENABLE_RAW_DATA;
		enable_bot = BCHP_CGMSAE_0_Bot_Control_ENABLE_RAW_DATA;
#else
		enable_top = BCHP_CGMSAE_0_Bot_Control_ENABLE_CGMSA;
		enable_bot = BCHP_CGMSAE_0_Bot_Control_ENABLE_CGMSA;
#endif
		break;

	case BFMT_VideoFmt_e720p:
	case BFMT_VideoFmt_e720p_50Hz:
		rise_time   = BCHP_CGMSAE_0_Top_Control_RISE_TIME_FAST;
		gain        = 0x67;
		pulse_width = 0x1D0;
		init_delay  = 0xB;
		top_line    = 24;
		bot_line    = 0;
		line_start  = 0;
#ifdef P_CGMS_SOFTWARE_CRC
		enable_top  = BCHP_CGMSAE_0_Bot_Control_ENABLE_RAW_DATA;
#else
		enable_top  = BCHP_CGMSAE_0_Bot_Control_ENABLE_CGMSA;
#endif
		enable_bot  = BCHP_CGMSAE_0_Bot_Control_ENABLE_DISABLED;
		break;

	case BFMT_VideoFmt_e576p_50Hz:
		/* TODO: tune these settings */
		rise_time   = 1;
		gain        = 0x62;
		pulse_width = 0x86;
		init_delay  = 0x26;
		top_line    = 43;
		bot_line    = 0;
		line_start  = 0;
		enable_top  = BCHP_CGMSAE_0_Bot_Control_ENABLE_RAW_DATA;
		enable_bot  = BCHP_CGMSAE_0_Bot_Control_ENABLE_DISABLED;
		break;

	case BFMT_VideoFmt_e480p:
	case BFMT_VideoFmt_e720x483p:
		/* VEC is operating at double rate (54 MHz sampling) */
		rise_time   = BCHP_CGMSAE_0_Top_Control_RISE_TIME_MED;
		gain        = 0x67;
		pulse_width = 0x1A0;
		init_delay  = 0x3E;
		top_line    = 41;
		bot_line    = 0;
		if (bArib480p)
		{
			--top_line;
		}
		line_start  = 0;
#ifdef P_CGMS_SOFTWARE_CRC
		enable_top  = BCHP_CGMSAE_0_Bot_Control_ENABLE_RAW_DATA;
#else
		enable_top  = BCHP_CGMSAE_0_Bot_Control_ENABLE_CGMSA;
#endif
		enable_bot  = BCHP_CGMSAE_0_Bot_Control_ENABLE_DISABLED;
		break;

	default:
		/* Shut up with the compiler warnings */
		rise_time   = 0;
		gain        = 0x0;
		pulse_width = 0;
		init_delay  = 0;
		top_line    = 0;
		bot_line    = 0;
		line_start  = 0;
		enable_top  = BCHP_CGMSAE_0_Bot_Control_ENABLE_DISABLED;
		enable_bot  = BCHP_CGMSAE_0_Bot_Control_ENABLE_DISABLED;
		if (bActive)
		{
			BDBG_ERR(("BVBI_CGMSE: video format %d not supported",
				eVideoFormat));
			return BERR_TRACE (BVBI_ERR_VFMT_CONFLICT);
		}
		break;
	}

	BKNI_EnterCriticalSection();

	/* Read control registers */
	ulTop_ControlReg =
		BREG_Read32 ( hReg, BCHP_CGMSAE_0_Top_Control + ulCoreOffset );
	ulBot_ControlReg =
		BREG_Read32 ( hReg, BCHP_CGMSAE_0_Bot_Control + ulCoreOffset );

	/* If enabling encoding */
	if (bActive)
	{
		/* Fill in the control registers */
		ulTop_ControlReg &= ~(
			BCHP_MASK       ( CGMSAE_0_Top_Control, RISE_TIME            ) |
			BCHP_MASK       ( CGMSAE_0_Top_Control, GAIN                 ) |
			BCHP_MASK       ( CGMSAE_0_Top_Control, RAW_COUNT            ) |
			BCHP_MASK       ( CGMSAE_0_Top_Control, VBI_LINE             ) |
#if defined(BVBI_P_CGMSAE_VER1) || defined(BVBI_P_CGMSAE_VER3) || \
	defined(BVBI_P_CGMSAE_VER5)
			BCHP_MASK       ( CGMSAE_0_Top_Control, BIT_ORDER            ) |
#endif
			BCHP_MASK       ( CGMSAE_0_Top_Control, ENABLE               ) );
		ulTop_ControlReg |= (
			BCHP_FIELD_DATA ( CGMSAE_0_Top_Control, RISE_TIME, rise_time ) |
			BCHP_FIELD_DATA ( CGMSAE_0_Top_Control, GAIN,           gain ) |
			BCHP_FIELD_DATA ( CGMSAE_0_Top_Control, RAW_COUNT,        21 ) |
			BCHP_FIELD_DATA ( CGMSAE_0_Top_Control, VBI_LINE,   top_line ) |
#if defined(BVBI_P_CGMSAE_VER1) || defined(BVBI_P_CGMSAE_VER3) || \
	defined(BVBI_P_CGMSAE_VER5)
			BCHP_FIELD_ENUM ( CGMSAE_0_Top_Control, BIT_ORDER, LSB_FIRST ) |
#endif
			BCHP_FIELD_DATA ( CGMSAE_0_Top_Control, ENABLE,   enable_top ) );
		ulBot_ControlReg &= ~(
			BCHP_MASK       ( CGMSAE_0_Bot_Control, GAIN                 ) |
			BCHP_MASK       ( CGMSAE_0_Bot_Control, RAW_COUNT            ) |
			BCHP_MASK       ( CGMSAE_0_Bot_Control, VBI_LINE             ) |
			BCHP_MASK       ( CGMSAE_0_Bot_Control, VBI_START            ) |
#if defined(BVBI_P_CGMSAE_VER1) || defined(BVBI_P_CGMSAE_VER3) || \
	defined(BVBI_P_CGMSAE_VER5)
			BCHP_MASK       ( CGMSAE_0_Bot_Control, BIT_ORDER            ) |
#endif
			BCHP_MASK       ( CGMSAE_0_Bot_Control, ENABLE               ) );
		ulBot_ControlReg |= (
			BCHP_FIELD_DATA ( CGMSAE_0_Bot_Control, GAIN,           gain ) |
			BCHP_FIELD_DATA ( CGMSAE_0_Bot_Control, RAW_COUNT,        21 ) |
			BCHP_FIELD_DATA ( CGMSAE_0_Bot_Control, VBI_LINE,   bot_line ) |
			BCHP_FIELD_DATA ( CGMSAE_0_Bot_Control, VBI_START,line_start ) |
#if defined(BVBI_P_CGMSAE_VER1) || defined(BVBI_P_CGMSAE_VER3) || \
	defined(BVBI_P_CGMSAE_VER5)
			BCHP_FIELD_ENUM ( CGMSAE_0_Bot_Control, BIT_ORDER, LSB_FIRST ) |
#endif
			BCHP_FIELD_DATA ( CGMSAE_0_Bot_Control, ENABLE,   enable_bot ) );

		/* Program the format registers */
		ulTop_FormatReg =
			BREG_Read32 ( hReg, BCHP_CGMSAE_0_Top_Format + ulCoreOffset );
		ulTop_FormatReg &= ~(
			BCHP_MASK       ( CGMSAE_0_Top_Format, PULSE_WIDTH       ) |
			BCHP_MASK       ( CGMSAE_0_Top_Format, INIT_DELAY        ) );
		ulTop_FormatReg |= (
			BCHP_FIELD_DATA ( CGMSAE_0_Top_Format, PULSE_WIDTH,
															pulse_width ) |
			BCHP_FIELD_DATA ( CGMSAE_0_Top_Format, INIT_DELAY,
															 init_delay ) );
		BREG_Write32 (
			hReg, BCHP_CGMSAE_0_Top_Format + ulCoreOffset, ulTop_FormatReg );
		ulBot_FormatReg =
			BREG_Read32 ( hReg, BCHP_CGMSAE_0_Bot_Format + ulCoreOffset );
		ulBot_FormatReg &= ~(
			BCHP_MASK       ( CGMSAE_0_Bot_Format, PULSE_WIDTH       ) |
			BCHP_MASK       ( CGMSAE_0_Bot_Format, INIT_DELAY        ) );
		ulBot_FormatReg |= (
			BCHP_FIELD_DATA ( CGMSAE_0_Bot_Format, PULSE_WIDTH,
															pulse_width ) |
			BCHP_FIELD_DATA ( CGMSAE_0_Bot_Format, INIT_DELAY,
															 init_delay ) );
		BREG_Write32 (
			hReg, BCHP_CGMSAE_0_Bot_Format + ulCoreOffset, ulBot_FormatReg );
	}
	else /* Disable encoding */
	{
		ulTop_ControlReg &=
			~BCHP_MASK       ( CGMSAE_0_Top_Control, ENABLE           );
		ulTop_ControlReg |=
			 BCHP_FIELD_ENUM ( CGMSAE_0_Top_Control, ENABLE, DISABLED );
		ulBot_ControlReg &=
			~BCHP_MASK       ( CGMSAE_0_Bot_Control, ENABLE           );
		ulBot_ControlReg |=
			 BCHP_FIELD_ENUM ( CGMSAE_0_Bot_Control, ENABLE, DISABLED );
	}

	/* Write the finished control register values */
	BREG_Write32 (
		hReg, BCHP_CGMSAE_0_Top_Control + ulCoreOffset, ulTop_ControlReg );
	BREG_Write32 (
		hReg, BCHP_CGMSAE_0_Bot_Control + ulCoreOffset, ulBot_ControlReg );

	BKNI_LeaveCriticalSection();

	BDBG_LEAVE(BVBI_P_CGMSA_Enc_Program);
	return eErr;
}

uint32_t BVBI_P_CGMSA_Encode_Data_isr (
	BREG_Handle hReg,
	bool is656,
	uint8_t hwCoreIndex,
	BAVC_Polarity polarity,
	uint32_t ulData)
{
	uint32_t ulCoreOffset;
	uint32_t ulErrInfo = 0;

	/* Debug code
	static uint32_t debugcounter = 0;
	++debugcounter;
	*/

	BDBG_ENTER(BVBI_P_CGMSA_Encode_Data_isr);

	/* Get register offset */
	ulCoreOffset = P_GetCoreOffset (is656, hwCoreIndex);
	if (ulCoreOffset == 0xFFFFFFFF)
	{
		/* Should never happen */
		BDBG_LEAVE(BVBI_P_CGMSA_Encode_Data_isr);
		return 0xFFFFFFFF;
	}

	/* If top field */
	if ((polarity == BAVC_Polarity_eTopField) ||
	    (polarity == BAVC_Polarity_eFrame   )   )
	{
		/* Write new register value */
		BREG_Write32 (
			hReg, BCHP_CGMSAE_0_Top_Data + ulCoreOffset, ulData );

		/* Debug code
		if ((debugcounter > 200) && (debugcounter <= 220))
			printf ("Wrote %08x to top field\n", ulData);
		*/
	}
	else /* polarity == BAVC_Polarity_eBotField */
	{
		/* Write new register value */
		BREG_Write32 (
			hReg, BCHP_CGMSAE_0_Bot_Data + ulCoreOffset, ulData );

		/* Debug code
		if ((debugcounter > 200) && (debugcounter <= 220))
			printf ("Wrote %08x to bottom field\n", ulData);
		*/
	}

	BDBG_LEAVE(BVBI_P_CGMSA_Encode_Data_isr);
	return ulErrInfo;
}

/***************************************************************************
 *
 */
BERR_Code BVBI_P_CGMSA_Encode_Enable_isr (
	BREG_Handle hReg,
	bool is656,
	uint8_t hwCoreIndex,
	BFMT_VideoFmt eVideoFormat,
	bool bEnable)
{
	uint32_t ulCoreOffset;
	uint32_t ulControlReg;
	uint32_t enable_top;
	uint32_t enable_bot;

	BDBG_ENTER(BVBI_P_CGMSA_Encode_Enable_isr);

	/* Figure out which encoder core to use */
	ulCoreOffset = P_GetCoreOffset (is656, hwCoreIndex);
	if (ulCoreOffset == 0xFFFFFFFF)
	{
		/* This should never happen!  This parameter was checked by
		   BVBI_Encode_Create() */
		BDBG_LEAVE(BVBI_P_CGMSA_Encode_Enable_isr);
		BDBG_ERR(("Invalid parameter\n"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* Determine a tuning parameter according to video standard */
	switch (eVideoFormat)
	{
	case BFMT_VideoFmt_eNTSC:
	case BFMT_VideoFmt_eNTSC_J:
	case BFMT_VideoFmt_e720x482_NTSC:
	case BFMT_VideoFmt_e720x482_NTSC_J:
#ifdef P_CGMS_SOFTWARE_CRC
		enable_top = BCHP_CGMSAE_0_Bot_Control_ENABLE_RAW_DATA;
		enable_bot = BCHP_CGMSAE_0_Bot_Control_ENABLE_RAW_DATA;
#else
		enable_top = BCHP_CGMSAE_0_Bot_Control_ENABLE_CGMSA;
		enable_bot = BCHP_CGMSAE_0_Bot_Control_ENABLE_CGMSA;
#endif
		break;

	case BFMT_VideoFmt_e1080i:
#ifdef P_CGMS_SOFTWARE_CRC
		enable_top = BCHP_CGMSAE_0_Bot_Control_ENABLE_RAW_DATA;
		enable_bot = BCHP_CGMSAE_0_Bot_Control_ENABLE_RAW_DATA;
#else
		enable_top = BCHP_CGMSAE_0_Bot_Control_ENABLE_CGMSA;
		enable_bot = BCHP_CGMSAE_0_Bot_Control_ENABLE_CGMSA;
#endif
		break;

	case BFMT_VideoFmt_e1080i_50Hz:
#ifdef P_CGMS_SOFTWARE_CRC
		enable_top = BCHP_CGMSAE_0_Bot_Control_ENABLE_RAW_DATA;
		enable_bot = BCHP_CGMSAE_0_Bot_Control_ENABLE_RAW_DATA;
#else
		enable_top = BCHP_CGMSAE_0_Bot_Control_ENABLE_CGMSA;
		enable_bot = BCHP_CGMSAE_0_Bot_Control_ENABLE_CGMSA;
#endif
		break;

	case BFMT_VideoFmt_e720p:
#ifdef P_CGMS_SOFTWARE_CRC
		enable_top  = BCHP_CGMSAE_0_Bot_Control_ENABLE_RAW_DATA;
#else
		enable_top  = BCHP_CGMSAE_0_Bot_Control_ENABLE_CGMSA;
#endif
		enable_bot  = BCHP_CGMSAE_0_Bot_Control_ENABLE_DISABLED;
		break;

	case BFMT_VideoFmt_e720p_50Hz:
#ifdef P_CGMS_SOFTWARE_CRC
		enable_top  = BCHP_CGMSAE_0_Bot_Control_ENABLE_RAW_DATA;
#else
		enable_top  = BCHP_CGMSAE_0_Bot_Control_ENABLE_CGMSA;
#endif
		enable_bot  = BCHP_CGMSAE_0_Bot_Control_ENABLE_DISABLED;
		break;

	case BFMT_VideoFmt_e480p:
	case BFMT_VideoFmt_e720x483p:
#ifdef P_CGMS_SOFTWARE_CRC
		enable_top  = BCHP_CGMSAE_0_Bot_Control_ENABLE_RAW_DATA;
#else
		enable_top  = BCHP_CGMSAE_0_Bot_Control_ENABLE_CGMSA;
#endif
		enable_bot  = BCHP_CGMSAE_0_Bot_Control_ENABLE_DISABLED;
		break;

	case BFMT_VideoFmt_e576p_50Hz:
		enable_top  = BCHP_CGMSAE_0_Bot_Control_ENABLE_RAW_DATA;
		enable_bot  = BCHP_CGMSAE_0_Bot_Control_ENABLE_DISABLED;
		break;

	default:
		/* Shut up with the compiler warning */
		enable_top  = BCHP_CGMSAE_0_Bot_Control_ENABLE_DISABLED;
		enable_bot  = BCHP_CGMSAE_0_Bot_Control_ENABLE_DISABLED;
		if (bEnable)
		{
			BDBG_ERR(("BVBI_CGMSE: video format %d not supported",
				eVideoFormat));
			return BERR_TRACE (BVBI_ERR_VFMT_CONFLICT);
		}
		break;
	}

    ulControlReg =
		BREG_Read32 (hReg, BCHP_CGMSAE_0_Top_Control + ulCoreOffset);
	ulControlReg &=
		~BCHP_MASK (CGMSAE_0_Top_Control, ENABLE);
	if (bEnable)
	{
		ulControlReg |=
			BCHP_FIELD_DATA (CGMSAE_0_Top_Control, ENABLE, enable_top);
	}
	else
	{
		ulControlReg |=
			BCHP_FIELD_ENUM (CGMSAE_0_Top_Control, ENABLE, DISABLED);
	}
	BREG_Write32 (hReg, BCHP_CGMSAE_0_Top_Control + ulCoreOffset, ulControlReg);

    ulControlReg =
		BREG_Read32 (hReg, BCHP_CGMSAE_0_Bot_Control + ulCoreOffset);
	ulControlReg &=
		~BCHP_MASK (CGMSAE_0_Bot_Control, ENABLE);
	if (bEnable)
	{
		ulControlReg |=
			BCHP_FIELD_DATA (CGMSAE_0_Bot_Control, ENABLE, enable_bot);
	}
	else
	{
		ulControlReg |=
			BCHP_FIELD_ENUM (CGMSAE_0_Bot_Control, ENABLE, DISABLED);
	}
	BREG_Write32 (
		hReg, BCHP_CGMSAE_0_Bot_Control + ulCoreOffset, ulControlReg);

	BDBG_LEAVE(BVBI_P_CGMSA_Encode_Enable_isr);
	return BERR_SUCCESS;
}

#if defined(BVBI_P_CGMSAE_VER2) || defined(BVBI_P_CGMSAE_VER3) || \
	defined(BVBI_P_CGMSAE_VER5) /** { **/

/***************************************************************************
 *
 */
uint32_t BVBI_P_CGMSB_Encode_Data_isr (
	BREG_Handle hReg,
	bool is656,
	uint8_t hwCoreIndex,
	BAVC_Polarity polarity,
	BVBI_CGMSB_Datum cgmsbDatum )
{
	uint32_t ulCoreOffset;
	uint32_t ulUpdate;
	uint32_t ulErrInfo = 0;

	BDBG_ENTER(BVBI_P_CGMSB_Encode_Data_isr);

	/* Get register offset */
	ulCoreOffset = P_GetCoreOffset (is656, hwCoreIndex);
	if (ulCoreOffset == 0xFFFFFFFF)
	{
		/* Should never happen */
		BDBG_LEAVE(BVBI_P_CGMSB_Encode_Data_isr);
		return 0xFFFFFFFF;
	}

	/* If top field */
	if ((polarity == BAVC_Polarity_eTopField) ||
	    (polarity == BAVC_Polarity_eFrame   )   )
	{
		/* Write new register values */
		BREG_Write32 (
			hReg, BCHP_CGMSAE_0_Top_Data_B0 + ulCoreOffset, cgmsbDatum[0] );
		BREG_Write32 (
			hReg, BCHP_CGMSAE_0_Top_Data_B1 + ulCoreOffset, cgmsbDatum[1] );
		BREG_Write32 (
			hReg, BCHP_CGMSAE_0_Top_Data_B2 + ulCoreOffset, cgmsbDatum[2] );
		BREG_Write32 (
			hReg, BCHP_CGMSAE_0_Top_Data_B3 + ulCoreOffset, cgmsbDatum[3] );
		BREG_Write32 (
			hReg, BCHP_CGMSAE_0_Top_Data_B4 + ulCoreOffset, cgmsbDatum[4] );
		ulUpdate  = BCHP_FIELD_DATA (CGMSAE_0_Reg_updt, TOP, 1);
	}
	else /* polarity == BAVC_Polarity_eBotField */
	{
		/* Write new register values */
		BREG_Write32 (
			hReg, BCHP_CGMSAE_0_Bot_Data_B0 + ulCoreOffset, cgmsbDatum[0] );
		BREG_Write32 (
			hReg, BCHP_CGMSAE_0_Bot_Data_B1 + ulCoreOffset, cgmsbDatum[1] );
		BREG_Write32 (
			hReg, BCHP_CGMSAE_0_Bot_Data_B2 + ulCoreOffset, cgmsbDatum[2] );
		BREG_Write32 (
			hReg, BCHP_CGMSAE_0_Bot_Data_B3 + ulCoreOffset, cgmsbDatum[3] );
		BREG_Write32 (
			hReg, BCHP_CGMSAE_0_Bot_Data_B4 + ulCoreOffset, cgmsbDatum[4] );
		ulUpdate  = BCHP_FIELD_DATA (CGMSAE_0_Reg_updt, BOT, 1);
	}

	/* Tell hardware we are done */
	BREG_Write32 (hReg, BCHP_CGMSAE_0_Reg_updt + ulCoreOffset, ulUpdate);

	BDBG_LEAVE(BVBI_P_CGMSB_Encode_Data_isr);
	return ulErrInfo;
}


/***************************************************************************
 *
 */
BERR_Code BVBI_P_CGMSB_Enc_Program (
	BREG_Handle hReg,
	bool is656,
	uint8_t hwCoreIndex,
	bool bActive,
	BFMT_VideoFmt eVideoFormat,
	bool bArib480p,
	BAVC_HDMI_PixelRepetition ePixRep,
	bool bCea805dStyle)
{
/*
	Programming note: the implementation here assumes that the bitfield layout
	within registers is the same for all CGMS encoder cores in the chip.

	If a chip is built that has multiple CGMS encoder cores that are not
	identical, then this routine will have to be redesigned.
*/
	uint32_t ulCoreOffset;
	uint32_t ulTop_FormatReg;
	uint32_t ulBot_FormatReg;
	uint32_t ulTop_ControlReg;
	uint32_t ulBot_ControlReg;

	uint32_t rise_time;
	uint32_t gain;
	uint32_t pulse_width;
	uint32_t init_delay;
	uint32_t top_line;
	uint32_t bot_line;
	uint32_t line_start;
	uint32_t enable_top;
	uint32_t enable_bot;
#if !defined(BVBI_P_CGMSAE_VER4)
	uint32_t bit_order;
#endif
#if !defined(BVBI_P_CGMSAE_VER2)
	uint32_t crc_meth;
#endif

	BERR_Code eErr = BERR_SUCCESS;

	BDBG_ENTER(BVBI_P_CGMSB_Enc_Program);
	BSTD_UNUSED (ePixRep);

#if defined(BVBI_P_CGMSAE_VER2)
	BSTD_UNUSED (bCea805dStyle);
#endif

	/* Figure out which encoder core to use */
	ulCoreOffset = P_GetCoreOffset (is656, hwCoreIndex);
	if (ulCoreOffset == 0xFFFFFFFF)
	{
		/* This should never happen!  This parameter was checked by
		   BVBI_Encode_Create() */
		BDBG_LEAVE(BVBI_P_CGMSB_Enc_Program);
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* Determine some tuning parameters according to video standard */
	switch (eVideoFormat)
	{
	case BFMT_VideoFmt_eNTSC:
	case BFMT_VideoFmt_eNTSC_J:
	case BFMT_VideoFmt_e720x482_NTSC:
	case BFMT_VideoFmt_e720x482_NTSC_J:
		rise_time   = BCHP_CGMSAE_0_Top_Control_B_RISE_TIME_MED;
		gain        = 0x62;
		pulse_width = 0x1E3;
		init_delay  = 0x55;
		top_line    = 20;
		bot_line    = 283 - 256;
		if (bArib480p)
		{
			--top_line;
			--bot_line;
		}
		line_start  = BCHP_CGMSAE_0_Bot_Control_B_VBI_START_LINE256;
#ifdef P_CGMS_SOFTWARE_CRC
		enable_top = BCHP_CGMSAE_0_Top_Control_B_ENABLE_RAW_DATA;
		enable_bot = BCHP_CGMSAE_0_Bot_Control_B_ENABLE_RAW_DATA;
#else
		enable_top = BCHP_CGMSAE_0_Top_Control_B_ENABLE_CGMSA;
		enable_bot = BCHP_CGMSAE_0_Bot_Control_B_ENABLE_CGMSA;
#endif
		break;

	case BFMT_VideoFmt_e1080i:
	case BFMT_VideoFmt_e1080i_50Hz:
		rise_time   = 2;
		gain        = 0x5F;
		pulse_width = 0x54;
		init_delay  = 0xAF;
		top_line    = 18;
		bot_line    = 581 - 544;
		line_start  = BCHP_CGMSAE_0_Bot_Control_B_VBI_START_LINE544;
#ifdef P_CGMS_SOFTWARE_CRC
		enable_top = BCHP_CGMSAE_0_Top_Control_B_ENABLE_RAW_DATA;
		enable_bot = BCHP_CGMSAE_0_Bot_Control_B_ENABLE_RAW_DATA;
#else
		enable_top = BCHP_CGMSAE_0_Top_Control_B_ENABLE_CGMSA;
		enable_bot = BCHP_CGMSAE_0_Bot_Control_B_ENABLE_CGMSA;
#endif
		break;

	case BFMT_VideoFmt_e720p:
	case BFMT_VideoFmt_e720p_50Hz:
		rise_time   = BCHP_CGMSAE_0_Top_Control_B_RISE_TIME_FAST_37;
		gain        = 0x68;
		pulse_width = 0x47;
		init_delay  = 0x10;
		top_line    = 23;
		bot_line    = 0;
		line_start  = 0;
#ifdef P_CGMS_SOFTWARE_CRC
		enable_top  = BCHP_CGMSAE_0_Top_Control_B_ENABLE_RAW_DATA;
#else
		enable_top  = BCHP_CGMSAE_0_Top_Control_B_ENABLE_CGMSA;
#endif
		enable_bot  = BCHP_CGMSAE_0_Bot_Control_B_ENABLE_DISABLED;
		break;

	case BFMT_VideoFmt_e576p_50Hz:
		/* TODO: tune these settings */
		rise_time   = 1;
		gain        = 0x62;
		pulse_width = 0x86;
		init_delay  = 0x26;
		top_line    = 43;
		bot_line    = 0;
		line_start  = 0;
		enable_top  = BCHP_CGMSAE_0_Top_Control_B_ENABLE_RAW_DATA;
		enable_bot  = BCHP_CGMSAE_0_Bot_Control_B_ENABLE_DISABLED;
		break;

	case BFMT_VideoFmt_e480p:
	case BFMT_VideoFmt_e720x483p:
		/* VEC is operating at double rate (54 MHz) */
		rise_time   = BCHP_CGMSAE_0_Top_Control_B_RISE_TIME_MED_37;
		gain        = 0x62;
		pulse_width = 0x40;
		init_delay  = 0x4C;
		top_line    = 40;
		bot_line    = 0;
		if (bArib480p)
		{
			--top_line;
		}
		line_start  = 0;
#ifdef P_CGMS_SOFTWARE_CRC
		enable_top  = BCHP_CGMSAE_0_Top_Control_B_ENABLE_RAW_DATA;
#else
		enable_top  = BCHP_CGMSAE_0_Top_Control_B_ENABLE_CGMSA;
#endif
		enable_bot  = BCHP_CGMSAE_0_Bot_Control_B_ENABLE_DISABLED;
		break;

	default:
		/* Shut up with the compiler warnings */
		rise_time   = 0;
		gain        = 0x0;
		pulse_width = 0;
		init_delay  = 0;
		top_line    = 0;
		bot_line    = 0;
		line_start  = 0;
		enable_top  = BCHP_CGMSAE_0_Top_Control_B_ENABLE_DISABLED;
		enable_bot  = BCHP_CGMSAE_0_Bot_Control_B_ENABLE_DISABLED;
		if (bActive)
		{
			BDBG_ERR(("BVBI_CGMSE: video format %d not supported",
				eVideoFormat));
			return BERR_TRACE (BVBI_ERR_VFMT_CONFLICT);
		}
		break;
	}

	/* Make the choice defined in CEA-805-D */
#if defined(BVBI_P_CGMSAE_VER3) || defined(BVBI_P_CGMSAE_VER5)/** { **/
/* Being careful */
#if BCHP_CGMSAE_0_Top_Control_BIT_ORDER_MSB_FIRST != \
	BCHP_CGMSAE_0_Bot_Control_BIT_ORDER_MSB_FIRST
	#error Programming error
#endif
#if BCHP_CGMSAE_0_Top_Control_BIT_ORDER_LSB_FIRST != \
	BCHP_CGMSAE_0_Bot_Control_BIT_ORDER_LSB_FIRST
	#error Programming error
#endif
#endif /** } **/
#if BCHP_CGMSAE_0_Top_Control_B_CRC_METHOD_METHOD1 != \
    BCHP_CGMSAE_0_Bot_Control_B_CRC_METHOD_METHOD1
	#error Programming error
#endif
#if BCHP_CGMSAE_0_Top_Control_B_CRC_METHOD_METHOD2 != \
    BCHP_CGMSAE_0_Bot_Control_B_CRC_METHOD_METHOD2
	#error Programming error
#endif
	if (bCea805dStyle)
	{
		crc_meth = BCHP_CGMSAE_0_Top_Control_B_CRC_METHOD_METHOD2;
	}
	else
	{
		crc_meth = BCHP_CGMSAE_0_Top_Control_B_CRC_METHOD_METHOD1;
	}
#if !defined(BVBI_P_CGMSAE_VER4)
		bit_order = BCHP_CGMSAE_0_Top_Control_BIT_ORDER_MSB_FIRST;
#endif

	BKNI_EnterCriticalSection();

	/* Read control registers */
	ulTop_ControlReg =
		BREG_Read32 ( hReg, BCHP_CGMSAE_0_Top_Control_B + ulCoreOffset );
	ulBot_ControlReg =
		BREG_Read32 ( hReg, BCHP_CGMSAE_0_Bot_Control_B + ulCoreOffset );

	/* If enabling encoding */
	if (bActive)
	{
		/* Fill in the control registers */
		ulTop_ControlReg &= ~(
			BCHP_MASK       (CGMSAE_0_Top_Control_B, RISE_TIME           ) |
			BCHP_MASK       (CGMSAE_0_Top_Control_B, GAIN                ) |
			BCHP_MASK       (CGMSAE_0_Top_Control_B, RAW_COUNT           ) |
			BCHP_MASK       (CGMSAE_0_Top_Control_B, VBI_LINE            ) |
#if !defined(BVBI_P_CGMSAE_VER4)
			BCHP_MASK       (CGMSAE_0_Top_Control_B, BIT_ORDER           ) |
#endif
#if !defined(BVBI_P_CGMSAE_VER4)
			BCHP_MASK       (CGMSAE_0_Top_Control_B, CRC_METHOD          ) |
#endif
			BCHP_MASK       (CGMSAE_0_Top_Control_B, ENABLE              ) );
		ulTop_ControlReg |= (
			BCHP_FIELD_DATA (CGMSAE_0_Top_Control_B, RISE_TIME, rise_time) |
			BCHP_FIELD_DATA (CGMSAE_0_Top_Control_B, GAIN,           gain) |
			BCHP_FIELD_DATA (CGMSAE_0_Top_Control_B, RAW_COUNT,        21) |
			BCHP_FIELD_DATA (CGMSAE_0_Top_Control_B, VBI_LINE,   top_line) |
#if !defined(BVBI_P_CGMSAE_VER4)
			BCHP_FIELD_DATA (CGMSAE_0_Top_Control_B, BIT_ORDER, bit_order) |
#endif
#if !defined(BVBI_P_CGMSAE_VER4)
			BCHP_FIELD_DATA (CGMSAE_0_Top_Control_B, CRC_METHOD, crc_meth) |
#endif
			BCHP_FIELD_DATA (CGMSAE_0_Top_Control_B, ENABLE,   enable_top) );
		ulBot_ControlReg &= ~(
			BCHP_MASK       (CGMSAE_0_Bot_Control_B, GAIN                ) |
			BCHP_MASK       (CGMSAE_0_Bot_Control_B, RAW_COUNT           ) |
			BCHP_MASK       (CGMSAE_0_Bot_Control_B, VBI_LINE            ) |
			BCHP_MASK       (CGMSAE_0_Bot_Control_B, VBI_START           ) |
#if !defined(BVBI_P_CGMSAE_VER4)
			BCHP_MASK       (CGMSAE_0_Bot_Control_B, BIT_ORDER           ) |
#endif
#if !defined(BVBI_P_CGMSAE_VER4)
			BCHP_MASK       (CGMSAE_0_Bot_Control_B, CRC_METHOD          ) |
#endif
			BCHP_MASK       (CGMSAE_0_Bot_Control_B, ENABLE              ) );
		ulBot_ControlReg |= (
			BCHP_FIELD_DATA (CGMSAE_0_Bot_Control_B, GAIN,           gain) |
			BCHP_FIELD_DATA (CGMSAE_0_Bot_Control_B, RAW_COUNT,        21) |
			BCHP_FIELD_DATA (CGMSAE_0_Bot_Control_B, VBI_LINE,   bot_line) |
			BCHP_FIELD_DATA (CGMSAE_0_Bot_Control_B, VBI_START,line_start) |
#if !defined(BVBI_P_CGMSAE_VER4)
			BCHP_FIELD_DATA (CGMSAE_0_Bot_Control_B, BIT_ORDER, bit_order) |
#endif
#if !defined(BVBI_P_CGMSAE_VER4)
			BCHP_FIELD_DATA (CGMSAE_0_Bot_Control_B, CRC_METHOD, crc_meth) |
#endif
			BCHP_FIELD_DATA (CGMSAE_0_Bot_Control_B, ENABLE,   enable_bot) );

		/* Program the format registers */
		ulTop_FormatReg =
			BREG_Read32 ( hReg, BCHP_CGMSAE_0_Top_Format_B + ulCoreOffset);
		ulTop_FormatReg &= ~(
			BCHP_MASK       (CGMSAE_0_Top_Format_B,      HEADER      ) |
			BCHP_MASK       (CGMSAE_0_Top_Format_B, PULSE_WIDTH      ) |
			BCHP_MASK       (CGMSAE_0_Top_Format_B,  INIT_DELAY      ) );
		ulTop_FormatReg |= (
			BCHP_FIELD_DATA (CGMSAE_0_Top_Format_B,      HEADER, 0x32) |
			BCHP_FIELD_DATA (CGMSAE_0_Top_Format_B, PULSE_WIDTH,
															pulse_width ) |
			BCHP_FIELD_DATA (CGMSAE_0_Top_Format_B, INIT_DELAY,
															 init_delay ) );
		BREG_Write32 (
			hReg, BCHP_CGMSAE_0_Top_Format_B + ulCoreOffset,
			ulTop_FormatReg);
		ulBot_FormatReg =
			BREG_Read32 (hReg, BCHP_CGMSAE_0_Bot_Format_B + ulCoreOffset);
		ulBot_FormatReg &= ~(
			BCHP_MASK       (CGMSAE_0_Bot_Format_B,      HEADER     ) |
			BCHP_MASK       (CGMSAE_0_Bot_Format_B, PULSE_WIDTH     ) |
			BCHP_MASK       (CGMSAE_0_Bot_Format_B, INIT_DELAY      ) );
		ulBot_FormatReg |= (
			BCHP_FIELD_DATA (CGMSAE_0_Bot_Format_B,      HEADER, 0x32) |
			BCHP_FIELD_DATA (CGMSAE_0_Bot_Format_B, PULSE_WIDTH,
															pulse_width) |
			BCHP_FIELD_DATA (CGMSAE_0_Bot_Format_B, INIT_DELAY,
															 init_delay) );
		BREG_Write32 (
			hReg, BCHP_CGMSAE_0_Bot_Format_B + ulCoreOffset,
			ulBot_FormatReg );
	}
	else /* Disable encoding */
	{
		ulTop_ControlReg &=
			~BCHP_MASK       (CGMSAE_0_Top_Control_B, ENABLE          );
		ulTop_ControlReg |=
			 BCHP_FIELD_ENUM (CGMSAE_0_Top_Control_B, ENABLE, DISABLED);
		ulBot_ControlReg &=
			~BCHP_MASK       (CGMSAE_0_Bot_Control_B, ENABLE          );
		ulBot_ControlReg |=
			 BCHP_FIELD_ENUM (CGMSAE_0_Bot_Control_B, ENABLE, DISABLED);
	}

	/* Write the finished control register values */
	BREG_Write32 (
		hReg, BCHP_CGMSAE_0_Top_Control_B + ulCoreOffset, ulTop_ControlReg);
	BREG_Write32 (
		hReg, BCHP_CGMSAE_0_Bot_Control_B + ulCoreOffset, ulBot_ControlReg);

	BKNI_LeaveCriticalSection();

	BDBG_LEAVE(BVBI_P_CGMSB_Enc_Program);
	return eErr;
}

#endif /** } **/


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
#if (BVBI_P_NUM_CGMSAE_656 >= 1)
		/* No CGMSA 656 encoder */
#endif
	}
	else
	{
		switch (hwCoreIndex)
		{
#if (BVBI_P_NUM_CGMSAE >= 1)
		case 0:
			ulCoreOffset = 0;
			break;
#endif
#if (BVBI_P_NUM_CGMSAE >= 2)
		case 1:
			ulCoreOffset = (BCHP_CGMSAE_1_RevID - BCHP_CGMSAE_0_RevID);
			break;
#endif
#if (BVBI_P_NUM_CGMSAE >= 3)
		case 2:
			ulCoreOffset = (BCHP_CGMSAE_2_RevID - BCHP_CGMSAE_0_RevID);
			break;
#endif
		default:
			break;
		}
	}

	return ulCoreOffset;
}

#ifdef P_CGMS_SOFTWARE_CRC
/**********************************************************************func*
 * P_CalculateCRC
 *
 * Calculates the CRC value for CGMS data. This was done due to replace
 * the calculation done by hardware.
 *
 * Tests:
 *   0x141 -> CRC of 0x3d
 *   0x1c0 -> CRC of 0x0b
 *   0x0c1 -> CRC of 0x24
 */
static uint32_t P_CalculateCRC (uint32_t  ulData)
{
	int     i;
	uint32_t  ulGate;
	uint32_t  ulCRC = (((uint32_t)1) << 6) - 1;  /* initially set to all 1s */
	uint32_t  ulGatePolynomial;

	/* mask out any current CRC */
	ulData &= (((uint32_t)1)<<14) - 1;

	/* traverse through all data bits */
	for (i=0; i<14; ++i)
	{
		/* calculate 1 bit gate value */
		ulGate = ((ulData >> i) ^ (ulCRC)) & 0x1;

		/* gate set? */
		if (ulGate)
		{
			/* use polynomial */
			ulGatePolynomial = 0x10;

		/* gate not set */
		} else
		{
			/* don't use gate polynomial */
			ulGatePolynomial = 0x0;
		}

		/* calculate new CRC */
		ulCRC = ((ulCRC >> 1) ^ ulGatePolynomial) | (ulGate << 5);
	}

	/* return data with calculated CRC */
	return ulData | (ulCRC << 14);
}
#endif
