/***************************************************************************
 *	   Copyright (c) 2003-2011, Broadcom Corporation
 *	   All Rights Reserved
 *	   Confidential Property of Broadcom Corporation
 *
 *	THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *	AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *	EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bint_35230.h $
 * $brcm_Revision: Hydra_Software_Devel/4 $
 * $brcm_Date: 3/28/11 4:52p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/int/35230/bint_35230.h $
 * 
 * Hydra_Software_Devel/4   3/28/11 4:52p jhaberf
 * SW35330-13: Updated 35233 support
 * 
 * Hydra_Software_Devel/3   2/22/11 7:20p jhaberf
 * SW35330-13: Added 35330 DTV chip support
 * 
 * Hydra_Software_Devel/2   12/1/10 8:01p jhaberf
 * SW35125-1: resolved link error for the 35125
 * 
 * Hydra_Software_Devel/1   9/10/09 6:36p jhaberf
 * SW35230-1: Creating 35230 DTV chip build environment
 *
 ***************************************************************************/
#ifndef BINT_35230_H
#define BINT_35230_H

#include "bint_plat.h"

#ifdef __cplusplus
extern "C" {
#endif

#if (BCHP_CHIP==35230)
const BINT_Settings *BINT_35230_GetSettings( void );
#elif (BCHP_CHIP==35125)
const BINT_Settings *BINT_35125_GetSettings( void );
#elif (BCHP_CHIP==35233)
const BINT_Settings *BINT_35233_GetSettings( void );
#endif

#ifdef __cplusplus
}
#endif

#endif
/* End of File */

