/***************************************************************************
 *     Copyright (c) 2003-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvbi_cgms.c $
 * $brcm_Revision: Hydra_Software_Devel/3 $
 * $brcm_Date: 6/24/09 3:53p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vbi/7400/bvbi_cgms.c $
 * 
 * Hydra_Software_Devel/3   6/24/09 3:53p darnstein
 * PR56289: now compiles for chipsets that do not have ITU-R 656 output.
 * 
 * Hydra_Software_Devel/2   12/3/08 7:56p darnstein
 * PR45819: New, more modular form of most BVBI source files.
 * 
 * Hydra_Software_Devel/84   9/12/08 1:24p darnstein
 * PR46112: Implement variant of CGMS-B (output) defined in CEA-805-D.
 * 
 * Hydra_Software_Devel/82   7/18/08 2:08p darnstein
 * PR44039: Adjusted line number for 480P input. I also adjusted the
 * horizontal offset. Problem: this might not be optimal for all
 * chipsets.
 * 
 * Hydra_Software_Devel/81   6/26/08 7:51p darnstein
 * PR40710: update for 50 Hz HDTV video formats.
 * 
 * Hydra_Software_Devel/80   6/26/08 4:15p darnstein
 * PR40709,PR40710: Adjust register settings for CGMS-B output with 720P
 * video and 1080I video.
 * 
 * Hydra_Software_Devel/79   6/24/08 7:32p darnstein
 * PR40709: Choose a value for six bit CGMS-B header.
 * 
 * Hydra_Software_Devel/78   6/24/08 6:22p darnstein
 * PR40709: Tune up some register settings for CGMS-B output. More work to
 * follow.
 * 
 * Hydra_Software_Devel/77   6/24/08 2:17p darnstein
 * PR38956: Got CGMS decoding to work for 1080i video.
 * 
 * Hydra_Software_Devel/76   6/24/08 1:56p darnstein
 * PR38956: Got CGMS decoding to work for 720P video.
 * 
 * Hydra_Software_Devel/75   6/23/08 7:58p darnstein
 * PR38956: Got CGMS decoding to work for 480P video.
 * 
 * Hydra_Software_Devel/74   6/13/08 12:11p darnstein
 * PR43463: Ror CGMSB, the BCHP_CGMSAE_PRIM_Reg_updt register must be
 * programmed.
 * 
 * Hydra_Software_Devel/73   6/6/08 5:34p darnstein
 * PR38956: fix some typing errors in register names (CGMS-B output).
 * 
 * Hydra_Software_Devel/72   4/28/08 7:49p darnstein
 * PR38956: CGMS-B encoding ready for bring-up. Need accurate register
 * settings for tuning.
 * 
 * Hydra_Software_Devel/71   4/14/08 8:09p darnstein
 * PR41309: Fix PULSE_WIDTH for IEC 62375 (576P_50Hz) output.
 * 
 * Hydra_Software_Devel/70   4/4/08 3:07p darnstein
 * PR41359: port PR36108 to main branch. Use CGMSAE core to produce
 * WSS/576P waveform.
 * 
 * Hydra_Software_Devel/69   4/2/08 7:54p darnstein
 * PR38956: VBI software compiles now.
 * 
 * Hydra_Software_Devel/68   3/19/08 5:38p darnstein
 * PR40768: Change (CGMSAE)_FORMAT.INIT_DELAY to 0x20. Per Lin Ma.
 * 
 * Hydra_Software_Devel/66   1/2/08 2:26p darnstein
 * PR32758: use PAL decoder settings from Brad Grossman.
 * 
 * Hydra_Software_Devel/65   11/27/07 3:00p darnstein
 * PR37493: first guess at CGMS for 1080i_50Hz and 720p_50Hz.
 * 
 * Hydra_Software_Devel/64   2/20/07 10:43a darnstein
 * PR27521: Apply software workaround. The TTE core is reset, at every
 * video field. Also, the encoder output for the field NOT in use is
 * disabled, at every video field.
 * 
 * Hydra_Software_Devel/63   12/14/06 7:18p darnstein
 * PR25990: Can compile for BCM97400-B0 now.
 * 
 * Hydra_Software_Devel/62   11/21/06 2:53p darnstein
 * PR26103: I applied small changes to register settings specified by Dan
 * Martinez.
 * 
 * Hydra_Software_Devel/61   11/16/06 4:41p darnstein
 * PR20428: Change the way interrupts for progressive video input is
 * handled. This might have to be revisited.
 * 
 * Hydra_Software_Devel/60   10/19/06 5:47p darnstein
 * PR24979: change horizontal offset of waveform when sending NTSC.
 * Theory: VEC microcode changed in such a way that the register values
 * are interpreted differently.
 * 
 * Hydra_Software_Devel/59   8/31/06 2:09p darnstein
 * PR23869: clean up the handling of multiple VECs and VDECs.
 * 
 * Hydra_Software_Devel/58   8/18/06 6:50p darnstein
 * PR23178: basic compile on 93563 is possible.
 * 
 * Hydra_Software_Devel/57   7/20/06 2:23p darnstein
 * PR21688: Use the new hardware soft reset scheme for later model chips.
 * 
 * Hydra_Software_Devel/56   5/26/06 1:20p darnstein
 * PR21710: A field handle can be dequeued for encoding at any one of
 * three times: top field, bottom field, or FRAME. FRAME is the new
 * choice. The field handles themselves continue to be defined as top
 * field, bottom field, or both. The new logic is when a field handle is
 * dequeued at time FRAME, it is always encoded immediately. The rest of
 * the logic is the same: a "top" field handle is only dequeued and
 * encoded at "top field" time, same for bottoms. When setting up field
 * interrupt callbacks, for progressive video output, the argument
 * BAVC_Polarity_eFrame should be used now.
 * 
 * Hydra_Software_Devel/55   5/3/06 2:30p darnstein
 * PR18010: Remove extraneous call to BVBI_P_CC_ConfigForOthers() that
 * might change the video format of closed caption encoder core.
 * 
 * Hydra_Software_Devel/54   4/12/06 6:17p darnstein
 * PR18010: Don't try to program encoders for don't exist for 656 output.
 * 
 * Hydra_Software_Devel/53   1/16/06 12:40p darnstein
 * PR18903: Use BSTD_UNUSED macro to suppress compiler warnings in
 * VxWorks.
 * 
 * Hydra_Software_Devel/52   12/22/05 6:08p darnstein
 * PR11725: support HDTV formats in decoding.
 * 
 * Hydra_Software_Devel/51   9/23/05 2:46p darnstein
 * PR13750: Proper use of BERR_TRACE and BERR_CODEs.
 * 
 * Hydra_Software_Devel/50   9/19/05 2:56p darnstein
 * PR17151: Check for chip name where needed. Also, convert to new scheme
 * for testing chip revisions (BCHP_VER).
 * 
 * Hydra_Software_Devel/49   8/23/05 11:53a darnstein
 * PR16057: Fix error in last checkin. Affects compilation for BCM93560.
 * 
 * Hydra_Software_Devel/48   8/22/05 8:11p darnstein
 * PR16057: To support many different chips, use private #defines that
 * specify number of VECs, VDECs, and (separately) pass-through VECs.
 * 
 * Hydra_Software_Devel/47   6/20/05 12:38p darnstein
 * PR 15923: I missed something in last checkin.
 * 
 * Hydra_Software_Devel/46   6/20/05 12:04p darnstein
 * PR 15923: Adjust some #define's to support 93560 chip.
 * 
 * Hydra_Software_Devel/45   4/14/05 12:00p darnstein
 * PR 14720: For each reference to BCHP_REV_B[01], add a BCHP_REV_B2.
 * Still untested.
 * 
 * Hydra_Software_Devel/44   3/31/05 11:55a darnstein
 * PR 13952: Apply different register settings for 7038-C0.
 * 
 * Hydra_Software_Devel/44   3/31/05 11:53a darnstein
 * PR 13952: Apply different register settings for 7038-C0.
 * 
 * Hydra_Software_Devel/43   3/21/05 4:48p vsilyaev
 * PR 14472: Fixed compile problem on 3560.
 * 
 * Hydra_Software_Devel/42   3/17/05 6:14p darnstein
 * PR 14472: Eliminate references to secondary VEC if chip is 3560.
 * 
 * Hydra_Software_Devel/41   3/17/05 6:10p darnstein
 * PR 14487: work around hardware bug with line count.
 * 
 * Hydra_Software_Devel/40   3/11/05 3:48p darnstein
 * PR 14426: use new _0 names for VDEC cores.
 * 
 * Hydra_Software_Devel/39   1/5/05 4:26p jasonh
 * PR 13700: Fixed VBI compile issues for 7038 C0.
 * 
 * Hydra_Software_Devel/38   11/11/04 11:53a darnstein
 * PR 13104: Fix a bad comment.
 * 
 * Hydra_Software_Devel/37   10/28/04 4:30p darnstein
 * PR 13104: The horizontal offset is now 11.u us, just what a customer
 * wanted.  Customer wanted a rise time of 0.14 us.  The rise time is now
 * 0.12 us, and this is the best that I can do.
 * 
 * Hydra_Software_Devel/36   10/27/04 5:25p darnstein
 * PR 13050: The microcode for setting line number in 480p analog output
 * has been fixed. So, don't kludge line number any more.
 * 
 * Hydra_Software_Devel/35   10/12/04 4:53p darnstein
 * PR 12855: Adjust VBI line for 480p encoding.  Clean up some sloppy
 * coding too.
 * 
 * Hydra_Software_Devel/34   10/1/04 4:48p darnstein
 * PR 9080: For decoder, subtract 1 from video line number.  Work-around
 * for quirk in line counter logic elsewhere on chip.
 * 
 * Hydra_Software_Devel/33   6/3/04 7:45p darnstein
 * PR 11308: CGMS encoder now puts its data on line 41 for 480p video.
 * 
 * Hydra_Software_Devel/32   5/5/04 4:31p darnstein
 * PR 9080: for encoding 1080i CGMS, change init_delay to 0xB5, per Kin
 * FanHo.
 * 
 * Hydra_Software_Devel/31   5/5/04 3:48p darnstein
 * PR 9080: for encoding 1080i CGMS, change init_delay to 0xB5, per Kin
 * Fan Ho.
 * 
 * Hydra_Software_Devel/30   5/3/04 12:54p darnstein
 * PR 9080: change rise time for encoder to 1 for 480p video output (re.
 * Kin Fan Ho).
 * 
 * Hydra_Software_Devel/29   4/29/04 5:11p darnstein
 * PR 9080: Tune init_delay and pulse_width for 480p and 720p video
 * output.
 * 
 * Hydra_Software_Devel/28   4/28/04 8:57p darnstein
 * PR 9080: begin to bring up for HD standards. More tuning is required.
 * 
 * Hydra_Software_Devel/27   4/2/04 6:42p darnstein
 * PR 9080: Allow NTSC-J video format.
 * 
 * Hydra_Software_Devel/26   3/18/04 11:11a darnstein
 * PR 9080: use a "crude" method for clearing status bits upon decode.
 * Necessary because of a hardware fault in A0 and B0 chips.
 * 
 * Hydra_Software_Devel/25   3/4/04 8:07p darnstein
 * PR 9080: adjust GAIN and INIT_DELAY values for 720P and 1080i video
 * formats.  Worked with Kin Fan to get these corrected values.
 * 
 * Hydra_Software_Devel/24   3/4/04 4:26p darnstein
 * PR 9080: add support for progressive and HDTV video formats.
 * 
 * Hydra_Software_Devel/23   2/27/04 6:11p darnstein
 * PR 9080: fix misuse of critical sections.
 * 
 * Hydra_Software_Devel/22   2/26/04 3:13p darnstein
 * PR 9906: When doing CRC computation in software, cache last computation
 * for reuse, to save CPU cycles.
 * 
 * Hydra_Software_Devel/21   2/26/04 2:05p darnstein
 * PR 9906: Compute CRC in software for CGMS encoder.
 * 
 * Hydra_Software_Devel/20   2/20/04 4:36p darnstein
 * PR 9090: almost working.
 * 
 * Hydra_Software_Devel/19   2/19/04 6:25p darnstein
 * PR 9080: When responding to decoder interrupt, be more careful about
 * clearing interrupt condition bits in control register.
 * 
 * Hydra_Software_Devel/18   2/6/04 11:49a darnstein
 * PR 9080: better handle VBI decoding errors.
 * 
 * Hydra_Software_Devel/17   2/3/04 4:42p darnstein
 * PR 9080: fix logic errors in handling decoding errors. A few other
 * minor fixes.
 * 
 * Hydra_Software_Devel/16   1/26/04 1:17p darnstein
 * PR 8730: Comment out some debugging code that was causing compiler
 * warnings.
 * 
 * Hydra_Software_Devel/15   1/23/04 4:08p darnstein
 * PR 9080: Routines used in (pseudo) ISRs have no critical sections.
 * 
 * Hydra_Software_Devel/14   1/19/04 12:00p darnstein
 * PR 9080: Tune some register values. Put in debugging code (temporary).
 * 
 * Hydra_Software_Devel/13   1/15/04 4:28p darnstein
 * PR 9080: fix problems with critical sections.
 * 
 * Hydra_Software_Devel/12   12/19/03 5:07p darnstein
 * PR 9080: adapt to changed BAVC enum for field polarity.
 * 
 * Hydra_Software_Devel/11   12/1/03 2:27p darnstein
 * Correct name of a function.
 * 
 * Hydra_Software_Devel/10   10/17/03 3:38p darnstein
 * VBI encoding can be disabled for one field time.
 * 
 * Hydra_Software_Devel/9   10/16/03 4:26p darnstein
 * Use revised #define BAVC_SourceId_Vdec0.
 * 
 * Hydra_Software_Devel/8   10/16/03 1:21p darnstein
 * Fix usage of BDBG_ENTER(), BDBG_LEAVE().
 * 
 * Hydra_Software_Devel/7   10/9/03 3:57p darnstein
 * Change name of several API functions to indicate that they may be
 * called from an ISR.
 * 
 * Hydra_Software_Devel/6   10/2/03 2:24p darnstein
 * Remove improper use of BERR_TRACE.
 * Put in some missing BDBG_LEAVE statements.
 * 
 * Hydra_Software_Devel/5   9/29/03 5:00p darnstein
 * Put in critical sections.
 * 
 * Hydra_Software_Devel/4   9/25/03 4:48p darnstein
 * BVBI module is mostly complete. The only things that I know are missing
 * are the critical sections, 656 support, macrovision support, and LOTS
 * OF TESTING.
 * 
 * Hydra_Software_Devel/3   9/12/03 6:50p darnstein
 * Done except for VPS, teletext, critical sections, and (lots of)
 * testing.
 * 
 * Hydra_Software_Devel/2   9/9/03 7:53p darnstein
 * I think all API functions have been implemented, though some with
 * stubs.
 * 
 * Hydra_Software_Devel/1   8/29/03 10:56a darnstein
 * CGMS specific software to support VBI processing.
 * 
 ***************************************************************************/

