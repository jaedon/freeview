/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bmxt_4538.c $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 9/28/12 6:39p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/mxt/4538/bmxt_4538.c $
 * 
 * Hydra_Software_Devel/1   9/28/12 6:39p jtna
 * SW7425-4008: add 4538 MXT support
 * 
 ***************************************************************************/

#include "bstd.h"
#include "bmxt_4538.h"

void BMXT_4538_GetDefaultSettings(BMXT_Settings *pSettings)
{
    BMXT_GetDefaultSettings(pSettings);
    pSettings->chip = BMXT_Chip_e4538;
    pSettings->chipRev = BMXT_ChipRev_eA0;
}

