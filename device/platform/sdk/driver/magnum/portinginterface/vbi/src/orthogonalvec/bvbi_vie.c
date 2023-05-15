
/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvbi_vie.c $
 * $brcm_Revision: Hydra_Software_Devel/27 $
 * $brcm_Date: 7/24/12 5:21p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vbi/7445/bvbi_vie.c $
 * 
 * Hydra_Software_Devel/27   7/24/12 5:21p darnstein
 * SW7445-12: trivial port to 7445 chipset.
 * 
 * Hydra_Software_Devel/26   6/26/12 6:54p darnstein
 * SW7563-10: port BVBI to new chipset 7563.
 * 
 * Hydra_Software_Devel/25   4/25/12 6:00p darnstein
 * SW7360-16: support 7360 chipset in VBI software. I cloned the logic for
 * 7358 chipset.
 * 
 * Hydra_Software_Devel/24   4/25/12 5:12p darnstein
 * SW7584-22: support BVBI for 7584 chipset. I cloned the logic for 7231
 * chipset.
 * 
 * Hydra_Software_Devel/23   10/28/11 2:41p darnstein
 * SW7435-14: port to 7435. Same software behavior as for 7425.
 * 
 * Hydra_Software_Devel/22   9/9/11 7:12p darnstein
 * SW7429-15: trivial adaptation to 7429 chipset.
 * 
 * Hydra_Software_Devel/21   6/14/11 2:29p darnstein
 * SWDTV-7525: back out previous check-in.
 * 
 * Hydra_Software_Devel/20   6/13/11 4:30p darnstein
 * SWDTV-7525: trivially add support for 35330 chipset.
 * 
 * Hydra_Software_Devel/19   4/4/11 4:20p darnstein
 * SWBLURAY-23702: add support for 7640 chipset.
 * 
 * Hydra_Software_Devel/18   3/24/11 7:10p darnstein
 * SWDTV-6195: add references to new 35233 chipset.
 * 
 * Hydra_Software_Devel/17   11/30/10 2:28p darnstein
 * SW7231-22: support 7231 chipset in same way as 7344 and 7346.
 * 
 * Hydra_Software_Devel/16   11/23/10 1:55p darnstein
 * SW7552-15: port to 7552 chipset. Same code as for 7358.
 * 
 * Hydra_Software_Devel/15   11/11/10 5:19p darnstein
 * SW7344-8: first cut at porting BVBI to 7344.
 * 
 * Hydra_Software_Devel/14   10/12/10 6:38p darnstein
 * SW7358-16: initial port to 7358-A0.
 * 
 * Hydra_Software_Devel/13   10/1/10 2:47p darnstein
 * SW7422-46: Adapt to 7422 and 7425 chipsets.
 * 
 * Hydra_Software_Devel/10   7/15/10 7:00p darnstein
 * SW7422-46: very simple updates for 7422 compatibility.
 * 
 * Hydra_Software_Devel/9   11/24/09 4:34p darnstein
 * SW35230-16: first cut at 35230 support.
 * 
 * Hydra_Software_Devel/8   6/24/09 5:39p darnstein
 * PR56342: BVBI compiles for 7550 chipset now.
 * 
 * Hydra_Software_Devel/7   6/24/09 4:59p darnstein
 * PR56290: BVBI now compiles for 7342 chipset.
 * 
 * Hydra_Software_Devel/6   6/24/09 4:38p darnstein
 * PR56289: BVBI compiles for 7340 chipset now.
 * 
 * Hydra_Software_Devel/5   1/9/09 7:17p darnstein
 * PR45819: In the reset function, I forgot to support the AMOLE core.
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
 * Hydra_Software_Devel/7   7/17/08 8:45p darnstein
 * PR44539: compilation now possible for 7601.
 * 
 * Hydra_Software_Devel/6   6/6/08 5:36p darnstein
 * PR38956: compile in support for SCTE and AMOL in 93548.
 * 
 * Hydra_Software_Devel/5   4/28/08 7:49p darnstein
 * PR38956: CGMS-B encoding ready for bring-up. Need accurate register
 * settings for tuning.
 * 
 * Hydra_Software_Devel/4   4/2/08 7:55p darnstein
 * PR38956: VBI software compiles now.
 * 
 * Hydra_Software_Devel/3   11/16/07 11:32a darnstein
 * PR29723: Improve handling of non-existent cores in soft reset service
 * routine.
 * 
 * Hydra_Software_Devel/2   9/11/07 5:18p darnstein
 * PR25708: First release of SCTE encoder software.
 * 
 * Hydra_Software_Devel/1   12/14/06 7:15p darnstein
 * PR25990: Programming for the VBI_ENC core.
 * 
 ***************************************************************************/

