/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvbi_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/16 $
 * $brcm_Date: 4/24/12 12:03p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vbi/7400/bvbi_priv.h $
 * 
 * Hydra_Software_Devel/16   4/24/12 12:03p darnstein
 * SW7405-5643: BVBI API change. BVBI_Encode_Create() now includes a
 * settings argument. It is a data structure that includes an option for
 * ARIB style video output.
 * 
 * Hydra_Software_Devel/15   4/21/11 7:21p darnstein
 * SW7125-916: Wrong interrupt control register was being programmed.
 * 
 * Hydra_Software_Devel/14   3/22/11 12:57p darnstein
 * SW7420-1693: CGMS for 480P video needs to take "VEC double rate
 * sampling" into account.
 * 
 * Hydra_Software_Devel/13   3/2/11 3:38p darnstein
 * SW7422-261: More problems with line numbering for PAL-M and multi-line
 * closed captioning. This time, chipsets whose CCE cores lack "SCTE"
 * extensions were affected.
 * 
 * Hydra_Software_Devel/12   10/1/10 2:49p darnstein
 * SW7422-46: Adapt to 7422 and 7425 chipsets.
 * 
 * Hydra_Software_Devel/11   3/3/10 8:08p darnstein
 * SW7420-598: Adjust #ifdefs that select capabilities CGMS-B, CGMS-Bd.
 * 
 * Hydra_Software_Devel/10   11/18/09 3:54p darnstein
 * SW7468-24: Gemstar options now placed in dedicated data structure.
 * 
 * Hydra_Software_Devel/9   6/24/09 4:38p darnstein
 * PR56289: BVBI compiles for 7340 chipset now.
 * 
 * Hydra_Software_Devel/8   3/27/09 5:56p darnstein
 * PR53635: Remove internal ConfigForOthers code. It is obsolete, and it
 * was causing a problem (this PR).
 * 
 * Hydra_Software_Devel/7   2/20/09 12:37p darnstein
 * PR49987: Add a missing piece of logic for the bBrokenDataCount option.
 * 
 * Hydra_Software_Devel/6   2/9/09 7:57p darnstein
 * PR45819: minor update for 7420 and CGMSAE core: version numbers.
 * 
 * Hydra_Software_Devel/5   12/11/08 4:23p darnstein
 * PR45819: program VBI_ENC and VEC_CFG cores.
 * 
 * Hydra_Software_Devel/4   12/9/08 12:04p darnstein
 * PR49987: remove some stray lines of code (copy and paste error).
 * 
 * Hydra_Software_Devel/3   12/8/08 7:36p darnstein
 * PR49987: fix bug in BVBI_Encode_Create(). Affects 656 output.
 * 
 * Hydra_Software_Devel/2   12/3/08 7:57p darnstein
 * PR45819: New, more modular form of most BVBI source files.
 * 
 * Hydra_Software_Devel/92   9/16/08 6:44p darnstein
 * PR46112: The new standard for CGMS-B CEA-805-D is now available for
 * chip 7400-E0.
 * 
 * Hydra_Software_Devel/91   9/15/08 8:58p darnstein
 * PR46356: VPS decoder software in BVBI is ready for test.
 * 
 * Hydra_Software_Devel/90   9/15/08 2:45p darnstein
 * PR44539: add support for the latest version of the WSE core hardware.
 * That is, the version present in 7601-A0.
 * 
 * Hydra_Software_Devel/89   9/12/08 1:24p darnstein
 * PR46112: Implement variant of CGMS-B (output) defined in CEA-805-D.
 * 
 * Hydra_Software_Devel/88   7/30/08 8:39p darnstein
 * PR43926,PR43413: Workaround for TTD addressing error in hardware.
 * Decodes at most one line of teletext per video field. Will not
 * corrrupt memory if input signal actually contains more teletext.
 * 
 * Hydra_Software_Devel/87   7/30/08 5:41p darnstein
 * PR43926: A basic solution to the memory addressing problem is in place.
 * It can decode one line of teletext. If more teletext is present, then
 * memory corruption will occur. This danger will be eliminated next.
 * 
 * Hydra_Software_Devel/86   7/17/08 8:44p darnstein
 * PR44539: compilation now possible for 7601.
 * 
 * Hydra_Software_Devel/85   6/6/08 5:36p darnstein
 * PR38956: compile in support for SCTE and AMOL in 93548.
 * 
 * Hydra_Software_Devel/84   5/28/08 5:35p darnstein
 * PR23220: Some implementation of Gemstar decoding was missing.
 * 
 * Hydra_Software_Devel/83   5/20/08 6:26p darnstein
 * PR42898: 7325-B0 and 7335-C0 have the "version 2" CGMSAE core.
 * 
 * Hydra_Software_Devel/82   5/13/08 2:15p darnstein
 * PR34584: Port over changes from 7401 dedicated branch. Allow user to
 * choose bit shift direction for teletext encoding.
 * 
 * Hydra_Software_Devel/81   5/12/08 2:56p darnstein
 * PR42416: Implement Grossman's work-around for the conflict between VDEC
 * frame comb filter and teletext decoder (TTD).
 * 
 * Hydra_Software_Devel/80   4/28/08 7:49p darnstein
 * PR38956: CGMS-B encoding ready for bring-up. Need accurate register
 * settings for tuning.
 * 
 * Hydra_Software_Devel/79   4/8/08 6:14p darnstein
 * PR37064: This is the same 15-lines-of-teletext problem that was solved
 * (worked-around, actually) on the dedicated 7401 ClearCase branch.
 * 
 * Hydra_Software_Devel/78   4/8/08 5:53p darnstein
 * PR41429: 97400-E0 chipset has CGMSAE core hardware version 2.
 * 
 * Hydra_Software_Devel/77   4/2/08 7:55p darnstein
 * PR38956: VBI software compiles now.
 * 
 * Hydra_Software_Devel/76   3/14/08 3:19p darnstein
 * PR23220: Gemstar decoding almost works. Cannot process video line 11
 * for some reason.
 * 
 * Hydra_Software_Devel/75   3/11/08 6:50p darnstein
 * PR23220: fill in some missing Gemstar decoder logic.
 * 
 * Hydra_Software_Devel/74   1/7/08 12:10p darnstein
 * PR22719: Apply TTX ADDR software workaround to the chip revisions that
 * actually need it. Using email 2008-01-06 from Ali Syed Mohammed as a
 * reference.
 * 
 * Hydra_Software_Devel/73   12/4/07 1:19p darnstein
 * PR36897: VBI software is ready for testing with 97335 chipset.
 * 
 * Hydra_Software_Devel/72   10/31/07 3:50p darnstein
 * PR34528: BVBI is ready for testing on 7325.
 * 
 * Hydra_Software_Devel/71   10/10/07 4:00p darnstein
 * PR35440: handle ancillary VEC and passthrough VEC in the same fashion.
 * Since there is already a #define macro for modular VEC, this seems
 * more graceful than before.
 * 
 * Hydra_Software_Devel/70   9/12/07 4:56p darnstein
 * PR29723: For 7405 SCTE encoder core, account for register differences
 * with same core in 7403 chipset.
 * 
 * Hydra_Software_Devel/69   9/11/07 6:39p darnstein
 * PR25708: fix ClearCase merge error.
 * 
 * Hydra_Software_Devel/68   9/11/07 5:14p darnstein
 * PR25708: First release of SCTE encoder software.
 * 
 * Hydra_Software_Devel/67   6/6/07 12:49p darnstein
 * PR30411: multi-line closed caption encoding is ready for test.
 * 
 * Hydra_Software_Devel/66   6/5/07 3:27p darnstein
 * PR21831: Let hardware compute WSS parity bit on 97403 chipset.
 * 
 * Hydra_Software_Devel/65   4/20/07 3:35p darnstein
 * PR29723: Compilation for 7405 chipset.
 * 
 * Hydra_Software_Devel/64   2/26/07 2:25p darnstein
 * PR25990: Some minor changes were required for compiling on 7400-B0.
 * 
 * Hydra_Software_Devel/63   2/20/07 10:43a darnstein
 * PR27521: Apply software workaround. The TTE core is reset, at every
 * video field. Also, the encoder output for the field NOT in use is
 * disabled, at every video field.
 * 
 * Hydra_Software_Devel/62   1/17/07 5:32p darnstein
 * PR26464: correctly handle teletext output to multiple VECs
 * 
 * Hydra_Software_Devel/61   12/15/06 2:33p darnstein
 * PR25990: Add a comment describing symbol BVBI_P_NEWER_SOFT_RESET.
 * 
 * Hydra_Software_Devel/60   12/14/06 7:16p darnstein
 * PR25990: Can compile for BCM97400-B0 now.
 * 
 * Hydra_Software_Devel/59   11/16/06 4:43p darnstein
 * PR25668: Mechanical changes to support the 97403 chipset.
 * 
 * Hydra_Software_Devel/58   11/13/06 3:07p darnstein
 * PR23247: Serial output of teletext is ready for testing.
 * 
 * Hydra_Software_Devel/57   11/9/06 4:53p darnstein
 * PR23247: The 3563 chipset is not ready for serial output of teletext
 * yet.
 * 
 * Hydra_Software_Devel/56   11/8/06 5:35p darnstein
 * PR23247: Merge back to main branch.
 * 
 * Hydra_Software_Devel/Refsw_Devel_3563/1   11/8/06 5:13p darnstein
 * PR23247: Serial output of teletext is partially supported now.
 * 
 * Hydra_Software_Devel/55   9/26/06 7:37p darnstein
 * PR24573: The BVBI porting interface module now compiles for the 7440
 * chipset. Correct operation has not been verified.
 * 
 * Hydra_Software_Devel/54   9/13/06 6:34p darnstein
 * PR21688: Can now compile for 97118-A0 chipset. Correct operation is not
 * guaranteed.
 * 
 * Hydra_Software_Devel/53   9/1/06 4:48p darnstein
 * PR23242: Use encoder auto-parity when available. Some code commented
 * out because RDB header files are not ready yet.
 * 
 * Hydra_Software_Devel/52   8/31/06 2:09p darnstein
 * PR23869: clean up the handling of multiple VECs and VDECs.
 * 
 * Hydra_Software_Devel/51   8/18/06 6:51p darnstein
 * PR23178: basic compile on 93563 is possible.
 * 
 * Hydra_Software_Devel/50   7/24/06 4:45p darnstein
 * PR22720: Implement the software workaround: duplicate the first framing
 * code byte.
 * 
 * Hydra_Software_Devel/49   7/20/06 2:23p darnstein
 * PR21688: Use the new hardware soft reset scheme for later model chips.
 * 
 * Hydra_Software_Devel/58   11/13/06 3:07p darnstein
 * PR23247: Serial output of teletext is ready for testing.
 * 
 * Hydra_Software_Devel/57   11/9/06 4:53p darnstein
 * PR23247: The 3563 chipset is not ready for serial output of teletext
 * yet.
 * 
 * Hydra_Software_Devel/56   11/8/06 5:35p darnstein
 * PR23247: Merge back to main branch.
 * 
 * Hydra_Software_Devel/Refsw_Devel_3563/1   11/8/06 5:13p darnstein
 * PR23247: Serial output of teletext is partially supported now.
 * 
 * Hydra_Software_Devel/55   9/26/06 7:37p darnstein
 * PR24573: The BVBI porting interface module now compiles for the 7440
 * chipset. Correct operation has not been verified.
 * 
 * Hydra_Software_Devel/54   9/13/06 6:34p darnstein
 * PR21688: Can now compile for 97118-A0 chipset. Correct operation is not
 * guaranteed.
 * 
 * Hydra_Software_Devel/53   9/1/06 4:48p darnstein
 * PR23242: Use encoder auto-parity when available. Some code commented
 * out because RDB header files are not ready yet.
 * 
 * Hydra_Software_Devel/52   8/31/06 2:09p darnstein
 * PR23869: clean up the handling of multiple VECs and VDECs.
 * 
 * Hydra_Software_Devel/51   8/18/06 6:51p darnstein
 * PR23178: basic compile on 93563 is possible.
 * 
 * Hydra_Software_Devel/50   7/24/06 4:45p darnstein
 * PR22720: Implement the software workaround: duplicate the first framing
 * code byte.
 * 
 * Hydra_Software_Devel/49   7/20/06 2:23p darnstein
 * PR21688: Use the new hardware soft reset scheme for later model chips.
 * 
 * Hydra_Software_Devel/48   4/12/06 1:57p darnstein
 * PR18010: Add ANCI656_PRIM_ANCI656_CNTRL setting to the PR18010
 * backwards compatibility option.
 * 
 * Hydra_Software_Devel/47   4/11/06 7:26p darnstein
 * PR20429: first cut at AMOL encoder software.
 * 
 * Hydra_Software_Devel/46   3/21/06 4:59p darnstein
 * PR20088: Improve handling of 7438 chip. It was broken before.
 * 
 * Hydra_Software_Devel/45   3/20/06 1:08p darnstein
 * PR19719: After changing BCHP_VBI_ENC_PRIM_Control (indirectly), poll
 * register until it has really changed. This enforces the one transition
 * that is critical to this PR: when disabling a VBI encoder core, the
 * appropriate bit in BCHP_VBI_ENC_PRIM_Control must be zeroed before the
 * individual VBI core is disabled.
 * 
 * Hydra_Software_Devel/44   3/9/06 2:02p darnstein
 * PR 20088: streamline the way that existence of EXT_656_TOP chip core is
 * handled.
 * 
 * Hydra_Software_Devel/43   3/8/06 4:55p darnstein
 * PR20088: add a clause for 7438 chip.
 * 
 * Hydra_Software_Devel/42   3/3/06 12:50p darnstein
 * PR18331: When BVBI_Decode_ApplyChanges() fails, roll back hardware
 * state completely. The state was random before this fix.
 * 
 * Hydra_Software_Devel/41   3/2/06 4:58p darnstein
 * PR18331: When BVBI_Encode_ApplyChanges() fails, roll back hardware
 * state completely. The state was random before this fix.
 * 
 * Hydra_Software_Devel/40   2/3/06 1:28p darnstein
 * PR19428: Add #ifdef's to support the 7400 chip.
 * 
 * Hydra_Software_Devel/39   12/21/05 5:15p darnstein
 * PR18341: Swap bytes of the teletext "line mask" DRAM entry on little-
 * endian systems.
 * 
 * Hydra_Software_Devel/38   12/5/05 7:29p darnstein
 * PR18010: Implement work-around for the bad line number, together with
 * an exception for customers that now depend on the bad line number.
 * 
 * Hydra_Software_Devel/37   11/14/05 7:05p darnstein
 * PR17732: Add macros for whether or not chip has Gemstar encoder,
 * component video input.
 * 
 * Hydra_Software_Devel/36   10/20/05 1:57p darnstein
 * PR17732: Gemstar features not ready yet. However, the build for 97038
 * is not broken.
 * 
 * Hydra_Software_Devel/35   9/19/05 2:57p darnstein
 * PR17151: Check for chip name where needed. Also, convert to new scheme
 * for testing chip revisions (BCHP_VER).
 * 
 * Hydra_Software_Devel/34   8/22/05 8:11p darnstein
 * PR16057: To support many different chips, use private #defines that
 * specify number of VECs, VDECs, and (separately) pass-through VECs.
 * 
 * Hydra_Software_Devel/33   7/7/05 3:34p darnstein
 * PR 16008: The default settings struct for BVBI_Open() now allows the
 * user to choose a buffer size for capturing ancillary data packets in
 * incoming ITU-R 656 digital video.
 * 
 * Hydra_Software_Devel/32   7/6/05 5:55p darnstein
 * PR 16008: Input of closed caption data in SAA7113 ancillary data
 * packets of ITU-R 656 digital video has been confirmed. SAA7114 input
 * almost certainly needs some debugging though.
 * 
 * Hydra_Software_Devel/31   6/20/05 12:03p darnstein
 * PR 15923: Adjust some #define's to support 93560 chip.
 * 
 * Hydra_Software_Devel/30   6/15/05 5:05p darnstein
 * PR 14487: Put in runtime checking for workaround for hardware PR 13583.
 * 
 * Hydra_Software_Devel/29   5/18/05 5:47p darnstein
 * PR 11440: Progress towards ITU-R 656 input of VBI data.
 * 
 * Hydra_Software_Devel/28   7/16/04 7:06p darnstein
 * PR 9080: merge in 656 input and output work. Some testing and debugging
 * remains to be done.
 * 
 * Hydra_Software_Devel/I656/3   7/15/04 5:52p darnstein
 * Allow choice of sdid field in ancillary data packets written out.
 * 
 * Hydra_Software_Devel/I656/2   7/8/04 7:44p darnstein
 * ITU-R 656 decoding of VBI seems to be ready for bring up.  Expect bugs.
 * 
 * Hydra_Software_Devel/I656/1   6/28/04 1:10p darnstein
 * 656 output is ready for testing.
 * 
 * Hydra_Software_Devel/27   6/17/04 6:19p darnstein
 * PR 11443: Get rid of a kludge that was only needed in revision -A0 of
 * the hardware.
 * 
 * Hydra_Software_Devel/26   6/17/04 5:54p darnstein
 * PR 9080: Do a better job of handling the fact that WSS and VPS only
 * occur on top field.
 * 
 * Hydra_Software_Devel/25   4/29/04 3:31p darnstein
 * PR 9080: do video format check before programming VPS core.
 * 
 * Hydra_Software_Devel/24   3/26/04 1:45p darnstein
 * PR 8543: Workaround for "teletext lines in use" hardware bug.
 * 
 * Hydra_Software_Devel/23   3/18/04 11:07a darnstein
 * PR 9080: declare a private function that programs the
 * VD_TOP_vd_interrupt register.
 * 
 * Hydra_Software_Devel/22   3/12/04 5:51p darnstein
 * PR 9080: Teletext for NTSC is working, with a kludge for an A0 hardware
 * flaw.  There is a lot of debug code that will be removed later.
 * 
 * Hydra_Software_Devel/21   3/4/04 4:24p darnstein
 * PR 9080: add some functionality required by progressive video formats.
 * Improve handling of teletext private data.
 * 
 * Hydra_Software_Devel/20   2/27/04 6:13p darnstein
 * PR 9080: Add checks for valid video format.
 * 
 * Hydra_Software_Devel/19   2/26/04 3:13p darnstein
 * PR 9906: When doing CRC computation in software, cache last computation
 * for reuse, to save CPU cycles.
 * 
 * Hydra_Software_Devel/18   2/26/04 2:05p darnstein
 * PR 9906: Compute CRC in software for CGMS encoder.
 * 
 * Hydra_Software_Devel/17   2/6/04 11:49a darnstein
 * PR 9080: better handle VBI decoding errors.
 * 
 * Hydra_Software_Devel/16   2/3/04 4:42p darnstein
 * PR 9080: fix logic errors in handling decoding errors. A few other
 * minor fixes.
 * 
 * Hydra_Software_Devel/15   1/23/04 4:08p darnstein
 * PR 9080: Routines used in (pseudo) ISRs have no critical sections.
 * 
 * Hydra_Software_Devel/14   1/20/04 1:20p darnstein
 * PR 9338: Don't fiddle with VBI_ENC_xxx_Control registers.  The VDC
 * module owns them now.
 * 
 * Hydra_Software_Devel/13   1/15/04 4:29p darnstein
 * PR 9080: Some renaming and refactoring.
 * 
 * Hydra_Software_Devel/12   12/19/03 5:05p darnstein
 * PR 9080: several changes. Including: adapt to new BAVC enum for field
 * polarity.
 * 
 * Hydra_Software_Devel/11   10/17/03 3:38p darnstein
 * VBI encoding can be disabled for one field time.
 * 
 * Hydra_Software_Devel/10   10/14/03 11:26a darnstein
 * Change polarity to polarityMask in BVBI_P_Field_Handle.
 * 
 * Hydra_Software_Devel/9   10/9/03 4:00p darnstein
 * Add polarity attribute to BVBI_Field_Handle and use it in the API.
 * 
 * Hydra_Software_Devel/8   10/9/03 1:17p darnstein
 * Add "use count" attribute to BVBI_Field_Handle.  Not for general use!
 * 
 * Hydra_Software_Devel/7   9/25/03 4:48p darnstein
 * BVBI module is mostly complete. The only things that I know are missing
 * are the critical sections, 656 support, macrovision support, and LOTS
 * OF TESTING.
 * 
 * Hydra_Software_Devel/6   9/12/03 6:49p darnstein
 * Done except for VPS, teletext, critical sections, and (lots of)
 * testing.
 * 
 * Hydra_Software_Devel/5   9/10/03 5:19p darnstein
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
 * Private data structures to support the VBI API.
 * 
 * Hydra_Software_Devel/1   8/28/03 11:37a darnstein
 * Private data structures to support the VBI API.
 * 
 ***************************************************************************/
