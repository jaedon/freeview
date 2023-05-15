/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvbi_ve.c $
 * $brcm_Revision: Hydra_Software_Devel/35 $
 * $brcm_Date: 9/11/12 11:55a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vbi/src/orthogonalvec/bvbi_ve.c $
 * 
 * Hydra_Software_Devel/35   9/11/12 11:55a darnstein
 * SW7552-344: try again with Coverity conventional comments.
 * 
 * Hydra_Software_Devel/34   9/10/12 4:07p darnstein
 * SW7552-344: change some Coverity conventional comments.
 * 
 * Hydra_Software_Devel/33   8/10/12 3:32p pntruong
 * SW7435-276: Updated NTSC/480p to 720x480i/720x480p respectively.
 * Added support 482i/483p with new enums.
 *
 * Hydra_Software_Devel/32   7/24/12 5:21p darnstein
 * SW7445-12: trivial port to 7445 chipset.
 *
 * Hydra_Software_Devel/31   6/26/12 6:54p darnstein
 * SW7563-10: port BVBI to new chipset 7563.
 *
 * Hydra_Software_Devel/30   6/14/12 11:54a darnstein
 * SW7425-3226: Fix Coverity errors involving dead code.
 *
 * Hydra_Software_Devel/29   4/25/12 6:00p darnstein
 * SW7360-16: support 7360 chipset in VBI software. I cloned the logic for
 * 7358 chipset.
 *
 * Hydra_Software_Devel/28   4/25/12 5:12p darnstein
 * SW7584-22: support BVBI for 7584 chipset. I cloned the logic for 7231
 * chipset.
 *
 * Hydra_Software_Devel/27   2/20/12 2:53p darnstein
 * SW7425-2434: more detail in error messages.
 *
 * Hydra_Software_Devel/26   2/20/12 12:56p darnstein
 * SW7425-2434: when an unsupported video format is entered, the BDBG
 * error message should be informative.
 *
 * Hydra_Software_Devel/25   11/18/11 1:04p darnstein
 * SW7125-1163: last check-in was not consistent with chips that lack ITU-
 * R 656 digital video output.
 *
 * Hydra_Software_Devel/24   11/17/11 4:50p darnstein
 * SW7125-1163: disconnect unused crossbar entries every time.
 *
 * Hydra_Software_Devel/23   10/28/11 2:40p darnstein
 * SW7435-14: port to 7435. Same software behavior as for 7425.
 *
 * Hydra_Software_Devel/22   9/9/11 7:12p darnstein
 * SW7429-15: trivial adaptation to 7429 chipset.
 *
 * Hydra_Software_Devel/21   6/14/11 2:28p darnstein
 * SWDTV-7525: back out previous check-in.
 *
 * Hydra_Software_Devel/20   6/13/11 4:30p darnstein
 * SWDTV-7525: trivially add support for 35330 chipset.
 *
 * Hydra_Software_Devel/19   3/24/11 5:25p darnstein
 * SWDTV-6195: Add references to new 35233 chipset.
 *
 * Hydra_Software_Devel/18   1/12/11 6:15p darnstein
 * SW3548-3123: adjustment for 35125 and 35230 chipsets.
 *
 * Hydra_Software_Devel/17   1/12/11 4:42p darnstein
 * SW3548-3123: New function BVBI_Encode_GetInterruptName().
 *
 * Hydra_Software_Devel/16   10/1/10 2:43p darnstein
 * SW7422-46: Adapt to 7422 and 7425 chipsets.
 *
 * Hydra_Software_Devel/14   4/26/10 1:05p darnstein
 * SW7125-274: previous fix did not work.
 *
 * Hydra_Software_Devel/13   4/13/10 2:06p darnstein
 * SW7125-274: Coverity problem resolved with conventional comment. Any
 * other solution would be complex.
 *
 * Hydra_Software_Devel/12   12/21/09 7:01p darnstein
 * SW7550-120: Add support for SECAM variants.
 *
 * Hydra_Software_Devel/11   11/24/09 4:34p darnstein
 * SW35230-16: first cut at 35230 support.
 *
 * Hydra_Software_Devel/10   11/16/09 5:58p darnstein
 * SW7468-24: first step towards support of 7468 chipset.
 *
 * Hydra_Software_Devel/9   8/25/09 2:57p darnstein
 * SW7340-30: fix silly error in counting SCTEE cores.
 *
 * Hydra_Software_Devel/8   8/21/09 2:38p darnstein
 * PR47900: port to new 7125 chipset.
 *
 * Hydra_Software_Devel/7   6/24/09 5:39p darnstein
 * PR56342: BVBI compiles for 7550 chipset now.
 *
 * Hydra_Software_Devel/6   6/24/09 4:58p darnstein
 * PR56290: BVBI now compiles for 7342 chipset.
 *
 * Hydra_Software_Devel/5   6/24/09 4:38p darnstein
 * PR56289: BVBI compiles for 7340 chipset now.
 *
 * Hydra_Software_Devel/4   12/11/08 4:22p darnstein
 * PR45819: program VBI_ENC and VEC_CFG cores.
 *
 * Hydra_Software_Devel/3   12/5/08 11:21a darnstein
 * PR45819: these functions compile, but do not work properly.
 *
 * Hydra_Software_Devel/2   12/4/08 6:07p darnstein
 * PR45819: 7420 software will now compile, but not link.
 *
 * Hydra_Software_Devel/2   12/3/08 7:58p darnstein
 * PR45819: New, more modular form of most BVBI source files.
 *
 * Hydra_Software_Devel/43   7/17/08 8:45p darnstein
 * PR44539: compilation now possible for 7601.
 *
 * Hydra_Software_Devel/42   6/6/08 5:36p darnstein
 * PR38956: compile in support for SCTE and AMOL in 93548.
 *
 * Hydra_Software_Devel/41   4/28/08 7:49p darnstein
 * PR38956: CGMS-B encoding ready for bring-up. Need accurate register
 * settings for tuning.
 *
 * Hydra_Software_Devel/40   4/15/08 3:17p darnstein
 * PR41844: increase delay before VEC VBI interrupts fire. The new
 * settings might have to be refined further.
 *
 * Hydra_Software_Devel/39   12/4/07 1:20p darnstein
 * PR36897: VBI software is ready for testing with 97335 chipset.
 *
 * Hydra_Software_Devel/38   10/31/07 3:50p darnstein
 * PR34528: BVBI is ready for testing on 7325.
 *
 * Hydra_Software_Devel/37   10/10/07 8:10p darnstein
 * PR35440: Some RDB adaptations were still not correct.
 *
 * Hydra_Software_Devel/36   10/10/07 6:08p darnstein
 * PR35440: Some RDB adaptations were not correct.
 *
 * Hydra_Software_Devel/35   9/11/07 6:03p darnstein
 * PR25708: Fix ClearCase merge errors. More to come.
 *
 * Hydra_Software_Devel/34   9/11/07 5:17p darnstein
 * PR25708: First release of SCTE encoder software.
 *
 * Hydra_Software_Devel/24   10/31/06 6:19p darnstein
 * PR15780: correct a typo in initializing for AMOL encoding.
 *
 * Hydra_Software_Devel/23   9/13/06 6:34p darnstein
 * PR21688: Can now compile for 97118-A0 chipset. Correct operation is not
 * guaranteed.
 *
 * Hydra_Software_Devel/22   8/18/06 6:51p darnstein
 * PR23178: basic compile on 93563 is possible.
 *
 * Hydra_Software_Devel/33   6/26/07 11:07a darnstein
 * PR32462: Update "alias central" wankage to account for 7400-C0 RDB
 * creative writing efforts.
 *
 * Hydra_Software_Devel/32   6/6/07 12:49p darnstein
 * PR30411: multi-line closed caption encoding is ready for test.
 *
 * Hydra_Software_Devel/31   4/20/07 3:35p darnstein
 * PR29723: Compilation for 7405 chipset.
 *
 * Hydra_Software_Devel/30   3/6/07 5:32p darnstein
 * PR28456: Undefined macros prevented compilation for VxWorks on 7400-B0.
 * I don't understand why compilation worked for ANY tool chain.
 *
 * Hydra_Software_Devel/28   2/26/07 2:28p darnstein
 * PR25990: Some minor changes were required for compiling on 7400-B0.
 *
 * Hydra_Software_Devel/27   1/2/07 4:19p darnstein
 * PR26872: Mechanically add SECAM to all cases where PAL formats are
 * accepted.
 *
 * Hydra_Software_Devel/26   12/14/06 7:19p darnstein
 * PR25990: Can compile for BCM97400-B0 now.
 *
 * Hydra_Software_Devel/25   11/16/06 4:43p darnstein
 * PR25668: Mechanical changes to support the 97403 chipset.
 *
 * Hydra_Software_Devel/24   10/31/06 6:19p darnstein
 * PR15780: correct a typo in initializing for AMOL encoding.
 *
 * Hydra_Software_Devel/23   9/13/06 6:34p darnstein
 * PR21688: Can now compile for 97118-A0 chipset. Correct operation is not
 * guaranteed.
 *
 * Hydra_Software_Devel/22   8/18/06 6:51p darnstein
 * PR23178: basic compile on 93563 is possible.
 *
 * Hydra_Software_Devel/21   5/12/06 11:49a darnstein
 * PR20429: Program the bits for AMOL encoder core.
 *
 * Hydra_Software_Devel/20   3/20/06 1:08p darnstein
 * PR19719: After changing BCHP_VBI_ENC_PRIM_Control (indirectly), poll
 * register until it has really changed. This enforces the one transition
 * that is critical to this PR: when disabling a VBI encoder core, the
 * appropriate bit in BCHP_VBI_ENC_PRIM_Control must be zeroed before the
 * individual VBI core is disabled.
 *
 * Hydra_Software_Devel/19   1/18/06 2:19p darnstein
 * PR19133: Fix two problems that were preventing VPS output.
 *
 * Hydra_Software_Devel/18   10/21/05 7:11p darnstein
 * PR17732: Gemstar encoder can output a waveform, but I can't make it
 * change yet.
 *
 * Hydra_Software_Devel/17   9/23/05 2:47p darnstein
 * PR13750: Proper use of BERR_TRACE and BERR_CODEs.
 *
 * Hydra_Software_Devel/16   8/22/05 8:11p darnstein
 * PR16057: To support many different chips, use private #defines that
 * specify number of VECs, VDECs, and (separately) pass-through VECs.
 *
 * Hydra_Software_Devel/15   7/22/05 3:53p darnstein
 * PR15630, PR16218: removed chip-dependency from bavc.h
 *
 * Hydra_Software_Devel/14   7/19/05 3:06p darnstein
 * PR 15630: Use scratch register defined in bavc.h. Writing to this
 * register will cause the BVDC module to write to VBI_ENC_aaa_CONTROL
 * register at the next field trigger, using a RUL.
 *
 * Hydra_Software_Devel/13   5/26/05 3:29p jasonh
 * PR 9338: Added some more comments to describe reason behind critical
 * section.
 *
 * Hydra_Software_Devel/12   3/17/05 7:39p darnstein
 * PR 14472: Eliminate references to secondary VEC if chip is 3560.
 *
 * Hydra_Software_Devel/11   3/9/05 3:43p darnstein
 * PR 14383: Fix errors in BVBI_P_VE_Enc_Program(). Discovered by Perrier
 * at the source.
 *
 * Hydra_Software_Devel/10   7/16/04 7:07p darnstein
 * PR 9080: merge in 656 input and output work. Some testing and debugging
 * remains to be done.
 *
 * Hydra_Software_Devel/I656/2   7/15/04 3:56p darnstein
 * Access to registers BCHP_VBI_ENC_PRIM_Control and
 * BCHP_VBI_ENC_SEC_Control are now in a critical section.  This is
 * because VDC code can also access this register at initialization time.
 *
 * Hydra_Software_Devel/I656/1   6/28/04 1:09p darnstein
 * 656 output is ready for testing.
 *
 * Hydra_Software_Devel/9   6/21/04 3:14p darnstein
 * PR 9080: The interrupt for VBI encoding occurs now occurs about 10
 * lines into the active video area.  I found this to be necessary.
 * Otherwise, the teletext encoder will still be busy encoding the
 * previous field of data when the interrupt fires.
 *
 * Hydra_Software_Devel/8   5/6/04 6:16p darnstein
 * PR 9080: choose video line for interrupt, even if no VBI data is to be
 * processed. Useful for testing.
 *
 * Hydra_Software_Devel/7   4/2/04 6:42p darnstein
 * PR 9080: Allow NTSC-J video format.
 *
 * Hydra_Software_Devel/6   3/4/04 4:30p darnstein
 * PR 9080: add support for HDTV video formats.
 *
 * Hydra_Software_Devel/5   2/27/04 6:09p darnstein
 * PR 9080: handle ALL of the PAL formats.
 *
 * Hydra_Software_Devel/4   2/19/04 2:52p darnstein
 * PR 9493: Use new PAL format enums.
 *
 * Hydra_Software_Devel/3   1/20/04 1:20p darnstein
 * PR 9338: Don't fiddle with VBI_ENC_xxx_Control registers.  The VDC
 * module owns them now.
 *
 * Hydra_Software_Devel/2   1/15/04 4:31p darnstein
 * PR 9080: remove some debugging code.
 *
 * Hydra_Software_Devel/1   12/19/03 4:51p darnstein
 * PR 9080: Initial version
 *
 ***************************************************************************/

