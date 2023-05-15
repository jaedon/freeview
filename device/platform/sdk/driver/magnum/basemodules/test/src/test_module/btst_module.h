/***************************************************************************
 *     Copyright (c) 2003, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: btst_module.h $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 4/8/03 2:18p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/test/test_module/btst_module.h $
 * 
 * Hydra_Software_Devel/2   4/8/03 2:18p vsilyaev
 * Rename btst to btst_module
 * 
 * Hydra_Software_Devel/1   3/10/03 6:30p vsilyaev
 * Test case for the base modules.
 * 
 *
 ***************************************************************************/

#ifndef BTST_MODULE_H__
#define BTST_MODULE_H__

#include "bchp.h"
#include "breg_mem.h"


#ifdef __cplusplus
extern "C" {
#endif

typedef struct BTST_HandleImpl *BTST_Handle;

typedef struct {
    uint32_t dummy;
} BTST_Settings;

BERR_Code BTST_GetDefaultSettings(BTST_Settings *pDefSettings);
BERR_Code BTST_Open(BTST_Handle *phTst, BCHP_Handle hChip, BREG_Handle hRegister, const BTST_Settings *pDefSettings);
BERR_Code BTST_Reset(BTST_Handle hTst, const BTST_Settings *pDefSettings);
BERR_Code BTST_Close(BTST_Handle hTst);
BERR_Code BTST_DoTest(BTST_Handle hTst);


#ifdef __cplusplus
}
#endif

#endif  /* BTST_MODULE_H__ */
