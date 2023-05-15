/***************************************************************************
 *	   Copyright (c) 2003-2005, Broadcom Corporation
 *	   All Rights Reserved
 *	   Confidential Property of Broadcom Corporation
 *
 *	THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *	AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *	EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bint_7400.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 1/13/06 12:36p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/int/7400/bint_7400.h $
 * 
 * Hydra_Software_Devel/1   1/13/06 12:36p jgarrett
 * PR 19007: Updating base modules for 7400
 * 
 * Hydra_Software_Devel/Refsw_Devel_7400_A0/2   1/10/06 6:07p jgarrett
 * PR 19007: Updating BINT for BCM7400
 * 
 * Hydra_Software_Devel/1   6/27/05 3:20p gmullen
 * PR15309: Added support for XPT interrupts.
 * 
 * 
 ***************************************************************************/
#ifndef BINT_7400_H
#define BINT_7400_H

#include "bint_plat.h"

#ifdef __cplusplus
extern "C" {
#endif

const BINT_Settings *BINT_7400_GetSettings( void );

#ifdef __cplusplus
}
#endif

#endif
/* End of File */

