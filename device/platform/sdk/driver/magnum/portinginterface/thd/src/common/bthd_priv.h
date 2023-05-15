/***************************************************************************
 *     Copyright (c) 2005-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bthd_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/5 $
 * $brcm_Date: 6/16/11 9:27p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: X:/win_views/97231_test/magnum/portinginterface/thd/bthd_priv.h $
 * 
 * Hydra_Software_Devel/5   6/16/11 9:27p vishk
 * SW3461-5: Add THD support to 3461
 * 
 * Hydra_Software_Devel/4   6/2/11 12:01p atanugul
 * SW3461-5: Revert back to version 2
 * 
 * Hydra_Software_Devel/3   6/1/11 3:38p atanugul
 * SW3461-5: Add THD support to 3461
 * 
 * Hydra_Software_Devel/2   11/5/07 3:43p dliu
 * PR36603: Add file header
 * 
 * Hydra_Software_Devel/1   11/5/07 3:40p dliu
 * PR36603: Initial version
 *
 ***************************************************************************/

#ifndef BTHD_PRIV_H__
#define BTHD_PRIV_H__

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************
Summary:
   This is the structure for the BTHD_Handle. 
******************************************************************************/
typedef struct BTHD_P_Handle
{
   BTHD_Settings    settings;          /* user settings */
   void             *pImpl;            /* pointer to chip-specific structure */
   uint32_t         magicId;
} BTHD_P_Handle;



#ifdef __cplusplus
}
#endif

#endif /* BTHD_PRIV_H__ */

