
/***************************************************************************
 *     Copyright (c) 2003-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvbi_vie.c $
 * $brcm_Revision: Hydra_Software_Devel/4 $
 * $brcm_Date: 10/4/10 4:04p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vbi/7325/bvbi_vie.c $
 * 
 * Hydra_Software_Devel/4   10/4/10 4:04p darnstein
 * SW7422-46: porting work for 7422 and 7425 broke the build for older
 * chipsets.
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

 /* TODO: in 7420 and beyond, the soft reset bits are in VEC_CFG core. */

#include "bstd.h"			/* standard types */
#include "bdbg.h"			/* Dbglib */
#include "bvbi.h"			/* VBI processing, this module. */
#include "bvbi_priv.h"		/* VBI internal data structures */

#include "bchp_video_enc.h"

BDBG_MODULE(BVBI);

/* Welcome to alias central */
#if (BCHP_CHIP == 3548) || (BCHP_CHIP == 3556)
#define BCHP_VIDEO_ENC_SOFT_RESET_PRIM_VBI_PRIM_VBI_CC_RESET_MASK \
	BCHP_VIDEO_ENC_SOFT_RESET_PRIM_VBI_CC_RESET_MASK
#define BCHP_VIDEO_ENC_SOFT_RESET_PRIM_VBI_PRIM_VBI_TTX_RESET_MASK \
	BCHP_VIDEO_ENC_SOFT_RESET_PRIM_VBI_TTX_RESET_MASK
#define BCHP_VIDEO_ENC_SOFT_RESET_PRIM_VBI_PRIM_VBI_WSS_RESET_MASK \
	BCHP_VIDEO_ENC_SOFT_RESET_PRIM_VBI_WSS_RESET_MASK
#define BCHP_VIDEO_ENC_SOFT_RESET_PRIM_VBI_PRIM_VBI_CGMSAE_RESET_MASK \
	BCHP_VIDEO_ENC_SOFT_RESET_PRIM_VBI_CGMSAE_RESET_MASK
#define BCHP_VIDEO_ENC_SOFT_RESET_PRIM_VBI_PRIM_VBI_AMOL_RESET_MASK \
	BCHP_VIDEO_ENC_SOFT_RESET_PRIM_VBI_AMOL_RESET_MASK
#define BCHP_VIDEO_ENC_SOFT_RESET_PRIM_VBI_PRIM_VBI_SCTE_RESET_MASK \
	BCHP_VIDEO_ENC_SOFT_RESET_PRIM_VBI_SCTE_RESET_MASK