#include "bstd.h"			/* standard types */
#include "bdbg.h"			/* Dbglib */
#include "bvbi.h"			/* VBI processing, this module. */
#include "bvbi_priv.h"		/* VBI internal data structures */
#include "bchp_vec_cfg.h"

BDBG_MODULE(BVBI);

/* Welcome to alias central */
#if (BCHP_CHIP == 7422) || (BCHP_CHIP == 7425) || (BCHP_CHIP == 7435) || \
	(BCHP_CHIP == 7445) || \
    (BCHP_CHIP == 7344) || (BCHP_CHIP == 7346) || (BCHP_CHIP == 7231) || \
	(BCHP_CHIP == 7584) || \
    (BCHP_CHIP == 7429) || (BCHP_CHIP == 7358) || (BCHP_CHIP == 7360) || \
	(BCHP_CHIP == 7563) || (BCHP_CHIP == 7552) || \
	(BCHP_CHIP == 7640) || (BCHP_CHIP == 35233)
	#define BCHP_VEC_CFG_SW_RESET_CCE_0               BCHP_VEC_CFG_SW_INIT_CCE_0
	#define BCHP_VEC_CFG_SW_RESET_WSE_0               BCHP_VEC_CFG_SW_INIT_WSE_0
	#define BCHP_VEC_CFG_SW_RESET_TTE_0               BCHP_VEC_CFG_SW_INIT_TTE_0
	#define BCHP_VEC_CFG_SW_RESET_GSE_0               BCHP_VEC_CFG_SW_INIT_GSE_0
	#define BCHP_VEC_CFG_SW_RESET_AMOLE_0           BCHP_VEC_CFG_SW_INIT_AMOLE_0
	#define BCHP_VEC_CFG_SW_RESET_CGMSAE_0         BCHP_VEC_CFG_SW_INIT_CGMSAE_0
#endif
#if (BCHP_CHIP == 7422) || (BCHP_CHIP == 7425) || (BCHP_CHIP == 7435) || \
	(BCHP_CHIP == 7445) || \
    (BCHP_CHIP == 7344) || (BCHP_CHIP == 7346) || (BCHP_CHIP == 7231) || \
	(BCHP_CHIP == 7429) || (BCHP_CHIP == 7584)
	#define BCHP_VEC_CFG_SW_RESET_CCE_ANCIL_0   BCHP_VEC_CFG_SW_INIT_CCE_ANCIL_0
	#define BCHP_VEC_CFG_SW_RESET_WSE_ANCIL_0   BCHP_VEC_CFG_SW_INIT_WSE_ANCIL_0
	#define BCHP_VEC_CFG_SW_RESET_TTE_ANCIL_0   BCHP_VEC_CFG_SW_INIT_TTE_ANCIL_0
	#define BCHP_VEC_CFG_SW_RESET_GSE_ANCIL_0   BCHP_VEC_CFG_SW_INIT_GSE_ANCIL_0
	#define BCHP_VEC_CFG_SW_RESET_AMOLE_ANCIL_0 \
		                                      BCHP_VEC_CFG_SW_INIT_AMOLE_ANCIL_0
	#define BCHP_VEC_CFG_SW_RESET_ANCI656_ANCIL_0 \
	                                        BCHP_VEC_CFG_SW_INIT_ANCI656_ANCIL_0
#endif

/* This will make code more legible, in special cases. Like, chipsets that do
 * not support 656 output.
 */
#if (BVBI_P_NUM_CCE_656 == 0)
#define BCHP_VEC_CFG_SW_RESET_CCE_ANCIL_0 0xFFFFFFFF
#endif
#if (BVBI_P_NUM_WSE_656 == 0)
#define BCHP_VEC_CFG_SW_RESET_WSE_ANCIL_0 0xFFFFFFFF
#endif
#if (BVBI_P_NUM_TTE_656 == 0)
#define BCHP_VEC_CFG_SW_RESET_TTE_ANCIL_0 0xFFFFFFFF
#endif
#if (BVBI_P_NUM_GSE_656 == 0)
#define BCHP_VEC_CFG_SW_RESET_GSE_ANCIL_0 0xFFFFFFFF
#endif
#if (BVBI_P_NUM_AMOLE_656 == 0)
#define BCHP_VEC_CFG_SW_RESET_AMOLE_ANCIL_0 0xFFFFFFFF
#endif

