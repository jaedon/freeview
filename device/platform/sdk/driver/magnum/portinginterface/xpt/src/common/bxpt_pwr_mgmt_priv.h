/***************************************************************************
 *     Copyright (c) 2003-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxpt_pwr_mgmt_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 12/2/11 12:15p $
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/xpt/base2/bxpt_pwr_mgmt_priv.h $
 * 
 * Hydra_Software_Devel/1   12/2/11 12:15p jtna
 * SW7425-1863: refactor xpt standby power management
 * 
 ***************************************************************************/
#ifndef BCHP_PWR_MGMT_PRIV_H__
#define BCHP_PWR_MGMT_PRIV_H__

#if BCHP_PWR_SUPPORT
#include "bchp_pwr.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct BXPT_P_RegisterRange {
    const char* name;
    uint32_t startAddress;
    uint32_t endAddress;
} BXPT_P_RegisterRange;

unsigned BXPT_P_RegisterToMemory(BREG_Handle reg, void *pMem, const BXPT_P_RegisterRange *pRanges);
unsigned BXPT_P_MemoryToRegister(BREG_Handle reg, const void *pMem, const BXPT_P_RegisterRange *pRanges);

#ifdef BCHP_PWR_RESOURCE_XPT_SRAM
extern const BXPT_P_RegisterRange XPT_SRAM_LIST[];
extern const BXPT_P_RegisterRange XPT_REG_SAVE_LIST[];

extern const unsigned XPT_SRAM_LIST_NUM_RANGES;     /* number of ranges in XPT_SRAM_LIST[] */
extern const unsigned XPT_SRAM_LIST_TOTAL_SIZE;     /* sum of sizes (in bytes) of all ranges in XPT_SRAM_LIST[] */
extern const unsigned XPT_REG_SAVE_LIST_NUM_RANGES; /* number of ranges in XPT_REG_SAVE_LIST[] */
extern const unsigned XPT_REG_SAVE_LIST_TOTAL_SIZE; /* sum of sizes (in bytes) of all ranges in XPT_REG_SAVE_LIST[] */
#endif

#ifdef __cplusplus
}
#endif

#endif /* BCHP_PWR_MGMT_PRIV_H__ */