/* For bavc.h.  This is a bit of an ugly hack. */
#include "bchp_gfd_0.h"
#include "bchp_gfd_1.h"
#include "bstd.h"			/* standard types */
#include "bint.h"
#include "bdbg.h"			/* Dbglib */
#include "bvbi.h"			/* VBI processing, this module. */
#include "bkni.h"			/* For critical sections */
#include "bvbi_priv.h"		/* VBI internal data structures */
#include "bchp_vbi_enc.h"	/* RDB info for VBI_ENC registers */
#include "bchp_int_id_video_enc_intr2.h"

/* Older chips. This should not have to be modified for newer ones */
#if (BCHP_CHIP == 3548) || (BCHP_CHIP == 3556)
#else
#define P_HAS_MODULAR_VEC (1)
#endif

BDBG_MODULE(BVBI);

/* This will make code more legible, in special cases. Like, chipsets that do
 * not support 656 output.
 */
#if (BVBI_P_NUM_CCE_656 == 0)
#define BCHP_VBI_ENC_VBI_ANCIL_0_CORE_0_SEL_SEL_CCE_ANCIL_0 0xFFFFFFFF
#endif
#if (BVBI_P_NUM_WSE_656 == 0)
#define BCHP_VBI_ENC_VBI_ANCIL_0_CORE_0_SEL_SEL_WSE_ANCIL_0 0xFFFFFFFF
#endif
#if (BVBI_P_NUM_TTE_656 == 0)
#define BCHP_VBI_ENC_VBI_ANCIL_0_CORE_0_SEL_SEL_TTE_ANCIL_0 0xFFFFFFFF
#endif
#if (BVBI_P_NUM_GSE_656 == 0)
#define BCHP_VBI_ENC_VBI_ANCIL_0_CORE_0_SEL_SEL_GSE_ANCIL_0 0xFFFFFFFF
#endif
#if (BVBI_P_NUM_AMOLE_656 == 0)
#define BCHP_VBI_ENC_VBI_ANCIL_0_CORE_0_SEL_SEL_AMOLE_ANCIL_0 0xFFFFFFFF
#endif

