/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvbi_ttd.c $
 * $brcm_Revision: Hydra_Software_Devel/5 $
 * $brcm_Date: 8/10/12 3:30p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vbi/7400/bvbi_ttd.c $
 * 
 * Hydra_Software_Devel/5   8/10/12 3:30p pntruong
 * SW7435-276: Updated NTSC/480p to 720x480i/720x480p respectively.
 * Added support 482i/483p with new enums.
 *
 * Hydra_Software_Devel/4   8/2/12 1:18p darnstein
 * SW7125-1311: with regards to teletext, handle PAL-M in same way as NTSC
 * and NTSC-J.
 *
 * Hydra_Software_Devel/3   12/21/09 7:06p darnstein
 * SW7550-120: Add support for SECAM variants.
 *
 * Hydra_Software_Devel/2   12/3/08 7:57p darnstein
 * PR45819: New, more modular form of most BVBI source files.
 *
 * Hydra_Software_Devel/43   7/30/08 8:40p darnstein
 * PR43926,PR43413: Workaround for TTD addressing error in hardware.
 * Decodes at most one line of teletext per video field. Will not
 * corrrupt memory if input signal actually contains more teletext.
 *
 * Hydra_Software_Devel/42   7/30/08 5:41p darnstein
 * PR43926: A basic solution to the memory addressing problem is in place.
 * It can decode one line of teletext. If more teletext is present, then
 * memory corruption will occur. This danger will be eliminated next.
 *
 * Hydra_Software_Devel/41   5/12/08 2:56p darnstein
 * PR42416: Implement Grossman's work-around for the conflict between VDEC
 * frame comb filter and teletext decoder (TTD).
 *
 * Hydra_Software_Devel/40   4/2/08 1:34p darnstein
 * PR38531,PR40702:
 * 1. Use magic line mask setting discovered by Brad Delanghe.
 * 2. Change line numbering.
 *
 * Hydra_Software_Devel/39   3/11/08 6:52p darnstein
 * PR23201: Change start_delay of teletext decoder core to 0, in the case
 * of PAL video input. Per Brad Grossman.
 *
 * Hydra_Software_Devel/38   1/18/08 1:09p darnstein
 * PR32758,PR38531: Fix bug in programming decoder black level register.
 * Thank you Ming Dei.
 *
 * Hydra_Software_Devel/37   1/11/08 4:37p darnstein
 * PR32758,PR38531: Corrected black_level setting (thank you hardware
 * architect).
 *
 * Hydra_Software_Devel/36   1/2/08 2:27p darnstein
 * PR32758: use PAL decoder settings from Brad Grossman.
 *
 * Hydra_Software_Devel/35   1/17/07 5:32p darnstein
 * PR26464: correctly handle teletext output to multiple VECs
 *
 * Hydra_Software_Devel/34   1/2/07 4:21p darnstein
 * PR26872: Mechanically add SECAM to all cases where PAL formats are
 * accepted.
 *
 * Hydra_Software_Devel/33   8/31/06 2:09p darnstein
 * PR23869: clean up the handling of multiple VECs and VDECs.
 *
 * Hydra_Software_Devel/32   8/18/06 6:51p darnstein
 * PR23178: basic compile on 93563 is possible.
 *
 * Hydra_Software_Devel/31   2/21/06 6:20p darnstein
 * PR18343: Move top line for teletext output up raster by 1 (numerically
 * smaller by 1), but ONLY for PAL.
 *
 * Hydra_Software_Devel/30   1/16/06 12:40p darnstein
 * PR18903: Use BSTD_UNUSED macro to suppress compiler warnings in
 * VxWorks.
 *
 * Hydra_Software_Devel/29   12/22/05 5:53p darnstein
 * PR18234: use some filter_phase_adjustment settings provided by Brad
 * Grossman.
 *
 * Hydra_Software_Devel/28   9/23/05 2:47p darnstein
 * PR13750: Proper use of BERR_TRACE and BERR_CODEs.
 *
 * Hydra_Software_Devel/27   9/19/05 2:57p darnstein
 * PR17151: Check for chip name where needed. Also, convert to new scheme
 * for testing chip revisions (BCHP_VER).
 *
 * Hydra_Software_Devel/26   6/15/05 12:03p darnstein
 * PR 14487: bring up VBI decoding on 97038-C1.
 *
 * Hydra_Software_Devel/25   4/14/05 12:01p darnstein
 * PR 14720: For each reference to BCHP_REV_B[01], add a BCHP_REV_B2.
 * Still untested.
 *
 * Hydra_Software_Devel/24   4/4/05 11:53a darnstein
 * PR 13952: BCM97038-C0 bringup.
 *
 * Hydra_Software_Devel/23   3/11/05 3:49p darnstein
 * PR 14426: use new _0 names for VDEC cores.
 *
 * Hydra_Software_Devel/22   1/5/05 4:27p jasonh
 * PR 13700: Fixed VBI compile issues for 7038 C0.
 *
 * Hydra_Software_Devel/21   9/21/04 2:02p darnstein
 * PR 12728: Replace a line of code that was too long for some compilers
 * to handle.
 *
 * Hydra_Software_Devel/20   7/16/04 7:07p darnstein
 * PR 9080: merge in 656 input and output work. Some testing and debugging
 * remains to be done.
 *
 * Hydra_Software_Devel/I656/1   7/8/04 7:45p darnstein
 * ITU-R 656 decoding of VBI seems to be ready for bring up.  Expect bugs.
 *
 * Hydra_Software_Devel/19   6/17/04 5:57p darnstein
 * PR 9080: get decoding working for 7038-B0 hardware.
 *
 * Hydra_Software_Devel/18   5/24/04 5:08p jasonh
 * PR 11189: Merge down from B0 to main-line
 *
 * Hydra_Software_Devel/Refsw_Devel_7038_B0/1   4/19/04 2:45p darnstein
 * PR 9080: Compilation is possible with 7038 B0. Correct execution is not
 * likely.
 *
 * Hydra_Software_Devel/17   4/2/04 6:42p darnstein
 * PR 9080: Allow NTSC-J video format.
 *
 * Hydra_Software_Devel/16   3/26/04 1:47p darnstein
 * PR 9080: remove some kludgey code regarding choice of top or bottom
 * field.
 *
 * Hydra_Software_Devel/15   3/18/04 11:15a darnstein
 * PR 9080: commented out some debugging code.
 *
 * Hydra_Software_Devel/14   3/12/04 5:52p darnstein
 * PR 9080: Teletext for NTSC is working, with a kludge for an A0 hardware
 * flaw.  There is a lot of debug code that will be removed later.
 *
 * Hydra_Software_Devel/13   3/4/04 4:28p darnstein
 * PR 9080: improve allocation of teletext private data.
 *
 * Hydra_Software_Devel/12   3/2/04 4:41p darnstein
 * PR 9080: many fixes and additions discovered during bring-up.
 *
 * Hydra_Software_Devel/11   2/27/04 6:10p darnstein
 * PR 9080: handle all of the PAL formats.  Fix misuse of BDBG_ASSERT
 * macro.
 *
 * Hydra_Software_Devel/10   2/19/04 2:51p darnstein
 * PR 9493: Use new PAL format enums.
 *
 * Hydra_Software_Devel/9   2/6/04 11:50a darnstein
 * PR 9080: better handle VBI decoding errors.
 *
 * Hydra_Software_Devel/8   2/3/04 4:43p darnstein
 * PR 9080: fix logic errors in handling decoding errors. A few other
 * minor fixes.
 *
 * Hydra_Software_Devel/7   1/23/04 4:08p darnstein
 * PR 9080: Routines used in (pseudo) ISRs have no critical sections.
 *
 * Hydra_Software_Devel/6   12/19/03 5:07p darnstein
 * PR 9080: adapt to changed BAVC enum for field polarity.
 *
 * Hydra_Software_Devel/5   10/16/03 4:26p darnstein
 * Use revised #define BAVC_SourceId_Vdec0.
 *
 * Hydra_Software_Devel/4   10/16/03 1:21p darnstein
 * Fix usage of BDBG_ENTER(), BDBG_LEAVE().
 *
 * Hydra_Software_Devel/3   9/29/03 5:00p darnstein
 * Put in critical sections.
 *
 * Hydra_Software_Devel/2   9/25/03 4:48p darnstein
 * BVBI module is mostly complete. The only things that I know are missing
 * are the critical sections, 656 support, macrovision support, and LOTS
 * OF TESTING.
 *
 * Hydra_Software_Devel/1   9/22/03 5:43p darnstein
 * Teletext specific code to support VBI decoding.
 *
 ***************************************************************************/

