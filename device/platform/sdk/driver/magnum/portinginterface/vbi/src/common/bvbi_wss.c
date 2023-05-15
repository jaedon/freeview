/***************************************************************************
 *     Copyright (c) 2003-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvbi_wss.c $
 * $brcm_Revision: Hydra_Software_Devel/4 $
 * $brcm_Date: 6/24/09 4:38p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vbi/7400/bvbi_wss.c $
 * 
 * Hydra_Software_Devel/4   6/24/09 4:38p darnstein
 * PR56289: BVBI compiles for 7340 chipset now.
 * 
 * Hydra_Software_Devel/3   12/4/08 6:12p darnstein
 * PR45819: remove some obsolete "alias central" logic.
 * 
 * Hydra_Software_Devel/2   12/3/08 7:58p darnstein
 * PR45819: New, more modular form of most BVBI source files.
 * 
 * Hydra_Software_Devel/62   9/15/08 2:45p darnstein
 * PR44539: add support for the latest version of the WSE core hardware.
 * That is, the version present in 7601-A0.
 * 
 * Hydra_Software_Devel/61   6/20/08 4:48p darnstein
 * PR38956: adjust a THRESHOLD and a SYNC_LEVEL to get WSS decoding to
 * work on 3548.
 * 
 * Hydra_Software_Devel/60   5/12/08 9:11p darnstein
 * PR42416: adjust decoder video line number when the frame comb work-
 * around is in place.
 * 
 * Hydra_Software_Devel/59   4/23/08 1:48p darnstein
 * PR41309: fix start_delay for WSS encoder with 576P_50Hz output. Assumes
 * use of the special 54 MHz VEC IT microcode. Thanks to Bo Zhang for
 * test and measurement.
 * 
 * Hydra_Software_Devel/58   4/21/08 11:13a darnstein
 * PR41309: fix output video line number for 576P_50_Hz. Thanks to Bo
 * Zhang.
 * 
 * Hydra_Software_Devel/57   4/17/08 5:33p darnstein
 * PR41309: Need to make a check for field parity a bit looser, for 576P
 * use.
 * 
 * Hydra_Software_Devel/56   4/17/08 4:18p darnstein
 * PR41309: allow 576P_50Hz output. Requires special BVN configuration.
 * 
 * Hydra_Software_Devel/55   4/2/08 7:55p darnstein
 * PR38956: VBI software compiles now.
 * 
 * Hydra_Software_Devel/54   3/19/08 7:40p darnstein
 * PR40773: change start_delay for AMOE encoder to 0xE. Per Lin Ma.
 * 
 * Hydra_Software_Devel/53   3/19/08 4:18p darnstein
 * PR23201: Change HSYNC_DELAY to 0x40. This is required by change of
 * VDEC_FE_0_HSTART.VBI setting.
 * 
 * Hydra_Software_Devel/52   12/19/07 2:00p darnstein
 * PR32758: use WSS decoder register settings provided by Brad Grossman.
 * 
 * Hydra_Software_Devel/51   12/19/07 1:25p darnstein
 * PR34586: increase start delay for WSS SECAM output. This compensates
 * for new SECAM VEC microcode. The new microcode issues the start signal
 * earlier in the video line.
 * 
 * Hydra_Software_Devel/50   4/12/07 2:17p darnstein
 * PR29235: fix computation of WSS parity bit. Same logic as fix on
 * dedicated 7401 branch of ClearCase.
 * 
 * Hydra_Software_Devel/49   2/20/07 10:44a darnstein
 * PR27521: Apply software workaround. The TTE core is reset, at every
 * video field. Also, the encoder output for the field NOT in use is
 * disabled, at every video field.
 * 
 * Hydra_Software_Devel/48   1/2/07 4:20p darnstein
 * PR26872: Mechanically add SECAM to all cases where PAL formats are
 * accepted.
 * 
 * Hydra_Software_Devel/47   12/18/06 1:02p darnstein
 * PR24573: Adapt to an odd register name for BCM97440-B0 chipset.
 * 
 * Hydra_Software_Devel/46   12/15/06 4:17p darnstein
 * PR24573: Continuation of previous checkin (sorry).
 * 
 * Hydra_Software_Devel/45   12/15/06 3:47p darnstein
 * PR24573: Adapt to different names for same registers across different
 * chipsets.
 * 
 * Hydra_Software_Devel/44   12/14/06 7:18p darnstein
 * PR25990: Can compile for BCM97400-B0 now.
 * 
 * Hydra_Software_Devel/43   11/16/06 4:46p darnstein
 * PR25668: Mechanical changes to support the 97403 chipset.
 * 
 * Hydra_Software_Devel/42   9/26/06 7:37p darnstein
 * PR24573: The BVBI porting interface module now compiles for the 7440
 * chipset. Correct operation has not been verified.
 * 
 * Hydra_Software_Devel/41   9/1/06 4:48p darnstein
 * PR23242: Use encoder auto-parity when available. Some code commented
 * out because RDB header files are not ready yet.
 * 
 * Hydra_Software_Devel/40   8/31/06 2:09p darnstein
 * PR23869: clean up the handling of multiple VECs and VDECs.
 * 
 * Hydra_Software_Devel/39   8/18/06 6:52p darnstein
 * PR23178: basic compile on 93563 is possible.
 * 
 * Hydra_Software_Devel/38   7/20/06 6:23p darnstein
 * PR21688: name of reset bit varies by chip revision, phase of the moon,
 * etc.
 * 
 * Hydra_Software_Devel/37   7/20/06 2:23p darnstein
 * PR21688: Use the new hardware soft reset scheme for later model chips.
 * 
 * Hydra_Software_Devel/36   6/5/06 4:53p darnstein
 * PR18010: Previous checkin was in error.
 * 
 * Hydra_Software_Devel/35   6/5/06 4:41p darnstein
 * PR18010: silence a compiler warning.
 * 
 * Hydra_Software_Devel/34   5/3/06 2:24p darnstein
 * PR18010: Get rid of parity bit computation until it can be properly
 * tested. Remove extraneous call to BVBI_P_CC_ConfigForOthers() that was
 * switching to PAL mode.
 * 
 * Hydra_Software_Devel/33   5/3/06 2:14p darnstein
 * PR20427: Add parity bit for WSS encoding. Try tuning a pixel offset for
 * WSS decoding.
 * 
 * Hydra_Software_Devel/32   1/16/06 12:40p darnstein
 * PR18903: Use BSTD_UNUSED macro to suppress compiler warnings in
 * VxWorks.
 * 
 * Hydra_Software_Devel/31   12/5/05 7:30p darnstein
 * PR18010: Implement work-around for the bad line number, together with
 * an exception for customers that now depend on the bad line number.
 * 
 * Hydra_Software_Devel/30   10/6/05 11:33a darnstein
 * PR 16817: Use tuning recommended by Kin Fan Ho for WSS encoder. The
 * setting is for WSE_PRIM_control.start_delay, This change affects ALL
 * chips.
 * 
 * Hydra_Software_Devel/29   9/23/05 2:47p darnstein
 * PR13750: Proper use of BERR_TRACE and BERR_CODEs.
 * 
 * Hydra_Software_Devel/28   9/19/05 2:57p darnstein
 * PR17151: Check for chip name where needed. Also, convert to new scheme
 * for testing chip revisions (BCHP_VER).
 * 
 * Hydra_Software_Devel/27   8/22/05 8:12p darnstein
 * PR16057: To support many different chips, use private #defines that
 * specify number of VECs, VDECs, and (separately) pass-through VECs.
 * 
 * Hydra_Software_Devel/26   8/8/05 3:09p darnstein
 * PR13952: Adapt to slight changes to core registers in -C0 version of
 * BCM97038 chip.
 * 
 * Hydra_Software_Devel/25   4/14/05 12:01p darnstein
 * PR 14720: For each reference to BCHP_REV_B[01], add a BCHP_REV_B2.
 * Still untested.
 * 
 * Hydra_Software_Devel/24   3/17/05 7:42p darnstein
 * PR 14472: Eliminate references to secondary VEC if chip is 3560.
 * 
 * Hydra_Software_Devel/23   3/11/05 3:49p darnstein
 * PR 14426: use new _0 names for VDEC cores.
 * 
 * Hydra_Software_Devel/22   1/5/05 4:27p jasonh
 * PR 13700: Fixed VBI compile issues for 7038 C0.
 * 
 * Hydra_Software_Devel/21   8/30/04 11:22a darnstein
 * PR 12502: Fix a simple oversight.
 * 
 * Hydra_Software_Devel/I656/1   6/28/04 1:10p darnstein
 * 656 output is ready for testing.
 * 
 * Hydra_Software_Devel/19   6/17/04 5:56p darnstein
 * PR 9080: do a better job of handling the fact that WSS data only
 * appears on top fields. Also, kludge up the software so that WSS works
 * in NTSC.  This kills its use in PAL, so the kludges will have to be
 * backed out when PAL is supported.
 * 
 * Hydra_Software_Devel/18   3/4/04 4:25p darnstein
 * PR 9080: add some functionality needed for progressive video formats.
 * 
 * Hydra_Software_Devel/17   2/27/04 6:11p darnstein
 * PR 9080: handle all PAL formats.  Add checks for supported video
 * format.  Use Kin Fan Ho's register values.
 * 
 * Hydra_Software_Devel/16   2/6/04 11:49a darnstein
 * PR 9080: better handle VBI decoding errors.
 * 
 * Hydra_Software_Devel/15   2/3/04 4:42p darnstein
 * PR 9080: fix logic errors in handling decoding errors. A few other
 * minor fixes.
 * 
 * Hydra_Software_Devel/14   1/23/04 4:08p darnstein
 * PR 9080: Routines used in (pseudo) ISRs have no critical sections.
 * 
 * Hydra_Software_Devel/13   1/15/04 4:33p darnstein
 * PR 9080: fix problems with critical sections.
 * 
 * Hydra_Software_Devel/12   12/1/03 2:27p darnstein
 * Correct name of a function.
 * 
 * Hydra_Software_Devel/11   10/17/03 3:39p darnstein
 * VBI encoding can be disabled for one field time.
 * 
 * Hydra_Software_Devel/10   10/16/03 4:26p darnstein
 * Use revised #define BAVC_SourceId_Vdec0.
 * 
 * Hydra_Software_Devel/9   10/16/03 1:22p darnstein
 * Fix usage of BDBG_ENTER(), BDBG_LEAVE().
 * 
 * Hydra_Software_Devel/8   10/9/03 3:57p darnstein
 * Change name of several API functions to indicate that they may be
 * called from an ISR.
 * 
 * Hydra_Software_Devel/7   10/2/03 2:25p darnstein
 * Remove improper use of BERR_TRACE.
 * Put in some missing BDBG_LEAVE statements.
 * 
 * Hydra_Software_Devel/6   9/30/03 12:54p darnstein
 * Fix minor syntax problem.
 * 
 * Hydra_Software_Devel/5   9/29/03 5:01p darnstein
 * Put in critical sections.
 * 
 * Hydra_Software_Devel/4   9/25/03 4:49p darnstein
 * BVBI module is mostly complete. The only things that I know are missing
 * are the critical sections, 656 support, macrovision support, and LOTS
 * OF TESTING.
 * 
 * Hydra_Software_Devel/3   9/12/03 6:50p darnstein
 * Done except for VPS, teletext, critical sections, and (lots of)
 * testing.
 * 
 * Hydra_Software_Devel/2   9/9/03 7:52p darnstein
 * I think all API functions have been implemented, though some with
 * stubs.
 * 
 * Hydra_Software_Devel/1   8/29/03 10:57a darnstein
 * WSS specific software to support VBI processing.
 * 
 ***************************************************************************/

