/***************************************************************************
 *     Copyright (c) 2005-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: btst_video_shmoo.h $
 * $brcm_Revision: Hydra_Software_Devel/4 $
 * $brcm_Date: 5/23/11 11:52a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/applications/video_shmoo_test/btst_video_shmoo.h $
 * 
 * Hydra_Software_Devel/4   5/23/11 11:52a jessem
 * SW7231-128: Added BCHP_Handle parameter to BTST_VideoShmoo() to support
 * power management.
 *
 * Hydra_Software_Devel/3   12/8/10 11:51a jessem
 * SW7422-93: Simplified public API for easy use with Nexus.
 *
 * Hydra_Software_Devel/2   12/2/10 4:04p jessem
 * SW7422-93: Modified to support 7344/7346.
 *
 * Hydra_Software_Devel/1   12/2/10 11:18a jessem
 * SW7422-93: Initial version.
 *
 ***************************************************************************/

#ifndef __BTST_VIDEO_SHMOO_H_
#define __BTST_VIDEO_SHMOO_H_

BERR_Code BTST_VideoShmoo
	( const BMEM_Heap_Handle           hMemory,
	  const BCHP_Handle                hChip,
	  const BREG_Handle                hRegister );

#endif  /* __BTST_VIDEO_SHMOO_H_ */

