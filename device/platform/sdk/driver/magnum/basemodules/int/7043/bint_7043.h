/***************************************************************************
 *     Copyright (c) 2003-2008, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bint_7043.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 9/8/08 7:56p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/int/7043/bint_7043.h $
 * 
 * Hydra_Software_Devel/1   9/8/08 7:56p shyam
 * PR40732 : Add PCI interrupt support for 7043
 * 
 * Hydra_Software_Devel/1   2/9/07 6:08p katrep
 * PR27793: Created BINT module for 7405
 * 
 ***************************************************************************/
#ifndef BINT_7043_H
#define BINT_7043_H

#include "bint_plat.h"

#ifdef __cplusplus
extern "C" {
#endif

const BINT_Settings *BINT_7043_GetSettings( void );

#ifdef __cplusplus
}
#endif

#endif
/* End of File */

