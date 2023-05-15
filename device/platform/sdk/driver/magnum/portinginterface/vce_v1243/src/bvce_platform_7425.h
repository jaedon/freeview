/***************************************************************************
 *     Copyright (c) 2003-2013, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvce_platform_7425.h $
 * $brcm_Revision: Hydra_Software_Devel/3 $
 * $brcm_Date: 3/6/13 5:54p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vce/src/bvce_platform_7425.h $
 * 
 * Hydra_Software_Devel/3   3/6/13 5:54p nilesh
 * SW7425-4615: Merge refactor of platform vs core parameters
 * 
 * Hydra_Software_Devel/SW7425-4615/1   3/6/13 5:42p nilesh
 * SW7425-4615: Refactor platform vs core specific parameters
 * 
 * Hydra_Software_Devel/2   2/26/13 11:11a nilesh
 * SW7435-590: Enforce platform specific ITB size alignment
 * 
 * Hydra_Software_Devel/1   1/10/12 1:25p nilesh
 * SW7435-1: Added support for multiple instances
 *
 ***************************************************************************/

#ifndef BVCE_PLATFORM_7425_H_
#define BVCE_PLATFORM_7425_H_

#ifdef __cplusplus
extern "C" {
#endif

#if (BCHP_VER < BCHP_VER_B2)
#error Unsupported chip
#else
/* 7425 B2 or greater */
#define BVCE_P_CORE_MAJOR     0
#define BVCE_P_CORE_MINOR     1
#define BVCE_P_CORE_SUBMINOR  8
#define BVCE_P_CORE_REVISION  2

#define BVCE_PLATFORM_P_NUM_ENCODE_INSTANCES 2
#endif

#include "bvce_core.h"

#ifdef __cplusplus
}
#endif

#endif /* BVCE_PLATFORM_7425_H_ */
/* End of File */