#ifndef BVBI_PRIV_H__
#define BVBI_PRIV_H__

#include "bvbi.h"
#include "bvbi_lcop.h"
#include "bvbi_util_priv.h"
#include "bvbi_chip_priv.h"
#include "blst_list.h"
#include "bavc_hdmi.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * This macro take the check for a validity of a handle, and
 * cast to context pointer.
 */
#define BVBI_P_GENERIC_GET_CONTEXT(handle, context, structname) \
do { \
	if(!(handle) || \
	   (((structname*)(handle))->ulBlackMagic != \
	   sizeof(structname))) \
	{ \
		BDBG_ERR(("Corrupted context handle\n")); \
		(context) = NULL; \
	} \
	else \
	{ \
		(context) = (structname*)(handle); \
	} \
} while (0)

/* Applications of the above macro */
#define BVBI_P_GET_CONTEXT(handle, context) \
	BVBI_P_GENERIC_GET_CONTEXT(handle, context, BVBI_P_Handle)
#define BVBI_P_GET_DECODE_CONTEXT(handle, context) \
	BVBI_P_GENERIC_GET_CONTEXT(handle, context, BVBI_P_Decode_Handle)
#define BVBI_P_GET_ENCODE_CONTEXT(handle, context) \
	BVBI_P_GENERIC_GET_CONTEXT(handle, context, BVBI_P_Encode_Handle)
