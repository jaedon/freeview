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
 * $brcm_Revision: Hydra_Software_Devel/7 $
 * $brcm_Date: 8/21/12 4:48p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vbi/7405/bvbi_ve.c $
 * 
 * Hydra_Software_Devel/7   8/21/12 4:48p darnstein
 * SW7435-305: support new video formats 720x482i and 720x483p in VBI. The
 * changes are the same as some of those made in SW7435-276.
 * 
 * Hydra_Software_Devel/6   1/12/11 4:42p darnstein
 * SW3548-3123: New function BVBI_Encode_GetInterruptName().
 * 
 * Hydra_Software_Devel/5   3/18/10 12:06p darnstein
 * SW7405-3954: scratch register moves from gfx hardware to rdc hardware.
 * 
 * Hydra_Software_Devel/4   1/14/10 6:06p darnstein
 * SW7550-120: Add support for SECAM variants.
 * 
 * Hydra_Software_Devel/3   1/31/09 1:00a jrubio
 * PR51629: add 7336 support
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
#include "bchp_rdc.h"

#include "bstd.h"			/* standard types */
#include "bdbg.h"			/* Dbglib */
#include "bvbi.h"			/* VBI processing, this module. */
#include "bkni.h"			/* For critical sections */
#include "bvbi_priv.h"		/* VBI internal data structures */
#include "bchp_vbi_enc_prim.h"	/* RDB info for VBI_ENC_PRIM registers */
#if (BVBI_P_NUM_VEC >= 2)
#include "bchp_vbi_enc_sec.h"	/* RDB info for VBI_ENC_SEC registers */
#endif
#if (BVBI_P_NUM_VEC >= 3)
#include "bchp_vbi_enc_tert.h"	/* RDB info for VBI_ENC_TERT registers */
#endif
#if (BVBI_P_NUM_PTVEC >= 1)
#if (BCHP_CHIP == 7400) || (BCHP_CHIP == 7405) || \
    (BCHP_CHIP == 7325) || (BCHP_CHIP == 7335) || (BCHP_CHIP==7336)
#include "bchp_vbi_enc_656_ancil.h"	
#else
#include "bchp_vbi_enc_656.h"	
#endif
#endif
#include "bchp_int_id_video_enc_intr2.h"

/* This is a kludge, I think. */
#define P_HAS_MODULAR_VEC (1)

/* Welcome to alias central */
#if (BCHP_CHIP == 7601) && (BCHP_VER >= BCHP_VER_B0)
	#define BCHP_VBI_ENC_PRIM_Interrupt_Control \
		BCHP_VBI_ENC_PRIM_INTERRUPT_CONTROL
	#define BCHP_VBI_ENC_PRIM_Interrupt_Control_INTER1_LINE_MASK \
		BCHP_VBI_ENC_PRIM_INTERRUPT_CONTROL_INTER1_LINE_MASK
	#define BCHP_VBI_ENC_PRIM_Interrupt_Control_INTER0_LINE_MASK \
		BCHP_VBI_ENC_PRIM_INTERRUPT_CONTROL_INTER0_LINE_MASK
	#define BCHP_VBI_ENC_PRIM_Interrupt_Control_INTER1_LINE_SHIFT \
		BCHP_VBI_ENC_PRIM_INTERRUPT_CONTROL_INTER1_LINE_SHIFT
	#define BCHP_VBI_ENC_PRIM_Interrupt_Control_INTER0_LINE_SHIFT \
		BCHP_VBI_ENC_PRIM_INTERRUPT_CONTROL_INTER0_LINE_SHIFT
#endif

BDBG_MODULE(BVBI);

/* Welcome to alias central */
#if (BCHP_CHIP == 7118)
	#define BCHP_TTE_PRIM_Reset_reset_SHIFT BCHP_TTE_PRIM_Reset_reset_SHIFT
