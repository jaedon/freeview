/***************************************************************************
 *     Copyright (c) 2003, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bint_7411.h $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 1/24/05 4:53p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/int/7411/bint_7411.h $
 * 
 * Hydra_Software_Devel/2   1/24/05 4:53p vsilyaev
 * PR 12333: Use new ID for the 7411 interrupt request.
 * 
 * Hydra_Software_Devel/1   8/19/04 6:36p marcusk
 * PR 12333: Initial version.
 * 
 ***************************************************************************/

#ifndef BINT_7411_H
#define BINT_7411_H

#include "bint_plat.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BINT_7411_IRQ_0	1
const BINT_Settings *BINT_7411_GetSettings( void );

#ifdef __cplusplus
}
#endif

#endif
/* End of File */