#include "bstd.h"           /* standard types */
#include "bdbg.h"           /* Dbglib */
#include "bvbi.h"           /* VBI processing, this module. */
#include "bkni.h"			/* For critical sections */
#include "bvbi_priv.h"      /* VBI internal data structures */

BDBG_MODULE(BVBI);

/***************************************************************************
* Implementation of "BVBI_" API functions
***************************************************************************/

/***************************************************************************
 *
 */
BERR_Code BVBI_Field_GetWSSData_isr(
	BVBI_Field_Handle vbiData, 
	uint16_t *pusWSSData
)
{
	BVBI_P_Field_Handle* pVbi_Fld;
	BERR_Code eErr = BERR_SUCCESS;

	BDBG_ENTER(BVBI_Field_GetWSSData_isr);

	/* check parameters */
	BVBI_P_GET_FIELD_CONTEXT(vbiData, pVbi_Fld);
	if((!pVbi_Fld) || (!pusWSSData))
	{
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* Verify that data is present on this field handle */
	if (!(pVbi_Fld->ulWhichPresent & BVBI_P_SELECT_WSS))
		return (BVBI_ERR_FIELD_NODATA);
	else if (pVbi_Fld->ulErrInfo & BVBI_LINE_ERROR_WSS_NOENCODE)
		eErr = (BVBI_ERR_FIELD_BADDATA);

	/* Return data as requested */
    *pusWSSData = pVbi_Fld->usWSSData;
	BDBG_LEAVE(BVBI_Field_GetWSSData_isr);
	return eErr;
}

/***************************************************************************
 *
 */
BERR_Code BVBI_Field_SetWSSData_isr( 
	BVBI_Field_Handle vbiData, uint16_t usWSSData )
{
	BVBI_P_Field_Handle* pVbi_Fld;

	BDBG_ENTER(BVBI_Field_SetWSSData_isr);

	/* check parameters */
	BVBI_P_GET_FIELD_CONTEXT(vbiData, pVbi_Fld);
	if(!pVbi_Fld)
	{
		BDBG_ERR(("Invalid parameter\n"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* Store data as requested */
    pVbi_Fld->usWSSData = usWSSData;

	/* Indicate valid data is present */
	pVbi_Fld->ulWhichPresent |= BVBI_P_SELECT_WSS;

	BDBG_LEAVE(BVBI_Field_SetWSSData_isr);
	return BERR_SUCCESS;
}


/***************************************************************************
* Implementation of supporting WSS functions that are not in API
***************************************************************************/


BERR_Code BVBI_P_WSS_Init( BVBI_P_Handle *pVbi )
{
	uint8_t hwIndex;
#if (BVBI_P_NUM_VDEC >= 1)
	uint32_t ulCoreOffset;
#endif

	BDBG_ENTER(BVBI_P_WSS_Init);

	/* Initialize WSS decoder */
#if (BVBI_P_NUM_VDEC >= 1)
	ulCoreOffset = 0x0;
	BVBI_P_WSS_Dec_Init (pVbi->hReg, ulCoreOffset);
#endif
#if (BVBI_P_NUM_VDEC >= 2)
	ulCoreOffset = BCHP_WSSD_1_Reset - BCHP_WSSD_0_Reset;
	BVBI_P_WSS_Dec_Init (pVbi->hReg, ulCoreOffset);
#endif

	/* Initialize WSS encoders */
	for (hwIndex = 0 ; hwIndex < BVBI_P_NUM_WSE ; ++hwIndex)
		BVBI_P_WSS_Enc_Init (pVbi->hReg, hwIndex);
#if (BVBI_P_NUM_WSE_656 > 0)
	for (hwIndex = 0 ; hwIndex < BVBI_P_NUM_WSE_656 ; ++hwIndex)
		BVBI_P_WSS_656_Enc_Init (pVbi->hReg, hwIndex);
#endif

	/* This line of code only serves to shut up a compiler warning: */
	(void) BVBI_P_AddWSSparity (0);

	BDBG_LEAVE(BVBI_P_WSS_Init);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
uint16_t BVBI_P_AddWSSparity (uint16_t usData)
{
    uint16_t usOriginalData = usData;
    uint8_t uchParity       = 0;
	static const uint16_t mask = 0x0008;

	/* The computation only depends on the 3 LSbits */
	uchParity += (usData & 0x1);
	usData >>= 1;
	uchParity += (usData & 0x1);
	usData >>= 1;
	uchParity += (usData & 0x1);

	/* Debug code 
	printf ("%04x (p%d) -> %04x\n", 
		usOriginalData, uchParity,
		(uchParity & 0x1) ? 
			(usOriginalData & ~mask) : (usOriginalData | mask));
	*/

	return 
		(uchParity & 0x1) ? 
			(usOriginalData & ~mask) : (usOriginalData | mask);
}


/***************************************************************************
* Static (private) functions
***************************************************************************/

/* End of file */
