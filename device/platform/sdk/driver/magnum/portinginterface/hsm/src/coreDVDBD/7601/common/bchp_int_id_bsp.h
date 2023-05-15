/***************************************************************************
 *     Copyright (c) 1999-2008, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *
 * THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 * AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 * EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bchp_int_id_bsp.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 8/12/08 4:59p $
 *
 * $brcm_Log: /magnum/portinginterface/hsm/7601/a0/common/bchp_int_id_bsp.h $
 * 
 * Hydra_Software_Devel/1   8/12/08 4:59p atruong
 * PR45631: HSM PI support for BCM7601 platform
 * 
 * Hydra_Software_Devel/2   9/26/06 3:05p btan
 * PR 23906: Converted BCHP_REV to BCHP_VER.
 * 
 * Hydra_Software_Devel/1   2/28/06 8:21p btan
 * PR 19931: Added HSM support for 7400A0
 * 
 * Hydra_Software_Devel/1   1/20/06 5:13p btan
 * PR 19140: Do not encrypt generic mode files.
 * 
 * Hydra_Software_Devel/2   1/17/06 9:31p btan
 * PR 19140: Do not encrypt generic mode files.
 *
 *
 ***************************************************************************/
#ifndef BCHP_INT_ID_BSP_H__
#define BCHP_INT_ID_BSP_H__

#include "bchp.h"
#if (BCHP_CHIP==7401) &&  (BCHP_VER == BCHP_VER_A0)
	#include "bchp_bsp_nonsecure_intr2.h"
	#define BCHP_INT_ID_BSP_OLOAD1_INTR		BCHP_INT_ID_CREATE( BCHP_BSP_NONSECURE_INTR2_CPU_STATUS, BCHP_BSP_NONSECURE_INTR2_CPU_STATUS_OCMDQUE1_INTR_SHIFT )
	#define BCHP_INT_ID_BSP_OLOAD2_INTR		BCHP_INT_ID_CREATE( BCHP_BSP_NONSECURE_INTR2_CPU_STATUS, BCHP_BSP_NONSECURE_INTR2_CPU_STATUS_OCMDQUE2_INTR_SHIFT )

#else
	#include "bchp_bsp_control_intr2.h"
	#define BCHP_INT_ID_BSP_OLOAD1_INTR		BCHP_INT_ID_CREATE( BCHP_BSP_CONTROL_INTR2_CPU_STATUS, BCHP_BSP_CONTROL_INTR2_CPU_STATUS_OCMDQUE1_INTR_SHIFT )
	#define BCHP_INT_ID_BSP_OLOAD2_INTR		BCHP_INT_ID_CREATE( BCHP_BSP_CONTROL_INTR2_CPU_STATUS, BCHP_BSP_CONTROL_INTR2_CPU_STATUS_OCMDQUE2_INTR_SHIFT )

#endif


#endif /* #ifndef BCHP_INT_ID_BSP_H__ */

/* End of File */
