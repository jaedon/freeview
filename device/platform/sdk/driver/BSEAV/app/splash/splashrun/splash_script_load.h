/***************************************************************************
 *     Copyright (c) 2002-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: splash_script_load.h $
 * $brcm_Revision: 9 $
 * $brcm_Date: 10/31/12 7:03p $
 *
 * Module Description: 
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/splash/splashrun/splash_script_load.h $
 * 
 * 9   10/31/12 7:03p mward
 * SW7435-114:  Back out dual surface support.  Move to branch pending CFE
 * support.
 * 
 * 7   9/10/12 4:40p jessem
 * SW7425-3872: Removed hMem1 use with 7425.
 * 
 * 6   4/12/12 5:05p randyjew
 * SWCFE-769: Memc0 only support
 * 
 * 5   8/22/11 5:14p jessem
 * SW7425-878: Added MEMC1 support for 7425.
 * 
 * 4   11/30/09 3:12p jrubio
 * SW7335-627: fix for non-7420 chips
 * 
 * 3   9/24/09 11:47p nickh
 * SW7420-351: Modify load script to add MEMC1 for 7420 support
 * 
 * 2   6/28/07 5:06p shyam
 * PR 30741 : syncup with cfe version
 * 
 * 1   5/14/07 7:16p shyam
 * PR 30741 : Add reference support for generic portable splash
 * 
 * Hydra_Software_Devel/1   11/17/05 4:49p dkaufman
 * PR3481: Added file
 * 
 *
 ***************************************************************************/

#ifndef SPLASH_VDC_SETUP_H__
#define SPLASH_VDC_SETUP_H__

#include "splash_magnum.h"

#if BCHP_CHIP == 7420 || BCHP_CHIP==7435
void splash_bvn_init(BREG_Handle hRegister, BMEM_Handle hHeap,  BMEM_Handle hHeap1, void* ulNewAddress);
#else
void splash_bvn_init(BREG_Handle hRegister, BMEM_Handle hHeap, void* ulNewAddress);
#endif

void splash_get_surf_dimensions(uint32_t* ulWidth, uint32_t* ulHeight);
void splash_get_surf_address(void** ulAddress, uint32_t* ulPitch);
void splash_get_surf_format(BPXL_Format* pxlformat);
void splash_get_video_format(BFMT_VideoFmt* vidformat);
uint32_t splash_get_surf_alloc_size(void);


#endif