#include "bstd.h"           /* standard types */
#include "bdbg.h"           /* Dbglib */
#include "bkni.h"			/* For critical sections */
#include "bvbi.h"           /* VBI processing, this module. */
#include "bvbi_priv.h"      /* VBI internal data structures */

#if (BVBI_P_NUM_VDEC >= 1) /** { **/

#include "bchp_ttd_0.h"     /* RDB info for TTD core */

#define TTD_0_ACKNOWLEDGE_READ                1

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
void BVBI_P_TT_Dec_Init (BREG_Handle hReg, uint32_t ulCoreOffset)
{
	uint32_t ulReg = 0x0;

	BDBG_ENTER(BVBI_P_TT_Dec_Init);

	BKNI_EnterCriticalSection();

	BREG_Write32 (hReg, BCHP_TTD_0_reset + ulCoreOffset, ulReg);

	BKNI_LeaveCriticalSection();

	BDBG_LEAVE(BVBI_P_TT_Dec_Init);
}


/***************************************************************************
 *
 */
BERR_Code BVBI_P_TT_Dec_Program (
	BREG_Handle hReg,
	BMEM_Handle hMem,
	BAVC_SourceId eSource,
	bool bActive,
	BFMT_VideoFmt eVideoFormat,
	#ifdef BVBI_P_TTD_SCB2_ERROR
	uint8_t* topTTscratch,
	uint8_t* botTTscratch,
	#endif
	BVBI_P_TTData* topData,
	BVBI_P_TTData* botData)
{
	uint32_t H_TopMask;
	uint32_t H_BotMask;
	uint32_t H_TopInsert;
	uint32_t H_BotInsert;
	uint32_t H_AddrTop;
	uint32_t H_AddrBot;
	uint32_t H_Control;
	uint32_t H_Lock;
	uint32_t H_Shift;
	uint32_t H_Filter;
	uint32_t H_TeletextFreq;
	uint32_t H_Window;
	uint32_t H_FramCod;
	uint32_t H_TfFcSel;
	uint32_t H_BfFcSel;
	uint32_t H_BlkLev;

	uint32_t ulTopMaskReg;
	uint32_t ulBotMaskReg;
	uint32_t ulTopInsertReg;
	uint32_t ulBotInsertReg;
	/* uint32_t ulAddrTopReg; */
	/* uint32_t ulAddrBotReg; */
	uint32_t ulControlReg;
	uint32_t ulLockReg;
	uint32_t ulShiftReg;
	uint32_t ulFilterReg;
	uint32_t ulTeletextFreqReg;
	uint32_t ulWindowReg;
	uint32_t ulFramCodReg;
	uint32_t ulTfFcSelReg;
	uint32_t ulBfFcSelReg;
	uint32_t ulCoreOffset;
	uint32_t ulBlkLevReg;

	uint8_t  ucNumLinesTF;
	uint8_t  ucNumLinesBF;
	uint8_t  ucBytesPerLine;
	uint16_t usStartLineTF;
	uint16_t usStartLineBF;

	int index;
	uint32_t offset;
	BERR_Code eErr;

	BDBG_ENTER(BVBI_P_TT_Dec_Program);

	/* Figure out which decoder core to use */
	switch (eSource)
	{
	case BAVC_SourceId_eVdec0:
		H_TopMask      = BCHP_TTD_0_tf_mask;
		H_BotMask      = BCHP_TTD_0_bf_mask;
		H_TopInsert    = BCHP_TTD_0_tf_insert;
		H_BotInsert    = BCHP_TTD_0_bf_insert;
		H_AddrTop      = BCHP_TTD_0_write_address_top;
		H_AddrBot      = BCHP_TTD_0_write_address_bottom;
		H_Control      = BCHP_TTD_0_control;
		H_Lock         = BCHP_TTD_0_lock_control;
		H_Shift        = BCHP_TTD_0_shift_control;
		H_Filter       = BCHP_TTD_0_filter_control;
		H_TeletextFreq = BCHP_TTD_0_teletext_freq;
		H_Window       = BCHP_TTD_0_window_control;
		H_FramCod      = BCHP_TTD_0_framing_code;
		H_TfFcSel      = BCHP_TTD_0_tf_fc_select;
		H_BfFcSel      = BCHP_TTD_0_bf_fc_select;
		ulCoreOffset   = 0x0;
		H_BlkLev       = BCHP_TTD_0_black_level;
		index = 0;
		break;
#if (BVBI_P_NUM_VDEC > 1)
	case BAVC_SourceId_eVdec1:
		H_TopMask      = BCHP_TTD_1_tf_mask;
		H_BotMask      = BCHP_TTD_1_bf_mask;
		H_TopInsert    = BCHP_TTD_1_tf_insert;
		H_BotInsert    = BCHP_TTD_1_bf_insert;
		H_AddrTop      = BCHP_TTD_1_write_address_top;
		H_AddrBot      = BCHP_TTD_1_write_address_bottom;
		H_Control      = BCHP_TTD_1_control;
		H_Lock         = BCHP_TTD_1_lock_control;
		H_Shift        = BCHP_TTD_1_shift_control;
		H_Filter       = BCHP_TTD_1_filter_control;
		H_TeletextFreq = BCHP_TTD_1_teletext_freq;
		H_Window       = BCHP_TTD_1_window_control;
		H_FramCod      = BCHP_TTD_1_framing_code;
		H_TfFcSel      = BCHP_TTD_1_tf_fc_select;
		H_BfFcSel      = BCHP_TTD_1_bf_fc_select;
		ulCoreOffset   = BCHP_TTD_1_reset - BCHP_TTD_0_reset;
		break;
#endif
	default:
		/* This should never happen!  This parameter was checked by
		   BVBI_Decode_Create() */
		BDBG_LEAVE(BVBI_P_TT_Dec_Program);
		return BERR_TRACE(BERR_INVALID_PARAMETER);
		break;
	}

	/* TODO: handle little endian vs. big endian */

	/* If user wants to turn off teletext processing, just reset the
	   entire core. */
	if (!bActive)
	{
		BVBI_P_TT_Dec_Init (hReg, ulCoreOffset);
		BDBG_LEAVE(BVBI_P_TT_Dec_Program);
		return BERR_SUCCESS;
	}

	BKNI_EnterCriticalSection();

	/* Start programming the TTD control register */
    ulControlReg = BREG_Read32 (hReg, H_Control);
	ulControlReg &= ~(
		BCHP_MASK       (TTD_0_control, enable_tf        ) |
		BCHP_MASK       (TTD_0_control, enable_bf        ) );
	ulControlReg |= (
		BCHP_FIELD_DATA (TTD_0_control, enable_tf,      1) |
		BCHP_FIELD_DATA (TTD_0_control, enable_bf,      1) );

	/* Read the lock control register */
    ulLockReg = BREG_Read32 (hReg, H_Lock);

	/* Program the top insert register */
	ulTopInsertReg = BREG_Read32 (hReg, H_TopInsert);
	ulTopInsertReg &=  ~(
		BCHP_MASK       (TTD_0_tf_insert, Insert                    ) );
	ulTopInsertReg |=  (
		BCHP_FIELD_DATA (TTD_0_tf_insert, Insert,                  0) );
	BREG_Write32 (hReg, H_TopInsert, ulTopInsertReg);

	/* Program the bottom insert register */
	ulBotInsertReg = BREG_Read32 (hReg, H_BotInsert);
	ulBotInsertReg &=  ~(
		BCHP_MASK       (TTD_0_bf_insert, Insert                    ) );
	ulBotInsertReg |=  (
		BCHP_FIELD_DATA (TTD_0_bf_insert, Insert,                  0) );
	BREG_Write32 (hReg, H_BotInsert, ulBotInsertReg);

	/* Start programming the shift control register */
	ulShiftReg = BREG_Read32 (hReg, H_Shift);
	ulShiftReg &= ~(
		BCHP_MASK       (TTD_0_shift_control, shift_direction             ) |
		BCHP_MASK       (TTD_0_shift_control, signal_threshold            ) );
	ulShiftReg |= (
#if (BSTD_CPU_ENDIAN == BSTD_ENDIAN_LITTLE)
		BCHP_FIELD_ENUM (TTD_0_shift_control, shift_direction,    MSBToLSB) |
#else
		BCHP_FIELD_ENUM (TTD_0_shift_control, shift_direction,    LSBToMSB) |
#endif
		BCHP_FIELD_DATA (TTD_0_shift_control, signal_threshold,       0x07) );

	/* Read the filter control register */
	ulFilterReg = BREG_Read32 (hReg, H_Filter);

	/* Read the frequency register */
	ulTeletextFreqReg = BREG_Read32 (hReg, H_TeletextFreq);

	/* Read the window control register */
	ulWindowReg = BREG_Read32 (hReg, H_Window);

	/* Start programming the framing code register */
	ulFramCodReg = BREG_Read32 (hReg, H_FramCod);

	/* Start programming the top field framing code selection register */
	ulTfFcSelReg = BREG_Read32 (hReg, H_TfFcSel);
	ulTfFcSelReg &= ~BCHP_MASK       (TTD_0_tf_fc_select, select     );
	ulTfFcSelReg |=  BCHP_FIELD_DATA (TTD_0_tf_fc_select, select, 0x0);

	/* Start programming the bottom field framing code selection register */
	ulBfFcSelReg = BREG_Read32 (hReg, H_BfFcSel);
	ulBfFcSelReg &= ~BCHP_MASK       (TTD_0_bf_fc_select, select     );
	ulBfFcSelReg |=  BCHP_FIELD_DATA (TTD_0_bf_fc_select, select, 0x0);

	/* Select video format */
	switch (eVideoFormat)
	{
	case BFMT_VideoFmt_eNTSC:
	case BFMT_VideoFmt_eNTSC_J:
	case BFMT_VideoFmt_e720x482_NTSC:
	case BFMT_VideoFmt_e720x482_NTSC_J:
	case BFMT_VideoFmt_ePAL_M:
		/* NTSC specific settings */

		ucNumLinesTF   =  11;
		ucNumLinesBF   =  11;
		usStartLineTF  =  11;
		usStartLineBF  = 273;
		ucBytesPerLine =  34;

		ulControlReg &= ~(
			BCHP_MASK       (TTD_0_control, startline_tf                      ) |
			BCHP_MASK       (TTD_0_control, startline_bf                      ) |
			BCHP_MASK       (TTD_0_control, start_delay                       ) |
			BCHP_MASK       (TTD_0_control, teletext_mode                     ) );
		ulControlReg |= (
			BCHP_FIELD_DATA (TTD_0_control, startline_tf,  usStartLineTF -   2) |
			BCHP_FIELD_DATA (TTD_0_control, startline_bf,  usStartLineBF - 264) |
			BCHP_FIELD_DATA (TTD_0_control, start_delay,                  0x01) |
			BCHP_FIELD_ENUM (TTD_0_control, teletext_mode,               NABTS) );

    /* Program the TTD lock register */
	ulLockReg &= ~(
		BCHP_MASK       (TTD_0_lock_control, frame_code_lines             ) |
		BCHP_MASK       (TTD_0_lock_control, write_inhibit                ) );
	ulLockReg |= (
		BCHP_FIELD_DATA (TTD_0_lock_control, frame_code_lines,        0x10) |
		BCHP_FIELD_DATA (TTD_0_lock_control, write_inhibit,              0) );

		ulShiftReg &=
			 ~BCHP_MASK       (TTD_0_shift_control, shift_direction          );
		ulShiftReg |=
#if (BSTD_CPU_ENDIAN == BSTD_ENDIAN_LITTLE)
			  BCHP_FIELD_ENUM (TTD_0_shift_control, shift_direction, MSBToLSB);
#else
			  BCHP_FIELD_ENUM (TTD_0_shift_control, shift_direction, LSBToMSB);
#endif

	/* Program the filter control register */
	ulFilterReg &= ~(
		BCHP_MASK       (TTD_0_filter_control, filter_phase_adjustment      ) |
		BCHP_MASK       (TTD_0_filter_control, min_signal_mean              ) |
		BCHP_MASK       (TTD_0_filter_control, avg_threshold                ) |
		BCHP_MASK       (TTD_0_filter_control, last_position                ) |
		BCHP_MASK       (TTD_0_filter_control, fc_one_bit_off               ) );
	ulFilterReg |= (
		BCHP_FIELD_DATA (TTD_0_filter_control, filter_phase_adjustment, 0x6b) |
		BCHP_FIELD_DATA (TTD_0_filter_control, min_signal_mean,          0x4) |
		BCHP_FIELD_DATA (TTD_0_filter_control, avg_threshold,            0x2) |
		BCHP_FIELD_DATA (TTD_0_filter_control, last_position,            0x0) |
		BCHP_FIELD_DATA (TTD_0_filter_control, fc_one_bit_off,           0x0) );

		ulTeletextFreqReg &=
			~BCHP_MASK       (TTD_0_teletext_freq, Frequency            );
		ulTeletextFreqReg |=
			 BCHP_FIELD_DATA (TTD_0_teletext_freq, Frequency, 0x1b26c978);

		ulWindowReg &= ~(
			BCHP_MASK       (TTD_0_window_control, enable                 ) |
			BCHP_MASK       (TTD_0_window_control, window_lock_lines      ) |
			BCHP_MASK       (TTD_0_window_control, window_length          ) );
		ulWindowReg |= (
			BCHP_FIELD_ENUM (TTD_0_window_control, enable, ENABLED        ) |
			BCHP_FIELD_DATA (TTD_0_window_control, window_lock_lines,    1) |
			BCHP_FIELD_DATA (TTD_0_window_control, window_length,     0x74) );

		ulFramCodReg &= ~(
			BCHP_MASK       (TTD_0_framing_code, framing_code_1               ) |
			BCHP_MASK       (TTD_0_framing_code, framing_code_0               ) );
		ulFramCodReg |= (
			BCHP_FIELD_DATA (
				TTD_0_framing_code, framing_code_1, BVBI_TT_NABTS_FRAMING_CODE) |
			BCHP_FIELD_DATA (
				TTD_0_framing_code, framing_code_0, BVBI_TT_NABTS_FRAMING_CODE) );

		/* Continue programming the black level register */
		ulBlkLevReg = BREG_Read32 (hReg, H_BlkLev);
		ulBlkLevReg &= ~BCHP_MASK       (TTD_0_black_level, black_level      );
		ulBlkLevReg |=  BCHP_FIELD_DATA (TTD_0_black_level, black_level, 0x49);

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

		ucNumLinesTF   = 17;
		ucNumLinesBF   = 18;
		usStartLineTF  = 7;
		usStartLineBF  = 319;
		/* PR18343 */
		usStartLineTF -= 1;

		/* Working with 93563-C0 */
		usStartLineBF -= 1;

#if (BVDC_P_3DCOMB_TTD_CONFLICT)
		usStartLineTF -= 1;
		usStartLineBF -= 1;
#endif

		ucBytesPerLine = 43;

		ulControlReg &= ~(
			BCHP_MASK       (TTD_0_control, startline_tf                      ) |
			BCHP_MASK       (TTD_0_control, startline_bf                      ) |
			BCHP_MASK       (TTD_0_control, start_delay                       ) |
			BCHP_MASK       (TTD_0_control, teletext_mode                     ) );
		ulControlReg |= (
			BCHP_FIELD_DATA (TTD_0_control, startline_tf,  usStartLineTF -   2) |
			BCHP_FIELD_DATA (TTD_0_control, startline_bf,  usStartLineBF - 314) |
			BCHP_FIELD_DATA (TTD_0_control, start_delay,                     0) |
			BCHP_FIELD_ENUM (TTD_0_control, teletext_mode,         ETSTeletext) );

    /* Program the TTD lock register */
	ulLockReg &= ~(
		BCHP_MASK       (TTD_0_lock_control, frame_code_lines             ) |
		BCHP_MASK       (TTD_0_lock_control, write_inhibit                ) );
	ulLockReg |= (
		BCHP_FIELD_DATA (TTD_0_lock_control, frame_code_lines,        0x02) |
		BCHP_FIELD_DATA (TTD_0_lock_control, write_inhibit,              0) );

		ulShiftReg &=
			~BCHP_MASK       (TTD_0_shift_control, shift_direction          );
		ulShiftReg |=
#if (BSTD_CPU_ENDIAN == BSTD_ENDIAN_LITTLE)
			 BCHP_FIELD_ENUM (TTD_0_shift_control, shift_direction, MSBToLSB);
#else
			 BCHP_FIELD_ENUM (TTD_0_shift_control, shift_direction, LSBToMSB);
#endif

	/* Program the filter control register */
	ulFilterReg &= ~(
		BCHP_MASK       (TTD_0_filter_control, filter_phase_adjustment      ) |
		BCHP_MASK       (TTD_0_filter_control, min_signal_mean              ) |
		BCHP_MASK       (TTD_0_filter_control, avg_threshold                ) |
		BCHP_MASK       (TTD_0_filter_control, last_position                ) |
		BCHP_MASK       (TTD_0_filter_control, fc_one_bit_off               ) );
	ulFilterReg |= (
		BCHP_FIELD_DATA (TTD_0_filter_control, filter_phase_adjustment, 0x23) |
		BCHP_FIELD_DATA (TTD_0_filter_control, min_signal_mean,          0x4) |
		BCHP_FIELD_DATA (TTD_0_filter_control, avg_threshold,            0x1) |
		BCHP_FIELD_DATA (TTD_0_filter_control, last_position,            0x0) |
		BCHP_FIELD_DATA (TTD_0_filter_control, fc_one_bit_off,           0x0) );

		ulTeletextFreqReg &=
			~BCHP_MASK       (TTD_0_teletext_freq, Frequency            );
		ulTeletextFreqReg |=
			 BCHP_FIELD_DATA (TTD_0_teletext_freq, Frequency, 0x20e38e39);

		ulWindowReg &= ~(
			BCHP_MASK       (TTD_0_window_control, enable                 ) |
			BCHP_MASK       (TTD_0_window_control, window_lock_lines      ) |
			BCHP_MASK       (TTD_0_window_control, window_length          ) );
		ulWindowReg |= (
			BCHP_FIELD_ENUM (TTD_0_window_control, enable,        DISABLED) |
			BCHP_FIELD_DATA (TTD_0_window_control, window_lock_lines,    0) |
			BCHP_FIELD_DATA (TTD_0_window_control, window_length,     0x74) );

		ulFramCodReg &= ~(
			BCHP_MASK       (TTD_0_framing_code, framing_code_1             ) |
			BCHP_MASK       (TTD_0_framing_code, framing_code_0             ) );
		ulFramCodReg |= (
			BCHP_FIELD_DATA (
				TTD_0_framing_code, framing_code_1, BVBI_TT_ETS_FRAMING_CODE) |
			BCHP_FIELD_DATA (
				TTD_0_framing_code, framing_code_0, BVBI_TT_ETS_FRAMING_CODE) );

		/* Continue programming the black level register */
		ulBlkLevReg = BREG_Read32 (hReg, H_BlkLev);
		ulBlkLevReg &= ~BCHP_MASK       (TTD_0_black_level, black_level     );
		ulBlkLevReg |=  BCHP_FIELD_DATA (TTD_0_black_level, black_level, 240);

		break;

	default:
		BKNI_LeaveCriticalSection();
		BDBG_LEAVE(BVBI_P_TT_Dec_Program);
		return BERR_TRACE (BERR_INVALID_PARAMETER);
		break;
	}

	/* Program the mask registers */
	/* Magic number discovered by the two Brads. Works for PAL. */
	ulTopMaskReg =  BCHP_FIELD_DATA (TTD_0_tf_mask, Mask, 0x3FFFE);
	BREG_Write32 (hReg, H_TopMask, ulTopMaskReg);
	ulBotMaskReg =  BCHP_FIELD_DATA (TTD_0_bf_mask, Mask, 0x3FFFE);
	BREG_Write32 (hReg, H_BotMask, ulBotMaskReg);

	/* Finish programming the black level register */
	BREG_Write32 (hReg, H_BlkLev, ulBlkLevReg);

	/* Prepare to collect data in the decode handle */
#ifdef BVBI_P_TTD_SCB2_ERROR
	eErr = BERR_TRACE (BMEM_ConvertAddressToOffset (
		hMem, topTTscratch, &offset));
	offset >>= 2;
#else
	eErr = BERR_TRACE (BMEM_ConvertAddressToOffset (
		hMem, topData->pucData, &offset));
#endif
	BDBG_ASSERT (eErr == BERR_SUCCESS);
	BREG_Write32 ( hReg, H_AddrTop, offset);
#ifdef BVBI_P_TTD_SCB2_ERROR
	eErr = BERR_TRACE (BMEM_ConvertAddressToOffset (
		hMem, botTTscratch, &offset));
	offset >>= 2;
#else
	eErr = BERR_TRACE (BMEM_ConvertAddressToOffset (
		hMem, botData->pucData, &offset));
#endif
	BDBG_ASSERT (eErr == BERR_SUCCESS);
	BREG_Write32 ( hReg, H_AddrBot, offset);

	/* Update the field handles that receive the data */
	topData->ucLines    = ucNumLinesTF;
	topData->ucLineSize = ucBytesPerLine;
	botData->ucLines    = ucNumLinesBF;
	botData->ucLineSize = ucBytesPerLine;

	/* write the ten registers with updated values */
    BREG_Write32 ( hReg, H_Control,           ulControlReg );
    BREG_Write32 ( hReg, H_Lock,                 ulLockReg );
    BREG_Write32 ( hReg, H_Shift,               ulShiftReg );
    BREG_Write32 ( hReg, H_Filter,             ulFilterReg );
    BREG_Write32 ( hReg, H_TeletextFreq, ulTeletextFreqReg );
    BREG_Write32 ( hReg, H_Window,             ulWindowReg );
    BREG_Write32 ( hReg, H_FramCod,           ulFramCodReg );
    BREG_Write32 ( hReg, H_TfFcSel,           ulTfFcSelReg );
    BREG_Write32 ( hReg, H_BfFcSel,           ulBfFcSelReg );

	BKNI_LeaveCriticalSection();
	BDBG_LEAVE(BVBI_P_TT_Dec_Program);
	return BERR_SUCCESS;
}

