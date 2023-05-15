/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvbi_tte.c $
 * $brcm_Revision: Hydra_Software_Devel/10 $
 * $brcm_Date: 8/21/12 4:48p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vbi/7405/bvbi_tte.c $
 * 
 * Hydra_Software_Devel/10   8/21/12 4:48p darnstein
 * SW7435-305: support new video formats 720x482i and 720x483p in VBI. The
 * changes are the same as some of those made in SW7435-276.
 * 
 * Hydra_Software_Devel/9   8/2/12 1:18p darnstein
 * SW7125-1311: with regards to teletext, handle PAL-M in same way as NTSC
 * and NTSC-J.
 * 
 * Hydra_Software_Devel/8   1/14/10 6:06p darnstein
 * SW7550-120: Add support for SECAM variants.
 * 
 * Hydra_Software_Devel/7   10/26/09 4:17p darnstein
 * AR35230-15,HW7403-261,SW7325-489: handle TTE bitmask the same way on
 * 7325 and 7335: assume the problem with large number of teletext lines
 * is in effect. Do not swap bytes on any system, little endian or big
 * endian.
 * 
 * Hydra_Software_Devel/6   4/16/09 7:28p darnstein
 * PR54282: On big-endian systems, do a 32-bit swap on the teletext line
 * bitmask.
 * 
 * Hydra_Software_Devel/6   4/16/09 7:25p darnstein
 * PR54282: On big-endian systems, do a 32-bit swap on the teletext line
 * bitmask.
 * 
 * Hydra_Software_Devel/5   4/13/09 3:24p darnstein
 * PR54117: At the request of a customer, allow NABTS to occupy video
 * lines as late as line 22.
 * 
 * Hydra_Software_Devel/4   3/27/09 5:57p darnstein
 * PR53635: Remove internal ConfigForOthers code. It is obsolete, and it
 * was causing a problem (this PR).
 * 
 * Hydra_Software_Devel/3   1/20/09 12:12p darnstein
 * PR51074: For the problem with 15 lines or more teletext (output), apply
 * the same software workaround that was used on 7401-C0. Note that this
 * workaround itself causes problems for some chipsets, such as 3563-C0.
 * So the workaround is controlled by an #ifdef.
 * 
 * Hydra_Software_Devel/2   12/3/08 7:57p darnstein
 * PR45819: New, more modular form of most BVBI source files.
 * 
 * Hydra_Software_Devel/52   5/13/08 2:15p darnstein
 * PR34584: Port over changes from 7401 dedicated branch. Allow user to
 * choose bit shift direction for teletext encoding.
 * 
 * Hydra_Software_Devel/51   4/11/08 5:27p darnstein
 * PR24573: silence a compiler warning.
 * 
 * Hydra_Software_Devel/50   4/9/08 8:04p darnstein
 * PR37064: The fix that works so well on 7401 hardware causes failure on
 * 3563 hardware. I believe that there is a hardware flaw involving the
 * line mask registers in the 3563-C0 TTE core. Will diagnose later.
 * 
 * Hydra_Software_Devel/49   4/8/08 6:14p darnstein
 * PR37064: This is the same 15-lines-of-teletext problem that was solved
 * (worked-around, actually) on the dedicated 7401 ClearCase branch.
 * 
 * Hydra_Software_Devel/48   3/23/07 4:06p darnstein
 * PR24573: The RDB name for the reset register is now the same for 7440-
 * A0 and 7440-B0. Adjust software to match this improvement.
 * 
 * Hydra_Software_Devel/47   2/20/07 10:44a darnstein
 * PR27521: Apply software workaround. The TTE core is reset, at every
 * video field. Also, the encoder output for the field NOT in use is
 * disabled, at every video field.
 * 
 * Hydra_Software_Devel/46   1/17/07 5:32p darnstein
 * PR26464: correctly handle teletext output to multiple VECs
 * 
 * Hydra_Software_Devel/45   1/2/07 4:19p darnstein
 * PR26872: Mechanically add SECAM to all cases where PAL formats are
 * accepted.
 * 
 * Hydra_Software_Devel/44   12/18/06 1:02p darnstein
 * PR24573: Adapt to an odd register name for BCM97440-B0 chipset.
 * 
 * Hydra_Software_Devel/43   12/15/06 4:16p darnstein
 * PR25990: I recently broke the build for BCM97438-A0 chipset. Fixed now.
 * 
 * Hydra_Software_Devel/42   12/15/06 3:50p darnstein
 * PR24573: Adapt to some odd register names in BCM97440-A0 chipset.
 * 
 * Hydra_Software_Devel/41   12/15/06 2:34p darnstein
 * PR25990: Fix a stupid typo.
 * 
 * Hydra_Software_Devel/40   12/14/06 7:19p darnstein
 * PR25990: Can compile for BCM97400-B0 now.
 * 
 * Hydra_Software_Devel/39   11/10/06 4:34p darnstein
 * PR23247: Fix a silly bug programming for serial port output of
 * teletext.
 * 
 * Hydra_Software_Devel/38   11/8/06 5:13p darnstein
 * PR23247: Serial output of teletext is partially supported now.
 * 
 * Hydra_Software_Devel/37   10/19/06 5:47p darnstein
 * PR24979: change horizontal offset of waveform when sending NTSC.
 * Theory: VEC microcode changed in such a way that the register values
 * are interpreted differently.
 * 
 * Hydra_Software_Devel/36   9/26/06 7:37p darnstein
 * PR24573: The BVBI porting interface module now compiles for the 7440
 * chipset. Correct operation has not been verified.
 * 
 * Hydra_Software_Devel/35   8/31/06 2:10p darnstein
 * PR23869: clean up the handling of multiple VECs and VDECs.
 * 
 * Hydra_Software_Devel/34   8/18/06 6:51p darnstein
 * PR23178: basic compile on 93563 is possible.
 * 
 * Hydra_Software_Devel/33   7/20/06 2:23p darnstein
 * PR21688: Use the new hardware soft reset scheme for later model chips.
 * 
 * Hydra_Software_Devel/32   5/4/06 5:31p darnstein
 * PR18010: Silence a compiler warning.
 * 
 * Hydra_Software_Devel/31   5/3/06 4:31p darnstein
 * PR18010: The previous checkin Hydra_Software_Devel/30 was incomplete.
 * This finishes it.
 * 
 * Hydra_Software_Devel/30   5/3/06 2:31p darnstein
 * PR18010: Remove extraneous call to BVBI_P_CC_ConfigForOthers() that
 * might change the video format of closed caption encoder core.
 * 
 * Hydra_Software_Devel/29   3/1/06 4:19p darnstein
 * PR19955: For PAL teletext encoding: max lines is now 18 for both top
 * and bottom fields. Do an "endian" swap on bit order (LSB first vs. MSB
 * first).
 * 
 * Hydra_Software_Devel/28   2/21/06 6:20p darnstein
 * PR18343: Move top line for teletext output up raster by 1 (numerically
 * smaller by 1), but ONLY for PAL.
 * 
 * Hydra_Software_Devel/27   2/6/06 4:09p darnstein
 * PR18343: Move top line for teletext output up raster by 1 (numerically
 * smaller by 1).
 * 
 * Hydra_Software_Devel/26   12/5/05 7:30p darnstein
 * PR18010: Implement work-around for the bad line number, together with
 * an exception for customers that now depend on the bad line number.
 * 
 * Hydra_Software_Devel/25   11/30/05 3:48p darnstein
 * PR18234: Apply encoder tuning parameters chosen by Kin Fan Ho, for PAL
 * teletext output.
 * 
 * Hydra_Software_Devel/24   9/23/05 2:47p darnstein
 * PR13750: Proper use of BERR_TRACE and BERR_CODEs.
 * 
 * Hydra_Software_Devel/23   9/2/05 2:16p darnstein
 * PR16980: fix my stupid typing error.
 * 
 * Hydra_Software_Devel/22   8/22/05 8:11p darnstein
 * PR16057: To support many different chips, use private #defines that
 * specify number of VECs, VDECs, and (separately) pass-through VECs.
 * 
 * Hydra_Software_Devel/21   3/17/05 7:35p darnstein
 * PR 14472: Eliminate references to secondary VEC if chip is 3560.
 * 
 * Hydra_Software_Devel/20   7/16/04 7:07p darnstein
 * PR 9080: merge in 656 input and output work. Some testing and debugging
 * remains to be done.
 * 
 * Hydra_Software_Devel/I656/1   6/28/04 1:10p darnstein
 * 656 output is ready for testing.
 * 
 * Hydra_Software_Devel/19   6/17/04 6:19p darnstein
 * PR 11443: Get rid of a kludge that was only needed in revision -A0 of
 * the hardware.
 * 
 * Hydra_Software_Devel/18   5/24/04 7:24p jasonh
 * PR 11189: Merge down from B0 to main-line
 * 
 * Hydra_Software_Devel/Refsw_Devel_7038_B0/2   4/29/04 6:23p darnstein
 * PR 9080: Tune start_delay and output_attenuation for NTSC encoding of
 * teletext (with Kin Fan). This duplicates version 17 in the main
 * branch.
 * 
 * Hydra_Software_Devel/Refsw_Devel_7038_B0/1   4/19/04 2:45p darnstein
 * PR 9080: Compilation is possible with 7038 B0. Correct execution is not
 * likely.
 * 
 * Hydra_Software_Devel/16   4/2/04 6:42p darnstein
 * PR 9080: Allow NTSC-J video format.
 * 
 * Hydra_Software_Devel/15   3/26/04 1:49p darnstein
 * PR 8543: Workaround for "last line of teletext" hardware bug.
 * 
 * Hydra_Software_Devel/14   3/18/04 11:18a darnstein
 * PR 9080: remove some debugging code.
 * 
 * Hydra_Software_Devel/13   3/12/04 5:52p darnstein
 * PR 9080: Teletext for NTSC is working, with a kludge for an A0 hardware
 * flaw.  There is a lot of debug code that will be removed later.
 * 
 * Hydra_Software_Devel/12   3/4/04 4:29p darnstein
 * PR 9080: improve allocation of teletext private data.  Add support for
 * progressive video formats.
 * 
 * Hydra_Software_Devel/11   2/27/04 6:09p darnstein
 * PR 9080: handle ALL of the PAL formats.
 * 
 * Hydra_Software_Devel/10   2/19/04 2:51p darnstein
 * PR 9493: Use new PAL format enums.
 * 
 * Hydra_Software_Devel/9   1/15/04 4:31p darnstein
 * PR 9080: fix problems with critical sections.
 * 
 * Hydra_Software_Devel/8   12/19/03 5:08p darnstein
 * PR 9080: adapt to changed BAVC enum for field polarity.
 * 
 * Hydra_Software_Devel/7   10/17/03 3:38p darnstein
 * VBI encoding can be disabled for one field time.
 * 
 * Hydra_Software_Devel/6   10/16/03 1:21p darnstein
 * Fix usage of BDBG_ENTER(), BDBG_LEAVE().
 * 
 * Hydra_Software_Devel/5   10/2/03 2:25p darnstein
 * Remove improper use of BERR_TRACE.
 * Put in some missing BDBG_LEAVE statements.
 * 
 * Hydra_Software_Devel/4   9/30/03 1:04p darnstein
 * Fix logic error discovered by Linux build script.
 * 
 * Hydra_Software_Devel/3   9/29/03 5:00p darnstein
 * Put in critical sections.
 * 
 * Hydra_Software_Devel/2   9/25/03 4:48p darnstein
 * BVBI module is mostly complete. The only things that I know are missing
 * are the critical sections, 656 support, macrovision support, and LOTS
 * OF TESTING.
 * 
 * Hydra_Software_Devel/1   9/23/03 11:02a darnstein
 * Teletext specific VBI software for encoding.
 * 
 ***************************************************************************/

