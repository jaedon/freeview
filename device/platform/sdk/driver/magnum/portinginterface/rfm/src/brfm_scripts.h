/***************************************************************************
 *     Copyright (c) 2005-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: brfm_scripts.h $
 * $brcm_Revision: Hydra_Software_Devel/5 $
 * $brcm_Date: 1/18/11 6:20p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/rfm/7401/brfm_scripts.h $
 * 
 * Hydra_Software_Devel/5   1/18/11 6:20p jtna
 * SW7550-660: make brfm_scripts.c more stand-alone
 * 
 * Hydra_Software_Devel/4   7/11/08 10:53a jtna
 * PR43336: Merge CRC BIST for 65nm platforms
 * 
 * Hydra_Software_Devel/PR43336/2   7/1/08 5:58p jtna
 * PR43336: Fix comment
 * 
 * Hydra_Software_Devel/PR43336/1   6/13/08 6:53p jtna
 * PR43336: Update CRC BIST
 * 
 * Hydra_Software_Devel/3   6/5/08 11:29a jtna
 * PR43336: Add built-in self-test for correcting bad startup states
 * 
 * Hydra_Software_Devel/2   9/26/07 11:18a vle
 * PR35017, PR26225: Add NICAM audio encoding mode and PAL-BG/I support
 * 
 * Hydra_Software_Devel/1   1/13/06 2:48p vle
 * PR17812, PR18396: Initial Version
 * 
 ***************************************************************************/

#ifndef BRFM_SCRIPTS_H__
#define BRFM_SCRIPTS_H__

#ifdef __cplusplus
extern "C" {
#endif

const BRFM_P_ModulationInfo *BRFM_P_GetModInfoPtr(
    BRFM_ModulationType modType /* [in] Requested modulation type */
    );

#ifdef __cplusplus
}
#endif
 
#endif