#define BVBI_P_GET_FIELD_CONTEXT(handle, context) \
	BVBI_P_GENERIC_GET_CONTEXT(handle, context, BVBI_P_Field_Handle)

/* Convenience macro for swapping two values */
#define P_SWAP(A,B,TEMP)	\
do {						\
	(TEMP) = (A);			\
	(A)    = (B);			\
	(B)    = (TEMP);		\
} while (0)

/* Convenience function for doing little-endian <--> big-endian swap */
uint32_t BVBI_P_LEBE_SWAP (uint32_t ulDatum);

/***************************************************************************
 * VBI internal enumerations and constants
 ***************************************************************************/

/* bitmasks for individual VBI standards */
#define BVBI_P_SELECT_CC          0x0001
#define BVBI_P_SELECT_CGMSA       0x0002
#define BVBI_P_SELECT_WSS         0x0004
#define BVBI_P_SELECT_TT          0x0008
#define BVBI_P_SELECT_VPS         0x0010
#define BVBI_P_SELECT_GS          0x0020
#define BVBI_P_SELECT_AMOL        0x0040
#define BVBI_P_SELECT_SCTE        0x0080
#define BVBI_P_SELECT_MCC         0x0100
#define BVBI_P_SELECT_CGMSB       0x0200
#define BVBI_P_SELECT_VBIENC      0x0400	/* Pseudo standard */
#define BVBI_P_SELECT_ANCI        0x0800	/* Pseudo standard */
#define BVBI_P_SELECT_LAST        0x1000