#include "bstd.h"           /* standard types */
#include "bdbg.h"           /* Dbglib */
#include "bvbi.h"           /* VBI processing, this module. */
#include "bkni.h"			/* For critical sections */
#include "bvbi_priv.h"      /* VBI internal data structures */
#if (BVBI_P_NUM_TTE >= 1)
#include "bchp_tte_prim.h"  /* RDB info for primary TTE core */
#endif
#if (BVBI_P_NUM_TTE >= 2)
#include "bchp_tte_sec.h"   /* RDB info for secondary TTE core */
#endif
#if (BVBI_P_NUM_TTE >= 3)
#include "bchp_tte_tert.h"   /* RDB info for tertiary TTE core */
#endif
#if (BVBI_P_NUM_TTE_656 >= 1)
#include "bchp_tte_656.h"   /* RDB info for ITU-R 656 "bypass" TTE core */
#endif

/* Welcome to alias central */
#if (BCHP_CHIP == 7601) && (BCHP_VER >= BCHP_VER_B0)
	#define BCHP_TTE_PRIM_control_shift_direction_MSBToLSB \
		BCHP_TTE_PRIM_CONTROL_shift_direction_MSBToLSB
	#define BCHP_TTE_PRIM_control BCHP_TTE_PRIM_CONTROL
	#define BCHP_TTE_PRIM_control_shift_direction_LSBToMSB \
		BCHP_TTE_PRIM_CONTROL_shift_direction_LSBToMSB
	#define BCHP_TTE_PRIM_control_constant_phase_MASK \
		BCHP_TTE_PRIM_CONTROL_constant_phase_MASK
	#define BCHP_TTE_PRIM_control_anci656_enable_MASK \
		BCHP_TTE_PRIM_CONTROL_anci656_enable_MASK
	#define BCHP_TTE_PRIM_control_anci656_output_fc_MASK \
		BCHP_TTE_PRIM_CONTROL_anci656_output_fc_MASK
	#define BCHP_TTE_PRIM_control_shift_direction_MASK \
		BCHP_TTE_PRIM_CONTROL_shift_direction_MASK
	#define BCHP_TTE_PRIM_control_enable_tf_MASK \
		BCHP_TTE_PRIM_CONTROL_enable_tf_MASK
	#define BCHP_TTE_PRIM_control_enable_bf_MASK \
		BCHP_TTE_PRIM_CONTROL_enable_bf_MASK
	#define BCHP_TTE_PRIM_control_constant_phase_SHIFT \
		BCHP_TTE_PRIM_CONTROL_constant_phase_SHIFT
	#define BCHP_TTE_PRIM_control_anci656_enable_SHIFT \
		BCHP_TTE_PRIM_CONTROL_anci656_enable_SHIFT
	#define BCHP_TTE_PRIM_control_anci656_output_fc_SHIFT \
		BCHP_TTE_PRIM_CONTROL_anci656_output_fc_SHIFT
	#define BCHP_TTE_PRIM_control_shift_direction_SHIFT \
		BCHP_TTE_PRIM_CONTROL_shift_direction_SHIFT
	#define BCHP_TTE_PRIM_control_enable_tf_SHIFT \
		BCHP_TTE_PRIM_CONTROL_enable_tf_SHIFT
	#define BCHP_TTE_PRIM_control_enable_bf_SHIFT \
		BCHP_TTE_PRIM_CONTROL_enable_bf_SHIFT
	#define BCHP_TTE_PRIM_top_mask BCHP_TTE_PRIM_TOP_MASK
	#define BCHP_TTE_PRIM_bottom_mask BCHP_TTE_PRIM_BOTTOM_MASK
	#define BCHP_TTE_PRIM_output_format BCHP_TTE_PRIM_OUTPUT_FORMAT
	#define BCHP_TTE_PRIM_control_start_delay_MASK \
		BCHP_TTE_PRIM_CONTROL_start_delay_MASK
	#define BCHP_TTE_PRIM_control_teletext_mode_MASK \
		BCHP_TTE_PRIM_CONTROL_teletext_mode_MASK
	#define BCHP_TTE_PRIM_control_start_delay_SHIFT \
		BCHP_TTE_PRIM_CONTROL_start_delay_SHIFT
	#define BCHP_TTE_PRIM_control_teletext_mode_NABTS \
		BCHP_TTE_PRIM_CONTROL_teletext_mode_NABTS
	#define BCHP_TTE_PRIM_control_teletext_mode_SHIFT \
		BCHP_TTE_PRIM_CONTROL_teletext_mode_SHIFT
	#define BCHP_TTE_PRIM_output_format_output_attenuation_MASK \
		BCHP_TTE_PRIM_OUTPUT_FORMAT_output_attenuation_MASK
	#define BCHP_TTE_PRIM_control_teletext_mode_ETSTeletext \
		BCHP_TTE_PRIM_CONTROL_teletext_mode_ETSTeletext
	#define BCHP_TTE_PRIM_read_address_top BCHP_TTE_PRIM_READ_ADDRESS_TOP
	#define BCHP_TTE_PRIM_output_format_output_attenuation_SHIFT \
		BCHP_TTE_PRIM_OUTPUT_FORMAT_output_attenuation_SHIFT
	#define BCHP_TTE_PRIM_read_address_bottom BCHP_TTE_PRIM_READ_ADDRESS_BOTTOM
	#define BCHP_TTE_PRIM_lines_active BCHP_TTE_PRIM_LINES_ACTIVE
	#define BCHP_TTE_PRIM_status BCHP_TTE_PRIM_STATUS
	#define BCHP_TTE_PRIM_status_data_sent_tf_MASK \
		BCHP_TTE_PRIM_STATUS_data_sent_tf_MASK
	#define BCHP_TTE_PRIM_lines_active_startline_tf_MASK \
		BCHP_TTE_PRIM_LINES_ACTIVE_startline_tf_MASK
	#define BCHP_TTE_PRIM_lines_active_startline_tf_SHIFT \
		BCHP_TTE_PRIM_LINES_ACTIVE_startline_tf_SHIFT
	#define BCHP_TTE_PRIM_status_data_sent_bf_MASK \
		BCHP_TTE_PRIM_STATUS_data_sent_bf_MASK
	#define BCHP_TTE_PRIM_lines_active_startline_bf_MASK \
		BCHP_TTE_PRIM_LINES_ACTIVE_startline_bf_MASK
	#define BCHP_TTE_PRIM_lines_active_startline_bf_SHIFT \
		BCHP_TTE_PRIM_LINES_ACTIVE_startline_bf_SHIFT