/***************************************************************************
* Forward declarations of static (private) functions
***************************************************************************/


/***************************************************************************
* Implementation of "BVBI_" API functions
***************************************************************************/


/***************************************************************************
* Implementation of supporting VBI_ENC functions that are not in API
***************************************************************************/

BERR_Code BVBI_P_VIE_SoftReset (
	BREG_Handle hReg,
	bool is656, 
	uint8_t hwCoreIndex,
	uint32_t whichStandard)
{
	uint32_t ulRegBase;
	uint32_t ulRegAddr;

	BDBG_ENTER(BVBI_P_VIE_SoftReset);

	switch (whichStandard)
	{
	case BVBI_P_SELECT_CC:
		ulRegBase = 
			(is656 ?  
				BCHP_VEC_CFG_SW_RESET_CCE_ANCIL_0 : 
				BCHP_VEC_CFG_SW_RESET_CCE_0);
		break;
#if (BVBI_P_NUM_TTE > 0) || (BVBI_P_NUM_TTE_656 > 0)
	case BVBI_P_SELECT_TT:
		ulRegBase = 
			(is656 ? 
				BCHP_VEC_CFG_SW_RESET_TTE_ANCIL_0 : 
				BCHP_VEC_CFG_SW_RESET_TTE_0);
		break;
#endif
	case BVBI_P_SELECT_WSS:
		ulRegBase = 
			(is656 ? 
				BCHP_VEC_CFG_SW_RESET_WSE_ANCIL_0 : 
				BCHP_VEC_CFG_SW_RESET_WSE_0);
		break;
#if (BVBI_P_NUM_GSE > 0)  
	case BVBI_P_SELECT_GS:
		ulRegBase = 
			(is656 ?  
				BCHP_VEC_CFG_SW_RESET_GSE_ANCIL_0 : 
				BCHP_VEC_CFG_SW_RESET_GSE_0);
		break;
#endif
#if (BVBI_P_NUM_AMOLE > 0)  
	case BVBI_P_SELECT_AMOL:
		ulRegBase = 
			(is656 ?  
				BCHP_VEC_CFG_SW_RESET_AMOLE_ANCIL_0 : 
				BCHP_VEC_CFG_SW_RESET_AMOLE_0);
		break;
#endif
	case BVBI_P_SELECT_CGMSA:
	case BVBI_P_SELECT_CGMSB:
		ulRegBase = BCHP_VEC_CFG_SW_RESET_CGMSAE_0;
		break;
#if (BVBI_P_NUM_SCTEE > 0)  
	case BVBI_P_SELECT_SCTE:
		ulRegBase = BCHP_VEC_CFG_SW_RESET_SCTE_0;
		break;
#endif
	default:
		/* This should never happen! */
		ulRegBase = 0xFFFFFFFF;
		break;
	}

	/* Take care of errors above */
	if (ulRegBase == 0xFFFFFFFF)
	{
		BDBG_LEAVE(BVBI_P_VIE_SoftReset);
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* Finally, program the soft reset register */
	ulRegAddr = ulRegBase + 4 * hwCoreIndex;
	BREG_Write32 (hReg, ulRegAddr, 0x1);
	BREG_Write32 (hReg, ulRegAddr, 0x0);

	BDBG_LEAVE(BVBI_P_VIE_SoftReset);
	return BERR_SUCCESS;
}

#if (BVBI_P_NUM_ANCI656_656 > 0)
BERR_Code BVBI_P_VIE_AncilSoftReset (
	BREG_Handle hReg,
	uint8_t hwCoreIndex)
{
	uint32_t ulRegBase;
	uint32_t ulRegAddr;

	BDBG_ENTER(BVBI_P_VIE_AncilSoftReset);

	/* Figure out which encoder core to use */
	ulRegBase = BCHP_VEC_CFG_SW_RESET_ANCI656_ANCIL_0;
	ulRegAddr = ulRegBase + 4 * hwCoreIndex;

	/* Program the soft reset register */
	BREG_Write32 (hReg, ulRegAddr, 0x1);
	BREG_Write32 (hReg, ulRegAddr, 0x0);

	BDBG_LEAVE(BVBI_P_VIE_SoftReset);
	return BERR_SUCCESS;
}
#endif

/***************************************************************************
* Static (private) functions
***************************************************************************/

/* End of file */