#endif
#if (BCHP_CHIP == 7400) && (BCHP_VER == BCHP_VER_A0)
	#define BCHP_VBI_ENC_PRIM_Control_ENABLE_AMOL_MASK \
		BCHP_VBI_ENC_PRIM_Control_ENABLE_AMOLE_MASK
	#define BCHP_VBI_ENC_PRIM_Control_ENABLE_AMOL_BYTE_MASK \
		BCHP_VBI_ENC_PRIM_Control_ENABLE_AMOLE_BYTE_MASK
#endif
#if (BCHP_CHIP == 7400) && (BCHP_VER >= BCHP_VER_B0)
	#define BCHP_VBI_ENC_PRIM_Control_ENABLE_AMOL_MASK \
		BCHP_VBI_ENC_PRIM_Control_ENABLE_AMOLE_MASK
	#define BCHP_VBI_ENC_656_Ancil_Control_ENABLE_AMOL_MASK \
		BCHP_VBI_ENC_656_Ancil_Control_ENABLE_AMOLE_MASK
#endif
#if (BCHP_CHIP == 7403)
	#define BCHP_VBI_ENC_PRIM_Control_ENABLE_AMOL_MASK \
		BCHP_VBI_ENC_PRIM_Control_ENABLE_AMOLE_MASK
	#define BCHP_VBI_ENC_PRIM_Control_ENABLE_AMOL_BYTE_MASK \
		BCHP_VBI_ENC_PRIM_Control_ENABLE_AMOLE_BYTE_MASK
#endif
#if (BCHP_CHIP == 7405) || (BCHP_CHIP == 7325) || (BCHP_CHIP == 7335) || (BCHP_CHIP==7336) || \
	(BCHP_CHIP == 3548) || (BCHP_CHIP == 3556) || (BCHP_CHIP == 7601)
	#define BCHP_VBI_ENC_PRIM_Control_ENABLE_AMOL_MASK \
		BCHP_VBI_ENC_PRIM_Control_ENABLE_AMOLE_MASK
	#define BCHP_VBI_ENC_PRIM_Control_ENABLE_AMOL_BYTE_MASK \
		BCHP_VBI_ENC_PRIM_Control_ENABLE_AMOLE_BYTE_MASK
	#define BCHP_VBI_ENC_656_Ancil_Control_ENABLE_AMOL_MASK \
		BCHP_VBI_ENC_656_Ancil_Control_ENABLE_AMOLE_MASK
#endif

/***************************************************************************
* Forward declarations of static (private) functions
***************************************************************************/
static void BVBI_P_VE_Enc_Init (BREG_Handle hReg, uint32_t ulCoreOffset);


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
	  (BCHP_CHIP == 7422) || (BCHP_CHIP == 7425) || \
	  (BCHP_CHIP == 7468) || (BCHP_CHIP == 7358) || (BCHP_CHIP == 7552) || \
	  (BCHP_CHIP == 7344) || (BCHP_CHIP == 7346) || \
	  (BCHP_CHIP == 7231) || (BCHP_CHIP == 7552)
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
	(BCHP_CHIP == 7422) || (BCHP_CHIP == 7425) || \
	(BCHP_CHIP == 7344) || (BCHP_CHIP == 7346) || \
	(BCHP_CHIP == 7231) ||                        \
	(BCHP_CHIP == 7468) || (BCHP_CHIP == 7358) || (BCHP_CHIP == 7552)
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
#if   (BCHP_CHIP == 7422) || (BCHP_CHIP == 7425)
		aEncodeInterruptName[0] = BCHP_INT_ID_ANCIL_VBI_0_0_INTR;
		aEncodeInterruptName[1] = BCHP_INT_ID_ANCIL_VBI_1_0_INTR;