#define BCHP_VIDEO_ENC_SOFT_RESET_ANCIL_VBI_ANCIL_VBI_AMOL_RESET_MASK \
	BCHP_VIDEO_ENC_SOFT_RESET_ANCIL_VBI_AMOL_RESET_MASK
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
	uint32_t ulReg;
	uint32_t ulRegVal, ulRegVal0;

	BDBG_ENTER(BVBI_P_VIE_SoftReset);

	/* Figure out which encoder core to use */
	switch (hwCoreIndex)
	{
	case 0:
		if (is656)
		{
#if (BVBI_P_NUM_PTVEC >= 1)
			ulReg = BCHP_VIDEO_ENC_SOFT_RESET_PRIM_VBI;
#endif
		}
		else
		{
			ulReg = BCHP_VIDEO_ENC_SOFT_RESET_ANCIL_VBI;
		}
		break;
#if (BVBI_P_NUM_VEC >= 2)
	case 1:
		ulReg = BCHP_VIDEO_ENC_SOFT_RESET_SEC_VBI;
		break;
#endif
#if (BVBI_P_NUM_VEC >= 3)
	case 2:
		ulReg = BCHP_VIDEO_ENC_SOFT_RESET_TERT_VBI;
		break;
#endif
	default:
		/* This should never happen!  This parameter was checked by
		   BVBI_Encode_Create() */
		BDBG_LEAVE(BVBI_P_VIE_SoftReset);
		return BERR_TRACE(BERR_INVALID_PARAMETER);
		break;
	}

	/* Select register bit */
	switch (whichStandard)
	{
	case BVBI_P_SELECT_CC:
		ulRegVal = BCHP_VIDEO_ENC_SOFT_RESET_PRIM_VBI_PRIM_VBI_CC_RESET_MASK;
		break;
	case BVBI_P_SELECT_TT:
		ulRegVal = BCHP_VIDEO_ENC_SOFT_RESET_PRIM_VBI_PRIM_VBI_TTX_RESET_MASK;
		break;
	case BVBI_P_SELECT_WSS:
		ulRegVal = BCHP_VIDEO_ENC_SOFT_RESET_PRIM_VBI_PRIM_VBI_WSS_RESET_MASK;
		break;
#if (BVBI_P_NUM_GSE > 0) 
	case BVBI_P_SELECT_GS:
		ulRegVal = BCHP_VIDEO_ENC_SOFT_RESET_PRIM_VBI_PRIM_VBI_GS_RESET_MASK;
		break;
#endif
	case BVBI_P_SELECT_CGMSA:
	case BVBI_P_SELECT_CGMSB:
		/* Irrititating special case */
		#if (BVBI_P_NUM_PTVEC >= 1)
		if (is656)
		{
			ulRegVal = 0x00000000;
		}
		else
		#endif
		{
			ulRegVal = BCHP_VIDEO_ENC_SOFT_RESET_PRIM_VBI_PRIM_VBI_CGMSAE_RESET_MASK;
		}
		break;
#if (BVBI_P_NUM_AMOLE > 0)
	case BVBI_P_SELECT_AMOL:
		/* Irritating special case */
		#if (BVBI_P_NUM_AMOLE_656 > 0)
		if (is656)
			ulRegVal = BCHP_VIDEO_ENC_SOFT_RESET_ANCIL_VBI_ANCIL_VBI_AMOL_RESET_MASK;
		else
		#endif
			ulRegVal = BCHP_VIDEO_ENC_SOFT_RESET_PRIM_VBI_PRIM_VBI_AMOL_RESET_MASK;
		break;
#endif
#if (BVBI_P_NUM_SCTEE > 0)
	case BVBI_P_SELECT_SCTE:
		/* Irritating special case */
		#if (BVBI_P_NUM_PTVEC >= 1)
		if (is656)
			ulRegVal = 0x00000000;
		else
		#endif
			ulRegVal = BCHP_VIDEO_ENC_SOFT_RESET_PRIM_VBI_PRIM_VBI_SCTE_RESET_MASK;
		break;
#endif
	default:
		/* This should never happen! */
		ulRegVal = 0xFFFFFFFF;
		break;
	}

	/* Take care of errors above */
	if (ulRegVal == 0xFFFFFFFF)
	{
		BDBG_LEAVE(BVBI_P_VIE_SoftReset);
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* Finally, program the soft reset register */
	if (ulRegVal != 0x00000000)
	{
		ulRegVal0 = BREG_Read32 (hReg, ulReg);
		BREG_Write32 (hReg, ulReg, (ulRegVal0|ulRegVal));
		BREG_Write32 (hReg, ulReg, ulRegVal0);
	}

	BDBG_LEAVE(BVBI_P_VIE_SoftReset);
	return BERR_SUCCESS;
}

BERR_Code BVBI_P_VIE_AncilSoftReset (
	BREG_Handle hReg,
	uint8_t hwCoreIndex)
{
	/* TODO: generalize this. I don't know which chips are affected now. */
#if (BCHP_CHIP==7601)
	BSTD_UNUSED (hReg);
	BSTD_UNUSED (hwCoreIndex);
#else
	uint32_t ulReg;
	uint32_t ulRegVal, ulRegVal0;

	BDBG_ENTER(BVBI_P_VIE_AncilSoftReset);

	BSTD_UNUSED (hwCoreIndex);

	/* Figure out which encoder core to use */
	ulReg = BCHP_VIDEO_ENC_SOFT_RESET_ITU656;

	/* Select register bit */
	ulRegVal = BCHP_VIDEO_ENC_SOFT_RESET_ITU656_ITU656_FORMATTER_RESET_MASK;

	/* Finally, program the soft reset register */
	ulRegVal0 = BREG_Read32 (hReg, ulReg);
	BREG_Write32 (hReg, ulReg, (ulRegVal0|ulRegVal));
	BREG_Write32 (hReg, ulReg, ulRegVal0);

	BDBG_LEAVE(BVBI_P_VIE_SoftReset);
#endif
	return BERR_SUCCESS;
}

/***************************************************************************
* Static (private) functions
***************************************************************************/

/* End of file */