#if (BVBI_P_NUM_CCE_656 < 2)
#define BCHP_VBI_ENC_VBI_ANCIL_0_CORE_0_SEL_SEL_CCE_ANCIL_1 0xFFFFFFFF
#endif
#if (BVBI_P_NUM_WSE_656 < 2)
#define BCHP_VBI_ENC_VBI_ANCIL_0_CORE_0_SEL_SEL_WSE_ANCIL_1 0xFFFFFFFF
#endif
#if (BVBI_P_NUM_TTE_656 < 2)
#define BCHP_VBI_ENC_VBI_ANCIL_0_CORE_0_SEL_SEL_TTE_ANCIL_1 0xFFFFFFFF
#endif
#if (BVBI_P_NUM_GSE_656 < 2)
#define BCHP_VBI_ENC_VBI_ANCIL_0_CORE_0_SEL_SEL_GSE_ANCIL_1 0xFFFFFFFF
#endif
#if (BVBI_P_NUM_AMOLE_656 < 2)
#define BCHP_VBI_ENC_VBI_ANCIL_0_CORE_0_SEL_SEL_AMOLE_ANCIL_1 0xFFFFFFFF
#endif

/***************************************************************************
* Forward declarations of static (private) functions
***************************************************************************/
static void BVBI_P_VE_Enc_Init (BREG_Handle hReg, uint32_t ulRegAddr);


/***************************************************************************
* Implementation of "BVBI_" API functions
***************************************************************************/

