/***************************************************************************
 *     Copyright (c) 2003-2008, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvbi_cc.c $
 * $brcm_Revision: Hydra_Software_Devel/3 $
 * $brcm_Date: 12/4/08 6:12p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vbi/7400/bvbi_cc.c $
 * 
 * Hydra_Software_Devel/3   12/4/08 6:12p darnstein
 * PR45819: remove some obsolete "alias central" logic.
 * 
 * Hydra_Software_Devel/2   12/3/08 7:56p darnstein
 * PR45819: New, more modular form of most BVBI source files.
 * 
 * Hydra_Software_Devel/64   5/13/08 12:02p darnstein
 * PR42416: adjust decoder video line number when the frame comb work-
 * around is in place.
 * 
 * Hydra_Software_Devel/63   12/19/07 2:06p darnstein
 * PR32758: apply register settings for PAL CC decode, provided by Brad
 * Grossman.
 * 
 * Hydra_Software_Devel/62   10/31/07 7:13p darnstein
 * PR35463: Implement CC PAL decoding solution contributed by xkliu.
 * 
 * Hydra_Software_Devel/61   2/20/07 10:43a darnstein
 * PR27521: Apply software workaround. The TTE core is reset, at every
 * video field. Also, the encoder output for the field NOT in use is
 * disabled, at every video field.
 * 
 * Hydra_Software_Devel/60   1/2/07 4:20p darnstein
 * PR26872: Mechanically add SECAM to all cases where PAL formats are
 * accepted.
 * 
 * Hydra_Software_Devel/59   12/18/06 1:01p darnstein
 * PR24573: Adapt to an odd register name for BCM97440-B0 chipset.
 * 
 * Hydra_Software_Devel/58   12/15/06 3:45p darnstein
 * PR24573: Adapt to an odd register name for BCM97440-A0 chipset.
 * 
 * Hydra_Software_Devel/57   12/14/06 7:16p darnstein
 * PR25990: Can compile for BCM97400-B0 now.
 * 
 * Hydra_Software_Devel/56   10/19/06 5:47p darnstein
 * PR24979: change horizontal offset of waveform when sending NTSC.
 * Theory: VEC microcode changed in such a way that the register values
 * are interpreted differently.
 * 
 * Hydra_Software_Devel/55   9/26/06 7:37p darnstein
 * PR24573: The BVBI porting interface module now compiles for the 7440
 * chipset. Correct operation has not been verified.
 * 
 * Hydra_Software_Devel/54   9/20/06 5:21p darnstein
 * PR23621: The reset register name has been changed in RDB as per my
 * request.
 * 
 * Hydra_Software_Devel/53   8/31/06 2:09p darnstein
 * PR23869: clean up the handling of multiple VECs and VDECs.
 * 
 * Hydra_Software_Devel/52   8/18/06 6:50p darnstein
 * PR23178: basic compile on 93563 is possible.
 * 
 * Hydra_Software_Devel/51   7/20/06 2:22p darnstein
 * PR21688: Use the new hardware soft reset scheme for later model chips.
 * 
 * Hydra_Software_Devel/50   4/12/06 8:04p darnstein
 * PR18010: When in PR18010 compatibility mode, put in 0x0400 for line
 * number register fields. This is what the external MPEG encoder
 * expects, apparently.
 * 
 * Hydra_Software_Devel/49   1/16/06 12:39p darnstein
 * PR18903: Use BSTD_UNUSED macro to suppress compiler warnings in
 * VxWorks.
 * 
 * Hydra_Software_Devel/48   12/5/05 7:29p darnstein
 * PR18010: Implement work-around for the bad line number, together with
 * an exception for customers that now depend on the bad line number.
 * 
 * Hydra_Software_Devel/47   9/23/05 2:46p darnstein
 * PR13750: Proper use of BERR_TRACE and BERR_CODEs.
 * 
 * Hydra_Software_Devel/46   9/20/05 5:46p darnstein
 * PR17162: Use new settings for GAIN and DELAY in CCE core. New settings
 * only apply to 97038-C0 and later chips.
 * 
 * Hydra_Software_Devel/45   9/19/05 2:56p darnstein
 * PR17151: Check for chip name where needed. Also, convert to new scheme
 * for testing chip revisions (BCHP_VER).
 * 
 * Hydra_Software_Devel/44   8/22/05 8:11p darnstein
 * PR16057: To support many different chips, use private #defines that
 * specify number of VECs, VDECs, and (separately) pass-through VECs.
 * 
 * Hydra_Software_Devel/43   7/25/05 5:19p darnstein
 * PR15630, PR16218: Avoid reset of the CCE cores. This seems to cause
 * the VEC to hang.
 * 
 * Hydra_Software_Devel/42   6/16/05 3:12p darnstein
 * PR 14487: fix typing error in previous checkin.
 * 
 * Hydra_Software_Devel/41   6/15/05 5:05p darnstein
 * PR 14487: Put in runtime checking for workaround for hardware PR 13583.
 * 
 * Hydra_Software_Devel/40   6/15/05 12:03p darnstein
 * PR 14487: bring up VBI decoding on 97038-C1.
 * 
 * Hydra_Software_Devel/39   4/14/05 12:00p darnstein
 * PR 14720: For each reference to BCHP_REV_B[01], add a BCHP_REV_B2.
 * Still untested.
 * 
 * Hydra_Software_Devel/38   4/4/05 3:42p darnstein
 * PR 14696: Get rid of a compiler warning (minor).
 * 
 * Hydra_Software_Devel/37   3/18/05 2:38p darnstein
 * PR 14487: Eliminate an irritating compiler warning.
 * 
 * Hydra_Software_Devel/36   3/17/05 6:09p darnstein
 * PR 14472: Eliminate references to secondary VEC if chip is 3560.
 * 
 * Hydra_Software_Devel/35   3/17/05 1:20p darnstein
 * PR 14487: work around line counter bug in hardware.
 * 
 * Hydra_Software_Devel/34   3/11/05 3:48p darnstein
 * PR 14426: use new _0 names for VDEC cores.
 * 
 * Hydra_Software_Devel/33   2/14/05 6:15p darnstein
 * PR 14116:  Make accesses to Gain_Delay register conditional.  Avoid bus
 * error.
 * 
 * Hydra_Software_Devel/32   1/5/05 4:13p jasonh
 * PR 13700: Fixed VBI compile issues for 7038 C0.
 * 
 * Hydra_Software_Devel/31   11/17/04 1:40p darnstein
 * PR 8365 : Removed Parity bit clear in BVBI_Field_GetCCData_isr().
 * 
 * Hydra_Software_Devel/30   7/16/04 7:06p darnstein
 * PR 9080: merge in 656 input and output work. Some testing and debugging
 * remains to be done.
 * 
 * Hydra_Software_Devel/I656/2   7/8/04 7:45p darnstein
 * ITU-R 656 decoding of VBI seems to be ready for bring up.  Expect bugs.
 * 
 * Hydra_Software_Devel/I656/1   6/28/04 1:10p darnstein
 * 656 output is ready for testing.
 * 
 * Hydra_Software_Devel/29   5/24/04 8:06p jasonh
 * PR 11189: Merge down from B0 to main-line
 * 
 * Hydra_Software_Devel/Refsw_Devel_7038_B0/2   5/6/04 6:17p darnstein
 * PR 9080: for encoding, set delay_count to x16. Per Kin Fan Ho.
 * 
 * Hydra_Software_Devel/Refsw_Devel_7038_B0/1   4/26/04 6:21p darnstein
 * PR 9080: Set the correct delay_count value for encoding.  This register
 * field was broken in the A0 chip.  It works fine now.
 * 
 * Hydra_Software_Devel/26   4/26/04 5:52p darnstein
 * PR 9080: fix the gain value for closed caption output.  Kin Fan Ho
 * found the amplitutde of the waveform was much too low.
 * 
 * Hydra_Software_Devel/25   4/2/04 6:41p darnstein
 * PR 9080: Allow NTSC-J video format.
 * 
 * Hydra_Software_Devel/24   3/4/04 4:25p darnstein
 * PR 9080: add some functionality needed for progressive video formats.
 * 
 * Hydra_Software_Devel/23   2/27/04 6:09p darnstein
 * PR 9080: handle ALL of the PAL formats.
 * 
 * Hydra_Software_Devel/22   2/19/04 2:50p darnstein
 * PR 9493: Use new PAL format enums.
 * 
 * Hydra_Software_Devel/21   2/6/04 11:49a darnstein
 * PR 9080: better handle VBI decoding errors.
 * 
 * Hydra_Software_Devel/20   2/3/04 4:42p darnstein
 * PR 9080: fix logic errors in handling decoding errors. A few other
 * minor fixes.
 * 
 * Hydra_Software_Devel/19   1/27/04 6:23p darnstein
 * PR 9080: Don't check for overrun errors on encoding.  It is only
 * possible for closed caption encoding, anyway.
 * 
 * Hydra_Software_Devel/18   1/23/04 4:10p darnstein
 * PR 9080: Closed caption encoding and decoding seems to work for NTSC
 * now.
 * 
 * Hydra_Software_Devel/17   1/21/04 5:21p darnstein
 * PR 9080: The porting interface API refers to closed caption data as a
 * pair of bytes per video field.  Internally, the CC data is still
 * stored as a 16-bit quantity, for speedy access to the chip registers.
 * 
 * Hydra_Software_Devel/16   1/15/04 4:27p darnstein
 * PR 9080: fix problems with critical sections. Other clean-ups.  Closed
 * caption encoding seems to work now.
 * 
 * Hydra_Software_Devel/15   12/19/03 5:06p darnstein
 * PR 9080: adapt to changed BAVC enum for field polarity.
 * 
 * Hydra_Software_Devel/14   12/1/03 2:28p darnstein
 * Put default: in a switch statement in order to prevent a lint warning.
 * 
 * Hydra_Software_Devel/13   11/25/03 1:26p darnstein
 * Correct name of a function.
 * 
 * Hydra_Software_Devel/12   10/17/03 3:38p darnstein
 * VBI encoding can be disabled for one field time.
 * 
 * Hydra_Software_Devel/11   10/16/03 4:25p darnstein
 * Use revised #define BAVC_SourceId_Vdec0.
 * 
 * Hydra_Software_Devel/10   10/16/03 1:21p darnstein
 * Fix usage of BDBG_ENTER(), BDBG_LEAVE().
 * 
 * Hydra_Software_Devel/9   10/9/03 3:57p darnstein
 * Change name of several API functions to indicate that they may be
 * called from an ISR.
 * 
 * Hydra_Software_Devel/8   10/2/03 2:25p darnstein
 * Remove improper use of BERR_TRACE.
 * Put in some missing BDBG_LEAVE statements.
 * 
 * Hydra_Software_Devel/7   9/29/03 5:00p darnstein
 * Put in critical sections.
 * 
 * Hydra_Software_Devel/6   9/25/03 4:48p darnstein
 * BVBI module is mostly complete. The only things that I know are missing
 * are the critical sections, 656 support, macrovision support, and LOTS
 * OF TESTING.
 * 
 * Hydra_Software_Devel/5   9/12/03 6:50p darnstein
 * Done except for VPS, teletext, critical sections, and (lots of)
 * testing.
 * 
 * Hydra_Software_Devel/4   9/9/03 7:53p darnstein
 * I think all API functions have been implemented, though some with
 * stubs.
 * 
 * Hydra_Software_Devel/3   9/2/03 8:08p darnstein
 * Still not ready for use.
 * 
 * Hydra_Software_Devel/2   8/29/03 6:47p darnstein
 * Closed caption specific code to support VBI processing.
 * 
 * Hydra_Software_Devel/1   8/29/03 10:55a darnstein
 * Closed caption specific code to support VBI processing.
 * 
 ***************************************************************************/

