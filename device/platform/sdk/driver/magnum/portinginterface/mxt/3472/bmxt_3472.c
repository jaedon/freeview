/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bmxt_3472.c $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 9/27/12 5:00p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/mxt/3472/bmxt_3472.c $
 * 
 * Hydra_Software_Devel/2   9/27/12 5:00p jtna
 * SW7425-1483: get rid of function pointers
 * 
 * Hydra_Software_Devel/1   7/19/12 4:22p jtna
 * SW3472-14: add 3472 MXT support
 * 
 ***************************************************************************/

#include "bstd.h"
#include "bmxt_3472.h"

void BMXT_3472_GetDefaultSettings(BMXT_Settings *pSettings)
{
    BMXT_GetDefaultSettings(pSettings);
    pSettings->chip = BMXT_Chip_e3472;
    pSettings->chipRev = BMXT_ChipRev_eA0;
}

