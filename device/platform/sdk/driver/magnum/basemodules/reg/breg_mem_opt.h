/***************************************************************************
 *	   Copyright (c) 2003-2008, Broadcom Corporation
 *	   All Rights Reserved
 *	   Confidential Property of Broadcom Corporation
 *
 *	THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *	AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *	EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: breg_mem_opt.h $
 * $brcm_Revision: Hydra_Software_Devel/13 $
 * $brcm_Date: 9/24/08 7:32p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/reg/breg_mem_opt.h $
 * 
 * Hydra_Software_Devel/13   9/24/08 7:32p vsilyaev
 * PR 46950: Use unified declaration of BREG handle for debug and release
 * builds
 * 
 * Hydra_Software_Devel/12   7/21/06 11:27a vsilyaev
 * PR 22695: Changes to make BREG_MEM compatible between debug and release
 * builds
 * 
 * Hydra_Software_Devel/10   8/30/04 7:05p brianlee
 * PR 12172: Fixed warning
 * 
 * Hydra_Software_Devel/BCM7042/1   8/30/04 7:03p vsilyaev
 * PR 12172: Fixed warning
 * 
 * Hydra_Software_Devel/9   8/10/04 10:40a vsilyaev
 * PR 12172: Unified type of the BREG_Handle, it removes space for
 * different compiler behaviour in debug and release builds
 * 
 * Hydra_Software_Devel/8   9/19/03 8:56a marcusk
 * Added volatile to typecasts to ensure compiler does not optimize
 * register reads and writes.
 * 
 * Hydra_Software_Devel/7   9/17/03 3:32p marcusk
 * Fixed compiler warning for win32.
 * 
 * Hydra_Software_Devel/6   9/16/03 10:13a frusso
 * Fixed name mismatch in BREG_Open
 * 
 * Hydra_Software_Devel/5   9/15/03 6:08p marcusk
 * Updated to use void * as register base address.
 * 
 * Hydra_Software_Devel/4   5/30/03 12:03p marcusk
 * Changed CreateRegHandle/DestroyRegHandle to Open/Close to be more
 * consistant.
 * 
 * Hydra_Software_Devel/3   3/7/03 9:24a marcusk
 * Minor cleanup.
 * 
 * Hydra_Software_Devel/2   3/5/03 4:18p marcusk
 * Fixed minor issues (got it to compile).
 * 
 * Hydra_Software_Devel/1   3/5/03 3:33p marcusk
 * Initial version.
 *
 ***************************************************************************/
#ifndef BREG_MEM_OPT_H
#define BREG_MEM_OPT_H

#if BDBG_DEBUG_BUILD == 1
#error breg_mem_opt.h should only be included for release builds!!
#endif


#define BREG_Write32(RegHandle, reg, data) (*((volatile uint32_t *)((uintptr_t)((RegHandle)->BaseAddr)+(reg)))=data)
#define BREG_Write16(RegHandle, reg, data) (*((volatile uint16_t *)((uintptr_t)((RegHandle)->BaseAddr)+(reg)))=data)
#define BREG_Write8(RegHandle, reg, data) (*((volatile uint8_t *)((uintptr_t)((RegHandle)->BaseAddr)+(reg)))=data)

#define BREG_Read32(RegHandle, reg) (*((volatile uint32_t *)((uintptr_t)((RegHandle)->BaseAddr)+(reg))))
#define BREG_Read16(RegHandle, reg) (*((volatile uint16_t *)((uintptr_t)((RegHandle)->BaseAddr)+(reg))))
#define BREG_Read8(RegHandle, reg) (*((volatile uint8_t *)((uintptr_t)((RegHandle)->BaseAddr)+(reg))))

 
#endif
/* End of File */