BERR_Code BVBI_Encode_GetInterruptName(
    BAVC_VbiPath  eVbiPath,
    BAVC_Polarity eFieldPolarity,
	BINT_Id*      pInterruptName
)
{
	int index;
	BERR_Code eErr = BERR_SUCCESS;
	BINT_Id aEncodeInterruptName[2] = {0, 0};

	BDBG_ENTER(BVBI_Encode_GetInterruptName);

	/* Check for some obvious errors */
	if(!pInterruptName)
	{
		BDBG_ERR(("Invalid parameter\n"));
		eErr = BERR_TRACE(BERR_INVALID_PARAMETER);
		goto BVBI_Encode_GetInterruptName_Done;
	}
	switch (eFieldPolarity)
	{
		case BAVC_Polarity_eFrame:
		case BAVC_Polarity_eTopField:
		case BAVC_Polarity_eBotField:
			break;
		default:
			BDBG_ERR(("Invalid parameter\n"));
			eErr = BERR_TRACE(BERR_INVALID_PARAMETER);
			goto BVBI_Encode_GetInterruptName_Done;
			break;
	}
	/* Interrupts to use according to VEC path */
	switch (eVbiPath)
	{
	case BAVC_VbiPath_eVec0:
#if   (BCHP_CHIP == 7420) || (BCHP_CHIP == 7340) || \
	  (BCHP_CHIP == 7342) || (BCHP_CHIP == 7550) || \
	  (BCHP_CHIP == 7125) || (BCHP_CHIP == 7408) || \
	  (BCHP_CHIP == 7422) || (BCHP_CHIP == 7425) || (BCHP_CHIP == 7435) || \
	  (BCHP_CHIP == 7445) || \
	  (BCHP_CHIP == 7468) || (BCHP_CHIP == 7358) || (BCHP_CHIP == 7360) || \
	  (BCHP_CHIP == 7563) || (BCHP_CHIP == 7552) || \
	  (BCHP_CHIP == 7344) || (BCHP_CHIP == 7346) || (BCHP_CHIP == 7429) || \
	  (BCHP_CHIP == 7231) || (BCHP_CHIP == 7552) || (BCHP_CHIP == 7584) || \
	  (BCHP_CHIP == 35125) || (BCHP_CHIP == 35230) || (BCHP_CHIP == 35233)
		aEncodeInterruptName[0] = BCHP_INT_ID_VBI_0_0_INTR;
		aEncodeInterruptName[1] = BCHP_INT_ID_VBI_0_1_INTR;
#else
		aEncodeInterruptName[0] = BCHP_INT_ID_PRIM_VBI_0_INTR;
		aEncodeInterruptName[1] = BCHP_INT_ID_PRIM_VBI_1_INTR;
#endif
		break;
#if (BVBI_P_NUM_VEC >= 2) /** { **/
	case BAVC_VbiPath_eVec1:
#if (BCHP_CHIP == 7420) || (BCHP_CHIP == 7340) || \
	(BCHP_CHIP == 7342) || (BCHP_CHIP == 7550) || \
	(BCHP_CHIP == 7125) || (BCHP_CHIP == 7408) || \
	(BCHP_CHIP == 7422) || (BCHP_CHIP == 7425) || (BCHP_CHIP == 7435) || \
	(BCHP_CHIP == 7445) || \
	(BCHP_CHIP == 7344) || (BCHP_CHIP == 7346) || \
	(BCHP_CHIP == 7231) || (BCHP_CHIP == 7429) || (BCHP_CHIP == 7584) || \
	(BCHP_CHIP == 7468) || (BCHP_CHIP == 7358) || (BCHP_CHIP == 7360) || \
	(BCHP_CHIP == 7563) || (BCHP_CHIP == 7552)
		aEncodeInterruptName[0] = BCHP_INT_ID_VBI_1_0_INTR;
		aEncodeInterruptName[1] = BCHP_INT_ID_VBI_1_1_INTR;
#else
		aEncodeInterruptName[0] = BCHP_INT_ID_SEC_VBI_0_INTR;
		aEncodeInterruptName[1] = BCHP_INT_ID_SEC_VBI_1_INTR;
#endif
		break;
#endif /** } (BVBI_P_NUM_VEC >= 2) **/
#if (BVBI_P_NUM_VEC >= 3) /** { **/
	case BAVC_VbiPath_eVec2:
#if (BCHP_CHIP == 7420) || (BCHP_CHIP == 7340) || \
	(BCHP_CHIP == 7342) || (BCHP_CHIP == 7550) || \
	(BCHP_CHIP == 7125) || (BCHP_CHIP == 7408) || \
	(BCHP_CHIP == 7468)
		aEncodeInterruptName[0] = BCHP_INT_ID_VBI_2_0_INTR;
		aEncodeInterruptName[1] = BCHP_INT_ID_VBI_2_1_INTR;
#else
		aEncodeInterruptName[0] = BCHP_INT_ID_TERT_VBI_0_INTR;
		aEncodeInterruptName[1] = BCHP_INT_ID_TERT_VBI_1_INTR;
#endif
		break;
#endif /** } (BVBI_P_NUM_VEC >= 3) **/
#if (BVBI_P_NUM_PTVEC > 0) /** { **/
	case BAVC_VbiPath_eBypass0:
#ifdef P_HAS_MODULAR_VEC
#if   (BCHP_CHIP == 7422) || (BCHP_CHIP == 7425) || (BCHP_CHIP == 7435)
		aEncodeInterruptName[0] = BCHP_INT_ID_ANCIL_VBI_0_0_INTR;
		aEncodeInterruptName[1] = BCHP_INT_ID_ANCIL_VBI_1_0_INTR;
#elif (BCHP_CHIP == 7344) || (BCHP_CHIP == 7346) || \
	  (BCHP_CHIP == 7231) || (BCHP_CHIP == 7429) || (BCHP_CHIP == 7584) || \
	  (BCHP_CHIP == 7445)
	    /* Weird */
		aEncodeInterruptName[0] = BCHP_INT_ID_ANCIL_VBI_0_0_INTR;
		aEncodeInterruptName[1] = BCHP_INT_ID_ANCIL_VBI_0_1_INTR;
#else
		aEncodeInterruptName[0] = BCHP_INT_ID_ANCIL_VBI_0_INTR;
		aEncodeInterruptName[1] = BCHP_INT_ID_ANCIL_VBI_1_INTR;
#endif
#else
		aEncodeInterruptName[0] = BCHP_INT_ID_BYPASS_VBI_0_INTR;
		aEncodeInterruptName[1] = BCHP_INT_ID_BYPASS_VBI_1_INTR;
#endif
		break;
#endif /** } (BVBI_P_NUM_PTVEC > 0) **/
	default:
		aEncodeInterruptName[0] = 0;
		aEncodeInterruptName[1] = 0;
		BDBG_ERR (("ERROR: failed to find VEC/VBI interrupts. %s: %d\n",
			__FILE__, __LINE__));
		eErr = BERR_TRACE(BERR_INVALID_PARAMETER);
		goto BVBI_Encode_GetInterruptName_Done;
		break;
	}

	/* Finally, apply field polarity */
	index = (eFieldPolarity == BAVC_Polarity_eBotField ? 1 : 0);
	*pInterruptName = aEncodeInterruptName[index];

BVBI_Encode_GetInterruptName_Done:
	BDBG_LEAVE(BVBI_Encode_GetInterruptName);
	return eErr;
}


