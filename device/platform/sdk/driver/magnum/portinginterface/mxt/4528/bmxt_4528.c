/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bmxt_4528.c $
 * $brcm_Revision: Hydra_Software_Devel/5 $
 * $brcm_Date: 9/27/12 4:59p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/mxt/4528/bmxt_4528.c $
 * 
 * Hydra_Software_Devel/5   9/27/12 4:59p jtna
 * SW7425-1483: get rid of function pointers
 * 
 * Hydra_Software_Devel/4   1/5/12 5:17p jtna
 * SW3128-84: add BMXT_ReadIntStatusRegister()
 * 
 * Hydra_Software_Devel/3   1/5/12 12:17p jtna
 * SW3128-94: include bstd.h and remove redundant bmxt.h
 * 
 * Hydra_Software_Devel/2   10/26/11 6:33p jtna
 * SW7425-1483: enforce API consistency. general clean-up.
 * 
 * Hydra_Software_Devel/1   10/21/11 7:19p jtna
 * SW7425-1483: add 4528-specific files
 * 
 ***************************************************************************/

#include "bstd.h"
#include "bmxt_4528.h"

void BMXT_4528_GetDefaultSettings(BMXT_Settings *pSettings)
{
    BMXT_GetDefaultSettings(pSettings);
    pSettings->chip = BMXT_Chip_e4528;
    pSettings->chipRev = BMXT_ChipRev_eA0;
}