/* Debug code
volatile uint64_t passfail = 0;
volatile int error_index = 0;
volatile uint32_t error_store[64];
*/

/***************************************************************************
 *
 */
uint32_t BVBI_P_TT_Decode_Data_isr (
	BREG_Handle hReg,
	BMEM_Handle hMem,
    BAVC_SourceId eSource,
	BFMT_VideoFmt eVideoFormat,
	BAVC_Polarity polarity,
	BVBI_P_TTData* pTTDataNext )
{
/*
	Programming note: the implementation here assumes that the bitfield layout
	within registers is the same for all teletext decoder cores in the chip.

	If a chip is built that has multiple teletext decoder cores that are not
	identical, then this routine will have to be redesigned.
*/
	uint32_t ulReg;
	uint32_t H_WrAdTop;
	uint32_t H_WrAdBot;
	uint32_t H_Status;
	uint32_t ulStat;
	uint32_t dataSize;
	bool recognized_video_format;
	uint32_t ulReg2 = 0x0;

	uint8_t  ucMinLines = 0;
	uint8_t  ucMinLineSize = 0;
	uint32_t ulErrInfo = 0;

#ifndef BVBI_P_TTD_SCB2_ERROR
	BERR_Code eErr;
	uint32_t  offset;
#endif

	/* Debug code
	uint8_t* printme = 0;
	passfail <<= 1;
	*/

#ifdef BVBI_P_TTD_SCB2_ERROR
	BSTD_UNUSED (hMem);
#endif

	BDBG_ENTER(BVBI_P_TT_Decode_Data_isr);

	/* Figure out which decoder core to use */
	switch (eSource)
	{
	case BAVC_SourceId_eVdec0:
		H_Status       = BCHP_TTD_0_status;
		H_WrAdTop      = BCHP_TTD_0_write_address_top;
		H_WrAdBot      = BCHP_TTD_0_write_address_bottom;
		break;
#if (BVBI_P_NUM_VDEC > 1)
	case BAVC_SourceId_eVdec1:
		H_Status       = BCHP_TTD_0_status;
		H_WrAdTop      = BCHP_TTD_0_write_address_top;
		H_WrAdBot      = BCHP_TTD_0_write_address_bottom;
		break;
#endif
	default:
		/* This should never happen!  This parameter was checked by
		   BVBI_Decode_Create() */
		BDBG_LEAVE(BVBI_P_TT_Decode_Data_isr);
		return (-1);
		break;
	}

	/* A sanity check */
	BDBG_ASSERT (
		(polarity == BAVC_Polarity_eTopField) ||
		(polarity == BAVC_Polarity_eBotField) );

	/* Verify that field handle is big enough to hold the TT data */
	switch (eVideoFormat)
	{
	case BFMT_VideoFmt_eNTSC:
	case BFMT_VideoFmt_eNTSC_J:
	case BFMT_VideoFmt_e720x482_NTSC:
	case BFMT_VideoFmt_e720x482_NTSC_J:
	case BFMT_VideoFmt_ePAL_M:
		ucMinLines    = 11;
		ucMinLineSize = 34;
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
		ucMinLines    = 18;
		ucMinLineSize = 43;
		break;
	default:
		/* This should never happen!  Checked in calling routine! */
		recognized_video_format = false;
		BDBG_ASSERT (recognized_video_format);
		break;
	}
	dataSize = BVBI_P_TT_Size_Storage (ucMinLines, ucMinLineSize);

	if (pTTDataNext->ucDataSize < dataSize)
	{
		ulErrInfo |=
			(BVBI_LINE_ERROR_FLDH_CONFLICT | BVBI_LINE_ERROR_TELETEXT_NODATA);
		BDBG_LEAVE(BVBI_P_TT_Decode_Data_isr);
		return ulErrInfo;
	}

	/* Read the status register */
	ulReg = BREG_Read32 (hReg, H_Status);

	/* If top field */
	if (polarity == BAVC_Polarity_eTopField)
	{
		/* Verify that the hardware has finished digesting the data, if any. */
		ulStat = ulReg & BCHP_MASK (TTD_0_status, buffer_complete_tf);
		if (ulStat == 0)
		{
			/* Inform of error */
			ulErrInfo |= BVBI_LINE_ERROR_TELETEXT_INCOMPLETE;
			goto done;
		}

		/* Will acknowledge data to hardware */
		ulReg2 |=  ulStat;

		/* This bit should always be written? */
		ulStat = ulReg & BCHP_MASK (TTD_0_status, interrupt_tf);
		if (ulStat)
			ulReg2 |= ulStat;

		/* Debug code
		printme = pTTDataNext->pucData;
		*/

		/* Debug code
		{
		static unsigned int ticker = 0;
		if ((++ticker % 4) == 0)
		{
			int index;
			unsigned char* val = pTTDataNext->pucData + 4 + 2*43;
			for (index = 0 ; index < 43 ; ++index)
				*val++ = 0xab;
			unsigned char* val = pTTDataNext->pucData + 4;
			*val = 0xff;
		}
		}
		*/

		/* Give hardware a new place to write data to */
#ifndef BVBI_P_TTD_SCB2_ERROR
		eErr = BERR_TRACE (BMEM_ConvertAddressToOffset (
			hMem, pTTDataNext->pucData, &offset));
		BDBG_ASSERT (eErr == BERR_SUCCESS);
		BREG_Write32 (hReg, H_WrAdTop, offset);
#endif
	}
	else /* Bottom field */
	{
		/* Verify that the hardware has finished digesting the data, if any. */
		ulStat = ulReg & BCHP_MASK (TTD_0_status, buffer_complete_bf);
		if (ulStat == 0)
		{
			/* Inform of error */
			ulErrInfo |= BVBI_LINE_ERROR_TELETEXT_INCOMPLETE;
			goto done;
		}

		/* Will acknowledge data to hardware */
		ulReg2 |=  ulStat;

		/* This bit should always be written? */
		ulStat = ulReg & BCHP_MASK (TTD_0_status, interrupt_bf);
		if (ulStat)
			ulReg2 |=  ulStat;

		/* Debug code
		printme = pTTDataNext->pucData;
		*/

		/* Debug code
		*(uint32_t*)(pTTDataNext->pucData)= 0x11;
		*/

		/* Give hardware a new place to write data to */
#ifndef BVBI_P_TTD_SCB2_ERROR
		eErr = BERR_TRACE (BMEM_ConvertAddressToOffset (
			hMem, pTTDataNext->pucData, &offset));
		BDBG_ASSERT (eErr == BERR_SUCCESS);
		BREG_Write32 (hReg, H_WrAdBot, offset);
#endif
	}

	/* Success, so finish describing the captured data */
	pTTDataNext->ucLines    = ucMinLines;
	pTTDataNext->ucLineSize = ucMinLineSize;

	/* Debug code
	passfail |= (uint64_t)(0x0000000000000001);
	*/

done:

	/* Debug code
	ulStat = (
		BCHP_MASK (TTD_0_status, buffer_complete_tf) |
		BCHP_MASK (TTD_0_status, buffer_complete_bf) );
	if ((ulStat & ulReg) == ulStat)
	{
		printf ("%c: two fields of data are waiting\n",
			(polarity == BAVC_Polarity_eTopField) ? 'T' : 'B');
	}
	if ((passfail & (uint64_t)(0x0000000000000001)) == 0)
	{
		error_store[error_index++] = ulReg;
		error_index &= (sizeof(error_store)/sizeof(error_store[0]) - 1);
		printf ("Decode error %c: %08x\n",
			(polarity == BAVC_Polarity_eTopField) ? 'T' : 'B',
			ulReg);
	}
	{
	static uint32_t dcounter = 0;
	++dcounter;
	if ((dcounter > 80) && (dcounter < 150))
	{
		if (printme)
		{
			uint32_t mask = *(uint32_t*)printme;
			uint8_t* p1 = printme + 4;
			printf ("%d%c: decoded M:%08x : %02x %02x %02x %02x\n",
				dcounter,
				(polarity == BAVC_Polarity_eTopField) ? 'T' : 'B',
				mask,
				*p1,
				*(p1 + ucMinLineSize),
				*(p1 + 2 * ucMinLineSize),
				*(p1 + 3 * ucMinLineSize));
		}
		else
		{
			printf ("%d%c: did not decode anything\n",
				dcounter,
				(polarity == BAVC_Polarity_eTopField) ? 'T' : 'B');
		}
	}
	}
	*/

	/* Finish acknowledging data */
    BREG_Write32 ( hReg, H_Status, ulReg2) ;

	BDBG_LEAVE(BVBI_P_TT_Decode_Data_isr);
	return ulErrInfo;
}

/***************************************************************************
* Static (private) functions
***************************************************************************/

#endif /** } (BVBI_P_NUM_VDEC >= 1) **/

/* End of file */
