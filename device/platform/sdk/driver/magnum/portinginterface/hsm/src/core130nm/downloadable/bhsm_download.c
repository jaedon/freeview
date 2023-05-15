/***************************************************************************
 *     Copyright (c) 2005-2007, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bhsm_download.c $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 9/18/07 1:30p $
 *
 * Module Description: This file contains Broadcom Host Secure Module (OS/platform 
 *                     independent) porting interface public functions.
 *                    			                    
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/hsm/7400/d0/downloadable/bhsm_download.c $
 * 
 * Hydra_Software_Devel/1   9/18/07 1:30p lyang
 * PR 34706: add support for 7400 D0 to portinginterface/hsm
 * 
 
 *
 *
 ***************************************************************************/
#include "bstd.h"
#include "bkni.h"



/* From Aegis */

#include "bhsm.h"
#include "bhsm_keyladder.h"



BDBG_MODULE(BHSM);

/* #define BDBG_MSG(x) printf x */

#if (BSP_DOWNLOAD_CODE==1)			/* this macro is defined in HSM makefile, bhsm.all, conditionally*/

#endif
