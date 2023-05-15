/***************************************************************************
 *     Copyright (c) 2005-2013, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: $
 * $brcm_Revision: $
 * $brcm_Date: $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: $
 * 
 ***************************************************************************/

#include "bstd.h"
#include "bdsq.h"
#include "bdsq_priv.h"

BDBG_MODULE(bdsq_priv);


/******************************************************************************
 BDSQ_P_ReadModifyWriteRegister()
******************************************************************************/
void BDSQ_P_ReadModifyWriteRegister(BDSQ_ChannelHandle h, uint32_t reg, uint32_t and_mask, uint32_t or_mask)
{
   uint32_t val;

   BDSQ_P_ReadRegister(h, reg, &val);
   val = (val & and_mask) | or_mask;
   BDSQ_P_WriteRegister(h, reg, val);
}


/******************************************************************************
 BDSQ_P_OrRegister()
******************************************************************************/
void BDSQ_P_OrRegister(BDSQ_ChannelHandle h, uint32_t reg, uint32_t or_mask)
{
   uint32_t val;

   BDSQ_P_ReadRegister(h, reg, &val);
   val |= or_mask;
   BDSQ_P_WriteRegister(h, reg, val);
}


/******************************************************************************
 BDSQ_P_AndRegister()
******************************************************************************/
void BDSQ_P_AndRegister(BDSQ_ChannelHandle h, uint32_t reg, uint32_t and_mask)
{
   uint32_t val;

   BDSQ_P_ReadRegister(h, reg, &val);
   val &= and_mask;
   BDSQ_P_WriteRegister(h, reg, val);
}


/******************************************************************************
 BDSQ_P_ToggleBit()
******************************************************************************/
void BDSQ_P_ToggleBit(BDSQ_ChannelHandle h, uint32_t reg, uint32_t mask)
{
   uint32_t val;
   
   BDSQ_P_ReadRegister(h, reg, &val);
      
   val |= mask;
   BDSQ_P_WriteRegister(h, reg, val);
      
   val &= ~mask;
   BDSQ_P_WriteRegister(h, reg, val);  
}