#include "bstd.h"             /* standard types */
#include "bdbg.h"             /* Dbglib */
#include "bkni.h"			  /* For critical sections */
#include "bvbi.h"             /* VBI processing, this module. */
#include "bvbi_priv.h"        /* VBI internal data structures */

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
BERR_Code 
BVBI_Field_GetCGMSAData_isr( BVBI_Field_Handle vbiData, uint32_t *pulCGMSData )
{
	BVBI_P_Field_Handle* pVbi_Fld;
	BERR_Code eErr = BERR_SUCCESS;

	BDBG_ENTER(BVBI_Field_GetCGMSAData_isr);

	/* check parameters */
	BVBI_P_GET_FIELD_CONTEXT(vbiData, pVbi_Fld);
	if((!pVbi_Fld) || (!pulCGMSData))
	{
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* Verify that data is present on this field handle */
	if (!(pVbi_Fld->ulWhichPresent & BVBI_P_SELECT_CGMSA))
		return (BVBI_ERR_FIELD_NODATA);
	else if (pVbi_Fld->ulErrInfo & BVBI_LINE_ERROR_CGMS_NOENCODE)
		eErr = (BVBI_ERR_FIELD_BADDATA);

	/* Return data as requested */
    *pulCGMSData = pVbi_Fld->ulCGMSData;
	BDBG_LEAVE(BVBI_Field_GetCGMSAData_isr);
	return eErr;
}

/***************************************************************************
 *
 */
BERR_Code BVBI_Field_SetCGMSAData_isr( 
	BVBI_Field_Handle vbiData, uint32_t ulCGMSData )
{
	BVBI_P_Field_Handle* pVbi_Fld;

	BDBG_ENTER(BVBI_Field_SetCGMSAData_isr);

	/* check parameters */
	BVBI_P_GET_FIELD_CONTEXT(vbiData, pVbi_Fld);
	if(!pVbi_Fld)
	{
		BDBG_ERR(("Invalid parameter\n"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* Store data as requested */
    pVbi_Fld->ulCGMSData = ulCGMSData;

	/* Indicate valid data is present */
	pVbi_Fld->ulWhichPresent |= BVBI_P_SELECT_CGMSA;

	BDBG_LEAVE(BVBI_Field_SetCGMSAData_isr);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code 
BVBI_Field_GetCGMSBData_isr( 
	BVBI_Field_Handle vbiData,  BVBI_CGMSB_Datum* pDatum)
{
	int subIndex;
	BVBI_P_Field_Handle* pVbi_Fld;
	BERR_Code eErr = BERR_SUCCESS;

	BDBG_ENTER(BVBI_Field_GetCGMSBData_isr);

	/* check parameters */
	BVBI_P_GET_FIELD_CONTEXT(vbiData, pVbi_Fld);
	if((!pVbi_Fld) || (!pDatum))
	{
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* Verify that data is present on this field handle */
	if (!(pVbi_Fld->ulWhichPresent & BVBI_P_SELECT_CGMSB))
		return (BVBI_ERR_FIELD_NODATA);
	else if (pVbi_Fld->ulErrInfo & BVBI_LINE_ERROR_CGMS_NOENCODE)
		eErr = (BVBI_ERR_FIELD_BADDATA);

	/* Check that field handle was properly sized */
	if (!pVbi_Fld->pCgmsbDatum)
	{
		return BERR_TRACE (BVBI_ERR_FLDH_CONFLICT);
	}

	/* Return data as requested */
	for (subIndex = 0 ; subIndex < 5 ; ++subIndex)
		(*pDatum)[subIndex] = (*pVbi_Fld->pCgmsbDatum)[subIndex];
	BDBG_LEAVE(BVBI_Field_GetCGMSBData_isr);
	return eErr;
}

/***************************************************************************
 *
 */
BERR_Code BVBI_Field_SetCGMSBData_isr( 
	BVBI_Field_Handle vbiData, BVBI_CGMSB_Datum* pDatum)
{
	BVBI_P_Field_Handle* pVbi_Fld;
	int subIndex;

	BDBG_ENTER(BVBI_Field_SetCGMSBData_isr);

	/* check parameters */
	BVBI_P_GET_FIELD_CONTEXT(vbiData, pVbi_Fld);
	if(!pVbi_Fld)
	{
		BDBG_ERR(("Invalid parameter\n"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* Check that field handle was properly sized */
	if (!pVbi_Fld->pCgmsbDatum)
	{
		return BERR_TRACE (BVBI_ERR_FLDH_CONFLICT);
	}

	/* Store data as requested */
	for (subIndex = 0 ; subIndex < 5 ; ++subIndex)
		(*pVbi_Fld->pCgmsbDatum)[subIndex] = (*pDatum)[subIndex];

	/* Indicate valid data is present */
	pVbi_Fld->ulWhichPresent |= BVBI_P_SELECT_CGMSB;

	BDBG_LEAVE(BVBI_Field_SetCGMSBData_isr);
	return BERR_SUCCESS;
}


/***************************************************************************
* Implementation of supporting CGMS functions that are not in API
***************************************************************************/


BERR_Code BVBI_P_CGMS_Init( BVBI_P_Handle *pVbi )
{
#if (BVBI_P_NUM_VDEC >= 1) 
	uint32_t ulCoreOffset;
#endif
	uint8_t hwIndex;

#if (BVBI_P_NUM_VDEC >= 1) 
	ulCoreOffset = 0x0;
	BVBI_P_CGMS_Dec_Init (pVbi->hReg, ulCoreOffset);
#endif 

	BDBG_ENTER(BVBI_P_CGMS_Init);

#if (BVBI_P_NUM_VDEC >= 2) 
	ulCoreOffset = BCHP_CGMSD_1_RevID - BCHP_CGMSD_0_RevID;
	BVBI_P_CGMS_Dec_Init (pVbi->hReg, ulCoreOffset);
#endif 

	/* Initialize CGMS encoders */
	for (hwIndex = 0 ; hwIndex < BVBI_P_NUM_CGMSAE ; ++hwIndex)
		BVBI_P_CGMS_Enc_Init (pVbi->hReg, false, hwIndex);
#if (BVBI_P_NUM_CGMSAE_656 > 0)
	for (hwIndex = 0 ; hwIndex < BVBI_P_NUM_CGMSAE_656 ; ++hwIndex)
		BVBI_P_CGMS_Enc_Init (pVbi->hReg, true, hwIndex);
#endif

	BDBG_LEAVE(BVBI_P_CGMS_Init);
	return BERR_SUCCESS;
}

uint32_t BVPI_P_CGMS_format_data (uint32_t userdata)
{
	/* Do CRC calculation if hardware is broken */
	/* Otherwise, just transform from decoder format to encoder format. */
#ifdef P_CGMS_SOFTWARE_CRC
	userdata &= 0x00003fff;
	userdata = P_CalculateCRC (userdata);
	userdata <<= 2;
	userdata &= 0x003ffffc;
	userdata |= 0x00000001;
#else
	userdata <<= 2;
	userdata &= 0x0000fffc;
#endif

	return userdata;
}


/***************************************************************************
* Static (private) functions
***************************************************************************/


/* End of file */
