/***************************************************************************
 *	   Copyright (c) 2003-2009, Broadcom Corporation
 *	   All Rights Reserved
 *	   Confidential Property of Broadcom Corporation
 *
 *	THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *	AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *	EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bint_7635.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 3/24/09 10:12p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/int/7635/bint_7635.h $
 * 
 * Hydra_Software_Devel/1   3/24/09 10:12p pntruong
 * PR52958: BINT support for 7635 "Dune" chip.
 * 
 * bdvd_v3.0/1   2/27/09 4:53p jquinlan
 * new file
 * 
 * Hydra_Software_Devel/2   7/7/08 5:43p tdo
 * PR33842: BINT support for 7635
 *
 * Hydra_Software_Devel/1   9/29/07 7:27p tdo
 * PR33842: INT module support for 7443.
 *
 ***************************************************************************/
#ifndef BINT_7635_H
#define BINT_7635_H

#include "bint_plat.h"

#ifdef __cplusplus
extern "C" {
#endif

const BINT_Settings *BINT_7635_GetSettings( void );

#ifdef __cplusplus
}
#endif

#endif
/* End of File */

