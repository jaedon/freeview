/***************************************************************************
 *     Copyright (c) 2003-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvbi.c $
 * $brcm_Revision: Hydra_Software_Devel/5 $
 * $brcm_Date: 2/17/11 6:31p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vbi/7400/bvbi.c $
 * 
 * Hydra_Software_Devel/5   2/17/11 6:31p jtna
 * SW7420-1456: update BCHP_PWR pm code
 * 
 * Hydra_Software_Devel/4   2/14/11 5:02p darnstein
 * SW7420-1456: Merge BVBI work to main branch: power management.
 * 
 * Hydra_Software_Devel/SW7420-1456/2   2/3/11 6:16p jtna
 * SW7420-1456: add VDC_656 output to power management
 * 
 * Hydra_Software_Devel/SW7420-1456/1   2/2/11 8:07p jtna
 * SW7420-1456: initial work on VDC/VBI power management
 * 
 * Hydra_Software_Devel/3   6/30/09 5:51p darnstein
 * PR56342: support for 7550 chipset.
 * 
 * Hydra_Software_Devel/2   12/3/08 7:55p darnstein
 * PR45819: New, more modular form of most BVBI source files.
 * 
 * Hydra_Software_Devel/103   9/15/08 8:57p darnstein
 * PR46356: VPS decoder software in BVBI is ready for test.
 * 
 * Hydra_Software_Devel/102   9/12/08 1:24p darnstein
 * PR46112: Implement variant of CGMS-B (output) defined in CEA-805-D.
 * 
 * Hydra_Software_Devel/101   9/8/08 1:52p darnstein
 * PR40709: support for CGMS-B.
 * 
 * Hydra_Software_Devel/100   9/4/08 7:16a vobadm
 * PR 44539:
 * 
 * Hydra_Software_Devel/bdvd_v2.0/1   8/29/08 8:01a fbasso
 * PR 10340: 7601 build
 *
 * Hydra_Software_Devel/98   7/30/08 8:39p darnstein
 * PR43926,PR43413: Workaround for TTD addressing error in hardware.
 * Decodes at most one line of teletext per video field. Will not
 * corrrupt memory if input signal actually contains more teletext.
 *
 * Hydra_Software_Devel/97   6/30/08 4:56p darnstein
 * PR41267: Silence a Coverity complaint with a big hammer.
 *
 * Hydra_Software_Devel/96   6/26/08 6:04p darnstein
 * PR39493: Remove extraneous line of code.
 *
 * Hydra_Software_Devel/95   5/28/08 5:41p darnstein
 * PR43083: deallocate SCTE data when destroying a field handle.
 *
 * Hydra_Software_Devel/94   5/28/08 5:35p darnstein
 * PR23220: Some implementation of Gemstar decoding was missing.
 *
 * Hydra_Software_Devel/93   5/19/08 2:35p darnstein
 * PR42717,PR42733,PR42734: Fix Coverity warnings.
 *
 * Hydra_Software_Devel/92   5/13/08 2:08p darnstein
 * PR34584: Port over changes from 7401 dedicated branch. Allow user to
 * choose bit shift direction for teletext encoding.
 *
 * Hydra_Software_Devel/91   4/30/08 3:41p darnstein
 * PR38956: fix problem: CGMSB not available on 7405 (and many other
 * chipsets).
 *
 * Hydra_Software_Devel/90   4/28/08 7:49p darnstein
 * PR38956: CGMS-B encoding ready for bring-up. Need accurate register
 * settings for tuning.
 *
 * Hydra_Software_Devel/89   4/14/08 8:10p darnstein
 * PR41309: Put in a 6-bit preamble in BVBI_WSS2CGMS(). I don't think that
 * is enough though.
 *
 * Hydra_Software_Devel/88   4/9/08 8:02p darnstein
 * PR37064: In previous checkin, I made a mistake in porting over from
 * dedicated 7401 branch.
 *
 * Hydra_Software_Devel/87   4/8/08 6:13p darnstein
 * PR37064: This is the same 15-lines-of-teletext problem that was solved
 * (worked-around, actually) on the dedicated 7401 ClearCase branch.
 *
 * Hydra_Software_Devel/86   4/4/08 3:07p darnstein
 * PR41359: port PR36108 to main branch. Use CGMSAE core to produce
 * WSS/576P waveform.
 *
 * Hydra_Software_Devel/85   3/14/08 3:18p darnstein
 * PR23220: Gemstar decoding almost works. Cannot process video line 11
 * for some reason.
 *
 * Hydra_Software_Devel/84   3/11/08 6:49p darnstein
 * PR23220: Fill in some missing Gemstar decoder logic.
 *
 * Hydra_Software_Devel/83   11/1/07 12:51p darnstein
 * PR30411: Fix some logic errors involving multi-line closed caption vs
 * classic closed caption.
 *
 * Hydra_Software_Devel/82   10/15/07 11:33a pntruong
 * PR 35037, PR 35038, PR 35039: Fixed linux build errors.
 *
 * Hydra_Software_Devel/81   10/12/07 11:15a syang
 * PR 35037, PR 35038, PR 35039:  init eStatus
 *
 * Hydra_Software_Devel/80   10/12/07 10:58a syang
 * PR 35037, PR 35038, PR 35039: clean up
 *
 * Hydra_Software_Devel/79   10/11/07 6:15p syang
 * PR 35037, PR 35038 , PR 35039: coverity (resouce_leak) fix
 *
 * Hydra_Software_Devel/78   9/11/07 6:03p darnstein
 * PR25708: Fix ClearCase merge errors. More to come.
 *
 * Hydra_Software_Devel/77   9/11/07 5:12p darnstein
 * PR25708: First release of SCTE encoder software.
 *
 * Hydra_Software_Devel/76   6/8/07 6:30p darnstein
 * PR31804: Deallocate bulky memory in BVBI_Field_Destroy().
 *
 * Hydra_Software_Devel/75   6/6/07 12:47p darnstein
 * PR30411: multi-line closed caption encoding is ready for test.
 *
 * Hydra_Software_Devel/74   2/27/07 11:52a darnstein
 * PR25990: Fix up programming of ANCI656 core, for 7400-B0 chipset.
 *
 * Hydra_Software_Devel/73   2/14/07 6:12p darnstein
 * PR27721: Release resources from BVBI_Decode_Create() when it fails
 * halfway through.
 *
 * Hydra_Software_Devel/72   1/17/07 5:31p darnstein
 * PR26464: correctly handle teletext output to multiple VECs
 *
 * Hydra_Software_Devel/71   12/14/06 7:16p darnstein
 * PR25990: Can compile for BCM97400-B0 now.
 *
 * Hydra_Software_Devel/70   11/13/06 3:07p darnstein
 * PR23247: Serial output of teletext is ready for testing.
 *
 * Hydra_Software_Devel/69   11/8/06 5:13p darnstein
 * PR23247: Serial output of teletext is partially supported now.
 *
 * Hydra_Software_Devel/68   9/26/06 7:37p darnstein
 * PR24573: The BVBI porting interface module now compiles for the 7440
 * chipset. Correct operation has not been verified.
 *
 * Hydra_Software_Devel/67   9/13/06 6:33p darnstein
 * PR21688: Can now compile for 97118-A0 chipset. Correct operation is not
 * guaranteed.
 *
 * Hydra_Software_Devel/66   8/18/06 6:50p darnstein
 * PR23178: basic compile on 93563 is possible.
 *
 * Hydra_Software_Devel/65   5/26/06 1:20p darnstein
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
 * Hydra_Software_Devel/70   11/13/06 3:07p darnstein
 * PR23247: Serial output of teletext is ready for testing.
 *
 * Hydra_Software_Devel/69   11/8/06 5:13p darnstein
 * PR23247: Serial output of teletext is partially supported now.
 *
 * Hydra_Software_Devel/68   9/26/06 7:37p darnstein
 * PR24573: The BVBI porting interface module now compiles for the 7440
 * chipset. Correct operation has not been verified.
 *
 * Hydra_Software_Devel/67   9/13/06 6:33p darnstein
 * PR21688: Can now compile for 97118-A0 chipset. Correct operation is not
 * guaranteed.
 *
 * Hydra_Software_Devel/66   8/18/06 6:50p darnstein
 * PR23178: basic compile on 93563 is possible.
 *
 * Hydra_Software_Devel/65   5/26/06 1:20p darnstein
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
 * Hydra_Software_Devel/64   4/12/06 1:56p darnstein
 * PR18010: Add ANCI656_PRIM_ANCI656_CNTRL setting to the PR18010
 * backwards compatibility option.
 *
 * Hydra_Software_Devel/63   4/11/06 7:26p darnstein
 * PR20429: first cut at AMOL encoder software.
 *
 * Hydra_Software_Devel/62   3/20/06 1:08p darnstein
 * PR19719: After changing BCHP_VBI_ENC_PRIM_Control (indirectly), poll
 * register until it has really changed. This enforces the one transition
 * that is critical to this PR: when disabling a VBI encoder core, the
 * appropriate bit in BCHP_VBI_ENC_PRIM_Control must be zeroed before the
 * individual VBI core is disabled.
 *
 * Hydra_Software_Devel/61   3/3/06 12:49p darnstein
 * PR18331: When BVBI_Decode_ApplyChanges() fails, roll back hardware
 * state completely. The state was random before this fix.
 *
 * Hydra_Software_Devel/60   3/2/06 4:58p darnstein
 * PR18331: When BVBI_Encode_ApplyChanges() fails, roll back hardware
 * state completely. The state was random before this fix.
 *
 * Hydra_Software_Devel/59   2/3/06 1:28p darnstein
 * PR19428: Add #ifdef's to support the 7400 chip.
 *
 * Hydra_Software_Devel/58   1/18/06 2:19p darnstein
 * PR19133: Fix two problems that were preventing VPS output.
 *
 * Hydra_Software_Devel/57   12/21/05 5:14p darnstein
 * PR18341: Swap bytes of the teletext "line mask" DRAM entry on little-
 * endian systems.
 *
 * Hydra_Software_Devel/56   12/5/05 7:29p darnstein
 * PR18010: Implement work-around for the bad line number, together with
 * an exception for customers that now depend on the bad line number.
 *
 * Hydra_Software_Devel/55   11/14/05 7:15p darnstein
 * PR17732: Change the way the bottom field base video line number is
 * expressed.
 *
 * Hydra_Software_Devel/54   10/21/05 7:10p darnstein
 * PR17732: Gemstar encoder can output a waveform, but I can't make it
 * change yet.
 *
 * Hydra_Software_Devel/53   10/20/05 4:16p darnstein
 * PR17732: Fix a silly coding error.
 *
 * Hydra_Software_Devel/52   10/20/05 1:56p darnstein
 * PR17732: Gemstar features not ready yet. However, the build for 97038
 * is not broken.
 *
 * Hydra_Software_Devel/51   9/23/05 2:46p darnstein
 * PR13750: Proper use of BERR_TRACE and BERR_CODEs.
 *
 * Hydra_Software_Devel/50   9/19/05 2:56p darnstein
 * PR17151: Check for chip name where needed. Also, convert to new scheme
 * for testing chip revisions (BCHP_VER).
 *
 * Hydra_Software_Devel/49   8/22/05 8:10p darnstein
 * PR16057: To support many different chips, use private #defines that
 * specify number of VECs, VDECs, and (separately) pass-through VECs.
 *
 * Hydra_Software_Devel/48   7/7/05 3:34p darnstein
 * PR 16008: The default settings struct for BVBI_Open() now allows the
 * user to choose a buffer size for capturing ancillary data packets in
 * incoming ITU-R 656 digital video.
 *
 * Hydra_Software_Devel/47   7/6/05 5:54p darnstein
 * PR 16008: Input of closed caption data in SAA7113 ancillary data
 * packets of ITU-R 656 digital video has been confirmed. SAA7114 input
 * almost certainly needs some debugging though.
 *
 * Hydra_Software_Devel/46   6/15/05 5:05p darnstein
 * PR 14487: Put in runtime checking for workaround for hardware PR 13583.
 *
 * Hydra_Software_Devel/45   5/18/05 5:47p darnstein
 * PR 11440: Progress towards ITU-R 656 input of VBI data.
 *
 * Hydra_Software_Devel/44   4/14/05 12:00p darnstein
 * PR 14720: For each reference to BCHP_REV_B[01], add a BCHP_REV_B2.
 * Still untested.
 *
 * Hydra_Software_Devel/43   3/18/05 5:12p darnstein
 * PR 14518: allow CGMS and WSS output to be persistent.
 *
 * Hydra_Software_Devel/42   3/18/05 4:12p darnstein
 * PR 14472: BVBI_Encode_Create() errors out right away if caller attempts
 * to open on secondary VEC, on a BCM3560 chip.
 *
 * Hydra_Software_Devel/41   3/9/05 3:44p darnstein
 * PR 11440: fix errors involving ITU-R 656 input and output.
 *
 * Hydra_Software_Devel/40   2/15/05 1:20p darnstein
 * PR 14118: Use function argument as required (twice). Bug discovered at
 * the source by Perrier.
 *
 * Hydra_Software_Devel/39   12/13/04 7:44p darnstein
 * PR 13516: Do not turn closed caption encoder off just because there is
 * no data immediately ready for output.
 *
 * Hydra_Software_Devel/38   7/22/04 12:44p darnstein
 * PR 9080: change names of BVBI GetDisplayFormat functions to
 * GetVideoFormat.  This change should have been made in the revision
 * Hydra_Software_Devel/32 on 4/2/04.
 *
 * Hydra_Software_Devel/37   7/16/04 7:06p darnstein
 * PR 9080: merge in 656 input and output work. Some testing and debugging
 * remains to be done.
 *
 * Hydra_Software_Devel/I656/5   7/16/04 6:22p darnstein
 * Don't de-initialize IN656 core.  It may hang the memory bus!
 *
 * Hydra_Software_Devel/I656/4   7/15/04 8:15p darnstein
 * Don't initialize IN656 core.  It may hang the memory bus!
 *
 * Hydra_Software_Devel/I656/3   7/15/04 5:51p darnstein
 * Allow choice of sdid field in ancillary data packets written out.
 *
 * Hydra_Software_Devel/I656/2   7/8/04 7:44p darnstein
 * ITU-R 656 decoding of VBI seems to be ready for bring up.  Expect bugs.
 *
 * Hydra_Software_Devel/I656/1   6/28/04 1:10p darnstein
 * 656 output is ready for testing.
 *
 * Hydra_Software_Devel/36   6/17/04 6:19p darnstein
 * PR 11443: Get rid of a kludge that was only needed in revision -A0 of
 * the hardware.
 *
 * Hydra_Software_Devel/35   6/17/04 5:54p darnstein
 * PR 9080: Do a better job of handling the fact that WSS and VPS only
 * occur on top field.
 *
 * Hydra_Software_Devel/34   5/6/04 6:16p darnstein
 * PR 9080: choose video line for interrupt, even if no VBI data is to be
 * processed. Useful for testing.
 *
 * Hydra_Software_Devel/33   4/29/04 3:31p darnstein
 * PR 9080: do video format check before programming VPS core.
 *
 * Hydra_Software_Devel/32   4/2/04 2:12p darnstein
 * PR 9080: change names of BVBI SetDisplayFormat functions to
 * SetVideoFormat. This change prevents wastage of VS cycles.
 *
 * Hydra_Software_Devel/31   3/26/04 1:44p darnstein
 * PR 8543: Work around for "teletext lines in use" hardware bug.
 *
 * Hydra_Software_Devel/30   3/18/04 11:03a darnstein
 * PR 9080: program the VD_TOP_vd_interrupt register to get decoding ISR
 * at the correct moment.
 *
 * Hydra_Software_Devel/29   3/12/04 5:51p darnstein
 * PR 9080: Teletext for NTSC is working, with a kludge for an A0 hardware
 * flaw.  There is a lot of debug code that will be removed later.
 *
 * Hydra_Software_Devel/28   3/4/04 4:23p darnstein
 * PR 9080: Fix some logic errors concerning activation of VBI.  Add some
 * functionality needed by progressive video formats.
 *
 * Hydra_Software_Devel/27   2/27/04 6:13p darnstein
 * PR 9080: Fix misuse of BDBG_ASSERT macro.  Add checks for valid video
 * format.
 *
 * Hydra_Software_Devel/26   2/26/04 3:13p darnstein
 * PR 9906: When doing CRC computation in software, cache last computation
 * for reuse, to save CPU cycles.
 *
 * Hydra_Software_Devel/25   2/6/04 11:48a darnstein
 * PR 9080: Better handle VBI decoding errors.
 *
 * Hydra_Software_Devel/24   2/3/04 4:42p darnstein
 * PR 9080: fix logic errors in handling decoding errors. A few other
 * minor fixes.
 *
 * Hydra_Software_Devel/23   1/27/04 6:22p darnstein
 * PR 9080: Don't check for overrun errors on encoding.  It is only
 * possible for closed caption encoding, anyway.
 *
 * Hydra_Software_Devel/22   1/23/04 4:08p darnstein
 * PR 9080: Routines used in (pseudo) ISRs have no critical sections.
 *
 * Hydra_Software_Devel/21   1/20/04 1:20p darnstein
 * PR 9338: Don't fiddle with VBI_ENC_xxx_Control registers.  The VDC
 * module owns them now.
 *
 * Hydra_Software_Devel/20   1/15/04 4:23p darnstein
 * PR 9080: Many fixes.  Closed caption encoding seems to work now.
 *
 * Hydra_Software_Devel/19   12/22/03 5:18p darnstein
 * PR 9080: fix a stupid bug.
 *
 * Hydra_Software_Devel/18   12/19/03 5:20p darnstein
 * PR 9080: adapt to changed BAVC enum for field polarity.
 *
 * Hydra_Software_Devel/17   12/19/03 5:04p darnstein
 * PR 9080: many changes. Still not working.
 *
 * Hydra_Software_Devel/16   10/17/03 3:38p darnstein
 * VBI encoding can be disabled for one field time.
 *
 * Hydra_Software_Devel/15   10/16/03 4:27p darnstein
 * Use revised #define BAVC_SourceId_eVdec0.
 *
 * Hydra_Software_Devel/14   10/16/03 1:21p darnstein
 * Fix usage of BDBG_ENTER(), BDBG_LEAVE().
 *
 * Hydra_Software_Devel/13   10/14/03 11:26a darnstein
 * Change polarity to polarityMask in BVBI_P_Field_Handle.
 *
 * Hydra_Software_Devel/12   10/9/03 4:00p darnstein
 * Change name of several API functions to indicate that they may be
 * called from an ISR.  Add polarity attribute to BVBI_Field_Handle and
 * use it in the API.  Fix some typing errors.
 *
 * Hydra_Software_Devel/11   10/9/03 1:17p darnstein
 * Add "use count" attribute to BVBI_Field_Handle.  Not for general use!
 *
 * Hydra_Software_Devel/10   10/2/03 2:25p darnstein
 * Remove improper use of BERR_TRACE.
 * Put in some missing BDBG_LEAVE statements.
 *
 * Hydra_Software_Devel/9   10/1/03 10:54a darnstein
 * Take out some debug code that caused problems in nightly build.
 *
 * Hydra_Software_Devel/8   9/25/03 5:37p darnstein
 * Fix simple typo for BDBG_ASSERT
 *
 * Hydra_Software_Devel/7   9/25/03 4:48p darnstein
 * BVBI module is mostly complete. The only things that I know are missing
 * are the critical sections, 656 support, macrovision support, and LOTS
 * OF TESTING.
 *
 * Hydra_Software_Devel/6   9/12/03 6:50p darnstein
 * Done except for VPS, teletext, critical sections, and (lots of)
 * testing.
 *
 * Hydra_Software_Devel/5   9/10/03 5:20p darnstein
 * Change API according to agreement with Jason (yesterday).  Add
 * documentation of function return values.  More documentation is needed
 * though.
 *
 * Hydra_Software_Devel/4   9/9/03 7:53p darnstein
 * I think all API functions have been implemented, though some with
 * stubs.
 *
 * Hydra_Software_Devel/3   9/2/03 8:08p darnstein
 * Still not ready for use.
 *
 * Hydra_Software_Devel/2   8/29/03 6:47p darnstein
 * Implementation of the most general parts of the VBI API.
 *
 * Hydra_Software_Devel/1   8/28/03 11:35a darnstein
 * Implementation of the most general parts of the VBI API.
 *
 ***************************************************************************/

