/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bmxt_4517.c $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 9/27/12 5:00p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/mxt/4517/bmxt_4517.c $
 * 
 * Hydra_Software_Devel/2   9/27/12 5:00p jtna
 * SW7425-1483: get rid of function pointers
 * 
 * Hydra_Software_Devel/1   1/23/12 5:39p jtna
 * SW7425-2125: add 4517 MXT support
 * 
 ***************************************************************************/

#include "bstd.h"
#include "bmxt_4517.h"

void BMXT_4517_GetDefaultSettings(BMXT_Settings *pSettings)
{
    BMXT_GetDefaultSettings(pSettings);
    pSettings->chip = BMXT_Chip_e4517;
    pSettings->chipRev = BMXT_ChipRev_eA0;
}