/* Enumeration of VEC VBI cores. Not exactly the same as the above. */
typedef enum BVBI_P_EncCoreType
{
	BVBI_P_EncCoreType_eCCE = 0,
	BVBI_P_EncCoreType_eCGMSAE,
	BVBI_P_EncCoreType_eWSE,
	BVBI_P_EncCoreType_eTTE,
	BVBI_P_EncCoreType_eGSE,
	BVBI_P_EncCoreType_eAMOLE,
	BVBI_P_EncCoreType_eSCTE,
	BVBI_P_EncCoreType_eVBIENC,
	BVBI_P_EncCoreType_eANCI,
	BVBI_P_EncCoreType_eLAST	/* Must be last! */
} 
BVBI_P_EncCoreType;


/***************************************************************************
 * VBI Internal data structures
 ***************************************************************************/

/* Linked list support: define decode_head and encode_head. */
typedef struct decode_head decode_head;
BLST_D_HEAD(decode_head, BVBI_P_Decode_Handle);
typedef struct encode_head encode_head;
BLST_D_HEAD(encode_head, BVBI_P_Encode_Handle);

/* The chip-compatible storage of teletext data */
typedef BVBI_P_LCOP_COUNTEDPROP(BVBI_P_TTData) BVBI_P_TTData_Counted;
typedef struct BVBI_P_TTData
{
	BVBI_P_TTData_Counted clink;
	uint16_t  ucDataSize;  
	uint8_t   ucLines;
	uint8_t   ucLineSize;
	uint8_t   firstLine;
	uint32_t  lineMask;
	uint8_t  *pucData;

} BVBI_P_TTData;
typedef BVBI_P_LCOP_OWNERPROP(BVBI_P_TTData) BVBI_P_TTData_Owner;

typedef struct BVBI_P_Handle
{
	/* Black magic */
	uint32_t                       ulBlackMagic;

	/* handed down from app. */
	BCHP_Handle hChip;
	BREG_Handle hReg;
	BMEM_Handle hMem;
	size_t      in656bufferSize;
	bool        tteShiftDirMsb2Lsb;

	/* List of decode contexts */
	decode_head decode_contexts;

	/* List of encode contexts */
	encode_head encode_contexts;

	/* Free lists for bulky data */
	BVBI_P_TTData_Counted ttFreelist;

	/* Usage of VEC hardware cores. 
	 * Each entry refers to a type of hardware core.
	 * Each entry is a bitmask.
	 * In each entry, the bits positioned 0, 1, 2 indicate "in use." */
	uint8_t vecHwCoreMask[BVBI_P_EncCoreType_eLAST];
	uint8_t vecHwCoreMask_656[BVBI_P_EncCoreType_eLAST];

} BVBI_P_Handle;

/* Options for parsing SMPTE 291M ancillary data packets from ITU-R 656
   bitstream input */
typedef struct
{
	BVBI_Decode_656_SMPTE291M_Cb fParseCb;
	void* fParseCbArg0;
	bool bLongHeader;
	bool bBrokenDataCount;

} BVBI_P_SMPTE291Moptions;

/* 
 * SCTE data internal format. This supports interleaved CC, PAM, and NRTV data,
 * ordered by line number.
 */
typedef struct
{
	uint8_t cc_data_1;
	uint8_t cc_data_2;

} BVBI_P_SCTE_CC_Line_Data;

typedef struct
{
	uint8_t sequence_number;
	uint8_t segment_number;

} BVBI_P_SCTE_NTRV_Line_Data;

typedef struct
{
	bool first_segment_flag;
	bool last_segment_flag;
	uint16_t first_pixel_position;
	uint8_t n_pixels;
	int iArray;

} BVBI_P_SCTE_Mono_Line_Data;

typedef struct
{
	uint8_t valid;
	uint8_t priority;
	uint8_t line_number;
	BVBI_SCTE_Type eType;
	union 
	{
		BVBI_P_SCTE_CC_Line_Data     CC;
		BVBI_P_SCTE_NTRV_Line_Data NRTV;
		BVBI_P_SCTE_Mono_Line_Data Mono;
	} d;
} BVBI_P_SCTE_Line_Data;

typedef struct
{
	uint8_t  field_number;
	size_t line_count;
	size_t line_size;
	BVBI_P_SCTE_Line_Data* pLine;
	uint8_t* nrtv_data[2];
	size_t pam_data_count;
	size_t pam_data_size;
	uint8_t* pam_data;
	uint8_t* mono_data[2];

} BVBI_P_SCTE_Data;