/***************************************************************************
* Implementation of supporting VBI_ENC functions that are not in API
***************************************************************************/


BERR_Code BVBI_P_VE_Init( BVBI_P_Handle *pVbi )
{
	int iCore;
	uint8_t hwCoreIndex[BVBI_P_EncCoreType_eLAST];

	BDBG_ENTER(BVBI_P_VE_Init);

	for (iCore = 0 ; iCore < BVBI_P_EncCoreType_eLAST ; ++iCore)
		hwCoreIndex[iCore] = 0xFF;

	BVBI_P_VE_Enc_Init (pVbi->hReg, BCHP_VBI_ENC_VBI_0_INTR_CTRL);
	BVBI_P_VE_Crossbar_Program (pVbi->hReg, BAVC_VbiPath_eVec0, hwCoreIndex);
#if (BVBI_P_NUM_VEC >= 2)
	BVBI_P_VE_Enc_Init (pVbi->hReg, BCHP_VBI_ENC_VBI_1_INTR_CTRL);
	BVBI_P_VE_Crossbar_Program (pVbi->hReg, BAVC_VbiPath_eVec1, hwCoreIndex);
#endif
#if (BVBI_P_NUM_VEC >= 3)
	BVBI_P_VE_Enc_Init (pVbi->hReg, BCHP_VBI_ENC_VBI_2_INTR_CTRL);
	BVBI_P_VE_Crossbar_Program (pVbi->hReg, BAVC_VbiPath_eVec2, hwCoreIndex);
#endif
#if (BVBI_P_NUM_PTVEC >= 1)
	BVBI_P_VE_Enc_Init (pVbi->hReg, BCHP_VBI_ENC_VBI_ANCIL_0_INTR_CTRL);
	BVBI_P_VE_Crossbar_Program (pVbi->hReg, BAVC_VbiPath_eBypass0, hwCoreIndex);
#endif
#if (BVBI_P_NUM_PTVEC >= 2)
	BVBI_P_VE_Enc_Init (pVbi->hReg, BCHP_VBI_ENC_VBI_ANCIL_1_INTR_CTRL);
	BVBI_P_VE_Crossbar_Program (pVbi->hReg, BAVC_VbiPath_eBypass1, hwCoreIndex);
#endif

	BDBG_LEAVE(BVBI_P_VE_Init);
	return BERR_SUCCESS;
}