#endif

BDBG_MODULE(BVBI);

/* Welcome to alias central */
#if (BCHP_CHIP == 7118) || (BCHP_CHIP == 7400) || (BCHP_CHIP == 7401) || \
    (BCHP_CHIP == 7403) || (BCHP_CHIP == 3563) || (BCHP_CHIP == 7038) || \
	(BCHP_CHIP == 7438)
	#define BCHP_TTE_PRIM_Reset             BCHP_TTE_PRIM_reset
	#define BCHP_TTE_PRIM_Reset_reset_SHIFT BCHP_TTE_PRIM_reset_reset_SHIFT
#endif
#if (BCHP_CHIP == 7440) 
	#define BCHP_TTE_PRIM_Reset             BCHP_TTE_PRIM_reset
	#define BCHP_TTE_PRIM_Reset_reset_SHIFT BCHP_TTE_PRIM_reset_reset_SHIFT
#endif


/***************************************************************************
* Forward declarations of static (private) functions
***************************************************************************/
static uint32_t P_GetCoreOffset (bool is656, uint8_t hwCoreIndex);


/***************************************************************************
* Implementation of "BVBI_" API functions
***************************************************************************/


/***************************************************************************
* Implementation of supporting teletext functions that are not in API
***************************************************************************/

static BERR_Code BVBI_P_TT_Enc_Program_isr (
	BREG_Handle hReg,
	BMEM_Handle hMem,
	bool is656, 
	uint8_t hwCoreIndex,
	bool bActive,
	bool bXserActive,
	BFMT_VideoFmt eVideoFormat,
	bool tteShiftDirMsb2Lsb,
	BVBI_XSER_Settings* xserSettings,
	BVBI_P_TTData* topData,
	BVBI_P_TTData* botData
);