/* The current/next state for decoder */
typedef struct BVBI_P_Decode_CNState
{
	/* What video display format to assume */
	BFMT_VideoFmt eVideoFormat;

	/* What format to expect for incoming VBI data in
	   ITU-R 656 ancillary data packets */
	BVBI_656Fmt e656Format;

	/* ITU-R 656 / SMPTE 291M options */
	BVBI_P_SMPTE291Moptions SMPTE291Moptions;

	/* WSS options */
	uint16_t wssVline576i;

	/* Gemstar options */
	BVBI_GSOptions gsOptions;

	/* Which VBI standards are active (bitmask of BVBI_P_SELECT_xyz) */
	uint32_t ulActive_Standards;

} BVBI_P_Decode_CNState;

/* The full state for decoder */
typedef struct BVBI_P_Decode_Handle
{
	/* Black magic */
	uint32_t ulBlackMagic;

	/* Current/next information */
	BVBI_P_Decode_CNState curr;
	BVBI_P_Decode_CNState next;
	bool gsOptionsChanged;

	/* Back pointer to the VBI context */
	BVBI_P_Handle *pVbi;

	/* Where to decode from */
	BAVC_SourceId eSource;

	/* LCO storage for bulky VBI data */
	BVBI_P_TTData_Owner topTTDataO;
	BVBI_P_TTData_Owner botTTDataO;

	#ifdef BVBI_P_TTD_SCB2_ERROR
	uint8_t* topTTscratch;
	uint8_t* botTTscratch;
	#endif

	/* Double buffer storage for ITU-R 656 ancillary data */
	uint8_t* top656Data;
	uint8_t* bot656Data;

	/* Linked list membership */
	BLST_D_ENTRY(BVBI_P_Decode_Handle) link;

} BVBI_P_Decode_Handle;

/* Options for writing ITU-R 656 ancillary data packets */
typedef struct
{
	BVBI_656Fmt e656Format;
	uint8_t sdid;

} BVBI_P_Encode_656_Options;

/* The current/next state for encoder */
typedef struct BVBI_P_Encode_CNState
{
	/* What video display format to assume */
	BFMT_VideoFmt eVideoFormat;

	/* HDMI pixel repetition to assume */
	BAVC_HDMI_PixelRepetition ePixRep;

	/* Options for encapsulating VBI data into 
	   ITU-R 656 ancillary data packets */
	BVBI_P_Encode_656_Options h656options;

	/* Which VBI standards are active for analog output 
	   (bitmask of BVBI_P_SELECT_xyz) */
	uint32_t ulActive_Standards;

	/* Same as above, but for ITU-R 656 output instead of analog output. */
	uint32_t ulActive_656_Standards;

	/* Same as above, but for serial output. */
	uint32_t ulActive_XSER_Standards;

	/* Serial output options */
	BVBI_XSER_Settings xserSettings;

	/* Gemstar options */
	BVBI_GSOptions gsOptions;

	/* AMOL options */
	BVBI_AMOL_Type amolType;

	/* SCTE options */
	BVBI_SCTE_Type scteType;
	BVBI_CSC eCsc;
	BVBI_CSC eCoCsc;

	/* CGMS-B options */
	bool bCea805dStyle;

	/* As of 2005-12-05, all chip products 97038 and later are affected by
	 * PR18010. The BVBI porting interface "works around" this bug in software,
	 * by default. However, when this flag is "true," then the buggy behavior
	 * will occur. This flag is provided for customers who have accomodated the
	 * buggy behavior and do not wish to switch over to correct behavior.
	 */
	bool bPR18010_bad_line_number;

	/* This identifies each VEC VBI core to use. 
	 * Each entry refers to a type of hardware core. 
	 * Each entry is either 0, 1, 2, or 0xFF.
	 * 0xFF means "unassigned."
	 * Entry BVBI_P_EncCoreType_eVBIENC is a special case. There is only a
	 * single core of type VBI_ENC. In this case, the value stored in the
	 * array indicates which VBI path is in use.                           */
	uint8_t hwCoreIndex[BVBI_P_EncCoreType_eLAST];

} BVBI_P_Encode_CNState;

/* The full state for encoder */
typedef struct BVBI_P_Encode_Handle
{
	/* Black magic */
	uint32_t                       ulBlackMagic;

	/* Current/next information */
	BVBI_P_Encode_CNState curr;
	BVBI_P_Encode_CNState next;

	/* Back pointer to the VBI context */
	BVBI_P_Handle *pVbi;

	/* Where to decode to */
	BAVC_VbiPath eDest;

	/* Used to temporarily disable VBI encoding */
	bool bDisabled;

	/* Double buffer storage for bulky VBI data */
	BVBI_P_TTData_Owner topTTDataO;
	BVBI_P_TTData_Owner botTTDataO;
	BVBI_LineBuilder_Handle hTopScteNrtv[2];
	BVBI_LineBuilder_Handle hBotScteNrtv[2];
	BVBI_LineBuilder_Handle hTopScteMono[2];
	BVBI_LineBuilder_Handle hBotScteMono[2];
	uint8_t* sctePamData;

	/* Cache CGMS data from user */
#ifdef P_CGMS_SOFTWARE_CRC
	uint32_t last_cgms_user;
	uint32_t last_cgms_formatted;
#endif /* P_CGMS_SOFTWARE_CRC */

	/* ARIB video? */
	bool bArib480p;

	/* Linked list membership */
	BLST_D_ENTRY(BVBI_P_Encode_Handle) link;

} BVBI_P_Encode_Handle;

typedef struct BVBI_P_Field_Handle
{
	/* Black magic */
	uint32_t                       ulBlackMagic;

	/* Pointer back to BVBI main context */
	BVBI_P_Handle* pVbi;

	/* Raw data storage */
	uint16_t             usCCData;
	uint16_t             usWSSData;
	uint32_t             ulCGMSData;
	BVBI_MCCData         *pMCCData;
	BVBI_P_TTData_Owner TTDataO;
	BVBI_VPSData         *pVPSData;
	BVBI_GSData          *pGSData;
	BVBI_AMOL_Type       amolType;
	uint8_t              *pAmolData;
	int                  amolSize;
	BVBI_P_SCTE_Data*    pPScteData;
	BVBI_CGMSB_Datum*    pCgmsbDatum;

	/* Which of the above pieces of data is VALID 
	   (bitmask of BVBI_P_SELECT_xyz) */
	uint32_t ulWhichPresent;

	/* Bitmask of error conditions from most 
	recent encode or decode operation */
	uint32_t ulErrInfo;

	/* Even/odd field parity bitmask.  May be "unspecified" or "any." */
    uint32_t polarityMask;

	/* In-use counter.  Used by BVBIlib syslib module. */
	int inUseCount;

} BVBI_P_Field_Handle;


/***************************************************************************
 * VBI private closed caption (CC) functions
 ***************************************************************************/
BERR_Code BVBI_P_CC_Init ( BVBI_P_Handle *pVbi );
void BVBI_P_CC_Enc_Init (BREG_Handle hReg, bool is656, uint8_t hwCoreIndex);
BERR_Code BVBI_P_CC_Enc_Program (
	BREG_Handle hReg,
	bool is656, 
	uint8_t hwCoreIndex,
	bool bActive,
	bool bPR18010_bad_line_number,
	BFMT_VideoFmt eVideoFormat,
	bool bArib480p);
BERR_Code BVBI_P_CC_Encode_Data_isr ( 
	BREG_Handle hReg, 
    bool is656,
	uint8_t hwCoreIndex,
	BAVC_Polarity polarity,
	uint16_t usData );
