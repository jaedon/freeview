/***************************************************************************
 *	   Copyright (c) 2003-2005, Broadcom Corporation
 *	   All Rights Reserved
 *	   Confidential Property of Broadcom Corporation
 *
 *	THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *	AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *	EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bint_3560.h $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 3/15/05 8:31p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/int/3560/bint_3560.h $
 * 
 * Hydra_Software_Devel/2   3/15/05 8:31p vsilyaev
 * PR 14413:  Merged from Devel_93560 branch
 * 
 * Hydra_Software_Devel/Devel_93560_A0/1   3/3/05 3:09p vinwang
 * PR14240: New for bcm93560
 * 
 * Hydra_Software_Devel/2   9/4/03 12:50p jasonh
 * Renamed bcm3560 to 3560.
 * 
 * Hydra_Software_Devel/1   8/26/03 10:24a marcusk
 * Initial version.
 * 
 ***************************************************************************/
#ifndef BINT_3560_H
#define BINT_3560_H

#include "bint_plat.h"

#ifdef __cplusplus
extern "C" {
#endif

const BINT_Settings *BINT_3560_GetSettings( void );

#ifdef __cplusplus
}
#endif

#endif
/* End of File */