static void BVBI_P_TT_Enc_Init_isr (
	BREG_Handle hReg, bool is656, uint8_t hwCoreIndex);

/***************************************************************************
 *
 */
void BVBI_P_TT_Enc_Init (BREG_Handle hReg, bool is656, uint8_t hwCoreIndex)
{
	BKNI_EnterCriticalSection();
	BVBI_P_TT_Enc_Init_isr (hReg, is656, hwCoreIndex);
	BKNI_LeaveCriticalSection();
}

/***************************************************************************
 *
 */
void BVBI_P_TT_Enc_Init_isr (BREG_Handle hReg, bool is656, uint8_t hwCoreIndex)
{
	BDBG_ENTER(BVBI_P_TT_Enc_Init_isr);

	BVBI_P_VIE_SoftReset (hReg, is656, hwCoreIndex, BVBI_P_SELECT_TT);

	BDBG_LEAVE(BVBI_P_TT_Enc_Init_isr);
}


/***************************************************************************
 *
 */
BERR_Code BVBI_P_TT_Enc_Program (
	BREG_Handle hReg,
	BMEM_Handle hMem,
	bool is656, 
	uint8_t hwCoreIndex,
	bool bActive,
	bool bXserActive,
	BFMT_VideoFmt eVideoFormat,
	bool tteShiftDirMsb2Lsb,
	BVBI_XSER_Settings* xserSettings,
	BVBI_P_TTData* topData,
	BVBI_P_TTData* botData
)
{
	BERR_Code retval;
	BDBG_ENTER(BVBI_P_TT_Enc_Program);
	BKNI_EnterCriticalSection();
	retval = BVBI_P_TT_Enc_Program_isr (
		hReg, hMem, is656, hwCoreIndex, bActive, bXserActive, 
		eVideoFormat, tteShiftDirMsb2Lsb, xserSettings, topData, botData);
	BKNI_LeaveCriticalSection();
	BDBG_LEAVE(BVBI_P_TT_Enc_Program);
	return retval;
}