BERR_Code BVBI_P_CC_Encode_Enable_isr (
	BREG_Handle hReg,
	bool is656,
	uint8_t hwCoreIndex,
	BFMT_VideoFmt eVideoFormat,
	bool bEnable);
uint16_t BVBI_P_SetCCParityBits_isr (
	uint16_t uchData);
void BVBI_P_CC_Dec_Init (BREG_Handle hReg, uint32_t ulCoreOffset);
uint32_t BVBI_P_CC_Decode_Data_isr ( 
	BREG_Handle hReg, 
    BAVC_SourceId eSource,
	BAVC_Polarity polarity,
	uint16_t *pusData );
BERR_Code BVBI_P_CC_Dec_Program (
	BREG_Handle hReg,
	BAVC_SourceId eSource,
	bool bActive,
	BFMT_VideoFmt eVideoFormat);


/***************************************************************************
 * VBI private CGMS functions
 ***************************************************************************/
BERR_Code BVBI_P_CGMS_Init ( BVBI_P_Handle *pVbi );

void BVBI_P_CGMS_Enc_Init (BREG_Handle hReg, bool is656, uint8_t hwCoreIndex);
BERR_Code BVBI_P_CGMSA_Enc_Program (
	BREG_Handle hReg,
	bool is656, 
	uint8_t hwCoreIndex,
	bool bActive,
	BFMT_VideoFmt eVideoFormat,
	bool bArib480p,
	BAVC_HDMI_PixelRepetition ePixRep);
BERR_Code BVBI_P_CGMSA_Encode_Data_isr ( 
	BREG_Handle hReg, 
	bool is656, 
	uint8_t hwCoreIndex,
	BAVC_Polarity polarity,
	uint32_t ulData );
BERR_Code BVBI_P_CGMSA_Encode_Enable_isr (
	BREG_Handle hReg,
	bool is656, 
	uint8_t hwCoreIndex,
	BFMT_VideoFmt eVideoFormat,
	bool bEnable);
#if defined(BVBI_P_CGMSAE_VER2) || defined(BVBI_P_CGMSAE_VER3) || \
    defined(BVBI_P_CGMSAE_VER5) /** { **/
BERR_Code BVBI_P_CGMSB_Enc_Program (
	BREG_Handle hReg,
	bool is656, 
	uint8_t hwCoreIndex,
	bool bActive,
	BFMT_VideoFmt eVideoFormat,
	bool bArib480p,
	BAVC_HDMI_PixelRepetition ePixRep,
	bool bCea805dStyle);

BERR_Code BVBI_P_CGMSB_Encode_Data_isr ( 
	BREG_Handle hReg, 
	bool is656, 
	uint8_t hwCoreIndex,
	BAVC_Polarity polarity,
	BVBI_CGMSB_Datum cgmsbDatum );

BERR_Code BVBI_P_CGMSB_Encode_Enable_isr (
	BREG_Handle hReg,
	bool is656, 
	uint8_t hwCoreIndex,
	BFMT_VideoFmt eVideoFormat,
	bool bEnable);
#endif /** } **/

void BVBI_P_CGMS_Dec_Init (BREG_Handle hReg, uint32_t ulCoreOffset);

BERR_Code BVBI_P_CGMS_Dec_Program (
	BREG_Handle hReg,
	BAVC_SourceId eSource,
	bool bActive,
	BFMT_VideoFmt eVideoFormat);
uint32_t BVBI_P_CGMS_Decode_Data_isr ( 
	BREG_Handle hReg, 
    BAVC_SourceId eSource,
	BAVC_Polarity polarity,
	uint32_t *pulData );

uint32_t BVPI_P_CGMS_format_data (uint32_t userdata);


/***************************************************************************
 * VBI private WSS functions
 ***************************************************************************/
BERR_Code BVBI_P_WSS_Init ( BVBI_P_Handle *pVbi );

void BVBI_P_WSS_Enc_Init     (BREG_Handle hReg, uint8_t hwCoreIndex);
void BVBI_P_WSS_656_Enc_Init (BREG_Handle hReg, uint8_t hwCoreIndex);
BERR_Code BVBI_P_WSS_Enc_Program (
	BREG_Handle hReg,
	bool is656, 
	uint8_t hwCoreIndex,
	bool bActive,
	bool bPR18010_bad_line_number,
	BFMT_VideoFmt eVideoFormat);
BERR_Code BVBI_P_WSS_Encode_Data_isr ( 
	BREG_Handle hReg, 
	bool is656, 
	uint8_t hwCoreIndex,
	BAVC_Polarity polarity,
	uint16_t usData );
BERR_Code BVBI_P_WSS_Encode_Enable_isr (
	BREG_Handle hReg,
	bool is656, 
	uint8_t hwCoreIndex,
	BFMT_VideoFmt eVideoFormat,
	bool bEnable);

void BVBI_P_WSS_Dec_Init (BREG_Handle hReg, uint32_t ulCoreOffset);
uint16_t BVBI_P_AddWSSparity (uint16_t usData);

BERR_Code BVBI_P_WSS_Dec_Program (
	BREG_Handle hReg,
	BAVC_SourceId eSource,
	bool bActive,
	BFMT_VideoFmt eVideoFormat,
	uint16_t wssVline576i);
uint32_t BVBI_P_WSS_Decode_Data_isr ( 
	BREG_Handle hReg, 
    BAVC_SourceId eSource,
	BAVC_Polarity polarity,
	uint16_t *pusData );


/***************************************************************************
 * VBI private VPS functions
 ***************************************************************************/
BERR_Code BVBI_P_VPS_Init ( BVBI_P_Handle *pVbi );

void BVBI_P_VPS_Enc_Init (BREG_Handle hReg, uint8_t hwCoreIndex);
BERR_Code BVBI_P_VPS_Encode_Enable_isr (
	BREG_Handle hReg,
	bool is656, 
	uint8_t hwCoreIndex,
	BFMT_VideoFmt eVideoFormat,
	bool bEnable);
BERR_Code BVBI_P_VPS_Enc_Program (
	BREG_Handle hReg,
	bool is656, 
	uint8_t hwCoreIndex,
	bool bActive,
	BFMT_VideoFmt eVideoFormat);
BERR_Code BVBI_P_VPS_Encode_Data_isr ( 
	BREG_Handle hReg, 
	bool is656, 
	uint8_t hwCoreIndex,
	BAVC_Polarity polarity,
	BVBI_VPSData *pVPSData );

#ifdef BVBI_P_HAS_VPSD
void BVBI_P_VPS_Dec_Init (BREG_Handle hReg, uint32_t ulCoreOffset);
BERR_Code BVBI_P_VPS_Dec_Program (
	BREG_Handle hReg,
	BAVC_SourceId eSource,
	bool bActive,
	BFMT_VideoFmt eVideoFormat);
uint32_t BVBI_P_VPS_Decode_Data_isr ( 
	BREG_Handle hReg, 
    BAVC_SourceId eSource,
	BAVC_Polarity polarity,
	BVBI_VPSData *pVPSData );
#endif

/***************************************************************************
 * VBI private Teletext functions
 ***************************************************************************/