#include "bstd.h"                /* standard types */
#include "bdbg.h"                /* Dbglib */
#include "bkni.h"                /* malloc */
#include "bvbi.h"                /* VBI processing, this module. */
#include "bvbi_priv.h"           /* VBI internal data structures */

#ifdef BCHP_PWR_SUPPORT
#include "bchp_pwr.h"
#endif

BDBG_MODULE(BVBI);

/***************************************************************************
* Private data items
***************************************************************************/

static const BVBI_Settings s_DefaultSettings =
{
	24576,						/* in656bufferSize    */
	false                       /* tteShiftDirMsb2Lsb */
};

static const BVBI_XSER_Settings s_XSER_DefaultSettings =
{
	BVBI_TTserialDataContent_DataMagFrmRun,    /* xsSerialDataContent */
	BVBI_TTserialDataSync_EAV,                 /* ttSerialDataSync */
	27                                         /* iTTserialDataSyncDelay */
};
/* TODO: determine sensible defaults (above) from our customers. */

/***************************************************************************
* Implementation of "BVBI_" API functions
***************************************************************************/

/***************************************************************************
 *
 */
BERR_Code BVBI_GetDefaultSettings(  BVBI_Settings *pSettings )
{
	BDBG_ENTER(BVBI_GetDefaultSettings);

	if (!pSettings)
	{
		BDBG_ERR(("Invalid parameter\n"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	*pSettings = s_DefaultSettings;

	BDBG_LEAVE(BVBI_GetDefaultSettings);

	return BERR_SUCCESS;
}


/***************************************************************************
 *
 */
BERR_Code BVBI_Open
	( BVBI_Handle		*pVbiHandle,
	  BCHP_Handle		 hChip,
	  BREG_Handle		 hRegister,
	  BMEM_Handle		 hMemory,
	  const BVBI_Settings		*pDefSettings )
{
	int type;
	BERR_Code eErr;
	BVBI_P_Handle *pVbi = NULL;
	const BVBI_Settings* settings =
		(pDefSettings ? pDefSettings : &s_DefaultSettings);

	BDBG_ENTER(BVBI_Open);

	if((!pVbiHandle) ||
	   (!hChip) ||
	   (!hRegister) ||
	   (!hMemory))
	{
		BDBG_ERR(("Invalid parameter\n"));
		eErr = BERR_TRACE(BERR_INVALID_PARAMETER);
		goto BVBI_Open_Done;
	}

	/* Alloc the main VBI context. */
	pVbi = (BVBI_P_Handle*)(BKNI_Malloc(sizeof(BVBI_P_Handle)));

	if(!pVbi)
	{
		eErr = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
		goto BVBI_Open_Done;
	}

	/* Clear out the context and set defaults. */
	BKNI_Memset((void*)pVbi, 0x0, sizeof(BVBI_P_Handle));

	/* Initialize magic number to the size of the struct */
	pVbi->ulBlackMagic = sizeof(BVBI_P_Handle);

	/* Store the hChip, hRegister, hMemory, and hRdc for later use. */
	pVbi->hChip = hChip;
	pVbi->hReg  = hRegister;
	pVbi->hMem  = hMemory;

	/* Store other settings from user */
	pVbi->in656bufferSize = settings->in656bufferSize;
	pVbi->tteShiftDirMsb2Lsb = settings->tteShiftDirMsb2Lsb;

	/* Initialize empty lists of decode and encode contexts */
	BLST_D_INIT(&pVbi->decode_contexts);
	BLST_D_INIT(&pVbi->encode_contexts);

	/* Initialize freelists for bulky data */
	BVBI_P_LCOP_INITFREELIST (&pVbi->ttFreelist);

	/* Clear out list of "in use" VEC cores */
	for (type = 0 ; type < BVBI_P_EncCoreType_eLAST ; ++type)
	{
		pVbi->vecHwCoreMask[type] = 0x0;
		pVbi->vecHwCoreMask_656[type] = 0x0;
	}

#ifdef BCHP_PWR_RESOURCE_VDC_VEC
	BCHP_PWR_AcquireResource(pVbi->hChip, BCHP_PWR_RESOURCE_VDC_VEC);
#endif
#ifdef BCHP_PWR_RESOURCE_VDC_656_OUT
	BCHP_PWR_AcquireResource(pVbi->hChip, BCHP_PWR_RESOURCE_VDC_656_OUT);
#endif

	/* Bang on the hardware until it makes a nice sound */
	if ((eErr = BERR_TRACE (BVBI_P_CC_Init (pVbi))) != BERR_SUCCESS)
	{
		goto BVBI_Open_Done;
	}
	if ((eErr = BERR_TRACE (BVBI_P_CGMS_Init (pVbi))) != BERR_SUCCESS)
	{
		goto BVBI_Open_Done;
	}
	if ((eErr = BERR_TRACE (BVBI_P_WSS_Init (pVbi))) != BERR_SUCCESS)
	{
		goto BVBI_Open_Done;
	}
	if ((eErr = BERR_TRACE (BVBI_P_VPS_Init (pVbi))) != BERR_SUCCESS)
	{
		goto BVBI_Open_Done;
	}
	if ((eErr = BERR_TRACE (BVBI_P_TT_Init (pVbi))) != BERR_SUCCESS)
	{
		goto BVBI_Open_Done;
	}
#if (BVBI_P_NUM_GSE > 0)
	if ((eErr = BERR_TRACE (BVBI_P_GS_Init (pVbi))) != BERR_SUCCESS)
	{
		goto BVBI_Open_Done;
	}
#endif 
#if (BVBI_P_NUM_AMOLE > 0)
	if ((eErr = BERR_TRACE (BVBI_P_AMOL_Init (pVbi))) != BERR_SUCCESS)
	{
		goto BVBI_Open_Done;
	}
#endif 
#if (BVBI_P_NUM_SCTEE > 0)  
	if ((eErr = BERR_TRACE (BVBI_P_SCTE_Init (pVbi))) != BERR_SUCCESS)
	{
		goto BVBI_Open_Done;
	}
#endif 
	if ((eErr = BERR_TRACE (BVBI_P_VE_Init (pVbi))) != BERR_SUCCESS)
	{
		goto BVBI_Open_Done;
	}
#if (BVBI_P_HAS_XSER_TT)
	if ((eErr = BERR_TRACE (
		BVBI_P_ITU656_Init (pVbi->hReg, BVBI_P_GetDefaultXserSettings())))
		!= BERR_SUCCESS)
	{
		goto BVBI_Open_Done;
	}
#endif
#if (BVBI_P_NUM_ANCI656_656 > 0)
	if ((eErr = BERR_TRACE (BVBI_P_A656_Init (pVbi))) != BERR_SUCCESS)
	{
		goto BVBI_Open_Done;
	}
#endif
#if (BVBI_P_NUM_IN656 > 0)
	if ((eErr = BERR_TRACE (BVBI_P_IN656_Init (pVbi))) != BERR_SUCCESS)
	{
		goto BVBI_Open_Done;
	}
#endif

	/* All done. now return the new fresh context to user. */
	*pVbiHandle = (BVBI_Handle)pVbi;

  BVBI_Open_Done:

	BDBG_LEAVE(BVBI_Open);

	if ((BERR_SUCCESS != eErr) && (NULL != pVbi)) {
		/* release power if open fails */
#ifdef BCHP_PWR_RESOURCE_VDC_656_OUT
		BCHP_PWR_ReleaseResource(pVbi->hChip, BCHP_PWR_RESOURCE_VDC_656_OUT);
#endif
#ifdef BCHP_PWR_RESOURCE_VDC_VEC
		BCHP_PWR_ReleaseResource(pVbi->hChip, BCHP_PWR_RESOURCE_VDC_VEC);
#endif
		BKNI_Free((void*)pVbi);
	}

	return eErr;
}


/***************************************************************************
 *
 */
void  BVBI_Close ( BVBI_Handle vbiHandle )
{
	BVBI_P_Handle *pVbi;
	BERR_Code eErr;

	BDBG_ENTER(BVBI_Close);

	/* check parameters */
	BVBI_P_GET_CONTEXT(vbiHandle, pVbi);
	BDBG_ASSERT (pVbi != NULL);

	/* Refuse service if user left any decoder objects open */
	BDBG_ASSERT (NULL == BLST_D_FIRST (&pVbi->decode_contexts));

	/* Refuse service if user left any encoder objects open */
	BDBG_ASSERT (NULL == BLST_D_FIRST (&pVbi->encode_contexts));

	/* Close the individual cores.  These same functions were used in
	   BVBI_Open(). */
	eErr = BERR_TRACE (BVBI_P_CC_Init   (pVbi));
	BDBG_ASSERT (eErr == BERR_SUCCESS);
	eErr = BERR_TRACE (BVBI_P_CGMS_Init (pVbi));
	BDBG_ASSERT (eErr == BERR_SUCCESS);
	eErr = BERR_TRACE (BVBI_P_WSS_Init  (pVbi));
	BDBG_ASSERT (eErr == BERR_SUCCESS);
	eErr = BERR_TRACE (BVBI_P_TT_Init   (pVbi));
	BDBG_ASSERT (eErr == BERR_SUCCESS);
	eErr = BERR_TRACE (BVBI_P_VPS_Init  (pVbi));
	BDBG_ASSERT (eErr == BERR_SUCCESS);
#if (BVBI_P_NUM_GSE > 0)
	eErr = BERR_TRACE (BVBI_P_GS_Init  (pVbi));
	BDBG_ASSERT (eErr == BERR_SUCCESS);
#endif 
#if (BVBI_P_NUM_AMOLE > 0)
	eErr = BERR_TRACE (BVBI_P_AMOL_Init  (pVbi));
	BDBG_ASSERT (eErr == BERR_SUCCESS);
#endif 
#if (BVBI_P_NUM_SCTEE > 0)  
	eErr = BERR_TRACE (BVBI_P_SCTE_Init  (pVbi));
	BDBG_ASSERT (eErr == BERR_SUCCESS);
#endif 
	eErr = BERR_TRACE (BVBI_P_VE_Init   (pVbi));
	BDBG_ASSERT (eErr == BERR_SUCCESS);
#if (BVBI_P_HAS_XSER_TT)
	eErr =
		BERR_TRACE (
			BVBI_P_ITU656_Init (pVbi->hReg, BVBI_P_GetDefaultXserSettings()));
	BDBG_ASSERT (eErr == BERR_SUCCESS);
#endif
#if (BVBI_P_NUM_ANCI656_656 > 0)
	eErr = BERR_TRACE (BVBI_P_A656_Init (pVbi));
	BDBG_ASSERT (eErr == BERR_SUCCESS);
#endif
#if (BVBI_P_NUM_IN656 > 0)
	eErr = BERR_TRACE (BVBI_P_IN656_Init(pVbi));
	BDBG_ASSERT (eErr == BERR_SUCCESS);
#endif

	/* The handle is invalid */
	pVbi->ulBlackMagic = 0;

	/* Sanity check for bulky storage */
	BDBG_ASSERT (pVbi->ttFreelist.l_co == 0x0);

#ifdef BCHP_PWR_RESOURCE_VDC_656_OUT
	BCHP_PWR_ReleaseResource(pVbi->hChip, BCHP_PWR_RESOURCE_VDC_656_OUT); 
#endif
#ifdef BCHP_PWR_RESOURCE_VDC_VEC
	BCHP_PWR_ReleaseResource(pVbi->hChip, BCHP_PWR_RESOURCE_VDC_VEC);
#endif

	/* Release context in system memory */
	BKNI_Free((void*)pVbi);

	BDBG_LEAVE(BVBI_Close);
}

/***************************************************************************
 *
 */
BERR_Code BVBI_Standby
	( BVBI_Handle hVbi,
	  BVBI_StandbySettings *pSettings )
{
	BVBI_P_Handle *pVbi;
	BSTD_UNUSED(pSettings); /* unused for now */

	BVBI_P_GET_CONTEXT(hVbi, pVbi);
	BDBG_ASSERT (pVbi != NULL);

	/* TODO: if BVBI is in use and there are clocks that need to be kept powered, 
	   we should short-circuit and return an error. for now, no such check is deemed necessary */
	if (0) {
		BDBG_ERR(("Cannot enter standby due to ( ) in use"));
		return BERR_UNKNOWN;
	}

#ifdef BCHP_PWR_RESOURCE_VDC_656_OUT
	BCHP_PWR_ReleaseResource(pVbi->hChip, BCHP_PWR_RESOURCE_VDC_656_OUT);
#endif
#ifdef BCHP_PWR_RESOURCE_VDC_VEC
	BCHP_PWR_ReleaseResource(pVbi->hChip, BCHP_PWR_RESOURCE_VDC_VEC);
#endif
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVBI_Resume (BVBI_Handle hVbi)
{
	BVBI_P_Handle *pVbi;

	BVBI_P_GET_CONTEXT(hVbi, pVbi);
	BDBG_ASSERT (pVbi != NULL);

#ifdef BCHP_PWR_RESOURCE_VDC_VEC
	BCHP_PWR_AcquireResource(pVbi->hChip, BCHP_PWR_RESOURCE_VDC_VEC);
#endif
#ifdef BCHP_PWR_RESOURCE_VDC_656_OUT
	BCHP_PWR_AcquireResource(pVbi->hChip, BCHP_PWR_RESOURCE_VDC_656_OUT);
#endif
	return BERR_SUCCESS;
}

/***************************************************************************
 * VBI private (top level) functions
 ***************************************************************************/

const BVBI_XSER_Settings* BVBI_P_GetDefaultXserSettings (void)
{
	return &s_XSER_DefaultSettings;
}

/***************************************************************************
 *
 */
uint32_t BVBI_P_LEBE_SWAP (uint32_t ulDatum)
{
	uint8_t temp;
	union {
		uint8_t b[4];
		uint32_t l;
	} scratch;

	scratch.l = ulDatum;
	P_SWAP (scratch.b[0], scratch.b[3], temp);
	P_SWAP (scratch.b[1], scratch.b[2], temp);

	return scratch.l;
}


/***************************************************************************
 *
 */
uint32_t BVBI_WSS2CGMS (uint16_t wssData)
{
	int iBit;
	uint32_t cgmsData;

	/* Insert the 6 bit preamble */
	cgmsData = 0x3E;

	/* Left justify the user's data */
	wssData <<= 3;

	/* Slide in the user's data */
	for (iBit = 0 ; iBit < 13 ; ++iBit)
	{
		cgmsData <<= 2;

		/* Process input data MSb first */
		if (wssData & (uint16_t)(0x8000))
		{
			/* 1 bits get encoded as 10 */
			cgmsData |= 0x2;
		}
		else
		{
			/* 0 bits get encoded as 01 */
			cgmsData |= 0x1;
		}

		/* Next bit please */
		wssData <<= 1;
	}

	return cgmsData;
}

/* End of File */