/***************************************************************************
 *
 */
static BERR_Code BVBI_P_TT_Enc_Program_isr (
	BREG_Handle hReg,
	BMEM_Handle hMem,
	bool is656, 
	uint8_t hwCoreIndex,
	bool bActive,
	bool bXserActive,
	BFMT_VideoFmt eVideoFormat,
	bool tteShiftDirMsb2Lsb,
	BVBI_XSER_Settings* xserSettings,
	BVBI_P_TTData* topData,
	BVBI_P_TTData* botData
)
{
	uint32_t ulControlReg;
	uint32_t ulFormatReg;
#if (BVBI_P_HAS_XSER_TT >= 1)
	uint32_t iSerialPortMode;
	uint32_t iSerialPort;
#endif

	uint8_t  ucNumLinesTF;
	uint8_t  ucNumLinesBF;
	uint8_t  ucBytesPerLine;

	uint32_t offset;
	uint32_t ulCoreOffset;
	uint32_t ulShiftDir;
	BERR_Code eErr;

#if (BVBI_P_HAS_XSER_TT >= 1)
#else
	BSTD_UNUSED (bXserActive);
	BSTD_UNUSED (xserSettings);
#endif

	BDBG_ENTER(BVBI_P_TT_Enc_Program_isr);

	/* Figure out which encoder core to use */
	ulCoreOffset = P_GetCoreOffset (is656, hwCoreIndex);
	if (ulCoreOffset == 0xFFFFFFFF)
	{
		/* This should never happen!  This parameter was checked by
		   BVBI_Encode_Create() */
		BDBG_LEAVE(BVBI_P_TT_Enc_Program_isr);
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* TODO: Verify little endian */

	/* If user wants to turn off teletext processing, just reset the
	   entire core. */
	if (!bActive)
	{
		BVBI_P_TT_Enc_Init_isr (hReg, is656, hwCoreIndex);
		BDBG_LEAVE(BVBI_P_TT_Enc_Program_isr);
		return BERR_SUCCESS;
	}

#if (BVBI_P_HAS_XSER_TT >= 1)
	iSerialPort = (bXserActive ? 
		BCHP_TTE_PRIM_control_serial_port_ENABLE : 
		BCHP_TTE_PRIM_control_serial_port_DISABLE);
	switch (xserSettings->xsSerialDataContent)
	{
	case BVBI_TTserialDataContent_None:
		iSerialPortMode = BCHP_TTE_PRIM_control_serial_port_mode_DATA_ONLY;
		iSerialPort = BCHP_TTE_PRIM_control_serial_port_DISABLE;
		break;
	case BVBI_TTserialDataContent_DataOnly:
		iSerialPortMode = BCHP_TTE_PRIM_control_serial_port_mode_DATA_ONLY;
		break;
	case BVBI_TTserialDataContent_DataMag:
		iSerialPortMode = BCHP_TTE_PRIM_control_serial_port_mode_MAGAZINE_DATA;
		break;
	case BVBI_TTserialDataContent_DataMagFrm:
		iSerialPortMode = 
			BCHP_TTE_PRIM_control_serial_port_mode_FRM_MAGAZINE_DATA;
		break;
	case BVBI_TTserialDataContent_DataMagFrmRun:
		iSerialPortMode = 
			BCHP_TTE_PRIM_control_serial_port_mode_RUNIN_FRM_MAGAZINE_DATA;
		break;
	default:
		iSerialPortMode = BCHP_TTE_PRIM_control_serial_port_mode_DATA_ONLY;
		/* This should never happen!  This parameter was checked by
		   BVBI_Encode_Create() */
		BDBG_LEAVE(BVBI_P_TT_Enc_Program_isr);
		return BERR_TRACE(BERR_INVALID_PARAMETER);
		break;
	}
#endif

	if (tteShiftDirMsb2Lsb)
		ulShiftDir = BCHP_TTE_PRIM_control_shift_direction_MSBToLSB;
	else
		ulShiftDir = BCHP_TTE_PRIM_control_shift_direction_LSBToMSB;

	/* Start programming the TTE control register */
    ulControlReg = BREG_Read32 (hReg, BCHP_TTE_PRIM_control + ulCoreOffset);
	ulControlReg &= ~(
#if (BVBI_P_HAS_XSER_TT >= 1)
		BCHP_MASK       (TTE_PRIM_control, serial_port_mode         ) |
		BCHP_MASK       (TTE_PRIM_control, serial_port              ) |
#endif
		BCHP_MASK       (TTE_PRIM_control, constant_phase           ) |
		BCHP_MASK       (TTE_PRIM_control, anci656_enable           ) |
		BCHP_MASK       (TTE_PRIM_control, anci656_output_fc        ) |
		BCHP_MASK       (TTE_PRIM_control, shift_direction          ) |
		BCHP_MASK       (TTE_PRIM_control, enable_tf                ) |
		BCHP_MASK       (TTE_PRIM_control, enable_bf                ) );
	ulControlReg |= (
#if (BVBI_P_HAS_XSER_TT >= 1)
		 BCHP_FIELD_DATA (TTE_PRIM_control, serial_port_mode, 
													 iSerialPortMode) |
		 BCHP_FIELD_DATA (TTE_PRIM_control, serial_port, iSerialPort) |
#endif
		BCHP_FIELD_DATA (TTE_PRIM_control, constant_phase,         0) |
		BCHP_FIELD_DATA (TTE_PRIM_control, anci656_enable,         1) |
		BCHP_FIELD_DATA (TTE_PRIM_control, anci656_output_fc,      1) |
		BCHP_FIELD_DATA (TTE_PRIM_control, shift_direction, ulShiftDir) |
		BCHP_FIELD_DATA (TTE_PRIM_control, enable_tf,              1) |
		BCHP_FIELD_DATA (TTE_PRIM_control, enable_bf,              1) );

    /* Program the TTE top mask register */
    BREG_Write32 (hReg, BCHP_TTE_PRIM_top_mask + ulCoreOffset, 0x0);

    /* Program the TTE bottom mask register */
    BREG_Write32 (hReg, BCHP_TTE_PRIM_bottom_mask + ulCoreOffset, 0x0);

	/* Start programming the output format register */
	ulFormatReg = 
		BREG_Read32 (hReg, BCHP_TTE_PRIM_output_format + ulCoreOffset);

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
		ucBytesPerLine =  34;

		/* Continue programming the control register */
		ulControlReg &= ~(
			BCHP_MASK       (TTE_PRIM_control, start_delay         ) |
			BCHP_MASK       (TTE_PRIM_control, teletext_mode       ) );
		ulControlReg |= (
			BCHP_FIELD_DATA (TTE_PRIM_control, start_delay,    0x1F) |
			BCHP_FIELD_ENUM (TTE_PRIM_control, teletext_mode, NABTS) );

		/* Continue programming the output_format register */
		ulFormatReg &=
			~BCHP_MASK       (TTE_PRIM_output_format, output_attenuation     );
		ulFormatReg |=
			 BCHP_FIELD_DATA (TTE_PRIM_output_format, output_attenuation,0x63);

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
        /* 576I specific settings */

		ucNumLinesTF   = 17;
		ucNumLinesBF   = 18;
		ucBytesPerLine = 43;

		/* Continue programming the control register */
		ulControlReg &= ~(
			BCHP_MASK       (TTE_PRIM_control, start_delay               ) |
			BCHP_MASK       (TTE_PRIM_control, teletext_mode             ) );
		ulControlReg |= (
			BCHP_FIELD_DATA (TTE_PRIM_control, start_delay,          0x00) |
			BCHP_FIELD_ENUM (TTE_PRIM_control, teletext_mode, ETSTeletext) );

		/* Continue programming the output_format register */
		ulFormatReg &=
			~BCHP_MASK       (TTE_PRIM_output_format, output_attenuation      );
		ulFormatReg |=
			 BCHP_FIELD_DATA (TTE_PRIM_output_format, output_attenuation, 0x5a);

		break;

	default:
		BDBG_LEAVE(BVBI_P_TT_Enc_Program_isr);
		return BERR_TRACE (BERR_INVALID_PARAMETER);
		break;
	}

	/* Prepare to send data in the encode handle */
	eErr = BERR_TRACE (BMEM_ConvertAddressToOffset ( 
		hMem, topData->pucData, &offset));
	BDBG_ASSERT (eErr == BERR_SUCCESS);
	BREG_Write32 (hReg, BCHP_TTE_PRIM_read_address_top + ulCoreOffset, offset);
	eErr = BERR_TRACE (BMEM_ConvertAddressToOffset ( 
		hMem, botData->pucData, &offset));
	BDBG_ASSERT (eErr == BERR_SUCCESS);
	BREG_Write32 (
		hReg, BCHP_TTE_PRIM_read_address_bottom + ulCoreOffset, offset);

	/* Update the field handles that send the data */
	topData->ucLines    = ucNumLinesTF;
	topData->ucLineSize = ucBytesPerLine;
	botData->ucLines    = ucNumLinesBF;
	botData->ucLineSize = ucBytesPerLine;

	/* write the three registers with updated values */
    BREG_Write32 (
		hReg, BCHP_TTE_PRIM_control       + ulCoreOffset, ulControlReg);
    BREG_Write32 (
		hReg, BCHP_TTE_PRIM_output_format + ulCoreOffset,  ulFormatReg);

	BDBG_LEAVE(BVBI_P_TT_Enc_Program_isr);
	return BERR_SUCCESS;
}


