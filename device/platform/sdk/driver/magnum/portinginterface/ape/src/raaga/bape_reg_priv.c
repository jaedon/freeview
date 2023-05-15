/***************************************************************************
 *     Copyright (c) 2006-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bape_reg_priv.c $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 11/14/11 3:13p $
 *
 * Module Description: APE Register Routines
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ape/7422/bape_reg_priv.c $
 * 
 * Hydra_Software_Devel/1   11/14/11 3:13p gskerl
 * SW7429-18: Merging 7429 changes back to main branch.
 * 
 * Hydra_Software_Devel/SW7429-18/1   10/21/11 6:40p jgarrett
 * SW7429-18: Initial compileable version for 7429
 * 
 ***************************************************************************/

#include "bape_priv.h"

BDBG_OBJECT_ID(BAPE_Reg_P_FieldList);

void BAPE_Reg_P_AddMaskValueToFieldList(BAPE_Reg_P_FieldList *pFieldList, uint32_t mask, uint32_t value)
{
    BDBG_OBJECT_ASSERT(pFieldList, BAPE_Reg_P_FieldList);
    pFieldList->mask |= mask;
    pFieldList->value &= ~mask;
    pFieldList->value |= value;    
}

void BAPE_Reg_P_ApplyFieldList(BAPE_Reg_P_FieldList *pFieldList, uint32_t address)
{
    BDBG_OBJECT_ASSERT(pFieldList, BAPE_Reg_P_FieldList); 
    BAPE_Reg_P_Update(pFieldList->hApe, address, pFieldList->mask, pFieldList->value); 
    BDBG_OBJECT_DESTROY(pFieldList, BAPE_Reg_P_FieldList);
}

void BAPE_Reg_P_ApplyFieldListAtomic(BAPE_Reg_P_FieldList *pFieldList, uint32_t address)
{
    BDBG_OBJECT_ASSERT(pFieldList, BAPE_Reg_P_FieldList); 
    BAPE_Reg_P_UpdateAtomic(pFieldList->hApe, address, pFieldList->mask, pFieldList->value); 
    BDBG_OBJECT_DESTROY(pFieldList, BAPE_Reg_P_FieldList);
}

void BAPE_Reg_P_ApplyFieldList_isr(BAPE_Reg_P_FieldList *pFieldList, uint32_t address)
{
    BDBG_OBJECT_ASSERT(pFieldList, BAPE_Reg_P_FieldList); 
    BAPE_Reg_P_Update_isr(pFieldList->hApe, address, pFieldList->mask, pFieldList->value); 
    BDBG_OBJECT_DESTROY(pFieldList, BAPE_Reg_P_FieldList);
}

void BAPE_Reg_P_ApplyFieldListAtomic_isr(BAPE_Reg_P_FieldList *pFieldList, uint32_t address)
{
    BDBG_OBJECT_ASSERT(pFieldList, BAPE_Reg_P_FieldList); 
    BAPE_Reg_P_UpdateAtomic_isr(pFieldList->hApe, address, pFieldList->mask, pFieldList->value); 
    BDBG_OBJECT_DESTROY(pFieldList, BAPE_Reg_P_FieldList);
}