#elif (BCHP_CHIP == 7344) || (BCHP_CHIP == 7346) || \
	  (BCHP_CHIP == 7231)
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
	uint32_t ulCoreOffset;

	BDBG_ENTER(BVBI_P_VE_Init);

	ulCoreOffset = 0x0;
	BVBI_P_VE_Enc_Init (pVbi->hReg, ulCoreOffset);
#if (BVBI_P_NUM_VEC >= 2)
	ulCoreOffset = BCHP_VBI_ENC_SEC_RevID - BCHP_VBI_ENC_PRIM_RevID;
	BVBI_P_VE_Enc_Init (pVbi->hReg, ulCoreOffset);
#endif
#if (BVBI_P_NUM_VEC >= 3)
	ulCoreOffset = BCHP_VBI_ENC_TERT_RevID - BCHP_VBI_ENC_PRIM_RevID;
	BVBI_P_VE_Enc_Init (pVbi->hReg, ulCoreOffset);
#endif
#if (BVBI_P_NUM_PTVEC >= 1)
	ulCoreOffset = BCHP_VBI_ENC_656_Ancil_RevID - BCHP_VBI_ENC_PRIM_RevID;
	BVBI_P_VE_Enc_Init (pVbi->hReg, ulCoreOffset);
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
	uint32_t ulReg;
	uint32_t topLine;
	uint32_t botLine;
	uint32_t vbiBits;
	int      rep;
	uint32_t ulScratchReg =        0x0;
	uint32_t ulOffset = 0xFFFFFFFF;

	BDBG_ENTER(BVBI_P_VE_Enc_Program);

	/* Figure out which encoder core to use */
	switch (hwCoreIndex)
	{
	case 0:
		if (is656)
		{
#if (BVBI_P_NUM_PTVEC >= 1)
			ulOffset = BCHP_VBI_ENC_656_Ancil_RevID - BCHP_VBI_ENC_PRIM_RevID;
			ulScratchReg = BAVC_VBI_ENC_BP_CTRL_SCRATCH;
#endif
		}
		else
		{
			ulOffset = 0;
			ulScratchReg = BAVC_VBI_ENC_0_CTRL_SCRATCH;
		}
		break;
#if (BVBI_P_NUM_VEC >= 2)
	case 1:
		ulOffset = BCHP_VBI_ENC_SEC_RevID - BCHP_VBI_ENC_PRIM_RevID;
		ulScratchReg = BAVC_VBI_ENC_1_CTRL_SCRATCH;
		break;
#endif
#if (BVBI_P_NUM_VEC >= 3)
	case 2:
		ulOffset = BCHP_VBI_ENC_TERT_RevID - BCHP_VBI_ENC_PRIM_RevID;
		ulScratchReg = BAVC_VBI_ENC_2_CTRL_SCRATCH;
		break;
#endif
	default:
		break;
	}
	if (ulOffset == 0xFFFFFFFF)
	{
		/* This should never happen!  This parameter was checked by
		   BVBI_Encode_Create() */
		BDBG_LEAVE(BVBI_P_VE_Enc_Program);
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* Prepare to program the interrupt control register */
	ulReg = 
		BREG_Read32 ( hReg,  BCHP_VBI_ENC_PRIM_Interrupt_Control + ulOffset );

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
		BDBG_ERR(("This video format not supported yet"));
		return BERR_TRACE (BERR_NOT_SUPPORTED);
		break;
	}

	/* Finish programming the interrupt control register */
	ulReg &= ~(
		 BCHP_MASK      (VBI_ENC_PRIM_Interrupt_Control, INTER1_LINE         ) |
		 BCHP_MASK      (VBI_ENC_PRIM_Interrupt_Control, INTER0_LINE         ) 
	);
	ulReg |= (
		 BCHP_FIELD_DATA(VBI_ENC_PRIM_Interrupt_Control, INTER1_LINE, botLine) |
		 BCHP_FIELD_DATA(VBI_ENC_PRIM_Interrupt_Control, INTER0_LINE, topLine) 
	);
	BREG_Write32 (hReg, BCHP_VBI_ENC_PRIM_Interrupt_Control + ulOffset, ulReg);

	/* Determine where VBI goes, analog and/or ITU-R 656 */
	vbiBits = 0x0;