/***************************************************************************
 *
 */
uint32_t BVBI_P_TT_Encode_Data_isr ( 
	BREG_Handle hReg, 
	BMEM_Handle hMem,
	bool is656, 
	uint8_t hwCoreIndex,
	BFMT_VideoFmt eVideoFormat,
	BAVC_Polarity polarity,
	bool bPR18010_bad_line_number,
	BVBI_P_TTData* pTTDataNext )
{
/*
	Programming note: the implementation here assumes that the bitfield layout
	within registers is the same for all teletext encoder cores in the chip.  

	If a chip is built that has multiple teletext encoder cores that are not
	identical, then this routine will have to be redesigned.
*/
	uint32_t ulCoreOffset;
	uint32_t H_ReAdTop;
	uint32_t H_ReAdBot;
	uint32_t H_MaskTop;
	uint32_t H_MaskBot;
	uint32_t H_Lines;
	uint32_t H_Status;
	uint32_t ulStatusReg;
	uint16_t usStartLineTF;
	uint16_t usStartLineBF;
	uint8_t  ucMinLines;
	uint8_t  ucMinLineSize;
	uint32_t ulLinesReg;
	uint32_t offset;
	uint32_t lineMask;
	uint32_t ulErrInfo = 0;

	/* Debug code 
	uint8_t* printme = 0;
	*/

#if (BVBI_P_NUM_TTE_656 >= 1)
#else
	BSTD_UNUSED (bPR18010_bad_line_number);
#endif

	BDBG_ENTER(BVBI_P_TT_Encode_Data_isr);

	/* Figure out which encoder core to use */
	ulCoreOffset = P_GetCoreOffset (is656, hwCoreIndex);
	if (ulCoreOffset == 0xFFFFFFFF)
	{
		/* This should never happen!  This parameter was checked by
		   BVBI_Encode_Create() */
		BDBG_LEAVE(BVBI_P_TT_Encode_Data_isr);
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	H_ReAdTop = BCHP_TTE_PRIM_read_address_top + ulCoreOffset;
	H_ReAdBot = BCHP_TTE_PRIM_read_address_bottom + ulCoreOffset;
	H_MaskTop = BCHP_TTE_PRIM_top_mask + ulCoreOffset;
	H_MaskBot = BCHP_TTE_PRIM_bottom_mask + ulCoreOffset;
	H_Lines   = BCHP_TTE_PRIM_lines_active + ulCoreOffset;
	H_Status  = BCHP_TTE_PRIM_status + ulCoreOffset;

	/* Verify that field handle is big enough to hold the TT data */
	switch (eVideoFormat)
	{
    case BFMT_VideoFmt_eNTSC:
    case BFMT_VideoFmt_eNTSC_J:
	case BFMT_VideoFmt_e720x482_NTSC:
	case BFMT_VideoFmt_e720x482_NTSC_J:
    case BFMT_VideoFmt_ePAL_M:
		ucMinLines    = 13;
		ucMinLineSize = 34;
		usStartLineTF  =  10 - 1;
		usStartLineBF  = 273 - 263;
#if (BVBI_P_NUM_TTE_656 >= 1)
		if (is656 && !bPR18010_bad_line_number)
		{
			usStartLineTF -= 1;
			usStartLineBF -= 1;
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
		ucMinLines    = 18;
		ucMinLineSize = 43;
		usStartLineTF  = 6 - 1;
		usStartLineBF  = 318 - 313;
#if (BVBI_P_NUM_TTE_656 >= 1)
		if (is656 && !bPR18010_bad_line_number)
		{
			usStartLineTF -= 1;
			usStartLineBF -= 1;
		}
#endif
		/* PR18343 */
		usStartLineTF -= 1;
		break;
	default:
		/* This should never happen! */ 
		ulErrInfo = (uint32_t)(-1);
		BDBG_LEAVE(BVBI_P_TT_Encode_Data_isr);
		return ulErrInfo;
		break;
	}
	if ( (pTTDataNext->ucLines     >    ucMinLines) || 
		 (pTTDataNext->ucLineSize != ucMinLineSize)    )
	{
		ulErrInfo |= BVBI_LINE_ERROR_FLDH_CONFLICT;
		BDBG_LEAVE(BVBI_P_TT_Encode_Data_isr);
		return ulErrInfo;
	}

	/* Read the status register */
	ulStatusReg = BREG_Read32 (hReg, H_Status);

	/* Start programming the lines_active register */
	ulLinesReg = BREG_Read32 (hReg, H_Lines);

	/* If top field */
	if (polarity == BAVC_Polarity_eTopField)
	{
		/* Check for hardware busy */
		if ((ulStatusReg & BCHP_MASK (TTE_PRIM_status, data_sent_tf)) == 0)
		{
			ulErrInfo |= BVBI_LINE_ERROR_TELETEXT_OVERRUN;
			goto done;
		}

		/* Will clear hardware status */
		ulStatusReg = BCHP_MASK (TTE_PRIM_status, data_sent_tf);

		/* Give hardware a new place to encode data from */
		if (BMEM_ConvertAddressToOffset (hMem, pTTDataNext->pucData, &offset) !=
			BERR_SUCCESS)
		{
			ulErrInfo = (uint32_t)(-1);
			goto done;
		}
		BREG_Write32 (hReg, H_ReAdTop, offset);

		/* Program the masking register */
		lineMask =  pTTDataNext->lineMask;
#ifdef BVBI_P_TTE_WA15
		/* This causes problems on 3563-C0 */
		BREG_Write32 (hReg, H_MaskTop, lineMask);
#else
		BREG_Write32 (hReg, H_MaskTop, 0xFFFFFFFF);
		*(uint32_t*)(pTTDataNext->pucData) = lineMask;
#endif

		/* Continue programming the lines_active register */
		ulLinesReg &= ~BCHP_MASK (TTE_PRIM_lines_active, startline_tf);
		ulLinesReg |=  BCHP_FIELD_DATA (
			TTE_PRIM_lines_active, startline_tf, 
			pTTDataNext->firstLine + usStartLineTF);

		/* Debug code 
		printme = pTTDataNext->pucData;
		*/
	}
	else /* Bottom field */
	{
		/* Check for hardware busy */
		if ((ulStatusReg & BCHP_MASK (TTE_PRIM_status, data_sent_bf)) == 0)
		{
			ulErrInfo |= BVBI_LINE_ERROR_TELETEXT_OVERRUN;
			goto done;
		}

		/* Will clear hardware status */
		ulStatusReg = BCHP_MASK (TTE_PRIM_status, data_sent_bf);

		/* Give hardware a new place to encode data from */
		if (BMEM_ConvertAddressToOffset (hMem, pTTDataNext->pucData, &offset) !=
			BERR_SUCCESS)
		{
			ulErrInfo = (uint32_t)(-1);
			goto done;
		}
		BREG_Write32 (hReg, H_ReAdBot, offset);

		/* Program the masking register */
		lineMask =  pTTDataNext->lineMask;
#ifdef BVBI_P_TTE_WA15
		/* This causes problems on 3563-C0 */
		BREG_Write32 (hReg, H_MaskBot, lineMask);
#else
		BREG_Write32 (hReg, H_MaskBot, 0xFFFFFFFF);
		*(uint32_t*)(pTTDataNext->pucData) = lineMask;
#endif

		/* Continue programming the lines_active register */
		ulLinesReg &= ~BCHP_MASK (TTE_PRIM_lines_active, startline_bf);
		ulLinesReg |=  BCHP_FIELD_DATA (
			TTE_PRIM_lines_active, startline_bf,   
			pTTDataNext->firstLine + usStartLineBF);

		/* Debug code 
		printme = pTTDataNext->pucData;
		*/
	}

	/* Finish programming the lines_active register */
    BREG_Write32 (hReg, H_Lines, ulLinesReg);

	/* Finish clearing status */
    BREG_Write32 (hReg, H_Status, ulStatusReg);

	/* Debug code */
	/*
	{
	static uint32_t dcounter = 0;
	++dcounter;
	if ((dcounter > 80) && (dcounter < 150))
	{
		if (printme)
		{
			uint32_t mask = *(uint32_t*)printme;
			char* p1 = printme + 4;
			char* p2 = printme + (4 + 34);
			printf ("%d%c: At %08x: encoding M:%08x \"%s\" \"%s\"\n", 
				dcounter, 
				(polarity == BAVC_Polarity_eTopField) ? 'T' : 'B',
				offset,
				mask, p1, p2);
		}
		else
			printf ("%d%c: Did not encode anything\n",
				dcounter, 
				(polarity == BAVC_Polarity_eTopField) ? 'T' : 'B');
		{
		}
	}
	}
	*/

done:
	BDBG_LEAVE(BVBI_P_TT_Encode_Data_isr);
	return ulErrInfo;
}

/***************************************************************************
 *
 */
BERR_Code BVBI_P_TT_Encode_Enable_isr (
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

	BDBG_ENTER(BVBI_P_TT_Encode_Enable_isr);

	/* Figure out which encoder core to use */
	ulCoreOffset = P_GetCoreOffset (is656, hwCoreIndex);
	if (ulCoreOffset == 0xFFFFFFFF)
	{
		/* This should never happen!  This parameter was checked by
		   BVBI_Encode_Create() */
		BDBG_LEAVE(BVBI_P_TT_Encode_Enable_isr);
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

    ulControlReg = BREG_Read32 ( hReg, BCHP_TTE_PRIM_control + ulCoreOffset );
	ulControlReg &= ~(
		BCHP_MASK (TTE_PRIM_control, enable_tf) |
		BCHP_MASK (TTE_PRIM_control, enable_bf) );
	if (bEnable)
	{
		ulControlReg |= (
			BCHP_FIELD_DATA (TTE_PRIM_control, enable_tf, 1) |
			BCHP_FIELD_DATA (TTE_PRIM_control, enable_bf, 1) );
	}
	else
	{
		ulControlReg |= (
			BCHP_FIELD_DATA (TTE_PRIM_control, enable_tf, 0) |
			BCHP_FIELD_DATA (TTE_PRIM_control, enable_bf, 0) );
	}
	BREG_Write32 ( hReg, BCHP_TTE_PRIM_control + ulCoreOffset, ulControlReg );

	BDBG_LEAVE(BVBI_P_TT_Encode_Enable_isr);
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
#if (BVBI_P_NUM_TTE_656 >= 1)
		ulCoreOffset = (BCHP_TTE_656_status - BCHP_TTE_PRIM_status);
#endif
	}
	else
	{
		switch (hwCoreIndex)
		{
#if (BVBI_P_NUM_TTE >= 1)
		case 0:
			ulCoreOffset = 0;
			break;
#endif
#if (BVBI_P_NUM_TTE >= 2)
		case 1:
			ulCoreOffset = (BCHP_TTE_SEC_status - BCHP_TTE_PRIM_status);
			break;
#endif
#if (BVBI_P_NUM_TTE >= 3)
		case 2:
			ulCoreOffset = (BCHP_TTE_SEC_status - BCHP_TTE_PRIM_status);
			break;
#endif
		default:
			break;
		}
	}

	return ulCoreOffset;
}

/* End of file */