BERR_Code BVBI_P_TT_Init ( BVBI_P_Handle *pVbi );

void BVBI_P_TT_Enc_Init (BREG_Handle hReg, bool is656, uint8_t hwCoreIndex);
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
);
uint32_t BVBI_P_TT_Encode_Data_isr ( 
	BREG_Handle hReg, 
	BMEM_Handle hMem,
	bool is656, 
	uint8_t hwCoreIndex,
	BFMT_VideoFmt eVideoFormat,
	BAVC_Polarity polarity,
	bool bPR18010_bad_line_number,
	BVBI_P_TTData* pTTDataNext );
BERR_Code BVBI_P_TT_Encode_Enable_isr (
	BREG_Handle hReg,
	bool is656, 
	uint8_t hwCoreIndex,
	BFMT_VideoFmt eVideoFormat,
	bool bEnable);

void BVBI_P_TT_Dec_Init (BREG_Handle hReg, uint32_t ulCoreOffset);
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
	BVBI_P_TTData* botData
);
uint32_t BVBI_P_TT_Decode_Data_isr ( 
	BREG_Handle hReg, 
	BMEM_Handle hMem,
    BAVC_SourceId eSource,
	BFMT_VideoFmt eVideoFormat,
	BAVC_Polarity polarity,
	BVBI_P_TTData* pTTDataNext );

uint32_t BVBI_P_TT_Size_Storage(uint32_t ulMaxLines, uint32_t ulMaxLineSize);
BERR_Code BVBI_P_TTData_Alloc (
	BMEM_Handle hMem, uint8_t ucMaxLines, uint8_t ucLineSize, 
	BVBI_P_TTData* pTTData);

/***************************************************************************
 * VBI private Gemstar functions
 ***************************************************************************/

BERR_Code BVBI_P_GS_Init ( BVBI_P_Handle *pVbi );
void BVBI_P_GS_Dec_Init (BREG_Handle hReg, uint32_t ulCoreOffset);
void BVBI_P_GS_Enc_Init (BREG_Handle hReg, bool is656, uint8_t hwCoreIndex);

BERR_Code BVBI_P_GS_Enc_Program (
	BREG_Handle hReg,
	bool is656, 
	uint8_t hwCoreIndex,
	bool bActive,
	BFMT_VideoFmt eVideoFormat,
	bool bArib480p,
	BVBI_GSOptions* gsOptions
);
uint32_t BVBI_P_GS_Encode_Data_isr ( 
	BREG_Handle hReg, 
	bool is656, 
	uint8_t hwCoreIndex,
	BFMT_VideoFmt eVideoFormat,
	BAVC_Polarity polarity,
	BVBI_GSData* pGSDataNext );
BERR_Code BVBI_P_GS_Encode_Enable_isr (
	BREG_Handle hReg,
	bool is656, 
	uint8_t hwCoreIndex,
	BFMT_VideoFmt eVideoFormat,
	bool bEnable);
#ifdef BVBI_P_HAS_GSD
BERR_Code BVBI_P_GS_Dec_Program (
	BREG_Handle hReg,
 	BAVC_SourceId eSource,
	bool bActive,
	BFMT_VideoFmt eVideoFormat,
	BVBI_GSOptions* gsOptions);
uint32_t BVBI_P_GS_Decode_Data_isr (
	BREG_Handle hReg, 
	BAVC_SourceId eSource,
	BFMT_VideoFmt eVideoFormat,
	BAVC_Polarity polarity,
	BVBI_GSData* pGSData);
#endif

/***************************************************************************
 * VBI private AMOL functions
 ***************************************************************************/

BERR_Code BVBI_P_AMOL_Init ( BVBI_P_Handle *pVbi );
void BVBI_P_AMOL_Enc_Init (BREG_Handle hReg, bool is656, uint8_t hwCoreIndex);

BERR_Code BVBI_P_AMOL_Enc_Program (
	BREG_Handle hReg,
	bool is656,
	uint8_t hwCoreIndex,
	bool bActive,
	BFMT_VideoFmt eVideoFormat,
	bool bArib480p,
	BVBI_AMOL_Type amolType
);
uint32_t BVBI_P_AMOL_Encode_Data_isr ( 
	BREG_Handle hReg, 
	bool is656,
	uint8_t hwCoreIndex,
	BFMT_VideoFmt eVideoFormat,
	BAVC_Polarity polarity,
	BVBI_AMOL_Type amolType,
	uint8_t* pAMOLData );
BERR_Code BVBI_P_AMOL_Encode_Enable_isr (
	BREG_Handle hReg,
	bool is656,
	uint8_t hwCoreIndex,
	BFMT_VideoFmt eVideoFormat,
	bool bEnable);

/***************************************************************************
 * VBI private multi-line closed caption functions
 ***************************************************************************/

BERR_Code BVBI_P_MCC_Enc_Program (
	BREG_Handle hReg,
	bool is656,
	uint8_t hwCoreIndex,
	bool bActive,
	BFMT_VideoFmt eVideoFormat,
	bool bArib480p
);
uint32_t BVBI_P_MCC_Encode_Data_isr ( 
	BREG_Handle hReg, 
	bool is656,
	uint8_t hwCoreIndex,
	BFMT_VideoFmt eVideoFormat,
	bool bArib480p,
	BAVC_Polarity polarity,
	bool bPR18010_bad_line_number,
	BVBI_MCCData* pMCCData );
BERR_Code BVBI_P_MCC_Encode_Enable_isr (
	BREG_Handle hReg,
	bool is656,
	uint8_t hwCoreIndex,
	BFMT_VideoFmt eVideoFormat,
	bool bEnable);

/***************************************************************************
 * VBI private SCTE functions
 ***************************************************************************/

BERR_Code BVBI_P_SCTE_Init ( BVBI_P_Handle *pVbi );

void BVBI_P_SCTE_Enc_Init (BREG_Handle hReg, uint8_t hwCoreIndex);

BERR_Code BVBI_P_SCTE_Enc_Program (
	BREG_Handle hReg,
	bool is656,
	uint8_t hwCoreIndex,
	bool bActive,
	BFMT_VideoFmt eVideoFormat,
	bool bArib480p,
	BVBI_SCTE_Type scteType,
	BVBI_CSC csc,
	BVBI_CSC coCsc
);
uint32_t BVBI_P_SCTE_Encode_Data_isr ( 
	BREG_Handle hReg, 
	BMEM_Handle hMem,
	bool is656,
	uint8_t hwCoreIndex,
	BFMT_VideoFmt eVideoFormat,
	BAVC_Polarity polarity,
	BVBI_SCTE_Type scteType,
	BVBI_P_SCTE_Data* pData,
	BVBI_LineBuilder_Handle hTopScteNrtv[2],
	BVBI_LineBuilder_Handle hBotScteNrtv[2],
	BVBI_LineBuilder_Handle hTopScteMono[2],
	BVBI_LineBuilder_Handle hBotScteMono[2],
	uint8_t** pSctePamData
);
BERR_Code BVBI_P_SCTE_Encode_Enable_isr (
	BREG_Handle hReg,
	bool is656,
	uint8_t hwCoreIndex,
	BFMT_VideoFmt eVideoFormat,
	BVBI_SCTE_Type scteType,
	bool bEnable);