#if (BVBI_P_NUM_PTVEC >= 1) /** { **/
	if (is656)
	{
		/* only enable these bits for the bypass path 
		   (VBI_ENC_656_Ancil_Control differs from PRIM_Control) */
		if (ulActive_656_Standards & BVBI_P_SELECT_CC)
			vbiBits |= BCHP_VBI_ENC_656_Ancil_Control_ENABLE_CC_MASK;
		if (ulActive_656_Standards & BVBI_P_SELECT_MCC)
			vbiBits |= BCHP_VBI_ENC_656_Ancil_Control_ENABLE_CC_MASK;
		if (ulActive_656_Standards & BVBI_P_SELECT_TT)
			vbiBits |= BCHP_VBI_ENC_656_Ancil_Control_ENABLE_TTX_MASK;
		if (ulActive_656_Standards & BVBI_P_SELECT_WSS)
			vbiBits |= BCHP_VBI_ENC_656_Ancil_Control_ENABLE_WSS_MASK;
		if (ulActive_656_Standards & BVBI_P_SELECT_VPS)
			vbiBits |= BCHP_VBI_ENC_656_Ancil_Control_ENABLE_WSS_MASK;
#if (BVBI_P_NUM_GSE_656 > 0) 
		if (ulActive_656_Standards & BVBI_P_SELECT_GS)
			vbiBits |= BCHP_VBI_ENC_656_Ancil_Control_ENABLE_GSE_MASK;
#endif 
#if (BVBI_P_NUM_AMOLE_656 > 0)
		if (ulActive_656_Standards & BVBI_P_SELECT_AMOL)
			vbiBits |= BCHP_VBI_ENC_656_Ancil_Control_ENABLE_AMOL_MASK;
#endif 
#if (BVBI_P_NUM_SCTEE_656 > 0) 
		if (ulActive_656_Standards & BVBI_P_SELECT_SCTE)
			vbiBits |= BCHP_VBI_ENC_656_Ancil_Control_ENABLE_SCTE_MASK;
#endif 
	}
	else
#endif /** } **/
	{
		/* only enable these bits for the non-bypass path 
		   (VBI_ENC_656_Ancil_Control differs from PRIM_Control) */
		if (ulActive_Standards & BVBI_P_SELECT_CC)
			vbiBits |= BCHP_VBI_ENC_PRIM_Control_ENABLE_CC_MASK;
		if (ulActive_Standards & BVBI_P_SELECT_MCC)
			vbiBits |= BCHP_VBI_ENC_PRIM_Control_ENABLE_CC_MASK;
		if (ulActive_Standards & BVBI_P_SELECT_CGMSA)
			vbiBits |= BCHP_VBI_ENC_PRIM_Control_ENABLE_CGMS_MASK;
		if (ulActive_Standards & BVBI_P_SELECT_CGMSB)
			vbiBits |= BCHP_VBI_ENC_PRIM_Control_ENABLE_CGMS_MASK;
		if (ulActive_Standards & BVBI_P_SELECT_TT)
			vbiBits |= BCHP_VBI_ENC_PRIM_Control_ENABLE_TTX_MASK;
		if (ulActive_Standards & BVBI_P_SELECT_WSS)
			vbiBits |= BCHP_VBI_ENC_PRIM_Control_ENABLE_WSS_MASK;
		if (ulActive_Standards & BVBI_P_SELECT_VPS)
			vbiBits |= BCHP_VBI_ENC_PRIM_Control_ENABLE_WSS_MASK;
#if (BVBI_P_NUM_GSE > 0) /** { **/
		if (ulActive_Standards & BVBI_P_SELECT_GS)
			vbiBits |= BCHP_VBI_ENC_PRIM_Control_ENABLE_GSE_MASK;
#endif /** } BVBI_P_NUM_GSE **/
#if (BVBI_P_NUM_AMOLE > 0) /** { **/
		if (ulActive_Standards & BVBI_P_SELECT_AMOL)
			vbiBits |= BCHP_VBI_ENC_PRIM_Control_ENABLE_AMOL_MASK;
#endif /** } BVBI_P_NUM_AMOLE **/
#if (BVBI_P_NUM_SCTEE > 0) /** { **/
		if (ulActive_Standards & BVBI_P_SELECT_SCTE)
			vbiBits |= BCHP_VBI_ENC_PRIM_Control_ENABLE_SCTE_MASK;
#endif /** } BVBI_P_NUM_SCTEE **/
	}
	
	/* This register is shared with VDC. We must enter a critical section
	   so that our read/modify/write operation does not get interrupted by 
	   VDC */
	BKNI_EnterCriticalSection();
	ulReg = BREG_Read32 ( hReg,  ulScratchReg );
