/***************************************************************************
 *     Copyright (c) 2003-2013, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvce_platform_7445.h $
 * $brcm_Revision: Hydra_Software_Devel/4 $
 * $brcm_Date: 3/6/13 5:54p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vce/src/bvce_platform_7445.h $
 * 
 * Hydra_Software_Devel/4   3/6/13 5:54p nilesh
 * SW7425-4615: Merge refactor of platform vs core parameters
 * 
 * Hydra_Software_Devel/SW7425-4615/1   3/6/13 5:43p nilesh
 * SW7425-4615: Refactor platform vs core specific parameters
 * 
 * Hydra_Software_Devel/3   3/5/13 11:46a nilesh
 * SW7445-181: Added support for larger secure buffer size for VICE2 v2.1
 * 
 * Hydra_Software_Devel/2   2/26/13 11:12a nilesh
 * SW7435-590: Enforce platform specific ITB size alignment
 * 
 * Hydra_Software_Devel/1   12/3/12 5:41p nilesh
 * SW7445-39: FW v4.0.7.0
 * 
 * Hydra_Software_Devel/SW7445-39/1   11/16/12 10:55a nilesh
 * SW7445-39: Added support for multiple slices
 * 
 ***************************************************************************/

#ifndef BVCE_PLATFORM_7445_H_
#define BVCE_PLATFORM_7445_H_

#ifdef __cplusplus
extern "C" {
#endif

#if (BCHP_VER < BCHP_VER_B0)
/* 7445 Ax */
#define BVCE_P_CORE_MAJOR     2
#define BVCE_P_CORE_MINOR     0
#define BVCE_P_CORE_SUBMINOR  0
#define BVCE_P_CORE_REVISION  2

#define BVCE_PLATFORM_P_NUM_ENCODE_INSTANCES 2
#elif (BCHP_VER < BCHP_VER_C0)
/* 7445 Bx */
#define BVCE_P_CORE_MAJOR     2
#define BVCE_P_CORE_MINOR     0
#define BVCE_P_CORE_SUBMINOR  15
#define BVCE_P_CORE_REVISION  2

#define BVCE_PLATFORM_P_NUM_ENCODE_INSTANCES 2
#elif (BCHP_VER < BCHP_VER_D0)
/* 7445 Cx */
#define BVCE_P_CORE_MAJOR     2
#define BVCE_P_CORE_MINOR     0
#define BVCE_P_CORE_SUBMINOR  2
#define BVCE_P_CORE_REVISION  2

#define BVCE_PLATFORM_P_NUM_ENCODE_INSTANCES 2
#else
/* 7445 D0 and greater */
#define BVCE_P_CORE_MAJOR     2
#define BVCE_P_CORE_MINOR     1
#define BVCE_P_CORE_SUBMINOR  0
#define BVCE_P_CORE_REVISION  3

#define BVCE_PLATFORM_P_NUM_ENCODE_INSTANCES 3
#endif

#include "bvce_core.h"

#ifdef __cplusplus
}
#endif

#endif /* BVCE_PLATFORM_7445_H_ */
/* End of File */
