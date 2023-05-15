/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bmxt_3383.c $
 * $brcm_Revision: Hydra_Software_Devel/6 $
 * $brcm_Date: 9/27/12 4:59p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/mxt/3383/bmxt_3383.c $
 * 
 * Hydra_Software_Devel/6   9/27/12 4:59p jtna
 * SW7425-1483: get rid of function pointers
 * 
 * Hydra_Software_Devel/5   1/25/12 4:49p jtna
 * SW7425-2267: make use of register R/W RPC and enable all APIs for 3383
 * MXT
 * 
 * Hydra_Software_Devel/4   1/5/12 5:16p jtna
 * SW3128-84: add BMXT_ReadIntStatusRegister()
 * 
 * Hydra_Software_Devel/3   1/5/12 12:17p jtna
 * SW3128-94: include bstd.h and remove redundant bmxt.h
 * 
 * Hydra_Software_Devel/2   10/26/11 6:33p jtna
 * SW7425-1483: enforce API consistency. general clean-up.
 * 
 * Hydra_Software_Devel/1   10/21/11 7:19p jtna
 * SW7425-1483: add 3383-specific files
 * 
 ***************************************************************************/

#include "bstd.h"
#include "bmxt_3383.h"

void BMXT_3383_GetDefaultSettings(BMXT_Settings *pSettings)
{
    BMXT_GetDefaultSettings(pSettings);
    pSettings->chip = BMXT_Chip_e3383;
    pSettings->chipRev = BMXT_ChipRev_eA0;
}