#if (BVBI_P_NUM_PTVEC >= 1)
	if (is656)
	{
		/* clear out all Ancil_Control fields except PASS_THROUGH_COUNT */
		ulReg &= BCHP_VBI_ENC_656_Ancil_Control_PASS_THROUGH_COUNT_MASK;
	}
	else
#endif
	{
		/* clear out all PRIM_Control fields except ENABLE_PASS_THROUGH */
		ulReg &= BCHP_VBI_ENC_PRIM_Control_ENABLE_PASS_THROUGH_MASK;
	}
	ulReg |= vbiBits;
	BREG_Write32 (hReg, ulScratchReg, ulReg);
	BKNI_LeaveCriticalSection();

	/* Now poll the real hardware register until it is updated */
	for (rep = 0 ; rep < BVBI_P_MAX_HW_TRIES ; ++rep)
	{
		ulReg = 
			BREG_Read32 (hReg, BCHP_VBI_ENC_PRIM_Control + ulOffset);
		if ((ulReg & ~BCHP_VBI_ENC_PRIM_Control_ENABLE_PASS_THROUGH_MASK) ==
			vbiBits)
		{
			break;
		}
		BKNI_Sleep (BVBI_P_SLEEP_HW);
	}

	BDBG_LEAVE(BVBI_P_VE_Enc_Program);
	return BERR_SUCCESS;
}

/***************************************************************************
* Static (private) functions
***************************************************************************/

/***************************************************************************
 *
 */
static void BVBI_P_VE_Enc_Init (BREG_Handle hReg, uint32_t ulCoreOffset)
{
	uint32_t topLine;
	uint32_t botLine;
	uint32_t ulReg;

	BDBG_ENTER(BVBI_P_VE_Enc_Init);

	/* Cause interrupts to occur according to start of NTSC active video
	(default) */
	ulReg = BREG_Read32 ( hReg,  
			BCHP_VBI_ENC_PRIM_Interrupt_Control + ulCoreOffset );
	topLine = 23;
	botLine = 286;
	ulReg &= ~(
		 BCHP_MASK      (VBI_ENC_PRIM_Interrupt_Control, INTER1_LINE         ) |
		 BCHP_MASK      (VBI_ENC_PRIM_Interrupt_Control, INTER0_LINE         ) 
	);
	ulReg |= (
		 BCHP_FIELD_DATA(VBI_ENC_PRIM_Interrupt_Control, INTER1_LINE, botLine) |
		 BCHP_FIELD_DATA(VBI_ENC_PRIM_Interrupt_Control, INTER0_LINE, topLine) 
	);
    BREG_Write32 (hReg, 
		BCHP_VBI_ENC_PRIM_Interrupt_Control + ulCoreOffset, ulReg);

	BDBG_LEAVE(BVBI_P_VE_Enc_Init);
}

/* End of file */