BERR_Code BVBI_P_SCTEData_Alloc (
	BMEM_Handle hMem, size_t cc_size, bool scteEnableNrtv, size_t pam_size, 
	bool scteEnableMono, BVBI_P_SCTE_Data* pPScteData);
BERR_Code BVBI_P_Encode_AllocScte (
	BMEM_Handle hMem,
	BVBI_LineBuilder_Handle topScteNrtv[2],
	BVBI_LineBuilder_Handle botScteNrtv[2],
	BVBI_LineBuilder_Handle topScteMono[2],
	BVBI_LineBuilder_Handle botScteMono[2],
	uint8_t** pSctePam);
void BVBI_P_Encode_FreeScte (
	BMEM_Handle hMem,
	BVBI_LineBuilder_Handle topScteNrtv[2],
	BVBI_LineBuilder_Handle botScteNrtv[2],
	BVBI_LineBuilder_Handle topScteMono[2],
	BVBI_LineBuilder_Handle botScteMono[2],
	uint8_t** pSctePam);

/***************************************************************************
 * VBI private VDTOP functions
 ***************************************************************************/
BERR_Code BVBI_P_VDTOP_Dec_Program (
	BREG_Handle hReg,
	BAVC_SourceId eSource,
	BFMT_VideoFmt eVideoFormat);

BERR_Code BVBI_P_VDTOP_Dec_Reset (
	BREG_Handle hReg,
	BAVC_SourceId eSource,
	uint32_t whichStandard);

BERR_Code BVBI_P_VDTOP_656_Dec_Program (
	BREG_Handle hReg,
	BAVC_SourceId eSource,
	BFMT_VideoFmt eVideoFormat);

/***************************************************************************
 * VBI private VBI_ENC functions
 ***************************************************************************/
BERR_Code BVBI_P_VE_Init  ( BVBI_P_Handle *pVbi );
BERR_Code BVBI_P_VE_Enc_Program (
	BREG_Handle hReg,
	bool is656, 
	uint8_t hwCoreIndex,
	uint32_t ulActive_Standards,
	uint32_t ulActive_656_Standards,
	BFMT_VideoFmt eVideoFormat);
#ifdef BVBI_P_HAS_CROSSBAR_VEC
void BVBI_P_VE_Crossbar_Program (
	BREG_Handle hReg,
	BAVC_VbiPath eDest,
	uint8_t hwCoreIndex[BVBI_P_EncCoreType_eLAST]);
#endif

#ifdef BVBI_P_HAS_XSER_TT
/***************************************************************************
 * VBI private ITU656 functions
 ***************************************************************************/
BERR_Code BVBI_P_ITU656_Init( 
	BREG_Handle hReg, const BVBI_XSER_Settings* pXSERdefaultSettings );
BERR_Code BVBI_P_ITU656_Enc_Program (
	BREG_Handle hReg,
	BVBI_XSER_Settings* pSettings,
	uint32_t ulActive_XSER_Standards);
#endif

/***************************************************************************
 * VBI private ANCI656 functions
 ***************************************************************************/
#if (BVBI_P_NUM_ANCI656_656 > 0)
BERR_Code BVBI_P_A656_Init  ( BVBI_P_Handle *pVbi );
BERR_Code BVBI_P_A656_Enc_Program (
	BREG_Handle hReg,
	uint8_t hwCoreIndex,
	BVBI_P_Encode_656_Options* h656options,
	bool bPR18010_bad_line_number,
	BFMT_VideoFmt eVideoFormat);
#endif

/***************************************************************************
 * VBI private IN656 functions
 ***************************************************************************/
BERR_Code BVBI_P_IN656_Init  ( BVBI_P_Handle *pVbi );
BERR_Code BVBI_P_IN656_Dec_Program (
	BREG_Handle hReg,
	BMEM_Handle hMem,
	BAVC_SourceId eSource,
	bool bActive,
	BVBI_656Fmt anci656Fmt,
	BVBI_P_SMPTE291Moptions* pMoptions,
	BFMT_VideoFmt eVideoFormat,
	uint8_t* topData,
	uint8_t* botData);
BERR_Code BVBI_P_IN656_Decode_Data_isr ( 
	BREG_Handle hReg, 
    BAVC_SourceId eSource,
	BAVC_Polarity polarity,
	bool* bDataFound);

/***************************************************************************
 * VBI private 656 (software parser) functions
 ***************************************************************************/
BERR_Code BVBI_P_P656_Init  ( BVBI_P_Decode_Handle* pVbi_Dec );
void      BVBI_P_P656_DeInit  ( BVBI_P_Decode_Handle* pVbi_Dec );
BERR_Code BVBI_P_P656_Process_Data_isr (
	BAVC_Polarity polarity,
	BVBI_P_Decode_Handle* pVbi_Dec,
	BVBI_P_Field_Handle* pVbi_Fld);
uint8_t BVBI_P_p656_SetEEbits (uint8_t arg);

/***************************************************************************
 * VBI private Video Encoder (top level) functions
 ***************************************************************************/
BERR_Code BVBI_P_VIE_SoftReset (
	BREG_Handle hReg,
	bool is656, 
	uint8_t hwCoreIndex,
	uint32_t whichStandard);
#if (BVBI_P_NUM_ANCI656_656 > 0)
BERR_Code BVBI_P_VIE_AncilSoftReset (
	BREG_Handle hReg,
	uint8_t hwCoreIndex);
#endif

/***************************************************************************
 * Other private, hardware dependent functions
 ***************************************************************************/

BERR_Code BVBI_P_Encode_ReserveCore (
	BAVC_VbiPath eDest, uint32_t ulSelect_Standard,
	uint8_t vecHwCoreMask[BVBI_P_EncCoreType_eLAST],
	uint8_t vecHwCoreMask_656[BVBI_P_EncCoreType_eLAST],
	uint8_t hwCoreIndex[BVBI_P_EncCoreType_eLAST]);
void BVBI_P_Encode_ReleaseCore (
	BAVC_VbiPath eDest, uint32_t ulSelect_Standard,
	uint32_t ulActive_Standards, 
	uint8_t vecHwCoreMask[BVBI_P_EncCoreType_eLAST],
	uint8_t vecHwCoreMask_656[BVBI_P_EncCoreType_eLAST],
	uint8_t hwCoreIndex[BVBI_P_EncCoreType_eLAST]);
void BVBI_P_Encode_ConnectCores (
	BREG_Handle hReg, BAVC_VbiPath eDest, 
	uint8_t hwCoreIndex[BVBI_P_EncCoreType_eLAST]);

/***************************************************************************
 * Other private functions
 ***************************************************************************/
bool BVBI_P_is656 (BAVC_VbiPath eDest);
const BVBI_XSER_Settings * BVBI_P_GetDefaultXserSettings (void);

/* 
 * Tuning parameters for retry access to VBI_ENC_PRIM_Control register, via a
 * RDMA scratch variable. These numbers express ten tries per field for 3
 * fields, if a field is 1/60 second.
 */
#define BVBI_P_MAX_HW_TRIES   30
#define BVBI_P_SLEEP_HW        2

#ifdef __cplusplus
}
#endif

#endif /* BVBI_PRIV_H__ */

/* End of file. */
