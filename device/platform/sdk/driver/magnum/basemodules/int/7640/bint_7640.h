/***************************************************************************
 *	   Copyright (c) 2003-2010, Broadcom Corporation
 *	   All Rights Reserved
 *	   Confidential Property of Broadcom Corporation
 *
 *	THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *	AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *	EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bint_7640.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 12/3/10 12:59p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/int/7640/bint_7640.h $
 * 
 * Hydra_Software_Devel/1   12/3/10 12:59p etrudeau
 * SWBLURAY-23579: add 7640 chip
 * 
 * Hydra_Software_Devel/1   12/3/10 12:51p etrudeau
 * SWBLURAY-23579: merge 7630 int files to Hydra
 * 
 * bdvd_v3.0/1   8/28/09 11:51a jquinlan
 * new file.
 * 
 *
 ***************************************************************************/
#ifndef BINT_7640_H
#define BINT_7640_H

#include "bint_plat.h"

#ifdef __cplusplus
extern "C" {
#endif

const BINT_Settings *BINT_7640_GetSettings( void );

#ifdef __cplusplus
}
#endif

#endif
/* End of File */