BERR_Code BVBI_P_VE_Enc_Program (
	BREG_Handle hReg,
	bool is656,
	uint8_t hwCoreIndex,
	uint32_t ulActive_Standards,
	uint32_t ulActive_656_Standards,
	BFMT_VideoFmt eVideoFormat)
{
	uint32_t topLine;
	uint32_t botLine;
	uint32_t ulReg;
	uint32_t ulRegAddr = 0xFFFFFFFF;

	BDBG_ENTER(BVBI_P_VE_Enc_Program);

	BSTD_UNUSED (ulActive_Standards);
	BSTD_UNUSED (ulActive_656_Standards);

	/* Figure out which core register to use */
	switch (hwCoreIndex)
	{
	case 0:
		/* coverity[dead_error_condition: FALSE] */
		if (is656)
		{
#if (BVBI_P_NUM_PTVEC >= 1)
			/* coverity[dead_error_line: FALSE] */
			ulRegAddr = BCHP_VBI_ENC_VBI_ANCIL_0_INTR_CTRL;
#endif
		}
		else
		{
			/* coverity[dead_error_line: FALSE] */
			ulRegAddr = BCHP_VBI_ENC_VBI_0_INTR_CTRL;
		}
		break;
	case 1:
		/* coverity[dead_error_condition: FALSE] */
		if (is656)
		{
#if (BVBI_P_NUM_PTVEC >= 2)
			/* coverity[dead_error_line: FALSE] */
			ulRegAddr = BCHP_VBI_ENC_VBI_ANCIL_1_INTR_CTRL;
#endif
		}
		else
		{
#if (BVBI_P_NUM_VEC >= 2)
			/* coverity[dead_error_line: FALSE] */
			ulRegAddr = BCHP_VBI_ENC_VBI_1_INTR_CTRL;
#endif
		}
		break;
	case 2:
		/* coverity[dead_error_condition: FALSE] */
		if (is656)
		{
#if (BVBI_P_NUM_PTVEC >= 3)
			/* coverity[dead_error_line: FALSE] */
			ulRegAddr = BCHP_VBI_ENC_VBI_ANCIL_2_INTR_CTRL;
#endif
		}
		else
		{
#if (BVBI_P_NUM_VEC >= 3)
			/* coverity[dead_error_line: FALSE] */
			ulRegAddr = BCHP_VBI_ENC_VBI_2_INTR_CTRL;
#endif
		}
		break;
	default:
		break;
	}
	if (ulRegAddr == 0xFFFFFFFF)
	{
		/* This should never happen!  This parameter was checked by
		   BVBI_Encode_Create() */
		BDBG_LEAVE(BVBI_P_VE_Enc_Program);
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* Prepare to program the interrupt control register */
	ulReg = BREG_Read32 ( hReg,  ulRegAddr );

	/* Select video format */
	switch (eVideoFormat)
	{
	case BFMT_VideoFmt_eNTSC:
	case BFMT_VideoFmt_eNTSC_J:
	case BFMT_VideoFmt_e720x482_NTSC:
	case BFMT_VideoFmt_e720x482_NTSC_J:
	case BFMT_VideoFmt_ePAL_M:
		/* NTSC specific settings */
		topLine = 37;
		botLine = 300;
		break;

	case BFMT_VideoFmt_e1080i:
	case BFMT_VideoFmt_e1080i_50Hz:
		topLine = 21;
		botLine = 584;
		break;

	case BFMT_VideoFmt_e720p:
	case BFMT_VideoFmt_e720p_50Hz:
		topLine = 26;
		botLine = 0;
		break;

	case BFMT_VideoFmt_e480p:
	case BFMT_VideoFmt_e720x483p:
		topLine = 46;
		botLine = 0;
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
		topLine = 47;
		botLine = 360;
		break;

	case BFMT_VideoFmt_e576p_50Hz:
		topLine = 23;
		botLine = 0;
		break;

	default:
		BDBG_LEAVE(BVBI_P_VE_Enc_Program);
		BDBG_ERR(("BVBI_VE: video format %d not supported", eVideoFormat));
		return BERR_TRACE (BERR_NOT_SUPPORTED);
		break;
	}

	/* Finish programming the interrupt control register */
	ulReg &= ~(
		 BCHP_MASK      (VBI_ENC_VBI_0_INTR_CTRL, INTR1_LINE         ) |
		 BCHP_MASK      (VBI_ENC_VBI_0_INTR_CTRL, INTR0_LINE         )
	);
	ulReg |= (
		 BCHP_FIELD_DATA(VBI_ENC_VBI_0_INTR_CTRL, INTR1_LINE, botLine) |
		 BCHP_FIELD_DATA(VBI_ENC_VBI_0_INTR_CTRL, INTR0_LINE, topLine)
	);
	BREG_Write32 (hReg, ulRegAddr, ulReg);

	BDBG_LEAVE(BVBI_P_VE_Enc_Program);
	return BERR_SUCCESS;
}


/***************************************************************************
 *
 */
void BVBI_P_VE_Crossbar_Program (
	BREG_Handle hReg,
	BAVC_VbiPath eDest,
	uint8_t hwCoreIndex[BVBI_P_EncCoreType_eLAST])
{
	bool is656;
	uint32_t ulRegMax;
	uint32_t ulRegBase;
	uint32_t ulRegAddr;
	uint32_t iReg;
	uint32_t ulBit;
	int iCore;
	uint8_t coreIndex;

	BDBG_ENTER (BVBI_P_VE_Crossbar_Program);

	switch (eDest)
	{
	#if (BVBI_P_NUM_VEC >= 1)
	case BAVC_VbiPath_eVec0:
		ulRegBase = BCHP_VBI_ENC_VBI_0_CORE_0_SEL;
		is656 = false;
		break;
	#endif
	#if (BVBI_P_NUM_VEC >= 2)
	case BAVC_VbiPath_eVec1:
		ulRegBase = BCHP_VBI_ENC_VBI_1_CORE_0_SEL;
		is656 = false;
		break;
	#endif
	#if (BVBI_P_NUM_VEC >= 3)
	case BAVC_VbiPath_eVec2:
		ulRegBase = BCHP_VBI_ENC_VBI_2_CORE_0_SEL;
		is656 = false;
		break;
	#endif
	#if (BVBI_P_NUM_PTVEC >= 1)
	case BAVC_VbiPath_eBypass0:
		ulRegBase = BCHP_VBI_ENC_VBI_ANCIL_0_CORE_0_SEL;
		is656 = true;
		break;
	#endif
	#if (BVBI_P_NUM_PTVEC >= 2)
	case BAVC_VbiPath_eBypass1:
		ulRegBase = BCHP_VBI_ENC_VBI_ANCIL_1_CORE_0_SEL;
		is656 = true;
		break;
	#endif
	default:
		ulRegBase = 0xFFFFFFFF;
		is656 = false;
		break;
	}
	BDBG_ASSERT (ulRegBase != 0xFFFFFFFF);
	/* coverity[dead_error_condition: FALSE] */
	if (is656)
	{
		/* coverity[dead_error_line: FALSE] */
		ulRegMax = BVBI_P_ENC_NUM_CROSSBAR_REG_656;
	}
	else
	{
		/* coverity[dead_error_line: FALSE] */
		ulRegMax = BVBI_P_ENC_NUM_CROSSBAR_REG;
	}

	iReg = 0;
	for (iCore = 0 ; iCore < BVBI_P_EncCoreType_eLAST ; ++iCore)
	{
		coreIndex = hwCoreIndex[iCore];
		switch (iCore)
		{
		case BVBI_P_EncCoreType_eCCE:
			switch (coreIndex)
			{
			case 0:
				/* coverity[dead_error_condition: FALSE] */
				/* coverity[dead_error_line: FALSE] */
				ulBit =
					(is656 ?
						BCHP_VBI_ENC_VBI_ANCIL_0_CORE_0_SEL_SEL_CCE_ANCIL_0 :
						BCHP_VBI_ENC_VBI_0_CORE_0_SEL_SEL_CCE_0);
				break;
#if (BVBI_P_NUM_CCE >= 2)
			case 1:
				/* coverity[dead_error_condition: FALSE] */
				/* coverity[dead_error_line: FALSE] */
				ulBit =
					(is656 ?
						BCHP_VBI_ENC_VBI_ANCIL_0_CORE_0_SEL_SEL_CCE_ANCIL_1 :
						BCHP_VBI_ENC_VBI_0_CORE_0_SEL_SEL_CCE_1);
				break;
#endif
#if (BVBI_P_NUM_CCE >= 3)
			case 2:
				/* coverity[dead_error_condition: FALSE] */
				/* coverity[dead_error_line: FALSE] */
				ulBit =
					(is656 ?
						0xFFFFFFFF : BCHP_VBI_ENC_VBI_0_CORE_0_SEL_SEL_CCE_2);
				break;
#endif
			default:
				ulBit = 0xFFFFFFFF;
				break;
			}
			break;
		case BVBI_P_EncCoreType_eCGMSAE:
			switch (coreIndex)
			{
			case 0:
				/* coverity[dead_error_condition: FALSE] */
				/* coverity[dead_error_line: FALSE] */
				ulBit =
					(is656 ?
						0xFFFFFFFF :
						BCHP_VBI_ENC_VBI_0_CORE_0_SEL_SEL_CGMSAE_0);
				break;
#if (BVBI_P_NUM_CGMSAE >= 2)
			case 1:
				/* coverity[dead_error_condition: FALSE] */
				/* coverity[dead_error_line: FALSE] */
				ulBit =
					(is656 ?
						0xFFFFFFFF :
						BCHP_VBI_ENC_VBI_0_CORE_0_SEL_SEL_CGMSAE_1);
				break;
#endif
#if (BVBI_P_NUM_CGMSAE >= 3)
			case 2:
				/* coverity[dead_error_condition: FALSE] */
				/* coverity[dead_error_line: FALSE] */
				ulBit =
					(is656 ?
						0xFFFFFFFF :
						BCHP_VBI_ENC_VBI_0_CORE_0_SEL_SEL_CGMSAE_2);
				break;
#endif
			default:
				ulBit = 0xFFFFFFFF;
				break;
			}
			break;
		case BVBI_P_EncCoreType_eWSE:
			switch (coreIndex)
			{
			case 0:
				/* coverity[dead_error_condition: FALSE] */
				/* coverity[dead_error_line: FALSE] */
				ulBit =
					(is656 ?
						BCHP_VBI_ENC_VBI_ANCIL_0_CORE_0_SEL_SEL_WSE_ANCIL_0 :
						BCHP_VBI_ENC_VBI_0_CORE_0_SEL_SEL_WSE_0);
				break;
#if (BVBI_P_NUM_WSE >= 2)
			case 1:
				/* coverity[dead_error_condition: FALSE] */
				/* coverity[dead_error_line: FALSE] */
				ulBit =
					(is656 ?
						BCHP_VBI_ENC_VBI_ANCIL_0_CORE_0_SEL_SEL_WSE_ANCIL_1 :
						BCHP_VBI_ENC_VBI_0_CORE_0_SEL_SEL_WSE_1);
				break;
#endif
#if (BVBI_P_NUM_WSE >= 3)
			case 2:
				/* coverity[dead_error_condition: FALSE] */
				/* coverity[dead_error_line: FALSE] */
				ulBit =
					(is656 ?
						0xFFFFFFFF :
						BCHP_VBI_ENC_VBI_0_CORE_0_SEL_SEL_WSE_2);
				break;
#endif
			default:
				ulBit = 0xFFFFFFFF;
				break;
			}
			break;
		case BVBI_P_EncCoreType_eTTE:
			switch (coreIndex)
			{
#if (BVBI_P_NUM_TTE >= 1)
			case 0:
				/* coverity[dead_error_condition: FALSE] */
				/* coverity[dead_error_line: FALSE] */
				ulBit =
					(is656 ?
						BCHP_VBI_ENC_VBI_ANCIL_0_CORE_0_SEL_SEL_TTE_ANCIL_0 :
						BCHP_VBI_ENC_VBI_0_CORE_0_SEL_SEL_TTE_0);
				break;
#endif
#if (BVBI_P_NUM_TTE >= 2)
			case 1:
				/* coverity[dead_error_condition: FALSE] */
				/* coverity[dead_error_line: FALSE] */
				ulBit =
					(is656 ?
						BCHP_VBI_ENC_VBI_ANCIL_0_CORE_0_SEL_SEL_TTE_ANCIL_1 :
						BCHP_VBI_ENC_VBI_0_CORE_0_SEL_SEL_TTE_1);
				break;
#endif
#if (BVBI_P_NUM_TTE >= 3)
			case 2:
				/* coverity[dead_error_condition: FALSE] */
				/* coverity[dead_error_line: FALSE] */
				ulBit =
					(is656 ?
						0xFFFFFFFF :
						BCHP_VBI_ENC_VBI_0_CORE_0_SEL_SEL_TTE_2);
				break;
#endif
			default:
				ulBit = 0xFFFFFFFF;
				break;
			}
			break;
		case BVBI_P_EncCoreType_eGSE:
			switch (coreIndex)
			{
#if (BVBI_P_NUM_GSE >= 1)
			case 0:
				/* coverity[dead_error_condition: FALSE] */
				/* coverity[dead_error_line: FALSE] */
				ulBit =
					(is656 ?
						BCHP_VBI_ENC_VBI_ANCIL_0_CORE_0_SEL_SEL_GSE_ANCIL_0 :
						BCHP_VBI_ENC_VBI_0_CORE_0_SEL_SEL_GSE_0);
				break;
#endif
#if (BVBI_P_NUM_GSE >= 2)
			case 1:
				/* coverity[dead_error_condition: FALSE] */
				/* coverity[dead_error_line: FALSE] */
				ulBit =
					(is656 ?
						BCHP_VBI_ENC_VBI_ANCIL_0_CORE_0_SEL_SEL_GSE_ANCIL_1 :
						BCHP_VBI_ENC_VBI_0_CORE_0_SEL_SEL_GSE_1);
				break;
#endif
#if (BVBI_P_NUM_GSE >= 3)
			case 2:
				/* coverity[dead_error_condition: FALSE] */
				/* coverity[dead_error_line: FALSE] */
				ulBit =
					(is656 ?
						0xFFFFFFFF :
						BCHP_VBI_ENC_VBI_0_CORE_0_SEL_SEL_GSE_2);
				break;
#endif
			default:
				ulBit = 0xFFFFFFFF;
				break;
			}
			break;
		case BVBI_P_EncCoreType_eAMOLE:
			switch (coreIndex)
			{
#if (BVBI_P_NUM_AMOLE >= 1)
			case 0:
				/* coverity[dead_error_condition: FALSE] */
				/* coverity[dead_error_line: FALSE] */
				ulBit =
					(is656 ?
						BCHP_VBI_ENC_VBI_ANCIL_0_CORE_0_SEL_SEL_AMOLE_ANCIL_0 :
						BCHP_VBI_ENC_VBI_0_CORE_0_SEL_SEL_AMOLE_0);
				break;
#endif
#if (BVBI_P_NUM_AMOLE >= 2)
			case 1:
				/* coverity[dead_error_condition: FALSE] */
				/* coverity[dead_error_line: FALSE] */
				ulBit =
					(is656 ?
						BCHP_VBI_ENC_VBI_ANCIL_0_CORE_0_SEL_SEL_AMOLE_ANCIL_1 :
						BCHP_VBI_ENC_VBI_0_CORE_0_SEL_SEL_AMOLE_1);
				break;
#endif
#if (BVBI_P_NUM_AMOLE >= 3)
			case 2:
				/* coverity[dead_error_condition: FALSE] */
				/* coverity[dead_error_line: FALSE] */
				ulBit =
					(is656 ?
						0xFFFFFFFF :
						BCHP_VBI_ENC_VBI_0_CORE_0_SEL_SEL_AMOLE_2);
				break;
#endif
			default:
				ulBit = 0xFFFFFFFF;
				break;
			}
			break;
		case BVBI_P_EncCoreType_eSCTE:
			switch (coreIndex)
			{
#if (BVBI_P_NUM_SCTEE >= 1)
			case 0:
				/* coverity[dead_error_condition: FALSE] */
				/* coverity[dead_error_line: FALSE] */
				ulBit =
					(is656 ?
						0xFFFFFFFF :
						BCHP_VBI_ENC_VBI_0_CORE_0_SEL_SEL_SCTE_0);
				break;
#endif
#if (BVBI_P_NUM_SCTEE >= 2)
			case 1:
				/* coverity[dead_error_condition: FALSE] */
				/* coverity[dead_error_line: FALSE] */
				ulBit =
					(is656 ?
						0xFFFFFFFF :
						BCHP_VBI_ENC_VBI_0_CORE_0_SEL_SEL_SCTE_1);
				break;
#endif
			default:
				ulBit = 0xFFFFFFFF;
				break;
			}
			break;
		default:
			ulBit = 0xFFFFFFFF;
			break;
		}
		if (iReg >= ulRegMax)
		{
			/* Note: this is a silent failure. TODO. */
			break;
		}
		if (ulBit != 0xFFFFFFFF)
		{
			ulRegAddr = ulRegBase + 4 * iReg;
			BREG_Write32 (hReg, ulRegAddr, ulBit);
			++iReg;
		}
	}

	/* Zero out unused crossbar entries */
#if (BVBI_P_NUM_PTVEC >= 1)
	/* coverity[dead_error_condition: FALSE] */
	/* coverity[dead_error_line: FALSE] */
	ulBit =
		(is656 ?
			BCHP_VBI_ENC_VBI_ANCIL_0_CORE_0_SEL_SEL_DISABLE :
			BCHP_VBI_ENC_VBI_0_CORE_0_SEL_SEL_DISABLE);
#else
	/* coverity[dead_error_condition: FALSE] */
	/* coverity[dead_error_line: FALSE] */
	BDBG_ASSERT (!is656);
	ulBit = BCHP_VBI_ENC_VBI_0_CORE_0_SEL_SEL_DISABLE;
#endif
	while (iReg < ulRegMax)
	{
		ulRegAddr = ulRegBase + 4 * iReg;
		BREG_Write32 (hReg, ulRegAddr, ulBit);
		++iReg;
	}

	BDBG_LEAVE (BVBI_P_VE_Crossbar_Program);
}

/***************************************************************************
* Static (private) functions
***************************************************************************/

/***************************************************************************
 *
 */
static void BVBI_P_VE_Enc_Init (BREG_Handle hReg, uint32_t ulRegAddr)
{
	uint32_t topLine;
	uint32_t botLine;
	uint32_t ulReg;

	BDBG_ENTER(BVBI_P_VE_Enc_Init);

	/* Cause interrupts to occur according to start of NTSC active video
	(default) */
	ulReg = BREG_Read32 ( hReg,  ulRegAddr );
	topLine = 23;
	botLine = 286;
	ulReg &= ~(
		 BCHP_MASK      (VBI_ENC_VBI_0_INTR_CTRL, INTR1_LINE         ) |
		 BCHP_MASK      (VBI_ENC_VBI_0_INTR_CTRL, INTR0_LINE         )
	);
	ulReg |= (
		 BCHP_FIELD_DATA(VBI_ENC_VBI_0_INTR_CTRL, INTR1_LINE, botLine) |
		 BCHP_FIELD_DATA(VBI_ENC_VBI_0_INTR_CTRL, INTR0_LINE, topLine)
	);
    BREG_Write32 (hReg, ulRegAddr, ulReg);

	BDBG_LEAVE(BVBI_P_VE_Enc_Init);
}

/* End of file */