#include "bstd.h"           /* standard types */
#include "bdbg.h"           /* Dbglib */
#include "bkni.h"			/* For critical sections */
#include "bvbi.h"           /* VBI processing, this module. */
#include "bvbi_priv.h"      /* VBI internal data structures */

BDBG_MODULE(BVBI);

/***************************************************************************
* Implementation of "BVBI_" API functions
***************************************************************************/

/***************************************************************************
 *
 */
BERR_Code BVBI_Field_GetCCData_isr (
	BVBI_Field_Handle fieldHandle, 
	uint8_t           *pucLowByte,	
	uint8_t          *pucHighByte
)
{
	BVBI_P_Field_Handle* pVbi_Fld;
	BERR_Code eErr = BERR_SUCCESS;

	BDBG_ENTER(BVBI_Field_GetCCData_isr);

	/* check parameters */
	BVBI_P_GET_FIELD_CONTEXT(fieldHandle, pVbi_Fld);
	if((!pVbi_Fld) || (!pucLowByte) || (!pucHighByte))
	{
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* Verify that data is present on this field handle */
	if (!(pVbi_Fld->ulWhichPresent & BVBI_P_SELECT_CC))
		return (BVBI_ERR_FIELD_NODATA);
	else if (pVbi_Fld->ulErrInfo & BVBI_LINE_ERROR_CC_NOENCODE)
		eErr = (BVBI_ERR_FIELD_BADDATA);

	/* Return data as requested */
	*pucLowByte  = pVbi_Fld->usCCData;
	*pucHighByte = (pVbi_Fld->usCCData >> 8);

	BDBG_LEAVE(BVBI_Field_GetCCData_isr);
	return eErr;
}

/***************************************************************************
 *
 */
BERR_Code BVBI_Field_SetCCData_isr( 
	BVBI_Field_Handle fieldHandle, 
	 uint8_t             cLowByte,	
	 uint8_t            cHighByte
)
{
	BVBI_P_Field_Handle* pVbi_Fld;

	BDBG_ENTER(BVBI_SetCCData_isr);

	/* check parameters */
	BVBI_P_GET_FIELD_CONTEXT(fieldHandle, pVbi_Fld);
	if(!pVbi_Fld)
	{
		BDBG_ERR(("Invalid parameter\n"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* Store data as requested */
    pVbi_Fld->usCCData = ((uint16_t)cHighByte << 8) | (uint16_t)cLowByte;

	/* Indicate valid data is present */
	pVbi_Fld->ulWhichPresent |= BVBI_P_SELECT_CC;

	BDBG_LEAVE(BVBI_SetCCData_isr);
	return BERR_SUCCESS;
}


/***************************************************************************
* Implementation supporting closed caption functions that are not in API
***************************************************************************/


BERR_Code BVBI_P_CC_Init( BVBI_P_Handle *pVbi )
{
	uint8_t hwIndex;
#if (BVBI_P_NUM_VDEC >= 1)
	uint32_t ulCoreOffset;
#endif

	BDBG_ENTER(BVBI_P_CC_Init);

	/* Initialize CC decoders */
#if (BVBI_P_NUM_VDEC >= 1)
	ulCoreOffset = 0x0;
	BVBI_P_CC_Dec_Init (pVbi->hReg, ulCoreOffset);
#endif
#if (BVBI_P_NUM_VDEC >= 2)
	ulCoreOffset = BCHP_CCD_1_RevID - BCHP_CCD_0_RevID;
	BVBI_P_CC_Dec_Init (pVbi->hReg, ulCoreOffset);
#endif

	/* Initialize CC encoders */
	for (hwIndex = 0 ; hwIndex < BVBI_P_NUM_CCE ; ++hwIndex)
		BVBI_P_CC_Enc_Init (pVbi->hReg, false, hwIndex);
#if (BVBI_P_NUM_CCE_656 > 0)
	for (hwIndex = 0 ; hwIndex < BVBI_P_NUM_CCE_656 ; ++hwIndex)
		BVBI_P_CC_Enc_Init (pVbi->hReg, true, hwIndex);
#endif

	BDBG_LEAVE(BVBI_P_CC_Init);
	return BERR_SUCCESS;
}

/* End of file */
